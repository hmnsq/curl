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
UNITTEST bool cidr4_match(स्थिरांक अक्षर *ipv4,    /* 1.2.3.4 address */
                          स्थिरांक अक्षर *network, /* 1.2.3.4 address */
                          पूर्णांक bits);
UNITTEST bool cidr4_match(स्थिरांक अक्षर *ipv4,    /* 1.2.3.4 address */
                          स्थिरांक अक्षर *network, /* 1.2.3.4 address */
                          पूर्णांक bits)
{
  अचिन्हित पूर्णांक address = 0;
  अचिन्हित पूर्णांक check = 0;

  जर(bits > 32)
    /* strange input */
    परत FALSE;

  जर(curlx_inet_pton(AF_INET, ipv4, &address) != 1)
    परत FALSE;
  जर(curlx_inet_pton(AF_INET, network, &check) != 1)
    परत FALSE;

  जर((bits > 0) && (bits != 32)) {
    अचिन्हित पूर्णांक mask = 0xffffffff << (32 - bits);
    अचिन्हित पूर्णांक haddr = htonl(address);
    अचिन्हित पूर्णांक hcheck = htonl(check);
#जर 0
    curl_mfprintf(मानक_त्रुटि, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, network, hcheck, bits, mask,
                  (haddr ^ hcheck) & mask);
#endif
    जर((haddr ^ hcheck) & mask)
      परत FALSE;
    परत TRUE;
  }
  अन्यथा जर(!bits && !check)
    /* 0.0.0.0/0 matches everything */
    परत TRUE;

  परत address == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_match(स्थिरांक अक्षर *ipv6, स्थिरांक अक्षर *network,
                          पूर्णांक bits);
UNITTEST bool cidr6_match(स्थिरांक अक्षर *ipv6, स्थिरांक अक्षर *network,
                          पूर्णांक bits)
{
#ifdef USE_IPV6
  स्थिर स्थिरांक अक्षर null6[16]= { 0 };
  अचिन्हित पूर्णांक bytes;
  अचिन्हित पूर्णांक rest;
  अचिन्हित अक्षर address[16];
  अचिन्हित अक्षर check[16];

  जर(bits == -1)
    bits = 128;

  bytes = bits / 8;
  rest = bits & 0x07;
  जर((bytes > 16) || ((bytes == 16) && rest))
    परत FALSE;
  जर(curlx_inet_pton(AF_INET6, ipv6, address) != 1)
    परत FALSE;
  जर(curlx_inet_pton(AF_INET6, network, check) != 1)
    परत FALSE;
  जर((bytes == 16) && !bits && !स_भेद(null6, check, 16))
    /* ::/0 matches everything */
    परत TRUE;
  जर(bytes && स_भेद(address, check, bytes))
    परत FALSE;
  जर(rest && ((address[bytes] ^ check[bytes]) & (0xff << (8 - rest))))
    परत FALSE;

  परत TRUE;
#अन्यथा
  (शून्य)ipv6;
  (शून्य)network;
  (शून्य)bits;
  परत FALSE;
#endif
}

गणना nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

स्थिर bool match_host(स्थिरांक अक्षर *token, माप_प्रकार tokenlen,
                       स्थिरांक अक्षर *name, माप_प्रकार namelen)
{
  bool match = FALSE;

  /* ignore trailing dots in the token to check */
  जर(token[tokenlen - 1] == '.')
    tokenlen--;

  जर(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  जर(tokenlen == namelen)
    /* case A, exact match */
    match = curl_strnequal(token, name, namelen);
  अन्यथा जर(tokenlen < namelen) {
    /* case B, tailmatch domain */
    match = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  परत match;
}

स्थिर bool match_ip(पूर्णांक type, स्थिरांक अक्षर *token, माप_प्रकार tokenlen,
                     स्थिरांक अक्षर *name)
{
  अक्षर *slash;
  पूर्णांक bits = -1; /* negative means not provided */
  अक्षर checkip[128];
  जर(tokenlen >= आकार(checkip))
    /* this cannot match */
    परत FALSE;
  /* copy the check name to a temp buffer */
  स_नकल(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = म_अक्षर(checkip, '/');
  /* if the slash is part of this token, use it */
  जर(slash) {
    curl_off_t मूल्य;
    स्थिरांक अक्षर *p = &slash[1];
    जर(curlx_str_संख्या(&p, &मूल्य, 128) || *p)
      परत FALSE;
    /* a too large value is rejected in the cidr function below */
    bits = (पूर्णांक)मूल्य;
    *slash = 0; /* null-terminate there */
  }
  जर(type == TYPE_IPV6)
    परत cidr6_match(name, checkip, bits);
  अन्यथा
    परत cidr4_match(name, checkip, bits);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(स्थिरांक अक्षर *name, स्थिरांक अक्षर *no_proxy);
UNITTEST bool proxy_check_noproxy(स्थिरांक अक्षर *name, स्थिरांक अक्षर *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  जर(!name || name[0] == '\0')
    परत FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  जर(no_proxy && no_proxy[0]) {
    स्थिरांक अक्षर *p = no_proxy;
    माप_प्रकार namelen;
    अक्षर address[16];
    गणना nametype type = TYPE_HOST;
    जर(!म_भेद("*", no_proxy))
      परत TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = म_माप(name);
    जर(curlx_inet_pton(AF_INET, name, &address) == 1)
      type = TYPE_IPV4;
#ifdef USE_IPV6
    अन्यथा जर(curlx_inet_pton(AF_INET6, name, &address) == 1)
      type = TYPE_IPV6;
#endif
    अन्यथा {
      /* ignore trailing dots in the hostname */
      जर(name[namelen - 1] == '.')
        namelen--;
    }

    पर्यंत(*p) {
      स्थिरांक अक्षर *token;
      माप_प्रकार tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      पर्यंत(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      जर(tokenlen) {
        bool match = FALSE;
        जर(type == TYPE_HOST)
          match = match_host(token, tokenlen, name, namelen);
        अन्यथा
          match = match_ip(type, token, tokenlen, name);

        जर(match)
          परत TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      जर(*p != ',')
        थांबा;
      /* pass any number of commas */
      पर्यंत(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  परत FALSE;
}

#ifndef CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
स्थिर अक्षर *proxy_detect_proxy(संरचना Curl_easy *data,
                                स्थिरांक संरचना Curl_scheme *scheme)
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
  स्थिरांक अक्षर *env_name = रिक्त;
  अक्षर *proxy = रिक्त;
  अक्षर name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  जर((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, आकार(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    जर(!proxy) {
      Curl_strntoupper(name_buf, name_buf, आकार(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  जर(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    जर(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  अन्यथा जर(!proxy &&
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

  जर(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    जर(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  जर(proxy)
    infof(data, "Uses proxy env variable %s == '%s'", env_name, proxy);

  परत proxy;
}
#endif /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
स्थिर CURLcode parse_proxy(संरचना Curl_easy *data,
                            स्थिरांक अक्षर *proxy,
                            bool for_pre_proxy,
                            संरचना proxy_info *proxyinfo)
{
  अक्षर *proxyuser = रिक्त;
  अक्षर *proxypasswd = रिक्त;
  अक्षर *scheme = रिक्त;
  CURLcode result = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : data->set.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  जर(!uhp) {
    result = CURLE_OUT_OF_MEMORY;
    जा error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_set(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  जर(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    जर(!uc) {
      result = Curl_scheme_to_proxytype(data, scheme, &proxytype, proxy);
      जर(result)
        जा error;
    }
    अन्यथा जर(uc != CURLUE_NO_SCHEME) {
      result = CURLE_OUT_OF_MEMORY;
      जा error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  अन्यथा {
    failf(data, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    result = CURLE_COULDNT_RESOLVE_PROXY;
    जा error;
  }

  result = Curl_peer_from_proxy_url(uhp, data, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  जर(result)
    जा error;

  निवड(proxytype) {
    स्थिती CURLPROXY_HTTP:
    स्थिती CURLPROXY_HTTP_1_0:
    स्थिती CURLPROXY_HTTPS:
    स्थिती CURLPROXY_HTTPS2:
    स्थिती CURLPROXY_HTTPS3:
      जर(for_pre_proxy) {
        failf(data, "Unsupported pre-proxy type for \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        जा error;
      }
      थांबा;
    स्थिती CURLPROXY_SOCKS4:
    स्थिती CURLPROXY_SOCKS4A:
    स्थिती CURLPROXY_SOCKS5:
    स्थिती CURLPROXY_SOCKS5_HOSTNAME:
      थांबा;
    पूर्वनिर्धारित:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      जा error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  जर(uc && (uc != CURLUE_NO_USER)) {
    result = Curl_uc_to_curlcode(uc);
    जा error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  जर(uc && (uc != CURLUE_NO_PASSWORD)) {
    result = Curl_uc_to_curlcode(uc);
    जा error;
  }

  जर(proxyuser || proxypasswd) {
    result = Curl_creds_create(proxyuser, proxypasswd, रिक्त, रिक्त,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    जर(result)
      जा error;
  }
  अन्यथा जर(!for_pre_proxy &&
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
  अन्यथा
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ifdef DEBUGBUILD
  जर(!result) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#endif
  परत result;
}

/* Is transfer's origin exempted from proxy use? */
स्थिर bool proxy_do_not_proxy(संरचना Curl_easy *data)
{
  स्थिरांक अक्षर *no_proxy;
  अक्षर *env_no_proxy = रिक्त;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  जर(data->state.origin->scheme->flags & PROTOPT_NONETWORK)
    परत TRUE;

  no_proxy = CURL_EASY_STR(data, STRING_NOPROXY);
  जर(!no_proxy) {
    स्थिरांक अक्षर *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    जर(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    जर(env_no_proxy)
      infof(data, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(data->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  परत do_not_proxy;
}

CURLcode Curl_proxy_init_conn(संरचना Curl_easy *data,
                              संरचना connectdata *conn)
{
  अक्षर *proxy = रिक्त;
  अक्षर *pre_proxy = रिक्त;
  स्थिरांक अक्षर *str = रिक्त;
  bool do_env_detect = TRUE;
  CURLcode result = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->bits.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  जर(proxy_do_not_proxy(data))
    जा out;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(data, STRING_PROXY);
  जर(str) {
    जर(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      जर(!proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        जा out;
      }
    }
    अन्यथा
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(data, STRING_PRE_PROXY);
  जर(str) {
    जर(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      जर(!pre_proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        जा out;
      }
    }
    अन्यथा
      do_env_detect = FALSE;
  }

#ifndef CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  जर(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(data, conn->scheme);
#अन्यथा
  (शून्य)do_env_detect;
#endif /* CURL_DISABLE_HTTP */

  जर(!proxy && !pre_proxy)
    जा out;

  जर(pre_proxy) {
    result = parse_proxy(data, pre_proxy, TRUE, &conn->socks_proxy);
    जर(result)
      जा out;
  }

  जर(proxy) {
    result = parse_proxy(data, proxy, FALSE, &conn->http_proxy);
    जर(result)
      जा out;

    निवड(conn->http_proxy.proxytype) {
    स्थिती CURLPROXY_SOCKS4:
    स्थिती CURLPROXY_SOCKS4A:
    स्थिती CURLPROXY_SOCKS5:
    स्थिती CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      जर(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(data, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        जा out;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      स_रखो(&conn->http_proxy, 0, आकार(conn->http_proxy));
      थांबा;
    पूर्वनिर्धारित:
      /* all other types are HTTP */
      थांबा;
    }
  }

  जर(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ifdef CURL_DISABLE_HTTP
  जर(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    result = CURLE_UNSUPPORTED_PROTOCOL;
    जा out;
  }

#अन्यथा /* CURL_DISABLE_HTTP */
  जर(conn->http_proxy.peer) {
    स्थिरांक संरचना Curl_scheme *scheme = data->state.origin->scheme;
    bool tunnel_proxy = (bool)data->set.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    जर(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      जर(conn->via_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      अन्यथा जर(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      अन्यथा जर(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      अन्यथा जर(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    जर(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->via_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->bits.origin_is_proxy = TRUE;
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    जर(!Curl_safecmp(data->state.envproxy, proxy)) {
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
  परत result;
}

#endif /* CURL_DISABLE_PROXY */
