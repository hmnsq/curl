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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "curlx/timediff.h"

ಸಂರಚನಾ curl_thrdq;

#ifdef USE_THREADS

ಸಂರಚನಾ Curl_easy;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  CURL_THRDQ_EV_ITEM_DONE /* an item has been processed and is ready */
} Curl_thrdq_event;

/* Notification callback when "events" happen in the queue. May be called from
 * any thread, queue is not locked. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ Curl_thrdq_ev_cb(ಧ್ರುವ ಸಂರಚನಾ curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              ಶೂನ್ಯ *user_data);

/* Process a queued item. May be called from any thread. Queue is not
 * locked. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ Curl_thrdq_item_process_cb(ಶೂನ್ಯ *item);

/* Free an item. May be called from any thread at any time for an
 * item that is in the queue (either before or after processing). */
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ Curl_thrdq_item_free_cb(ಶೂನ್ಯ *item);

/* Create a new queue processing "items" by a thread pool.
 */
CURLcode Curl_thrdq_create(ಸಂರಚನಾ curl_thrdq **ptqueue,
                           ಧ್ರುವ ವರ್ಣ *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event, /* optional */
                           ಶೂನ್ಯ *user_data);

/* Destroy the queue, free all queued items unprocessed and destroy
 * the thread pool used.
 * @param join TRUE when thread pool shall be joined. FALSE for
 *             detaching any running threads.
 */
ಶೂನ್ಯ Curl_thrdq_destroy(ಸಂರಚನಾ curl_thrdq *tqueue, bool join);

/* Send "item" onto the queue. The caller needs to clear any reference
 * to "item" on success, e.g. the queue takes ownership.
 * `description` is an optional string describing the item for tracing
 * purposes. It needs to have the same lifetime as `item`.
 *
 * With`timeout_ms` != 0, items that get stuck that long in the send
 * queue are removed and added to the receive queue right away.
 */
CURLcode Curl_thrdq_send(ಸಂರಚನಾ curl_thrdq *tqueue, ಶೂನ್ಯ *item,
                         ಧ್ರುವ ವರ್ಣ *description, timediff_t timeout_ms);

/* Receive the oldest, processed item from the queue again, if there is one.
 * The caller takes ownership of the item received, e.g. the queue
 * relinquishes all references to item.
 * Returns CURLE_AGAIN when there is no processed item, setting `pitem`
 * to NULL. When nothing has been processed while items await sending,
 * the pool is signalled to make sure a worker thread exists: an
 * earlier thread start may have failed.
 */
CURLcode Curl_thrdq_recv(ಸಂರಚನಾ curl_thrdq *tqueue, ಶೂನ್ಯ **pitem);

/* Check that items awaiting processing have a worker thread to run
 * them, signalling the pool to start one when needed -- an earlier
 * thread start may have failed. Returns TRUE when everything is ok:
 * no items are waiting or the pool took the signal. FALSE when a
 * thread start just failed again; callers may want to check again
 * soon rather than wait indefinitely.
 */
bool Curl_thrdq_check_started(ಸಂರಚನಾ curl_thrdq *tqueue);

/* Return TRUE if the passed "item" matches. */
ಪ್ರಕಾರನಿರ್ದೇಶ bool Curl_thrdq_item_match_cb(ಶೂನ್ಯ *item, ಶೂನ್ಯ *match_data);

/* Clear all scheduled/processed items that match from the queue. This
 * will *not* be able to clear items that are being processed.
 */
ಶೂನ್ಯ Curl_thrdq_clear(ಸಂರಚನಾ curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      ಶೂನ್ಯ *match_data);

CURLcode Curl_thrdq_await_done(ಸಂರಚನಾ curl_thrdq *tqueue,
                               uint32_t timeout_ms);

CURLcode Curl_thrdq_set_props(ಸಂರಚನಾ curl_thrdq *tqueue,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms);

#ifdef CURLVERBOSE
ಶೂನ್ಯ Curl_thrdq_trace(ಸಂರಚನಾ curl_thrdq *tqueue,
                      ಸಂರಚನಾ Curl_easy *data);
#define CURL_THRDQ_TRACE(t, d) \
  ಕುರು { \
    ಯದಿ(Curl_trc_ft_is_verbose(d, &Curl_trc_feat_threads)) \
      Curl_thrdq_trace(t, d); \
  } ಯಾವತ್(0)
#ಅನ್ಯಥಾ
#define CURL_THRDQ_TRACE(t, d)  Curl_nop_stmt
#endif

#endif /* USE_THREADS */

#endif /* HEADER_CURL_THRDQUEUE_H */
