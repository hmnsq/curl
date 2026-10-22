#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SHA256_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SHA256_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Florin Petriuc, <petriuc.florin@gmail.com>
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

#ਜੇ !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_HTTPSIG) ||     \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||                                 \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_hmac.h"

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ HMAC_params Curl_HMAC_SHA256;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_SHA256_DIGEST_ਲੰਮਾਈ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SHA256_DIGEST_ਲੰਮਾਈ 32 /* fixed size */
#ਜੇ_ਅੰਤ

CURLcode Curl_sha256it(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *output, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼,
                       size_t len);

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_SHA256_H */
