#अगर_अघोषित HEADER_CURL_WS_H
#घोषणा HEADER_CURL_WS_H
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

#अगर !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)

बाह्य स्थिर काष्ठा Curl_protocol Curl_protocol_ws;

/* meta key for storing protocol meta at connection */
#घोषणा CURL_META_PROTO_WS_CONN   "meta:proto:ws:conn"

CURLcode Curl_ws_request(काष्ठा Curl_easy *तथ्य, काष्ठा dynbuf *req);
CURLcode Curl_ws_accept(काष्ठा Curl_easy *तथ्य,
                        स्थिर अक्षर *mem, माप_प्रकार nread);

#अन्यथा
#घोषणा Curl_ws_request(x, y) CURLE_OK
#घोषणा Curl_ws_free(x)       Curl_nop_stmt
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_WS_H */
