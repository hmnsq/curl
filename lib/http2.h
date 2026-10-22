#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_HTTP2_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_HTTP2_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
ਖਾਲੀ Curl_http2_ver(ਅੱਖਰ *p, size_t len);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਪੂਰਨ Curl_nghttp2_fr_print(ਅਚਲ nghttp2_ਚੌਖਟਾ *ਚੌਖਟਾ, ਅੱਖਰ *ਬਫਰ,
                          size_t blen);
#ਜੇ_ਅੰਤ

CURLcode Curl_http2_request_upgrade(ਬਣਤਰ dynbuf *req,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

bool Curl_http2_may_switch(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

CURLcode Curl_http2_switch(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

CURLcode Curl_http2_switch_at(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

CURLcode Curl_http2_upgrade(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ connectdata *conn, int8_t sockindex,
                            ਅਚਲ ਅੱਖਰ *mem, size_t nread);

ਖਾਲੀ *Curl_nghttp2_malloc(size_t ਮਾਪ, ਖਾਲੀ *user_ਅੰਕੜੇ);
ਖਾਲੀ Curl_nghttp2_free(ਖਾਲੀ *ptr, ਖਾਲੀ *user_ਅੰਕੜੇ);
ਖਾਲੀ *Curl_nghttp2_calloc(size_t nmemb, size_t ਮਾਪ, ਖਾਲੀ *user_ਅੰਕੜੇ);
ਖਾਲੀ *Curl_nghttp2_realloc(ਖਾਲੀ *ptr, size_t ਮਾਪ, ਖਾਲੀ *user_ਅੰਕੜੇ);

ਬਾਹਰੀ ਬਣਤਰ Curl_cftype Curl_cft_nghttp2;

#ਨਹੀਂਤਾਂ /* USE_NGHTTP2 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_http2_may_switch(a) FALSE

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_h2_http_1_1_error(x)         0
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_HTTP2_H */
