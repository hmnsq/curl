#నిర్వచితం_కాకపోతే శీర్షిక_CURL_CONNCACHE_H
#నిర్వచించు శీర్షిక_CURL_CONNCACHE_H
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
#అన్తర్భావ "uint-bset.h"
#అన్తర్భావ "uint-table.h"
#అన్తర్భావ "curlx/timeval.h"

నిర్మాణం connectdata;
నిర్మాణం Curl_easy;
నిర్మాణం curl_pollfds;
నిర్మాణం Curl_waitfds;
నిర్మాణం Curl_multi;
నిర్మాణం Curl_share;

/**
 * Close and destroy the connection.
 * If the connection is in a cpool, remove it.
 * If a `cshutdn` is available (e.g. data has a multi handle),
 * pass the connection to that for controlled shutdown.
 * Otherwise terminate it right away.
 * Takes ownership of `conn`.
 * `data` should not be attached to a connection.
 */
శూన్య Curl_conn_close(నిర్మాణం Curl_easy *దత్తాంశం,
                     నిర్మాణం connectdata *conn,
                     bool aborted);

నిర్మాణం cpool {
  నిర్మాణం uint32_tbl conns; /* connections added to this pool */
  నిర్మాణం uint32_bset idles; /* pool_ids of conns being idle */
  నిర్మాణం Curl_hash dest2bundle; /* conn destination sets */
  curl_off_t తదుపరి_connection_id;
  curl_off_t తదుపరి_easy_id;
  నిర్మాణం curltime చివరి_cleanup;
  నిర్మాణం Curl_share *share; /* != NULL if pool belongs to share */
  బిట్టు(locked);
  బిట్టు(initialized);
  బిట్టు(లోపల_shutdown);
};

/* Get connection pool instance for data or NULL if none exists */
నిర్మాణం cpool *Curl_cpool_get_instance(నిర్మాణం Curl_easy *దత్తాంశం);

/* Init the pool, pass multi only if pool is owned by it.
 * Cannot fail.
 */
శూన్య Curl_cpool_init(నిర్మాణం cpool *cpool,
                     నిర్మాణం Curl_share *share,
                     size_t కొలత);

/* Destroy all connections and free all members */
శూన్య Curl_cpool_destroy(నిర్మాణం cpool *cpool,
                        నిర్మాణం Curl_easy *admin);

/* Init the transfer to be used within its connection pool.
 * Assigns `data->id`. */
శూన్య Curl_cpool_xfer_init(నిర్మాణం Curl_easy *దత్తాంశం);

/* Get the connection last used by data,
 * if there was one and it still exists. */
నిర్మాణం connectdata *Curl_cpool_get_చివరి_conn(నిర్మాణం Curl_easy *దత్తాంశం);

/* Add the connection to the pool. */
CURLcode Curl_cpool_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం connectdata *conn,
                        uint32_t max_మొత్తం,
                        uint32_t max_host,
                        స్థిరరాశి నిర్మాణం curltime *pnow) WARN_వాడని_ఫలితం;

/* Connection was used at the given time. */
శూన్య Curl_cpool_conn_was_వాడిన(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn,
                              స్థిరరాశి నిర్మాణం curltime *pnow);

/* Return connection age in milliseconds since its creation. */
timediff_t Curl_cpool_conn_age_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn,
                                  స్థిరరాశి నిర్మాణం curltime *pnow);

రకనిర్వచనం గణన {
  CPOOL_సరిపోలిక_దొరికింది, /* The passed `conn` matches, stop looking further */
  CPOOL_సరిపోలిక_CONT,  /* No match, continue looking */
  CPOOL_సరిపోలిక_CLOSE, /* No match, close `conn`, continue looking */
  CPOOL_సరిపోలిక_TOO_OLD /* No match, `conn` is too old, close when idle */
} cpool_సరిపోలిక_ఫలితం;

/* Return of conn is suitable. If so, stops iteration. */
రకనిర్వచనం cpool_సరిపోలిక_ఫలితం
 Curl_cpool_conn_సరిపోలిక_cb(నిర్మాణం connectdata *conn, శూన్య *userdata);

/* Act on the result of the find, may override it. */
రకనిర్వచనం bool Curl_cpool_పూర్తి_సరిపోలిక_cb(శూన్య *userdata);

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
bool Curl_cpool_కనుగొను(నిర్మాణం Curl_easy *దత్తాంశం,
                     స్థిరరాశి అక్షరం *destination,
                     bool prune_dead,
                     స్థిరరాశి నిర్మాణం curltime *pnow,
                     Curl_cpool_conn_సరిపోలిక_cb *conn_cb,
                     Curl_cpool_పూర్తి_సరిపోలిక_cb *పూర్తి_cb,
                     శూన్య *userdata);

/**
 * Perform upkeep actions on connections in the transfer's pool.
 */
CURLcode Curl_cpool_upkeep(నిర్మాణం Curl_easy *దత్తాంశం);

రకనిర్వచనం గణన {
  CPOOL_DO_KEEP, /* Keep the connection, still in use */
  CPOOL_DO_IDLE, /* Connection is idle, may get closed now */
  CPOOL_DO_CLOSE, /* Close the connection (clean) */
  CPOOL_DO_TERMINATE /* Terminate the connection (unclean) */
} cpool_do_ఫలితం;

రకనిర్వచనం cpool_do_ఫలితం Curl_cpool_return_cb(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం connectdata *conn,
                                             శూన్య *cbdata,
                                             స్థిరరాశి నిర్మాణం curltime *pnow);

/**
 * Invoked the callback for the given data + connection under the
 * connection pool's lock.
 * The callback is always invoked, even if the transfer has no connection
 * pool associated.
 */
శూన్య Curl_cpool_return(నిర్మాణం Curl_easy *దత్తాంశం,
                       నిర్మాణం connectdata *conn,
                       Curl_cpool_return_cb *cb, శూన్య *cbdata,
                       స్థిరరాశి నిర్మాణం curltime *pnow);

/* Close all unused connections, prevent reuse of existing ones. */
శూన్య Curl_cpool_nw_మారింది(నిర్మాణం cpool *cpool, నిర్మాణం Curl_easy *admin);

/* Return TRUE iff the given connection is considered healthy, e.g.
 * usable for more transfers. */
bool Curl_cpool_conn_seems_healthy(నిర్మాణం connectdata *conn,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   స్థిరరాశి నిర్మాణం curltime *pnow);

#అయితే_ముగింపు /* HEADER_CURL_CONNCACHE_H */
