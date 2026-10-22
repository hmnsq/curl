#ifndef HEADER_CURL_CSHUTDN_H
#define HEADER_CURL_CSHUTDN_H
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
#ضمن "ptrarray.h"

بنية connectdata;
بنية Curl_easy;
بنية curl_pollfds;
بنية Curl_waitfds;
بنية Curl_multi;
بنية Curl_share;
بنية Curl_sigpipe_ctx;


/* Terminates the connection, e.g. closes and destroys it.
 * If `do_shutdown` is TRUE, the shutdown will be run once.
 * Takes ownership of `conn`. `conn` MUST no longer be in
 * a pool or on a shutdown list. */
فراغ Curl_cshutdn_terminate(بنية Curl_easy *admin,
                            بنية connectdata *conn,
                            bool do_shutdown);

/* Start the shutdown timer,
 * marks the connection sockindex as being shut down. */
فراغ Curl_cshutdn_start_timer(بنية Curl_easy *data, int8_t sockindex,
                              صحيح timeout_ms);
/* Clear the shutdown timer at sockindex again. */
فراغ Curl_cshutdn_clear_timer(بنية Curl_easy *data, int8_t sockindex);

/* return how much time there is left to shutdown the connection at
 * sockindex. Returns 0 if there is no limit or shutdown has not started. */
timediff_t Curl_cshutdn_timeleft_ms(بنية Curl_easy *data,
                                    بنية connectdata *conn,
                                    int8_t sockindex);

#define CURL_CONN_IN_SHUTDOWN(c,i)   ((c)->shutdown.start_ms[(i)] >= 0)

/* Shutdown the connection at `sockindex` non-blocking.
 * Will start the shutdown timer if not already set.
 * Return CURLE_OK and *done == FALSE if not finished. */
CURLcode Curl_cshutdn_try_once_idx(بنية Curl_easy *data,
                                   int8_t sockindex, bool *done);

/* Run the shutdown of the connection once non-blocking for both
 * socket indices. Will start the shutdown timer if not already set.
 * Shortly attach/detach the admin handle to `conn` while doing so.
 * `done` will be set TRUE if any error was encountered or if
 * the connection was shut down completely. */
فراغ Curl_cshutdn_try_once(بنية Curl_easy *admin,
                           بنية connectdata *conn, bool *done);

/* A `cshutdown` is always owned by a multi handle to maintain
 * the connections to be shut down. It registers timers and
 * sockets to monitor via the multi handle. */
بنية cshutdn {
  بنية ptrarray conns;    /* connections being shut down */
};

/* Init as part of the given multi handle. */
فراغ Curl_cshutdn_init(بنية cshutdn *cshutdn);

/* Terminate all remaining connections and free resources. */
فراغ Curl_cshutdn_destroy(بنية cshutdn *cshutdn,
                          بنية Curl_easy *admin);

/* Number of connections being shut down. */
size_t Curl_cshutdn_عدد(بنية cshutdn *cshutdn);

/* Number of connections to the destination being shut down. */
size_t Curl_cshutdn_dest_عدد(بنية cshutdn *cshutdn,
                               ثابت حرف *destination);

/* Close the oldest `count` connections in shutdown to destination or,
 * when destination is NULL for any destination.
 * Return the number of connections closed. */
size_t Curl_cshutdn_close_oldest(بنية cshutdn *cshutdn,
                                 بنية Curl_easy *admin,
                                 ثابت حرف *destination,
                                 size_t عدد);

/* Add a connection to have it shut down. Terminate the oldest
 * connection when shutdowns exceed max_shutdowns. */
فراغ Curl_cshutdn_add(بنية cshutdn *cshutdn,
                      بنية Curl_multi *multi,
                      بنية connectdata *conn,
                      size_t max_shutdowns);

/* Add sockets and POLLIN/OUT flags for connections being shut down. */
CURLcode Curl_cshutdn_add_pollfds(بنية cshutdn *cshutdn,
                                  بنية Curl_easy *admin,
                                  بنية curl_pollfds *cpfds);

unsigned صحيح Curl_cshutdn_add_waitfds(بنية cshutdn *cshutdn,
                                      بنية Curl_easy *admin,
                                      بنية Curl_waitfds *cwfds);

فراغ Curl_cshutdn_setfds(بنية cshutdn *cshutdn,
                         بنية Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         صحيح *maxfd);

/* Run maintenance on all connections. */
فراغ Curl_cshutdn_perform(بنية cshutdn *cshutdn,
                          بنية Curl_easy *admin,
                          بنية Curl_sigpipe_ctx *sigpipe_ctx);

#endif /* HEADER_CURL_CSHUTDN_H */
