#अगर_अघोषित HEADER_CURL_CF_DNS_H
#घोषणा HEADER_CURL_CF_DNS_H
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
#समावेश "curl_setup.h"

काष्ठा Curl_easy;
काष्ठा connectdata;
काष्ठा Curl_dns_entry;
काष्ठा Curl_addrinfo;
काष्ठा Curl_peer;

CURLcode Curl_conn_dns_add_addr_resolve(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा connectdata *conn,
                                        int8_t sockindex,
                                        काष्ठा Curl_peer *peer,
                                        uint8_t dns_queries,
                                        uint8_t transport);

CURLcode Curl_conn_dns_addr_परिणाम(काष्ठा connectdata *conn,
                                   int8_t sockindex,
                                   काष्ठा Curl_peer *peer);

स्थिर काष्ठा Curl_addrinfo *Curl_conn_dns_get_ai(काष्ठा Curl_easy *तथ्य,
                                                 काष्ठा Curl_peer *peer,
                                                 int8_t sockindex,
                                                 पूर्णांक ai_family,
                                                 अचिन्हित पूर्णांक सूचकांक);

#अगर_घोषित USE_HTTPSRR
CURLcode Curl_conn_dns_add_https_resolve(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा connectdata *conn,
                                         int8_t sockindex,
                                         काष्ठा Curl_peer *peer);

स्थिर काष्ठा Curl_https_rrinfo *
Curl_conn_dns_get_https(काष्ठा Curl_easy *तथ्य,
                        int8_t sockindex,
                        काष्ठा Curl_peer *peer);
bool Curl_conn_dns_resolved_https(काष्ठा Curl_easy *तथ्य,
                                  int8_t sockindex,
                                  काष्ठा Curl_peer *peer);
#अन्यथा
#घोषणा Curl_conn_dns_get_https(a, b, c)        शून्य
#घोषणा Curl_conn_dns_resolved_https(a, b, c)   TRUE
#पूर्ण_अगर

बाह्य काष्ठा Curl_cftype Curl_cft_dns;

#पूर्ण_अगर /* HEADER_CURL_CF_DNS_H */
