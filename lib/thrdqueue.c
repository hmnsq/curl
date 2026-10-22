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

#اگر_تعریف_شدہ USE_دھاگے

#شامل "llist.h"
#شامل "curl_threads.h"
#شامل "thrdpool.h"
#شامل "thrdqueue.h"
#شامل "curlx/timeval.h"
#اگر_تعریف_شدہ CURLVERBOSE
#شامل "curl_trc.h"
#شامل "urldata.h"
#اگر_ختم


ساخت curl_thrdq {
  ثابت حرف *name;
  curl_mutex_t تالا;
  curl_cond_t await;
  ساخت Curl_llist sendq;
  ساخت Curl_llist recvq;
  ساخت curl_thrdpool *tpool;
  Curl_thrdq_شے_free_cb *fn_free;
  Curl_thrdq_شے_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  خالی *fn_user_ڈیٹا;
  ثنائی_ہندسہ(aborted);
};

ساخت thrdq_شے {
  ساخت Curl_llist_گرہ گرہ;
  Curl_thrdq_شے_free_cb *fn_free;
  Curl_thrdq_شے_process_cb *fn_process;
  خالی *شے;
  ساخت curltime آغاز;
  timediff_t timeout_ms;
  ثابت حرف *description;
};

ساکن ساخت thrdq_شے *thrdq_شے_create(ساخت curl_thrdq *tqueue,
                                            خالی *شے,
                                            ثابت حرف *description,
                                            timediff_t timeout_ms)
{
  ساخت thrdq_شے *qitem;

  qitem = curlx_calloc(1, حجم(*qitem));
  اگر(!qitem)
    واپس تہی;
  qitem->شے = شے;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  اگر(timeout_ms) {
    qitem->آغاز = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  واپس qitem;
}

ساکن خالی thrdq_شے_destroy(ساخت thrdq_شے *qitem)
{
  اگر(qitem->شے)
    qitem->fn_free(qitem->شے);
  curlx_free(qitem);
}

ساکن خالی thrdq_شے_list_dtor(خالی *user_ڈیٹا, خالی *elem)
{
  (خالی)user_ڈیٹا;
  thrdq_شے_destroy(elem);
}

ساکن خالی *thrdq_tpool_take(خالی *user_ڈیٹا, ثابت حرف **pdescription,
                              timediff_t *ptimeout_ms)
{
  ساخت curl_thrdq *tqueue = user_ڈیٹا;
  ساخت thrdq_شے *qitem = تہی;
  ساخت Curl_llist_گرہ *e;
  Curl_thrdq_ev_cb *fn_event = تہی;
  خالی *fn_user_ڈیٹا = تہی;

  Curl_mutex_acquire(&tqueue->تالا);
  *pdescription = تہی;
  *ptimeout_ms = 0;
  اگر(!tqueue->aborted) {
    e = Curl_llist_سر(&tqueue->sendq);
    اگر(e) {
      ساخت curltime now = curlx_now();
      timediff_t timeout_ms;
      جب(e) {
        qitem = Curl_گرہ_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->آغاز));
        اگر(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->گرہ);
          fn_event = tqueue->fn_event;
          fn_user_ڈیٹا = tqueue->fn_user_ڈیٹا;
          qitem = تہی;
          e = Curl_llist_سر(&tqueue->sendq);
          جاری;
        }
        ورنہ {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          توڑو;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->تالا);
  /* avoiding deadlocks */
  اگر(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_شے_مکمل, fn_user_ڈیٹا);
  واپس qitem;
}

ساکن خالی thrdq_tpool_return(خالی *شے, خالی *user_ڈیٹا)
{
  ساخت curl_thrdq *tqueue = user_ڈیٹا;
  ساخت thrdq_شے *qitem = شے;
  Curl_thrdq_ev_cb *fn_event = تہی;
  خالی *fn_user_ڈیٹا = تہی;

  اگر(!tqueue) {
    thrdq_شے_destroy(شے);
    واپس;
  }

  Curl_mutex_acquire(&tqueue->تالا);
  اگر(tqueue->aborted) {
    thrdq_شے_destroy(qitem);
  }
  ورنہ {
    DEBUGASSERT(!Curl_گرہ_llist(&qitem->گرہ));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->گرہ);
    fn_event = tqueue->fn_event;
    fn_user_ڈیٹا = tqueue->fn_user_ڈیٹا;
  }
  Curl_mutex_release(&tqueue->تالا);
  /* avoiding deadlocks */
  اگر(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_شے_مکمل, fn_user_ڈیٹا);
}

