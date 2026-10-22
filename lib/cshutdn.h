#यदि_अपरिभाषितम् शीर्षकम्_CURL_CSHUTDN_H
#परिभाषय शीर्षकम्_CURL_CSHUTDN_H
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
#अन्तर्भाव "ptrarray.h"

संरचना connectdata;
संरचना Curl_easy;
संरचना curl_pollfds;
संरचना Curl_waitfds;
संरचना Curl_multi;
संरचना Curl_share;
संरचना Curl_sigpipe_ctx;


/* Terminates the connection, e.g. closes and destroys it.
 * If `do_shutdown` is TRUE, the shutdown will be run once.
 * Takes ownership of `conn`. `conn` MUST no longer be in
 * a pool or on a shutdown list. */
शून्य Curl_cshutdn_terminate(संरचना Curl_easy *admin,
                            संरचना connectdata *conn,
                            bool do_shutdown);

/* Start the shutdown timer,
 * marks the connection sockindex as being shut down. */
शून्य Curl_cshutdn_start_timer(संरचना Curl_easy *दत्त, int8_t sockindex,
                              पूर्णाङ्क timeout_ms);
/* Clear the shutdown timer at sockindex again. */
शून्य Curl_cshutdn_clear_timer(संरचना Curl_easy *दत्त, int8_t sockindex);

/* return how much time there is left to shutdown the connection at
 * sockindex. Returns 0 if there is no limit or shutdown has not started. */
timediff_t Curl_cshutdn_timeleft_ms(संरचना Curl_easy *दत्त,
                                    संरचना connectdata *conn,
                                    int8_t sockindex);

#परिभाषय CURL_CONN_अन्तर्_SHUTDOWN(c,i)   ((c)->shutdown.start_ms[(i)] >= 0)

/* Shutdown the connection at `sockindex` non-blocking.
 * Will start the shutdown timer if not already set.
 * Return CURLE_OK and *done == FALSE if not finished. */
CURLcode Curl_cshutdn_try_once_idx(संरचना Curl_easy *दत्त,
                                   int8_t sockindex, bool *समाप्तम्);

/* Run the shutdown of the connection once non-blocking for both
 * socket indices. Will start the shutdown timer if not already set.
 * Shortly attach/detach the admin handle to `conn` while doing so.
 * `done` will be set TRUE if any error was encountered or if
 * the connection was shut down completely. */
शून्य Curl_cshutdn_try_once(संरचना Curl_easy *admin,
                           संरचना connectdata *conn, bool *समाप्तम्);

/* A `cshutdown` is always owned by a multi handle to maintain
 * the connections to be shut down. It registers timers and
 * sockets to monitor via the multi handle. */
संरचना cshutdn {
  संरचना ptrarray conns;    /* connections being shut down */
};

/* Init as part of the given multi handle. */
शून्य Curl_cshutdn_init(संरचना cshutdn *cshutdn);

/* Terminate all remaining connections and free resources. */
शून्य Curl_cshutdn_destroy(संरचना cshutdn *cshutdn,
                          संरचना Curl_easy *admin);

/* Number of connections being shut down. */
size_t Curl_cshutdn_गणनम्(संरचना cshutdn *cshutdn);

/* Number of connections to the destination being shut down. */
size_t Curl_cshutdn_dest_गणनम्(संरचना cshutdn *cshutdn,
                               ध्रुव वर्ण *destination);

/* Close the oldest `count` connections in shutdown to destination or,
 * when destination is NULL for any destination.
 * Return the number of connections closed. */
size_t Curl_cshutdn_close_oldest(संरचना cshutdn *cshutdn,
                                 संरचना Curl_easy *admin,
                                 ध्रुव वर्ण *destination,
                                 size_t गणनम्);

/* Add a connection to have it shut down. Terminate the oldest
 * connection when shutdowns exceed max_shutdowns. */
शून्य Curl_cshutdn_सङ्कलय(संरचना cshutdn *cshutdn,
                      संरचना Curl_multi *multi,
                      संरचना connectdata *conn,
                      size_t max_shutdowns);

/* Add sockets and POLLIN/OUT flags for connections being shut down. */
CURLcode Curl_cshutdn_सङ्कलय_pollfds(संरचना cshutdn *cshutdn,
                                  संरचना Curl_easy *admin,
                                  संरचना curl_pollfds *cpfds);

अचिह्नित पूर्णाङ्क Curl_cshutdn_सङ्कलय_waitfds(संरचना cshutdn *cshutdn,
                                      संरचना Curl_easy *admin,
                                      संरचना Curl_waitfds *cwfds);

शून्य Curl_cshutdn_setfds(संरचना cshutdn *cshutdn,
                         संरचना Curl_easy *admin,
                         fd_set *read_fd_समुच्चयः, fd_set *write_fd_समुच्चयः,
                         पूर्णाङ्क *maxfd);

/* Run maintenance on all connections. */
शून्य Curl_cshutdn_perform(संरचना cshutdn *cshutdn,
                          संरचना Curl_easy *admin,
                          संरचना Curl_sigpipe_ctx *sigpipe_ctx);

#यदि_अन्तः /* HEADER_CURL_CSHUTDN_H */
