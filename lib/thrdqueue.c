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

#ifdef USE_THREADS

#অন্তর্ভাব "llist.h"
#অন্তর্ভাব "curl_threads.h"
#অন্তর্ভাব "thrdpool.h"
#অন্তর্ভাব "thrdqueue.h"
#অন্তর্ভাব "curlx/timeval.h"
#ifdef CURLVERBOSE
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "urldata.h"
#endif


কাঠামো curl_thrdq {
  ধ্রুবক অক্ষর *name;
  curl_mutex_t lock;
  curl_cond_t await;
  কাঠামো Curl_llist sendq;
  কাঠামো Curl_llist recvq;
  কাঠামো curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  শূন্য *fn_user_তথ্য;
  BIT(aborted);
};

কাঠামো thrdq_item {
  কাঠামো Curl_llist_গ্রন্থি গ্রন্থি;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  শূন্য *item;
  কাঠামো curltime start;
  timediff_t timeout_ms;
  ধ্রুবক অক্ষর *description;
};

স্থির কাঠামো thrdq_item *thrdq_item_create(কাঠামো curl_thrdq *tqueue,
                                            শূন্য *item,
                                            ধ্রুবক অক্ষর *description,
                                            timediff_t timeout_ms)
{
  কাঠামো thrdq_item *qitem;

  qitem = curlx_calloc(1, আকার(*qitem));
  যদি(!qitem)
    ফেরত ফাঁকা;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  যদি(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  ফেরত qitem;
}

স্থির শূন্য thrdq_item_destroy(কাঠামো thrdq_item *qitem)
{
  যদি(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

স্থির শূন্য thrdq_item_list_dtor(শূন্য *user_তথ্য, শূন্য *elem)
{
  (শূন্য)user_তথ্য;
  thrdq_item_destroy(elem);
}

স্থির শূন্য *thrdq_tpool_take(শূন্য *user_তথ্য, ধ্রুবক অক্ষর **pdescription,
                              timediff_t *ptimeout_ms)
{
  কাঠামো curl_thrdq *tqueue = user_তথ্য;
  কাঠামো thrdq_item *qitem = ফাঁকা;
  কাঠামো Curl_llist_গ্রন্থি *e;
  Curl_thrdq_ev_cb *fn_event = ফাঁকা;
  শূন্য *fn_user_তথ্য = ফাঁকা;

  Curl_mutex_acquire(&tqueue->lock);
  *pdescription = ফাঁকা;
  *ptimeout_ms = 0;
  যদি(!tqueue->aborted) {
    e = Curl_llist_শীর্ষ(&tqueue->sendq);
    যদি(e) {
      কাঠামো curltime now = curlx_now();
      timediff_t timeout_ms;
      যখন(e) {
        qitem = Curl_গ্রন্থি_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        যদি(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->গ্রন্থি);
          fn_event = tqueue->fn_event;
          fn_user_তথ্য = tqueue->fn_user_তথ্য;
          qitem = ফাঁকা;
          e = Curl_llist_শীর্ষ(&tqueue->sendq);
          চালু;
        }
        অন্যথা {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          বিরতি;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  যদি(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_তথ্য);
  ফেরত qitem;
}

স্থির শূন্য thrdq_tpool_return(শূন্য *item, শূন্য *user_তথ্য)
{
  কাঠামো curl_thrdq *tqueue = user_তথ্য;
  কাঠামো thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = ফাঁকা;
  শূন্য *fn_user_তথ্য = ফাঁকা;

  যদি(!tqueue) {
    thrdq_item_destroy(item);
    ফেরত;
  }

  Curl_mutex_acquire(&tqueue->lock);
  যদি(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  অন্যথা {
    DEBUGASSERT(!Curl_গ্রন্থি_llist(&qitem->গ্রন্থি));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->গ্রন্থি);
    fn_event = tqueue->fn_event;
    fn_user_তথ্য = tqueue->fn_user_তথ্য;
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  যদি(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_তথ্য);
}

স্থির শূন্য thrdq_tpool_process(শূন্য *item)
{
  কাঠামো thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

স্থির শূন্য thrdq_unlink(কাঠামো curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  যদি(tqueue->tpool) {
    যদি(locked)
      Curl_mutex_release(&tqueue->lock);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = ফাঁকা;
    যদি(locked)
      Curl_mutex_acquire(&tqueue->lock);
  }

  Curl_llist_destroy(&tqueue->sendq, ফাঁকা);
  Curl_llist_destroy(&tqueue->recvq, ফাঁকা);
  Curl_cond_destroy(&tqueue->await);
  যদি(locked)
    Curl_mutex_release(&tqueue->lock);
  Curl_mutex_destroy(&tqueue->lock);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(কাঠামো curl_thrdq **ptqueue,
                           ধ্রুবক অক্ষর *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           শূন্য *user_তথ্য)
{
  কাঠামো curl_thrdq *tqueue;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, আকার(*tqueue));
  যদি(!tqueue)
    যাও out;

  Curl_mutex_init(&tqueue->lock);
  Curl_cond_init(&tqueue->await);
  Curl_llist_init(&tqueue->sendq, thrdq_item_list_dtor);
  Curl_llist_init(&tqueue->recvq, thrdq_item_list_dtor);
  tqueue->fn_free = fn_free;
  tqueue->fn_process = fn_process;
  tqueue->fn_event = fn_event;
  tqueue->fn_user_তথ্য = user_তথ্য;

  /* a const string that remains */
  tqueue->name = name;

  ফলাফল = Curl_thrdpool_create(&tqueue->tpool, name,
                                min_threads, max_threads, idle_time_ms,
                                thrdq_tpool_take,
                                thrdq_tpool_process,
                                thrdq_tpool_return,
                                tqueue);

out:
  যদি(ফলাফল && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = ফাঁকা;
  }
  *ptqueue = tqueue;
  ফেরত ফলাফল;
}

শূন্য Curl_thrdq_destroy(কাঠামো curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->lock);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

স্থির uint32_t thrdq_get_signals(কাঠামো curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_গুনতি(&tqueue->sendq);
  ফেরত (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(কাঠামো curl_thrdq *tqueue, শূন্য *item,
                         ধ্রুবক অক্ষর *description, timediff_t timeout_ms)
{
  কাঠামো thrdq_item *qitem;
  CURLcode ফলাফল = CURLE_OK;
  uint32_t signals = 0;

  Curl_mutex_acquire(&tqueue->lock);
  যদি(tqueue->aborted) {
    DEBUGASSERT(0);
    ফলাফল = CURLE_SEND_ERROR;
    যাও out;
  }
  যদি(timeout_ms < 0) {
    ফলাফল = CURLE_OPERATION_TIMEDOUT;
    যাও out;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  যদি(!qitem) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->গ্রন্থি);
  signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  যদি(!ফলাফল && signals)
    (শূন্য)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ফেরত ফলাফল;
}

bool Curl_thrdq_check_started(কাঠামো curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->lock);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_গুনতি(&tqueue->sendq);
  Curl_mutex_release(&tqueue->lock);
  ফেরত !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(কাঠামো curl_thrdq *tqueue, শূন্য **pitem)
{
  CURLcode ফলাফল = CURLE_AGAIN;
  কাঠামো Curl_llist_গ্রন্থি *e;
  uint32_t signals = 0;

  *pitem = ফাঁকা;
  Curl_mutex_acquire(&tqueue->lock);
  যদি(tqueue->aborted) {
    DEBUGASSERT(0);
    ফলাফল = CURLE_RECV_ERROR;
    যাও out;
  }

  e = Curl_llist_শীর্ষ(&tqueue->recvq);
  যদি(e) {
    কাঠামো thrdq_item *qitem = Curl_গ্রন্থি_take_elem(e);
    *pitem = qitem->item;
    qitem->item = ফাঁকা;
    thrdq_item_destroy(qitem);
    ফলাফল = CURLE_OK;
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
  ফেরত ফলাফল;
}

স্থির শূন্য thrdq_llist_clean_matches(কাঠামো Curl_llist *llist,
                                      Curl_thrdq_item_match_cb *fn_match,
                                      শূন্য *match_তথ্য)
{
  কাঠামো Curl_llist_গ্রন্থি *e, *n;
  কাঠামো thrdq_item *qitem;

  জন্য(e = Curl_llist_শীর্ষ(llist); e; e = n) {
    n = Curl_গ্রন্থি_পরবর্তী(e);
    qitem = Curl_গ্রন্থি_elem(e);
    যদি(fn_match(qitem->item, match_তথ্য))
      Curl_গ্রন্থি_remove(e);
  }
}

শূন্য Curl_thrdq_clear(কাঠামো curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      শূন্য *match_তথ্য)
{
  Curl_mutex_acquire(&tqueue->lock);
  যদি(tqueue->aborted) {
    DEBUGASSERT(0);
    যাও out;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_match, match_তথ্য);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_match, match_তথ্য);
out:
  Curl_mutex_release(&tqueue->lock);
}

#ifdef UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_done(কাঠামো curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_done(কাঠামো curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  ফেরত Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#endif

CURLcode Curl_thrdq_set_props(কাঠামো curl_thrdq *tqueue,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms)
{
  CURLcode ফলাফল;
  uint32_t signals;

  Curl_mutex_acquire(&tqueue->lock);
  signals = thrdq_get_signals(tqueue);
  Curl_mutex_release(&tqueue->lock);

  ফলাফল = Curl_thrdpool_set_props(tqueue->tpool, min_threads,
                                   max_threads, idle_time_ms);
  যদি(!ফলাফল && signals)
    ফলাফল = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ফেরত ফলাফল;
}

#ifdef CURLVERBOSE
শূন্য Curl_thrdq_trace(কাঠামো curl_thrdq *tqueue,
                      কাঠামো Curl_easy *তথ্য)
{
  কাঠামো curl_trc_feat *feat = &Curl_trc_feat_threads;
  যদি(Curl_trc_ft_is_verbose(তথ্য, feat)) {
    কাঠামো Curl_llist_গ্রন্থি *e;
    কাঠামো thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, তথ্য);
    Curl_mutex_acquire(&tqueue->lock);
    যদি(!Curl_llist_গুনতি(&tqueue->sendq) &&
       !Curl_llist_গুনতি(&tqueue->recvq)) {
      Curl_trc_feat_infof(তথ্য, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    জন্য(e = Curl_llist_শীর্ষ(&tqueue->sendq); e; e = Curl_গ্রন্থি_পরবর্তী(e)) {
      qitem = Curl_গ্রন্থি_elem(e);
      Curl_trc_feat_infof(তথ্য, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    জন্য(e = Curl_llist_শীর্ষ(&tqueue->recvq); e; e = Curl_গ্রন্থি_পরবর্তী(e)) {
      qitem = Curl_গ্রন্থি_elem(e);
      Curl_trc_feat_infof(তথ্য, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->lock);
  }
}
#endif

#endif /* USE_THREADS */
