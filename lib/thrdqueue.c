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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_THREADS

#अन्तर्भाव "llist.h"
#अन्तर्भाव "curl_threads.h"
#अन्तर्भाव "thrdpool.h"
#अन्तर्भाव "thrdqueue.h"
#अन्तर्भाव "curlx/timeval.h"
#ifdef CURLVERBOSE
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "urldata.h"
#endif


संरचना curl_thrdq {
  ध्रुव वर्ण *name;
  curl_mutex_t lock;
  curl_cond_t await;
  संरचना Curl_llist sendq;
  संरचना Curl_llist recvq;
  संरचना curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  शून्य *fn_user_data;
  BIT(aborted);
};

संरचना thrdq_item {
  संरचना Curl_llist_ग्रन्थिः ग्रन्थिः;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  शून्य *item;
  संरचना curltime start;
  timediff_t timeout_ms;
  ध्रुव वर्ण *description;
};

स्थिर संरचना thrdq_item *thrdq_item_create(संरचना curl_thrdq *tqueue,
                                            शून्य *item,
                                            ध्रुव वर्ण *description,
                                            timediff_t timeout_ms)
{
  संरचना thrdq_item *qitem;

  qitem = curlx_calloc(1, परिमाण(*qitem));
  सचे(!qitem)
    पटिनिवत्त रिक्त;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  सचे(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  पटिनिवत्त qitem;
}

स्थिर शून्य thrdq_item_destroy(संरचना thrdq_item *qitem)
{
  सचे(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

स्थिर शून्य thrdq_item_list_dtor(शून्य *user_data, शून्य *elem)
{
  (शून्य)user_data;
  thrdq_item_destroy(elem);
}

स्थिर शून्य *thrdq_tpool_take(शून्य *user_data, ध्रुव वर्ण **pdescription,
                              timediff_t *ptimeout_ms)
{
  संरचना curl_thrdq *tqueue = user_data;
  संरचना thrdq_item *qitem = रिक्त;
  संरचना Curl_llist_ग्रन्थिः *e;
  Curl_thrdq_ev_cb *fn_event = रिक्त;
  शून्य *fn_user_data = रिक्त;

  Curl_mutex_acquire(&tqueue->lock);
  *pdescription = रिक्त;
  *ptimeout_ms = 0;
  सचे(!tqueue->aborted) {
    e = Curl_llist_head(&tqueue->sendq);
    सचे(e) {
      संरचना curltime now = curlx_now();
      timediff_t timeout_ms;
      याव(e) {
        qitem = Curl_ग्रन्थिः_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        सचे(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->ग्रन्थिः);
          fn_event = tqueue->fn_event;
          fn_user_data = tqueue->fn_user_data;
          qitem = रिक्त;
          e = Curl_llist_head(&tqueue->sendq);
          अनुवर्त;
        }
        अञ्ञथा {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          भङ्ग;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  सचे(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
  पटिनिवत्त qitem;
}

स्थिर शून्य thrdq_tpool_return(शून्य *item, शून्य *user_data)
{
  संरचना curl_thrdq *tqueue = user_data;
  संरचना thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = रिक्त;
  शून्य *fn_user_data = रिक्त;

  सचे(!tqueue) {
    thrdq_item_destroy(item);
    पटिनिवत्त;
  }

  Curl_mutex_acquire(&tqueue->lock);
  सचे(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  अञ्ञथा {
    DEBUGASSERT(!Curl_ग्रन्थिः_llist(&qitem->ग्रन्थिः));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->ग्रन्थिः);
    fn_event = tqueue->fn_event;
    fn_user_data = tqueue->fn_user_data;
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  सचे(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
}

स्थिर शून्य thrdq_tpool_process(शून्य *item)
{
  संरचना thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

स्थिर शून्य thrdq_unlink(संरचना curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  सचे(tqueue->tpool) {
    सचे(locked)
      Curl_mutex_release(&tqueue->lock);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = रिक्त;
    सचे(locked)
      Curl_mutex_acquire(&tqueue->lock);
  }

  Curl_llist_destroy(&tqueue->sendq, रिक्त);
  Curl_llist_destroy(&tqueue->recvq, रिक्त);
  Curl_cond_destroy(&tqueue->await);
  सचे(locked)
    Curl_mutex_release(&tqueue->lock);
  Curl_mutex_destroy(&tqueue->lock);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(संरचना curl_thrdq **ptqueue,
                           ध्रुव वर्ण *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           शून्य *user_data)
{
  संरचना curl_thrdq *tqueue;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, परिमाण(*tqueue));
  सचे(!tqueue)
    गच्छ out;

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
  सचे(result && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = रिक्त;
  }
  *ptqueue = tqueue;
  पटिनिवत्त result;
}

शून्य Curl_thrdq_destroy(संरचना curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->lock);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

स्थिर uint32_t thrdq_get_signals(संरचना curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_गणनम्(&tqueue->sendq);
  पटिनिवत्त (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(संरचना curl_thrdq *tqueue, शून्य *item,
                         ध्रुव वर्ण *description, timediff_t timeout_ms)
{
  संरचना thrdq_item *qitem;
  CURLcode result = CURLE_OK;
  uint32_t signals = 0;

  Curl_mutex_acquire(&tqueue->lock);
  सचे(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_SEND_ERROR;
    गच्छ out;
  }
  सचे(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    गच्छ out;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  सचे(!qitem) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->ग्रन्थिः);
  signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  सचे(!result && signals)
    (शून्य)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  पटिनिवत्त result;
}

bool Curl_thrdq_check_started(संरचना curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->lock);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_गणनम्(&tqueue->sendq);
  Curl_mutex_release(&tqueue->lock);
  पटिनिवत्त !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(संरचना curl_thrdq *tqueue, शून्य **pitem)
{
  CURLcode result = CURLE_AGAIN;
  संरचना Curl_llist_ग्रन्थिः *e;
  uint32_t signals = 0;

  *pitem = रिक्त;
  Curl_mutex_acquire(&tqueue->lock);
  सचे(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_RECV_ERROR;
    गच्छ out;
  }

  e = Curl_llist_head(&tqueue->recvq);
  सचे(e) {
    संरचना thrdq_item *qitem = Curl_ग्रन्थिः_take_elem(e);
    *pitem = qitem->item;
    qitem->item = रिक्त;
    thrdq_item_destroy(qitem);
    result = CURLE_OK;
  }
  अञ्ञथा
    signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  सचे(signals)
    (शून्य)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  पटिनिवत्त result;
}

स्थिर शून्य thrdq_llist_clean_matches(संरचना Curl_llist *llist,
                                      Curl_thrdq_item_match_cb *fn_match,
                                      शून्य *match_data)
{
  संरचना Curl_llist_ग्रन्थिः *e, *n;
  संरचना thrdq_item *qitem;

  अत्थाय(e = Curl_llist_head(llist); e; e = n) {
    n = Curl_ग्रन्थिः_next(e);
    qitem = Curl_ग्रन्थिः_elem(e);
    सचे(fn_match(qitem->item, match_data))
      Curl_ग्रन्थिः_remove(e);
  }
}

शून्य Curl_thrdq_clear(संरचना curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      शून्य *match_data)
{
  Curl_mutex_acquire(&tqueue->lock);
  सचे(tqueue->aborted) {
    DEBUGASSERT(0);
    गच्छ out;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_match, match_data);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_match, match_data);
out:
  Curl_mutex_release(&tqueue->lock);
}

#ifdef UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_done(संरचना curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_done(संरचना curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  पटिनिवत्त Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#endif

CURLcode Curl_thrdq_set_props(संरचना curl_thrdq *tqueue,
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
  सचे(!result && signals)
    result = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  पटिनिवत्त result;
}

#ifdef CURLVERBOSE
शून्य Curl_thrdq_trace(संरचना curl_thrdq *tqueue,
                      संरचना Curl_easy *data)
{
  संरचना curl_trc_feat *feat = &Curl_trc_feat_threads;
  सचे(Curl_trc_ft_is_verbose(data, feat)) {
    संरचना Curl_llist_ग्रन्थिः *e;
    संरचना thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, data);
    Curl_mutex_acquire(&tqueue->lock);
    सचे(!Curl_llist_गणनम्(&tqueue->sendq) &&
       !Curl_llist_गणनम्(&tqueue->recvq)) {
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    अत्थाय(e = Curl_llist_head(&tqueue->sendq); e; e = Curl_ग्रन्थिः_next(e)) {
      qitem = Curl_ग्रन्थिः_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    अत्थाय(e = Curl_llist_head(&tqueue->recvq); e; e = Curl_ग्रन्थिः_next(e)) {
      qitem = Curl_ग्रन्थिः_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->lock);
  }
}
#endif

#endif /* USE_THREADS */
