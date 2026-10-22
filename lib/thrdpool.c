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
#شامل "curl_setup.h"

#ifdef USE_THREADS

#شامل "llist.h"
#شامل "curl_threads.h"
#شامل "curlx/timeval.h"
#شامل "curlx/strparse.h"
#شامل "thrdpool.h"
#ifdef CURLVERBOSE
#شامل "curl_trc.h"
#شامل "urldata.h"
#endif


ساختار thrdslot {
  ساختار Curl_llist_گره گره;
  ساختار curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  ساختار curltime starttime;
  ثابت نویسه *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  BIT(running);
  BIT(idle);
};

ساختار curl_thrdpool {
  ثابت نویسه *name;
  uint64_t refcount;
  curl_mutex_t lock;
  curl_cond_t await;
  ساختار Curl_llist slots;
  ساختار Curl_llist zombies;
  Curl_thrdpool_take_item_cb *fn_take;
  Curl_thrdpool_process_item_cb *fn_process;
  Curl_thrdpool_return_item_cb *fn_return;
  تهی *fn_user_data;
  CURLcode fatal_err;
  uint32_t min_threads;
  uint32_t max_threads;
  uint32_t idle_time_ms;
  uint32_t next_id;
#ifdef DEBUGBUILD
  صحیح dbg_fail_starts; /* fail this many thread starts */
#endif
  BIT(aborted);
  BIT(detached);
};

ایستا تهی thrdpool_join_zombies(ساختار curl_thrdpool *tpool);
ایستا bool thrdpool_unlink(ساختار curl_thrdpool *tpool, bool locked);

ایستا تهی thrdslot_destroy(ساختار thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

ایستا تهی thrdslot_done(ساختار thrdslot *tslot)
{
  ساختار curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_گره_llist(&tslot->گره) == &tpool->slots);
  Curl_گره_remove(&tslot->گره);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->گره);
  Curl_cond_signal(&tpool->await);
}

ایستا CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_run(تهی *arg)
{
  ساختار thrdslot *tslot = arg;
  ساختار curl_thrdpool *tpool = tslot->tpool;
  تهی *item;

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(Curl_گره_llist(&tslot->گره) == &tpool->slots);
  برای(;;) {
    وقتی(!tpool->aborted) {
      tslot->work_description = پوچ;
      tslot->work_timeout_ms = 0;
      item = tpool->fn_take(tpool->fn_user_data, &tslot->work_description,
                            &tslot->work_timeout_ms);
      اگر(!item)
        بشکن;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->lock);

      tpool->fn_process(item);

      Curl_mutex_acquire(&tpool->lock);
      tslot->work_description = پوچ;
      tpool->fn_return(item, tpool->aborted ? پوچ : tpool->fn_user_data);
    }

    اگر(tpool->aborted ||
       (Curl_llist_شمار(&tpool->slots) > tpool->max_threads))
      برو out;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    اگر((tpool->idle_time_ms > 0) &&
       (Curl_llist_شمار(&tpool->slots) > tpool->min_threads)) {
      CURLcode result = Curl_cond_timedwait(&tslot->await, &tpool->lock,
                                            tpool->idle_time_ms);
      اگر((result == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_شمار(&tpool->slots) > tpool->min_threads)) {
        برو out;
      }
    }
    وگرنه {
      Curl_cond_wait(&tslot->await, &tpool->lock);
    }
  }

out:
  thrdslot_done(tslot);
  اگر(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
  بازگشت 0;
}

