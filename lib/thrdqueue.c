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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_దారాలు

#అన్తర్భావ "llist.h"
#అన్తర్భావ "curl_threads.h"
#అన్తర్భావ "thrdpool.h"
#అన్తర్భావ "thrdqueue.h"
#అన్తర్భావ "curlx/timeval.h"
#నిర్వచితమైతే CURLVERBOSE
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "urldata.h"
#అయితే_ముగింపు


నిర్మాణం curl_thrdq {
  స్థిరరాశి అక్షరం *name;
  curl_mutex_t తాళం;
  curl_cond_t await;
  నిర్మాణం Curl_llist sendq;
  నిర్మాణం Curl_llist recvq;
  నిర్మాణం curl_thrdpool *tpool;
  Curl_thrdq_అంశం_free_cb *fn_free;
  Curl_thrdq_అంశం_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  శూన్య *fn_user_దత్తాంశం;
  బిట్టు(aborted);
};

నిర్మాణం thrdq_అంశం {
  నిర్మాణం Curl_llist_కణుపు కణుపు;
  Curl_thrdq_అంశం_free_cb *fn_free;
  Curl_thrdq_అంశం_process_cb *fn_process;
  శూన్య *అంశం;
  నిర్మాణం curltime ప్రారంభం;
  timediff_t timeout_ms;
  స్థిరరాశి అక్షరం *description;
};

స్థిర నిర్మాణం thrdq_అంశం *thrdq_అంశం_create(నిర్మాణం curl_thrdq *tqueue,
                                            శూన్య *అంశం,
                                            స్థిరరాశి అక్షరం *description,
                                            timediff_t timeout_ms)
{
  నిర్మాణం thrdq_అంశం *qitem;

  qitem = curlx_calloc(1, పరిమాణం(*qitem));
  ఒకవేళ(!qitem)
    తిరిగి ఖాళీ;
  qitem->అంశం = అంశం;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  ఒకవేళ(timeout_ms) {
    qitem->ప్రారంభం = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  తిరిగి qitem;
}

స్థిర శూన్య thrdq_అంశం_destroy(నిర్మాణం thrdq_అంశం *qitem)
{
  ఒకవేళ(qitem->అంశం)
    qitem->fn_free(qitem->అంశం);
  curlx_free(qitem);
}

స్థిర శూన్య thrdq_అంశం_list_dtor(శూన్య *user_దత్తాంశం, శూన్య *elem)
{
  (శూన్య)user_దత్తాంశం;
  thrdq_అంశం_destroy(elem);
}

స్థిర శూన్య *thrdq_tpool_take(శూన్య *user_దత్తాంశం, స్థిరరాశి అక్షరం **pdescription,
                              timediff_t *ptimeout_ms)
{
  నిర్మాణం curl_thrdq *tqueue = user_దత్తాంశం;
  నిర్మాణం thrdq_అంశం *qitem = ఖాళీ;
  నిర్మాణం Curl_llist_కణుపు *e;
  Curl_thrdq_ev_cb *fn_event = ఖాళీ;
  శూన్య *fn_user_దత్తాంశం = ఖాళీ;

  Curl_mutex_acquire(&tqueue->తాళం);
  *pdescription = ఖాళీ;
  *ptimeout_ms = 0;
  ఒకవేళ(!tqueue->aborted) {
    e = Curl_llist_తల(&tqueue->sendq);
    ఒకవేళ(e) {
      నిర్మాణం curltime now = curlx_now();
      timediff_t timeout_ms;
      అయితే(e) {
        qitem = Curl_కణుపు_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->ప్రారంభం));
        ఒకవేళ(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->కణుపు);
          fn_event = tqueue->fn_event;
          fn_user_దత్తాంశం = tqueue->fn_user_దత్తాంశం;
          qitem = ఖాళీ;
          e = Curl_llist_తల(&tqueue->sendq);
          కొనసాగు;
        }
        లేకపోతే {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          ఆపు;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->తాళం);
  /* avoiding deadlocks */
  ఒకవేళ(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_అంశం_పూర్తి, fn_user_దత్తాంశం);
  తిరిగి qitem;
}

స్థిర శూన్య thrdq_tpool_return(శూన్య *అంశం, శూన్య *user_దత్తాంశం)
{
  నిర్మాణం curl_thrdq *tqueue = user_దత్తాంశం;
  నిర్మాణం thrdq_అంశం *qitem = అంశం;
  Curl_thrdq_ev_cb *fn_event = ఖాళీ;
  శూన్య *fn_user_దత్తాంశం = ఖాళీ;

  ఒకవేళ(!tqueue) {
    thrdq_అంశం_destroy(అంశం);
    తిరిగి;
  }

  Curl_mutex_acquire(&tqueue->తాళం);
  ఒకవేళ(tqueue->aborted) {
    thrdq_అంశం_destroy(qitem);
  }
  లేకపోతే {
    DEBUGASSERT(!Curl_కణుపు_llist(&qitem->కణుపు));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->కణుపు);
    fn_event = tqueue->fn_event;
    fn_user_దత్తాంశం = tqueue->fn_user_దత్తాంశం;
  }
  Curl_mutex_release(&tqueue->తాళం);
  /* avoiding deadlocks */
  ఒకవేళ(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_అంశం_పూర్తి, fn_user_దత్తాంశం);
}

