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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_APPLE_SECTRUST
সংরচনা Curl_cfilter;
সংরচনা Curl_easy;
সংরচনা ssl_peer;

/* Get the DER encoded i-th certificate in the server handshake */
প্রকারনির্দেশ CURLcode Curl_vtls_get_cert_der(সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data,
                                        শূন্য *user_data,
                                        size_t i,
                                        unsigned বর্ণ **pder,
                                        size_t *pder_len);

/* Ask Apple's Security framework to verify the certificate chain
 * send by the peer. On CURLE_OK it has been verified.
 */
CURLcode Curl_vtls_apple_verify(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                সংরচনা ssl_peer *peer,
                                size_t অঙ্ক_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                শূন্য *cb_user_data,
                                ধ্রুব unsigned বর্ণ *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp);

bool Curl_vtls_apple_verifies_ocsp(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data);

#endif /* USE_APPLE_SECTRUST */

#endif /* HEADER_CURL_VTLS_APPLE_H */
