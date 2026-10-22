#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_RAND_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_RAND_H
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
ਬਣਤਰ Curl_easy;

CURLcode Curl_rand_ਬਾਈਟਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
                         bool env_override,
#ਜੇ_ਅੰਤ
                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *rnd, size_t ਅੰਕ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_rand(a, b, c) Curl_rand_ਬਾਈਟਾਂ(a, TRUE, b, c)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_rand(a, b, c) Curl_rand_ਬਾਈਟਾਂ(a, b, c)
#ਜੇ_ਅੰਤ

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */
CURLcode Curl_rand_hex(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *rnd, size_t ਅੰਕ);

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */
CURLcode Curl_rand_alnum(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *rnd,
                         size_t ਅੰਕ);

#ਜੇ_ਅੰਤ /* HEADER_CURL_RAND_H */
