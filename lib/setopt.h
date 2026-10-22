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
CURLcode Curl_setopt_SSLVERSION(সংরচনা Curl_easy *data, CURLoption option,
                                long arg);
#অন্যথা
#define Curl_setopt_SSLVERSION(a, b, c) CURLE_NOT_BUILT_IN
#endif

CURLcode Curl_setstropt(সংরচনা Curl_easy *data,
                        গণনা dupstring id, ধ্রুব বর্ণ *s) WARN_UNUSED_RESULT;
CURLcode Curl_setblobopt(সংরচনা curl_blob **blobp,
                         ধ্রুব সংরচনা curl_blob *blob) WARN_UNUSED_RESULT;
CURLcode Curl_vsetopt(সংরচনা Curl_easy *data, CURLoption option, va_list param)
  WARN_UNUSED_RESULT;

#endif /* HEADER_CURL_SETOPT_H */
