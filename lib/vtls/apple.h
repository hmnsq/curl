#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_VTLS_APPLE_H
#وصف_ڪريو سرو_CURL_VTLS_APPLE_H
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
#ضمن "curl_setup.h"

#جيڪڏهن_وصف_ٿيل USE_APPLE_SECTRUST
بنية Curl_cfilter;
بنية Curl_easy;
بنية ssl_peer;

/* Get the DER encoded i-th certificate in the server handshake */
تعريفنوع CURLcode Curl_vtls_get_cert_der(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        فراغ *user_معلومات,
                                        size_t i,
                                        بنانشان حرف **pder,
                                        size_t *pder_len);

/* Ask Apple's Security framework to verify the certificate chain
 * send by the peer. On CURLE_OK it has been verified.
 */
CURLcode Curl_vtls_apple_verify(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                بنية ssl_peer *peer,
                                size_t انگ_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                فراغ *cb_user_معلومات,
                                ثابت بنانشان حرف *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp);

bool Curl_vtls_apple_verifies_ocsp(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات);

#جيڪڏهن_پورو /* USE_APPLE_SECTRUST */

#جيڪڏهن_پورو /* HEADER_CURL_VTLS_APPLE_H */
