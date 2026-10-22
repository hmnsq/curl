#اگر_غیر_تعریف_شدہ سرخی_CURL_VTLS_SPACK_H
#تعریف_کرو سرخی_CURL_VTLS_SPACK_H
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
#شامل "curl_setup.h"

#اگر defined(USE_SSL) && defined(USE_SSLS_EXPORT)

ساخت dynbuf;
ساخت Curl_ssl_session;

CURLcode Curl_ssl_session_pack(ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_ssl_session *s,
                               ساخت dynbuf *buf);

CURLcode Curl_ssl_session_unpack(ساخت Curl_easy *ڈیٹا,
                                 ثابت خالی *bufv, size_t buflen,
                                 ساخت Curl_ssl_session **ps);

#اگر_ختم /* USE_SSL && USE_SSLS_EXPORT */

#اگر_ختم /* HEADER_CURL_VTLS_SPACK_H */
