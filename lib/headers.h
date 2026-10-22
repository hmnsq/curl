#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_ਸਿਰਲੇਖ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_ਸਿਰਲੇਖ_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ਸਿਰਲੇਖਾਂ_API)

ਬਣਤਰ Curl_ਸਿਰਲੇਖ_store {
  ਬਣਤਰ Curl_llist_ਗੰਢ ਗੰਢ;
  ਅੱਖਰ *name; /* points into 'buffer' */
  ਅੱਖਰ *ਮੁੱਲ; /* points into 'buffer' */
  size_t nh_amount; /* cached amount for curl_easy_nextheader() */
  size_t nh_ਸੂਚਕਾਂਕ; /* cached index for curl_easy_nextheader() */
  ਪੂਰਨ request; /* 0 is the first request, then 1.. 2.. */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ type; /* CURLH_* defines */
  ਅੱਖਰ ਬਫਰ[1]; /* this is the raw header blob */
};

/*
 * Initialize header collecting for a transfer.
 * Add a client writer that catches CLIENTWRITE_HEADER writes.
 */
CURLcode Curl_ਸਿਰਲੇਖਾਂ_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/*
 * Curl_headers_push() gets passed a full header to store.
 */
CURLcode Curl_ਸਿਰਲੇਖਾਂ_ਧੱਕੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ,
                           size_t hlen, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ type);

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_ਸਿਰਲੇਖਾਂ_cleanup(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ਸਿਰਲੇਖਾਂ_init(x)          CURLE_OK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ਸਿਰਲੇਖਾਂ_ਧੱਕੋ(x, y, z, a) \
  ((ਖਾਲੀ)(x), (ਖਾਲੀ)(y), (ਖਾਲੀ)(z), (ਖਾਲੀ)(a), CURLE_OK)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ਸਿਰਲੇਖਾਂ_cleanup(x)       Curl_nop_stmt
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_HEADER_H */
