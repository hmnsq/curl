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
#ضمن "curl_setup.h"

#ifdef USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#define DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
فراغ Curl_http2_ver(حرف *p, size_t len);

#ifdef CURLVERBOSE
صحيح Curl_nghttp2_fr_اطبع(ثابت nghttp2_frame *frame, حرف *buffer,
                          size_t blen);
#endif

CURLcode Curl_http2_request_upgrade(بنية dynbuf *req,
                                    بنية Curl_easy *data);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(بنية Curl_easy *data);

bool Curl_http2_may_switch(بنية Curl_easy *data);

CURLcode Curl_http2_switch(بنية Curl_easy *data);

CURLcode Curl_http2_switch_at(بنية Curl_cfilter *cf, بنية Curl_easy *data);

CURLcode Curl_http2_upgrade(بنية Curl_easy *data,
                            بنية connectdata *conn, int8_t sockindex,
                            ثابت حرف *mem, size_t nread);

فراغ *Curl_nghttp2_malloc(size_t size, فراغ *user_data);
فراغ Curl_nghttp2_free(فراغ *ptr, فراغ *user_data);
فراغ *Curl_nghttp2_calloc(size_t nmemb, size_t size, فراغ *user_data);
فراغ *Curl_nghttp2_realloc(فراغ *ptr, size_t size, فراغ *user_data);

خارجي بنية Curl_cftype Curl_cft_nghttp2;

#وإلا /* USE_NGHTTP2 */

#define Curl_http2_may_switch(a) FALSE

#define Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#define Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#define Curl_h2_http_1_1_error(x)         0
#endif

#endif /* HEADER_CURL_HTTP2_H */
