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
#ଅନ୍ତର୍ଭାଵ "uint-bset.h"
#ଅନ୍ତର୍ଭାଵ "uint-table.h"
#ଅନ୍ତର୍ଭାଵ "curlx/timeval.h"

ସଂରଚନା connectdata;
ସଂରଚନା Curl_easy;
ସଂରଚନା curl_pollfds;
ସଂରଚନା Curl_waitfds;
ସଂରଚନା Curl_multi;
ସଂରଚନା Curl_share;

/**
 * Close and destroy the connection.
 * If the connection is in a cpool, remove it.
 * If a `cshutdn` is available (e.g. data has a multi handle),
 * pass the connection to that for controlled shutdown.
 * Otherwise terminate it right away.
 * Takes ownership of `conn`.
 * `data` should not be attached to a connection.
 */
ଶୂନ୍ୟ Curl_conn_close(ସଂରଚନା Curl_easy *data,
                     ସଂରଚନା connectdata *conn,
                     bool aborted);

ସଂରଚନା cpool {
  ସଂରଚନା uint32_tbl conns; /* connections added to this pool */
  ସଂରଚନା uint32_bset idles; /* pool_ids of conns being idle */
  ସଂରଚନା Curl_hash dest2bundle; /* conn destination sets */
  curl_off_t next_connection_id;
  curl_off_t next_easy_id;
  ସଂରଚନା curltime last_cleanup;
  ସଂରଚନା Curl_share *share; /* != NULL if pool belongs to share */
  BIT(locked);
  BIT(initialized);
  BIT(in_shutdown);
};

/* Get connection pool instance for data or NULL if none exists */
ସଂରଚନା cpool *Curl_cpool_get_instance(ସଂରଚନା Curl_easy *data);

/* Init the pool, pass multi only if pool is owned by it.
 * Cannot fail.
 */
ଶୂନ୍ୟ Curl_cpool_init(ସଂରଚନା cpool *cpool,
                     ସଂରଚନା Curl_share *share,
                     size_t size);

/* Destroy all connections and free all members */
ଶୂନ୍ୟ Curl_cpool_destroy(ସଂରଚନା cpool *cpool,
                        ସଂରଚନା Curl_easy *admin);

/* Init the transfer to be used within its connection pool.
 * Assigns `data->id`. */
ଶୂନ୍ୟ Curl_cpool_xfer_init(ସଂରଚନା Curl_easy *data);

/* Get the connection last used by data,
 * if there was one and it still exists. */
ସଂରଚନା connectdata *Curl_cpool_get_last_conn(ସଂରଚନା Curl_easy *data);

/* Add the connection to the pool. */
CURLcode Curl_cpool_add(ସଂରଚନା Curl_easy *data,
                        ସଂରଚନା connectdata *conn,
                        uint32_t max_total,
                        uint32_t max_host,
                        ଅଚଳ ସଂରଚନା curltime *pnow) WARN_UNUSED_RESULT;

/* Connection was used at the given time. */
ଶୂନ୍ୟ Curl_cpool_conn_was_used(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା connectdata *conn,
                              ଅଚଳ ସଂରଚନା curltime *pnow);

/* Return connection age in milliseconds since its creation. */
timediff_t Curl_cpool_conn_age_ms(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା connectdata *conn,
                                  ଅଚଳ ସଂରଚନା curltime *pnow);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  CPOOL_MATCH_FOUND, /* The passed `conn` matches, stop looking further */
  CPOOL_MATCH_CONT,  /* No match, continue looking */
  CPOOL_MATCH_CLOSE, /* No match, close `conn`, continue looking */
  CPOOL_MATCH_TOO_OLD /* No match, `conn` is too old, close when idle */
} cpool_match_result;

/* Return of conn is suitable. If so, stops iteration. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା cpool_match_result
 Curl_cpool_conn_match_cb(ସଂରଚନା connectdata *conn, ଶୂନ୍ୟ *userdata);

/* Act on the result of the find, may override it. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା bool Curl_cpool_done_match_cb(ଶୂନ୍ୟ *userdata);

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
bool Curl_cpool_find(ସଂରଚନା Curl_easy *data,
                     ଅଚଳ ଅକ୍ଷର *destination,
                     bool prune_dead,
                     ଅଚଳ ସଂରଚନା curltime *pnow,
                     Curl_cpool_conn_match_cb *conn_cb,
                     Curl_cpool_done_match_cb *done_cb,
                     ଶୂନ୍ୟ *userdata);

/**
 * Perform upkeep actions on connections in the transfer's pool.
 */
CURLcode Curl_cpool_upkeep(ସଂରଚନା Curl_easy *data);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  CPOOL_DO_KEEP, /* Keep the connection, still in use */
  CPOOL_DO_IDLE, /* Connection is idle, may get closed now */
  CPOOL_DO_CLOSE, /* Close the connection (clean) */
  CPOOL_DO_TERMINATE /* Terminate the connection (unclean) */
} cpool_do_result;

ପ୍ରକାରବ୍ୟାଖ୍ୟା cpool_do_result Curl_cpool_return_cb(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା connectdata *conn,
                                             ଶୂନ୍ୟ *cbdata,
                                             ଅଚଳ ସଂରଚନା curltime *pnow);

/**
 * Invoked the callback for the given data + connection under the
 * connection pool's lock.
 * The callback is always invoked, even if the transfer has no connection
 * pool associated.
 */
ଶୂନ୍ୟ Curl_cpool_return(ସଂରଚନା Curl_easy *data,
                       ସଂରଚନା connectdata *conn,
                       Curl_cpool_return_cb *cb, ଶୂନ୍ୟ *cbdata,
                       ଅଚଳ ସଂରଚନା curltime *pnow);

/* Close all unused connections, prevent reuse of existing ones. */
ଶୂନ୍ୟ Curl_cpool_nw_changed(ସଂରଚନା cpool *cpool, ସଂରଚନା Curl_easy *admin);

/* Return TRUE iff the given connection is considered healthy, e.g.
 * usable for more transfers. */
bool Curl_cpool_conn_seems_healthy(ସଂରଚନା connectdata *conn,
                                   ସଂରଚନା Curl_easy *data,
                                   ଅଚଳ ସଂରଚନା curltime *pnow);

#endif /* HEADER_CURL_CONNCACHE_H */
