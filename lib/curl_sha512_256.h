#جے_بنا_تعریف سرخی_CURL_SHA512_256_H
#تعریف_کرو سرخی_CURL_SHA512_256_H
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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_DIGEST_AUTH) && !defined(CURL_DISABLE_SHA512_256)

#ضمن "curl_hmac.h"

#تعریف_کرو CURL_HAVE_SHA512_256

خارجي ثابت بنية HMAC_params Curl_HMAC_SHA512_256[1];

#تعریف_کرو CURL_SHA512_256_DIGEST_لمائی 32

CURLcode Curl_sha512_256it(بنانشانی حرف *output, ثابت بنانشانی حرف *داخلہ,
                           size_t داخلہ_ناپ);

#جے_انت /* !CURL_DISABLE_DIGEST_AUTH && !CURL_DISABLE_SHA512_256 */

#جے_انت /* HEADER_CURL_SHA256_H */
