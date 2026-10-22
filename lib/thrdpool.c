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
#అన్తర్భావ "curlx/timeval.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "thrdpool.h"
#నిర్వచితమైతే CURLVERBOSE
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "urldata.h"
#అయితే_ముగింపు


నిర్మాణం thrdslot {
  నిర్మాణం Curl_llist_కణుపు కణుపు;
  నిర్మాణం curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  నిర్మాణం curltime starttime;
  స్థిరరాశి అక్షరం *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  బిట్టు(running);
  బిట్టు(idle);
};

నిర్మాణం curl_thrdpool {
  స్థిరరాశి అక్షరం *name;
  uint64_t refcount;
  curl_mutex_t తాళం;
  curl_cond_t await;
  నిర్మాణం Curl_llist slots;
  నిర్మాణం Curl_llist zombies;
  Curl_thrdpool_take_అంశం_cb *fn_take;
  Curl_thrdpool_process_అంశం_cb *fn_process;
  Curl_thrdpool_return_అంశం_cb *fn_return;
  శూన్య *fn_user_దత్తాంశం;
  CURLcode fatal_err;
  uint32_t min_దారాలు;
  uint32_t max_దారాలు;
  uint32_t idle_సమయం_ms;
  uint32_t తదుపరి_id;
#నిర్వచితమైతే DEBUGBUILD
  పూర్ణాంకం dbg_fail_starts; /* fail this many thread starts */
#అయితే_ముగింపు
  బిట్టు(aborted);
  బిట్టు(detached);
};

స్థిర శూన్య thrdpool_join_zombies(నిర్మాణం curl_thrdpool *tpool);
స్థిర bool thrdpool_unlink(నిర్మాణం curl_thrdpool *tpool, bool locked);

స్థిర శూన్య thrdslot_destroy(నిర్మాణం thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

స్థిర శూన్య thrdslot_పూర్తి(నిర్మాణం thrdslot *tslot)
{
  నిర్మాణం curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_కణుపు_llist(&tslot->కణుపు) == &tpool->slots);
  Curl_కణుపు_remove(&tslot->కణుపు);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->కణుపు);
  Curl_cond_signal(&tpool->await);
}

స్థిర CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_నిరంతరం(శూన్య *arg)
{
  నిర్మాణం thrdslot *tslot = arg;
  నిర్మాణం curl_thrdpool *tpool = tslot->tpool;
  శూన్య *అంశం;

  Curl_mutex_acquire(&tpool->తాళం);
  DEBUGASSERT(Curl_కణుపు_llist(&tslot->కణుపు) == &tpool->slots);
  కొరకు(;;) {
    అయితే(!tpool->aborted) {
      tslot->work_description = ఖాళీ;
      tslot->work_timeout_ms = 0;
      అంశం = tpool->fn_take(tpool->fn_user_దత్తాంశం, &tslot->work_description,
                            &tslot->work_timeout_ms);
      ఒకవేళ(!అంశం)
        ఆపు;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->తాళం);

      tpool->fn_process(అంశం);

      Curl_mutex_acquire(&tpool->తాళం);
      tslot->work_description = ఖాళీ;
      tpool->fn_return(అంశం, tpool->aborted ? ఖాళీ : tpool->fn_user_దత్తాంశం);
    }

    ఒకవేళ(tpool->aborted ||
       (Curl_llist_లెక్క(&tpool->slots) > tpool->max_దారాలు))
      వెళ్ళు బహిర్గతం;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    ఒకవేళ((tpool->idle_సమయం_ms > 0) &&
       (Curl_llist_లెక్క(&tpool->slots) > tpool->min_దారాలు)) {
      CURLcode ఫలితం = Curl_cond_timedwait(&tslot->await, &tpool->తాళం,
                                            tpool->idle_సమయం_ms);
      ఒకవేళ((ఫలితం == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_లెక్క(&tpool->slots) > tpool->min_దారాలు)) {
        వెళ్ళు బహిర్గతం;
      }
    }
    లేకపోతే {
      Curl_cond_wait(&tslot->await, &tpool->తాళం);
    }
  }

బహిర్గతం:
  thrdslot_పూర్తి(tslot);
  ఒకవేళ(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->తాళం);
  }
  తిరిగి 0;
}

స్థిర CURLcode thrdslot_ప్రారంభం(నిర్మాణం curl_thrdpool *tpool)
{
  నిర్మాణం thrdslot *tslot;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  tslot = curlx_calloc(1, పరిమాణం(*tslot));
  ఒకవేళ(!tslot)
    వెళ్ళు బహిర్గతం;
  tslot->id = tpool->తదుపరి_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  లేకపోతే
#అయితే_ముగింపు
    tslot->thread = Curl_thread_create(thrdslot_నిరంతరం, tslot);
  ఒకవేళ(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    ఫలితం = CURLE_FAILED_INIT;
    వెళ్ళు బహిర్గతం;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->కణుపు);
  tslot = ఖాళీ;
  ఫలితం = CURLE_OK;

బహిర్గతం:
  ఒకవేళ(tslot)
    thrdslot_destroy(tslot);
  తిరిగి ఫలితం;
}

