#اگر_تعریف_نشده سرآیند_CURL_WS_H
#تعریف_کن سرآیند_CURL_WS_H
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

#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)

خارجي ثابت بنية Curl_protocol Curl_protocol_ws;

/* meta key for storing protocol meta at connection */
#تعریف_کن CURL_META_PROTO_WS_CONN   "meta:proto:ws:conn"

CURLcode Curl_ws_request(بنية Curl_easy *داده, بنية dynbuf *req);
CURLcode Curl_ws_accept(بنية Curl_easy *داده,
                        ثابت حرف *mem, size_t nread);

#وإلا
#تعریف_کن Curl_ws_request(x, y) CURLE_OK
#تعریف_کن Curl_ws_free(x)       Curl_nop_stmt
#پایان_اگر

#پایان_اگر /* HEADER_CURL_WS_H */
