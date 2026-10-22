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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "protocol.h"
#அந்தர்பாவ "proxy.h"
#அந்தர்பாவ "http_proxy.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "vauth/vauth.h"
#அந்தர்பாவ "curlx/inet_pton.h"
#அந்தர்பாவ "curlx/strparse.h"

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_match(மாறிலி எழுத்து *ipv4,    /* 1.2.3.4 address */
                          மாறிலி எழுத்து *network, /* 1.2.3.4 address */
                          முழு bits);
UNITTEST bool cidr4_match(மாறிலி எழுத்து *ipv4,    /* 1.2.3.4 address */
                          மாறிலி எழுத்து *network, /* 1.2.3.4 address */
                          முழு bits)
{
  unsigned முழு address = 0;
  unsigned முழு check = 0;

  எனில்(bits > 32)
    /* strange input */
    திரும்பு FALSE;

  எனில்(curlx_inet_pton(AF_INET, ipv4, &address) != 1)
    திரும்பு FALSE;
  எனில்(curlx_inet_pton(AF_INET, network, &check) != 1)
    திரும்பு FALSE;

  எனில்((bits > 0) && (bits != 32)) {
    unsigned முழு mask = 0xffffffff << (32 - bits);
    unsigned முழு haddr = htonl(address);
    unsigned முழு hcheck = htonl(check);
#எனில் 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, network, hcheck, bits, mask,
                  (haddr ^ hcheck) & mask);
#endif
    எனில்((haddr ^ hcheck) & mask)
      திரும்பு FALSE;
    திரும்பு TRUE;
  }
  இல்லையெனில் எனில்(!bits && !check)
    /* 0.0.0.0/0 matches everything */
    திரும்பு TRUE;

  திரும்பு address == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_match(மாறிலி எழுத்து *ipv6, மாறிலி எழுத்து *network,
                          முழு bits);
