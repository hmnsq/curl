#جے_بنا_تعریف سرخی_CURL_PEER_H
#تعریف_کرو سرخی_CURL_PEER_H
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

بنية Curl_scheme;

/* if peer hostname starts with this, the peer is a unix domain socket
 * path, e.g. the remainder after 'localhost'. */
#تعریف_کرو CURL_PEER_UDS_اگیتر "localhost/"

بنية Curl_peer {
  ثابت بنية Curl_scheme *scheme; /* url scheme */
  حرف *hostname; /* normalized hostname (IDN decoded when supported) */
  حرف *zoneid; /* NULL or IPv6 zone identifier */
  uint32_t refcount;  /* created with 1, freed when dropping to 0 */
  uint32_t scopeid;  /* != 0, IPv6 scope to use */
  uint16_t پورٹ;
  ثنائی_ہندسہ(unix_socket); /* hostname is a UDS path without the prefix */
  ثنائی_ہندسہ(abstract_uds); /* only TRUE when `unix_socket` also TRUE */
  ثنائی_ہندسہ(ipv6); /* hostname is an IPv6 address stripped of '[]' */
  حرف user_hostname[1]; /* hostname supplied by user/url */
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
CURLcode Curl_peer_create(بنية Curl_easy *انکڑے,
                          ثابت بنية Curl_scheme *scheme,
                          ثابت حرف *hostname,
                          uint16_t پورٹ,
                          بنية Curl_peer **ppeer);

#جے_تعریف_شدہ USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ثابت بنية Curl_scheme *scheme,
                              ثابت حرف *رستہ,
                              bool abstract_unix_socket,
                              بنية Curl_peer **ppeer);
#جے_انت

/* Unlink any peer in `*pdest`, assign src, increase src
 * refcount when not NULL. */
فراغ Curl_peer_link(بنية Curl_peer **pdest, بنية Curl_peer *src);

/* Drop a reference, peer may be passed as NULL */
فراغ Curl_peer_unlink(بنية Curl_peer **ppeer);

/* TRUE if both peers are NULL or have completely same properties. */
bool Curl_peer_equal(بنية Curl_peer *p1, بنية Curl_peer *p2);

/* TRUE if both peers are NULL or have same properties except the scheme. */
bool Curl_peer_same_destination(بنية Curl_peer *p1, بنية Curl_peer *p2);

CURLcode Curl_peer_from_url(CURLU *uh, بنية Curl_easy *انکڑے,
                            uint16_t پورٹ_override,
                            uint32_t scopeid_override,
                            بنية Curl_peer **ppeer);

CURLcode Curl_peer_from_connect_منزل(بنية Curl_easy *انکڑے,
                                   ثابت بنية Curl_peer *dest,
                                   ثابت حرف *connect_منزل,
                                   بنية Curl_peer **ppeer);

#جے_بنا_تعریف CURL_DISABLE_PROXY

CURLcode Curl_scheme_منزل_proxytype(بنية Curl_easy *انکڑے,
                                  ثابت حرف *scheme,
                                  uint8_t *proxytype,
                                  ثابت حرف *url);

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  بنية Curl_easy *انکڑے,
                                  ثابت حرف *url,
                                  uint8_t proxytype,
                                  بنية Curl_peer **ppeer,
                                  uint8_t *pproxytype);
#جے_انت /* !CURL_DISABLE_PROXY */

#جے_انت /* HEADER_CURL_PEER_H */
