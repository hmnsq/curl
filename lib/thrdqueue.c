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

#جے_تعریف_شدہ USE_دھاگے

#ضمن "llist.h"
#ضمن "curl_threads.h"
#ضمن "thrdpool.h"
#ضمن "thrdqueue.h"
#ضمن "curlx/timeval.h"
#جے_تعریف_شدہ CURLVERBOSE
#ضمن "curl_trc.h"
#ضمن "urldata.h"
#جے_انت


بنية curl_thrdq {
  ثابت حرف *name;
  curl_mutex_t جندرا;
  curl_cond_t await;
  بنية Curl_llist sendq;
  بنية Curl_llist recvq;
  بنية curl_thrdpool *tpool;
  Curl_thrdq_شے_free_cb *fn_free;
  Curl_thrdq_شے_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  فراغ *fn_user_انکڑے;
  ثنائی_ہندسہ(aborted);
};

بنية thrdq_شے {
  بنية Curl_llist_گرہ گرہ;
  Curl_thrdq_شے_free_cb *fn_free;
  Curl_thrdq_شے_process_cb *fn_process;
  فراغ *شے;
  بنية curltime شروع;
  timediff_t timeout_ms;
  ثابت حرف *description;
};

ساكن بنية thrdq_شے *thrdq_شے_create(بنية curl_thrdq *tqueue,
                                            فراغ *شے,
                                            ثابت حرف *description,
                                            timediff_t timeout_ms)
{
  بنية thrdq_شے *qitem;

  qitem = curlx_calloc(1, حجم(*qitem));
  إذا(!qitem)
    أرجع عدم;
  qitem->شے = شے;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  إذا(timeout_ms) {
    qitem->شروع = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  أرجع qitem;
}

ساكن فراغ thrdq_شے_destroy(بنية thrdq_شے *qitem)
{
  إذا(qitem->شے)
    qitem->fn_free(qitem->شے);
  curlx_free(qitem);
}

ساكن فراغ thrdq_شے_list_dtor(فراغ *user_انکڑے, فراغ *elem)
{
  (فراغ)user_انکڑے;
  thrdq_شے_destroy(elem);
}

ساكن فراغ *thrdq_tpool_take(فراغ *user_انکڑے, ثابت حرف **pdescription,
                              timediff_t *ptimeout_ms)
{
  بنية curl_thrdq *tqueue = user_انکڑے;
  بنية thrdq_شے *qitem = عدم;
  بنية Curl_llist_گرہ *e;
  Curl_thrdq_ev_cb *fn_event = عدم;
  فراغ *fn_user_انکڑے = عدم;

  Curl_mutex_acquire(&tqueue->جندرا);
  *pdescription = عدم;
  *ptimeout_ms = 0;
  إذا(!tqueue->aborted) {
    e = Curl_llist_سر(&tqueue->sendq);
    إذا(e) {
      بنية curltime now = curlx_now();
      timediff_t timeout_ms;
      طالما(e) {
        qitem = Curl_گرہ_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->شروع));
        إذا(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->گرہ);
          fn_event = tqueue->fn_event;
          fn_user_انکڑے = tqueue->fn_user_انکڑے;
          qitem = عدم;
          e = Curl_llist_سر(&tqueue->sendq);
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
  Curl_mutex_release(&tqueue->جندرا);
  /* avoiding deadlocks */
  إذا(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_شے_پورا, fn_user_انکڑے);
  أرجع qitem;
}

ساكن فراغ thrdq_tpool_return(فراغ *شے, فراغ *user_انکڑے)
{
  بنية curl_thrdq *tqueue = user_انکڑے;
  بنية thrdq_شے *qitem = شے;
  Curl_thrdq_ev_cb *fn_event = عدم;
  فراغ *fn_user_انکڑے = عدم;

  إذا(!tqueue) {
    thrdq_شے_destroy(شے);
    أرجع;
  }

  Curl_mutex_acquire(&tqueue->جندرا);
  إذا(tqueue->aborted) {
    thrdq_شے_destroy(qitem);
  }
  وإلا {
    DEBUGASSERT(!Curl_گرہ_llist(&qitem->گرہ));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->گرہ);
    fn_event = tqueue->fn_event;
    fn_user_انکڑے = tqueue->fn_user_انکڑے;
  }
  Curl_mutex_release(&tqueue->جندرا);
  /* avoiding deadlocks */
  إذا(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_شے_پورا, fn_user_انکڑے);
}

ساكن فراغ thrdq_tpool_process(فراغ *شے)
{
  بنية thrdq_شے *qitem = شے;
  qitem->fn_process(qitem->شے);
}

ساكن فراغ thrdq_unlink(بنية curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  إذا(tqueue->tpool) {
    إذا(locked)
      Curl_mutex_release(&tqueue->جندرا);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = عدم;
    إذا(locked)
      Curl_mutex_acquire(&tqueue->جندرا);
  }

  Curl_llist_destroy(&tqueue->sendq, عدم);
  Curl_llist_destroy(&tqueue->recvq, عدم);
  Curl_cond_destroy(&tqueue->await);
  إذا(locked)
    Curl_mutex_release(&tqueue->جندرا);
  Curl_mutex_destroy(&tqueue->جندرا);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(بنية curl_thrdq **ptqueue,
                           ثابت حرف *name,
                           uint32_t min_دھاگے,
                           uint32_t max_دھاگے,
                           uint32_t idle_ویلا_ms,
                           Curl_thrdq_شے_free_cb *fn_free,
                           Curl_thrdq_شے_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           فراغ *user_انکڑے)
{
  بنية curl_thrdq *tqueue;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, حجم(*tqueue));
  إذا(!tqueue)
    اذهب باہرلا;

  Curl_mutex_init(&tqueue->جندرا);
  Curl_cond_init(&tqueue->await);
  Curl_llist_init(&tqueue->sendq, thrdq_شے_list_dtor);
  Curl_llist_init(&tqueue->recvq, thrdq_شے_list_dtor);
  tqueue->fn_free = fn_free;
  tqueue->fn_process = fn_process;
  tqueue->fn_event = fn_event;
  tqueue->fn_user_انکڑے = user_انکڑے;

  /* a const string that remains */
  tqueue->name = name;

  نتیجہ = Curl_thrdpool_create(&tqueue->tpool, name,
                                min_دھاگے, max_دھاگے, idle_ویلا_ms,
                                thrdq_tpool_take,
                                thrdq_tpool_process,
                                thrdq_tpool_return,
                                tqueue);

باہرلا:
  إذا(نتیجہ && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = عدم;
  }
  *ptqueue = tqueue;
  أرجع نتیجہ;
}

فراغ Curl_thrdq_destroy(بنية curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->جندرا);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

ساكن uint32_t thrdq_get_اشارے(بنية curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_گنتی(&tqueue->sendq);
  أرجع (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(بنية curl_thrdq *tqueue, فراغ *شے,
                         ثابت حرف *description, timediff_t timeout_ms)
{
  بنية thrdq_شے *qitem;
  CURLcode نتیجہ = CURLE_OK;
  uint32_t اشارے = 0;

  Curl_mutex_acquire(&tqueue->جندرا);
  إذا(tqueue->aborted) {
    DEBUGASSERT(0);
    نتیجہ = CURLE_SEND_ERROR;
    اذهب باہرلا;
  }
  إذا(timeout_ms < 0) {
    نتیجہ = CURLE_OPERATION_TIMEDOUT;
    اذهب باہرلا;
  }

  qitem = thrdq_شے_create(tqueue, شے, description, timeout_ms);
  إذا(!qitem) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->گرہ);
  اشارے = thrdq_get_اشارے(tqueue);

باہرلا:
  Curl_mutex_release(&tqueue->جندرا);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  إذا(!نتیجہ && اشارے)
    (فراغ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)اشارے);
  أرجع نتیجہ;
}

bool Curl_thrdq_check_started(بنية curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->جندرا);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_گنتی(&tqueue->sendq);
  Curl_mutex_release(&tqueue->جندرا);
  أرجع !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(بنية curl_thrdq *tqueue, فراغ **pitem)
{
  CURLcode نتیجہ = CURLE_AGAIN;
  بنية Curl_llist_گرہ *e;
  uint32_t اشارے = 0;

  *pitem = عدم;
  Curl_mutex_acquire(&tqueue->جندرا);
  إذا(tqueue->aborted) {
    DEBUGASSERT(0);
    نتیجہ = CURLE_RECV_ERROR;
    اذهب باہرلا;
  }

  e = Curl_llist_سر(&tqueue->recvq);
  إذا(e) {
    بنية thrdq_شے *qitem = Curl_گرہ_take_elem(e);
    *pitem = qitem->شے;
    qitem->شے = عدم;
    thrdq_شے_destroy(qitem);
    نتیجہ = CURLE_OK;
  }
  وإلا
    اشارے = thrdq_get_اشارے(tqueue);

باہرلا:
  Curl_mutex_release(&tqueue->جندرا);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  إذا(اشارے)
    (فراغ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)اشارے);
  أرجع نتیجہ;
}

