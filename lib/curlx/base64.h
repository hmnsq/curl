#अगर_अघोषित HEADER_CURL_BASE64_H
#घोषणा HEADER_CURL_BASE64_H
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

CURLcode curlx_base64_encode(स्थिर uint8_t *inputbuff, माप_प्रकार insize,
                             अक्षर **outptr, माप_प्रकार *outlen);
CURLcode curlx_base64url_encode(स्थिर uint8_t *inputbuff, माप_प्रकार insize,
                                अक्षर **outptr, माप_प्रकार *outlen);
CURLcode curlx_base64_decode(स्थिर अक्षर *src,
                             uint8_t **outptr, माप_प्रकार *outlen);

बाह्य स्थिर अक्षर curlx_base64encdec[];

/* maximum input length acceptable to base64 encode, here to catch and prevent
   mistakes */
#घोषणा CURL_MAX_BASE64_निवेश 16000000

#पूर्ण_अगर /* HEADER_CURL_BASE64_H */
