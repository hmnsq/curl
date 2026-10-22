#నిర్వచితం_కాకపోతే శీర్షిక_CURL_URL_H
#నిర్వచించు శీర్షిక_CURL_URL_H
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

/* Reject URLs exceeding this length */
#నిర్వచించు MAX_URL_LEN 0xffff

/*
 * Prototypes for library-wide functions
 */

CURLcode Curl_init_transfer(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn);
CURLcode Curl_open(నిర్మాణం Curl_easy **curl);
శూన్య Curl_init_userdefined(నిర్మాణం Curl_easy *దత్తాంశం);

శూన్య Curl_freeset(నిర్మాణం Curl_easy *దత్తాంశం);
CURLcode Curl_uc_గమ్యం_curlcode(CURLUcode uc);
CURLcode Curl_close(నిర్మాణం Curl_easy **datap); /* opposite of Curl_open() */
CURLcode Curl_connect(నిర్మాణం Curl_easy *దత్తాంశం, bool *pconnected);
శూన్య Curl_conn_free(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn);
CURLcode Curl_parse_login_details(స్థిరరాశి అక్షరం *login, స్థిరరాశి size_t len,
                                  అక్షరం **userp, అక్షరం **passwdp,
                                  అక్షరం **optionsp);

/* Attach/Clear/Get meta data for an easy handle. Needs to provide
 * a destructor, will be automatically called when the easy handle
 * is reset or closed. */
రకనిర్వచనం శూన్య Curl_meta_dtor(స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_len, శూన్య *meta_దత్తాంశం);

/* Set the transfer meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_meta_సమితి(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *కీలకం,
                       శూన్య *meta_దత్తాంశం, Curl_meta_dtor *meta_dtor);
శూన్య Curl_meta_remove(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *కీలకం);
శూన్య *Curl_meta_get(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *కీలకం);
శూన్య Curl_meta_reset(నిర్మాణం Curl_easy *దత్తాంశం);

/* Set connection meta data for the key. Any existing entry for that
 * key will be destroyed.
 * Takes ownership of `meta_data` and destroys it when the call fails. */
CURLcode Curl_conn_meta_సమితి(నిర్మాణం connectdata *conn, స్థిరరాశి అక్షరం *కీలకం,
                            శూన్య *meta_దత్తాంశం, Curl_meta_dtor *meta_dtor);
శూన్య Curl_conn_meta_remove(నిర్మాణం connectdata *conn, స్థిరరాశి అక్షరం *కీలకం);
శూన్య *Curl_conn_meta_get(నిర్మాణం connectdata *conn, స్థిరరాశి అక్షరం *కీలకం);

/* Get an admin handle for internal operations from the given
 * easy handle, if possible. The admin handle inherits certain
 * properties from `data`. If no admin handle is available (not multi
 * or share attached), the easy handle itself is returned. */
నిర్మాణం Curl_easy *Curl_get_admin(నిర్మాణం Curl_easy *దత్తాంశం);

#నిర్వచించు CURL_DEFAULT_PROXY_పోర్టు 1080 /* default proxy port unless specified */
#నిర్వచించు CURL_DEFAULT_HTTPS_PROXY_పోర్టు 443 /* default https proxy port unless
                                             specified */

/* Always eval all arguments, return the first
 * result != (CURLE_OK | CURLE_AGAIN) or `r1`. */
CURLcode Curl_1st_fatal(CURLcode r1, CURLcode r2);

#ఒకవేళ defined(USE_HTTP2) || defined(USE_HTTP3)
శూన్య Curl_దత్తాంశం_ప్రాధాన్యత_clear_state(నిర్మాణం Curl_easy *దత్తాంశం);
#లేకపోతే
#నిర్వచించు Curl_దత్తాంశం_ప్రాధాన్యత_clear_state(x)
#అయితే_ముగింపు /* USE_HTTP2 || USE_HTTP3 */

#నిర్వచితమైతే USE_NGHTTP2
CURLcode Curl_దత్తాంశం_ప్రాధాన్యత_కూడు_బిడ్డ(నిర్మాణం Curl_easy *మాతృ,
                                      నిర్మాణం Curl_easy *బిడ్డ,
                                      bool exclusive);
#లేకపోతే
#నిర్వచించు Curl_దత్తాంశం_ప్రాధాన్యత_కూడు_బిడ్డ(x, y, z) CURLE_NOT_BUILT_లోపల
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_URL_H */
