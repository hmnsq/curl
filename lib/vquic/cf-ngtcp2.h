#اگر_غیر_تعریف_شدہ سرخی_CURL_VQUIC_CF_NGTCP2_H
#تعریف_کرو سرخی_CURL_VQUIC_CF_NGTCP2_H
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

#اگر_تعریف_شدہ HAVE_NETINET_UDP_H
#شامل <netinet/udp.h>
#اگر_ختم

#شامل <ngtcp2/ngtcp2_crypto.h>
#اگر_تعریف_شدہ OPENSSL_QUIC_API2
#شامل <ngtcp2/ngtcp2_crypto_ossl.h>
#اگر_ختم
#شامل <nghttp3/nghttp3.h>
#اگر_تعریف_شدہ USE_OPENSSL
#شامل <openssl/ssl.h>
#وگرنہ_اگر defined(USE_WOLFSSL)
#شامل <wolfssl/options.h>
#شامل <wolfssl/ssl.h>
#شامل <wolfssl/quic.h>
#اگر_ختم

ساخت Curl_cfilter;

#شامل "urldata.h"

CURLcode Curl_cf_ngtcp2_create(ساخت Curl_cfilter **pcf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_peer *origin,
                               ساخت Curl_peer *peer,
                               ساخت connectdata *conn,
                               ساخت Curl_sockaddr_ex *addr);

CURLcode Curl_cf_ngtcp2_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                     ساخت Curl_peer *origin,
                                     ساخت Curl_peer *peer,
                                     ساخت Curl_cfilter **pcf);
#اگر_ختم

#اگر_ختم /* HEADER_CURL_VQUIC_CF_NGTCP2_H */
