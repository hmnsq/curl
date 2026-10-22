#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_DIGEST_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_DIGEST_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DIGEST_AUTH

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIGEST_MAX_ਮੁੱਲ_ਲੰਮਾਈ           256
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIGEST_MAX_CONTENT_ਲੰਮਾਈ         1024

ਬਣਤਰ Curl_str;

/* This is used to extract the realm from a challenge message */
bool Curl_auth_digest_get_pair(ਅਚਲ ਅੱਖਰ *str, ਬਣਤਰ Curl_str *ਮੁੱਲ,
                               ਅੱਖਰ *content, ਅਚਲ ਅੱਖਰ **endptr);

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_DIGEST_H */
