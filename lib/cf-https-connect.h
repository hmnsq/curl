#నిర్వచితం_కాకపోతే శీర్షిక_CURL_CF_HTTP_H
#నిర్వచించు శీర్షిక_CURL_CF_HTTP_H
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

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP

నిర్మాణం Curl_cfilter;
నిర్మాణం Curl_easy;
నిర్మాణం connectdata;
నిర్మాణం Curl_cftype;
నిర్మాణం Curl_peer;

బాహ్య నిర్మాణం Curl_cftype Curl_cft_http_connect;

CURLcode Curl_cf_https_అమరిక(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_peer *destination,
                             నిర్మాణం connectdata *conn,
                             int8_t sockindex);

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP */
#అయితే_ముగింపు /* HEADER_CURL_CF_HTTP_H */
