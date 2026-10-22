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

#که_تعریف_شوی USE_تارونه

#ضمن "llist.h"
#ضمن "curl_threads.h"
#ضمن "curlx/timeval.h"
#ضمن "curlx/strparse.h"
#ضمن "thrdpool.h"
#که_تعریف_شوی CURLVERBOSE
#ضمن "curl_trc.h"
#ضمن "urldata.h"
#د_که_پای


بنية thrdslot {
  بنية Curl_llist_غوټه غوټه;
  بنية curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  بنية curltime starttime;
  ثابت حرف *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  بټ(running);
  بټ(idle);
};

بنية curl_thrdpool {
  ثابت حرف *name;
  uint64_t refcount;
  curl_mutex_t قلف;
  curl_cond_t await;
  بنية Curl_llist slots;
  بنية Curl_llist zombies;
  Curl_thrdpool_take_توکی_cb *fn_take;
  Curl_thrdpool_process_توکی_cb *fn_process;
  Curl_thrdpool_return_توکی_cb *fn_return;
  فراغ *fn_user_معلومات;
  CURLcode fatal_err;
  uint32_t min_تارونه;
  uint32_t max_تارونه;
  uint32_t idle_وخت_ms;
  uint32_t بل_id;
#که_تعریف_شوی DEBUGBUILD
  صحيح dbg_fail_starts; /* fail this many thread starts */
#د_که_پای
  بټ(aborted);
  بټ(detached);
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

ساكن فراغ thrdslot_بشپړ(بنية thrdslot *tslot)
{
  بنية curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_غوټه_llist(&tslot->غوټه) == &tpool->slots);
  Curl_غوټه_remove(&tslot->غوټه);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->غوټه);
  Curl_cond_signal(&tpool->await);
}

ساكن CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_پرلپسې(فراغ *arg)
{
  بنية thrdslot *tslot = arg;
  بنية curl_thrdpool *tpool = tslot->tpool;
  فراغ *توکی;

  Curl_mutex_acquire(&tpool->قلف);
  DEBUGASSERT(Curl_غوټه_llist(&tslot->غوټه) == &tpool->slots);
  لكل(;;) {
    طالما(!tpool->aborted) {
      tslot->work_description = عدم;
      tslot->work_timeout_ms = 0;
      توکی = tpool->fn_take(tpool->fn_user_معلومات, &tslot->work_description,
                            &tslot->work_timeout_ms);
      إذا(!توکی)
        اكسر;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->قلف);

      tpool->fn_process(توکی);

      Curl_mutex_acquire(&tpool->قلف);
      tslot->work_description = عدم;
      tpool->fn_return(توکی, tpool->aborted ? عدم : tpool->fn_user_معلومات);
    }

    إذا(tpool->aborted ||
       (Curl_llist_شمېر(&tpool->slots) > tpool->max_تارونه))
      اذهب بهر;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    إذا((tpool->idle_وخت_ms > 0) &&
       (Curl_llist_شمېر(&tpool->slots) > tpool->min_تارونه)) {
      CURLcode پایله = Curl_cond_timedwait(&tslot->await, &tpool->قلف,
                                            tpool->idle_وخت_ms);
      إذا((پایله == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_شمېر(&tpool->slots) > tpool->min_تارونه)) {
        اذهب بهر;
      }
    }
    وإلا {
      Curl_cond_wait(&tslot->await, &tpool->قلف);
    }
  }

بهر:
  thrdslot_بشپړ(tslot);
  إذا(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->قلف);
  }
  أرجع 0;
}

