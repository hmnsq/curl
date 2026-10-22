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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY

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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ

/*
 * cidr4_match() returns TRUE if the given IPv4 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr4_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ipv4,    /* 1.2.3.4 address */
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಜಾಲ, /* 1.2.3.4 address */
                          ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು);
UNITTEST bool cidr4_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ipv4,    /* 1.2.3.4 address */
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಜಾಲ, /* 1.2.3.4 address */
                          ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ವಿಳಾಸ = 0;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ check = 0;

  ಆದರೆ(ಬಿಟ್ಟುಗಳು > 32)
    /* strange input */
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ(curlx_inet_pton(AF_INET, ipv4, &ವಿಳಾಸ) != 1)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(curlx_inet_pton(AF_INET, ಜಾಲ, &check) != 1)
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ((ಬಿಟ್ಟುಗಳು > 0) && (ಬಿಟ್ಟುಗಳು != 32)) {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ = 0xffffffff << (32 - ಬಿಟ್ಟುಗಳು);
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ haddr = htonl(ವಿಳಾಸ);
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hcheck = htonl(check);
#ಆದರೆ 0
    curl_mfprintf(stderr, "Host %s (%x) network %s (%x) "
                  "bits %u mask %x => %x\n",
                  ipv4, haddr, ಜಾಲ, hcheck, ಬಿಟ್ಟುಗಳು, ಮುಖವಾಡ,
                  (haddr ^ hcheck) & ಮುಖವಾಡ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ((haddr ^ hcheck) & ಮುಖವಾಡ)
      ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಬಿಟ್ಟುಗಳು && !check)
    /* 0.0.0.0/0 matches everything */
    ಹಿಂತಿರುಗಿ TRUE;

  ಹಿಂತಿರುಗಿ ವಿಳಾಸ == check;
}

/*
 * cidr6_match() returns TRUE if the given IPv6 address is within the
 * specified CIDR address range.
 *
 * bits is passed as -1 if not provided.
 *
 * @unittest 1614
 */
UNITTEST bool cidr6_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ipv6, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಜಾಲ,
                          ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು);
UNITTEST bool cidr6_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ipv6, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಜಾಲ,
                          ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ null6[16]= { 0 };
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ rest;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ವಿಳಾಸ[16];
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ check[16];

  ಆದರೆ(ಬಿಟ್ಟುಗಳು == -1)
    ಬಿಟ್ಟುಗಳು = 128;

  ಬೈಟುಗಳು = ಬಿಟ್ಟುಗಳು / 8;
  rest = ಬಿಟ್ಟುಗಳು & 0x07;
  ಆದರೆ((ಬೈಟುಗಳು > 16) || ((ಬೈಟುಗಳು == 16) && rest))
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(curlx_inet_pton(AF_INET6, ipv6, ವಿಳಾಸ) != 1)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(curlx_inet_pton(AF_INET6, ಜಾಲ, check) != 1)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ((ಬೈಟುಗಳು == 16) && !ಬಿಟ್ಟುಗಳು && !memcmp(null6, check, 16))
    /* ::/0 matches everything */
    ಹಿಂತಿರುಗಿ TRUE;
  ಆದರೆ(ಬೈಟುಗಳು && memcmp(ವಿಳಾಸ, check, ಬೈಟುಗಳು))
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(rest && ((ವಿಳಾಸ[ಬೈಟುಗಳು] ^ check[ಬೈಟುಗಳು]) & (0xff << (8 - rest))))
    ಹಿಂತಿರುಗಿ FALSE;

  ಹಿಂತಿರುಗಿ TRUE;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ipv6;
  (ಶೂನ್ಯ)ಜಾಲ;
  (ಶೂನ್ಯ)ಬಿಟ್ಟುಗಳು;
  ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

ಎಣಿಕೆ nametype {
  TYPE_HOST,
  TYPE_IPV4,
  TYPE_IPV6
};

