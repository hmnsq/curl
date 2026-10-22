#నిర్వచితం_కాకపోతే శీర్షిక_CURL_MD5_H
#నిర్వచించు శీర్షిక_CURL_MD5_H
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

#ఒకవేళ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) || \
  !defined(CURL_DISABLE_DIGEST_AUTH)

#అన్తర్భావ "curl_hmac.h"

#నిర్వచించు MD5_DIGEST_LEN  16

రకనిర్వచనం CURLcode (*Curl_MD5_init_func)(శూన్య *context);
రకనిర్వచనం శూన్య (*Curl_MD5_update_func)(శూన్య *context,
                                     స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం,
                                     గుర్తులేని పూర్ణాంకం len);
రకనిర్వచనం శూన్య (*Curl_MD5_final_func)(గుర్తులేని అక్షరం *ఫలితం, శూన్య *context);

నిర్మాణం MD5_params {
  Curl_MD5_init_func md5_init_func;     /* Initialize context procedure */
  Curl_MD5_update_func md5_update_func; /* Update context with data */
  Curl_MD5_final_func md5_final_func;   /* Get final result procedure */
  గుర్తులేని పూర్ణాంకం md5_ctxtsize;            /* Context structure size */
  గుర్తులేని పూర్ణాంకం md5_resultlen;           /* Result length (bytes) */
};

నిర్మాణం MD5_context {
  స్థిరరాశి నిర్మాణం MD5_params *md5_hash;  /* Hash function definition */
  శూన్య *md5_hashctx;                  /* Hash function context */
};

బాహ్య స్థిరరాశి నిర్మాణం MD5_params Curl_DIGEST_MD5;
బాహ్య స్థిరరాశి నిర్మాణం HMAC_params Curl_HMAC_MD5;

CURLcode Curl_md5it(గుర్తులేని అక్షరం *output, స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం,
                    size_t len);

నిర్మాణం MD5_context *Curl_MD5_init(స్థిరరాశి నిర్మాణం MD5_params *md5params);
CURLcode Curl_MD5_update(నిర్మాణం MD5_context *context,
                         స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం,
                         గుర్తులేని పూర్ణాంకం len);
CURLcode Curl_MD5_final(నిర్మాణం MD5_context *context, గుర్తులేని అక్షరం *ఫలితం);

#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_MD5_H */
