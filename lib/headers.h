#اگر_تعریف_نشده سرآیند_CURL_سرآیند_H
#تعریف_کن سرآیند_CURL_سرآیند_H
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

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_سرآیندها_API)

بنية Curl_سرآیند_store {
  بنية Curl_llist_گره گره;
  حرف *name; /* points into 'buffer' */
  حرف *مقدار; /* points into 'buffer' */
  size_t nh_amount; /* cached amount for curl_easy_nextheader() */
  size_t nh_شاخص; /* cached index for curl_easy_nextheader() */
  صحيح request; /* 0 is the first request, then 1.. 2.. */
  بیعلامت حرف type; /* CURLH_* defines */
  حرف میانگیر[1]; /* this is the raw header blob */
};

/*
 * Initialize header collecting for a transfer.
 * Add a client writer that catches CLIENTWRITE_HEADER writes.
 */
CURLcode Curl_سرآیندها_init(بنية Curl_easy *داده);

/*
 * Curl_headers_push() gets passed a full header to store.
 */
CURLcode Curl_سرآیندها_افزودن(بنية Curl_easy *داده, ثابت حرف *سرآیند,
                           size_t hlen, بیعلامت حرف type);

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_سرآیندها_cleanup(بنية Curl_easy *داده);

#وإلا
#تعریف_کن Curl_سرآیندها_init(x)          CURLE_OK
#تعریف_کن Curl_سرآیندها_افزودن(x, y, z, a) \
  ((فراغ)(x), (فراغ)(y), (فراغ)(z), (فراغ)(a), CURLE_OK)
#تعریف_کن Curl_سرآیندها_cleanup(x)       Curl_nop_stmt
#پایان_اگر

#پایان_اگر /* HEADER_CURL_HEADER_H */