ساكن فراغ thrdq_llist_clean_matches(بنية Curl_llist *llist,
                                      Curl_thrdq_شے_میل_cb *fn_میل,
                                      فراغ *میل_انکڑے)
{
  بنية Curl_llist_گرہ *e, *n;
  بنية thrdq_شے *qitem;

  لكل(e = Curl_llist_سر(llist); e; e = n) {
    n = Curl_گرہ_اگلا(e);
    qitem = Curl_گرہ_elem(e);
    إذا(fn_میل(qitem->شے, میل_انکڑے))
      Curl_گرہ_remove(e);
  }
}

فراغ Curl_thrdq_clear(بنية curl_thrdq *tqueue,
                      Curl_thrdq_شے_میل_cb *fn_میل,
                      فراغ *میل_انکڑے)
{
  Curl_mutex_acquire(&tqueue->جندرا);
  إذا(tqueue->aborted) {
    DEBUGASSERT(0);
    اذهب باہرلا;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_میل, میل_انکڑے);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_میل, میل_انکڑے);
باہرلا:
  Curl_mutex_release(&tqueue->جندرا);
}

#جے_تعریف_شدہ UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_پورا(بنية curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_پورا(بنية curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  أرجع Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#جے_انت

CURLcode Curl_thrdq_سیٹ_props(بنية curl_thrdq *tqueue,
                              uint32_t min_دھاگے,
                              uint32_t max_دھاگے,
                              uint32_t idle_ویلا_ms)
{
  CURLcode نتیجہ;
  uint32_t اشارے;

  Curl_mutex_acquire(&tqueue->جندرا);
  اشارے = thrdq_get_اشارے(tqueue);
  Curl_mutex_release(&tqueue->جندرا);

  نتیجہ = Curl_thrdpool_سیٹ_props(tqueue->tpool, min_دھاگے,
                                   max_دھاگے, idle_ویلا_ms);
  إذا(!نتیجہ && اشارے)
    نتیجہ = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)اشارے);
  أرجع نتیجہ;
}

