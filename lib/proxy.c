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
#अन्तर्भाव "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "protocol.h"
#अन्तर्भाव "proxy.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "curlx/strparse.h"

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_match(ध्रुव वर्ण *ipv4,    /* 1.2.3.4 address */
                          ध्रुव वर्ण *network, /* 1.2.3.4 address */
                          पूर्णाङ्क bits);
UNITTEST bool cidr4_match(ध्रुव वर्ण *ipv4,    /* 1.2.3.4 address */
                          ध्रुव वर्ण *network, /* 1.2.3.4 address */
                          पूर्णाङ्क bits)
{
  unsigned पूर्णाङ्क address = 0;
  unsigned पूर्णाङ्क check = 0;

  सचे(bits > 32)
    /* strange input */
    पटिनिवत्त FALSE;

  सचे(curlx_inet_pton(AF_INET, ipv4, &address) != 1)
    पटिनिवत्त FALSE;
  सचे(curlx_inet_pton(AF_INET, network, &check) != 1)
    पटिनिवत्त FALSE;

  सचे((bits > 0) && (bits != 32)) {
    unsigned पूर्णाङ्क mask = 0xffffffff << (32 - bits);
    unsigned पूर्णाङ्क haddr = htonl(address);
    unsigned पूर्णाङ्क hcheck = htonl(check);
#सचे 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, network, hcheck, bits, mask,
                  (haddr ^ hcheck) & mask);
#endif
    सचे((haddr ^ hcheck) & mask)
      पटिनिवत्त FALSE;
    पटिनिवत्त TRUE;
  }
  अञ्ञथा सचे(!bits && !check)
    /* 0.0.0.0/0 matches everything */
    पटिनिवत्त TRUE;

  पटिनिवत्त address == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_match(ध्रुव वर्ण *ipv6, ध्रुव वर्ण *network,
                          पूर्णाङ्क bits);
