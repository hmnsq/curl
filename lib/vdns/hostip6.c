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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

/***********************************************************************
 * Only for IPv6-enabled builds
 **********************************************************************/
#ifdef CURLRES_IPV6

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "vdns/hostip.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_pton.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"

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
ସଂରଚନା Curl_addrinfo *Curl_sync_getaddrinfo(ସଂରଚନା Curl_easy *data,
                                            uint8_t dns_queries,
                                            ଅଚଳ ଅକ୍ଷର *hostname,
                                            uint16_t port,
                                            uint8_t transport)
{
  ସଂରଚନା addrinfo hints;
  ସଂରଚନା Curl_addrinfo *res;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା error;
  ଅକ୍ଷର sbuf[12];
  ଅକ୍ଷର *sbufptr = ଖାଲି;
#ifndef USE_RESOLVE_ON_IPS
  ଅକ୍ଷର addrbuf[128];
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pf = PF_INET;

  ଯଦି(dns_queries & CURL_DNSQ_AAAA)
    pf = PF_UNSPEC;

  memset(&hints, 0, ଆକାର(hints));
  hints.ai_family = pf;
  hints.ai_socktype = (transport == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifndef USE_RESOLVE_ON_IPS
  /*
   * The AI_NUMERICHOST must not be set to get synthesized IPv6 address from
   * an IPv4 address on iOS and macOS.
   */
  ଯଦି((curlx_inet_pton(AF_INET, hostname, addrbuf) == 1) ||
     (curlx_inet_pton(AF_INET6, hostname, addrbuf) == 1)) {
    /* the given address is numerical only, prevent a reverse lookup */
    hints.ai_flags = AI_NUMERICHOST;
  }
#endif

  ଯଦି(port) {
    curl_msnprintf(sbuf, ଆକାର(sbuf), "%d", port);
    sbufptr = sbuf;
  }

  error = Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &res);
  ଯଦି(error) {
    infof(data, "getaddrinfo(3) failed for %s:%d", hostname, port);
    ଫେରନ୍ତୁ ଖାଲି;
  }

  ଯଦି(port) {
    Curl_addrinfo_set_port(res, port);
  }

  ଫେରନ୍ତୁ res;
}
#endif /* CURLRES_SYNCH */

#endif /* CURLRES_IPV6 */
