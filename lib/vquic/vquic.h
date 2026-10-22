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
#অন্তর্ভাৱ "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
সংরচনা Curl_cfilter;
সংরচনা Curl_easy;
সংরচনা connectdata;
সংরচনা Curl_addrinfo;

শূন্য Curl_quic_ver(ৱর্ণ *p, size_t len);
পূর্ণাঙ্ক Curl_vquic_init(শূন্য);
শূন্য Curl_vquic_cleanup(শূন্য);

CURLcode Curl_qlogdir(সংরচনা Curl_easy *data,
                      unsigned ৱর্ণ *scid,
                      size_t scidlen,
                      পূর্ণাঙ্ক *qlogfdp);

CURLcode Curl_cf_quic_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা Curl_peer *origin,
                                   সংরচনা Curl_peer *peer);

CURLcode Curl_cf_quic_create(সংরচনা Curl_cfilter **pcf,
                             সংরচনা Curl_easy *data,
                             সংরচনা Curl_peer *origin,
                             সংরচনা Curl_peer *peer,
                             uint8_t transport_peer,
                             সংরচনা connectdata *conn,
                             সংরচনা Curl_sockaddr_ex *addr,
                             সংরচনা Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

বাহ্য সংরচনা Curl_cftype Curl_cft_http3;

#যদি !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                       সংরচনা Curl_easy *data,
                                       সংরচনা Curl_peer *origin,
                                       সংরচনা Curl_peer *peer,
                                       সংরচনা Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(সংরচনা Curl_cfilter **pcf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা Curl_peer *origin,
                                 সংরচনা Curl_peer *peer,
                                 uint8_t transport_peer,
                                 সংরচনা connectdata *conn,
                                 সংরচনা Curl_sockaddr_ex *addr,
                                 সংরচনা Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

বাহ্য সংরচনা Curl_cftype Curl_cft_h3_proxy;

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#অন্যথা
#define Curl_vquic_init() 1
#define Curl_vquic_cleanup()
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(সংরচনা Curl_easy *data,
                             সংরচনা connectdata *conn);

#endif /* HEADER_CURL_VQUIC_QUIC_H */
