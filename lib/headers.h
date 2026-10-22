#నిర్వచితం_కాకపోతే శీర్షిక_CURL_శీర్షిక_H
#నిర్వచించు శీర్షిక_CURL_శీర్షిక_H
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

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_శీర్షికలు_API)

నిర్మాణం Curl_శీర్షిక_store {
  నిర్మాణం Curl_llist_కణుపు కణుపు;
  అక్షరం *name; /* points into 'buffer' */
  అక్షరం *విలువ; /* points into 'buffer' */
  size_t nh_amount; /* cached amount for curl_easy_nextheader() */
  size_t nh_సూచీ; /* cached index for curl_easy_nextheader() */
  పూర్ణాంకం request; /* 0 is the first request, then 1.. 2.. */
  గుర్తులేని అక్షరం type; /* CURLH_* defines */
  అక్షరం బఫరు[1]; /* this is the raw header blob */
};

/*
 * Initialize header collecting for a transfer.
 * Add a client writer that catches CLIENTWRITE_HEADER writes.
 */
CURLcode Curl_శీర్షికలు_init(నిర్మాణం Curl_easy *దత్తాంశం);

/*
 * Curl_headers_push() gets passed a full header to store.
 */
CURLcode Curl_శీర్షికలు_నెట్టు(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *శీర్షిక,
                           size_t hlen, గుర్తులేని అక్షరం type);

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_శీర్షికలు_cleanup(నిర్మాణం Curl_easy *దత్తాంశం);

#లేకపోతే
#నిర్వచించు Curl_శీర్షికలు_init(x)          CURLE_OK
#నిర్వచించు Curl_శీర్షికలు_నెట్టు(x, y, z, a) \
  ((శూన్య)(x), (శూన్య)(y), (శూన్య)(z), (శూన్య)(a), CURLE_OK)
#నిర్వచించు Curl_శీర్షికలు_cleanup(x)       Curl_nop_stmt
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_HEADER_H */