ساكن CURLcode thrdslot_پیل(بنية curl_thrdpool *tpool)
{
  بنية thrdslot *tslot;
  CURLcode پایله = CURLE_بهر_د_حافظه;

  tslot = curlx_calloc(1, حجم(*tslot));
  إذا(!tslot)
    اذهب بهر;
  tslot->id = tpool->بل_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#که_تعریف_شوی DEBUGBUILD
  إذا(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  وإلا
#د_که_پای
    tslot->thread = Curl_thread_create(thrdslot_پرلپسې, tslot);
  إذا(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    پایله = CURLE_FAILED_INIT;
    اذهب بهر;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->غوټه);
  tslot = عدم;
  پایله = CURLE_OK;

بهر:
  إذا(tslot)
    thrdslot_destroy(tslot);
  أرجع پایله;
}

ساكن فراغ thrdpool_wake_all(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_غوټه *e;
  لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_غوټه_بل(e)) {
    بنية thrdslot *tslot = Curl_غوټه_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

ساكن فراغ thrdpool_join_zombies(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_غوټه *e;

  لكل(e = Curl_llist_سر(&tpool->zombies); e;
      e = Curl_llist_سر(&tpool->zombies)) {
    بنية thrdslot *tslot = Curl_غوټه_elem(e);

    Curl_غوټه_remove(&tslot->غوټه);
    إذا(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->قلف);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->قلف);
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
    Curl_mutex_release(&tpool->قلف);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->قلف);
  curlx_free(tpool);
  أرجع TRUE;
}

ساكن CURLcode thrdpool_signal(بنية curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  بنية Curl_llist_غوټه *e, *n;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  لكل(e = Curl_llist_سر(&tpool->slots); e && nthreads; e = n) {
    بنية thrdslot *tslot = Curl_غوټه_elem(e);
    n = Curl_غوټه_بل(e);
    إذا(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    وإلا إذا(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  طالما(nthreads && !پایله &&
        Curl_llist_شمېر(&tpool->slots) < tpool->max_تارونه) {
    پایله = thrdslot_پیل(tpool);
    إذا(پایله)
      اكسر;
    --nthreads;
  }

  أرجع پایله;
}

CURLcode Curl_thrdpool_سېټ_props(بنية curl_thrdpool *tpool,
                                 uint32_t min_تارونه,
                                 uint32_t max_تارونه,
                                 uint32_t idle_وخت_ms)
{
  CURLcode پایله = CURLE_OK;
  size_t running;

  إذا(!max_تارونه || (min_تارونه > max_تارونه))
    أرجع CURLE_BAD_FUNCTION_دلیل;

  Curl_mutex_acquire(&tpool->قلف);
  tpool->min_تارونه = min_تارونه;
  tpool->max_تارونه = max_تارونه;
  tpool->idle_وخت_ms = idle_وخت_ms;
  running = Curl_llist_شمېر(&tpool->slots);
  إذا(tpool->min_تارونه > running) {
    پایله = thrdpool_signal(tpool, tpool->min_تارونه - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->قلف);

  أرجع پایله;
}

CURLcode Curl_thrdpool_create(بنية curl_thrdpool **ptpool,
                              ثابت حرف *name,
                              uint32_t min_تارونه,
                              uint32_t max_تارونه,
                              uint32_t idle_وخت_ms,
                              Curl_thrdpool_take_توکی_cb *fn_take,
                              Curl_thrdpool_process_توکی_cb *fn_process,
                              Curl_thrdpool_return_توکی_cb *fn_return,
                              فراغ *user_معلومات)
{
  بنية curl_thrdpool *tpool;
  CURLcode پایله = CURLE_بهر_د_حافظه;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, حجم(*tpool));
  إذا(!tpool)
    اذهب بهر;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->قلف);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, عدم);
  Curl_llist_init(&tpool->zombies, عدم);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_معلومات = user_معلومات;

  /* a const string that remains */
  tpool->name = name;

#که_تعریف_شوی DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_شمېره(&p, &l, لوی_عدد))
        tpool->dbg_fail_starts = (صحيح)l;
    }
  }
#د_که_پای

  پایله = Curl_thrdpool_سېټ_props(tpool, min_تارونه, max_تارونه,
                                   idle_وخت_ms);

بهر:
  إذا(پایله && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = عدم;
  }
  *ptpool = tpool;
  أرجع پایله;
}

