/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#شامل "curl_setup.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "protocol.h"
#شامل "proxy.h"
#شامل "http_proxy.h"
#شامل "strcase.h"
#شامل "url.h"
#شامل "vauth/vauth.h"
#شامل "curlx/inet_pton.h"
#شامل "curlx/strparse.h"

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_مطابقت(ثابت حرف *ipv4,    /* 1.2.3.4 address */
                          ثابت حرف *جال, /* 1.2.3.4 address */
                          صحیح بٹ);
UNITTEST bool cidr4_مطابقت(ثابت حرف *ipv4,    /* 1.2.3.4 address */
                          ثابت حرف *جال, /* 1.2.3.4 address */
                          صحیح بٹ)
{
  غیرعلامتی صحیح پتہ = 0;
  غیرعلامتی صحیح check = 0;

  اگر(بٹ > 32)
    /* strange input */
    واپس FALSE;

  اگر(curlx_inet_pton(AF_INET, ipv4, &پتہ) != 1)
    واپس FALSE;
  اگر(curlx_inet_pton(AF_INET, جال, &check) != 1)
    واپس FALSE;

  اگر((بٹ > 0) && (بٹ != 32)) {
    غیرعلامتی صحیح نقاب = 0xffffffff << (32 - بٹ);
    غیرعلامتی صحیح haddr = htonl(پتہ);
    غیرعلامتی صحیح hcheck = htonl(check);
#اگر 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, جال, hcheck, بٹ, نقاب,
                  (haddr ^ hcheck) & نقاب);
#اگر_ختم
    اگر((haddr ^ hcheck) & نقاب)
      واپس FALSE;
    واپس TRUE;
  }
  ورنہ اگر(!بٹ && !check)
    /* 0.0.0.0/0 matches everything */
    واپس TRUE;

  واپس پتہ == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_مطابقت(ثابت حرف *ipv6, ثابت حرف *جال,
                          صحیح بٹ);