ساکن خالی thrdq_tpool_process(خالی *شے)
{
  ساخت thrdq_شے *qitem = شے;
  qitem->fn_process(qitem->شے);
}

ساکن خالی thrdq_unlink(ساخت curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  اگر(tqueue->tpool) {
    اگر(locked)
      Curl_mutex_release(&tqueue->تالا);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = تہی;
    اگر(locked)
      Curl_mutex_acquire(&tqueue->تالا);
  }

  Curl_llist_destroy(&tqueue->sendq, تہی);
  Curl_llist_destroy(&tqueue->recvq, تہی);
  Curl_cond_destroy(&tqueue->await);
  اگر(locked)
    Curl_mutex_release(&tqueue->تالا);
  Curl_mutex_destroy(&tqueue->تالا);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(ساخت curl_thrdq **ptqueue,
                           ثابت حرف *name,
                           uint32_t min_دھاگے,
                           uint32_t max_دھاگے,
                           uint32_t idle_وقت_ms,
                           Curl_thrdq_شے_free_cb *fn_free,
                           Curl_thrdq_شے_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           خالی *user_ڈیٹا)
{
  ساخت curl_thrdq *tqueue;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, حجم(*tqueue));
  اگر(!tqueue)
    جاؤ برآمد;

  Curl_mutex_init(&tqueue->تالا);
  Curl_cond_init(&tqueue->await);
  Curl_llist_init(&tqueue->sendq, thrdq_شے_list_dtor);
  Curl_llist_init(&tqueue->recvq, thrdq_شے_list_dtor);
  tqueue->fn_free = fn_free;
  tqueue->fn_process = fn_process;
  tqueue->fn_event = fn_event;
  tqueue->fn_user_ڈیٹا = user_ڈیٹا;

  /* a const string that remains */
  tqueue->name = name;

  نتیجہ = Curl_thrdpool_create(&tqueue->tpool, name,
                                min_دھاگے, max_دھاگے, idle_وقت_ms,
                                thrdq_tpool_take,
                                thrdq_tpool_process,
                                thrdq_tpool_return,
                                tqueue);

برآمد:
  اگر(نتیجہ && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = تہی;
  }
  *ptqueue = tqueue;
  واپس نتیجہ;
}

خالی Curl_thrdq_destroy(ساخت curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->تالا);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

ساکن uint32_t thrdq_get_اشارات(ساخت curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_گنتی(&tqueue->sendq);
  واپس (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(ساخت curl_thrdq *tqueue, خالی *شے,
                         ثابت حرف *description, timediff_t timeout_ms)
{
  ساخت thrdq_شے *qitem;
  CURLcode نتیجہ = CURLE_OK;
  uint32_t اشارات = 0;

  Curl_mutex_acquire(&tqueue->تالا);
  اگر(tqueue->aborted) {
    DEBUGASSERT(0);
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ برآمد;
  }
  اگر(timeout_ms < 0) {
    نتیجہ = CURLE_OPERATION_TIMEDOUT;
    جاؤ برآمد;
  }

  qitem = thrdq_شے_create(tqueue, شے, description, timeout_ms);
  اگر(!qitem) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->گرہ);
  اشارات = thrdq_get_اشارات(tqueue);

برآمد:
  Curl_mutex_release(&tqueue->تالا);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  اگر(!نتیجہ && اشارات)
    (خالی)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)اشارات);
  واپس نتیجہ;
}

