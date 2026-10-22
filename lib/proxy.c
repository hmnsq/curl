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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "protocol.h"
#അന്തര്ഭാവ "proxy.h"
#അന്തര്ഭാവ "http_proxy.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "curlx/inet_pton.h"
#അന്തര്ഭാവ "curlx/strparse.h"

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_match(സ്ഥിരം അക്ഷരം *ipv4,    /* 1.2.3.4 address */
                          സ്ഥിരം അക്ഷരം *network, /* 1.2.3.4 address */
                          പൂർണ്ണസംഖ്യ bits);
UNITTEST bool cidr4_match(സ്ഥിരം അക്ഷരം *ipv4,    /* 1.2.3.4 address */
                          സ്ഥിരം അക്ഷരം *network, /* 1.2.3.4 address */
                          പൂർണ്ണസംഖ്യ bits)
{
  unsigned പൂർണ്ണസംഖ്യ address = 0;
  unsigned പൂർണ്ണസംഖ്യ check = 0;

  എങ്കിൽ(bits > 32)
    /* strange input */
    തിരികെ FALSE;

  എങ്കിൽ(curlx_inet_pton(AF_INET, ipv4, &address) != 1)
    തിരികെ FALSE;
  എങ്കിൽ(curlx_inet_pton(AF_INET, network, &check) != 1)
    തിരികെ FALSE;

  എങ്കിൽ((bits > 0) && (bits != 32)) {
    unsigned പൂർണ്ണസംഖ്യ mask = 0xffffffff << (32 - bits);
    unsigned പൂർണ്ണസംഖ്യ haddr = htonl(address);
    unsigned പൂർണ്ണസംഖ്യ hcheck = htonl(check);
#എങ്കിൽ 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, network, hcheck, bits, mask,
                  (haddr ^ hcheck) & mask);
#endif
    എങ്കിൽ((haddr ^ hcheck) & mask)
      തിരികെ FALSE;
    തിരികെ TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!bits && !check)
    /* 0.0.0.0/0 matches everything */
    തിരികെ TRUE;

  തിരികെ address == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_match(സ്ഥിരം അക്ഷരം *ipv6, സ്ഥിരം അക്ഷരം *network,
                          പൂർണ്ണസംഖ്യ bits);
