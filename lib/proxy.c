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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "protocol.h"
#અન્તર્ભાવ "proxy.h"
#અન્તર્ભાવ "http_proxy.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "vauth/vauth.h"
#અન્તર્ભાવ "curlx/inet_pton.h"
#અન્તર્ભાવ "curlx/strparse.h"

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_match(અચલ અક્ષર *ipv4,    /* 1.2.3.4 address */
                          અચલ અક્ષર *network, /* 1.2.3.4 address */
                          પૂર્ણાંક bits);
UNITTEST bool cidr4_match(અચલ અક્ષર *ipv4,    /* 1.2.3.4 address */
                          અચલ અક્ષર *network, /* 1.2.3.4 address */
                          પૂર્ણાંક bits)
{
  unsigned પૂર્ણાંક address = 0;
  unsigned પૂર્ણાંક check = 0;

  જો(bits > 32)
    /* strange input */
    પાછા FALSE;

  જો(curlx_inet_pton(AF_INET, ipv4, &address) != 1)
    પાછા FALSE;
  જો(curlx_inet_pton(AF_INET, network, &check) != 1)
    પાછા FALSE;

  જો((bits > 0) && (bits != 32)) {
    unsigned પૂર્ણાંક mask = 0xffffffff << (32 - bits);
    unsigned પૂર્ણાંક haddr = htonl(address);
    unsigned પૂર્ણાંક hcheck = htonl(check);
#જો 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, network, hcheck, bits, mask,
                  (haddr ^ hcheck) & mask);
#endif
    જો((haddr ^ hcheck) & mask)
      પાછા FALSE;
    પાછા TRUE;
  }
  નહીંતર જો(!bits && !check)
    /* 0.0.0.0/0 matches everything */
    પાછા TRUE;

  પાછા address == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_match(અચલ અક્ષર *ipv6, અચલ અક્ષર *network,
                          પૂર્ણાંક bits);
