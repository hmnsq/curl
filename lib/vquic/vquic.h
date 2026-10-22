#నిర్వచితం_కాకపోతే శీర్షిక_CURL_VQUIC_QUIC_H
#నిర్వచించు శీర్షిక_CURL_VQUIC_QUIC_H
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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
నిర్మాణం Curl_cfilter;
నిర్మాణం Curl_easy;
నిర్మాణం connectdata;
నిర్మాణం Curl_addrinfo;

శూన్య Curl_quic_ver(అక్షరం *p, size_t len);
పూర్ణాంకం Curl_vquic_init(శూన్య);
శూన్య Curl_vquic_cleanup(శూన్య);

CURLcode Curl_qlogdir(నిర్మాణం Curl_easy *దత్తాంశం,
                      గుర్తులేని అక్షరం *scid,
                      size_t scidlen,
                      పూర్ణాంకం *qlogfdp);

CURLcode Curl_cf_quic_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం Curl_peer *origin,
                                   నిర్మాణం Curl_peer *peer);

CURLcode Curl_cf_quic_create(నిర్మాణం Curl_cfilter **pcf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_peer *origin,
                             నిర్మాణం Curl_peer *peer,
                             uint8_t transport_peer,
                             నిర్మాణం connectdata *conn,
                             నిర్మాణం Curl_sockaddr_ex *addr,
                             నిర్మాణం Curl_peer *tunnel_peer,
                             uint8_t tunnel_transport);

బాహ్య నిర్మాణం Curl_cftype Curl_cft_http3;

#ఒకవేళ !defined(CURL_DISABLE_PROXY) && defined(USE_PROXY_HTTP3)

CURLcode Curl_cf_h3_proxy_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం Curl_peer *origin,
                                       నిర్మాణం Curl_peer *peer,
                                       నిర్మాణం Curl_peer *tunnel_peer,
                                       uint8_t tunnel_transport);

CURLcode Curl_cf_h3_proxy_create(నిర్మాణం Curl_cfilter **pcf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం Curl_peer *origin,
                                 నిర్మాణం Curl_peer *peer,
                                 uint8_t transport_peer,
                                 నిర్మాణం connectdata *conn,
                                 నిర్మాణం Curl_sockaddr_ex *addr,
                                 నిర్మాణం Curl_peer *tunnel_peer,
                                 uint8_t tunnel_transport);

బాహ్య నిర్మాణం Curl_cftype Curl_cft_h3_proxy;

#అయితే_ముగింపు /* !CURL_DISABLE_PROXY && USE_PROXY_HTTP3 */

#లేకపోతే
#నిర్వచించు Curl_vquic_init() 1
#నిర్వచించు Curl_vquic_cleanup()
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && USE_HTTP3 */

CURLcode Curl_conn_may_http3(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం connectdata *conn);

#అయితే_ముగింపు /* HEADER_CURL_VQUIC_QUIC_H */
