#यदि_अपरिभाषितम् शीर्षकम्_CURL_RAND_H
#परिभाषय शीर्षकम्_CURL_RAND_H
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
संरचना Curl_easy;

CURLcode Curl_rand_अष्टकानि(संरचना Curl_easy *दत्त,
#यदि_परिभाषितम् DEBUGBUILD
                         bool env_override,
#यदि_अन्तः
                         अचिह्नित वर्ण *rnd, size_t अङ्कः);

#यदि_परिभाषितम् DEBUGBUILD
#परिभाषय Curl_rand(a, b, c) Curl_rand_अष्टकानि(a, TRUE, b, c)
#अन्यथा
#परिभाषय Curl_rand(a, b, c) Curl_rand_अष्टकानि(a, b, c)
#यदि_अन्तः

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */
CURLcode Curl_rand_hex(संरचना Curl_easy *दत्त, अचिह्नित वर्ण *rnd, size_t अङ्कः);

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */
CURLcode Curl_rand_alnum(संरचना Curl_easy *दत्त, अचिह्नित वर्ण *rnd,
                         size_t अङ्कः);

#यदि_अन्तः /* HEADER_CURL_RAND_H */
