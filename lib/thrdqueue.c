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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_THREADS

#ଅନ୍ତର୍ଭାଵ "llist.h"
#ଅନ୍ତର୍ଭାଵ "curl_threads.h"
#ଅନ୍ତର୍ଭାଵ "thrdpool.h"
#ଅନ୍ତର୍ଭାଵ "thrdqueue.h"
#ଅନ୍ତର୍ଭାଵ "curlx/timeval.h"
#ifdef CURLVERBOSE
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#endif


ସଂରଚନା curl_thrdq {
  ଅଚଳ ଅକ୍ଷର *name;
  curl_mutex_t lock;
  curl_cond_t await;
  ସଂରଚନା Curl_llist sendq;
  ସଂରଚନା Curl_llist recvq;
  ସଂରଚନା curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  ଶୂନ୍ୟ *fn_user_data;
  BIT(aborted);
};

ସଂରଚନା thrdq_item {
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି ଗଣ୍ଠି;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  ଶୂନ୍ୟ *item;
  ସଂରଚନା curltime start;
  timediff_t timeout_ms;
  ଅଚଳ ଅକ୍ଷର *description;
};

ସ୍ଥିର ସଂରଚନା thrdq_item *thrdq_item_create(ସଂରଚନା curl_thrdq *tqueue,
                                            ଶୂନ୍ୟ *item,
                                            ଅଚଳ ଅକ୍ଷର *description,
                                            timediff_t timeout_ms)
{
  ସଂରଚନା thrdq_item *qitem;

  qitem = curlx_calloc(1, ଆକାର(*qitem));
  ଯଦି(!qitem)
    ଫେରନ୍ତୁ ଖାଲି;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  ଯଦି(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  ଫେରନ୍ତୁ qitem;
}

ସ୍ଥିର ଶୂନ୍ୟ thrdq_item_destroy(ସଂରଚନା thrdq_item *qitem)
{
  ଯଦି(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

ସ୍ଥିର ଶୂନ୍ୟ thrdq_item_list_dtor(ଶୂନ୍ୟ *user_data, ଶୂନ୍ୟ *elem)
{
  (ଶୂନ୍ୟ)user_data;
  thrdq_item_destroy(elem);
}

ସ୍ଥିର ଶୂନ୍ୟ *thrdq_tpool_take(ଶୂନ୍ୟ *user_data, ଅଚଳ ଅକ୍ଷର **pdescription,
                              timediff_t *ptimeout_ms)
{
  ସଂରଚନା curl_thrdq *tqueue = user_data;
  ସଂରଚନା thrdq_item *qitem = ଖାଲି;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
  Curl_thrdq_ev_cb *fn_event = ଖାଲି;
  ଶୂନ୍ୟ *fn_user_data = ଖାଲି;

  Curl_mutex_acquire(&tqueue->lock);
  *pdescription = ଖାଲି;
  *ptimeout_ms = 0;
  ଯଦି(!tqueue->aborted) {
    e = Curl_llist_head(&tqueue->sendq);
    ଯଦି(e) {
      ସଂରଚନା curltime now = curlx_now();
      timediff_t timeout_ms;
      ଯେତେବେଳେ(e) {
        qitem = Curl_ଗଣ୍ଠି_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        ଯଦି(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->ଗଣ୍ଠି);
          fn_event = tqueue->fn_event;
          fn_user_data = tqueue->fn_user_data;
          qitem = ଖାଲି;
          e = Curl_llist_head(&tqueue->sendq);
          ଜାରି;
        }
        ଅନ୍ୟଥା {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          ଭାଙ୍ଗ;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  ଯଦି(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
  ଫେରନ୍ତୁ qitem;
}

ସ୍ଥିର ଶୂନ୍ୟ thrdq_tpool_return(ଶୂନ୍ୟ *item, ଶୂନ୍ୟ *user_data)
{
  ସଂରଚନା curl_thrdq *tqueue = user_data;
  ସଂରଚନା thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = ଖାଲି;
  ଶୂନ୍ୟ *fn_user_data = ଖାଲି;

  ଯଦି(!tqueue) {
    thrdq_item_destroy(item);
    ଫେରନ୍ତୁ;
  }

  Curl_mutex_acquire(&tqueue->lock);
  ଯଦି(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  ଅନ୍ୟଥା {
    DEBUGASSERT(!Curl_ଗଣ୍ଠି_llist(&qitem->ଗଣ୍ଠି));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->ଗଣ୍ଠି);
    fn_event = tqueue->fn_event;
    fn_user_data = tqueue->fn_user_data;
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  ଯଦି(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
}

ସ୍ଥିର ଶୂନ୍ୟ thrdq_tpool_process(ଶୂନ୍ୟ *item)
{
  ସଂରଚନା thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

ସ୍ଥିର ଶୂନ୍ୟ thrdq_unlink(ସଂରଚନା curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  ଯଦି(tqueue->tpool) {
    ଯଦି(locked)
      Curl_mutex_release(&tqueue->lock);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = ଖାଲି;
    ଯଦି(locked)
      Curl_mutex_acquire(&tqueue->lock);
  }

  Curl_llist_destroy(&tqueue->sendq, ଖାଲି);
  Curl_llist_destroy(&tqueue->recvq, ଖାଲି);
  Curl_cond_destroy(&tqueue->await);
  ଯଦି(locked)
    Curl_mutex_release(&tqueue->lock);
  Curl_mutex_destroy(&tqueue->lock);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(ସଂରଚନା curl_thrdq **ptqueue,
                           ଅଚଳ ଅକ୍ଷର *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           ଶୂନ୍ୟ *user_data)
{
  ସଂରଚନା curl_thrdq *tqueue;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, ଆକାର(*tqueue));
  ଯଦି(!tqueue)
    ଯାଅ out;

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
  ଯଦି(result && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = ଖାଲି;
  }
  *ptqueue = tqueue;
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_thrdq_destroy(ସଂରଚନା curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->lock);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

ସ୍ଥିର uint32_t thrdq_get_signals(ସଂରଚନା curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_ଗଣନା(&tqueue->sendq);
  ଫେରନ୍ତୁ (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(ସଂରଚନା curl_thrdq *tqueue, ଶୂନ୍ୟ *item,
                         ଅଚଳ ଅକ୍ଷର *description, timediff_t timeout_ms)
{
  ସଂରଚନା thrdq_item *qitem;
  CURLcode result = CURLE_OK;
  uint32_t signals = 0;

  Curl_mutex_acquire(&tqueue->lock);
  ଯଦି(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_SEND_ERROR;
    ଯାଅ out;
  }
  ଯଦି(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    ଯାଅ out;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  ଯଦି(!qitem) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->ଗଣ୍ଠି);
  signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  ଯଦି(!result && signals)
    (ଶୂନ୍ୟ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ଫେରନ୍ତୁ result;
}

bool Curl_thrdq_check_started(ସଂରଚନା curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->lock);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_ଗଣନା(&tqueue->sendq);
  Curl_mutex_release(&tqueue->lock);
  ଫେରନ୍ତୁ !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(ସଂରଚନା curl_thrdq *tqueue, ଶୂନ୍ୟ **pitem)
{
  CURLcode result = CURLE_AGAIN;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
  uint32_t signals = 0;

  *pitem = ଖାଲି;
  Curl_mutex_acquire(&tqueue->lock);
  ଯଦି(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_RECV_ERROR;
    ଯାଅ out;
  }

  e = Curl_llist_head(&tqueue->recvq);
  ଯଦି(e) {
    ସଂରଚନା thrdq_item *qitem = Curl_ଗଣ୍ଠି_take_elem(e);
    *pitem = qitem->item;
    qitem->item = ଖାଲି;
    thrdq_item_destroy(qitem);
    result = CURLE_OK;
  }
  ଅନ୍ୟଥା
    signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  ଯଦି(signals)
    (ଶୂନ୍ୟ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ thrdq_llist_clean_matches(ସଂରଚନା Curl_llist *llist,
                                      Curl_thrdq_item_match_cb *fn_match,
                                      ଶୂନ୍ୟ *match_data)
{
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e, *n;
  ସଂରଚନା thrdq_item *qitem;

  ପାଇଁ(e = Curl_llist_head(llist); e; e = n) {
    n = Curl_ଗଣ୍ଠି_next(e);
    qitem = Curl_ଗଣ୍ଠି_elem(e);
    ଯଦି(fn_match(qitem->item, match_data))
      Curl_ଗଣ୍ଠି_remove(e);
  }
}

ଶୂନ୍ୟ Curl_thrdq_clear(ସଂରଚନା curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      ଶୂନ୍ୟ *match_data)
{
  Curl_mutex_acquire(&tqueue->lock);
  ଯଦି(tqueue->aborted) {
    DEBUGASSERT(0);
    ଯାଅ out;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_match, match_data);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_match, match_data);
out:
  Curl_mutex_release(&tqueue->lock);
}

#ifdef UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_done(ସଂରଚନା curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_done(ସଂରଚନା curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  ଫେରନ୍ତୁ Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#endif

CURLcode Curl_thrdq_set_props(ସଂରଚନା curl_thrdq *tqueue,
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
  ଯଦି(!result && signals)
    result = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ଫେରନ୍ତୁ result;
}

#ifdef CURLVERBOSE
ଶୂନ୍ୟ Curl_thrdq_trace(ସଂରଚନା curl_thrdq *tqueue,
                      ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା curl_trc_feat *feat = &Curl_trc_feat_threads;
  ଯଦି(Curl_trc_ft_is_verbose(data, feat)) {
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
    ସଂରଚନା thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, data);
    Curl_mutex_acquire(&tqueue->lock);
    ଯଦି(!Curl_llist_ଗଣନା(&tqueue->sendq) &&
       !Curl_llist_ଗଣନା(&tqueue->recvq)) {
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    ପାଇଁ(e = Curl_llist_head(&tqueue->sendq); e; e = Curl_ଗଣ୍ଠି_next(e)) {
      qitem = Curl_ଗଣ୍ଠି_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    ପାଇଁ(e = Curl_llist_head(&tqueue->recvq); e; e = Curl_ଗଣ୍ଠି_next(e)) {
      qitem = Curl_ଗଣ୍ଠି_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->lock);
  }
}
#endif

#endif /* USE_THREADS */
