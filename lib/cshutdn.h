#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_CSHUTDN_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_CSHUTDN_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "ptrarray.h"

ਬਣਤਰ connectdata;
ਬਣਤਰ Curl_easy;
ਬਣਤਰ curl_pollfds;
ਬਣਤਰ Curl_waitfds;
ਬਣਤਰ Curl_multi;
ਬਣਤਰ Curl_share;
ਬਣਤਰ Curl_sigpipe_ctx;


/* Terminates the connection, e.g. closes and destroys it.
 * If `do_shutdown` is TRUE, the shutdown will be run once.
 * Takes ownership of `conn`. `conn` MUST no longer be in
 * a pool or on a shutdown list. */
ਖਾਲੀ Curl_cshutdn_terminate(ਬਣਤਰ Curl_easy *admin,
                            ਬਣਤਰ connectdata *conn,
                            bool do_shutdown);

/* Start the shutdown timer,
 * marks the connection sockindex as being shut down. */
ਖਾਲੀ Curl_cshutdn_ਸ਼ੁਰੂ_timer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                              ਪੂਰਨ timeout_ms);
/* Clear the shutdown timer at sockindex again. */
ਖਾਲੀ Curl_cshutdn_clear_timer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex);

/* return how much time there is left to shutdown the connection at
 * sockindex. Returns 0 if there is no limit or shutdown has not started. */
timediff_t Curl_cshutdn_timeleft_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ connectdata *conn,
                                    int8_t sockindex);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CONN_ਵਿੱਚ_SHUTDOWN(c,i)   ((c)->shutdown.ਸ਼ੁਰੂ_ms[(i)] >= 0)

/* Shutdown the connection at `sockindex` non-blocking.
 * Will start the shutdown timer if not already set.
 * Return CURLE_OK and *done == FALSE if not finished. */
CURLcode Curl_cshutdn_try_once_idx(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   int8_t sockindex, bool *ਪੂਰਾ);

/* Run the shutdown of the connection once non-blocking for both
 * socket indices. Will start the shutdown timer if not already set.
 * Shortly attach/detach the admin handle to `conn` while doing so.
 * `done` will be set TRUE if any error was encountered or if
 * the connection was shut down completely. */
ਖਾਲੀ Curl_cshutdn_try_once(ਬਣਤਰ Curl_easy *admin,
                           ਬਣਤਰ connectdata *conn, bool *ਪੂਰਾ);

/* A `cshutdown` is always owned by a multi handle to maintain
 * the connections to be shut down. It registers timers and
 * sockets to monitor via the multi handle. */
ਬਣਤਰ cshutdn {
  ਬਣਤਰ ptrarray conns;    /* connections being shut down */
};

/* Init as part of the given multi handle. */
ਖਾਲੀ Curl_cshutdn_init(ਬਣਤਰ cshutdn *cshutdn);

/* Terminate all remaining connections and free resources. */
ਖਾਲੀ Curl_cshutdn_destroy(ਬਣਤਰ cshutdn *cshutdn,
                          ਬਣਤਰ Curl_easy *admin);

/* Number of connections being shut down. */
size_t Curl_cshutdn_ਗਿਣਤੀ(ਬਣਤਰ cshutdn *cshutdn);

/* Number of connections to the destination being shut down. */
size_t Curl_cshutdn_dest_ਗਿਣਤੀ(ਬਣਤਰ cshutdn *cshutdn,
                               ਅਚਲ ਅੱਖਰ *destination);

/* Close the oldest `count` connections in shutdown to destination or,
 * when destination is NULL for any destination.
 * Return the number of connections closed. */
size_t Curl_cshutdn_close_oldest(ਬਣਤਰ cshutdn *cshutdn,
                                 ਬਣਤਰ Curl_easy *admin,
                                 ਅਚਲ ਅੱਖਰ *destination,
                                 size_t ਗਿਣਤੀ_2);

/* Add a connection to have it shut down. Terminate the oldest
 * connection when shutdowns exceed max_shutdowns. */
ਖਾਲੀ Curl_cshutdn_ਜੋੜੋ(ਬਣਤਰ cshutdn *cshutdn,
                      ਬਣਤਰ Curl_multi *multi,
                      ਬਣਤਰ connectdata *conn,
                      size_t max_shutdowns);

/* Add sockets and POLLIN/OUT flags for connections being shut down. */
CURLcode Curl_cshutdn_ਜੋੜੋ_pollfds(ਬਣਤਰ cshutdn *cshutdn,
                                  ਬਣਤਰ Curl_easy *admin,
                                  ਬਣਤਰ curl_pollfds *cpfds);

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ Curl_cshutdn_ਜੋੜੋ_waitfds(ਬਣਤਰ cshutdn *cshutdn,
                                      ਬਣਤਰ Curl_easy *admin,
                                      ਬਣਤਰ Curl_waitfds *cwfds);

ਖਾਲੀ Curl_cshutdn_setfds(ਬਣਤਰ cshutdn *cshutdn,
                         ਬਣਤਰ Curl_easy *admin,
                         fd_set *read_fd_ਸੈੱਟ, fd_set *write_fd_ਸੈੱਟ,
                         ਪੂਰਨ *maxfd);

/* Run maintenance on all connections. */
ਖਾਲੀ Curl_cshutdn_perform(ਬਣਤਰ cshutdn *cshutdn,
                          ਬਣਤਰ Curl_easy *admin,
                          ਬਣਤਰ Curl_sigpipe_ctx *sigpipe_ctx);

#ਜੇ_ਅੰਤ /* HEADER_CURL_CSHUTDN_H */
