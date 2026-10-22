#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_EASYIF_H
#وصف_ڪريو سرو_CURL_EASYIF_H
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
CURLcode Curl_senddata(بنية Curl_easy *معلومات, ثابت فراغ *بفر,
                       size_t buflen, size_t *n);

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(بنية Curl_easy *معلومات);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
CURL_EXTERN CURLcode curl_easy_perform_ev(بنية Curl_easy *easy);
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_EASYIF_H */
