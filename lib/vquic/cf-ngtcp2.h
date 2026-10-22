#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_VQUIC_CF_NGTCP2_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_VQUIC_CF_NGTCP2_H
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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_UDP_H
#ಅನ್ತರ್ಭಾವ <netinet/udp.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSSL_QUIC_API2
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_ossl.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <nghttp3/nghttp3.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/ssl.h>
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/ssl.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/quic.h>
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ Curl_cfilter;

#ಅನ್ತರ್ಭಾವ "urldata.h"

CURLcode Curl_cf_ngtcp2_create(ರಚನೆ Curl_cfilter **pcf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_peer *origin,
                               ರಚನೆ Curl_peer *peer,
                               ರಚನೆ connectdata *conn,
                               ರಚನೆ Curl_sockaddr_ex *addr);

CURLcode Curl_cf_ngtcp2_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                     ರಚನೆ Curl_peer *origin,
                                     ರಚನೆ Curl_peer *peer,
                                     ರಚನೆ Curl_cfilter **pcf);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_VQUIC_CF_NGTCP2_H */
