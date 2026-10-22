#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_CSHUTDN_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_CSHUTDN_H
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
#ಅನ್ತರ್ಭಾವ "ptrarray.h"

ರಚನೆ connectdata;
ರಚನೆ Curl_easy;
ರಚನೆ curl_pollfds;
ರಚನೆ Curl_waitfds;
ರಚನೆ Curl_multi;
ರಚನೆ Curl_share;
ರಚನೆ Curl_sigpipe_ctx;


/* Terminates the connection, e.g. closes and destroys it.
 * If `do_shutdown` is TRUE, the shutdown will be run once.
 * Takes ownership of `conn`. `conn` MUST no longer be in
 * a pool or on a shutdown list. */
ಶೂನ್ಯ Curl_cshutdn_terminate(ರಚನೆ Curl_easy *admin,
                            ರಚನೆ connectdata *conn,
                            bool do_shutdown);

/* Start the shutdown timer,
 * marks the connection sockindex as being shut down. */
ಶೂನ್ಯ Curl_cshutdn_ಆರಂಭ_timer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                              ಪೂರ್ಣಾಂಕ timeout_ms);
/* Clear the shutdown timer at sockindex again. */
ಶೂನ್ಯ Curl_cshutdn_clear_timer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex);

/* return how much time there is left to shutdown the connection at
 * sockindex. Returns 0 if there is no limit or shutdown has not started. */
timediff_t Curl_cshutdn_timeleft_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ connectdata *conn,
                                    int8_t sockindex);

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CONN_ಒಳ_SHUTDOWN(c,i)   ((c)->shutdown.ಆರಂಭ_ms[(i)] >= 0)

/* Shutdown the connection at `sockindex` non-blocking.
 * Will start the shutdown timer if not already set.
 * Return CURLE_OK and *done == FALSE if not finished. */
CURLcode Curl_cshutdn_try_once_idx(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   int8_t sockindex, bool *ಮುಗಿದ);

/* Run the shutdown of the connection once non-blocking for both
 * socket indices. Will start the shutdown timer if not already set.
 * Shortly attach/detach the admin handle to `conn` while doing so.
 * `done` will be set TRUE if any error was encountered or if
 * the connection was shut down completely. */
ಶೂನ್ಯ Curl_cshutdn_try_once(ರಚನೆ Curl_easy *admin,
                           ರಚನೆ connectdata *conn, bool *ಮುಗಿದ);

/* A `cshutdown` is always owned by a multi handle to maintain
 * the connections to be shut down. It registers timers and
 * sockets to monitor via the multi handle. */
ರಚನೆ cshutdn {
  ರಚನೆ ptrarray conns;    /* connections being shut down */
};

/* Init as part of the given multi handle. */
ಶೂನ್ಯ Curl_cshutdn_init(ರಚನೆ cshutdn *cshutdn);

/* Terminate all remaining connections and free resources. */
ಶೂನ್ಯ Curl_cshutdn_destroy(ರಚನೆ cshutdn *cshutdn,
                          ರಚನೆ Curl_easy *admin);

/* Number of connections being shut down. */
size_t Curl_cshutdn_ಎಣಿಕೆ(ರಚನೆ cshutdn *cshutdn);

/* Number of connections to the destination being shut down. */
size_t Curl_cshutdn_dest_ಎಣಿಕೆ(ರಚನೆ cshutdn *cshutdn,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *destination);

/* Close the oldest `count` connections in shutdown to destination or,
 * when destination is NULL for any destination.
 * Return the number of connections closed. */
size_t Curl_cshutdn_close_oldest(ರಚನೆ cshutdn *cshutdn,
                                 ರಚನೆ Curl_easy *admin,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *destination,
                                 size_t ಎಣಿಕೆ_2);

/* Add a connection to have it shut down. Terminate the oldest
 * connection when shutdowns exceed max_shutdowns. */
ಶೂನ್ಯ Curl_cshutdn_ಕೂಡಿಸು(ರಚನೆ cshutdn *cshutdn,
                      ರಚನೆ Curl_multi *multi,
                      ರಚನೆ connectdata *conn,
                      size_t max_shutdowns);

/* Add sockets and POLLIN/OUT flags for connections being shut down. */
CURLcode Curl_cshutdn_ಕೂಡಿಸು_pollfds(ರಚನೆ cshutdn *cshutdn,
                                  ರಚನೆ Curl_easy *admin,
                                  ರಚನೆ curl_pollfds *cpfds);

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ Curl_cshutdn_ಕೂಡಿಸು_waitfds(ರಚನೆ cshutdn *cshutdn,
                                      ರಚನೆ Curl_easy *admin,
                                      ರಚನೆ Curl_waitfds *cwfds);

ಶೂನ್ಯ Curl_cshutdn_setfds(ರಚನೆ cshutdn *cshutdn,
                         ರಚನೆ Curl_easy *admin,
                         fd_set *read_fd_ಗಣ, fd_set *write_fd_ಗಣ,
                         ಪೂರ್ಣಾಂಕ *maxfd);

/* Run maintenance on all connections. */
ಶೂನ್ಯ Curl_cshutdn_perform(ರಚನೆ cshutdn *cshutdn,
                          ರಚನೆ Curl_easy *admin,
                          ರಚನೆ Curl_sigpipe_ctx *sigpipe_ctx);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_CSHUTDN_H */
