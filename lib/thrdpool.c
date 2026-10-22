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

#جے_تعریف_شدہ USE_دھاگے

#ضمن "llist.h"
#ضمن "curl_threads.h"
#ضمن "curlx/timeval.h"
#ضمن "curlx/strparse.h"
#ضمن "thrdpool.h"
#جے_تعریف_شدہ CURLVERBOSE
#ضمن "curl_trc.h"
#ضمن "urldata.h"
#جے_انت


بنية thrdslot {
  بنية Curl_llist_گرہ گرہ;
  بنية curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  بنية curltime starttime;
  ثابت حرف *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  ثنائی_ہندسہ(running);
  ثنائی_ہندسہ(idle);
};

بنية curl_thrdpool {
  ثابت حرف *name;
  uint64_t refcount;
  curl_mutex_t جندرا;
  curl_cond_t await;
  بنية Curl_llist slots;
  بنية Curl_llist zombies;
  Curl_thrdpool_take_شے_cb *fn_take;
  Curl_thrdpool_process_شے_cb *fn_process;
  Curl_thrdpool_return_شے_cb *fn_return;
  فراغ *fn_user_انکڑے;
  CURLcode fatal_err;
  uint32_t min_دھاگے;
  uint32_t max_دھاگے;
  uint32_t idle_ویلا_ms;
  uint32_t اگلا_id;
#جے_تعریف_شدہ DEBUGBUILD
  صحيح dbg_fail_starts; /* fail this many thread starts */
#جے_انت
  ثنائی_ہندسہ(aborted);
  ثنائی_ہندسہ(detached);
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

ساكن فراغ thrdslot_پورا(بنية thrdslot *tslot)
{
  بنية curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_گرہ_llist(&tslot->گرہ) == &tpool->slots);
  Curl_گرہ_remove(&tslot->گرہ);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->گرہ);
  Curl_cond_signal(&tpool->await);
}

ساكن CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_لگاتار(فراغ *arg)
{
  بنية thrdslot *tslot = arg;
  بنية curl_thrdpool *tpool = tslot->tpool;
  فراغ *شے;

  Curl_mutex_acquire(&tpool->جندرا);
  DEBUGASSERT(Curl_گرہ_llist(&tslot->گرہ) == &tpool->slots);
  لكل(;;) {
    طالما(!tpool->aborted) {
      tslot->work_description = عدم;
      tslot->work_timeout_ms = 0;
      شے = tpool->fn_take(tpool->fn_user_انکڑے, &tslot->work_description,
                            &tslot->work_timeout_ms);
      إذا(!شے)
        اكسر;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->جندرا);

      tpool->fn_process(شے);

      Curl_mutex_acquire(&tpool->جندرا);
      tslot->work_description = عدم;
      tpool->fn_return(شے, tpool->aborted ? عدم : tpool->fn_user_انکڑے);
    }

    إذا(tpool->aborted ||
       (Curl_llist_گنتی(&tpool->slots) > tpool->max_دھاگے))
      اذهب باہرلا;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    إذا((tpool->idle_ویلا_ms > 0) &&
       (Curl_llist_گنتی(&tpool->slots) > tpool->min_دھاگے)) {
      CURLcode نتیجہ = Curl_cond_timedwait(&tslot->await, &tpool->جندرا,
                                            tpool->idle_ویلا_ms);
      إذا((نتیجہ == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_گنتی(&tpool->slots) > tpool->min_دھاگے)) {
        اذهب باہرلا;
      }
    }
    وإلا {
      Curl_cond_wait(&tslot->await, &tpool->جندرا);
    }
  }

باہرلا:
  thrdslot_پورا(tslot);
  إذا(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->جندرا);
  }
  أرجع 0;
}

