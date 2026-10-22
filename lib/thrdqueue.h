#ifndef HEADER_CURL_THRDQUEUE_H
#define HEADER_CURL_THRDQUEUE_H
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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "curlx/timediff.h"

কাঠামো curl_thrdq;

#ifdef USE_THREADS

কাঠামো Curl_easy;

প্রকারলেখ গণনা {
  CURL_THRDQ_EV_ITEM_DONE /* an item has been processed and is ready */
} Curl_thrdq_event;

/* Notification callback when "events" happen in the queue. May be called from
 * any thread, queue is not locked. */
প্রকারলেখ শূন্য Curl_thrdq_ev_cb(ধ্রুবক কাঠামো curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              শূন্য *user_তথ্য);

/* Process a queued item. May be called from any thread. Queue is not
 * locked. */
প্রকারলেখ শূন্য Curl_thrdq_item_process_cb(শূন্য *item);

/* Free an item. May be called from any thread at any time for an
 * item that is in the queue (either before or after processing). */
প্রকারলেখ শূন্য Curl_thrdq_item_free_cb(শূন্য *item);

/* Create a new queue processing "items" by a thread pool.
 */
CURLcode Curl_thrdq_create(কাঠামো curl_thrdq **ptqueue,
                           ধ্রুবক অক্ষর *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event, /* optional */
                           শূন্য *user_তথ্য);

/* Destroy the queue, free all queued items unprocessed and destroy
 * the thread pool used.
 * @param join TRUE when thread pool shall be joined. FALSE for
 *             detaching any running threads.
 */
শূন্য Curl_thrdq_destroy(কাঠামো curl_thrdq *tqueue, bool join);

/* Send "item" onto the queue. The caller needs to clear any reference
 * to "item" on success, e.g. the queue takes ownership.
 * `description` is an optional string describing the item for tracing
 * purposes. It needs to have the same lifetime as `item`.
 *
 * With`timeout_ms` != 0, items that get stuck that long in the send
 * queue are removed and added to the receive queue right away.
 */
CURLcode Curl_thrdq_send(কাঠামো curl_thrdq *tqueue, শূন্য *item,
                         ধ্রুবক অক্ষর *description, timediff_t timeout_ms);

/* Receive the oldest, processed item from the queue again, if there is one.
 * The caller takes ownership of the item received, e.g. the queue
 * relinquishes all references to item.
 * Returns CURLE_AGAIN when there is no processed item, setting `pitem`
 * to NULL. When nothing has been processed while items await sending,
 * the pool is signalled to make sure a worker thread exists: an
 * earlier thread start may have failed.
 */
CURLcode Curl_thrdq_recv(কাঠামো curl_thrdq *tqueue, শূন্য **pitem);

/* Check that items awaiting processing have a worker thread to run
 * them, signalling the pool to start one when needed -- an earlier
 * thread start may have failed. Returns TRUE when everything is ok:
 * no items are waiting or the pool took the signal. FALSE when a
 * thread start just failed again; callers may want to check again
 * soon rather than wait indefinitely.
 */
bool Curl_thrdq_check_started(কাঠামো curl_thrdq *tqueue);

/* Return TRUE if the passed "item" matches. */
প্রকারলেখ bool Curl_thrdq_item_match_cb(শূন্য *item, শূন্য *match_তথ্য);

/* Clear all scheduled/processed items that match from the queue. This
 * will *not* be able to clear items that are being processed.
 */
শূন্য Curl_thrdq_clear(কাঠামো curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      শূন্য *match_তথ্য);

CURLcode Curl_thrdq_await_done(কাঠামো curl_thrdq *tqueue,
                               uint32_t timeout_ms);

CURLcode Curl_thrdq_set_props(কাঠামো curl_thrdq *tqueue,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms);

#ifdef CURLVERBOSE
শূন্য Curl_thrdq_trace(কাঠামো curl_thrdq *tqueue,
                      কাঠামো Curl_easy *তথ্য);
#define CURL_THRDQ_TRACE(t, d) \
  করো { \
    যদি(Curl_trc_ft_is_verbose(d, &Curl_trc_feat_threads)) \
      Curl_thrdq_trace(t, d); \
  } যখন(0)
#অন্যথা
#define CURL_THRDQ_TRACE(t, d)  Curl_nop_stmt
#endif

#endif /* USE_THREADS */

#endif /* HEADER_CURL_THRDQUEUE_H */
