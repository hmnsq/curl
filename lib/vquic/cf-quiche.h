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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)

#അന്തര്ഭാവ <quiche.h>
#അന്തര്ഭാവ <openssl/ssl.h>

ഘടന Curl_cfilter;
ഘടന Curl_easy;

ശൂന്യം Curl_quiche_ver(അക്ഷരം *p, size_t len);

CURLcode Curl_cf_quiche_create(ഘടന Curl_cfilter **pcf,
                               ഘടന Curl_easy *data,
                               ഘടന Curl_peer *origin,
                               ഘടന Curl_peer *peer,
                               ഘടന connectdata *conn,
                               ഘടന Curl_sockaddr_ex *addr);
CURLcode Curl_cf_quiche_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                     ഘടന Curl_peer *origin,
                                     ഘടന Curl_peer *peer,
                                     ഘടന Curl_cfilter **pcf);

#endif

#endif /* HEADER_CURL_VQUIC_CF_QUICHE_H */
