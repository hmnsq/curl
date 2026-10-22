#ifndef HEADER_CURL_SHA512_256_H
#define HEADER_CURL_SHA512_256_H
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
#অন্তর্ভাৱ "curl_setup.h"

#যদি !defined(CURL_DISABLE_DIGEST_AUTH) && !defined(CURL_DISABLE_SHA512_256)

#অন্তর্ভাৱ "curl_hmac.h"

#define CURL_HAVE_SHA512_256

বাহ্য ধ্রুৱ সংরচনা HMAC_params Curl_HMAC_SHA512_256[1];

#define CURL_SHA512_256_DIGEST_LENGTH 32

CURLcode Curl_sha512_256it(unsigned ৱর্ণ *output, ধ্রুৱ unsigned ৱর্ণ *প্ৰবেশ,
                           size_t প্ৰবেশ_size);

#endif /* !CURL_DISABLE_DIGEST_AUTH && !CURL_DISABLE_SHA512_256 */

#endif /* HEADER_CURL_SHA256_H */
