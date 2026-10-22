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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "psl.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "multi_ev.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "curlx/wait.h"
#ಅನ್ತರ್ಭಾವ "conncache.h"
#ಅನ್ತರ್ಭಾವ "multihandle.h"
#ಅನ್ತರ್ಭಾವ "sigpipe.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "http_proxy.h"
#ಅನ್ತರ್ಭಾವ "http2.h"
#ಅನ್ತರ್ಭಾವ "socketpair.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

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

ಸ್ಥಿರ ಶೂನ್ಯ move_pending_to_connect(ಸಂರಚನಾ Curl_multi *multi,
                                    ಸಂರಚನಾ Curl_easy *data);
ಸ್ಥಿರ CURLMcode add_next_timeout(ಧ್ರುವ ಸಂರಚನಾ curltime *pnow,
                                  ಸಂರಚನಾ Curl_multi *multi,
                                  ಸಂರಚನಾ Curl_easy *data);
ಸ್ಥಿರ ಶೂನ್ಯ multi_timeout(ಸಂರಚನಾ Curl_multi *multi,
                          timediff_t *pexire_offset_us,
                          ಪೂರ್ಣಾಙ್ಕ *timeout_ms);
ಸ್ಥಿರ ಶೂನ್ಯ multi_schedule_pending(ಸಂರಚನಾ Curl_multi *multi);
ಸ್ಥಿರ ಶೂನ್ಯ multi_xfer_bufs_free(ಸಂರಚನಾ Curl_multi *multi);
#ifdef DEBUGBUILD
ಸ್ಥಿರ ಶೂನ್ಯ multi_xfer_tbl_dump(ಸಂರಚನಾ Curl_multi *multi);
#endif
/* Get the # of transfers current in process/pending. */
ಸ್ಥಿರ uint32_t multi_xfers_running(ಸಂರಚನಾ Curl_multi *multi);

/* function pointer called once when entering a state */
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*mstate_enter_func)(ಸಂರಚನಾ Curl_easy *data,
                                  CURLMstate from_state);

ಸ್ಥಿರ ಶೂನ್ಯ mstate_enter_connect(ಸಂರಚನಾ Curl_easy *data,
                                 CURLMstate from_state)
{
  (ಶೂನ್ಯ)from_state;
  Curl_init_CONNECT(data);
}

ಸ್ಥಿರ ಶೂನ್ಯ mstate_enter_did(ಸಂರಚನಾ Curl_easy *data,
                             CURLMstate from_state)
{
  (ಶೂನ್ಯ)from_state;
  data->req.chunk = FALSE;
  Curl_pgrsTime(data, TIMER_PRETRANSFER);
  ಯದಿ(!CURL_REQ_WANT_SEND(data))
    Curl_pgrsTime(data, TIMER_POSTRANSFER);
}

ಸ್ಥಿರ ಶೂನ್ಯ mstate_enter_done(ಸಂರಚನಾ Curl_easy *data,
                              CURLMstate from_state)
{
  (ಶೂನ್ಯ)from_state;
  CURLM_NTFY(data, CURLMNOTIFY_EASY_DONE);
}

ಸ್ಥಿರ ಶೂನ್ಯ mstate_enter_completed(ಸಂರಚನಾ Curl_easy *data,
                                   CURLMstate from_state)
{
  /* we sometimes directly jump to COMPLETED, trigger things
   * we then missed. */
  ಯದಿ(from_state < MSTATE_DID) {
    Curl_pgrsTime(data, TIMER_PRETRANSFER);
    Curl_pgrsTime(data, TIMER_POSTRANSFER);
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);
  }
  /* multi_done() should have run and detached any connection already */
  DEBUGASSERT(!data->conn);
  Curl_pgrsCompleted(data);
  ಯದಿ(from_state < MSTATE_DONE)
    CURLM_NTFY(data, CURLMNOTIFY_EASY_DONE);
  /* changing to COMPLETED means it is in process and needs to go */
  DEBUGASSERT(Curl_uint32_bset_contains(&data->multi->process, data->mid));
  Curl_uint32_bset_remove(&data->multi->process, data->mid);
  Curl_uint32_bset_remove(&data->multi->pending, data->mid); /* to be sure */

  ಯದಿ(Curl_uint32_bset_empty(&data->multi->process)) {
    /* free the transfer buffer when we have no more active transfers */
    multi_xfer_bufs_free(data->multi);
  }
  /* Important: reset the conn pointer so that we do not point to memory
     that could be freed anytime */
  Curl_detach_connection(data);
  Curl_expire_clear_all(data); /* stop all timers */
}

/* always use this function to change state, to make debugging easier */
ಸ್ಥಿರ ಶೂನ್ಯ mstate(ಸಂರಚನಾ Curl_easy *data, CURLMstate state
#ifdef DEBUGBUILD
                   , ಪೂರ್ಣಾಙ್ಕ lineno
#endif
)
{
  CURLMstate oldstate = data->mstate;
  ಸ್ಥಿರ ಧ್ರುವ mstate_enter_func state_enter[MSTATE_LAST] = {
    ರಿಕ್ತ,                      /* INIT */
    ರಿಕ್ತ,                      /* PENDING */
    ರಿಕ್ತ,                      /* SETUP */
    mstate_enter_connect,      /* CONNECT */
    ರಿಕ್ತ,                      /* CONNECTING */
    ರಿಕ್ತ,                      /* PROTOCONNECT */
    ರಿಕ್ತ,                      /* PROTOCONNECTING */
    ರಿಕ್ತ,                      /* DO */
    ರಿಕ್ತ,                      /* DOING */
    ರಿಕ್ತ,                      /* DOING_MORE */
    mstate_enter_did,          /* DID */
    ರಿಕ್ತ,                      /* PERFORMING */
    ರಿಕ್ತ,                      /* RATELIMITING */
    mstate_enter_done,         /* DONE */
    mstate_enter_completed,    /* COMPLETED */
    ರಿಕ್ತ                       /* MSGSENT */
  };

  ಯದಿ(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    ಪ್ರತ್ಯಾಗಮ;

#ifdef DEBUGBUILD
  NOVERBOSE((ಶೂನ್ಯ)lineno);
  CURL_TRC_M(data, "-> [%s] (line %d)", CURL_MSTATE_NAME(state), lineno);
#ಅನ್ಯಥಾ
  CURL_TRC_M(data, "-> [%s]", CURL_MSTATE_NAME(state));
#endif

  /* really switching state */
  data->mstate = state;
  ಯದಿ(state_enter[state])
    state_enter[state](data, oldstate);
}

#ifndef DEBUGBUILD
#define multistate(x, y) mstate(x, y)
#ಅನ್ಯಥಾ
#define multistate(x, y) mstate(x, y, __LINE__)
#endif

/* multi->proto_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
ಸ್ಥಿರ ಶೂನ್ಯ ph_freeentry(ಶೂನ್ಯ *p)
{
  (ಶೂನ್ಯ)p;
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
ಸ್ಥಿರ ಶೂನ್ಯ multi_addmsg(ಸಂರಚನಾ Curl_multi *multi, ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(Curl_uint32_bset_empty(&multi->msgsent))
    CURLM_NTFY(multi->admin, CURLMNOTIFY_INFO_READ);
  Curl_uint32_bset_add(&multi->msgsent, data->mid);
}

ಸ್ಥಿರ ಶೂನ್ಯ multi_timeouts_init(ಸಂರಚನಾ Curl_easy *data);

ಸಂರಚನಾ Curl_multi *Curl_multi_handle(uint32_t xfer_table_size,
                                     size_t ev_hashsize,  /* event hash */
                                     size_t chashsize, /* connection hash */
                                     size_t dnssize,   /* dns hash */
                                     size_t sesssize)  /* TLS session cache */
{
  ಸಂರಚನಾ curltime created;
  ಸಂರಚನಾ Curl_multi *multi = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_multi));

  ಯದಿ(!multi)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  multi->magic = CURLMULTI_MAGIC_ಸಂಖ್ಯೆ;

  /* Initialisation order is important here!
   * easy_init() does a lazy check on curl_global_init() which sets
   * up platform specific things we need. For example calling curlx_pnow()
   * before this is not safe. */
  multi->admin = curl_easy_init();
  ಯದಿ(!multi->admin) {
    curlx_free(multi);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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

  ಯದಿ(Curl_uint32_bset_resize(&multi->process, xfer_table_size) ||
     Curl_uint32_bset_resize(&multi->pending, xfer_table_size) ||
     Curl_uint32_bset_resize(&multi->dirty, xfer_table_size) ||
     Curl_uint32_bset_resize(&multi->msgsent, xfer_table_size) ||
     Curl_uint32_tbl_resize(&multi->xfers, xfer_table_size))
    ಗಚ್ಛ error;

#ifdef DEBUGBUILD
  {
    ಧ್ರುವ ವರ್ಣ *p;
    ಯದಿ(getenv("CURL_DEBUG"))
      multi->admin->set.verbose = TRUE;
    p = getenv("CURL_DBG_MAX_TOTAL_CONNECTIONS");
    ಯದಿ(p) {
      curl_off_t l;
      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, UINT32_MAX)) {
        multi->max_total_connections = (uint32_t)l;
      }
    }
  }
#endif
  Curl_uint32_tbl_add(&multi->xfers, multi->admin, &multi->admin->mid);
  Curl_uint32_bset_add(&multi->process, multi->admin->mid);

  Curl_cpool_init(&multi->cpool, ರಿಕ್ತ, chashsize);

#ifdef USE_SSL
  ಯದಿ(Curl_ssl_scache_create(sesssize, 2, &multi->ssl_scache))
    ಗಚ್ಛ error;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)sesssize;
#endif

#ifdef USE_WINSOCK
  multi->wsa_event = WSACreateEvent();
  ಯದಿ(multi->wsa_event == WSA_INVALID_EVENT)
    ಗಚ್ಛ error;
#endif
#ifdef ENABLE_WAKEUP
  /* When enabled, rely on this to work. We ignore this in previous
   * versions, but that seems an unnecessary complication. */
  ಯದಿ(Curl_wakeup_init(multi->wakeup_pair, TRUE) < 0)
    ಗಚ್ಛ error;
#endif
#ifdef ENABLE_INTERNAL_WAKEUP
  ಯದಿ(Curl_wakeup_init(multi->wakeup_internal, TRUE) < 0)
    ಗಚ್ಛ error;
#endif

  ಯದಿ(Curl_probeipv6(multi))
    ಗಚ್ಛ error;

#ifdef USE_RESOLV_THREADED
  ಯದಿ(xfer_table_size < CURL_XFER_TABLE_SIZE) { /* easy multi */
    ಯದಿ(Curl_async_thrdd_multi_init(multi, 0, 2, 10))
      ಗಚ್ಛ error;
  }
  ಅನ್ಯಥಾ { /* real multi handle */
    ಯದಿ(Curl_async_thrdd_multi_init(multi, 0, 20, 2000))
      ಗಚ್ಛ error;
  }
#endif

  ಪ್ರತ್ಯಾಗಮ multi;

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
  ಯದಿ(multi->admin) {
    Curl_multi_ev_xfer_done(multi, multi->admin);
    multi->admin->multi = ರಿಕ್ತ;
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
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

CURLM *curl_multi_init(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ Curl_multi_handle(CURL_XFER_TABLE_SIZE,
                           CURL_SOCKET_HASH_TABLE_SIZE,
                           CURL_CONNECTION_HASH_SIZE,
                           CURL_DNS_HASH_SIZE,
                           CURL_TLS_SESSION_SIZE);
}

#ಯದಿ defined(DEBUGBUILD) && defined(CURLVERBOSE)
ಸ್ಥಿರ ಶೂನ್ಯ multi_warn_debug(ಸಂರಚನಾ Curl_multi *multi, ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(!multi->warned) {
    infof(data, "!!! WARNING !!!");
    infof(data, "This is a debug build of libcurl, "
                "do not use in production.");
    multi->warned = TRUE;
  }
}
#ಅನ್ಯಥಾ
#define multi_warn_debug(x, y) Curl_nop_stmt
#endif

bool Curl_is_connecting(ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ data->mstate < MSTATE_DO;
}

ಸ್ಥಿರ CURLMcode multi_assess_wakeup(ಸಂರಚನಾ Curl_multi *multi)
{
#ifdef ENABLE_INTERNAL_WAKEUP
  ಯದಿ(multi->socket_cb)
    ಪ್ರತ್ಯಾಗಮ Curl_multi_ev_assess_xfer(multi, multi->admin);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)multi;
#endif
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode multi_xfers_add(ಸಂರಚನಾ Curl_multi *multi,
                                 ಸಂರಚನಾ Curl_easy *data)
{
  uint32_t capacity = Curl_uint32_tbl_capacity(&multi->xfers);
  uint32_t new_size = 0;
  /* Prepare to make this into a CURLMOPT_MAX_TRANSFERS, because some
   * applications may want to prevent a run-away of their memory use. */
  /* UINT_MAX is our "invalid" id, do not let the table grow up to that. */
  ಧ್ರುವ uint32_t max_capacity = UINT_MAX - 1;

  ಯದಿ(capacity < max_capacity) {
    /* We want `multi->xfers` to have "sufficient" free rows, so that we do
     * not have to reuse the `mid` from a removed easy right away.
     * Check if an 8th of the capacity is still free */
    uint32_t used = Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers);
    uint32_t unused = capacity - used;
    uint32_t min_unused = CURLMAX(capacity >> 3, 4);
    ಯದಿ(unused < min_unused) {
      /* Grow by 50% of current capacity, in range of [128, 2048],
       * which means the table grows max by 16kb on 64-bit arch. */
      uint32_t growth = CURLMIN(CURLMAX(capacity >> 1, 128), 2048);
      /* Make sure the uint arithmetic here works on the corner
       * cases where we are close to max_capacity or UINT_MAX */
      ಯದಿ((max_capacity - growth) <= capacity)
        new_size = max_capacity;
      ಅನ್ಯಥಾ
        new_size = capacity + growth;
    }
  }

  ಯದಿ(new_size > capacity) {
    /* Grow the bitsets first. Should one fail, we do not need
     * to downsize the already resized ones. The sets continue
     * to work properly when larger than the table, but not
     * the other way around. */
    CURL_TRC_M(data, "increasing xfer table size to %u", new_size);
    ಯದಿ(Curl_uint32_bset_resize(&multi->process, new_size) ||
       Curl_uint32_bset_resize(&multi->dirty, new_size) ||
       Curl_uint32_bset_resize(&multi->pending, new_size) ||
       Curl_uint32_bset_resize(&multi->msgsent, new_size) ||
       Curl_uint32_tbl_resize(&multi->xfers, new_size))
      ಪ್ರತ್ಯಾಗಮ CURLM_OUT_OF_MEMORY;
  }

  /* Insert the easy into the table now */
  ಯದಿ(!Curl_uint32_tbl_add(&multi->xfers, data, &data->mid)) {
    /* MUST only happen when table is full */
    DEBUGASSERT(Curl_uint32_tbl_capacity(&multi->xfers) <=
                Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers));
    ಪ್ರತ್ಯಾಗಮ CURLM_OUT_OF_MEMORY;
  }
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

CURLMcode Curl_multi_add_handle(ಸಂರಚನಾ Curl_multi *multi,
                                ಸಂರಚನಾ Curl_easy *data)
{
  CURLMcode mresult;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  ಯದಿ(data->multi)
    ಪ್ರತ್ಯಾಗಮ CURLM_ADDED_ALREADY;

  ಯದಿ(multi->dead) {
    /* a "dead" handle cannot get added transfers while any existing easy
       handles are still alive - but if there are none alive anymore, it is
       fine to start over and unmark the "deadness" of this handle.
       This means only the admin handle MUST be present. */
    ಯದಿ((Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers) != 1) ||
       !Curl_uint32_tbl_contains(&multi->xfers, 0))
      ಪ್ರತ್ಯಾಗಮ CURLM_ABORTED_BY_CALLBACK;
    multi->dead = FALSE;
    Curl_uint32_bset_clear(&multi->process);
    Curl_uint32_bset_clear(&multi->dirty);
    Curl_uint32_bset_clear(&multi->pending);
    Curl_uint32_bset_clear(&multi->msgsent);
  }

  ಯದಿ(data->multi_easy) {
    /* if this easy handle was previously used for curl_easy_perform(), there
       is a private multi handle here that we can kill */
    curl_multi_cleanup(data->multi_easy);
    data->multi_easy = ರಿಕ್ತ;
  }

  /* Insert the easy into the multi->xfers table, assigning it a `mid`. */
  ಯದಿ(multi_xfers_add(multi, data))
    ಪ್ರತ್ಯಾಗಮ CURLM_OUT_OF_MEMORY;

  /* Initialize timeouts for this handle */
  multi_timeouts_init(data);

  /*
   * No failure allowed in this function beyond this point. No modification
   * of easy nor multi handle allowed before this except for potential
   * multi's connection pool growing which will not be undone in this
   * function no matter what.
   */
  ಯದಿ(data->set.errorbuffer)
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
  ಯದಿ(data->share && (data->share->specifier & (1 << CURL_LOCK_DATA_PSL)))
    data->psl = &data->share->psl;
  ಅನ್ಯಥಾ
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
  ಯದಿ(mresult) {
    /* undo the additions above */
    Curl_uint32_bset_remove(&multi->process, data->mid);
    Curl_uint32_bset_remove(&multi->dirty, data->mid);
    --multi->xfers_alive;
    data->multi = ರಿಕ್ತ; /* not anymore */
    Curl_uint32_tbl_remove(&multi->xfers, data->mid);
    data->mid = UINT32_MAX;
    ಪ್ರತ್ಯಾಗಮ mresult;
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
             Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers));
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

