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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ifdef HAVE_NETINET_UDP_H
#ಅನ್ತರ್ಭಾವ <netinet/udp.h>
#endif

#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto.h>
#ifdef OPENSSL_QUIC_API2
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_ossl.h>
#endif
#ಅನ್ತರ್ಭಾವ <nghttp3/nghttp3.h>
#ifdef USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/ssl.h>
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/ssl.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/quic.h>
#endif

ಸಂರಚನಾ Curl_cfilter;

#ಅನ್ತರ್ಭಾವ "urldata.h"

CURLcode Curl_cf_ngtcp2_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_peer *origin,
                               ಸಂರಚನಾ Curl_peer *peer,
                               ಸಂರಚನಾ connectdata *conn,
                               ಸಂರಚನಾ Curl_sockaddr_ex *addr);

CURLcode Curl_cf_ngtcp2_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                     ಸಂರಚನಾ Curl_peer *origin,
                                     ಸಂರಚನಾ Curl_peer *peer,
                                     ಸಂರಚನಾ Curl_cfilter **pcf);
#endif

#endif /* HEADER_CURL_VQUIC_CF_NGTCP2_H */