ಸ್ಥಿರ bool ಹೊಂದಿಕೆ_host(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *token, size_t tokenlen,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t namelen)
{
  bool ಹೊಂದಿಕೆ = FALSE;

  /* ignore trailing dots in the token to check */
  ಆದರೆ(token[tokenlen - 1] == '.')
    tokenlen--;

  ಆದರೆ(tokenlen && (*token == '.')) {
    /* ignore leading token dot as well */
    token++;
    tokenlen--;
  }
  /* A: example.com matches 'example.com'
     B: www.example.com matches 'example.com'
     C: nonexample.com DOES NOT match 'example.com'
   */
  ಆದರೆ(tokenlen == namelen)
    /* case A, exact match */
    ಹೊಂದಿಕೆ = curl_strnequal(token, name, namelen);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(tokenlen < namelen) {
    /* case B, tailmatch domain */
    ಹೊಂದಿಕೆ = (name[namelen - tokenlen - 1] == '.') &&
            curl_strnequal(token, name + (namelen - tokenlen), tokenlen);
  }
  /* case C passes through, not a match */
  ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ;
}

ಸ್ಥಿರ bool ಹೊಂದಿಕೆ_ip(ಪೂರ್ಣಾಂಕ type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *token, size_t tokenlen,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  ಅಕ್ಷರ *slash;
  ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು = -1; /* negative means not provided */
  ಅಕ್ಷರ checkip[128];
  ಆದರೆ(tokenlen >= ಗಾತ್ರ(checkip))
    /* this cannot match */
    ಹಿಂತಿರುಗಿ FALSE;
  /* copy the check name to a temp buffer */
  ಸ್ಮರಣೆ_ನಕಲು(checkip, token, tokenlen);
  checkip[tokenlen] = 0;

  slash = strchr(checkip, '/');
  /* if the slash is part of this token, use it */
  ಆದರೆ(slash) {
    curl_off_t ಮೌಲ್ಯ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = &slash[1];
    ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&p, &ಮೌಲ್ಯ, 128) || *p)
      ಹಿಂತಿರುಗಿ FALSE;
    /* a too large value is rejected in the cidr function below */
    ಬಿಟ್ಟುಗಳು = (ಪೂರ್ಣಾಂಕ)ಮೌಲ್ಯ;
    *slash = 0; /* null-terminate there */
  }
  ಆದರೆ(type == TYPE_IPV6)
    ಹಿಂತಿರುಗಿ cidr6_ಹೊಂದಿಕೆ(name, checkip, ಬಿಟ್ಟುಗಳು);
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ cidr4_ಹೊಂದಿಕೆ(name, checkip, ಬಿಟ್ಟುಗಳು);
}

/****************************************************************
 * Checks if the host is in the noproxy list. returns TRUE if it matches and
 * therefore the proxy should NOT be used.
 ****************************************************************/
/* @unittest 1614 */
UNITTEST bool proxy_check_noproxy(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *no_proxy);
UNITTEST bool proxy_check_noproxy(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *no_proxy)
{
  /*
   * If we do not have a hostname at all, like for example with a FILE
   * transfer, we have nothing to interrogate the noproxy list with.
   */
  ಆದರೆ(!name || name[0] == '\0')
    ಹಿಂತಿರುಗಿ FALSE;

  /* no_proxy=domain1.dom,host.domain2.dom
   *   (a comma-separated list of hosts which should
   *   not be proxied, or an asterisk to override
   *   all proxy variables)
   */
  ಆದರೆ(no_proxy && no_proxy[0]) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = no_proxy;
    size_t namelen;
    ಅಕ್ಷರ ವಿಳಾಸ[16];
    ಎಣಿಕೆ nametype type = TYPE_HOST;
    ಆದರೆ(!strcmp("*", no_proxy))
      ಹಿಂತಿರುಗಿ TRUE;

    /* NO_PROXY was specified and it was not only an asterisk */

    /* Check if name is an IP address; if not, assume it being a hostname. */
    namelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);
    ಆದರೆ(curlx_inet_pton(AF_INET, name, &ವಿಳಾಸ) == 1)
      type = TYPE_IPV4;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_inet_pton(AF_INET6, name, &ವಿಳಾಸ) == 1)
      type = TYPE_IPV6;
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ {
      /* ignore trailing dots in the hostname */
      ಆದರೆ(name[namelen - 1] == '.')
        namelen--;
    }

    ಆಗ(*p) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *token;
      size_t tokenlen = 0;

      /* pass blanks */
      curlx_str_passblanks(&p);

      token = p;
      /* pass over the pattern */
      ಆಗ(*p && !ISBLANK(*p) && (*p != ',')) {
        p++;
        tokenlen++;
      }

      ಆದರೆ(tokenlen) {
        bool ಹೊಂದಿಕೆ = FALSE;
        ಆದರೆ(type == TYPE_HOST)
          ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ_host(token, tokenlen, name, namelen);
        ಇಲ್ಲದಿದ್ದರೆ
          ಹೊಂದಿಕೆ = ಹೊಂದಿಕೆ_ip(type, token, tokenlen, name);

        ಆದರೆ(ಹೊಂದಿಕೆ)
          ಹಿಂತಿರುಗಿ TRUE;
      }

      /* pass blanks after pattern */
      curlx_str_passblanks(&p);
      /* if not a comma, this ends the loop */
      ಆದರೆ(*p != ',')
        ನಿಲ್ಲಿಸು;
      /* pass any number of commas */
      ಆಗ(*p == ',')
        p++;
    } /* while(*p) */
  } /* NO_PROXY was specified and it was not only an asterisk */

  ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP

