#اگر_غیر_تعریف_شدہ سرخی_CURL_RAND_H
#تعریف_کرو سرخی_CURL_RAND_H
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
ساخت Curl_easy;

CURLcode Curl_rand_بائٹیں(ساخت Curl_easy *ڈیٹا,
#اگر_تعریف_شدہ DEBUGBUILD
                         bool env_override,
#اگر_ختم
                         غیرعلامتی حرف *rnd, size_t عدد);

#اگر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو Curl_rand(a, b, c) Curl_rand_بائٹیں(a, TRUE, b, c)
#ورنہ
#تعریف_کرو Curl_rand(a, b, c) Curl_rand_بائٹیں(a, b, c)
#اگر_ختم

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */
CURLcode Curl_rand_hex(ساخت Curl_easy *ڈیٹا, غیرعلامتی حرف *rnd, size_t عدد);

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */
CURLcode Curl_rand_alnum(ساخت Curl_easy *ڈیٹا, غیرعلامتی حرف *rnd,
                         size_t عدد);

#اگر_ختم /* HEADER_CURL_RAND_H */
