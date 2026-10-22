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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_دھاگے

#شامل "llist.h"
#شامل "curl_threads.h"
#شامل "curlx/timeval.h"
#شامل "curlx/strparse.h"
#شامل "thrdpool.h"
#اگر_تعریف_شدہ CURLVERBOSE
#شامل "curl_trc.h"
#شامل "urldata.h"
#اگر_ختم


ساخت thrdslot {
  ساخت Curl_llist_گرہ گرہ;
  ساخت curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  ساخت curltime starttime;
  ثابت حرف *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  ثنائی_ہندسہ(running);
  ثنائی_ہندسہ(idle);
};

ساخت curl_thrdpool {
  ثابت حرف *name;
  uint64_t refcount;
  curl_mutex_t تالا;
  curl_cond_t await;
  ساخت Curl_llist slots;
  ساخت Curl_llist zombies;
  Curl_thrdpool_take_شے_cb *fn_take;
  Curl_thrdpool_process_شے_cb *fn_process;
  Curl_thrdpool_return_شے_cb *fn_return;
  خالی *fn_user_ڈیٹا;
  CURLcode fatal_err;
  uint32_t min_دھاگے;
  uint32_t max_دھاگے;
  uint32_t idle_وقت_ms;
  uint32_t اگلا_id;
#اگر_تعریف_شدہ DEBUGBUILD
  صحیح dbg_fail_starts; /* fail this many thread starts */
#اگر_ختم
  ثنائی_ہندسہ(aborted);
  ثنائی_ہندسہ(detached);
};

ساکن خالی thrdpool_join_zombies(ساخت curl_thrdpool *tpool);
ساکن bool thrdpool_unlink(ساخت curl_thrdpool *tpool, bool locked);

ساکن خالی thrdslot_destroy(ساخت thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

ساکن خالی thrdslot_مکمل(ساخت thrdslot *tslot)
{
  ساخت curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_گرہ_llist(&tslot->گرہ) == &tpool->slots);
  Curl_گرہ_remove(&tslot->گرہ);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->گرہ);
  Curl_cond_signal(&tpool->await);
}

ساکن CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_تسلسل(خالی *arg)
{
  ساخت thrdslot *tslot = arg;
  ساخت curl_thrdpool *tpool = tslot->tpool;
  خالی *شے;

  Curl_mutex_acquire(&tpool->تالا);
  DEBUGASSERT(Curl_گرہ_llist(&tslot->گرہ) == &tpool->slots);
  برائے(;;) {
    جب(!tpool->aborted) {
      tslot->work_description = تہی;
      tslot->work_timeout_ms = 0;
      شے = tpool->fn_take(tpool->fn_user_ڈیٹا, &tslot->work_description,
                            &tslot->work_timeout_ms);
      اگر(!شے)
        توڑو;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->تالا);

      tpool->fn_process(شے);

      Curl_mutex_acquire(&tpool->تالا);
      tslot->work_description = تہی;
      tpool->fn_return(شے, tpool->aborted ? تہی : tpool->fn_user_ڈیٹا);
    }

    اگر(tpool->aborted ||
       (Curl_llist_گنتی(&tpool->slots) > tpool->max_دھاگے))
      جاؤ برآمد;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    اگر((tpool->idle_وقت_ms > 0) &&
       (Curl_llist_گنتی(&tpool->slots) > tpool->min_دھاگے)) {
      CURLcode نتیجہ = Curl_cond_timedwait(&tslot->await, &tpool->تالا,
                                            tpool->idle_وقت_ms);
      اگر((نتیجہ == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_گنتی(&tpool->slots) > tpool->min_دھاگے)) {
        جاؤ برآمد;
      }
    }
    ورنہ {
      Curl_cond_wait(&tslot->await, &tpool->تالا);
    }
  }

برآمد:
  thrdslot_مکمل(tslot);
  اگر(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->تالا);
  }
  واپس 0;
}

