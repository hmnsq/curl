#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_HTTP2_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_HTTP2_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
ಶೂನ್ಯ Curl_http2_ver(ಅಕ್ಷರ *p, size_t len);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಪೂರ್ಣಾಂಕ Curl_nghttp2_fr_print(ಸ್ಥಿರಾಂಕ nghttp2_ಚೌಕಟ್ಟು *ಚೌಕಟ್ಟು, ಅಕ್ಷರ *ಬಫರ್,
                          size_t blen);
#ಆದರೆ_ಅಂತ್ಯ

CURLcode Curl_http2_request_upgrade(ರಚನೆ dynbuf *req,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

bool Curl_http2_may_switch(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

CURLcode Curl_http2_switch(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

CURLcode Curl_http2_switch_at(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

CURLcode Curl_http2_upgrade(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ connectdata *conn, int8_t sockindex,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mem, size_t nread);

ಶೂನ್ಯ *Curl_nghttp2_malloc(size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);
ಶೂನ್ಯ Curl_nghttp2_free(ಶೂನ್ಯ *ptr, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);
ಶೂನ್ಯ *Curl_nghttp2_calloc(size_t nmemb, size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);
ಶೂನ್ಯ *Curl_nghttp2_realloc(ಶೂನ್ಯ *ptr, size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);

ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_nghttp2;

#ಇಲ್ಲದಿದ್ದರೆ /* USE_NGHTTP2 */

#ವ್ಯಾಖ್ಯಾನಿಸು Curl_http2_may_switch(a) FALSE

#ವ್ಯಾಖ್ಯಾನಿಸು Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_h2_http_1_1_error(x)         0
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_HTTP2_H */
