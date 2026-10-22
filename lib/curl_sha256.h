#اگر_غیر_تعریف_شدہ سرخی_CURL_SHA256_H
#تعریف_کرو سرخی_CURL_SHA256_H
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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_HTTPSIG) ||     \
  !defined(CURL_DISABLE_DIGEST_AUTH) ||                                 \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#شامل "curl_hmac.h"

خارجی ثابت ساخت HMAC_params Curl_HMAC_SHA256;

#اگر_غیر_تعریف_شدہ CURL_SHA256_DIGEST_لمبائی
#تعریف_کرو CURL_SHA256_DIGEST_لمبائی 32 /* fixed size */
#اگر_ختم

CURLcode Curl_sha256it(غیرعلامتی حرف *output, ثابت غیرعلامتی حرف *داخلہ,
                       size_t len);

#اگر_ختم

#اگر_ختم /* HEADER_CURL_SHA256_H */
