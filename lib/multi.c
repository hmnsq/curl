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
#تعریف_کن CURL_XFER_جدول_اندازه 128

/* CURL_SOCKET_HASH_TABLE_SIZE should be a prime number. Increasing it from 97
   to 911 takes on a 32-bit machine 4 x 804 = 3211 more bytes. Still, every
   curl handle takes 6K memory, therefore this 3K are not significant. */
#اگر_تعریف_نشده CURL_SOCKET_HASH_جدول_اندازه
#تعریف_کن CURL_SOCKET_HASH_جدول_اندازه 911
#پایان_اگر

#اگر_تعریف_نشده CURL_CONNECTION_HASH_اندازه
#تعریف_کن CURL_CONNECTION_HASH_اندازه 97
#پایان_اگر

#اگر_تعریف_نشده CURL_DNS_HASH_اندازه
#تعریف_کن CURL_DNS_HASH_اندازه 71
#پایان_اگر

#اگر_تعریف_نشده CURL_TLS_SESSION_اندازه
#تعریف_کن CURL_TLS_SESSION_اندازه 25
#پایان_اگر

ساكن فراغ move_pending_مقصد_connect(بنية Curl_multi *multi,
                                    بنية Curl_easy *داده);
ساكن CURLMcode جمع_کن_بعدی_timeout(ثابت بنية curltime *pnow,
                                  بنية Curl_multi *multi,
                                  بنية Curl_easy *داده);
ساكن فراغ multi_timeout(بنية Curl_multi *multi,
                          timediff_t *pexire_جابه_جایی_us,
                          صحيح *timeout_ms);
ساكن فراغ multi_زمانبندی_pending(بنية Curl_multi *multi);
ساكن فراغ multi_xfer_bufs_free(بنية Curl_multi *multi);
#اگر_تعریف_شده DEBUGBUILD
ساكن فراغ multi_xfer_tbl_dump(بنية Curl_multi *multi);
#پایان_اگر
/* Get the # of transfers current in process/pending. */
ساكن uint32_t multi_xfers_running(بنية Curl_multi *multi);

/* function pointer called once when entering a state */
تعريفنوع فراغ (*mstate_enter_func)(بنية Curl_easy *داده,
                                  CURLMstate from_state);

ساكن فراغ mstate_enter_connect(بنية Curl_easy *داده,
                                 CURLMstate from_state)
{
  (فراغ)from_state;
  Curl_init_CONNECT(داده);
}

ساكن فراغ mstate_enter_did(بنية Curl_easy *داده,
                             CURLMstate from_state)
{
  (فراغ)from_state;
  داده->req.chunk = FALSE;
  Curl_pgrs_زمان(داده, TIMER_PRETRANSFER);
  إذا(!CURL_REQ_WANT_SEND(داده))
    Curl_pgrs_زمان(داده, TIMER_POSTRANSFER);
}

ساكن فراغ mstate_enter_انجامشده(بنية Curl_easy *داده,
                              CURLMstate from_state)
{
  (فراغ)from_state;
  CURLM_NTFY(داده, CURLMNOTIFY_EASY_انجامشده);
}

ساكن فراغ mstate_enter_completed(بنية Curl_easy *داده,
                                   CURLMstate from_state)
{
  /* we sometimes directly jump to COMPLETED, trigger things
   * we then missed. */
  إذا(from_state < MSTATE_DID) {
    Curl_pgrs_زمان(داده, TIMER_PRETRANSFER);
    Curl_pgrs_زمان(داده, TIMER_POSTRANSFER);
    Curl_pgrs_زمان(داده, TIMER_STARTTRANSFER);
  }
  /* multi_done() should have run and detached any connection already */
  DEBUGASSERT(!داده->conn);
  Curl_pgrsCompleted(داده);
  إذا(from_state < MSTATE_انجامشده)
    CURLM_NTFY(داده, CURLMNOTIFY_EASY_انجامشده);
  /* changing to COMPLETED means it is in process and needs to go */
  DEBUGASSERT(Curl_uint32_bset_شامل_است(&داده->multi->process, داده->mid));
  Curl_uint32_bset_remove(&داده->multi->process, داده->mid);
  Curl_uint32_bset_remove(&داده->multi->pending, داده->mid); /* to be sure */

  إذا(Curl_uint32_bset_خالی(&داده->multi->process)) {
    /* free the transfer buffer when we have no more active transfers */
    multi_xfer_bufs_free(داده->multi);
  }
  /* Important: reset the conn pointer so that we do not point to memory
     that could be freed anytime */
  Curl_detach_connection(داده);
  Curl_expire_clear_all(داده); /* stop all timers */
}

/* always use this function to change state, to make debugging easier */
ساكن فراغ mstate(بنية Curl_easy *داده, CURLMstate state
#اگر_تعریف_شده DEBUGBUILD
                   , صحيح lineno
#پایان_اگر
)
{
  CURLMstate oldstate = داده->mstate;
  ساكن ثابت mstate_enter_func state_enter[MSTATE_آخرین] = {
    عدم,                      /* INIT */
    عدم,                      /* PENDING */
    عدم,                      /*راهاندازی*/
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
    mstate_enter_انجامشده,         /* DONE */
    mstate_enter_completed,    /* COMPLETED */
    عدم                       /* MSGSENT */
  };

  إذا(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    أرجع;

#اگر_تعریف_شده DEBUGBUILD
  NOVERBOSE((فراغ)lineno);
  CURL_TRC_M(داده, "-> [%s] (line %d)", CURL_MSTATE_NAME(state), lineno);
#وإلا
  CURL_TRC_M(داده, "-> [%s]", CURL_MSTATE_NAME(state));
#پایان_اگر

  /* really switching state */
  داده->mstate = state;
  إذا(state_enter[state])
    state_enter[state](داده, oldstate);
}

#اگر_تعریف_نشده DEBUGBUILD
#تعریف_کن multistate(x, y) mstate(x, y)
#وإلا
#تعریف_کن multistate(x, y) mstate(x, y, __LINE__)
#پایان_اگر

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
ساكن فراغ multi_addmsg(بنية Curl_multi *multi, بنية Curl_easy *داده)
{
  إذا(Curl_uint32_bset_خالی(&multi->msgsent))
    CURLM_NTFY(multi->admin, CURLMNOTIFY_INFO_READ);
  Curl_uint32_bset_جمع_کن(&multi->msgsent, داده->mid);
}

ساكن فراغ multi_timeouts_init(بنية Curl_easy *داده);

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

  multi->magic = CURLMULTI_MAGIC_عدد;

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
  multi->admin->state.داخلی = TRUE;

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
  Curl_hash_init(&multi->proto_hash, 23, CURL_HASH_TYPE_بایتها, ph_freeentry);
  Curl_cshutdn_init(&multi->cshutdn);

  multi->multiplexing = TRUE;
  multi->max_concurrent_streams = 100;
#اگر_تعریف_شده ENABLE_WAKEUP
  multi->wakeup_pair[0] = CURL_SOCKET_BAD;
  multi->wakeup_pair[1] = CURL_SOCKET_BAD;
#پایان_اگر
#اگر_تعریف_شده ENABLE_داخلی_WAKEUP
  multi->wakeup_داخلی[0] = CURL_SOCKET_BAD;
  multi->wakeup_داخلی[1] = CURL_SOCKET_BAD;
#پایان_اگر

  إذا(Curl_uint32_bset_resize(&multi->process, xfer_جدول_اندازه) ||
     Curl_uint32_bset_resize(&multi->pending, xfer_جدول_اندازه) ||
     Curl_uint32_bset_resize(&multi->dirty, xfer_جدول_اندازه) ||
     Curl_uint32_bset_resize(&multi->msgsent, xfer_جدول_اندازه) ||
     Curl_uint32_tbl_resize(&multi->xfers, xfer_جدول_اندازه))
    اذهب error;

#اگر_تعریف_شده DEBUGBUILD
  {
    ثابت حرف *p;
    إذا(getenv("CURL_DEBUG"))
      multi->admin->مجموعه.verbose = TRUE;
    p = getenv("CURL_DBG_MAX_TOTAL_CONNECTIONS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_عدد(&p, &l, UINT32_MAX)) {
        multi->max_مجموع_connections = (uint32_t)l;
      }
    }
  }
#پایان_اگر
  Curl_uint32_tbl_جمع_کن(&multi->xfers, multi->admin, &multi->admin->mid);
  Curl_uint32_bset_جمع_کن(&multi->process, multi->admin->mid);

  Curl_cpool_init(&multi->cpool, عدم, chashsize);

#اگر_تعریف_شده USE_SSL
  إذا(Curl_ssl_scache_create(sesssize, 2, &multi->ssl_scache))
    اذهب error;
#وإلا
  (فراغ)sesssize;
#پایان_اگر

#اگر_تعریف_شده USE_WINSOCK
  multi->wsa_event = WSACreateEvent();
  إذا(multi->wsa_event == WSA_INVALID_EVENT)
    اذهب error;
#پایان_اگر
#اگر_تعریف_شده ENABLE_WAKEUP
  /* When enabled, rely on this to work. We ignore this in previous
   * versions, but that seems an unnecessary complication. */
  إذا(Curl_wakeup_init(multi->wakeup_pair, TRUE) < 0)
    اذهب error;
#پایان_اگر
#اگر_تعریف_شده ENABLE_داخلی_WAKEUP
  إذا(Curl_wakeup_init(multi->wakeup_داخلی, TRUE) < 0)
    اذهب error;
#پایان_اگر

  إذا(Curl_probeipv6(multi))
    اذهب error;

#اگر_تعریف_شده USE_RESOLV_THREADED
  إذا(xfer_جدول_اندازه < CURL_XFER_جدول_اندازه) { /* easy multi */
    إذا(Curl_async_thrdd_multi_init(multi, 0, 2, 10))
      اذهب error;
  }
  وإلا { /* real multi handle */
    إذا(Curl_async_thrdd_multi_init(multi, 0, 20, 2000))
      اذهب error;
  }
#پایان_اگر

  أرجع multi;

error:

#اگر_تعریف_شده USE_RESOLV_THREADED
  Curl_async_thrdd_multi_destroy(multi, TRUE);
#پایان_اگر
  Curl_multi_ev_cleanup(multi);
  Curl_hash_destroy(&multi->proto_hash);
  Curl_dnscache_destroy(&multi->dnscache);
  Curl_cpool_destroy(&multi->cpool, multi->admin);
  Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
#اگر_تعریف_شده USE_SSL
  Curl_ssl_scache_destroy(multi->ssl_scache);
#پایان_اگر
  إذا(multi->admin) {
    Curl_multi_ev_xfer_انجامشده(multi, multi->admin);
    multi->admin->multi = عدم;
    Curl_close(&multi->admin);
  }
  Curl_mntfy_cleanup(multi);

  Curl_uint32_bset_destroy(&multi->process);
  Curl_uint32_bset_destroy(&multi->dirty);
  Curl_uint32_bset_destroy(&multi->pending);
  Curl_uint32_bset_destroy(&multi->msgsent);
  Curl_uint32_tbl_destroy(&multi->xfers);
#اگر_تعریف_شده ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#پایان_اگر
#اگر_تعریف_شده ENABLE_داخلی_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_داخلی);
#پایان_اگر

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
ساكن فراغ multi_warn_debug(بنية Curl_multi *multi, بنية Curl_easy *داده)
{
  إذا(!multi->warned) {
    infof(داده, "!!! WARNING !!!");
    infof(داده, "This is a debug build of libcurl, "
                "do not use in production.");
    multi->warned = TRUE;
  }
}
#وإلا
#تعریف_کن multi_warn_debug(x, y) Curl_nop_stmt
#پایان_اگر

bool Curl_is_connecting(بنية Curl_easy *داده)
{
  أرجع داده->mstate < MSTATE_DO;
}

ساكن CURLMcode multi_assess_wakeup(بنية Curl_multi *multi)
{
#اگر_تعریف_شده ENABLE_داخلی_WAKEUP
  إذا(multi->socket_cb)
    أرجع Curl_multi_ev_assess_xfer(multi, multi->admin);
#وإلا
  (فراغ)multi;
#پایان_اگر
  أرجع CURLM_OK;
}

ساكن CURLMcode multi_xfers_جمع_کن(بنية Curl_multi *multi,
                                 بنية Curl_easy *داده)
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
    uint32_t استفاده_شده = Curl_uint32_tbl_شمار(&multi->xfers);
    uint32_t استفادهنشده = ظرفیت - استفاده_شده;
    uint32_t min_استفادهنشده = CURLMAX(ظرفیت >> 3, 4);
    إذا(استفادهنشده < min_استفادهنشده) {
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
    CURL_TRC_M(داده, "increasing xfer table size to %u", new_اندازه);
    إذا(Curl_uint32_bset_resize(&multi->process, new_اندازه) ||
       Curl_uint32_bset_resize(&multi->dirty, new_اندازه) ||
       Curl_uint32_bset_resize(&multi->pending, new_اندازه) ||
       Curl_uint32_bset_resize(&multi->msgsent, new_اندازه) ||
       Curl_uint32_tbl_resize(&multi->xfers, new_اندازه))
      أرجع CURLM_بیرون_از_حافظه;
  }

  /* Insert the easy into the table now */
  إذا(!Curl_uint32_tbl_جمع_کن(&multi->xfers, داده, &داده->mid)) {
    /* MUST only happen when table is full */
    DEBUGASSERT(Curl_uint32_tbl_ظرفیت(&multi->xfers) <=
                Curl_uint32_tbl_شمار(&multi->xfers));
    أرجع CURLM_بیرون_از_حافظه;
  }
  أرجع CURLM_OK;
}

CURLMcode Curl_multi_جمع_کن_handle(بنية Curl_multi *multi,
                                بنية Curl_easy *داده)
{
  CURLMcode mresult;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  إذا(داده->multi)
    أرجع CURLM_ADDED_ALREADY;

  إذا(multi->dead) {
    /* a "dead" handle cannot get added transfers while any existing easy
       handles are still alive - but if there are none alive anymore, it is
       fine to start over and unmark the "deadness" of this handle.
       This means only the admin handle MUST be present. */
    إذا((Curl_uint32_tbl_شمار(&multi->xfers) != 1) ||
       !Curl_uint32_tbl_شامل_است(&multi->xfers, 0))
      أرجع CURLM_ABORTED_BY_CALLBACK;
    multi->dead = FALSE;
    Curl_uint32_bset_clear(&multi->process);
    Curl_uint32_bset_clear(&multi->dirty);
    Curl_uint32_bset_clear(&multi->pending);
    Curl_uint32_bset_clear(&multi->msgsent);
  }

  إذا(داده->multi_easy) {
    /* if this easy handle was previously used for curl_easy_perform(), there
       is a private multi handle here that we can kill */
    curl_multi_cleanup(داده->multi_easy);
    داده->multi_easy = عدم;
  }

  /* Insert the easy into the multi->xfers table, assigning it a `mid`. */
  إذا(multi_xfers_جمع_کن(multi, داده))
    أرجع CURLM_بیرون_از_حافظه;

  /* Initialize timeouts for this handle */
  multi_timeouts_init(داده);

  /*
   * No failure allowed in this function beyond this point. No modification
   * of easy nor multi handle allowed before this except for potential
   * multi's connection pool growing which will not be undone in this
   * function no matter what.
   */
  إذا(داده->مجموعه.errorbuffer)
    داده->مجموعه.errorbuffer[0] = 0;

  داده->state.os_errno = 0;

  /* make the Curl_easy refer back to this multi handle - before
     Curl_expire() is called. */
  داده->multi = multi;

  /* set the easy handle */
  multistate(داده, MSTATE_INIT);
  /* not yet passed INIT state */
  داده->state.really_alive = FALSE;

#اگر_تعریف_شده USE_LIBPSL
  /* Do the same for PSL. */
  إذا(داده->share && (داده->share->specifier & (1 << CURL_قفل_داده_PSL)))
    داده->psl = &داده->share->psl;
  وإلا
    داده->psl = &multi->psl;
#پایان_اگر

  /* add the easy handle to the process set */
  Curl_uint32_bset_جمع_کن(&multi->process, داده->mid);
  ++multi->xfers_alive;
  ++multi->xfers_مجموع_ever;

  Curl_cpool_xfer_init(داده);
  multi_warn_debug(multi, داده);

  /* Make sure the new handle will run */
  Curl_multi_mark_dirty(داده);

  /* Necessary in event based processing, where dirty handles trigger
   * a timeout callback invocation. */
  mresult = Curl_update_timer(multi);
  إذا(mresult) {
    /* undo the additions above */
    Curl_uint32_bset_remove(&multi->process, داده->mid);
    Curl_uint32_bset_remove(&multi->dirty, داده->mid);
    --multi->xfers_alive;
    داده->multi = عدم; /* not anymore */
    Curl_uint32_tbl_remove(&multi->xfers, داده->mid);
    داده->mid = UINT32_MAX;
    أرجع mresult;
  }

  /* The admin handle only ever has default timeouts set. To improve the
     state somewhat we clone the timeouts from each added handle so that the
     admin handle always has the same timeouts as the most recently added
     easy handle. */
  multi->admin->مجموعه.timeout = داده->مجموعه.timeout;
  multi->admin->مجموعه.server_response_timeout =
    داده->مجموعه.server_response_timeout;
  multi->admin->مجموعه.no_signal = داده->مجموعه.no_signal;

  CURL_TRC_M(داده, "added to multi, mid=%u, running=%u, total=%u",
             داده->mid, multi_xfers_running(multi),
             Curl_uint32_tbl_شمار(&multi->xfers));
  أرجع CURLM_OK;
}

