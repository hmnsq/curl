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
#شامل "curl_setup.h"

/***********************************************************************
 * Only for IPv6-enabled builds
 **********************************************************************/
#اگر_تعریف_شدہ CURLRES_IPV6

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "curl_addrinfo.h"
#شامل "curl_trc.h"
#شامل "url.h"
#شامل "vdns/hostip.h"
#شامل "curlx/inet_pton.h"
#شامل "connect.h"

#اگر_تعریف_شدہ CURLRES_SYNCH

/*
 * Curl_sync_getaddrinfo() when built IPv6-enabled (non-threading and
 * non-ares version).
 *
 * Returns name information about the given hostname and port number. If
 * successful, the 'addrinfo' is returned and the fourth argument will point
 * to memory we need to free after use. That memory *MUST* be freed with
 * Curl_freeaddrinfo(), nothing else.
 */
ساخت Curl_addrinfo *Curl_sync_getaddrinfo(ساخت Curl_easy *ڈیٹا,
                                            uint8_t dns_queries,
                                            ثابت حرف *hostname,
                                            uint16_t پورٹ,
                                            uint8_t transport)
{
  ساخت addrinfo hints;
  ساخت Curl_addrinfo *res;
  صحیح error;
  حرف sbuf[12];
  حرف *sbufptr = تہی;
#اگر_غیر_تعریف_شدہ USE_RESOLVE_ON_IPS
  حرف addrbuf[128];
#اگر_ختم
  صحیح pf = PF_INET;

  اگر(dns_queries & CURL_DNSQ_AAAA)
    pf = PF_UNSPEC;

  memset(&hints, 0, حجم(hints));
  hints.ai_family = pf;
  hints.ai_socktype = (transport == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#اگر_غیر_تعریف_شدہ USE_RESOLVE_ON_IPS
  /*
   * The AI_NUMERICHOST must not be set to get synthesized IPv6 address from
   * an IPv4 address on iOS and macOS.
   */
  اگر((curlx_inet_pton(AF_INET, hostname, addrbuf) == 1) ||
     (curlx_inet_pton(AF_INET6, hostname, addrbuf) == 1)) {
    /* the given address is numerical only, prevent a reverse lookup */
    hints.ai_flags = AI_NUMERICHOST;
  }
#اگر_ختم

  اگر(پورٹ) {
    curl_msnprintf(sbuf, حجم(sbuf), "%d", پورٹ);
    sbufptr = sbuf;
  }

  error = Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &res);
  اگر(error) {
    infof(ڈیٹا, "getaddrinfo(3) failed for %s:%d", hostname, پورٹ);
    واپس تہی;
  }

  اگر(پورٹ) {
    Curl_addrinfo_سیٹ_پورٹ(res, پورٹ);
  }

  واپس res;
}
#اگر_ختم /* CURLRES_SYNCH */

#اگر_ختم /* CURLRES_IPV6 */
