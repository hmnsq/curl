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

#ضمن "urldata.h"
#ضمن "transfer.h"
#ضمن "url.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "progress.h"
#ضمن "curl_share.h"
#ضمن "psl.h"
#ضمن "multiif.h"
#ضمن "multi_ev.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "http.h"
#ضمن "select.h"
#ضمن "curlx/wait.h"
#ضمن "conncache.h"
#ضمن "multihandle.h"
#ضمن "sigpipe.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "http_proxy.h"
#ضمن "http2.h"
#ضمن "socketpair.h"
#ضمن "bufref.h"
#ضمن "curlx/strparse.h"

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

ساكن فراغ move_pending_to_connect(بنية Curl_multi *multi,
                                    بنية Curl_easy *data);
ساكن CURLMcode add_next_timeout(ثابت بنية curltime *pnow,
                                  بنية Curl_multi *multi,
                                  بنية Curl_easy *data);
ساكن فراغ multi_timeout(بنية Curl_multi *multi,
                          timediff_t *pexire_offset_us,
                          صحيح *timeout_ms);
ساكن فراغ multi_schedule_pending(بنية Curl_multi *multi);
ساكن فراغ multi_xfer_bufs_free(بنية Curl_multi *multi);
#ifdef DEBUGBUILD
ساكن فراغ multi_xfer_tbl_dump(بنية Curl_multi *multi);
#endif
/* Get the # of transfers current in process/pending. */
ساكن uint32_t multi_xfers_running(بنية Curl_multi *multi);

/* function pointer called once when entering a state */
تعريفنوع فراغ (*mstate_enter_func)(بنية Curl_easy *data,
                                  CURLMstate from_state);

ساكن فراغ mstate_enter_connect(بنية Curl_easy *data,
                                 CURLMstate from_state)
{
  (فراغ)from_state;
  Curl_init_CONNECT(data);
}

ساكن فراغ mstate_enter_did(بنية Curl_easy *data,
                             CURLMstate from_state)
{
  (فراغ)from_state;
  data->req.chunk = FALSE;
  Curl_pgrsTime(data, TIMER_PRETRANSFER);
  إذا(!CURL_REQ_WANT_SEND(data))
    Curl_pgrsTime(data, TIMER_POSTRANSFER);
}

ساكن فراغ mstate_enter_done(بنية Curl_easy *data,
                              CURLMstate from_state)
{
  (فراغ)from_state;
  CURLM_NTFY(data, CURLMNOTIFY_EASY_DONE);
}

ساكن فراغ mstate_enter_completed(بنية Curl_easy *data,
                                   CURLMstate from_state)
{
  /* we sometimes directly jump to COMPLETED, trigger things
   * we then missed. */
  إذا(from_state < MSTATE_DID) {
    Curl_pgrsTime(data, TIMER_PRETRANSFER);
    Curl_pgrsTime(data, TIMER_POSTRANSFER);
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
  }
  /* multi_done() should have run and detached any connection already */
  DEBUGASSERT(!data->conn);
  Curl_pgrsCompleted(data);
  إذا(from_state < MSTATE_DONE)
    CURLM_NTFY(data, CURLMNOTIFY_EASY_DONE);
  /* changing to COMPLETED means it is in process and needs to go */
  DEBUGASSERT(Curl_uint32_bset_contains(&data->multi->process, data->mid));
  Curl_uint32_bset_remove(&data->multi->process, data->mid);
  Curl_uint32_bset_remove(&data->multi->pending, data->mid); /* to be sure */

  إذا(Curl_uint32_bset_empty(&data->multi->process)) {
    /* free the transfer buffer when we have no more active transfers */
    multi_xfer_bufs_free(data->multi);
  }
  /* Important: reset the conn pointer so that we do not point to memory
     that could be freed anytime */
  Curl_detach_connection(data);
  Curl_expire_clear_all(data); /* stop all timers */
}

