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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਧਾਗੇ

#ਅਨ੍ਤਰ੍ਭਾਵ "llist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_threads.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "thrdpool.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਜੇ_ਅੰਤ


ਬਣਤਰ thrdslot {
  ਬਣਤਰ Curl_llist_ਗੰਢ ਗੰਢ;
  ਬਣਤਰ curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  ਬਣਤਰ curltime starttime;
  ਅਚਲ ਅੱਖਰ *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  ਦੋਆਧਾਰੀ_ਅੰਕ(running);
  ਦੋਆਧਾਰੀ_ਅੰਕ(idle);
};

ਬਣਤਰ curl_thrdpool {
  ਅਚਲ ਅੱਖਰ *name;
  uint64_t refcount;
  curl_mutex_t ਤਾਲਾ;
  curl_cond_t await;
  ਬਣਤਰ Curl_llist slots;
  ਬਣਤਰ Curl_llist zombies;
  Curl_thrdpool_take_ਵਸਤੂ_cb *fn_take;
  Curl_thrdpool_process_ਵਸਤੂ_cb *fn_process;
  Curl_thrdpool_return_ਵਸਤੂ_cb *fn_return;
  ਖਾਲੀ *fn_user_ਅੰਕੜੇ;
  CURLcode fatal_err;
  uint32_t min_ਧਾਗੇ;
  uint32_t max_ਧਾਗੇ;
  uint32_t idle_ਸਮਾਂ_ms;
  uint32_t ਅਗਲਾ_id;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਪੂਰਨ dbg_fail_starts; /* fail this many thread starts */
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(aborted);
  ਦੋਆਧਾਰੀ_ਅੰਕ(detached);
};

ਸਥਿਰ ਖਾਲੀ thrdpool_join_zombies(ਬਣਤਰ curl_thrdpool *tpool);
ਸਥਿਰ bool thrdpool_unlink(ਬਣਤਰ curl_thrdpool *tpool, bool locked);

ਸਥਿਰ ਖਾਲੀ thrdslot_destroy(ਬਣਤਰ thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

ਸਥਿਰ ਖਾਲੀ thrdslot_ਪੂਰਾ(ਬਣਤਰ thrdslot *tslot)
{
  ਬਣਤਰ curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_ਗੰਢ_llist(&tslot->ਗੰਢ) == &tpool->slots);
  Curl_ਗੰਢ_remove(&tslot->ਗੰਢ);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->ਗੰਢ);
  Curl_cond_signal(&tpool->await);
}

