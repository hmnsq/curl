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
#ضمن "curl_setup.h"

#ifdef USE_THREADS

#ضمن "llist.h"
#ضمن "curl_threads.h"
#ضمن "curlx/timeval.h"
#ضمن "curlx/strparse.h"
#ضمن "thrdpool.h"
#ifdef CURLVERBOSE
#ضمن "curl_trc.h"
#ضمن "urldata.h"
#endif


بنية thrdslot {
  بنية Curl_llist_node node;
  بنية curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  بنية curltime starttime;
  ثابت حرف *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  BIT(running);
  BIT(idle);
};

بنية curl_thrdpool {
  ثابت حرف *name;
  uint64_t refcount;
  curl_mutex_t lock;
  curl_cond_t await;
  بنية Curl_llist slots;
  بنية Curl_llist zombies;
  Curl_thrdpool_take_item_cb *fn_take;
  Curl_thrdpool_process_item_cb *fn_process;
  Curl_thrdpool_return_item_cb *fn_return;
  فراغ *fn_user_data;
  CURLcode fatal_err;
  uint32_t min_threads;
  uint32_t max_threads;
  uint32_t idle_time_ms;
  uint32_t next_id;
#ifdef DEBUGBUILD
  صحيح dbg_fail_starts; /* fail this many thread starts */
#endif
  BIT(aborted);
  BIT(detached);
};

ساكن فراغ thrdpool_join_zombies(بنية curl_thrdpool *tpool);
ساكن bool thrdpool_unlink(بنية curl_thrdpool *tpool, bool locked);

ساكن فراغ thrdslot_destroy(بنية thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

ساكن فراغ thrdslot_done(بنية thrdslot *tslot)
{
  بنية curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_node_llist(&tslot->node) == &tpool->slots);
  Curl_node_remove(&tslot->node);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->node);
  Curl_cond_signal(&tpool->await);
}

ساكن CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_run(فراغ *arg)
{
  بنية thrdslot *tslot = arg;
  بنية curl_thrdpool *tpool = tslot->tpool;
  فراغ *item;

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(Curl_node_llist(&tslot->node) == &tpool->slots);
  لكل(;;) {
    ܟܡܐ(!tpool->aborted) {
      tslot->work_description = عدم;
      tslot->work_timeout_ms = 0;
      item = tpool->fn_take(tpool->fn_user_data, &tslot->work_description,
                            &tslot->work_timeout_ms);
      ܐܢ(!item)
        اكسر;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->lock);

      tpool->fn_process(item);

      Curl_mutex_acquire(&tpool->lock);
      tslot->work_description = عدم;
      tpool->fn_return(item, tpool->aborted ? عدم : tpool->fn_user_data);
    }

    ܐܢ(tpool->aborted ||
       (Curl_llist_count(&tpool->slots) > tpool->max_threads))
      اذهب out;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    ܐܢ((tpool->idle_time_ms > 0) &&
       (Curl_llist_count(&tpool->slots) > tpool->min_threads)) {
      CURLcode result = Curl_cond_timedwait(&tslot->await, &tpool->lock,
                                            tpool->idle_time_ms);
      ܐܢ((result == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_count(&tpool->slots) > tpool->min_threads)) {
        اذهب out;
      }
    }
    ܐܚܪܢܐ {
      Curl_cond_wait(&tslot->await, &tpool->lock);
    }
  }

out:
  thrdslot_done(tslot);
  ܐܢ(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
  ܦܢܐ 0;
}

