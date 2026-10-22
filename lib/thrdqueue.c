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


struct curl_thrdq {
  const char *name;
  curl_mutex_t lock;
  curl_cond_t await;
  struct Curl_llist sendq;
  struct Curl_llist recvq;
  struct curl_thrdpool *tpool;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  Curl_thrdq_ev_cb *fn_event;
  void *fn_user_data;
  BIT(aborted);
};

struct thrdq_item {
  struct Curl_llist_node node;
  Curl_thrdq_item_free_cb *fn_free;
  Curl_thrdq_item_process_cb *fn_process;
  void *item;
  struct curltime start;
  timediff_t timeout_ms;
  const char *description;
};

static struct thrdq_item *thrdq_item_create(struct curl_thrdq *tqueue,
                                            void *item,
                                            const char *description,
                                            timediff_t timeout_ms)
{
  struct thrdq_item *qitem;

  qitem = curlx_calloc(1, sizeof(*qitem));
  ако(!qitem)
    върни NULL;
  qitem->item = item;
  qitem->description = description;
  qitem->fn_free = tqueue->fn_free;
  qitem->fn_process = tqueue->fn_process;
  ако(timeout_ms) {
    qitem->start = curlx_now();
    qitem->timeout_ms = timeout_ms;
  }
  върни qitem;
}

static void thrdq_item_destroy(struct thrdq_item *qitem)
{
  ако(qitem->item)
    qitem->fn_free(qitem->item);
  curlx_free(qitem);
}

static void thrdq_item_list_dtor(void *user_data, void *elem)
{
  (void)user_data;
  thrdq_item_destroy(elem);
}