ਸਥਿਰ CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_ਲੜੀਵਾਰ(ਖਾਲੀ *arg)
{
  ਬਣਤਰ thrdslot *tslot = arg;
  ਬਣਤਰ curl_thrdpool *tpool = tslot->tpool;
  ਖਾਲੀ *ਵਸਤੂ;

  Curl_mutex_acquire(&tpool->ਤਾਲਾ);
  DEBUGASSERT(Curl_ਗੰਢ_llist(&tslot->ਗੰਢ) == &tpool->slots);
  ਲਈ(;;) {
    ਜਦੋਂ(!tpool->aborted) {
      tslot->work_description = ਨਿਰਾ;
      tslot->work_timeout_ms = 0;
      ਵਸਤੂ = tpool->fn_take(tpool->fn_user_ਅੰਕੜੇ, &tslot->work_description,
                            &tslot->work_timeout_ms);
      ਜੇ(!ਵਸਤੂ)
        ਰੋਕੋ;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->ਤਾਲਾ);

      tpool->fn_process(ਵਸਤੂ);

      Curl_mutex_acquire(&tpool->ਤਾਲਾ);
      tslot->work_description = ਨਿਰਾ;
      tpool->fn_return(ਵਸਤੂ, tpool->aborted ? ਨਿਰਾ : tpool->fn_user_ਅੰਕੜੇ);
    }

    ਜੇ(tpool->aborted ||
       (Curl_llist_ਗਿਣਤੀ(&tpool->slots) > tpool->max_ਧਾਗੇ))
      ਜਾਓ ਬਾਹਰਲਾ;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    ਜੇ((tpool->idle_ਸਮਾਂ_ms > 0) &&
       (Curl_llist_ਗਿਣਤੀ(&tpool->slots) > tpool->min_ਧਾਗੇ)) {
      CURLcode ਨਤੀਜਾ = Curl_cond_timedwait(&tslot->await, &tpool->ਤਾਲਾ,
                                            tpool->idle_ਸਮਾਂ_ms);
      ਜੇ((ਨਤੀਜਾ == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_ਗਿਣਤੀ(&tpool->slots) > tpool->min_ਧਾਗੇ)) {
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    ਨਹੀਂਤਾਂ {
      Curl_cond_wait(&tslot->await, &tpool->ਤਾਲਾ);
    }
  }

ਬਾਹਰਲਾ:
  thrdslot_ਪੂਰਾ(tslot);
  ਜੇ(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->ਤਾਲਾ);
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ CURLcode thrdslot_ਸ਼ੁਰੂ(ਬਣਤਰ curl_thrdpool *tpool)
{
  ਬਣਤਰ thrdslot *tslot;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  tslot = curlx_calloc(1, ਆਕਾਰ(*tslot));
  ਜੇ(!tslot)
    ਜਾਓ ਬਾਹਰਲਾ;
  tslot->id = tpool->ਅਗਲਾ_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    tslot->thread = Curl_thread_create(thrdslot_ਲੜੀਵਾਰ, tslot);
  ਜੇ(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    ਨਤੀਜਾ = CURLE_FAILED_INIT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->ਗੰਢ);
  tslot = ਨਿਰਾ;
  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  ਜੇ(tslot)
    thrdslot_destroy(tslot);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ thrdpool_wake_all(ਬਣਤਰ curl_thrdpool *tpool)
{
  ਬਣਤਰ Curl_llist_ਗੰਢ *e;
  ਲਈ(e = Curl_llist_ਸਿਰ(&tpool->slots); e; e = Curl_ਗੰਢ_ਅਗਲਾ(e)) {
    ਬਣਤਰ thrdslot *tslot = Curl_ਗੰਢ_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

ਸਥਿਰ ਖਾਲੀ thrdpool_join_zombies(ਬਣਤਰ curl_thrdpool *tpool)
{
  ਬਣਤਰ Curl_llist_ਗੰਢ *e;

  ਲਈ(e = Curl_llist_ਸਿਰ(&tpool->zombies); e;
      e = Curl_llist_ਸਿਰ(&tpool->zombies)) {
    ਬਣਤਰ thrdslot *tslot = Curl_ਗੰਢ_elem(e);

    Curl_ਗੰਢ_remove(&tslot->ਗੰਢ);
    ਜੇ(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->ਤਾਲਾ);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->ਤਾਲਾ);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

ਸਥਿਰ bool thrdpool_unlink(ਬਣਤਰ curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  ਜੇ(tpool->refcount)
    tpool->refcount--;
  ਜੇ(tpool->refcount)
    ਵਾਪਸ FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  ਜੇ(locked)
    Curl_mutex_release(&tpool->ਤਾਲਾ);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->ਤਾਲਾ);
  curlx_free(tpool);
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ CURLcode thrdpool_signal(ਬਣਤਰ curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  ਬਣਤਰ Curl_llist_ਗੰਢ *e, *n;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  ਲਈ(e = Curl_llist_ਸਿਰ(&tpool->slots); e && nthreads; e = n) {
    ਬਣਤਰ thrdslot *tslot = Curl_ਗੰਢ_elem(e);
    n = Curl_ਗੰਢ_ਅਗਲਾ(e);
    ਜੇ(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  ਜਦੋਂ(nthreads && !ਨਤੀਜਾ &&
        Curl_llist_ਗਿਣਤੀ(&tpool->slots) < tpool->max_ਧਾਗੇ) {
    ਨਤੀਜਾ = thrdslot_ਸ਼ੁਰੂ(tpool);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
    --nthreads;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_thrdpool_ਸੈੱਟ_props(ਬਣਤਰ curl_thrdpool *tpool,
                                 uint32_t min_ਧਾਗੇ,
                                 uint32_t max_ਧਾਗੇ,
                                 uint32_t idle_ਸਮਾਂ_ms)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t running;

  ਜੇ(!max_ਧਾਗੇ || (min_ਧਾਗੇ > max_ਧਾਗੇ))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  Curl_mutex_acquire(&tpool->ਤਾਲਾ);
  tpool->min_ਧਾਗੇ = min_ਧਾਗੇ;
  tpool->max_ਧਾਗੇ = max_ਧਾਗੇ;
  tpool->idle_ਸਮਾਂ_ms = idle_ਸਮਾਂ_ms;
  running = Curl_llist_ਗਿਣਤੀ(&tpool->slots);
  ਜੇ(tpool->min_ਧਾਗੇ > running) {
    ਨਤੀਜਾ = thrdpool_signal(tpool, tpool->min_ਧਾਗੇ - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->ਤਾਲਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_thrdpool_create(ਬਣਤਰ curl_thrdpool **ptpool,
                              ਅਚਲ ਅੱਖਰ *name,
                              uint32_t min_ਧਾਗੇ,
                              uint32_t max_ਧਾਗੇ,
                              uint32_t idle_ਸਮਾਂ_ms,
                              Curl_thrdpool_take_ਵਸਤੂ_cb *fn_take,
                              Curl_thrdpool_process_ਵਸਤੂ_cb *fn_process,
                              Curl_thrdpool_return_ਵਸਤੂ_cb *fn_return,
                              ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  ਬਣਤਰ curl_thrdpool *tpool;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, ਆਕਾਰ(*tpool));
  ਜੇ(!tpool)
    ਜਾਓ ਬਾਹਰਲਾ;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->ਤਾਲਾ);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, ਨਿਰਾ);
  Curl_llist_init(&tpool->zombies, ਨਿਰਾ);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_ਅੰਕੜੇ = user_ਅੰਕੜੇ;

  /* a const string that remains */
  tpool->name = name;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  {
    ਅਚਲ ਅੱਖਰ *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    ਜੇ(p) {
      curl_off_t l;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ))
        tpool->dbg_fail_starts = (ਪੂਰਨ)l;
    }
  }
#ਜੇ_ਅੰਤ

  ਨਤੀਜਾ = Curl_thrdpool_ਸੈੱਟ_props(tpool, min_ਧਾਗੇ, max_ਧਾਗੇ,
                                   idle_ਸਮਾਂ_ms);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = ਨਿਰਾ;
  }
  *ptpool = tpool;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_thrdpool_destroy(ਬਣਤਰ curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->ਤਾਲਾ);

  tpool->aborted = TRUE;

  ਜਦੋਂ(join && Curl_llist_ਗਿਣਤੀ(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->ਤਾਲਾ);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  ਜੇ(Curl_llist_ਗਿਣਤੀ(&tpool->slots)) {
    ਬਣਤਰ Curl_llist_ਗੰਢ *e;
    ਲਈ(e = Curl_llist_ਸਿਰ(&tpool->slots); e; e = Curl_ਗੰਢ_ਅਗਲਾ(e)) {
      ਬਣਤਰ thrdslot *tslot = Curl_ਗੰਢ_elem(e);
      ਜੇ(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  ਜੇ(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->ਤਾਲਾ);
  }
}

CURLcode Curl_thrdpool_signal(ਬਣਤਰ curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode ਨਤੀਜਾ;

  Curl_mutex_acquire(&tpool->ਤਾਲਾ);
  ਨਤੀਜਾ = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->ਤਾਲਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool thrdpool_all_idle(ਬਣਤਰ curl_thrdpool *tpool)
{
  ਬਣਤਰ Curl_llist_ਗੰਢ *e;
  ਲਈ(e = Curl_llist_ਸਿਰ(&tpool->slots); e; e = Curl_ਗੰਢ_ਅਗਲਾ(e)) {
    ਬਣਤਰ thrdslot *tslot = Curl_ਗੰਢ_elem(e);
    ਜੇ(!tslot->idle)
      ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE;
}

CURLcode Curl_thrdpool_await_idle(ਬਣਤਰ curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ curltime ਸਿਰਾ = { 0 };

  Curl_mutex_acquire(&tpool->ਤਾਲਾ);
  DEBUGASSERT(!tpool->aborted);
  ਜੇ(tpool->aborted) {
    ਨਤੀਜਾ = CURLE_FAILED_INIT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜਦੋਂ(!thrdpool_all_idle(tpool)) {
    ਜੇ(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      ਜੇ(!ਸਿਰਾ.tv_sec && !ਸਿਰਾ.tv_usec) {
        ਸਿਰਾ = curlx_now();
        ਸਿਰਾ.tv_sec += (time_t)(timeout_ms / 1000);
        ਸਿਰਾ.tv_usec += (ਪੂਰਨ)(timeout_ms % 1000) * 1000;
        ਜੇ(ਸਿਰਾ.tv_usec >= 1000000) {
          ਸਿਰਾ.tv_sec++;
          ਸਿਰਾ.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), ਸਿਰਾ);
      ਜੇ(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      ਨਹੀਂਤਾਂ
        r = Curl_cond_timedwait(&tpool->await, &tpool->ਤਾਲਾ,
                                (uint32_t)remain_ms);
      ਜੇ(r == CURLE_OPERATION_TIMEDOUT) {
        ਨਤੀਜਾ = r;
        ਰੋਕੋ;
      }
    }
    ਨਹੀਂਤਾਂ {
      Curl_cond_wait(&tpool->await, &tpool->ਤਾਲਾ);
    }
  }

ਬਾਹਰਲਾ:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->ਤਾਲਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਖਾਲੀ Curl_thrdpool_trace(ਬਣਤਰ curl_thrdpool *tpool,
                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ curl_trc_feat *feat = &Curl_trc_feat_ਧਾਗੇ;
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, feat)) {
    ਬਣਤਰ Curl_llist_ਗੰਢ *e;
    ਬਣਤਰ curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->ਤਾਲਾ);
    ਜੇ(!Curl_llist_ਗਿਣਤੀ(&tpool->slots)) {
      Curl_trc_feat_infof(ਅੰਕੜੇ, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    ਲਈ(e = Curl_llist_ਸਿਰ(&tpool->slots); e; e = Curl_ਗੰਢ_ਅਗਲਾ(e)) {
      ਬਣਤਰ thrdslot *tslot = Curl_ਗੰਢ_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      ਜੇ(!tslot->running) {
        Curl_trc_feat_infof(ਅੰਕੜੇ, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      ਨਹੀਂਤਾਂ ਜੇ(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(ਅੰਕੜੇ, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      ਨਹੀਂਤਾਂ ਜੇ(tslot->idle) {
        Curl_trc_feat_infof(ਅੰਕੜੇ, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      ਨਹੀਂਤਾਂ {
        timediff_t remain_ms = tslot->work_timeout_ms ?
          (tslot->work_timeout_ms - elapsed_ms) : 0;
        Curl_trc_feat_infof(ਅੰਕੜੇ, feat, "[TPOOL-%s] [%u]: busy %"
                            FMT_TIMEDIFF_T "ms, timeout in %" FMT_TIMEDIFF_T
                            "ms: %s",
                            tpool->name, tslot->id, elapsed_ms, remain_ms,
                            tslot->work_description);
      }
    }
    Curl_mutex_release(&tpool->ਤਾਲਾ);
  }
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* USE_THREADS */
