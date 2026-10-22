#اگر_تعریف_نشده سرآیند_CURL_VQUIC_CF_QUICHE_H
#تعریف_کن سرآیند_CURL_VQUIC_CF_QUICHE_H
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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)

#ضمن <quiche.h>
#ضمن <openssl/ssl.h>

بنية Curl_cfilter;
بنية Curl_easy;

فراغ Curl_quiche_ver(حرف *p, size_t len);

CURLcode Curl_cf_quiche_create(بنية Curl_cfilter **pcf,
                               بنية Curl_easy *داده,
                               بنية Curl_peer *origin,
                               بنية Curl_peer *peer,
                               بنية connectdata *conn,
                               بنية Curl_sockaddr_ex *addr);
CURLcode Curl_cf_quiche_درج_after(بنية Curl_cfilter *cf_at,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     بنية Curl_cfilter **pcf);

#پایان_اگر

#پایان_اگر /* HEADER_CURL_VQUIC_CF_QUICHE_H */
