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
#ضمن "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "protocol.h"
#ضمن "proxy.h"
#ضمن "http_proxy.h"
#ضمن "strcase.h"
#ضمن "url.h"
#ضمن "vauth/vauth.h"
#ضمن "curlx/inet_pton.h"
#ضمن "curlx/strparse.h"

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_match(ثابت حرف *ipv4,    /* 1.2.3.4 address */
                          ثابت حرف *network, /* 1.2.3.4 address */
                          صحيح bits);
UNITTEST bool cidr4_match(ثابت حرف *ipv4,    /* 1.2.3.4 address */
                          ثابت حرف *network, /* 1.2.3.4 address */
                          صحيح bits)
{
  unsigned صحيح address = 0;
  unsigned صحيح check = 0;

  אם(bits > 32)
    /* strange input */
    החזר FALSE;

  אם(curlx_inet_pton(AF_INET, ipv4, &address) != 1)
    החזר FALSE;
  אם(curlx_inet_pton(AF_INET, network, &check) != 1)
    החזר FALSE;

  אם((bits > 0) && (bits != 32)) {
    unsigned صحيح mask = 0xffffffff << (32 - bits);
    unsigned صحيح haddr = htonl(address);
    unsigned صحيح hcheck = htonl(check);
#אם 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, network, hcheck, bits, mask,
                  (haddr ^ hcheck) & mask);
#endif
    אם((haddr ^ hcheck) & mask)
      החזר FALSE;
    החזר TRUE;
  }
  אחרת אם(!bits && !check)
    /* 0.0.0.0/0 matches everything */
    החזר TRUE;

  החזר address == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_match(ثابت حرف *ipv6, ثابت حرف *network,
                          صحيح bits);
