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

بنية Curl_str;
بنية hostname;

bool Curl_is_ASCII_name(ثابت حرف *hostname);
bool Curl_is_ASCII_str(بنية Curl_str *s);

CURLcode Curl_idnconvert_hostname(بنية hostname *host);

#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
فراغ Curl_free_idnconverted_hostname(بنية hostname *host);
CURLcode Curl_idn_decode(ثابت حرف *ماسوقن, حرف **output);
CURLcode Curl_idn_encode(ثابت حرف *puny, حرف **output);
#وإلا
#define Curl_free_idnconverted_hostname(x)
#define Curl_idn_decode(x) عدم
#endif

#endif /* HEADER_CURL_IDN_H */
