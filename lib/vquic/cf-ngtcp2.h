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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ifdef HAVE_NETINET_UDP_H
#அந்தர்பாவ <netinet/udp.h>
#endif

#அந்தர்பாவ <ngtcp2/ngtcp2_crypto.h>
#ifdef OPENSSL_QUIC_API2
#அந்தர்பாவ <ngtcp2/ngtcp2_crypto_ossl.h>
#endif
#அந்தர்பாவ <nghttp3/nghttp3.h>
#ifdef USE_OPENSSL
#அந்தர்பாவ <openssl/ssl.h>
#அல்லதெனில் defined(USE_WOLFSSL)
#அந்தர்பாவ <wolfssl/options.h>
#அந்தர்பாவ <wolfssl/ssl.h>
#அந்தர்பாவ <wolfssl/quic.h>
#endif

கட்டமைப்பு Curl_cfilter;

#அந்தர்பாவ "urldata.h"

CURLcode Curl_cf_ngtcp2_create(கட்டமைப்பு Curl_cfilter **pcf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_peer *origin,
                               கட்டமைப்பு Curl_peer *peer,
                               கட்டமைப்பு connectdata *conn,
                               கட்டமைப்பு Curl_sockaddr_ex *addr);

CURLcode Curl_cf_ngtcp2_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                     கட்டமைப்பு Curl_peer *origin,
                                     கட்டமைப்பு Curl_peer *peer,
                                     கட்டமைப்பு Curl_cfilter **pcf);
#endif

#endif /* HEADER_CURL_VQUIC_CF_NGTCP2_H */
