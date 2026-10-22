#నిర్వచితం_కాకపోతే శీర్షిక_CURL_CF_HAPROXY_H
#నిర్వచించు శీర్షిక_CURL_CF_HAPROXY_H
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

#అన్తర్భావ "urldata.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY

CURLcode Curl_cf_haproxy_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                      నిర్మాణం Curl_easy *దత్తాంశం);

బాహ్య నిర్మాణం Curl_cftype Curl_cft_haproxy;

#అయితే_ముగింపు /* !CURL_DISABLE_PROXY */

#అయితే_ముగింపు /* HEADER_CURL_CF_HAPROXY_H */
