#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_CF_DNS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_CF_DNS_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

ਬਣਤਰ Curl_easy;
ਬਣਤਰ connectdata;
ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ;
ਬਣਤਰ Curl_addrinfo;
ਬਣਤਰ Curl_peer;

CURLcode Curl_conn_dns_ਜੋੜੋ_addr_resolve(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ connectdata *conn,
                                        int8_t sockindex,
                                        ਬਣਤਰ Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_ਨਤੀਜਾ(ਬਣਤਰ connectdata *conn,
                                   int8_t sockindex,
                                   ਬਣਤਰ Curl_peer *peer);

ਅਚਲ ਬਣਤਰ Curl_addrinfo *Curl_conn_dns_get_ai(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                 ਬਣਤਰ Curl_peer *peer,
                                                 int8_t sockindex,
                                                 ਪੂਰਨ ai_family,
                                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੂਚਕਾਂਕ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
CURLcode Curl_conn_dns_ਜੋੜੋ_https_resolve(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ connectdata *conn,
                                         int8_t sockindex,
                                         ਬਣਤਰ Curl_peer *peer);

ਅਚਲ ਬਣਤਰ Curl_https_rrinfo *
Curl_conn_dns_get_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        int8_t sockindex,
                        ਬਣਤਰ Curl_peer *peer);
bool Curl_conn_dns_resolved_https(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  int8_t sockindex,
                                  ਬਣਤਰ Curl_peer *peer);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_conn_dns_get_https(a, b, c)        ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_conn_dns_resolved_https(a, b, c)   TRUE
#ਜੇ_ਅੰਤ

ਬਾਹਰੀ ਬਣਤਰ Curl_cftype Curl_cft_dns;

#ਜੇ_ਅੰਤ /* HEADER_CURL_CF_DNS_H */
