#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_SETOPT_H
#وصف_ڪريو سرو_CURL_SETOPT_H
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

#جيڪڏهن_وصف_ٿيل USE_SSL
CURLcode Curl_setopt_SSLVERSION(بنية Curl_easy *معلومات, CURLoption option,
                                ڊگهو arg);
#وإلا
#وصف_ڪريو Curl_setopt_SSLVERSION(a, b, c) CURLE_NOT_BUILT_اندريون
#جيڪڏهن_پورو

CURLcode Curl_setstropt(بنية Curl_easy *معلومات,
                        تعداد dupstring id, ثابت حرف *s) WARN_اڻ_استعمال_ٿيل_نتيجو;
CURLcode Curl_setblobopt(بنية curl_blob **blobp,
                         ثابت بنية curl_blob *blob) WARN_اڻ_استعمال_ٿيل_نتيجو;
CURLcode Curl_vsetopt(بنية Curl_easy *معلومات, CURLoption option, va_list param)
  WARN_اڻ_استعمال_ٿيل_نتيجو;

#جيڪڏهن_پورو /* HEADER_CURL_SETOPT_H */
