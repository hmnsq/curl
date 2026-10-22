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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "curl_share.h"
#অন্তর্ভাৱ "psl.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "multi_ev.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "http.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "curlx/wait.h"
#অন্তর্ভাৱ "conncache.h"
#অন্তর্ভাৱ "multihandle.h"
#অন্তর্ভাৱ "sigpipe.h"
#অন্তর্ভাৱ "vtls/vtls.h"
#অন্তর্ভাৱ "vtls/vtls_scache.h"
#অন্তর্ভাৱ "http_proxy.h"
#অন্তর্ভাৱ "http2.h"
#অন্তর্ভাৱ "socketpair.h"
#অন্তর্ভাৱ "bufref.h"
#অন্তর্ভাৱ "curlx/strparse.h"

/* initial multi->xfers table size for a full multi */
#define CURL_XFER_TABLE_SIZE 128

/* CURL_SOCKET_HASH_TABLE_SIZE should be a prime number. Increasing it from 97
   to 911 takes on a 32-bit machine 4 x 804 = 3211 more bytes. Still, every
   curl handle takes 6K memory, therefore this 3K are not significant. */
#ifndef CURL_SOCKET_HASH_TABLE_SIZE
#define CURL_SOCKET_HASH_TABLE_SIZE 911
#endif

#ifndef CURL_CONNECTION_HASH_SIZE
#define CURL_CONNECTION_HASH_SIZE 97
#endif

#ifndef CURL_DNS_HASH_SIZE
#define CURL_DNS_HASH_SIZE 71
#endif

#ifndef CURL_TLS_SESSION_SIZE
#define CURL_TLS_SESSION_SIZE 25
#endif

স্থির শূন্য move_pending_to_connect(সংরচনা Curl_multi *multi,
                                    সংরচনা Curl_easy *data);
স্থির CURLMcode add_next_timeout(ধ্রুৱ সংরচনা curltime *pnow,
                                  সংরচনা Curl_multi *multi,
                                  সংরচনা Curl_easy *data);
স্থির শূন্য multi_timeout(সংরচনা Curl_multi *multi,
                          timediff_t *pexire_offset_us,
                          পূর্ণাঙ্ক *timeout_ms);
স্থির শূন্য multi_schedule_pending(সংরচনা Curl_multi *multi);
স্থির শূন্য multi_xfer_bufs_free(সংরচনা Curl_multi *multi);
#ifdef DEBUGBUILD
স্থির শূন্য multi_xfer_tbl_dump(সংরচনা Curl_multi *multi);
#endif
/* Get the # of transfers current in process/pending. */
স্থির uint32_t multi_xfers_running(সংরচনা Curl_multi *multi);

/* function pointer called once when entering a state */
প্রকারনির্দেশ শূন্য (*mstate_enter_func)(সংরচনা Curl_easy *data,
                                  CURLMstate from_state);

স্থির শূন্য mstate_enter_connect(সংরচনা Curl_easy *data,
                                 CURLMstate from_state)
{
  (শূন্য)from_state;
  Curl_init_CONNECT(data);
}

স্থির শূন্য mstate_enter_did(সংরচনা Curl_easy *data,
                             CURLMstate from_state)
{
  (শূন্য)from_state;
  data->req.chunk = FALSE;
  Curl_pgrsTime(data, TIMER_PRETRANSFER);
  যদি(!CURL_REQ_WANT_SEND(data))
    Curl_pgrsTime(data, TIMER_POSTRANSFER);
}

স্থির শূন্য mstate_enter_done(সংরচনা Curl_easy *data,
                              CURLMstate from_state)
{
  (শূন্য)from_state;
  CURLM_NTFY(data, CURLMNOTIFY_EASY_DONE);
}

স্থির শূন্য mstate_enter_completed(সংরচনা Curl_easy *data,
                                   CURLMstate from_state)
{
  /* we sometimes directly jump to COMPLETED, trigger things
   * we then missed. */
  যদি(from_state < MSTATE_DID) {
    Curl_pgrsTime(data, TIMER_PRETRANSFER);
    Curl_pgrsTime(data, TIMER_POSTRANSFER);
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
  }
  /* multi_done() should have run and detached any connection already */
  DEBUGASSERT(!data->conn);
  Curl_pgrsCompleted(data);
  যদি(from_state < MSTATE_DONE)
    CURLM_NTFY(data, CURLMNOTIFY_EASY_DONE);
  /* changing to COMPLETED means it is in process and needs to go */
  DEBUGASSERT(Curl_uint32_bset_contains(&data->multi->process, data->mid));
  Curl_uint32_bset_remove(&data->multi->process, data->mid);
  Curl_uint32_bset_remove(&data->multi->pending, data->mid); /* to be sure */

  যদি(Curl_uint32_bset_empty(&data->multi->process)) {
    /* free the transfer buffer when we have no more active transfers */
    multi_xfer_bufs_free(data->multi);
  }
  /* Important: reset the conn pointer so that we do not point to memory
     that could be freed anytime */
  Curl_detach_connection(data);
  Curl_expire_clear_all(data); /* stop all timers */
}

