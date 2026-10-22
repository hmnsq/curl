#अगर_अघोषित HEADER_CURL_CSHUTDN_H
#घोषणा HEADER_CURL_CSHUTDN_H
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
#समावेश "ptrarray.h"

काष्ठा connectdata;
काष्ठा Curl_easy;
काष्ठा curl_pollfds;
काष्ठा Curl_waitfds;
काष्ठा Curl_multi;
काष्ठा Curl_share;
काष्ठा Curl_sigpipe_ctx;


/* Terminates the connection, e.g. closes and destroys it.
 * If `do_shutdown` is TRUE, the shutdown will be run once.
 * Takes ownership of `conn`. `conn` MUST no longer be in
 * a pool or on a shutdown list. */
व्योम Curl_cshutdn_terminate(काष्ठा Curl_easy *admin,
                            काष्ठा connectdata *conn,
                            bool do_shutdown);

/* Start the shutdown timer,
 * marks the connection sockindex as being shut down. */
व्योम Curl_cshutdn_start_timer(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                              पूर्णांक timeout_ms);
/* Clear the shutdown timer at sockindex again. */
व्योम Curl_cshutdn_clear_timer(काष्ठा Curl_easy *तथ्य, int8_t sockindex);

/* return how much time there is left to shutdown the connection at
 * sockindex. Returns 0 if there is no limit or shutdown has not started. */
timediff_t Curl_cshutdn_timeleft_ms(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा connectdata *conn,
                                    int8_t sockindex);

#घोषणा CURL_CONN_IN_SHUTDOWN(c,i)   ((c)->shutdown.start_ms[(i)] >= 0)

/* Shutdown the connection at `sockindex` non-blocking.
 * Will start the shutdown timer if not already set.
 * Return CURLE_OK and *done == FALSE if not finished. */
CURLcode Curl_cshutdn_try_once_idx(काष्ठा Curl_easy *तथ्य,
                                   int8_t sockindex, bool *done);

/* Run the shutdown of the connection once non-blocking for both
 * socket indices. Will start the shutdown timer if not already set.
 * Shortly attach/detach the admin handle to `conn` while doing so.
 * `done` will be set TRUE if any error was encountered or if
 * the connection was shut down completely. */
व्योम Curl_cshutdn_try_once(काष्ठा Curl_easy *admin,
                           काष्ठा connectdata *conn, bool *done);

/* A `cshutdown` is always owned by a multi handle to maintain
 * the connections to be shut down. It registers timers and
 * sockets to monitor via the multi handle. */
काष्ठा cshutdn {
  काष्ठा ptrarray conns;    /* connections being shut down */
};

/* Init as part of the given multi handle. */
व्योम Curl_cshutdn_init(काष्ठा cshutdn *cshutdn);

/* Terminate all remaining connections and free resources. */
व्योम Curl_cshutdn_destroy(काष्ठा cshutdn *cshutdn,
                          काष्ठा Curl_easy *admin);

/* Number of connections being shut down. */
माप_प्रकार Curl_cshutdn_गिनती(काष्ठा cshutdn *cshutdn);

/* Number of connections to the destination being shut down. */
माप_प्रकार Curl_cshutdn_dest_गिनती(काष्ठा cshutdn *cshutdn,
                               स्थिर अक्षर *destination);

/* Close the oldest `count` connections in shutdown to destination or,
 * when destination is NULL for any destination.
 * Return the number of connections closed. */
माप_प्रकार Curl_cshutdn_close_oldest(काष्ठा cshutdn *cshutdn,
                                 काष्ठा Curl_easy *admin,
                                 स्थिर अक्षर *destination,
                                 माप_प्रकार गिनती);

/* Add a connection to have it shut down. Terminate the oldest
 * connection when shutdowns exceed max_shutdowns. */
व्योम Curl_cshutdn_add(काष्ठा cshutdn *cshutdn,
                      काष्ठा Curl_multi *multi,
                      काष्ठा connectdata *conn,
                      माप_प्रकार max_shutdowns);

/* Add sockets and POLLIN/OUT flags for connections being shut down. */
CURLcode Curl_cshutdn_add_pollfds(काष्ठा cshutdn *cshutdn,
                                  काष्ठा Curl_easy *admin,
                                  काष्ठा curl_pollfds *cpfds);

अचिन्हित पूर्णांक Curl_cshutdn_add_waitfds(काष्ठा cshutdn *cshutdn,
                                      काष्ठा Curl_easy *admin,
                                      काष्ठा Curl_waitfds *cwfds);

व्योम Curl_cshutdn_setfds(काष्ठा cshutdn *cshutdn,
                         काष्ठा Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         पूर्णांक *maxfd);

/* Run maintenance on all connections. */
व्योम Curl_cshutdn_perform(काष्ठा cshutdn *cshutdn,
                          काष्ठा Curl_easy *admin,
                          काष्ठा Curl_sigpipe_ctx *sigpipe_ctx);

#पूर्ण_अगर /* HEADER_CURL_CSHUTDN_H */
