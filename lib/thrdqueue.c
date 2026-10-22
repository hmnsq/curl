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

#ifdef USE_THREADS

#شامل "llist.h"
#شامل "curl_threads.h"
#شامل "thrdpool.h"
#شامل "thrdqueue.h"
#شامل "curlx/timeval.h"
#ifdef CURLVERBOSE
#شامل "curl_trc.h"
#شامل "urldata.h"
#endif


ساختار curl_thrdq {
  ثابت نویسه *name;
  curl_mutex_t lock;
  curl_cond_t await;
  ساختار Curl_llist sendq;
  ساختار Curl_llist recvq;
  ساختار curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  تهی *fn_user_data;
  BIT(aborted);
};

ساختار thrdq_item {
  ساختار Curl_llist_گره گره;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  تهی *item;
  ساختار curltime start;
  timediff_t timeout_ms;
  ثابت نویسه *description;
};

ایستا ساختار thrdq_item *thrdq_item_create(ساختار curl_thrdq *tqueue,
                                            تهی *item,
                                            ثابت نویسه *description,
                                            timediff_t timeout_ms)
{
  ساختار thrdq_item *qitem;

  qitem = curlx_calloc(1, اندازه(*qitem));
  اگر(!qitem)
    بازگشت پوچ;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  اگر(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  بازگشت qitem;
}

ایستا تهی thrdq_item_destroy(ساختار thrdq_item *qitem)
{
  اگر(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

ایستا تهی thrdq_item_list_dtor(تهی *user_data, تهی *elem)
{
  (تهی)user_data;
  thrdq_item_destroy(elem);
}

ایستا تهی *thrdq_tpool_take(تهی *user_data, ثابت نویسه **pdescription,
                              timediff_t *ptimeout_ms)
{
  ساختار curl_thrdq *tqueue = user_data;
  ساختار thrdq_item *qitem = پوچ;
  ساختار Curl_llist_گره *e;
  Curl_thrdq_ev_cb *fn_event = پوچ;
  تهی *fn_user_data = پوچ;

  Curl_mutex_acquire(&tqueue->lock);
  *pdescription = پوچ;
  *ptimeout_ms = 0;
  اگر(!tqueue->aborted) {
    e = Curl_llist_head(&tqueue->sendq);
    اگر(e) {
      ساختار curltime now = curlx_now();
      timediff_t timeout_ms;
      وقتی(e) {
        qitem = Curl_گره_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        اگر(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->گره);
          fn_event = tqueue->fn_event;
          fn_user_data = tqueue->fn_user_data;
          qitem = پوچ;
          e = Curl_llist_head(&tqueue->sendq);
          ادامه;
        }
        وگرنه {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          بشکن;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  اگر(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
  بازگشت qitem;
}

ایستا تهی thrdq_tpool_return(تهی *item, تهی *user_data)
{
  ساختار curl_thrdq *tqueue = user_data;
  ساختار thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = پوچ;
  تهی *fn_user_data = پوچ;

  اگر(!tqueue) {
    thrdq_item_destroy(item);
    بازگشت;
  }

  Curl_mutex_acquire(&tqueue->lock);
  اگر(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  وگرنه {
    DEBUGASSERT(!Curl_گره_llist(&qitem->گره));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->گره);
    fn_event = tqueue->fn_event;
    fn_user_data = tqueue->fn_user_data;
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  اگر(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
}

ایستا تهی thrdq_tpool_process(تهی *item)
{
  ساختار thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

ایستا تهی thrdq_unlink(ساختار curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  اگر(tqueue->tpool) {
    اگر(locked)
      Curl_mutex_release(&tqueue->lock);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = پوچ;
    اگر(locked)
      Curl_mutex_acquire(&tqueue->lock);
  }

  Curl_llist_destroy(&tqueue->sendq, پوچ);
  Curl_llist_destroy(&tqueue->recvq, پوچ);
  Curl_cond_destroy(&tqueue->await);
  اگر(locked)
    Curl_mutex_release(&tqueue->lock);
  Curl_mutex_destroy(&tqueue->lock);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(ساختار curl_thrdq **ptqueue,
                           ثابت نویسه *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           تهی *user_data)
{
  ساختار curl_thrdq *tqueue;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, اندازه(*tqueue));
  اگر(!tqueue)
    برو out;

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
  اگر(result && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = پوچ;
  }
  *ptqueue = tqueue;
  بازگشت result;
}

تهی Curl_thrdq_destroy(ساختار curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->lock);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

ایستا uint32_t thrdq_get_signals(ساختار curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_شمار(&tqueue->sendq);
  بازگشت (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(ساختار curl_thrdq *tqueue, تهی *item,
                         ثابت نویسه *description, timediff_t timeout_ms)
{
  ساختار thrdq_item *qitem;
  CURLcode result = CURLE_OK;
  uint32_t signals = 0;

  Curl_mutex_acquire(&tqueue->lock);
  اگر(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_SEND_ERROR;
    برو out;
  }
  اگر(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    برو out;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  اگر(!qitem) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->گره);
  signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  اگر(!result && signals)
    (تهی)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  بازگشت result;
}

bool Curl_thrdq_check_started(ساختار curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->lock);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_شمار(&tqueue->sendq);
  Curl_mutex_release(&tqueue->lock);
  بازگشت !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(ساختار curl_thrdq *tqueue, تهی **pitem)
{
  CURLcode result = CURLE_AGAIN;
  ساختار Curl_llist_گره *e;
  uint32_t signals = 0;

  *pitem = پوچ;
  Curl_mutex_acquire(&tqueue->lock);
  اگر(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_RECV_ERROR;
    برو out;
  }

  e = Curl_llist_head(&tqueue->recvq);
  اگر(e) {
    ساختار thrdq_item *qitem = Curl_گره_take_elem(e);
    *pitem = qitem->item;
    qitem->item = پوچ;
    thrdq_item_destroy(qitem);
    result = CURLE_OK;
  }
  وگرنه
    signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  اگر(signals)
    (تهی)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  بازگشت result;
}

ایستا تهی thrdq_llist_clean_matches(ساختار Curl_llist *llist,
                                      Curl_thrdq_item_match_cb *fn_match,
                                      تهی *match_data)
{
  ساختار Curl_llist_گره *e, *n;
  ساختار thrdq_item *qitem;

  برای(e = Curl_llist_head(llist); e; e = n) {
    n = Curl_گره_next(e);
    qitem = Curl_گره_elem(e);
    اگر(fn_match(qitem->item, match_data))
      Curl_گره_remove(e);
  }
}

تهی Curl_thrdq_clear(ساختار curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      تهی *match_data)
{
  Curl_mutex_acquire(&tqueue->lock);
  اگر(tqueue->aborted) {
    DEBUGASSERT(0);
    برو out;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_match, match_data);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_match, match_data);
out:
  Curl_mutex_release(&tqueue->lock);
}

#ifdef UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_done(ساختار curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_done(ساختار curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  بازگشت Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#endif

CURLcode Curl_thrdq_set_props(ساختار curl_thrdq *tqueue,
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
  اگر(!result && signals)
    result = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  بازگشت result;
}

#ifdef CURLVERBOSE
تهی Curl_thrdq_trace(ساختار curl_thrdq *tqueue,
                      ساختار Curl_easy *data)
{
  ساختار curl_trc_feat *feat = &Curl_trc_feat_threads;
  اگر(Curl_trc_ft_is_verbose(data, feat)) {
    ساختار Curl_llist_گره *e;
    ساختار thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, data);
    Curl_mutex_acquire(&tqueue->lock);
    اگر(!Curl_llist_شمار(&tqueue->sendq) &&
       !Curl_llist_شمار(&tqueue->recvq)) {
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    برای(e = Curl_llist_head(&tqueue->sendq); e; e = Curl_گره_next(e)) {
      qitem = Curl_گره_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    برای(e = Curl_llist_head(&tqueue->recvq); e; e = Curl_گره_next(e)) {
      qitem = Curl_گره_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->lock);
  }
}
#endif

#endif /* USE_THREADS */
