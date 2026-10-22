#నిర్వచితం_కాకపోతే శీర్షిక_CURL_HTTP2_H
#నిర్వచించు శీర్షిక_CURL_HTTP2_H
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

#నిర్వచితమైతే USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#నిర్వచించు DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
శూన్య Curl_http2_ver(అక్షరం *p, size_t len);

#నిర్వచితమైతే CURLVERBOSE
పూర్ణాంకం Curl_nghttp2_fr_print(స్థిరరాశి nghttp2_చట్రం *చట్రం, అక్షరం *బఫరు,
                          size_t blen);
#అయితే_ముగింపు

CURLcode Curl_http2_request_upgrade(నిర్మాణం dynbuf *req,
                                    నిర్మాణం Curl_easy *దత్తాంశం);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(నిర్మాణం Curl_easy *దత్తాంశం);

bool Curl_http2_may_switch(నిర్మాణం Curl_easy *దత్తాంశం);

CURLcode Curl_http2_switch(నిర్మాణం Curl_easy *దత్తాంశం);

CURLcode Curl_http2_switch_at(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం);

CURLcode Curl_http2_upgrade(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం connectdata *conn, int8_t sockindex,
                            స్థిరరాశి అక్షరం *mem, size_t nread);

శూన్య *Curl_nghttp2_malloc(size_t కొలత, శూన్య *user_దత్తాంశం);
శూన్య Curl_nghttp2_free(శూన్య *ptr, శూన్య *user_దత్తాంశం);
శూన్య *Curl_nghttp2_calloc(size_t nmemb, size_t కొలత, శూన్య *user_దత్తాంశం);
శూన్య *Curl_nghttp2_realloc(శూన్య *ptr, size_t కొలత, శూన్య *user_దత్తాంశం);

బాహ్య నిర్మాణం Curl_cftype Curl_cft_nghttp2;

#లేకపోతే /* USE_NGHTTP2 */

#నిర్వచించు Curl_http2_may_switch(a) FALSE

#నిర్వచించు Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#నిర్వచించు Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#నిర్వచించు Curl_h2_http_1_1_error(x)         0
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_HTTP2_H */
