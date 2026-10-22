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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_THREADS

#അന്തര്ഭാവ "llist.h"
#അന്തര്ഭാവ "curl_threads.h"
#അന്തര്ഭാവ "curlx/timeval.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "thrdpool.h"
#ifdef CURLVERBOSE
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "urldata.h"
#endif


ഘടന thrdslot {
  ഘടന Curl_llist_കണ്ണി കണ്ണി;
  ഘടന curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  ഘടന curltime starttime;
  സ്ഥിരം അക്ഷരം *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  BIT(running);
  BIT(idle);
};

ഘടന curl_thrdpool {
  സ്ഥിരം അക്ഷരം *name;
  uint64_t refcount;
  curl_mutex_t lock;
  curl_cond_t await;
  ഘടന Curl_llist slots;
  ഘടന Curl_llist zombies;
  Curl_thrdpool_take_item_cb *fn_take;
  Curl_thrdpool_process_item_cb *fn_process;
  Curl_thrdpool_return_item_cb *fn_return;
  ശൂന്യം *fn_user_data;
  CURLcode fatal_err;
  uint32_t min_threads;
  uint32_t max_threads;
  uint32_t idle_time_ms;
  uint32_t next_id;
#ifdef DEBUGBUILD
  പൂർണ്ണസംഖ്യ dbg_fail_starts; /* fail this many thread starts */
#endif
  BIT(aborted);
  BIT(detached);
};

സ്ഥിതം ശൂന്യം thrdpool_join_zombies(ഘടന curl_thrdpool *tpool);
സ്ഥിതം bool thrdpool_unlink(ഘടന curl_thrdpool *tpool, bool locked);

സ്ഥിതം ശൂന്യം thrdslot_destroy(ഘടന thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

സ്ഥിതം ശൂന്യം thrdslot_done(ഘടന thrdslot *tslot)
{
  ഘടന curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_കണ്ണി_llist(&tslot->കണ്ണി) == &tpool->slots);
  Curl_കണ്ണി_remove(&tslot->കണ്ണി);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->കണ്ണി);
  Curl_cond_signal(&tpool->await);
}

സ്ഥിതം CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_run(ശൂന്യം *arg)
{
  ഘടന thrdslot *tslot = arg;
  ഘടന curl_thrdpool *tpool = tslot->tpool;
  ശൂന്യം *item;

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(Curl_കണ്ണി_llist(&tslot->കണ്ണി) == &tpool->slots);
  വേണ്ടി(;;) {
    ആകുമ്പോൾ(!tpool->aborted) {
      tslot->work_description = ശൂന്യ;
      tslot->work_timeout_ms = 0;
      item = tpool->fn_take(tpool->fn_user_data, &tslot->work_description,
                            &tslot->work_timeout_ms);
      എങ്കിൽ(!item)
        നിർത്തുക;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->lock);

      tpool->fn_process(item);

      Curl_mutex_acquire(&tpool->lock);
      tslot->work_description = ശൂന്യ;
      tpool->fn_return(item, tpool->aborted ? ശൂന്യ : tpool->fn_user_data);
    }

    എങ്കിൽ(tpool->aborted ||
       (Curl_llist_എണ്ണം(&tpool->slots) > tpool->max_threads))
      പോകുക out;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    എങ്കിൽ((tpool->idle_time_ms > 0) &&
       (Curl_llist_എണ്ണം(&tpool->slots) > tpool->min_threads)) {
      CURLcode result = Curl_cond_timedwait(&tslot->await, &tpool->lock,
                                            tpool->idle_time_ms);
      എങ്കിൽ((result == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_എണ്ണം(&tpool->slots) > tpool->min_threads)) {
        പോകുക out;
      }
    }
    അല്ലെങ്കിൽ {
      Curl_cond_wait(&tslot->await, &tpool->lock);
    }
  }

out:
  thrdslot_done(tslot);
  എങ്കിൽ(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
  തിരികെ 0;
}

