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
#అన్తర్భావ "curl_setup.h"

/***********************************************************************
 * Only for IPv6-enabled builds
 **********************************************************************/
#నిర్వచితమైతే CURLRES_IPV6

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "vdns/hostip.h"
#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "connect.h"

#నిర్వచితమైతే CURLRES_SYNCH

/*
 * Curl_sync_getaddrinfo() when built IPv6-enabled (non-threading and
 * non-ares version).
 *
 * Returns name information about the given hostname and port number. If
 * successful, the 'addrinfo' is returned and the fourth argument will point
 * to memory we need to free after use. That memory *MUST* be freed with
 * Curl_freeaddrinfo(), nothing else.
 */
నిర్మాణం Curl_addrinfo *Curl_sync_getaddrinfo(నిర్మాణం Curl_easy *దత్తాంశం,
                                            uint8_t dns_queries,
                                            స్థిరరాశి అక్షరం *hostname,
                                            uint16_t పోర్టు,
                                            uint8_t transport)
{
  నిర్మాణం addrinfo hints;
  నిర్మాణం Curl_addrinfo *res;
  పూర్ణాంకం error;
  అక్షరం sbuf[12];
  అక్షరం *sbufptr = ఖాళీ;
#నిర్వచితం_కాకపోతే USE_RESOLVE_ON_IPS
  అక్షరం addrbuf[128];
#అయితే_ముగింపు
  పూర్ణాంకం pf = PF_INET;

  ఒకవేళ(dns_queries & CURL_DNSQ_AAAA)
    pf = PF_UNSPEC;

  memset(&hints, 0, పరిమాణం(hints));
  hints.ai_family = pf;
  hints.ai_socktype = (transport == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#నిర్వచితం_కాకపోతే USE_RESOLVE_ON_IPS
  /*
   * The AI_NUMERICHOST must not be set to get synthesized IPv6 address from
   * an IPv4 address on iOS and macOS.
   */
  ఒకవేళ((curlx_inet_pton(AF_INET, hostname, addrbuf) == 1) ||
     (curlx_inet_pton(AF_INET6, hostname, addrbuf) == 1)) {
    /* the given address is numerical only, prevent a reverse lookup */
    hints.ai_flags = AI_NUMERICHOST;
  }
#అయితే_ముగింపు

  ఒకవేళ(పోర్టు) {
    curl_msnprintf(sbuf, పరిమాణం(sbuf), "%d", పోర్టు);
    sbufptr = sbuf;
  }

  error = Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &res);
  ఒకవేళ(error) {
    infof(దత్తాంశం, "getaddrinfo(3) failed for %s:%d", hostname, పోర్టు);
    తిరిగి ఖాళీ;
  }

  ఒకవేళ(పోర్టు) {
    Curl_addrinfo_సమితి_పోర్టు(res, పోర్టు);
  }

  తిరిగి res;
}
#అయితే_ముగింపు /* CURLRES_SYNCH */

#అయితే_ముగింపు /* CURLRES_IPV6 */