UNITTEST bool cidr6_match(અચલ અક્ષર *ipv6, અચલ અક્ષર *network,
                          પૂર્ણાંક bits)
{
#ifdef USE_IPV6
  સ્થિર અચલ અક્ષર null6[16]= { 0 };
  unsigned પૂર્ણાંક bytes;
  unsigned પૂર્ણાંક rest;
  unsigned અક્ષર address[16];
  unsigned અક્ષર check[16];

  જો(bits == -1)
    bits = 128;

  bytes = bits / 8;
  rest = bits & 0x07;
  જો((bytes > 16) || ((bytes == 16) && rest))
    પાછા FALSE;
  જો(curlx_inet_pton(AF_INET6, ipv6, address) != 1)
    પાછા FALSE;
  જો(curlx_inet_pton(AF_INET6, network, check) != 1)
    પાછા FALSE;
  જો((bytes == 16) && !bits && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    પાછા TRUE;
  જો(bytes && memcmp(address, check, bytes))
    પાછા FALSE;
  જો(rest && ((address[bytes] ^ check[bytes]) & (0xff << (8 - rest))))
    પાછા FALSE;

  પાછા TRUE;
#નહીંતર
  (શૂન્ય)ipv6;
  (શૂન્ય)network;
  (શૂન્ય)bits;
  પાછા FALSE;
#endif
}

ગણના nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

સ્થિર bool match_host(અચલ અક્ષર *token, size_t tokenlen,
                       અચલ અક્ષર *name, size_t namelen)
{
  bool match = FALSE;

  /* ignore trailing dots in the token to check */
  જો(token[tokenlen - 1] == '.')
    tokenlen--;

  જો(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  જો(tokenlen == namelen)
    /* case A, exact match */
    match = curl_strnequal(token, name, namelen);
  નહીંતર જો(tokenlen < namelen) {
    /* case B, tailmatch domain */
    match = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  પાછા match;
}

સ્થિર bool match_ip(પૂર્ણાંક type, અચલ અક્ષર *token, size_t tokenlen,
                     અચલ અક્ષર *name)
{
  અક્ષર *slash;
  પૂર્ણાંક bits = -1; /* negative means not provided */
  અક્ષર checkip[128];
  જો(tokenlen >= કદ(checkip))
    /* this cannot match */
    પાછા FALSE;
  /* copy the check name to a temp buffer */
  memcpy(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  જો(slash) {
    curl_off_t કિંમત;
    અચલ અક્ષર *p = &slash[1];
    જો(curlx_str_સંખ્યા(&p, &કિંમત, 128) || *p)
      પાછા FALSE;
    /* a too large value is rejected in the cidr function below */
    bits = (પૂર્ણાંક)કિંમત;
    *slash = 0; /* null-terminate there */
  }
  જો(type == TYPE_IPV6)
    પાછા cidr6_match(name, checkip, bits);
  નહીંતર
    પાછા cidr4_match(name, checkip, bits);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(અચલ અક્ષર *name, અચલ અક્ષર *no_proxy);
UNITTEST bool proxy_check_noproxy(અચલ અક્ષર *name, અચલ અક્ષર *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  જો(!name || name[0] == '\0')
    પાછા FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  જો(no_proxy && no_proxy[0]) {
    અચલ અક્ષર *p = no_proxy;
    size_t namelen;
    અક્ષર address[16];
    ગણના nametype type = TYPE_HOST;
    જો(!strcmp("*", no_proxy))
      પાછા TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = strlen(name);
    જો(curlx_inet_pton(AF_INET, name, &address) == 1)
      type = TYPE_IPV4;
#ifdef USE_IPV6
    નહીંતર જો(curlx_inet_pton(AF_INET6, name, &address) == 1)
      type = TYPE_IPV6;
#endif
    નહીંતર {
      /* ignore trailing dots in the hostname */
      જો(name[namelen - 1] == '.')
        namelen--;
    }

    જ્યારે(*p) {
      અચલ અક્ષર *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      જ્યારે(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      જો(tokenlen) {
        bool match = FALSE;
        જો(type == TYPE_HOST)
          match = match_host(token, tokenlen, name, namelen);
        નહીંતર
          match = match_ip(type, token, tokenlen, name);

        જો(match)
          પાછા TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      જો(*p != ',')
        તોડો;
      /* pass any number of commas */
      જ્યારે(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  પાછા FALSE;
}

#ifndef CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
સ્થિર અક્ષર *proxy_detect_proxy(સંરચના Curl_easy *data,
                                અચલ સંરચના Curl_scheme *scheme)
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
  અચલ અક્ષર *env_name = ખાલી;
  અક્ષર *proxy = ખાલી;
  અક્ષર name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  જો((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, કદ(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    જો(!proxy) {
      Curl_strntoupper(name_buf, name_buf, કદ(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  જો(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    જો(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  નહીંતર જો(!proxy &&
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

  જો(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    જો(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  જો(proxy)
    infof(data, "Uses proxy env variable %s == '%s'", env_name, proxy);

  પાછા proxy;
}
#endif /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
સ્થિર CURLcode parse_proxy(સંરચના Curl_easy *data,
                            અચલ અક્ષર *proxy,
                            bool for_pre_proxy,
                            સંરચના proxy_info *proxyinfo)
{
  અક્ષર *proxyuser = ખાલી;
  અક્ષર *proxypasswd = ખાલી;
  અક્ષર *scheme = ખાલી;
  CURLcode result = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : data->set.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  જો(!uhp) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_set(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  જો(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    જો(!uc) {
      result = Curl_scheme_to_proxytype(data, scheme, &proxytype, proxy);
      જો(result)
        જાઓ error;
    }
    નહીંતર જો(uc != CURLUE_NO_SCHEME) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  નહીંતર {
    failf(data, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    result = CURLE_COULDNT_RESOLVE_PROXY;
    જાઓ error;
  }

  result = Curl_peer_from_proxy_url(uhp, data, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  જો(result)
    જાઓ error;

  પસંદગી(proxytype) {
    સ્થિતિ CURLPROXY_HTTP:
    સ્થિતિ CURLPROXY_HTTP_1_0:
    સ્થિતિ CURLPROXY_HTTPS:
    સ્થિતિ CURLPROXY_HTTPS2:
    સ્થિતિ CURLPROXY_HTTPS3:
      જો(for_pre_proxy) {
        failf(data, "Unsupported pre-proxy type for \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        જાઓ error;
      }
      તોડો;
    સ્થિતિ CURLPROXY_SOCKS4:
    સ્થિતિ CURLPROXY_SOCKS4A:
    સ્થિતિ CURLPROXY_SOCKS5:
    સ્થિતિ CURLPROXY_SOCKS5_HOSTNAME:
      તોડો;
    મૂળભૂત:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      જાઓ error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  જો(uc && (uc != CURLUE_NO_USER)) {
    result = Curl_uc_to_curlcode(uc);
    જાઓ error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  જો(uc && (uc != CURLUE_NO_PASSWORD)) {
    result = Curl_uc_to_curlcode(uc);
    જાઓ error;
  }

  જો(proxyuser || proxypasswd) {
    result = Curl_creds_create(proxyuser, proxypasswd, ખાલી, ખાલી,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    જો(result)
      જાઓ error;
  }
  નહીંતર જો(!for_pre_proxy &&
          (CURL_EASY_STR(data, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(data, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    result = Curl_creds_create(CURL_EASY_STR(data, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(data, STRING_PROXYPASSWORD),
                               ખાલી, ખાલી,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  નહીંતર
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ifdef DEBUGBUILD
  જો(!result) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#endif
  પાછા result;
}

/* Is transfer's origin exempted from proxy use? */
સ્થિર bool proxy_do_not_proxy(સંરચના Curl_easy *data)
{
  અચલ અક્ષર *no_proxy;
  અક્ષર *env_no_proxy = ખાલી;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  જો(data->state.origin->scheme->flags & PROTOPT_NONETWORK)
    પાછા TRUE;

  no_proxy = CURL_EASY_STR(data, STRING_NOPROXY);
  જો(!no_proxy) {
    અચલ અક્ષર *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    જો(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    જો(env_no_proxy)
      infof(data, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(data->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  પાછા do_not_proxy;
}

CURLcode Curl_proxy_init_conn(સંરચના Curl_easy *data,
                              સંરચના connectdata *conn)
{
  અક્ષર *proxy = ખાલી;
  અક્ષર *pre_proxy = ખાલી;
  અચલ અક્ષર *str = ખાલી;
  bool do_env_detect = TRUE;
  CURLcode result = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->bits.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  જો(proxy_do_not_proxy(data))
    જાઓ out;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(data, STRING_PROXY);
  જો(str) {
    જો(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      જો(!proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
    }
    નહીંતર
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(data, STRING_PRE_PROXY);
  જો(str) {
    જો(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      જો(!pre_proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
    }
    નહીંતર
      do_env_detect = FALSE;
  }

#ifndef CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  જો(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(data, conn->scheme);
#નહીંતર
  (શૂન્ય)do_env_detect;
#endif /* CURL_DISABLE_HTTP */

  જો(!proxy && !pre_proxy)
    જાઓ out;

  જો(pre_proxy) {
    result = parse_proxy(data, pre_proxy, TRUE, &conn->socks_proxy);
    જો(result)
      જાઓ out;
  }

  જો(proxy) {
    result = parse_proxy(data, proxy, FALSE, &conn->http_proxy);
    જો(result)
      જાઓ out;

    પસંદગી(conn->http_proxy.proxytype) {
    સ્થિતિ CURLPROXY_SOCKS4:
    સ્થિતિ CURLPROXY_SOCKS4A:
    સ્થિતિ CURLPROXY_SOCKS5:
    સ્થિતિ CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      જો(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(data, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        જાઓ out;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, કદ(conn->http_proxy));
      તોડો;
    મૂળભૂત:
      /* all other types are HTTP */
      તોડો;
    }
  }

  જો(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ifdef CURL_DISABLE_HTTP
  જો(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    result = CURLE_UNSUPPORTED_PROTOCOL;
    જાઓ out;
  }

#નહીંતર /* CURL_DISABLE_HTTP */
  જો(conn->http_proxy.peer) {
    અચલ સંરચના Curl_scheme *scheme = data->state.origin->scheme;
    bool tunnel_proxy = (bool)data->set.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    જો(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      જો(conn->via_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      નહીંતર જો(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      નહીંતર જો(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      નહીંતર જો(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    જો(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->via_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->bits.origin_is_proxy = TRUE;
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    જો(!Curl_safecmp(data->state.envproxy, proxy)) {
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
  પાછા result;
}

#endif /* CURL_DISABLE_PROXY */
