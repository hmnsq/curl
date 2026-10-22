#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_VTLS_APPLE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_VTLS_APPLE_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Jan Venekamp, <jan@venekamp.net>
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
ਬਣਤਰ Curl_cfilter;
ਬਣਤਰ Curl_easy;
ਬਣਤਰ ssl_peer;

/* Get the DER encoded i-th certificate in the server handshake */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode Curl_vtls_get_cert_der(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਖਾਲੀ *user_ਅੰਕੜੇ,
                                        size_t i,
                                        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **pder,
                                        size_t *pder_len);

/* Ask Apple's Security framework to verify the certificate chain
 * send by the peer. On CURLE_OK it has been verified.
 */
CURLcode Curl_vtls_apple_verify(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ ssl_peer *peer,
                                size_t ਅੰਕ_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                ਖਾਲੀ *cb_user_ਅੰਕੜੇ,
                                ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp);

bool Curl_vtls_apple_verifies_ocsp(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

#ਜੇ_ਅੰਤ /* USE_APPLE_SECTRUST */

#ਜੇ_ਅੰਤ /* HEADER_CURL_VTLS_APPLE_H */