CURLMcode curl_multi_جمع_کن_handle(CURLM *m, CURL *curl)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_جمع_کن_handle, &mresult)) {
    بنية Curl_easy *داده = curl;
    /* Verify that we got a somewhat good easy handle too */
    إذا(!GOOD_EASY_HANDLE(داده) || داده->state.داخلی)
      mresult = CURLM_BAD_EASY_HANDLE;
    وإلا
      mresult = Curl_multi_جمع_کن_handle(m, داده);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

بنية multi_انجامشده_ctx {
  بیت(premature);
};

ساكن bool multi_conn_should_close(بنية connectdata *conn,
                                    بنية Curl_easy *داده,
                                    bool premature)
{
  /* if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of everything else. */
  إذا(conn->بیتها.close)
    أرجع TRUE;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM/NEGOTIATE authentication handshake. */
  إذا(داده->مجموعه.reuse_forbid
#اگر_تعریف_شده USE_NTLM
     && !(conn->http_ntlm_state == NTLMSTATE_TYPE2 ||
          conn->proxy_ntlm_state == NTLMSTATE_TYPE2)
#پایان_اگر
#اگر_تعریف_شده USE_SPNEGO
     && !(conn->http_negotiate_state == CURL_NEGO_AUTH_RECV ||
          conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)
#پایان_اگر
    )
    أرجع TRUE;

  /* Unless this connection is for a "connect-only" transfer, it
   * needs to be closed if the protocol handler does not support reuse. */
  إذا(!داده->مجموعه.connect_only && conn->scheme &&
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

ساكن cpool_do_نتیجه multi_انجامشده_locked(بنية Curl_easy *داده,
                                         بنية connectdata *conn,
                                         فراغ *userdata,
                                         ثابت بنية curltime *pnow)
{
  بنية multi_انجامشده_ctx *mdctx = userdata;

  (فراغ)pnow;
  CURL_TRC_M(داده, "multi_done_locked, in use=%u", conn->attached_xfers);
  إذا(CONN_INUSE(conn)) {
    /* Stop if still used. */
    CURL_TRC_M(داده, "Connection still in use %u, no more multi_done now!",
               conn->attached_xfers);
    أرجع CPOOL_DO_KEEP;
  }

  إذا(multi_conn_should_close(conn, داده, (bool)mdctx->premature)) {
    CURL_TRC_M(داده, "multi_done, terminating conn #%" FMT_OFF_T " to %s:%u, "
               "forbid=%d, close=%d, premature=%d, conn_multiplex=%d",
               conn->connection_id, conn->origin->user_hostname,
               conn->origin->پورت,
               داده->مجموعه.reuse_forbid, conn->بیتها.close, mdctx->premature,
               Curl_conn_is_multiplex(conn, FIRSTSOCKET));
    connclose(conn);
    أرجع mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  وإلا إذا(!Curl_conn_get_max_concurrent(داده, conn, FIRSTSOCKET)) {
    CURL_TRC_M(داده, "multi_done, conn #%" FMT_OFF_T " to %s:%u was shutdown"
               " by server, not reusing", conn->connection_id,
               conn->origin->user_hostname, conn->origin->پورت);
    connclose(conn);
    أرجع mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  وإلا {
    أرجع CPOOL_DO_IDLE;
  }
}

ساكن CURLcode multi_انجامشده(بنية Curl_easy *داده,
                           CURLcode وضعیت,  /* an error if this is called
                                                after an error was detected */
                           bool premature)
{
  CURLcode نتیجه;
  بنية connectdata *conn = داده->conn;
  ثابت بنية curltime *pnow = Curl_pgrs_now(داده);

  CURL_TRC_M(داده, "multi_done: status: %d prem: %d done: %d",
             (صحيح)وضعیت, (صحيح)premature, داده->state.انجامشده);

  إذا(داده->state.انجامشده) {
    /* Stop if multi_done() has already been called */
    أرجع CURLE_OK;
  }
  داده->state.انجامشده = TRUE; /* called now! */

  /* Shut down any ongoing async resolver operation. */
  Curl_resolv_shutdown_all(داده);

  /* Cleanup possible redirect junk */
  curlx_safefree(داده->req.newurl);
  curlx_safefree(داده->req.location);

  بدل(وضعیت) {
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
  إذا(conn && conn->scheme->پیاپی->انجامشده && (داده->mstate >= MSTATE_PROTOCONNECT))
    نتیجه = conn->scheme->پیاپی->انجامشده(داده, وضعیت, premature);
  وإلا
    نتیجه = وضعیت;

  إذا(داده->mstate > MSTATE_CONNECTING &&
     (نتیجه != CURLE_ABORTED_BY_CALLBACK)) {
    /* avoid this if
     * - the transfer has not connected
     * - we already aborted by callback to avoid this calling another callback
     */
    صحيح rc = Curl_pgrs_انجامشده(داده, pnow);
    إذا(!نتیجه && rc)
      نتیجه = CURLE_ABORTED_BY_CALLBACK;
  }

  /* Make sure that transfer client writes are really done now. */
  نتیجه = Curl_1st_fatal(نتیجه, Curl_xfer_write_انجامشده(داده, premature));

  إذا(!نتیجه)
    نتیجه = Curl_req_انجامشده(&داده->req, داده, premature);

  إذا(conn) {
    /* Detach connection and decide what to do with it. */
    بنية multi_انجامشده_ctx mdctx;

    /* Inform connection filters that this transfer is done and they
     * need to give up any references to `data`. */
    Curl_conn_ev_داده_انجامشده(داده, premature);
    Curl_detach_connection(داده);
    Curl_dnscache_prune(داده, pnow);

    memset(&mdctx, 0, حجم(mdctx));
    mdctx.premature = premature;
    Curl_cpool_return(داده, conn, multi_انجامشده_locked, &mdctx, pnow);

    إذا(داده->multi)
      multi_زمانبندی_pending(داده->multi);
  }

  /* flush the netrc cache */
  Curl_netrc_cleanup(&داده->state.netrc);
  أرجع نتیجه;
}

CURLMcode Curl_multi_remove_handle(بنية Curl_multi *multi,
                                   بنية Curl_easy *داده)
{
  CURLMcode mresult;
  bool premature;
  uint32_t mid;

  /* Prevent users from trying to remove same easy handle more than once */
  إذا(!داده->multi)
    أرجع CURLM_OK; /* it is already removed so let's say it is fine! */

  /* Prevent users from trying to remove an easy handle from the wrong multi */
  إذا(داده->multi != multi)
    أرجع CURLM_BAD_EASY_HANDLE;

  إذا(داده->mid == UINT32_MAX) {
    DEBUGASSERT(0);
    أرجع CURLM_داخلی_ERROR;
  }
  إذا(Curl_uint32_tbl_get(&multi->xfers, داده->mid) != داده) {
    DEBUGASSERT(0);
    أرجع CURLM_داخلی_ERROR;
  }

  premature = (داده->mstate < MSTATE_COMPLETED);

  إذا(داده->conn) {
    /* If the 'state' is not INIT or COMPLETED, we might need to do something
       nice to put the easy_handle in a good known state when this returns. */
    إذا(premature && (داده->mstate > MSTATE_DO))
      streamclose(داده->conn);

    /* multi_done() clears the association between the easy handle and the
       connection.
       Note that this ignores the return code because there is
       nothing really useful to do with it anyway! */
    (فراغ)multi_انجامشده(داده, داده->نتیجه, premature);
  }

  /* The timer must be shut down before data->multi is set to NULL, else
     data's splaynode would remain in the splay tree after curl_easy_cleanup is
     called. Do it after multi_done() in case that sets another time! */
  Curl_expire_clear_all(داده);

  /* In MSGSENT, it was deducted from `multi->xfers_alive` already. */
  إذا(داده->mstate != MSTATE_MSGSENT)
    --multi->xfers_alive;

  إذا(داده->state.really_alive) {
    داده->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    إذا(!multi->xfers_really_alive)
      (فراغ)multi_assess_wakeup(multi);
  }

  Curl_wildcard_dtor(&داده->wildcard);

  داده->mstate = MSTATE_COMPLETED;

  /* Remove the association between the connection and the handle */
  Curl_detach_connection(داده);

  /* Tell event handling that this transfer is definitely going away */
  Curl_multi_ev_xfer_انجامشده(multi, داده);

  إذا(داده->مجموعه.connect_only) {
    إذا(داده->multi_easy) {
      إذا(داده->state.آخرین_conn_id != -1) {
        /* Mark any connect-only connection for closure */
        بنية connectdata *conn;
        (فراغ)Curl_getconnectinfo(داده, &conn);
        إذا(conn && conn->بیتها.connect_only)
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
      (فراغ)Curl_getconnectinfo(داده, &conn);
      إذا(conn)
        Curl_conn_close(داده, conn, TRUE);
    }
  }

#اگر_تعریف_شده USE_LIBPSL
  /* Remove the PSL association. */
  إذا(داده->psl == &multi->psl)
    داده->psl = عدم;
#پایان_اگر

  /* clear the association to this multi handle */
  mid = داده->mid;
  DEBUGASSERT(Curl_uint32_tbl_شامل_است(&multi->xfers, mid));
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  Curl_uint32_bset_remove(&multi->process, mid);
  Curl_uint32_bset_remove(&multi->dirty, mid);
  Curl_uint32_bset_remove(&multi->pending, mid);
  Curl_uint32_bset_remove(&multi->msgsent, mid);
  داده->multi = عدم;
  داده->mid = UINT32_MAX;
  داده->master_mid = UINT32_MAX;

  /* A pending transfer *might* be able to run now. */
  multi_زمانبندی_pending(multi);
  mresult = Curl_update_timer(multi);
  إذا(mresult)
    أرجع mresult;

  mresult = multi_assess_wakeup(multi);
  إذا(mresult) {
    failf(داده, "error enabling wakeup listening: %d", mresult);
    أرجع mresult;
  }

  CURL_TRC_M(داده, "removed from multi, mid=%u, running=%u, total=%u",
             mid, multi_xfers_running(multi),
             Curl_uint32_tbl_شمار(&multi->xfers));
  أرجع CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *curl)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_remove_handle, &mresult)) {
    بنية Curl_easy *داده = curl;
    إذا(!GOOD_EASY_HANDLE(داده) || داده->state.داخلی)
      mresult = CURLM_BAD_EASY_HANDLE;
    وإلا
      mresult = Curl_multi_remove_handle(m, داده);
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
فراغ Curl_detach_connection(بنية Curl_easy *داده)
{
  بنية connectdata *conn = داده->conn;
  إذا(conn) {
    /* this should never happen, prevent underflow */
    DEBUGASSERT(conn->attached_xfers);
    إذا(conn->attached_xfers) {
      conn->attached_xfers--;
      إذا(!conn->attached_xfers)
        conn->attached_multi = عدم;
    }
  }
  داده->conn = عدم;
}

/*
 * Curl_attach_connection() attaches this transfer to this connection.
 *
 * This is the only function that should assign data->conn.
 * `matched == TRUE` means the transfer's properties match this
 * connection and it is not a temporary attach for maintenance.
 */
فراغ Curl_attach_connection(بنية Curl_easy *داده,
                            بنية connectdata *conn,
                            bool matched)
{
  DEBUGASSERT(داده);
  DEBUGASSERT(!داده->conn);
  DEBUGASSERT(conn);
  DEBUGASSERT(conn->attached_xfers < UINT32_MAX);
  داده->conn = conn;
  إذا(matched) {
    داده->state.آخرین_conn_id = conn->connection_id;
    داده->state.آخرین_cpid = conn->cpid;
  }
  وإلا
    DEBUGASSERT(!داده->mid); /* admin handle */
  conn->attached_xfers++;
  /* all attached transfers must be from the same multi */
  إذا(!conn->attached_multi)
    conn->attached_multi = داده->multi;
  DEBUGASSERT(conn->attached_multi == داده->multi);

  إذا(conn->scheme && conn->scheme->پیاپی->attach)
    conn->scheme->پیاپی->attach(داده, conn);
}

/* adjust pollset for rate limits/pauses */
ساكن CURLcode multi_adjust_pollset(بنية Curl_easy *داده,
                                     بنية easy_pollset *ps)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(ps->n) {
    bool send_blocked, recv_blocked;

    recv_blocked = (Curl_rlimit_avail(&داده->پیشرفت.dl.rlimit, عدم) <= 0);
    send_blocked = (Curl_rlimit_avail(&داده->پیشرفت.ul.rlimit, عدم) <= 0);
    إذا(send_blocked || recv_blocked) {
      صحيح i;
      لكل(i = 0; i <= SECONDARYSOCKET; ++i) {
        curl_socket_t sock = داده->conn->sock[i];
        إذا(sock == CURL_SOCKET_BAD)
          استمر;
        إذا(recv_blocked && Curl_pollset_want_recv(داده, ps, sock)) {
          نتیجه = Curl_pollset_remove_درون(داده, ps, sock);
          إذا(نتیجه)
            اكسر;
        }
        إذا(send_blocked && Curl_pollset_want_send(داده, ps, sock)) {
          نتیجه = Curl_pollset_remove_بیرون(داده, ps, sock);
          إذا(نتیجه)
            اكسر;
        }
      }
    }

    /* Not blocked and wanting to receive. If there is data pending
     * in the connection filters, make transfer run again. */
    إذا(!recv_blocked &&
       ((Curl_pollset_want_recv(داده, ps, داده->conn->sock[FIRSTSOCKET]) &&
         Curl_conn_داده_pending(داده, FIRSTSOCKET)) ||
        (Curl_pollset_want_recv(داده, ps, داده->conn->sock[SECONDARYSOCKET]) &&
         Curl_conn_داده_pending(داده, SECONDARYSOCKET)))) {
      CURL_TRC_M(داده, "pollset[] has POLLIN, but there is still "
                 "buffered input -> mark as dirty");
      Curl_multi_mark_dirty(داده);
    }
  }
  أرجع نتیجه;
}

ساكن CURLcode mstate_connecting_pollset(بنية Curl_easy *داده,
                                          بنية easy_pollset *ps)
{
  بنية connectdata *conn = داده->conn;
  curl_socket_t sockfd;
  CURLcode نتیجه = CURLE_OK;

  إذا(Curl_xfer_recv_is_paused(داده))
    أرجع CURLE_OK;
  /* If a socket is set, receiving is default. If the socket
   * has not been determined yet (eyeballing), always ask the
   * connection filters for what to monitor. */
  sockfd = Curl_conn_get_اولی_socket(داده);
  إذا(sockfd != CURL_SOCKET_BAD) {
    نتیجه = Curl_pollset_change(داده, ps, sockfd, CURL_POLL_درون, 0);
    إذا(!نتیجه)
      نتیجه = multi_adjust_pollset(داده, ps);
  }
  إذا(!نتیجه)
    نتیجه = Curl_conn_adjust_pollset(داده, conn, ps);
  أرجع نتیجه;
}

ساكن CURLcode mstate_protocol_pollset(بنية Curl_easy *داده,
                                        بنية easy_pollset *ps)
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه = CURLE_OK;

  إذا(conn->scheme->پیاپی->proto_pollset)
    نتیجه = conn->scheme->پیاپی->proto_pollset(داده, ps);
  وإلا {
    curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
    إذا(sockfd != CURL_SOCKET_BAD) {
      /* Default is to wait to something from the server */
      نتیجه = Curl_pollset_change(داده, ps, sockfd, CURL_POLL_درون, 0);
    }
  }
  إذا(!نتیجه)
    نتیجه = multi_adjust_pollset(داده, ps);
  إذا(!نتیجه)
    نتیجه = Curl_conn_adjust_pollset(داده, conn, ps);
  أرجع نتیجه;
}

ساكن CURLcode mstate_do_pollset(بنية Curl_easy *داده,
                                  بنية easy_pollset *ps)
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه = CURLE_OK;

  إذا(conn->scheme->پیاپی->doing_pollset)
    نتیجه = conn->scheme->پیاپی->doing_pollset(داده, ps);
  وإلا إذا(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    نتیجه = Curl_pollset_جمع_کن_بیرون(داده, ps, conn->sock[conn->send_idx]);
  }
  إذا(!نتیجه)
    نتیجه = multi_adjust_pollset(داده, ps);
  إذا(!نتیجه)
    نتیجه = Curl_conn_adjust_pollset(داده, conn, ps);
  أرجع نتیجه;
}

ساكن CURLcode mstate_domore_pollset(بنية Curl_easy *داده,
                                      بنية easy_pollset *ps)
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه = CURLE_OK;

  إذا(conn->scheme->پیاپی->domore_pollset)
    نتیجه = conn->scheme->پیاپی->domore_pollset(داده, ps);
  وإلا إذا(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    نتیجه = Curl_pollset_جمع_کن_بیرون(داده, ps, conn->sock[conn->send_idx]);
  }
  إذا(!نتیجه)
    نتیجه = multi_adjust_pollset(داده, ps);
  إذا(!نتیجه)
    نتیجه = Curl_conn_adjust_pollset(داده, conn, ps);
  أرجع نتیجه;
}

ساكن CURLcode mstate_perform_pollset(بنية Curl_easy *داده,
                                       بنية easy_pollset *ps)
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه = CURLE_OK;

  إذا(conn->scheme->پیاپی->perform_pollset)
    نتیجه = conn->scheme->پیاپی->perform_pollset(داده, ps);
  وإلا {
    /* Default is to obey the request flags for send/recv */
    إذا(Curl_req_want_recv(داده) && CONN_SOCK_IDX_VALID(conn->recv_idx)) {
      نتیجه = Curl_pollset_جمع_کن_درون(داده, ps, conn->sock[conn->recv_idx]);
    }
    إذا(!نتیجه && Curl_req_want_send(داده) &&
       CONN_SOCK_IDX_VALID(conn->send_idx)) {
      نتیجه = Curl_pollset_جمع_کن_بیرون(داده, ps, conn->sock[conn->send_idx]);
    }
  }
  إذا(!نتیجه)
    نتیجه = multi_adjust_pollset(داده, ps);
  إذا(!نتیجه)
    نتیجه = Curl_conn_adjust_pollset(داده, conn, ps);
  أرجع نتیجه;
}

#اگر_تعریف_شده CURLVERBOSE
ساكن size_t multi_timeouts_شمار(بنية expire_timers *timeouts)
{
  size_t n = 0;
  uint8_t eid = timeouts->اولی;
  لكل(; eid < EXPIRE_آخرین; eid = timeouts->بعدی[eid])
    ++n;
  أرجع n;
}
#پایان_اگر

/* Initializes `poll_set` with the current socket poll actions needed
 * for transfer `data`. */
CURLMcode Curl_multi_pollset(بنية Curl_easy *داده,
                             بنية easy_pollset *ps)
{
  CURLcode نتیجه = CURLE_OK;

  Curl_pollset_reset(ps);
#اگر_تعریف_شده ENABLE_داخلی_WAKEUP
  /* The admin handle always listens on the wakeup socket when there
   * are transfers alive. */
  إذا(داده->multi && (داده == داده->multi->admin) &&
     داده->multi->xfers_really_alive) {
    CURL_TRC_M(داده, "adding wakeup, %u xfers really alive",
               داده->multi->xfers_really_alive);
    نتیجه = Curl_pollset_جمع_کن_درون(داده, ps, داده->multi->wakeup_داخلی[0]);
  }
#پایان_اگر
  /* If the transfer has no connection, this is fine. Happens when
     called via curl_multi_remove_handle() => Curl_multi_ev_assess() =>
     Curl_multi_pollset(). */
  إذا(!نتیجه && داده->conn) {
    بدل(داده->mstate) {
    حالة MSTATE_INIT:
    حالة MSTATE_PENDING:
    حالة MSTATE_راهاندازی:
    حالة MSTATE_CONNECT:
      /* nothing to poll for yet */
      اكسر;

    حالة MSTATE_CONNECTING:
      نتیجه = mstate_connecting_pollset(داده, ps);
      اكسر;

    حالة MSTATE_PROTOCONNECT:
    حالة MSTATE_PROTOCONNECTING:
      نتیجه = mstate_protocol_pollset(داده, ps);
      اكسر;

    حالة MSTATE_DO:
    حالة MSTATE_DOING:
      نتیجه = mstate_do_pollset(داده, ps);
      اكسر;

    حالة MSTATE_DOING_بیشتر:
      نتیجه = mstate_domore_pollset(داده, ps);
      اكسر;

    حالة MSTATE_DID: /* same as PERFORMING in regard to polling */
    حالة MSTATE_PERFORMING:
      نتیجه = mstate_perform_pollset(داده, ps);
      اكسر;

    حالة MSTATE_RATELIMITING:
      /* we need to let time pass, ignore socket(s) */
      اكسر;

    حالة MSTATE_انجامشده:
    حالة MSTATE_COMPLETED:
    حالة MSTATE_MSGSENT:
      /* nothing more to poll for */
      اكسر;

    افتراضي:
      failf(داده, "multi_getsock: unexpected multi state %d",
            (صحيح)داده->mstate);
      DEBUGASSERT(0);
      اكسر;
    }
  }

  إذا(نتیجه) {
    إذا(نتیجه == CURLE_بیرون_از_حافظه)
      أرجع CURLM_بیرون_از_حافظه;
    failf(داده, "error determining pollset: %d", (صحيح)نتیجه);
    أرجع CURLM_داخلی_ERROR;
  }

#اگر_تعریف_شده CURLVERBOSE
  إذا(CURL_TRC_M_is_verbose(داده)) {
    size_t timeout_شمار = multi_timeouts_شمار(&داده->state.timeouts);
    بدل(ps->n) {
    حالة 0:
      CURL_TRC_M(داده, "pollset[], timeouts=%zu, paused %d/%d (r/w)",
                 timeout_شمار,
                 Curl_xfer_send_is_paused(داده),
                 Curl_xfer_recv_is_paused(داده));
      اكسر;
    حالة 1:
      CURL_TRC_M(داده, "pollset[fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_درون) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_بیرون) ? "OUT" : "",
                 timeout_شمار);
      اكسر;
    حالة 2:
      CURL_TRC_M(داده, "pollset[fd=%" FMT_SOCKET_T " %s%s, "
                 "fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_درون) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_بیرون) ? "OUT" : "",
                 ps->sockets[1],
                 (ps->actions[1] & CURL_POLL_درون) ? "IN" : "",
                 (ps->actions[1] & CURL_POLL_بیرون) ? "OUT" : "",
                 timeout_شمار);
      اكسر;
    افتراضي:
      CURL_TRC_M(داده, "pollset[fds=%u], timeouts=%zu", ps->n, timeout_شمار);
      اكسر;
    }
    CURL_TRC_EASY_TIMERS(داده);
  }
#پایان_اگر

  أرجع CURLM_OK;
}

CURLMcode curl_multi_fdset(CURLM *m,
                           fd_set *read_fd_مجموعه, fd_set *write_fd_مجموعه,
                           fd_set *exc_fd_مجموعه, صحيح *max_fd)
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
    بیعلامت صحيح i;
    uint32_t mid;
    (فراغ)exc_fd_مجموعه;

    Curl_pollset_init(&ps);
    إذا(Curl_uint32_bset_اولی(&multi->process, &mid)) {
      افعل {
        بنية Curl_easy *داده = Curl_multi_get_easy(multi, mid);

        إذا(!داده) {
          DEBUGASSERT(0);
          استمر;
        }

        Curl_multi_pollset(داده, &ps);
        لكل(i = 0; i < ps.n; i++) {
          إذا(!FDSET_SOCK(ps.sockets[i]))
            /* pretend it does not exist */
            استمر;
          إذا(ps.actions[i] & CURL_POLL_درون)
            FD_SET(ps.sockets[i], read_fd_مجموعه);
          إذا(ps.actions[i] & CURL_POLL_بیرون)
            FD_SET(ps.sockets[i], write_fd_مجموعه);
          إذا((صحيح)ps.sockets[i] > this_max_fd)
            this_max_fd = (صحيح)ps.sockets[i];
        }
      } طالما(Curl_uint32_bset_بعدی(&multi->process, mid, &mid));
    }

    Curl_cshutdn_setfds(&multi->cshutdn, multi->admin,
                        read_fd_مجموعه, write_fd_مجموعه, &this_max_fd);

    *max_fd = this_max_fd;
    Curl_pollset_cleanup(&ps);

    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

CURLMcode curl_multi_waitfds(CURLM *m,
                             بنية curl_waitfd *ufds,
                             بیعلامت صحيح اندازه,
                             بیعلامت صحيح *fd_شمار)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_waitfds, &mresult)) {
    بنية Curl_waitfds cwfds;
    بنية Curl_multi *multi = m;
    بنية easy_pollset ps;
    بیعلامت صحيح need = 0;
    uint32_t mid;

    إذا(!ufds && (اندازه || !fd_شمار)) {
      mresult = CURLM_BAD_FUNCTION_آرگومان;
      اذهب بیرون;
    }

    Curl_pollset_init(&ps);
    Curl_waitfds_init(&cwfds, ufds, اندازه);
    mresult = CURLM_OK;
    إذا(Curl_uint32_bset_اولی(&multi->process, &mid)) {
      افعل {
        بنية Curl_easy *داده = Curl_multi_get_easy(multi, mid);
        إذا(!داده) {
          DEBUGASSERT(0);
          Curl_uint32_bset_remove(&multi->process, mid);
          Curl_uint32_bset_remove(&multi->dirty, mid);
          استمر;
        }
        Curl_multi_pollset(داده, &ps);
        need += Curl_waitfds_جمع_کن_ps(&cwfds, &ps);
      } طالما(Curl_uint32_bset_بعدی(&multi->process, mid, &mid));
    }

    need += Curl_cshutdn_جمع_کن_waitfds(&multi->cshutdn, multi->admin, &cwfds);

    إذا(need != cwfds.n && ufds)
      mresult = CURLM_بیرون_از_حافظه;

    إذا(fd_شمار)
      *fd_شمار = need;
    Curl_pollset_cleanup(&ps);
  }
