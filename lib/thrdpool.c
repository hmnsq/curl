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

#اگر_تعریف_شده USE_نخها

#ضمن "llist.h"
#ضمن "curl_threads.h"
#ضمن "curlx/timeval.h"
#ضمن "curlx/strparse.h"
#ضمن "thrdpool.h"
#اگر_تعریف_شده CURLVERBOSE
#ضمن "curl_trc.h"
#ضمن "urldata.h"
#پایان_اگر


بنية thrdslot {
  بنية Curl_llist_گره گره;
  بنية curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  بنية curltime starttime;
  ثابت حرف *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  بیت(running);
  بیت(idle);
};

بنية curl_thrdpool {
  ثابت حرف *name;
  uint64_t refcount;
  curl_mutex_t قفل;
  curl_cond_t await;
  بنية Curl_llist slots;
  بنية Curl_llist zombies;
  Curl_thrdpool_take_قلم_cb *fn_take;
  Curl_thrdpool_process_قلم_cb *fn_process;
  Curl_thrdpool_return_قلم_cb *fn_return;
  فراغ *fn_user_داده;
  CURLcode fatal_err;
  uint32_t min_نخها;
  uint32_t max_نخها;
  uint32_t idle_زمان_ms;
  uint32_t بعدی_id;
#اگر_تعریف_شده DEBUGBUILD
  صحيح dbg_fail_starts; /* fail this many thread starts */
#پایان_اگر
  بیت(aborted);
  بیت(detached);
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

ساكن فراغ thrdslot_انجامشده(بنية thrdslot *tslot)
{
  بنية curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_گره_llist(&tslot->گره) == &tpool->slots);
  Curl_گره_remove(&tslot->گره);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->گره);
  Curl_cond_signal(&tpool->await);
}

ساكن CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_پیاپی(فراغ *arg)
{
  بنية thrdslot *tslot = arg;
  بنية curl_thrdpool *tpool = tslot->tpool;
  فراغ *قلم;

  Curl_mutex_acquire(&tpool->قفل);
  DEBUGASSERT(Curl_گره_llist(&tslot->گره) == &tpool->slots);
  لكل(;;) {
    طالما(!tpool->aborted) {
      tslot->work_description = عدم;
      tslot->work_timeout_ms = 0;
      قلم = tpool->fn_take(tpool->fn_user_داده, &tslot->work_description,
                            &tslot->work_timeout_ms);
      إذا(!قلم)
        اكسر;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->قفل);

      tpool->fn_process(قلم);

      Curl_mutex_acquire(&tpool->قفل);
      tslot->work_description = عدم;
      tpool->fn_return(قلم, tpool->aborted ? عدم : tpool->fn_user_داده);
    }

    إذا(tpool->aborted ||
       (Curl_llist_شمار(&tpool->slots) > tpool->max_نخها))
      اذهب بیرون;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    إذا((tpool->idle_زمان_ms > 0) &&
       (Curl_llist_شمار(&tpool->slots) > tpool->min_نخها)) {
      CURLcode نتیجه = Curl_cond_timedwait(&tslot->await, &tpool->قفل,
                                            tpool->idle_زمان_ms);
      إذا((نتیجه == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_شمار(&tpool->slots) > tpool->min_نخها)) {
        اذهب بیرون;
      }
    }
    وإلا {
      Curl_cond_wait(&tslot->await, &tpool->قفل);
    }
  }

بیرون:
  thrdslot_انجامشده(tslot);
  إذا(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->قفل);
  }
  أرجع 0;
}

