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

#جيڪڏهن_وصف_ٿيل USE_ڌاڳا

#ضمن "llist.h"
#ضمن "curl_threads.h"
#ضمن "curlx/timeval.h"
#ضمن "curlx/strparse.h"
#ضمن "thrdpool.h"
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
#ضمن "curl_trc.h"
#ضمن "urldata.h"
#جيڪڏهن_پورو


بنية thrdslot {
  بنية Curl_llist_ڳنڍ ڳنڍ;
  بنية curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  بنية curltime starttime;
  ثابت حرف *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  ٻٽو_انگ(running);
  ٻٽو_انگ(idle);
};

بنية curl_thrdpool {
  ثابت حرف *name;
  uint64_t refcount;
  curl_mutex_t تالو;
  curl_cond_t await;
  بنية Curl_llist slots;
  بنية Curl_llist zombies;
  Curl_thrdpool_take_شي_cb *fn_take;
  Curl_thrdpool_process_شي_cb *fn_process;
  Curl_thrdpool_return_شي_cb *fn_return;
  فراغ *fn_user_معلومات;
  CURLcode fatal_err;
  uint32_t min_ڌاڳا;
  uint32_t max_ڌاڳا;
  uint32_t idle_وقت_ms;
  uint32_t ايندڙ_id;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  صحيح dbg_fail_starts; /* fail this many thread starts */
#جيڪڏهن_پورو
  ٻٽو_انگ(aborted);
  ٻٽو_انگ(detached);
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

ساكن فراغ thrdslot_پورو(بنية thrdslot *tslot)
{
  بنية curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_ڳنڍ_llist(&tslot->ڳنڍ) == &tpool->slots);
  Curl_ڳنڍ_remove(&tslot->ڳنڍ);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->ڳنڍ);
  Curl_cond_signal(&tpool->await);
}

ساكن CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_لاڳيتو(فراغ *arg)
{
  بنية thrdslot *tslot = arg;
  بنية curl_thrdpool *tpool = tslot->tpool;
  فراغ *شي;

  Curl_mutex_acquire(&tpool->تالو);
  DEBUGASSERT(Curl_ڳنڍ_llist(&tslot->ڳنڍ) == &tpool->slots);
  لكل(;;) {
    طالما(!tpool->aborted) {
      tslot->work_description = عدم;
      tslot->work_timeout_ms = 0;
      شي = tpool->fn_take(tpool->fn_user_معلومات, &tslot->work_description,
                            &tslot->work_timeout_ms);
      إذا(!شي)
        اكسر;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->تالو);

      tpool->fn_process(شي);

      Curl_mutex_acquire(&tpool->تالو);
      tslot->work_description = عدم;
      tpool->fn_return(شي, tpool->aborted ? عدم : tpool->fn_user_معلومات);
    }

    إذا(tpool->aborted ||
       (Curl_llist_ڳڻپ(&tpool->slots) > tpool->max_ڌاڳا))
      اذهب ٻاهريون;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    إذا((tpool->idle_وقت_ms > 0) &&
       (Curl_llist_ڳڻپ(&tpool->slots) > tpool->min_ڌاڳا)) {
      CURLcode نتيجو = Curl_cond_timedwait(&tslot->await, &tpool->تالو,
                                            tpool->idle_وقت_ms);
      إذا((نتيجو == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_ڳڻپ(&tpool->slots) > tpool->min_ڌاڳا)) {
        اذهب ٻاهريون;
      }
    }
    وإلا {
      Curl_cond_wait(&tslot->await, &tpool->تالو);
    }
  }

ٻاهريون:
  thrdslot_پورو(tslot);
  إذا(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->تالو);
  }
  أرجع 0;
}

ساكن CURLcode thrdslot_شروعات(بنية curl_thrdpool *tpool)
{
  بنية thrdslot *tslot;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;

  tslot = curlx_calloc(1, حجم(*tslot));
  إذا(!tslot)
    اذهب ٻاهريون;
  tslot->id = tpool->ايندڙ_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  إذا(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  وإلا
#جيڪڏهن_پورو
    tslot->thread = Curl_thread_create(thrdslot_لاڳيتو, tslot);
  إذا(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    نتيجو = CURLE_FAILED_INIT;
    اذهب ٻاهريون;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->ڳنڍ);
  tslot = عدم;
  نتيجو = CURLE_OK;

ٻاهريون:
  إذا(tslot)
    thrdslot_destroy(tslot);
  أرجع نتيجو;
}

ساكن فراغ thrdpool_wake_all(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_ڳنڍ *e;
  لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_ڳنڍ_ايندڙ(e)) {
    بنية thrdslot *tslot = Curl_ڳنڍ_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

ساكن فراغ thrdpool_join_zombies(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_ڳنڍ *e;

  لكل(e = Curl_llist_سر(&tpool->zombies); e;
      e = Curl_llist_سر(&tpool->zombies)) {
    بنية thrdslot *tslot = Curl_ڳنڍ_elem(e);

    Curl_ڳنڍ_remove(&tslot->ڳنڍ);
    إذا(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->تالو);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->تالو);
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
    Curl_mutex_release(&tpool->تالو);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->تالو);
  curlx_free(tpool);
  أرجع TRUE;
}

ساكن CURLcode thrdpool_signal(بنية curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  بنية Curl_llist_ڳنڍ *e, *n;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  لكل(e = Curl_llist_سر(&tpool->slots); e && nthreads; e = n) {
    بنية thrdslot *tslot = Curl_ڳنڍ_elem(e);
    n = Curl_ڳنڍ_ايندڙ(e);
    إذا(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    وإلا إذا(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  طالما(nthreads && !نتيجو &&
        Curl_llist_ڳڻپ(&tpool->slots) < tpool->max_ڌاڳا) {
    نتيجو = thrdslot_شروعات(tpool);
    إذا(نتيجو)
      اكسر;
    --nthreads;
  }

  أرجع نتيجو;
}

CURLcode Curl_thrdpool_سيٽ_props(بنية curl_thrdpool *tpool,
                                 uint32_t min_ڌاڳا,
                                 uint32_t max_ڌاڳا,
                                 uint32_t idle_وقت_ms)
{
  CURLcode نتيجو = CURLE_OK;
  size_t running;

  إذا(!max_ڌاڳا || (min_ڌاڳا > max_ڌاڳا))
    أرجع CURLE_BAD_FUNCTION_دليل;

  Curl_mutex_acquire(&tpool->تالو);
  tpool->min_ڌاڳا = min_ڌاڳا;
  tpool->max_ڌاڳا = max_ڌاڳا;
  tpool->idle_وقت_ms = idle_وقت_ms;
  running = Curl_llist_ڳڻپ(&tpool->slots);
  إذا(tpool->min_ڌاڳا > running) {
    نتيجو = thrdpool_signal(tpool, tpool->min_ڌاڳا - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->تالو);

  أرجع نتيجو;
}

CURLcode Curl_thrdpool_create(بنية curl_thrdpool **ptpool,
                              ثابت حرف *name,
                              uint32_t min_ڌاڳا,
                              uint32_t max_ڌاڳا,
                              uint32_t idle_وقت_ms,
                              Curl_thrdpool_take_شي_cb *fn_take,
                              Curl_thrdpool_process_شي_cb *fn_process,
                              Curl_thrdpool_return_شي_cb *fn_return,
                              فراغ *user_معلومات)
{
  بنية curl_thrdpool *tpool;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, حجم(*tpool));
  إذا(!tpool)
    اذهب ٻاهريون;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->تالو);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, عدم);
  Curl_llist_init(&tpool->zombies, عدم);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_معلومات = user_معلومات;

  /* a const string that remains */
  tpool->name = name;

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, وڌ_کان_وڌ_عدد))
        tpool->dbg_fail_starts = (صحيح)l;
    }
  }
