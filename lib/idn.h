#اگر_غیر_تعریف_شدہ سرخی_CURL_IDN_H
#تعریف_کرو سرخی_CURL_IDN_H
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

ساخت Curl_str;
ساخت hostname;

bool Curl_is_ASCII_name(ثابت حرف *hostname);
bool Curl_is_ASCII_str(ساخت Curl_str *s);

CURLcode Curl_idnconvert_hostname(ساخت hostname *host);

#اگر defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#تعریف_کرو USE_IDN
خالی Curl_free_idnconverted_hostname(ساخت hostname *host);
CURLcode Curl_idn_decode(ثابت حرف *داخلہ, حرف **output);
CURLcode Curl_idn_encode(ثابت حرف *puny, حرف **output);
#ورنہ
#تعریف_کرو Curl_free_idnconverted_hostname(x)
#تعریف_کرو Curl_idn_decode(x) تہی
#اگر_ختم

#اگر_ختم /* HEADER_CURL_IDN_H */
