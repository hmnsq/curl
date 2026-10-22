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

#که_تعریف_شوی USE_تارونه

#ضمن "llist.h"
#ضمن "curl_threads.h"
#ضمن "thrdpool.h"
#ضمن "thrdqueue.h"
#ضمن "curlx/timeval.h"
#که_تعریف_شوی CURLVERBOSE
#ضمن "curl_trc.h"
#ضمن "urldata.h"
#د_که_پای


بنية curl_thrdq {
  ثابت حرف *name;
  curl_mutex_t قلف;
  curl_cond_t await;
  بنية Curl_llist sendq;
  بنية Curl_llist recvq;
  بنية curl_thrdpool *tpool;
  Curl_thrdq_توکی_free_cb *fn_free;
  Curl_thrdq_توکی_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  فراغ *fn_user_معلومات;
  بټ(aborted);
};

بنية thrdq_توکی {
  بنية Curl_llist_غوټه غوټه;
  Curl_thrdq_توکی_free_cb *fn_free;
  Curl_thrdq_توکی_process_cb *fn_process;
  فراغ *توکی;
  بنية curltime پیل;
  timediff_t timeout_ms;
  ثابت حرف *description;
};

ساكن بنية thrdq_توکی *thrdq_توکی_create(بنية curl_thrdq *tqueue,
                                            فراغ *توکی,
                                            ثابت حرف *description,
                                            timediff_t timeout_ms)
{
  بنية thrdq_توکی *qitem;

  qitem = curlx_calloc(1, حجم(*qitem));
  إذا(!qitem)
    أرجع عدم;
  qitem->توکی = توکی;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  إذا(timeout_ms) {
    qitem->پیل = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  أرجع qitem;
}

ساكن فراغ thrdq_توکی_destroy(بنية thrdq_توکی *qitem)
{
  إذا(qitem->توکی)
    qitem->fn_free(qitem->توکی);
  curlx_free(qitem);
}

ساكن فراغ thrdq_توکی_list_dtor(فراغ *user_معلومات, فراغ *elem)
{
  (فراغ)user_معلومات;
  thrdq_توکی_destroy(elem);
}

ساكن فراغ *thrdq_tpool_take(فراغ *user_معلومات, ثابت حرف **pdescription,
                              timediff_t *ptimeout_ms)
{
  بنية curl_thrdq *tqueue = user_معلومات;
  بنية thrdq_توکی *qitem = عدم;
  بنية Curl_llist_غوټه *e;
  Curl_thrdq_ev_cb *fn_event = عدم;
  فراغ *fn_user_معلومات = عدم;

  Curl_mutex_acquire(&tqueue->قلف);
  *pdescription = عدم;
  *ptimeout_ms = 0;
  إذا(!tqueue->aborted) {
    e = Curl_llist_سر(&tqueue->sendq);
    إذا(e) {
      بنية curltime now = curlx_now();
      timediff_t timeout_ms;
      طالما(e) {
        qitem = Curl_غوټه_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->پیل));
        إذا(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->غوټه);
          fn_event = tqueue->fn_event;
          fn_user_معلومات = tqueue->fn_user_معلومات;
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
  Curl_mutex_release(&tqueue->قلف);
  /* avoiding deadlocks */
  إذا(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_توکی_بشپړ, fn_user_معلومات);
  أرجع qitem;
}

ساكن فراغ thrdq_tpool_return(فراغ *توکی, فراغ *user_معلومات)
{
  بنية curl_thrdq *tqueue = user_معلومات;
  بنية thrdq_توکی *qitem = توکی;
  Curl_thrdq_ev_cb *fn_event = عدم;
  فراغ *fn_user_معلومات = عدم;

  إذا(!tqueue) {
    thrdq_توکی_destroy(توکی);
    أرجع;
  }

  Curl_mutex_acquire(&tqueue->قلف);
  إذا(tqueue->aborted) {
    thrdq_توکی_destroy(qitem);
  }
  وإلا {
    DEBUGASSERT(!Curl_غوټه_llist(&qitem->غوټه));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->غوټه);
    fn_event = tqueue->fn_event;
    fn_user_معلومات = tqueue->fn_user_معلومات;
  }
  Curl_mutex_release(&tqueue->قلف);
  /* avoiding deadlocks */
  إذا(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_توکی_بشپړ, fn_user_معلومات);
}

ساكن فراغ thrdq_tpool_process(فراغ *توکی)
{
  بنية thrdq_توکی *qitem = توکی;
  qitem->fn_process(qitem->توکی);
}

ساكن فراغ thrdq_unlink(بنية curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  إذا(tqueue->tpool) {
    إذا(locked)
      Curl_mutex_release(&tqueue->قلف);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = عدم;
    إذا(locked)
      Curl_mutex_acquire(&tqueue->قلف);
  }

  Curl_llist_destroy(&tqueue->sendq, عدم);
  Curl_llist_destroy(&tqueue->recvq, عدم);
  Curl_cond_destroy(&tqueue->await);
  إذا(locked)
    Curl_mutex_release(&tqueue->قلف);
  Curl_mutex_destroy(&tqueue->قلف);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(بنية curl_thrdq **ptqueue,
                           ثابت حرف *name,
                           uint32_t min_تارونه,
                           uint32_t max_تارونه,
                           uint32_t idle_وخت_ms,
                           Curl_thrdq_توکی_free_cb *fn_free,
                           Curl_thrdq_توکی_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           فراغ *user_معلومات)
{
  بنية curl_thrdq *tqueue;
  CURLcode پایله = CURLE_بهر_د_حافظه;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, حجم(*tqueue));
  إذا(!tqueue)
    اذهب بهر;

  Curl_mutex_init(&tqueue->قلف);
  Curl_cond_init(&tqueue->await);
  Curl_llist_init(&tqueue->sendq, thrdq_توکی_list_dtor);
  Curl_llist_init(&tqueue->recvq, thrdq_توکی_list_dtor);
  tqueue->fn_free = fn_free;
  tqueue->fn_process = fn_process;
  tqueue->fn_event = fn_event;
  tqueue->fn_user_معلومات = user_معلومات;

  /* a const string that remains */
  tqueue->name = name;

  پایله = Curl_thrdpool_create(&tqueue->tpool, name,
                                min_تارونه, max_تارونه, idle_وخت_ms,
                                thrdq_tpool_take,
                                thrdq_tpool_process,
                                thrdq_tpool_return,
                                tqueue);

بهر:
  إذا(پایله && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = عدم;
  }
  *ptqueue = tqueue;
  أرجع پایله;
}

فراغ Curl_thrdq_destroy(بنية curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->قلف);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

ساكن uint32_t thrdq_get_سیګنالونه(بنية curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_شمېر(&tqueue->sendq);
  أرجع (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(بنية curl_thrdq *tqueue, فراغ *توکی,
                         ثابت حرف *description, timediff_t timeout_ms)
{
  بنية thrdq_توکی *qitem;
  CURLcode پایله = CURLE_OK;
  uint32_t سیګنالونه = 0;

  Curl_mutex_acquire(&tqueue->قلف);
  إذا(tqueue->aborted) {
    DEBUGASSERT(0);
    پایله = CURLE_SEND_ERROR;
    اذهب بهر;
  }
  إذا(timeout_ms < 0) {
    پایله = CURLE_OPERATION_TIMEDOUT;
    اذهب بهر;
  }

  qitem = thrdq_توکی_create(tqueue, توکی, description, timeout_ms);
  إذا(!qitem) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->غوټه);
  سیګنالونه = thrdq_get_سیګنالونه(tqueue);

بهر:
  Curl_mutex_release(&tqueue->قلف);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  إذا(!پایله && سیګنالونه)
    (فراغ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)سیګنالونه);
  أرجع پایله;
}

bool Curl_thrdq_check_started(بنية curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->قلف);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_شمېر(&tqueue->sendq);
  Curl_mutex_release(&tqueue->قلف);
  أرجع !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(بنية curl_thrdq *tqueue, فراغ **pitem)
{
  CURLcode پایله = CURLE_AGAIN;
  بنية Curl_llist_غوټه *e;
  uint32_t سیګنالونه = 0;

  *pitem = عدم;
  Curl_mutex_acquire(&tqueue->قلف);
  إذا(tqueue->aborted) {
    DEBUGASSERT(0);
    پایله = CURLE_RECV_ERROR;
    اذهب بهر;
  }

  e = Curl_llist_سر(&tqueue->recvq);
  إذا(e) {
    بنية thrdq_توکی *qitem = Curl_غوټه_take_elem(e);
    *pitem = qitem->توکی;
    qitem->توکی = عدم;
    thrdq_توکی_destroy(qitem);
    پایله = CURLE_OK;
  }
  وإلا
    سیګنالونه = thrdq_get_سیګنالونه(tqueue);

بهر:
  Curl_mutex_release(&tqueue->قلف);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  إذا(سیګنالونه)
    (فراغ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)سیګنالونه);
  أرجع پایله;
}