ساکن CURLcode thrdslot_آغاز(ساخت curl_thrdpool *tpool)
{
  ساخت thrdslot *tslot;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  tslot = curlx_calloc(1, حجم(*tslot));
  اگر(!tslot)
    جاؤ برآمد;
  tslot->id = tpool->اگلا_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#اگر_تعریف_شدہ DEBUGBUILD
  اگر(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  ورنہ
#اگر_ختم
    tslot->thread = Curl_thread_create(thrdslot_تسلسل, tslot);
  اگر(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    نتیجہ = CURLE_FAILED_INIT;
    جاؤ برآمد;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->گرہ);
  tslot = تہی;
  نتیجہ = CURLE_OK;

برآمد:
  اگر(tslot)
    thrdslot_destroy(tslot);
  واپس نتیجہ;
}

ساکن خالی thrdpool_wake_all(ساخت curl_thrdpool *tpool)
{
  ساخت Curl_llist_گرہ *e;
  برائے(e = Curl_llist_سر(&tpool->slots); e; e = Curl_گرہ_اگلا(e)) {
    ساخت thrdslot *tslot = Curl_گرہ_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

ساکن خالی thrdpool_join_zombies(ساخت curl_thrdpool *tpool)
{
  ساخت Curl_llist_گرہ *e;

  برائے(e = Curl_llist_سر(&tpool->zombies); e;
      e = Curl_llist_سر(&tpool->zombies)) {
    ساخت thrdslot *tslot = Curl_گرہ_elem(e);

    Curl_گرہ_remove(&tslot->گرہ);
    اگر(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->تالا);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->تالا);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

ساکن bool thrdpool_unlink(ساخت curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  اگر(tpool->refcount)
    tpool->refcount--;
  اگر(tpool->refcount)
    واپس FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  اگر(locked)
    Curl_mutex_release(&tpool->تالا);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->تالا);
  curlx_free(tpool);
  واپس TRUE;
}

ساکن CURLcode thrdpool_signal(ساخت curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  ساخت Curl_llist_گرہ *e, *n;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  برائے(e = Curl_llist_سر(&tpool->slots); e && nthreads; e = n) {
    ساخت thrdslot *tslot = Curl_گرہ_elem(e);
    n = Curl_گرہ_اگلا(e);
    اگر(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    ورنہ اگر(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  جب(nthreads && !نتیجہ &&
        Curl_llist_گنتی(&tpool->slots) < tpool->max_دھاگے) {
    نتیجہ = thrdslot_آغاز(tpool);
    اگر(نتیجہ)
      توڑو;
    --nthreads;
  }

  واپس نتیجہ;
}

CURLcode Curl_thrdpool_سیٹ_props(ساخت curl_thrdpool *tpool,
                                 uint32_t min_دھاگے,
                                 uint32_t max_دھاگے,
                                 uint32_t idle_وقت_ms)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t running;

  اگر(!max_دھاگے || (min_دھاگے > max_دھاگے))
    واپس CURLE_BAD_FUNCTION_دلیل;

  Curl_mutex_acquire(&tpool->تالا);
  tpool->min_دھاگے = min_دھاگے;
  tpool->max_دھاگے = max_دھاگے;
  tpool->idle_وقت_ms = idle_وقت_ms;
  running = Curl_llist_گنتی(&tpool->slots);
  اگر(tpool->min_دھاگے > running) {
    نتیجہ = thrdpool_signal(tpool, tpool->min_دھاگے - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->تالا);

  واپس نتیجہ;
}

CURLcode Curl_thrdpool_create(ساخت curl_thrdpool **ptpool,
                              ثابت حرف *name,
                              uint32_t min_دھاگے,
                              uint32_t max_دھاگے,
                              uint32_t idle_وقت_ms,
                              Curl_thrdpool_take_شے_cb *fn_take,
                              Curl_thrdpool_process_شے_cb *fn_process,
                              Curl_thrdpool_return_شے_cb *fn_return,
                              خالی *user_ڈیٹا)
{
  ساخت curl_thrdpool *tpool;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, حجم(*tpool));
  اگر(!tpool)
    جاؤ برآمد;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->تالا);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, تہی);
  Curl_llist_init(&tpool->zombies, تہی);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_ڈیٹا = user_ڈیٹا;

  /* a const string that remains */
  tpool->name = name;

#اگر_تعریف_شدہ DEBUGBUILD
  {
    ثابت حرف *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_نمبر(&p, &l, زیادہ_سے_زیادہ_عدد))
        tpool->dbg_fail_starts = (صحیح)l;
    }
  }
#اگر_ختم

  نتیجہ = Curl_thrdpool_سیٹ_props(tpool, min_دھاگے, max_دھاگے,
                                   idle_وقت_ms);

برآمد:
  اگر(نتیجہ && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = تہی;
  }
  *ptpool = tpool;
  واپس نتیجہ;
}

خالی Curl_thrdpool_destroy(ساخت curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->تالا);

  tpool->aborted = TRUE;

  جب(join && Curl_llist_گنتی(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->تالا);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  اگر(Curl_llist_گنتی(&tpool->slots)) {
    ساخت Curl_llist_گرہ *e;
    برائے(e = Curl_llist_سر(&tpool->slots); e; e = Curl_گرہ_اگلا(e)) {
      ساخت thrdslot *tslot = Curl_گرہ_elem(e);
      اگر(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  اگر(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->تالا);
  }
}

CURLcode Curl_thrdpool_signal(ساخت curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode نتیجہ;

  Curl_mutex_acquire(&tpool->تالا);
  نتیجہ = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->تالا);
  واپس نتیجہ;
}

ساکن bool thrdpool_all_idle(ساخت curl_thrdpool *tpool)
{
  ساخت Curl_llist_گرہ *e;
  برائے(e = Curl_llist_سر(&tpool->slots); e; e = Curl_گرہ_اگلا(e)) {
    ساخت thrdslot *tslot = Curl_گرہ_elem(e);
    اگر(!tslot->idle)
      واپس FALSE;
  }
  واپس TRUE;
}

CURLcode Curl_thrdpool_await_idle(ساخت curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت curltime انتہا = { 0 };

  Curl_mutex_acquire(&tpool->تالا);
  DEBUGASSERT(!tpool->aborted);
  اگر(tpool->aborted) {
    نتیجہ = CURLE_FAILED_INIT;
    جاؤ برآمد;
  }

  جب(!thrdpool_all_idle(tpool)) {
    اگر(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      اگر(!انتہا.tv_sec && !انتہا.tv_usec) {
        انتہا = curlx_now();
        انتہا.tv_sec += (time_t)(timeout_ms / 1000);
        انتہا.tv_usec += (صحیح)(timeout_ms % 1000) * 1000;
        اگر(انتہا.tv_usec >= 1000000) {
          انتہا.tv_sec++;
          انتہا.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), انتہا);
      اگر(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      ورنہ
        r = Curl_cond_timedwait(&tpool->await, &tpool->تالا,
                                (uint32_t)remain_ms);
      اگر(r == CURLE_OPERATION_TIMEDOUT) {
        نتیجہ = r;
        توڑو;
      }
    }
    ورنہ {
      Curl_cond_wait(&tpool->await, &tpool->تالا);
    }
  }

برآمد:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->تالا);
  واپس نتیجہ;
}

#اگر_تعریف_شدہ CURLVERBOSE
خالی Curl_thrdpool_trace(ساخت curl_thrdpool *tpool,
                         ساخت Curl_easy *ڈیٹا)
{
  ساخت curl_trc_feat *feat = &Curl_trc_feat_دھاگے;
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, feat)) {
    ساخت Curl_llist_گرہ *e;
    ساخت curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->تالا);
    اگر(!Curl_llist_گنتی(&tpool->slots)) {
      Curl_trc_feat_infof(ڈیٹا, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    برائے(e = Curl_llist_سر(&tpool->slots); e; e = Curl_گرہ_اگلا(e)) {
      ساخت thrdslot *tslot = Curl_گرہ_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      اگر(!tslot->running) {
        Curl_trc_feat_infof(ڈیٹا, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      ورنہ اگر(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(ڈیٹا, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      ورنہ اگر(tslot->idle) {
        Curl_trc_feat_infof(ڈیٹا, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      ورنہ {
        timediff_t remain_ms = tslot->work_timeout_ms ?
          (tslot->work_timeout_ms - elapsed_ms) : 0;
        Curl_trc_feat_infof(ڈیٹا, feat, "[TPOOL-%s] [%u]: busy %"
                            FMT_TIMEDIFF_T "ms, timeout in %" FMT_TIMEDIFF_T
                            "ms: %s",
                            tpool->name, tslot->id, elapsed_ms, remain_ms,
                            tslot->work_description);
      }
    }
    Curl_mutex_release(&tpool->تالا);
  }
}
#اگر_ختم

#اگر_ختم /* USE_THREADS */
