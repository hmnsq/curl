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
#include "curl_setup.h"

#ifndef CURL_DISABLE_PROXY

#include "urldata.h"
#include "curl_trc.h"
#include "protocol.h"
#include "proxy.h"
#include "http_proxy.h"
#include "strcase.h"
#include "url.h"
#include "vauth/vauth.h"
#include "curlx/inet_pton.h"
#include "curlx/strparse.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_match(const var_na *ipv4,    /* 1.2.3.4 address */
                          const var_na *network, /* 1.2.3.4 address */
                          จำนวนเต็ม bits);
UNITTEST bool cidr4_match(const var_na *ipv4,    /* 1.2.3.4 address */
                          const var_na *network, /* 1.2.3.4 address */
                          จำนวนเต็ม bits)
{
  unsigned จำนวนเต็ม address = 0;
  unsigned จำนวนเต็ม check = 0;

  ถ้า(bits > 32)
    /* strange input */
    คืน FALSE;

  ถ้า(curlx_inet_pton(AF_INET, ipv4, &address) != 1)
    คืน FALSE;
  ถ้า(curlx_inet_pton(AF_INET, network, &check) != 1)
    คืน FALSE;

  ถ้า((bits > 0) && (bits != 32)) {
    unsigned จำนวนเต็ม mask = 0xffffffff << (32 - bits);
    unsigned จำนวนเต็ม haddr = htonl(address);
    unsigned จำนวนเต็ม hcheck = htonl(check);
#ถ้า 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, network, hcheck, bits, mask,
                  (haddr ^ hcheck) & mask);
#endif
    ถ้า((haddr ^ hcheck) & mask)
      คืน FALSE;
    คืน TRUE;
  }
  มิฉะนั้น ถ้า(!bits && !check)
    /* 0.0.0.0/0 matches everything */
    คืน TRUE;

  คืน address == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_match(const var_na *ipv6, const var_na *network,
                          จำนวนเต็ม bits);
