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
#تعریف_کړه CURL_XFER_جدول_اندازه 128

/* CURL_SOCKET_HASH_TABLE_SIZE should be a prime number. Increasing it from 97
   to 911 takes on a 32-bit machine 4 x 804 = 3211 more bytes. Still, every
   curl handle takes 6K memory, therefore this 3K are not significant. */
#که_نه_تعریف_شوی CURL_SOCKET_HASH_جدول_اندازه
#تعریف_کړه CURL_SOCKET_HASH_جدول_اندازه 911
#د_که_پای

#که_نه_تعریف_شوی CURL_CONNECTION_HASH_اندازه
#تعریف_کړه CURL_CONNECTION_HASH_اندازه 97
#د_که_پای

#که_نه_تعریف_شوی CURL_DNS_HASH_اندازه
#تعریف_کړه CURL_DNS_HASH_اندازه 71
#د_که_پای

#که_نه_تعریف_شوی CURL_TLS_SESSION_اندازه
#تعریف_کړه CURL_TLS_SESSION_اندازه 25
#د_که_پای

ساكن فراغ move_pending_منزل_connect(بنية Curl_multi *multi,
                                    بنية Curl_easy *معلومات);
ساكن CURLMcode جمع_کړه_بل_timeout(ثابت بنية curltime *pnow,
                                  بنية Curl_multi *multi,
                                  بنية Curl_easy *معلومات);
ساكن فراغ multi_timeout(بنية Curl_multi *multi,
                          timediff_t *pexire_دننه_واټن_us,
                          صحيح *timeout_ms);
ساكن فراغ multi_مهالوېش_pending(بنية Curl_multi *multi);
ساكن فراغ multi_xfer_bufs_free(بنية Curl_multi *multi);
#که_تعریف_شوی DEBUGBUILD
ساكن فراغ multi_xfer_tbl_dump(بنية Curl_multi *multi);
#د_که_پای
/* Get the # of transfers current in process/pending. */
ساكن uint32_t multi_xfers_running(بنية Curl_multi *multi);

/* function pointer called once when entering a state */
تعريفنوع فراغ (*mstate_enter_func)(بنية Curl_easy *معلومات,
                                  CURLMstate from_state);

ساكن فراغ mstate_enter_connect(بنية Curl_easy *معلومات,
                                 CURLMstate from_state)
{
  (فراغ)from_state;
  Curl_init_CONNECT(معلومات);
}

ساكن فراغ mstate_enter_did(بنية Curl_easy *معلومات,
                             CURLMstate from_state)
{
  (فراغ)from_state;
  معلومات->req.chunk = FALSE;
  Curl_pgrs_وخت(معلومات, TIMER_PRETRANSFER);
  إذا(!CURL_REQ_WANT_SEND(معلومات))
    Curl_pgrs_وخت(معلومات, TIMER_POSTRANSFER);
}

ساكن فراغ mstate_enter_بشپړ(بنية Curl_easy *معلومات,
                              CURLMstate from_state)
{
  (فراغ)from_state;
  CURLM_NTFY(معلومات, CURLMNOTIFY_EASY_بشپړ);
}

ساكن فراغ mstate_enter_completed(بنية Curl_easy *معلومات,
                                   CURLMstate from_state)
{
  /* we sometimes directly jump to COMPLETED, trigger things
   * we then missed. */
  إذا(from_state < MSTATE_DID) {
    Curl_pgrs_وخت(معلومات, TIMER_PRETRANSFER);
    Curl_pgrs_وخت(معلومات, TIMER_POSTRANSFER);
    Curl_pgrs_وخت(معلومات, TIMER_STARTTRANSFER);
  }
  /* multi_done() should have run and detached any connection already */
  DEBUGASSERT(!معلومات->conn);
  Curl_pgrsCompleted(معلومات);
  إذا(from_state < MSTATE_بشپړ)
    CURLM_NTFY(معلومات, CURLMNOTIFY_EASY_بشپړ);
  /* changing to COMPLETED means it is in process and needs to go */
  DEBUGASSERT(Curl_uint32_bset_لري(&معلومات->multi->process, معلومات->mid));
  Curl_uint32_bset_remove(&معلومات->multi->process, معلومات->mid);
  Curl_uint32_bset_remove(&معلومات->multi->pending, معلومات->mid); /* to be sure */

  إذا(Curl_uint32_bset_تشه(&معلومات->multi->process)) {
    /* free the transfer buffer when we have no more active transfers */
    multi_xfer_bufs_free(معلومات->multi);
  }
  /* Important: reset the conn pointer so that we do not point to memory
     that could be freed anytime */
  Curl_detach_connection(معلومات);
  Curl_expire_clear_all(معلومات); /* stop all timers */
}

/* always use this function to change state, to make debugging easier */
ساكن فراغ mstate(بنية Curl_easy *معلومات, CURLMstate state
#که_تعریف_شوی DEBUGBUILD
                   , صحيح lineno
#د_که_پای
)
{
  CURLMstate oldstate = معلومات->mstate;
  ساكن ثابت mstate_enter_func state_enter[MSTATE_وروستی] = {
    عدم,                      /* INIT */
    عدم,                      /* PENDING */
    عدم,                      /*تیاری*/
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
    mstate_enter_بشپړ,         /* DONE */
    mstate_enter_completed,    /* COMPLETED */
    عدم                       /* MSGSENT */
  };

  إذا(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    أرجع;

#که_تعریف_شوی DEBUGBUILD
  NOVERBOSE((فراغ)lineno);
  CURL_TRC_M(معلومات, "-> [%s] (line %d)", CURL_MSTATE_NAME(state), lineno);
#وإلا
  CURL_TRC_M(معلومات, "-> [%s]", CURL_MSTATE_NAME(state));
#د_که_پای

  /* really switching state */
  معلومات->mstate = state;
  إذا(state_enter[state])
    state_enter[state](معلومات, oldstate);
}

#که_نه_تعریف_شوی DEBUGBUILD
#تعریف_کړه multistate(x, y) mstate(x, y)
#وإلا
#تعریف_کړه multistate(x, y) mstate(x, y, __LINE__)
#د_که_پای

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
ساكن فراغ multi_addmsg(بنية Curl_multi *multi, بنية Curl_easy *معلومات)
{
  إذا(Curl_uint32_bset_تشه(&multi->msgsent))
    CURLM_NTFY(multi->admin, CURLMNOTIFY_INFO_READ);
  Curl_uint32_bset_جمع_کړه(&multi->msgsent, معلومات->mid);
}

ساكن فراغ multi_timeouts_init(بنية Curl_easy *معلومات);

بنية Curl_multi *Curl_multi_handle(uint32_t xfer_جدول_اندازه,
                                     size_t ev_hashsize,  /* event hash */
                                     size_t chashsize, /* connection hash */
                                     size_t dnssize,   /* dns hash */
                                     size_t sesssize)  /* TLS session cache */
{
  بنية curltime created;
  بنية Curl_multi *multi = curlx_calloc(1, حجم(بنية Curl_multi));

  إذا(!multi)
    أرجع عدم;

  multi->magic = CURLMULTI_MAGIC_شمېره;

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
  multi->admin->state.دننی = TRUE;

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
  Curl_hash_init(&multi->proto_hash, 23, CURL_HASH_TYPE_بایټونه, ph_freeentry);
  Curl_cshutdn_init(&multi->cshutdn);

  multi->multiplexing = TRUE;
  multi->max_concurrent_streams = 100;
#که_تعریف_شوی ENABLE_WAKEUP
  multi->wakeup_pair[0] = CURL_SOCKET_BAD;
  multi->wakeup_pair[1] = CURL_SOCKET_BAD;
#د_که_پای
#که_تعریف_شوی ENABLE_دننی_WAKEUP
  multi->wakeup_دننی[0] = CURL_SOCKET_BAD;
  multi->wakeup_دننی[1] = CURL_SOCKET_BAD;
#د_که_پای

  إذا(Curl_uint32_bset_resize(&multi->process, xfer_جدول_اندازه) ||
     Curl_uint32_bset_resize(&multi->pending, xfer_جدول_اندازه) ||
     Curl_uint32_bset_resize(&multi->dirty, xfer_جدول_اندازه) ||
     Curl_uint32_bset_resize(&multi->msgsent, xfer_جدول_اندازه) ||
     Curl_uint32_tbl_resize(&multi->xfers, xfer_جدول_اندازه))
    اذهب error;

#که_تعریف_شوی DEBUGBUILD
  {
    ثابت حرف *p;
    إذا(getenv("CURL_DEBUG"))
      multi->admin->سېټ.verbose = TRUE;
    p = getenv("CURL_DBG_MAX_TOTAL_CONNECTIONS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_شمېره(&p, &l, UINT32_MAX)) {
        multi->max_ټول_connections = (uint32_t)l;
      }
    }
  }
#د_که_پای
  Curl_uint32_tbl_جمع_کړه(&multi->xfers, multi->admin, &multi->admin->mid);
  Curl_uint32_bset_جمع_کړه(&multi->process, multi->admin->mid);

  Curl_cpool_init(&multi->cpool, عدم, chashsize);

#که_تعریف_شوی USE_SSL
  إذا(Curl_ssl_scache_create(sesssize, 2, &multi->ssl_scache))
    اذهب error;
#وإلا
  (فراغ)sesssize;
#د_که_پای

#که_تعریف_شوی USE_WINSOCK
  multi->wsa_event = WSACreateEvent();
  إذا(multi->wsa_event == WSA_INVALID_EVENT)
    اذهب error;
#د_که_پای
#که_تعریف_شوی ENABLE_WAKEUP
  /* When enabled, rely on this to work. We ignore this in previous
   * versions, but that seems an unnecessary complication. */
  إذا(Curl_wakeup_init(multi->wakeup_pair, TRUE) < 0)
    اذهب error;
#د_که_پای
#که_تعریف_شوی ENABLE_دننی_WAKEUP
  إذا(Curl_wakeup_init(multi->wakeup_دننی, TRUE) < 0)
    اذهب error;
#د_که_پای

  إذا(Curl_probeipv6(multi))
    اذهب error;

#که_تعریف_شوی USE_RESOLV_THREADED
  إذا(xfer_جدول_اندازه < CURL_XFER_جدول_اندازه) { /* easy multi */
    إذا(Curl_async_thrdd_multi_init(multi, 0, 2, 10))
      اذهب error;
  }
  وإلا { /* real multi handle */
    إذا(Curl_async_thrdd_multi_init(multi, 0, 20, 2000))
      اذهب error;
  }
#د_که_پای

  أرجع multi;

error:

#که_تعریف_شوی USE_RESOLV_THREADED
  Curl_async_thrdd_multi_destroy(multi, TRUE);
#د_که_پای
  Curl_multi_ev_cleanup(multi);
  Curl_hash_destroy(&multi->proto_hash);
  Curl_dnscache_destroy(&multi->dnscache);
  Curl_cpool_destroy(&multi->cpool, multi->admin);
  Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
#که_تعریف_شوی USE_SSL
  Curl_ssl_scache_destroy(multi->ssl_scache);
#د_که_پای
  إذا(multi->admin) {
    Curl_multi_ev_xfer_بشپړ(multi, multi->admin);
    multi->admin->multi = عدم;
    Curl_close(&multi->admin);
  }
  Curl_mntfy_cleanup(multi);

  Curl_uint32_bset_destroy(&multi->process);
  Curl_uint32_bset_destroy(&multi->dirty);
  Curl_uint32_bset_destroy(&multi->pending);
  Curl_uint32_bset_destroy(&multi->msgsent);
  Curl_uint32_tbl_destroy(&multi->xfers);
#که_تعریف_شوی ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#د_که_پای
#که_تعریف_شوی ENABLE_دننی_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_دننی);
#د_که_پای

  curlx_free(multi);
  أرجع عدم;
}

CURLM *curl_multi_init(فراغ)
{
  أرجع Curl_multi_handle(CURL_XFER_جدول_اندازه,
                           CURL_SOCKET_HASH_جدول_اندازه,
                           CURL_CONNECTION_HASH_اندازه,
                           CURL_DNS_HASH_اندازه,
                           CURL_TLS_SESSION_اندازه);
}

#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
ساكن فراغ multi_warn_debug(بنية Curl_multi *multi, بنية Curl_easy *معلومات)
{
  إذا(!multi->warned) {
    infof(معلومات, "!!! WARNING !!!");
    infof(معلومات, "This is a debug build of libcurl, "
                "do not use in production.");
    multi->warned = TRUE;
  }
}
#وإلا
#تعریف_کړه multi_warn_debug(x, y) Curl_nop_stmt
#د_که_پای

bool Curl_is_connecting(بنية Curl_easy *معلومات)
{
  أرجع معلومات->mstate < MSTATE_DO;
}

ساكن CURLMcode multi_assess_wakeup(بنية Curl_multi *multi)
{
#که_تعریف_شوی ENABLE_دننی_WAKEUP
  إذا(multi->socket_cb)
    أرجع Curl_multi_ev_assess_xfer(multi, multi->admin);
#وإلا
  (فراغ)multi;
#د_که_پای
  أرجع CURLM_OK;
}

ساكن CURLMcode multi_xfers_جمع_کړه(بنية Curl_multi *multi,
                                 بنية Curl_easy *معلومات)
{
  uint32_t ظرفیت = Curl_uint32_tbl_ظرفیت(&multi->xfers);
  uint32_t new_اندازه = 0;
  /* Prepare to make this into a CURLMOPT_MAX_TRANSFERS, because some
   * applications may want to prevent a run-away of their memory use. */
  /* UINT_MAX is our "invalid" id, do not let the table grow up to that. */
  ثابت uint32_t max_ظرفیت = UINT_MAX - 1;

  إذا(ظرفیت < max_ظرفیت) {
    /* We want `multi->xfers` to have "sufficient" free rows, so that we do
     * not have to reuse the `mid` from a removed easy right away.
     * Check if an 8th of the capacity is still free */
    uint32_t کارول_شوی = Curl_uint32_tbl_شمېر(&multi->xfers);
    uint32_t ناکارول_شوی = ظرفیت - کارول_شوی;
    uint32_t min_ناکارول_شوی = CURLMAX(ظرفیت >> 3, 4);
    إذا(ناکارول_شوی < min_ناکارول_شوی) {
      /* Grow by 50% of current capacity, in range of [128, 2048],
       * which means the table grows max by 16kb on 64-bit arch. */
      uint32_t growth = CURLMIN(CURLMAX(ظرفیت >> 1, 128), 2048);
      /* Make sure the uint arithmetic here works on the corner
       * cases where we are close to max_capacity or UINT_MAX */
      إذا((max_ظرفیت - growth) <= ظرفیت)
        new_اندازه = max_ظرفیت;
      وإلا
        new_اندازه = ظرفیت + growth;
    }
  }

  إذا(new_اندازه > ظرفیت) {
    /* Grow the bitsets first. Should one fail, we do not need
     * to downsize the already resized ones. The sets continue
     * to work properly when larger than the table, but not
     * the other way around. */
    CURL_TRC_M(معلومات, "increasing xfer table size to %u", new_اندازه);
    إذا(Curl_uint32_bset_resize(&multi->process, new_اندازه) ||
       Curl_uint32_bset_resize(&multi->dirty, new_اندازه) ||
       Curl_uint32_bset_resize(&multi->pending, new_اندازه) ||
       Curl_uint32_bset_resize(&multi->msgsent, new_اندازه) ||
       Curl_uint32_tbl_resize(&multi->xfers, new_اندازه))
      أرجع CURLM_بهر_د_حافظه;
  }

  /* Insert the easy into the table now */
  إذا(!Curl_uint32_tbl_جمع_کړه(&multi->xfers, معلومات, &معلومات->mid)) {
    /* MUST only happen when table is full */
    DEBUGASSERT(Curl_uint32_tbl_ظرفیت(&multi->xfers) <=
                Curl_uint32_tbl_شمېر(&multi->xfers));
    أرجع CURLM_بهر_د_حافظه;
  }
  أرجع CURLM_OK;
}