bool Curl_thrdq_check_started(ساخت curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->تالا);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_گنتی(&tqueue->sendq);
  Curl_mutex_release(&tqueue->تالا);
  واپس !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(ساخت curl_thrdq *tqueue, خالی **pitem)
{
  CURLcode نتیجہ = CURLE_AGAIN;
  ساخت Curl_llist_گرہ *e;
  uint32_t اشارات = 0;

  *pitem = تہی;
  Curl_mutex_acquire(&tqueue->تالا);
  اگر(tqueue->aborted) {
    DEBUGASSERT(0);
    نتیجہ = CURLE_RECV_ERROR;
    جاؤ برآمد;
  }

  e = Curl_llist_سر(&tqueue->recvq);
  اگر(e) {
    ساخت thrdq_شے *qitem = Curl_گرہ_take_elem(e);
    *pitem = qitem->شے;
    qitem->شے = تہی;
    thrdq_شے_destroy(qitem);
    نتیجہ = CURLE_OK;
  }
  ورنہ
    اشارات = thrdq_get_اشارات(tqueue);

برآمد:
  Curl_mutex_release(&tqueue->تالا);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  اگر(اشارات)
    (خالی)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)اشارات);
  واپس نتیجہ;
}

ساکن خالی thrdq_llist_clean_matches(ساخت Curl_llist *llist,
                                      Curl_thrdq_شے_مطابقت_cb *fn_مطابقت,
                                      خالی *مطابقت_ڈیٹا)
{
  ساخت Curl_llist_گرہ *e, *n;
  ساخت thrdq_شے *qitem;

  برائے(e = Curl_llist_سر(llist); e; e = n) {
    n = Curl_گرہ_اگلا(e);
    qitem = Curl_گرہ_elem(e);
    اگر(fn_مطابقت(qitem->شے, مطابقت_ڈیٹا))
      Curl_گرہ_remove(e);
  }
}

خالی Curl_thrdq_clear(ساخت curl_thrdq *tqueue,
                      Curl_thrdq_شے_مطابقت_cb *fn_مطابقت,
                      خالی *مطابقت_ڈیٹا)
{
  Curl_mutex_acquire(&tqueue->تالا);
  اگر(tqueue->aborted) {
    DEBUGASSERT(0);
    جاؤ برآمد;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_مطابقت, مطابقت_ڈیٹا);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_مطابقت, مطابقت_ڈیٹا);
برآمد:
  Curl_mutex_release(&tqueue->تالا);
}

#اگر_تعریف_شدہ UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_مکمل(ساخت curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_مکمل(ساخت curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  واپس Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#اگر_ختم

CURLcode Curl_thrdq_سیٹ_props(ساخت curl_thrdq *tqueue,
                              uint32_t min_دھاگے,
                              uint32_t max_دھاگے,
                              uint32_t idle_وقت_ms)
{
  CURLcode نتیجہ;
  uint32_t اشارات;

  Curl_mutex_acquire(&tqueue->تالا);
  اشارات = thrdq_get_اشارات(tqueue);
  Curl_mutex_release(&tqueue->تالا);

  نتیجہ = Curl_thrdpool_سیٹ_props(tqueue->tpool, min_دھاگے,
                                   max_دھاگے, idle_وقت_ms);
  اگر(!نتیجہ && اشارات)
    نتیجہ = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)اشارات);
  واپس نتیجہ;
}

#اگر_تعریف_شدہ CURLVERBOSE
خالی Curl_thrdq_trace(ساخت curl_thrdq *tqueue,
                      ساخت Curl_easy *ڈیٹا)
{
  ساخت curl_trc_feat *feat = &Curl_trc_feat_دھاگے;
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, feat)) {
    ساخت Curl_llist_گرہ *e;
    ساخت thrdq_شے *qitem;

    Curl_thrdpool_trace(tqueue->tpool, ڈیٹا);
    Curl_mutex_acquire(&tqueue->تالا);
    اگر(!Curl_llist_گنتی(&tqueue->sendq) &&
       !Curl_llist_گنتی(&tqueue->recvq)) {
      Curl_trc_feat_infof(ڈیٹا, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    برائے(e = Curl_llist_سر(&tqueue->sendq); e; e = Curl_گرہ_اگلا(e)) {
      qitem = Curl_گرہ_elem(e);
      Curl_trc_feat_infof(ڈیٹا, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    برائے(e = Curl_llist_سر(&tqueue->recvq); e; e = Curl_گرہ_اگلا(e)) {
      qitem = Curl_گرہ_elem(e);
      Curl_trc_feat_infof(ڈیٹا, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->تالا);
  }
}
#اگر_ختم

#اگر_ختم /* USE_THREADS */