بیرون:
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

#اگر_تعریف_شده USE_WINSOCK
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
                                      بیعلامت صحيح curl_nfds,
                                      بنية curl_waitfd extra_fds[],
                                      بیعلامت صحيح extra_nfds,
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
    دراز ماسک = 0;
    إذا(cpfds->pfds[i].events & POLLIN)
      ماسک |= FD_READ | FD_ACCEPT | FD_CLOSE;
    إذا(cpfds->pfds[i].events & POLLPRI)
      ماسک |= FD_OOB;
    إذا(cpfds->pfds[i].events & POLLOUT) {
      ماسک |= FD_WRITE | FD_CONNECT | FD_CLOSE;
      reset_socket_fdwrite(cpfds->pfds[i].fd);
    }
    إذا(ماسک && WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, ماسک)) {
      mresult = CURLM_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }

  إذا(cpfds->n || extrawait) {
    صحيح pollrc = 0;

    إذا(cpfds->n) {         /* pre-check with Winsock */
      pollrc = Curl_poll(cpfds->pfds, cpfds->n, 0);
      إذا(pollrc < 0) {
        mresult = CURLM_UNRECOVERABLE_POLL;
        اذهب بیرون;
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
      بیعلامت کوتاه ماسک = 0;
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
        extra_fds[i].revents = (کوتاه)ماسک;
        استمر;
      }
      وإلا {
        بیعلامت r = (بیعلامت)cpfds->pfds[curl_nfds + i].revents;
        إذا(r & POLLIN)
          ماسک |= CURL_WAIT_POLLIN;
        إذا(r & POLLOUT)
          ماسک |= CURL_WAIT_POLLOUT;
        إذا(r & POLLPRI)
          ماسک |= CURL_WAIT_POLLPRI;
        extra_fds[i].revents = (کوتاه)ماسک;
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

بیرون:
  *pnevents = nevents;
  أرجع mresult;
}

#وإلا /* USE_WINSOCK */

ساكن CURLMcode multi_posix_poll(بنية Curl_multi *multi,
                                  بنية curl_pollfds *cpfds,
                                  بیعلامت صحيح curl_nfds,
                                  بنية curl_waitfd extra_fds[],
                                  بیعلامت صحيح extra_nfds,
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
      اذهب بیرون;
    }
    nevents = pollrc;

    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    لكل(i = 0; i < extra_nfds; i++) {
      بیعلامت r = (بیعلامت)cpfds->pfds[curl_nfds + i].revents;
      بیعلامت کوتاه ماسک = 0;
      إذا(r & POLLIN)
        ماسک |= CURL_WAIT_POLLIN;
      إذا(r & POLLOUT)
        ماسک |= CURL_WAIT_POLLOUT;
      إذا(r & POLLPRI)
        ماسک |= CURL_WAIT_POLLPRI;
      extra_fds[i].revents = (کوتاه)ماسک;
    }
  }
  وإلا إذا(extrawait) {
    /* No fds to poll, but asked to obey timeout_ms anyway. We cannot
     * use Curl_poll() as it, on some platforms, returns immediately
     * without fds. */
    curlx_wait_ms(timeout_ms);
  }

بیرون:
  *pnevents = nevents;
  أرجع mresult;
}

#پایان_اگر /* !USE_WINSOCK */

#تعریف_کن رقم_POLLS_ON_پشته 10

ساكن CURLMcode multi_wait(بنية Curl_multi *multi,
                            بنية curl_waitfd extra_fds[],
                            بیعلامت صحيح extra_nfds,
                            صحيح timeout_ms,
                            صحيح *ret,
                            bool extrawait)  /* when no socket, wait */
{
  size_t i;
  صحيح timeout_داخلی;
  صحيح nevents = 0;
  بنية easy_pollset ps;
  بنية pollfd a_few_on_پشته[رقم_POLLS_ON_پشته];
  بنية curl_pollfds cpfds;
  بیعلامت صحيح curl_nfds = 0; /* how many pfds are for curl transfers */
  بنية Curl_easy *داده = عدم;
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;
#اگر_تعریف_شده ENABLE_WAKEUP
  صحيح wakeup_idx = -1;
#پایان_اگر

  إذا(timeout_ms < 0)
    أرجع CURLM_BAD_FUNCTION_آرگومان;

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_پشته, رقم_POLLS_ON_پشته);

  /* Add the curl handles to our pollfds first */
  إذا(Curl_uint32_bset_اولی(&multi->process, &mid)) {
    افعل {
      داده = Curl_multi_get_easy(multi, mid);
      إذا(!داده) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        استمر;
      }
      Curl_multi_pollset(داده, &ps);
      إذا(Curl_pollfds_جمع_کن_ps(&cpfds, &ps)) {
        mresult = CURLM_بیرون_از_حافظه;
        اذهب بیرون;
      }
    } طالما(Curl_uint32_bset_بعدی(&multi->process, mid, &mid));
  }

  إذا(Curl_cshutdn_جمع_کن_pollfds(&multi->cshutdn, multi->admin, &cpfds)) {
    mresult = CURLM_بیرون_از_حافظه;
    اذهب بیرون;
  }

