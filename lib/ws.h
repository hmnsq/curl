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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)

ବାହ୍ୟ ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_ws;

/* meta key for storing protocol meta at connection */
#define CURL_META_PROTO_WS_CONN   "meta:proto:ws:conn"

CURLcode Curl_ws_request(ସଂରଚନା Curl_easy *data, ସଂରଚନା dynbuf *req);
CURLcode Curl_ws_accept(ସଂରଚନା Curl_easy *data,
                        ଅଚଳ ଅକ୍ଷର *mem, size_t nread);

#ଅନ୍ୟଥା
#define Curl_ws_request(x, y) CURLE_OK
#define Curl_ws_free(x)       Curl_nop_stmt
#endif

#endif /* HEADER_CURL_WS_H */
