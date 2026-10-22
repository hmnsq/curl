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
#ضمن "curl_setup.h"

#اگر_تعریف_نشده CURL_DISABLE_PROXY

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "protocol.h"
#ضمن "proxy.h"
#ضمن "http_proxy.h"
#ضمن "strcase.h"
#ضمن "url.h"
#ضمن "vauth/vauth.h"
#ضمن "curlx/inet_pton.h"
#ضمن "curlx/strparse.h"

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_تطابق(ثابت حرف *ipv4,    /* 1.2.3.4 address */
                          ثابت حرف *شبکه, /* 1.2.3.4 address */
                          صحيح بیتها);
UNITTEST bool cidr4_تطابق(ثابت حرف *ipv4,    /* 1.2.3.4 address */
                          ثابت حرف *شبکه, /* 1.2.3.4 address */
                          صحيح بیتها)
{
  بیعلامت صحيح آدرس = 0;
  بیعلامت صحيح check = 0;

  إذا(بیتها > 32)
    /* strange input */
    أرجع FALSE;

  إذا(curlx_inet_pton(AF_INET, ipv4, &آدرس) != 1)
    أرجع FALSE;
  إذا(curlx_inet_pton(AF_INET, شبکه, &check) != 1)
    أرجع FALSE;

  إذا((بیتها > 0) && (بیتها != 32)) {
    بیعلامت صحيح ماسک = 0xffffffff << (32 - بیتها);
    بیعلامت صحيح haddr = htonl(آدرس);
    بیعلامت صحيح hcheck = htonl(check);
#إذا 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, شبکه, hcheck, بیتها, ماسک,
                  (haddr ^ hcheck) & ماسک);
#پایان_اگر
    إذا((haddr ^ hcheck) & ماسک)
      أرجع FALSE;
    أرجع TRUE;
  }
  وإلا إذا(!بیتها && !check)
    /* 0.0.0.0/0 matches everything */
    أرجع TRUE;

  أرجع آدرس == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_تطابق(ثابت حرف *ipv6, ثابت حرف *شبکه,
                          صحيح بیتها);
