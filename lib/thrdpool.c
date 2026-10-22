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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_तन्तवः

#अन्तर्भाव "llist.h"
#अन्तर्भाव "curl_threads.h"
#अन्तर्भाव "curlx/timeval.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "thrdpool.h"
#यदि_परिभाषितम् CURLVERBOSE
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "urldata.h"
#यदि_अन्तः


संरचना thrdslot {
  संरचना Curl_llist_ग्रन्थिः ग्रन्थिः;
  संरचना curl_thrdpool *tpool;
  curl_thread_t thread;
  curl_cond_t await;
  संरचना curltime starttime;
  ध्रुव वर्ण *work_description;
  timediff_t work_timeout_ms;
  uint32_t id;
  BIT(running);
  BIT(idle);
};

संरचना curl_thrdpool {
  ध्रुव वर्ण *name;
  uint64_t refcount;
  curl_mutex_t तालकम्;
  curl_cond_t await;
  संरचना Curl_llist slots;
  संरचना Curl_llist zombies;
  Curl_thrdpool_take_item_cb *fn_take;
  Curl_thrdpool_process_item_cb *fn_process;
  Curl_thrdpool_return_item_cb *fn_return;
  शून्य *fn_user_दत्त;
  CURLcode fatal_err;
  uint32_t min_तन्तवः;
  uint32_t max_तन्तवः;
  uint32_t idle_time_ms;
  uint32_t अग्र_id;
#यदि_परिभाषितम् DEBUGBUILD
  पूर्णाङ्क dbg_fail_starts; /* fail this many thread starts */
#यदि_अन्तः
  BIT(aborted);
  BIT(detached);
};

स्थिर शून्य thrdpool_join_zombies(संरचना curl_thrdpool *tpool);
स्थिर bool thrdpool_unlink(संरचना curl_thrdpool *tpool, bool locked);

स्थिर शून्य thrdslot_destroy(संरचना thrdslot *tslot)
{
  DEBUGASSERT(tslot->thread == curl_thread_t_null);
  DEBUGASSERT(!tslot->running);
  Curl_cond_destroy(&tslot->await);
  curlx_free(tslot);
}

स्थिर शून्य thrdslot_समाप्तम्(संरचना thrdslot *tslot)
{
  संरचना curl_thrdpool *tpool = tslot->tpool;

  DEBUGASSERT(Curl_ग्रन्थिः_llist(&tslot->ग्रन्थिः) == &tpool->slots);
  Curl_ग्रन्थिः_remove(&tslot->ग्रन्थिः);
  tslot->running = FALSE;
  Curl_llist_append(&tpool->zombies, tslot, &tslot->ग्रन्थिः);
  Curl_cond_signal(&tpool->await);
}

स्थिर CURL_THREAD_RETURN_T CURL_STDCALL thrdslot_सततम्(शून्य *arg)
{
  संरचना thrdslot *tslot = arg;
  संरचना curl_thrdpool *tpool = tslot->tpool;
  शून्य *item;

  Curl_mutex_acquire(&tpool->तालकम्);
  DEBUGASSERT(Curl_ग्रन्थिः_llist(&tslot->ग्रन्थिः) == &tpool->slots);
  प्रति(;;) {
    यावत्(!tpool->aborted) {
      tslot->work_description = रिक्त;
      tslot->work_timeout_ms = 0;
      item = tpool->fn_take(tpool->fn_user_दत्त, &tslot->work_description,
                            &tslot->work_timeout_ms);
      यदि(!item)
        भङ्ग;
      tslot->starttime = curlx_now();
      tslot->idle = FALSE;
      Curl_mutex_release(&tpool->तालकम्);

      tpool->fn_process(item);

      Curl_mutex_acquire(&tpool->तालकम्);
      tslot->work_description = रिक्त;
      tpool->fn_return(item, tpool->aborted ? रिक्त : tpool->fn_user_दत्त);
    }

    यदि(tpool->aborted ||
       (Curl_llist_गणनम्(&tpool->slots) > tpool->max_तन्तवः))
      गच्छ बहिर्गमनम्;

    tslot->idle = TRUE;
    tslot->starttime = curlx_now();
    thrdpool_join_zombies(tpool);
    Curl_cond_signal(&tpool->await);
    /* Only wait with idle timeout when we are above the minimum
     * number of threads. Otherwise short idle timeouts will keep
     * on activating threads that have no means to shut down. */
    यदि((tpool->idle_time_ms > 0) &&
       (Curl_llist_गणनम्(&tpool->slots) > tpool->min_तन्तवः)) {
      CURLcode परिणाम = Curl_cond_timedwait(&tslot->await, &tpool->तालकम्,
                                            tpool->idle_time_ms);
      यदि((परिणाम == CURLE_OPERATION_TIMEDOUT) &&
         (Curl_llist_गणनम्(&tpool->slots) > tpool->min_तन्तवः)) {
        गच्छ बहिर्गमनम्;
      }
    }
    अन्यथा {
      Curl_cond_wait(&tslot->await, &tpool->तालकम्);
    }
  }

बहिर्गमनम्:
  thrdslot_समाप्तम्(tslot);
  यदि(!thrdpool_unlink(tslot->tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->तालकम्);
  }
  प्रत्यागम 0;
}

