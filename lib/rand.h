#अगर_अघोषित HEADER_CURL_RAND_H
#घोषणा HEADER_CURL_RAND_H
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
काष्ठा Curl_easy;

CURLcode Curl_rand_bytes(काष्ठा Curl_easy *तथ्य,
#अगर_घोषित DEBUGBUILD
                         bool env_override,
#पूर्ण_अगर
                         अचिन्हित अक्षर *rnd, माप_प्रकार अंक);

#अगर_घोषित DEBUGBUILD
#घोषणा Curl_rand(a, b, c) Curl_rand_bytes(a, TRUE, b, c)
#अन्यथा
#घोषणा Curl_rand(a, b, c) Curl_rand_bytes(a, b, c)
#पूर्ण_अगर

/*
 * Curl_rand_hex() fills the 'rnd' buffer with a given 'num' size with random
 * hexadecimal digits PLUS a null-terminator byte. It must be an odd number
 * size.
 */
CURLcode Curl_rand_hex(काष्ठा Curl_easy *तथ्य, अचिन्हित अक्षर *rnd, माप_प्रकार अंक);

/*
 * Curl_rand_alnum() fills the 'rnd' buffer with a given 'num' size with random
 * alphanumerical chars PLUS a null-terminator byte.
 */
CURLcode Curl_rand_alnum(काष्ठा Curl_easy *तथ्य, अचिन्हित अक्षर *rnd,
                         माप_प्रकार अंक);

#पूर्ण_अगर /* HEADER_CURL_RAND_H */
