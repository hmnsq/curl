#अगर_अघोषित HEADER_CURL_EASYIF_H
#घोषणा HEADER_CURL_EASYIF_H
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
/*
 * Prototypes for library-wide functions provided by easy.c
 */
CURLcode Curl_senddata(काष्ठा Curl_easy *तथ्य, स्थिर व्योम *बफर,
                       माप_प्रकार buflen, माप_प्रकार *n);

#अगर_अघोषित CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(काष्ठा Curl_easy *तथ्य);
#पूर्ण_अगर

#अगर_घोषित DEBUGBUILD
CURL_EXTERN CURLcode curl_easy_perform_ev(काष्ठा Curl_easy *easy);
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_EASYIF_H */
