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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_APPLE_SECTRUST
સંરચના Curl_cfilter;
સંરચના Curl_easy;
સંરચના ssl_peer;

/* Get the DER encoded i-th certificate in the server handshake */
પ્રકારવ્યાખ્યા CURLcode Curl_vtls_get_cert_der(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        શૂન્ય *user_data,
                                        size_t i,
                                        unsigned અક્ષર **pder,
                                        size_t *pder_len);

/* Ask Apple's Security framework to verify the certificate chain
 * send by the peer. On CURLE_OK it has been verified.
 */
CURLcode Curl_vtls_apple_verify(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                સંરચના ssl_peer *peer,
                                size_t અંક_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                શૂન્ય *cb_user_data,
                                અચલ unsigned અક્ષર *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp);

bool Curl_vtls_apple_verifies_ocsp(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data);

#endif /* USE_APPLE_SECTRUST */

#endif /* HEADER_CURL_VTLS_APPLE_H */
