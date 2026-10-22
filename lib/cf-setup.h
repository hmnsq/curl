#अगर_अघोषित HEADER_CURL_CF_SETUP_H
#घोषणा HEADER_CURL_CF_SETUP_H
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

काष्ठा Curl_dns_entry;
काष्ठा ip_quadruple;
काष्ठा Curl_peer;
काष्ठा Curl_str;

CURLcode Curl_cf_setup_add(काष्ठा Curl_easy *तथ्य,
                           काष्ठा connectdata *conn,
                           int8_t sockindex,
                           uint8_t transport,
                           पूर्णांक ssl_mode);

CURLcode Curl_cf_setup_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                    काष्ठा Curl_easy *तथ्य,
                                    uint8_t transport,
                                    पूर्णांक ssl_mode);

बाह्य काष्ठा Curl_cftype Curl_cft_setup;

#पूर्ण_अगर /* HEADER_CURL_CF_SETUP_H */
