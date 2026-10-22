#ifndef HEADER_CURL_VTLS_SPACK_H
#define HEADER_CURL_VTLS_SPACK_H
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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି defined(USE_SSL) && defined(USE_SSLS_EXPORT)

ସଂରଚନା dynbuf;
ସଂରଚନା Curl_ssl_session;

CURLcode Curl_ssl_session_pack(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_ssl_session *s,
                               ସଂରଚନା dynbuf *buf);

CURLcode Curl_ssl_session_unpack(ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ ଶୂନ୍ୟ *bufv, size_t buflen,
                                 ସଂରଚନା Curl_ssl_session **ps);

#endif /* USE_SSL && USE_SSLS_EXPORT */

#endif /* HEADER_CURL_VTLS_SPACK_H */
