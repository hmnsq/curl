#اگر_تعریف_نشده سرآیند_CURL_SHA256_H
#تعریف_کن سرآیند_CURL_SHA256_H
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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_HTTPSIG) ||     \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||                                 \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#ضمن "curl_hmac.h"

خارجي ثابت بنية HMAC_params Curl_HMAC_SHA256;

#اگر_تعریف_نشده CURL_SHA256_DIGEST_درازی
#تعریف_کن CURL_SHA256_DIGEST_درازی 32 /* fixed size */
#پایان_اگر

CURLcode Curl_sha256it(بیعلامت حرف *output, ثابت بیعلامت حرف *درون_داد,
                       size_t len);

#پایان_اگر

#پایان_اگر /* HEADER_CURL_SHA256_H */
