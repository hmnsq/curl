#اگر_غیر_تعریف_شدہ سرخی_CURL_VQUIC_QUIC_H
#تعریف_کرو سرخی_CURL_VQUIC_QUIC_H
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
ساخت Curl_cfilter;
ساخت Curl_easy;
ساخت connectdata;
ساخت Curl_addrinfo;

خالی Curl_quic_ver(حرف *p, size_t len);
صحیح Curl_vquic_init(خالی);
خالی Curl_vquic_cleanup(خالی);

CURLcode Curl_qlogdir(ساخت Curl_easy *ڈیٹا,
                      غیرعلامتی حرف *scid,
                      size_t scidlen,
                      صحیح *qlogfdp);

CURLcode Curl_cf_quic_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت Curl_peer *origin,
                                   ساخت Curl_peer *peer);

CURLcode Curl_cf_quic_create(ساخت Curl_cfilter **pcf,
                             ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_peer *origin,
                             ساخت Curl_peer *peer,
                             uint8_t transport_peer,
                             ساخت connectdata *conn,
                             ساخت Curl_sockaddr_ex *addr,
                             ساخت Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

خارجی ساخت Curl_cftype Curl_cft_http3;

#اگر !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                       ساخت Curl_easy *ڈیٹا,
                                       ساخت Curl_peer *origin,
                                       ساخت Curl_peer *peer,
                                       ساخت Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(ساخت Curl_cfilter **pcf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت Curl_peer *origin,
                                 ساخت Curl_peer *peer,
                                 uint8_t transport_peer,
                                 ساخت connectdata *conn,
                                 ساخت Curl_sockaddr_ex *addr,
                                 ساخت Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

خارجی ساخت Curl_cftype Curl_cft_h3_proxy;

#اگر_ختم /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#ورنہ
#تعریف_کرو Curl_vquic_init() 1
#تعریف_کرو Curl_vquic_cleanup()
#اگر_ختم /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(ساخت Curl_easy *ڈیٹا,
                             ساخت connectdata *conn);

#اگر_ختم /* HEADER_CURL_VQUIC_QUIC_H */
