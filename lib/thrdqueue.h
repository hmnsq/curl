#నిర్వచితం_కాకపోతే శీర్షిక_CURL_THRDQUEUE_H
#నిర్వచించు శీర్షిక_CURL_THRDQUEUE_H
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

నిర్మాణం curl_thrdq;

#నిర్వచితమైతే USE_దారాలు

నిర్మాణం Curl_easy;

రకనిర్వచనం గణన {
  CURL_THRDQ_EV_అంశం_పూర్తి /* an item has been processed and is ready */
} Curl_thrdq_event;

/* Notification callback when "events" happen in the queue. May be called from
 * any thread, queue is not locked. */
రకనిర్వచనం శూన్య Curl_thrdq_ev_cb(స్థిరరాశి నిర్మాణం curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              శూన్య *user_దత్తాంశం);

/* Process a queued item. May be called from any thread. Queue is not
 * locked. */
రకనిర్వచనం శూన్య Curl_thrdq_అంశం_process_cb(శూన్య *అంశం);

/* Free an item. May be called from any thread at any time for an
 * item that is in the queue (either before or after processing). */
రకనిర్వచనం శూన్య Curl_thrdq_అంశం_free_cb(శూన్య *అంశం);

/* Create a new queue processing "items" by a thread pool.
 */
CURLcode Curl_thrdq_create(నిర్మాణం curl_thrdq **ptqueue,
                           స్థిరరాశి అక్షరం *name,
                           uint32_t min_దారాలు,
                           uint32_t max_దారాలు,
                           uint32_t idle_సమయం_ms,
                           Curl_thrdq_అంశం_free_cb *fn_free,
                           Curl_thrdq_అంశం_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event, /* optional */
                           శూన్య *user_దత్తాంశం);

/* Destroy the queue, free all queued items unprocessed and destroy
 * the thread pool used.
 * @param join TRUE when thread pool shall be joined. FALSE for
 *             detaching any running threads.
 */
శూన్య Curl_thrdq_destroy(నిర్మాణం curl_thrdq *tqueue, bool join);

/* Send "item" onto the queue. The caller needs to clear any reference
 * to "item" on success, e.g. the queue takes ownership.
 * `description` is an optional string describing the item for tracing
 * purposes. It needs to have the same lifetime as `item`.
 *
 * With`timeout_ms` != 0, items that get stuck that long in the send
 * queue are removed and added to the receive queue right away.
 */
CURLcode Curl_thrdq_send(నిర్మాణం curl_thrdq *tqueue, శూన్య *అంశం,
                         స్థిరరాశి అక్షరం *description, timediff_t timeout_ms);

/* Receive the oldest, processed item from the queue again, if there is one.
 * The caller takes ownership of the item received, e.g. the queue
 * relinquishes all references to item.
 * Returns CURLE_AGAIN when there is no processed item, setting `pitem`
 * to NULL. When nothing has been processed while items await sending,
 * the pool is signalled to make sure a worker thread exists: an
 * earlier thread start may have failed.
 */
CURLcode Curl_thrdq_recv(నిర్మాణం curl_thrdq *tqueue, శూన్య **pitem);

/* Check that items awaiting processing have a worker thread to run
 * them, signalling the pool to start one when needed -- an earlier
 * thread start may have failed. Returns TRUE when everything is ok:
 * no items are waiting or the pool took the signal. FALSE when a
 * thread start just failed again; callers may want to check again
 * soon rather than wait indefinitely.
 */
bool Curl_thrdq_check_started(నిర్మాణం curl_thrdq *tqueue);

/* Return TRUE if the passed "item" matches. */
రకనిర్వచనం bool Curl_thrdq_అంశం_సరిపోలిక_cb(శూన్య *అంశం, శూన్య *సరిపోలిక_దత్తాంశం);

/* Clear all scheduled/processed items that match from the queue. This
 * will *not* be able to clear items that are being processed.
 */
శూన్య Curl_thrdq_clear(నిర్మాణం curl_thrdq *tqueue,
                      Curl_thrdq_అంశం_సరిపోలిక_cb *fn_సరిపోలిక,
                      శూన్య *సరిపోలిక_దత్తాంశం);

CURLcode Curl_thrdq_await_పూర్తి(నిర్మాణం curl_thrdq *tqueue,
                               uint32_t timeout_ms);

CURLcode Curl_thrdq_సమితి_props(నిర్మాణం curl_thrdq *tqueue,
                              uint32_t min_దారాలు,
                              uint32_t max_దారాలు,
                              uint32_t idle_సమయం_ms);

#నిర్వచితమైతే CURLVERBOSE
శూన్య Curl_thrdq_trace(నిర్మాణం curl_thrdq *tqueue,
                      నిర్మాణం Curl_easy *దత్తాంశం);
#నిర్వచించు CURL_THRDQ_TRACE(t, d) \
  చేయి { \
    ఒకవేళ(Curl_trc_ft_is_verbose(d, &Curl_trc_feat_దారాలు)) \
      Curl_thrdq_trace(t, d); \
  } అయితే(0)
#లేకపోతే
#నిర్వచించు CURL_THRDQ_TRACE(t, d)  Curl_nop_stmt
#అయితే_ముగింపు

#అయితే_ముగింపు /* USE_THREADS */

#అయితే_ముగింపు /* HEADER_CURL_THRDQUEUE_H */
