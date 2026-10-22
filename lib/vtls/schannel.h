#నిర్వచితం_కాకపోతే శీర్షిక_CURL_SCHANNEL_H
#నిర్వచించు శీర్షిక_CURL_SCHANNEL_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Marc Hoersken, <info@marc-hoersken.de>, et al.
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

#నిర్వచితమైతే USE_SCHANNEL

#అన్తర్భావ <schannel.h>

#అన్తర్భావ "curl_sspi.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "urldata.h"

బాహ్య స్థిరరాశి నిర్మాణం Curl_ssl Curl_ssl_schannel;

CURLcode Curl_verify_host(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం);

CURLcode Curl_verify_certificate(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం);

#అయితే_ముగింపు /* USE_SCHANNEL */
#అయితే_ముగింపు /* HEADER_CURL_SCHANNEL_H */
