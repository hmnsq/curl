#నిర్వచితం_కాకపోతే శీర్షిక_CURL_MULTI_EV_H
#నిర్వచించు శీర్షిక_CURL_MULTI_EV_H
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
#అన్తర్భావ "hash.h"

నిర్మాణం Curl_easy;
నిర్మాణం Curl_multi;
నిర్మాణం easy_pollset;
నిర్మాణం uint32_bset;

/* meta key for event pollset at easy handle or connection */
#నిర్వచించు CURL_META_MEV_POLLSET   "meta:mev:ps"

నిర్మాణం curl_multi_ev {
  నిర్మాణం Curl_hash sh_entries;
};

/* Setup/teardown of multi event book-keeping. */
శూన్య Curl_multi_ev_init(నిర్మాణం Curl_multi *multi, size_t hashsize);
శూన్య Curl_multi_ev_cleanup(నిర్మాణం Curl_multi *multi);

/* Assign a 'user_data' to be passed to the socket callback when
 * invoked with the given socket. This will fail if this socket
 * is not active, e.g. the application has not been told to monitor it. */
CURLMcode Curl_multi_ev_కేటాయించు(నిర్మాణం Curl_multi *multi, curl_socket_t s,
                               శూన్య *user_దత్తాంశం);

/* Assess the transfer by getting its current pollset, compute
 * any changes to the last one and inform the application's socket
 * callback if things have changed. */
CURLMcode Curl_multi_ev_assess_xfer(నిర్మాణం Curl_multi *multi,
                                    నిర్మాణం Curl_easy *దత్తాంశం);
/* Assess all easy handles on the list */
CURLMcode Curl_multi_ev_assess_xfer_bset(నిర్మాణం Curl_multi *multi,
                                         నిర్మాణం uint32_bset *సమితి);
/* Assess the connection by getting its current pollset */
CURLMcode Curl_multi_ev_assess_conn(నిర్మాణం Curl_multi *multi,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం connectdata *conn);

/* Mark all transfers tied to the given socket as dirty */
శూన్య Curl_multi_ev_dirty_xfers(నిర్మాణం Curl_multi *multi,
                               curl_socket_t s);

/* Socket will be closed, forget anything we know about it. */
శూన్య Curl_multi_ev_socket_పూర్తి(నిర్మాణం Curl_multi *multi,
                               నిర్మాణం Curl_easy *దత్తాంశం, curl_socket_t s);

/* Transfer is removed from the multi */
శూన్య Curl_multi_ev_xfer_పూర్తి(నిర్మాణం Curl_multi *multi,
                             నిర్మాణం Curl_easy *దత్తాంశం);

/* Connection is being destroyed */
శూన్య Curl_multi_ev_conn_పూర్తి(నిర్మాణం Curl_multi *multi,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం connectdata *conn);

#అయితే_ముగింపు /* HEADER_CURL_MULTI_EV_H */
