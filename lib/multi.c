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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "curl_share.h"
#অন্তর্ভাব "psl.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "multi_ev.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "http.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "curlx/wait.h"
#অন্তর্ভাব "conncache.h"
#অন্তর্ভাব "multihandle.h"
#অন্তর্ভাব "sigpipe.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "http_proxy.h"
#অন্তর্ভাব "http2.h"
#অন্তর্ভাব "socketpair.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "curlx/strparse.h"

/* initial multi->xfers table size for a full multi */
#define CURL_XFER_TABLE_আকার 128

/* CURL_SOCKET_HASH_TABLE_SIZE should be a prime number. Increasing it from 97
   to 911 takes on a 32-bit machine 4 x 804 = 3211 more bytes. Still, every
   curl handle takes 6K memory, therefore this 3K are not significant. */
#ifndef CURL_SOCKET_HASH_TABLE_আকার
#define CURL_SOCKET_HASH_TABLE_আকার 911
#endif

#ifndef CURL_CONNECTION_HASH_আকার
#define CURL_CONNECTION_HASH_আকার 97
#endif

#ifndef CURL_DNS_HASH_আকার
#define CURL_DNS_HASH_আকার 71
#endif

#ifndef CURL_TLS_SESSION_আকার
#define CURL_TLS_SESSION_আকার 25
#endif

স্থির শূন্য move_pending_to_connect(কাঠামো Curl_multi *multi,
                                    কাঠামো Curl_easy *তথ্য);
স্থির CURLMcode add_পরবর্তী_timeout(ধ্রুবক কাঠামো curltime *pnow,
                                  কাঠামো Curl_multi *multi,
                                  কাঠামো Curl_easy *তথ্য);
স্থির শূন্য multi_timeout(কাঠামো Curl_multi *multi,
                          timediff_t *pexire_offset_us,
                          পূর্ণসংখ্যা *timeout_ms);
স্থির শূন্য multi_schedule_pending(কাঠামো Curl_multi *multi);
স্থির শূন্য multi_xfer_bufs_free(কাঠামো Curl_multi *multi);
#ifdef DEBUGBUILD
স্থির শূন্য multi_xfer_tbl_dump(কাঠামো Curl_multi *multi);
#endif
/* Get the # of transfers current in process/pending. */
স্থির uint32_t multi_xfers_running(কাঠামো Curl_multi *multi);

/* function pointer called once when entering a state */
প্রকারলেখ শূন্য (*mstate_enter_func)(কাঠামো Curl_easy *তথ্য,
                                  CURLMstate from_state);

স্থির শূন্য mstate_enter_connect(কাঠামো Curl_easy *তথ্য,
                                 CURLMstate from_state)
{
  (শূন্য)from_state;
  Curl_init_CONNECT(তথ্য);
}

স্থির শূন্য mstate_enter_did(কাঠামো Curl_easy *তথ্য,
                             CURLMstate from_state)
{
  (শূন্য)from_state;
  তথ্য->req.chunk = FALSE;
  Curl_pgrsTime(তথ্য, TIMER_PRETRANSFER);
  যদি(!CURL_REQ_WANT_SEND(তথ্য))
    Curl_pgrsTime(তথ্য, TIMER_POSTRANSFER);
}

স্থির শূন্য mstate_enter_done(কাঠামো Curl_easy *তথ্য,
                              CURLMstate from_state)
{
  (শূন্য)from_state;
  CURLM_NTFY(তথ্য, CURLMNOTIFY_EASY_DONE);
}

স্থির শূন্য mstate_enter_completed(কাঠামো Curl_easy *তথ্য,
                                   CURLMstate from_state)
{
  /* we sometimes directly jump to COMPLETED, trigger things
   * we then missed. */
  যদি(from_state < MSTATE_DID) {
    Curl_pgrsTime(তথ্য, TIMER_PRETRANSFER);
    Curl_pgrsTime(তথ্য, TIMER_POSTRANSFER);
    Curl_pgrsTime(তথ্য, TIMER_STARTTRANSFER);
  }
  /* multi_done() should have run and detached any connection already */
  DEBUGASSERT(!তথ্য->conn);
  Curl_pgrsCompleted(তথ্য);
  যদি(from_state < MSTATE_DONE)
    CURLM_NTFY(তথ্য, CURLMNOTIFY_EASY_DONE);
  /* changing to COMPLETED means it is in process and needs to go */
  DEBUGASSERT(Curl_uint32_bset_contains(&তথ্য->multi->process, তথ্য->mid));
  Curl_uint32_bset_remove(&তথ্য->multi->process, তথ্য->mid);
  Curl_uint32_bset_remove(&তথ্য->multi->pending, তথ্য->mid); /* to be sure */

  যদি(Curl_uint32_bset_empty(&তথ্য->multi->process)) {
    /* free the transfer buffer when we have no more active transfers */
    multi_xfer_bufs_free(তথ্য->multi);
  }
  /* Important: reset the conn pointer so that we do not point to memory
     that could be freed anytime */
  Curl_detach_connection(তথ্য);
  Curl_expire_clear_all(তথ্য); /* stop all timers */
}

/* always use this function to change state, to make debugging easier */
স্থির শূন্য mstate(কাঠামো Curl_easy *তথ্য, CURLMstate state
#ifdef DEBUGBUILD
                   , পূর্ণসংখ্যা lineno
#endif
)
{
  CURLMstate oldstate = তথ্য->mstate;
  স্থির ধ্রুবক mstate_enter_func state_enter[MSTATE_LAST] = {
    ফাঁকা,                      /* INIT */
    ফাঁকা,                      /* PENDING */
    ফাঁকা,                      /* SETUP */
    mstate_enter_connect,      /* CONNECT */
    ফাঁকা,                      /* CONNECTING */
    ফাঁকা,                      /* PROTOCONNECT */
    ফাঁকা,                      /* PROTOCONNECTING */
    ফাঁকা,                      /* DO */
    ফাঁকা,                      /* DOING */
    ফাঁকা,                      /* DOING_MORE */
    mstate_enter_did,          /* DID */
    ফাঁকা,                      /* PERFORMING */
    ফাঁকা,                      /* RATELIMITING */
    mstate_enter_done,         /* DONE */
    mstate_enter_completed,    /* COMPLETED */
    ফাঁকা                       /* MSGSENT */
  };

  যদি(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    ফেরত;

#ifdef DEBUGBUILD
  NOVERBOSE((শূন্য)lineno);
  CURL_TRC_M(তথ্য, "-> [%s] (line %d)", CURL_MSTATE_NAME(state), lineno);
#অন্যথা
  CURL_TRC_M(তথ্য, "-> [%s]", CURL_MSTATE_NAME(state));
#endif

  /* really switching state */
  তথ্য->mstate = state;
  যদি(state_enter[state])
    state_enter[state](তথ্য, oldstate);
}

#ifndef DEBUGBUILD
#define multistate(x, y) mstate(x, y)
#অন্যথা
#define multistate(x, y) mstate(x, y, __LINE__)
#endif

/* multi->proto_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
স্থির শূন্য ph_freeentry(শূন্য *p)
{
  (শূন্য)p;
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
স্থির শূন্য multi_addmsg(কাঠামো Curl_multi *multi, কাঠামো Curl_easy *তথ্য)
{
  যদি(Curl_uint32_bset_empty(&multi->msgsent))
    CURLM_NTFY(multi->admin, CURLMNOTIFY_INFO_READ);
  Curl_uint32_bset_add(&multi->msgsent, তথ্য->mid);
}

স্থির শূন্য multi_timeouts_init(কাঠামো Curl_easy *তথ্য);

কাঠামো Curl_multi *Curl_multi_handle(uint32_t xfer_table_আকার,
                                     size_t ev_hashsize,  /* event hash */
                                     size_t chashsize, /* connection hash */
                                     size_t dnssize,   /* dns hash */
                                     size_t sesssize)  /* TLS session cache */
{
  কাঠামো curltime created;
  কাঠামো Curl_multi *multi = curlx_calloc(1, আকার(কাঠামো Curl_multi));

  যদি(!multi)
    ফেরত ফাঁকা;

  multi->magic = CURLMULTI_MAGIC_সংখ্যা;

  /* Initialisation order is important here!
   * easy_init() does a lazy check on curl_global_init() which sets
   * up platform specific things we need. For example calling curlx_pnow()
   * before this is not safe. */
  multi->admin = curl_easy_init();
  যদি(!multi->admin) {
    curlx_free(multi);
    ফেরত ফাঁকা;
  }
  multi->admin->multi = multi;
  multi->admin->state.internal = TRUE;

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
  Curl_hash_init(&multi->proto_hash, 23, CURL_HASH_TYPE_BYTES, ph_freeentry);
  Curl_cshutdn_init(&multi->cshutdn);

  multi->multiplexing = TRUE;
  multi->max_concurrent_streams = 100;
#ifdef ENABLE_WAKEUP
  multi->wakeup_pair[0] = CURL_SOCKET_BAD;
  multi->wakeup_pair[1] = CURL_SOCKET_BAD;
#endif
#ifdef ENABLE_INTERNAL_WAKEUP
  multi->wakeup_internal[0] = CURL_SOCKET_BAD;
  multi->wakeup_internal[1] = CURL_SOCKET_BAD;
#endif

  যদি(Curl_uint32_bset_resize(&multi->process, xfer_table_আকার) ||
     Curl_uint32_bset_resize(&multi->pending, xfer_table_আকার) ||
     Curl_uint32_bset_resize(&multi->dirty, xfer_table_আকার) ||
     Curl_uint32_bset_resize(&multi->msgsent, xfer_table_আকার) ||
     Curl_uint32_tbl_resize(&multi->xfers, xfer_table_আকার))
    যাও error;

#ifdef DEBUGBUILD
  {
    ধ্রুবক অক্ষর *p;
    যদি(getenv("CURL_DEBUG"))
      multi->admin->set.verbose = TRUE;
    p = getenv("CURL_DBG_MAX_TOTAL_CONNECTIONS");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, UINT32_MAX)) {
        multi->max_যোগ_connections = (uint32_t)l;
      }
    }
  }
#endif
  Curl_uint32_tbl_add(&multi->xfers, multi->admin, &multi->admin->mid);
  Curl_uint32_bset_add(&multi->process, multi->admin->mid);

  Curl_cpool_init(&multi->cpool, ফাঁকা, chashsize);

#ifdef USE_SSL
  যদি(Curl_ssl_scache_create(sesssize, 2, &multi->ssl_scache))
    যাও error;
#অন্যথা
  (শূন্য)sesssize;
#endif

#ifdef USE_WINSOCK
  multi->wsa_event = WSACreateEvent();
  যদি(multi->wsa_event == WSA_INVALID_EVENT)
    যাও error;
#endif
#ifdef ENABLE_WAKEUP
  /* When enabled, rely on this to work. We ignore this in previous
   * versions, but that seems an unnecessary complication. */
  যদি(Curl_wakeup_init(multi->wakeup_pair, TRUE) < 0)
    যাও error;
#endif
#ifdef ENABLE_INTERNAL_WAKEUP
  যদি(Curl_wakeup_init(multi->wakeup_internal, TRUE) < 0)
    যাও error;
#endif

  যদি(Curl_probeipv6(multi))
    যাও error;

#ifdef USE_RESOLV_THREADED
  যদি(xfer_table_আকার < CURL_XFER_TABLE_আকার) { /* easy multi */
    যদি(Curl_async_thrdd_multi_init(multi, 0, 2, 10))
      যাও error;
  }
  অন্যথা { /* real multi handle */
    যদি(Curl_async_thrdd_multi_init(multi, 0, 20, 2000))
      যাও error;
  }
#endif

  ফেরত multi;

error:

#ifdef USE_RESOLV_THREADED
  Curl_async_thrdd_multi_destroy(multi, TRUE);
#endif
  Curl_multi_ev_cleanup(multi);
  Curl_hash_destroy(&multi->proto_hash);
  Curl_dnscache_destroy(&multi->dnscache);
  Curl_cpool_destroy(&multi->cpool, multi->admin);
  Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
#ifdef USE_SSL
  Curl_ssl_scache_destroy(multi->ssl_scache);
#endif
  যদি(multi->admin) {
    Curl_multi_ev_xfer_done(multi, multi->admin);
    multi->admin->multi = ফাঁকা;
    Curl_close(&multi->admin);
  }
  Curl_mntfy_cleanup(multi);

  Curl_uint32_bset_destroy(&multi->process);
  Curl_uint32_bset_destroy(&multi->dirty);
  Curl_uint32_bset_destroy(&multi->pending);
  Curl_uint32_bset_destroy(&multi->msgsent);
  Curl_uint32_tbl_destroy(&multi->xfers);
#ifdef ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#endif
#ifdef ENABLE_INTERNAL_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_internal);
#endif

  curlx_free(multi);
  ফেরত ফাঁকা;
}

CURLM *curl_multi_init(শূন্য)
{
  ফেরত Curl_multi_handle(CURL_XFER_TABLE_আকার,
                           CURL_SOCKET_HASH_TABLE_আকার,
                           CURL_CONNECTION_HASH_আকার,
                           CURL_DNS_HASH_আকার,
                           CURL_TLS_SESSION_আকার);
}

#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
স্থির শূন্য multi_warn_debug(কাঠামো Curl_multi *multi, কাঠামো Curl_easy *তথ্য)
{
  যদি(!multi->warned) {
    infof(তথ্য, "!!! WARNING !!!");
    infof(তথ্য, "This is a debug build of libcurl, "
                "do not use in production.");
    multi->warned = TRUE;
  }
}
#অন্যথা
#define multi_warn_debug(x, y) Curl_nop_stmt
#endif

bool Curl_is_connecting(কাঠামো Curl_easy *তথ্য)
{
  ফেরত তথ্য->mstate < MSTATE_DO;
}

স্থির CURLMcode multi_assess_wakeup(কাঠামো Curl_multi *multi)
{
#ifdef ENABLE_INTERNAL_WAKEUP
  যদি(multi->socket_cb)
    ফেরত Curl_multi_ev_assess_xfer(multi, multi->admin);
#অন্যথা
  (শূন্য)multi;
#endif
  ফেরত CURLM_OK;
}

স্থির CURLMcode multi_xfers_add(কাঠামো Curl_multi *multi,
                                 কাঠামো Curl_easy *তথ্য)
{
  uint32_t capacity = Curl_uint32_tbl_capacity(&multi->xfers);
  uint32_t new_আকার = 0;
  /* Prepare to make this into a CURLMOPT_MAX_TRANSFERS, because some
   * applications may want to prevent a run-away of their memory use. */
  /* UINT_MAX is our "invalid" id, do not let the table grow up to that. */
  ধ্রুবক uint32_t max_capacity = UINT_MAX - 1;

  যদি(capacity < max_capacity) {
    /* We want `multi->xfers` to have "sufficient" free rows, so that we do
     * not have to reuse the `mid` from a removed easy right away.
     * Check if an 8th of the capacity is still free */
    uint32_t used = Curl_uint32_tbl_গুনতি(&multi->xfers);
    uint32_t unused = capacity - used;
    uint32_t min_unused = CURLMAX(capacity >> 3, 4);
    যদি(unused < min_unused) {
      /* Grow by 50% of current capacity, in range of [128, 2048],
       * which means the table grows max by 16kb on 64-bit arch. */
      uint32_t growth = CURLMIN(CURLMAX(capacity >> 1, 128), 2048);
      /* Make sure the uint arithmetic here works on the corner
       * cases where we are close to max_capacity or UINT_MAX */
      যদি((max_capacity - growth) <= capacity)
        new_আকার = max_capacity;
      অন্যথা
        new_আকার = capacity + growth;
    }
  }

  যদি(new_আকার > capacity) {
    /* Grow the bitsets first. Should one fail, we do not need
     * to downsize the already resized ones. The sets continue
     * to work properly when larger than the table, but not
     * the other way around. */
    CURL_TRC_M(তথ্য, "increasing xfer table size to %u", new_আকার);
    যদি(Curl_uint32_bset_resize(&multi->process, new_আকার) ||
       Curl_uint32_bset_resize(&multi->dirty, new_আকার) ||
       Curl_uint32_bset_resize(&multi->pending, new_আকার) ||
       Curl_uint32_bset_resize(&multi->msgsent, new_আকার) ||
       Curl_uint32_tbl_resize(&multi->xfers, new_আকার))
      ফেরত CURLM_OUT_OF_MEMORY;
  }

  /* Insert the easy into the table now */
  যদি(!Curl_uint32_tbl_add(&multi->xfers, তথ্য, &তথ্য->mid)) {
    /* MUST only happen when table is full */
    DEBUGASSERT(Curl_uint32_tbl_capacity(&multi->xfers) <=
                Curl_uint32_tbl_গুনতি(&multi->xfers));
    ফেরত CURLM_OUT_OF_MEMORY;
  }
  ফেরত CURLM_OK;
}

CURLMcode Curl_multi_add_handle(কাঠামো Curl_multi *multi,
                                কাঠামো Curl_easy *তথ্য)
{
  CURLMcode mresult;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  যদি(তথ্য->multi)
    ফেরত CURLM_ADDED_ALREADY;

  যদি(multi->dead) {
    /* a "dead" handle cannot get added transfers while any existing easy
       handles are still alive - but if there are none alive anymore, it is
       fine to start over and unmark the "deadness" of this handle.
       This means only the admin handle MUST be present. */
    যদি((Curl_uint32_tbl_গুনতি(&multi->xfers) != 1) ||
       !Curl_uint32_tbl_contains(&multi->xfers, 0))
      ফেরত CURLM_ABORTED_BY_CALLBACK;
    multi->dead = FALSE;
    Curl_uint32_bset_clear(&multi->process);
    Curl_uint32_bset_clear(&multi->dirty);
    Curl_uint32_bset_clear(&multi->pending);
    Curl_uint32_bset_clear(&multi->msgsent);
  }

  যদি(তথ্য->multi_easy) {
    /* if this easy handle was previously used for curl_easy_perform(), there
       is a private multi handle here that we can kill */
    curl_multi_cleanup(তথ্য->multi_easy);
    তথ্য->multi_easy = ফাঁকা;
  }

  /* Insert the easy into the multi->xfers table, assigning it a `mid`. */
  যদি(multi_xfers_add(multi, তথ্য))
    ফেরত CURLM_OUT_OF_MEMORY;

  /* Initialize timeouts for this handle */
  multi_timeouts_init(তথ্য);

  /*
   * No failure allowed in this function beyond this point. No modification
   * of easy nor multi handle allowed before this except for potential
   * multi's connection pool growing which will not be undone in this
   * function no matter what.
   */
  যদি(তথ্য->set.errorbuffer)
    তথ্য->set.errorbuffer[0] = 0;

  তথ্য->state.os_errno = 0;

  /* make the Curl_easy refer back to this multi handle - before
     Curl_expire() is called. */
  তথ্য->multi = multi;

  /* set the easy handle */
  multistate(তথ্য, MSTATE_INIT);
  /* not yet passed INIT state */
  তথ্য->state.really_alive = FALSE;

#ifdef USE_LIBPSL
  /* Do the same for PSL. */
  যদি(তথ্য->share && (তথ্য->share->specifier & (1 << CURL_LOCK_তথ্য_PSL)))
    তথ্য->psl = &তথ্য->share->psl;
  অন্যথা
    তথ্য->psl = &multi->psl;
#endif

  /* add the easy handle to the process set */
  Curl_uint32_bset_add(&multi->process, তথ্য->mid);
  ++multi->xfers_alive;
  ++multi->xfers_যোগ_ever;

  Curl_cpool_xfer_init(তথ্য);
  multi_warn_debug(multi, তথ্য);

  /* Make sure the new handle will run */
  Curl_multi_mark_dirty(তথ্য);

  /* Necessary in event based processing, where dirty handles trigger
   * a timeout callback invocation. */
  mresult = Curl_update_timer(multi);
  যদি(mresult) {
    /* undo the additions above */
    Curl_uint32_bset_remove(&multi->process, তথ্য->mid);
    Curl_uint32_bset_remove(&multi->dirty, তথ্য->mid);
    --multi->xfers_alive;
    তথ্য->multi = ফাঁকা; /* not anymore */
    Curl_uint32_tbl_remove(&multi->xfers, তথ্য->mid);
    তথ্য->mid = UINT32_MAX;
    ফেরত mresult;
  }

  /* The admin handle only ever has default timeouts set. To improve the
     state somewhat we clone the timeouts from each added handle so that the
     admin handle always has the same timeouts as the most recently added
     easy handle. */
  multi->admin->set.timeout = তথ্য->set.timeout;
  multi->admin->set.server_response_timeout =
    তথ্য->set.server_response_timeout;
  multi->admin->set.no_signal = তথ্য->set.no_signal;

  CURL_TRC_M(তথ্য, "added to multi, mid=%u, running=%u, total=%u",
             তথ্য->mid, multi_xfers_running(multi),
             Curl_uint32_tbl_গুনতি(&multi->xfers));
  ফেরত CURLM_OK;
}

