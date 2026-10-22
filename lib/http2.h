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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#define DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
ଶୂନ୍ୟ Curl_http2_ver(ଅକ୍ଷର *p, size_t len);

#ifdef CURLVERBOSE
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_nghttp2_fr_print(ଅଚଳ nghttp2_frame *frame, ଅକ୍ଷର *buffer,
                          size_t blen);
#endif

CURLcode Curl_http2_request_upgrade(ସଂରଚନା dynbuf *req,
                                    ସଂରଚନା Curl_easy *data);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(ସଂରଚନା Curl_easy *data);

bool Curl_http2_may_switch(ସଂରଚନା Curl_easy *data);

CURLcode Curl_http2_switch(ସଂରଚନା Curl_easy *data);

CURLcode Curl_http2_switch_at(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data);

CURLcode Curl_http2_upgrade(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା connectdata *conn, int8_t sockindex,
                            ଅଚଳ ଅକ୍ଷର *mem, size_t nread);

ଶୂନ୍ୟ *Curl_nghttp2_malloc(size_t size, ଶୂନ୍ୟ *user_data);
ଶୂନ୍ୟ Curl_nghttp2_free(ଶୂନ୍ୟ *ptr, ଶୂନ୍ୟ *user_data);
ଶୂନ୍ୟ *Curl_nghttp2_calloc(size_t nmemb, size_t size, ଶୂନ୍ୟ *user_data);
ଶୂନ୍ୟ *Curl_nghttp2_realloc(ଶୂନ୍ୟ *ptr, size_t size, ଶୂନ୍ୟ *user_data);

ବାହ୍ୟ ସଂରଚନା Curl_cftype Curl_cft_nghttp2;

#ଅନ୍ୟଥା /* USE_NGHTTP2 */

#define Curl_http2_may_switch(a) FALSE

#define Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#define Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#define Curl_h2_http_1_1_error(x)         0
#endif

#endif /* HEADER_CURL_HTTP2_H */
