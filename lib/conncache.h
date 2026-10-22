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
#অন্তর্ভাব "uint-bset.h"
#অন্তর্ভাব "uint-table.h"
#অন্তর্ভাব "curlx/timeval.h"

কাঠামো connectdata;
কাঠামো Curl_easy;
কাঠামো curl_pollfds;
কাঠামো Curl_waitfds;
কাঠামো Curl_multi;
কাঠামো Curl_share;

/**
 * Close and destroy the connection.
 * If the connection is in a cpool, remove it.
 * If a `cshutdn` is available (e.g. data has a multi handle),
 * pass the connection to that for controlled shutdown.
 * Otherwise terminate it right away.
 * Takes ownership of `conn`.
 * `data` should not be attached to a connection.
 */
শূন্য Curl_conn_close(কাঠামো Curl_easy *তথ্য,
                     কাঠামো connectdata *conn,
                     bool aborted);

কাঠামো cpool {
  কাঠামো uint32_tbl conns; /* connections added to this pool */
  কাঠামো uint32_bset idles; /* pool_ids of conns being idle */
  কাঠামো Curl_hash dest2bundle; /* conn destination sets */
  curl_off_t পরবর্তী_connection_id;
  curl_off_t পরবর্তী_easy_id;
  কাঠামো curltime last_cleanup;
  কাঠামো Curl_share *share; /* != NULL if pool belongs to share */
  BIT(locked);
  BIT(initialized);
  BIT(in_shutdown);
};

/* Get connection pool instance for data or NULL if none exists */
কাঠামো cpool *Curl_cpool_get_instance(কাঠামো Curl_easy *তথ্য);

/* Init the pool, pass multi only if pool is owned by it.
 * Cannot fail.
 */
শূন্য Curl_cpool_init(কাঠামো cpool *cpool,
                     কাঠামো Curl_share *share,
                     size_t আকার_2);

/* Destroy all connections and free all members */
শূন্য Curl_cpool_destroy(কাঠামো cpool *cpool,
                        কাঠামো Curl_easy *admin);

/* Init the transfer to be used within its connection pool.
 * Assigns `data->id`. */
শূন্য Curl_cpool_xfer_init(কাঠামো Curl_easy *তথ্য);

/* Get the connection last used by data,
 * if there was one and it still exists. */
কাঠামো connectdata *Curl_cpool_get_last_conn(কাঠামো Curl_easy *তথ্য);

/* Add the connection to the pool. */
CURLcode Curl_cpool_add(কাঠামো Curl_easy *তথ্য,
                        কাঠামো connectdata *conn,
                        uint32_t max_যোগ,
                        uint32_t max_host,
                        ধ্রুবক কাঠামো curltime *pnow) WARN_UNUSED_ফলাফল;

/* Connection was used at the given time. */
শূন্য Curl_cpool_conn_was_used(কাঠামো Curl_easy *তথ্য,
                              কাঠামো connectdata *conn,
                              ধ্রুবক কাঠামো curltime *pnow);

/* Return connection age in milliseconds since its creation. */
timediff_t Curl_cpool_conn_age_ms(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata *conn,
                                  ধ্রুবক কাঠামো curltime *pnow);

প্রকারলেখ গণনা {
  CPOOL_MATCH_FOUND, /* The passed `conn` matches, stop looking further */
  CPOOL_MATCH_CONT,  /* No match, continue looking */
  CPOOL_MATCH_CLOSE, /* No match, close `conn`, continue looking */
  CPOOL_MATCH_TOO_OLD /* No match, `conn` is too old, close when idle */
} cpool_match_ফলাফল;

/* Return of conn is suitable. If so, stops iteration. */
প্রকারলেখ cpool_match_ফলাফল
 Curl_cpool_conn_match_cb(কাঠামো connectdata *conn, শূন্য *userdata);

/* Act on the result of the find, may override it. */
প্রকারলেখ bool Curl_cpool_done_match_cb(শূন্য *userdata);

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
bool Curl_cpool_find(কাঠামো Curl_easy *তথ্য,
                     ধ্রুবক অক্ষর *destination,
                     bool prune_dead,
                     ধ্রুবক কাঠামো curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     শূন্য *userdata);

/**
 * Perform upkeep actions on connections in the transfer's pool.
 */
CURLcode Curl_cpool_upkeep(কাঠামো Curl_easy *তথ্য);

প্রকারলেখ গণনা {
  CPOOL_DO_KEEP, /* Keep the connection, still in use */
  CPOOL_DO_IDLE, /* Connection is idle, may get closed now */
  CPOOL_DO_CLOSE, /* Close the connection (clean) */
  CPOOL_DO_TERMINATE /* Terminate the connection (unclean) */
} cpool_do_ফলাফল;

প্রকারলেখ cpool_do_ফলাফল Curl_cpool_return_cb(কাঠামো Curl_easy *তথ্য,
                                             কাঠামো connectdata *conn,
                                             শূন্য *cbdata,
                                             ধ্রুবক কাঠামো curltime *pnow);

/**
 * Invoked the callback for the given data + connection under the
 * connection pool's lock.
 * The callback is always invoked, even if the transfer has no connection
 * pool associated.
 */
শূন্য Curl_cpool_return(কাঠামো Curl_easy *তথ্য,
                       কাঠামো connectdata *conn,
                       Curl_cpool_return_cb *cb, শূন্য *cbdata,
                       ধ্রুবক কাঠামো curltime *pnow);

/* Close all unused connections, prevent reuse of existing ones. */
শূন্য Curl_cpool_nw_changed(কাঠামো cpool *cpool, কাঠামো Curl_easy *admin);

/* Return TRUE iff the given connection is considered healthy, e.g.
 * usable for more transfers. */
bool Curl_cpool_conn_seems_healthy(কাঠামো connectdata *conn,
                                   কাঠামো Curl_easy *তথ্য,
                                   ধ্রুবক কাঠামো curltime *pnow);

#endif /* HEADER_CURL_CONNCACHE_H */