CURLMcode Curl_multi_جمع_کړه_handle(بنية Curl_multi *multi,
                                بنية Curl_easy *معلومات)
{
  CURLMcode mresult;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  إذا(معلومات->multi)
    أرجع CURLM_ADDED_ALREADY;

  إذا(multi->dead) {
    /* a "dead" handle cannot get added transfers while any existing easy
       handles are still alive - but if there are none alive anymore, it is
       fine to start over and unmark the "deadness" of this handle.
       This means only the admin handle MUST be present. */
    إذا((Curl_uint32_tbl_شمېر(&multi->xfers) != 1) ||
       !Curl_uint32_tbl_لري(&multi->xfers, 0))
      أرجع CURLM_ABORTED_BY_CALLBACK;
    multi->dead = FALSE;
    Curl_uint32_bset_clear(&multi->process);
    Curl_uint32_bset_clear(&multi->dirty);
    Curl_uint32_bset_clear(&multi->pending);
    Curl_uint32_bset_clear(&multi->msgsent);
  }

  إذا(معلومات->multi_easy) {
    /* if this easy handle was previously used for curl_easy_perform(), there
       is a private multi handle here that we can kill */
    curl_multi_cleanup(معلومات->multi_easy);
    معلومات->multi_easy = عدم;
  }

  /* Insert the easy into the multi->xfers table, assigning it a `mid`. */
  إذا(multi_xfers_جمع_کړه(multi, معلومات))
    أرجع CURLM_بهر_د_حافظه;

  /* Initialize timeouts for this handle */
  multi_timeouts_init(معلومات);

  /*
   * No failure allowed in this function beyond this point. No modification
   * of easy nor multi handle allowed before this except for potential
   * multi's connection pool growing which will not be undone in this
   * function no matter what.
   */
  إذا(معلومات->سېټ.errorbuffer)
    معلومات->سېټ.errorbuffer[0] = 0;

  معلومات->state.os_errno = 0;

  /* make the Curl_easy refer back to this multi handle - before
     Curl_expire() is called. */
  معلومات->multi = multi;

  /* set the easy handle */
  multistate(معلومات, MSTATE_INIT);
  /* not yet passed INIT state */
  معلومات->state.really_alive = FALSE;

#که_تعریف_شوی USE_LIBPSL
  /* Do the same for PSL. */
  إذا(معلومات->share && (معلومات->share->specifier & (1 << CURL_قلف_معلومات_PSL)))
    معلومات->psl = &معلومات->share->psl;
  وإلا
    معلومات->psl = &multi->psl;
#د_که_پای

  /* add the easy handle to the process set */
  Curl_uint32_bset_جمع_کړه(&multi->process, معلومات->mid);
  ++multi->xfers_alive;
  ++multi->xfers_ټول_ever;

  Curl_cpool_xfer_init(معلومات);
  multi_warn_debug(multi, معلومات);

  /* Make sure the new handle will run */
  Curl_multi_mark_dirty(معلومات);

  /* Necessary in event based processing, where dirty handles trigger
   * a timeout callback invocation. */
  mresult = Curl_update_timer(multi);
  إذا(mresult) {
    /* undo the additions above */
    Curl_uint32_bset_remove(&multi->process, معلومات->mid);
    Curl_uint32_bset_remove(&multi->dirty, معلومات->mid);
    --multi->xfers_alive;
    معلومات->multi = عدم; /* not anymore */
    Curl_uint32_tbl_remove(&multi->xfers, معلومات->mid);
    معلومات->mid = UINT32_MAX;
    أرجع mresult;
  }

  /* The admin handle only ever has default timeouts set. To improve the
     state somewhat we clone the timeouts from each added handle so that the
     admin handle always has the same timeouts as the most recently added
     easy handle. */
  multi->admin->سېټ.timeout = معلومات->سېټ.timeout;
  multi->admin->سېټ.server_response_timeout =
    معلومات->سېټ.server_response_timeout;
  multi->admin->سېټ.no_signal = معلومات->سېټ.no_signal;

  CURL_TRC_M(معلومات, "added to multi, mid=%u, running=%u, total=%u",
             معلومات->mid, multi_xfers_running(multi),
             Curl_uint32_tbl_شمېر(&multi->xfers));
  أرجع CURLM_OK;
}

