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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "curl_share.h"
#అన్తర్భావ "psl.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "multi_ev.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "http.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "curlx/wait.h"
#అన్తర్భావ "conncache.h"
#అన్తర్భావ "multihandle.h"
#అన్తర్భావ "sigpipe.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/vtls_scache.h"
#అన్తర్భావ "http_proxy.h"
#అన్తర్భావ "http2.h"
#అన్తర్భావ "socketpair.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "curlx/strparse.h"

/* initial multi->xfers table size for a full multi */
#నిర్వచించు CURL_XFER_పట్టిక_కొలత 128

/* CURL_SOCKET_HASH_TABLE_SIZE should be a prime number. Increasing it from 97
   to 911 takes on a 32-bit machine 4 x 804 = 3211 more bytes. Still, every
   curl handle takes 6K memory, therefore this 3K are not significant. */
#నిర్వచితం_కాకపోతే CURL_SOCKET_HASH_పట్టిక_కొలత
#నిర్వచించు CURL_SOCKET_HASH_పట్టిక_కొలత 911
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_CONNECTION_HASH_కొలత
#నిర్వచించు CURL_CONNECTION_HASH_కొలత 97
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DNS_HASH_కొలత
#నిర్వచించు CURL_DNS_HASH_కొలత 71
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_TLS_SESSION_కొలత
#నిర్వచించు CURL_TLS_SESSION_కొలత 25
#అయితే_ముగింపు

స్థిర శూన్య move_pending_గమ్యం_connect(నిర్మాణం Curl_multi *multi,
                                    నిర్మాణం Curl_easy *దత్తాంశం);
స్థిర CURLMcode కూడు_తదుపరి_timeout(స్థిరరాశి నిర్మాణం curltime *pnow,
                                  నిర్మాణం Curl_multi *multi,
                                  నిర్మాణం Curl_easy *దత్తాంశం);
స్థిర శూన్య multi_timeout(నిర్మాణం Curl_multi *multi,
                          timediff_t *pexire_స్థానభ్రంశం_us,
                          పూర్ణాంకం *timeout_ms);
స్థిర శూన్య multi_కాలపట్టిక_pending(నిర్మాణం Curl_multi *multi);
స్థిర శూన్య multi_xfer_bufs_free(నిర్మాణం Curl_multi *multi);
#నిర్వచితమైతే DEBUGBUILD
స్థిర శూన్య multi_xfer_tbl_dump(నిర్మాణం Curl_multi *multi);
#అయితే_ముగింపు
/* Get the # of transfers current in process/pending. */
స్థిర uint32_t multi_xfers_running(నిర్మాణం Curl_multi *multi);

/* function pointer called once when entering a state */
రకనిర్వచనం శూన్య (*mstate_enter_func)(నిర్మాణం Curl_easy *దత్తాంశం,
                                  CURLMstate from_state);

స్థిర శూన్య mstate_enter_connect(నిర్మాణం Curl_easy *దత్తాంశం,
                                 CURLMstate from_state)
{
  (శూన్య)from_state;
  Curl_init_CONNECT(దత్తాంశం);
}

స్థిర శూన్య mstate_enter_did(నిర్మాణం Curl_easy *దత్తాంశం,
                             CURLMstate from_state)
{
  (శూన్య)from_state;
  దత్తాంశం->req.chunk = FALSE;
  Curl_pgrs_సమయం(దత్తాంశం, TIMER_PRETRANSFER);
  ఒకవేళ(!CURL_REQ_WANT_SEND(దత్తాంశం))
    Curl_pgrs_సమయం(దత్తాంశం, TIMER_POSTRANSFER);
}

స్థిర శూన్య mstate_enter_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                              CURLMstate from_state)
{
  (శూన్య)from_state;
  CURLM_NTFY(దత్తాంశం, CURLMNOTIFY_EASY_పూర్తి);
}

స్థిర శూన్య mstate_enter_completed(నిర్మాణం Curl_easy *దత్తాంశం,
                                   CURLMstate from_state)
{
  /* we sometimes directly jump to COMPLETED, trigger things
   * we then missed. */
  ఒకవేళ(from_state < MSTATE_DID) {
    Curl_pgrs_సమయం(దత్తాంశం, TIMER_PRETRANSFER);
    Curl_pgrs_సమయం(దత్తాంశం, TIMER_POSTRANSFER);
    Curl_pgrs_సమయం(దత్తాంశం, TIMER_STARTTRANSFER);
  }
  /* multi_done() should have run and detached any connection already */
  DEBUGASSERT(!దత్తాంశం->conn);
  Curl_pgrsCompleted(దత్తాంశం);
  ఒకవేళ(from_state < MSTATE_పూర్తి)
    CURLM_NTFY(దత్తాంశం, CURLMNOTIFY_EASY_పూర్తి);
  /* changing to COMPLETED means it is in process and needs to go */
  DEBUGASSERT(Curl_uint32_bset_కలిగియుంది(&దత్తాంశం->multi->process, దత్తాంశం->mid));
  Curl_uint32_bset_remove(&దత్తాంశం->multi->process, దత్తాంశం->mid);
  Curl_uint32_bset_remove(&దత్తాంశం->multi->pending, దత్తాంశం->mid); /* to be sure */

  ఒకవేళ(Curl_uint32_bset_ఖాళీలు(&దత్తాంశం->multi->process)) {
    /* free the transfer buffer when we have no more active transfers */
    multi_xfer_bufs_free(దత్తాంశం->multi);
  }
  /* Important: reset the conn pointer so that we do not point to memory
     that could be freed anytime */
  Curl_detach_connection(దత్తాంశం);
  Curl_expire_clear_all(దత్తాంశం); /* stop all timers */
}

/* always use this function to change state, to make debugging easier */
స్థిర శూన్య mstate(నిర్మాణం Curl_easy *దత్తాంశం, CURLMstate state
#నిర్వచితమైతే DEBUGBUILD
                   , పూర్ణాంకం lineno
#అయితే_ముగింపు
)
{
  CURLMstate oldstate = దత్తాంశం->mstate;
  స్థిర స్థిరరాశి mstate_enter_func state_enter[MSTATE_చివరి] = {
    ఖాళీ,                      /* INIT */
    ఖాళీ,                      /* PENDING */
    ఖాళీ,                      /*అమరిక*/
    mstate_enter_connect,      /* CONNECT */
    ఖాళీ,                      /* CONNECTING */
    ఖాళీ,                      /* PROTOCONNECT */
    ఖాళీ,                      /* PROTOCONNECTING */
    ఖాళీ,                      /* DO */
    ఖాళీ,                      /* DOING */
    ఖాళీ,                      /* DOING_MORE */
    mstate_enter_did,          /* DID */
    ఖాళీ,                      /* PERFORMING */
    ఖాళీ,                      /* RATELIMITING */
    mstate_enter_పూర్తి,         /* DONE */
    mstate_enter_completed,    /* COMPLETED */
    ఖాళీ                       /* MSGSENT */
  };

  ఒకవేళ(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    తిరిగి;

#నిర్వచితమైతే DEBUGBUILD
  NOVERBOSE((శూన్య)lineno);
  CURL_TRC_M(దత్తాంశం, "-> [%s] (line %d)", CURL_MSTATE_NAME(state), lineno);
#లేకపోతే
  CURL_TRC_M(దత్తాంశం, "-> [%s]", CURL_MSTATE_NAME(state));
#అయితే_ముగింపు

  /* really switching state */
  దత్తాంశం->mstate = state;
  ఒకవేళ(state_enter[state])
    state_enter[state](దత్తాంశం, oldstate);
}

#నిర్వచితం_కాకపోతే DEBUGBUILD
#నిర్వచించు multistate(x, y) mstate(x, y)
#లేకపోతే
#నిర్వచించు multistate(x, y) mstate(x, y, __LINE__)
#అయితే_ముగింపు

/* multi->proto_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
స్థిర శూన్య ph_freeentry(శూన్య *p)
{
  (శూన్య)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

/*
 * multi_addmsg()
 *
 * Called when a transfer is completed. Marks its message as unread.
 */
స్థిర శూన్య multi_addmsg(నిర్మాణం Curl_multi *multi, నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(Curl_uint32_bset_ఖాళీలు(&multi->msgsent))
    CURLM_NTFY(multi->admin, CURLMNOTIFY_INFO_READ);
  Curl_uint32_bset_కూడు(&multi->msgsent, దత్తాంశం->mid);
}

స్థిర శూన్య multi_timeouts_init(నిర్మాణం Curl_easy *దత్తాంశం);

నిర్మాణం Curl_multi *Curl_multi_handle(uint32_t xfer_పట్టిక_కొలత,
                                     size_t ev_hashsize,  /* event hash */
                                     size_t chashsize, /* connection hash */
                                     size_t dnssize,   /* dns hash */
                                     size_t sesssize)  /* TLS session cache */
{
  నిర్మాణం curltime created;
  నిర్మాణం Curl_multi *multi = curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_multi));

  ఒకవేళ(!multi)
    తిరిగి ఖాళీ;

  multi->magic = CURLMULTI_MAGIC_సంఖ్య;

  /* Initialisation order is important here!
   * easy_init() does a lazy check on curl_global_init() which sets
   * up platform specific things we need. For example calling curlx_pnow()
   * before this is not safe. */
  multi->admin = curl_easy_init();
  ఒకవేళ(!multi->admin) {
    curlx_free(multi);
    తిరిగి ఖాళీ;
  }
  multi->admin->multi = multi;
  multi->admin->state.అంతర్గత = TRUE;

  /* Now we can use curlx_* things safely */
  curlx_pnow(&created);
  Curl_timeouts_init(&multi->timeouts, &created);
  multi_timeouts_init(multi->admin);

  Curl_dnscache_init(&multi->dnscache, dnssize);
  Curl_mntfy_init(multi);
  Curl_multi_ev_init(multi, ev_hashsize);
  Curl_uint32_tbl_init(&multi->xfers);
  Curl_uint32_bset_init(&multi->process);
  Curl_uint32_bset_init(&multi->dirty);
  Curl_uint32_bset_init(&multi->pending);
  Curl_uint32_bset_init(&multi->msgsent);
  Curl_hash_init(&multi->proto_hash, 23, CURL_HASH_TYPE_బైట్లు, ph_freeentry);
  Curl_cshutdn_init(&multi->cshutdn);

  multi->multiplexing = TRUE;
  multi->max_concurrent_streams = 100;
#నిర్వచితమైతే ENABLE_WAKEUP
  multi->wakeup_pair[0] = CURL_SOCKET_BAD;
  multi->wakeup_pair[1] = CURL_SOCKET_BAD;
#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_అంతర్గత_WAKEUP
  multi->wakeup_అంతర్గత[0] = CURL_SOCKET_BAD;
  multi->wakeup_అంతర్గత[1] = CURL_SOCKET_BAD;
#అయితే_ముగింపు

  ఒకవేళ(Curl_uint32_bset_resize(&multi->process, xfer_పట్టిక_కొలత) ||
     Curl_uint32_bset_resize(&multi->pending, xfer_పట్టిక_కొలత) ||
     Curl_uint32_bset_resize(&multi->dirty, xfer_పట్టిక_కొలత) ||
     Curl_uint32_bset_resize(&multi->msgsent, xfer_పట్టిక_కొలత) ||
     Curl_uint32_tbl_resize(&multi->xfers, xfer_పట్టిక_కొలత))
    వెళ్ళు error;

#నిర్వచితమైతే DEBUGBUILD
  {
    స్థిరరాశి అక్షరం *p;
    ఒకవేళ(getenv("CURL_DEBUG"))
      multi->admin->సమితి.verbose = TRUE;
    p = getenv("CURL_DBG_MAX_TOTAL_CONNECTIONS");
    ఒకవేళ(p) {
      curl_off_t l;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, UINT32_MAX)) {
        multi->max_మొత్తం_connections = (uint32_t)l;
      }
    }
  }
#అయితే_ముగింపు
  Curl_uint32_tbl_కూడు(&multi->xfers, multi->admin, &multi->admin->mid);
  Curl_uint32_bset_కూడు(&multi->process, multi->admin->mid);

  Curl_cpool_init(&multi->cpool, ఖాళీ, chashsize);

#నిర్వచితమైతే USE_SSL
  ఒకవేళ(Curl_ssl_scache_create(sesssize, 2, &multi->ssl_scache))
    వెళ్ళు error;
#లేకపోతే
  (శూన్య)sesssize;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_WINSOCK
  multi->wsa_event = WSACreateEvent();
  ఒకవేళ(multi->wsa_event == WSA_INVALID_EVENT)
    వెళ్ళు error;
#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_WAKEUP
  /* When enabled, rely on this to work. We ignore this in previous
   * versions, but that seems an unnecessary complication. */
  ఒకవేళ(Curl_wakeup_init(multi->wakeup_pair, TRUE) < 0)
    వెళ్ళు error;
#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_అంతర్గత_WAKEUP
  ఒకవేళ(Curl_wakeup_init(multi->wakeup_అంతర్గత, TRUE) < 0)
    వెళ్ళు error;
#అయితే_ముగింపు

  ఒకవేళ(Curl_probeipv6(multi))
    వెళ్ళు error;

#నిర్వచితమైతే USE_RESOLV_THREADED
  ఒకవేళ(xfer_పట్టిక_కొలత < CURL_XFER_పట్టిక_కొలత) { /* easy multi */
    ఒకవేళ(Curl_async_thrdd_multi_init(multi, 0, 2, 10))
      వెళ్ళు error;
  }
  లేకపోతే { /* real multi handle */
    ఒకవేళ(Curl_async_thrdd_multi_init(multi, 0, 20, 2000))
      వెళ్ళు error;
  }
#అయితే_ముగింపు

  తిరిగి multi;

error:

#నిర్వచితమైతే USE_RESOLV_THREADED
  Curl_async_thrdd_multi_destroy(multi, TRUE);
#అయితే_ముగింపు
  Curl_multi_ev_cleanup(multi);
  Curl_hash_destroy(&multi->proto_hash);
  Curl_dnscache_destroy(&multi->dnscache);
  Curl_cpool_destroy(&multi->cpool, multi->admin);
  Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
#నిర్వచితమైతే USE_SSL
  Curl_ssl_scache_destroy(multi->ssl_scache);
#అయితే_ముగింపు
  ఒకవేళ(multi->admin) {
    Curl_multi_ev_xfer_పూర్తి(multi, multi->admin);
    multi->admin->multi = ఖాళీ;
    Curl_close(&multi->admin);
  }
  Curl_mntfy_cleanup(multi);

  Curl_uint32_bset_destroy(&multi->process);
  Curl_uint32_bset_destroy(&multi->dirty);
  Curl_uint32_bset_destroy(&multi->pending);
  Curl_uint32_bset_destroy(&multi->msgsent);
  Curl_uint32_tbl_destroy(&multi->xfers);
#నిర్వచితమైతే ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_అంతర్గత_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_అంతర్గత);
#అయితే_ముగింపు

  curlx_free(multi);
  తిరిగి ఖాళీ;
}

CURLM *curl_multi_init(శూన్య)
{
  తిరిగి Curl_multi_handle(CURL_XFER_పట్టిక_కొలత,
                           CURL_SOCKET_HASH_పట్టిక_కొలత,
                           CURL_CONNECTION_HASH_కొలత,
                           CURL_DNS_HASH_కొలత,
                           CURL_TLS_SESSION_కొలత);
}

#ఒకవేళ defined(DEBUGBUILD) && defined(CURLVERBOSE)
స్థిర శూన్య multi_warn_debug(నిర్మాణం Curl_multi *multi, నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(!multi->warned) {
    infof(దత్తాంశం, "!!! WARNING !!!");
    infof(దత్తాంశం, "This is a debug build of libcurl, "
                "do not use in production.");
    multi->warned = TRUE;
  }
}
#లేకపోతే
#నిర్వచించు multi_warn_debug(x, y) Curl_nop_stmt
#అయితే_ముగింపు

bool Curl_is_connecting(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి దత్తాంశం->mstate < MSTATE_DO;
}

స్థిర CURLMcode multi_assess_wakeup(నిర్మాణం Curl_multi *multi)
{
#నిర్వచితమైతే ENABLE_అంతర్గత_WAKEUP
  ఒకవేళ(multi->socket_cb)
    తిరిగి Curl_multi_ev_assess_xfer(multi, multi->admin);
#లేకపోతే
  (శూన్య)multi;
#అయితే_ముగింపు
  తిరిగి CURLM_OK;
}

