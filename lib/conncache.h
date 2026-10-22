#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_CONNCACHE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_CONNCACHE_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "uint-bset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uint-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h"

ਬਣਤਰ connectdata;
ਬਣਤਰ Curl_easy;
ਬਣਤਰ curl_pollfds;
ਬਣਤਰ Curl_waitfds;
ਬਣਤਰ Curl_multi;
ਬਣਤਰ Curl_share;

/**
 * Close and destroy the connection.
 * If the connection is in a cpool, remove it.
 * If a `cshutdn` is available (e.g. data has a multi handle),
 * pass the connection to that for controlled shutdown.
 * Otherwise terminate it right away.
 * Takes ownership of `conn`.
 * `data` should not be attached to a connection.
 */
ਖਾਲੀ Curl_conn_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                     ਬਣਤਰ connectdata *conn,
                     bool aborted);

ਬਣਤਰ cpool {
  ਬਣਤਰ uint32_tbl conns; /* connections added to this pool */
  ਬਣਤਰ uint32_bset idles; /* pool_ids of conns being idle */
  ਬਣਤਰ Curl_hash dest2bundle; /* conn destination sets */
  curl_off_t ਅਗਲਾ_connection_id;
  curl_off_t ਅਗਲਾ_easy_id;
  ਬਣਤਰ curltime ਆਖਰੀ_cleanup;
  ਬਣਤਰ Curl_share *share; /* != NULL if pool belongs to share */
  ਦੋਆਧਾਰੀ_ਅੰਕ(locked);
  ਦੋਆਧਾਰੀ_ਅੰਕ(initialized);
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਵਿੱਚ_shutdown);
};

/* Get connection pool instance for data or NULL if none exists */
ਬਣਤਰ cpool *Curl_cpool_get_instance(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* Init the pool, pass multi only if pool is owned by it.
 * Cannot fail.
 */
ਖਾਲੀ Curl_cpool_init(ਬਣਤਰ cpool *cpool,
                     ਬਣਤਰ Curl_share *share,
                     size_t ਮਾਪ);

/* Destroy all connections and free all members */
ਖਾਲੀ Curl_cpool_destroy(ਬਣਤਰ cpool *cpool,
                        ਬਣਤਰ Curl_easy *admin);

/* Init the transfer to be used within its connection pool.
 * Assigns `data->id`. */
ਖਾਲੀ Curl_cpool_xfer_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* Get the connection last used by data,
 * if there was one and it still exists. */
ਬਣਤਰ connectdata *Curl_cpool_get_ਆਖਰੀ_conn(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* Add the connection to the pool. */
CURLcode Curl_cpool_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ connectdata *conn,
                        uint32_t max_ਕੁੱਲ,
                        uint32_t max_host,
                        ਅਚਲ ਬਣਤਰ curltime *pnow) WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;

/* Connection was used at the given time. */
ਖਾਲੀ Curl_cpool_conn_was_ਵਰਤਿਆ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ connectdata *conn,
                              ਅਚਲ ਬਣਤਰ curltime *pnow);

/* Return connection age in milliseconds since its creation. */
timediff_t Curl_cpool_conn_age_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn,
                                  ਅਚਲ ਬਣਤਰ curltime *pnow);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CPOOL_ਮੇਲ_ਮਿਲਿਆ, /* The passed `conn` matches, stop looking further */
  CPOOL_ਮੇਲ_CONT,  /* No match, continue looking */
  CPOOL_ਮੇਲ_CLOSE, /* No match, close `conn`, continue looking */
  CPOOL_ਮੇਲ_TOO_OLD /* No match, `conn` is too old, close when idle */
} cpool_ਮੇਲ_ਨਤੀਜਾ;

/* Return of conn is suitable. If so, stops iteration. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ cpool_ਮੇਲ_ਨਤੀਜਾ
 Curl_cpool_conn_ਮੇਲ_cb(ਬਣਤਰ connectdata *conn, ਖਾਲੀ *userdata);

/* Act on the result of the find, may override it. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ bool Curl_cpool_ਪੂਰਾ_ਮੇਲ_cb(ਖਾਲੀ *userdata);

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
bool Curl_cpool_ਲੱਭੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                     ਅਚਲ ਅੱਖਰ *destination,
                     bool prune_dead,
                     ਅਚਲ ਬਣਤਰ curltime *pnow,
                     Curl_cpool_conn_ਮੇਲ_cb *conn_cb,
                     Curl_cpool_ਪੂਰਾ_ਮੇਲ_cb *ਪੂਰਾ_cb,
                     ਖਾਲੀ *userdata);

/**
 * Perform upkeep actions on connections in the transfer's pool.
 */
CURLcode Curl_cpool_upkeep(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CPOOL_DO_KEEP, /* Keep the connection, still in use */
  CPOOL_DO_IDLE, /* Connection is idle, may get closed now */
  CPOOL_DO_CLOSE, /* Close the connection (clean) */
  CPOOL_DO_TERMINATE /* Terminate the connection (unclean) */
} cpool_do_ਨਤੀਜਾ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ cpool_do_ਨਤੀਜਾ Curl_cpool_return_cb(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ connectdata *conn,
                                             ਖਾਲੀ *cbdata,
                                             ਅਚਲ ਬਣਤਰ curltime *pnow);

/**
 * Invoked the callback for the given data + connection under the
 * connection pool's lock.
 * The callback is always invoked, even if the transfer has no connection
 * pool associated.
 */
ਖਾਲੀ Curl_cpool_return(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       ਬਣਤਰ connectdata *conn,
                       Curl_cpool_return_cb *cb, ਖਾਲੀ *cbdata,
                       ਅਚਲ ਬਣਤਰ curltime *pnow);

/* Close all unused connections, prevent reuse of existing ones. */
ਖਾਲੀ Curl_cpool_nw_ਬਦਲਿਆ(ਬਣਤਰ cpool *cpool, ਬਣਤਰ Curl_easy *admin);

/* Return TRUE iff the given connection is considered healthy, e.g.
 * usable for more transfers. */
bool Curl_cpool_conn_seems_healthy(ਬਣਤਰ connectdata *conn,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਅਚਲ ਬਣਤਰ curltime *pnow);

#ਜੇ_ਅੰਤ /* HEADER_CURL_CONNCACHE_H */
