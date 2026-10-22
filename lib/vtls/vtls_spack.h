#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_VTLS_SPACK_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_VTLS_SPACK_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(USE_SSL) && defined(USE_SSLS_EXPORT)

ਬਣਤਰ dynbuf;
ਬਣਤਰ Curl_ssl_session;

CURLcode Curl_ssl_session_pack(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_ssl_session *s,
                               ਬਣਤਰ dynbuf *buf);

CURLcode Curl_ssl_session_unpack(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਖਾਲੀ *bufv, size_t buflen,
                                 ਬਣਤਰ Curl_ssl_session **ps);

#ਜੇ_ਅੰਤ /* USE_SSL && USE_SSLS_EXPORT */

#ਜੇ_ਅੰਤ /* HEADER_CURL_VTLS_SPACK_H */
