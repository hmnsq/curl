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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "protocol.h"
#అన్తర్భావ "proxy.h"
#అన్తర్భావ "http_proxy.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "curlx/strparse.h"

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_సరిపోలిక(స్థిరరాశి అక్షరం *ipv4,    /* 1.2.3.4 address */
                          స్థిరరాశి అక్షరం *జాలం, /* 1.2.3.4 address */
                          పూర్ణాంకం బిట్లు);
UNITTEST bool cidr4_సరిపోలిక(స్థిరరాశి అక్షరం *ipv4,    /* 1.2.3.4 address */
                          స్థిరరాశి అక్షరం *జాలం, /* 1.2.3.4 address */
                          పూర్ణాంకం బిట్లు)
{
  గుర్తులేని పూర్ణాంకం చిరునామా = 0;
  గుర్తులేని పూర్ణాంకం check = 0;

  ఒకవేళ(బిట్లు > 32)
    /* strange input */
    తిరిగి FALSE;

  ఒకవేళ(curlx_inet_pton(AF_INET, ipv4, &చిరునామా) != 1)
    తిరిగి FALSE;
  ఒకవేళ(curlx_inet_pton(AF_INET, జాలం, &check) != 1)
    తిరిగి FALSE;

  ఒకవేళ((బిట్లు > 0) && (బిట్లు != 32)) {
    గుర్తులేని పూర్ణాంకం ముసుగు = 0xffffffff << (32 - బిట్లు);
    గుర్తులేని పూర్ణాంకం haddr = htonl(చిరునామా);
    గుర్తులేని పూర్ణాంకం hcheck = htonl(check);
#ఒకవేళ 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, జాలం, hcheck, బిట్లు, ముసుగు,
                  (haddr ^ hcheck) & ముసుగు);
#అయితే_ముగింపు
    ఒకవేళ((haddr ^ hcheck) & ముసుగు)
      తిరిగి FALSE;
    తిరిగి TRUE;
  }
  లేకపోతే ఒకవేళ(!బిట్లు && !check)
    /* 0.0.0.0/0 matches everything */
    తిరిగి TRUE;

  తిరిగి చిరునామా == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_సరిపోలిక(స్థిరరాశి అక్షరం *ipv6, స్థిరరాశి అక్షరం *జాలం,
                          పూర్ణాంకం బిట్లు);
