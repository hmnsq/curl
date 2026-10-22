#ifndef HEADER_CURL_VQUIC_CF_NGTCP2_H
#define HEADER_CURL_VQUIC_CF_NGTCP2_H
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

#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ifdef HAVE_NETINET_UDP_H
#ଅନ୍ତର୍ଭାଵ <netinet/udp.h>
#endif

#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2_crypto.h>
#ifdef OPENSSL_QUIC_API2
#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2_crypto_ossl.h>
#endif
#ଅନ୍ତର୍ଭାଵ <nghttp3/nghttp3.h>
#ifdef USE_OPENSSL
#ଅନ୍ତର୍ଭାଵ <openssl/ssl.h>
#ନଚେତଯଦି defined(USE_WOLFSSL)
#ଅନ୍ତର୍ଭାଵ <wolfssl/options.h>
#ଅନ୍ତର୍ଭାଵ <wolfssl/ssl.h>
#ଅନ୍ତର୍ଭାଵ <wolfssl/quic.h>
#endif

ସଂରଚନା Curl_cfilter;

#ଅନ୍ତର୍ଭାଵ "urldata.h"

CURLcode Curl_cf_ngtcp2_create(ସଂରଚନା Curl_cfilter **pcf,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_peer *origin,
                               ସଂରଚନା Curl_peer *peer,
                               ସଂରଚନା connectdata *conn,
                               ସଂରଚନା Curl_sockaddr_ex *addr);

CURLcode Curl_cf_ngtcp2_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                     ସଂରଚନା Curl_peer *origin,
                                     ସଂରଚନା Curl_peer *peer,
                                     ସଂରଚନା Curl_cfilter **pcf);
#endif

#endif /* HEADER_CURL_VQUIC_CF_NGTCP2_H */
