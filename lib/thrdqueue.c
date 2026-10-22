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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಎಳೆಗಳು

#ಅನ್ತರ್ಭಾವ "llist.h"
#ಅನ್ತರ್ಭಾವ "curl_threads.h"
#ಅನ್ತರ್ಭಾವ "thrdpool.h"
#ಅನ್ತರ್ಭಾವ "thrdqueue.h"
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಆದರೆ_ಅಂತ್ಯ


ರಚನೆ curl_thrdq {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
  curl_mutex_t ಬೀಗ;
  curl_cond_t await;
  ರಚನೆ Curl_llist sendq;
  ರಚನೆ Curl_llist recvq;
  ರಚನೆ curl_thrdpool *tpool;
  Curl_thrdq_ಅಂಶ_free_cb *fn_free;
  Curl_thrdq_ಅಂಶ_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  ಶೂನ್ಯ *fn_user_ದತ್ತಾಂಶ;
  ಬಿಟ್(aborted);
};

ರಚನೆ thrdq_ಅಂಶ {
  ರಚನೆ Curl_llist_ಗಂಟು ಗಂಟು;
  Curl_thrdq_ಅಂಶ_free_cb *fn_free;
  Curl_thrdq_ಅಂಶ_process_cb *fn_process;
  ಶೂನ್ಯ *ಅಂಶ;
  ರಚನೆ curltime ಆರಂಭ;
  timediff_t timeout_ms;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *description;
};

