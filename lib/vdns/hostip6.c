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
#جے_تعریف_شدہ CURLRES_IPV6

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جے_انت

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "url.h"
#ضمن "vdns/hostip.h"
#ضمن "curlx/inet_pton.h"
#ضمن "connect.h"

#جے_تعریف_شدہ CURLRES_SYNCH

/*
 * Curl_sync_getaddrinfo() when built IPv6-enabled (non-threading and
 * non-ares version).
 *
 * Returns name information about the given hostname and port number. If
 * successful, the 'addrinfo' is returned and the fourth argument will point
 * to memory we need to free after use. That memory *MUST* be freed with
 * Curl_freeaddrinfo(), nothing else.
 */
بنية Curl_addrinfo *Curl_sync_getaddrinfo(بنية Curl_easy *انکڑے,
                                            uint8_t dns_queries,
                                            ثابت حرف *hostname,
                                            uint16_t پورٹ,
                                            uint8_t transport)
{
  بنية addrinfo hints;
  بنية Curl_addrinfo *res;
  صحيح error;
  حرف sbuf[12];
  حرف *sbufptr = عدم;
#جے_بنا_تعریف USE_RESOLVE_ON_IPS
  حرف addrbuf[128];
#جے_انت
  صحيح pf = PF_INET;

  إذا(dns_queries & CURL_DNSQ_AAAA)
    pf = PF_UNSPEC;

  memset(&hints, 0, حجم(hints));
  hints.ai_family = pf;
  hints.ai_socktype = (transport == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#جے_بنا_تعریف USE_RESOLVE_ON_IPS
  /*
   * The AI_NUMERICHOST must not be set to get synthesized IPv6 address from
   * an IPv4 address on iOS and macOS.
   */
  إذا((curlx_inet_pton(AF_INET, hostname, addrbuf) == 1) ||
     (curlx_inet_pton(AF_INET6, hostname, addrbuf) == 1)) {
    /* the given address is numerical only, prevent a reverse lookup */
    hints.ai_flags = AI_NUMERICHOST;
  }
#جے_انت

  إذا(پورٹ) {
    curl_msnprintf(sbuf, حجم(sbuf), "%d", پورٹ);
    sbufptr = sbuf;
  }

  error = Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &res);
  إذا(error) {
    infof(انکڑے, "getaddrinfo(3) failed for %s:%d", hostname, پورٹ);
    أرجع عدم;
  }

  إذا(پورٹ) {
    Curl_addrinfo_سیٹ_پورٹ(res, پورٹ);
  }

  أرجع res;
}
#جے_انت /* CURLRES_SYNCH */

#جے_انت /* CURLRES_IPV6 */
