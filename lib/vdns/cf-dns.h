#ifndef HEADER_CURL_CF_DNS_H
#define HEADER_CURL_CF_DNS_H
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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

ସଂରଚନା Curl_easy;
ସଂରଚନା connectdata;
ସଂରଚନା Curl_dns_entry;
ସଂରଚନା Curl_addrinfo;
ସଂରଚନା Curl_peer;

CURLcode Curl_conn_dns_add_addr_resolve(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା connectdata *conn,
                                        int8_t sockindex,
                                        ସଂରଚନା Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_result(ସଂରଚନା connectdata *conn,
                                   int8_t sockindex,
                                   ସଂରଚନା Curl_peer *peer);

ଅଚଳ ସଂରଚନା Curl_addrinfo *Curl_conn_dns_get_ai(ସଂରଚନା Curl_easy *data,
                                                 ସଂରଚନା Curl_peer *peer,
                                                 int8_t sockindex,
                                                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା ai_family,
                                                 unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା index);

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା connectdata *conn,
                                         int8_t sockindex,
                                         ସଂରଚନା Curl_peer *peer);

ଅଚଳ ସଂରଚନା Curl_https_rrinfo *
Curl_conn_dns_get_https(ସଂରଚନା Curl_easy *data,
                        int8_t sockindex,
                        ସଂରଚନା Curl_peer *peer);
bool Curl_conn_dns_resolved_https(ସଂରଚନା Curl_easy *data,
                                  int8_t sockindex,
                                  ସଂରଚନା Curl_peer *peer);
#ଅନ୍ୟଥା
#define Curl_conn_dns_get_https(a, b, c)        ଖାଲି
#define Curl_conn_dns_resolved_https(a, b, c)   TRUE
#endif

ବାହ୍ୟ ସଂରଚନା Curl_cftype Curl_cft_dns;

#endif /* HEADER_CURL_CF_DNS_H */
