#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_VQUIC_CF_NGTCP2_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_VQUIC_CF_NGTCP2_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_UDP_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/udp.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2_crypto.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSSL_QUIC_API2
#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2_crypto_ossl.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <nghttp3/nghttp3.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/ssl.h>
#ਜਾਂਜੇ defined(USE_WOLFSSL)
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/options.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/ssl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/quic.h>
#ਜੇ_ਅੰਤ

ਬਣਤਰ Curl_cfilter;

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

CURLcode Curl_cf_ngtcp2_create(ਬਣਤਰ Curl_cfilter **pcf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_peer *origin,
                               ਬਣਤਰ Curl_peer *peer,
                               ਬਣਤਰ connectdata *conn,
                               ਬਣਤਰ Curl_sockaddr_ex *addr);

CURLcode Curl_cf_ngtcp2_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                     ਬਣਤਰ Curl_peer *origin,
                                     ਬਣਤਰ Curl_peer *peer,
                                     ਬਣਤਰ Curl_cfilter **pcf);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_VQUIC_CF_NGTCP2_H */
