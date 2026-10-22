#اگر_غیر_تعریف_شدہ سرخی_CURL_CONNCACHE_H
#تعریف_کرو سرخی_CURL_CONNCACHE_H
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

ساخت connectdata;
ساخت Curl_easy;
ساخت curl_pollfds;
ساخت Curl_waitfds;
ساخت Curl_multi;
ساخت Curl_share;

/**
 * Close and destroy the connection.
 * If the connection is in a cpool, remove it.
 * If a `cshutdn` is available (e.g. data has a multi handle),
 * pass the connection to that for controlled shutdown.
 * Otherwise terminate it right away.
 * Takes ownership of `conn`.
 * `data` should not be attached to a connection.
 */
خالی Curl_conn_close(ساخت Curl_easy *ڈیٹا,
                     ساخت connectdata *conn,
                     bool aborted);

ساخت cpool {
  ساخت uint32_tbl conns; /* connections added to this pool */
  ساخت uint32_bset idles; /* pool_ids of conns being idle */
  ساخت Curl_hash dest2bundle; /* conn destination sets */
  curl_off_t اگلا_connection_id;
  curl_off_t اگلا_easy_id;
  ساخت curltime آخری_cleanup;
  ساخت Curl_share *share; /* != NULL if pool belongs to share */
  ثنائی_ہندسہ(locked);
  ثنائی_ہندسہ(initialized);
  ثنائی_ہندسہ(اندر_shutdown);
};

/* Get connection pool instance for data or NULL if none exists */
ساخت cpool *Curl_cpool_get_instance(ساخت Curl_easy *ڈیٹا);

/* Init the pool, pass multi only if pool is owned by it.
 * Cannot fail.
 */
خالی Curl_cpool_init(ساخت cpool *cpool,
                     ساخت Curl_share *share,
                     size_t سائز);

/* Destroy all connections and free all members */
خالی Curl_cpool_destroy(ساخت cpool *cpool,
                        ساخت Curl_easy *admin);

/* Init the transfer to be used within its connection pool.
 * Assigns `data->id`. */
خالی Curl_cpool_xfer_init(ساخت Curl_easy *ڈیٹا);

/* Get the connection last used by data,
 * if there was one and it still exists. */
ساخت connectdata *Curl_cpool_get_آخری_conn(ساخت Curl_easy *ڈیٹا);

/* Add the connection to the pool. */
CURLcode Curl_cpool_جوڑو(ساخت Curl_easy *ڈیٹا,
                        ساخت connectdata *conn,
                        uint32_t max_کل,
                        uint32_t max_host,
                        ثابت ساخت curltime *pnow) WARN_غیر_مستعمل_نتیجہ;

/* Connection was used at the given time. */
خالی Curl_cpool_conn_was_مستعمل(ساخت Curl_easy *ڈیٹا,
                              ساخت connectdata *conn,
                              ثابت ساخت curltime *pnow);

/* Return connection age in milliseconds since its creation. */
timediff_t Curl_cpool_conn_age_ms(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn,
                                  ثابت ساخت curltime *pnow);

قسمتعریف شمار {
  CPOOL_مطابقت_ملا, /* The passed `conn` matches, stop looking further */
  CPOOL_مطابقت_CONT,  /* No match, continue looking */
  CPOOL_مطابقت_CLOSE, /* No match, close `conn`, continue looking */
  CPOOL_مطابقت_TOO_OLD /* No match, `conn` is too old, close when idle */
} cpool_مطابقت_نتیجہ;

/* Return of conn is suitable. If so, stops iteration. */
قسمتعریف cpool_مطابقت_نتیجہ
 Curl_cpool_conn_مطابقت_cb(ساخت connectdata *conn, خالی *userdata);

/* Act on the result of the find, may override it. */
قسمتعریف bool Curl_cpool_مکمل_مطابقت_cb(خالی *userdata);

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
bool Curl_cpool_ڈھونڈو(ساخت Curl_easy *ڈیٹا,
                     ثابت حرف *destination,
                     bool prune_dead,
                     ثابت ساخت curltime *pnow,
                     Curl_cpool_conn_مطابقت_cb *conn_cb,
                     Curl_cpool_مکمل_مطابقت_cb *مکمل_cb,
                     خالی *userdata);

/**
 * Perform upkeep actions on connections in the transfer's pool.
 */
CURLcode Curl_cpool_upkeep(ساخت Curl_easy *ڈیٹا);

قسمتعریف شمار {
  CPOOL_DO_KEEP, /* Keep the connection, still in use */
  CPOOL_DO_IDLE, /* Connection is idle, may get closed now */
  CPOOL_DO_CLOSE, /* Close the connection (clean) */
  CPOOL_DO_TERMINATE /* Terminate the connection (unclean) */
} cpool_do_نتیجہ;

قسمتعریف cpool_do_نتیجہ Curl_cpool_return_cb(ساخت Curl_easy *ڈیٹا,
                                             ساخت connectdata *conn,
                                             خالی *cbdata,
                                             ثابت ساخت curltime *pnow);

/**
 * Invoked the callback for the given data + connection under the
 * connection pool's lock.
 * The callback is always invoked, even if the transfer has no connection
 * pool associated.
 */
خالی Curl_cpool_return(ساخت Curl_easy *ڈیٹا,
                       ساخت connectdata *conn,
                       Curl_cpool_return_cb *cb, خالی *cbdata,
                       ثابت ساخت curltime *pnow);

/* Close all unused connections, prevent reuse of existing ones. */
خالی Curl_cpool_nw_بدلا(ساخت cpool *cpool, ساخت Curl_easy *admin);

/* Return TRUE iff the given connection is considered healthy, e.g.
 * usable for more transfers. */
bool Curl_cpool_conn_seems_healthy(ساخت connectdata *conn,
                                   ساخت Curl_easy *ڈیٹا,
                                   ثابت ساخت curltime *pnow);

#اگر_ختم /* HEADER_CURL_CONNCACHE_H */
