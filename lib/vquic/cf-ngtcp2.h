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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ifdef HAVE_NETINET_UDP_H
#شامل <netinet/udp.h>
#endif

#شامل <ngtcp2/ngtcp2_crypto.h>
#ifdef OPENSSL_QUIC_API2
#شامل <ngtcp2/ngtcp2_crypto_ossl.h>
#endif
#شامل <nghttp3/nghttp3.h>
#ifdef USE_OPENSSL
#شامل <openssl/ssl.h>
#وگرنهاگر defined(USE_WOLFSSL)
#شامل <wolfssl/options.h>
#شامل <wolfssl/ssl.h>
#شامل <wolfssl/quic.h>
#endif

ساختار Curl_cfilter;

#شامل "urldata.h"

CURLcode Curl_cf_ngtcp2_create(ساختار Curl_cfilter **pcf,
                               ساختار Curl_easy *data,
                               ساختار Curl_peer *origin,
                               ساختار Curl_peer *peer,
                               ساختار connectdata *conn,
                               ساختار Curl_sockaddr_ex *addr);

CURLcode Curl_cf_ngtcp2_درج_after(ساختار Curl_cfilter *cf_at,
                                     ساختار Curl_peer *origin,
                                     ساختار Curl_peer *peer,
                                     ساختار Curl_cfilter **pcf);
#endif

#endif /* HEADER_CURL_VQUIC_CF_NGTCP2_H */
