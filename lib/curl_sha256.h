#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_SHA256_H
#وصف_ڪريو سرو_CURL_SHA256_H
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

#جيڪڏهن_وصف_نه_ٿيل CURL_SHA256_DIGEST_ڊيگهائي
#وصف_ڪريو CURL_SHA256_DIGEST_ڊيگهائي 32 /* fixed size */
#جيڪڏهن_پورو

CURLcode Curl_sha256it(بنانشان حرف *output, ثابت بنانشان حرف *داخلا,
                       size_t len);

#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_SHA256_H */
