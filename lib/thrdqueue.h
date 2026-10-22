#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_THRDQUEUE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_THRDQUEUE_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timediff.h"

ਬਣਤਰ curl_thrdq;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਧਾਗੇ

ਬਣਤਰ Curl_easy;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURL_THRDQ_EV_ਵਸਤੂ_ਪੂਰਾ /* an item has been processed and is ready */
} Curl_thrdq_event;

/* Notification callback when "events" happen in the queue. May be called from
 * any thread, queue is not locked. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ Curl_thrdq_ev_cb(ਅਚਲ ਬਣਤਰ curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              ਖਾਲੀ *user_ਅੰਕੜੇ);

/* Process a queued item. May be called from any thread. Queue is not
 * locked. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ Curl_thrdq_ਵਸਤੂ_process_cb(ਖਾਲੀ *ਵਸਤੂ);

/* Free an item. May be called from any thread at any time for an
 * item that is in the queue (either before or after processing). */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ Curl_thrdq_ਵਸਤੂ_free_cb(ਖਾਲੀ *ਵਸਤੂ);

/* Create a new queue processing "items" by a thread pool.
 */
CURLcode Curl_thrdq_create(ਬਣਤਰ curl_thrdq **ptqueue,
                           ਅਚਲ ਅੱਖਰ *name,
                           uint32_t min_ਧਾਗੇ,
                           uint32_t max_ਧਾਗੇ,
                           uint32_t idle_ਸਮਾਂ_ms,
                           Curl_thrdq_ਵਸਤੂ_free_cb *fn_free,
                           Curl_thrdq_ਵਸਤੂ_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event, /* optional */
                           ਖਾਲੀ *user_ਅੰਕੜੇ);

/* Destroy the queue, free all queued items unprocessed and destroy
 * the thread pool used.
 * @param join TRUE when thread pool shall be joined. FALSE for
 *             detaching any running threads.
 */
ਖਾਲੀ Curl_thrdq_destroy(ਬਣਤਰ curl_thrdq *tqueue, bool join);

/* Send "item" onto the queue. The caller needs to clear any reference
 * to "item" on success, e.g. the queue takes ownership.
 * `description` is an optional string describing the item for tracing
 * purposes. It needs to have the same lifetime as `item`.
 *
 * With`timeout_ms` != 0, items that get stuck that long in the send
 * queue are removed and added to the receive queue right away.
 */
CURLcode Curl_thrdq_send(ਬਣਤਰ curl_thrdq *tqueue, ਖਾਲੀ *ਵਸਤੂ,
                         ਅਚਲ ਅੱਖਰ *description, timediff_t timeout_ms);

/* Receive the oldest, processed item from the queue again, if there is one.
 * The caller takes ownership of the item received, e.g. the queue
 * relinquishes all references to item.
 * Returns CURLE_AGAIN when there is no processed item, setting `pitem`
 * to NULL. When nothing has been processed while items await sending,
 * the pool is signalled to make sure a worker thread exists: an
 * earlier thread start may have failed.
 */
CURLcode Curl_thrdq_recv(ਬਣਤਰ curl_thrdq *tqueue, ਖਾਲੀ **pitem);

/* Check that items awaiting processing have a worker thread to run
 * them, signalling the pool to start one when needed -- an earlier
 * thread start may have failed. Returns TRUE when everything is ok:
 * no items are waiting or the pool took the signal. FALSE when a
 * thread start just failed again; callers may want to check again
 * soon rather than wait indefinitely.
 */
bool Curl_thrdq_check_started(ਬਣਤਰ curl_thrdq *tqueue);

/* Return TRUE if the passed "item" matches. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ bool Curl_thrdq_ਵਸਤੂ_ਮੇਲ_cb(ਖਾਲੀ *ਵਸਤੂ, ਖਾਲੀ *ਮੇਲ_ਅੰਕੜੇ);

/* Clear all scheduled/processed items that match from the queue. This
 * will *not* be able to clear items that are being processed.
 */
ਖਾਲੀ Curl_thrdq_clear(ਬਣਤਰ curl_thrdq *tqueue,
                      Curl_thrdq_ਵਸਤੂ_ਮੇਲ_cb *fn_ਮੇਲ,
                      ਖਾਲੀ *ਮੇਲ_ਅੰਕੜੇ);

CURLcode Curl_thrdq_await_ਪੂਰਾ(ਬਣਤਰ curl_thrdq *tqueue,
                               uint32_t timeout_ms);

CURLcode Curl_thrdq_ਸੈੱਟ_props(ਬਣਤਰ curl_thrdq *tqueue,
                              uint32_t min_ਧਾਗੇ,
                              uint32_t max_ਧਾਗੇ,
                              uint32_t idle_ਸਮਾਂ_ms);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਖਾਲੀ Curl_thrdq_trace(ਬਣਤਰ curl_thrdq *tqueue,
                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_THRDQ_TRACE(t, d) \
  ਕਰੋ { \
    ਜੇ(Curl_trc_ft_is_verbose(d, &Curl_trc_feat_ਧਾਗੇ)) \
      Curl_thrdq_trace(t, d); \
  } ਜਦੋਂ(0)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_THRDQ_TRACE(t, d)  Curl_nop_stmt
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* USE_THREADS */

#ਜੇ_ਅੰਤ /* HEADER_CURL_THRDQUEUE_H */
