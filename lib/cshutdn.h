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
#અન્તર્ભાવ "ptrarray.h"

સંરચના connectdata;
સંરચના Curl_easy;
સંરચના curl_pollfds;
સંરચના Curl_waitfds;
સંરચના Curl_multi;
સંરચના Curl_share;
સંરચના Curl_sigpipe_ctx;


/* Terminates the connection, e.g. closes and destroys it.
 * If `do_shutdown` is TRUE, the shutdown will be run once.
 * Takes ownership of `conn`. `conn` MUST no longer be in
 * a pool or on a shutdown list. */
શૂન્ય Curl_cshutdn_terminate(સંરચના Curl_easy *admin,
                            સંરચના connectdata *conn,
                            bool do_shutdown);

/* Start the shutdown timer,
 * marks the connection sockindex as being shut down. */
શૂન્ય Curl_cshutdn_start_timer(સંરચના Curl_easy *data, int8_t sockindex,
                              પૂર્ણાંક timeout_ms);
/* Clear the shutdown timer at sockindex again. */
શૂન્ય Curl_cshutdn_clear_timer(સંરચના Curl_easy *data, int8_t sockindex);

/* return how much time there is left to shutdown the connection at
 * sockindex. Returns 0 if there is no limit or shutdown has not started. */
timediff_t Curl_cshutdn_timeleft_ms(સંરચના Curl_easy *data,
                                    સંરચના connectdata *conn,
                                    int8_t sockindex);

#define CURL_CONN_IN_SHUTDOWN(c,i)   ((c)->shutdown.start_ms[(i)] >= 0)

/* Shutdown the connection at `sockindex` non-blocking.
 * Will start the shutdown timer if not already set.
 * Return CURLE_OK and *done == FALSE if not finished. */
CURLcode Curl_cshutdn_try_once_idx(સંરચના Curl_easy *data,
                                   int8_t sockindex, bool *done);

/* Run the shutdown of the connection once non-blocking for both
 * socket indices. Will start the shutdown timer if not already set.
 * Shortly attach/detach the admin handle to `conn` while doing so.
 * `done` will be set TRUE if any error was encountered or if
 * the connection was shut down completely. */
શૂન્ય Curl_cshutdn_try_once(સંરચના Curl_easy *admin,
                           સંરચના connectdata *conn, bool *done);

/* A `cshutdown` is always owned by a multi handle to maintain
 * the connections to be shut down. It registers timers and
 * sockets to monitor via the multi handle. */
સંરચના cshutdn {
  સંરચના ptrarray conns;    /* connections being shut down */
};

/* Init as part of the given multi handle. */
શૂન્ય Curl_cshutdn_init(સંરચના cshutdn *cshutdn);

/* Terminate all remaining connections and free resources. */
શૂન્ય Curl_cshutdn_destroy(સંરચના cshutdn *cshutdn,
                          સંરચના Curl_easy *admin);

/* Number of connections being shut down. */
size_t Curl_cshutdn_ગણતરી(સંરચના cshutdn *cshutdn);

/* Number of connections to the destination being shut down. */
size_t Curl_cshutdn_dest_ગણતરી(સંરચના cshutdn *cshutdn,
                               અચલ અક્ષર *destination);

/* Close the oldest `count` connections in shutdown to destination or,
 * when destination is NULL for any destination.
 * Return the number of connections closed. */
size_t Curl_cshutdn_close_oldest(સંરચના cshutdn *cshutdn,
                                 સંરચના Curl_easy *admin,
                                 અચલ અક્ષર *destination,
                                 size_t ગણતરી);

/* Add a connection to have it shut down. Terminate the oldest
 * connection when shutdowns exceed max_shutdowns. */
શૂન્ય Curl_cshutdn_add(સંરચના cshutdn *cshutdn,
                      સંરચના Curl_multi *multi,
                      સંરચના connectdata *conn,
                      size_t max_shutdowns);

/* Add sockets and POLLIN/OUT flags for connections being shut down. */
CURLcode Curl_cshutdn_add_pollfds(સંરચના cshutdn *cshutdn,
                                  સંરચના Curl_easy *admin,
                                  સંરચના curl_pollfds *cpfds);

unsigned પૂર્ણાંક Curl_cshutdn_add_waitfds(સંરચના cshutdn *cshutdn,
                                      સંરચના Curl_easy *admin,
                                      સંરચના Curl_waitfds *cwfds);

શૂન્ય Curl_cshutdn_setfds(સંરચના cshutdn *cshutdn,
                         સંરચના Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         પૂર્ણાંક *maxfd);

/* Run maintenance on all connections. */
શૂન્ય Curl_cshutdn_perform(સંરચના cshutdn *cshutdn,
                          સંરચના Curl_easy *admin,
                          સંરચના Curl_sigpipe_ctx *sigpipe_ctx);

#endif /* HEADER_CURL_CSHUTDN_H */
