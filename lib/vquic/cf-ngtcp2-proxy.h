#अगर_अघोषित HEADER_CURL_VQUIC_CF_NGTCP2_PROXY_H
#घोषणा HEADER_CURL_VQUIC_CF_NGTCP2_PROXY_H
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

#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

CURLcode Curl_cf_ngtcp2_proxy_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                           काष्ठा Curl_easy *तथ्य,
                                           काष्ठा Curl_peer *origin,
                                           काष्ठा Curl_peer *peer,
                                           काष्ठा Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport);

CURLcode Curl_cf_ngtcp2_proxy_create(काष्ठा Curl_cfilter **pcf,
                                     काष्ठा Curl_easy *तथ्य,
                                     काष्ठा Curl_peer *origin,
                                     काष्ठा Curl_peer *peer,
                                     uint8_t transport_peer,
                                     काष्ठा connectdata *conn,
                                     काष्ठा Curl_sockaddr_ex *addr,
                                     काष्ठा Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport);

#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_VQUIC_CF_NGTCP2_PROXY_H */
