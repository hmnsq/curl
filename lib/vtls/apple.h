#నిర్వచితం_కాకపోతే శీర్షిక_CURL_VTLS_APPLE_H
#నిర్వచించు శీర్షిక_CURL_VTLS_APPLE_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_APPLE_SECTRUST
నిర్మాణం Curl_cfilter;
నిర్మాణం Curl_easy;
నిర్మాణం ssl_peer;

/* Get the DER encoded i-th certificate in the server handshake */
రకనిర్వచనం CURLcode Curl_vtls_get_cert_der(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        శూన్య *user_దత్తాంశం,
                                        size_t i,
                                        గుర్తులేని అక్షరం **pder,
                                        size_t *pder_len);

/* Ask Apple's Security framework to verify the certificate chain
 * send by the peer. On CURLE_OK it has been verified.
 */
CURLcode Curl_vtls_apple_verify(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం ssl_peer *peer,
                                size_t అంకె_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                శూన్య *cb_user_దత్తాంశం,
                                స్థిరరాశి గుర్తులేని అక్షరం *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp);

bool Curl_vtls_apple_verifies_ocsp(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం);

#అయితే_ముగింపు /* USE_APPLE_SECTRUST */

#అయితే_ముగింపు /* HEADER_CURL_VTLS_APPLE_H */
