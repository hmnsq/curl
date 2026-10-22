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
#ضمن "curl_setup.h"

#𐤀𐤌 !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ifdef HAVE_NETINET_UDP_H
#ضمن <netinet/udp.h>
#endif

#ضمن <ngtcp2/ngtcp2_crypto.h>
#ifdef OPENSSL_QUIC_API2
#ضمن <ngtcp2/ngtcp2_crypto_ossl.h>
#endif
#ضمن <nghttp3/nghttp3.h>
#ifdef USE_OPENSSL
#ضمن <openssl/ssl.h>
#وإلاإذا defined(USE_WOLFSSL)
#ضمن <wolfssl/options.h>
#ضمن <wolfssl/ssl.h>
#ضمن <wolfssl/quic.h>
#endif

بنية Curl_cfilter;

#ضمن "urldata.h"

CURLcode Curl_cf_ngtcp2_create(بنية Curl_cfilter **pcf,
                               بنية Curl_easy *data,
                               بنية Curl_peer *origin,
                               بنية Curl_peer *peer,
                               بنية connectdata *conn,
                               بنية Curl_sockaddr_ex *addr);

CURLcode Curl_cf_ngtcp2_insert_after(بنية Curl_cfilter *cf_at,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     بنية Curl_cfilter **pcf);
#endif

#endif /* HEADER_CURL_VQUIC_CF_NGTCP2_H */
