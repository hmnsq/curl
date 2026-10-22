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
#समावेश "curl_setup.h"

#अगर_घोषित USE_THREADS

#समावेश "llist.h"
#समावेश "curl_threads.h"
#समावेश "curlx/timeval.h"
#समावेश "curlx/strparse.h"
#समावेश "thrdpool.h"
#अगर_घोषित CURLVERBOSE
#समावेश "curl_trc.h"
#समावेश "urldata.h"
#पूर्ण_अगर


काष्ठा thrdslot {
  काष्ठा Curl_llist_गाँठ गाँठ;
  काष्ठा curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  काष्ठा curltime starttime;
  स्थिर अक्षर *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  BIT(running);
  BIT(idle);
};

काष्ठा curl_thrdpool {
  स्थिर अक्षर *name;
  uint64_t refcount;
  curl_mutex_t lock;
  curl_cond_t await;
  काष्ठा Curl_llist slots;
  काष्ठा Curl_llist zombies;
  Curl_thrdpool_take_item_cb *fn_take;
  Curl_thrdpool_process_item_cb *fn_process;
  Curl_thrdpool_return_item_cb *fn_return;
  व्योम *fn_user_तथ्य;
  CURLcode fatal_err;
  uint32_t min_threads;
  uint32_t max_threads;
  uint32_t idle_time_ms;
  uint32_t अग्र_id;
#अगर_घोषित DEBUGBUILD
  पूर्णांक dbg_fail_starts; /* fail this many thread starts */
#पूर्ण_अगर
  BIT(aborted);
  BIT(detached);
};

अटल व्योम thrdpool_join_zombies(काष्ठा curl_thrdpool *tpool);
अटल bool thrdpool_unlink(काष्ठा curl_thrdpool *tpool, bool locked);

अटल व्योम thrdslot_destroy(काष्ठा thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

अटल व्योम thrdslot_done(काष्ठा thrdslot *tslot)
{
  काष्ठा curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_गाँठ_llist(&tslot->गाँठ) == &tpool->slots);
  Curl_गाँठ_remove(&tslot->गाँठ);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->गाँठ);
  Curl_cond_signal(&tpool->await);
}

अटल CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_run(व्योम *arg)
{
  काष्ठा thrdslot *tslot = arg;
  काष्ठा curl_thrdpool *tpool = tslot->tpool;
  व्योम *item;

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(Curl_गाँठ_llist(&tslot->गाँठ) == &tpool->slots);
  क्रम(;;) {
    जबतक(!tpool->aborted) {
      tslot->work_description = शून्य;
      tslot->work_timeout_ms = 0;
      item = tpool->fn_take(tpool->fn_user_तथ्य, &tslot->work_description,
                            &tslot->work_timeout_ms);
      अगर(!item)
        अवरोध;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->lock);

      tpool->fn_process(item);

      Curl_mutex_acquire(&tpool->lock);
      tslot->work_description = शून्य;
      tpool->fn_return(item, tpool->aborted ? शून्य : tpool->fn_user_तथ्य);
    }

    अगर(tpool->aborted ||
       (Curl_llist_गिनती(&tpool->slots) > tpool->max_threads))
      जाओ out;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    अगर((tpool->idle_time_ms > 0) &&
       (Curl_llist_गिनती(&tpool->slots) > tpool->min_threads)) {
      CURLcode परिणाम = Curl_cond_timedwait(&tslot->await, &tpool->lock,
                                            tpool->idle_time_ms);
      अगर((परिणाम == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_गिनती(&tpool->slots) > tpool->min_threads)) {
        जाओ out;
      }
    }
    अन्यथा {
      Curl_cond_wait(&tslot->await, &tpool->lock);
    }
  }

out:
  thrdslot_done(tslot);
  अगर(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
  वापस 0;
}

