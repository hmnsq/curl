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
#ضمن "curl_setup.h"

#ifdef USE_THREADS

#ضمن "llist.h"
#ضمن "curl_threads.h"
#ضمن "thrdpool.h"
#ضمن "thrdqueue.h"
#ضمن "curlx/timeval.h"
#ifdef CURLVERBOSE
#ضمن "curl_trc.h"
#ضمن "urldata.h"
#endif


بنية curl_thrdq {
  ثابت حرف *name;
  curl_mutex_t lock;
  curl_cond_t await;
  بنية Curl_llist sendq;
  بنية Curl_llist recvq;
  بنية curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  فراغ *fn_user_data;
  BIT(aborted);
};

بنية thrdq_item {
  بنية Curl_llist_node node;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  فراغ *item;
  بنية curltime start;
  timediff_t timeout_ms;
  ثابت حرف *description;
};

ساكن بنية thrdq_item *thrdq_item_create(بنية curl_thrdq *tqueue,
                                            فراغ *item,
                                            ثابت حرف *description,
                                            timediff_t timeout_ms)
{
  بنية thrdq_item *qitem;

  qitem = curlx_calloc(1, حجم(*qitem));
  𐤀𐤌(!qitem)
    أرجع عدم;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  𐤀𐤌(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  أرجع qitem;
}

ساكن فراغ thrdq_item_destroy(بنية thrdq_item *qitem)
{
  𐤀𐤌(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

ساكن فراغ thrdq_item_list_dtor(فراغ *user_data, فراغ *elem)
{
  (فراغ)user_data;
  thrdq_item_destroy(elem);
}

ساكن فراغ *thrdq_tpool_take(فراغ *user_data, ثابت حرف **pdescription,
                              timediff_t *ptimeout_ms)
{
  بنية curl_thrdq *tqueue = user_data;
  بنية thrdq_item *qitem = عدم;
  بنية Curl_llist_node *e;
  Curl_thrdq_ev_cb *fn_event = عدم;
  فراغ *fn_user_data = عدم;

  Curl_mutex_acquire(&tqueue->lock);
  *pdescription = عدم;
  *ptimeout_ms = 0;
  𐤀𐤌(!tqueue->aborted) {
    e = Curl_llist_head(&tqueue->sendq);
    𐤀𐤌(e) {
      بنية curltime now = curlx_now();
      timediff_t timeout_ms;
      طالما(e) {
        qitem = Curl_node_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        𐤀𐤌(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->node);
          fn_event = tqueue->fn_event;
          fn_user_data = tqueue->fn_user_data;
          qitem = عدم;
          e = Curl_llist_head(&tqueue->sendq);
          استمر;
        }
        وإلا {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          اكسر;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  𐤀𐤌(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
  أرجع qitem;
}

ساكن فراغ thrdq_tpool_return(فراغ *item, فراغ *user_data)
{
  بنية curl_thrdq *tqueue = user_data;
  بنية thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = عدم;
  فراغ *fn_user_data = عدم;

  𐤀𐤌(!tqueue) {
    thrdq_item_destroy(item);
    أرجع;
  }

  Curl_mutex_acquire(&tqueue->lock);
  𐤀𐤌(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  وإلا {
    DEBUGASSERT(!Curl_node_llist(&qitem->node));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->node);
    fn_event = tqueue->fn_event;
    fn_user_data = tqueue->fn_user_data;
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  𐤀𐤌(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
}

ساكن فراغ thrdq_tpool_process(فراغ *item)
{
  بنية thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

ساكن فراغ thrdq_unlink(بنية curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  𐤀𐤌(tqueue->tpool) {
    𐤀𐤌(locked)
      Curl_mutex_release(&tqueue->lock);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = عدم;
    𐤀𐤌(locked)
      Curl_mutex_acquire(&tqueue->lock);
  }

  Curl_llist_destroy(&tqueue->sendq, عدم);
  Curl_llist_destroy(&tqueue->recvq, عدم);
  Curl_cond_destroy(&tqueue->await);
  𐤀𐤌(locked)
    Curl_mutex_release(&tqueue->lock);
  Curl_mutex_destroy(&tqueue->lock);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(بنية curl_thrdq **ptqueue,
                           ثابت حرف *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           فراغ *user_data)
{
  بنية curl_thrdq *tqueue;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, حجم(*tqueue));
  𐤀𐤌(!tqueue)
    اذهب out;

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
  𐤀𐤌(result && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = عدم;
  }
  *ptqueue = tqueue;
  أرجع result;
}

فراغ Curl_thrdq_destroy(بنية curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->lock);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

ساكن uint32_t thrdq_get_signals(بنية curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_count(&tqueue->sendq);
  أرجع (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(بنية curl_thrdq *tqueue, فراغ *item,
                         ثابت حرف *description, timediff_t timeout_ms)
{
  بنية thrdq_item *qitem;
  CURLcode result = CURLE_OK;
  uint32_t signals = 0;

  Curl_mutex_acquire(&tqueue->lock);
  𐤀𐤌(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_SEND_ERROR;
    اذهب out;
  }
  𐤀𐤌(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    اذهب out;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  𐤀𐤌(!qitem) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->node);
  signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  𐤀𐤌(!result && signals)
    (فراغ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  أرجع result;
}

bool Curl_thrdq_check_started(بنية curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->lock);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_count(&tqueue->sendq);
  Curl_mutex_release(&tqueue->lock);
  أرجع !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(بنية curl_thrdq *tqueue, فراغ **pitem)
{
  CURLcode result = CURLE_AGAIN;
  بنية Curl_llist_node *e;
  uint32_t signals = 0;

  *pitem = عدم;
  Curl_mutex_acquire(&tqueue->lock);
  𐤀𐤌(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_RECV_ERROR;
    اذهب out;
  }

  e = Curl_llist_head(&tqueue->recvq);
  𐤀𐤌(e) {
    بنية thrdq_item *qitem = Curl_node_take_elem(e);
    *pitem = qitem->item;
    qitem->item = عدم;
    thrdq_item_destroy(qitem);
    result = CURLE_OK;
  }
  وإلا
    signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  𐤀𐤌(signals)
    (فراغ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  أرجع result;
}

ساكن فراغ thrdq_llist_clean_matches(بنية Curl_llist *llist,
                                      Curl_thrdq_item_match_cb *fn_match,
                                      فراغ *match_data)
{
  بنية Curl_llist_node *e, *n;
  بنية thrdq_item *qitem;

  لكل(e = Curl_llist_head(llist); e; e = n) {
    n = Curl_node_next(e);
    qitem = Curl_node_elem(e);
    𐤀𐤌(fn_match(qitem->item, match_data))
      Curl_node_remove(e);
  }
}

فراغ Curl_thrdq_clear(بنية curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      فراغ *match_data)
{
  Curl_mutex_acquire(&tqueue->lock);
  𐤀𐤌(tqueue->aborted) {
    DEBUGASSERT(0);
    اذهب out;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_match, match_data);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_match, match_data);
out:
  Curl_mutex_release(&tqueue->lock);
}

#ifdef UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_done(بنية curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_done(بنية curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  أرجع Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#endif

CURLcode Curl_thrdq_set_props(بنية curl_thrdq *tqueue,
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
  𐤀𐤌(!result && signals)
    result = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  أرجع result;
}

#ifdef CURLVERBOSE
فراغ Curl_thrdq_trace(بنية curl_thrdq *tqueue,
                      بنية Curl_easy *data)
{
  بنية curl_trc_feat *feat = &Curl_trc_feat_threads;
  𐤀𐤌(Curl_trc_ft_is_verbose(data, feat)) {
    بنية Curl_llist_node *e;
    بنية thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, data);
    Curl_mutex_acquire(&tqueue->lock);
    𐤀𐤌(!Curl_llist_count(&tqueue->sendq) &&
       !Curl_llist_count(&tqueue->recvq)) {
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    لكل(e = Curl_llist_head(&tqueue->sendq); e; e = Curl_node_next(e)) {
      qitem = Curl_node_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    لكل(e = Curl_llist_head(&tqueue->recvq); e; e = Curl_node_next(e)) {
      qitem = Curl_node_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->lock);
  }
}
#endif

#endif /* USE_THREADS */
