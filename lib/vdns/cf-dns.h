#यदि_अपरिभाषितम् शीर्षकम्_CURL_CF_DNS_H
#परिभाषय शीर्षकम्_CURL_CF_DNS_H
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
#अन्तर्भाव "curl_setup.h"

संरचना Curl_easy;
संरचना connectdata;
संरचना Curl_dns_प्रविष्टिः;
संरचना Curl_addrinfo;
संरचना Curl_peer;

CURLcode Curl_conn_dns_सङ्कलय_addr_resolve(संरचना Curl_easy *दत्त,
                                        संरचना connectdata *conn,
                                        int8_t sockindex,
                                        संरचना Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_परिणाम(संरचना connectdata *conn,
                                   int8_t sockindex,
                                   संरचना Curl_peer *peer);

ध्रुव संरचना Curl_addrinfo *Curl_conn_dns_get_ai(संरचना Curl_easy *दत्त,
                                                 संरचना Curl_peer *peer,
                                                 int8_t sockindex,
                                                 पूर्णाङ्क ai_family,
                                                 अचिह्नित पूर्णाङ्क सूचकांक);

#यदि_परिभाषितम् USE_HTTPSRR
CURLcode Curl_conn_dns_सङ्कलय_https_resolve(संरचना Curl_easy *दत्त,
                                         संरचना connectdata *conn,
                                         int8_t sockindex,
                                         संरचना Curl_peer *peer);

ध्रुव संरचना Curl_https_rrinfo *
Curl_conn_dns_get_https(संरचना Curl_easy *दत्त,
                        int8_t sockindex,
                        संरचना Curl_peer *peer);
bool Curl_conn_dns_resolved_https(संरचना Curl_easy *दत्त,
                                  int8_t sockindex,
                                  संरचना Curl_peer *peer);
#अन्यथा
#परिभाषय Curl_conn_dns_get_https(a, b, c)        रिक्त
#परिभाषय Curl_conn_dns_resolved_https(a, b, c)   TRUE
#यदि_अन्तः

बाह्य संरचना Curl_cftype Curl_cft_dns;

#यदि_अन्तः /* HEADER_CURL_CF_DNS_H */