ساكن CURLcode thrdslot_start(بنية curl_thrdpool *tpool)
{
  بنية thrdslot *tslot;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  tslot = curlx_calloc(1, حجم(*tslot));
  ܐܢ(!tslot)
    اذهب out;
  tslot->id = tpool->next_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#ifdef DEBUGBUILD
  ܐܢ(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  ܐܚܪܢܐ
#endif
    tslot->thread = Curl_thread_create(thrdslot_run, tslot);
  ܐܢ(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    result = CURLE_FAILED_INIT;
    اذهب out;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->node);
  tslot = عدم;
  result = CURLE_OK;

out:
  ܐܢ(tslot)
    thrdslot_destroy(tslot);
  ܦܢܐ result;
}

ساكن فراغ thrdpool_wake_all(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_node *e;
  لكل(e = Curl_llist_head(&tpool->slots); e; e = Curl_node_next(e)) {
    بنية thrdslot *tslot = Curl_node_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

ساكن فراغ thrdpool_join_zombies(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_node *e;

  لكل(e = Curl_llist_head(&tpool->zombies); e;
      e = Curl_llist_head(&tpool->zombies)) {
    بنية thrdslot *tslot = Curl_node_elem(e);

    Curl_node_remove(&tslot->node);
    ܐܢ(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->lock);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->lock);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

ساكن bool thrdpool_unlink(بنية curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  ܐܢ(tpool->refcount)
    tpool->refcount--;
  ܐܢ(tpool->refcount)
    ܦܢܐ FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  ܐܢ(locked)
    Curl_mutex_release(&tpool->lock);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->lock);
  curlx_free(tpool);
  ܦܢܐ TRUE;
}

ساكن CURLcode thrdpool_signal(بنية curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  بنية Curl_llist_node *e, *n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  لكل(e = Curl_llist_head(&tpool->slots); e && nthreads; e = n) {
    بنية thrdslot *tslot = Curl_node_elem(e);
    n = Curl_node_next(e);
    ܐܢ(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    ܐܚܪܢܐ ܐܢ(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  ܟܡܐ(nthreads && !result &&
        Curl_llist_count(&tpool->slots) < tpool->max_threads) {
    result = thrdslot_start(tpool);
    ܐܢ(result)
      اكسر;
    --nthreads;
  }

  ܦܢܐ result;
}

CURLcode Curl_thrdpool_set_props(بنية curl_thrdpool *tpool,
                                 uint32_t min_threads,
                                 uint32_t max_threads,
                                 uint32_t idle_time_ms)
{
  CURLcode result = CURLE_OK;
  size_t running;

  ܐܢ(!max_threads || (min_threads > max_threads))
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mutex_acquire(&tpool->lock);
  tpool->min_threads = min_threads;
  tpool->max_threads = max_threads;
  tpool->idle_time_ms = idle_time_ms;
  running = Curl_llist_count(&tpool->slots);
  ܐܢ(tpool->min_threads > running) {
    result = thrdpool_signal(tpool, tpool->min_threads - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->lock);

  ܦܢܐ result;
}

CURLcode Curl_thrdpool_create(بنية curl_thrdpool **ptpool,
                              ثابت حرف *name,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms,
                              Curl_thrdpool_take_item_cb *fn_take,
                              Curl_thrdpool_process_item_cb *fn_process,
                              Curl_thrdpool_return_item_cb *fn_return,
                              فراغ *user_data)
{
  بنية curl_thrdpool *tpool;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, حجم(*tpool));
  ܐܢ(!tpool)
    اذهب out;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->lock);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, عدم);
  Curl_llist_init(&tpool->zombies, عدم);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_data = user_data;

  /* a const string that remains */
  tpool->name = name;

#ifdef DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    ܐܢ(p) {
      curl_off_t l;
      ܐܢ(!curlx_str_number(&p, &l, INT_MAX))
        tpool->dbg_fail_starts = (صحيح)l;
    }
  }
#endif

  result = Curl_thrdpool_set_props(tpool, min_threads, max_threads,
                                   idle_time_ms);

out:
  ܐܢ(result && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = عدم;
  }
  *ptpool = tpool;
  ܦܢܐ result;
}

فراغ Curl_thrdpool_destroy(بنية curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->lock);

  tpool->aborted = TRUE;

  ܟܡܐ(join && Curl_llist_count(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->lock);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  ܐܢ(Curl_llist_count(&tpool->slots)) {
    بنية Curl_llist_node *e;
    لكل(e = Curl_llist_head(&tpool->slots); e; e = Curl_node_next(e)) {
      بنية thrdslot *tslot = Curl_node_elem(e);
      ܐܢ(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  ܐܢ(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
}

CURLcode Curl_thrdpool_signal(بنية curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode result;

  Curl_mutex_acquire(&tpool->lock);
  result = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->lock);
  ܦܢܐ result;
}

ساكن bool thrdpool_all_idle(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_node *e;
  لكل(e = Curl_llist_head(&tpool->slots); e; e = Curl_node_next(e)) {
    بنية thrdslot *tslot = Curl_node_elem(e);
    ܐܢ(!tslot->idle)
      ܦܢܐ FALSE;
  }
  ܦܢܐ TRUE;
}

CURLcode Curl_thrdpool_await_idle(بنية curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode result = CURLE_OK;
  بنية curltime end = { 0 };

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(!tpool->aborted);
  ܐܢ(tpool->aborted) {
    result = CURLE_FAILED_INIT;
    اذهب out;
  }

  ܟܡܐ(!thrdpool_all_idle(tpool)) {
    ܐܢ(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      ܐܢ(!end.tv_sec && !end.tv_usec) {
        end = curlx_now();
        end.tv_sec += (time_t)(timeout_ms / 1000);
        end.tv_usec += (صحيح)(timeout_ms % 1000) * 1000;
        ܐܢ(end.tv_usec >= 1000000) {
          end.tv_sec++;
          end.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), end);
      ܐܢ(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      ܐܚܪܢܐ
        r = Curl_cond_timedwait(&tpool->await, &tpool->lock,
                                (uint32_t)remain_ms);
      ܐܢ(r == CURLE_OPERATION_TIMEDOUT) {
        result = r;
        اكسر;
      }
    }
    ܐܚܪܢܐ {
      Curl_cond_wait(&tpool->await, &tpool->lock);
    }
  }

out:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->lock);
  ܦܢܐ result;
}

#ifdef CURLVERBOSE
فراغ Curl_thrdpool_trace(بنية curl_thrdpool *tpool,
                         بنية Curl_easy *data)
{
  بنية curl_trc_feat *feat = &Curl_trc_feat_threads;
  ܐܢ(Curl_trc_ft_is_verbose(data, feat)) {
    بنية Curl_llist_node *e;
    بنية curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->lock);
    ܐܢ(!Curl_llist_count(&tpool->slots)) {
      Curl_trc_feat_infof(data, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    لكل(e = Curl_llist_head(&tpool->slots); e; e = Curl_node_next(e)) {
      بنية thrdslot *tslot = Curl_node_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      ܐܢ(!tslot->running) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      ܐܚܪܢܐ ܐܢ(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      ܐܚܪܢܐ ܐܢ(tslot->idle) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      ܐܚܪܢܐ {
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
