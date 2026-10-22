#अगर_अघोषित HEADER_CURL_CONNCACHE_H
#घोषणा HEADER_CURL_CONNCACHE_H
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
#समावेश "uint-bset.h"
#समावेश "uint-table.h"
#समावेश "curlx/timeval.h"

काष्ठा connectdata;
काष्ठा Curl_easy;
काष्ठा curl_pollfds;
काष्ठा Curl_waitfds;
काष्ठा Curl_multi;
काष्ठा Curl_share;

/**
 * Close and destroy the connection.
 * If the connection is in a cpool, remove it.
 * If a `cshutdn` is available (e.g. data has a multi handle),
 * pass the connection to that for controlled shutdown.
 * Otherwise terminate it right away.
 * Takes ownership of `conn`.
 * `data` should not be attached to a connection.
 */
व्योम Curl_conn_close(काष्ठा Curl_easy *तथ्य,
                     काष्ठा connectdata *conn,
                     bool aborted);

काष्ठा cpool {
  काष्ठा uint32_tbl conns; /* connections added to this pool */
  काष्ठा uint32_bset idles; /* pool_ids of conns being idle */
  काष्ठा Curl_hash dest2bundle; /* conn destination sets */
  curl_off_t अग्र_connection_id;
  curl_off_t अग्र_easy_id;
  काष्ठा curltime last_cleanup;
  काष्ठा Curl_share *share; /* != NULL if pool belongs to share */
  BIT(locked);
  BIT(initialized);
  BIT(in_shutdown);
};

/* Get connection pool instance for data or NULL if none exists */
काष्ठा cpool *Curl_cpool_get_instance(काष्ठा Curl_easy *तथ्य);

/* Init the pool, pass multi only if pool is owned by it.
 * Cannot fail.
 */
व्योम Curl_cpool_init(काष्ठा cpool *cpool,
                     काष्ठा Curl_share *share,
                     माप_प्रकार आकार);

/* Destroy all connections and free all members */
व्योम Curl_cpool_destroy(काष्ठा cpool *cpool,
                        काष्ठा Curl_easy *admin);

/* Init the transfer to be used within its connection pool.
 * Assigns `data->id`. */
व्योम Curl_cpool_xfer_init(काष्ठा Curl_easy *तथ्य);

/* Get the connection last used by data,
 * if there was one and it still exists. */
काष्ठा connectdata *Curl_cpool_get_last_conn(काष्ठा Curl_easy *तथ्य);

/* Add the connection to the pool. */
CURLcode Curl_cpool_add(काष्ठा Curl_easy *तथ्य,
                        काष्ठा connectdata *conn,
                        uint32_t max_योग,
                        uint32_t max_host,
                        स्थिर काष्ठा curltime *pnow) WARN_UNUSED_परिणाम;

/* Connection was used at the given time. */
व्योम Curl_cpool_conn_was_used(काष्ठा Curl_easy *तथ्य,
                              काष्ठा connectdata *conn,
                              स्थिर काष्ठा curltime *pnow);

/* Return connection age in milliseconds since its creation. */
timediff_t Curl_cpool_conn_age_ms(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn,
                                  स्थिर काष्ठा curltime *pnow);

प्रकार क्रमागत {
  CPOOL_MATCH_FOUND, /* The passed `conn` matches, stop looking further */
  CPOOL_MATCH_CONT,  /* No match, continue looking */
  CPOOL_MATCH_CLOSE, /* No match, close `conn`, continue looking */
  CPOOL_MATCH_TOO_OLD /* No match, `conn` is too old, close when idle */
} cpool_match_परिणाम;

/* Return of conn is suitable. If so, stops iteration. */
प्रकार cpool_match_परिणाम
 Curl_cpool_conn_match_cb(काष्ठा connectdata *conn, व्योम *userdata);

/* Act on the result of the find, may override it. */
प्रकार bool Curl_cpool_done_match_cb(व्योम *userdata);

/**
 * Find a connection in the pool matching `destination`.
 * All callbacks are invoked while the pool's lock is held.
 * @param data        current transfer
 * @param destination match against `conn->destination` in pool
 * @param prune_dead  perform reaping of dead connections at start
 * @param pnow        timestamp of operation
 * @param conn_cb     must be present, called for each connection in the
 *                    bundle until it returns TRUE
 * @return combined result of last conn_db and result_cb or FALSE if no
                      connections were present.
 */
bool Curl_cpool_find(काष्ठा Curl_easy *तथ्य,
                     स्थिर अक्षर *destination,
                     bool prune_dead,
                     स्थिर काष्ठा curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     व्योम *userdata);

/**
 * Perform upkeep actions on connections in the transfer's pool.
 */
CURLcode Curl_cpool_upkeep(काष्ठा Curl_easy *तथ्य);

प्रकार क्रमागत {
  CPOOL_DO_KEEP, /* Keep the connection, still in use */
  CPOOL_DO_IDLE, /* Connection is idle, may get closed now */
  CPOOL_DO_CLOSE, /* Close the connection (clean) */
  CPOOL_DO_TERMINATE /* Terminate the connection (unclean) */
} cpool_do_परिणाम;

प्रकार cpool_do_परिणाम Curl_cpool_return_cb(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा connectdata *conn,
                                             व्योम *cbdata,
                                             स्थिर काष्ठा curltime *pnow);

/**
 * Invoked the callback for the given data + connection under the
 * connection pool's lock.
 * The callback is always invoked, even if the transfer has no connection
 * pool associated.
 */
व्योम Curl_cpool_return(काष्ठा Curl_easy *तथ्य,
                       काष्ठा connectdata *conn,
                       Curl_cpool_return_cb *cb, व्योम *cbdata,
                       स्थिर काष्ठा curltime *pnow);

/* Close all unused connections, prevent reuse of existing ones. */
व्योम Curl_cpool_nw_changed(काष्ठा cpool *cpool, काष्ठा Curl_easy *admin);

/* Return TRUE iff the given connection is considered healthy, e.g.
 * usable for more transfers. */
bool Curl_cpool_conn_seems_healthy(काष्ठा connectdata *conn,
                                   काष्ठा Curl_easy *तथ्य,
                                   स्थिर काष्ठा curltime *pnow);

#पूर्ण_अगर /* HEADER_CURL_CONNCACHE_H */
