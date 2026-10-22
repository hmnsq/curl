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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_APPLE_SECTRUST
संरचना Curl_cfilter;
संरचना Curl_easy;
संरचना ssl_peer;

/* Get the DER encoded i-th certificate in the server handshake */
प्रकारनिर्देश CURLcode Curl_vtls_get_cert_der(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        शून्य *user_data,
                                        माप_प्रकार i,
                                        अचिन्हित वर्ण **pder,
                                        माप_प्रकार *pder_len);

/* Ask Apple's Security framework to verify the certificate chain
 * send by the peer. On CURLE_OK it has been verified.
 */
CURLcode Curl_vtls_apple_verify(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                संरचना ssl_peer *peer,
                                माप_प्रकार अंक_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                शून्य *cb_user_data,
                                ध्रुव अचिन्हित वर्ण *ocsp_buf,
                                माप_प्रकार ocsp_len,
                                bool require_ocsp);

bool Curl_vtls_apple_verifies_ocsp(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data);

#endif /* USE_APPLE_SECTRUST */

#endif /* HEADER_CURL_VTLS_APPLE_H */
