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
#അന്തര്ഭാവ "curl_setup.h"

ഘടന Curl_easy;
ഘടന connectdata;
ഘടന Curl_dns_entry;
ഘടന Curl_addrinfo;
ഘടന Curl_peer;

CURLcode Curl_conn_dns_add_addr_resolve(ഘടന Curl_easy *data,
                                        ഘടന connectdata *conn,
                                        int8_t sockindex,
                                        ഘടന Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_result(ഘടന connectdata *conn,
                                   int8_t sockindex,
                                   ഘടന Curl_peer *peer);

സ്ഥിരം ഘടന Curl_addrinfo *Curl_conn_dns_get_ai(ഘടന Curl_easy *data,
                                                 ഘടന Curl_peer *peer,
                                                 int8_t sockindex,
                                                 പൂർണ്ണസംഖ്യ ai_family,
                                                 unsigned പൂർണ്ണസംഖ്യ index);

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(ഘടന Curl_easy *data,
                                         ഘടന connectdata *conn,
                                         int8_t sockindex,
                                         ഘടന Curl_peer *peer);

സ്ഥിരം ഘടന Curl_https_rrinfo *
Curl_conn_dns_get_https(ഘടന Curl_easy *data,
                        int8_t sockindex,
                        ഘടന Curl_peer *peer);
bool Curl_conn_dns_resolved_https(ഘടന Curl_easy *data,
                                  int8_t sockindex,
                                  ഘടന Curl_peer *peer);
#അല്ലെങ്കിൽ
#define Curl_conn_dns_get_https(a, b, c)        ശൂന്യ
#define Curl_conn_dns_resolved_https(a, b, c)   TRUE
#endif

ബാഹ്യ ഘടന Curl_cftype Curl_cft_dns;

#endif /* HEADER_CURL_CF_DNS_H */
