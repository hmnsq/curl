#अगर_अघोषित HEADER_CURL_PEER_H
#घोषणा HEADER_CURL_PEER_H
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

काष्ठा Curl_scheme;

/* if peer hostname starts with this, the peer is a unix domain socket
 * path, e.g. the remainder after 'localhost'. */
#घोषणा CURL_PEER_UDS_PREFIX "localhost/"

काष्ठा Curl_peer {
  स्थिर काष्ठा Curl_scheme *scheme; /* url scheme */
  अक्षर *hostname; /* normalized hostname (IDN decoded when supported) */
  अक्षर *zoneid; /* NULL or IPv6 zone identifier */
  uint32_t refcount;  /* created with 1, freed when dropping to 0 */
  uint32_t scopeid;  /* != 0, IPv6 scope to use */
  uint16_t port;
  BIT(unix_socket); /* hostname is a UDS path without the prefix */
  BIT(abstract_uds); /* only TRUE when `unix_socket` also TRUE */
  BIT(ipv6); /* hostname is an IPv6 address stripped of '[]' */
  अक्षर user_hostname[1]; /* hostname supplied by user/url */
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
CURLcode Curl_peer_create(काष्ठा Curl_easy *तथ्य,
                          स्थिर काष्ठा Curl_scheme *scheme,
                          स्थिर अक्षर *hostname,
                          uint16_t port,
                          काष्ठा Curl_peer **ppeer);

#अगर_घोषित USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(स्थिर काष्ठा Curl_scheme *scheme,
                              स्थिर अक्षर *path,
                              bool abstract_unix_socket,
                              काष्ठा Curl_peer **ppeer);
#पूर्ण_अगर

/* Unlink any peer in `*pdest`, assign src, increase src
 * refcount when not NULL. */
व्योम Curl_peer_link(काष्ठा Curl_peer **pdest, काष्ठा Curl_peer *src);

/* Drop a reference, peer may be passed as NULL */
व्योम Curl_peer_unlink(काष्ठा Curl_peer **ppeer);

/* TRUE if both peers are NULL or have completely same properties. */
bool Curl_peer_equal(काष्ठा Curl_peer *p1, काष्ठा Curl_peer *p2);

/* TRUE if both peers are NULL or have same properties except the scheme. */
bool Curl_peer_same_destination(काष्ठा Curl_peer *p1, काष्ठा Curl_peer *p2);

CURLcode Curl_peer_from_url(CURLU *uh, काष्ठा Curl_easy *तथ्य,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            काष्ठा Curl_peer **ppeer);

CURLcode Curl_peer_from_connect_to(काष्ठा Curl_easy *तथ्य,
                                   स्थिर काष्ठा Curl_peer *dest,
                                   स्थिर अक्षर *connect_to,
                                   काष्ठा Curl_peer **ppeer);

#अगर_अघोषित CURL_DISABLE_PROXY

CURLcode Curl_scheme_to_proxytype(काष्ठा Curl_easy *तथ्य,
                                  स्थिर अक्षर *scheme,
                                  uint8_t *proxytype,
                                  स्थिर अक्षर *url);

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  काष्ठा Curl_easy *तथ्य,
                                  स्थिर अक्षर *url,
                                  uint8_t proxytype,
                                  काष्ठा Curl_peer **ppeer,
                                  uint8_t *pproxytype);
#पूर्ण_अगर /* !CURL_DISABLE_PROXY */

#पूर्ण_अगर /* HEADER_CURL_PEER_H */