స్థిర శూన్య thrdpool_wake_all(నిర్మాణం curl_thrdpool *tpool)
{
  నిర్మాణం Curl_llist_కణుపు *e;
  కొరకు(e = Curl_llist_తల(&tpool->slots); e; e = Curl_కణుపు_తదుపరి(e)) {
    నిర్మాణం thrdslot *tslot = Curl_కణుపు_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

స్థిర శూన్య thrdpool_join_zombies(నిర్మాణం curl_thrdpool *tpool)
{
  నిర్మాణం Curl_llist_కణుపు *e;

  కొరకు(e = Curl_llist_తల(&tpool->zombies); e;
      e = Curl_llist_తల(&tpool->zombies)) {
    నిర్మాణం thrdslot *tslot = Curl_కణుపు_elem(e);

    Curl_కణుపు_remove(&tslot->కణుపు);
    ఒకవేళ(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->తాళం);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->తాళం);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

స్థిర bool thrdpool_unlink(నిర్మాణం curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  ఒకవేళ(tpool->refcount)
    tpool->refcount--;
  ఒకవేళ(tpool->refcount)
    తిరిగి FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  ఒకవేళ(locked)
    Curl_mutex_release(&tpool->తాళం);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->తాళం);
  curlx_free(tpool);
  తిరిగి TRUE;
}

స్థిర CURLcode thrdpool_signal(నిర్మాణం curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  నిర్మాణం Curl_llist_కణుపు *e, *n;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  కొరకు(e = Curl_llist_తల(&tpool->slots); e && nthreads; e = n) {
    నిర్మాణం thrdslot *tslot = Curl_కణుపు_elem(e);
    n = Curl_కణుపు_తదుపరి(e);
    ఒకవేళ(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    లేకపోతే ఒకవేళ(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  అయితే(nthreads && !ఫలితం &&
        Curl_llist_లెక్క(&tpool->slots) < tpool->max_దారాలు) {
    ఫలితం = thrdslot_ప్రారంభం(tpool);
    ఒకవేళ(ఫలితం)
      ఆపు;
    --nthreads;
  }

  తిరిగి ఫలితం;
}

CURLcode Curl_thrdpool_సమితి_props(నిర్మాణం curl_thrdpool *tpool,
                                 uint32_t min_దారాలు,
                                 uint32_t max_దారాలు,
                                 uint32_t idle_సమయం_ms)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t running;

  ఒకవేళ(!max_దారాలు || (min_దారాలు > max_దారాలు))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  Curl_mutex_acquire(&tpool->తాళం);
  tpool->min_దారాలు = min_దారాలు;
  tpool->max_దారాలు = max_దారాలు;
  tpool->idle_సమయం_ms = idle_సమయం_ms;
  running = Curl_llist_లెక్క(&tpool->slots);
  ఒకవేళ(tpool->min_దారాలు > running) {
    ఫలితం = thrdpool_signal(tpool, tpool->min_దారాలు - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->తాళం);

  తిరిగి ఫలితం;
}

CURLcode Curl_thrdpool_create(నిర్మాణం curl_thrdpool **ptpool,
                              స్థిరరాశి అక్షరం *name,
                              uint32_t min_దారాలు,
                              uint32_t max_దారాలు,
                              uint32_t idle_సమయం_ms,
                              Curl_thrdpool_take_అంశం_cb *fn_take,
                              Curl_thrdpool_process_అంశం_cb *fn_process,
                              Curl_thrdpool_return_అంశం_cb *fn_return,
                              శూన్య *user_దత్తాంశం)
{
  నిర్మాణం curl_thrdpool *tpool;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, పరిమాణం(*tpool));
  ఒకవేళ(!tpool)
    వెళ్ళు బహిర్గతం;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->తాళం);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, ఖాళీ);
  Curl_llist_init(&tpool->zombies, ఖాళీ);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_దత్తాంశం = user_దత్తాంశం;

  /* a const string that remains */
  tpool->name = name;

#నిర్వచితమైతే DEBUGBUILD
  {
    స్థిరరాశి అక్షరం *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    ఒకవేళ(p) {
      curl_off_t l;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, గరిష్ఠ_పూర్ణాంకం))
        tpool->dbg_fail_starts = (పూర్ణాంకం)l;
    }
  }
#అయితే_ముగింపు

  ఫలితం = Curl_thrdpool_సమితి_props(tpool, min_దారాలు, max_దారాలు,
                                   idle_సమయం_ms);

బహిర్గతం:
  ఒకవేళ(ఫలితం && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = ఖాళీ;
  }
  *ptpool = tpool;
  తిరిగి ఫలితం;
}

