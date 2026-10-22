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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)

#ଅନ୍ତର୍ଭାଵ <quiche.h>
#ଅନ୍ତର୍ଭାଵ <openssl/ssl.h>

ସଂରଚନା Curl_cfilter;
ସଂରଚନା Curl_easy;

ଶୂନ୍ୟ Curl_quiche_ver(ଅକ୍ଷର *p, size_t len);

CURLcode Curl_cf_quiche_create(ସଂରଚନା Curl_cfilter **pcf,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_peer *origin,
                               ସଂରଚନା Curl_peer *peer,
                               ସଂରଚନା connectdata *conn,
                               ସଂରଚନା Curl_sockaddr_ex *addr);
CURLcode Curl_cf_quiche_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                     ସଂରଚନା Curl_peer *origin,
                                     ସଂରଚନା Curl_peer *peer,
                                     ସଂରଚନା Curl_cfilter **pcf);

#endif

#endif /* HEADER_CURL_VQUIC_CF_QUICHE_H */
