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
                          puur_naa_nka bits);
UNITTEST bool cidr4_match(const var_na *ipv4,    /* 1.2.3.4 address */
                          const var_na *network, /* 1.2.3.4 address */
                          puur_naa_nka bits)
{
  unsigned puur_naa_nka address = 0;
  unsigned puur_naa_nka check = 0;

  ᬬᬾᬦ᭄(bits > 32)
    /* strange input */
    ᬯᬮᬶ FALSE;

  ᬬᬾᬦ᭄(curlx_inet_pton(AF_INET, ipv4, &address) != 1)
    ᬯᬮᬶ FALSE;
  ᬬᬾᬦ᭄(curlx_inet_pton(AF_INET, network, &check) != 1)
    ᬯᬮᬶ FALSE;

  ᬬᬾᬦ᭄((bits > 0) && (bits != 32)) {
    unsigned puur_naa_nka mask = 0xffffffff << (32 - bits);
    unsigned puur_naa_nka haddr = htonl(address);
    unsigned puur_naa_nka hcheck = htonl(check);
#ᬬᬾᬦ᭄ 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, network, hcheck, bits, mask,
                  (haddr ^ hcheck) & mask);
#endif
    ᬬᬾᬦ᭄((haddr ^ hcheck) & mask)
      ᬯᬮᬶ FALSE;
    ᬯᬮᬶ TRUE;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(!bits && !check)
    /* 0.0.0.0/0 matches everything */
    ᬯᬮᬶ TRUE;

  ᬯᬮᬶ address == check;
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
                          puur_naa_nka bits);
