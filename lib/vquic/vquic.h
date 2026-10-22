#جے_بنا_تعریف سرخی_CURL_VQUIC_QUIC_H
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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
بنية Curl_cfilter;
بنية Curl_easy;
بنية connectdata;
بنية Curl_addrinfo;

فراغ Curl_quic_ver(حرف *p, size_t len);
صحيح Curl_vquic_init(فراغ);
فراغ Curl_vquic_cleanup(فراغ);

CURLcode Curl_qlogdir(بنية Curl_easy *انکڑے,
                      بنانشانی حرف *scid,
                      size_t scidlen,
                      صحيح *qlogfdp);

CURLcode Curl_cf_quic_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                   بنية Curl_easy *انکڑے,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer);

CURLcode Curl_cf_quic_create(بنية Curl_cfilter **pcf,
                             بنية Curl_easy *انکڑے,
                             بنية Curl_peer *origin,
                             بنية Curl_peer *peer,
                             uint8_t transport_peer,
                             بنية connectdata *conn,
                             بنية Curl_sockaddr_ex *addr,
                             بنية Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

خارجي بنية Curl_cftype Curl_cft_http3;

#إذا !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                       بنية Curl_easy *انکڑے,
                                       بنية Curl_peer *origin,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(بنية Curl_cfilter **pcf,
                                 بنية Curl_easy *انکڑے,
                                 بنية Curl_peer *origin,
                                 بنية Curl_peer *peer,
                                 uint8_t transport_peer,
                                 بنية connectdata *conn,
                                 بنية Curl_sockaddr_ex *addr,
                                 بنية Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

خارجي بنية Curl_cftype Curl_cft_h3_proxy;

#جے_انت /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#وإلا
#تعریف_کرو Curl_vquic_init() 1
#تعریف_کرو Curl_vquic_cleanup()
#جے_انت /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(بنية Curl_easy *انکڑے,
                             بنية connectdata *conn);

#جے_انت /* HEADER_CURL_VQUIC_QUIC_H */