ساكن CURLcode thrdslot_شروع(بنية curl_thrdpool *tpool)
{
  بنية thrdslot *tslot;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;

  tslot = curlx_calloc(1, حجم(*tslot));
  إذا(!tslot)
    اذهب باہرلا;
  tslot->id = tpool->اگلا_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#جے_تعریف_شدہ DEBUGBUILD
  إذا(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  وإلا
#جے_انت
    tslot->thread = Curl_thread_create(thrdslot_لگاتار, tslot);
  إذا(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    نتیجہ = CURLE_FAILED_INIT;
    اذهب باہرلا;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->گرہ);
  tslot = عدم;
  نتیجہ = CURLE_OK;

باہرلا:
  إذا(tslot)
    thrdslot_destroy(tslot);
  أرجع نتیجہ;
}

ساكن فراغ thrdpool_wake_all(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_گرہ *e;
  لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_گرہ_اگلا(e)) {
    بنية thrdslot *tslot = Curl_گرہ_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

ساكن فراغ thrdpool_join_zombies(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_گرہ *e;

  لكل(e = Curl_llist_سر(&tpool->zombies); e;
      e = Curl_llist_سر(&tpool->zombies)) {
    بنية thrdslot *tslot = Curl_گرہ_elem(e);

    Curl_گرہ_remove(&tslot->گرہ);
    إذا(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->جندرا);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->جندرا);
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
    Curl_mutex_release(&tpool->جندرا);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->جندرا);
  curlx_free(tpool);
  أرجع TRUE;
}

ساكن CURLcode thrdpool_signal(بنية curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  بنية Curl_llist_گرہ *e, *n;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  لكل(e = Curl_llist_سر(&tpool->slots); e && nthreads; e = n) {
    بنية thrdslot *tslot = Curl_گرہ_elem(e);
    n = Curl_گرہ_اگلا(e);
    إذا(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    وإلا إذا(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  طالما(nthreads && !نتیجہ &&
        Curl_llist_گنتی(&tpool->slots) < tpool->max_دھاگے) {
    نتیجہ = thrdslot_شروع(tpool);
    إذا(نتیجہ)
      اكسر;
    --nthreads;
  }

  أرجع نتیجہ;
}

CURLcode Curl_thrdpool_سیٹ_props(بنية curl_thrdpool *tpool,
                                 uint32_t min_دھاگے,
                                 uint32_t max_دھاگے,
                                 uint32_t idle_ویلا_ms)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t running;

  إذا(!max_دھاگے || (min_دھاگے > max_دھاگے))
    أرجع CURLE_BAD_FUNCTION_دلیل;

  Curl_mutex_acquire(&tpool->جندرا);
  tpool->min_دھاگے = min_دھاگے;
  tpool->max_دھاگے = max_دھاگے;
  tpool->idle_ویلا_ms = idle_ویلا_ms;
  running = Curl_llist_گنتی(&tpool->slots);
  إذا(tpool->min_دھاگے > running) {
    نتیجہ = thrdpool_signal(tpool, tpool->min_دھاگے - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->جندرا);

  أرجع نتیجہ;
}

CURLcode Curl_thrdpool_create(بنية curl_thrdpool **ptpool,
                              ثابت حرف *name,
                              uint32_t min_دھاگے,
                              uint32_t max_دھاگے,
                              uint32_t idle_ویلا_ms,
                              Curl_thrdpool_take_شے_cb *fn_take,
                              Curl_thrdpool_process_شے_cb *fn_process,
                              Curl_thrdpool_return_شے_cb *fn_return,
                              فراغ *user_انکڑے)
{
  بنية curl_thrdpool *tpool;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, حجم(*tpool));
  إذا(!tpool)
    اذهب باہرلا;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->جندرا);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, عدم);
  Curl_llist_init(&tpool->zombies, عدم);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_انکڑے = user_انکڑے;

  /* a const string that remains */
  tpool->name = name;

#جے_تعریف_شدہ DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نمبر(&p, &l, ودھ_توں_ودھ_عدد))
        tpool->dbg_fail_starts = (صحيح)l;
    }
  }
#جے_انت

  نتیجہ = Curl_thrdpool_سیٹ_props(tpool, min_دھاگے, max_دھاگے,
                                   idle_ویلا_ms);

