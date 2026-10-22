#ifndef HEADER_CURL_VQUIC_CF_NGTCP2_PROXY_H
#define HEADER_CURL_VQUIC_CF_NGTCP2_PROXY_H
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

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

CURLcode Curl_cf_ngtcp2_proxy_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                           সংরচনা Curl_easy *data,
                                           সংরচনা Curl_peer *origin,
                                           সংরচনা Curl_peer *peer,
                                           সংরচনা Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport);

CURLcode Curl_cf_ngtcp2_proxy_create(সংরচনা Curl_cfilter **pcf,
                                     সংরচনা Curl_easy *data,
                                     সংরচনা Curl_peer *origin,
                                     সংরচনা Curl_peer *peer,
                                     uint8_t transport_peer,
                                     সংরচনা connectdata *conn,
                                     সংরচনা Curl_sockaddr_ex *addr,
                                     সংরচনা Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport);

#endif

#endif /* HEADER_CURL_VQUIC_CF_NGTCP2_PROXY_H */
