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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef USE_THREADS

#ಅನ್ತರ್ಭಾವ "llist.h"
#ಅನ್ತರ್ಭಾವ "curl_threads.h"
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "thrdpool.h"
#ifdef CURLVERBOSE
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#endif


ಸಂರಚನಾ thrdslot {
  ಸಂರಚನಾ Curl_llist_ಗಂಟು ಗಂಟು;
  ಸಂರಚನಾ curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  ಸಂರಚನಾ curltime starttime;
  ಧ್ರುವ ವರ್ಣ *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  BIT(running);
  BIT(idle);
};

ಸಂರಚನಾ curl_thrdpool {
  ಧ್ರುವ ವರ್ಣ *name;
  uint64_t refcount;
  curl_mutex_t lock;
  curl_cond_t await;
  ಸಂರಚನಾ Curl_llist slots;
  ಸಂರಚನಾ Curl_llist zombies;
  Curl_thrdpool_take_item_cb *fn_take;
  Curl_thrdpool_process_item_cb *fn_process;
  Curl_thrdpool_return_item_cb *fn_return;
  ಶೂನ್ಯ *fn_user_data;
  CURLcode fatal_err;
  uint32_t min_threads;
  uint32_t max_threads;
  uint32_t idle_time_ms;
  uint32_t next_id;
#ifdef DEBUGBUILD
  ಪೂರ್ಣಾಙ್ಕ dbg_fail_starts; /* fail this many thread starts */
#endif
  BIT(aborted);
  BIT(detached);
};

ಸ್ಥಿರ ಶೂನ್ಯ thrdpool_join_zombies(ಸಂರಚನಾ curl_thrdpool *tpool);
ಸ್ಥಿರ bool thrdpool_unlink(ಸಂರಚನಾ curl_thrdpool *tpool, bool locked);

ಸ್ಥಿರ ಶೂನ್ಯ thrdslot_destroy(ಸಂರಚನಾ thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdslot_done(ಸಂರಚನಾ thrdslot *tslot)
{
  ಸಂರಚನಾ curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_ಗಂಟು_llist(&tslot->ಗಂಟು) == &tpool->slots);
  Curl_ಗಂಟು_remove(&tslot->ಗಂಟು);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->ಗಂಟು);
  Curl_cond_signal(&tpool->await);
}

ಸ್ಥಿರ CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_run(ಶೂನ್ಯ *arg)
{
  ಸಂರಚನಾ thrdslot *tslot = arg;
  ಸಂರಚನಾ curl_thrdpool *tpool = tslot->tpool;
  ಶೂನ್ಯ *item;

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(Curl_ಗಂಟು_llist(&tslot->ಗಂಟು) == &tpool->slots);
  ಪ್ರತಿ(;;) {
    ಯಾವತ್(!tpool->aborted) {
      tslot->work_description = ರಿಕ್ತ;
      tslot->work_timeout_ms = 0;
      item = tpool->fn_take(tpool->fn_user_data, &tslot->work_description,
                            &tslot->work_timeout_ms);
      ಯದಿ(!item)
        ಭಙ್ಗ;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->lock);

      tpool->fn_process(item);

      Curl_mutex_acquire(&tpool->lock);
      tslot->work_description = ರಿಕ್ತ;
      tpool->fn_return(item, tpool->aborted ? ರಿಕ್ತ : tpool->fn_user_data);
    }

    ಯದಿ(tpool->aborted ||
       (Curl_llist_ಎಣಿಕೆ(&tpool->slots) > tpool->max_threads))
      ಗಚ್ಛ out;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    ಯದಿ((tpool->idle_time_ms > 0) &&
       (Curl_llist_ಎಣಿಕೆ(&tpool->slots) > tpool->min_threads)) {
      CURLcode result = Curl_cond_timedwait(&tslot->await, &tpool->lock,
                                            tpool->idle_time_ms);
      ಯದಿ((result == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_ಎಣಿಕೆ(&tpool->slots) > tpool->min_threads)) {
        ಗಚ್ಛ out;
      }
    }
    ಅನ್ಯಥಾ {
      Curl_cond_wait(&tslot->await, &tpool->lock);
    }
  }