/* always use this function to change state, to make debugging easier */
ساكن فراغ mstate(بنية Curl_easy *data, CURLMstate state
#ifdef DEBUGBUILD
                   , صحيح lineno
#endif
)
{
  CURLMstate oldstate = data->mstate;
  ساكن ثابت mstate_enter_func state_enter[MSTATE_LAST] = {
    عدم,                      /* INIT */
    عدم,                      /* PENDING */
    عدم,                      /* SETUP */
    mstate_enter_connect,      /* CONNECT */
    عدم,                      /* CONNECTING */
    عدم,                      /* PROTOCONNECT */
    عدم,                      /* PROTOCONNECTING */
    عدم,                      /* DO */
    عدم,                      /* DOING */
    عدم,                      /* DOING_MORE */
    mstate_enter_did,          /* DID */
    عدم,                      /* PERFORMING */
    عدم,                      /* RATELIMITING */
    mstate_enter_done,         /* DONE */
    mstate_enter_completed,    /* COMPLETED */
    عدم                       /* MSGSENT */
  };

  إذا(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    أرجع;

#ifdef DEBUGBUILD
  NOVERBOSE((فراغ)lineno);
  CURL_TRC_M(data, "-> [%s] (line %d)", CURL_MSTATE_NAME(state), lineno);
#وإلا
  CURL_TRC_M(data, "-> [%s]", CURL_MSTATE_NAME(state));
#endif

  /* really switching state */
  data->mstate = state;
  إذا(state_enter[state])
    state_enter[state](data, oldstate);
}

#ifndef DEBUGBUILD
#define multistate(x, y) mstate(x, y)
#وإلا
#define multistate(x, y) mstate(x, y, __LINE__)
#endif

/* multi->proto_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
ساكن فراغ ph_freeentry(فراغ *p)
{
  (فراغ)p;
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
ساكن فراغ multi_addmsg(بنية Curl_multi *multi, بنية Curl_easy *data)
{
  إذا(Curl_uint32_bset_empty(&multi->msgsent))
    CURLM_NTFY(multi->admin, CURLMNOTIFY_INFO_READ);
  Curl_uint32_bset_add(&multi->msgsent, data->mid);
}

ساكن فراغ multi_timeouts_init(بنية Curl_easy *data);

بنية Curl_multi *Curl_multi_handle(uint32_t xfer_table_size,
                                     size_t ev_hashsize,  /* event hash */
                                     size_t chashsize, /* connection hash */
                                     size_t dnssize,   /* dns hash */
                                     size_t sesssize)  /* TLS session cache */
{
  بنية curltime created;
  بنية Curl_multi *multi = curlx_calloc(1, حجم(بنية Curl_multi));

  إذا(!multi)
    أرجع عدم;

  multi->magic = CURLMULTI_MAGIC_نومبور;

  /* Initialisation order is important here!
   * easy_init() does a lazy check on curl_global_init() which sets
   * up platform specific things we need. For example calling curlx_pnow()
   * before this is not safe. */
  multi->admin = curl_easy_init();
  إذا(!multi->admin) {
    curlx_free(multi);
    أرجع عدم;
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

  إذا(Curl_uint32_bset_resize(&multi->process, xfer_table_size) ||
     Curl_uint32_bset_resize(&multi->pending, xfer_table_size) ||
     Curl_uint32_bset_resize(&multi->dirty, xfer_table_size) ||
     Curl_uint32_bset_resize(&multi->msgsent, xfer_table_size) ||
     Curl_uint32_tbl_resize(&multi->xfers, xfer_table_size))
    اذهب error;

#ifdef DEBUGBUILD
  {
    ثابت حرف *p;
    إذا(getenv("CURL_DEBUG"))
      multi->admin->set.verbose = TRUE;
    p = getenv("CURL_DBG_MAX_TOTAL_CONNECTIONS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نومبور(&p, &l, UINT32_MAX)) {
        multi->max_total_connections = (uint32_t)l;
      }
    }
  }
#endif
  Curl_uint32_tbl_add(&multi->xfers, multi->admin, &multi->admin->mid);
  Curl_uint32_bset_add(&multi->process, multi->admin->mid);

  Curl_cpool_init(&multi->cpool, عدم, chashsize);

#ifdef USE_SSL
  إذا(Curl_ssl_scache_create(sesssize, 2, &multi->ssl_scache))
    اذهب error;
#وإلا
  (فراغ)sesssize;
#endif

#ifdef USE_WINSOCK
  multi->wsa_event = WSACreateEvent();
  إذا(multi->wsa_event == WSA_INVALID_EVENT)
    اذهب error;
#endif
#ifdef ENABLE_WAKEUP
  /* When enabled, rely on this to work. We ignore this in previous
   * versions, but that seems an unnecessary complication. */
  إذا(Curl_wakeup_init(multi->wakeup_pair, TRUE) < 0)
    اذهب error;
#endif
#ifdef ENABLE_INTERNAL_WAKEUP
  إذا(Curl_wakeup_init(multi->wakeup_internal, TRUE) < 0)
    اذهب error;
#endif

  إذا(Curl_probeipv6(multi))
    اذهب error;

#ifdef USE_RESOLV_THREADED
  إذا(xfer_table_size < CURL_XFER_TABLE_SIZE) { /* easy multi */
    إذا(Curl_async_thrdd_multi_init(multi, 0, 2, 10))
      اذهب error;
  }
  وإلا { /* real multi handle */
    إذا(Curl_async_thrdd_multi_init(multi, 0, 20, 2000))
      اذهب error;
  }
#endif

  أرجع multi;

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
  إذا(multi->admin) {
    Curl_multi_ev_xfer_done(multi, multi->admin);
    multi->admin->multi = عدم;
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
  أرجع عدم;
}

CURLM *curl_multi_init(فراغ)
{
  أرجع Curl_multi_handle(CURL_XFER_TABLE_SIZE,
                           CURL_SOCKET_HASH_TABLE_SIZE,
                           CURL_CONNECTION_HASH_SIZE,
                           CURL_DNS_HASH_SIZE,
                           CURL_TLS_SESSION_SIZE);
}

#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
ساكن فراغ multi_warn_debug(بنية Curl_multi *multi, بنية Curl_easy *data)
{
  إذا(!multi->warned) {
    infof(data, "!!! WARNING !!!");
    infof(data, "This is a debug build of libcurl, "
                "do not use in production.");
    multi->warned = TRUE;
  }
}
#وإلا
#define multi_warn_debug(x, y) Curl_nop_stmt
#endif

bool Curl_is_connecting(بنية Curl_easy *data)
{
  أرجع data->mstate < MSTATE_DO;
}

ساكن CURLMcode multi_assess_wakeup(بنية Curl_multi *multi)
{
#ifdef ENABLE_INTERNAL_WAKEUP
  إذا(multi->socket_cb)
    أرجع Curl_multi_ev_assess_xfer(multi, multi->admin);
#وإلا
  (فراغ)multi;
#endif
  أرجع CURLM_OK;
}

ساكن CURLMcode multi_xfers_add(بنية Curl_multi *multi,
                                 بنية Curl_easy *data)
{
  uint32_t capacity = Curl_uint32_tbl_capacity(&multi->xfers);
  uint32_t new_size = 0;
  /* Prepare to make this into a CURLMOPT_MAX_TRANSFERS, because some
   * applications may want to prevent a run-away of their memory use. */
  /* UINT_MAX is our "invalid" id, do not let the table grow up to that. */
  ثابت uint32_t max_capacity = UINT_MAX - 1;

  إذا(capacity < max_capacity) {
    /* We want `multi->xfers` to have "sufficient" free rows, so that we do
     * not have to reuse the `mid` from a removed easy right away.
     * Check if an 8th of the capacity is still free */
    uint32_t used = Curl_uint32_tbl_کيراءن(&multi->xfers);
    uint32_t unused = capacity - used;
    uint32_t min_unused = CURLMAX(capacity >> 3, 4);
    إذا(unused < min_unused) {
      /* Grow by 50% of current capacity, in range of [128, 2048],
       * which means the table grows max by 16kb on 64-bit arch. */
      uint32_t growth = CURLMIN(CURLMAX(capacity >> 1, 128), 2048);
      /* Make sure the uint arithmetic here works on the corner
       * cases where we are close to max_capacity or UINT_MAX */
      إذا((max_capacity - growth) <= capacity)
        new_size = max_capacity;
      وإلا
        new_size = capacity + growth;
    }
  }

  إذا(new_size > capacity) {
    /* Grow the bitsets first. Should one fail, we do not need
     * to downsize the already resized ones. The sets continue
     * to work properly when larger than the table, but not
     * the other way around. */
    CURL_TRC_M(data, "increasing xfer table size to %u", new_size);
    إذا(Curl_uint32_bset_resize(&multi->process, new_size) ||
       Curl_uint32_bset_resize(&multi->dirty, new_size) ||
       Curl_uint32_bset_resize(&multi->pending, new_size) ||
       Curl_uint32_bset_resize(&multi->msgsent, new_size) ||
       Curl_uint32_tbl_resize(&multi->xfers, new_size))
      أرجع CURLM_OUT_OF_MEMORY;
  }

  /* Insert the easy into the table now */
  إذا(!Curl_uint32_tbl_add(&multi->xfers, data, &data->mid)) {
    /* MUST only happen when table is full */
    DEBUGASSERT(Curl_uint32_tbl_capacity(&multi->xfers) <=
                Curl_uint32_tbl_کيراءن(&multi->xfers));
    أرجع CURLM_OUT_OF_MEMORY;
  }
  أرجع CURLM_OK;
}

CURLMcode Curl_multi_add_handle(بنية Curl_multi *multi,
                                بنية Curl_easy *data)
{
  CURLMcode mresult;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  إذا(data->multi)
    أرجع CURLM_ADDED_ALREADY;

  إذا(multi->dead) {
    /* a "dead" handle cannot get added transfers while any existing easy
       handles are still alive - but if there are none alive anymore, it is
       fine to start over and unmark the "deadness" of this handle.
       This means only the admin handle MUST be present. */
    إذا((Curl_uint32_tbl_کيراءن(&multi->xfers) != 1) ||
       !Curl_uint32_tbl_contains(&multi->xfers, 0))
      أرجع CURLM_ABORTED_BY_CALLBACK;
    multi->dead = FALSE;
    Curl_uint32_bset_clear(&multi->process);
    Curl_uint32_bset_clear(&multi->dirty);
    Curl_uint32_bset_clear(&multi->pending);
    Curl_uint32_bset_clear(&multi->msgsent);
  }

  إذا(data->multi_easy) {
    /* if this easy handle was previously used for curl_easy_perform(), there
       is a private multi handle here that we can kill */
    curl_multi_cleanup(data->multi_easy);
    data->multi_easy = عدم;
  }

  /* Insert the easy into the multi->xfers table, assigning it a `mid`. */
  إذا(multi_xfers_add(multi, data))
    أرجع CURLM_OUT_OF_MEMORY;

  /* Initialize timeouts for this handle */
  multi_timeouts_init(data);

  /*
   * No failure allowed in this function beyond this point. No modification
   * of easy nor multi handle allowed before this except for potential
   * multi's connection pool growing which will not be undone in this
   * function no matter what.
   */
  إذا(data->set.errorbuffer)
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
  إذا(data->share && (data->share->specifier & (1 << CURL_LOCK_DATA_PSL)))
    data->psl = &data->share->psl;
  وإلا
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
  إذا(mresult) {
    /* undo the additions above */
    Curl_uint32_bset_remove(&multi->process, data->mid);
    Curl_uint32_bset_remove(&multi->dirty, data->mid);
    --multi->xfers_alive;
    data->multi = عدم; /* not anymore */
    Curl_uint32_tbl_remove(&multi->xfers, data->mid);
    data->mid = UINT32_MAX;
    أرجع mresult;
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
             Curl_uint32_tbl_کيراءن(&multi->xfers));
  أرجع CURLM_OK;
}

CURLMcode curl_multi_add_handle(CURLM *m, CURL *curl)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_add_handle, &mresult)) {
    بنية Curl_easy *data = curl;
    /* Verify that we got a somewhat good easy handle too */
    إذا(!GOOD_EASY_HANDLE(data) || data->state.internal)
      mresult = CURLM_BAD_EASY_HANDLE;
    وإلا
      mresult = Curl_multi_add_handle(m, data);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

بنية multi_done_ctx {
  BIT(premature);
};

ساكن bool multi_conn_should_close(بنية connectdata *conn,
                                    بنية Curl_easy *data,
                                    bool premature)
{
  /* if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of everything else. */
  إذا(conn->bits.close)
    أرجع TRUE;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM/NEGOTIATE authentication handshake. */
  إذا(data->set.reuse_forbid
#ifdef USE_NTLM
     && !(conn->http_ntlm_state == NTLMSTATE_TYPE2 ||
          conn->proxy_ntlm_state == NTLMSTATE_TYPE2)
#endif
#ifdef USE_SPNEGO
     && !(conn->http_negotiate_state == CURL_NEGO_AUTH_RECV ||
          conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)
#endif
    )
    أرجع TRUE;

  /* Unless this connection is for a "connect-only" transfer, it
   * needs to be closed if the protocol handler does not support reuse. */
  إذا(!data->set.connect_only && conn->scheme &&
     !(conn->scheme->flags & PROTOPT_CONN_REUSE))
    أرجع TRUE;

  /* if premature is TRUE, it means this connection was said to be DONE before
     the entire request operation is complete and thus we cannot know in what
     state it is for reusing, so we are forced to close it. In a perfect world
     we can add code that keep track of if we really must close it here or not,
     but currently we have no such detail knowledge. */
  إذا(premature && !Curl_conn_is_multiplex(conn, FIRSTSOCKET))
    أرجع TRUE;

  أرجع FALSE;
}

ساكن cpool_do_result multi_done_locked(بنية Curl_easy *data,
                                         بنية connectdata *conn,
                                         فراغ *userdata,
                                         ثابت بنية curltime *pnow)
{
  بنية multi_done_ctx *mdctx = userdata;

  (فراغ)pnow;
  CURL_TRC_M(data, "multi_done_locked, in use=%u", conn->attached_xfers);
  إذا(CONN_INUSE(conn)) {
    /* Stop if still used. */
    CURL_TRC_M(data, "Connection still in use %u, no more multi_done now!",
               conn->attached_xfers);
    أرجع CPOOL_DO_KEEP;
  }

  إذا(multi_conn_should_close(conn, data, (bool)mdctx->premature)) {
    CURL_TRC_M(data, "multi_done, terminating conn #%" FMT_OFF_T " to %s:%u, "
               "forbid=%d, close=%d, premature=%d, conn_multiplex=%d",
               conn->connection_id, conn->origin->user_hostname,
               conn->origin->port,
               data->set.reuse_forbid, conn->bits.close, mdctx->premature,
               Curl_conn_is_multiplex(conn, FIRSTSOCKET));
    connclose(conn);
    أرجع mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  وإلا إذا(!Curl_conn_get_max_concurrent(data, conn, FIRSTSOCKET)) {
    CURL_TRC_M(data, "multi_done, conn #%" FMT_OFF_T " to %s:%u was shutdown"
               " by server, not reusing", conn->connection_id,
               conn->origin->user_hostname, conn->origin->port);
    connclose(conn);
    أرجع mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  وإلا {
    أرجع CPOOL_DO_IDLE;
  }
}

ساكن CURLcode multi_done(بنية Curl_easy *data,
                           CURLcode status,  /* an error if this is called
                                                after an error was detected */
                           bool premature)
{
  CURLcode result;
  بنية connectdata *conn = data->conn;
  ثابت بنية curltime *pnow = Curl_pgrs_now(data);

  CURL_TRC_M(data, "multi_done: status: %d prem: %d done: %d",
             (صحيح)status, (صحيح)premature, data->state.done);

  إذا(data->state.done) {
    /* Stop if multi_done() has already been called */
    أرجع CURLE_OK;
  }
  data->state.done = TRUE; /* called now! */

  /* Shut down any ongoing async resolver operation. */
  Curl_resolv_shutdown_all(data);

  /* Cleanup possible redirect junk */
  curlx_safefree(data->req.newurl);
  curlx_safefree(data->req.location);

  بدل(status) {
  حالة CURLE_ABORTED_BY_CALLBACK:
  حالة CURLE_READ_ERROR:
  حالة CURLE_WRITE_ERROR:
    /* When we are aborted due to a callback return code it has to be counted
       as premature as there is trouble ahead if we do not. We have many
       callbacks and protocols work differently, we could potentially do this
       more fine-grained in the future. */
    premature = TRUE;
    FALLTHROUGH();
  افتراضي:
    اكسر;
  }

  /* this calls the protocol-specific function pointer previously set */
  إذا(conn && conn->scheme->run->done && (data->mstate >= MSTATE_PROTOCONNECT))
    result = conn->scheme->run->done(data, status, premature);
  وإلا
    result = status;

  إذا(data->mstate > MSTATE_CONNECTING &&
     (result != CURLE_ABORTED_BY_CALLBACK)) {
    /* avoid this if
     * - the transfer has not connected
     * - we already aborted by callback to avoid this calling another callback
     */
    صحيح rc = Curl_pgrsDone(data, pnow);
    إذا(!result && rc)
      result = CURLE_ABORTED_BY_CALLBACK;
  }

  /* Make sure that transfer client writes are really done now. */
  result = Curl_1st_fatal(result, Curl_xfer_write_done(data, premature));

  إذا(!result)
    result = Curl_req_done(&data->req, data, premature);

  إذا(conn) {
    /* Detach connection and decide what to do with it. */
    بنية multi_done_ctx mdctx;

    /* Inform connection filters that this transfer is done and they
     * need to give up any references to `data`. */
    Curl_conn_ev_data_done(data, premature);
    Curl_detach_connection(data);
    Curl_dnscache_prune(data, pnow);

    memset(&mdctx, 0, حجم(mdctx));
    mdctx.premature = premature;
    Curl_cpool_return(data, conn, multi_done_locked, &mdctx, pnow);

    إذا(data->multi)
      multi_schedule_pending(data->multi);
  }

  /* flush the netrc cache */
  Curl_netrc_cleanup(&data->state.netrc);
  أرجع result;
}

CURLMcode Curl_multi_remove_handle(بنية Curl_multi *multi,
                                   بنية Curl_easy *data)
{
  CURLMcode mresult;
  bool premature;
  uint32_t mid;

  /* Prevent users from trying to remove same easy handle more than once */
  إذا(!data->multi)
    أرجع CURLM_OK; /* it is already removed so let's say it is fine! */

  /* Prevent users from trying to remove an easy handle from the wrong multi */
  إذا(data->multi != multi)
    أرجع CURLM_BAD_EASY_HANDLE;

  إذا(data->mid == UINT32_MAX) {
    DEBUGASSERT(0);
    أرجع CURLM_INTERNAL_ERROR;
  }
  إذا(Curl_uint32_tbl_get(&multi->xfers, data->mid) != data) {
    DEBUGASSERT(0);
    أرجع CURLM_INTERNAL_ERROR;
  }

  premature = (data->mstate < MSTATE_COMPLETED);

  إذا(data->conn) {
    /* If the 'state' is not INIT or COMPLETED, we might need to do something
       nice to put the easy_handle in a good known state when this returns. */
    إذا(premature && (data->mstate > MSTATE_DO))
      streamclose(data->conn);

    /* multi_done() clears the association between the easy handle and the
       connection.
       Note that this ignores the return code because there is
       nothing really useful to do with it anyway! */
    (فراغ)multi_done(data, data->result, premature);
  }

  /* The timer must be shut down before data->multi is set to NULL, else
     data's splaynode would remain in the splay tree after curl_easy_cleanup is
     called. Do it after multi_done() in case that sets another time! */
  Curl_expire_clear_all(data);

  /* In MSGSENT, it was deducted from `multi->xfers_alive` already. */
  إذا(data->mstate != MSTATE_MSGSENT)
    --multi->xfers_alive;

  إذا(data->state.really_alive) {
    data->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    إذا(!multi->xfers_really_alive)
      (فراغ)multi_assess_wakeup(multi);
  }

  Curl_wildcard_dtor(&data->wildcard);

  data->mstate = MSTATE_COMPLETED;

  /* Remove the association between the connection and the handle */
  Curl_detach_connection(data);

  /* Tell event handling that this transfer is definitely going away */
  Curl_multi_ev_xfer_done(multi, data);

  إذا(data->set.connect_only) {
    إذا(data->multi_easy) {
      إذا(data->state.last_conn_id != -1) {
        /* Mark any connect-only connection for closure */
        بنية connectdata *conn;
        (فراغ)Curl_getconnectinfo(data, &conn);
        إذا(conn && conn->bits.connect_only)
          connclose(conn);
      }
    }
    وإلا {
      /* This removes a handle that was part the multi interface that used
         CONNECT_ONLY, that connection is now left alive but since this handle
         has bits.close set nothing can use that connection anymore and it is
         forbidden from reuse. This easy handle cannot find the connection
         anymore once removed from the multi handle

         Better close the connection here, at once. */
      بنية connectdata *conn;
      (فراغ)Curl_getconnectinfo(data, &conn);
      إذا(conn)
        Curl_conn_close(data, conn, TRUE);
    }
  }

#ifdef USE_LIBPSL
  /* Remove the PSL association. */
  إذا(data->psl == &multi->psl)
    data->psl = عدم;
#endif

  /* clear the association to this multi handle */
  mid = data->mid;
  DEBUGASSERT(Curl_uint32_tbl_contains(&multi->xfers, mid));
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  Curl_uint32_bset_remove(&multi->process, mid);
  Curl_uint32_bset_remove(&multi->dirty, mid);
  Curl_uint32_bset_remove(&multi->pending, mid);
  Curl_uint32_bset_remove(&multi->msgsent, mid);
  data->multi = عدم;
  data->mid = UINT32_MAX;
  data->master_mid = UINT32_MAX;

  /* A pending transfer *might* be able to run now. */
  multi_schedule_pending(multi);
  mresult = Curl_update_timer(multi);
  إذا(mresult)
    أرجع mresult;

  mresult = multi_assess_wakeup(multi);
  إذا(mresult) {
    failf(data, "error enabling wakeup listening: %d", mresult);
    أرجع mresult;
  }

  CURL_TRC_M(data, "removed from multi, mid=%u, running=%u, total=%u",
             mid, multi_xfers_running(multi),
             Curl_uint32_tbl_کيراءن(&multi->xfers));
  أرجع CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *curl)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_remove_handle, &mresult)) {
    بنية Curl_easy *data = curl;
    إذا(!GOOD_EASY_HANDLE(data) || data->state.internal)
      mresult = CURLM_BAD_EASY_HANDLE;
    وإلا
      mresult = Curl_multi_remove_handle(m, data);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

/* Return TRUE if the application asked for multiplexing */
bool Curl_multiplex_wanted(ثابت بنية Curl_multi *multi)
{
  أرجع multi && multi->multiplexing;
}

/*
 * Curl_detach_connection() removes the given transfer from the connection.
 *
 * This is the only function that should clear data->conn. This will
 * occasionally be called with the data->conn pointer already cleared.
 */
فراغ Curl_detach_connection(بنية Curl_easy *data)
{
  بنية connectdata *conn = data->conn;
  إذا(conn) {
    /* this should never happen, prevent underflow */
    DEBUGASSERT(conn->attached_xfers);
    إذا(conn->attached_xfers) {
      conn->attached_xfers--;
      إذا(!conn->attached_xfers)
        conn->attached_multi = عدم;
    }
  }
  data->conn = عدم;
}

/*
 * Curl_attach_connection() attaches this transfer to this connection.
 *
 * This is the only function that should assign data->conn.
 * `matched == TRUE` means the transfer's properties match this
 * connection and it is not a temporary attach for maintenance.
 */
فراغ Curl_attach_connection(بنية Curl_easy *data,
                            بنية connectdata *conn,
                            bool matched)
{
  DEBUGASSERT(data);
  DEBUGASSERT(!data->conn);
  DEBUGASSERT(conn);
  DEBUGASSERT(conn->attached_xfers < UINT32_MAX);
  data->conn = conn;
  إذا(matched) {
    data->state.last_conn_id = conn->connection_id;
    data->state.last_cpid = conn->cpid;
  }
  وإلا
    DEBUGASSERT(!data->mid); /* admin handle */
  conn->attached_xfers++;
  /* all attached transfers must be from the same multi */
  إذا(!conn->attached_multi)
    conn->attached_multi = data->multi;
  DEBUGASSERT(conn->attached_multi == data->multi);

  إذا(conn->scheme && conn->scheme->run->attach)
    conn->scheme->run->attach(data, conn);
}

/* adjust pollset for rate limits/pauses */
ساكن CURLcode multi_adjust_pollset(بنية Curl_easy *data,
                                     بنية easy_pollset *ps)
{
  CURLcode result = CURLE_OK;

  إذا(ps->n) {
    bool send_blocked, recv_blocked;

    recv_blocked = (Curl_rlimit_avail(&data->progress.dl.rlimit, عدم) <= 0);
    send_blocked = (Curl_rlimit_avail(&data->progress.ul.rlimit, عدم) <= 0);
    إذا(send_blocked || recv_blocked) {
      صحيح i;
      لكل(i = 0; i <= SECONDARYSOCKET; ++i) {
        curl_socket_t sock = data->conn->sock[i];
        إذا(sock == CURL_SOCKET_BAD)
          استمر;
        إذا(recv_blocked && Curl_pollset_want_recv(data, ps, sock)) {
          result = Curl_pollset_remove_in(data, ps, sock);
          إذا(result)
            اكسر;
        }
        إذا(send_blocked && Curl_pollset_want_send(data, ps, sock)) {
          result = Curl_pollset_remove_out(data, ps, sock);
          إذا(result)
            اكسر;
        }
      }
    }

    /* Not blocked and wanting to receive. If there is data pending
     * in the connection filters, make transfer run again. */
    إذا(!recv_blocked &&
       ((Curl_pollset_want_recv(data, ps, data->conn->sock[FIRSTSOCKET]) &&
         Curl_conn_data_pending(data, FIRSTSOCKET)) ||
        (Curl_pollset_want_recv(data, ps, data->conn->sock[SECONDARYSOCKET]) &&
         Curl_conn_data_pending(data, SECONDARYSOCKET)))) {
      CURL_TRC_M(data, "pollset[] has POLLIN, but there is still "
                 "buffered input -> mark as dirty");
      Curl_multi_mark_dirty(data);
    }
  }
  أرجع result;
}

ساكن CURLcode mstate_connecting_pollset(بنية Curl_easy *data,
                                          بنية easy_pollset *ps)
{
  بنية connectdata *conn = data->conn;
  curl_socket_t sockfd;
  CURLcode result = CURLE_OK;

  إذا(Curl_xfer_recv_is_paused(data))
    أرجع CURLE_OK;
  /* If a socket is set, receiving is default. If the socket
   * has not been determined yet (eyeballing), always ask the
   * connection filters for what to monitor. */
  sockfd = Curl_conn_get_first_socket(data);
  إذا(sockfd != CURL_SOCKET_BAD) {
    result = Curl_pollset_change(data, ps, sockfd, CURL_POLL_IN, 0);
    إذا(!result)
      result = multi_adjust_pollset(data, ps);
  }
  إذا(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  أرجع result;
}

ساكن CURLcode mstate_protocol_pollset(بنية Curl_easy *data,
                                        بنية easy_pollset *ps)
{
  بنية connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  إذا(conn->scheme->run->proto_pollset)
    result = conn->scheme->run->proto_pollset(data, ps);
  وإلا {
    curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
    إذا(sockfd != CURL_SOCKET_BAD) {
      /* Default is to wait to something from the server */
      result = Curl_pollset_change(data, ps, sockfd, CURL_POLL_IN, 0);
    }
  }
  إذا(!result)
    result = multi_adjust_pollset(data, ps);
  إذا(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  أرجع result;
}

ساكن CURLcode mstate_do_pollset(بنية Curl_easy *data,
                                  بنية easy_pollset *ps)
{
  بنية connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  إذا(conn->scheme->run->doing_pollset)
    result = conn->scheme->run->doing_pollset(data, ps);
  وإلا إذا(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    result = Curl_pollset_add_out(data, ps, conn->sock[conn->send_idx]);
  }
  إذا(!result)
    result = multi_adjust_pollset(data, ps);
  إذا(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  أرجع result;
}

ساكن CURLcode mstate_domore_pollset(بنية Curl_easy *data,
                                      بنية easy_pollset *ps)
{
  بنية connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  إذا(conn->scheme->run->domore_pollset)
    result = conn->scheme->run->domore_pollset(data, ps);
  وإلا إذا(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    result = Curl_pollset_add_out(data, ps, conn->sock[conn->send_idx]);
  }
  إذا(!result)
    result = multi_adjust_pollset(data, ps);
  إذا(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  أرجع result;
}

ساكن CURLcode mstate_perform_pollset(بنية Curl_easy *data,
                                       بنية easy_pollset *ps)
{
  بنية connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  إذا(conn->scheme->run->perform_pollset)
    result = conn->scheme->run->perform_pollset(data, ps);
  وإلا {
    /* Default is to obey the request flags for send/recv */
    إذا(Curl_req_want_recv(data) && CONN_SOCK_IDX_VALID(conn->recv_idx)) {
      result = Curl_pollset_add_in(data, ps, conn->sock[conn->recv_idx]);
    }
    إذا(!result && Curl_req_want_send(data) &&
       CONN_SOCK_IDX_VALID(conn->send_idx)) {
      result = Curl_pollset_add_out(data, ps, conn->sock[conn->send_idx]);
    }
  }
  إذا(!result)
    result = multi_adjust_pollset(data, ps);
  إذا(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  أرجع result;
}

#ifdef CURLVERBOSE
ساكن size_t multi_timeouts_کيراءن(بنية expire_timers *timeouts)
{
  size_t n = 0;
  uint8_t eid = timeouts->first;
  لكل(; eid < EXPIRE_LAST; eid = timeouts->next[eid])
    ++n;
  أرجع n;
}
#endif

/* Initializes `poll_set` with the current socket poll actions needed
 * for transfer `data`. */
CURLMcode Curl_multi_pollset(بنية Curl_easy *data,
                             بنية easy_pollset *ps)
{
  CURLcode result = CURLE_OK;

  Curl_pollset_reset(ps);
#ifdef ENABLE_INTERNAL_WAKEUP
  /* The admin handle always listens on the wakeup socket when there
   * are transfers alive. */
  إذا(data->multi && (data == data->multi->admin) &&
     data->multi->xfers_really_alive) {
    CURL_TRC_M(data, "adding wakeup, %u xfers really alive",
               data->multi->xfers_really_alive);
    result = Curl_pollset_add_in(data, ps, data->multi->wakeup_internal[0]);
  }
#endif
  /* If the transfer has no connection, this is fine. Happens when
     called via curl_multi_remove_handle() => Curl_multi_ev_assess() =>
     Curl_multi_pollset(). */
  إذا(!result && data->conn) {
    بدل(data->mstate) {
    حالة MSTATE_INIT:
    حالة MSTATE_PENDING:
    حالة MSTATE_SETUP:
    حالة MSTATE_CONNECT:
      /* nothing to poll for yet */
      اكسر;

    حالة MSTATE_CONNECTING:
      result = mstate_connecting_pollset(data, ps);
      اكسر;

    حالة MSTATE_PROTOCONNECT:
    حالة MSTATE_PROTOCONNECTING:
      result = mstate_protocol_pollset(data, ps);
      اكسر;

    حالة MSTATE_DO:
    حالة MSTATE_DOING:
      result = mstate_do_pollset(data, ps);
      اكسر;

    حالة MSTATE_DOING_MORE:
      result = mstate_domore_pollset(data, ps);
      اكسر;

    حالة MSTATE_DID: /* same as PERFORMING in regard to polling */
    حالة MSTATE_PERFORMING:
      result = mstate_perform_pollset(data, ps);
      اكسر;

    حالة MSTATE_RATELIMITING:
      /* we need to let time pass, ignore socket(s) */
      اكسر;

    حالة MSTATE_DONE:
    حالة MSTATE_COMPLETED:
    حالة MSTATE_MSGSENT:
      /* nothing more to poll for */
      اكسر;

    افتراضي:
      failf(data, "multi_getsock: unexpected multi state %d",
            (صحيح)data->mstate);
      DEBUGASSERT(0);
      اكسر;
    }
  }

  إذا(result) {
    إذا(result == CURLE_OUT_OF_MEMORY)
      أرجع CURLM_OUT_OF_MEMORY;
    failf(data, "error determining pollset: %d", (صحيح)result);
    أرجع CURLM_INTERNAL_ERROR;
  }

#ifdef CURLVERBOSE
  إذا(CURL_TRC_M_is_verbose(data)) {
    size_t timeout_کيراءن = multi_timeouts_کيراءن(&data->state.timeouts);
    بدل(ps->n) {
    حالة 0:
      CURL_TRC_M(data, "pollset[], timeouts=%zu, paused %d/%d (r/w)",
                 timeout_کيراءن,
                 Curl_xfer_send_is_paused(data),
                 Curl_xfer_recv_is_paused(data));
      اكسر;
    حالة 1:
      CURL_TRC_M(data, "pollset[fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_OUT) ? "OUT" : "",
                 timeout_کيراءن);
      اكسر;
    حالة 2:
      CURL_TRC_M(data, "pollset[fd=%" FMT_SOCKET_T " %s%s, "
                 "fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_OUT) ? "OUT" : "",
                 ps->sockets[1],
                 (ps->actions[1] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[1] & CURL_POLL_OUT) ? "OUT" : "",
                 timeout_کيراءن);
      اكسر;
    افتراضي:
      CURL_TRC_M(data, "pollset[fds=%u], timeouts=%zu", ps->n, timeout_کيراءن);
      اكسر;
    }
    CURL_TRC_EASY_TIMERS(data);
  }
#endif

  أرجع CURLM_OK;
}

CURLMcode curl_multi_fdset(CURLM *m,
                           fd_set *read_fd_set, fd_set *write_fd_set,
                           fd_set *exc_fd_set, صحيح *max_fd)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_fdset, &mresult)) {
    /* Scan through all the easy handles to get the file descriptors set.
       Some easy handles may not have connected to the remote host yet,
       and then we must make sure that is done. */
    بنية Curl_multi *multi = m;
    بنية easy_pollset ps;
    صحيح this_max_fd = -1;
    unsigned صحيح i;
    uint32_t mid;
    (فراغ)exc_fd_set;

    Curl_pollset_init(&ps);
    إذا(Curl_uint32_bset_first(&multi->process, &mid)) {
      افعل {
        بنية Curl_easy *data = Curl_multi_get_easy(multi, mid);

        إذا(!data) {
          DEBUGASSERT(0);
          استمر;
        }

        Curl_multi_pollset(data, &ps);
        لكل(i = 0; i < ps.n; i++) {
          إذا(!FDSET_SOCK(ps.sockets[i]))
            /* pretend it does not exist */
            استمر;
          إذا(ps.actions[i] & CURL_POLL_IN)
            FD_SET(ps.sockets[i], read_fd_set);
          إذا(ps.actions[i] & CURL_POLL_OUT)
            FD_SET(ps.sockets[i], write_fd_set);
          إذا((صحيح)ps.sockets[i] > this_max_fd)
            this_max_fd = (صحيح)ps.sockets[i];
        }
      } طالما(Curl_uint32_bset_next(&multi->process, mid, &mid));
    }

    Curl_cshutdn_setfds(&multi->cshutdn, multi->admin,
                        read_fd_set, write_fd_set, &this_max_fd);

    *max_fd = this_max_fd;
    Curl_pollset_cleanup(&ps);

    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

CURLMcode curl_multi_waitfds(CURLM *m,
                             بنية curl_waitfd *ufds,
                             unsigned صحيح size,
                             unsigned صحيح *fd_کيراءن)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_waitfds, &mresult)) {
    بنية Curl_waitfds cwfds;
    بنية Curl_multi *multi = m;
    بنية easy_pollset ps;
    unsigned صحيح need = 0;
    uint32_t mid;

    إذا(!ufds && (size || !fd_کيراءن)) {
      mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      اذهب out;
    }

    Curl_pollset_init(&ps);
    Curl_waitfds_init(&cwfds, ufds, size);
    mresult = CURLM_OK;
    إذا(Curl_uint32_bset_first(&multi->process, &mid)) {
      افعل {
        بنية Curl_easy *data = Curl_multi_get_easy(multi, mid);
        إذا(!data) {
          DEBUGASSERT(0);
          Curl_uint32_bset_remove(&multi->process, mid);
          Curl_uint32_bset_remove(&multi->dirty, mid);
          استمر;
        }
        Curl_multi_pollset(data, &ps);
        need += Curl_waitfds_add_ps(&cwfds, &ps);
      } طالما(Curl_uint32_bset_next(&multi->process, mid, &mid));
    }

    need += Curl_cshutdn_add_waitfds(&multi->cshutdn, multi->admin, &cwfds);

    إذا(need != cwfds.n && ufds)
      mresult = CURLM_OUT_OF_MEMORY;

    إذا(fd_کيراءن)
      *fd_کيراءن = need;
    Curl_pollset_cleanup(&ps);
  }
out:
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

#ifdef USE_WINSOCK
/* Reset FD_WRITE for TCP sockets. Nothing is actually sent. UDP sockets cannot
 * be reset this way because an empty datagram would be sent. #9203
 *
 * "On Windows the internal state of FD_WRITE as returned from
 * WSAEnumNetworkEvents is only reset after successful send()."
 */
ساكن فراغ reset_socket_fdwrite(curl_socket_t s)
{
  صحيح t;
  صحيح l = (صحيح)حجم(t);
  إذا(!getsockopt(s, SOL_SOCKET, SO_TYPE, (حرف *)&t, &l) && t == SOCK_STREAM)
    swrite(s, عدم, 0);
}

ساكن CURLMcode multi_winsock_select(بنية Curl_multi *multi,
                                      بنية curl_pollfds *cpfds,
                                      unsigned صحيح curl_nfds,
                                      بنية curl_waitfd extra_fds[],
                                      unsigned صحيح extra_nfds,
                                      صحيح timeout_ms,
                                      bool extrawait,
                                      صحيح *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  WSANETWORKEVENTS wsa_events;
  صحيح nevents = 0;
  size_t i;

  DEBUGASSERT(multi->wsa_event != WSA_INVALID_EVENT);

  /* Set the WSA events based on the collected pollds */
  لكل(i = 0; i < cpfds->n; i++) {
    long mask = 0;
    إذا(cpfds->pfds[i].events & POLLIN)
      mask |= FD_READ | FD_ACCEPT | FD_CLOSE;
    إذا(cpfds->pfds[i].events & POLLPRI)
      mask |= FD_OOB;
    إذا(cpfds->pfds[i].events & POLLOUT) {
      mask |= FD_WRITE | FD_CONNECT | FD_CLOSE;
      reset_socket_fdwrite(cpfds->pfds[i].fd);
    }
    إذا(mask && WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, mask)) {
      mresult = CURLM_OUT_OF_MEMORY;
      اذهب out;
    }
  }

  إذا(cpfds->n || extrawait) {
    صحيح pollrc = 0;

    إذا(cpfds->n) {         /* pre-check with Winsock */
      pollrc = Curl_poll(cpfds->pfds, cpfds->n, 0);
      إذا(pollrc < 0) {
        mresult = CURLM_UNRECOVERABLE_POLL;
        اذهب out;
      }
      nevents = pollrc;
    }

    إذا(!nevents) {
      /* now wait... if not ready during the pre-check (pollrc == 0) */
      WSAWaitForMultipleEvents(1, &multi->wsa_event, FALSE, (DWORD)timeout_ms,
                               FALSE);
    }

    /* With Winsock, we have to run the following section unconditionally
       to call WSAEventSelect(fd, event, 0) on all the sockets */
    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    لكل(i = 0; i < extra_nfds; i++) {
      unsigned short mask = 0;
      curl_socket_t s = extra_fds[i].fd;

      wsa_events.lNetworkEvents = 0;
      إذا(WSAEnumNetworkEvents(s, عدم, &wsa_events) == 0) {
        إذا(wsa_events.lNetworkEvents & (FD_READ | FD_ACCEPT | FD_CLOSE))
          mask |= CURL_WAIT_POLLIN;
        إذا(wsa_events.lNetworkEvents & (FD_WRITE | FD_CONNECT | FD_CLOSE))
          mask |= CURL_WAIT_POLLOUT;
        إذا(wsa_events.lNetworkEvents & FD_OOB)
          mask |= CURL_WAIT_POLLPRI;
        إذا(!pollrc && wsa_events.lNetworkEvents)
          nevents++;
      }
      WSAEventSelect(s, multi->wsa_event, 0);
      إذا(!pollrc) {
        extra_fds[i].revents = (short)mask;
        استمر;
      }
      وإلا {
        unsigned r = (unsigned)cpfds->pfds[curl_nfds + i].revents;
        إذا(r & POLLIN)
          mask |= CURL_WAIT_POLLIN;
        إذا(r & POLLOUT)
          mask |= CURL_WAIT_POLLOUT;
        إذا(r & POLLPRI)
          mask |= CURL_WAIT_POLLPRI;
        extra_fds[i].revents = (short)mask;
      }
    }

    /* Count up all our own sockets that had activity,
       and remove them from the event. */
    لكل(i = 0; i < curl_nfds; ++i) {
      wsa_events.lNetworkEvents = 0;
      إذا(WSAEnumNetworkEvents(cpfds->pfds[i].fd, عدم, &wsa_events) == 0) {
        إذا(!pollrc && wsa_events.lNetworkEvents)
          nevents++;
      }
      WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, 0);
    }
    WSAResetEvent(multi->wsa_event);
  }

out:
  *pnevents = nevents;
  أرجع mresult;
}

#وإلا /* USE_WINSOCK */

ساكن CURLMcode multi_posix_poll(بنية Curl_multi *multi,
                                  بنية curl_pollfds *cpfds,
                                  unsigned صحيح curl_nfds,
                                  بنية curl_waitfd extra_fds[],
                                  unsigned صحيح extra_nfds,
                                  صحيح timeout_ms,
                                  bool extrawait,
                                  صحيح *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  صحيح nevents = 0;
  size_t i;

  (فراغ)multi;
  إذا(cpfds->n) {
    صحيح pollrc = Curl_poll(cpfds->pfds, cpfds->n, timeout_ms); /* wait... */
    إذا(pollrc < 0) {
      mresult = CURLM_UNRECOVERABLE_POLL;
      اذهب out;
    }
    nevents = pollrc;

    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    لكل(i = 0; i < extra_nfds; i++) {
      unsigned r = (unsigned)cpfds->pfds[curl_nfds + i].revents;
      unsigned short mask = 0;
      إذا(r & POLLIN)
        mask |= CURL_WAIT_POLLIN;
      إذا(r & POLLOUT)
        mask |= CURL_WAIT_POLLOUT;
      إذا(r & POLLPRI)
        mask |= CURL_WAIT_POLLPRI;
      extra_fds[i].revents = (short)mask;
    }
  }
  وإلا إذا(extrawait) {
    /* No fds to poll, but asked to obey timeout_ms anyway. We cannot
     * use Curl_poll() as it, on some platforms, returns immediately
     * without fds. */
    curlx_wait_ms(timeout_ms);
  }

out:
  *pnevents = nevents;
  أرجع mresult;
}

#endif /* !USE_WINSOCK */

#define اڠک_POLLS_ON_تيندنن 10

ساكن CURLMcode multi_wait(بنية Curl_multi *multi,
                            بنية curl_waitfd extra_fds[],
                            unsigned صحيح extra_nfds,
                            صحيح timeout_ms,
                            صحيح *ret,
                            bool extrawait)  /* when no socket, wait */
{
  size_t i;
  صحيح timeout_internal;
  صحيح nevents = 0;
  بنية easy_pollset ps;
  بنية pollfd a_few_on_تيندنن[اڠک_POLLS_ON_تيندنن];
  بنية curl_pollfds cpfds;
  unsigned صحيح curl_nfds = 0; /* how many pfds are for curl transfers */
  بنية Curl_easy *data = عدم;
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;
#ifdef ENABLE_WAKEUP
  صحيح wakeup_idx = -1;
#endif

  إذا(timeout_ms < 0)
    أرجع CURLM_BAD_FUNCTION_ARGUMENT;

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_تيندنن, اڠک_POLLS_ON_تيندنن);

  /* Add the curl handles to our pollfds first */
  إذا(Curl_uint32_bset_first(&multi->process, &mid)) {
    افعل {
      data = Curl_multi_get_easy(multi, mid);
      إذا(!data) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        استمر;
      }
      Curl_multi_pollset(data, &ps);
      إذا(Curl_pollfds_add_ps(&cpfds, &ps)) {
        mresult = CURLM_OUT_OF_MEMORY;
        اذهب out;
      }
    } طالما(Curl_uint32_bset_next(&multi->process, mid, &mid));
  }

  إذا(Curl_cshutdn_add_pollfds(&multi->cshutdn, multi->admin, &cpfds)) {
    mresult = CURLM_OUT_OF_MEMORY;
    اذهب out;
  }

#ifdef ENABLE_WAKEUP
  /* If `extrawait` is TRUE *or* we have `extra_fds`to poll *or* we
   * have transfer sockets to poll, we obey `timeout_ms`.
   * Then we need to also monitor the multi's wakeup
   * socket to catch calls to `curl_multi_wakeup()` during the wait. */
  إذا(extrawait || cpfds.n || extra_nfds) {
    wakeup_idx = cpfds.n;
    إذا(Curl_pollfds_add_sock(&cpfds, multi->wakeup_pair[0], POLLIN)) {
      mresult = CURLM_OUT_OF_MEMORY;
      اذهب out;
    }
  }
#endif

  curl_nfds = cpfds.n; /* what curl internally uses in cpfds */
  /* Add external file descriptions from poll-like struct curl_waitfd */
  لكل(i = 0; i < extra_nfds; i++) {
    unsigned short events = 0;
    إذا(extra_fds[i].events & CURL_WAIT_POLLIN)
      events |= POLLIN;
    إذا(extra_fds[i].events & CURL_WAIT_POLLPRI)
      events |= POLLPRI;
    إذا(extra_fds[i].events & CURL_WAIT_POLLOUT)
      events |= POLLOUT;
    إذا(Curl_pollfds_add_sock(&cpfds, extra_fds[i].fd, events)) {
      mresult = CURLM_OUT_OF_MEMORY;
      اذهب out;
    }
  }

  /* We check the internal timeout *AFTER* we collected all sockets to
   * poll. Collecting the sockets may install new timers by protocols
   * and connection filters.
   * Use the shorter one of the internal and the caller requested timeout.
   * If we are called with `!extrawait` and multi_timeout() reports no
   * timeouts exist, do not wait. */
  multi_timeout(multi, عدم, &timeout_internal);
  إذا((timeout_internal >= 0) && (timeout_internal < timeout_ms))
    timeout_ms = timeout_internal;

  إذا(data)
    CURL_TRC_M(data, "multi_wait(fds=%u, timeout=%d) tinternal=%d",
               cpfds.n, timeout_ms, timeout_internal);

#ifdef USE_WINSOCK
  mresult = multi_winsock_select(multi, &cpfds, curl_nfds,
                                 extra_fds, extra_nfds,
                                 timeout_ms, extrawait, &nevents);
#وإلا
  mresult = multi_posix_poll(multi, &cpfds, curl_nfds,
                             extra_fds, extra_nfds,
                             timeout_ms, extrawait, &nevents);
#endif

#ifdef ENABLE_WAKEUP
    إذا(nevents && (wakeup_idx >= 0)) {
      إذا(cpfds.pfds[wakeup_idx].revents & POLLIN) {
        (فراغ)Curl_wakeup_consume(multi->wakeup_pair, TRUE);
        /* do not count the wakeup socket into the returned value */
        nevents--;
      }
    }
#endif

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  إذا(ret)
    *ret = nevents;
  أرجع mresult;
}

