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

#ifdef USE_THREADS

#ಅನ್ತರ್ಭಾವ "llist.h"
#ಅನ್ತರ್ಭಾವ "curl_threads.h"
#ಅನ್ತರ್ಭಾವ "thrdpool.h"
#ಅನ್ತರ್ಭಾವ "thrdqueue.h"
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"
#ifdef CURLVERBOSE
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#endif


ಸಂರಚನಾ curl_thrdq {
  ಧ್ರುವ ವರ್ಣ *name;
  curl_mutex_t lock;
  curl_cond_t await;
  ಸಂರಚನಾ Curl_llist sendq;
  ಸಂರಚನಾ Curl_llist recvq;
  ಸಂರಚನಾ curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  ಶೂನ್ಯ *fn_user_data;
  BIT(aborted);
};

ಸಂರಚನಾ thrdq_item {
  ಸಂರಚನಾ Curl_llist_ಗಂಟು ಗಂಟು;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  ಶೂನ್ಯ *item;
  ಸಂರಚನಾ curltime start;
  timediff_t timeout_ms;
  ಧ್ರುವ ವರ್ಣ *description;
};

ಸ್ಥಿರ ಸಂರಚನಾ thrdq_item *thrdq_item_create(ಸಂರಚನಾ curl_thrdq *tqueue,
                                            ಶೂನ್ಯ *item,
                                            ಧ್ರುವ ವರ್ಣ *description,
                                            timediff_t timeout_ms)
{
  ಸಂರಚನಾ thrdq_item *qitem;

  qitem = curlx_calloc(1, ಪರಿಮಾಣ(*qitem));
  ಯದಿ(!qitem)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  ಯದಿ(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  ಪ್ರತ್ಯಾಗಮ qitem;
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdq_item_destroy(ಸಂರಚನಾ thrdq_item *qitem)
{
  ಯದಿ(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdq_item_list_dtor(ಶೂನ್ಯ *user_data, ಶೂನ್ಯ *elem)
{
  (ಶೂನ್ಯ)user_data;
  thrdq_item_destroy(elem);
}

ಸ್ಥಿರ ಶೂನ್ಯ *thrdq_tpool_take(ಶೂನ್ಯ *user_data, ಧ್ರುವ ವರ್ಣ **pdescription,
                              timediff_t *ptimeout_ms)
{
  ಸಂರಚನಾ curl_thrdq *tqueue = user_data;
  ಸಂರಚನಾ thrdq_item *qitem = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
  Curl_thrdq_ev_cb *fn_event = ರಿಕ್ತ;
  ಶೂನ್ಯ *fn_user_data = ರಿಕ್ತ;

  Curl_mutex_acquire(&tqueue->lock);
  *pdescription = ರಿಕ್ತ;
  *ptimeout_ms = 0;
  ಯದಿ(!tqueue->aborted) {
    e = Curl_llist_head(&tqueue->sendq);
    ಯದಿ(e) {
      ಸಂರಚನಾ curltime now = curlx_now();
      timediff_t timeout_ms;
      ಯಾವತ್(e) {
        qitem = Curl_ಗಂಟು_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        ಯದಿ(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->ಗಂಟು);
          fn_event = tqueue->fn_event;
          fn_user_data = tqueue->fn_user_data;
          qitem = ರಿಕ್ತ;
          e = Curl_llist_head(&tqueue->sendq);
          ಅನುವರ್ತ;
        }
        ಅನ್ಯಥಾ {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          ಭಙ್ಗ;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  ಯದಿ(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
  ಪ್ರತ್ಯಾಗಮ qitem;
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdq_tpool_return(ಶೂನ್ಯ *item, ಶೂನ್ಯ *user_data)
{
  ಸಂರಚನಾ curl_thrdq *tqueue = user_data;
  ಸಂರಚನಾ thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = ರಿಕ್ತ;
  ಶೂನ್ಯ *fn_user_data = ರಿಕ್ತ;

  ಯದಿ(!tqueue) {
    thrdq_item_destroy(item);
    ಪ್ರತ್ಯಾಗಮ;
  }

  Curl_mutex_acquire(&tqueue->lock);
  ಯದಿ(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  ಅನ್ಯಥಾ {
    DEBUGASSERT(!Curl_ಗಂಟು_llist(&qitem->ಗಂಟು));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->ಗಂಟು);
    fn_event = tqueue->fn_event;
    fn_user_data = tqueue->fn_user_data;
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  ಯದಿ(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdq_tpool_process(ಶೂನ್ಯ *item)
{
  ಸಂರಚನಾ thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdq_unlink(ಸಂರಚನಾ curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  ಯದಿ(tqueue->tpool) {
    ಯದಿ(locked)
      Curl_mutex_release(&tqueue->lock);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = ರಿಕ್ತ;
    ಯದಿ(locked)
      Curl_mutex_acquire(&tqueue->lock);
  }

  Curl_llist_destroy(&tqueue->sendq, ರಿಕ್ತ);
  Curl_llist_destroy(&tqueue->recvq, ರಿಕ್ತ);
  Curl_cond_destroy(&tqueue->await);
  ಯದಿ(locked)
    Curl_mutex_release(&tqueue->lock);
  Curl_mutex_destroy(&tqueue->lock);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(ಸಂರಚನಾ curl_thrdq **ptqueue,
                           ಧ್ರುವ ವರ್ಣ *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           ಶೂನ್ಯ *user_data)
{
  ಸಂರಚನಾ curl_thrdq *tqueue;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, ಪರಿಮಾಣ(*tqueue));
  ಯದಿ(!tqueue)
    ಗಚ್ಛ out;

  Curl_mutex_init(&tqueue->lock);
  Curl_cond_init(&tqueue->await);
  Curl_llist_init(&tqueue->sendq, thrdq_item_list_dtor);
  Curl_llist_init(&tqueue->recvq, thrdq_item_list_dtor);
  tqueue->fn_free = fn_free;
  tqueue->fn_process = fn_process;
  tqueue->fn_event = fn_event;
  tqueue->fn_user_data = user_data;

  /* a const string that remains */
  tqueue->name = name;

  result = Curl_thrdpool_create(&tqueue->tpool, name,
                                min_threads, max_threads, idle_time_ms,
                                thrdq_tpool_take,
                                thrdq_tpool_process,
                                thrdq_tpool_return,
                                tqueue);

out:
  ಯದಿ(result && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = ರಿಕ್ತ;
  }
  *ptqueue = tqueue;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_thrdq_destroy(ಸಂರಚನಾ curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->lock);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

ಸ್ಥಿರ uint32_t thrdq_get_signals(ಸಂರಚನಾ curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_ಎಣಿಕೆ(&tqueue->sendq);
  ಪ್ರತ್ಯಾಗಮ (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(ಸಂರಚನಾ curl_thrdq *tqueue, ಶೂನ್ಯ *item,
                         ಧ್ರುವ ವರ್ಣ *description, timediff_t timeout_ms)
{
  ಸಂರಚನಾ thrdq_item *qitem;
  CURLcode result = CURLE_OK;
  uint32_t signals = 0;

  Curl_mutex_acquire(&tqueue->lock);
  ಯದಿ(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ out;
  }
  ಯದಿ(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    ಗಚ್ಛ out;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  ಯದಿ(!qitem) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->ಗಂಟು);
  signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  ಯದಿ(!result && signals)
    (ಶೂನ್ಯ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ಪ್ರತ್ಯಾಗಮ result;
}

bool Curl_thrdq_check_started(ಸಂರಚನಾ curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->lock);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_ಎಣಿಕೆ(&tqueue->sendq);
  Curl_mutex_release(&tqueue->lock);
  ಪ್ರತ್ಯಾಗಮ !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(ಸಂರಚನಾ curl_thrdq *tqueue, ಶೂನ್ಯ **pitem)
{
  CURLcode result = CURLE_AGAIN;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
  uint32_t signals = 0;

  *pitem = ರಿಕ್ತ;
  Curl_mutex_acquire(&tqueue->lock);
  ಯದಿ(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_RECV_ERROR;
    ಗಚ್ಛ out;
  }

  e = Curl_llist_head(&tqueue->recvq);
  ಯದಿ(e) {
    ಸಂರಚನಾ thrdq_item *qitem = Curl_ಗಂಟು_take_elem(e);
    *pitem = qitem->item;
    qitem->item = ರಿಕ್ತ;
    thrdq_item_destroy(qitem);
    result = CURLE_OK;
  }
  ಅನ್ಯಥಾ
    signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  ಯದಿ(signals)
    (ಶೂನ್ಯ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdq_llist_clean_matches(ಸಂರಚನಾ Curl_llist *llist,
                                      Curl_thrdq_item_match_cb *fn_match,
                                      ಶೂನ್ಯ *match_data)
{
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *e, *n;
  ಸಂರಚನಾ thrdq_item *qitem;

  ಪ್ರತಿ(e = Curl_llist_head(llist); e; e = n) {
    n = Curl_ಗಂಟು_next(e);
    qitem = Curl_ಗಂಟು_elem(e);
    ಯದಿ(fn_match(qitem->item, match_data))
      Curl_ಗಂಟು_remove(e);
  }
}

ಶೂನ್ಯ Curl_thrdq_clear(ಸಂರಚನಾ curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      ಶೂನ್ಯ *match_data)
{
  Curl_mutex_acquire(&tqueue->lock);
  ಯದಿ(tqueue->aborted) {
    DEBUGASSERT(0);
    ಗಚ್ಛ out;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_match, match_data);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_match, match_data);
out:
  Curl_mutex_release(&tqueue->lock);
}

#ifdef UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_done(ಸಂರಚನಾ curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_done(ಸಂರಚನಾ curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  ಪ್ರತ್ಯಾಗಮ Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#endif

CURLcode Curl_thrdq_set_props(ಸಂರಚನಾ curl_thrdq *tqueue,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms)
{
  CURLcode result;
  uint32_t signals;

  Curl_mutex_acquire(&tqueue->lock);
  signals = thrdq_get_signals(tqueue);
  Curl_mutex_release(&tqueue->lock);

  result = Curl_thrdpool_set_props(tqueue->tpool, min_threads,
                                   max_threads, idle_time_ms);
  ಯದಿ(!result && signals)
    result = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef CURLVERBOSE
ಶೂನ್ಯ Curl_thrdq_trace(ಸಂರಚನಾ curl_thrdq *tqueue,
                      ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ curl_trc_feat *feat = &Curl_trc_feat_threads;
  ಯದಿ(Curl_trc_ft_is_verbose(data, feat)) {
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
    ಸಂರಚನಾ thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, data);
    Curl_mutex_acquire(&tqueue->lock);
    ಯದಿ(!Curl_llist_ಎಣಿಕೆ(&tqueue->sendq) &&
       !Curl_llist_ಎಣಿಕೆ(&tqueue->recvq)) {
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    ಪ್ರತಿ(e = Curl_llist_head(&tqueue->sendq); e; e = Curl_ಗಂಟು_next(e)) {
      qitem = Curl_ಗಂಟು_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    ಪ್ರತಿ(e = Curl_llist_head(&tqueue->recvq); e; e = Curl_ಗಂಟು_next(e)) {
      qitem = Curl_ಗಂಟು_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->lock);
  }
}
#endif

#endif /* USE_THREADS */