స్థిర CURLMcode multi_xfers_కూడు(నిర్మాణం Curl_multi *multi,
                                 నిర్మాణం Curl_easy *దత్తాంశం)
{
  uint32_t సామర్థ్యం = Curl_uint32_tbl_సామర్థ్యం(&multi->xfers);
  uint32_t new_కొలత = 0;
  /* Prepare to make this into a CURLMOPT_MAX_TRANSFERS, because some
   * applications may want to prevent a run-away of their memory use. */
  /* UINT_MAX is our "invalid" id, do not let the table grow up to that. */
  స్థిరరాశి uint32_t max_సామర్థ్యం = UINT_MAX - 1;

  ఒకవేళ(సామర్థ్యం < max_సామర్థ్యం) {
    /* We want `multi->xfers` to have "sufficient" free rows, so that we do
     * not have to reuse the `mid` from a removed easy right away.
     * Check if an 8th of the capacity is still free */
    uint32_t వాడిన = Curl_uint32_tbl_లెక్క(&multi->xfers);
    uint32_t వాడని = సామర్థ్యం - వాడిన;
    uint32_t min_వాడని = CURLMAX(సామర్థ్యం >> 3, 4);
    ఒకవేళ(వాడని < min_వాడని) {
      /* Grow by 50% of current capacity, in range of [128, 2048],
       * which means the table grows max by 16kb on 64-bit arch. */
      uint32_t growth = CURLMIN(CURLMAX(సామర్థ్యం >> 1, 128), 2048);
      /* Make sure the uint arithmetic here works on the corner
       * cases where we are close to max_capacity or UINT_MAX */
      ఒకవేళ((max_సామర్థ్యం - growth) <= సామర్థ్యం)
        new_కొలత = max_సామర్థ్యం;
      లేకపోతే
        new_కొలత = సామర్థ్యం + growth;
    }
  }

  ఒకవేళ(new_కొలత > సామర్థ్యం) {
    /* Grow the bitsets first. Should one fail, we do not need
     * to downsize the already resized ones. The sets continue
     * to work properly when larger than the table, but not
     * the other way around. */
    CURL_TRC_M(దత్తాంశం, "increasing xfer table size to %u", new_కొలత);
    ఒకవేళ(Curl_uint32_bset_resize(&multi->process, new_కొలత) ||
       Curl_uint32_bset_resize(&multi->dirty, new_కొలత) ||
       Curl_uint32_bset_resize(&multi->pending, new_కొలత) ||
       Curl_uint32_bset_resize(&multi->msgsent, new_కొలత) ||
       Curl_uint32_tbl_resize(&multi->xfers, new_కొలత))
      తిరిగి CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  /* Insert the easy into the table now */
  ఒకవేళ(!Curl_uint32_tbl_కూడు(&multi->xfers, దత్తాంశం, &దత్తాంశం->mid)) {
    /* MUST only happen when table is full */
    DEBUGASSERT(Curl_uint32_tbl_సామర్థ్యం(&multi->xfers) <=
                Curl_uint32_tbl_లెక్క(&multi->xfers));
    తిరిగి CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  తిరిగి CURLM_OK;
}

CURLMcode Curl_multi_కూడు_handle(నిర్మాణం Curl_multi *multi,
                                నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLMcode mresult;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  ఒకవేళ(దత్తాంశం->multi)
    తిరిగి CURLM_ADDED_ALREADY;

  ఒకవేళ(multi->dead) {
    /* a "dead" handle cannot get added transfers while any existing easy
       handles are still alive - but if there are none alive anymore, it is
       fine to start over and unmark the "deadness" of this handle.
       This means only the admin handle MUST be present. */
    ఒకవేళ((Curl_uint32_tbl_లెక్క(&multi->xfers) != 1) ||
       !Curl_uint32_tbl_కలిగియుంది(&multi->xfers, 0))
      తిరిగి CURLM_ABORTED_BY_CALLBACK;
    multi->dead = FALSE;
    Curl_uint32_bset_clear(&multi->process);
    Curl_uint32_bset_clear(&multi->dirty);
    Curl_uint32_bset_clear(&multi->pending);
    Curl_uint32_bset_clear(&multi->msgsent);
  }

  ఒకవేళ(దత్తాంశం->multi_easy) {
    /* if this easy handle was previously used for curl_easy_perform(), there
       is a private multi handle here that we can kill */
    curl_multi_cleanup(దత్తాంశం->multi_easy);
    దత్తాంశం->multi_easy = ఖాళీ;
  }

  /* Insert the easy into the multi->xfers table, assigning it a `mid`. */
  ఒకవేళ(multi_xfers_కూడు(multi, దత్తాంశం))
    తిరిగి CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Initialize timeouts for this handle */
  multi_timeouts_init(దత్తాంశం);

  /*
   * No failure allowed in this function beyond this point. No modification
   * of easy nor multi handle allowed before this except for potential
   * multi's connection pool growing which will not be undone in this
   * function no matter what.
   */
  ఒకవేళ(దత్తాంశం->సమితి.errorbuffer)
    దత్తాంశం->సమితి.errorbuffer[0] = 0;

  దత్తాంశం->state.os_errno = 0;

  /* make the Curl_easy refer back to this multi handle - before
     Curl_expire() is called. */
  దత్తాంశం->multi = multi;

  /* set the easy handle */
  multistate(దత్తాంశం, MSTATE_INIT);
  /* not yet passed INIT state */
  దత్తాంశం->state.really_alive = FALSE;

#నిర్వచితమైతే USE_LIBPSL
  /* Do the same for PSL. */
  ఒకవేళ(దత్తాంశం->share && (దత్తాంశం->share->specifier & (1 << CURL_తాళం_దత్తాంశం_PSL)))
    దత్తాంశం->psl = &దత్తాంశం->share->psl;
  లేకపోతే
    దత్తాంశం->psl = &multi->psl;
#అయితే_ముగింపు

  /* add the easy handle to the process set */
  Curl_uint32_bset_కూడు(&multi->process, దత్తాంశం->mid);
  ++multi->xfers_alive;
  ++multi->xfers_మొత్తం_ever;

  Curl_cpool_xfer_init(దత్తాంశం);
  multi_warn_debug(multi, దత్తాంశం);

  /* Make sure the new handle will run */
  Curl_multi_mark_dirty(దత్తాంశం);

  /* Necessary in event based processing, where dirty handles trigger
   * a timeout callback invocation. */
  mresult = Curl_update_timer(multi);
  ఒకవేళ(mresult) {
    /* undo the additions above */
    Curl_uint32_bset_remove(&multi->process, దత్తాంశం->mid);
    Curl_uint32_bset_remove(&multi->dirty, దత్తాంశం->mid);
    --multi->xfers_alive;
    దత్తాంశం->multi = ఖాళీ; /* not anymore */
    Curl_uint32_tbl_remove(&multi->xfers, దత్తాంశం->mid);
    దత్తాంశం->mid = UINT32_MAX;
    తిరిగి mresult;
  }

  /* The admin handle only ever has default timeouts set. To improve the
     state somewhat we clone the timeouts from each added handle so that the
     admin handle always has the same timeouts as the most recently added
     easy handle. */
  multi->admin->సమితి.timeout = దత్తాంశం->సమితి.timeout;
  multi->admin->సమితి.server_response_timeout =
    దత్తాంశం->సమితి.server_response_timeout;
  multi->admin->సమితి.no_signal = దత్తాంశం->సమితి.no_signal;

  CURL_TRC_M(దత్తాంశం, "added to multi, mid=%u, running=%u, total=%u",
             దత్తాంశం->mid, multi_xfers_running(multi),
             Curl_uint32_tbl_లెక్క(&multi->xfers));
  తిరిగి CURLM_OK;
}

CURLMcode curl_multi_కూడు_handle(CURLM *m, CURL *curl)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_కూడు_handle, &mresult)) {
    నిర్మాణం Curl_easy *దత్తాంశం = curl;
    /* Verify that we got a somewhat good easy handle too */
    ఒకవేళ(!GOOD_EASY_HANDLE(దత్తాంశం) || దత్తాంశం->state.అంతర్గత)
      mresult = CURLM_BAD_EASY_HANDLE;
    లేకపోతే
      mresult = Curl_multi_కూడు_handle(m, దత్తాంశం);
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

నిర్మాణం multi_పూర్తి_ctx {
  బిట్టు(premature);
};

స్థిర bool multi_conn_should_close(నిర్మాణం connectdata *conn,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    bool premature)
{
  /* if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of everything else. */
  ఒకవేళ(conn->బిట్లు.close)
    తిరిగి TRUE;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM/NEGOTIATE authentication handshake. */
  ఒకవేళ(దత్తాంశం->సమితి.reuse_forbid
#నిర్వచితమైతే USE_NTLM
     && !(conn->http_ntlm_state == NTLMSTATE_TYPE2 ||
          conn->proxy_ntlm_state == NTLMSTATE_TYPE2)
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SPNEGO
     && !(conn->http_negotiate_state == CURL_NEGO_AUTH_RECV ||
          conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)
#అయితే_ముగింపు
    )
    తిరిగి TRUE;

  /* Unless this connection is for a "connect-only" transfer, it
   * needs to be closed if the protocol handler does not support reuse. */
  ఒకవేళ(!దత్తాంశం->సమితి.connect_only && conn->scheme &&
     !(conn->scheme->flags & PROTOPT_CONN_REUSE))
    తిరిగి TRUE;

  /* if premature is TRUE, it means this connection was said to be DONE before
     the entire request operation is complete and thus we cannot know in what
     state it is for reusing, so we are forced to close it. In a perfect world
     we can add code that keep track of if we really must close it here or not,
     but currently we have no such detail knowledge. */
  ఒకవేళ(premature && !Curl_conn_is_multiplex(conn, FIRSTSOCKET))
    తిరిగి TRUE;

  తిరిగి FALSE;
}

స్థిర cpool_do_ఫలితం multi_పూర్తి_locked(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం connectdata *conn,
                                         శూన్య *userdata,
                                         స్థిరరాశి నిర్మాణం curltime *pnow)
{
  నిర్మాణం multi_పూర్తి_ctx *mdctx = userdata;

  (శూన్య)pnow;
  CURL_TRC_M(దత్తాంశం, "multi_done_locked, in use=%u", conn->attached_xfers);
  ఒకవేళ(CONN_INUSE(conn)) {
    /* Stop if still used. */
    CURL_TRC_M(దత్తాంశం, "Connection still in use %u, no more multi_done now!",
               conn->attached_xfers);
    తిరిగి CPOOL_DO_KEEP;
  }

  ఒకవేళ(multi_conn_should_close(conn, దత్తాంశం, (bool)mdctx->premature)) {
    CURL_TRC_M(దత్తాంశం, "multi_done, terminating conn #%" FMT_OFF_T " to %s:%u, "
               "forbid=%d, close=%d, premature=%d, conn_multiplex=%d",
               conn->connection_id, conn->origin->user_hostname,
               conn->origin->పోర్టు,
               దత్తాంశం->సమితి.reuse_forbid, conn->బిట్లు.close, mdctx->premature,
               Curl_conn_is_multiplex(conn, FIRSTSOCKET));
    connclose(conn);
    తిరిగి mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  లేకపోతే ఒకవేళ(!Curl_conn_get_max_concurrent(దత్తాంశం, conn, FIRSTSOCKET)) {
    CURL_TRC_M(దత్తాంశం, "multi_done, conn #%" FMT_OFF_T " to %s:%u was shutdown"
               " by server, not reusing", conn->connection_id,
               conn->origin->user_hostname, conn->origin->పోర్టు);
    connclose(conn);
    తిరిగి mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  లేకపోతే {
    తిరిగి CPOOL_DO_IDLE;
  }
}

స్థిర CURLcode multi_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                           CURLcode స్థితి_2,  /* an error if this is called
                                                after an error was detected */
                           bool premature)
{
  CURLcode ఫలితం;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  స్థిరరాశి నిర్మాణం curltime *pnow = Curl_pgrs_now(దత్తాంశం);

  CURL_TRC_M(దత్తాంశం, "multi_done: status: %d prem: %d done: %d",
             (పూర్ణాంకం)స్థితి_2, (పూర్ణాంకం)premature, దత్తాంశం->state.పూర్తి);

  ఒకవేళ(దత్తాంశం->state.పూర్తి) {
    /* Stop if multi_done() has already been called */
    తిరిగి CURLE_OK;
  }
  దత్తాంశం->state.పూర్తి = TRUE; /* called now! */

  /* Shut down any ongoing async resolver operation. */
  Curl_resolv_shutdown_all(దత్తాంశం);

  /* Cleanup possible redirect junk */
  curlx_safefree(దత్తాంశం->req.newurl);
  curlx_safefree(దత్తాంశం->req.location);

  ఎంపిక(స్థితి_2) {
  స్థితి CURLE_ABORTED_BY_CALLBACK:
  స్థితి CURLE_READ_ERROR:
  స్థితి CURLE_WRITE_ERROR:
    /* When we are aborted due to a callback return code it has to be counted
       as premature as there is trouble ahead if we do not. We have many
       callbacks and protocols work differently, we could potentially do this
       more fine-grained in the future. */
    premature = TRUE;
    FALLTHROUGH();
  అప్రమేయ:
    ఆపు;
  }

  /* this calls the protocol-specific function pointer previously set */
  ఒకవేళ(conn && conn->scheme->నిరంతరం->పూర్తి && (దత్తాంశం->mstate >= MSTATE_PROTOCONNECT))
    ఫలితం = conn->scheme->నిరంతరం->పూర్తి(దత్తాంశం, స్థితి_2, premature);
  లేకపోతే
    ఫలితం = స్థితి_2;

  ఒకవేళ(దత్తాంశం->mstate > MSTATE_CONNECTING &&
     (ఫలితం != CURLE_ABORTED_BY_CALLBACK)) {
    /* avoid this if
     * - the transfer has not connected
     * - we already aborted by callback to avoid this calling another callback
     */
    పూర్ణాంకం rc = Curl_pgrs_పూర్తి(దత్తాంశం, pnow);
    ఒకవేళ(!ఫలితం && rc)
      ఫలితం = CURLE_ABORTED_BY_CALLBACK;
  }

  /* Make sure that transfer client writes are really done now. */
  ఫలితం = Curl_1st_fatal(ఫలితం, Curl_xfer_write_పూర్తి(దత్తాంశం, premature));

  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_req_పూర్తి(&దత్తాంశం->req, దత్తాంశం, premature);

  ఒకవేళ(conn) {
    /* Detach connection and decide what to do with it. */
    నిర్మాణం multi_పూర్తి_ctx mdctx;

    /* Inform connection filters that this transfer is done and they
     * need to give up any references to `data`. */
    Curl_conn_ev_దత్తాంశం_పూర్తి(దత్తాంశం, premature);
    Curl_detach_connection(దత్తాంశం);
    Curl_dnscache_prune(దత్తాంశం, pnow);

    memset(&mdctx, 0, పరిమాణం(mdctx));
    mdctx.premature = premature;
    Curl_cpool_return(దత్తాంశం, conn, multi_పూర్తి_locked, &mdctx, pnow);

    ఒకవేళ(దత్తాంశం->multi)
      multi_కాలపట్టిక_pending(దత్తాంశం->multi);
  }

  /* flush the netrc cache */
  Curl_netrc_cleanup(&దత్తాంశం->state.netrc);
  తిరిగి ఫలితం;
}

CURLMcode Curl_multi_remove_handle(నిర్మాణం Curl_multi *multi,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLMcode mresult;
  bool premature;
  uint32_t mid;

  /* Prevent users from trying to remove same easy handle more than once */
  ఒకవేళ(!దత్తాంశం->multi)
    తిరిగి CURLM_OK; /* it is already removed so let's say it is fine! */

  /* Prevent users from trying to remove an easy handle from the wrong multi */
  ఒకవేళ(దత్తాంశం->multi != multi)
    తిరిగి CURLM_BAD_EASY_HANDLE;

  ఒకవేళ(దత్తాంశం->mid == UINT32_MAX) {
    DEBUGASSERT(0);
    తిరిగి CURLM_అంతర్గత_ERROR;
  }
  ఒకవేళ(Curl_uint32_tbl_get(&multi->xfers, దత్తాంశం->mid) != దత్తాంశం) {
    DEBUGASSERT(0);
    తిరిగి CURLM_అంతర్గత_ERROR;
  }

  premature = (దత్తాంశం->mstate < MSTATE_COMPLETED);

  ఒకవేళ(దత్తాంశం->conn) {
    /* If the 'state' is not INIT or COMPLETED, we might need to do something
       nice to put the easy_handle in a good known state when this returns. */
    ఒకవేళ(premature && (దత్తాంశం->mstate > MSTATE_DO))
      streamclose(దత్తాంశం->conn);

    /* multi_done() clears the association between the easy handle and the
       connection.
       Note that this ignores the return code because there is
       nothing really useful to do with it anyway! */
    (శూన్య)multi_పూర్తి(దత్తాంశం, దత్తాంశం->ఫలితం, premature);
  }

  /* The timer must be shut down before data->multi is set to NULL, else
     data's splaynode would remain in the splay tree after curl_easy_cleanup is
     called. Do it after multi_done() in case that sets another time! */
  Curl_expire_clear_all(దత్తాంశం);

  /* In MSGSENT, it was deducted from `multi->xfers_alive` already. */
  ఒకవేళ(దత్తాంశం->mstate != MSTATE_MSGSENT)
    --multi->xfers_alive;

  ఒకవేళ(దత్తాంశం->state.really_alive) {
    దత్తాంశం->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    ఒకవేళ(!multi->xfers_really_alive)
      (శూన్య)multi_assess_wakeup(multi);
  }

  Curl_wildcard_dtor(&దత్తాంశం->wildcard);

  దత్తాంశం->mstate = MSTATE_COMPLETED;

  /* Remove the association between the connection and the handle */
  Curl_detach_connection(దత్తాంశం);

  /* Tell event handling that this transfer is definitely going away */
  Curl_multi_ev_xfer_పూర్తి(multi, దత్తాంశం);

  ఒకవేళ(దత్తాంశం->సమితి.connect_only) {
    ఒకవేళ(దత్తాంశం->multi_easy) {
      ఒకవేళ(దత్తాంశం->state.చివరి_conn_id != -1) {
        /* Mark any connect-only connection for closure */
        నిర్మాణం connectdata *conn;
        (శూన్య)Curl_getconnectinfo(దత్తాంశం, &conn);
        ఒకవేళ(conn && conn->బిట్లు.connect_only)
          connclose(conn);
      }
    }
    లేకపోతే {
      /* This removes a handle that was part the multi interface that used
         CONNECT_ONLY, that connection is now left alive but since this handle
         has bits.close set nothing can use that connection anymore and it is
         forbidden from reuse. This easy handle cannot find the connection
         anymore once removed from the multi handle

         Better close the connection here, at once. */
      నిర్మాణం connectdata *conn;
      (శూన్య)Curl_getconnectinfo(దత్తాంశం, &conn);
      ఒకవేళ(conn)
        Curl_conn_close(దత్తాంశం, conn, TRUE);
    }
  }

#నిర్వచితమైతే USE_LIBPSL
  /* Remove the PSL association. */
  ఒకవేళ(దత్తాంశం->psl == &multi->psl)
    దత్తాంశం->psl = ఖాళీ;
#అయితే_ముగింపు

  /* clear the association to this multi handle */
  mid = దత్తాంశం->mid;
  DEBUGASSERT(Curl_uint32_tbl_కలిగియుంది(&multi->xfers, mid));
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  Curl_uint32_bset_remove(&multi->process, mid);
  Curl_uint32_bset_remove(&multi->dirty, mid);
  Curl_uint32_bset_remove(&multi->pending, mid);
  Curl_uint32_bset_remove(&multi->msgsent, mid);
  దత్తాంశం->multi = ఖాళీ;
  దత్తాంశం->mid = UINT32_MAX;
  దత్తాంశం->master_mid = UINT32_MAX;

  /* A pending transfer *might* be able to run now. */
  multi_కాలపట్టిక_pending(multi);
  mresult = Curl_update_timer(multi);
  ఒకవేళ(mresult)
    తిరిగి mresult;

  mresult = multi_assess_wakeup(multi);
  ఒకవేళ(mresult) {
    failf(దత్తాంశం, "error enabling wakeup listening: %d", mresult);
    తిరిగి mresult;
  }

  CURL_TRC_M(దత్తాంశం, "removed from multi, mid=%u, running=%u, total=%u",
             mid, multi_xfers_running(multi),
             Curl_uint32_tbl_లెక్క(&multi->xfers));
  తిరిగి CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *curl)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_remove_handle, &mresult)) {
    నిర్మాణం Curl_easy *దత్తాంశం = curl;
    ఒకవేళ(!GOOD_EASY_HANDLE(దత్తాంశం) || దత్తాంశం->state.అంతర్గత)
      mresult = CURLM_BAD_EASY_HANDLE;
    లేకపోతే
      mresult = Curl_multi_remove_handle(m, దత్తాంశం);
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

/* Return TRUE if the application asked for multiplexing */
bool Curl_multiplex_wanted(స్థిరరాశి నిర్మాణం Curl_multi *multi)
{
  తిరిగి multi && multi->multiplexing;
}

/*
 * Curl_detach_connection() removes the given transfer from the connection.
 *
 * This is the only function that should clear data->conn. This will
 * occasionally be called with the data->conn pointer already cleared.
 */
శూన్య Curl_detach_connection(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  ఒకవేళ(conn) {
    /* this should never happen, prevent underflow */
    DEBUGASSERT(conn->attached_xfers);
    ఒకవేళ(conn->attached_xfers) {
      conn->attached_xfers--;
      ఒకవేళ(!conn->attached_xfers)
        conn->attached_multi = ఖాళీ;
    }
  }
  దత్తాంశం->conn = ఖాళీ;
}

/*
 * Curl_attach_connection() attaches this transfer to this connection.
 *
 * This is the only function that should assign data->conn.
 * `matched == TRUE` means the transfer's properties match this
 * connection and it is not a temporary attach for maintenance.
 */
శూన్య Curl_attach_connection(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం connectdata *conn,
                            bool matched)
{
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(!దత్తాంశం->conn);
  DEBUGASSERT(conn);
  DEBUGASSERT(conn->attached_xfers < UINT32_MAX);
  దత్తాంశం->conn = conn;
  ఒకవేళ(matched) {
    దత్తాంశం->state.చివరి_conn_id = conn->connection_id;
    దత్తాంశం->state.చివరి_cpid = conn->cpid;
  }
  లేకపోతే
    DEBUGASSERT(!దత్తాంశం->mid); /* admin handle */
  conn->attached_xfers++;
  /* all attached transfers must be from the same multi */
  ఒకవేళ(!conn->attached_multi)
    conn->attached_multi = దత్తాంశం->multi;
  DEBUGASSERT(conn->attached_multi == దత్తాంశం->multi);

  ఒకవేళ(conn->scheme && conn->scheme->నిరంతరం->attach)
    conn->scheme->నిరంతరం->attach(దత్తాంశం, conn);
}

/* adjust pollset for rate limits/pauses */
స్థిర CURLcode multi_adjust_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం easy_pollset *ps)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(ps->n) {
    bool send_blocked, recv_blocked;

    recv_blocked = (Curl_rlimit_avail(&దత్తాంశం->పురోగతి.dl.rlimit, ఖాళీ) <= 0);
    send_blocked = (Curl_rlimit_avail(&దత్తాంశం->పురోగతి.ul.rlimit, ఖాళీ) <= 0);
    ఒకవేళ(send_blocked || recv_blocked) {
      పూర్ణాంకం i;
      కొరకు(i = 0; i <= SECONDARYSOCKET; ++i) {
        curl_socket_t sock = దత్తాంశం->conn->sock[i];
        ఒకవేళ(sock == CURL_SOCKET_BAD)
          కొనసాగు;
        ఒకవేళ(recv_blocked && Curl_pollset_want_recv(దత్తాంశం, ps, sock)) {
          ఫలితం = Curl_pollset_remove_లోపల(దత్తాంశం, ps, sock);
          ఒకవేళ(ఫలితం)
            ఆపు;
        }
        ఒకవేళ(send_blocked && Curl_pollset_want_send(దత్తాంశం, ps, sock)) {
          ఫలితం = Curl_pollset_remove_బహిర్గతం(దత్తాంశం, ps, sock);
          ఒకవేళ(ఫలితం)
            ఆపు;
        }
      }
    }

    /* Not blocked and wanting to receive. If there is data pending
     * in the connection filters, make transfer run again. */
    ఒకవేళ(!recv_blocked &&
       ((Curl_pollset_want_recv(దత్తాంశం, ps, దత్తాంశం->conn->sock[FIRSTSOCKET]) &&
         Curl_conn_దత్తాంశం_pending(దత్తాంశం, FIRSTSOCKET)) ||
        (Curl_pollset_want_recv(దత్తాంశం, ps, దత్తాంశం->conn->sock[SECONDARYSOCKET]) &&
         Curl_conn_దత్తాంశం_pending(దత్తాంశం, SECONDARYSOCKET)))) {
      CURL_TRC_M(దత్తాంశం, "pollset[] has POLLIN, but there is still "
                 "buffered input -> mark as dirty");
      Curl_multi_mark_dirty(దత్తాంశం);
    }
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode mstate_connecting_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  curl_socket_t sockfd;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(Curl_xfer_recv_is_paused(దత్తాంశం))
    తిరిగి CURLE_OK;
  /* If a socket is set, receiving is default. If the socket
   * has not been determined yet (eyeballing), always ask the
   * connection filters for what to monitor. */
  sockfd = Curl_conn_get_మొదటి_socket(దత్తాంశం);
  ఒకవేళ(sockfd != CURL_SOCKET_BAD) {
    ఫలితం = Curl_pollset_change(దత్తాంశం, ps, sockfd, CURL_POLL_లోపల, 0);
    ఒకవేళ(!ఫలితం)
      ఫలితం = multi_adjust_pollset(దత్తాంశం, ps);
  }
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_conn_adjust_pollset(దత్తాంశం, conn, ps);
  తిరిగి ఫలితం;
}

స్థిర CURLcode mstate_protocol_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(conn->scheme->నిరంతరం->proto_pollset)
    ఫలితం = conn->scheme->నిరంతరం->proto_pollset(దత్తాంశం, ps);
  లేకపోతే {
    curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
    ఒకవేళ(sockfd != CURL_SOCKET_BAD) {
      /* Default is to wait to something from the server */
      ఫలితం = Curl_pollset_change(దత్తాంశం, ps, sockfd, CURL_POLL_లోపల, 0);
    }
  }
  ఒకవేళ(!ఫలితం)
    ఫలితం = multi_adjust_pollset(దత్తాంశం, ps);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_conn_adjust_pollset(దత్తాంశం, conn, ps);
  తిరిగి ఫలితం;
}

