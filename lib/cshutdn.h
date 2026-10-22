#اگر_غیر_تعریف_شدہ سرخی_CURL_CSHUTDN_H
#تعریف_کرو سرخی_CURL_CSHUTDN_H
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
#شامل "ptrarray.h"

ساخت connectdata;
ساخت Curl_easy;
ساخت curl_pollfds;
ساخت Curl_waitfds;
ساخت Curl_multi;
ساخت Curl_share;
ساخت Curl_sigpipe_ctx;


/* Terminates the connection, e.g. closes and destroys it.
 * If `do_shutdown` is TRUE, the shutdown will be run once.
 * Takes ownership of `conn`. `conn` MUST no longer be in
 * a pool or on a shutdown list. */
خالی Curl_cshutdn_terminate(ساخت Curl_easy *admin,
                            ساخت connectdata *conn,
                            bool do_shutdown);

/* Start the shutdown timer,
 * marks the connection sockindex as being shut down. */
خالی Curl_cshutdn_آغاز_timer(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                              صحیح timeout_ms);
/* Clear the shutdown timer at sockindex again. */
خالی Curl_cshutdn_clear_timer(ساخت Curl_easy *ڈیٹا, int8_t sockindex);

/* return how much time there is left to shutdown the connection at
 * sockindex. Returns 0 if there is no limit or shutdown has not started. */
timediff_t Curl_cshutdn_timeleft_ms(ساخت Curl_easy *ڈیٹا,
                                    ساخت connectdata *conn,
                                    int8_t sockindex);

#تعریف_کرو CURL_CONN_اندر_SHUTDOWN(c,i)   ((c)->shutdown.آغاز_ms[(i)] >= 0)

/* Shutdown the connection at `sockindex` non-blocking.
 * Will start the shutdown timer if not already set.
 * Return CURLE_OK and *done == FALSE if not finished. */
CURLcode Curl_cshutdn_try_once_idx(ساخت Curl_easy *ڈیٹا,
                                   int8_t sockindex, bool *مکمل);

/* Run the shutdown of the connection once non-blocking for both
 * socket indices. Will start the shutdown timer if not already set.
 * Shortly attach/detach the admin handle to `conn` while doing so.
 * `done` will be set TRUE if any error was encountered or if
 * the connection was shut down completely. */
خالی Curl_cshutdn_try_once(ساخت Curl_easy *admin,
                           ساخت connectdata *conn, bool *مکمل);

/* A `cshutdown` is always owned by a multi handle to maintain
 * the connections to be shut down. It registers timers and
 * sockets to monitor via the multi handle. */
ساخت cshutdn {
  ساخت ptrarray conns;    /* connections being shut down */
};

/* Init as part of the given multi handle. */
خالی Curl_cshutdn_init(ساخت cshutdn *cshutdn);

/* Terminate all remaining connections and free resources. */
خالی Curl_cshutdn_destroy(ساخت cshutdn *cshutdn,
                          ساخت Curl_easy *admin);

/* Number of connections being shut down. */
size_t Curl_cshutdn_گنتی(ساخت cshutdn *cshutdn);

/* Number of connections to the destination being shut down. */
size_t Curl_cshutdn_dest_گنتی(ساخت cshutdn *cshutdn,
                               ثابت حرف *destination);

/* Close the oldest `count` connections in shutdown to destination or,
 * when destination is NULL for any destination.
 * Return the number of connections closed. */
size_t Curl_cshutdn_close_oldest(ساخت cshutdn *cshutdn,
                                 ساخت Curl_easy *admin,
                                 ثابت حرف *destination,
                                 size_t گنتی);

/* Add a connection to have it shut down. Terminate the oldest
 * connection when shutdowns exceed max_shutdowns. */
خالی Curl_cshutdn_جوڑو(ساخت cshutdn *cshutdn,
                      ساخت Curl_multi *multi,
                      ساخت connectdata *conn,
                      size_t max_shutdowns);

/* Add sockets and POLLIN/OUT flags for connections being shut down. */
CURLcode Curl_cshutdn_جوڑو_pollfds(ساخت cshutdn *cshutdn,
                                  ساخت Curl_easy *admin,
                                  ساخت curl_pollfds *cpfds);

غیرعلامتی صحیح Curl_cshutdn_جوڑو_waitfds(ساخت cshutdn *cshutdn,
                                      ساخت Curl_easy *admin,
                                      ساخت Curl_waitfds *cwfds);

خالی Curl_cshutdn_setfds(ساخت cshutdn *cshutdn,
                         ساخت Curl_easy *admin,
                         fd_set *read_fd_سیٹ, fd_set *write_fd_سیٹ,
                         صحیح *maxfd);

/* Run maintenance on all connections. */
خالی Curl_cshutdn_perform(ساخت cshutdn *cshutdn,
                          ساخت Curl_easy *admin,
                          ساخت Curl_sigpipe_ctx *sigpipe_ctx);

#اگر_ختم /* HEADER_CURL_CSHUTDN_H */