/****************************************************************
 * Detect what (if any) proxy to use. Remember that this selects a host
 * name and is not limited to HTTP proxies only.
 * The returned pointer must be freed by the caller.
 ****************************************************************/
ಸ್ಥಿರ ಅಕ್ಷರ *proxy_detect_proxy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *scheme)
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
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *env_name = ಖಾಲಿ;
  ಅಕ್ಷರ *proxy = ಖಾಲಿ;
  ಅಕ್ಷರ name_buf[20];

  /* Try scheme specific env var first, unless http(s).
   * lowercase first, then uppercase. */
  ಆದರೆ((scheme != &Curl_scheme_https) && (scheme != &Curl_scheme_http)) {
    curl_msnprintf(name_buf, ಗಾತ್ರ(name_buf), "%s_proxy", scheme->name);
    env_name = name_buf;
    proxy = curl_getenv(env_name);
    ಆದರೆ(!proxy) {
      Curl_strntoupper(name_buf, name_buf, ಗಾತ್ರ(name_buf));
      proxy = curl_getenv(env_name);
    }
  }

  ಆದರೆ(!proxy &&
     ((scheme == &Curl_scheme_https) || (scheme == &Curl_scheme_wss))) {
    /* Not found, check 'https' env vars, also for 'wss'.
     * Again, first lowercase then uppercase. */
    env_name = "https_proxy";
    proxy = curl_getenv(env_name);
    ಆದರೆ(!proxy) {
      env_name = "HTTPS_PROXY";
      proxy = curl_getenv(env_name);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!proxy &&
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

  ಆದರೆ(!proxy) {
    /* still not found, last resort checks. */
    env_name = "all_proxy";
    proxy = curl_getenv(env_name);
    ಆದರೆ(!proxy) {
      env_name = "ALL_PROXY";
      proxy = curl_getenv(env_name);
    }
  }

  ಆದರೆ(proxy)
    infof(ದತ್ತಾಂಶ, "Uses proxy env variable %s == '%s'", env_name, proxy);

  ಹಿಂತಿರುಗಿ proxy;
}
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_HTTP */

/*
 * If this is supposed to use a proxy, we need to figure out the proxy
 * hostname, so that we can reuse an existing connection
 * that may exist registered to the same proxy host.
 */
ಸ್ಥಿರ CURLcode parse_proxy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *proxy,
                            bool for_pre_proxy,
                            ರಚನೆ proxy_info *proxyinfo)
{
  ಅಕ್ಷರ *proxyuser = ಖಾಲಿ;
  ಅಕ್ಷರ *proxypasswd = ಖಾಲಿ;
  ಅಕ್ಷರ *scheme = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  /* Set the start proxy type for URL scheme guessing */
  uint8_t proxytype = for_pre_proxy ? CURLPROXY_SOCKS4 : ದತ್ತಾಂಶ->ಗಣ.proxytype;
  CURLU *uhp = curl_url();
  CURLUcode uc;

  ಆದರೆ(!uhp) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು error;
  }
  /* When parsing the proxy, allowing non-supported schemes since we have
     these made up ones for proxies. Guess scheme for URLs without it. */
  uc = curl_url_ಗಣ(uhp, CURLUPART_URL, proxy,
                    CURLU_NON_SUPPORT_SCHEME | CURLU_GUESS_SCHEME);
  ಆದರೆ(!uc) {
    /* parsed okay as a URL - only update proxytype when scheme was explicit */
    uc = curl_url_get(uhp, CURLUPART_SCHEME, &scheme, CURLU_NO_GUESS_SCHEME);
    ಆದರೆ(!uc) {
      ಫಲಿತಾಂಶ = Curl_scheme_ಗಮ್ಯ_proxytype(ದತ್ತಾಂಶ, scheme, &proxytype, proxy);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು error;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(uc != CURLUE_NO_SCHEME) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು error;
    }
    /* else: no explicit scheme, keep the configured proxytype */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    failf(ದತ್ತಾಂಶ, "Unsupported proxy syntax in \'%s\': %s", proxy,
          curl_url_strerror(uc));
    ಫಲಿತಾಂಶ = CURLE_COULDNT_RESOLVE_PROXY;
    ಹೋಗು error;
  }

  ಫಲಿತಾಂಶ = Curl_peer_from_proxy_url(uhp, ದತ್ತಾಂಶ, proxy, proxytype,
                                    &proxyinfo->peer, &proxytype);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು error;

  ಆಯ್ಕೆ(proxytype) {
    ಸ್ಥಿತಿ CURLPROXY_HTTP:
    ಸ್ಥಿತಿ CURLPROXY_HTTP_1_0:
    ಸ್ಥಿತಿ CURLPROXY_HTTPS:
    ಸ್ಥಿತಿ CURLPROXY_HTTPS2:
    ಸ್ಥಿತಿ CURLPROXY_HTTPS3:
      ಆದರೆ(for_pre_proxy) {
        failf(ದತ್ತಾಂಶ, "Unsupported pre-proxy type for \'%s\'", proxy);
        ಫಲಿತಾಂಶ = CURLE_COULDNT_RESOLVE_PROXY;
        ಹೋಗು error;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLPROXY_SOCKS4:
    ಸ್ಥಿತಿ CURLPROXY_SOCKS4A:
    ಸ್ಥಿತಿ CURLPROXY_SOCKS5:
    ಸ್ಥಿತಿ CURLPROXY_SOCKS5_HOSTNAME:
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      failf(ದತ್ತಾಂಶ, "Unsupported proxy type %u for \'%s\'", proxytype, proxy);
      ಫಲಿತಾಂಶ = CURLE_COULDNT_RESOLVE_PROXY;
      ಹೋಗು error;
  }

  /* Is there a username and password given in this proxy URL? */
  uc = curl_url_get(uhp, CURLUPART_USER, &proxyuser, CURLU_URLDECODE);
  ಆದರೆ(uc && (uc != CURLUE_NO_USER)) {
    ಫಲಿತಾಂಶ = Curl_uc_ಗಮ್ಯ_curlcode(uc);
    ಹೋಗು error;
  }
  uc = curl_url_get(uhp, CURLUPART_PASSWORD, &proxypasswd, CURLU_URLDECODE);
  ಆದರೆ(uc && (uc != CURLUE_NO_PASSWORD)) {
    ಫಲಿತಾಂಶ = Curl_uc_ಗಮ್ಯ_curlcode(uc);
    ಹೋಗು error;
  }

  ಆದರೆ(proxyuser || proxypasswd) {
    ಫಲಿತಾಂಶ = Curl_creds_create(proxyuser, proxypasswd, ಖಾಲಿ, ಖಾಲಿ,
                               CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_PROXY_SERVICE_NAME),
                               CREDS_URL, &proxyinfo->creds);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು error;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!for_pre_proxy &&
          (CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_PROXYUSERNAME) ||
           CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_PROXYPASSWORD) ||
           CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_PROXY_SERVICE_NAME))) {
    /* No user/passwd in URL, if this is not a pre-proxy, the
     * CURLOPT_PROXY* settings apply. */
    ಫಲಿತಾಂಶ = Curl_creds_create(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_PROXYUSERNAME),
                               CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_PROXYPASSWORD),
                               ಖಾಲಿ, ಖಾಲಿ,
                               CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_PROXY_SERVICE_NAME),
                               CREDS_OPTION, &proxyinfo->creds);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    Curl_creds_unlink(&proxyinfo->creds);

  proxyinfo->proxytype = proxytype;

