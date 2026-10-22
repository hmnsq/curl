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
#ضمن "curl_setup.h"

#אם !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
بنية Curl_cfilter;
بنية Curl_easy;
بنية connectdata;
بنية Curl_addrinfo;

فراغ Curl_quic_ver(حرف *p, size_t len);
صحيح Curl_vquic_init(فراغ);
فراغ Curl_vquic_cleanup(فراغ);

CURLcode Curl_qlogdir(بنية Curl_easy *data,
                      unsigned حرف *scid,
                      size_t scidlen,
                      صحيح *qlogfdp);

CURLcode Curl_cf_quic_insert_after(بنية Curl_cfilter *cf_at,
                                   بنية Curl_easy *data,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer);

CURLcode Curl_cf_quic_create(بنية Curl_cfilter **pcf,
                             بنية Curl_easy *data,
                             بنية Curl_peer *origin,
                             بنية Curl_peer *peer,
                             uint8_t transport_peer,
                             بنية connectdata *conn,
                             بنية Curl_sockaddr_ex *addr,
                             بنية Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

خارجي بنية Curl_cftype Curl_cft_http3;

#אם !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_insert_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *data,
                                       بنية Curl_peer *origin,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(بنية Curl_cfilter **pcf,
                                 بنية Curl_easy *data,
                                 بنية Curl_peer *origin,
                                 بنية Curl_peer *peer,
                                 uint8_t transport_peer,
                                 بنية connectdata *conn,
                                 بنية Curl_sockaddr_ex *addr,
                                 بنية Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

خارجي بنية Curl_cftype Curl_cft_h3_proxy;

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#אחרת
#define Curl_vquic_init() 1
#define Curl_vquic_cleanup()
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(بنية Curl_easy *data,
                             بنية connectdata *conn);

#endif /* HEADER_CURL_VQUIC_QUIC_H */
