#اگر_غیر_تعریف_شدہ سرخی_CURL_VTLS_APPLE_H
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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_APPLE_SECTRUST
ساخت Curl_cfilter;
ساخت Curl_easy;
ساخت ssl_peer;

/* Get the DER encoded i-th certificate in the server handshake */
قسمتعریف CURLcode Curl_vtls_get_cert_der(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        خالی *user_ڈیٹا,
                                        size_t i,
                                        غیرعلامتی حرف **pder,
                                        size_t *pder_len);

/* Ask Apple's Security framework to verify the certificate chain
 * send by the peer. On CURLE_OK it has been verified.
 */
CURLcode Curl_vtls_apple_verify(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                ساخت ssl_peer *peer,
                                size_t عدد_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                خالی *cb_user_ڈیٹا,
                                ثابت غیرعلامتی حرف *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp);

bool Curl_vtls_apple_verifies_ocsp(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا);

#اگر_ختم /* USE_APPLE_SECTRUST */

#اگر_ختم /* HEADER_CURL_VTLS_APPLE_H */
