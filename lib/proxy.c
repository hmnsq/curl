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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "protocol.h"
#ಅನ್ತರ್ಭಾವ "proxy.h"
#ಅನ್ತರ್ಭಾವ "http_proxy.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_match(ಧ್ರುವ ವರ್ಣ *ipv4,    /* 1.2.3.4 address */
                          ಧ್ರುವ ವರ್ಣ *network, /* 1.2.3.4 address */
                          ಪೂರ್ಣಾಙ್ಕ bits);
UNITTEST bool cidr4_match(ಧ್ರುವ ವರ್ಣ *ipv4,    /* 1.2.3.4 address */
                          ಧ್ರುವ ವರ್ಣ *network, /* 1.2.3.4 address */
                          ಪೂರ್ಣಾಙ್ಕ bits)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ address = 0;
  unsigned ಪೂರ್ಣಾಙ್ಕ check = 0;

  ಯದಿ(bits > 32)
    /* strange input */
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಯದಿ(curlx_inet_pton(AF_INET, ipv4, &address) != 1)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(curlx_inet_pton(AF_INET, network, &check) != 1)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಯದಿ((bits > 0) && (bits != 32)) {
    unsigned ಪೂರ್ಣಾಙ್ಕ mask = 0xffffffff << (32 - bits);
    unsigned ಪೂರ್ಣಾಙ್ಕ haddr = htonl(address);
    unsigned ಪೂರ್ಣಾಙ್ಕ hcheck = htonl(check);
#ಯದಿ 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, network, hcheck, bits, mask,
                  (haddr ^ hcheck) & mask);
#endif
    ಯದಿ((haddr ^ hcheck) & mask)
      ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(!bits && !check)
    /* 0.0.0.0/0 matches everything */
    ಪ್ರತ್ಯಾಗಮ TRUE;

  ಪ್ರತ್ಯಾಗಮ address == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_match(ಧ್ರುವ ವರ್ಣ *ipv6, ಧ್ರುವ ವರ್ಣ *network,
                          ಪೂರ್ಣಾಙ್ಕ bits);
UNITTEST bool cidr6_match(ಧ್ರುವ ವರ್ಣ *ipv6, ಧ್ರುವ ವರ್ಣ *network,
                          ಪೂರ್ಣಾಙ್ಕ bits)
{
#ifdef USE_IPV6
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ null6[16]= { 0 };
  unsigned ಪೂರ್ಣಾಙ್ಕ bytes;
  unsigned ಪೂರ್ಣಾಙ್ಕ rest;
  unsigned ವರ್ಣ address[16];
  unsigned ವರ್ಣ check[16];

  ಯದಿ(bits == -1)
    bits = 128;

  bytes = bits / 8;
  rest = bits & 0x07;
  ಯದಿ((bytes > 16) || ((bytes == 16) && rest))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(curlx_inet_pton(AF_INET6, ipv6, address) != 1)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(curlx_inet_pton(AF_INET6, network, check) != 1)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ((bytes == 16) && !bits && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಯದಿ(bytes && memcmp(address, check, bytes))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(rest && ((address[bytes] ^ check[bytes]) & (0xff << (8 - rest))))
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಪ್ರತ್ಯಾಗಮ TRUE;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)ipv6;
  (ಶೂನ್ಯ)network;
  (ಶೂನ್ಯ)bits;
  ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
}

ಗಣನಾ nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

