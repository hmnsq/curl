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
#شامل "curl_setup.h"

ساختار Curl_easy;
ساختار connectdata;
ساختار Curl_dns_entry;
ساختار Curl_addrinfo;
ساختار Curl_peer;

CURLcode Curl_conn_dns_add_addr_resolve(ساختار Curl_easy *data,
                                        ساختار connectdata *conn,
                                        int8_t sockindex,
                                        ساختار Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_result(ساختار connectdata *conn,
                                   int8_t sockindex,
                                   ساختار Curl_peer *peer);

ثابت ساختار Curl_addrinfo *Curl_conn_dns_get_ai(ساختار Curl_easy *data,
                                                 ساختار Curl_peer *peer,
                                                 int8_t sockindex,
                                                 صحیح ai_family,
                                                 unsigned صحیح index);

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(ساختار Curl_easy *data,
                                         ساختار connectdata *conn,
                                         int8_t sockindex,
                                         ساختار Curl_peer *peer);

ثابت ساختار Curl_https_rrinfo *
Curl_conn_dns_get_https(ساختار Curl_easy *data,
                        int8_t sockindex,
                        ساختار Curl_peer *peer);
bool Curl_conn_dns_resolved_https(ساختار Curl_easy *data,
                                  int8_t sockindex,
                                  ساختار Curl_peer *peer);
#وگرنه
#define Curl_conn_dns_get_https(a, b, c)        پوچ
#define Curl_conn_dns_resolved_https(a, b, c)   TRUE
#endif

بیرونی ساختار Curl_cftype Curl_cft_dns;

#endif /* HEADER_CURL_CF_DNS_H */
