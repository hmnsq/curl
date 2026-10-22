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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
কাঠামো Curl_cfilter;
কাঠামো Curl_easy;
কাঠামো connectdata;
কাঠামো Curl_addrinfo;

শূন্য Curl_quic_ver(অক্ষর *p, size_t len);
পূর্ণসংখ্যা Curl_vquic_init(শূন্য);
শূন্য Curl_vquic_cleanup(শূন্য);

CURLcode Curl_qlogdir(কাঠামো Curl_easy *তথ্য,
                      unsigned অক্ষর *scid,
                      size_t scidlen,
                      পূর্ণসংখ্যা *qlogfdp);

CURLcode Curl_cf_quic_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো Curl_peer *origin,
                                   কাঠামো Curl_peer *peer);

CURLcode Curl_cf_quic_create(কাঠামো Curl_cfilter **pcf,
                             কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_peer *origin,
                             কাঠামো Curl_peer *peer,
                             uint8_t transport_peer,
                             কাঠামো connectdata *conn,
                             কাঠামো Curl_sockaddr_ex *addr,
                             কাঠামো Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

বাহ্যিক কাঠামো Curl_cftype Curl_cft_http3;

#যদি !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                       কাঠামো Curl_easy *তথ্য,
                                       কাঠামো Curl_peer *origin,
                                       কাঠামো Curl_peer *peer,
                                       কাঠামো Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(কাঠামো Curl_cfilter **pcf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো Curl_peer *origin,
                                 কাঠামো Curl_peer *peer,
                                 uint8_t transport_peer,
                                 কাঠামো connectdata *conn,
                                 কাঠামো Curl_sockaddr_ex *addr,
                                 কাঠামো Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

বাহ্যিক কাঠামো Curl_cftype Curl_cft_h3_proxy;

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#অন্যথা
#define Curl_vquic_init() 1
#define Curl_vquic_cleanup()
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(কাঠামো Curl_easy *তথ্য,
                             কাঠামো connectdata *conn);

#endif /* HEADER_CURL_VQUIC_QUIC_H */
