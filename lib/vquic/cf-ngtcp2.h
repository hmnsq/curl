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
#অন্তর্ভাৱ "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ifdef HAVE_NETINET_UDP_H
#অন্তর্ভাৱ <netinet/udp.h>
#endif

#অন্তর্ভাৱ <ngtcp2/ngtcp2_crypto.h>
#ifdef OPENSSL_QUIC_API2
#অন্তর্ভাৱ <ngtcp2/ngtcp2_crypto_ossl.h>
#endif
#অন্তর্ভাৱ <nghttp3/nghttp3.h>
#ifdef USE_OPENSSL
#অন্তর্ভাৱ <openssl/ssl.h>
#নোচেদ্যদি defined(USE_WOLFSSL)
#অন্তর্ভাৱ <wolfssl/options.h>
#অন্তর্ভাৱ <wolfssl/ssl.h>
#অন্তর্ভাৱ <wolfssl/quic.h>
#endif

সংরচনা Curl_cfilter;

#অন্তর্ভাৱ "urldata.h"

CURLcode Curl_cf_ngtcp2_create(সংরচনা Curl_cfilter **pcf,
                               সংরচনা Curl_easy *data,
                               সংরচনা Curl_peer *origin,
                               সংরচনা Curl_peer *peer,
                               সংরচনা connectdata *conn,
                               সংরচনা Curl_sockaddr_ex *addr);

CURLcode Curl_cf_ngtcp2_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                     সংরচনা Curl_peer *origin,
                                     সংরচনা Curl_peer *peer,
                                     সংরচনা Curl_cfilter **pcf);
#endif

#endif /* HEADER_CURL_VQUIC_CF_NGTCP2_H */
