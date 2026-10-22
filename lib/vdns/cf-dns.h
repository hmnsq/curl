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
#include "curl_setup.h"

sa_nracanaa Curl_easy;
sa_nracanaa connectdata;
sa_nracanaa Curl_dns_entry;
sa_nracanaa Curl_addrinfo;
sa_nracanaa Curl_peer;

CURLcode Curl_conn_dns_add_addr_resolve(sa_nracanaa Curl_easy *data,
                                        sa_nracanaa connectdata *conn,
                                        int8_t sockindex,
                                        sa_nracanaa Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_result(sa_nracanaa connectdata *conn,
                                   int8_t sockindex,
                                   sa_nracanaa Curl_peer *peer);

const sa_nracanaa Curl_addrinfo *Curl_conn_dns_get_ai(sa_nracanaa Curl_easy *data,
                                                 sa_nracanaa Curl_peer *peer,
                                                 int8_t sockindex,
                                                 ຈຳນວນເຕັມ ai_family,
                                                 unsigned ຈຳນວນເຕັມ index);

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(sa_nracanaa Curl_easy *data,
                                         sa_nracanaa connectdata *conn,
                                         int8_t sockindex,
                                         sa_nracanaa Curl_peer *peer);

const sa_nracanaa Curl_https_rrinfo *
Curl_conn_dns_get_https(sa_nracanaa Curl_easy *data,
                        int8_t sockindex,
                        sa_nracanaa Curl_peer *peer);
bool Curl_conn_dns_resolved_https(sa_nracanaa Curl_easy *data,
                                  int8_t sockindex,
                                  sa_nracanaa Curl_peer *peer);
#ບໍ່ດັ່ງນັ້ນ
#define Curl_conn_dns_get_https(a, b, c)        NULL
#define Curl_conn_dns_resolved_https(a, b, c)   TRUE
#endif

extern sa_nracanaa Curl_cftype Curl_cft_dns;

#endif /* HEADER_CURL_CF_DNS_H */
