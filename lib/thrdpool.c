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
#अन्तर्भाव "curlx/timeval.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "thrdpool.h"
#ifdef CURLVERBOSE
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "urldata.h"
#endif


संरचना thrdslot {
  संरचना Curl_llist_ग्रन्थिः ग्रन्थिः;
  संरचना curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  संरचना curltime starttime;
  ध्रुव वर्ण *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  BIT(running);
  BIT(idle);
};

संरचना curl_thrdpool {
  ध्रुव वर्ण *name;
  uint64_t refcount;
  curl_mutex_t lock;
  curl_cond_t await;
  संरचना Curl_llist slots;
  संरचना Curl_llist zombies;
  Curl_thrdpool_take_item_cb *fn_take;
  Curl_thrdpool_process_item_cb *fn_process;
  Curl_thrdpool_return_item_cb *fn_return;
  शून्य *fn_user_data;
  CURLcode fatal_err;
  uint32_t min_threads;
  uint32_t max_threads;
  uint32_t idle_time_ms;
  uint32_t next_id;
#ifdef DEBUGBUILD
  पूर्णाङ्क dbg_fail_starts; /* fail this many thread starts */
#endif
  BIT(aborted);
  BIT(detached);
};

स्थिर शून्य thrdpool_join_zombies(संरचना curl_thrdpool *tpool);
स्थिर bool thrdpool_unlink(संरचना curl_thrdpool *tpool, bool locked);

स्थिर शून्य thrdslot_destroy(संरचना thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

स्थिर शून्य thrdslot_done(संरचना thrdslot *tslot)
{
  संरचना curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_ग्रन्थिः_llist(&tslot->ग्रन्थिः) == &tpool->slots);
  Curl_ग्रन्थिः_remove(&tslot->ग्रन्थिः);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->ग्रन्थिः);
  Curl_cond_signal(&tpool->await);
}

स्थिर CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_run(शून्य *arg)
{
  संरचना thrdslot *tslot = arg;
  संरचना curl_thrdpool *tpool = tslot->tpool;
  शून्य *item;

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(Curl_ग्रन्थिः_llist(&tslot->ग्रन्थिः) == &tpool->slots);
  अत्थाय(;;) {
    याव(!tpool->aborted) {
      tslot->work_description = रिक्त;
      tslot->work_timeout_ms = 0;
      item = tpool->fn_take(tpool->fn_user_data, &tslot->work_description,
                            &tslot->work_timeout_ms);
      सचे(!item)
        भङ्ग;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->lock);

      tpool->fn_process(item);

      Curl_mutex_acquire(&tpool->lock);
      tslot->work_description = रिक्त;
      tpool->fn_return(item, tpool->aborted ? रिक्त : tpool->fn_user_data);
    }

    सचे(tpool->aborted ||
       (Curl_llist_गणनम्(&tpool->slots) > tpool->max_threads))
      गच्छ out;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    सचे((tpool->idle_time_ms > 0) &&
       (Curl_llist_गणनम्(&tpool->slots) > tpool->min_threads)) {
      CURLcode result = Curl_cond_timedwait(&tslot->await, &tpool->lock,
                                            tpool->idle_time_ms);
      सचे((result == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_गणनम्(&tpool->slots) > tpool->min_threads)) {
        गच्छ out;
      }
    }
    अञ्ञथा {
      Curl_cond_wait(&tslot->await, &tpool->lock);
    }
  }

out:
  thrdslot_done(tslot);
  सचे(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
  पटिनिवत्त 0;
}

