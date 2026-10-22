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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_THREADS

#ଅନ୍ତର୍ଭାଵ "llist.h"
#ଅନ୍ତର୍ଭାଵ "curl_threads.h"
#ଅନ୍ତର୍ଭାଵ "curlx/timeval.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "thrdpool.h"
#ifdef CURLVERBOSE
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#endif


ସଂରଚନା thrdslot {
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି ଗଣ୍ଠି;
  ସଂରଚନା curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  ସଂରଚନା curltime starttime;
  ଅଚଳ ଅକ୍ଷର *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  BIT(running);
  BIT(idle);
};

ସଂରଚନା curl_thrdpool {
  ଅଚଳ ଅକ୍ଷର *name;
  uint64_t refcount;
  curl_mutex_t lock;
  curl_cond_t await;
  ସଂରଚନା Curl_llist slots;
  ସଂରଚନା Curl_llist zombies;
  Curl_thrdpool_take_item_cb *fn_take;
  Curl_thrdpool_process_item_cb *fn_process;
  Curl_thrdpool_return_item_cb *fn_return;
  ଶୂନ୍ୟ *fn_user_data;
  CURLcode fatal_err;
  uint32_t min_threads;
  uint32_t max_threads;
  uint32_t idle_time_ms;
  uint32_t next_id;
#ifdef DEBUGBUILD
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା dbg_fail_starts; /* fail this many thread starts */
#endif
  BIT(aborted);
  BIT(detached);
};

ସ୍ଥିର ଶୂନ୍ୟ thrdpool_join_zombies(ସଂରଚନା curl_thrdpool *tpool);
ସ୍ଥିର bool thrdpool_unlink(ସଂରଚନା curl_thrdpool *tpool, bool locked);

ସ୍ଥିର ଶୂନ୍ୟ thrdslot_destroy(ସଂରଚନା thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

ସ୍ଥିର ଶୂନ୍ୟ thrdslot_done(ସଂରଚନା thrdslot *tslot)
{
  ସଂରଚନା curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_ଗଣ୍ଠି_llist(&tslot->ଗଣ୍ଠି) == &tpool->slots);
  Curl_ଗଣ୍ଠି_remove(&tslot->ଗଣ୍ଠି);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->ଗଣ୍ଠି);
  Curl_cond_signal(&tpool->await);
}

ସ୍ଥିର CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_run(ଶୂନ୍ୟ *arg)
{
  ସଂରଚନା thrdslot *tslot = arg;
  ସଂରଚନା curl_thrdpool *tpool = tslot->tpool;
  ଶୂନ୍ୟ *item;

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(Curl_ଗଣ୍ଠି_llist(&tslot->ଗଣ୍ଠି) == &tpool->slots);
  ପାଇଁ(;;) {
    ଯେତେବେଳେ(!tpool->aborted) {
      tslot->work_description = ଖାଲି;
      tslot->work_timeout_ms = 0;
      item = tpool->fn_take(tpool->fn_user_data, &tslot->work_description,
                            &tslot->work_timeout_ms);
      ଯଦି(!item)
        ଭାଙ୍ଗ;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->lock);

      tpool->fn_process(item);

      Curl_mutex_acquire(&tpool->lock);
      tslot->work_description = ଖାଲି;
      tpool->fn_return(item, tpool->aborted ? ଖାଲି : tpool->fn_user_data);
    }

    ଯଦି(tpool->aborted ||
       (Curl_llist_ଗଣନା(&tpool->slots) > tpool->max_threads))
      ଯାଅ out;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    ଯଦି((tpool->idle_time_ms > 0) &&
       (Curl_llist_ଗଣନା(&tpool->slots) > tpool->min_threads)) {
      CURLcode result = Curl_cond_timedwait(&tslot->await, &tpool->lock,
                                            tpool->idle_time_ms);
      ଯଦି((result == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_ଗଣନା(&tpool->slots) > tpool->min_threads)) {
        ଯାଅ out;
      }
    }
    ଅନ୍ୟଥା {
      Curl_cond_wait(&tslot->await, &tpool->lock);
    }
  }

