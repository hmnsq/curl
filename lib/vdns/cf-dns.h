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
#அந்தர்பாவ "curl_setup.h"

கட்டமைப்பு Curl_easy;
கட்டமைப்பு connectdata;
கட்டமைப்பு Curl_dns_entry;
கட்டமைப்பு Curl_addrinfo;
கட்டமைப்பு Curl_peer;

CURLcode Curl_conn_dns_add_addr_resolve(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு connectdata *conn,
                                        int8_t sockindex,
                                        கட்டமைப்பு Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_result(கட்டமைப்பு connectdata *conn,
                                   int8_t sockindex,
                                   கட்டமைப்பு Curl_peer *peer);

மாறிலி கட்டமைப்பு Curl_addrinfo *Curl_conn_dns_get_ai(கட்டமைப்பு Curl_easy *data,
                                                 கட்டமைப்பு Curl_peer *peer,
                                                 int8_t sockindex,
                                                 முழு ai_family,
                                                 unsigned முழு index);

#ifdef USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு connectdata *conn,
                                         int8_t sockindex,
                                         கட்டமைப்பு Curl_peer *peer);

மாறிலி கட்டமைப்பு Curl_https_rrinfo *
Curl_conn_dns_get_https(கட்டமைப்பு Curl_easy *data,
                        int8_t sockindex,
                        கட்டமைப்பு Curl_peer *peer);
bool Curl_conn_dns_resolved_https(கட்டமைப்பு Curl_easy *data,
                                  int8_t sockindex,
                                  கட்டமைப்பு Curl_peer *peer);
#இல்லையெனில்
#define Curl_conn_dns_get_https(a, b, c)        வெற்று
#define Curl_conn_dns_resolved_https(a, b, c)   TRUE
#endif

வெளி கட்டமைப்பு Curl_cftype Curl_cft_dns;

#endif /* HEADER_CURL_CF_DNS_H */
