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
  संरचना Curl_llist_गाठ गाठ;
  संरचना curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  संरचना curltime starttime;
  स्थिरांक अक्षर *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  BIT(running);
  BIT(idle);
};

संरचना curl_thrdpool {
  स्थिरांक अक्षर *name;
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
  पूर्णांक dbg_fail_starts; /* fail this many thread starts */
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

  DEBUGASSERT(Curl_गाठ_llist(&tslot->गाठ) == &tpool->slots);
  Curl_गाठ_remove(&tslot->गाठ);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->गाठ);
  Curl_cond_signal(&tpool->await);
}

स्थिर CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_run(शून्य *arg)
{
  संरचना thrdslot *tslot = arg;
  संरचना curl_thrdpool *tpool = tslot->tpool;
  शून्य *item;

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(Curl_गाठ_llist(&tslot->गाठ) == &tpool->slots);
  साठी(;;) {
    पर्यंत(!tpool->aborted) {
      tslot->work_description = रिक्त;
      tslot->work_timeout_ms = 0;
      item = tpool->fn_take(tpool->fn_user_data, &tslot->work_description,
                            &tslot->work_timeout_ms);
      जर(!item)
        थांबा;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->lock);

      tpool->fn_process(item);

      Curl_mutex_acquire(&tpool->lock);
      tslot->work_description = रिक्त;
      tpool->fn_return(item, tpool->aborted ? रिक्त : tpool->fn_user_data);
    }

    जर(tpool->aborted ||
       (Curl_llist_मोजणी(&tpool->slots) > tpool->max_threads))
      जा out;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    जर((tpool->idle_time_ms > 0) &&
       (Curl_llist_मोजणी(&tpool->slots) > tpool->min_threads)) {
      CURLcode result = Curl_cond_timedwait(&tslot->await, &tpool->lock,
                                            tpool->idle_time_ms);
      जर((result == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_मोजणी(&tpool->slots) > tpool->min_threads)) {
        जा out;
      }
    }
    अन्यथा {
      Curl_cond_wait(&tslot->await, &tpool->lock);
    }
  }

out:
  thrdslot_done(tslot);
  जर(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
  परत 0;
}

