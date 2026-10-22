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

#ifndef CURL_DISABLE_PROXY

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

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_match(ثابت نویسه *ipv4,    /* 1.2.3.4 address */
                          ثابت نویسه *network, /* 1.2.3.4 address */
                          صحیح bits);
UNITTEST bool cidr4_match(ثابت نویسه *ipv4,    /* 1.2.3.4 address */
                          ثابت نویسه *network, /* 1.2.3.4 address */
                          صحیح bits)
{
  unsigned صحیح address = 0;
  unsigned صحیح check = 0;

  اگر(bits > 32)
    /* strange input */
    بازگشت FALSE;

  اگر(curlx_inet_pton(AF_INET, ipv4, &address) != 1)
    بازگشت FALSE;
  اگر(curlx_inet_pton(AF_INET, network, &check) != 1)
    بازگشت FALSE;

  اگر((bits > 0) && (bits != 32)) {
    unsigned صحیح mask = 0xffffffff << (32 - bits);
    unsigned صحیح haddr = htonl(address);
    unsigned صحیح hcheck = htonl(check);
#اگر 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, network, hcheck, bits, mask,
                  (haddr ^ hcheck) & mask);
#endif
    اگر((haddr ^ hcheck) & mask)
      بازگشت FALSE;
    بازگشت TRUE;
  }
  وگرنه اگر(!bits && !check)
    /* 0.0.0.0/0 matches everything */
    بازگشت TRUE;

  بازگشت address == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_match(ثابت نویسه *ipv6, ثابت نویسه *network,
                          صحیح bits);
