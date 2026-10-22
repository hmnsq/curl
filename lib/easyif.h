#اگر_تعریف_نشده سرآیند_CURL_EASYIF_H
#تعریف_کن سرآیند_CURL_EASYIF_H
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
/*
 * Prototypes for library-wide functions provided by easy.c
 */
CURLcode Curl_senddata(بنية Curl_easy *داده, ثابت فراغ *میانگیر,
                       size_t buflen, size_t *n);

#اگر_تعریف_نشده CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(بنية Curl_easy *داده);
#پایان_اگر

#اگر_تعریف_شده DEBUGBUILD
CURL_EXTERN CURLcode curl_easy_perform_ev(بنية Curl_easy *easy);
#پایان_اگر

#پایان_اگر /* HEADER_CURL_EASYIF_H */
