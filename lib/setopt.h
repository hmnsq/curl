#నిర్వచితం_కాకపోతే శీర్షిక_CURL_SETOPT_H
#నిర్వచించు శీర్షిక_CURL_SETOPT_H
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

#నిర్వచితమైతే USE_SSL
CURLcode Curl_setopt_SSLVERSION(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option,
                                దీర్ఘ arg);
#లేకపోతే
#నిర్వచించు Curl_setopt_SSLVERSION(a, b, c) CURLE_NOT_BUILT_లోపల
#అయితే_ముగింపు

CURLcode Curl_setstropt(నిర్మాణం Curl_easy *దత్తాంశం,
                        గణన dupstring id, స్థిరరాశి అక్షరం *s) WARN_వాడని_ఫలితం;
CURLcode Curl_setblobopt(నిర్మాణం curl_blob **blobp,
                         స్థిరరాశి నిర్మాణం curl_blob *blob) WARN_వాడని_ఫలితం;
CURLcode Curl_vsetopt(నిర్మాణం Curl_easy *దత్తాంశం, CURLoption option, va_list param)
  WARN_వాడని_ఫలితం;

#అయితే_ముగింపు /* HEADER_CURL_SETOPT_H */
