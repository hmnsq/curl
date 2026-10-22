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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "protocol.h"
#অন্তর্ভাব "proxy.h"
#অন্তর্ভাব "http_proxy.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "curlx/strparse.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_match(ধ্রুবক অক্ষর *ipv4,    /* 1.2.3.4 address */
                          ধ্রুবক অক্ষর *network, /* 1.2.3.4 address */
                          পূর্ণসংখ্যা bits);
UNITTEST bool cidr4_match(ধ্রুবক অক্ষর *ipv4,    /* 1.2.3.4 address */
                          ধ্রুবক অক্ষর *network, /* 1.2.3.4 address */
                          পূর্ণসংখ্যা bits)
{
  unsigned পূর্ণসংখ্যা address = 0;
  unsigned পূর্ণসংখ্যা check = 0;

  যদি(bits > 32)
    /* strange input */
    ফেরত FALSE;

  যদি(curlx_inet_pton(AF_INET, ipv4, &address) != 1)
    ফেরত FALSE;
  যদি(curlx_inet_pton(AF_INET, network, &check) != 1)
    ফেরত FALSE;

  যদি((bits > 0) && (bits != 32)) {
    unsigned পূর্ণসংখ্যা mask = 0xffffffff << (32 - bits);
    unsigned পূর্ণসংখ্যা haddr = htonl(address);
    unsigned পূর্ণসংখ্যা hcheck = htonl(check);
#যদি 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, network, hcheck, bits, mask,
                  (haddr ^ hcheck) & mask);
#endif
    যদি((haddr ^ hcheck) & mask)
      ফেরত FALSE;
    ফেরত TRUE;
  }
  অন্যথা যদি(!bits && !check)
    /* 0.0.0.0/0 matches everything */
    ফেরত TRUE;

  ফেরত address == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_match(ধ্রুবক অক্ষর *ipv6, ধ্রুবক অক্ষর *network,
                          পূর্ণসংখ্যা bits);
