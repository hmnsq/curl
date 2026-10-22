#ifndef HEADER_CURL_HEADER_H
#define HEADER_CURL_HEADER_H
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

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

கட்டமைப்பு Curl_header_store {
  கட்டமைப்பு Curl_llist_முனை முனை;
  எழுத்து *name; /* points into 'buffer' */
  எழுத்து *மதிப்பு; /* points into 'buffer' */
  size_t nh_amount; /* cached amount for curl_easy_nextheader() */
  size_t nh_index; /* cached index for curl_easy_nextheader() */
  முழு request; /* 0 is the first request, then 1.. 2.. */
  unsigned எழுத்து type; /* CURLH_* defines */
  எழுத்து buffer[1]; /* this is the raw header blob */
};

/*
 * Initialize header collecting for a transfer.
 * Add a client writer that catches CLIENTWRITE_HEADER writes.
 */
CURLcode Curl_headers_init(கட்டமைப்பு Curl_easy *data);

/*
 * Curl_headers_push() gets passed a full header to store.
 */
CURLcode Curl_headers_தள்ளு(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *header,
                           size_t hlen, unsigned எழுத்து type);

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(கட்டமைப்பு Curl_easy *data);

#இல்லையெனில்
#define Curl_headers_init(x)          CURLE_OK
#define Curl_headers_தள்ளு(x, y, z, a) \
  ((வெறுமை)(x), (வெறுமை)(y), (வெறுமை)(z), (வெறுமை)(a), CURLE_OK)
#define Curl_headers_cleanup(x)       Curl_nop_stmt
#endif

#endif /* HEADER_CURL_HEADER_H */
