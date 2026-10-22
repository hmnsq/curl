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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HEADERS_API)

কাঠামো Curl_header_store {
  কাঠামো Curl_llist_গ্রন্থি গ্রন্থি;
  অক্ষর *name; /* points into 'buffer' */
  অক্ষর *মান; /* points into 'buffer' */
  size_t nh_amount; /* cached amount for curl_easy_nextheader() */
  size_t nh_সূচক; /* cached index for curl_easy_nextheader() */
  পূর্ণসংখ্যা request; /* 0 is the first request, then 1.. 2.. */
  unsigned অক্ষর type; /* CURLH_* defines */
  অক্ষর বাফার[1]; /* this is the raw header blob */
};

/*
 * Initialize header collecting for a transfer.
 * Add a client writer that catches CLIENTWRITE_HEADER writes.
 */
CURLcode Curl_headers_init(কাঠামো Curl_easy *তথ্য);

/*
 * Curl_headers_push() gets passed a full header to store.
 */
CURLcode Curl_headers_ঠেলো(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *header,
                           size_t hlen, unsigned অক্ষর type);

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_headers_cleanup(কাঠামো Curl_easy *তথ্য);

#অন্যথা
#define Curl_headers_init(x)          CURLE_OK
#define Curl_headers_ঠেলো(x, y, z, a) \
  ((শূন্য)(x), (শূন্য)(y), (শূন্য)(z), (শূন্য)(a), CURLE_OK)
#define Curl_headers_cleanup(x)       Curl_nop_stmt
#endif

#endif /* HEADER_CURL_HEADER_H */
