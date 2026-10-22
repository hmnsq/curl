#اگر_غیر_تعریف_شدہ سرخی_CURL_HTTP2_H
#تعریف_کرو سرخی_CURL_HTTP2_H
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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#تعریف_کرو DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
خالی Curl_http2_ver(حرف *p, size_t len);

#اگر_تعریف_شدہ CURLVERBOSE
صحیح Curl_nghttp2_fr_print(ثابت nghttp2_چوکھٹا *چوکھٹا, حرف *بفر,
                          size_t blen);
#اگر_ختم

CURLcode Curl_http2_request_upgrade(ساخت dynbuf *req,
                                    ساخت Curl_easy *ڈیٹا);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(ساخت Curl_easy *ڈیٹا);

bool Curl_http2_may_switch(ساخت Curl_easy *ڈیٹا);

CURLcode Curl_http2_switch(ساخت Curl_easy *ڈیٹا);

CURLcode Curl_http2_switch_at(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا);

CURLcode Curl_http2_upgrade(ساخت Curl_easy *ڈیٹا,
                            ساخت connectdata *conn, int8_t sockindex,
                            ثابت حرف *mem, size_t nread);

خالی *Curl_nghttp2_malloc(size_t سائز, خالی *user_ڈیٹا);
خالی Curl_nghttp2_free(خالی *ptr, خالی *user_ڈیٹا);
خالی *Curl_nghttp2_calloc(size_t nmemb, size_t سائز, خالی *user_ڈیٹا);
خالی *Curl_nghttp2_realloc(خالی *ptr, size_t سائز, خالی *user_ڈیٹا);

خارجی ساخت Curl_cftype Curl_cft_nghttp2;

#ورنہ /* USE_NGHTTP2 */

#تعریف_کرو Curl_http2_may_switch(a) FALSE

#تعریف_کرو Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#تعریف_کرو Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#تعریف_کرو Curl_h2_http_1_1_error(x)         0
#اگر_ختم

#اگر_ختم /* HEADER_CURL_HTTP2_H */
