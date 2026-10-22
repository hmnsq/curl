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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_THREADS

#অন্তর্ভাব "llist.h"
#অন্তর্ভাব "curl_threads.h"
#অন্তর্ভাব "curlx/timeval.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "thrdpool.h"
#ifdef CURLVERBOSE
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "urldata.h"
#endif


কাঠামো thrdslot {
  কাঠামো Curl_llist_গ্রন্থি গ্রন্থি;
  কাঠামো curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  কাঠামো curltime starttime;
  ধ্রুবক অক্ষর *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  BIT(running);
  BIT(idle);
};

কাঠামো curl_thrdpool {
  ধ্রুবক অক্ষর *name;
  uint64_t refcount;
  curl_mutex_t lock;
  curl_cond_t await;
  কাঠামো Curl_llist slots;
  কাঠামো Curl_llist zombies;
  Curl_thrdpool_take_item_cb *fn_take;
  Curl_thrdpool_process_item_cb *fn_process;
  Curl_thrdpool_return_item_cb *fn_return;
  শূন্য *fn_user_তথ্য;
  CURLcode fatal_err;
  uint32_t min_threads;
  uint32_t max_threads;
  uint32_t idle_time_ms;
  uint32_t পরবর্তী_id;
#ifdef DEBUGBUILD
  পূর্ণসংখ্যা dbg_fail_starts; /* fail this many thread starts */
#endif
  BIT(aborted);
  BIT(detached);
};

স্থির শূন্য thrdpool_join_zombies(কাঠামো curl_thrdpool *tpool);
স্থির bool thrdpool_unlink(কাঠামো curl_thrdpool *tpool, bool locked);

স্থির শূন্য thrdslot_destroy(কাঠামো thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

স্থির শূন্য thrdslot_done(কাঠামো thrdslot *tslot)
{
  কাঠামো curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_গ্রন্থি_llist(&tslot->গ্রন্থি) == &tpool->slots);
  Curl_গ্রন্থি_remove(&tslot->গ্রন্থি);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->গ্রন্থি);
  Curl_cond_signal(&tpool->await);
}

স্থির CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_run(শূন্য *arg)
{
  কাঠামো thrdslot *tslot = arg;
  কাঠামো curl_thrdpool *tpool = tslot->tpool;
  শূন্য *item;

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(Curl_গ্রন্থি_llist(&tslot->গ্রন্থি) == &tpool->slots);
  জন্য(;;) {
    যখন(!tpool->aborted) {
      tslot->work_description = ফাঁকা;
      tslot->work_timeout_ms = 0;
      item = tpool->fn_take(tpool->fn_user_তথ্য, &tslot->work_description,
                            &tslot->work_timeout_ms);
      যদি(!item)
        বিরতি;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->lock);

      tpool->fn_process(item);

      Curl_mutex_acquire(&tpool->lock);
      tslot->work_description = ফাঁকা;
      tpool->fn_return(item, tpool->aborted ? ফাঁকা : tpool->fn_user_তথ্য);
    }

    যদি(tpool->aborted ||
       (Curl_llist_গুনতি(&tpool->slots) > tpool->max_threads))
      যাও out;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    যদি((tpool->idle_time_ms > 0) &&
       (Curl_llist_গুনতি(&tpool->slots) > tpool->min_threads)) {
      CURLcode ফলাফল = Curl_cond_timedwait(&tslot->await, &tpool->lock,
                                            tpool->idle_time_ms);
      যদি((ফলাফল == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_গুনতি(&tpool->slots) > tpool->min_threads)) {
        যাও out;
      }
    }
    অন্যথা {
      Curl_cond_wait(&tslot->await, &tpool->lock);
    }
  }

out:
  thrdslot_done(tslot);
  যদি(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
  ফেরত 0;
}