UNITTEST bool cidr6_تطابق(ثابت حرف *ipv6, ثابت حرف *شبکه,
                          صحيح بیتها)
{
#اگر_تعریف_شده USE_IPV6
  ساكن ثابت حرف null6[16]= { 0 };
  بیعلامت صحيح بایتها;
  بیعلامت صحيح rest;
  بیعلامت حرف آدرس[16];
  بیعلامت حرف check[16];

  إذا(بیتها == -1)
    بیتها = 128;

  بایتها = بیتها / 8;
  rest = بیتها & 0x07;
  إذا((بایتها > 16) || ((بایتها == 16) && rest))
    أرجع FALSE;
  إذا(curlx_inet_pton(AF_INET6, ipv6, آدرس) != 1)
    أرجع FALSE;
  إذا(curlx_inet_pton(AF_INET6, شبکه, check) != 1)
    أرجع FALSE;
  إذا((بایتها == 16) && !بیتها && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    أرجع TRUE;
  إذا(بایتها && memcmp(آدرس, check, بایتها))
    أرجع FALSE;
  إذا(rest && ((آدرس[بایتها] ^ check[بایتها]) & (0xff << (8 - rest))))
    أرجع FALSE;

  أرجع TRUE;
#وإلا
  (فراغ)ipv6;
  (فراغ)شبکه;
  (فراغ)بیتها;
  أرجع FALSE;
#پایان_اگر
}

تعداد nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

ساكن bool تطابق_host(ثابت حرف *token, size_t tokenlen,
                       ثابت حرف *name, size_t namelen)
{
  bool تطابق = FALSE;

  /* ignore trailing dots in the token to check */
  إذا(token[tokenlen - 1] == '.')
    tokenlen--;

  إذا(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  إذا(tokenlen == namelen)
    /* case A, exact match */
    تطابق = curl_strnequal(token, name, namelen);
  وإلا إذا(tokenlen < namelen) {
    /* case B, tailmatch domain */
    تطابق = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  أرجع تطابق;
}

ساكن bool تطابق_ip(صحيح type, ثابت حرف *token, size_t tokenlen,
                     ثابت حرف *name)
{
  حرف *slash;
  صحيح بیتها = -1; /* negative means not provided */
  حرف checkip[128];
  إذا(tokenlen >= حجم(checkip))
    /* this cannot match */
    أرجع FALSE;
  /* copy the check name to a temp buffer */
  کپی_حافظه(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  إذا(slash) {
    curl_off_t مقدار;
    ثابت حرف *p = &slash[1];
    إذا(curlx_str_عدد(&p, &مقدار, 128) || *p)
      أرجع FALSE;
    /* a too large value is rejected in the cidr function below */
    بیتها = (صحيح)مقدار;
    *slash = 0; /* null-terminate there */
  }
  إذا(type == TYPE_IPV6)
    أرجع cidr6_تطابق(name, checkip, بیتها);
  وإلا
    أرجع cidr4_تطابق(name, checkip, بیتها);
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
  إذا(!name || name[0] == '\0')
    أرجع FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  إذا(no_proxy && no_proxy[0]) {
    ثابت حرف *p = no_proxy;
    size_t namelen;
    حرف آدرس[16];
    تعداد nametype type = TYPE_HOST;
    إذا(!strcmp("*", no_proxy))
      أرجع TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = طول_رشته(name);
    إذا(curlx_inet_pton(AF_INET, name, &آدرس) == 1)
      type = TYPE_IPV4;
#اگر_تعریف_شده USE_IPV6
    وإلا إذا(curlx_inet_pton(AF_INET6, name, &آدرس) == 1)
      type = TYPE_IPV6;
#پایان_اگر
    وإلا {
      /* ignore trailing dots in the hostname */
      إذا(name[namelen - 1] == '.')
        namelen--;
    }

    طالما(*p) {
      ثابت حرف *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      طالما(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      إذا(tokenlen) {
        bool تطابق = FALSE;
        إذا(type == TYPE_HOST)
          تطابق = تطابق_host(token, tokenlen, name, namelen);
        وإلا
          تطابق = تطابق_ip(type, token, tokenlen, name);

        إذا(تطابق)
          أرجع TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      إذا(*p != ',')
        اكسر;
      /* pass any number of commas */
      طالما(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  أرجع FALSE;
}

#اگر_تعریف_نشده CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
ساكن حرف *proxy_detect_proxy(بنية Curl_easy *داده,
                                ثابت بنية Curl_scheme *scheme)
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
  ثابت حرف *env_name = عدم;
  حرف *proxy = عدم;
  حرف name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  إذا((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, حجم(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    إذا(!proxy) {
      Curl_strntoupper(name_buf, name_buf, حجم(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  إذا(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    إذا(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  وإلا إذا(!proxy &&
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

  إذا(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    إذا(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  إذا(proxy)
    infof(داده, "Uses proxy env variable %s == '%s'", env_name, proxy);

  أرجع proxy;
}
#پایان_اگر /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
ساكن CURLcode parse_proxy(بنية Curl_easy *داده,
                            ثابت حرف *proxy,
                            bool for_pre_proxy,
                            بنية proxy_info *proxyinfo)
{
  حرف *proxyuser = عدم;
  حرف *proxypasswd = عدم;
  حرف *scheme = عدم;
  CURLcode نتیجه = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : داده->مجموعه.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  إذا(!uhp) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_مجموعه(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  إذا(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    إذا(!uc) {
      نتیجه = Curl_scheme_مقصد_proxytype(داده, scheme, &proxytype, proxy);
      إذا(نتیجه)
        اذهب error;
    }
    وإلا إذا(uc != CURLUE_NO_SCHEME) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  وإلا {
    failf(داده, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    نتیجه = CURLE_COULDNT_RESOLVE_PROXY;
    اذهب error;
  }

  نتیجه = Curl_peer_from_proxy_url(uhp, داده, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  إذا(نتیجه)
    اذهب error;

  بدل(proxytype) {
    حالة CURLPROXY_HTTP:
    حالة CURLPROXY_HTTP_1_0:
    حالة CURLPROXY_HTTPS:
    حالة CURLPROXY_HTTPS2:
    حالة CURLPROXY_HTTPS3:
      إذا(for_pre_proxy) {
        failf(داده, "Unsupported pre-proxy type for \'%s\'", proxy);
        نتیجه = CURLE_COULDNT_RESOLVE_PROXY;
        اذهب error;
      }
      اكسر;
    حالة CURLPROXY_SOCKS4:
    حالة CURLPROXY_SOCKS4A:
    حالة CURLPROXY_SOCKS5:
    حالة CURLPROXY_SOCKS5_HOSTNAME:
      اكسر;
    افتراضي:
      failf(داده, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      نتیجه = CURLE_COULDNT_RESOLVE_PROXY;
      اذهب error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  إذا(uc && (uc != CURLUE_NO_USER)) {
    نتیجه = Curl_uc_مقصد_curlcode(uc);
    اذهب error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  إذا(uc && (uc != CURLUE_NO_PASSWORD)) {
    نتیجه = Curl_uc_مقصد_curlcode(uc);
    اذهب error;
  }

  إذا(proxyuser || proxypasswd) {
    نتیجه = Curl_creds_create(proxyuser, proxypasswd, عدم, عدم,
                               CURL_EASY_STR(داده, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    إذا(نتیجه)
      اذهب error;
  }
  وإلا إذا(!for_pre_proxy &&
          (CURL_EASY_STR(داده, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(داده, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(داده, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    نتیجه = Curl_creds_create(CURL_EASY_STR(داده, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(داده, STRING_PROXYPASSWORD),
                               عدم, عدم,
                               CURL_EASY_STR(داده, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  وإلا
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#اگر_تعریف_شده DEBUGBUILD
  إذا(!نتیجه) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#پایان_اگر
  أرجع نتیجه;
}

/* Is transfer's origin exempted from proxy use? */
ساكن bool proxy_do_not_proxy(بنية Curl_easy *داده)
{
  ثابت حرف *no_proxy;
  حرف *env_no_proxy = عدم;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  إذا(داده->state.origin->scheme->flags & PROTOPT_NONETWORK)
    أرجع TRUE;

  no_proxy = CURL_EASY_STR(داده, STRING_NOPROXY);
  إذا(!no_proxy) {
    ثابت حرف *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    إذا(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    إذا(env_no_proxy)
      infof(داده, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(داده->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  أرجع do_not_proxy;
}

CURLcode Curl_proxy_init_conn(بنية Curl_easy *داده,
                              بنية connectdata *conn)
{
  حرف *proxy = عدم;
  حرف *pre_proxy = عدم;
  ثابت حرف *str = عدم;
  bool do_env_detect = TRUE;
  CURLcode نتیجه = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->بیتها.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  إذا(proxy_do_not_proxy(داده))
    اذهب بیرون;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(داده, STRING_PROXY);
  إذا(str) {
    إذا(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      إذا(!proxy) {
        failf(داده, "memory shortage");
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب بیرون;
      }
    }
    وإلا
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(داده, STRING_PRE_PROXY);
  إذا(str) {
    إذا(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      إذا(!pre_proxy) {
        failf(داده, "memory shortage");
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب بیرون;
      }
    }
    وإلا
      do_env_detect = FALSE;
  }

#اگر_تعریف_نشده CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  إذا(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(داده, conn->scheme);
#وإلا
  (فراغ)do_env_detect;
#پایان_اگر /* CURL_DISABLE_HTTP */

  إذا(!proxy && !pre_proxy)
    اذهب بیرون;

  إذا(pre_proxy) {
    نتیجه = parse_proxy(داده, pre_proxy, TRUE, &conn->socks_proxy);
    إذا(نتیجه)
      اذهب بیرون;
  }

  إذا(proxy) {
    نتیجه = parse_proxy(داده, proxy, FALSE, &conn->http_proxy);
    إذا(نتیجه)
      اذهب بیرون;

    بدل(conn->http_proxy.proxytype) {
    حالة CURLPROXY_SOCKS4:
    حالة CURLPROXY_SOCKS4A:
    حالة CURLPROXY_SOCKS5:
    حالة CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      إذا(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(داده, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        نتیجه = CURLE_COULDNT_RESOLVE_PROXY;
        اذهب بیرون;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, حجم(conn->http_proxy));
      اكسر;
    افتراضي:
      /* all other types are HTTP */
      اكسر;
    }
  }

  إذا(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#اگر_تعریف_شده CURL_DISABLE_HTTP
  إذا(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    نتیجه = CURLE_UNSUPPORTED_PROTOCOL;
    اذهب بیرون;
  }

#وإلا /* CURL_DISABLE_HTTP */
  إذا(conn->http_proxy.peer) {
    ثابت بنية Curl_scheme *scheme = داده->state.origin->scheme;
    bool tunnel_proxy = (bool)داده->مجموعه.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    إذا(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      إذا(conn->از_طریق_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      وإلا إذا(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      وإلا إذا(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      وإلا إذا(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    إذا(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->از_طریق_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->بیتها.origin_is_proxy = TRUE;
    }

#اگر_تعریف_نشده CURL_DISABLE_DIGEST_AUTH
    إذا(!Curl_safecmp(داده->state.envproxy, proxy)) {
      /* proxy changed */
      Curl_auth_digest_cleanup(&داده->state.proxydigest);
      curlx_free(داده->state.envproxy);
      داده->state.envproxy = curlx_strdup(proxy);
    }
#پایان_اگر
  }
#پایان_اگر /* !CURL_DISABLE_HTTP */

بیرون:
  curlx_free(pre_proxy);
  curlx_free(proxy);
  أرجع نتیجه;
}

#پایان_اگر /* CURL_DISABLE_PROXY */