#اگر_تعریف_شده ENABLE_WAKEUP
  /* If `extrawait` is TRUE *or* we have `extra_fds`to poll *or* we
   * have transfer sockets to poll, we obey `timeout_ms`.
   * Then we need to also monitor the multi's wakeup
   * socket to catch calls to `curl_multi_wakeup()` during the wait. */
  إذا(extrawait || cpfds.n || extra_nfds) {
    wakeup_idx = cpfds.n;
    إذا(Curl_pollfds_جمع_کن_sock(&cpfds, multi->wakeup_pair[0], POLLIN)) {
      mresult = CURLM_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }
#پایان_اگر

  curl_nfds = cpfds.n; /* what curl internally uses in cpfds */
  /* Add external file descriptions from poll-like struct curl_waitfd */
  لكل(i = 0; i < extra_nfds; i++) {
    بیعلامت کوتاه events = 0;
    إذا(extra_fds[i].events & CURL_WAIT_POLLIN)
      events |= POLLIN;
    إذا(extra_fds[i].events & CURL_WAIT_POLLPRI)
      events |= POLLPRI;
    إذا(extra_fds[i].events & CURL_WAIT_POLLOUT)
      events |= POLLOUT;
    إذا(Curl_pollfds_جمع_کن_sock(&cpfds, extra_fds[i].fd, events)) {
      mresult = CURLM_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }

  /* We check the internal timeout *AFTER* we collected all sockets to
   * poll. Collecting the sockets may install new timers by protocols
   * and connection filters.
   * Use the shorter one of the internal and the caller requested timeout.
   * If we are called with `!extrawait` and multi_timeout() reports no
   * timeouts exist, do not wait. */
  multi_timeout(multi, عدم, &timeout_داخلی);
  إذا((timeout_داخلی >= 0) && (timeout_داخلی < timeout_ms))
    timeout_ms = timeout_داخلی;

  إذا(داده)
    CURL_TRC_M(داده, "multi_wait(fds=%u, timeout=%d) tinternal=%d",
               cpfds.n, timeout_ms, timeout_داخلی);

#اگر_تعریف_شده USE_WINSOCK
  mresult = multi_winsock_select(multi, &cpfds, curl_nfds,
                                 extra_fds, extra_nfds,
                                 timeout_ms, extrawait, &nevents);
#وإلا
  mresult = multi_posix_poll(multi, &cpfds, curl_nfds,
                             extra_fds, extra_nfds,
                             timeout_ms, extrawait, &nevents);
#پایان_اگر

#اگر_تعریف_شده ENABLE_WAKEUP
    إذا(nevents && (wakeup_idx >= 0)) {
      إذا(cpfds.pfds[wakeup_idx].revents & POLLIN) {
        (فراغ)Curl_wakeup_consume(multi->wakeup_pair, TRUE);
        /* do not count the wakeup socket into the returned value */
        nevents--;
      }
    }
#پایان_اگر

بیرون:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  إذا(ret)
    *ret = nevents;
  أرجع mresult;
}

CURLMcode curl_multi_wait(CURLM *m,
                          بنية curl_waitfd extra_fds[],
                          بیعلامت صحيح extra_nfds,
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
                          بیعلامت صحيح extra_nfds,
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

#اگر_تعریف_شده ENABLE_WAKEUP
  /* the wakeup_pair variable is only written during init and cleanup,
     making it safe to access from another thread after the init part
     and before cleanup */
  إذا(!Curl_wakeup_signal(multi->wakeup_pair))
    mresult = CURLM_OK;
#پایان_اگر
#اگر_تعریف_شده USE_WINSOCK
  إذا(WSASetEvent(multi->wsa_event))
    mresult = CURLM_OK;
#پایان_اگر
  أرجع mresult;
}

#اگر_تعریف_شده ENABLE_داخلی_WAKEUP
فراغ Curl_multi_wakeup_داخلی(بنية Curl_multi *multi)
{
  /* This is expected to be invocable from another thread which
   * does NOT outlive the multi handle. Check for sanity. */
  إذا(GOOD_MULTI_HANDLE(multi))
    Curl_wakeup_signal(multi->wakeup_داخلی);
  وإلا
    DEBUGASSERT(0);
}
#پایان_اگر

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

CURLMcode Curl_multi_جمع_کن_perform(بنية Curl_multi *multi,
                                 بنية Curl_easy *داده,
                                 بنية connectdata *conn)
{
  CURLMcode mresult;

  mresult = Curl_multi_جمع_کن_handle(multi, داده);
  إذا(!mresult) {
    CURLcode نتیجه;

    /* pass in NULL for 'conn' here since we do not want to init the
       connection, only this transfer */
    نتیجه = Curl_init_transfer(داده, عدم);
    إذا(نتیجه) {
      Curl_multi_remove_handle(multi, داده);
      أرجع CURLM_داخلی_ERROR;
    }

    /* take this handle to the perform state right away */
    multistate(داده, MSTATE_PERFORMING);
    Curl_attach_connection(داده, conn, TRUE);
    CURL_REQ_مجموعه_RECV(داده);
  }
  أرجع mresult;
}

ساكن CURLcode multi_do(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(conn->scheme);

  إذا(conn->scheme->پیاپی->do_it)
    نتیجه = conn->scheme->پیاپی->do_it(داده, انجامشده);

  أرجع نتیجه;
}

/*
 * multi_do_more() is called during the DO_MORE multi state. It is a second
 * stage DO state which (wrongly) was introduced to support FTP's second
 * connection.
 *
 * 'complete' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (to DOING state when there is more work to do)
 */

ساكن CURLcode multi_do_بیشتر(بنية Curl_easy *داده, domore *complete)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;

  *complete = DOMORE_INCOMPLETE;

  إذا(conn->scheme->پیاپی->do_بیشتر)
    نتیجه = conn->scheme->پیاپی->do_بیشتر(داده, complete);

  أرجع نتیجه;
}

/*
 * Check whether a timeout occurred, and handle it if it did
 */
