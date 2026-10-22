#నిర్వచితం_కాకపోతే శీర్షిక_CURL_IDN_H
#నిర్వచించు శీర్షిక_CURL_IDN_H
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

నిర్మాణం Curl_str;
నిర్మాణం hostname;

bool Curl_is_ASCII_name(స్థిరరాశి అక్షరం *hostname);
bool Curl_is_ASCII_str(నిర్మాణం Curl_str *s);

CURLcode Curl_idnconvert_hostname(నిర్మాణం hostname *host);

#ఒకవేళ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#నిర్వచించు USE_IDN
శూన్య Curl_free_idnconverted_hostname(నిర్మాణం hostname *host);
CURLcode Curl_idn_decode(స్థిరరాశి అక్షరం *ఆదానం, అక్షరం **output);
CURLcode Curl_idn_encode(స్థిరరాశి అక్షరం *puny, అక్షరం **output);
#లేకపోతే
#నిర్వచించు Curl_free_idnconverted_hostname(x)
#నిర్వచించు Curl_idn_decode(x) ఖాళీ
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_IDN_H */
