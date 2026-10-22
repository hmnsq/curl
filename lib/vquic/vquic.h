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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
સંરચના Curl_cfilter;
સંરચના Curl_easy;
સંરચના connectdata;
સંરચના Curl_addrinfo;

શૂન્ય Curl_quic_ver(અક્ષર *p, size_t len);
પૂર્ણાંક Curl_vquic_init(શૂન્ય);
શૂન્ય Curl_vquic_cleanup(શૂન્ય);

CURLcode Curl_qlogdir(સંરચના Curl_easy *data,
                      unsigned અક્ષર *scid,
                      size_t scidlen,
                      પૂર્ણાંક *qlogfdp);

CURLcode Curl_cf_quic_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                   સંરચના Curl_easy *data,
                                   સંરચના Curl_peer *origin,
                                   સંરચના Curl_peer *peer);

CURLcode Curl_cf_quic_create(સંરચના Curl_cfilter **pcf,
                             સંરચના Curl_easy *data,
                             સંરચના Curl_peer *origin,
                             સંરચના Curl_peer *peer,
                             uint8_t transport_peer,
                             સંરચના connectdata *conn,
                             સંરચના Curl_sockaddr_ex *addr,
                             સંરચના Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

બાહ્ય સંરચના Curl_cftype Curl_cft_http3;

#જો !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                       સંરચના Curl_easy *data,
                                       સંરચના Curl_peer *origin,
                                       સંરચના Curl_peer *peer,
                                       સંરચના Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(સંરચના Curl_cfilter **pcf,
                                 સંરચના Curl_easy *data,
                                 સંરચના Curl_peer *origin,
                                 સંરચના Curl_peer *peer,
                                 uint8_t transport_peer,
                                 સંરચના connectdata *conn,
                                 સંરચના Curl_sockaddr_ex *addr,
                                 સંરચના Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

બાહ્ય સંરચના Curl_cftype Curl_cft_h3_proxy;

#endif /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#નહીંતર
#define Curl_vquic_init() 1
#define Curl_vquic_cleanup()
#endif /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(સંરચના Curl_easy *data,
                             સંરચના connectdata *conn);

#endif /* HEADER_CURL_VQUIC_QUIC_H */
