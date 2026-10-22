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
#അന്തര്ഭാവ "ptrarray.h"

ഘടന connectdata;
ഘടന Curl_easy;
ഘടന curl_pollfds;
ഘടന Curl_waitfds;
ഘടന Curl_multi;
ഘടന Curl_share;
ഘടന Curl_sigpipe_ctx;


/* Terminates the connection, e.g. closes and destroys it.
 * If `do_shutdown` is TRUE, the shutdown will be run once.
 * Takes ownership of `conn`. `conn` MUST no longer be in
 * a pool or on a shutdown list. */
ശൂന്യം Curl_cshutdn_terminate(ഘടന Curl_easy *admin,
                            ഘടന connectdata *conn,
                            bool do_shutdown);

/* Start the shutdown timer,
 * marks the connection sockindex as being shut down. */
ശൂന്യം Curl_cshutdn_start_timer(ഘടന Curl_easy *data, int8_t sockindex,
                              പൂർണ്ണസംഖ്യ timeout_ms);
/* Clear the shutdown timer at sockindex again. */
ശൂന്യം Curl_cshutdn_clear_timer(ഘടന Curl_easy *data, int8_t sockindex);

/* return how much time there is left to shutdown the connection at
 * sockindex. Returns 0 if there is no limit or shutdown has not started. */
timediff_t Curl_cshutdn_timeleft_ms(ഘടന Curl_easy *data,
                                    ഘടന connectdata *conn,
                                    int8_t sockindex);

#define CURL_CONN_IN_SHUTDOWN(c,i)   ((c)->shutdown.start_ms[(i)] >= 0)

/* Shutdown the connection at `sockindex` non-blocking.
 * Will start the shutdown timer if not already set.
 * Return CURLE_OK and *done == FALSE if not finished. */
CURLcode Curl_cshutdn_try_once_idx(ഘടന Curl_easy *data,
                                   int8_t sockindex, bool *done);

/* Run the shutdown of the connection once non-blocking for both
 * socket indices. Will start the shutdown timer if not already set.
 * Shortly attach/detach the admin handle to `conn` while doing so.
 * `done` will be set TRUE if any error was encountered or if
 * the connection was shut down completely. */
ശൂന്യം Curl_cshutdn_try_once(ഘടന Curl_easy *admin,
                           ഘടന connectdata *conn, bool *done);

/* A `cshutdown` is always owned by a multi handle to maintain
 * the connections to be shut down. It registers timers and
 * sockets to monitor via the multi handle. */
ഘടന cshutdn {
  ഘടന ptrarray conns;    /* connections being shut down */
};

/* Init as part of the given multi handle. */
ശൂന്യം Curl_cshutdn_init(ഘടന cshutdn *cshutdn);

/* Terminate all remaining connections and free resources. */
ശൂന്യം Curl_cshutdn_destroy(ഘടന cshutdn *cshutdn,
                          ഘടന Curl_easy *admin);

/* Number of connections being shut down. */
size_t Curl_cshutdn_എണ്ണം(ഘടന cshutdn *cshutdn);

/* Number of connections to the destination being shut down. */
size_t Curl_cshutdn_dest_എണ്ണം(ഘടന cshutdn *cshutdn,
                               സ്ഥിരം അക്ഷരം *destination);

/* Close the oldest `count` connections in shutdown to destination or,
 * when destination is NULL for any destination.
 * Return the number of connections closed. */
size_t Curl_cshutdn_close_oldest(ഘടന cshutdn *cshutdn,
                                 ഘടന Curl_easy *admin,
                                 സ്ഥിരം അക്ഷരം *destination,
                                 size_t എണ്ണം);

/* Add a connection to have it shut down. Terminate the oldest
 * connection when shutdowns exceed max_shutdowns. */
ശൂന്യം Curl_cshutdn_add(ഘടന cshutdn *cshutdn,
                      ഘടന Curl_multi *multi,
                      ഘടന connectdata *conn,
                      size_t max_shutdowns);

/* Add sockets and POLLIN/OUT flags for connections being shut down. */
CURLcode Curl_cshutdn_add_pollfds(ഘടന cshutdn *cshutdn,
                                  ഘടന Curl_easy *admin,
                                  ഘടന curl_pollfds *cpfds);

unsigned പൂർണ്ണസംഖ്യ Curl_cshutdn_add_waitfds(ഘടന cshutdn *cshutdn,
                                      ഘടന Curl_easy *admin,
                                      ഘടന Curl_waitfds *cwfds);

ശൂന്യം Curl_cshutdn_setfds(ഘടന cshutdn *cshutdn,
                         ഘടന Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         പൂർണ്ണസംഖ്യ *maxfd);

/* Run maintenance on all connections. */
ശൂന്യം Curl_cshutdn_perform(ഘടന cshutdn *cshutdn,
                          ഘടന Curl_easy *admin,
                          ഘടന Curl_sigpipe_ctx *sigpipe_ctx);

#endif /* HEADER_CURL_CSHUTDN_H */