ساكن فراغ thrdq_llist_clean_matches(بنية Curl_llist *llist,
                                      Curl_thrdq_توکی_سمون_cb *fn_سمون,
                                      فراغ *سمون_معلومات)
{
  بنية Curl_llist_غوټه *e, *n;
  بنية thrdq_توکی *qitem;

  لكل(e = Curl_llist_سر(llist); e; e = n) {
    n = Curl_غوټه_بل(e);
    qitem = Curl_غوټه_elem(e);
    إذا(fn_سمون(qitem->توکی, سمون_معلومات))
      Curl_غوټه_remove(e);
  }
}

فراغ Curl_thrdq_clear(بنية curl_thrdq *tqueue,
                      Curl_thrdq_توکی_سمون_cb *fn_سمون,
                      فراغ *سمون_معلومات)
{
  Curl_mutex_acquire(&tqueue->قلف);
  إذا(tqueue->aborted) {
    DEBUGASSERT(0);
    اذهب بهر;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_سمون, سمون_معلومات);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_سمون, سمون_معلومات);
بهر:
  Curl_mutex_release(&tqueue->قلف);
}

#که_تعریف_شوی UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_بشپړ(بنية curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_بشپړ(بنية curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  أرجع Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#د_که_پای

CURLcode Curl_thrdq_سېټ_props(بنية curl_thrdq *tqueue,
                              uint32_t min_تارونه,
                              uint32_t max_تارونه,
                              uint32_t idle_وخت_ms)
{
  CURLcode پایله;
  uint32_t سیګنالونه;

  Curl_mutex_acquire(&tqueue->قلف);
  سیګنالونه = thrdq_get_سیګنالونه(tqueue);
  Curl_mutex_release(&tqueue->قلف);

  پایله = Curl_thrdpool_سېټ_props(tqueue->tpool, min_تارونه,
                                   max_تارونه, idle_وخت_ms);
  إذا(!پایله && سیګنالونه)
    پایله = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)سیګنالونه);
  أرجع پایله;
}

