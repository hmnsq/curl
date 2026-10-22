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
#تعریف_کرو CURL_XFER_جدول_ناپ 128

/* CURL_SOCKET_HASH_TABLE_SIZE should be a prime number. Increasing it from 97
   to 911 takes on a 32-bit machine 4 x 804 = 3211 more bytes. Still, every
   curl handle takes 6K memory, therefore this 3K are not significant. */
#جے_بنا_تعریف CURL_SOCKET_HASH_جدول_ناپ
#تعریف_کرو CURL_SOCKET_HASH_جدول_ناپ 911
#جے_انت

#جے_بنا_تعریف CURL_CONNECTION_HASH_ناپ
#تعریف_کرو CURL_CONNECTION_HASH_ناپ 97
#جے_انت

#جے_بنا_تعریف CURL_DNS_HASH_ناپ
#تعریف_کرو CURL_DNS_HASH_ناپ 71
#جے_انت

#جے_بنا_تعریف CURL_TLS_SESSION_ناپ
#تعریف_کرو CURL_TLS_SESSION_ناپ 25
#جے_انت

ساكن فراغ move_pending_منزل_connect(بنية Curl_multi *multi,
                                    بنية Curl_easy *انکڑے);
ساكن CURLMcode جوڑو_اگلا_timeout(ثابت بنية curltime *pnow,
                                  بنية Curl_multi *multi,
                                  بنية Curl_easy *انکڑے);
ساكن فراغ multi_timeout(بنية Curl_multi *multi,
                          timediff_t *pexire_وتھ_us,
                          صحيح *timeout_ms);
ساكن فراغ multi_ویلا_سارنی_pending(بنية Curl_multi *multi);
ساكن فراغ multi_xfer_bufs_free(بنية Curl_multi *multi);
#جے_تعریف_شدہ DEBUGBUILD
ساكن فراغ multi_xfer_tbl_dump(بنية Curl_multi *multi);
#جے_انت
/* Get the # of transfers current in process/pending. */
ساكن uint32_t multi_xfers_running(بنية Curl_multi *multi);

/* function pointer called once when entering a state */
تعريفنوع فراغ (*mstate_enter_func)(بنية Curl_easy *انکڑے,
                                  CURLMstate from_state);

ساكن فراغ mstate_enter_connect(بنية Curl_easy *انکڑے,
                                 CURLMstate from_state)
{
  (فراغ)from_state;
  Curl_init_CONNECT(انکڑے);
}

ساكن فراغ mstate_enter_did(بنية Curl_easy *انکڑے,
                             CURLMstate from_state)
{
  (فراغ)from_state;
  انکڑے->req.chunk = FALSE;
  Curl_pgrs_ویلا(انکڑے, TIMER_PRETRANSFER);
  إذا(!CURL_REQ_WANT_SEND(انکڑے))
    Curl_pgrs_ویلا(انکڑے, TIMER_POSTRANSFER);
}

ساكن فراغ mstate_enter_پورا(بنية Curl_easy *انکڑے,
                              CURLMstate from_state)
{
  (فراغ)from_state;
  CURLM_NTFY(انکڑے, CURLMNOTIFY_EASY_پورا);
}

ساكن فراغ mstate_enter_completed(بنية Curl_easy *انکڑے,
                                   CURLMstate from_state)
{
  /* we sometimes directly jump to COMPLETED, trigger things
   * we then missed. */
  إذا(from_state < MSTATE_DID) {
    Curl_pgrs_ویلا(انکڑے, TIMER_PRETRANSFER);
    Curl_pgrs_ویلا(انکڑے, TIMER_POSTRANSFER);
    Curl_pgrs_ویلا(انکڑے, TIMER_STARTTRANSFER);
  }
  /* multi_done() should have run and detached any connection already */
  DEBUGASSERT(!انکڑے->conn);
  Curl_pgrsCompleted(انکڑے);
  إذا(from_state < MSTATE_پورا)
    CURLM_NTFY(انکڑے, CURLMNOTIFY_EASY_پورا);
  /* changing to COMPLETED means it is in process and needs to go */
  DEBUGASSERT(Curl_uint32_bset_شامل_اے(&انکڑے->multi->process, انکڑے->mid));
  Curl_uint32_bset_remove(&انکڑے->multi->process, انکڑے->mid);
  Curl_uint32_bset_remove(&انکڑے->multi->pending, انکڑے->mid); /* to be sure */

  إذا(Curl_uint32_bset_خالی_تھاواں(&انکڑے->multi->process)) {
    /* free the transfer buffer when we have no more active transfers */
    multi_xfer_bufs_free(انکڑے->multi);
  }
  /* Important: reset the conn pointer so that we do not point to memory
     that could be freed anytime */
  Curl_detach_connection(انکڑے);
  Curl_expire_clear_all(انکڑے); /* stop all timers */
}

