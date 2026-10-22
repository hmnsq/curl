#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_VQUIC_CF_NGTCP2_PROXY_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_VQUIC_CF_NGTCP2_PROXY_H
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

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

CURLcode Curl_cf_ngtcp2_proxy_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ Curl_peer *origin,
                                           ਬਣਤਰ Curl_peer *peer,
                                           ਬਣਤਰ Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport);

CURLcode Curl_cf_ngtcp2_proxy_create(ਬਣਤਰ Curl_cfilter **pcf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ Curl_peer *origin,
                                     ਬਣਤਰ Curl_peer *peer,
                                     uint8_t transport_peer,
                                     ਬਣਤਰ connectdata *conn,
                                     ਬਣਤਰ Curl_sockaddr_ex *addr,
                                     ਬਣਤਰ Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport);

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_VQUIC_CF_NGTCP2_PROXY_H */
