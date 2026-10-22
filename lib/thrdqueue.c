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

#اگر_تعریف_شده USE_نخها

#ضمن "llist.h"
#ضمن "curl_threads.h"
#ضمن "thrdpool.h"
#ضمن "thrdqueue.h"
#ضمن "curlx/timeval.h"
#اگر_تعریف_شده CURLVERBOSE
#ضمن "curl_trc.h"
#ضمن "urldata.h"
#پایان_اگر


بنية curl_thrdq {
  ثابت حرف *name;
  curl_mutex_t قفل;
  curl_cond_t await;
  بنية Curl_llist sendq;
  بنية Curl_llist recvq;
  بنية curl_thrdpool *tpool;
  Curl_thrdq_قلم_free_cb *fn_free;
  Curl_thrdq_قلم_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  فراغ *fn_user_داده;
  بیت(aborted);
};

بنية thrdq_قلم {
  بنية Curl_llist_گره گره;
  Curl_thrdq_قلم_free_cb *fn_free;
  Curl_thrdq_قلم_process_cb *fn_process;
  فراغ *قلم;
  بنية curltime شروع;
  timediff_t timeout_ms;
  ثابت حرف *description;
};

ساكن بنية thrdq_قلم *thrdq_قلم_create(بنية curl_thrdq *tqueue,
                                            فراغ *قلم,
                                            ثابت حرف *description,
                                            timediff_t timeout_ms)
{
  بنية thrdq_قلم *qitem;

  qitem = curlx_calloc(1, حجم(*qitem));
  إذا(!qitem)
    أرجع عدم;
  qitem->قلم = قلم;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  إذا(timeout_ms) {
    qitem->شروع = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  أرجع qitem;
}

ساكن فراغ thrdq_قلم_destroy(بنية thrdq_قلم *qitem)
{
  إذا(qitem->قلم)
    qitem->fn_free(qitem->قلم);
  curlx_free(qitem);
}

ساكن فراغ thrdq_قلم_list_dtor(فراغ *user_داده, فراغ *elem)
{
  (فراغ)user_داده;
  thrdq_قلم_destroy(elem);
}

ساكن فراغ *thrdq_tpool_take(فراغ *user_داده, ثابت حرف **pdescription,
                              timediff_t *ptimeout_ms)
{
  بنية curl_thrdq *tqueue = user_داده;
  بنية thrdq_قلم *qitem = عدم;
  بنية Curl_llist_گره *e;
  Curl_thrdq_ev_cb *fn_event = عدم;
  فراغ *fn_user_داده = عدم;

  Curl_mutex_acquire(&tqueue->قفل);
  *pdescription = عدم;
  *ptimeout_ms = 0;
  إذا(!tqueue->aborted) {
    e = Curl_llist_سر(&tqueue->sendq);
    إذا(e) {
      بنية curltime now = curlx_now();
      timediff_t timeout_ms;
      طالما(e) {
        qitem = Curl_گره_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->شروع));
        إذا(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->گره);
          fn_event = tqueue->fn_event;
          fn_user_داده = tqueue->fn_user_داده;
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
  Curl_mutex_release(&tqueue->قفل);
  /* avoiding deadlocks */
  إذا(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_قلم_انجامشده, fn_user_داده);
  أرجع qitem;
}

ساكن فراغ thrdq_tpool_return(فراغ *قلم, فراغ *user_داده)
{
  بنية curl_thrdq *tqueue = user_داده;
  بنية thrdq_قلم *qitem = قلم;
  Curl_thrdq_ev_cb *fn_event = عدم;
  فراغ *fn_user_داده = عدم;

  إذا(!tqueue) {
    thrdq_قلم_destroy(قلم);
    أرجع;
  }

  Curl_mutex_acquire(&tqueue->قفل);
  إذا(tqueue->aborted) {
    thrdq_قلم_destroy(qitem);
  }
  وإلا {
    DEBUGASSERT(!Curl_گره_llist(&qitem->گره));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->گره);
    fn_event = tqueue->fn_event;
    fn_user_داده = tqueue->fn_user_داده;
  }
  Curl_mutex_release(&tqueue->قفل);
  /* avoiding deadlocks */
  إذا(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_قلم_انجامشده, fn_user_داده);
}

ساكن فراغ thrdq_tpool_process(فراغ *قلم)
{
  بنية thrdq_قلم *qitem = قلم;
  qitem->fn_process(qitem->قلم);
}

ساكن فراغ thrdq_unlink(بنية curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  إذا(tqueue->tpool) {
    إذا(locked)
      Curl_mutex_release(&tqueue->قفل);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = عدم;
    إذا(locked)
      Curl_mutex_acquire(&tqueue->قفل);
  }

  Curl_llist_destroy(&tqueue->sendq, عدم);
  Curl_llist_destroy(&tqueue->recvq, عدم);
  Curl_cond_destroy(&tqueue->await);
  إذا(locked)
    Curl_mutex_release(&tqueue->قفل);
  Curl_mutex_destroy(&tqueue->قفل);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(بنية curl_thrdq **ptqueue,
                           ثابت حرف *name,
                           uint32_t min_نخها,
                           uint32_t max_نخها,
                           uint32_t idle_زمان_ms,
                           Curl_thrdq_قلم_free_cb *fn_free,
                           Curl_thrdq_قلم_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           فراغ *user_داده)
{
  بنية curl_thrdq *tqueue;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, حجم(*tqueue));
  إذا(!tqueue)
    اذهب بیرون;

  Curl_mutex_init(&tqueue->قفل);
  Curl_cond_init(&tqueue->await);
  Curl_llist_init(&tqueue->sendq, thrdq_قلم_list_dtor);
  Curl_llist_init(&tqueue->recvq, thrdq_قلم_list_dtor);
  tqueue->fn_free = fn_free;
  tqueue->fn_process = fn_process;
  tqueue->fn_event = fn_event;
  tqueue->fn_user_داده = user_داده;

  /* a const string that remains */
  tqueue->name = name;

  نتیجه = Curl_thrdpool_create(&tqueue->tpool, name,
                                min_نخها, max_نخها, idle_زمان_ms,
                                thrdq_tpool_take,
                                thrdq_tpool_process,
                                thrdq_tpool_return,
                                tqueue);

بیرون:
  إذا(نتیجه && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = عدم;
  }
  *ptqueue = tqueue;
  أرجع نتیجه;
}

فراغ Curl_thrdq_destroy(بنية curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->قفل);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

ساكن uint32_t thrdq_get_سیگنالها(بنية curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_شمار(&tqueue->sendq);
  أرجع (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(بنية curl_thrdq *tqueue, فراغ *قلم,
                         ثابت حرف *description, timediff_t timeout_ms)
{
  بنية thrdq_قلم *qitem;
  CURLcode نتیجه = CURLE_OK;
  uint32_t سیگنالها = 0;

  Curl_mutex_acquire(&tqueue->قفل);
  إذا(tqueue->aborted) {
    DEBUGASSERT(0);
    نتیجه = CURLE_SEND_ERROR;
    اذهب بیرون;
  }
  إذا(timeout_ms < 0) {
    نتیجه = CURLE_OPERATION_TIMEDOUT;
    اذهب بیرون;
  }

  qitem = thrdq_قلم_create(tqueue, قلم, description, timeout_ms);
  إذا(!qitem) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->گره);
  سیگنالها = thrdq_get_سیگنالها(tqueue);

بیرون:
  Curl_mutex_release(&tqueue->قفل);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  إذا(!نتیجه && سیگنالها)
    (فراغ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)سیگنالها);
  أرجع نتیجه;
}

bool Curl_thrdq_check_started(بنية curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->قفل);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_شمار(&tqueue->sendq);
  Curl_mutex_release(&tqueue->قفل);
  أرجع !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(بنية curl_thrdq *tqueue, فراغ **pitem)
{
  CURLcode نتیجه = CURLE_AGAIN;
  بنية Curl_llist_گره *e;
  uint32_t سیگنالها = 0;

  *pitem = عدم;
  Curl_mutex_acquire(&tqueue->قفل);
  إذا(tqueue->aborted) {
    DEBUGASSERT(0);
    نتیجه = CURLE_RECV_ERROR;
    اذهب بیرون;
  }

  e = Curl_llist_سر(&tqueue->recvq);
  إذا(e) {
    بنية thrdq_قلم *qitem = Curl_گره_take_elem(e);
    *pitem = qitem->قلم;
    qitem->قلم = عدم;
    thrdq_قلم_destroy(qitem);
    نتیجه = CURLE_OK;
  }
  وإلا
    سیگنالها = thrdq_get_سیگنالها(tqueue);

بیرون:
  Curl_mutex_release(&tqueue->قفل);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  إذا(سیگنالها)
    (فراغ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)سیگنالها);
  أرجع نتیجه;
}

