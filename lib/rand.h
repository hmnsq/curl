#నిర్వచితం_కాకపోతే శీర్షిక_CURL_RAND_H
#నిర్వచించు శీర్షిక_CURL_RAND_H
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
నిర్మాణం Curl_easy;

CURLcode Curl_rand_బైట్లు(నిర్మాణం Curl_easy *దత్తాంశం,
#నిర్వచితమైతే DEBUGBUILD
                         bool env_override,
#అయితే_ముగింపు
                         గుర్తులేని అక్షరం *rnd, size_t అంకె);

#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు Curl_rand(a, b, c) Curl_rand_బైట్లు(a, TRUE, b, c)
#లేకపోతే
#నిర్వచించు Curl_rand(a, b, c) Curl_rand_బైట్లు(a, b, c)
#అయితే_ముగింపు

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */
CURLcode Curl_rand_hex(నిర్మాణం Curl_easy *దత్తాంశం, గుర్తులేని అక్షరం *rnd, size_t అంకె);

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */
CURLcode Curl_rand_alnum(నిర్మాణం Curl_easy *దత్తాంశం, గుర్తులేని అక్షరం *rnd,
                         size_t అంకె);

#అయితే_ముగింపు /* HEADER_CURL_RAND_H */
