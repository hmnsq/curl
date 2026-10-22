#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_IDN_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_IDN_H
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

ਬਣਤਰ Curl_str;
ਬਣਤਰ hostname;

bool Curl_is_ASCII_name(ਅਚਲ ਅੱਖਰ *hostname);
bool Curl_is_ASCII_str(ਬਣਤਰ Curl_str *s);

CURLcode Curl_idnconvert_hostname(ਬਣਤਰ hostname *host);

#ਜੇ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_IDN
ਖਾਲੀ Curl_free_idnconverted_hostname(ਬਣਤਰ hostname *host);
CURLcode Curl_idn_decode(ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼, ਅੱਖਰ **output);
CURLcode Curl_idn_encode(ਅਚਲ ਅੱਖਰ *puny, ਅੱਖਰ **output);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_free_idnconverted_hostname(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_idn_decode(x) ਨਿਰਾ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_IDN_H */
