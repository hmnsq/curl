#جے_بنا_تعریف سرخی_CURL_SETOPT_H
#تعریف_کرو سرخی_CURL_SETOPT_H
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

#جے_تعریف_شدہ USE_SSL
CURLcode Curl_setopt_SSLVERSION(بنية Curl_easy *انکڑے, CURLoption option,
                                لما arg);
#وإلا
#تعریف_کرو Curl_setopt_SSLVERSION(a, b, c) CURLE_NOT_BUILT_وچ
#جے_انت

CURLcode Curl_setstropt(بنية Curl_easy *انکڑے,
                        تعداد dupstring id, ثابت حرف *s) WARN_بن_ورتیا_نتیجہ;
CURLcode Curl_setblobopt(بنية curl_blob **blobp,
                         ثابت بنية curl_blob *blob) WARN_بن_ورتیا_نتیجہ;
CURLcode Curl_vsetopt(بنية Curl_easy *انکڑے, CURLoption option, va_list param)
  WARN_بن_ورتیا_نتیجہ;

#جے_انت /* HEADER_CURL_SETOPT_H */