static void *thrdq_tpool_take(void *user_data, const char **pdescription,
                              timediff_t *ptimeout_ms)
{
  struct curl_thrdq *tqueue = user_data;
  struct thrdq_item *qitem = NULL;
  struct Curl_llist_node *e;
  Curl_thrdq_ev_cb *fn_event = NULL;
  void *fn_user_data = NULL;

  Curl_mutex_acquire(&tqueue->lock);
  *pdescription = NULL;
  *ptimeout_ms = 0;
  ако(!tqueue->aborted) {
    e = Curl_llist_head(&tqueue->sendq);
    ако(e) {
      struct curltime now = curlx_now();
      timediff_t timeout_ms;
      докато(e) {
        qitem = Curl_node_take_elem(e);
        timeout_ms = (!qitem->timeout_ms) ? 0 :
          (qitem->timeout_ms - curlx_ptimediff_ms(&now, &qitem->start));
        ако(timeout_ms < 0) {
          /* timed out while queued, place on receive queue */
          Curl_llist_append(&tqueue->recvq, qitem, &qitem->node);
          fn_event = tqueue->fn_event;
          fn_user_data = tqueue->fn_user_data;
          qitem = NULL;
          e = Curl_llist_head(&tqueue->sendq);
          continue;
        }
        иначе {
          *pdescription = qitem->description;
          *ptimeout_ms = timeout_ms;
          break;
        }
      }
    }
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  ако(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
  върни qitem;
}

static void thrdq_tpool_return(void *item, void *user_data)
{
  struct curl_thrdq *tqueue = user_data;
  struct thrdq_item *qitem = item;
  Curl_thrdq_ev_cb *fn_event = NULL;
  void *fn_user_data = NULL;

  ако(!tqueue) {
    thrdq_item_destroy(item);
    върни;
  }

  Curl_mutex_acquire(&tqueue->lock);
  ако(tqueue->aborted) {
    thrdq_item_destroy(qitem);
  }
  иначе {
    DEBUGASSERT(!Curl_node_llist(&qitem->node));
    Curl_llist_append(&tqueue->recvq, qitem, &qitem->node);
    fn_event = tqueue->fn_event;
    fn_user_data = tqueue->fn_user_data;
  }
  Curl_mutex_release(&tqueue->lock);
  /* avoiding deadlocks */
  ако(fn_event)
    fn_event(tqueue, CURL_THRDQ_EV_ITEM_DONE, fn_user_data);
}

static void thrdq_tpool_process(void *item)
{
  struct thrdq_item *qitem = item;
  qitem->fn_process(qitem->item);
}

static void thrdq_unlink(struct curl_thrdq *tqueue, bool locked, bool join)
{
  DEBUGASSERT(tqueue->aborted);
  ако(tqueue->tpool) {
    ако(locked)
      Curl_mutex_release(&tqueue->lock);
    Curl_thrdpool_destroy(tqueue->tpool, join);
    tqueue->tpool = NULL;
    ако(locked)
      Curl_mutex_acquire(&tqueue->lock);
  }

  Curl_llist_destroy(&tqueue->sendq, NULL);
  Curl_llist_destroy(&tqueue->recvq, NULL);
  Curl_cond_destroy(&tqueue->await);
  ако(locked)
    Curl_mutex_release(&tqueue->lock);
  Curl_mutex_destroy(&tqueue->lock);
  curlx_free(tqueue);
}

CURLcode Curl_thrdq_create(struct curl_thrdq **ptqueue,
                           const char *name,
                           uint32_t min_threads,
                           uint32_t max_threads,
                           uint32_t idle_time_ms,
                           Curl_thrdq_item_free_cb *fn_free,
                           Curl_thrdq_item_process_cb *fn_process,
                           Curl_thrdq_ev_cb *fn_event,
                           void *user_data)
{
  struct curl_thrdq *tqueue;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  DEBUGASSERT(name);

  tqueue = curlx_calloc(1, sizeof(*tqueue));
  ако(!tqueue)
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
  ако(result && tqueue) {
    tqueue->aborted = TRUE;
    thrdq_unlink(tqueue, FALSE, TRUE);
    tqueue = NULL;
  }
  *ptqueue = tqueue;
  върни result;
}

void Curl_thrdq_destroy(struct curl_thrdq *tqueue, bool join)
{
  Curl_mutex_acquire(&tqueue->lock);
  DEBUGASSERT(!tqueue->aborted);
  tqueue->aborted = TRUE;
  thrdq_unlink(tqueue, TRUE, join);
}

static uint32_t thrdq_get_signals(struct curl_thrdq *tqueue)
{
  size_t qlen = Curl_llist_count(&tqueue->sendq);
  върни (qlen <= UINT32_MAX) ? (uint32_t)qlen : UINT32_MAX;
}

CURLcode Curl_thrdq_send(struct curl_thrdq *tqueue, void *item,
                         const char *description, timediff_t timeout_ms)
{
  struct thrdq_item *qitem;
  CURLcode result = CURLE_OK;
  uint32_t signals = 0;

  Curl_mutex_acquire(&tqueue->lock);
  ако(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_SEND_ERROR;
    goto out;
  }
  ако(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    goto out;
  }

  qitem = thrdq_item_create(tqueue, item, description, timeout_ms);
  ако(!qitem) {
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
  ако(!result && signals)
    (void)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  върни result;
}

bool Curl_thrdq_check_started(struct curl_thrdq *tqueue)
{
  size_t unprocessed;

  Curl_mutex_acquire(&tqueue->lock);
  unprocessed = tqueue->aborted ? 0 : Curl_llist_count(&tqueue->sendq);
  Curl_mutex_release(&tqueue->lock);
  върни !unprocessed ||
         !Curl_thrdpool_signal(tqueue->tpool, (uint32_t)unprocessed);
}

CURLcode Curl_thrdq_recv(struct curl_thrdq *tqueue, void **pitem)
{
  CURLcode result = CURLE_AGAIN;
  struct Curl_llist_node *e;
  uint32_t signals = 0;

  *pitem = NULL;
  Curl_mutex_acquire(&tqueue->lock);
  ако(tqueue->aborted) {
    DEBUGASSERT(0);
    result = CURLE_RECV_ERROR;
    goto out;
  }

  e = Curl_llist_head(&tqueue->recvq);
  ако(e) {
    struct thrdq_item *qitem = Curl_node_take_elem(e);
    *pitem = qitem->item;
    qitem->item = NULL;
    thrdq_item_destroy(qitem);
    result = CURLE_OK;
  }
  иначе
    signals = thrdq_get_signals(tqueue);

out:
  Curl_mutex_release(&tqueue->lock);
  /* Signal thread pool unlocked to avoid deadlocks. If items await
   * processing while nothing was ready, make sure the pool has a
   * thread to work on them. An earlier thread start may have failed,
   * which `Curl_thrdq_send()` cannot report to its caller. Without
   * this, such items would sit unprocessed until the next send. */
  ако(signals)
    (void)Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  върни result;
}

static void thrdq_llist_clean_matches(struct Curl_llist *llist,
                                      Curl_thrdq_item_match_cb *fn_match,
                                      void *match_data)
{
  struct Curl_llist_node *e, *n;
  struct thrdq_item *qitem;

  за(e = Curl_llist_head(llist); e; e = n) {
    n = Curl_node_next(e);
    qitem = Curl_node_elem(e);
    ако(fn_match(qitem->item, match_data))
      Curl_node_remove(e);
  }
}

void Curl_thrdq_clear(struct curl_thrdq *tqueue,
                      Curl_thrdq_item_match_cb *fn_match,
                      void *match_data)
{
  Curl_mutex_acquire(&tqueue->lock);
  ако(tqueue->aborted) {
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
UNITTEST CURLcode thrdq_await_done(struct curl_thrdq *tqueue,
                                   uint32_t timeout_ms);
UNITTEST CURLcode thrdq_await_done(struct curl_thrdq *tqueue,
                                   uint32_t timeout_ms)
{
  върни Curl_thrdpool_await_idle(tqueue->tpool, timeout_ms);
}
#endif

CURLcode Curl_thrdq_set_props(struct curl_thrdq *tqueue,
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
  ако(!result && signals)
    result = Curl_thrdpool_signal(tqueue->tpool, (uint32_t)signals);
  върни result;
}

#ifdef CURLVERBOSE
void Curl_thrdq_trace(struct curl_thrdq *tqueue,
                      struct Curl_easy *data)
{
  struct curl_trc_feat *feat = &Curl_trc_feat_threads;
  ако(Curl_trc_ft_is_verbose(data, feat)) {
    struct Curl_llist_node *e;
    struct thrdq_item *qitem;

    Curl_thrdpool_trace(tqueue->tpool, data);
    Curl_mutex_acquire(&tqueue->lock);
    ако(!Curl_llist_count(&tqueue->sendq) &&
       !Curl_llist_count(&tqueue->recvq)) {
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] empty", tqueue->name);
    }
    за(e = Curl_llist_head(&tqueue->sendq); e; e = Curl_node_next(e)) {
      qitem = Curl_node_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] in: %s",
                          tqueue->name, qitem->description);
    }
    за(e = Curl_llist_head(&tqueue->recvq); e; e = Curl_node_next(e)) {
      qitem = Curl_node_elem(e);
      Curl_trc_feat_infof(data, feat, "[TQUEUE-%s] out: %s",
                          tqueue->name, qitem->description);
    }
    Curl_mutex_release(&tqueue->lock);
  }
}
#endif

#endif /* USE_THREADS */