स्थिर CURLcode thrdslot_start(संरचना curl_thrdpool *tpool)
{
  संरचना thrdslot *tslot;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  tslot = curlx_calloc(1, परिमाण(*tslot));
  यदि(!tslot)
    गच्छ बहिर्गमनम्;
  tslot->id = tpool->अग्र_id++;
  tslot->tpool = tpool;
  tslot->thread = curl_thread_t_null;
  Curl_cond_init(&tslot->await);

  tpool->refcount++;
  tslot->running = TRUE;
#यदि_परिभाषितम् DEBUGBUILD
  यदि(tpool->dbg_fail_starts > 0) {
    --tpool->dbg_fail_starts;
    tslot->thread = curl_thread_t_null;
  }
  अन्यथा
#यदि_अन्तः
    tslot->thread = Curl_thread_create(thrdslot_सततम्, tslot);
  यदि(tslot->thread == curl_thread_t_null) { /* never started */
    tslot->running = FALSE;
    thrdpool_unlink(tpool, TRUE);
    परिणाम = CURLE_FAILED_INIT;
    गच्छ बहिर्गमनम्;
  }

  Curl_llist_append(&tpool->slots, tslot, &tslot->ग्रन्थिः);
  tslot = रिक्त;
  परिणाम = CURLE_OK;

बहिर्गमनम्:
  यदि(tslot)
    thrdslot_destroy(tslot);
  प्रत्यागम परिणाम;
}

स्थिर शून्य thrdpool_wake_all(संरचना curl_thrdpool *tpool)
{
  संरचना Curl_llist_ग्रन्थिः *e;
  प्रति(e = Curl_llist_शीर्ष(&tpool->slots); e; e = Curl_ग्रन्थिः_अग्र(e)) {
    संरचना thrdslot *tslot = Curl_ग्रन्थिः_elem(e);
    Curl_cond_signal(&tslot->await);
  }
}

स्थिर शून्य thrdpool_join_zombies(संरचना curl_thrdpool *tpool)
{
  संरचना Curl_llist_ग्रन्थिः *e;

  प्रति(e = Curl_llist_शीर्ष(&tpool->zombies); e;
      e = Curl_llist_शीर्ष(&tpool->zombies)) {
    संरचना thrdslot *tslot = Curl_ग्रन्थिः_elem(e);

    Curl_ग्रन्थिः_remove(&tslot->ग्रन्थिः);
    यदि(tslot->thread != curl_thread_t_null) {
      Curl_mutex_release(&tpool->तालकम्);
      Curl_thread_join(&tslot->thread);
      Curl_mutex_acquire(&tpool->तालकम्);
      tslot->thread = curl_thread_t_null;
    }
    thrdslot_destroy(tslot);
  }
}

स्थिर bool thrdpool_unlink(संरचना curl_thrdpool *tpool, bool locked)
{
  DEBUGASSERT(tpool->refcount);
  यदि(tpool->refcount)
    tpool->refcount--;
  यदि(tpool->refcount)
    प्रत्यागम FALSE;

  /* no more references, free */
  DEBUGASSERT(tpool->aborted);
  thrdpool_join_zombies(tpool);
  यदि(locked)
    Curl_mutex_release(&tpool->तालकम्);
  Curl_cond_destroy(&tpool->await);
  Curl_mutex_destroy(&tpool->तालकम्);
  curlx_free(tpool);
  प्रत्यागम TRUE;
}

स्थिर CURLcode thrdpool_signal(संरचना curl_thrdpool *tpool,
                                uint32_t nthreads)
{
  संरचना Curl_llist_ग्रन्थिः *e, *n;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(!tpool->aborted);
  thrdpool_join_zombies(tpool);

  प्रति(e = Curl_llist_शीर्ष(&tpool->slots); e && nthreads; e = n) {
    संरचना thrdslot *tslot = Curl_ग्रन्थिः_elem(e);
    n = Curl_ग्रन्थिः_अग्र(e);
    यदि(tslot->idle) {
      Curl_cond_signal(&tslot->await);
      --nthreads;
    }
    अन्यथा यदि(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
      /* starting thread, queries for work soon. */
      --nthreads;
    }
  }

  यावत्(nthreads && !परिणाम &&
        Curl_llist_गणनम्(&tpool->slots) < tpool->max_तन्तवः) {
    परिणाम = thrdslot_start(tpool);
    यदि(परिणाम)
      भङ्ग;
    --nthreads;
  }

  प्रत्यागम परिणाम;
}

