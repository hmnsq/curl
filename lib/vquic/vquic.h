#ifndef HEADER_CURL_VQUIC_QUIC_H
#define HEADER_CURL_VQUIC_QUIC_H
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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
ഘടന Curl_cfilter;
ഘടന Curl_easy;
ഘടന connectdata;
ഘടന Curl_addrinfo;

ശൂന്യം Curl_quic_ver(അക്ഷരം *p, size_t len);
പൂർണ്ണസംഖ്യ Curl_vquic_init(ശൂന്യം);
ശൂന്യം Curl_vquic_cleanup(ശൂന്യം);

CURLcode Curl_qlogdir(ഘടന Curl_easy *data,
                      unsigned അക്ഷരം *scid,
                      size_t scidlen,
                      പൂർണ്ണസംഖ്യ *qlogfdp);

CURLcode Curl_cf_quic_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                   ഘടന Curl_easy *data,
                                   ഘടന Curl_peer *origin,
                                   ഘടന Curl_peer *peer);

CURLcode Curl_cf_quic_create(ഘടന Curl_cfilter **pcf,
                             ഘടന Curl_easy *data,
                             ഘടന Curl_peer *origin,
                             ഘടന Curl_peer *peer,
                             uint8_t transport_peer,
                             ഘടന connectdata *conn,
                             ഘടന Curl_sockaddr_ex *addr,
                             ഘടന Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

ബാഹ്യ ഘടന Curl_cftype Curl_cft_http3;

#എങ്കിൽ !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                       ഘടന Curl_easy *data,
                                       ഘടന Curl_peer *origin,
                                       ഘടന Curl_peer *peer,
                                       ഘടന Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(ഘടന Curl_cfilter **pcf,
                                 ഘടന Curl_easy *data,
                                 ഘടന Curl_peer *origin,
                                 ഘടന Curl_peer *peer,
                                 uint8_t transport_peer,
                                 ഘടന connectdata *conn,
                                 ഘടന Curl_sockaddr_ex *addr,
                                 ഘടന Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

ബാഹ്യ ഘടന Curl_cftype Curl_cft_h3_proxy;

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#അല്ലെങ്കിൽ
#define Curl_vquic_init() 1
#define Curl_vquic_cleanup()
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(ഘടന Curl_easy *data,
                             ഘടന connectdata *conn);

#endif /* HEADER_CURL_VQUIC_QUIC_H */
