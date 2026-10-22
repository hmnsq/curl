#यदि_अपरिभाषितम् शीर्षकम्_CURL_SHA512_256_H
#परिभाषय शीर्षकम्_CURL_SHA512_256_H
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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_DIGEST_AUTH) && !defined(CURL_DISABLE_SHA512_256)

#अन्तर्भाव "curl_hmac.h"

#परिभाषय CURL_HAVE_SHA512_256

बाह्य ध्रुव संरचना HMAC_params Curl_HMAC_SHA512_256[1];

#परिभाषय CURL_SHA512_256_DIGEST_दीर्घता 32

CURLcode Curl_sha512_256it(अचिह्नित वर्ण *output, ध्रुव अचिह्नित वर्ण *निवेशः,
                           size_t निवेशः_आकार);

#यदि_अन्तः /* !CURL_DISABLE_DIGEST_AUTH && !CURL_DISABLE_SHA512_256 */

#यदि_अन्तः /* HEADER_CURL_SHA256_H */
