#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_RAND_H
#وصف_ڪريو سرو_CURL_RAND_H
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
بنية Curl_easy;

CURLcode Curl_rand_بائيٽون(بنية Curl_easy *معلومات,
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
                         bool env_override,
#جيڪڏهن_پورو
                         بنانشان حرف *rnd, size_t انگ);

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
#وصف_ڪريو Curl_rand(a, b, c) Curl_rand_بائيٽون(a, TRUE, b, c)
#وإلا
#وصف_ڪريو Curl_rand(a, b, c) Curl_rand_بائيٽون(a, b, c)
#جيڪڏهن_پورو

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */
CURLcode Curl_rand_hex(بنية Curl_easy *معلومات, بنانشان حرف *rnd, size_t انگ);

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */
CURLcode Curl_rand_alnum(بنية Curl_easy *معلومات, بنانشان حرف *rnd,
                         size_t انگ);

#جيڪڏهن_پورو /* HEADER_CURL_RAND_H */