स्थिर CURLcode thrdslot_start(संरचना curl_thrdpool *tpool)
{
  संरचना thrdslot *tslot;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  tslot = curlx_calloc(1, परिमाण(*tslot));
  सचे(!tslot)
    गच्छ out;
  tslot->id = tpool->next_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#ifdef DEBUGBUILD
  सचे(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  अञ्ञथा
#endif
    tslot->thread = Curl_thread_create(thrdslot_run, tslot);
  सचे(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    result = CURLE_FAILED_INIT;
    गच्छ out;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->ग्रन्थिः);
  tslot = रिक्त;
  result = CURLE_OK;

out:
  सचे(tslot)
    thrdslot_destroy(tslot);
  पटिनिवत्त result;
}

स्थिर शून्य thrdpool_wake_all(संरचना curl_thrdpool *tpool)
{
  संरचना Curl_llist_ग्रन्थिः *e;
  अत्थाय(e = Curl_llist_head(&tpool->slots); e; e = Curl_ग्रन्थिः_next(e)) {
    संरचना thrdslot *tslot = Curl_ग्रन्थिः_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

स्थिर शून्य thrdpool_join_zombies(संरचना curl_thrdpool *tpool)
{
  संरचना Curl_llist_ग्रन्थिः *e;

  अत्थाय(e = Curl_llist_head(&tpool->zombies); e;
      e = Curl_llist_head(&tpool->zombies)) {
    संरचना thrdslot *tslot = Curl_ग्रन्थिः_elem(e);

    Curl_ग्रन्थिः_remove(&tslot->ग्रन्थिः);
    सचे(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->lock);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->lock);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

स्थिर bool thrdpool_unlink(संरचना curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  सचे(tpool->refcount)
    tpool->refcount--;
  सचे(tpool->refcount)
    पटिनिवत्त FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  सचे(locked)
    Curl_mutex_release(&tpool->lock);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->lock);
  curlx_free(tpool);
  पटिनिवत्त TRUE;
}

स्थिर CURLcode thrdpool_signal(संरचना curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  संरचना Curl_llist_ग्रन्थिः *e, *n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  अत्थाय(e = Curl_llist_head(&tpool->slots); e && nthreads; e = n) {
    संरचना thrdslot *tslot = Curl_ग्रन्थिः_elem(e);
    n = Curl_ग्रन्थिः_next(e);
    सचे(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    अञ्ञथा सचे(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  याव(nthreads && !result &&
        Curl_llist_गणनम्(&tpool->slots) < tpool->max_threads) {
    result = thrdslot_start(tpool);
    सचे(result)
      भङ्ग;
    --nthreads;
  }

  पटिनिवत्त result;
}

CURLcode Curl_thrdpool_set_props(संरचना curl_thrdpool *tpool,
                                 uint32_t min_threads,
                                 uint32_t max_threads,
                                 uint32_t idle_time_ms)
{
  CURLcode result = CURLE_OK;
  size_t running;

  सचे(!max_threads || (min_threads > max_threads))
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mutex_acquire(&tpool->lock);
  tpool->min_threads = min_threads;
  tpool->max_threads = max_threads;
  tpool->idle_time_ms = idle_time_ms;
  running = Curl_llist_गणनम्(&tpool->slots);
  सचे(tpool->min_threads > running) {
    result = thrdpool_signal(tpool, tpool->min_threads - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->lock);

  पटिनिवत्त result;
}

CURLcode Curl_thrdpool_create(संरचना curl_thrdpool **ptpool,
                              ध्रुव वर्ण *name,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms,
                              Curl_thrdpool_take_item_cb *fn_take,
                              Curl_thrdpool_process_item_cb *fn_process,
                              Curl_thrdpool_return_item_cb *fn_return,
                              शून्य *user_data)
{
  संरचना curl_thrdpool *tpool;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, परिमाण(*tpool));
  सचे(!tpool)
    गच्छ out;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->lock);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, रिक्त);
  Curl_llist_init(&tpool->zombies, रिक्त);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_data = user_data;

  /* a const string that remains */
  tpool->name = name;

#ifdef DEBUGBUILD
  {
    ध्रुव वर्ण *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    सचे(p) {
      curl_off_t l;
      सचे(!curlx_str_सङ्ख्या(&p, &l, INT_MAX))
        tpool->dbg_fail_starts = (पूर्णाङ्क)l;
    }
  }
#endif

  result = Curl_thrdpool_set_props(tpool, min_threads, max_threads,
                                   idle_time_ms);

out:
  सचे(result && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = रिक्त;
  }
  *ptpool = tpool;
  पटिनिवत्त result;
}

शून्य Curl_thrdpool_destroy(संरचना curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->lock);

  tpool->aborted = TRUE;

  याव(join && Curl_llist_गणनम्(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->lock);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  सचे(Curl_llist_गणनम्(&tpool->slots)) {
    संरचना Curl_llist_ग्रन्थिः *e;
    अत्थाय(e = Curl_llist_head(&tpool->slots); e; e = Curl_ग्रन्थिः_next(e)) {
      संरचना thrdslot *tslot = Curl_ग्रन्थिः_elem(e);
      सचे(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  सचे(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
}

CURLcode Curl_thrdpool_signal(संरचना curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode result;

  Curl_mutex_acquire(&tpool->lock);
  result = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->lock);
  पटिनिवत्त result;
}

स्थिर bool thrdpool_all_idle(संरचना curl_thrdpool *tpool)
{
  संरचना Curl_llist_ग्रन्थिः *e;
  अत्थाय(e = Curl_llist_head(&tpool->slots); e; e = Curl_ग्रन्थिः_next(e)) {
    संरचना thrdslot *tslot = Curl_ग्रन्थिः_elem(e);
    सचे(!tslot->idle)
      पटिनिवत्त FALSE;
  }
  पटिनिवत्त TRUE;
}

CURLcode Curl_thrdpool_await_idle(संरचना curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode result = CURLE_OK;
  संरचना curltime end = { 0 };

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(!tpool->aborted);
  सचे(tpool->aborted) {
    result = CURLE_FAILED_INIT;
    गच्छ out;
  }

  याव(!thrdpool_all_idle(tpool)) {
    सचे(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      सचे(!end.tv_sec && !end.tv_usec) {
        end = curlx_now();
        end.tv_sec += (time_t)(timeout_ms / 1000);
        end.tv_usec += (पूर्णाङ्क)(timeout_ms % 1000) * 1000;
        सचे(end.tv_usec >= 1000000) {
          end.tv_sec++;
          end.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), end);
      सचे(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      अञ्ञथा
        r = Curl_cond_timedwait(&tpool->await, &tpool->lock,
                                (uint32_t)remain_ms);
      सचे(r == CURLE_OPERATION_TIMEDOUT) {
        result = r;
        भङ्ग;
      }
    }
    अञ्ञथा {
      Curl_cond_wait(&tpool->await, &tpool->lock);
    }
  }

out:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->lock);
  पटिनिवत्त result;
}

#ifdef CURLVERBOSE
शून्य Curl_thrdpool_trace(संरचना curl_thrdpool *tpool,
                         संरचना Curl_easy *data)
{
  संरचना curl_trc_feat *feat = &Curl_trc_feat_threads;
  सचे(Curl_trc_ft_is_verbose(data, feat)) {
    संरचना Curl_llist_ग्रन्थिः *e;
    संरचना curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->lock);
    सचे(!Curl_llist_गणनम्(&tpool->slots)) {
      Curl_trc_feat_infof(data, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    अत्थाय(e = Curl_llist_head(&tpool->slots); e; e = Curl_ग्रन्थिः_next(e)) {
      संरचना thrdslot *tslot = Curl_ग्रन्थिः_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      सचे(!tslot->running) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      अञ्ञथा सचे(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      अञ्ञथा सचे(tslot->idle) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      अञ्ञथा {
        timediff_t remain_ms = tslot->work_timeout_ms ?
          (tslot->work_timeout_ms - elapsed_ms) : 0;
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: busy %"
                            FMT_TIMEDIFF_T "ms, timeout in %" FMT_TIMEDIFF_T
                            "ms: %s",
                            tpool->name, tslot->id, elapsed_ms, remain_ms,
                            tslot->work_description);
      }
    }
    Curl_mutex_release(&tpool->lock);
  }
}
#endif

#endif /* USE_THREADS */
