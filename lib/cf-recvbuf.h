#నిర్వచితం_కాకపోతే శీర్షిక_CURL_CF_RECVBUF_H
#నిర్వచించు శీర్షిక_CURL_CF_RECVBUF_H
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

#నిర్వచితం_కాకపోతే CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

CURLcode Curl_cf_recvbuf_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం connectdata *conn,
                             int8_t sockindex,
                             స్థిరరాశి uint8_t *buf, size_t blen);

బాహ్య నిర్మాణం Curl_cftype Curl_cft_recvbuf;

#అయితే_ముగింపు /* !CURL_DISABLE_WEBSOCKETS */

#అయితే_ముగింపు /* HEADER_CURL_CF_RECVBUF_H */
