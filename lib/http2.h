#اگر_تعریف_نشده سرآیند_CURL_HTTP2_H
#تعریف_کن سرآیند_CURL_HTTP2_H
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

#اگر_تعریف_شده USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#تعریف_کن DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
فراغ Curl_http2_ver(حرف *p, size_t len);

#اگر_تعریف_شده CURLVERBOSE
صحيح Curl_nghttp2_fr_print(ثابت nghttp2_قاب *قاب, حرف *میانگیر,
                          size_t blen);
#پایان_اگر

CURLcode Curl_http2_request_upgrade(بنية dynbuf *req,
                                    بنية Curl_easy *داده);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(بنية Curl_easy *داده);

bool Curl_http2_may_switch(بنية Curl_easy *داده);

CURLcode Curl_http2_switch(بنية Curl_easy *داده);

CURLcode Curl_http2_switch_at(بنية Curl_cfilter *cf, بنية Curl_easy *داده);

CURLcode Curl_http2_upgrade(بنية Curl_easy *داده,
                            بنية connectdata *conn, int8_t sockindex,
                            ثابت حرف *mem, size_t nread);

فراغ *Curl_nghttp2_malloc(size_t اندازه, فراغ *user_داده);
فراغ Curl_nghttp2_free(فراغ *ptr, فراغ *user_داده);
فراغ *Curl_nghttp2_calloc(size_t nmemb, size_t اندازه, فراغ *user_داده);
فراغ *Curl_nghttp2_realloc(فراغ *ptr, size_t اندازه, فراغ *user_داده);

خارجي بنية Curl_cftype Curl_cft_nghttp2;

#وإلا /* USE_NGHTTP2 */

#تعریف_کن Curl_http2_may_switch(a) FALSE

#تعریف_کن Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#تعریف_کن Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#تعریف_کن Curl_h2_http_1_1_error(x)         0
#پایان_اگر

#پایان_اگر /* HEADER_CURL_HTTP2_H */
