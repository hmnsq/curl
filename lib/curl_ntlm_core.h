#నిర్వచితం_కాకపోతే శీర్షిక_CURL_NTLM_CORE_H
#నిర్వచించు శీర్షిక_CURL_NTLM_CORE_H
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

#నిర్వచితమైతే USE_CURL_NTLM_CORE

నిర్మాణం ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#నిర్వచించు SHORTPAIR(x)   ((పూర్ణాంకం)((x) & 0xff)), ((పూర్ణాంకం)(((x) >> 8) & 0xff))
#నిర్వచించు LONGQUARTET(x) ((పూర్ణాంకం)((x) & 0xff)), ((పూర్ణాంకం)(((x) >> 8) & 0xff)), \
  ((పూర్ణాంకం)(((x) >> 16) & 0xff)), ((పూర్ణాంకం)(((x) >> 24) & 0xff))

శూన్య Curl_ntlm_core_lm_resp(స్థిరరాశి గుర్తులేని అక్షరం *కీలకాలు,
                            స్థిరరాశి గుర్తులేని అక్షరం *plaintext,
                            గుర్తులేని అక్షరం *results);

CURLcode Curl_ntlm_core_mk_lm_hash(స్థిరరాశి అక్షరం *password,
                                   గుర్తులేని అక్షరం *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(స్థిరరాశి అక్షరం *password,
                                   గుర్తులేని అక్షరం *ntbuffer /* 21 bytes */);

#నిర్వచితం_కాకపోతే USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(స్థిరరాశి గుర్తులేని అక్షరం *కీలకం, గుర్తులేని పూర్ణాంకం keylen,
                       స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం, గుర్తులేని పూర్ణాంకం datalen,
                       గుర్తులేని అక్షరం *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(స్థిరరాశి అక్షరం *user, size_t userlen,
                                       స్థిరరాశి అక్షరం *domain, size_t domlen,
                                       గుర్తులేని అక్షరం *ntlmhash,
                                       గుర్తులేని అక్షరం *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(స్థిరరాశి గుర్తులేని అక్షరం *ntlmv2hash,
                                       స్థిరరాశి గుర్తులేని అక్షరం *challenge_client,
                                       స్థిరరాశి నిర్మాణం ntlmdata *ntlm,
                                       గుర్తులేని అక్షరం **ntresp,
                                       గుర్తులేని పూర్ణాంకం *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(స్థిరరాశి గుర్తులేని అక్షరం *ntlmv2hash,
                                     స్థిరరాశి గుర్తులేని అక్షరం *challenge_client,
                                     స్థిరరాశి గుర్తులేని అక్షరం *challenge_server,
                                     గుర్తులేని అక్షరం *lmresp);

#అయితే_ముగింపు /* !USE_WINDOWS_SSPI */

#అయితే_ముగింపు /* USE_CURL_NTLM_CORE */

#అయితే_ముగింపు /* HEADER_CURL_NTLM_CORE_H */
