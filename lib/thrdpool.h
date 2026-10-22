#నిర్వచితం_కాకపోతే శీర్షిక_CURL_THRDPOOL_H
#నిర్వచించు శీర్షిక_CURL_THRDPOOL_H
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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "curlx/timediff.h"

#నిర్వచితమైతే USE_దారాలు

నిర్మాణం curl_thrdpool;
నిర్మాణం Curl_easy;

/* Invoked under thread pool lock to get an "item" to work on. Must
 * return NULL if there is nothing to do.
 * Caller might return a descriptive string about the "item", where
 * available. The string needs to have the same lifetime as the
 * item itself. */
రకనిర్వచనం శూన్య *Curl_thrdpool_take_అంశం_cb(శూన్య *user_దత్తాంశం,
                                         స్థిరరాశి అక్షరం **pdescription,
                                         timediff_t *ptimeout_ms);

/* Invoked outside thread pool lock to process the item taken. */
రకనిర్వచనం శూన్య Curl_thrdpool_process_అంశం_cb(శూన్య *అంశం);

/* Invoked under thread pool lock to return a processed item back
 * to the producer.
 * If the thread pool has been destroyed, `user_data` will be NULL
 * and the callback is responsible to release all `item` resources. */
రకనిర్వచనం శూన్య Curl_thrdpool_return_అంశం_cb(శూన్య *అంశం, శూన్య *user_దత్తాంశం);

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
CURLcode Curl_thrdpool_create(నిర్మాణం curl_thrdpool **ptpool,
                              స్థిరరాశి అక్షరం *name,
                              uint32_t min_దారాలు,
                              uint32_t max_దారాలు,
                              uint32_t idle_సమయం_ms,
                              Curl_thrdpool_take_అంశం_cb *fn_take,
                              Curl_thrdpool_process_అంశం_cb *fn_process,
                              Curl_thrdpool_return_అంశం_cb *fn_return,
                              శూన్య *user_దత్తాంశం);

/* Destroy the thread pool, release its resources.
 * With `join` being TRUE, the call will wait for all threads to finish
 * processing before returning. On FALSE, it will detach all threads
 * running. Ongoing item processing will continue to run and
 * `fn_return` will be invoked with NULL user_data before the thread exits.
 */
శూన్య Curl_thrdpool_destroy(నిర్మాణం curl_thrdpool *tpool, bool join);

/* Signal the pool to wake up `nthreads` idle worker threads, possible
 * creating new threads up to the max limit. The number should reflect
 * the items that can actually be taken for processing right away, e.g.
 * the producers "queue" length of outstanding items.
 */
CURLcode Curl_thrdpool_signal(నిర్మాణం curl_thrdpool *tpool, uint32_t nthreads);

CURLcode Curl_thrdpool_await_idle(నిర్మాణం curl_thrdpool *tpool,
                                  uint32_t timeout_ms);

/* Change the properties of a threadpool. */
CURLcode Curl_thrdpool_సమితి_props(నిర్మాణం curl_thrdpool *tpool,
                                 uint32_t min_దారాలు,
                                 uint32_t max_దారాలు,
                                 uint32_t idle_సమయం_ms);

#నిర్వచితమైతే CURLVERBOSE
శూన్య Curl_thrdpool_trace(నిర్మాణం curl_thrdpool *tpool,
                         నిర్మాణం Curl_easy *దత్తాంశం);
#అయితే_ముగింపు

#అయితే_ముగింపు /* USE_THREADS */

#అయితే_ముగింపు /* HEADER_CURL_THRDPOOL_H */
