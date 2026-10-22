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
#include "curl_setup.h"

#ꦪꦼꦤ꧀ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

sa_nracanaa Curl_header_store {
  sa_nracanaa Curl_llist_node node;
  var_na *name; /* points into 'buffer' */
  var_na *value; /* points into 'buffer' */
  size_t nh_amount; /* cached amount for curl_easy_nextheader() */
  size_t nh_index; /* cached index for curl_easy_nextheader() */
  puur_naa_nka request; /* 0 is the first request, then 1.. 2.. */
  unsigned var_na type; /* CURLH_* defines */
  var_na buffer[1]; /* this is the raw header blob */
};

/*
 * Initialize header collecting for a transfer.
 * Add a client writer that catches CLIENTWRITE_HEADER writes.
 */
CURLcode Curl_headers_init(sa_nracanaa Curl_easy *data);

/*
 * Curl_headers_push() gets passed a full header to store.
 */
CURLcode Curl_headers_push(sa_nracanaa Curl_easy *data, const var_na *header,
                           size_t hlen, unsigned var_na type);

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(sa_nracanaa Curl_easy *data);

#ꦭꦶꦪ
#define Curl_headers_init(x)          CURLE_OK
#define Curl_headers_push(x, y, z, a) \
  ((void)(x), (void)(y), (void)(z), (void)(a), CURLE_OK)
#define Curl_headers_cleanup(x)       Curl_nop_stmt
#endif

#endif /* HEADER_CURL_HEADER_H */