/* always use this function to change state, to make debugging easier */
স্থির শূন্য mstate(সংরচনা Curl_easy *data, CURLMstate state
#ifdef DEBUGBUILD
                   , পূর্ণাঙ্ক lineno
#endif
)
{
  CURLMstate oldstate = data->mstate;
  স্থির ধ্রুৱ mstate_enter_func state_enter[MSTATE_LAST] = {
    রিক্ত,                      /* INIT */
    রিক্ত,                      /* PENDING */
    রিক্ত,                      /* SETUP */
    mstate_enter_connect,      /* CONNECT */
    রিক্ত,                      /* CONNECTING */
    রিক্ত,                      /* PROTOCONNECT */
    রিক্ত,                      /* PROTOCONNECTING */
    রিক্ত,                      /* DO */
    রিক্ত,                      /* DOING */
    রিক্ত,                      /* DOING_MORE */
    mstate_enter_did,          /* DID */
    রিক্ত,                      /* PERFORMING */
    রিক্ত,                      /* RATELIMITING */
    mstate_enter_done,         /* DONE */
    mstate_enter_completed,    /* COMPLETED */
    রিক্ত                       /* MSGSENT */
  };

  যদি(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    ঘূৰাই;

#ifdef DEBUGBUILD
  NOVERBOSE((শূন্য)lineno);
  CURL_TRC_M(data, "-> [%s] (line %d)", CURL_MSTATE_NAME(state), lineno);
#অন্যথা
  CURL_TRC_M(data, "-> [%s]", CURL_MSTATE_NAME(state));
#endif

  /* really switching state */
  data->mstate = state;
  যদি(state_enter[state])
    state_enter[state](data, oldstate);
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
স্থির শূন্য multi_addmsg(সংরচনা Curl_multi *multi, সংরচনা Curl_easy *data)
{
  যদি(Curl_uint32_bset_empty(&multi->msgsent))
    CURLM_NTFY(multi->admin, CURLMNOTIFY_INFO_READ);
  Curl_uint32_bset_add(&multi->msgsent, data->mid);
}

স্থির শূন্য multi_timeouts_init(সংরচনা Curl_easy *data);

সংরচনা Curl_multi *Curl_multi_handle(uint32_t xfer_table_size,
                                     size_t ev_hashsize,  /* event hash */
                                     size_t chashsize, /* connection hash */
                                     size_t dnssize,   /* dns hash */
                                     size_t sesssize)  /* TLS session cache */
{
  সংরচনা curltime created;
  সংরচনা Curl_multi *multi = curlx_calloc(1, পরিমাণ(সংরচনা Curl_multi));

  যদি(!multi)
    ঘূৰাই রিক্ত;

  multi->magic = CURLMULTI_MAGIC_সংখ্যা;

  /* Initialisation order is important here!
   * easy_init() does a lazy check on curl_global_init() which sets
   * up platform specific things we need. For example calling curlx_pnow()
   * before this is not safe. */
  multi->admin = curl_easy_init();
  যদি(!multi->admin) {
    curlx_free(multi);
    ঘূৰাই রিক্ত;
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

  যদি(Curl_uint32_bset_resize(&multi->process, xfer_table_size) ||
     Curl_uint32_bset_resize(&multi->pending, xfer_table_size) ||
     Curl_uint32_bset_resize(&multi->dirty, xfer_table_size) ||
     Curl_uint32_bset_resize(&multi->msgsent, xfer_table_size) ||
     Curl_uint32_tbl_resize(&multi->xfers, xfer_table_size))
    গচ্ছ error;

#ifdef DEBUGBUILD
  {
    ধ্রুৱ ৱর্ণ *p;
    যদি(getenv("CURL_DEBUG"))
      multi->admin->set.verbose = TRUE;
    p = getenv("CURL_DBG_MAX_TOTAL_CONNECTIONS");
    যদি(p) {
      curl_off_t l;
      যদি(!curlx_str_সংখ্যা(&p, &l, UINT32_MAX)) {
        multi->max_total_connections = (uint32_t)l;
      }
    }
  }
#endif
  Curl_uint32_tbl_add(&multi->xfers, multi->admin, &multi->admin->mid);
  Curl_uint32_bset_add(&multi->process, multi->admin->mid);

  Curl_cpool_init(&multi->cpool, রিক্ত, chashsize);

#ifdef USE_SSL
  যদি(Curl_ssl_scache_create(sesssize, 2, &multi->ssl_scache))
    গচ্ছ error;
#অন্যথা
  (শূন্য)sesssize;
#endif

#ifdef USE_WINSOCK
  multi->wsa_event = WSACreateEvent();
  যদি(multi->wsa_event == WSA_INVALID_EVENT)
    গচ্ছ error;
#endif
#ifdef ENABLE_WAKEUP
  /* When enabled, rely on this to work. We ignore this in previous
   * versions, but that seems an unnecessary complication. */
  যদি(Curl_wakeup_init(multi->wakeup_pair, TRUE) < 0)
    গচ্ছ error;
#endif
#ifdef ENABLE_INTERNAL_WAKEUP
  যদি(Curl_wakeup_init(multi->wakeup_internal, TRUE) < 0)
    গচ্ছ error;
#endif

  যদি(Curl_probeipv6(multi))
    গচ্ছ error;

#ifdef USE_RESOLV_THREADED
  যদি(xfer_table_size < CURL_XFER_TABLE_SIZE) { /* easy multi */
    যদি(Curl_async_thrdd_multi_init(multi, 0, 2, 10))
      গচ্ছ error;
  }
  অন্যথা { /* real multi handle */
    যদি(Curl_async_thrdd_multi_init(multi, 0, 20, 2000))
      গচ্ছ error;
  }
#endif

  ঘূৰাই multi;

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
    multi->admin->multi = রিক্ত;
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
  ঘূৰাই রিক্ত;
}

CURLM *curl_multi_init(শূন্য)
{
  ঘূৰাই Curl_multi_handle(CURL_XFER_TABLE_SIZE,
                           CURL_SOCKET_HASH_TABLE_SIZE,
                           CURL_CONNECTION_HASH_SIZE,
                           CURL_DNS_HASH_SIZE,
                           CURL_TLS_SESSION_SIZE);
}

#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
স্থির শূন্য multi_warn_debug(সংরচনা Curl_multi *multi, সংরচনা Curl_easy *data)
{
  যদি(!multi->warned) {
    infof(data, "!!! WARNING !!!");
    infof(data, "This is a debug build of libcurl, "
                "do not use in production.");
    multi->warned = TRUE;
  }
}
#অন্যথা
#define multi_warn_debug(x, y) Curl_nop_stmt
#endif

bool Curl_is_connecting(সংরচনা Curl_easy *data)
{
  ঘূৰাই data->mstate < MSTATE_DO;
}

স্থির CURLMcode multi_assess_wakeup(সংরচনা Curl_multi *multi)
{
#ifdef ENABLE_INTERNAL_WAKEUP
  যদি(multi->socket_cb)
    ঘূৰাই Curl_multi_ev_assess_xfer(multi, multi->admin);
#অন্যথা
  (শূন্য)multi;
#endif
  ঘূৰাই CURLM_OK;
}

স্থির CURLMcode multi_xfers_add(সংরচনা Curl_multi *multi,
                                 সংরচনা Curl_easy *data)
{
  uint32_t capacity = Curl_uint32_tbl_capacity(&multi->xfers);
  uint32_t new_size = 0;
  /* Prepare to make this into a CURLMOPT_MAX_TRANSFERS, because some
   * applications may want to prevent a run-away of their memory use. */
  /* UINT_MAX is our "invalid" id, do not let the table grow up to that. */
  ধ্রুৱ uint32_t max_capacity = UINT_MAX - 1;

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
        new_size = max_capacity;
      অন্যথা
        new_size = capacity + growth;
    }
  }

  যদি(new_size > capacity) {
    /* Grow the bitsets first. Should one fail, we do not need
     * to downsize the already resized ones. The sets continue
     * to work properly when larger than the table, but not
     * the other way around. */
    CURL_TRC_M(data, "increasing xfer table size to %u", new_size);
    যদি(Curl_uint32_bset_resize(&multi->process, new_size) ||
       Curl_uint32_bset_resize(&multi->dirty, new_size) ||
       Curl_uint32_bset_resize(&multi->pending, new_size) ||
       Curl_uint32_bset_resize(&multi->msgsent, new_size) ||
       Curl_uint32_tbl_resize(&multi->xfers, new_size))
      ঘূৰাই CURLM_OUT_OF_MEMORY;
  }

  /* Insert the easy into the table now */
  যদি(!Curl_uint32_tbl_add(&multi->xfers, data, &data->mid)) {
    /* MUST only happen when table is full */
    DEBUGASSERT(Curl_uint32_tbl_capacity(&multi->xfers) <=
                Curl_uint32_tbl_গুনতি(&multi->xfers));
    ঘূৰাই CURLM_OUT_OF_MEMORY;
  }
  ঘূৰাই CURLM_OK;
}

CURLMcode Curl_multi_add_handle(সংরচনা Curl_multi *multi,
                                সংরচনা Curl_easy *data)
{
  CURLMcode mresult;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  যদি(data->multi)
    ঘূৰাই CURLM_ADDED_ALREADY;

  যদি(multi->dead) {
    /* a "dead" handle cannot get added transfers while any existing easy
       handles are still alive - but if there are none alive anymore, it is
       fine to start over and unmark the "deadness" of this handle.
       This means only the admin handle MUST be present. */
    যদি((Curl_uint32_tbl_গুনতি(&multi->xfers) != 1) ||
       !Curl_uint32_tbl_contains(&multi->xfers, 0))
      ঘূৰাই CURLM_ABORTED_BY_CALLBACK;
    multi->dead = FALSE;
    Curl_uint32_bset_clear(&multi->process);
    Curl_uint32_bset_clear(&multi->dirty);
    Curl_uint32_bset_clear(&multi->pending);
    Curl_uint32_bset_clear(&multi->msgsent);
  }

  যদি(data->multi_easy) {
    /* if this easy handle was previously used for curl_easy_perform(), there
       is a private multi handle here that we can kill */
    curl_multi_cleanup(data->multi_easy);
    data->multi_easy = রিক্ত;
  }

  /* Insert the easy into the multi->xfers table, assigning it a `mid`. */
  যদি(multi_xfers_add(multi, data))
    ঘূৰাই CURLM_OUT_OF_MEMORY;

  /* Initialize timeouts for this handle */
  multi_timeouts_init(data);

  /*
   * No failure allowed in this function beyond this point. No modification
   * of easy nor multi handle allowed before this except for potential
   * multi's connection pool growing which will not be undone in this
   * function no matter what.
   */
  যদি(data->set.errorbuffer)
    data->set.errorbuffer[0] = 0;

  data->state.os_errno = 0;

  /* make the Curl_easy refer back to this multi handle - before
     Curl_expire() is called. */
  data->multi = multi;

  /* set the easy handle */
  multistate(data, MSTATE_INIT);
  /* not yet passed INIT state */
  data->state.really_alive = FALSE;

#ifdef USE_LIBPSL
  /* Do the same for PSL. */
  যদি(data->share && (data->share->specifier & (1 << CURL_LOCK_DATA_PSL)))
    data->psl = &data->share->psl;
  অন্যথা
    data->psl = &multi->psl;
#endif

  /* add the easy handle to the process set */
  Curl_uint32_bset_add(&multi->process, data->mid);
  ++multi->xfers_alive;
  ++multi->xfers_total_ever;

  Curl_cpool_xfer_init(data);
  multi_warn_debug(multi, data);

  /* Make sure the new handle will run */
  Curl_multi_mark_dirty(data);

  /* Necessary in event based processing, where dirty handles trigger
   * a timeout callback invocation. */
  mresult = Curl_update_timer(multi);
  যদি(mresult) {
    /* undo the additions above */
    Curl_uint32_bset_remove(&multi->process, data->mid);
    Curl_uint32_bset_remove(&multi->dirty, data->mid);
    --multi->xfers_alive;
    data->multi = রিক্ত; /* not anymore */
    Curl_uint32_tbl_remove(&multi->xfers, data->mid);
    data->mid = UINT32_MAX;
    ঘূৰাই mresult;
  }

  /* The admin handle only ever has default timeouts set. To improve the
     state somewhat we clone the timeouts from each added handle so that the
     admin handle always has the same timeouts as the most recently added
     easy handle. */
  multi->admin->set.timeout = data->set.timeout;
  multi->admin->set.server_response_timeout =
    data->set.server_response_timeout;
  multi->admin->set.no_signal = data->set.no_signal;

  CURL_TRC_M(data, "added to multi, mid=%u, running=%u, total=%u",
             data->mid, multi_xfers_running(multi),
             Curl_uint32_tbl_গুনতি(&multi->xfers));
  ঘূৰাই CURLM_OK;
}

CURLMcode curl_multi_add_handle(CURLM *m, CURL *curl)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_add_handle, &mresult)) {
    সংরচনা Curl_easy *data = curl;
    /* Verify that we got a somewhat good easy handle too */
    যদি(!GOOD_EASY_HANDLE(data) || data->state.internal)
      mresult = CURLM_BAD_EASY_HANDLE;
    অন্যথা
      mresult = Curl_multi_add_handle(m, data);
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

সংরচনা multi_done_ctx {
  BIT(premature);
};

স্থির bool multi_conn_should_close(সংরচনা connectdata *conn,
                                    সংরচনা Curl_easy *data,
                                    bool premature)
{
  /* if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of everything else. */
  যদি(conn->bits.close)
    ঘূৰাই TRUE;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM/NEGOTIATE authentication handshake. */
  যদি(data->set.reuse_forbid
#ifdef USE_NTLM
     && !(conn->http_ntlm_state == NTLMSTATE_TYPE2 ||
          conn->proxy_ntlm_state == NTLMSTATE_TYPE2)
#endif
#ifdef USE_SPNEGO
     && !(conn->http_negotiate_state == CURL_NEGO_AUTH_RECV ||
          conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)
#endif
    )
    ঘূৰাই TRUE;

  /* Unless this connection is for a "connect-only" transfer, it
   * needs to be closed if the protocol handler does not support reuse. */
  যদি(!data->set.connect_only && conn->scheme &&
     !(conn->scheme->flags & PROTOPT_CONN_REUSE))
    ঘূৰাই TRUE;

  /* if premature is TRUE, it means this connection was said to be DONE before
     the entire request operation is complete and thus we cannot know in what
     state it is for reusing, so we are forced to close it. In a perfect world
     we can add code that keep track of if we really must close it here or not,
     but currently we have no such detail knowledge. */
  যদি(premature && !Curl_conn_is_multiplex(conn, FIRSTSOCKET))
    ঘূৰাই TRUE;

  ঘূৰাই FALSE;
}

স্থির cpool_do_result multi_done_locked(সংরচনা Curl_easy *data,
                                         সংরচনা connectdata *conn,
                                         শূন্য *userdata,
                                         ধ্রুৱ সংরচনা curltime *pnow)
{
  সংরচনা multi_done_ctx *mdctx = userdata;

  (শূন্য)pnow;
  CURL_TRC_M(data, "multi_done_locked, in use=%u", conn->attached_xfers);
  যদি(CONN_INUSE(conn)) {
    /* Stop if still used. */
    CURL_TRC_M(data, "Connection still in use %u, no more multi_done now!",
               conn->attached_xfers);
    ঘূৰাই CPOOL_DO_KEEP;
  }

  যদি(multi_conn_should_close(conn, data, (bool)mdctx->premature)) {
    CURL_TRC_M(data, "multi_done, terminating conn #%" FMT_OFF_T " to %s:%u, "
               "forbid=%d, close=%d, premature=%d, conn_multiplex=%d",
               conn->connection_id, conn->origin->user_hostname,
               conn->origin->port,
               data->set.reuse_forbid, conn->bits.close, mdctx->premature,
               Curl_conn_is_multiplex(conn, FIRSTSOCKET));
    connclose(conn);
    ঘূৰাই mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  অন্যথা যদি(!Curl_conn_get_max_concurrent(data, conn, FIRSTSOCKET)) {
    CURL_TRC_M(data, "multi_done, conn #%" FMT_OFF_T " to %s:%u was shutdown"
               " by server, not reusing", conn->connection_id,
               conn->origin->user_hostname, conn->origin->port);
    connclose(conn);
    ঘূৰাই mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  অন্যথা {
    ঘূৰাই CPOOL_DO_IDLE;
  }
}

স্থির CURLcode multi_done(সংরচনা Curl_easy *data,
                           CURLcode status,  /* an error if this is called
                                                after an error was detected */
                           bool premature)
{
  CURLcode result;
  সংরচনা connectdata *conn = data->conn;
  ধ্রুৱ সংরচনা curltime *pnow = Curl_pgrs_now(data);

  CURL_TRC_M(data, "multi_done: status: %d prem: %d done: %d",
             (পূর্ণাঙ্ক)status, (পূর্ণাঙ্ক)premature, data->state.done);

  যদি(data->state.done) {
    /* Stop if multi_done() has already been called */
    ঘূৰাই CURLE_OK;
  }
  data->state.done = TRUE; /* called now! */

  /* Shut down any ongoing async resolver operation. */
  Curl_resolv_shutdown_all(data);

  /* Cleanup possible redirect junk */
  curlx_safefree(data->req.newurl);
  curlx_safefree(data->req.location);

  ৱিকল্প(status) {
  পক্ষ CURLE_ABORTED_BY_CALLBACK:
  পক্ষ CURLE_READ_ERROR:
  পক্ষ CURLE_WRITE_ERROR:
    /* When we are aborted due to a callback return code it has to be counted
       as premature as there is trouble ahead if we do not. We have many
       callbacks and protocols work differently, we could potentially do this
       more fine-grained in the future. */
    premature = TRUE;
    FALLTHROUGH();
  স্ৱভাৱ:
    ভঙ্গ;
  }

  /* this calls the protocol-specific function pointer previously set */
  যদি(conn && conn->scheme->run->done && (data->mstate >= MSTATE_PROTOCONNECT))
    result = conn->scheme->run->done(data, status, premature);
  অন্যথা
    result = status;

  যদি(data->mstate > MSTATE_CONNECTING &&
     (result != CURLE_ABORTED_BY_CALLBACK)) {
    /* avoid this if
     * - the transfer has not connected
     * - we already aborted by callback to avoid this calling another callback
     */
    পূর্ণাঙ্ক rc = Curl_pgrsDone(data, pnow);
    যদি(!result && rc)
      result = CURLE_ABORTED_BY_CALLBACK;
  }

  /* Make sure that transfer client writes are really done now. */
  result = Curl_1st_fatal(result, Curl_xfer_write_done(data, premature));

  যদি(!result)
    result = Curl_req_done(&data->req, data, premature);

  যদি(conn) {
    /* Detach connection and decide what to do with it. */
    সংরচনা multi_done_ctx mdctx;

    /* Inform connection filters that this transfer is done and they
     * need to give up any references to `data`. */
    Curl_conn_ev_data_done(data, premature);
    Curl_detach_connection(data);
    Curl_dnscache_prune(data, pnow);

    memset(&mdctx, 0, পরিমাণ(mdctx));
    mdctx.premature = premature;
    Curl_cpool_return(data, conn, multi_done_locked, &mdctx, pnow);

    যদি(data->multi)
      multi_schedule_pending(data->multi);
  }

  /* flush the netrc cache */
  Curl_netrc_cleanup(&data->state.netrc);
  ঘূৰাই result;
}

CURLMcode Curl_multi_remove_handle(সংরচনা Curl_multi *multi,
                                   সংরচনা Curl_easy *data)
{
  CURLMcode mresult;
  bool premature;
  uint32_t mid;

  /* Prevent users from trying to remove same easy handle more than once */
  যদি(!data->multi)
    ঘূৰাই CURLM_OK; /* it is already removed so let's say it is fine! */

  /* Prevent users from trying to remove an easy handle from the wrong multi */
  যদি(data->multi != multi)
    ঘূৰাই CURLM_BAD_EASY_HANDLE;

  যদি(data->mid == UINT32_MAX) {
    DEBUGASSERT(0);
    ঘূৰাই CURLM_INTERNAL_ERROR;
  }
  যদি(Curl_uint32_tbl_get(&multi->xfers, data->mid) != data) {
    DEBUGASSERT(0);
    ঘূৰাই CURLM_INTERNAL_ERROR;
  }

  premature = (data->mstate < MSTATE_COMPLETED);

  যদি(data->conn) {
    /* If the 'state' is not INIT or COMPLETED, we might need to do something
       nice to put the easy_handle in a good known state when this returns. */
    যদি(premature && (data->mstate > MSTATE_DO))
      streamclose(data->conn);

    /* multi_done() clears the association between the easy handle and the
       connection.
       Note that this ignores the return code because there is
       nothing really useful to do with it anyway! */
    (শূন্য)multi_done(data, data->result, premature);
  }

  /* The timer must be shut down before data->multi is set to NULL, else
     data's splaynode would remain in the splay tree after curl_easy_cleanup is
     called. Do it after multi_done() in case that sets another time! */
  Curl_expire_clear_all(data);

  /* In MSGSENT, it was deducted from `multi->xfers_alive` already. */
  যদি(data->mstate != MSTATE_MSGSENT)
    --multi->xfers_alive;

  যদি(data->state.really_alive) {
    data->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    যদি(!multi->xfers_really_alive)
      (শূন্য)multi_assess_wakeup(multi);
  }

  Curl_wildcard_dtor(&data->wildcard);

  data->mstate = MSTATE_COMPLETED;

  /* Remove the association between the connection and the handle */
  Curl_detach_connection(data);

  /* Tell event handling that this transfer is definitely going away */
  Curl_multi_ev_xfer_done(multi, data);

  যদি(data->set.connect_only) {
    যদি(data->multi_easy) {
      যদি(data->state.last_conn_id != -1) {
        /* Mark any connect-only connection for closure */
        সংরচনা connectdata *conn;
        (শূন্য)Curl_getconnectinfo(data, &conn);
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
      সংরচনা connectdata *conn;
      (শূন্য)Curl_getconnectinfo(data, &conn);
      যদি(conn)
        Curl_conn_close(data, conn, TRUE);
    }
  }

#ifdef USE_LIBPSL
  /* Remove the PSL association. */
  যদি(data->psl == &multi->psl)
    data->psl = রিক্ত;
#endif

  /* clear the association to this multi handle */
  mid = data->mid;
  DEBUGASSERT(Curl_uint32_tbl_contains(&multi->xfers, mid));
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  Curl_uint32_bset_remove(&multi->process, mid);
  Curl_uint32_bset_remove(&multi->dirty, mid);
  Curl_uint32_bset_remove(&multi->pending, mid);
  Curl_uint32_bset_remove(&multi->msgsent, mid);
  data->multi = রিক্ত;
  data->mid = UINT32_MAX;
  data->master_mid = UINT32_MAX;

  /* A pending transfer *might* be able to run now. */
  multi_schedule_pending(multi);
  mresult = Curl_update_timer(multi);
  যদি(mresult)
    ঘূৰাই mresult;

  mresult = multi_assess_wakeup(multi);
  যদি(mresult) {
    failf(data, "error enabling wakeup listening: %d", mresult);
    ঘূৰাই mresult;
  }

  CURL_TRC_M(data, "removed from multi, mid=%u, running=%u, total=%u",
             mid, multi_xfers_running(multi),
             Curl_uint32_tbl_গুনতি(&multi->xfers));
  ঘূৰাই CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *curl)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_remove_handle, &mresult)) {
    সংরচনা Curl_easy *data = curl;
    যদি(!GOOD_EASY_HANDLE(data) || data->state.internal)
      mresult = CURLM_BAD_EASY_HANDLE;
    অন্যথা
      mresult = Curl_multi_remove_handle(m, data);
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

/* Return TRUE if the application asked for multiplexing */
bool Curl_multiplex_wanted(ধ্রুৱ সংরচনা Curl_multi *multi)
{
  ঘূৰাই multi && multi->multiplexing;
}

/*
 * Curl_detach_connection() removes the given transfer from the connection.
 *
 * This is the only function that should clear data->conn. This will
 * occasionally be called with the data->conn pointer already cleared.
 */
শূন্য Curl_detach_connection(সংরচনা Curl_easy *data)
{
  সংরচনা connectdata *conn = data->conn;
  যদি(conn) {
    /* this should never happen, prevent underflow */
    DEBUGASSERT(conn->attached_xfers);
    যদি(conn->attached_xfers) {
      conn->attached_xfers--;
      যদি(!conn->attached_xfers)
        conn->attached_multi = রিক্ত;
    }
  }
  data->conn = রিক্ত;
}

/*
 * Curl_attach_connection() attaches this transfer to this connection.
 *
 * This is the only function that should assign data->conn.
 * `matched == TRUE` means the transfer's properties match this
 * connection and it is not a temporary attach for maintenance.
 */
শূন্য Curl_attach_connection(সংরচনা Curl_easy *data,
                            সংরচনা connectdata *conn,
                            bool matched)
{
  DEBUGASSERT(data);
  DEBUGASSERT(!data->conn);
  DEBUGASSERT(conn);
  DEBUGASSERT(conn->attached_xfers < UINT32_MAX);
  data->conn = conn;
  যদি(matched) {
    data->state.last_conn_id = conn->connection_id;
    data->state.last_cpid = conn->cpid;
  }
  অন্যথা
    DEBUGASSERT(!data->mid); /* admin handle */
  conn->attached_xfers++;
  /* all attached transfers must be from the same multi */
  যদি(!conn->attached_multi)
    conn->attached_multi = data->multi;
  DEBUGASSERT(conn->attached_multi == data->multi);

  যদি(conn->scheme && conn->scheme->run->attach)
    conn->scheme->run->attach(data, conn);
}

/* adjust pollset for rate limits/pauses */
স্থির CURLcode multi_adjust_pollset(সংরচনা Curl_easy *data,
                                     সংরচনা easy_pollset *ps)
{
  CURLcode result = CURLE_OK;

  যদি(ps->n) {
    bool send_blocked, recv_blocked;

    recv_blocked = (Curl_rlimit_avail(&data->progress.dl.rlimit, রিক্ত) <= 0);
    send_blocked = (Curl_rlimit_avail(&data->progress.ul.rlimit, রিক্ত) <= 0);
    যদি(send_blocked || recv_blocked) {
      পূর্ণাঙ্ক i;
      বাবে(i = 0; i <= SECONDARYSOCKET; ++i) {
        curl_socket_t sock = data->conn->sock[i];
        যদি(sock == CURL_SOCKET_BAD)
          অনুৱর্ত;
        যদি(recv_blocked && Curl_pollset_want_recv(data, ps, sock)) {
          result = Curl_pollset_remove_in(data, ps, sock);
          যদি(result)
            ভঙ্গ;
        }
        যদি(send_blocked && Curl_pollset_want_send(data, ps, sock)) {
          result = Curl_pollset_remove_out(data, ps, sock);
          যদি(result)
            ভঙ্গ;
        }
      }
    }

    /* Not blocked and wanting to receive. If there is data pending
     * in the connection filters, make transfer run again. */
    যদি(!recv_blocked &&
       ((Curl_pollset_want_recv(data, ps, data->conn->sock[FIRSTSOCKET]) &&
         Curl_conn_data_pending(data, FIRSTSOCKET)) ||
        (Curl_pollset_want_recv(data, ps, data->conn->sock[SECONDARYSOCKET]) &&
         Curl_conn_data_pending(data, SECONDARYSOCKET)))) {
      CURL_TRC_M(data, "pollset[] has POLLIN, but there is still "
                 "buffered input -> mark as dirty");
      Curl_multi_mark_dirty(data);
    }
  }
  ঘূৰাই result;
}

স্থির CURLcode mstate_connecting_pollset(সংরচনা Curl_easy *data,
                                          সংরচনা easy_pollset *ps)
{
  সংরচনা connectdata *conn = data->conn;
  curl_socket_t sockfd;
  CURLcode result = CURLE_OK;

  যদি(Curl_xfer_recv_is_paused(data))
    ঘূৰাই CURLE_OK;
  /* If a socket is set, receiving is default. If the socket
   * has not been determined yet (eyeballing), always ask the
   * connection filters for what to monitor. */
  sockfd = Curl_conn_get_first_socket(data);
  যদি(sockfd != CURL_SOCKET_BAD) {
    result = Curl_pollset_change(data, ps, sockfd, CURL_POLL_IN, 0);
    যদি(!result)
      result = multi_adjust_pollset(data, ps);
  }
  যদি(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  ঘূৰাই result;
}

স্থির CURLcode mstate_protocol_pollset(সংরচনা Curl_easy *data,
                                        সংরচনা easy_pollset *ps)
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  যদি(conn->scheme->run->proto_pollset)
    result = conn->scheme->run->proto_pollset(data, ps);
  অন্যথা {
    curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
    যদি(sockfd != CURL_SOCKET_BAD) {
      /* Default is to wait to something from the server */
      result = Curl_pollset_change(data, ps, sockfd, CURL_POLL_IN, 0);
    }
  }
  যদি(!result)
    result = multi_adjust_pollset(data, ps);
  যদি(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  ঘূৰাই result;
}

স্থির CURLcode mstate_do_pollset(সংরচনা Curl_easy *data,
                                  সংরচনা easy_pollset *ps)
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  যদি(conn->scheme->run->doing_pollset)
    result = conn->scheme->run->doing_pollset(data, ps);
  অন্যথা যদি(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    result = Curl_pollset_add_out(data, ps, conn->sock[conn->send_idx]);
  }
  যদি(!result)
    result = multi_adjust_pollset(data, ps);
  যদি(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  ঘূৰাই result;
}

স্থির CURLcode mstate_domore_pollset(সংরচনা Curl_easy *data,
                                      সংরচনা easy_pollset *ps)
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  যদি(conn->scheme->run->domore_pollset)
    result = conn->scheme->run->domore_pollset(data, ps);
  অন্যথা যদি(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    result = Curl_pollset_add_out(data, ps, conn->sock[conn->send_idx]);
  }
  যদি(!result)
    result = multi_adjust_pollset(data, ps);
  যদি(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  ঘূৰাই result;
}

স্থির CURLcode mstate_perform_pollset(সংরচনা Curl_easy *data,
                                       সংরচনা easy_pollset *ps)
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  যদি(conn->scheme->run->perform_pollset)
    result = conn->scheme->run->perform_pollset(data, ps);
  অন্যথা {
    /* Default is to obey the request flags for send/recv */
    যদি(Curl_req_want_recv(data) && CONN_SOCK_IDX_VALID(conn->recv_idx)) {
      result = Curl_pollset_add_in(data, ps, conn->sock[conn->recv_idx]);
    }
    যদি(!result && Curl_req_want_send(data) &&
       CONN_SOCK_IDX_VALID(conn->send_idx)) {
      result = Curl_pollset_add_out(data, ps, conn->sock[conn->send_idx]);
    }
  }
  যদি(!result)
    result = multi_adjust_pollset(data, ps);
  যদি(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  ঘূৰাই result;
}

#ifdef CURLVERBOSE
স্থির size_t multi_timeouts_গুনতি(সংরচনা expire_timers *timeouts)
{
  size_t n = 0;
  uint8_t eid = timeouts->first;
  বাবে(; eid < EXPIRE_LAST; eid = timeouts->next[eid])
    ++n;
  ঘূৰাই n;
}
#endif

/* Initializes `poll_set` with the current socket poll actions needed
 * for transfer `data`. */
CURLMcode Curl_multi_pollset(সংরচনা Curl_easy *data,
                             সংরচনা easy_pollset *ps)
{
  CURLcode result = CURLE_OK;

  Curl_pollset_reset(ps);
#ifdef ENABLE_INTERNAL_WAKEUP
  /* The admin handle always listens on the wakeup socket when there
   * are transfers alive. */
  যদি(data->multi && (data == data->multi->admin) &&
     data->multi->xfers_really_alive) {
    CURL_TRC_M(data, "adding wakeup, %u xfers really alive",
               data->multi->xfers_really_alive);
    result = Curl_pollset_add_in(data, ps, data->multi->wakeup_internal[0]);
  }
#endif
  /* If the transfer has no connection, this is fine. Happens when
     called via curl_multi_remove_handle() => Curl_multi_ev_assess() =>
     Curl_multi_pollset(). */
  যদি(!result && data->conn) {
    ৱিকল্প(data->mstate) {
    পক্ষ MSTATE_INIT:
    পক্ষ MSTATE_PENDING:
    পক্ষ MSTATE_SETUP:
    পক্ষ MSTATE_CONNECT:
      /* nothing to poll for yet */
      ভঙ্গ;

    পক্ষ MSTATE_CONNECTING:
      result = mstate_connecting_pollset(data, ps);
      ভঙ্গ;

    পক্ষ MSTATE_PROTOCONNECT:
    পক্ষ MSTATE_PROTOCONNECTING:
      result = mstate_protocol_pollset(data, ps);
      ভঙ্গ;

    পক্ষ MSTATE_DO:
    পক্ষ MSTATE_DOING:
      result = mstate_do_pollset(data, ps);
      ভঙ্গ;

    পক্ষ MSTATE_DOING_MORE:
      result = mstate_domore_pollset(data, ps);
      ভঙ্গ;

    পক্ষ MSTATE_DID: /* same as PERFORMING in regard to polling */
    পক্ষ MSTATE_PERFORMING:
      result = mstate_perform_pollset(data, ps);
      ভঙ্গ;

    পক্ষ MSTATE_RATELIMITING:
      /* we need to let time pass, ignore socket(s) */
      ভঙ্গ;

    পক্ষ MSTATE_DONE:
    পক্ষ MSTATE_COMPLETED:
    পক্ষ MSTATE_MSGSENT:
      /* nothing more to poll for */
      ভঙ্গ;

    স্ৱভাৱ:
      failf(data, "multi_getsock: unexpected multi state %d",
            (পূর্ণাঙ্ক)data->mstate);
      DEBUGASSERT(0);
      ভঙ্গ;
    }
  }

  যদি(result) {
    যদি(result == CURLE_OUT_OF_MEMORY)
      ঘূৰাই CURLM_OUT_OF_MEMORY;
    failf(data, "error determining pollset: %d", (পূর্ণাঙ্ক)result);
    ঘূৰাই CURLM_INTERNAL_ERROR;
  }

#ifdef CURLVERBOSE
  যদি(CURL_TRC_M_is_verbose(data)) {
    size_t timeout_গুনতি = multi_timeouts_গুনতি(&data->state.timeouts);
    ৱিকল্প(ps->n) {
    পক্ষ 0:
      CURL_TRC_M(data, "pollset[], timeouts=%zu, paused %d/%d (r/w)",
                 timeout_গুনতি,
                 Curl_xfer_send_is_paused(data),
                 Curl_xfer_recv_is_paused(data));
      ভঙ্গ;
    পক্ষ 1:
      CURL_TRC_M(data, "pollset[fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_OUT) ? "OUT" : "",
                 timeout_গুনতি);
      ভঙ্গ;
    পক্ষ 2:
      CURL_TRC_M(data, "pollset[fd=%" FMT_SOCKET_T " %s%s, "
                 "fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_OUT) ? "OUT" : "",
                 ps->sockets[1],
                 (ps->actions[1] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[1] & CURL_POLL_OUT) ? "OUT" : "",
                 timeout_গুনতি);
      ভঙ্গ;
    স্ৱভাৱ:
      CURL_TRC_M(data, "pollset[fds=%u], timeouts=%zu", ps->n, timeout_গুনতি);
      ভঙ্গ;
    }
    CURL_TRC_EASY_TIMERS(data);
  }
#endif

  ঘূৰাই CURLM_OK;
}

CURLMcode curl_multi_fdset(CURLM *m,
                           fd_set *read_fd_set, fd_set *write_fd_set,
                           fd_set *exc_fd_set, পূর্ণাঙ্ক *max_fd)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_fdset, &mresult)) {
    /* Scan through all the easy handles to get the file descriptors set.
       Some easy handles may not have connected to the remote host yet,
       and then we must make sure that is done. */
    সংরচনা Curl_multi *multi = m;
    সংরচনা easy_pollset ps;
    পূর্ণাঙ্ক this_max_fd = -1;
    unsigned পূর্ণাঙ্ক i;
    uint32_t mid;
    (শূন্য)exc_fd_set;

    Curl_pollset_init(&ps);
    যদি(Curl_uint32_bset_first(&multi->process, &mid)) {
      কৰা {
        সংরচনা Curl_easy *data = Curl_multi_get_easy(multi, mid);

        যদি(!data) {
          DEBUGASSERT(0);
          অনুৱর্ত;
        }

        Curl_multi_pollset(data, &ps);
        বাবে(i = 0; i < ps.n; i++) {
          যদি(!FDSET_SOCK(ps.sockets[i]))
            /* pretend it does not exist */
            অনুৱর্ত;
          যদি(ps.actions[i] & CURL_POLL_IN)
            FD_SET(ps.sockets[i], read_fd_set);
          যদি(ps.actions[i] & CURL_POLL_OUT)
            FD_SET(ps.sockets[i], write_fd_set);
          যদি((পূর্ণাঙ্ক)ps.sockets[i] > this_max_fd)
            this_max_fd = (পূর্ণাঙ্ক)ps.sockets[i];
        }
      } যেতিয়ালৈ(Curl_uint32_bset_next(&multi->process, mid, &mid));
    }

    Curl_cshutdn_setfds(&multi->cshutdn, multi->admin,
                        read_fd_set, write_fd_set, &this_max_fd);

    *max_fd = this_max_fd;
    Curl_pollset_cleanup(&ps);

    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

CURLMcode curl_multi_waitfds(CURLM *m,
                             সংরচনা curl_waitfd *ufds,
                             unsigned পূর্ণাঙ্ক size,
                             unsigned পূর্ণাঙ্ক *fd_গুনতি)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_waitfds, &mresult)) {
    সংরচনা Curl_waitfds cwfds;
    সংরচনা Curl_multi *multi = m;
    সংরচনা easy_pollset ps;
    unsigned পূর্ণাঙ্ক need = 0;
    uint32_t mid;

    যদি(!ufds && (size || !fd_গুনতি)) {
      mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      গচ্ছ out;
    }

    Curl_pollset_init(&ps);
    Curl_waitfds_init(&cwfds, ufds, size);
    mresult = CURLM_OK;
    যদি(Curl_uint32_bset_first(&multi->process, &mid)) {
      কৰা {
        সংরচনা Curl_easy *data = Curl_multi_get_easy(multi, mid);
        যদি(!data) {
          DEBUGASSERT(0);
          Curl_uint32_bset_remove(&multi->process, mid);
          Curl_uint32_bset_remove(&multi->dirty, mid);
          অনুৱর্ত;
        }
        Curl_multi_pollset(data, &ps);
        need += Curl_waitfds_add_ps(&cwfds, &ps);
      } যেতিয়ালৈ(Curl_uint32_bset_next(&multi->process, mid, &mid));
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
  ঘূৰাই mresult;
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
  পূর্ণাঙ্ক t;
  পূর্ণাঙ্ক l = (পূর্ণাঙ্ক)পরিমাণ(t);
  যদি(!getsockopt(s, SOL_SOCKET, SO_TYPE, (ৱর্ণ *)&t, &l) && t == SOCK_STREAM)
    swrite(s, রিক্ত, 0);
}

স্থির CURLMcode multi_winsock_select(সংরচনা Curl_multi *multi,
                                      সংরচনা curl_pollfds *cpfds,
                                      unsigned পূর্ণাঙ্ক curl_nfds,
                                      সংরচনা curl_waitfd extra_fds[],
                                      unsigned পূর্ণাঙ্ক extra_nfds,
                                      পূর্ণাঙ্ক timeout_ms,
                                      bool extrawait,
                                      পূর্ণাঙ্ক *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  WSANETWORKEVENTS wsa_events;
  পূর্ণাঙ্ক nevents = 0;
  size_t i;

  DEBUGASSERT(multi->wsa_event != WSA_INVALID_EVENT);

  /* Set the WSA events based on the collected pollds */
  বাবে(i = 0; i < cpfds->n; i++) {
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
      গচ্ছ out;
    }
  }

  যদি(cpfds->n || extrawait) {
    পূর্ণাঙ্ক pollrc = 0;

    যদি(cpfds->n) {         /* pre-check with Winsock */
      pollrc = Curl_poll(cpfds->pfds, cpfds->n, 0);
      যদি(pollrc < 0) {
        mresult = CURLM_UNRECOVERABLE_POLL;
        গচ্ছ out;
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
    বাবে(i = 0; i < extra_nfds; i++) {
      unsigned short mask = 0;
      curl_socket_t s = extra_fds[i].fd;

      wsa_events.lNetworkEvents = 0;
      যদি(WSAEnumNetworkEvents(s, রিক্ত, &wsa_events) == 0) {
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
        অনুৱর্ত;
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
    বাবে(i = 0; i < curl_nfds; ++i) {
      wsa_events.lNetworkEvents = 0;
      যদি(WSAEnumNetworkEvents(cpfds->pfds[i].fd, রিক্ত, &wsa_events) == 0) {
        যদি(!pollrc && wsa_events.lNetworkEvents)
          nevents++;
      }
      WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, 0);
    }
    WSAResetEvent(multi->wsa_event);
  }

out:
  *pnevents = nevents;
  ঘূৰাই mresult;
}

#অন্যথা /* USE_WINSOCK */

স্থির CURLMcode multi_posix_poll(সংরচনা Curl_multi *multi,
                                  সংরচনা curl_pollfds *cpfds,
                                  unsigned পূর্ণাঙ্ক curl_nfds,
                                  সংরচনা curl_waitfd extra_fds[],
                                  unsigned পূর্ণাঙ্ক extra_nfds,
                                  পূর্ণাঙ্ক timeout_ms,
                                  bool extrawait,
                                  পূর্ণাঙ্ক *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  পূর্ণাঙ্ক nevents = 0;
  size_t i;

  (শূন্য)multi;
  যদি(cpfds->n) {
    পূর্ণাঙ্ক pollrc = Curl_poll(cpfds->pfds, cpfds->n, timeout_ms); /* wait... */
    যদি(pollrc < 0) {
      mresult = CURLM_UNRECOVERABLE_POLL;
      গচ্ছ out;
    }
    nevents = pollrc;

    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    বাবে(i = 0; i < extra_nfds; i++) {
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
  ঘূৰাই mresult;
}

#endif /* !USE_WINSOCK */

#define অঙ্ক_POLLS_ON_স্তূপ 10

স্থির CURLMcode multi_wait(সংরচনা Curl_multi *multi,
                            সংরচনা curl_waitfd extra_fds[],
                            unsigned পূর্ণাঙ্ক extra_nfds,
                            পূর্ণাঙ্ক timeout_ms,
                            পূর্ণাঙ্ক *ret,
                            bool extrawait)  /* when no socket, wait */
{
  size_t i;
  পূর্ণাঙ্ক timeout_internal;
  পূর্ণাঙ্ক nevents = 0;
  সংরচনা easy_pollset ps;
  সংরচনা pollfd a_few_on_স্তূপ[অঙ্ক_POLLS_ON_স্তূপ];
  সংরচনা curl_pollfds cpfds;
  unsigned পূর্ণাঙ্ক curl_nfds = 0; /* how many pfds are for curl transfers */
  সংরচনা Curl_easy *data = রিক্ত;
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;
#ifdef ENABLE_WAKEUP
  পূর্ণাঙ্ক wakeup_idx = -1;
#endif

  যদি(timeout_ms < 0)
    ঘূৰাই CURLM_BAD_FUNCTION_ARGUMENT;

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_স্তূপ, অঙ্ক_POLLS_ON_স্তূপ);

  /* Add the curl handles to our pollfds first */
  যদি(Curl_uint32_bset_first(&multi->process, &mid)) {
    কৰা {
      data = Curl_multi_get_easy(multi, mid);
      যদি(!data) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        অনুৱর্ত;
      }
      Curl_multi_pollset(data, &ps);
      যদি(Curl_pollfds_add_ps(&cpfds, &ps)) {
        mresult = CURLM_OUT_OF_MEMORY;
        গচ্ছ out;
      }
    } যেতিয়ালৈ(Curl_uint32_bset_next(&multi->process, mid, &mid));
  }

  যদি(Curl_cshutdn_add_pollfds(&multi->cshutdn, multi->admin, &cpfds)) {
    mresult = CURLM_OUT_OF_MEMORY;
    গচ্ছ out;
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
      গচ্ছ out;
    }
  }
#endif

  curl_nfds = cpfds.n; /* what curl internally uses in cpfds */
  /* Add external file descriptions from poll-like struct curl_waitfd */
  বাবে(i = 0; i < extra_nfds; i++) {
    unsigned short events = 0;
    যদি(extra_fds[i].events & CURL_WAIT_POLLIN)
      events |= POLLIN;
    যদি(extra_fds[i].events & CURL_WAIT_POLLPRI)
      events |= POLLPRI;
    যদি(extra_fds[i].events & CURL_WAIT_POLLOUT)
      events |= POLLOUT;
    যদি(Curl_pollfds_add_sock(&cpfds, extra_fds[i].fd, events)) {
      mresult = CURLM_OUT_OF_MEMORY;
      গচ্ছ out;
    }
  }

  /* We check the internal timeout *AFTER* we collected all sockets to
   * poll. Collecting the sockets may install new timers by protocols
   * and connection filters.
   * Use the shorter one of the internal and the caller requested timeout.
   * If we are called with `!extrawait` and multi_timeout() reports no
   * timeouts exist, do not wait. */
  multi_timeout(multi, রিক্ত, &timeout_internal);
  যদি((timeout_internal >= 0) && (timeout_internal < timeout_ms))
    timeout_ms = timeout_internal;

  যদি(data)
    CURL_TRC_M(data, "multi_wait(fds=%u, timeout=%d) tinternal=%d",
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
  ঘূৰাই mresult;
}

CURLMcode curl_multi_wait(CURLM *m,
                          সংরচনা curl_waitfd extra_fds[],
                          unsigned পূর্ণাঙ্ক extra_nfds,
                          পূর্ণাঙ্ক timeout_ms,
                          পূর্ণাঙ্ক *ret)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_wait, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, FALSE);
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

CURLMcode curl_multi_poll(CURLM *m,
                          সংরচনা curl_waitfd extra_fds[],
                          unsigned পূর্ণাঙ্ক extra_nfds,
                          পূর্ণাঙ্ক timeout_ms,
                          পূর্ণাঙ্ক *ret)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_poll, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, TRUE);
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

CURLMcode curl_multi_wakeup(CURLM *m)
{
  /* this function is usually called from another thread,
     it has to be careful only to access parts of the
     Curl_multi struct that are constant */
  সংরচনা Curl_multi *multi = m;
  CURLMcode mresult = CURLM_WAKEUP_FAILURE;

  /* GOOD_MULTI_HANDLE can be safely called */
  যদি(!GOOD_MULTI_HANDLE(multi))
    ঘূৰাই CURLM_BAD_HANDLE;

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
  ঘূৰাই mresult;
}

#ifdef ENABLE_INTERNAL_WAKEUP
শূন্য Curl_multi_wakeup_internal(সংরচনা Curl_multi *multi)
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
স্থির bool multi_ischanged(সংরচনা Curl_multi *multi, bool clear)
{
  bool retval = FALSE;
  DEBUGASSERT(multi);
  যদি(multi) {
    retval = (bool)multi->recheckstate;
    যদি(clear)
      multi->recheckstate = FALSE;
  }
  ঘূৰাই retval;
}

/*
 * Curl_multi_connchanged() is called to tell that there is a connection in
 * this multi handle that has changed state (multiplexing become possible, the
 * number of allowed streams changed or similar), and a subsequent use of this
 * multi handle should move CONNECT_PEND handles back to CONNECT to have them
 * retry.
 */
শূন্য Curl_multi_connchanged(সংরচনা Curl_multi *multi)
{
  /* May be invoked for a `data->multi` when no multi is present. */
  যদি(multi)
    multi->recheckstate = TRUE;
}

CURLMcode Curl_multi_add_perform(সংরচনা Curl_multi *multi,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা connectdata *conn)
{
  CURLMcode mresult;

  mresult = Curl_multi_add_handle(multi, data);
  যদি(!mresult) {
    CURLcode result;

    /* pass in NULL for 'conn' here since we do not want to init the
       connection, only this transfer */
    result = Curl_init_transfer(data, রিক্ত);
    যদি(result) {
      Curl_multi_remove_handle(multi, data);
      ঘূৰাই CURLM_INTERNAL_ERROR;
    }

    /* take this handle to the perform state right away */
    multistate(data, MSTATE_PERFORMING);
    Curl_attach_connection(data, conn, TRUE);
    CURL_REQ_SET_RECV(data);
  }
  ঘূৰাই mresult;
}

স্থির CURLcode multi_do(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(conn->scheme);

  যদি(conn->scheme->run->do_it)
    result = conn->scheme->run->do_it(data, done);

  ঘূৰাই result;
}

/*
 * multi_do_more() is called during the DO_MORE multi state. It is a second
 * stage DO state which (wrongly) was introduced to support FTP's second
 * connection.
 *
 * 'complete' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (to DOING state when there is more work to do)
 */

স্থির CURLcode multi_do_more(সংরচনা Curl_easy *data, domore *complete)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;

  *complete = DOMORE_INCOMPLETE;

  যদি(conn->scheme->run->do_more)
    result = conn->scheme->run->do_more(data, complete);

  ঘূৰাই result;
}

/*
 * Check whether a timeout occurred, and handle it if it did
 */
স্থির bool multi_handle_timeout(সংরচনা Curl_easy *data,
                                 ধ্রুৱ সংরচনা curltime *pnow,
                                 bool *stream_error,
                                 CURLcode *result)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_timeleft_now_ms(data, pnow);
  যদি(timeout_ms < 0) {
    /* Handle timed out */
    timerid base_timer = Curl_is_connecting(data) ?
                         TIMER_STARTSINGLE : TIMER_STARTOP;
    timediff_t elapsed_ms = Curl_pgrs_since_ms(data, রিক্ত, base_timer);
    যদি(data->mstate == MSTATE_CONNECTING) {
      failf(data, "%s timed out after %" FMT_TIMEDIFF_T " milliseconds",
            data->conn->bits.dns_resolved ? "Connection" : "Resolving",
            elapsed_ms);
      /* keep what we know about a partly done connect, e.g. TCP done
       * but TLS handshake still ongoing */
      Curl_conn_report_failed(data, FIRSTSOCKET);
    }
    অন্যথা {
      সংরচনা SingleRequest *k = &data->req;
      যদি(k->size != -1) {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount, k->size);
      }
      অন্যথা {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount);
      }
    }
    *result = CURLE_OPERATION_TIMEDOUT;
    যদি(data->conn) {
      /* Force connection closed if the connection has indeed been used */
      যদি(data->mstate > MSTATE_DO) {
        streamclose(data->conn);
        *stream_error = TRUE;
      }
      (শূন্য)multi_done(data, *result, TRUE);
    }
    ঘূৰাই TRUE;
  }

  ঘূৰাই FALSE;
}