ಸ್ಥಿರ ರಚನೆ thrdq_ಅಂಶ *thrdq_ಅಂಶ_create(ರಚನೆ curl_thrdq *tqueue,
                                            ಶೂನ್ಯ *ಅಂಶ,
                                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *description,
                                            timediff_t timeout_ms)
{
  ರಚನೆ thrdq_ಅಂಶ *qitem;

  qitem = curlx_calloc(1, ಗಾತ್ರ(*qitem));
  ಆದರೆ(!qitem)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  qitem->ಅಂಶ = ಅಂಶ;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  ಆದರೆ(timeout_ms) {
    qitem->ಆರಂಭ = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  ಹಿಂತಿರುಗಿ qitem;
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdq_ಅಂಶ_destroy(ರಚನೆ thrdq_ಅಂಶ *qitem)
{
  ಆದರೆ(qitem->ಅಂಶ)
    qitem->fn_free(qitem->ಅಂಶ);
  curlx_free(qitem);
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdq_ಅಂಶ_list_dtor(ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಶೂನ್ಯ *elem)
{
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;
  thrdq_ಅಂಶ_destroy(elem);
}

ಸ್ಥಿರ ಶೂನ್ಯ *thrdq_tpool_take(ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **pdescription,
                              timediff_t *ptimeout_ms)
{
  ರಚನೆ curl_thrdq *tqueue = user_ದತ್ತಾಂಶ;
  ರಚನೆ thrdq_ಅಂಶ *qitem = ಖಾಲಿ;
  ರಚನೆ Curl_llist_ಗಂಟು *e;
  Curl_thrdq_ev_cb *fn_event = ಖಾಲಿ;
  ಶೂನ್ಯ *fn_user_ದತ್ತಾಂಶ = ಖಾಲಿ;

  Curl_mutex_acquire(&tqueue->ಬೀಗ);
  *pdescription = ಖಾಲಿ;
  *ptimeout_ms = 0;
  ಆದರೆ(!tqueue->aborted) {
    e = Curl_llist_ತಲೆ(&tqueue->sendq);
    ಆದರೆ(e) {
      ರಚನೆ curltime now = curlx_now();
      timediff_t timeout_ms;
      ಆಗ(e) {
        qitem = Curl_ಗಂಟು_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->ಆರಂಭ));
        ಆದರೆ(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->ಗಂಟು);
          fn_event = tqueue->fn_event;
          fn_user_ದತ್ತಾಂಶ = tqueue->fn_user_ದತ್ತಾಂಶ;
          qitem = ಖಾಲಿ;
          e = Curl_llist_ತಲೆ(&tqueue->sendq);
          ಮುಂದುವರಿ;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          ನಿಲ್ಲಿಸು;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->ಬೀಗ);
  /* avoiding deadlocks */
  ಆದರೆ(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ಅಂಶ_ಮುಗಿದ, fn_user_ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ qitem;
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdq_tpool_return(ಶೂನ್ಯ *ಅಂಶ, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  ರಚನೆ curl_thrdq *tqueue = user_ದತ್ತಾಂಶ;
  ರಚನೆ thrdq_ಅಂಶ *qitem = ಅಂಶ;
  Curl_thrdq_ev_cb *fn_event = ಖಾಲಿ;
  ಶೂನ್ಯ *fn_user_ದತ್ತಾಂಶ = ಖಾಲಿ;

  ಆದರೆ(!tqueue) {
    thrdq_ಅಂಶ_destroy(ಅಂಶ);
    ಹಿಂತಿರುಗಿ;
  }

  Curl_mutex_acquire(&tqueue->ಬೀಗ);
  ಆದರೆ(tqueue->aborted) {
    thrdq_ಅಂಶ_destroy(qitem);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    DEBUGASSERT(!Curl_ಗಂಟು_llist(&qitem->ಗಂಟು));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->ಗಂಟು);
    fn_event = tqueue->fn_event;
    fn_user_ದತ್ತಾಂಶ = tqueue->fn_user_ದತ್ತಾಂಶ;
  }
  Curl_mutex_release(&tqueue->ಬೀಗ);
  /* avoiding deadlocks */
  ಆದರೆ(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ಅಂಶ_ಮುಗಿದ, fn_user_ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdq_tpool_process(ಶೂನ್ಯ *ಅಂಶ)
{
  ರಚನೆ thrdq_ಅಂಶ *qitem = ಅಂಶ;
  qitem->fn_process(qitem->ಅಂಶ);
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdq_unlink(ರಚನೆ curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  ಆದರೆ(tqueue->tpool) {
    ಆದರೆ(locked)
      Curl_mutex_release(&tqueue->ಬೀಗ);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = ಖಾಲಿ;
    ಆದರೆ(locked)
      Curl_mutex_acquire(&tqueue->ಬೀಗ);
  }

  Curl_llist_destroy(&tqueue->sendq, ಖಾಲಿ);
  Curl_llist_destroy(&tqueue->recvq, ಖಾಲಿ);
  Curl_cond_destroy(&tqueue->await);
  ಆದರೆ(locked)
    Curl_mutex_release(&tqueue->ಬೀಗ);
  Curl_mutex_destroy(&tqueue->ಬೀಗ);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(ರಚನೆ curl_thrdq **ptqueue,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                           uint32_t min_ಎಳೆಗಳು,
                           uint32_t max_ಎಳೆಗಳು,
                           uint32_t idle_ಸಮಯ_ms,
                           Curl_thrdq_ಅಂಶ_free_cb *fn_free,
                           Curl_thrdq_ಅಂಶ_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  ರಚನೆ curl_thrdq *tqueue;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, ಗಾತ್ರ(*tqueue));
  ಆದರೆ(!tqueue)
    ಹೋಗು ಹೊರಹೋಗು;

  Curl_mutex_init(&tqueue->ಬೀಗ);
  Curl_cond_init(&tqueue->await);
  Curl_llist_init(&tqueue->sendq, thrdq_ಅಂಶ_list_dtor);
  Curl_llist_init(&tqueue->recvq, thrdq_ಅಂಶ_list_dtor);
  tqueue->fn_free = fn_free;
  tqueue->fn_process = fn_process;
  tqueue->fn_event = fn_event;
  tqueue->fn_user_ದತ್ತಾಂಶ = user_ದತ್ತಾಂಶ;

  /* a const string that remains */
  tqueue->name = name;

  ಫಲಿತಾಂಶ = Curl_thrdpool_create(&tqueue->tpool, name,
                                min_ಎಳೆಗಳು, max_ಎಳೆಗಳು, idle_ಸಮಯ_ms,
                                thrdq_tpool_take,
                                thrdq_tpool_process,
                                thrdq_tpool_return,
                                tqueue);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = ಖಾಲಿ;
  }
  *ptqueue = tqueue;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_thrdq_destroy(ರಚನೆ curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->ಬೀಗ);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

ಸ್ಥಿರ uint32_t thrdq_get_ಸಂಕೇತಗಳು(ರಚನೆ curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_ಎಣಿಕೆ(&tqueue->sendq);
  ಹಿಂತಿರುಗಿ (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(ರಚನೆ curl_thrdq *tqueue, ಶೂನ್ಯ *ಅಂಶ,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *description, timediff_t timeout_ms)
{
  ರಚನೆ thrdq_ಅಂಶ *qitem;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  uint32_t ಸಂಕೇತಗಳು = 0;

  Curl_mutex_acquire(&tqueue->ಬೀಗ);
  ಆದರೆ(tqueue->aborted) {
    DEBUGASSERT(0);
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(timeout_ms < 0) {
    ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  qitem = thrdq_ಅಂಶ_create(tqueue, ಅಂಶ, description, timeout_ms);
  ಆದರೆ(!qitem) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->ಗಂಟು);
  ಸಂಕೇತಗಳು = thrdq_get_ಸಂಕೇತಗಳು(tqueue);

ಹೊರಹೋಗು:
  Curl_mutex_release(&tqueue->ಬೀಗ);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  ಆದರೆ(!ಫಲಿತಾಂಶ && ಸಂಕೇತಗಳು)
    (ಶೂನ್ಯ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)ಸಂಕೇತಗಳು);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool Curl_thrdq_check_started(ರಚನೆ curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->ಬೀಗ);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_ಎಣಿಕೆ(&tqueue->sendq);
  Curl_mutex_release(&tqueue->ಬೀಗ);
  ಹಿಂತಿರುಗಿ !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(ರಚನೆ curl_thrdq *tqueue, ಶೂನ್ಯ **pitem)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_AGAIN;
  ರಚನೆ Curl_llist_ಗಂಟು *e;
  uint32_t ಸಂಕೇತಗಳು = 0;

  *pitem = ಖಾಲಿ;
  Curl_mutex_acquire(&tqueue->ಬೀಗ);
  ಆದರೆ(tqueue->aborted) {
    DEBUGASSERT(0);
    ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  e = Curl_llist_ತಲೆ(&tqueue->recvq);
  ಆದರೆ(e) {
    ರಚನೆ thrdq_ಅಂಶ *qitem = Curl_ಗಂಟು_take_elem(e);
    *pitem = qitem->ಅಂಶ;
    qitem->ಅಂಶ = ಖಾಲಿ;
    thrdq_ಅಂಶ_destroy(qitem);
    ಫಲಿತಾಂಶ = CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಸಂಕೇತಗಳು = thrdq_get_ಸಂಕೇತಗಳು(tqueue);

ಹೊರಹೋಗು:
  Curl_mutex_release(&tqueue->ಬೀಗ);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  ಆದರೆ(ಸಂಕೇತಗಳು)
    (ಶೂನ್ಯ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)ಸಂಕೇತಗಳು);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdq_llist_clean_matches(ರಚನೆ Curl_llist *llist,
                                      Curl_thrdq_ಅಂಶ_ಹೊಂದಿಕೆ_cb *fn_ಹೊಂದಿಕೆ,
                                      ಶೂನ್ಯ *ಹೊಂದಿಕೆ_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_llist_ಗಂಟು *e, *n;
  ರಚನೆ thrdq_ಅಂಶ *qitem;

  ಗಾಗಿ(e = Curl_llist_ತಲೆ(llist); e; e = n) {
    n = Curl_ಗಂಟು_ಮುಂದಿನ(e);
    qitem = Curl_ಗಂಟು_elem(e);
    ಆದರೆ(fn_ಹೊಂದಿಕೆ(qitem->ಅಂಶ, ಹೊಂದಿಕೆ_ದತ್ತಾಂಶ))
      Curl_ಗಂಟು_remove(e);
  }
}

ಶೂನ್ಯ Curl_thrdq_clear(ರಚನೆ curl_thrdq *tqueue,
                      Curl_thrdq_ಅಂಶ_ಹೊಂದಿಕೆ_cb *fn_ಹೊಂದಿಕೆ,
                      ಶೂನ್ಯ *ಹೊಂದಿಕೆ_ದತ್ತಾಂಶ)
{
  Curl_mutex_acquire(&tqueue->ಬೀಗ);
  ಆದರೆ(tqueue->aborted) {
    DEBUGASSERT(0);
    ಹೋಗು ಹೊರಹೋಗು;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_ಹೊಂದಿಕೆ, ಹೊಂದಿಕೆ_ದತ್ತಾಂಶ);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_ಹೊಂದಿಕೆ, ಹೊಂದಿಕೆ_ದತ್ತಾಂಶ);
ಹೊರಹೋಗು:
  Curl_mutex_release(&tqueue->ಬೀಗ);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_ಮುಗಿದ(ರಚನೆ curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_ಮುಗಿದ(ರಚನೆ curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  ಹಿಂತಿರುಗಿ Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#ಆದರೆ_ಅಂತ್ಯ

CURLcode Curl_thrdq_ಗಣ_props(ರಚನೆ curl_thrdq *tqueue,
                              uint32_t min_ಎಳೆಗಳು,
                              uint32_t max_ಎಳೆಗಳು,
                              uint32_t idle_ಸಮಯ_ms)
{
  CURLcode ಫಲಿತಾಂಶ;
  uint32_t ಸಂಕೇತಗಳು;

  Curl_mutex_acquire(&tqueue->ಬೀಗ);
  ಸಂಕೇತಗಳು = thrdq_get_ಸಂಕೇತಗಳು(tqueue);
  Curl_mutex_release(&tqueue->ಬೀಗ);

  ಫಲಿತಾಂಶ = Curl_thrdpool_ಗಣ_props(tqueue->tpool, min_ಎಳೆಗಳು,
                                   max_ಎಳೆಗಳು, idle_ಸಮಯ_ms);
  ಆದರೆ(!ಫಲಿತಾಂಶ && ಸಂಕೇತಗಳು)
    ಫಲಿತಾಂಶ = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)ಸಂಕೇತಗಳು);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಶೂನ್ಯ Curl_thrdq_trace(ರಚನೆ curl_thrdq *tqueue,
                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ curl_trc_feat *feat = &Curl_trc_feat_ಎಳೆಗಳು;
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, feat)) {
    ರಚನೆ Curl_llist_ಗಂಟು *e;
    ರಚನೆ thrdq_ಅಂಶ *qitem;

    Curl_thrdpool_trace(tqueue->tpool, ದತ್ತಾಂಶ);
    Curl_mutex_acquire(&tqueue->ಬೀಗ);
    ಆದರೆ(!Curl_llist_ಎಣಿಕೆ(&tqueue->sendq) &&
       !Curl_llist_ಎಣಿಕೆ(&tqueue->recvq)) {
      Curl_trc_feat_infof(ದತ್ತಾಂಶ, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    ಗಾಗಿ(e = Curl_llist_ತಲೆ(&tqueue->sendq); e; e = Curl_ಗಂಟು_ಮುಂದಿನ(e)) {
      qitem = Curl_ಗಂಟು_elem(e);
      Curl_trc_feat_infof(ದತ್ತಾಂಶ, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    ಗಾಗಿ(e = Curl_llist_ತಲೆ(&tqueue->recvq); e; e = Curl_ಗಂಟು_ಮುಂದಿನ(e)) {
      qitem = Curl_ಗಂಟು_elem(e);
      Curl_trc_feat_infof(ದತ್ತಾಂಶ, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->ಬೀಗ);
  }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* USE_THREADS */