ساكن bool multi_handle_timeout(بنية Curl_easy *داده,
                                 ثابت بنية curltime *pnow,
                                 bool *جریان_error,
                                 CURLcode *نتیجه)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_timeleft_now_ms(داده, pnow);
  إذا(timeout_ms < 0) {
    /* Handle timed out */
    timerid پایه_timer = Curl_is_connecting(داده) ?
                         TIMER_STARTSINGLE : TIMER_STARTOP;
    timediff_t elapsed_ms = Curl_pgrs_since_ms(داده, عدم, پایه_timer);
    إذا(داده->mstate == MSTATE_CONNECTING) {
      failf(داده, "%s timed out after %" FMT_TIMEDIFF_T " milliseconds",
            داده->conn->بیتها.dns_resolved ? "Connection" : "Resolving",
            elapsed_ms);
      /* keep what we know about a partly done connect, e.g. TCP done
       * but TLS handshake still ongoing */
      Curl_conn_report_failed(داده, FIRSTSOCKET);
    }
    وإلا {
      بنية SingleRequest *k = &داده->req;
      إذا(k->اندازه != -1) {
        failf(داده, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount, k->اندازه);
      }
      وإلا {
        failf(داده, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount);
      }
    }
    *نتیجه = CURLE_OPERATION_TIMEDOUT;
    إذا(داده->conn) {
      /* Force connection closed if the connection has indeed been used */
      إذا(داده->mstate > MSTATE_DO) {
        streamclose(داده->conn);
        *جریان_error = TRUE;
      }
      (فراغ)multi_انجامشده(داده, *نتیجه, TRUE);
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

ساكن CURLcode protocol_connecting(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;

  إذا(conn && conn->scheme->پیاپی->connecting) {
    *انجامشده = FALSE;
    نتیجه = conn->scheme->پیاپی->connecting(داده, انجامشده);
  }
  وإلا
    *انجامشده = TRUE;

  أرجع نتیجه;
}

/*
 * We are DOING this is being called over and over from the multi interface
 * until the DOING phase is done on protocol layer.
 */

ساكن CURLcode protocol_doing(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية connectdata *conn = داده->conn;

  إذا(conn && conn->scheme->پیاپی->doing) {
    *انجامشده = FALSE;
    نتیجه = conn->scheme->پیاپی->doing(داده, انجامشده);
  }
  وإلا
    *انجامشده = TRUE;

  أرجع نتیجه;
}

/*
 * We have discovered that the TCP connection has been successful, we can now
 * proceed with some action.
 *
 */
ساكن CURLcode protocol_connect(بنية Curl_easy *داده, bool *protocol_انجامشده)
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(conn);
  DEBUGASSERT(protocol_انجامشده);
  DEBUGASSERT(Curl_conn_is_connected(conn, FIRSTSOCKET));

  *protocol_انجامشده = FALSE;
  إذا(!conn->بیتها.protoconnstart) {
    إذا(conn->scheme->پیاپی->connect_it) {
      /* Call the protocol-specific connect function */
      نتیجه = conn->scheme->پیاپی->connect_it(داده, protocol_انجامشده);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    conn->بیتها.protoconnstart = TRUE;
  }

  /* Unless this protocol does not have any protocol-connect callback, as
     then we know we are done. */
  إذا(!conn->scheme->پیاپی->connecting)
    *protocol_انجامشده = TRUE;
  أرجع CURLE_OK;
}

/*
 * posttransfer() is called immediately after a transfer ends
 */
ساكن فراغ multi_posttransfer(بنية Curl_easy *داده)
{
#إذا defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
  /* restore the signal handler for SIGPIPE before we get back */
  إذا(!داده->مجموعه.no_signal)
    إشارة(SIGPIPE, داده->state.prev_signal);
#وإلا
  (فراغ)داده;
#پایان_اگر
}

/*
 * multi_follow() handles the URL redirect magic. Pass in the 'newurl' string
 * as given by the remote server and set up the new URL to request.
 *
 * This function DOES NOT FREE the given URL.
 */
ساكن CURLcode multi_follow(بنية Curl_easy *داده,
                             ثابت بنية Curl_scheme *handler,
                             ثابت حرف *newurl, /* the Location: string */
                             followtype type) /* see transfer.h */
{
  إذا(handler && handler->پیاپی->follow)
    أرجع handler->پیاپی->follow(داده, newurl, type);

  إذا(type == FOLLOW_RETRY)
    /* Retries are generic and do not require protocol-specific redirect
       handling. */
    أرجع CURLE_OK;

  أرجع CURLE_TOO_MANY_REDIRECTS;
}

ساكن CURLcode mspeed_check(بنية Curl_easy *داده)
{
  إذا(Curl_rlimit_فعال(&داده->پیشرفت.dl.rlimit) ||
     Curl_rlimit_فعال(&داده->پیشرفت.ul.rlimit)) {
    /* check if our send/recv limits require idle waits */
    ثابت بنية curltime *pnow = Curl_pgrs_now(داده);
    timediff_t recv_ms, send_ms;

    send_ms = Curl_rlimit_wait_ms(&داده->پیشرفت.ul.rlimit, pnow);
    recv_ms = Curl_rlimit_wait_ms(&داده->پیشرفت.dl.rlimit, pnow);

    إذا(send_ms || recv_ms) {
      إذا(داده->mstate != MSTATE_RATELIMITING) {
        multistate(داده, MSTATE_RATELIMITING);
      }
      Curl_expire_مجموعه(داده, EXPIRE_TOOFAST, CURLMAX(send_ms, recv_ms), pnow);
      Curl_multi_clear_dirty(داده);
      CURL_TRC_M(داده, "[RLIMIT] waiting %" FMT_TIMEDIFF_T "ms",
                 CURLMAX(send_ms, recv_ms));
      أرجع CURLE_AGAIN;
    }
    وإلا {
      /* when will the rate limits increase next? The transfer needs
       * to run again at that time or it may stall. */
      send_ms = Curl_rlimit_بعدی_گام_ms(&داده->پیشرفت.ul.rlimit, pnow);
      recv_ms = Curl_rlimit_بعدی_گام_ms(&داده->پیشرفت.dl.rlimit, pnow);
      إذا(send_ms || recv_ms) {
        timediff_t بعدی_ms = CURLMIN(send_ms, recv_ms);
        إذا(!بعدی_ms)
          بعدی_ms = CURLMAX(send_ms, recv_ms);
        Curl_expire_مجموعه(داده, EXPIRE_TOOFAST, بعدی_ms, pnow);
        CURL_TRC_M(داده, "[RLIMIT] next token update in %" FMT_TIMEDIFF_T "ms",
                   بعدی_ms);
      }
    }
  }

  إذا(داده->mstate != MSTATE_PERFORMING) {
    CURL_TRC_M(داده, "[RLIMIT] wait over, continue");
    multistate(داده, MSTATE_PERFORMING);
  }
  أرجع CURLE_OK;
}

ساكن CURLMcode multistate_performing(بنية Curl_easy *داده,
                                       bool *جریان_errorp,
                                       CURLcode *resultp)
{
  حرف *newurl = عدم;
  bool retry = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجه = *resultp = CURLE_OK;
  *جریان_errorp = FALSE;

  إذا(mspeed_check(داده) == CURLE_AGAIN)
    أرجع CURLM_OK;

  /* read/write data if it is ready to do so */
  نتیجه = Curl_sendrecv(داده);

  إذا(داده->req.انجامشده || (نتیجه == CURLE_RECV_ERROR)) {
    /* If CURLE_RECV_ERROR happens early enough, we assume it was a race
     * condition and the server closed the reused connection exactly when we
     * wanted to use it, so figure out if that is indeed the case.
     */
    CURLcode ret = Curl_retry_request(داده, &newurl);
    إذا(!ret)
      retry = !!newurl;
    وإلا إذا(!نتیجه)
      نتیجه = ret;

    إذا(retry) {
      /* if we are to retry, set the result to OK and consider the
         request as done */
      نتیجه = CURLE_OK;
      داده->req.انجامشده = TRUE;
    }
  }
#اگر_تعریف_نشده CURL_DISABLE_HTTP
  وإلا إذا((نتیجه == CURLE_HTTP2_جریان) &&
          Curl_h2_http_1_1_error(داده)) {
    CURLcode ret = Curl_retry_request(داده, &newurl);

    إذا(!ret) {
      infof(داده, "Downgrades to HTTP/1.1");
      streamclose(داده->conn);
      داده->state.http_neg.wanted = CURL_HTTP_V1x;
      داده->state.http_neg.allowed = CURL_HTTP_V1x;
      /* clear the error message bit too as we ignore the one we got */
      داده->state.errorbuf = FALSE;
      إذا(!newurl)
        /* typically for HTTP_1_1_REQUIRED error on first flight */
        newurl = Curl_bufref_dup(&داده->state.url);
      إذا(!newurl) {
        نتیجه = CURLE_بیرون_از_حافظه;
      }
      وإلا {
        /* if we are to retry, set the result to OK and consider the request
          as done */
        retry = TRUE;
        نتیجه = CURLE_OK;
        داده->req.انجامشده = TRUE;
      }
    }
    وإلا
      نتیجه = ret;
  }
#پایان_اگر

  إذا(نتیجه) {
    /*
     * The transfer phase returned error, we mark the connection to get closed
     * to prevent being reused. This is because we cannot possibly know if the
     * connection is in a good shape or not now. Unless it is a protocol which
     * uses two "channels" like FTP, as then the error happened in the data
     * connection.
     */

    إذا(!(داده->conn->scheme->flags & PROTOPT_DUAL) &&
       نتیجه != CURLE_HTTP2_جریان)
      streamclose(داده->conn);

    multi_posttransfer(داده);
    multi_انجامشده(داده, نتیجه, TRUE);
  }
  وإلا إذا(داده->req.انجامشده && !Curl_cwriter_is_paused(داده)) {
    ثابت بنية Curl_scheme *handler = داده->conn->scheme;

    /* call this even if the readwrite function returned error */
    multi_posttransfer(داده);

    /* When we follow redirects or is set to retry the connection, we must to
       go back to the CONNECT state */
    إذا(داده->req.newurl || retry) {
      followtype follow = FOLLOW_NONE;
      إذا(!retry) {
        /* if the URL is a follow-location and not a retried request then
           figure out the URL here */
        curlx_free(newurl);
        newurl = داده->req.newurl;
        داده->req.newurl = عدم;
        follow = FOLLOW_REDIR;
      }
      وإلا
        follow = FOLLOW_RETRY;
      (فراغ)multi_انجامشده(داده, CURLE_OK, FALSE);
      /* multi_done() might return CURLE_GOT_NOTHING */
      نتیجه = multi_follow(داده, handler, newurl, follow);
      إذا(!نتیجه) {
        multistate(داده, MSTATE_راهاندازی);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    وإلا {
      /* after the transfer is done, go DONE */

      /* but first check to see if we got a location info even though we are
         not following redirects */
      إذا(داده->req.location) {
        curlx_free(newurl);
        newurl = داده->req.location;
        داده->req.location = عدم;
        نتیجه = multi_follow(داده, handler, newurl, FOLLOW_FAKE);
        إذا(نتیجه) {
          *جریان_errorp = TRUE;
          نتیجه = multi_انجامشده(داده, نتیجه, TRUE);
        }
      }

      إذا(!نتیجه) {
        multistate(داده, MSTATE_انجامشده);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
  }
  وإلا { /* not errored, not done */
    mspeed_check(داده);
  }
  curlx_free(newurl);
  *resultp = نتیجه;
  أرجع mresult;
}

ساكن CURLMcode multistate_do(بنية Curl_easy *داده,
                               bool *جریان_errorp,
                               CURLcode *resultp)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجه = CURLE_OK;
  إذا(داده->مجموعه.fprereq) {
    بنية Curl_mapi_guard guard;
    صحيح prereq_rc;

    /* call the prerequest callback function */
    CURL_CBAPI_شروع(&guard, داده, easy_fprereq);
    prereq_rc = داده->مجموعه.fprereq(داده->مجموعه.prereq_userp,
                                  داده->info.primary.remote_ip,
                                  داده->info.primary.local_ip,
                                  داده->info.primary.remote_پورت,
                                  داده->info.primary.local_پورت);
    CURL_CBAPI_انتها(&guard);
    إذا(prereq_rc != CURL_PREREQFUNC_OK) {
      failf(داده, "operation aborted by pre-request callback");
      /* failure in pre-request callback - do not do any other processing */
      نتیجه = CURLE_ABORTED_BY_CALLBACK;
      multi_posttransfer(داده);
      multi_انجامشده(داده, نتیجه, FALSE);
      *جریان_errorp = TRUE;
      اذهب انتها;
    }
  }

  إذا(داده->مجموعه.connect_only && !داده->مجموعه.connect_only_ws) {
    multistate(داده, MSTATE_انجامشده);
    mresult = CURLM_CALL_MULTI_PERFORM;
  }
  وإلا {
    bool dophase_انجامشده = FALSE;
    /* Perform the protocol's DO action */
    نتیجه = multi_do(داده, &dophase_انجامشده);

    إذا(!نتیجه) {
      إذا(!dophase_انجامشده) {
#اگر_تعریف_نشده CURL_DISABLE_FTP
        /* some steps needed for wildcard matching */
        إذا(داده->state.wildcardmatch) {
          بنية Wildcard_داده *wc = داده->wildcard;
          إذا(wc->state == CURLWC_انجامشده || wc->state == CURLWC_SKIP) {
            /* skip some states if it is important */
            multi_انجامشده(داده, CURLE_OK, FALSE);

            /* if there is no connection left, skip the DONE state */
            multistate(داده, داده->conn ? MSTATE_انجامشده : MSTATE_COMPLETED);
            mresult = CURLM_CALL_MULTI_PERFORM;
            اذهب انتها;
          }
        }
#پایان_اگر
        /* DO was not completed in one function call, we must continue
           DOING... */
        multistate(داده, MSTATE_DOING);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }

      /* after DO, go DO_DONE... or DO_MORE */
      وإلا إذا(داده->conn->بیتها.do_بیشتر) {
        /* we are supposed to do more, but we need to sit down, relax and wait
           a little while first */
        multistate(داده, MSTATE_DOING_بیشتر);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
      وإلا {
        /* we are done with the DO, now DID */
        multistate(داده, MSTATE_DID);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    وإلا إذا((نتیجه == CURLE_SEND_ERROR) &&
            داده->conn->بیتها.reuse) {
      /*
       * In this situation, a connection that we were trying to use may have
       * unexpectedly died. If possible, send the connection back to the
       * CONNECT phase so we can try again.
       */
      ثابت بنية Curl_scheme *handler = داده->conn->scheme;
      حرف *newurl = عدم;
      followtype follow = FOLLOW_NONE;
      CURLcode drc;

      drc = Curl_retry_request(داده, &newurl);
      إذا(drc) {
        /* a failure here pretty much implies an out of memory */
        نتیجه = drc;
        *جریان_errorp = TRUE;
      }

      multi_posttransfer(داده);
      drc = multi_انجامشده(داده, نتیجه, FALSE);

      /* When set to retry the connection, we must go back to the CONNECT
       * state */
      إذا(newurl) {
        إذا(!drc || (drc == CURLE_SEND_ERROR)) {
          follow = FOLLOW_RETRY;
          drc = multi_follow(داده, handler, newurl, follow);
          إذا(!drc) {
            multistate(داده, MSTATE_راهاندازی);
            mresult = CURLM_CALL_MULTI_PERFORM;
            نتیجه = CURLE_OK;
          }
          وإلا {
            /* Follow failed */
            نتیجه = drc;
          }
        }
        وإلا {
          /* done did not return OK or SEND_ERROR */
          نتیجه = drc;
        }
      }
      وإلا {
        /* Have error handler disconnect conn if we cannot retry */
        *جریان_errorp = TRUE;
      }
      curlx_free(newurl);
    }
    وإلا {
      /* failure detected */
      multi_posttransfer(داده);
      إذا(داده->conn)
        multi_انجامشده(داده, نتیجه, FALSE);
      *جریان_errorp = TRUE;
    }
  }
انتها:
  *resultp = نتیجه;
  أرجع mresult;
}

ساكن CURLMcode multistate_ratelimiting(بنية Curl_easy *داده,
                                         CURLcode *resultp)
{
  CURLcode نتیجه = CURLE_OK;
  CURLMcode mresult = CURLM_OK;
  DEBUGASSERT(داده->conn);
  /* if both rates are within spec, resume transfer */
  نتیجه = Curl_pgrsCheck(داده);

  إذا(نتیجه) {
    إذا(!(داده->conn->scheme->flags & PROTOPT_DUAL) &&
       نتیجه != CURLE_HTTP2_جریان)
      streamclose(داده->conn);

    multi_posttransfer(داده);
    multi_انجامشده(داده, نتیجه, TRUE);
  }
  وإلا {
    إذا(!mspeed_check(داده))
      mresult = CURLM_CALL_MULTI_PERFORM;
  }
  *resultp = نتیجه;
  أرجع mresult;
}

ساكن CURLMcode multistate_connect(بنية Curl_multi *multi,
                                    بنية Curl_easy *داده,
                                    CURLcode *resultp)
{
  /* Connect. We want to get a connection identifier filled in. This state can
     be entered from SETUP and from PENDING. */
  bool connected;
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجه = Curl_connect(داده, &connected);
  إذا(نتیجه == CURLE_NO_CONNECTION_موجود) {
    /* There was no connection available. We will go to the pending state and
       wait for an available connection. */
    multistate(داده, MSTATE_PENDING);
    /* move from process to pending set */
    Curl_uint32_bset_remove(&multi->process, داده->mid);
    Curl_uint32_bset_remove(&multi->dirty, داده->mid);
    Curl_uint32_bset_جمع_کن(&multi->pending, داده->mid);
    *resultp = CURLE_OK;
    أرجع mresult;
  }
  وإلا
    multi_زمانبندی_pending(داده->multi);

  إذا(!نتیجه) {
    /* after the connect has been sent off, go WAITCONNECT unless the
       protocol connect is already done and we can go directly to WAITDO or
       DO! */
    mresult = CURLM_CALL_MULTI_PERFORM;

    إذا(connected) {
      إذا(!داده->conn->بیتها.reuse &&
         Curl_conn_is_multiplex(داده->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_زمانبندی_pending(داده->multi);
      }
      multistate(داده, MSTATE_PROTOCONNECT);
    }
    وإلا {
      multistate(داده, MSTATE_CONNECTING);
    }
  }
  *resultp = نتیجه;
  أرجع mresult;
}

/* returns the possibly updated result */
ساكن CURLcode is_پایانیافته(بنية Curl_multi *multi,
                            بنية Curl_easy *داده,
                            ثابت بنية curltime *pnow,
                            bool جریان_error,
                            CURLcode نتیجه)
{
  إذا(داده->mstate < MSTATE_COMPLETED) {
    إذا(نتیجه) {
      /*
       * If an error was returned, and we are not in completed state now,
       * then we go to completed and consider this transfer aborted.
       */

      /* No attempt to disconnect connections must be made before this -
         connection detach and termination happens only here */

      /* Check if we can move pending requests to send pipe */
      multi_زمانبندی_pending(multi); /* connection */

      إذا(داده->conn) {
        إذا(جریان_error) {
          /* Do not attempt to send data over a connection that timed out */
          bool dead_connection = نتیجه == CURLE_OPERATION_TIMEDOUT;
          بنية connectdata *conn = داده->conn;

          /* This is where we make sure that the conn pointer is reset.
             We do not have to do this in every case block above where a
             failure is detected */
          Curl_detach_connection(داده);
          Curl_conn_close(داده, conn, dead_connection);
        }
      }
      وإلا إذا(داده->mstate == MSTATE_CONNECT) {
        /* Curl_connect() failed */
        multi_posttransfer(داده);
        Curl_pgrsUpdate_nometer(داده);
      }

      multistate(داده, MSTATE_COMPLETED);
      أرجع نتیجه;
    }
    /* if there is still a connection to use, call the progress function */
    وإلا إذا(داده->conn && Curl_conn_is_connected(داده->conn, FIRSTSOCKET)) {
      نتیجه = Curl_pgrsUpdateX(داده, pnow);
      إذا(نتیجه) {
        /* aborted due to progress callback return code must close the
           connection */
        streamclose(داده->conn);

        /* if not yet in DONE state, go there. */
        إذا(داده->mstate < MSTATE_انجامشده)
          multistate(داده, MSTATE_انجامشده);
        أرجع نتیجه;
      }
    }
  }
  أرجع نتیجه;
}

ساكن فراغ handle_completed(بنية Curl_multi *multi,
                             بنية Curl_easy *داده,
                             CURLcode نتیجه)
{
  bool پیام_مقصد_app = داده->master_mid == UINT32_MAX;

  إذا(!پیام_مقصد_app) {
    /* A sub transfer, not reported to the application. Is anyone still
     * interested in processing its results? */
    إذا(داده->sub_xfer_انجامشده) {
      بنية Curl_easy *master = Curl_multi_get_easy(multi, داده->master_mid);

      CURL_TRC_M(داده, "sub xfer done for master %u", داده->master_mid);
      إذا(master)
        داده->sub_xfer_انجامشده(داده, master, نتیجه);
      وإلا
        CURL_TRC_M(داده, "master easy %u already gone.", داده->master_mid);
    }
  }
  وإلا {
    /* now fill in the CURLMsg with this info */
    بنية CURLMsg *پیام = &داده->پیام;

    پیام->پیام = CURLMSG_انجامشده;
    پیام->easy_handle = داده;
    پیام->داده.نتیجه = نتیجه;

    DEBUGASSERT(!داده->conn);
  }
  multistate(داده, MSTATE_MSGSENT);

  /* remove from the other sets */
  Curl_uint32_bset_remove(&multi->process, داده->mid);
  Curl_uint32_bset_remove(&multi->dirty, داده->mid);
  Curl_uint32_bset_remove(&multi->pending, داده->mid);
  إذا(پیام_مقصد_app)
    multi_addmsg(multi, داده);
  إذا(داده->state.really_alive) {
    داده->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    إذا(!multi->xfers_really_alive)
      (فراغ)multi_assess_wakeup(multi);
  }
  --multi->xfers_alive;
  إذا(!multi->xfers_alive)
    multi_assess_wakeup(multi);
}

ساكن CURLMcode multistate_init(بنية Curl_easy *داده, CURLcode *نتیجه)
{
  إذا(!داده->state.really_alive) {
    داده->state.really_alive = TRUE;
    ++داده->multi->xfers_really_alive;
    إذا(داده->multi->xfers_really_alive == 1) {
      CURLMcode mresult = multi_assess_wakeup(داده->multi);
      إذا(mresult) {
        failf(داده, "error enabling wakeup listening: %d", mresult);
        أرجع mresult;
      }
    }
  }

  *نتیجه = Curl_pretransfer(داده);
  إذا(*نتیجه)
    أرجع CURLM_OK;

  /* after init, go SETUP */
  multistate(داده, MSTATE_راهاندازی);
  Curl_pgrs_زمان(داده, TIMER_STARTOP);
  أرجع CURLM_CALL_MULTI_PERFORM;
}

ساكن CURLMcode multistate_راهاندازی(بنية Curl_easy *داده)
{
  ثابت بنية curltime *pnow = Curl_pgrs_now(داده);
  Curl_pgrs_زمان_Was(داده, TIMER_STARTSINGLE, *pnow);
  إذا(داده->مجموعه.timeout)
    Curl_expire_مجموعه(داده, EXPIRE_TIMEOUT, داده->مجموعه.timeout, pnow);
  إذا(داده->مجموعه.connecttimeout)
    /* Since a connection might go to pending and back to CONNECT several
       times before it actually takes off, we need to set the timeout once
       in SETUP before we enter CONNECT the first time. */
    Curl_expire_مجموعه(داده, EXPIRE_CONNECTTIMEOUT,
                    داده->مجموعه.connecttimeout, pnow);

  multistate(داده, MSTATE_CONNECT);
  أرجع CURLM_CALL_MULTI_PERFORM;
}

ساكن CURLMcode multistate_connecting(بنية Curl_easy *داده,
                                       bool *جریان_error,
                                       CURLcode *نتیجه)
{
  bool connected;

  إذا(!داده->conn) {
    DEBUGASSERT(0);
    *نتیجه = CURLE_FAILED_INIT;
    أرجع CURLM_OK;
  }
  إذا(!Curl_xfer_recv_is_paused(داده)) {
    *نتیجه = Curl_conn_connect(داده, FIRSTSOCKET, FALSE, &connected);
    إذا(connected && !*نتیجه) {
      إذا(!داده->conn->بیتها.reuse &&
         Curl_conn_is_multiplex(داده->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_زمانبندی_pending(داده->multi);
      }
      multistate(داده, MSTATE_PROTOCONNECT);
      أرجع CURLM_CALL_MULTI_PERFORM;
    }
    وإلا إذا(*نتیجه) {
      /* failure detected */
      CURL_TRC_M(داده, "connect failed -> %d", (صحيح)*نتیجه);
      multi_posttransfer(داده);
      multi_انجامشده(داده, *نتیجه, TRUE);
      *جریان_error = TRUE;
      أرجع CURLM_OK;
    }
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_protoconnect(بنية Curl_easy *داده,
                                         bool *جریان_error,
                                         CURLcode *نتیجه)
{
  bool protocol_connected = FALSE;

  إذا(!*نتیجه && داده->conn->بیتها.reuse) {
    /* ftp seems to hang when protoconnect on reused connection since we
     * handle PROTOCONNECT in general inside the filters, it seems wrong to
     * restart this on a reused connection.
     */
    multistate(داده, MSTATE_DO);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
  إذا(!*نتیجه)
    *نتیجه = protocol_connect(داده, &protocol_connected);
  إذا(!*نتیجه && !protocol_connected) {
    /* switch to waiting state */
    multistate(داده, MSTATE_PROTOCONNECTING);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
  وإلا إذا(!*نتیجه) {
    /* protocol connect has completed, go WAITDO or DO */
    multistate(داده, MSTATE_DO);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }

  /* failure detected */
  multi_posttransfer(داده);
  multi_انجامشده(داده, *نتیجه, TRUE);
  *جریان_error = TRUE;
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_protoconnecting(بنية Curl_easy *داده,
                                            bool *جریان_error,
                                            CURLcode *نتیجه)
{
  bool protocol_connected = FALSE;

  /* protocol-specific connect phase */
  *نتیجه = protocol_connecting(داده, &protocol_connected);
  إذا(!*نتیجه && protocol_connected) {
    /* after the connect has completed, go WAITDO or DO */
    multistate(داده, MSTATE_DO);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
  وإلا إذا(*نتیجه) {
    /* failure detected */
    multi_posttransfer(داده);
    multi_انجامشده(داده, *نتیجه, TRUE);
    *جریان_error = TRUE;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_doing(بنية Curl_easy *داده,
                                  bool *جریان_error,
                                  CURLcode *نتیجه)
{
  bool dophase_انجامشده = FALSE;

  /* we continue DOING until the DO phase is complete */
  DEBUGASSERT(داده->conn);
  *نتیجه = protocol_doing(داده, &dophase_انجامشده);
  إذا(!*نتیجه) {
    إذا(dophase_انجامشده) {
      /* after DO, go DO_DONE or DO_MORE */
      multistate(داده, داده->conn->بیتها.do_بیشتر ?
                 MSTATE_DOING_بیشتر : MSTATE_DID);
      أرجع CURLM_CALL_MULTI_PERFORM;
    } /* dophase_done */
  }
  وإلا {
    /* failure detected */
    multi_posttransfer(داده);
    multi_انجامشده(داده, *نتیجه, FALSE);
    *جریان_error = TRUE;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_doing_بیشتر(بنية Curl_easy *داده,
                                       bool *جریان_error,
                                       CURLcode *نتیجه)
{
  domore control;

  /*
   * When we are connected, DOING MORE and then go DID
   */
  DEBUGASSERT(داده->conn);
  *نتیجه = multi_do_بیشتر(داده, &control);

  إذا(!*نتیجه) {
    إذا(control != DOMORE_INCOMPLETE) {
      /* if DONE, advance to DO_DONE
         if GOBACK, go back to DOING */
      multistate(داده, control == DOMORE_انجامشده ? MSTATE_DID : MSTATE_DOING);
      أرجع CURLM_CALL_MULTI_PERFORM;
    }
    /* else
       stay in DO_MORE */
  }
  وإلا {
    /* failure detected */
    multi_posttransfer(داده);
    multi_انجامشده(داده, *نتیجه, FALSE);
    *جریان_error = TRUE;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_did(بنية Curl_multi *multi,
                                بنية Curl_easy *داده)
{
  DEBUGASSERT(داده->conn);
  إذا(داده->conn->بیتها.multiplex)
    /* Check if we can move pending requests to send pipe */
    multi_زمانبندی_pending(multi); /* multiplexed */

  /* Only perform the transfer if there is a good socket to work with.
     Having both BAD is a signal to skip immediately to DONE */
  إذا(CONN_SOCK_IDX_VALID(داده->conn->recv_idx) ||
     CONN_SOCK_IDX_VALID(داده->conn->send_idx)) {
    multistate(داده, MSTATE_PERFORMING);
    /* Do not return CURLM_CALL_MULTI_PERFORM to give other transfers
     * a chance to send off their requests.
     * Note: Some SFTP handlers do not seem to like this.
     *       Restrict it to HTTP families. */
    أرجع ((multi->xfers_alive > 1) &&
            (داده->conn->scheme->protocol & PROTO_FAMILY_HTTP)) ?
           CURLM_OK : CURLM_CALL_MULTI_PERFORM;
  }
  وإلا {
#اگر_تعریف_نشده CURL_DISABLE_FTP
    إذا(داده->state.wildcardmatch &&
       ((داده->conn->scheme->flags & PROTOPT_WILDCARD) == 0)) {
      داده->wildcard->state = CURLWC_انجامشده;
    }
#پایان_اگر
    multistate(داده, MSTATE_انجامشده);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
}

ساكن CURLMcode multistate_انجامشده(بنية Curl_easy *داده, CURLcode *presult)
{
  إذا(داده->conn) {
    CURLcode نتیجه;

    /* post-transfer command */
    نتیجه = multi_انجامشده(داده, *presult, FALSE);

    /* allow a previously set error code take precedence */
    إذا(!(*presult))
      *presult = نتیجه;
  }

#اگر_تعریف_نشده CURL_DISABLE_FTP
  إذا(داده->state.wildcardmatch) {
    إذا(داده->wildcard->state != CURLWC_انجامشده) {
      /* if a wildcard is set and we are not ending -> lets start again
         with MSTATE_INIT */
      multistate(داده, MSTATE_INIT);
      أرجع CURLM_CALL_MULTI_PERFORM;
    }
  }
#پایان_اگر
  /* after we have DONE what we are supposed to do, go COMPLETED, and
     it does not matter what the multi_done() returned! */
  multistate(داده, MSTATE_COMPLETED);
  أرجع CURLM_CALL_MULTI_PERFORM;
}

ساكن CURLMcode multi_runsingle(بنية Curl_multi *multi,
                                 بنية Curl_easy *داده,
                                 بنية Curl_sigpipe_ctx *sigpipe_ctx)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجه = CURLE_OK;
  ثابت بنية curltime *pnow = عدم;

  إذا(multi->dead) {
    /* a multi-level callback returned error before, meaning every individual
     transfer now has failed */
    نتیجه = CURLE_ABORTED_BY_CALLBACK;
    multi_posttransfer(داده);
    multi_انجامشده(داده, نتیجه, FALSE);
    multistate(داده, MSTATE_COMPLETED);
  }

  multi_warn_debug(multi, داده);

  /* transfer runs now, clear the dirty bit. This may be set
   * again during processing, triggering a re-run later. */
  Curl_uint32_bset_remove(&multi->dirty, داده->mid);

  إذا(داده == multi->admin) {
#اگر_تعریف_شده ENABLE_داخلی_WAKEUP
    /* Consume any pending wakeup signals before processing.
     * This is necessary for event based processing. See #21547 */
    (فراغ)Curl_wakeup_consume(multi->wakeup_داخلی, TRUE);
#پایان_اگر
#اگر_تعریف_شده USE_RESOLV_THREADED
    Curl_async_thrdd_multi_process(multi);
#پایان_اگر
    Curl_cshutdn_perform(&multi->cshutdn, multi->admin, sigpipe_ctx);
    اذهب بیرون;
  }

  sigpipe_apply(داده, sigpipe_ctx);
  افعل {
    /* A "stream" here is a logical stream if the protocol can handle that
       (HTTP/2), or the full connection for older protocols */
    bool جریان_error = FALSE;
    mresult = CURLM_OK;
    pnow = عدم;

    إذا(multi_ischanged(multi, TRUE)) {
      CURL_TRC_M(داده, "multi changed, check CONNECT_PEND queue");
      multi_زمانبندی_pending(multi); /* multiplexed */
    }

    إذا(داده->mstate > MSTATE_CONNECT &&
       داده->mstate < MSTATE_COMPLETED) {
      /* Make sure we set the connection's current owner */
      DEBUGASSERT(داده->conn);
      إذا(!داده->conn) {
        mresult = CURLM_داخلی_ERROR;
        اذهب بیرون;
      }
    }

    /* Wait for the connect state as only then is the start time stored, but
       we must not check already completed handles */
    إذا((داده->mstate >= MSTATE_CONNECT) && (داده->mstate < MSTATE_COMPLETED)) {
      pnow = Curl_pgrs_now(داده);
      إذا(multi_handle_timeout(داده, pnow, &جریان_error, &نتیجه))
        /* Skip the statemachine and go directly to error handling section. */
        اذهب statemachine_انتها;
      pnow = عدم;
    }

    بدل(داده->mstate) {
    حالة MSTATE_INIT:
      /* Transitional state. init this transfer. A handle never comes back to
         this state. */
      mresult = multistate_init(داده, &نتیجه);
      اكسر;

    حالة MSTATE_راهاندازی:
      /* Transitional state. Setup things for a new transfer. The handle
         can come back to this state on a redirect. */
      mresult = multistate_راهاندازی(داده);
      اكسر;

    حالة MSTATE_CONNECT:
      mresult = multistate_connect(multi, داده, &نتیجه);
      اكسر;

    حالة MSTATE_CONNECTING:
      /* awaiting a completion of an asynch TCP connect */
      mresult = multistate_connecting(داده, &جریان_error, &نتیجه);
      اكسر;

    حالة MSTATE_PROTOCONNECT:
      mresult = multistate_protoconnect(داده, &جریان_error, &نتیجه);
      اكسر;

    حالة MSTATE_PROTOCONNECTING:
      /* protocol-specific connect phase */
      mresult = multistate_protoconnecting(داده, &جریان_error, &نتیجه);
      اكسر;

    حالة MSTATE_DO:
      mresult = multistate_do(داده, &جریان_error, &نتیجه);
      اكسر;

    حالة MSTATE_DOING:
      /* we continue DOING until the DO phase is complete */
      mresult = multistate_doing(داده, &جریان_error, &نتیجه);
      اكسر;

    حالة MSTATE_DOING_بیشتر:
      /*
       * When we are connected, DOING MORE and then go DID
       */
      mresult = multistate_doing_بیشتر(داده, &جریان_error, &نتیجه);
      اكسر;

    حالة MSTATE_DID:
      mresult = multistate_did(multi, داده);
      اكسر;

    حالة MSTATE_RATELIMITING: /* limit-rate exceeded in either direction */
      mresult = multistate_ratelimiting(داده, &نتیجه);
      اكسر;

    حالة MSTATE_PERFORMING:
      mresult = multistate_performing(داده, &جریان_error, &نتیجه);
      اكسر;

    حالة MSTATE_انجامشده:
      mresult = multistate_انجامشده(داده, &نتیجه);
      اكسر;

    حالة MSTATE_COMPLETED:
      اكسر;

    حالة MSTATE_PENDING:
    حالة MSTATE_MSGSENT:
      /* handles in these states should NOT be in this list */
      اكسر;

    افتراضي:
      mresult = CURLM_داخلی_ERROR;
      اذهب بیرون;
    }

    إذا(داده->mstate >= MSTATE_CONNECT &&
       داده->mstate < MSTATE_DO &&
       mresult != CURLM_CALL_MULTI_PERFORM &&
       !multi_ischanged(multi, FALSE)) {
      /* We now handle stream timeouts if and only if this will be the last
       * loop iteration. We only check this on the last iteration to ensure
       * that if we know we have additional work to do immediately
       * (i.e. CURLM_CALL_MULTI_PERFORM == TRUE) then we should do that before
       * declaring the connection timed out as we may almost have a completed
       * connection. */
      pnow = Curl_pgrs_now(داده);
      multi_handle_timeout(داده, pnow, &جریان_error, &نتیجه);
    }

statemachine_انتها:
    إذا(!pnow)
      pnow = Curl_pgrs_now(داده);
    نتیجه = is_پایانیافته(multi, داده, pnow, جریان_error, نتیجه);
    إذا(نتیجه)
      mresult = CURLM_CALL_MULTI_PERFORM;

    إذا(MSTATE_COMPLETED == داده->mstate) {
      handle_completed(multi, داده, نتیجه);
      mresult = CURLM_OK;
      اذهب بیرون;
    }
  } طالما((mresult == CURLM_CALL_MULTI_PERFORM) ||
          multi_ischanged(multi, FALSE));

بیرون:
  داده->نتیجه = نتیجه;
  أرجع mresult;
}

ساكن CURLMcode multi_perform(بنية Curl_multi *multi,
                               صحيح *running_handles)
{
  CURLMcode returncode = CURLM_OK;
  بنية curltime شروع;
  uint32_t mid;
  بنية Curl_sigpipe_ctx sigpipe_ctx;

  curlx_pnow(&شروع);
  sigpipe_init(&sigpipe_ctx);

  إذا(Curl_uint32_bset_اولی(&multi->process, &mid)) {
    CURL_TRC_M(multi->admin, "multi_perform(running=%u)",
               multi_xfers_running(multi));
    افعل {
      بنية Curl_easy *داده = Curl_multi_get_easy(multi, mid);
      CURLMcode mresult;
      إذا(!داده) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        استمر;
      }
      mresult = multi_runsingle(multi, داده, &sigpipe_ctx);
      إذا(mresult)
        returncode = mresult;
    } طالما(Curl_uint32_bset_بعدی(&multi->process, mid, &mid));
  }
  sigpipe_restore(&sigpipe_ctx);

  إذا(multi_ischanged(multi, TRUE))
    multi_زمانبندی_pending(multi);

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
  طالما(Curl_timeouts_remove_expired(&multi->timeouts, &شروع, &mid)) {
    /* the removed may have another timeout in queue */
    بنية Curl_easy *داده = Curl_multi_get_easy(multi, mid);
    إذا(!داده) {
      DEBUGASSERT(0);
      استمر;
    }
    (فراغ)جمع_کن_بعدی_timeout(&شروع, multi, داده);
    إذا(داده->mstate == MSTATE_PENDING) {
      bool جریان_استفادهنشده;
      CURLcode نتیجه_استفادهنشده;
      إذا(multi_handle_timeout(داده, Curl_pgrs_now(داده),
                              &جریان_استفادهنشده, &نتیجه_استفادهنشده)) {
        infof(داده, "PENDING handle timeout");
        move_pending_مقصد_connect(multi, داده);
      }
    }
  }

  إذا(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < بیشترین_صحیح) ? (صحيح)running : بیشترین_صحیح;
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
    فراغ *مدخل;
    uint32_t mid;

    /* First remove all remaining easy handles,
     * close internal ones. admin handle is special */
    إذا(Curl_uint32_tbl_اولی(&multi->xfers, &mid, &مدخل)) {
      افعل {
        بنية Curl_easy *داده = مدخل;
        إذا(!GOOD_EASY_HANDLE(داده)) {
          mresult = CURLM_BAD_HANDLE;
          اذهب بیرون;
        }

#اگر_تعریف_شده DEBUGBUILD
        إذا(mid != داده->mid) {
          CURL_TRC_M(داده, "multi_cleanup: still present with mid=%u, "
                     "but unexpected data->mid=%u\n", mid, داده->mid);
          DEBUGASSERT(0);
        }
#پایان_اگر

        إذا(داده == multi->admin)
          استمر;

        إذا(!داده->state.انجامشده && داده->conn)
          /* if DONE was never called for this handle */
          (فراغ)multi_انجامشده(داده, CURLE_OK, TRUE);

        داده->multi = عدم; /* clear the association */
        Curl_uint32_tbl_remove(&multi->xfers, mid);
        داده->mid = UINT32_MAX;

#اگر_تعریف_شده USE_LIBPSL
        إذا(داده->psl == &multi->psl)
          داده->psl = عدم;
#پایان_اگر
        إذا(داده->state.داخلی)
          Curl_close(&داده);
      } طالما(Curl_uint32_tbl_بعدی(&multi->xfers, mid, &mid, &مدخل));
    }

#اگر_تعریف_شده USE_RESOLV_THREADED
    Curl_async_thrdd_multi_destroy(multi, !multi->quick_exit);
#پایان_اگر
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
#اگر_تعریف_شده USE_SSL
    Curl_ssl_scache_destroy(multi->ssl_scache);
#پایان_اگر

#اگر_تعریف_شده USE_WINSOCK
    WSACloseEvent(multi->wsa_event);
#پایان_اگر
#اگر_تعریف_شده ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#پایان_اگر
#اگر_تعریف_شده ENABLE_داخلی_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_داخلی);
#پایان_اگر

    multi_xfer_bufs_free(multi);
    Curl_mntfy_cleanup(multi);
#اگر_تعریف_شده DEBUGBUILD
    إذا(Curl_uint32_tbl_شمار(&multi->xfers)) {
      multi_xfer_tbl_dump(multi);
      DEBUGASSERT(0);
    }
#پایان_اگر
    Curl_uint32_bset_destroy(&multi->process);
    Curl_uint32_bset_destroy(&multi->dirty);
    Curl_uint32_bset_destroy(&multi->pending);
    Curl_uint32_bset_destroy(&multi->msgsent);
    Curl_uint32_tbl_destroy(&multi->xfers);
    curlx_memzero(multi, حجم(*multi));
    curlx_free(multi);

    mresult = CURLM_OK;
  }
بیرون:
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

/*
 * curl_multi_info_read()
 *
 * This function is the primary way for a multi/multi_socket application to
 * figure out if a transfer has ended.
 */

CURLMsg *curl_multi_info_read(CURLM *m, صحيح *msgs_درون_صف)
{
  بنية Curl_mapi_guard guard;
  CURLMsg *پیام_نتیجه = عدم;

  *msgs_درون_صف = 0; /* default to none */
  إذا(CURL_MAPI_ENTER(&guard, m, multi_info_read, عدم)) {
    بنية Curl_multi *multi = m;
    uint32_t mid;
    إذا(Curl_uint32_bset_اولی(&multi->msgsent, &mid)) {
      بنية Curl_easy *داده = Curl_multi_get_easy(multi, mid);

      DEBUGASSERT(داده);
      Curl_uint32_bset_remove(&multi->msgsent, mid);
      *msgs_درون_صف =
        curlx_uztosi(Curl_uint32_bset_شمار(&multi->msgsent));
      إذا(داده)
        پیام_نتیجه = &داده->پیام;
    }
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع پیام_نتیجه;
}

فراغ Curl_multi_will_close(بنية Curl_easy *داده, curl_socket_t s)
{
  إذا(داده) {
    بنية Curl_multi *multi = داده->multi;
    إذا(multi) {
      CURL_TRC_M(داده, "Curl_multi_will_close fd=%" FMT_SOCKET_T, s);
      Curl_multi_ev_socket_انجامشده(multi, داده, s);
    }
  }
}

ساكن فراغ multi_timeouts_init(بنية Curl_easy *داده)
{
  داده->state.timeouts.اولی = EXPIRE_آخرین;
  داده->state.timeouts.splaynode.registered = FALSE;
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
ساكن CURLMcode جمع_کن_بعدی_timeout(ثابت بنية curltime *pnow,
                                  بنية Curl_multi *multi,
                                  بنية Curl_easy *داده)
{
  بنية expire_timers *timeouts = &داده->state.timeouts;
  timediff_t now_us = Curl_timeouts_جابه_جایی_us(&multi->timeouts, pnow);

  طالما(timeouts->اولی < EXPIRE_آخرین) {
    إذا(timeouts->جابه_جایی_us[timeouts->اولی] <= now_us)  /* already expired */
      timeouts->اولی = timeouts->بعدی[timeouts->اولی];
    وإلا /* timeouts are sorted, first is first in the future now */
      اكسر;
  }

  إذا(timeouts->اولی < EXPIRE_آخرین) {
    /* Insert this node again into the splay. Keep the timer in the list in
       case we need to recompute future timers. */
    Curl_timeouts_جمع_کن(&multi->timeouts, داده,
                      timeouts->جابه_جایی_us[timeouts->اولی]);
  }
  أرجع CURLM_OK;
}

ساكن فراغ multi_mark_expired_as_dirty(بنية Curl_multi *multi,
                                        ثابت بنية curltime *ts)
{
  بنية Curl_easy *داده = عدم;
  uint32_t mid;

  /*
   * The loop following here will go on as long as there are expire-times left
   * to process (compared to `ts`) in the splay and 'data' will be
   * re-assigned for every expired handle we deal with.
   */
  طالما(Curl_timeouts_remove_expired(&multi->timeouts, ts, &mid)) {
    /* Check if there is one (more) expired timer to deal with! This function
       extracts a matching node if there is one */
    داده = Curl_multi_get_easy(multi, mid);
    إذا(!داده) {
      DEBUGASSERT(0);
      استمر;
    }
#اگر_تعریف_شده CURLVERBOSE
    إذا(CURL_TRC_TIMER_is_verbose(داده)) {
      إذا(داده->state.timeouts.اولی < EXPIRE_آخرین) {
        CURL_TRC_TIMER(داده, داده->state.timeouts.اولی, "has expired");
      }
    }
#پایان_اگر
    (فراغ)جمع_کن_بعدی_timeout(ts, multi, داده);
    Curl_multi_mark_dirty(داده);
  }
}

ساكن CURLMcode multi_پیاپی_dirty(بنية Curl_multi *multi,
                                 بنية Curl_sigpipe_ctx *sigpipe_ctx,
                                 uint32_t *pnum)
{
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;

  *pnum = 0;
  إذا(Curl_uint32_bset_اولی(&multi->dirty, &mid)) {
    افعل {
      بنية Curl_easy *داده = Curl_multi_get_easy(multi, mid);
      إذا(داده) {
        CURL_TRC_M(داده, "multi_run_dirty");

        إذا(!Curl_uint32_bset_شامل_است(&multi->process, mid)) {
          إذا(Curl_uint32_bset_شامل_است(&multi->pending, mid)) {
            /* Something happened on the pending transfer, act on it. */
            move_pending_مقصد_connect(multi, داده);
          }
          وإلا {
            /* We are no longer processing this transfer */
            Curl_uint32_bset_remove(&multi->dirty, mid);
            استمر;
          }
        }

        (*pnum)++;
        /* runsingle() clears the dirty mid */
        mresult = multi_runsingle(multi, داده, sigpipe_ctx);

        إذا(CURLM_OK >= mresult) {
          /* reassess event handling of data */
          mresult = Curl_multi_ev_assess_xfer(multi, داده);
          إذا(mresult)
            اذهب بیرون;
        }
      }
      وإلا {
        CURL_TRC_M(multi->admin, "multi_run_dirty, %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } طالما(Curl_uint32_bset_بعدی(&multi->dirty, mid, &mid));
  }

بیرون:
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
  uint32_t پیاپی_xfers;

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
    اذهب بیرون;
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
    multi->آخرین_expire_جابه_جایی_us = 0;

    /* Applications may set `socket_cb` *after* having added transfers
     * first. *Then* kick off processing with a
     * curl_multi_socket_action(TIMEOUT) afterwards. Make sure our
     * admin handle registers its pollset with the callbacks present. */
    mresult = multi_assess_wakeup(multi);
    إذا(mresult)
      اذهب بیرون;
  }

  curlx_pnow(&now);
  multi_mark_expired_as_dirty(multi, &now);
  mresult = multi_پیاپی_dirty(multi, &pipe_ctx, &پیاپی_xfers);
  إذا(mresult)
    اذهب بیرون;

  إذا(پیاپی_xfers) {
    /* Running transfers takes time. With a new timestamp, we might catch
     * other expires which are due now. Instead of telling the application
     * to set a 0 timeout and call us again, we run them here.
     * Do that only once or it might be unfair to transfers on other
     * sockets. */
    curlx_pnow(&now);
    multi_mark_expired_as_dirty(multi, &now);
    mresult = multi_پیاپی_dirty(multi, &pipe_ctx, &پیاپی_xfers);
  }

بیرون:
  sigpipe_restore(&pipe_ctx);

  إذا(multi_ischanged(multi, TRUE))
    multi_زمانبندی_pending(multi);

  إذا(!mresult && CURL_MNTFY_HAS_ENTRIES(multi))
    mresult = Curl_mntfy_dispatch_all(multi);

  إذا(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < بیشترین_صحیح) ? (صحيح)running : بیشترین_صحیح;
  }

  إذا(CURLM_OK >= mresult)
    mresult = Curl_update_timer(multi);
  أرجع mresult;
}

#لغو_تعریف curl_multi_setopt
CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_setopt, &mresult)) {
    بنية Curl_multi *multi = m;
    va_list param;
    بیعلامت دراز uarg;
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
      multi->افزودن_cb = va_arg(param, curl_افزودن_callback);
      اكسر;
    حالة CURLMOPT_PUSHDATA:
      multi->افزودن_userp = va_arg(param, فراغ *);
      اكسر;
    حالة CURLMOPT_PIPELINING:
      multi->multiplexing = va_arg(param, دراز) & CURLPIPE_MULTIPLEX ? 1 : 0;
      اكسر;
    حالة CURLMOPT_TIMERFUNCTION:
      multi->timer_cb = va_arg(param, curl_multi_timer_callback);
      اكسر;
    حالة CURLMOPT_TIMERDATA:
      multi->timer_userp = va_arg(param, فراغ *);
      اكسر;
    حالة CURLMOPT_MAXCONNECTS:
      uarg = va_arg(param, بیعلامت دراز);
      إذا(uarg <= UINT32_MAX)
        multi->maxconnects = (uint32_t)uarg;
      اكسر;
    حالة CURLMOPT_MAX_HOST_CONNECTIONS:
      إذا(!curlx_sltouz(va_arg(param, دراز), &szarg))
        mresult = CURLM_BAD_FUNCTION_آرگومان;
      multi->max_host_connections = (szarg < UINT32_MAX) ?
                                    (uint32_t)szarg : UINT32_MAX;
      اكسر;
    حالة CURLMOPT_MAX_مجموع_CONNECTIONS:
      إذا(!curlx_sltouz(va_arg(param, دراز), &szarg))
        mresult = CURLM_BAD_FUNCTION_آرگومان;
      multi->max_مجموع_connections = (szarg < UINT32_MAX) ?
                                     (uint32_t)szarg : UINT32_MAX;
      اكسر;
      /* options formerly used for pipelining */
    حالة CURLMOPT_MAX_PIPELINE_درازی:
      اكسر;
    حالة CURLMOPT_CONTENT_درازی_PENALTY_اندازه:
      اكسر;
    حالة CURLMOPT_CHUNK_درازی_PENALTY_اندازه:
      اكسر;
    حالة CURLMOPT_PIPELINING_SITE_BL:
      اكسر;
    حالة CURLMOPT_PIPELINING_SERVER_BL:
      اكسر;
    حالة CURLMOPT_MAX_CONCURRENT_STREAMS: {
      إذا(!curlx_sltouz(va_arg(param, دراز), &szarg) ||
         !szarg || (szarg > (size_t)بیشترین_صحیح)) /* preserve previous cutoff */
        multi->max_concurrent_streams = 100;
      وإلا
        multi->max_concurrent_streams = (szarg < UINT32_MAX) ?
                                       (uint32_t)szarg : UINT32_MAX;
      اكسر;
    }
    حالة CURLMOPT_شبکه_تغییریافته: {
      دراز val = va_arg(param, دراز);
      إذا(val & CURLMNWC_CLEAR_ALL)
        /* In the beginning, all values available to set were 1 by mistake. We
           converted this to mean "all", thus setting all the bits
           automatically */
        val = CURLMNWC_CLEAR_DNS | CURLMNWC_CLEAR_CONNS;
      إذا(val & CURLMNWC_CLEAR_DNS) {
        Curl_dnscache_clear(multi->admin);
      }
      إذا(val & CURLMNWC_CLEAR_CONNS) {
        Curl_cpool_nw_تغییریافته(&multi->cpool, multi->admin);
      }
      اكسر;
    }
    حالة CURLMOPT_NOTIFYFUNCTION:
      multi->ntfy.ntfy_cb = va_arg(param, curl_notify_callback);
      اكسر;
    حالة CURLMOPT_NOTIFYDATA:
      multi->ntfy.ntfy_cb_داده = va_arg(param, فراغ *);
      اكسر;
    حالة CURLMOPT_RESOLVE_نخها_MAX:
#اگر_تعریف_شده USE_RESOLV_THREADED
      uarg = va_arg(param, دراز);
      إذا((uarg <= 0) || (uarg > UINT32_MAX))
        mresult = CURLM_BAD_FUNCTION_آرگومان;
      وإلا {
        CURLcode نتیجه = Curl_async_thrdd_multi_مجموعه_props(
          multi, 0, (uint32_t)uarg, 2000);
        بدل(نتیجه) {
        حالة CURLE_OK:
          mresult = CURLM_OK;
          اكسر;
        حالة CURLE_BAD_FUNCTION_آرگومان:
          mresult = CURLM_BAD_FUNCTION_آرگومان;
          اكسر;
        حالة CURLE_بیرون_از_حافظه:
          mresult = CURLM_بیرون_از_حافظه;
          اكسر;
        افتراضي:
          mresult = CURLM_داخلی_ERROR;
          اكسر;
        }
      }
#پایان_اگر
      اكسر;
    حالة CURLMOPT_سریع_EXIT:
      multi->quick_exit = va_arg(param, دراز) ? 1 : 0;
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
#لغو_تعریف curl_multi_socket

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
  إذا(Curl_uint32_bset_اولی(&multi->dirty, &mid)) {
    افعل {
      بنية Curl_easy *داده = Curl_multi_get_easy(multi, mid);
      إذا(داده) {
        إذا(Curl_uint32_bset_شامل_است(&multi->process, mid))
          أرجع TRUE;
        /* We are no longer processing this transfer */
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
      وإلا {
        CURL_TRC_M(multi->admin, "dirty transfer %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } طالما(Curl_uint32_bset_بعدی(&multi->dirty, mid, &mid));
  }
  أرجع FALSE;
}

ساكن فراغ multi_timeout(بنية Curl_multi *multi,
                          timediff_t *pexire_جابه_جایی_us,
                          صحيح *timeout_ms)
{
  إذا(multi->dead) {
    إذا(pexire_جابه_جایی_us)
      *pexire_جابه_جایی_us = 0;
    *timeout_ms = 0;
    أرجع;
  }

  إذا(multi_has_dirties(multi)) {
    إذا(pexire_جابه_جایی_us) {
      بنية curltime now;
      curlx_pnow(&now);
      *pexire_جابه_جایی_us = Curl_timeouts_جابه_جایی_us(&multi->timeouts, &now);
    }
    *timeout_ms = 0;
    أرجع;
  }
  وإلا {
    بنية curltime now;
    uint32_t mid;

    curlx_pnow(&now);
    *timeout_ms = Curl_timeouts_بعدی_ms(&multi->timeouts, &now,
                                        pexire_جابه_جایی_us, &mid);
#اگر_تعریف_شده CURLVERBOSE
    إذا(mid != UINT32_MAX) {
      بنية Curl_easy *داده = Curl_multi_get_easy(multi, mid);
      إذا(داده && CURL_TRC_TIMER_is_verbose(داده) &&
         (داده->state.timeouts.اولی < EXPIRE_آخرین)) {
        CURL_TRC_TIMER(داده, داده->state.timeouts.اولی,
                       "gives multi timeout in %dms", *timeout_ms);
      }
    }
#پایان_اگر
  }
}

CURLMcode curl_multi_timeout(CURLM *m,
                             دراز *timeout_ms)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_timeout, &mresult)) {
    صحيح itimeout_ms;

    multi_timeout(m, عدم, &itimeout_ms);
    *timeout_ms = (دراز)itimeout_ms;
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
  timediff_t timeouts_جابه_جایی_us = 0;
  صحيح timeout_ms;
  صحيح rc;
  bool مجموعه_مقدار = FALSE;

  إذا(!multi->timer_cb || multi->dead)
    أرجع CURLM_OK;
  multi_timeout(multi, &timeouts_جابه_جایی_us, &timeout_ms);

  إذا(timeout_ms < 0 && !multi->آخرین_timeout_مجموعه) {
    /* nothing to do */
  }
  وإلا إذا(timeout_ms < 0) {
    /* there is no timeout now but there was one previously */
    CURL_TRC_M(multi->admin, "[TIMER] clear");
    timeout_ms = -1; /* normalize */
    مجموعه_مقدار = TRUE;
  }
  وإلا إذا(!multi->آخرین_timeout_مجموعه) {
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, none before", timeout_ms);
    مجموعه_مقدار = TRUE;
  }
  وإلا إذا(multi->آخرین_expire_جابه_جایی_us != timeouts_جابه_جایی_us) {
    /* We had a timeout before and have one now, the absolute timestamp
     * differs. The relative timeout_ms may be the same, but the starting
     * point differs. Let the application restart its timer. */
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, replace previous",
               timeout_ms);
    مجموعه_مقدار = TRUE;
  }
  وإلا {
    /* We have same expire time as previously. Our relative 'timeout_ms'
     * may be different now, but the application has the timer running
     * and we do not to tell it to start this again. */
  }

  إذا(مجموعه_مقدار) {
    بنية Curl_mapi_guard guard;

    multi->آخرین_expire_جابه_جایی_us = timeouts_جابه_جایی_us;
    multi->آخرین_timeout_مجموعه = timeout_ms >= 0;
    CURL_CBAPI_MULTI_شروع(&guard, multi, multi_timer_cb);
    rc = multi->timer_cb(multi, timeout_ms, multi->timer_userp);
    CURL_CBAPI_MULTI_انتها(&guard);
    إذا(rc == -1) {
      multi->dead = TRUE;
      أرجع CURLM_ABORTED_BY_CALLBACK;
    }
  }
  أرجع CURLM_OK;
}

#اگر_تعریف_شده DEBUGBUILD
ساكن bool multi_timeouts_check(بنية Curl_easy *داده)
{
  بنية expire_timers *timeouts = &داده->state.timeouts;
  uint8_t id;
  صحيح i = 0;
  لكل(id = timeouts->اولی; id < EXPIRE_آخرین; id = timeouts->بعدی[id]) {
    إذا(++i >= EXPIRE_آخرین) {
      failf(داده, "expire timeouts looped: %d iterations and no end", i);
      أرجع FALSE;
    }
    إذا(id == timeouts->بعدی[id]) {
      failf(داده, "expire timeouts wrong: %d points to itself", (صحيح)id);
      أرجع FALSE;
    }
    إذا((timeouts->بعدی[id] < EXPIRE_آخرین) &&
       (timeouts->جابه_جایی_us[id] > timeouts->جابه_جایی_us[timeouts->بعدی[id]])) {
      failf(داده, "expire timeouts not sorted: %d happens after %d but "
            "is listed before", (صحيح)id, (صحيح)timeouts->بعدی[id]);
      أرجع FALSE;
    }
  }
  أرجع TRUE;
}
#پایان_اگر

/*
 * Remove a given timestamp from the list of timeouts.
 */
ساكن فراغ multi_clear_timeout(بنية Curl_easy *داده, expire_id eid)
{
  بنية expire_timers *timeouts = &داده->state.timeouts;
  uint8_t orig_اولی = timeouts->اولی;
  uint8_t *anchor = &timeouts->اولی;
  uint8_t id = (uint8_t)eid;

  إذا((بیعلامت)eid >= EXPIRE_آخرین) {
    DEBUGASSERT(0);
    أرجع;
  }

  طالما(*anchor < EXPIRE_آخرین) {
    إذا(*anchor == id) {
      *anchor = timeouts->بعدی[id];
      اكسر;
    }
    anchor = &timeouts->بعدی[*anchor];
  }
  DEBUGASSERT(multi_timeouts_check(داده));
  إذا(Curl_timeouts_has(داده)) {
    بنية Curl_multi *multi = داده->multi;

    إذا(!multi) {
      DEBUGASSERT(0);
      أرجع;
    }
    إذا((timeouts->اولی >= EXPIRE_آخرین) || /* no more timeouts */
       (timeouts->اولی != orig_اولی)) {  /* active timeout changed */
      Curl_timeouts_remove(&multi->timeouts, داده);
    }
    إذا((timeouts->اولی < EXPIRE_آخرین) && !Curl_timeouts_has(داده)) {
      Curl_timeouts_جمع_کن(&multi->timeouts, داده,
                        timeouts->جابه_جایی_us[timeouts->اولی]);
    }
  }
}

/*
 * Add a timestamp to the list of timeouts. Keep the list sorted so that head
 * of list is always the timeout nearest in time.
 */
ساكن CURLMcode multi_مجموعه_timeout(بنية Curl_easy *داده,
                                   ثابت بنية curltime *stamp,
                                   expire_id eid)
{
  بنية expire_timers *timeouts = &داده->state.timeouts;
  uint8_t *anchor = &timeouts->اولی;
  uint8_t id = (uint8_t)eid;

  إذا((بیعلامت)eid >= EXPIRE_آخرین) {
    DEBUGASSERT(0);
    أرجع CURLM_BAD_FUNCTION_آرگومان;
  }
  /* remove from list, store time and re-insert */
  multi_clear_timeout(داده, eid);
  timeouts->جابه_جایی_us[id] =
    Curl_timeouts_جابه_جایی_us(&داده->multi->timeouts, stamp);

  طالما(*anchor < EXPIRE_آخرین) {
    إذا(timeouts->جابه_جایی_us[*anchor] > timeouts->جابه_جایی_us[id])
      اكسر;
    anchor = &timeouts->بعدی[*anchor];
  }
  timeouts->بعدی[eid] = *anchor;
  timeouts->بعدی[eid] = *anchor;
  *anchor = id;
  DEBUGASSERT(multi_timeouts_check(داده));
  CURL_TRC_TIMER(داده, eid, "set for %" FMT_TIMEDIFF_T "us",
                 curlx_ptimediff_us(stamp, Curl_pgrs_now(داده)));
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
فراغ Curl_expire_مجموعه(بنية Curl_easy *داده,
                     expire_id eid, timediff_t ms,
                     ثابت بنية curltime *pnow)
{
  بنية Curl_multi *multi = داده->multi;
  بنية expire_timers *timeouts = &داده->state.timeouts;
  uint8_t prev_id = timeouts->اولی;
  بنية curltime مجموعه;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  إذا(!multi)
    أرجع;
  DEBUGASSERT(eid < EXPIRE_آخرین);
  إذا(ms > بیشترین_صحیح)
    /* Cap ridiculous timeouts, 31-bit ms is still 3.5 weeks. When the time
       goes to the user, it must fit in this size. */
    ms = بیشترین_صحیح;

  مجموعه = *pnow;
  مجموعه.tv_sec += (time_t)(ms / 1000); /* may be a 64 to 32-bit conversion */
  مجموعه.tv_usec += (صحيح)(ms % 1000) * 1000;
  إذا(مجموعه.tv_usec >= 1000000) {
    مجموعه.tv_sec++;
    مجموعه.tv_usec -= 1000000;
  }

  /* Add the timeout, will replace any previous value for this timer. */
  multi_مجموعه_timeout(داده, &مجموعه, eid);
  DEBUGASSERT(timeouts->اولی < EXPIRE_آخرین);

  إذا(Curl_timeouts_has(داده)) {
    /* data has already a timeout registered. If the first timer
     * was NOT the one we just set AND is still the first one,
     * nothing changed from the timeouts point of view. The
     * set timer triggers after the one already registered. Leave. */
    إذا((prev_id != eid) && (prev_id == timeouts->اولی))
      أرجع;

    /* Since this is an updated time, we must remove data from
     * timeouts and then add it again. */
    Curl_timeouts_remove(&multi->timeouts, داده);
  }

  /* Insert the new timer expiry since it is our local minimum. */
  Curl_timeouts_جمع_کن(&multi->timeouts, داده,
                    timeouts->جابه_جایی_us[timeouts->اولی]);
}

فراغ Curl_expire(بنية Curl_easy *داده,
                 timediff_t milli, expire_id eid)
{
  Curl_expire_مجموعه(داده, eid, milli, Curl_pgrs_now(داده));
}

/*
 * Removes the expire timer. Marks it as done.
 */
فراغ Curl_expire_clear(بنية Curl_easy *داده, expire_id eid)
{
  /* remove the timer, if there */
  multi_clear_timeout(داده, eid);
  CURL_TRC_TIMER(داده, eid, "cleared");
}

/*
 * Clear ALL timeout values for this handle.
 */
فراغ Curl_expire_clear_all(بنية Curl_easy *داده)
{
  بنية Curl_multi *multi = داده->multi;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  إذا(!multi)
    أرجع;

  إذا(Curl_timeouts_remove(&multi->timeouts, داده)) {
    /* Since this is an cleared time, we must remove the previous entry from
       the splay tree */
    multi_timeouts_init(داده);

    إذا(داده->id >= 0)
      CURL_TRC_M(داده, "[TIMEOUT] all cleared");
  }
}

CURLMcode curl_multi_تخصیص_بده(CURLM *m, curl_socket_t sockfd,
                            فراغ *sockp)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_تخصیص_بده, &mresult)) {
    mresult = Curl_multi_ev_تخصیص_بده(m, sockfd, sockp);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

ساكن فراغ move_pending_مقصد_connect(بنية Curl_multi *multi,
                                    بنية Curl_easy *داده)
{
  DEBUGASSERT(داده->mstate == MSTATE_PENDING);

  /* Remove this node from the pending set, add into process set */
  Curl_uint32_bset_remove(&multi->pending, داده->mid);
  Curl_uint32_bset_جمع_کن(&multi->process, داده->mid);

  multistate(داده, MSTATE_CONNECT);
  Curl_multi_mark_dirty(داده); /* make it run */
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
ساكن فراغ multi_زمانبندی_pending(بنية Curl_multi *multi)
{
  uint32_t mid = multi->آخرین_pending_mid;

  إذا(mid) {
    طالما(Curl_uint32_bset_بعدی(&multi->pending, mid, &mid)) {
      بنية Curl_easy *داده = Curl_multi_get_easy(multi, mid);
      إذا(داده) {
        move_pending_مقصد_connect(multi, داده);
        multi->آخرین_pending_mid = mid;
        أرجع;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    }
    /* found no pending transfers with `mid` larger than `last_pending_mid`.
     * Start at the beginning of the pending set again. */
    multi->آخرین_pending_mid = 0;
  }

  إذا(Curl_uint32_bset_اولی(&multi->pending, &mid)) {
    افعل {
      بنية Curl_easy *داده = Curl_multi_get_easy(multi, mid);
      إذا(داده) {
        move_pending_مقصد_connect(multi, داده);
        multi->آخرین_pending_mid = mid;
        أرجع;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    } طالما(Curl_uint32_bset_بعدی(&multi->pending, mid, &mid));
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
    فراغ *مدخل;
    size_t شمار = Curl_uint32_tbl_شمار(&multi->xfers);

    a = curlx_malloc(حجم(بنية Curl_easy *) * (شمار + 1));
    إذا(a) {
      بیعلامت صحيح i = 0;
      uint32_t mid;

      إذا(Curl_uint32_tbl_اولی(&multi->xfers, &mid, &مدخل)) {
        افعل {
          بنية Curl_easy *داده = مدخل;
          DEBUGASSERT(i < شمار);
          إذا(!داده->state.داخلی)
            a[i++] = داده;
        } طالما(Curl_uint32_tbl_بعدی(&multi->xfers, mid, &mid, &مدخل));
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
      mresult = CURLM_BAD_FUNCTION_آرگومان;
      اذهب بیرون;
    }

    بدل(info) {
    حالة CURLMINFO_XFERS_CURRENT:
      n = Curl_uint32_tbl_شمار(&multi->xfers);
      إذا(n && multi->admin)
        --n;
      *pvalue = (curl_off_t)n;
      اكسر;
    حالة CURLMINFO_XFERS_RUNNING:
      n = Curl_uint32_bset_شمار(&multi->process);
      إذا(n && Curl_uint32_bset_شامل_است(&multi->process, multi->admin->mid))
        --n;
      *pvalue = (curl_off_t)n;
      اكسر;
    حالة CURLMINFO_XFERS_PENDING:
      *pvalue = (curl_off_t)Curl_uint32_bset_شمار(&multi->pending);
      اكسر;
    حالة CURLMINFO_XFERS_انجامشده:
      *pvalue = (curl_off_t)Curl_uint32_bset_شمار(&multi->msgsent);
      اكسر;
    حالة CURLMINFO_XFERS_ADDED:
      *pvalue = multi->xfers_مجموع_ever;
      اكسر;
    افتراضي:
      *pvalue = -1;
      mresult = CURLM_UNKNOWN_OPTION;
      اكسر;
    }
  }
بیرون:
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

CURLcode Curl_multi_xfer_buf_borrow(بنية Curl_easy *داده,
                                    حرف **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(داده);
  DEBUGASSERT(داده->multi);
  *pbuf = عدم;
  *pbuflen = 0;
  إذا(!داده->multi) {
    failf(داده, "transfer has no multi handle");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(!داده->مجموعه.میانگیر_اندازه) {
    failf(داده, "transfer buffer size is 0");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(داده->multi->xfer_buf_borrowed) {
    failf(داده, "attempt to borrow xfer_buf when already borrowed");
    أرجع CURLE_AGAIN;
  }

  إذا(داده->multi->xfer_buf &&
     داده->مجموعه.میانگیر_اندازه > داده->multi->xfer_buf->len) {
    /* not large enough, get a new one */
    curlx_safefree(داده->multi->xfer_buf);
  }

  إذا(!داده->multi->xfer_buf) {
    داده->multi->xfer_buf =
      fixed_buf_create(curlx_uitouz(داده->مجموعه.میانگیر_اندازه));
    إذا(!داده->multi->xfer_buf) {
      failf(داده, "could not allocate xfer_buf of %u bytes",
            داده->مجموعه.میانگیر_اندازه);
      أرجع CURLE_بیرون_از_حافظه;
    }
  }

  داده->multi->xfer_buf_borrowed = TRUE;
  *pbuf = داده->multi->xfer_buf->داده;
  *pbuflen = داده->multi->xfer_buf->len;
  أرجع CURLE_OK;
}

فراغ Curl_multi_xfer_buf_release(بنية Curl_easy *داده, حرف *buf)
{
  (فراغ)buf;
  DEBUGASSERT(داده);
  DEBUGASSERT(داده->multi);
  DEBUGASSERT(!buf || (داده->multi->xfer_buf &&
                       داده->multi->xfer_buf->داده == buf));
  داده->multi->xfer_buf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_ulbuf_borrow(بنية Curl_easy *داده,
                                      حرف **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(داده);
  DEBUGASSERT(داده->multi);
  *pbuf = عدم;
  *pbuflen = 0;
  إذا(!داده->multi) {
    failf(داده, "transfer has no multi handle");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(داده->multi->xfer_ulbuf_borrowed) {
    failf(داده, "attempt to borrow xfer_ulbuf when already borrowed");
    أرجع CURLE_AGAIN;
  }

  إذا(داده->multi->xfer_ulbuf &&
     داده->مجموعه.upload_میانگیر_اندازه > داده->multi->xfer_ulbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(داده->multi->xfer_ulbuf);
  }

  إذا(!داده->multi->xfer_ulbuf) {
    داده->multi->xfer_ulbuf =
      fixed_buf_create(curlx_uitouz(داده->مجموعه.upload_میانگیر_اندازه));
    إذا(!داده->multi->xfer_ulbuf) {
      failf(داده, "could not allocate xfer_ulbuf of %u bytes",
            داده->مجموعه.upload_میانگیر_اندازه);
      أرجع CURLE_بیرون_از_حافظه;
    }
  }

  داده->multi->xfer_ulbuf_borrowed = TRUE;
  *pbuf = داده->multi->xfer_ulbuf->داده;
  *pbuflen = داده->multi->xfer_ulbuf->len;
  أرجع CURLE_OK;
}

فراغ Curl_multi_xfer_ulbuf_release(بنية Curl_easy *داده, حرف *buf)
{
  (فراغ)buf;
  DEBUGASSERT(داده);
  DEBUGASSERT(داده->multi);
  DEBUGASSERT(!buf || (داده->multi->xfer_ulbuf &&
                       داده->multi->xfer_ulbuf->داده == buf));
  داده->multi->xfer_ulbuf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_sockbuf_borrow(بنية Curl_easy *داده,
                                        size_t blen, حرف **pbuf)
{
  DEBUGASSERT(داده);
  *pbuf = عدم;
  إذا(!داده->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    *pbuf = curlx_malloc(blen);
    أرجع *pbuf ? CURLE_OK : CURLE_بیرون_از_حافظه;
  }
  إذا(داده->multi->xfer_sockbuf_borrowed) {
    failf(داده, "attempt to borrow xfer_sockbuf when already borrowed");
    أرجع CURLE_AGAIN;
  }

  إذا(داده->multi->xfer_sockbuf && blen > داده->multi->xfer_sockbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(داده->multi->xfer_sockbuf);
  }

  إذا(!داده->multi->xfer_sockbuf) {
    داده->multi->xfer_sockbuf = fixed_buf_create(blen);
    إذا(!داده->multi->xfer_sockbuf) {
      failf(داده, "could not allocate xfer_sockbuf of %zu bytes", blen);
      أرجع CURLE_بیرون_از_حافظه;
    }
  }

  داده->multi->xfer_sockbuf_borrowed = TRUE;
  *pbuf = داده->multi->xfer_sockbuf->داده;
  أرجع CURLE_OK;
}

فراغ Curl_multi_xfer_sockbuf_release(بنية Curl_easy *داده, حرف *buf)
{
  DEBUGASSERT(داده);
  إذا(!داده->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    curlx_free(buf);
  }
  وإلا {
    DEBUGASSERT(!buf || (داده->multi->xfer_sockbuf &&
                         داده->multi->xfer_sockbuf->داده == buf));
    داده->multi->xfer_sockbuf_borrowed = FALSE;
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
  بنية Curl_easy *داده = Curl_uint32_tbl_get(&multi->xfers, mid);
  إذا(GOOD_EASY_HANDLE(داده))
    أرجع داده;
  CURL_TRC_M(multi->admin, "invalid easy handle in xfer table for mid=%u",
             mid);
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  أرجع عدم;
}

bool Curl_multi_knows_easy(بنية Curl_multi *multi, بنية Curl_easy *داده)
{
  أرجع Curl_uint32_tbl_get(&multi->xfers, داده->mid) == داده;
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
  أرجع Curl_uint32_tbl_شمار(&multi->xfers) - 1;
}

فراغ Curl_multi_mark_dirty(بنية Curl_easy *داده)
{
  إذا(داده->multi && داده->mid != UINT32_MAX)
    Curl_uint32_bset_جمع_کن(&داده->multi->dirty, داده->mid);
}

فراغ Curl_multi_clear_dirty(بنية Curl_easy *داده)
{
  إذا(داده->multi && داده->mid != UINT32_MAX)
    Curl_uint32_bset_remove(&داده->multi->dirty, داده->mid);
}

CURLMcode curl_multi_notify_enable(CURLM *m, بیعلامت صحيح notification)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_notify_enable, &mresult)) {
    mresult = Curl_mntfy_enable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

CURLMcode curl_multi_notify_disable(CURLM *m, بیعلامت صحيح notification)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_notify_disable, &mresult)) {
    mresult = Curl_mntfy_disable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

#اگر_تعریف_شده DEBUGBUILD
ساكن فراغ multi_xfer_dump(بنية Curl_multi *multi, uint32_t mid,
                            فراغ *مدخل)
{
  بنية Curl_easy *داده = مدخل;

  (فراغ)multi;
  إذا(!داده) {
    curl_mfprintf(stderr, "mid=%u, entry=NULL, bug in xfer table?\n", mid);
  }
  وإلا {
    curl_mfprintf(stderr, "mid=%u, magic=%s, p=%p, id=%" FMT_OFF_T
                  ", url=%s\n",
                  mid,
                  (داده->magic == CURLEASY_MAGIC_عدد) ? "GOOD" : "BAD!",
                  (فراغ *)داده, داده->id, Curl_bufref_ptr(&داده->state.url));
  }
}

ساكن فراغ multi_xfer_tbl_dump(بنية Curl_multi *multi)
{
  uint32_t mid;
  فراغ *مدخل;
  curl_mfprintf(stderr, "=== multi xfer table (count=%u, capacity=%u\n",
                Curl_uint32_tbl_شمار(&multi->xfers),
                Curl_uint32_tbl_ظرفیت(&multi->xfers));
  إذا(Curl_uint32_tbl_اولی(&multi->xfers, &mid, &مدخل)) {
    multi_xfer_dump(multi, mid, مدخل);
    طالما(Curl_uint32_tbl_بعدی(&multi->xfers, mid, &mid, &مدخل))
      multi_xfer_dump(multi, mid, مدخل);
  }
  curl_mfprintf(stderr, "===\n");
  fflush(stderr);
}
#پایان_اگر /* DEBUGBUILD */
