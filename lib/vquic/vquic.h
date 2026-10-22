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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
கட்டமைப்பு Curl_cfilter;
கட்டமைப்பு Curl_easy;
கட்டமைப்பு connectdata;
கட்டமைப்பு Curl_addrinfo;

வெறுமை Curl_quic_ver(எழுத்து *p, size_t len);
முழு Curl_vquic_init(வெறுமை);
வெறுமை Curl_vquic_cleanup(வெறுமை);

CURLcode Curl_qlogdir(கட்டமைப்பு Curl_easy *data,
                      unsigned எழுத்து *scid,
                      size_t scidlen,
                      முழு *qlogfdp);

CURLcode Curl_cf_quic_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு Curl_peer *origin,
                                   கட்டமைப்பு Curl_peer *peer);

CURLcode Curl_cf_quic_create(கட்டமைப்பு Curl_cfilter **pcf,
                             கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_peer *origin,
                             கட்டமைப்பு Curl_peer *peer,
                             uint8_t transport_peer,
                             கட்டமைப்பு connectdata *conn,
                             கட்டமைப்பு Curl_sockaddr_ex *addr,
                             கட்டமைப்பு Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

வெளி கட்டமைப்பு Curl_cftype Curl_cft_http3;

#எனில் !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                       கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு Curl_peer *origin,
                                       கட்டமைப்பு Curl_peer *peer,
                                       கட்டமைப்பு Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(கட்டமைப்பு Curl_cfilter **pcf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு Curl_peer *origin,
                                 கட்டமைப்பு Curl_peer *peer,
                                 uint8_t transport_peer,
                                 கட்டமைப்பு connectdata *conn,
                                 கட்டமைப்பு Curl_sockaddr_ex *addr,
                                 கட்டமைப்பு Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

வெளி கட்டமைப்பு Curl_cftype Curl_cft_h3_proxy;

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#இல்லையெனில்
#define Curl_vquic_init() 1
#define Curl_vquic_cleanup()
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு connectdata *conn);

#endif /* HEADER_CURL_VQUIC_QUIC_H */