ایستا CURLcode thrdslot_start(ساختار curl_thrdpool *tpool)
{
  ساختار thrdslot *tslot;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  tslot = curlx_calloc(1, اندازه(*tslot));
  اگر(!tslot)
    برو out;
  tslot->id = tpool->next_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#ifdef DEBUGBUILD
  اگر(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  وگرنه
#endif
    tslot->thread = Curl_thread_create(thrdslot_run, tslot);
  اگر(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    result = CURLE_FAILED_INIT;
    برو out;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->گره);
  tslot = پوچ;
  result = CURLE_OK;

out:
  اگر(tslot)
    thrdslot_destroy(tslot);
  بازگشت result;
}

ایستا تهی thrdpool_wake_all(ساختار curl_thrdpool *tpool)
{
  ساختار Curl_llist_گره *e;
  برای(e = Curl_llist_head(&tpool->slots); e; e = Curl_گره_next(e)) {
    ساختار thrdslot *tslot = Curl_گره_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

ایستا تهی thrdpool_join_zombies(ساختار curl_thrdpool *tpool)
{
  ساختار Curl_llist_گره *e;

  برای(e = Curl_llist_head(&tpool->zombies); e;
      e = Curl_llist_head(&tpool->zombies)) {
    ساختار thrdslot *tslot = Curl_گره_elem(e);

    Curl_گره_remove(&tslot->گره);
    اگر(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->lock);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->lock);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

ایستا bool thrdpool_unlink(ساختار curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  اگر(tpool->refcount)
    tpool->refcount--;
  اگر(tpool->refcount)
    بازگشت FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  اگر(locked)
    Curl_mutex_release(&tpool->lock);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->lock);
  curlx_free(tpool);
  بازگشت TRUE;
}

ایستا CURLcode thrdpool_signal(ساختار curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  ساختار Curl_llist_گره *e, *n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  برای(e = Curl_llist_head(&tpool->slots); e && nthreads; e = n) {
    ساختار thrdslot *tslot = Curl_گره_elem(e);
    n = Curl_گره_next(e);
    اگر(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    وگرنه اگر(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  وقتی(nthreads && !result &&
        Curl_llist_شمار(&tpool->slots) < tpool->max_threads) {
    result = thrdslot_start(tpool);
    اگر(result)
      بشکن;
    --nthreads;
  }

  بازگشت result;
}

CURLcode Curl_thrdpool_set_props(ساختار curl_thrdpool *tpool,
                                 uint32_t min_threads,
                                 uint32_t max_threads,
                                 uint32_t idle_time_ms)
{
  CURLcode result = CURLE_OK;
  size_t running;

  اگر(!max_threads || (min_threads > max_threads))
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mutex_acquire(&tpool->lock);
  tpool->min_threads = min_threads;
  tpool->max_threads = max_threads;
  tpool->idle_time_ms = idle_time_ms;
  running = Curl_llist_شمار(&tpool->slots);
  اگر(tpool->min_threads > running) {
    result = thrdpool_signal(tpool, tpool->min_threads - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->lock);

  بازگشت result;
}

CURLcode Curl_thrdpool_create(ساختار curl_thrdpool **ptpool,
                              ثابت نویسه *name,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms,
                              Curl_thrdpool_take_item_cb *fn_take,
                              Curl_thrdpool_process_item_cb *fn_process,
                              Curl_thrdpool_return_item_cb *fn_return,
                              تهی *user_data)
{
  ساختار curl_thrdpool *tpool;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, اندازه(*tpool));
  اگر(!tpool)
    برو out;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->lock);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, پوچ);
  Curl_llist_init(&tpool->zombies, پوچ);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_data = user_data;

  /* a const string that remains */
  tpool->name = name;

#ifdef DEBUGBUILD
  {
    ثابت نویسه *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_عدد(&p, &l, INT_MAX))
        tpool->dbg_fail_starts = (صحیح)l;
    }
  }
#endif

  result = Curl_thrdpool_set_props(tpool, min_threads, max_threads,
                                   idle_time_ms);

out:
  اگر(result && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = پوچ;
  }
  *ptpool = tpool;
  بازگشت result;
}

تهی Curl_thrdpool_destroy(ساختار curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->lock);

  tpool->aborted = TRUE;

  وقتی(join && Curl_llist_شمار(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->lock);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  اگر(Curl_llist_شمار(&tpool->slots)) {
    ساختار Curl_llist_گره *e;
    برای(e = Curl_llist_head(&tpool->slots); e; e = Curl_گره_next(e)) {
      ساختار thrdslot *tslot = Curl_گره_elem(e);
      اگر(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  اگر(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
}

CURLcode Curl_thrdpool_signal(ساختار curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode result;

  Curl_mutex_acquire(&tpool->lock);
  result = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->lock);
  بازگشت result;
}

ایستا bool thrdpool_all_idle(ساختار curl_thrdpool *tpool)
{
  ساختار Curl_llist_گره *e;
  برای(e = Curl_llist_head(&tpool->slots); e; e = Curl_گره_next(e)) {
    ساختار thrdslot *tslot = Curl_گره_elem(e);
    اگر(!tslot->idle)
      بازگشت FALSE;
  }
  بازگشت TRUE;
}

CURLcode Curl_thrdpool_await_idle(ساختار curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode result = CURLE_OK;
  ساختار curltime end = { 0 };

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(!tpool->aborted);
  اگر(tpool->aborted) {
    result = CURLE_FAILED_INIT;
    برو out;
  }

  وقتی(!thrdpool_all_idle(tpool)) {
    اگر(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      اگر(!end.tv_sec && !end.tv_usec) {
        end = curlx_now();
        end.tv_sec += (time_t)(timeout_ms / 1000);
        end.tv_usec += (صحیح)(timeout_ms % 1000) * 1000;
        اگر(end.tv_usec >= 1000000) {
          end.tv_sec++;
          end.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), end);
      اگر(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      وگرنه
        r = Curl_cond_timedwait(&tpool->await, &tpool->lock,
                                (uint32_t)remain_ms);
      اگر(r == CURLE_OPERATION_TIMEDOUT) {
        result = r;
        بشکن;
      }
    }
    وگرنه {
      Curl_cond_wait(&tpool->await, &tpool->lock);
    }
  }

out:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->lock);
  بازگشت result;
}

#ifdef CURLVERBOSE
تهی Curl_thrdpool_trace(ساختار curl_thrdpool *tpool,
                         ساختار Curl_easy *data)
{
  ساختار curl_trc_feat *feat = &Curl_trc_feat_threads;
  اگر(Curl_trc_ft_is_verbose(data, feat)) {
    ساختار Curl_llist_گره *e;
    ساختار curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->lock);
    اگر(!Curl_llist_شمار(&tpool->slots)) {
      Curl_trc_feat_infof(data, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    برای(e = Curl_llist_head(&tpool->slots); e; e = Curl_گره_next(e)) {
      ساختار thrdslot *tslot = Curl_گره_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      اگر(!tslot->running) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      وگرنه اگر(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      وگرنه اگر(tslot->idle) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      وگرنه {
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
