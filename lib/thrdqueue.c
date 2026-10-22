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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_THREADS

#અન્તર્ભાવ "llist.h"
#અન્તર્ભાવ "curl_threads.h"
#અન્તર્ભાવ "thrdpool.h"
#અન્તર્ભાવ "thrdqueue.h"
#અન્તર્ભાવ "curlx/timeval.h"
#ifdef CURLVERBOSE
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "urldata.h"
#endif


સંરચના curl_thrdq {
  અચલ અક્ષર *name;
  curl_mutex_t lock;
  curl_cond_t await;
  સંરચના Curl_llist sendq;
  સંરચના Curl_llist recvq;
  સંરચના curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  શૂન્ય *fn_user_data;
  BIT(aborted);
};

સંરચના thrdq_item {
  સંરચના Curl_llist_ગાંઠ ગાંઠ;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  શૂન્ય *item;
  સંરચના curltime start;
  timediff_t timeout_ms;
  અચલ અક્ષર *description;
};

સ્થિર સંરચના thrdq_item *thrdq_item_create(સંરચના curl_thrdq *tqueue,
                                            શૂન્ય *item,
                                            અચલ અક્ષર *description,
                                            timediff_t timeout_ms)
{
  સંરચના thrdq_item *qitem;

  qitem = curlx_calloc(1, કદ(*qitem));
  જો(!qitem)
    પાછા ખાલી;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  જો(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  પાછા qitem;
}

સ્થિર શૂન્ય thrdq_item_destroy(સંરચના thrdq_item *qitem)
{
  જો(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

સ્થિર શૂન્ય thrdq_item_list_dtor(શૂન્ય *user_data, શૂન્ય *elem)
{
  (શૂન્ય)user_data;
  thrdq_item_destroy(elem);
}

સ્થિર શૂન્ય *thrdq_tpool_take(શૂન્ય *user_data, અચલ અક્ષર **pdescription,
                              timediff_t *ptimeout_ms)
{
  સંરચના curl_thrdq *tqueue = user_data;
  સંરચના thrdq_item *qitem = ખાલી;
  સંરચના Curl_llist_ગાંઠ *e;
  Curl_thrdq_ev_cb *fn_event = ખાલી;
  શૂન્ય *fn_user_data = ખાલી;

  Curl_mutex_acquire(&tqueue->lock);
  *pdescription = ખાલી;
  *ptimeout_ms = 0;
  જો(!tqueue->aborted) {
    e = Curl_llist_head(&tqueue->sendq);
    જો(e) {
      સંરચના curltime now = curlx_now();
      timediff_t timeout_ms;
      જ્યારે(e) {
        qitem = Curl_ગાંઠ_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        જો(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->ગાંઠ);
          fn_event = tqueue->fn_event;
          fn_user_data = tqueue->fn_user_data;
          qitem = ખાલી;
          e = Curl_llist_head(&tqueue->sendq);
          ચાલુ;
        }
        નહીંતર {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          તોડો;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  જો(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
  પાછા qitem;
}

સ્થિર શૂન્ય thrdq_tpool_return(શૂન્ય *item, શૂન્ય *user_data)
{
  સંરચના curl_thrdq *tqueue = user_data;
  સંરચના thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = ખાલી;
  શૂન્ય *fn_user_data = ખાલી;

  જો(!tqueue) {
    thrdq_item_destroy(item);
    પાછા;
  }

  Curl_mutex_acquire(&tqueue->lock);
  જો(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  નહીંતર {
    DEBUGASSERT(!Curl_ગાંઠ_llist(&qitem->ગાંઠ));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->ગાંઠ);
    fn_event = tqueue->fn_event;
    fn_user_data = tqueue->fn_user_data;
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  જો(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
}

સ્થિર શૂન્ય thrdq_tpool_process(શૂન્ય *item)
{
  સંરચના thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

સ્થિર શૂન્ય thrdq_unlink(સંરચના curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  જો(tqueue->tpool) {
    જો(locked)
      Curl_mutex_release(&tqueue->lock);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = ખાલી;
    જો(locked)
      Curl_mutex_acquire(&tqueue->lock);
  }

  Curl_llist_destroy(&tqueue->sendq, ખાલી);
  Curl_llist_destroy(&tqueue->recvq, ખાલી);
  Curl_cond_destroy(&tqueue->await);
  જો(locked)
    Curl_mutex_release(&tqueue->lock);
  Curl_mutex_destroy(&tqueue->lock);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(સંરચના curl_thrdq **ptqueue,
                           અચલ અક્ષર *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           શૂન્ય *user_data)
{
  સંરચના curl_thrdq *tqueue;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, કદ(*tqueue));
  જો(!tqueue)
    જાઓ out;

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
  જો(result && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = ખાલી;
  }
  *ptqueue = tqueue;
  પાછા result;
}

શૂન્ય Curl_thrdq_destroy(સંરચના curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->lock);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

સ્થિર uint32_t thrdq_get_signals(સંરચના curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_ગણતરી(&tqueue->sendq);
  પાછા (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(સંરચના curl_thrdq *tqueue, શૂન્ય *item,
                         અચલ અક્ષર *description, timediff_t timeout_ms)
{
  સંરચના thrdq_item *qitem;
  CURLcode result = CURLE_OK;
  uint32_t signals = 0;

  Curl_mutex_acquire(&tqueue->lock);
  જો(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_SEND_ERROR;
    જાઓ out;
  }
  જો(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    જાઓ out;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  જો(!qitem) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->ગાંઠ);
  signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  જો(!result && signals)
    (શૂન્ય)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  પાછા result;
}

bool Curl_thrdq_check_started(સંરચના curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->lock);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_ગણતરી(&tqueue->sendq);
  Curl_mutex_release(&tqueue->lock);
  પાછા !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(સંરચના curl_thrdq *tqueue, શૂન્ય **pitem)
{
  CURLcode result = CURLE_AGAIN;
  સંરચના Curl_llist_ગાંઠ *e;
  uint32_t signals = 0;

  *pitem = ખાલી;
  Curl_mutex_acquire(&tqueue->lock);
  જો(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_RECV_ERROR;
    જાઓ out;
  }

  e = Curl_llist_head(&tqueue->recvq);
  જો(e) {
    સંરચના thrdq_item *qitem = Curl_ગાંઠ_take_elem(e);
    *pitem = qitem->item;
    qitem->item = ખાલી;
    thrdq_item_destroy(qitem);
    result = CURLE_OK;
  }
  નહીંતર
    signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  જો(signals)
    (શૂન્ય)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  પાછા result;
}

સ્થિર શૂન્ય thrdq_llist_clean_matches(સંરચના Curl_llist *llist,
                                      Curl_thrdq_item_match_cb *fn_match,
                                      શૂન્ય *match_data)
{
  સંરચના Curl_llist_ગાંઠ *e, *n;
  સંરચના thrdq_item *qitem;

  માટે(e = Curl_llist_head(llist); e; e = n) {
    n = Curl_ગાંઠ_next(e);
    qitem = Curl_ગાંઠ_elem(e);
    જો(fn_match(qitem->item, match_data))
      Curl_ગાંઠ_remove(e);
  }
}

શૂન્ય Curl_thrdq_clear(સંરચના curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      શૂન્ય *match_data)
{
  Curl_mutex_acquire(&tqueue->lock);
  જો(tqueue->aborted) {
    DEBUGASSERT(0);
    જાઓ out;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_match, match_data);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_match, match_data);
out:
  Curl_mutex_release(&tqueue->lock);
}

#ifdef UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_done(સંરચના curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_done(સંરચના curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  પાછા Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#endif

CURLcode Curl_thrdq_set_props(સંરચના curl_thrdq *tqueue,
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
  જો(!result && signals)
    result = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  પાછા result;
}

#ifdef CURLVERBOSE
શૂન્ય Curl_thrdq_trace(સંરચના curl_thrdq *tqueue,
                      સંરચના Curl_easy *data)
{
  સંરચના curl_trc_feat *feat = &Curl_trc_feat_threads;
  જો(Curl_trc_ft_is_verbose(data, feat)) {
    સંરચના Curl_llist_ગાંઠ *e;
    સંરચના thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, data);
    Curl_mutex_acquire(&tqueue->lock);
    જો(!Curl_llist_ગણતરી(&tqueue->sendq) &&
       !Curl_llist_ગણતરી(&tqueue->recvq)) {
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    માટે(e = Curl_llist_head(&tqueue->sendq); e; e = Curl_ગાંઠ_next(e)) {
      qitem = Curl_ગાંઠ_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    માટે(e = Curl_llist_head(&tqueue->recvq); e; e = Curl_ગાંઠ_next(e)) {
      qitem = Curl_ગાંઠ_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->lock);
  }
}
#endif

#endif /* USE_THREADS */
