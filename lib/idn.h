#ifndef HEADER_CURL_IDN_H
#define HEADER_CURL_IDN_H
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

কাঠামো Curl_str;
কাঠামো hostname;

bool Curl_is_ASCII_name(ধ্রুবক অক্ষর *hostname);
bool Curl_is_ASCII_str(কাঠামো Curl_str *s);

CURLcode Curl_idnconvert_hostname(কাঠামো hostname *host);

#যদি defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
শূন্য Curl_free_idnconverted_hostname(কাঠামো hostname *host);
CURLcode Curl_idn_decode(ধ্রুবক অক্ষর *প্রবেশ, অক্ষর **output);
CURLcode Curl_idn_encode(ধ্রুবক অক্ষর *puny, অক্ষর **output);
#অন্যথা
#define Curl_free_idnconverted_hostname(x)
#define Curl_idn_decode(x) ফাঁকা
#endif

#endif /* HEADER_CURL_IDN_H */