/* always use this function to change state, to make debugging easier */
ساكن فراغ mstate(بنية Curl_easy *انکڑے, CURLMstate state
#جے_تعریف_شدہ DEBUGBUILD
                   , صحيح lineno
#جے_انت
)
{
  CURLMstate oldstate = انکڑے->mstate;
  ساكن ثابت mstate_enter_func state_enter[MSTATE_آخری] = {
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
    mstate_enter_پورا,         /* DONE */
    mstate_enter_completed,    /* COMPLETED */
    عدم                       /* MSGSENT */
  };

  إذا(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    أرجع;

#جے_تعریف_شدہ DEBUGBUILD
  NOVERBOSE((فراغ)lineno);
  CURL_TRC_M(انکڑے, "-> [%s] (line %d)", CURL_MSTATE_NAME(state), lineno);
#وإلا
  CURL_TRC_M(انکڑے, "-> [%s]", CURL_MSTATE_NAME(state));
#جے_انت

  /* really switching state */
  انکڑے->mstate = state;
  إذا(state_enter[state])
    state_enter[state](انکڑے, oldstate);
}

#جے_بنا_تعریف DEBUGBUILD
#تعریف_کرو multistate(x, y) mstate(x, y)
#وإلا
#تعریف_کرو multistate(x, y) mstate(x, y, __LINE__)
#جے_انت

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
ساكن فراغ multi_addmsg(بنية Curl_multi *multi, بنية Curl_easy *انکڑے)
{
  إذا(Curl_uint32_bset_خالی_تھاواں(&multi->msgsent))
    CURLM_NTFY(multi->admin, CURLMNOTIFY_INFO_READ);
  Curl_uint32_bset_جوڑو(&multi->msgsent, انکڑے->mid);
}

ساكن فراغ multi_timeouts_init(بنية Curl_easy *انکڑے);

بنية Curl_multi *Curl_multi_handle(uint32_t xfer_جدول_ناپ,
                                     size_t ev_hashsize,  /* event hash */
                                     size_t chashsize, /* connection hash */
                                     size_t dnssize,   /* dns hash */
                                     size_t sesssize)  /* TLS session cache */
{
  بنية curltime created;
  بنية Curl_multi *multi = curlx_calloc(1, حجم(بنية Curl_multi));

  إذا(!multi)
    أرجع عدم;

  multi->magic = CURLMULTI_MAGIC_نمبر;

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
  multi->admin->state.اندرونی = TRUE;

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
  Curl_hash_init(&multi->proto_hash, 23, CURL_HASH_TYPE_بائٹاں, ph_freeentry);
  Curl_cshutdn_init(&multi->cshutdn);

  multi->multiplexing = TRUE;
  multi->max_concurrent_streams = 100;
#جے_تعریف_شدہ ENABLE_WAKEUP
  multi->wakeup_pair[0] = CURL_SOCKET_BAD;
  multi->wakeup_pair[1] = CURL_SOCKET_BAD;
#جے_انت
#جے_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  multi->wakeup_اندرونی[0] = CURL_SOCKET_BAD;
  multi->wakeup_اندرونی[1] = CURL_SOCKET_BAD;
#جے_انت

  إذا(Curl_uint32_bset_resize(&multi->process, xfer_جدول_ناپ) ||
     Curl_uint32_bset_resize(&multi->pending, xfer_جدول_ناپ) ||
     Curl_uint32_bset_resize(&multi->dirty, xfer_جدول_ناپ) ||
     Curl_uint32_bset_resize(&multi->msgsent, xfer_جدول_ناپ) ||
     Curl_uint32_tbl_resize(&multi->xfers, xfer_جدول_ناپ))
    اذهب error;

#جے_تعریف_شدہ DEBUGBUILD
  {
    ثابت حرف *p;
    إذا(getenv("CURL_DEBUG"))
      multi->admin->سیٹ.verbose = TRUE;
    p = getenv("CURL_DBG_MAX_TOTAL_CONNECTIONS");
    إذا(p) {
      curl_off_t l;
      إذا(!curlx_str_نمبر(&p, &l, UINT32_MAX)) {
        multi->max_کل_connections = (uint32_t)l;
      }
    }
  }
#جے_انت
  Curl_uint32_tbl_جوڑو(&multi->xfers, multi->admin, &multi->admin->mid);
  Curl_uint32_bset_جوڑو(&multi->process, multi->admin->mid);

  Curl_cpool_init(&multi->cpool, عدم, chashsize);

#جے_تعریف_شدہ USE_SSL
  إذا(Curl_ssl_scache_create(sesssize, 2, &multi->ssl_scache))
    اذهب error;
#وإلا
  (فراغ)sesssize;
#جے_انت

#جے_تعریف_شدہ USE_WINSOCK
  multi->wsa_event = WSACreateEvent();
  إذا(multi->wsa_event == WSA_INVALID_EVENT)
    اذهب error;
#جے_انت
#جے_تعریف_شدہ ENABLE_WAKEUP
  /* When enabled, rely on this to work. We ignore this in previous
   * versions, but that seems an unnecessary complication. */
  إذا(Curl_wakeup_init(multi->wakeup_pair, TRUE) < 0)
    اذهب error;
#جے_انت
#جے_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  إذا(Curl_wakeup_init(multi->wakeup_اندرونی, TRUE) < 0)
    اذهب error;
#جے_انت

  إذا(Curl_probeipv6(multi))
    اذهب error;

#جے_تعریف_شدہ USE_RESOLV_THREADED
  إذا(xfer_جدول_ناپ < CURL_XFER_جدول_ناپ) { /* easy multi */
    إذا(Curl_async_thrdd_multi_init(multi, 0, 2, 10))
      اذهب error;
  }
  وإلا { /* real multi handle */
    إذا(Curl_async_thrdd_multi_init(multi, 0, 20, 2000))
      اذهب error;
  }
#جے_انت

  أرجع multi;

error:

#جے_تعریف_شدہ USE_RESOLV_THREADED
  Curl_async_thrdd_multi_destroy(multi, TRUE);
#جے_انت
  Curl_multi_ev_cleanup(multi);
  Curl_hash_destroy(&multi->proto_hash);
  Curl_dnscache_destroy(&multi->dnscache);
  Curl_cpool_destroy(&multi->cpool, multi->admin);
  Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
#جے_تعریف_شدہ USE_SSL
  Curl_ssl_scache_destroy(multi->ssl_scache);
#جے_انت
  إذا(multi->admin) {
    Curl_multi_ev_xfer_پورا(multi, multi->admin);
    multi->admin->multi = عدم;
    Curl_close(&multi->admin);
  }
  Curl_mntfy_cleanup(multi);

  Curl_uint32_bset_destroy(&multi->process);
  Curl_uint32_bset_destroy(&multi->dirty);
  Curl_uint32_bset_destroy(&multi->pending);
  Curl_uint32_bset_destroy(&multi->msgsent);
  Curl_uint32_tbl_destroy(&multi->xfers);
#جے_تعریف_شدہ ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#جے_انت
#جے_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_اندرونی);
#جے_انت

  curlx_free(multi);
  أرجع عدم;
}

CURLM *curl_multi_init(فراغ)
{
  أرجع Curl_multi_handle(CURL_XFER_جدول_ناپ,
                           CURL_SOCKET_HASH_جدول_ناپ,
                           CURL_CONNECTION_HASH_ناپ,
                           CURL_DNS_HASH_ناپ,
                           CURL_TLS_SESSION_ناپ);
}

#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
ساكن فراغ multi_warn_debug(بنية Curl_multi *multi, بنية Curl_easy *انکڑے)
{
  إذا(!multi->warned) {
    infof(انکڑے, "!!! WARNING !!!");
    infof(انکڑے, "This is a debug build of libcurl, "
                "do not use in production.");
    multi->warned = TRUE;
  }
}
#وإلا
#تعریف_کرو multi_warn_debug(x, y) Curl_nop_stmt
#جے_انت

bool Curl_is_connecting(بنية Curl_easy *انکڑے)
{
  أرجع انکڑے->mstate < MSTATE_DO;
}

ساكن CURLMcode multi_assess_wakeup(بنية Curl_multi *multi)
{
#جے_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  إذا(multi->socket_cb)
    أرجع Curl_multi_ev_assess_xfer(multi, multi->admin);
#وإلا
  (فراغ)multi;
#جے_انت
  أرجع CURLM_OK;
}

ساكن CURLMcode multi_xfers_جوڑو(بنية Curl_multi *multi,
                                 بنية Curl_easy *انکڑے)
{
  uint32_t سمرتھا = Curl_uint32_tbl_سمرتھا(&multi->xfers);
  uint32_t new_ناپ = 0;
  /* Prepare to make this into a CURLMOPT_MAX_TRANSFERS, because some
   * applications may want to prevent a run-away of their memory use. */
  /* UINT_MAX is our "invalid" id, do not let the table grow up to that. */
  ثابت uint32_t max_سمرتھا = UINT_MAX - 1;

  إذا(سمرتھا < max_سمرتھا) {
    /* We want `multi->xfers` to have "sufficient" free rows, so that we do
     * not have to reuse the `mid` from a removed easy right away.
     * Check if an 8th of the capacity is still free */
    uint32_t ورتیا = Curl_uint32_tbl_گنتی(&multi->xfers);
    uint32_t بن_ورتیا = سمرتھا - ورتیا;
    uint32_t min_بن_ورتیا = CURLMAX(سمرتھا >> 3, 4);
    إذا(بن_ورتیا < min_بن_ورتیا) {
      /* Grow by 50% of current capacity, in range of [128, 2048],
       * which means the table grows max by 16kb on 64-bit arch. */
      uint32_t growth = CURLMIN(CURLMAX(سمرتھا >> 1, 128), 2048);
      /* Make sure the uint arithmetic here works on the corner
       * cases where we are close to max_capacity or UINT_MAX */
      إذا((max_سمرتھا - growth) <= سمرتھا)
        new_ناپ = max_سمرتھا;
      وإلا
        new_ناپ = سمرتھا + growth;
    }
  }

  إذا(new_ناپ > سمرتھا) {
    /* Grow the bitsets first. Should one fail, we do not need
     * to downsize the already resized ones. The sets continue
     * to work properly when larger than the table, but not
     * the other way around. */
    CURL_TRC_M(انکڑے, "increasing xfer table size to %u", new_ناپ);
    إذا(Curl_uint32_bset_resize(&multi->process, new_ناپ) ||
       Curl_uint32_bset_resize(&multi->dirty, new_ناپ) ||
       Curl_uint32_bset_resize(&multi->pending, new_ناپ) ||
       Curl_uint32_bset_resize(&multi->msgsent, new_ناپ) ||
       Curl_uint32_tbl_resize(&multi->xfers, new_ناپ))
      أرجع CURLM_باہرلا_دا_حافظہ;
  }

  /* Insert the easy into the table now */
  إذا(!Curl_uint32_tbl_جوڑو(&multi->xfers, انکڑے, &انکڑے->mid)) {
    /* MUST only happen when table is full */
    DEBUGASSERT(Curl_uint32_tbl_سمرتھا(&multi->xfers) <=
                Curl_uint32_tbl_گنتی(&multi->xfers));
    أرجع CURLM_باہرلا_دا_حافظہ;
  }
  أرجع CURLM_OK;
}

CURLMcode Curl_multi_جوڑو_handle(بنية Curl_multi *multi,
                                بنية Curl_easy *انکڑے)
{
  CURLMcode mresult;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  إذا(انکڑے->multi)
    أرجع CURLM_ADDED_ALREADY;

  إذا(multi->dead) {
    /* a "dead" handle cannot get added transfers while any existing easy
       handles are still alive - but if there are none alive anymore, it is
       fine to start over and unmark the "deadness" of this handle.
       This means only the admin handle MUST be present. */
    إذا((Curl_uint32_tbl_گنتی(&multi->xfers) != 1) ||
       !Curl_uint32_tbl_شامل_اے(&multi->xfers, 0))
      أرجع CURLM_ABORTED_BY_CALLBACK;
    multi->dead = FALSE;
    Curl_uint32_bset_clear(&multi->process);
    Curl_uint32_bset_clear(&multi->dirty);
    Curl_uint32_bset_clear(&multi->pending);
    Curl_uint32_bset_clear(&multi->msgsent);
  }

  إذا(انکڑے->multi_easy) {
    /* if this easy handle was previously used for curl_easy_perform(), there
       is a private multi handle here that we can kill */
    curl_multi_cleanup(انکڑے->multi_easy);
    انکڑے->multi_easy = عدم;
  }

  /* Insert the easy into the multi->xfers table, assigning it a `mid`. */
  إذا(multi_xfers_جوڑو(multi, انکڑے))
    أرجع CURLM_باہرلا_دا_حافظہ;

  /* Initialize timeouts for this handle */
  multi_timeouts_init(انکڑے);

  /*
   * No failure allowed in this function beyond this point. No modification
   * of easy nor multi handle allowed before this except for potential
   * multi's connection pool growing which will not be undone in this
   * function no matter what.
   */
  إذا(انکڑے->سیٹ.errorbuffer)
    انکڑے->سیٹ.errorbuffer[0] = 0;

  انکڑے->state.os_errno = 0;

  /* make the Curl_easy refer back to this multi handle - before
     Curl_expire() is called. */
  انکڑے->multi = multi;

  /* set the easy handle */
  multistate(انکڑے, MSTATE_INIT);
  /* not yet passed INIT state */
  انکڑے->state.really_alive = FALSE;

#جے_تعریف_شدہ USE_LIBPSL
  /* Do the same for PSL. */
  إذا(انکڑے->share && (انکڑے->share->specifier & (1 << CURL_جندرا_انکڑے_PSL)))
    انکڑے->psl = &انکڑے->share->psl;
  وإلا
    انکڑے->psl = &multi->psl;
#جے_انت

  /* add the easy handle to the process set */
  Curl_uint32_bset_جوڑو(&multi->process, انکڑے->mid);
  ++multi->xfers_alive;
  ++multi->xfers_کل_ever;

  Curl_cpool_xfer_init(انکڑے);
  multi_warn_debug(multi, انکڑے);

  /* Make sure the new handle will run */
  Curl_multi_mark_dirty(انکڑے);

  /* Necessary in event based processing, where dirty handles trigger
   * a timeout callback invocation. */
  mresult = Curl_update_timer(multi);
  إذا(mresult) {
    /* undo the additions above */
    Curl_uint32_bset_remove(&multi->process, انکڑے->mid);
    Curl_uint32_bset_remove(&multi->dirty, انکڑے->mid);
    --multi->xfers_alive;
    انکڑے->multi = عدم; /* not anymore */
    Curl_uint32_tbl_remove(&multi->xfers, انکڑے->mid);
    انکڑے->mid = UINT32_MAX;
    أرجع mresult;
  }

  /* The admin handle only ever has default timeouts set. To improve the
     state somewhat we clone the timeouts from each added handle so that the
     admin handle always has the same timeouts as the most recently added
     easy handle. */
  multi->admin->سیٹ.timeout = انکڑے->سیٹ.timeout;
  multi->admin->سیٹ.server_response_timeout =
    انکڑے->سیٹ.server_response_timeout;
  multi->admin->سیٹ.no_signal = انکڑے->سیٹ.no_signal;

  CURL_TRC_M(انکڑے, "added to multi, mid=%u, running=%u, total=%u",
             انکڑے->mid, multi_xfers_running(multi),
             Curl_uint32_tbl_گنتی(&multi->xfers));
  أرجع CURLM_OK;
}

CURLMcode curl_multi_جوڑو_handle(CURLM *m, CURL *curl)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_جوڑو_handle, &mresult)) {
    بنية Curl_easy *انکڑے = curl;
    /* Verify that we got a somewhat good easy handle too */
    إذا(!GOOD_EASY_HANDLE(انکڑے) || انکڑے->state.اندرونی)
      mresult = CURLM_BAD_EASY_HANDLE;
    وإلا
      mresult = Curl_multi_جوڑو_handle(m, انکڑے);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

بنية multi_پورا_ctx {
  ثنائی_ہندسہ(premature);
};

ساكن bool multi_conn_should_close(بنية connectdata *conn,
                                    بنية Curl_easy *انکڑے,
                                    bool premature)
{
  /* if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of everything else. */
  إذا(conn->بٹ.close)
    أرجع TRUE;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM/NEGOTIATE authentication handshake. */
  إذا(انکڑے->سیٹ.reuse_forbid
#جے_تعریف_شدہ USE_NTLM
     && !(conn->http_ntlm_state == NTLMSTATE_TYPE2 ||
          conn->proxy_ntlm_state == NTLMSTATE_TYPE2)
#جے_انت
#جے_تعریف_شدہ USE_SPNEGO
     && !(conn->http_negotiate_state == CURL_NEGO_AUTH_RECV ||
          conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)
#جے_انت
    )
    أرجع TRUE;

  /* Unless this connection is for a "connect-only" transfer, it
   * needs to be closed if the protocol handler does not support reuse. */
  إذا(!انکڑے->سیٹ.connect_only && conn->scheme &&
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

ساكن cpool_do_نتیجہ multi_پورا_locked(بنية Curl_easy *انکڑے,
                                         بنية connectdata *conn,
                                         فراغ *userdata,
                                         ثابت بنية curltime *pnow)
{
  بنية multi_پورا_ctx *mdctx = userdata;

  (فراغ)pnow;
  CURL_TRC_M(انکڑے, "multi_done_locked, in use=%u", conn->attached_xfers);
  إذا(CONN_INUSE(conn)) {
    /* Stop if still used. */
    CURL_TRC_M(انکڑے, "Connection still in use %u, no more multi_done now!",
               conn->attached_xfers);
    أرجع CPOOL_DO_KEEP;
  }

  إذا(multi_conn_should_close(conn, انکڑے, (bool)mdctx->premature)) {
    CURL_TRC_M(انکڑے, "multi_done, terminating conn #%" FMT_OFF_T " to %s:%u, "
               "forbid=%d, close=%d, premature=%d, conn_multiplex=%d",
               conn->connection_id, conn->origin->user_hostname,
               conn->origin->پورٹ,
               انکڑے->سیٹ.reuse_forbid, conn->بٹ.close, mdctx->premature,
               Curl_conn_is_multiplex(conn, FIRSTSOCKET));
    connclose(conn);
    أرجع mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  وإلا إذا(!Curl_conn_get_max_concurrent(انکڑے, conn, FIRSTSOCKET)) {
    CURL_TRC_M(انکڑے, "multi_done, conn #%" FMT_OFF_T " to %s:%u was shutdown"
               " by server, not reusing", conn->connection_id,
               conn->origin->user_hostname, conn->origin->پورٹ);
    connclose(conn);
    أرجع mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  وإلا {
    أرجع CPOOL_DO_IDLE;
  }
}

ساكن CURLcode multi_پورا(بنية Curl_easy *انکڑے,
                           CURLcode حالت,  /* an error if this is called
                                                after an error was detected */
                           bool premature)
{
  CURLcode نتیجہ;
  بنية connectdata *conn = انکڑے->conn;
  ثابت بنية curltime *pnow = Curl_pgrs_now(انکڑے);

  CURL_TRC_M(انکڑے, "multi_done: status: %d prem: %d done: %d",
             (صحيح)حالت, (صحيح)premature, انکڑے->state.پورا);

  إذا(انکڑے->state.پورا) {
    /* Stop if multi_done() has already been called */
    أرجع CURLE_OK;
  }
  انکڑے->state.پورا = TRUE; /* called now! */

  /* Shut down any ongoing async resolver operation. */
  Curl_resolv_shutdown_all(انکڑے);

  /* Cleanup possible redirect junk */
  curlx_safefree(انکڑے->req.newurl);
  curlx_safefree(انکڑے->req.location);

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
  إذا(conn && conn->scheme->لگاتار->پورا && (انکڑے->mstate >= MSTATE_PROTOCONNECT))
    نتیجہ = conn->scheme->لگاتار->پورا(انکڑے, حالت, premature);
  وإلا
    نتیجہ = حالت;

  إذا(انکڑے->mstate > MSTATE_CONNECTING &&
     (نتیجہ != CURLE_ABORTED_BY_CALLBACK)) {
    /* avoid this if
     * - the transfer has not connected
     * - we already aborted by callback to avoid this calling another callback
     */
    صحيح rc = Curl_pgrs_پورا(انکڑے, pnow);
    إذا(!نتیجہ && rc)
      نتیجہ = CURLE_ABORTED_BY_CALLBACK;
  }

  /* Make sure that transfer client writes are really done now. */
  نتیجہ = Curl_1st_fatal(نتیجہ, Curl_xfer_write_پورا(انکڑے, premature));

  إذا(!نتیجہ)
    نتیجہ = Curl_req_پورا(&انکڑے->req, انکڑے, premature);

  إذا(conn) {
    /* Detach connection and decide what to do with it. */
    بنية multi_پورا_ctx mdctx;

    /* Inform connection filters that this transfer is done and they
     * need to give up any references to `data`. */
    Curl_conn_ev_انکڑے_پورا(انکڑے, premature);
    Curl_detach_connection(انکڑے);
    Curl_dnscache_prune(انکڑے, pnow);

    memset(&mdctx, 0, حجم(mdctx));
    mdctx.premature = premature;
    Curl_cpool_return(انکڑے, conn, multi_پورا_locked, &mdctx, pnow);

    إذا(انکڑے->multi)
      multi_ویلا_سارنی_pending(انکڑے->multi);
  }

  /* flush the netrc cache */
  Curl_netrc_cleanup(&انکڑے->state.netrc);
  أرجع نتیجہ;
}

CURLMcode Curl_multi_remove_handle(بنية Curl_multi *multi,
                                   بنية Curl_easy *انکڑے)
{
  CURLMcode mresult;
  bool premature;
  uint32_t mid;

  /* Prevent users from trying to remove same easy handle more than once */
  إذا(!انکڑے->multi)
    أرجع CURLM_OK; /* it is already removed so let's say it is fine! */

  /* Prevent users from trying to remove an easy handle from the wrong multi */
  إذا(انکڑے->multi != multi)
    أرجع CURLM_BAD_EASY_HANDLE;

  إذا(انکڑے->mid == UINT32_MAX) {
    DEBUGASSERT(0);
    أرجع CURLM_اندرونی_ERROR;
  }
  إذا(Curl_uint32_tbl_get(&multi->xfers, انکڑے->mid) != انکڑے) {
    DEBUGASSERT(0);
    أرجع CURLM_اندرونی_ERROR;
  }

  premature = (انکڑے->mstate < MSTATE_COMPLETED);

  إذا(انکڑے->conn) {
    /* If the 'state' is not INIT or COMPLETED, we might need to do something
       nice to put the easy_handle in a good known state when this returns. */
    إذا(premature && (انکڑے->mstate > MSTATE_DO))
      streamclose(انکڑے->conn);

    /* multi_done() clears the association between the easy handle and the
       connection.
       Note that this ignores the return code because there is
       nothing really useful to do with it anyway! */
    (فراغ)multi_پورا(انکڑے, انکڑے->نتیجہ, premature);
  }

  /* The timer must be shut down before data->multi is set to NULL, else
     data's splaynode would remain in the splay tree after curl_easy_cleanup is
     called. Do it after multi_done() in case that sets another time! */
  Curl_expire_clear_all(انکڑے);

  /* In MSGSENT, it was deducted from `multi->xfers_alive` already. */
  إذا(انکڑے->mstate != MSTATE_MSGSENT)
    --multi->xfers_alive;

  إذا(انکڑے->state.really_alive) {
    انکڑے->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    إذا(!multi->xfers_really_alive)
      (فراغ)multi_assess_wakeup(multi);
  }

  Curl_wildcard_dtor(&انکڑے->wildcard);

  انکڑے->mstate = MSTATE_COMPLETED;

  /* Remove the association between the connection and the handle */
  Curl_detach_connection(انکڑے);

  /* Tell event handling that this transfer is definitely going away */
  Curl_multi_ev_xfer_پورا(multi, انکڑے);

  إذا(انکڑے->سیٹ.connect_only) {
    إذا(انکڑے->multi_easy) {
      إذا(انکڑے->state.آخری_conn_id != -1) {
        /* Mark any connect-only connection for closure */
        بنية connectdata *conn;
        (فراغ)Curl_getconnectinfo(انکڑے, &conn);
        إذا(conn && conn->بٹ.connect_only)
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
      (فراغ)Curl_getconnectinfo(انکڑے, &conn);
      إذا(conn)
        Curl_conn_close(انکڑے, conn, TRUE);
    }
  }

#جے_تعریف_شدہ USE_LIBPSL
  /* Remove the PSL association. */
  إذا(انکڑے->psl == &multi->psl)
    انکڑے->psl = عدم;
#جے_انت

  /* clear the association to this multi handle */
  mid = انکڑے->mid;
  DEBUGASSERT(Curl_uint32_tbl_شامل_اے(&multi->xfers, mid));
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  Curl_uint32_bset_remove(&multi->process, mid);
  Curl_uint32_bset_remove(&multi->dirty, mid);
  Curl_uint32_bset_remove(&multi->pending, mid);
  Curl_uint32_bset_remove(&multi->msgsent, mid);
  انکڑے->multi = عدم;
  انکڑے->mid = UINT32_MAX;
  انکڑے->master_mid = UINT32_MAX;

  /* A pending transfer *might* be able to run now. */
  multi_ویلا_سارنی_pending(multi);
  mresult = Curl_update_timer(multi);
  إذا(mresult)
    أرجع mresult;

  mresult = multi_assess_wakeup(multi);
  إذا(mresult) {
    failf(انکڑے, "error enabling wakeup listening: %d", mresult);
    أرجع mresult;
  }

  CURL_TRC_M(انکڑے, "removed from multi, mid=%u, running=%u, total=%u",
             mid, multi_xfers_running(multi),
             Curl_uint32_tbl_گنتی(&multi->xfers));
  أرجع CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *curl)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_remove_handle, &mresult)) {
    بنية Curl_easy *انکڑے = curl;
    إذا(!GOOD_EASY_HANDLE(انکڑے) || انکڑے->state.اندرونی)
      mresult = CURLM_BAD_EASY_HANDLE;
    وإلا
      mresult = Curl_multi_remove_handle(m, انکڑے);
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
فراغ Curl_detach_connection(بنية Curl_easy *انکڑے)
{
  بنية connectdata *conn = انکڑے->conn;
  إذا(conn) {
    /* this should never happen, prevent underflow */
    DEBUGASSERT(conn->attached_xfers);
    إذا(conn->attached_xfers) {
      conn->attached_xfers--;
      إذا(!conn->attached_xfers)
        conn->attached_multi = عدم;
    }
  }
  انکڑے->conn = عدم;
}

/*
 * Curl_attach_connection() attaches this transfer to this connection.
 *
 * This is the only function that should assign data->conn.
 * `matched == TRUE` means the transfer's properties match this
 * connection and it is not a temporary attach for maintenance.
 */
فراغ Curl_attach_connection(بنية Curl_easy *انکڑے,
                            بنية connectdata *conn,
                            bool matched)
{
  DEBUGASSERT(انکڑے);
  DEBUGASSERT(!انکڑے->conn);
  DEBUGASSERT(conn);
  DEBUGASSERT(conn->attached_xfers < UINT32_MAX);
  انکڑے->conn = conn;
  إذا(matched) {
    انکڑے->state.آخری_conn_id = conn->connection_id;
    انکڑے->state.آخری_cpid = conn->cpid;
  }
  وإلا
    DEBUGASSERT(!انکڑے->mid); /* admin handle */
  conn->attached_xfers++;
  /* all attached transfers must be from the same multi */
  إذا(!conn->attached_multi)
    conn->attached_multi = انکڑے->multi;
  DEBUGASSERT(conn->attached_multi == انکڑے->multi);

  إذا(conn->scheme && conn->scheme->لگاتار->attach)
    conn->scheme->لگاتار->attach(انکڑے, conn);
}

/* adjust pollset for rate limits/pauses */
ساكن CURLcode multi_adjust_pollset(بنية Curl_easy *انکڑے,
                                     بنية easy_pollset *ps)
{
  CURLcode نتیجہ = CURLE_OK;

  إذا(ps->n) {
    bool send_blocked, recv_blocked;

    recv_blocked = (Curl_rlimit_avail(&انکڑے->ترقی.dl.rlimit, عدم) <= 0);
    send_blocked = (Curl_rlimit_avail(&انکڑے->ترقی.ul.rlimit, عدم) <= 0);
    إذا(send_blocked || recv_blocked) {
      صحيح i;
      لكل(i = 0; i <= SECONDARYSOCKET; ++i) {
        curl_socket_t sock = انکڑے->conn->sock[i];
        إذا(sock == CURL_SOCKET_BAD)
          استمر;
        إذا(recv_blocked && Curl_pollset_want_recv(انکڑے, ps, sock)) {
          نتیجہ = Curl_pollset_remove_وچ(انکڑے, ps, sock);
          إذا(نتیجہ)
            اكسر;
        }
        إذا(send_blocked && Curl_pollset_want_send(انکڑے, ps, sock)) {
          نتیجہ = Curl_pollset_remove_باہرلا(انکڑے, ps, sock);
          إذا(نتیجہ)
            اكسر;
        }
      }
    }

    /* Not blocked and wanting to receive. If there is data pending
     * in the connection filters, make transfer run again. */
    إذا(!recv_blocked &&
       ((Curl_pollset_want_recv(انکڑے, ps, انکڑے->conn->sock[FIRSTSOCKET]) &&
         Curl_conn_انکڑے_pending(انکڑے, FIRSTSOCKET)) ||
        (Curl_pollset_want_recv(انکڑے, ps, انکڑے->conn->sock[SECONDARYSOCKET]) &&
         Curl_conn_انکڑے_pending(انکڑے, SECONDARYSOCKET)))) {
      CURL_TRC_M(انکڑے, "pollset[] has POLLIN, but there is still "
                 "buffered input -> mark as dirty");
      Curl_multi_mark_dirty(انکڑے);
    }
  }
  أرجع نتیجہ;
}

ساكن CURLcode mstate_connecting_pollset(بنية Curl_easy *انکڑے,
                                          بنية easy_pollset *ps)
{
  بنية connectdata *conn = انکڑے->conn;
  curl_socket_t sockfd;
  CURLcode نتیجہ = CURLE_OK;

  إذا(Curl_xfer_recv_is_paused(انکڑے))
    أرجع CURLE_OK;
  /* If a socket is set, receiving is default. If the socket
   * has not been determined yet (eyeballing), always ask the
   * connection filters for what to monitor. */
  sockfd = Curl_conn_get_پہلا_socket(انکڑے);
  إذا(sockfd != CURL_SOCKET_BAD) {
    نتیجہ = Curl_pollset_change(انکڑے, ps, sockfd, CURL_POLL_وچ, 0);
    إذا(!نتیجہ)
      نتیجہ = multi_adjust_pollset(انکڑے, ps);
  }
  إذا(!نتیجہ)
    نتیجہ = Curl_conn_adjust_pollset(انکڑے, conn, ps);
  أرجع نتیجہ;
}

ساكن CURLcode mstate_protocol_pollset(بنية Curl_easy *انکڑے,
                                        بنية easy_pollset *ps)
{
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ = CURLE_OK;

  إذا(conn->scheme->لگاتار->proto_pollset)
    نتیجہ = conn->scheme->لگاتار->proto_pollset(انکڑے, ps);
  وإلا {
    curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
    إذا(sockfd != CURL_SOCKET_BAD) {
      /* Default is to wait to something from the server */
      نتیجہ = Curl_pollset_change(انکڑے, ps, sockfd, CURL_POLL_وچ, 0);
    }
  }
  إذا(!نتیجہ)
    نتیجہ = multi_adjust_pollset(انکڑے, ps);
  إذا(!نتیجہ)
    نتیجہ = Curl_conn_adjust_pollset(انکڑے, conn, ps);
  أرجع نتیجہ;
}

ساكن CURLcode mstate_do_pollset(بنية Curl_easy *انکڑے,
                                  بنية easy_pollset *ps)
{
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ = CURLE_OK;

  إذا(conn->scheme->لگاتار->doing_pollset)
    نتیجہ = conn->scheme->لگاتار->doing_pollset(انکڑے, ps);
  وإلا إذا(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    نتیجہ = Curl_pollset_جوڑو_باہرلا(انکڑے, ps, conn->sock[conn->send_idx]);
  }
  إذا(!نتیجہ)
    نتیجہ = multi_adjust_pollset(انکڑے, ps);
  إذا(!نتیجہ)
    نتیجہ = Curl_conn_adjust_pollset(انکڑے, conn, ps);
  أرجع نتیجہ;
}

ساكن CURLcode mstate_domore_pollset(بنية Curl_easy *انکڑے,
                                      بنية easy_pollset *ps)
{
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ = CURLE_OK;

  إذا(conn->scheme->لگاتار->domore_pollset)
    نتیجہ = conn->scheme->لگاتار->domore_pollset(انکڑے, ps);
  وإلا إذا(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    نتیجہ = Curl_pollset_جوڑو_باہرلا(انکڑے, ps, conn->sock[conn->send_idx]);
  }
  إذا(!نتیجہ)
    نتیجہ = multi_adjust_pollset(انکڑے, ps);
  إذا(!نتیجہ)
    نتیجہ = Curl_conn_adjust_pollset(انکڑے, conn, ps);
  أرجع نتیجہ;
}

ساكن CURLcode mstate_perform_pollset(بنية Curl_easy *انکڑے,
                                       بنية easy_pollset *ps)
{
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ = CURLE_OK;

  إذا(conn->scheme->لگاتار->perform_pollset)
    نتیجہ = conn->scheme->لگاتار->perform_pollset(انکڑے, ps);
  وإلا {
    /* Default is to obey the request flags for send/recv */
    إذا(Curl_req_want_recv(انکڑے) && CONN_SOCK_IDX_VALID(conn->recv_idx)) {
      نتیجہ = Curl_pollset_جوڑو_وچ(انکڑے, ps, conn->sock[conn->recv_idx]);
    }
    إذا(!نتیجہ && Curl_req_want_send(انکڑے) &&
       CONN_SOCK_IDX_VALID(conn->send_idx)) {
      نتیجہ = Curl_pollset_جوڑو_باہرلا(انکڑے, ps, conn->sock[conn->send_idx]);
    }
  }
  إذا(!نتیجہ)
    نتیجہ = multi_adjust_pollset(انکڑے, ps);
  إذا(!نتیجہ)
    نتیجہ = Curl_conn_adjust_pollset(انکڑے, conn, ps);
  أرجع نتیجہ;
}

#جے_تعریف_شدہ CURLVERBOSE
ساكن size_t multi_timeouts_گنتی(بنية expire_timers *timeouts)
{
  size_t n = 0;
  uint8_t eid = timeouts->پہلا;
  لكل(; eid < EXPIRE_آخری; eid = timeouts->اگلا[eid])
    ++n;
  أرجع n;
}
#جے_انت

/* Initializes `poll_set` with the current socket poll actions needed
 * for transfer `data`. */
CURLMcode Curl_multi_pollset(بنية Curl_easy *انکڑے,
                             بنية easy_pollset *ps)
{
  CURLcode نتیجہ = CURLE_OK;

  Curl_pollset_reset(ps);
#جے_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  /* The admin handle always listens on the wakeup socket when there
   * are transfers alive. */
  إذا(انکڑے->multi && (انکڑے == انکڑے->multi->admin) &&
     انکڑے->multi->xfers_really_alive) {
    CURL_TRC_M(انکڑے, "adding wakeup, %u xfers really alive",
               انکڑے->multi->xfers_really_alive);
    نتیجہ = Curl_pollset_جوڑو_وچ(انکڑے, ps, انکڑے->multi->wakeup_اندرونی[0]);
  }
#جے_انت
  /* If the transfer has no connection, this is fine. Happens when
     called via curl_multi_remove_handle() => Curl_multi_ev_assess() =>
     Curl_multi_pollset(). */
  إذا(!نتیجہ && انکڑے->conn) {
    بدل(انکڑے->mstate) {
    حالة MSTATE_INIT:
    حالة MSTATE_PENDING:
    حالة MSTATE_تیاری:
    حالة MSTATE_CONNECT:
      /* nothing to poll for yet */
      اكسر;

    حالة MSTATE_CONNECTING:
      نتیجہ = mstate_connecting_pollset(انکڑے, ps);
      اكسر;

    حالة MSTATE_PROTOCONNECT:
    حالة MSTATE_PROTOCONNECTING:
      نتیجہ = mstate_protocol_pollset(انکڑے, ps);
      اكسر;

    حالة MSTATE_DO:
    حالة MSTATE_DOING:
      نتیجہ = mstate_do_pollset(انکڑے, ps);
      اكسر;

    حالة MSTATE_DOING_ہور:
      نتیجہ = mstate_domore_pollset(انکڑے, ps);
      اكسر;

    حالة MSTATE_DID: /* same as PERFORMING in regard to polling */
    حالة MSTATE_PERFORMING:
      نتیجہ = mstate_perform_pollset(انکڑے, ps);
      اكسر;

    حالة MSTATE_RATELIMITING:
      /* we need to let time pass, ignore socket(s) */
      اكسر;

    حالة MSTATE_پورا:
    حالة MSTATE_COMPLETED:
    حالة MSTATE_MSGSENT:
      /* nothing more to poll for */
      اكسر;

    افتراضي:
      failf(انکڑے, "multi_getsock: unexpected multi state %d",
            (صحيح)انکڑے->mstate);
      DEBUGASSERT(0);
      اكسر;
    }
  }

  إذا(نتیجہ) {
    إذا(نتیجہ == CURLE_باہرلا_دا_حافظہ)
      أرجع CURLM_باہرلا_دا_حافظہ;
    failf(انکڑے, "error determining pollset: %d", (صحيح)نتیجہ);
    أرجع CURLM_اندرونی_ERROR;
  }

#جے_تعریف_شدہ CURLVERBOSE
  إذا(CURL_TRC_M_is_verbose(انکڑے)) {
    size_t timeout_گنتی = multi_timeouts_گنتی(&انکڑے->state.timeouts);
    بدل(ps->n) {
    حالة 0:
      CURL_TRC_M(انکڑے, "pollset[], timeouts=%zu, paused %d/%d (r/w)",
                 timeout_گنتی,
                 Curl_xfer_send_is_paused(انکڑے),
                 Curl_xfer_recv_is_paused(انکڑے));
      اكسر;
    حالة 1:
      CURL_TRC_M(انکڑے, "pollset[fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_وچ) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_باہرلا) ? "OUT" : "",
                 timeout_گنتی);
      اكسر;
    حالة 2:
      CURL_TRC_M(انکڑے, "pollset[fd=%" FMT_SOCKET_T " %s%s, "
                 "fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_وچ) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_باہرلا) ? "OUT" : "",
                 ps->sockets[1],
                 (ps->actions[1] & CURL_POLL_وچ) ? "IN" : "",
                 (ps->actions[1] & CURL_POLL_باہرلا) ? "OUT" : "",
                 timeout_گنتی);
      اكسر;
    افتراضي:
      CURL_TRC_M(انکڑے, "pollset[fds=%u], timeouts=%zu", ps->n, timeout_گنتی);
      اكسر;
    }
    CURL_TRC_EASY_TIMERS(انکڑے);
  }
#جے_انت

  أرجع CURLM_OK;
}

CURLMcode curl_multi_fdset(CURLM *m,
                           fd_set *read_fd_سیٹ, fd_set *write_fd_سیٹ,
                           fd_set *exc_fd_سیٹ, صحيح *max_fd)
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
    بنانشانی صحيح i;
    uint32_t mid;
    (فراغ)exc_fd_سیٹ;

    Curl_pollset_init(&ps);
    إذا(Curl_uint32_bset_پہلا(&multi->process, &mid)) {
      افعل {
        بنية Curl_easy *انکڑے = Curl_multi_get_easy(multi, mid);

        إذا(!انکڑے) {
          DEBUGASSERT(0);
          استمر;
        }

        Curl_multi_pollset(انکڑے, &ps);
        لكل(i = 0; i < ps.n; i++) {
          إذا(!FDSET_SOCK(ps.sockets[i]))
            /* pretend it does not exist */
            استمر;
          إذا(ps.actions[i] & CURL_POLL_وچ)
            FD_SET(ps.sockets[i], read_fd_سیٹ);
          إذا(ps.actions[i] & CURL_POLL_باہرلا)
            FD_SET(ps.sockets[i], write_fd_سیٹ);
          إذا((صحيح)ps.sockets[i] > this_max_fd)
            this_max_fd = (صحيح)ps.sockets[i];
        }
      } طالما(Curl_uint32_bset_اگلا(&multi->process, mid, &mid));
    }

    Curl_cshutdn_setfds(&multi->cshutdn, multi->admin,
                        read_fd_سیٹ, write_fd_سیٹ, &this_max_fd);

    *max_fd = this_max_fd;
    Curl_pollset_cleanup(&ps);

    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

CURLMcode curl_multi_waitfds(CURLM *m,
                             بنية curl_waitfd *ufds,
                             بنانشانی صحيح ناپ,
                             بنانشانی صحيح *fd_گنتی)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_waitfds, &mresult)) {
    بنية Curl_waitfds cwfds;
    بنية Curl_multi *multi = m;
    بنية easy_pollset ps;
    بنانشانی صحيح need = 0;
    uint32_t mid;

    إذا(!ufds && (ناپ || !fd_گنتی)) {
      mresult = CURLM_BAD_FUNCTION_دلیل;
      اذهب باہرلا;
    }

    Curl_pollset_init(&ps);
    Curl_waitfds_init(&cwfds, ufds, ناپ);
    mresult = CURLM_OK;
    إذا(Curl_uint32_bset_پہلا(&multi->process, &mid)) {
      افعل {
        بنية Curl_easy *انکڑے = Curl_multi_get_easy(multi, mid);
        إذا(!انکڑے) {
          DEBUGASSERT(0);
          Curl_uint32_bset_remove(&multi->process, mid);
          Curl_uint32_bset_remove(&multi->dirty, mid);
          استمر;
        }
        Curl_multi_pollset(انکڑے, &ps);
        need += Curl_waitfds_جوڑو_ps(&cwfds, &ps);
      } طالما(Curl_uint32_bset_اگلا(&multi->process, mid, &mid));
    }

    need += Curl_cshutdn_جوڑو_waitfds(&multi->cshutdn, multi->admin, &cwfds);

    إذا(need != cwfds.n && ufds)
      mresult = CURLM_باہرلا_دا_حافظہ;

    إذا(fd_گنتی)
      *fd_گنتی = need;
    Curl_pollset_cleanup(&ps);
  }
باہرلا:
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

#جے_تعریف_شدہ USE_WINSOCK
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
                                      بنانشانی صحيح curl_nfds,
                                      بنية curl_waitfd extra_fds[],
                                      بنانشانی صحيح extra_nfds,
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
    لما نقاب = 0;
    إذا(cpfds->pfds[i].events & POLLIN)
      نقاب |= FD_READ | FD_ACCEPT | FD_CLOSE;
    إذا(cpfds->pfds[i].events & POLLPRI)
      نقاب |= FD_OOB;
    إذا(cpfds->pfds[i].events & POLLOUT) {
      نقاب |= FD_WRITE | FD_CONNECT | FD_CLOSE;
      reset_socket_fdwrite(cpfds->pfds[i].fd);
    }
    إذا(نقاب && WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, نقاب)) {
      mresult = CURLM_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }

  إذا(cpfds->n || extrawait) {
    صحيح pollrc = 0;

    إذا(cpfds->n) {         /* pre-check with Winsock */
      pollrc = Curl_poll(cpfds->pfds, cpfds->n, 0);
      إذا(pollrc < 0) {
        mresult = CURLM_UNRECOVERABLE_POLL;
        اذهب باہرلا;
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
      بنانشانی چھوٹا نقاب = 0;
      curl_socket_t s = extra_fds[i].fd;

      wsa_events.l_جال_Events = 0;
      إذا(WSAEnum_جال_Events(s, عدم, &wsa_events) == 0) {
        إذا(wsa_events.l_جال_Events & (FD_READ | FD_ACCEPT | FD_CLOSE))
          نقاب |= CURL_WAIT_POLLIN;
        إذا(wsa_events.l_جال_Events & (FD_WRITE | FD_CONNECT | FD_CLOSE))
          نقاب |= CURL_WAIT_POLLOUT;
        إذا(wsa_events.l_جال_Events & FD_OOB)
          نقاب |= CURL_WAIT_POLLPRI;
        إذا(!pollrc && wsa_events.l_جال_Events)
          nevents++;
      }
      WSAEventSelect(s, multi->wsa_event, 0);
      إذا(!pollrc) {
        extra_fds[i].revents = (چھوٹا)نقاب;
        استمر;
      }
      وإلا {
        بنانشانی r = (بنانشانی)cpfds->pfds[curl_nfds + i].revents;
        إذا(r & POLLIN)
          نقاب |= CURL_WAIT_POLLIN;
        إذا(r & POLLOUT)
          نقاب |= CURL_WAIT_POLLOUT;
        إذا(r & POLLPRI)
          نقاب |= CURL_WAIT_POLLPRI;
        extra_fds[i].revents = (چھوٹا)نقاب;
      }
    }

    /* Count up all our own sockets that had activity,
       and remove them from the event. */
    لكل(i = 0; i < curl_nfds; ++i) {
      wsa_events.l_جال_Events = 0;
      إذا(WSAEnum_جال_Events(cpfds->pfds[i].fd, عدم, &wsa_events) == 0) {
        إذا(!pollrc && wsa_events.l_جال_Events)
          nevents++;
      }
      WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, 0);
    }
    WSAResetEvent(multi->wsa_event);
  }

باہرلا:
  *pnevents = nevents;
  أرجع mresult;
}

#وإلا /* USE_WINSOCK */

ساكن CURLMcode multi_posix_poll(بنية Curl_multi *multi,
                                  بنية curl_pollfds *cpfds,
                                  بنانشانی صحيح curl_nfds,
                                  بنية curl_waitfd extra_fds[],
                                  بنانشانی صحيح extra_nfds,
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
      اذهب باہرلا;
    }
    nevents = pollrc;

    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    لكل(i = 0; i < extra_nfds; i++) {
      بنانشانی r = (بنانشانی)cpfds->pfds[curl_nfds + i].revents;
      بنانشانی چھوٹا نقاب = 0;
      إذا(r & POLLIN)
        نقاب |= CURL_WAIT_POLLIN;
      إذا(r & POLLOUT)
        نقاب |= CURL_WAIT_POLLOUT;
      إذا(r & POLLPRI)
        نقاب |= CURL_WAIT_POLLPRI;
      extra_fds[i].revents = (چھوٹا)نقاب;
    }
  }
  وإلا إذا(extrawait) {
    /* No fds to poll, but asked to obey timeout_ms anyway. We cannot
     * use Curl_poll() as it, on some platforms, returns immediately
     * without fds. */
    curlx_wait_ms(timeout_ms);
  }

