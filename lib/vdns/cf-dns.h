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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

ಸಂರಚನಾ Curl_easy;
ಸಂರಚನಾ connectdata;
ಸಂರಚನಾ Curl_dns_entry;
ಸಂರಚನಾ Curl_addrinfo;
ಸಂರಚನಾ Curl_peer;

CURLcode Curl_conn_dns_add_addr_resolve(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ connectdata *conn,
                                        int8_t sockindex,
                                        ಸಂರಚನಾ Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_result(ಸಂರಚನಾ connectdata *conn,
                                   int8_t sockindex,
                                   ಸಂರಚನಾ Curl_peer *peer);

ಧ್ರುವ ಸಂರಚನಾ Curl_addrinfo *Curl_conn_dns_get_ai(ಸಂರಚನಾ Curl_easy *data,
                                                 ಸಂರಚನಾ Curl_peer *peer,
                                                 int8_t sockindex,
                                                 ಪೂರ್ಣಾಙ್ಕ ai_family,
                                                 unsigned ಪೂರ್ಣಾಙ್ಕ index);

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ connectdata *conn,
                                         int8_t sockindex,
                                         ಸಂರಚನಾ Curl_peer *peer);

ಧ್ರುವ ಸಂರಚನಾ Curl_https_rrinfo *
Curl_conn_dns_get_https(ಸಂರಚನಾ Curl_easy *data,
                        int8_t sockindex,
                        ಸಂರಚನಾ Curl_peer *peer);
bool Curl_conn_dns_resolved_https(ಸಂರಚನಾ Curl_easy *data,
                                  int8_t sockindex,
                                  ಸಂರಚನಾ Curl_peer *peer);
#ಅನ್ಯಥಾ
#define Curl_conn_dns_get_https(a, b, c)        ರಿಕ್ತ
#define Curl_conn_dns_resolved_https(a, b, c)   TRUE
#endif

ಬಾಹ್ಯ ಸಂರಚನಾ Curl_cftype Curl_cft_dns;

#endif /* HEADER_CURL_CF_DNS_H */
