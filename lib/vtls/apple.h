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
কাঠামো Curl_cfilter;
কাঠামো Curl_easy;
কাঠামো ssl_peer;

/* Get the DER encoded i-th certificate in the server handshake */
প্রকারলেখ CURLcode Curl_vtls_get_cert_der(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        শূন্য *user_তথ্য,
                                        size_t i,
                                        unsigned অক্ষর **pder,
                                        size_t *pder_len);

/* Ask Apple's Security framework to verify the certificate chain
 * send by the peer. On CURLE_OK it has been verified.
 */
CURLcode Curl_vtls_apple_verify(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                কাঠামো ssl_peer *peer,
                                size_t অঙ্ক_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                শূন্য *cb_user_তথ্য,
                                ধ্রুবক unsigned অক্ষর *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp);

bool Curl_vtls_apple_verifies_ocsp(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য);

#endif /* USE_APPLE_SECTRUST */

#endif /* HEADER_CURL_VTLS_APPLE_H */
