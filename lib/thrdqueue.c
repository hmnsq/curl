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

#यदि_परिभाषितम् USE_तन्तवः

#अन्तर्भाव "llist.h"
#अन्तर्भाव "curl_threads.h"
#अन्तर्भाव "thrdpool.h"
#अन्तर्भाव "thrdqueue.h"
#अन्तर्भाव "curlx/timeval.h"
#यदि_परिभाषितम् CURLVERBOSE
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "urldata.h"
#यदि_अन्तः


संरचना curl_thrdq {
  ध्रुव वर्ण *name;
  curl_mutex_t तालकम्;
  curl_cond_t await;
  संरचना Curl_llist sendq;
  संरचना Curl_llist recvq;
  संरचना curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  शून्य *fn_user_दत्त;
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
  यदि(!qitem)
    प्रत्यागम रिक्त;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  यदि(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  प्रत्यागम qitem;
}

स्थिर शून्य thrdq_item_destroy(संरचना thrdq_item *qitem)
{
  यदि(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

स्थिर शून्य thrdq_item_list_dtor(शून्य *user_दत्त, शून्य *elem)
{
  (शून्य)user_दत्त;
  thrdq_item_destroy(elem);
}

स्थिर शून्य *thrdq_tpool_take(शून्य *user_दत्त, ध्रुव वर्ण **pdescription,
                              timediff_t *ptimeout_ms)
{
  संरचना curl_thrdq *tqueue = user_दत्त;
  संरचना thrdq_item *qitem = रिक्त;
  संरचना Curl_llist_ग्रन्थिः *e;
  Curl_thrdq_ev_cb *fn_event = रिक्त;
  शून्य *fn_user_दत्त = रिक्त;

  Curl_mutex_acquire(&tqueue->तालकम्);
  *pdescription = रिक्त;
  *ptimeout_ms = 0;
  यदि(!tqueue->aborted) {
    e = Curl_llist_शीर्ष(&tqueue->sendq);
    यदि(e) {
      संरचना curltime now = curlx_now();
      timediff_t timeout_ms;
      यावत्(e) {
        qitem = Curl_ग्रन्थिः_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        यदि(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->ग्रन्थिः);
          fn_event = tqueue->fn_event;
          fn_user_दत्त = tqueue->fn_user_दत्त;
          qitem = रिक्त;
          e = Curl_llist_शीर्ष(&tqueue->sendq);
          अनुवर्त;
        }
        अन्यथा {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          भङ्ग;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->तालकम्);
  /* avoiding deadlocks */
  यदि(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_समाप्तम्, fn_user_दत्त);
  प्रत्यागम qitem;
}

स्थिर शून्य thrdq_tpool_return(शून्य *item, शून्य *user_दत्त)
{
  संरचना curl_thrdq *tqueue = user_दत्त;
  संरचना thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = रिक्त;
  शून्य *fn_user_दत्त = रिक्त;

  यदि(!tqueue) {
    thrdq_item_destroy(item);
    प्रत्यागम;
  }

  Curl_mutex_acquire(&tqueue->तालकम्);
  यदि(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  अन्यथा {
    DEBUGASSERT(!Curl_ग्रन्थिः_llist(&qitem->ग्रन्थिः));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->ग्रन्थिः);
    fn_event = tqueue->fn_event;
    fn_user_दत्त = tqueue->fn_user_दत्त;
  }
  Curl_mutex_release(&tqueue->तालकम्);
  /* avoiding deadlocks */
  यदि(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_समाप्तम्, fn_user_दत्त);
}

स्थिर शून्य thrdq_tpool_process(शून्य *item)
{
  संरचना thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

स्थिर शून्य thrdq_unlink(संरचना curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  यदि(tqueue->tpool) {
    यदि(locked)
      Curl_mutex_release(&tqueue->तालकम्);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = रिक्त;
    यदि(locked)
      Curl_mutex_acquire(&tqueue->तालकम्);
  }

  Curl_llist_destroy(&tqueue->sendq, रिक्त);
  Curl_llist_destroy(&tqueue->recvq, रिक्त);
  Curl_cond_destroy(&tqueue->await);
  यदि(locked)
    Curl_mutex_release(&tqueue->तालकम्);
  Curl_mutex_destroy(&tqueue->तालकम्);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(संरचना curl_thrdq **ptqueue,
                           ध्रुव वर्ण *name,
                           uint32_t min_तन्तवः,
                           uint32_t max_तन्तवः,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           शून्य *user_दत्त)
{
  संरचना curl_thrdq *tqueue;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, परिमाण(*tqueue));
  यदि(!tqueue)
    गच्छ बहिर्गमनम्;

  Curl_mutex_init(&tqueue->तालकम्);
  Curl_cond_init(&tqueue->await);
  Curl_llist_init(&tqueue->sendq, thrdq_item_list_dtor);
  Curl_llist_init(&tqueue->recvq, thrdq_item_list_dtor);
  tqueue->fn_free = fn_free;
  tqueue->fn_process = fn_process;
  tqueue->fn_event = fn_event;
  tqueue->fn_user_दत्त = user_दत्त;

  /* a const string that remains */
  tqueue->name = name;

  परिणाम = Curl_thrdpool_create(&tqueue->tpool, name,
                                min_तन्तवः, max_तन्तवः, idle_time_ms,
                                thrdq_tpool_take,
                                thrdq_tpool_process,
                                thrdq_tpool_return,
                                tqueue);

बहिर्गमनम्:
  यदि(परिणाम && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = रिक्त;
  }
  *ptqueue = tqueue;
  प्रत्यागम परिणाम;
}

शून्य Curl_thrdq_destroy(संरचना curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->तालकम्);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

स्थिर uint32_t thrdq_get_सङ्केताः(संरचना curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_गणनम्(&tqueue->sendq);
  प्रत्यागम (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(संरचना curl_thrdq *tqueue, शून्य *item,
                         ध्रुव वर्ण *description, timediff_t timeout_ms)
{
  संरचना thrdq_item *qitem;
  CURLcode परिणाम = CURLE_OK;
  uint32_t सङ्केताः = 0;

  Curl_mutex_acquire(&tqueue->तालकम्);
  यदि(tqueue->aborted) {
    DEBUGASSERT(0);
    परिणाम = CURLE_SEND_ERROR;
    गच्छ बहिर्गमनम्;
  }
  यदि(timeout_ms < 0) {
    परिणाम = CURLE_OPERATION_TIMEDOUT;
    गच्छ बहिर्गमनम्;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  यदि(!qitem) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->ग्रन्थिः);
  सङ्केताः = thrdq_get_सङ्केताः(tqueue);

बहिर्गमनम्:
  Curl_mutex_release(&tqueue->तालकम्);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  यदि(!परिणाम && सङ्केताः)
    (शून्य)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)सङ्केताः);
  प्रत्यागम परिणाम;
}

bool Curl_thrdq_check_started(संरचना curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->तालकम्);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_गणनम्(&tqueue->sendq);
  Curl_mutex_release(&tqueue->तालकम्);
  प्रत्यागम !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(संरचना curl_thrdq *tqueue, शून्य **pitem)
{
  CURLcode परिणाम = CURLE_AGAIN;
  संरचना Curl_llist_ग्रन्थिः *e;
  uint32_t सङ्केताः = 0;

  *pitem = रिक्त;
  Curl_mutex_acquire(&tqueue->तालकम्);
  यदि(tqueue->aborted) {
    DEBUGASSERT(0);
    परिणाम = CURLE_RECV_ERROR;
    गच्छ बहिर्गमनम्;
  }

  e = Curl_llist_शीर्ष(&tqueue->recvq);
  यदि(e) {
    संरचना thrdq_item *qitem = Curl_ग्रन्थिः_take_elem(e);
    *pitem = qitem->item;
    qitem->item = रिक्त;
    thrdq_item_destroy(qitem);
    परिणाम = CURLE_OK;
  }
  अन्यथा
    सङ्केताः = thrdq_get_सङ्केताः(tqueue);

बहिर्गमनम्:
  Curl_mutex_release(&tqueue->तालकम्);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  यदि(सङ्केताः)
    (शून्य)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)सङ्केताः);
  प्रत्यागम परिणाम;
}

स्थिर शून्य thrdq_llist_clean_matches(संरचना Curl_llist *llist,
                                      Curl_thrdq_item_मेलनम्_cb *fn_मेलनम्,
                                      शून्य *मेलनम्_दत्त)
{
  संरचना Curl_llist_ग्रन्थिः *e, *n;
  संरचना thrdq_item *qitem;

  प्रति(e = Curl_llist_शीर्ष(llist); e; e = n) {
    n = Curl_ग्रन्थिः_अग्र(e);
    qitem = Curl_ग्रन्थिः_elem(e);
    यदि(fn_मेलनम्(qitem->item, मेलनम्_दत्त))
      Curl_ग्रन्थिः_remove(e);
  }
}

शून्य Curl_thrdq_clear(संरचना curl_thrdq *tqueue,
                      Curl_thrdq_item_मेलनम्_cb *fn_मेलनम्,
                      शून्य *मेलनम्_दत्त)
{
  Curl_mutex_acquire(&tqueue->तालकम्);
  यदि(tqueue->aborted) {
    DEBUGASSERT(0);
    गच्छ बहिर्गमनम्;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_मेलनम्, मेलनम्_दत्त);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_मेलनम्, मेलनम्_दत्त);
बहिर्गमनम्:
  Curl_mutex_release(&tqueue->तालकम्);
}

#यदि_परिभाषितम् UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_समाप्तम्(संरचना curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_समाप्तम्(संरचना curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  प्रत्यागम Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#यदि_अन्तः

CURLcode Curl_thrdq_समुच्चयः_props(संरचना curl_thrdq *tqueue,
                              uint32_t min_तन्तवः,
                              uint32_t max_तन्तवः,
                              uint32_t idle_time_ms)
{
  CURLcode परिणाम;
  uint32_t सङ्केताः;

  Curl_mutex_acquire(&tqueue->तालकम्);
  सङ्केताः = thrdq_get_सङ्केताः(tqueue);
  Curl_mutex_release(&tqueue->तालकम्);

  परिणाम = Curl_thrdpool_समुच्चयः_props(tqueue->tpool, min_तन्तवः,
                                   max_तन्तवः, idle_time_ms);
  यदि(!परिणाम && सङ्केताः)
    परिणाम = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)सङ्केताः);
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् CURLVERBOSE
शून्य Curl_thrdq_trace(संरचना curl_thrdq *tqueue,
                      संरचना Curl_easy *दत्त)
{
  संरचना curl_trc_feat *feat = &Curl_trc_feat_तन्तवः;
  यदि(Curl_trc_ft_is_verbose(दत्त, feat)) {
    संरचना Curl_llist_ग्रन्थिः *e;
    संरचना thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, दत्त);
    Curl_mutex_acquire(&tqueue->तालकम्);
    यदि(!Curl_llist_गणनम्(&tqueue->sendq) &&
       !Curl_llist_गणनम्(&tqueue->recvq)) {
      Curl_trc_feat_infof(दत्त, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    प्रति(e = Curl_llist_शीर्ष(&tqueue->sendq); e; e = Curl_ग्रन्थिः_अग्र(e)) {
      qitem = Curl_ग्रन्थिः_elem(e);
      Curl_trc_feat_infof(दत्त, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    प्रति(e = Curl_llist_शीर्ष(&tqueue->recvq); e; e = Curl_ग्रन्थिः_अग्र(e)) {
      qitem = Curl_ग्रन्थिः_elem(e);
      Curl_trc_feat_infof(दत्त, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->तालकम्);
  }
}
#यदि_अन्तः

#यदि_अन्तः /* USE_THREADS */
