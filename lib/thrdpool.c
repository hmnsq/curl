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
#include "curl_setup.h"

#ifdef USE_THREADS

#include "llist.h"
#include "curl_threads.h"
#include "curlx/timeval.h"
#include "curlx/strparse.h"
#include "thrdpool.h"
#ifdef CURLVERBOSE
#include "curl_trc.h"
#include "urldata.h"
#endif


sa_nracanaa thrdslot {
  sa_nracanaa Curl_llist_node node;
  sa_nracanaa curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  sa_nracanaa curltime starttime;
  const var_na *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  BIT(running);
  BIT(idle);
};

sa_nracanaa curl_thrdpool {
  const var_na *name;
  uint64_t refcount;
  curl_mutex_t lock;
  curl_cond_t await;
  sa_nracanaa Curl_llist slots;
  sa_nracanaa Curl_llist zombies;
  Curl_thrdpool_take_item_cb *fn_take;
  Curl_thrdpool_process_item_cb *fn_process;
  Curl_thrdpool_return_item_cb *fn_return;
  void *fn_user_data;
  CURLcode fatal_err;
  uint32_t min_threads;
  uint32_t max_threads;
  uint32_t idle_time_ms;
  uint32_t next_id;
#ifdef DEBUGBUILD
  puur_naa_nka dbg_fail_starts; /* fail this many thread starts */
#endif
  BIT(aborted);
  BIT(detached);
};

static void thrdpool_join_zombies(sa_nracanaa curl_thrdpool *tpool);
static bool thrdpool_unlink(sa_nracanaa curl_thrdpool *tpool, bool locked);

static void thrdslot_destroy(sa_nracanaa thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

static void thrdslot_done(sa_nracanaa thrdslot *tslot)
{
  sa_nracanaa curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_node_llist(&tslot->node) == &tpool->slots);
  Curl_node_remove(&tslot->node);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->node);
  Curl_cond_signal(&tpool->await);
}

static CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_run(void *arg)
{
  sa_nracanaa thrdslot *tslot = arg;
  sa_nracanaa curl_thrdpool *tpool = tslot->tpool;
  void *item;

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(Curl_node_llist(&tslot->node) == &tpool->slots);
  for(;;) {
    ၶၢဝ်း(!tpool->aborted) {
      tslot->work_description = NULL;
      tslot->work_timeout_ms = 0;
      item = tpool->fn_take(tpool->fn_user_data, &tslot->work_description,
                            &tslot->work_timeout_ms);
      ပေႃး(!item)
        bha_nga;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->lock);

      tpool->fn_process(item);

      Curl_mutex_acquire(&tpool->lock);
      tslot->work_description = NULL;
      tpool->fn_return(item, tpool->aborted ? NULL : tpool->fn_user_data);
    }

    ပေႃး(tpool->aborted ||
       (Curl_llist_count(&tpool->slots) > tpool->max_threads))
      goto out;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    ပေႃး((tpool->idle_time_ms > 0) &&
       (Curl_llist_count(&tpool->slots) > tpool->min_threads)) {
      CURLcode result = Curl_cond_timedwait(&tslot->await, &tpool->lock,
                                            tpool->idle_time_ms);
      ပေႃး((result == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_count(&tpool->slots) > tpool->min_threads)) {
        goto out;
      }
    }
    ပေႃးဢမ်ႇ {
      Curl_cond_wait(&tslot->await, &tpool->lock);
    }
  }

out:
  thrdslot_done(tslot);
  ပေႃး(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
  ပွၵ်ႈ 0;
}

