#ifndef HEADER_CURL_WS_H
#define HEADER_CURL_WS_H
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

#എങ്കിൽ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)

ബാഹ്യ സ്ഥിരം ഘടന Curl_protocol Curl_protocol_ws;

/* meta key for storing protocol meta at connection */
#define CURL_META_PROTO_WS_CONN   "meta:proto:ws:conn"

CURLcode Curl_ws_request(ഘടന Curl_easy *data, ഘടന dynbuf *req);
CURLcode Curl_ws_accept(ഘടന Curl_easy *data,
                        സ്ഥിരം അക്ഷരം *mem, size_t nread);

#അല്ലെങ്കിൽ
#define Curl_ws_request(x, y) CURLE_OK
#define Curl_ws_free(x)       Curl_nop_stmt
#endif

#endif /* HEADER_CURL_WS_H */