CURLMcode curl_multi_add_handle(CURLM *m, CURL *curl)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_add_handle, &mresult)) {
    কাঠামো Curl_easy *তথ্য = curl;
    /* Verify that we got a somewhat good easy handle too */
    যদি(!GOOD_EASY_HANDLE(তথ্য) || তথ্য->state.internal)
      mresult = CURLM_BAD_EASY_HANDLE;
    অন্যথা
      mresult = Curl_multi_add_handle(m, তথ্য);
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

কাঠামো multi_done_ctx {
  BIT(premature);
};

স্থির bool multi_conn_should_close(কাঠামো connectdata *conn,
                                    কাঠামো Curl_easy *তথ্য,
                                    bool premature)
{
  /* if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of everything else. */
  যদি(conn->bits.close)
    ফেরত TRUE;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM/NEGOTIATE authentication handshake. */
  যদি(তথ্য->set.reuse_forbid
#ifdef USE_NTLM
     && !(conn->http_ntlm_state == NTLMSTATE_TYPE2 ||
          conn->proxy_ntlm_state == NTLMSTATE_TYPE2)
#endif
#ifdef USE_SPNEGO
     && !(conn->http_negotiate_state == CURL_NEGO_AUTH_RECV ||
          conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)
#endif
    )
    ফেরত TRUE;

  /* Unless this connection is for a "connect-only" transfer, it
   * needs to be closed if the protocol handler does not support reuse. */
  যদি(!তথ্য->set.connect_only && conn->scheme &&
     !(conn->scheme->flags & PROTOPT_CONN_REUSE))
    ফেরত TRUE;

  /* if premature is TRUE, it means this connection was said to be DONE before
     the entire request operation is complete and thus we cannot know in what
     state it is for reusing, so we are forced to close it. In a perfect world
     we can add code that keep track of if we really must close it here or not,
     but currently we have no such detail knowledge. */
  যদি(premature && !Curl_conn_is_multiplex(conn, FIRSTSOCKET))
    ফেরত TRUE;

  ফেরত FALSE;
}

স্থির cpool_do_ফলাফল multi_done_locked(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো connectdata *conn,
                                         শূন্য *userdata,
                                         ধ্রুবক কাঠামো curltime *pnow)
{
  কাঠামো multi_done_ctx *mdctx = userdata;

  (শূন্য)pnow;
  CURL_TRC_M(তথ্য, "multi_done_locked, in use=%u", conn->attached_xfers);
  যদি(CONN_INUSE(conn)) {
    /* Stop if still used. */
    CURL_TRC_M(তথ্য, "Connection still in use %u, no more multi_done now!",
               conn->attached_xfers);
    ফেরত CPOOL_DO_KEEP;
  }

  যদি(multi_conn_should_close(conn, তথ্য, (bool)mdctx->premature)) {
    CURL_TRC_M(তথ্য, "multi_done, terminating conn #%" FMT_OFF_T " to %s:%u, "
               "forbid=%d, close=%d, premature=%d, conn_multiplex=%d",
               conn->connection_id, conn->origin->user_hostname,
               conn->origin->port,
               তথ্য->set.reuse_forbid, conn->bits.close, mdctx->premature,
               Curl_conn_is_multiplex(conn, FIRSTSOCKET));
    connclose(conn);
    ফেরত mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  অন্যথা যদি(!Curl_conn_get_max_concurrent(তথ্য, conn, FIRSTSOCKET)) {
    CURL_TRC_M(তথ্য, "multi_done, conn #%" FMT_OFF_T " to %s:%u was shutdown"
               " by server, not reusing", conn->connection_id,
               conn->origin->user_hostname, conn->origin->port);
    connclose(conn);
    ফেরত mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  অন্যথা {
    ফেরত CPOOL_DO_IDLE;
  }
}

স্থির CURLcode multi_done(কাঠামো Curl_easy *তথ্য,
                           CURLcode status,  /* an error if this is called
                                                after an error was detected */
                           bool premature)
{
  CURLcode ফলাফল;
  কাঠামো connectdata *conn = তথ্য->conn;
  ধ্রুবক কাঠামো curltime *pnow = Curl_pgrs_now(তথ্য);

  CURL_TRC_M(তথ্য, "multi_done: status: %d prem: %d done: %d",
             (পূর্ণসংখ্যা)status, (পূর্ণসংখ্যা)premature, তথ্য->state.done);

  যদি(তথ্য->state.done) {
    /* Stop if multi_done() has already been called */
    ফেরত CURLE_OK;
  }
  তথ্য->state.done = TRUE; /* called now! */

  /* Shut down any ongoing async resolver operation. */
  Curl_resolv_shutdown_all(তথ্য);

  /* Cleanup possible redirect junk */
  curlx_safefree(তথ্য->req.newurl);
  curlx_safefree(তথ্য->req.location);

  নির্বাচন(status) {
  ক্ষেত্র CURLE_ABORTED_BY_CALLBACK:
  ক্ষেত্র CURLE_READ_ERROR:
  ক্ষেত্র CURLE_WRITE_ERROR:
    /* When we are aborted due to a callback return code it has to be counted
       as premature as there is trouble ahead if we do not. We have many
       callbacks and protocols work differently, we could potentially do this
       more fine-grained in the future. */
    premature = TRUE;
    FALLTHROUGH();
  পূর্বনির্ধারিত:
    বিরতি;
  }

  /* this calls the protocol-specific function pointer previously set */
  যদি(conn && conn->scheme->run->done && (তথ্য->mstate >= MSTATE_PROTOCONNECT))
    ফলাফল = conn->scheme->run->done(তথ্য, status, premature);
  অন্যথা
    ফলাফল = status;

  যদি(তথ্য->mstate > MSTATE_CONNECTING &&
     (ফলাফল != CURLE_ABORTED_BY_CALLBACK)) {
    /* avoid this if
     * - the transfer has not connected
     * - we already aborted by callback to avoid this calling another callback
     */
    পূর্ণসংখ্যা rc = Curl_pgrsDone(তথ্য, pnow);
    যদি(!ফলাফল && rc)
      ফলাফল = CURLE_ABORTED_BY_CALLBACK;
  }

  /* Make sure that transfer client writes are really done now. */
  ফলাফল = Curl_1st_fatal(ফলাফল, Curl_xfer_write_done(তথ্য, premature));

  যদি(!ফলাফল)
    ফলাফল = Curl_req_done(&তথ্য->req, তথ্য, premature);

  যদি(conn) {
    /* Detach connection and decide what to do with it. */
    কাঠামো multi_done_ctx mdctx;

    /* Inform connection filters that this transfer is done and they
     * need to give up any references to `data`. */
    Curl_conn_ev_তথ্য_done(তথ্য, premature);
    Curl_detach_connection(তথ্য);
    Curl_dnscache_prune(তথ্য, pnow);

    memset(&mdctx, 0, আকার(mdctx));
    mdctx.premature = premature;
    Curl_cpool_return(তথ্য, conn, multi_done_locked, &mdctx, pnow);

    যদি(তথ্য->multi)
      multi_schedule_pending(তথ্য->multi);
  }

  /* flush the netrc cache */
  Curl_netrc_cleanup(&তথ্য->state.netrc);
  ফেরত ফলাফল;
}

CURLMcode Curl_multi_remove_handle(কাঠামো Curl_multi *multi,
                                   কাঠামো Curl_easy *তথ্য)
{
  CURLMcode mresult;
  bool premature;
  uint32_t mid;

  /* Prevent users from trying to remove same easy handle more than once */
  যদি(!তথ্য->multi)
    ফেরত CURLM_OK; /* it is already removed so let's say it is fine! */

  /* Prevent users from trying to remove an easy handle from the wrong multi */
  যদি(তথ্য->multi != multi)
    ফেরত CURLM_BAD_EASY_HANDLE;

  যদি(তথ্য->mid == UINT32_MAX) {
    DEBUGASSERT(0);
    ফেরত CURLM_INTERNAL_ERROR;
  }
  যদি(Curl_uint32_tbl_get(&multi->xfers, তথ্য->mid) != তথ্য) {
    DEBUGASSERT(0);
    ফেরত CURLM_INTERNAL_ERROR;
  }

  premature = (তথ্য->mstate < MSTATE_COMPLETED);

  যদি(তথ্য->conn) {
    /* If the 'state' is not INIT or COMPLETED, we might need to do something
       nice to put the easy_handle in a good known state when this returns. */
    যদি(premature && (তথ্য->mstate > MSTATE_DO))
      streamclose(তথ্য->conn);

    /* multi_done() clears the association between the easy handle and the
       connection.
       Note that this ignores the return code because there is
       nothing really useful to do with it anyway! */
    (শূন্য)multi_done(তথ্য, তথ্য->ফলাফল, premature);
  }

  /* The timer must be shut down before data->multi is set to NULL, else
     data's splaynode would remain in the splay tree after curl_easy_cleanup is
     called. Do it after multi_done() in case that sets another time! */
  Curl_expire_clear_all(তথ্য);

  /* In MSGSENT, it was deducted from `multi->xfers_alive` already. */
  যদি(তথ্য->mstate != MSTATE_MSGSENT)
    --multi->xfers_alive;

  যদি(তথ্য->state.really_alive) {
    তথ্য->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    যদি(!multi->xfers_really_alive)
      (শূন্য)multi_assess_wakeup(multi);
  }

  Curl_wildcard_dtor(&তথ্য->wildcard);

  তথ্য->mstate = MSTATE_COMPLETED;

  /* Remove the association between the connection and the handle */
  Curl_detach_connection(তথ্য);

  /* Tell event handling that this transfer is definitely going away */
  Curl_multi_ev_xfer_done(multi, তথ্য);

  যদি(তথ্য->set.connect_only) {
    যদি(তথ্য->multi_easy) {
      যদি(তথ্য->state.last_conn_id != -1) {
        /* Mark any connect-only connection for closure */
        কাঠামো connectdata *conn;
        (শূন্য)Curl_getconnectinfo(তথ্য, &conn);
        যদি(conn && conn->bits.connect_only)
          connclose(conn);
      }
    }
    অন্যথা {
      /* This removes a handle that was part the multi interface that used
         CONNECT_ONLY, that connection is now left alive but since this handle
         has bits.close set nothing can use that connection anymore and it is
         forbidden from reuse. This easy handle cannot find the connection
         anymore once removed from the multi handle

         Better close the connection here, at once. */
      কাঠামো connectdata *conn;
      (শূন্য)Curl_getconnectinfo(তথ্য, &conn);
      যদি(conn)
        Curl_conn_close(তথ্য, conn, TRUE);
    }
  }

#ifdef USE_LIBPSL
  /* Remove the PSL association. */
  যদি(তথ্য->psl == &multi->psl)
    তথ্য->psl = ফাঁকা;
#endif

  /* clear the association to this multi handle */
  mid = তথ্য->mid;
  DEBUGASSERT(Curl_uint32_tbl_contains(&multi->xfers, mid));
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  Curl_uint32_bset_remove(&multi->process, mid);
  Curl_uint32_bset_remove(&multi->dirty, mid);
  Curl_uint32_bset_remove(&multi->pending, mid);
  Curl_uint32_bset_remove(&multi->msgsent, mid);
  তথ্য->multi = ফাঁকা;
  তথ্য->mid = UINT32_MAX;
  তথ্য->master_mid = UINT32_MAX;

  /* A pending transfer *might* be able to run now. */
  multi_schedule_pending(multi);
  mresult = Curl_update_timer(multi);
  যদি(mresult)
    ফেরত mresult;

  mresult = multi_assess_wakeup(multi);
  যদি(mresult) {
    failf(তথ্য, "error enabling wakeup listening: %d", mresult);
    ফেরত mresult;
  }

  CURL_TRC_M(তথ্য, "removed from multi, mid=%u, running=%u, total=%u",
             mid, multi_xfers_running(multi),
             Curl_uint32_tbl_গুনতি(&multi->xfers));
  ফেরত CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *curl)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_remove_handle, &mresult)) {
    কাঠামো Curl_easy *তথ্য = curl;
    যদি(!GOOD_EASY_HANDLE(তথ্য) || তথ্য->state.internal)
      mresult = CURLM_BAD_EASY_HANDLE;
    অন্যথা
      mresult = Curl_multi_remove_handle(m, তথ্য);
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

/* Return TRUE if the application asked for multiplexing */
bool Curl_multiplex_wanted(ধ্রুবক কাঠামো Curl_multi *multi)
{
  ফেরত multi && multi->multiplexing;
}

/*
 * Curl_detach_connection() removes the given transfer from the connection.
 *
 * This is the only function that should clear data->conn. This will
 * occasionally be called with the data->conn pointer already cleared.
 */
শূন্য Curl_detach_connection(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  যদি(conn) {
    /* this should never happen, prevent underflow */
    DEBUGASSERT(conn->attached_xfers);
    যদি(conn->attached_xfers) {
      conn->attached_xfers--;
      যদি(!conn->attached_xfers)
        conn->attached_multi = ফাঁকা;
    }
  }
  তথ্য->conn = ফাঁকা;
}

/*
 * Curl_attach_connection() attaches this transfer to this connection.
 *
 * This is the only function that should assign data->conn.
 * `matched == TRUE` means the transfer's properties match this
 * connection and it is not a temporary attach for maintenance.
 */
শূন্য Curl_attach_connection(কাঠামো Curl_easy *তথ্য,
                            কাঠামো connectdata *conn,
                            bool matched)
{
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(!তথ্য->conn);
  DEBUGASSERT(conn);
  DEBUGASSERT(conn->attached_xfers < UINT32_MAX);
  তথ্য->conn = conn;
  যদি(matched) {
    তথ্য->state.last_conn_id = conn->connection_id;
    তথ্য->state.last_cpid = conn->cpid;
  }
  অন্যথা
    DEBUGASSERT(!তথ্য->mid); /* admin handle */
  conn->attached_xfers++;
  /* all attached transfers must be from the same multi */
  যদি(!conn->attached_multi)
    conn->attached_multi = তথ্য->multi;
  DEBUGASSERT(conn->attached_multi == তথ্য->multi);

  যদি(conn->scheme && conn->scheme->run->attach)
    conn->scheme->run->attach(তথ্য, conn);
}

/* adjust pollset for rate limits/pauses */
স্থির CURLcode multi_adjust_pollset(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো easy_pollset *ps)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(ps->n) {
    bool send_blocked, recv_blocked;

    recv_blocked = (Curl_rlimit_avail(&তথ্য->progress.dl.rlimit, ফাঁকা) <= 0);
    send_blocked = (Curl_rlimit_avail(&তথ্য->progress.ul.rlimit, ফাঁকা) <= 0);
    যদি(send_blocked || recv_blocked) {
      পূর্ণসংখ্যা i;
      জন্য(i = 0; i <= SECONDARYSOCKET; ++i) {
        curl_socket_t sock = তথ্য->conn->sock[i];
        যদি(sock == CURL_SOCKET_BAD)
          চালু;
        যদি(recv_blocked && Curl_pollset_want_recv(তথ্য, ps, sock)) {
          ফলাফল = Curl_pollset_remove_in(তথ্য, ps, sock);
          যদি(ফলাফল)
            বিরতি;
        }
        যদি(send_blocked && Curl_pollset_want_send(তথ্য, ps, sock)) {
          ফলাফল = Curl_pollset_remove_out(তথ্য, ps, sock);
          যদি(ফলাফল)
            বিরতি;
        }
      }
    }

    /* Not blocked and wanting to receive. If there is data pending
     * in the connection filters, make transfer run again. */
    যদি(!recv_blocked &&
       ((Curl_pollset_want_recv(তথ্য, ps, তথ্য->conn->sock[FIRSTSOCKET]) &&
         Curl_conn_তথ্য_pending(তথ্য, FIRSTSOCKET)) ||
        (Curl_pollset_want_recv(তথ্য, ps, তথ্য->conn->sock[SECONDARYSOCKET]) &&
         Curl_conn_তথ্য_pending(তথ্য, SECONDARYSOCKET)))) {
      CURL_TRC_M(তথ্য, "pollset[] has POLLIN, but there is still "
                 "buffered input -> mark as dirty");
      Curl_multi_mark_dirty(তথ্য);
    }
  }
  ফেরত ফলাফল;
}

স্থির CURLcode mstate_connecting_pollset(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো easy_pollset *ps)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  curl_socket_t sockfd;
  CURLcode ফলাফল = CURLE_OK;

  যদি(Curl_xfer_recv_is_paused(তথ্য))
    ফেরত CURLE_OK;
  /* If a socket is set, receiving is default. If the socket
   * has not been determined yet (eyeballing), always ask the
   * connection filters for what to monitor. */
  sockfd = Curl_conn_get_first_socket(তথ্য);
  যদি(sockfd != CURL_SOCKET_BAD) {
    ফলাফল = Curl_pollset_change(তথ্য, ps, sockfd, CURL_POLL_IN, 0);
    যদি(!ফলাফল)
      ফলাফল = multi_adjust_pollset(তথ্য, ps);
  }
  যদি(!ফলাফল)
    ফলাফল = Curl_conn_adjust_pollset(তথ্য, conn, ps);
  ফেরত ফলাফল;
}

স্থির CURLcode mstate_protocol_pollset(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো easy_pollset *ps)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল = CURLE_OK;

  যদি(conn->scheme->run->proto_pollset)
    ফলাফল = conn->scheme->run->proto_pollset(তথ্য, ps);
  অন্যথা {
    curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
    যদি(sockfd != CURL_SOCKET_BAD) {
      /* Default is to wait to something from the server */
      ফলাফল = Curl_pollset_change(তথ্য, ps, sockfd, CURL_POLL_IN, 0);
    }
  }
  যদি(!ফলাফল)
    ফলাফল = multi_adjust_pollset(তথ্য, ps);
  যদি(!ফলাফল)
    ফলাফল = Curl_conn_adjust_pollset(তথ্য, conn, ps);
  ফেরত ফলাফল;
}