UNITTEST bool cidr6_match(ধ্রুবক অক্ষর *ipv6, ধ্রুবক অক্ষর *network,
                          পূর্ণসংখ্যা bits)
{
#ifdef USE_IPV6
  স্থির ধ্রুবক অক্ষর null6[16]= { 0 };
  unsigned পূর্ণসংখ্যা bytes;
  unsigned পূর্ণসংখ্যা rest;
  unsigned অক্ষর address[16];
  unsigned অক্ষর check[16];

  যদি(bits == -1)
    bits = 128;

  bytes = bits / 8;
  rest = bits & 0x07;
  যদি((bytes > 16) || ((bytes == 16) && rest))
    ফেরত FALSE;
  যদি(curlx_inet_pton(AF_INET6, ipv6, address) != 1)
    ফেরত FALSE;
  যদি(curlx_inet_pton(AF_INET6, network, check) != 1)
    ফেরত FALSE;
  যদি((bytes == 16) && !bits && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    ফেরত TRUE;
  যদি(bytes && memcmp(address, check, bytes))
    ফেরত FALSE;
  যদি(rest && ((address[bytes] ^ check[bytes]) & (0xff << (8 - rest))))
    ফেরত FALSE;

  ফেরত TRUE;
#অন্যথা
  (শূন্য)ipv6;
  (শূন্য)network;
  (শূন্য)bits;
  ফেরত FALSE;
#endif
}

গণনা nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

স্থির bool match_host(ধ্রুবক অক্ষর *token, size_t tokenlen,
                       ধ্রুবক অক্ষর *name, size_t namelen)
{
  bool match = FALSE;

  /* ignore trailing dots in the token to check */
  যদি(token[tokenlen - 1] == '.')
    tokenlen--;

  যদি(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  যদি(tokenlen == namelen)
    /* case A, exact match */
    match = curl_strnequal(token, name, namelen);
  অন্যথা যদি(tokenlen < namelen) {
    /* case B, tailmatch domain */
    match = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  ফেরত match;
}

স্থির bool match_ip(পূর্ণসংখ্যা type, ধ্রুবক অক্ষর *token, size_t tokenlen,
                     ধ্রুবক অক্ষর *name)
{
  অক্ষর *slash;
  পূর্ণসংখ্যা bits = -1; /* negative means not provided */
  অক্ষর checkip[128];
  যদি(tokenlen >= আকার(checkip))
    /* this cannot match */
    ফেরত FALSE;
  /* copy the check name to a temp buffer */
  memcpy(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  যদি(slash) {
    curl_off_t মান;
    ধ্রুবক অক্ষর *p = &slash[1];
    যদি(curlx_str_সংখ্যা(&p, &মান, 128) || *p)
      ফেরত FALSE;
    /* a too large value is rejected in the cidr function below */
    bits = (পূর্ণসংখ্যা)মান;
    *slash = 0; /* null-terminate there */
  }
  যদি(type == TYPE_IPV6)
    ফেরত cidr6_match(name, checkip, bits);
  অন্যথা
    ফেরত cidr4_match(name, checkip, bits);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *no_proxy);
UNITTEST bool proxy_check_noproxy(ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  যদি(!name || name[0] == '\0')
    ফেরত FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  যদি(no_proxy && no_proxy[0]) {
    ধ্রুবক অক্ষর *p = no_proxy;
    size_t namelen;
    অক্ষর address[16];
    গণনা nametype type = TYPE_HOST;
    যদি(!strcmp("*", no_proxy))
      ফেরত TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = strlen(name);
    যদি(curlx_inet_pton(AF_INET, name, &address) == 1)
      type = TYPE_IPV4;
#ifdef USE_IPV6
    অন্যথা যদি(curlx_inet_pton(AF_INET6, name, &address) == 1)
      type = TYPE_IPV6;
#endif
    অন্যথা {
      /* ignore trailing dots in the hostname */
      যদি(name[namelen - 1] == '.')
        namelen--;
    }

    যখন(*p) {
      ধ্রুবক অক্ষর *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      যখন(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      যদি(tokenlen) {
        bool match = FALSE;
        যদি(type == TYPE_HOST)
          match = match_host(token, tokenlen, name, namelen);
        অন্যথা
          match = match_ip(type, token, tokenlen, name);

        যদি(match)
          ফেরত TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      যদি(*p != ',')
        বিরতি;
      /* pass any number of commas */
      যখন(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  ফেরত FALSE;
}

#ifndef CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
স্থির অক্ষর *proxy_detect_proxy(কাঠামো Curl_easy *তথ্য,
                                ধ্রুবক কাঠামো Curl_scheme *scheme)
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
  ধ্রুবক অক্ষর *env_name = ফাঁকা;
  অক্ষর *proxy = ফাঁকা;
  অক্ষর name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  যদি((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, আকার(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    যদি(!proxy) {
      Curl_strntoupper(name_buf, name_buf, আকার(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  যদি(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    যদি(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  অন্যথা যদি(!proxy &&
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

  যদি(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    যদি(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  যদি(proxy)
    infof(তথ্য, "Uses proxy env variable %s == '%s'", env_name, proxy);

  ফেরত proxy;
}
#endif /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
স্থির CURLcode parse_proxy(কাঠামো Curl_easy *তথ্য,
                            ধ্রুবক অক্ষর *proxy,
                            bool for_pre_proxy,
                            কাঠামো proxy_info *proxyinfo)
{
  অক্ষর *proxyuser = ফাঁকা;
  অক্ষর *proxypasswd = ফাঁকা;
  অক্ষর *scheme = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : তথ্য->set.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  যদি(!uhp) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_set(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  যদি(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    যদি(!uc) {
      ফলাফল = Curl_scheme_to_proxytype(তথ্য, scheme, &proxytype, proxy);
      যদি(ফলাফল)
        যাও error;
    }
    অন্যথা যদি(uc != CURLUE_NO_SCHEME) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  অন্যথা {
    failf(তথ্য, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    ফলাফল = CURLE_COULDNT_RESOLVE_PROXY;
    যাও error;
  }

  ফলাফল = Curl_peer_from_proxy_url(uhp, তথ্য, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  যদি(ফলাফল)
    যাও error;

  নির্বাচন(proxytype) {
    ক্ষেত্র CURLPROXY_HTTP:
    ক্ষেত্র CURLPROXY_HTTP_1_0:
    ক্ষেত্র CURLPROXY_HTTPS:
    ক্ষেত্র CURLPROXY_HTTPS2:
    ক্ষেত্র CURLPROXY_HTTPS3:
      যদি(for_pre_proxy) {
        failf(তথ্য, "Unsupported pre-proxy type for \'%s\'", proxy);
        ফলাফল = CURLE_COULDNT_RESOLVE_PROXY;
        যাও error;
      }
      বিরতি;
    ক্ষেত্র CURLPROXY_SOCKS4:
    ক্ষেত্র CURLPROXY_SOCKS4A:
    ক্ষেত্র CURLPROXY_SOCKS5:
    ক্ষেত্র CURLPROXY_SOCKS5_HOSTNAME:
      বিরতি;
    পূর্বনির্ধারিত:
      failf(তথ্য, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      ফলাফল = CURLE_COULDNT_RESOLVE_PROXY;
      যাও error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  যদি(uc && (uc != CURLUE_NO_USER)) {
    ফলাফল = Curl_uc_to_curlcode(uc);
    যাও error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  যদি(uc && (uc != CURLUE_NO_PASSWORD)) {
    ফলাফল = Curl_uc_to_curlcode(uc);
    যাও error;
  }

  যদি(proxyuser || proxypasswd) {
    ফলাফল = Curl_creds_create(proxyuser, proxypasswd, ফাঁকা, ফাঁকা,
                               CURL_EASY_STR(তথ্য, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    যদি(ফলাফল)
      যাও error;
  }
  অন্যথা যদি(!for_pre_proxy &&
          (CURL_EASY_STR(তথ্য, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(তথ্য, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(তথ্য, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    ফলাফল = Curl_creds_create(CURL_EASY_STR(তথ্য, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(তথ্য, STRING_PROXYPASSWORD),
                               ফাঁকা, ফাঁকা,
                               CURL_EASY_STR(তথ্য, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  অন্যথা
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ifdef DEBUGBUILD
  যদি(!ফলাফল) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#endif
  ফেরত ফলাফল;
}

/* Is transfer's origin exempted from proxy use? */
স্থির bool proxy_do_not_proxy(কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক অক্ষর *no_proxy;
  অক্ষর *env_no_proxy = ফাঁকা;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  যদি(তথ্য->state.origin->scheme->flags & PROTOPT_NONETWORK)
    ফেরত TRUE;

  no_proxy = CURL_EASY_STR(তথ্য, STRING_NOPROXY);
  যদি(!no_proxy) {
    ধ্রুবক অক্ষর *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    যদি(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    যদি(env_no_proxy)
      infof(তথ্য, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(তথ্য->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  ফেরত do_not_proxy;
}

CURLcode Curl_proxy_init_conn(কাঠামো Curl_easy *তথ্য,
                              কাঠামো connectdata *conn)
{
  অক্ষর *proxy = ফাঁকা;
  অক্ষর *pre_proxy = ফাঁকা;
  ধ্রুবক অক্ষর *str = ফাঁকা;
  bool do_env_detect = TRUE;
  CURLcode ফলাফল = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->bits.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  যদি(proxy_do_not_proxy(তথ্য))
    যাও out;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(তথ্য, STRING_PROXY);
  যদি(str) {
    যদি(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      যদি(!proxy) {
        failf(তথ্য, "memory shortage");
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
    }
    অন্যথা
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(তথ্য, STRING_PRE_PROXY);
  যদি(str) {
    যদি(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      যদি(!pre_proxy) {
        failf(তথ্য, "memory shortage");
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
    }
    অন্যথা
      do_env_detect = FALSE;
  }

#ifndef CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  যদি(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(তথ্য, conn->scheme);
#অন্যথা
  (শূন্য)do_env_detect;
#endif /* CURL_DISABLE_HTTP */

  যদি(!proxy && !pre_proxy)
    যাও out;

  যদি(pre_proxy) {
    ফলাফল = parse_proxy(তথ্য, pre_proxy, TRUE, &conn->socks_proxy);
    যদি(ফলাফল)
      যাও out;
  }

  যদি(proxy) {
    ফলাফল = parse_proxy(তথ্য, proxy, FALSE, &conn->http_proxy);
    যদি(ফলাফল)
      যাও out;

    নির্বাচন(conn->http_proxy.proxytype) {
    ক্ষেত্র CURLPROXY_SOCKS4:
    ক্ষেত্র CURLPROXY_SOCKS4A:
    ক্ষেত্র CURLPROXY_SOCKS5:
    ক্ষেত্র CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      যদি(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(তথ্য, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        ফলাফল = CURLE_COULDNT_RESOLVE_PROXY;
        যাও out;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, আকার(conn->http_proxy));
      বিরতি;
    পূর্বনির্ধারিত:
      /* all other types are HTTP */
      বিরতি;
    }
  }

  যদি(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ifdef CURL_DISABLE_HTTP
  যদি(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    ফলাফল = CURLE_UNSUPPORTED_PROTOCOL;
    যাও out;
  }

#অন্যথা /* CURL_DISABLE_HTTP */
  যদি(conn->http_proxy.peer) {
    ধ্রুবক কাঠামো Curl_scheme *scheme = তথ্য->state.origin->scheme;
    bool tunnel_proxy = (bool)তথ্য->set.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    যদি(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      যদি(conn->via_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      অন্যথা যদি(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      অন্যথা যদি(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      অন্যথা যদি(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    যদি(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->via_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->bits.origin_is_proxy = TRUE;
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    যদি(!Curl_safecmp(তথ্য->state.envproxy, proxy)) {
      /* proxy changed */
      Curl_auth_digest_cleanup(&তথ্য->state.proxydigest);
      curlx_free(তথ্য->state.envproxy);
      তথ্য->state.envproxy = curlx_strdup(proxy);
    }
#endif
  }
#endif /* !CURL_DISABLE_HTTP */

out:
  curlx_free(pre_proxy);
  curlx_free(proxy);
  ফেরত ফলাফল;
}

#endif /* CURL_DISABLE_PROXY */
