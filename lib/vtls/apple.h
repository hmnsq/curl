#ifndef HEADER_CURL_VTLS_APPLE_H
#define HEADER_CURL_VTLS_APPLE_H
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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_APPLE_SECTRUST
ഘടന Curl_cfilter;
ഘടന Curl_easy;
ഘടന ssl_peer;

/* Get the DER encoded i-th certificate in the server handshake */
തരനിർവചനം CURLcode Curl_vtls_get_cert_der(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        ശൂന്യം *user_data,
                                        size_t i,
                                        unsigned അക്ഷരം **pder,
                                        size_t *pder_len);

/* Ask Apple's Security framework to verify the certificate chain
 * send by the peer. On CURLE_OK it has been verified.
 */
CURLcode Curl_vtls_apple_verify(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                ഘടന ssl_peer *peer,
                                size_t അക്കം_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                ശൂന്യം *cb_user_data,
                                സ്ഥിരം unsigned അക്ഷരം *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp);

bool Curl_vtls_apple_verifies_ocsp(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data);

#endif /* USE_APPLE_SECTRUST */

#endif /* HEADER_CURL_VTLS_APPLE_H */
