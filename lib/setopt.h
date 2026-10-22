#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SETOPT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SETOPT_H
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
CURLcode Curl_setopt_SSLVERSION(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option,
                                ਲੰਮਾ arg);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_setopt_SSLVERSION(a, b, c) CURLE_NOT_BUILT_ਵਿੱਚ
#ਜੇ_ਅੰਤ

CURLcode Curl_setstropt(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਗਿਣਤੀ dupstring id, ਅਚਲ ਅੱਖਰ *s) WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;
CURLcode Curl_setblobopt(ਬਣਤਰ curl_blob **blobp,
                         ਅਚਲ ਬਣਤਰ curl_blob *blob) WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;
CURLcode Curl_vsetopt(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLoption option, va_list param)
  WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;

#ਜੇ_ਅੰਤ /* HEADER_CURL_SETOPT_H */
