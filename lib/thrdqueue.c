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

#جيڪڏهن_وصف_ٿيل USE_ڌاڳا

#ضمن "llist.h"
#ضمن "curl_threads.h"
#ضمن "thrdpool.h"
#ضمن "thrdqueue.h"
#ضمن "curlx/timeval.h"
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
#ضمن "curl_trc.h"
#ضمن "urldata.h"
#جيڪڏهن_پورو


بنية curl_thrdq {
  ثابت حرف *name;
  curl_mutex_t تالو;
  curl_cond_t await;
  بنية Curl_llist sendq;
  بنية Curl_llist recvq;
  بنية curl_thrdpool *tpool;
  Curl_thrdq_شي_free_cb *fn_free;
  Curl_thrdq_شي_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  فراغ *fn_user_معلومات;
  ٻٽو_انگ(aborted);
};

بنية thrdq_شي {
  بنية Curl_llist_ڳنڍ ڳنڍ;
  Curl_thrdq_شي_free_cb *fn_free;
  Curl_thrdq_شي_process_cb *fn_process;
  فراغ *شي;
  بنية curltime شروعات;
  timediff_t timeout_ms;
  ثابت حرف *description;
};

ساكن بنية thrdq_شي *thrdq_شي_create(بنية curl_thrdq *tqueue,
                                            فراغ *شي,
                                            ثابت حرف *description,
                                            timediff_t timeout_ms)
{
  بنية thrdq_شي *qitem;

  qitem = curlx_calloc(1, حجم(*qitem));
  إذا(!qitem)
    أرجع عدم;
  qitem->شي = شي;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  إذا(timeout_ms) {
    qitem->شروعات = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  أرجع qitem;
}

ساكن فراغ thrdq_شي_destroy(بنية thrdq_شي *qitem)
{
  إذا(qitem->شي)
    qitem->fn_free(qitem->شي);
  curlx_free(qitem);
}

ساكن فراغ thrdq_شي_list_dtor(فراغ *user_معلومات, فراغ *elem)
{
  (فراغ)user_معلومات;
  thrdq_شي_destroy(elem);
}

ساكن فراغ *thrdq_tpool_take(فراغ *user_معلومات, ثابت حرف **pdescription,
                              timediff_t *ptimeout_ms)
{
  بنية curl_thrdq *tqueue = user_معلومات;
  بنية thrdq_شي *qitem = عدم;
  بنية Curl_llist_ڳنڍ *e;
  Curl_thrdq_ev_cb *fn_event = عدم;
  فراغ *fn_user_معلومات = عدم;

  Curl_mutex_acquire(&tqueue->تالو);
  *pdescription = عدم;
  *ptimeout_ms = 0;
  إذا(!tqueue->aborted) {
    e = Curl_llist_سر(&tqueue->sendq);
    إذا(e) {
      بنية curltime now = curlx_now();
      timediff_t timeout_ms;
      طالما(e) {
        qitem = Curl_ڳنڍ_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->شروعات));
        إذا(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->ڳنڍ);
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
  Curl_mutex_release(&tqueue->تالو);
  /* avoiding deadlocks */
  إذا(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_شي_پورو, fn_user_معلومات);
  أرجع qitem;
}

ساكن فراغ thrdq_tpool_return(فراغ *شي, فراغ *user_معلومات)
{
  بنية curl_thrdq *tqueue = user_معلومات;
  بنية thrdq_شي *qitem = شي;
  Curl_thrdq_ev_cb *fn_event = عدم;
  فراغ *fn_user_معلومات = عدم;

  إذا(!tqueue) {
    thrdq_شي_destroy(شي);
    أرجع;
  }

  Curl_mutex_acquire(&tqueue->تالو);
  إذا(tqueue->aborted) {
    thrdq_شي_destroy(qitem);
  }
  وإلا {
    DEBUGASSERT(!Curl_ڳنڍ_llist(&qitem->ڳنڍ));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->ڳنڍ);
    fn_event = tqueue->fn_event;
    fn_user_معلومات = tqueue->fn_user_معلومات;
  }
  Curl_mutex_release(&tqueue->تالو);
  /* avoiding deadlocks */
  إذا(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_شي_پورو, fn_user_معلومات);
}

ساكن فراغ thrdq_tpool_process(فراغ *شي)
{
  بنية thrdq_شي *qitem = شي;
  qitem->fn_process(qitem->شي);
}

ساكن فراغ thrdq_unlink(بنية curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  إذا(tqueue->tpool) {
    إذا(locked)
      Curl_mutex_release(&tqueue->تالو);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = عدم;
    إذا(locked)
      Curl_mutex_acquire(&tqueue->تالو);
  }

  Curl_llist_destroy(&tqueue->sendq, عدم);
  Curl_llist_destroy(&tqueue->recvq, عدم);
  Curl_cond_destroy(&tqueue->await);
  إذا(locked)
    Curl_mutex_release(&tqueue->تالو);
  Curl_mutex_destroy(&tqueue->تالو);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(بنية curl_thrdq **ptqueue,
                           ثابت حرف *name,
                           uint32_t min_ڌاڳا,
                           uint32_t max_ڌاڳا,
                           uint32_t idle_وقت_ms,
                           Curl_thrdq_شي_free_cb *fn_free,
                           Curl_thrdq_شي_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           فراغ *user_معلومات)
{
  بنية curl_thrdq *tqueue;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, حجم(*tqueue));
  إذا(!tqueue)
    اذهب ٻاهريون;

  Curl_mutex_init(&tqueue->تالو);
  Curl_cond_init(&tqueue->await);
  Curl_llist_init(&tqueue->sendq, thrdq_شي_list_dtor);
  Curl_llist_init(&tqueue->recvq, thrdq_شي_list_dtor);
  tqueue->fn_free = fn_free;
  tqueue->fn_process = fn_process;
  tqueue->fn_event = fn_event;
  tqueue->fn_user_معلومات = user_معلومات;

  /* a const string that remains */
  tqueue->name = name;

  نتيجو = Curl_thrdpool_create(&tqueue->tpool, name,
                                min_ڌاڳا, max_ڌاڳا, idle_وقت_ms,
                                thrdq_tpool_take,
                                thrdq_tpool_process,
                                thrdq_tpool_return,
                                tqueue);

ٻاهريون:
  إذا(نتيجو && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = عدم;
  }
  *ptqueue = tqueue;
  أرجع نتيجو;
}

فراغ Curl_thrdq_destroy(بنية curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->تالو);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

ساكن uint32_t thrdq_get_سگنل(بنية curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_ڳڻپ(&tqueue->sendq);
  أرجع (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(بنية curl_thrdq *tqueue, فراغ *شي,
                         ثابت حرف *description, timediff_t timeout_ms)
{
  بنية thrdq_شي *qitem;
  CURLcode نتيجو = CURLE_OK;
  uint32_t سگنل = 0;

  Curl_mutex_acquire(&tqueue->تالو);
  إذا(tqueue->aborted) {
    DEBUGASSERT(0);
    نتيجو = CURLE_SEND_ERROR;
    اذهب ٻاهريون;
  }
  إذا(timeout_ms < 0) {
    نتيجو = CURLE_OPERATION_TIMEDOUT;
    اذهب ٻاهريون;
  }

  qitem = thrdq_شي_create(tqueue, شي, description, timeout_ms);
  إذا(!qitem) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->ڳنڍ);
  سگنل = thrdq_get_سگنل(tqueue);

ٻاهريون:
  Curl_mutex_release(&tqueue->تالو);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  إذا(!نتيجو && سگنل)
    (فراغ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)سگنل);
  أرجع نتيجو;
}

bool Curl_thrdq_check_started(بنية curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->تالو);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_ڳڻپ(&tqueue->sendq);
  Curl_mutex_release(&tqueue->تالو);
  أرجع !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(بنية curl_thrdq *tqueue, فراغ **pitem)
{
  CURLcode نتيجو = CURLE_AGAIN;
  بنية Curl_llist_ڳنڍ *e;
  uint32_t سگنل = 0;

  *pitem = عدم;
  Curl_mutex_acquire(&tqueue->تالو);
  إذا(tqueue->aborted) {
    DEBUGASSERT(0);
    نتيجو = CURLE_RECV_ERROR;
    اذهب ٻاهريون;
  }

  e = Curl_llist_سر(&tqueue->recvq);
  إذا(e) {
    بنية thrdq_شي *qitem = Curl_ڳنڍ_take_elem(e);
    *pitem = qitem->شي;
    qitem->شي = عدم;
    thrdq_شي_destroy(qitem);
    نتيجو = CURLE_OK;
  }
  وإلا
    سگنل = thrdq_get_سگنل(tqueue);

ٻاهريون:
  Curl_mutex_release(&tqueue->تالو);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  إذا(سگنل)
    (فراغ)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)سگنل);
  أرجع نتيجو;
}

