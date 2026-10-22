#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_BASE64_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_BASE64_H
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

CURLcode curlx_base64_encode(ਅਚਲ uint8_t *inputbuff, size_t insize,
                             ਅੱਖਰ **outptr, size_t *outlen);
CURLcode curlx_base64url_encode(ਅਚਲ uint8_t *inputbuff, size_t insize,
                                ਅੱਖਰ **outptr, size_t *outlen);
CURLcode curlx_base64_decode(ਅਚਲ ਅੱਖਰ *src,
                             uint8_t **outptr, size_t *outlen);

ਬਾਹਰੀ ਅਚਲ ਅੱਖਰ curlx_base64encdec[];

/* maximum input length acceptable to base64 encode, here to catch and prevent
   mistakes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MAX_BASE64_ਨਿਵੇਸ਼ 16000000

#ਜੇ_ਅੰਤ /* HEADER_CURL_BASE64_H */
