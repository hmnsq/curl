#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_PEER_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_PEER_H
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

ਬਣਤਰ Curl_scheme;

/* if peer hostname starts with this, the peer is a unix domain socket
 * path, e.g. the remainder after 'localhost'. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_PEER_UDS_ਅਗੇਤਰ "localhost/"

ਬਣਤਰ Curl_peer {
  ਅਚਲ ਬਣਤਰ Curl_scheme *scheme; /* url scheme */
  ਅੱਖਰ *hostname; /* normalized hostname (IDN decoded when supported) */
  ਅੱਖਰ *zoneid; /* NULL or IPv6 zone identifier */
  uint32_t refcount;  /* created with 1, freed when dropping to 0 */
  uint32_t scopeid;  /* != 0, IPv6 scope to use */
  uint16_t ਪੋਰਟ;
  ਦੋਆਧਾਰੀ_ਅੰਕ(unix_socket); /* hostname is a UDS path without the prefix */
  ਦੋਆਧਾਰੀ_ਅੰਕ(abstract_uds); /* only TRUE when `unix_socket` also TRUE */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ipv6); /* hostname is an IPv6 address stripped of '[]' */
  ਅੱਖਰ user_hostname[1]; /* hostname supplied by user/url */
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
CURLcode Curl_peer_create(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅਚਲ ਬਣਤਰ Curl_scheme *scheme,
                          ਅਚਲ ਅੱਖਰ *hostname,
                          uint16_t ਪੋਰਟ,
                          ਬਣਤਰ Curl_peer **ppeer);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ਅਚਲ ਬਣਤਰ Curl_scheme *scheme,
                              ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                              bool abstract_unix_socket,
                              ਬਣਤਰ Curl_peer **ppeer);
#ਜੇ_ਅੰਤ

/* Unlink any peer in `*pdest`, assign src, increase src
 * refcount when not NULL. */
ਖਾਲੀ Curl_peer_link(ਬਣਤਰ Curl_peer **pdest, ਬਣਤਰ Curl_peer *src);

/* Drop a reference, peer may be passed as NULL */
ਖਾਲੀ Curl_peer_unlink(ਬਣਤਰ Curl_peer **ppeer);

/* TRUE if both peers are NULL or have completely same properties. */
bool Curl_peer_equal(ਬਣਤਰ Curl_peer *p1, ਬਣਤਰ Curl_peer *p2);

/* TRUE if both peers are NULL or have same properties except the scheme. */
bool Curl_peer_same_destination(ਬਣਤਰ Curl_peer *p1, ਬਣਤਰ Curl_peer *p2);

CURLcode Curl_peer_from_url(CURLU *uh, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            uint16_t ਪੋਰਟ_override,
                            uint32_t scopeid_override,
                            ਬਣਤਰ Curl_peer **ppeer);

CURLcode Curl_peer_from_connect_ਮੰਜ਼ਿਲ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਅਚਲ ਬਣਤਰ Curl_peer *dest,
                                   ਅਚਲ ਅੱਖਰ *connect_ਮੰਜ਼ਿਲ,
                                   ਬਣਤਰ Curl_peer **ppeer);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY

CURLcode Curl_scheme_ਮੰਜ਼ਿਲ_proxytype(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਅੱਖਰ *scheme,
                                  uint8_t *proxytype,
                                  ਅਚਲ ਅੱਖਰ *url);

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਅੱਖਰ *url,
                                  uint8_t proxytype,
                                  ਬਣਤਰ Curl_peer **ppeer,
                                  uint8_t *pproxytype);
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY */

#ਜੇ_ਅੰਤ /* HEADER_CURL_PEER_H */
