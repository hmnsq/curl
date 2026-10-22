#अगर_अघोषित HEADER_CURL_HTTP2_H
#घोषणा HEADER_CURL_HTTP2_H
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
#समावेश "curl_setup.h"

#अगर_घोषित USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#घोषणा DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
व्योम Curl_http2_ver(अक्षर *p, माप_प्रकार len);

#अगर_घोषित CURLVERBOSE
पूर्णांक Curl_nghttp2_fr_print(स्थिर nghttp2_frame *frame, अक्षर *बफर,
                          माप_प्रकार blen);
#पूर्ण_अगर

CURLcode Curl_http2_request_upgrade(काष्ठा dynbuf *req,
                                    काष्ठा Curl_easy *तथ्य);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(काष्ठा Curl_easy *तथ्य);

bool Curl_http2_may_switch(काष्ठा Curl_easy *तथ्य);

CURLcode Curl_http2_switch(काष्ठा Curl_easy *तथ्य);

CURLcode Curl_http2_switch_at(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य);

CURLcode Curl_http2_upgrade(काष्ठा Curl_easy *तथ्य,
                            काष्ठा connectdata *conn, int8_t sockindex,
                            स्थिर अक्षर *mem, माप_प्रकार nread);

व्योम *Curl_nghttp2_malloc(माप_प्रकार आकार, व्योम *user_तथ्य);
व्योम Curl_nghttp2_free(व्योम *ptr, व्योम *user_तथ्य);
व्योम *Curl_nghttp2_calloc(माप_प्रकार nmemb, माप_प्रकार आकार, व्योम *user_तथ्य);
व्योम *Curl_nghttp2_realloc(व्योम *ptr, माप_प्रकार आकार, व्योम *user_तथ्य);

बाह्य काष्ठा Curl_cftype Curl_cft_nghttp2;

#अन्यथा /* USE_NGHTTP2 */

#घोषणा Curl_http2_may_switch(a) FALSE

#घोषणा Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#घोषणा Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#घोषणा Curl_h2_http_1_1_error(x)         0
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_HTTP2_H */
