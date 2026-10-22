#جے_بنا_تعریف سرخی_CURL_VTLS_APPLE_H
#تعریف_کرو سرخی_CURL_VTLS_APPLE_H
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

#جے_تعریف_شدہ USE_APPLE_SECTRUST
بنية Curl_cfilter;
بنية Curl_easy;
بنية ssl_peer;

/* Get the DER encoded i-th certificate in the server handshake */
تعريفنوع CURLcode Curl_vtls_get_cert_der(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *انکڑے,
                                        فراغ *user_انکڑے,
                                        size_t i,
                                        بنانشانی حرف **pder,
                                        size_t *pder_len);

/* Ask Apple's Security framework to verify the certificate chain
 * send by the peer. On CURLE_OK it has been verified.
 */
CURLcode Curl_vtls_apple_verify(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                بنية ssl_peer *peer,
                                size_t عدد_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                فراغ *cb_user_انکڑے,
                                ثابت بنانشانی حرف *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp);

bool Curl_vtls_apple_verifies_ocsp(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے);

#جے_انت /* USE_APPLE_SECTRUST */

#جے_انت /* HEADER_CURL_VTLS_APPLE_H */
