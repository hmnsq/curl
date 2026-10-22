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
#অন্তর্ভাব "curl_setup.h"

কাঠামো Curl_easy;
কাঠামো connectdata;
কাঠামো Curl_dns_entry;
কাঠামো Curl_addrinfo;
কাঠামো Curl_peer;

CURLcode Curl_conn_dns_add_addr_resolve(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো connectdata *conn,
                                        int8_t sockindex,
                                        কাঠামো Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_ফলাফল(কাঠামো connectdata *conn,
                                   int8_t sockindex,
                                   কাঠামো Curl_peer *peer);

ধ্রুবক কাঠামো Curl_addrinfo *Curl_conn_dns_get_ai(কাঠামো Curl_easy *তথ্য,
                                                 কাঠামো Curl_peer *peer,
                                                 int8_t sockindex,
                                                 পূর্ণসংখ্যা ai_family,
                                                 unsigned পূর্ণসংখ্যা সূচক);

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো connectdata *conn,
                                         int8_t sockindex,
                                         কাঠামো Curl_peer *peer);

ধ্রুবক কাঠামো Curl_https_rrinfo *
Curl_conn_dns_get_https(কাঠামো Curl_easy *তথ্য,
                        int8_t sockindex,
                        কাঠামো Curl_peer *peer);
bool Curl_conn_dns_resolved_https(কাঠামো Curl_easy *তথ্য,
                                  int8_t sockindex,
                                  কাঠামো Curl_peer *peer);
#অন্যথা
#define Curl_conn_dns_get_https(a, b, c)        ফাঁকা
#define Curl_conn_dns_resolved_https(a, b, c)   TRUE
#endif

বাহ্যিক কাঠামো Curl_cftype Curl_cft_dns;

#endif /* HEADER_CURL_CF_DNS_H */