#که_تعریف_شوی CURLVERBOSE
فراغ Curl_thrdq_trace(بنية curl_thrdq *tqueue,
                      بنية Curl_easy *معلومات)
{
  بنية curl_trc_feat *feat = &Curl_trc_feat_تارونه;
  إذا(Curl_trc_ft_is_verbose(معلومات, feat)) {
    بنية Curl_llist_غوټه *e;
    بنية thrdq_توکی *qitem;

    Curl_thrdpool_trace(tqueue->tpool, معلومات);
    Curl_mutex_acquire(&tqueue->قلف);
    إذا(!Curl_llist_شمېر(&tqueue->sendq) &&
       !Curl_llist_شمېر(&tqueue->recvq)) {
      Curl_trc_feat_infof(معلومات, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    لكل(e = Curl_llist_سر(&tqueue->sendq); e; e = Curl_غوټه_بل(e)) {
      qitem = Curl_غوټه_elem(e);
      Curl_trc_feat_infof(معلومات, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    لكل(e = Curl_llist_سر(&tqueue->recvq); e; e = Curl_غوټه_بل(e)) {
      qitem = Curl_غوټه_elem(e);
      Curl_trc_feat_infof(معلومات, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->قلف);
  }
}
#د_که_پای

#د_که_پای /* USE_THREADS */
