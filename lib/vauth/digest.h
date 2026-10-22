#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_DIGEST_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_DIGEST_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DIGEST_AUTH

#ವ್ಯಾಖ್ಯಾನಿಸು DIGEST_MAX_ಮೌಲ್ಯ_ಉದ್ದಳತೆ           256
#ವ್ಯಾಖ್ಯಾನಿಸು DIGEST_MAX_CONTENT_ಉದ್ದಳತೆ         1024

ರಚನೆ Curl_str;

/* This is used to extract the realm from a challenge message */
bool Curl_auth_digest_get_pair(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ರಚನೆ Curl_str *ಮೌಲ್ಯ,
                               ಅಕ್ಷರ *content, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **endptr);

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_DIGEST_H */