స్థిర CURLcode mstate_do_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(conn->scheme->నిరంతరం->doing_pollset)
    ఫలితం = conn->scheme->నిరంతరం->doing_pollset(దత్తాంశం, ps);
  లేకపోతే ఒకవేళ(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    ఫలితం = Curl_pollset_కూడు_బహిర్గతం(దత్తాంశం, ps, conn->sock[conn->send_idx]);
  }
  ఒకవేళ(!ఫలితం)
    ఫలితం = multi_adjust_pollset(దత్తాంశం, ps);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_conn_adjust_pollset(దత్తాంశం, conn, ps);
  తిరిగి ఫలితం;
}

స్థిర CURLcode mstate_domore_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(conn->scheme->నిరంతరం->domore_pollset)
    ఫలితం = conn->scheme->నిరంతరం->domore_pollset(దత్తాంశం, ps);
  లేకపోతే ఒకవేళ(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    ఫలితం = Curl_pollset_కూడు_బహిర్గతం(దత్తాంశం, ps, conn->sock[conn->send_idx]);
  }
  ఒకవేళ(!ఫలితం)
    ఫలితం = multi_adjust_pollset(దత్తాంశం, ps);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_conn_adjust_pollset(దత్తాంశం, conn, ps);
  తిరిగి ఫలితం;
}

స్థిర CURLcode mstate_perform_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(conn->scheme->నిరంతరం->perform_pollset)
    ఫలితం = conn->scheme->నిరంతరం->perform_pollset(దత్తాంశం, ps);
  లేకపోతే {
    /* Default is to obey the request flags for send/recv */
    ఒకవేళ(Curl_req_want_recv(దత్తాంశం) && CONN_SOCK_IDX_VALID(conn->recv_idx)) {
      ఫలితం = Curl_pollset_కూడు_లోపల(దత్తాంశం, ps, conn->sock[conn->recv_idx]);
    }
    ఒకవేళ(!ఫలితం && Curl_req_want_send(దత్తాంశం) &&
       CONN_SOCK_IDX_VALID(conn->send_idx)) {
      ఫలితం = Curl_pollset_కూడు_బహిర్గతం(దత్తాంశం, ps, conn->sock[conn->send_idx]);
    }
  }
  ఒకవేళ(!ఫలితం)
    ఫలితం = multi_adjust_pollset(దత్తాంశం, ps);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_conn_adjust_pollset(దత్తాంశం, conn, ps);
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే CURLVERBOSE
స్థిర size_t multi_timeouts_లెక్క(నిర్మాణం expire_timers *timeouts)
{
  size_t n = 0;
  uint8_t eid = timeouts->మొదటి;
  కొరకు(; eid < EXPIRE_చివరి; eid = timeouts->తదుపరి[eid])
    ++n;
  తిరిగి n;
}
#అయితే_ముగింపు

/* Initializes `poll_set` with the current socket poll actions needed
 * for transfer `data`. */
CURLMcode Curl_multi_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం easy_pollset *ps)
{
  CURLcode ఫలితం = CURLE_OK;

  Curl_pollset_reset(ps);
#నిర్వచితమైతే ENABLE_అంతర్గత_WAKEUP
  /* The admin handle always listens on the wakeup socket when there
   * are transfers alive. */
  ఒకవేళ(దత్తాంశం->multi && (దత్తాంశం == దత్తాంశం->multi->admin) &&
     దత్తాంశం->multi->xfers_really_alive) {
    CURL_TRC_M(దత్తాంశం, "adding wakeup, %u xfers really alive",
               దత్తాంశం->multi->xfers_really_alive);
    ఫలితం = Curl_pollset_కూడు_లోపల(దత్తాంశం, ps, దత్తాంశం->multi->wakeup_అంతర్గత[0]);
  }
#అయితే_ముగింపు
  /* If the transfer has no connection, this is fine. Happens when
     called via curl_multi_remove_handle() => Curl_multi_ev_assess() =>
     Curl_multi_pollset(). */
  ఒకవేళ(!ఫలితం && దత్తాంశం->conn) {
    ఎంపిక(దత్తాంశం->mstate) {
    స్థితి MSTATE_INIT:
    స్థితి MSTATE_PENDING:
    స్థితి MSTATE_అమరిక:
    స్థితి MSTATE_CONNECT:
      /* nothing to poll for yet */
      ఆపు;

    స్థితి MSTATE_CONNECTING:
      ఫలితం = mstate_connecting_pollset(దత్తాంశం, ps);
      ఆపు;

    స్థితి MSTATE_PROTOCONNECT:
    స్థితి MSTATE_PROTOCONNECTING:
      ఫలితం = mstate_protocol_pollset(దత్తాంశం, ps);
      ఆపు;

    స్థితి MSTATE_DO:
    స్థితి MSTATE_DOING:
      ఫలితం = mstate_do_pollset(దత్తాంశం, ps);
      ఆపు;

    స్థితి MSTATE_DOING_మరిన్ని:
      ఫలితం = mstate_domore_pollset(దత్తాంశం, ps);
      ఆపు;

    స్థితి MSTATE_DID: /* same as PERFORMING in regard to polling */
    స్థితి MSTATE_PERFORMING:
      ఫలితం = mstate_perform_pollset(దత్తాంశం, ps);
      ఆపు;

    స్థితి MSTATE_RATELIMITING:
      /* we need to let time pass, ignore socket(s) */
      ఆపు;

    స్థితి MSTATE_పూర్తి:
    స్థితి MSTATE_COMPLETED:
    స్థితి MSTATE_MSGSENT:
      /* nothing more to poll for */
      ఆపు;

    అప్రమేయ:
      failf(దత్తాంశం, "multi_getsock: unexpected multi state %d",
            (పూర్ణాంకం)దత్తాంశం->mstate);
      DEBUGASSERT(0);
      ఆపు;
    }
  }

  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
      తిరిగి CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
    failf(దత్తాంశం, "error determining pollset: %d", (పూర్ణాంకం)ఫలితం);
    తిరిగి CURLM_అంతర్గత_ERROR;
  }

#నిర్వచితమైతే CURLVERBOSE
  ఒకవేళ(CURL_TRC_M_is_verbose(దత్తాంశం)) {
    size_t timeout_లెక్క = multi_timeouts_లెక్క(&దత్తాంశం->state.timeouts);
    ఎంపిక(ps->n) {
    స్థితి 0:
      CURL_TRC_M(దత్తాంశం, "pollset[], timeouts=%zu, paused %d/%d (r/w)",
                 timeout_లెక్క,
                 Curl_xfer_send_is_paused(దత్తాంశం),
                 Curl_xfer_recv_is_paused(దత్తాంశం));
      ఆపు;
    స్థితి 1:
      CURL_TRC_M(దత్తాంశం, "pollset[fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_లోపల) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_బహిర్గతం) ? "OUT" : "",
                 timeout_లెక్క);
      ఆపు;
    స్థితి 2:
      CURL_TRC_M(దత్తాంశం, "pollset[fd=%" FMT_SOCKET_T " %s%s, "
                 "fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_లోపల) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_బహిర్గతం) ? "OUT" : "",
                 ps->sockets[1],
                 (ps->actions[1] & CURL_POLL_లోపల) ? "IN" : "",
                 (ps->actions[1] & CURL_POLL_బహిర్గతం) ? "OUT" : "",
                 timeout_లెక్క);
      ఆపు;
    అప్రమేయ:
      CURL_TRC_M(దత్తాంశం, "pollset[fds=%u], timeouts=%zu", ps->n, timeout_లెక్క);
      ఆపు;
    }
    CURL_TRC_EASY_TIMERS(దత్తాంశం);
  }
#అయితే_ముగింపు

  తిరిగి CURLM_OK;
}

CURLMcode curl_multi_fdset(CURLM *m,
                           fd_set *read_fd_సమితి, fd_set *write_fd_సమితి,
                           fd_set *exc_fd_సమితి, పూర్ణాంకం *max_fd)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_fdset, &mresult)) {
    /* Scan through all the easy handles to get the file descriptors set.
       Some easy handles may not have connected to the remote host yet,
       and then we must make sure that is done. */
    నిర్మాణం Curl_multi *multi = m;
    నిర్మాణం easy_pollset ps;
    పూర్ణాంకం this_max_fd = -1;
    గుర్తులేని పూర్ణాంకం i;
    uint32_t mid;
    (శూన్య)exc_fd_సమితి;

    Curl_pollset_init(&ps);
    ఒకవేళ(Curl_uint32_bset_మొదటి(&multi->process, &mid)) {
      చేయి {
        నిర్మాణం Curl_easy *దత్తాంశం = Curl_multi_get_easy(multi, mid);

        ఒకవేళ(!దత్తాంశం) {
          DEBUGASSERT(0);
          కొనసాగు;
        }

        Curl_multi_pollset(దత్తాంశం, &ps);
        కొరకు(i = 0; i < ps.n; i++) {
          ఒకవేళ(!FDSET_SOCK(ps.sockets[i]))
            /* pretend it does not exist */
            కొనసాగు;
          ఒకవేళ(ps.actions[i] & CURL_POLL_లోపల)
            FD_SET(ps.sockets[i], read_fd_సమితి);
          ఒకవేళ(ps.actions[i] & CURL_POLL_బహిర్గతం)
            FD_SET(ps.sockets[i], write_fd_సమితి);
          ఒకవేళ((పూర్ణాంకం)ps.sockets[i] > this_max_fd)
            this_max_fd = (పూర్ణాంకం)ps.sockets[i];
        }
      } అయితే(Curl_uint32_bset_తదుపరి(&multi->process, mid, &mid));
    }

    Curl_cshutdn_setfds(&multi->cshutdn, multi->admin,
                        read_fd_సమితి, write_fd_సమితి, &this_max_fd);

    *max_fd = this_max_fd;
    Curl_pollset_cleanup(&ps);

    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

CURLMcode curl_multi_waitfds(CURLM *m,
                             నిర్మాణం curl_waitfd *ufds,
                             గుర్తులేని పూర్ణాంకం కొలత,
                             గుర్తులేని పూర్ణాంకం *fd_లెక్క)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_waitfds, &mresult)) {
    నిర్మాణం Curl_waitfds cwfds;
    నిర్మాణం Curl_multi *multi = m;
    నిర్మాణం easy_pollset ps;
    గుర్తులేని పూర్ణాంకం need = 0;
    uint32_t mid;

    ఒకవేళ(!ufds && (కొలత || !fd_లెక్క)) {
      mresult = CURLM_BAD_FUNCTION_వాదం;
      వెళ్ళు బహిర్గతం;
    }

    Curl_pollset_init(&ps);
    Curl_waitfds_init(&cwfds, ufds, కొలత);
    mresult = CURLM_OK;
    ఒకవేళ(Curl_uint32_bset_మొదటి(&multi->process, &mid)) {
      చేయి {
        నిర్మాణం Curl_easy *దత్తాంశం = Curl_multi_get_easy(multi, mid);
        ఒకవేళ(!దత్తాంశం) {
          DEBUGASSERT(0);
          Curl_uint32_bset_remove(&multi->process, mid);
          Curl_uint32_bset_remove(&multi->dirty, mid);
          కొనసాగు;
        }
        Curl_multi_pollset(దత్తాంశం, &ps);
        need += Curl_waitfds_కూడు_ps(&cwfds, &ps);
      } అయితే(Curl_uint32_bset_తదుపరి(&multi->process, mid, &mid));
    }

    need += Curl_cshutdn_కూడు_waitfds(&multi->cshutdn, multi->admin, &cwfds);

    ఒకవేళ(need != cwfds.n && ufds)
      mresult = CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;

    ఒకవేళ(fd_లెక్క)
      *fd_లెక్క = need;
    Curl_pollset_cleanup(&ps);
  }
బహిర్గతం:
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

#నిర్వచితమైతే USE_WINSOCK
/* Reset FD_WRITE for TCP sockets. Nothing is actually sent. UDP sockets cannot
 * be reset this way because an empty datagram would be sent. #9203
 *
 * "On Windows the internal state of FD_WRITE as returned from
 * WSAEnumNetworkEvents is only reset after successful send()."
 */
స్థిర శూన్య reset_socket_fdwrite(curl_socket_t s)
{
  పూర్ణాంకం t;
  పూర్ణాంకం l = (పూర్ణాంకం)పరిమాణం(t);
  ఒకవేళ(!getsockopt(s, SOL_SOCKET, SO_TYPE, (అక్షరం *)&t, &l) && t == SOCK_STREAM)
    swrite(s, ఖాళీ, 0);
}

స్థిర CURLMcode multi_winsock_select(నిర్మాణం Curl_multi *multi,
                                      నిర్మాణం curl_pollfds *cpfds,
                                      గుర్తులేని పూర్ణాంకం curl_nfds,
                                      నిర్మాణం curl_waitfd extra_fds[],
                                      గుర్తులేని పూర్ణాంకం extra_nfds,
                                      పూర్ణాంకం timeout_ms,
                                      bool extrawait,
                                      పూర్ణాంకం *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  WSANETWORKEVENTS wsa_events;
  పూర్ణాంకం nevents = 0;
  size_t i;

  DEBUGASSERT(multi->wsa_event != WSA_INVALID_EVENT);

  /* Set the WSA events based on the collected pollds */
  కొరకు(i = 0; i < cpfds->n; i++) {
    దీర్ఘ ముసుగు = 0;
    ఒకవేళ(cpfds->pfds[i].events & POLLIN)
      ముసుగు |= FD_READ | FD_ACCEPT | FD_CLOSE;
    ఒకవేళ(cpfds->pfds[i].events & POLLPRI)
      ముసుగు |= FD_OOB;
    ఒకవేళ(cpfds->pfds[i].events & POLLOUT) {
      ముసుగు |= FD_WRITE | FD_CONNECT | FD_CLOSE;
      reset_socket_fdwrite(cpfds->pfds[i].fd);
    }
    ఒకవేళ(ముసుగు && WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, ముసుగు)) {
      mresult = CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఒకవేళ(cpfds->n || extrawait) {
    పూర్ణాంకం pollrc = 0;

    ఒకవేళ(cpfds->n) {         /* pre-check with Winsock */
      pollrc = Curl_poll(cpfds->pfds, cpfds->n, 0);
      ఒకవేళ(pollrc < 0) {
        mresult = CURLM_UNRECOVERABLE_POLL;
        వెళ్ళు బహిర్గతం;
      }
      nevents = pollrc;
    }

    ఒకవేళ(!nevents) {
      /* now wait... if not ready during the pre-check (pollrc == 0) */
      WSAWait_For_గుణిజం_Events(1, &multi->wsa_event, FALSE, (DWORD)timeout_ms,
                               FALSE);
    }

    /* With Winsock, we have to run the following section unconditionally
       to call WSAEventSelect(fd, event, 0) on all the sockets */
    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    కొరకు(i = 0; i < extra_nfds; i++) {
      గుర్తులేని పొట్టి ముసుగు = 0;
      curl_socket_t s = extra_fds[i].fd;

      wsa_events.l_జాలం_Events = 0;
      ఒకవేళ(WSAEnum_జాలం_Events(s, ఖాళీ, &wsa_events) == 0) {
        ఒకవేళ(wsa_events.l_జాలం_Events & (FD_READ | FD_ACCEPT | FD_CLOSE))
          ముసుగు |= CURL_WAIT_POLLIN;
        ఒకవేళ(wsa_events.l_జాలం_Events & (FD_WRITE | FD_CONNECT | FD_CLOSE))
          ముసుగు |= CURL_WAIT_POLLOUT;
        ఒకవేళ(wsa_events.l_జాలం_Events & FD_OOB)
          ముసుగు |= CURL_WAIT_POLLPRI;
        ఒకవేళ(!pollrc && wsa_events.l_జాలం_Events)
          nevents++;
      }
      WSAEventSelect(s, multi->wsa_event, 0);
      ఒకవేళ(!pollrc) {
        extra_fds[i].revents = (పొట్టి)ముసుగు;
        కొనసాగు;
      }
      లేకపోతే {
        గుర్తులేని r = (గుర్తులేని)cpfds->pfds[curl_nfds + i].revents;
        ఒకవేళ(r & POLLIN)
          ముసుగు |= CURL_WAIT_POLLIN;
        ఒకవేళ(r & POLLOUT)
          ముసుగు |= CURL_WAIT_POLLOUT;
        ఒకవేళ(r & POLLPRI)
          ముసుగు |= CURL_WAIT_POLLPRI;
        extra_fds[i].revents = (పొట్టి)ముసుగు;
      }
    }

    /* Count up all our own sockets that had activity,
       and remove them from the event. */
    కొరకు(i = 0; i < curl_nfds; ++i) {
      wsa_events.l_జాలం_Events = 0;
      ఒకవేళ(WSAEnum_జాలం_Events(cpfds->pfds[i].fd, ఖాళీ, &wsa_events) == 0) {
        ఒకవేళ(!pollrc && wsa_events.l_జాలం_Events)
          nevents++;
      }
      WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, 0);
    }
    WSAResetEvent(multi->wsa_event);
  }

బహిర్గతం:
  *pnevents = nevents;
  తిరిగి mresult;
}

#లేకపోతే /* USE_WINSOCK */

స్థిర CURLMcode multi_posix_poll(నిర్మాణం Curl_multi *multi,
                                  నిర్మాణం curl_pollfds *cpfds,
                                  గుర్తులేని పూర్ణాంకం curl_nfds,
                                  నిర్మాణం curl_waitfd extra_fds[],
                                  గుర్తులేని పూర్ణాంకం extra_nfds,
                                  పూర్ణాంకం timeout_ms,
                                  bool extrawait,
                                  పూర్ణాంకం *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  పూర్ణాంకం nevents = 0;
  size_t i;

  (శూన్య)multi;
  ఒకవేళ(cpfds->n) {
    పూర్ణాంకం pollrc = Curl_poll(cpfds->pfds, cpfds->n, timeout_ms); /* wait... */
    ఒకవేళ(pollrc < 0) {
      mresult = CURLM_UNRECOVERABLE_POLL;
      వెళ్ళు బహిర్గతం;
    }
    nevents = pollrc;

    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    కొరకు(i = 0; i < extra_nfds; i++) {
      గుర్తులేని r = (గుర్తులేని)cpfds->pfds[curl_nfds + i].revents;
      గుర్తులేని పొట్టి ముసుగు = 0;
      ఒకవేళ(r & POLLIN)
        ముసుగు |= CURL_WAIT_POLLIN;
      ఒకవేళ(r & POLLOUT)
        ముసుగు |= CURL_WAIT_POLLOUT;
      ఒకవేళ(r & POLLPRI)
        ముసుగు |= CURL_WAIT_POLLPRI;
      extra_fds[i].revents = (పొట్టి)ముసుగు;
    }
  }
  లేకపోతే ఒకవేళ(extrawait) {
    /* No fds to poll, but asked to obey timeout_ms anyway. We cannot
     * use Curl_poll() as it, on some platforms, returns immediately
     * without fds. */
    curlx_wait_ms(timeout_ms);
  }

బహిర్గతం:
  *pnevents = nevents;
  తిరిగి mresult;
}

#అయితే_ముగింపు /* !USE_WINSOCK */

#నిర్వచించు అంకె_POLLS_ON_దొంతర 10

స్థిర CURLMcode multi_wait(నిర్మాణం Curl_multi *multi,
                            నిర్మాణం curl_waitfd extra_fds[],
                            గుర్తులేని పూర్ణాంకం extra_nfds,
                            పూర్ణాంకం timeout_ms,
                            పూర్ణాంకం *ret,
                            bool extrawait)  /* when no socket, wait */
{
  size_t i;
  పూర్ణాంకం timeout_అంతర్గత;
  పూర్ణాంకం nevents = 0;
  నిర్మాణం easy_pollset ps;
  నిర్మాణం pollfd a_few_on_దొంతర[అంకె_POLLS_ON_దొంతర];
  నిర్మాణం curl_pollfds cpfds;
  గుర్తులేని పూర్ణాంకం curl_nfds = 0; /* how many pfds are for curl transfers */
  నిర్మాణం Curl_easy *దత్తాంశం = ఖాళీ;
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;
#నిర్వచితమైతే ENABLE_WAKEUP
  పూర్ణాంకం wakeup_idx = -1;
#అయితే_ముగింపు

  ఒకవేళ(timeout_ms < 0)
    తిరిగి CURLM_BAD_FUNCTION_వాదం;

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_దొంతర, అంకె_POLLS_ON_దొంతర);

  /* Add the curl handles to our pollfds first */
  ఒకవేళ(Curl_uint32_bset_మొదటి(&multi->process, &mid)) {
    చేయి {
      దత్తాంశం = Curl_multi_get_easy(multi, mid);
      ఒకవేళ(!దత్తాంశం) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        కొనసాగు;
      }
      Curl_multi_pollset(దత్తాంశం, &ps);
      ఒకవేళ(Curl_pollfds_కూడు_ps(&cpfds, &ps)) {
        mresult = CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
    } అయితే(Curl_uint32_bset_తదుపరి(&multi->process, mid, &mid));
  }

  ఒకవేళ(Curl_cshutdn_కూడు_pollfds(&multi->cshutdn, multi->admin, &cpfds)) {
    mresult = CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

#నిర్వచితమైతే ENABLE_WAKEUP
  /* If `extrawait` is TRUE *or* we have `extra_fds`to poll *or* we
   * have transfer sockets to poll, we obey `timeout_ms`.
   * Then we need to also monitor the multi's wakeup
   * socket to catch calls to `curl_multi_wakeup()` during the wait. */
  ఒకవేళ(extrawait || cpfds.n || extra_nfds) {
    wakeup_idx = cpfds.n;
    ఒకవేళ(Curl_pollfds_కూడు_sock(&cpfds, multi->wakeup_pair[0], POLLIN)) {
      mresult = CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }
#అయితే_ముగింపు

  curl_nfds = cpfds.n; /* what curl internally uses in cpfds */
  /* Add external file descriptions from poll-like struct curl_waitfd */
  కొరకు(i = 0; i < extra_nfds; i++) {
    గుర్తులేని పొట్టి events = 0;
    ఒకవేళ(extra_fds[i].events & CURL_WAIT_POLLIN)
      events |= POLLIN;
    ఒకవేళ(extra_fds[i].events & CURL_WAIT_POLLPRI)
      events |= POLLPRI;
    ఒకవేళ(extra_fds[i].events & CURL_WAIT_POLLOUT)
      events |= POLLOUT;
    ఒకవేళ(Curl_pollfds_కూడు_sock(&cpfds, extra_fds[i].fd, events)) {
      mresult = CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }

  /* We check the internal timeout *AFTER* we collected all sockets to
   * poll. Collecting the sockets may install new timers by protocols
   * and connection filters.
   * Use the shorter one of the internal and the caller requested timeout.
   * If we are called with `!extrawait` and multi_timeout() reports no
   * timeouts exist, do not wait. */
  multi_timeout(multi, ఖాళీ, &timeout_అంతర్గత);
  ఒకవేళ((timeout_అంతర్గత >= 0) && (timeout_అంతర్గత < timeout_ms))
    timeout_ms = timeout_అంతర్గత;

  ఒకవేళ(దత్తాంశం)
    CURL_TRC_M(దత్తాంశం, "multi_wait(fds=%u, timeout=%d) tinternal=%d",
               cpfds.n, timeout_ms, timeout_అంతర్గత);

#నిర్వచితమైతే USE_WINSOCK
  mresult = multi_winsock_select(multi, &cpfds, curl_nfds,
                                 extra_fds, extra_nfds,
                                 timeout_ms, extrawait, &nevents);
#లేకపోతే
  mresult = multi_posix_poll(multi, &cpfds, curl_nfds,
                             extra_fds, extra_nfds,
                             timeout_ms, extrawait, &nevents);
#అయితే_ముగింపు

#నిర్వచితమైతే ENABLE_WAKEUP
    ఒకవేళ(nevents && (wakeup_idx >= 0)) {
      ఒకవేళ(cpfds.pfds[wakeup_idx].revents & POLLIN) {
        (శూన్య)Curl_wakeup_consume(multi->wakeup_pair, TRUE);
        /* do not count the wakeup socket into the returned value */
        nevents--;
      }
    }
#అయితే_ముగింపు

బహిర్గతం:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  ఒకవేళ(ret)
    *ret = nevents;
  తిరిగి mresult;
}

CURLMcode curl_multi_wait(CURLM *m,
                          నిర్మాణం curl_waitfd extra_fds[],
                          గుర్తులేని పూర్ణాంకం extra_nfds,
                          పూర్ణాంకం timeout_ms,
                          పూర్ణాంకం *ret)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_wait, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, FALSE);
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

CURLMcode curl_multi_poll(CURLM *m,
                          నిర్మాణం curl_waitfd extra_fds[],
                          గుర్తులేని పూర్ణాంకం extra_nfds,
                          పూర్ణాంకం timeout_ms,
                          పూర్ణాంకం *ret)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_poll, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, TRUE);
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

