#ifndef HEADER_CURL_THRDPOOL_H
#define HEADER_CURL_THRDPOOL_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
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
#ضمن "curl_setup.h"
#ضمن "curlx/timediff.h"

#ifdef USE_THREADS

بنية curl_thrdpool;
بنية Curl_easy;

/* Invoked under thread pool lock to get an "item" to work on. Must
 * return NULL if there is nothing to do.
 * Caller might return a descriptive string about the "item", where
 * available. The string needs to have the same lifetime as the
 * item itself. */
تعريفنوع فراغ *Curl_thrdpool_take_item_cb(فراغ *user_data,
                                         ثابت حرف **pdescription,
                                         timediff_t *ptimeout_ms);

/* Invoked outside thread pool lock to process the item taken. */
تعريفنوع فراغ Curl_thrdpool_process_item_cb(فراغ *item);

/* Invoked under thread pool lock to return a processed item back
 * to the producer.
 * If the thread pool has been destroyed, `user_data` will be NULL
 * and the callback is responsible to release all `item` resources. */
تعريفنوع فراغ Curl_thrdpool_return_item_cb(فراغ *item, فراغ *user_data);

/* Create a new thread pool.
 * @param name         name of pool for tracing purposes
 * @param min_threads  minimum number of threads to have always running
 * @param max_threads  maximum number of threads running, ever.
 * @param idle_time_ms maximum time a thread should wait for tasks to
 *                     process before shutting down (unless the pool is
 *                     already at minimum thread count), use 0 for
 *                     infinite wait.
 * @param fn_take      take the next item to process
 * @param fn_process   process the item taken
 * @param fn_return    return the processed item
 * @param user_data    parameter passed to take/return callbacks
 */
CURLcode Curl_thrdpool_create(بنية curl_thrdpool **ptpool,
                              ثابت حرف *name,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms,
                              Curl_thrdpool_take_item_cb *fn_take,
                              Curl_thrdpool_process_item_cb *fn_process,
                              Curl_thrdpool_return_item_cb *fn_return,
                              فراغ *user_data);

/* Destroy the thread pool, release its resources.
 * With `join` being TRUE, the call will wait for all threads to finish
 * processing before returning. On FALSE, it will detach all threads
 * running. Ongoing item processing will continue to run and
 * `fn_return` will be invoked with NULL user_data before the thread exits.
 */
فراغ Curl_thrdpool_destroy(بنية curl_thrdpool *tpool, bool join);

/* Signal the pool to wake up `nthreads` idle worker threads, possible
 * creating new threads up to the max limit. The number should reflect
 * the items that can actually be taken for processing right away, e.g.
 * the producers "queue" length of outstanding items.
 */
CURLcode Curl_thrdpool_signal(بنية curl_thrdpool *tpool, uint32_t nthreads);

CURLcode Curl_thrdpool_await_idle(بنية curl_thrdpool *tpool,
                                  uint32_t timeout_ms);

/* Change the properties of a threadpool. */
CURLcode Curl_thrdpool_set_props(بنية curl_thrdpool *tpool,
                                 uint32_t min_threads,
                                 uint32_t max_threads,
                                 uint32_t idle_time_ms);

#ifdef CURLVERBOSE
فراغ Curl_thrdpool_trace(بنية curl_thrdpool *tpool,
                         بنية Curl_easy *data);
#endif

#endif /* USE_THREADS */

#endif /* HEADER_CURL_THRDPOOL_H */
