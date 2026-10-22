#अगर_अघोषित HEADER_CURL_VQUIC_QUIC_H
#घोषणा HEADER_CURL_VQUIC_QUIC_H
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

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
काष्ठा Curl_cfilter;
काष्ठा Curl_easy;
काष्ठा connectdata;
काष्ठा Curl_addrinfo;

व्योम Curl_quic_ver(अक्षर *p, माप_प्रकार len);
पूर्णांक Curl_vquic_init(व्योम);
व्योम Curl_vquic_cleanup(व्योम);

CURLcode Curl_qlogdir(काष्ठा Curl_easy *तथ्य,
                      अचिन्हित अक्षर *scid,
                      माप_प्रकार scidlen,
                      पूर्णांक *qlogfdp);

CURLcode Curl_cf_quic_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा Curl_peer *origin,
                                   काष्ठा Curl_peer *peer);

CURLcode Curl_cf_quic_create(काष्ठा Curl_cfilter **pcf,
                             काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_peer *origin,
                             काष्ठा Curl_peer *peer,
                             uint8_t transport_peer,
                             काष्ठा connectdata *conn,
                             काष्ठा Curl_sockaddr_ex *addr,
                             काष्ठा Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

बाह्य काष्ठा Curl_cftype Curl_cft_http3;

#अगर !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                       काष्ठा Curl_easy *तथ्य,
                                       काष्ठा Curl_peer *origin,
                                       काष्ठा Curl_peer *peer,
                                       काष्ठा Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(काष्ठा Curl_cfilter **pcf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा Curl_peer *origin,
                                 काष्ठा Curl_peer *peer,
                                 uint8_t transport_peer,
                                 काष्ठा connectdata *conn,
                                 काष्ठा Curl_sockaddr_ex *addr,
                                 काष्ठा Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

बाह्य काष्ठा Curl_cftype Curl_cft_h3_proxy;

#पूर्ण_अगर /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#अन्यथा
#घोषणा Curl_vquic_init() 1
#घोषणा Curl_vquic_cleanup()
#पूर्ण_अगर /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(काष्ठा Curl_easy *तथ्य,
                             काष्ठा connectdata *conn);

#पूर्ण_अगर /* HEADER_CURL_VQUIC_QUIC_H */