ಸ್ಥಿರ bool match_host(ಧ್ರುವ ವರ್ಣ *token, size_t tokenlen,
                       ಧ್ರುವ ವರ್ಣ *name, size_t namelen)
{
  bool match = FALSE;

  /* ignore trailing dots in the token to check */
  ಯದಿ(token[tokenlen - 1] == '.')
    tokenlen--;

  ಯದಿ(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  ಯದಿ(tokenlen == namelen)
    /* case A, exact match */
    match = curl_strnequal(token, name, namelen);
  ಅನ್ಯಥಾ ಯದಿ(tokenlen < namelen) {
    /* case B, tailmatch domain */
    match = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  ಪ್ರತ್ಯಾಗಮ match;
}

ಸ್ಥಿರ bool match_ip(ಪೂರ್ಣಾಙ್ಕ type, ಧ್ರುವ ವರ್ಣ *token, size_t tokenlen,
                     ಧ್ರುವ ವರ್ಣ *name)
{
  ವರ್ಣ *slash;
  ಪೂರ್ಣಾಙ್ಕ bits = -1; /* negative means not provided */
  ವರ್ಣ checkip[128];
  ಯದಿ(tokenlen >= ಪರಿಮಾಣ(checkip))
    /* this cannot match */
    ಪ್ರತ್ಯಾಗಮ FALSE;
  /* copy the check name to a temp buffer */
  memcpy(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  ಯದಿ(slash) {
    curl_off_t ಮೌಲ್ಯ;
    ಧ್ರುವ ವರ್ಣ *p = &slash[1];
    ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&p, &ಮೌಲ್ಯ, 128) || *p)
      ಪ್ರತ್ಯಾಗಮ FALSE;
    /* a too large value is rejected in the cidr function below */
    bits = (ಪೂರ್ಣಾಙ್ಕ)ಮೌಲ್ಯ;
    *slash = 0; /* null-terminate there */
  }
  ಯದಿ(type == TYPE_IPV6)
    ಪ್ರತ್ಯಾಗಮ cidr6_match(name, checkip, bits);
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ cidr4_match(name, checkip, bits);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *no_proxy);
UNITTEST bool proxy_check_noproxy(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  ಯದಿ(!name || name[0] == '\0')
    ಪ್ರತ್ಯಾಗಮ FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  ಯದಿ(no_proxy && no_proxy[0]) {
    ಧ್ರುವ ವರ್ಣ *p = no_proxy;
    size_t namelen;
    ವರ್ಣ address[16];
    ಗಣನಾ nametype type = TYPE_HOST;
    ಯದಿ(!strcmp("*", no_proxy))
      ಪ್ರತ್ಯಾಗಮ TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = strlen(name);
    ಯದಿ(curlx_inet_pton(AF_INET, name, &address) == 1)
      type = TYPE_IPV4;
#ifdef USE_IPV6
    ಅನ್ಯಥಾ ಯದಿ(curlx_inet_pton(AF_INET6, name, &address) == 1)
      type = TYPE_IPV6;
#endif
    ಅನ್ಯಥಾ {
      /* ignore trailing dots in the hostname */
      ಯದಿ(name[namelen - 1] == '.')
        namelen--;
    }

    ಯಾವತ್(*p) {
      ಧ್ರುವ ವರ್ಣ *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      ಯಾವತ್(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      ಯದಿ(tokenlen) {
        bool match = FALSE;
        ಯದಿ(type == TYPE_HOST)
          match = match_host(token, tokenlen, name, namelen);
        ಅನ್ಯಥಾ
          match = match_ip(type, token, tokenlen, name);

        ಯದಿ(match)
          ಪ್ರತ್ಯಾಗಮ TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      ಯದಿ(*p != ',')
        ಭಙ್ಗ;
      /* pass any number of commas */
      ಯಾವತ್(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ifndef CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
ಸ್ಥಿರ ವರ್ಣ *proxy_detect_proxy(ಸಂರಚನಾ Curl_easy *data,
                                ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *scheme)
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
  ಧ್ರುವ ವರ್ಣ *env_name = ರಿಕ್ತ;
  ವರ್ಣ *proxy = ರಿಕ್ತ;
  ವರ್ಣ name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  ಯದಿ((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, ಪರಿಮಾಣ(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    ಯದಿ(!proxy) {
      Curl_strntoupper(name_buf, name_buf, ಪರಿಮಾಣ(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  ಯದಿ(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    ಯದಿ(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(!proxy &&
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

  ಯದಿ(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    ಯದಿ(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  ಯದಿ(proxy)
    infof(data, "Uses proxy env variable %s == '%s'", env_name, proxy);

  ಪ್ರತ್ಯಾಗಮ proxy;
}
#endif /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
ಸ್ಥಿರ CURLcode parse_proxy(ಸಂರಚನಾ Curl_easy *data,
                            ಧ್ರುವ ವರ್ಣ *proxy,
                            bool for_pre_proxy,
                            ಸಂರಚನಾ proxy_info *proxyinfo)
{
  ವರ್ಣ *proxyuser = ರಿಕ್ತ;
  ವರ್ಣ *proxypasswd = ರಿಕ್ತ;
  ವರ್ಣ *scheme = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : data->set.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  ಯದಿ(!uhp) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_set(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  ಯದಿ(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    ಯದಿ(!uc) {
      result = Curl_scheme_to_proxytype(data, scheme, &proxytype, proxy);
      ಯದಿ(result)
        ಗಚ್ಛ error;
    }
    ಅನ್ಯಥಾ ಯದಿ(uc != CURLUE_NO_SCHEME) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  ಅನ್ಯಥಾ {
    failf(data, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    result = CURLE_COULDNT_RESOLVE_PROXY;
    ಗಚ್ಛ error;
  }

  result = Curl_peer_from_proxy_url(uhp, data, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  ಯದಿ(result)
    ಗಚ್ಛ error;

  ವಿಕಲ್ಪ(proxytype) {
    ಪಕ್ಷ CURLPROXY_HTTP:
    ಪಕ್ಷ CURLPROXY_HTTP_1_0:
    ಪಕ್ಷ CURLPROXY_HTTPS:
    ಪಕ್ಷ CURLPROXY_HTTPS2:
    ಪಕ್ಷ CURLPROXY_HTTPS3:
      ಯದಿ(for_pre_proxy) {
        failf(data, "Unsupported pre-proxy type for \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        ಗಚ್ಛ error;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLPROXY_SOCKS4:
    ಪಕ್ಷ CURLPROXY_SOCKS4A:
    ಪಕ್ಷ CURLPROXY_SOCKS5:
    ಪಕ್ಷ CURLPROXY_SOCKS5_HOSTNAME:
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      ಗಚ್ಛ error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  ಯದಿ(uc && (uc != CURLUE_NO_USER)) {
    result = Curl_uc_to_curlcode(uc);
    ಗಚ್ಛ error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  ಯದಿ(uc && (uc != CURLUE_NO_PASSWORD)) {
    result = Curl_uc_to_curlcode(uc);
    ಗಚ್ಛ error;
  }

  ಯದಿ(proxyuser || proxypasswd) {
    result = Curl_creds_create(proxyuser, proxypasswd, ರಿಕ್ತ, ರಿಕ್ತ,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    ಯದಿ(result)
      ಗಚ್ಛ error;
  }
  ಅನ್ಯಥಾ ಯದಿ(!for_pre_proxy &&
          (CURL_EASY_STR(data, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(data, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    result = Curl_creds_create(CURL_EASY_STR(data, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(data, STRING_PROXYPASSWORD),
                               ರಿಕ್ತ, ರಿಕ್ತ,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  ಅನ್ಯಥಾ
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ifdef DEBUGBUILD
  ಯದಿ(!result) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#endif
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Is transfer's origin exempted from proxy use? */
ಸ್ಥಿರ bool proxy_do_not_proxy(ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ ವರ್ಣ *no_proxy;
  ವರ್ಣ *env_no_proxy = ರಿಕ್ತ;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  ಯದಿ(data->state.origin->scheme->flags & PROTOPT_NONETWORK)
    ಪ್ರತ್ಯಾಗಮ TRUE;

  no_proxy = CURL_EASY_STR(data, STRING_NOPROXY);
  ಯದಿ(!no_proxy) {
    ಧ್ರುವ ವರ್ಣ *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    ಯದಿ(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    ಯದಿ(env_no_proxy)
      infof(data, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(data->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  ಪ್ರತ್ಯಾಗಮ do_not_proxy;
}

CURLcode Curl_proxy_init_conn(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ connectdata *conn)
{
  ವರ್ಣ *proxy = ರಿಕ್ತ;
  ವರ್ಣ *pre_proxy = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *str = ರಿಕ್ತ;
  bool do_env_detect = TRUE;
  CURLcode result = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->bits.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  ಯದಿ(proxy_do_not_proxy(data))
    ಗಚ್ಛ out;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(data, STRING_PROXY);
  ಯದಿ(str) {
    ಯದಿ(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      ಯದಿ(!proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
    }
    ಅನ್ಯಥಾ
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(data, STRING_PRE_PROXY);
  ಯದಿ(str) {
    ಯದಿ(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      ಯದಿ(!pre_proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
    }
    ಅನ್ಯಥಾ
      do_env_detect = FALSE;
  }

#ifndef CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  ಯದಿ(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(data, conn->scheme);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)do_env_detect;
#endif /* CURL_DISABLE_HTTP */

  ಯದಿ(!proxy && !pre_proxy)
    ಗಚ್ಛ out;

  ಯದಿ(pre_proxy) {
    result = parse_proxy(data, pre_proxy, TRUE, &conn->socks_proxy);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ಯದಿ(proxy) {
    result = parse_proxy(data, proxy, FALSE, &conn->http_proxy);
    ಯದಿ(result)
      ಗಚ್ಛ out;

    ವಿಕಲ್ಪ(conn->http_proxy.proxytype) {
    ಪಕ್ಷ CURLPROXY_SOCKS4:
    ಪಕ್ಷ CURLPROXY_SOCKS4A:
    ಪಕ್ಷ CURLPROXY_SOCKS5:
    ಪಕ್ಷ CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      ಯದಿ(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(data, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        ಗಚ್ಛ out;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, ಪರಿಮಾಣ(conn->http_proxy));
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      /* all other types are HTTP */
      ಭಙ್ಗ;
    }
  }

  ಯದಿ(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ifdef CURL_DISABLE_HTTP
  ಯದಿ(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    result = CURLE_UNSUPPORTED_PROTOCOL;
    ಗಚ್ಛ out;
  }

#ಅನ್ಯಥಾ /* CURL_DISABLE_HTTP */
  ಯದಿ(conn->http_proxy.peer) {
    ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *scheme = data->state.origin->scheme;
    bool tunnel_proxy = (bool)data->set.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    ಯದಿ(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      ಯದಿ(conn->via_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      ಅನ್ಯಥಾ ಯದಿ(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      ಅನ್ಯಥಾ ಯದಿ(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      ಅನ್ಯಥಾ ಯದಿ(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    ಯದಿ(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->via_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->bits.origin_is_proxy = TRUE;
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    ಯದಿ(!Curl_safecmp(data->state.envproxy, proxy)) {
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
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* CURL_DISABLE_PROXY */
