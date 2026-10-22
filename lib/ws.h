#నిర్వచితం_కాకపోతే శీర్షిక_CURL_WS_H
#నిర్వచించు శీర్షిక_CURL_WS_H
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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)

బాహ్య స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_ws;

/* meta key for storing protocol meta at connection */
#నిర్వచించు CURL_META_PROTO_WS_CONN   "meta:proto:ws:conn"

CURLcode Curl_ws_request(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం dynbuf *req);
CURLcode Curl_ws_accept(నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి అక్షరం *mem, size_t nread);

#లేకపోతే
#నిర్వచించు Curl_ws_request(x, y) CURLE_OK
#నిర్వచించు Curl_ws_free(x)       Curl_nop_stmt
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_WS_H */