স্থির CURLcode mstate_do_pollset(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো easy_pollset *ps)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল = CURLE_OK;

  যদি(conn->scheme->run->doing_pollset)
    ফলাফল = conn->scheme->run->doing_pollset(তথ্য, ps);
  অন্যথা যদি(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    ফলাফল = Curl_pollset_add_out(তথ্য, ps, conn->sock[conn->send_idx]);
  }
  যদি(!ফলাফল)
    ফলাফল = multi_adjust_pollset(তথ্য, ps);
  যদি(!ফলাফল)
    ফলাফল = Curl_conn_adjust_pollset(তথ্য, conn, ps);
  ফেরত ফলাফল;
}

স্থির CURLcode mstate_domore_pollset(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো easy_pollset *ps)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল = CURLE_OK;

  যদি(conn->scheme->run->domore_pollset)
    ফলাফল = conn->scheme->run->domore_pollset(তথ্য, ps);
  অন্যথা যদি(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    ফলাফল = Curl_pollset_add_out(তথ্য, ps, conn->sock[conn->send_idx]);
  }
  যদি(!ফলাফল)
    ফলাফল = multi_adjust_pollset(তথ্য, ps);
  যদি(!ফলাফল)
    ফলাফল = Curl_conn_adjust_pollset(তথ্য, conn, ps);
  ফেরত ফলাফল;
}

স্থির CURLcode mstate_perform_pollset(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো easy_pollset *ps)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল = CURLE_OK;

  যদি(conn->scheme->run->perform_pollset)
    ফলাফল = conn->scheme->run->perform_pollset(তথ্য, ps);
  অন্যথা {
    /* Default is to obey the request flags for send/recv */
    যদি(Curl_req_want_recv(তথ্য) && CONN_SOCK_IDX_VALID(conn->recv_idx)) {
      ফলাফল = Curl_pollset_add_in(তথ্য, ps, conn->sock[conn->recv_idx]);
    }
    যদি(!ফলাফল && Curl_req_want_send(তথ্য) &&
       CONN_SOCK_IDX_VALID(conn->send_idx)) {
      ফলাফল = Curl_pollset_add_out(তথ্য, ps, conn->sock[conn->send_idx]);
    }
  }
  যদি(!ফলাফল)
    ফলাফল = multi_adjust_pollset(তথ্য, ps);
  যদি(!ফলাফল)
    ফলাফল = Curl_conn_adjust_pollset(তথ্য, conn, ps);
  ফেরত ফলাফল;
}

#ifdef CURLVERBOSE
স্থির size_t multi_timeouts_গুনতি(কাঠামো expire_timers *timeouts)
{
  size_t n = 0;
  uint8_t eid = timeouts->first;
  জন্য(; eid < EXPIRE_LAST; eid = timeouts->পরবর্তী[eid])
    ++n;
  ফেরত n;
}
#endif

/* Initializes `poll_set` with the current socket poll actions needed
 * for transfer `data`. */
CURLMcode Curl_multi_pollset(কাঠামো Curl_easy *তথ্য,
                             কাঠামো easy_pollset *ps)
{
  CURLcode ফলাফল = CURLE_OK;

  Curl_pollset_reset(ps);
#ifdef ENABLE_INTERNAL_WAKEUP
  /* The admin handle always listens on the wakeup socket when there
   * are transfers alive. */
  যদি(তথ্য->multi && (তথ্য == তথ্য->multi->admin) &&
     তথ্য->multi->xfers_really_alive) {
    CURL_TRC_M(তথ্য, "adding wakeup, %u xfers really alive",
               তথ্য->multi->xfers_really_alive);
    ফলাফল = Curl_pollset_add_in(তথ্য, ps, তথ্য->multi->wakeup_internal[0]);
  }
#endif
  /* If the transfer has no connection, this is fine. Happens when
     called via curl_multi_remove_handle() => Curl_multi_ev_assess() =>
     Curl_multi_pollset(). */
  যদি(!ফলাফল && তথ্য->conn) {
    নির্বাচন(তথ্য->mstate) {
    ক্ষেত্র MSTATE_INIT:
    ক্ষেত্র MSTATE_PENDING:
    ক্ষেত্র MSTATE_SETUP:
    ক্ষেত্র MSTATE_CONNECT:
      /* nothing to poll for yet */
      বিরতি;

    ক্ষেত্র MSTATE_CONNECTING:
      ফলাফল = mstate_connecting_pollset(তথ্য, ps);
      বিরতি;

    ক্ষেত্র MSTATE_PROTOCONNECT:
    ক্ষেত্র MSTATE_PROTOCONNECTING:
      ফলাফল = mstate_protocol_pollset(তথ্য, ps);
      বিরতি;

    ক্ষেত্র MSTATE_DO:
    ক্ষেত্র MSTATE_DOING:
      ফলাফল = mstate_do_pollset(তথ্য, ps);
      বিরতি;

    ক্ষেত্র MSTATE_DOING_MORE:
      ফলাফল = mstate_domore_pollset(তথ্য, ps);
      বিরতি;

    ক্ষেত্র MSTATE_DID: /* same as PERFORMING in regard to polling */
    ক্ষেত্র MSTATE_PERFORMING:
      ফলাফল = mstate_perform_pollset(তথ্য, ps);
      বিরতি;

    ক্ষেত্র MSTATE_RATELIMITING:
      /* we need to let time pass, ignore socket(s) */
      বিরতি;

    ক্ষেত্র MSTATE_DONE:
    ক্ষেত্র MSTATE_COMPLETED:
    ক্ষেত্র MSTATE_MSGSENT:
      /* nothing more to poll for */
      বিরতি;

    পূর্বনির্ধারিত:
      failf(তথ্য, "multi_getsock: unexpected multi state %d",
            (পূর্ণসংখ্যা)তথ্য->mstate);
      DEBUGASSERT(0);
      বিরতি;
    }
  }

  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_OUT_OF_MEMORY)
      ফেরত CURLM_OUT_OF_MEMORY;
    failf(তথ্য, "error determining pollset: %d", (পূর্ণসংখ্যা)ফলাফল);
    ফেরত CURLM_INTERNAL_ERROR;
  }

#ifdef CURLVERBOSE
  যদি(CURL_TRC_M_is_verbose(তথ্য)) {
    size_t timeout_গুনতি = multi_timeouts_গুনতি(&তথ্য->state.timeouts);
    নির্বাচন(ps->n) {
    ক্ষেত্র 0:
      CURL_TRC_M(তথ্য, "pollset[], timeouts=%zu, paused %d/%d (r/w)",
                 timeout_গুনতি,
                 Curl_xfer_send_is_paused(তথ্য),
                 Curl_xfer_recv_is_paused(তথ্য));
      বিরতি;
    ক্ষেত্র 1:
      CURL_TRC_M(তথ্য, "pollset[fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_OUT) ? "OUT" : "",
                 timeout_গুনতি);
      বিরতি;
    ক্ষেত্র 2:
      CURL_TRC_M(তথ্য, "pollset[fd=%" FMT_SOCKET_T " %s%s, "
                 "fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_OUT) ? "OUT" : "",
                 ps->sockets[1],
                 (ps->actions[1] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[1] & CURL_POLL_OUT) ? "OUT" : "",
                 timeout_গুনতি);
      বিরতি;
    পূর্বনির্ধারিত:
      CURL_TRC_M(তথ্য, "pollset[fds=%u], timeouts=%zu", ps->n, timeout_গুনতি);
      বিরতি;
    }
    CURL_TRC_EASY_TIMERS(তথ্য);
  }
#endif

  ফেরত CURLM_OK;
}

CURLMcode curl_multi_fdset(CURLM *m,
                           fd_set *read_fd_set, fd_set *write_fd_set,
                           fd_set *exc_fd_set, পূর্ণসংখ্যা *max_fd)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_fdset, &mresult)) {
    /* Scan through all the easy handles to get the file descriptors set.
       Some easy handles may not have connected to the remote host yet,
       and then we must make sure that is done. */
    কাঠামো Curl_multi *multi = m;
    কাঠামো easy_pollset ps;
    পূর্ণসংখ্যা this_max_fd = -1;
    unsigned পূর্ণসংখ্যা i;
    uint32_t mid;
    (শূন্য)exc_fd_set;

    Curl_pollset_init(&ps);
    যদি(Curl_uint32_bset_first(&multi->process, &mid)) {
      করো {
        কাঠামো Curl_easy *তথ্য = Curl_multi_get_easy(multi, mid);

        যদি(!তথ্য) {
          DEBUGASSERT(0);
          চালু;
        }

        Curl_multi_pollset(তথ্য, &ps);
        জন্য(i = 0; i < ps.n; i++) {
          যদি(!FDSET_SOCK(ps.sockets[i]))
            /* pretend it does not exist */
            চালু;
          যদি(ps.actions[i] & CURL_POLL_IN)
            FD_SET(ps.sockets[i], read_fd_set);
          যদি(ps.actions[i] & CURL_POLL_OUT)
            FD_SET(ps.sockets[i], write_fd_set);
          যদি((পূর্ণসংখ্যা)ps.sockets[i] > this_max_fd)
            this_max_fd = (পূর্ণসংখ্যা)ps.sockets[i];
        }
      } যখন(Curl_uint32_bset_পরবর্তী(&multi->process, mid, &mid));
    }

    Curl_cshutdn_setfds(&multi->cshutdn, multi->admin,
                        read_fd_set, write_fd_set, &this_max_fd);

    *max_fd = this_max_fd;
    Curl_pollset_cleanup(&ps);

    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

CURLMcode curl_multi_waitfds(CURLM *m,
                             কাঠামো curl_waitfd *ufds,
                             unsigned পূর্ণসংখ্যা আকার_2,
                             unsigned পূর্ণসংখ্যা *fd_গুনতি)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_waitfds, &mresult)) {
    কাঠামো Curl_waitfds cwfds;
    কাঠামো Curl_multi *multi = m;
    কাঠামো easy_pollset ps;
    unsigned পূর্ণসংখ্যা need = 0;
    uint32_t mid;

    যদি(!ufds && (আকার_2 || !fd_গুনতি)) {
      mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      যাও out;
    }

    Curl_pollset_init(&ps);
    Curl_waitfds_init(&cwfds, ufds, আকার_2);
    mresult = CURLM_OK;
    যদি(Curl_uint32_bset_first(&multi->process, &mid)) {
      করো {
        কাঠামো Curl_easy *তথ্য = Curl_multi_get_easy(multi, mid);
        যদি(!তথ্য) {
          DEBUGASSERT(0);
          Curl_uint32_bset_remove(&multi->process, mid);
          Curl_uint32_bset_remove(&multi->dirty, mid);
          চালু;
        }
        Curl_multi_pollset(তথ্য, &ps);
        need += Curl_waitfds_add_ps(&cwfds, &ps);
      } যখন(Curl_uint32_bset_পরবর্তী(&multi->process, mid, &mid));
    }

    need += Curl_cshutdn_add_waitfds(&multi->cshutdn, multi->admin, &cwfds);

    যদি(need != cwfds.n && ufds)
      mresult = CURLM_OUT_OF_MEMORY;

    যদি(fd_গুনতি)
      *fd_গুনতি = need;
    Curl_pollset_cleanup(&ps);
  }
out:
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

#ifdef USE_WINSOCK
/* Reset FD_WRITE for TCP sockets. Nothing is actually sent. UDP sockets cannot
 * be reset this way because an empty datagram would be sent. #9203
 *
 * "On Windows the internal state of FD_WRITE as returned from
 * WSAEnumNetworkEvents is only reset after successful send()."
 */
স্থির শূন্য reset_socket_fdwrite(curl_socket_t s)
{
  পূর্ণসংখ্যা t;
  পূর্ণসংখ্যা l = (পূর্ণসংখ্যা)আকার(t);
  যদি(!getsockopt(s, SOL_SOCKET, SO_TYPE, (অক্ষর *)&t, &l) && t == SOCK_STREAM)
    swrite(s, ফাঁকা, 0);
}

স্থির CURLMcode multi_winsock_select(কাঠামো Curl_multi *multi,
                                      কাঠামো curl_pollfds *cpfds,
                                      unsigned পূর্ণসংখ্যা curl_nfds,
                                      কাঠামো curl_waitfd extra_fds[],
                                      unsigned পূর্ণসংখ্যা extra_nfds,
                                      পূর্ণসংখ্যা timeout_ms,
                                      bool extrawait,
                                      পূর্ণসংখ্যা *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  WSANETWORKEVENTS wsa_events;
  পূর্ণসংখ্যা nevents = 0;
  size_t i;

  DEBUGASSERT(multi->wsa_event != WSA_INVALID_EVENT);

  /* Set the WSA events based on the collected pollds */
  জন্য(i = 0; i < cpfds->n; i++) {
    long mask = 0;
    যদি(cpfds->pfds[i].events & POLLIN)
      mask |= FD_READ | FD_ACCEPT | FD_CLOSE;
    যদি(cpfds->pfds[i].events & POLLPRI)
      mask |= FD_OOB;
    যদি(cpfds->pfds[i].events & POLLOUT) {
      mask |= FD_WRITE | FD_CONNECT | FD_CLOSE;
      reset_socket_fdwrite(cpfds->pfds[i].fd);
    }
    যদি(mask && WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, mask)) {
      mresult = CURLM_OUT_OF_MEMORY;
      যাও out;
    }
  }

  যদি(cpfds->n || extrawait) {
    পূর্ণসংখ্যা pollrc = 0;

    যদি(cpfds->n) {         /* pre-check with Winsock */
      pollrc = Curl_poll(cpfds->pfds, cpfds->n, 0);
      যদি(pollrc < 0) {
        mresult = CURLM_UNRECOVERABLE_POLL;
        যাও out;
      }
      nevents = pollrc;
    }

    যদি(!nevents) {
      /* now wait... if not ready during the pre-check (pollrc == 0) */
      WSAWaitForMultipleEvents(1, &multi->wsa_event, FALSE, (DWORD)timeout_ms,
                               FALSE);
    }

    /* With Winsock, we have to run the following section unconditionally
       to call WSAEventSelect(fd, event, 0) on all the sockets */
    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    জন্য(i = 0; i < extra_nfds; i++) {
      unsigned short mask = 0;
      curl_socket_t s = extra_fds[i].fd;

      wsa_events.lNetworkEvents = 0;
      যদি(WSAEnumNetworkEvents(s, ফাঁকা, &wsa_events) == 0) {
        যদি(wsa_events.lNetworkEvents & (FD_READ | FD_ACCEPT | FD_CLOSE))
          mask |= CURL_WAIT_POLLIN;
        যদি(wsa_events.lNetworkEvents & (FD_WRITE | FD_CONNECT | FD_CLOSE))
          mask |= CURL_WAIT_POLLOUT;
        যদি(wsa_events.lNetworkEvents & FD_OOB)
          mask |= CURL_WAIT_POLLPRI;
        যদি(!pollrc && wsa_events.lNetworkEvents)
          nevents++;
      }
      WSAEventSelect(s, multi->wsa_event, 0);
      যদি(!pollrc) {
        extra_fds[i].revents = (short)mask;
        চালু;
      }
      অন্যথা {
        unsigned r = (unsigned)cpfds->pfds[curl_nfds + i].revents;
        যদি(r & POLLIN)
          mask |= CURL_WAIT_POLLIN;
        যদি(r & POLLOUT)
          mask |= CURL_WAIT_POLLOUT;
        যদি(r & POLLPRI)
          mask |= CURL_WAIT_POLLPRI;
        extra_fds[i].revents = (short)mask;
      }
    }

    /* Count up all our own sockets that had activity,
       and remove them from the event. */
    জন্য(i = 0; i < curl_nfds; ++i) {
      wsa_events.lNetworkEvents = 0;
      যদি(WSAEnumNetworkEvents(cpfds->pfds[i].fd, ফাঁকা, &wsa_events) == 0) {
        যদি(!pollrc && wsa_events.lNetworkEvents)
          nevents++;
      }
      WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, 0);
    }
    WSAResetEvent(multi->wsa_event);
  }

out:
  *pnevents = nevents;
  ফেরত mresult;
}

#অন্যথা /* USE_WINSOCK */

স্থির CURLMcode multi_posix_poll(কাঠামো Curl_multi *multi,
                                  কাঠামো curl_pollfds *cpfds,
                                  unsigned পূর্ণসংখ্যা curl_nfds,
                                  কাঠামো curl_waitfd extra_fds[],
                                  unsigned পূর্ণসংখ্যা extra_nfds,
                                  পূর্ণসংখ্যা timeout_ms,
                                  bool extrawait,
                                  পূর্ণসংখ্যা *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  পূর্ণসংখ্যা nevents = 0;
  size_t i;

  (শূন্য)multi;
  যদি(cpfds->n) {
    পূর্ণসংখ্যা pollrc = Curl_poll(cpfds->pfds, cpfds->n, timeout_ms); /* wait... */
    যদি(pollrc < 0) {
      mresult = CURLM_UNRECOVERABLE_POLL;
      যাও out;
    }
    nevents = pollrc;

    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    জন্য(i = 0; i < extra_nfds; i++) {
      unsigned r = (unsigned)cpfds->pfds[curl_nfds + i].revents;
      unsigned short mask = 0;
      যদি(r & POLLIN)
        mask |= CURL_WAIT_POLLIN;
      যদি(r & POLLOUT)
        mask |= CURL_WAIT_POLLOUT;
      যদি(r & POLLPRI)
        mask |= CURL_WAIT_POLLPRI;
      extra_fds[i].revents = (short)mask;
    }
  }
  অন্যথা যদি(extrawait) {
    /* No fds to poll, but asked to obey timeout_ms anyway. We cannot
     * use Curl_poll() as it, on some platforms, returns immediately
     * without fds. */
    curlx_wait_ms(timeout_ms);
  }

out:
  *pnevents = nevents;
  ফেরত mresult;
}

#endif /* !USE_WINSOCK */

#define অঙ্ক_POLLS_ON_স্তূপ 10

স্থির CURLMcode multi_wait(কাঠামো Curl_multi *multi,
                            কাঠামো curl_waitfd extra_fds[],
                            unsigned পূর্ণসংখ্যা extra_nfds,
                            পূর্ণসংখ্যা timeout_ms,
                            পূর্ণসংখ্যা *ret,
                            bool extrawait)  /* when no socket, wait */
{
  size_t i;
  পূর্ণসংখ্যা timeout_internal;
  পূর্ণসংখ্যা nevents = 0;
  কাঠামো easy_pollset ps;
  কাঠামো pollfd a_few_on_স্তূপ[অঙ্ক_POLLS_ON_স্তূপ];
  কাঠামো curl_pollfds cpfds;
  unsigned পূর্ণসংখ্যা curl_nfds = 0; /* how many pfds are for curl transfers */
  কাঠামো Curl_easy *তথ্য = ফাঁকা;
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;
#ifdef ENABLE_WAKEUP
  পূর্ণসংখ্যা wakeup_idx = -1;
#endif

  যদি(timeout_ms < 0)
    ফেরত CURLM_BAD_FUNCTION_ARGUMENT;

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_স্তূপ, অঙ্ক_POLLS_ON_স্তূপ);

  /* Add the curl handles to our pollfds first */
  যদি(Curl_uint32_bset_first(&multi->process, &mid)) {
    করো {
      তথ্য = Curl_multi_get_easy(multi, mid);
      যদি(!তথ্য) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        চালু;
      }
      Curl_multi_pollset(তথ্য, &ps);
      যদি(Curl_pollfds_add_ps(&cpfds, &ps)) {
        mresult = CURLM_OUT_OF_MEMORY;
        যাও out;
      }
    } যখন(Curl_uint32_bset_পরবর্তী(&multi->process, mid, &mid));
  }

  যদি(Curl_cshutdn_add_pollfds(&multi->cshutdn, multi->admin, &cpfds)) {
    mresult = CURLM_OUT_OF_MEMORY;
    যাও out;
  }

