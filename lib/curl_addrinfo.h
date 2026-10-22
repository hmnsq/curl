#నిర్వచితం_కాకపోతే శీర్షిక_CURL_ADDRINFO_H
#నిర్వచించు శీర్షిక_CURL_ADDRINFO_H
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

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#  include <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#  include <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#  include <arpa/inet.h>
#అయితే_ముగింపు

#నిర్వచితమైతే __VMS
#  include <in.h>
#  include <inet.h>
#అయితే_ముగింపు

నిర్మాణం Curl_str;

/*
 * Curl_addrinfo is our internal struct definition that we use to allow
 * consistent internal handling of this data. We use this even when the system
 * provides an addrinfo structure definition. We use this for all sorts of
 * IPv4 and IPV6 builds.
 */

నిర్మాణం Curl_addrinfo {
  పూర్ణాంకం                   ai_flags;
  పూర్ణాంకం                   ai_family;
  పూర్ణాంకం                   ai_socktype;
  పూర్ణాంకం                   ai_protocol;
  curl_socklen_t        ai_addrlen;   /* Follow rfc3493 struct addrinfo */
  అక్షరం                 *ai_canonname;
  నిర్మాణం sockaddr      *ai_addr;
  నిర్మాణం Curl_addrinfo *ai_తదుపరి;
};

శూన్య Curl_freeaddrinfo(నిర్మాణం Curl_addrinfo *cahead);

/* Get the n-th addrinfo of family ai_family. */
నిర్మాణం Curl_addrinfo *Curl_addrinfo_get(నిర్మాణం Curl_addrinfo *ai,
                                        పూర్ణాంకం ai_family,
                                        గుర్తులేని పూర్ణాంకం n);

#నిర్వచితమైతే HAVE_GETADDRINFO
పూర్ణాంకం Curl_getaddrinfo_ex(స్థిరరాశి అక్షరం *nodename,
                        స్థిరరాశి అక్షరం *servname,
                        స్థిరరాశి నిర్మాణం addrinfo *hints,
                        నిర్మాణం Curl_addrinfo **ఫలితం);
#అయితే_ముగింపు

#ఒకవేళ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
నిర్మాణం Curl_addrinfo *Curl_he2ai(స్థిరరాశి నిర్మాణం hostent *he, పూర్ణాంకం పోర్టు);
#అయితే_ముగింపు

bool Curl_is_ipv4addr(స్థిరరాశి అక్షరం *చిరునామా);
bool Curl_is_ipaddr(స్థిరరాశి అక్షరం *చిరునామా);
bool Curl_looks_like_ipv6(స్థిరరాశి అక్షరం *s, size_t len, bool maybe_url_encoded,
                          నిర్మాణం Curl_str *host, నిర్మాణం Curl_str *zone);

CURLcode Curl_str2addr(స్థిరరాశి అక్షరం *dotted, uint16_t పోర్టు,
                       నిర్మాణం Curl_addrinfo **addrp);

#నిర్వచితమైతే USE_UNIX_SOCKETS
CURLcode Curl_unix2addr(స్థిరరాశి అక్షరం *మార్గం, bool abstract,
                        నిర్మాణం Curl_addrinfo **paddr);
#అయితే_ముగింపు

#ఒకవేళ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
శూన్య curl_dbg_freeaddrinfo(నిర్మాణం addrinfo *freethis, పూర్ణాంకం పంక్తి,
                           స్థిరరాశి అక్షరం *ఆధారం);
#అయితే_ముగింపు

#ఒకవేళ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
పూర్ణాంకం curl_dbg_getaddrinfo(స్థిరరాశి అక్షరం *hostname, స్థిరరాశి అక్షరం *service,
                         స్థిరరాశి నిర్మాణం addrinfo *hints,
                         నిర్మాణం addrinfo **ఫలితం, పూర్ణాంకం పంక్తి,
                         స్థిరరాశి అక్షరం *ఆధారం);
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_GETADDRINFO
#నిర్వచితమైతే USE_RESOLVE_ON_IPS
శూన్య Curl_addrinfo_సమితి_పోర్టు(నిర్మాణం Curl_addrinfo *addrinfo, పూర్ణాంకం పోర్టు);
#లేకపోతే
#నిర్వచించు Curl_addrinfo_సమితి_పోర్టు(x, y)
#అయితే_ముగింపు
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_ADDRINFO_H */
