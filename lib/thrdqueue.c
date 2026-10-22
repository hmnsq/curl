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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef USE_THREADS

#অন্তর্ভাৱ "llist.h"
#অন্তর্ভাৱ "curl_threads.h"
#অন্তর্ভাৱ "thrdpool.h"
#অন্তর্ভাৱ "thrdqueue.h"
#অন্তর্ভাৱ "curlx/timeval.h"
#ifdef CURLVERBOSE
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "urldata.h"
#endif


সংরচনা curl_thrdq {
  ধ্রুৱ ৱর্ণ *name;
  curl_mutex_t lock;
  curl_cond_t await;
  সংরচনা Curl_llist sendq;
  সংরচনা Curl_llist recvq;
  সংরচনা curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  শূন্য *fn_user_data;
  BIT(aborted);
};

সংরচনা thrdq_item {
  সংরচনা Curl_llist_গ্ৰন্থি গ্ৰন্থি;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  শূন্য *item;
  সংরচনা curltime start;
  timediff_t timeout_ms;
  ধ্রুৱ ৱর্ণ *description;
};

স্থির সংরচনা thrdq_item *thrdq_item_create(সংরচনা curl_thrdq *tqueue,
                                            শূন্য *item,
                                            ধ্রুৱ ৱর্ণ *description,
                                            timediff_t timeout_ms)
{
  সংরচনা thrdq_item *qitem;

  qitem = curlx_calloc(1, পরিমাণ(*qitem));
  যদি(!qitem)
    ঘূৰাই রিক্ত;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  যদি(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  ঘূৰাই qitem;
}

স্থির শূন্য thrdq_item_destroy(সংরচনা thrdq_item *qitem)
{
  যদি(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

স্থির শূন্য thrdq_item_list_dtor(শূন্য *user_data, শূন্য *elem)
{
  (শূন্য)user_data;
  thrdq_item_destroy(elem);
}

স্থির শূন্য *thrdq_tpool_take(শূন্য *user_data, ধ্রুৱ ৱর্ণ **pdescription,
                              timediff_t *ptimeout_ms)
{
  সংরচনা curl_thrdq *tqueue = user_data;
  সংরচনা thrdq_item *qitem = রিক্ত;
  সংরচনা Curl_llist_গ্ৰন্থি *e;
  Curl_thrdq_ev_cb *fn_event = রিক্ত;
  শূন্য *fn_user_data = রিক্ত;

  Curl_mutex_acquire(&tqueue->lock);
  *pdescription = রিক্ত;
  *ptimeout_ms = 0;
  যদি(!tqueue->aborted) {
    e = Curl_llist_head(&tqueue->sendq);
    যদি(e) {
      সংরচনা curltime now = curlx_now();
      timediff_t timeout_ms;
      যেতিয়ালৈ(e) {
        qitem = Curl_গ্ৰন্থি_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        যদি(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->গ্ৰন্থি);
          fn_event = tqueue->fn_event;
          fn_user_data = tqueue->fn_user_data;
          qitem = রিক্ত;
          e = Curl_llist_head(&tqueue->sendq);
          অনুৱর্ত;
        }
        অন্যথা {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          ভঙ্গ;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  যদি(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
  ঘূৰাই qitem;
}

স্থির শূন্য thrdq_tpool_return(শূন্য *item, শূন্য *user_data)
{
  সংরচনা curl_thrdq *tqueue = user_data;
  সংরচনা thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = রিক্ত;
  শূন্য *fn_user_data = রিক্ত;

  যদি(!tqueue) {
    thrdq_item_destroy(item);
    ঘূৰাই;
  }

  Curl_mutex_acquire(&tqueue->lock);
  যদি(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  অন্যথা {
    DEBUGASSERT(!Curl_গ্ৰন্থি_llist(&qitem->গ্ৰন্থি));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->গ্ৰন্থি);
    fn_event = tqueue->fn_event;
    fn_user_data = tqueue->fn_user_data;
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  যদি(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
}

স্থির শূন্য thrdq_tpool_process(শূন্য *item)
{
  সংরচনা thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

স্থির শূন্য thrdq_unlink(সংরচনা curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  যদি(tqueue->tpool) {
    যদি(locked)
      Curl_mutex_release(&tqueue->lock);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = রিক্ত;
    যদি(locked)
      Curl_mutex_acquire(&tqueue->lock);
  }

  Curl_llist_destroy(&tqueue->sendq, রিক্ত);
  Curl_llist_destroy(&tqueue->recvq, রিক্ত);
  Curl_cond_destroy(&tqueue->await);
  যদি(locked)
    Curl_mutex_release(&tqueue->lock);
  Curl_mutex_destroy(&tqueue->lock);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(সংরচনা curl_thrdq **ptqueue,
                           ধ্রুৱ ৱর্ণ *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           শূন্য *user_data)
{
  সংরচনা curl_thrdq *tqueue;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, পরিমাণ(*tqueue));
  যদি(!tqueue)
    গচ্ছ out;

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
  যদি(result && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = রিক্ত;
  }
  *ptqueue = tqueue;
  ঘূৰাই result;
}

শূন্য Curl_thrdq_destroy(সংরচনা curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->lock);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

স্থির uint32_t thrdq_get_signals(সংরচনা curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_গুনতি(&tqueue->sendq);
  ঘূৰাই (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(সংরচনা curl_thrdq *tqueue, শূন্য *item,
                         ধ্রুৱ ৱর্ণ *description, timediff_t timeout_ms)
{
  সংরচনা thrdq_item *qitem;
  CURLcode result = CURLE_OK;
  uint32_t signals = 0;

  Curl_mutex_acquire(&tqueue->lock);
  যদি(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_SEND_ERROR;
    গচ্ছ out;
  }
  যদি(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    গচ্ছ out;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  যদি(!qitem) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->গ্ৰন্থি);
  signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  যদি(!result && signals)
    (শূন্য)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ঘূৰাই result;
}

bool Curl_thrdq_check_started(সংরচনা curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->lock);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_গুনতি(&tqueue->sendq);
  Curl_mutex_release(&tqueue->lock);
  ঘূৰাই !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(সংরচনা curl_thrdq *tqueue, শূন্য **pitem)
{
  CURLcode result = CURLE_AGAIN;
  সংরচনা Curl_llist_গ্ৰন্থি *e;
  uint32_t signals = 0;

  *pitem = রিক্ত;
  Curl_mutex_acquire(&tqueue->lock);
  যদি(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_RECV_ERROR;
    গচ্ছ out;
  }

  e = Curl_llist_head(&tqueue->recvq);
  যদি(e) {
    সংরচনা thrdq_item *qitem = Curl_গ্ৰন্থি_take_elem(e);
    *pitem = qitem->item;
    qitem->item = রিক্ত;
    thrdq_item_destroy(qitem);
    result = CURLE_OK;
  }
  অন্যথা
    signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  যদি(signals)
    (শূন্য)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ঘূৰাই result;
}

স্থির শূন্য thrdq_llist_clean_matches(সংরচনা Curl_llist *llist,
                                      Curl_thrdq_item_match_cb *fn_match,
                                      শূন্য *match_data)
{
  সংরচনা Curl_llist_গ্ৰন্থি *e, *n;
  সংরচনা thrdq_item *qitem;

  বাবে(e = Curl_llist_head(llist); e; e = n) {
    n = Curl_গ্ৰন্থি_next(e);
    qitem = Curl_গ্ৰন্থি_elem(e);
    যদি(fn_match(qitem->item, match_data))
      Curl_গ্ৰন্থি_remove(e);
  }
}

শূন্য Curl_thrdq_clear(সংরচনা curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      শূন্য *match_data)
{
  Curl_mutex_acquire(&tqueue->lock);
  যদি(tqueue->aborted) {
    DEBUGASSERT(0);
    গচ্ছ out;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_match, match_data);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_match, match_data);
out:
  Curl_mutex_release(&tqueue->lock);
}

#ifdef UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_done(সংরচনা curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_done(সংরচনা curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  ঘূৰাই Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#endif

CURLcode Curl_thrdq_set_props(সংরচনা curl_thrdq *tqueue,
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
  যদি(!result && signals)
    result = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ঘূৰাই result;
}

#ifdef CURLVERBOSE
শূন্য Curl_thrdq_trace(সংরচনা curl_thrdq *tqueue,
                      সংরচনা Curl_easy *data)
{
  সংরচনা curl_trc_feat *feat = &Curl_trc_feat_threads;
  যদি(Curl_trc_ft_is_verbose(data, feat)) {
    সংরচনা Curl_llist_গ্ৰন্থি *e;
    সংরচনা thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, data);
    Curl_mutex_acquire(&tqueue->lock);
    যদি(!Curl_llist_গুনতি(&tqueue->sendq) &&
       !Curl_llist_গুনতি(&tqueue->recvq)) {
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    বাবে(e = Curl_llist_head(&tqueue->sendq); e; e = Curl_গ্ৰন্থি_next(e)) {
      qitem = Curl_গ্ৰন্থি_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    বাবে(e = Curl_llist_head(&tqueue->recvq); e; e = Curl_গ্ৰন্থি_next(e)) {
      qitem = Curl_গ্ৰন্থি_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->lock);
  }
}
#endif

#endif /* USE_THREADS */