CURLMcode curl_multi_wakeup(CURLM *m)
{
  /* this function is usually called from another thread,
     it has to be careful only to access parts of the
     Curl_multi struct that are constant */
  నిర్మాణం Curl_multi *multi = m;
  CURLMcode mresult = CURLM_WAKEUP_FAILURE;

  /* GOOD_MULTI_HANDLE can be safely called */
  ఒకవేళ(!GOOD_MULTI_HANDLE(multi))
    తిరిగి CURLM_BAD_HANDLE;

#నిర్వచితమైతే ENABLE_WAKEUP
  /* the wakeup_pair variable is only written during init and cleanup,
     making it safe to access from another thread after the init part
     and before cleanup */
  ఒకవేళ(!Curl_wakeup_signal(multi->wakeup_pair))
    mresult = CURLM_OK;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_WINSOCK
  ఒకవేళ(WSASetEvent(multi->wsa_event))
    mresult = CURLM_OK;
#అయితే_ముగింపు
  తిరిగి mresult;
}

#నిర్వచితమైతే ENABLE_అంతర్గత_WAKEUP
శూన్య Curl_multi_wakeup_అంతర్గత(నిర్మాణం Curl_multi *multi)
{
  /* This is expected to be invocable from another thread which
   * does NOT outlive the multi handle. Check for sanity. */
  ఒకవేళ(GOOD_MULTI_HANDLE(multi))
    Curl_wakeup_signal(multi->wakeup_అంతర్గత);
  లేకపోతే
    DEBUGASSERT(0);
}
#అయితే_ముగింపు

/*
 * multi_ischanged() is called
 *
 * Returns TRUE/FALSE whether the state is changed to trigger a CONNECT_PEND
 * => CONNECT action.
 *
 * Set 'clear' to TRUE to have it also clear the state variable.
 */
స్థిర bool multi_ischanged(నిర్మాణం Curl_multi *multi, bool clear)
{
  bool retval = FALSE;
  DEBUGASSERT(multi);
  ఒకవేళ(multi) {
    retval = (bool)multi->recheckstate;
    ఒకవేళ(clear)
      multi->recheckstate = FALSE;
  }
  తిరిగి retval;
}

/*
 * Curl_multi_connchanged() is called to tell that there is a connection in
 * this multi handle that has changed state (multiplexing become possible, the
 * number of allowed streams changed or similar), and a subsequent use of this
 * multi handle should move CONNECT_PEND handles back to CONNECT to have them
 * retry.
 */
శూన్య Curl_multi_connchanged(నిర్మాణం Curl_multi *multi)
{
  /* May be invoked for a `data->multi` when no multi is present. */
  ఒకవేళ(multi)
    multi->recheckstate = TRUE;
}

CURLMcode Curl_multi_కూడు_perform(నిర్మాణం Curl_multi *multi,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం connectdata *conn)
{
  CURLMcode mresult;

  mresult = Curl_multi_కూడు_handle(multi, దత్తాంశం);
  ఒకవేళ(!mresult) {
    CURLcode ఫలితం;

    /* pass in NULL for 'conn' here since we do not want to init the
       connection, only this transfer */
    ఫలితం = Curl_init_transfer(దత్తాంశం, ఖాళీ);
    ఒకవేళ(ఫలితం) {
      Curl_multi_remove_handle(multi, దత్తాంశం);
      తిరిగి CURLM_అంతర్గత_ERROR;
    }

    /* take this handle to the perform state right away */
    multistate(దత్తాంశం, MSTATE_PERFORMING);
    Curl_attach_connection(దత్తాంశం, conn, TRUE);
    CURL_REQ_సమితి_RECV(దత్తాంశం);
  }
  తిరిగి mresult;
}

స్థిర CURLcode multi_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(conn->scheme);

  ఒకవేళ(conn->scheme->నిరంతరం->do_it)
    ఫలితం = conn->scheme->నిరంతరం->do_it(దత్తాంశం, పూర్తి);

  తిరిగి ఫలితం;
}

/*
 * multi_do_more() is called during the DO_MORE multi state. It is a second
 * stage DO state which (wrongly) was introduced to support FTP's second
 * connection.
 *
 * 'complete' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (to DOING state when there is more work to do)
 */

స్థిర CURLcode multi_do_మరిన్ని(నిర్మాణం Curl_easy *దత్తాంశం, domore *complete)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  *complete = DOMORE_INCOMPLETE;

  ఒకవేళ(conn->scheme->నిరంతరం->do_మరిన్ని)
    ఫలితం = conn->scheme->నిరంతరం->do_మరిన్ని(దత్తాంశం, complete);

  తిరిగి ఫలితం;
}

/*
 * Check whether a timeout occurred, and handle it if it did
 */
స్థిర bool multi_handle_timeout(నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి నిర్మాణం curltime *pnow,
                                 bool *ప్రవాహం_error,
                                 CURLcode *ఫలితం)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_timeleft_now_ms(దత్తాంశం, pnow);
  ఒకవేళ(timeout_ms < 0) {
    /* Handle timed out */
    timerid పునాది_timer = Curl_is_connecting(దత్తాంశం) ?
                         TIMER_STARTSINGLE : TIMER_STARTOP;
    timediff_t elapsed_ms = Curl_pgrs_since_ms(దత్తాంశం, ఖాళీ, పునాది_timer);
    ఒకవేళ(దత్తాంశం->mstate == MSTATE_CONNECTING) {
      failf(దత్తాంశం, "%s timed out after %" FMT_TIMEDIFF_T " milliseconds",
            దత్తాంశం->conn->బిట్లు.dns_resolved ? "Connection" : "Resolving",
            elapsed_ms);
      /* keep what we know about a partly done connect, e.g. TCP done
       * but TLS handshake still ongoing */
      Curl_conn_report_failed(దత్తాంశం, FIRSTSOCKET);
    }
    లేకపోతే {
      నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
      ఒకవేళ(k->కొలత != -1) {
        failf(దత్తాంశం, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount, k->కొలత);
      }
      లేకపోతే {
        failf(దత్తాంశం, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount);
      }
    }
    *ఫలితం = CURLE_OPERATION_TIMEDOUT;
    ఒకవేళ(దత్తాంశం->conn) {
      /* Force connection closed if the connection has indeed been used */
      ఒకవేళ(దత్తాంశం->mstate > MSTATE_DO) {
        streamclose(దత్తాంశం->conn);
        *ప్రవాహం_error = TRUE;
      }
      (శూన్య)multi_పూర్తి(దత్తాంశం, *ఫలితం, TRUE);
    }
    తిరిగి TRUE;
  }

  తిరిగి FALSE;
}

/*
 * We are doing protocol-specific connecting and this is being called over and
 * over from the multi interface until the connection phase is done on
 * protocol layer.
 */

స్థిర CURLcode protocol_connecting(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  ఒకవేళ(conn && conn->scheme->నిరంతరం->connecting) {
    *పూర్తి = FALSE;
    ఫలితం = conn->scheme->నిరంతరం->connecting(దత్తాంశం, పూర్తి);
  }
  లేకపోతే
    *పూర్తి = TRUE;

  తిరిగి ఫలితం;
}

/*
 * We are DOING this is being called over and over from the multi interface
 * until the DOING phase is done on protocol layer.
 */

స్థిర CURLcode protocol_doing(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  ఒకవేళ(conn && conn->scheme->నిరంతరం->doing) {
    *పూర్తి = FALSE;
    ఫలితం = conn->scheme->నిరంతరం->doing(దత్తాంశం, పూర్తి);
  }
  లేకపోతే
    *పూర్తి = TRUE;

  తిరిగి ఫలితం;
}

/*
 * We have discovered that the TCP connection has been successful, we can now
 * proceed with some action.
 *
 */
స్థిర CURLcode protocol_connect(నిర్మాణం Curl_easy *దత్తాంశం, bool *protocol_పూర్తి)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(conn);
  DEBUGASSERT(protocol_పూర్తి);
  DEBUGASSERT(Curl_conn_is_connected(conn, FIRSTSOCKET));

  *protocol_పూర్తి = FALSE;
  ఒకవేళ(!conn->బిట్లు.protoconnstart) {
    ఒకవేళ(conn->scheme->నిరంతరం->connect_it) {
      /* Call the protocol-specific connect function */
      ఫలితం = conn->scheme->నిరంతరం->connect_it(దత్తాంశం, protocol_పూర్తి);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    conn->బిట్లు.protoconnstart = TRUE;
  }

  /* Unless this protocol does not have any protocol-connect callback, as
     then we know we are done. */
  ఒకవేళ(!conn->scheme->నిరంతరం->connecting)
    *protocol_పూర్తి = TRUE;
  తిరిగి CURLE_OK;
}

/*
 * posttransfer() is called immediately after a transfer ends
 */
స్థిర శూన్య multi_posttransfer(నిర్మాణం Curl_easy *దత్తాంశం)
{
#ఒకవేళ defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
  /* restore the signal handler for SIGPIPE before we get back */
  ఒకవేళ(!దత్తాంశం->సమితి.no_signal)
    సంకేతం(SIGPIPE, దత్తాంశం->state.prev_signal);
#లేకపోతే
  (శూన్య)దత్తాంశం;
#అయితే_ముగింపు
}

/*
 * multi_follow() handles the URL redirect magic. Pass in the 'newurl' string
 * as given by the remote server and set up the new URL to request.
 *
 * This function DOES NOT FREE the given URL.
 */
స్థిర CURLcode multi_follow(నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి నిర్మాణం Curl_scheme *handler,
                             స్థిరరాశి అక్షరం *newurl, /* the Location: string */
                             followtype type) /* see transfer.h */
{
  ఒకవేళ(handler && handler->నిరంతరం->follow)
    తిరిగి handler->నిరంతరం->follow(దత్తాంశం, newurl, type);

  ఒకవేళ(type == FOLLOW_RETRY)
    /* Retries are generic and do not require protocol-specific redirect
       handling. */
    తిరిగి CURLE_OK;

  తిరిగి CURLE_TOO_MANY_REDIRECTS;
}

