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

/***********************************************************************
 * Only for IPv6-enabled builds
 **********************************************************************/
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_IPV6

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_SYNCH

/*
 * Curl_sync_getaddrinfo() when built IPv6-enabled (non-threading and
 * non-ares version).
 *
 * Returns name information about the given hostname and port number. If
 * successful, the 'addrinfo' is returned and the fourth argument will point
 * to memory we need to free after use. That memory *MUST* be freed with
 * Curl_freeaddrinfo(), nothing else.
 */
ਬਣਤਰ Curl_addrinfo *Curl_sync_getaddrinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            uint8_t dns_queries,
                                            ਅਚਲ ਅੱਖਰ *hostname,
                                            uint16_t ਪੋਰਟ,
                                            uint8_t transport)
{
  ਬਣਤਰ addrinfo hints;
  ਬਣਤਰ Curl_addrinfo *res;
  ਪੂਰਨ error;
  ਅੱਖਰ sbuf[12];
  ਅੱਖਰ *sbufptr = ਨਿਰਾ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_RESOLVE_ON_IPS
  ਅੱਖਰ addrbuf[128];
#ਜੇ_ਅੰਤ
  ਪੂਰਨ pf = PF_INET;

  ਜੇ(dns_queries & CURL_DNSQ_AAAA)
    pf = PF_UNSPEC;

  memset(&hints, 0, ਆਕਾਰ(hints));
  hints.ai_family = pf;
  hints.ai_socktype = (transport == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_RESOLVE_ON_IPS
  /*
   * The AI_NUMERICHOST must not be set to get synthesized IPv6 address from
   * an IPv4 address on iOS and macOS.
   */
  ਜੇ((curlx_inet_pton(AF_INET, hostname, addrbuf) == 1) ||
     (curlx_inet_pton(AF_INET6, hostname, addrbuf) == 1)) {
    /* the given address is numerical only, prevent a reverse lookup */
    hints.ai_flags = AI_NUMERICHOST;
  }
#ਜੇ_ਅੰਤ

  ਜੇ(ਪੋਰਟ) {
    curl_msnprintf(sbuf, ਆਕਾਰ(sbuf), "%d", ਪੋਰਟ);
    sbufptr = sbuf;
  }

  error = Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &res);
  ਜੇ(error) {
    infof(ਅੰਕੜੇ, "getaddrinfo(3) failed for %s:%d", hostname, ਪੋਰਟ);
    ਵਾਪਸ ਨਿਰਾ;
  }

  ਜੇ(ਪੋਰਟ) {
    Curl_addrinfo_ਸੈੱਟ_ਪੋਰਟ(res, ਪੋਰਟ);
  }

  ਵਾਪਸ res;
}
#ਜੇ_ਅੰਤ /* CURLRES_SYNCH */

#ਜੇ_ਅੰਤ /* CURLRES_IPV6 */
