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

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY

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

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_मेलनम्(ध्रुव वर्ण *ipv4,    /* 1.2.3.4 address */
                          ध्रुव वर्ण *जालम्, /* 1.2.3.4 address */
                          पूर्णाङ्क द्वयङ्काः);
UNITTEST bool cidr4_मेलनम्(ध्रुव वर्ण *ipv4,    /* 1.2.3.4 address */
                          ध्रुव वर्ण *जालम्, /* 1.2.3.4 address */
                          पूर्णाङ्क द्वयङ्काः)
{
  अचिह्नित पूर्णाङ्क स्थानाङ्कः = 0;
  अचिह्नित पूर्णाङ्क check = 0;

  यदि(द्वयङ्काः > 32)
    /* strange input */
    प्रत्यागम FALSE;

  यदि(curlx_inet_pton(AF_INET, ipv4, &स्थानाङ्कः) != 1)
    प्रत्यागम FALSE;
  यदि(curlx_inet_pton(AF_INET, जालम्, &check) != 1)
    प्रत्यागम FALSE;

  यदि((द्वयङ्काः > 0) && (द्वयङ्काः != 32)) {
    अचिह्नित पूर्णाङ्क आवरणम् = 0xffffffff << (32 - द्वयङ्काः);
    अचिह्नित पूर्णाङ्क haddr = htonl(स्थानाङ्कः);
    अचिह्नित पूर्णाङ्क hcheck = htonl(check);
#यदि 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, जालम्, hcheck, द्वयङ्काः, आवरणम्,
                  (haddr ^ hcheck) & आवरणम्);
#यदि_अन्तः
    यदि((haddr ^ hcheck) & आवरणम्)
      प्रत्यागम FALSE;
    प्रत्यागम TRUE;
  }
  अन्यथा यदि(!द्वयङ्काः && !check)
    /* 0.0.0.0/0 matches everything */
    प्रत्यागम TRUE;

  प्रत्यागम स्थानाङ्कः == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_मेलनम्(ध्रुव वर्ण *ipv6, ध्रुव वर्ण *जालम्,
                          पूर्णाङ्क द्वयङ्काः);
