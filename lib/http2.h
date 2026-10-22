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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#define DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
வெறுமை Curl_http2_ver(எழுத்து *p, size_t len);

#ifdef CURLVERBOSE
முழு Curl_nghttp2_fr_print(மாறிலி nghttp2_frame *frame, எழுத்து *buffer,
                          size_t blen);
#endif

CURLcode Curl_http2_request_upgrade(கட்டமைப்பு dynbuf *req,
                                    கட்டமைப்பு Curl_easy *data);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(கட்டமைப்பு Curl_easy *data);

bool Curl_http2_may_switch(கட்டமைப்பு Curl_easy *data);

CURLcode Curl_http2_switch(கட்டமைப்பு Curl_easy *data);

CURLcode Curl_http2_switch_at(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data);

CURLcode Curl_http2_upgrade(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு connectdata *conn, int8_t sockindex,
                            மாறிலி எழுத்து *mem, size_t nread);

வெறுமை *Curl_nghttp2_malloc(size_t size, வெறுமை *user_data);
வெறுமை Curl_nghttp2_free(வெறுமை *ptr, வெறுமை *user_data);
வெறுமை *Curl_nghttp2_calloc(size_t nmemb, size_t size, வெறுமை *user_data);
வெறுமை *Curl_nghttp2_realloc(வெறுமை *ptr, size_t size, வெறுமை *user_data);

வெளி கட்டமைப்பு Curl_cftype Curl_cft_nghttp2;

#இல்லையெனில் /* USE_NGHTTP2 */

#define Curl_http2_may_switch(a) FALSE

#define Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#define Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#define Curl_h2_http_1_1_error(x)         0
#endif

#endif /* HEADER_CURL_HTTP2_H */
