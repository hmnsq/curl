#अगर_अघोषित HEADER_CURL_IDN_H
#घोषणा HEADER_CURL_IDN_H
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

काष्ठा Curl_str;
काष्ठा hostname;

bool Curl_is_ASCII_name(स्थिर अक्षर *hostname);
bool Curl_is_ASCII_str(काष्ठा Curl_str *s);

CURLcode Curl_idnconvert_hostname(काष्ठा hostname *host);

#अगर defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#घोषणा USE_IDN
व्योम Curl_free_idnconverted_hostname(काष्ठा hostname *host);
CURLcode Curl_idn_decode(स्थिर अक्षर *निवेश, अक्षर **output);
CURLcode Curl_idn_encode(स्थिर अक्षर *puny, अक्षर **output);
#अन्यथा
#घोषणा Curl_free_idnconverted_hostname(x)
#घोषणा Curl_idn_decode(x) शून्य
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_IDN_H */