UNITTEST bool cidr6_match(ثابت حرف *ipv6, ثابت حرف *network,
                          صحيح bits)
{
#ifdef USE_IPV6
  ساكن ثابت حرف null6[16]= { 0 };
  unsigned صحيح bytes;
  unsigned صحيح rest;
  unsigned حرف address[16];
  unsigned حرف check[16];

  אם(bits == -1)
    bits = 128;

  bytes = bits / 8;
  rest = bits & 0x07;
  אם((bytes > 16) || ((bytes == 16) && rest))
    החזר FALSE;
  אם(curlx_inet_pton(AF_INET6, ipv6, address) != 1)
    החזר FALSE;
  אם(curlx_inet_pton(AF_INET6, network, check) != 1)
    החזר FALSE;
  אם((bytes == 16) && !bits && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    החזר TRUE;
  אם(bytes && memcmp(address, check, bytes))
    החזר FALSE;
  אם(rest && ((address[bytes] ^ check[bytes]) & (0xff << (8 - rest))))
    החזר FALSE;

  החזר TRUE;
#אחרת
  (فراغ)ipv6;
  (فراغ)network;
  (فراغ)bits;
  החזר FALSE;
#endif
}

تعداد nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

ساكن bool match_host(ثابت حرف *token, size_t tokenlen,
                       ثابت حرف *name, size_t namelen)
{
  bool match = FALSE;

  /* ignore trailing dots in the token to check */
  אם(token[tokenlen - 1] == '.')
    tokenlen--;

  אם(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  אם(tokenlen == namelen)
    /* case A, exact match */
    match = curl_strnequal(token, name, namelen);
  אחרת אם(tokenlen < namelen) {
    /* case B, tailmatch domain */
    match = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  החזר match;
}

ساكن bool match_ip(صحيح type, ثابت حرف *token, size_t tokenlen,
                     ثابت حرف *name)
{
  حرف *slash;
  صحيح bits = -1; /* negative means not provided */
  حرف checkip[128];
  אם(tokenlen >= حجم(checkip))
    /* this cannot match */
    החזר FALSE;
  /* copy the check name to a temp buffer */
  memcpy(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  אם(slash) {
    curl_off_t value;
    ثابت حرف *p = &slash[1];
    אם(curlx_str_number(&p, &value, 128) || *p)
      החזר FALSE;
    /* a too large value is rejected in the cidr function below */
    bits = (صحيح)value;
    *slash = 0; /* null-terminate there */
  }
  אם(type == TYPE_IPV6)
    החזר cidr6_match(name, checkip, bits);
  אחרת
    החזר cidr4_match(name, checkip, bits);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(ثابت حرف *name, ثابت حرف *no_proxy);
UNITTEST bool proxy_check_noproxy(ثابت حرف *name, ثابت حرف *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  אם(!name || name[0] == '\0')
    החזר FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  אם(no_proxy && no_proxy[0]) {
    ثابت حرف *p = no_proxy;
    size_t namelen;
    حرف address[16];
    تعداد nametype type = TYPE_HOST;
    אם(!strcmp("*", no_proxy))
      החזר TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = strlen(name);
    אם(curlx_inet_pton(AF_INET, name, &address) == 1)
      type = TYPE_IPV4;
#ifdef USE_IPV6
    אחרת אם(curlx_inet_pton(AF_INET6, name, &address) == 1)
      type = TYPE_IPV6;
#endif
    אחרת {
      /* ignore trailing dots in the hostname */
      אם(name[namelen - 1] == '.')
        namelen--;
    }

    כלעוד(*p) {
      ثابت حرف *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      כלעוד(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      אם(tokenlen) {
        bool match = FALSE;
        אם(type == TYPE_HOST)
          match = match_host(token, tokenlen, name, namelen);
        אחרת
          match = match_ip(type, token, tokenlen, name);

        אם(match)
          החזר TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      אם(*p != ',')
        اكسر;
      /* pass any number of commas */
      כלעוד(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  החזר FALSE;
}

#ifndef CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
ساكن حرف *proxy_detect_proxy(بنية Curl_easy *data,
                                ثابت بنية Curl_scheme *scheme)
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
  ثابت حرف *env_name = عدم;
  حرف *proxy = عدم;
  حرف name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  אם((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, حجم(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    אם(!proxy) {
      Curl_strntoupper(name_buf, name_buf, حجم(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  אם(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    אם(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  אחרת אם(!proxy &&
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

  אם(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    אם(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  אם(proxy)
    infof(data, "Uses proxy env variable %s == '%s'", env_name, proxy);

  החזר proxy;
}
#endif /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
ساكن CURLcode parse_proxy(بنية Curl_easy *data,
                            ثابت حرف *proxy,
                            bool for_pre_proxy,
                            بنية proxy_info *proxyinfo)
{
  حرف *proxyuser = عدم;
  حرف *proxypasswd = عدم;
  حرف *scheme = عدم;
  CURLcode result = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : data->set.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  אם(!uhp) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_set(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  אם(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    אם(!uc) {
      result = Curl_scheme_to_proxytype(data, scheme, &proxytype, proxy);
      אם(result)
        اذهب error;
    }
    אחרת אם(uc != CURLUE_NO_SCHEME) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  אחרת {
    failf(data, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    result = CURLE_COULDNT_RESOLVE_PROXY;
    اذهب error;
  }

  result = Curl_peer_from_proxy_url(uhp, data, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  אם(result)
    اذهب error;

  بدل(proxytype) {
    حالة CURLPROXY_HTTP:
    حالة CURLPROXY_HTTP_1_0:
    حالة CURLPROXY_HTTPS:
    حالة CURLPROXY_HTTPS2:
    حالة CURLPROXY_HTTPS3:
      אם(for_pre_proxy) {
        failf(data, "Unsupported pre-proxy type for \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        اذهب error;
      }
      اكسر;
    حالة CURLPROXY_SOCKS4:
    حالة CURLPROXY_SOCKS4A:
    حالة CURLPROXY_SOCKS5:
    حالة CURLPROXY_SOCKS5_HOSTNAME:
      اكسر;
    افتراضي:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      اذهب error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  אם(uc && (uc != CURLUE_NO_USER)) {
    result = Curl_uc_to_curlcode(uc);
    اذهب error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  אם(uc && (uc != CURLUE_NO_PASSWORD)) {
    result = Curl_uc_to_curlcode(uc);
    اذهب error;
  }

  אם(proxyuser || proxypasswd) {
    result = Curl_creds_create(proxyuser, proxypasswd, عدم, عدم,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    אם(result)
      اذهب error;
  }
  אחרת אם(!for_pre_proxy &&
          (CURL_EASY_STR(data, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(data, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    result = Curl_creds_create(CURL_EASY_STR(data, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(data, STRING_PROXYPASSWORD),
                               عدم, عدم,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  אחרת
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ifdef DEBUGBUILD
  אם(!result) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#endif
  החזר result;
}

/* Is transfer's origin exempted from proxy use? */
ساكن bool proxy_do_not_proxy(بنية Curl_easy *data)
{
  ثابت حرف *no_proxy;
  حرف *env_no_proxy = عدم;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  אם(data->state.origin->scheme->flags & PROTOPT_NONETWORK)
    החזר TRUE;

  no_proxy = CURL_EASY_STR(data, STRING_NOPROXY);
  אם(!no_proxy) {
    ثابت حرف *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    אם(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    אם(env_no_proxy)
      infof(data, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(data->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  החזר do_not_proxy;
}

CURLcode Curl_proxy_init_conn(بنية Curl_easy *data,
                              بنية connectdata *conn)
{
  حرف *proxy = عدم;
  حرف *pre_proxy = عدم;
  ثابت حرف *str = عدم;
  bool do_env_detect = TRUE;
  CURLcode result = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->bits.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  אם(proxy_do_not_proxy(data))
    اذهب out;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(data, STRING_PROXY);
  אם(str) {
    אם(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      אם(!proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
    }
    אחרת
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(data, STRING_PRE_PROXY);
  אם(str) {
    אם(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      אם(!pre_proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
    }
    אחרת
      do_env_detect = FALSE;
  }

#ifndef CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  אם(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(data, conn->scheme);
#אחרת
  (فراغ)do_env_detect;
#endif /* CURL_DISABLE_HTTP */

  אם(!proxy && !pre_proxy)
    اذهب out;

  אם(pre_proxy) {
    result = parse_proxy(data, pre_proxy, TRUE, &conn->socks_proxy);
    אם(result)
      اذهب out;
  }

  אם(proxy) {
    result = parse_proxy(data, proxy, FALSE, &conn->http_proxy);
    אם(result)
      اذهب out;

    بدل(conn->http_proxy.proxytype) {
    حالة CURLPROXY_SOCKS4:
    حالة CURLPROXY_SOCKS4A:
    حالة CURLPROXY_SOCKS5:
    حالة CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      אם(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(data, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        اذهب out;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, حجم(conn->http_proxy));
      اكسر;
    افتراضي:
      /* all other types are HTTP */
      اكسر;
    }
  }

  אם(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ifdef CURL_DISABLE_HTTP
  אם(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    result = CURLE_UNSUPPORTED_PROTOCOL;
    اذهب out;
  }

#אחרת /* CURL_DISABLE_HTTP */
  אם(conn->http_proxy.peer) {
    ثابت بنية Curl_scheme *scheme = data->state.origin->scheme;
    bool tunnel_proxy = (bool)data->set.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    אם(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      אם(conn->via_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      אחרת אם(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      אחרת אם(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      אחרת אם(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    אם(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->via_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->bits.origin_is_proxy = TRUE;
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    אם(!Curl_safecmp(data->state.envproxy, proxy)) {
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
  החזר result;
}

#endif /* CURL_DISABLE_PROXY */
