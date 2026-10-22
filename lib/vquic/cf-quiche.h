#ifndef HEADER_CURL_VQUIC_CF_QUICHE_H
#define HEADER_CURL_VQUIC_CF_QUICHE_H
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
#অন্তর্ভাৱ "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)

#অন্তর্ভাৱ <quiche.h>
#অন্তর্ভাৱ <openssl/ssl.h>

সংরচনা Curl_cfilter;
সংরচনা Curl_easy;

শূন্য Curl_quiche_ver(ৱর্ণ *p, size_t len);

CURLcode Curl_cf_quiche_create(সংরচনা Curl_cfilter **pcf,
                               সংরচনা Curl_easy *data,
                               সংরচনা Curl_peer *origin,
                               সংরচনা Curl_peer *peer,
                               সংরচনা connectdata *conn,
                               সংরচনা Curl_sockaddr_ex *addr);
CURLcode Curl_cf_quiche_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                     সংরচনা Curl_peer *origin,
                                     সংরচনা Curl_peer *peer,
                                     সংরচনা Curl_cfilter **pcf);

#endif

#endif /* HEADER_CURL_VQUIC_CF_QUICHE_H */
