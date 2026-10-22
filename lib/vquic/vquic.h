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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
ಸಂರಚನಾ Curl_cfilter;
ಸಂರಚನಾ Curl_easy;
ಸಂರಚನಾ connectdata;
ಸಂರಚನಾ Curl_addrinfo;

ಶೂನ್ಯ Curl_quic_ver(ವರ್ಣ *p, size_t len);
ಪೂರ್ಣಾಙ್ಕ Curl_vquic_init(ಶೂನ್ಯ);
ಶೂನ್ಯ Curl_vquic_cleanup(ಶೂನ್ಯ);

CURLcode Curl_qlogdir(ಸಂರಚನಾ Curl_easy *data,
                      unsigned ವರ್ಣ *scid,
                      size_t scidlen,
                      ಪೂರ್ಣಾಙ್ಕ *qlogfdp);

CURLcode Curl_cf_quic_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ Curl_peer *origin,
                                   ಸಂರಚನಾ Curl_peer *peer);

CURLcode Curl_cf_quic_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_peer *origin,
                             ಸಂರಚನಾ Curl_peer *peer,
                             uint8_t transport_peer,
                             ಸಂರಚನಾ connectdata *conn,
                             ಸಂರಚನಾ Curl_sockaddr_ex *addr,
                             ಸಂರಚನಾ Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

ಬಾಹ್ಯ ಸಂರಚನಾ Curl_cftype Curl_cft_http3;

#ಯದಿ !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ Curl_peer *origin,
                                       ಸಂರಚನಾ Curl_peer *peer,
                                       ಸಂರಚನಾ Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ Curl_peer *origin,
                                 ಸಂರಚನಾ Curl_peer *peer,
                                 uint8_t transport_peer,
                                 ಸಂರಚನಾ connectdata *conn,
                                 ಸಂರಚನಾ Curl_sockaddr_ex *addr,
                                 ಸಂರಚನಾ Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

ಬಾಹ್ಯ ಸಂರಚನಾ Curl_cftype Curl_cft_h3_proxy;

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#ಅನ್ಯಥಾ
#define Curl_vquic_init() 1
#define Curl_vquic_cleanup()
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ connectdata *conn);

#endif /* HEADER_CURL_VQUIC_QUIC_H */
