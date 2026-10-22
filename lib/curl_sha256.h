#ifndef HEADER_CURL_SHA256_H
#define HEADER_CURL_SHA256_H
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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_HTTPSIG) ||     \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||                                 \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#অন্তর্ভাব "curl_hmac.h"

বাহ্যিক ধ্রুবক কাঠামো HMAC_params Curl_HMAC_SHA256;

#ifndef CURL_SHA256_DIGEST_দৈর্ঘ্য
#define CURL_SHA256_DIGEST_দৈর্ঘ্য 32 /* fixed size */
#endif

CURLcode Curl_sha256it(unsigned অক্ষর *output, ধ্রুবক unsigned অক্ষর *প্রবেশ,
                       size_t len);

#endif

#endif /* HEADER_CURL_SHA256_H */
