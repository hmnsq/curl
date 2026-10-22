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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_ਮੇਲ(ਅਚਲ ਅੱਖਰ *ipv4,    /* 1.2.3.4 address */
                          ਅਚਲ ਅੱਖਰ *ਜਾਲ, /* 1.2.3.4 address */
                          ਪੂਰਨ ਬਿੱਟ);
UNITTEST bool cidr4_ਮੇਲ(ਅਚਲ ਅੱਖਰ *ipv4,    /* 1.2.3.4 address */
                          ਅਚਲ ਅੱਖਰ *ਜਾਲ, /* 1.2.3.4 address */
                          ਪੂਰਨ ਬਿੱਟ)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਪਤਾ = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ check = 0;

  ਜੇ(ਬਿੱਟ > 32)
    /* strange input */
    ਵਾਪਸ FALSE;

  ਜੇ(curlx_inet_pton(AF_INET, ipv4, &ਪਤਾ) != 1)
    ਵਾਪਸ FALSE;
  ਜੇ(curlx_inet_pton(AF_INET, ਜਾਲ, &check) != 1)
    ਵਾਪਸ FALSE;

  ਜੇ((ਬਿੱਟ > 0) && (ਬਿੱਟ != 32)) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਨਕਾਬ = 0xffffffff << (32 - ਬਿੱਟ);
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ haddr = htonl(ਪਤਾ);
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hcheck = htonl(check);
#ਜੇ 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, ਜਾਲ, hcheck, ਬਿੱਟ, ਨਕਾਬ,
                  (haddr ^ hcheck) & ਨਕਾਬ);
#ਜੇ_ਅੰਤ
    ਜੇ((haddr ^ hcheck) & ਨਕਾਬ)
      ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਬਿੱਟ && !check)
    /* 0.0.0.0/0 matches everything */
    ਵਾਪਸ TRUE;

  ਵਾਪਸ ਪਤਾ == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_ਮੇਲ(ਅਚਲ ਅੱਖਰ *ipv6, ਅਚਲ ਅੱਖਰ *ਜਾਲ,
                          ਪੂਰਨ ਬਿੱਟ);
