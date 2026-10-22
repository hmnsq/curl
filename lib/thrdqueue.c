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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਧਾਗੇ

#ਅਨ੍ਤਰ੍ਭਾਵ "llist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_threads.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "thrdpool.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "thrdqueue.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਜੇ_ਅੰਤ


ਬਣਤਰ curl_thrdq {
  ਅਚਲ ਅੱਖਰ *name;
  curl_mutex_t ਤਾਲਾ;
  curl_cond_t await;
  ਬਣਤਰ Curl_llist sendq;
  ਬਣਤਰ Curl_llist recvq;
  ਬਣਤਰ curl_thrdpool *tpool;
  Curl_thrdq_ਵਸਤੂ_free_cb *fn_free;
  Curl_thrdq_ਵਸਤੂ_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  ਖਾਲੀ *fn_user_ਅੰਕੜੇ;
  ਦੋਆਧਾਰੀ_ਅੰਕ(aborted);
};

ਬਣਤਰ thrdq_ਵਸਤੂ {
  ਬਣਤਰ Curl_llist_ਗੰਢ ਗੰਢ;
  Curl_thrdq_ਵਸਤੂ_free_cb *fn_free;
  Curl_thrdq_ਵਸਤੂ_process_cb *fn_process;
  ਖਾਲੀ *ਵਸਤੂ;
  ਬਣਤਰ curltime ਸ਼ੁਰੂ;
  timediff_t timeout_ms;
  ਅਚਲ ਅੱਖਰ *description;
};

ਸਥਿਰ ਬਣਤਰ thrdq_ਵਸਤੂ *thrdq_ਵਸਤੂ_create(ਬਣਤਰ curl_thrdq *tqueue,
                                            ਖਾਲੀ *ਵਸਤੂ,
                                            ਅਚਲ ਅੱਖਰ *description,
                                            timediff_t timeout_ms)
{
  ਬਣਤਰ thrdq_ਵਸਤੂ *qitem;

  qitem = curlx_calloc(1, ਆਕਾਰ(*qitem));
  ਜੇ(!qitem)
    ਵਾਪਸ ਨਿਰਾ;
  qitem->ਵਸਤੂ = ਵਸਤੂ;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  ਜੇ(timeout_ms) {
    qitem->ਸ਼ੁਰੂ = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  ਵਾਪਸ qitem;
}

ਸਥਿਰ ਖਾਲੀ thrdq_ਵਸਤੂ_destroy(ਬਣਤਰ thrdq_ਵਸਤੂ *qitem)
{
  ਜੇ(qitem->ਵਸਤੂ)
    qitem->fn_free(qitem->ਵਸਤੂ);
  curlx_free(qitem);
}

ਸਥਿਰ ਖਾਲੀ thrdq_ਵਸਤੂ_list_dtor(ਖਾਲੀ *user_ਅੰਕੜੇ, ਖਾਲੀ *elem)
{
  (ਖਾਲੀ)user_ਅੰਕੜੇ;
  thrdq_ਵਸਤੂ_destroy(elem);
}

ਸਥਿਰ ਖਾਲੀ *thrdq_tpool_take(ਖਾਲੀ *user_ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ **pdescription,
                              timediff_t *ptimeout_ms)
{
  ਬਣਤਰ curl_thrdq *tqueue = user_ਅੰਕੜੇ;
  ਬਣਤਰ thrdq_ਵਸਤੂ *qitem = ਨਿਰਾ;
  ਬਣਤਰ Curl_llist_ਗੰਢ *e;
  Curl_thrdq_ev_cb *fn_event = ਨਿਰਾ;
  ਖਾਲੀ *fn_user_ਅੰਕੜੇ = ਨਿਰਾ;

  Curl_mutex_acquire(&tqueue->ਤਾਲਾ);
  *pdescription = ਨਿਰਾ;
  *ptimeout_ms = 0;
  ਜੇ(!tqueue->aborted) {
    e = Curl_llist_ਸਿਰ(&tqueue->sendq);
    ਜੇ(e) {
      ਬਣਤਰ curltime now = curlx_now();
      timediff_t timeout_ms;
      ਜਦੋਂ(e) {
        qitem = Curl_ਗੰਢ_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->ਸ਼ੁਰੂ));
        ਜੇ(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->ਗੰਢ);
          fn_event = tqueue->fn_event;
          fn_user_ਅੰਕੜੇ = tqueue->fn_user_ਅੰਕੜੇ;
          qitem = ਨਿਰਾ;
          e = Curl_llist_ਸਿਰ(&tqueue->sendq);
          ਜਾਰੀ;
        }
        ਨਹੀਂਤਾਂ {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          ਰੋਕੋ;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->ਤਾਲਾ);
  /* avoiding deadlocks */
  ਜੇ(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ਵਸਤੂ_ਪੂਰਾ, fn_user_ਅੰਕੜੇ);
  ਵਾਪਸ qitem;
}

ਸਥਿਰ ਖਾਲੀ thrdq_tpool_return(ਖਾਲੀ *ਵਸਤੂ, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  ਬਣਤਰ curl_thrdq *tqueue = user_ਅੰਕੜੇ;
  ਬਣਤਰ thrdq_ਵਸਤੂ *qitem = ਵਸਤੂ;
  Curl_thrdq_ev_cb *fn_event = ਨਿਰਾ;
  ਖਾਲੀ *fn_user_ਅੰਕੜੇ = ਨਿਰਾ;

  ਜੇ(!tqueue) {
    thrdq_ਵਸਤੂ_destroy(ਵਸਤੂ);
    ਵਾਪਸ;
  }

  Curl_mutex_acquire(&tqueue->ਤਾਲਾ);
  ਜੇ(tqueue->aborted) {
    thrdq_ਵਸਤੂ_destroy(qitem);
  }
  ਨਹੀਂਤਾਂ {
    DEBUGASSERT(!Curl_ਗੰਢ_llist(&qitem->ਗੰਢ));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->ਗੰਢ);
    fn_event = tqueue->fn_event;
    fn_user_ਅੰਕੜੇ = tqueue->fn_user_ਅੰਕੜੇ;
  }
  Curl_mutex_release(&tqueue->ਤਾਲਾ);
  /* avoiding deadlocks */
  ਜੇ(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ਵਸਤੂ_ਪੂਰਾ, fn_user_ਅੰਕੜੇ);
}