UNITTEST bool cidr6_मेलनम्(ध्रुव वर्ण *ipv6, ध्रुव वर्ण *जालम्,
                          पूर्णाङ्क द्वयङ्काः)
{
#यदि_परिभाषितम् USE_IPV6
  स्थिर ध्रुव वर्ण null6[16]= { 0 };
  अचिह्नित पूर्णाङ्क अष्टकानि;
  अचिह्नित पूर्णाङ्क rest;
  अचिह्नित वर्ण स्थानाङ्कः[16];
  अचिह्नित वर्ण check[16];

  यदि(द्वयङ्काः == -1)
    द्वयङ्काः = 128;

  अष्टकानि = द्वयङ्काः / 8;
  rest = द्वयङ्काः & 0x07;
  यदि((अष्टकानि > 16) || ((अष्टकानि == 16) && rest))
    प्रत्यागम FALSE;
  यदि(curlx_inet_pton(AF_INET6, ipv6, स्थानाङ्कः) != 1)
    प्रत्यागम FALSE;
  यदि(curlx_inet_pton(AF_INET6, जालम्, check) != 1)
    प्रत्यागम FALSE;
  यदि((अष्टकानि == 16) && !द्वयङ्काः && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    प्रत्यागम TRUE;
  यदि(अष्टकानि && memcmp(स्थानाङ्कः, check, अष्टकानि))
    प्रत्यागम FALSE;
  यदि(rest && ((स्थानाङ्कः[अष्टकानि] ^ check[अष्टकानि]) & (0xff << (8 - rest))))
    प्रत्यागम FALSE;

  प्रत्यागम TRUE;
#अन्यथा
  (शून्य)ipv6;
  (शून्य)जालम्;
  (शून्य)द्वयङ्काः;
  प्रत्यागम FALSE;
#यदि_अन्तः
}

गणना nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

स्थिर bool मेलनम्_host(ध्रुव वर्ण *token, size_t tokenlen,
                       ध्रुव वर्ण *name, size_t namelen)
{
  bool मेलनम् = FALSE;

  /* ignore trailing dots in the token to check */
  यदि(token[tokenlen - 1] == '.')
    tokenlen--;

  यदि(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  यदि(tokenlen == namelen)
    /* case A, exact match */
    मेलनम् = curl_strnequal(token, name, namelen);
  अन्यथा यदि(tokenlen < namelen) {
    /* case B, tailmatch domain */
    मेलनम् = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  प्रत्यागम मेलनम्;
}

स्थिर bool मेलनम्_ip(पूर्णाङ्क type, ध्रुव वर्ण *token, size_t tokenlen,
                     ध्रुव वर्ण *name)
{
  वर्ण *slash;
  पूर्णाङ्क द्वयङ्काः = -1; /* negative means not provided */
  वर्ण checkip[128];
  यदि(tokenlen >= परिमाण(checkip))
    /* this cannot match */
    प्रत्यागम FALSE;
  /* copy the check name to a temp buffer */
  स्मृतिं_प्रतिलिख(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  यदि(slash) {
    curl_off_t मानम्;
    ध्रुव वर्ण *p = &slash[1];
    यदि(curlx_str_सङ्ख्या(&p, &मानम्, 128) || *p)
      प्रत्यागम FALSE;
    /* a too large value is rejected in the cidr function below */
    द्वयङ्काः = (पूर्णाङ्क)मानम्;
    *slash = 0; /* null-terminate there */
  }
  यदि(type == TYPE_IPV6)
    प्रत्यागम cidr6_मेलनम्(name, checkip, द्वयङ्काः);
  अन्यथा
    प्रत्यागम cidr4_मेलनम्(name, checkip, द्वयङ्काः);
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
  यदि(!name || name[0] == '\0')
    प्रत्यागम FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  यदि(no_proxy && no_proxy[0]) {
    ध्रुव वर्ण *p = no_proxy;
    size_t namelen;
    वर्ण स्थानाङ्कः[16];
    गणना nametype type = TYPE_HOST;
    यदि(!वाक्यं_तुलय("*", no_proxy))
      प्रत्यागम TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = वाक्यदैर्घ्यम्(name);
    यदि(curlx_inet_pton(AF_INET, name, &स्थानाङ्कः) == 1)
      type = TYPE_IPV4;
#यदि_परिभाषितम् USE_IPV6
    अन्यथा यदि(curlx_inet_pton(AF_INET6, name, &स्थानाङ्कः) == 1)
      type = TYPE_IPV6;
#यदि_अन्तः
    अन्यथा {
      /* ignore trailing dots in the hostname */
      यदि(name[namelen - 1] == '.')
        namelen--;
    }

    यावत्(*p) {
      ध्रुव वर्ण *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      यावत्(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      यदि(tokenlen) {
        bool मेलनम् = FALSE;
        यदि(type == TYPE_HOST)
          मेलनम् = मेलनम्_host(token, tokenlen, name, namelen);
        अन्यथा
          मेलनम् = मेलनम्_ip(type, token, tokenlen, name);

        यदि(मेलनम्)
          प्रत्यागम TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      यदि(*p != ',')
        भङ्ग;
      /* pass any number of commas */
      यावत्(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  प्रत्यागम FALSE;
}

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
स्थिर वर्ण *proxy_detect_proxy(संरचना Curl_easy *दत्त,
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
  यदि((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, परिमाण(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    यदि(!proxy) {
      Curl_strntoupper(name_buf, name_buf, परिमाण(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  यदि(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    यदि(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  अन्यथा यदि(!proxy &&
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

  यदि(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    यदि(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  यदि(proxy)
    infof(दत्त, "Uses proxy env variable %s == '%s'", env_name, proxy);

  प्रत्यागम proxy;
}
#यदि_अन्तः /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
स्थिर CURLcode parse_proxy(संरचना Curl_easy *दत्त,
                            ध्रुव वर्ण *proxy,
                            bool for_pre_proxy,
                            संरचना proxy_info *proxyinfo)
{
  वर्ण *proxyuser = रिक्त;
  वर्ण *proxypasswd = रिक्त;
  वर्ण *scheme = रिक्त;
  CURLcode परिणाम = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : दत्त->समुच्चयः.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  यदि(!uhp) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_समुच्चयः(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  यदि(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    यदि(!uc) {
      परिणाम = Curl_scheme_गन्तव्यम्_proxytype(दत्त, scheme, &proxytype, proxy);
      यदि(परिणाम)
        गच्छ error;
    }
    अन्यथा यदि(uc != CURLUE_NO_SCHEME) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  अन्यथा {
    failf(दत्त, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    परिणाम = CURLE_COULDNT_RESOLVE_PROXY;
    गच्छ error;
  }

  परिणाम = Curl_peer_from_proxy_url(uhp, दत्त, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  यदि(परिणाम)
    गच्छ error;

  विकल्प(proxytype) {
    पक्ष CURLPROXY_HTTP:
    पक्ष CURLPROXY_HTTP_1_0:
    पक्ष CURLPROXY_HTTPS:
    पक्ष CURLPROXY_HTTPS2:
    पक्ष CURLPROXY_HTTPS3:
      यदि(for_pre_proxy) {
        failf(दत्त, "Unsupported pre-proxy type for \'%s\'", proxy);
        परिणाम = CURLE_COULDNT_RESOLVE_PROXY;
        गच्छ error;
      }
      भङ्ग;
    पक्ष CURLPROXY_SOCKS4:
    पक्ष CURLPROXY_SOCKS4A:
    पक्ष CURLPROXY_SOCKS5:
    पक्ष CURLPROXY_SOCKS5_HOSTNAME:
      भङ्ग;
    स्वभाव:
      failf(दत्त, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      परिणाम = CURLE_COULDNT_RESOLVE_PROXY;
      गच्छ error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  यदि(uc && (uc != CURLUE_NO_USER)) {
    परिणाम = Curl_uc_गन्तव्यम्_curlcode(uc);
    गच्छ error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  यदि(uc && (uc != CURLUE_NO_PASSWORD)) {
    परिणाम = Curl_uc_गन्तव्यम्_curlcode(uc);
    गच्छ error;
  }

  यदि(proxyuser || proxypasswd) {
    परिणाम = Curl_creds_create(proxyuser, proxypasswd, रिक्त, रिक्त,
                               CURL_EASY_STR(दत्त, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    यदि(परिणाम)
      गच्छ error;
  }
  अन्यथा यदि(!for_pre_proxy &&
          (CURL_EASY_STR(दत्त, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(दत्त, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(दत्त, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    परिणाम = Curl_creds_create(CURL_EASY_STR(दत्त, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(दत्त, STRING_PROXYPASSWORD),
                               रिक्त, रिक्त,
                               CURL_EASY_STR(दत्त, STRING_PROXY_SERVICE_NAME),
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
#यदि_परिभाषितम् DEBUGBUILD
  यदि(!परिणाम) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#यदि_अन्तः
  प्रत्यागम परिणाम;
}

/* Is transfer's origin exempted from proxy use? */
स्थिर bool proxy_do_not_proxy(संरचना Curl_easy *दत्त)
{
  ध्रुव वर्ण *no_proxy;
  वर्ण *env_no_proxy = रिक्त;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  यदि(दत्त->state.origin->scheme->flags & PROTOPT_NONETWORK)
    प्रत्यागम TRUE;

  no_proxy = CURL_EASY_STR(दत्त, STRING_NOPROXY);
  यदि(!no_proxy) {
    ध्रुव वर्ण *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    यदि(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    यदि(env_no_proxy)
      infof(दत्त, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(दत्त->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  प्रत्यागम do_not_proxy;
}

CURLcode Curl_proxy_init_conn(संरचना Curl_easy *दत्त,
                              संरचना connectdata *conn)
{
  वर्ण *proxy = रिक्त;
  वर्ण *pre_proxy = रिक्त;
  ध्रुव वर्ण *str = रिक्त;
  bool do_env_detect = TRUE;
  CURLcode परिणाम = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->द्वयङ्काः.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  यदि(proxy_do_not_proxy(दत्त))
    गच्छ बहिर्गमनम्;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(दत्त, STRING_PROXY);
  यदि(str) {
    यदि(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      यदि(!proxy) {
        failf(दत्त, "memory shortage");
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
    }
    अन्यथा
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(दत्त, STRING_PRE_PROXY);
  यदि(str) {
    यदि(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      यदि(!pre_proxy) {
        failf(दत्त, "memory shortage");
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
    }
    अन्यथा
      do_env_detect = FALSE;
  }

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  यदि(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(दत्त, conn->scheme);
#अन्यथा
  (शून्य)do_env_detect;
#यदि_अन्तः /* CURL_DISABLE_HTTP */

  यदि(!proxy && !pre_proxy)
    गच्छ बहिर्गमनम्;

  यदि(pre_proxy) {
    परिणाम = parse_proxy(दत्त, pre_proxy, TRUE, &conn->socks_proxy);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  यदि(proxy) {
    परिणाम = parse_proxy(दत्त, proxy, FALSE, &conn->http_proxy);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    विकल्प(conn->http_proxy.proxytype) {
    पक्ष CURLPROXY_SOCKS4:
    पक्ष CURLPROXY_SOCKS4A:
    पक्ष CURLPROXY_SOCKS5:
    पक्ष CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      यदि(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(दत्त, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        परिणाम = CURLE_COULDNT_RESOLVE_PROXY;
        गच्छ बहिर्गमनम्;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      स्मृतिं_पूरय(&conn->http_proxy, 0, परिमाण(conn->http_proxy));
      भङ्ग;
    स्वभाव:
      /* all other types are HTTP */
      भङ्ग;
    }
  }

  यदि(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#यदि_परिभाषितम् CURL_DISABLE_HTTP
  यदि(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
    गच्छ बहिर्गमनम्;
  }

#अन्यथा /* CURL_DISABLE_HTTP */
  यदि(conn->http_proxy.peer) {
    ध्रुव संरचना Curl_scheme *scheme = दत्त->state.origin->scheme;
    bool tunnel_proxy = (bool)दत्त->समुच्चयः.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    यदि(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      यदि(conn->द्वारा_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      अन्यथा यदि(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      अन्यथा यदि(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      अन्यथा यदि(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    यदि(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->द्वारा_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->द्वयङ्काः.origin_is_proxy = TRUE;
    }

#यदि_अपरिभाषितम् CURL_DISABLE_DIGEST_AUTH
    यदि(!Curl_safecmp(दत्त->state.envproxy, proxy)) {
      /* proxy changed */
      Curl_auth_digest_cleanup(&दत्त->state.proxydigest);
      curlx_free(दत्त->state.envproxy);
      दत्त->state.envproxy = curlx_strdup(proxy);
    }
#यदि_अन्तः
  }
#यदि_अन्तः /* !CURL_DISABLE_HTTP */

बहिर्गमनम्:
  curlx_free(pre_proxy);
  curlx_free(proxy);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* CURL_DISABLE_PROXY */
