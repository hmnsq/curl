#ifndef HEADER_CURL_HTTP2_H
#define HEADER_CURL_HTTP2_H
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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#define DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
শূন্য Curl_http2_ver(বর্ণ *p, size_t len);

#ifdef CURLVERBOSE
পূর্ণাঙ্ক Curl_nghttp2_fr_print(ধ্রুব nghttp2_frame *frame, বর্ণ *buffer,
                          size_t blen);
#endif

CURLcode Curl_http2_request_upgrade(সংরচনা dynbuf *req,
                                    সংরচনা Curl_easy *data);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(সংরচনা Curl_easy *data);

bool Curl_http2_may_switch(সংরচনা Curl_easy *data);

CURLcode Curl_http2_switch(সংরচনা Curl_easy *data);

CURLcode Curl_http2_switch_at(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data);

CURLcode Curl_http2_upgrade(সংরচনা Curl_easy *data,
                            সংরচনা connectdata *conn, int8_t sockindex,
                            ধ্রুব বর্ণ *mem, size_t nread);

শূন্য *Curl_nghttp2_malloc(size_t size, শূন্য *user_data);
শূন্য Curl_nghttp2_free(শূন্য *ptr, শূন্য *user_data);
শূন্য *Curl_nghttp2_calloc(size_t nmemb, size_t size, শূন্য *user_data);
শূন্য *Curl_nghttp2_realloc(শূন্য *ptr, size_t size, শূন্য *user_data);

বাহ্য সংরচনা Curl_cftype Curl_cft_nghttp2;

#অন্যথা /* USE_NGHTTP2 */

#define Curl_http2_may_switch(a) FALSE

#define Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#define Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#define Curl_h2_http_1_1_error(x)         0
#endif

#endif /* HEADER_CURL_HTTP2_H */