/*
 * We are doing protocol-specific connecting and this is being called over and
 * over from the multi interface until the connection phase is done on
 * protocol layer.
 */

স্থির CURLcode protocol_connecting(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;

  যদি(conn && conn->scheme->run->connecting) {
    *done = FALSE;
    result = conn->scheme->run->connecting(data, done);
  }
  অন্যথা
    *done = TRUE;

  ঘূৰাই result;
}

/*
 * We are DOING this is being called over and over from the multi interface
 * until the DOING phase is done on protocol layer.
 */

স্থির CURLcode protocol_doing(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  সংরচনা connectdata *conn = data->conn;

  যদি(conn && conn->scheme->run->doing) {
    *done = FALSE;
    result = conn->scheme->run->doing(data, done);
  }
  অন্যথা
    *done = TRUE;

  ঘূৰাই result;
}

/*
 * We have discovered that the TCP connection has been successful, we can now
 * proceed with some action.
 *
 */
স্থির CURLcode protocol_connect(সংরচনা Curl_easy *data, bool *protocol_done)
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(conn);
  DEBUGASSERT(protocol_done);
  DEBUGASSERT(Curl_conn_is_connected(conn, FIRSTSOCKET));

  *protocol_done = FALSE;
  যদি(!conn->bits.protoconnstart) {
    যদি(conn->scheme->run->connect_it) {
      /* Call the protocol-specific connect function */
      result = conn->scheme->run->connect_it(data, protocol_done);
      যদি(result)
        ঘূৰাই result;
    }
    conn->bits.protoconnstart = TRUE;
  }

  /* Unless this protocol does not have any protocol-connect callback, as
     then we know we are done. */
  যদি(!conn->scheme->run->connecting)
    *protocol_done = TRUE;
  ঘূৰাই CURLE_OK;
}