CURLcode Curl_thrdpool_समुच्चयः_props(संरचना curl_thrdpool *tpool,
                                 uint32_t min_तन्तवः,
                                 uint32_t max_तन्तवः,
                                 uint32_t idle_time_ms)
{
  CURLcode परिणाम = CURLE_OK;
  size_t running;

  यदि(!max_तन्तवः || (min_तन्तवः > max_तन्तवः))
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  Curl_mutex_acquire(&tpool->तालकम्);
  tpool->min_तन्तवः = min_तन्तवः;
  tpool->max_तन्तवः = max_तन्तवः;
  tpool->idle_time_ms = idle_time_ms;
  running = Curl_llist_गणनम्(&tpool->slots);
  यदि(tpool->min_तन्तवः > running) {
    परिणाम = thrdpool_signal(tpool, tpool->min_तन्तवः - (uint32_t)running);
  }
  Curl_mutex_release(&tpool->तालकम्);

  प्रत्यागम परिणाम;
}

CURLcode Curl_thrdpool_create(संरचना curl_thrdpool **ptpool,
                              ध्रुव वर्ण *name,
                              uint32_t min_तन्तवः,
                              uint32_t max_तन्तवः,
                              uint32_t idle_time_ms,
                              Curl_thrdpool_take_item_cb *fn_take,
                              Curl_thrdpool_process_item_cb *fn_process,
                              Curl_thrdpool_return_item_cb *fn_return,
                              शून्य *user_दत्त)
{
  संरचना curl_thrdpool *tpool;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  DEBUGASSERT(name);

  tpool = curlx_calloc(1, परिमाण(*tpool));
  यदि(!tpool)
    गच्छ बहिर्गमनम्;
  tpool->refcount = 1;

  Curl_mutex_init(&tpool->तालकम्);
  Curl_cond_init(&tpool->await);
  Curl_llist_init(&tpool->slots, रिक्त);
  Curl_llist_init(&tpool->zombies, रिक्त);
  tpool->fn_take = fn_take;
  tpool->fn_process = fn_process;
  tpool->fn_return = fn_return;
  tpool->fn_user_दत्त = user_दत्त;

  /* a const string that remains */
  tpool->name = name;

#यदि_परिभाषितम् DEBUGBUILD
  {
    ध्रुव वर्ण *p = getenv("CURL_DBG_THRDPOOL_FAIL_STARTS");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_सङ्ख्या(&p, &l, अधिकतम_पूर्णाङ्कः))
        tpool->dbg_fail_starts = (पूर्णाङ्क)l;
    }
  }
#यदि_अन्तः

  परिणाम = Curl_thrdpool_समुच्चयः_props(tpool, min_तन्तवः, max_तन्तवः,
                                   idle_time_ms);

बहिर्गमनम्:
  यदि(परिणाम && tpool) {
    tpool->aborted = TRUE;
    thrdpool_unlink(tpool, FALSE);
    tpool = रिक्त;
  }
  *ptpool = tpool;
  प्रत्यागम परिणाम;
}

शून्य Curl_thrdpool_destroy(संरचना curl_thrdpool *tpool, bool join)
{
  Curl_mutex_acquire(&tpool->तालकम्);

  tpool->aborted = TRUE;

  यावत्(join && Curl_llist_गणनम्(&tpool->slots)) {
    thrdpool_wake_all(tpool);
    Curl_cond_wait(&tpool->await, &tpool->तालकम्);
  }

  thrdpool_join_zombies(tpool);

  /* detach all still running threads */
  यदि(Curl_llist_गणनम्(&tpool->slots)) {
    संरचना Curl_llist_ग्रन्थिः *e;
    प्रति(e = Curl_llist_शीर्ष(&tpool->slots); e; e = Curl_ग्रन्थिः_अग्र(e)) {
      संरचना thrdslot *tslot = Curl_ग्रन्थिः_elem(e);
      यदि(tslot->thread != curl_thread_t_null)
        Curl_thread_destroy(&tslot->thread);
    }
    tpool->detached = TRUE;
  }

  यदि(!thrdpool_unlink(tpool, TRUE)) {
    /* tpool not destroyed */
    Curl_mutex_release(&tpool->तालकम्);
  }
}

CURLcode Curl_thrdpool_signal(संरचना curl_thrdpool *tpool, uint32_t nthreads)
{
  CURLcode परिणाम;

  Curl_mutex_acquire(&tpool->तालकम्);
  परिणाम = thrdpool_signal(tpool, nthreads);
  Curl_mutex_release(&tpool->तालकम्);
  प्रत्यागम परिणाम;
}

