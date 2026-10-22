#నిర్వచితం_కాకపోతే శీర్షిక_CURL_CF_DNS_H
#నిర్వచించు శీర్షిక_CURL_CF_DNS_H
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

నిర్మాణం Curl_easy;
నిర్మాణం connectdata;
నిర్మాణం Curl_dns_నమోదు;
నిర్మాణం Curl_addrinfo;
నిర్మాణం Curl_peer;

CURLcode Curl_conn_dns_కూడు_addr_resolve(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం connectdata *conn,
                                        int8_t sockindex,
                                        నిర్మాణం Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_ఫలితం(నిర్మాణం connectdata *conn,
                                   int8_t sockindex,
                                   నిర్మాణం Curl_peer *peer);

స్థిరరాశి నిర్మాణం Curl_addrinfo *Curl_conn_dns_get_ai(నిర్మాణం Curl_easy *దత్తాంశం,
                                                 నిర్మాణం Curl_peer *peer,
                                                 int8_t sockindex,
                                                 పూర్ణాంకం ai_family,
                                                 గుర్తులేని పూర్ణాంకం సూచీ);

#నిర్వచితమైతే USE_HTTPSRR
CURLcode Curl_conn_dns_కూడు_https_resolve(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం connectdata *conn,
                                         int8_t sockindex,
                                         నిర్మాణం Curl_peer *peer);

స్థిరరాశి నిర్మాణం Curl_https_rrinfo *
Curl_conn_dns_get_https(నిర్మాణం Curl_easy *దత్తాంశం,
                        int8_t sockindex,
                        నిర్మాణం Curl_peer *peer);
bool Curl_conn_dns_resolved_https(నిర్మాణం Curl_easy *దత్తాంశం,
                                  int8_t sockindex,
                                  నిర్మాణం Curl_peer *peer);
#లేకపోతే
#నిర్వచించు Curl_conn_dns_get_https(a, b, c)        ఖాళీ
#నిర్వచించు Curl_conn_dns_resolved_https(a, b, c)   TRUE
#అయితే_ముగింపు

బాహ్య నిర్మాణం Curl_cftype Curl_cft_dns;

#అయితే_ముగింపు /* HEADER_CURL_CF_DNS_H */
