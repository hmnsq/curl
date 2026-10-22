#که_نه_تعریف_شوی سرلیک_CURL_SETOPT_H
#تعریف_کړه سرلیک_CURL_SETOPT_H
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

#که_تعریف_شوی USE_SSL
CURLcode Curl_setopt_SSLVERSION(بنية Curl_easy *معلومات, CURLoption option,
                                اوږد arg);
#وإلا
#تعریف_کړه Curl_setopt_SSLVERSION(a, b, c) CURLE_NOT_BUILT_دننه
#د_که_پای

CURLcode Curl_setstropt(بنية Curl_easy *معلومات,
                        تعداد dupstring id, ثابت حرف *s) WARN_ناکارول_شوی_پایله;
CURLcode Curl_setblobopt(بنية curl_blob **blobp,
                         ثابت بنية curl_blob *blob) WARN_ناکارول_شوی_پایله;
CURLcode Curl_vsetopt(بنية Curl_easy *معلومات, CURLoption option, va_list param)
  WARN_ناکارول_شوی_پایله;

#د_که_پای /* HEADER_CURL_SETOPT_H */
