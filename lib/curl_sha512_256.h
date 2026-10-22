#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_SHA512_256_H
#وصف_ڪريو سرو_CURL_SHA512_256_H
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

#وصف_ڪريو CURL_HAVE_SHA512_256

خارجي ثابت بنية HMAC_params Curl_HMAC_SHA512_256[1];

#وصف_ڪريو CURL_SHA512_256_DIGEST_ڊيگهائي 32

CURLcode Curl_sha512_256it(بنانشان حرف *output, ثابت بنانشان حرف *داخلا,
                           size_t داخلا_سائيز);

#جيڪڏهن_پورو /* !CURL_DISABLE_DIGEST_AUTH && !CURL_DISABLE_SHA512_256 */

#جيڪڏهن_پورو /* HEADER_CURL_SHA256_H */
