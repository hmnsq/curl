#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_CONNCACHE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_CONNCACHE_H
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
#ಅನ್ತರ್ಭಾವ "uint-bset.h"
#ಅನ್ತರ್ಭಾವ "uint-table.h"
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"

ರಚನೆ connectdata;
ರಚನೆ Curl_easy;
ರಚನೆ curl_pollfds;
ರಚನೆ Curl_waitfds;
ರಚನೆ Curl_multi;
ರಚನೆ Curl_share;

/**
 * Close and destroy the connection.
 * If the connection is in a cpool, remove it.
 * If a `cshutdn` is available (e.g. data has a multi handle),
 * pass the connection to that for controlled shutdown.
 * Otherwise terminate it right away.
 * Takes ownership of `conn`.
 * `data` should not be attached to a connection.
 */
ಶೂನ್ಯ Curl_conn_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                     ರಚನೆ connectdata *conn,
                     bool aborted);

ರಚನೆ cpool {
  ರಚನೆ uint32_tbl conns; /* connections added to this pool */
  ರಚನೆ uint32_bset idles; /* pool_ids of conns being idle */
  ರಚನೆ Curl_hash dest2bundle; /* conn destination sets */
  curl_off_t ಮುಂದಿನ_connection_id;
  curl_off_t ಮುಂದಿನ_easy_id;
  ರಚನೆ curltime ಕೊನೆಯ_cleanup;
  ರಚನೆ Curl_share *share; /* != NULL if pool belongs to share */
  ಬಿಟ್(locked);
  ಬಿಟ್(initialized);
  ಬಿಟ್(ಒಳ_shutdown);
};

/* Get connection pool instance for data or NULL if none exists */
ರಚನೆ cpool *Curl_cpool_get_instance(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* Init the pool, pass multi only if pool is owned by it.
 * Cannot fail.
 */
ಶೂನ್ಯ Curl_cpool_init(ರಚನೆ cpool *cpool,
                     ರಚನೆ Curl_share *share,
                     size_t ಗಾತ್ರ_2);

/* Destroy all connections and free all members */
ಶೂನ್ಯ Curl_cpool_destroy(ರಚನೆ cpool *cpool,
                        ರಚನೆ Curl_easy *admin);

/* Init the transfer to be used within its connection pool.
 * Assigns `data->id`. */
ಶೂನ್ಯ Curl_cpool_xfer_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* Get the connection last used by data,
 * if there was one and it still exists. */
ರಚನೆ connectdata *Curl_cpool_get_ಕೊನೆಯ_conn(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* Add the connection to the pool. */
CURLcode Curl_cpool_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ connectdata *conn,
                        uint32_t max_ಒಟ್ಟು,
                        uint32_t max_host,
                        ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow) WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;

/* Connection was used at the given time. */
ಶೂನ್ಯ Curl_cpool_conn_was_ಬಳಸಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ connectdata *conn,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);

/* Return connection age in milliseconds since its creation. */
timediff_t Curl_cpool_conn_age_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CPOOL_ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು, /* The passed `conn` matches, stop looking further */
  CPOOL_ಹೊಂದಿಕೆ_CONT,  /* No match, continue looking */
  CPOOL_ಹೊಂದಿಕೆ_CLOSE, /* No match, close `conn`, continue looking */
  CPOOL_ಹೊಂದಿಕೆ_TOO_OLD /* No match, `conn` is too old, close when idle */
} cpool_ಹೊಂದಿಕೆ_ಫಲಿತಾಂಶ;

/* Return of conn is suitable. If so, stops iteration. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ cpool_ಹೊಂದಿಕೆ_ಫಲಿತಾಂಶ
 Curl_cpool_conn_ಹೊಂದಿಕೆ_cb(ರಚನೆ connectdata *conn, ಶೂನ್ಯ *userdata);

/* Act on the result of the find, may override it. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ bool Curl_cpool_ಮುಗಿದ_ಹೊಂದಿಕೆ_cb(ಶೂನ್ಯ *userdata);

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
bool Curl_cpool_ಕಂಡುಹಿಡಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *destination,
                     bool prune_dead,
                     ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow,
                     Curl_cpool_conn_ಹೊಂದಿಕೆ_cb *conn_cb,
                     Curl_cpool_ಮುಗಿದ_ಹೊಂದಿಕೆ_cb *ಮುಗಿದ_cb,
                     ಶೂನ್ಯ *userdata);

/**
 * Perform upkeep actions on connections in the transfer's pool.
 */
CURLcode Curl_cpool_upkeep(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CPOOL_DO_KEEP, /* Keep the connection, still in use */
  CPOOL_DO_IDLE, /* Connection is idle, may get closed now */
  CPOOL_DO_CLOSE, /* Close the connection (clean) */
  CPOOL_DO_TERMINATE /* Terminate the connection (unclean) */
} cpool_do_ಫಲಿತಾಂಶ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ cpool_do_ಫಲಿತಾಂಶ Curl_cpool_return_cb(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ connectdata *conn,
                                             ಶೂನ್ಯ *cbdata,
                                             ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);

/**
 * Invoked the callback for the given data + connection under the
 * connection pool's lock.
 * The callback is always invoked, even if the transfer has no connection
 * pool associated.
 */
ಶೂನ್ಯ Curl_cpool_return(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                       ರಚನೆ connectdata *conn,
                       Curl_cpool_return_cb *cb, ಶೂನ್ಯ *cbdata,
                       ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);

/* Close all unused connections, prevent reuse of existing ones. */
ಶೂನ್ಯ Curl_cpool_nw_ಬದಲಾಯಿತು(ರಚನೆ cpool *cpool, ರಚನೆ Curl_easy *admin);

/* Return TRUE iff the given connection is considered healthy, e.g.
 * usable for more transfers. */
bool Curl_cpool_conn_seems_healthy(ರಚನೆ connectdata *conn,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_CONNCACHE_H */