#ifdef ENABLE_WAKEUP
  /* If `extrawait` is TRUE *or* we have `extra_fds`to poll *or* we
   * have transfer sockets to poll, we obey `timeout_ms`.
   * Then we need to also monitor the multi's wakeup
   * socket to catch calls to `curl_multi_wakeup()` during the wait. */
  যদি(extrawait || cpfds.n || extra_nfds) {
    wakeup_idx = cpfds.n;
    যদি(Curl_pollfds_add_sock(&cpfds, multi->wakeup_pair[0], POLLIN)) {
      mresult = CURLM_OUT_OF_MEMORY;
      যাও out;
    }
  }
#endif

  curl_nfds = cpfds.n; /* what curl internally uses in cpfds */
  /* Add external file descriptions from poll-like struct curl_waitfd */
  জন্য(i = 0; i < extra_nfds; i++) {
    unsigned short events = 0;
    যদি(extra_fds[i].events & CURL_WAIT_POLLIN)
      events |= POLLIN;
    যদি(extra_fds[i].events & CURL_WAIT_POLLPRI)
      events |= POLLPRI;
    যদি(extra_fds[i].events & CURL_WAIT_POLLOUT)
      events |= POLLOUT;
    যদি(Curl_pollfds_add_sock(&cpfds, extra_fds[i].fd, events)) {
      mresult = CURLM_OUT_OF_MEMORY;
      যাও out;
    }
  }

  /* We check the internal timeout *AFTER* we collected all sockets to
   * poll. Collecting the sockets may install new timers by protocols
   * and connection filters.
   * Use the shorter one of the internal and the caller requested timeout.
   * If we are called with `!extrawait` and multi_timeout() reports no
   * timeouts exist, do not wait. */
  multi_timeout(multi, ফাঁকা, &timeout_internal);
  যদি((timeout_internal >= 0) && (timeout_internal < timeout_ms))
    timeout_ms = timeout_internal;

  যদি(তথ্য)
    CURL_TRC_M(তথ্য, "multi_wait(fds=%u, timeout=%d) tinternal=%d",
               cpfds.n, timeout_ms, timeout_internal);

#ifdef USE_WINSOCK
  mresult = multi_winsock_select(multi, &cpfds, curl_nfds,
                                 extra_fds, extra_nfds,
                                 timeout_ms, extrawait, &nevents);
#অন্যথা
  mresult = multi_posix_poll(multi, &cpfds, curl_nfds,
                             extra_fds, extra_nfds,
                             timeout_ms, extrawait, &nevents);
#endif

#ifdef ENABLE_WAKEUP
    যদি(nevents && (wakeup_idx >= 0)) {
      যদি(cpfds.pfds[wakeup_idx].revents & POLLIN) {
        (শূন্য)Curl_wakeup_consume(multi->wakeup_pair, TRUE);
        /* do not count the wakeup socket into the returned value */
        nevents--;
      }
    }
#endif

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  যদি(ret)
    *ret = nevents;
  ফেরত mresult;
}

CURLMcode curl_multi_wait(CURLM *m,
                          কাঠামো curl_waitfd extra_fds[],
                          unsigned পূর্ণসংখ্যা extra_nfds,
                          পূর্ণসংখ্যা timeout_ms,
                          পূর্ণসংখ্যা *ret)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_wait, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, FALSE);
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

CURLMcode curl_multi_poll(CURLM *m,
                          কাঠামো curl_waitfd extra_fds[],
                          unsigned পূর্ণসংখ্যা extra_nfds,
                          পূর্ণসংখ্যা timeout_ms,
                          পূর্ণসংখ্যা *ret)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_poll, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, TRUE);
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

CURLMcode curl_multi_wakeup(CURLM *m)
{
  /* this function is usually called from another thread,
     it has to be careful only to access parts of the
     Curl_multi struct that are constant */
  কাঠামো Curl_multi *multi = m;
  CURLMcode mresult = CURLM_WAKEUP_FAILURE;

  /* GOOD_MULTI_HANDLE can be safely called */
  যদি(!GOOD_MULTI_HANDLE(multi))
    ফেরত CURLM_BAD_HANDLE;

#ifdef ENABLE_WAKEUP
  /* the wakeup_pair variable is only written during init and cleanup,
     making it safe to access from another thread after the init part
     and before cleanup */
  যদি(!Curl_wakeup_signal(multi->wakeup_pair))
    mresult = CURLM_OK;
#endif
#ifdef USE_WINSOCK
  যদি(WSASetEvent(multi->wsa_event))
    mresult = CURLM_OK;
#endif
  ফেরত mresult;
}

#ifdef ENABLE_INTERNAL_WAKEUP
শূন্য Curl_multi_wakeup_internal(কাঠামো Curl_multi *multi)
{
  /* This is expected to be invocable from another thread which
   * does NOT outlive the multi handle. Check for sanity. */
  যদি(GOOD_MULTI_HANDLE(multi))
    Curl_wakeup_signal(multi->wakeup_internal);
  অন্যথা
    DEBUGASSERT(0);
}
#endif

/*
 * multi_ischanged() is called
 *
 * Returns TRUE/FALSE whether the state is changed to trigger a CONNECT_PEND
 * => CONNECT action.
 *
 * Set 'clear' to TRUE to have it also clear the state variable.
 */
স্থির bool multi_ischanged(কাঠামো Curl_multi *multi, bool clear)
{
  bool retval = FALSE;
  DEBUGASSERT(multi);
  যদি(multi) {
    retval = (bool)multi->recheckstate;
    যদি(clear)
      multi->recheckstate = FALSE;
  }
  ফেরত retval;
}

/*
 * Curl_multi_connchanged() is called to tell that there is a connection in
 * this multi handle that has changed state (multiplexing become possible, the
 * number of allowed streams changed or similar), and a subsequent use of this
 * multi handle should move CONNECT_PEND handles back to CONNECT to have them
 * retry.
 */
শূন্য Curl_multi_connchanged(কাঠামো Curl_multi *multi)
{
  /* May be invoked for a `data->multi` when no multi is present. */
  যদি(multi)
    multi->recheckstate = TRUE;
}

CURLMcode Curl_multi_add_perform(কাঠামো Curl_multi *multi,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো connectdata *conn)
{
  CURLMcode mresult;

  mresult = Curl_multi_add_handle(multi, তথ্য);
  যদি(!mresult) {
    CURLcode ফলাফল;

    /* pass in NULL for 'conn' here since we do not want to init the
       connection, only this transfer */
    ফলাফল = Curl_init_transfer(তথ্য, ফাঁকা);
    যদি(ফলাফল) {
      Curl_multi_remove_handle(multi, তথ্য);
      ফেরত CURLM_INTERNAL_ERROR;
    }

    /* take this handle to the perform state right away */
    multistate(তথ্য, MSTATE_PERFORMING);
    Curl_attach_connection(তথ্য, conn, TRUE);
    CURL_REQ_SET_RECV(তথ্য);
  }
  ফেরত mresult;
}

স্থির CURLcode multi_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(conn->scheme);

  যদি(conn->scheme->run->do_it)
    ফলাফল = conn->scheme->run->do_it(তথ্য, done);

  ফেরত ফলাফল;
}

/*
 * multi_do_more() is called during the DO_MORE multi state. It is a second
 * stage DO state which (wrongly) was introduced to support FTP's second
 * connection.
 *
 * 'complete' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (to DOING state when there is more work to do)
 */

স্থির CURLcode multi_do_more(কাঠামো Curl_easy *তথ্য, domore *complete)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;

  *complete = DOMORE_INCOMPLETE;

  যদি(conn->scheme->run->do_more)
    ফলাফল = conn->scheme->run->do_more(তথ্য, complete);

  ফেরত ফলাফল;
}

/*
 * Check whether a timeout occurred, and handle it if it did
 */
স্থির bool multi_handle_timeout(কাঠামো Curl_easy *তথ্য,
                                 ধ্রুবক কাঠামো curltime *pnow,
                                 bool *stream_error,
                                 CURLcode *ফলাফল)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_timeleft_now_ms(তথ্য, pnow);
  যদি(timeout_ms < 0) {
    /* Handle timed out */
    timerid base_timer = Curl_is_connecting(তথ্য) ?
                         TIMER_STARTSINGLE : TIMER_STARTOP;
    timediff_t elapsed_ms = Curl_pgrs_since_ms(তথ্য, ফাঁকা, base_timer);
    যদি(তথ্য->mstate == MSTATE_CONNECTING) {
      failf(তথ্য, "%s timed out after %" FMT_TIMEDIFF_T " milliseconds",
            তথ্য->conn->bits.dns_resolved ? "Connection" : "Resolving",
            elapsed_ms);
      /* keep what we know about a partly done connect, e.g. TCP done
       * but TLS handshake still ongoing */
      Curl_conn_report_failed(তথ্য, FIRSTSOCKET);
    }
    অন্যথা {
      কাঠামো SingleRequest *k = &তথ্য->req;
      যদি(k->আকার_2 != -1) {
        failf(তথ্য, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount, k->আকার_2);
      }
      অন্যথা {
        failf(তথ্য, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount);
      }
    }
    *ফলাফল = CURLE_OPERATION_TIMEDOUT;
    যদি(তথ্য->conn) {
      /* Force connection closed if the connection has indeed been used */
      যদি(তথ্য->mstate > MSTATE_DO) {
        streamclose(তথ্য->conn);
        *stream_error = TRUE;
      }
      (শূন্য)multi_done(তথ্য, *ফলাফল, TRUE);
    }
    ফেরত TRUE;
  }

  ফেরত FALSE;
}

/*
 * We are doing protocol-specific connecting and this is being called over and
 * over from the multi interface until the connection phase is done on
 * protocol layer.
 */

স্থির CURLcode protocol_connecting(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;

  যদি(conn && conn->scheme->run->connecting) {
    *done = FALSE;
    ফলাফল = conn->scheme->run->connecting(তথ্য, done);
  }
  অন্যথা
    *done = TRUE;

  ফেরত ফলাফল;
}

/*
 * We are DOING this is being called over and over from the multi interface
 * until the DOING phase is done on protocol layer.
 */

স্থির CURLcode protocol_doing(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো connectdata *conn = তথ্য->conn;

  যদি(conn && conn->scheme->run->doing) {
    *done = FALSE;
    ফলাফল = conn->scheme->run->doing(তথ্য, done);
  }
  অন্যথা
    *done = TRUE;

  ফেরত ফলাফল;
}

/*
 * We have discovered that the TCP connection has been successful, we can now
 * proceed with some action.
 *
 */
স্থির CURLcode protocol_connect(কাঠামো Curl_easy *তথ্য, bool *protocol_done)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(conn);
  DEBUGASSERT(protocol_done);
  DEBUGASSERT(Curl_conn_is_connected(conn, FIRSTSOCKET));

  *protocol_done = FALSE;
  যদি(!conn->bits.protoconnstart) {
    যদি(conn->scheme->run->connect_it) {
      /* Call the protocol-specific connect function */
      ফলাফল = conn->scheme->run->connect_it(তথ্য, protocol_done);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    conn->bits.protoconnstart = TRUE;
  }

  /* Unless this protocol does not have any protocol-connect callback, as
     then we know we are done. */
  যদি(!conn->scheme->run->connecting)
    *protocol_done = TRUE;
  ফেরত CURLE_OK;
}

/*
 * posttransfer() is called immediately after a transfer ends
 */
স্থির শূন্য multi_posttransfer(কাঠামো Curl_easy *তথ্য)
{
#যদি defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
  /* restore the signal handler for SIGPIPE before we get back */
  যদি(!তথ্য->set.no_signal)
    সংকেত(SIGPIPE, তথ্য->state.পূর্ব_signal);
#অন্যথা
  (শূন্য)তথ্য;
#endif
}

/*
 * multi_follow() handles the URL redirect magic. Pass in the 'newurl' string
 * as given by the remote server and set up the new URL to request.
 *
 * This function DOES NOT FREE the given URL.
 */
স্থির CURLcode multi_follow(কাঠামো Curl_easy *তথ্য,
                             ধ্রুবক কাঠামো Curl_scheme *handler,
                             ধ্রুবক অক্ষর *newurl, /* the Location: string */
                             followtype type) /* see transfer.h */
{
  যদি(handler && handler->run->follow)
    ফেরত handler->run->follow(তথ্য, newurl, type);

  যদি(type == FOLLOW_RETRY)
    /* Retries are generic and do not require protocol-specific redirect
       handling. */
    ফেরত CURLE_OK;

  ফেরত CURLE_TOO_MANY_REDIRECTS;
}

স্থির CURLcode mspeed_check(কাঠামো Curl_easy *তথ্য)
{
  যদি(Curl_rlimit_active(&তথ্য->progress.dl.rlimit) ||
     Curl_rlimit_active(&তথ্য->progress.ul.rlimit)) {
    /* check if our send/recv limits require idle waits */
    ধ্রুবক কাঠামো curltime *pnow = Curl_pgrs_now(তথ্য);
    timediff_t recv_ms, send_ms;

    send_ms = Curl_rlimit_wait_ms(&তথ্য->progress.ul.rlimit, pnow);
    recv_ms = Curl_rlimit_wait_ms(&তথ্য->progress.dl.rlimit, pnow);

    যদি(send_ms || recv_ms) {
      যদি(তথ্য->mstate != MSTATE_RATELIMITING) {
        multistate(তথ্য, MSTATE_RATELIMITING);
      }
      Curl_expire_set(তথ্য, EXPIRE_TOOFAST, CURLMAX(send_ms, recv_ms), pnow);
      Curl_multi_clear_dirty(তথ্য);
      CURL_TRC_M(তথ্য, "[RLIMIT] waiting %" FMT_TIMEDIFF_T "ms",
                 CURLMAX(send_ms, recv_ms));
      ফেরত CURLE_AGAIN;
    }
    অন্যথা {
      /* when will the rate limits increase next? The transfer needs
       * to run again at that time or it may stall. */
      send_ms = Curl_rlimit_পরবর্তী_step_ms(&তথ্য->progress.ul.rlimit, pnow);
      recv_ms = Curl_rlimit_পরবর্তী_step_ms(&তথ্য->progress.dl.rlimit, pnow);
      যদি(send_ms || recv_ms) {
        timediff_t পরবর্তী_ms = CURLMIN(send_ms, recv_ms);
        যদি(!পরবর্তী_ms)
          পরবর্তী_ms = CURLMAX(send_ms, recv_ms);
        Curl_expire_set(তথ্য, EXPIRE_TOOFAST, পরবর্তী_ms, pnow);
        CURL_TRC_M(তথ্য, "[RLIMIT] next token update in %" FMT_TIMEDIFF_T "ms",
                   পরবর্তী_ms);
      }
    }
  }

  যদি(তথ্য->mstate != MSTATE_PERFORMING) {
    CURL_TRC_M(তথ্য, "[RLIMIT] wait over, continue");
    multistate(তথ্য, MSTATE_PERFORMING);
  }
  ফেরত CURLE_OK;
}

স্থির CURLMcode multistate_performing(কাঠামো Curl_easy *তথ্য,
                                       bool *stream_errorp,
                                       CURLcode *resultp)
{
  অক্ষর *newurl = ফাঁকা;
  bool retry = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode ফলাফল = *resultp = CURLE_OK;
  *stream_errorp = FALSE;

  যদি(mspeed_check(তথ্য) == CURLE_AGAIN)
    ফেরত CURLM_OK;

  /* read/write data if it is ready to do so */
  ফলাফল = Curl_sendrecv(তথ্য);

  যদি(তথ্য->req.done || (ফলাফল == CURLE_RECV_ERROR)) {
    /* If CURLE_RECV_ERROR happens early enough, we assume it was a race
     * condition and the server closed the reused connection exactly when we
     * wanted to use it, so figure out if that is indeed the case.
     */
    CURLcode ret = Curl_retry_request(তথ্য, &newurl);
    যদি(!ret)
      retry = !!newurl;
    অন্যথা যদি(!ফলাফল)
      ফলাফল = ret;

    যদি(retry) {
      /* if we are to retry, set the result to OK and consider the
         request as done */
      ফলাফল = CURLE_OK;
      তথ্য->req.done = TRUE;
    }
  }
#ifndef CURL_DISABLE_HTTP
  অন্যথা যদি((ফলাফল == CURLE_HTTP2_STREAM) &&
          Curl_h2_http_1_1_error(তথ্য)) {
    CURLcode ret = Curl_retry_request(তথ্য, &newurl);

    যদি(!ret) {
      infof(তথ্য, "Downgrades to HTTP/1.1");
      streamclose(তথ্য->conn);
      তথ্য->state.http_neg.wanted = CURL_HTTP_V1x;
      তথ্য->state.http_neg.allowed = CURL_HTTP_V1x;
      /* clear the error message bit too as we ignore the one we got */
      তথ্য->state.errorbuf = FALSE;
      যদি(!newurl)
        /* typically for HTTP_1_1_REQUIRED error on first flight */
        newurl = Curl_bufref_dup(&তথ্য->state.url);
      যদি(!newurl) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
      }
      অন্যথা {
        /* if we are to retry, set the result to OK and consider the request
          as done */
        retry = TRUE;
        ফলাফল = CURLE_OK;
        তথ্য->req.done = TRUE;
      }
    }
    অন্যথা
      ফলাফল = ret;
  }