CURLMcode curl_multi_wait(CURLM *m,
                          بنية curl_waitfd extra_fds[],
                          unsigned صحيح extra_nfds,
                          صحيح timeout_ms,
                          صحيح *ret)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_wait, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, FALSE);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

CURLMcode curl_multi_poll(CURLM *m,
                          بنية curl_waitfd extra_fds[],
                          unsigned صحيح extra_nfds,
                          صحيح timeout_ms,
                          صحيح *ret)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_poll, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, TRUE);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

CURLMcode curl_multi_wakeup(CURLM *m)
{
  /* this function is usually called from another thread,
     it has to be careful only to access parts of the
     Curl_multi struct that are constant */
  بنية Curl_multi *multi = m;
  CURLMcode mresult = CURLM_WAKEUP_FAILURE;

  /* GOOD_MULTI_HANDLE can be safely called */
  إذا(!GOOD_MULTI_HANDLE(multi))
    أرجع CURLM_BAD_HANDLE;

#ifdef ENABLE_WAKEUP
  /* the wakeup_pair variable is only written during init and cleanup,
     making it safe to access from another thread after the init part
     and before cleanup */
  إذا(!Curl_wakeup_signal(multi->wakeup_pair))
    mresult = CURLM_OK;
#endif
#ifdef USE_WINSOCK
  إذا(WSASetEvent(multi->wsa_event))
    mresult = CURLM_OK;
#endif
  أرجع mresult;
}

