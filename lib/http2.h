#که_نه_تعریف_شوی سرلیک_CURL_HTTP2_H
#تعریف_کړه سرلیک_CURL_HTTP2_H
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

#که_تعریف_شوی USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#تعریف_کړه DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
فراغ Curl_http2_ver(حرف *p, size_t len);

#که_تعریف_شوی CURLVERBOSE
صحيح Curl_nghttp2_fr_print(ثابت nghttp2_چوکاټ *چوکاټ, حرف *بفر,
                          size_t blen);
#د_که_پای

CURLcode Curl_http2_request_upgrade(بنية dynbuf *req,
                                    بنية Curl_easy *معلومات);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(بنية Curl_easy *معلومات);

bool Curl_http2_may_switch(بنية Curl_easy *معلومات);

CURLcode Curl_http2_switch(بنية Curl_easy *معلومات);

CURLcode Curl_http2_switch_at(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات);

CURLcode Curl_http2_upgrade(بنية Curl_easy *معلومات,
                            بنية connectdata *conn, int8_t sockindex,
                            ثابت حرف *mem, size_t nread);

فراغ *Curl_nghttp2_malloc(size_t اندازه, فراغ *user_معلومات);
فراغ Curl_nghttp2_free(فراغ *ptr, فراغ *user_معلومات);
فراغ *Curl_nghttp2_calloc(size_t nmemb, size_t اندازه, فراغ *user_معلومات);
فراغ *Curl_nghttp2_realloc(فراغ *ptr, size_t اندازه, فراغ *user_معلومات);

خارجي بنية Curl_cftype Curl_cft_nghttp2;

#وإلا /* USE_NGHTTP2 */

#تعریف_کړه Curl_http2_may_switch(a) FALSE

#تعریف_کړه Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#تعریف_کړه Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#تعریف_کړه Curl_h2_http_1_1_error(x)         0
#د_که_پای

#د_که_پای /* HEADER_CURL_HTTP2_H */