UNITTEST bool cidr6_match(மாறிலி எழுத்து *ipv6, மாறிலி எழுத்து *network,
                          முழு bits)
{
#ifdef USE_IPV6
  நிலையான மாறிலி எழுத்து null6[16]= { 0 };
  unsigned முழு bytes;
  unsigned முழு rest;
  unsigned எழுத்து address[16];
  unsigned எழுத்து check[16];

  எனில்(bits == -1)
    bits = 128;

  bytes = bits / 8;
  rest = bits & 0x07;
  எனில்((bytes > 16) || ((bytes == 16) && rest))
    திரும்பு FALSE;
  எனில்(curlx_inet_pton(AF_INET6, ipv6, address) != 1)
    திரும்பு FALSE;
  எனில்(curlx_inet_pton(AF_INET6, network, check) != 1)
    திரும்பு FALSE;
  எனில்((bytes == 16) && !bits && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    திரும்பு TRUE;
  எனில்(bytes && memcmp(address, check, bytes))
    திரும்பு FALSE;
  எனில்(rest && ((address[bytes] ^ check[bytes]) & (0xff << (8 - rest))))
    திரும்பு FALSE;

  திரும்பு TRUE;
#இல்லையெனில்
  (வெறுமை)ipv6;
  (வெறுமை)network;
  (வெறுமை)bits;
  திரும்பு FALSE;
#endif
}

எண்ணிக்கை nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

நிலையான bool match_host(மாறிலி எழுத்து *token, size_t tokenlen,
                       மாறிலி எழுத்து *name, size_t namelen)
{
  bool match = FALSE;

  /* ignore trailing dots in the token to check */
  எனில்(token[tokenlen - 1] == '.')
    tokenlen--;

  எனில்(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  எனில்(tokenlen == namelen)
    /* case A, exact match */
    match = curl_strnequal(token, name, namelen);
  இல்லையெனில் எனில்(tokenlen < namelen) {
    /* case B, tailmatch domain */
    match = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  திரும்பு match;
}

நிலையான bool match_ip(முழு type, மாறிலி எழுத்து *token, size_t tokenlen,
                     மாறிலி எழுத்து *name)
{
  எழுத்து *slash;
  முழு bits = -1; /* negative means not provided */
  எழுத்து checkip[128];
  எனில்(tokenlen >= அளவு(checkip))
    /* this cannot match */
    திரும்பு FALSE;
  /* copy the check name to a temp buffer */
  memcpy(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  எனில்(slash) {
    curl_off_t மதிப்பு;
    மாறிலி எழுத்து *p = &slash[1];
    எனில்(curlx_str_எண்(&p, &மதிப்பு, 128) || *p)
      திரும்பு FALSE;
    /* a too large value is rejected in the cidr function below */
    bits = (முழு)மதிப்பு;
    *slash = 0; /* null-terminate there */
  }
  எனில்(type == TYPE_IPV6)
    திரும்பு cidr6_match(name, checkip, bits);
  இல்லையெனில்
    திரும்பு cidr4_match(name, checkip, bits);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(மாறிலி எழுத்து *name, மாறிலி எழுத்து *no_proxy);
UNITTEST bool proxy_check_noproxy(மாறிலி எழுத்து *name, மாறிலி எழுத்து *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  எனில்(!name || name[0] == '\0')
    திரும்பு FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  எனில்(no_proxy && no_proxy[0]) {
    மாறிலி எழுத்து *p = no_proxy;
    size_t namelen;
    எழுத்து address[16];
    எண்ணிக்கை nametype type = TYPE_HOST;
    எனில்(!strcmp("*", no_proxy))
      திரும்பு TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = strlen(name);
    எனில்(curlx_inet_pton(AF_INET, name, &address) == 1)
      type = TYPE_IPV4;
#ifdef USE_IPV6
    இல்லையெனில் எனில்(curlx_inet_pton(AF_INET6, name, &address) == 1)
      type = TYPE_IPV6;
#endif
    இல்லையெனில் {
      /* ignore trailing dots in the hostname */
      எனில்(name[namelen - 1] == '.')
        namelen--;
    }

    போது(*p) {
      மாறிலி எழுத்து *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      போது(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      எனில்(tokenlen) {
        bool match = FALSE;
        எனில்(type == TYPE_HOST)
          match = match_host(token, tokenlen, name, namelen);
        இல்லையெனில்
          match = match_ip(type, token, tokenlen, name);

        எனில்(match)
          திரும்பு TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      எனில்(*p != ',')
        நிறுத்து;
      /* pass any number of commas */
      போது(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  திரும்பு FALSE;
}

#ifndef CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
நிலையான எழுத்து *proxy_detect_proxy(கட்டமைப்பு Curl_easy *data,
                                மாறிலி கட்டமைப்பு Curl_scheme *scheme)
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
  மாறிலி எழுத்து *env_name = வெற்று;
  எழுத்து *proxy = வெற்று;
  எழுத்து name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  எனில்((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, அளவு(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    எனில்(!proxy) {
      Curl_strntoupper(name_buf, name_buf, அளவு(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  எனில்(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    எனில்(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  இல்லையெனில் எனில்(!proxy &&
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

  எனில்(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    எனில்(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  எனில்(proxy)
    infof(data, "Uses proxy env variable %s == '%s'", env_name, proxy);

  திரும்பு proxy;
}
#endif /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
நிலையான CURLcode parse_proxy(கட்டமைப்பு Curl_easy *data,
                            மாறிலி எழுத்து *proxy,
                            bool for_pre_proxy,
                            கட்டமைப்பு proxy_info *proxyinfo)
{
  எழுத்து *proxyuser = வெற்று;
  எழுத்து *proxypasswd = வெற்று;
  எழுத்து *scheme = வெற்று;
  CURLcode result = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : data->set.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  எனில்(!uhp) {
    result = CURLE_OUT_OF_MEMORY;
    செல் error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_set(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  எனில்(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    எனில்(!uc) {
      result = Curl_scheme_to_proxytype(data, scheme, &proxytype, proxy);
      எனில்(result)
        செல் error;
    }
    இல்லையெனில் எனில்(uc != CURLUE_NO_SCHEME) {
      result = CURLE_OUT_OF_MEMORY;
      செல் error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  இல்லையெனில் {
    failf(data, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    result = CURLE_COULDNT_RESOLVE_PROXY;
    செல் error;
  }

  result = Curl_peer_from_proxy_url(uhp, data, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  எனில்(result)
    செல் error;

  தேர்வு(proxytype) {
    நிலை CURLPROXY_HTTP:
    நிலை CURLPROXY_HTTP_1_0:
    நிலை CURLPROXY_HTTPS:
    நிலை CURLPROXY_HTTPS2:
    நிலை CURLPROXY_HTTPS3:
      எனில்(for_pre_proxy) {
        failf(data, "Unsupported pre-proxy type for \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        செல் error;
      }
      நிறுத்து;
    நிலை CURLPROXY_SOCKS4:
    நிலை CURLPROXY_SOCKS4A:
    நிலை CURLPROXY_SOCKS5:
    நிலை CURLPROXY_SOCKS5_HOSTNAME:
      நிறுத்து;
    இயல்பு:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      செல் error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  எனில்(uc && (uc != CURLUE_NO_USER)) {
    result = Curl_uc_to_curlcode(uc);
    செல் error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  எனில்(uc && (uc != CURLUE_NO_PASSWORD)) {
    result = Curl_uc_to_curlcode(uc);
    செல் error;
  }

  எனில்(proxyuser || proxypasswd) {
    result = Curl_creds_create(proxyuser, proxypasswd, வெற்று, வெற்று,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    எனில்(result)
      செல் error;
  }
  இல்லையெனில் எனில்(!for_pre_proxy &&
          (CURL_EASY_STR(data, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(data, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    result = Curl_creds_create(CURL_EASY_STR(data, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(data, STRING_PROXYPASSWORD),
                               வெற்று, வெற்று,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  இல்லையெனில்
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ifdef DEBUGBUILD
  எனில்(!result) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#endif
  திரும்பு result;
}

/* Is transfer's origin exempted from proxy use? */
நிலையான bool proxy_do_not_proxy(கட்டமைப்பு Curl_easy *data)
{
  மாறிலி எழுத்து *no_proxy;
  எழுத்து *env_no_proxy = வெற்று;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  எனில்(data->state.origin->scheme->flags & PROTOPT_NONETWORK)
    திரும்பு TRUE;

  no_proxy = CURL_EASY_STR(data, STRING_NOPROXY);
  எனில்(!no_proxy) {
    மாறிலி எழுத்து *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    எனில்(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    எனில்(env_no_proxy)
      infof(data, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(data->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  திரும்பு do_not_proxy;
}

CURLcode Curl_proxy_init_conn(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு connectdata *conn)
{
  எழுத்து *proxy = வெற்று;
  எழுத்து *pre_proxy = வெற்று;
  மாறிலி எழுத்து *str = வெற்று;
  bool do_env_detect = TRUE;
  CURLcode result = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->bits.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  எனில்(proxy_do_not_proxy(data))
    செல் out;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(data, STRING_PROXY);
  எனில்(str) {
    எனில்(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      எனில்(!proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
    }
    இல்லையெனில்
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(data, STRING_PRE_PROXY);
  எனில்(str) {
    எனில்(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      எனில்(!pre_proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
    }
    இல்லையெனில்
      do_env_detect = FALSE;
  }

#ifndef CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  எனில்(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(data, conn->scheme);
#இல்லையெனில்
  (வெறுமை)do_env_detect;
#endif /* CURL_DISABLE_HTTP */

  எனில்(!proxy && !pre_proxy)
    செல் out;

  எனில்(pre_proxy) {
    result = parse_proxy(data, pre_proxy, TRUE, &conn->socks_proxy);
    எனில்(result)
      செல் out;
  }

  எனில்(proxy) {
    result = parse_proxy(data, proxy, FALSE, &conn->http_proxy);
    எனில்(result)
      செல் out;

    தேர்வு(conn->http_proxy.proxytype) {
    நிலை CURLPROXY_SOCKS4:
    நிலை CURLPROXY_SOCKS4A:
    நிலை CURLPROXY_SOCKS5:
    நிலை CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      எனில்(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(data, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        செல் out;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, அளவு(conn->http_proxy));
      நிறுத்து;
    இயல்பு:
      /* all other types are HTTP */
      நிறுத்து;
    }
  }

  எனில்(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ifdef CURL_DISABLE_HTTP
  எனில்(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    result = CURLE_UNSUPPORTED_PROTOCOL;
    செல் out;
  }

#இல்லையெனில் /* CURL_DISABLE_HTTP */
  எனில்(conn->http_proxy.peer) {
    மாறிலி கட்டமைப்பு Curl_scheme *scheme = data->state.origin->scheme;
    bool tunnel_proxy = (bool)data->set.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    எனில்(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      எனில்(conn->via_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      இல்லையெனில் எனில்(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      இல்லையெனில் எனில்(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      இல்லையெனில் எனில்(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    எனில்(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->via_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->bits.origin_is_proxy = TRUE;
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    எனில்(!Curl_safecmp(data->state.envproxy, proxy)) {
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
  திரும்பு result;
}

#endif /* CURL_DISABLE_PROXY */