स्थिर bool thrdpool_all_idle(संरचना curl_thrdpool *tpool)
{
  संरचना Curl_llist_ग्रन्थिः *e;
  प्रति(e = Curl_llist_शीर्ष(&tpool->slots); e; e = Curl_ग्रन्थिः_अग्र(e)) {
    संरचना thrdslot *tslot = Curl_ग्रन्थिः_elem(e);
    यदि(!tslot->idle)
      प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

CURLcode Curl_thrdpool_await_idle(संरचना curl_thrdpool *tpool,
                                  uint32_t timeout_ms)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना curltime अन्तः = { 0 };

  Curl_mutex_acquire(&tpool->तालकम्);
  DEBUGASSERT(!tpool->aborted);
  यदि(tpool->aborted) {
    परिणाम = CURLE_FAILED_INIT;
    गच्छ बहिर्गमनम्;
  }

  यावत्(!thrdpool_all_idle(tpool)) {
    यदि(timeout_ms) {
      timediff_t remain_ms;
      CURLcode r;

      यदि(!अन्तः.tv_sec && !अन्तः.tv_usec) {
        अन्तः = curlx_now();
        अन्तः.tv_sec += (time_t)(timeout_ms / 1000);
        अन्तः.tv_usec += (पूर्णाङ्क)(timeout_ms % 1000) * 1000;
        यदि(अन्तः.tv_usec >= 1000000) {
          अन्तः.tv_sec++;
          अन्तः.tv_usec -= 1000000;
        }
      }
      remain_ms = curlx_timediff_ms(curlx_now(), अन्तः);
      यदि(remain_ms <= 0)
        r = CURLE_OPERATION_TIMEDOUT;
      अन्यथा
        r = Curl_cond_timedwait(&tpool->await, &tpool->तालकम्,
                                (uint32_t)remain_ms);
      यदि(r == CURLE_OPERATION_TIMEDOUT) {
        परिणाम = r;
        भङ्ग;
      }
    }
    अन्यथा {
      Curl_cond_wait(&tpool->await, &tpool->तालकम्);
    }
  }

बहिर्गमनम्:
  thrdpool_join_zombies(tpool);
  Curl_mutex_release(&tpool->तालकम्);
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् CURLVERBOSE
शून्य Curl_thrdpool_trace(संरचना curl_thrdpool *tpool,
                         संरचना Curl_easy *दत्त)
{
  संरचना curl_trc_feat *feat = &Curl_trc_feat_तन्तवः;
  यदि(Curl_trc_ft_is_verbose(दत्त, feat)) {
    संरचना Curl_llist_ग्रन्थिः *e;
    संरचना curltime now = curlx_now();

    Curl_mutex_acquire(&tpool->तालकम्);
    यदि(!Curl_llist_गणनम्(&tpool->slots)) {
      Curl_trc_feat_infof(दत्त, feat, "[TPOOL-%s] no threads running",
                          tpool->name);
    }
    प्रति(e = Curl_llist_शीर्ष(&tpool->slots); e; e = Curl_ग्रन्थिः_अग्र(e)) {
      संरचना thrdslot *tslot = Curl_ग्रन्थिः_elem(e);
      timediff_t elapsed_ms = curlx_ptimediff_ms(&now, &tslot->starttime);
      यदि(!tslot->running) {
        Curl_trc_feat_infof(दत्त, feat, "[TPOOL-%s] [%u]: not running",
                            tpool->name, tslot->id);
      }
      अन्यथा यदि(!tslot->starttime.tv_sec && !tslot->starttime.tv_usec) {
        Curl_trc_feat_infof(दत्त, feat, "[TPOOL-%s] [%u]: starting...",
                            tpool->name, tslot->id);
      }
      अन्यथा यदि(tslot->idle) {
        Curl_trc_feat_infof(दत्त, feat, "[TPOOL-%s] [%u]: idle for %"
                            FMT_TIMEDIFF_T "ms",
                            tpool->name, tslot->id, elapsed_ms);
      }
      अन्यथा {
        timediff_t remain_ms = tslot->work_timeout_ms ?
          (tslot->work_timeout_ms - elapsed_ms) : 0;
        Curl_trc_feat_infof(दत्त, feat, "[TPOOL-%s] [%u]: busy %"
                            FMT_TIMEDIFF_T "ms, timeout in %" FMT_TIMEDIFF_T
                            "ms: %s",
                            tpool->name, tslot->id, elapsed_ms, remain_ms,
                            tslot->work_description);
      }
    }
    Curl_mutex_release(&tpool->तालकम्);
  }
}
#यदि_अन्तः

#यदि_अन्तः /* USE_THREADS */
