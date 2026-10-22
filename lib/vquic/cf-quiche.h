#अगर_अघोषित HEADER_CURL_VQUIC_CF_QUICHE_H
#घोषणा HEADER_CURL_VQUIC_CF_QUICHE_H
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

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)

#समावेश <quiche.h>
#समावेश <openssl/ssl.h>

काष्ठा Curl_cfilter;
काष्ठा Curl_easy;

व्योम Curl_quiche_ver(अक्षर *p, माप_प्रकार len);

CURLcode Curl_cf_quiche_create(काष्ठा Curl_cfilter **pcf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_peer *origin,
                               काष्ठा Curl_peer *peer,
                               काष्ठा connectdata *conn,
                               काष्ठा Curl_sockaddr_ex *addr);
CURLcode Curl_cf_quiche_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                     काष्ठा Curl_peer *origin,
                                     काष्ठा Curl_peer *peer,
                                     काष्ठा Curl_cfilter **pcf);

#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_VQUIC_CF_QUICHE_H */
