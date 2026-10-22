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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಎಳೆಗಳು

#ಅನ್ತರ್ಭಾವ "llist.h"
#ಅನ್ತರ್ಭಾವ "curl_threads.h"
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "thrdpool.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಆದರೆ_ಅಂತ್ಯ


ರಚನೆ thrdslot {
  ರಚನೆ Curl_llist_ಗಂಟು ಗಂಟು;
  ರಚನೆ curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  ರಚನೆ curltime starttime;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  ಬಿಟ್(running);
  ಬಿಟ್(idle);
};

ರಚನೆ curl_thrdpool {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
  uint64_t refcount;
  curl_mutex_t ಬೀಗ;
  curl_cond_t await;
  ರಚನೆ Curl_llist slots;
  ರಚನೆ Curl_llist zombies;
  Curl_thrdpool_take_ಅಂಶ_cb *fn_take;
  Curl_thrdpool_process_ಅಂಶ_cb *fn_process;
  Curl_thrdpool_return_ಅಂಶ_cb *fn_return;
  ಶೂನ್ಯ *fn_user_ದತ್ತಾಂಶ;
  CURLcode fatal_err;
  uint32_t min_ಎಳೆಗಳು;
  uint32_t max_ಎಳೆಗಳು;
  uint32_t idle_ಸಮಯ_ms;
  uint32_t ಮುಂದಿನ_id;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಪೂರ್ಣಾಂಕ dbg_fail_starts; /* fail this many thread starts */
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(aborted);
  ಬಿಟ್(detached);
};

ಸ್ಥಿರ ಶೂನ್ಯ thrdpool_join_zombies(ರಚನೆ curl_thrdpool *tpool);
ಸ್ಥಿರ bool thrdpool_unlink(ರಚನೆ curl_thrdpool *tpool, bool locked);

ಸ್ಥಿರ ಶೂನ್ಯ thrdslot_destroy(ರಚನೆ thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdslot_ಮುಗಿದ(ರಚನೆ thrdslot *tslot)
{
  ರಚನೆ curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_ಗಂಟು_llist(&tslot->ಗಂಟು) == &tpool->slots);
  Curl_ಗಂಟು_remove(&tslot->ಗಂಟು);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->ಗಂಟು);
  Curl_cond_signal(&tpool->await);
}

