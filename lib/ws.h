#यदि_अपरिभाषितम् शीर्षकम्_CURL_WS_H
#परिभाषय शीर्षकम्_CURL_WS_H
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

#यदि !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)

बाह्य ध्रुव संरचना Curl_protocol Curl_protocol_ws;

/* meta key for storing protocol meta at connection */
#परिभाषय CURL_META_PROTO_WS_CONN   "meta:proto:ws:conn"

CURLcode Curl_ws_request(संरचना Curl_easy *दत्त, संरचना dynbuf *req);
CURLcode Curl_ws_accept(संरचना Curl_easy *दत्त,
                        ध्रुव वर्ण *mem, size_t nread);

#अन्यथा
#परिभाषय Curl_ws_request(x, y) CURLE_OK
#परिभाषय Curl_ws_free(x)       Curl_nop_stmt
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_WS_H */
