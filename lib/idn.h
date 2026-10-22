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

ಸಂರಚನಾ Curl_str;
ಸಂರಚನಾ hostname;

bool Curl_is_ASCII_name(ಧ್ರುವ ವರ್ಣ *hostname);
bool Curl_is_ASCII_str(ಸಂರಚನಾ Curl_str *s);

CURLcode Curl_idnconvert_hostname(ಸಂರಚನಾ hostname *host);

#ಯದಿ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
ಶೂನ್ಯ Curl_free_idnconverted_hostname(ಸಂರಚನಾ hostname *host);
CURLcode Curl_idn_decode(ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು, ವರ್ಣ **output);
CURLcode Curl_idn_encode(ಧ್ರುವ ವರ್ಣ *puny, ವರ್ಣ **output);
#ಅನ್ಯಥಾ
#define Curl_free_idnconverted_hostname(x)
#define Curl_idn_decode(x) ರಿಕ್ತ
#endif

#endif /* HEADER_CURL_IDN_H */