స్థిర శూన్య thrdq_tpool_process(శూన్య *అంశం)
{
  నిర్మాణం thrdq_అంశం *qitem = అంశం;
  qitem->fn_process(qitem->అంశం);
}

స్థిర శూన్య thrdq_unlink(నిర్మాణం curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  ఒకవేళ(tqueue->tpool) {
    ఒకవేళ(locked)
      Curl_mutex_release(&tqueue->తాళం);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = ఖాళీ;
    ఒకవేళ(locked)
      Curl_mutex_acquire(&tqueue->తాళం);
  }

  Curl_llist_destroy(&tqueue->sendq, ఖాళీ);
  Curl_llist_destroy(&tqueue->recvq, ఖాళీ);
  Curl_cond_destroy(&tqueue->await);
  ఒకవేళ(locked)
    Curl_mutex_release(&tqueue->తాళం);
  Curl_mutex_destroy(&tqueue->తాళం);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(నిర్మాణం curl_thrdq **ptqueue,
                           స్థిరరాశి అక్షరం *name,
                           uint32_t min_దారాలు,
                           uint32_t max_దారాలు,
                           uint32_t idle_సమయం_ms,
                           Curl_thrdq_అంశం_free_cb *fn_free,
                           Curl_thrdq_అంశం_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           శూన్య *user_దత్తాంశం)
{
  నిర్మాణం curl_thrdq *tqueue;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, పరిమాణం(*tqueue));
  ఒకవేళ(!tqueue)
    వెళ్ళు బహిర్గతం;

  Curl_mutex_init(&tqueue->తాళం);
  Curl_cond_init(&tqueue->await);
  Curl_llist_init(&tqueue->sendq, thrdq_అంశం_list_dtor);
  Curl_llist_init(&tqueue->recvq, thrdq_అంశం_list_dtor);
  tqueue->fn_free = fn_free;
  tqueue->fn_process = fn_process;
  tqueue->fn_event = fn_event;
  tqueue->fn_user_దత్తాంశం = user_దత్తాంశం;

  /* a const string that remains */
  tqueue->name = name;

  ఫలితం = Curl_thrdpool_create(&tqueue->tpool, name,
                                min_దారాలు, max_దారాలు, idle_సమయం_ms,
                                thrdq_tpool_take,
                                thrdq_tpool_process,
                                thrdq_tpool_return,
                                tqueue);

బహిర్గతం:
  ఒకవేళ(ఫలితం && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = ఖాళీ;
  }
  *ptqueue = tqueue;
  తిరిగి ఫలితం;
}

శూన్య Curl_thrdq_destroy(నిర్మాణం curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->తాళం);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

స్థిర uint32_t thrdq_get_సంకేతాలు(నిర్మాణం curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_లెక్క(&tqueue->sendq);
  తిరిగి (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(నిర్మాణం curl_thrdq *tqueue, శూన్య *అంశం,
                         స్థిరరాశి అక్షరం *description, timediff_t timeout_ms)
{
  నిర్మాణం thrdq_అంశం *qitem;
  CURLcode ఫలితం = CURLE_OK;
  uint32_t సంకేతాలు = 0;

  Curl_mutex_acquire(&tqueue->తాళం);
  ఒకవేళ(tqueue->aborted) {
    DEBUGASSERT(0);
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(timeout_ms < 0) {
    ఫలితం = CURLE_OPERATION_TIMEDOUT;
    వెళ్ళు బహిర్గతం;
  }

  qitem = thrdq_అంశం_create(tqueue, అంశం, description, timeout_ms);
  ఒకవేళ(!qitem) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->కణుపు);
  సంకేతాలు = thrdq_get_సంకేతాలు(tqueue);

బహిర్గతం:
  Curl_mutex_release(&tqueue->తాళం);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  ఒకవేళ(!ఫలితం && సంకేతాలు)
    (శూన్య)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)సంకేతాలు);
  తిరిగి ఫలితం;
}