UNITTEST bool cidr6_مطابقت(ثابت حرف *ipv6, ثابت حرف *جال,
                          صحیح بٹ)
{
#اگر_تعریف_شدہ USE_IPV6
  ساکن ثابت حرف null6[16]= { 0 };
  غیرعلامتی صحیح بائٹیں;
  غیرعلامتی صحیح rest;
  غیرعلامتی حرف پتہ[16];
  غیرعلامتی حرف check[16];

  اگر(بٹ == -1)
    بٹ = 128;

  بائٹیں = بٹ / 8;
  rest = بٹ & 0x07;
  اگر((بائٹیں > 16) || ((بائٹیں == 16) && rest))
    واپس FALSE;
  اگر(curlx_inet_pton(AF_INET6, ipv6, پتہ) != 1)
    واپس FALSE;
  اگر(curlx_inet_pton(AF_INET6, جال, check) != 1)
    واپس FALSE;
  اگر((بائٹیں == 16) && !بٹ && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    واپس TRUE;
  اگر(بائٹیں && memcmp(پتہ, check, بائٹیں))
    واپس FALSE;
  اگر(rest && ((پتہ[بائٹیں] ^ check[بائٹیں]) & (0xff << (8 - rest))))
    واپس FALSE;

  واپس TRUE;
#ورنہ
  (خالی)ipv6;
  (خالی)جال;
  (خالی)بٹ;
  واپس FALSE;
#اگر_ختم
}

شمار nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

ساکن bool مطابقت_host(ثابت حرف *token, size_t tokenlen,
                       ثابت حرف *name, size_t namelen)
{
  bool مطابقت = FALSE;

  /* ignore trailing dots in the token to check */
  اگر(token[tokenlen - 1] == '.')
    tokenlen--;

  اگر(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  اگر(tokenlen == namelen)
    /* case A, exact match */
    مطابقت = curl_strnequal(token, name, namelen);
  ورنہ اگر(tokenlen < namelen) {
    /* case B, tailmatch domain */
    مطابقت = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  واپس مطابقت;
}

ساکن bool مطابقت_ip(صحیح type, ثابت حرف *token, size_t tokenlen,
                     ثابت حرف *name)
{
  حرف *slash;
  صحیح بٹ = -1; /* negative means not provided */
  حرف checkip[128];
  اگر(tokenlen >= حجم(checkip))
    /* this cannot match */
    واپس FALSE;
  /* copy the check name to a temp buffer */
  حافظہ_نقل(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  اگر(slash) {
    curl_off_t قدر;
    ثابت حرف *p = &slash[1];
    اگر(curlx_str_نمبر(&p, &قدر, 128) || *p)
      واپس FALSE;
    /* a too large value is rejected in the cidr function below */
    بٹ = (صحیح)قدر;
    *slash = 0; /* null-terminate there */
  }
  اگر(type == TYPE_IPV6)
    واپس cidr6_مطابقت(name, checkip, بٹ);
  ورنہ
    واپس cidr4_مطابقت(name, checkip, بٹ);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(ثابت حرف *name, ثابت حرف *no_proxy);
UNITTEST bool proxy_check_noproxy(ثابت حرف *name, ثابت حرف *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  اگر(!name || name[0] == '\0')
    واپس FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  اگر(no_proxy && no_proxy[0]) {
    ثابت حرف *p = no_proxy;
    size_t namelen;
    حرف پتہ[16];
    شمار nametype type = TYPE_HOST;
    اگر(!strcmp("*", no_proxy))
      واپس TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = لڑی_لمبائی(name);
    اگر(curlx_inet_pton(AF_INET, name, &پتہ) == 1)
      type = TYPE_IPV4;
#اگر_تعریف_شدہ USE_IPV6
    ورنہ اگر(curlx_inet_pton(AF_INET6, name, &پتہ) == 1)
      type = TYPE_IPV6;
#اگر_ختم
    ورنہ {
      /* ignore trailing dots in the hostname */
      اگر(name[namelen - 1] == '.')
        namelen--;
    }

    جب(*p) {
      ثابت حرف *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      جب(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      اگر(tokenlen) {
        bool مطابقت = FALSE;
        اگر(type == TYPE_HOST)
          مطابقت = مطابقت_host(token, tokenlen, name, namelen);
        ورنہ
          مطابقت = مطابقت_ip(type, token, tokenlen, name);

        اگر(مطابقت)
          واپس TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      اگر(*p != ',')
        توڑو;
      /* pass any number of commas */
      جب(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  واپس FALSE;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
ساکن حرف *proxy_detect_proxy(ساخت Curl_easy *ڈیٹا,
                                ثابت ساخت Curl_scheme *scheme)
{
  /* If proxy was not specified, we check for default proxy environment
   * variables, to enable i.e Lynx compliance:
   *
   * http_proxy=http://some.server.dom:port/
   * https_proxy=http://some.server.dom:port/
   * ftp_proxy=http://some.server.dom:port/
   * no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   * all_proxy=http://some.server.dom:port/
   *   (seems to exist for the CERN www lib. Probably
   *   the first to check for.)
   *
   * For compatibility, the all-uppercase versions of these variables are
   * checked if the lowercase versions do not exist.
   */
  ثابت حرف *env_name = تہی;
  حرف *proxy = تہی;
  حرف name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  اگر((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, حجم(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    اگر(!proxy) {
      Curl_strntoupper(name_buf, name_buf, حجم(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  اگر(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    اگر(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  ورنہ اگر(!proxy &&
          ((scheme == &Curl_scheme_http) || (scheme == &Curl_scheme_ws))) {
    /* Not found, check 'http' env vars, also for 'ws'.
     * We do NOT try the uppercase version 'HTTP_PROXY' because of
     * security reasons:
     *
     * When curl is used in a webserver application
     * environment (cgi or php), this environment variable can
     * be controlled by the web server user by setting the
     * http header 'Proxy:' to some value.
     *
     * This can cause 'internal' http/ftp requests to be
     * arbitrarily redirected by any external attacker.
     */
    env_name = "http_proxy";
    proxy = curl_getenv(env_name);
  }

  اگر(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    اگر(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  اگر(proxy)
    infof(ڈیٹا, "Uses proxy env variable %s == '%s'", env_name, proxy);

  واپس proxy;
}
#اگر_ختم /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
ساکن CURLcode parse_proxy(ساخت Curl_easy *ڈیٹا,
                            ثابت حرف *proxy,
                            bool for_pre_proxy,
                            ساخت proxy_info *proxyinfo)
{
  حرف *proxyuser = تہی;
  حرف *proxypasswd = تہی;
  حرف *scheme = تہی;
  CURLcode نتیجہ = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : ڈیٹا->سیٹ.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  اگر(!uhp) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_سیٹ(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  اگر(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    اگر(!uc) {
      نتیجہ = Curl_scheme_مقصد_proxytype(ڈیٹا, scheme, &proxytype, proxy);
      اگر(نتیجہ)
        جاؤ error;
    }
    ورنہ اگر(uc != CURLUE_NO_SCHEME) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  ورنہ {
    failf(ڈیٹا, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    نتیجہ = CURLE_COULDNT_RESOLVE_PROXY;
    جاؤ error;
  }

  نتیجہ = Curl_peer_from_proxy_url(uhp, ڈیٹا, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  اگر(نتیجہ)
    جاؤ error;

  انتخاب(proxytype) {
    حالت CURLPROXY_HTTP:
    حالت CURLPROXY_HTTP_1_0:
    حالت CURLPROXY_HTTPS:
    حالت CURLPROXY_HTTPS2:
    حالت CURLPROXY_HTTPS3:
      اگر(for_pre_proxy) {
        failf(ڈیٹا, "Unsupported pre-proxy type for \'%s\'", proxy);
        نتیجہ = CURLE_COULDNT_RESOLVE_PROXY;
        جاؤ error;
      }
      توڑو;
    حالت CURLPROXY_SOCKS4:
    حالت CURLPROXY_SOCKS4A:
    حالت CURLPROXY_SOCKS5:
    حالت CURLPROXY_SOCKS5_HOSTNAME:
      توڑو;
    طےشدہ:
      failf(ڈیٹا, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      نتیجہ = CURLE_COULDNT_RESOLVE_PROXY;
      جاؤ error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  اگر(uc && (uc != CURLUE_NO_USER)) {
    نتیجہ = Curl_uc_مقصد_curlcode(uc);
    جاؤ error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  اگر(uc && (uc != CURLUE_NO_PASSWORD)) {
    نتیجہ = Curl_uc_مقصد_curlcode(uc);
    جاؤ error;
  }

  اگر(proxyuser || proxypasswd) {
    نتیجہ = Curl_creds_create(proxyuser, proxypasswd, تہی, تہی,
                               CURL_EASY_STR(ڈیٹا, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    اگر(نتیجہ)
      جاؤ error;
  }
  ورنہ اگر(!for_pre_proxy &&
          (CURL_EASY_STR(ڈیٹا, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(ڈیٹا, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(ڈیٹا, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    نتیجہ = Curl_creds_create(CURL_EASY_STR(ڈیٹا, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(ڈیٹا, STRING_PROXYPASSWORD),
                               تہی, تہی,
                               CURL_EASY_STR(ڈیٹا, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  ورنہ
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#اگر_تعریف_شدہ DEBUGBUILD
  اگر(!نتیجہ) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#اگر_ختم
  واپس نتیجہ;
}

/* Is transfer's origin exempted from proxy use? */
ساکن bool proxy_do_not_proxy(ساخت Curl_easy *ڈیٹا)
{
  ثابت حرف *no_proxy;
  حرف *env_no_proxy = تہی;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  اگر(ڈیٹا->state.origin->scheme->flags & PROTOPT_NONETWORK)
    واپس TRUE;

  no_proxy = CURL_EASY_STR(ڈیٹا, STRING_NOPROXY);
  اگر(!no_proxy) {
    ثابت حرف *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    اگر(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    اگر(env_no_proxy)
      infof(ڈیٹا, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(ڈیٹا->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  واپس do_not_proxy;
}

CURLcode Curl_proxy_init_conn(ساخت Curl_easy *ڈیٹا,
                              ساخت connectdata *conn)
{
  حرف *proxy = تہی;
  حرف *pre_proxy = تہی;
  ثابت حرف *str = تہی;
  bool do_env_detect = TRUE;
  CURLcode نتیجہ = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->بٹ.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  اگر(proxy_do_not_proxy(ڈیٹا))
    جاؤ برآمد;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(ڈیٹا, STRING_PROXY);
  اگر(str) {
    اگر(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      اگر(!proxy) {
        failf(ڈیٹا, "memory shortage");
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
    }
    ورنہ
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(ڈیٹا, STRING_PRE_PROXY);
  اگر(str) {
    اگر(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      اگر(!pre_proxy) {
        failf(ڈیٹا, "memory shortage");
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
    }
    ورنہ
      do_env_detect = FALSE;
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  اگر(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(ڈیٹا, conn->scheme);
#ورنہ
  (خالی)do_env_detect;
#اگر_ختم /* CURL_DISABLE_HTTP */

  اگر(!proxy && !pre_proxy)
    جاؤ برآمد;

  اگر(pre_proxy) {
    نتیجہ = parse_proxy(ڈیٹا, pre_proxy, TRUE, &conn->socks_proxy);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  اگر(proxy) {
    نتیجہ = parse_proxy(ڈیٹا, proxy, FALSE, &conn->http_proxy);
    اگر(نتیجہ)
      جاؤ برآمد;

    انتخاب(conn->http_proxy.proxytype) {
    حالت CURLPROXY_SOCKS4:
    حالت CURLPROXY_SOCKS4A:
    حالت CURLPROXY_SOCKS5:
    حالت CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      اگر(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(ڈیٹا, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        نتیجہ = CURLE_COULDNT_RESOLVE_PROXY;
        جاؤ برآمد;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, حجم(conn->http_proxy));
      توڑو;
    طےشدہ:
      /* all other types are HTTP */
      توڑو;
    }
  }

  اگر(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#اگر_تعریف_شدہ CURL_DISABLE_HTTP
  اگر(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    نتیجہ = CURLE_UNSUPPORTED_PROTOCOL;
    جاؤ برآمد;
  }

#ورنہ /* CURL_DISABLE_HTTP */
  اگر(conn->http_proxy.peer) {
    ثابت ساخت Curl_scheme *scheme = ڈیٹا->state.origin->scheme;
    bool tunnel_proxy = (bool)ڈیٹا->سیٹ.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    اگر(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      اگر(conn->بذریعہ_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      ورنہ اگر(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      ورنہ اگر(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      ورنہ اگر(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    اگر(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->بذریعہ_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->بٹ.origin_is_proxy = TRUE;
    }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DIGEST_AUTH
    اگر(!Curl_safecmp(ڈیٹا->state.envproxy, proxy)) {
      /* proxy changed */
      Curl_auth_digest_cleanup(&ڈیٹا->state.proxydigest);
      curlx_free(ڈیٹا->state.envproxy);
      ڈیٹا->state.envproxy = curlx_strdup(proxy);
    }
#اگر_ختم
  }
#اگر_ختم /* !CURL_DISABLE_HTTP */

برآمد:
  curlx_free(pre_proxy);
  curlx_free(proxy);
  واپس نتیجہ;
}

#اگر_ختم /* CURL_DISABLE_PROXY */
