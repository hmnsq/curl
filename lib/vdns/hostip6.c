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
#അന്തര്ഭാവ "curl_setup.h"

/***********************************************************************
 * Only for IPv6-enabled builds
 **********************************************************************/
#ifdef CURLRES_IPV6

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "vdns/hostip.h"
#അന്തര്ഭാവ "curlx/inet_pton.h"
#അന്തര്ഭാവ "connect.h"

#ifdef CURLRES_SYNCH

/*
 * Curl_sync_getaddrinfo() when built IPv6-enabled (non-threading and
 * non-ares version).
 *
 * Returns name information about the given hostname and port number. If
 * successful, the 'addrinfo' is returned and the fourth argument will point
 * to memory we need to free after use. That memory *MUST* be freed with
 * Curl_freeaddrinfo(), nothing else.
 */
ഘടന Curl_addrinfo *Curl_sync_getaddrinfo(ഘടന Curl_easy *data,
                                            uint8_t dns_queries,
                                            സ്ഥിരം അക്ഷരം *hostname,
                                            uint16_t port,
                                            uint8_t transport)
{
  ഘടന addrinfo hints;
  ഘടന Curl_addrinfo *res;
  പൂർണ്ണസംഖ്യ error;
  അക്ഷരം sbuf[12];
  അക്ഷരം *sbufptr = ശൂന്യ;
#ifndef USE_RESOLVE_ON_IPS
  അക്ഷരം addrbuf[128];
#endif
  പൂർണ്ണസംഖ്യ pf = PF_INET;

  എങ്കിൽ(dns_queries & CURL_DNSQ_AAAA)
    pf = PF_UNSPEC;

  memset(&hints, 0, വലുപ്പം(hints));
  hints.ai_family = pf;
  hints.ai_socktype = (transport == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifndef USE_RESOLVE_ON_IPS
  /*
   * The AI_NUMERICHOST must not be set to get synthesized IPv6 address from
   * an IPv4 address on iOS and macOS.
   */
  എങ്കിൽ((curlx_inet_pton(AF_INET, hostname, addrbuf) == 1) ||
     (curlx_inet_pton(AF_INET6, hostname, addrbuf) == 1)) {
    /* the given address is numerical only, prevent a reverse lookup */
    hints.ai_flags = AI_NUMERICHOST;
  }
#endif

  എങ്കിൽ(port) {
    curl_msnprintf(sbuf, വലുപ്പം(sbuf), "%d", port);
    sbufptr = sbuf;
  }

  error = Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &res);
  എങ്കിൽ(error) {
    infof(data, "getaddrinfo(3) failed for %s:%d", hostname, port);
    തിരികെ ശൂന്യ;
  }

  എങ്കിൽ(port) {
    Curl_addrinfo_set_port(res, port);
  }

  തിരികെ res;
}
#endif /* CURLRES_SYNCH */

#endif /* CURLRES_IPV6 */
