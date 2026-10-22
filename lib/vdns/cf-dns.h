#جے_بنا_تعریف سرخی_CURL_CF_DNS_H
#تعریف_کرو سرخی_CURL_CF_DNS_H
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
#ضمن "curl_setup.h"

بنية Curl_easy;
بنية connectdata;
بنية Curl_dns_اندراج;
بنية Curl_addrinfo;
بنية Curl_peer;

CURLcode Curl_conn_dns_جوڑو_addr_resolve(بنية Curl_easy *انکڑے,
                                        بنية connectdata *conn,
                                        int8_t sockindex,
                                        بنية Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_نتیجہ(بنية connectdata *conn,
                                   int8_t sockindex,
                                   بنية Curl_peer *peer);

ثابت بنية Curl_addrinfo *Curl_conn_dns_get_ai(بنية Curl_easy *انکڑے,
                                                 بنية Curl_peer *peer,
                                                 int8_t sockindex,
                                                 صحيح ai_family,
                                                 بنانشانی صحيح اشاریہ);

#جے_تعریف_شدہ USE_HTTPSRR
CURLcode Curl_conn_dns_جوڑو_https_resolve(بنية Curl_easy *انکڑے,
                                         بنية connectdata *conn,
                                         int8_t sockindex,
                                         بنية Curl_peer *peer);

ثابت بنية Curl_https_rrinfo *
Curl_conn_dns_get_https(بنية Curl_easy *انکڑے,
                        int8_t sockindex,
                        بنية Curl_peer *peer);
bool Curl_conn_dns_resolved_https(بنية Curl_easy *انکڑے,
                                  int8_t sockindex,
                                  بنية Curl_peer *peer);
#وإلا
#تعریف_کرو Curl_conn_dns_get_https(a, b, c)        عدم
#تعریف_کرو Curl_conn_dns_resolved_https(a, b, c)   TRUE
#جے_انت

خارجي بنية Curl_cftype Curl_cft_dns;

#جے_انت /* HEADER_CURL_CF_DNS_H */
