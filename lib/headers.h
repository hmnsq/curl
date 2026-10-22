#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_سرو_H
#وصف_ڪريو سرو_CURL_سرو_H
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

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_سرا_API)

بنية Curl_سرو_store {
  بنية Curl_llist_ڳنڍ ڳنڍ;
  حرف *name; /* points into 'buffer' */
  حرف *قدر; /* points into 'buffer' */
  size_t nh_amount; /* cached amount for curl_easy_nextheader() */
  size_t nh_اشاريو; /* cached index for curl_easy_nextheader() */
  صحيح request; /* 0 is the first request, then 1.. 2.. */
  بنانشان حرف type; /* CURLH_* defines */
  حرف بفر[1]; /* this is the raw header blob */
};

/*
 * Initialize header collecting for a transfer.
 * Add a client writer that catches CLIENTWRITE_HEADER writes.
 */
CURLcode Curl_سرا_init(بنية Curl_easy *معلومات);

/*
 * Curl_headers_push() gets passed a full header to store.
 */
CURLcode Curl_سرا_ڌڪو(بنية Curl_easy *معلومات, ثابت حرف *سرو,
                           size_t hlen, بنانشان حرف type);

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_سرا_cleanup(بنية Curl_easy *معلومات);

#وإلا
#وصف_ڪريو Curl_سرا_init(x)          CURLE_OK
#وصف_ڪريو Curl_سرا_ڌڪو(x, y, z, a) \
  ((فراغ)(x), (فراغ)(y), (فراغ)(z), (فراغ)(a), CURLE_OK)
#وصف_ڪريو Curl_سرا_cleanup(x)       Curl_nop_stmt
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_HEADER_H */
