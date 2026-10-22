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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)

#અન્તર્ભાવ <quiche.h>
#અન્તર્ભાવ <openssl/ssl.h>

સંરચના Curl_cfilter;
સંરચના Curl_easy;

શૂન્ય Curl_quiche_ver(અક્ષર *p, size_t len);

CURLcode Curl_cf_quiche_create(સંરચના Curl_cfilter **pcf,
                               સંરચના Curl_easy *data,
                               સંરચના Curl_peer *origin,
                               સંરચના Curl_peer *peer,
                               સંરચના connectdata *conn,
                               સંરચના Curl_sockaddr_ex *addr);
CURLcode Curl_cf_quiche_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                     સંરચના Curl_peer *origin,
                                     સંરચના Curl_peer *peer,
                                     સંરચના Curl_cfilter **pcf);

#endif

#endif /* HEADER_CURL_VQUIC_CF_QUICHE_H */
