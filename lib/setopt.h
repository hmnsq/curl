#اگر_غیر_تعریف_شدہ سرخی_CURL_SETOPT_H
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

#اگر_تعریف_شدہ USE_SSL
CURLcode Curl_setopt_SSLVERSION(ساخت Curl_easy *ڈیٹا, CURLoption option,
                                طویل arg);
#ورنہ
#تعریف_کرو Curl_setopt_SSLVERSION(a, b, c) CURLE_NOT_BUILT_اندر
#اگر_ختم

CURLcode Curl_setstropt(ساخت Curl_easy *ڈیٹا,
                        شمار dupstring id, ثابت حرف *s) WARN_غیر_مستعمل_نتیجہ;
CURLcode Curl_setblobopt(ساخت curl_blob **blobp,
                         ثابت ساخت curl_blob *blob) WARN_غیر_مستعمل_نتیجہ;
CURLcode Curl_vsetopt(ساخت Curl_easy *ڈیٹا, CURLoption option, va_list param)
  WARN_غیر_مستعمل_نتیجہ;

#اگر_ختم /* HEADER_CURL_SETOPT_H */