CURLMcode curl_multi_add_handle(CURLM *m, CURL *curl)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_add_handle, &mresult)) {
    ಸಂರಚನಾ Curl_easy *data = curl;
    /* Verify that we got a somewhat good easy handle too */
    ಯದಿ(!GOOD_EASY_HANDLE(data) || data->state.internal)
      mresult = CURLM_BAD_EASY_HANDLE;
    ಅನ್ಯಥಾ
      mresult = Curl_multi_add_handle(m, data);
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

ಸಂರಚನಾ multi_done_ctx {
  BIT(premature);
};

ಸ್ಥಿರ bool multi_conn_should_close(ಸಂರಚನಾ connectdata *conn,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    bool premature)
{
  /* if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of everything else. */
  ಯದಿ(conn->bits.close)
    ಪ್ರತ್ಯಾಗಮ TRUE;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM/NEGOTIATE authentication handshake. */
  ಯದಿ(data->set.reuse_forbid
#ifdef USE_NTLM
     && !(conn->http_ntlm_state == NTLMSTATE_TYPE2 ||
          conn->proxy_ntlm_state == NTLMSTATE_TYPE2)
#endif
#ifdef USE_SPNEGO
     && !(conn->http_negotiate_state == CURL_NEGO_AUTH_RECV ||
          conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)
#endif
    )
    ಪ್ರತ್ಯಾಗಮ TRUE;

  /* Unless this connection is for a "connect-only" transfer, it
   * needs to be closed if the protocol handler does not support reuse. */
  ಯದಿ(!data->set.connect_only && conn->scheme &&
     !(conn->scheme->flags & PROTOPT_CONN_REUSE))
    ಪ್ರತ್ಯಾಗಮ TRUE;

  /* if premature is TRUE, it means this connection was said to be DONE before
     the entire request operation is complete and thus we cannot know in what
     state it is for reusing, so we are forced to close it. In a perfect world
     we can add code that keep track of if we really must close it here or not,
     but currently we have no such detail knowledge. */
  ಯದಿ(premature && !Curl_conn_is_multiplex(conn, FIRSTSOCKET))
    ಪ್ರತ್ಯಾಗಮ TRUE;

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ cpool_do_result multi_done_locked(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ connectdata *conn,
                                         ಶೂನ್ಯ *userdata,
                                         ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಸಂರಚನಾ multi_done_ctx *mdctx = userdata;

  (ಶೂನ್ಯ)pnow;
  CURL_TRC_M(data, "multi_done_locked, in use=%u", conn->attached_xfers);
  ಯದಿ(CONN_INUSE(conn)) {
    /* Stop if still used. */
    CURL_TRC_M(data, "Connection still in use %u, no more multi_done now!",
               conn->attached_xfers);
    ಪ್ರತ್ಯಾಗಮ CPOOL_DO_KEEP;
  }

  ಯದಿ(multi_conn_should_close(conn, data, (bool)mdctx->premature)) {
    CURL_TRC_M(data, "multi_done, terminating conn #%" FMT_OFF_T " to %s:%u, "
               "forbid=%d, close=%d, premature=%d, conn_multiplex=%d",
               conn->connection_id, conn->origin->user_hostname,
               conn->origin->port,
               data->set.reuse_forbid, conn->bits.close, mdctx->premature,
               Curl_conn_is_multiplex(conn, FIRSTSOCKET));
    connclose(conn);
    ಪ್ರತ್ಯಾಗಮ mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  ಅನ್ಯಥಾ ಯದಿ(!Curl_conn_get_max_concurrent(data, conn, FIRSTSOCKET)) {
    CURL_TRC_M(data, "multi_done, conn #%" FMT_OFF_T " to %s:%u was shutdown"
               " by server, not reusing", conn->connection_id,
               conn->origin->user_hostname, conn->origin->port);
    connclose(conn);
    ಪ್ರತ್ಯಾಗಮ mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  ಅನ್ಯಥಾ {
    ಪ್ರತ್ಯಾಗಮ CPOOL_DO_IDLE;
  }
}

ಸ್ಥಿರ CURLcode multi_done(ಸಂರಚನಾ Curl_easy *data,
                           CURLcode status,  /* an error if this is called
                                                after an error was detected */
                           bool premature)
{
  CURLcode result;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow = Curl_pgrs_now(data);

  CURL_TRC_M(data, "multi_done: status: %d prem: %d done: %d",
             (ಪೂರ್ಣಾಙ್ಕ)status, (ಪೂರ್ಣಾಙ್ಕ)premature, data->state.done);

  ಯದಿ(data->state.done) {
    /* Stop if multi_done() has already been called */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  data->state.done = TRUE; /* called now! */

  /* Shut down any ongoing async resolver operation. */
  Curl_resolv_shutdown_all(data);

  /* Cleanup possible redirect junk */
  curlx_safefree(data->req.newurl);
  curlx_safefree(data->req.location);

  ವಿಕಲ್ಪ(status) {
  ಪಕ್ಷ CURLE_ABORTED_BY_CALLBACK:
  ಪಕ್ಷ CURLE_READ_ERROR:
  ಪಕ್ಷ CURLE_WRITE_ERROR:
    /* When we are aborted due to a callback return code it has to be counted
       as premature as there is trouble ahead if we do not. We have many
       callbacks and protocols work differently, we could potentially do this
       more fine-grained in the future. */
    premature = TRUE;
    FALLTHROUGH();
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }

  /* this calls the protocol-specific function pointer previously set */
  ಯದಿ(conn && conn->scheme->run->done && (data->mstate >= MSTATE_PROTOCONNECT))
    result = conn->scheme->run->done(data, status, premature);
  ಅನ್ಯಥಾ
    result = status;

  ಯದಿ(data->mstate > MSTATE_CONNECTING &&
     (result != CURLE_ABORTED_BY_CALLBACK)) {
    /* avoid this if
     * - the transfer has not connected
     * - we already aborted by callback to avoid this calling another callback
     */
    ಪೂರ್ಣಾಙ್ಕ rc = Curl_pgrsDone(data, pnow);
    ಯದಿ(!result && rc)
      result = CURLE_ABORTED_BY_CALLBACK;
  }

  /* Make sure that transfer client writes are really done now. */
  result = Curl_1st_fatal(result, Curl_xfer_write_done(data, premature));

  ಯದಿ(!result)
    result = Curl_req_done(&data->req, data, premature);

  ಯದಿ(conn) {
    /* Detach connection and decide what to do with it. */
    ಸಂರಚನಾ multi_done_ctx mdctx;

    /* Inform connection filters that this transfer is done and they
     * need to give up any references to `data`. */
    Curl_conn_ev_data_done(data, premature);
    Curl_detach_connection(data);
    Curl_dnscache_prune(data, pnow);

    memset(&mdctx, 0, ಪರಿಮಾಣ(mdctx));
    mdctx.premature = premature;
    Curl_cpool_return(data, conn, multi_done_locked, &mdctx, pnow);

    ಯದಿ(data->multi)
      multi_schedule_pending(data->multi);
  }

  /* flush the netrc cache */
  Curl_netrc_cleanup(&data->state.netrc);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLMcode Curl_multi_remove_handle(ಸಂರಚನಾ Curl_multi *multi,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  CURLMcode mresult;
  bool premature;
  uint32_t mid;

  /* Prevent users from trying to remove same easy handle more than once */
  ಯದಿ(!data->multi)
    ಪ್ರತ್ಯಾಗಮ CURLM_OK; /* it is already removed so let's say it is fine! */

  /* Prevent users from trying to remove an easy handle from the wrong multi */
  ಯದಿ(data->multi != multi)
    ಪ್ರತ್ಯಾಗಮ CURLM_BAD_EASY_HANDLE;

  ಯದಿ(data->mid == UINT32_MAX) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ CURLM_INTERNAL_ERROR;
  }
  ಯದಿ(Curl_uint32_tbl_get(&multi->xfers, data->mid) != data) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ CURLM_INTERNAL_ERROR;
  }

  premature = (data->mstate < MSTATE_COMPLETED);

  ಯದಿ(data->conn) {
    /* If the 'state' is not INIT or COMPLETED, we might need to do something
       nice to put the easy_handle in a good known state when this returns. */
    ಯದಿ(premature && (data->mstate > MSTATE_DO))
      streamclose(data->conn);

    /* multi_done() clears the association between the easy handle and the
       connection.
       Note that this ignores the return code because there is
       nothing really useful to do with it anyway! */
    (ಶೂನ್ಯ)multi_done(data, data->result, premature);
  }

  /* The timer must be shut down before data->multi is set to NULL, else
     data's splaynode would remain in the splay tree after curl_easy_cleanup is
     called. Do it after multi_done() in case that sets another time! */
  Curl_expire_clear_all(data);

  /* In MSGSENT, it was deducted from `multi->xfers_alive` already. */
  ಯದಿ(data->mstate != MSTATE_MSGSENT)
    --multi->xfers_alive;

  ಯದಿ(data->state.really_alive) {
    data->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    ಯದಿ(!multi->xfers_really_alive)
      (ಶೂನ್ಯ)multi_assess_wakeup(multi);
  }

  Curl_wildcard_dtor(&data->wildcard);

  data->mstate = MSTATE_COMPLETED;

  /* Remove the association between the connection and the handle */
  Curl_detach_connection(data);

  /* Tell event handling that this transfer is definitely going away */
  Curl_multi_ev_xfer_done(multi, data);

  ಯದಿ(data->set.connect_only) {
    ಯದಿ(data->multi_easy) {
      ಯದಿ(data->state.last_conn_id != -1) {
        /* Mark any connect-only connection for closure */
        ಸಂರಚನಾ connectdata *conn;
        (ಶೂನ್ಯ)Curl_getconnectinfo(data, &conn);
        ಯದಿ(conn && conn->bits.connect_only)
          connclose(conn);
      }
    }
    ಅನ್ಯಥಾ {
      /* This removes a handle that was part the multi interface that used
         CONNECT_ONLY, that connection is now left alive but since this handle
         has bits.close set nothing can use that connection anymore and it is
         forbidden from reuse. This easy handle cannot find the connection
         anymore once removed from the multi handle

         Better close the connection here, at once. */
      ಸಂರಚನಾ connectdata *conn;
      (ಶೂನ್ಯ)Curl_getconnectinfo(data, &conn);
      ಯದಿ(conn)
        Curl_conn_close(data, conn, TRUE);
    }
  }

#ifdef USE_LIBPSL
  /* Remove the PSL association. */
  ಯದಿ(data->psl == &multi->psl)
    data->psl = ರಿಕ್ತ;
#endif

  /* clear the association to this multi handle */
  mid = data->mid;
  DEBUGASSERT(Curl_uint32_tbl_contains(&multi->xfers, mid));
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  Curl_uint32_bset_remove(&multi->process, mid);
  Curl_uint32_bset_remove(&multi->dirty, mid);
  Curl_uint32_bset_remove(&multi->pending, mid);
  Curl_uint32_bset_remove(&multi->msgsent, mid);
  data->multi = ರಿಕ್ತ;
  data->mid = UINT32_MAX;
  data->master_mid = UINT32_MAX;

  /* A pending transfer *might* be able to run now. */
  multi_schedule_pending(multi);
  mresult = Curl_update_timer(multi);
  ಯದಿ(mresult)
    ಪ್ರತ್ಯಾಗಮ mresult;

  mresult = multi_assess_wakeup(multi);
  ಯದಿ(mresult) {
    failf(data, "error enabling wakeup listening: %d", mresult);
    ಪ್ರತ್ಯಾಗಮ mresult;
  }

  CURL_TRC_M(data, "removed from multi, mid=%u, running=%u, total=%u",
             mid, multi_xfers_running(multi),
             Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers));
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *curl)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_remove_handle, &mresult)) {
    ಸಂರಚನಾ Curl_easy *data = curl;
    ಯದಿ(!GOOD_EASY_HANDLE(data) || data->state.internal)
      mresult = CURLM_BAD_EASY_HANDLE;
    ಅನ್ಯಥಾ
      mresult = Curl_multi_remove_handle(m, data);
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

/* Return TRUE if the application asked for multiplexing */
bool Curl_multiplex_wanted(ಧ್ರುವ ಸಂರಚನಾ Curl_multi *multi)
{
  ಪ್ರತ್ಯಾಗಮ multi && multi->multiplexing;
}

/*
 * Curl_detach_connection() removes the given transfer from the connection.
 *
 * This is the only function that should clear data->conn. This will
 * occasionally be called with the data->conn pointer already cleared.
 */
ಶೂನ್ಯ Curl_detach_connection(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಯದಿ(conn) {
    /* this should never happen, prevent underflow */
    DEBUGASSERT(conn->attached_xfers);
    ಯದಿ(conn->attached_xfers) {
      conn->attached_xfers--;
      ಯದಿ(!conn->attached_xfers)
        conn->attached_multi = ರಿಕ್ತ;
    }
  }
  data->conn = ರಿಕ್ತ;
}

/*
 * Curl_attach_connection() attaches this transfer to this connection.
 *
 * This is the only function that should assign data->conn.
 * `matched == TRUE` means the transfer's properties match this
 * connection and it is not a temporary attach for maintenance.
 */
ಶೂನ್ಯ Curl_attach_connection(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ connectdata *conn,
                            bool matched)
{
  DEBUGASSERT(data);
  DEBUGASSERT(!data->conn);
  DEBUGASSERT(conn);
  DEBUGASSERT(conn->attached_xfers < UINT32_MAX);
  data->conn = conn;
  ಯದಿ(matched) {
    data->state.last_conn_id = conn->connection_id;
    data->state.last_cpid = conn->cpid;
  }
  ಅನ್ಯಥಾ
    DEBUGASSERT(!data->mid); /* admin handle */
  conn->attached_xfers++;
  /* all attached transfers must be from the same multi */
  ಯದಿ(!conn->attached_multi)
    conn->attached_multi = data->multi;
  DEBUGASSERT(conn->attached_multi == data->multi);

  ಯದಿ(conn->scheme && conn->scheme->run->attach)
    conn->scheme->run->attach(data, conn);
}

/* adjust pollset for rate limits/pauses */
ಸ್ಥಿರ CURLcode multi_adjust_pollset(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ easy_pollset *ps)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(ps->n) {
    bool send_blocked, recv_blocked;

    recv_blocked = (Curl_rlimit_avail(&data->progress.dl.rlimit, ರಿಕ್ತ) <= 0);
    send_blocked = (Curl_rlimit_avail(&data->progress.ul.rlimit, ರಿಕ್ತ) <= 0);
    ಯದಿ(send_blocked || recv_blocked) {
      ಪೂರ್ಣಾಙ್ಕ i;
      ಪ್ರತಿ(i = 0; i <= SECONDARYSOCKET; ++i) {
        curl_socket_t sock = data->conn->sock[i];
        ಯದಿ(sock == CURL_SOCKET_BAD)
          ಅನುವರ್ತ;
        ಯದಿ(recv_blocked && Curl_pollset_want_recv(data, ps, sock)) {
          result = Curl_pollset_remove_in(data, ps, sock);
          ಯದಿ(result)
            ಭಙ್ಗ;
        }
        ಯದಿ(send_blocked && Curl_pollset_want_send(data, ps, sock)) {
          result = Curl_pollset_remove_out(data, ps, sock);
          ಯದಿ(result)
            ಭಙ್ಗ;
        }
      }
    }

    /* Not blocked and wanting to receive. If there is data pending
     * in the connection filters, make transfer run again. */
    ಯದಿ(!recv_blocked &&
       ((Curl_pollset_want_recv(data, ps, data->conn->sock[FIRSTSOCKET]) &&
         Curl_conn_data_pending(data, FIRSTSOCKET)) ||
        (Curl_pollset_want_recv(data, ps, data->conn->sock[SECONDARYSOCKET]) &&
         Curl_conn_data_pending(data, SECONDARYSOCKET)))) {
      CURL_TRC_M(data, "pollset[] has POLLIN, but there is still "
                 "buffered input -> mark as dirty");
      Curl_multi_mark_dirty(data);
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode mstate_connecting_pollset(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  curl_socket_t sockfd;
  CURLcode result = CURLE_OK;

  ಯದಿ(Curl_xfer_recv_is_paused(data))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  /* If a socket is set, receiving is default. If the socket
   * has not been determined yet (eyeballing), always ask the
   * connection filters for what to monitor. */
  sockfd = Curl_conn_get_first_socket(data);
  ಯದಿ(sockfd != CURL_SOCKET_BAD) {
    result = Curl_pollset_change(data, ps, sockfd, CURL_POLL_IN, 0);
    ಯದಿ(!result)
      result = multi_adjust_pollset(data, ps);
  }
  ಯದಿ(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode mstate_protocol_pollset(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  ಯದಿ(conn->scheme->run->proto_pollset)
    result = conn->scheme->run->proto_pollset(data, ps);
  ಅನ್ಯಥಾ {
    curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
    ಯದಿ(sockfd != CURL_SOCKET_BAD) {
      /* Default is to wait to something from the server */
      result = Curl_pollset_change(data, ps, sockfd, CURL_POLL_IN, 0);
    }
  }
  ಯದಿ(!result)
    result = multi_adjust_pollset(data, ps);
  ಯದಿ(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode mstate_do_pollset(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  ಯದಿ(conn->scheme->run->doing_pollset)
    result = conn->scheme->run->doing_pollset(data, ps);
  ಅನ್ಯಥಾ ಯದಿ(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    result = Curl_pollset_add_out(data, ps, conn->sock[conn->send_idx]);
  }
  ಯದಿ(!result)
    result = multi_adjust_pollset(data, ps);
  ಯದಿ(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode mstate_domore_pollset(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  ಯದಿ(conn->scheme->run->domore_pollset)
    result = conn->scheme->run->domore_pollset(data, ps);
  ಅನ್ಯಥಾ ಯದಿ(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    result = Curl_pollset_add_out(data, ps, conn->sock[conn->send_idx]);
  }
  ಯದಿ(!result)
    result = multi_adjust_pollset(data, ps);
  ಯದಿ(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode mstate_perform_pollset(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  ಯದಿ(conn->scheme->run->perform_pollset)
    result = conn->scheme->run->perform_pollset(data, ps);
  ಅನ್ಯಥಾ {
    /* Default is to obey the request flags for send/recv */
    ಯದಿ(Curl_req_want_recv(data) && CONN_SOCK_IDX_VALID(conn->recv_idx)) {
      result = Curl_pollset_add_in(data, ps, conn->sock[conn->recv_idx]);
    }
    ಯದಿ(!result && Curl_req_want_send(data) &&
       CONN_SOCK_IDX_VALID(conn->send_idx)) {
      result = Curl_pollset_add_out(data, ps, conn->sock[conn->send_idx]);
    }
  }
  ಯದಿ(!result)
    result = multi_adjust_pollset(data, ps);
  ಯದಿ(!result)
    result = Curl_conn_adjust_pollset(data, conn, ps);
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef CURLVERBOSE
ಸ್ಥಿರ size_t multi_timeouts_ಎಣಿಕೆ(ಸಂರಚನಾ expire_timers *timeouts)
{
  size_t n = 0;
  uint8_t eid = timeouts->first;
  ಪ್ರತಿ(; eid < EXPIRE_LAST; eid = timeouts->next[eid])
    ++n;
  ಪ್ರತ್ಯಾಗಮ n;
}
#endif

/* Initializes `poll_set` with the current socket poll actions needed
 * for transfer `data`. */
CURLMcode Curl_multi_pollset(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ easy_pollset *ps)
{
  CURLcode result = CURLE_OK;

  Curl_pollset_reset(ps);
#ifdef ENABLE_INTERNAL_WAKEUP
  /* The admin handle always listens on the wakeup socket when there
   * are transfers alive. */
  ಯದಿ(data->multi && (data == data->multi->admin) &&
     data->multi->xfers_really_alive) {
    CURL_TRC_M(data, "adding wakeup, %u xfers really alive",
               data->multi->xfers_really_alive);
    result = Curl_pollset_add_in(data, ps, data->multi->wakeup_internal[0]);
  }
#endif
  /* If the transfer has no connection, this is fine. Happens when
     called via curl_multi_remove_handle() => Curl_multi_ev_assess() =>
     Curl_multi_pollset(). */
  ಯದಿ(!result && data->conn) {
    ವಿಕಲ್ಪ(data->mstate) {
    ಪಕ್ಷ MSTATE_INIT:
    ಪಕ್ಷ MSTATE_PENDING:
    ಪಕ್ಷ MSTATE_SETUP:
    ಪಕ್ಷ MSTATE_CONNECT:
      /* nothing to poll for yet */
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_CONNECTING:
      result = mstate_connecting_pollset(data, ps);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_PROTOCONNECT:
    ಪಕ್ಷ MSTATE_PROTOCONNECTING:
      result = mstate_protocol_pollset(data, ps);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_DO:
    ಪಕ್ಷ MSTATE_DOING:
      result = mstate_do_pollset(data, ps);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_DOING_MORE:
      result = mstate_domore_pollset(data, ps);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_DID: /* same as PERFORMING in regard to polling */
    ಪಕ್ಷ MSTATE_PERFORMING:
      result = mstate_perform_pollset(data, ps);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_RATELIMITING:
      /* we need to let time pass, ignore socket(s) */
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_DONE:
    ಪಕ್ಷ MSTATE_COMPLETED:
    ಪಕ್ಷ MSTATE_MSGSENT:
      /* nothing more to poll for */
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      failf(data, "multi_getsock: unexpected multi state %d",
            (ಪೂರ್ಣಾಙ್ಕ)data->mstate);
      DEBUGASSERT(0);
      ಭಙ್ಗ;
    }
  }

  ಯದಿ(result) {
    ಯದಿ(result == CURLE_OUT_OF_MEMORY)
      ಪ್ರತ್ಯಾಗಮ CURLM_OUT_OF_MEMORY;
    failf(data, "error determining pollset: %d", (ಪೂರ್ಣಾಙ್ಕ)result);
    ಪ್ರತ್ಯಾಗಮ CURLM_INTERNAL_ERROR;
  }

#ifdef CURLVERBOSE
  ಯದಿ(CURL_TRC_M_is_verbose(data)) {
    size_t timeout_ಎಣಿಕೆ = multi_timeouts_ಎಣಿಕೆ(&data->state.timeouts);
    ವಿಕಲ್ಪ(ps->n) {
    ಪಕ್ಷ 0:
      CURL_TRC_M(data, "pollset[], timeouts=%zu, paused %d/%d (r/w)",
                 timeout_ಎಣಿಕೆ,
                 Curl_xfer_send_is_paused(data),
                 Curl_xfer_recv_is_paused(data));
      ಭಙ್ಗ;
    ಪಕ್ಷ 1:
      CURL_TRC_M(data, "pollset[fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_OUT) ? "OUT" : "",
                 timeout_ಎಣಿಕೆ);
      ಭಙ್ಗ;
    ಪಕ್ಷ 2:
      CURL_TRC_M(data, "pollset[fd=%" FMT_SOCKET_T " %s%s, "
                 "fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_OUT) ? "OUT" : "",
                 ps->sockets[1],
                 (ps->actions[1] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[1] & CURL_POLL_OUT) ? "OUT" : "",
                 timeout_ಎಣಿಕೆ);
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      CURL_TRC_M(data, "pollset[fds=%u], timeouts=%zu", ps->n, timeout_ಎಣಿಕೆ);
      ಭಙ್ಗ;
    }
    CURL_TRC_EASY_TIMERS(data);
  }
#endif

  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

CURLMcode curl_multi_fdset(CURLM *m,
                           fd_set *read_fd_set, fd_set *write_fd_set,
                           fd_set *exc_fd_set, ಪೂರ್ಣಾಙ್ಕ *max_fd)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_fdset, &mresult)) {
    /* Scan through all the easy handles to get the file descriptors set.
       Some easy handles may not have connected to the remote host yet,
       and then we must make sure that is done. */
    ಸಂರಚನಾ Curl_multi *multi = m;
    ಸಂರಚನಾ easy_pollset ps;
    ಪೂರ್ಣಾಙ್ಕ this_max_fd = -1;
    unsigned ಪೂರ್ಣಾಙ್ಕ i;
    uint32_t mid;
    (ಶೂನ್ಯ)exc_fd_set;

    Curl_pollset_init(&ps);
    ಯದಿ(Curl_uint32_bset_first(&multi->process, &mid)) {
      ಕುರು {
        ಸಂರಚನಾ Curl_easy *data = Curl_multi_get_easy(multi, mid);

        ಯದಿ(!data) {
          DEBUGASSERT(0);
          ಅನುವರ್ತ;
        }

        Curl_multi_pollset(data, &ps);
        ಪ್ರತಿ(i = 0; i < ps.n; i++) {
          ಯದಿ(!FDSET_SOCK(ps.sockets[i]))
            /* pretend it does not exist */
            ಅನುವರ್ತ;
          ಯದಿ(ps.actions[i] & CURL_POLL_IN)
            FD_SET(ps.sockets[i], read_fd_set);
          ಯದಿ(ps.actions[i] & CURL_POLL_OUT)
            FD_SET(ps.sockets[i], write_fd_set);
          ಯದಿ((ಪೂರ್ಣಾಙ್ಕ)ps.sockets[i] > this_max_fd)
            this_max_fd = (ಪೂರ್ಣಾಙ್ಕ)ps.sockets[i];
        }
      } ಯಾವತ್(Curl_uint32_bset_next(&multi->process, mid, &mid));
    }

    Curl_cshutdn_setfds(&multi->cshutdn, multi->admin,
                        read_fd_set, write_fd_set, &this_max_fd);

    *max_fd = this_max_fd;
    Curl_pollset_cleanup(&ps);

    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

CURLMcode curl_multi_waitfds(CURLM *m,
                             ಸಂರಚನಾ curl_waitfd *ufds,
                             unsigned ಪೂರ್ಣಾಙ್ಕ size,
                             unsigned ಪೂರ್ಣಾಙ್ಕ *fd_ಎಣಿಕೆ)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_waitfds, &mresult)) {
    ಸಂರಚನಾ Curl_waitfds cwfds;
    ಸಂರಚನಾ Curl_multi *multi = m;
    ಸಂರಚನಾ easy_pollset ps;
    unsigned ಪೂರ್ಣಾಙ್ಕ need = 0;
    uint32_t mid;

    ಯದಿ(!ufds && (size || !fd_ಎಣಿಕೆ)) {
      mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      ಗಚ್ಛ out;
    }

    Curl_pollset_init(&ps);
    Curl_waitfds_init(&cwfds, ufds, size);
    mresult = CURLM_OK;
    ಯದಿ(Curl_uint32_bset_first(&multi->process, &mid)) {
      ಕುರು {
        ಸಂರಚನಾ Curl_easy *data = Curl_multi_get_easy(multi, mid);
        ಯದಿ(!data) {
          DEBUGASSERT(0);
          Curl_uint32_bset_remove(&multi->process, mid);
          Curl_uint32_bset_remove(&multi->dirty, mid);
          ಅನುವರ್ತ;
        }
        Curl_multi_pollset(data, &ps);
        need += Curl_waitfds_add_ps(&cwfds, &ps);
      } ಯಾವತ್(Curl_uint32_bset_next(&multi->process, mid, &mid));
    }

    need += Curl_cshutdn_add_waitfds(&multi->cshutdn, multi->admin, &cwfds);

    ಯದಿ(need != cwfds.n && ufds)
      mresult = CURLM_OUT_OF_MEMORY;

    ಯದಿ(fd_ಎಣಿಕೆ)
      *fd_ಎಣಿಕೆ = need;
    Curl_pollset_cleanup(&ps);
  }
out:
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

#ifdef USE_WINSOCK
/* Reset FD_WRITE for TCP sockets. Nothing is actually sent. UDP sockets cannot
 * be reset this way because an empty datagram would be sent. #9203
 *
 * "On Windows the internal state of FD_WRITE as returned from
 * WSAEnumNetworkEvents is only reset after successful send()."
 */
ಸ್ಥಿರ ಶೂನ್ಯ reset_socket_fdwrite(curl_socket_t s)
{
  ಪೂರ್ಣಾಙ್ಕ t;
  ಪೂರ್ಣಾಙ್ಕ l = (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(t);
  ಯದಿ(!getsockopt(s, SOL_SOCKET, SO_TYPE, (ವರ್ಣ *)&t, &l) && t == SOCK_STREAM)
    swrite(s, ರಿಕ್ತ, 0);
}

ಸ್ಥಿರ CURLMcode multi_winsock_select(ಸಂರಚನಾ Curl_multi *multi,
                                      ಸಂರಚನಾ curl_pollfds *cpfds,
                                      unsigned ಪೂರ್ಣಾಙ್ಕ curl_nfds,
                                      ಸಂರಚನಾ curl_waitfd extra_fds[],
                                      unsigned ಪೂರ್ಣಾಙ್ಕ extra_nfds,
                                      ಪೂರ್ಣಾಙ್ಕ timeout_ms,
                                      bool extrawait,
                                      ಪೂರ್ಣಾಙ್ಕ *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  WSANETWORKEVENTS wsa_events;
  ಪೂರ್ಣಾಙ್ಕ nevents = 0;
  size_t i;

  DEBUGASSERT(multi->wsa_event != WSA_INVALID_EVENT);

  /* Set the WSA events based on the collected pollds */
  ಪ್ರತಿ(i = 0; i < cpfds->n; i++) {
    long mask = 0;
    ಯದಿ(cpfds->pfds[i].events & POLLIN)
      mask |= FD_READ | FD_ACCEPT | FD_CLOSE;
    ಯದಿ(cpfds->pfds[i].events & POLLPRI)
      mask |= FD_OOB;
    ಯದಿ(cpfds->pfds[i].events & POLLOUT) {
      mask |= FD_WRITE | FD_CONNECT | FD_CLOSE;
      reset_socket_fdwrite(cpfds->pfds[i].fd);
    }
    ಯದಿ(mask && WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, mask)) {
      mresult = CURLM_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }

  ಯದಿ(cpfds->n || extrawait) {
    ಪೂರ್ಣಾಙ್ಕ pollrc = 0;

    ಯದಿ(cpfds->n) {         /* pre-check with Winsock */
      pollrc = Curl_poll(cpfds->pfds, cpfds->n, 0);
      ಯದಿ(pollrc < 0) {
        mresult = CURLM_UNRECOVERABLE_POLL;
        ಗಚ್ಛ out;
      }
      nevents = pollrc;
    }

    ಯದಿ(!nevents) {
      /* now wait... if not ready during the pre-check (pollrc == 0) */
      WSAWaitForMultipleEvents(1, &multi->wsa_event, FALSE, (DWORD)timeout_ms,
                               FALSE);
    }

    /* With Winsock, we have to run the following section unconditionally
       to call WSAEventSelect(fd, event, 0) on all the sockets */
    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    ಪ್ರತಿ(i = 0; i < extra_nfds; i++) {
      unsigned short mask = 0;
      curl_socket_t s = extra_fds[i].fd;

      wsa_events.lNetworkEvents = 0;
      ಯದಿ(WSAEnumNetworkEvents(s, ರಿಕ್ತ, &wsa_events) == 0) {
        ಯದಿ(wsa_events.lNetworkEvents & (FD_READ | FD_ACCEPT | FD_CLOSE))
          mask |= CURL_WAIT_POLLIN;
        ಯದಿ(wsa_events.lNetworkEvents & (FD_WRITE | FD_CONNECT | FD_CLOSE))
          mask |= CURL_WAIT_POLLOUT;
        ಯದಿ(wsa_events.lNetworkEvents & FD_OOB)
          mask |= CURL_WAIT_POLLPRI;
        ಯದಿ(!pollrc && wsa_events.lNetworkEvents)
          nevents++;
      }
      WSAEventSelect(s, multi->wsa_event, 0);
      ಯದಿ(!pollrc) {
        extra_fds[i].revents = (short)mask;
        ಅನುವರ್ತ;
      }
      ಅನ್ಯಥಾ {
        unsigned r = (unsigned)cpfds->pfds[curl_nfds + i].revents;
        ಯದಿ(r & POLLIN)
          mask |= CURL_WAIT_POLLIN;
        ಯದಿ(r & POLLOUT)
          mask |= CURL_WAIT_POLLOUT;
        ಯದಿ(r & POLLPRI)
          mask |= CURL_WAIT_POLLPRI;
        extra_fds[i].revents = (short)mask;
      }
    }

    /* Count up all our own sockets that had activity,
       and remove them from the event. */
    ಪ್ರತಿ(i = 0; i < curl_nfds; ++i) {
      wsa_events.lNetworkEvents = 0;
      ಯದಿ(WSAEnumNetworkEvents(cpfds->pfds[i].fd, ರಿಕ್ತ, &wsa_events) == 0) {
        ಯದಿ(!pollrc && wsa_events.lNetworkEvents)
          nevents++;
      }
      WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, 0);
    }
    WSAResetEvent(multi->wsa_event);
  }

out:
  *pnevents = nevents;
  ಪ್ರತ್ಯಾಗಮ mresult;
}

#ಅನ್ಯಥಾ /* USE_WINSOCK */

ಸ್ಥಿರ CURLMcode multi_posix_poll(ಸಂರಚನಾ Curl_multi *multi,
                                  ಸಂರಚನಾ curl_pollfds *cpfds,
                                  unsigned ಪೂರ್ಣಾಙ್ಕ curl_nfds,
                                  ಸಂರಚನಾ curl_waitfd extra_fds[],
                                  unsigned ಪೂರ್ಣಾಙ್ಕ extra_nfds,
                                  ಪೂರ್ಣಾಙ್ಕ timeout_ms,
                                  bool extrawait,
                                  ಪೂರ್ಣಾಙ್ಕ *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  ಪೂರ್ಣಾಙ್ಕ nevents = 0;
  size_t i;

  (ಶೂನ್ಯ)multi;
  ಯದಿ(cpfds->n) {
    ಪೂರ್ಣಾಙ್ಕ pollrc = Curl_poll(cpfds->pfds, cpfds->n, timeout_ms); /* wait... */
    ಯದಿ(pollrc < 0) {
      mresult = CURLM_UNRECOVERABLE_POLL;
      ಗಚ್ಛ out;
    }
    nevents = pollrc;

    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    ಪ್ರತಿ(i = 0; i < extra_nfds; i++) {
      unsigned r = (unsigned)cpfds->pfds[curl_nfds + i].revents;
      unsigned short mask = 0;
      ಯದಿ(r & POLLIN)
        mask |= CURL_WAIT_POLLIN;
      ಯದಿ(r & POLLOUT)
        mask |= CURL_WAIT_POLLOUT;
      ಯದಿ(r & POLLPRI)
        mask |= CURL_WAIT_POLLPRI;
      extra_fds[i].revents = (short)mask;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(extrawait) {
    /* No fds to poll, but asked to obey timeout_ms anyway. We cannot
     * use Curl_poll() as it, on some platforms, returns immediately
     * without fds. */
    curlx_wait_ms(timeout_ms);
  }

out:
  *pnevents = nevents;
  ಪ್ರತ್ಯಾಗಮ mresult;
}

#endif /* !USE_WINSOCK */

#define ಅಂಕೆ_POLLS_ON_ರಾಶಿ 10

ಸ್ಥಿರ CURLMcode multi_wait(ಸಂರಚನಾ Curl_multi *multi,
                            ಸಂರಚನಾ curl_waitfd extra_fds[],
                            unsigned ಪೂರ್ಣಾಙ್ಕ extra_nfds,
                            ಪೂರ್ಣಾಙ್ಕ timeout_ms,
                            ಪೂರ್ಣಾಙ್ಕ *ret,
                            bool extrawait)  /* when no socket, wait */
{
  size_t i;
  ಪೂರ್ಣಾಙ್ಕ timeout_internal;
  ಪೂರ್ಣಾಙ್ಕ nevents = 0;
  ಸಂರಚನಾ easy_pollset ps;
  ಸಂರಚನಾ pollfd a_few_on_ರಾಶಿ[ಅಂಕೆ_POLLS_ON_ರಾಶಿ];
  ಸಂರಚನಾ curl_pollfds cpfds;
  unsigned ಪೂರ್ಣಾಙ್ಕ curl_nfds = 0; /* how many pfds are for curl transfers */
  ಸಂರಚನಾ Curl_easy *data = ರಿಕ್ತ;
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;
#ifdef ENABLE_WAKEUP
  ಪೂರ್ಣಾಙ್ಕ wakeup_idx = -1;
#endif

  ಯದಿ(timeout_ms < 0)
    ಪ್ರತ್ಯಾಗಮ CURLM_BAD_FUNCTION_ARGUMENT;

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ರಾಶಿ, ಅಂಕೆ_POLLS_ON_ರಾಶಿ);

  /* Add the curl handles to our pollfds first */
  ಯದಿ(Curl_uint32_bset_first(&multi->process, &mid)) {
    ಕುರು {
      data = Curl_multi_get_easy(multi, mid);
      ಯದಿ(!data) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        ಅನುವರ್ತ;
      }
      Curl_multi_pollset(data, &ps);
      ಯದಿ(Curl_pollfds_add_ps(&cpfds, &ps)) {
        mresult = CURLM_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
    } ಯಾವತ್(Curl_uint32_bset_next(&multi->process, mid, &mid));
  }

  ಯದಿ(Curl_cshutdn_add_pollfds(&multi->cshutdn, multi->admin, &cpfds)) {
    mresult = CURLM_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

#ifdef ENABLE_WAKEUP
  /* If `extrawait` is TRUE *or* we have `extra_fds`to poll *or* we
   * have transfer sockets to poll, we obey `timeout_ms`.
   * Then we need to also monitor the multi's wakeup
   * socket to catch calls to `curl_multi_wakeup()` during the wait. */
  ಯದಿ(extrawait || cpfds.n || extra_nfds) {
    wakeup_idx = cpfds.n;
    ಯದಿ(Curl_pollfds_add_sock(&cpfds, multi->wakeup_pair[0], POLLIN)) {
      mresult = CURLM_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }
#endif

  curl_nfds = cpfds.n; /* what curl internally uses in cpfds */
  /* Add external file descriptions from poll-like struct curl_waitfd */
  ಪ್ರತಿ(i = 0; i < extra_nfds; i++) {
    unsigned short events = 0;
    ಯದಿ(extra_fds[i].events & CURL_WAIT_POLLIN)
      events |= POLLIN;
    ಯದಿ(extra_fds[i].events & CURL_WAIT_POLLPRI)
      events |= POLLPRI;
    ಯದಿ(extra_fds[i].events & CURL_WAIT_POLLOUT)
      events |= POLLOUT;
    ಯದಿ(Curl_pollfds_add_sock(&cpfds, extra_fds[i].fd, events)) {
      mresult = CURLM_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }

  /* We check the internal timeout *AFTER* we collected all sockets to
   * poll. Collecting the sockets may install new timers by protocols
   * and connection filters.
   * Use the shorter one of the internal and the caller requested timeout.
   * If we are called with `!extrawait` and multi_timeout() reports no
   * timeouts exist, do not wait. */
  multi_timeout(multi, ರಿಕ್ತ, &timeout_internal);
  ಯದಿ((timeout_internal >= 0) && (timeout_internal < timeout_ms))
    timeout_ms = timeout_internal;

  ಯದಿ(data)
    CURL_TRC_M(data, "multi_wait(fds=%u, timeout=%d) tinternal=%d",
               cpfds.n, timeout_ms, timeout_internal);

#ifdef USE_WINSOCK
  mresult = multi_winsock_select(multi, &cpfds, curl_nfds,
                                 extra_fds, extra_nfds,
                                 timeout_ms, extrawait, &nevents);
#ಅನ್ಯಥಾ
  mresult = multi_posix_poll(multi, &cpfds, curl_nfds,
                             extra_fds, extra_nfds,
                             timeout_ms, extrawait, &nevents);
#endif

#ifdef ENABLE_WAKEUP
    ಯದಿ(nevents && (wakeup_idx >= 0)) {
      ಯದಿ(cpfds.pfds[wakeup_idx].revents & POLLIN) {
        (ಶೂನ್ಯ)Curl_wakeup_consume(multi->wakeup_pair, TRUE);
        /* do not count the wakeup socket into the returned value */
        nevents--;
      }
    }
#endif

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  ಯದಿ(ret)
    *ret = nevents;
  ಪ್ರತ್ಯಾಗಮ mresult;
}

CURLMcode curl_multi_wait(CURLM *m,
                          ಸಂರಚನಾ curl_waitfd extra_fds[],
                          unsigned ಪೂರ್ಣಾಙ್ಕ extra_nfds,
                          ಪೂರ್ಣಾಙ್ಕ timeout_ms,
                          ಪೂರ್ಣಾಙ್ಕ *ret)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_wait, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, FALSE);
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

CURLMcode curl_multi_poll(CURLM *m,
                          ಸಂರಚನಾ curl_waitfd extra_fds[],
                          unsigned ಪೂರ್ಣಾಙ್ಕ extra_nfds,
                          ಪೂರ್ಣಾಙ್ಕ timeout_ms,
                          ಪೂರ್ಣಾಙ್ಕ *ret)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_poll, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, TRUE);
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

CURLMcode curl_multi_wakeup(CURLM *m)
{
  /* this function is usually called from another thread,
     it has to be careful only to access parts of the
     Curl_multi struct that are constant */
  ಸಂರಚನಾ Curl_multi *multi = m;
  CURLMcode mresult = CURLM_WAKEUP_FAILURE;

  /* GOOD_MULTI_HANDLE can be safely called */
  ಯದಿ(!GOOD_MULTI_HANDLE(multi))
    ಪ್ರತ್ಯಾಗಮ CURLM_BAD_HANDLE;

#ifdef ENABLE_WAKEUP
  /* the wakeup_pair variable is only written during init and cleanup,
     making it safe to access from another thread after the init part
     and before cleanup */
  ಯದಿ(!Curl_wakeup_signal(multi->wakeup_pair))
    mresult = CURLM_OK;
#endif
#ifdef USE_WINSOCK
  ಯದಿ(WSASetEvent(multi->wsa_event))
    mresult = CURLM_OK;
#endif
  ಪ್ರತ್ಯಾಗಮ mresult;
}

#ifdef ENABLE_INTERNAL_WAKEUP
ಶೂನ್ಯ Curl_multi_wakeup_internal(ಸಂರಚನಾ Curl_multi *multi)
{
  /* This is expected to be invocable from another thread which
   * does NOT outlive the multi handle. Check for sanity. */
  ಯದಿ(GOOD_MULTI_HANDLE(multi))
    Curl_wakeup_signal(multi->wakeup_internal);
  ಅನ್ಯಥಾ
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
ಸ್ಥಿರ bool multi_ischanged(ಸಂರಚನಾ Curl_multi *multi, bool clear)
{
  bool retval = FALSE;
  DEBUGASSERT(multi);
  ಯದಿ(multi) {
    retval = (bool)multi->recheckstate;
    ಯದಿ(clear)
      multi->recheckstate = FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Curl_multi_connchanged() is called to tell that there is a connection in
 * this multi handle that has changed state (multiplexing become possible, the
 * number of allowed streams changed or similar), and a subsequent use of this
 * multi handle should move CONNECT_PEND handles back to CONNECT to have them
 * retry.
 */
ಶೂನ್ಯ Curl_multi_connchanged(ಸಂರಚನಾ Curl_multi *multi)
{
  /* May be invoked for a `data->multi` when no multi is present. */
  ಯದಿ(multi)
    multi->recheckstate = TRUE;
}

CURLMcode Curl_multi_add_perform(ಸಂರಚನಾ Curl_multi *multi,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ connectdata *conn)
{
  CURLMcode mresult;

  mresult = Curl_multi_add_handle(multi, data);
  ಯದಿ(!mresult) {
    CURLcode result;

    /* pass in NULL for 'conn' here since we do not want to init the
       connection, only this transfer */
    result = Curl_init_transfer(data, ರಿಕ್ತ);
    ಯದಿ(result) {
      Curl_multi_remove_handle(multi, data);
      ಪ್ರತ್ಯಾಗಮ CURLM_INTERNAL_ERROR;
    }

    /* take this handle to the perform state right away */
    multistate(data, MSTATE_PERFORMING);
    Curl_attach_connection(data, conn, TRUE);
    CURL_REQ_SET_RECV(data);
  }
  ಪ್ರತ್ಯಾಗಮ mresult;
}

ಸ್ಥಿರ CURLcode multi_do(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(conn->scheme);

  ಯದಿ(conn->scheme->run->do_it)
    result = conn->scheme->run->do_it(data, done);

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * multi_do_more() is called during the DO_MORE multi state. It is a second
 * stage DO state which (wrongly) was introduced to support FTP's second
 * connection.
 *
 * 'complete' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (to DOING state when there is more work to do)
 */

ಸ್ಥಿರ CURLcode multi_do_more(ಸಂರಚನಾ Curl_easy *data, domore *complete)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;

  *complete = DOMORE_INCOMPLETE;

  ಯದಿ(conn->scheme->run->do_more)
    result = conn->scheme->run->do_more(data, complete);

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Check whether a timeout occurred, and handle it if it did
 */
ಸ್ಥಿರ bool multi_handle_timeout(ಸಂರಚನಾ Curl_easy *data,
                                 ಧ್ರುವ ಸಂರಚನಾ curltime *pnow,
                                 bool *stream_error,
                                 CURLcode *result)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_timeleft_now_ms(data, pnow);
  ಯದಿ(timeout_ms < 0) {
    /* Handle timed out */
    timerid base_timer = Curl_is_connecting(data) ?
                         TIMER_STARTSINGLE : TIMER_STARTOP;
    timediff_t elapsed_ms = Curl_pgrs_since_ms(data, ರಿಕ್ತ, base_timer);
    ಯದಿ(data->mstate == MSTATE_CONNECTING) {
      failf(data, "%s timed out after %" FMT_TIMEDIFF_T " milliseconds",
            data->conn->bits.dns_resolved ? "Connection" : "Resolving",
            elapsed_ms);
      /* keep what we know about a partly done connect, e.g. TCP done
       * but TLS handshake still ongoing */
      Curl_conn_report_failed(data, FIRSTSOCKET);
    }
    ಅನ್ಯಥಾ {
      ಸಂರಚನಾ SingleRequest *k = &data->req;
      ಯದಿ(k->size != -1) {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount, k->size);
      }
      ಅನ್ಯಥಾ {
        failf(data, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount);
      }
    }
    *result = CURLE_OPERATION_TIMEDOUT;
    ಯದಿ(data->conn) {
      /* Force connection closed if the connection has indeed been used */
      ಯದಿ(data->mstate > MSTATE_DO) {
        streamclose(data->conn);
        *stream_error = TRUE;
      }
      (ಶೂನ್ಯ)multi_done(data, *result, TRUE);
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * We are doing protocol-specific connecting and this is being called over and
 * over from the multi interface until the connection phase is done on
 * protocol layer.
 */

ಸ್ಥಿರ CURLcode protocol_connecting(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;

  ಯದಿ(conn && conn->scheme->run->connecting) {
    *done = FALSE;
    result = conn->scheme->run->connecting(data, done);
  }
  ಅನ್ಯಥಾ
    *done = TRUE;

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * We are DOING this is being called over and over from the multi interface
 * until the DOING phase is done on protocol layer.
 */

ಸ್ಥಿರ CURLcode protocol_doing(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ connectdata *conn = data->conn;

  ಯದಿ(conn && conn->scheme->run->doing) {
    *done = FALSE;
    result = conn->scheme->run->doing(data, done);
  }
  ಅನ್ಯಥಾ
    *done = TRUE;

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * We have discovered that the TCP connection has been successful, we can now
 * proceed with some action.
 *
 */
ಸ್ಥಿರ CURLcode protocol_connect(ಸಂರಚನಾ Curl_easy *data, bool *protocol_done)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(conn);
  DEBUGASSERT(protocol_done);
  DEBUGASSERT(Curl_conn_is_connected(conn, FIRSTSOCKET));

  *protocol_done = FALSE;
  ಯದಿ(!conn->bits.protoconnstart) {
    ಯದಿ(conn->scheme->run->connect_it) {
      /* Call the protocol-specific connect function */
      result = conn->scheme->run->connect_it(data, protocol_done);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    conn->bits.protoconnstart = TRUE;
  }

  /* Unless this protocol does not have any protocol-connect callback, as
     then we know we are done. */
  ಯದಿ(!conn->scheme->run->connecting)
    *protocol_done = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * posttransfer() is called immediately after a transfer ends
 */
ಸ್ಥಿರ ಶೂನ್ಯ multi_posttransfer(ಸಂರಚನಾ Curl_easy *data)
{
#ಯದಿ defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
  /* restore the signal handler for SIGPIPE before we get back */
  ಯದಿ(!data->set.no_signal)
    ಸಙ್ಕೇತ(SIGPIPE, data->state.prev_signal);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
#endif
}

/*
 * multi_follow() handles the URL redirect magic. Pass in the 'newurl' string
 * as given by the remote server and set up the new URL to request.
 *
 * This function DOES NOT FREE the given URL.
 */
ಸ್ಥಿರ CURLcode multi_follow(ಸಂರಚನಾ Curl_easy *data,
                             ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *handler,
                             ಧ್ರುವ ವರ್ಣ *newurl, /* the Location: string */
                             followtype type) /* see transfer.h */
{
  ಯದಿ(handler && handler->run->follow)
    ಪ್ರತ್ಯಾಗಮ handler->run->follow(data, newurl, type);

  ಯದಿ(type == FOLLOW_RETRY)
    /* Retries are generic and do not require protocol-specific redirect
       handling. */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಪ್ರತ್ಯಾಗಮ CURLE_TOO_MANY_REDIRECTS;
}

ಸ್ಥಿರ CURLcode mspeed_check(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(Curl_rlimit_active(&data->progress.dl.rlimit) ||
     Curl_rlimit_active(&data->progress.ul.rlimit)) {
    /* check if our send/recv limits require idle waits */
    ಧ್ರುವ ಸಂರಚನಾ curltime *pnow = Curl_pgrs_now(data);
    timediff_t recv_ms, send_ms;

    send_ms = Curl_rlimit_wait_ms(&data->progress.ul.rlimit, pnow);
    recv_ms = Curl_rlimit_wait_ms(&data->progress.dl.rlimit, pnow);

    ಯದಿ(send_ms || recv_ms) {
      ಯದಿ(data->mstate != MSTATE_RATELIMITING) {
        multistate(data, MSTATE_RATELIMITING);
      }
      Curl_expire_set(data, EXPIRE_TOOFAST, CURLMAX(send_ms, recv_ms), pnow);
      Curl_multi_clear_dirty(data);
      CURL_TRC_M(data, "[RLIMIT] waiting %" FMT_TIMEDIFF_T "ms",
                 CURLMAX(send_ms, recv_ms));
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    }
    ಅನ್ಯಥಾ {
      /* when will the rate limits increase next? The transfer needs
       * to run again at that time or it may stall. */
      send_ms = Curl_rlimit_next_step_ms(&data->progress.ul.rlimit, pnow);
      recv_ms = Curl_rlimit_next_step_ms(&data->progress.dl.rlimit, pnow);
      ಯದಿ(send_ms || recv_ms) {
        timediff_t next_ms = CURLMIN(send_ms, recv_ms);
        ಯದಿ(!next_ms)
          next_ms = CURLMAX(send_ms, recv_ms);
        Curl_expire_set(data, EXPIRE_TOOFAST, next_ms, pnow);
        CURL_TRC_M(data, "[RLIMIT] next token update in %" FMT_TIMEDIFF_T "ms",
                   next_ms);
      }
    }
  }

  ಯದಿ(data->mstate != MSTATE_PERFORMING) {
    CURL_TRC_M(data, "[RLIMIT] wait over, continue");
    multistate(data, MSTATE_PERFORMING);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLMcode multistate_performing(ಸಂರಚನಾ Curl_easy *data,
                                       bool *stream_errorp,
                                       CURLcode *resultp)
{
  ವರ್ಣ *newurl = ರಿಕ್ತ;
  bool retry = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = *resultp = CURLE_OK;
  *stream_errorp = FALSE;

  ಯದಿ(mspeed_check(data) == CURLE_AGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLM_OK;

  /* read/write data if it is ready to do so */
  result = Curl_sendrecv(data);

  ಯದಿ(data->req.done || (result == CURLE_RECV_ERROR)) {
    /* If CURLE_RECV_ERROR happens early enough, we assume it was a race
     * condition and the server closed the reused connection exactly when we
     * wanted to use it, so figure out if that is indeed the case.
     */
    CURLcode ret = Curl_retry_request(data, &newurl);
    ಯದಿ(!ret)
      retry = !!newurl;
    ಅನ್ಯಥಾ ಯದಿ(!result)
      result = ret;

    ಯದಿ(retry) {
      /* if we are to retry, set the result to OK and consider the
         request as done */
      result = CURLE_OK;
      data->req.done = TRUE;
    }
  }
#ifndef CURL_DISABLE_HTTP
  ಅನ್ಯಥಾ ಯದಿ((result == CURLE_HTTP2_STREAM) &&
          Curl_h2_http_1_1_error(data)) {
    CURLcode ret = Curl_retry_request(data, &newurl);

    ಯದಿ(!ret) {
      infof(data, "Downgrades to HTTP/1.1");
      streamclose(data->conn);
      data->state.http_neg.wanted = CURL_HTTP_V1x;
      data->state.http_neg.allowed = CURL_HTTP_V1x;
      /* clear the error message bit too as we ignore the one we got */
      data->state.errorbuf = FALSE;
      ಯದಿ(!newurl)
        /* typically for HTTP_1_1_REQUIRED error on first flight */
        newurl = Curl_bufref_dup(&data->state.url);
      ಯದಿ(!newurl) {
        result = CURLE_OUT_OF_MEMORY;
      }
      ಅನ್ಯಥಾ {
        /* if we are to retry, set the result to OK and consider the request
          as done */
        retry = TRUE;
        result = CURLE_OK;
        data->req.done = TRUE;
      }
    }
    ಅನ್ಯಥಾ
      result = ret;
  }
#endif

  ಯದಿ(result) {
    /*
     * The transfer phase returned error, we mark the connection to get closed
     * to prevent being reused. This is because we cannot possibly know if the
     * connection is in a good shape or not now. Unless it is a protocol which
     * uses two "channels" like FTP, as then the error happened in the data
     * connection.
     */

    ಯದಿ(!(data->conn->scheme->flags & PROTOPT_DUAL) &&
       result != CURLE_HTTP2_STREAM)
      streamclose(data->conn);

    multi_posttransfer(data);
    multi_done(data, result, TRUE);
  }
  ಅನ್ಯಥಾ ಯದಿ(data->req.done && !Curl_cwriter_is_paused(data)) {
    ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *handler = data->conn->scheme;

    /* call this even if the readwrite function returned error */
    multi_posttransfer(data);

    /* When we follow redirects or is set to retry the connection, we must to
       go back to the CONNECT state */
    ಯದಿ(data->req.newurl || retry) {
      followtype follow = FOLLOW_NONE;
      ಯದಿ(!retry) {
        /* if the URL is a follow-location and not a retried request then
           figure out the URL here */
        curlx_free(newurl);
        newurl = data->req.newurl;
        data->req.newurl = ರಿಕ್ತ;
        follow = FOLLOW_REDIR;
      }
      ಅನ್ಯಥಾ
        follow = FOLLOW_RETRY;
      (ಶೂನ್ಯ)multi_done(data, CURLE_OK, FALSE);
      /* multi_done() might return CURLE_GOT_NOTHING */
      result = multi_follow(data, handler, newurl, follow);
      ಯದಿ(!result) {
        multistate(data, MSTATE_SETUP);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    ಅನ್ಯಥಾ {
      /* after the transfer is done, go DONE */

      /* but first check to see if we got a location info even though we are
         not following redirects */
      ಯದಿ(data->req.location) {
        curlx_free(newurl);
        newurl = data->req.location;
        data->req.location = ರಿಕ್ತ;
        result = multi_follow(data, handler, newurl, FOLLOW_FAKE);
        ಯದಿ(result) {
          *stream_errorp = TRUE;
          result = multi_done(data, result, TRUE);
        }
      }

      ಯದಿ(!result) {
        multistate(data, MSTATE_DONE);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
  }
  ಅನ್ಯಥಾ { /* not errored, not done */
    mspeed_check(data);
  }
  curlx_free(newurl);
  *resultp = result;
  ಪ್ರತ್ಯಾಗಮ mresult;
}

ಸ್ಥಿರ CURLMcode multistate_do(ಸಂರಚನಾ Curl_easy *data,
                               bool *stream_errorp,
                               CURLcode *resultp)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;
  ಯದಿ(data->set.fprereq) {
    ಸಂರಚನಾ Curl_mapi_guard guard;
    ಪೂರ್ಣಾಙ್ಕ prereq_rc;

    /* call the prerequest callback function */
    CURL_CBAPI_START(&guard, data, easy_fprereq);
    prereq_rc = data->set.fprereq(data->set.prereq_userp,
                                  data->info.primary.remote_ip,
                                  data->info.primary.local_ip,
                                  data->info.primary.remote_port,
                                  data->info.primary.local_port);
    CURL_CBAPI_END(&guard);
    ಯದಿ(prereq_rc != CURL_PREREQFUNC_OK) {
      failf(data, "operation aborted by pre-request callback");
      /* failure in pre-request callback - do not do any other processing */
      result = CURLE_ABORTED_BY_CALLBACK;
      multi_posttransfer(data);
      multi_done(data, result, FALSE);
      *stream_errorp = TRUE;
      ಗಚ್ಛ end;
    }
  }

  ಯದಿ(data->set.connect_only && !data->set.connect_only_ws) {
    multistate(data, MSTATE_DONE);
    mresult = CURLM_CALL_MULTI_PERFORM;
  }
  ಅನ್ಯಥಾ {
    bool dophase_done = FALSE;
    /* Perform the protocol's DO action */
    result = multi_do(data, &dophase_done);

    ಯದಿ(!result) {
      ಯದಿ(!dophase_done) {
#ifndef CURL_DISABLE_FTP
        /* some steps needed for wildcard matching */
        ಯದಿ(data->state.wildcardmatch) {
          ಸಂರಚನಾ WildcardData *wc = data->wildcard;
          ಯದಿ(wc->state == CURLWC_DONE || wc->state == CURLWC_SKIP) {
            /* skip some states if it is important */
            multi_done(data, CURLE_OK, FALSE);

            /* if there is no connection left, skip the DONE state */
            multistate(data, data->conn ? MSTATE_DONE : MSTATE_COMPLETED);
            mresult = CURLM_CALL_MULTI_PERFORM;
            ಗಚ್ಛ end;
          }
        }
#endif
        /* DO was not completed in one function call, we must continue
           DOING... */
        multistate(data, MSTATE_DOING);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }

      /* after DO, go DO_DONE... or DO_MORE */
      ಅನ್ಯಥಾ ಯದಿ(data->conn->bits.do_more) {
        /* we are supposed to do more, but we need to sit down, relax and wait
           a little while first */
        multistate(data, MSTATE_DOING_MORE);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
      ಅನ್ಯಥಾ {
        /* we are done with the DO, now DID */
        multistate(data, MSTATE_DID);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ((result == CURLE_SEND_ERROR) &&
            data->conn->bits.reuse) {
      /*
       * In this situation, a connection that we were trying to use may have
       * unexpectedly died. If possible, send the connection back to the
       * CONNECT phase so we can try again.
       */
      ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *handler = data->conn->scheme;
      ವರ್ಣ *newurl = ರಿಕ್ತ;
      followtype follow = FOLLOW_NONE;
      CURLcode drc;

      drc = Curl_retry_request(data, &newurl);
      ಯದಿ(drc) {
        /* a failure here pretty much implies an out of memory */
        result = drc;
        *stream_errorp = TRUE;
      }

      multi_posttransfer(data);
      drc = multi_done(data, result, FALSE);

      /* When set to retry the connection, we must go back to the CONNECT
       * state */
      ಯದಿ(newurl) {
        ಯದಿ(!drc || (drc == CURLE_SEND_ERROR)) {
          follow = FOLLOW_RETRY;
          drc = multi_follow(data, handler, newurl, follow);
          ಯದಿ(!drc) {
            multistate(data, MSTATE_SETUP);
            mresult = CURLM_CALL_MULTI_PERFORM;
            result = CURLE_OK;
          }
          ಅನ್ಯಥಾ {
            /* Follow failed */
            result = drc;
          }
        }
        ಅನ್ಯಥಾ {
          /* done did not return OK or SEND_ERROR */
          result = drc;
        }
      }
      ಅನ್ಯಥಾ {
        /* Have error handler disconnect conn if we cannot retry */
        *stream_errorp = TRUE;
      }
      curlx_free(newurl);
    }
    ಅನ್ಯಥಾ {
      /* failure detected */
      multi_posttransfer(data);
      ಯದಿ(data->conn)
        multi_done(data, result, FALSE);
      *stream_errorp = TRUE;
    }
  }
end:
  *resultp = result;
  ಪ್ರತ್ಯಾಗಮ mresult;
}

ಸ್ಥಿರ CURLMcode multistate_ratelimiting(ಸಂರಚನಾ Curl_easy *data,
                                         CURLcode *resultp)
{
  CURLcode result = CURLE_OK;
  CURLMcode mresult = CURLM_OK;
  DEBUGASSERT(data->conn);
  /* if both rates are within spec, resume transfer */
  result = Curl_pgrsCheck(data);

  ಯದಿ(result) {
    ಯದಿ(!(data->conn->scheme->flags & PROTOPT_DUAL) &&
       result != CURLE_HTTP2_STREAM)
      streamclose(data->conn);

    multi_posttransfer(data);
    multi_done(data, result, TRUE);
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(!mspeed_check(data))
      mresult = CURLM_CALL_MULTI_PERFORM;
  }
  *resultp = result;
  ಪ್ರತ್ಯಾಗಮ mresult;
}

ಸ್ಥಿರ CURLMcode multistate_connect(ಸಂರಚನಾ Curl_multi *multi,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    CURLcode *resultp)
{
  /* Connect. We want to get a connection identifier filled in. This state can
     be entered from SETUP and from PENDING. */
  bool connected;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = Curl_connect(data, &connected);
  ಯದಿ(result == CURLE_NO_CONNECTION_AVAILABLE) {
    /* There was no connection available. We will go to the pending state and
       wait for an available connection. */
    multistate(data, MSTATE_PENDING);
    /* move from process to pending set */
    Curl_uint32_bset_remove(&multi->process, data->mid);
    Curl_uint32_bset_remove(&multi->dirty, data->mid);
    Curl_uint32_bset_add(&multi->pending, data->mid);
    *resultp = CURLE_OK;
    ಪ್ರತ್ಯಾಗಮ mresult;
  }
  ಅನ್ಯಥಾ
    multi_schedule_pending(data->multi);

  ಯದಿ(!result) {
    /* after the connect has been sent off, go WAITCONNECT unless the
       protocol connect is already done and we can go directly to WAITDO or
       DO! */
    mresult = CURLM_CALL_MULTI_PERFORM;

    ಯದಿ(connected) {
      ಯದಿ(!data->conn->bits.reuse &&
         Curl_conn_is_multiplex(data->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_schedule_pending(data->multi);
      }
      multistate(data, MSTATE_PROTOCONNECT);
    }
    ಅನ್ಯಥಾ {
      multistate(data, MSTATE_CONNECTING);
    }
  }
  *resultp = result;
  ಪ್ರತ್ಯಾಗಮ mresult;
}

/* returns the possibly updated result */
ಸ್ಥಿರ CURLcode is_finished(ಸಂರಚನಾ Curl_multi *multi,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಧ್ರುವ ಸಂರಚನಾ curltime *pnow,
                            bool stream_error,
                            CURLcode result)
{
  ಯದಿ(data->mstate < MSTATE_COMPLETED) {
    ಯದಿ(result) {
      /*
       * If an error was returned, and we are not in completed state now,
       * then we go to completed and consider this transfer aborted.
       */

      /* No attempt to disconnect connections must be made before this -
         connection detach and termination happens only here */

      /* Check if we can move pending requests to send pipe */
      multi_schedule_pending(multi); /* connection */

      ಯದಿ(data->conn) {
        ಯದಿ(stream_error) {
          /* Do not attempt to send data over a connection that timed out */
          bool dead_connection = result == CURLE_OPERATION_TIMEDOUT;
          ಸಂರಚನಾ connectdata *conn = data->conn;

          /* This is where we make sure that the conn pointer is reset.
             We do not have to do this in every case block above where a
             failure is detected */
          Curl_detach_connection(data);
          Curl_conn_close(data, conn, dead_connection);
        }
      }
      ಅನ್ಯಥಾ ಯದಿ(data->mstate == MSTATE_CONNECT) {
        /* Curl_connect() failed */
        multi_posttransfer(data);
        Curl_pgrsUpdate_nometer(data);
      }

      multistate(data, MSTATE_COMPLETED);
      ಪ್ರತ್ಯಾಗಮ result;
    }
    /* if there is still a connection to use, call the progress function */
    ಅನ್ಯಥಾ ಯದಿ(data->conn && Curl_conn_is_connected(data->conn, FIRSTSOCKET)) {
      result = Curl_pgrsUpdateX(data, pnow);
      ಯದಿ(result) {
        /* aborted due to progress callback return code must close the
           connection */
        streamclose(data->conn);

        /* if not yet in DONE state, go there. */
        ಯದಿ(data->mstate < MSTATE_DONE)
          multistate(data, MSTATE_DONE);
        ಪ್ರತ್ಯಾಗಮ result;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ handle_completed(ಸಂರಚನಾ Curl_multi *multi,
                             ಸಂರಚನಾ Curl_easy *data,
                             CURLcode result)
{
  bool ಸಂದೇಶ_to_app = data->master_mid == UINT32_MAX;

  ಯದಿ(!ಸಂದೇಶ_to_app) {
    /* A sub transfer, not reported to the application. Is anyone still
     * interested in processing its results? */
    ಯದಿ(data->sub_xfer_done) {
      ಸಂರಚನಾ Curl_easy *master = Curl_multi_get_easy(multi, data->master_mid);

      CURL_TRC_M(data, "sub xfer done for master %u", data->master_mid);
      ಯದಿ(master)
        data->sub_xfer_done(data, master, result);
      ಅನ್ಯಥಾ
        CURL_TRC_M(data, "master easy %u already gone.", data->master_mid);
    }
  }
  ಅನ್ಯಥಾ {
    /* now fill in the CURLMsg with this info */
    ಸಂರಚನಾ CURLMsg *ಸಂದೇಶ = &data->ಸಂದೇಶ;

    ಸಂದೇಶ->ಸಂದೇಶ = CURLMSG_DONE;
    ಸಂದೇಶ->easy_handle = data;
    ಸಂದೇಶ->data.result = result;

    DEBUGASSERT(!data->conn);
  }
  multistate(data, MSTATE_MSGSENT);

  /* remove from the other sets */
  Curl_uint32_bset_remove(&multi->process, data->mid);
  Curl_uint32_bset_remove(&multi->dirty, data->mid);
  Curl_uint32_bset_remove(&multi->pending, data->mid);
  ಯದಿ(ಸಂದೇಶ_to_app)
    multi_addmsg(multi, data);
  ಯದಿ(data->state.really_alive) {
    data->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    ಯದಿ(!multi->xfers_really_alive)
      (ಶೂನ್ಯ)multi_assess_wakeup(multi);
  }
  --multi->xfers_alive;
  ಯದಿ(!multi->xfers_alive)
    multi_assess_wakeup(multi);
}

ಸ್ಥಿರ CURLMcode multistate_init(ಸಂರಚನಾ Curl_easy *data, CURLcode *result)
{
  ಯದಿ(!data->state.really_alive) {
    data->state.really_alive = TRUE;
    ++data->multi->xfers_really_alive;
    ಯದಿ(data->multi->xfers_really_alive == 1) {
      CURLMcode mresult = multi_assess_wakeup(data->multi);
      ಯದಿ(mresult) {
        failf(data, "error enabling wakeup listening: %d", mresult);
        ಪ್ರತ್ಯಾಗಮ mresult;
      }
    }
  }

  *result = Curl_pretransfer(data);
  ಯದಿ(*result)
    ಪ್ರತ್ಯಾಗಮ CURLM_OK;

  /* after init, go SETUP */
  multistate(data, MSTATE_SETUP);
  Curl_pgrsTime(data, TIMER_STARTOP);
  ಪ್ರತ್ಯಾಗಮ CURLM_CALL_MULTI_PERFORM;
}

ಸ್ಥಿರ CURLMcode multistate_setup(ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow = Curl_pgrs_now(data);
  Curl_pgrsTimeWas(data, TIMER_STARTSINGLE, *pnow);
  ಯದಿ(data->set.timeout)
    Curl_expire_set(data, EXPIRE_TIMEOUT, data->set.timeout, pnow);
  ಯದಿ(data->set.connecttimeout)
    /* Since a connection might go to pending and back to CONNECT several
       times before it actually takes off, we need to set the timeout once
       in SETUP before we enter CONNECT the first time. */
    Curl_expire_set(data, EXPIRE_CONNECTTIMEOUT,
                    data->set.connecttimeout, pnow);

  multistate(data, MSTATE_CONNECT);
  ಪ್ರತ್ಯಾಗಮ CURLM_CALL_MULTI_PERFORM;
}

ಸ್ಥಿರ CURLMcode multistate_connecting(ಸಂರಚನಾ Curl_easy *data,
                                       bool *stream_error,
                                       CURLcode *result)
{
  bool connected;

  ಯದಿ(!data->conn) {
    DEBUGASSERT(0);
    *result = CURLE_FAILED_INIT;
    ಪ್ರತ್ಯಾಗಮ CURLM_OK;
  }
  ಯದಿ(!Curl_xfer_recv_is_paused(data)) {
    *result = Curl_conn_connect(data, FIRSTSOCKET, FALSE, &connected);
    ಯದಿ(connected && !*result) {
      ಯದಿ(!data->conn->bits.reuse &&
         Curl_conn_is_multiplex(data->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_schedule_pending(data->multi);
      }
      multistate(data, MSTATE_PROTOCONNECT);
      ಪ್ರತ್ಯಾಗಮ CURLM_CALL_MULTI_PERFORM;
    }
    ಅನ್ಯಥಾ ಯದಿ(*result) {
      /* failure detected */
      CURL_TRC_M(data, "connect failed -> %d", (ಪೂರ್ಣಾಙ್ಕ)*result);
      multi_posttransfer(data);
      multi_done(data, *result, TRUE);
      *stream_error = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLM_OK;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode multistate_protoconnect(ಸಂರಚನಾ Curl_easy *data,
                                         bool *stream_error,
                                         CURLcode *result)
{
  bool protocol_connected = FALSE;

  ಯದಿ(!*result && data->conn->bits.reuse) {
    /* ftp seems to hang when protoconnect on reused connection since we
     * handle PROTOCONNECT in general inside the filters, it seems wrong to
     * restart this on a reused connection.
     */
    multistate(data, MSTATE_DO);
    ಪ್ರತ್ಯಾಗಮ CURLM_CALL_MULTI_PERFORM;
  }
  ಯದಿ(!*result)
    *result = protocol_connect(data, &protocol_connected);
  ಯದಿ(!*result && !protocol_connected) {
    /* switch to waiting state */
    multistate(data, MSTATE_PROTOCONNECTING);
    ಪ್ರತ್ಯಾಗಮ CURLM_CALL_MULTI_PERFORM;
  }
  ಅನ್ಯಥಾ ಯದಿ(!*result) {
    /* protocol connect has completed, go WAITDO or DO */
    multistate(data, MSTATE_DO);
    ಪ್ರತ್ಯಾಗಮ CURLM_CALL_MULTI_PERFORM;
  }

  /* failure detected */
  multi_posttransfer(data);
  multi_done(data, *result, TRUE);
  *stream_error = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode multistate_protoconnecting(ಸಂರಚನಾ Curl_easy *data,
                                            bool *stream_error,
                                            CURLcode *result)
{
  bool protocol_connected = FALSE;

  /* protocol-specific connect phase */
  *result = protocol_connecting(data, &protocol_connected);
  ಯದಿ(!*result && protocol_connected) {
    /* after the connect has completed, go WAITDO or DO */
    multistate(data, MSTATE_DO);
    ಪ್ರತ್ಯಾಗಮ CURLM_CALL_MULTI_PERFORM;
  }
  ಅನ್ಯಥಾ ಯದಿ(*result) {
    /* failure detected */
    multi_posttransfer(data);
    multi_done(data, *result, TRUE);
    *stream_error = TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode multistate_doing(ಸಂರಚನಾ Curl_easy *data,
                                  bool *stream_error,
                                  CURLcode *result)
{
  bool dophase_done = FALSE;

  /* we continue DOING until the DO phase is complete */
  DEBUGASSERT(data->conn);
  *result = protocol_doing(data, &dophase_done);
  ಯದಿ(!*result) {
    ಯದಿ(dophase_done) {
      /* after DO, go DO_DONE or DO_MORE */
      multistate(data, data->conn->bits.do_more ?
                 MSTATE_DOING_MORE : MSTATE_DID);
      ಪ್ರತ್ಯಾಗಮ CURLM_CALL_MULTI_PERFORM;
    } /* dophase_done */
  }
  ಅನ್ಯಥಾ {
    /* failure detected */
    multi_posttransfer(data);
    multi_done(data, *result, FALSE);
    *stream_error = TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode multistate_doing_more(ಸಂರಚನಾ Curl_easy *data,
                                       bool *stream_error,
                                       CURLcode *result)
{
  domore control;

  /*
   * When we are connected, DOING MORE and then go DID
   */
  DEBUGASSERT(data->conn);
  *result = multi_do_more(data, &control);

  ಯದಿ(!*result) {
    ಯದಿ(control != DOMORE_INCOMPLETE) {
      /* if DONE, advance to DO_DONE
         if GOBACK, go back to DOING */
      multistate(data, control == DOMORE_DONE ? MSTATE_DID : MSTATE_DOING);
      ಪ್ರತ್ಯಾಗಮ CURLM_CALL_MULTI_PERFORM;
    }
    /* else
       stay in DO_MORE */
  }
  ಅನ್ಯಥಾ {
    /* failure detected */
    multi_posttransfer(data);
    multi_done(data, *result, FALSE);
    *stream_error = TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode multistate_did(ಸಂರಚನಾ Curl_multi *multi,
                                ಸಂರಚನಾ Curl_easy *data)
{
  DEBUGASSERT(data->conn);
  ಯದಿ(data->conn->bits.multiplex)
    /* Check if we can move pending requests to send pipe */
    multi_schedule_pending(multi); /* multiplexed */

  /* Only perform the transfer if there is a good socket to work with.
     Having both BAD is a signal to skip immediately to DONE */
  ಯದಿ(CONN_SOCK_IDX_VALID(data->conn->recv_idx) ||
     CONN_SOCK_IDX_VALID(data->conn->send_idx)) {
    multistate(data, MSTATE_PERFORMING);
    /* Do not return CURLM_CALL_MULTI_PERFORM to give other transfers
     * a chance to send off their requests.
     * Note: Some SFTP handlers do not seem to like this.
     *       Restrict it to HTTP families. */
    ಪ್ರತ್ಯಾಗಮ ((multi->xfers_alive > 1) &&
            (data->conn->scheme->protocol & PROTO_FAMILY_HTTP)) ?
           CURLM_OK : CURLM_CALL_MULTI_PERFORM;
  }
  ಅನ್ಯಥಾ {
#ifndef CURL_DISABLE_FTP
    ಯದಿ(data->state.wildcardmatch &&
       ((data->conn->scheme->flags & PROTOPT_WILDCARD) == 0)) {
      data->wildcard->state = CURLWC_DONE;
    }
#endif
    multistate(data, MSTATE_DONE);
    ಪ್ರತ್ಯಾಗಮ CURLM_CALL_MULTI_PERFORM;
  }
}

ಸ್ಥಿರ CURLMcode multistate_done(ಸಂರಚನಾ Curl_easy *data, CURLcode *presult)
{
  ಯದಿ(data->conn) {
    CURLcode result;

    /* post-transfer command */
    result = multi_done(data, *presult, FALSE);

    /* allow a previously set error code take precedence */
    ಯದಿ(!(*presult))
      *presult = result;
  }

#ifndef CURL_DISABLE_FTP
  ಯದಿ(data->state.wildcardmatch) {
    ಯದಿ(data->wildcard->state != CURLWC_DONE) {
      /* if a wildcard is set and we are not ending -> lets start again
         with MSTATE_INIT */
      multistate(data, MSTATE_INIT);
      ಪ್ರತ್ಯಾಗಮ CURLM_CALL_MULTI_PERFORM;
    }
  }
#endif
  /* after we have DONE what we are supposed to do, go COMPLETED, and
     it does not matter what the multi_done() returned! */
  multistate(data, MSTATE_COMPLETED);
  ಪ್ರತ್ಯಾಗಮ CURLM_CALL_MULTI_PERFORM;
}

ಸ್ಥಿರ CURLMcode multi_runsingle(ಸಂರಚನಾ Curl_multi *multi,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ Curl_sigpipe_ctx *sigpipe_ctx)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow = ರಿಕ್ತ;

  ಯದಿ(multi->dead) {
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

  ಯದಿ(data == multi->admin) {
#ifdef ENABLE_INTERNAL_WAKEUP
    /* Consume any pending wakeup signals before processing.
     * This is necessary for event based processing. See #21547 */
    (ಶೂನ್ಯ)Curl_wakeup_consume(multi->wakeup_internal, TRUE);
#endif
#ifdef USE_RESOLV_THREADED
    Curl_async_thrdd_multi_process(multi);
#endif
    Curl_cshutdn_perform(&multi->cshutdn, multi->admin, sigpipe_ctx);
    ಗಚ್ಛ out;
  }

  sigpipe_apply(data, sigpipe_ctx);
  ಕುರು {
    /* A "stream" here is a logical stream if the protocol can handle that
       (HTTP/2), or the full connection for older protocols */
    bool stream_error = FALSE;
    mresult = CURLM_OK;
    pnow = ರಿಕ್ತ;

    ಯದಿ(multi_ischanged(multi, TRUE)) {
      CURL_TRC_M(data, "multi changed, check CONNECT_PEND queue");
      multi_schedule_pending(multi); /* multiplexed */
    }

    ಯದಿ(data->mstate > MSTATE_CONNECT &&
       data->mstate < MSTATE_COMPLETED) {
      /* Make sure we set the connection's current owner */
      DEBUGASSERT(data->conn);
      ಯದಿ(!data->conn) {
        mresult = CURLM_INTERNAL_ERROR;
        ಗಚ್ಛ out;
      }
    }

    /* Wait for the connect state as only then is the start time stored, but
       we must not check already completed handles */
    ಯದಿ((data->mstate >= MSTATE_CONNECT) && (data->mstate < MSTATE_COMPLETED)) {
      pnow = Curl_pgrs_now(data);
      ಯದಿ(multi_handle_timeout(data, pnow, &stream_error, &result))
        /* Skip the statemachine and go directly to error handling section. */
        ಗಚ್ಛ statemachine_end;
      pnow = ರಿಕ್ತ;
    }

    ವಿಕಲ್ಪ(data->mstate) {
    ಪಕ್ಷ MSTATE_INIT:
      /* Transitional state. init this transfer. A handle never comes back to
         this state. */
      mresult = multistate_init(data, &result);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_SETUP:
      /* Transitional state. Setup things for a new transfer. The handle
         can come back to this state on a redirect. */
      mresult = multistate_setup(data);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_CONNECT:
      mresult = multistate_connect(multi, data, &result);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_CONNECTING:
      /* awaiting a completion of an asynch TCP connect */
      mresult = multistate_connecting(data, &stream_error, &result);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_PROTOCONNECT:
      mresult = multistate_protoconnect(data, &stream_error, &result);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_PROTOCONNECTING:
      /* protocol-specific connect phase */
      mresult = multistate_protoconnecting(data, &stream_error, &result);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_DO:
      mresult = multistate_do(data, &stream_error, &result);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_DOING:
      /* we continue DOING until the DO phase is complete */
      mresult = multistate_doing(data, &stream_error, &result);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_DOING_MORE:
      /*
       * When we are connected, DOING MORE and then go DID
       */
      mresult = multistate_doing_more(data, &stream_error, &result);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_DID:
      mresult = multistate_did(multi, data);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_RATELIMITING: /* limit-rate exceeded in either direction */
      mresult = multistate_ratelimiting(data, &result);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_PERFORMING:
      mresult = multistate_performing(data, &stream_error, &result);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_DONE:
      mresult = multistate_done(data, &result);
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_COMPLETED:
      ಭಙ್ಗ;

    ಪಕ್ಷ MSTATE_PENDING:
    ಪಕ್ಷ MSTATE_MSGSENT:
      /* handles in these states should NOT be in this list */
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      mresult = CURLM_INTERNAL_ERROR;
      ಗಚ್ಛ out;
    }

    ಯದಿ(data->mstate >= MSTATE_CONNECT &&
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
    ಯದಿ(!pnow)
      pnow = Curl_pgrs_now(data);
    result = is_finished(multi, data, pnow, stream_error, result);
    ಯದಿ(result)
      mresult = CURLM_CALL_MULTI_PERFORM;

    ಯದಿ(MSTATE_COMPLETED == data->mstate) {
      handle_completed(multi, data, result);
      mresult = CURLM_OK;
      ಗಚ್ಛ out;
    }
  } ಯಾವತ್((mresult == CURLM_CALL_MULTI_PERFORM) ||
          multi_ischanged(multi, FALSE));

out:
  data->result = result;
  ಪ್ರತ್ಯಾಗಮ mresult;
}

ಸ್ಥಿರ CURLMcode multi_perform(ಸಂರಚನಾ Curl_multi *multi,
                               ಪೂರ್ಣಾಙ್ಕ *running_handles)
{
  CURLMcode returncode = CURLM_OK;
  ಸಂರಚನಾ curltime start;
  uint32_t mid;
  ಸಂರಚನಾ Curl_sigpipe_ctx sigpipe_ctx;

  curlx_pnow(&start);
  sigpipe_init(&sigpipe_ctx);

  ಯದಿ(Curl_uint32_bset_first(&multi->process, &mid)) {
    CURL_TRC_M(multi->admin, "multi_perform(running=%u)",
               multi_xfers_running(multi));
    ಕುರು {
      ಸಂರಚನಾ Curl_easy *data = Curl_multi_get_easy(multi, mid);
      CURLMcode mresult;
      ಯದಿ(!data) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        ಅನುವರ್ತ;
      }
      mresult = multi_runsingle(multi, data, &sigpipe_ctx);
      ಯದಿ(mresult)
        returncode = mresult;
    } ಯಾವತ್(Curl_uint32_bset_next(&multi->process, mid, &mid));
  }
  sigpipe_restore(&sigpipe_ctx);

  ಯದಿ(multi_ischanged(multi, TRUE))
    multi_schedule_pending(multi);

  ಯದಿ(!returncode && CURL_MNTFY_HAS_ENTRIES(multi))
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
  ಯಾವತ್(Curl_timeouts_remove_expired(&multi->timeouts, &start, &mid)) {
    /* the removed may have another timeout in queue */
    ಸಂರಚನಾ Curl_easy *data = Curl_multi_get_easy(multi, mid);
    ಯದಿ(!data) {
      DEBUGASSERT(0);
      ಅನುವರ್ತ;
    }
    (ಶೂನ್ಯ)add_next_timeout(&start, multi, data);
    ಯದಿ(data->mstate == MSTATE_PENDING) {
      bool stream_unused;
      CURLcode result_unused;
      ಯದಿ(multi_handle_timeout(data, Curl_pgrs_now(data),
                              &stream_unused, &result_unused)) {
        infof(data, "PENDING handle timeout");
        move_pending_to_connect(multi, data);
      }
    }
  }

  ಯದಿ(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < INT_MAX) ? (ಪೂರ್ಣಾಙ್ಕ)running : INT_MAX;
  }

  ಯದಿ(CURLM_OK >= returncode)
    returncode = Curl_update_timer(multi);

  ಪ್ರತ್ಯಾಗಮ returncode;
}

CURLMcode curl_multi_perform(CURLM *m, ಪೂರ್ಣಾಙ್ಕ *running_handles)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_perform, &mresult)) {
    mresult = multi_perform(m, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

CURLMcode curl_multi_cleanup(CURLM *m)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_cleanup, &mresult)) {
    ಸಂರಚನಾ Curl_multi *multi = m;
    ಶೂನ್ಯ *entry;
    uint32_t mid;

    /* First remove all remaining easy handles,
     * close internal ones. admin handle is special */
    ಯದಿ(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
      ಕುರು {
        ಸಂರಚನಾ Curl_easy *data = entry;
        ಯದಿ(!GOOD_EASY_HANDLE(data)) {
          mresult = CURLM_BAD_HANDLE;
          ಗಚ್ಛ out;
        }

#ifdef DEBUGBUILD
        ಯದಿ(mid != data->mid) {
          CURL_TRC_M(data, "multi_cleanup: still present with mid=%u, "
                     "but unexpected data->mid=%u\n", mid, data->mid);
          DEBUGASSERT(0);
        }
#endif

        ಯದಿ(data == multi->admin)
          ಅನುವರ್ತ;

        ಯದಿ(!data->state.done && data->conn)
          /* if DONE was never called for this handle */
          (ಶೂನ್ಯ)multi_done(data, CURLE_OK, TRUE);

        data->multi = ರಿಕ್ತ; /* clear the association */
        Curl_uint32_tbl_remove(&multi->xfers, mid);
        data->mid = UINT32_MAX;

#ifdef USE_LIBPSL
        ಯದಿ(data->psl == &multi->psl)
          data->psl = ರಿಕ್ತ;
#endif
        ಯದಿ(data->state.internal)
          Curl_close(&data);
      } ಯಾವತ್(Curl_uint32_tbl_next(&multi->xfers, mid, &mid, &entry));
    }

#ifdef USE_RESOLV_THREADED
    Curl_async_thrdd_multi_destroy(multi, !multi->quick_exit);
#endif
    Curl_cpool_destroy(&multi->cpool, multi->admin);
    Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
    ಯದಿ(multi->admin) {
      CURL_TRC_M(multi->admin, "multi_cleanup, closing admin handle, done");
      multi->admin->multi = ರಿಕ್ತ;
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
    ಯದಿ(Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers)) {
      multi_xfer_tbl_dump(multi);
      DEBUGASSERT(0);
    }
#endif
    Curl_uint32_bset_destroy(&multi->process);
    Curl_uint32_bset_destroy(&multi->dirty);
    Curl_uint32_bset_destroy(&multi->pending);
    Curl_uint32_bset_destroy(&multi->msgsent);
    Curl_uint32_tbl_destroy(&multi->xfers);
    curlx_memzero(multi, ಪರಿಮಾಣ(*multi));
    curlx_free(multi);

    mresult = CURLM_OK;
  }
out:
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

/*
 * curl_multi_info_read()
 *
 * This function is the primary way for a multi/multi_socket application to
 * figure out if a transfer has ended.
 */

CURLMsg *curl_multi_info_read(CURLM *m, ಪೂರ್ಣಾಙ್ಕ *msgs_in_queue)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMsg *ಸಂದೇಶ_result = ರಿಕ್ತ;

  *msgs_in_queue = 0; /* default to none */
  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_info_read, ರಿಕ್ತ)) {
    ಸಂರಚನಾ Curl_multi *multi = m;
    uint32_t mid;
    ಯದಿ(Curl_uint32_bset_first(&multi->msgsent, &mid)) {
      ಸಂರಚನಾ Curl_easy *data = Curl_multi_get_easy(multi, mid);

      DEBUGASSERT(data);
      Curl_uint32_bset_remove(&multi->msgsent, mid);
      *msgs_in_queue =
        curlx_uztosi(Curl_uint32_bset_ಎಣಿಕೆ(&multi->msgsent));
      ಯದಿ(data)
        ಸಂದೇಶ_result = &data->ಸಂದೇಶ;
    }
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ_result;
}

ಶೂನ್ಯ Curl_multi_will_close(ಸಂರಚನಾ Curl_easy *data, curl_socket_t s)
{
  ಯದಿ(data) {
    ಸಂರಚನಾ Curl_multi *multi = data->multi;
    ಯದಿ(multi) {
      CURL_TRC_M(data, "Curl_multi_will_close fd=%" FMT_SOCKET_T, s);
      Curl_multi_ev_socket_done(multi, data, s);
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ multi_timeouts_init(ಸಂರಚನಾ Curl_easy *data)
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
ಸ್ಥಿರ CURLMcode add_next_timeout(ಧ್ರುವ ಸಂರಚನಾ curltime *pnow,
                                  ಸಂರಚನಾ Curl_multi *multi,
                                  ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ expire_timers *timeouts = &data->state.timeouts;
  timediff_t now_us = Curl_timeouts_offset_us(&multi->timeouts, pnow);

  ಯಾವತ್(timeouts->first < EXPIRE_LAST) {
    ಯದಿ(timeouts->offset_us[timeouts->first] <= now_us)  /* already expired */
      timeouts->first = timeouts->next[timeouts->first];
    ಅನ್ಯಥಾ /* timeouts are sorted, first is first in the future now */
      ಭಙ್ಗ;
  }

  ಯದಿ(timeouts->first < EXPIRE_LAST) {
    /* Insert this node again into the splay. Keep the timer in the list in
       case we need to recompute future timers. */
    Curl_timeouts_add(&multi->timeouts, data,
                      timeouts->offset_us[timeouts->first]);
  }
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ multi_mark_expired_as_dirty(ಸಂರಚನಾ Curl_multi *multi,
                                        ಧ್ರುವ ಸಂರಚನಾ curltime *ts)
{
  ಸಂರಚನಾ Curl_easy *data = ರಿಕ್ತ;
  uint32_t mid;

  /*
   * The loop following here will go on as long as there are expire-times left
   * to process (compared to `ts`) in the splay and 'data' will be
   * re-assigned for every expired handle we deal with.
   */
  ಯಾವತ್(Curl_timeouts_remove_expired(&multi->timeouts, ts, &mid)) {
    /* Check if there is one (more) expired timer to deal with! This function
       extracts a matching node if there is one */
    data = Curl_multi_get_easy(multi, mid);
    ಯದಿ(!data) {
      DEBUGASSERT(0);
      ಅನುವರ್ತ;
    }
#ifdef CURLVERBOSE
    ಯದಿ(CURL_TRC_TIMER_is_verbose(data)) {
      ಯದಿ(data->state.timeouts.first < EXPIRE_LAST) {
        CURL_TRC_TIMER(data, data->state.timeouts.first, "has expired");
      }
    }
#endif
    (ಶೂನ್ಯ)add_next_timeout(ts, multi, data);
    Curl_multi_mark_dirty(data);
  }
}

ಸ್ಥಿರ CURLMcode multi_run_dirty(ಸಂರಚನಾ Curl_multi *multi,
                                 ಸಂರಚನಾ Curl_sigpipe_ctx *sigpipe_ctx,
                                 uint32_t *pnum)
{
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;

  *pnum = 0;
  ಯದಿ(Curl_uint32_bset_first(&multi->dirty, &mid)) {
    ಕುರು {
      ಸಂರಚನಾ Curl_easy *data = Curl_multi_get_easy(multi, mid);
      ಯದಿ(data) {
        CURL_TRC_M(data, "multi_run_dirty");

        ಯದಿ(!Curl_uint32_bset_contains(&multi->process, mid)) {
          ಯದಿ(Curl_uint32_bset_contains(&multi->pending, mid)) {
            /* Something happened on the pending transfer, act on it. */
            move_pending_to_connect(multi, data);
          }
          ಅನ್ಯಥಾ {
            /* We are no longer processing this transfer */
            Curl_uint32_bset_remove(&multi->dirty, mid);
            ಅನುವರ್ತ;
          }
        }

        (*pnum)++;
        /* runsingle() clears the dirty mid */
        mresult = multi_runsingle(multi, data, sigpipe_ctx);

        ಯದಿ(CURLM_OK >= mresult) {
          /* reassess event handling of data */
          mresult = Curl_multi_ev_assess_xfer(multi, data);
          ಯದಿ(mresult)
            ಗಚ್ಛ out;
        }
      }
      ಅನ್ಯಥಾ {
        CURL_TRC_M(multi->admin, "multi_run_dirty, %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } ಯಾವತ್(Curl_uint32_bset_next(&multi->dirty, mid, &mid));
  }

out:
  ಪ್ರತ್ಯಾಗಮ mresult;
}

ಸ್ಥಿರ CURLMcode multi_socket(ಸಂರಚನಾ Curl_multi *multi,
                              bool checkall,
                              curl_socket_t s,
                              ಪೂರ್ಣಾಙ್ಕ ev_bitmask,
                              ಪೂರ್ಣಾಙ್ಕ *running_handles)
{
  CURLMcode mresult = CURLM_OK;
  ಸಂರಚನಾ Curl_sigpipe_ctx pipe_ctx;
  ಸಂರಚನಾ curltime now;
  uint32_t run_xfers;

  (ಶೂನ್ಯ)ev_bitmask;
  sigpipe_init(&pipe_ctx);

  ಯದಿ(checkall) {
    /* *perform() deals with running_handles on its own */
    mresult = multi_perform(multi, running_handles);

    ಯದಿ(mresult != CURLM_BAD_HANDLE) {
      /* Reassess event status of all active transfers, but keep any error
         from multi_perform() */
      CURLMcode mresult2 =
        Curl_multi_ev_assess_xfer_bset(multi, &multi->process);
      ಯದಿ(!mresult)
        mresult = mresult2;
    }
    ಗಚ್ಛ out;
  }

  ಯದಿ(s != CURL_SOCKET_TIMEOUT) {
    /* Mark all transfers of that socket as dirty */
    Curl_multi_ev_dirty_xfers(multi, s);
  }
  ಅನ್ಯಥಾ {
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
    ಯದಿ(mresult)
      ಗಚ್ಛ out;
  }

  curlx_pnow(&now);
  multi_mark_expired_as_dirty(multi, &now);
  mresult = multi_run_dirty(multi, &pipe_ctx, &run_xfers);
  ಯದಿ(mresult)
    ಗಚ್ಛ out;

  ಯದಿ(run_xfers) {
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

  ಯದಿ(multi_ischanged(multi, TRUE))
    multi_schedule_pending(multi);

  ಯದಿ(!mresult && CURL_MNTFY_HAS_ENTRIES(multi))
    mresult = Curl_mntfy_dispatch_all(multi);

  ಯದಿ(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < INT_MAX) ? (ಪೂರ್ಣಾಙ್ಕ)running : INT_MAX;
  }

  ಯದಿ(CURLM_OK >= mresult)
    mresult = Curl_update_timer(multi);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

#undef curl_multi_setopt
CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_setopt, &mresult)) {
    ಸಂರಚನಾ Curl_multi *multi = m;
    va_list param;
    unsigned long uarg;
    size_t szarg;

    va_start(param, option);

    ವಿಕಲ್ಪ(option) {
    ಪಕ್ಷ CURLMOPT_SOCKETFUNCTION:
      multi->socket_cb = va_arg(param, curl_socket_callback);
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_SOCKETDATA:
      multi->socket_userp = va_arg(param, ಶೂನ್ಯ *);
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_PUSHFUNCTION:
      multi->ತಳ್ಳು_cb = va_arg(param, curl_ತಳ್ಳು_callback);
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_PUSHDATA:
      multi->ತಳ್ಳು_userp = va_arg(param, ಶೂನ್ಯ *);
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_PIPELINING:
      multi->multiplexing = va_arg(param, long) & CURLPIPE_MULTIPLEX ? 1 : 0;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_TIMERFUNCTION:
      multi->timer_cb = va_arg(param, curl_multi_timer_callback);
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_TIMERDATA:
      multi->timer_userp = va_arg(param, ಶೂನ್ಯ *);
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_MAXCONNECTS:
      uarg = va_arg(param, unsigned long);
      ಯದಿ(uarg <= UINT32_MAX)
        multi->maxconnects = (uint32_t)uarg;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_MAX_HOST_CONNECTIONS:
      ಯದಿ(!curlx_sltouz(va_arg(param, long), &szarg))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      multi->max_host_connections = (szarg < UINT32_MAX) ?
                                    (uint32_t)szarg : UINT32_MAX;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_MAX_TOTAL_CONNECTIONS:
      ಯದಿ(!curlx_sltouz(va_arg(param, long), &szarg))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      multi->max_total_connections = (szarg < UINT32_MAX) ?
                                     (uint32_t)szarg : UINT32_MAX;
      ಭಙ್ಗ;
      /* options formerly used for pipelining */
    ಪಕ್ಷ CURLMOPT_MAX_PIPELINE_LENGTH:
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_CONTENT_LENGTH_PENALTY_SIZE:
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_CHUNK_LENGTH_PENALTY_SIZE:
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_PIPELINING_SITE_BL:
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_PIPELINING_SERVER_BL:
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_MAX_CONCURRENT_STREAMS: {
      ಯದಿ(!curlx_sltouz(va_arg(param, long), &szarg) ||
         !szarg || (szarg > (size_t)INT_MAX)) /* preserve previous cutoff */
        multi->max_concurrent_streams = 100;
      ಅನ್ಯಥಾ
        multi->max_concurrent_streams = (szarg < UINT32_MAX) ?
                                       (uint32_t)szarg : UINT32_MAX;
      ಭಙ್ಗ;
    }
    ಪಕ್ಷ CURLMOPT_NETWORK_CHANGED: {
      long val = va_arg(param, long);
      ಯದಿ(val & CURLMNWC_CLEAR_ALL)
        /* In the beginning, all values available to set were 1 by mistake. We
           converted this to mean "all", thus setting all the bits
           automatically */
        val = CURLMNWC_CLEAR_DNS | CURLMNWC_CLEAR_CONNS;
      ಯದಿ(val & CURLMNWC_CLEAR_DNS) {
        Curl_dnscache_clear(multi->admin);
      }
      ಯದಿ(val & CURLMNWC_CLEAR_CONNS) {
        Curl_cpool_nw_changed(&multi->cpool, multi->admin);
      }
      ಭಙ್ಗ;
    }
    ಪಕ್ಷ CURLMOPT_NOTIFYFUNCTION:
      multi->ntfy.ntfy_cb = va_arg(param, curl_notify_callback);
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_NOTIFYDATA:
      multi->ntfy.ntfy_cb_data = va_arg(param, ಶೂನ್ಯ *);
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_RESOLVE_THREADS_MAX:
#ifdef USE_RESOLV_THREADED
      uarg = va_arg(param, long);
      ಯದಿ((uarg <= 0) || (uarg > UINT32_MAX))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      ಅನ್ಯಥಾ {
        CURLcode result = Curl_async_thrdd_multi_set_props(
          multi, 0, (uint32_t)uarg, 2000);
        ವಿಕಲ್ಪ(result) {
        ಪಕ್ಷ CURLE_OK:
          mresult = CURLM_OK;
          ಭಙ್ಗ;
        ಪಕ್ಷ CURLE_BAD_FUNCTION_ARGUMENT:
          mresult = CURLM_BAD_FUNCTION_ARGUMENT;
          ಭಙ್ಗ;
        ಪಕ್ಷ CURLE_OUT_OF_MEMORY:
          mresult = CURLM_OUT_OF_MEMORY;
          ಭಙ್ಗ;
        ಸ್ವಭಾವ:
          mresult = CURLM_INTERNAL_ERROR;
          ಭಙ್ಗ;
        }
      }
#endif
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMOPT_QUICK_EXIT:
      multi->quick_exit = va_arg(param, long) ? 1 : 0;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      mresult = CURLM_UNKNOWN_OPTION;
      ಭಙ್ಗ;
    }
    va_end(param);
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

/* we define curl_multi_socket() in the public multi.h header */
#undef curl_multi_socket

CURLMcode curl_multi_socket(CURLM *m, curl_socket_t s, ಪೂರ್ಣಾಙ್ಕ *running_handles)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_socket, &mresult)) {
    mresult = multi_socket(m, FALSE, s, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

CURLMcode curl_multi_socket_action(CURLM *m, curl_socket_t s,
                                   ಪೂರ್ಣಾಙ್ಕ ev_bitmask, ಪೂರ್ಣಾಙ್ಕ *running_handles)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_socket_action, &mresult)) {
    mresult = multi_socket(m, FALSE, s, ev_bitmask, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

CURLMcode curl_multi_socket_all(CURLM *m, ಪೂರ್ಣಾಙ್ಕ *running_handles)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_socket_all, &mresult)) {
    mresult = multi_socket(m, TRUE, CURL_SOCKET_BAD, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

ಸ್ಥಿರ bool multi_has_dirties(ಸಂರಚನಾ Curl_multi *multi)
{
  uint32_t mid;
  ಯದಿ(Curl_uint32_bset_first(&multi->dirty, &mid)) {
    ಕುರು {
      ಸಂರಚನಾ Curl_easy *data = Curl_multi_get_easy(multi, mid);
      ಯದಿ(data) {
        ಯದಿ(Curl_uint32_bset_contains(&multi->process, mid))
          ಪ್ರತ್ಯಾಗಮ TRUE;
        /* We are no longer processing this transfer */
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
      ಅನ್ಯಥಾ {
        CURL_TRC_M(multi->admin, "dirty transfer %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } ಯಾವತ್(Curl_uint32_bset_next(&multi->dirty, mid, &mid));
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ multi_timeout(ಸಂರಚನಾ Curl_multi *multi,
                          timediff_t *pexire_offset_us,
                          ಪೂರ್ಣಾಙ್ಕ *timeout_ms)
{
  ಯದಿ(multi->dead) {
    ಯದಿ(pexire_offset_us)
      *pexire_offset_us = 0;
    *timeout_ms = 0;
    ಪ್ರತ್ಯಾಗಮ;
  }

  ಯದಿ(multi_has_dirties(multi)) {
    ಯದಿ(pexire_offset_us) {
      ಸಂರಚನಾ curltime now;
      curlx_pnow(&now);
      *pexire_offset_us = Curl_timeouts_offset_us(&multi->timeouts, &now);
    }
    *timeout_ms = 0;
    ಪ್ರತ್ಯಾಗಮ;
  }
  ಅನ್ಯಥಾ {
    ಸಂರಚನಾ curltime now;
    uint32_t mid;

    curlx_pnow(&now);
    *timeout_ms = Curl_timeouts_next_ms(&multi->timeouts, &now,
                                        pexire_offset_us, &mid);
#ifdef CURLVERBOSE
    ಯದಿ(mid != UINT32_MAX) {
      ಸಂರಚನಾ Curl_easy *data = Curl_multi_get_easy(multi, mid);
      ಯದಿ(data && CURL_TRC_TIMER_is_verbose(data) &&
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
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_timeout, &mresult)) {
    ಪೂರ್ಣಾಙ್ಕ itimeout_ms;

    multi_timeout(m, ರಿಕ್ತ, &itimeout_ms);
    *timeout_ms = (long)itimeout_ms;
    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

/*
 * Tell the application it should update its timers, if it subscribes to the
 * update timer callback.
 */
CURLMcode Curl_update_timer(ಸಂರಚನಾ Curl_multi *multi)
{
  timediff_t timeouts_offset_us = 0;
  ಪೂರ್ಣಾಙ್ಕ timeout_ms;
  ಪೂರ್ಣಾಙ್ಕ rc;
  bool set_ಮೌಲ್ಯ = FALSE;

  ಯದಿ(!multi->timer_cb || multi->dead)
    ಪ್ರತ್ಯಾಗಮ CURLM_OK;
  multi_timeout(multi, &timeouts_offset_us, &timeout_ms);

  ಯದಿ(timeout_ms < 0 && !multi->last_timeout_set) {
    /* nothing to do */
  }
  ಅನ್ಯಥಾ ಯದಿ(timeout_ms < 0) {
    /* there is no timeout now but there was one previously */
    CURL_TRC_M(multi->admin, "[TIMER] clear");
    timeout_ms = -1; /* normalize */
    set_ಮೌಲ್ಯ = TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(!multi->last_timeout_set) {
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, none before", timeout_ms);
    set_ಮೌಲ್ಯ = TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(multi->last_expire_offset_us != timeouts_offset_us) {
    /* We had a timeout before and have one now, the absolute timestamp
     * differs. The relative timeout_ms may be the same, but the starting
     * point differs. Let the application restart its timer. */
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, replace previous",
               timeout_ms);
    set_ಮೌಲ್ಯ = TRUE;
  }
  ಅನ್ಯಥಾ {
    /* We have same expire time as previously. Our relative 'timeout_ms'
     * may be different now, but the application has the timer running
     * and we do not to tell it to start this again. */
  }

  ಯದಿ(set_ಮೌಲ್ಯ) {
    ಸಂರಚನಾ Curl_mapi_guard guard;

    multi->last_expire_offset_us = timeouts_offset_us;
    multi->last_timeout_set = timeout_ms >= 0;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_timer_cb);
    rc = multi->timer_cb(multi, timeout_ms, multi->timer_userp);
    CURL_CBAPI_MULTI_END(&guard);
    ಯದಿ(rc == -1) {
      multi->dead = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLM_ABORTED_BY_CALLBACK;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
}

#ifdef DEBUGBUILD
ಸ್ಥಿರ bool multi_timeouts_check(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ expire_timers *timeouts = &data->state.timeouts;
  uint8_t id;
  ಪೂರ್ಣಾಙ್ಕ i = 0;
  ಪ್ರತಿ(id = timeouts->first; id < EXPIRE_LAST; id = timeouts->next[id]) {
    ಯದಿ(++i >= EXPIRE_LAST) {
      failf(data, "expire timeouts looped: %d iterations and no end", i);
      ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಯದಿ(id == timeouts->next[id]) {
      failf(data, "expire timeouts wrong: %d points to itself", (ಪೂರ್ಣಾಙ್ಕ)id);
      ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಯದಿ((timeouts->next[id] < EXPIRE_LAST) &&
       (timeouts->offset_us[id] > timeouts->offset_us[timeouts->next[id]])) {
      failf(data, "expire timeouts not sorted: %d happens after %d but "
            "is listed before", (ಪೂರ್ಣಾಙ್ಕ)id, (ಪೂರ್ಣಾಙ್ಕ)timeouts->next[id]);
      ಪ್ರತ್ಯಾಗಮ FALSE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}
#endif

/*
 * Remove a given timestamp from the list of timeouts.
 */
ಸ್ಥಿರ ಶೂನ್ಯ multi_clear_timeout(ಸಂರಚನಾ Curl_easy *data, expire_id eid)
{
  ಸಂರಚನಾ expire_timers *timeouts = &data->state.timeouts;
  uint8_t orig_first = timeouts->first;
  uint8_t *anchor = &timeouts->first;
  uint8_t id = (uint8_t)eid;

  ಯದಿ((unsigned)eid >= EXPIRE_LAST) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ;
  }

  ಯಾವತ್(*anchor < EXPIRE_LAST) {
    ಯದಿ(*anchor == id) {
      *anchor = timeouts->next[id];
      ಭಙ್ಗ;
    }
    anchor = &timeouts->next[*anchor];
  }
  DEBUGASSERT(multi_timeouts_check(data));
  ಯದಿ(Curl_timeouts_has(data)) {
    ಸಂರಚನಾ Curl_multi *multi = data->multi;

    ಯದಿ(!multi) {
      DEBUGASSERT(0);
      ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ((timeouts->first >= EXPIRE_LAST) || /* no more timeouts */
       (timeouts->first != orig_first)) {  /* active timeout changed */
      Curl_timeouts_remove(&multi->timeouts, data);
    }
    ಯದಿ((timeouts->first < EXPIRE_LAST) && !Curl_timeouts_has(data)) {
      Curl_timeouts_add(&multi->timeouts, data,
                        timeouts->offset_us[timeouts->first]);
    }
  }
}

/*
 * Add a timestamp to the list of timeouts. Keep the list sorted so that head
 * of list is always the timeout nearest in time.
 */
ಸ್ಥಿರ CURLMcode multi_set_timeout(ಸಂರಚನಾ Curl_easy *data,
                                   ಧ್ರುವ ಸಂರಚನಾ curltime *stamp,
                                   expire_id eid)
{
  ಸಂರಚನಾ expire_timers *timeouts = &data->state.timeouts;
  uint8_t *anchor = &timeouts->first;
  uint8_t id = (uint8_t)eid;

  ಯದಿ((unsigned)eid >= EXPIRE_LAST) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ CURLM_BAD_FUNCTION_ARGUMENT;
  }
  /* remove from list, store time and re-insert */
  multi_clear_timeout(data, eid);
  timeouts->offset_us[id] =
    Curl_timeouts_offset_us(&data->multi->timeouts, stamp);

  ಯಾವತ್(*anchor < EXPIRE_LAST) {
    ಯದಿ(timeouts->offset_us[*anchor] > timeouts->offset_us[id])
      ಭಙ್ಗ;
    anchor = &timeouts->next[*anchor];
  }
  timeouts->next[eid] = *anchor;
  timeouts->next[eid] = *anchor;
  *anchor = id;
  DEBUGASSERT(multi_timeouts_check(data));
  CURL_TRC_TIMER(data, eid, "set for %" FMT_TIMEDIFF_T "us",
                 curlx_ptimediff_us(stamp, Curl_pgrs_now(data)));
  ಪ್ರತ್ಯಾಗಮ CURLM_OK;
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
ಶೂನ್ಯ Curl_expire_set(ಸಂರಚನಾ Curl_easy *data,
                     expire_id eid, timediff_t ms,
                     ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಸಂರಚನಾ Curl_multi *multi = data->multi;
  ಸಂರಚನಾ expire_timers *timeouts = &data->state.timeouts;
  uint8_t prev_id = timeouts->first;
  ಸಂರಚನಾ curltime set;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  ಯದಿ(!multi)
    ಪ್ರತ್ಯಾಗಮ;
  DEBUGASSERT(eid < EXPIRE_LAST);
  ಯದಿ(ms > INT_MAX)
    /* Cap ridiculous timeouts, 31-bit ms is still 3.5 weeks. When the time
       goes to the user, it must fit in this size. */
    ms = INT_MAX;

  set = *pnow;
  set.tv_sec += (time_t)(ms / 1000); /* may be a 64 to 32-bit conversion */
  set.tv_usec += (ಪೂರ್ಣಾಙ್ಕ)(ms % 1000) * 1000;
  ಯದಿ(set.tv_usec >= 1000000) {
    set.tv_sec++;
    set.tv_usec -= 1000000;
  }

  /* Add the timeout, will replace any previous value for this timer. */
  multi_set_timeout(data, &set, eid);
  DEBUGASSERT(timeouts->first < EXPIRE_LAST);

  ಯದಿ(Curl_timeouts_has(data)) {
    /* data has already a timeout registered. If the first timer
     * was NOT the one we just set AND is still the first one,
     * nothing changed from the timeouts point of view. The
     * set timer triggers after the one already registered. Leave. */
    ಯದಿ((prev_id != eid) && (prev_id == timeouts->first))
      ಪ್ರತ್ಯಾಗಮ;

    /* Since this is an updated time, we must remove data from
     * timeouts and then add it again. */
    Curl_timeouts_remove(&multi->timeouts, data);
  }

  /* Insert the new timer expiry since it is our local minimum. */
  Curl_timeouts_add(&multi->timeouts, data,
                    timeouts->offset_us[timeouts->first]);
}

ಶೂನ್ಯ Curl_expire(ಸಂರಚನಾ Curl_easy *data,
                 timediff_t milli, expire_id eid)
{
  Curl_expire_set(data, eid, milli, Curl_pgrs_now(data));
}

/*
 * Removes the expire timer. Marks it as done.
 */
ಶೂನ್ಯ Curl_expire_clear(ಸಂರಚನಾ Curl_easy *data, expire_id eid)
{
  /* remove the timer, if there */
  multi_clear_timeout(data, eid);
  CURL_TRC_TIMER(data, eid, "cleared");
}

/*
 * Clear ALL timeout values for this handle.
 */
ಶೂನ್ಯ Curl_expire_clear_all(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_multi *multi = data->multi;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  ಯದಿ(!multi)
    ಪ್ರತ್ಯಾಗಮ;

  ಯದಿ(Curl_timeouts_remove(&multi->timeouts, data)) {
    /* Since this is an cleared time, we must remove the previous entry from
       the splay tree */
    multi_timeouts_init(data);

    ಯದಿ(data->id >= 0)
      CURL_TRC_M(data, "[TIMEOUT] all cleared");
  }
}

CURLMcode curl_multi_assign(CURLM *m, curl_socket_t sockfd,
                            ಶೂನ್ಯ *sockp)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_assign, &mresult)) {
    mresult = Curl_multi_ev_assign(m, sockfd, sockp);
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

ಸ್ಥಿರ ಶೂನ್ಯ move_pending_to_connect(ಸಂರಚನಾ Curl_multi *multi,
                                    ಸಂರಚನಾ Curl_easy *data)
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
ಸ್ಥಿರ ಶೂನ್ಯ multi_schedule_pending(ಸಂರಚನಾ Curl_multi *multi)
{
  uint32_t mid = multi->last_pending_mid;

  ಯದಿ(mid) {
    ಯಾವತ್(Curl_uint32_bset_next(&multi->pending, mid, &mid)) {
      ಸಂರಚನಾ Curl_easy *data = Curl_multi_get_easy(multi, mid);
      ಯದಿ(data) {
        move_pending_to_connect(multi, data);
        multi->last_pending_mid = mid;
        ಪ್ರತ್ಯಾಗಮ;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    }
    /* found no pending transfers with `mid` larger than `last_pending_mid`.
     * Start at the beginning of the pending set again. */
    multi->last_pending_mid = 0;
  }

  ಯದಿ(Curl_uint32_bset_first(&multi->pending, &mid)) {
    ಕುರು {
      ಸಂರಚನಾ Curl_easy *data = Curl_multi_get_easy(multi, mid);
      ಯದಿ(data) {
        move_pending_to_connect(multi, data);
        multi->last_pending_mid = mid;
        ಪ್ರತ್ಯಾಗಮ;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    } ಯಾವತ್(Curl_uint32_bset_next(&multi->pending, mid, &mid));
  }
}

uint32_t Curl_multi_max_concurrent_streams(ಸಂರಚನಾ Curl_multi *multi)
{
  DEBUGASSERT(multi);
  ಪ್ರತ್ಯಾಗಮ multi->max_concurrent_streams;
}

CURL **curl_multi_get_handles(CURLM *m)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURL **a = ರಿಕ್ತ;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_get_handles, ರಿಕ್ತ)) {
    ಸಂರಚನಾ Curl_multi *multi = m;
    ಶೂನ್ಯ *entry;
    size_t ಎಣಿಕೆ = Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers);

    a = curlx_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_easy *) * (ಎಣಿಕೆ + 1));
    ಯದಿ(a) {
      unsigned ಪೂರ್ಣಾಙ್ಕ i = 0;
      uint32_t mid;

      ಯದಿ(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
        ಕುರು {
          ಸಂರಚನಾ Curl_easy *data = entry;
          DEBUGASSERT(i < ಎಣಿಕೆ);
          ಯದಿ(!data->state.internal)
            a[i++] = data;
        } ಯಾವತ್(Curl_uint32_tbl_next(&multi->xfers, mid, &mid, &entry));
      }
      a[i] = ರಿಕ್ತ; /* last entry is a NULL */
    }
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ a;
}

CURLMcode curl_multi_get_offt(CURLM *m,
                              CURLMinfo_offt info,
                              curl_off_t *pvalue)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_get_offt, &mresult)) {
    ಸಂರಚನಾ Curl_multi *multi = m;
    uint32_t n;

    ಯದಿ(!pvalue) {
      mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      ಗಚ್ಛ out;
    }

    ವಿಕಲ್ಪ(info) {
    ಪಕ್ಷ CURLMINFO_XFERS_CURRENT:
      n = Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers);
      ಯದಿ(n && multi->admin)
        --n;
      *pvalue = (curl_off_t)n;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMINFO_XFERS_RUNNING:
      n = Curl_uint32_bset_ಎಣಿಕೆ(&multi->process);
      ಯದಿ(n && Curl_uint32_bset_contains(&multi->process, multi->admin->mid))
        --n;
      *pvalue = (curl_off_t)n;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMINFO_XFERS_PENDING:
      *pvalue = (curl_off_t)Curl_uint32_bset_ಎಣಿಕೆ(&multi->pending);
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMINFO_XFERS_DONE:
      *pvalue = (curl_off_t)Curl_uint32_bset_ಎಣಿಕೆ(&multi->msgsent);
      ಭಙ್ಗ;
    ಪಕ್ಷ CURLMINFO_XFERS_ADDED:
      *pvalue = multi->xfers_total_ever;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      *pvalue = -1;
      mresult = CURLM_UNKNOWN_OPTION;
      ಭಙ್ಗ;
    }
  }
out:
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

ಸ್ಥಿರ ಸಂರಚನಾ Curl_fixed_buf *fixed_buf_create(size_t len)
{
  ಸಂರಚನಾ Curl_fixed_buf *fbuf;
  ಯದಿ((SIZE_MAX - ಪರಿಮಾಣ(*fbuf)) < len)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* too large */
  fbuf = curlx_malloc(len + ಪರಿಮಾಣ(*fbuf));
  ಯದಿ(fbuf)
    fbuf->len = len;
  ಪ್ರತ್ಯಾಗಮ fbuf;
}

CURLcode Curl_multi_xfer_buf_borrow(ಸಂರಚನಾ Curl_easy *data,
                                    ವರ್ಣ **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  *pbuf = ರಿಕ್ತ;
  *pbuflen = 0;
  ಯದಿ(!data->multi) {
    failf(data, "transfer has no multi handle");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  ಯದಿ(!data->set.buffer_size) {
    failf(data, "transfer buffer size is 0");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  ಯದಿ(data->multi->xfer_buf_borrowed) {
    failf(data, "attempt to borrow xfer_buf when already borrowed");
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  }

  ಯದಿ(data->multi->xfer_buf &&
     data->set.buffer_size > data->multi->xfer_buf->len) {
    /* not large enough, get a new one */
    curlx_safefree(data->multi->xfer_buf);
  }

  ಯದಿ(!data->multi->xfer_buf) {
    data->multi->xfer_buf =
      fixed_buf_create(curlx_uitouz(data->set.buffer_size));
    ಯದಿ(!data->multi->xfer_buf) {
      failf(data, "could not allocate xfer_buf of %u bytes",
            data->set.buffer_size);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
  }

  data->multi->xfer_buf_borrowed = TRUE;
  *pbuf = data->multi->xfer_buf->data;
  *pbuflen = data->multi->xfer_buf->len;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_multi_xfer_buf_release(ಸಂರಚನಾ Curl_easy *data, ವರ್ಣ *buf)
{
  (ಶೂನ್ಯ)buf;
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  DEBUGASSERT(!buf || (data->multi->xfer_buf &&
                       data->multi->xfer_buf->data == buf));
  data->multi->xfer_buf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_ulbuf_borrow(ಸಂರಚನಾ Curl_easy *data,
                                      ವರ್ಣ **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  *pbuf = ರಿಕ್ತ;
  *pbuflen = 0;
  ಯದಿ(!data->multi) {
    failf(data, "transfer has no multi handle");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  ಯದಿ(data->multi->xfer_ulbuf_borrowed) {
    failf(data, "attempt to borrow xfer_ulbuf when already borrowed");
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  }

  ಯದಿ(data->multi->xfer_ulbuf &&
     data->set.upload_buffer_size > data->multi->xfer_ulbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(data->multi->xfer_ulbuf);
  }

  ಯದಿ(!data->multi->xfer_ulbuf) {
    data->multi->xfer_ulbuf =
      fixed_buf_create(curlx_uitouz(data->set.upload_buffer_size));
    ಯದಿ(!data->multi->xfer_ulbuf) {
      failf(data, "could not allocate xfer_ulbuf of %u bytes",
            data->set.upload_buffer_size);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
  }

  data->multi->xfer_ulbuf_borrowed = TRUE;
  *pbuf = data->multi->xfer_ulbuf->data;
  *pbuflen = data->multi->xfer_ulbuf->len;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_multi_xfer_ulbuf_release(ಸಂರಚನಾ Curl_easy *data, ವರ್ಣ *buf)
{
  (ಶೂನ್ಯ)buf;
  DEBUGASSERT(data);
  DEBUGASSERT(data->multi);
  DEBUGASSERT(!buf || (data->multi->xfer_ulbuf &&
                       data->multi->xfer_ulbuf->data == buf));
  data->multi->xfer_ulbuf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_sockbuf_borrow(ಸಂರಚನಾ Curl_easy *data,
                                        size_t blen, ವರ್ಣ **pbuf)
{
  DEBUGASSERT(data);
  *pbuf = ರಿಕ್ತ;
  ಯದಿ(!data->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    *pbuf = curlx_malloc(blen);
    ಪ್ರತ್ಯಾಗಮ *pbuf ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  ಯದಿ(data->multi->xfer_sockbuf_borrowed) {
    failf(data, "attempt to borrow xfer_sockbuf when already borrowed");
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  }

  ಯದಿ(data->multi->xfer_sockbuf && blen > data->multi->xfer_sockbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(data->multi->xfer_sockbuf);
  }

  ಯದಿ(!data->multi->xfer_sockbuf) {
    data->multi->xfer_sockbuf = fixed_buf_create(blen);
    ಯದಿ(!data->multi->xfer_sockbuf) {
      failf(data, "could not allocate xfer_sockbuf of %zu bytes", blen);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
  }

  data->multi->xfer_sockbuf_borrowed = TRUE;
  *pbuf = data->multi->xfer_sockbuf->data;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_multi_xfer_sockbuf_release(ಸಂರಚನಾ Curl_easy *data, ವರ್ಣ *buf)
{
  DEBUGASSERT(data);
  ಯದಿ(!data->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    curlx_free(buf);
  }
  ಅನ್ಯಥಾ {
    DEBUGASSERT(!buf || (data->multi->xfer_sockbuf &&
                         data->multi->xfer_sockbuf->data == buf));
    data->multi->xfer_sockbuf_borrowed = FALSE;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ multi_xfer_bufs_free(ಸಂರಚನಾ Curl_multi *multi)
{
  DEBUGASSERT(multi);
  curlx_safefree(multi->xfer_buf);
  multi->xfer_buf_borrowed = FALSE;
  curlx_safefree(multi->xfer_ulbuf);
  multi->xfer_ulbuf_borrowed = FALSE;
  curlx_safefree(multi->xfer_sockbuf);
  multi->xfer_sockbuf_borrowed = FALSE;
}

ಸಂರಚನಾ Curl_easy *Curl_multi_get_easy(ಸಂರಚನಾ Curl_multi *multi,
                                      uint32_t mid)
{
  ಸಂರಚನಾ Curl_easy *data = Curl_uint32_tbl_get(&multi->xfers, mid);
  ಯದಿ(GOOD_EASY_HANDLE(data))
    ಪ್ರತ್ಯಾಗಮ data;
  CURL_TRC_M(multi->admin, "invalid easy handle in xfer table for mid=%u",
             mid);
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

bool Curl_multi_knows_easy(ಸಂರಚನಾ Curl_multi *multi, ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ Curl_uint32_tbl_get(&multi->xfers, data->mid) == data;
}

ಸ್ಥಿರ uint32_t multi_xfers_running(ಸಂರಚನಾ Curl_multi *multi)
{
  ಯದಿ(!multi) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಪ್ರತ್ಯಾಗಮ multi->xfers_alive;
}

uint32_t Curl_multi_xfers_attached(ಸಂರಚನಾ Curl_multi *multi)
{
  ಯದಿ(!multi || !multi->admin) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  /* Discount the admin handle */
  ಪ್ರತ್ಯಾಗಮ Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers) - 1;
}

ಶೂನ್ಯ Curl_multi_mark_dirty(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(data->multi && data->mid != UINT32_MAX)
    Curl_uint32_bset_add(&data->multi->dirty, data->mid);
}

ಶೂನ್ಯ Curl_multi_clear_dirty(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(data->multi && data->mid != UINT32_MAX)
    Curl_uint32_bset_remove(&data->multi->dirty, data->mid);
}

CURLMcode curl_multi_notify_enable(CURLM *m, unsigned ಪೂರ್ಣಾಙ್ಕ notification)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_notify_enable, &mresult)) {
    mresult = Curl_mntfy_enable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

CURLMcode curl_multi_notify_disable(CURLM *m, unsigned ಪೂರ್ಣಾಙ್ಕ notification)
{
  ಸಂರಚನಾ Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ಯದಿ(CURL_MAPI_ENTER(&guard, m, multi_notify_disable, &mresult)) {
    mresult = Curl_mntfy_disable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

#ifdef DEBUGBUILD
ಸ್ಥಿರ ಶೂನ್ಯ multi_xfer_dump(ಸಂರಚನಾ Curl_multi *multi, uint32_t mid,
                            ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ Curl_easy *data = entry;

  (ಶೂನ್ಯ)multi;
  ಯದಿ(!data) {
    curl_mfprintf(stderr, "mid=%u, entry=NULL, bug in xfer table?\n", mid);
  }
  ಅನ್ಯಥಾ {
    curl_mfprintf(stderr, "mid=%u, magic=%s, p=%p, id=%" FMT_OFF_T
                  ", url=%s\n",
                  mid,
                  (data->magic == CURLEASY_MAGIC_ಸಂಖ್ಯೆ) ? "GOOD" : "BAD!",
                  (ಶೂನ್ಯ *)data, data->id, Curl_bufref_ptr(&data->state.url));
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ multi_xfer_tbl_dump(ಸಂರಚನಾ Curl_multi *multi)
{
  uint32_t mid;
  ಶೂನ್ಯ *entry;
  curl_mfprintf(stderr, "=== multi xfer table (count=%u, capacity=%u\n",
                Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers),
                Curl_uint32_tbl_capacity(&multi->xfers));
  ಯದಿ(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
    multi_xfer_dump(multi, mid, entry);
    ಯಾವತ್(Curl_uint32_tbl_next(&multi->xfers, mid, &mid, &entry))
      multi_xfer_dump(multi, mid, entry);
  }
  curl_mfprintf(stderr, "===\n");
  fflush(stderr);
}
#endif /* DEBUGBUILD */
