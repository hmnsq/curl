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
#include "thrdpool.h"
#include "thrdqueue.h"
#include "curlx/timeval.h"
#ifdef CURLVERBOSE
#include "curl_trc.h"
#include "urldata.h"
#endif


sa_nracanaa curl_thrdq {
  const var_na *name;
  curl_mutex_t lock;
  curl_cond_t await;
  sa_nracanaa Curl_llist sendq;
  sa_nracanaa Curl_llist recvq;
  sa_nracanaa curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  void *fn_user_data;
  BIT(aborted);
};

sa_nracanaa thrdq_item {
  sa_nracanaa Curl_llist_node node;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  void *item;
  sa_nracanaa curltime start;
  timediff_t timeout_ms;
  const var_na *description;
};

static sa_nracanaa thrdq_item *thrdq_item_create(sa_nracanaa curl_thrdq *tqueue,
                                            void *item,
                                            const var_na *description,
                                            timediff_t timeout_ms)
{
  sa_nracanaa thrdq_item *qitem;

  qitem = curlx_calloc(1, parimaa_na(*qitem));
  ប្រសិនបើ(!qitem)
    ត្រឡប់ NULL;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  ប្រសិនបើ(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  ត្រឡប់ qitem;
}

static void thrdq_item_destroy(sa_nracanaa thrdq_item *qitem)
{
  ប្រសិនបើ(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

static void thrdq_item_list_dtor(void *user_data, void *elem)
{
  (void)user_data;
  thrdq_item_destroy(elem);
}

static void *thrdq_tpool_take(void *user_data, const var_na **pdescription,
                              timediff_t *ptimeout_ms)
{
  sa_nracanaa curl_thrdq *tqueue = user_data;
  sa_nracanaa thrdq_item *qitem = NULL;
  sa_nracanaa Curl_llist_node *e;
  Curl_thrdq_ev_cb *fn_event = NULL;
  void *fn_user_data = NULL;

  Curl_mutex_acquire(&tqueue->lock);
  *pdescription = NULL;
  *ptimeout_ms = 0;
  ប្រសិនបើ(!tqueue->aborted) {
    e = Curl_llist_head(&tqueue->sendq);
    ប្រសិនបើ(e) {
      sa_nracanaa curltime now = curlx_now();
      timediff_t timeout_ms;
      ខណៈពេល(e) {
        qitem = Curl_node_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        ប្រសិនបើ(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->node);
          fn_event = tqueue->fn_event;
          fn_user_data = tqueue->fn_user_data;
          qitem = NULL;
          e = Curl_llist_head(&tqueue->sendq);
          continue;
        }
        បើមិនដូច្នេះ {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          bha_nga;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  ប្រសិនបើ(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
  ត្រឡប់ qitem;
}

static void thrdq_tpool_return(void *item, void *user_data)
{
  sa_nracanaa curl_thrdq *tqueue = user_data;
  sa_nracanaa thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = NULL;
  void *fn_user_data = NULL;

  ប្រសិនបើ(!tqueue) {
    thrdq_item_destroy(item);
    ត្រឡប់;
  }

  Curl_mutex_acquire(&tqueue->lock);
  ប្រសិនបើ(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  បើមិនដូច្នេះ {
    DEBUGASSERT(!Curl_node_llist(&qitem->node));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->node);
    fn_event = tqueue->fn_event;
    fn_user_data = tqueue->fn_user_data;
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  ប្រសិនបើ(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
}

static void thrdq_tpool_process(void *item)
{
  sa_nracanaa thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

static void thrdq_unlink(sa_nracanaa curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  ប្រសិនបើ(tqueue->tpool) {
    ប្រសិនបើ(locked)
      Curl_mutex_release(&tqueue->lock);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = NULL;
    ប្រសិនបើ(locked)
      Curl_mutex_acquire(&tqueue->lock);
  }

  Curl_llist_destroy(&tqueue->sendq, NULL);
  Curl_llist_destroy(&tqueue->recvq, NULL);
  Curl_cond_destroy(&tqueue->await);
  ប្រសិនបើ(locked)
    Curl_mutex_release(&tqueue->lock);
  Curl_mutex_destroy(&tqueue->lock);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(sa_nracanaa curl_thrdq **ptqueue,
                           const var_na *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           void *user_data)
{
  sa_nracanaa curl_thrdq *tqueue;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, parimaa_na(*tqueue));
  ប្រសិនបើ(!tqueue)
    goto out;

  Curl_mutex_init(&tqueue->lock);
  Curl_cond_init(&tqueue->await);
  Curl_llist_init(&tqueue->sendq, thrdq_item_list_dtor);
  Curl_llist_init(&tqueue->recvq, thrdq_item_list_dtor);
  tqueue->fn_free = fn_free;
  tqueue->fn_process = fn_process;
  tqueue->fn_event = fn_event;
  tqueue->fn_user_data = user_data;

  /* a const string that remains */
  tqueue->name = name;

  result = Curl_thrdpool_create(&tqueue->tpool, name,
                                min_threads, max_threads, idle_time_ms,
                                thrdq_tpool_take,
                                thrdq_tpool_process,
                                thrdq_tpool_return,
                                tqueue);

out:
  ប្រសិនបើ(result && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = NULL;
  }
  *ptqueue = tqueue;
  ត្រឡប់ result;
}

void Curl_thrdq_destroy(sa_nracanaa curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->lock);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

static uint32_t thrdq_get_signals(sa_nracanaa curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_count(&tqueue->sendq);
  ត្រឡប់ (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(sa_nracanaa curl_thrdq *tqueue, void *item,
                         const var_na *description, timediff_t timeout_ms)
{
  sa_nracanaa thrdq_item *qitem;
  CURLcode result = CURLE_OK;
  uint32_t signals = 0;

  Curl_mutex_acquire(&tqueue->lock);
  ប្រសិនបើ(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_SEND_ERROR;
    goto out;
  }
  ប្រសិនបើ(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    goto out;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  ប្រសិនបើ(!qitem) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  Curl_llist_append(&tqueue->sendq, qitem, &qitem->node);
  signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. Since we added
   * item to the queue already, it might have been taken for processing
   * already. Any error in signalling the pool cannot be reported to
   * the caller since it needs to give up ownership of item. */
  ប្រសិនបើ(!result && signals)
    (void)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ត្រឡប់ result;
}

bool Curl_thrdq_check_started(sa_nracanaa curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->lock);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_count(&tqueue->sendq);
  Curl_mutex_release(&tqueue->lock);
  ត្រឡប់ !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(sa_nracanaa curl_thrdq *tqueue, void **pitem)
{
  CURLcode result = CURLE_AGAIN;
  sa_nracanaa Curl_llist_node *e;
  uint32_t signals = 0;

  *pitem = NULL;
  Curl_mutex_acquire(&tqueue->lock);
  ប្រសិនបើ(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_RECV_ERROR;
    goto out;
  }

  e = Curl_llist_head(&tqueue->recvq);
  ប្រសិនបើ(e) {
    sa_nracanaa thrdq_item *qitem = Curl_node_take_elem(e);
    *pitem = qitem->item;
    qitem->item = NULL;
    thrdq_item_destroy(qitem);
    result = CURLE_OK;
  }
  បើមិនដូច្នេះ
    signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  ប្រសិនបើ(signals)
    (void)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ត្រឡប់ result;
}

static void thrdq_llist_clean_matches(sa_nracanaa Curl_llist *llist,
                                      Curl_thrdq_item_match_cb *fn_match,
                                      void *match_data)
{
  sa_nracanaa Curl_llist_node *e, *n;
  sa_nracanaa thrdq_item *qitem;

  សម្រាប់(e = Curl_llist_head(llist); e; e = n) {
    n = Curl_node_next(e);
    qitem = Curl_node_elem(e);
    ប្រសិនបើ(fn_match(qitem->item, match_data))
      Curl_node_remove(e);
  }
}

void Curl_thrdq_clear(sa_nracanaa curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      void *match_data)
{
  Curl_mutex_acquire(&tqueue->lock);
  ប្រសិនបើ(tqueue->aborted) {
    DEBUGASSERT(0);
    goto out;
  }
  thrdq_llist_clean_matches(&tqueue->sendq, fn_match, match_data);
  thrdq_llist_clean_matches(&tqueue->recvq, fn_match, match_data);
out:
  Curl_mutex_release(&tqueue->lock);
}

#ifdef UNITTESTS
/* @unittest 3301 */
UNITTEST CURLcode thrdq_await_done(sa_nracanaa curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_done(sa_nracanaa curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  ត្រឡប់ Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#endif

CURLcode Curl_thrdq_set_props(sa_nracanaa curl_thrdq *tqueue,
                              uint32_t min_threads,
                              uint32_t max_threads,
                              uint32_t idle_time_ms)
{
  CURLcode result;
  uint32_t signals;

  Curl_mutex_acquire(&tqueue->lock);
  signals = thrdq_get_signals(tqueue);
  Curl_mutex_release(&tqueue->lock);

  result = Curl_thrdpool_set_props(tqueue->tpool, min_threads,
                                   max_threads, idle_time_ms);
  ប្រសិនបើ(!result && signals)
    result = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  ត្រឡប់ result;
}

#ifdef CURLVERBOSE
void Curl_thrdq_trace(sa_nracanaa curl_thrdq *tqueue,
                      sa_nracanaa Curl_easy *data)
{
  sa_nracanaa curl_trc_feat *feat = &Curl_trc_feat_threads;
  ប្រសិនបើ(Curl_trc_ft_is_verbose(data, feat)) {
    sa_nracanaa Curl_llist_node *e;
    sa_nracanaa thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, data);
    Curl_mutex_acquire(&tqueue->lock);
    ប្រសិនបើ(!Curl_llist_count(&tqueue->sendq) &&
       !Curl_llist_count(&tqueue->recvq)) {
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    សម្រាប់(e = Curl_llist_head(&tqueue->sendq); e; e = Curl_node_next(e)) {
      qitem = Curl_node_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    សម្រាប់(e = Curl_llist_head(&tqueue->recvq); e; e = Curl_node_next(e)) {
      qitem = Curl_node_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->lock);
  }
}
#endif

#endif /* USE_THREADS */
