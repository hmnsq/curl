#నిర్వచితం_కాకపోతే శీర్షిక_CURL_PEER_H
#నిర్వచించు శీర్షిక_CURL_PEER_H
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

నిర్మాణం Curl_scheme;

/* if peer hostname starts with this, the peer is a unix domain socket
 * path, e.g. the remainder after 'localhost'. */
#నిర్వచించు CURL_PEER_UDS_ఉపసర్గ "localhost/"

నిర్మాణం Curl_peer {
  స్థిరరాశి నిర్మాణం Curl_scheme *scheme; /* url scheme */
  అక్షరం *hostname; /* normalized hostname (IDN decoded when supported) */
  అక్షరం *zoneid; /* NULL or IPv6 zone identifier */
  uint32_t refcount;  /* created with 1, freed when dropping to 0 */
  uint32_t scopeid;  /* != 0, IPv6 scope to use */
  uint16_t పోర్టు;
  బిట్టు(unix_socket); /* hostname is a UDS path without the prefix */
  బిట్టు(abstract_uds); /* only TRUE when `unix_socket` also TRUE */
  బిట్టు(ipv6); /* hostname is an IPv6 address stripped of '[]' */
  అక్షరం user_hostname[1]; /* hostname supplied by user/url */
};

/* Create a new peer:
 * - `peer->user_hostname` is the passed `hostname`
 * - `peer->hostname` is the normalized `hostname` via
 *    + IDN conversion if it has non-ASCII characters
 *    + stripping of surrounding '[]' for URL formatted IPv6 addresses
 *    + the path alone in case of a unix domain socket, e.g. hostname
 *      starts with CURL_PEER_UDS_PREFIX and is longer
 *   Scans for IPv6 addresses even without surrounding '[]'.
 * - `zoneid` IPv6 zone identifier or NULL
 * - `scopeid` IPv6 scopeid of zoneid, when known.
 */
CURLcode Curl_peer_create(నిర్మాణం Curl_easy *దత్తాంశం,
                          స్థిరరాశి నిర్మాణం Curl_scheme *scheme,
                          స్థిరరాశి అక్షరం *hostname,
                          uint16_t పోర్టు,
                          నిర్మాణం Curl_peer **ppeer);

#నిర్వచితమైతే USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(స్థిరరాశి నిర్మాణం Curl_scheme *scheme,
                              స్థిరరాశి అక్షరం *మార్గం,
                              bool abstract_unix_socket,
                              నిర్మాణం Curl_peer **ppeer);
#అయితే_ముగింపు

/* Unlink any peer in `*pdest`, assign src, increase src
 * refcount when not NULL. */
శూన్య Curl_peer_link(నిర్మాణం Curl_peer **pdest, నిర్మాణం Curl_peer *src);

/* Drop a reference, peer may be passed as NULL */
శూన్య Curl_peer_unlink(నిర్మాణం Curl_peer **ppeer);

/* TRUE if both peers are NULL or have completely same properties. */
bool Curl_peer_equal(నిర్మాణం Curl_peer *p1, నిర్మాణం Curl_peer *p2);

/* TRUE if both peers are NULL or have same properties except the scheme. */
bool Curl_peer_same_destination(నిర్మాణం Curl_peer *p1, నిర్మాణం Curl_peer *p2);

CURLcode Curl_peer_from_url(CURLU *uh, నిర్మాణం Curl_easy *దత్తాంశం,
                            uint16_t పోర్టు_override,
                            uint32_t scopeid_override,
                            నిర్మాణం Curl_peer **ppeer);

CURLcode Curl_peer_from_connect_గమ్యం(నిర్మాణం Curl_easy *దత్తాంశం,
                                   స్థిరరాశి నిర్మాణం Curl_peer *dest,
                                   స్థిరరాశి అక్షరం *connect_గమ్యం,
                                   నిర్మాణం Curl_peer **ppeer);

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY

CURLcode Curl_scheme_గమ్యం_proxytype(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి అక్షరం *scheme,
                                  uint8_t *proxytype,
                                  స్థిరరాశి అక్షరం *url);

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి అక్షరం *url,
                                  uint8_t proxytype,
                                  నిర్మాణం Curl_peer **ppeer,
                                  uint8_t *pproxytype);
#అయితే_ముగింపు /* !CURL_DISABLE_PROXY */

#అయితే_ముగింపు /* HEADER_CURL_PEER_H */