ਸਥਿਰ ਖਾਲੀ thrdq_tpool_process(ਖਾਲੀ *ਵਸਤੂ)
{
  ਬਣਤਰ thrdq_ਵਸਤੂ *qitem = ਵਸਤੂ;
  qitem->fn_process(qitem->ਵਸਤੂ);
}

ਸਥਿਰ ਖਾਲੀ thrdq_unlink(ਬਣਤਰ curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  ਜੇ(tqueue->tpool) {
    ਜੇ(locked)
      Curl_mutex_release(&tqueue->ਤਾਲਾ);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = ਨਿਰਾ;
    ਜੇ(locked)
      Curl_mutex_acquire(&tqueue->ਤਾਲਾ);
  }

  Curl_llist_destroy(&tqueue->sendq, ਨਿਰਾ);
  Curl_llist_destroy(&tqueue->recvq, ਨਿਰਾ);
  Curl_cond_destroy(&tqueue->await);
  ਜੇ(locked)
    Curl_mutex_release(&tqueue->ਤਾਲਾ);
  Curl_mutex_destroy(&tqueue->ਤਾਲਾ);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(ਬਣਤਰ curl_thrdq **ptqueue,
                           ਅਚਲ ਅੱਖਰ *name,
                           uint32_t min_ਧਾਗੇ,
                           uint32_t max_ਧਾਗੇ,
                           uint32_t idle_ਸਮਾਂ_ms,
                           Curl_thrdq_ਵਸਤੂ_free_cb *fn_free,
                           Curl_thrdq_ਵਸਤੂ_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  ਬਣਤਰ curl_thrdq *tqueue;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, ਆਕਾਰ(*tqueue));
  ਜੇ(!tqueue)
    ਜਾਓ ਬਾਹਰਲਾ;

  Curl_mutex_init(&tqueue->ਤਾਲਾ);
  Curl_cond_init(&tqueue->await);
  Curl_llist_init(&tqueue->sendq, thrdq_ਵਸਤੂ_list_dtor);
  Curl_llist_init(&tqueue->recvq, thrdq_ਵਸਤੂ_list_dtor);
  tqueue->fn_free = fn_free;
  tqueue->fn_process = fn_process;
  tqueue->fn_event = fn_event;
  tqueue->fn_user_ਅੰਕੜੇ = user_ਅੰਕੜੇ;

  /* a const string that remains */
  tqueue->name = name;

  ਨਤੀਜਾ = Curl_thrdpool_create(&tqueue->tpool, name,
                                min_ਧਾਗੇ, max_ਧਾਗੇ, idle_ਸਮਾਂ_ms,
                                thrdq_tpool_take,
                                thrdq_tpool_process,
                                thrdq_tpool_return,
                                tqueue);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = ਨਿਰਾ;
  }
  *ptqueue = tqueue;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_thrdq_destroy(ਬਣਤਰ curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->ਤਾਲਾ);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