UNITTEST bool cidr6_match(const var_na *ipv6, const var_na *network,
                          puur_naa_nka bits)
{
#ifdef USE_IPV6
  static const var_na null6[16]= { 0 };
  unsigned puur_naa_nka bytes;
  unsigned puur_naa_nka rest;
  unsigned var_na address[16];
  unsigned var_na check[16];

  ᬬᬾᬦ᭄(bits == -1)
    bits = 128;

  bytes = bits / 8;
  rest = bits & 0x07;
  ᬬᬾᬦ᭄((bytes > 16) || ((bytes == 16) && rest))
    ᬯᬮᬶ FALSE;
  ᬬᬾᬦ᭄(curlx_inet_pton(AF_INET6, ipv6, address) != 1)
    ᬯᬮᬶ FALSE;
  ᬬᬾᬦ᭄(curlx_inet_pton(AF_INET6, network, check) != 1)
    ᬯᬮᬶ FALSE;
  ᬬᬾᬦ᭄((bytes == 16) && !bits && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    ᬯᬮᬶ TRUE;
  ᬬᬾᬦ᭄(bytes && memcmp(address, check, bytes))
    ᬯᬮᬶ FALSE;
  ᬬᬾᬦ᭄(rest && ((address[bytes] ^ check[bytes]) & (0xff << (8 - rest))))
    ᬯᬮᬶ FALSE;

  ᬯᬮᬶ TRUE;
#ᬮᬶᬬ
  (void)ipv6;
  (void)network;
  (void)bits;
  ᬯᬮᬶ FALSE;
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
  ᬬᬾᬦ᭄(token[tokenlen - 1] == '.')
    tokenlen--;

  ᬬᬾᬦ᭄(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  ᬬᬾᬦ᭄(tokenlen == namelen)
    /* case A, exact match */
    match = curl_strnequal(token, name, namelen);
  ᬮᬶᬬ ᬬᬾᬦ᭄(tokenlen < namelen) {
    /* case B, tailmatch domain */
    match = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  ᬯᬮᬶ match;
}

static bool match_ip(puur_naa_nka type, const var_na *token, size_t tokenlen,
                     const var_na *name)
{
  var_na *slash;
  puur_naa_nka bits = -1; /* negative means not provided */
  var_na checkip[128];
  ᬬᬾᬦ᭄(tokenlen >= parimaa_na(checkip))
    /* this cannot match */
    ᬯᬮᬶ FALSE;
  /* copy the check name to a temp buffer */
  memcpy(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  ᬬᬾᬦ᭄(slash) {
    curl_off_t value;
    const var_na *p = &slash[1];
    ᬬᬾᬦ᭄(curlx_str_number(&p, &value, 128) || *p)
      ᬯᬮᬶ FALSE;
    /* a too large value is rejected in the cidr function below */
    bits = (puur_naa_nka)value;
    *slash = 0; /* null-terminate there */
  }
  ᬬᬾᬦ᭄(type == TYPE_IPV6)
    ᬯᬮᬶ cidr6_match(name, checkip, bits);
  ᬮᬶᬬ
    ᬯᬮᬶ cidr4_match(name, checkip, bits);
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
  ᬬᬾᬦ᭄(!name || name[0] == '\0')
    ᬯᬮᬶ FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  ᬬᬾᬦ᭄(no_proxy && no_proxy[0]) {
    const var_na *p = no_proxy;
    size_t namelen;
    var_na address[16];
    ga_nanaa nametype type = TYPE_HOST;
    ᬬᬾᬦ᭄(!strcmp("*", no_proxy))
      ᬯᬮᬶ TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = strlen(name);
    ᬬᬾᬦ᭄(curlx_inet_pton(AF_INET, name, &address) == 1)
      type = TYPE_IPV4;
#ifdef USE_IPV6
    ᬮᬶᬬ ᬬᬾᬦ᭄(curlx_inet_pton(AF_INET6, name, &address) == 1)
      type = TYPE_IPV6;
#endif
    ᬮᬶᬬ {
      /* ignore trailing dots in the hostname */
      ᬬᬾᬦ᭄(name[namelen - 1] == '.')
        namelen--;
    }

    ᬲᭂᬤᭂᬮ(*p) {
      const var_na *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      ᬲᭂᬤᭂᬮ(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      ᬬᬾᬦ᭄(tokenlen) {
        bool match = FALSE;
        ᬬᬾᬦ᭄(type == TYPE_HOST)
          match = match_host(token, tokenlen, name, namelen);
        ᬮᬶᬬ
          match = match_ip(type, token, tokenlen, name);

        ᬬᬾᬦ᭄(match)
          ᬯᬮᬶ TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      ᬬᬾᬦ᭄(*p != ',')
        bha_nga;
      /* pass any number of commas */
      ᬲᭂᬤᭂᬮ(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  ᬯᬮᬶ FALSE;
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
  ᬬᬾᬦ᭄((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, parimaa_na(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    ᬬᬾᬦ᭄(!proxy) {
      Curl_strntoupper(name_buf, name_buf, parimaa_na(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  ᬬᬾᬦ᭄(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    ᬬᬾᬦ᭄(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(!proxy &&
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

  ᬬᬾᬦ᭄(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    ᬬᬾᬦ᭄(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  ᬬᬾᬦ᭄(proxy)
    infof(data, "Uses proxy env variable %s == '%s'", env_name, proxy);

  ᬯᬮᬶ proxy;
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

  ᬬᬾᬦ᭄(!uhp) {
    result = CURLE_OUT_OF_MEMORY;
    goto error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_set(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  ᬬᬾᬦ᭄(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    ᬬᬾᬦ᭄(!uc) {
      result = Curl_scheme_to_proxytype(data, scheme, &proxytype, proxy);
      ᬬᬾᬦ᭄(result)
        goto error;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(uc != CURLUE_NO_SCHEME) {
      result = CURLE_OUT_OF_MEMORY;
      goto error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  ᬮᬶᬬ {
    failf(data, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    result = CURLE_COULDNT_RESOLVE_PROXY;
    goto error;
  }

  result = Curl_peer_from_proxy_url(uhp, data, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  ᬬᬾᬦ᭄(result)
    goto error;

  switch(proxytype) {
    pak_sa CURLPROXY_HTTP:
    pak_sa CURLPROXY_HTTP_1_0:
    pak_sa CURLPROXY_HTTPS:
    pak_sa CURLPROXY_HTTPS2:
    pak_sa CURLPROXY_HTTPS3:
      ᬬᬾᬦ᭄(for_pre_proxy) {
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
  ᬬᬾᬦ᭄(uc && (uc != CURLUE_NO_USER)) {
    result = Curl_uc_to_curlcode(uc);
    goto error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  ᬬᬾᬦ᭄(uc && (uc != CURLUE_NO_PASSWORD)) {
    result = Curl_uc_to_curlcode(uc);
    goto error;
  }

  ᬬᬾᬦ᭄(proxyuser || proxypasswd) {
    result = Curl_creds_create(proxyuser, proxypasswd, NULL, NULL,
                               CURL_EASY_STR(data, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    ᬬᬾᬦ᭄(result)
      goto error;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(!for_pre_proxy &&
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
  ᬮᬶᬬ
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ifdef DEBUGBUILD
  ᬬᬾᬦ᭄(!result) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#endif
  ᬯᬮᬶ result;
}

/* Is transfer's origin exempted from proxy use? */
static bool proxy_do_not_proxy(sa_nracanaa Curl_easy *data)
{
  const var_na *no_proxy;
  var_na *env_no_proxy = NULL;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  ᬬᬾᬦ᭄(data->state.origin->scheme->flags & PROTOPT_NONETWORK)
    ᬯᬮᬶ TRUE;

  no_proxy = CURL_EASY_STR(data, STRING_NOPROXY);
  ᬬᬾᬦ᭄(!no_proxy) {
    const var_na *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    ᬬᬾᬦ᭄(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    ᬬᬾᬦ᭄(env_no_proxy)
      infof(data, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(data->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  ᬯᬮᬶ do_not_proxy;
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

  ᬬᬾᬦ᭄(proxy_do_not_proxy(data))
    goto out;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(data, STRING_PROXY);
  ᬬᬾᬦ᭄(str) {
    ᬬᬾᬦ᭄(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      ᬬᬾᬦ᭄(!proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
    }
    ᬮᬶᬬ
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(data, STRING_PRE_PROXY);
  ᬬᬾᬦ᭄(str) {
    ᬬᬾᬦ᭄(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      ᬬᬾᬦ᭄(!pre_proxy) {
        failf(data, "memory shortage");
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
    }
    ᬮᬶᬬ
      do_env_detect = FALSE;
  }

#ifndef CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  ᬬᬾᬦ᭄(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(data, conn->scheme);
#ᬮᬶᬬ
  (void)do_env_detect;
#endif /* CURL_DISABLE_HTTP */

  ᬬᬾᬦ᭄(!proxy && !pre_proxy)
    goto out;

  ᬬᬾᬦ᭄(pre_proxy) {
    result = parse_proxy(data, pre_proxy, TRUE, &conn->socks_proxy);
    ᬬᬾᬦ᭄(result)
      goto out;
  }

  ᬬᬾᬦ᭄(proxy) {
    result = parse_proxy(data, proxy, FALSE, &conn->http_proxy);
    ᬬᬾᬦ᭄(result)
      goto out;

    switch(conn->http_proxy.proxytype) {
    pak_sa CURLPROXY_SOCKS4:
    pak_sa CURLPROXY_SOCKS4A:
    pak_sa CURLPROXY_SOCKS5:
    pak_sa CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      ᬬᬾᬦ᭄(pre_proxy) {
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

  ᬬᬾᬦ᭄(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ifdef CURL_DISABLE_HTTP
  ᬬᬾᬦ᭄(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    result = CURLE_UNSUPPORTED_PROTOCOL;
    goto out;
  }

#ᬮᬶᬬ /* CURL_DISABLE_HTTP */
  ᬬᬾᬦ᭄(conn->http_proxy.peer) {
    const sa_nracanaa Curl_scheme *scheme = data->state.origin->scheme;
    bool tunnel_proxy = (bool)data->set.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    ᬬᬾᬦ᭄(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      ᬬᬾᬦ᭄(conn->via_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      ᬮᬶᬬ ᬬᬾᬦ᭄(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      ᬮᬶᬬ ᬬᬾᬦ᭄(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      ᬮᬶᬬ ᬬᬾᬦ᭄(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    ᬬᬾᬦ᭄(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->via_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->bits.origin_is_proxy = TRUE;
    }

#ifndef CURL_DISABLE_DIGEST_AUTH
    ᬬᬾᬦ᭄(!Curl_safecmp(data->state.envproxy, proxy)) {
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
  ᬯᬮᬶ result;
}

#endif /* CURL_DISABLE_PROXY */
