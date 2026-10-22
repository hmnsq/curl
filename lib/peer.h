#ifndef HEADER_CURL_PEER_H
#define HEADER_CURL_PEER_H
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

संरचना Curl_scheme;

/* if peer hostname starts with this, the peer is a unix domain socket
 * path, e.g. the remainder after 'localhost'. */
#define CURL_PEER_UDS_PREFIX "localhost/"

संरचना Curl_peer {
  अचर संरचना Curl_scheme *scheme; /* url scheme */
  वर्ण *hostname; /* normalized hostname (IDN decoded when supported) */
  वर्ण *zoneid; /* NULL or IPv6 zone identifier */
  uint32_t refcount;  /* created with 1, freed when dropping to 0 */
  uint32_t scopeid;  /* != 0, IPv6 scope to use */
  uint16_t port;
  BIT(unix_socket); /* hostname is a UDS path without the prefix */
  BIT(abstract_uds); /* only TRUE when `unix_socket` also TRUE */
  BIT(ipv6); /* hostname is an IPv6 address stripped of '[]' */
  वर्ण user_hostname[1]; /* hostname supplied by user/url */
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
CURLcode Curl_peer_create(संरचना Curl_easy *data,
                          अचर संरचना Curl_scheme *scheme,
                          अचर वर्ण *hostname,
                          uint16_t port,
                          संरचना Curl_peer **ppeer);

#ifdef USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(अचर संरचना Curl_scheme *scheme,
                              अचर वर्ण *path,
                              bool abstract_unix_socket,
                              संरचना Curl_peer **ppeer);
#endif

/* Unlink any peer in `*pdest`, assign src, increase src
 * refcount when not NULL. */
शून्य Curl_peer_link(संरचना Curl_peer **pdest, संरचना Curl_peer *src);

/* Drop a reference, peer may be passed as NULL */
शून्य Curl_peer_unlink(संरचना Curl_peer **ppeer);

/* TRUE if both peers are NULL or have completely same properties. */
bool Curl_peer_equal(संरचना Curl_peer *p1, संरचना Curl_peer *p2);

/* TRUE if both peers are NULL or have same properties except the scheme. */
bool Curl_peer_same_destination(संरचना Curl_peer *p1, संरचना Curl_peer *p2);

CURLcode Curl_peer_from_url(CURLU *uh, संरचना Curl_easy *data,
                            uint16_t port_override,
                            uint32_t scopeid_override,
                            संरचना Curl_peer **ppeer);

CURLcode Curl_peer_from_connect_to(संरचना Curl_easy *data,
                                   अचर संरचना Curl_peer *dest,
                                   अचर वर्ण *connect_to,
                                   संरचना Curl_peer **ppeer);

#ifndef CURL_DISABLE_PROXY

CURLcode Curl_scheme_to_proxytype(संरचना Curl_easy *data,
                                  अचर वर्ण *scheme,
                                  uint8_t *proxytype,
                                  अचर वर्ण *url);

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  संरचना Curl_easy *data,
                                  अचर वर्ण *url,
                                  uint8_t proxytype,
                                  संरचना Curl_peer **ppeer,
                                  uint8_t *pproxytype);
#endif /* !CURL_DISABLE_PROXY */

#endif /* HEADER_CURL_PEER_H */