/*
 * posttransfer() is called immediately after a transfer ends
 */
স্থির শূন্য multi_posttransfer(সংরচনা Curl_easy *data)
{
#যদি defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
  /* restore the signal handler for SIGPIPE before we get back */
  যদি(!data->set.no_signal)
    সঙ্কেত(SIGPIPE, data->state.prev_signal);
#অন্যথা
  (শূন্য)data;
#endif
}

/*
 * multi_follow() handles the URL redirect magic. Pass in the 'newurl' string
 * as given by the remote server and set up the new URL to request.
 *
 * This function DOES NOT FREE the given URL.
 */
স্থির CURLcode multi_follow(সংরচনা Curl_easy *data,
                             ধ্রুৱ সংরচনা Curl_scheme *handler,
                             ধ্রুৱ ৱর্ণ *newurl, /* the Location: string */
                             followtype type) /* see transfer.h */
{
  যদি(handler && handler->run->follow)
    ঘূৰাই handler->run->follow(data, newurl, type);

  যদি(type == FOLLOW_RETRY)
    /* Retries are generic and do not require protocol-specific redirect
       handling. */
    ঘূৰাই CURLE_OK;

  ঘূৰাই CURLE_TOO_MANY_REDIRECTS;
}

স্থির CURLcode mspeed_check(সংরচনা Curl_easy *data)
{
  যদি(Curl_rlimit_active(&data->progress.dl.rlimit) ||
     Curl_rlimit_active(&data->progress.ul.rlimit)) {
    /* check if our send/recv limits require idle waits */
    ধ্রুৱ সংরচনা curltime *pnow = Curl_pgrs_now(data);
    timediff_t recv_ms, send_ms;

    send_ms = Curl_rlimit_wait_ms(&data->progress.ul.rlimit, pnow);
    recv_ms = Curl_rlimit_wait_ms(&data->progress.dl.rlimit, pnow);

    যদি(send_ms || recv_ms) {
      যদি(data->mstate != MSTATE_RATELIMITING) {
        multistate(data, MSTATE_RATELIMITING);
      }
      Curl_expire_set(data, EXPIRE_TOOFAST, CURLMAX(send_ms, recv_ms), pnow);
      Curl_multi_clear_dirty(data);
      CURL_TRC_M(data, "[RLIMIT] waiting %" FMT_TIMEDIFF_T "ms",
                 CURLMAX(send_ms, recv_ms));
      ঘূৰাই CURLE_AGAIN;
    }
    অন্যথা {
      /* when will the rate limits increase next? The transfer needs
       * to run again at that time or it may stall. */
      send_ms = Curl_rlimit_next_step_ms(&data->progress.ul.rlimit, pnow);
      recv_ms = Curl_rlimit_next_step_ms(&data->progress.dl.rlimit, pnow);
      যদি(send_ms || recv_ms) {
        timediff_t next_ms = CURLMIN(send_ms, recv_ms);
        যদি(!next_ms)
          next_ms = CURLMAX(send_ms, recv_ms);
        Curl_expire_set(data, EXPIRE_TOOFAST, next_ms, pnow);
        CURL_TRC_M(data, "[RLIMIT] next token update in %" FMT_TIMEDIFF_T "ms",
                   next_ms);
      }
    }
  }

  যদি(data->mstate != MSTATE_PERFORMING) {
    CURL_TRC_M(data, "[RLIMIT] wait over, continue");
    multistate(data, MSTATE_PERFORMING);
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLMcode multistate_performing(সংরচনা Curl_easy *data,
                                       bool *stream_errorp,
                                       CURLcode *resultp)
{
  ৱর্ণ *newurl = রিক্ত;
  bool retry = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = *resultp = CURLE_OK;
  *stream_errorp = FALSE;

  যদি(mspeed_check(data) == CURLE_AGAIN)
    ঘূৰাই CURLM_OK;

  /* read/write data if it is ready to do so */
  result = Curl_sendrecv(data);

  যদি(data->req.done || (result == CURLE_RECV_ERROR)) {
    /* If CURLE_RECV_ERROR happens early enough, we assume it was a race
     * condition and the server closed the reused connection exactly when we
     * wanted to use it, so figure out if that is indeed the case.
     */
    CURLcode ret = Curl_retry_request(data, &newurl);
    যদি(!ret)
      retry = !!newurl;
    অন্যথা যদি(!result)
      result = ret;

    যদি(retry) {
      /* if we are to retry, set the result to OK and consider the
         request as done */
      result = CURLE_OK;
      data->req.done = TRUE;
    }
  }
#ifndef CURL_DISABLE_HTTP
  অন্যথা যদি((result == CURLE_HTTP2_STREAM) &&
          Curl_h2_http_1_1_error(data)) {
    CURLcode ret = Curl_retry_request(data, &newurl);

    যদি(!ret) {
      infof(data, "Downgrades to HTTP/1.1");
      streamclose(data->conn);
      data->state.http_neg.wanted = CURL_HTTP_V1x;
      data->state.http_neg.allowed = CURL_HTTP_V1x;
      /* clear the error message bit too as we ignore the one we got */
      data->state.errorbuf = FALSE;
      যদি(!newurl)
        /* typically for HTTP_1_1_REQUIRED error on first flight */
        newurl = Curl_bufref_dup(&data->state.url);
      যদি(!newurl) {
        result = CURLE_OUT_OF_MEMORY;
      }
      অন্যথা {
        /* if we are to retry, set the result to OK and consider the request
          as done */
        retry = TRUE;
        result = CURLE_OK;
        data->req.done = TRUE;
      }
    }
    অন্যথা
      result = ret;
  }
#endif

  যদি(result) {
    /*
     * The transfer phase returned error, we mark the connection to get closed
     * to prevent being reused. This is because we cannot possibly know if the
     * connection is in a good shape or not now. Unless it is a protocol which
     * uses two "channels" like FTP, as then the error happened in the data
     * connection.
     */

    যদি(!(data->conn->scheme->flags & PROTOPT_DUAL) &&
       result != CURLE_HTTP2_STREAM)
      streamclose(data->conn);

    multi_posttransfer(data);
    multi_done(data, result, TRUE);
  }
  অন্যথা যদি(data->req.done && !Curl_cwriter_is_paused(data)) {
    ধ্রুৱ সংরচনা Curl_scheme *handler = data->conn->scheme;

    /* call this even if the readwrite function returned error */
    multi_posttransfer(data);

    /* When we follow redirects or is set to retry the connection, we must to
       go back to the CONNECT state */
    যদি(data->req.newurl || retry) {
      followtype follow = FOLLOW_NONE;
      যদি(!retry) {
        /* if the URL is a follow-location and not a retried request then
           figure out the URL here */
        curlx_free(newurl);
        newurl = data->req.newurl;
        data->req.newurl = রিক্ত;
        follow = FOLLOW_REDIR;
      }
      অন্যথা
        follow = FOLLOW_RETRY;
      (শূন্য)multi_done(data, CURLE_OK, FALSE);
      /* multi_done() might return CURLE_GOT_NOTHING */
      result = multi_follow(data, handler, newurl, follow);
      যদি(!result) {
        multistate(data, MSTATE_SETUP);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    অন্যথা {
      /* after the transfer is done, go DONE */

      /* but first check to see if we got a location info even though we are
         not following redirects */
      যদি(data->req.location) {
        curlx_free(newurl);
        newurl = data->req.location;
        data->req.location = রিক্ত;
        result = multi_follow(data, handler, newurl, FOLLOW_FAKE);
        যদি(result) {
          *stream_errorp = TRUE;
          result = multi_done(data, result, TRUE);
        }
      }

      যদি(!result) {
        multistate(data, MSTATE_DONE);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
  }
  অন্যথা { /* not errored, not done */
    mspeed_check(data);
  }
  curlx_free(newurl);
  *resultp = result;
  ঘূৰাই mresult;
}

স্থির CURLMcode multistate_do(সংরচনা Curl_easy *data,
                               bool *stream_errorp,
                               CURLcode *resultp)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;
  যদি(data->set.fprereq) {
    সংরচনা Curl_mapi_guard guard;
    পূর্ণাঙ্ক prereq_rc;

    /* call the prerequest callback function */
    CURL_CBAPI_START(&guard, data, easy_fprereq);
    prereq_rc = data->set.fprereq(data->set.prereq_userp,
                                  data->info.primary.remote_ip,
                                  data->info.primary.local_ip,
                                  data->info.primary.remote_port,
                                  data->info.primary.local_port);
    CURL_CBAPI_END(&guard);
    যদি(prereq_rc != CURL_PREREQFUNC_OK) {
      failf(data, "operation aborted by pre-request callback");
      /* failure in pre-request callback - do not do any other processing */
      result = CURLE_ABORTED_BY_CALLBACK;
      multi_posttransfer(data);
      multi_done(data, result, FALSE);
      *stream_errorp = TRUE;
      গচ্ছ end;
    }
  }

  যদি(data->set.connect_only && !data->set.connect_only_ws) {
    multistate(data, MSTATE_DONE);
    mresult = CURLM_CALL_MULTI_PERFORM;
  }
  অন্যথা {
    bool dophase_done = FALSE;
    /* Perform the protocol's DO action */
    result = multi_do(data, &dophase_done);

    যদি(!result) {
      যদি(!dophase_done) {
#ifndef CURL_DISABLE_FTP
        /* some steps needed for wildcard matching */
        যদি(data->state.wildcardmatch) {
          সংরচনা WildcardData *wc = data->wildcard;
          যদি(wc->state == CURLWC_DONE || wc->state == CURLWC_SKIP) {
            /* skip some states if it is important */
            multi_done(data, CURLE_OK, FALSE);

            /* if there is no connection left, skip the DONE state */
            multistate(data, data->conn ? MSTATE_DONE : MSTATE_COMPLETED);
            mresult = CURLM_CALL_MULTI_PERFORM;
            গচ্ছ end;
          }
        }
#endif
        /* DO was not completed in one function call, we must continue
           DOING... */
        multistate(data, MSTATE_DOING);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }

      /* after DO, go DO_DONE... or DO_MORE */
      অন্যথা যদি(data->conn->bits.do_more) {
        /* we are supposed to do more, but we need to sit down, relax and wait
           a little while first */
        multistate(data, MSTATE_DOING_MORE);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
      অন্যথা {
        /* we are done with the DO, now DID */
        multistate(data, MSTATE_DID);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    অন্যথা যদি((result == CURLE_SEND_ERROR) &&
            data->conn->bits.reuse) {
      /*
       * In this situation, a connection that we were trying to use may have
       * unexpectedly died. If possible, send the connection back to the
       * CONNECT phase so we can try again.
       */
      ধ্রুৱ সংরচনা Curl_scheme *handler = data->conn->scheme;
      ৱর্ণ *newurl = রিক্ত;
      followtype follow = FOLLOW_NONE;
      CURLcode drc;

      drc = Curl_retry_request(data, &newurl);
      যদি(drc) {
        /* a failure here pretty much implies an out of memory */
        result = drc;
        *stream_errorp = TRUE;
      }

      multi_posttransfer(data);
      drc = multi_done(data, result, FALSE);

      /* When set to retry the connection, we must go back to the CONNECT
       * state */
      যদি(newurl) {
        যদি(!drc || (drc == CURLE_SEND_ERROR)) {
          follow = FOLLOW_RETRY;
          drc = multi_follow(data, handler, newurl, follow);
          যদি(!drc) {
            multistate(data, MSTATE_SETUP);
            mresult = CURLM_CALL_MULTI_PERFORM;
            result = CURLE_OK;
          }
          অন্যথা {
            /* Follow failed */
            result = drc;
          }
        }
        অন্যথা {
          /* done did not return OK or SEND_ERROR */
          result = drc;
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
      multi_posttransfer(data);
      যদি(data->conn)
        multi_done(data, result, FALSE);
      *stream_errorp = TRUE;
    }
  }
end:
  *resultp = result;
  ঘূৰাই mresult;
}

স্থির CURLMcode multistate_ratelimiting(সংরচনা Curl_easy *data,
                                         CURLcode *resultp)
{
  CURLcode result = CURLE_OK;
  CURLMcode mresult = CURLM_OK;
  DEBUGASSERT(data->conn);
  /* if both rates are within spec, resume transfer */
  result = Curl_pgrsCheck(data);

  যদি(result) {
    যদি(!(data->conn->scheme->flags & PROTOPT_DUAL) &&
       result != CURLE_HTTP2_STREAM)
      streamclose(data->conn);

    multi_posttransfer(data);
    multi_done(data, result, TRUE);
  }
  অন্যথা {
    যদি(!mspeed_check(data))
      mresult = CURLM_CALL_MULTI_PERFORM;
  }
  *resultp = result;
  ঘূৰাই mresult;
}

স্থির CURLMcode multistate_connect(সংরচনা Curl_multi *multi,
                                    সংরচনা Curl_easy *data,
                                    CURLcode *resultp)
{
  /* Connect. We want to get a connection identifier filled in. This state can
     be entered from SETUP and from PENDING. */
  bool connected;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = Curl_connect(data, &connected);
  যদি(result == CURLE_NO_CONNECTION_AVAILABLE) {
    /* There was no connection available. We will go to the pending state and
       wait for an available connection. */
    multistate(data, MSTATE_PENDING);
    /* move from process to pending set */
    Curl_uint32_bset_remove(&multi->process, data->mid);
    Curl_uint32_bset_remove(&multi->dirty, data->mid);
    Curl_uint32_bset_add(&multi->pending, data->mid);
    *resultp = CURLE_OK;
    ঘূৰাই mresult;
  }
  অন্যথা
    multi_schedule_pending(data->multi);

  যদি(!result) {
    /* after the connect has been sent off, go WAITCONNECT unless the
       protocol connect is already done and we can go directly to WAITDO or
       DO! */
    mresult = CURLM_CALL_MULTI_PERFORM;

    যদি(connected) {
      যদি(!data->conn->bits.reuse &&
         Curl_conn_is_multiplex(data->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_schedule_pending(data->multi);
      }
      multistate(data, MSTATE_PROTOCONNECT);
    }
    অন্যথা {
      multistate(data, MSTATE_CONNECTING);
    }
  }
  *resultp = result;
  ঘূৰাই mresult;
}

/* returns the possibly updated result */
স্থির CURLcode is_finished(সংরচনা Curl_multi *multi,
                            সংরচনা Curl_easy *data,
                            ধ্রুৱ সংরচনা curltime *pnow,
                            bool stream_error,
                            CURLcode result)
{
  যদি(data->mstate < MSTATE_COMPLETED) {
    যদি(result) {
      /*
       * If an error was returned, and we are not in completed state now,
       * then we go to completed and consider this transfer aborted.
       */

      /* No attempt to disconnect connections must be made before this -
         connection detach and termination happens only here */

      /* Check if we can move pending requests to send pipe */
      multi_schedule_pending(multi); /* connection */

      যদি(data->conn) {
        যদি(stream_error) {
          /* Do not attempt to send data over a connection that timed out */
          bool dead_connection = result == CURLE_OPERATION_TIMEDOUT;
          সংরচনা connectdata *conn = data->conn;

          /* This is where we make sure that the conn pointer is reset.
             We do not have to do this in every case block above where a
             failure is detected */
          Curl_detach_connection(data);
          Curl_conn_close(data, conn, dead_connection);
        }
      }
      অন্যথা যদি(data->mstate == MSTATE_CONNECT) {
        /* Curl_connect() failed */
        multi_posttransfer(data);
        Curl_pgrsUpdate_nometer(data);
      }

      multistate(data, MSTATE_COMPLETED);
      ঘূৰাই result;
    }
    /* if there is still a connection to use, call the progress function */
    অন্যথা যদি(data->conn && Curl_conn_is_connected(data->conn, FIRSTSOCKET)) {
      result = Curl_pgrsUpdateX(data, pnow);
      যদি(result) {
        /* aborted due to progress callback return code must close the
           connection */
        streamclose(data->conn);

        /* if not yet in DONE state, go there. */
        যদি(data->mstate < MSTATE_DONE)
          multistate(data, MSTATE_DONE);
        ঘূৰাই result;
      }
    }
  }
  ঘূৰাই result;
}

স্থির শূন্য handle_completed(সংরচনা Curl_multi *multi,
                             সংরচনা Curl_easy *data,
                             CURLcode result)
{
  bool বাৰ্তা_to_app = data->master_mid == UINT32_MAX;

  যদি(!বাৰ্তা_to_app) {
    /* A sub transfer, not reported to the application. Is anyone still
     * interested in processing its results? */
    যদি(data->sub_xfer_done) {
      সংরচনা Curl_easy *master = Curl_multi_get_easy(multi, data->master_mid);

      CURL_TRC_M(data, "sub xfer done for master %u", data->master_mid);
      যদি(master)
        data->sub_xfer_done(data, master, result);
      অন্যথা
        CURL_TRC_M(data, "master easy %u already gone.", data->master_mid);
    }
  }
  অন্যথা {
    /* now fill in the CURLMsg with this info */
    সংরচনা CURLMsg *বাৰ্তা = &data->বাৰ্তা;

    বাৰ্তা->বাৰ্তা = CURLMSG_DONE;
    বাৰ্তা->easy_handle = data;
    বাৰ্তা->data.result = result;

    DEBUGASSERT(!data->conn);
  }
  multistate(data, MSTATE_MSGSENT);

  /* remove from the other sets */
  Curl_uint32_bset_remove(&multi->process, data->mid);
  Curl_uint32_bset_remove(&multi->dirty, data->mid);
  Curl_uint32_bset_remove(&multi->pending, data->mid);
  যদি(বাৰ্তা_to_app)
    multi_addmsg(multi, data);
  যদি(data->state.really_alive) {
    data->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    যদি(!multi->xfers_really_alive)
      (শূন্য)multi_assess_wakeup(multi);
  }
  --multi->xfers_alive;
  যদি(!multi->xfers_alive)
    multi_assess_wakeup(multi);
}

স্থির CURLMcode multistate_init(সংরচনা Curl_easy *data, CURLcode *result)
{
  যদি(!data->state.really_alive) {
    data->state.really_alive = TRUE;
    ++data->multi->xfers_really_alive;
    যদি(data->multi->xfers_really_alive == 1) {
      CURLMcode mresult = multi_assess_wakeup(data->multi);
      যদি(mresult) {
        failf(data, "error enabling wakeup listening: %d", mresult);
        ঘূৰাই mresult;
      }
    }
  }

  *result = Curl_pretransfer(data);
  যদি(*result)
    ঘূৰাই CURLM_OK;

  /* after init, go SETUP */
  multistate(data, MSTATE_SETUP);
  Curl_pgrsTime(data, TIMER_STARTOP);
  ঘূৰাই CURLM_CALL_MULTI_PERFORM;
}

স্থির CURLMcode multistate_setup(সংরচনা Curl_easy *data)
{
  ধ্রুৱ সংরচনা curltime *pnow = Curl_pgrs_now(data);
  Curl_pgrsTimeWas(data, TIMER_STARTSINGLE, *pnow);
  যদি(data->set.timeout)
    Curl_expire_set(data, EXPIRE_TIMEOUT, data->set.timeout, pnow);
  যদি(data->set.connecttimeout)
    /* Since a connection might go to pending and back to CONNECT several
       times before it actually takes off, we need to set the timeout once
       in SETUP before we enter CONNECT the first time. */
    Curl_expire_set(data, EXPIRE_CONNECTTIMEOUT,
                    data->set.connecttimeout, pnow);

  multistate(data, MSTATE_CONNECT);
  ঘূৰাই CURLM_CALL_MULTI_PERFORM;
}

স্থির CURLMcode multistate_connecting(সংরচনা Curl_easy *data,
                                       bool *stream_error,
                                       CURLcode *result)
{
  bool connected;

  যদি(!data->conn) {
    DEBUGASSERT(0);
    *result = CURLE_FAILED_INIT;
    ঘূৰাই CURLM_OK;
  }
  যদি(!Curl_xfer_recv_is_paused(data)) {
    *result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &connected);
    যদি(connected && !*result) {
      যদি(!data->conn->bits.reuse &&
         Curl_conn_is_multiplex(data->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_schedule_pending(data->multi);
      }
      multistate(data, MSTATE_PROTOCONNECT);
      ঘূৰাই CURLM_CALL_MULTI_PERFORM;
    }
    অন্যথা যদি(*result) {
      /* failure detected */
      CURL_TRC_M(data, "connect failed -> %d", (পূর্ণাঙ্ক)*result);
      multi_posttransfer(data);
      multi_done(data, *result, TRUE);
      *stream_error = TRUE;
      ঘূৰাই CURLM_OK;
    }
  }
  ঘূৰাই CURLM_OK;
}

স্থির CURLMcode multistate_protoconnect(সংরচনা Curl_easy *data,
                                         bool *stream_error,
                                         CURLcode *result)
{
  bool protocol_connected = FALSE;

  যদি(!*result && data->conn->bits.reuse) {
    /* ftp seems to hang when protoconnect on reused connection since we
     * handle PROTOCONNECT in general inside the filters, it seems wrong to
     * restart this on a reused connection.
     */
    multistate(data, MSTATE_DO);
    ঘূৰাই CURLM_CALL_MULTI_PERFORM;
  }
  যদি(!*result)
    *result = protocol_connect(data, &protocol_connected);
  যদি(!*result && !protocol_connected) {
    /* switch to waiting state */
    multistate(data, MSTATE_PROTOCONNECTING);
    ঘূৰাই CURLM_CALL_MULTI_PERFORM;
  }
  অন্যথা যদি(!*result) {
    /* protocol connect has completed, go WAITDO or DO */
    multistate(data, MSTATE_DO);
    ঘূৰাই CURLM_CALL_MULTI_PERFORM;
  }

  /* failure detected */
  multi_posttransfer(data);
  multi_done(data, *result, TRUE);
  *stream_error = TRUE;
  ঘূৰাই CURLM_OK;
}

স্থির CURLMcode multistate_protoconnecting(সংরচনা Curl_easy *data,
                                            bool *stream_error,
                                            CURLcode *result)
{
  bool protocol_connected = FALSE;

  /* protocol-specific connect phase */
  *result = protocol_connecting(data, &protocol_connected);
  যদি(!*result && protocol_connected) {
    /* after the connect has completed, go WAITDO or DO */
    multistate(data, MSTATE_DO);
    ঘূৰাই CURLM_CALL_MULTI_PERFORM;
  }
  অন্যথা যদি(*result) {
    /* failure detected */
    multi_posttransfer(data);
    multi_done(data, *result, TRUE);
    *stream_error = TRUE;
  }
  ঘূৰাই CURLM_OK;
}

স্থির CURLMcode multistate_doing(সংরচনা Curl_easy *data,
                                  bool *stream_error,
                                  CURLcode *result)
{
  bool dophase_done = FALSE;

  /* we continue DOING until the DO phase is complete */
  DEBUGASSERT(data->conn);
  *result = protocol_doing(data, &dophase_done);
  যদি(!*result) {
    যদি(dophase_done) {
      /* after DO, go DO_DONE or DO_MORE */
      multistate(data, data->conn->bits.do_more ?
                 MSTATE_DOING_MORE : MSTATE_DID);
      ঘূৰাই CURLM_CALL_MULTI_PERFORM;
    } /* dophase_done */
  }
  অন্যথা {
    /* failure detected */
    multi_posttransfer(data);
    multi_done(data, *result, FALSE);
    *stream_error = TRUE;
  }
  ঘূৰাই CURLM_OK;
}

স্থির CURLMcode multistate_doing_more(সংরচনা Curl_easy *data,
                                       bool *stream_error,
                                       CURLcode *result)
{
  domore control;

  /*
   * When we are connected, DOING MORE and then go DID
   */
  DEBUGASSERT(data->conn);
  *result = multi_do_more(data, &control);

  যদি(!*result) {
    যদি(control != DOMORE_INCOMPLETE) {
      /* if DONE, advance to DO_DONE
         if GOBACK, go back to DOING */
      multistate(data, control == DOMORE_DONE ? MSTATE_DID : MSTATE_DOING);
      ঘূৰাই CURLM_CALL_MULTI_PERFORM;
    }
    /* else
       stay in DO_MORE */
  }
  অন্যথা {
    /* failure detected */
    multi_posttransfer(data);
    multi_done(data, *result, FALSE);
    *stream_error = TRUE;
  }
  ঘূৰাই CURLM_OK;
}

স্থির CURLMcode multistate_did(সংরচনা Curl_multi *multi,
                                সংরচনা Curl_easy *data)
{
  DEBUGASSERT(data->conn);
  যদি(data->conn->bits.multiplex)
    /* Check if we can move pending requests to send pipe */
    multi_schedule_pending(multi); /* multiplexed */

  /* Only perform the transfer if there is a good socket to work with.
     Having both BAD is a signal to skip immediately to DONE */
  যদি(CONN_SOCK_IDX_VALID(data->conn->recv_idx) ||
     CONN_SOCK_IDX_VALID(data->conn->send_idx)) {
    multistate(data, MSTATE_PERFORMING);
    /* Do not return CURLM_CALL_MULTI_PERFORM to give other transfers
     * a chance to send off their requests.
     * Note: Some SFTP handlers do not seem to like this.
     *       Restrict it to HTTP families. */
    ঘূৰাই ((multi->xfers_alive > 1) &&
            (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) ?
           CURLM_OK : CURLM_CALL_MULTI_PERFORM;
  }
  অন্যথা {
#ifndef CURL_DISABLE_FTP
    যদি(data->state.wildcardmatch &&
       ((data->conn->scheme->flags & PROTOPT_WILDCARD) == 0)) {
      data->wildcard->state = CURLWC_DONE;
    }
#endif
    multistate(data, MSTATE_DONE);
    ঘূৰাই CURLM_CALL_MULTI_PERFORM;
  }
}

স্থির CURLMcode multistate_done(সংরচনা Curl_easy *data, CURLcode *presult)
{
  যদি(data->conn) {
    CURLcode result;

    /* post-transfer command */
    result = multi_done(data, *presult, FALSE);

    /* allow a previously set error code take precedence */
    যদি(!(*presult))
      *presult = result;
  }

#ifndef CURL_DISABLE_FTP
  যদি(data->state.wildcardmatch) {
    যদি(data->wildcard->state != CURLWC_DONE) {
      /* if a wildcard is set and we are not ending -> lets start again
         with MSTATE_INIT */
      multistate(data, MSTATE_INIT);
      ঘূৰাই CURLM_CALL_MULTI_PERFORM;
    }
  }
#endif
  /* after we have DONE what we are supposed to do, go COMPLETED, and
     it does not matter what the multi_done() returned! */
  multistate(data, MSTATE_COMPLETED);
  ঘূৰাই CURLM_CALL_MULTI_PERFORM;
}

স্থির CURLMcode multi_runsingle(সংরচনা Curl_multi *multi,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা Curl_sigpipe_ctx *sigpipe_ctx)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;
  ধ্রুৱ সংরচনা curltime *pnow = রিক্ত;

  যদি(multi->dead) {
    /* a multi-level callback returned error before, meaning every individual
     transfer now has failed */
    result = CURLE_ABORTED_BY_CALLBACK;
    multi_posttransfer(data);
    multi_done(data, result, FALSE);
    multistate(data, MSTATE_COMPLETED);
  }

  multi_warn_debug(multi, data);

  /* transfer runs now, clear the dirty bit. This may be set
   * again during processing, triggering a re-run later. */
  Curl_uint32_bset_remove(&multi->dirty, data->mid);

  যদি(data == multi->admin) {
#ifdef ENABLE_INTERNAL_WAKEUP
    /* Consume any pending wakeup signals before processing.
     * This is necessary for event based processing. See #21547 */
    (শূন্য)Curl_wakeup_consume(multi->wakeup_internal, TRUE);
#endif
#ifdef USE_RESOLV_THREADED
    Curl_async_thrdd_multi_process(multi);
#endif
    Curl_cshutdn_perform(&multi->cshutdn, multi->admin, sigpipe_ctx);
    গচ্ছ out;
  }

  sigpipe_apply(data, sigpipe_ctx);
  কৰা {
    /* A "stream" here is a logical stream if the protocol can handle that
       (HTTP/2), or the full connection for older protocols */
    bool stream_error = FALSE;
    mresult = CURLM_OK;
    pnow = রিক্ত;

    যদি(multi_ischanged(multi, TRUE)) {
      CURL_TRC_M(data, "multi changed, check CONNECT_PEND queue");
      multi_schedule_pending(multi); /* multiplexed */
    }

    যদি(data->mstate > MSTATE_CONNECT &&
       data->mstate < MSTATE_COMPLETED) {
      /* Make sure we set the connection's current owner */
      DEBUGASSERT(data->conn);
      যদি(!data->conn) {
        mresult = CURLM_INTERNAL_ERROR;
        গচ্ছ out;
      }
    }

    /* Wait for the connect state as only then is the start time stored, but
       we must not check already completed handles */
    যদি((data->mstate >= MSTATE_CONNECT) && (data->mstate < MSTATE_COMPLETED)) {
      pnow = Curl_pgrs_now(data);
      যদি(multi_handle_timeout(data, pnow, &stream_error, &result))
        /* Skip the statemachine and go directly to error handling section. */
        গচ্ছ statemachine_end;
      pnow = রিক্ত;
    }

    ৱিকল্প(data->mstate) {
    পক্ষ MSTATE_INIT:
      /* Transitional state. init this transfer. A handle never comes back to
         this state. */
      mresult = multistate_init(data, &result);
      ভঙ্গ;

    পক্ষ MSTATE_SETUP:
      /* Transitional state. Setup things for a new transfer. The handle
         can come back to this state on a redirect. */
      mresult = multistate_setup(data);
      ভঙ্গ;

    পক্ষ MSTATE_CONNECT:
      mresult = multistate_connect(multi, data, &result);
      ভঙ্গ;

    পক্ষ MSTATE_CONNECTING:
      /* awaiting a completion of an asynch TCP connect */
      mresult = multistate_connecting(data, &stream_error, &result);
      ভঙ্গ;

    পক্ষ MSTATE_PROTOCONNECT:
      mresult = multistate_protoconnect(data, &stream_error, &result);
      ভঙ্গ;

    পক্ষ MSTATE_PROTOCONNECTING:
      /* protocol-specific connect phase */
      mresult = multistate_protoconnecting(data, &stream_error, &result);
      ভঙ্গ;

    পক্ষ MSTATE_DO:
      mresult = multistate_do(data, &stream_error, &result);
      ভঙ্গ;

    পক্ষ MSTATE_DOING:
      /* we continue DOING until the DO phase is complete */
      mresult = multistate_doing(data, &stream_error, &result);
      ভঙ্গ;

    পক্ষ MSTATE_DOING_MORE:
      /*
       * When we are connected, DOING MORE and then go DID
       */
      mresult = multistate_doing_more(data, &stream_error, &result);
      ভঙ্গ;

    পক্ষ MSTATE_DID:
      mresult = multistate_did(multi, data);
      ভঙ্গ;

    পক্ষ MSTATE_RATELIMITING: /* limit-rate exceeded in either direction */
      mresult = multistate_ratelimiting(data, &result);
      ভঙ্গ;

    পক্ষ MSTATE_PERFORMING:
      mresult = multistate_performing(data, &stream_error, &result);
      ভঙ্গ;

    পক্ষ MSTATE_DONE:
      mresult = multistate_done(data, &result);
      ভঙ্গ;

    পক্ষ MSTATE_COMPLETED:
      ভঙ্গ;

    পক্ষ MSTATE_PENDING:
    পক্ষ MSTATE_MSGSENT:
      /* handles in these states should NOT be in this list */
      ভঙ্গ;

    স্ৱভাৱ:
      mresult = CURLM_INTERNAL_ERROR;
      গচ্ছ out;
    }

    যদি(data->mstate >= MSTATE_CONNECT &&
       data->mstate < MSTATE_DO &&
       mresult != CURLM_CALL_MULTI_PERFORM &&
       !multi_ischanged(multi, FALSE)) {
      /* We now handle stream timeouts if and only if this will be the last
       * loop iteration. We only check this on the last iteration to ensure
       * that if we know we have additional work to do immediately
       * (i.e. CURLM_CALL_MULTI_PERFORM == TRUE) then we should do that before
       * declaring the connection timed out as we may almost have a completed
       * connection. */
      pnow = Curl_pgrs_now(data);
      multi_handle_timeout(data, pnow, &stream_error, &result);
    }

statemachine_end:
    যদি(!pnow)
      pnow = Curl_pgrs_now(data);
    result = is_finished(multi, data, pnow, stream_error, result);
    যদি(result)
      mresult = CURLM_CALL_MULTI_PERFORM;

    যদি(MSTATE_COMPLETED == data->mstate) {
      handle_completed(multi, data, result);
      mresult = CURLM_OK;
      গচ্ছ out;
    }
  } যেতিয়ালৈ((mresult == CURLM_CALL_MULTI_PERFORM) ||
          multi_ischanged(multi, FALSE));

out:
  data->result = result;
  ঘূৰাই mresult;
}

স্থির CURLMcode multi_perform(সংরচনা Curl_multi *multi,
                               পূর্ণাঙ্ক *running_handles)
{
  CURLMcode returncode = CURLM_OK;
  সংরচনা curltime start;
  uint32_t mid;
  সংরচনা Curl_sigpipe_ctx sigpipe_ctx;

  curlx_pnow(&start);
  sigpipe_init(&sigpipe_ctx);

  যদি(Curl_uint32_bset_first(&multi->process, &mid)) {
    CURL_TRC_M(multi->admin, "multi_perform(running=%u)",
               multi_xfers_running(multi));
    কৰা {
      সংরচনা Curl_easy *data = Curl_multi_get_easy(multi, mid);
      CURLMcode mresult;
      যদি(!data) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        অনুৱর্ত;
      }
      mresult = multi_runsingle(multi, data, &sigpipe_ctx);
      যদি(mresult)
        returncode = mresult;
    } যেতিয়ালৈ(Curl_uint32_bset_next(&multi->process, mid, &mid));
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
  যেতিয়ালৈ(Curl_timeouts_remove_expired(&multi->timeouts, &start, &mid)) {
    /* the removed may have another timeout in queue */
    সংরচনা Curl_easy *data = Curl_multi_get_easy(multi, mid);
    যদি(!data) {
      DEBUGASSERT(0);
      অনুৱর্ত;
    }
    (শূন্য)add_next_timeout(&start, multi, data);
    যদি(data->mstate == MSTATE_PENDING) {
      bool stream_unused;
      CURLcode result_unused;
      যদি(multi_handle_timeout(data, Curl_pgrs_now(data),
                              &stream_unused, &result_unused)) {
        infof(data, "PENDING handle timeout");
        move_pending_to_connect(multi, data);
      }
    }
  }

  যদি(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < INT_MAX) ? (পূর্ণাঙ্ক)running : INT_MAX;
  }

  যদি(CURLM_OK >= returncode)
    returncode = Curl_update_timer(multi);

  ঘূৰাই returncode;
}

CURLMcode curl_multi_perform(CURLM *m, পূর্ণাঙ্ক *running_handles)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_perform, &mresult)) {
    mresult = multi_perform(m, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

CURLMcode curl_multi_cleanup(CURLM *m)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_cleanup, &mresult)) {
    সংরচনা Curl_multi *multi = m;
    শূন্য *entry;
    uint32_t mid;

    /* First remove all remaining easy handles,
     * close internal ones. admin handle is special */
    যদি(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
      কৰা {
        সংরচনা Curl_easy *data = entry;
        যদি(!GOOD_EASY_HANDLE(data)) {
          mresult = CURLM_BAD_HANDLE;
          গচ্ছ out;
        }

#ifdef DEBUGBUILD
        যদি(mid != data->mid) {
          CURL_TRC_M(data, "multi_cleanup: still present with mid=%u, "
                     "but unexpected data->mid=%u\n", mid, data->mid);
          DEBUGASSERT(0);
        }
#endif

        যদি(data == multi->admin)
          অনুৱর্ত;

        যদি(!data->state.done && data->conn)
          /* if DONE was never called for this handle */
          (শূন্য)multi_done(data, CURLE_OK, TRUE);

        data->multi = রিক্ত; /* clear the association */
        Curl_uint32_tbl_remove(&multi->xfers, mid);
        data->mid = UINT32_MAX;

#ifdef USE_LIBPSL
        যদি(data->psl == &multi->psl)
          data->psl = রিক্ত;
#endif
        যদি(data->state.internal)
          Curl_close(&data);
      } যেতিয়ালৈ(Curl_uint32_tbl_next(&multi->xfers, mid, &mid, &entry));
    }

#ifdef USE_RESOLV_THREADED
    Curl_async_thrdd_multi_destroy(multi, !multi->quick_exit);
#endif
    Curl_cpool_destroy(&multi->cpool, multi->admin);
    Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
    যদি(multi->admin) {
      CURL_TRC_M(multi->admin, "multi_cleanup, closing admin handle, done");
      multi->admin->multi = রিক্ত;
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
    curlx_memzero(multi, পরিমাণ(*multi));
    curlx_free(multi);

    mresult = CURLM_OK;
  }
out:
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

/*
 * curl_multi_info_read()
 *
 * This function is the primary way for a multi/multi_socket application to
 * figure out if a transfer has ended.
 */

CURLMsg *curl_multi_info_read(CURLM *m, পূর্ণাঙ্ক *msgs_in_queue)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMsg *বাৰ্তা_result = রিক্ত;

  *msgs_in_queue = 0; /* default to none */
  যদি(CURL_MAPI_ENTER(&guard, m, multi_info_read, রিক্ত)) {
    সংরচনা Curl_multi *multi = m;
    uint32_t mid;
    যদি(Curl_uint32_bset_first(&multi->msgsent, &mid)) {
      সংরচনা Curl_easy *data = Curl_multi_get_easy(multi, mid);

      DEBUGASSERT(data);
      Curl_uint32_bset_remove(&multi->msgsent, mid);
      *msgs_in_queue =
        curlx_uztosi(Curl_uint32_bset_গুনতি(&multi->msgsent));
      যদি(data)
        বাৰ্তা_result = &data->বাৰ্তা;
    }
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই বাৰ্তা_result;
}

শূন্য Curl_multi_will_close(সংরচনা Curl_easy *data, curl_socket_t s)
{
  যদি(data) {
    সংরচনা Curl_multi *multi = data->multi;
    যদি(multi) {
      CURL_TRC_M(data, "Curl_multi_will_close fd=%" FMT_SOCKET_T, s);
      Curl_multi_ev_socket_done(multi, data, s);
    }
  }
}

স্থির শূন্য multi_timeouts_init(সংরচনা Curl_easy *data)
{
  data->state.timeouts.first = EXPIRE_LAST;
  data->state.timeouts.splaynode.registered = FALSE;
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
স্থির CURLMcode add_next_timeout(ধ্রুৱ সংরচনা curltime *pnow,
                                  সংরচনা Curl_multi *multi,
                                  সংরচনা Curl_easy *data)
{
  সংরচনা expire_timers *timeouts = &data->state.timeouts;
  timediff_t now_us = Curl_timeouts_offset_us(&multi->timeouts, pnow);

  যেতিয়ালৈ(timeouts->first < EXPIRE_LAST) {
    যদি(timeouts->offset_us[timeouts->first] <= now_us)  /* already expired */
      timeouts->first = timeouts->next[timeouts->first];
    অন্যথা /* timeouts are sorted, first is first in the future now */
      ভঙ্গ;
  }

  যদি(timeouts->first < EXPIRE_LAST) {
    /* Insert this node again into the splay. Keep the timer in the list in
       case we need to recompute future timers. */
    Curl_timeouts_add(&multi->timeouts, data,
                      timeouts->offset_us[timeouts->first]);
  }
  ঘূৰাই CURLM_OK;
}

স্থির শূন্য multi_mark_expired_as_dirty(সংরচনা Curl_multi *multi,
                                        ধ্রুৱ সংরচনা curltime *ts)
{
  সংরচনা Curl_easy *data = রিক্ত;
  uint32_t mid;

  /*
   * The loop following here will go on as long as there are expire-times left
   * to process (compared to `ts`) in the splay and 'data' will be
   * re-assigned for every expired handle we deal with.
   */
  যেতিয়ালৈ(Curl_timeouts_remove_expired(&multi->timeouts, ts, &mid)) {
    /* Check if there is one (more) expired timer to deal with! This function
       extracts a matching node if there is one */
    data = Curl_multi_get_easy(multi, mid);
    যদি(!data) {
      DEBUGASSERT(0);
      অনুৱর্ত;
    }
#ifdef CURLVERBOSE
    যদি(CURL_TRC_TIMER_is_verbose(data)) {
      যদি(data->state.timeouts.first < EXPIRE_LAST) {
        CURL_TRC_TIMER(data, data->state.timeouts.first, "has expired");
      }
    }
#endif
    (শূন্য)add_next_timeout(ts, multi, data);
    Curl_multi_mark_dirty(data);
  }
}

স্থির CURLMcode multi_run_dirty(সংরচনা Curl_multi *multi,
                                 সংরচনা Curl_sigpipe_ctx *sigpipe_ctx,
                                 uint32_t *pnum)
{
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;

  *pnum = 0;
  যদি(Curl_uint32_bset_first(&multi->dirty, &mid)) {
    কৰা {
      সংরচনা Curl_easy *data = Curl_multi_get_easy(multi, mid);
      যদি(data) {
        CURL_TRC_M(data, "multi_run_dirty");

        যদি(!Curl_uint32_bset_contains(&multi->process, mid)) {
          যদি(Curl_uint32_bset_contains(&multi->pending, mid)) {
            /* Something happened on the pending transfer, act on it. */
            move_pending_to_connect(multi, data);
          }
          অন্যথা {
            /* We are no longer processing this transfer */
            Curl_uint32_bset_remove(&multi->dirty, mid);
            অনুৱর্ত;
          }
        }

        (*pnum)++;
        /* runsingle() clears the dirty mid */
        mresult = multi_runsingle(multi, data, sigpipe_ctx);

        যদি(CURLM_OK >= mresult) {
          /* reassess event handling of data */
          mresult = Curl_multi_ev_assess_xfer(multi, data);
          যদি(mresult)
            গচ্ছ out;
        }
      }
      অন্যথা {
        CURL_TRC_M(multi->admin, "multi_run_dirty, %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } যেতিয়ালৈ(Curl_uint32_bset_next(&multi->dirty, mid, &mid));
  }

out:
  ঘূৰাই mresult;
}

স্থির CURLMcode multi_socket(সংরচনা Curl_multi *multi,
                              bool checkall,
                              curl_socket_t s,
                              পূর্ণাঙ্ক ev_bitmask,
                              পূর্ণাঙ্ক *running_handles)
{
  CURLMcode mresult = CURLM_OK;
  সংরচনা Curl_sigpipe_ctx pipe_ctx;
  সংরচনা curltime now;
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
    গচ্ছ out;
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
      গচ্ছ out;
  }

  curlx_pnow(&now);
  multi_mark_expired_as_dirty(multi, &now);
  mresult = multi_run_dirty(multi, &pipe_ctx, &run_xfers);
  যদি(mresult)
    গচ্ছ out;

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
    *running_handles = (running < INT_MAX) ? (পূর্ণাঙ্ক)running : INT_MAX;
  }

  যদি(CURLM_OK >= mresult)
    mresult = Curl_update_timer(multi);
  ঘূৰাই mresult;
}

#undef curl_multi_setopt
CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_setopt, &mresult)) {
    সংরচনা Curl_multi *multi = m;
    va_list param;
    unsigned long uarg;
    size_t szarg;

    va_start(param, option);

    ৱিকল্প(option) {
    পক্ষ CURLMOPT_SOCKETFUNCTION:
      multi->socket_cb = va_arg(param, curl_socket_callback);
      ভঙ্গ;
    পক্ষ CURLMOPT_SOCKETDATA:
      multi->socket_userp = va_arg(param, শূন্য *);
      ভঙ্গ;
    পক্ষ CURLMOPT_PUSHFUNCTION:
      multi->ঠেলো_cb = va_arg(param, curl_ঠেলো_callback);
      ভঙ্গ;
    পক্ষ CURLMOPT_PUSHDATA:
      multi->ঠেলো_userp = va_arg(param, শূন্য *);
      ভঙ্গ;
    পক্ষ CURLMOPT_PIPELINING:
      multi->multiplexing = va_arg(param, long) & CURLPIPE_MULTIPLEX ? 1 : 0;
      ভঙ্গ;
    পক্ষ CURLMOPT_TIMERFUNCTION:
      multi->timer_cb = va_arg(param, curl_multi_timer_callback);
      ভঙ্গ;
    পক্ষ CURLMOPT_TIMERDATA:
      multi->timer_userp = va_arg(param, শূন্য *);
      ভঙ্গ;
    পক্ষ CURLMOPT_MAXCONNECTS:
      uarg = va_arg(param, unsigned long);
      যদি(uarg <= UINT32_MAX)
        multi->maxconnects = (uint32_t)uarg;
      ভঙ্গ;
    পক্ষ CURLMOPT_MAX_HOST_CONNECTIONS:
      যদি(!curlx_sltouz(va_arg(param, long), &szarg))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      multi->max_host_connections = (szarg < UINT32_MAX) ?
                                    (uint32_t)szarg : UINT32_MAX;
      ভঙ্গ;
    পক্ষ CURLMOPT_MAX_TOTAL_CONNECTIONS:
      যদি(!curlx_sltouz(va_arg(param, long), &szarg))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      multi->max_total_connections = (szarg < UINT32_MAX) ?
                                     (uint32_t)szarg : UINT32_MAX;
      ভঙ্গ;
      /* options formerly used for pipelining */
    পক্ষ CURLMOPT_MAX_PIPELINE_LENGTH:
      ভঙ্গ;
    পক্ষ CURLMOPT_CONTENT_LENGTH_PENALTY_SIZE:
      ভঙ্গ;
    পক্ষ CURLMOPT_CHUNK_LENGTH_PENALTY_SIZE:
      ভঙ্গ;
    পক্ষ CURLMOPT_PIPELINING_SITE_BL:
      ভঙ্গ;
    পক্ষ CURLMOPT_PIPELINING_SERVER_BL:
      ভঙ্গ;
    পক্ষ CURLMOPT_MAX_CONCURRENT_STREAMS: {
      যদি(!curlx_sltouz(va_arg(param, long), &szarg) ||
         !szarg || (szarg > (size_t)INT_MAX)) /* preserve previous cutoff */
        multi->max_concurrent_streams = 100;
      অন্যথা
        multi->max_concurrent_streams = (szarg < UINT32_MAX) ?
                                       (uint32_t)szarg : UINT32_MAX;
      ভঙ্গ;
    }
    পক্ষ CURLMOPT_NETWORK_CHANGED: {
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
      ভঙ্গ;
    }
    পক্ষ CURLMOPT_NOTIFYFUNCTION:
      multi->ntfy.ntfy_cb = va_arg(param, curl_notify_callback);
      ভঙ্গ;
    পক্ষ CURLMOPT_NOTIFYDATA:
      multi->ntfy.ntfy_cb_data = va_arg(param, শূন্য *);
      ভঙ্গ;
    পক্ষ CURLMOPT_RESOLVE_THREADS_MAX:
#ifdef USE_RESOLV_THREADED
      uarg = va_arg(param, long);
      যদি((uarg <= 0) || (uarg > UINT32_MAX))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      অন্যথা {
        CURLcode result = Curl_async_thrdd_multi_set_props(
          multi, 0, (uint32_t)uarg, 2000);
        ৱিকল্প(result) {
        পক্ষ CURLE_OK:
          mresult = CURLM_OK;
          ভঙ্গ;
        পক্ষ CURLE_BAD_FUNCTION_ARGUMENT:
          mresult = CURLM_BAD_FUNCTION_ARGUMENT;
          ভঙ্গ;
        পক্ষ CURLE_OUT_OF_MEMORY:
          mresult = CURLM_OUT_OF_MEMORY;
          ভঙ্গ;
        স্ৱভাৱ:
          mresult = CURLM_INTERNAL_ERROR;
          ভঙ্গ;
        }
      }
#endif
      ভঙ্গ;
    পক্ষ CURLMOPT_QUICK_EXIT:
      multi->quick_exit = va_arg(param, long) ? 1 : 0;
      ভঙ্গ;
    স্ৱভাৱ:
      mresult = CURLM_UNKNOWN_OPTION;
      ভঙ্গ;
    }
    va_end(param);
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

/* we define curl_multi_socket() in the public multi.h header */
#undef curl_multi_socket

CURLMcode curl_multi_socket(CURLM *m, curl_socket_t s, পূর্ণাঙ্ক *running_handles)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_socket, &mresult)) {
    mresult = multi_socket(m, FALSE, s, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

CURLMcode curl_multi_socket_action(CURLM *m, curl_socket_t s,
                                   পূর্ণাঙ্ক ev_bitmask, পূর্ণাঙ্ক *running_handles)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_socket_action, &mresult)) {
    mresult = multi_socket(m, FALSE, s, ev_bitmask, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

CURLMcode curl_multi_socket_all(CURLM *m, পূর্ণাঙ্ক *running_handles)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_socket_all, &mresult)) {
    mresult = multi_socket(m, TRUE, CURL_SOCKET_BAD, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

স্থির bool multi_has_dirties(সংরচনা Curl_multi *multi)
{
  uint32_t mid;
  যদি(Curl_uint32_bset_first(&multi->dirty, &mid)) {
    কৰা {
      সংরচনা Curl_easy *data = Curl_multi_get_easy(multi, mid);
      যদি(data) {
        যদি(Curl_uint32_bset_contains(&multi->process, mid))
          ঘূৰাই TRUE;
        /* We are no longer processing this transfer */
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
      অন্যথা {
        CURL_TRC_M(multi->admin, "dirty transfer %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } যেতিয়ালৈ(Curl_uint32_bset_next(&multi->dirty, mid, &mid));
  }
  ঘূৰাই FALSE;
}

স্থির শূন্য multi_timeout(সংরচনা Curl_multi *multi,
                          timediff_t *pexire_offset_us,
                          পূর্ণাঙ্ক *timeout_ms)
{
  যদি(multi->dead) {
    যদি(pexire_offset_us)
      *pexire_offset_us = 0;
    *timeout_ms = 0;
    ঘূৰাই;
  }

  যদি(multi_has_dirties(multi)) {
    যদি(pexire_offset_us) {
      সংরচনা curltime now;
      curlx_pnow(&now);
      *pexire_offset_us = Curl_timeouts_offset_us(&multi->timeouts, &now);
    }
    *timeout_ms = 0;
    ঘূৰাই;
  }
  অন্যথা {
    সংরচনা curltime now;
    uint32_t mid;

    curlx_pnow(&now);
    *timeout_ms = Curl_timeouts_next_ms(&multi->timeouts, &now,
                                        pexire_offset_us, &mid);
#ifdef CURLVERBOSE
    যদি(mid != UINT32_MAX) {
      সংরচনা Curl_easy *data = Curl_multi_get_easy(multi, mid);
      যদি(data && CURL_TRC_TIMER_is_verbose(data) &&
         (data->state.timeouts.first < EXPIRE_LAST)) {
        CURL_TRC_TIMER(data, data->state.timeouts.first,
                       "gives multi timeout in %dms", *timeout_ms);
      }
    }
#endif
  }
}

CURLMcode curl_multi_timeout(CURLM *m,
                             long *timeout_ms)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_timeout, &mresult)) {
    পূর্ণাঙ্ক itimeout_ms;

    multi_timeout(m, রিক্ত, &itimeout_ms);
    *timeout_ms = (long)itimeout_ms;
    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

/*
 * Tell the application it should update its timers, if it subscribes to the
 * update timer callback.
 */
CURLMcode Curl_update_timer(সংরচনা Curl_multi *multi)
{
  timediff_t timeouts_offset_us = 0;
  পূর্ণাঙ্ক timeout_ms;
  পূর্ণাঙ্ক rc;
  bool set_মান = FALSE;

  যদি(!multi->timer_cb || multi->dead)
    ঘূৰাই CURLM_OK;
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
    সংরচনা Curl_mapi_guard guard;

    multi->last_expire_offset_us = timeouts_offset_us;
    multi->last_timeout_set = timeout_ms >= 0;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_timer_cb);
    rc = multi->timer_cb(multi, timeout_ms, multi->timer_userp);
    CURL_CBAPI_MULTI_END(&guard);
    যদি(rc == -1) {
      multi->dead = TRUE;
      ঘূৰাই CURLM_ABORTED_BY_CALLBACK;
    }
  }
  ঘূৰাই CURLM_OK;
}

#ifdef DEBUGBUILD
স্থির bool multi_timeouts_check(সংরচনা Curl_easy *data)
{
  সংরচনা expire_timers *timeouts = &data->state.timeouts;
  uint8_t id;
  পূর্ণাঙ্ক i = 0;
  বাবে(id = timeouts->first; id < EXPIRE_LAST; id = timeouts->next[id]) {
    যদি(++i >= EXPIRE_LAST) {
      failf(data, "expire timeouts looped: %d iterations and no end", i);
      ঘূৰাই FALSE;
    }
    যদি(id == timeouts->next[id]) {
      failf(data, "expire timeouts wrong: %d points to itself", (পূর্ণাঙ্ক)id);
      ঘূৰাই FALSE;
    }
    যদি((timeouts->next[id] < EXPIRE_LAST) &&
       (timeouts->offset_us[id] > timeouts->offset_us[timeouts->next[id]])) {
      failf(data, "expire timeouts not sorted: %d happens after %d but "
            "is listed before", (পূর্ণাঙ্ক)id, (পূর্ণাঙ্ক)timeouts->next[id]);
      ঘূৰাই FALSE;
    }
  }
  ঘূৰাই TRUE;
}
#endif

/*
 * Remove a given timestamp from the list of timeouts.
 */
স্থির শূন্য multi_clear_timeout(সংরচনা Curl_easy *data, expire_id eid)
{
  সংরচনা expire_timers *timeouts = &data->state.timeouts;
  uint8_t orig_first = timeouts->first;
  uint8_t *anchor = &timeouts->first;
  uint8_t id = (uint8_t)eid;

  যদি((unsigned)eid >= EXPIRE_LAST) {
    DEBUGASSERT(0);
    ঘূৰাই;
  }

  যেতিয়ালৈ(*anchor < EXPIRE_LAST) {
    যদি(*anchor == id) {
      *anchor = timeouts->next[id];
      ভঙ্গ;
    }
    anchor = &timeouts->next[*anchor];
  }
  DEBUGASSERT(multi_timeouts_check(data));
  যদি(Curl_timeouts_has(data)) {
    সংরচনা Curl_multi *multi = data->multi;

    যদি(!multi) {
      DEBUGASSERT(0);
      ঘূৰাই;
    }
    যদি((timeouts->first >= EXPIRE_LAST) || /* no more timeouts */
       (timeouts->first != orig_first)) {  /* active timeout changed */
      Curl_timeouts_remove(&multi->timeouts, data);
    }
    যদি((timeouts->first < EXPIRE_LAST) && !Curl_timeouts_has(data)) {
      Curl_timeouts_add(&multi->timeouts, data,
                        timeouts->offset_us[timeouts->first]);
    }
  }
}

/*
 * Add a timestamp to the list of timeouts. Keep the list sorted so that head
 * of list is always the timeout nearest in time.
 */
স্থির CURLMcode multi_set_timeout(সংরচনা Curl_easy *data,
                                   ধ্রুৱ সংরচনা curltime *stamp,
                                   expire_id eid)
{
  সংরচনা expire_timers *timeouts = &data->state.timeouts;
  uint8_t *anchor = &timeouts->first;
  uint8_t id = (uint8_t)eid;

  যদি((unsigned)eid >= EXPIRE_LAST) {
    DEBUGASSERT(0);
    ঘূৰাই CURLM_BAD_FUNCTION_ARGUMENT;
  }
  /* remove from list, store time and re-insert */
  multi_clear_timeout(data, eid);
  timeouts->offset_us[id] =
    Curl_timeouts_offset_us(&data->multi->timeouts, stamp);

  যেতিয়ালৈ(*anchor < EXPIRE_LAST) {
    যদি(timeouts->offset_us[*anchor] > timeouts->offset_us[id])
      ভঙ্গ;
    anchor = &timeouts->next[*anchor];
  }
  timeouts->next[eid] = *anchor;
  timeouts->next[eid] = *anchor;
  *anchor = id;
  DEBUGASSERT(multi_timeouts_check(data));
  CURL_TRC_TIMER(data, eid, "set for %" FMT_TIMEDIFF_T "us",
                 curlx_ptimediff_us(stamp, Curl_pgrs_now(data)));
  ঘূৰাই CURLM_OK;
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
শূন্য Curl_expire_set(সংরচনা Curl_easy *data,
                     expire_id eid, timediff_t ms,
                     ধ্রুৱ সংরচনা curltime *pnow)
{
  সংরচনা Curl_multi *multi = data->multi;
  সংরচনা expire_timers *timeouts = &data->state.timeouts;
  uint8_t prev_id = timeouts->first;
  সংরচনা curltime set;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  যদি(!multi)
    ঘূৰাই;
  DEBUGASSERT(eid < EXPIRE_LAST);
  যদি(ms > INT_MAX)
    /* Cap ridiculous timeouts, 31-bit ms is still 3.5 weeks. When the time
       goes to the user, it must fit in this size. */
    ms = INT_MAX;

  set = *pnow;
  set.tv_sec += (time_t)(ms / 1000); /* may be a 64 to 32-bit conversion */
  set.tv_usec += (পূর্ণাঙ্ক)(ms % 1000) * 1000;
  যদি(set.tv_usec >= 1000000) {
    set.tv_sec++;
    set.tv_usec -= 1000000;
  }

  /* Add the timeout, will replace any previous value for this timer. */
  multi_set_timeout(data, &set, eid);
  DEBUGASSERT(timeouts->first < EXPIRE_LAST);

  যদি(Curl_timeouts_has(data)) {
    /* data has already a timeout registered. If the first timer
     * was NOT the one we just set AND is still the first one,
     * nothing changed from the timeouts point of view. The
     * set timer triggers after the one already registered. Leave. */
    যদি((prev_id != eid) && (prev_id == timeouts->first))
      ঘূৰাই;

    /* Since this is an updated time, we must remove data from
     * timeouts and then add it again. */
    Curl_timeouts_remove(&multi->timeouts, data);
  }

  /* Insert the new timer expiry since it is our local minimum. */
  Curl_timeouts_add(&multi->timeouts, data,
                    timeouts->offset_us[timeouts->first]);
}

শূন্য Curl_expire(সংরচনা Curl_easy *data,
                 timediff_t milli, expire_id eid)
{
  Curl_expire_set(data, eid, milli, Curl_pgrs_now(data));
}

/*
 * Removes the expire timer. Marks it as done.
 */
শূন্য Curl_expire_clear(সংরচনা Curl_easy *data, expire_id eid)
{
  /* remove the timer, if there */
  multi_clear_timeout(data, eid);
  CURL_TRC_TIMER(data, eid, "cleared");
}

/*
 * Clear ALL timeout values for this handle.
 */
শূন্য Curl_expire_clear_all(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_multi *multi = data->multi;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  যদি(!multi)
    ঘূৰাই;

  যদি(Curl_timeouts_remove(&multi->timeouts, data)) {
    /* Since this is an cleared time, we must remove the previous entry from
       the splay tree */
    multi_timeouts_init(data);

    যদি(data->id >= 0)
      CURL_TRC_M(data, "[TIMEOUT] all cleared");
  }
}

CURLMcode curl_multi_assign(CURLM *m, curl_socket_t sockfd,
                            শূন্য *sockp)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_assign, &mresult)) {
    mresult = Curl_multi_ev_assign(m, sockfd, sockp);
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

স্থির শূন্য move_pending_to_connect(সংরচনা Curl_multi *multi,
                                    সংরচনা Curl_easy *data)
{
  DEBUGASSERT(data->mstate == MSTATE_PENDING);

  /* Remove this node from the pending set, add into process set */
  Curl_uint32_bset_remove(&multi->pending, data->mid);
  Curl_uint32_bset_add(&multi->process, data->mid);

  multistate(data, MSTATE_CONNECT);
  Curl_multi_mark_dirty(data); /* make it run */
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
স্থির শূন্য multi_schedule_pending(সংরচনা Curl_multi *multi)
{
  uint32_t mid = multi->last_pending_mid;

  যদি(mid) {
    যেতিয়ালৈ(Curl_uint32_bset_next(&multi->pending, mid, &mid)) {
      সংরচনা Curl_easy *data = Curl_multi_get_easy(multi, mid);
      যদি(data) {
        move_pending_to_connect(multi, data);
        multi->last_pending_mid = mid;
        ঘূৰাই;
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
    কৰা {
      সংরচনা Curl_easy *data = Curl_multi_get_easy(multi, mid);
      যদি(data) {
        move_pending_to_connect(multi, data);
        multi->last_pending_mid = mid;
        ঘূৰাই;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    } যেতিয়ালৈ(Curl_uint32_bset_next(&multi->pending, mid, &mid));
  }
}

uint32_t Curl_multi_max_concurrent_streams(সংরচনা Curl_multi *multi)
{
  DEBUGASSERT(multi);
  ঘূৰাই multi->max_concurrent_streams;
}

CURL **curl_multi_get_handles(CURLM *m)
{
  সংরচনা Curl_mapi_guard guard;
  CURL **a = রিক্ত;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_get_handles, রিক্ত)) {
    সংরচনা Curl_multi *multi = m;
    শূন্য *entry;
    size_t গুনতি = Curl_uint32_tbl_গুনতি(&multi->xfers);

    a = curlx_malloc(পরিমাণ(সংরচনা Curl_easy *) * (গুনতি + 1));
    যদি(a) {
      unsigned পূর্ণাঙ্ক i = 0;
      uint32_t mid;

      যদি(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
        কৰা {
          সংরচনা Curl_easy *data = entry;
          DEBUGASSERT(i < গুনতি);
          যদি(!data->state.internal)
            a[i++] = data;
        } যেতিয়ালৈ(Curl_uint32_tbl_next(&multi->xfers, mid, &mid, &entry));
      }
      a[i] = রিক্ত; /* last entry is a NULL */
    }
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই a;
}

CURLMcode curl_multi_get_offt(CURLM *m,
                              CURLMinfo_offt info,
                              curl_off_t *pvalue)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_get_offt, &mresult)) {
    সংরচনা Curl_multi *multi = m;
    uint32_t n;

    যদি(!pvalue) {
      mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      গচ্ছ out;
    }

    ৱিকল্প(info) {
    পক্ষ CURLMINFO_XFERS_CURRENT:
      n = Curl_uint32_tbl_গুনতি(&multi->xfers);
      যদি(n && multi->admin)
        --n;
      *pvalue = (curl_off_t)n;
      ভঙ্গ;
    পক্ষ CURLMINFO_XFERS_RUNNING:
      n = Curl_uint32_bset_গুনতি(&multi->process);
      যদি(n && Curl_uint32_bset_contains(&multi->process, multi->admin->mid))
        --n;
      *pvalue = (curl_off_t)n;
      ভঙ্গ;
    পক্ষ CURLMINFO_XFERS_PENDING:
      *pvalue = (curl_off_t)Curl_uint32_bset_গুনতি(&multi->pending);
      ভঙ্গ;
    পক্ষ CURLMINFO_XFERS_DONE:
      *pvalue = (curl_off_t)Curl_uint32_bset_গুনতি(&multi->msgsent);
      ভঙ্গ;
    পক্ষ CURLMINFO_XFERS_ADDED:
      *pvalue = multi->xfers_total_ever;
      ভঙ্গ;
    স্ৱভাৱ:
      *pvalue = -1;
      mresult = CURLM_UNKNOWN_OPTION;
      ভঙ্গ;
    }
  }
out:
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

স্থির সংরচনা Curl_fixed_buf *fixed_buf_create(size_t len)
{
  সংরচনা Curl_fixed_buf *fbuf;
  যদি((SIZE_MAX - পরিমাণ(*fbuf)) < len)
    ঘূৰাই রিক্ত; /* too large */
  fbuf = curlx_malloc(len + পরিমাণ(*fbuf));
  যদি(fbuf)
    fbuf->len = len;
  ঘূৰাই fbuf;
}

CURLcode Curl_multi_xfer_buf_borrow(সংরচনা Curl_easy *data,
                                    ৱর্ণ **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  *pbuf = রিক্ত;
  *pbuflen = 0;
  যদি(!data->multi) {
    failf(data, "transfer has no multi handle");
    ঘূৰাই CURLE_FAILED_INIT;
  }
  যদি(!data->set.buffer_size) {
    failf(data, "transfer buffer size is 0");
    ঘূৰাই CURLE_FAILED_INIT;
  }
  যদি(data->multi->xfer_buf_borrowed) {
    failf(data, "attempt to borrow xfer_buf when already borrowed");
    ঘূৰাই CURLE_AGAIN;
  }

  যদি(data->multi->xfer_buf &&
     data->set.buffer_size > data->multi->xfer_buf->len) {
    /* not large enough, get a new one */
    curlx_safefree(data->multi->xfer_buf);
  }

  যদি(!data->multi->xfer_buf) {
    data->multi->xfer_buf =
      fixed_buf_create(curlx_uitouz(data->set.buffer_size));
    যদি(!data->multi->xfer_buf) {
      failf(data, "could not allocate xfer_buf of %u bytes",
            data->set.buffer_size);
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    }
  }

  data->multi->xfer_buf_borrowed = TRUE;
  *pbuf = data->multi->xfer_buf->data;
  *pbuflen = data->multi->xfer_buf->len;
  ঘূৰাই CURLE_OK;
}

শূন্য Curl_multi_xfer_buf_release(সংরচনা Curl_easy *data, ৱর্ণ *buf)
{
  (শূন্য)buf;
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  DEBUGASSERT(!buf || (data->multi->xfer_buf &&
                       data->multi->xfer_buf->data == buf));
  data->multi->xfer_buf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_ulbuf_borrow(সংরচনা Curl_easy *data,
                                      ৱর্ণ **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  *pbuf = রিক্ত;
  *pbuflen = 0;
  যদি(!data->multi) {
    failf(data, "transfer has no multi handle");
    ঘূৰাই CURLE_FAILED_INIT;
  }
  যদি(data->multi->xfer_ulbuf_borrowed) {
    failf(data, "attempt to borrow xfer_ulbuf when already borrowed");
    ঘূৰাই CURLE_AGAIN;
  }

  যদি(data->multi->xfer_ulbuf &&
     data->set.upload_buffer_size > data->multi->xfer_ulbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(data->multi->xfer_ulbuf);
  }

  যদি(!data->multi->xfer_ulbuf) {
    data->multi->xfer_ulbuf =
      fixed_buf_create(curlx_uitouz(data->set.upload_buffer_size));
    যদি(!data->multi->xfer_ulbuf) {
      failf(data, "could not allocate xfer_ulbuf of %u bytes",
            data->set.upload_buffer_size);
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    }
  }

  data->multi->xfer_ulbuf_borrowed = TRUE;
  *pbuf = data->multi->xfer_ulbuf->data;
  *pbuflen = data->multi->xfer_ulbuf->len;
  ঘূৰাই CURLE_OK;
}

শূন্য Curl_multi_xfer_ulbuf_release(সংরচনা Curl_easy *data, ৱর্ণ *buf)
{
  (শূন্য)buf;
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  DEBUGASSERT(!buf || (data->multi->xfer_ulbuf &&
                       data->multi->xfer_ulbuf->data == buf));
  data->multi->xfer_ulbuf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_sockbuf_borrow(সংরচনা Curl_easy *data,
                                        size_t blen, ৱর্ণ **pbuf)
{
  DEBUGASSERT(data);
  *pbuf = রিক্ত;
  যদি(!data->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    *pbuf = curlx_malloc(blen);
    ঘূৰাই *pbuf ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  যদি(data->multi->xfer_sockbuf_borrowed) {
    failf(data, "attempt to borrow xfer_sockbuf when already borrowed");
    ঘূৰাই CURLE_AGAIN;
  }

  যদি(data->multi->xfer_sockbuf && blen > data->multi->xfer_sockbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(data->multi->xfer_sockbuf);
  }

  যদি(!data->multi->xfer_sockbuf) {
    data->multi->xfer_sockbuf = fixed_buf_create(blen);
    যদি(!data->multi->xfer_sockbuf) {
      failf(data, "could not allocate xfer_sockbuf of %zu bytes", blen);
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    }
  }

  data->multi->xfer_sockbuf_borrowed = TRUE;
  *pbuf = data->multi->xfer_sockbuf->data;
  ঘূৰাই CURLE_OK;
}

শূন্য Curl_multi_xfer_sockbuf_release(সংরচনা Curl_easy *data, ৱর্ণ *buf)
{
  DEBUGASSERT(data);
  যদি(!data->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    curlx_free(buf);
  }
  অন্যথা {
    DEBUGASSERT(!buf || (data->multi->xfer_sockbuf &&
                         data->multi->xfer_sockbuf->data == buf));
    data->multi->xfer_sockbuf_borrowed = FALSE;
  }
}

স্থির শূন্য multi_xfer_bufs_free(সংরচনা Curl_multi *multi)
{
  DEBUGASSERT(multi);
  curlx_safefree(multi->xfer_buf);
  multi->xfer_buf_borrowed = FALSE;
  curlx_safefree(multi->xfer_ulbuf);
  multi->xfer_ulbuf_borrowed = FALSE;
  curlx_safefree(multi->xfer_sockbuf);
  multi->xfer_sockbuf_borrowed = FALSE;
}

সংরচনা Curl_easy *Curl_multi_get_easy(সংরচনা Curl_multi *multi,
                                      uint32_t mid)
{
  সংরচনা Curl_easy *data = Curl_uint32_tbl_get(&multi->xfers, mid);
  যদি(GOOD_EASY_HANDLE(data))
    ঘূৰাই data;
  CURL_TRC_M(multi->admin, "invalid easy handle in xfer table for mid=%u",
             mid);
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  ঘূৰাই রিক্ত;
}

bool Curl_multi_knows_easy(সংরচনা Curl_multi *multi, সংরচনা Curl_easy *data)
{
  ঘূৰাই Curl_uint32_tbl_get(&multi->xfers, data->mid) == data;
}

স্থির uint32_t multi_xfers_running(সংরচনা Curl_multi *multi)
{
  যদি(!multi) {
    DEBUGASSERT(0);
    ঘূৰাই 0;
  }
  ঘূৰাই multi->xfers_alive;
}

uint32_t Curl_multi_xfers_attached(সংরচনা Curl_multi *multi)
{
  যদি(!multi || !multi->admin) {
    DEBUGASSERT(0);
    ঘূৰাই 0;
  }
  /* Discount the admin handle */
  ঘূৰাই Curl_uint32_tbl_গুনতি(&multi->xfers) - 1;
}

শূন্য Curl_multi_mark_dirty(সংরচনা Curl_easy *data)
{
  যদি(data->multi && data->mid != UINT32_MAX)
    Curl_uint32_bset_add(&data->multi->dirty, data->mid);
}

শূন্য Curl_multi_clear_dirty(সংরচনা Curl_easy *data)
{
  যদি(data->multi && data->mid != UINT32_MAX)
    Curl_uint32_bset_remove(&data->multi->dirty, data->mid);
}

CURLMcode curl_multi_notify_enable(CURLM *m, unsigned পূর্ণাঙ্ক notification)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_notify_enable, &mresult)) {
    mresult = Curl_mntfy_enable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

CURLMcode curl_multi_notify_disable(CURLM *m, unsigned পূর্ণাঙ্ক notification)
{
  সংরচনা Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  যদি(CURL_MAPI_ENTER(&guard, m, multi_notify_disable, &mresult)) {
    mresult = Curl_mntfy_disable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  ঘূৰাই mresult;
}

#ifdef DEBUGBUILD
স্থির শূন্য multi_xfer_dump(সংরচনা Curl_multi *multi, uint32_t mid,
                            শূন্য *entry)
{
  সংরচনা Curl_easy *data = entry;

  (শূন্য)multi;
  যদি(!data) {
    curl_mfprintf(stderr, "mid=%u, entry=NULL, bug in xfer table?\n", mid);
  }
  অন্যথা {
    curl_mfprintf(stderr, "mid=%u, magic=%s, p=%p, id=%" FMT_OFF_T
                  ", url=%s\n",
                  mid,
                  (data->magic == CURLEASY_MAGIC_সংখ্যা) ? "GOOD" : "BAD!",
                  (শূন্য *)data, data->id, Curl_bufref_ptr(&data->state.url));
  }
}

স্থির শূন্য multi_xfer_tbl_dump(সংরচনা Curl_multi *multi)
{
  uint32_t mid;
  শূন্য *entry;
  curl_mfprintf(stderr, "=== multi xfer table (count=%u, capacity=%u\n",
                Curl_uint32_tbl_গুনতি(&multi->xfers),
                Curl_uint32_tbl_capacity(&multi->xfers));
  যদি(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
    multi_xfer_dump(multi, mid, entry);
    যেতিয়ালৈ(Curl_uint32_tbl_next(&multi->xfers, mid, &mid, &entry))
      multi_xfer_dump(multi, mid, entry);
  }
  curl_mfprintf(stderr, "===\n");
  fflush(stderr);
}
#endif /* DEBUGBUILD */
