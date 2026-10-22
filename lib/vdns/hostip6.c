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

/***********************************************************************
 * Only for IPv6-enabled builds
 **********************************************************************/
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_IPV6

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "connect.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_SYNCH

/*
 * Curl_sync_getaddrinfo() when built IPv6-enabled (non-threading and
 * non-ares version).
 *
 * Returns name information about the given hostname and port number. If
 * successful, the 'addrinfo' is returned and the fourth argument will point
 * to memory we need to free after use. That memory *MUST* be freed with
 * Curl_freeaddrinfo(), nothing else.
 */
ರಚನೆ Curl_addrinfo *Curl_sync_getaddrinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            uint8_t dns_queries,
                                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                                            uint16_t ಪೋರ್ಟ್,
                                            uint8_t transport)
{
  ರಚನೆ addrinfo hints;
  ರಚನೆ Curl_addrinfo *res;
  ಪೂರ್ಣಾಂಕ error;
  ಅಕ್ಷರ sbuf[12];
  ಅಕ್ಷರ *sbufptr = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_RESOLVE_ON_IPS
  ಅಕ್ಷರ addrbuf[128];
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ pf = PF_INET;

  ಆದರೆ(dns_queries & CURL_DNSQ_AAAA)
    pf = PF_UNSPEC;

  memset(&hints, 0, ಗಾತ್ರ(hints));
  hints.ai_family = pf;
  hints.ai_socktype = (transport == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_RESOLVE_ON_IPS
  /*
   * The AI_NUMERICHOST must not be set to get synthesized IPv6 address from
   * an IPv4 address on iOS and macOS.
   */
  ಆದರೆ((curlx_inet_pton(AF_INET, hostname, addrbuf) == 1) ||
     (curlx_inet_pton(AF_INET6, hostname, addrbuf) == 1)) {
    /* the given address is numerical only, prevent a reverse lookup */
    hints.ai_flags = AI_NUMERICHOST;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(ಪೋರ್ಟ್) {
    curl_msnprintf(sbuf, ಗಾತ್ರ(sbuf), "%d", ಪೋರ್ಟ್);
    sbufptr = sbuf;
  }

  error = Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &res);
  ಆದರೆ(error) {
    infof(ದತ್ತಾಂಶ, "getaddrinfo(3) failed for %s:%d", hostname, ಪೋರ್ಟ್);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  ಆದರೆ(ಪೋರ್ಟ್) {
    Curl_addrinfo_ಗಣ_ಪೋರ್ಟ್(res, ಪೋರ್ಟ್);
  }

  ಹಿಂತಿರುಗಿ res;
}
#ಆದರೆ_ಅಂತ್ಯ /* CURLRES_SYNCH */

#ಆದರೆ_ಅಂತ್ಯ /* CURLRES_IPV6 */
