#నిర్వచితం_కాకపోతే శీర్షిక_CURL_BASE64_H
#నిర్వచించు శీర్షిక_CURL_BASE64_H
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

CURLcode curlx_base64_encode(స్థిరరాశి uint8_t *inputbuff, size_t insize,
                             అక్షరం **outptr, size_t *outlen);
CURLcode curlx_base64url_encode(స్థిరరాశి uint8_t *inputbuff, size_t insize,
                                అక్షరం **outptr, size_t *outlen);
CURLcode curlx_base64_decode(స్థిరరాశి అక్షరం *src,
                             uint8_t **outptr, size_t *outlen);

బాహ్య స్థిరరాశి అక్షరం curlx_base64encdec[];

/* maximum input length acceptable to base64 encode, here to catch and prevent
   mistakes */
#నిర్వచించు CURL_MAX_BASE64_ఆదానం 16000000

#అయితే_ముగింపు /* HEADER_CURL_BASE64_H */