#ifdef ENABLE_INTERNAL_WAKEUP
فراغ Curl_multi_wakeup_internal(بنية Curl_multi *multi)
{
  /* This is expected to be invocable from another thread which
   * does NOT outlive the multi handle. Check for sanity. */
  إذا(GOOD_MULTI_HANDLE(multi))
    Curl_wakeup_signal(multi->wakeup_internal);
  وإلا
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
ساكن bool multi_ischanged(بنية Curl_multi *multi, bool clear)
{
  bool retval = FALSE;
  DEBUGASSERT(multi);
  إذا(multi) {
    retval = (bool)multi->recheckstate;
    إذا(clear)
      multi->recheckstate = FALSE;
  }
  أرجع retval;
}

/*
 * Curl_multi_connchanged() is called to tell that there is a connection in
 * this multi handle that has changed state (multiplexing become possible, the
 * number of allowed streams changed or similar), and a subsequent use of this
 * multi handle should move CONNECT_PEND handles back to CONNECT to have them
 * retry.
 */
فراغ Curl_multi_connchanged(بنية Curl_multi *multi)
{
  /* May be invoked for a `data->multi` when no multi is present. */
  إذا(multi)
    multi->recheckstate = TRUE;
}

CURLMcode Curl_multi_add_perform(بنية Curl_multi *multi,
                                 بنية Curl_easy *data,
                                 بنية connectdata *conn)
{
  CURLMcode mresult;

  mresult = Curl_multi_add_handle(multi, data);
  إذا(!mresult) {
    CURLcode result;

    /* pass in NULL for 'conn' here since we do not want to init the
       connection, only this transfer */
    result = Curl_init_transfer(data, عدم);
    إذا(result) {
      Curl_multi_remove_handle(multi, data);
      أرجع CURLM_INTERNAL_ERROR;
    }

    /* take this handle to the perform state right away */
    multistate(data, MSTATE_PERFORMING);
    Curl_attach_connection(data, conn, TRUE);
    CURL_REQ_SET_RECV(data);
  }
  أرجع mresult;
}

ساكن CURLcode multi_do(بنية Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(conn->scheme);

  إذا(conn->scheme->run->do_it)
    result = conn->scheme->run->do_it(data, done);

  أرجع result;
}

/*
 * multi_do_more() is called during the DO_MORE multi state. It is a second
 * stage DO state which (wrongly) was introduced to support FTP's second
 * connection.
 *
 * 'complete' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (to DOING state when there is more work to do)
 */

ساكن CURLcode multi_do_more(بنية Curl_easy *data, domore *complete)
{
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;

  *complete = DOMORE_INCOMPLETE;

  إذا(conn->scheme->run->do_more)
    result = conn->scheme->run->do_more(data, complete);

  أرجع result;
}

/*
 * Check whether a timeout occurred, and handle it if it did
 */
ساكن bool multi_handle_timeout(بنية Curl_easy *data,
                                 ثابت بنية curltime *pnow,
                                 bool *stream_error,
                                 CURLcode *result)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_timeleft_now_ms(data, pnow);
  إذا(timeout_ms < 0) {
    /* Handle timed out */
    timerid base_timer = Curl_is_connecting(data) ?
                         TIMER_STARTSINGLE : TIMER_STARTOP;
    timediff_t elapsed_ms = Curl_pgrs_since_ms(data, عدم, base_timer);
    إذا(data->mstate == MSTATE_CONNECTING) {
      failf(data, "%s timed out after %" FMT_TIMEDIFF_T " milliseconds",
            data->conn->bits.dns_resolved ? "Connection" : "Resolving",
            elapsed_ms);
      /* keep what we know about a partly done connect, e.g. TCP done
       * but TLS handshake still ongoing */
      Curl_conn_report_failed(data, FIRSTSOCKET);
    }
    وإلا {
      بنية SingleRequest *k = &data->req;
      إذا(k->size != -1) {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount, k->size);
      }
      وإلا {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount);
      }
    }
    *result = CURLE_OPERATION_TIMEDOUT;
    إذا(data->conn) {
      /* Force connection closed if the connection has indeed been used */
      إذا(data->mstate > MSTATE_DO) {
        streamclose(data->conn);
        *stream_error = TRUE;
      }
      (فراغ)multi_done(data, *result, TRUE);
    }
    أرجع TRUE;
  }

  أرجع FALSE;
}

/*
 * We are doing protocol-specific connecting and this is being called over and
 * over from the multi interface until the connection phase is done on
 * protocol layer.
 */

ساكن CURLcode protocol_connecting(بنية Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;

  إذا(conn && conn->scheme->run->connecting) {
    *done = FALSE;
    result = conn->scheme->run->connecting(data, done);
  }
  وإلا
    *done = TRUE;

  أرجع result;
}

/*
 * We are DOING this is being called over and over from the multi interface
 * until the DOING phase is done on protocol layer.
 */

ساكن CURLcode protocol_doing(بنية Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  بنية connectdata *conn = data->conn;

  إذا(conn && conn->scheme->run->doing) {
    *done = FALSE;
    result = conn->scheme->run->doing(data, done);
  }
  وإلا
    *done = TRUE;

  أرجع result;
}

/*
 * We have discovered that the TCP connection has been successful, we can now
 * proceed with some action.
 *
 */
ساكن CURLcode protocol_connect(بنية Curl_easy *data, bool *protocol_done)
{
  بنية connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(conn);
  DEBUGASSERT(protocol_done);
  DEBUGASSERT(Curl_conn_is_connected(conn, FIRSTSOCKET));

  *protocol_done = FALSE;
  إذا(!conn->bits.protoconnstart) {
    إذا(conn->scheme->run->connect_it) {
      /* Call the protocol-specific connect function */
      result = conn->scheme->run->connect_it(data, protocol_done);
      إذا(result)
        أرجع result;
    }
    conn->bits.protoconnstart = TRUE;
  }

  /* Unless this protocol does not have any protocol-connect callback, as
     then we know we are done. */
  إذا(!conn->scheme->run->connecting)
    *protocol_done = TRUE;
  أرجع CURLE_OK;
}

/*
 * posttransfer() is called immediately after a transfer ends
 */
ساكن فراغ multi_posttransfer(بنية Curl_easy *data)
{
#إذا defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
  /* restore the signal handler for SIGPIPE before we get back */
  إذا(!data->set.no_signal)
    إشارة(SIGPIPE, data->state.prev_signal);
#وإلا
  (فراغ)data;
#endif
}

/*
 * multi_follow() handles the URL redirect magic. Pass in the 'newurl' string
 * as given by the remote server and set up the new URL to request.
 *
 * This function DOES NOT FREE the given URL.
 */
ساكن CURLcode multi_follow(بنية Curl_easy *data,
                             ثابت بنية Curl_scheme *handler,
                             ثابت حرف *newurl, /* the Location: string */
                             followtype type) /* see transfer.h */
{
  إذا(handler && handler->run->follow)
    أرجع handler->run->follow(data, newurl, type);

  إذا(type == FOLLOW_RETRY)
    /* Retries are generic and do not require protocol-specific redirect
       handling. */
    أرجع CURLE_OK;

  أرجع CURLE_TOO_MANY_REDIRECTS;
}

ساكن CURLcode mspeed_check(بنية Curl_easy *data)
{
  إذا(Curl_rlimit_active(&data->progress.dl.rlimit) ||
     Curl_rlimit_active(&data->progress.ul.rlimit)) {
    /* check if our send/recv limits require idle waits */
    ثابت بنية curltime *pnow = Curl_pgrs_now(data);
    timediff_t recv_ms, send_ms;

    send_ms = Curl_rlimit_wait_ms(&data->progress.ul.rlimit, pnow);
    recv_ms = Curl_rlimit_wait_ms(&data->progress.dl.rlimit, pnow);

    إذا(send_ms || recv_ms) {
      إذا(data->mstate != MSTATE_RATELIMITING) {
        multistate(data, MSTATE_RATELIMITING);
      }
      Curl_expire_set(data, EXPIRE_TOOFAST, CURLMAX(send_ms, recv_ms), pnow);
      Curl_multi_clear_dirty(data);
      CURL_TRC_M(data, "[RLIMIT] waiting %" FMT_TIMEDIFF_T "ms",
                 CURLMAX(send_ms, recv_ms));
      أرجع CURLE_AGAIN;
    }
    وإلا {
      /* when will the rate limits increase next? The transfer needs
       * to run again at that time or it may stall. */
      send_ms = Curl_rlimit_next_step_ms(&data->progress.ul.rlimit, pnow);
      recv_ms = Curl_rlimit_next_step_ms(&data->progress.dl.rlimit, pnow);
      إذا(send_ms || recv_ms) {
        timediff_t next_ms = CURLMIN(send_ms, recv_ms);
        إذا(!next_ms)
          next_ms = CURLMAX(send_ms, recv_ms);
        Curl_expire_set(data, EXPIRE_TOOFAST, next_ms, pnow);
        CURL_TRC_M(data, "[RLIMIT] next token update in %" FMT_TIMEDIFF_T "ms",
                   next_ms);
      }
    }
  }

  إذا(data->mstate != MSTATE_PERFORMING) {
    CURL_TRC_M(data, "[RLIMIT] wait over, continue");
    multistate(data, MSTATE_PERFORMING);
  }
  أرجع CURLE_OK;
}

