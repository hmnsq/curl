#నిర్వచితం_కాకపోతే శీర్షిక_CURL_CSHUTDN_H
#నిర్వచించు శీర్షిక_CURL_CSHUTDN_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 * Copyright (C) Linus Nielsen Feltzing, <linus@haxx.se>
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
#అన్తర్భావ "ptrarray.h"

నిర్మాణం connectdata;
నిర్మాణం Curl_easy;
నిర్మాణం curl_pollfds;
నిర్మాణం Curl_waitfds;
నిర్మాణం Curl_multi;
నిర్మాణం Curl_share;
నిర్మాణం Curl_sigpipe_ctx;


/* Terminates the connection, e.g. closes and destroys it.
 * If `do_shutdown` is TRUE, the shutdown will be run once.
 * Takes ownership of `conn`. `conn` MUST no longer be in
 * a pool or on a shutdown list. */
శూన్య Curl_cshutdn_terminate(నిర్మాణం Curl_easy *admin,
                            నిర్మాణం connectdata *conn,
                            bool do_shutdown);

/* Start the shutdown timer,
 * marks the connection sockindex as being shut down. */
శూన్య Curl_cshutdn_ప్రారంభం_timer(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                              పూర్ణాంకం timeout_ms);
/* Clear the shutdown timer at sockindex again. */
శూన్య Curl_cshutdn_clear_timer(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex);

/* return how much time there is left to shutdown the connection at
 * sockindex. Returns 0 if there is no limit or shutdown has not started. */
timediff_t Curl_cshutdn_timeleft_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం connectdata *conn,
                                    int8_t sockindex);

#నిర్వచించు CURL_CONN_లోపల_SHUTDOWN(c,i)   ((c)->shutdown.ప్రారంభం_ms[(i)] >= 0)

/* Shutdown the connection at `sockindex` non-blocking.
 * Will start the shutdown timer if not already set.
 * Return CURLE_OK and *done == FALSE if not finished. */
CURLcode Curl_cshutdn_try_once_idx(నిర్మాణం Curl_easy *దత్తాంశం,
                                   int8_t sockindex, bool *పూర్తి);

/* Run the shutdown of the connection once non-blocking for both
 * socket indices. Will start the shutdown timer if not already set.
 * Shortly attach/detach the admin handle to `conn` while doing so.
 * `done` will be set TRUE if any error was encountered or if
 * the connection was shut down completely. */
శూన్య Curl_cshutdn_try_once(నిర్మాణం Curl_easy *admin,
                           నిర్మాణం connectdata *conn, bool *పూర్తి);

/* A `cshutdown` is always owned by a multi handle to maintain
 * the connections to be shut down. It registers timers and
 * sockets to monitor via the multi handle. */
నిర్మాణం cshutdn {
  నిర్మాణం ptrarray conns;    /* connections being shut down */
};

/* Init as part of the given multi handle. */
శూన్య Curl_cshutdn_init(నిర్మాణం cshutdn *cshutdn);

/* Terminate all remaining connections and free resources. */
శూన్య Curl_cshutdn_destroy(నిర్మాణం cshutdn *cshutdn,
                          నిర్మాణం Curl_easy *admin);

/* Number of connections being shut down. */
size_t Curl_cshutdn_లెక్క(నిర్మాణం cshutdn *cshutdn);

/* Number of connections to the destination being shut down. */
size_t Curl_cshutdn_dest_లెక్క(నిర్మాణం cshutdn *cshutdn,
                               స్థిరరాశి అక్షరం *destination);

/* Close the oldest `count` connections in shutdown to destination or,
 * when destination is NULL for any destination.
 * Return the number of connections closed. */
size_t Curl_cshutdn_close_oldest(నిర్మాణం cshutdn *cshutdn,
                                 నిర్మాణం Curl_easy *admin,
                                 స్థిరరాశి అక్షరం *destination,
                                 size_t లెక్క);

/* Add a connection to have it shut down. Terminate the oldest
 * connection when shutdowns exceed max_shutdowns. */
శూన్య Curl_cshutdn_కూడు(నిర్మాణం cshutdn *cshutdn,
                      నిర్మాణం Curl_multi *multi,
                      నిర్మాణం connectdata *conn,
                      size_t max_shutdowns);

/* Add sockets and POLLIN/OUT flags for connections being shut down. */
CURLcode Curl_cshutdn_కూడు_pollfds(నిర్మాణం cshutdn *cshutdn,
                                  నిర్మాణం Curl_easy *admin,
                                  నిర్మాణం curl_pollfds *cpfds);

గుర్తులేని పూర్ణాంకం Curl_cshutdn_కూడు_waitfds(నిర్మాణం cshutdn *cshutdn,
                                      నిర్మాణం Curl_easy *admin,
                                      నిర్మాణం Curl_waitfds *cwfds);

శూన్య Curl_cshutdn_setfds(నిర్మాణం cshutdn *cshutdn,
                         నిర్మాణం Curl_easy *admin,
                         fd_set *read_fd_సమితి, fd_set *write_fd_సమితి,
                         పూర్ణాంకం *maxfd);

/* Run maintenance on all connections. */
శూన్య Curl_cshutdn_perform(నిర్మాణం cshutdn *cshutdn,
                          నిర్మాణం Curl_easy *admin,
                          నిర్మాణం Curl_sigpipe_ctx *sigpipe_ctx);

#అయితే_ముగింపు /* HEADER_CURL_CSHUTDN_H */