CURLMcode curl_multi_جمع_کړه_handle(CURLM *m, CURL *curl)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_جمع_کړه_handle, &mresult)) {
    بنية Curl_easy *معلومات = curl;
    /* Verify that we got a somewhat good easy handle too */
    إذا(!GOOD_EASY_HANDLE(معلومات) || معلومات->state.دننی)
      mresult = CURLM_BAD_EASY_HANDLE;
    وإلا
      mresult = Curl_multi_جمع_کړه_handle(m, معلومات);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

بنية multi_بشپړ_ctx {
  بټ(premature);
};

ساكن bool multi_conn_should_close(بنية connectdata *conn,
                                    بنية Curl_easy *معلومات,
                                    bool premature)
{
  /* if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of everything else. */
  إذا(conn->بټونه.close)
    أرجع TRUE;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM/NEGOTIATE authentication handshake. */
  إذا(معلومات->سېټ.reuse_forbid
#که_تعریف_شوی USE_NTLM
     && !(conn->http_ntlm_state == NTLMSTATE_TYPE2 ||
          conn->proxy_ntlm_state == NTLMSTATE_TYPE2)
#د_که_پای
#که_تعریف_شوی USE_SPNEGO
     && !(conn->http_negotiate_state == CURL_NEGO_AUTH_RECV ||
          conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)
#د_که_پای
    )
    أرجع TRUE;

  /* Unless this connection is for a "connect-only" transfer, it
   * needs to be closed if the protocol handler does not support reuse. */
  إذا(!معلومات->سېټ.connect_only && conn->scheme &&
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

ساكن cpool_do_پایله multi_بشپړ_locked(بنية Curl_easy *معلومات,
                                         بنية connectdata *conn,
                                         فراغ *userdata,
                                         ثابت بنية curltime *pnow)
{
  بنية multi_بشپړ_ctx *mdctx = userdata;

  (فراغ)pnow;
  CURL_TRC_M(معلومات, "multi_done_locked, in use=%u", conn->attached_xfers);
  إذا(CONN_INUSE(conn)) {
    /* Stop if still used. */
    CURL_TRC_M(معلومات, "Connection still in use %u, no more multi_done now!",
               conn->attached_xfers);
    أرجع CPOOL_DO_KEEP;
  }

  إذا(multi_conn_should_close(conn, معلومات, (bool)mdctx->premature)) {
    CURL_TRC_M(معلومات, "multi_done, terminating conn #%" FMT_OFF_T " to %s:%u, "
               "forbid=%d, close=%d, premature=%d, conn_multiplex=%d",
               conn->connection_id, conn->origin->user_hostname,
               conn->origin->پورټ,
               معلومات->سېټ.reuse_forbid, conn->بټونه.close, mdctx->premature,
               Curl_conn_is_multiplex(conn, FIRSTSOCKET));
    connclose(conn);
    أرجع mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  وإلا إذا(!Curl_conn_get_max_concurrent(معلومات, conn, FIRSTSOCKET)) {
    CURL_TRC_M(معلومات, "multi_done, conn #%" FMT_OFF_T " to %s:%u was shutdown"
               " by server, not reusing", conn->connection_id,
               conn->origin->user_hostname, conn->origin->پورټ);
    connclose(conn);
    أرجع mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  وإلا {
    أرجع CPOOL_DO_IDLE;
  }
}

ساكن CURLcode multi_بشپړ(بنية Curl_easy *معلومات,
                           CURLcode حالت,  /* an error if this is called
                                                after an error was detected */
                           bool premature)
{
  CURLcode پایله;
  بنية connectdata *conn = معلومات->conn;
  ثابت بنية curltime *pnow = Curl_pgrs_now(معلومات);

  CURL_TRC_M(معلومات, "multi_done: status: %d prem: %d done: %d",
             (صحيح)حالت, (صحيح)premature, معلومات->state.بشپړ);

  إذا(معلومات->state.بشپړ) {
    /* Stop if multi_done() has already been called */
    أرجع CURLE_OK;
  }
  معلومات->state.بشپړ = TRUE; /* called now! */

  /* Shut down any ongoing async resolver operation. */
  Curl_resolv_shutdown_all(معلومات);

  /* Cleanup possible redirect junk */
  curlx_safefree(معلومات->req.newurl);
  curlx_safefree(معلومات->req.location);

  بدل(حالت) {
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
  إذا(conn && conn->scheme->پرلپسې->بشپړ && (معلومات->mstate >= MSTATE_PROTOCONNECT))
    پایله = conn->scheme->پرلپسې->بشپړ(معلومات, حالت, premature);
  وإلا
    پایله = حالت;

  إذا(معلومات->mstate > MSTATE_CONNECTING &&
     (پایله != CURLE_ABORTED_BY_CALLBACK)) {
    /* avoid this if
     * - the transfer has not connected
     * - we already aborted by callback to avoid this calling another callback
     */
    صحيح rc = Curl_pgrs_بشپړ(معلومات, pnow);
    إذا(!پایله && rc)
      پایله = CURLE_ABORTED_BY_CALLBACK;
  }

  /* Make sure that transfer client writes are really done now. */
  پایله = Curl_1st_fatal(پایله, Curl_xfer_write_بشپړ(معلومات, premature));

  إذا(!پایله)
    پایله = Curl_req_بشپړ(&معلومات->req, معلومات, premature);

  إذا(conn) {
    /* Detach connection and decide what to do with it. */
    بنية multi_بشپړ_ctx mdctx;

    /* Inform connection filters that this transfer is done and they
     * need to give up any references to `data`. */
    Curl_conn_ev_معلومات_بشپړ(معلومات, premature);
    Curl_detach_connection(معلومات);
    Curl_dnscache_prune(معلومات, pnow);

    memset(&mdctx, 0, حجم(mdctx));
    mdctx.premature = premature;
    Curl_cpool_return(معلومات, conn, multi_بشپړ_locked, &mdctx, pnow);

    إذا(معلومات->multi)
      multi_مهالوېش_pending(معلومات->multi);
  }

  /* flush the netrc cache */
  Curl_netrc_cleanup(&معلومات->state.netrc);
  أرجع پایله;
}

CURLMcode Curl_multi_remove_handle(بنية Curl_multi *multi,
                                   بنية Curl_easy *معلومات)
{
  CURLMcode mresult;
  bool premature;
  uint32_t mid;

  /* Prevent users from trying to remove same easy handle more than once */
  إذا(!معلومات->multi)
    أرجع CURLM_OK; /* it is already removed so let's say it is fine! */

  /* Prevent users from trying to remove an easy handle from the wrong multi */
  إذا(معلومات->multi != multi)
    أرجع CURLM_BAD_EASY_HANDLE;

  إذا(معلومات->mid == UINT32_MAX) {
    DEBUGASSERT(0);
    أرجع CURLM_دننی_ERROR;
  }
  إذا(Curl_uint32_tbl_get(&multi->xfers, معلومات->mid) != معلومات) {
    DEBUGASSERT(0);
    أرجع CURLM_دننی_ERROR;
  }

  premature = (معلومات->mstate < MSTATE_COMPLETED);

  إذا(معلومات->conn) {
    /* If the 'state' is not INIT or COMPLETED, we might need to do something
       nice to put the easy_handle in a good known state when this returns. */
    إذا(premature && (معلومات->mstate > MSTATE_DO))
      streamclose(معلومات->conn);

    /* multi_done() clears the association between the easy handle and the
       connection.
       Note that this ignores the return code because there is
       nothing really useful to do with it anyway! */
    (فراغ)multi_بشپړ(معلومات, معلومات->پایله, premature);
  }

  /* The timer must be shut down before data->multi is set to NULL, else
     data's splaynode would remain in the splay tree after curl_easy_cleanup is
     called. Do it after multi_done() in case that sets another time! */
  Curl_expire_clear_all(معلومات);

  /* In MSGSENT, it was deducted from `multi->xfers_alive` already. */
  إذا(معلومات->mstate != MSTATE_MSGSENT)
    --multi->xfers_alive;

  إذا(معلومات->state.really_alive) {
    معلومات->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    إذا(!multi->xfers_really_alive)
      (فراغ)multi_assess_wakeup(multi);
  }

  Curl_wildcard_dtor(&معلومات->wildcard);

  معلومات->mstate = MSTATE_COMPLETED;

  /* Remove the association between the connection and the handle */
  Curl_detach_connection(معلومات);

  /* Tell event handling that this transfer is definitely going away */
  Curl_multi_ev_xfer_بشپړ(multi, معلومات);

  إذا(معلومات->سېټ.connect_only) {
    إذا(معلومات->multi_easy) {
      إذا(معلومات->state.وروستی_conn_id != -1) {
        /* Mark any connect-only connection for closure */
        بنية connectdata *conn;
        (فراغ)Curl_getconnectinfo(معلومات, &conn);
        إذا(conn && conn->بټونه.connect_only)
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
      (فراغ)Curl_getconnectinfo(معلومات, &conn);
      إذا(conn)
        Curl_conn_close(معلومات, conn, TRUE);
    }
  }

#که_تعریف_شوی USE_LIBPSL
  /* Remove the PSL association. */
  إذا(معلومات->psl == &multi->psl)
    معلومات->psl = عدم;
#د_که_پای

  /* clear the association to this multi handle */
  mid = معلومات->mid;
  DEBUGASSERT(Curl_uint32_tbl_لري(&multi->xfers, mid));
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  Curl_uint32_bset_remove(&multi->process, mid);
  Curl_uint32_bset_remove(&multi->dirty, mid);
  Curl_uint32_bset_remove(&multi->pending, mid);
  Curl_uint32_bset_remove(&multi->msgsent, mid);
  معلومات->multi = عدم;
  معلومات->mid = UINT32_MAX;
  معلومات->master_mid = UINT32_MAX;

  /* A pending transfer *might* be able to run now. */
  multi_مهالوېش_pending(multi);
  mresult = Curl_update_timer(multi);
  إذا(mresult)
    أرجع mresult;

  mresult = multi_assess_wakeup(multi);
  إذا(mresult) {
    failf(معلومات, "error enabling wakeup listening: %d", mresult);
    أرجع mresult;
  }

  CURL_TRC_M(معلومات, "removed from multi, mid=%u, running=%u, total=%u",
             mid, multi_xfers_running(multi),
             Curl_uint32_tbl_شمېر(&multi->xfers));
  أرجع CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *curl)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_remove_handle, &mresult)) {
    بنية Curl_easy *معلومات = curl;
    إذا(!GOOD_EASY_HANDLE(معلومات) || معلومات->state.دننی)
      mresult = CURLM_BAD_EASY_HANDLE;
    وإلا
      mresult = Curl_multi_remove_handle(m, معلومات);
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
فراغ Curl_detach_connection(بنية Curl_easy *معلومات)
{
  بنية connectdata *conn = معلومات->conn;
  إذا(conn) {
    /* this should never happen, prevent underflow */
    DEBUGASSERT(conn->attached_xfers);
    إذا(conn->attached_xfers) {
      conn->attached_xfers--;
      إذا(!conn->attached_xfers)
        conn->attached_multi = عدم;
    }
  }
  معلومات->conn = عدم;
}

/*
 * Curl_attach_connection() attaches this transfer to this connection.
 *
 * This is the only function that should assign data->conn.
 * `matched == TRUE` means the transfer's properties match this
 * connection and it is not a temporary attach for maintenance.
 */
فراغ Curl_attach_connection(بنية Curl_easy *معلومات,
                            بنية connectdata *conn,
                            bool matched)
{
  DEBUGASSERT(معلومات);
  DEBUGASSERT(!معلومات->conn);
  DEBUGASSERT(conn);
  DEBUGASSERT(conn->attached_xfers < UINT32_MAX);
  معلومات->conn = conn;
  إذا(matched) {
    معلومات->state.وروستی_conn_id = conn->connection_id;
    معلومات->state.وروستی_cpid = conn->cpid;
  }
  وإلا
    DEBUGASSERT(!معلومات->mid); /* admin handle */
  conn->attached_xfers++;
  /* all attached transfers must be from the same multi */
  إذا(!conn->attached_multi)
    conn->attached_multi = معلومات->multi;
  DEBUGASSERT(conn->attached_multi == معلومات->multi);

  إذا(conn->scheme && conn->scheme->پرلپسې->attach)
    conn->scheme->پرلپسې->attach(معلومات, conn);
}

/* adjust pollset for rate limits/pauses */
ساكن CURLcode multi_adjust_pollset(بنية Curl_easy *معلومات,
                                     بنية easy_pollset *ps)
{
  CURLcode پایله = CURLE_OK;

  إذا(ps->n) {
    bool send_blocked, recv_blocked;

    recv_blocked = (Curl_rlimit_avail(&معلومات->پرمختګ.dl.rlimit, عدم) <= 0);
    send_blocked = (Curl_rlimit_avail(&معلومات->پرمختګ.ul.rlimit, عدم) <= 0);
    إذا(send_blocked || recv_blocked) {
      صحيح i;
      لكل(i = 0; i <= SECONDARYSOCKET; ++i) {
        curl_socket_t sock = معلومات->conn->sock[i];
        إذا(sock == CURL_SOCKET_BAD)
          استمر;
        إذا(recv_blocked && Curl_pollset_want_recv(معلومات, ps, sock)) {
          پایله = Curl_pollset_remove_دننه(معلومات, ps, sock);
          إذا(پایله)
            اكسر;
        }
        إذا(send_blocked && Curl_pollset_want_send(معلومات, ps, sock)) {
          پایله = Curl_pollset_remove_بهر(معلومات, ps, sock);
          إذا(پایله)
            اكسر;
        }
      }
    }

    /* Not blocked and wanting to receive. If there is data pending
     * in the connection filters, make transfer run again. */
    إذا(!recv_blocked &&
       ((Curl_pollset_want_recv(معلومات, ps, معلومات->conn->sock[FIRSTSOCKET]) &&
         Curl_conn_معلومات_pending(معلومات, FIRSTSOCKET)) ||
        (Curl_pollset_want_recv(معلومات, ps, معلومات->conn->sock[SECONDARYSOCKET]) &&
         Curl_conn_معلومات_pending(معلومات, SECONDARYSOCKET)))) {
      CURL_TRC_M(معلومات, "pollset[] has POLLIN, but there is still "
                 "buffered input -> mark as dirty");
      Curl_multi_mark_dirty(معلومات);
    }
  }
  أرجع پایله;
}

ساكن CURLcode mstate_connecting_pollset(بنية Curl_easy *معلومات,
                                          بنية easy_pollset *ps)
{
  بنية connectdata *conn = معلومات->conn;
  curl_socket_t sockfd;
  CURLcode پایله = CURLE_OK;

  إذا(Curl_xfer_recv_is_paused(معلومات))
    أرجع CURLE_OK;
  /* If a socket is set, receiving is default. If the socket
   * has not been determined yet (eyeballing), always ask the
   * connection filters for what to monitor. */
  sockfd = Curl_conn_get_لومړی_socket(معلومات);
  إذا(sockfd != CURL_SOCKET_BAD) {
    پایله = Curl_pollset_change(معلومات, ps, sockfd, CURL_POLL_دننه, 0);
    إذا(!پایله)
      پایله = multi_adjust_pollset(معلومات, ps);
  }
  إذا(!پایله)
    پایله = Curl_conn_adjust_pollset(معلومات, conn, ps);
  أرجع پایله;
}

ساكن CURLcode mstate_protocol_pollset(بنية Curl_easy *معلومات,
                                        بنية easy_pollset *ps)
{
  بنية connectdata *conn = معلومات->conn;
  CURLcode پایله = CURLE_OK;

  إذا(conn->scheme->پرلپسې->proto_pollset)
    پایله = conn->scheme->پرلپسې->proto_pollset(معلومات, ps);
  وإلا {
    curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
    إذا(sockfd != CURL_SOCKET_BAD) {
      /* Default is to wait to something from the server */
      پایله = Curl_pollset_change(معلومات, ps, sockfd, CURL_POLL_دننه, 0);
    }
  }
  إذا(!پایله)
    پایله = multi_adjust_pollset(معلومات, ps);
  إذا(!پایله)
    پایله = Curl_conn_adjust_pollset(معلومات, conn, ps);
  أرجع پایله;
}

ساكن CURLcode mstate_do_pollset(بنية Curl_easy *معلومات,
                                  بنية easy_pollset *ps)
{
  بنية connectdata *conn = معلومات->conn;
  CURLcode پایله = CURLE_OK;

  إذا(conn->scheme->پرلپسې->doing_pollset)
    پایله = conn->scheme->پرلپسې->doing_pollset(معلومات, ps);
  وإلا إذا(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    پایله = Curl_pollset_جمع_کړه_بهر(معلومات, ps, conn->sock[conn->send_idx]);
  }
  إذا(!پایله)
    پایله = multi_adjust_pollset(معلومات, ps);
  إذا(!پایله)
    پایله = Curl_conn_adjust_pollset(معلومات, conn, ps);
  أرجع پایله;
}

ساكن CURLcode mstate_domore_pollset(بنية Curl_easy *معلومات,
                                      بنية easy_pollset *ps)
{
  بنية connectdata *conn = معلومات->conn;
  CURLcode پایله = CURLE_OK;

  إذا(conn->scheme->پرلپسې->domore_pollset)
    پایله = conn->scheme->پرلپسې->domore_pollset(معلومات, ps);
  وإلا إذا(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    پایله = Curl_pollset_جمع_کړه_بهر(معلومات, ps, conn->sock[conn->send_idx]);
  }
  إذا(!پایله)
    پایله = multi_adjust_pollset(معلومات, ps);
  إذا(!پایله)
    پایله = Curl_conn_adjust_pollset(معلومات, conn, ps);
  أرجع پایله;
}

ساكن CURLcode mstate_perform_pollset(بنية Curl_easy *معلومات,
                                       بنية easy_pollset *ps)
{
  بنية connectdata *conn = معلومات->conn;
  CURLcode پایله = CURLE_OK;

  إذا(conn->scheme->پرلپسې->perform_pollset)
    پایله = conn->scheme->پرلپسې->perform_pollset(معلومات, ps);
  وإلا {
    /* Default is to obey the request flags for send/recv */
    إذا(Curl_req_want_recv(معلومات) && CONN_SOCK_IDX_VALID(conn->recv_idx)) {
      پایله = Curl_pollset_جمع_کړه_دننه(معلومات, ps, conn->sock[conn->recv_idx]);
    }
    إذا(!پایله && Curl_req_want_send(معلومات) &&
       CONN_SOCK_IDX_VALID(conn->send_idx)) {
      پایله = Curl_pollset_جمع_کړه_بهر(معلومات, ps, conn->sock[conn->send_idx]);
    }
  }
  إذا(!پایله)
    پایله = multi_adjust_pollset(معلومات, ps);
  إذا(!پایله)
    پایله = Curl_conn_adjust_pollset(معلومات, conn, ps);
  أرجع پایله;
}

#که_تعریف_شوی CURLVERBOSE
ساكن size_t multi_timeouts_شمېر(بنية expire_timers *timeouts)
{
  size_t n = 0;
  uint8_t eid = timeouts->لومړی;
  لكل(; eid < EXPIRE_وروستی; eid = timeouts->بل[eid])
    ++n;
  أرجع n;
}
#د_که_پای

/* Initializes `poll_set` with the current socket poll actions needed
 * for transfer `data`. */
CURLMcode Curl_multi_pollset(بنية Curl_easy *معلومات,
                             بنية easy_pollset *ps)
{
  CURLcode پایله = CURLE_OK;

  Curl_pollset_reset(ps);
#که_تعریف_شوی ENABLE_دننی_WAKEUP
  /* The admin handle always listens on the wakeup socket when there
   * are transfers alive. */
  إذا(معلومات->multi && (معلومات == معلومات->multi->admin) &&
     معلومات->multi->xfers_really_alive) {
    CURL_TRC_M(معلومات, "adding wakeup, %u xfers really alive",
               معلومات->multi->xfers_really_alive);
    پایله = Curl_pollset_جمع_کړه_دننه(معلومات, ps, معلومات->multi->wakeup_دننی[0]);
  }
#د_که_پای
  /* If the transfer has no connection, this is fine. Happens when
     called via curl_multi_remove_handle() => Curl_multi_ev_assess() =>
     Curl_multi_pollset(). */
  إذا(!پایله && معلومات->conn) {
    بدل(معلومات->mstate) {
    حالة MSTATE_INIT:
    حالة MSTATE_PENDING:
    حالة MSTATE_تیاری:
    حالة MSTATE_CONNECT:
      /* nothing to poll for yet */
      اكسر;

    حالة MSTATE_CONNECTING:
      پایله = mstate_connecting_pollset(معلومات, ps);
      اكسر;

    حالة MSTATE_PROTOCONNECT:
    حالة MSTATE_PROTOCONNECTING:
      پایله = mstate_protocol_pollset(معلومات, ps);
      اكسر;

    حالة MSTATE_DO:
    حالة MSTATE_DOING:
      پایله = mstate_do_pollset(معلومات, ps);
      اكسر;

    حالة MSTATE_DOING_نور:
      پایله = mstate_domore_pollset(معلومات, ps);
      اكسر;

    حالة MSTATE_DID: /* same as PERFORMING in regard to polling */
    حالة MSTATE_PERFORMING:
      پایله = mstate_perform_pollset(معلومات, ps);
      اكسر;

    حالة MSTATE_RATELIMITING:
      /* we need to let time pass, ignore socket(s) */
      اكسر;

    حالة MSTATE_بشپړ:
    حالة MSTATE_COMPLETED:
    حالة MSTATE_MSGSENT:
      /* nothing more to poll for */
      اكسر;

    افتراضي:
      failf(معلومات, "multi_getsock: unexpected multi state %d",
            (صحيح)معلومات->mstate);
      DEBUGASSERT(0);
      اكسر;
    }
  }

  إذا(پایله) {
    إذا(پایله == CURLE_بهر_د_حافظه)
      أرجع CURLM_بهر_د_حافظه;
    failf(معلومات, "error determining pollset: %d", (صحيح)پایله);
    أرجع CURLM_دننی_ERROR;
  }

#که_تعریف_شوی CURLVERBOSE
  إذا(CURL_TRC_M_is_verbose(معلومات)) {
    size_t timeout_شمېر = multi_timeouts_شمېر(&معلومات->state.timeouts);
    بدل(ps->n) {
    حالة 0:
      CURL_TRC_M(معلومات, "pollset[], timeouts=%zu, paused %d/%d (r/w)",
                 timeout_شمېر,
                 Curl_xfer_send_is_paused(معلومات),
                 Curl_xfer_recv_is_paused(معلومات));
      اكسر;
    حالة 1:
      CURL_TRC_M(معلومات, "pollset[fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_دننه) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_بهر) ? "OUT" : "",
                 timeout_شمېر);
      اكسر;
    حالة 2:
      CURL_TRC_M(معلومات, "pollset[fd=%" FMT_SOCKET_T " %s%s, "
                 "fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_دننه) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_بهر) ? "OUT" : "",
                 ps->sockets[1],
                 (ps->actions[1] & CURL_POLL_دننه) ? "IN" : "",
                 (ps->actions[1] & CURL_POLL_بهر) ? "OUT" : "",
                 timeout_شمېر);
      اكسر;
    افتراضي:
      CURL_TRC_M(معلومات, "pollset[fds=%u], timeouts=%zu", ps->n, timeout_شمېر);
      اكسر;
    }
    CURL_TRC_EASY_TIMERS(معلومات);
  }
#د_که_پای

  أرجع CURLM_OK;
}

CURLMcode curl_multi_fdset(CURLM *m,
                           fd_set *read_fd_سېټ, fd_set *write_fd_سېټ,
                           fd_set *exc_fd_سېټ, صحيح *max_fd)
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
    بینښه صحيح i;
    uint32_t mid;
    (فراغ)exc_fd_سېټ;

    Curl_pollset_init(&ps);
    إذا(Curl_uint32_bset_لومړی(&multi->process, &mid)) {
      افعل {
        بنية Curl_easy *معلومات = Curl_multi_get_easy(multi, mid);

        إذا(!معلومات) {
          DEBUGASSERT(0);
          استمر;
        }

        Curl_multi_pollset(معلومات, &ps);
        لكل(i = 0; i < ps.n; i++) {
          إذا(!FDSET_SOCK(ps.sockets[i]))
            /* pretend it does not exist */
            استمر;
          إذا(ps.actions[i] & CURL_POLL_دننه)
            FD_SET(ps.sockets[i], read_fd_سېټ);
          إذا(ps.actions[i] & CURL_POLL_بهر)
            FD_SET(ps.sockets[i], write_fd_سېټ);
          إذا((صحيح)ps.sockets[i] > this_max_fd)
            this_max_fd = (صحيح)ps.sockets[i];
        }
      } طالما(Curl_uint32_bset_بل(&multi->process, mid, &mid));
    }

    Curl_cshutdn_setfds(&multi->cshutdn, multi->admin,
                        read_fd_سېټ, write_fd_سېټ, &this_max_fd);

    *max_fd = this_max_fd;
    Curl_pollset_cleanup(&ps);

    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

CURLMcode curl_multi_waitfds(CURLM *m,
                             بنية curl_waitfd *ufds,
                             بینښه صحيح اندازه,
                             بینښه صحيح *fd_شمېر)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_waitfds, &mresult)) {
    بنية Curl_waitfds cwfds;
    بنية Curl_multi *multi = m;
    بنية easy_pollset ps;
    بینښه صحيح need = 0;
    uint32_t mid;

    إذا(!ufds && (اندازه || !fd_شمېر)) {
      mresult = CURLM_BAD_FUNCTION_دلیل;
      اذهب بهر;
    }

    Curl_pollset_init(&ps);
    Curl_waitfds_init(&cwfds, ufds, اندازه);
    mresult = CURLM_OK;
    إذا(Curl_uint32_bset_لومړی(&multi->process, &mid)) {
      افعل {
        بنية Curl_easy *معلومات = Curl_multi_get_easy(multi, mid);
        إذا(!معلومات) {
          DEBUGASSERT(0);
          Curl_uint32_bset_remove(&multi->process, mid);
          Curl_uint32_bset_remove(&multi->dirty, mid);
          استمر;
        }
        Curl_multi_pollset(معلومات, &ps);
        need += Curl_waitfds_جمع_کړه_ps(&cwfds, &ps);
      } طالما(Curl_uint32_bset_بل(&multi->process, mid, &mid));
    }

    need += Curl_cshutdn_جمع_کړه_waitfds(&multi->cshutdn, multi->admin, &cwfds);

    إذا(need != cwfds.n && ufds)
      mresult = CURLM_بهر_د_حافظه;

    إذا(fd_شمېر)
      *fd_شمېر = need;
    Curl_pollset_cleanup(&ps);
  }
بهر:
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

#که_تعریف_شوی USE_WINSOCK
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
                                      بینښه صحيح curl_nfds,
                                      بنية curl_waitfd extra_fds[],
                                      بینښه صحيح extra_nfds,
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
    اوږد ماسک = 0;
    إذا(cpfds->pfds[i].events & POLLIN)
      ماسک |= FD_READ | FD_ACCEPT | FD_CLOSE;
    إذا(cpfds->pfds[i].events & POLLPRI)
      ماسک |= FD_OOB;
    إذا(cpfds->pfds[i].events & POLLOUT) {
      ماسک |= FD_WRITE | FD_CONNECT | FD_CLOSE;
      reset_socket_fdwrite(cpfds->pfds[i].fd);
    }
    إذا(ماسک && WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, ماسک)) {
      mresult = CURLM_بهر_د_حافظه;
      اذهب بهر;
    }
  }

  إذا(cpfds->n || extrawait) {
    صحيح pollrc = 0;

    إذا(cpfds->n) {         /* pre-check with Winsock */
      pollrc = Curl_poll(cpfds->pfds, cpfds->n, 0);
      إذا(pollrc < 0) {
        mresult = CURLM_UNRECOVERABLE_POLL;
        اذهب بهر;
      }
      nevents = pollrc;
    }

    إذا(!nevents) {
      /* now wait... if not ready during the pre-check (pollrc == 0) */
      WSAWait_For_مضرب_Events(1, &multi->wsa_event, FALSE, (DWORD)timeout_ms,
                               FALSE);
    }

    /* With Winsock, we have to run the following section unconditionally
       to call WSAEventSelect(fd, event, 0) on all the sockets */
    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    لكل(i = 0; i < extra_nfds; i++) {
      بینښه لنډ ماسک = 0;
      curl_socket_t s = extra_fds[i].fd;

      wsa_events.l_شبکه_Events = 0;
      إذا(WSAEnum_شبکه_Events(s, عدم, &wsa_events) == 0) {
        إذا(wsa_events.l_شبکه_Events & (FD_READ | FD_ACCEPT | FD_CLOSE))
          ماسک |= CURL_WAIT_POLLIN;
        إذا(wsa_events.l_شبکه_Events & (FD_WRITE | FD_CONNECT | FD_CLOSE))
          ماسک |= CURL_WAIT_POLLOUT;
        إذا(wsa_events.l_شبکه_Events & FD_OOB)
          ماسک |= CURL_WAIT_POLLPRI;
        إذا(!pollrc && wsa_events.l_شبکه_Events)
          nevents++;
      }
      WSAEventSelect(s, multi->wsa_event, 0);
      إذا(!pollrc) {
        extra_fds[i].revents = (لنډ)ماسک;
        استمر;
      }
      وإلا {
        بینښه r = (بینښه)cpfds->pfds[curl_nfds + i].revents;
        إذا(r & POLLIN)
          ماسک |= CURL_WAIT_POLLIN;
        إذا(r & POLLOUT)
          ماسک |= CURL_WAIT_POLLOUT;
        إذا(r & POLLPRI)
          ماسک |= CURL_WAIT_POLLPRI;
        extra_fds[i].revents = (لنډ)ماسک;
      }
    }

    /* Count up all our own sockets that had activity,
       and remove them from the event. */
    لكل(i = 0; i < curl_nfds; ++i) {
      wsa_events.l_شبکه_Events = 0;
      إذا(WSAEnum_شبکه_Events(cpfds->pfds[i].fd, عدم, &wsa_events) == 0) {
        إذا(!pollrc && wsa_events.l_شبکه_Events)
          nevents++;
      }
      WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, 0);
    }
    WSAResetEvent(multi->wsa_event);
  }

بهر:
  *pnevents = nevents;
  أرجع mresult;
}

#وإلا /* USE_WINSOCK */

ساكن CURLMcode multi_posix_poll(بنية Curl_multi *multi,
                                  بنية curl_pollfds *cpfds,
                                  بینښه صحيح curl_nfds,
                                  بنية curl_waitfd extra_fds[],
                                  بینښه صحيح extra_nfds,
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
      اذهب بهر;
    }
    nevents = pollrc;

    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    لكل(i = 0; i < extra_nfds; i++) {
      بینښه r = (بینښه)cpfds->pfds[curl_nfds + i].revents;
      بینښه لنډ ماسک = 0;
      إذا(r & POLLIN)
        ماسک |= CURL_WAIT_POLLIN;
      إذا(r & POLLOUT)
        ماسک |= CURL_WAIT_POLLOUT;
      إذا(r & POLLPRI)
        ماسک |= CURL_WAIT_POLLPRI;
      extra_fds[i].revents = (لنډ)ماسک;
    }
  }
  وإلا إذا(extrawait) {
    /* No fds to poll, but asked to obey timeout_ms anyway. We cannot
     * use Curl_poll() as it, on some platforms, returns immediately
     * without fds. */
    curlx_wait_ms(timeout_ms);
  }

بهر:
  *pnevents = nevents;
  أرجع mresult;
}

#د_که_پای /* !USE_WINSOCK */

#تعریف_کړه عدد_POLLS_ON_ډېری 10