శూన్య Curl_thrdpool_destroy(నిర్మాణం curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->తాళం);

  tpool->aborted = TRUE;

  అయితే(join && Curl_llist_లెక్క(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->తాళం);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  ఒకవేళ(Curl_llist_లెక్క(&tpool->slots)) {
    నిర్మాణం Curl_llist_కణుపు *e;
    కొరకు(e = Curl_llist_తల(&tpool->slots); e; e = Curl_కణుపు_తదుపరి(e)) {
      నిర్మాణం thrdslot *tslot = Curl_కణుపు_elem(e);
      ఒకవేళ(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  ఒకవేళ(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->తాళం);
  }
}

CURLcode Curl_thrdpool_signal(నిర్మాణం curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode ఫలితం;

  Curl_mutex_acquire(&tpool->తాళం);
  ఫలితం = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->తాళం);
  తిరిగి ఫలితం;
}

స్థిర bool thrdpool_all_idle(నిర్మాణం curl_thrdpool *tpool)
{
  నిర్మాణం Curl_llist_కణుపు *e;
  కొరకు(e = Curl_llist_తల(&tpool->slots); e; e = Curl_కణుపు_తదుపరి(e)) {
    నిర్మాణం thrdslot *tslot = Curl_కణుపు_elem(e);
    ఒకవేళ(!tslot->idle)
      తిరిగి FALSE;
  }
  తిరిగి TRUE;
}

CURLcode Curl_thrdpool_await_idle(నిర్మాణం curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం curltime చివర = { 0 };

  Curl_mutex_acquire(&tpool->తాళం);
  DEBUGASSERT(!tpool->aborted);
  ఒకవేళ(tpool->aborted) {
    ఫలితం = CURLE_FAILED_INIT;
    వెళ్ళు బహిర్గతం;
  }

  అయితే(!thrdpool_all_idle(tpool)) {
    ఒకవేళ(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      ఒకవేళ(!చివర.tv_sec && !చివర.tv_usec) {
        చివర = curlx_now();
        చివర.tv_sec += (time_t)(timeout_ms / 1000);
        చివర.tv_usec += (పూర్ణాంకం)(timeout_ms % 1000) * 1000;
        ఒకవేళ(చివర.tv_usec >= 1000000) {
          చివర.tv_sec++;
          చివర.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), చివర);
      ఒకవేళ(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      లేకపోతే
        r = Curl_cond_timedwait(&tpool->await, &tpool->తాళం,
                                (uint32_t)remain_ms);
      ఒకవేళ(r == CURLE_OPERATION_TIMEDOUT) {
        ఫలితం = r;
        ఆపు;
      }
    }
    లేకపోతే {
      Curl_cond_wait(&tpool->await, &tpool->తాళం);
    }
  }

బహిర్గతం:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->తాళం);
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే CURLVERBOSE
శూన్య Curl_thrdpool_trace(నిర్మాణం curl_thrdpool *tpool,
                         నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం curl_trc_feat *feat = &Curl_trc_feat_దారాలు;
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, feat)) {
    నిర్మాణం Curl_llist_కణుపు *e;
    నిర్మాణం curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->తాళం);
    ఒకవేళ(!Curl_llist_లెక్క(&tpool->slots)) {
      Curl_trc_feat_infof(దత్తాంశం, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    కొరకు(e = Curl_llist_తల(&tpool->slots); e; e = Curl_కణుపు_తదుపరి(e)) {
      నిర్మాణం thrdslot *tslot = Curl_కణుపు_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      ఒకవేళ(!tslot->running) {
        Curl_trc_feat_infof(దత్తాంశం, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      లేకపోతే ఒకవేళ(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(దత్తాంశం, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      లేకపోతే ఒకవేళ(tslot->idle) {
        Curl_trc_feat_infof(దత్తాంశం, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      లేకపోతే {
        timediff_t remain_ms = tslot->work_timeout_ms ?
          (tslot->work_timeout_ms - elapsed_ms) : 0;
        Curl_trc_feat_infof(దత్తాంశం, feat, "[TPOOL-%s] [%u]: busy %"
                            FMT_TIMEDIFF_T "ms, timeout in %" FMT_TIMEDIFF_T
                            "ms: %s",
                            tpool->name, tslot->id, elapsed_ms, remain_ms,
                            tslot->work_description);
      }
    }
    Curl_mutex_release(&tpool->తాళం);
  }
}
#అయితే_ముగింపు

#అయితే_ముగింపు /* USE_THREADS */
