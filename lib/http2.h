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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#define DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
शून्य Curl_http2_ver(अक्षर *p, माप_प्रकार len);

#ifdef CURLVERBOSE
पूर्णांक Curl_nghttp2_fr_print(स्थिरांक nghttp2_frame *frame, अक्षर *buffer,
                          माप_प्रकार blen);
#endif

CURLcode Curl_http2_request_upgrade(संरचना dynbuf *req,
                                    संरचना Curl_easy *data);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(संरचना Curl_easy *data);

bool Curl_http2_may_switch(संरचना Curl_easy *data);

CURLcode Curl_http2_switch(संरचना Curl_easy *data);

CURLcode Curl_http2_switch_at(संरचना Curl_cfilter *cf, संरचना Curl_easy *data);

CURLcode Curl_http2_upgrade(संरचना Curl_easy *data,
                            संरचना connectdata *conn, int8_t sockindex,
                            स्थिरांक अक्षर *mem, माप_प्रकार nread);

शून्य *Curl_nghttp2_malloc(माप_प्रकार size, शून्य *user_data);
शून्य Curl_nghttp2_free(शून्य *ptr, शून्य *user_data);
शून्य *Curl_nghttp2_calloc(माप_प्रकार nmemb, माप_प्रकार size, शून्य *user_data);
शून्य *Curl_nghttp2_realloc(शून्य *ptr, माप_प्रकार size, शून्य *user_data);

बाह्य संरचना Curl_cftype Curl_cft_nghttp2;

#अन्यथा /* USE_NGHTTP2 */

#define Curl_http2_may_switch(a) FALSE

#define Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#define Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#define Curl_h2_http_1_1_error(x)         0
#endif

#endif /* HEADER_CURL_HTTP2_H */
