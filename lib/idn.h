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

ഘടന Curl_str;
ഘടന hostname;

bool Curl_is_ASCII_name(സ്ഥിരം അക്ഷരം *hostname);
bool Curl_is_ASCII_str(ഘടന Curl_str *s);

CURLcode Curl_idnconvert_hostname(ഘടന hostname *host);

#എങ്കിൽ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
ശൂന്യം Curl_free_idnconverted_hostname(ഘടന hostname *host);
CURLcode Curl_idn_decode(സ്ഥിരം അക്ഷരം *നിവേശം, അക്ഷരം **output);
CURLcode Curl_idn_encode(സ്ഥിരം അക്ഷരം *puny, അക്ഷരം **output);
#അല്ലെങ്കിൽ
#define Curl_free_idnconverted_hostname(x)
#define Curl_idn_decode(x) ശൂന്യ
#endif

#endif /* HEADER_CURL_IDN_H */