ساكن CURLMcode multi_wait(بنية Curl_multi *multi,
                            بنية curl_waitfd extra_fds[],
                            بینښه صحيح extra_nfds,
                            صحيح timeout_ms,
                            صحيح *ret,
                            bool extrawait)  /* when no socket, wait */
{
  size_t i;
  صحيح timeout_دننی;
  صحيح nevents = 0;
  بنية easy_pollset ps;
  بنية pollfd a_few_on_ډېری[عدد_POLLS_ON_ډېری];
  بنية curl_pollfds cpfds;
  بینښه صحيح curl_nfds = 0; /* how many pfds are for curl transfers */
  بنية Curl_easy *معلومات = عدم;
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;
#که_تعریف_شوی ENABLE_WAKEUP
  صحيح wakeup_idx = -1;
#د_که_پای

  إذا(timeout_ms < 0)
    أرجع CURLM_BAD_FUNCTION_دلیل;

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ډېری, عدد_POLLS_ON_ډېری);

  /* Add the curl handles to our pollfds first */
  إذا(Curl_uint32_bset_لومړی(&multi->process, &mid)) {
    افعل {
      معلومات = Curl_multi_get_easy(multi, mid);
      إذا(!معلومات) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        استمر;
      }
      Curl_multi_pollset(معلومات, &ps);
      إذا(Curl_pollfds_جمع_کړه_ps(&cpfds, &ps)) {
        mresult = CURLM_بهر_د_حافظه;
        اذهب بهر;
      }
    } طالما(Curl_uint32_bset_بل(&multi->process, mid, &mid));
  }

  إذا(Curl_cshutdn_جمع_کړه_pollfds(&multi->cshutdn, multi->admin, &cpfds)) {
    mresult = CURLM_بهر_د_حافظه;
    اذهب بهر;
  }

#که_تعریف_شوی ENABLE_WAKEUP
  /* If `extrawait` is TRUE *or* we have `extra_fds`to poll *or* we
   * have transfer sockets to poll, we obey `timeout_ms`.
   * Then we need to also monitor the multi's wakeup
   * socket to catch calls to `curl_multi_wakeup()` during the wait. */
  إذا(extrawait || cpfds.n || extra_nfds) {
    wakeup_idx = cpfds.n;
    إذا(Curl_pollfds_جمع_کړه_sock(&cpfds, multi->wakeup_pair[0], POLLIN)) {
      mresult = CURLM_بهر_د_حافظه;
      اذهب بهر;
    }
  }
#د_که_پای

  curl_nfds = cpfds.n; /* what curl internally uses in cpfds */
  /* Add external file descriptions from poll-like struct curl_waitfd */
  لكل(i = 0; i < extra_nfds; i++) {
    بینښه لنډ events = 0;
    إذا(extra_fds[i].events & CURL_WAIT_POLLIN)
      events |= POLLIN;
    إذا(extra_fds[i].events & CURL_WAIT_POLLPRI)
      events |= POLLPRI;
    إذا(extra_fds[i].events & CURL_WAIT_POLLOUT)
      events |= POLLOUT;
    إذا(Curl_pollfds_جمع_کړه_sock(&cpfds, extra_fds[i].fd, events)) {
      mresult = CURLM_بهر_د_حافظه;
      اذهب بهر;
    }
  }

  /* We check the internal timeout *AFTER* we collected all sockets to
   * poll. Collecting the sockets may install new timers by protocols
   * and connection filters.
   * Use the shorter one of the internal and the caller requested timeout.
   * If we are called with `!extrawait` and multi_timeout() reports no
   * timeouts exist, do not wait. */
  multi_timeout(multi, عدم, &timeout_دننی);
  إذا((timeout_دننی >= 0) && (timeout_دننی < timeout_ms))
    timeout_ms = timeout_دننی;

  إذا(معلومات)
    CURL_TRC_M(معلومات, "multi_wait(fds=%u, timeout=%d) tinternal=%d",
               cpfds.n, timeout_ms, timeout_دننی);

#که_تعریف_شوی USE_WINSOCK
  mresult = multi_winsock_select(multi, &cpfds, curl_nfds,
                                 extra_fds, extra_nfds,
                                 timeout_ms, extrawait, &nevents);
#وإلا
  mresult = multi_posix_poll(multi, &cpfds, curl_nfds,
                             extra_fds, extra_nfds,
                             timeout_ms, extrawait, &nevents);
#د_که_پای

#که_تعریف_شوی ENABLE_WAKEUP
    إذا(nevents && (wakeup_idx >= 0)) {
      إذا(cpfds.pfds[wakeup_idx].revents & POLLIN) {
        (فراغ)Curl_wakeup_consume(multi->wakeup_pair, TRUE);
        /* do not count the wakeup socket into the returned value */
        nevents--;
      }
    }
#د_که_پای

بهر:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  إذا(ret)
    *ret = nevents;
  أرجع mresult;
}

CURLMcode curl_multi_wait(CURLM *m,
                          بنية curl_waitfd extra_fds[],
                          بینښه صحيح extra_nfds,
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
                          بینښه صحيح extra_nfds,
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

#که_تعریف_شوی ENABLE_WAKEUP
  /* the wakeup_pair variable is only written during init and cleanup,
     making it safe to access from another thread after the init part
     and before cleanup */
  إذا(!Curl_wakeup_signal(multi->wakeup_pair))
    mresult = CURLM_OK;
#د_که_پای
#که_تعریف_شوی USE_WINSOCK
  إذا(WSASetEvent(multi->wsa_event))
    mresult = CURLM_OK;
#د_که_پای
  أرجع mresult;
}

#که_تعریف_شوی ENABLE_دننی_WAKEUP
فراغ Curl_multi_wakeup_دننی(بنية Curl_multi *multi)
{
  /* This is expected to be invocable from another thread which
   * does NOT outlive the multi handle. Check for sanity. */
  إذا(GOOD_MULTI_HANDLE(multi))
    Curl_wakeup_signal(multi->wakeup_دننی);
  وإلا
    DEBUGASSERT(0);
}
#د_که_پای

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

CURLMcode Curl_multi_جمع_کړه_perform(بنية Curl_multi *multi,
                                 بنية Curl_easy *معلومات,
                                 بنية connectdata *conn)
{
  CURLMcode mresult;

  mresult = Curl_multi_جمع_کړه_handle(multi, معلومات);
  إذا(!mresult) {
    CURLcode پایله;

    /* pass in NULL for 'conn' here since we do not want to init the
       connection, only this transfer */
    پایله = Curl_init_transfer(معلومات, عدم);
    إذا(پایله) {
      Curl_multi_remove_handle(multi, معلومات);
      أرجع CURLM_دننی_ERROR;
    }

    /* take this handle to the perform state right away */
    multistate(معلومات, MSTATE_PERFORMING);
    Curl_attach_connection(معلومات, conn, TRUE);
    CURL_REQ_سېټ_RECV(معلومات);
  }
  أرجع mresult;
}

ساكن CURLcode multi_do(بنية Curl_easy *معلومات, bool *بشپړ)
{
  CURLcode پایله = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(conn->scheme);

  إذا(conn->scheme->پرلپسې->do_it)
    پایله = conn->scheme->پرلپسې->do_it(معلومات, بشپړ);

  أرجع پایله;
}

/*
 * multi_do_more() is called during the DO_MORE multi state. It is a second
 * stage DO state which (wrongly) was introduced to support FTP's second
 * connection.
 *
 * 'complete' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (to DOING state when there is more work to do)
 */

ساكن CURLcode multi_do_نور(بنية Curl_easy *معلومات, domore *complete)
{
  CURLcode پایله = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;

  *complete = DOMORE_INCOMPLETE;

  إذا(conn->scheme->پرلپسې->do_نور)
    پایله = conn->scheme->پرلپسې->do_نور(معلومات, complete);

  أرجع پایله;
}

/*
 * Check whether a timeout occurred, and handle it if it did
 */
