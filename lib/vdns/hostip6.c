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

/***********************************************************************
 * Only for IPv6-enabled builds
 **********************************************************************/
#جيڪڏهن_وصف_ٿيل CURLRES_IPV6

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جيڪڏهن_پورو

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "url.h"
#ضمن "vdns/hostip.h"
#ضمن "curlx/inet_pton.h"
#ضمن "connect.h"

#جيڪڏهن_وصف_ٿيل CURLRES_SYNCH

/*
 * Curl_sync_getaddrinfo() when built IPv6-enabled (non-threading and
 * non-ares version).
 *
 * Returns name information about the given hostname and port number. If
 * successful, the 'addrinfo' is returned and the fourth argument will point
 * to memory we need to free after use. That memory *MUST* be freed with
 * Curl_freeaddrinfo(), nothing else.
 */
بنية Curl_addrinfo *Curl_sync_getaddrinfo(بنية Curl_easy *معلومات,
                                            uint8_t dns_queries,
                                            ثابت حرف *hostname,
                                            uint16_t پورٽ,
                                            uint8_t transport)
{
  بنية addrinfo hints;
  بنية Curl_addrinfo *res;
  صحيح error;
  حرف sbuf[12];
  حرف *sbufptr = عدم;
#جيڪڏهن_وصف_نه_ٿيل USE_RESOLVE_ON_IPS
  حرف addrbuf[128];
#جيڪڏهن_پورو
  صحيح pf = PF_INET;

  إذا(dns_queries & CURL_DNSQ_AAAA)
    pf = PF_UNSPEC;

  memset(&hints, 0, حجم(hints));
  hints.ai_family = pf;
  hints.ai_socktype = (transport == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#جيڪڏهن_وصف_نه_ٿيل USE_RESOLVE_ON_IPS
  /*
   * The AI_NUMERICHOST must not be set to get synthesized IPv6 address from
   * an IPv4 address on iOS and macOS.
   */
  إذا((curlx_inet_pton(AF_INET, hostname, addrbuf) == 1) ||
     (curlx_inet_pton(AF_INET6, hostname, addrbuf) == 1)) {
    /* the given address is numerical only, prevent a reverse lookup */
    hints.ai_flags = AI_NUMERICHOST;
  }
#جيڪڏهن_پورو

  إذا(پورٽ) {
    curl_msnprintf(sbuf, حجم(sbuf), "%d", پورٽ);
    sbufptr = sbuf;
  }

  error = Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &res);
  إذا(error) {
    infof(معلومات, "getaddrinfo(3) failed for %s:%d", hostname, پورٽ);
    أرجع عدم;
  }

  إذا(پورٽ) {
    Curl_addrinfo_سيٽ_پورٽ(res, پورٽ);
  }

  أرجع res;
}
#جيڪڏهن_پورو /* CURLRES_SYNCH */

#جيڪڏهن_پورو /* CURLRES_IPV6 */