UNITTEST bool cidr6_match(ध्रुव वर्ण *ipv6, ध्रुव वर्ण *network,
                          पूर्णाङ्क bits)
{
#ifdef USE_IPV6
  स्थिर ध्रुव वर्ण null6[16]= { 0 };
  unsigned पूर्णाङ्क bytes;
  unsigned पूर्णाङ्क rest;
  unsigned वर्ण address[16];
  unsigned वर्ण check[16];

  सचे(bits == -1)
    bits = 128;

  bytes = bits / 8;
  rest = bits & 0x07;
  सचे((bytes > 16) || ((bytes == 16) && rest))
    पटिनिवत्त FALSE;
  सचे(curlx_inet_pton(AF_INET6, ipv6, address) != 1)
    पटिनिवत्त FALSE;
  सचे(curlx_inet_pton(AF_INET6, network, check) != 1)
    पटिनिवत्त FALSE;
  सचे((bytes == 16) && !bits && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    पटिनिवत्त TRUE;
  सचे(bytes && memcmp(address, check, bytes))
    पटिनिवत्त FALSE;
  सचे(rest && ((address[bytes] ^ check[bytes]) & (0xff << (8 - rest))))
    पटिनिवत्त FALSE;

  पटिनिवत्त TRUE;
#अञ्ञथा
  (शून्य)ipv6;
  (शून्य)network;
  (शून्य)bits;
  पटिनिवत्त FALSE;
#endif
}

गणना nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

स्थिर bool match_host(ध्रुव वर्ण *token, size_t tokenlen,
                       ध्रुव वर्ण *name, size_t namelen)
{
  bool match = FALSE;

  /* ignore trailing dots in the token to check */
  सचे(token[tokenlen - 1] == '.')
    tokenlen--;

  सचे(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  सचे(tokenlen == namelen)
    /* case A, exact match */
    match = curl_strnequal(token, name, namelen);
  अञ्ञथा सचे(tokenlen < namelen) {
    /* case B, tailmatch domain */
    match = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  पटिनिवत्त match;
}

स्थिर bool match_ip(पूर्णाङ्क type, ध्रुव वर्ण *token, size_t tokenlen,
                     ध्रुव वर्ण *name)
{
  वर्ण *slash;
  पूर्णाङ्क bits = -1; /* negative means not provided */
  वर्ण checkip[128];
  सचे(tokenlen >= परिमाण(checkip))
    /* this cannot match */
    पटिनिवत्त FALSE;
  /* copy the check name to a temp buffer */
  memcpy(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  सचे(slash) {
    curl_off_t मानम्;
    ध्रुव वर्ण *p = &slash[1];
    सचे(curlx_str_सङ्ख्या(&p, &मानम्, 128) || *p)
      पटिनिवत्त FALSE;
    /* a too large value is rejected in the cidr function below */
    bits = (पूर्णाङ्क)मानम्;
    *slash = 0; /* null-terminate there */
  }
  सचे(type == TYPE_IPV6)
    पटिनिवत्त cidr6_match(name, checkip, bits);
  अञ्ञथा
    पटिनिवत्त cidr4_match(name, checkip, bits);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(ध्रुव वर्ण *name, ध्रुव वर्ण *no_proxy);
UNITTEST bool proxy_check_noproxy(ध्रुव वर्ण *name, ध्रुव वर्ण *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  सचे(!name || name[0] == '\0')
    पटिनिवत्त FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  सचे(no_proxy && no_proxy[0]) {
    ध्रुव वर्ण *p = no_proxy;
    size_t namelen;
    वर्ण address[16];
    गणना nametype type = TYPE_HOST;
    सचे(!strcmp("*", no_proxy))
      पटिनिवत्त TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = strlen(name);
    सचे(curlx_inet_pton(AF_INET, name, &address) == 1)
      type = TYPE_IPV4;
#ifdef USE_IPV6
    अञ्ञथा सचे(curlx_inet_pton(AF_INET6, name, &address) == 1)
      type = TYPE_IPV6;
#endif
    अञ्ञथा {
      /* ignore trailing dots in the hostname */
      सचे(name[namelen - 1] == '.')
        namelen--;
    }

    याव(*p) {
      ध्रुव वर्ण *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      याव(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      सचे(tokenlen) {
        bool match = FALSE;
        सचे(type == TYPE_HOST)
          match = match_host(token, tokenlen, name, namelen);
        अञ्ञथा
          match = match_ip(type, token, tokenlen, name);

        सचे(match)
          पटिनिवत्त TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      सचे(*p != ',')
        भङ्ग;
      /* pass any number of commas */
      याव(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  पटिनिवत्त FALSE;
}

#ifndef CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
स्थिर वर्ण *proxy_detect_proxy(संरचना Curl_easy *data,
                                ध्रुव संरचना Curl_scheme *scheme)
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
  ध्रुव वर्ण *env_name = रिक्त;
  वर्ण *proxy = रिक्त;
  वर्ण name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  सचे((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, परिमाण(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    सचे(!proxy) {
      Curl_strntoupper(name_buf, name_buf, परिमाण(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  सचे(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    सचे(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  अञ्ञथा सचे(!proxy &&
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

  सचे(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    सचे(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  सचे(proxy)
    infof(data, "Uses proxy env variable %s == '%s'", env_name, proxy);

  पटिनिवत्त proxy;
}
#endif /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
स्थिर CURLcode parse_proxy(संरचना Curl_easy *data,
                            ध्रुव वर्ण *proxy,
                            bool for_pre_proxy,
                            संरचना proxy_info *proxyinfo)
{
  वर्ण *proxyuser = रिक्त;
  वर्ण *proxypasswd = रिक्त;
  वर्ण *scheme = रिक्त;
  CURLcode result = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : data->set.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  सचे(!uhp) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_set(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  सचे(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    सचे(!uc) {
      result = Curl_scheme_to_proxytype(data, scheme, &proxytype, proxy);
      सचे(result)
        गच्छ error;
    }
    अञ्ञथा सचे(uc != CURLUE_NO_SCHEME) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  अञ्ञथा {
    failf(data, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    result = CURLE_COULDNT_RESOLVE_PROXY;
    गच्छ error;
  }

  result = Curl_peer_from_proxy_url(uhp, data, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  सचे(result)
    गच्छ error;

  विकल्प(proxytype) {
    पक्ष CURLPROXY_HTTP:
    पक्ष CURLPROXY_HTTP_1_0:
    पक्ष CURLPROXY_HTTPS:
    पक्ष CURLPROXY_HTTPS2:
    पक्ष CURLPROXY_HTTPS3:
      सचे(for_pre_proxy) {
        failf(data, "Unsupported pre-proxy type for \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        गच्छ error;
      }
      भङ्ग;
    पक्ष CURLPROXY_SOCKS4:
    पक्ष CURLPROXY_SOCKS4A:
    पक्ष CURLPROXY_SOCKS5:
    पक्ष CURLPROXY_SOCKS5_HOSTNAME:
      भङ्ग;
    स्वभाव:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      गच्छ error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  सचे(uc && (uc != CURLUE_NO_USER)) {
    result = Curl_uc_to_curlcode(uc);
    गच्छ error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  सचे(uc && (uc != CURLUE_NO_PASSWORD)) {
    result = Curl_uc_to_curlcode(uc);
    गच्छ error;
  }

  सचे(proxyuser || proxypasswd) {
    result = Curl_creds_create(proxyuser, proxypasswd, रिक्त, रिक्त,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    सचे(result)
      गच्छ error;
  }
  अञ्ञथा सचे(!for_pre_proxy &&
          (CURL_EASY_STR(data, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(data, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    result = Curl_creds_create(CURL_EASY_STR(data, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(data, STRING_PROXYPASSWORD),
                               रिक्त, रिक्त,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  अञ्ञथा
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ifdef DEBUGBUILD
  सचे(!result) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#endif
  पटिनिवत्त result;
}

/* Is transfer's origin exempted from proxy use? */
स्थिर bool proxy_do_not_proxy(संरचना Curl_easy *data)
{
  ध्रुव वर्ण *no_proxy;
  वर्ण *env_no_proxy = रिक्त;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  सचे(data->state.origin->scheme->flags & PROTOPT_NONETWORK)
    पटिनिवत्त TRUE;

  no_proxy = CURL_EASY_STR(data, STRING_NOPROXY);
  सचे(!no_proxy) {
    ध्रुव वर्ण *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    सचे(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    सचे(env_no_proxy)
      infof(data, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(data->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  पटिनिवत्त do_not_proxy;
}

CURLcode Curl_proxy_init_conn(संरचना Curl_easy *data,
                              संरचना connectdata *conn)
{
  वर्ण *proxy = रिक्त;
  वर्ण *pre_proxy = रिक्त;
  ध्रुव वर्ण *str = रिक्त;
  bool do_env_detect = TRUE;
  CURLcode result = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->bits.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  सचे(proxy_do_not_proxy(data))
    गच्छ out;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(data, STRING_PROXY);
  सचे(str) {
    सचे(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      सचे(!proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        गच्छ out;
      }
    }
    अञ्ञथा
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(data, STRING_PRE_PROXY);
  सचे(str) {
    सचे(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      सचे(!pre_proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        गच्छ out;
      }
    }
    अञ्ञथा
      do_env_detect = FALSE;
  }

#ifndef CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  सचे(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(data, conn->scheme);
#अञ्ञथा
  (शून्य)do_env_detect;
#endif /* CURL_DISABLE_HTTP */

  सचे(!proxy && !pre_proxy)
    गच्छ out;

  सचे(pre_proxy) {
    result = parse_proxy(data, pre_proxy, TRUE, &conn->socks_proxy);
    सचे(result)
      गच्छ out;
  }

  सचे(proxy) {
    result = parse_proxy(data, proxy, FALSE, &conn->http_proxy);
    सचे(result)
      गच्छ out;

    विकल्प(conn->http_proxy.proxytype) {
    पक्ष CURLPROXY_SOCKS4:
    पक्ष CURLPROXY_SOCKS4A:
    पक्ष CURLPROXY_SOCKS5:
    पक्ष CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      सचे(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(data, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        गच्छ out;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, परिमाण(conn->http_proxy));
      भङ्ग;
    स्वभाव:
      /* all other types are HTTP */
      भङ्ग;
    }
  }

  सचे(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ifdef CURL_DISABLE_HTTP
  सचे(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    result = CURLE_UNSUPPORTED_PROTOCOL;
    गच्छ out;
  }

#अञ्ञथा /* CURL_DISABLE_HTTP */
  सचे(conn->http_proxy.peer) {
    ध्रुव संरचना Curl_scheme *scheme = data->state.origin->scheme;
    bool tunnel_proxy = (bool)data->set.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    सचे(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      सचे(conn->via_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      अञ्ञथा सचे(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      अञ्ञथा सचे(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      अञ्ञथा सचे(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    सचे(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->via_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->bits.origin_is_proxy = TRUE;
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    सचे(!Curl_safecmp(data->state.envproxy, proxy)) {
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
  पटिनिवत्त result;
}

#endif /* CURL_DISABLE_PROXY */