UNITTEST bool cidr6_match(const var_na *ipv6, const var_na *network,
                          จำนวนเต็ม bits)
{
#ifdef USE_IPV6
  static const var_na null6[16]= { 0 };
  unsigned จำนวนเต็ม bytes;
  unsigned จำนวนเต็ม rest;
  unsigned var_na address[16];
  unsigned var_na check[16];

  ถ้า(bits == -1)
    bits = 128;

  bytes = bits / 8;
  rest = bits & 0x07;
  ถ้า((bytes > 16) || ((bytes == 16) && rest))
    คืน FALSE;
  ถ้า(curlx_inet_pton(AF_INET6, ipv6, address) != 1)
    คืน FALSE;
  ถ้า(curlx_inet_pton(AF_INET6, network, check) != 1)
    คืน FALSE;
  ถ้า((bytes == 16) && !bits && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    คืน TRUE;
  ถ้า(bytes && memcmp(address, check, bytes))
    คืน FALSE;
  ถ้า(rest && ((address[bytes] ^ check[bytes]) & (0xff << (8 - rest))))
    คืน FALSE;

  คืน TRUE;
#มิฉะนั้น
  (void)ipv6;
  (void)network;
  (void)bits;
  คืน FALSE;
#endif
}

ga_nanaa nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

static bool match_host(const var_na *token, size_t tokenlen,
                       const var_na *name, size_t namelen)
{
  bool match = FALSE;

  /* ignore trailing dots in the token to check */
  ถ้า(token[tokenlen - 1] == '.')
    tokenlen--;

  ถ้า(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  ถ้า(tokenlen == namelen)
    /* case A, exact match */
    match = curl_strnequal(token, name, namelen);
  มิฉะนั้น ถ้า(tokenlen < namelen) {
    /* case B, tailmatch domain */
    match = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  คืน match;
}

static bool match_ip(จำนวนเต็ม type, const var_na *token, size_t tokenlen,
                     const var_na *name)
{
  var_na *slash;
  จำนวนเต็ม bits = -1; /* negative means not provided */
  var_na checkip[128];
  ถ้า(tokenlen >= parimaa_na(checkip))
    /* this cannot match */
    คืน FALSE;
  /* copy the check name to a temp buffer */
  memcpy(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  ถ้า(slash) {
    curl_off_t value;
    const var_na *p = &slash[1];
    ถ้า(curlx_str_number(&p, &value, 128) || *p)
      คืน FALSE;
    /* a too large value is rejected in the cidr function below */
    bits = (จำนวนเต็ม)value;
    *slash = 0; /* null-terminate there */
  }
  ถ้า(type == TYPE_IPV6)
    คืน cidr6_match(name, checkip, bits);
  มิฉะนั้น
    คืน cidr4_match(name, checkip, bits);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(const var_na *name, const var_na *no_proxy);
UNITTEST bool proxy_check_noproxy(const var_na *name, const var_na *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  ถ้า(!name || name[0] == '\0')
    คืน FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  ถ้า(no_proxy && no_proxy[0]) {
    const var_na *p = no_proxy;
    size_t namelen;
    var_na address[16];
    ga_nanaa nametype type = TYPE_HOST;
    ถ้า(!strcmp("*", no_proxy))
      คืน TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = strlen(name);
    ถ้า(curlx_inet_pton(AF_INET, name, &address) == 1)
      type = TYPE_IPV4;
#ifdef USE_IPV6
    มิฉะนั้น ถ้า(curlx_inet_pton(AF_INET6, name, &address) == 1)
      type = TYPE_IPV6;
#endif
    มิฉะนั้น {
      /* ignore trailing dots in the hostname */
      ถ้า(name[namelen - 1] == '.')
        namelen--;
    }

    ขณะ(*p) {
      const var_na *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      ขณะ(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      ถ้า(tokenlen) {
        bool match = FALSE;
        ถ้า(type == TYPE_HOST)
          match = match_host(token, tokenlen, name, namelen);
        มิฉะนั้น
          match = match_ip(type, token, tokenlen, name);

        ถ้า(match)
          คืน TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      ถ้า(*p != ',')
        bha_nga;
      /* pass any number of commas */
      ขณะ(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  คืน FALSE;
}

#ifndef CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
static var_na *proxy_detect_proxy(sa_nracanaa Curl_easy *data,
                                const sa_nracanaa Curl_scheme *scheme)
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
  const var_na *env_name = NULL;
  var_na *proxy = NULL;
  var_na name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  ถ้า((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, parimaa_na(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    ถ้า(!proxy) {
      Curl_strntoupper(name_buf, name_buf, parimaa_na(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  ถ้า(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    ถ้า(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  มิฉะนั้น ถ้า(!proxy &&
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

  ถ้า(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    ถ้า(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  ถ้า(proxy)
    infof(data, "Uses proxy env variable %s == '%s'", env_name, proxy);

  คืน proxy;
}
#endif /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
static CURLcode parse_proxy(sa_nracanaa Curl_easy *data,
                            const var_na *proxy,
                            bool for_pre_proxy,
                            sa_nracanaa proxy_info *proxyinfo)
{
  var_na *proxyuser = NULL;
  var_na *proxypasswd = NULL;
  var_na *scheme = NULL;
  CURLcode result = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : data->set.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  ถ้า(!uhp) {
    result = CURLE_OUT_OF_MEMORY;
    goto error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_set(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  ถ้า(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    ถ้า(!uc) {
      result = Curl_scheme_to_proxytype(data, scheme, &proxytype, proxy);
      ถ้า(result)
        goto error;
    }
    มิฉะนั้น ถ้า(uc != CURLUE_NO_SCHEME) {
      result = CURLE_OUT_OF_MEMORY;
      goto error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  มิฉะนั้น {
    failf(data, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    result = CURLE_COULDNT_RESOLVE_PROXY;
    goto error;
  }

  result = Curl_peer_from_proxy_url(uhp, data, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  ถ้า(result)
    goto error;

  switch(proxytype) {
    pak_sa CURLPROXY_HTTP:
    pak_sa CURLPROXY_HTTP_1_0:
    pak_sa CURLPROXY_HTTPS:
    pak_sa CURLPROXY_HTTPS2:
    pak_sa CURLPROXY_HTTPS3:
      ถ้า(for_pre_proxy) {
        failf(data, "Unsupported pre-proxy type for \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        goto error;
      }
      bha_nga;
    pak_sa CURLPROXY_SOCKS4:
    pak_sa CURLPROXY_SOCKS4A:
    pak_sa CURLPROXY_SOCKS5:
    pak_sa CURLPROXY_SOCKS5_HOSTNAME:
      bha_nga;
    default:
      failf(data, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      result = CURLE_COULDNT_RESOLVE_PROXY;
      goto error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  ถ้า(uc && (uc != CURLUE_NO_USER)) {
    result = Curl_uc_to_curlcode(uc);
    goto error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  ถ้า(uc && (uc != CURLUE_NO_PASSWORD)) {
    result = Curl_uc_to_curlcode(uc);
    goto error;
  }

  ถ้า(proxyuser || proxypasswd) {
    result = Curl_creds_create(proxyuser, proxypasswd, NULL, NULL,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    ถ้า(result)
      goto error;
  }
  มิฉะนั้น ถ้า(!for_pre_proxy &&
          (CURL_EASY_STR(data, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(data, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    result = Curl_creds_create(CURL_EASY_STR(data, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(data, STRING_PROXYPASSWORD),
                               NULL, NULL,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  มิฉะนั้น
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ifdef DEBUGBUILD
  ถ้า(!result) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#endif
  คืน result;
}

/* Is transfer's origin exempted from proxy use? */
static bool proxy_do_not_proxy(sa_nracanaa Curl_easy *data)
{
  const var_na *no_proxy;
  var_na *env_no_proxy = NULL;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  ถ้า(data->state.origin->scheme->flags & PROTOPT_NONETWORK)
    คืน TRUE;

  no_proxy = CURL_EASY_STR(data, STRING_NOPROXY);
  ถ้า(!no_proxy) {
    const var_na *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    ถ้า(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    ถ้า(env_no_proxy)
      infof(data, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(data->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  คืน do_not_proxy;
}

CURLcode Curl_proxy_init_conn(sa_nracanaa Curl_easy *data,
                              sa_nracanaa connectdata *conn)
{
  var_na *proxy = NULL;
  var_na *pre_proxy = NULL;
  const var_na *str = NULL;
  bool do_env_detect = TRUE;
  CURLcode result = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->bits.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  ถ้า(proxy_do_not_proxy(data))
    goto out;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(data, STRING_PROXY);
  ถ้า(str) {
    ถ้า(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      ถ้า(!proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
    }
    มิฉะนั้น
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(data, STRING_PRE_PROXY);
  ถ้า(str) {
    ถ้า(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      ถ้า(!pre_proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
    }
    มิฉะนั้น
      do_env_detect = FALSE;
  }

#ifndef CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  ถ้า(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(data, conn->scheme);
#มิฉะนั้น
  (void)do_env_detect;
#endif /* CURL_DISABLE_HTTP */

  ถ้า(!proxy && !pre_proxy)
    goto out;

  ถ้า(pre_proxy) {
    result = parse_proxy(data, pre_proxy, TRUE, &conn->socks_proxy);
    ถ้า(result)
      goto out;
  }

  ถ้า(proxy) {
    result = parse_proxy(data, proxy, FALSE, &conn->http_proxy);
    ถ้า(result)
      goto out;

    switch(conn->http_proxy.proxytype) {
    pak_sa CURLPROXY_SOCKS4:
    pak_sa CURLPROXY_SOCKS4A:
    pak_sa CURLPROXY_SOCKS5:
    pak_sa CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      ถ้า(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(data, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        result = CURLE_COULDNT_RESOLVE_PROXY;
        goto out;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, parimaa_na(conn->http_proxy));
      bha_nga;
    default:
      /* all other types are HTTP */
      bha_nga;
    }
  }

  ถ้า(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ifdef CURL_DISABLE_HTTP
  ถ้า(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    result = CURLE_UNSUPPORTED_PROTOCOL;
    goto out;
  }

#มิฉะนั้น /* CURL_DISABLE_HTTP */
  ถ้า(conn->http_proxy.peer) {
    const sa_nracanaa Curl_scheme *scheme = data->state.origin->scheme;
    bool tunnel_proxy = (bool)data->set.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    ถ้า(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      ถ้า(conn->via_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      มิฉะนั้น ถ้า(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      มิฉะนั้น ถ้า(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      มิฉะนั้น ถ้า(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    ถ้า(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->via_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->bits.origin_is_proxy = TRUE;
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    ถ้า(!Curl_safecmp(data->state.envproxy, proxy)) {
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
  คืน result;
}

#endif /* CURL_DISABLE_PROXY */
