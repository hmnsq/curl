#यदि_अपरिभाषितम् शीर्षकम्_CURL_BASE64_H
#परिभाषय शीर्षकम्_CURL_BASE64_H
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

CURLcode curlx_base64_encode(ध्रुव uint8_t *inputbuff, size_t insize,
                             वर्ण **outptr, size_t *outlen);
CURLcode curlx_base64url_encode(ध्रुव uint8_t *inputbuff, size_t insize,
                                वर्ण **outptr, size_t *outlen);
CURLcode curlx_base64_decode(ध्रुव वर्ण *src,
                             uint8_t **outptr, size_t *outlen);

बाह्य ध्रुव वर्ण curlx_base64encdec[];

/* maximum input length acceptable to base64 encode, here to catch and prevent
   mistakes */
#परिभाषय CURL_MAX_BASE64_निवेशः 16000000

#यदि_अन्तः /* HEADER_CURL_BASE64_H */
