#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_THRDQUEUE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_THRDQUEUE_H
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

ರಚನೆ curl_thrdq;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಎಳೆಗಳು

ರಚನೆ Curl_easy;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CURL_THRDQ_EV_ಅಂಶ_ಮುಗಿದ /* an item has been processed and is ready */
} Curl_thrdq_event;

/* Notification callback when "events" happen in the queue. May be called from
 * any thread, queue is not locked. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ Curl_thrdq_ev_cb(ಸ್ಥಿರಾಂಕ ರಚನೆ curl_thrdq *tqueue,
                              Curl_thrdq_event ev,
                              ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);

/* Process a queued item. May be called from any thread. Queue is not
 * locked. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ Curl_thrdq_ಅಂಶ_process_cb(ಶೂನ್ಯ *ಅಂಶ);

/* Free an item. May be called from any thread at any time for an
 * item that is in the queue (either before or after processing). */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ Curl_thrdq_ಅಂಶ_free_cb(ಶೂನ್ಯ *ಅಂಶ);

/* Create a new queue processing "items" by a thread pool.
 */
CURLcode Curl_thrdq_create(ರಚನೆ curl_thrdq **ptqueue,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                           uint32_t min_ಎಳೆಗಳು,
                           uint32_t max_ಎಳೆಗಳು,
                           uint32_t idle_ಸಮಯ_ms,
                           Curl_thrdq_ಅಂಶ_free_cb *fn_free,
                           Curl_thrdq_ಅಂಶ_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event, /* optional */
                           ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);

/* Destroy the queue, free all queued items unprocessed and destroy
 * the thread pool used.
 * @param join TRUE when thread pool shall be joined. FALSE for
 *             detaching any running threads.
 */
ಶೂನ್ಯ Curl_thrdq_destroy(ರಚನೆ curl_thrdq *tqueue, bool join);

/* Send "item" onto the queue. The caller needs to clear any reference
 * to "item" on success, e.g. the queue takes ownership.
 * `description` is an optional string describing the item for tracing
 * purposes. It needs to have the same lifetime as `item`.
 *
 * With`timeout_ms` != 0, items that get stuck that long in the send
 * queue are removed and added to the receive queue right away.
 */
CURLcode Curl_thrdq_send(ರಚನೆ curl_thrdq *tqueue, ಶೂನ್ಯ *ಅಂಶ,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *description, timediff_t timeout_ms);

/* Receive the oldest, processed item from the queue again, if there is one.
 * The caller takes ownership of the item received, e.g. the queue
 * relinquishes all references to item.
 * Returns CURLE_AGAIN when there is no processed item, setting `pitem`
 * to NULL. When nothing has been processed while items await sending,
 * the pool is signalled to make sure a worker thread exists: an
 * earlier thread start may have failed.
 */
CURLcode Curl_thrdq_recv(ರಚನೆ curl_thrdq *tqueue, ಶೂನ್ಯ **pitem);

/* Check that items awaiting processing have a worker thread to run
 * them, signalling the pool to start one when needed -- an earlier
 * thread start may have failed. Returns TRUE when everything is ok:
 * no items are waiting or the pool took the signal. FALSE when a
 * thread start just failed again; callers may want to check again
 * soon rather than wait indefinitely.
 */
bool Curl_thrdq_check_started(ರಚನೆ curl_thrdq *tqueue);

/* Return TRUE if the passed "item" matches. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ bool Curl_thrdq_ಅಂಶ_ಹೊಂದಿಕೆ_cb(ಶೂನ್ಯ *ಅಂಶ, ಶೂನ್ಯ *ಹೊಂದಿಕೆ_ದತ್ತಾಂಶ);

/* Clear all scheduled/processed items that match from the queue. This
 * will *not* be able to clear items that are being processed.
 */
ಶೂನ್ಯ Curl_thrdq_clear(ರಚನೆ curl_thrdq *tqueue,
                      Curl_thrdq_ಅಂಶ_ಹೊಂದಿಕೆ_cb *fn_ಹೊಂದಿಕೆ,
                      ಶೂನ್ಯ *ಹೊಂದಿಕೆ_ದತ್ತಾಂಶ);

CURLcode Curl_thrdq_await_ಮುಗಿದ(ರಚನೆ curl_thrdq *tqueue,
                               uint32_t timeout_ms);

CURLcode Curl_thrdq_ಗಣ_props(ರಚನೆ curl_thrdq *tqueue,
                              uint32_t min_ಎಳೆಗಳು,
                              uint32_t max_ಎಳೆಗಳು,
                              uint32_t idle_ಸಮಯ_ms);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಶೂನ್ಯ Curl_thrdq_trace(ರಚನೆ curl_thrdq *tqueue,
                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_THRDQ_TRACE(t, d) \
  ಮಾಡು { \
    ಆದರೆ(Curl_trc_ft_is_verbose(d, &Curl_trc_feat_ಎಳೆಗಳು)) \
      Curl_thrdq_trace(t, d); \
  } ಆಗ(0)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_THRDQ_TRACE(t, d)  Curl_nop_stmt
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* USE_THREADS */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_THRDQUEUE_H */
