#अगर_अघोषित HEADER_CURL_VTLS_APPLE_H
#घोषणा HEADER_CURL_VTLS_APPLE_H
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
#समावेश "curl_setup.h"

#अगर_घोषित USE_APPLE_SECTRUST
काष्ठा Curl_cfilter;
काष्ठा Curl_easy;
काष्ठा ssl_peer;

/* Get the DER encoded i-th certificate in the server handshake */
प्रकार CURLcode Curl_vtls_get_cert_der(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        व्योम *user_तथ्य,
                                        माप_प्रकार i,
                                        अचिन्हित अक्षर **pder,
                                        माप_प्रकार *pder_len);

/* Ask Apple's Security framework to verify the certificate chain
 * send by the peer. On CURLE_OK it has been verified.
 */
CURLcode Curl_vtls_apple_verify(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                काष्ठा ssl_peer *peer,
                                माप_प्रकार अंक_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                व्योम *cb_user_तथ्य,
                                स्थिर अचिन्हित अक्षर *ocsp_buf,
                                माप_प्रकार ocsp_len,
                                bool require_ocsp);

bool Curl_vtls_apple_verifies_ocsp(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य);

#पूर्ण_अगर /* USE_APPLE_SECTRUST */

#पूर्ण_अगर /* HEADER_CURL_VTLS_APPLE_H */