#endif

  যদি(ফলাফল) {
    /*
     * The transfer phase returned error, we mark the connection to get closed
     * to prevent being reused. This is because we cannot possibly know if the
     * connection is in a good shape or not now. Unless it is a protocol which
     * uses two "channels" like FTP, as then the error happened in the data
     * connection.
     */

    যদি(!(তথ্য->conn->scheme->flags & PROTOPT_DUAL) &&
       ফলাফল != CURLE_HTTP2_STREAM)
      streamclose(তথ্য->conn);

    multi_posttransfer(তথ্য);
    multi_done(তথ্য, ফলাফল, TRUE);
  }
  অন্যথা যদি(তথ্য->req.done && !Curl_cwriter_is_paused(তথ্য)) {
    ধ্রুবক কাঠামো Curl_scheme *handler = তথ্য->conn->scheme;

    /* call this even if the readwrite function returned error */
    multi_posttransfer(তথ্য);

    /* When we follow redirects or is set to retry the connection, we must to
       go back to the CONNECT state */
    যদি(তথ্য->req.newurl || retry) {
      followtype follow = FOLLOW_NONE;
      যদি(!retry) {
        /* if the URL is a follow-location and not a retried request then
           figure out the URL here */
        curlx_free(newurl);
        newurl = তথ্য->req.newurl;
        তথ্য->req.newurl = ফাঁকা;
        follow = FOLLOW_REDIR;
      }
      অন্যথা
        follow = FOLLOW_RETRY;
      (শূন্য)multi_done(তথ্য, CURLE_OK, FALSE);
      /* multi_done() might return CURLE_GOT_NOTHING */
      ফলাফল = multi_follow(তথ্য, handler, newurl, follow);
      যদি(!ফলাফল) {
        multistate(তথ্য, MSTATE_SETUP);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    অন্যথা {
      /* after the transfer is done, go DONE */

      /* but first check to see if we got a location info even though we are
         not following redirects */
      যদি(তথ্য->req.location) {
        curlx_free(newurl);
        newurl = তথ্য->req.location;
        তথ্য->req.location = ফাঁকা;
        ফলাফল = multi_follow(তথ্য, handler, newurl, FOLLOW_FAKE);
        যদি(ফলাফল) {
          *stream_errorp = TRUE;
          ফলাফল = multi_done(তথ্য, ফলাফল, TRUE);
        }
      }

      যদি(!ফলাফল) {
        multistate(তথ্য, MSTATE_DONE);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
  }
  অন্যথা { /* not errored, not done */
    mspeed_check(তথ্য);
  }
  curlx_free(newurl);
  *resultp = ফলাফল;
  ফেরত mresult;
}

স্থির CURLMcode multistate_do(কাঠামো Curl_easy *তথ্য,
                               bool *stream_errorp,
                               CURLcode *resultp)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode ফলাফল = CURLE_OK;
  যদি(তথ্য->set.fprereq) {
    কাঠামো Curl_mapi_guard guard;
    পূর্ণসংখ্যা prereq_rc;

    /* call the prerequest callback function */
    CURL_CBAPI_START(&guard, তথ্য, easy_fprereq);
    prereq_rc = তথ্য->set.fprereq(তথ্য->set.prereq_userp,
                                  তথ্য->info.primary.remote_ip,
                                  তথ্য->info.primary.local_ip,
                                  তথ্য->info.primary.remote_port,
                                  তথ্য->info.primary.local_port);
    CURL_CBAPI_END(&guard);
    যদি(prereq_rc != CURL_PREREQFUNC_OK) {
      failf(তথ্য, "operation aborted by pre-request callback");
      /* failure in pre-request callback - do not do any other processing */
      ফলাফল = CURLE_ABORTED_BY_CALLBACK;
      multi_posttransfer(তথ্য);
      multi_done(তথ্য, ফলাফল, FALSE);
      *stream_errorp = TRUE;
      যাও end;
    }
  }

  যদি(তথ্য->set.connect_only && !তথ্য->set.connect_only_ws) {
    multistate(তথ্য, MSTATE_DONE);
    mresult = CURLM_CALL_MULTI_PERFORM;
  }
  অন্যথা {
    bool dophase_done = FALSE;
    /* Perform the protocol's DO action */
    ফলাফল = multi_do(তথ্য, &dophase_done);

    যদি(!ফলাফল) {
      যদি(!dophase_done) {
#ifndef CURL_DISABLE_FTP
        /* some steps needed for wildcard matching */
        যদি(তথ্য->state.wildcardmatch) {
          কাঠামো Wildcard_তথ্য *wc = তথ্য->wildcard;
          যদি(wc->state == CURLWC_DONE || wc->state == CURLWC_SKIP) {
            /* skip some states if it is important */
            multi_done(তথ্য, CURLE_OK, FALSE);

            /* if there is no connection left, skip the DONE state */
            multistate(তথ্য, তথ্য->conn ? MSTATE_DONE : MSTATE_COMPLETED);
            mresult = CURLM_CALL_MULTI_PERFORM;
            যাও end;
          }
        }
#endif
        /* DO was not completed in one function call, we must continue
           DOING... */
        multistate(তথ্য, MSTATE_DOING);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }

      /* after DO, go DO_DONE... or DO_MORE */
      অন্যথা যদি(তথ্য->conn->bits.do_more) {
        /* we are supposed to do more, but we need to sit down, relax and wait
           a little while first */
        multistate(তথ্য, MSTATE_DOING_MORE);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
      অন্যথা {
        /* we are done with the DO, now DID */
        multistate(তথ্য, MSTATE_DID);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    অন্যথা যদি((ফলাফল == CURLE_SEND_ERROR) &&
            তথ্য->conn->bits.reuse) {
      /*
       * In this situation, a connection that we were trying to use may have
       * unexpectedly died. If possible, send the connection back to the
       * CONNECT phase so we can try again.
       */
      ধ্রুবক কাঠামো Curl_scheme *handler = তথ্য->conn->scheme;
      অক্ষর *newurl = ফাঁকা;
      followtype follow = FOLLOW_NONE;
      CURLcode drc;

      drc = Curl_retry_request(তথ্য, &newurl);
      যদি(drc) {
        /* a failure here pretty much implies an out of memory */
        ফলাফল = drc;
        *stream_errorp = TRUE;
      }

      multi_posttransfer(তথ্য);
      drc = multi_done(তথ্য, ফলাফল, FALSE);

      /* When set to retry the connection, we must go back to the CONNECT
       * state */
      যদি(newurl) {
        যদি(!drc || (drc == CURLE_SEND_ERROR)) {
          follow = FOLLOW_RETRY;
          drc = multi_follow(তথ্য, handler, newurl, follow);
          যদি(!drc) {
            multistate(তথ্য, MSTATE_SETUP);
            mresult = CURLM_CALL_MULTI_PERFORM;
            ফলাফল = CURLE_OK;
          }
          অন্যথা {
            /* Follow failed */
            ফলাফল = drc;
          }
        }
        অন্যথা {
          /* done did not return OK or SEND_ERROR */
          ফলাফল = drc;
        }
      }
      অন্যথা {
        /* Have error handler disconnect conn if we cannot retry */
        *stream_errorp = TRUE;
      }
      curlx_free(newurl);
    }
    অন্যথা {
      /* failure detected */
      multi_posttransfer(তথ্য);
      যদি(তথ্য->conn)
        multi_done(তথ্য, ফলাফল, FALSE);
      *stream_errorp = TRUE;
    }
  }
end:
  *resultp = ফলাফল;
  ফেরত mresult;
}

স্থির CURLMcode multistate_ratelimiting(কাঠামো Curl_easy *তথ্য,
                                         CURLcode *resultp)
{
  CURLcode ফলাফল = CURLE_OK;
  CURLMcode mresult = CURLM_OK;
  DEBUGASSERT(তথ্য->conn);
  /* if both rates are within spec, resume transfer */
  ফলাফল = Curl_pgrsCheck(তথ্য);

  যদি(ফলাফল) {
    যদি(!(তথ্য->conn->scheme->flags & PROTOPT_DUAL) &&
       ফলাফল != CURLE_HTTP2_STREAM)
      streamclose(তথ্য->conn);

    multi_posttransfer(তথ্য);
    multi_done(তথ্য, ফলাফল, TRUE);
  }
  অন্যথা {
    যদি(!mspeed_check(তথ্য))
      mresult = CURLM_CALL_MULTI_PERFORM;
  }
  *resultp = ফলাফল;
  ফেরত mresult;
}

স্থির CURLMcode multistate_connect(কাঠামো Curl_multi *multi,
                                    কাঠামো Curl_easy *তথ্য,
                                    CURLcode *resultp)
{
  /* Connect. We want to get a connection identifier filled in. This state can
     be entered from SETUP and from PENDING. */
  bool connected;
  CURLMcode mresult = CURLM_OK;
  CURLcode ফলাফল = Curl_connect(তথ্য, &connected);
  যদি(ফলাফল == CURLE_NO_CONNECTION_AVAILABLE) {
    /* There was no connection available. We will go to the pending state and
       wait for an available connection. */
    multistate(তথ্য, MSTATE_PENDING);
    /* move from process to pending set */
    Curl_uint32_bset_remove(&multi->process, তথ্য->mid);
    Curl_uint32_bset_remove(&multi->dirty, তথ্য->mid);
    Curl_uint32_bset_add(&multi->pending, তথ্য->mid);
    *resultp = CURLE_OK;
    ফেরত mresult;
  }
  অন্যথা
    multi_schedule_pending(তথ্য->multi);

  যদি(!ফলাফল) {
    /* after the connect has been sent off, go WAITCONNECT unless the
       protocol connect is already done and we can go directly to WAITDO or
       DO! */
    mresult = CURLM_CALL_MULTI_PERFORM;

    যদি(connected) {
      যদি(!তথ্য->conn->bits.reuse &&
         Curl_conn_is_multiplex(তথ্য->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_schedule_pending(তথ্য->multi);
      }
      multistate(তথ্য, MSTATE_PROTOCONNECT);
    }
    অন্যথা {
      multistate(তথ্য, MSTATE_CONNECTING);
    }
  }
  *resultp = ফলাফল;
  ফেরত mresult;
}

/* returns the possibly updated result */
স্থির CURLcode is_finished(কাঠামো Curl_multi *multi,
                            কাঠামো Curl_easy *তথ্য,
                            ধ্রুবক কাঠামো curltime *pnow,
                            bool stream_error,
                            CURLcode ফলাফল)
{
  যদি(তথ্য->mstate < MSTATE_COMPLETED) {
    যদি(ফলাফল) {
      /*
       * If an error was returned, and we are not in completed state now,
       * then we go to completed and consider this transfer aborted.
       */

      /* No attempt to disconnect connections must be made before this -
         connection detach and termination happens only here */

      /* Check if we can move pending requests to send pipe */
      multi_schedule_pending(multi); /* connection */

      যদি(তথ্য->conn) {
        যদি(stream_error) {
          /* Do not attempt to send data over a connection that timed out */
          bool dead_connection = ফলাফল == CURLE_OPERATION_TIMEDOUT;
          কাঠামো connectdata *conn = তথ্য->conn;

          /* This is where we make sure that the conn pointer is reset.
             We do not have to do this in every case block above where a
             failure is detected */
          Curl_detach_connection(তথ্য);
          Curl_conn_close(তথ্য, conn, dead_connection);
        }
      }
      অন্যথা যদি(তথ্য->mstate == MSTATE_CONNECT) {
        /* Curl_connect() failed */
        multi_posttransfer(তথ্য);
        Curl_pgrsUpdate_nometer(তথ্য);
      }

      multistate(তথ্য, MSTATE_COMPLETED);
      ফেরত ফলাফল;
    }
    /* if there is still a connection to use, call the progress function */
    অন্যথা যদি(তথ্য->conn && Curl_conn_is_connected(তথ্য->conn, FIRSTSOCKET)) {
      ফলাফল = Curl_pgrsUpdateX(তথ্য, pnow);
      যদি(ফলাফল) {
        /* aborted due to progress callback return code must close the
           connection */
        streamclose(তথ্য->conn);

        /* if not yet in DONE state, go there. */
        যদি(তথ্য->mstate < MSTATE_DONE)
          multistate(তথ্য, MSTATE_DONE);
        ফেরত ফলাফল;
      }
    }
  }
  ফেরত ফলাফল;
}

স্থির শূন্য handle_completed(কাঠামো Curl_multi *multi,
                             কাঠামো Curl_easy *তথ্য,
                             CURLcode ফলাফল)
{
  bool বার্তা_to_app = তথ্য->master_mid == UINT32_MAX;

  যদি(!বার্তা_to_app) {
    /* A sub transfer, not reported to the application. Is anyone still
     * interested in processing its results? */
    যদি(তথ্য->sub_xfer_done) {
      কাঠামো Curl_easy *master = Curl_multi_get_easy(multi, তথ্য->master_mid);

      CURL_TRC_M(তথ্য, "sub xfer done for master %u", তথ্য->master_mid);
      যদি(master)
        তথ্য->sub_xfer_done(তথ্য, master, ফলাফল);
      অন্যথা
        CURL_TRC_M(তথ্য, "master easy %u already gone.", তথ্য->master_mid);
    }
  }
  অন্যথা {
    /* now fill in the CURLMsg with this info */
    কাঠামো CURLMsg *বার্তা = &তথ্য->বার্তা;

    বার্তা->বার্তা = CURLMSG_DONE;
    বার্তা->easy_handle = তথ্য;
    বার্তা->তথ্য.ফলাফল = ফলাফল;

    DEBUGASSERT(!তথ্য->conn);
  }
  multistate(তথ্য, MSTATE_MSGSENT);

  /* remove from the other sets */
  Curl_uint32_bset_remove(&multi->process, তথ্য->mid);
  Curl_uint32_bset_remove(&multi->dirty, তথ্য->mid);
  Curl_uint32_bset_remove(&multi->pending, তথ্য->mid);
  যদি(বার্তা_to_app)
    multi_addmsg(multi, তথ্য);
  যদি(তথ্য->state.really_alive) {
    তথ্য->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    যদি(!multi->xfers_really_alive)
      (শূন্য)multi_assess_wakeup(multi);
  }
  --multi->xfers_alive;
  যদি(!multi->xfers_alive)
    multi_assess_wakeup(multi);
}

স্থির CURLMcode multistate_init(কাঠামো Curl_easy *তথ্য, CURLcode *ফলাফল)
{
  যদি(!তথ্য->state.really_alive) {
    তথ্য->state.really_alive = TRUE;
    ++তথ্য->multi->xfers_really_alive;
    যদি(তথ্য->multi->xfers_really_alive == 1) {
      CURLMcode mresult = multi_assess_wakeup(তথ্য->multi);
      যদি(mresult) {
        failf(তথ্য, "error enabling wakeup listening: %d", mresult);
        ফেরত mresult;
      }
    }
  }

  *ফলাফল = Curl_pretransfer(তথ্য);
  যদি(*ফলাফল)
    ফেরত CURLM_OK;

  /* after init, go SETUP */
  multistate(তথ্য, MSTATE_SETUP);
  Curl_pgrsTime(তথ্য, TIMER_STARTOP);
  ফেরত CURLM_CALL_MULTI_PERFORM;
}

স্থির CURLMcode multistate_setup(কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক কাঠামো curltime *pnow = Curl_pgrs_now(তথ্য);
  Curl_pgrsTimeWas(তথ্য, TIMER_STARTSINGLE, *pnow);
  যদি(তথ্য->set.timeout)
    Curl_expire_set(তথ্য, EXPIRE_TIMEOUT, তথ্য->set.timeout, pnow);
  যদি(তথ্য->set.connecttimeout)
    /* Since a connection might go to pending and back to CONNECT several
       times before it actually takes off, we need to set the timeout once
       in SETUP before we enter CONNECT the first time. */
    Curl_expire_set(তথ্য, EXPIRE_CONNECTTIMEOUT,
                    তথ্য->set.connecttimeout, pnow);

  multistate(তথ্য, MSTATE_CONNECT);
  ফেরত CURLM_CALL_MULTI_PERFORM;
}

স্থির CURLMcode multistate_connecting(কাঠামো Curl_easy *তথ্য,
                                       bool *stream_error,
                                       CURLcode *ফলাফল)
{
  bool connected;

  যদি(!তথ্য->conn) {
    DEBUGASSERT(0);
    *ফলাফল = CURLE_FAILED_INIT;
    ফেরত CURLM_OK;
  }
  যদি(!Curl_xfer_recv_is_paused(তথ্য)) {
    *ফলাফল = Curl_conn_connect(তথ্য, FIRSTSOCKET, FALSE, &connected);
    যদি(connected && !*ফলাফল) {
      যদি(!তথ্য->conn->bits.reuse &&
         Curl_conn_is_multiplex(তথ্য->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_schedule_pending(তথ্য->multi);
      }
      multistate(তথ্য, MSTATE_PROTOCONNECT);
      ফেরত CURLM_CALL_MULTI_PERFORM;
    }
    অন্যথা যদি(*ফলাফল) {
      /* failure detected */
      CURL_TRC_M(তথ্য, "connect failed -> %d", (পূর্ণসংখ্যা)*ফলাফল);
      multi_posttransfer(তথ্য);
      multi_done(তথ্য, *ফলাফল, TRUE);
      *stream_error = TRUE;
      ফেরত CURLM_OK;
    }
  }
  ফেরত CURLM_OK;
}

স্থির CURLMcode multistate_protoconnect(কাঠামো Curl_easy *তথ্য,
                                         bool *stream_error,
                                         CURLcode *ফলাফল)
{
  bool protocol_connected = FALSE;

  যদি(!*ফলাফল && তথ্য->conn->bits.reuse) {
    /* ftp seems to hang when protoconnect on reused connection since we
     * handle PROTOCONNECT in general inside the filters, it seems wrong to
     * restart this on a reused connection.
     */
    multistate(তথ্য, MSTATE_DO);
    ফেরত CURLM_CALL_MULTI_PERFORM;
  }
  যদি(!*ফলাফল)
    *ফলাফল = protocol_connect(তথ্য, &protocol_connected);
  যদি(!*ফলাফল && !protocol_connected) {
    /* switch to waiting state */
    multistate(তথ্য, MSTATE_PROTOCONNECTING);
    ফেরত CURLM_CALL_MULTI_PERFORM;
  }
  অন্যথা যদি(!*ফলাফল) {
    /* protocol connect has completed, go WAITDO or DO */
    multistate(তথ্য, MSTATE_DO);
    ফেরত CURLM_CALL_MULTI_PERFORM;
  }

  /* failure detected */
  multi_posttransfer(তথ্য);
  multi_done(তথ্য, *ফলাফল, TRUE);
  *stream_error = TRUE;
  ফেরত CURLM_OK;
}

স্থির CURLMcode multistate_protoconnecting(কাঠামো Curl_easy *তথ্য,
                                            bool *stream_error,
                                            CURLcode *ফলাফল)
{
  bool protocol_connected = FALSE;

  /* protocol-specific connect phase */
  *ফলাফল = protocol_connecting(তথ্য, &protocol_connected);
  যদি(!*ফলাফল && protocol_connected) {
    /* after the connect has completed, go WAITDO or DO */
    multistate(তথ্য, MSTATE_DO);
    ফেরত CURLM_CALL_MULTI_PERFORM;
  }
  অন্যথা যদি(*ফলাফল) {
    /* failure detected */
    multi_posttransfer(তথ্য);
    multi_done(তথ্য, *ফলাফল, TRUE);
    *stream_error = TRUE;
  }
  ফেরত CURLM_OK;
}

স্থির CURLMcode multistate_doing(কাঠামো Curl_easy *তথ্য,
                                  bool *stream_error,
                                  CURLcode *ফলাফল)
{
  bool dophase_done = FALSE;

  /* we continue DOING until the DO phase is complete */
  DEBUGASSERT(তথ্য->conn);
  *ফলাফল = protocol_doing(তথ্য, &dophase_done);
  যদি(!*ফলাফল) {
    যদি(dophase_done) {
      /* after DO, go DO_DONE or DO_MORE */
      multistate(তথ্য, তথ্য->conn->bits.do_more ?
                 MSTATE_DOING_MORE : MSTATE_DID);
      ফেরত CURLM_CALL_MULTI_PERFORM;
    } /* dophase_done */
  }
  অন্যথা {
    /* failure detected */
    multi_posttransfer(তথ্য);
    multi_done(তথ্য, *ফলাফল, FALSE);
    *stream_error = TRUE;
  }
  ফেরত CURLM_OK;
}

স্থির CURLMcode multistate_doing_more(কাঠামো Curl_easy *তথ্য,
                                       bool *stream_error,
                                       CURLcode *ফলাফল)
{
  domore control;

  /*
   * When we are connected, DOING MORE and then go DID
   */
  DEBUGASSERT(তথ্য->conn);
  *ফলাফল = multi_do_more(তথ্য, &control);

  যদি(!*ফলাফল) {
    যদি(control != DOMORE_INCOMPLETE) {
      /* if DONE, advance to DO_DONE
         if GOBACK, go back to DOING */
      multistate(তথ্য, control == DOMORE_DONE ? MSTATE_DID : MSTATE_DOING);
      ফেরত CURLM_CALL_MULTI_PERFORM;
    }
    /* else
       stay in DO_MORE */
  }
  অন্যথা {
    /* failure detected */
    multi_posttransfer(তথ্য);
    multi_done(তথ্য, *ফলাফল, FALSE);
    *stream_error = TRUE;
  }
  ফেরত CURLM_OK;
}

স্থির CURLMcode multistate_did(কাঠামো Curl_multi *multi,
                                কাঠামো Curl_easy *তথ্য)
{
  DEBUGASSERT(তথ্য->conn);
  যদি(তথ্য->conn->bits.multiplex)
    /* Check if we can move pending requests to send pipe */
    multi_schedule_pending(multi); /* multiplexed */

  /* Only perform the transfer if there is a good socket to work with.
     Having both BAD is a signal to skip immediately to DONE */
  যদি(CONN_SOCK_IDX_VALID(তথ্য->conn->recv_idx) ||
     CONN_SOCK_IDX_VALID(তথ্য->conn->send_idx)) {
    multistate(তথ্য, MSTATE_PERFORMING);
    /* Do not return CURLM_CALL_MULTI_PERFORM to give other transfers
     * a chance to send off their requests.
     * Note: Some SFTP handlers do not seem to like this.
     *       Restrict it to HTTP families. */
    ফেরত ((multi->xfers_alive > 1) &&
            (তথ্য->conn->scheme->protocol & PROTO_FAMILY_HTTP)) ?
           CURLM_OK : CURLM_CALL_MULTI_PERFORM;
  }
  অন্যথা {
#ifndef CURL_DISABLE_FTP
    যদি(তথ্য->state.wildcardmatch &&
       ((তথ্য->conn->scheme->flags & PROTOPT_WILDCARD) == 0)) {
      তথ্য->wildcard->state = CURLWC_DONE;
    }
#endif
    multistate(তথ্য, MSTATE_DONE);
    ফেরত CURLM_CALL_MULTI_PERFORM;
  }
}

স্থির CURLMcode multistate_done(কাঠামো Curl_easy *তথ্য, CURLcode *presult)
{
  যদি(তথ্য->conn) {
    CURLcode ফলাফল;

    /* post-transfer command */
    ফলাফল = multi_done(তথ্য, *presult, FALSE);

    /* allow a previously set error code take precedence */
    যদি(!(*presult))
      *presult = ফলাফল;
  }

#ifndef CURL_DISABLE_FTP
  যদি(তথ্য->state.wildcardmatch) {
    যদি(তথ্য->wildcard->state != CURLWC_DONE) {
      /* if a wildcard is set and we are not ending -> lets start again
         with MSTATE_INIT */
      multistate(তথ্য, MSTATE_INIT);
      ফেরত CURLM_CALL_MULTI_PERFORM;
    }
  }
#endif
  /* after we have DONE what we are supposed to do, go COMPLETED, and
     it does not matter what the multi_done() returned! */
  multistate(তথ্য, MSTATE_COMPLETED);
  ফেরত CURLM_CALL_MULTI_PERFORM;
}

স্থির CURLMcode multi_runsingle(কাঠামো Curl_multi *multi,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো Curl_sigpipe_ctx *sigpipe_ctx)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক কাঠামো curltime *pnow = ফাঁকা;

  যদি(multi->dead) {
    /* a multi-level callback returned error before, meaning every individual
     transfer now has failed */
    ফলাফল = CURLE_ABORTED_BY_CALLBACK;
    multi_posttransfer(তথ্য);
    multi_done(তথ্য, ফলাফল, FALSE);
    multistate(তথ্য, MSTATE_COMPLETED);
  }

  multi_warn_debug(multi, তথ্য);

  /* transfer runs now, clear the dirty bit. This may be set
   * again during processing, triggering a re-run later. */
  Curl_uint32_bset_remove(&multi->dirty, তথ্য->mid);

  যদি(তথ্য == multi->admin) {
#ifdef ENABLE_INTERNAL_WAKEUP
    /* Consume any pending wakeup signals before processing.
     * This is necessary for event based processing. See #21547 */
    (শূন্য)Curl_wakeup_consume(multi->wakeup_internal, TRUE);
#endif
#ifdef USE_RESOLV_THREADED
    Curl_async_thrdd_multi_process(multi);
#endif
    Curl_cshutdn_perform(&multi->cshutdn, multi->admin, sigpipe_ctx);
    যাও out;
  }

  sigpipe_apply(তথ্য, sigpipe_ctx);
  করো {
    /* A "stream" here is a logical stream if the protocol can handle that
       (HTTP/2), or the full connection for older protocols */
    bool stream_error = FALSE;
    mresult = CURLM_OK;
    pnow = ফাঁকা;

    যদি(multi_ischanged(multi, TRUE)) {
      CURL_TRC_M(তথ্য, "multi changed, check CONNECT_PEND queue");
      multi_schedule_pending(multi); /* multiplexed */
    }

    যদি(তথ্য->mstate > MSTATE_CONNECT &&
       তথ্য->mstate < MSTATE_COMPLETED) {
      /* Make sure we set the connection's current owner */
      DEBUGASSERT(তথ্য->conn);
      যদি(!তথ্য->conn) {
        mresult = CURLM_INTERNAL_ERROR;
        যাও out;
      }
    }

    /* Wait for the connect state as only then is the start time stored, but
       we must not check already completed handles */
    যদি((তথ্য->mstate >= MSTATE_CONNECT) && (তথ্য->mstate < MSTATE_COMPLETED)) {
      pnow = Curl_pgrs_now(তথ্য);
      যদি(multi_handle_timeout(তথ্য, pnow, &stream_error, &ফলাফল))
        /* Skip the statemachine and go directly to error handling section. */
        যাও statemachine_end;
      pnow = ফাঁকা;
    }

    নির্বাচন(তথ্য->mstate) {
    ক্ষেত্র MSTATE_INIT:
      /* Transitional state. init this transfer. A handle never comes back to
         this state. */
      mresult = multistate_init(তথ্য, &ফলাফল);
      বিরতি;

    ক্ষেত্র MSTATE_SETUP:
      /* Transitional state. Setup things for a new transfer. The handle
         can come back to this state on a redirect. */
      mresult = multistate_setup(তথ্য);
      বিরতি;

    ক্ষেত্র MSTATE_CONNECT:
      mresult = multistate_connect(multi, তথ্য, &ফলাফল);
      বিরতি;

    ক্ষেত্র MSTATE_CONNECTING:
      /* awaiting a completion of an asynch TCP connect */
      mresult = multistate_connecting(তথ্য, &stream_error, &ফলাফল);
      বিরতি;

    ক্ষেত্র MSTATE_PROTOCONNECT:
      mresult = multistate_protoconnect(তথ্য, &stream_error, &ফলাফল);
      বিরতি;

    ক্ষেত্র MSTATE_PROTOCONNECTING:
      /* protocol-specific connect phase */
      mresult = multistate_protoconnecting(তথ্য, &stream_error, &ফলাফল);
      বিরতি;

    ক্ষেত্র MSTATE_DO:
      mresult = multistate_do(তথ্য, &stream_error, &ফলাফল);
      বিরতি;

    ক্ষেত্র MSTATE_DOING:
      /* we continue DOING until the DO phase is complete */
      mresult = multistate_doing(তথ্য, &stream_error, &ফলাফল);
      বিরতি;

    ক্ষেত্র MSTATE_DOING_MORE:
      /*
       * When we are connected, DOING MORE and then go DID
       */
      mresult = multistate_doing_more(তথ্য, &stream_error, &ফলাফল);
      বিরতি;

    ক্ষেত্র MSTATE_DID:
      mresult = multistate_did(multi, তথ্য);
      বিরতি;

    ক্ষেত্র MSTATE_RATELIMITING: /* limit-rate exceeded in either direction */
      mresult = multistate_ratelimiting(তথ্য, &ফলাফল);
      বিরতি;

    ক্ষেত্র MSTATE_PERFORMING:
      mresult = multistate_performing(তথ্য, &stream_error, &ফলাফল);
      বিরতি;

    ক্ষেত্র MSTATE_DONE:
      mresult = multistate_done(তথ্য, &ফলাফল);
      বিরতি;

    ক্ষেত্র MSTATE_COMPLETED:
      বিরতি;

    ক্ষেত্র MSTATE_PENDING:
    ক্ষেত্র MSTATE_MSGSENT:
      /* handles in these states should NOT be in this list */
      বিরতি;

    পূর্বনির্ধারিত:
      mresult = CURLM_INTERNAL_ERROR;
      যাও out;
    }

    যদি(তথ্য->mstate >= MSTATE_CONNECT &&
       তথ্য->mstate < MSTATE_DO &&
       mresult != CURLM_CALL_MULTI_PERFORM &&
       !multi_ischanged(multi, FALSE)) {
      /* We now handle stream timeouts if and only if this will be the last
       * loop iteration. We only check this on the last iteration to ensure
       * that if we know we have additional work to do immediately
       * (i.e. CURLM_CALL_MULTI_PERFORM == TRUE) then we should do that before
       * declaring the connection timed out as we may almost have a completed
       * connection. */
      pnow = Curl_pgrs_now(তথ্য);
      multi_handle_timeout(তথ্য, pnow, &stream_error, &ফলাফল);
    }

statemachine_end:
    যদি(!pnow)
      pnow = Curl_pgrs_now(তথ্য);
    ফলাফল = is_finished(multi, তথ্য, pnow, stream_error, ফলাফল);
    যদি(ফলাফল)
      mresult = CURLM_CALL_MULTI_PERFORM;

    যদি(MSTATE_COMPLETED == তথ্য->mstate) {
      handle_completed(multi, তথ্য, ফলাফল);
      mresult = CURLM_OK;
      যাও out;
    }
  } যখন((mresult == CURLM_CALL_MULTI_PERFORM) ||
          multi_ischanged(multi, FALSE));

out:
  তথ্য->ফলাফল = ফলাফল;
  ফেরত mresult;
}

স্থির CURLMcode multi_perform(কাঠামো Curl_multi *multi,
                               পূর্ণসংখ্যা *running_handles)
{
  CURLMcode returncode = CURLM_OK;
  কাঠামো curltime start;
  uint32_t mid;
  কাঠামো Curl_sigpipe_ctx sigpipe_ctx;

  curlx_pnow(&start);
  sigpipe_init(&sigpipe_ctx);

  যদি(Curl_uint32_bset_first(&multi->process, &mid)) {
    CURL_TRC_M(multi->admin, "multi_perform(running=%u)",
               multi_xfers_running(multi));
    করো {
      কাঠামো Curl_easy *তথ্য = Curl_multi_get_easy(multi, mid);
      CURLMcode mresult;
      যদি(!তথ্য) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        চালু;
      }
      mresult = multi_runsingle(multi, তথ্য, &sigpipe_ctx);
      যদি(mresult)
        returncode = mresult;
    } যখন(Curl_uint32_bset_পরবর্তী(&multi->process, mid, &mid));
  }
  sigpipe_restore(&sigpipe_ctx);

  যদি(multi_ischanged(multi, TRUE))
    multi_schedule_pending(multi);

  যদি(!returncode && CURL_MNTFY_HAS_ENTRIES(multi))
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
  যখন(Curl_timeouts_remove_expired(&multi->timeouts, &start, &mid)) {
    /* the removed may have another timeout in queue */
    কাঠামো Curl_easy *তথ্য = Curl_multi_get_easy(multi, mid);
    যদি(!তথ্য) {
      DEBUGASSERT(0);
      চালু;
    }
    (শূন্য)add_পরবর্তী_timeout(&start, multi, তথ্য);
    যদি(তথ্য->mstate == MSTATE_PENDING) {
      bool stream_unused;
      CURLcode ফলাফল_unused;
      যদি(multi_handle_timeout(তথ্য, Curl_pgrs_now(তথ্য),
                              &stream_unused, &ফলাফল_unused)) {
        infof(তথ্য, "PENDING handle timeout");
        move_pending_to_connect(multi, তথ্য);
      }
    }
  }

  যদি(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < INT_MAX) ? (পূর্ণসংখ্যা)running : INT_MAX;
  }

  যদি(CURLM_OK >= returncode)
    returncode = Curl_update_timer(multi);

  ফেরত returncode;
}

CURLMcode curl_multi_perform(CURLM *m, পূর্ণসংখ্যা *running_handles)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_perform, &mresult)) {
    mresult = multi_perform(m, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

CURLMcode curl_multi_cleanup(CURLM *m)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_cleanup, &mresult)) {
    কাঠামো Curl_multi *multi = m;
    শূন্য *entry;
    uint32_t mid;

    /* First remove all remaining easy handles,
     * close internal ones. admin handle is special */
    যদি(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
      করো {
        কাঠামো Curl_easy *তথ্য = entry;
        যদি(!GOOD_EASY_HANDLE(তথ্য)) {
          mresult = CURLM_BAD_HANDLE;
          যাও out;
        }

#ifdef DEBUGBUILD
        যদি(mid != তথ্য->mid) {
          CURL_TRC_M(তথ্য, "multi_cleanup: still present with mid=%u, "
                     "but unexpected data->mid=%u\n", mid, তথ্য->mid);
          DEBUGASSERT(0);
        }
#endif

        যদি(তথ্য == multi->admin)
          চালু;

        যদি(!তথ্য->state.done && তথ্য->conn)
          /* if DONE was never called for this handle */
          (শূন্য)multi_done(তথ্য, CURLE_OK, TRUE);

        তথ্য->multi = ফাঁকা; /* clear the association */
        Curl_uint32_tbl_remove(&multi->xfers, mid);
        তথ্য->mid = UINT32_MAX;

#ifdef USE_LIBPSL
        যদি(তথ্য->psl == &multi->psl)
          তথ্য->psl = ফাঁকা;
#endif
        যদি(তথ্য->state.internal)
          Curl_close(&তথ্য);
      } যখন(Curl_uint32_tbl_পরবর্তী(&multi->xfers, mid, &mid, &entry));
    }

#ifdef USE_RESOLV_THREADED
    Curl_async_thrdd_multi_destroy(multi, !multi->quick_exit);
#endif
    Curl_cpool_destroy(&multi->cpool, multi->admin);
    Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
    যদি(multi->admin) {
      CURL_TRC_M(multi->admin, "multi_cleanup, closing admin handle, done");
      multi->admin->multi = ফাঁকা;
      Curl_uint32_tbl_remove(&multi->xfers, multi->admin->mid);
      Curl_close(&multi->admin);
    }

    multi->magic = 0; /* not good anymore */

    Curl_multi_ev_cleanup(multi);
    Curl_hash_destroy(&multi->proto_hash);
    Curl_dnscache_destroy(&multi->dnscache);
    Curl_psl_destroy(&multi->psl);
#ifdef USE_SSL
    Curl_ssl_scache_destroy(multi->ssl_scache);
#endif

#ifdef USE_WINSOCK
    WSACloseEvent(multi->wsa_event);
#endif
#ifdef ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#endif
#ifdef ENABLE_INTERNAL_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_internal);
#endif

    multi_xfer_bufs_free(multi);
    Curl_mntfy_cleanup(multi);
#ifdef DEBUGBUILD
    যদি(Curl_uint32_tbl_গুনতি(&multi->xfers)) {
      multi_xfer_tbl_dump(multi);
      DEBUGASSERT(0);
    }
#endif
    Curl_uint32_bset_destroy(&multi->process);
    Curl_uint32_bset_destroy(&multi->dirty);
    Curl_uint32_bset_destroy(&multi->pending);
    Curl_uint32_bset_destroy(&multi->msgsent);
    Curl_uint32_tbl_destroy(&multi->xfers);
    curlx_memzero(multi, আকার(*multi));
    curlx_free(multi);

    mresult = CURLM_OK;
  }
out:
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

/*
 * curl_multi_info_read()
 *
 * This function is the primary way for a multi/multi_socket application to
 * figure out if a transfer has ended.
 */

CURLMsg *curl_multi_info_read(CURLM *m, পূর্ণসংখ্যা *msgs_in_queue)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMsg *বার্তা_ফলাফল = ফাঁকা;

  *msgs_in_queue = 0; /* default to none */
  যদি(CURL_MAPI_ENTER(&guard, m, multi_info_read, ফাঁকা)) {
    কাঠামো Curl_multi *multi = m;
    uint32_t mid;
    যদি(Curl_uint32_bset_first(&multi->msgsent, &mid)) {
      কাঠামো Curl_easy *তথ্য = Curl_multi_get_easy(multi, mid);

      DEBUGASSERT(তথ্য);
      Curl_uint32_bset_remove(&multi->msgsent, mid);
      *msgs_in_queue =
        curlx_uztosi(Curl_uint32_bset_গুনতি(&multi->msgsent));
      যদি(তথ্য)
        বার্তা_ফলাফল = &তথ্য->বার্তা;
    }
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত বার্তা_ফলাফল;
}

শূন্য Curl_multi_will_close(কাঠামো Curl_easy *তথ্য, curl_socket_t s)
{
  যদি(তথ্য) {
    কাঠামো Curl_multi *multi = তথ্য->multi;
    যদি(multi) {
      CURL_TRC_M(তথ্য, "Curl_multi_will_close fd=%" FMT_SOCKET_T, s);
      Curl_multi_ev_socket_done(multi, তথ্য, s);
    }
  }
}

স্থির শূন্য multi_timeouts_init(কাঠামো Curl_easy *তথ্য)
{
  তথ্য->state.timeouts.first = EXPIRE_LAST;
  তথ্য->state.timeouts.splaynode.registered = FALSE;
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
স্থির CURLMcode add_পরবর্তী_timeout(ধ্রুবক কাঠামো curltime *pnow,
                                  কাঠামো Curl_multi *multi,
                                  কাঠামো Curl_easy *তথ্য)
{
  কাঠামো expire_timers *timeouts = &তথ্য->state.timeouts;
  timediff_t now_us = Curl_timeouts_offset_us(&multi->timeouts, pnow);

  যখন(timeouts->first < EXPIRE_LAST) {
    যদি(timeouts->offset_us[timeouts->first] <= now_us)  /* already expired */
      timeouts->first = timeouts->পরবর্তী[timeouts->first];
    অন্যথা /* timeouts are sorted, first is first in the future now */
      বিরতি;
  }

  যদি(timeouts->first < EXPIRE_LAST) {
    /* Insert this node again into the splay. Keep the timer in the list in
       case we need to recompute future timers. */
    Curl_timeouts_add(&multi->timeouts, তথ্য,
                      timeouts->offset_us[timeouts->first]);
  }
  ফেরত CURLM_OK;
}

স্থির শূন্য multi_mark_expired_as_dirty(কাঠামো Curl_multi *multi,
                                        ধ্রুবক কাঠামো curltime *ts)
{
  কাঠামো Curl_easy *তথ্য = ফাঁকা;
  uint32_t mid;

  /*
   * The loop following here will go on as long as there are expire-times left
   * to process (compared to `ts`) in the splay and 'data' will be
   * re-assigned for every expired handle we deal with.
   */
  যখন(Curl_timeouts_remove_expired(&multi->timeouts, ts, &mid)) {
    /* Check if there is one (more) expired timer to deal with! This function
       extracts a matching node if there is one */
    তথ্য = Curl_multi_get_easy(multi, mid);
    যদি(!তথ্য) {
      DEBUGASSERT(0);
      চালু;
    }
#ifdef CURLVERBOSE
    যদি(CURL_TRC_TIMER_is_verbose(তথ্য)) {
      যদি(তথ্য->state.timeouts.first < EXPIRE_LAST) {
        CURL_TRC_TIMER(তথ্য, তথ্য->state.timeouts.first, "has expired");
      }
    }
#endif
    (শূন্য)add_পরবর্তী_timeout(ts, multi, তথ্য);
    Curl_multi_mark_dirty(তথ্য);
  }
}

স্থির CURLMcode multi_run_dirty(কাঠামো Curl_multi *multi,
                                 কাঠামো Curl_sigpipe_ctx *sigpipe_ctx,
                                 uint32_t *pnum)
{
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;

  *pnum = 0;
  যদি(Curl_uint32_bset_first(&multi->dirty, &mid)) {
    করো {
      কাঠামো Curl_easy *তথ্য = Curl_multi_get_easy(multi, mid);
      যদি(তথ্য) {
        CURL_TRC_M(তথ্য, "multi_run_dirty");

        যদি(!Curl_uint32_bset_contains(&multi->process, mid)) {
          যদি(Curl_uint32_bset_contains(&multi->pending, mid)) {
            /* Something happened on the pending transfer, act on it. */
            move_pending_to_connect(multi, তথ্য);
          }
          অন্যথা {
            /* We are no longer processing this transfer */
            Curl_uint32_bset_remove(&multi->dirty, mid);
            চালু;
          }
        }

        (*pnum)++;
        /* runsingle() clears the dirty mid */
        mresult = multi_runsingle(multi, তথ্য, sigpipe_ctx);

        যদি(CURLM_OK >= mresult) {
          /* reassess event handling of data */
          mresult = Curl_multi_ev_assess_xfer(multi, তথ্য);
          যদি(mresult)
            যাও out;
        }
      }
      অন্যথা {
        CURL_TRC_M(multi->admin, "multi_run_dirty, %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } যখন(Curl_uint32_bset_পরবর্তী(&multi->dirty, mid, &mid));
  }

out:
  ফেরত mresult;
}

স্থির CURLMcode multi_socket(কাঠামো Curl_multi *multi,
                              bool checkall,
                              curl_socket_t s,
                              পূর্ণসংখ্যা ev_bitmask,
                              পূর্ণসংখ্যা *running_handles)
{
  CURLMcode mresult = CURLM_OK;
  কাঠামো Curl_sigpipe_ctx pipe_ctx;
  কাঠামো curltime now;
  uint32_t run_xfers;

  (শূন্য)ev_bitmask;
  sigpipe_init(&pipe_ctx);

  যদি(checkall) {
    /* *perform() deals with running_handles on its own */
    mresult = multi_perform(multi, running_handles);

    যদি(mresult != CURLM_BAD_HANDLE) {
      /* Reassess event status of all active transfers, but keep any error
         from multi_perform() */
      CURLMcode mresult2 =
        Curl_multi_ev_assess_xfer_bset(multi, &multi->process);
      যদি(!mresult)
        mresult = mresult2;
    }
    যাও out;
  }

  যদি(s != CURL_SOCKET_TIMEOUT) {
    /* Mark all transfers of that socket as dirty */
    Curl_multi_ev_dirty_xfers(multi, s);
  }
  অন্যথা {
    /* Asked to run due to time-out. Clear the 'last_expire_ts' variable to
       force Curl_update_timer() to trigger a callback to the app again even
       if the same timeout is still the one to run after this call. That
       handles the case when the application asks libcurl to run the timeout
       prematurely. */
    multi->last_expire_offset_us = 0;

    /* Applications may set `socket_cb` *after* having added transfers
     * first. *Then* kick off processing with a
     * curl_multi_socket_action(TIMEOUT) afterwards. Make sure our
     * admin handle registers its pollset with the callbacks present. */
    mresult = multi_assess_wakeup(multi);
    যদি(mresult)
      যাও out;
  }

  curlx_pnow(&now);
  multi_mark_expired_as_dirty(multi, &now);
  mresult = multi_run_dirty(multi, &pipe_ctx, &run_xfers);
  যদি(mresult)
    যাও out;

  যদি(run_xfers) {
    /* Running transfers takes time. With a new timestamp, we might catch
     * other expires which are due now. Instead of telling the application
     * to set a 0 timeout and call us again, we run them here.
     * Do that only once or it might be unfair to transfers on other
     * sockets. */
    curlx_pnow(&now);
    multi_mark_expired_as_dirty(multi, &now);
    mresult = multi_run_dirty(multi, &pipe_ctx, &run_xfers);
  }

out:
  sigpipe_restore(&pipe_ctx);

  যদি(multi_ischanged(multi, TRUE))
    multi_schedule_pending(multi);

  যদি(!mresult && CURL_MNTFY_HAS_ENTRIES(multi))
    mresult = Curl_mntfy_dispatch_all(multi);

  যদি(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < INT_MAX) ? (পূর্ণসংখ্যা)running : INT_MAX;
  }

  যদি(CURLM_OK >= mresult)
    mresult = Curl_update_timer(multi);
  ফেরত mresult;
}

#undef curl_multi_setopt
CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_setopt, &mresult)) {
    কাঠামো Curl_multi *multi = m;
    va_list param;
    unsigned long uarg;
    size_t szarg;

    va_start(param, option);

    নির্বাচন(option) {
    ক্ষেত্র CURLMOPT_SOCKETFUNCTION:
      multi->socket_cb = va_arg(param, curl_socket_callback);
      বিরতি;
    ক্ষেত্র CURLMOPT_SOCKETDATA:
      multi->socket_userp = va_arg(param, শূন্য *);
      বিরতি;
    ক্ষেত্র CURLMOPT_PUSHFUNCTION:
      multi->ঠেলো_cb = va_arg(param, curl_ঠেলো_callback);
      বিরতি;
    ক্ষেত্র CURLMOPT_PUSHDATA:
      multi->ঠেলো_userp = va_arg(param, শূন্য *);
      বিরতি;
    ক্ষেত্র CURLMOPT_PIPELINING:
      multi->multiplexing = va_arg(param, long) & CURLPIPE_MULTIPLEX ? 1 : 0;
      বিরতি;
    ক্ষেত্র CURLMOPT_TIMERFUNCTION:
      multi->timer_cb = va_arg(param, curl_multi_timer_callback);
      বিরতি;
    ক্ষেত্র CURLMOPT_TIMERDATA:
      multi->timer_userp = va_arg(param, শূন্য *);
      বিরতি;
    ক্ষেত্র CURLMOPT_MAXCONNECTS:
      uarg = va_arg(param, unsigned long);
      যদি(uarg <= UINT32_MAX)
        multi->maxconnects = (uint32_t)uarg;
      বিরতি;
    ক্ষেত্র CURLMOPT_MAX_HOST_CONNECTIONS:
      যদি(!curlx_sltouz(va_arg(param, long), &szarg))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      multi->max_host_connections = (szarg < UINT32_MAX) ?
                                    (uint32_t)szarg : UINT32_MAX;
      বিরতি;
    ক্ষেত্র CURLMOPT_MAX_যোগ_CONNECTIONS:
      যদি(!curlx_sltouz(va_arg(param, long), &szarg))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      multi->max_যোগ_connections = (szarg < UINT32_MAX) ?
                                     (uint32_t)szarg : UINT32_MAX;
      বিরতি;
      /* options formerly used for pipelining */
    ক্ষেত্র CURLMOPT_MAX_PIPELINE_দৈর্ঘ্য:
      বিরতি;
    ক্ষেত্র CURLMOPT_CONTENT_দৈর্ঘ্য_PENALTY_আকার:
      বিরতি;
    ক্ষেত্র CURLMOPT_CHUNK_দৈর্ঘ্য_PENALTY_আকার:
      বিরতি;
    ক্ষেত্র CURLMOPT_PIPELINING_SITE_BL:
      বিরতি;
    ক্ষেত্র CURLMOPT_PIPELINING_SERVER_BL:
      বিরতি;
    ক্ষেত্র CURLMOPT_MAX_CONCURRENT_STREAMS: {
      যদি(!curlx_sltouz(va_arg(param, long), &szarg) ||
         !szarg || (szarg > (size_t)INT_MAX)) /* preserve previous cutoff */
        multi->max_concurrent_streams = 100;
      অন্যথা
        multi->max_concurrent_streams = (szarg < UINT32_MAX) ?
                                       (uint32_t)szarg : UINT32_MAX;
      বিরতি;
    }
    ক্ষেত্র CURLMOPT_NETWORK_CHANGED: {
      long val = va_arg(param, long);
      যদি(val & CURLMNWC_CLEAR_ALL)
        /* In the beginning, all values available to set were 1 by mistake. We
           converted this to mean "all", thus setting all the bits
           automatically */
        val = CURLMNWC_CLEAR_DNS | CURLMNWC_CLEAR_CONNS;
      যদি(val & CURLMNWC_CLEAR_DNS) {
        Curl_dnscache_clear(multi->admin);
      }
      যদি(val & CURLMNWC_CLEAR_CONNS) {
        Curl_cpool_nw_changed(&multi->cpool, multi->admin);
      }
      বিরতি;
    }
    ক্ষেত্র CURLMOPT_NOTIFYFUNCTION:
      multi->ntfy.ntfy_cb = va_arg(param, curl_notify_callback);
      বিরতি;
    ক্ষেত্র CURLMOPT_NOTIFYDATA:
      multi->ntfy.ntfy_cb_তথ্য = va_arg(param, শূন্য *);
      বিরতি;
    ক্ষেত্র CURLMOPT_RESOLVE_THREADS_MAX:
#ifdef USE_RESOLV_THREADED
      uarg = va_arg(param, long);
      যদি((uarg <= 0) || (uarg > UINT32_MAX))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      অন্যথা {
        CURLcode ফলাফল = Curl_async_thrdd_multi_set_props(
          multi, 0, (uint32_t)uarg, 2000);
        নির্বাচন(ফলাফল) {
        ক্ষেত্র CURLE_OK:
          mresult = CURLM_OK;
          বিরতি;
        ক্ষেত্র CURLE_BAD_FUNCTION_ARGUMENT:
          mresult = CURLM_BAD_FUNCTION_ARGUMENT;
          বিরতি;
        ক্ষেত্র CURLE_OUT_OF_MEMORY:
          mresult = CURLM_OUT_OF_MEMORY;
          বিরতি;
        পূর্বনির্ধারিত:
          mresult = CURLM_INTERNAL_ERROR;
          বিরতি;
        }
      }
#endif
      বিরতি;
    ক্ষেত্র CURLMOPT_QUICK_EXIT:
      multi->quick_exit = va_arg(param, long) ? 1 : 0;
      বিরতি;
    পূর্বনির্ধারিত:
      mresult = CURLM_UNKNOWN_OPTION;
      বিরতি;
    }
    va_end(param);
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

/* we define curl_multi_socket() in the public multi.h header */
#undef curl_multi_socket

CURLMcode curl_multi_socket(CURLM *m, curl_socket_t s, পূর্ণসংখ্যা *running_handles)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_socket, &mresult)) {
    mresult = multi_socket(m, FALSE, s, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

CURLMcode curl_multi_socket_action(CURLM *m, curl_socket_t s,
                                   পূর্ণসংখ্যা ev_bitmask, পূর্ণসংখ্যা *running_handles)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_socket_action, &mresult)) {
    mresult = multi_socket(m, FALSE, s, ev_bitmask, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

CURLMcode curl_multi_socket_all(CURLM *m, পূর্ণসংখ্যা *running_handles)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_socket_all, &mresult)) {
    mresult = multi_socket(m, TRUE, CURL_SOCKET_BAD, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

স্থির bool multi_has_dirties(কাঠামো Curl_multi *multi)
{
  uint32_t mid;
  যদি(Curl_uint32_bset_first(&multi->dirty, &mid)) {
    করো {
      কাঠামো Curl_easy *তথ্য = Curl_multi_get_easy(multi, mid);
      যদি(তথ্য) {
        যদি(Curl_uint32_bset_contains(&multi->process, mid))
          ফেরত TRUE;
        /* We are no longer processing this transfer */
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
      অন্যথা {
        CURL_TRC_M(multi->admin, "dirty transfer %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } যখন(Curl_uint32_bset_পরবর্তী(&multi->dirty, mid, &mid));
  }
  ফেরত FALSE;
}

স্থির শূন্য multi_timeout(কাঠামো Curl_multi *multi,
                          timediff_t *pexire_offset_us,
                          পূর্ণসংখ্যা *timeout_ms)
{
  যদি(multi->dead) {
    যদি(pexire_offset_us)
      *pexire_offset_us = 0;
    *timeout_ms = 0;
    ফেরত;
  }

  যদি(multi_has_dirties(multi)) {
    যদি(pexire_offset_us) {
      কাঠামো curltime now;
      curlx_pnow(&now);
      *pexire_offset_us = Curl_timeouts_offset_us(&multi->timeouts, &now);
    }
    *timeout_ms = 0;
    ফেরত;
  }
  অন্যথা {
    কাঠামো curltime now;
    uint32_t mid;

    curlx_pnow(&now);
    *timeout_ms = Curl_timeouts_পরবর্তী_ms(&multi->timeouts, &now,
                                        pexire_offset_us, &mid);
#ifdef CURLVERBOSE
    যদি(mid != UINT32_MAX) {
      কাঠামো Curl_easy *তথ্য = Curl_multi_get_easy(multi, mid);
      যদি(তথ্য && CURL_TRC_TIMER_is_verbose(তথ্য) &&
         (তথ্য->state.timeouts.first < EXPIRE_LAST)) {
        CURL_TRC_TIMER(তথ্য, তথ্য->state.timeouts.first,
                       "gives multi timeout in %dms", *timeout_ms);
      }
    }
#endif
  }
}

CURLMcode curl_multi_timeout(CURLM *m,
                             long *timeout_ms)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_timeout, &mresult)) {
    পূর্ণসংখ্যা itimeout_ms;

    multi_timeout(m, ফাঁকা, &itimeout_ms);
    *timeout_ms = (long)itimeout_ms;
    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

/*
 * Tell the application it should update its timers, if it subscribes to the
 * update timer callback.
 */
CURLMcode Curl_update_timer(কাঠামো Curl_multi *multi)
{
  timediff_t timeouts_offset_us = 0;
  পূর্ণসংখ্যা timeout_ms;
  পূর্ণসংখ্যা rc;
  bool set_মান = FALSE;

  যদি(!multi->timer_cb || multi->dead)
    ফেরত CURLM_OK;
  multi_timeout(multi, &timeouts_offset_us, &timeout_ms);

  যদি(timeout_ms < 0 && !multi->last_timeout_set) {
    /* nothing to do */
  }
  অন্যথা যদি(timeout_ms < 0) {
    /* there is no timeout now but there was one previously */
    CURL_TRC_M(multi->admin, "[TIMER] clear");
    timeout_ms = -1; /* normalize */
    set_মান = TRUE;
  }
  অন্যথা যদি(!multi->last_timeout_set) {
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, none before", timeout_ms);
    set_মান = TRUE;
  }
  অন্যথা যদি(multi->last_expire_offset_us != timeouts_offset_us) {
    /* We had a timeout before and have one now, the absolute timestamp
     * differs. The relative timeout_ms may be the same, but the starting
     * point differs. Let the application restart its timer. */
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, replace previous",
               timeout_ms);
    set_মান = TRUE;
  }
  অন্যথা {
    /* We have same expire time as previously. Our relative 'timeout_ms'
     * may be different now, but the application has the timer running
     * and we do not to tell it to start this again. */
  }

  যদি(set_মান) {
    কাঠামো Curl_mapi_guard guard;

    multi->last_expire_offset_us = timeouts_offset_us;
    multi->last_timeout_set = timeout_ms >= 0;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_timer_cb);
    rc = multi->timer_cb(multi, timeout_ms, multi->timer_userp);
    CURL_CBAPI_MULTI_END(&guard);
    যদি(rc == -1) {
      multi->dead = TRUE;
      ফেরত CURLM_ABORTED_BY_CALLBACK;
    }
  }
  ফেরত CURLM_OK;
}

#ifdef DEBUGBUILD
স্থির bool multi_timeouts_check(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো expire_timers *timeouts = &তথ্য->state.timeouts;
  uint8_t id;
  পূর্ণসংখ্যা i = 0;
  জন্য(id = timeouts->first; id < EXPIRE_LAST; id = timeouts->পরবর্তী[id]) {
    যদি(++i >= EXPIRE_LAST) {
      failf(তথ্য, "expire timeouts looped: %d iterations and no end", i);
      ফেরত FALSE;
    }
    যদি(id == timeouts->পরবর্তী[id]) {
      failf(তথ্য, "expire timeouts wrong: %d points to itself", (পূর্ণসংখ্যা)id);
      ফেরত FALSE;
    }
    যদি((timeouts->পরবর্তী[id] < EXPIRE_LAST) &&
       (timeouts->offset_us[id] > timeouts->offset_us[timeouts->পরবর্তী[id]])) {
      failf(তথ্য, "expire timeouts not sorted: %d happens after %d but "
            "is listed before", (পূর্ণসংখ্যা)id, (পূর্ণসংখ্যা)timeouts->পরবর্তী[id]);
      ফেরত FALSE;
    }
  }
  ফেরত TRUE;
}
#endif

/*
 * Remove a given timestamp from the list of timeouts.
 */
স্থির শূন্য multi_clear_timeout(কাঠামো Curl_easy *তথ্য, expire_id eid)
{
  কাঠামো expire_timers *timeouts = &তথ্য->state.timeouts;
  uint8_t orig_first = timeouts->first;
  uint8_t *anchor = &timeouts->first;
  uint8_t id = (uint8_t)eid;

  যদি((unsigned)eid >= EXPIRE_LAST) {
    DEBUGASSERT(0);
    ফেরত;
  }

  যখন(*anchor < EXPIRE_LAST) {
    যদি(*anchor == id) {
      *anchor = timeouts->পরবর্তী[id];
      বিরতি;
    }
    anchor = &timeouts->পরবর্তী[*anchor];
  }
  DEBUGASSERT(multi_timeouts_check(তথ্য));
  যদি(Curl_timeouts_has(তথ্য)) {
    কাঠামো Curl_multi *multi = তথ্য->multi;

    যদি(!multi) {
      DEBUGASSERT(0);
      ফেরত;
    }
    যদি((timeouts->first >= EXPIRE_LAST) || /* no more timeouts */
       (timeouts->first != orig_first)) {  /* active timeout changed */
      Curl_timeouts_remove(&multi->timeouts, তথ্য);
    }
    যদি((timeouts->first < EXPIRE_LAST) && !Curl_timeouts_has(তথ্য)) {
      Curl_timeouts_add(&multi->timeouts, তথ্য,
                        timeouts->offset_us[timeouts->first]);
    }
  }
}

/*
 * Add a timestamp to the list of timeouts. Keep the list sorted so that head
 * of list is always the timeout nearest in time.
 */
স্থির CURLMcode multi_set_timeout(কাঠামো Curl_easy *তথ্য,
                                   ধ্রুবক কাঠামো curltime *stamp,
                                   expire_id eid)
{
  কাঠামো expire_timers *timeouts = &তথ্য->state.timeouts;
  uint8_t *anchor = &timeouts->first;
  uint8_t id = (uint8_t)eid;

  যদি((unsigned)eid >= EXPIRE_LAST) {
    DEBUGASSERT(0);
    ফেরত CURLM_BAD_FUNCTION_ARGUMENT;
  }
  /* remove from list, store time and re-insert */
  multi_clear_timeout(তথ্য, eid);
  timeouts->offset_us[id] =
    Curl_timeouts_offset_us(&তথ্য->multi->timeouts, stamp);

  যখন(*anchor < EXPIRE_LAST) {
    যদি(timeouts->offset_us[*anchor] > timeouts->offset_us[id])
      বিরতি;
    anchor = &timeouts->পরবর্তী[*anchor];
  }
  timeouts->পরবর্তী[eid] = *anchor;
  timeouts->পরবর্তী[eid] = *anchor;
  *anchor = id;
  DEBUGASSERT(multi_timeouts_check(তথ্য));
  CURL_TRC_TIMER(তথ্য, eid, "set for %" FMT_TIMEDIFF_T "us",
                 curlx_ptimediff_us(stamp, Curl_pgrs_now(তথ্য)));
  ফেরত CURLM_OK;
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
শূন্য Curl_expire_set(কাঠামো Curl_easy *তথ্য,
                     expire_id eid, timediff_t ms,
                     ধ্রুবক কাঠামো curltime *pnow)
{
  কাঠামো Curl_multi *multi = তথ্য->multi;
  কাঠামো expire_timers *timeouts = &তথ্য->state.timeouts;
  uint8_t পূর্ব_id = timeouts->first;
  কাঠামো curltime set;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  যদি(!multi)
    ফেরত;
  DEBUGASSERT(eid < EXPIRE_LAST);
  যদি(ms > INT_MAX)
    /* Cap ridiculous timeouts, 31-bit ms is still 3.5 weeks. When the time
       goes to the user, it must fit in this size. */
    ms = INT_MAX;

  set = *pnow;
  set.tv_sec += (time_t)(ms / 1000); /* may be a 64 to 32-bit conversion */
  set.tv_usec += (পূর্ণসংখ্যা)(ms % 1000) * 1000;
  যদি(set.tv_usec >= 1000000) {
    set.tv_sec++;
    set.tv_usec -= 1000000;
  }

  /* Add the timeout, will replace any previous value for this timer. */
  multi_set_timeout(তথ্য, &set, eid);
  DEBUGASSERT(timeouts->first < EXPIRE_LAST);

  যদি(Curl_timeouts_has(তথ্য)) {
    /* data has already a timeout registered. If the first timer
     * was NOT the one we just set AND is still the first one,
     * nothing changed from the timeouts point of view. The
     * set timer triggers after the one already registered. Leave. */
    যদি((পূর্ব_id != eid) && (পূর্ব_id == timeouts->first))
      ফেরত;

    /* Since this is an updated time, we must remove data from
     * timeouts and then add it again. */
    Curl_timeouts_remove(&multi->timeouts, তথ্য);
  }

  /* Insert the new timer expiry since it is our local minimum. */
  Curl_timeouts_add(&multi->timeouts, তথ্য,
                    timeouts->offset_us[timeouts->first]);
}

শূন্য Curl_expire(কাঠামো Curl_easy *তথ্য,
                 timediff_t milli, expire_id eid)
{
  Curl_expire_set(তথ্য, eid, milli, Curl_pgrs_now(তথ্য));
}

/*
 * Removes the expire timer. Marks it as done.
 */
শূন্য Curl_expire_clear(কাঠামো Curl_easy *তথ্য, expire_id eid)
{
  /* remove the timer, if there */
  multi_clear_timeout(তথ্য, eid);
  CURL_TRC_TIMER(তথ্য, eid, "cleared");
}

/*
 * Clear ALL timeout values for this handle.
 */
শূন্য Curl_expire_clear_all(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_multi *multi = তথ্য->multi;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  যদি(!multi)
    ফেরত;

  যদি(Curl_timeouts_remove(&multi->timeouts, তথ্য)) {
    /* Since this is an cleared time, we must remove the previous entry from
       the splay tree */
    multi_timeouts_init(তথ্য);

    যদি(তথ্য->id >= 0)
      CURL_TRC_M(তথ্য, "[TIMEOUT] all cleared");
  }
}

CURLMcode curl_multi_assign(CURLM *m, curl_socket_t sockfd,
                            শূন্য *sockp)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_assign, &mresult)) {
    mresult = Curl_multi_ev_assign(m, sockfd, sockp);
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

স্থির শূন্য move_pending_to_connect(কাঠামো Curl_multi *multi,
                                    কাঠামো Curl_easy *তথ্য)
{
  DEBUGASSERT(তথ্য->mstate == MSTATE_PENDING);

  /* Remove this node from the pending set, add into process set */
  Curl_uint32_bset_remove(&multi->pending, তথ্য->mid);
  Curl_uint32_bset_add(&multi->process, তথ্য->mid);

  multistate(তথ্য, MSTATE_CONNECT);
  Curl_multi_mark_dirty(তথ্য); /* make it run */
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
স্থির শূন্য multi_schedule_pending(কাঠামো Curl_multi *multi)
{
  uint32_t mid = multi->last_pending_mid;

  যদি(mid) {
    যখন(Curl_uint32_bset_পরবর্তী(&multi->pending, mid, &mid)) {
      কাঠামো Curl_easy *তথ্য = Curl_multi_get_easy(multi, mid);
      যদি(তথ্য) {
        move_pending_to_connect(multi, তথ্য);
        multi->last_pending_mid = mid;
        ফেরত;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    }
    /* found no pending transfers with `mid` larger than `last_pending_mid`.
     * Start at the beginning of the pending set again. */
    multi->last_pending_mid = 0;
  }

  যদি(Curl_uint32_bset_first(&multi->pending, &mid)) {
    করো {
      কাঠামো Curl_easy *তথ্য = Curl_multi_get_easy(multi, mid);
      যদি(তথ্য) {
        move_pending_to_connect(multi, তথ্য);
        multi->last_pending_mid = mid;
        ফেরত;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    } যখন(Curl_uint32_bset_পরবর্তী(&multi->pending, mid, &mid));
  }
}

uint32_t Curl_multi_max_concurrent_streams(কাঠামো Curl_multi *multi)
{
  DEBUGASSERT(multi);
  ফেরত multi->max_concurrent_streams;
}

CURL **curl_multi_get_handles(CURLM *m)
{
  কাঠামো Curl_mapi_guard guard;
  CURL **a = ফাঁকা;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_get_handles, ফাঁকা)) {
    কাঠামো Curl_multi *multi = m;
    শূন্য *entry;
    size_t গুনতি = Curl_uint32_tbl_গুনতি(&multi->xfers);

    a = curlx_malloc(আকার(কাঠামো Curl_easy *) * (গুনতি + 1));
    যদি(a) {
      unsigned পূর্ণসংখ্যা i = 0;
      uint32_t mid;

      যদি(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
        করো {
          কাঠামো Curl_easy *তথ্য = entry;
          DEBUGASSERT(i < গুনতি);
          যদি(!তথ্য->state.internal)
            a[i++] = তথ্য;
        } যখন(Curl_uint32_tbl_পরবর্তী(&multi->xfers, mid, &mid, &entry));
      }
      a[i] = ফাঁকা; /* last entry is a NULL */
    }
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত a;
}

CURLMcode curl_multi_get_offt(CURLM *m,
                              CURLMinfo_offt info,
                              curl_off_t *pvalue)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_get_offt, &mresult)) {
    কাঠামো Curl_multi *multi = m;
    uint32_t n;

    যদি(!pvalue) {
      mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      যাও out;
    }

    নির্বাচন(info) {
    ক্ষেত্র CURLMINFO_XFERS_CURRENT:
      n = Curl_uint32_tbl_গুনতি(&multi->xfers);
      যদি(n && multi->admin)
        --n;
      *pvalue = (curl_off_t)n;
      বিরতি;
    ক্ষেত্র CURLMINFO_XFERS_RUNNING:
      n = Curl_uint32_bset_গুনতি(&multi->process);
      যদি(n && Curl_uint32_bset_contains(&multi->process, multi->admin->mid))
        --n;
      *pvalue = (curl_off_t)n;
      বিরতি;
    ক্ষেত্র CURLMINFO_XFERS_PENDING:
      *pvalue = (curl_off_t)Curl_uint32_bset_গুনতি(&multi->pending);
      বিরতি;
    ক্ষেত্র CURLMINFO_XFERS_DONE:
      *pvalue = (curl_off_t)Curl_uint32_bset_গুনতি(&multi->msgsent);
      বিরতি;
    ক্ষেত্র CURLMINFO_XFERS_ADDED:
      *pvalue = multi->xfers_যোগ_ever;
      বিরতি;
    পূর্বনির্ধারিত:
      *pvalue = -1;
      mresult = CURLM_UNKNOWN_OPTION;
      বিরতি;
    }
  }
out:
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

স্থির কাঠামো Curl_fixed_buf *fixed_buf_create(size_t len)
{
  কাঠামো Curl_fixed_buf *fbuf;
  যদি((SIZE_MAX - আকার(*fbuf)) < len)
    ফেরত ফাঁকা; /* too large */
  fbuf = curlx_malloc(len + আকার(*fbuf));
  যদি(fbuf)
    fbuf->len = len;
  ফেরত fbuf;
}

CURLcode Curl_multi_xfer_buf_borrow(কাঠামো Curl_easy *তথ্য,
                                    অক্ষর **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->multi);
  *pbuf = ফাঁকা;
  *pbuflen = 0;
  যদি(!তথ্য->multi) {
    failf(তথ্য, "transfer has no multi handle");
    ফেরত CURLE_FAILED_INIT;
  }
  যদি(!তথ্য->set.বাফার_আকার) {
    failf(তথ্য, "transfer buffer size is 0");
    ফেরত CURLE_FAILED_INIT;
  }
  যদি(তথ্য->multi->xfer_buf_borrowed) {
    failf(তথ্য, "attempt to borrow xfer_buf when already borrowed");
    ফেরত CURLE_AGAIN;
  }

  যদি(তথ্য->multi->xfer_buf &&
     তথ্য->set.বাফার_আকার > তথ্য->multi->xfer_buf->len) {
    /* not large enough, get a new one */
    curlx_safefree(তথ্য->multi->xfer_buf);
  }

  যদি(!তথ্য->multi->xfer_buf) {
    তথ্য->multi->xfer_buf =
      fixed_buf_create(curlx_uitouz(তথ্য->set.বাফার_আকার));
    যদি(!তথ্য->multi->xfer_buf) {
      failf(তথ্য, "could not allocate xfer_buf of %u bytes",
            তথ্য->set.বাফার_আকার);
      ফেরত CURLE_OUT_OF_MEMORY;
    }
  }

  তথ্য->multi->xfer_buf_borrowed = TRUE;
  *pbuf = তথ্য->multi->xfer_buf->তথ্য;
  *pbuflen = তথ্য->multi->xfer_buf->len;
  ফেরত CURLE_OK;
}

শূন্য Curl_multi_xfer_buf_release(কাঠামো Curl_easy *তথ্য, অক্ষর *buf)
{
  (শূন্য)buf;
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->multi);
  DEBUGASSERT(!buf || (তথ্য->multi->xfer_buf &&
                       তথ্য->multi->xfer_buf->তথ্য == buf));
  তথ্য->multi->xfer_buf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_ulbuf_borrow(কাঠামো Curl_easy *তথ্য,
                                      অক্ষর **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->multi);
  *pbuf = ফাঁকা;
  *pbuflen = 0;
  যদি(!তথ্য->multi) {
    failf(তথ্য, "transfer has no multi handle");
    ফেরত CURLE_FAILED_INIT;
  }
  যদি(তথ্য->multi->xfer_ulbuf_borrowed) {
    failf(তথ্য, "attempt to borrow xfer_ulbuf when already borrowed");
    ফেরত CURLE_AGAIN;
  }

  যদি(তথ্য->multi->xfer_ulbuf &&
     তথ্য->set.upload_বাফার_আকার > তথ্য->multi->xfer_ulbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(তথ্য->multi->xfer_ulbuf);
  }

  যদি(!তথ্য->multi->xfer_ulbuf) {
    তথ্য->multi->xfer_ulbuf =
      fixed_buf_create(curlx_uitouz(তথ্য->set.upload_বাফার_আকার));
    যদি(!তথ্য->multi->xfer_ulbuf) {
      failf(তথ্য, "could not allocate xfer_ulbuf of %u bytes",
            তথ্য->set.upload_বাফার_আকার);
      ফেরত CURLE_OUT_OF_MEMORY;
    }
  }

  তথ্য->multi->xfer_ulbuf_borrowed = TRUE;
  *pbuf = তথ্য->multi->xfer_ulbuf->তথ্য;
  *pbuflen = তথ্য->multi->xfer_ulbuf->len;
  ফেরত CURLE_OK;
}

শূন্য Curl_multi_xfer_ulbuf_release(কাঠামো Curl_easy *তথ্য, অক্ষর *buf)
{
  (শূন্য)buf;
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->multi);
  DEBUGASSERT(!buf || (তথ্য->multi->xfer_ulbuf &&
                       তথ্য->multi->xfer_ulbuf->তথ্য == buf));
  তথ্য->multi->xfer_ulbuf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_sockbuf_borrow(কাঠামো Curl_easy *তথ্য,
                                        size_t blen, অক্ষর **pbuf)
{
  DEBUGASSERT(তথ্য);
  *pbuf = ফাঁকা;
  যদি(!তথ্য->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    *pbuf = curlx_malloc(blen);
    ফেরত *pbuf ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  যদি(তথ্য->multi->xfer_sockbuf_borrowed) {
    failf(তথ্য, "attempt to borrow xfer_sockbuf when already borrowed");
    ফেরত CURLE_AGAIN;
  }

  যদি(তথ্য->multi->xfer_sockbuf && blen > তথ্য->multi->xfer_sockbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(তথ্য->multi->xfer_sockbuf);
  }

  যদি(!তথ্য->multi->xfer_sockbuf) {
    তথ্য->multi->xfer_sockbuf = fixed_buf_create(blen);
    যদি(!তথ্য->multi->xfer_sockbuf) {
      failf(তথ্য, "could not allocate xfer_sockbuf of %zu bytes", blen);
      ফেরত CURLE_OUT_OF_MEMORY;
    }
  }

  তথ্য->multi->xfer_sockbuf_borrowed = TRUE;
  *pbuf = তথ্য->multi->xfer_sockbuf->তথ্য;
  ফেরত CURLE_OK;
}

শূন্য Curl_multi_xfer_sockbuf_release(কাঠামো Curl_easy *তথ্য, অক্ষর *buf)
{
  DEBUGASSERT(তথ্য);
  যদি(!তথ্য->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    curlx_free(buf);
  }
  অন্যথা {
    DEBUGASSERT(!buf || (তথ্য->multi->xfer_sockbuf &&
                         তথ্য->multi->xfer_sockbuf->তথ্য == buf));
    তথ্য->multi->xfer_sockbuf_borrowed = FALSE;
  }
}

স্থির শূন্য multi_xfer_bufs_free(কাঠামো Curl_multi *multi)
{
  DEBUGASSERT(multi);
  curlx_safefree(multi->xfer_buf);
  multi->xfer_buf_borrowed = FALSE;
  curlx_safefree(multi->xfer_ulbuf);
  multi->xfer_ulbuf_borrowed = FALSE;
  curlx_safefree(multi->xfer_sockbuf);
  multi->xfer_sockbuf_borrowed = FALSE;
}

কাঠামো Curl_easy *Curl_multi_get_easy(কাঠামো Curl_multi *multi,
                                      uint32_t mid)
{
  কাঠামো Curl_easy *তথ্য = Curl_uint32_tbl_get(&multi->xfers, mid);
  যদি(GOOD_EASY_HANDLE(তথ্য))
    ফেরত তথ্য;
  CURL_TRC_M(multi->admin, "invalid easy handle in xfer table for mid=%u",
             mid);
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  ফেরত ফাঁকা;
}

bool Curl_multi_knows_easy(কাঠামো Curl_multi *multi, কাঠামো Curl_easy *তথ্য)
{
  ফেরত Curl_uint32_tbl_get(&multi->xfers, তথ্য->mid) == তথ্য;
}

স্থির uint32_t multi_xfers_running(কাঠামো Curl_multi *multi)
{
  যদি(!multi) {
    DEBUGASSERT(0);
    ফেরত 0;
  }
  ফেরত multi->xfers_alive;
}

uint32_t Curl_multi_xfers_attached(কাঠামো Curl_multi *multi)
{
  যদি(!multi || !multi->admin) {
    DEBUGASSERT(0);
    ফেরত 0;
  }
  /* Discount the admin handle */
  ফেরত Curl_uint32_tbl_গুনতি(&multi->xfers) - 1;
}

শূন্য Curl_multi_mark_dirty(কাঠামো Curl_easy *তথ্য)
{
  যদি(তথ্য->multi && তথ্য->mid != UINT32_MAX)
    Curl_uint32_bset_add(&তথ্য->multi->dirty, তথ্য->mid);
}

শূন্য Curl_multi_clear_dirty(কাঠামো Curl_easy *তথ্য)
{
  যদি(তথ্য->multi && তথ্য->mid != UINT32_MAX)
    Curl_uint32_bset_remove(&তথ্য->multi->dirty, তথ্য->mid);
}

CURLMcode curl_multi_notify_enable(CURLM *m, unsigned পূর্ণসংখ্যা notification)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_notify_enable, &mresult)) {
    mresult = Curl_mntfy_enable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

CURLMcode curl_multi_notify_disable(CURLM *m, unsigned পূর্ণসংখ্যা notification)
{
  কাঠামো Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_notify_disable, &mresult)) {
    mresult = Curl_mntfy_disable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  ফেরত mresult;
}

#ifdef DEBUGBUILD
স্থির শূন্য multi_xfer_dump(কাঠামো Curl_multi *multi, uint32_t mid,
                            শূন্য *entry)
{
  কাঠামো Curl_easy *তথ্য = entry;

  (শূন্য)multi;
  যদি(!তথ্য) {
    curl_mfprintf(stderr, "mid=%u, entry=NULL, bug in xfer table?\n", mid);
  }
  অন্যথা {
    curl_mfprintf(stderr, "mid=%u, magic=%s, p=%p, id=%" FMT_OFF_T
                  ", url=%s\n",
                  mid,
                  (তথ্য->magic == CURLEASY_MAGIC_সংখ্যা) ? "GOOD" : "BAD!",
                  (শূন্য *)তথ্য, তথ্য->id, Curl_bufref_ptr(&তথ্য->state.url));
  }
}

স্থির শূন্য multi_xfer_tbl_dump(কাঠামো Curl_multi *multi)
{
  uint32_t mid;
  শূন্য *entry;
  curl_mfprintf(stderr, "=== multi xfer table (count=%u, capacity=%u\n",
                Curl_uint32_tbl_গুনতি(&multi->xfers),
                Curl_uint32_tbl_capacity(&multi->xfers));
  যদি(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
    multi_xfer_dump(multi, mid, entry);
    যখন(Curl_uint32_tbl_পরবর্তী(&multi->xfers, mid, &mid, &entry))
      multi_xfer_dump(multi, mid, entry);
  }
  curl_mfprintf(stderr, "===\n");
  fflush(stderr);
}
#endif /* DEBUGBUILD */
