#اگر_غیر_تعریف_شدہ سرخی_CURL_THRDQUEUE_H
#تعریف_کرو سرخی_CURL_THRDQUEUE_H
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
#شامل "curl_setup.h"
#شامل "curlx/timediff.h"

ساخت curl_thrdq;

#اگر_تعریف_شدہ USE_دھاگے

ساخت Curl_easy;

قسمتعریف شمار {
  CURL_THRDQ_EV_شے_مکمل /* an item has been processed and is ready */
} Curl_thrdq_event;

/* Notification callback when "events" happen in the queue. May be called from
 * any thread, queue is not locked. */
قسمتعریف خالی Curl_thrdq_ev_cb(ثابت ساخت curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              خالی *user_ڈیٹا);

/* Process a queued item. May be called from any thread. Queue is not
 * locked. */
قسمتعریف خالی Curl_thrdq_شے_process_cb(خالی *شے);

/* Free an item. May be called from any thread at any time for an
 * item that is in the queue (either before or after processing). */
قسمتعریف خالی Curl_thrdq_شے_free_cb(خالی *شے);

/* Create a new queue processing "items" by a thread pool.
 */
CURLcode Curl_thrdq_create(ساخت curl_thrdq **ptqueue,
                           ثابت حرف *name,
                           uint32_t min_دھاگے,
                           uint32_t max_دھاگے,
                           uint32_t idle_وقت_ms,
                           Curl_thrdq_شے_free_cb *fn_free,
                           Curl_thrdq_شے_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event, /* optional */
                           خالی *user_ڈیٹا);

/* Destroy the queue, free all queued items unprocessed and destroy
 * the thread pool used.
 * @param join TRUE when thread pool shall be joined. FALSE for
 *             detaching any running threads.
 */
خالی Curl_thrdq_destroy(ساخت curl_thrdq *tqueue, bool join);

/* Send "item" onto the queue. The caller needs to clear any reference
 * to "item" on success, e.g. the queue takes ownership.
 * `description` is an optional string describing the item for tracing
 * purposes. It needs to have the same lifetime as `item`.
 *
 * With`timeout_ms` != 0, items that get stuck that long in the send
 * queue are removed and added to the receive queue right away.
 */
CURLcode Curl_thrdq_send(ساخت curl_thrdq *tqueue, خالی *شے,
                         ثابت حرف *description, timediff_t timeout_ms);

/* Receive the oldest, processed item from the queue again, if there is one.
 * The caller takes ownership of the item received, e.g. the queue
 * relinquishes all references to item.
 * Returns CURLE_AGAIN when there is no processed item, setting `pitem`
 * to NULL. When nothing has been processed while items await sending,
 * the pool is signalled to make sure a worker thread exists: an
 * earlier thread start may have failed.
 */
CURLcode Curl_thrdq_recv(ساخت curl_thrdq *tqueue, خالی **pitem);

/* Check that items awaiting processing have a worker thread to run
 * them, signalling the pool to start one when needed -- an earlier
 * thread start may have failed. Returns TRUE when everything is ok:
 * no items are waiting or the pool took the signal. FALSE when a
 * thread start just failed again; callers may want to check again
 * soon rather than wait indefinitely.
 */
bool Curl_thrdq_check_started(ساخت curl_thrdq *tqueue);

/* Return TRUE if the passed "item" matches. */
قسمتعریف bool Curl_thrdq_شے_مطابقت_cb(خالی *شے, خالی *مطابقت_ڈیٹا);

/* Clear all scheduled/processed items that match from the queue. This
 * will *not* be able to clear items that are being processed.
 */
خالی Curl_thrdq_clear(ساخت curl_thrdq *tqueue,
                      Curl_thrdq_شے_مطابقت_cb *fn_مطابقت,
                      خالی *مطابقت_ڈیٹا);

CURLcode Curl_thrdq_await_مکمل(ساخت curl_thrdq *tqueue,
                               uint32_t timeout_ms);

CURLcode Curl_thrdq_سیٹ_props(ساخت curl_thrdq *tqueue,
                              uint32_t min_دھاگے,
                              uint32_t max_دھاگے,
                              uint32_t idle_وقت_ms);

#اگر_تعریف_شدہ CURLVERBOSE
خالی Curl_thrdq_trace(ساخت curl_thrdq *tqueue,
                      ساخت Curl_easy *ڈیٹا);
#تعریف_کرو CURL_THRDQ_TRACE(t, d) \
  کرو { \
    اگر(Curl_trc_ft_is_verbose(d, &Curl_trc_feat_دھاگے)) \
      Curl_thrdq_trace(t, d); \
  } جب(0)
#ورنہ
#تعریف_کرو CURL_THRDQ_TRACE(t, d)  Curl_nop_stmt
#اگر_ختم

#اگر_ختم /* USE_THREADS */

#اگر_ختم /* HEADER_CURL_THRDQUEUE_H */