ਸਥਿਰ uint32_t thrdq_get_ਸੰਕੇਤ(ਬਣਤਰ curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_ਗਿਣਤੀ(&tqueue->sendq);
  ਵਾਪਸ (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(ਬਣਤਰ curl_thrdq *tqueue, ਖਾਲੀ *ਵਸਤੂ,
                         ਅਚਲ ਅੱਖਰ *description, timediff_t timeout_ms)
{
  ਬਣਤਰ thrdq_ਵਸਤੂ *qitem;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  uint32_t ਸੰਕੇਤ_2 = 0;

  Curl_mutex_acquire(&tqueue->ਤਾਲਾ);
  ਜੇ(tqueue->aborted) {
    DEBUGASSERT(0);
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(timeout_ms < 0) {
    ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  qitem = thrdq_ਵਸਤੂ_create(tqueue, ਵਸਤੂ, description, timeout_ms);
  ਜੇ(!qitem) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->ਗੰਢ);
  ਸੰਕੇਤ_2 = thrdq_get_ਸੰਕੇਤ(tqueue);

ਬਾਹਰਲਾ:
  Curl_mutex_release(&tqueue->ਤਾਲਾ);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  ਜੇ(!ਨਤੀਜਾ && ਸੰਕੇਤ_2)
    (ਖਾਲੀ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)ਸੰਕੇਤ_2);
  ਵਾਪਸ ਨਤੀਜਾ;
}

bool Curl_thrdq_check_started(ਬਣਤਰ curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->ਤਾਲਾ);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_ਗਿਣਤੀ(&tqueue->sendq);
  Curl_mutex_release(&tqueue->ਤਾਲਾ);
  ਵਾਪਸ !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(ਬਣਤਰ curl_thrdq *tqueue, ਖਾਲੀ **pitem)
{
  CURLcode ਨਤੀਜਾ = CURLE_AGAIN;
  ਬਣਤਰ Curl_llist_ਗੰਢ *e;
  uint32_t ਸੰਕੇਤ_2 = 0;

  *pitem = ਨਿਰਾ;
  Curl_mutex_acquire(&tqueue->ਤਾਲਾ);
  ਜੇ(tqueue->aborted) {
    DEBUGASSERT(0);
    ਨਤੀਜਾ = CURLE_RECV_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  e = Curl_llist_ਸਿਰ(&tqueue->recvq);
  ਜੇ(e) {
    ਬਣਤਰ thrdq_ਵਸਤੂ *qitem = Curl_ਗੰਢ_take_elem(e);
    *pitem = qitem->ਵਸਤੂ;
    qitem->ਵਸਤੂ = ਨਿਰਾ;
    thrdq_ਵਸਤੂ_destroy(qitem);
    ਨਤੀਜਾ = CURLE_OK;
  }
  ਨਹੀਂਤਾਂ
    ਸੰਕੇਤ_2 = thrdq_get_ਸੰਕੇਤ(tqueue);

ਬਾਹਰਲਾ:
  Curl_mutex_release(&tqueue->ਤਾਲਾ);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  ਜੇ(ਸੰਕੇਤ_2)
    (ਖਾਲੀ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)ਸੰਕੇਤ_2);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ thrdq_llist_clean_matches(ਬਣਤਰ Curl_llist *llist,
                                      Curl_thrdq_ਵਸਤੂ_ਮੇਲ_cb *fn_ਮੇਲ,
                                      ਖਾਲੀ *ਮੇਲ_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_llist_ਗੰਢ *e, *n;
  ਬਣਤਰ thrdq_ਵਸਤੂ *qitem;

  ਲਈ(e = Curl_llist_ਸਿਰ(llist); e; e = n) {
    n = Curl_ਗੰਢ_ਅਗਲਾ(e);
    qitem = Curl_ਗੰਢ_elem(e);
    ਜੇ(fn_ਮੇਲ(qitem->ਵਸਤੂ, ਮੇਲ_ਅੰਕੜੇ))
      Curl_ਗੰਢ_remove(e);
  }
}

ਖਾਲੀ Curl_thrdq_clear(ਬਣਤਰ curl_thrdq *tqueue,
                      Curl_thrdq_ਵਸਤੂ_ਮੇਲ_cb *fn_ਮੇਲ,
                      ਖਾਲੀ *ਮੇਲ_ਅੰਕੜੇ)
{
  Curl_mutex_acquire(&tqueue->ਤਾਲਾ);
  ਜੇ(tqueue->aborted) {
    DEBUGASSERT(0);
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_ਮੇਲ, ਮੇਲ_ਅੰਕੜੇ);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_ਮੇਲ, ਮੇਲ_ਅੰਕੜੇ);
ਬਾਹਰਲਾ:
  Curl_mutex_release(&tqueue->ਤਾਲਾ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_ਪੂਰਾ(ਬਣਤਰ curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_ਪੂਰਾ(ਬਣਤਰ curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  ਵਾਪਸ Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#ਜੇ_ਅੰਤ

CURLcode Curl_thrdq_ਸੈੱਟ_props(ਬਣਤਰ curl_thrdq *tqueue,
                              uint32_t min_ਧਾਗੇ,
                              uint32_t max_ਧਾਗੇ,
                              uint32_t idle_ਸਮਾਂ_ms)
{
  CURLcode ਨਤੀਜਾ;
  uint32_t ਸੰਕੇਤ_2;

  Curl_mutex_acquire(&tqueue->ਤਾਲਾ);
  ਸੰਕੇਤ_2 = thrdq_get_ਸੰਕੇਤ(tqueue);
  Curl_mutex_release(&tqueue->ਤਾਲਾ);

  ਨਤੀਜਾ = Curl_thrdpool_ਸੈੱਟ_props(tqueue->tpool, min_ਧਾਗੇ,
                                   max_ਧਾਗੇ, idle_ਸਮਾਂ_ms);
  ਜੇ(!ਨਤੀਜਾ && ਸੰਕੇਤ_2)
    ਨਤੀਜਾ = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)ਸੰਕੇਤ_2);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਖਾਲੀ Curl_thrdq_trace(ਬਣਤਰ curl_thrdq *tqueue,
                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ curl_trc_feat *feat = &Curl_trc_feat_ਧਾਗੇ;
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, feat)) {
    ਬਣਤਰ Curl_llist_ਗੰਢ *e;
    ਬਣਤਰ thrdq_ਵਸਤੂ *qitem;

    Curl_thrdpool_trace(tqueue->tpool, ਅੰਕੜੇ);
    Curl_mutex_acquire(&tqueue->ਤਾਲਾ);
    ਜੇ(!Curl_llist_ਗਿਣਤੀ(&tqueue->sendq) &&
       !Curl_llist_ਗਿਣਤੀ(&tqueue->recvq)) {
      Curl_trc_feat_infof(ਅੰਕੜੇ, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    ਲਈ(e = Curl_llist_ਸਿਰ(&tqueue->sendq); e; e = Curl_ਗੰਢ_ਅਗਲਾ(e)) {
      qitem = Curl_ਗੰਢ_elem(e);
      Curl_trc_feat_infof(ਅੰਕੜੇ, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    ਲਈ(e = Curl_llist_ਸਿਰ(&tqueue->recvq); e; e = Curl_ਗੰਢ_ਅਗਲਾ(e)) {
      qitem = Curl_ਗੰਢ_elem(e);
      Curl_trc_feat_infof(ਅੰਕੜੇ, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->ਤਾਲਾ);
  }
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* USE_THREADS */
