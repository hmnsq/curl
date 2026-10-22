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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#define DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
ശൂന്യം Curl_http2_ver(അക്ഷരം *p, size_t len);

#ifdef CURLVERBOSE
പൂർണ്ണസംഖ്യ Curl_nghttp2_fr_print(സ്ഥിരം nghttp2_frame *frame, അക്ഷരം *buffer,
                          size_t blen);
#endif

CURLcode Curl_http2_request_upgrade(ഘടന dynbuf *req,
                                    ഘടന Curl_easy *data);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(ഘടന Curl_easy *data);

bool Curl_http2_may_switch(ഘടന Curl_easy *data);

CURLcode Curl_http2_switch(ഘടന Curl_easy *data);

CURLcode Curl_http2_switch_at(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data);

CURLcode Curl_http2_upgrade(ഘടന Curl_easy *data,
                            ഘടന connectdata *conn, int8_t sockindex,
                            സ്ഥിരം അക്ഷരം *mem, size_t nread);

ശൂന്യം *Curl_nghttp2_malloc(size_t size, ശൂന്യം *user_data);
ശൂന്യം Curl_nghttp2_free(ശൂന്യം *ptr, ശൂന്യം *user_data);
ശൂന്യം *Curl_nghttp2_calloc(size_t nmemb, size_t size, ശൂന്യം *user_data);
ശൂന്യം *Curl_nghttp2_realloc(ശൂന്യം *ptr, size_t size, ശൂന്യം *user_data);

ബാഹ്യ ഘടന Curl_cftype Curl_cft_nghttp2;

#അല്ലെങ്കിൽ /* USE_NGHTTP2 */

#define Curl_http2_may_switch(a) FALSE

#define Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#define Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#define Curl_h2_http_1_1_error(x)         0
#endif

#endif /* HEADER_CURL_HTTP2_H */