error:
  curlx_free(scheme);
  curlx_free(proxyuser);
  curlx_free(proxypasswd);
  curl_url_cleanup(uhp);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    DEBUGASSERT(proxyinfo);
    DEBUGASSERT(proxyinfo->peer);
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Is transfer's origin exempted from proxy use? */
ಸ್ಥಿರ bool proxy_do_not_proxy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *no_proxy;
  ಅಕ್ಷರ *env_no_proxy = ಖಾಲಿ;
  bool do_not_proxy;

  /* no proxying if the transfer does not use the network */
  ಆದರೆ(ದತ್ತಾಂಶ->state.origin->scheme->flags & PROTOPT_NONETWORK)
    ಹಿಂತಿರುಗಿ TRUE;

  no_proxy = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_NOPROXY);
  ಆದರೆ(!no_proxy) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = "no_proxy";
    env_no_proxy = curl_getenv(p);
    ಆದರೆ(!env_no_proxy) {
      p = "NO_PROXY";
      env_no_proxy = curl_getenv(p);
    }
    ಆದರೆ(env_no_proxy)
      infof(ದತ್ತಾಂಶ, "Uses proxy env variable %s == '%s'", p, env_no_proxy);
    no_proxy = env_no_proxy;
  }

  do_not_proxy = proxy_check_noproxy(ದತ್ತಾಂಶ->state.origin->hostname, no_proxy);
  curlx_safefree(env_no_proxy);
  ಹಿಂತಿರುಗಿ do_not_proxy;
}

