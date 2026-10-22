#यदि_अपरिभाषितम् शीर्षकम्_CURL_CONNCACHE_H
#परिभाषय शीर्षकम्_CURL_CONNCACHE_H
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
#अन्तर्भाव "uint-bset.h"
#अन्तर्भाव "uint-table.h"
#अन्तर्भाव "curlx/timeval.h"

संरचना connectdata;
संरचना Curl_easy;
संरचना curl_pollfds;
संरचना Curl_waitfds;
संरचना Curl_multi;
संरचना Curl_share;

/**
 * Close and destroy the connection.
 * If the connection is in a cpool, remove it.
 * If a `cshutdn` is available (e.g. data has a multi handle),
 * pass the connection to that for controlled shutdown.
 * Otherwise terminate it right away.
 * Takes ownership of `conn`.
 * `data` should not be attached to a connection.
 */
शून्य Curl_conn_close(संरचना Curl_easy *दत्त,
                     संरचना connectdata *conn,
                     bool aborted);

संरचना cpool {
  संरचना uint32_tbl conns; /* connections added to this pool */
  संरचना uint32_bset idles; /* pool_ids of conns being idle */
  संरचना Curl_hash dest2bundle; /* conn destination sets */
  curl_off_t अग्र_connection_id;
  curl_off_t अग्र_easy_id;
  संरचना curltime चरमः_cleanup;
  संरचना Curl_share *share; /* != NULL if pool belongs to share */
  BIT(locked);
  BIT(initialized);
  BIT(अन्तर्_shutdown);
};

/* Get connection pool instance for data or NULL if none exists */
संरचना cpool *Curl_cpool_get_instance(संरचना Curl_easy *दत्त);

/* Init the pool, pass multi only if pool is owned by it.
 * Cannot fail.
 */
शून्य Curl_cpool_init(संरचना cpool *cpool,
                     संरचना Curl_share *share,
                     size_t आकार);

/* Destroy all connections and free all members */
शून्य Curl_cpool_destroy(संरचना cpool *cpool,
                        संरचना Curl_easy *admin);

/* Init the transfer to be used within its connection pool.
 * Assigns `data->id`. */
शून्य Curl_cpool_xfer_init(संरचना Curl_easy *दत्त);

/* Get the connection last used by data,
 * if there was one and it still exists. */
संरचना connectdata *Curl_cpool_get_चरमः_conn(संरचना Curl_easy *दत्त);

/* Add the connection to the pool. */
CURLcode Curl_cpool_सङ्कलय(संरचना Curl_easy *दत्त,
                        संरचना connectdata *conn,
                        uint32_t max_योग,
                        uint32_t max_host,
                        ध्रुव संरचना curltime *pnow) WARN_अप्रयुक्तम्_परिणाम;

/* Connection was used at the given time. */
शून्य Curl_cpool_conn_was_प्रयुक्तम्(संरचना Curl_easy *दत्त,
                              संरचना connectdata *conn,
                              ध्रुव संरचना curltime *pnow);

/* Return connection age in milliseconds since its creation. */
timediff_t Curl_cpool_conn_age_ms(संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn,
                                  ध्रुव संरचना curltime *pnow);

प्रकारनिर्देश गणना {
  CPOOL_मेलनम्_प्राप्तम्, /* The passed `conn` matches, stop looking further */
  CPOOL_मेलनम्_CONT,  /* No match, continue looking */
  CPOOL_मेलनम्_CLOSE, /* No match, close `conn`, continue looking */
  CPOOL_मेलनम्_TOO_OLD /* No match, `conn` is too old, close when idle */
} cpool_मेलनम्_परिणाम;

/* Return of conn is suitable. If so, stops iteration. */
प्रकारनिर्देश cpool_मेलनम्_परिणाम
 Curl_cpool_conn_मेलनम्_cb(संरचना connectdata *conn, शून्य *userdata);

/* Act on the result of the find, may override it. */
प्रकारनिर्देश bool Curl_cpool_समाप्तम्_मेलनम्_cb(शून्य *userdata);

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
bool Curl_cpool_find(संरचना Curl_easy *दत्त,
                     ध्रुव वर्ण *destination,
                     bool prune_dead,
                     ध्रुव संरचना curltime *pnow,
                     Curl_cpool_conn_मेलनम्_cb *conn_cb,
                     Curl_cpool_समाप्तम्_मेलनम्_cb *समाप्तम्_cb,
                     शून्य *userdata);

/**
 * Perform upkeep actions on connections in the transfer's pool.
 */
CURLcode Curl_cpool_upkeep(संरचना Curl_easy *दत्त);

प्रकारनिर्देश गणना {
  CPOOL_DO_KEEP, /* Keep the connection, still in use */
  CPOOL_DO_IDLE, /* Connection is idle, may get closed now */
  CPOOL_DO_CLOSE, /* Close the connection (clean) */
  CPOOL_DO_TERMINATE /* Terminate the connection (unclean) */
} cpool_do_परिणाम;

प्रकारनिर्देश cpool_do_परिणाम Curl_cpool_return_cb(संरचना Curl_easy *दत्त,
                                             संरचना connectdata *conn,
                                             शून्य *cbdata,
                                             ध्रुव संरचना curltime *pnow);

/**
 * Invoked the callback for the given data + connection under the
 * connection pool's lock.
 * The callback is always invoked, even if the transfer has no connection
 * pool associated.
 */
शून्य Curl_cpool_return(संरचना Curl_easy *दत्त,
                       संरचना connectdata *conn,
                       Curl_cpool_return_cb *cb, शून्य *cbdata,
                       ध्रुव संरचना curltime *pnow);

/* Close all unused connections, prevent reuse of existing ones. */
शून्य Curl_cpool_nw_परिवर्तितम्(संरचना cpool *cpool, संरचना Curl_easy *admin);

/* Return TRUE iff the given connection is considered healthy, e.g.
 * usable for more transfers. */
bool Curl_cpool_conn_seems_healthy(संरचना connectdata *conn,
                                   संरचना Curl_easy *दत्त,
                                   ध्रुव संरचना curltime *pnow);

#यदि_अन्तः /* HEADER_CURL_CONNCACHE_H */