#جيڪڏهن_پورو

  نتيجو = Curl_thrdpool_سيٽ_props(tpool, min_ڌاڳا, max_ڌاڳا,
                                   idle_وقت_ms);

ٻاهريون:
  إذا(نتيجو && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = عدم;
  }
  *ptpool = tpool;
  أرجع نتيجو;
}

فراغ Curl_thrdpool_destroy(بنية curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->تالو);

  tpool->aborted = TRUE;

  طالما(join && Curl_llist_ڳڻپ(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->تالو);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  إذا(Curl_llist_ڳڻپ(&tpool->slots)) {
    بنية Curl_llist_ڳنڍ *e;
    لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_ڳنڍ_ايندڙ(e)) {
      بنية thrdslot *tslot = Curl_ڳنڍ_elem(e);
      إذا(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  إذا(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->تالو);
  }
}

CURLcode Curl_thrdpool_signal(بنية curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode نتيجو;

  Curl_mutex_acquire(&tpool->تالو);
  نتيجو = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->تالو);
  أرجع نتيجو;
}

ساكن bool thrdpool_all_idle(بنية curl_thrdpool *tpool)
{
  بنية Curl_llist_ڳنڍ *e;
  لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_ڳنڍ_ايندڙ(e)) {
    بنية thrdslot *tslot = Curl_ڳنڍ_elem(e);
    إذا(!tslot->idle)
      أرجع FALSE;
  }
  أرجع TRUE;
}

CURLcode Curl_thrdpool_await_idle(بنية curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode نتيجو = CURLE_OK;
  بنية curltime پڇاڙي = { 0 };

  Curl_mutex_acquire(&tpool->تالو);
  DEBUGASSERT(!tpool->aborted);
  إذا(tpool->aborted) {
    نتيجو = CURLE_FAILED_INIT;
    اذهب ٻاهريون;
  }

  طالما(!thrdpool_all_idle(tpool)) {
    إذا(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      إذا(!پڇاڙي.tv_sec && !پڇاڙي.tv_usec) {
        پڇاڙي = curlx_now();
        پڇاڙي.tv_sec += (time_t)(timeout_ms / 1000);
        پڇاڙي.tv_usec += (صحيح)(timeout_ms % 1000) * 1000;
        إذا(پڇاڙي.tv_usec >= 1000000) {
          پڇاڙي.tv_sec++;
          پڇاڙي.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), پڇاڙي);
      إذا(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      وإلا
        r = Curl_cond_timedwait(&tpool->await, &tpool->تالو,
                                (uint32_t)remain_ms);
      إذا(r == CURLE_OPERATION_TIMEDOUT) {
        نتيجو = r;
        اكسر;
      }
    }
    وإلا {
      Curl_cond_wait(&tpool->await, &tpool->تالو);
    }
  }

ٻاهريون:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->تالو);
  أرجع نتيجو;
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
فراغ Curl_thrdpool_trace(بنية curl_thrdpool *tpool,
                         بنية Curl_easy *معلومات)
{
  بنية curl_trc_feat *feat = &Curl_trc_feat_ڌاڳا;
  إذا(Curl_trc_ft_is_verbose(معلومات, feat)) {
    بنية Curl_llist_ڳنڍ *e;
    بنية curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->تالو);
    إذا(!Curl_llist_ڳڻپ(&tpool->slots)) {
      Curl_trc_feat_infof(معلومات, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    لكل(e = Curl_llist_سر(&tpool->slots); e; e = Curl_ڳنڍ_ايندڙ(e)) {
      بنية thrdslot *tslot = Curl_ڳنڍ_elem(e);
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
    Curl_mutex_release(&tpool->تالو);
  }
}
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* USE_THREADS */