স্থির CURLcode thrdslot_start(কাঠামো curl_thrdpool *tpool)
{
  কাঠামো thrdslot *tslot;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  tslot = curlx_calloc(1, আকার(*tslot));
  যদি(!tslot)
    যাও out;
  tslot->id = tpool->পরবর্তী_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#ifdef DEBUGBUILD
  যদি(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  অন্যথা
#endif
    tslot->thread = Curl_thread_create(thrdslot_run, tslot);
  যদি(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    ফলাফল = CURLE_FAILED_INIT;
    যাও out;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->গ্রন্থি);
  tslot = ফাঁকা;
  ফলাফল = CURLE_OK;

out:
  যদি(tslot)
    thrdslot_destroy(tslot);
  ফেরত ফলাফল;
}

স্থির শূন্য thrdpool_wake_all(কাঠামো curl_thrdpool *tpool)
{
  কাঠামো Curl_llist_গ্রন্থি *e;
  জন্য(e = Curl_llist_শীর্ষ(&tpool->slots); e; e = Curl_গ্রন্থি_পরবর্তী(e)) {
    কাঠামো thrdslot *tslot = Curl_গ্রন্থি_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

স্থির শূন্য thrdpool_join_zombies(কাঠামো curl_thrdpool *tpool)
{
  কাঠামো Curl_llist_গ্রন্থি *e;

  জন্য(e = Curl_llist_শীর্ষ(&tpool->zombies); e;
      e = Curl_llist_শীর্ষ(&tpool->zombies)) {
    কাঠামো thrdslot *tslot = Curl_গ্রন্থি_elem(e);

    Curl_গ্রন্থি_remove(&tslot->গ্রন্থি);
    যদি(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->lock);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->lock);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

স্থির bool thrdpool_unlink(কাঠামো curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  যদি(tpool->refcount)
    tpool->refcount--;
  যদি(tpool->refcount)
    ফেরত FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  যদি(locked)
    Curl_mutex_release(&tpool->lock);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->lock);
  curlx_free(tpool);
  ফেরত TRUE;
}

স্থির CURLcode thrdpool_signal(কাঠামো curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  কাঠামো Curl_llist_গ্রন্থি *e, *n;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  জন্য(e = Curl_llist_শীর্ষ(&tpool->slots); e && nthreads; e = n) {
    কাঠামো thrdslot *tslot = Curl_গ্রন্থি_elem(e);
    n = Curl_গ্রন্থি_পরবর্তী(e);
    যদি(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    অন্যথা যদি(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  যখন(nthreads && !ফলাফল &&
        Curl_llist_গুনতি(&tpool->slots) < tpool->max_threads) {
    ফলাফল = thrdslot_start(tpool);
    যদি(ফলাফল)
      বিরতি;
    --nthreads;
  }

  ফেরত ফলাফল;
}

CURLcode Curl_thrdpool_set_props(কাঠামো curl_thrdpool *tpool,
                                 uint32_t min_threads,
                                 uint32_t max_threads,
                                 uint32_t idle_time_ms)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t running;

  যদি(!max_threads || (min_threads > max_threads))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mutex_acquire(&tpool->lock);
  tpool->min_threads = min_threads;
  tpool->max_threads = max_threads;
  tpool->idle_time_ms = idle_time_ms;
  running = Curl_llist_গুনতি(&tpool->slots);
  যদি(tpool->min_threads > running) {
    ফলাফল = thrdpool_signal(tpool, tpool->min_threads - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->lock);

  ফেরত ফলাফল;
}

CURLcode Curl_thrdpool_create(কাঠামো curl_thrdpool **ptpool,
                              ধ্রুবক অক্ষর *name,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms,
                              Curl_thrdpool_take_item_cb *fn_take,
                              Curl_thrdpool_process_item_cb *fn_process,
                              Curl_thrdpool_return_item_cb *fn_return,
                              শূন্য *user_তথ্য)
{
  কাঠামো curl_thrdpool *tpool;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, আকার(*tpool));
  যদি(!tpool)
    যাও out;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->lock);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, ফাঁকা);
  Curl_llist_init(&tpool->zombies, ফাঁকা);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_তথ্য = user_তথ্য;

  /* a const string that remains */
  tpool->name = name;

#ifdef DEBUGBUILD
  {
    ধ্রুবক অক্ষর *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, INT_MAX))
        tpool->dbg_fail_starts = (পূর্ণসংখ্যা)l;
    }
  }
#endif

  ফলাফল = Curl_thrdpool_set_props(tpool, min_threads, max_threads,
                                   idle_time_ms);

out:
  যদি(ফলাফল && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = ফাঁকা;
  }
  *ptpool = tpool;
  ফেরত ফলাফল;
}