CURLcode Curl_proxy_init_conn(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ connectdata *conn)
{
  ಅಕ್ಷರ *proxy = ಖಾಲಿ;
  ಅಕ್ಷರ *pre_proxy = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str = ಖಾಲಿ;
  bool do_env_detect = TRUE;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* Enforce no proxy use unless we decide to use one */
  conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy = FALSE;
  DEBUGASSERT(!conn->socks_proxy.peer);
  DEBUGASSERT(!conn->http_proxy.peer);

  ಆದರೆ(proxy_do_not_proxy(ದತ್ತಾಂಶ))
    ಹೋಗು ಹೊರಹೋಗು;

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  /* the empty config strings disable proxy use and env detects */
  str = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_PROXY);
  ಆದರೆ(str) {
    ಆದರೆ(*str) {
      proxy = curlx_strdup(str);
      /* if global proxy is set, this is it */
      ಆದರೆ(!proxy) {
        failf(ದತ್ತಾಂಶ, "memory shortage");
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      do_env_detect = FALSE;
  }

  str = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_PRE_PROXY);
  ಆದರೆ(str) {
    ಆದರೆ(*str) {
      pre_proxy = curlx_strdup(str);
      /* if global socks proxy is set, this is it */
      ಆದರೆ(!pre_proxy) {
        failf(ದತ್ತಾಂಶ, "memory shortage");
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      do_env_detect = FALSE;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  /* None configured, detect possible proxy from environment. */
  ಆದರೆ(!proxy && !pre_proxy && do_env_detect)
    proxy = proxy_detect_proxy(ದತ್ತಾಂಶ, conn->scheme);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)do_env_detect;
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_HTTP */

  ಆದರೆ(!proxy && !pre_proxy)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(pre_proxy) {
    ಫಲಿತಾಂಶ = parse_proxy(ದತ್ತಾಂಶ, pre_proxy, TRUE, &conn->socks_proxy);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(proxy) {
    ಫಲಿತಾಂಶ = parse_proxy(ದತ್ತಾಂಶ, proxy, FALSE, &conn->http_proxy);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    ಆಯ್ಕೆ(conn->http_proxy.proxytype) {
    ಸ್ಥಿತಿ CURLPROXY_SOCKS4:
    ಸ್ಥಿತಿ CURLPROXY_SOCKS4A:
    ಸ್ಥಿತಿ CURLPROXY_SOCKS5:
    ಸ್ಥಿತಿ CURLPROXY_SOCKS5_HOSTNAME:
      /* Whoops, it is not an HTTP proxy */
      ಆದರೆ(pre_proxy) {
        /* and we already have a SOCKS pre-proxy. Cannot have both */
        failf(ದತ್ತಾಂಶ, "Having a SOCKS pre-proxy and proxy is not "
              "supported with \'%s\'", proxy);
        ಫಲಿತಾಂಶ = CURLE_COULDNT_RESOLVE_PROXY;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      /* switch */
      conn->socks_proxy = conn->http_proxy;
      memset(&conn->http_proxy, 0, ಗಾತ್ರ(conn->http_proxy));
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      /* all other types are HTTP */
      ನಿಲ್ಲಿಸು;
    }
  }

  ಆದರೆ(conn->socks_proxy.peer) {
    DEBUGASSERT(!CURL_PROXY_IS_ANY_HTTP(conn->socks_proxy.proxytype));
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_HTTP
  ಆದರೆ(conn->http_proxy.peer) {
    /* asking for an HTTP proxy is a bit funny when HTTP is disabled... */
    ಫಲಿತಾಂಶ = CURLE_UNSUPPORTED_PROTOCOL;
    ಹೋಗು ಹೊರಹೋಗು;
  }

#ಇಲ್ಲದಿದ್ದರೆ /* CURL_DISABLE_HTTP */
  ಆದರೆ(conn->http_proxy.peer) {
    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *scheme = ದತ್ತಾಂಶ->state.origin->scheme;
    bool tunnel_proxy = (bool)ದತ್ತಾಂಶ->ಗಣ.tunnel_thru_httpproxy;
    DEBUGASSERT(CURL_PROXY_IS_ANY_HTTP(conn->http_proxy.proxytype));

    ಆದರೆ(!tunnel_proxy) {
      /* Decide if we tunnel through proxy automatically */
      ಆದರೆ(conn->ಮೂಲಕ_peer) {
        /* With connect-to, we always tunnel */
        tunnel_proxy = TRUE;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(scheme->flags & PROTOPT_SSL) {
        /* If the transfer is supposed to be secure, we tunnel */
        tunnel_proxy = TRUE;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(scheme->flags & PROTOPT_HTTP_PROXY_TUNNEL) {
        /* transfer scheme required tunneling */
        tunnel_proxy = TRUE;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!(scheme->protocol & PROTO_FAMILY_HTTP) &&
              !(scheme->flags & PROTOPT_PROXY_AS_HTTP)) {
        /* Cannot delegate transfer URL to HTTP proxy */
        tunnel_proxy = TRUE;
      }
    }

    ಆದರೆ(!tunnel_proxy) {
      /* HTTP proxy used in forwarding mode. This means the connection
       * is really to the proxy and NOT the origin of the transfer. */
      DEBUGASSERT(!conn->ಮೂಲಕ_peer);
      Curl_peer_link(&conn->origin, conn->http_proxy.peer);
      conn->scheme = conn->http_proxy.peer->scheme;
      conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy = TRUE;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH
    ಆದರೆ(!Curl_safecmp(ದತ್ತಾಂಶ->state.envproxy, proxy)) {
      /* proxy changed */
      Curl_auth_digest_cleanup(&ದತ್ತಾಂಶ->state.proxydigest);
      curlx_free(ದತ್ತಾಂಶ->state.envproxy);
      ದತ್ತಾಂಶ->state.envproxy = curlx_strdup(proxy);
    }
#ಆದರೆ_ಅಂತ್ಯ
  }
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP */

ಹೊರಹೋಗು:
  curlx_free(pre_proxy);
  curlx_free(proxy);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_PROXY */