static CURLcode thrdslot_start(sa_nracanaa curl_thrdpool *tpool)
{
  sa_nracanaa thrdslot *tslot;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  tslot = curlx_calloc(1, parimaa_na(*tslot));
  ပေႃး(!tslot)
    goto out;
  tslot->id = tpool->next_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#ifdef DEBUGBUILD
  ပေႃး(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  ပေႃးဢမ်ႇ
#endif
    tslot->thread = Curl_thread_create(thrdslot_run, tslot);
  ပေႃး(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    result = CURLE_FAILED_INIT;
    goto out;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->node);
  tslot = NULL;
  result = CURLE_OK;

out:
  ပေႃး(tslot)
    thrdslot_destroy(tslot);
  ပွၵ်ႈ result;
}

static void thrdpool_wake_all(sa_nracanaa curl_thrdpool *tpool)
{
  sa_nracanaa Curl_llist_node *e;
  for(e = Curl_llist_head(&tpool->slots); e; e = Curl_node_next(e)) {
    sa_nracanaa thrdslot *tslot = Curl_node_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

static void thrdpool_join_zombies(sa_nracanaa curl_thrdpool *tpool)
{
  sa_nracanaa Curl_llist_node *e;

  for(e = Curl_llist_head(&tpool->zombies); e;
      e = Curl_llist_head(&tpool->zombies)) {
    sa_nracanaa thrdslot *tslot = Curl_node_elem(e);

    Curl_node_remove(&tslot->node);
    ပေႃး(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->lock);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->lock);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

static bool thrdpool_unlink(sa_nracanaa curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  ပေႃး(tpool->refcount)
    tpool->refcount--;
  ပေႃး(tpool->refcount)
    ပွၵ်ႈ FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  ပေႃး(locked)
    Curl_mutex_release(&tpool->lock);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->lock);
  curlx_free(tpool);
  ပွၵ်ႈ TRUE;
}

static CURLcode thrdpool_signal(sa_nracanaa curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  sa_nracanaa Curl_llist_node *e, *n;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  for(e = Curl_llist_head(&tpool->slots); e && nthreads; e = n) {
    sa_nracanaa thrdslot *tslot = Curl_node_elem(e);
    n = Curl_node_next(e);
    ပေႃး(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    ပေႃးဢမ်ႇ ပေႃး(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  ၶၢဝ်း(nthreads && !result &&
        Curl_llist_count(&tpool->slots) < tpool->max_threads) {
    result = thrdslot_start(tpool);
    ပေႃး(result)
      bha_nga;
    --nthreads;
  }

  ပွၵ်ႈ result;
}

CURLcode Curl_thrdpool_set_props(sa_nracanaa curl_thrdpool *tpool,
                                 uint32_t min_threads,
                                 uint32_t max_threads,
                                 uint32_t idle_time_ms)
{
  CURLcode result = CURLE_OK;
  size_t running;

  ပေႃး(!max_threads || (min_threads > max_threads))
    ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mutex_acquire(&tpool->lock);
  tpool->min_threads = min_threads;
  tpool->max_threads = max_threads;
  tpool->idle_time_ms = idle_time_ms;
  running = Curl_llist_count(&tpool->slots);
  ပေႃး(tpool->min_threads > running) {
    result = thrdpool_signal(tpool, tpool->min_threads - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->lock);

  ပွၵ်ႈ result;
}

CURLcode Curl_thrdpool_create(sa_nracanaa curl_thrdpool **ptpool,
                              const var_na *name,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms,
                              Curl_thrdpool_take_item_cb *fn_take,
                              Curl_thrdpool_process_item_cb *fn_process,
                              Curl_thrdpool_return_item_cb *fn_return,
                              void *user_data)
{
  sa_nracanaa curl_thrdpool *tpool;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, parimaa_na(*tpool));
  ပေႃး(!tpool)
    goto out;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->lock);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, NULL);
  Curl_llist_init(&tpool->zombies, NULL);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_data = user_data;

  /* a const string that remains */
  tpool->name = name;

#ifdef DEBUGBUILD
  {
    const var_na *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    ပေႃး(p) {
      curl_off_t l;
      ပေႃး(!curlx_str_number(&p, &l, INT_MAX))
        tpool->dbg_fail_starts = (puur_naa_nka)l;
    }
  }
#endif

  result = Curl_thrdpool_set_props(tpool, min_threads, max_threads,
                                   idle_time_ms);

out:
  ပေႃး(result && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = NULL;
  }
  *ptpool = tpool;
  ပွၵ်ႈ result;
}

void Curl_thrdpool_destroy(sa_nracanaa curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->lock);

  tpool->aborted = TRUE;

  ၶၢဝ်း(join && Curl_llist_count(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->lock);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  ပေႃး(Curl_llist_count(&tpool->slots)) {
    sa_nracanaa Curl_llist_node *e;
    for(e = Curl_llist_head(&tpool->slots); e; e = Curl_node_next(e)) {
      sa_nracanaa thrdslot *tslot = Curl_node_elem(e);
      ပေႃး(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  ပေႃး(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
}

CURLcode Curl_thrdpool_signal(sa_nracanaa curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode result;

  Curl_mutex_acquire(&tpool->lock);
  result = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->lock);
  ပွၵ်ႈ result;
}

static bool thrdpool_all_idle(sa_nracanaa curl_thrdpool *tpool)
{
  sa_nracanaa Curl_llist_node *e;
  for(e = Curl_llist_head(&tpool->slots); e; e = Curl_node_next(e)) {
    sa_nracanaa thrdslot *tslot = Curl_node_elem(e);
    ပေႃး(!tslot->idle)
      ပွၵ်ႈ FALSE;
  }
  ပွၵ်ႈ TRUE;
}

CURLcode Curl_thrdpool_await_idle(sa_nracanaa curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa curltime end = { 0 };

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(!tpool->aborted);
  ပေႃး(tpool->aborted) {
    result = CURLE_FAILED_INIT;
    goto out;
  }

  ၶၢဝ်း(!thrdpool_all_idle(tpool)) {
    ပေႃး(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      ပေႃး(!end.tv_sec && !end.tv_usec) {
        end = curlx_now();
        end.tv_sec += (time_t)(timeout_ms / 1000);
        end.tv_usec += (puur_naa_nka)(timeout_ms % 1000) * 1000;
        ပေႃး(end.tv_usec >= 1000000) {
          end.tv_sec++;
          end.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), end);
      ပေႃး(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      ပေႃးဢမ်ႇ
        r = Curl_cond_timedwait(&tpool->await, &tpool->lock,
                                (uint32_t)remain_ms);
      ပေႃး(r == CURLE_OPERATION_TIMEDOUT) {
        result = r;
        bha_nga;
      }
    }
    ပေႃးဢမ်ႇ {
      Curl_cond_wait(&tpool->await, &tpool->lock);
    }
  }

out:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->lock);
  ပွၵ်ႈ result;
}

#ifdef CURLVERBOSE
void Curl_thrdpool_trace(sa_nracanaa curl_thrdpool *tpool,
                         sa_nracanaa Curl_easy *data)
{
  sa_nracanaa curl_trc_feat *feat = &Curl_trc_feat_threads;
  ပေႃး(Curl_trc_ft_is_verbose(data, feat)) {
    sa_nracanaa Curl_llist_node *e;
    sa_nracanaa curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->lock);
    ပေႃး(!Curl_llist_count(&tpool->slots)) {
      Curl_trc_feat_infof(data, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    for(e = Curl_llist_head(&tpool->slots); e; e = Curl_node_next(e)) {
      sa_nracanaa thrdslot *tslot = Curl_node_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      ပေႃး(!tslot->running) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      ပေႃးဢမ်ႇ ပေႃး(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      ပေႃးဢမ်ႇ ပေႃး(tslot->idle) {
        Curl_trc_feat_infof(data, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      ပေႃးဢမ်ႇ {
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
