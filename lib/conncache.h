#ifndef HEADER_CURL_CONNCACHE_H
#define HEADER_CURL_CONNCACHE_H
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
#شامل "uint-bset.h"
#شامل "uint-table.h"
#شامل "curlx/timeval.h"

ساختار connectdata;
ساختار Curl_easy;
ساختار curl_pollfds;
ساختار Curl_waitfds;
ساختار Curl_multi;
ساختار Curl_share;

/**
 * Close and destroy the connection.
 * If the connection is in a cpool, remove it.
 * If a `cshutdn` is available (e.g. data has a multi handle),
 * pass the connection to that for controlled shutdown.
 * Otherwise terminate it right away.
 * Takes ownership of `conn`.
 * `data` should not be attached to a connection.
 */
تهی Curl_conn_close(ساختار Curl_easy *data,
                     ساختار connectdata *conn,
                     bool aborted);

ساختار cpool {
  ساختار uint32_tbl conns; /* connections added to this pool */
  ساختار uint32_bset idles; /* pool_ids of conns being idle */
  ساختار Curl_hash dest2bundle; /* conn destination sets */
  curl_off_t next_connection_id;
  curl_off_t next_easy_id;
  ساختار curltime last_cleanup;
  ساختار Curl_share *share; /* != NULL if pool belongs to share */
  BIT(locked);
  BIT(initialized);
  BIT(in_shutdown);
};

/* Get connection pool instance for data or NULL if none exists */
ساختار cpool *Curl_cpool_get_instance(ساختار Curl_easy *data);

/* Init the pool, pass multi only if pool is owned by it.
 * Cannot fail.
 */
تهی Curl_cpool_init(ساختار cpool *cpool,
                     ساختار Curl_share *share,
                     size_t size);

/* Destroy all connections and free all members */
تهی Curl_cpool_destroy(ساختار cpool *cpool,
                        ساختار Curl_easy *admin);

/* Init the transfer to be used within its connection pool.
 * Assigns `data->id`. */
تهی Curl_cpool_xfer_init(ساختار Curl_easy *data);

/* Get the connection last used by data,
 * if there was one and it still exists. */
ساختار connectdata *Curl_cpool_get_last_conn(ساختار Curl_easy *data);

/* Add the connection to the pool. */
CURLcode Curl_cpool_add(ساختار Curl_easy *data,
                        ساختار connectdata *conn,
                        uint32_t max_total,
                        uint32_t max_host,
                        ثابت ساختار curltime *pnow) WARN_UNUSED_RESULT;

/* Connection was used at the given time. */
تهی Curl_cpool_conn_was_used(ساختار Curl_easy *data,
                              ساختار connectdata *conn,
                              ثابت ساختار curltime *pnow);

/* Return connection age in milliseconds since its creation. */
timediff_t Curl_cpool_conn_age_ms(ساختار Curl_easy *data,
                                  ساختار connectdata *conn,
                                  ثابت ساختار curltime *pnow);

تعریفنوع شمارش {
  CPOOL_MATCH_FOUND, /* The passed `conn` matches, stop looking further */
  CPOOL_MATCH_CONT,  /* No match, continue looking */
  CPOOL_MATCH_CLOSE, /* No match, close `conn`, continue looking */
  CPOOL_MATCH_TOO_OLD /* No match, `conn` is too old, close when idle */
} cpool_match_result;

/* Return of conn is suitable. If so, stops iteration. */
تعریفنوع cpool_match_result
 Curl_cpool_conn_match_cb(ساختار connectdata *conn, تهی *userdata);

/* Act on the result of the find, may override it. */
تعریفنوع bool Curl_cpool_done_match_cb(تهی *userdata);

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
bool Curl_cpool_find(ساختار Curl_easy *data,
                     ثابت نویسه *destination,
                     bool prune_dead,
                     ثابت ساختار curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     تهی *userdata);

/**
 * Perform upkeep actions on connections in the transfer's pool.
 */
CURLcode Curl_cpool_upkeep(ساختار Curl_easy *data);

تعریفنوع شمارش {
  CPOOL_DO_KEEP, /* Keep the connection, still in use */
  CPOOL_DO_IDLE, /* Connection is idle, may get closed now */
  CPOOL_DO_CLOSE, /* Close the connection (clean) */
  CPOOL_DO_TERMINATE /* Terminate the connection (unclean) */
} cpool_do_result;

تعریفنوع cpool_do_result Curl_cpool_return_cb(ساختار Curl_easy *data,
                                             ساختار connectdata *conn,
                                             تهی *cbdata,
                                             ثابت ساختار curltime *pnow);

/**
 * Invoked the callback for the given data + connection under the
 * connection pool's lock.
 * The callback is always invoked, even if the transfer has no connection
 * pool associated.
 */
تهی Curl_cpool_return(ساختار Curl_easy *data,
                       ساختار connectdata *conn,
                       Curl_cpool_return_cb *cb, تهی *cbdata,
                       ثابت ساختار curltime *pnow);

/* Close all unused connections, prevent reuse of existing ones. */
تهی Curl_cpool_nw_changed(ساختار cpool *cpool, ساختار Curl_easy *admin);

/* Return TRUE iff the given connection is considered healthy, e.g.
 * usable for more transfers. */
bool Curl_cpool_conn_seems_healthy(ساختار connectdata *conn,
                                   ساختار Curl_easy *data,
                                   ثابت ساختار curltime *pnow);

#endif /* HEADER_CURL_CONNCACHE_H */
