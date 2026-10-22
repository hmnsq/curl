#ifndef HEADER_CURL_SETOPT_H
#define HEADER_CURL_SETOPT_H
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

#ifdef USE_SSL
CURLcode Curl_setopt_SSLVERSION(কাঠামো Curl_easy *তথ্য, CURLoption option,
                                long arg);
#অন্যথা
#define Curl_setopt_SSLVERSION(a, b, c) CURLE_NOT_BUILT_IN
#endif

CURLcode Curl_setstropt(কাঠামো Curl_easy *তথ্য,
                        গণনা dupstring id, ধ্রুবক অক্ষর *s) WARN_UNUSED_ফলাফল;
CURLcode Curl_setblobopt(কাঠামো curl_blob **blobp,
                         ধ্রুবক কাঠামো curl_blob *blob) WARN_UNUSED_ফলাফল;
CURLcode Curl_vsetopt(কাঠামো Curl_easy *তথ্য, CURLoption option, va_list param)
  WARN_UNUSED_ফলাফল;

#endif /* HEADER_CURL_SETOPT_H */
