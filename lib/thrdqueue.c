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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_THREADS

#അന്തര്ഭാവ "llist.h"
#അന്തര്ഭാവ "curl_threads.h"
#അന്തര്ഭാവ "thrdpool.h"
#അന്തര്ഭാവ "thrdqueue.h"
#അന്തര്ഭാവ "curlx/timeval.h"
#ifdef CURLVERBOSE
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "urldata.h"
#endif


ഘടന curl_thrdq {
  സ്ഥിരം അക്ഷരം *name;
  curl_mutex_t lock;
  curl_cond_t await;
  ഘടന Curl_llist sendq;
  ഘടന Curl_llist recvq;
  ഘടന curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  ശൂന്യം *fn_user_data;
  BIT(aborted);
};

ഘടന thrdq_item {
  ഘടന Curl_llist_കണ്ണി കണ്ണി;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  ശൂന്യം *item;
  ഘടന curltime start;
  timediff_t timeout_ms;
  സ്ഥിരം അക്ഷരം *description;
};

സ്ഥിതം ഘടന thrdq_item *thrdq_item_create(ഘടന curl_thrdq *tqueue,
                                            ശൂന്യം *item,
                                            സ്ഥിരം അക്ഷരം *description,
                                            timediff_t timeout_ms)
{
  ഘടന thrdq_item *qitem;

  qitem = curlx_calloc(1, വലുപ്പം(*qitem));
  എങ്കിൽ(!qitem)
    തിരികെ ശൂന്യ;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  എങ്കിൽ(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  തിരികെ qitem;
}

സ്ഥിതം ശൂന്യം thrdq_item_destroy(ഘടന thrdq_item *qitem)
{
  എങ്കിൽ(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

സ്ഥിതം ശൂന്യം thrdq_item_list_dtor(ശൂന്യം *user_data, ശൂന്യം *elem)
{
  (ശൂന്യം)user_data;
  thrdq_item_destroy(elem);
}

സ്ഥിതം ശൂന്യം *thrdq_tpool_take(ശൂന്യം *user_data, സ്ഥിരം അക്ഷരം **pdescription,
                              timediff_t *ptimeout_ms)
{
  ഘടന curl_thrdq *tqueue = user_data;
  ഘടന thrdq_item *qitem = ശൂന്യ;
  ഘടന Curl_llist_കണ്ണി *e;
  Curl_thrdq_ev_cb *fn_event = ശൂന്യ;
  ശൂന്യം *fn_user_data = ശൂന്യ;

  Curl_mutex_acquire(&tqueue->lock);
  *pdescription = ശൂന്യ;
  *ptimeout_ms = 0;
  എങ്കിൽ(!tqueue->aborted) {
    e = Curl_llist_head(&tqueue->sendq);
    എങ്കിൽ(e) {
      ഘടന curltime now = curlx_now();
      timediff_t timeout_ms;
      ആകുമ്പോൾ(e) {
        qitem = Curl_കണ്ണി_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        എങ്കിൽ(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->കണ്ണി);
          fn_event = tqueue->fn_event;
          fn_user_data = tqueue->fn_user_data;
          qitem = ശൂന്യ;
          e = Curl_llist_head(&tqueue->sendq);
          തുടരുക;
        }
        അല്ലെങ്കിൽ {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          നിർത്തുക;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  എങ്കിൽ(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
  തിരികെ qitem;
}

സ്ഥിതം ശൂന്യം thrdq_tpool_return(ശൂന്യം *item, ശൂന്യം *user_data)
{
  ഘടന curl_thrdq *tqueue = user_data;
  ഘടന thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = ശൂന്യ;
  ശൂന്യം *fn_user_data = ശൂന്യ;

  എങ്കിൽ(!tqueue) {
    thrdq_item_destroy(item);
    തിരികെ;
  }

  Curl_mutex_acquire(&tqueue->lock);
  എങ്കിൽ(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  അല്ലെങ്കിൽ {
    DEBUGASSERT(!Curl_കണ്ണി_llist(&qitem->കണ്ണി));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->കണ്ണി);
    fn_event = tqueue->fn_event;
    fn_user_data = tqueue->fn_user_data;
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  എങ്കിൽ(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
}

സ്ഥിതം ശൂന്യം thrdq_tpool_process(ശൂന്യം *item)
{
  ഘടന thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

സ്ഥിതം ശൂന്യം thrdq_unlink(ഘടന curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  എങ്കിൽ(tqueue->tpool) {
    എങ്കിൽ(locked)
      Curl_mutex_release(&tqueue->lock);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = ശൂന്യ;
    എങ്കിൽ(locked)
      Curl_mutex_acquire(&tqueue->lock);
  }

  Curl_llist_destroy(&tqueue->sendq, ശൂന്യ);
  Curl_llist_destroy(&tqueue->recvq, ശൂന്യ);
  Curl_cond_destroy(&tqueue->await);
  എങ്കിൽ(locked)
    Curl_mutex_release(&tqueue->lock);
  Curl_mutex_destroy(&tqueue->lock);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(ഘടന curl_thrdq **ptqueue,
                           സ്ഥിരം അക്ഷരം *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           ശൂന്യം *user_data)
{
  ഘടന curl_thrdq *tqueue;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, വലുപ്പം(*tqueue));
  എങ്കിൽ(!tqueue)
    പോകുക out;

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
  എങ്കിൽ(result && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = ശൂന്യ;
  }
  *ptqueue = tqueue;
  തിരികെ result;
}

ശൂന്യം Curl_thrdq_destroy(ഘടന curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->lock);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

സ്ഥിതം uint32_t thrdq_get_signals(ഘടന curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_എണ്ണം(&tqueue->sendq);
  തിരികെ (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(ഘടന curl_thrdq *tqueue, ശൂന്യം *item,
                         സ്ഥിരം അക്ഷരം *description, timediff_t timeout_ms)
{
  ഘടന thrdq_item *qitem;
  CURLcode result = CURLE_OK;
  uint32_t signals = 0;

  Curl_mutex_acquire(&tqueue->lock);
  എങ്കിൽ(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_SEND_ERROR;
    പോകുക out;
  }
  എങ്കിൽ(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    പോകുക out;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  എങ്കിൽ(!qitem) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->കണ്ണി);
  signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  എങ്കിൽ(!result && signals)
    (ശൂന്യം)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  തിരികെ result;
}

bool Curl_thrdq_check_started(ഘടന curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->lock);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_എണ്ണം(&tqueue->sendq);
  Curl_mutex_release(&tqueue->lock);
  തിരികെ !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(ഘടന curl_thrdq *tqueue, ശൂന്യം **pitem)
{
  CURLcode result = CURLE_AGAIN;
  ഘടന Curl_llist_കണ്ണി *e;
  uint32_t signals = 0;

  *pitem = ശൂന്യ;
  Curl_mutex_acquire(&tqueue->lock);
  എങ്കിൽ(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_RECV_ERROR;
    പോകുക out;
  }

  e = Curl_llist_head(&tqueue->recvq);
  എങ്കിൽ(e) {
    ഘടന thrdq_item *qitem = Curl_കണ്ണി_take_elem(e);
    *pitem = qitem->item;
    qitem->item = ശൂന്യ;
    thrdq_item_destroy(qitem);
    result = CURLE_OK;
  }
  അല്ലെങ്കിൽ
    signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  എങ്കിൽ(signals)
    (ശൂന്യം)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം thrdq_llist_clean_matches(ഘടന Curl_llist *llist,
                                      Curl_thrdq_item_match_cb *fn_match,
                                      ശൂന്യം *match_data)
{
  ഘടന Curl_llist_കണ്ണി *e, *n;
  ഘടന thrdq_item *qitem;

  വേണ്ടി(e = Curl_llist_head(llist); e; e = n) {
    n = Curl_കണ്ണി_next(e);
    qitem = Curl_കണ്ണി_elem(e);
    എങ്കിൽ(fn_match(qitem->item, match_data))
      Curl_കണ്ണി_remove(e);
  }
}

ശൂന്യം Curl_thrdq_clear(ഘടന curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      ശൂന്യം *match_data)
{
  Curl_mutex_acquire(&tqueue->lock);
  എങ്കിൽ(tqueue->aborted) {
    DEBUGASSERT(0);
    പോകുക out;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_match, match_data);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_match, match_data);
out:
  Curl_mutex_release(&tqueue->lock);
}

#ifdef UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_done(ഘടന curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_done(ഘടന curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  തിരികെ Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#endif

CURLcode Curl_thrdq_set_props(ഘടന curl_thrdq *tqueue,
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
  എങ്കിൽ(!result && signals)
    result = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  തിരികെ result;
}

#ifdef CURLVERBOSE
ശൂന്യം Curl_thrdq_trace(ഘടന curl_thrdq *tqueue,
                      ഘടന Curl_easy *data)
{
  ഘടന curl_trc_feat *feat = &Curl_trc_feat_threads;
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, feat)) {
    ഘടന Curl_llist_കണ്ണി *e;
    ഘടന thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, data);
    Curl_mutex_acquire(&tqueue->lock);
    എങ്കിൽ(!Curl_llist_എണ്ണം(&tqueue->sendq) &&
       !Curl_llist_എണ്ണം(&tqueue->recvq)) {
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    വേണ്ടി(e = Curl_llist_head(&tqueue->sendq); e; e = Curl_കണ്ണി_next(e)) {
      qitem = Curl_കണ്ണി_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    വേണ്ടി(e = Curl_llist_head(&tqueue->recvq); e; e = Curl_കണ്ണി_next(e)) {
      qitem = Curl_കണ്ണി_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->lock);
  }
}
#endif

#endif /* USE_THREADS */