out:
  thrdslot_done(tslot);
  ಯದಿ(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ CURLcode thrdslot_start(ಸಂರಚನಾ curl_thrdpool *tpool)
{
  ಸಂರಚನಾ thrdslot *tslot;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  tslot = curlx_calloc(1, ಪರಿಮಾಣ(*tslot));
  ಯದಿ(!tslot)
    ಗಚ್ಛ out;
  tslot->id = tpool->next_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#ifdef DEBUGBUILD
  ಯದಿ(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  ಅನ್ಯಥಾ
#endif
    tslot->thread = Curl_thread_create(thrdslot_run, tslot);
  ಯದಿ(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    result = CURLE_FAILED_INIT;
    ಗಚ್ಛ out;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->ಗಂಟು);
  tslot = ರಿಕ್ತ;
  result = CURLE_OK;

out:
  ಯದಿ(tslot)
    thrdslot_destroy(tslot);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdpool_wake_all(ಸಂರಚನಾ curl_thrdpool *tpool)
{
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
  ಪ್ರತಿ(e = Curl_llist_head(&tpool->slots); e; e = Curl_ಗಂಟು_next(e)) {
    ಸಂರಚನಾ thrdslot *tslot = Curl_ಗಂಟು_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdpool_join_zombies(ಸಂರಚನಾ curl_thrdpool *tpool)
{
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;

  ಪ್ರತಿ(e = Curl_llist_head(&tpool->zombies); e;
      e = Curl_llist_head(&tpool->zombies)) {
    ಸಂರಚನಾ thrdslot *tslot = Curl_ಗಂಟು_elem(e);

    Curl_ಗಂಟು_remove(&tslot->ಗಂಟು);
    ಯದಿ(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->lock);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->lock);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

ಸ್ಥಿರ bool thrdpool_unlink(ಸಂರಚನಾ curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  ಯದಿ(tpool->refcount)
    tpool->refcount--;
  ಯದಿ(tpool->refcount)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  ಯದಿ(locked)
    Curl_mutex_release(&tpool->lock);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->lock);
  curlx_free(tpool);
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ CURLcode thrdpool_signal(ಸಂರಚನಾ curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *e, *n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  ಪ್ರತಿ(e = Curl_llist_head(&tpool->slots); e && nthreads; e = n) {
    ಸಂರಚನಾ thrdslot *tslot = Curl_ಗಂಟು_elem(e);
    n = Curl_ಗಂಟು_next(e);
    ಯದಿ(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    ಅನ್ಯಥಾ ಯದಿ(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  ಯಾವತ್(nthreads && !result &&
        Curl_llist_ಎಣಿಕೆ(&tpool->slots) < tpool->max_threads) {
    result = thrdslot_start(tpool);
    ಯದಿ(result)
      ಭಙ್ಗ;
    --nthreads;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_thrdpool_set_props(ಸಂರಚನಾ curl_thrdpool *tpool,
                                 uint32_t min_threads,
                                 uint32_t max_threads,
                                 uint32_t idle_time_ms)
{
  CURLcode result = CURLE_OK;
  size_t running;

  ಯದಿ(!max_threads || (min_threads > max_threads))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mutex_acquire(&tpool->lock);
  tpool->min_threads = min_threads;
  tpool->max_threads = max_threads;
  tpool->idle_time_ms = idle_time_ms;
  running = Curl_llist_ಎಣಿಕೆ(&tpool->slots);
  ಯದಿ(tpool->min_threads > running) {
    result = thrdpool_signal(tpool, tpool->min_threads - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->lock);

  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_thrdpool_create(ಸಂರಚನಾ curl_thrdpool **ptpool,
                              ಧ್ರುವ ವರ್ಣ *name,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms,
                              Curl_thrdpool_take_item_cb *fn_take,
                              Curl_thrdpool_process_item_cb *fn_process,
                              Curl_thrdpool_return_item_cb *fn_return,
                              ಶೂನ್ಯ *user_data)
{
  ಸಂರಚನಾ curl_thrdpool *tpool;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, ಪರಿಮಾಣ(*tpool));
  ಯದಿ(!tpool)
    ಗಚ್ಛ out;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->lock);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, ರಿಕ್ತ);
  Curl_llist_init(&tpool->zombies, ರಿಕ್ತ);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_data = user_data;

  /* a const string that remains */
  tpool->name = name;

#ifdef DEBUGBUILD
  {
    ಧ್ರುವ ವರ್ಣ *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    ಯದಿ(p) {
      curl_off_t l;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, INT_MAX))
        tpool->dbg_fail_starts = (ಪೂರ್ಣಾಙ್ಕ)l;
    }
  }
#endif

  result = Curl_thrdpool_set_props(tpool, min_threads, max_threads,
                                   idle_time_ms);

out:
  ಯದಿ(result && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = ರಿಕ್ತ;
  }
  *ptpool = tpool;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_thrdpool_destroy(ಸಂರಚನಾ curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->lock);

  tpool->aborted = TRUE;

  ಯಾವತ್(join && Curl_llist_ಎಣಿಕೆ(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->lock);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  ಯದಿ(Curl_llist_ಎಣಿಕೆ(&tpool->slots)) {
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
    ಪ್ರತಿ(e = Curl_llist_head(&tpool->slots); e; e = Curl_ಗಂಟು_next(e)) {
      ಸಂರಚನಾ thrdslot *tslot = Curl_ಗಂಟು_elem(e);
      ಯದಿ(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  ಯದಿ(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
}

CURLcode Curl_thrdpool_signal(ಸಂರಚನಾ curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode result;

  Curl_mutex_acquire(&tpool->lock);
  result = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->lock);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool thrdpool_all_idle(ಸಂರಚನಾ curl_thrdpool *tpool)
{
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
  ಪ್ರತಿ(e = Curl_llist_head(&tpool->slots); e; e = Curl_ಗಂಟು_next(e)) {
    ಸಂರಚನಾ thrdslot *tslot = Curl_ಗಂಟು_elem(e);
    ಯದಿ(!tslot->idle)
      ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

CURLcode Curl_thrdpool_await_idle(ಸಂರಚನಾ curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ curltime end = { 0 };

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(!tpool->aborted);
  ಯದಿ(tpool->aborted) {
    result = CURLE_FAILED_INIT;
    ಗಚ್ಛ out;
  }

  ಯಾವತ್(!thrdpool_all_idle(tpool)) {
    ಯದಿ(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      ಯದಿ(!end.tv_sec && !end.tv_usec) {
        end = curlx_now();
        end.tv_sec += (time_t)(timeout_ms / 1000);
        end.tv_usec += (ಪೂರ್ಣಾಙ್ಕ)(timeout_ms % 1000) * 1000;
        ಯದಿ(end.tv_usec >= 1000000) {
          end.tv_sec++;
          end.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), end);
      ಯದಿ(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      ಅನ್ಯಥಾ
        r = Curl_cond_timedwait(&tpool->await, &tpool->lock,
                                (uint32_t)remain_ms);
      ಯದಿ(r == CURLE_OPERATION_TIMEDOUT) {
        result = r;
        ಭಙ್ಗ;
      }
    }
    ಅನ್ಯಥಾ {
      Curl_cond_wait(&tpool->await, &tpool->lock);
    }
  }

out:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->lock);
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef CURLVERBOSE
ಶೂನ್ಯ Curl_thrdpool_trace(ಸಂರಚನಾ curl_thrdpool *tpool,
                         ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ curl_trc_feat *feat = &Curl_trc_feat_threads;
  ಯದಿ(Curl_trc_ft_is_verbose(data, feat)) {
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
    ಸಂರಚನಾ curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->lock);
    ಯದಿ(!Curl_llist_ಎಣಿಕೆ(&tpool->slots)) {
      Curl_trc_feat_infof(data, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    ಪ್ರತಿ(e = Curl_llist_head(&tpool->slots); e; e = Curl_ಗಂಟು_next(e)) {
      ಸಂರಚನಾ thrdslot *tslot = Curl_ಗಂಟು_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      ಯದಿ(!tslot->running) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      ಅನ್ಯಥಾ ಯದಿ(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      ಅನ್ಯಥಾ ಯದಿ(tslot->idle) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      ಅನ್ಯಥಾ {
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
