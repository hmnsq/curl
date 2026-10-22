#यदि_अपरिभाषितम् शीर्षकम्_CURL_SETOPT_H
#परिभाषय शीर्षकम्_CURL_SETOPT_H
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

#यदि_परिभाषितम् USE_SSL
CURLcode Curl_setopt_SSLVERSION(संरचना Curl_easy *दत्त, CURLoption option,
                                दीर्घ arg);
#अन्यथा
#परिभाषय Curl_setopt_SSLVERSION(a, b, c) CURLE_NOT_BUILT_अन्तर्
#यदि_अन्तः

CURLcode Curl_setstropt(संरचना Curl_easy *दत्त,
                        गणना dupstring id, ध्रुव वर्ण *s) WARN_अप्रयुक्तम्_परिणाम;
CURLcode Curl_setblobopt(संरचना curl_blob **blobp,
                         ध्रुव संरचना curl_blob *blob) WARN_अप्रयुक्तम्_परिणाम;
CURLcode Curl_vsetopt(संरचना Curl_easy *दत्त, CURLoption option, va_list param)
  WARN_अप्रयुक्तम्_परिणाम;

#यदि_अन्तः /* HEADER_CURL_SETOPT_H */
