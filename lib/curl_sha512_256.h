#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_SHA512_256_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_SHA512_256_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ !defined(CURL_DISABLE_DIGEST_AUTH) && !defined(CURL_DISABLE_SHA512_256)

#ಅನ್ತರ್ಭಾವ "curl_hmac.h"

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_HAVE_SHA512_256

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ HMAC_params Curl_HMAC_SHA512_256[1];

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SHA512_256_DIGEST_ಉದ್ದಳತೆ 32

CURLcode Curl_sha512_256it(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *output, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು,
                           size_t ಒಳಹರಿವು_ಗಾತ್ರ);

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_DIGEST_AUTH && !CURL_DISABLE_SHA512_256 */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SHA256_H */
