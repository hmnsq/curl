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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "protocol.h"
#ଅନ୍ତର୍ଭାଵ "proxy.h"
#ଅନ୍ତର୍ଭାଵ "http_proxy.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_pton.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_match(ଅଚଳ ଅକ୍ଷର *ipv4,    /* 1.2.3.4 address */
                          ଅଚଳ ଅକ୍ଷର *network, /* 1.2.3.4 address */
                          ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits);
UNITTEST bool cidr4_match(ଅଚଳ ଅକ୍ଷର *ipv4,    /* 1.2.3.4 address */
                          ଅଚଳ ଅକ୍ଷର *network, /* 1.2.3.4 address */
                          ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା address = 0;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା check = 0;

  ଯଦି(bits > 32)
    /* strange input */
    ଫେରନ୍ତୁ FALSE;

  ଯଦି(curlx_inet_pton(AF_INET, ipv4, &address) != 1)
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(curlx_inet_pton(AF_INET, network, &check) != 1)
    ଫେରନ୍ତୁ FALSE;

  ଯଦି((bits > 0) && (bits != 32)) {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask = 0xffffffff << (32 - bits);
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା haddr = htonl(address);
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hcheck = htonl(check);
#ଯଦି 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, network, hcheck, bits, mask,
                  (haddr ^ hcheck) & mask);
#endif
    ଯଦି((haddr ^ hcheck) & mask)
      ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(!bits && !check)
    /* 0.0.0.0/0 matches everything */
    ଫେରନ୍ତୁ TRUE;

  ଫେରନ୍ତୁ address == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_match(ଅଚଳ ଅକ୍ଷର *ipv6, ଅଚଳ ଅକ୍ଷର *network,
                          ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits);
UNITTEST bool cidr6_match(ଅଚଳ ଅକ୍ଷର *ipv6, ଅଚଳ ଅକ୍ଷର *network,
                          ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits)
{
#ifdef USE_IPV6
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର null6[16]= { 0 };
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytes;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା rest;
  unsigned ଅକ୍ଷର address[16];
  unsigned ଅକ୍ଷର check[16];

  ଯଦି(bits == -1)
    bits = 128;

  bytes = bits / 8;
  rest = bits & 0x07;
  ଯଦି((bytes > 16) || ((bytes == 16) && rest))
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(curlx_inet_pton(AF_INET6, ipv6, address) != 1)
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(curlx_inet_pton(AF_INET6, network, check) != 1)
    ଫେରନ୍ତୁ FALSE;
  ଯଦି((bytes == 16) && !bits && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    ଫେରନ୍ତୁ TRUE;
  ଯଦି(bytes && memcmp(address, check, bytes))
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(rest && ((address[bytes] ^ check[bytes]) & (0xff << (8 - rest))))
    ଫେରନ୍ତୁ FALSE;

  ଫେରନ୍ତୁ TRUE;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)ipv6;
  (ଶୂନ୍ୟ)network;
  (ଶୂନ୍ୟ)bits;
  ଫେରନ୍ତୁ FALSE;
#endif
}

ଗଣନା nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

ସ୍ଥିର bool match_host(ଅଚଳ ଅକ୍ଷର *token, size_t tokenlen,
                       ଅଚଳ ଅକ୍ଷର *name, size_t namelen)
{
  bool match = FALSE;

  /* ignore trailing dots in the token to check */
  ଯଦି(token[tokenlen - 1] == '.')
    tokenlen--;

  ଯଦି(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  ଯଦି(tokenlen == namelen)
    /* case A, exact match */
    match = curl_strnequal(token, name, namelen);
  ଅନ୍ୟଥା ଯଦି(tokenlen < namelen) {
    /* case B, tailmatch domain */
    match = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  ଫେରନ୍ତୁ match;
}

ସ୍ଥିର bool match_ip(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଅଚଳ ଅକ୍ଷର *token, size_t tokenlen,
                     ଅଚଳ ଅକ୍ଷର *name)
{
  ଅକ୍ଷର *slash;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits = -1; /* negative means not provided */
  ଅକ୍ଷର checkip[128];
  ଯଦି(tokenlen >= ଆକାର(checkip))
    /* this cannot match */
    ଫେରନ୍ତୁ FALSE;
  /* copy the check name to a temp buffer */
  memcpy(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  ଯଦି(slash) {
    curl_off_t ମୂଲ୍ୟ;
    ଅଚଳ ଅକ୍ଷର *p = &slash[1];
    ଯଦି(curlx_str_ସଂଖ୍ୟା(&p, &ମୂଲ୍ୟ, 128) || *p)
      ଫେରନ୍ତୁ FALSE;
    /* a too large value is rejected in the cidr function below */
    bits = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ମୂଲ୍ୟ;
    *slash = 0; /* null-terminate there */
  }
  ଯଦି(type == TYPE_IPV6)
    ଫେରନ୍ତୁ cidr6_match(name, checkip, bits);
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ cidr4_match(name, checkip, bits);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *no_proxy);
UNITTEST bool proxy_check_noproxy(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  ଯଦି(!name || name[0] == '\0')
    ଫେରନ୍ତୁ FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  ଯଦି(no_proxy && no_proxy[0]) {
    ଅଚଳ ଅକ୍ଷର *p = no_proxy;
    size_t namelen;
    ଅକ୍ଷର address[16];
    ଗଣନା nametype type = TYPE_HOST;
    ଯଦି(!strcmp("*", no_proxy))
      ଫେରନ୍ତୁ TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = strlen(name);
    ଯଦି(curlx_inet_pton(AF_INET, name, &address) == 1)
      type = TYPE_IPV4;
#ifdef USE_IPV6
    ଅନ୍ୟଥା ଯଦି(curlx_inet_pton(AF_INET6, name, &address) == 1)
      type = TYPE_IPV6;
#endif
    ଅନ୍ୟଥା {
      /* ignore trailing dots in the hostname */
      ଯଦି(name[namelen - 1] == '.')
        namelen--;
    }

    ଯେତେବେଳେ(*p) {
      ଅଚଳ ଅକ୍ଷର *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      ଯେତେବେଳେ(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      ଯଦି(tokenlen) {
        bool match = FALSE;
        ଯଦି(type == TYPE_HOST)
          match = match_host(token, tokenlen, name, namelen);
        ଅନ୍ୟଥା
          match = match_ip(type, token, tokenlen, name);

        ଯଦି(match)
          ଫେରନ୍ତୁ TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      ଯଦି(*p != ',')
        ଭାଙ୍ଗ;
      /* pass any number of commas */
      ଯେତେବେଳେ(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  ଫେରନ୍ତୁ FALSE;
}

#ifndef CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
ସ୍ଥିର ଅକ୍ଷର *proxy_detect_proxy(ସଂରଚନା Curl_easy *data,
                                ଅଚଳ ସଂରଚନା Curl_scheme *scheme)
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
  ଅଚଳ ଅକ୍ଷର *env_name = ଖାଲି;
  ଅକ୍ଷର *proxy = ଖାଲି;
  ଅକ୍ଷର name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  ଯଦି((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, ଆକାର(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    ଯଦି(!proxy) {
      Curl_strntoupper(name_buf, name_buf, ଆକାର(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  ଯଦି(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    ଯଦି(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  ଅନ୍ୟଥା ଯଦି(!proxy &&
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

  ଯଦି(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    ଯଦି(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  ଯଦି(proxy)
    infof(data, "Uses proxy env variable %s == '%s'", env_name, proxy);

  ଫେରନ୍ତୁ proxy;
}
#endif /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
ସ୍ଥିର CURLcode parse_proxy(ସଂରଚନା Curl_easy *data,
                            ଅଚଳ ଅକ୍ଷର *proxy,
                            bool for_pre_proxy,
                            ସଂରଚନା proxy_info *proxyinfo)
{
  ଅକ୍ଷର *proxyuser = ଖାଲି;
  ଅକ୍ଷର *proxypasswd = ଖାଲି;
  ଅକ୍ଷର *scheme = ଖାଲି;
  CURLcode result = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : data->set.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  ଯଦି(!uhp) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_set(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  ଯଦି(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    ଯଦି(!uc) {
      result = Curl_scheme_to_proxytype(data, scheme, &proxytype, proxy);
      ଯଦି(result)
        ଯାଅ error;
    }
    ଅନ୍ୟଥା ଯଦି(uc != CURLUE_NO_SCHEME) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  ଅନ୍ୟଥା {
    failf(data, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    result = CURLE_COULDNT_RESOLVE_PROXY;
    ଯାଅ error;
  }

  result = Curl_peer_from_proxy_url(uhp, data, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  ଯଦି(result)
    ଯାଅ error;

  ବାଛ(proxytype) {
    ସ୍ଥିତି CURLPROXY_HTTP:
    ସ୍ଥିତି CURLPROXY_HTTP_1_0:
    ସ୍ଥିତି CURLPROXY_HTTPS:
    ସ୍ଥିତି CURLPROXY_HTTPS2:
    ସ୍ଥିତି CURLPROXY_HTTPS3:
      ଯଦି(for_pre_proxy) {
        failf(data, "Unsupported pre-proxy type for \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        ଯାଅ error;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURLPROXY_SOCKS4:
    ସ୍ଥିତି CURLPROXY_SOCKS4A:
    ସ୍ଥିତି CURLPROXY_SOCKS5:
    ସ୍ଥିତି CURLPROXY_SOCKS5_HOSTNAME:
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      ଯାଅ error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  ଯଦି(uc && (uc != CURLUE_NO_USER)) {
    result = Curl_uc_to_curlcode(uc);
    ଯାଅ error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  ଯଦି(uc && (uc != CURLUE_NO_PASSWORD)) {
    result = Curl_uc_to_curlcode(uc);
    ଯାଅ error;
  }

  ଯଦି(proxyuser || proxypasswd) {
    result = Curl_creds_create(proxyuser, proxypasswd, ଖାଲି, ଖାଲି,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    ଯଦି(result)
      ଯାଅ error;
  }
  ଅନ୍ୟଥା ଯଦି(!for_pre_proxy &&
          (CURL_EASY_STR(data, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(data, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    result = Curl_creds_create(CURL_EASY_STR(data, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(data, STRING_PROXYPASSWORD),
                               ଖାଲି, ଖାଲି,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  ଅନ୍ୟଥା
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ifdef DEBUGBUILD
  ଯଦି(!result) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#endif
  ଫେରନ୍ତୁ result;
}

/* Is transfer's origin exempted from proxy use? */
ସ୍ଥିର bool proxy_do_not_proxy(ସଂରଚନା Curl_easy *data)
{
  ଅଚଳ ଅକ୍ଷର *no_proxy;
  ଅକ୍ଷର *env_no_proxy = ଖାଲି;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  ଯଦି(data->state.origin->scheme->flags & PROTOPT_NONETWORK)
    ଫେରନ୍ତୁ TRUE;

  no_proxy = CURL_EASY_STR(data, STRING_NOPROXY);
  ଯଦି(!no_proxy) {
    ଅଚଳ ଅକ୍ଷର *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    ଯଦି(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    ଯଦି(env_no_proxy)
      infof(data, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(data->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  ଫେରନ୍ତୁ do_not_proxy;
}

CURLcode Curl_proxy_init_conn(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା connectdata *conn)
{
  ଅକ୍ଷର *proxy = ଖାଲି;
  ଅକ୍ଷର *pre_proxy = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *str = ଖାଲି;
  bool do_env_detect = TRUE;
  CURLcode result = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->bits.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  ଯଦି(proxy_do_not_proxy(data))
    ଯାଅ out;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(data, STRING_PROXY);
  ଯଦି(str) {
    ଯଦି(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      ଯଦି(!proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
    }
    ଅନ୍ୟଥା
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(data, STRING_PRE_PROXY);
  ଯଦି(str) {
    ଯଦି(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      ଯଦି(!pre_proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
    }
    ଅନ୍ୟଥା
      do_env_detect = FALSE;
  }

#ifndef CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  ଯଦି(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(data, conn->scheme);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)do_env_detect;
#endif /* CURL_DISABLE_HTTP */

  ଯଦି(!proxy && !pre_proxy)
    ଯାଅ out;

  ଯଦି(pre_proxy) {
    result = parse_proxy(data, pre_proxy, TRUE, &conn->socks_proxy);
    ଯଦି(result)
      ଯାଅ out;
  }

  ଯଦି(proxy) {
    result = parse_proxy(data, proxy, FALSE, &conn->http_proxy);
    ଯଦି(result)
      ଯାଅ out;

    ବାଛ(conn->http_proxy.proxytype) {
    ସ୍ଥିତି CURLPROXY_SOCKS4:
    ସ୍ଥିତି CURLPROXY_SOCKS4A:
    ସ୍ଥିତି CURLPROXY_SOCKS5:
    ସ୍ଥିତି CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      ଯଦି(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(data, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        ଯାଅ out;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, ଆକାର(conn->http_proxy));
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      /* all other types are HTTP */
      ଭାଙ୍ଗ;
    }
  }

  ଯଦି(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ifdef CURL_DISABLE_HTTP
  ଯଦି(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ଯାଅ out;
  }

#ଅନ୍ୟଥା /* CURL_DISABLE_HTTP */
  ଯଦି(conn->http_proxy.peer) {
    ଅଚଳ ସଂରଚନା Curl_scheme *scheme = data->state.origin->scheme;
    bool tunnel_proxy = (bool)data->set.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    ଯଦି(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      ଯଦି(conn->via_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      ଅନ୍ୟଥା ଯଦି(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      ଅନ୍ୟଥା ଯଦି(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      ଅନ୍ୟଥା ଯଦି(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    ଯଦି(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->via_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->bits.origin_is_proxy = TRUE;
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    ଯଦି(!Curl_safecmp(data->state.envproxy, proxy)) {
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
  ଫେରନ୍ତୁ result;
}

#endif /* CURL_DISABLE_PROXY */
