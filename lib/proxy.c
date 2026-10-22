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
#समावेश "curl_setup.h"

#अगर_अघोषित CURL_DISABLE_PROXY

#समावेश "urldata.h"
#समावेश "curl_trc.h"
#समावेश "protocol.h"
#समावेश "proxy.h"
#समावेश "http_proxy.h"
#समावेश "strcase.h"
#समावेश "url.h"
#समावेश "vauth/vauth.h"
#समावेश "curlx/inet_pton.h"
#समावेश "curlx/strparse.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_match(स्थिर अक्षर *ipv4,    /* 1.2.3.4 address */
                          स्थिर अक्षर *network, /* 1.2.3.4 address */
                          पूर्णांक bits);
UNITTEST bool cidr4_match(स्थिर अक्षर *ipv4,    /* 1.2.3.4 address */
                          स्थिर अक्षर *network, /* 1.2.3.4 address */
                          पूर्णांक bits)
{
  अचिन्हित पूर्णांक address = 0;
  अचिन्हित पूर्णांक check = 0;

  अगर(bits > 32)
    /* strange input */
    वापस FALSE;

  अगर(curlx_inet_pton(AF_INET, ipv4, &address) != 1)
    वापस FALSE;
  अगर(curlx_inet_pton(AF_INET, network, &check) != 1)
    वापस FALSE;

  अगर((bits > 0) && (bits != 32)) {
    अचिन्हित पूर्णांक mask = 0xffffffff << (32 - bits);
    अचिन्हित पूर्णांक haddr = htonl(address);
    अचिन्हित पूर्णांक hcheck = htonl(check);
#अगर 0
    curl_mfprintf(मानक_त्रुटि, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, network, hcheck, bits, mask,
                  (haddr ^ hcheck) & mask);
#पूर्ण_अगर
    अगर((haddr ^ hcheck) & mask)
      वापस FALSE;
    वापस TRUE;
  }
  अन्यथा अगर(!bits && !check)
    /* 0.0.0.0/0 matches everything */
    वापस TRUE;

  वापस address == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_match(स्थिर अक्षर *ipv6, स्थिर अक्षर *network,
                          पूर्णांक bits);
UNITTEST bool cidr6_match(स्थिर अक्षर *ipv6, स्थिर अक्षर *network,
                          पूर्णांक bits)
{
#अगर_घोषित USE_IPV6
  अटल स्थिर अक्षर null6[16]= { 0 };
  अचिन्हित पूर्णांक bytes;
  अचिन्हित पूर्णांक rest;
  अचिन्हित अक्षर address[16];
  अचिन्हित अक्षर check[16];

  अगर(bits == -1)
    bits = 128;

  bytes = bits / 8;
  rest = bits & 0x07;
  अगर((bytes > 16) || ((bytes == 16) && rest))
    वापस FALSE;
  अगर(curlx_inet_pton(AF_INET6, ipv6, address) != 1)
    वापस FALSE;
  अगर(curlx_inet_pton(AF_INET6, network, check) != 1)
    वापस FALSE;
  अगर((bytes == 16) && !bits && !स_भेद(null6, check, 16))
    /* ::/0 matches everything */
    वापस TRUE;
  अगर(bytes && स_भेद(address, check, bytes))
    वापस FALSE;
  अगर(rest && ((address[bytes] ^ check[bytes]) & (0xff << (8 - rest))))
    वापस FALSE;

  वापस TRUE;
#अन्यथा
  (व्योम)ipv6;
  (व्योम)network;
  (व्योम)bits;
  वापस FALSE;
#पूर्ण_अगर
}

क्रमागत nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

अटल bool match_host(स्थिर अक्षर *token, माप_प्रकार tokenlen,
                       स्थिर अक्षर *name, माप_प्रकार namelen)
{
  bool match = FALSE;

  /* ignore trailing dots in the token to check */
  अगर(token[tokenlen - 1] == '.')
    tokenlen--;

  अगर(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  अगर(tokenlen == namelen)
    /* case A, exact match */
    match = curl_strnequal(token, name, namelen);
  अन्यथा अगर(tokenlen < namelen) {
    /* case B, tailmatch domain */
    match = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  वापस match;
}

अटल bool match_ip(पूर्णांक type, स्थिर अक्षर *token, माप_प्रकार tokenlen,
                     स्थिर अक्षर *name)
{
  अक्षर *slash;
  पूर्णांक bits = -1; /* negative means not provided */
  अक्षर checkip[128];
  अगर(tokenlen >= माप(checkip))
    /* this cannot match */
    वापस FALSE;
  /* copy the check name to a temp buffer */
  स_नकल(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = म_अक्षर(checkip, '/');
  /* if the slash is part of this token, use it */
  अगर(slash) {
    curl_off_t मूल्य;
    स्थिर अक्षर *p = &slash[1];
    अगर(curlx_str_संख्या(&p, &मूल्य, 128) || *p)
      वापस FALSE;
    /* a too large value is rejected in the cidr function below */
    bits = (पूर्णांक)मूल्य;
    *slash = 0; /* null-terminate there */
  }
  अगर(type == TYPE_IPV6)
    वापस cidr6_match(name, checkip, bits);
  अन्यथा
    वापस cidr4_match(name, checkip, bits);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(स्थिर अक्षर *name, स्थिर अक्षर *no_proxy);
UNITTEST bool proxy_check_noproxy(स्थिर अक्षर *name, स्थिर अक्षर *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  अगर(!name || name[0] == '\0')
    वापस FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  अगर(no_proxy && no_proxy[0]) {
    स्थिर अक्षर *p = no_proxy;
    माप_प्रकार namelen;
    अक्षर address[16];
    क्रमागत nametype type = TYPE_HOST;
    अगर(!म_भेद("*", no_proxy))
      वापस TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = म_माप(name);
    अगर(curlx_inet_pton(AF_INET, name, &address) == 1)
      type = TYPE_IPV4;
#अगर_घोषित USE_IPV6
    अन्यथा अगर(curlx_inet_pton(AF_INET6, name, &address) == 1)
      type = TYPE_IPV6;
#पूर्ण_अगर
    अन्यथा {
      /* ignore trailing dots in the hostname */
      अगर(name[namelen - 1] == '.')
        namelen--;
    }

    जबतक(*p) {
      स्थिर अक्षर *token;
      माप_प्रकार tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      जबतक(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      अगर(tokenlen) {
        bool match = FALSE;
        अगर(type == TYPE_HOST)
          match = match_host(token, tokenlen, name, namelen);
        अन्यथा
          match = match_ip(type, token, tokenlen, name);

        अगर(match)
          वापस TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      अगर(*p != ',')
        अवरोध;
      /* pass any number of commas */
      जबतक(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  वापस FALSE;
}

#अगर_अघोषित CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
अटल अक्षर *proxy_detect_proxy(काष्ठा Curl_easy *तथ्य,
                                स्थिर काष्ठा Curl_scheme *scheme)
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
  स्थिर अक्षर *env_name = शून्य;
  अक्षर *proxy = शून्य;
  अक्षर name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  अगर((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, माप(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    अगर(!proxy) {
      Curl_strntoupper(name_buf, name_buf, माप(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  अगर(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    अगर(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  अन्यथा अगर(!proxy &&
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

  अगर(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    अगर(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  अगर(proxy)
    infof(तथ्य, "Uses proxy env variable %s == '%s'", env_name, proxy);

  वापस proxy;
}
#पूर्ण_अगर /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
अटल CURLcode parse_proxy(काष्ठा Curl_easy *तथ्य,
                            स्थिर अक्षर *proxy,
                            bool for_pre_proxy,
                            काष्ठा proxy_info *proxyinfo)
{
  अक्षर *proxyuser = शून्य;
  अक्षर *proxypasswd = शून्य;
  अक्षर *scheme = शून्य;
  CURLcode परिणाम = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : तथ्य->set.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  अगर(!uhp) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_set(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  अगर(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    अगर(!uc) {
      परिणाम = Curl_scheme_to_proxytype(तथ्य, scheme, &proxytype, proxy);
      अगर(परिणाम)
        जाओ error;
    }
    अन्यथा अगर(uc != CURLUE_NO_SCHEME) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  अन्यथा {
    failf(तथ्य, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    परिणाम = CURLE_COULDNT_RESOLVE_PROXY;
    जाओ error;
  }

  परिणाम = Curl_peer_from_proxy_url(uhp, तथ्य, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  अगर(परिणाम)
    जाओ error;

  चयन(proxytype) {
    हाल CURLPROXY_HTTP:
    हाल CURLPROXY_HTTP_1_0:
    हाल CURLPROXY_HTTPS:
    हाल CURLPROXY_HTTPS2:
    हाल CURLPROXY_HTTPS3:
      अगर(for_pre_proxy) {
        failf(तथ्य, "Unsupported pre-proxy type for \'%s\'", proxy);
        परिणाम = CURLE_COULDNT_RESOLVE_PROXY;
        जाओ error;
      }
      अवरोध;
    हाल CURLPROXY_SOCKS4:
    हाल CURLPROXY_SOCKS4A:
    हाल CURLPROXY_SOCKS5:
    हाल CURLPROXY_SOCKS5_HOSTNAME:
      अवरोध;
    शेष:
      failf(तथ्य, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      परिणाम = CURLE_COULDNT_RESOLVE_PROXY;
      जाओ error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  अगर(uc && (uc != CURLUE_NO_USER)) {
    परिणाम = Curl_uc_to_curlcode(uc);
    जाओ error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  अगर(uc && (uc != CURLUE_NO_PASSWORD)) {
    परिणाम = Curl_uc_to_curlcode(uc);
    जाओ error;
  }

  अगर(proxyuser || proxypasswd) {
    परिणाम = Curl_creds_create(proxyuser, proxypasswd, शून्य, शून्य,
                               CURL_EASY_STR(तथ्य, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    अगर(परिणाम)
      जाओ error;
  }
  अन्यथा अगर(!for_pre_proxy &&
          (CURL_EASY_STR(तथ्य, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(तथ्य, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(तथ्य, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    परिणाम = Curl_creds_create(CURL_EASY_STR(तथ्य, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(तथ्य, STRING_PROXYPASSWORD),
                               शून्य, शून्य,
                               CURL_EASY_STR(तथ्य, STRING_PROXY_SERVICE_NAME),
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
#अगर_घोषित DEBUGBUILD
  अगर(!परिणाम) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#पूर्ण_अगर
  वापस परिणाम;
}

/* Is transfer's origin exempted from proxy use? */
अटल bool proxy_do_not_proxy(काष्ठा Curl_easy *तथ्य)
{
  स्थिर अक्षर *no_proxy;
  अक्षर *env_no_proxy = शून्य;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  अगर(तथ्य->state.origin->scheme->flags & PROTOPT_NONETWORK)
    वापस TRUE;

  no_proxy = CURL_EASY_STR(तथ्य, STRING_NOPROXY);
  अगर(!no_proxy) {
    स्थिर अक्षर *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    अगर(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    अगर(env_no_proxy)
      infof(तथ्य, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(तथ्य->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  वापस do_not_proxy;
}

CURLcode Curl_proxy_init_conn(काष्ठा Curl_easy *तथ्य,
                              काष्ठा connectdata *conn)
{
  अक्षर *proxy = शून्य;
  अक्षर *pre_proxy = शून्य;
  स्थिर अक्षर *str = शून्य;
  bool do_env_detect = TRUE;
  CURLcode परिणाम = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->bits.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  अगर(proxy_do_not_proxy(तथ्य))
    जाओ out;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(तथ्य, STRING_PROXY);
  अगर(str) {
    अगर(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      अगर(!proxy) {
        failf(तथ्य, "memory shortage");
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
    }
    अन्यथा
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(तथ्य, STRING_PRE_PROXY);
  अगर(str) {
    अगर(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      अगर(!pre_proxy) {
        failf(तथ्य, "memory shortage");
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
    }
    अन्यथा
      do_env_detect = FALSE;
  }

#अगर_अघोषित CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  अगर(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(तथ्य, conn->scheme);
#अन्यथा
  (व्योम)do_env_detect;
#पूर्ण_अगर /* CURL_DISABLE_HTTP */

  अगर(!proxy && !pre_proxy)
    जाओ out;

  अगर(pre_proxy) {
    परिणाम = parse_proxy(तथ्य, pre_proxy, TRUE, &conn->socks_proxy);
    अगर(परिणाम)
      जाओ out;
  }

  अगर(proxy) {
    परिणाम = parse_proxy(तथ्य, proxy, FALSE, &conn->http_proxy);
    अगर(परिणाम)
      जाओ out;

    चयन(conn->http_proxy.proxytype) {
    हाल CURLPROXY_SOCKS4:
    हाल CURLPROXY_SOCKS4A:
    हाल CURLPROXY_SOCKS5:
    हाल CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      अगर(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(तथ्य, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        परिणाम = CURLE_COULDNT_RESOLVE_PROXY;
        जाओ out;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      स_रखो(&conn->http_proxy, 0, माप(conn->http_proxy));
      अवरोध;
    शेष:
      /* all other types are HTTP */
      अवरोध;
    }
  }

  अगर(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#अगर_घोषित CURL_DISABLE_HTTP
  अगर(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    परिणाम = CURLE_UNSUPPORTED_PROTOCOL;
    जाओ out;
  }

#अन्यथा /* CURL_DISABLE_HTTP */
  अगर(conn->http_proxy.peer) {
    स्थिर काष्ठा Curl_scheme *scheme = तथ्य->state.origin->scheme;
    bool tunnel_proxy = (bool)तथ्य->set.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    अगर(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      अगर(conn->via_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      अन्यथा अगर(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      अन्यथा अगर(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      अन्यथा अगर(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    अगर(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->via_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->bits.origin_is_proxy = TRUE;
    }

#अगर_अघोषित CURL_DISABLE_DIGEST_AUTH
    अगर(!Curl_safecmp(तथ्य->state.envproxy, proxy)) {
      /* proxy changed */
      Curl_auth_digest_cleanup(&तथ्य->state.proxydigest);
      curlx_free(तथ्य->state.envproxy);
      तथ्य->state.envproxy = curlx_strdup(proxy);
    }
#पूर्ण_अगर
  }
#पूर्ण_अगर /* !CURL_DISABLE_HTTP */

out:
  curlx_free(pre_proxy);
  curlx_free(proxy);
  वापस परिणाम;
}

#पूर्ण_अगर /* CURL_DISABLE_PROXY */