స్థిర CURLcode mspeed_check(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(Curl_rlimit_చురుకైన(&దత్తాంశం->పురోగతి.dl.rlimit) ||
     Curl_rlimit_చురుకైన(&దత్తాంశం->పురోగతి.ul.rlimit)) {
    /* check if our send/recv limits require idle waits */
    స్థిరరాశి నిర్మాణం curltime *pnow = Curl_pgrs_now(దత్తాంశం);
    timediff_t recv_ms, send_ms;

    send_ms = Curl_rlimit_wait_ms(&దత్తాంశం->పురోగతి.ul.rlimit, pnow);
    recv_ms = Curl_rlimit_wait_ms(&దత్తాంశం->పురోగతి.dl.rlimit, pnow);

    ఒకవేళ(send_ms || recv_ms) {
      ఒకవేళ(దత్తాంశం->mstate != MSTATE_RATELIMITING) {
        multistate(దత్తాంశం, MSTATE_RATELIMITING);
      }
      Curl_expire_సమితి(దత్తాంశం, EXPIRE_TOOFAST, CURLMAX(send_ms, recv_ms), pnow);
      Curl_multi_clear_dirty(దత్తాంశం);
      CURL_TRC_M(దత్తాంశం, "[RLIMIT] waiting %" FMT_TIMEDIFF_T "ms",
                 CURLMAX(send_ms, recv_ms));
      తిరిగి CURLE_AGAIN;
    }
    లేకపోతే {
      /* when will the rate limits increase next? The transfer needs
       * to run again at that time or it may stall. */
      send_ms = Curl_rlimit_తదుపరి_అడుగు_ms(&దత్తాంశం->పురోగతి.ul.rlimit, pnow);
      recv_ms = Curl_rlimit_తదుపరి_అడుగు_ms(&దత్తాంశం->పురోగతి.dl.rlimit, pnow);
      ఒకవేళ(send_ms || recv_ms) {
        timediff_t తదుపరి_ms = CURLMIN(send_ms, recv_ms);
        ఒకవేళ(!తదుపరి_ms)
          తదుపరి_ms = CURLMAX(send_ms, recv_ms);
        Curl_expire_సమితి(దత్తాంశం, EXPIRE_TOOFAST, తదుపరి_ms, pnow);
        CURL_TRC_M(దత్తాంశం, "[RLIMIT] next token update in %" FMT_TIMEDIFF_T "ms",
                   తదుపరి_ms);
      }
    }
  }

  ఒకవేళ(దత్తాంశం->mstate != MSTATE_PERFORMING) {
    CURL_TRC_M(దత్తాంశం, "[RLIMIT] wait over, continue");
    multistate(దత్తాంశం, MSTATE_PERFORMING);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLMcode multistate_performing(నిర్మాణం Curl_easy *దత్తాంశం,
                                       bool *ప్రవాహం_errorp,
                                       CURLcode *resultp)
{
  అక్షరం *newurl = ఖాళీ;
  bool retry = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode ఫలితం = *resultp = CURLE_OK;
  *ప్రవాహం_errorp = FALSE;

  ఒకవేళ(mspeed_check(దత్తాంశం) == CURLE_AGAIN)
    తిరిగి CURLM_OK;

  /* read/write data if it is ready to do so */
  ఫలితం = Curl_sendrecv(దత్తాంశం);

  ఒకవేళ(దత్తాంశం->req.పూర్తి || (ఫలితం == CURLE_RECV_ERROR)) {
    /* If CURLE_RECV_ERROR happens early enough, we assume it was a race
     * condition and the server closed the reused connection exactly when we
     * wanted to use it, so figure out if that is indeed the case.
     */
    CURLcode ret = Curl_retry_request(దత్తాంశం, &newurl);
    ఒకవేళ(!ret)
      retry = !!newurl;
    లేకపోతే ఒకవేళ(!ఫలితం)
      ఫలితం = ret;

    ఒకవేళ(retry) {
      /* if we are to retry, set the result to OK and consider the
         request as done */
      ఫలితం = CURLE_OK;
      దత్తాంశం->req.పూర్తి = TRUE;
    }
  }
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  లేకపోతే ఒకవేళ((ఫలితం == CURLE_HTTP2_ప్రవాహం) &&
          Curl_h2_http_1_1_error(దత్తాంశం)) {
    CURLcode ret = Curl_retry_request(దత్తాంశం, &newurl);

    ఒకవేళ(!ret) {
      infof(దత్తాంశం, "Downgrades to HTTP/1.1");
      streamclose(దత్తాంశం->conn);
      దత్తాంశం->state.http_neg.wanted = CURL_HTTP_V1x;
      దత్తాంశం->state.http_neg.allowed = CURL_HTTP_V1x;
      /* clear the error message bit too as we ignore the one we got */
      దత్తాంశం->state.errorbuf = FALSE;
      ఒకవేళ(!newurl)
        /* typically for HTTP_1_1_REQUIRED error on first flight */
        newurl = Curl_bufref_dup(&దత్తాంశం->state.url);
      ఒకవేళ(!newurl) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      లేకపోతే {
        /* if we are to retry, set the result to OK and consider the request
          as done */
        retry = TRUE;
        ఫలితం = CURLE_OK;
        దత్తాంశం->req.పూర్తి = TRUE;
      }
    }
    లేకపోతే
      ఫలితం = ret;
  }
#అయితే_ముగింపు

  ఒకవేళ(ఫలితం) {
    /*
     * The transfer phase returned error, we mark the connection to get closed
     * to prevent being reused. This is because we cannot possibly know if the
     * connection is in a good shape or not now. Unless it is a protocol which
     * uses two "channels" like FTP, as then the error happened in the data
     * connection.
     */

    ఒకవేళ(!(దత్తాంశం->conn->scheme->flags & PROTOPT_DUAL) &&
       ఫలితం != CURLE_HTTP2_ప్రవాహం)
      streamclose(దత్తాంశం->conn);

    multi_posttransfer(దత్తాంశం);
    multi_పూర్తి(దత్తాంశం, ఫలితం, TRUE);
  }
  లేకపోతే ఒకవేళ(దత్తాంశం->req.పూర్తి && !Curl_cwriter_is_paused(దత్తాంశం)) {
    స్థిరరాశి నిర్మాణం Curl_scheme *handler = దత్తాంశం->conn->scheme;

    /* call this even if the readwrite function returned error */
    multi_posttransfer(దత్తాంశం);

    /* When we follow redirects or is set to retry the connection, we must to
       go back to the CONNECT state */
    ఒకవేళ(దత్తాంశం->req.newurl || retry) {
      followtype follow = FOLLOW_NONE;
      ఒకవేళ(!retry) {
        /* if the URL is a follow-location and not a retried request then
           figure out the URL here */
        curlx_free(newurl);
        newurl = దత్తాంశం->req.newurl;
        దత్తాంశం->req.newurl = ఖాళీ;
        follow = FOLLOW_REDIR;
      }
      లేకపోతే
        follow = FOLLOW_RETRY;
      (శూన్య)multi_పూర్తి(దత్తాంశం, CURLE_OK, FALSE);
      /* multi_done() might return CURLE_GOT_NOTHING */
      ఫలితం = multi_follow(దత్తాంశం, handler, newurl, follow);
      ఒకవేళ(!ఫలితం) {
        multistate(దత్తాంశం, MSTATE_అమరిక);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    లేకపోతే {
      /* after the transfer is done, go DONE */

      /* but first check to see if we got a location info even though we are
         not following redirects */
      ఒకవేళ(దత్తాంశం->req.location) {
        curlx_free(newurl);
        newurl = దత్తాంశం->req.location;
        దత్తాంశం->req.location = ఖాళీ;
        ఫలితం = multi_follow(దత్తాంశం, handler, newurl, FOLLOW_FAKE);
        ఒకవేళ(ఫలితం) {
          *ప్రవాహం_errorp = TRUE;
          ఫలితం = multi_పూర్తి(దత్తాంశం, ఫలితం, TRUE);
        }
      }

      ఒకవేళ(!ఫలితం) {
        multistate(దత్తాంశం, MSTATE_పూర్తి);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
  }
  లేకపోతే { /* not errored, not done */
    mspeed_check(దత్తాంశం);
  }
  curlx_free(newurl);
  *resultp = ఫలితం;
  తిరిగి mresult;
}

స్థిర CURLMcode multistate_do(నిర్మాణం Curl_easy *దత్తాంశం,
                               bool *ప్రవాహం_errorp,
                               CURLcode *resultp)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode ఫలితం = CURLE_OK;
  ఒకవేళ(దత్తాంశం->సమితి.fprereq) {
    నిర్మాణం Curl_mapi_guard guard;
    పూర్ణాంకం prereq_rc;

    /* call the prerequest callback function */
    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_fprereq);
    prereq_rc = దత్తాంశం->సమితి.fprereq(దత్తాంశం->సమితి.prereq_userp,
                                  దత్తాంశం->info.primary.remote_ip,
                                  దత్తాంశం->info.primary.local_ip,
                                  దత్తాంశం->info.primary.remote_పోర్టు,
                                  దత్తాంశం->info.primary.local_పోర్టు);
    CURL_CBAPI_చివర(&guard);
    ఒకవేళ(prereq_rc != CURL_PREREQFUNC_OK) {
      failf(దత్తాంశం, "operation aborted by pre-request callback");
      /* failure in pre-request callback - do not do any other processing */
      ఫలితం = CURLE_ABORTED_BY_CALLBACK;
      multi_posttransfer(దత్తాంశం);
      multi_పూర్తి(దత్తాంశం, ఫలితం, FALSE);
      *ప్రవాహం_errorp = TRUE;
      వెళ్ళు చివర;
    }
  }

  ఒకవేళ(దత్తాంశం->సమితి.connect_only && !దత్తాంశం->సమితి.connect_only_ws) {
    multistate(దత్తాంశం, MSTATE_పూర్తి);
    mresult = CURLM_CALL_MULTI_PERFORM;
  }
  లేకపోతే {
    bool dophase_పూర్తి = FALSE;
    /* Perform the protocol's DO action */
    ఫలితం = multi_do(దత్తాంశం, &dophase_పూర్తి);

    ఒకవేళ(!ఫలితం) {
      ఒకవేళ(!dophase_పూర్తి) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
        /* some steps needed for wildcard matching */
        ఒకవేళ(దత్తాంశం->state.wildcardmatch) {
          నిర్మాణం Wildcard_దత్తాంశం *wc = దత్తాంశం->wildcard;
          ఒకవేళ(wc->state == CURLWC_పూర్తి || wc->state == CURLWC_SKIP) {
            /* skip some states if it is important */
            multi_పూర్తి(దత్తాంశం, CURLE_OK, FALSE);

            /* if there is no connection left, skip the DONE state */
            multistate(దత్తాంశం, దత్తాంశం->conn ? MSTATE_పూర్తి : MSTATE_COMPLETED);
            mresult = CURLM_CALL_MULTI_PERFORM;
            వెళ్ళు చివర;
          }
        }
#అయితే_ముగింపు
        /* DO was not completed in one function call, we must continue
           DOING... */
        multistate(దత్తాంశం, MSTATE_DOING);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }

      /* after DO, go DO_DONE... or DO_MORE */
      లేకపోతే ఒకవేళ(దత్తాంశం->conn->బిట్లు.do_మరిన్ని) {
        /* we are supposed to do more, but we need to sit down, relax and wait
           a little while first */
        multistate(దత్తాంశం, MSTATE_DOING_మరిన్ని);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
      లేకపోతే {
        /* we are done with the DO, now DID */
        multistate(దత్తాంశం, MSTATE_DID);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    లేకపోతే ఒకవేళ((ఫలితం == CURLE_SEND_ERROR) &&
            దత్తాంశం->conn->బిట్లు.reuse) {
      /*
       * In this situation, a connection that we were trying to use may have
       * unexpectedly died. If possible, send the connection back to the
       * CONNECT phase so we can try again.
       */
      స్థిరరాశి నిర్మాణం Curl_scheme *handler = దత్తాంశం->conn->scheme;
      అక్షరం *newurl = ఖాళీ;
      followtype follow = FOLLOW_NONE;
      CURLcode drc;

      drc = Curl_retry_request(దత్తాంశం, &newurl);
      ఒకవేళ(drc) {
        /* a failure here pretty much implies an out of memory */
        ఫలితం = drc;
        *ప్రవాహం_errorp = TRUE;
      }

      multi_posttransfer(దత్తాంశం);
      drc = multi_పూర్తి(దత్తాంశం, ఫలితం, FALSE);

      /* When set to retry the connection, we must go back to the CONNECT
       * state */
      ఒకవేళ(newurl) {
        ఒకవేళ(!drc || (drc == CURLE_SEND_ERROR)) {
          follow = FOLLOW_RETRY;
          drc = multi_follow(దత్తాంశం, handler, newurl, follow);
          ఒకవేళ(!drc) {
            multistate(దత్తాంశం, MSTATE_అమరిక);
            mresult = CURLM_CALL_MULTI_PERFORM;
            ఫలితం = CURLE_OK;
          }
          లేకపోతే {
            /* Follow failed */
            ఫలితం = drc;
          }
        }
        లేకపోతే {
          /* done did not return OK or SEND_ERROR */
          ఫలితం = drc;
        }
      }
      లేకపోతే {
        /* Have error handler disconnect conn if we cannot retry */
        *ప్రవాహం_errorp = TRUE;
      }
      curlx_free(newurl);
    }
    లేకపోతే {
      /* failure detected */
      multi_posttransfer(దత్తాంశం);
      ఒకవేళ(దత్తాంశం->conn)
        multi_పూర్తి(దత్తాంశం, ఫలితం, FALSE);
      *ప్రవాహం_errorp = TRUE;
    }
  }
చివర:
  *resultp = ఫలితం;
  తిరిగి mresult;
}

స్థిర CURLMcode multistate_ratelimiting(నిర్మాణం Curl_easy *దత్తాంశం,
                                         CURLcode *resultp)
{
  CURLcode ఫలితం = CURLE_OK;
  CURLMcode mresult = CURLM_OK;
  DEBUGASSERT(దత్తాంశం->conn);
  /* if both rates are within spec, resume transfer */
  ఫలితం = Curl_pgrsCheck(దత్తాంశం);

  ఒకవేళ(ఫలితం) {
    ఒకవేళ(!(దత్తాంశం->conn->scheme->flags & PROTOPT_DUAL) &&
       ఫలితం != CURLE_HTTP2_ప్రవాహం)
      streamclose(దత్తాంశం->conn);

    multi_posttransfer(దత్తాంశం);
    multi_పూర్తి(దత్తాంశం, ఫలితం, TRUE);
  }
  లేకపోతే {
    ఒకవేళ(!mspeed_check(దత్తాంశం))
      mresult = CURLM_CALL_MULTI_PERFORM;
  }
  *resultp = ఫలితం;
  తిరిగి mresult;
}

స్థిర CURLMcode multistate_connect(నిర్మాణం Curl_multi *multi,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    CURLcode *resultp)
{
  /* Connect. We want to get a connection identifier filled in. This state can
     be entered from SETUP and from PENDING. */
  bool connected;
  CURLMcode mresult = CURLM_OK;
  CURLcode ఫలితం = Curl_connect(దత్తాంశం, &connected);
  ఒకవేళ(ఫలితం == CURLE_NO_CONNECTION_అందుబాటు) {
    /* There was no connection available. We will go to the pending state and
       wait for an available connection. */
    multistate(దత్తాంశం, MSTATE_PENDING);
    /* move from process to pending set */
    Curl_uint32_bset_remove(&multi->process, దత్తాంశం->mid);
    Curl_uint32_bset_remove(&multi->dirty, దత్తాంశం->mid);
    Curl_uint32_bset_కూడు(&multi->pending, దత్తాంశం->mid);
    *resultp = CURLE_OK;
    తిరిగి mresult;
  }
  లేకపోతే
    multi_కాలపట్టిక_pending(దత్తాంశం->multi);

  ఒకవేళ(!ఫలితం) {
    /* after the connect has been sent off, go WAITCONNECT unless the
       protocol connect is already done and we can go directly to WAITDO or
       DO! */
    mresult = CURLM_CALL_MULTI_PERFORM;

    ఒకవేళ(connected) {
      ఒకవేళ(!దత్తాంశం->conn->బిట్లు.reuse &&
         Curl_conn_is_multiplex(దత్తాంశం->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_కాలపట్టిక_pending(దత్తాంశం->multi);
      }
      multistate(దత్తాంశం, MSTATE_PROTOCONNECT);
    }
    లేకపోతే {
      multistate(దత్తాంశం, MSTATE_CONNECTING);
    }
  }
  *resultp = ఫలితం;
  తిరిగి mresult;
}

/* returns the possibly updated result */
స్థిర CURLcode is_ముగిసిన(నిర్మాణం Curl_multi *multi,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            స్థిరరాశి నిర్మాణం curltime *pnow,
                            bool ప్రవాహం_error,
                            CURLcode ఫలితం)
{
  ఒకవేళ(దత్తాంశం->mstate < MSTATE_COMPLETED) {
    ఒకవేళ(ఫలితం) {
      /*
       * If an error was returned, and we are not in completed state now,
       * then we go to completed and consider this transfer aborted.
       */

      /* No attempt to disconnect connections must be made before this -
         connection detach and termination happens only here */

      /* Check if we can move pending requests to send pipe */
      multi_కాలపట్టిక_pending(multi); /* connection */

      ఒకవేళ(దత్తాంశం->conn) {
        ఒకవేళ(ప్రవాహం_error) {
          /* Do not attempt to send data over a connection that timed out */
          bool dead_connection = ఫలితం == CURLE_OPERATION_TIMEDOUT;
          నిర్మాణం connectdata *conn = దత్తాంశం->conn;

          /* This is where we make sure that the conn pointer is reset.
             We do not have to do this in every case block above where a
             failure is detected */
          Curl_detach_connection(దత్తాంశం);
          Curl_conn_close(దత్తాంశం, conn, dead_connection);
        }
      }
      లేకపోతే ఒకవేళ(దత్తాంశం->mstate == MSTATE_CONNECT) {
        /* Curl_connect() failed */
        multi_posttransfer(దత్తాంశం);
        Curl_pgrsUpdate_nometer(దత్తాంశం);
      }

      multistate(దత్తాంశం, MSTATE_COMPLETED);
      తిరిగి ఫలితం;
    }
    /* if there is still a connection to use, call the progress function */
    లేకపోతే ఒకవేళ(దత్తాంశం->conn && Curl_conn_is_connected(దత్తాంశం->conn, FIRSTSOCKET)) {
      ఫలితం = Curl_pgrsUpdateX(దత్తాంశం, pnow);
      ఒకవేళ(ఫలితం) {
        /* aborted due to progress callback return code must close the
           connection */
        streamclose(దత్తాంశం->conn);

        /* if not yet in DONE state, go there. */
        ఒకవేళ(దత్తాంశం->mstate < MSTATE_పూర్తి)
          multistate(దత్తాంశం, MSTATE_పూర్తి);
        తిరిగి ఫలితం;
      }
    }
  }
  తిరిగి ఫలితం;
}

స్థిర శూన్య handle_completed(నిర్మాణం Curl_multi *multi,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             CURLcode ఫలితం)
{
  bool సందేశం_గమ్యం_app = దత్తాంశం->master_mid == UINT32_MAX;

  ఒకవేళ(!సందేశం_గమ్యం_app) {
    /* A sub transfer, not reported to the application. Is anyone still
     * interested in processing its results? */
    ఒకవేళ(దత్తాంశం->sub_xfer_పూర్తి) {
      నిర్మాణం Curl_easy *master = Curl_multi_get_easy(multi, దత్తాంశం->master_mid);

      CURL_TRC_M(దత్తాంశం, "sub xfer done for master %u", దత్తాంశం->master_mid);
      ఒకవేళ(master)
        దత్తాంశం->sub_xfer_పూర్తి(దత్తాంశం, master, ఫలితం);
      లేకపోతే
        CURL_TRC_M(దత్తాంశం, "master easy %u already gone.", దత్తాంశం->master_mid);
    }
  }
  లేకపోతే {
    /* now fill in the CURLMsg with this info */
    నిర్మాణం CURLMsg *సందేశం = &దత్తాంశం->సందేశం;

    సందేశం->సందేశం = CURLMSG_పూర్తి;
    సందేశం->easy_handle = దత్తాంశం;
    సందేశం->దత్తాంశం.ఫలితం = ఫలితం;

    DEBUGASSERT(!దత్తాంశం->conn);
  }
  multistate(దత్తాంశం, MSTATE_MSGSENT);

  /* remove from the other sets */
  Curl_uint32_bset_remove(&multi->process, దత్తాంశం->mid);
  Curl_uint32_bset_remove(&multi->dirty, దత్తాంశం->mid);
  Curl_uint32_bset_remove(&multi->pending, దత్తాంశం->mid);
  ఒకవేళ(సందేశం_గమ్యం_app)
    multi_addmsg(multi, దత్తాంశం);
  ఒకవేళ(దత్తాంశం->state.really_alive) {
    దత్తాంశం->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    ఒకవేళ(!multi->xfers_really_alive)
      (శూన్య)multi_assess_wakeup(multi);
  }
  --multi->xfers_alive;
  ఒకవేళ(!multi->xfers_alive)
    multi_assess_wakeup(multi);
}

స్థిర CURLMcode multistate_init(నిర్మాణం Curl_easy *దత్తాంశం, CURLcode *ఫలితం)
{
  ఒకవేళ(!దత్తాంశం->state.really_alive) {
    దత్తాంశం->state.really_alive = TRUE;
    ++దత్తాంశం->multi->xfers_really_alive;
    ఒకవేళ(దత్తాంశం->multi->xfers_really_alive == 1) {
      CURLMcode mresult = multi_assess_wakeup(దత్తాంశం->multi);
      ఒకవేళ(mresult) {
        failf(దత్తాంశం, "error enabling wakeup listening: %d", mresult);
        తిరిగి mresult;
      }
    }
  }

  *ఫలితం = Curl_pretransfer(దత్తాంశం);
  ఒకవేళ(*ఫలితం)
    తిరిగి CURLM_OK;

  /* after init, go SETUP */
  multistate(దత్తాంశం, MSTATE_అమరిక);
  Curl_pgrs_సమయం(దత్తాంశం, TIMER_STARTOP);
  తిరిగి CURLM_CALL_MULTI_PERFORM;
}

స్థిర CURLMcode multistate_అమరిక(నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి నిర్మాణం curltime *pnow = Curl_pgrs_now(దత్తాంశం);
  Curl_pgrs_సమయం_Was(దత్తాంశం, TIMER_STARTSINGLE, *pnow);
  ఒకవేళ(దత్తాంశం->సమితి.timeout)
    Curl_expire_సమితి(దత్తాంశం, EXPIRE_TIMEOUT, దత్తాంశం->సమితి.timeout, pnow);
  ఒకవేళ(దత్తాంశం->సమితి.connecttimeout)
    /* Since a connection might go to pending and back to CONNECT several
       times before it actually takes off, we need to set the timeout once
       in SETUP before we enter CONNECT the first time. */
    Curl_expire_సమితి(దత్తాంశం, EXPIRE_CONNECTTIMEOUT,
                    దత్తాంశం->సమితి.connecttimeout, pnow);

  multistate(దత్తాంశం, MSTATE_CONNECT);
  తిరిగి CURLM_CALL_MULTI_PERFORM;
}

స్థిర CURLMcode multistate_connecting(నిర్మాణం Curl_easy *దత్తాంశం,
                                       bool *ప్రవాహం_error,
                                       CURLcode *ఫలితం)
{
  bool connected;

  ఒకవేళ(!దత్తాంశం->conn) {
    DEBUGASSERT(0);
    *ఫలితం = CURLE_FAILED_INIT;
    తిరిగి CURLM_OK;
  }
  ఒకవేళ(!Curl_xfer_recv_is_paused(దత్తాంశం)) {
    *ఫలితం = Curl_conn_connect(దత్తాంశం, FIRSTSOCKET, FALSE, &connected);
    ఒకవేళ(connected && !*ఫలితం) {
      ఒకవేళ(!దత్తాంశం->conn->బిట్లు.reuse &&
         Curl_conn_is_multiplex(దత్తాంశం->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_కాలపట్టిక_pending(దత్తాంశం->multi);
      }
      multistate(దత్తాంశం, MSTATE_PROTOCONNECT);
      తిరిగి CURLM_CALL_MULTI_PERFORM;
    }
    లేకపోతే ఒకవేళ(*ఫలితం) {
      /* failure detected */
      CURL_TRC_M(దత్తాంశం, "connect failed -> %d", (పూర్ణాంకం)*ఫలితం);
      multi_posttransfer(దత్తాంశం);
      multi_పూర్తి(దత్తాంశం, *ఫలితం, TRUE);
      *ప్రవాహం_error = TRUE;
      తిరిగి CURLM_OK;
    }
  }
  తిరిగి CURLM_OK;
}

స్థిర CURLMcode multistate_protoconnect(నిర్మాణం Curl_easy *దత్తాంశం,
                                         bool *ప్రవాహం_error,
                                         CURLcode *ఫలితం)
{
  bool protocol_connected = FALSE;

  ఒకవేళ(!*ఫలితం && దత్తాంశం->conn->బిట్లు.reuse) {
    /* ftp seems to hang when protoconnect on reused connection since we
     * handle PROTOCONNECT in general inside the filters, it seems wrong to
     * restart this on a reused connection.
     */
    multistate(దత్తాంశం, MSTATE_DO);
    తిరిగి CURLM_CALL_MULTI_PERFORM;
  }
  ఒకవేళ(!*ఫలితం)
    *ఫలితం = protocol_connect(దత్తాంశం, &protocol_connected);
  ఒకవేళ(!*ఫలితం && !protocol_connected) {
    /* switch to waiting state */
    multistate(దత్తాంశం, MSTATE_PROTOCONNECTING);
    తిరిగి CURLM_CALL_MULTI_PERFORM;
  }
  లేకపోతే ఒకవేళ(!*ఫలితం) {
    /* protocol connect has completed, go WAITDO or DO */
    multistate(దత్తాంశం, MSTATE_DO);
    తిరిగి CURLM_CALL_MULTI_PERFORM;
  }

  /* failure detected */
  multi_posttransfer(దత్తాంశం);
  multi_పూర్తి(దత్తాంశం, *ఫలితం, TRUE);
  *ప్రవాహం_error = TRUE;
  తిరిగి CURLM_OK;
}

స్థిర CURLMcode multistate_protoconnecting(నిర్మాణం Curl_easy *దత్తాంశం,
                                            bool *ప్రవాహం_error,
                                            CURLcode *ఫలితం)
{
  bool protocol_connected = FALSE;

  /* protocol-specific connect phase */
  *ఫలితం = protocol_connecting(దత్తాంశం, &protocol_connected);
  ఒకవేళ(!*ఫలితం && protocol_connected) {
    /* after the connect has completed, go WAITDO or DO */
    multistate(దత్తాంశం, MSTATE_DO);
    తిరిగి CURLM_CALL_MULTI_PERFORM;
  }
  లేకపోతే ఒకవేళ(*ఫలితం) {
    /* failure detected */
    multi_posttransfer(దత్తాంశం);
    multi_పూర్తి(దత్తాంశం, *ఫలితం, TRUE);
    *ప్రవాహం_error = TRUE;
  }
  తిరిగి CURLM_OK;
}

స్థిర CURLMcode multistate_doing(నిర్మాణం Curl_easy *దత్తాంశం,
                                  bool *ప్రవాహం_error,
                                  CURLcode *ఫలితం)
{
  bool dophase_పూర్తి = FALSE;

  /* we continue DOING until the DO phase is complete */
  DEBUGASSERT(దత్తాంశం->conn);
  *ఫలితం = protocol_doing(దత్తాంశం, &dophase_పూర్తి);
  ఒకవేళ(!*ఫలితం) {
    ఒకవేళ(dophase_పూర్తి) {
      /* after DO, go DO_DONE or DO_MORE */
      multistate(దత్తాంశం, దత్తాంశం->conn->బిట్లు.do_మరిన్ని ?
                 MSTATE_DOING_మరిన్ని : MSTATE_DID);
      తిరిగి CURLM_CALL_MULTI_PERFORM;
    } /* dophase_done */
  }
  లేకపోతే {
    /* failure detected */
    multi_posttransfer(దత్తాంశం);
    multi_పూర్తి(దత్తాంశం, *ఫలితం, FALSE);
    *ప్రవాహం_error = TRUE;
  }
  తిరిగి CURLM_OK;
}

స్థిర CURLMcode multistate_doing_మరిన్ని(నిర్మాణం Curl_easy *దత్తాంశం,
                                       bool *ప్రవాహం_error,
                                       CURLcode *ఫలితం)
{
  domore control;

  /*
   * When we are connected, DOING MORE and then go DID
   */
  DEBUGASSERT(దత్తాంశం->conn);
  *ఫలితం = multi_do_మరిన్ని(దత్తాంశం, &control);

  ఒకవేళ(!*ఫలితం) {
    ఒకవేళ(control != DOMORE_INCOMPLETE) {
      /* if DONE, advance to DO_DONE
         if GOBACK, go back to DOING */
      multistate(దత్తాంశం, control == DOMORE_పూర్తి ? MSTATE_DID : MSTATE_DOING);
      తిరిగి CURLM_CALL_MULTI_PERFORM;
    }
    /* else
       stay in DO_MORE */
  }
  లేకపోతే {
    /* failure detected */
    multi_posttransfer(దత్తాంశం);
    multi_పూర్తి(దత్తాంశం, *ఫలితం, FALSE);
    *ప్రవాహం_error = TRUE;
  }
  తిరిగి CURLM_OK;
}

స్థిర CURLMcode multistate_did(నిర్మాణం Curl_multi *multi,
                                నిర్మాణం Curl_easy *దత్తాంశం)
{
  DEBUGASSERT(దత్తాంశం->conn);
  ఒకవేళ(దత్తాంశం->conn->బిట్లు.multiplex)
    /* Check if we can move pending requests to send pipe */
    multi_కాలపట్టిక_pending(multi); /* multiplexed */

  /* Only perform the transfer if there is a good socket to work with.
     Having both BAD is a signal to skip immediately to DONE */
  ఒకవేళ(CONN_SOCK_IDX_VALID(దత్తాంశం->conn->recv_idx) ||
     CONN_SOCK_IDX_VALID(దత్తాంశం->conn->send_idx)) {
    multistate(దత్తాంశం, MSTATE_PERFORMING);
    /* Do not return CURLM_CALL_MULTI_PERFORM to give other transfers
     * a chance to send off their requests.
     * Note: Some SFTP handlers do not seem to like this.
     *       Restrict it to HTTP families. */
    తిరిగి ((multi->xfers_alive > 1) &&
            (దత్తాంశం->conn->scheme->protocol & PROTO_FAMILY_HTTP)) ?
           CURLM_OK : CURLM_CALL_MULTI_PERFORM;
  }
  లేకపోతే {
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
    ఒకవేళ(దత్తాంశం->state.wildcardmatch &&
       ((దత్తాంశం->conn->scheme->flags & PROTOPT_WILDCARD) == 0)) {
      దత్తాంశం->wildcard->state = CURLWC_పూర్తి;
    }
#అయితే_ముగింపు
    multistate(దత్తాంశం, MSTATE_పూర్తి);
    తిరిగి CURLM_CALL_MULTI_PERFORM;
  }
}

స్థిర CURLMcode multistate_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, CURLcode *presult)
{
  ఒకవేళ(దత్తాంశం->conn) {
    CURLcode ఫలితం;

    /* post-transfer command */
    ఫలితం = multi_పూర్తి(దత్తాంశం, *presult, FALSE);

    /* allow a previously set error code take precedence */
    ఒకవేళ(!(*presult))
      *presult = ఫలితం;
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  ఒకవేళ(దత్తాంశం->state.wildcardmatch) {
    ఒకవేళ(దత్తాంశం->wildcard->state != CURLWC_పూర్తి) {
      /* if a wildcard is set and we are not ending -> lets start again
         with MSTATE_INIT */
      multistate(దత్తాంశం, MSTATE_INIT);
      తిరిగి CURLM_CALL_MULTI_PERFORM;
    }
  }
#అయితే_ముగింపు
  /* after we have DONE what we are supposed to do, go COMPLETED, and
     it does not matter what the multi_done() returned! */
  multistate(దత్తాంశం, MSTATE_COMPLETED);
  తిరిగి CURLM_CALL_MULTI_PERFORM;
}

స్థిర CURLMcode multi_runsingle(నిర్మాణం Curl_multi *multi,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం Curl_sigpipe_ctx *sigpipe_ctx)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి నిర్మాణం curltime *pnow = ఖాళీ;

  ఒకవేళ(multi->dead) {
    /* a multi-level callback returned error before, meaning every individual
     transfer now has failed */
    ఫలితం = CURLE_ABORTED_BY_CALLBACK;
    multi_posttransfer(దత్తాంశం);
    multi_పూర్తి(దత్తాంశం, ఫలితం, FALSE);
    multistate(దత్తాంశం, MSTATE_COMPLETED);
  }

  multi_warn_debug(multi, దత్తాంశం);

  /* transfer runs now, clear the dirty bit. This may be set
   * again during processing, triggering a re-run later. */
  Curl_uint32_bset_remove(&multi->dirty, దత్తాంశం->mid);

  ఒకవేళ(దత్తాంశం == multi->admin) {
#నిర్వచితమైతే ENABLE_అంతర్గత_WAKEUP
    /* Consume any pending wakeup signals before processing.
     * This is necessary for event based processing. See #21547 */
    (శూన్య)Curl_wakeup_consume(multi->wakeup_అంతర్గత, TRUE);
#అయితే_ముగింపు
#నిర్వచితమైతే USE_RESOLV_THREADED
    Curl_async_thrdd_multi_process(multi);
#అయితే_ముగింపు
    Curl_cshutdn_perform(&multi->cshutdn, multi->admin, sigpipe_ctx);
    వెళ్ళు బహిర్గతం;
  }

  sigpipe_apply(దత్తాంశం, sigpipe_ctx);
  చేయి {
    /* A "stream" here is a logical stream if the protocol can handle that
       (HTTP/2), or the full connection for older protocols */
    bool ప్రవాహం_error = FALSE;
    mresult = CURLM_OK;
    pnow = ఖాళీ;

    ఒకవేళ(multi_ischanged(multi, TRUE)) {
      CURL_TRC_M(దత్తాంశం, "multi changed, check CONNECT_PEND queue");
      multi_కాలపట్టిక_pending(multi); /* multiplexed */
    }

    ఒకవేళ(దత్తాంశం->mstate > MSTATE_CONNECT &&
       దత్తాంశం->mstate < MSTATE_COMPLETED) {
      /* Make sure we set the connection's current owner */
      DEBUGASSERT(దత్తాంశం->conn);
      ఒకవేళ(!దత్తాంశం->conn) {
        mresult = CURLM_అంతర్గత_ERROR;
        వెళ్ళు బహిర్గతం;
      }
    }

    /* Wait for the connect state as only then is the start time stored, but
       we must not check already completed handles */
    ఒకవేళ((దత్తాంశం->mstate >= MSTATE_CONNECT) && (దత్తాంశం->mstate < MSTATE_COMPLETED)) {
      pnow = Curl_pgrs_now(దత్తాంశం);
      ఒకవేళ(multi_handle_timeout(దత్తాంశం, pnow, &ప్రవాహం_error, &ఫలితం))
        /* Skip the statemachine and go directly to error handling section. */
        వెళ్ళు statemachine_చివర;
      pnow = ఖాళీ;
    }

    ఎంపిక(దత్తాంశం->mstate) {
    స్థితి MSTATE_INIT:
      /* Transitional state. init this transfer. A handle never comes back to
         this state. */
      mresult = multistate_init(దత్తాంశం, &ఫలితం);
      ఆపు;

    స్థితి MSTATE_అమరిక:
      /* Transitional state. Setup things for a new transfer. The handle
         can come back to this state on a redirect. */
      mresult = multistate_అమరిక(దత్తాంశం);
      ఆపు;

    స్థితి MSTATE_CONNECT:
      mresult = multistate_connect(multi, దత్తాంశం, &ఫలితం);
      ఆపు;

    స్థితి MSTATE_CONNECTING:
      /* awaiting a completion of an asynch TCP connect */
      mresult = multistate_connecting(దత్తాంశం, &ప్రవాహం_error, &ఫలితం);
      ఆపు;

    స్థితి MSTATE_PROTOCONNECT:
      mresult = multistate_protoconnect(దత్తాంశం, &ప్రవాహం_error, &ఫలితం);
      ఆపు;

    స్థితి MSTATE_PROTOCONNECTING:
      /* protocol-specific connect phase */
      mresult = multistate_protoconnecting(దత్తాంశం, &ప్రవాహం_error, &ఫలితం);
      ఆపు;

    స్థితి MSTATE_DO:
      mresult = multistate_do(దత్తాంశం, &ప్రవాహం_error, &ఫలితం);
      ఆపు;

    స్థితి MSTATE_DOING:
      /* we continue DOING until the DO phase is complete */
      mresult = multistate_doing(దత్తాంశం, &ప్రవాహం_error, &ఫలితం);
      ఆపు;

    స్థితి MSTATE_DOING_మరిన్ని:
      /*
       * When we are connected, DOING MORE and then go DID
       */
      mresult = multistate_doing_మరిన్ని(దత్తాంశం, &ప్రవాహం_error, &ఫలితం);
      ఆపు;

    స్థితి MSTATE_DID:
      mresult = multistate_did(multi, దత్తాంశం);
      ఆపు;

    స్థితి MSTATE_RATELIMITING: /* limit-rate exceeded in either direction */
      mresult = multistate_ratelimiting(దత్తాంశం, &ఫలితం);
      ఆపు;

    స్థితి MSTATE_PERFORMING:
      mresult = multistate_performing(దత్తాంశం, &ప్రవాహం_error, &ఫలితం);
      ఆపు;

    స్థితి MSTATE_పూర్తి:
      mresult = multistate_పూర్తి(దత్తాంశం, &ఫలితం);
      ఆపు;

    స్థితి MSTATE_COMPLETED:
      ఆపు;

    స్థితి MSTATE_PENDING:
    స్థితి MSTATE_MSGSENT:
      /* handles in these states should NOT be in this list */
      ఆపు;

    అప్రమేయ:
      mresult = CURLM_అంతర్గత_ERROR;
      వెళ్ళు బహిర్గతం;
    }

    ఒకవేళ(దత్తాంశం->mstate >= MSTATE_CONNECT &&
       దత్తాంశం->mstate < MSTATE_DO &&
       mresult != CURLM_CALL_MULTI_PERFORM &&
       !multi_ischanged(multi, FALSE)) {
      /* We now handle stream timeouts if and only if this will be the last
       * loop iteration. We only check this on the last iteration to ensure
       * that if we know we have additional work to do immediately
       * (i.e. CURLM_CALL_MULTI_PERFORM == TRUE) then we should do that before
       * declaring the connection timed out as we may almost have a completed
       * connection. */
      pnow = Curl_pgrs_now(దత్తాంశం);
      multi_handle_timeout(దత్తాంశం, pnow, &ప్రవాహం_error, &ఫలితం);
    }

statemachine_చివర:
    ఒకవేళ(!pnow)
      pnow = Curl_pgrs_now(దత్తాంశం);
    ఫలితం = is_ముగిసిన(multi, దత్తాంశం, pnow, ప్రవాహం_error, ఫలితం);
    ఒకవేళ(ఫలితం)
      mresult = CURLM_CALL_MULTI_PERFORM;

    ఒకవేళ(MSTATE_COMPLETED == దత్తాంశం->mstate) {
      handle_completed(multi, దత్తాంశం, ఫలితం);
      mresult = CURLM_OK;
      వెళ్ళు బహిర్గతం;
    }
  } అయితే((mresult == CURLM_CALL_MULTI_PERFORM) ||
          multi_ischanged(multi, FALSE));

బహిర్గతం:
  దత్తాంశం->ఫలితం = ఫలితం;
  తిరిగి mresult;
}

స్థిర CURLMcode multi_perform(నిర్మాణం Curl_multi *multi,
                               పూర్ణాంకం *running_handles)
{
  CURLMcode returncode = CURLM_OK;
  నిర్మాణం curltime ప్రారంభం;
  uint32_t mid;
  నిర్మాణం Curl_sigpipe_ctx sigpipe_ctx;

  curlx_pnow(&ప్రారంభం);
  sigpipe_init(&sigpipe_ctx);

  ఒకవేళ(Curl_uint32_bset_మొదటి(&multi->process, &mid)) {
    CURL_TRC_M(multi->admin, "multi_perform(running=%u)",
               multi_xfers_running(multi));
    చేయి {
      నిర్మాణం Curl_easy *దత్తాంశం = Curl_multi_get_easy(multi, mid);
      CURLMcode mresult;
      ఒకవేళ(!దత్తాంశం) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        కొనసాగు;
      }
      mresult = multi_runsingle(multi, దత్తాంశం, &sigpipe_ctx);
      ఒకవేళ(mresult)
        returncode = mresult;
    } అయితే(Curl_uint32_bset_తదుపరి(&multi->process, mid, &mid));
  }
  sigpipe_restore(&sigpipe_ctx);

  ఒకవేళ(multi_ischanged(multi, TRUE))
    multi_కాలపట్టిక_pending(multi);

  ఒకవేళ(!returncode && CURL_MNTFY_HAS_ENTRIES(multi))
    returncode = Curl_mntfy_dispatch_all(multi);

  /*
   * Remove all expired timers from the splay since handles are dealt
   * with unconditionally by this function and curl_multi_timeout() requires
   * that already passed/handled expire times are removed from the splay.
   *
   * It is important that the 'now' value is set at the entry of this function
   * and not for the current time as it may have ticked a little while since
   * then and then we risk this loop to remove timers that actually have not
   * been handled!
   */
  అయితే(Curl_timeouts_remove_expired(&multi->timeouts, &ప్రారంభం, &mid)) {
    /* the removed may have another timeout in queue */
    నిర్మాణం Curl_easy *దత్తాంశం = Curl_multi_get_easy(multi, mid);
    ఒకవేళ(!దత్తాంశం) {
      DEBUGASSERT(0);
      కొనసాగు;
    }
    (శూన్య)కూడు_తదుపరి_timeout(&ప్రారంభం, multi, దత్తాంశం);
    ఒకవేళ(దత్తాంశం->mstate == MSTATE_PENDING) {
      bool ప్రవాహం_వాడని;
      CURLcode ఫలితం_వాడని;
      ఒకవేళ(multi_handle_timeout(దత్తాంశం, Curl_pgrs_now(దత్తాంశం),
                              &ప్రవాహం_వాడని, &ఫలితం_వాడని)) {
        infof(దత్తాంశం, "PENDING handle timeout");
        move_pending_గమ్యం_connect(multi, దత్తాంశం);
      }
    }
  }

  ఒకవేళ(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < గరిష్ఠ_పూర్ణాంకం) ? (పూర్ణాంకం)running : గరిష్ఠ_పూర్ణాంకం;
  }

  ఒకవేళ(CURLM_OK >= returncode)
    returncode = Curl_update_timer(multi);

  తిరిగి returncode;
}

CURLMcode curl_multi_perform(CURLM *m, పూర్ణాంకం *running_handles)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_perform, &mresult)) {
    mresult = multi_perform(m, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

CURLMcode curl_multi_cleanup(CURLM *m)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_cleanup, &mresult)) {
    నిర్మాణం Curl_multi *multi = m;
    శూన్య *నమోదు_2;
    uint32_t mid;

    /* First remove all remaining easy handles,
     * close internal ones. admin handle is special */
    ఒకవేళ(Curl_uint32_tbl_మొదటి(&multi->xfers, &mid, &నమోదు_2)) {
      చేయి {
        నిర్మాణం Curl_easy *దత్తాంశం = నమోదు_2;
        ఒకవేళ(!GOOD_EASY_HANDLE(దత్తాంశం)) {
          mresult = CURLM_BAD_HANDLE;
          వెళ్ళు బహిర్గతం;
        }

#నిర్వచితమైతే DEBUGBUILD
        ఒకవేళ(mid != దత్తాంశం->mid) {
          CURL_TRC_M(దత్తాంశం, "multi_cleanup: still present with mid=%u, "
                     "but unexpected data->mid=%u\n", mid, దత్తాంశం->mid);
          DEBUGASSERT(0);
        }
#అయితే_ముగింపు

        ఒకవేళ(దత్తాంశం == multi->admin)
          కొనసాగు;

        ఒకవేళ(!దత్తాంశం->state.పూర్తి && దత్తాంశం->conn)
          /* if DONE was never called for this handle */
          (శూన్య)multi_పూర్తి(దత్తాంశం, CURLE_OK, TRUE);

        దత్తాంశం->multi = ఖాళీ; /* clear the association */
        Curl_uint32_tbl_remove(&multi->xfers, mid);
        దత్తాంశం->mid = UINT32_MAX;

#నిర్వచితమైతే USE_LIBPSL
        ఒకవేళ(దత్తాంశం->psl == &multi->psl)
          దత్తాంశం->psl = ఖాళీ;
#అయితే_ముగింపు
        ఒకవేళ(దత్తాంశం->state.అంతర్గత)
          Curl_close(&దత్తాంశం);
      } అయితే(Curl_uint32_tbl_తదుపరి(&multi->xfers, mid, &mid, &నమోదు_2));
    }

#నిర్వచితమైతే USE_RESOLV_THREADED
    Curl_async_thrdd_multi_destroy(multi, !multi->quick_exit);
#అయితే_ముగింపు
    Curl_cpool_destroy(&multi->cpool, multi->admin);
    Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
    ఒకవేళ(multi->admin) {
      CURL_TRC_M(multi->admin, "multi_cleanup, closing admin handle, done");
      multi->admin->multi = ఖాళీ;
      Curl_uint32_tbl_remove(&multi->xfers, multi->admin->mid);
      Curl_close(&multi->admin);
    }

    multi->magic = 0; /* not good anymore */

    Curl_multi_ev_cleanup(multi);
    Curl_hash_destroy(&multi->proto_hash);
    Curl_dnscache_destroy(&multi->dnscache);
    Curl_psl_destroy(&multi->psl);
#నిర్వచితమైతే USE_SSL
    Curl_ssl_scache_destroy(multi->ssl_scache);
#అయితే_ముగింపు

#నిర్వచితమైతే USE_WINSOCK
    WSACloseEvent(multi->wsa_event);
#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_అంతర్గత_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_అంతర్గత);
#అయితే_ముగింపు

    multi_xfer_bufs_free(multi);
    Curl_mntfy_cleanup(multi);
#నిర్వచితమైతే DEBUGBUILD
    ఒకవేళ(Curl_uint32_tbl_లెక్క(&multi->xfers)) {
      multi_xfer_tbl_dump(multi);
      DEBUGASSERT(0);
    }
#అయితే_ముగింపు
    Curl_uint32_bset_destroy(&multi->process);
    Curl_uint32_bset_destroy(&multi->dirty);
    Curl_uint32_bset_destroy(&multi->pending);
    Curl_uint32_bset_destroy(&multi->msgsent);
    Curl_uint32_tbl_destroy(&multi->xfers);
    curlx_memzero(multi, పరిమాణం(*multi));
    curlx_free(multi);

    mresult = CURLM_OK;
  }
బహిర్గతం:
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

/*
 * curl_multi_info_read()
 *
 * This function is the primary way for a multi/multi_socket application to
 * figure out if a transfer has ended.
 */

CURLMsg *curl_multi_info_read(CURLM *m, పూర్ణాంకం *msgs_లోపల_వరుస)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMsg *సందేశం_ఫలితం = ఖాళీ;

  *msgs_లోపల_వరుస = 0; /* default to none */
  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_info_read, ఖాళీ)) {
    నిర్మాణం Curl_multi *multi = m;
    uint32_t mid;
    ఒకవేళ(Curl_uint32_bset_మొదటి(&multi->msgsent, &mid)) {
      నిర్మాణం Curl_easy *దత్తాంశం = Curl_multi_get_easy(multi, mid);

      DEBUGASSERT(దత్తాంశం);
      Curl_uint32_bset_remove(&multi->msgsent, mid);
      *msgs_లోపల_వరుస =
        curlx_uztosi(Curl_uint32_bset_లెక్క(&multi->msgsent));
      ఒకవేళ(దత్తాంశం)
        సందేశం_ఫలితం = &దత్తాంశం->సందేశం;
    }
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి సందేశం_ఫలితం;
}

శూన్య Curl_multi_will_close(నిర్మాణం Curl_easy *దత్తాంశం, curl_socket_t s)
{
  ఒకవేళ(దత్తాంశం) {
    నిర్మాణం Curl_multi *multi = దత్తాంశం->multi;
    ఒకవేళ(multi) {
      CURL_TRC_M(దత్తాంశం, "Curl_multi_will_close fd=%" FMT_SOCKET_T, s);
      Curl_multi_ev_socket_పూర్తి(multi, దత్తాంశం, s);
    }
  }
}

స్థిర శూన్య multi_timeouts_init(నిర్మాణం Curl_easy *దత్తాంశం)
{
  దత్తాంశం->state.timeouts.మొదటి = EXPIRE_చివరి;
  దత్తాంశం->state.timeouts.splaynode.registered = FALSE;
}

/*
 * Each Curl_easy has a list of timeouts. The add_next_timeout() is called
 * when it has been removed from the splay tree because the timeout has
 * expired. This function is then to advance in the list to pick the next
 * timeout to use (skip the already expired ones) and add this node back to
 * the splay tree again.
 *
 * The splay tree only has each Curl_easy as a single node and the nearest
 * timeout is used to sort it on.
 */
స్థిర CURLMcode కూడు_తదుపరి_timeout(స్థిరరాశి నిర్మాణం curltime *pnow,
                                  నిర్మాణం Curl_multi *multi,
                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం expire_timers *timeouts = &దత్తాంశం->state.timeouts;
  timediff_t now_us = Curl_timeouts_స్థానభ్రంశం_us(&multi->timeouts, pnow);

  అయితే(timeouts->మొదటి < EXPIRE_చివరి) {
    ఒకవేళ(timeouts->స్థానభ్రంశం_us[timeouts->మొదటి] <= now_us)  /* already expired */
      timeouts->మొదటి = timeouts->తదుపరి[timeouts->మొదటి];
    లేకపోతే /* timeouts are sorted, first is first in the future now */
      ఆపు;
  }

  ఒకవేళ(timeouts->మొదటి < EXPIRE_చివరి) {
    /* Insert this node again into the splay. Keep the timer in the list in
       case we need to recompute future timers. */
    Curl_timeouts_కూడు(&multi->timeouts, దత్తాంశం,
                      timeouts->స్థానభ్రంశం_us[timeouts->మొదటి]);
  }
  తిరిగి CURLM_OK;
}

స్థిర శూన్య multi_mark_expired_as_dirty(నిర్మాణం Curl_multi *multi,
                                        స్థిరరాశి నిర్మాణం curltime *ts)
{
  నిర్మాణం Curl_easy *దత్తాంశం = ఖాళీ;
  uint32_t mid;

  /*
   * The loop following here will go on as long as there are expire-times left
   * to process (compared to `ts`) in the splay and 'data' will be
   * re-assigned for every expired handle we deal with.
   */
  అయితే(Curl_timeouts_remove_expired(&multi->timeouts, ts, &mid)) {
    /* Check if there is one (more) expired timer to deal with! This function
       extracts a matching node if there is one */
    దత్తాంశం = Curl_multi_get_easy(multi, mid);
    ఒకవేళ(!దత్తాంశం) {
      DEBUGASSERT(0);
      కొనసాగు;
    }
#నిర్వచితమైతే CURLVERBOSE
    ఒకవేళ(CURL_TRC_TIMER_is_verbose(దత్తాంశం)) {
      ఒకవేళ(దత్తాంశం->state.timeouts.మొదటి < EXPIRE_చివరి) {
        CURL_TRC_TIMER(దత్తాంశం, దత్తాంశం->state.timeouts.మొదటి, "has expired");
      }
    }
#అయితే_ముగింపు
    (శూన్య)కూడు_తదుపరి_timeout(ts, multi, దత్తాంశం);
    Curl_multi_mark_dirty(దత్తాంశం);
  }
}

స్థిర CURLMcode multi_నిరంతరం_dirty(నిర్మాణం Curl_multi *multi,
                                 నిర్మాణం Curl_sigpipe_ctx *sigpipe_ctx,
                                 uint32_t *pnum)
{
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;

  *pnum = 0;
  ఒకవేళ(Curl_uint32_bset_మొదటి(&multi->dirty, &mid)) {
    చేయి {
      నిర్మాణం Curl_easy *దత్తాంశం = Curl_multi_get_easy(multi, mid);
      ఒకవేళ(దత్తాంశం) {
        CURL_TRC_M(దత్తాంశం, "multi_run_dirty");

        ఒకవేళ(!Curl_uint32_bset_కలిగియుంది(&multi->process, mid)) {
          ఒకవేళ(Curl_uint32_bset_కలిగియుంది(&multi->pending, mid)) {
            /* Something happened on the pending transfer, act on it. */
            move_pending_గమ్యం_connect(multi, దత్తాంశం);
          }
          లేకపోతే {
            /* We are no longer processing this transfer */
            Curl_uint32_bset_remove(&multi->dirty, mid);
            కొనసాగు;
          }
        }

        (*pnum)++;
        /* runsingle() clears the dirty mid */
        mresult = multi_runsingle(multi, దత్తాంశం, sigpipe_ctx);

        ఒకవేళ(CURLM_OK >= mresult) {
          /* reassess event handling of data */
          mresult = Curl_multi_ev_assess_xfer(multi, దత్తాంశం);
          ఒకవేళ(mresult)
            వెళ్ళు బహిర్గతం;
        }
      }
      లేకపోతే {
        CURL_TRC_M(multi->admin, "multi_run_dirty, %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } అయితే(Curl_uint32_bset_తదుపరి(&multi->dirty, mid, &mid));
  }

బహిర్గతం:
  తిరిగి mresult;
}

స్థిర CURLMcode multi_socket(నిర్మాణం Curl_multi *multi,
                              bool checkall,
                              curl_socket_t s,
                              పూర్ణాంకం ev_bitmask,
                              పూర్ణాంకం *running_handles)
{
  CURLMcode mresult = CURLM_OK;
  నిర్మాణం Curl_sigpipe_ctx pipe_ctx;
  నిర్మాణం curltime now;
  uint32_t నిరంతరం_xfers;

  (శూన్య)ev_bitmask;
  sigpipe_init(&pipe_ctx);

  ఒకవేళ(checkall) {
    /* *perform() deals with running_handles on its own */
    mresult = multi_perform(multi, running_handles);

    ఒకవేళ(mresult != CURLM_BAD_HANDLE) {
      /* Reassess event status of all active transfers, but keep any error
         from multi_perform() */
      CURLMcode mresult2 =
        Curl_multi_ev_assess_xfer_bset(multi, &multi->process);
      ఒకవేళ(!mresult)
        mresult = mresult2;
    }
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(s != CURL_SOCKET_TIMEOUT) {
    /* Mark all transfers of that socket as dirty */
    Curl_multi_ev_dirty_xfers(multi, s);
  }
  లేకపోతే {
    /* Asked to run due to time-out. Clear the 'last_expire_ts' variable to
       force Curl_update_timer() to trigger a callback to the app again even
       if the same timeout is still the one to run after this call. That
       handles the case when the application asks libcurl to run the timeout
       prematurely. */
    multi->చివరి_expire_స్థానభ్రంశం_us = 0;

    /* Applications may set `socket_cb` *after* having added transfers
     * first. *Then* kick off processing with a
     * curl_multi_socket_action(TIMEOUT) afterwards. Make sure our
     * admin handle registers its pollset with the callbacks present. */
    mresult = multi_assess_wakeup(multi);
    ఒకవేళ(mresult)
      వెళ్ళు బహిర్గతం;
  }

  curlx_pnow(&now);
  multi_mark_expired_as_dirty(multi, &now);
  mresult = multi_నిరంతరం_dirty(multi, &pipe_ctx, &నిరంతరం_xfers);
  ఒకవేళ(mresult)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(నిరంతరం_xfers) {
    /* Running transfers takes time. With a new timestamp, we might catch
     * other expires which are due now. Instead of telling the application
     * to set a 0 timeout and call us again, we run them here.
     * Do that only once or it might be unfair to transfers on other
     * sockets. */
    curlx_pnow(&now);
    multi_mark_expired_as_dirty(multi, &now);
    mresult = multi_నిరంతరం_dirty(multi, &pipe_ctx, &నిరంతరం_xfers);
  }

బహిర్గతం:
  sigpipe_restore(&pipe_ctx);

  ఒకవేళ(multi_ischanged(multi, TRUE))
    multi_కాలపట్టిక_pending(multi);

  ఒకవేళ(!mresult && CURL_MNTFY_HAS_ENTRIES(multi))
    mresult = Curl_mntfy_dispatch_all(multi);

  ఒకవేళ(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < గరిష్ఠ_పూర్ణాంకం) ? (పూర్ణాంకం)running : గరిష్ఠ_పూర్ణాంకం;
  }

  ఒకవేళ(CURLM_OK >= mresult)
    mresult = Curl_update_timer(multi);
  తిరిగి mresult;
}

#నిర్వచనం_రద్దు curl_multi_setopt
CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_setopt, &mresult)) {
    నిర్మాణం Curl_multi *multi = m;
    va_list param;
    గుర్తులేని దీర్ఘ uarg;
    size_t szarg;

    va_start(param, option);

    ఎంపిక(option) {
    స్థితి CURLMOPT_SOCKETFUNCTION:
      multi->socket_cb = va_arg(param, curl_socket_callback);
      ఆపు;
    స్థితి CURLMOPT_SOCKETDATA:
      multi->socket_userp = va_arg(param, శూన్య *);
      ఆపు;
    స్థితి CURLMOPT_PUSHFUNCTION:
      multi->నెట్టు_cb = va_arg(param, curl_నెట్టు_callback);
      ఆపు;
    స్థితి CURLMOPT_PUSHDATA:
      multi->నెట్టు_userp = va_arg(param, శూన్య *);
      ఆపు;
    స్థితి CURLMOPT_PIPELINING:
      multi->multiplexing = va_arg(param, దీర్ఘ) & CURLPIPE_MULTIPLEX ? 1 : 0;
      ఆపు;
    స్థితి CURLMOPT_TIMERFUNCTION:
      multi->timer_cb = va_arg(param, curl_multi_timer_callback);
      ఆపు;
    స్థితి CURLMOPT_TIMERDATA:
      multi->timer_userp = va_arg(param, శూన్య *);
      ఆపు;
    స్థితి CURLMOPT_MAXCONNECTS:
      uarg = va_arg(param, గుర్తులేని దీర్ఘ);
      ఒకవేళ(uarg <= UINT32_MAX)
        multi->maxconnects = (uint32_t)uarg;
      ఆపు;
    స్థితి CURLMOPT_MAX_HOST_CONNECTIONS:
      ఒకవేళ(!curlx_sltouz(va_arg(param, దీర్ఘ), &szarg))
        mresult = CURLM_BAD_FUNCTION_వాదం;
      multi->max_host_connections = (szarg < UINT32_MAX) ?
                                    (uint32_t)szarg : UINT32_MAX;
      ఆపు;
    స్థితి CURLMOPT_MAX_మొత్తం_CONNECTIONS:
      ఒకవేళ(!curlx_sltouz(va_arg(param, దీర్ఘ), &szarg))
        mresult = CURLM_BAD_FUNCTION_వాదం;
      multi->max_మొత్తం_connections = (szarg < UINT32_MAX) ?
                                     (uint32_t)szarg : UINT32_MAX;
      ఆపు;
      /* options formerly used for pipelining */
    స్థితి CURLMOPT_MAX_PIPELINE_నిడివి:
      ఆపు;
    స్థితి CURLMOPT_CONTENT_నిడివి_PENALTY_కొలత:
      ఆపు;
    స్థితి CURLMOPT_CHUNK_నిడివి_PENALTY_కొలత:
      ఆపు;
    స్థితి CURLMOPT_PIPELINING_SITE_BL:
      ఆపు;
    స్థితి CURLMOPT_PIPELINING_SERVER_BL:
      ఆపు;
    స్థితి CURLMOPT_MAX_CONCURRENT_STREAMS: {
      ఒకవేళ(!curlx_sltouz(va_arg(param, దీర్ఘ), &szarg) ||
         !szarg || (szarg > (size_t)గరిష్ఠ_పూర్ణాంకం)) /* preserve previous cutoff */
        multi->max_concurrent_streams = 100;
      లేకపోతే
        multi->max_concurrent_streams = (szarg < UINT32_MAX) ?
                                       (uint32_t)szarg : UINT32_MAX;
      ఆపు;
    }
    స్థితి CURLMOPT_జాలం_మారింది: {
      దీర్ఘ val = va_arg(param, దీర్ఘ);
      ఒకవేళ(val & CURLMNWC_CLEAR_ALL)
        /* In the beginning, all values available to set were 1 by mistake. We
           converted this to mean "all", thus setting all the bits
           automatically */
        val = CURLMNWC_CLEAR_DNS | CURLMNWC_CLEAR_CONNS;
      ఒకవేళ(val & CURLMNWC_CLEAR_DNS) {
        Curl_dnscache_clear(multi->admin);
      }
      ఒకవేళ(val & CURLMNWC_CLEAR_CONNS) {
        Curl_cpool_nw_మారింది(&multi->cpool, multi->admin);
      }
      ఆపు;
    }
    స్థితి CURLMOPT_NOTIFYFUNCTION:
      multi->ntfy.ntfy_cb = va_arg(param, curl_notify_callback);
      ఆపు;
    స్థితి CURLMOPT_NOTIFYDATA:
      multi->ntfy.ntfy_cb_దత్తాంశం = va_arg(param, శూన్య *);
      ఆపు;
    స్థితి CURLMOPT_RESOLVE_దారాలు_MAX:
#నిర్వచితమైతే USE_RESOLV_THREADED
      uarg = va_arg(param, దీర్ఘ);
      ఒకవేళ((uarg <= 0) || (uarg > UINT32_MAX))
        mresult = CURLM_BAD_FUNCTION_వాదం;
      లేకపోతే {
        CURLcode ఫలితం = Curl_async_thrdd_multi_సమితి_props(
          multi, 0, (uint32_t)uarg, 2000);
        ఎంపిక(ఫలితం) {
        స్థితి CURLE_OK:
          mresult = CURLM_OK;
          ఆపు;
        స్థితి CURLE_BAD_FUNCTION_వాదం:
          mresult = CURLM_BAD_FUNCTION_వాదం;
          ఆపు;
        స్థితి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం:
          mresult = CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం;
          ఆపు;
        అప్రమేయ:
          mresult = CURLM_అంతర్గత_ERROR;
          ఆపు;
        }
      }
#అయితే_ముగింపు
      ఆపు;
    స్థితి CURLMOPT_శీఘ్ర_EXIT:
      multi->quick_exit = va_arg(param, దీర్ఘ) ? 1 : 0;
      ఆపు;
    అప్రమేయ:
      mresult = CURLM_UNKNOWN_OPTION;
      ఆపు;
    }
    va_end(param);
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

/* we define curl_multi_socket() in the public multi.h header */
#నిర్వచనం_రద్దు curl_multi_socket

CURLMcode curl_multi_socket(CURLM *m, curl_socket_t s, పూర్ణాంకం *running_handles)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_socket, &mresult)) {
    mresult = multi_socket(m, FALSE, s, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

CURLMcode curl_multi_socket_action(CURLM *m, curl_socket_t s,
                                   పూర్ణాంకం ev_bitmask, పూర్ణాంకం *running_handles)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_socket_action, &mresult)) {
    mresult = multi_socket(m, FALSE, s, ev_bitmask, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

CURLMcode curl_multi_socket_all(CURLM *m, పూర్ణాంకం *running_handles)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_socket_all, &mresult)) {
    mresult = multi_socket(m, TRUE, CURL_SOCKET_BAD, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

స్థిర bool multi_has_dirties(నిర్మాణం Curl_multi *multi)
{
  uint32_t mid;
  ఒకవేళ(Curl_uint32_bset_మొదటి(&multi->dirty, &mid)) {
    చేయి {
      నిర్మాణం Curl_easy *దత్తాంశం = Curl_multi_get_easy(multi, mid);
      ఒకవేళ(దత్తాంశం) {
        ఒకవేళ(Curl_uint32_bset_కలిగియుంది(&multi->process, mid))
          తిరిగి TRUE;
        /* We are no longer processing this transfer */
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
      లేకపోతే {
        CURL_TRC_M(multi->admin, "dirty transfer %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } అయితే(Curl_uint32_bset_తదుపరి(&multi->dirty, mid, &mid));
  }
  తిరిగి FALSE;
}

స్థిర శూన్య multi_timeout(నిర్మాణం Curl_multi *multi,
                          timediff_t *pexire_స్థానభ్రంశం_us,
                          పూర్ణాంకం *timeout_ms)
{
  ఒకవేళ(multi->dead) {
    ఒకవేళ(pexire_స్థానభ్రంశం_us)
      *pexire_స్థానభ్రంశం_us = 0;
    *timeout_ms = 0;
    తిరిగి;
  }

  ఒకవేళ(multi_has_dirties(multi)) {
    ఒకవేళ(pexire_స్థానభ్రంశం_us) {
      నిర్మాణం curltime now;
      curlx_pnow(&now);
      *pexire_స్థానభ్రంశం_us = Curl_timeouts_స్థానభ్రంశం_us(&multi->timeouts, &now);
    }
    *timeout_ms = 0;
    తిరిగి;
  }
  లేకపోతే {
    నిర్మాణం curltime now;
    uint32_t mid;

    curlx_pnow(&now);
    *timeout_ms = Curl_timeouts_తదుపరి_ms(&multi->timeouts, &now,
                                        pexire_స్థానభ్రంశం_us, &mid);
#నిర్వచితమైతే CURLVERBOSE
    ఒకవేళ(mid != UINT32_MAX) {
      నిర్మాణం Curl_easy *దత్తాంశం = Curl_multi_get_easy(multi, mid);
      ఒకవేళ(దత్తాంశం && CURL_TRC_TIMER_is_verbose(దత్తాంశం) &&
         (దత్తాంశం->state.timeouts.మొదటి < EXPIRE_చివరి)) {
        CURL_TRC_TIMER(దత్తాంశం, దత్తాంశం->state.timeouts.మొదటి,
                       "gives multi timeout in %dms", *timeout_ms);
      }
    }
#అయితే_ముగింపు
  }
}

CURLMcode curl_multi_timeout(CURLM *m,
                             దీర్ఘ *timeout_ms)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_timeout, &mresult)) {
    పూర్ణాంకం itimeout_ms;

    multi_timeout(m, ఖాళీ, &itimeout_ms);
    *timeout_ms = (దీర్ఘ)itimeout_ms;
    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

/*
 * Tell the application it should update its timers, if it subscribes to the
 * update timer callback.
 */
CURLMcode Curl_update_timer(నిర్మాణం Curl_multi *multi)
{
  timediff_t timeouts_స్థానభ్రంశం_us = 0;
  పూర్ణాంకం timeout_ms;
  పూర్ణాంకం rc;
  bool సమితి_విలువ = FALSE;

  ఒకవేళ(!multi->timer_cb || multi->dead)
    తిరిగి CURLM_OK;
  multi_timeout(multi, &timeouts_స్థానభ్రంశం_us, &timeout_ms);

  ఒకవేళ(timeout_ms < 0 && !multi->చివరి_timeout_సమితి) {
    /* nothing to do */
  }
  లేకపోతే ఒకవేళ(timeout_ms < 0) {
    /* there is no timeout now but there was one previously */
    CURL_TRC_M(multi->admin, "[TIMER] clear");
    timeout_ms = -1; /* normalize */
    సమితి_విలువ = TRUE;
  }
  లేకపోతే ఒకవేళ(!multi->చివరి_timeout_సమితి) {
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, none before", timeout_ms);
    సమితి_విలువ = TRUE;
  }
  లేకపోతే ఒకవేళ(multi->చివరి_expire_స్థానభ్రంశం_us != timeouts_స్థానభ్రంశం_us) {
    /* We had a timeout before and have one now, the absolute timestamp
     * differs. The relative timeout_ms may be the same, but the starting
     * point differs. Let the application restart its timer. */
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, replace previous",
               timeout_ms);
    సమితి_విలువ = TRUE;
  }
  లేకపోతే {
    /* We have same expire time as previously. Our relative 'timeout_ms'
     * may be different now, but the application has the timer running
     * and we do not to tell it to start this again. */
  }

  ఒకవేళ(సమితి_విలువ) {
    నిర్మాణం Curl_mapi_guard guard;

    multi->చివరి_expire_స్థానభ్రంశం_us = timeouts_స్థానభ్రంశం_us;
    multi->చివరి_timeout_సమితి = timeout_ms >= 0;
    CURL_CBAPI_MULTI_ప్రారంభం(&guard, multi, multi_timer_cb);
    rc = multi->timer_cb(multi, timeout_ms, multi->timer_userp);
    CURL_CBAPI_MULTI_చివర(&guard);
    ఒకవేళ(rc == -1) {
      multi->dead = TRUE;
      తిరిగి CURLM_ABORTED_BY_CALLBACK;
    }
  }
  తిరిగి CURLM_OK;
}

#నిర్వచితమైతే DEBUGBUILD
స్థిర bool multi_timeouts_check(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం expire_timers *timeouts = &దత్తాంశం->state.timeouts;
  uint8_t id;
  పూర్ణాంకం i = 0;
  కొరకు(id = timeouts->మొదటి; id < EXPIRE_చివరి; id = timeouts->తదుపరి[id]) {
    ఒకవేళ(++i >= EXPIRE_చివరి) {
      failf(దత్తాంశం, "expire timeouts looped: %d iterations and no end", i);
      తిరిగి FALSE;
    }
    ఒకవేళ(id == timeouts->తదుపరి[id]) {
      failf(దత్తాంశం, "expire timeouts wrong: %d points to itself", (పూర్ణాంకం)id);
      తిరిగి FALSE;
    }
    ఒకవేళ((timeouts->తదుపరి[id] < EXPIRE_చివరి) &&
       (timeouts->స్థానభ్రంశం_us[id] > timeouts->స్థానభ్రంశం_us[timeouts->తదుపరి[id]])) {
      failf(దత్తాంశం, "expire timeouts not sorted: %d happens after %d but "
            "is listed before", (పూర్ణాంకం)id, (పూర్ణాంకం)timeouts->తదుపరి[id]);
      తిరిగి FALSE;
    }
  }
  తిరిగి TRUE;
}
#అయితే_ముగింపు

/*
 * Remove a given timestamp from the list of timeouts.
 */
స్థిర శూన్య multi_clear_timeout(నిర్మాణం Curl_easy *దత్తాంశం, expire_id eid)
{
  నిర్మాణం expire_timers *timeouts = &దత్తాంశం->state.timeouts;
  uint8_t orig_మొదటి = timeouts->మొదటి;
  uint8_t *anchor = &timeouts->మొదటి;
  uint8_t id = (uint8_t)eid;

  ఒకవేళ((గుర్తులేని)eid >= EXPIRE_చివరి) {
    DEBUGASSERT(0);
    తిరిగి;
  }

  అయితే(*anchor < EXPIRE_చివరి) {
    ఒకవేళ(*anchor == id) {
      *anchor = timeouts->తదుపరి[id];
      ఆపు;
    }
    anchor = &timeouts->తదుపరి[*anchor];
  }
  DEBUGASSERT(multi_timeouts_check(దత్తాంశం));
  ఒకవేళ(Curl_timeouts_has(దత్తాంశం)) {
    నిర్మాణం Curl_multi *multi = దత్తాంశం->multi;

    ఒకవేళ(!multi) {
      DEBUGASSERT(0);
      తిరిగి;
    }
    ఒకవేళ((timeouts->మొదటి >= EXPIRE_చివరి) || /* no more timeouts */
       (timeouts->మొదటి != orig_మొదటి)) {  /* active timeout changed */
      Curl_timeouts_remove(&multi->timeouts, దత్తాంశం);
    }
    ఒకవేళ((timeouts->మొదటి < EXPIRE_చివరి) && !Curl_timeouts_has(దత్తాంశం)) {
      Curl_timeouts_కూడు(&multi->timeouts, దత్తాంశం,
                        timeouts->స్థానభ్రంశం_us[timeouts->మొదటి]);
    }
  }
}

/*
 * Add a timestamp to the list of timeouts. Keep the list sorted so that head
 * of list is always the timeout nearest in time.
 */
స్థిర CURLMcode multi_సమితి_timeout(నిర్మాణం Curl_easy *దత్తాంశం,
                                   స్థిరరాశి నిర్మాణం curltime *stamp,
                                   expire_id eid)
{
  నిర్మాణం expire_timers *timeouts = &దత్తాంశం->state.timeouts;
  uint8_t *anchor = &timeouts->మొదటి;
  uint8_t id = (uint8_t)eid;

  ఒకవేళ((గుర్తులేని)eid >= EXPIRE_చివరి) {
    DEBUGASSERT(0);
    తిరిగి CURLM_BAD_FUNCTION_వాదం;
  }
  /* remove from list, store time and re-insert */
  multi_clear_timeout(దత్తాంశం, eid);
  timeouts->స్థానభ్రంశం_us[id] =
    Curl_timeouts_స్థానభ్రంశం_us(&దత్తాంశం->multi->timeouts, stamp);

  అయితే(*anchor < EXPIRE_చివరి) {
    ఒకవేళ(timeouts->స్థానభ్రంశం_us[*anchor] > timeouts->స్థానభ్రంశం_us[id])
      ఆపు;
    anchor = &timeouts->తదుపరి[*anchor];
  }
  timeouts->తదుపరి[eid] = *anchor;
  timeouts->తదుపరి[eid] = *anchor;
  *anchor = id;
  DEBUGASSERT(multi_timeouts_check(దత్తాంశం));
  CURL_TRC_TIMER(దత్తాంశం, eid, "set for %" FMT_TIMEDIFF_T "us",
                 curlx_ptimediff_us(stamp, Curl_pgrs_now(దత్తాంశం)));
  తిరిగి CURLM_OK;
}

/*
 * given a number of milliseconds from now to use to set the 'act before
 * this'-time for the transfer, to be extracted by curl_multi_timeout()
 *
 * The timeout will be added to a queue of timeouts if it defines a moment in
 * time that is later than the current head of queue.
 *
 * Expire replaces a former timeout using the same id if already set.
 */
శూన్య Curl_expire_సమితి(నిర్మాణం Curl_easy *దత్తాంశం,
                     expire_id eid, timediff_t ms,
                     స్థిరరాశి నిర్మాణం curltime *pnow)
{
  నిర్మాణం Curl_multi *multi = దత్తాంశం->multi;
  నిర్మాణం expire_timers *timeouts = &దత్తాంశం->state.timeouts;
  uint8_t prev_id = timeouts->మొదటి;
  నిర్మాణం curltime సమితి;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  ఒకవేళ(!multi)
    తిరిగి;
  DEBUGASSERT(eid < EXPIRE_చివరి);
  ఒకవేళ(ms > గరిష్ఠ_పూర్ణాంకం)
    /* Cap ridiculous timeouts, 31-bit ms is still 3.5 weeks. When the time
       goes to the user, it must fit in this size. */
    ms = గరిష్ఠ_పూర్ణాంకం;

  సమితి = *pnow;
  సమితి.tv_sec += (time_t)(ms / 1000); /* may be a 64 to 32-bit conversion */
  సమితి.tv_usec += (పూర్ణాంకం)(ms % 1000) * 1000;
  ఒకవేళ(సమితి.tv_usec >= 1000000) {
    సమితి.tv_sec++;
    సమితి.tv_usec -= 1000000;
  }

  /* Add the timeout, will replace any previous value for this timer. */
  multi_సమితి_timeout(దత్తాంశం, &సమితి, eid);
  DEBUGASSERT(timeouts->మొదటి < EXPIRE_చివరి);

  ఒకవేళ(Curl_timeouts_has(దత్తాంశం)) {
    /* data has already a timeout registered. If the first timer
     * was NOT the one we just set AND is still the first one,
     * nothing changed from the timeouts point of view. The
     * set timer triggers after the one already registered. Leave. */
    ఒకవేళ((prev_id != eid) && (prev_id == timeouts->మొదటి))
      తిరిగి;

    /* Since this is an updated time, we must remove data from
     * timeouts and then add it again. */
    Curl_timeouts_remove(&multi->timeouts, దత్తాంశం);
  }

  /* Insert the new timer expiry since it is our local minimum. */
  Curl_timeouts_కూడు(&multi->timeouts, దత్తాంశం,
                    timeouts->స్థానభ్రంశం_us[timeouts->మొదటి]);
}

శూన్య Curl_expire(నిర్మాణం Curl_easy *దత్తాంశం,
                 timediff_t milli, expire_id eid)
{
  Curl_expire_సమితి(దత్తాంశం, eid, milli, Curl_pgrs_now(దత్తాంశం));
}

/*
 * Removes the expire timer. Marks it as done.
 */
శూన్య Curl_expire_clear(నిర్మాణం Curl_easy *దత్తాంశం, expire_id eid)
{
  /* remove the timer, if there */
  multi_clear_timeout(దత్తాంశం, eid);
  CURL_TRC_TIMER(దత్తాంశం, eid, "cleared");
}

/*
 * Clear ALL timeout values for this handle.
 */
శూన్య Curl_expire_clear_all(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_multi *multi = దత్తాంశం->multi;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  ఒకవేళ(!multi)
    తిరిగి;

  ఒకవేళ(Curl_timeouts_remove(&multi->timeouts, దత్తాంశం)) {
    /* Since this is an cleared time, we must remove the previous entry from
       the splay tree */
    multi_timeouts_init(దత్తాంశం);

    ఒకవేళ(దత్తాంశం->id >= 0)
      CURL_TRC_M(దత్తాంశం, "[TIMEOUT] all cleared");
  }
}

CURLMcode curl_multi_కేటాయించు(CURLM *m, curl_socket_t sockfd,
                            శూన్య *sockp)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_కేటాయించు, &mresult)) {
    mresult = Curl_multi_ev_కేటాయించు(m, sockfd, sockp);
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

స్థిర శూన్య move_pending_గమ్యం_connect(నిర్మాణం Curl_multi *multi,
                                    నిర్మాణం Curl_easy *దత్తాంశం)
{
  DEBUGASSERT(దత్తాంశం->mstate == MSTATE_PENDING);

  /* Remove this node from the pending set, add into process set */
  Curl_uint32_bset_remove(&multi->pending, దత్తాంశం->mid);
  Curl_uint32_bset_కూడు(&multi->process, దత్తాంశం->mid);

  multistate(దత్తాంశం, MSTATE_CONNECT);
  Curl_multi_mark_dirty(దత్తాంశం); /* make it run */
}

/* multi_schedule_pending() moves a handle from PENDING back into the process
   list and change state to CONNECT.

   We do not move all transfers because that can be a significant amount.
   Since this is tried every now and then doing too many too often becomes a
   performance problem.

   When there is a change for connection limits like max host connections etc,
   this likely only allows one new transfer. When there is a pipewait change,
   it can potentially allow hundreds of new transfers.

   We could consider an improvement where we store the queue reason and allow
   more pipewait rechecks than others. */
స్థిర శూన్య multi_కాలపట్టిక_pending(నిర్మాణం Curl_multi *multi)
{
  uint32_t mid = multi->చివరి_pending_mid;

  ఒకవేళ(mid) {
    అయితే(Curl_uint32_bset_తదుపరి(&multi->pending, mid, &mid)) {
      నిర్మాణం Curl_easy *దత్తాంశం = Curl_multi_get_easy(multi, mid);
      ఒకవేళ(దత్తాంశం) {
        move_pending_గమ్యం_connect(multi, దత్తాంశం);
        multi->చివరి_pending_mid = mid;
        తిరిగి;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    }
    /* found no pending transfers with `mid` larger than `last_pending_mid`.
     * Start at the beginning of the pending set again. */
    multi->చివరి_pending_mid = 0;
  }

  ఒకవేళ(Curl_uint32_bset_మొదటి(&multi->pending, &mid)) {
    చేయి {
      నిర్మాణం Curl_easy *దత్తాంశం = Curl_multi_get_easy(multi, mid);
      ఒకవేళ(దత్తాంశం) {
        move_pending_గమ్యం_connect(multi, దత్తాంశం);
        multi->చివరి_pending_mid = mid;
        తిరిగి;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    } అయితే(Curl_uint32_bset_తదుపరి(&multi->pending, mid, &mid));
  }
}

uint32_t Curl_multi_max_concurrent_streams(నిర్మాణం Curl_multi *multi)
{
  DEBUGASSERT(multi);
  తిరిగి multi->max_concurrent_streams;
}

CURL **curl_multi_get_handles(CURLM *m)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURL **a = ఖాళీ;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_get_handles, ఖాళీ)) {
    నిర్మాణం Curl_multi *multi = m;
    శూన్య *నమోదు_2;
    size_t లెక్క = Curl_uint32_tbl_లెక్క(&multi->xfers);

    a = curlx_malloc(పరిమాణం(నిర్మాణం Curl_easy *) * (లెక్క + 1));
    ఒకవేళ(a) {
      గుర్తులేని పూర్ణాంకం i = 0;
      uint32_t mid;

      ఒకవేళ(Curl_uint32_tbl_మొదటి(&multi->xfers, &mid, &నమోదు_2)) {
        చేయి {
          నిర్మాణం Curl_easy *దత్తాంశం = నమోదు_2;
          DEBUGASSERT(i < లెక్క);
          ఒకవేళ(!దత్తాంశం->state.అంతర్గత)
            a[i++] = దత్తాంశం;
        } అయితే(Curl_uint32_tbl_తదుపరి(&multi->xfers, mid, &mid, &నమోదు_2));
      }
      a[i] = ఖాళీ; /* last entry is a NULL */
    }
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి a;
}

CURLMcode curl_multi_get_offt(CURLM *m,
                              CURLMinfo_offt info,
                              curl_off_t *pvalue)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_get_offt, &mresult)) {
    నిర్మాణం Curl_multi *multi = m;
    uint32_t n;

    ఒకవేళ(!pvalue) {
      mresult = CURLM_BAD_FUNCTION_వాదం;
      వెళ్ళు బహిర్గతం;
    }

    ఎంపిక(info) {
    స్థితి CURLMINFO_XFERS_CURRENT:
      n = Curl_uint32_tbl_లెక్క(&multi->xfers);
      ఒకవేళ(n && multi->admin)
        --n;
      *pvalue = (curl_off_t)n;
      ఆపు;
    స్థితి CURLMINFO_XFERS_RUNNING:
      n = Curl_uint32_bset_లెక్క(&multi->process);
      ఒకవేళ(n && Curl_uint32_bset_కలిగియుంది(&multi->process, multi->admin->mid))
        --n;
      *pvalue = (curl_off_t)n;
      ఆపు;
    స్థితి CURLMINFO_XFERS_PENDING:
      *pvalue = (curl_off_t)Curl_uint32_bset_లెక్క(&multi->pending);
      ఆపు;
    స్థితి CURLMINFO_XFERS_పూర్తి:
      *pvalue = (curl_off_t)Curl_uint32_bset_లెక్క(&multi->msgsent);
      ఆపు;
    స్థితి CURLMINFO_XFERS_ADDED:
      *pvalue = multi->xfers_మొత్తం_ever;
      ఆపు;
    అప్రమేయ:
      *pvalue = -1;
      mresult = CURLM_UNKNOWN_OPTION;
      ఆపు;
    }
  }
బహిర్గతం:
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

స్థిర నిర్మాణం Curl_fixed_buf *fixed_buf_create(size_t len)
{
  నిర్మాణం Curl_fixed_buf *fbuf;
  ఒకవేళ((SIZE_MAX - పరిమాణం(*fbuf)) < len)
    తిరిగి ఖాళీ; /* too large */
  fbuf = curlx_malloc(len + పరిమాణం(*fbuf));
  ఒకవేళ(fbuf)
    fbuf->len = len;
  తిరిగి fbuf;
}

CURLcode Curl_multi_xfer_buf_borrow(నిర్మాణం Curl_easy *దత్తాంశం,
                                    అక్షరం **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->multi);
  *pbuf = ఖాళీ;
  *pbuflen = 0;
  ఒకవేళ(!దత్తాంశం->multi) {
    failf(దత్తాంశం, "transfer has no multi handle");
    తిరిగి CURLE_FAILED_INIT;
  }
  ఒకవేళ(!దత్తాంశం->సమితి.బఫరు_కొలత) {
    failf(దత్తాంశం, "transfer buffer size is 0");
    తిరిగి CURLE_FAILED_INIT;
  }
  ఒకవేళ(దత్తాంశం->multi->xfer_buf_borrowed) {
    failf(దత్తాంశం, "attempt to borrow xfer_buf when already borrowed");
    తిరిగి CURLE_AGAIN;
  }

  ఒకవేళ(దత్తాంశం->multi->xfer_buf &&
     దత్తాంశం->సమితి.బఫరు_కొలత > దత్తాంశం->multi->xfer_buf->len) {
    /* not large enough, get a new one */
    curlx_safefree(దత్తాంశం->multi->xfer_buf);
  }

  ఒకవేళ(!దత్తాంశం->multi->xfer_buf) {
    దత్తాంశం->multi->xfer_buf =
      fixed_buf_create(curlx_uitouz(దత్తాంశం->సమితి.బఫరు_కొలత));
    ఒకవేళ(!దత్తాంశం->multi->xfer_buf) {
      failf(దత్తాంశం, "could not allocate xfer_buf of %u bytes",
            దత్తాంశం->సమితి.బఫరు_కొలత);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }

  దత్తాంశం->multi->xfer_buf_borrowed = TRUE;
  *pbuf = దత్తాంశం->multi->xfer_buf->దత్తాంశం;
  *pbuflen = దత్తాంశం->multi->xfer_buf->len;
  తిరిగి CURLE_OK;
}

శూన్య Curl_multi_xfer_buf_release(నిర్మాణం Curl_easy *దత్తాంశం, అక్షరం *buf)
{
  (శూన్య)buf;
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->multi);
  DEBUGASSERT(!buf || (దత్తాంశం->multi->xfer_buf &&
                       దత్తాంశం->multi->xfer_buf->దత్తాంశం == buf));
  దత్తాంశం->multi->xfer_buf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_ulbuf_borrow(నిర్మాణం Curl_easy *దత్తాంశం,
                                      అక్షరం **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->multi);
  *pbuf = ఖాళీ;
  *pbuflen = 0;
  ఒకవేళ(!దత్తాంశం->multi) {
    failf(దత్తాంశం, "transfer has no multi handle");
    తిరిగి CURLE_FAILED_INIT;
  }
  ఒకవేళ(దత్తాంశం->multi->xfer_ulbuf_borrowed) {
    failf(దత్తాంశం, "attempt to borrow xfer_ulbuf when already borrowed");
    తిరిగి CURLE_AGAIN;
  }

  ఒకవేళ(దత్తాంశం->multi->xfer_ulbuf &&
     దత్తాంశం->సమితి.upload_బఫరు_కొలత > దత్తాంశం->multi->xfer_ulbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(దత్తాంశం->multi->xfer_ulbuf);
  }

  ఒకవేళ(!దత్తాంశం->multi->xfer_ulbuf) {
    దత్తాంశం->multi->xfer_ulbuf =
      fixed_buf_create(curlx_uitouz(దత్తాంశం->సమితి.upload_బఫరు_కొలత));
    ఒకవేళ(!దత్తాంశం->multi->xfer_ulbuf) {
      failf(దత్తాంశం, "could not allocate xfer_ulbuf of %u bytes",
            దత్తాంశం->సమితి.upload_బఫరు_కొలత);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }

  దత్తాంశం->multi->xfer_ulbuf_borrowed = TRUE;
  *pbuf = దత్తాంశం->multi->xfer_ulbuf->దత్తాంశం;
  *pbuflen = దత్తాంశం->multi->xfer_ulbuf->len;
  తిరిగి CURLE_OK;
}

శూన్య Curl_multi_xfer_ulbuf_release(నిర్మాణం Curl_easy *దత్తాంశం, అక్షరం *buf)
{
  (శూన్య)buf;
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->multi);
  DEBUGASSERT(!buf || (దత్తాంశం->multi->xfer_ulbuf &&
                       దత్తాంశం->multi->xfer_ulbuf->దత్తాంశం == buf));
  దత్తాంశం->multi->xfer_ulbuf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_sockbuf_borrow(నిర్మాణం Curl_easy *దత్తాంశం,
                                        size_t blen, అక్షరం **pbuf)
{
  DEBUGASSERT(దత్తాంశం);
  *pbuf = ఖాళీ;
  ఒకవేళ(!దత్తాంశం->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    *pbuf = curlx_malloc(blen);
    తిరిగి *pbuf ? CURLE_OK : CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  ఒకవేళ(దత్తాంశం->multi->xfer_sockbuf_borrowed) {
    failf(దత్తాంశం, "attempt to borrow xfer_sockbuf when already borrowed");
    తిరిగి CURLE_AGAIN;
  }

  ఒకవేళ(దత్తాంశం->multi->xfer_sockbuf && blen > దత్తాంశం->multi->xfer_sockbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(దత్తాంశం->multi->xfer_sockbuf);
  }

  ఒకవేళ(!దత్తాంశం->multi->xfer_sockbuf) {
    దత్తాంశం->multi->xfer_sockbuf = fixed_buf_create(blen);
    ఒకవేళ(!దత్తాంశం->multi->xfer_sockbuf) {
      failf(దత్తాంశం, "could not allocate xfer_sockbuf of %zu bytes", blen);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }

  దత్తాంశం->multi->xfer_sockbuf_borrowed = TRUE;
  *pbuf = దత్తాంశం->multi->xfer_sockbuf->దత్తాంశం;
  తిరిగి CURLE_OK;
}

శూన్య Curl_multi_xfer_sockbuf_release(నిర్మాణం Curl_easy *దత్తాంశం, అక్షరం *buf)
{
  DEBUGASSERT(దత్తాంశం);
  ఒకవేళ(!దత్తాంశం->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    curlx_free(buf);
  }
  లేకపోతే {
    DEBUGASSERT(!buf || (దత్తాంశం->multi->xfer_sockbuf &&
                         దత్తాంశం->multi->xfer_sockbuf->దత్తాంశం == buf));
    దత్తాంశం->multi->xfer_sockbuf_borrowed = FALSE;
  }
}

స్థిర శూన్య multi_xfer_bufs_free(నిర్మాణం Curl_multi *multi)
{
  DEBUGASSERT(multi);
  curlx_safefree(multi->xfer_buf);
  multi->xfer_buf_borrowed = FALSE;
  curlx_safefree(multi->xfer_ulbuf);
  multi->xfer_ulbuf_borrowed = FALSE;
  curlx_safefree(multi->xfer_sockbuf);
  multi->xfer_sockbuf_borrowed = FALSE;
}

నిర్మాణం Curl_easy *Curl_multi_get_easy(నిర్మాణం Curl_multi *multi,
                                      uint32_t mid)
{
  నిర్మాణం Curl_easy *దత్తాంశం = Curl_uint32_tbl_get(&multi->xfers, mid);
  ఒకవేళ(GOOD_EASY_HANDLE(దత్తాంశం))
    తిరిగి దత్తాంశం;
  CURL_TRC_M(multi->admin, "invalid easy handle in xfer table for mid=%u",
             mid);
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  తిరిగి ఖాళీ;
}

bool Curl_multi_knows_easy(నిర్మాణం Curl_multi *multi, నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి Curl_uint32_tbl_get(&multi->xfers, దత్తాంశం->mid) == దత్తాంశం;
}

స్థిర uint32_t multi_xfers_running(నిర్మాణం Curl_multi *multi)
{
  ఒకవేళ(!multi) {
    DEBUGASSERT(0);
    తిరిగి 0;
  }
  తిరిగి multi->xfers_alive;
}

uint32_t Curl_multi_xfers_attached(నిర్మాణం Curl_multi *multi)
{
  ఒకవేళ(!multi || !multi->admin) {
    DEBUGASSERT(0);
    తిరిగి 0;
  }
  /* Discount the admin handle */
  తిరిగి Curl_uint32_tbl_లెక్క(&multi->xfers) - 1;
}

శూన్య Curl_multi_mark_dirty(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(దత్తాంశం->multi && దత్తాంశం->mid != UINT32_MAX)
    Curl_uint32_bset_కూడు(&దత్తాంశం->multi->dirty, దత్తాంశం->mid);
}

శూన్య Curl_multi_clear_dirty(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(దత్తాంశం->multi && దత్తాంశం->mid != UINT32_MAX)
    Curl_uint32_bset_remove(&దత్తాంశం->multi->dirty, దత్తాంశం->mid);
}

CURLMcode curl_multi_notify_enable(CURLM *m, గుర్తులేని పూర్ణాంకం notification)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_notify_enable, &mresult)) {
    mresult = Curl_mntfy_enable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

CURLMcode curl_multi_notify_disable(CURLM *m, గుర్తులేని పూర్ణాంకం notification)
{
  నిర్మాణం Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ఒకవేళ(CURL_MAPI_ENTER(&guard, m, multi_notify_disable, &mresult)) {
    mresult = Curl_mntfy_disable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  తిరిగి mresult;
}

#నిర్వచితమైతే DEBUGBUILD
స్థిర శూన్య multi_xfer_dump(నిర్మాణం Curl_multi *multi, uint32_t mid,
                            శూన్య *నమోదు_2)
{
  నిర్మాణం Curl_easy *దత్తాంశం = నమోదు_2;

  (శూన్య)multi;
  ఒకవేళ(!దత్తాంశం) {
    curl_mfprintf(stderr, "mid=%u, entry=NULL, bug in xfer table?\n", mid);
  }
  లేకపోతే {
    curl_mfprintf(stderr, "mid=%u, magic=%s, p=%p, id=%" FMT_OFF_T
                  ", url=%s\n",
                  mid,
                  (దత్తాంశం->magic == CURLEASY_MAGIC_సంఖ్య) ? "GOOD" : "BAD!",
                  (శూన్య *)దత్తాంశం, దత్తాంశం->id, Curl_bufref_ptr(&దత్తాంశం->state.url));
  }
}

స్థిర శూన్య multi_xfer_tbl_dump(నిర్మాణం Curl_multi *multi)
{
  uint32_t mid;
  శూన్య *నమోదు_2;
  curl_mfprintf(stderr, "=== multi xfer table (count=%u, capacity=%u\n",
                Curl_uint32_tbl_లెక్క(&multi->xfers),
                Curl_uint32_tbl_సామర్థ్యం(&multi->xfers));
  ఒకవేళ(Curl_uint32_tbl_మొదటి(&multi->xfers, &mid, &నమోదు_2)) {
    multi_xfer_dump(multi, mid, నమోదు_2);
    అయితే(Curl_uint32_tbl_తదుపరి(&multi->xfers, mid, &mid, &నమోదు_2))
      multi_xfer_dump(multi, mid, నమోదు_2);
  }
  curl_mfprintf(stderr, "===\n");
  fflush(stderr);
}
#అయితే_ముగింపు /* DEBUGBUILD */