ساكن CURLMcode multistate_performing(بنية Curl_easy *data,
                                       bool *stream_errorp,
                                       CURLcode *resultp)
{
  حرف *newurl = عدم;
  bool retry = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = *resultp = CURLE_OK;
  *stream_errorp = FALSE;

  إذا(mspeed_check(data) == CURLE_AGAIN)
    أرجع CURLM_OK;

  /* read/write data if it is ready to do so */
  result = Curl_sendrecv(data);

  إذا(data->req.done || (result == CURLE_RECV_ERROR)) {
    /* If CURLE_RECV_ERROR happens early enough, we assume it was a race
     * condition and the server closed the reused connection exactly when we
     * wanted to use it, so figure out if that is indeed the case.
     */
    CURLcode ret = Curl_retry_request(data, &newurl);
    إذا(!ret)
      retry = !!newurl;
    وإلا إذا(!result)
      result = ret;

    إذا(retry) {
      /* if we are to retry, set the result to OK and consider the
         request as done */
      result = CURLE_OK;
      data->req.done = TRUE;
    }
  }
#ifndef CURL_DISABLE_HTTP
  وإلا إذا((result == CURLE_HTTP2_STREAM) &&
          Curl_h2_http_1_1_error(data)) {
    CURLcode ret = Curl_retry_request(data, &newurl);

    إذا(!ret) {
      infof(data, "Downgrades to HTTP/1.1");
      streamclose(data->conn);
      data->state.http_neg.wanted = CURL_HTTP_V1x;
      data->state.http_neg.allowed = CURL_HTTP_V1x;
      /* clear the error message bit too as we ignore the one we got */
      data->state.errorbuf = FALSE;
      إذا(!newurl)
        /* typically for HTTP_1_1_REQUIRED error on first flight */
        newurl = Curl_bufref_dup(&data->state.url);
      إذا(!newurl) {
        result = CURLE_OUT_OF_MEMORY;
      }
      وإلا {
        /* if we are to retry, set the result to OK and consider the request
          as done */
        retry = TRUE;
        result = CURLE_OK;
        data->req.done = TRUE;
      }
    }
    وإلا
      result = ret;
  }
#endif

  إذا(result) {
    /*
     * The transfer phase returned error, we mark the connection to get closed
     * to prevent being reused. This is because we cannot possibly know if the
     * connection is in a good shape or not now. Unless it is a protocol which
     * uses two "channels" like FTP, as then the error happened in the data
     * connection.
     */

    إذا(!(data->conn->scheme->flags & PROTOPT_DUAL) &&
       result != CURLE_HTTP2_STREAM)
      streamclose(data->conn);

    multi_posttransfer(data);
    multi_done(data, result, TRUE);
  }
  وإلا إذا(data->req.done && !Curl_cwriter_is_paused(data)) {
    ثابت بنية Curl_scheme *handler = data->conn->scheme;

    /* call this even if the readwrite function returned error */
    multi_posttransfer(data);

    /* When we follow redirects or is set to retry the connection, we must to
       go back to the CONNECT state */
    إذا(data->req.newurl || retry) {
      followtype follow = FOLLOW_NONE;
      إذا(!retry) {
        /* if the URL is a follow-location and not a retried request then
           figure out the URL here */
        curlx_free(newurl);
        newurl = data->req.newurl;
        data->req.newurl = عدم;
        follow = FOLLOW_REDIR;
      }
      وإلا
        follow = FOLLOW_RETRY;
      (فراغ)multi_done(data, CURLE_OK, FALSE);
      /* multi_done() might return CURLE_GOT_NOTHING */
      result = multi_follow(data, handler, newurl, follow);
      إذا(!result) {
        multistate(data, MSTATE_SETUP);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    وإلا {
      /* after the transfer is done, go DONE */

      /* but first check to see if we got a location info even though we are
         not following redirects */
      إذا(data->req.location) {
        curlx_free(newurl);
        newurl = data->req.location;
        data->req.location = عدم;
        result = multi_follow(data, handler, newurl, FOLLOW_FAKE);
        إذا(result) {
          *stream_errorp = TRUE;
          result = multi_done(data, result, TRUE);
        }
      }

      إذا(!result) {
        multistate(data, MSTATE_DONE);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
  }
  وإلا { /* not errored, not done */
    mspeed_check(data);
  }
  curlx_free(newurl);
  *resultp = result;
  أرجع mresult;
}

ساكن CURLMcode multistate_do(بنية Curl_easy *data,
                               bool *stream_errorp,
                               CURLcode *resultp)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;
  إذا(data->set.fprereq) {
    بنية Curl_mapi_guard guard;
    صحيح prereq_rc;

    /* call the prerequest callback function */
    CURL_CBAPI_START(&guard, data, easy_fprereq);
    prereq_rc = data->set.fprereq(data->set.prereq_userp,
                                  data->info.primary.remote_ip,
                                  data->info.primary.local_ip,
                                  data->info.primary.remote_port,
                                  data->info.primary.local_port);
    CURL_CBAPI_END(&guard);
    إذا(prereq_rc != CURL_PREREQFUNC_OK) {
      failf(data, "operation aborted by pre-request callback");
      /* failure in pre-request callback - do not do any other processing */
      result = CURLE_ABORTED_BY_CALLBACK;
      multi_posttransfer(data);
      multi_done(data, result, FALSE);
      *stream_errorp = TRUE;
      اذهب end;
    }
  }

  إذا(data->set.connect_only && !data->set.connect_only_ws) {
    multistate(data, MSTATE_DONE);
    mresult = CURLM_CALL_MULTI_PERFORM;
  }
  وإلا {
    bool dophase_done = FALSE;
    /* Perform the protocol's DO action */
    result = multi_do(data, &dophase_done);

    إذا(!result) {
      إذا(!dophase_done) {
#ifndef CURL_DISABLE_FTP
        /* some steps needed for wildcard matching */
        إذا(data->state.wildcardmatch) {
          بنية WildcardData *wc = data->wildcard;
          إذا(wc->state == CURLWC_DONE || wc->state == CURLWC_SKIP) {
            /* skip some states if it is important */
            multi_done(data, CURLE_OK, FALSE);

            /* if there is no connection left, skip the DONE state */
            multistate(data, data->conn ? MSTATE_DONE : MSTATE_COMPLETED);
            mresult = CURLM_CALL_MULTI_PERFORM;
            اذهب end;
          }
        }
#endif
        /* DO was not completed in one function call, we must continue
           DOING... */
        multistate(data, MSTATE_DOING);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }

      /* after DO, go DO_DONE... or DO_MORE */
      وإلا إذا(data->conn->bits.do_more) {
        /* we are supposed to do more, but we need to sit down, relax and wait
           a little while first */
        multistate(data, MSTATE_DOING_MORE);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
      وإلا {
        /* we are done with the DO, now DID */
        multistate(data, MSTATE_DID);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    وإلا إذا((result == CURLE_SEND_ERROR) &&
            data->conn->bits.reuse) {
      /*
       * In this situation, a connection that we were trying to use may have
       * unexpectedly died. If possible, send the connection back to the
       * CONNECT phase so we can try again.
       */
      ثابت بنية Curl_scheme *handler = data->conn->scheme;
      حرف *newurl = عدم;
      followtype follow = FOLLOW_NONE;
      CURLcode drc;

      drc = Curl_retry_request(data, &newurl);
      إذا(drc) {
        /* a failure here pretty much implies an out of memory */
        result = drc;
        *stream_errorp = TRUE;
      }

      multi_posttransfer(data);
      drc = multi_done(data, result, FALSE);

      /* When set to retry the connection, we must go back to the CONNECT
       * state */
      إذا(newurl) {
        إذا(!drc || (drc == CURLE_SEND_ERROR)) {
          follow = FOLLOW_RETRY;
          drc = multi_follow(data, handler, newurl, follow);
          إذا(!drc) {
            multistate(data, MSTATE_SETUP);
            mresult = CURLM_CALL_MULTI_PERFORM;
            result = CURLE_OK;
          }
          وإلا {
            /* Follow failed */
            result = drc;
          }
        }
        وإلا {
          /* done did not return OK or SEND_ERROR */
          result = drc;
        }
      }
      وإلا {
        /* Have error handler disconnect conn if we cannot retry */
        *stream_errorp = TRUE;
      }
      curlx_free(newurl);
    }
    وإلا {
      /* failure detected */
      multi_posttransfer(data);
      إذا(data->conn)
        multi_done(data, result, FALSE);
      *stream_errorp = TRUE;
    }
  }
end:
  *resultp = result;
  أرجع mresult;
}

ساكن CURLMcode multistate_ratelimiting(بنية Curl_easy *data,
                                         CURLcode *resultp)
{
  CURLcode result = CURLE_OK;
  CURLMcode mresult = CURLM_OK;
  DEBUGASSERT(data->conn);
  /* if both rates are within spec, resume transfer */
  result = Curl_pgrsCheck(data);

  إذا(result) {
    إذا(!(data->conn->scheme->flags & PROTOPT_DUAL) &&
       result != CURLE_HTTP2_STREAM)
      streamclose(data->conn);

    multi_posttransfer(data);
    multi_done(data, result, TRUE);
  }
  وإلا {
    إذا(!mspeed_check(data))
      mresult = CURLM_CALL_MULTI_PERFORM;
  }
  *resultp = result;
  أرجع mresult;
}

ساكن CURLMcode multistate_connect(بنية Curl_multi *multi,
                                    بنية Curl_easy *data,
                                    CURLcode *resultp)
{
  /* Connect. We want to get a connection identifier filled in. This state can
     be entered from SETUP and from PENDING. */
  bool connected;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = Curl_connect(data, &connected);
  إذا(result == CURLE_NO_CONNECTION_AVAILABLE) {
    /* There was no connection available. We will go to the pending state and
       wait for an available connection. */
    multistate(data, MSTATE_PENDING);
    /* move from process to pending set */
    Curl_uint32_bset_remove(&multi->process, data->mid);
    Curl_uint32_bset_remove(&multi->dirty, data->mid);
    Curl_uint32_bset_add(&multi->pending, data->mid);
    *resultp = CURLE_OK;
    أرجع mresult;
  }
  وإلا
    multi_schedule_pending(data->multi);

  إذا(!result) {
    /* after the connect has been sent off, go WAITCONNECT unless the
       protocol connect is already done and we can go directly to WAITDO or
       DO! */
    mresult = CURLM_CALL_MULTI_PERFORM;

    إذا(connected) {
      إذا(!data->conn->bits.reuse &&
         Curl_conn_is_multiplex(data->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_schedule_pending(data->multi);
      }
      multistate(data, MSTATE_PROTOCONNECT);
    }
    وإلا {
      multistate(data, MSTATE_CONNECTING);
    }
  }
  *resultp = result;
  أرجع mresult;
}

/* returns the possibly updated result */
ساكن CURLcode is_finished(بنية Curl_multi *multi,
                            بنية Curl_easy *data,
                            ثابت بنية curltime *pnow,
                            bool stream_error,
                            CURLcode result)
{
  إذا(data->mstate < MSTATE_COMPLETED) {
    إذا(result) {
      /*
       * If an error was returned, and we are not in completed state now,
       * then we go to completed and consider this transfer aborted.
       */

      /* No attempt to disconnect connections must be made before this -
         connection detach and termination happens only here */

      /* Check if we can move pending requests to send pipe */
      multi_schedule_pending(multi); /* connection */

      إذا(data->conn) {
        إذا(stream_error) {
          /* Do not attempt to send data over a connection that timed out */
          bool dead_connection = result == CURLE_OPERATION_TIMEDOUT;
          بنية connectdata *conn = data->conn;

          /* This is where we make sure that the conn pointer is reset.
             We do not have to do this in every case block above where a
             failure is detected */
          Curl_detach_connection(data);
          Curl_conn_close(data, conn, dead_connection);
        }
      }
      وإلا إذا(data->mstate == MSTATE_CONNECT) {
        /* Curl_connect() failed */
        multi_posttransfer(data);
        Curl_pgrsUpdate_nometer(data);
      }

      multistate(data, MSTATE_COMPLETED);
      أرجع result;
    }
    /* if there is still a connection to use, call the progress function */
    وإلا إذا(data->conn && Curl_conn_is_connected(data->conn, FIRSTSOCKET)) {
      result = Curl_pgrsUpdateX(data, pnow);
      إذا(result) {
        /* aborted due to progress callback return code must close the
           connection */
        streamclose(data->conn);

        /* if not yet in DONE state, go there. */
        إذا(data->mstate < MSTATE_DONE)
          multistate(data, MSTATE_DONE);
        أرجع result;
      }
    }
  }
  أرجع result;
}

ساكن فراغ handle_completed(بنية Curl_multi *multi,
                             بنية Curl_easy *data,
                             CURLcode result)
{
  bool ڤسنن_to_app = data->master_mid == UINT32_MAX;

  إذا(!ڤسنن_to_app) {
    /* A sub transfer, not reported to the application. Is anyone still
     * interested in processing its results? */
    إذا(data->sub_xfer_done) {
      بنية Curl_easy *master = Curl_multi_get_easy(multi, data->master_mid);

      CURL_TRC_M(data, "sub xfer done for master %u", data->master_mid);
      إذا(master)
        data->sub_xfer_done(data, master, result);
      وإلا
        CURL_TRC_M(data, "master easy %u already gone.", data->master_mid);
    }
  }
  وإلا {
    /* now fill in the CURLMsg with this info */
    بنية CURLMsg *ڤسنن = &data->ڤسنن;

    ڤسنن->ڤسنن = CURLMSG_DONE;
    ڤسنن->easy_handle = data;
    ڤسنن->data.result = result;

    DEBUGASSERT(!data->conn);
  }
  multistate(data, MSTATE_MSGSENT);

  /* remove from the other sets */
  Curl_uint32_bset_remove(&multi->process, data->mid);
  Curl_uint32_bset_remove(&multi->dirty, data->mid);
  Curl_uint32_bset_remove(&multi->pending, data->mid);
  إذا(ڤسنن_to_app)
    multi_addmsg(multi, data);
  إذا(data->state.really_alive) {
    data->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    إذا(!multi->xfers_really_alive)
      (فراغ)multi_assess_wakeup(multi);
  }
  --multi->xfers_alive;
  إذا(!multi->xfers_alive)
    multi_assess_wakeup(multi);
}

ساكن CURLMcode multistate_init(بنية Curl_easy *data, CURLcode *result)
{
  إذا(!data->state.really_alive) {
    data->state.really_alive = TRUE;
    ++data->multi->xfers_really_alive;
    إذا(data->multi->xfers_really_alive == 1) {
      CURLMcode mresult = multi_assess_wakeup(data->multi);
      إذا(mresult) {
        failf(data, "error enabling wakeup listening: %d", mresult);
        أرجع mresult;
      }
    }
  }

  *result = Curl_pretransfer(data);
  إذا(*result)
    أرجع CURLM_OK;

  /* after init, go SETUP */
  multistate(data, MSTATE_SETUP);
  Curl_pgrsTime(data, TIMER_STARTOP);
  أرجع CURLM_CALL_MULTI_PERFORM;
}

ساكن CURLMcode multistate_setup(بنية Curl_easy *data)
{
  ثابت بنية curltime *pnow = Curl_pgrs_now(data);
  Curl_pgrsTimeWas(data, TIMER_STARTSINGLE, *pnow);
  إذا(data->set.timeout)
    Curl_expire_set(data, EXPIRE_TIMEOUT, data->set.timeout, pnow);
  إذا(data->set.connecttimeout)
    /* Since a connection might go to pending and back to CONNECT several
       times before it actually takes off, we need to set the timeout once
       in SETUP before we enter CONNECT the first time. */
    Curl_expire_set(data, EXPIRE_CONNECTTIMEOUT,
                    data->set.connecttimeout, pnow);

  multistate(data, MSTATE_CONNECT);
  أرجع CURLM_CALL_MULTI_PERFORM;
}

ساكن CURLMcode multistate_connecting(بنية Curl_easy *data,
                                       bool *stream_error,
                                       CURLcode *result)
{
  bool connected;

  إذا(!data->conn) {
    DEBUGASSERT(0);
    *result = CURLE_FAILED_INIT;
    أرجع CURLM_OK;
  }
  إذا(!Curl_xfer_recv_is_paused(data)) {
    *result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &connected);
    إذا(connected && !*result) {
      إذا(!data->conn->bits.reuse &&
         Curl_conn_is_multiplex(data->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_schedule_pending(data->multi);
      }
      multistate(data, MSTATE_PROTOCONNECT);
      أرجع CURLM_CALL_MULTI_PERFORM;
    }
    وإلا إذا(*result) {
      /* failure detected */
      CURL_TRC_M(data, "connect failed -> %d", (صحيح)*result);
      multi_posttransfer(data);
      multi_done(data, *result, TRUE);
      *stream_error = TRUE;
      أرجع CURLM_OK;
    }
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_protoconnect(بنية Curl_easy *data,
                                         bool *stream_error,
                                         CURLcode *result)
{
  bool protocol_connected = FALSE;

  إذا(!*result && data->conn->bits.reuse) {
    /* ftp seems to hang when protoconnect on reused connection since we
     * handle PROTOCONNECT in general inside the filters, it seems wrong to
     * restart this on a reused connection.
     */
    multistate(data, MSTATE_DO);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
  إذا(!*result)
    *result = protocol_connect(data, &protocol_connected);
  إذا(!*result && !protocol_connected) {
    /* switch to waiting state */
    multistate(data, MSTATE_PROTOCONNECTING);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
  وإلا إذا(!*result) {
    /* protocol connect has completed, go WAITDO or DO */
    multistate(data, MSTATE_DO);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }

  /* failure detected */
  multi_posttransfer(data);
  multi_done(data, *result, TRUE);
  *stream_error = TRUE;
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_protoconnecting(بنية Curl_easy *data,
                                            bool *stream_error,
                                            CURLcode *result)
{
  bool protocol_connected = FALSE;

  /* protocol-specific connect phase */
  *result = protocol_connecting(data, &protocol_connected);
  إذا(!*result && protocol_connected) {
    /* after the connect has completed, go WAITDO or DO */
    multistate(data, MSTATE_DO);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
  وإلا إذا(*result) {
    /* failure detected */
    multi_posttransfer(data);
    multi_done(data, *result, TRUE);
    *stream_error = TRUE;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_doing(بنية Curl_easy *data,
                                  bool *stream_error,
                                  CURLcode *result)
{
  bool dophase_done = FALSE;

  /* we continue DOING until the DO phase is complete */
  DEBUGASSERT(data->conn);
  *result = protocol_doing(data, &dophase_done);
  إذا(!*result) {
    إذا(dophase_done) {
      /* after DO, go DO_DONE or DO_MORE */
      multistate(data, data->conn->bits.do_more ?
                 MSTATE_DOING_MORE : MSTATE_DID);
      أرجع CURLM_CALL_MULTI_PERFORM;
    } /* dophase_done */
  }
  وإلا {
    /* failure detected */
    multi_posttransfer(data);
    multi_done(data, *result, FALSE);
    *stream_error = TRUE;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_doing_more(بنية Curl_easy *data,
                                       bool *stream_error,
                                       CURLcode *result)
{
  domore control;

  /*
   * When we are connected, DOING MORE and then go DID
   */
  DEBUGASSERT(data->conn);
  *result = multi_do_more(data, &control);

  إذا(!*result) {
    إذا(control != DOMORE_INCOMPLETE) {
      /* if DONE, advance to DO_DONE
         if GOBACK, go back to DOING */
      multistate(data, control == DOMORE_DONE ? MSTATE_DID : MSTATE_DOING);
      أرجع CURLM_CALL_MULTI_PERFORM;
    }
    /* else
       stay in DO_MORE */
  }
  وإلا {
    /* failure detected */
    multi_posttransfer(data);
    multi_done(data, *result, FALSE);
    *stream_error = TRUE;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_did(بنية Curl_multi *multi,
                                بنية Curl_easy *data)
{
  DEBUGASSERT(data->conn);
  إذا(data->conn->bits.multiplex)
    /* Check if we can move pending requests to send pipe */
    multi_schedule_pending(multi); /* multiplexed */

  /* Only perform the transfer if there is a good socket to work with.
     Having both BAD is a signal to skip immediately to DONE */
  إذا(CONN_SOCK_IDX_VALID(data->conn->recv_idx) ||
     CONN_SOCK_IDX_VALID(data->conn->send_idx)) {
    multistate(data, MSTATE_PERFORMING);
    /* Do not return CURLM_CALL_MULTI_PERFORM to give other transfers
     * a chance to send off their requests.
     * Note: Some SFTP handlers do not seem to like this.
     *       Restrict it to HTTP families. */
    أرجع ((multi->xfers_alive > 1) &&
            (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) ?
           CURLM_OK : CURLM_CALL_MULTI_PERFORM;
  }
  وإلا {
#ifndef CURL_DISABLE_FTP
    إذا(data->state.wildcardmatch &&
       ((data->conn->scheme->flags & PROTOPT_WILDCARD) == 0)) {
      data->wildcard->state = CURLWC_DONE;
    }
#endif
    multistate(data, MSTATE_DONE);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
}

ساكن CURLMcode multistate_done(بنية Curl_easy *data, CURLcode *presult)
{
  إذا(data->conn) {
    CURLcode result;

    /* post-transfer command */
    result = multi_done(data, *presult, FALSE);

    /* allow a previously set error code take precedence */
    إذا(!(*presult))
      *presult = result;
  }

#ifndef CURL_DISABLE_FTP
  إذا(data->state.wildcardmatch) {
    إذا(data->wildcard->state != CURLWC_DONE) {
      /* if a wildcard is set and we are not ending -> lets start again
         with MSTATE_INIT */
      multistate(data, MSTATE_INIT);
      أرجع CURLM_CALL_MULTI_PERFORM;
    }
  }
#endif
  /* after we have DONE what we are supposed to do, go COMPLETED, and
     it does not matter what the multi_done() returned! */
  multistate(data, MSTATE_COMPLETED);
  أرجع CURLM_CALL_MULTI_PERFORM;
}

ساكن CURLMcode multi_runsingle(بنية Curl_multi *multi,
                                 بنية Curl_easy *data,
                                 بنية Curl_sigpipe_ctx *sigpipe_ctx)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;
  ثابت بنية curltime *pnow = عدم;

  إذا(multi->dead) {
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

  إذا(data == multi->admin) {
#ifdef ENABLE_INTERNAL_WAKEUP
    /* Consume any pending wakeup signals before processing.
     * This is necessary for event based processing. See #21547 */
    (فراغ)Curl_wakeup_consume(multi->wakeup_internal, TRUE);
#endif
#ifdef USE_RESOLV_THREADED
    Curl_async_thrdd_multi_process(multi);
#endif
    Curl_cshutdn_perform(&multi->cshutdn, multi->admin, sigpipe_ctx);
    اذهب out;
  }

  sigpipe_apply(data, sigpipe_ctx);
  افعل {
    /* A "stream" here is a logical stream if the protocol can handle that
       (HTTP/2), or the full connection for older protocols */
    bool stream_error = FALSE;
    mresult = CURLM_OK;
    pnow = عدم;

    إذا(multi_ischanged(multi, TRUE)) {
      CURL_TRC_M(data, "multi changed, check CONNECT_PEND queue");
      multi_schedule_pending(multi); /* multiplexed */
    }

    إذا(data->mstate > MSTATE_CONNECT &&
       data->mstate < MSTATE_COMPLETED) {
      /* Make sure we set the connection's current owner */
      DEBUGASSERT(data->conn);
      إذا(!data->conn) {
        mresult = CURLM_INTERNAL_ERROR;
        اذهب out;
      }
    }

    /* Wait for the connect state as only then is the start time stored, but
       we must not check already completed handles */
    إذا((data->mstate >= MSTATE_CONNECT) && (data->mstate < MSTATE_COMPLETED)) {
      pnow = Curl_pgrs_now(data);
      إذا(multi_handle_timeout(data, pnow, &stream_error, &result))
        /* Skip the statemachine and go directly to error handling section. */
        اذهب statemachine_end;
      pnow = عدم;
    }

    بدل(data->mstate) {
    حالة MSTATE_INIT:
      /* Transitional state. init this transfer. A handle never comes back to
         this state. */
      mresult = multistate_init(data, &result);
      اكسر;

    حالة MSTATE_SETUP:
      /* Transitional state. Setup things for a new transfer. The handle
         can come back to this state on a redirect. */
      mresult = multistate_setup(data);
      اكسر;

    حالة MSTATE_CONNECT:
      mresult = multistate_connect(multi, data, &result);
      اكسر;

    حالة MSTATE_CONNECTING:
      /* awaiting a completion of an asynch TCP connect */
      mresult = multistate_connecting(data, &stream_error, &result);
      اكسر;

    حالة MSTATE_PROTOCONNECT:
      mresult = multistate_protoconnect(data, &stream_error, &result);
      اكسر;

    حالة MSTATE_PROTOCONNECTING:
      /* protocol-specific connect phase */
      mresult = multistate_protoconnecting(data, &stream_error, &result);
      اكسر;

    حالة MSTATE_DO:
      mresult = multistate_do(data, &stream_error, &result);
      اكسر;

    حالة MSTATE_DOING:
      /* we continue DOING until the DO phase is complete */
      mresult = multistate_doing(data, &stream_error, &result);
      اكسر;

    حالة MSTATE_DOING_MORE:
      /*
       * When we are connected, DOING MORE and then go DID
       */
      mresult = multistate_doing_more(data, &stream_error, &result);
      اكسر;

    حالة MSTATE_DID:
      mresult = multistate_did(multi, data);
      اكسر;

    حالة MSTATE_RATELIMITING: /* limit-rate exceeded in either direction */
      mresult = multistate_ratelimiting(data, &result);
      اكسر;

    حالة MSTATE_PERFORMING:
      mresult = multistate_performing(data, &stream_error, &result);
      اكسر;

    حالة MSTATE_DONE:
      mresult = multistate_done(data, &result);
      اكسر;

    حالة MSTATE_COMPLETED:
      اكسر;

    حالة MSTATE_PENDING:
    حالة MSTATE_MSGSENT:
      /* handles in these states should NOT be in this list */
      اكسر;

    افتراضي:
      mresult = CURLM_INTERNAL_ERROR;
      اذهب out;
    }

    إذا(data->mstate >= MSTATE_CONNECT &&
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
    إذا(!pnow)
      pnow = Curl_pgrs_now(data);
    result = is_finished(multi, data, pnow, stream_error, result);
    إذا(result)
      mresult = CURLM_CALL_MULTI_PERFORM;

    إذا(MSTATE_COMPLETED == data->mstate) {
      handle_completed(multi, data, result);
      mresult = CURLM_OK;
      اذهب out;
    }
  } طالما((mresult == CURLM_CALL_MULTI_PERFORM) ||
          multi_ischanged(multi, FALSE));

out:
  data->result = result;
  أرجع mresult;
}

ساكن CURLMcode multi_perform(بنية Curl_multi *multi,
                               صحيح *running_handles)
{
  CURLMcode returncode = CURLM_OK;
  بنية curltime start;
  uint32_t mid;
  بنية Curl_sigpipe_ctx sigpipe_ctx;

  curlx_pnow(&start);
  sigpipe_init(&sigpipe_ctx);

  إذا(Curl_uint32_bset_first(&multi->process, &mid)) {
    CURL_TRC_M(multi->admin, "multi_perform(running=%u)",
               multi_xfers_running(multi));
    افعل {
      بنية Curl_easy *data = Curl_multi_get_easy(multi, mid);
      CURLMcode mresult;
      إذا(!data) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        استمر;
      }
      mresult = multi_runsingle(multi, data, &sigpipe_ctx);
      إذا(mresult)
        returncode = mresult;
    } طالما(Curl_uint32_bset_next(&multi->process, mid, &mid));
  }
  sigpipe_restore(&sigpipe_ctx);

  إذا(multi_ischanged(multi, TRUE))
    multi_schedule_pending(multi);

  إذا(!returncode && CURL_MNTFY_HAS_ENTRIES(multi))
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
  طالما(Curl_timeouts_remove_expired(&multi->timeouts, &start, &mid)) {
    /* the removed may have another timeout in queue */
    بنية Curl_easy *data = Curl_multi_get_easy(multi, mid);
    إذا(!data) {
      DEBUGASSERT(0);
      استمر;
    }
    (فراغ)add_next_timeout(&start, multi, data);
    إذا(data->mstate == MSTATE_PENDING) {
      bool stream_unused;
      CURLcode result_unused;
      إذا(multi_handle_timeout(data, Curl_pgrs_now(data),
                              &stream_unused, &result_unused)) {
        infof(data, "PENDING handle timeout");
        move_pending_to_connect(multi, data);
      }
    }
  }

  إذا(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < INT_MAX) ? (صحيح)running : INT_MAX;
  }

  إذا(CURLM_OK >= returncode)
    returncode = Curl_update_timer(multi);

  أرجع returncode;
}

CURLMcode curl_multi_perform(CURLM *m, صحيح *running_handles)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_perform, &mresult)) {
    mresult = multi_perform(m, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

CURLMcode curl_multi_cleanup(CURLM *m)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_cleanup, &mresult)) {
    بنية Curl_multi *multi = m;
    فراغ *entry;
    uint32_t mid;

    /* First remove all remaining easy handles,
     * close internal ones. admin handle is special */
    إذا(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
      افعل {
        بنية Curl_easy *data = entry;
        إذا(!GOOD_EASY_HANDLE(data)) {
          mresult = CURLM_BAD_HANDLE;
          اذهب out;
        }

#ifdef DEBUGBUILD
        إذا(mid != data->mid) {
          CURL_TRC_M(data, "multi_cleanup: still present with mid=%u, "
                     "but unexpected data->mid=%u\n", mid, data->mid);
          DEBUGASSERT(0);
        }
#endif

        إذا(data == multi->admin)
          استمر;

        إذا(!data->state.done && data->conn)
          /* if DONE was never called for this handle */
          (فراغ)multi_done(data, CURLE_OK, TRUE);

        data->multi = عدم; /* clear the association */
        Curl_uint32_tbl_remove(&multi->xfers, mid);
        data->mid = UINT32_MAX;

#ifdef USE_LIBPSL
        إذا(data->psl == &multi->psl)
          data->psl = عدم;
#endif
        إذا(data->state.internal)
          Curl_close(&data);
      } طالما(Curl_uint32_tbl_next(&multi->xfers, mid, &mid, &entry));
    }

#ifdef USE_RESOLV_THREADED
    Curl_async_thrdd_multi_destroy(multi, !multi->quick_exit);
#endif
    Curl_cpool_destroy(&multi->cpool, multi->admin);
    Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
    إذا(multi->admin) {
      CURL_TRC_M(multi->admin, "multi_cleanup, closing admin handle, done");
      multi->admin->multi = عدم;
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
    إذا(Curl_uint32_tbl_کيراءن(&multi->xfers)) {
      multi_xfer_tbl_dump(multi);
      DEBUGASSERT(0);
    }
#endif
    Curl_uint32_bset_destroy(&multi->process);
    Curl_uint32_bset_destroy(&multi->dirty);
    Curl_uint32_bset_destroy(&multi->pending);
    Curl_uint32_bset_destroy(&multi->msgsent);
    Curl_uint32_tbl_destroy(&multi->xfers);
    curlx_memzero(multi, حجم(*multi));
    curlx_free(multi);

    mresult = CURLM_OK;
  }
out:
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

/*
 * curl_multi_info_read()
 *
 * This function is the primary way for a multi/multi_socket application to
 * figure out if a transfer has ended.
 */

CURLMsg *curl_multi_info_read(CURLM *m, صحيح *msgs_in_queue)
{
  بنية Curl_mapi_guard guard;
  CURLMsg *ڤسنن_result = عدم;

  *msgs_in_queue = 0; /* default to none */
  إذا(CURL_MAPI_ENTER(&guard, m, multi_info_read, عدم)) {
    بنية Curl_multi *multi = m;
    uint32_t mid;
    إذا(Curl_uint32_bset_first(&multi->msgsent, &mid)) {
      بنية Curl_easy *data = Curl_multi_get_easy(multi, mid);

      DEBUGASSERT(data);
      Curl_uint32_bset_remove(&multi->msgsent, mid);
      *msgs_in_queue =
        curlx_uztosi(Curl_uint32_bset_کيراءن(&multi->msgsent));
      إذا(data)
        ڤسنن_result = &data->ڤسنن;
    }
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع ڤسنن_result;
}

فراغ Curl_multi_will_close(بنية Curl_easy *data, curl_socket_t s)
{
  إذا(data) {
    بنية Curl_multi *multi = data->multi;
    إذا(multi) {
      CURL_TRC_M(data, "Curl_multi_will_close fd=%" FMT_SOCKET_T, s);
      Curl_multi_ev_socket_done(multi, data, s);
    }
  }
}

ساكن فراغ multi_timeouts_init(بنية Curl_easy *data)
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
ساكن CURLMcode add_next_timeout(ثابت بنية curltime *pnow,
                                  بنية Curl_multi *multi,
                                  بنية Curl_easy *data)
{
  بنية expire_timers *timeouts = &data->state.timeouts;
  timediff_t now_us = Curl_timeouts_offset_us(&multi->timeouts, pnow);

  طالما(timeouts->first < EXPIRE_LAST) {
    إذا(timeouts->offset_us[timeouts->first] <= now_us)  /* already expired */
      timeouts->first = timeouts->next[timeouts->first];
    وإلا /* timeouts are sorted, first is first in the future now */
      اكسر;
  }

  إذا(timeouts->first < EXPIRE_LAST) {
    /* Insert this node again into the splay. Keep the timer in the list in
       case we need to recompute future timers. */
    Curl_timeouts_add(&multi->timeouts, data,
                      timeouts->offset_us[timeouts->first]);
  }
  أرجع CURLM_OK;
}

ساكن فراغ multi_mark_expired_as_dirty(بنية Curl_multi *multi,
                                        ثابت بنية curltime *ts)
{
  بنية Curl_easy *data = عدم;
  uint32_t mid;

  /*
   * The loop following here will go on as long as there are expire-times left
   * to process (compared to `ts`) in the splay and 'data' will be
   * re-assigned for every expired handle we deal with.
   */
  طالما(Curl_timeouts_remove_expired(&multi->timeouts, ts, &mid)) {
    /* Check if there is one (more) expired timer to deal with! This function
       extracts a matching node if there is one */
    data = Curl_multi_get_easy(multi, mid);
    إذا(!data) {
      DEBUGASSERT(0);
      استمر;
    }
#ifdef CURLVERBOSE
    إذا(CURL_TRC_TIMER_is_verbose(data)) {
      إذا(data->state.timeouts.first < EXPIRE_LAST) {
        CURL_TRC_TIMER(data, data->state.timeouts.first, "has expired");
      }
    }
#endif
    (فراغ)add_next_timeout(ts, multi, data);
    Curl_multi_mark_dirty(data);
  }
}

ساكن CURLMcode multi_run_dirty(بنية Curl_multi *multi,
                                 بنية Curl_sigpipe_ctx *sigpipe_ctx,
                                 uint32_t *pnum)
{
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;

  *pnum = 0;
  إذا(Curl_uint32_bset_first(&multi->dirty, &mid)) {
    افعل {
      بنية Curl_easy *data = Curl_multi_get_easy(multi, mid);
      إذا(data) {
        CURL_TRC_M(data, "multi_run_dirty");

        إذا(!Curl_uint32_bset_contains(&multi->process, mid)) {
          إذا(Curl_uint32_bset_contains(&multi->pending, mid)) {
            /* Something happened on the pending transfer, act on it. */
            move_pending_to_connect(multi, data);
          }
          وإلا {
            /* We are no longer processing this transfer */
            Curl_uint32_bset_remove(&multi->dirty, mid);
            استمر;
          }
        }

        (*pnum)++;
        /* runsingle() clears the dirty mid */
        mresult = multi_runsingle(multi, data, sigpipe_ctx);

        إذا(CURLM_OK >= mresult) {
          /* reassess event handling of data */
          mresult = Curl_multi_ev_assess_xfer(multi, data);
          إذا(mresult)
            اذهب out;
        }
      }
      وإلا {
        CURL_TRC_M(multi->admin, "multi_run_dirty, %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } طالما(Curl_uint32_bset_next(&multi->dirty, mid, &mid));
  }

out:
  أرجع mresult;
}

ساكن CURLMcode multi_socket(بنية Curl_multi *multi,
                              bool checkall,
                              curl_socket_t s,
                              صحيح ev_bitmask,
                              صحيح *running_handles)
{
  CURLMcode mresult = CURLM_OK;
  بنية Curl_sigpipe_ctx pipe_ctx;
  بنية curltime now;
  uint32_t run_xfers;

  (فراغ)ev_bitmask;
  sigpipe_init(&pipe_ctx);

  إذا(checkall) {
    /* *perform() deals with running_handles on its own */
    mresult = multi_perform(multi, running_handles);

    إذا(mresult != CURLM_BAD_HANDLE) {
      /* Reassess event status of all active transfers, but keep any error
         from multi_perform() */
      CURLMcode mresult2 =
        Curl_multi_ev_assess_xfer_bset(multi, &multi->process);
      إذا(!mresult)
        mresult = mresult2;
    }
    اذهب out;
  }

  إذا(s != CURL_SOCKET_TIMEOUT) {
    /* Mark all transfers of that socket as dirty */
    Curl_multi_ev_dirty_xfers(multi, s);
  }
  وإلا {
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
    إذا(mresult)
      اذهب out;
  }

  curlx_pnow(&now);
  multi_mark_expired_as_dirty(multi, &now);
  mresult = multi_run_dirty(multi, &pipe_ctx, &run_xfers);
  إذا(mresult)
    اذهب out;

  إذا(run_xfers) {
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

  إذا(multi_ischanged(multi, TRUE))
    multi_schedule_pending(multi);

  إذا(!mresult && CURL_MNTFY_HAS_ENTRIES(multi))
    mresult = Curl_mntfy_dispatch_all(multi);

  إذا(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < INT_MAX) ? (صحيح)running : INT_MAX;
  }

  إذا(CURLM_OK >= mresult)
    mresult = Curl_update_timer(multi);
  أرجع mresult;
}

#undef curl_multi_setopt
CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_setopt, &mresult)) {
    بنية Curl_multi *multi = m;
    va_list param;
    unsigned long uarg;
    size_t szarg;

    va_start(param, option);

    بدل(option) {
    حالة CURLMOPT_SOCKETFUNCTION:
      multi->socket_cb = va_arg(param, curl_socket_callback);
      اكسر;
    حالة CURLMOPT_SOCKETDATA:
      multi->socket_userp = va_arg(param, فراغ *);
      اكسر;
    حالة CURLMOPT_PUSHFUNCTION:
      multi->تولق_cb = va_arg(param, curl_تولق_callback);
      اكسر;
    حالة CURLMOPT_PUSHDATA:
      multi->تولق_userp = va_arg(param, فراغ *);
      اكسر;
    حالة CURLMOPT_PIPELINING:
      multi->multiplexing = va_arg(param, long) & CURLPIPE_MULTIPLEX ? 1 : 0;
      اكسر;
    حالة CURLMOPT_TIMERFUNCTION:
      multi->timer_cb = va_arg(param, curl_multi_timer_callback);
      اكسر;
    حالة CURLMOPT_TIMERDATA:
      multi->timer_userp = va_arg(param, فراغ *);
      اكسر;
    حالة CURLMOPT_MAXCONNECTS:
      uarg = va_arg(param, unsigned long);
      إذا(uarg <= UINT32_MAX)
        multi->maxconnects = (uint32_t)uarg;
      اكسر;
    حالة CURLMOPT_MAX_HOST_CONNECTIONS:
      إذا(!curlx_sltouz(va_arg(param, long), &szarg))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      multi->max_host_connections = (szarg < UINT32_MAX) ?
                                    (uint32_t)szarg : UINT32_MAX;
      اكسر;
    حالة CURLMOPT_MAX_TOTAL_CONNECTIONS:
      إذا(!curlx_sltouz(va_arg(param, long), &szarg))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      multi->max_total_connections = (szarg < UINT32_MAX) ?
                                     (uint32_t)szarg : UINT32_MAX;
      اكسر;
      /* options formerly used for pipelining */
    حالة CURLMOPT_MAX_PIPELINE_LENGTH:
      اكسر;
    حالة CURLMOPT_CONTENT_LENGTH_PENALTY_SIZE:
      اكسر;
    حالة CURLMOPT_CHUNK_LENGTH_PENALTY_SIZE:
      اكسر;
    حالة CURLMOPT_PIPELINING_SITE_BL:
      اكسر;
    حالة CURLMOPT_PIPELINING_SERVER_BL:
      اكسر;
    حالة CURLMOPT_MAX_CONCURRENT_STREAMS: {
      إذا(!curlx_sltouz(va_arg(param, long), &szarg) ||
         !szarg || (szarg > (size_t)INT_MAX)) /* preserve previous cutoff */
        multi->max_concurrent_streams = 100;
      وإلا
        multi->max_concurrent_streams = (szarg < UINT32_MAX) ?
                                       (uint32_t)szarg : UINT32_MAX;
      اكسر;
    }
    حالة CURLMOPT_NETWORK_CHANGED: {
      long val = va_arg(param, long);
      إذا(val & CURLMNWC_CLEAR_ALL)
        /* In the beginning, all values available to set were 1 by mistake. We
           converted this to mean "all", thus setting all the bits
           automatically */
        val = CURLMNWC_CLEAR_DNS | CURLMNWC_CLEAR_CONNS;
      إذا(val & CURLMNWC_CLEAR_DNS) {
        Curl_dnscache_clear(multi->admin);
      }
      إذا(val & CURLMNWC_CLEAR_CONNS) {
        Curl_cpool_nw_changed(&multi->cpool, multi->admin);
      }
      اكسر;
    }
    حالة CURLMOPT_NOTIFYFUNCTION:
      multi->ntfy.ntfy_cb = va_arg(param, curl_notify_callback);
      اكسر;
    حالة CURLMOPT_NOTIFYDATA:
      multi->ntfy.ntfy_cb_data = va_arg(param, فراغ *);
      اكسر;
    حالة CURLMOPT_RESOLVE_THREADS_MAX:
#ifdef USE_RESOLV_THREADED
      uarg = va_arg(param, long);
      إذا((uarg <= 0) || (uarg > UINT32_MAX))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      وإلا {
        CURLcode result = Curl_async_thrdd_multi_set_props(
          multi, 0, (uint32_t)uarg, 2000);
        بدل(result) {
        حالة CURLE_OK:
          mresult = CURLM_OK;
          اكسر;
        حالة CURLE_BAD_FUNCTION_ARGUMENT:
          mresult = CURLM_BAD_FUNCTION_ARGUMENT;
          اكسر;
        حالة CURLE_OUT_OF_MEMORY:
          mresult = CURLM_OUT_OF_MEMORY;
          اكسر;
        افتراضي:
          mresult = CURLM_INTERNAL_ERROR;
          اكسر;
        }
      }
#endif
      اكسر;
    حالة CURLMOPT_QUICK_EXIT:
      multi->quick_exit = va_arg(param, long) ? 1 : 0;
      اكسر;
    افتراضي:
      mresult = CURLM_UNKNOWN_OPTION;
      اكسر;
    }
    va_end(param);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

/* we define curl_multi_socket() in the public multi.h header */
#undef curl_multi_socket

CURLMcode curl_multi_socket(CURLM *m, curl_socket_t s, صحيح *running_handles)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_socket, &mresult)) {
    mresult = multi_socket(m, FALSE, s, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

CURLMcode curl_multi_socket_action(CURLM *m, curl_socket_t s,
                                   صحيح ev_bitmask, صحيح *running_handles)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_socket_action, &mresult)) {
    mresult = multi_socket(m, FALSE, s, ev_bitmask, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

CURLMcode curl_multi_socket_all(CURLM *m, صحيح *running_handles)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_socket_all, &mresult)) {
    mresult = multi_socket(m, TRUE, CURL_SOCKET_BAD, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

ساكن bool multi_has_dirties(بنية Curl_multi *multi)
{
  uint32_t mid;
  إذا(Curl_uint32_bset_first(&multi->dirty, &mid)) {
    افعل {
      بنية Curl_easy *data = Curl_multi_get_easy(multi, mid);
      إذا(data) {
        إذا(Curl_uint32_bset_contains(&multi->process, mid))
          أرجع TRUE;
        /* We are no longer processing this transfer */
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
      وإلا {
        CURL_TRC_M(multi->admin, "dirty transfer %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } طالما(Curl_uint32_bset_next(&multi->dirty, mid, &mid));
  }
  أرجع FALSE;
}

ساكن فراغ multi_timeout(بنية Curl_multi *multi,
                          timediff_t *pexire_offset_us,
                          صحيح *timeout_ms)
{
  إذا(multi->dead) {
    إذا(pexire_offset_us)
      *pexire_offset_us = 0;
    *timeout_ms = 0;
    أرجع;
  }

  إذا(multi_has_dirties(multi)) {
    إذا(pexire_offset_us) {
      بنية curltime now;
      curlx_pnow(&now);
      *pexire_offset_us = Curl_timeouts_offset_us(&multi->timeouts, &now);
    }
    *timeout_ms = 0;
    أرجع;
  }
  وإلا {
    بنية curltime now;
    uint32_t mid;

    curlx_pnow(&now);
    *timeout_ms = Curl_timeouts_next_ms(&multi->timeouts, &now,
                                        pexire_offset_us, &mid);
#ifdef CURLVERBOSE
    إذا(mid != UINT32_MAX) {
      بنية Curl_easy *data = Curl_multi_get_easy(multi, mid);
      إذا(data && CURL_TRC_TIMER_is_verbose(data) &&
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
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_timeout, &mresult)) {
    صحيح itimeout_ms;

    multi_timeout(m, عدم, &itimeout_ms);
    *timeout_ms = (long)itimeout_ms;
    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

/*
 * Tell the application it should update its timers, if it subscribes to the
 * update timer callback.
 */
CURLMcode Curl_update_timer(بنية Curl_multi *multi)
{
  timediff_t timeouts_offset_us = 0;
  صحيح timeout_ms;
  صحيح rc;
  bool set_نيلاي = FALSE;

  إذا(!multi->timer_cb || multi->dead)
    أرجع CURLM_OK;
  multi_timeout(multi, &timeouts_offset_us, &timeout_ms);

  إذا(timeout_ms < 0 && !multi->last_timeout_set) {
    /* nothing to do */
  }
  وإلا إذا(timeout_ms < 0) {
    /* there is no timeout now but there was one previously */
    CURL_TRC_M(multi->admin, "[TIMER] clear");
    timeout_ms = -1; /* normalize */
    set_نيلاي = TRUE;
  }
  وإلا إذا(!multi->last_timeout_set) {
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, none before", timeout_ms);
    set_نيلاي = TRUE;
  }
  وإلا إذا(multi->last_expire_offset_us != timeouts_offset_us) {
    /* We had a timeout before and have one now, the absolute timestamp
     * differs. The relative timeout_ms may be the same, but the starting
     * point differs. Let the application restart its timer. */
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, replace previous",
               timeout_ms);
    set_نيلاي = TRUE;
  }
  وإلا {
    /* We have same expire time as previously. Our relative 'timeout_ms'
     * may be different now, but the application has the timer running
     * and we do not to tell it to start this again. */
  }

  إذا(set_نيلاي) {
    بنية Curl_mapi_guard guard;

    multi->last_expire_offset_us = timeouts_offset_us;
    multi->last_timeout_set = timeout_ms >= 0;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_timer_cb);
    rc = multi->timer_cb(multi, timeout_ms, multi->timer_userp);
    CURL_CBAPI_MULTI_END(&guard);
    إذا(rc == -1) {
      multi->dead = TRUE;
      أرجع CURLM_ABORTED_BY_CALLBACK;
    }
  }
  أرجع CURLM_OK;
}

#ifdef DEBUGBUILD
ساكن bool multi_timeouts_check(بنية Curl_easy *data)
{
  بنية expire_timers *timeouts = &data->state.timeouts;
  uint8_t id;
  صحيح i = 0;
  لكل(id = timeouts->first; id < EXPIRE_LAST; id = timeouts->next[id]) {
    إذا(++i >= EXPIRE_LAST) {
      failf(data, "expire timeouts looped: %d iterations and no end", i);
      أرجع FALSE;
    }
    إذا(id == timeouts->next[id]) {
      failf(data, "expire timeouts wrong: %d points to itself", (صحيح)id);
      أرجع FALSE;
    }
    إذا((timeouts->next[id] < EXPIRE_LAST) &&
       (timeouts->offset_us[id] > timeouts->offset_us[timeouts->next[id]])) {
      failf(data, "expire timeouts not sorted: %d happens after %d but "
            "is listed before", (صحيح)id, (صحيح)timeouts->next[id]);
      أرجع FALSE;
    }
  }
  أرجع TRUE;
}
#endif

/*
 * Remove a given timestamp from the list of timeouts.
 */
ساكن فراغ multi_clear_timeout(بنية Curl_easy *data, expire_id eid)
{
  بنية expire_timers *timeouts = &data->state.timeouts;
  uint8_t orig_first = timeouts->first;
  uint8_t *anchor = &timeouts->first;
  uint8_t id = (uint8_t)eid;

  إذا((unsigned)eid >= EXPIRE_LAST) {
    DEBUGASSERT(0);
    أرجع;
  }

  طالما(*anchor < EXPIRE_LAST) {
    إذا(*anchor == id) {
      *anchor = timeouts->next[id];
      اكسر;
    }
    anchor = &timeouts->next[*anchor];
  }
  DEBUGASSERT(multi_timeouts_check(data));
  إذا(Curl_timeouts_has(data)) {
    بنية Curl_multi *multi = data->multi;

    إذا(!multi) {
      DEBUGASSERT(0);
      أرجع;
    }
    إذا((timeouts->first >= EXPIRE_LAST) || /* no more timeouts */
       (timeouts->first != orig_first)) {  /* active timeout changed */
      Curl_timeouts_remove(&multi->timeouts, data);
    }
    إذا((timeouts->first < EXPIRE_LAST) && !Curl_timeouts_has(data)) {
      Curl_timeouts_add(&multi->timeouts, data,
                        timeouts->offset_us[timeouts->first]);
    }
  }
}

/*
 * Add a timestamp to the list of timeouts. Keep the list sorted so that head
 * of list is always the timeout nearest in time.
 */
ساكن CURLMcode multi_set_timeout(بنية Curl_easy *data,
                                   ثابت بنية curltime *stamp,
                                   expire_id eid)
{
  بنية expire_timers *timeouts = &data->state.timeouts;
  uint8_t *anchor = &timeouts->first;
  uint8_t id = (uint8_t)eid;

  إذا((unsigned)eid >= EXPIRE_LAST) {
    DEBUGASSERT(0);
    أرجع CURLM_BAD_FUNCTION_ARGUMENT;
  }
  /* remove from list, store time and re-insert */
  multi_clear_timeout(data, eid);
  timeouts->offset_us[id] =
    Curl_timeouts_offset_us(&data->multi->timeouts, stamp);

  طالما(*anchor < EXPIRE_LAST) {
    إذا(timeouts->offset_us[*anchor] > timeouts->offset_us[id])
      اكسر;
    anchor = &timeouts->next[*anchor];
  }
  timeouts->next[eid] = *anchor;
  timeouts->next[eid] = *anchor;
  *anchor = id;
  DEBUGASSERT(multi_timeouts_check(data));
  CURL_TRC_TIMER(data, eid, "set for %" FMT_TIMEDIFF_T "us",
                 curlx_ptimediff_us(stamp, Curl_pgrs_now(data)));
  أرجع CURLM_OK;
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
فراغ Curl_expire_set(بنية Curl_easy *data,
                     expire_id eid, timediff_t ms,
                     ثابت بنية curltime *pnow)
{
  بنية Curl_multi *multi = data->multi;
  بنية expire_timers *timeouts = &data->state.timeouts;
  uint8_t prev_id = timeouts->first;
  بنية curltime set;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  إذا(!multi)
    أرجع;
  DEBUGASSERT(eid < EXPIRE_LAST);
  إذا(ms > INT_MAX)
    /* Cap ridiculous timeouts, 31-bit ms is still 3.5 weeks. When the time
       goes to the user, it must fit in this size. */
    ms = INT_MAX;

  set = *pnow;
  set.tv_sec += (time_t)(ms / 1000); /* may be a 64 to 32-bit conversion */
  set.tv_usec += (صحيح)(ms % 1000) * 1000;
  إذا(set.tv_usec >= 1000000) {
    set.tv_sec++;
    set.tv_usec -= 1000000;
  }

  /* Add the timeout, will replace any previous value for this timer. */
  multi_set_timeout(data, &set, eid);
  DEBUGASSERT(timeouts->first < EXPIRE_LAST);

  إذا(Curl_timeouts_has(data)) {
    /* data has already a timeout registered. If the first timer
     * was NOT the one we just set AND is still the first one,
     * nothing changed from the timeouts point of view. The
     * set timer triggers after the one already registered. Leave. */
    إذا((prev_id != eid) && (prev_id == timeouts->first))
      أرجع;

    /* Since this is an updated time, we must remove data from
     * timeouts and then add it again. */
    Curl_timeouts_remove(&multi->timeouts, data);
  }

  /* Insert the new timer expiry since it is our local minimum. */
  Curl_timeouts_add(&multi->timeouts, data,
                    timeouts->offset_us[timeouts->first]);
}

فراغ Curl_expire(بنية Curl_easy *data,
                 timediff_t milli, expire_id eid)
{
  Curl_expire_set(data, eid, milli, Curl_pgrs_now(data));
}

/*
 * Removes the expire timer. Marks it as done.
 */
فراغ Curl_expire_clear(بنية Curl_easy *data, expire_id eid)
{
  /* remove the timer, if there */
  multi_clear_timeout(data, eid);
  CURL_TRC_TIMER(data, eid, "cleared");
}

/*
 * Clear ALL timeout values for this handle.
 */
فراغ Curl_expire_clear_all(بنية Curl_easy *data)
{
  بنية Curl_multi *multi = data->multi;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  إذا(!multi)
    أرجع;

  إذا(Curl_timeouts_remove(&multi->timeouts, data)) {
    /* Since this is an cleared time, we must remove the previous entry from
       the splay tree */
    multi_timeouts_init(data);

    إذا(data->id >= 0)
      CURL_TRC_M(data, "[TIMEOUT] all cleared");
  }
}

CURLMcode curl_multi_assign(CURLM *m, curl_socket_t sockfd,
                            فراغ *sockp)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_assign, &mresult)) {
    mresult = Curl_multi_ev_assign(m, sockfd, sockp);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

ساكن فراغ move_pending_to_connect(بنية Curl_multi *multi,
                                    بنية Curl_easy *data)
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
ساكن فراغ multi_schedule_pending(بنية Curl_multi *multi)
{
  uint32_t mid = multi->last_pending_mid;

  إذا(mid) {
    طالما(Curl_uint32_bset_next(&multi->pending, mid, &mid)) {
      بنية Curl_easy *data = Curl_multi_get_easy(multi, mid);
      إذا(data) {
        move_pending_to_connect(multi, data);
        multi->last_pending_mid = mid;
        أرجع;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    }
    /* found no pending transfers with `mid` larger than `last_pending_mid`.
     * Start at the beginning of the pending set again. */
    multi->last_pending_mid = 0;
  }

  إذا(Curl_uint32_bset_first(&multi->pending, &mid)) {
    افعل {
      بنية Curl_easy *data = Curl_multi_get_easy(multi, mid);
      إذا(data) {
        move_pending_to_connect(multi, data);
        multi->last_pending_mid = mid;
        أرجع;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    } طالما(Curl_uint32_bset_next(&multi->pending, mid, &mid));
  }
}

uint32_t Curl_multi_max_concurrent_streams(بنية Curl_multi *multi)
{
  DEBUGASSERT(multi);
  أرجع multi->max_concurrent_streams;
}

CURL **curl_multi_get_handles(CURLM *m)
{
  بنية Curl_mapi_guard guard;
  CURL **a = عدم;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_get_handles, عدم)) {
    بنية Curl_multi *multi = m;
    فراغ *entry;
    size_t کيراءن = Curl_uint32_tbl_کيراءن(&multi->xfers);

    a = curlx_malloc(حجم(بنية Curl_easy *) * (کيراءن + 1));
    إذا(a) {
      unsigned صحيح i = 0;
      uint32_t mid;

      إذا(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
        افعل {
          بنية Curl_easy *data = entry;
          DEBUGASSERT(i < کيراءن);
          إذا(!data->state.internal)
            a[i++] = data;
        } طالما(Curl_uint32_tbl_next(&multi->xfers, mid, &mid, &entry));
      }
      a[i] = عدم; /* last entry is a NULL */
    }
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع a;
}

CURLMcode curl_multi_get_offt(CURLM *m,
                              CURLMinfo_offt info,
                              curl_off_t *pvalue)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_get_offt, &mresult)) {
    بنية Curl_multi *multi = m;
    uint32_t n;

    إذا(!pvalue) {
      mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      اذهب out;
    }

    بدل(info) {
    حالة CURLMINFO_XFERS_CURRENT:
      n = Curl_uint32_tbl_کيراءن(&multi->xfers);
      إذا(n && multi->admin)
        --n;
      *pvalue = (curl_off_t)n;
      اكسر;
    حالة CURLMINFO_XFERS_RUNNING:
      n = Curl_uint32_bset_کيراءن(&multi->process);
      إذا(n && Curl_uint32_bset_contains(&multi->process, multi->admin->mid))
        --n;
      *pvalue = (curl_off_t)n;
      اكسر;
    حالة CURLMINFO_XFERS_PENDING:
      *pvalue = (curl_off_t)Curl_uint32_bset_کيراءن(&multi->pending);
      اكسر;
    حالة CURLMINFO_XFERS_DONE:
      *pvalue = (curl_off_t)Curl_uint32_bset_کيراءن(&multi->msgsent);
      اكسر;
    حالة CURLMINFO_XFERS_ADDED:
      *pvalue = multi->xfers_total_ever;
      اكسر;
    افتراضي:
      *pvalue = -1;
      mresult = CURLM_UNKNOWN_OPTION;
      اكسر;
    }
  }
out:
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

ساكن بنية Curl_fixed_buf *fixed_buf_create(size_t len)
{
  بنية Curl_fixed_buf *fbuf;
  إذا((SIZE_MAX - حجم(*fbuf)) < len)
    أرجع عدم; /* too large */
  fbuf = curlx_malloc(len + حجم(*fbuf));
  إذا(fbuf)
    fbuf->len = len;
  أرجع fbuf;
}

CURLcode Curl_multi_xfer_buf_borrow(بنية Curl_easy *data,
                                    حرف **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  *pbuf = عدم;
  *pbuflen = 0;
  إذا(!data->multi) {
    failf(data, "transfer has no multi handle");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(!data->set.buffer_size) {
    failf(data, "transfer buffer size is 0");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(data->multi->xfer_buf_borrowed) {
    failf(data, "attempt to borrow xfer_buf when already borrowed");
    أرجع CURLE_AGAIN;
  }

  إذا(data->multi->xfer_buf &&
     data->set.buffer_size > data->multi->xfer_buf->len) {
    /* not large enough, get a new one */
    curlx_safefree(data->multi->xfer_buf);
  }

  إذا(!data->multi->xfer_buf) {
    data->multi->xfer_buf =
      fixed_buf_create(curlx_uitouz(data->set.buffer_size));
    إذا(!data->multi->xfer_buf) {
      failf(data, "could not allocate xfer_buf of %u bytes",
            data->set.buffer_size);
      أرجع CURLE_OUT_OF_MEMORY;
    }
  }

  data->multi->xfer_buf_borrowed = TRUE;
  *pbuf = data->multi->xfer_buf->data;
  *pbuflen = data->multi->xfer_buf->len;
  أرجع CURLE_OK;
}

فراغ Curl_multi_xfer_buf_release(بنية Curl_easy *data, حرف *buf)
{
  (فراغ)buf;
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  DEBUGASSERT(!buf || (data->multi->xfer_buf &&
                       data->multi->xfer_buf->data == buf));
  data->multi->xfer_buf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_ulbuf_borrow(بنية Curl_easy *data,
                                      حرف **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  *pbuf = عدم;
  *pbuflen = 0;
  إذا(!data->multi) {
    failf(data, "transfer has no multi handle");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(data->multi->xfer_ulbuf_borrowed) {
    failf(data, "attempt to borrow xfer_ulbuf when already borrowed");
    أرجع CURLE_AGAIN;
  }

  إذا(data->multi->xfer_ulbuf &&
     data->set.upload_buffer_size > data->multi->xfer_ulbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(data->multi->xfer_ulbuf);
  }

  إذا(!data->multi->xfer_ulbuf) {
    data->multi->xfer_ulbuf =
      fixed_buf_create(curlx_uitouz(data->set.upload_buffer_size));
    إذا(!data->multi->xfer_ulbuf) {
      failf(data, "could not allocate xfer_ulbuf of %u bytes",
            data->set.upload_buffer_size);
      أرجع CURLE_OUT_OF_MEMORY;
    }
  }

  data->multi->xfer_ulbuf_borrowed = TRUE;
  *pbuf = data->multi->xfer_ulbuf->data;
  *pbuflen = data->multi->xfer_ulbuf->len;
  أرجع CURLE_OK;
}

فراغ Curl_multi_xfer_ulbuf_release(بنية Curl_easy *data, حرف *buf)
{
  (فراغ)buf;
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  DEBUGASSERT(!buf || (data->multi->xfer_ulbuf &&
                       data->multi->xfer_ulbuf->data == buf));
  data->multi->xfer_ulbuf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_sockbuf_borrow(بنية Curl_easy *data,
                                        size_t blen, حرف **pbuf)
{
  DEBUGASSERT(data);
  *pbuf = عدم;
  إذا(!data->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    *pbuf = curlx_malloc(blen);
    أرجع *pbuf ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  إذا(data->multi->xfer_sockbuf_borrowed) {
    failf(data, "attempt to borrow xfer_sockbuf when already borrowed");
    أرجع CURLE_AGAIN;
  }

  إذا(data->multi->xfer_sockbuf && blen > data->multi->xfer_sockbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(data->multi->xfer_sockbuf);
  }

  إذا(!data->multi->xfer_sockbuf) {
    data->multi->xfer_sockbuf = fixed_buf_create(blen);
    إذا(!data->multi->xfer_sockbuf) {
      failf(data, "could not allocate xfer_sockbuf of %zu bytes", blen);
      أرجع CURLE_OUT_OF_MEMORY;
    }
  }

  data->multi->xfer_sockbuf_borrowed = TRUE;
  *pbuf = data->multi->xfer_sockbuf->data;
  أرجع CURLE_OK;
}

فراغ Curl_multi_xfer_sockbuf_release(بنية Curl_easy *data, حرف *buf)
{
  DEBUGASSERT(data);
  إذا(!data->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    curlx_free(buf);
  }
  وإلا {
    DEBUGASSERT(!buf || (data->multi->xfer_sockbuf &&
                         data->multi->xfer_sockbuf->data == buf));
    data->multi->xfer_sockbuf_borrowed = FALSE;
  }
}

ساكن فراغ multi_xfer_bufs_free(بنية Curl_multi *multi)
{
  DEBUGASSERT(multi);
  curlx_safefree(multi->xfer_buf);
  multi->xfer_buf_borrowed = FALSE;
  curlx_safefree(multi->xfer_ulbuf);
  multi->xfer_ulbuf_borrowed = FALSE;
  curlx_safefree(multi->xfer_sockbuf);
  multi->xfer_sockbuf_borrowed = FALSE;
}

بنية Curl_easy *Curl_multi_get_easy(بنية Curl_multi *multi,
                                      uint32_t mid)
{
  بنية Curl_easy *data = Curl_uint32_tbl_get(&multi->xfers, mid);
  إذا(GOOD_EASY_HANDLE(data))
    أرجع data;
  CURL_TRC_M(multi->admin, "invalid easy handle in xfer table for mid=%u",
             mid);
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  أرجع عدم;
}

bool Curl_multi_knows_easy(بنية Curl_multi *multi, بنية Curl_easy *data)
{
  أرجع Curl_uint32_tbl_get(&multi->xfers, data->mid) == data;
}

ساكن uint32_t multi_xfers_running(بنية Curl_multi *multi)
{
  إذا(!multi) {
    DEBUGASSERT(0);
    أرجع 0;
  }
  أرجع multi->xfers_alive;
}

uint32_t Curl_multi_xfers_attached(بنية Curl_multi *multi)
{
  إذا(!multi || !multi->admin) {
    DEBUGASSERT(0);
    أرجع 0;
  }
  /* Discount the admin handle */
  أرجع Curl_uint32_tbl_کيراءن(&multi->xfers) - 1;
}

فراغ Curl_multi_mark_dirty(بنية Curl_easy *data)
{
  إذا(data->multi && data->mid != UINT32_MAX)
    Curl_uint32_bset_add(&data->multi->dirty, data->mid);
}

فراغ Curl_multi_clear_dirty(بنية Curl_easy *data)
{
  إذا(data->multi && data->mid != UINT32_MAX)
    Curl_uint32_bset_remove(&data->multi->dirty, data->mid);
}

CURLMcode curl_multi_notify_enable(CURLM *m, unsigned صحيح notification)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_notify_enable, &mresult)) {
    mresult = Curl_mntfy_enable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

CURLMcode curl_multi_notify_disable(CURLM *m, unsigned صحيح notification)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_notify_disable, &mresult)) {
    mresult = Curl_mntfy_disable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

#ifdef DEBUGBUILD
ساكن فراغ multi_xfer_dump(بنية Curl_multi *multi, uint32_t mid,
                            فراغ *entry)
{
  بنية Curl_easy *data = entry;

  (فراغ)multi;
  إذا(!data) {
    curl_mfprintf(stderr, "mid=%u, entry=NULL, bug in xfer table?\n", mid);
  }
  وإلا {
    curl_mfprintf(stderr, "mid=%u, magic=%s, p=%p, id=%" FMT_OFF_T
                  ", url=%s\n",
                  mid,
                  (data->magic == CURLEASY_MAGIC_نومبور) ? "GOOD" : "BAD!",
                  (فراغ *)data, data->id, Curl_bufref_ptr(&data->state.url));
  }
}

ساكن فراغ multi_xfer_tbl_dump(بنية Curl_multi *multi)
{
  uint32_t mid;
  فراغ *entry;
  curl_mfprintf(stderr, "=== multi xfer table (count=%u, capacity=%u\n",
                Curl_uint32_tbl_کيراءن(&multi->xfers),
                Curl_uint32_tbl_capacity(&multi->xfers));
  إذا(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
    multi_xfer_dump(multi, mid, entry);
    طالما(Curl_uint32_tbl_next(&multi->xfers, mid, &mid, &entry))
      multi_xfer_dump(multi, mid, entry);
  }
  curl_mfprintf(stderr, "===\n");
  fflush(stderr);
}
#endif /* DEBUGBUILD */
