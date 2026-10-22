#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_VQUIC_CF_NGTCP2_PROXY_H
#وصف_ڪريو سرو_CURL_VQUIC_CF_NGTCP2_PROXY_H
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

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

CURLcode Curl_cf_ngtcp2_proxy_وجهو_after(بنية Curl_cfilter *cf_at,
                                           بنية Curl_easy *معلومات,
                                           بنية Curl_peer *origin,
                                           بنية Curl_peer *peer,
                                           بنية Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport);

CURLcode Curl_cf_ngtcp2_proxy_create(بنية Curl_cfilter **pcf,
                                     بنية Curl_easy *معلومات,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     uint8_t transport_peer,
                                     بنية connectdata *conn,
                                     بنية Curl_sockaddr_ex *addr,
                                     بنية Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport);

#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_VQUIC_CF_NGTCP2_PROXY_H */
