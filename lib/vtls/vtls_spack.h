#నిర్వచితం_కాకపోతే శీర్షిక_CURL_VTLS_SPACK_H
#నిర్వచించు శీర్షిక_CURL_VTLS_SPACK_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
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

#ఒకవేళ defined(USE_SSL) && defined(USE_SSLS_EXPORT)

నిర్మాణం dynbuf;
నిర్మాణం Curl_ssl_session;

CURLcode Curl_ssl_session_pack(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_ssl_session *s,
                               నిర్మాణం dynbuf *buf);

CURLcode Curl_ssl_session_unpack(నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి శూన్య *bufv, size_t buflen,
                                 నిర్మాణం Curl_ssl_session **ps);

#అయితే_ముగింపు /* USE_SSL && USE_SSLS_EXPORT */

#అయితే_ముగింపు /* HEADER_CURL_VTLS_SPACK_H */
