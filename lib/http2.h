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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_NGHTTP2

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#define DEFAULT_MAX_CONCURRENT_STREAMS 100

/*
 * Store nghttp2 version info in this buffer.
 */
શૂન્ય Curl_http2_ver(અક્ષર *p, size_t len);

#ifdef CURLVERBOSE
પૂર્ણાંક Curl_nghttp2_fr_print(અચલ nghttp2_frame *frame, અક્ષર *buffer,
                          size_t blen);
#endif

CURLcode Curl_http2_request_upgrade(સંરચના dynbuf *req,
                                    સંરચના Curl_easy *data);

/* returns true if the HTTP/2 stream error was HTTP_1_1_REQUIRED */
bool Curl_h2_http_1_1_error(સંરચના Curl_easy *data);

bool Curl_http2_may_switch(સંરચના Curl_easy *data);

CURLcode Curl_http2_switch(સંરચના Curl_easy *data);

CURLcode Curl_http2_switch_at(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data);

CURLcode Curl_http2_upgrade(સંરચના Curl_easy *data,
                            સંરચના connectdata *conn, int8_t sockindex,
                            અચલ અક્ષર *mem, size_t nread);

શૂન્ય *Curl_nghttp2_malloc(size_t size, શૂન્ય *user_data);
શૂન્ય Curl_nghttp2_free(શૂન્ય *ptr, શૂન્ય *user_data);
શૂન્ય *Curl_nghttp2_calloc(size_t nmemb, size_t size, શૂન્ય *user_data);
શૂન્ય *Curl_nghttp2_realloc(શૂન્ય *ptr, size_t size, શૂન્ય *user_data);

બાહ્ય સંરચના Curl_cftype Curl_cft_nghttp2;

#નહીંતર /* USE_NGHTTP2 */

#define Curl_http2_may_switch(a) FALSE

#define Curl_http2_request_upgrade(x, y)  CURLE_UNSUPPORTED_PROTOCOL
#define Curl_http2_switch(a)              CURLE_UNSUPPORTED_PROTOCOL
#define Curl_h2_http_1_1_error(x)         0
#endif

#endif /* HEADER_CURL_HTTP2_H */