ಸ್ಥಿರ CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_ಸತತ(ಶೂನ್ಯ *arg)
{
  ರಚನೆ thrdslot *tslot = arg;
  ರಚನೆ curl_thrdpool *tpool = tslot->tpool;
  ಶೂನ್ಯ *ಅಂಶ;

  Curl_mutex_acquire(&tpool->ಬೀಗ);
  DEBUGASSERT(Curl_ಗಂಟು_llist(&tslot->ಗಂಟು) == &tpool->slots);
  ಗಾಗಿ(;;) {
    ಆಗ(!tpool->aborted) {
      tslot->work_description = ಖಾಲಿ;
      tslot->work_timeout_ms = 0;
      ಅಂಶ = tpool->fn_take(tpool->fn_user_ದತ್ತಾಂಶ, &tslot->work_description,
                            &tslot->work_timeout_ms);
      ಆದರೆ(!ಅಂಶ)
        ನಿಲ್ಲಿಸು;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->ಬೀಗ);

      tpool->fn_process(ಅಂಶ);

      Curl_mutex_acquire(&tpool->ಬೀಗ);
      tslot->work_description = ಖಾಲಿ;
      tpool->fn_return(ಅಂಶ, tpool->aborted ? ಖಾಲಿ : tpool->fn_user_ದತ್ತಾಂಶ);
    }

    ಆದರೆ(tpool->aborted ||
       (Curl_llist_ಎಣಿಕೆ(&tpool->slots) > tpool->max_ಎಳೆಗಳು))
      ಹೋಗು ಹೊರಹೋಗು;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    ಆದರೆ((tpool->idle_ಸಮಯ_ms > 0) &&
       (Curl_llist_ಎಣಿಕೆ(&tpool->slots) > tpool->min_ಎಳೆಗಳು)) {
      CURLcode ಫಲಿತಾಂಶ = Curl_cond_timedwait(&tslot->await, &tpool->ಬೀಗ,
                                            tpool->idle_ಸಮಯ_ms);
      ಆದರೆ((ಫಲಿತಾಂಶ == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_ಎಣಿಕೆ(&tpool->slots) > tpool->min_ಎಳೆಗಳು)) {
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      Curl_cond_wait(&tslot->await, &tpool->ಬೀಗ);
    }
  }

ಹೊರಹೋಗು:
  thrdslot_ಮುಗಿದ(tslot);
  ಆದರೆ(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->ಬೀಗ);
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ CURLcode thrdslot_ಆರಂಭ(ರಚನೆ curl_thrdpool *tpool)
{
  ರಚನೆ thrdslot *tslot;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  tslot = curlx_calloc(1, ಗಾತ್ರ(*tslot));
  ಆದರೆ(!tslot)
    ಹೋಗು ಹೊರಹೋಗು;
  tslot->id = tpool->ಮುಂದಿನ_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    tslot->thread = Curl_thread_create(thrdslot_ಸತತ, tslot);
  ಆದರೆ(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->ಗಂಟು);
  tslot = ಖಾಲಿ;
  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  ಆದರೆ(tslot)
    thrdslot_destroy(tslot);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdpool_wake_all(ರಚನೆ curl_thrdpool *tpool)
{
  ರಚನೆ Curl_llist_ಗಂಟು *e;
  ಗಾಗಿ(e = Curl_llist_ತಲೆ(&tpool->slots); e; e = Curl_ಗಂಟು_ಮುಂದಿನ(e)) {
    ರಚನೆ thrdslot *tslot = Curl_ಗಂಟು_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ thrdpool_join_zombies(ರಚನೆ curl_thrdpool *tpool)
{
  ರಚನೆ Curl_llist_ಗಂಟು *e;

  ಗಾಗಿ(e = Curl_llist_ತಲೆ(&tpool->zombies); e;
      e = Curl_llist_ತಲೆ(&tpool->zombies)) {
    ರಚನೆ thrdslot *tslot = Curl_ಗಂಟು_elem(e);

    Curl_ಗಂಟು_remove(&tslot->ಗಂಟು);
    ಆದರೆ(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->ಬೀಗ);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->ಬೀಗ);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

ಸ್ಥಿರ bool thrdpool_unlink(ರಚನೆ curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  ಆದರೆ(tpool->refcount)
    tpool->refcount--;
  ಆದರೆ(tpool->refcount)
    ಹಿಂತಿರುಗಿ FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  ಆದರೆ(locked)
    Curl_mutex_release(&tpool->ಬೀಗ);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->ಬೀಗ);
  curlx_free(tpool);
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ CURLcode thrdpool_signal(ರಚನೆ curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  ರಚನೆ Curl_llist_ಗಂಟು *e, *n;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  ಗಾಗಿ(e = Curl_llist_ತಲೆ(&tpool->slots); e && nthreads; e = n) {
    ರಚನೆ thrdslot *tslot = Curl_ಗಂಟು_elem(e);
    n = Curl_ಗಂಟು_ಮುಂದಿನ(e);
    ಆದರೆ(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  ಆಗ(nthreads && !ಫಲಿತಾಂಶ &&
        Curl_llist_ಎಣಿಕೆ(&tpool->slots) < tpool->max_ಎಳೆಗಳು) {
    ಫಲಿತಾಂಶ = thrdslot_ಆರಂಭ(tpool);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
    --nthreads;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_thrdpool_ಗಣ_props(ರಚನೆ curl_thrdpool *tpool,
                                 uint32_t min_ಎಳೆಗಳು,
                                 uint32_t max_ಎಳೆಗಳು,
                                 uint32_t idle_ಸಮಯ_ms)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t running;

  ಆದರೆ(!max_ಎಳೆಗಳು || (min_ಎಳೆಗಳು > max_ಎಳೆಗಳು))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  Curl_mutex_acquire(&tpool->ಬೀಗ);
  tpool->min_ಎಳೆಗಳು = min_ಎಳೆಗಳು;
  tpool->max_ಎಳೆಗಳು = max_ಎಳೆಗಳು;
  tpool->idle_ಸಮಯ_ms = idle_ಸಮಯ_ms;
  running = Curl_llist_ಎಣಿಕೆ(&tpool->slots);
  ಆದರೆ(tpool->min_ಎಳೆಗಳು > running) {
    ಫಲಿತಾಂಶ = thrdpool_signal(tpool, tpool->min_ಎಳೆಗಳು - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->ಬೀಗ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_thrdpool_create(ರಚನೆ curl_thrdpool **ptpool,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                              uint32_t min_ಎಳೆಗಳು,
                              uint32_t max_ಎಳೆಗಳು,
                              uint32_t idle_ಸಮಯ_ms,
                              Curl_thrdpool_take_ಅಂಶ_cb *fn_take,
                              Curl_thrdpool_process_ಅಂಶ_cb *fn_process,
                              Curl_thrdpool_return_ಅಂಶ_cb *fn_return,
                              ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  ರಚನೆ curl_thrdpool *tpool;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, ಗಾತ್ರ(*tpool));
  ಆದರೆ(!tpool)
    ಹೋಗು ಹೊರಹೋಗು;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->ಬೀಗ);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, ಖಾಲಿ);
  Curl_llist_init(&tpool->zombies, ಖಾಲಿ);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_ದತ್ತಾಂಶ = user_ದತ್ತಾಂಶ;

  /* a const string that remains */
  tpool->name = name;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    ಆದರೆ(p) {
      curl_off_t l;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ))
        tpool->dbg_fail_starts = (ಪೂರ್ಣಾಂಕ)l;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಫಲಿತಾಂಶ = Curl_thrdpool_ಗಣ_props(tpool, min_ಎಳೆಗಳು, max_ಎಳೆಗಳು,
                                   idle_ಸಮಯ_ms);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = ಖಾಲಿ;
  }
  *ptpool = tpool;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_thrdpool_destroy(ರಚನೆ curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->ಬೀಗ);

  tpool->aborted = TRUE;

  ಆಗ(join && Curl_llist_ಎಣಿಕೆ(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->ಬೀಗ);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  ಆದರೆ(Curl_llist_ಎಣಿಕೆ(&tpool->slots)) {
    ರಚನೆ Curl_llist_ಗಂಟು *e;
    ಗಾಗಿ(e = Curl_llist_ತಲೆ(&tpool->slots); e; e = Curl_ಗಂಟು_ಮುಂದಿನ(e)) {
      ರಚನೆ thrdslot *tslot = Curl_ಗಂಟು_elem(e);
      ಆದರೆ(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  ಆದರೆ(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->ಬೀಗ);
  }
}

CURLcode Curl_thrdpool_signal(ರಚನೆ curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode ಫಲಿತಾಂಶ;

  Curl_mutex_acquire(&tpool->ಬೀಗ);
  ಫಲಿತಾಂಶ = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->ಬೀಗ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool thrdpool_all_idle(ರಚನೆ curl_thrdpool *tpool)
{
  ರಚನೆ Curl_llist_ಗಂಟು *e;
  ಗಾಗಿ(e = Curl_llist_ತಲೆ(&tpool->slots); e; e = Curl_ಗಂಟು_ಮುಂದಿನ(e)) {
    ರಚನೆ thrdslot *tslot = Curl_ಗಂಟು_elem(e);
    ಆದರೆ(!tslot->idle)
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

CURLcode Curl_thrdpool_await_idle(ರಚನೆ curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ curltime ಕೊನೆ = { 0 };

  Curl_mutex_acquire(&tpool->ಬೀಗ);
  DEBUGASSERT(!tpool->aborted);
  ಆದರೆ(tpool->aborted) {
    ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆಗ(!thrdpool_all_idle(tpool)) {
    ಆದರೆ(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      ಆದರೆ(!ಕೊನೆ.tv_sec && !ಕೊನೆ.tv_usec) {
        ಕೊನೆ = curlx_now();
        ಕೊನೆ.tv_sec += (time_t)(timeout_ms / 1000);
        ಕೊನೆ.tv_usec += (ಪೂರ್ಣಾಂಕ)(timeout_ms % 1000) * 1000;
        ಆದರೆ(ಕೊನೆ.tv_usec >= 1000000) {
          ಕೊನೆ.tv_sec++;
          ಕೊನೆ.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), ಕೊನೆ);
      ಆದರೆ(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      ಇಲ್ಲದಿದ್ದರೆ
        r = Curl_cond_timedwait(&tpool->await, &tpool->ಬೀಗ,
                                (uint32_t)remain_ms);
      ಆದರೆ(r == CURLE_OPERATION_TIMEDOUT) {
        ಫಲಿತಾಂಶ = r;
        ನಿಲ್ಲಿಸು;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      Curl_cond_wait(&tpool->await, &tpool->ಬೀಗ);
    }
  }

ಹೊರಹೋಗು:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->ಬೀಗ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಶೂನ್ಯ Curl_thrdpool_trace(ರಚನೆ curl_thrdpool *tpool,
                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ curl_trc_feat *feat = &Curl_trc_feat_ಎಳೆಗಳು;
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, feat)) {
    ರಚನೆ Curl_llist_ಗಂಟು *e;
    ರಚನೆ curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->ಬೀಗ);
    ಆದರೆ(!Curl_llist_ಎಣಿಕೆ(&tpool->slots)) {
      Curl_trc_feat_infof(ದತ್ತಾಂಶ, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    ಗಾಗಿ(e = Curl_llist_ತಲೆ(&tpool->slots); e; e = Curl_ಗಂಟು_ಮುಂದಿನ(e)) {
      ರಚನೆ thrdslot *tslot = Curl_ಗಂಟು_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      ಆದರೆ(!tslot->running) {
        Curl_trc_feat_infof(ದತ್ತಾಂಶ, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(ದತ್ತಾಂಶ, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(tslot->idle) {
        Curl_trc_feat_infof(ದತ್ತಾಂಶ, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        timediff_t remain_ms = tslot->work_timeout_ms ?
          (tslot->work_timeout_ms - elapsed_ms) : 0;
        Curl_trc_feat_infof(ದತ್ತಾಂಶ, feat, "[TPOOL-%s] [%u]: busy %"
                            FMT_TIMEDIFF_T "ms, timeout in %" FMT_TIMEDIFF_T
                            "ms: %s",
                            tpool->name, tslot->id, elapsed_ms, remain_ms,
                            tslot->work_description);
      }
    }
    Curl_mutex_release(&tpool->ಬೀಗ);
  }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* USE_THREADS */