ساكن فراغ thrdq_llist_clean_matches(بنية Curl_llist *llist,
                                      Curl_thrdq_قلم_تطابق_cb *fn_تطابق,
                                      فراغ *تطابق_داده)
{
  بنية Curl_llist_گره *e, *n;
  بنية thrdq_قلم *qitem;

  لكل(e = Curl_llist_سر(llist); e; e = n) {
    n = Curl_گره_بعدی(e);
    qitem = Curl_گره_elem(e);
    إذا(fn_تطابق(qitem->قلم, تطابق_داده))
      Curl_گره_remove(e);
  }
}

فراغ Curl_thrdq_clear(بنية curl_thrdq *tqueue,
                      Curl_thrdq_قلم_تطابق_cb *fn_تطابق,
                      فراغ *تطابق_داده)
{
  Curl_mutex_acquire(&tqueue->قفل);
  إذا(tqueue->aborted) {
    DEBUGASSERT(0);
    اذهب بیرون;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_تطابق, تطابق_داده);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_تطابق, تطابق_داده);
بیرون:
  Curl_mutex_release(&tqueue->قفل);
}

#اگر_تعریف_شده UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_انجامشده(بنية curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_انجامشده(بنية curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  أرجع Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#پایان_اگر

CURLcode Curl_thrdq_مجموعه_props(بنية curl_thrdq *tqueue,
                              uint32_t min_نخها,
                              uint32_t max_نخها,
                              uint32_t idle_زمان_ms)
{
  CURLcode نتیجه;
  uint32_t سیگنالها;

  Curl_mutex_acquire(&tqueue->قفل);
  سیگنالها = thrdq_get_سیگنالها(tqueue);
  Curl_mutex_release(&tqueue->قفل);

  نتیجه = Curl_thrdpool_مجموعه_props(tqueue->tpool, min_نخها,
                                   max_نخها, idle_زمان_ms);
  إذا(!نتیجه && سیگنالها)
    نتیجه = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)سیگنالها);
  أرجع نتیجه;
}

#اگر_تعریف_شده CURLVERBOSE
فراغ Curl_thrdq_trace(بنية curl_thrdq *tqueue,
                      بنية Curl_easy *داده)
{
  بنية curl_trc_feat *feat = &Curl_trc_feat_نخها;
  إذا(Curl_trc_ft_is_verbose(داده, feat)) {
    بنية Curl_llist_گره *e;
    بنية thrdq_قلم *qitem;

    Curl_thrdpool_trace(tqueue->tpool, داده);
    Curl_mutex_acquire(&tqueue->قفل);
    إذا(!Curl_llist_شمار(&tqueue->sendq) &&
       !Curl_llist_شمار(&tqueue->recvq)) {
      Curl_trc_feat_infof(داده, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    لكل(e = Curl_llist_سر(&tqueue->sendq); e; e = Curl_گره_بعدی(e)) {
      qitem = Curl_گره_elem(e);
      Curl_trc_feat_infof(داده, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    لكل(e = Curl_llist_سر(&tqueue->recvq); e; e = Curl_گره_بعدی(e)) {
      qitem = Curl_گره_elem(e);
      Curl_trc_feat_infof(داده, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->قفل);
  }
}
#پایان_اگر

#پایان_اگر /* USE_THREADS */