ساكن CURLcode thrdslot_شروع(بنية curl_thrdpool *tpool)
{
  بنية thrdslot *tslot;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;

  tslot = curlx_calloc(1, حجم(*tslot));
  إذا(!tslot)
    اذهب بیرون;
  tslot->id = tpool->بعدی_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#اگر_تعریف_شده DEBUGBUILD
  إذا(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  وإلا
#پایان_اگر
    tslot->thread = Curl_thread_create(thrdslot_پیاپی, tslot);
  إذا(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    نتیجه = CURLE_FAILED_INIT;
    اذهب بیرون;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->گره);
  tslot = عدم;
  نتیجه = CURLE_OK;

بیرون:
  إذا(tslot)
    thrdslot_destroy(tslot);
  أرجع نتیجه;
}

ساكن فراغ thrdpool_wake_all(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_گره *e;
  لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_گره_بعدی(e)) {
    بنية thrdslot *tslot = Curl_گره_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

ساكن فراغ thrdpool_join_zombies(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_گره *e;

  لكل(e = Curl_llist_سر(&tpool->zombies); e;
      e = Curl_llist_سر(&tpool->zombies)) {
    بنية thrdslot *tslot = Curl_گره_elem(e);

    Curl_گره_remove(&tslot->گره);
    إذا(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->قفل);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->قفل);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

ساكن bool thrdpool_unlink(بنية curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  إذا(tpool->refcount)
    tpool->refcount--;
  إذا(tpool->refcount)
    أرجع FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  إذا(locked)
    Curl_mutex_release(&tpool->قفل);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->قفل);
  curlx_free(tpool);
  أرجع TRUE;
}

ساكن CURLcode thrdpool_signal(بنية curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  بنية Curl_llist_گره *e, *n;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  لكل(e = Curl_llist_سر(&tpool->slots); e && nthreads; e = n) {
    بنية thrdslot *tslot = Curl_گره_elem(e);
    n = Curl_گره_بعدی(e);
    إذا(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    وإلا إذا(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  طالما(nthreads && !نتیجه &&
        Curl_llist_شمار(&tpool->slots) < tpool->max_نخها) {
    نتیجه = thrdslot_شروع(tpool);
    إذا(نتیجه)
      اكسر;
    --nthreads;
  }

  أرجع نتیجه;
}

CURLcode Curl_thrdpool_مجموعه_props(بنية curl_thrdpool *tpool,
                                 uint32_t min_نخها,
                                 uint32_t max_نخها,
                                 uint32_t idle_زمان_ms)
{
  CURLcode نتیجه = CURLE_OK;
  size_t running;

  إذا(!max_نخها || (min_نخها > max_نخها))
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  Curl_mutex_acquire(&tpool->قفل);
  tpool->min_نخها = min_نخها;
  tpool->max_نخها = max_نخها;
  tpool->idle_زمان_ms = idle_زمان_ms;
  running = Curl_llist_شمار(&tpool->slots);
  إذا(tpool->min_نخها > running) {
    نتیجه = thrdpool_signal(tpool, tpool->min_نخها - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->قفل);

  أرجع نتیجه;
}

CURLcode Curl_thrdpool_create(بنية curl_thrdpool **ptpool,
                              ثابت حرف *name,
                              uint32_t min_نخها,
                              uint32_t max_نخها,
                              uint32_t idle_زمان_ms,
                              Curl_thrdpool_take_قلم_cb *fn_take,
                              Curl_thrdpool_process_قلم_cb *fn_process,
                              Curl_thrdpool_return_قلم_cb *fn_return,
                              فراغ *user_داده)
{
  بنية curl_thrdpool *tpool;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, حجم(*tpool));
  إذا(!tpool)
    اذهب بیرون;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->قفل);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, عدم);
  Curl_llist_init(&tpool->zombies, عدم);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_داده = user_داده;

  /* a const string that remains */
  tpool->name = name;

#اگر_تعریف_شده DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, بیشترین_صحیح))
        tpool->dbg_fail_starts = (صحيح)l;
    }
  }
#پایان_اگر

  نتیجه = Curl_thrdpool_مجموعه_props(tpool, min_نخها, max_نخها,
                                   idle_زمان_ms);

بیرون:
  إذا(نتیجه && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = عدم;
  }
  *ptpool = tpool;
  أرجع نتیجه;
}

فراغ Curl_thrdpool_destroy(بنية curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->قفل);

  tpool->aborted = TRUE;

  طالما(join && Curl_llist_شمار(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->قفل);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  إذا(Curl_llist_شمار(&tpool->slots)) {
    بنية Curl_llist_گره *e;
    لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_گره_بعدی(e)) {
      بنية thrdslot *tslot = Curl_گره_elem(e);
      إذا(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  إذا(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->قفل);
  }
}

CURLcode Curl_thrdpool_signal(بنية curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode نتیجه;

  Curl_mutex_acquire(&tpool->قفل);
  نتیجه = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->قفل);
  أرجع نتیجه;
}

ساكن bool thrdpool_all_idle(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_گره *e;
  لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_گره_بعدی(e)) {
    بنية thrdslot *tslot = Curl_گره_elem(e);
    إذا(!tslot->idle)
      أرجع FALSE;
  }
  أرجع TRUE;
}

CURLcode Curl_thrdpool_await_idle(بنية curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode نتیجه = CURLE_OK;
  بنية curltime انتها = { 0 };

  Curl_mutex_acquire(&tpool->قفل);
  DEBUGASSERT(!tpool->aborted);
  إذا(tpool->aborted) {
    نتیجه = CURLE_FAILED_INIT;
    اذهب بیرون;
  }

  طالما(!thrdpool_all_idle(tpool)) {
    إذا(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      إذا(!انتها.tv_sec && !انتها.tv_usec) {
        انتها = curlx_now();
        انتها.tv_sec += (time_t)(timeout_ms / 1000);
        انتها.tv_usec += (صحيح)(timeout_ms % 1000) * 1000;
        إذا(انتها.tv_usec >= 1000000) {
          انتها.tv_sec++;
          انتها.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), انتها);
      إذا(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      وإلا
        r = Curl_cond_timedwait(&tpool->await, &tpool->قفل,
                                (uint32_t)remain_ms);
      إذا(r == CURLE_OPERATION_TIMEDOUT) {
        نتیجه = r;
        اكسر;
      }
    }
    وإلا {
      Curl_cond_wait(&tpool->await, &tpool->قفل);
    }
  }

بیرون:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->قفل);
  أرجع نتیجه;
}

#اگر_تعریف_شده CURLVERBOSE
فراغ Curl_thrdpool_trace(بنية curl_thrdpool *tpool,
                         بنية Curl_easy *داده)
{
  بنية curl_trc_feat *feat = &Curl_trc_feat_نخها;
  إذا(Curl_trc_ft_is_verbose(داده, feat)) {
    بنية Curl_llist_گره *e;
    بنية curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->قفل);
    إذا(!Curl_llist_شمار(&tpool->slots)) {
      Curl_trc_feat_infof(داده, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_گره_بعدی(e)) {
      بنية thrdslot *tslot = Curl_گره_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      إذا(!tslot->running) {
        Curl_trc_feat_infof(داده, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      وإلا إذا(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(داده, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      وإلا إذا(tslot->idle) {
        Curl_trc_feat_infof(داده, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      وإلا {
        timediff_t remain_ms = tslot->work_timeout_ms ?
          (tslot->work_timeout_ms - elapsed_ms) : 0;
        Curl_trc_feat_infof(داده, feat, "[TPOOL-%s] [%u]: busy %"
                            FMT_TIMEDIFF_T "ms, timeout in %" FMT_TIMEDIFF_T
                            "ms: %s",
                            tpool->name, tslot->id, elapsed_ms, remain_ms,
                            tslot->work_description);
      }
    }
    Curl_mutex_release(&tpool->قفل);
  }
}
#پایان_اگر

#پایان_اگر /* USE_THREADS */
