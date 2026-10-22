#నిర్వచితం_కాకపోతే శీర్షిక_CURL_HTTP_HTTPSIG_H
#నిర్వచించు శీర్షిక_CURL_HTTP_HTTPSIG_H
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

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HTTPSIG)

#అన్తర్భావ "urldata.h"

CURLcode Curl_output_httpsig(నిర్మాణం Curl_easy *దత్తాంశం);

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && !CURL_DISABLE_HTTPSIG */
#అయితే_ముగింపు /* HEADER_CURL_HTTP_HTTPSIG_H */