out:
  thrdslot_done(tslot);
  ଯଦି(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର CURLcode thrdslot_start(ସଂରଚନା curl_thrdpool *tpool)
{
  ସଂରଚନା thrdslot *tslot;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  tslot = curlx_calloc(1, ଆକାର(*tslot));
  ଯଦି(!tslot)
    ଯାଅ out;
  tslot->id = tpool->next_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#ifdef DEBUGBUILD
  ଯଦି(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  ଅନ୍ୟଥା
#endif
    tslot->thread = Curl_thread_create(thrdslot_run, tslot);
  ଯଦି(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    result = CURLE_FAILED_INIT;
    ଯାଅ out;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->ଗଣ୍ଠି);
  tslot = ଖାଲି;
  result = CURLE_OK;

out:
  ଯଦି(tslot)
    thrdslot_destroy(tslot);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ thrdpool_wake_all(ସଂରଚନା curl_thrdpool *tpool)
{
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
  ପାଇଁ(e = Curl_llist_head(&tpool->slots); e; e = Curl_ଗଣ୍ଠି_next(e)) {
    ସଂରଚନା thrdslot *tslot = Curl_ଗଣ୍ଠି_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ thrdpool_join_zombies(ସଂରଚନା curl_thrdpool *tpool)
{
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;

  ପାଇଁ(e = Curl_llist_head(&tpool->zombies); e;
      e = Curl_llist_head(&tpool->zombies)) {
    ସଂରଚନା thrdslot *tslot = Curl_ଗଣ୍ଠି_elem(e);

    Curl_ଗଣ୍ଠି_remove(&tslot->ଗଣ୍ଠି);
    ଯଦି(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->lock);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->lock);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

ସ୍ଥିର bool thrdpool_unlink(ସଂରଚନା curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  ଯଦି(tpool->refcount)
    tpool->refcount--;
  ଯଦି(tpool->refcount)
    ଫେରନ୍ତୁ FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  ଯଦି(locked)
    Curl_mutex_release(&tpool->lock);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->lock);
  curlx_free(tpool);
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର CURLcode thrdpool_signal(ସଂରଚନା curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e, *n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  ପାଇଁ(e = Curl_llist_head(&tpool->slots); e && nthreads; e = n) {
    ସଂରଚନା thrdslot *tslot = Curl_ଗଣ୍ଠି_elem(e);
    n = Curl_ଗଣ୍ଠି_next(e);
    ଯଦି(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    ଅନ୍ୟଥା ଯଦି(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  ଯେତେବେଳେ(nthreads && !result &&
        Curl_llist_ଗଣନା(&tpool->slots) < tpool->max_threads) {
    result = thrdslot_start(tpool);
    ଯଦି(result)
      ଭାଙ୍ଗ;
    --nthreads;
  }

  ଫେରନ୍ତୁ result;
}

CURLcode Curl_thrdpool_set_props(ସଂରଚନା curl_thrdpool *tpool,
                                 uint32_t min_threads,
                                 uint32_t max_threads,
                                 uint32_t idle_time_ms)
{
  CURLcode result = CURLE_OK;
  size_t running;

  ଯଦି(!max_threads || (min_threads > max_threads))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mutex_acquire(&tpool->lock);
  tpool->min_threads = min_threads;
  tpool->max_threads = max_threads;
  tpool->idle_time_ms = idle_time_ms;
  running = Curl_llist_ଗଣନା(&tpool->slots);
  ଯଦି(tpool->min_threads > running) {
    result = thrdpool_signal(tpool, tpool->min_threads - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->lock);

  ଫେରନ୍ତୁ result;
}

CURLcode Curl_thrdpool_create(ସଂରଚନା curl_thrdpool **ptpool,
                              ଅଚଳ ଅକ୍ଷର *name,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms,
                              Curl_thrdpool_take_item_cb *fn_take,
                              Curl_thrdpool_process_item_cb *fn_process,
                              Curl_thrdpool_return_item_cb *fn_return,
                              ଶୂନ୍ୟ *user_data)
{
  ସଂରଚନା curl_thrdpool *tpool;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, ଆକାର(*tpool));
  ଯଦି(!tpool)
    ଯାଅ out;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->lock);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, ଖାଲି);
  Curl_llist_init(&tpool->zombies, ଖାଲି);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_data = user_data;

  /* a const string that remains */
  tpool->name = name;

#ifdef DEBUGBUILD
  {
    ଅଚଳ ଅକ୍ଷର *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    ଯଦି(p) {
      curl_off_t l;
      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, INT_MAX))
        tpool->dbg_fail_starts = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)l;
    }
  }
#endif

  result = Curl_thrdpool_set_props(tpool, min_threads, max_threads,
                                   idle_time_ms);

out:
  ଯଦି(result && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = ଖାଲି;
  }
  *ptpool = tpool;
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_thrdpool_destroy(ସଂରଚନା curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->lock);

  tpool->aborted = TRUE;

  ଯେତେବେଳେ(join && Curl_llist_ଗଣନା(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->lock);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  ଯଦି(Curl_llist_ଗଣନା(&tpool->slots)) {
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
    ପାଇଁ(e = Curl_llist_head(&tpool->slots); e; e = Curl_ଗଣ୍ଠି_next(e)) {
      ସଂରଚନା thrdslot *tslot = Curl_ଗଣ୍ଠି_elem(e);
      ଯଦି(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  ଯଦି(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
}

CURLcode Curl_thrdpool_signal(ସଂରଚନା curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode result;

  Curl_mutex_acquire(&tpool->lock);
  result = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->lock);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool thrdpool_all_idle(ସଂରଚନା curl_thrdpool *tpool)
{
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
  ପାଇଁ(e = Curl_llist_head(&tpool->slots); e; e = Curl_ଗଣ୍ଠି_next(e)) {
    ସଂରଚନା thrdslot *tslot = Curl_ଗଣ୍ଠି_elem(e);
    ଯଦି(!tslot->idle)
      ଫେରନ୍ତୁ FALSE;
  }
  ଫେରନ୍ତୁ TRUE;
}

CURLcode Curl_thrdpool_await_idle(ସଂରଚନା curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା curltime end = { 0 };

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(!tpool->aborted);
  ଯଦି(tpool->aborted) {
    result = CURLE_FAILED_INIT;
    ଯାଅ out;
  }

  ଯେତେବେଳେ(!thrdpool_all_idle(tpool)) {
    ଯଦି(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      ଯଦି(!end.tv_sec && !end.tv_usec) {
        end = curlx_now();
        end.tv_sec += (time_t)(timeout_ms / 1000);
        end.tv_usec += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(timeout_ms % 1000) * 1000;
        ଯଦି(end.tv_usec >= 1000000) {
          end.tv_sec++;
          end.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), end);
      ଯଦି(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      ଅନ୍ୟଥା
        r = Curl_cond_timedwait(&tpool->await, &tpool->lock,
                                (uint32_t)remain_ms);
      ଯଦି(r == CURLE_OPERATION_TIMEDOUT) {
        result = r;
        ଭାଙ୍ଗ;
      }
    }
    ଅନ୍ୟଥା {
      Curl_cond_wait(&tpool->await, &tpool->lock);
    }
  }

out:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->lock);
  ଫେରନ୍ତୁ result;
}

#ifdef CURLVERBOSE
ଶୂନ୍ୟ Curl_thrdpool_trace(ସଂରଚନା curl_thrdpool *tpool,
                         ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା curl_trc_feat *feat = &Curl_trc_feat_threads;
  ଯଦି(Curl_trc_ft_is_verbose(data, feat)) {
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
    ସଂରଚନା curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->lock);
    ଯଦି(!Curl_llist_ଗଣନା(&tpool->slots)) {
      Curl_trc_feat_infof(data, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    ପାଇଁ(e = Curl_llist_head(&tpool->slots); e; e = Curl_ଗଣ୍ଠି_next(e)) {
      ସଂରଚନା thrdslot *tslot = Curl_ଗଣ୍ଠି_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      ଯଦି(!tslot->running) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      ଅନ୍ୟଥା ଯଦି(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      ଅନ୍ୟଥା ଯଦି(tslot->idle) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      ଅନ୍ୟଥା {
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