ساكن فراغ thrdq_llist_clean_matches(بنية Curl_llist *llist,
                                      Curl_thrdq_شي_ميلاپ_cb *fn_ميلاپ,
                                      فراغ *ميلاپ_معلومات)
{
  بنية Curl_llist_ڳنڍ *e, *n;
  بنية thrdq_شي *qitem;

  لكل(e = Curl_llist_سر(llist); e; e = n) {
    n = Curl_ڳنڍ_ايندڙ(e);
    qitem = Curl_ڳنڍ_elem(e);
    إذا(fn_ميلاپ(qitem->شي, ميلاپ_معلومات))
      Curl_ڳنڍ_remove(e);
  }
}

فراغ Curl_thrdq_clear(بنية curl_thrdq *tqueue,
                      Curl_thrdq_شي_ميلاپ_cb *fn_ميلاپ,
                      فراغ *ميلاپ_معلومات)
{
  Curl_mutex_acquire(&tqueue->تالو);
  إذا(tqueue->aborted) {
    DEBUGASSERT(0);
    اذهب ٻاهريون;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_ميلاپ, ميلاپ_معلومات);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_ميلاپ, ميلاپ_معلومات);
ٻاهريون:
  Curl_mutex_release(&tqueue->تالو);
}

#جيڪڏهن_وصف_ٿيل UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_پورو(بنية curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_پورو(بنية curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  أرجع Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#جيڪڏهن_پورو

