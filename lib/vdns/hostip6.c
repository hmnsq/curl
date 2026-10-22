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
#অন্তর্ভাব "curl_setup.h"

/***********************************************************************
 * Only for IPv6-enabled builds
 **********************************************************************/
#ifdef CURLRES_IPV6

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "vdns/hostip.h"
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "connect.h"

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
সংরচনা Curl_addrinfo *Curl_sync_getaddrinfo(সংরচনা Curl_easy *data,
                                            uint8_t dns_queries,
                                            ধ্রুব বর্ণ *hostname,
                                            uint16_t port,
                                            uint8_t transport)
{
  সংরচনা addrinfo hints;
  সংরচনা Curl_addrinfo *res;
  পূর্ণাঙ্ক error;
  বর্ণ sbuf[12];
  বর্ণ *sbufptr = রিক্ত;
#ifndef USE_RESOLVE_ON_IPS
  বর্ণ addrbuf[128];
#endif
  পূর্ণাঙ্ক pf = PF_INET;

  যদি(dns_queries & CURL_DNSQ_AAAA)
    pf = PF_UNSPEC;

  memset(&hints, 0, পরিমাণ(hints));
  hints.ai_family = pf;
  hints.ai_socktype = (transport == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifndef USE_RESOLVE_ON_IPS
  /*
   * The AI_NUMERICHOST must not be set to get synthesized IPv6 address from
   * an IPv4 address on iOS and macOS.
   */
  যদি((curlx_inet_pton(AF_INET, hostname, addrbuf) == 1) ||
     (curlx_inet_pton(AF_INET6, hostname, addrbuf) == 1)) {
    /* the given address is numerical only, prevent a reverse lookup */
    hints.ai_flags = AI_NUMERICHOST;
  }
#endif

  যদি(port) {
    curl_msnprintf(sbuf, পরিমাণ(sbuf), "%d", port);
    sbufptr = sbuf;
  }

  error = Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &res);
  যদি(error) {
    infof(data, "getaddrinfo(3) failed for %s:%d", hostname, port);
    প্রত্যাগম রিক্ত;
  }

  যদি(port) {
    Curl_addrinfo_set_port(res, port);
  }

  প্রত্যাগম res;
}
#endif /* CURLRES_SYNCH */

#endif /* CURLRES_IPV6 */
