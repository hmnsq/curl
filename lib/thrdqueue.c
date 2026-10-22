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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_THREADS

#அந்தர்பாவ "llist.h"
#அந்தர்பாவ "curl_threads.h"
#அந்தர்பாவ "thrdpool.h"
#அந்தர்பாவ "thrdqueue.h"
#அந்தர்பாவ "curlx/timeval.h"
#ifdef CURLVERBOSE
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "urldata.h"
#endif


கட்டமைப்பு curl_thrdq {
  மாறிலி எழுத்து *name;
  curl_mutex_t lock;
  curl_cond_t await;
  கட்டமைப்பு Curl_llist sendq;
  கட்டமைப்பு Curl_llist recvq;
  கட்டமைப்பு curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  வெறுமை *fn_user_data;
  BIT(aborted);
};

கட்டமைப்பு thrdq_item {
  கட்டமைப்பு Curl_llist_முனை முனை;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  வெறுமை *item;
  கட்டமைப்பு curltime start;
  timediff_t timeout_ms;
  மாறிலி எழுத்து *description;
};

நிலையான கட்டமைப்பு thrdq_item *thrdq_item_create(கட்டமைப்பு curl_thrdq *tqueue,
                                            வெறுமை *item,
                                            மாறிலி எழுத்து *description,
                                            timediff_t timeout_ms)
{
  கட்டமைப்பு thrdq_item *qitem;

  qitem = curlx_calloc(1, அளவு(*qitem));
  எனில்(!qitem)
    திரும்பு வெற்று;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  எனில்(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  திரும்பு qitem;
}

நிலையான வெறுமை thrdq_item_destroy(கட்டமைப்பு thrdq_item *qitem)
{
  எனில்(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

நிலையான வெறுமை thrdq_item_list_dtor(வெறுமை *user_data, வெறுமை *elem)
{
  (வெறுமை)user_data;
  thrdq_item_destroy(elem);
}

நிலையான வெறுமை *thrdq_tpool_take(வெறுமை *user_data, மாறிலி எழுத்து **pdescription,
                              timediff_t *ptimeout_ms)
{
  கட்டமைப்பு curl_thrdq *tqueue = user_data;
  கட்டமைப்பு thrdq_item *qitem = வெற்று;
  கட்டமைப்பு Curl_llist_முனை *e;
  Curl_thrdq_ev_cb *fn_event = வெற்று;
  வெறுமை *fn_user_data = வெற்று;

  Curl_mutex_acquire(&tqueue->lock);
  *pdescription = வெற்று;
  *ptimeout_ms = 0;
  எனில்(!tqueue->aborted) {
    e = Curl_llist_head(&tqueue->sendq);
    எனில்(e) {
      கட்டமைப்பு curltime now = curlx_now();
      timediff_t timeout_ms;
      போது(e) {
        qitem = Curl_முனை_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        எனில்(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->முனை);
          fn_event = tqueue->fn_event;
          fn_user_data = tqueue->fn_user_data;
          qitem = வெற்று;
          e = Curl_llist_head(&tqueue->sendq);
          தொடர்;
        }
        இல்லையெனில் {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          நிறுத்து;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  எனில்(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
  திரும்பு qitem;
}

நிலையான வெறுமை thrdq_tpool_return(வெறுமை *item, வெறுமை *user_data)
{
  கட்டமைப்பு curl_thrdq *tqueue = user_data;
  கட்டமைப்பு thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = வெற்று;
  வெறுமை *fn_user_data = வெற்று;

  எனில்(!tqueue) {
    thrdq_item_destroy(item);
    திரும்பு;
  }

  Curl_mutex_acquire(&tqueue->lock);
  எனில்(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  இல்லையெனில் {
    DEBUGASSERT(!Curl_முனை_llist(&qitem->முனை));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->முனை);
    fn_event = tqueue->fn_event;
    fn_user_data = tqueue->fn_user_data;
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  எனில்(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
}

நிலையான வெறுமை thrdq_tpool_process(வெறுமை *item)
{
  கட்டமைப்பு thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

நிலையான வெறுமை thrdq_unlink(கட்டமைப்பு curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  எனில்(tqueue->tpool) {
    எனில்(locked)
      Curl_mutex_release(&tqueue->lock);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = வெற்று;
    எனில்(locked)
      Curl_mutex_acquire(&tqueue->lock);
  }

  Curl_llist_destroy(&tqueue->sendq, வெற்று);
  Curl_llist_destroy(&tqueue->recvq, வெற்று);
  Curl_cond_destroy(&tqueue->await);
  எனில்(locked)
    Curl_mutex_release(&tqueue->lock);
  Curl_mutex_destroy(&tqueue->lock);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(கட்டமைப்பு curl_thrdq **ptqueue,
                           மாறிலி எழுத்து *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           வெறுமை *user_data)
{
  கட்டமைப்பு curl_thrdq *tqueue;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, அளவு(*tqueue));
  எனில்(!tqueue)
    செல் out;

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
  எனில்(result && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = வெற்று;
  }
  *ptqueue = tqueue;
  திரும்பு result;
}

வெறுமை Curl_thrdq_destroy(கட்டமைப்பு curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->lock);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

நிலையான uint32_t thrdq_get_signals(கட்டமைப்பு curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_மொத்தம்(&tqueue->sendq);
  திரும்பு (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(கட்டமைப்பு curl_thrdq *tqueue, வெறுமை *item,
                         மாறிலி எழுத்து *description, timediff_t timeout_ms)
{
  கட்டமைப்பு thrdq_item *qitem;
  CURLcode result = CURLE_OK;
  uint32_t signals = 0;

  Curl_mutex_acquire(&tqueue->lock);
  எனில்(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_SEND_ERROR;
    செல் out;
  }
  எனில்(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    செல் out;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  எனில்(!qitem) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->முனை);
  signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  எனில்(!result && signals)
    (வெறுமை)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  திரும்பு result;
}

bool Curl_thrdq_check_started(கட்டமைப்பு curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->lock);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_மொத்தம்(&tqueue->sendq);
  Curl_mutex_release(&tqueue->lock);
  திரும்பு !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(கட்டமைப்பு curl_thrdq *tqueue, வெறுமை **pitem)
{
  CURLcode result = CURLE_AGAIN;
  கட்டமைப்பு Curl_llist_முனை *e;
  uint32_t signals = 0;

  *pitem = வெற்று;
  Curl_mutex_acquire(&tqueue->lock);
  எனில்(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_RECV_ERROR;
    செல் out;
  }

  e = Curl_llist_head(&tqueue->recvq);
  எனில்(e) {
    கட்டமைப்பு thrdq_item *qitem = Curl_முனை_take_elem(e);
    *pitem = qitem->item;
    qitem->item = வெற்று;
    thrdq_item_destroy(qitem);
    result = CURLE_OK;
  }
  இல்லையெனில்
    signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  எனில்(signals)
    (வெறுமை)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  திரும்பு result;
}

நிலையான வெறுமை thrdq_llist_clean_matches(கட்டமைப்பு Curl_llist *llist,
                                      Curl_thrdq_item_match_cb *fn_match,
                                      வெறுமை *match_data)
{
  கட்டமைப்பு Curl_llist_முனை *e, *n;
  கட்டமைப்பு thrdq_item *qitem;

  க்கு(e = Curl_llist_head(llist); e; e = n) {
    n = Curl_முனை_next(e);
    qitem = Curl_முனை_elem(e);
    எனில்(fn_match(qitem->item, match_data))
      Curl_முனை_remove(e);
  }
}

வெறுமை Curl_thrdq_clear(கட்டமைப்பு curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      வெறுமை *match_data)
{
  Curl_mutex_acquire(&tqueue->lock);
  எனில்(tqueue->aborted) {
    DEBUGASSERT(0);
    செல் out;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_match, match_data);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_match, match_data);
out:
  Curl_mutex_release(&tqueue->lock);
}

#ifdef UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_done(கட்டமைப்பு curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_done(கட்டமைப்பு curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  திரும்பு Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#endif

CURLcode Curl_thrdq_set_props(கட்டமைப்பு curl_thrdq *tqueue,
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
  எனில்(!result && signals)
    result = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  திரும்பு result;
}

#ifdef CURLVERBOSE
வெறுமை Curl_thrdq_trace(கட்டமைப்பு curl_thrdq *tqueue,
                      கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு curl_trc_feat *feat = &Curl_trc_feat_threads;
  எனில்(Curl_trc_ft_is_verbose(data, feat)) {
    கட்டமைப்பு Curl_llist_முனை *e;
    கட்டமைப்பு thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, data);
    Curl_mutex_acquire(&tqueue->lock);
    எனில்(!Curl_llist_மொத்தம்(&tqueue->sendq) &&
       !Curl_llist_மொத்தம்(&tqueue->recvq)) {
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    க்கு(e = Curl_llist_head(&tqueue->sendq); e; e = Curl_முனை_next(e)) {
      qitem = Curl_முனை_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    க்கு(e = Curl_llist_head(&tqueue->recvq); e; e = Curl_முனை_next(e)) {
      qitem = Curl_முனை_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->lock);
  }
}
#endif

#endif /* USE_THREADS */