UNITTEST bool cidr6_match(സ്ഥിരം അക്ഷരം *ipv6, സ്ഥിരം അക്ഷരം *network,
                          പൂർണ്ണസംഖ്യ bits)
{
#ifdef USE_IPV6
  സ്ഥിതം സ്ഥിരം അക്ഷരം null6[16]= { 0 };
  unsigned പൂർണ്ണസംഖ്യ bytes;
  unsigned പൂർണ്ണസംഖ്യ rest;
  unsigned അക്ഷരം address[16];
  unsigned അക്ഷരം check[16];

  എങ്കിൽ(bits == -1)
    bits = 128;

  bytes = bits / 8;
  rest = bits & 0x07;
  എങ്കിൽ((bytes > 16) || ((bytes == 16) && rest))
    തിരികെ FALSE;
  എങ്കിൽ(curlx_inet_pton(AF_INET6, ipv6, address) != 1)
    തിരികെ FALSE;
  എങ്കിൽ(curlx_inet_pton(AF_INET6, network, check) != 1)
    തിരികെ FALSE;
  എങ്കിൽ((bytes == 16) && !bits && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    തിരികെ TRUE;
  എങ്കിൽ(bytes && memcmp(address, check, bytes))
    തിരികെ FALSE;
  എങ്കിൽ(rest && ((address[bytes] ^ check[bytes]) & (0xff << (8 - rest))))
    തിരികെ FALSE;

  തിരികെ TRUE;
#അല്ലെങ്കിൽ
  (ശൂന്യം)ipv6;
  (ശൂന്യം)network;
  (ശൂന്യം)bits;
  തിരികെ FALSE;
#endif
}

എണ്ണൽ nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

സ്ഥിതം bool match_host(സ്ഥിരം അക്ഷരം *token, size_t tokenlen,
                       സ്ഥിരം അക്ഷരം *name, size_t namelen)
{
  bool match = FALSE;

  /* ignore trailing dots in the token to check */
  എങ്കിൽ(token[tokenlen - 1] == '.')
    tokenlen--;

  എങ്കിൽ(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  എങ്കിൽ(tokenlen == namelen)
    /* case A, exact match */
    match = curl_strnequal(token, name, namelen);
  അല്ലെങ്കിൽ എങ്കിൽ(tokenlen < namelen) {
    /* case B, tailmatch domain */
    match = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  തിരികെ match;
}

സ്ഥിതം bool match_ip(പൂർണ്ണസംഖ്യ type, സ്ഥിരം അക്ഷരം *token, size_t tokenlen,
                     സ്ഥിരം അക്ഷരം *name)
{
  അക്ഷരം *slash;
  പൂർണ്ണസംഖ്യ bits = -1; /* negative means not provided */
  അക്ഷരം checkip[128];
  എങ്കിൽ(tokenlen >= വലുപ്പം(checkip))
    /* this cannot match */
    തിരികെ FALSE;
  /* copy the check name to a temp buffer */
  memcpy(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  എങ്കിൽ(slash) {
    curl_off_t മൂല്യം;
    സ്ഥിരം അക്ഷരം *p = &slash[1];
    എങ്കിൽ(curlx_str_സംഖ്യ(&p, &മൂല്യം, 128) || *p)
      തിരികെ FALSE;
    /* a too large value is rejected in the cidr function below */
    bits = (പൂർണ്ണസംഖ്യ)മൂല്യം;
    *slash = 0; /* null-terminate there */
  }
  എങ്കിൽ(type == TYPE_IPV6)
    തിരികെ cidr6_match(name, checkip, bits);
  അല്ലെങ്കിൽ
    തിരികെ cidr4_match(name, checkip, bits);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *no_proxy);
UNITTEST bool proxy_check_noproxy(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  എങ്കിൽ(!name || name[0] == '\0')
    തിരികെ FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  എങ്കിൽ(no_proxy && no_proxy[0]) {
    സ്ഥിരം അക്ഷരം *p = no_proxy;
    size_t namelen;
    അക്ഷരം address[16];
    എണ്ണൽ nametype type = TYPE_HOST;
    എങ്കിൽ(!strcmp("*", no_proxy))
      തിരികെ TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = strlen(name);
    എങ്കിൽ(curlx_inet_pton(AF_INET, name, &address) == 1)
      type = TYPE_IPV4;
#ifdef USE_IPV6
    അല്ലെങ്കിൽ എങ്കിൽ(curlx_inet_pton(AF_INET6, name, &address) == 1)
      type = TYPE_IPV6;
#endif
    അല്ലെങ്കിൽ {
      /* ignore trailing dots in the hostname */
      എങ്കിൽ(name[namelen - 1] == '.')
        namelen--;
    }

    ആകുമ്പോൾ(*p) {
      സ്ഥിരം അക്ഷരം *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      ആകുമ്പോൾ(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      എങ്കിൽ(tokenlen) {
        bool match = FALSE;
        എങ്കിൽ(type == TYPE_HOST)
          match = match_host(token, tokenlen, name, namelen);
        അല്ലെങ്കിൽ
          match = match_ip(type, token, tokenlen, name);

        എങ്കിൽ(match)
          തിരികെ TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      എങ്കിൽ(*p != ',')
        നിർത്തുക;
      /* pass any number of commas */
      ആകുമ്പോൾ(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  തിരികെ FALSE;
}

#ifndef CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
സ്ഥിതം അക്ഷരം *proxy_detect_proxy(ഘടന Curl_easy *data,
                                സ്ഥിരം ഘടന Curl_scheme *scheme)
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
  സ്ഥിരം അക്ഷരം *env_name = ശൂന്യ;
  അക്ഷരം *proxy = ശൂന്യ;
  അക്ഷരം name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  എങ്കിൽ((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, വലുപ്പം(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    എങ്കിൽ(!proxy) {
      Curl_strntoupper(name_buf, name_buf, വലുപ്പം(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  എങ്കിൽ(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    എങ്കിൽ(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!proxy &&
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

  എങ്കിൽ(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    എങ്കിൽ(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  എങ്കിൽ(proxy)
    infof(data, "Uses proxy env variable %s == '%s'", env_name, proxy);

  തിരികെ proxy;
}
#endif /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
സ്ഥിതം CURLcode parse_proxy(ഘടന Curl_easy *data,
                            സ്ഥിരം അക്ഷരം *proxy,
                            bool for_pre_proxy,
                            ഘടന proxy_info *proxyinfo)
{
  അക്ഷരം *proxyuser = ശൂന്യ;
  അക്ഷരം *proxypasswd = ശൂന്യ;
  അക്ഷരം *scheme = ശൂന്യ;
  CURLcode result = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : data->set.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  എങ്കിൽ(!uhp) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_set(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  എങ്കിൽ(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    എങ്കിൽ(!uc) {
      result = Curl_scheme_to_proxytype(data, scheme, &proxytype, proxy);
      എങ്കിൽ(result)
        പോകുക error;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(uc != CURLUE_NO_SCHEME) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  അല്ലെങ്കിൽ {
    failf(data, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    result = CURLE_COULDNT_RESOLVE_PROXY;
    പോകുക error;
  }

  result = Curl_peer_from_proxy_url(uhp, data, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  എങ്കിൽ(result)
    പോകുക error;

  തിരഞ്ഞെടുപ്പ്(proxytype) {
    അവസ്ഥ CURLPROXY_HTTP:
    അവസ്ഥ CURLPROXY_HTTP_1_0:
    അവസ്ഥ CURLPROXY_HTTPS:
    അവസ്ഥ CURLPROXY_HTTPS2:
    അവസ്ഥ CURLPROXY_HTTPS3:
      എങ്കിൽ(for_pre_proxy) {
        failf(data, "Unsupported pre-proxy type for \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        പോകുക error;
      }
      നിർത്തുക;
    അവസ്ഥ CURLPROXY_SOCKS4:
    അവസ്ഥ CURLPROXY_SOCKS4A:
    അവസ്ഥ CURLPROXY_SOCKS5:
    അവസ്ഥ CURLPROXY_SOCKS5_HOSTNAME:
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      പോകുക error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  എങ്കിൽ(uc && (uc != CURLUE_NO_USER)) {
    result = Curl_uc_to_curlcode(uc);
    പോകുക error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  എങ്കിൽ(uc && (uc != CURLUE_NO_PASSWORD)) {
    result = Curl_uc_to_curlcode(uc);
    പോകുക error;
  }

  എങ്കിൽ(proxyuser || proxypasswd) {
    result = Curl_creds_create(proxyuser, proxypasswd, ശൂന്യ, ശൂന്യ,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    എങ്കിൽ(result)
      പോകുക error;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!for_pre_proxy &&
          (CURL_EASY_STR(data, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(data, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    result = Curl_creds_create(CURL_EASY_STR(data, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(data, STRING_PROXYPASSWORD),
                               ശൂന്യ, ശൂന്യ,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  അല്ലെങ്കിൽ
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ifdef DEBUGBUILD
  എങ്കിൽ(!result) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#endif
  തിരികെ result;
}

/* Is transfer's origin exempted from proxy use? */
സ്ഥിതം bool proxy_do_not_proxy(ഘടന Curl_easy *data)
{
  സ്ഥിരം അക്ഷരം *no_proxy;
  അക്ഷരം *env_no_proxy = ശൂന്യ;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  എങ്കിൽ(data->state.origin->scheme->flags & PROTOPT_NONETWORK)
    തിരികെ TRUE;

  no_proxy = CURL_EASY_STR(data, STRING_NOPROXY);
  എങ്കിൽ(!no_proxy) {
    സ്ഥിരം അക്ഷരം *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    എങ്കിൽ(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    എങ്കിൽ(env_no_proxy)
      infof(data, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(data->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  തിരികെ do_not_proxy;
}

CURLcode Curl_proxy_init_conn(ഘടന Curl_easy *data,
                              ഘടന connectdata *conn)
{
  അക്ഷരം *proxy = ശൂന്യ;
  അക്ഷരം *pre_proxy = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *str = ശൂന്യ;
  bool do_env_detect = TRUE;
  CURLcode result = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->bits.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  എങ്കിൽ(proxy_do_not_proxy(data))
    പോകുക out;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(data, STRING_PROXY);
  എങ്കിൽ(str) {
    എങ്കിൽ(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      എങ്കിൽ(!proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
    }
    അല്ലെങ്കിൽ
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(data, STRING_PRE_PROXY);
  എങ്കിൽ(str) {
    എങ്കിൽ(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      എങ്കിൽ(!pre_proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
    }
    അല്ലെങ്കിൽ
      do_env_detect = FALSE;
  }

#ifndef CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  എങ്കിൽ(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(data, conn->scheme);
#അല്ലെങ്കിൽ
  (ശൂന്യം)do_env_detect;
#endif /* CURL_DISABLE_HTTP */

  എങ്കിൽ(!proxy && !pre_proxy)
    പോകുക out;

  എങ്കിൽ(pre_proxy) {
    result = parse_proxy(data, pre_proxy, TRUE, &conn->socks_proxy);
    എങ്കിൽ(result)
      പോകുക out;
  }

  എങ്കിൽ(proxy) {
    result = parse_proxy(data, proxy, FALSE, &conn->http_proxy);
    എങ്കിൽ(result)
      പോകുക out;

    തിരഞ്ഞെടുപ്പ്(conn->http_proxy.proxytype) {
    അവസ്ഥ CURLPROXY_SOCKS4:
    അവസ്ഥ CURLPROXY_SOCKS4A:
    അവസ്ഥ CURLPROXY_SOCKS5:
    അവസ്ഥ CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      എങ്കിൽ(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(data, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        പോകുക out;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, വലുപ്പം(conn->http_proxy));
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      /* all other types are HTTP */
      നിർത്തുക;
    }
  }

  എങ്കിൽ(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ifdef CURL_DISABLE_HTTP
  എങ്കിൽ(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    result = CURLE_UNSUPPORTED_PROTOCOL;
    പോകുക out;
  }

#അല്ലെങ്കിൽ /* CURL_DISABLE_HTTP */
  എങ്കിൽ(conn->http_proxy.peer) {
    സ്ഥിരം ഘടന Curl_scheme *scheme = data->state.origin->scheme;
    bool tunnel_proxy = (bool)data->set.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    എങ്കിൽ(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      എങ്കിൽ(conn->via_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    എങ്കിൽ(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->via_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->bits.origin_is_proxy = TRUE;
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    എങ്കിൽ(!Curl_safecmp(data->state.envproxy, proxy)) {
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
  തിരികെ result;
}

#endif /* CURL_DISABLE_PROXY */
