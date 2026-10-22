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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
ସଂରଚନା Curl_cfilter;
ସଂରଚନା Curl_easy;
ସଂରଚନା connectdata;
ସଂରଚନା Curl_addrinfo;

ଶୂନ୍ୟ Curl_quic_ver(ଅକ୍ଷର *p, size_t len);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_vquic_init(ଶୂନ୍ୟ);
ଶୂନ୍ୟ Curl_vquic_cleanup(ଶୂନ୍ୟ);

CURLcode Curl_qlogdir(ସଂରଚନା Curl_easy *data,
                      unsigned ଅକ୍ଷର *scid,
                      size_t scidlen,
                      ପୂର୍ଣ୍ଣସଂଖ୍ୟା *qlogfdp);

CURLcode Curl_cf_quic_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                   ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା Curl_peer *origin,
                                   ସଂରଚନା Curl_peer *peer);

CURLcode Curl_cf_quic_create(ସଂରଚନା Curl_cfilter **pcf,
                             ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_peer *origin,
                             ସଂରଚନା Curl_peer *peer,
                             uint8_t transport_peer,
                             ସଂରଚନା connectdata *conn,
                             ସଂରଚନା Curl_sockaddr_ex *addr,
                             ସଂରଚନା Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

ବାହ୍ୟ ସଂରଚନା Curl_cftype Curl_cft_http3;

#ଯଦି !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                       ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା Curl_peer *origin,
                                       ସଂରଚନା Curl_peer *peer,
                                       ସଂରଚନା Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(ସଂରଚନା Curl_cfilter **pcf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା Curl_peer *origin,
                                 ସଂରଚନା Curl_peer *peer,
                                 uint8_t transport_peer,
                                 ସଂରଚନା connectdata *conn,
                                 ସଂରଚନା Curl_sockaddr_ex *addr,
                                 ସଂରଚନା Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

ବାହ୍ୟ ସଂରଚନା Curl_cftype Curl_cft_h3_proxy;

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#ଅନ୍ୟଥା
#define Curl_vquic_init() 1
#define Curl_vquic_cleanup()
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା connectdata *conn);

#endif /* HEADER_CURL_VQUIC_QUIC_H */