باہرلا:
  *pnevents = nevents;
  أرجع mresult;
}

#جے_انت /* !USE_WINSOCK */

#تعریف_کرو عدد_POLLS_ON_ڈھیر 10

ساكن CURLMcode multi_wait(بنية Curl_multi *multi,
                            بنية curl_waitfd extra_fds[],
                            بنانشانی صحيح extra_nfds,
                            صحيح timeout_ms,
                            صحيح *ret,
                            bool extrawait)  /* when no socket, wait */
{
  size_t i;
  صحيح timeout_اندرونی;
  صحيح nevents = 0;
  بنية easy_pollset ps;
  بنية pollfd a_few_on_ڈھیر[عدد_POLLS_ON_ڈھیر];
  بنية curl_pollfds cpfds;
  بنانشانی صحيح curl_nfds = 0; /* how many pfds are for curl transfers */
  بنية Curl_easy *انکڑے = عدم;
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;
#جے_تعریف_شدہ ENABLE_WAKEUP
  صحيح wakeup_idx = -1;
#جے_انت

  إذا(timeout_ms < 0)
    أرجع CURLM_BAD_FUNCTION_دلیل;

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ڈھیر, عدد_POLLS_ON_ڈھیر);

  /* Add the curl handles to our pollfds first */
  إذا(Curl_uint32_bset_پہلا(&multi->process, &mid)) {
    افعل {
      انکڑے = Curl_multi_get_easy(multi, mid);
      إذا(!انکڑے) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        استمر;
      }
      Curl_multi_pollset(انکڑے, &ps);
      إذا(Curl_pollfds_جوڑو_ps(&cpfds, &ps)) {
        mresult = CURLM_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
    } طالما(Curl_uint32_bset_اگلا(&multi->process, mid, &mid));
  }

  إذا(Curl_cshutdn_جوڑو_pollfds(&multi->cshutdn, multi->admin, &cpfds)) {
    mresult = CURLM_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

#جے_تعریف_شدہ ENABLE_WAKEUP
  /* If `extrawait` is TRUE *or* we have `extra_fds`to poll *or* we
   * have transfer sockets to poll, we obey `timeout_ms`.
   * Then we need to also monitor the multi's wakeup
   * socket to catch calls to `curl_multi_wakeup()` during the wait. */
  إذا(extrawait || cpfds.n || extra_nfds) {
    wakeup_idx = cpfds.n;
    إذا(Curl_pollfds_جوڑو_sock(&cpfds, multi->wakeup_pair[0], POLLIN)) {
      mresult = CURLM_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }
#جے_انت

  curl_nfds = cpfds.n; /* what curl internally uses in cpfds */
  /* Add external file descriptions from poll-like struct curl_waitfd */
  لكل(i = 0; i < extra_nfds; i++) {
    بنانشانی چھوٹا events = 0;
    إذا(extra_fds[i].events & CURL_WAIT_POLLIN)
      events |= POLLIN;
    إذا(extra_fds[i].events & CURL_WAIT_POLLPRI)
      events |= POLLPRI;
    إذا(extra_fds[i].events & CURL_WAIT_POLLOUT)
      events |= POLLOUT;
    إذا(Curl_pollfds_جوڑو_sock(&cpfds, extra_fds[i].fd, events)) {
      mresult = CURLM_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }

  /* We check the internal timeout *AFTER* we collected all sockets to
   * poll. Collecting the sockets may install new timers by protocols
   * and connection filters.
   * Use the shorter one of the internal and the caller requested timeout.
   * If we are called with `!extrawait` and multi_timeout() reports no
   * timeouts exist, do not wait. */
  multi_timeout(multi, عدم, &timeout_اندرونی);
  إذا((timeout_اندرونی >= 0) && (timeout_اندرونی < timeout_ms))
    timeout_ms = timeout_اندرونی;

  إذا(انکڑے)
    CURL_TRC_M(انکڑے, "multi_wait(fds=%u, timeout=%d) tinternal=%d",
               cpfds.n, timeout_ms, timeout_اندرونی);

#جے_تعریف_شدہ USE_WINSOCK
  mresult = multi_winsock_select(multi, &cpfds, curl_nfds,
                                 extra_fds, extra_nfds,
                                 timeout_ms, extrawait, &nevents);
#وإلا
  mresult = multi_posix_poll(multi, &cpfds, curl_nfds,
                             extra_fds, extra_nfds,
                             timeout_ms, extrawait, &nevents);
#جے_انت

#جے_تعریف_شدہ ENABLE_WAKEUP
    إذا(nevents && (wakeup_idx >= 0)) {
      إذا(cpfds.pfds[wakeup_idx].revents & POLLIN) {
        (فراغ)Curl_wakeup_consume(multi->wakeup_pair, TRUE);
        /* do not count the wakeup socket into the returned value */
        nevents--;
      }
    }
#جے_انت

باہرلا:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  إذا(ret)
    *ret = nevents;
  أرجع mresult;
}

CURLMcode curl_multi_wait(CURLM *m,
                          بنية curl_waitfd extra_fds[],
                          بنانشانی صحيح extra_nfds,
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
                          بنانشانی صحيح extra_nfds,
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

#جے_تعریف_شدہ ENABLE_WAKEUP
  /* the wakeup_pair variable is only written during init and cleanup,
     making it safe to access from another thread after the init part
     and before cleanup */
  إذا(!Curl_wakeup_signal(multi->wakeup_pair))
    mresult = CURLM_OK;
#جے_انت
#جے_تعریف_شدہ USE_WINSOCK
  إذا(WSASetEvent(multi->wsa_event))
    mresult = CURLM_OK;
#جے_انت
  أرجع mresult;
}

#جے_تعریف_شدہ ENABLE_اندرونی_WAKEUP
فراغ Curl_multi_wakeup_اندرونی(بنية Curl_multi *multi)
{
  /* This is expected to be invocable from another thread which
   * does NOT outlive the multi handle. Check for sanity. */
  إذا(GOOD_MULTI_HANDLE(multi))
    Curl_wakeup_signal(multi->wakeup_اندرونی);
  وإلا
    DEBUGASSERT(0);
}
#جے_انت

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

CURLMcode Curl_multi_جوڑو_perform(بنية Curl_multi *multi,
                                 بنية Curl_easy *انکڑے,
                                 بنية connectdata *conn)
{
  CURLMcode mresult;

  mresult = Curl_multi_جوڑو_handle(multi, انکڑے);
  إذا(!mresult) {
    CURLcode نتیجہ;

    /* pass in NULL for 'conn' here since we do not want to init the
       connection, only this transfer */
    نتیجہ = Curl_init_transfer(انکڑے, عدم);
    إذا(نتیجہ) {
      Curl_multi_remove_handle(multi, انکڑے);
      أرجع CURLM_اندرونی_ERROR;
    }

    /* take this handle to the perform state right away */
    multistate(انکڑے, MSTATE_PERFORMING);
    Curl_attach_connection(انکڑے, conn, TRUE);
    CURL_REQ_سیٹ_RECV(انکڑے);
  }
  أرجع mresult;
}

ساكن CURLcode multi_do(بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(conn->scheme);

  إذا(conn->scheme->لگاتار->do_it)
    نتیجہ = conn->scheme->لگاتار->do_it(انکڑے, پورا);

  أرجع نتیجہ;
}

/*
 * multi_do_more() is called during the DO_MORE multi state. It is a second
 * stage DO state which (wrongly) was introduced to support FTP's second
 * connection.
 *
 * 'complete' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (to DOING state when there is more work to do)
 */

ساكن CURLcode multi_do_ہور(بنية Curl_easy *انکڑے, domore *complete)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;

  *complete = DOMORE_INCOMPLETE;

  إذا(conn->scheme->لگاتار->do_ہور)
    نتیجہ = conn->scheme->لگاتار->do_ہور(انکڑے, complete);

  أرجع نتیجہ;
}

/*
 * Check whether a timeout occurred, and handle it if it did
 */
ساكن bool multi_handle_timeout(بنية Curl_easy *انکڑے,
                                 ثابت بنية curltime *pnow,
                                 bool *دھارا_error,
                                 CURLcode *نتیجہ)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_timeleft_now_ms(انکڑے, pnow);
  إذا(timeout_ms < 0) {
    /* Handle timed out */
    timerid ادھار_timer = Curl_is_connecting(انکڑے) ?
                         TIMER_STARTSINGLE : TIMER_STARTOP;
    timediff_t elapsed_ms = Curl_pgrs_since_ms(انکڑے, عدم, ادھار_timer);
    إذا(انکڑے->mstate == MSTATE_CONNECTING) {
      failf(انکڑے, "%s timed out after %" FMT_TIMEDIFF_T " milliseconds",
            انکڑے->conn->بٹ.dns_resolved ? "Connection" : "Resolving",
            elapsed_ms);
      /* keep what we know about a partly done connect, e.g. TCP done
       * but TLS handshake still ongoing */
      Curl_conn_report_failed(انکڑے, FIRSTSOCKET);
    }
    وإلا {
      بنية SingleRequest *k = &انکڑے->req;
      إذا(k->ناپ != -1) {
        failf(انکڑے, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount, k->ناپ);
      }
      وإلا {
        failf(انکڑے, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount);
      }
    }
    *نتیجہ = CURLE_OPERATION_TIMEDOUT;
    إذا(انکڑے->conn) {
      /* Force connection closed if the connection has indeed been used */
      إذا(انکڑے->mstate > MSTATE_DO) {
        streamclose(انکڑے->conn);
        *دھارا_error = TRUE;
      }
      (فراغ)multi_پورا(انکڑے, *نتیجہ, TRUE);
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

ساكن CURLcode protocol_connecting(بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;

  إذا(conn && conn->scheme->لگاتار->connecting) {
    *پورا = FALSE;
    نتیجہ = conn->scheme->لگاتار->connecting(انکڑے, پورا);
  }
  وإلا
    *پورا = TRUE;

  أرجع نتیجہ;
}

/*
 * We are DOING this is being called over and over from the multi interface
 * until the DOING phase is done on protocol layer.
 */

ساكن CURLcode protocol_doing(بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية connectdata *conn = انکڑے->conn;

  إذا(conn && conn->scheme->لگاتار->doing) {
    *پورا = FALSE;
    نتیجہ = conn->scheme->لگاتار->doing(انکڑے, پورا);
  }
  وإلا
    *پورا = TRUE;

  أرجع نتیجہ;
}

/*
 * We have discovered that the TCP connection has been successful, we can now
 * proceed with some action.
 *
 */
ساكن CURLcode protocol_connect(بنية Curl_easy *انکڑے, bool *protocol_پورا)
{
  بنية connectdata *conn = انکڑے->conn;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(conn);
  DEBUGASSERT(protocol_پورا);
  DEBUGASSERT(Curl_conn_is_connected(conn, FIRSTSOCKET));

  *protocol_پورا = FALSE;
  إذا(!conn->بٹ.protoconnstart) {
    إذا(conn->scheme->لگاتار->connect_it) {
      /* Call the protocol-specific connect function */
      نتیجہ = conn->scheme->لگاتار->connect_it(انکڑے, protocol_پورا);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    conn->بٹ.protoconnstart = TRUE;
  }

  /* Unless this protocol does not have any protocol-connect callback, as
     then we know we are done. */
  إذا(!conn->scheme->لگاتار->connecting)
    *protocol_پورا = TRUE;
  أرجع CURLE_OK;
}

/*
 * posttransfer() is called immediately after a transfer ends
 */
ساكن فراغ multi_posttransfer(بنية Curl_easy *انکڑے)
{
#إذا defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
  /* restore the signal handler for SIGPIPE before we get back */
  إذا(!انکڑے->سیٹ.no_signal)
    إشارة(SIGPIPE, انکڑے->state.prev_signal);
#وإلا
  (فراغ)انکڑے;
#جے_انت
}

/*
 * multi_follow() handles the URL redirect magic. Pass in the 'newurl' string
 * as given by the remote server and set up the new URL to request.
 *
 * This function DOES NOT FREE the given URL.
 */
ساكن CURLcode multi_follow(بنية Curl_easy *انکڑے,
                             ثابت بنية Curl_scheme *handler,
                             ثابت حرف *newurl, /* the Location: string */
                             followtype type) /* see transfer.h */
{
  إذا(handler && handler->لگاتار->follow)
    أرجع handler->لگاتار->follow(انکڑے, newurl, type);

  إذا(type == FOLLOW_RETRY)
    /* Retries are generic and do not require protocol-specific redirect
       handling. */
    أرجع CURLE_OK;

  أرجع CURLE_TOO_MANY_REDIRECTS;
}

ساكن CURLcode mspeed_check(بنية Curl_easy *انکڑے)
{
  إذا(Curl_rlimit_سرگرم(&انکڑے->ترقی.dl.rlimit) ||
     Curl_rlimit_سرگرم(&انکڑے->ترقی.ul.rlimit)) {
    /* check if our send/recv limits require idle waits */
    ثابت بنية curltime *pnow = Curl_pgrs_now(انکڑے);
    timediff_t recv_ms, send_ms;

    send_ms = Curl_rlimit_wait_ms(&انکڑے->ترقی.ul.rlimit, pnow);
    recv_ms = Curl_rlimit_wait_ms(&انکڑے->ترقی.dl.rlimit, pnow);

    إذا(send_ms || recv_ms) {
      إذا(انکڑے->mstate != MSTATE_RATELIMITING) {
        multistate(انکڑے, MSTATE_RATELIMITING);
      }
      Curl_expire_سیٹ(انکڑے, EXPIRE_TOOFAST, CURLMAX(send_ms, recv_ms), pnow);
      Curl_multi_clear_dirty(انکڑے);
      CURL_TRC_M(انکڑے, "[RLIMIT] waiting %" FMT_TIMEDIFF_T "ms",
                 CURLMAX(send_ms, recv_ms));
      أرجع CURLE_AGAIN;
    }
    وإلا {
      /* when will the rate limits increase next? The transfer needs
       * to run again at that time or it may stall. */
      send_ms = Curl_rlimit_اگلا_قدم_ms(&انکڑے->ترقی.ul.rlimit, pnow);
      recv_ms = Curl_rlimit_اگلا_قدم_ms(&انکڑے->ترقی.dl.rlimit, pnow);
      إذا(send_ms || recv_ms) {
        timediff_t اگلا_ms = CURLMIN(send_ms, recv_ms);
        إذا(!اگلا_ms)
          اگلا_ms = CURLMAX(send_ms, recv_ms);
        Curl_expire_سیٹ(انکڑے, EXPIRE_TOOFAST, اگلا_ms, pnow);
        CURL_TRC_M(انکڑے, "[RLIMIT] next token update in %" FMT_TIMEDIFF_T "ms",
                   اگلا_ms);
      }
    }
  }

  إذا(انکڑے->mstate != MSTATE_PERFORMING) {
    CURL_TRC_M(انکڑے, "[RLIMIT] wait over, continue");
    multistate(انکڑے, MSTATE_PERFORMING);
  }
  أرجع CURLE_OK;
}

ساكن CURLMcode multistate_performing(بنية Curl_easy *انکڑے,
                                       bool *دھارا_errorp,
                                       CURLcode *resultp)
{
  حرف *newurl = عدم;
  bool retry = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجہ = *resultp = CURLE_OK;
  *دھارا_errorp = FALSE;

  إذا(mspeed_check(انکڑے) == CURLE_AGAIN)
    أرجع CURLM_OK;

  /* read/write data if it is ready to do so */
  نتیجہ = Curl_sendrecv(انکڑے);

  إذا(انکڑے->req.پورا || (نتیجہ == CURLE_RECV_ERROR)) {
    /* If CURLE_RECV_ERROR happens early enough, we assume it was a race
     * condition and the server closed the reused connection exactly when we
     * wanted to use it, so figure out if that is indeed the case.
     */
    CURLcode ret = Curl_retry_request(انکڑے, &newurl);
    إذا(!ret)
      retry = !!newurl;
    وإلا إذا(!نتیجہ)
      نتیجہ = ret;

    إذا(retry) {
      /* if we are to retry, set the result to OK and consider the
         request as done */
      نتیجہ = CURLE_OK;
      انکڑے->req.پورا = TRUE;
    }
  }
#جے_بنا_تعریف CURL_DISABLE_HTTP
  وإلا إذا((نتیجہ == CURLE_HTTP2_دھارا) &&
          Curl_h2_http_1_1_error(انکڑے)) {
    CURLcode ret = Curl_retry_request(انکڑے, &newurl);

    إذا(!ret) {
      infof(انکڑے, "Downgrades to HTTP/1.1");
      streamclose(انکڑے->conn);
      انکڑے->state.http_neg.wanted = CURL_HTTP_V1x;
      انکڑے->state.http_neg.allowed = CURL_HTTP_V1x;
      /* clear the error message bit too as we ignore the one we got */
      انکڑے->state.errorbuf = FALSE;
      إذا(!newurl)
        /* typically for HTTP_1_1_REQUIRED error on first flight */
        newurl = Curl_bufref_dup(&انکڑے->state.url);
      إذا(!newurl) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
      }
      وإلا {
        /* if we are to retry, set the result to OK and consider the request
          as done */
        retry = TRUE;
        نتیجہ = CURLE_OK;
        انکڑے->req.پورا = TRUE;
      }
    }
    وإلا
      نتیجہ = ret;
  }
#جے_انت

  إذا(نتیجہ) {
    /*
     * The transfer phase returned error, we mark the connection to get closed
     * to prevent being reused. This is because we cannot possibly know if the
     * connection is in a good shape or not now. Unless it is a protocol which
     * uses two "channels" like FTP, as then the error happened in the data
     * connection.
     */

    إذا(!(انکڑے->conn->scheme->flags & PROTOPT_DUAL) &&
       نتیجہ != CURLE_HTTP2_دھارا)
      streamclose(انکڑے->conn);

    multi_posttransfer(انکڑے);
    multi_پورا(انکڑے, نتیجہ, TRUE);
  }
  وإلا إذا(انکڑے->req.پورا && !Curl_cwriter_is_paused(انکڑے)) {
    ثابت بنية Curl_scheme *handler = انکڑے->conn->scheme;

    /* call this even if the readwrite function returned error */
    multi_posttransfer(انکڑے);

    /* When we follow redirects or is set to retry the connection, we must to
       go back to the CONNECT state */
    إذا(انکڑے->req.newurl || retry) {
      followtype follow = FOLLOW_NONE;
      إذا(!retry) {
        /* if the URL is a follow-location and not a retried request then
           figure out the URL here */
        curlx_free(newurl);
        newurl = انکڑے->req.newurl;
        انکڑے->req.newurl = عدم;
        follow = FOLLOW_REDIR;
      }
      وإلا
        follow = FOLLOW_RETRY;
      (فراغ)multi_پورا(انکڑے, CURLE_OK, FALSE);
      /* multi_done() might return CURLE_GOT_NOTHING */
      نتیجہ = multi_follow(انکڑے, handler, newurl, follow);
      إذا(!نتیجہ) {
        multistate(انکڑے, MSTATE_تیاری);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    وإلا {
      /* after the transfer is done, go DONE */

      /* but first check to see if we got a location info even though we are
         not following redirects */
      إذا(انکڑے->req.location) {
        curlx_free(newurl);
        newurl = انکڑے->req.location;
        انکڑے->req.location = عدم;
        نتیجہ = multi_follow(انکڑے, handler, newurl, FOLLOW_FAKE);
        إذا(نتیجہ) {
          *دھارا_errorp = TRUE;
          نتیجہ = multi_پورا(انکڑے, نتیجہ, TRUE);
        }
      }

      إذا(!نتیجہ) {
        multistate(انکڑے, MSTATE_پورا);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
  }
  وإلا { /* not errored, not done */
    mspeed_check(انکڑے);
  }
  curlx_free(newurl);
  *resultp = نتیجہ;
  أرجع mresult;
}

ساكن CURLMcode multistate_do(بنية Curl_easy *انکڑے,
                               bool *دھارا_errorp,
                               CURLcode *resultp)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجہ = CURLE_OK;
  إذا(انکڑے->سیٹ.fprereq) {
    بنية Curl_mapi_guard guard;
    صحيح prereq_rc;

    /* call the prerequest callback function */
    CURL_CBAPI_شروع(&guard, انکڑے, easy_fprereq);
    prereq_rc = انکڑے->سیٹ.fprereq(انکڑے->سیٹ.prereq_userp,
                                  انکڑے->info.primary.remote_ip,
                                  انکڑے->info.primary.local_ip,
                                  انکڑے->info.primary.remote_پورٹ,
                                  انکڑے->info.primary.local_پورٹ);
    CURL_CBAPI_سرا(&guard);
    إذا(prereq_rc != CURL_PREREQFUNC_OK) {
      failf(انکڑے, "operation aborted by pre-request callback");
      /* failure in pre-request callback - do not do any other processing */
      نتیجہ = CURLE_ABORTED_BY_CALLBACK;
      multi_posttransfer(انکڑے);
      multi_پورا(انکڑے, نتیجہ, FALSE);
      *دھارا_errorp = TRUE;
      اذهب سرا;
    }
  }

  إذا(انکڑے->سیٹ.connect_only && !انکڑے->سیٹ.connect_only_ws) {
    multistate(انکڑے, MSTATE_پورا);
    mresult = CURLM_CALL_MULTI_PERFORM;
  }
  وإلا {
    bool dophase_پورا = FALSE;
    /* Perform the protocol's DO action */
    نتیجہ = multi_do(انکڑے, &dophase_پورا);

    إذا(!نتیجہ) {
      إذا(!dophase_پورا) {
#جے_بنا_تعریف CURL_DISABLE_FTP
        /* some steps needed for wildcard matching */
        إذا(انکڑے->state.wildcardmatch) {
          بنية Wildcard_انکڑے *wc = انکڑے->wildcard;
          إذا(wc->state == CURLWC_پورا || wc->state == CURLWC_SKIP) {
            /* skip some states if it is important */
            multi_پورا(انکڑے, CURLE_OK, FALSE);

            /* if there is no connection left, skip the DONE state */
            multistate(انکڑے, انکڑے->conn ? MSTATE_پورا : MSTATE_COMPLETED);
            mresult = CURLM_CALL_MULTI_PERFORM;
            اذهب سرا;
          }
        }
#جے_انت
        /* DO was not completed in one function call, we must continue
           DOING... */
        multistate(انکڑے, MSTATE_DOING);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }

      /* after DO, go DO_DONE... or DO_MORE */
      وإلا إذا(انکڑے->conn->بٹ.do_ہور) {
        /* we are supposed to do more, but we need to sit down, relax and wait
           a little while first */
        multistate(انکڑے, MSTATE_DOING_ہور);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
      وإلا {
        /* we are done with the DO, now DID */
        multistate(انکڑے, MSTATE_DID);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    وإلا إذا((نتیجہ == CURLE_SEND_ERROR) &&
            انکڑے->conn->بٹ.reuse) {
      /*
       * In this situation, a connection that we were trying to use may have
       * unexpectedly died. If possible, send the connection back to the
       * CONNECT phase so we can try again.
       */
      ثابت بنية Curl_scheme *handler = انکڑے->conn->scheme;
      حرف *newurl = عدم;
      followtype follow = FOLLOW_NONE;
      CURLcode drc;

      drc = Curl_retry_request(انکڑے, &newurl);
      إذا(drc) {
        /* a failure here pretty much implies an out of memory */
        نتیجہ = drc;
        *دھارا_errorp = TRUE;
      }

      multi_posttransfer(انکڑے);
      drc = multi_پورا(انکڑے, نتیجہ, FALSE);

      /* When set to retry the connection, we must go back to the CONNECT
       * state */
      إذا(newurl) {
        إذا(!drc || (drc == CURLE_SEND_ERROR)) {
          follow = FOLLOW_RETRY;
          drc = multi_follow(انکڑے, handler, newurl, follow);
          إذا(!drc) {
            multistate(انکڑے, MSTATE_تیاری);
            mresult = CURLM_CALL_MULTI_PERFORM;
            نتیجہ = CURLE_OK;
          }
          وإلا {
            /* Follow failed */
            نتیجہ = drc;
          }
        }
        وإلا {
          /* done did not return OK or SEND_ERROR */
          نتیجہ = drc;
        }
      }
      وإلا {
        /* Have error handler disconnect conn if we cannot retry */
        *دھارا_errorp = TRUE;
      }
      curlx_free(newurl);
    }
    وإلا {
      /* failure detected */
      multi_posttransfer(انکڑے);
      إذا(انکڑے->conn)
        multi_پورا(انکڑے, نتیجہ, FALSE);
      *دھارا_errorp = TRUE;
    }
  }
سرا:
  *resultp = نتیجہ;
  أرجع mresult;
}

ساكن CURLMcode multistate_ratelimiting(بنية Curl_easy *انکڑے,
                                         CURLcode *resultp)
{
  CURLcode نتیجہ = CURLE_OK;
  CURLMcode mresult = CURLM_OK;
  DEBUGASSERT(انکڑے->conn);
  /* if both rates are within spec, resume transfer */
  نتیجہ = Curl_pgrsCheck(انکڑے);

  إذا(نتیجہ) {
    إذا(!(انکڑے->conn->scheme->flags & PROTOPT_DUAL) &&
       نتیجہ != CURLE_HTTP2_دھارا)
      streamclose(انکڑے->conn);

    multi_posttransfer(انکڑے);
    multi_پورا(انکڑے, نتیجہ, TRUE);
  }
  وإلا {
    إذا(!mspeed_check(انکڑے))
      mresult = CURLM_CALL_MULTI_PERFORM;
  }
  *resultp = نتیجہ;
  أرجع mresult;
}

ساكن CURLMcode multistate_connect(بنية Curl_multi *multi,
                                    بنية Curl_easy *انکڑے,
                                    CURLcode *resultp)
{
  /* Connect. We want to get a connection identifier filled in. This state can
     be entered from SETUP and from PENDING. */
  bool connected;
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجہ = Curl_connect(انکڑے, &connected);
  إذا(نتیجہ == CURLE_NO_CONNECTION_دستیاب) {
    /* There was no connection available. We will go to the pending state and
       wait for an available connection. */
    multistate(انکڑے, MSTATE_PENDING);
    /* move from process to pending set */
    Curl_uint32_bset_remove(&multi->process, انکڑے->mid);
    Curl_uint32_bset_remove(&multi->dirty, انکڑے->mid);
    Curl_uint32_bset_جوڑو(&multi->pending, انکڑے->mid);
    *resultp = CURLE_OK;
    أرجع mresult;
  }
  وإلا
    multi_ویلا_سارنی_pending(انکڑے->multi);

  إذا(!نتیجہ) {
    /* after the connect has been sent off, go WAITCONNECT unless the
       protocol connect is already done and we can go directly to WAITDO or
       DO! */
    mresult = CURLM_CALL_MULTI_PERFORM;

    إذا(connected) {
      إذا(!انکڑے->conn->بٹ.reuse &&
         Curl_conn_is_multiplex(انکڑے->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_ویلا_سارنی_pending(انکڑے->multi);
      }
      multistate(انکڑے, MSTATE_PROTOCONNECT);
    }
    وإلا {
      multistate(انکڑے, MSTATE_CONNECTING);
    }
  }
  *resultp = نتیجہ;
  أرجع mresult;
}

/* returns the possibly updated result */
ساكن CURLcode is_مکمل(بنية Curl_multi *multi,
                            بنية Curl_easy *انکڑے,
                            ثابت بنية curltime *pnow,
                            bool دھارا_error,
                            CURLcode نتیجہ)
{
  إذا(انکڑے->mstate < MSTATE_COMPLETED) {
    إذا(نتیجہ) {
      /*
       * If an error was returned, and we are not in completed state now,
       * then we go to completed and consider this transfer aborted.
       */

      /* No attempt to disconnect connections must be made before this -
         connection detach and termination happens only here */

      /* Check if we can move pending requests to send pipe */
      multi_ویلا_سارنی_pending(multi); /* connection */

      إذا(انکڑے->conn) {
        إذا(دھارا_error) {
          /* Do not attempt to send data over a connection that timed out */
          bool dead_connection = نتیجہ == CURLE_OPERATION_TIMEDOUT;
          بنية connectdata *conn = انکڑے->conn;

          /* This is where we make sure that the conn pointer is reset.
             We do not have to do this in every case block above where a
             failure is detected */
          Curl_detach_connection(انکڑے);
          Curl_conn_close(انکڑے, conn, dead_connection);
        }
      }
      وإلا إذا(انکڑے->mstate == MSTATE_CONNECT) {
        /* Curl_connect() failed */
        multi_posttransfer(انکڑے);
        Curl_pgrsUpdate_nometer(انکڑے);
      }

      multistate(انکڑے, MSTATE_COMPLETED);
      أرجع نتیجہ;
    }
    /* if there is still a connection to use, call the progress function */
    وإلا إذا(انکڑے->conn && Curl_conn_is_connected(انکڑے->conn, FIRSTSOCKET)) {
      نتیجہ = Curl_pgrsUpdateX(انکڑے, pnow);
      إذا(نتیجہ) {
        /* aborted due to progress callback return code must close the
           connection */
        streamclose(انکڑے->conn);

        /* if not yet in DONE state, go there. */
        إذا(انکڑے->mstate < MSTATE_پورا)
          multistate(انکڑے, MSTATE_پورا);
        أرجع نتیجہ;
      }
    }
  }
  أرجع نتیجہ;
}

ساكن فراغ handle_completed(بنية Curl_multi *multi,
                             بنية Curl_easy *انکڑے,
                             CURLcode نتیجہ)
{
  bool پیغام_منزل_app = انکڑے->master_mid == UINT32_MAX;

  إذا(!پیغام_منزل_app) {
    /* A sub transfer, not reported to the application. Is anyone still
     * interested in processing its results? */
    إذا(انکڑے->sub_xfer_پورا) {
      بنية Curl_easy *master = Curl_multi_get_easy(multi, انکڑے->master_mid);

      CURL_TRC_M(انکڑے, "sub xfer done for master %u", انکڑے->master_mid);
      إذا(master)
        انکڑے->sub_xfer_پورا(انکڑے, master, نتیجہ);
      وإلا
        CURL_TRC_M(انکڑے, "master easy %u already gone.", انکڑے->master_mid);
    }
  }
  وإلا {
    /* now fill in the CURLMsg with this info */
    بنية CURLMsg *پیغام = &انکڑے->پیغام;

    پیغام->پیغام = CURLMSG_پورا;
    پیغام->easy_handle = انکڑے;
    پیغام->انکڑے.نتیجہ = نتیجہ;

    DEBUGASSERT(!انکڑے->conn);
  }
  multistate(انکڑے, MSTATE_MSGSENT);

  /* remove from the other sets */
  Curl_uint32_bset_remove(&multi->process, انکڑے->mid);
  Curl_uint32_bset_remove(&multi->dirty, انکڑے->mid);
  Curl_uint32_bset_remove(&multi->pending, انکڑے->mid);
  إذا(پیغام_منزل_app)
    multi_addmsg(multi, انکڑے);
  إذا(انکڑے->state.really_alive) {
    انکڑے->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    إذا(!multi->xfers_really_alive)
      (فراغ)multi_assess_wakeup(multi);
  }
  --multi->xfers_alive;
  إذا(!multi->xfers_alive)
    multi_assess_wakeup(multi);
}

ساكن CURLMcode multistate_init(بنية Curl_easy *انکڑے, CURLcode *نتیجہ)
{
  إذا(!انکڑے->state.really_alive) {
    انکڑے->state.really_alive = TRUE;
    ++انکڑے->multi->xfers_really_alive;
    إذا(انکڑے->multi->xfers_really_alive == 1) {
      CURLMcode mresult = multi_assess_wakeup(انکڑے->multi);
      إذا(mresult) {
        failf(انکڑے, "error enabling wakeup listening: %d", mresult);
        أرجع mresult;
      }
    }
  }

  *نتیجہ = Curl_pretransfer(انکڑے);
  إذا(*نتیجہ)
    أرجع CURLM_OK;

  /* after init, go SETUP */
  multistate(انکڑے, MSTATE_تیاری);
  Curl_pgrs_ویلا(انکڑے, TIMER_STARTOP);
  أرجع CURLM_CALL_MULTI_PERFORM;
}

ساكن CURLMcode multistate_تیاری(بنية Curl_easy *انکڑے)
{
  ثابت بنية curltime *pnow = Curl_pgrs_now(انکڑے);
  Curl_pgrs_ویلا_Was(انکڑے, TIMER_STARTSINGLE, *pnow);
  إذا(انکڑے->سیٹ.timeout)
    Curl_expire_سیٹ(انکڑے, EXPIRE_TIMEOUT, انکڑے->سیٹ.timeout, pnow);
  إذا(انکڑے->سیٹ.connecttimeout)
    /* Since a connection might go to pending and back to CONNECT several
       times before it actually takes off, we need to set the timeout once
       in SETUP before we enter CONNECT the first time. */
    Curl_expire_سیٹ(انکڑے, EXPIRE_CONNECTTIMEOUT,
                    انکڑے->سیٹ.connecttimeout, pnow);

  multistate(انکڑے, MSTATE_CONNECT);
  أرجع CURLM_CALL_MULTI_PERFORM;
}

ساكن CURLMcode multistate_connecting(بنية Curl_easy *انکڑے,
                                       bool *دھارا_error,
                                       CURLcode *نتیجہ)
{
  bool connected;

  إذا(!انکڑے->conn) {
    DEBUGASSERT(0);
    *نتیجہ = CURLE_FAILED_INIT;
    أرجع CURLM_OK;
  }
  إذا(!Curl_xfer_recv_is_paused(انکڑے)) {
    *نتیجہ = Curl_conn_connect(انکڑے, FIRSTSOCKET, FALSE, &connected);
    إذا(connected && !*نتیجہ) {
      إذا(!انکڑے->conn->بٹ.reuse &&
         Curl_conn_is_multiplex(انکڑے->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_ویلا_سارنی_pending(انکڑے->multi);
      }
      multistate(انکڑے, MSTATE_PROTOCONNECT);
      أرجع CURLM_CALL_MULTI_PERFORM;
    }
    وإلا إذا(*نتیجہ) {
      /* failure detected */
      CURL_TRC_M(انکڑے, "connect failed -> %d", (صحيح)*نتیجہ);
      multi_posttransfer(انکڑے);
      multi_پورا(انکڑے, *نتیجہ, TRUE);
      *دھارا_error = TRUE;
      أرجع CURLM_OK;
    }
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_protoconnect(بنية Curl_easy *انکڑے,
                                         bool *دھارا_error,
                                         CURLcode *نتیجہ)
{
  bool protocol_connected = FALSE;

  إذا(!*نتیجہ && انکڑے->conn->بٹ.reuse) {
    /* ftp seems to hang when protoconnect on reused connection since we
     * handle PROTOCONNECT in general inside the filters, it seems wrong to
     * restart this on a reused connection.
     */
    multistate(انکڑے, MSTATE_DO);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
  إذا(!*نتیجہ)
    *نتیجہ = protocol_connect(انکڑے, &protocol_connected);
  إذا(!*نتیجہ && !protocol_connected) {
    /* switch to waiting state */
    multistate(انکڑے, MSTATE_PROTOCONNECTING);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
  وإلا إذا(!*نتیجہ) {
    /* protocol connect has completed, go WAITDO or DO */
    multistate(انکڑے, MSTATE_DO);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }

  /* failure detected */
  multi_posttransfer(انکڑے);
  multi_پورا(انکڑے, *نتیجہ, TRUE);
  *دھارا_error = TRUE;
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_protoconnecting(بنية Curl_easy *انکڑے,
                                            bool *دھارا_error,
                                            CURLcode *نتیجہ)
{
  bool protocol_connected = FALSE;

  /* protocol-specific connect phase */
  *نتیجہ = protocol_connecting(انکڑے, &protocol_connected);
  إذا(!*نتیجہ && protocol_connected) {
    /* after the connect has completed, go WAITDO or DO */
    multistate(انکڑے, MSTATE_DO);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
  وإلا إذا(*نتیجہ) {
    /* failure detected */
    multi_posttransfer(انکڑے);
    multi_پورا(انکڑے, *نتیجہ, TRUE);
    *دھارا_error = TRUE;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_doing(بنية Curl_easy *انکڑے,
                                  bool *دھارا_error,
                                  CURLcode *نتیجہ)
{
  bool dophase_پورا = FALSE;

  /* we continue DOING until the DO phase is complete */
  DEBUGASSERT(انکڑے->conn);
  *نتیجہ = protocol_doing(انکڑے, &dophase_پورا);
  إذا(!*نتیجہ) {
    إذا(dophase_پورا) {
      /* after DO, go DO_DONE or DO_MORE */
      multistate(انکڑے, انکڑے->conn->بٹ.do_ہور ?
                 MSTATE_DOING_ہور : MSTATE_DID);
      أرجع CURLM_CALL_MULTI_PERFORM;
    } /* dophase_done */
  }
  وإلا {
    /* failure detected */
    multi_posttransfer(انکڑے);
    multi_پورا(انکڑے, *نتیجہ, FALSE);
    *دھارا_error = TRUE;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_doing_ہور(بنية Curl_easy *انکڑے,
                                       bool *دھارا_error,
                                       CURLcode *نتیجہ)
{
  domore control;

  /*
   * When we are connected, DOING MORE and then go DID
   */
  DEBUGASSERT(انکڑے->conn);
  *نتیجہ = multi_do_ہور(انکڑے, &control);

  إذا(!*نتیجہ) {
    إذا(control != DOMORE_INCOMPLETE) {
      /* if DONE, advance to DO_DONE
         if GOBACK, go back to DOING */
      multistate(انکڑے, control == DOMORE_پورا ? MSTATE_DID : MSTATE_DOING);
      أرجع CURLM_CALL_MULTI_PERFORM;
    }
    /* else
       stay in DO_MORE */
  }
  وإلا {
    /* failure detected */
    multi_posttransfer(انکڑے);
    multi_پورا(انکڑے, *نتیجہ, FALSE);
    *دھارا_error = TRUE;
  }
  أرجع CURLM_OK;
}

ساكن CURLMcode multistate_did(بنية Curl_multi *multi,
                                بنية Curl_easy *انکڑے)
{
  DEBUGASSERT(انکڑے->conn);
  إذا(انکڑے->conn->بٹ.multiplex)
    /* Check if we can move pending requests to send pipe */
    multi_ویلا_سارنی_pending(multi); /* multiplexed */

  /* Only perform the transfer if there is a good socket to work with.
     Having both BAD is a signal to skip immediately to DONE */
  إذا(CONN_SOCK_IDX_VALID(انکڑے->conn->recv_idx) ||
     CONN_SOCK_IDX_VALID(انکڑے->conn->send_idx)) {
    multistate(انکڑے, MSTATE_PERFORMING);
    /* Do not return CURLM_CALL_MULTI_PERFORM to give other transfers
     * a chance to send off their requests.
     * Note: Some SFTP handlers do not seem to like this.
     *       Restrict it to HTTP families. */
    أرجع ((multi->xfers_alive > 1) &&
            (انکڑے->conn->scheme->protocol & PROTO_FAMILY_HTTP)) ?
           CURLM_OK : CURLM_CALL_MULTI_PERFORM;
  }
  وإلا {
#جے_بنا_تعریف CURL_DISABLE_FTP
    إذا(انکڑے->state.wildcardmatch &&
       ((انکڑے->conn->scheme->flags & PROTOPT_WILDCARD) == 0)) {
      انکڑے->wildcard->state = CURLWC_پورا;
    }
#جے_انت
    multistate(انکڑے, MSTATE_پورا);
    أرجع CURLM_CALL_MULTI_PERFORM;
  }
}

ساكن CURLMcode multistate_پورا(بنية Curl_easy *انکڑے, CURLcode *presult)
{
  إذا(انکڑے->conn) {
    CURLcode نتیجہ;

    /* post-transfer command */
    نتیجہ = multi_پورا(انکڑے, *presult, FALSE);

    /* allow a previously set error code take precedence */
    إذا(!(*presult))
      *presult = نتیجہ;
  }

#جے_بنا_تعریف CURL_DISABLE_FTP
  إذا(انکڑے->state.wildcardmatch) {
    إذا(انکڑے->wildcard->state != CURLWC_پورا) {
      /* if a wildcard is set and we are not ending -> lets start again
         with MSTATE_INIT */
      multistate(انکڑے, MSTATE_INIT);
      أرجع CURLM_CALL_MULTI_PERFORM;
    }
  }
#جے_انت
  /* after we have DONE what we are supposed to do, go COMPLETED, and
     it does not matter what the multi_done() returned! */
  multistate(انکڑے, MSTATE_COMPLETED);
  أرجع CURLM_CALL_MULTI_PERFORM;
}

ساكن CURLMcode multi_runsingle(بنية Curl_multi *multi,
                                 بنية Curl_easy *انکڑے,
                                 بنية Curl_sigpipe_ctx *sigpipe_ctx)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجہ = CURLE_OK;
  ثابت بنية curltime *pnow = عدم;

  إذا(multi->dead) {
    /* a multi-level callback returned error before, meaning every individual
     transfer now has failed */
    نتیجہ = CURLE_ABORTED_BY_CALLBACK;
    multi_posttransfer(انکڑے);
    multi_پورا(انکڑے, نتیجہ, FALSE);
    multistate(انکڑے, MSTATE_COMPLETED);
  }

  multi_warn_debug(multi, انکڑے);

  /* transfer runs now, clear the dirty bit. This may be set
   * again during processing, triggering a re-run later. */
  Curl_uint32_bset_remove(&multi->dirty, انکڑے->mid);

  إذا(انکڑے == multi->admin) {
#جے_تعریف_شدہ ENABLE_اندرونی_WAKEUP
    /* Consume any pending wakeup signals before processing.
     * This is necessary for event based processing. See #21547 */
    (فراغ)Curl_wakeup_consume(multi->wakeup_اندرونی, TRUE);
#جے_انت
#جے_تعریف_شدہ USE_RESOLV_THREADED
    Curl_async_thrdd_multi_process(multi);
#جے_انت
    Curl_cshutdn_perform(&multi->cshutdn, multi->admin, sigpipe_ctx);
    اذهب باہرلا;
  }

  sigpipe_apply(انکڑے, sigpipe_ctx);
  افعل {
    /* A "stream" here is a logical stream if the protocol can handle that
       (HTTP/2), or the full connection for older protocols */
    bool دھارا_error = FALSE;
    mresult = CURLM_OK;
    pnow = عدم;

    إذا(multi_ischanged(multi, TRUE)) {
      CURL_TRC_M(انکڑے, "multi changed, check CONNECT_PEND queue");
      multi_ویلا_سارنی_pending(multi); /* multiplexed */
    }

    إذا(انکڑے->mstate > MSTATE_CONNECT &&
       انکڑے->mstate < MSTATE_COMPLETED) {
      /* Make sure we set the connection's current owner */
      DEBUGASSERT(انکڑے->conn);
      إذا(!انکڑے->conn) {
        mresult = CURLM_اندرونی_ERROR;
        اذهب باہرلا;
      }
    }

    /* Wait for the connect state as only then is the start time stored, but
       we must not check already completed handles */
    إذا((انکڑے->mstate >= MSTATE_CONNECT) && (انکڑے->mstate < MSTATE_COMPLETED)) {
      pnow = Curl_pgrs_now(انکڑے);
      إذا(multi_handle_timeout(انکڑے, pnow, &دھارا_error, &نتیجہ))
        /* Skip the statemachine and go directly to error handling section. */
        اذهب statemachine_سرا;
      pnow = عدم;
    }

    بدل(انکڑے->mstate) {
    حالة MSTATE_INIT:
      /* Transitional state. init this transfer. A handle never comes back to
         this state. */
      mresult = multistate_init(انکڑے, &نتیجہ);
      اكسر;

    حالة MSTATE_تیاری:
      /* Transitional state. Setup things for a new transfer. The handle
         can come back to this state on a redirect. */
      mresult = multistate_تیاری(انکڑے);
      اكسر;

    حالة MSTATE_CONNECT:
      mresult = multistate_connect(multi, انکڑے, &نتیجہ);
      اكسر;

    حالة MSTATE_CONNECTING:
      /* awaiting a completion of an asynch TCP connect */
      mresult = multistate_connecting(انکڑے, &دھارا_error, &نتیجہ);
      اكسر;

    حالة MSTATE_PROTOCONNECT:
      mresult = multistate_protoconnect(انکڑے, &دھارا_error, &نتیجہ);
      اكسر;

    حالة MSTATE_PROTOCONNECTING:
      /* protocol-specific connect phase */
      mresult = multistate_protoconnecting(انکڑے, &دھارا_error, &نتیجہ);
      اكسر;

    حالة MSTATE_DO:
      mresult = multistate_do(انکڑے, &دھارا_error, &نتیجہ);
      اكسر;

    حالة MSTATE_DOING:
      /* we continue DOING until the DO phase is complete */
      mresult = multistate_doing(انکڑے, &دھارا_error, &نتیجہ);
      اكسر;

    حالة MSTATE_DOING_ہور:
      /*
       * When we are connected, DOING MORE and then go DID
       */
      mresult = multistate_doing_ہور(انکڑے, &دھارا_error, &نتیجہ);
      اكسر;

    حالة MSTATE_DID:
      mresult = multistate_did(multi, انکڑے);
      اكسر;

    حالة MSTATE_RATELIMITING: /* limit-rate exceeded in either direction */
      mresult = multistate_ratelimiting(انکڑے, &نتیجہ);
      اكسر;

    حالة MSTATE_PERFORMING:
      mresult = multistate_performing(انکڑے, &دھارا_error, &نتیجہ);
      اكسر;

    حالة MSTATE_پورا:
      mresult = multistate_پورا(انکڑے, &نتیجہ);
      اكسر;

    حالة MSTATE_COMPLETED:
      اكسر;

    حالة MSTATE_PENDING:
    حالة MSTATE_MSGSENT:
      /* handles in these states should NOT be in this list */
      اكسر;

    افتراضي:
      mresult = CURLM_اندرونی_ERROR;
      اذهب باہرلا;
    }

    إذا(انکڑے->mstate >= MSTATE_CONNECT &&
       انکڑے->mstate < MSTATE_DO &&
       mresult != CURLM_CALL_MULTI_PERFORM &&
       !multi_ischanged(multi, FALSE)) {
      /* We now handle stream timeouts if and only if this will be the last
       * loop iteration. We only check this on the last iteration to ensure
       * that if we know we have additional work to do immediately
       * (i.e. CURLM_CALL_MULTI_PERFORM == TRUE) then we should do that before
       * declaring the connection timed out as we may almost have a completed
       * connection. */
      pnow = Curl_pgrs_now(انکڑے);
      multi_handle_timeout(انکڑے, pnow, &دھارا_error, &نتیجہ);
    }

statemachine_سرا:
    إذا(!pnow)
      pnow = Curl_pgrs_now(انکڑے);
    نتیجہ = is_مکمل(multi, انکڑے, pnow, دھارا_error, نتیجہ);
    إذا(نتیجہ)
      mresult = CURLM_CALL_MULTI_PERFORM;

    إذا(MSTATE_COMPLETED == انکڑے->mstate) {
      handle_completed(multi, انکڑے, نتیجہ);
      mresult = CURLM_OK;
      اذهب باہرلا;
    }
  } طالما((mresult == CURLM_CALL_MULTI_PERFORM) ||
          multi_ischanged(multi, FALSE));

باہرلا:
  انکڑے->نتیجہ = نتیجہ;
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

  إذا(Curl_uint32_bset_پہلا(&multi->process, &mid)) {
    CURL_TRC_M(multi->admin, "multi_perform(running=%u)",
               multi_xfers_running(multi));
    افعل {
      بنية Curl_easy *انکڑے = Curl_multi_get_easy(multi, mid);
      CURLMcode mresult;
      إذا(!انکڑے) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        استمر;
      }
      mresult = multi_runsingle(multi, انکڑے, &sigpipe_ctx);
      إذا(mresult)
        returncode = mresult;
    } طالما(Curl_uint32_bset_اگلا(&multi->process, mid, &mid));
  }
  sigpipe_restore(&sigpipe_ctx);

  إذا(multi_ischanged(multi, TRUE))
    multi_ویلا_سارنی_pending(multi);

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
    بنية Curl_easy *انکڑے = Curl_multi_get_easy(multi, mid);
    إذا(!انکڑے) {
      DEBUGASSERT(0);
      استمر;
    }
    (فراغ)جوڑو_اگلا_timeout(&شروع, multi, انکڑے);
    إذا(انکڑے->mstate == MSTATE_PENDING) {
      bool دھارا_بن_ورتیا;
      CURLcode نتیجہ_بن_ورتیا;
      إذا(multi_handle_timeout(انکڑے, Curl_pgrs_now(انکڑے),
                              &دھارا_بن_ورتیا, &نتیجہ_بن_ورتیا)) {
        infof(انکڑے, "PENDING handle timeout");
        move_pending_منزل_connect(multi, انکڑے);
      }
    }
  }

  إذا(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < ودھ_توں_ودھ_عدد) ? (صحيح)running : ودھ_توں_ودھ_عدد;
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
    فراغ *اندراج;
    uint32_t mid;

    /* First remove all remaining easy handles,
     * close internal ones. admin handle is special */
    إذا(Curl_uint32_tbl_پہلا(&multi->xfers, &mid, &اندراج)) {
      افعل {
        بنية Curl_easy *انکڑے = اندراج;
        إذا(!GOOD_EASY_HANDLE(انکڑے)) {
          mresult = CURLM_BAD_HANDLE;
          اذهب باہرلا;
        }

#جے_تعریف_شدہ DEBUGBUILD
        إذا(mid != انکڑے->mid) {
          CURL_TRC_M(انکڑے, "multi_cleanup: still present with mid=%u, "
                     "but unexpected data->mid=%u\n", mid, انکڑے->mid);
          DEBUGASSERT(0);
        }
#جے_انت

        إذا(انکڑے == multi->admin)
          استمر;

        إذا(!انکڑے->state.پورا && انکڑے->conn)
          /* if DONE was never called for this handle */
          (فراغ)multi_پورا(انکڑے, CURLE_OK, TRUE);

        انکڑے->multi = عدم; /* clear the association */
        Curl_uint32_tbl_remove(&multi->xfers, mid);
        انکڑے->mid = UINT32_MAX;

#جے_تعریف_شدہ USE_LIBPSL
        إذا(انکڑے->psl == &multi->psl)
          انکڑے->psl = عدم;
#جے_انت
        إذا(انکڑے->state.اندرونی)
          Curl_close(&انکڑے);
      } طالما(Curl_uint32_tbl_اگلا(&multi->xfers, mid, &mid, &اندراج));
    }

#جے_تعریف_شدہ USE_RESOLV_THREADED
    Curl_async_thrdd_multi_destroy(multi, !multi->quick_exit);
#جے_انت
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
#جے_تعریف_شدہ USE_SSL
    Curl_ssl_scache_destroy(multi->ssl_scache);
#جے_انت

#جے_تعریف_شدہ USE_WINSOCK
    WSACloseEvent(multi->wsa_event);
#جے_انت
#جے_تعریف_شدہ ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#جے_انت
#جے_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_اندرونی);
#جے_انت

    multi_xfer_bufs_free(multi);
    Curl_mntfy_cleanup(multi);
#جے_تعریف_شدہ DEBUGBUILD
    إذا(Curl_uint32_tbl_گنتی(&multi->xfers)) {
      multi_xfer_tbl_dump(multi);
      DEBUGASSERT(0);
    }
#جے_انت
    Curl_uint32_bset_destroy(&multi->process);
    Curl_uint32_bset_destroy(&multi->dirty);
    Curl_uint32_bset_destroy(&multi->pending);
    Curl_uint32_bset_destroy(&multi->msgsent);
    Curl_uint32_tbl_destroy(&multi->xfers);
    curlx_memzero(multi, حجم(*multi));
    curlx_free(multi);

    mresult = CURLM_OK;
  }
باہرلا:
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

/*
 * curl_multi_info_read()
 *
 * This function is the primary way for a multi/multi_socket application to
 * figure out if a transfer has ended.
 */

CURLMsg *curl_multi_info_read(CURLM *m, صحيح *msgs_وچ_قطار)
{
  بنية Curl_mapi_guard guard;
  CURLMsg *پیغام_نتیجہ = عدم;

  *msgs_وچ_قطار = 0; /* default to none */
  إذا(CURL_MAPI_ENTER(&guard, m, multi_info_read, عدم)) {
    بنية Curl_multi *multi = m;
    uint32_t mid;
    إذا(Curl_uint32_bset_پہلا(&multi->msgsent, &mid)) {
      بنية Curl_easy *انکڑے = Curl_multi_get_easy(multi, mid);

      DEBUGASSERT(انکڑے);
      Curl_uint32_bset_remove(&multi->msgsent, mid);
      *msgs_وچ_قطار =
        curlx_uztosi(Curl_uint32_bset_گنتی(&multi->msgsent));
      إذا(انکڑے)
        پیغام_نتیجہ = &انکڑے->پیغام;
    }
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع پیغام_نتیجہ;
}

فراغ Curl_multi_will_close(بنية Curl_easy *انکڑے, curl_socket_t s)
{
  إذا(انکڑے) {
    بنية Curl_multi *multi = انکڑے->multi;
    إذا(multi) {
      CURL_TRC_M(انکڑے, "Curl_multi_will_close fd=%" FMT_SOCKET_T, s);
      Curl_multi_ev_socket_پورا(multi, انکڑے, s);
    }
  }
}

ساكن فراغ multi_timeouts_init(بنية Curl_easy *انکڑے)
{
  انکڑے->state.timeouts.پہلا = EXPIRE_آخری;
  انکڑے->state.timeouts.splaynode.registered = FALSE;
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
ساكن CURLMcode جوڑو_اگلا_timeout(ثابت بنية curltime *pnow,
                                  بنية Curl_multi *multi,
                                  بنية Curl_easy *انکڑے)
{
  بنية expire_timers *timeouts = &انکڑے->state.timeouts;
  timediff_t now_us = Curl_timeouts_وتھ_us(&multi->timeouts, pnow);

  طالما(timeouts->پہلا < EXPIRE_آخری) {
    إذا(timeouts->وتھ_us[timeouts->پہلا] <= now_us)  /* already expired */
      timeouts->پہلا = timeouts->اگلا[timeouts->پہلا];
    وإلا /* timeouts are sorted, first is first in the future now */
      اكسر;
  }

  إذا(timeouts->پہلا < EXPIRE_آخری) {
    /* Insert this node again into the splay. Keep the timer in the list in
       case we need to recompute future timers. */
    Curl_timeouts_جوڑو(&multi->timeouts, انکڑے,
                      timeouts->وتھ_us[timeouts->پہلا]);
  }
  أرجع CURLM_OK;
}

ساكن فراغ multi_mark_expired_as_dirty(بنية Curl_multi *multi,
                                        ثابت بنية curltime *ts)
{
  بنية Curl_easy *انکڑے = عدم;
  uint32_t mid;

  /*
   * The loop following here will go on as long as there are expire-times left
   * to process (compared to `ts`) in the splay and 'data' will be
   * re-assigned for every expired handle we deal with.
   */
  طالما(Curl_timeouts_remove_expired(&multi->timeouts, ts, &mid)) {
    /* Check if there is one (more) expired timer to deal with! This function
       extracts a matching node if there is one */
    انکڑے = Curl_multi_get_easy(multi, mid);
    إذا(!انکڑے) {
      DEBUGASSERT(0);
      استمر;
    }
#جے_تعریف_شدہ CURLVERBOSE
    إذا(CURL_TRC_TIMER_is_verbose(انکڑے)) {
      إذا(انکڑے->state.timeouts.پہلا < EXPIRE_آخری) {
        CURL_TRC_TIMER(انکڑے, انکڑے->state.timeouts.پہلا, "has expired");
      }
    }
#جے_انت
    (فراغ)جوڑو_اگلا_timeout(ts, multi, انکڑے);
    Curl_multi_mark_dirty(انکڑے);
  }
}

ساكن CURLMcode multi_لگاتار_dirty(بنية Curl_multi *multi,
                                 بنية Curl_sigpipe_ctx *sigpipe_ctx,
                                 uint32_t *pnum)
{
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;

  *pnum = 0;
  إذا(Curl_uint32_bset_پہلا(&multi->dirty, &mid)) {
    افعل {
      بنية Curl_easy *انکڑے = Curl_multi_get_easy(multi, mid);
      إذا(انکڑے) {
        CURL_TRC_M(انکڑے, "multi_run_dirty");

        إذا(!Curl_uint32_bset_شامل_اے(&multi->process, mid)) {
          إذا(Curl_uint32_bset_شامل_اے(&multi->pending, mid)) {
            /* Something happened on the pending transfer, act on it. */
            move_pending_منزل_connect(multi, انکڑے);
          }
          وإلا {
            /* We are no longer processing this transfer */
            Curl_uint32_bset_remove(&multi->dirty, mid);
            استمر;
          }
        }

        (*pnum)++;
        /* runsingle() clears the dirty mid */
        mresult = multi_runsingle(multi, انکڑے, sigpipe_ctx);

        إذا(CURLM_OK >= mresult) {
          /* reassess event handling of data */
          mresult = Curl_multi_ev_assess_xfer(multi, انکڑے);
          إذا(mresult)
            اذهب باہرلا;
        }
      }
      وإلا {
        CURL_TRC_M(multi->admin, "multi_run_dirty, %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } طالما(Curl_uint32_bset_اگلا(&multi->dirty, mid, &mid));
  }

باہرلا:
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
  uint32_t لگاتار_xfers;

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
    اذهب باہرلا;
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
    multi->آخری_expire_وتھ_us = 0;

    /* Applications may set `socket_cb` *after* having added transfers
     * first. *Then* kick off processing with a
     * curl_multi_socket_action(TIMEOUT) afterwards. Make sure our
     * admin handle registers its pollset with the callbacks present. */
    mresult = multi_assess_wakeup(multi);
    إذا(mresult)
      اذهب باہرلا;
  }

  curlx_pnow(&now);
  multi_mark_expired_as_dirty(multi, &now);
  mresult = multi_لگاتار_dirty(multi, &pipe_ctx, &لگاتار_xfers);
  إذا(mresult)
    اذهب باہرلا;

  إذا(لگاتار_xfers) {
    /* Running transfers takes time. With a new timestamp, we might catch
     * other expires which are due now. Instead of telling the application
     * to set a 0 timeout and call us again, we run them here.
     * Do that only once or it might be unfair to transfers on other
     * sockets. */
    curlx_pnow(&now);
    multi_mark_expired_as_dirty(multi, &now);
    mresult = multi_لگاتار_dirty(multi, &pipe_ctx, &لگاتار_xfers);
  }

باہرلا:
  sigpipe_restore(&pipe_ctx);

  إذا(multi_ischanged(multi, TRUE))
    multi_ویلا_سارنی_pending(multi);

  إذا(!mresult && CURL_MNTFY_HAS_ENTRIES(multi))
    mresult = Curl_mntfy_dispatch_all(multi);

  إذا(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < ودھ_توں_ودھ_عدد) ? (صحيح)running : ودھ_توں_ودھ_عدد;
  }

  إذا(CURLM_OK >= mresult)
    mresult = Curl_update_timer(multi);
  أرجع mresult;
}

#تعریف_رد curl_multi_setopt
CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_setopt, &mresult)) {
    بنية Curl_multi *multi = m;
    va_list param;
    بنانشانی لما uarg;
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
      multi->ڈالو_cb = va_arg(param, curl_ڈالو_callback);
      اكسر;
    حالة CURLMOPT_PUSHDATA:
      multi->ڈالو_userp = va_arg(param, فراغ *);
      اكسر;
    حالة CURLMOPT_PIPELINING:
      multi->multiplexing = va_arg(param, لما) & CURLPIPE_MULTIPLEX ? 1 : 0;
      اكسر;
    حالة CURLMOPT_TIMERFUNCTION:
      multi->timer_cb = va_arg(param, curl_multi_timer_callback);
      اكسر;
    حالة CURLMOPT_TIMERDATA:
      multi->timer_userp = va_arg(param, فراغ *);
      اكسر;
    حالة CURLMOPT_MAXCONNECTS:
      uarg = va_arg(param, بنانشانی لما);
      إذا(uarg <= UINT32_MAX)
        multi->maxconnects = (uint32_t)uarg;
      اكسر;
    حالة CURLMOPT_MAX_HOST_CONNECTIONS:
      إذا(!curlx_sltouz(va_arg(param, لما), &szarg))
        mresult = CURLM_BAD_FUNCTION_دلیل;
      multi->max_host_connections = (szarg < UINT32_MAX) ?
                                    (uint32_t)szarg : UINT32_MAX;
      اكسر;
    حالة CURLMOPT_MAX_کل_CONNECTIONS:
      إذا(!curlx_sltouz(va_arg(param, لما), &szarg))
        mresult = CURLM_BAD_FUNCTION_دلیل;
      multi->max_کل_connections = (szarg < UINT32_MAX) ?
                                     (uint32_t)szarg : UINT32_MAX;
      اكسر;
      /* options formerly used for pipelining */
    حالة CURLMOPT_MAX_PIPELINE_لمائی:
      اكسر;
    حالة CURLMOPT_CONTENT_لمائی_PENALTY_ناپ:
      اكسر;
    حالة CURLMOPT_CHUNK_لمائی_PENALTY_ناپ:
      اكسر;
    حالة CURLMOPT_PIPELINING_SITE_BL:
      اكسر;
    حالة CURLMOPT_PIPELINING_SERVER_BL:
      اكسر;
    حالة CURLMOPT_MAX_CONCURRENT_STREAMS: {
      إذا(!curlx_sltouz(va_arg(param, لما), &szarg) ||
         !szarg || (szarg > (size_t)ودھ_توں_ودھ_عدد)) /* preserve previous cutoff */
        multi->max_concurrent_streams = 100;
      وإلا
        multi->max_concurrent_streams = (szarg < UINT32_MAX) ?
                                       (uint32_t)szarg : UINT32_MAX;
      اكسر;
    }
    حالة CURLMOPT_جال_بدلیا: {
      لما val = va_arg(param, لما);
      إذا(val & CURLMNWC_CLEAR_ALL)
        /* In the beginning, all values available to set were 1 by mistake. We
           converted this to mean "all", thus setting all the bits
           automatically */
        val = CURLMNWC_CLEAR_DNS | CURLMNWC_CLEAR_CONNS;
      إذا(val & CURLMNWC_CLEAR_DNS) {
        Curl_dnscache_clear(multi->admin);
      }
      إذا(val & CURLMNWC_CLEAR_CONNS) {
        Curl_cpool_nw_بدلیا(&multi->cpool, multi->admin);
      }
      اكسر;
    }
    حالة CURLMOPT_NOTIFYFUNCTION:
      multi->ntfy.ntfy_cb = va_arg(param, curl_notify_callback);
      اكسر;
    حالة CURLMOPT_NOTIFYDATA:
      multi->ntfy.ntfy_cb_انکڑے = va_arg(param, فراغ *);
      اكسر;
    حالة CURLMOPT_RESOLVE_دھاگے_MAX:
#جے_تعریف_شدہ USE_RESOLV_THREADED
      uarg = va_arg(param, لما);
      إذا((uarg <= 0) || (uarg > UINT32_MAX))
        mresult = CURLM_BAD_FUNCTION_دلیل;
      وإلا {
        CURLcode نتیجہ = Curl_async_thrdd_multi_سیٹ_props(
          multi, 0, (uint32_t)uarg, 2000);
        بدل(نتیجہ) {
        حالة CURLE_OK:
          mresult = CURLM_OK;
          اكسر;
        حالة CURLE_BAD_FUNCTION_دلیل:
          mresult = CURLM_BAD_FUNCTION_دلیل;
          اكسر;
        حالة CURLE_باہرلا_دا_حافظہ:
          mresult = CURLM_باہرلا_دا_حافظہ;
          اكسر;
        افتراضي:
          mresult = CURLM_اندرونی_ERROR;
          اكسر;
        }
      }
#جے_انت
      اكسر;
    حالة CURLMOPT_تیز_EXIT:
      multi->quick_exit = va_arg(param, لما) ? 1 : 0;
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
#تعریف_رد curl_multi_socket

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
  إذا(Curl_uint32_bset_پہلا(&multi->dirty, &mid)) {
    افعل {
      بنية Curl_easy *انکڑے = Curl_multi_get_easy(multi, mid);
      إذا(انکڑے) {
        إذا(Curl_uint32_bset_شامل_اے(&multi->process, mid))
          أرجع TRUE;
        /* We are no longer processing this transfer */
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
      وإلا {
        CURL_TRC_M(multi->admin, "dirty transfer %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } طالما(Curl_uint32_bset_اگلا(&multi->dirty, mid, &mid));
  }
  أرجع FALSE;
}

ساكن فراغ multi_timeout(بنية Curl_multi *multi,
                          timediff_t *pexire_وتھ_us,
                          صحيح *timeout_ms)
{
  إذا(multi->dead) {
    إذا(pexire_وتھ_us)
      *pexire_وتھ_us = 0;
    *timeout_ms = 0;
    أرجع;
  }

  إذا(multi_has_dirties(multi)) {
    إذا(pexire_وتھ_us) {
      بنية curltime now;
      curlx_pnow(&now);
      *pexire_وتھ_us = Curl_timeouts_وتھ_us(&multi->timeouts, &now);
    }
    *timeout_ms = 0;
    أرجع;
  }
  وإلا {
    بنية curltime now;
    uint32_t mid;

    curlx_pnow(&now);
    *timeout_ms = Curl_timeouts_اگلا_ms(&multi->timeouts, &now,
                                        pexire_وتھ_us, &mid);
#جے_تعریف_شدہ CURLVERBOSE
    إذا(mid != UINT32_MAX) {
      بنية Curl_easy *انکڑے = Curl_multi_get_easy(multi, mid);
      إذا(انکڑے && CURL_TRC_TIMER_is_verbose(انکڑے) &&
         (انکڑے->state.timeouts.پہلا < EXPIRE_آخری)) {
        CURL_TRC_TIMER(انکڑے, انکڑے->state.timeouts.پہلا,
                       "gives multi timeout in %dms", *timeout_ms);
      }
    }
#جے_انت
  }
}

CURLMcode curl_multi_timeout(CURLM *m,
                             لما *timeout_ms)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_timeout, &mresult)) {
    صحيح itimeout_ms;

    multi_timeout(m, عدم, &itimeout_ms);
    *timeout_ms = (لما)itimeout_ms;
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
  timediff_t timeouts_وتھ_us = 0;
  صحيح timeout_ms;
  صحيح rc;
  bool سیٹ_قدر = FALSE;

  إذا(!multi->timer_cb || multi->dead)
    أرجع CURLM_OK;
  multi_timeout(multi, &timeouts_وتھ_us, &timeout_ms);

  إذا(timeout_ms < 0 && !multi->آخری_timeout_سیٹ) {
    /* nothing to do */
  }
  وإلا إذا(timeout_ms < 0) {
    /* there is no timeout now but there was one previously */
    CURL_TRC_M(multi->admin, "[TIMER] clear");
    timeout_ms = -1; /* normalize */
    سیٹ_قدر = TRUE;
  }
  وإلا إذا(!multi->آخری_timeout_سیٹ) {
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, none before", timeout_ms);
    سیٹ_قدر = TRUE;
  }
  وإلا إذا(multi->آخری_expire_وتھ_us != timeouts_وتھ_us) {
    /* We had a timeout before and have one now, the absolute timestamp
     * differs. The relative timeout_ms may be the same, but the starting
     * point differs. Let the application restart its timer. */
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, replace previous",
               timeout_ms);
    سیٹ_قدر = TRUE;
  }
  وإلا {
    /* We have same expire time as previously. Our relative 'timeout_ms'
     * may be different now, but the application has the timer running
     * and we do not to tell it to start this again. */
  }

  إذا(سیٹ_قدر) {
    بنية Curl_mapi_guard guard;

    multi->آخری_expire_وتھ_us = timeouts_وتھ_us;
    multi->آخری_timeout_سیٹ = timeout_ms >= 0;
    CURL_CBAPI_MULTI_شروع(&guard, multi, multi_timer_cb);
    rc = multi->timer_cb(multi, timeout_ms, multi->timer_userp);
    CURL_CBAPI_MULTI_سرا(&guard);
    إذا(rc == -1) {
      multi->dead = TRUE;
      أرجع CURLM_ABORTED_BY_CALLBACK;
    }
  }
  أرجع CURLM_OK;
}

#جے_تعریف_شدہ DEBUGBUILD
ساكن bool multi_timeouts_check(بنية Curl_easy *انکڑے)
{
  بنية expire_timers *timeouts = &انکڑے->state.timeouts;
  uint8_t id;
  صحيح i = 0;
  لكل(id = timeouts->پہلا; id < EXPIRE_آخری; id = timeouts->اگلا[id]) {
    إذا(++i >= EXPIRE_آخری) {
      failf(انکڑے, "expire timeouts looped: %d iterations and no end", i);
      أرجع FALSE;
    }
    إذا(id == timeouts->اگلا[id]) {
      failf(انکڑے, "expire timeouts wrong: %d points to itself", (صحيح)id);
      أرجع FALSE;
    }
    إذا((timeouts->اگلا[id] < EXPIRE_آخری) &&
       (timeouts->وتھ_us[id] > timeouts->وتھ_us[timeouts->اگلا[id]])) {
      failf(انکڑے, "expire timeouts not sorted: %d happens after %d but "
            "is listed before", (صحيح)id, (صحيح)timeouts->اگلا[id]);
      أرجع FALSE;
    }
  }
  أرجع TRUE;
}
#جے_انت

/*
 * Remove a given timestamp from the list of timeouts.
 */
ساكن فراغ multi_clear_timeout(بنية Curl_easy *انکڑے, expire_id eid)
{
  بنية expire_timers *timeouts = &انکڑے->state.timeouts;
  uint8_t orig_پہلا = timeouts->پہلا;
  uint8_t *anchor = &timeouts->پہلا;
  uint8_t id = (uint8_t)eid;

  إذا((بنانشانی)eid >= EXPIRE_آخری) {
    DEBUGASSERT(0);
    أرجع;
  }

  طالما(*anchor < EXPIRE_آخری) {
    إذا(*anchor == id) {
      *anchor = timeouts->اگلا[id];
      اكسر;
    }
    anchor = &timeouts->اگلا[*anchor];
  }
  DEBUGASSERT(multi_timeouts_check(انکڑے));
  إذا(Curl_timeouts_has(انکڑے)) {
    بنية Curl_multi *multi = انکڑے->multi;

    إذا(!multi) {
      DEBUGASSERT(0);
      أرجع;
    }
    إذا((timeouts->پہلا >= EXPIRE_آخری) || /* no more timeouts */
       (timeouts->پہلا != orig_پہلا)) {  /* active timeout changed */
      Curl_timeouts_remove(&multi->timeouts, انکڑے);
    }
    إذا((timeouts->پہلا < EXPIRE_آخری) && !Curl_timeouts_has(انکڑے)) {
      Curl_timeouts_جوڑو(&multi->timeouts, انکڑے,
                        timeouts->وتھ_us[timeouts->پہلا]);
    }
  }
}

/*
 * Add a timestamp to the list of timeouts. Keep the list sorted so that head
 * of list is always the timeout nearest in time.
 */
ساكن CURLMcode multi_سیٹ_timeout(بنية Curl_easy *انکڑے,
                                   ثابت بنية curltime *stamp,
                                   expire_id eid)
{
  بنية expire_timers *timeouts = &انکڑے->state.timeouts;
  uint8_t *anchor = &timeouts->پہلا;
  uint8_t id = (uint8_t)eid;

  إذا((بنانشانی)eid >= EXPIRE_آخری) {
    DEBUGASSERT(0);
    أرجع CURLM_BAD_FUNCTION_دلیل;
  }
  /* remove from list, store time and re-insert */
  multi_clear_timeout(انکڑے, eid);
  timeouts->وتھ_us[id] =
    Curl_timeouts_وتھ_us(&انکڑے->multi->timeouts, stamp);

  طالما(*anchor < EXPIRE_آخری) {
    إذا(timeouts->وتھ_us[*anchor] > timeouts->وتھ_us[id])
      اكسر;
    anchor = &timeouts->اگلا[*anchor];
  }
  timeouts->اگلا[eid] = *anchor;
  timeouts->اگلا[eid] = *anchor;
  *anchor = id;
  DEBUGASSERT(multi_timeouts_check(انکڑے));
  CURL_TRC_TIMER(انکڑے, eid, "set for %" FMT_TIMEDIFF_T "us",
                 curlx_ptimediff_us(stamp, Curl_pgrs_now(انکڑے)));
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
فراغ Curl_expire_سیٹ(بنية Curl_easy *انکڑے,
                     expire_id eid, timediff_t ms,
                     ثابت بنية curltime *pnow)
{
  بنية Curl_multi *multi = انکڑے->multi;
  بنية expire_timers *timeouts = &انکڑے->state.timeouts;
  uint8_t prev_id = timeouts->پہلا;
  بنية curltime سیٹ;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  إذا(!multi)
    أرجع;
  DEBUGASSERT(eid < EXPIRE_آخری);
  إذا(ms > ودھ_توں_ودھ_عدد)
    /* Cap ridiculous timeouts, 31-bit ms is still 3.5 weeks. When the time
       goes to the user, it must fit in this size. */
    ms = ودھ_توں_ودھ_عدد;

  سیٹ = *pnow;
  سیٹ.tv_sec += (time_t)(ms / 1000); /* may be a 64 to 32-bit conversion */
  سیٹ.tv_usec += (صحيح)(ms % 1000) * 1000;
  إذا(سیٹ.tv_usec >= 1000000) {
    سیٹ.tv_sec++;
    سیٹ.tv_usec -= 1000000;
  }

  /* Add the timeout, will replace any previous value for this timer. */
  multi_سیٹ_timeout(انکڑے, &سیٹ, eid);
  DEBUGASSERT(timeouts->پہلا < EXPIRE_آخری);

  إذا(Curl_timeouts_has(انکڑے)) {
    /* data has already a timeout registered. If the first timer
     * was NOT the one we just set AND is still the first one,
     * nothing changed from the timeouts point of view. The
     * set timer triggers after the one already registered. Leave. */
    إذا((prev_id != eid) && (prev_id == timeouts->پہلا))
      أرجع;

    /* Since this is an updated time, we must remove data from
     * timeouts and then add it again. */
    Curl_timeouts_remove(&multi->timeouts, انکڑے);
  }

  /* Insert the new timer expiry since it is our local minimum. */
  Curl_timeouts_جوڑو(&multi->timeouts, انکڑے,
                    timeouts->وتھ_us[timeouts->پہلا]);
}

فراغ Curl_expire(بنية Curl_easy *انکڑے,
                 timediff_t milli, expire_id eid)
{
  Curl_expire_سیٹ(انکڑے, eid, milli, Curl_pgrs_now(انکڑے));
}

/*
 * Removes the expire timer. Marks it as done.
 */
فراغ Curl_expire_clear(بنية Curl_easy *انکڑے, expire_id eid)
{
  /* remove the timer, if there */
  multi_clear_timeout(انکڑے, eid);
  CURL_TRC_TIMER(انکڑے, eid, "cleared");
}

/*
 * Clear ALL timeout values for this handle.
 */
فراغ Curl_expire_clear_all(بنية Curl_easy *انکڑے)
{
  بنية Curl_multi *multi = انکڑے->multi;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  إذا(!multi)
    أرجع;

  إذا(Curl_timeouts_remove(&multi->timeouts, انکڑے)) {
    /* Since this is an cleared time, we must remove the previous entry from
       the splay tree */
    multi_timeouts_init(انکڑے);

    إذا(انکڑے->id >= 0)
      CURL_TRC_M(انکڑے, "[TIMEOUT] all cleared");
  }
}

CURLMcode curl_multi_سونپو(CURLM *m, curl_socket_t sockfd,
                            فراغ *sockp)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_سونپو, &mresult)) {
    mresult = Curl_multi_ev_سونپو(m, sockfd, sockp);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

ساكن فراغ move_pending_منزل_connect(بنية Curl_multi *multi,
                                    بنية Curl_easy *انکڑے)
{
  DEBUGASSERT(انکڑے->mstate == MSTATE_PENDING);

  /* Remove this node from the pending set, add into process set */
  Curl_uint32_bset_remove(&multi->pending, انکڑے->mid);
  Curl_uint32_bset_جوڑو(&multi->process, انکڑے->mid);

  multistate(انکڑے, MSTATE_CONNECT);
  Curl_multi_mark_dirty(انکڑے); /* make it run */
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
ساكن فراغ multi_ویلا_سارنی_pending(بنية Curl_multi *multi)
{
  uint32_t mid = multi->آخری_pending_mid;

  إذا(mid) {
    طالما(Curl_uint32_bset_اگلا(&multi->pending, mid, &mid)) {
      بنية Curl_easy *انکڑے = Curl_multi_get_easy(multi, mid);
      إذا(انکڑے) {
        move_pending_منزل_connect(multi, انکڑے);
        multi->آخری_pending_mid = mid;
        أرجع;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    }
    /* found no pending transfers with `mid` larger than `last_pending_mid`.
     * Start at the beginning of the pending set again. */
    multi->آخری_pending_mid = 0;
  }

  إذا(Curl_uint32_bset_پہلا(&multi->pending, &mid)) {
    افعل {
      بنية Curl_easy *انکڑے = Curl_multi_get_easy(multi, mid);
      إذا(انکڑے) {
        move_pending_منزل_connect(multi, انکڑے);
        multi->آخری_pending_mid = mid;
        أرجع;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    } طالما(Curl_uint32_bset_اگلا(&multi->pending, mid, &mid));
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
    فراغ *اندراج;
    size_t گنتی = Curl_uint32_tbl_گنتی(&multi->xfers);

    a = curlx_malloc(حجم(بنية Curl_easy *) * (گنتی + 1));
    إذا(a) {
      بنانشانی صحيح i = 0;
      uint32_t mid;

      إذا(Curl_uint32_tbl_پہلا(&multi->xfers, &mid, &اندراج)) {
        افعل {
          بنية Curl_easy *انکڑے = اندراج;
          DEBUGASSERT(i < گنتی);
          إذا(!انکڑے->state.اندرونی)
            a[i++] = انکڑے;
        } طالما(Curl_uint32_tbl_اگلا(&multi->xfers, mid, &mid, &اندراج));
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
      اذهب باہرلا;
    }

    بدل(info) {
    حالة CURLMINFO_XFERS_CURRENT:
      n = Curl_uint32_tbl_گنتی(&multi->xfers);
      إذا(n && multi->admin)
        --n;
      *pvalue = (curl_off_t)n;
      اكسر;
    حالة CURLMINFO_XFERS_RUNNING:
      n = Curl_uint32_bset_گنتی(&multi->process);
      إذا(n && Curl_uint32_bset_شامل_اے(&multi->process, multi->admin->mid))
        --n;
      *pvalue = (curl_off_t)n;
      اكسر;
    حالة CURLMINFO_XFERS_PENDING:
      *pvalue = (curl_off_t)Curl_uint32_bset_گنتی(&multi->pending);
      اكسر;
    حالة CURLMINFO_XFERS_پورا:
      *pvalue = (curl_off_t)Curl_uint32_bset_گنتی(&multi->msgsent);
      اكسر;
    حالة CURLMINFO_XFERS_ADDED:
      *pvalue = multi->xfers_کل_ever;
      اكسر;
    افتراضي:
      *pvalue = -1;
      mresult = CURLM_UNKNOWN_OPTION;
      اكسر;
    }
  }
باہرلا:
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

CURLcode Curl_multi_xfer_buf_borrow(بنية Curl_easy *انکڑے,
                                    حرف **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(انکڑے);
  DEBUGASSERT(انکڑے->multi);
  *pbuf = عدم;
  *pbuflen = 0;
  إذا(!انکڑے->multi) {
    failf(انکڑے, "transfer has no multi handle");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(!انکڑے->سیٹ.بفر_ناپ) {
    failf(انکڑے, "transfer buffer size is 0");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(انکڑے->multi->xfer_buf_borrowed) {
    failf(انکڑے, "attempt to borrow xfer_buf when already borrowed");
    أرجع CURLE_AGAIN;
  }

  إذا(انکڑے->multi->xfer_buf &&
     انکڑے->سیٹ.بفر_ناپ > انکڑے->multi->xfer_buf->len) {
    /* not large enough, get a new one */
    curlx_safefree(انکڑے->multi->xfer_buf);
  }

  إذا(!انکڑے->multi->xfer_buf) {
    انکڑے->multi->xfer_buf =
      fixed_buf_create(curlx_uitouz(انکڑے->سیٹ.بفر_ناپ));
    إذا(!انکڑے->multi->xfer_buf) {
      failf(انکڑے, "could not allocate xfer_buf of %u bytes",
            انکڑے->سیٹ.بفر_ناپ);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }
  }

  انکڑے->multi->xfer_buf_borrowed = TRUE;
  *pbuf = انکڑے->multi->xfer_buf->انکڑے;
  *pbuflen = انکڑے->multi->xfer_buf->len;
  أرجع CURLE_OK;
}

فراغ Curl_multi_xfer_buf_release(بنية Curl_easy *انکڑے, حرف *buf)
{
  (فراغ)buf;
  DEBUGASSERT(انکڑے);
  DEBUGASSERT(انکڑے->multi);
  DEBUGASSERT(!buf || (انکڑے->multi->xfer_buf &&
                       انکڑے->multi->xfer_buf->انکڑے == buf));
  انکڑے->multi->xfer_buf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_ulbuf_borrow(بنية Curl_easy *انکڑے,
                                      حرف **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(انکڑے);
  DEBUGASSERT(انکڑے->multi);
  *pbuf = عدم;
  *pbuflen = 0;
  إذا(!انکڑے->multi) {
    failf(انکڑے, "transfer has no multi handle");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(انکڑے->multi->xfer_ulbuf_borrowed) {
    failf(انکڑے, "attempt to borrow xfer_ulbuf when already borrowed");
    أرجع CURLE_AGAIN;
  }

  إذا(انکڑے->multi->xfer_ulbuf &&
     انکڑے->سیٹ.upload_بفر_ناپ > انکڑے->multi->xfer_ulbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(انکڑے->multi->xfer_ulbuf);
  }

  إذا(!انکڑے->multi->xfer_ulbuf) {
    انکڑے->multi->xfer_ulbuf =
      fixed_buf_create(curlx_uitouz(انکڑے->سیٹ.upload_بفر_ناپ));
    إذا(!انکڑے->multi->xfer_ulbuf) {
      failf(انکڑے, "could not allocate xfer_ulbuf of %u bytes",
            انکڑے->سیٹ.upload_بفر_ناپ);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }
  }

  انکڑے->multi->xfer_ulbuf_borrowed = TRUE;
  *pbuf = انکڑے->multi->xfer_ulbuf->انکڑے;
  *pbuflen = انکڑے->multi->xfer_ulbuf->len;
  أرجع CURLE_OK;
}

فراغ Curl_multi_xfer_ulbuf_release(بنية Curl_easy *انکڑے, حرف *buf)
{
  (فراغ)buf;
  DEBUGASSERT(انکڑے);
  DEBUGASSERT(انکڑے->multi);
  DEBUGASSERT(!buf || (انکڑے->multi->xfer_ulbuf &&
                       انکڑے->multi->xfer_ulbuf->انکڑے == buf));
  انکڑے->multi->xfer_ulbuf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_sockbuf_borrow(بنية Curl_easy *انکڑے,
                                        size_t blen, حرف **pbuf)
{
  DEBUGASSERT(انکڑے);
  *pbuf = عدم;
  إذا(!انکڑے->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    *pbuf = curlx_malloc(blen);
    أرجع *pbuf ? CURLE_OK : CURLE_باہرلا_دا_حافظہ;
  }
  إذا(انکڑے->multi->xfer_sockbuf_borrowed) {
    failf(انکڑے, "attempt to borrow xfer_sockbuf when already borrowed");
    أرجع CURLE_AGAIN;
  }

  إذا(انکڑے->multi->xfer_sockbuf && blen > انکڑے->multi->xfer_sockbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(انکڑے->multi->xfer_sockbuf);
  }

  إذا(!انکڑے->multi->xfer_sockbuf) {
    انکڑے->multi->xfer_sockbuf = fixed_buf_create(blen);
    إذا(!انکڑے->multi->xfer_sockbuf) {
      failf(انکڑے, "could not allocate xfer_sockbuf of %zu bytes", blen);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }
  }

  انکڑے->multi->xfer_sockbuf_borrowed = TRUE;
  *pbuf = انکڑے->multi->xfer_sockbuf->انکڑے;
  أرجع CURLE_OK;
}

فراغ Curl_multi_xfer_sockbuf_release(بنية Curl_easy *انکڑے, حرف *buf)
{
  DEBUGASSERT(انکڑے);
  إذا(!انکڑے->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    curlx_free(buf);
  }
  وإلا {
    DEBUGASSERT(!buf || (انکڑے->multi->xfer_sockbuf &&
                         انکڑے->multi->xfer_sockbuf->انکڑے == buf));
    انکڑے->multi->xfer_sockbuf_borrowed = FALSE;
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
  بنية Curl_easy *انکڑے = Curl_uint32_tbl_get(&multi->xfers, mid);
  إذا(GOOD_EASY_HANDLE(انکڑے))
    أرجع انکڑے;
  CURL_TRC_M(multi->admin, "invalid easy handle in xfer table for mid=%u",
             mid);
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  أرجع عدم;
}

bool Curl_multi_knows_easy(بنية Curl_multi *multi, بنية Curl_easy *انکڑے)
{
  أرجع Curl_uint32_tbl_get(&multi->xfers, انکڑے->mid) == انکڑے;
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
  أرجع Curl_uint32_tbl_گنتی(&multi->xfers) - 1;
}

فراغ Curl_multi_mark_dirty(بنية Curl_easy *انکڑے)
{
  إذا(انکڑے->multi && انکڑے->mid != UINT32_MAX)
    Curl_uint32_bset_جوڑو(&انکڑے->multi->dirty, انکڑے->mid);
}

فراغ Curl_multi_clear_dirty(بنية Curl_easy *انکڑے)
{
  إذا(انکڑے->multi && انکڑے->mid != UINT32_MAX)
    Curl_uint32_bset_remove(&انکڑے->multi->dirty, انکڑے->mid);
}

CURLMcode curl_multi_notify_enable(CURLM *m, بنانشانی صحيح notification)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_notify_enable, &mresult)) {
    mresult = Curl_mntfy_enable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

CURLMcode curl_multi_notify_disable(CURLM *m, بنانشانی صحيح notification)
{
  بنية Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  إذا(CURL_MAPI_ENTER(&guard, m, multi_notify_disable, &mresult)) {
    mresult = Curl_mntfy_disable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  أرجع mresult;
}

#جے_تعریف_شدہ DEBUGBUILD
ساكن فراغ multi_xfer_dump(بنية Curl_multi *multi, uint32_t mid,
                            فراغ *اندراج)
{
  بنية Curl_easy *انکڑے = اندراج;

  (فراغ)multi;
  إذا(!انکڑے) {
    curl_mfprintf(stderr, "mid=%u, entry=NULL, bug in xfer table?\n", mid);
  }
  وإلا {
    curl_mfprintf(stderr, "mid=%u, magic=%s, p=%p, id=%" FMT_OFF_T
                  ", url=%s\n",
                  mid,
                  (انکڑے->magic == CURLEASY_MAGIC_نمبر) ? "GOOD" : "BAD!",
                  (فراغ *)انکڑے, انکڑے->id, Curl_bufref_ptr(&انکڑے->state.url));
  }
}

ساكن فراغ multi_xfer_tbl_dump(بنية Curl_multi *multi)
{
  uint32_t mid;
  فراغ *اندراج;
  curl_mfprintf(stderr, "=== multi xfer table (count=%u, capacity=%u\n",
                Curl_uint32_tbl_گنتی(&multi->xfers),
                Curl_uint32_tbl_سمرتھا(&multi->xfers));
  إذا(Curl_uint32_tbl_پہلا(&multi->xfers, &mid, &اندراج)) {
    multi_xfer_dump(multi, mid, اندراج);
    طالما(Curl_uint32_tbl_اگلا(&multi->xfers, mid, &mid, &اندراج))
      multi_xfer_dump(multi, mid, اندراج);
  }
  curl_mfprintf(stderr, "===\n");
  fflush(stderr);
}
#جے_انت /* DEBUGBUILD */
