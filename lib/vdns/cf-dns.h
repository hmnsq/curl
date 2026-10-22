#اگر_غیر_تعریف_شدہ سرخی_CURL_CF_DNS_H
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
#شامل "curl_setup.h"

ساخت Curl_easy;
ساخت connectdata;
ساخت Curl_dns_اندراج;
ساخت Curl_addrinfo;
ساخت Curl_peer;

CURLcode Curl_conn_dns_جوڑو_addr_resolve(ساخت Curl_easy *ڈیٹا,
                                        ساخت connectdata *conn,
                                        int8_t sockindex,
                                        ساخت Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_نتیجہ(ساخت connectdata *conn,
                                   int8_t sockindex,
                                   ساخت Curl_peer *peer);

ثابت ساخت Curl_addrinfo *Curl_conn_dns_get_ai(ساخت Curl_easy *ڈیٹا,
                                                 ساخت Curl_peer *peer,
                                                 int8_t sockindex,
                                                 صحیح ai_family,
                                                 غیرعلامتی صحیح اشاریہ);

#اگر_تعریف_شدہ USE_HTTPSRR
CURLcode Curl_conn_dns_جوڑو_https_resolve(ساخت Curl_easy *ڈیٹا,
                                         ساخت connectdata *conn,
                                         int8_t sockindex,
                                         ساخت Curl_peer *peer);

ثابت ساخت Curl_https_rrinfo *
Curl_conn_dns_get_https(ساخت Curl_easy *ڈیٹا,
                        int8_t sockindex,
                        ساخت Curl_peer *peer);
bool Curl_conn_dns_resolved_https(ساخت Curl_easy *ڈیٹا,
                                  int8_t sockindex,
                                  ساخت Curl_peer *peer);
#ورنہ
#تعریف_کرو Curl_conn_dns_get_https(a, b, c)        تہی
#تعریف_کرو Curl_conn_dns_resolved_https(a, b, c)   TRUE
#اگر_ختم

خارجی ساخت Curl_cftype Curl_cft_dns;

#اگر_ختم /* HEADER_CURL_CF_DNS_H */