باہرلا:
  إذا(نتیجہ && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = عدم;
  }
  *ptpool = tpool;
  أرجع نتیجہ;
}

فراغ Curl_thrdpool_destroy(بنية curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->جندرا);

  tpool->aborted = TRUE;

  طالما(join && Curl_llist_گنتی(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->جندرا);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  إذا(Curl_llist_گنتی(&tpool->slots)) {
    بنية Curl_llist_گرہ *e;
    لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_گرہ_اگلا(e)) {
      بنية thrdslot *tslot = Curl_گرہ_elem(e);
      إذا(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  إذا(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->جندرا);
  }
}

CURLcode Curl_thrdpool_signal(بنية curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode نتیجہ;

  Curl_mutex_acquire(&tpool->جندرا);
  نتیجہ = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->جندرا);
  أرجع نتیجہ;
}

ساكن bool thrdpool_all_idle(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_گرہ *e;
  لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_گرہ_اگلا(e)) {
    بنية thrdslot *tslot = Curl_گرہ_elem(e);
    إذا(!tslot->idle)
      أرجع FALSE;
  }
  أرجع TRUE;
}

CURLcode Curl_thrdpool_await_idle(بنية curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية curltime سرا = { 0 };

  Curl_mutex_acquire(&tpool->جندرا);
  DEBUGASSERT(!tpool->aborted);
  إذا(tpool->aborted) {
    نتیجہ = CURLE_FAILED_INIT;
    اذهب باہرلا;
  }

  طالما(!thrdpool_all_idle(tpool)) {
    إذا(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      إذا(!سرا.tv_sec && !سرا.tv_usec) {
        سرا = curlx_now();
        سرا.tv_sec += (time_t)(timeout_ms / 1000);
        سرا.tv_usec += (صحيح)(timeout_ms % 1000) * 1000;
        إذا(سرا.tv_usec >= 1000000) {
          سرا.tv_sec++;
          سرا.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), سرا);
      إذا(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      وإلا
        r = Curl_cond_timedwait(&tpool->await, &tpool->جندرا,
                                (uint32_t)remain_ms);
      إذا(r == CURLE_OPERATION_TIMEDOUT) {
        نتیجہ = r;
        اكسر;
      }
    }
    وإلا {
      Curl_cond_wait(&tpool->await, &tpool->جندرا);
    }
  }

باہرلا:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->جندرا);
  أرجع نتیجہ;
}

#جے_تعریف_شدہ CURLVERBOSE
فراغ Curl_thrdpool_trace(بنية curl_thrdpool *tpool,
                         بنية Curl_easy *انکڑے)
{
  بنية curl_trc_feat *feat = &Curl_trc_feat_دھاگے;
  إذا(Curl_trc_ft_is_verbose(انکڑے, feat)) {
    بنية Curl_llist_گرہ *e;
    بنية curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->جندرا);
    إذا(!Curl_llist_گنتی(&tpool->slots)) {
      Curl_trc_feat_infof(انکڑے, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_گرہ_اگلا(e)) {
      بنية thrdslot *tslot = Curl_گرہ_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      إذا(!tslot->running) {
        Curl_trc_feat_infof(انکڑے, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      وإلا إذا(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(انکڑے, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      وإلا إذا(tslot->idle) {
        Curl_trc_feat_infof(انکڑے, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      وإلا {
        timediff_t remain_ms = tslot->work_timeout_ms ?
          (tslot->work_timeout_ms - elapsed_ms) : 0;
        Curl_trc_feat_infof(انکڑے, feat, "[TPOOL-%s] [%u]: busy %"
                            FMT_TIMEDIFF_T "ms, timeout in %" FMT_TIMEDIFF_T
                            "ms: %s",
                            tpool->name, tslot->id, elapsed_ms, remain_ms,
                            tslot->work_description);
      }
    }
    Curl_mutex_release(&tpool->جندرا);
  }
}
#جے_انت

#جے_انت /* USE_THREADS */