UNITTEST bool cidr6_సరిపోలిక(స్థిరరాశి అక్షరం *ipv6, స్థిరరాశి అక్షరం *జాలం,
                          పూర్ణాంకం బిట్లు)
{
#నిర్వచితమైతే USE_IPV6
  స్థిర స్థిరరాశి అక్షరం null6[16]= { 0 };
  గుర్తులేని పూర్ణాంకం బైట్లు;
  గుర్తులేని పూర్ణాంకం rest;
  గుర్తులేని అక్షరం చిరునామా[16];
  గుర్తులేని అక్షరం check[16];

  ఒకవేళ(బిట్లు == -1)
    బిట్లు = 128;

  బైట్లు = బిట్లు / 8;
  rest = బిట్లు & 0x07;
  ఒకవేళ((బైట్లు > 16) || ((బైట్లు == 16) && rest))
    తిరిగి FALSE;
  ఒకవేళ(curlx_inet_pton(AF_INET6, ipv6, చిరునామా) != 1)
    తిరిగి FALSE;
  ఒకవేళ(curlx_inet_pton(AF_INET6, జాలం, check) != 1)
    తిరిగి FALSE;
  ఒకవేళ((బైట్లు == 16) && !బిట్లు && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    తిరిగి TRUE;
  ఒకవేళ(బైట్లు && memcmp(చిరునామా, check, బైట్లు))
    తిరిగి FALSE;
  ఒకవేళ(rest && ((చిరునామా[బైట్లు] ^ check[బైట్లు]) & (0xff << (8 - rest))))
    తిరిగి FALSE;

  తిరిగి TRUE;
#లేకపోతే
  (శూన్య)ipv6;
  (శూన్య)జాలం;
  (శూన్య)బిట్లు;
  తిరిగి FALSE;
#అయితే_ముగింపు
}

గణన nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

స్థిర bool సరిపోలిక_host(స్థిరరాశి అక్షరం *token, size_t tokenlen,
                       స్థిరరాశి అక్షరం *name, size_t namelen)
{
  bool సరిపోలిక = FALSE;

  /* ignore trailing dots in the token to check */
  ఒకవేళ(token[tokenlen - 1] == '.')
    tokenlen--;

  ఒకవేళ(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  ఒకవేళ(tokenlen == namelen)
    /* case A, exact match */
    సరిపోలిక = curl_strnequal(token, name, namelen);
  లేకపోతే ఒకవేళ(tokenlen < namelen) {
    /* case B, tailmatch domain */
    సరిపోలిక = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  తిరిగి సరిపోలిక;
}

స్థిర bool సరిపోలిక_ip(పూర్ణాంకం type, స్థిరరాశి అక్షరం *token, size_t tokenlen,
                     స్థిరరాశి అక్షరం *name)
{
  అక్షరం *slash;
  పూర్ణాంకం బిట్లు = -1; /* negative means not provided */
  అక్షరం checkip[128];
  ఒకవేళ(tokenlen >= పరిమాణం(checkip))
    /* this cannot match */
    తిరిగి FALSE;
  /* copy the check name to a temp buffer */
  జ్ఞాపకం_నకలు(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  ఒకవేళ(slash) {
    curl_off_t విలువ;
    స్థిరరాశి అక్షరం *p = &slash[1];
    ఒకవేళ(curlx_str_సంఖ్య(&p, &విలువ, 128) || *p)
      తిరిగి FALSE;
    /* a too large value is rejected in the cidr function below */
    బిట్లు = (పూర్ణాంకం)విలువ;
    *slash = 0; /* null-terminate there */
  }
  ఒకవేళ(type == TYPE_IPV6)
    తిరిగి cidr6_సరిపోలిక(name, checkip, బిట్లు);
  లేకపోతే
    తిరిగి cidr4_సరిపోలిక(name, checkip, బిట్లు);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *no_proxy);
UNITTEST bool proxy_check_noproxy(స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  ఒకవేళ(!name || name[0] == '\0')
    తిరిగి FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  ఒకవేళ(no_proxy && no_proxy[0]) {
    స్థిరరాశి అక్షరం *p = no_proxy;
    size_t namelen;
    అక్షరం చిరునామా[16];
    గణన nametype type = TYPE_HOST;
    ఒకవేళ(!strcmp("*", no_proxy))
      తిరిగి TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = అక్షరశ్రేణి_పొడవు(name);
    ఒకవేళ(curlx_inet_pton(AF_INET, name, &చిరునామా) == 1)
      type = TYPE_IPV4;
#నిర్వచితమైతే USE_IPV6
    లేకపోతే ఒకవేళ(curlx_inet_pton(AF_INET6, name, &చిరునామా) == 1)
      type = TYPE_IPV6;
#అయితే_ముగింపు
    లేకపోతే {
      /* ignore trailing dots in the hostname */
      ఒకవేళ(name[namelen - 1] == '.')
        namelen--;
    }

    అయితే(*p) {
      స్థిరరాశి అక్షరం *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      అయితే(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      ఒకవేళ(tokenlen) {
        bool సరిపోలిక = FALSE;
        ఒకవేళ(type == TYPE_HOST)
          సరిపోలిక = సరిపోలిక_host(token, tokenlen, name, namelen);
        లేకపోతే
          సరిపోలిక = సరిపోలిక_ip(type, token, tokenlen, name);

        ఒకవేళ(సరిపోలిక)
          తిరిగి TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      ఒకవేళ(*p != ',')
        ఆపు;
      /* pass any number of commas */
      అయితే(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  తిరిగి FALSE;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
స్థిర అక్షరం *proxy_detect_proxy(నిర్మాణం Curl_easy *దత్తాంశం,
                                స్థిరరాశి నిర్మాణం Curl_scheme *scheme)
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
  స్థిరరాశి అక్షరం *env_name = ఖాళీ;
  అక్షరం *proxy = ఖాళీ;
  అక్షరం name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  ఒకవేళ((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, పరిమాణం(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    ఒకవేళ(!proxy) {
      Curl_strntoupper(name_buf, name_buf, పరిమాణం(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  ఒకవేళ(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    ఒకవేళ(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  లేకపోతే ఒకవేళ(!proxy &&
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

  ఒకవేళ(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    ఒకవేళ(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  ఒకవేళ(proxy)
    infof(దత్తాంశం, "Uses proxy env variable %s == '%s'", env_name, proxy);

  తిరిగి proxy;
}
#అయితే_ముగింపు /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
స్థిర CURLcode parse_proxy(నిర్మాణం Curl_easy *దత్తాంశం,
                            స్థిరరాశి అక్షరం *proxy,
                            bool for_pre_proxy,
                            నిర్మాణం proxy_info *proxyinfo)
{
  అక్షరం *proxyuser = ఖాళీ;
  అక్షరం *proxypasswd = ఖాళీ;
  అక్షరం *scheme = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : దత్తాంశం->సమితి.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  ఒకవేళ(!uhp) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_సమితి(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  ఒకవేళ(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    ఒకవేళ(!uc) {
      ఫలితం = Curl_scheme_గమ్యం_proxytype(దత్తాంశం, scheme, &proxytype, proxy);
      ఒకవేళ(ఫలితం)
        వెళ్ళు error;
    }
    లేకపోతే ఒకవేళ(uc != CURLUE_NO_SCHEME) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  లేకపోతే {
    failf(దత్తాంశం, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    ఫలితం = CURLE_COULDNT_RESOLVE_PROXY;
    వెళ్ళు error;
  }

  ఫలితం = Curl_peer_from_proxy_url(uhp, దత్తాంశం, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  ఒకవేళ(ఫలితం)
    వెళ్ళు error;

  ఎంపిక(proxytype) {
    స్థితి CURLPROXY_HTTP:
    స్థితి CURLPROXY_HTTP_1_0:
    స్థితి CURLPROXY_HTTPS:
    స్థితి CURLPROXY_HTTPS2:
    స్థితి CURLPROXY_HTTPS3:
      ఒకవేళ(for_pre_proxy) {
        failf(దత్తాంశం, "Unsupported pre-proxy type for \'%s\'", proxy);
        ఫలితం = CURLE_COULDNT_RESOLVE_PROXY;
        వెళ్ళు error;
      }
      ఆపు;
    స్థితి CURLPROXY_SOCKS4:
    స్థితి CURLPROXY_SOCKS4A:
    స్థితి CURLPROXY_SOCKS5:
    స్థితి CURLPROXY_SOCKS5_HOSTNAME:
      ఆపు;
    అప్రమేయ:
      failf(దత్తాంశం, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      ఫలితం = CURLE_COULDNT_RESOLVE_PROXY;
      వెళ్ళు error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  ఒకవేళ(uc && (uc != CURLUE_NO_USER)) {
    ఫలితం = Curl_uc_గమ్యం_curlcode(uc);
    వెళ్ళు error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  ఒకవేళ(uc && (uc != CURLUE_NO_PASSWORD)) {
    ఫలితం = Curl_uc_గమ్యం_curlcode(uc);
    వెళ్ళు error;
  }

  ఒకవేళ(proxyuser || proxypasswd) {
    ఫలితం = Curl_creds_create(proxyuser, proxypasswd, ఖాళీ, ఖాళీ,
                               CURL_EASY_STR(దత్తాంశం, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    ఒకవేళ(ఫలితం)
      వెళ్ళు error;
  }
  లేకపోతే ఒకవేళ(!for_pre_proxy &&
          (CURL_EASY_STR(దత్తాంశం, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(దత్తాంశం, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(దత్తాంశం, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    ఫలితం = Curl_creds_create(CURL_EASY_STR(దత్తాంశం, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(దత్తాంశం, STRING_PROXYPASSWORD),
                               ఖాళీ, ఖాళీ,
                               CURL_EASY_STR(దత్తాంశం, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  లేకపోతే
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(!ఫలితం) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#అయితే_ముగింపు
  తిరిగి ఫలితం;
}

/* Is transfer's origin exempted from proxy use? */
స్థిర bool proxy_do_not_proxy(నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి అక్షరం *no_proxy;
  అక్షరం *env_no_proxy = ఖాళీ;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  ఒకవేళ(దత్తాంశం->state.origin->scheme->flags & PROTOPT_NONETWORK)
    తిరిగి TRUE;

  no_proxy = CURL_EASY_STR(దత్తాంశం, STRING_NOPROXY);
  ఒకవేళ(!no_proxy) {
    స్థిరరాశి అక్షరం *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    ఒకవేళ(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    ఒకవేళ(env_no_proxy)
      infof(దత్తాంశం, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(దత్తాంశం->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  తిరిగి do_not_proxy;
}

CURLcode Curl_proxy_init_conn(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn)
{
  అక్షరం *proxy = ఖాళీ;
  అక్షరం *pre_proxy = ఖాళీ;
  స్థిరరాశి అక్షరం *str = ఖాళీ;
  bool do_env_detect = TRUE;
  CURLcode ఫలితం = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->బిట్లు.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  ఒకవేళ(proxy_do_not_proxy(దత్తాంశం))
    వెళ్ళు బహిర్గతం;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(దత్తాంశం, STRING_PROXY);
  ఒకవేళ(str) {
    ఒకవేళ(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      ఒకవేళ(!proxy) {
        failf(దత్తాంశం, "memory shortage");
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
    }
    లేకపోతే
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(దత్తాంశం, STRING_PRE_PROXY);
  ఒకవేళ(str) {
    ఒకవేళ(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      ఒకవేళ(!pre_proxy) {
        failf(దత్తాంశం, "memory shortage");
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
    }
    లేకపోతే
      do_env_detect = FALSE;
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  ఒకవేళ(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(దత్తాంశం, conn->scheme);
#లేకపోతే
  (శూన్య)do_env_detect;
#అయితే_ముగింపు /* CURL_DISABLE_HTTP */

  ఒకవేళ(!proxy && !pre_proxy)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(pre_proxy) {
    ఫలితం = parse_proxy(దత్తాంశం, pre_proxy, TRUE, &conn->socks_proxy);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(proxy) {
    ఫలితం = parse_proxy(దత్తాంశం, proxy, FALSE, &conn->http_proxy);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    ఎంపిక(conn->http_proxy.proxytype) {
    స్థితి CURLPROXY_SOCKS4:
    స్థితి CURLPROXY_SOCKS4A:
    స్థితి CURLPROXY_SOCKS5:
    స్థితి CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      ఒకవేళ(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(దత్తాంశం, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        ఫలితం = CURLE_COULDNT_RESOLVE_PROXY;
        వెళ్ళు బహిర్గతం;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, పరిమాణం(conn->http_proxy));
      ఆపు;
    అప్రమేయ:
      /* all other types are HTTP */
      ఆపు;
    }
  }

  ఒకవేళ(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#నిర్వచితమైతే CURL_DISABLE_HTTP
  ఒకవేళ(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    ఫలితం = CURLE_UNSUPPORTED_PROTOCOL;
    వెళ్ళు బహిర్గతం;
  }

#లేకపోతే /* CURL_DISABLE_HTTP */
  ఒకవేళ(conn->http_proxy.peer) {
    స్థిరరాశి నిర్మాణం Curl_scheme *scheme = దత్తాంశం->state.origin->scheme;
    bool tunnel_proxy = (bool)దత్తాంశం->సమితి.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    ఒకవేళ(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      ఒకవేళ(conn->ద్వారా_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      లేకపోతే ఒకవేళ(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      లేకపోతే ఒకవేళ(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      లేకపోతే ఒకవేళ(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    ఒకవేళ(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->ద్వారా_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->బిట్లు.origin_is_proxy = TRUE;
    }

#నిర్వచితం_కాకపోతే CURL_DISABLE_DIGEST_AUTH
    ఒకవేళ(!Curl_safecmp(దత్తాంశం->state.envproxy, proxy)) {
      /* proxy changed */
      Curl_auth_digest_cleanup(&దత్తాంశం->state.proxydigest);
      curlx_free(దత్తాంశం->state.envproxy);
      దత్తాంశం->state.envproxy = curlx_strdup(proxy);
    }
#అయితే_ముగింపు
  }
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP */

బహిర్గతం:
  curlx_free(pre_proxy);
  curlx_free(proxy);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* CURL_DISABLE_PROXY */