സ്ഥിതം CURLcode thrdslot_start(ഘടന curl_thrdpool *tpool)
{
  ഘടന thrdslot *tslot;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  tslot = curlx_calloc(1, വലുപ്പം(*tslot));
  എങ്കിൽ(!tslot)
    പോകുക out;
  tslot->id = tpool->next_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#ifdef DEBUGBUILD
  എങ്കിൽ(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  അല്ലെങ്കിൽ
#endif
    tslot->thread = Curl_thread_create(thrdslot_run, tslot);
  എങ്കിൽ(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    result = CURLE_FAILED_INIT;
    പോകുക out;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->കണ്ണി);
  tslot = ശൂന്യ;
  result = CURLE_OK;

out:
  എങ്കിൽ(tslot)
    thrdslot_destroy(tslot);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം thrdpool_wake_all(ഘടന curl_thrdpool *tpool)
{
  ഘടന Curl_llist_കണ്ണി *e;
  വേണ്ടി(e = Curl_llist_head(&tpool->slots); e; e = Curl_കണ്ണി_next(e)) {
    ഘടന thrdslot *tslot = Curl_കണ്ണി_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

സ്ഥിതം ശൂന്യം thrdpool_join_zombies(ഘടന curl_thrdpool *tpool)
{
  ഘടന Curl_llist_കണ്ണി *e;

  വേണ്ടി(e = Curl_llist_head(&tpool->zombies); e;
      e = Curl_llist_head(&tpool->zombies)) {
    ഘടന thrdslot *tslot = Curl_കണ്ണി_elem(e);

    Curl_കണ്ണി_remove(&tslot->കണ്ണി);
    എങ്കിൽ(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->lock);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->lock);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

സ്ഥിതം bool thrdpool_unlink(ഘടന curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  എങ്കിൽ(tpool->refcount)
    tpool->refcount--;
  എങ്കിൽ(tpool->refcount)
    തിരികെ FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  എങ്കിൽ(locked)
    Curl_mutex_release(&tpool->lock);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->lock);
  curlx_free(tpool);
  തിരികെ TRUE;
}

സ്ഥിതം CURLcode thrdpool_signal(ഘടന curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  ഘടന Curl_llist_കണ്ണി *e, *n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  വേണ്ടി(e = Curl_llist_head(&tpool->slots); e && nthreads; e = n) {
    ഘടന thrdslot *tslot = Curl_കണ്ണി_elem(e);
    n = Curl_കണ്ണി_next(e);
    എങ്കിൽ(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  ആകുമ്പോൾ(nthreads && !result &&
        Curl_llist_എണ്ണം(&tpool->slots) < tpool->max_threads) {
    result = thrdslot_start(tpool);
    എങ്കിൽ(result)
      നിർത്തുക;
    --nthreads;
  }

  തിരികെ result;
}

CURLcode Curl_thrdpool_set_props(ഘടന curl_thrdpool *tpool,
                                 uint32_t min_threads,
                                 uint32_t max_threads,
                                 uint32_t idle_time_ms)
{
  CURLcode result = CURLE_OK;
  size_t running;

  എങ്കിൽ(!max_threads || (min_threads > max_threads))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mutex_acquire(&tpool->lock);
  tpool->min_threads = min_threads;
  tpool->max_threads = max_threads;
  tpool->idle_time_ms = idle_time_ms;
  running = Curl_llist_എണ്ണം(&tpool->slots);
  എങ്കിൽ(tpool->min_threads > running) {
    result = thrdpool_signal(tpool, tpool->min_threads - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->lock);

  തിരികെ result;
}

CURLcode Curl_thrdpool_create(ഘടന curl_thrdpool **ptpool,
                              സ്ഥിരം അക്ഷരം *name,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms,
                              Curl_thrdpool_take_item_cb *fn_take,
                              Curl_thrdpool_process_item_cb *fn_process,
                              Curl_thrdpool_return_item_cb *fn_return,
                              ശൂന്യം *user_data)
{
  ഘടന curl_thrdpool *tpool;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, വലുപ്പം(*tpool));
  എങ്കിൽ(!tpool)
    പോകുക out;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->lock);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, ശൂന്യ);
  Curl_llist_init(&tpool->zombies, ശൂന്യ);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_data = user_data;

  /* a const string that remains */
  tpool->name = name;

#ifdef DEBUGBUILD
  {
    സ്ഥിരം അക്ഷരം *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    എങ്കിൽ(p) {
      curl_off_t l;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &l, INT_MAX))
        tpool->dbg_fail_starts = (പൂർണ്ണസംഖ്യ)l;
    }
  }
#endif

  result = Curl_thrdpool_set_props(tpool, min_threads, max_threads,
                                   idle_time_ms);

out:
  എങ്കിൽ(result && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = ശൂന്യ;
  }
  *ptpool = tpool;
  തിരികെ result;
}

ശൂന്യം Curl_thrdpool_destroy(ഘടന curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->lock);

  tpool->aborted = TRUE;

  ആകുമ്പോൾ(join && Curl_llist_എണ്ണം(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->lock);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  എങ്കിൽ(Curl_llist_എണ്ണം(&tpool->slots)) {
    ഘടന Curl_llist_കണ്ണി *e;
    വേണ്ടി(e = Curl_llist_head(&tpool->slots); e; e = Curl_കണ്ണി_next(e)) {
      ഘടന thrdslot *tslot = Curl_കണ്ണി_elem(e);
      എങ്കിൽ(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  എങ്കിൽ(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
}

CURLcode Curl_thrdpool_signal(ഘടന curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode result;

  Curl_mutex_acquire(&tpool->lock);
  result = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->lock);
  തിരികെ result;
}

സ്ഥിതം bool thrdpool_all_idle(ഘടന curl_thrdpool *tpool)
{
  ഘടന Curl_llist_കണ്ണി *e;
  വേണ്ടി(e = Curl_llist_head(&tpool->slots); e; e = Curl_കണ്ണി_next(e)) {
    ഘടന thrdslot *tslot = Curl_കണ്ണി_elem(e);
    എങ്കിൽ(!tslot->idle)
      തിരികെ FALSE;
  }
  തിരികെ TRUE;
}

CURLcode Curl_thrdpool_await_idle(ഘടന curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode result = CURLE_OK;
  ഘടന curltime end = { 0 };

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(!tpool->aborted);
  എങ്കിൽ(tpool->aborted) {
    result = CURLE_FAILED_INIT;
    പോകുക out;
  }

  ആകുമ്പോൾ(!thrdpool_all_idle(tpool)) {
    എങ്കിൽ(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      എങ്കിൽ(!end.tv_sec && !end.tv_usec) {
        end = curlx_now();
        end.tv_sec += (time_t)(timeout_ms / 1000);
        end.tv_usec += (പൂർണ്ണസംഖ്യ)(timeout_ms % 1000) * 1000;
        എങ്കിൽ(end.tv_usec >= 1000000) {
          end.tv_sec++;
          end.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), end);
      എങ്കിൽ(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      അല്ലെങ്കിൽ
        r = Curl_cond_timedwait(&tpool->await, &tpool->lock,
                                (uint32_t)remain_ms);
      എങ്കിൽ(r == CURLE_OPERATION_TIMEDOUT) {
        result = r;
        നിർത്തുക;
      }
    }
    അല്ലെങ്കിൽ {
      Curl_cond_wait(&tpool->await, &tpool->lock);
    }
  }

out:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->lock);
  തിരികെ result;
}

#ifdef CURLVERBOSE
ശൂന്യം Curl_thrdpool_trace(ഘടന curl_thrdpool *tpool,
                         ഘടന Curl_easy *data)
{
  ഘടന curl_trc_feat *feat = &Curl_trc_feat_threads;
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, feat)) {
    ഘടന Curl_llist_കണ്ണി *e;
    ഘടന curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->lock);
    എങ്കിൽ(!Curl_llist_എണ്ണം(&tpool->slots)) {
      Curl_trc_feat_infof(data, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    വേണ്ടി(e = Curl_llist_head(&tpool->slots); e; e = Curl_കണ്ണി_next(e)) {
      ഘടന thrdslot *tslot = Curl_കണ്ണി_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      എങ്കിൽ(!tslot->running) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      അല്ലെങ്കിൽ എങ്കിൽ(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      അല്ലെങ്കിൽ എങ്കിൽ(tslot->idle) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      അല്ലെങ്കിൽ {
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