UNITTEST bool cidr6_match(ثابت نویسه *ipv6, ثابت نویسه *network,
                          صحیح bits)
{
#ifdef USE_IPV6
  ایستا ثابت نویسه null6[16]= { 0 };
  unsigned صحیح bytes;
  unsigned صحیح rest;
  unsigned نویسه address[16];
  unsigned نویسه check[16];

  اگر(bits == -1)
    bits = 128;

  bytes = bits / 8;
  rest = bits & 0x07;
  اگر((bytes > 16) || ((bytes == 16) && rest))
    بازگشت FALSE;
  اگر(curlx_inet_pton(AF_INET6, ipv6, address) != 1)
    بازگشت FALSE;
  اگر(curlx_inet_pton(AF_INET6, network, check) != 1)
    بازگشت FALSE;
  اگر((bytes == 16) && !bits && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    بازگشت TRUE;
  اگر(bytes && memcmp(address, check, bytes))
    بازگشت FALSE;
  اگر(rest && ((address[bytes] ^ check[bytes]) & (0xff << (8 - rest))))
    بازگشت FALSE;

  بازگشت TRUE;
#وگرنه
  (تهی)ipv6;
  (تهی)network;
  (تهی)bits;
  بازگشت FALSE;
#endif
}

شمارش nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

ایستا bool match_host(ثابت نویسه *token, size_t tokenlen,
                       ثابت نویسه *name, size_t namelen)
{
  bool match = FALSE;

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
    match = curl_strnequal(token, name, namelen);
  وگرنه اگر(tokenlen < namelen) {
    /* case B, tailmatch domain */
    match = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  بازگشت match;
}

ایستا bool match_ip(صحیح type, ثابت نویسه *token, size_t tokenlen,
                     ثابت نویسه *name)
{
  نویسه *slash;
  صحیح bits = -1; /* negative means not provided */
  نویسه checkip[128];
  اگر(tokenlen >= اندازه(checkip))
    /* this cannot match */
    بازگشت FALSE;
  /* copy the check name to a temp buffer */
  memcpy(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  اگر(slash) {
    curl_off_t مقدار;
    ثابت نویسه *p = &slash[1];
    اگر(curlx_str_عدد(&p, &مقدار, 128) || *p)
      بازگشت FALSE;
    /* a too large value is rejected in the cidr function below */
    bits = (صحیح)مقدار;
    *slash = 0; /* null-terminate there */
  }
  اگر(type == TYPE_IPV6)
    بازگشت cidr6_match(name, checkip, bits);
  وگرنه
    بازگشت cidr4_match(name, checkip, bits);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(ثابت نویسه *name, ثابت نویسه *no_proxy);
UNITTEST bool proxy_check_noproxy(ثابت نویسه *name, ثابت نویسه *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  اگر(!name || name[0] == '\0')
    بازگشت FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  اگر(no_proxy && no_proxy[0]) {
    ثابت نویسه *p = no_proxy;
    size_t namelen;
    نویسه address[16];
    شمارش nametype type = TYPE_HOST;
    اگر(!strcmp("*", no_proxy))
      بازگشت TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = strlen(name);
    اگر(curlx_inet_pton(AF_INET, name, &address) == 1)
      type = TYPE_IPV4;
#ifdef USE_IPV6
    وگرنه اگر(curlx_inet_pton(AF_INET6, name, &address) == 1)
      type = TYPE_IPV6;
#endif
    وگرنه {
      /* ignore trailing dots in the hostname */
      اگر(name[namelen - 1] == '.')
        namelen--;
    }

    وقتی(*p) {
      ثابت نویسه *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      وقتی(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      اگر(tokenlen) {
        bool match = FALSE;
        اگر(type == TYPE_HOST)
          match = match_host(token, tokenlen, name, namelen);
        وگرنه
          match = match_ip(type, token, tokenlen, name);

        اگر(match)
          بازگشت TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      اگر(*p != ',')
        بشکن;
      /* pass any number of commas */
      وقتی(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  بازگشت FALSE;
}

#ifndef CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
ایستا نویسه *proxy_detect_proxy(ساختار Curl_easy *data,
                                ثابت ساختار Curl_scheme *scheme)
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
  ثابت نویسه *env_name = پوچ;
  نویسه *proxy = پوچ;
  نویسه name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  اگر((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, اندازه(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    اگر(!proxy) {
      Curl_strntoupper(name_buf, name_buf, اندازه(name_buf));
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
  وگرنه اگر(!proxy &&
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
    infof(data, "Uses proxy env variable %s == '%s'", env_name, proxy);

  بازگشت proxy;
}
#endif /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
ایستا CURLcode parse_proxy(ساختار Curl_easy *data,
                            ثابت نویسه *proxy,
                            bool for_pre_proxy,
                            ساختار proxy_info *proxyinfo)
{
  نویسه *proxyuser = پوچ;
  نویسه *proxypasswd = پوچ;
  نویسه *scheme = پوچ;
  CURLcode result = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : data->set.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  اگر(!uhp) {
    result = CURLE_OUT_OF_MEMORY;
    برو error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_set(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  اگر(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    اگر(!uc) {
      result = Curl_scheme_to_proxytype(data, scheme, &proxytype, proxy);
      اگر(result)
        برو error;
    }
    وگرنه اگر(uc != CURLUE_NO_SCHEME) {
      result = CURLE_OUT_OF_MEMORY;
      برو error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  وگرنه {
    failf(data, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    result = CURLE_COULDNT_RESOLVE_PROXY;
    برو error;
  }

  result = Curl_peer_from_proxy_url(uhp, data, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  اگر(result)
    برو error;

  انتخاب(proxytype) {
    حالت CURLPROXY_HTTP:
    حالت CURLPROXY_HTTP_1_0:
    حالت CURLPROXY_HTTPS:
    حالت CURLPROXY_HTTPS2:
    حالت CURLPROXY_HTTPS3:
      اگر(for_pre_proxy) {
        failf(data, "Unsupported pre-proxy type for \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        برو error;
      }
      بشکن;
    حالت CURLPROXY_SOCKS4:
    حالت CURLPROXY_SOCKS4A:
    حالت CURLPROXY_SOCKS5:
    حالت CURLPROXY_SOCKS5_HOSTNAME:
      بشکن;
    پیشفرض:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      برو error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  اگر(uc && (uc != CURLUE_NO_USER)) {
    result = Curl_uc_to_curlcode(uc);
    برو error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  اگر(uc && (uc != CURLUE_NO_PASSWORD)) {
    result = Curl_uc_to_curlcode(uc);
    برو error;
  }

  اگر(proxyuser || proxypasswd) {
    result = Curl_creds_create(proxyuser, proxypasswd, پوچ, پوچ,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    اگر(result)
      برو error;
  }
  وگرنه اگر(!for_pre_proxy &&
          (CURL_EASY_STR(data, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(data, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    result = Curl_creds_create(CURL_EASY_STR(data, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(data, STRING_PROXYPASSWORD),
                               پوچ, پوچ,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  وگرنه
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ifdef DEBUGBUILD
  اگر(!result) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#endif
  بازگشت result;
}

/* Is transfer's origin exempted from proxy use? */
ایستا bool proxy_do_not_proxy(ساختار Curl_easy *data)
{
  ثابت نویسه *no_proxy;
  نویسه *env_no_proxy = پوچ;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  اگر(data->state.origin->scheme->flags & PROTOPT_NONETWORK)
    بازگشت TRUE;

  no_proxy = CURL_EASY_STR(data, STRING_NOPROXY);
  اگر(!no_proxy) {
    ثابت نویسه *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    اگر(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    اگر(env_no_proxy)
      infof(data, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(data->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  بازگشت do_not_proxy;
}

CURLcode Curl_proxy_init_conn(ساختار Curl_easy *data,
                              ساختار connectdata *conn)
{
  نویسه *proxy = پوچ;
  نویسه *pre_proxy = پوچ;
  ثابت نویسه *str = پوچ;
  bool do_env_detect = TRUE;
  CURLcode result = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->bits.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  اگر(proxy_do_not_proxy(data))
    برو out;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(data, STRING_PROXY);
  اگر(str) {
    اگر(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      اگر(!proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
    }
    وگرنه
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(data, STRING_PRE_PROXY);
  اگر(str) {
    اگر(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      اگر(!pre_proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
    }
    وگرنه
      do_env_detect = FALSE;
  }

#ifndef CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  اگر(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(data, conn->scheme);
#وگرنه
  (تهی)do_env_detect;
#endif /* CURL_DISABLE_HTTP */

  اگر(!proxy && !pre_proxy)
    برو out;

  اگر(pre_proxy) {
    result = parse_proxy(data, pre_proxy, TRUE, &conn->socks_proxy);
    اگر(result)
      برو out;
  }

  اگر(proxy) {
    result = parse_proxy(data, proxy, FALSE, &conn->http_proxy);
    اگر(result)
      برو out;

    انتخاب(conn->http_proxy.proxytype) {
    حالت CURLPROXY_SOCKS4:
    حالت CURLPROXY_SOCKS4A:
    حالت CURLPROXY_SOCKS5:
    حالت CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      اگر(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(data, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        برو out;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, اندازه(conn->http_proxy));
      بشکن;
    پیشفرض:
      /* all other types are HTTP */
      بشکن;
    }
  }

  اگر(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ifdef CURL_DISABLE_HTTP
  اگر(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    result = CURLE_UNSUPPORTED_PROTOCOL;
    برو out;
  }

#وگرنه /* CURL_DISABLE_HTTP */
  اگر(conn->http_proxy.peer) {
    ثابت ساختار Curl_scheme *scheme = data->state.origin->scheme;
    bool tunnel_proxy = (bool)data->set.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    اگر(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      اگر(conn->via_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      وگرنه اگر(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      وگرنه اگر(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      وگرنه اگر(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    اگر(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->via_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->bits.origin_is_proxy = TRUE;
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    اگر(!Curl_safecmp(data->state.envproxy, proxy)) {
      /* proxy changed */
      Curl_auth_digest_cleanup(&data->state.proxydigest);
      curlx_free(data->state.envproxy);
      data->state.envproxy = curlx_strdup(proxy);
    }
#endif
  }
#endif /* !CURL_DISABLE_HTTP */

out:
  curlx_free(pre_proxy);
  curlx_free(proxy);
  بازگشت result;
}

#endif /* CURL_DISABLE_PROXY */