فراغ Curl_thrdpool_destroy(بنية curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->قلف);

  tpool->aborted = TRUE;

  طالما(join && Curl_llist_شمېر(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->قلف);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  إذا(Curl_llist_شمېر(&tpool->slots)) {
    بنية Curl_llist_غوټه *e;
    لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_غوټه_بل(e)) {
      بنية thrdslot *tslot = Curl_غوټه_elem(e);
      إذا(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  إذا(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->قلف);
  }
}

CURLcode Curl_thrdpool_signal(بنية curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode پایله;

  Curl_mutex_acquire(&tpool->قلف);
  پایله = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->قلف);
  أرجع پایله;
}

ساكن bool thrdpool_all_idle(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_غوټه *e;
  لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_غوټه_بل(e)) {
    بنية thrdslot *tslot = Curl_غوټه_elem(e);
    إذا(!tslot->idle)
      أرجع FALSE;
  }
  أرجع TRUE;
}

CURLcode Curl_thrdpool_await_idle(بنية curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode پایله = CURLE_OK;
  بنية curltime پای_ټکی = { 0 };

  Curl_mutex_acquire(&tpool->قلف);
  DEBUGASSERT(!tpool->aborted);
  إذا(tpool->aborted) {
    پایله = CURLE_FAILED_INIT;
    اذهب بهر;
  }

  طالما(!thrdpool_all_idle(tpool)) {
    إذا(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      إذا(!پای_ټکی.tv_sec && !پای_ټکی.tv_usec) {
        پای_ټکی = curlx_now();
        پای_ټکی.tv_sec += (time_t)(timeout_ms / 1000);
        پای_ټکی.tv_usec += (صحيح)(timeout_ms % 1000) * 1000;
        إذا(پای_ټکی.tv_usec >= 1000000) {
          پای_ټکی.tv_sec++;
          پای_ټکی.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), پای_ټکی);
      إذا(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      وإلا
        r = Curl_cond_timedwait(&tpool->await, &tpool->قلف,
                                (uint32_t)remain_ms);
      إذا(r == CURLE_OPERATION_TIMEDOUT) {
        پایله = r;
        اكسر;
      }
    }
    وإلا {
      Curl_cond_wait(&tpool->await, &tpool->قلف);
    }
  }

بهر:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->قلف);
  أرجع پایله;
}

#که_تعریف_شوی CURLVERBOSE
فراغ Curl_thrdpool_trace(بنية curl_thrdpool *tpool,
                         بنية Curl_easy *معلومات)
{
  بنية curl_trc_feat *feat = &Curl_trc_feat_تارونه;
  إذا(Curl_trc_ft_is_verbose(معلومات, feat)) {
    بنية Curl_llist_غوټه *e;
    بنية curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->قلف);
    إذا(!Curl_llist_شمېر(&tpool->slots)) {
      Curl_trc_feat_infof(معلومات, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_غوټه_بل(e)) {
      بنية thrdslot *tslot = Curl_غوټه_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      إذا(!tslot->running) {
        Curl_trc_feat_infof(معلومات, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      وإلا إذا(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(معلومات, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      وإلا إذا(tslot->idle) {
        Curl_trc_feat_infof(معلومات, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      وإلا {
        timediff_t remain_ms = tslot->work_timeout_ms ?
          (tslot->work_timeout_ms - elapsed_ms) : 0;
        Curl_trc_feat_infof(معلومات, feat, "[TPOOL-%s] [%u]: busy %"
                            FMT_TIMEDIFF_T "ms, timeout in %" FMT_TIMEDIFF_T
                            "ms: %s",
                            tpool->name, tslot->id, elapsed_ms, remain_ms,
                            tslot->work_description);
      }
    }
    Curl_mutex_release(&tpool->قلف);
  }
}
#د_که_پای

#د_که_پای /* USE_THREADS */
