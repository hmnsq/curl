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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_THREADS

#அந்தர்பாவ "llist.h"
#அந்தர்பாவ "curl_threads.h"
#அந்தர்பாவ "curlx/timeval.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "thrdpool.h"
#ifdef CURLVERBOSE
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "urldata.h"
#endif


கட்டமைப்பு thrdslot {
  கட்டமைப்பு Curl_llist_முனை முனை;
  கட்டமைப்பு curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  கட்டமைப்பு curltime starttime;
  மாறிலி எழுத்து *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  BIT(running);
  BIT(idle);
};

கட்டமைப்பு curl_thrdpool {
  மாறிலி எழுத்து *name;
  uint64_t refcount;
  curl_mutex_t lock;
  curl_cond_t await;
  கட்டமைப்பு Curl_llist slots;
  கட்டமைப்பு Curl_llist zombies;
  Curl_thrdpool_take_item_cb *fn_take;
  Curl_thrdpool_process_item_cb *fn_process;
  Curl_thrdpool_return_item_cb *fn_return;
  வெறுமை *fn_user_data;
  CURLcode fatal_err;
  uint32_t min_threads;
  uint32_t max_threads;
  uint32_t idle_time_ms;
  uint32_t next_id;
#ifdef DEBUGBUILD
  முழு dbg_fail_starts; /* fail this many thread starts */
#endif
  BIT(aborted);
  BIT(detached);
};

நிலையான வெறுமை thrdpool_join_zombies(கட்டமைப்பு curl_thrdpool *tpool);
நிலையான bool thrdpool_unlink(கட்டமைப்பு curl_thrdpool *tpool, bool locked);

நிலையான வெறுமை thrdslot_destroy(கட்டமைப்பு thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

நிலையான வெறுமை thrdslot_done(கட்டமைப்பு thrdslot *tslot)
{
  கட்டமைப்பு curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_முனை_llist(&tslot->முனை) == &tpool->slots);
  Curl_முனை_remove(&tslot->முனை);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->முனை);
  Curl_cond_signal(&tpool->await);
}

நிலையான CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_run(வெறுமை *arg)
{
  கட்டமைப்பு thrdslot *tslot = arg;
  கட்டமைப்பு curl_thrdpool *tpool = tslot->tpool;
  வெறுமை *item;

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(Curl_முனை_llist(&tslot->முனை) == &tpool->slots);
  க்கு(;;) {
    போது(!tpool->aborted) {
      tslot->work_description = வெற்று;
      tslot->work_timeout_ms = 0;
      item = tpool->fn_take(tpool->fn_user_data, &tslot->work_description,
                            &tslot->work_timeout_ms);
      எனில்(!item)
        நிறுத்து;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->lock);

      tpool->fn_process(item);

      Curl_mutex_acquire(&tpool->lock);
      tslot->work_description = வெற்று;
      tpool->fn_return(item, tpool->aborted ? வெற்று : tpool->fn_user_data);
    }

    எனில்(tpool->aborted ||
       (Curl_llist_மொத்தம்(&tpool->slots) > tpool->max_threads))
      செல் out;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    எனில்((tpool->idle_time_ms > 0) &&
       (Curl_llist_மொத்தம்(&tpool->slots) > tpool->min_threads)) {
      CURLcode result = Curl_cond_timedwait(&tslot->await, &tpool->lock,
                                            tpool->idle_time_ms);
      எனில்((result == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_மொத்தம்(&tpool->slots) > tpool->min_threads)) {
        செல் out;
      }
    }
    இல்லையெனில் {
      Curl_cond_wait(&tslot->await, &tpool->lock);
    }
  }

out:
  thrdslot_done(tslot);
  எனில்(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
  திரும்பு 0;
}

