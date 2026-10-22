#که_نه_تعریف_شوی سرلیک_CURL_سرلیک_H
#تعریف_کړه سرلیک_CURL_سرلیک_H
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

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_سرلیکونه_API)

بنية Curl_سرلیک_store {
  بنية Curl_llist_غوټه غوټه;
  حرف *name; /* points into 'buffer' */
  حرف *ارزښت; /* points into 'buffer' */
  size_t nh_amount; /* cached amount for curl_easy_nextheader() */
  size_t nh_شاخص; /* cached index for curl_easy_nextheader() */
  صحيح request; /* 0 is the first request, then 1.. 2.. */
  بینښه حرف type; /* CURLH_* defines */
  حرف بفر[1]; /* this is the raw header blob */
};

/*
 * Initialize header collecting for a transfer.
 * Add a client writer that catches CLIENTWRITE_HEADER writes.
 */
CURLcode Curl_سرلیکونه_init(بنية Curl_easy *معلومات);

/*
 * Curl_headers_push() gets passed a full header to store.
 */
CURLcode Curl_سرلیکونه_ټېل(بنية Curl_easy *معلومات, ثابت حرف *سرلیک,
                           size_t hlen, بینښه حرف type);

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_سرلیکونه_cleanup(بنية Curl_easy *معلومات);

#وإلا
#تعریف_کړه Curl_سرلیکونه_init(x)          CURLE_OK
#تعریف_کړه Curl_سرلیکونه_ټېل(x, y, z, a) \
  ((فراغ)(x), (فراغ)(y), (فراغ)(z), (فراغ)(a), CURLE_OK)
#تعریف_کړه Curl_سرلیکونه_cleanup(x)       Curl_nop_stmt
#د_که_پای

#د_که_پای /* HEADER_CURL_HEADER_H */
