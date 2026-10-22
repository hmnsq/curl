#यदि_अपरिभाषितम् शीर्षकम्_CURL_EASYIF_H
#परिभाषय शीर्षकम्_CURL_EASYIF_H
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
CURLcode Curl_senddata(संरचना Curl_easy *दत्त, ध्रुव शून्य *बफर,
                       size_t buflen, size_t *n);

#यदि_अपरिभाषितम् CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(संरचना Curl_easy *दत्त);
#यदि_अन्तः

#यदि_परिभाषितम् DEBUGBUILD
CURL_EXTERN CURLcode curl_easy_perform_ev(संरचना Curl_easy *easy);
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_EASYIF_H */