CURLcode Curl_thrdq_سيٽ_props(بنية curl_thrdq *tqueue,
                              uint32_t min_ڌاڳا,
                              uint32_t max_ڌاڳا,
                              uint32_t idle_وقت_ms)
{
  CURLcode نتيجو;
  uint32_t سگنل;

  Curl_mutex_acquire(&tqueue->تالو);
  سگنل = thrdq_get_سگنل(tqueue);
  Curl_mutex_release(&tqueue->تالو);

  نتيجو = Curl_thrdpool_سيٽ_props(tqueue->tpool, min_ڌاڳا,
                                   max_ڌاڳا, idle_وقت_ms);
  إذا(!نتيجو && سگنل)
    نتيجو = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)سگنل);
  أرجع نتيجو;
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
فراغ Curl_thrdq_trace(بنية curl_thrdq *tqueue,
                      بنية Curl_easy *معلومات)
{
  بنية curl_trc_feat *feat = &Curl_trc_feat_ڌاڳا;
  إذا(Curl_trc_ft_is_verbose(معلومات, feat)) {
    بنية Curl_llist_ڳنڍ *e;
    بنية thrdq_شي *qitem;

    Curl_thrdpool_trace(tqueue->tpool, معلومات);
    Curl_mutex_acquire(&tqueue->تالو);
    إذا(!Curl_llist_ڳڻپ(&tqueue->sendq) &&
       !Curl_llist_ڳڻپ(&tqueue->recvq)) {
      Curl_trc_feat_infof(معلومات, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    لكل(e = Curl_llist_سر(&tqueue->sendq); e; e = Curl_ڳنڍ_ايندڙ(e)) {
      qitem = Curl_ڳنڍ_elem(e);
      Curl_trc_feat_infof(معلومات, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    لكل(e = Curl_llist_سر(&tqueue->recvq); e; e = Curl_ڳنڍ_ايندڙ(e)) {
      qitem = Curl_ڳنڍ_elem(e);
      Curl_trc_feat_infof(معلومات, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->تالو);
  }
}
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* USE_THREADS */
