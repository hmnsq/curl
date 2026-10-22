#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SHA512_256_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SHA512_256_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Evgeny Grin (Karlson2k), <k2k@narod.ru>.
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

#ਜੇ !defined(CURL_DISABLE_DIGEST_AUTH) && !defined(CURL_DISABLE_SHA512_256)

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_hmac.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_HAVE_SHA512_256

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ HMAC_params Curl_HMAC_SHA512_256[1];

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SHA512_256_DIGEST_ਲੰਮਾਈ 32

CURLcode Curl_sha512_256it(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *output, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼,
                           size_t ਨਿਵੇਸ਼_ਮਾਪ);

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_DIGEST_AUTH && !CURL_DISABLE_SHA512_256 */

#ਜੇ_ਅੰਤ /* HEADER_CURL_SHA256_H */
