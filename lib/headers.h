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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

ಸಂರಚನಾ Curl_header_store {
  ಸಂರಚನಾ Curl_llist_ಗಂಟು ಗಂಟು;
  ವರ್ಣ *name; /* points into 'buffer' */
  ವರ್ಣ *ಮೌಲ್ಯ; /* points into 'buffer' */
  size_t nh_amount; /* cached amount for curl_easy_nextheader() */
  size_t nh_index; /* cached index for curl_easy_nextheader() */
  ಪೂರ್ಣಾಙ್ಕ request; /* 0 is the first request, then 1.. 2.. */
  unsigned ವರ್ಣ type; /* CURLH_* defines */
  ವರ್ಣ buffer[1]; /* this is the raw header blob */
};

/*
 * Initialize header collecting for a transfer.
 * Add a client writer that catches CLIENTWRITE_HEADER writes.
 */
CURLcode Curl_headers_init(ಸಂರಚನಾ Curl_easy *data);

/*
 * Curl_headers_push() gets passed a full header to store.
 */
CURLcode Curl_headers_ತಳ್ಳು(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *header,
                           size_t hlen, unsigned ವರ್ಣ type);

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(ಸಂರಚನಾ Curl_easy *data);

#ಅನ್ಯಥಾ
#define Curl_headers_init(x)          CURLE_OK
#define Curl_headers_ತಳ್ಳು(x, y, z, a) \
  ((ಶೂನ್ಯ)(x), (ಶೂನ್ಯ)(y), (ಶೂನ್ಯ)(z), (ಶೂನ್ಯ)(a), CURLE_OK)
#define Curl_headers_cleanup(x)       Curl_nop_stmt
#endif

#endif /* HEADER_CURL_HEADER_H */
