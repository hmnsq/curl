#اگر_تعریف_نشده سرآیند_CURL_CONNCACHE_H
#تعریف_کن سرآیند_CURL_CONNCACHE_H
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
#ضمن "uint-bset.h"
#ضمن "uint-table.h"
#ضمن "curlx/timeval.h"

بنية connectdata;
بنية Curl_easy;
بنية curl_pollfds;
بنية Curl_waitfds;
بنية Curl_multi;
بنية Curl_share;

/**
 * Close and destroy the connection.
 * If the connection is in a cpool, remove it.
 * If a `cshutdn` is available (e.g. data has a multi handle),
 * pass the connection to that for controlled shutdown.
 * Otherwise terminate it right away.
 * Takes ownership of `conn`.
 * `data` should not be attached to a connection.
 */
فراغ Curl_conn_close(بنية Curl_easy *داده,
                     بنية connectdata *conn,
                     bool aborted);

بنية cpool {
  بنية uint32_tbl conns; /* connections added to this pool */
  بنية uint32_bset idles; /* pool_ids of conns being idle */
  بنية Curl_hash dest2bundle; /* conn destination sets */
  curl_off_t بعدی_connection_id;
  curl_off_t بعدی_easy_id;
  بنية curltime آخرین_cleanup;
  بنية Curl_share *share; /* != NULL if pool belongs to share */
  بیت(locked);
  بیت(initialized);
  بیت(درون_shutdown);
};

/* Get connection pool instance for data or NULL if none exists */
بنية cpool *Curl_cpool_get_instance(بنية Curl_easy *داده);

/* Init the pool, pass multi only if pool is owned by it.
 * Cannot fail.
 */
فراغ Curl_cpool_init(بنية cpool *cpool,
                     بنية Curl_share *share,
                     size_t اندازه);

/* Destroy all connections and free all members */
فراغ Curl_cpool_destroy(بنية cpool *cpool,
                        بنية Curl_easy *admin);

/* Init the transfer to be used within its connection pool.
 * Assigns `data->id`. */
فراغ Curl_cpool_xfer_init(بنية Curl_easy *داده);

/* Get the connection last used by data,
 * if there was one and it still exists. */
بنية connectdata *Curl_cpool_get_آخرین_conn(بنية Curl_easy *داده);

/* Add the connection to the pool. */
CURLcode Curl_cpool_جمع_کن(بنية Curl_easy *داده,
                        بنية connectdata *conn,
                        uint32_t max_مجموع,
                        uint32_t max_host,
                        ثابت بنية curltime *pnow) WARN_استفادهنشده_نتیجه;

/* Connection was used at the given time. */
فراغ Curl_cpool_conn_was_استفاده_شده(بنية Curl_easy *داده,
                              بنية connectdata *conn,
                              ثابت بنية curltime *pnow);

/* Return connection age in milliseconds since its creation. */
timediff_t Curl_cpool_conn_age_ms(بنية Curl_easy *داده,
                                  بنية connectdata *conn,
                                  ثابت بنية curltime *pnow);

تعريفنوع تعداد {
  CPOOL_تطابق_یافت, /* The passed `conn` matches, stop looking further */
  CPOOL_تطابق_CONT,  /* No match, continue looking */
  CPOOL_تطابق_CLOSE, /* No match, close `conn`, continue looking */
  CPOOL_تطابق_TOO_OLD /* No match, `conn` is too old, close when idle */
} cpool_تطابق_نتیجه;

/* Return of conn is suitable. If so, stops iteration. */
تعريفنوع cpool_تطابق_نتیجه
 Curl_cpool_conn_تطابق_cb(بنية connectdata *conn, فراغ *userdata);

/* Act on the result of the find, may override it. */
تعريفنوع bool Curl_cpool_انجامشده_تطابق_cb(فراغ *userdata);

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
bool Curl_cpool_پیدا_کن(بنية Curl_easy *داده,
                     ثابت حرف *destination,
                     bool prune_dead,
                     ثابت بنية curltime *pnow,
                     Curl_cpool_conn_تطابق_cb *conn_cb,
                     Curl_cpool_انجامشده_تطابق_cb *انجامشده_cb,
                     فراغ *userdata);

/**
 * Perform upkeep actions on connections in the transfer's pool.
 */
CURLcode Curl_cpool_upkeep(بنية Curl_easy *داده);

تعريفنوع تعداد {
  CPOOL_DO_KEEP, /* Keep the connection, still in use */
  CPOOL_DO_IDLE, /* Connection is idle, may get closed now */
  CPOOL_DO_CLOSE, /* Close the connection (clean) */
  CPOOL_DO_TERMINATE /* Terminate the connection (unclean) */
} cpool_do_نتیجه;

تعريفنوع cpool_do_نتیجه Curl_cpool_return_cb(بنية Curl_easy *داده,
                                             بنية connectdata *conn,
                                             فراغ *cbdata,
                                             ثابت بنية curltime *pnow);

/**
 * Invoked the callback for the given data + connection under the
 * connection pool's lock.
 * The callback is always invoked, even if the transfer has no connection
 * pool associated.
 */
فراغ Curl_cpool_return(بنية Curl_easy *داده,
                       بنية connectdata *conn,
                       Curl_cpool_return_cb *cb, فراغ *cbdata,
                       ثابت بنية curltime *pnow);

/* Close all unused connections, prevent reuse of existing ones. */
فراغ Curl_cpool_nw_تغییریافته(بنية cpool *cpool, بنية Curl_easy *admin);

/* Return TRUE iff the given connection is considered healthy, e.g.
 * usable for more transfers. */
bool Curl_cpool_conn_seems_healthy(بنية connectdata *conn,
                                   بنية Curl_easy *داده,
                                   ثابت بنية curltime *pnow);

#پایان_اگر /* HEADER_CURL_CONNCACHE_H */