अटल CURLcode thrdslot_start(काष्ठा curl_thrdpool *tpool)
{
  काष्ठा thrdslot *tslot;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  tslot = curlx_calloc(1, माप(*tslot));
  अगर(!tslot)
    जाओ out;
  tslot->id = tpool->अग्र_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#अगर_घोषित DEBUGBUILD
  अगर(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  अन्यथा
#पूर्ण_अगर
    tslot->thread = Curl_thread_create(thrdslot_run, tslot);
  अगर(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    परिणाम = CURLE_FAILED_INIT;
    जाओ out;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->गाँठ);
  tslot = शून्य;
  परिणाम = CURLE_OK;

out:
  अगर(tslot)
    thrdslot_destroy(tslot);
  वापस परिणाम;
}

अटल व्योम thrdpool_wake_all(काष्ठा curl_thrdpool *tpool)
{
  काष्ठा Curl_llist_गाँठ *e;
  क्रम(e = Curl_llist_शीर्ष(&tpool->slots); e; e = Curl_गाँठ_अग्र(e)) {
    काष्ठा thrdslot *tslot = Curl_गाँठ_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

अटल व्योम thrdpool_join_zombies(काष्ठा curl_thrdpool *tpool)
{
  काष्ठा Curl_llist_गाँठ *e;

  क्रम(e = Curl_llist_शीर्ष(&tpool->zombies); e;
      e = Curl_llist_शीर्ष(&tpool->zombies)) {
    काष्ठा thrdslot *tslot = Curl_गाँठ_elem(e);

    Curl_गाँठ_remove(&tslot->गाँठ);
    अगर(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->lock);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->lock);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

अटल bool thrdpool_unlink(काष्ठा curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  अगर(tpool->refcount)
    tpool->refcount--;
  अगर(tpool->refcount)
    वापस FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  अगर(locked)
    Curl_mutex_release(&tpool->lock);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->lock);
  curlx_free(tpool);
  वापस TRUE;
}

अटल CURLcode thrdpool_signal(काष्ठा curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  काष्ठा Curl_llist_गाँठ *e, *n;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  क्रम(e = Curl_llist_शीर्ष(&tpool->slots); e && nthreads; e = n) {
    काष्ठा thrdslot *tslot = Curl_गाँठ_elem(e);
    n = Curl_गाँठ_अग्र(e);
    अगर(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    अन्यथा अगर(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  जबतक(nthreads && !परिणाम &&
        Curl_llist_गिनती(&tpool->slots) < tpool->max_threads) {
    परिणाम = thrdslot_start(tpool);
    अगर(परिणाम)
      अवरोध;
    --nthreads;
  }

  वापस परिणाम;
}

CURLcode Curl_thrdpool_set_props(काष्ठा curl_thrdpool *tpool,
                                 uint32_t min_threads,
                                 uint32_t max_threads,
                                 uint32_t idle_time_ms)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार running;

  अगर(!max_threads || (min_threads > max_threads))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mutex_acquire(&tpool->lock);
  tpool->min_threads = min_threads;
  tpool->max_threads = max_threads;
  tpool->idle_time_ms = idle_time_ms;
  running = Curl_llist_गिनती(&tpool->slots);
  अगर(tpool->min_threads > running) {
    परिणाम = thrdpool_signal(tpool, tpool->min_threads - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->lock);

  वापस परिणाम;
}

CURLcode Curl_thrdpool_create(काष्ठा curl_thrdpool **ptpool,
                              स्थिर अक्षर *name,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms,
                              Curl_thrdpool_take_item_cb *fn_take,
                              Curl_thrdpool_process_item_cb *fn_process,
                              Curl_thrdpool_return_item_cb *fn_return,
                              व्योम *user_तथ्य)
{
  काष्ठा curl_thrdpool *tpool;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, माप(*tpool));
  अगर(!tpool)
    जाओ out;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->lock);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, शून्य);
  Curl_llist_init(&tpool->zombies, शून्य);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_तथ्य = user_तथ्य;

  /* a const string that remains */
  tpool->name = name;

#अगर_घोषित DEBUGBUILD
  {
    स्थिर अक्षर *p = दो_पर्या("CURL_DBG_THRDPOOL_FAIL_STARTS");
    अगर(p) {
      curl_off_t l;
      अगर(!curlx_str_संख्या(&p, &l, पूर्णांक_उच्च))
        tpool->dbg_fail_starts = (पूर्णांक)l;
    }
  }
#पूर्ण_अगर

  परिणाम = Curl_thrdpool_set_props(tpool, min_threads, max_threads,
                                   idle_time_ms);

out:
  अगर(परिणाम && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = शून्य;
  }
  *ptpool = tpool;
  वापस परिणाम;
}

व्योम Curl_thrdpool_destroy(काष्ठा curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->lock);

  tpool->aborted = TRUE;

  जबतक(join && Curl_llist_गिनती(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->lock);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  अगर(Curl_llist_गिनती(&tpool->slots)) {
    काष्ठा Curl_llist_गाँठ *e;
    क्रम(e = Curl_llist_शीर्ष(&tpool->slots); e; e = Curl_गाँठ_अग्र(e)) {
      काष्ठा thrdslot *tslot = Curl_गाँठ_elem(e);
      अगर(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  अगर(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->lock);
  }
}

CURLcode Curl_thrdpool_signal(काष्ठा curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode परिणाम;

  Curl_mutex_acquire(&tpool->lock);
  परिणाम = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->lock);
  वापस परिणाम;
}

अटल bool thrdpool_all_idle(काष्ठा curl_thrdpool *tpool)
{
  काष्ठा Curl_llist_गाँठ *e;
  क्रम(e = Curl_llist_शीर्ष(&tpool->slots); e; e = Curl_गाँठ_अग्र(e)) {
    काष्ठा thrdslot *tslot = Curl_गाँठ_elem(e);
    अगर(!tslot->idle)
      वापस FALSE;
  }
  वापस TRUE;
}

CURLcode Curl_thrdpool_await_idle(काष्ठा curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा curltime end = { 0 };

  Curl_mutex_acquire(&tpool->lock);
  DEBUGASSERT(!tpool->aborted);
  अगर(tpool->aborted) {
    परिणाम = CURLE_FAILED_INIT;
    जाओ out;
  }

  जबतक(!thrdpool_all_idle(tpool)) {
    अगर(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      अगर(!end.tv_sec && !end.tv_usec) {
        end = curlx_now();
        end.tv_sec += (समय_प्रकार)(timeout_ms / 1000);
        end.tv_usec += (पूर्णांक)(timeout_ms % 1000) * 1000;
        अगर(end.tv_usec >= 1000000) {
          end.tv_sec++;
          end.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), end);
      अगर(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      अन्यथा
        r = Curl_cond_timedwait(&tpool->await, &tpool->lock,
                                (uint32_t)remain_ms);
      अगर(r == CURLE_OPERATION_TIMEDOUT) {
        परिणाम = r;
        अवरोध;
      }
    }
    अन्यथा {
      Curl_cond_wait(&tpool->await, &tpool->lock);
    }
  }

out:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->lock);
  वापस परिणाम;
}

#अगर_घोषित CURLVERBOSE
व्योम Curl_thrdpool_trace(काष्ठा curl_thrdpool *tpool,
                         काष्ठा Curl_easy *तथ्य)
{
  काष्ठा curl_trc_feat *feat = &Curl_trc_feat_threads;
  अगर(Curl_trc_ft_is_verbose(तथ्य, feat)) {
    काष्ठा Curl_llist_गाँठ *e;
    काष्ठा curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->lock);
    अगर(!Curl_llist_गिनती(&tpool->slots)) {
      Curl_trc_feat_infof(तथ्य, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    क्रम(e = Curl_llist_शीर्ष(&tpool->slots); e; e = Curl_गाँठ_अग्र(e)) {
      काष्ठा thrdslot *tslot = Curl_गाँठ_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      अगर(!tslot->running) {
        Curl_trc_feat_infof(तथ्य, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      अन्यथा अगर(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(तथ्य, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      अन्यथा अगर(tslot->idle) {
        Curl_trc_feat_infof(तथ्य, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      अन्यथा {
        timediff_t remain_ms = tslot->work_timeout_ms ?
          (tslot->work_timeout_ms - elapsed_ms) : 0;
        Curl_trc_feat_infof(तथ्य, feat, "[TPOOL-%s] [%u]: busy %"
                            FMT_TIMEDIFF_T "ms, timeout in %" FMT_TIMEDIFF_T
                            "ms: %s",
                            tpool->name, tslot->id, elapsed_ms, remain_ms,
                            tslot->work_description);
      }
    }
    Curl_mutex_release(&tpool->lock);
  }
}
#पूर्ण_अगर

#पूर्ण_अगर /* USE_THREADS */