ساكن bool multi_handle_timeout(بنية Curl_easy *معلومات,
                                 ثابت بنية curltime *pnow,
                                 bool *بهیر_error,
                                 CURLcode *پایله)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_timeleft_now_ms(معلومات, pnow);
  إذا(timeout_ms < 0) {
    /* Handle timed out */
    timerid بنسټ_timer = Curl_is_connecting(معلومات) ?
                         TIMER_STARTSINGLE : TIMER_STARTOP;
    timediff_t elapsed_ms = Curl_pgrs_since_ms(معلومات, عدم, بنسټ_timer);
    إذا(معلومات->mstate == MSTATE_CONNECTING) {
      failf(معلومات, "%s timed out after %" FMT_TIMEDIFF_T " milliseconds",
            معلومات->conn->بټونه.dns_resolved ? "Connection" : "Resolving",
            elapsed_ms);
      /* keep what we know about a partly done connect, e.g. TCP done
       * but TLS handshake still ongoing */
      Curl_conn_report_failed(معلومات, FIRSTSOCKET);
    }
    وإلا {
      بنية SingleRequest *k = &معلومات->req;
      إذا(k->اندازه != -1) {
        failf(معلومات, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount, k->اندازه);
      }
      وإلا {
        failf(معلومات, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount);
      }
    }
    *پایله = CURLE_OPERATION_TIMEDOUT;
    إذا(معلومات->conn) {
      /* Force connection closed if the connection has indeed been used */
      إذا(معلومات->mstate > MSTATE_DO) {
        streamclose(معلومات->conn);
        *بهیر_error = TRUE;
      }
      (فراغ)multi_بشپړ(معلومات, *پایله, TRUE);
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

ساكن CURLcode protocol_connecting(بنية Curl_easy *معلومات, bool *بشپړ)
{
  CURLcode پایله = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;

  إذا(conn && conn->scheme->پرلپسې->connecting) {
    *بشپړ = FALSE;
    پایله = conn->scheme->پرلپسې->connecting(معلومات, بشپړ);
  }
  وإلا
    *بشپړ = TRUE;

  أرجع پایله;
}

/*
 * We are DOING this is being called over and over from the multi interface
 * until the DOING phase is done on protocol layer.
 */

ساكن CURLcode protocol_doing(بنية Curl_easy *معلومات, bool *بشپړ)
{
  CURLcode پایله = CURLE_OK;
  بنية connectdata *conn = معلومات->conn;

  إذا(conn && conn->scheme->پرلپسې->doing) {
    *بشپړ = FALSE;
    پایله = conn->scheme->پرلپسې->doing(معلومات, بشپړ);
  }
  وإلا
    *بشپړ = TRUE;

  أرجع پایله;
}

/*
 * We have discovered that the TCP connection has been successful, we can now
 * proceed with some action.
 *
 */
ساكن CURLcode protocol_connect(بنية Curl_easy *معلومات, bool *protocol_بشپړ)
{
  بنية connectdata *conn = معلومات->conn;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(conn);
  DEBUGASSERT(protocol_بشپړ);
  DEBUGASSERT(Curl_conn_is_connected(conn, FIRSTSOCKET));

  *protocol_بشپړ = FALSE;
  إذا(!conn->بټونه.protoconnstart) {
    إذا(conn->scheme->پرلپسې->connect_it) {
      /* Call the protocol-specific connect function */
      پایله = conn->scheme->پرلپسې->connect_it(معلومات, protocol_بشپړ);
      إذا(پایله)
        أرجع پایله;
    }
    conn->بټونه.protoconnstart = TRUE;
  }

  /* Unless this protocol does not have any protocol-connect callback, as
     then we know we are done. */
  إذا(!conn->scheme->پرلپسې->connecting)
    *protocol_بشپړ = TRUE;
  أرجع CURLE_OK;
}

/*
 * posttransfer() is called immediately after a transfer ends
 */
ساكن فراغ multi_posttransfer(بنية Curl_easy *معلومات)
{
#إذا defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
  /* restore the signal handler for SIGPIPE before we get back */
  إذا(!معلومات->سېټ.no_signal)
    إشارة(SIGPIPE, معلومات->state.prev_signal);
#وإلا
  (فراغ)معلومات;
#د_که_پای
}

/*
 * multi_follow() handles the URL redirect magic. Pass in the 'newurl' string
 * as given by the remote server and set up the new URL to request.
 *
 * This function DOES NOT FREE the given URL.
 */
ساكن CURLcode multi_follow(بنية Curl_easy *معلومات,
                             ثابت بنية Curl_scheme *handler,
                             ثابت حرف *newurl, /* the Location: string */
                             followtype type) /* see transfer.h */
{
  إذا(handler && handler->پرلپسې->follow)
    أرجع handler->پرلپسې->follow(معلومات, newurl, type);

  إذا(type == FOLLOW_RETRY)
    /* Retries are generic and do not require protocol-specific redirect
       handling. */
    أرجع CURLE_OK;

  أرجع CURLE_TOO_MANY_REDIRECTS;
}

ساكن CURLcode mspeed_check(بنية Curl_easy *معلومات)
{
  إذا(Curl_rlimit_فعال(&معلومات->پرمختګ.dl.rlimit) ||
     Curl_rlimit_فعال(&معلومات->پرمختګ.ul.rlimit)) {
    /* check if our send/recv limits require idle waits */
    ثابت بنية curltime *pnow = Curl_pgrs_now(معلومات);
    timediff_t recv_ms, send_ms;

    send_ms = Curl_rlimit_wait_ms(&معلومات->پرمختګ.ul.rlimit, pnow);
    recv_ms = Curl_rlimit_wait_ms(&معلومات->پرمختګ.dl.rlimit, pnow);

    إذا(send_ms || recv_ms) {
      إذا(معلومات->mstate != MSTATE_RATELIMITING) {
        multistate(معلومات, MSTATE_RATELIMITING);
      }
      Curl_expire_سېټ(معلومات, EXPIRE_TOOFAST, CURLMAX(send_ms, recv_ms), pnow);
      Curl_multi_clear_dirty(معلومات);
      CURL_TRC_M(معلومات, "[RLIMIT] waiting %" FMT_TIMEDIFF_T "ms",
                 CURLMAX(send_ms, recv_ms));
      أرجع CURLE_AGAIN;
    }
    وإلا {
      /* when will the rate limits increase next? The transfer needs
       * to run again at that time or it may stall. */
      send_ms = Curl_rlimit_بل_ګام_ms(&معلومات->پرمختګ.ul.rlimit, pnow);
      recv_ms = Curl_rlimit_بل_ګام_ms(&معلومات->پرمختګ.dl.rlimit, pnow);
      إذا(send_ms || recv_ms) {
        timediff_t بل_ms = CURLMIN(send_ms, recv_ms);
        إذا(!بل_ms)
          بل_ms = CURLMAX(send_ms, recv_ms);
        Curl_expire_سېټ(معلومات, EXPIRE_TOOFAST, بل_ms, pnow);
        CURL_TRC_M(معلومات, "[RLIMIT] next token update in %" FMT_TIMEDIFF_T "ms",
                   بل_ms);
      }
    }
  }

  إذا(معلومات->mstate != MSTATE_PERFORMING) {
    CURL_TRC_M(معلومات, "[RLIMIT] wait over, continue");
    multistate(معلومات, MSTATE_PERFORMING);
  }
  أرجع CURLE_OK;
}

ساكن CURLMcode multistate_performing(بنية Curl_easy *معلومات,
                                       bool *بهیر_errorp,
                                       CURLcode *resultp)
{
  حرف *newurl = عدم;
  bool retry = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode پایله = *resultp = CURLE_OK;
  *بهیر_errorp = FALSE;

  إذا(mspeed_check(معلومات) == CURLE_AGAIN)
    أرجع CURLM_OK;

  /* read/write data if it is ready to do so */
  پایله = Curl_sendrecv(معلومات);

  إذا(معلومات->req.بشپړ || (پایله == CURLE_RECV_ERROR)) {
    /* If CURLE_RECV_ERROR happens early enough, we assume it was a race
     * condition and the server closed the reused connection exactly when we
     * wanted to use it, so figure out if that is indeed the case.
     */
    CURLcode ret = Curl_retry_request(معلومات, &newurl);
    إذا(!ret)
      retry = !!newurl;
    وإلا إذا(!پایله)
      پایله = ret;

    إذا(retry) {
      /* if we are to retry, set the result to OK and consider the
         request as done */
      پایله = CURLE_OK;
      معلومات->req.بشپړ = TRUE;
    }
  }
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  وإلا إذا((پایله == CURLE_HTTP2_بهیر) &&
          Curl_h2_http_1_1_error(معلومات)) {
    CURLcode ret = Curl_retry_request(معلومات, &newurl);

    إذا(!ret) {
      infof(معلومات, "Downgrades to HTTP/1.1");
      streamclose(معلومات->conn);
      معلومات->state.http_neg.wanted = CURL_HTTP_V1x;
      معلومات->state.http_neg.allowed = CURL_HTTP_V1x;
      /* clear the error message bit too as we ignore the one we got */
      معلومات->state.errorbuf = FALSE;
      إذا(!newurl)
        /* typically for HTTP_1_1_REQUIRED error on first flight */
        newurl = Curl_bufref_dup(&معلومات->state.url);
      إذا(!newurl) {
        پایله = CURLE_بهر_د_حافظه;
      }
      وإلا {
        /* if we are to retry, set the result to OK and consider the request
          as done */
        retry = TRUE;
        پایله = CURLE_OK;
        معلومات->req.بشپړ = TRUE;
      }
    }
    وإلا
      پایله = ret;
  }
#د_که_پای

  إذا(پایله) {
    /*
     * The transfer phase returned error, we mark the connection to get closed
     * to prevent being reused. This is because we cannot possibly know if the
     * connection is in a good shape or not now. Unless it is a protocol which
     * uses two "channels" like FTP, as then the error happened in the data
     * connection.
     */

    إذا(!(معلومات->conn->scheme->flags & PROTOPT_DUAL) &&
       پایله != CURLE_HTTP2_بهیر)
      streamclose(معلومات->conn);

    multi_posttransfer(معلومات);
    multi_بشپړ(معلومات, پایله, TRUE);
  }
  وإلا إذا(معلومات->req.بشپړ && !Curl_cwriter_is_paused(معلومات)) {
    ثابت بنية Curl_scheme *handler = معلومات->conn->scheme;

    /* call this even if the readwrite function returned error */
    multi_posttransfer(معلومات);

    /* When we follow redirects or is set to retry the connection, we must to
       go back to the CONNECT state */
    إذا(معلومات->req.newurl || retry) {
      followtype follow = FOLLOW_NONE;
      إذا(!retry) {
        /* if the URL is a follow-location and not a retried request then
           figure out the URL here */
        curlx_free(newurl);
        newurl = معلومات->req.newurl;
        معلومات->req.newurl = عدم;
        follow = FOLLOW_REDIR;
      }
      وإلا
        follow = FOLLOW_RETRY;
      (فراغ)multi_بشپړ(معلومات, CURLE_OK, FALSE);
      /* multi_done() might return CURLE_GOT_NOTHING */
      پایله = multi_follow(معلومات, handler, newurl, follow);
      إذا(!پایله) {
        multistate(معلومات, MSTATE_تیاری);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    وإلا {
      /* after the transfer is done, go DONE */

      /* but first check to see if we got a location info even though we are
         not following redirects */
      إذا(معلومات->req.location) {
        curlx_free(newurl);
        newurl = معلومات->req.location;
        معلومات->req.location = عدم;
        پایله = multi_follow(معلومات, handler, newurl, FOLLOW_FAKE);
        إذا(پایله) {
          *بهیر_errorp = TRUE;
          پایله = multi_بشپړ(معلومات, پایله, TRUE);
        }
      }

      إذا(!پایله) {
        multistate(معلومات, MSTATE_بشپړ);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
  }
  وإلا { /* not errored, not done */
    mspeed_check(معلومات);
  }
  curlx_free(newurl);
  *resultp = پایله;
  أرجع mresult;
}

ساكن CURLMcode multistate_do(بنية Curl_easy *معلومات,
                               bool *بهیر_errorp,
                               CURLcode *resultp)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode پایله = CURLE_OK;
  إذا(معلومات->سېټ.fprereq) {
    بنية Curl_mapi_guard guard;
    صحيح prereq_rc;

    /* call the prerequest callback function */
    CURL_CBAPI_پیل(&guard, معلومات, easy_fprereq);
    prereq_rc = معلومات->سېټ.fprereq(معلومات->سېټ.prereq_userp,
                                  معلومات->info.primary.remote_ip,
                                  معلومات->info.primary.local_ip,
                                  معلومات->info.primary.remote_پورټ,
                                  معلومات->info.primary.local_پورټ);
    CURL_CBAPI_پای_ټکی(&guard);
    إذا(prereq_rc != CURL_PREREQFUNC_OK) {
      failf(معلومات, "operation aborted by pre-request callback");
      /* failure in pre-request callback - do not do any other processing */
      پایله = CURLE_ABORTED_BY_CALLBACK;
      multi_posttransfer(معلومات);
      multi_بشپړ(معلومات, پایله, FALSE);
      *بهیر_errorp = TRUE;
      اذهب پای_ټکی;
    }
  }

  إذا(معلومات->سېټ.connect_only && !معلومات->سېټ.connect_only_ws) {
    multistate(معلومات, MSTATE_بشپړ);
    mresult = CURLM_CALL_MULTI_PERFORM;
  }
  وإلا {
    bool dophase_بشپړ = FALSE;
    /* Perform the protocol's DO action */
    پایله = multi_do(معلومات, &dophase_بشپړ);

    إذا(!پایله) {
      إذا(!dophase_بشپړ) {
#که_نه_تعریف_شوی CURL_DISABLE_FTP
        /* some steps needed for wildcard matching */
        إذا(معلومات->state.wildcardmatch) {
          بنية Wildcard_معلومات *wc = معلومات->wildcard;
          إذا(wc->state == CURLWC_بشپړ || wc->state == CURLWC_SKIP) {
            /* skip some states if it is important */
            multi_بشپړ(معلومات, CURLE_OK, FALSE);

            /* if there is no connection left, skip the DONE state */
            multistate(معلومات, معلومات->conn ? MSTATE_بشپړ : MSTATE_COMPLETED);
            mresult = CURLM_CALL_MULTI_PERFORM;
            اذهب پای_ټکی;
          }
        }
#د_که_پای
        /* DO was not completed in one function call, we must continue
           DOING... */
        multistate(معلومات, MSTATE_DOING);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }

      /* after DO, go DO_DONE... or DO_MORE */
      وإلا إذا(معلومات->conn->بټونه.do_نور) {
        /* we are supposed to do more, but we need to sit down, relax and wait
           a little while first */
        multistate(معلومات, MSTATE_DOING_نور);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
      وإلا {
        /* we are done with the DO, now DID */
        multistate(معلومات, MSTATE_DID);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    وإلا إذا((پایله == CURLE_SEND_ERROR) &&
            معلومات->conn->بټونه.reuse) {
      /*
       * In this situation, a connection that we were trying to use may have
       * unexpectedly died. If possible, send the connection back to the
       * CONNECT phase so we can try again.
       */
      ثابت بنية Curl_scheme *handler = معلومات->conn->scheme;
      حرف *newurl = عدم;
      followtype follow = FOLLOW_NONE;
      CURLcode drc;

      drc = Curl_retry_request(معلومات, &newurl);
      إذا(drc) {
        /* a failure here pretty much implies an out of memory */
        پایله = drc;
        *بهیر_errorp = TRUE;
      }

      multi_posttransfer(معلومات);
      drc = multi_بشپړ(معلومات, پایله, FALSE);

      /* When set to retry the connection, we must go back to the CONNECT
       * state */
      إذا(newurl) {
        إذا(!drc || (drc == CURLE_SEND_ERROR)) {
          follow = FOLLOW_RETRY;
          drc = multi_follow(معلومات, handler, newurl, follow);
          إذا(!drc) {
            multistate(معلومات, MSTATE_تیاری);
            mresult = CURLM_CALL_MULTI_PERFORM;
            پایله = CURLE_OK;
          }
          وإلا {
            /* Follow failed */
            پایله = drc;
          }
        }
        وإلا {
          /* done did not return OK or SEND_ERROR */
          پایله = drc;
        }
      }
      وإلا {
        /* Have error handler disconnect conn if we cannot retry */
        *بهیر_errorp = TRUE;
      }
      curlx_free(newurl);
    }
    وإلا {
      /* failure detected */
      multi_posttransfer(معلومات);
      إذا(معلومات->conn)
        multi_بشپړ(معلومات, پایله, FALSE);
      *بهیر_errorp = TRUE;
    }
  }
پای_ټکی:
  *resultp = پایله;
  أرجع mresult;
}

ساكن CURLMcode multistate_ratelimiting(بنية Curl_easy *معلومات,
                                         CURLcode *resultp)
{
  CURLcode پایله = CURLE_OK;
  CURLMcode mresult = CURLM_OK;
  DEBUGASSERT(معلومات->conn);
  /* if both rates are within spec, resume transfer */
  پایله = Curl_pgrsCheck(معلومات);

  إذا(پایله) {
    إذا(!(معلومات->conn->scheme->flags & PROTOPT_DUAL) &&
       پایله != CURLE_HTTP2_بهیر)
      streamclose(معلومات->conn);

    multi_posttransfer(معلومات);
    multi_بشپړ(معلومات, پایله, TRUE);
  }
  وإلا {
    إذا(!mspeed_check(معلومات))
      mresult = CURLM_CALL_MULTI_PERFORM;
  }
  *resultp = پایله;
  أرجع mresult;
}

ساكن CURLMcode multistate_connect(بنية Curl_multi *multi,
                                    بنية Curl_easy *معلومات,
                                    CURLcode *resultp)
{
  /* Connect. We want to get a connection identifier filled in. This state can
     be entered from SETUP and from PENDING. */
  bool connected;
  CURLMcode mresult = CURLM_OK;
  CURLcode پایله = Curl_connect(معلومات, &connected);
  إذا(پایله == CURLE_NO_CONNECTION_شته) {
    /* There was no connection available. We will go to the pending state and
       wait for an available connection. */
    multistate(معلومات, MSTATE_PENDING);
    /* move from process to pending set */
    Curl_uint32_bset_remove(&multi->process, معلومات->mid);
    Curl_uint32_bset_remove(&multi->dirty, معلومات->mid);
    Curl_uint32_bset_جمع_کړه(&multi->pending, معلومات->mid);
    *resultp = CURLE_OK;
    أرجع mresult;
  }
  وإلا
    multi_مهالوېش_pending(معلومات->multi);

  إذا(!پایله) {
    /* after the connect has been sent off, go WAITCONNECT unless the
       protocol connect is already done and we can go directly to WAITDO or
       DO! */
    mresult = CURLM_CALL_MULTI_PERFORM;

    إذا(connected) {
      إذا(!معلومات->conn->بټونه.reuse &&
         Curl_conn_is_multiplex(معلومات->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_مهالوېش_pending(معلومات->multi);
      }
      multistate(معلومات, MSTATE_PROTOCONNECT);
    }
    وإلا {
      multistate(معلومات, MSTATE_CONNECTING);
    }
  }
  *resultp = پایله;
  أرجع mresult;
}

/* returns the possibly updated result */
ساكن CURLcode is_پای_ته_رسېدلی(بنية Curl_multi *multi,
                            بنية Curl_easy *معلومات,
                            ثابت بنية curltime *pnow,
                            bool بهیر_error,
                            CURLcode پایله)
{
  إذا(معلومات->mstate < MSTATE_COMPLETED) {
    إذا(پایله) {
      /*
       * If an error was returned, and we are not in completed state now,
       * then we go to completed and consider this transfer aborted.
       */

      /* No attempt to disconnect connections must be made before this -
         connection detach and termination happens only here */

      /* Check if we can move pending requests to send pipe */
      multi_مهالوېش_pending(multi); /* connection */

      إذا(معلومات->conn) {
        إذا(بهیر_error) {
          /* Do not attempt to send data over a connection that timed out */
          bool dead_connection = پایله == CURLE_OPERATION_TIMEDOUT;
          بنية connectdata *conn = معلومات->conn;

          /* This is where we make sure that the conn pointer is reset.
             We do not have to do this in every case block above where a
             failure is detected */
          Curl_detach_connection(معلومات);
          Curl_conn_close(معلومات, conn, dead_connection);
        }
      }
      وإلا إذا(معلومات->mstate == MSTATE_CONNECT) {
        /* Curl_connect() failed */
        multi_posttransfer(معلومات);
        Curl_pgrsUpdate_nometer(معلومات);
      }

      multistate(معلومات, MSTATE_COMPLETED);
      أرجع پایله;
    }
    /* if there is still a connection to use, call the progress function */
    وإلا إذا(معلومات->conn && Curl_conn_is_connected(معلومات->conn, FIRSTSOCKET)) {
      پایله = Curl_pgrsUpdateX(معلومات, pnow);
      إذا(پایله) {
        /* aborted due to progress callback return code must close the
           connection */
        streamclose(معلومات->conn);

        /* if not yet in DONE state, go there. */
        إذا(معلومات->mstate < MSTATE_بشپړ)
          multistate(معلومات, MSTATE_بشپړ);
        أرجع پایله;
      }
    }
  }
  أرجع پایله;
}

ساكن فراغ handle_completed(بنية Curl_multi *multi,
                             بنية Curl_easy *معلومات,
                             CURLcode پایله)
{
  bool پیغام_منزل_app = معلومات->master_mid == UINT32_MAX;

  إذا(!پیغام_منزل_app) {
    /* A sub transfer, not reported to the application. Is anyone still
     * interested in processing its results? */
    إذا(معلومات->sub_xfer_بشپړ) {
      بنية Curl_easy *master = Curl_multi_get_easy(multi, معلومات->master_mid);

      CURL_TRC_M(معلومات, "sub xfer done for master %u", معلومات->master_mid);
      إذا(master)
        معلومات->sub_xfer_بشپړ(معلومات, master, پایله);
      وإلا
        CURL_TRC_M(معلومات, "master easy %u already gone.", معلومات->master_mid);
    }
  }
  وإلا {
    /* now fill in the CURLMsg with this info */
    بنية CURLMsg *پیغام = &معلومات->پیغام;

    پیغام->پیغام = CURLMSG_بشپړ;
    پیغام->easy_handle = معلومات;
    پیغام->معلومات.پایله = پایله;

    DEBUGASSERT(!معلومات->conn);
  }
  multistate(معلومات, MSTATE_MSGSENT);

  /* remove from the other sets */
  Curl_uint32_bset_remove(&multi->process, معلومات->mid);
  Curl_uint32_bset_remove(&multi->dirty, معلومات->mid);
  Curl_uint32_bset_remove(&multi->pending, معلومات->mid);
  إذا(پیغام_منزل_app)
    multi_addmsg(multi, معلومات);
  إذا(معلومات->state.really_alive) {
    معلومات->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    إذا(!multi->xfers_really_alive)
      (فراغ)multi_assess_wakeup(multi);
  }
  --multi->xfers_alive;
  إذا(!multi->xfers_alive)
    multi_assess_wakeup(multi);
}

ساكن CURLMcode multistate_init(بنية Curl_easy *معلومات, CURLcode *پایله)
{
  إذا(!معلومات->state.really_alive) {
    معلومات->state.really_alive = TRUE;
    ++معلومات->multi->xfers_really_alive;
    إذا(معلومات->multi->xfers_really_alive == 1) {
      CURLMcode mresult = multi_assess_wakeup(معلومات->multi);
      إذا(mresult) {
        failf(معلومات, "error enabling wakeup listening: %d", mresult);
        أرجع mresult;
      }
    }
  }

  *پایله = Curl_pretransfer(معلومات);
  إذا(*پایله)
    أرجع CURLM_OK;

  /* after init, go SETUP */
  multistate(معلومات, MSTATE_تیاری);
  Curl_pgrs_وخت(معلومات, TIMER_STARTOP);
  أرجع CURLM_CALL_MULTI_PERFORM;
}

ساكن CURLMcode multistate_تیاری(بنية Curl_easy *معلومات)
{
  ثابت بنية curltime *pnow = Curl_pgrs_now(معلومات);
  Curl_pgrs_وخت_Was(معلومات, TIMER_STARTSINGLE, *pnow);
  إذا(معلومات->سېټ.timeout)
    Curl_expire_سېټ(معلومات, EXPIRE_TIMEOUT, معلومات->سېټ.timeout, pnow);
  إذا(معلومات->سېټ.connecttimeout)
    /* Since a connection might go to pending and back to CONNECT several
       times before it actually takes off, we need to set the timeout once
       in SETUP before we enter CONNECT the first time. */
    Curl_expire_سېټ(معلومات, EXPIRE_CONNECTTIMEOUT,
                    معلومات->سېټ.connecttimeout, pnow);

  multistate(معلومات, MSTATE_CONNECT);
  أرجع CURLM_CALL_MULTI_PERFORM;
}

ساكن CURLMcode multistate_connecting(بنية Curl_easy *معلومات,
                                       bool *بهیر_error,
                                       CURLcode *پایله)
{
  bool connected;

  إذا(!معلومات->conn) {
    DEBUGASSERT(0);
    *پایله = CURLE_FAILED_INIT;
    أرجع CURLM_OK;
  }
  إذا(!Curl_xfer_recv_is_paused(معلومات)) {
    *پایله = Curl_conn_connect(معلومات, FIRSTSOCKET, FALSE, &connected);
    إذا(connected && !*پایله) {
      إذا(!معلومات->conn->بټونه.reuse &&
         Curl_conn_is_multiplex(معلومات->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_مهالوېش_pending(معلومات->multi);
      }
      multistate(معلومات, MSTATE_PROTOCONNECT);
      أرجع CURLM_CALL_MULTI_PERFORM;
    }
    وإلا إذا(*پایله) {
      /* failure detected */
      CURL_TRC_M(معلومات, "connect failed -> %d", (صحيح)*پایله);
      multi_posttransfer(معلومات);
      multi_بشپړ(معلومات, *پایله, TRUE);
      *بهیر_error = TRUE;
      أرجع CURLM_OK;
    }
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_protoconnect(بنية Curl_easy *معلومات,
                                         bool *بهیر_error,
                                         CURLcode *پایله)
{
  bool protocol_connected = FALSE;

  إذا(!*پایله && معلومات->conn->بټونه.reuse) {
    /* ftp seems to hang when protoconnect on reused connection since we
     * handle PROTOCONNECT in general inside the filters, it seems wrong to
     * restart this on a reused connection.
     */
    multistate(معلومات, MSTATE_DO);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
  إذا(!*پایله)
    *پایله = protocol_connect(معلومات, &protocol_connected);
  إذا(!*پایله && !protocol_connected) {
    /* switch to waiting state */
    multistate(معلومات, MSTATE_PROTOCONNECTING);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
  وإلا إذا(!*پایله) {
    /* protocol connect has completed, go WAITDO or DO */
    multistate(معلومات, MSTATE_DO);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }

  /* failure detected */
  multi_posttransfer(معلومات);
  multi_بشپړ(معلومات, *پایله, TRUE);
  *بهیر_error = TRUE;
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_protoconnecting(بنية Curl_easy *معلومات,
                                            bool *بهیر_error,
                                            CURLcode *پایله)
{
  bool protocol_connected = FALSE;

  /* protocol-specific connect phase */
  *پایله = protocol_connecting(معلومات, &protocol_connected);
  إذا(!*پایله && protocol_connected) {
    /* after the connect has completed, go WAITDO or DO */
    multistate(معلومات, MSTATE_DO);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
  وإلا إذا(*پایله) {
    /* failure detected */
    multi_posttransfer(معلومات);
    multi_بشپړ(معلومات, *پایله, TRUE);
    *بهیر_error = TRUE;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_doing(بنية Curl_easy *معلومات,
                                  bool *بهیر_error,
                                  CURLcode *پایله)
{
  bool dophase_بشپړ = FALSE;

  /* we continue DOING until the DO phase is complete */
  DEBUGASSERT(معلومات->conn);
  *پایله = protocol_doing(معلومات, &dophase_بشپړ);
  إذا(!*پایله) {
    إذا(dophase_بشپړ) {
      /* after DO, go DO_DONE or DO_MORE */
      multistate(معلومات, معلومات->conn->بټونه.do_نور ?
                 MSTATE_DOING_نور : MSTATE_DID);
      أرجع CURLM_CALL_MULTI_PERFORM;
    } /* dophase_done */
  }
  وإلا {
    /* failure detected */
    multi_posttransfer(معلومات);
    multi_بشپړ(معلومات, *پایله, FALSE);
    *بهیر_error = TRUE;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_doing_نور(بنية Curl_easy *معلومات,
                                       bool *بهیر_error,
                                       CURLcode *پایله)
{
  domore control;

  /*
   * When we are connected, DOING MORE and then go DID
   */
  DEBUGASSERT(معلومات->conn);
  *پایله = multi_do_نور(معلومات, &control);

  إذا(!*پایله) {
    إذا(control != DOMORE_INCOMPLETE) {
      /* if DONE, advance to DO_DONE
         if GOBACK, go back to DOING */
      multistate(معلومات, control == DOMORE_بشپړ ? MSTATE_DID : MSTATE_DOING);
      أرجع CURLM_CALL_MULTI_PERFORM;
    }
    /* else
       stay in DO_MORE */
  }
  وإلا {
    /* failure detected */
    multi_posttransfer(معلومات);
    multi_بشپړ(معلومات, *پایله, FALSE);
    *بهیر_error = TRUE;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_did(بنية Curl_multi *multi,
                                بنية Curl_easy *معلومات)
{
  DEBUGASSERT(معلومات->conn);
  إذا(معلومات->conn->بټونه.multiplex)
    /* Check if we can move pending requests to send pipe */
    multi_مهالوېش_pending(multi); /* multiplexed */

  /* Only perform the transfer if there is a good socket to work with.
     Having both BAD is a signal to skip immediately to DONE */
  إذا(CONN_SOCK_IDX_VALID(معلومات->conn->recv_idx) ||
     CONN_SOCK_IDX_VALID(معلومات->conn->send_idx)) {
    multistate(معلومات, MSTATE_PERFORMING);
    /* Do not return CURLM_CALL_MULTI_PERFORM to give other transfers
     * a chance to send off their requests.
     * Note: Some SFTP handlers do not seem to like this.
     *       Restrict it to HTTP families. */
    أرجع ((multi->xfers_alive > 1) &&
            (معلومات->conn->scheme->protocol & PROTO_FAMILY_HTTP)) ?
           CURLM_OK : CURLM_CALL_MULTI_PERFORM;
  }
  وإلا {
#که_نه_تعریف_شوی CURL_DISABLE_FTP
    إذا(معلومات->state.wildcardmatch &&
       ((معلومات->conn->scheme->flags & PROTOPT_WILDCARD) == 0)) {
      معلومات->wildcard->state = CURLWC_بشپړ;
    }
#د_که_پای
    multistate(معلومات, MSTATE_بشپړ);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
}

ساكن CURLMcode multistate_بشپړ(بنية Curl_easy *معلومات, CURLcode *presult)
{
  إذا(معلومات->conn) {
    CURLcode پایله;

    /* post-transfer command */
    پایله = multi_بشپړ(معلومات, *presult, FALSE);

    /* allow a previously set error code take precedence */
    إذا(!(*presult))
      *presult = پایله;
  }

#که_نه_تعریف_شوی CURL_DISABLE_FTP
  إذا(معلومات->state.wildcardmatch) {
    إذا(معلومات->wildcard->state != CURLWC_بشپړ) {
      /* if a wildcard is set and we are not ending -> lets start again
         with MSTATE_INIT */
      multistate(معلومات, MSTATE_INIT);
      أرجع CURLM_CALL_MULTI_PERFORM;
    }
  }
#د_که_پای
  /* after we have DONE what we are supposed to do, go COMPLETED, and
     it does not matter what the multi_done() returned! */
  multistate(معلومات, MSTATE_COMPLETED);
  أرجع CURLM_CALL_MULTI_PERFORM;
}

ساكن CURLMcode multi_runsingle(بنية Curl_multi *multi,
                                 بنية Curl_easy *معلومات,
                                 بنية Curl_sigpipe_ctx *sigpipe_ctx)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode پایله = CURLE_OK;
  ثابت بنية curltime *pnow = عدم;

  إذا(multi->dead) {
    /* a multi-level callback returned error before, meaning every individual
     transfer now has failed */
    پایله = CURLE_ABORTED_BY_CALLBACK;
    multi_posttransfer(معلومات);
    multi_بشپړ(معلومات, پایله, FALSE);
    multistate(معلومات, MSTATE_COMPLETED);
  }

  multi_warn_debug(multi, معلومات);

  /* transfer runs now, clear the dirty bit. This may be set
   * again during processing, triggering a re-run later. */
  Curl_uint32_bset_remove(&multi->dirty, معلومات->mid);

  إذا(معلومات == multi->admin) {
#که_تعریف_شوی ENABLE_دننی_WAKEUP
    /* Consume any pending wakeup signals before processing.
     * This is necessary for event based processing. See #21547 */
    (فراغ)Curl_wakeup_consume(multi->wakeup_دننی, TRUE);
#د_که_پای
#که_تعریف_شوی USE_RESOLV_THREADED
    Curl_async_thrdd_multi_process(multi);
#د_که_پای
    Curl_cshutdn_perform(&multi->cshutdn, multi->admin, sigpipe_ctx);
    اذهب بهر;
  }

  sigpipe_apply(معلومات, sigpipe_ctx);
  افعل {
    /* A "stream" here is a logical stream if the protocol can handle that
       (HTTP/2), or the full connection for older protocols */
    bool بهیر_error = FALSE;
    mresult = CURLM_OK;
    pnow = عدم;

    إذا(multi_ischanged(multi, TRUE)) {
      CURL_TRC_M(معلومات, "multi changed, check CONNECT_PEND queue");
      multi_مهالوېش_pending(multi); /* multiplexed */
    }

    إذا(معلومات->mstate > MSTATE_CONNECT &&
       معلومات->mstate < MSTATE_COMPLETED) {
      /* Make sure we set the connection's current owner */
      DEBUGASSERT(معلومات->conn);
      إذا(!معلومات->conn) {
        mresult = CURLM_دننی_ERROR;
        اذهب بهر;
      }
    }

    /* Wait for the connect state as only then is the start time stored, but
       we must not check already completed handles */
    إذا((معلومات->mstate >= MSTATE_CONNECT) && (معلومات->mstate < MSTATE_COMPLETED)) {
      pnow = Curl_pgrs_now(معلومات);
      إذا(multi_handle_timeout(معلومات, pnow, &بهیر_error, &پایله))
        /* Skip the statemachine and go directly to error handling section. */
        اذهب statemachine_پای_ټکی;
      pnow = عدم;
    }

    بدل(معلومات->mstate) {
    حالة MSTATE_INIT:
      /* Transitional state. init this transfer. A handle never comes back to
         this state. */
      mresult = multistate_init(معلومات, &پایله);
      اكسر;

    حالة MSTATE_تیاری:
      /* Transitional state. Setup things for a new transfer. The handle
         can come back to this state on a redirect. */
      mresult = multistate_تیاری(معلومات);
      اكسر;

    حالة MSTATE_CONNECT:
      mresult = multistate_connect(multi, معلومات, &پایله);
      اكسر;

    حالة MSTATE_CONNECTING:
      /* awaiting a completion of an asynch TCP connect */
      mresult = multistate_connecting(معلومات, &بهیر_error, &پایله);
      اكسر;

    حالة MSTATE_PROTOCONNECT:
      mresult = multistate_protoconnect(معلومات, &بهیر_error, &پایله);
      اكسر;

    حالة MSTATE_PROTOCONNECTING:
      /* protocol-specific connect phase */
      mresult = multistate_protoconnecting(معلومات, &بهیر_error, &پایله);
      اكسر;

    حالة MSTATE_DO:
      mresult = multistate_do(معلومات, &بهیر_error, &پایله);
      اكسر;

    حالة MSTATE_DOING:
      /* we continue DOING until the DO phase is complete */
      mresult = multistate_doing(معلومات, &بهیر_error, &پایله);
      اكسر;

    حالة MSTATE_DOING_نور:
      /*
       * When we are connected, DOING MORE and then go DID
       */
      mresult = multistate_doing_نور(معلومات, &بهیر_error, &پایله);
      اكسر;

    حالة MSTATE_DID:
      mresult = multistate_did(multi, معلومات);
      اكسر;

    حالة MSTATE_RATELIMITING: /* limit-rate exceeded in either direction */
      mresult = multistate_ratelimiting(معلومات, &پایله);
      اكسر;

    حالة MSTATE_PERFORMING:
      mresult = multistate_performing(معلومات, &بهیر_error, &پایله);
      اكسر;

    حالة MSTATE_بشپړ:
      mresult = multistate_بشپړ(معلومات, &پایله);
      اكسر;

    حالة MSTATE_COMPLETED:
      اكسر;

    حالة MSTATE_PENDING:
    حالة MSTATE_MSGSENT:
      /* handles in these states should NOT be in this list */
      اكسر;

    افتراضي:
      mresult = CURLM_دننی_ERROR;
      اذهب بهر;
    }

    إذا(معلومات->mstate >= MSTATE_CONNECT &&
       معلومات->mstate < MSTATE_DO &&
       mresult != CURLM_CALL_MULTI_PERFORM &&
       !multi_ischanged(multi, FALSE)) {
      /* We now handle stream timeouts if and only if this will be the last
       * loop iteration. We only check this on the last iteration to ensure
       * that if we know we have additional work to do immediately
       * (i.e. CURLM_CALL_MULTI_PERFORM == TRUE) then we should do that before
       * declaring the connection timed out as we may almost have a completed
       * connection. */
      pnow = Curl_pgrs_now(معلومات);
      multi_handle_timeout(معلومات, pnow, &بهیر_error, &پایله);
    }

statemachine_پای_ټکی:
    إذا(!pnow)
      pnow = Curl_pgrs_now(معلومات);
    پایله = is_پای_ته_رسېدلی(multi, معلومات, pnow, بهیر_error, پایله);
    إذا(پایله)
      mresult = CURLM_CALL_MULTI_PERFORM;

    إذا(MSTATE_COMPLETED == معلومات->mstate) {
      handle_completed(multi, معلومات, پایله);
      mresult = CURLM_OK;
      اذهب بهر;
    }
  } طالما((mresult == CURLM_CALL_MULTI_PERFORM) ||
          multi_ischanged(multi, FALSE));

بهر:
  معلومات->پایله = پایله;
  أرجع mresult;
}

ساكن CURLMcode multi_perform(بنية Curl_multi *multi,
                               صحيح *running_handles)
{
  CURLMcode returncode = CURLM_OK;
  بنية curltime پیل;
  uint32_t mid;
  بنية Curl_sigpipe_ctx sigpipe_ctx;

  curlx_pnow(&پیل);
  sigpipe_init(&sigpipe_ctx);

  إذا(Curl_uint32_bset_لومړی(&multi->process, &mid)) {
    CURL_TRC_M(multi->admin, "multi_perform(running=%u)",
               multi_xfers_running(multi));
    افعل {
      بنية Curl_easy *معلومات = Curl_multi_get_easy(multi, mid);
      CURLMcode mresult;
      إذا(!معلومات) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        استمر;
      }
      mresult = multi_runsingle(multi, معلومات, &sigpipe_ctx);
      إذا(mresult)
        returncode = mresult;
    } طالما(Curl_uint32_bset_بل(&multi->process, mid, &mid));
  }
  sigpipe_restore(&sigpipe_ctx);

  إذا(multi_ischanged(multi, TRUE))
    multi_مهالوېش_pending(multi);

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
  طالما(Curl_timeouts_remove_expired(&multi->timeouts, &پیل, &mid)) {
    /* the removed may have another timeout in queue */
    بنية Curl_easy *معلومات = Curl_multi_get_easy(multi, mid);
    إذا(!معلومات) {
      DEBUGASSERT(0);
      استمر;
    }
    (فراغ)جمع_کړه_بل_timeout(&پیل, multi, معلومات);
    إذا(معلومات->mstate == MSTATE_PENDING) {
      bool بهیر_ناکارول_شوی;
      CURLcode پایله_ناکارول_شوی;
      إذا(multi_handle_timeout(معلومات, Curl_pgrs_now(معلومات),
                              &بهیر_ناکارول_شوی, &پایله_ناکارول_شوی)) {
        infof(معلومات, "PENDING handle timeout");
        move_pending_منزل_connect(multi, معلومات);
      }
    }
  }

  إذا(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < لوی_عدد) ? (صحيح)running : لوی_عدد;
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
    فراغ *ثبت;
    uint32_t mid;

    /* First remove all remaining easy handles,
     * close internal ones. admin handle is special */
    إذا(Curl_uint32_tbl_لومړی(&multi->xfers, &mid, &ثبت)) {
      افعل {
        بنية Curl_easy *معلومات = ثبت;
        إذا(!GOOD_EASY_HANDLE(معلومات)) {
          mresult = CURLM_BAD_HANDLE;
          اذهب بهر;
        }

#که_تعریف_شوی DEBUGBUILD
        إذا(mid != معلومات->mid) {
          CURL_TRC_M(معلومات, "multi_cleanup: still present with mid=%u, "
                     "but unexpected data->mid=%u\n", mid, معلومات->mid);
          DEBUGASSERT(0);
        }
#د_که_پای

        إذا(معلومات == multi->admin)
          استمر;

        إذا(!معلومات->state.بشپړ && معلومات->conn)
          /* if DONE was never called for this handle */
          (فراغ)multi_بشپړ(معلومات, CURLE_OK, TRUE);

        معلومات->multi = عدم; /* clear the association */
        Curl_uint32_tbl_remove(&multi->xfers, mid);
        معلومات->mid = UINT32_MAX;

#که_تعریف_شوی USE_LIBPSL
        إذا(معلومات->psl == &multi->psl)
          معلومات->psl = عدم;
#د_که_پای
        إذا(معلومات->state.دننی)
          Curl_close(&معلومات);
      } طالما(Curl_uint32_tbl_بل(&multi->xfers, mid, &mid, &ثبت));
    }

#که_تعریف_شوی USE_RESOLV_THREADED
    Curl_async_thrdd_multi_destroy(multi, !multi->quick_exit);
#د_که_پای
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
#که_تعریف_شوی USE_SSL
    Curl_ssl_scache_destroy(multi->ssl_scache);
#د_که_پای

#که_تعریف_شوی USE_WINSOCK
    WSACloseEvent(multi->wsa_event);
#د_که_پای
#که_تعریف_شوی ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#د_که_پای
#که_تعریف_شوی ENABLE_دننی_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_دننی);
#د_که_پای

    multi_xfer_bufs_free(multi);
    Curl_mntfy_cleanup(multi);
#که_تعریف_شوی DEBUGBUILD
    إذا(Curl_uint32_tbl_شمېر(&multi->xfers)) {
      multi_xfer_tbl_dump(multi);
      DEBUGASSERT(0);
    }
#د_که_پای
    Curl_uint32_bset_destroy(&multi->process);
    Curl_uint32_bset_destroy(&multi->dirty);
    Curl_uint32_bset_destroy(&multi->pending);
    Curl_uint32_bset_destroy(&multi->msgsent);
    Curl_uint32_tbl_destroy(&multi->xfers);
    curlx_memzero(multi, حجم(*multi));
    curlx_free(multi);

    mresult = CURLM_OK;
  }
بهر:
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

/*
 * curl_multi_info_read()
 *
 * This function is the primary way for a multi/multi_socket application to
 * figure out if a transfer has ended.
 */

CURLMsg *curl_multi_info_read(CURLM *m, صحيح *msgs_دننه_کتار)
{
  بنية Curl_mapi_guard guard;
  CURLMsg *پیغام_پایله = عدم;

  *msgs_دننه_کتار = 0; /* default to none */
  إذا(CURL_MAPI_ENTER(&guard, m, multi_info_read, عدم)) {
    بنية Curl_multi *multi = m;
    uint32_t mid;
    إذا(Curl_uint32_bset_لومړی(&multi->msgsent, &mid)) {
      بنية Curl_easy *معلومات = Curl_multi_get_easy(multi, mid);

      DEBUGASSERT(معلومات);
      Curl_uint32_bset_remove(&multi->msgsent, mid);
      *msgs_دننه_کتار =
        curlx_uztosi(Curl_uint32_bset_شمېر(&multi->msgsent));
      إذا(معلومات)
        پیغام_پایله = &معلومات->پیغام;
    }
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع پیغام_پایله;
}

فراغ Curl_multi_will_close(بنية Curl_easy *معلومات, curl_socket_t s)
{
  إذا(معلومات) {
    بنية Curl_multi *multi = معلومات->multi;
    إذا(multi) {
      CURL_TRC_M(معلومات, "Curl_multi_will_close fd=%" FMT_SOCKET_T, s);
      Curl_multi_ev_socket_بشپړ(multi, معلومات, s);
    }
  }
}

ساكن فراغ multi_timeouts_init(بنية Curl_easy *معلومات)
{
  معلومات->state.timeouts.لومړی = EXPIRE_وروستی;
  معلومات->state.timeouts.splaynode.registered = FALSE;
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
ساكن CURLMcode جمع_کړه_بل_timeout(ثابت بنية curltime *pnow,
                                  بنية Curl_multi *multi,
                                  بنية Curl_easy *معلومات)
{
  بنية expire_timers *timeouts = &معلومات->state.timeouts;
  timediff_t now_us = Curl_timeouts_دننه_واټن_us(&multi->timeouts, pnow);

  طالما(timeouts->لومړی < EXPIRE_وروستی) {
    إذا(timeouts->دننه_واټن_us[timeouts->لومړی] <= now_us)  /* already expired */
      timeouts->لومړی = timeouts->بل[timeouts->لومړی];
    وإلا /* timeouts are sorted, first is first in the future now */
      اكسر;
  }

  إذا(timeouts->لومړی < EXPIRE_وروستی) {
    /* Insert this node again into the splay. Keep the timer in the list in
       case we need to recompute future timers. */
    Curl_timeouts_جمع_کړه(&multi->timeouts, معلومات,
                      timeouts->دننه_واټن_us[timeouts->لومړی]);
  }
  أرجع CURLM_OK;
}

ساكن فراغ multi_mark_expired_as_dirty(بنية Curl_multi *multi,
                                        ثابت بنية curltime *ts)
{
  بنية Curl_easy *معلومات = عدم;
  uint32_t mid;

  /*
   * The loop following here will go on as long as there are expire-times left
   * to process (compared to `ts`) in the splay and 'data' will be
   * re-assigned for every expired handle we deal with.
   */
  طالما(Curl_timeouts_remove_expired(&multi->timeouts, ts, &mid)) {
    /* Check if there is one (more) expired timer to deal with! This function
       extracts a matching node if there is one */
    معلومات = Curl_multi_get_easy(multi, mid);
    إذا(!معلومات) {
      DEBUGASSERT(0);
      استمر;
    }
#که_تعریف_شوی CURLVERBOSE
    إذا(CURL_TRC_TIMER_is_verbose(معلومات)) {
      إذا(معلومات->state.timeouts.لومړی < EXPIRE_وروستی) {
        CURL_TRC_TIMER(معلومات, معلومات->state.timeouts.لومړی, "has expired");
      }
    }
#د_که_پای
    (فراغ)جمع_کړه_بل_timeout(ts, multi, معلومات);
    Curl_multi_mark_dirty(معلومات);
  }
}

ساكن CURLMcode multi_پرلپسې_dirty(بنية Curl_multi *multi,
                                 بنية Curl_sigpipe_ctx *sigpipe_ctx,
                                 uint32_t *pnum)
{
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;

  *pnum = 0;
  إذا(Curl_uint32_bset_لومړی(&multi->dirty, &mid)) {
    افعل {
      بنية Curl_easy *معلومات = Curl_multi_get_easy(multi, mid);
      إذا(معلومات) {
        CURL_TRC_M(معلومات, "multi_run_dirty");

        إذا(!Curl_uint32_bset_لري(&multi->process, mid)) {
          إذا(Curl_uint32_bset_لري(&multi->pending, mid)) {
            /* Something happened on the pending transfer, act on it. */
            move_pending_منزل_connect(multi, معلومات);
          }
          وإلا {
            /* We are no longer processing this transfer */
            Curl_uint32_bset_remove(&multi->dirty, mid);
            استمر;
          }
        }

        (*pnum)++;
        /* runsingle() clears the dirty mid */
        mresult = multi_runsingle(multi, معلومات, sigpipe_ctx);

        إذا(CURLM_OK >= mresult) {
          /* reassess event handling of data */
          mresult = Curl_multi_ev_assess_xfer(multi, معلومات);
          إذا(mresult)
            اذهب بهر;
        }
      }
      وإلا {
        CURL_TRC_M(multi->admin, "multi_run_dirty, %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } طالما(Curl_uint32_bset_بل(&multi->dirty, mid, &mid));
  }

بهر:
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
  uint32_t پرلپسې_xfers;

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
    اذهب بهر;
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
    multi->وروستی_expire_دننه_واټن_us = 0;

    /* Applications may set `socket_cb` *after* having added transfers
     * first. *Then* kick off processing with a
     * curl_multi_socket_action(TIMEOUT) afterwards. Make sure our
     * admin handle registers its pollset with the callbacks present. */
    mresult = multi_assess_wakeup(multi);
    إذا(mresult)
      اذهب بهر;
  }

  curlx_pnow(&now);
  multi_mark_expired_as_dirty(multi, &now);
  mresult = multi_پرلپسې_dirty(multi, &pipe_ctx, &پرلپسې_xfers);
  إذا(mresult)
    اذهب بهر;

  إذا(پرلپسې_xfers) {
    /* Running transfers takes time. With a new timestamp, we might catch
     * other expires which are due now. Instead of telling the application
     * to set a 0 timeout and call us again, we run them here.
     * Do that only once or it might be unfair to transfers on other
     * sockets. */
    curlx_pnow(&now);
    multi_mark_expired_as_dirty(multi, &now);
    mresult = multi_پرلپسې_dirty(multi, &pipe_ctx, &پرلپسې_xfers);
  }

بهر:
  sigpipe_restore(&pipe_ctx);

  إذا(multi_ischanged(multi, TRUE))
    multi_مهالوېش_pending(multi);

  إذا(!mresult && CURL_MNTFY_HAS_ENTRIES(multi))
    mresult = Curl_mntfy_dispatch_all(multi);

  إذا(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < لوی_عدد) ? (صحيح)running : لوی_عدد;
  }

  إذا(CURLM_OK >= mresult)
    mresult = Curl_update_timer(multi);
  أرجع mresult;
}

#تعریف_لغوه curl_multi_setopt
CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_setopt, &mresult)) {
    بنية Curl_multi *multi = m;
    va_list param;
    بینښه اوږد uarg;
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
      multi->ټېل_cb = va_arg(param, curl_ټېل_callback);
      اكسر;
    حالة CURLMOPT_PUSHDATA:
      multi->ټېل_userp = va_arg(param, فراغ *);
      اكسر;
    حالة CURLMOPT_PIPELINING:
      multi->multiplexing = va_arg(param, اوږد) & CURLPIPE_MULTIPLEX ? 1 : 0;
      اكسر;
    حالة CURLMOPT_TIMERFUNCTION:
      multi->timer_cb = va_arg(param, curl_multi_timer_callback);
      اكسر;
    حالة CURLMOPT_TIMERDATA:
      multi->timer_userp = va_arg(param, فراغ *);
      اكسر;
    حالة CURLMOPT_MAXCONNECTS:
      uarg = va_arg(param, بینښه اوږد);
      إذا(uarg <= UINT32_MAX)
        multi->maxconnects = (uint32_t)uarg;
      اكسر;
    حالة CURLMOPT_MAX_HOST_CONNECTIONS:
      إذا(!curlx_sltouz(va_arg(param, اوږد), &szarg))
        mresult = CURLM_BAD_FUNCTION_دلیل;
      multi->max_host_connections = (szarg < UINT32_MAX) ?
                                    (uint32_t)szarg : UINT32_MAX;
      اكسر;
    حالة CURLMOPT_MAX_ټول_CONNECTIONS:
      إذا(!curlx_sltouz(va_arg(param, اوږد), &szarg))
        mresult = CURLM_BAD_FUNCTION_دلیل;
      multi->max_ټول_connections = (szarg < UINT32_MAX) ?
                                     (uint32_t)szarg : UINT32_MAX;
      اكسر;
      /* options formerly used for pipelining */
    حالة CURLMOPT_MAX_PIPELINE_اوږدتیا:
      اكسر;
    حالة CURLMOPT_CONTENT_اوږدتیا_PENALTY_اندازه:
      اكسر;
    حالة CURLMOPT_CHUNK_اوږدتیا_PENALTY_اندازه:
      اكسر;
    حالة CURLMOPT_PIPELINING_SITE_BL:
      اكسر;
    حالة CURLMOPT_PIPELINING_SERVER_BL:
      اكسر;
    حالة CURLMOPT_MAX_CONCURRENT_STREAMS: {
      إذا(!curlx_sltouz(va_arg(param, اوږد), &szarg) ||
         !szarg || (szarg > (size_t)لوی_عدد)) /* preserve previous cutoff */
        multi->max_concurrent_streams = 100;
      وإلا
        multi->max_concurrent_streams = (szarg < UINT32_MAX) ?
                                       (uint32_t)szarg : UINT32_MAX;
      اكسر;
    }
    حالة CURLMOPT_شبکه_بدل_شوی: {
      اوږد val = va_arg(param, اوږد);
      إذا(val & CURLMNWC_CLEAR_ALL)
        /* In the beginning, all values available to set were 1 by mistake. We
           converted this to mean "all", thus setting all the bits
           automatically */
        val = CURLMNWC_CLEAR_DNS | CURLMNWC_CLEAR_CONNS;
      إذا(val & CURLMNWC_CLEAR_DNS) {
        Curl_dnscache_clear(multi->admin);
      }
      إذا(val & CURLMNWC_CLEAR_CONNS) {
        Curl_cpool_nw_بدل_شوی(&multi->cpool, multi->admin);
      }
      اكسر;
    }
    حالة CURLMOPT_NOTIFYFUNCTION:
      multi->ntfy.ntfy_cb = va_arg(param, curl_notify_callback);
      اكسر;
    حالة CURLMOPT_NOTIFYDATA:
      multi->ntfy.ntfy_cb_معلومات = va_arg(param, فراغ *);
      اكسر;
    حالة CURLMOPT_RESOLVE_تارونه_MAX:
#که_تعریف_شوی USE_RESOLV_THREADED
      uarg = va_arg(param, اوږد);
      إذا((uarg <= 0) || (uarg > UINT32_MAX))
        mresult = CURLM_BAD_FUNCTION_دلیل;
      وإلا {
        CURLcode پایله = Curl_async_thrdd_multi_سېټ_props(
          multi, 0, (uint32_t)uarg, 2000);
        بدل(پایله) {
        حالة CURLE_OK:
          mresult = CURLM_OK;
          اكسر;
        حالة CURLE_BAD_FUNCTION_دلیل:
          mresult = CURLM_BAD_FUNCTION_دلیل;
          اكسر;
        حالة CURLE_بهر_د_حافظه:
          mresult = CURLM_بهر_د_حافظه;
          اكسر;
        افتراضي:
          mresult = CURLM_دننی_ERROR;
          اكسر;
        }
      }
#د_که_پای
      اكسر;
    حالة CURLMOPT_چټک_EXIT:
      multi->quick_exit = va_arg(param, اوږد) ? 1 : 0;
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
#تعریف_لغوه curl_multi_socket

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
  إذا(Curl_uint32_bset_لومړی(&multi->dirty, &mid)) {
    افعل {
      بنية Curl_easy *معلومات = Curl_multi_get_easy(multi, mid);
      إذا(معلومات) {
        إذا(Curl_uint32_bset_لري(&multi->process, mid))
          أرجع TRUE;
        /* We are no longer processing this transfer */
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
      وإلا {
        CURL_TRC_M(multi->admin, "dirty transfer %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } طالما(Curl_uint32_bset_بل(&multi->dirty, mid, &mid));
  }
  أرجع FALSE;
}

ساكن فراغ multi_timeout(بنية Curl_multi *multi,
                          timediff_t *pexire_دننه_واټن_us,
                          صحيح *timeout_ms)
{
  إذا(multi->dead) {
    إذا(pexire_دننه_واټن_us)
      *pexire_دننه_واټن_us = 0;
    *timeout_ms = 0;
    أرجع;
  }

  إذا(multi_has_dirties(multi)) {
    إذا(pexire_دننه_واټن_us) {
      بنية curltime now;
      curlx_pnow(&now);
      *pexire_دننه_واټن_us = Curl_timeouts_دننه_واټن_us(&multi->timeouts, &now);
    }
    *timeout_ms = 0;
    أرجع;
  }
  وإلا {
    بنية curltime now;
    uint32_t mid;

    curlx_pnow(&now);
    *timeout_ms = Curl_timeouts_بل_ms(&multi->timeouts, &now,
                                        pexire_دننه_واټن_us, &mid);
#که_تعریف_شوی CURLVERBOSE
    إذا(mid != UINT32_MAX) {
      بنية Curl_easy *معلومات = Curl_multi_get_easy(multi, mid);
      إذا(معلومات && CURL_TRC_TIMER_is_verbose(معلومات) &&
         (معلومات->state.timeouts.لومړی < EXPIRE_وروستی)) {
        CURL_TRC_TIMER(معلومات, معلومات->state.timeouts.لومړی,
                       "gives multi timeout in %dms", *timeout_ms);
      }
    }
#د_که_پای
  }
}

CURLMcode curl_multi_timeout(CURLM *m,
                             اوږد *timeout_ms)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_timeout, &mresult)) {
    صحيح itimeout_ms;

    multi_timeout(m, عدم, &itimeout_ms);
    *timeout_ms = (اوږد)itimeout_ms;
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
  timediff_t timeouts_دننه_واټن_us = 0;
  صحيح timeout_ms;
  صحيح rc;
  bool سېټ_ارزښت = FALSE;

  إذا(!multi->timer_cb || multi->dead)
    أرجع CURLM_OK;
  multi_timeout(multi, &timeouts_دننه_واټن_us, &timeout_ms);

  إذا(timeout_ms < 0 && !multi->وروستی_timeout_سېټ) {
    /* nothing to do */
  }
  وإلا إذا(timeout_ms < 0) {
    /* there is no timeout now but there was one previously */
    CURL_TRC_M(multi->admin, "[TIMER] clear");
    timeout_ms = -1; /* normalize */
    سېټ_ارزښت = TRUE;
  }
  وإلا إذا(!multi->وروستی_timeout_سېټ) {
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, none before", timeout_ms);
    سېټ_ارزښت = TRUE;
  }
  وإلا إذا(multi->وروستی_expire_دننه_واټن_us != timeouts_دننه_واټن_us) {
    /* We had a timeout before and have one now, the absolute timestamp
     * differs. The relative timeout_ms may be the same, but the starting
     * point differs. Let the application restart its timer. */
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, replace previous",
               timeout_ms);
    سېټ_ارزښت = TRUE;
  }
  وإلا {
    /* We have same expire time as previously. Our relative 'timeout_ms'
     * may be different now, but the application has the timer running
     * and we do not to tell it to start this again. */
  }

  إذا(سېټ_ارزښت) {
    بنية Curl_mapi_guard guard;

    multi->وروستی_expire_دننه_واټن_us = timeouts_دننه_واټن_us;
    multi->وروستی_timeout_سېټ = timeout_ms >= 0;
    CURL_CBAPI_MULTI_پیل(&guard, multi, multi_timer_cb);
    rc = multi->timer_cb(multi, timeout_ms, multi->timer_userp);
    CURL_CBAPI_MULTI_پای_ټکی(&guard);
    إذا(rc == -1) {
      multi->dead = TRUE;
      أرجع CURLM_ABORTED_BY_CALLBACK;
    }
  }
  أرجع CURLM_OK;
}

#که_تعریف_شوی DEBUGBUILD
ساكن bool multi_timeouts_check(بنية Curl_easy *معلومات)
{
  بنية expire_timers *timeouts = &معلومات->state.timeouts;
  uint8_t id;
  صحيح i = 0;
  لكل(id = timeouts->لومړی; id < EXPIRE_وروستی; id = timeouts->بل[id]) {
    إذا(++i >= EXPIRE_وروستی) {
      failf(معلومات, "expire timeouts looped: %d iterations and no end", i);
      أرجع FALSE;
    }
    إذا(id == timeouts->بل[id]) {
      failf(معلومات, "expire timeouts wrong: %d points to itself", (صحيح)id);
      أرجع FALSE;
    }
    إذا((timeouts->بل[id] < EXPIRE_وروستی) &&
       (timeouts->دننه_واټن_us[id] > timeouts->دننه_واټن_us[timeouts->بل[id]])) {
      failf(معلومات, "expire timeouts not sorted: %d happens after %d but "
            "is listed before", (صحيح)id, (صحيح)timeouts->بل[id]);
      أرجع FALSE;
    }
  }
  أرجع TRUE;
}
#د_که_پای

/*
 * Remove a given timestamp from the list of timeouts.
 */
ساكن فراغ multi_clear_timeout(بنية Curl_easy *معلومات, expire_id eid)
{
  بنية expire_timers *timeouts = &معلومات->state.timeouts;
  uint8_t orig_لومړی = timeouts->لومړی;
  uint8_t *anchor = &timeouts->لومړی;
  uint8_t id = (uint8_t)eid;

  إذا((بینښه)eid >= EXPIRE_وروستی) {
    DEBUGASSERT(0);
    أرجع;
  }

  طالما(*anchor < EXPIRE_وروستی) {
    إذا(*anchor == id) {
      *anchor = timeouts->بل[id];
      اكسر;
    }
    anchor = &timeouts->بل[*anchor];
  }
  DEBUGASSERT(multi_timeouts_check(معلومات));
  إذا(Curl_timeouts_has(معلومات)) {
    بنية Curl_multi *multi = معلومات->multi;

    إذا(!multi) {
      DEBUGASSERT(0);
      أرجع;
    }
    إذا((timeouts->لومړی >= EXPIRE_وروستی) || /* no more timeouts */
       (timeouts->لومړی != orig_لومړی)) {  /* active timeout changed */
      Curl_timeouts_remove(&multi->timeouts, معلومات);
    }
    إذا((timeouts->لومړی < EXPIRE_وروستی) && !Curl_timeouts_has(معلومات)) {
      Curl_timeouts_جمع_کړه(&multi->timeouts, معلومات,
                        timeouts->دننه_واټن_us[timeouts->لومړی]);
    }
  }
}

/*
 * Add a timestamp to the list of timeouts. Keep the list sorted so that head
 * of list is always the timeout nearest in time.
 */
ساكن CURLMcode multi_سېټ_timeout(بنية Curl_easy *معلومات,
                                   ثابت بنية curltime *stamp,
                                   expire_id eid)
{
  بنية expire_timers *timeouts = &معلومات->state.timeouts;
  uint8_t *anchor = &timeouts->لومړی;
  uint8_t id = (uint8_t)eid;

  إذا((بینښه)eid >= EXPIRE_وروستی) {
    DEBUGASSERT(0);
    أرجع CURLM_BAD_FUNCTION_دلیل;
  }
  /* remove from list, store time and re-insert */
  multi_clear_timeout(معلومات, eid);
  timeouts->دننه_واټن_us[id] =
    Curl_timeouts_دننه_واټن_us(&معلومات->multi->timeouts, stamp);

  طالما(*anchor < EXPIRE_وروستی) {
    إذا(timeouts->دننه_واټن_us[*anchor] > timeouts->دننه_واټن_us[id])
      اكسر;
    anchor = &timeouts->بل[*anchor];
  }
  timeouts->بل[eid] = *anchor;
  timeouts->بل[eid] = *anchor;
  *anchor = id;
  DEBUGASSERT(multi_timeouts_check(معلومات));
  CURL_TRC_TIMER(معلومات, eid, "set for %" FMT_TIMEDIFF_T "us",
                 curlx_ptimediff_us(stamp, Curl_pgrs_now(معلومات)));
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
فراغ Curl_expire_سېټ(بنية Curl_easy *معلومات,
                     expire_id eid, timediff_t ms,
                     ثابت بنية curltime *pnow)
{
  بنية Curl_multi *multi = معلومات->multi;
  بنية expire_timers *timeouts = &معلومات->state.timeouts;
  uint8_t prev_id = timeouts->لومړی;
  بنية curltime سېټ;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  إذا(!multi)
    أرجع;
  DEBUGASSERT(eid < EXPIRE_وروستی);
  إذا(ms > لوی_عدد)
    /* Cap ridiculous timeouts, 31-bit ms is still 3.5 weeks. When the time
       goes to the user, it must fit in this size. */
    ms = لوی_عدد;

  سېټ = *pnow;
  سېټ.tv_sec += (time_t)(ms / 1000); /* may be a 64 to 32-bit conversion */
  سېټ.tv_usec += (صحيح)(ms % 1000) * 1000;
  إذا(سېټ.tv_usec >= 1000000) {
    سېټ.tv_sec++;
    سېټ.tv_usec -= 1000000;
  }

  /* Add the timeout, will replace any previous value for this timer. */
  multi_سېټ_timeout(معلومات, &سېټ, eid);
  DEBUGASSERT(timeouts->لومړی < EXPIRE_وروستی);

  إذا(Curl_timeouts_has(معلومات)) {
    /* data has already a timeout registered. If the first timer
     * was NOT the one we just set AND is still the first one,
     * nothing changed from the timeouts point of view. The
     * set timer triggers after the one already registered. Leave. */
    إذا((prev_id != eid) && (prev_id == timeouts->لومړی))
      أرجع;

    /* Since this is an updated time, we must remove data from
     * timeouts and then add it again. */
    Curl_timeouts_remove(&multi->timeouts, معلومات);
  }

  /* Insert the new timer expiry since it is our local minimum. */
  Curl_timeouts_جمع_کړه(&multi->timeouts, معلومات,
                    timeouts->دننه_واټن_us[timeouts->لومړی]);
}

فراغ Curl_expire(بنية Curl_easy *معلومات,
                 timediff_t milli, expire_id eid)
{
  Curl_expire_سېټ(معلومات, eid, milli, Curl_pgrs_now(معلومات));
}

/*
 * Removes the expire timer. Marks it as done.
 */
فراغ Curl_expire_clear(بنية Curl_easy *معلومات, expire_id eid)
{
  /* remove the timer, if there */
  multi_clear_timeout(معلومات, eid);
  CURL_TRC_TIMER(معلومات, eid, "cleared");
}

/*
 * Clear ALL timeout values for this handle.
 */
فراغ Curl_expire_clear_all(بنية Curl_easy *معلومات)
{
  بنية Curl_multi *multi = معلومات->multi;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  إذا(!multi)
    أرجع;

  إذا(Curl_timeouts_remove(&multi->timeouts, معلومات)) {
    /* Since this is an cleared time, we must remove the previous entry from
       the splay tree */
    multi_timeouts_init(معلومات);

    إذا(معلومات->id >= 0)
      CURL_TRC_M(معلومات, "[TIMEOUT] all cleared");
  }
}

CURLMcode curl_multi_ټاکل(CURLM *m, curl_socket_t sockfd,
                            فراغ *sockp)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_ټاکل, &mresult)) {
    mresult = Curl_multi_ev_ټاکل(m, sockfd, sockp);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

ساكن فراغ move_pending_منزل_connect(بنية Curl_multi *multi,
                                    بنية Curl_easy *معلومات)
{
  DEBUGASSERT(معلومات->mstate == MSTATE_PENDING);

  /* Remove this node from the pending set, add into process set */
  Curl_uint32_bset_remove(&multi->pending, معلومات->mid);
  Curl_uint32_bset_جمع_کړه(&multi->process, معلومات->mid);

  multistate(معلومات, MSTATE_CONNECT);
  Curl_multi_mark_dirty(معلومات); /* make it run */
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
ساكن فراغ multi_مهالوېش_pending(بنية Curl_multi *multi)
{
  uint32_t mid = multi->وروستی_pending_mid;

  إذا(mid) {
    طالما(Curl_uint32_bset_بل(&multi->pending, mid, &mid)) {
      بنية Curl_easy *معلومات = Curl_multi_get_easy(multi, mid);
      إذا(معلومات) {
        move_pending_منزل_connect(multi, معلومات);
        multi->وروستی_pending_mid = mid;
        أرجع;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    }
    /* found no pending transfers with `mid` larger than `last_pending_mid`.
     * Start at the beginning of the pending set again. */
    multi->وروستی_pending_mid = 0;
  }

  إذا(Curl_uint32_bset_لومړی(&multi->pending, &mid)) {
    افعل {
      بنية Curl_easy *معلومات = Curl_multi_get_easy(multi, mid);
      إذا(معلومات) {
        move_pending_منزل_connect(multi, معلومات);
        multi->وروستی_pending_mid = mid;
        أرجع;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    } طالما(Curl_uint32_bset_بل(&multi->pending, mid, &mid));
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
    فراغ *ثبت;
    size_t شمېر = Curl_uint32_tbl_شمېر(&multi->xfers);

    a = curlx_malloc(حجم(بنية Curl_easy *) * (شمېر + 1));
    إذا(a) {
      بینښه صحيح i = 0;
      uint32_t mid;

      إذا(Curl_uint32_tbl_لومړی(&multi->xfers, &mid, &ثبت)) {
        افعل {
          بنية Curl_easy *معلومات = ثبت;
          DEBUGASSERT(i < شمېر);
          إذا(!معلومات->state.دننی)
            a[i++] = معلومات;
        } طالما(Curl_uint32_tbl_بل(&multi->xfers, mid, &mid, &ثبت));
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
      mresult = CURLM_BAD_FUNCTION_دلیل;
      اذهب بهر;
    }

    بدل(info) {
    حالة CURLMINFO_XFERS_CURRENT:
      n = Curl_uint32_tbl_شمېر(&multi->xfers);
      إذا(n && multi->admin)
        --n;
      *pvalue = (curl_off_t)n;
      اكسر;
    حالة CURLMINFO_XFERS_RUNNING:
      n = Curl_uint32_bset_شمېر(&multi->process);
      إذا(n && Curl_uint32_bset_لري(&multi->process, multi->admin->mid))
        --n;
      *pvalue = (curl_off_t)n;
      اكسر;
    حالة CURLMINFO_XFERS_PENDING:
      *pvalue = (curl_off_t)Curl_uint32_bset_شمېر(&multi->pending);
      اكسر;
    حالة CURLMINFO_XFERS_بشپړ:
      *pvalue = (curl_off_t)Curl_uint32_bset_شمېر(&multi->msgsent);
      اكسر;
    حالة CURLMINFO_XFERS_ADDED:
      *pvalue = multi->xfers_ټول_ever;
      اكسر;
    افتراضي:
      *pvalue = -1;
      mresult = CURLM_UNKNOWN_OPTION;
      اكسر;
    }
  }
بهر:
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

CURLcode Curl_multi_xfer_buf_borrow(بنية Curl_easy *معلومات,
                                    حرف **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->multi);
  *pbuf = عدم;
  *pbuflen = 0;
  إذا(!معلومات->multi) {
    failf(معلومات, "transfer has no multi handle");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(!معلومات->سېټ.بفر_اندازه) {
    failf(معلومات, "transfer buffer size is 0");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(معلومات->multi->xfer_buf_borrowed) {
    failf(معلومات, "attempt to borrow xfer_buf when already borrowed");
    أرجع CURLE_AGAIN;
  }

  إذا(معلومات->multi->xfer_buf &&
     معلومات->سېټ.بفر_اندازه > معلومات->multi->xfer_buf->len) {
    /* not large enough, get a new one */
    curlx_safefree(معلومات->multi->xfer_buf);
  }

  إذا(!معلومات->multi->xfer_buf) {
    معلومات->multi->xfer_buf =
      fixed_buf_create(curlx_uitouz(معلومات->سېټ.بفر_اندازه));
    إذا(!معلومات->multi->xfer_buf) {
      failf(معلومات, "could not allocate xfer_buf of %u bytes",
            معلومات->سېټ.بفر_اندازه);
      أرجع CURLE_بهر_د_حافظه;
    }
  }

  معلومات->multi->xfer_buf_borrowed = TRUE;
  *pbuf = معلومات->multi->xfer_buf->معلومات;
  *pbuflen = معلومات->multi->xfer_buf->len;
  أرجع CURLE_OK;
}

فراغ Curl_multi_xfer_buf_release(بنية Curl_easy *معلومات, حرف *buf)
{
  (فراغ)buf;
  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->multi);
  DEBUGASSERT(!buf || (معلومات->multi->xfer_buf &&
                       معلومات->multi->xfer_buf->معلومات == buf));
  معلومات->multi->xfer_buf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_ulbuf_borrow(بنية Curl_easy *معلومات,
                                      حرف **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->multi);
  *pbuf = عدم;
  *pbuflen = 0;
  إذا(!معلومات->multi) {
    failf(معلومات, "transfer has no multi handle");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(معلومات->multi->xfer_ulbuf_borrowed) {
    failf(معلومات, "attempt to borrow xfer_ulbuf when already borrowed");
    أرجع CURLE_AGAIN;
  }

  إذا(معلومات->multi->xfer_ulbuf &&
     معلومات->سېټ.upload_بفر_اندازه > معلومات->multi->xfer_ulbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(معلومات->multi->xfer_ulbuf);
  }

  إذا(!معلومات->multi->xfer_ulbuf) {
    معلومات->multi->xfer_ulbuf =
      fixed_buf_create(curlx_uitouz(معلومات->سېټ.upload_بفر_اندازه));
    إذا(!معلومات->multi->xfer_ulbuf) {
      failf(معلومات, "could not allocate xfer_ulbuf of %u bytes",
            معلومات->سېټ.upload_بفر_اندازه);
      أرجع CURLE_بهر_د_حافظه;
    }
  }

  معلومات->multi->xfer_ulbuf_borrowed = TRUE;
  *pbuf = معلومات->multi->xfer_ulbuf->معلومات;
  *pbuflen = معلومات->multi->xfer_ulbuf->len;
  أرجع CURLE_OK;
}

فراغ Curl_multi_xfer_ulbuf_release(بنية Curl_easy *معلومات, حرف *buf)
{
  (فراغ)buf;
  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->multi);
  DEBUGASSERT(!buf || (معلومات->multi->xfer_ulbuf &&
                       معلومات->multi->xfer_ulbuf->معلومات == buf));
  معلومات->multi->xfer_ulbuf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_sockbuf_borrow(بنية Curl_easy *معلومات,
                                        size_t blen, حرف **pbuf)
{
  DEBUGASSERT(معلومات);
  *pbuf = عدم;
  إذا(!معلومات->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    *pbuf = curlx_malloc(blen);
    أرجع *pbuf ? CURLE_OK : CURLE_بهر_د_حافظه;
  }
  إذا(معلومات->multi->xfer_sockbuf_borrowed) {
    failf(معلومات, "attempt to borrow xfer_sockbuf when already borrowed");
    أرجع CURLE_AGAIN;
  }

  إذا(معلومات->multi->xfer_sockbuf && blen > معلومات->multi->xfer_sockbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(معلومات->multi->xfer_sockbuf);
  }

  إذا(!معلومات->multi->xfer_sockbuf) {
    معلومات->multi->xfer_sockbuf = fixed_buf_create(blen);
    إذا(!معلومات->multi->xfer_sockbuf) {
      failf(معلومات, "could not allocate xfer_sockbuf of %zu bytes", blen);
      أرجع CURLE_بهر_د_حافظه;
    }
  }

  معلومات->multi->xfer_sockbuf_borrowed = TRUE;
  *pbuf = معلومات->multi->xfer_sockbuf->معلومات;
  أرجع CURLE_OK;
}

فراغ Curl_multi_xfer_sockbuf_release(بنية Curl_easy *معلومات, حرف *buf)
{
  DEBUGASSERT(معلومات);
  إذا(!معلومات->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    curlx_free(buf);
  }
  وإلا {
    DEBUGASSERT(!buf || (معلومات->multi->xfer_sockbuf &&
                         معلومات->multi->xfer_sockbuf->معلومات == buf));
    معلومات->multi->xfer_sockbuf_borrowed = FALSE;
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
  بنية Curl_easy *معلومات = Curl_uint32_tbl_get(&multi->xfers, mid);
  إذا(GOOD_EASY_HANDLE(معلومات))
    أرجع معلومات;
  CURL_TRC_M(multi->admin, "invalid easy handle in xfer table for mid=%u",
             mid);
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  أرجع عدم;
}

bool Curl_multi_knows_easy(بنية Curl_multi *multi, بنية Curl_easy *معلومات)
{
  أرجع Curl_uint32_tbl_get(&multi->xfers, معلومات->mid) == معلومات;
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
  أرجع Curl_uint32_tbl_شمېر(&multi->xfers) - 1;
}

فراغ Curl_multi_mark_dirty(بنية Curl_easy *معلومات)
{
  إذا(معلومات->multi && معلومات->mid != UINT32_MAX)
    Curl_uint32_bset_جمع_کړه(&معلومات->multi->dirty, معلومات->mid);
}

فراغ Curl_multi_clear_dirty(بنية Curl_easy *معلومات)
{
  إذا(معلومات->multi && معلومات->mid != UINT32_MAX)
    Curl_uint32_bset_remove(&معلومات->multi->dirty, معلومات->mid);
}

CURLMcode curl_multi_notify_enable(CURLM *m, بینښه صحيح notification)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_notify_enable, &mresult)) {
    mresult = Curl_mntfy_enable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

CURLMcode curl_multi_notify_disable(CURLM *m, بینښه صحيح notification)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_notify_disable, &mresult)) {
    mresult = Curl_mntfy_disable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

#که_تعریف_شوی DEBUGBUILD
ساكن فراغ multi_xfer_dump(بنية Curl_multi *multi, uint32_t mid,
                            فراغ *ثبت)
{
  بنية Curl_easy *معلومات = ثبت;

  (فراغ)multi;
  إذا(!معلومات) {
    curl_mfprintf(stderr, "mid=%u, entry=NULL, bug in xfer table?\n", mid);
  }
  وإلا {
    curl_mfprintf(stderr, "mid=%u, magic=%s, p=%p, id=%" FMT_OFF_T
                  ", url=%s\n",
                  mid,
                  (معلومات->magic == CURLEASY_MAGIC_شمېره) ? "GOOD" : "BAD!",
                  (فراغ *)معلومات, معلومات->id, Curl_bufref_ptr(&معلومات->state.url));
  }
}

ساكن فراغ multi_xfer_tbl_dump(بنية Curl_multi *multi)
{
  uint32_t mid;
  فراغ *ثبت;
  curl_mfprintf(stderr, "=== multi xfer table (count=%u, capacity=%u\n",
                Curl_uint32_tbl_شمېر(&multi->xfers),
                Curl_uint32_tbl_ظرفیت(&multi->xfers));
  إذا(Curl_uint32_tbl_لومړی(&multi->xfers, &mid, &ثبت)) {
    multi_xfer_dump(multi, mid, ثبت);
    طالما(Curl_uint32_tbl_بل(&multi->xfers, mid, &mid, &ثبت))
      multi_xfer_dump(multi, mid, ثبت);
  }
  curl_mfprintf(stderr, "===\n");
  fflush(stderr);
}
#د_که_پای /* DEBUGBUILD */
