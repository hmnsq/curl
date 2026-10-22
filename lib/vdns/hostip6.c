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
#अन्तर्भाव "curl_setup.h"

/***********************************************************************
 * Only for IPv6-enabled builds
 **********************************************************************/
#ifdef CURLRES_IPV6

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "connect.h"

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
संरचना Curl_addrinfo *Curl_sync_getaddrinfo(संरचना Curl_easy *data,
                                            uint8_t dns_queries,
                                            ध्रुव वर्ण *hostname,
                                            uint16_t port,
                                            uint8_t transport)
{
  संरचना addrinfo hints;
  संरचना Curl_addrinfo *res;
  पूर्णाङ्क error;
  वर्ण sbuf[12];
  वर्ण *sbufptr = रिक्त;
#ifndef USE_RESOLVE_ON_IPS
  वर्ण addrbuf[128];
#endif
  पूर्णाङ्क pf = PF_INET;

  यदि(dns_queries & CURL_DNSQ_AAAA)
    pf = PF_UNSPEC;

  memset(&hints, 0, परिमाण(hints));
  hints.ai_family = pf;
  hints.ai_socktype = (transport == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#ifndef USE_RESOLVE_ON_IPS
  /*
   * The AI_NUMERICHOST must not be set to get synthesized IPv6 address from
   * an IPv4 address on iOS and macOS.
   */
  यदि((curlx_inet_pton(AF_INET, hostname, addrbuf) == 1) ||
     (curlx_inet_pton(AF_INET6, hostname, addrbuf) == 1)) {
    /* the given address is numerical only, prevent a reverse lookup */
    hints.ai_flags = AI_NUMERICHOST;
  }
#endif

  यदि(port) {
    curl_msnprintf(sbuf, परिमाण(sbuf), "%d", port);
    sbufptr = sbuf;
  }

  error = Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &res);
  यदि(error) {
    infof(data, "getaddrinfo(3) failed for %s:%d", hostname, port);
    प्रत्यागम रिक्त;
  }

  यदि(port) {
    Curl_addrinfo_set_port(res, port);
  }

  प्रत्यागम res;
}
#endif /* CURLRES_SYNCH */

#endif /* CURLRES_IPV6 */
