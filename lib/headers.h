#اگر_غیر_تعریف_شدہ سرخی_CURL_سرخی_H
#تعریف_کرو سرخی_CURL_سرخی_H
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

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_سرخیاں_API)

ساخت Curl_سرخی_store {
  ساخت Curl_llist_گرہ گرہ;
  حرف *name; /* points into 'buffer' */
  حرف *قدر; /* points into 'buffer' */
  size_t nh_amount; /* cached amount for curl_easy_nextheader() */
  size_t nh_اشاریہ; /* cached index for curl_easy_nextheader() */
  صحیح request; /* 0 is the first request, then 1.. 2.. */
  غیرعلامتی حرف type; /* CURLH_* defines */
  حرف بفر[1]; /* this is the raw header blob */
};

/*
 * Initialize header collecting for a transfer.
 * Add a client writer that catches CLIENTWRITE_HEADER writes.
 */
CURLcode Curl_سرخیاں_init(ساخت Curl_easy *ڈیٹا);

/*
 * Curl_headers_push() gets passed a full header to store.
 */
CURLcode Curl_سرخیاں_ڈالو(ساخت Curl_easy *ڈیٹا, ثابت حرف *سرخی,
                           size_t hlen, غیرعلامتی حرف type);

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_سرخیاں_cleanup(ساخت Curl_easy *ڈیٹا);

#ورنہ
#تعریف_کرو Curl_سرخیاں_init(x)          CURLE_OK
#تعریف_کرو Curl_سرخیاں_ڈالو(x, y, z, a) \
  ((خالی)(x), (خالی)(y), (خالی)(z), (خالی)(a), CURLE_OK)
#تعریف_کرو Curl_سرخیاں_cleanup(x)       Curl_nop_stmt
#اگر_ختم

#اگر_ختم /* HEADER_CURL_HEADER_H */