#جے_تعریف_شدہ CURLVERBOSE
فراغ Curl_thrdq_trace(بنية curl_thrdq *tqueue,
                      بنية Curl_easy *انکڑے)
{
  بنية curl_trc_feat *feat = &Curl_trc_feat_دھاگے;
  إذا(Curl_trc_ft_is_verbose(انکڑے, feat)) {
    بنية Curl_llist_گرہ *e;
    بنية thrdq_شے *qitem;

    Curl_thrdpool_trace(tqueue->tpool, انکڑے);
    Curl_mutex_acquire(&tqueue->جندرا);
    إذا(!Curl_llist_گنتی(&tqueue->sendq) &&
       !Curl_llist_گنتی(&tqueue->recvq)) {
      Curl_trc_feat_infof(انکڑے, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    لكل(e = Curl_llist_سر(&tqueue->sendq); e; e = Curl_گرہ_اگلا(e)) {
      qitem = Curl_گرہ_elem(e);
      Curl_trc_feat_infof(انکڑے, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    لكل(e = Curl_llist_سر(&tqueue->recvq); e; e = Curl_گرہ_اگلا(e)) {
      qitem = Curl_گرہ_elem(e);
      Curl_trc_feat_infof(انکڑے, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->جندرا);
  }
}
#جے_انت

#جے_انت /* USE_THREADS */