bool Curl_thrdq_check_started(నిర్మాణం curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->తాళం);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_లెక్క(&tqueue->sendq);
  Curl_mutex_release(&tqueue->తాళం);
  తిరిగి !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(నిర్మాణం curl_thrdq *tqueue, శూన్య **pitem)
{
  CURLcode ఫలితం = CURLE_AGAIN;
  నిర్మాణం Curl_llist_కణుపు *e;
  uint32_t సంకేతాలు = 0;

  *pitem = ఖాళీ;
  Curl_mutex_acquire(&tqueue->తాళం);
  ఒకవేళ(tqueue->aborted) {
    DEBUGASSERT(0);
    ఫలితం = CURLE_RECV_ERROR;
    వెళ్ళు బహిర్గతం;
  }

  e = Curl_llist_తల(&tqueue->recvq);
  ఒకవేళ(e) {
    నిర్మాణం thrdq_అంశం *qitem = Curl_కణుపు_take_elem(e);
    *pitem = qitem->అంశం;
    qitem->అంశం = ఖాళీ;
    thrdq_అంశం_destroy(qitem);
    ఫలితం = CURLE_OK;
  }
  లేకపోతే
    సంకేతాలు = thrdq_get_సంకేతాలు(tqueue);

బహిర్గతం:
  Curl_mutex_release(&tqueue->తాళం);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  ఒకవేళ(సంకేతాలు)
    (శూన్య)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)సంకేతాలు);
  తిరిగి ఫలితం;
}

స్థిర శూన్య thrdq_llist_clean_matches(నిర్మాణం Curl_llist *llist,
                                      Curl_thrdq_అంశం_సరిపోలిక_cb *fn_సరిపోలిక,
                                      శూన్య *సరిపోలిక_దత్తాంశం)
{
  నిర్మాణం Curl_llist_కణుపు *e, *n;
  నిర్మాణం thrdq_అంశం *qitem;

  కొరకు(e = Curl_llist_తల(llist); e; e = n) {
    n = Curl_కణుపు_తదుపరి(e);
    qitem = Curl_కణుపు_elem(e);
    ఒకవేళ(fn_సరిపోలిక(qitem->అంశం, సరిపోలిక_దత్తాంశం))
      Curl_కణుపు_remove(e);
  }
}

శూన్య Curl_thrdq_clear(నిర్మాణం curl_thrdq *tqueue,
                      Curl_thrdq_అంశం_సరిపోలిక_cb *fn_సరిపోలిక,
                      శూన్య *సరిపోలిక_దత్తాంశం)
{
  Curl_mutex_acquire(&tqueue->తాళం);
  ఒకవేళ(tqueue->aborted) {
    DEBUGASSERT(0);
    వెళ్ళు బహిర్గతం;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_సరిపోలిక, సరిపోలిక_దత్తాంశం);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_సరిపోలిక, సరిపోలిక_దత్తాంశం);
బహిర్గతం:
  Curl_mutex_release(&tqueue->తాళం);
}

#నిర్వచితమైతే UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_పూర్తి(నిర్మాణం curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_పూర్తి(నిర్మాణం curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  తిరిగి Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#అయితే_ముగింపు

CURLcode Curl_thrdq_సమితి_props(నిర్మాణం curl_thrdq *tqueue,
                              uint32_t min_దారాలు,
                              uint32_t max_దారాలు,
                              uint32_t idle_సమయం_ms)
{
  CURLcode ఫలితం;
  uint32_t సంకేతాలు;

  Curl_mutex_acquire(&tqueue->తాళం);
  సంకేతాలు = thrdq_get_సంకేతాలు(tqueue);
  Curl_mutex_release(&tqueue->తాళం);

  ఫలితం = Curl_thrdpool_సమితి_props(tqueue->tpool, min_దారాలు,
                                   max_దారాలు, idle_సమయం_ms);
  ఒకవేళ(!ఫలితం && సంకేతాలు)
    ఫలితం = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)సంకేతాలు);
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే CURLVERBOSE
శూన్య Curl_thrdq_trace(నిర్మాణం curl_thrdq *tqueue,
                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం curl_trc_feat *feat = &Curl_trc_feat_దారాలు;
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, feat)) {
    నిర్మాణం Curl_llist_కణుపు *e;
    నిర్మాణం thrdq_అంశం *qitem;

    Curl_thrdpool_trace(tqueue->tpool, దత్తాంశం);
    Curl_mutex_acquire(&tqueue->తాళం);
    ఒకవేళ(!Curl_llist_లెక్క(&tqueue->sendq) &&
       !Curl_llist_లెక్క(&tqueue->recvq)) {
      Curl_trc_feat_infof(దత్తాంశం, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    కొరకు(e = Curl_llist_తల(&tqueue->sendq); e; e = Curl_కణుపు_తదుపరి(e)) {
      qitem = Curl_కణుపు_elem(e);
      Curl_trc_feat_infof(దత్తాంశం, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    కొరకు(e = Curl_llist_తల(&tqueue->recvq); e; e = Curl_కణుపు_తదుపరి(e)) {
      qitem = Curl_కణుపు_elem(e);
      Curl_trc_feat_infof(దత్తాంశం, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->తాళం);
  }
}
#అయితే_ముగింపు

#అయితే_ముగింపు /* USE_THREADS */
