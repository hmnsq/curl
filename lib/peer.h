#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_PEER_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_PEER_H
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

ರಚನೆ Curl_scheme;

/* if peer hostname starts with this, the peer is a unix domain socket
 * path, e.g. the remainder after 'localhost'. */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_PEER_UDS_ಪೂರ್ವಪ್ರತ್ಯಯ "localhost/"

ರಚನೆ Curl_peer {
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *scheme; /* url scheme */
  ಅಕ್ಷರ *hostname; /* normalized hostname (IDN decoded when supported) */
  ಅಕ್ಷರ *zoneid; /* NULL or IPv6 zone identifier */
  uint32_t refcount;  /* created with 1, freed when dropping to 0 */
  uint32_t scopeid;  /* != 0, IPv6 scope to use */
  uint16_t ಪೋರ್ಟ್;
  ಬಿಟ್(unix_socket); /* hostname is a UDS path without the prefix */
  ಬಿಟ್(abstract_uds); /* only TRUE when `unix_socket` also TRUE */
  ಬಿಟ್(ipv6); /* hostname is an IPv6 address stripped of '[]' */
  ಅಕ್ಷರ user_hostname[1]; /* hostname supplied by user/url */
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
CURLcode Curl_peer_create(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *scheme,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                          uint16_t ಪೋರ್ಟ್,
                          ರಚನೆ Curl_peer **ppeer);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
CURLcode Curl_peer_uds_create(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *scheme,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                              bool abstract_unix_socket,
                              ರಚನೆ Curl_peer **ppeer);
#ಆದರೆ_ಅಂತ್ಯ

/* Unlink any peer in `*pdest`, assign src, increase src
 * refcount when not NULL. */
ಶೂನ್ಯ Curl_peer_link(ರಚನೆ Curl_peer **pdest, ರಚನೆ Curl_peer *src);

/* Drop a reference, peer may be passed as NULL */
ಶೂನ್ಯ Curl_peer_unlink(ರಚನೆ Curl_peer **ppeer);

/* TRUE if both peers are NULL or have completely same properties. */
bool Curl_peer_equal(ರಚನೆ Curl_peer *p1, ರಚನೆ Curl_peer *p2);

/* TRUE if both peers are NULL or have same properties except the scheme. */
bool Curl_peer_same_destination(ರಚನೆ Curl_peer *p1, ರಚನೆ Curl_peer *p2);

CURLcode Curl_peer_from_url(CURLU *uh, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            uint16_t ಪೋರ್ಟ್_override,
                            uint32_t scopeid_override,
                            ರಚನೆ Curl_peer **ppeer);

CURLcode Curl_peer_from_connect_ಗಮ್ಯ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_peer *dest,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *connect_ಗಮ್ಯ,
                                   ರಚನೆ Curl_peer **ppeer);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY

CURLcode Curl_scheme_ಗಮ್ಯ_proxytype(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme,
                                  uint8_t *proxytype,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url);

CURLcode Curl_peer_from_proxy_url(CURLU *uh,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url,
                                  uint8_t proxytype,
                                  ರಚನೆ Curl_peer **ppeer,
                                  uint8_t *pproxytype);
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_PEER_H */
