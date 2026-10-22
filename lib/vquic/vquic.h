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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
ساختار Curl_cfilter;
ساختار Curl_easy;
ساختار connectdata;
ساختار Curl_addrinfo;

تهی Curl_quic_ver(نویسه *p, size_t len);
صحیح Curl_vquic_init(تهی);
تهی Curl_vquic_cleanup(تهی);

CURLcode Curl_qlogdir(ساختار Curl_easy *data,
                      unsigned نویسه *scid,
                      size_t scidlen,
                      صحیح *qlogfdp);

CURLcode Curl_cf_quic_درج_after(ساختار Curl_cfilter *cf_at,
                                   ساختار Curl_easy *data,
                                   ساختار Curl_peer *origin,
                                   ساختار Curl_peer *peer);

CURLcode Curl_cf_quic_create(ساختار Curl_cfilter **pcf,
                             ساختار Curl_easy *data,
                             ساختار Curl_peer *origin,
                             ساختار Curl_peer *peer,
                             uint8_t transport_peer,
                             ساختار connectdata *conn,
                             ساختار Curl_sockaddr_ex *addr,
                             ساختار Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

بیرونی ساختار Curl_cftype Curl_cft_http3;

#اگر !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_درج_after(ساختار Curl_cfilter *cf_at,
                                       ساختار Curl_easy *data,
                                       ساختار Curl_peer *origin,
                                       ساختار Curl_peer *peer,
                                       ساختار Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(ساختار Curl_cfilter **pcf,
                                 ساختار Curl_easy *data,
                                 ساختار Curl_peer *origin,
                                 ساختار Curl_peer *peer,
                                 uint8_t transport_peer,
                                 ساختار connectdata *conn,
                                 ساختار Curl_sockaddr_ex *addr,
                                 ساختار Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

بیرونی ساختار Curl_cftype Curl_cft_h3_proxy;

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#وگرنه
#define Curl_vquic_init() 1
#define Curl_vquic_cleanup()
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(ساختار Curl_easy *data,
                             ساختار connectdata *conn);

#endif /* HEADER_CURL_VQUIC_QUIC_H */
