#నిర్వచితం_కాకపోతే శీర్షిక_CURL_VQUIC_CF_QUICHE_H
#నిర్వచించు శీర్షిక_CURL_VQUIC_CF_QUICHE_H
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

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)

#అన్తర్భావ <quiche.h>
#అన్తర్భావ <openssl/ssl.h>

నిర్మాణం Curl_cfilter;
నిర్మాణం Curl_easy;

శూన్య Curl_quiche_ver(అక్షరం *p, size_t len);

CURLcode Curl_cf_quiche_create(నిర్మాణం Curl_cfilter **pcf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_peer *origin,
                               నిర్మాణం Curl_peer *peer,
                               నిర్మాణం connectdata *conn,
                               నిర్మాణం Curl_sockaddr_ex *addr);
CURLcode Curl_cf_quiche_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                     నిర్మాణం Curl_peer *origin,
                                     నిర్మాణం Curl_peer *peer,
                                     నిర్మాణం Curl_cfilter **pcf);

#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_VQUIC_CF_QUICHE_H */