நிலையான CURLcode thrdslot_start(கட்டமைப்பு curl_thrdpool *tpool)
{
  கட்டமைப்பு thrdslot *tslot;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  tslot = curlx_calloc(1, அளவு(*tslot));
  எனில்(!tslot)
    செல் out;
  tslot->id = tpool->next_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#ifdef DEBUGBUILD
  எனில்(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  இல்லையெனில்
#endif
    tslot->thread = Curl_thread_create(thrdslot_run, tslot);
  எனில்(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    result = CURLE_FAILED_INIT;
    செல் out;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->முனை);
  tslot = வெற்று;
  result = CURLE_OK;

out:
  எனில்(tslot)
    thrdslot_destroy(tslot);
  திரும்பு result;
}

நிலையான வெறுமை thrdpool_wake_all(கட்டமைப்பு curl_thrdpool *tpool)
{
  கட்டமைப்பு Curl_llist_முனை *e;
  க்கு(e = Curl_llist_head(&tpool->slots); e; e = Curl_முனை_next(e)) {
    கட்டமைப்பு thrdslot *tslot = Curl_முனை_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

நிலையான வெறுமை thrdpool_join_zombies(கட்டமைப்பு curl_thrdpool *tpool)
{
  கட்டமைப்பு Curl_llist_முனை *e;

  க்கு(e = Curl_llist_head(&tpool->zombies); e;
      e = Curl_llist_head(&tpool->zombies)) {
    கட்டமைப்பு thrdslot *tslot = Curl_முனை_elem(e);

    Curl_முனை_remove(&tslot->முனை);
    எனில்(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->lock);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->lock);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

நிலையான bool thrdpool_unlink(கட்டமைப்பு curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  எனில்(tpool->refcount)
    tpool->refcount--;
  எனில்(tpool->refcount)
    திரும்பு FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  எனில்(locked)
    Curl_mutex_release(&tpool->lock);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->lock);
  curlx_free(tpool);
  திரும்பு TRUE;
}

நிலையான CURLcode thrdpool_signal(கட்டமைப்பு curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  கட்டமைப்பு Curl_llist_முனை *e, *n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  க்கு(e = Curl_llist_head(&tpool->slots); e && nthreads; e = n) {
    கட்டமைப்பு thrdslot *tslot = Curl_முனை_elem(e);
    n = Curl_முனை_next(e);
    எனில்(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    இல்லையெனில் எனில்(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  போது(nthreads && !result &&
        Curl_llist_மொத்தம்(&tpool->slots) < tpool->max_threads) {
    result = thrdslot_start(tpool);
    எனில்(result)
      நிறுத்து;
    --nthreads;
  }

  திரும்பு result;
}

CURLcode Curl_thrdpool_set_props(கட்டமைப்பு curl_thrdpool *tpool,
                                 uint32_t min_threads,
                                 uint32_t max_threads,
                                 uint32_t idle_time_ms)
{
  CURLcode result = CURLE_OK;
  size_t running;

  எனில்(!max_threads || (min_threads > max_threads))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mutex_acquire(&tpool->lock);
  tpool->min_threads = min_threads;
  tpool->max_threads = max_threads;
  tpool->idle_time_ms = idle_time_ms;
  running = Curl_llist_மொத்தம்(&tpool->slots);
  எனில்(tpool->min_threads > running) {
    result = thrdpool_signal(tpool, tpool->min_threads - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->lock);

  திரும்பு result;
}

CURLcode Curl_thrdpool_create(கட்டமைப்பு curl_thrdpool **ptpool,
                              மாறிலி எழுத்து *name,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms,
                              Curl_thrdpool_take_item_cb *fn_take,
                              Curl_thrdpool_process_item_cb *fn_process,
                              Curl_thrdpool_return_item_cb *fn_return,
                              வெறுமை *user_data)
{
  கட்டமைப்பு curl_thrdpool *tpool;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, அளவு(*tpool));
  எனில்(!tpool)
    செல் out;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->lock);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, வெற்று);
  Curl_llist_init(&tpool->zombies, வெற்று);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_data = user_data;

  /* a const string that remains */
  tpool->name = name;

#ifdef DEBUGBUILD
  {
    மாறிலி எழுத்து *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    எனில்(p) {
      curl_off_t l;
      எனில்(!curlx_str_எண்(&p, &l, INT_MAX))
        tpool->dbg_fail_starts = (முழு)l;
    }
  }
#endif

  result = Curl_thrdpool_set_props(tpool, min_threads, max_threads,
                                   idle_time_ms);

out:
  எனில்(result && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = வெற்று;
  }
  *ptpool = tpool;
  திரும்பு result;
}

வெறுமை Curl_thrdpool_destroy(கட்டமைப்பு curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->lock);

  tpool->aborted = TRUE;

  போது(join && Curl_llist_மொத்தம்(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->lock);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  எனில்(Curl_llist_மொத்தம்(&tpool->slots)) {
    கட்டமைப்பு Curl_llist_முனை *e;
    க்கு(e = Curl_llist_head(&tpool->slots); e; e = Curl_முனை_next(e)) {
      கட்டமைப்பு thrdslot *tslot = Curl_முனை_elem(e);
      எனில்(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  எனில்(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
}

CURLcode Curl_thrdpool_signal(கட்டமைப்பு curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode result;

  Curl_mutex_acquire(&tpool->lock);
  result = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->lock);
  திரும்பு result;
}

நிலையான bool thrdpool_all_idle(கட்டமைப்பு curl_thrdpool *tpool)
{
  கட்டமைப்பு Curl_llist_முனை *e;
  க்கு(e = Curl_llist_head(&tpool->slots); e; e = Curl_முனை_next(e)) {
    கட்டமைப்பு thrdslot *tslot = Curl_முனை_elem(e);
    எனில்(!tslot->idle)
      திரும்பு FALSE;
  }
  திரும்பு TRUE;
}

CURLcode Curl_thrdpool_await_idle(கட்டமைப்பு curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு curltime end = { 0 };

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(!tpool->aborted);
  எனில்(tpool->aborted) {
    result = CURLE_FAILED_INIT;
    செல் out;
  }

  போது(!thrdpool_all_idle(tpool)) {
    எனில்(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      எனில்(!end.tv_sec && !end.tv_usec) {
        end = curlx_now();
        end.tv_sec += (time_t)(timeout_ms / 1000);
        end.tv_usec += (முழு)(timeout_ms % 1000) * 1000;
        எனில்(end.tv_usec >= 1000000) {
          end.tv_sec++;
          end.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), end);
      எனில்(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      இல்லையெனில்
        r = Curl_cond_timedwait(&tpool->await, &tpool->lock,
                                (uint32_t)remain_ms);
      எனில்(r == CURLE_OPERATION_TIMEDOUT) {
        result = r;
        நிறுத்து;
      }
    }
    இல்லையெனில் {
      Curl_cond_wait(&tpool->await, &tpool->lock);
    }
  }

out:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->lock);
  திரும்பு result;
}

#ifdef CURLVERBOSE
வெறுமை Curl_thrdpool_trace(கட்டமைப்பு curl_thrdpool *tpool,
                         கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு curl_trc_feat *feat = &Curl_trc_feat_threads;
  எனில்(Curl_trc_ft_is_verbose(data, feat)) {
    கட்டமைப்பு Curl_llist_முனை *e;
    கட்டமைப்பு curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->lock);
    எனில்(!Curl_llist_மொத்தம்(&tpool->slots)) {
      Curl_trc_feat_infof(data, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    க்கு(e = Curl_llist_head(&tpool->slots); e; e = Curl_முனை_next(e)) {
      கட்டமைப்பு thrdslot *tslot = Curl_முனை_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      எனில்(!tslot->running) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      இல்லையெனில் எனில்(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      இல்லையெனில் எனில்(tslot->idle) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      இல்லையெனில் {
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
