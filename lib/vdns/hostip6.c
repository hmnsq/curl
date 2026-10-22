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
#समावेश "curl_setup.h"

/***********************************************************************
 * Only for IPv6-enabled builds
 **********************************************************************/
#अगर_घोषित CURLRES_IPV6

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "cfilters.h"
#समावेश "curl_addrinfo.h"
#समावेश "curl_trc.h"
#समावेश "url.h"
#समावेश "vdns/hostip.h"
#समावेश "curlx/inet_pton.h"
#समावेश "connect.h"

#अगर_घोषित CURLRES_SYNCH

/*
 * Curl_sync_getaddrinfo() when built IPv6-enabled (non-threading and
 * non-ares version).
 *
 * Returns name information about the given hostname and port number. If
 * successful, the 'addrinfo' is returned and the fourth argument will point
 * to memory we need to free after use. That memory *MUST* be freed with
 * Curl_freeaddrinfo(), nothing else.
 */
काष्ठा Curl_addrinfo *Curl_sync_getaddrinfo(काष्ठा Curl_easy *तथ्य,
                                            uint8_t dns_queries,
                                            स्थिर अक्षर *hostname,
                                            uint16_t port,
                                            uint8_t transport)
{
  काष्ठा addrinfo hints;
  काष्ठा Curl_addrinfo *res;
  पूर्णांक error;
  अक्षर sbuf[12];
  अक्षर *sbufptr = शून्य;
#अगर_अघोषित USE_RESOLVE_ON_IPS
  अक्षर addrbuf[128];
#पूर्ण_अगर
  पूर्णांक pf = PF_INET;

  अगर(dns_queries & CURL_DNSQ_AAAA)
    pf = PF_UNSPEC;

  स_रखो(&hints, 0, माप(hints));
  hints.ai_family = pf;
  hints.ai_socktype = (transport == TRNSPRT_TCP) ?
    SOCK_STREAM : SOCK_DGRAM;

#अगर_अघोषित USE_RESOLVE_ON_IPS
  /*
   * The AI_NUMERICHOST must not be set to get synthesized IPv6 address from
   * an IPv4 address on iOS and macOS.
   */
  अगर((curlx_inet_pton(AF_INET, hostname, addrbuf) == 1) ||
     (curlx_inet_pton(AF_INET6, hostname, addrbuf) == 1)) {
    /* the given address is numerical only, prevent a reverse lookup */
    hints.ai_flags = AI_NUMERICHOST;
  }
#पूर्ण_अगर

  अगर(port) {
    curl_msnprintf(sbuf, माप(sbuf), "%d", port);
    sbufptr = sbuf;
  }

  error = Curl_getaddrinfo_ex(hostname, sbufptr, &hints, &res);
  अगर(error) {
    infof(तथ्य, "getaddrinfo(3) failed for %s:%d", hostname, port);
    वापस शून्य;
  }

  अगर(port) {
    Curl_addrinfo_set_port(res, port);
  }

  वापस res;
}
#पूर्ण_अगर /* CURLRES_SYNCH */

#पूर्ण_अगर /* CURLRES_IPV6 */