UNITTEST bool cidr6_ਮੇਲ(ਅਚਲ ਅੱਖਰ *ipv6, ਅਚਲ ਅੱਖਰ *ਜਾਲ,
                          ਪੂਰਨ ਬਿੱਟ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ null6[16]= { 0 };
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਬਾਈਟਾਂ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ rest;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਪਤਾ[16];
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ check[16];

  ਜੇ(ਬਿੱਟ == -1)
    ਬਿੱਟ = 128;

  ਬਾਈਟਾਂ = ਬਿੱਟ / 8;
  rest = ਬਿੱਟ & 0x07;
  ਜੇ((ਬਾਈਟਾਂ > 16) || ((ਬਾਈਟਾਂ == 16) && rest))
    ਵਾਪਸ FALSE;
  ਜੇ(curlx_inet_pton(AF_INET6, ipv6, ਪਤਾ) != 1)
    ਵਾਪਸ FALSE;
  ਜੇ(curlx_inet_pton(AF_INET6, ਜਾਲ, check) != 1)
    ਵਾਪਸ FALSE;
  ਜੇ((ਬਾਈਟਾਂ == 16) && !ਬਿੱਟ && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    ਵਾਪਸ TRUE;
  ਜੇ(ਬਾਈਟਾਂ && memcmp(ਪਤਾ, check, ਬਾਈਟਾਂ))
    ਵਾਪਸ FALSE;
  ਜੇ(rest && ((ਪਤਾ[ਬਾਈਟਾਂ] ^ check[ਬਾਈਟਾਂ]) & (0xff << (8 - rest))))
    ਵਾਪਸ FALSE;

  ਵਾਪਸ TRUE;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ipv6;
  (ਖਾਲੀ)ਜਾਲ;
  (ਖਾਲੀ)ਬਿੱਟ;
  ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
}

ਗਿਣਤੀ nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

ਸਥਿਰ bool ਮੇਲ_host(ਅਚਲ ਅੱਖਰ *token, size_t tokenlen,
                       ਅਚਲ ਅੱਖਰ *name, size_t namelen)
{
  bool ਮੇਲ = FALSE;

  /* ignore trailing dots in the token to check */
  ਜੇ(token[tokenlen - 1] == '.')
    tokenlen--;

  ਜੇ(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  ਜੇ(tokenlen == namelen)
    /* case A, exact match */
    ਮੇਲ = curl_strnequal(token, name, namelen);
  ਨਹੀਂਤਾਂ ਜੇ(tokenlen < namelen) {
    /* case B, tailmatch domain */
    ਮੇਲ = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  ਵਾਪਸ ਮੇਲ;
}

ਸਥਿਰ bool ਮੇਲ_ip(ਪੂਰਨ type, ਅਚਲ ਅੱਖਰ *token, size_t tokenlen,
                     ਅਚਲ ਅੱਖਰ *name)
{
  ਅੱਖਰ *slash;
  ਪੂਰਨ ਬਿੱਟ = -1; /* negative means not provided */
  ਅੱਖਰ checkip[128];
  ਜੇ(tokenlen >= ਆਕਾਰ(checkip))
    /* this cannot match */
    ਵਾਪਸ FALSE;
  /* copy the check name to a temp buffer */
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  ਜੇ(slash) {
    curl_off_t ਮੁੱਲ;
    ਅਚਲ ਅੱਖਰ *p = &slash[1];
    ਜੇ(curlx_str_ਸੰਖਿਆ(&p, &ਮੁੱਲ, 128) || *p)
      ਵਾਪਸ FALSE;
    /* a too large value is rejected in the cidr function below */
    ਬਿੱਟ = (ਪੂਰਨ)ਮੁੱਲ;
    *slash = 0; /* null-terminate there */
  }
  ਜੇ(type == TYPE_IPV6)
    ਵਾਪਸ cidr6_ਮੇਲ(name, checkip, ਬਿੱਟ);
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ cidr4_ਮੇਲ(name, checkip, ਬਿੱਟ);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *no_proxy);
UNITTEST bool proxy_check_noproxy(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  ਜੇ(!name || name[0] == '\0')
    ਵਾਪਸ FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  ਜੇ(no_proxy && no_proxy[0]) {
    ਅਚਲ ਅੱਖਰ *p = no_proxy;
    size_t namelen;
    ਅੱਖਰ ਪਤਾ[16];
    ਗਿਣਤੀ nametype type = TYPE_HOST;
    ਜੇ(!strcmp("*", no_proxy))
      ਵਾਪਸ TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = ਲੜੀ_ਲੰਬਾਈ(name);
    ਜੇ(curlx_inet_pton(AF_INET, name, &ਪਤਾ) == 1)
      type = TYPE_IPV4;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਨਹੀਂਤਾਂ ਜੇ(curlx_inet_pton(AF_INET6, name, &ਪਤਾ) == 1)
      type = TYPE_IPV6;
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ {
      /* ignore trailing dots in the hostname */
      ਜੇ(name[namelen - 1] == '.')
        namelen--;
    }

    ਜਦੋਂ(*p) {
      ਅਚਲ ਅੱਖਰ *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      ਜਦੋਂ(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      ਜੇ(tokenlen) {
        bool ਮੇਲ = FALSE;
        ਜੇ(type == TYPE_HOST)
          ਮੇਲ = ਮੇਲ_host(token, tokenlen, name, namelen);
        ਨਹੀਂਤਾਂ
          ਮੇਲ = ਮੇਲ_ip(type, token, tokenlen, name);

        ਜੇ(ਮੇਲ)
          ਵਾਪਸ TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      ਜੇ(*p != ',')
        ਰੋਕੋ;
      /* pass any number of commas */
      ਜਦੋਂ(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  ਵਾਪਸ FALSE;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
ਸਥਿਰ ਅੱਖਰ *proxy_detect_proxy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਅਚਲ ਬਣਤਰ Curl_scheme *scheme)
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
  ਅਚਲ ਅੱਖਰ *env_name = ਨਿਰਾ;
  ਅੱਖਰ *proxy = ਨਿਰਾ;
  ਅੱਖਰ name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  ਜੇ((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, ਆਕਾਰ(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    ਜੇ(!proxy) {
      Curl_strntoupper(name_buf, name_buf, ਆਕਾਰ(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  ਜੇ(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    ਜੇ(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(!proxy &&
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

  ਜੇ(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    ਜੇ(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  ਜੇ(proxy)
    infof(ਅੰਕੜੇ, "Uses proxy env variable %s == '%s'", env_name, proxy);

  ਵਾਪਸ proxy;
}
#ਜੇ_ਅੰਤ /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
ਸਥਿਰ CURLcode parse_proxy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅਚਲ ਅੱਖਰ *proxy,
                            bool for_pre_proxy,
                            ਬਣਤਰ proxy_info *proxyinfo)
{
  ਅੱਖਰ *proxyuser = ਨਿਰਾ;
  ਅੱਖਰ *proxypasswd = ਨਿਰਾ;
  ਅੱਖਰ *scheme = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : ਅੰਕੜੇ->ਸੈੱਟ.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  ਜੇ(!uhp) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_ਸੈੱਟ(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  ਜੇ(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    ਜੇ(!uc) {
      ਨਤੀਜਾ = Curl_scheme_ਮੰਜ਼ਿਲ_proxytype(ਅੰਕੜੇ, scheme, &proxytype, proxy);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ error;
    }
    ਨਹੀਂਤਾਂ ਜੇ(uc != CURLUE_NO_SCHEME) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  ਨਹੀਂਤਾਂ {
    failf(ਅੰਕੜੇ, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    ਨਤੀਜਾ = CURLE_COULDNT_RESOLVE_PROXY;
    ਜਾਓ error;
  }

  ਨਤੀਜਾ = Curl_peer_from_proxy_url(uhp, ਅੰਕੜੇ, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ error;

  ਚੋਣ(proxytype) {
    ਹਾਲਤ CURLPROXY_HTTP:
    ਹਾਲਤ CURLPROXY_HTTP_1_0:
    ਹਾਲਤ CURLPROXY_HTTPS:
    ਹਾਲਤ CURLPROXY_HTTPS2:
    ਹਾਲਤ CURLPROXY_HTTPS3:
      ਜੇ(for_pre_proxy) {
        failf(ਅੰਕੜੇ, "Unsupported pre-proxy type for \'%s\'", proxy);
        ਨਤੀਜਾ = CURLE_COULDNT_RESOLVE_PROXY;
        ਜਾਓ error;
      }
      ਰੋਕੋ;
    ਹਾਲਤ CURLPROXY_SOCKS4:
    ਹਾਲਤ CURLPROXY_SOCKS4A:
    ਹਾਲਤ CURLPROXY_SOCKS5:
    ਹਾਲਤ CURLPROXY_SOCKS5_HOSTNAME:
      ਰੋਕੋ;
    ਮੂਲ:
      failf(ਅੰਕੜੇ, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      ਨਤੀਜਾ = CURLE_COULDNT_RESOLVE_PROXY;
      ਜਾਓ error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  ਜੇ(uc && (uc != CURLUE_NO_USER)) {
    ਨਤੀਜਾ = Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
    ਜਾਓ error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  ਜੇ(uc && (uc != CURLUE_NO_PASSWORD)) {
    ਨਤੀਜਾ = Curl_uc_ਮੰਜ਼ਿਲ_curlcode(uc);
    ਜਾਓ error;
  }

  ਜੇ(proxyuser || proxypasswd) {
    ਨਤੀਜਾ = Curl_creds_create(proxyuser, proxypasswd, ਨਿਰਾ, ਨਿਰਾ,
                               CURL_EASY_STR(ਅੰਕੜੇ, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ error;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!for_pre_proxy &&
          (CURL_EASY_STR(ਅੰਕੜੇ, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(ਅੰਕੜੇ, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(ਅੰਕੜੇ, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    ਨਤੀਜਾ = Curl_creds_create(CURL_EASY_STR(ਅੰਕੜੇ, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(ਅੰਕੜੇ, STRING_PROXYPASSWORD),
                               ਨਿਰਾ, ਨਿਰਾ,
                               CURL_EASY_STR(ਅੰਕੜੇ, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  ਨਹੀਂਤਾਂ
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(!ਨਤੀਜਾ) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Is transfer's origin exempted from proxy use? */
ਸਥਿਰ bool proxy_do_not_proxy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਅੱਖਰ *no_proxy;
  ਅੱਖਰ *env_no_proxy = ਨਿਰਾ;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  ਜੇ(ਅੰਕੜੇ->state.origin->scheme->flags & PROTOPT_NONETWORK)
    ਵਾਪਸ TRUE;

  no_proxy = CURL_EASY_STR(ਅੰਕੜੇ, STRING_NOPROXY);
  ਜੇ(!no_proxy) {
    ਅਚਲ ਅੱਖਰ *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    ਜੇ(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    ਜੇ(env_no_proxy)
      infof(ਅੰਕੜੇ, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(ਅੰਕੜੇ->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  ਵਾਪਸ do_not_proxy;
}

CURLcode Curl_proxy_init_conn(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ connectdata *conn)
{
  ਅੱਖਰ *proxy = ਨਿਰਾ;
  ਅੱਖਰ *pre_proxy = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *str = ਨਿਰਾ;
  bool do_env_detect = TRUE;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->ਬਿੱਟ.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  ਜੇ(proxy_do_not_proxy(ਅੰਕੜੇ))
    ਜਾਓ ਬਾਹਰਲਾ;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(ਅੰਕੜੇ, STRING_PROXY);
  ਜੇ(str) {
    ਜੇ(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      ਜੇ(!proxy) {
        failf(ਅੰਕੜੇ, "memory shortage");
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    ਨਹੀਂਤਾਂ
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(ਅੰਕੜੇ, STRING_PRE_PROXY);
  ਜੇ(str) {
    ਜੇ(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      ਜੇ(!pre_proxy) {
        failf(ਅੰਕੜੇ, "memory shortage");
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    ਨਹੀਂਤਾਂ
      do_env_detect = FALSE;
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  ਜੇ(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(ਅੰਕੜੇ, conn->scheme);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)do_env_detect;
#ਜੇ_ਅੰਤ /* CURL_DISABLE_HTTP */

  ਜੇ(!proxy && !pre_proxy)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(pre_proxy) {
    ਨਤੀਜਾ = parse_proxy(ਅੰਕੜੇ, pre_proxy, TRUE, &conn->socks_proxy);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(proxy) {
    ਨਤੀਜਾ = parse_proxy(ਅੰਕੜੇ, proxy, FALSE, &conn->http_proxy);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    ਚੋਣ(conn->http_proxy.proxytype) {
    ਹਾਲਤ CURLPROXY_SOCKS4:
    ਹਾਲਤ CURLPROXY_SOCKS4A:
    ਹਾਲਤ CURLPROXY_SOCKS5:
    ਹਾਲਤ CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      ਜੇ(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(ਅੰਕੜੇ, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        ਨਤੀਜਾ = CURLE_COULDNT_RESOLVE_PROXY;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, ਆਕਾਰ(conn->http_proxy));
      ਰੋਕੋ;
    ਮੂਲ:
      /* all other types are HTTP */
      ਰੋਕੋ;
    }
  }

  ਜੇ(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  ਜੇ(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    ਨਤੀਜਾ = CURLE_UNSUPPORTED_PROTOCOL;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

#ਨਹੀਂਤਾਂ /* CURL_DISABLE_HTTP */
  ਜੇ(conn->http_proxy.peer) {
    ਅਚਲ ਬਣਤਰ Curl_scheme *scheme = ਅੰਕੜੇ->state.origin->scheme;
    bool tunnel_proxy = (bool)ਅੰਕੜੇ->ਸੈੱਟ.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    ਜੇ(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      ਜੇ(conn->ਰਾਹੀਂ_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      ਨਹੀਂਤਾਂ ਜੇ(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      ਨਹੀਂਤਾਂ ਜੇ(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      ਨਹੀਂਤਾਂ ਜੇ(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    ਜੇ(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->ਰਾਹੀਂ_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->ਬਿੱਟ.origin_is_proxy = TRUE;
    }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH
    ਜੇ(!Curl_safecmp(ਅੰਕੜੇ->state.envproxy, proxy)) {
      /* proxy changed */
      Curl_auth_digest_cleanup(&ਅੰਕੜੇ->state.proxydigest);
      curlx_free(ਅੰਕੜੇ->state.envproxy);
      ਅੰਕੜੇ->state.envproxy = curlx_strdup(proxy);
    }
#ਜੇ_ਅੰਤ
  }
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP */

ਬਾਹਰਲਾ:
  curlx_free(pre_proxy);
  curlx_free(proxy);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* CURL_DISABLE_PROXY */
