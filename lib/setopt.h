#अगर_अघोषित HEADER_CURL_SETOPT_H
#घोषणा HEADER_CURL_SETOPT_H
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

#अगर_घोषित USE_SSL
CURLcode Curl_setopt_SSLVERSION(काष्ठा Curl_easy *तथ्य, CURLoption option,
                                दीर्घ arg);
#अन्यथा
#घोषणा Curl_setopt_SSLVERSION(a, b, c) CURLE_NOT_BUILT_IN
#पूर्ण_अगर

CURLcode Curl_setstropt(काष्ठा Curl_easy *तथ्य,
                        क्रमागत dupstring id, स्थिर अक्षर *s) WARN_UNUSED_परिणाम;
CURLcode Curl_setblobopt(काष्ठा curl_blob **blobp,
                         स्थिर काष्ठा curl_blob *blob) WARN_UNUSED_परिणाम;
CURLcode Curl_vsetopt(काष्ठा Curl_easy *तथ्य, CURLoption option, बहु_सूची param)
  WARN_UNUSED_परिणाम;

#पूर्ण_अगर /* HEADER_CURL_SETOPT_H */