শূন্য Curl_thrdpool_destroy(কাঠামো curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->lock);

  tpool->aborted = TRUE;

  যখন(join && Curl_llist_গুনতি(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->lock);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  যদি(Curl_llist_গুনতি(&tpool->slots)) {
    কাঠামো Curl_llist_গ্রন্থি *e;
    জন্য(e = Curl_llist_শীর্ষ(&tpool->slots); e; e = Curl_গ্রন্থি_পরবর্তী(e)) {
      কাঠামো thrdslot *tslot = Curl_গ্রন্থি_elem(e);
      যদি(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  যদি(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
}

CURLcode Curl_thrdpool_signal(কাঠামো curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode ফলাফল;

  Curl_mutex_acquire(&tpool->lock);
  ফলাফল = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->lock);
  ফেরত ফলাফল;
}

স্থির bool thrdpool_all_idle(কাঠামো curl_thrdpool *tpool)
{
  কাঠামো Curl_llist_গ্রন্থি *e;
  জন্য(e = Curl_llist_শীর্ষ(&tpool->slots); e; e = Curl_গ্রন্থি_পরবর্তী(e)) {
    কাঠামো thrdslot *tslot = Curl_গ্রন্থি_elem(e);
    যদি(!tslot->idle)
      ফেরত FALSE;
  }
  ফেরত TRUE;
}

CURLcode Curl_thrdpool_await_idle(কাঠামো curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো curltime end = { 0 };

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(!tpool->aborted);
  যদি(tpool->aborted) {
    ফলাফল = CURLE_FAILED_INIT;
    যাও out;
  }

  যখন(!thrdpool_all_idle(tpool)) {
    যদি(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      যদি(!end.tv_sec && !end.tv_usec) {
        end = curlx_now();
        end.tv_sec += (time_t)(timeout_ms / 1000);
        end.tv_usec += (পূর্ণসংখ্যা)(timeout_ms % 1000) * 1000;
        যদি(end.tv_usec >= 1000000) {
          end.tv_sec++;
          end.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), end);
      যদি(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      অন্যথা
        r = Curl_cond_timedwait(&tpool->await, &tpool->lock,
                                (uint32_t)remain_ms);
      যদি(r == CURLE_OPERATION_TIMEDOUT) {
        ফলাফল = r;
        বিরতি;
      }
    }
    অন্যথা {
      Curl_cond_wait(&tpool->await, &tpool->lock);
    }
  }

out:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->lock);
  ফেরত ফলাফল;
}

#ifdef CURLVERBOSE
শূন্য Curl_thrdpool_trace(কাঠামো curl_thrdpool *tpool,
                         কাঠামো Curl_easy *তথ্য)
{
  কাঠামো curl_trc_feat *feat = &Curl_trc_feat_threads;
  যদি(Curl_trc_ft_is_verbose(তথ্য, feat)) {
    কাঠামো Curl_llist_গ্রন্থি *e;
    কাঠামো curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->lock);
    যদি(!Curl_llist_গুনতি(&tpool->slots)) {
      Curl_trc_feat_infof(তথ্য, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    জন্য(e = Curl_llist_শীর্ষ(&tpool->slots); e; e = Curl_গ্রন্থি_পরবর্তী(e)) {
      কাঠামো thrdslot *tslot = Curl_গ্রন্থি_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      যদি(!tslot->running) {
        Curl_trc_feat_infof(তথ্য, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      অন্যথা যদি(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(তথ্য, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      অন্যথা যদি(tslot->idle) {
        Curl_trc_feat_infof(তথ্য, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      অন্যথা {
        timediff_t remain_ms = tslot->work_timeout_ms ?
          (tslot->work_timeout_ms - elapsed_ms) : 0;
        Curl_trc_feat_infof(তথ্য, feat, "[TPOOL-%s] [%u]: busy %"
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
