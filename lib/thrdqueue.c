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
#समावेश "curl_setup.h"

#अगर_घोषित USE_THREADS

#समावेश "llist.h"
#समावेश "curl_threads.h"
#समावेश "thrdpool.h"
#समावेश "thrdqueue.h"
#समावेश "curlx/timeval.h"
#अगर_घोषित CURLVERBOSE
#समावेश "curl_trc.h"
#समावेश "urldata.h"
#पूर्ण_अगर


काष्ठा curl_thrdq {
  स्थिर अक्षर *name;
  curl_mutex_t lock;
  curl_cond_t await;
  काष्ठा Curl_llist sendq;
  काष्ठा Curl_llist recvq;
  काष्ठा curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  व्योम *fn_user_तथ्य;
  BIT(aborted);
};

काष्ठा thrdq_item {
  काष्ठा Curl_llist_गाँठ गाँठ;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  व्योम *item;
  काष्ठा curltime start;
  timediff_t timeout_ms;
  स्थिर अक्षर *description;
};

अटल काष्ठा thrdq_item *thrdq_item_create(काष्ठा curl_thrdq *tqueue,
                                            व्योम *item,
                                            स्थिर अक्षर *description,
                                            timediff_t timeout_ms)
{
  काष्ठा thrdq_item *qitem;

  qitem = curlx_calloc(1, माप(*qitem));
  अगर(!qitem)
    वापस शून्य;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  अगर(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  वापस qitem;
}

अटल व्योम thrdq_item_destroy(काष्ठा thrdq_item *qitem)
{
  अगर(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

अटल व्योम thrdq_item_list_dtor(व्योम *user_तथ्य, व्योम *elem)
{
  (व्योम)user_तथ्य;
  thrdq_item_destroy(elem);
}

अटल व्योम *thrdq_tpool_take(व्योम *user_तथ्य, स्थिर अक्षर **pdescription,
                              timediff_t *ptimeout_ms)
{
  काष्ठा curl_thrdq *tqueue = user_तथ्य;
  काष्ठा thrdq_item *qitem = शून्य;
  काष्ठा Curl_llist_गाँठ *e;
  Curl_thrdq_ev_cb *fn_event = शून्य;
  व्योम *fn_user_तथ्य = शून्य;

  Curl_mutex_acquire(&tqueue->lock);
  *pdescription = शून्य;
  *ptimeout_ms = 0;
  अगर(!tqueue->aborted) {
    e = Curl_llist_शीर्ष(&tqueue->sendq);
    अगर(e) {
      काष्ठा curltime now = curlx_now();
      timediff_t timeout_ms;
      जबतक(e) {
        qitem = Curl_गाँठ_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        अगर(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->गाँठ);
          fn_event = tqueue->fn_event;
          fn_user_तथ्य = tqueue->fn_user_तथ्य;
          qitem = शून्य;
          e = Curl_llist_शीर्ष(&tqueue->sendq);
          जारी;
        }
        अन्यथा {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          अवरोध;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  अगर(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_तथ्य);
  वापस qitem;
}

अटल व्योम thrdq_tpool_return(व्योम *item, व्योम *user_तथ्य)
{
  काष्ठा curl_thrdq *tqueue = user_तथ्य;
  काष्ठा thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = शून्य;
  व्योम *fn_user_तथ्य = शून्य;

  अगर(!tqueue) {
    thrdq_item_destroy(item);
    वापस;
  }

  Curl_mutex_acquire(&tqueue->lock);
  अगर(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  अन्यथा {
    DEBUGASSERT(!Curl_गाँठ_llist(&qitem->गाँठ));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->गाँठ);
    fn_event = tqueue->fn_event;
    fn_user_तथ्य = tqueue->fn_user_तथ्य;
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  अगर(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_तथ्य);
}

अटल व्योम thrdq_tpool_process(व्योम *item)
{
  काष्ठा thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

अटल व्योम thrdq_unlink(काष्ठा curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  अगर(tqueue->tpool) {
    अगर(locked)
      Curl_mutex_release(&tqueue->lock);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = शून्य;
    अगर(locked)
      Curl_mutex_acquire(&tqueue->lock);
  }

  Curl_llist_destroy(&tqueue->sendq, शून्य);
  Curl_llist_destroy(&tqueue->recvq, शून्य);
  Curl_cond_destroy(&tqueue->await);
  अगर(locked)
    Curl_mutex_release(&tqueue->lock);
  Curl_mutex_destroy(&tqueue->lock);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(काष्ठा curl_thrdq **ptqueue,
                           स्थिर अक्षर *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           व्योम *user_तथ्य)
{
  काष्ठा curl_thrdq *tqueue;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, माप(*tqueue));
  अगर(!tqueue)
    जाओ out;

  Curl_mutex_init(&tqueue->lock);
  Curl_cond_init(&tqueue->await);
  Curl_llist_init(&tqueue->sendq, thrdq_item_list_dtor);
  Curl_llist_init(&tqueue->recvq, thrdq_item_list_dtor);
  tqueue->fn_free = fn_free;
  tqueue->fn_process = fn_process;
  tqueue->fn_event = fn_event;
  tqueue->fn_user_तथ्य = user_तथ्य;

  /* a const string that remains */
  tqueue->name = name;

  परिणाम = Curl_thrdpool_create(&tqueue->tpool, name,
                                min_threads, max_threads, idle_time_ms,
                                thrdq_tpool_take,
                                thrdq_tpool_process,
                                thrdq_tpool_return,
                                tqueue);

out:
  अगर(परिणाम && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = शून्य;
  }
  *ptqueue = tqueue;
  वापस परिणाम;
}

व्योम Curl_thrdq_destroy(काष्ठा curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->lock);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

अटल uint32_t thrdq_get_signals(काष्ठा curl_thrdq *tqueue)
{
  माप_प्रकार qlen = Curl_llist_गिनती(&tqueue->sendq);
  वापस (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(काष्ठा curl_thrdq *tqueue, व्योम *item,
                         स्थिर अक्षर *description, timediff_t timeout_ms)
{
  काष्ठा thrdq_item *qitem;
  CURLcode परिणाम = CURLE_OK;
  uint32_t signals = 0;

  Curl_mutex_acquire(&tqueue->lock);
  अगर(tqueue->aborted) {
    DEBUGASSERT(0);
    परिणाम = CURLE_SEND_ERROR;
    जाओ out;
  }
  अगर(timeout_ms < 0) {
    परिणाम = CURLE_OPERATION_TIMEDOUT;
    जाओ out;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  अगर(!qitem) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->गाँठ);
  signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  अगर(!परिणाम && signals)
    (व्योम)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  वापस परिणाम;
}

bool Curl_thrdq_check_started(काष्ठा curl_thrdq *tqueue)
{
  माप_प्रकार unprocessed;

  Curl_mutex_acquire(&tqueue->lock);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_गिनती(&tqueue->sendq);
  Curl_mutex_release(&tqueue->lock);
  वापस !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(काष्ठा curl_thrdq *tqueue, व्योम **pitem)
{
  CURLcode परिणाम = CURLE_AGAIN;
  काष्ठा Curl_llist_गाँठ *e;
  uint32_t signals = 0;

  *pitem = शून्य;
  Curl_mutex_acquire(&tqueue->lock);
  अगर(tqueue->aborted) {
    DEBUGASSERT(0);
    परिणाम = CURLE_RECV_ERROR;
    जाओ out;
  }

  e = Curl_llist_शीर्ष(&tqueue->recvq);
  अगर(e) {
    काष्ठा thrdq_item *qitem = Curl_गाँठ_take_elem(e);
    *pitem = qitem->item;
    qitem->item = शून्य;
    thrdq_item_destroy(qitem);
    परिणाम = CURLE_OK;
  }
  अन्यथा
    signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  अगर(signals)
    (व्योम)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  वापस परिणाम;
}

अटल व्योम thrdq_llist_clean_matches(काष्ठा Curl_llist *llist,
                                      Curl_thrdq_item_match_cb *fn_match,
                                      व्योम *match_तथ्य)
{
  काष्ठा Curl_llist_गाँठ *e, *n;
  काष्ठा thrdq_item *qitem;

  क्रम(e = Curl_llist_शीर्ष(llist); e; e = n) {
    n = Curl_गाँठ_अग्र(e);
    qitem = Curl_गाँठ_elem(e);
    अगर(fn_match(qitem->item, match_तथ्य))
      Curl_गाँठ_remove(e);
  }
}

व्योम Curl_thrdq_clear(काष्ठा curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      व्योम *match_तथ्य)
{
  Curl_mutex_acquire(&tqueue->lock);
  अगर(tqueue->aborted) {
    DEBUGASSERT(0);
    जाओ out;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_match, match_तथ्य);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_match, match_तथ्य);
out:
  Curl_mutex_release(&tqueue->lock);
}

#अगर_घोषित UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_done(काष्ठा curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_done(काष्ठा curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  वापस Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#पूर्ण_अगर

CURLcode Curl_thrdq_set_props(काष्ठा curl_thrdq *tqueue,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms)
{
  CURLcode परिणाम;
  uint32_t signals;

  Curl_mutex_acquire(&tqueue->lock);
  signals = thrdq_get_signals(tqueue);
  Curl_mutex_release(&tqueue->lock);

  परिणाम = Curl_thrdpool_set_props(tqueue->tpool, min_threads,
                                   max_threads, idle_time_ms);
  अगर(!परिणाम && signals)
    परिणाम = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  वापस परिणाम;
}

#अगर_घोषित CURLVERBOSE
व्योम Curl_thrdq_trace(काष्ठा curl_thrdq *tqueue,
                      काष्ठा Curl_easy *तथ्य)
{
  काष्ठा curl_trc_feat *feat = &Curl_trc_feat_threads;
  अगर(Curl_trc_ft_is_verbose(तथ्य, feat)) {
    काष्ठा Curl_llist_गाँठ *e;
    काष्ठा thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, तथ्य);
    Curl_mutex_acquire(&tqueue->lock);
    अगर(!Curl_llist_गिनती(&tqueue->sendq) &&
       !Curl_llist_गिनती(&tqueue->recvq)) {
      Curl_trc_feat_infof(तथ्य, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    क्रम(e = Curl_llist_शीर्ष(&tqueue->sendq); e; e = Curl_गाँठ_अग्र(e)) {
      qitem = Curl_गाँठ_elem(e);
      Curl_trc_feat_infof(तथ्य, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    क्रम(e = Curl_llist_शीर्ष(&tqueue->recvq); e; e = Curl_गाँठ_अग्र(e)) {
      qitem = Curl_गाँठ_elem(e);
      Curl_trc_feat_infof(तथ्य, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->lock);
  }
}
#पूर्ण_अगर

#पूर्ण_अगर /* USE_THREADS */
