#که_نه_تعریف_شوی سرلیک_CURL_SHA256_H
#تعریف_کړه سرلیک_CURL_SHA256_H
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

#که_نه_تعریف_شوی CURL_SHA256_DIGEST_اوږدتیا
#تعریف_کړه CURL_SHA256_DIGEST_اوږدتیا 32 /* fixed size */
#د_که_پای

CURLcode Curl_sha256it(بینښه حرف *output, ثابت بینښه حرف *ننوت,
                       size_t len);

#د_که_پای

#د_که_پای /* HEADER_CURL_SHA256_H */