स्थिर CURLcode thrdslot_start(संरचना curl_thrdpool *tpool)
{
  संरचना thrdslot *tslot;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  tslot = curlx_calloc(1, आकार(*tslot));
  जर(!tslot)
    जा out;
  tslot->id = tpool->next_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#ifdef DEBUGBUILD
  जर(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  अन्यथा
#endif
    tslot->thread = Curl_thread_create(thrdslot_run, tslot);
  जर(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    result = CURLE_FAILED_INIT;
    जा out;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->गाठ);
  tslot = रिक्त;
  result = CURLE_OK;

out:
  जर(tslot)
    thrdslot_destroy(tslot);
  परत result;
}

स्थिर शून्य thrdpool_wake_all(संरचना curl_thrdpool *tpool)
{
  संरचना Curl_llist_गाठ *e;
  साठी(e = Curl_llist_head(&tpool->slots); e; e = Curl_गाठ_next(e)) {
    संरचना thrdslot *tslot = Curl_गाठ_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

स्थिर शून्य thrdpool_join_zombies(संरचना curl_thrdpool *tpool)
{
  संरचना Curl_llist_गाठ *e;

  साठी(e = Curl_llist_head(&tpool->zombies); e;
      e = Curl_llist_head(&tpool->zombies)) {
    संरचना thrdslot *tslot = Curl_गाठ_elem(e);

    Curl_गाठ_remove(&tslot->गाठ);
    जर(tslot->thread != curl_thread_t_null) {
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
  जर(tpool->refcount)
    tpool->refcount--;
  जर(tpool->refcount)
    परत FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  जर(locked)
    Curl_mutex_release(&tpool->lock);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->lock);
  curlx_free(tpool);
  परत TRUE;
}

स्थिर CURLcode thrdpool_signal(संरचना curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  संरचना Curl_llist_गाठ *e, *n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  साठी(e = Curl_llist_head(&tpool->slots); e && nthreads; e = n) {
    संरचना thrdslot *tslot = Curl_गाठ_elem(e);
    n = Curl_गाठ_next(e);
    जर(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    अन्यथा जर(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  पर्यंत(nthreads && !result &&
        Curl_llist_मोजणी(&tpool->slots) < tpool->max_threads) {
    result = thrdslot_start(tpool);
    जर(result)
      थांबा;
    --nthreads;
  }

  परत result;
}

CURLcode Curl_thrdpool_set_props(संरचना curl_thrdpool *tpool,
                                 uint32_t min_threads,
                                 uint32_t max_threads,
                                 uint32_t idle_time_ms)
{
  CURLcode result = CURLE_OK;
  माप_प्रकार running;

  जर(!max_threads || (min_threads > max_threads))
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mutex_acquire(&tpool->lock);
  tpool->min_threads = min_threads;
  tpool->max_threads = max_threads;
  tpool->idle_time_ms = idle_time_ms;
  running = Curl_llist_मोजणी(&tpool->slots);
  जर(tpool->min_threads > running) {
    result = thrdpool_signal(tpool, tpool->min_threads - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->lock);

  परत result;
}

CURLcode Curl_thrdpool_create(संरचना curl_thrdpool **ptpool,
                              स्थिरांक अक्षर *name,
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

  tpool = curlx_calloc(1, आकार(*tpool));
  जर(!tpool)
    जा out;
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
    स्थिरांक अक्षर *p = दो_पर्या("CURL_DBG_THRDPOOL_FAIL_STARTS");
    जर(p) {
      curl_off_t l;
      जर(!curlx_str_संख्या(&p, &l, पूर्णांक_उच्च))
        tpool->dbg_fail_starts = (पूर्णांक)l;
    }
  }
#endif

  result = Curl_thrdpool_set_props(tpool, min_threads, max_threads,
                                   idle_time_ms);

out:
  जर(result && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = रिक्त;
  }
  *ptpool = tpool;
  परत result;
}

शून्य Curl_thrdpool_destroy(संरचना curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->lock);

  tpool->aborted = TRUE;

  पर्यंत(join && Curl_llist_मोजणी(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->lock);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  जर(Curl_llist_मोजणी(&tpool->slots)) {
    संरचना Curl_llist_गाठ *e;
    साठी(e = Curl_llist_head(&tpool->slots); e; e = Curl_गाठ_next(e)) {
      संरचना thrdslot *tslot = Curl_गाठ_elem(e);
      जर(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  जर(!thrdpool_unlink(tpool, TRUE)) {
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
  परत result;
}

स्थिर bool thrdpool_all_idle(संरचना curl_thrdpool *tpool)
{
  संरचना Curl_llist_गाठ *e;
  साठी(e = Curl_llist_head(&tpool->slots); e; e = Curl_गाठ_next(e)) {
    संरचना thrdslot *tslot = Curl_गाठ_elem(e);
    जर(!tslot->idle)
      परत FALSE;
  }
  परत TRUE;
}

CURLcode Curl_thrdpool_await_idle(संरचना curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode result = CURLE_OK;
  संरचना curltime end = { 0 };

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(!tpool->aborted);
  जर(tpool->aborted) {
    result = CURLE_FAILED_INIT;
    जा out;
  }

  पर्यंत(!thrdpool_all_idle(tpool)) {
    जर(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      जर(!end.tv_sec && !end.tv_usec) {
        end = curlx_now();
        end.tv_sec += (समय_प्रकार)(timeout_ms / 1000);
        end.tv_usec += (पूर्णांक)(timeout_ms % 1000) * 1000;
        जर(end.tv_usec >= 1000000) {
          end.tv_sec++;
          end.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), end);
      जर(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      अन्यथा
        r = Curl_cond_timedwait(&tpool->await, &tpool->lock,
                                (uint32_t)remain_ms);
      जर(r == CURLE_OPERATION_TIMEDOUT) {
        result = r;
        थांबा;
      }
    }
    अन्यथा {
      Curl_cond_wait(&tpool->await, &tpool->lock);
    }
  }

out:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->lock);
  परत result;
}

#ifdef CURLVERBOSE
शून्य Curl_thrdpool_trace(संरचना curl_thrdpool *tpool,
                         संरचना Curl_easy *data)
{
  संरचना curl_trc_feat *feat = &Curl_trc_feat_threads;
  जर(Curl_trc_ft_is_verbose(data, feat)) {
    संरचना Curl_llist_गाठ *e;
    संरचना curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->lock);
    जर(!Curl_llist_मोजणी(&tpool->slots)) {
      Curl_trc_feat_infof(data, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    साठी(e = Curl_llist_head(&tpool->slots); e; e = Curl_गाठ_next(e)) {
      संरचना thrdslot *tslot = Curl_गाठ_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      जर(!tslot->running) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      अन्यथा जर(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      अन्यथा जर(tslot->idle) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      अन्यथा {
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
