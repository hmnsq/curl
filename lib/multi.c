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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "transfer.h"
#شامل "url.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "progress.h"
#شامل "curl_share.h"
#شامل "psl.h"
#شامل "multiif.h"
#شامل "multi_ev.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "http.h"
#شامل "select.h"
#شامل "curlx/wait.h"
#شامل "conncache.h"
#شامل "multihandle.h"
#شامل "sigpipe.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_scache.h"
#شامل "http_proxy.h"
#شامل "http2.h"
#شامل "socketpair.h"
#شامل "bufref.h"
#شامل "curlx/strparse.h"

/* initial multi->xfers table size for a full multi */
#تعریف_کرو CURL_XFER_جدول_سائز 128

/* CURL_SOCKET_HASH_TABLE_SIZE should be a prime number. Increasing it from 97
   to 911 takes on a 32-bit machine 4 x 804 = 3211 more bytes. Still, every
   curl handle takes 6K memory, therefore this 3K are not significant. */
#اگر_غیر_تعریف_شدہ CURL_SOCKET_HASH_جدول_سائز
#تعریف_کرو CURL_SOCKET_HASH_جدول_سائز 911
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_CONNECTION_HASH_سائز
#تعریف_کرو CURL_CONNECTION_HASH_سائز 97
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DNS_HASH_سائز
#تعریف_کرو CURL_DNS_HASH_سائز 71
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_TLS_SESSION_سائز
#تعریف_کرو CURL_TLS_SESSION_سائز 25
#اگر_ختم

ساکن خالی move_pending_مقصد_connect(ساخت Curl_multi *multi,
                                    ساخت Curl_easy *ڈیٹا);
ساکن CURLMcode جوڑو_اگلا_timeout(ثابت ساخت curltime *pnow,
                                  ساخت Curl_multi *multi,
                                  ساخت Curl_easy *ڈیٹا);
ساکن خالی multi_timeout(ساخت Curl_multi *multi,
                          timediff_t *pexire_انحراف_us,
                          صحیح *timeout_ms);
ساکن خالی multi_جدول_وقت_pending(ساخت Curl_multi *multi);
ساکن خالی multi_xfer_bufs_free(ساخت Curl_multi *multi);
#اگر_تعریف_شدہ DEBUGBUILD
ساکن خالی multi_xfer_tbl_dump(ساخت Curl_multi *multi);
#اگر_ختم
/* Get the # of transfers current in process/pending. */
ساکن uint32_t multi_xfers_running(ساخت Curl_multi *multi);

/* function pointer called once when entering a state */
قسمتعریف خالی (*mstate_enter_func)(ساخت Curl_easy *ڈیٹا,
                                  CURLMstate from_state);

ساکن خالی mstate_enter_connect(ساخت Curl_easy *ڈیٹا,
                                 CURLMstate from_state)
{
  (خالی)from_state;
  Curl_init_CONNECT(ڈیٹا);
}

ساکن خالی mstate_enter_did(ساخت Curl_easy *ڈیٹا,
                             CURLMstate from_state)
{
  (خالی)from_state;
  ڈیٹا->req.chunk = FALSE;
  Curl_pgrs_وقت(ڈیٹا, TIMER_PRETRANSFER);
  اگر(!CURL_REQ_WANT_SEND(ڈیٹا))
    Curl_pgrs_وقت(ڈیٹا, TIMER_POSTRANSFER);
}

ساکن خالی mstate_enter_مکمل(ساخت Curl_easy *ڈیٹا,
                              CURLMstate from_state)
{
  (خالی)from_state;
  CURLM_NTFY(ڈیٹا, CURLMNOTIFY_EASY_مکمل);
}

ساکن خالی mstate_enter_completed(ساخت Curl_easy *ڈیٹا,
                                   CURLMstate from_state)
{
  /* we sometimes directly jump to COMPLETED, trigger things
   * we then missed. */
  اگر(from_state < MSTATE_DID) {
    Curl_pgrs_وقت(ڈیٹا, TIMER_PRETRANSFER);
    Curl_pgrs_وقت(ڈیٹا, TIMER_POSTRANSFER);
    Curl_pgrs_وقت(ڈیٹا, TIMER_STARTTRANSFER);
  }
  /* multi_done() should have run and detached any connection already */
  DEBUGASSERT(!ڈیٹا->conn);
  Curl_pgrsCompleted(ڈیٹا);
  اگر(from_state < MSTATE_مکمل)
    CURLM_NTFY(ڈیٹا, CURLMNOTIFY_EASY_مکمل);
  /* changing to COMPLETED means it is in process and needs to go */
  DEBUGASSERT(Curl_uint32_bset_شامل_ہے(&ڈیٹا->multi->process, ڈیٹا->mid));
  Curl_uint32_bset_remove(&ڈیٹا->multi->process, ڈیٹا->mid);
  Curl_uint32_bset_remove(&ڈیٹا->multi->pending, ڈیٹا->mid); /* to be sure */

  اگر(Curl_uint32_bset_خالی_خانے(&ڈیٹا->multi->process)) {
    /* free the transfer buffer when we have no more active transfers */
    multi_xfer_bufs_free(ڈیٹا->multi);
  }
  /* Important: reset the conn pointer so that we do not point to memory
     that could be freed anytime */
  Curl_detach_connection(ڈیٹا);
  Curl_expire_clear_all(ڈیٹا); /* stop all timers */
}

/* always use this function to change state, to make debugging easier */
ساکن خالی mstate(ساخت Curl_easy *ڈیٹا, CURLMstate state
#اگر_تعریف_شدہ DEBUGBUILD
                   , صحیح lineno
#اگر_ختم
)
{
  CURLMstate oldstate = ڈیٹا->mstate;
  ساکن ثابت mstate_enter_func state_enter[MSTATE_آخری] = {
    تہی,                      /* INIT */
    تہی,                      /* PENDING */
    تہی,                      /*تیاری*/
    mstate_enter_connect,      /* CONNECT */
    تہی,                      /* CONNECTING */
    تہی,                      /* PROTOCONNECT */
    تہی,                      /* PROTOCONNECTING */
    تہی,                      /* DO */
    تہی,                      /* DOING */
    تہی,                      /* DOING_MORE */
    mstate_enter_did,          /* DID */
    تہی,                      /* PERFORMING */
    تہی,                      /* RATELIMITING */
    mstate_enter_مکمل,         /* DONE */
    mstate_enter_completed,    /* COMPLETED */
    تہی                       /* MSGSENT */
  };

  اگر(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    واپس;

#اگر_تعریف_شدہ DEBUGBUILD
  NOVERBOSE((خالی)lineno);
  CURL_TRC_M(ڈیٹا, "-> [%s] (line %d)", CURL_MSTATE_NAME(state), lineno);
#ورنہ
  CURL_TRC_M(ڈیٹا, "-> [%s]", CURL_MSTATE_NAME(state));
#اگر_ختم

  /* really switching state */
  ڈیٹا->mstate = state;
  اگر(state_enter[state])
    state_enter[state](ڈیٹا, oldstate);
}

#اگر_غیر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو multistate(x, y) mstate(x, y)
#ورنہ
#تعریف_کرو multistate(x, y) mstate(x, y, __LINE__)
#اگر_ختم

/* multi->proto_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
ساکن خالی ph_freeentry(خالی *p)
{
  (خالی)p;
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
ساکن خالی multi_addmsg(ساخت Curl_multi *multi, ساخت Curl_easy *ڈیٹا)
{
  اگر(Curl_uint32_bset_خالی_خانے(&multi->msgsent))
    CURLM_NTFY(multi->admin, CURLMNOTIFY_INFO_READ);
  Curl_uint32_bset_جوڑو(&multi->msgsent, ڈیٹا->mid);
}

ساکن خالی multi_timeouts_init(ساخت Curl_easy *ڈیٹا);

ساخت Curl_multi *Curl_multi_handle(uint32_t xfer_جدول_سائز,
                                     size_t ev_hashsize,  /* event hash */
                                     size_t chashsize, /* connection hash */
                                     size_t dnssize,   /* dns hash */
                                     size_t sesssize)  /* TLS session cache */
{
  ساخت curltime created;
  ساخت Curl_multi *multi = curlx_calloc(1, حجم(ساخت Curl_multi));

  اگر(!multi)
    واپس تہی;

  multi->magic = CURLMULTI_MAGIC_نمبر;

  /* Initialisation order is important here!
   * easy_init() does a lazy check on curl_global_init() which sets
   * up platform specific things we need. For example calling curlx_pnow()
   * before this is not safe. */
  multi->admin = curl_easy_init();
  اگر(!multi->admin) {
    curlx_free(multi);
    واپس تہی;
  }
  multi->admin->multi = multi;
  multi->admin->state.اندرونی_2 = TRUE;

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
  Curl_hash_init(&multi->proto_hash, 23, CURL_HASH_TYPE_بائٹیں, ph_freeentry);
  Curl_cshutdn_init(&multi->cshutdn);

  multi->multiplexing = TRUE;
  multi->max_concurrent_streams = 100;
#اگر_تعریف_شدہ ENABLE_WAKEUP
  multi->wakeup_pair[0] = CURL_SOCKET_BAD;
  multi->wakeup_pair[1] = CURL_SOCKET_BAD;
#اگر_ختم
#اگر_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  multi->wakeup_اندرونی[0] = CURL_SOCKET_BAD;
  multi->wakeup_اندرونی[1] = CURL_SOCKET_BAD;
#اگر_ختم

  اگر(Curl_uint32_bset_resize(&multi->process, xfer_جدول_سائز) ||
     Curl_uint32_bset_resize(&multi->pending, xfer_جدول_سائز) ||
     Curl_uint32_bset_resize(&multi->dirty, xfer_جدول_سائز) ||
     Curl_uint32_bset_resize(&multi->msgsent, xfer_جدول_سائز) ||
     Curl_uint32_tbl_resize(&multi->xfers, xfer_جدول_سائز))
    جاؤ error;

#اگر_تعریف_شدہ DEBUGBUILD
  {
    ثابت حرف *p;
    اگر(getenv("CURL_DEBUG"))
      multi->admin->سیٹ.verbose = TRUE;
    p = getenv("CURL_DBG_MAX_TOTAL_CONNECTIONS");
    اگر(p) {
      curl_off_t l;
      اگر(!curlx_str_نمبر(&p, &l, UINT32_MAX)) {
        multi->max_کل_connections = (uint32_t)l;
      }
    }
  }
#اگر_ختم
  Curl_uint32_tbl_جوڑو(&multi->xfers, multi->admin, &multi->admin->mid);
  Curl_uint32_bset_جوڑو(&multi->process, multi->admin->mid);

  Curl_cpool_init(&multi->cpool, تہی, chashsize);

#اگر_تعریف_شدہ USE_SSL
  اگر(Curl_ssl_scache_create(sesssize, 2, &multi->ssl_scache))
    جاؤ error;
#ورنہ
  (خالی)sesssize;
#اگر_ختم

#اگر_تعریف_شدہ USE_WINSOCK
  multi->wsa_event = WSACreateEvent();
  اگر(multi->wsa_event == WSA_INVALID_EVENT)
    جاؤ error;
#اگر_ختم
#اگر_تعریف_شدہ ENABLE_WAKEUP
  /* When enabled, rely on this to work. We ignore this in previous
   * versions, but that seems an unnecessary complication. */
  اگر(Curl_wakeup_init(multi->wakeup_pair, TRUE) < 0)
    جاؤ error;
#اگر_ختم
#اگر_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  اگر(Curl_wakeup_init(multi->wakeup_اندرونی, TRUE) < 0)
    جاؤ error;
#اگر_ختم

  اگر(Curl_probeipv6(multi))
    جاؤ error;

#اگر_تعریف_شدہ USE_RESOLV_THREADED
  اگر(xfer_جدول_سائز < CURL_XFER_جدول_سائز) { /* easy multi */
    اگر(Curl_async_thrdd_multi_init(multi, 0, 2, 10))
      جاؤ error;
  }
  ورنہ { /* real multi handle */
    اگر(Curl_async_thrdd_multi_init(multi, 0, 20, 2000))
      جاؤ error;
  }
#اگر_ختم

  واپس multi;

error:

#اگر_تعریف_شدہ USE_RESOLV_THREADED
  Curl_async_thrdd_multi_destroy(multi, TRUE);
#اگر_ختم
  Curl_multi_ev_cleanup(multi);
  Curl_hash_destroy(&multi->proto_hash);
  Curl_dnscache_destroy(&multi->dnscache);
  Curl_cpool_destroy(&multi->cpool, multi->admin);
  Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
#اگر_تعریف_شدہ USE_SSL
  Curl_ssl_scache_destroy(multi->ssl_scache);
#اگر_ختم
  اگر(multi->admin) {
    Curl_multi_ev_xfer_مکمل(multi, multi->admin);
    multi->admin->multi = تہی;
    Curl_close(&multi->admin);
  }
  Curl_mntfy_cleanup(multi);

  Curl_uint32_bset_destroy(&multi->process);
  Curl_uint32_bset_destroy(&multi->dirty);
  Curl_uint32_bset_destroy(&multi->pending);
  Curl_uint32_bset_destroy(&multi->msgsent);
  Curl_uint32_tbl_destroy(&multi->xfers);
#اگر_تعریف_شدہ ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#اگر_ختم
#اگر_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_اندرونی);
#اگر_ختم

  curlx_free(multi);
  واپس تہی;
}

CURLM *curl_multi_init(خالی)
{
  واپس Curl_multi_handle(CURL_XFER_جدول_سائز,
                           CURL_SOCKET_HASH_جدول_سائز,
                           CURL_CONNECTION_HASH_سائز,
                           CURL_DNS_HASH_سائز,
                           CURL_TLS_SESSION_سائز);
}

#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
ساکن خالی multi_warn_debug(ساخت Curl_multi *multi, ساخت Curl_easy *ڈیٹا)
{
  اگر(!multi->warned) {
    infof(ڈیٹا, "!!! WARNING !!!");
    infof(ڈیٹا, "This is a debug build of libcurl, "
                "do not use in production.");
    multi->warned = TRUE;
  }
}
#ورنہ
#تعریف_کرو multi_warn_debug(x, y) Curl_nop_stmt
#اگر_ختم

bool Curl_is_connecting(ساخت Curl_easy *ڈیٹا)
{
  واپس ڈیٹا->mstate < MSTATE_DO;
}

ساکن CURLMcode multi_assess_wakeup(ساخت Curl_multi *multi)
{
#اگر_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  اگر(multi->socket_cb)
    واپس Curl_multi_ev_assess_xfer(multi, multi->admin);
#ورنہ
  (خالی)multi;
#اگر_ختم
  واپس CURLM_OK;
}

ساکن CURLMcode multi_xfers_جوڑو(ساخت Curl_multi *multi,
                                 ساخت Curl_easy *ڈیٹا)
{
  uint32_t استعداد = Curl_uint32_tbl_استعداد(&multi->xfers);
  uint32_t new_سائز = 0;
  /* Prepare to make this into a CURLMOPT_MAX_TRANSFERS, because some
   * applications may want to prevent a run-away of their memory use. */
  /* UINT_MAX is our "invalid" id, do not let the table grow up to that. */
  ثابت uint32_t max_استعداد = UINT_MAX - 1;

  اگر(استعداد < max_استعداد) {
    /* We want `multi->xfers` to have "sufficient" free rows, so that we do
     * not have to reuse the `mid` from a removed easy right away.
     * Check if an 8th of the capacity is still free */
    uint32_t مستعمل = Curl_uint32_tbl_گنتی(&multi->xfers);
    uint32_t غیر_مستعمل = استعداد - مستعمل;
    uint32_t min_غیر_مستعمل = CURLMAX(استعداد >> 3, 4);
    اگر(غیر_مستعمل < min_غیر_مستعمل) {
      /* Grow by 50% of current capacity, in range of [128, 2048],
       * which means the table grows max by 16kb on 64-bit arch. */
      uint32_t growth = CURLMIN(CURLMAX(استعداد >> 1, 128), 2048);
      /* Make sure the uint arithmetic here works on the corner
       * cases where we are close to max_capacity or UINT_MAX */
      اگر((max_استعداد - growth) <= استعداد)
        new_سائز = max_استعداد;
      ورنہ
        new_سائز = استعداد + growth;
    }
  }

  اگر(new_سائز > استعداد) {
    /* Grow the bitsets first. Should one fail, we do not need
     * to downsize the already resized ones. The sets continue
     * to work properly when larger than the table, but not
     * the other way around. */
    CURL_TRC_M(ڈیٹا, "increasing xfer table size to %u", new_سائز);
    اگر(Curl_uint32_bset_resize(&multi->process, new_سائز) ||
       Curl_uint32_bset_resize(&multi->dirty, new_سائز) ||
       Curl_uint32_bset_resize(&multi->pending, new_سائز) ||
       Curl_uint32_bset_resize(&multi->msgsent, new_سائز) ||
       Curl_uint32_tbl_resize(&multi->xfers, new_سائز))
      واپس CURLM_برآمد_کا_حافظہ;
  }

  /* Insert the easy into the table now */
  اگر(!Curl_uint32_tbl_جوڑو(&multi->xfers, ڈیٹا, &ڈیٹا->mid)) {
    /* MUST only happen when table is full */
    DEBUGASSERT(Curl_uint32_tbl_استعداد(&multi->xfers) <=
                Curl_uint32_tbl_گنتی(&multi->xfers));
    واپس CURLM_برآمد_کا_حافظہ;
  }
  واپس CURLM_OK;
}

CURLMcode Curl_multi_جوڑو_handle(ساخت Curl_multi *multi,
                                ساخت Curl_easy *ڈیٹا)
{
  CURLMcode mresult;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  اگر(ڈیٹا->multi)
    واپس CURLM_ADDED_ALREADY;

  اگر(multi->dead) {
    /* a "dead" handle cannot get added transfers while any existing easy
       handles are still alive - but if there are none alive anymore, it is
       fine to start over and unmark the "deadness" of this handle.
       This means only the admin handle MUST be present. */
    اگر((Curl_uint32_tbl_گنتی(&multi->xfers) != 1) ||
       !Curl_uint32_tbl_شامل_ہے(&multi->xfers, 0))
      واپس CURLM_ABORTED_BY_CALLBACK;
    multi->dead = FALSE;
    Curl_uint32_bset_clear(&multi->process);
    Curl_uint32_bset_clear(&multi->dirty);
    Curl_uint32_bset_clear(&multi->pending);
    Curl_uint32_bset_clear(&multi->msgsent);
  }

  اگر(ڈیٹا->multi_easy) {
    /* if this easy handle was previously used for curl_easy_perform(), there
       is a private multi handle here that we can kill */
    curl_multi_cleanup(ڈیٹا->multi_easy);
    ڈیٹا->multi_easy = تہی;
  }

  /* Insert the easy into the multi->xfers table, assigning it a `mid`. */
  اگر(multi_xfers_جوڑو(multi, ڈیٹا))
    واپس CURLM_برآمد_کا_حافظہ;

  /* Initialize timeouts for this handle */
  multi_timeouts_init(ڈیٹا);

  /*
   * No failure allowed in this function beyond this point. No modification
   * of easy nor multi handle allowed before this except for potential
   * multi's connection pool growing which will not be undone in this
   * function no matter what.
   */
  اگر(ڈیٹا->سیٹ.errorbuffer)
    ڈیٹا->سیٹ.errorbuffer[0] = 0;

  ڈیٹا->state.os_errno = 0;

  /* make the Curl_easy refer back to this multi handle - before
     Curl_expire() is called. */
  ڈیٹا->multi = multi;

  /* set the easy handle */
  multistate(ڈیٹا, MSTATE_INIT);
  /* not yet passed INIT state */
  ڈیٹا->state.really_alive = FALSE;

#اگر_تعریف_شدہ USE_LIBPSL
  /* Do the same for PSL. */
  اگر(ڈیٹا->share && (ڈیٹا->share->specifier & (1 << CURL_تالا_ڈیٹا_PSL)))
    ڈیٹا->psl = &ڈیٹا->share->psl;
  ورنہ
    ڈیٹا->psl = &multi->psl;
#اگر_ختم

  /* add the easy handle to the process set */
  Curl_uint32_bset_جوڑو(&multi->process, ڈیٹا->mid);
  ++multi->xfers_alive;
  ++multi->xfers_کل_ever;

  Curl_cpool_xfer_init(ڈیٹا);
  multi_warn_debug(multi, ڈیٹا);

  /* Make sure the new handle will run */
  Curl_multi_mark_dirty(ڈیٹا);

  /* Necessary in event based processing, where dirty handles trigger
   * a timeout callback invocation. */
  mresult = Curl_update_timer(multi);
  اگر(mresult) {
    /* undo the additions above */
    Curl_uint32_bset_remove(&multi->process, ڈیٹا->mid);
    Curl_uint32_bset_remove(&multi->dirty, ڈیٹا->mid);
    --multi->xfers_alive;
    ڈیٹا->multi = تہی; /* not anymore */
    Curl_uint32_tbl_remove(&multi->xfers, ڈیٹا->mid);
    ڈیٹا->mid = UINT32_MAX;
    واپس mresult;
  }

  /* The admin handle only ever has default timeouts set. To improve the
     state somewhat we clone the timeouts from each added handle so that the
     admin handle always has the same timeouts as the most recently added
     easy handle. */
  multi->admin->سیٹ.timeout = ڈیٹا->سیٹ.timeout;
  multi->admin->سیٹ.server_response_timeout =
    ڈیٹا->سیٹ.server_response_timeout;
  multi->admin->سیٹ.no_signal = ڈیٹا->سیٹ.no_signal;

  CURL_TRC_M(ڈیٹا, "added to multi, mid=%u, running=%u, total=%u",
             ڈیٹا->mid, multi_xfers_running(multi),
             Curl_uint32_tbl_گنتی(&multi->xfers));
  واپس CURLM_OK;
}

CURLMcode curl_multi_جوڑو_handle(CURLM *m, CURL *curl)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_جوڑو_handle, &mresult)) {
    ساخت Curl_easy *ڈیٹا = curl;
    /* Verify that we got a somewhat good easy handle too */
    اگر(!GOOD_EASY_HANDLE(ڈیٹا) || ڈیٹا->state.اندرونی_2)
      mresult = CURLM_BAD_EASY_HANDLE;
    ورنہ
      mresult = Curl_multi_جوڑو_handle(m, ڈیٹا);
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

ساخت multi_مکمل_ctx {
  ثنائی_ہندسہ(premature);
};

ساکن bool multi_conn_should_close(ساخت connectdata *conn,
                                    ساخت Curl_easy *ڈیٹا,
                                    bool premature)
{
  /* if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of everything else. */
  اگر(conn->بٹ.close)
    واپس TRUE;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM/NEGOTIATE authentication handshake. */
  اگر(ڈیٹا->سیٹ.reuse_forbid
#اگر_تعریف_شدہ USE_NTLM
     && !(conn->http_ntlm_state == NTLMSTATE_TYPE2 ||
          conn->proxy_ntlm_state == NTLMSTATE_TYPE2)
#اگر_ختم
#اگر_تعریف_شدہ USE_SPNEGO
     && !(conn->http_negotiate_state == CURL_NEGO_AUTH_RECV ||
          conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)
#اگر_ختم
    )
    واپس TRUE;

  /* Unless this connection is for a "connect-only" transfer, it
   * needs to be closed if the protocol handler does not support reuse. */
  اگر(!ڈیٹا->سیٹ.connect_only && conn->scheme &&
     !(conn->scheme->flags & PROTOPT_CONN_REUSE))
    واپس TRUE;

  /* if premature is TRUE, it means this connection was said to be DONE before
     the entire request operation is complete and thus we cannot know in what
     state it is for reusing, so we are forced to close it. In a perfect world
     we can add code that keep track of if we really must close it here or not,
     but currently we have no such detail knowledge. */
  اگر(premature && !Curl_conn_is_multiplex(conn, FIRSTSOCKET))
    واپس TRUE;

  واپس FALSE;
}

ساکن cpool_do_نتیجہ multi_مکمل_locked(ساخت Curl_easy *ڈیٹا,
                                         ساخت connectdata *conn,
                                         خالی *userdata,
                                         ثابت ساخت curltime *pnow)
{
  ساخت multi_مکمل_ctx *mdctx = userdata;

  (خالی)pnow;
  CURL_TRC_M(ڈیٹا, "multi_done_locked, in use=%u", conn->attached_xfers);
  اگر(CONN_INUSE(conn)) {
    /* Stop if still used. */
    CURL_TRC_M(ڈیٹا, "Connection still in use %u, no more multi_done now!",
               conn->attached_xfers);
    واپس CPOOL_DO_KEEP;
  }

  اگر(multi_conn_should_close(conn, ڈیٹا, (bool)mdctx->premature)) {
    CURL_TRC_M(ڈیٹا, "multi_done, terminating conn #%" FMT_OFF_T " to %s:%u, "
               "forbid=%d, close=%d, premature=%d, conn_multiplex=%d",
               conn->connection_id, conn->origin->user_hostname,
               conn->origin->پورٹ,
               ڈیٹا->سیٹ.reuse_forbid, conn->بٹ.close, mdctx->premature,
               Curl_conn_is_multiplex(conn, FIRSTSOCKET));
    connclose(conn);
    واپس mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  ورنہ اگر(!Curl_conn_get_max_concurrent(ڈیٹا, conn, FIRSTSOCKET)) {
    CURL_TRC_M(ڈیٹا, "multi_done, conn #%" FMT_OFF_T " to %s:%u was shutdown"
               " by server, not reusing", conn->connection_id,
               conn->origin->user_hostname, conn->origin->پورٹ);
    connclose(conn);
    واپس mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  ورنہ {
    واپس CPOOL_DO_IDLE;
  }
}

ساکن CURLcode multi_مکمل(ساخت Curl_easy *ڈیٹا,
                           CURLcode حالت_2,  /* an error if this is called
                                                after an error was detected */
                           bool premature)
{
  CURLcode نتیجہ;
  ساخت connectdata *conn = ڈیٹا->conn;
  ثابت ساخت curltime *pnow = Curl_pgrs_now(ڈیٹا);

  CURL_TRC_M(ڈیٹا, "multi_done: status: %d prem: %d done: %d",
             (صحیح)حالت_2, (صحیح)premature, ڈیٹا->state.مکمل);

  اگر(ڈیٹا->state.مکمل) {
    /* Stop if multi_done() has already been called */
    واپس CURLE_OK;
  }
  ڈیٹا->state.مکمل = TRUE; /* called now! */

  /* Shut down any ongoing async resolver operation. */
  Curl_resolv_shutdown_all(ڈیٹا);

  /* Cleanup possible redirect junk */
  curlx_safefree(ڈیٹا->req.newurl);
  curlx_safefree(ڈیٹا->req.location);

  انتخاب(حالت_2) {
  حالت CURLE_ABORTED_BY_CALLBACK:
  حالت CURLE_READ_ERROR:
  حالت CURLE_WRITE_ERROR:
    /* When we are aborted due to a callback return code it has to be counted
       as premature as there is trouble ahead if we do not. We have many
       callbacks and protocols work differently, we could potentially do this
       more fine-grained in the future. */
    premature = TRUE;
    FALLTHROUGH();
  طےشدہ:
    توڑو;
  }

  /* this calls the protocol-specific function pointer previously set */
  اگر(conn && conn->scheme->تسلسل->مکمل && (ڈیٹا->mstate >= MSTATE_PROTOCONNECT))
    نتیجہ = conn->scheme->تسلسل->مکمل(ڈیٹا, حالت_2, premature);
  ورنہ
    نتیجہ = حالت_2;

  اگر(ڈیٹا->mstate > MSTATE_CONNECTING &&
     (نتیجہ != CURLE_ABORTED_BY_CALLBACK)) {
    /* avoid this if
     * - the transfer has not connected
     * - we already aborted by callback to avoid this calling another callback
     */
    صحیح rc = Curl_pgrs_مکمل(ڈیٹا, pnow);
    اگر(!نتیجہ && rc)
      نتیجہ = CURLE_ABORTED_BY_CALLBACK;
  }

  /* Make sure that transfer client writes are really done now. */
  نتیجہ = Curl_1st_fatal(نتیجہ, Curl_xfer_write_مکمل(ڈیٹا, premature));

  اگر(!نتیجہ)
    نتیجہ = Curl_req_مکمل(&ڈیٹا->req, ڈیٹا, premature);

  اگر(conn) {
    /* Detach connection and decide what to do with it. */
    ساخت multi_مکمل_ctx mdctx;

    /* Inform connection filters that this transfer is done and they
     * need to give up any references to `data`. */
    Curl_conn_ev_ڈیٹا_مکمل(ڈیٹا, premature);
    Curl_detach_connection(ڈیٹا);
    Curl_dnscache_prune(ڈیٹا, pnow);

    memset(&mdctx, 0, حجم(mdctx));
    mdctx.premature = premature;
    Curl_cpool_return(ڈیٹا, conn, multi_مکمل_locked, &mdctx, pnow);

    اگر(ڈیٹا->multi)
      multi_جدول_وقت_pending(ڈیٹا->multi);
  }

  /* flush the netrc cache */
  Curl_netrc_cleanup(&ڈیٹا->state.netrc);
  واپس نتیجہ;
}

CURLMcode Curl_multi_remove_handle(ساخت Curl_multi *multi,
                                   ساخت Curl_easy *ڈیٹا)
{
  CURLMcode mresult;
  bool premature;
  uint32_t mid;

  /* Prevent users from trying to remove same easy handle more than once */
  اگر(!ڈیٹا->multi)
    واپس CURLM_OK; /* it is already removed so let's say it is fine! */

  /* Prevent users from trying to remove an easy handle from the wrong multi */
  اگر(ڈیٹا->multi != multi)
    واپس CURLM_BAD_EASY_HANDLE;

  اگر(ڈیٹا->mid == UINT32_MAX) {
    DEBUGASSERT(0);
    واپس CURLM_اندرونی_ERROR;
  }
  اگر(Curl_uint32_tbl_get(&multi->xfers, ڈیٹا->mid) != ڈیٹا) {
    DEBUGASSERT(0);
    واپس CURLM_اندرونی_ERROR;
  }

  premature = (ڈیٹا->mstate < MSTATE_COMPLETED);

  اگر(ڈیٹا->conn) {
    /* If the 'state' is not INIT or COMPLETED, we might need to do something
       nice to put the easy_handle in a good known state when this returns. */
    اگر(premature && (ڈیٹا->mstate > MSTATE_DO))
      streamclose(ڈیٹا->conn);

    /* multi_done() clears the association between the easy handle and the
       connection.
       Note that this ignores the return code because there is
       nothing really useful to do with it anyway! */
    (خالی)multi_مکمل(ڈیٹا, ڈیٹا->نتیجہ, premature);
  }

  /* The timer must be shut down before data->multi is set to NULL, else
     data's splaynode would remain in the splay tree after curl_easy_cleanup is
     called. Do it after multi_done() in case that sets another time! */
  Curl_expire_clear_all(ڈیٹا);

  /* In MSGSENT, it was deducted from `multi->xfers_alive` already. */
  اگر(ڈیٹا->mstate != MSTATE_MSGSENT)
    --multi->xfers_alive;

  اگر(ڈیٹا->state.really_alive) {
    ڈیٹا->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    اگر(!multi->xfers_really_alive)
      (خالی)multi_assess_wakeup(multi);
  }

  Curl_wildcard_dtor(&ڈیٹا->wildcard);

  ڈیٹا->mstate = MSTATE_COMPLETED;

  /* Remove the association between the connection and the handle */
  Curl_detach_connection(ڈیٹا);

  /* Tell event handling that this transfer is definitely going away */
  Curl_multi_ev_xfer_مکمل(multi, ڈیٹا);

  اگر(ڈیٹا->سیٹ.connect_only) {
    اگر(ڈیٹا->multi_easy) {
      اگر(ڈیٹا->state.آخری_conn_id != -1) {
        /* Mark any connect-only connection for closure */
        ساخت connectdata *conn;
        (خالی)Curl_getconnectinfo(ڈیٹا, &conn);
        اگر(conn && conn->بٹ.connect_only)
          connclose(conn);
      }
    }
    ورنہ {
      /* This removes a handle that was part the multi interface that used
         CONNECT_ONLY, that connection is now left alive but since this handle
         has bits.close set nothing can use that connection anymore and it is
         forbidden from reuse. This easy handle cannot find the connection
         anymore once removed from the multi handle

         Better close the connection here, at once. */
      ساخت connectdata *conn;
      (خالی)Curl_getconnectinfo(ڈیٹا, &conn);
      اگر(conn)
        Curl_conn_close(ڈیٹا, conn, TRUE);
    }
  }

#اگر_تعریف_شدہ USE_LIBPSL
  /* Remove the PSL association. */
  اگر(ڈیٹا->psl == &multi->psl)
    ڈیٹا->psl = تہی;
#اگر_ختم

  /* clear the association to this multi handle */
  mid = ڈیٹا->mid;
  DEBUGASSERT(Curl_uint32_tbl_شامل_ہے(&multi->xfers, mid));
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  Curl_uint32_bset_remove(&multi->process, mid);
  Curl_uint32_bset_remove(&multi->dirty, mid);
  Curl_uint32_bset_remove(&multi->pending, mid);
  Curl_uint32_bset_remove(&multi->msgsent, mid);
  ڈیٹا->multi = تہی;
  ڈیٹا->mid = UINT32_MAX;
  ڈیٹا->master_mid = UINT32_MAX;

  /* A pending transfer *might* be able to run now. */
  multi_جدول_وقت_pending(multi);
  mresult = Curl_update_timer(multi);
  اگر(mresult)
    واپس mresult;

  mresult = multi_assess_wakeup(multi);
  اگر(mresult) {
    failf(ڈیٹا, "error enabling wakeup listening: %d", mresult);
    واپس mresult;
  }

  CURL_TRC_M(ڈیٹا, "removed from multi, mid=%u, running=%u, total=%u",
             mid, multi_xfers_running(multi),
             Curl_uint32_tbl_گنتی(&multi->xfers));
  واپس CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *curl)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_remove_handle, &mresult)) {
    ساخت Curl_easy *ڈیٹا = curl;
    اگر(!GOOD_EASY_HANDLE(ڈیٹا) || ڈیٹا->state.اندرونی_2)
      mresult = CURLM_BAD_EASY_HANDLE;
    ورنہ
      mresult = Curl_multi_remove_handle(m, ڈیٹا);
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

/* Return TRUE if the application asked for multiplexing */
bool Curl_multiplex_wanted(ثابت ساخت Curl_multi *multi)
{
  واپس multi && multi->multiplexing;
}

/*
 * Curl_detach_connection() removes the given transfer from the connection.
 *
 * This is the only function that should clear data->conn. This will
 * occasionally be called with the data->conn pointer already cleared.
 */
خالی Curl_detach_connection(ساخت Curl_easy *ڈیٹا)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  اگر(conn) {
    /* this should never happen, prevent underflow */
    DEBUGASSERT(conn->attached_xfers);
    اگر(conn->attached_xfers) {
      conn->attached_xfers--;
      اگر(!conn->attached_xfers)
        conn->attached_multi = تہی;
    }
  }
  ڈیٹا->conn = تہی;
}

/*
 * Curl_attach_connection() attaches this transfer to this connection.
 *
 * This is the only function that should assign data->conn.
 * `matched == TRUE` means the transfer's properties match this
 * connection and it is not a temporary attach for maintenance.
 */
خالی Curl_attach_connection(ساخت Curl_easy *ڈیٹا,
                            ساخت connectdata *conn,
                            bool matched)
{
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(!ڈیٹا->conn);
  DEBUGASSERT(conn);
  DEBUGASSERT(conn->attached_xfers < UINT32_MAX);
  ڈیٹا->conn = conn;
  اگر(matched) {
    ڈیٹا->state.آخری_conn_id = conn->connection_id;
    ڈیٹا->state.آخری_cpid = conn->cpid;
  }
  ورنہ
    DEBUGASSERT(!ڈیٹا->mid); /* admin handle */
  conn->attached_xfers++;
  /* all attached transfers must be from the same multi */
  اگر(!conn->attached_multi)
    conn->attached_multi = ڈیٹا->multi;
  DEBUGASSERT(conn->attached_multi == ڈیٹا->multi);

  اگر(conn->scheme && conn->scheme->تسلسل->attach)
    conn->scheme->تسلسل->attach(ڈیٹا, conn);
}

/* adjust pollset for rate limits/pauses */
ساکن CURLcode multi_adjust_pollset(ساخت Curl_easy *ڈیٹا,
                                     ساخت easy_pollset *ps)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(ps->n) {
    bool send_blocked, recv_blocked;

    recv_blocked = (Curl_rlimit_avail(&ڈیٹا->پیش_رفت.dl.rlimit, تہی) <= 0);
    send_blocked = (Curl_rlimit_avail(&ڈیٹا->پیش_رفت.ul.rlimit, تہی) <= 0);
    اگر(send_blocked || recv_blocked) {
      صحیح i;
      برائے(i = 0; i <= SECONDARYSOCKET; ++i) {
        curl_socket_t sock = ڈیٹا->conn->sock[i];
        اگر(sock == CURL_SOCKET_BAD)
          جاری;
        اگر(recv_blocked && Curl_pollset_want_recv(ڈیٹا, ps, sock)) {
          نتیجہ = Curl_pollset_remove_اندر(ڈیٹا, ps, sock);
          اگر(نتیجہ)
            توڑو;
        }
        اگر(send_blocked && Curl_pollset_want_send(ڈیٹا, ps, sock)) {
          نتیجہ = Curl_pollset_remove_برآمد(ڈیٹا, ps, sock);
          اگر(نتیجہ)
            توڑو;
        }
      }
    }

    /* Not blocked and wanting to receive. If there is data pending
     * in the connection filters, make transfer run again. */
    اگر(!recv_blocked &&
       ((Curl_pollset_want_recv(ڈیٹا, ps, ڈیٹا->conn->sock[FIRSTSOCKET]) &&
         Curl_conn_ڈیٹا_pending(ڈیٹا, FIRSTSOCKET)) ||
        (Curl_pollset_want_recv(ڈیٹا, ps, ڈیٹا->conn->sock[SECONDARYSOCKET]) &&
         Curl_conn_ڈیٹا_pending(ڈیٹا, SECONDARYSOCKET)))) {
      CURL_TRC_M(ڈیٹا, "pollset[] has POLLIN, but there is still "
                 "buffered input -> mark as dirty");
      Curl_multi_mark_dirty(ڈیٹا);
    }
  }
  واپس نتیجہ;
}

ساکن CURLcode mstate_connecting_pollset(ساخت Curl_easy *ڈیٹا,
                                          ساخت easy_pollset *ps)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  curl_socket_t sockfd;
  CURLcode نتیجہ = CURLE_OK;

  اگر(Curl_xfer_recv_is_paused(ڈیٹا))
    واپس CURLE_OK;
  /* If a socket is set, receiving is default. If the socket
   * has not been determined yet (eyeballing), always ask the
   * connection filters for what to monitor. */
  sockfd = Curl_conn_get_پہلا_socket(ڈیٹا);
  اگر(sockfd != CURL_SOCKET_BAD) {
    نتیجہ = Curl_pollset_change(ڈیٹا, ps, sockfd, CURL_POLL_اندر, 0);
    اگر(!نتیجہ)
      نتیجہ = multi_adjust_pollset(ڈیٹا, ps);
  }
  اگر(!نتیجہ)
    نتیجہ = Curl_conn_adjust_pollset(ڈیٹا, conn, ps);
  واپس نتیجہ;
}

ساکن CURLcode mstate_protocol_pollset(ساخت Curl_easy *ڈیٹا,
                                        ساخت easy_pollset *ps)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ = CURLE_OK;

  اگر(conn->scheme->تسلسل->proto_pollset)
    نتیجہ = conn->scheme->تسلسل->proto_pollset(ڈیٹا, ps);
  ورنہ {
    curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
    اگر(sockfd != CURL_SOCKET_BAD) {
      /* Default is to wait to something from the server */
      نتیجہ = Curl_pollset_change(ڈیٹا, ps, sockfd, CURL_POLL_اندر, 0);
    }
  }
  اگر(!نتیجہ)
    نتیجہ = multi_adjust_pollset(ڈیٹا, ps);
  اگر(!نتیجہ)
    نتیجہ = Curl_conn_adjust_pollset(ڈیٹا, conn, ps);
  واپس نتیجہ;
}

ساکن CURLcode mstate_do_pollset(ساخت Curl_easy *ڈیٹا,
                                  ساخت easy_pollset *ps)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ = CURLE_OK;

  اگر(conn->scheme->تسلسل->doing_pollset)
    نتیجہ = conn->scheme->تسلسل->doing_pollset(ڈیٹا, ps);
  ورنہ اگر(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    نتیجہ = Curl_pollset_جوڑو_برآمد(ڈیٹا, ps, conn->sock[conn->send_idx]);
  }
  اگر(!نتیجہ)
    نتیجہ = multi_adjust_pollset(ڈیٹا, ps);
  اگر(!نتیجہ)
    نتیجہ = Curl_conn_adjust_pollset(ڈیٹا, conn, ps);
  واپس نتیجہ;
}

ساکن CURLcode mstate_domore_pollset(ساخت Curl_easy *ڈیٹا,
                                      ساخت easy_pollset *ps)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ = CURLE_OK;

  اگر(conn->scheme->تسلسل->domore_pollset)
    نتیجہ = conn->scheme->تسلسل->domore_pollset(ڈیٹا, ps);
  ورنہ اگر(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    نتیجہ = Curl_pollset_جوڑو_برآمد(ڈیٹا, ps, conn->sock[conn->send_idx]);
  }
  اگر(!نتیجہ)
    نتیجہ = multi_adjust_pollset(ڈیٹا, ps);
  اگر(!نتیجہ)
    نتیجہ = Curl_conn_adjust_pollset(ڈیٹا, conn, ps);
  واپس نتیجہ;
}

ساکن CURLcode mstate_perform_pollset(ساخت Curl_easy *ڈیٹا,
                                       ساخت easy_pollset *ps)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ = CURLE_OK;

  اگر(conn->scheme->تسلسل->perform_pollset)
    نتیجہ = conn->scheme->تسلسل->perform_pollset(ڈیٹا, ps);
  ورنہ {
    /* Default is to obey the request flags for send/recv */
    اگر(Curl_req_want_recv(ڈیٹا) && CONN_SOCK_IDX_VALID(conn->recv_idx)) {
      نتیجہ = Curl_pollset_جوڑو_اندر(ڈیٹا, ps, conn->sock[conn->recv_idx]);
    }
    اگر(!نتیجہ && Curl_req_want_send(ڈیٹا) &&
       CONN_SOCK_IDX_VALID(conn->send_idx)) {
      نتیجہ = Curl_pollset_جوڑو_برآمد(ڈیٹا, ps, conn->sock[conn->send_idx]);
    }
  }
  اگر(!نتیجہ)
    نتیجہ = multi_adjust_pollset(ڈیٹا, ps);
  اگر(!نتیجہ)
    نتیجہ = Curl_conn_adjust_pollset(ڈیٹا, conn, ps);
  واپس نتیجہ;
}

#اگر_تعریف_شدہ CURLVERBOSE
ساکن size_t multi_timeouts_گنتی(ساخت expire_timers *timeouts)
{
  size_t n = 0;
  uint8_t eid = timeouts->پہلا;
  برائے(; eid < EXPIRE_آخری; eid = timeouts->اگلا[eid])
    ++n;
  واپس n;
}
#اگر_ختم

/* Initializes `poll_set` with the current socket poll actions needed
 * for transfer `data`. */
CURLMcode Curl_multi_pollset(ساخت Curl_easy *ڈیٹا,
                             ساخت easy_pollset *ps)
{
  CURLcode نتیجہ = CURLE_OK;

  Curl_pollset_reset(ps);
#اگر_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  /* The admin handle always listens on the wakeup socket when there
   * are transfers alive. */
  اگر(ڈیٹا->multi && (ڈیٹا == ڈیٹا->multi->admin) &&
     ڈیٹا->multi->xfers_really_alive) {
    CURL_TRC_M(ڈیٹا, "adding wakeup, %u xfers really alive",
               ڈیٹا->multi->xfers_really_alive);
    نتیجہ = Curl_pollset_جوڑو_اندر(ڈیٹا, ps, ڈیٹا->multi->wakeup_اندرونی[0]);
  }
#اگر_ختم
  /* If the transfer has no connection, this is fine. Happens when
     called via curl_multi_remove_handle() => Curl_multi_ev_assess() =>
     Curl_multi_pollset(). */
  اگر(!نتیجہ && ڈیٹا->conn) {
    انتخاب(ڈیٹا->mstate) {
    حالت MSTATE_INIT:
    حالت MSTATE_PENDING:
    حالت MSTATE_تیاری:
    حالت MSTATE_CONNECT:
      /* nothing to poll for yet */
      توڑو;

    حالت MSTATE_CONNECTING:
      نتیجہ = mstate_connecting_pollset(ڈیٹا, ps);
      توڑو;

    حالت MSTATE_PROTOCONNECT:
    حالت MSTATE_PROTOCONNECTING:
      نتیجہ = mstate_protocol_pollset(ڈیٹا, ps);
      توڑو;

    حالت MSTATE_DO:
    حالت MSTATE_DOING:
      نتیجہ = mstate_do_pollset(ڈیٹا, ps);
      توڑو;

    حالت MSTATE_DOING_مزید:
      نتیجہ = mstate_domore_pollset(ڈیٹا, ps);
      توڑو;

    حالت MSTATE_DID: /* same as PERFORMING in regard to polling */
    حالت MSTATE_PERFORMING:
      نتیجہ = mstate_perform_pollset(ڈیٹا, ps);
      توڑو;

    حالت MSTATE_RATELIMITING:
      /* we need to let time pass, ignore socket(s) */
      توڑو;

    حالت MSTATE_مکمل:
    حالت MSTATE_COMPLETED:
    حالت MSTATE_MSGSENT:
      /* nothing more to poll for */
      توڑو;

    طےشدہ:
      failf(ڈیٹا, "multi_getsock: unexpected multi state %d",
            (صحیح)ڈیٹا->mstate);
      DEBUGASSERT(0);
      توڑو;
    }
  }

  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_برآمد_کا_حافظہ)
      واپس CURLM_برآمد_کا_حافظہ;
    failf(ڈیٹا, "error determining pollset: %d", (صحیح)نتیجہ);
    واپس CURLM_اندرونی_ERROR;
  }

#اگر_تعریف_شدہ CURLVERBOSE
  اگر(CURL_TRC_M_is_verbose(ڈیٹا)) {
    size_t timeout_گنتی = multi_timeouts_گنتی(&ڈیٹا->state.timeouts);
    انتخاب(ps->n) {
    حالت 0:
      CURL_TRC_M(ڈیٹا, "pollset[], timeouts=%zu, paused %d/%d (r/w)",
                 timeout_گنتی,
                 Curl_xfer_send_is_paused(ڈیٹا),
                 Curl_xfer_recv_is_paused(ڈیٹا));
      توڑو;
    حالت 1:
      CURL_TRC_M(ڈیٹا, "pollset[fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_اندر) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_برآمد) ? "OUT" : "",
                 timeout_گنتی);
      توڑو;
    حالت 2:
      CURL_TRC_M(ڈیٹا, "pollset[fd=%" FMT_SOCKET_T " %s%s, "
                 "fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_اندر) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_برآمد) ? "OUT" : "",
                 ps->sockets[1],
                 (ps->actions[1] & CURL_POLL_اندر) ? "IN" : "",
                 (ps->actions[1] & CURL_POLL_برآمد) ? "OUT" : "",
                 timeout_گنتی);
      توڑو;
    طےشدہ:
      CURL_TRC_M(ڈیٹا, "pollset[fds=%u], timeouts=%zu", ps->n, timeout_گنتی);
      توڑو;
    }
    CURL_TRC_EASY_TIMERS(ڈیٹا);
  }
#اگر_ختم

  واپس CURLM_OK;
}

CURLMcode curl_multi_fdset(CURLM *m,
                           fd_set *read_fd_سیٹ, fd_set *write_fd_سیٹ,
                           fd_set *exc_fd_سیٹ, صحیح *max_fd)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_fdset, &mresult)) {
    /* Scan through all the easy handles to get the file descriptors set.
       Some easy handles may not have connected to the remote host yet,
       and then we must make sure that is done. */
    ساخت Curl_multi *multi = m;
    ساخت easy_pollset ps;
    صحیح this_max_fd = -1;
    غیرعلامتی صحیح i;
    uint32_t mid;
    (خالی)exc_fd_سیٹ;

    Curl_pollset_init(&ps);
    اگر(Curl_uint32_bset_پہلا(&multi->process, &mid)) {
      کرو {
        ساخت Curl_easy *ڈیٹا = Curl_multi_get_easy(multi, mid);

        اگر(!ڈیٹا) {
          DEBUGASSERT(0);
          جاری;
        }

        Curl_multi_pollset(ڈیٹا, &ps);
        برائے(i = 0; i < ps.n; i++) {
          اگر(!FDSET_SOCK(ps.sockets[i]))
            /* pretend it does not exist */
            جاری;
          اگر(ps.actions[i] & CURL_POLL_اندر)
            FD_SET(ps.sockets[i], read_fd_سیٹ);
          اگر(ps.actions[i] & CURL_POLL_برآمد)
            FD_SET(ps.sockets[i], write_fd_سیٹ);
          اگر((صحیح)ps.sockets[i] > this_max_fd)
            this_max_fd = (صحیح)ps.sockets[i];
        }
      } جب(Curl_uint32_bset_اگلا(&multi->process, mid, &mid));
    }

    Curl_cshutdn_setfds(&multi->cshutdn, multi->admin,
                        read_fd_سیٹ, write_fd_سیٹ, &this_max_fd);

    *max_fd = this_max_fd;
    Curl_pollset_cleanup(&ps);

    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

CURLMcode curl_multi_waitfds(CURLM *m,
                             ساخت curl_waitfd *ufds,
                             غیرعلامتی صحیح سائز,
                             غیرعلامتی صحیح *fd_گنتی)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_waitfds, &mresult)) {
    ساخت Curl_waitfds cwfds;
    ساخت Curl_multi *multi = m;
    ساخت easy_pollset ps;
    غیرعلامتی صحیح need = 0;
    uint32_t mid;

    اگر(!ufds && (سائز || !fd_گنتی)) {
      mresult = CURLM_BAD_FUNCTION_دلیل;
      جاؤ برآمد;
    }

    Curl_pollset_init(&ps);
    Curl_waitfds_init(&cwfds, ufds, سائز);
    mresult = CURLM_OK;
    اگر(Curl_uint32_bset_پہلا(&multi->process, &mid)) {
      کرو {
        ساخت Curl_easy *ڈیٹا = Curl_multi_get_easy(multi, mid);
        اگر(!ڈیٹا) {
          DEBUGASSERT(0);
          Curl_uint32_bset_remove(&multi->process, mid);
          Curl_uint32_bset_remove(&multi->dirty, mid);
          جاری;
        }
        Curl_multi_pollset(ڈیٹا, &ps);
        need += Curl_waitfds_جوڑو_ps(&cwfds, &ps);
      } جب(Curl_uint32_bset_اگلا(&multi->process, mid, &mid));
    }

    need += Curl_cshutdn_جوڑو_waitfds(&multi->cshutdn, multi->admin, &cwfds);

    اگر(need != cwfds.n && ufds)
      mresult = CURLM_برآمد_کا_حافظہ;

    اگر(fd_گنتی)
      *fd_گنتی = need;
    Curl_pollset_cleanup(&ps);
  }
برآمد:
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

#اگر_تعریف_شدہ USE_WINSOCK
/* Reset FD_WRITE for TCP sockets. Nothing is actually sent. UDP sockets cannot
 * be reset this way because an empty datagram would be sent. #9203
 *
 * "On Windows the internal state of FD_WRITE as returned from
 * WSAEnumNetworkEvents is only reset after successful send()."
 */
ساکن خالی reset_socket_fdwrite(curl_socket_t s)
{
  صحیح t;
  صحیح l = (صحیح)حجم(t);
  اگر(!getsockopt(s, SOL_SOCKET, SO_TYPE, (حرف *)&t, &l) && t == SOCK_STREAM)
    swrite(s, تہی, 0);
}

ساکن CURLMcode multi_winsock_select(ساخت Curl_multi *multi,
                                      ساخت curl_pollfds *cpfds,
                                      غیرعلامتی صحیح curl_nfds,
                                      ساخت curl_waitfd extra_fds[],
                                      غیرعلامتی صحیح extra_nfds,
                                      صحیح timeout_ms,
                                      bool extrawait,
                                      صحیح *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  WSANETWORKEVENTS wsa_events;
  صحیح nevents = 0;
  size_t i;

  DEBUGASSERT(multi->wsa_event != WSA_INVALID_EVENT);

  /* Set the WSA events based on the collected pollds */
  برائے(i = 0; i < cpfds->n; i++) {
    طویل نقاب = 0;
    اگر(cpfds->pfds[i].events & POLLIN)
      نقاب |= FD_READ | FD_ACCEPT | FD_CLOSE;
    اگر(cpfds->pfds[i].events & POLLPRI)
      نقاب |= FD_OOB;
    اگر(cpfds->pfds[i].events & POLLOUT) {
      نقاب |= FD_WRITE | FD_CONNECT | FD_CLOSE;
      reset_socket_fdwrite(cpfds->pfds[i].fd);
    }
    اگر(نقاب && WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, نقاب)) {
      mresult = CURLM_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }

  اگر(cpfds->n || extrawait) {
    صحیح pollrc = 0;

    اگر(cpfds->n) {         /* pre-check with Winsock */
      pollrc = Curl_poll(cpfds->pfds, cpfds->n, 0);
      اگر(pollrc < 0) {
        mresult = CURLM_UNRECOVERABLE_POLL;
        جاؤ برآمد;
      }
      nevents = pollrc;
    }

    اگر(!nevents) {
      /* now wait... if not ready during the pre-check (pollrc == 0) */
      WSAWait_For_مضرب_Events(1, &multi->wsa_event, FALSE, (DWORD)timeout_ms,
                               FALSE);
    }

    /* With Winsock, we have to run the following section unconditionally
       to call WSAEventSelect(fd, event, 0) on all the sockets */
    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    برائے(i = 0; i < extra_nfds; i++) {
      غیرعلامتی مختصر نقاب = 0;
      curl_socket_t s = extra_fds[i].fd;

      wsa_events.l_جال_Events = 0;
      اگر(WSAEnum_جال_Events(s, تہی, &wsa_events) == 0) {
        اگر(wsa_events.l_جال_Events & (FD_READ | FD_ACCEPT | FD_CLOSE))
          نقاب |= CURL_WAIT_POLLIN;
        اگر(wsa_events.l_جال_Events & (FD_WRITE | FD_CONNECT | FD_CLOSE))
          نقاب |= CURL_WAIT_POLLOUT;
        اگر(wsa_events.l_جال_Events & FD_OOB)
          نقاب |= CURL_WAIT_POLLPRI;
        اگر(!pollrc && wsa_events.l_جال_Events)
          nevents++;
      }
      WSAEventSelect(s, multi->wsa_event, 0);
      اگر(!pollrc) {
        extra_fds[i].revents = (مختصر)نقاب;
        جاری;
      }
      ورنہ {
        غیرعلامتی r = (غیرعلامتی)cpfds->pfds[curl_nfds + i].revents;
        اگر(r & POLLIN)
          نقاب |= CURL_WAIT_POLLIN;
        اگر(r & POLLOUT)
          نقاب |= CURL_WAIT_POLLOUT;
        اگر(r & POLLPRI)
          نقاب |= CURL_WAIT_POLLPRI;
        extra_fds[i].revents = (مختصر)نقاب;
      }
    }

    /* Count up all our own sockets that had activity,
       and remove them from the event. */
    برائے(i = 0; i < curl_nfds; ++i) {
      wsa_events.l_جال_Events = 0;
      اگر(WSAEnum_جال_Events(cpfds->pfds[i].fd, تہی, &wsa_events) == 0) {
        اگر(!pollrc && wsa_events.l_جال_Events)
          nevents++;
      }
      WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, 0);
    }
    WSAResetEvent(multi->wsa_event);
  }

برآمد:
  *pnevents = nevents;
  واپس mresult;
}

#ورنہ /* USE_WINSOCK */

ساکن CURLMcode multi_posix_poll(ساخت Curl_multi *multi,
                                  ساخت curl_pollfds *cpfds,
                                  غیرعلامتی صحیح curl_nfds,
                                  ساخت curl_waitfd extra_fds[],
                                  غیرعلامتی صحیح extra_nfds,
                                  صحیح timeout_ms,
                                  bool extrawait,
                                  صحیح *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  صحیح nevents = 0;
  size_t i;

  (خالی)multi;
  اگر(cpfds->n) {
    صحیح pollrc = Curl_poll(cpfds->pfds, cpfds->n, timeout_ms); /* wait... */
    اگر(pollrc < 0) {
      mresult = CURLM_UNRECOVERABLE_POLL;
      جاؤ برآمد;
    }
    nevents = pollrc;

    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    برائے(i = 0; i < extra_nfds; i++) {
      غیرعلامتی r = (غیرعلامتی)cpfds->pfds[curl_nfds + i].revents;
      غیرعلامتی مختصر نقاب = 0;
      اگر(r & POLLIN)
        نقاب |= CURL_WAIT_POLLIN;
      اگر(r & POLLOUT)
        نقاب |= CURL_WAIT_POLLOUT;
      اگر(r & POLLPRI)
        نقاب |= CURL_WAIT_POLLPRI;
      extra_fds[i].revents = (مختصر)نقاب;
    }
  }
  ورنہ اگر(extrawait) {
    /* No fds to poll, but asked to obey timeout_ms anyway. We cannot
     * use Curl_poll() as it, on some platforms, returns immediately
     * without fds. */
    curlx_wait_ms(timeout_ms);
  }

برآمد:
  *pnevents = nevents;
  واپس mresult;
}

#اگر_ختم /* !USE_WINSOCK */

#تعریف_کرو عدد_POLLS_ON_ڈھیر 10

ساکن CURLMcode multi_wait(ساخت Curl_multi *multi,
                            ساخت curl_waitfd extra_fds[],
                            غیرعلامتی صحیح extra_nfds,
                            صحیح timeout_ms,
                            صحیح *ret,
                            bool extrawait)  /* when no socket, wait */
{
  size_t i;
  صحیح timeout_اندرونی;
  صحیح nevents = 0;
  ساخت easy_pollset ps;
  ساخت pollfd a_few_on_ڈھیر[عدد_POLLS_ON_ڈھیر];
  ساخت curl_pollfds cpfds;
  غیرعلامتی صحیح curl_nfds = 0; /* how many pfds are for curl transfers */
  ساخت Curl_easy *ڈیٹا = تہی;
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;
#اگر_تعریف_شدہ ENABLE_WAKEUP
  صحیح wakeup_idx = -1;
#اگر_ختم

  اگر(timeout_ms < 0)
    واپس CURLM_BAD_FUNCTION_دلیل;

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ڈھیر, عدد_POLLS_ON_ڈھیر);

  /* Add the curl handles to our pollfds first */
  اگر(Curl_uint32_bset_پہلا(&multi->process, &mid)) {
    کرو {
      ڈیٹا = Curl_multi_get_easy(multi, mid);
      اگر(!ڈیٹا) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        جاری;
      }
      Curl_multi_pollset(ڈیٹا, &ps);
      اگر(Curl_pollfds_جوڑو_ps(&cpfds, &ps)) {
        mresult = CURLM_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
    } جب(Curl_uint32_bset_اگلا(&multi->process, mid, &mid));
  }

  اگر(Curl_cshutdn_جوڑو_pollfds(&multi->cshutdn, multi->admin, &cpfds)) {
    mresult = CURLM_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

#اگر_تعریف_شدہ ENABLE_WAKEUP
  /* If `extrawait` is TRUE *or* we have `extra_fds`to poll *or* we
   * have transfer sockets to poll, we obey `timeout_ms`.
   * Then we need to also monitor the multi's wakeup
   * socket to catch calls to `curl_multi_wakeup()` during the wait. */
  اگر(extrawait || cpfds.n || extra_nfds) {
    wakeup_idx = cpfds.n;
    اگر(Curl_pollfds_جوڑو_sock(&cpfds, multi->wakeup_pair[0], POLLIN)) {
      mresult = CURLM_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }
#اگر_ختم

  curl_nfds = cpfds.n; /* what curl internally uses in cpfds */
  /* Add external file descriptions from poll-like struct curl_waitfd */
  برائے(i = 0; i < extra_nfds; i++) {
    غیرعلامتی مختصر events = 0;
    اگر(extra_fds[i].events & CURL_WAIT_POLLIN)
      events |= POLLIN;
    اگر(extra_fds[i].events & CURL_WAIT_POLLPRI)
      events |= POLLPRI;
    اگر(extra_fds[i].events & CURL_WAIT_POLLOUT)
      events |= POLLOUT;
    اگر(Curl_pollfds_جوڑو_sock(&cpfds, extra_fds[i].fd, events)) {
      mresult = CURLM_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }

  /* We check the internal timeout *AFTER* we collected all sockets to
   * poll. Collecting the sockets may install new timers by protocols
   * and connection filters.
   * Use the shorter one of the internal and the caller requested timeout.
   * If we are called with `!extrawait` and multi_timeout() reports no
   * timeouts exist, do not wait. */
  multi_timeout(multi, تہی, &timeout_اندرونی);
  اگر((timeout_اندرونی >= 0) && (timeout_اندرونی < timeout_ms))
    timeout_ms = timeout_اندرونی;

  اگر(ڈیٹا)
    CURL_TRC_M(ڈیٹا, "multi_wait(fds=%u, timeout=%d) tinternal=%d",
               cpfds.n, timeout_ms, timeout_اندرونی);

#اگر_تعریف_شدہ USE_WINSOCK
  mresult = multi_winsock_select(multi, &cpfds, curl_nfds,
                                 extra_fds, extra_nfds,
                                 timeout_ms, extrawait, &nevents);
#ورنہ
  mresult = multi_posix_poll(multi, &cpfds, curl_nfds,
                             extra_fds, extra_nfds,
                             timeout_ms, extrawait, &nevents);
#اگر_ختم

#اگر_تعریف_شدہ ENABLE_WAKEUP
    اگر(nevents && (wakeup_idx >= 0)) {
      اگر(cpfds.pfds[wakeup_idx].revents & POLLIN) {
        (خالی)Curl_wakeup_consume(multi->wakeup_pair, TRUE);
        /* do not count the wakeup socket into the returned value */
        nevents--;
      }
    }
#اگر_ختم

برآمد:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  اگر(ret)
    *ret = nevents;
  واپس mresult;
}

CURLMcode curl_multi_wait(CURLM *m,
                          ساخت curl_waitfd extra_fds[],
                          غیرعلامتی صحیح extra_nfds,
                          صحیح timeout_ms,
                          صحیح *ret)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_wait, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, FALSE);
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

CURLMcode curl_multi_poll(CURLM *m,
                          ساخت curl_waitfd extra_fds[],
                          غیرعلامتی صحیح extra_nfds,
                          صحیح timeout_ms,
                          صحیح *ret)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_poll, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, TRUE);
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

CURLMcode curl_multi_wakeup(CURLM *m)
{
  /* this function is usually called from another thread,
     it has to be careful only to access parts of the
     Curl_multi struct that are constant */
  ساخت Curl_multi *multi = m;
  CURLMcode mresult = CURLM_WAKEUP_FAILURE;

  /* GOOD_MULTI_HANDLE can be safely called */
  اگر(!GOOD_MULTI_HANDLE(multi))
    واپس CURLM_BAD_HANDLE;

#اگر_تعریف_شدہ ENABLE_WAKEUP
  /* the wakeup_pair variable is only written during init and cleanup,
     making it safe to access from another thread after the init part
     and before cleanup */
  اگر(!Curl_wakeup_signal(multi->wakeup_pair))
    mresult = CURLM_OK;
#اگر_ختم
#اگر_تعریف_شدہ USE_WINSOCK
  اگر(WSASetEvent(multi->wsa_event))
    mresult = CURLM_OK;
#اگر_ختم
  واپس mresult;
}

#اگر_تعریف_شدہ ENABLE_اندرونی_WAKEUP
خالی Curl_multi_wakeup_اندرونی(ساخت Curl_multi *multi)
{
  /* This is expected to be invocable from another thread which
   * does NOT outlive the multi handle. Check for sanity. */
  اگر(GOOD_MULTI_HANDLE(multi))
    Curl_wakeup_signal(multi->wakeup_اندرونی);
  ورنہ
    DEBUGASSERT(0);
}
#اگر_ختم

/*
 * multi_ischanged() is called
 *
 * Returns TRUE/FALSE whether the state is changed to trigger a CONNECT_PEND
 * => CONNECT action.
 *
 * Set 'clear' to TRUE to have it also clear the state variable.
 */
ساکن bool multi_ischanged(ساخت Curl_multi *multi, bool clear)
{
  bool retval = FALSE;
  DEBUGASSERT(multi);
  اگر(multi) {
    retval = (bool)multi->recheckstate;
    اگر(clear)
      multi->recheckstate = FALSE;
  }
  واپس retval;
}

/*
 * Curl_multi_connchanged() is called to tell that there is a connection in
 * this multi handle that has changed state (multiplexing become possible, the
 * number of allowed streams changed or similar), and a subsequent use of this
 * multi handle should move CONNECT_PEND handles back to CONNECT to have them
 * retry.
 */
خالی Curl_multi_connchanged(ساخت Curl_multi *multi)
{
  /* May be invoked for a `data->multi` when no multi is present. */
  اگر(multi)
    multi->recheckstate = TRUE;
}

CURLMcode Curl_multi_جوڑو_perform(ساخت Curl_multi *multi,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت connectdata *conn)
{
  CURLMcode mresult;

  mresult = Curl_multi_جوڑو_handle(multi, ڈیٹا);
  اگر(!mresult) {
    CURLcode نتیجہ;

    /* pass in NULL for 'conn' here since we do not want to init the
       connection, only this transfer */
    نتیجہ = Curl_init_transfer(ڈیٹا, تہی);
    اگر(نتیجہ) {
      Curl_multi_remove_handle(multi, ڈیٹا);
      واپس CURLM_اندرونی_ERROR;
    }

    /* take this handle to the perform state right away */
    multistate(ڈیٹا, MSTATE_PERFORMING);
    Curl_attach_connection(ڈیٹا, conn, TRUE);
    CURL_REQ_سیٹ_RECV(ڈیٹا);
  }
  واپس mresult;
}

ساکن CURLcode multi_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(conn->scheme);

  اگر(conn->scheme->تسلسل->do_it)
    نتیجہ = conn->scheme->تسلسل->do_it(ڈیٹا, مکمل);

  واپس نتیجہ;
}

/*
 * multi_do_more() is called during the DO_MORE multi state. It is a second
 * stage DO state which (wrongly) was introduced to support FTP's second
 * connection.
 *
 * 'complete' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (to DOING state when there is more work to do)
 */

ساکن CURLcode multi_do_مزید(ساخت Curl_easy *ڈیٹا, domore *complete)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;

  *complete = DOMORE_INCOMPLETE;

  اگر(conn->scheme->تسلسل->do_مزید)
    نتیجہ = conn->scheme->تسلسل->do_مزید(ڈیٹا, complete);

  واپس نتیجہ;
}

/*
 * Check whether a timeout occurred, and handle it if it did
 */
ساکن bool multi_handle_timeout(ساخت Curl_easy *ڈیٹا,
                                 ثابت ساخت curltime *pnow,
                                 bool *دھارا_error,
                                 CURLcode *نتیجہ)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_timeleft_now_ms(ڈیٹا, pnow);
  اگر(timeout_ms < 0) {
    /* Handle timed out */
    timerid بنیاد_timer = Curl_is_connecting(ڈیٹا) ?
                         TIMER_STARTSINGLE : TIMER_STARTOP;
    timediff_t elapsed_ms = Curl_pgrs_since_ms(ڈیٹا, تہی, بنیاد_timer);
    اگر(ڈیٹا->mstate == MSTATE_CONNECTING) {
      failf(ڈیٹا, "%s timed out after %" FMT_TIMEDIFF_T " milliseconds",
            ڈیٹا->conn->بٹ.dns_resolved ? "Connection" : "Resolving",
            elapsed_ms);
      /* keep what we know about a partly done connect, e.g. TCP done
       * but TLS handshake still ongoing */
      Curl_conn_report_failed(ڈیٹا, FIRSTSOCKET);
    }
    ورنہ {
      ساخت SingleRequest *k = &ڈیٹا->req;
      اگر(k->سائز != -1) {
        failf(ڈیٹا, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount, k->سائز);
      }
      ورنہ {
        failf(ڈیٹا, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount);
      }
    }
    *نتیجہ = CURLE_OPERATION_TIMEDOUT;
    اگر(ڈیٹا->conn) {
      /* Force connection closed if the connection has indeed been used */
      اگر(ڈیٹا->mstate > MSTATE_DO) {
        streamclose(ڈیٹا->conn);
        *دھارا_error = TRUE;
      }
      (خالی)multi_مکمل(ڈیٹا, *نتیجہ, TRUE);
    }
    واپس TRUE;
  }

  واپس FALSE;
}

/*
 * We are doing protocol-specific connecting and this is being called over and
 * over from the multi interface until the connection phase is done on
 * protocol layer.
 */

ساکن CURLcode protocol_connecting(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;

  اگر(conn && conn->scheme->تسلسل->connecting) {
    *مکمل = FALSE;
    نتیجہ = conn->scheme->تسلسل->connecting(ڈیٹا, مکمل);
  }
  ورنہ
    *مکمل = TRUE;

  واپس نتیجہ;
}

/*
 * We are DOING this is being called over and over from the multi interface
 * until the DOING phase is done on protocol layer.
 */

ساکن CURLcode protocol_doing(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت connectdata *conn = ڈیٹا->conn;

  اگر(conn && conn->scheme->تسلسل->doing) {
    *مکمل = FALSE;
    نتیجہ = conn->scheme->تسلسل->doing(ڈیٹا, مکمل);
  }
  ورنہ
    *مکمل = TRUE;

  واپس نتیجہ;
}

/*
 * We have discovered that the TCP connection has been successful, we can now
 * proceed with some action.
 *
 */
ساکن CURLcode protocol_connect(ساخت Curl_easy *ڈیٹا, bool *protocol_مکمل)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(conn);
  DEBUGASSERT(protocol_مکمل);
  DEBUGASSERT(Curl_conn_is_connected(conn, FIRSTSOCKET));

  *protocol_مکمل = FALSE;
  اگر(!conn->بٹ.protoconnstart) {
    اگر(conn->scheme->تسلسل->connect_it) {
      /* Call the protocol-specific connect function */
      نتیجہ = conn->scheme->تسلسل->connect_it(ڈیٹا, protocol_مکمل);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    conn->بٹ.protoconnstart = TRUE;
  }

  /* Unless this protocol does not have any protocol-connect callback, as
     then we know we are done. */
  اگر(!conn->scheme->تسلسل->connecting)
    *protocol_مکمل = TRUE;
  واپس CURLE_OK;
}

/*
 * posttransfer() is called immediately after a transfer ends
 */
ساکن خالی multi_posttransfer(ساخت Curl_easy *ڈیٹا)
{
#اگر defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
  /* restore the signal handler for SIGPIPE before we get back */
  اگر(!ڈیٹا->سیٹ.no_signal)
    اشارہ(SIGPIPE, ڈیٹا->state.پچھلا_signal);
#ورنہ
  (خالی)ڈیٹا;
#اگر_ختم
}

/*
 * multi_follow() handles the URL redirect magic. Pass in the 'newurl' string
 * as given by the remote server and set up the new URL to request.
 *
 * This function DOES NOT FREE the given URL.
 */
ساکن CURLcode multi_follow(ساخت Curl_easy *ڈیٹا,
                             ثابت ساخت Curl_scheme *handler,
                             ثابت حرف *newurl, /* the Location: string */
                             followtype type) /* see transfer.h */
{
  اگر(handler && handler->تسلسل->follow)
    واپس handler->تسلسل->follow(ڈیٹا, newurl, type);

  اگر(type == FOLLOW_RETRY)
    /* Retries are generic and do not require protocol-specific redirect
       handling. */
    واپس CURLE_OK;

  واپس CURLE_TOO_MANY_REDIRECTS;
}

ساکن CURLcode mspeed_check(ساخت Curl_easy *ڈیٹا)
{
  اگر(Curl_rlimit_فعال(&ڈیٹا->پیش_رفت.dl.rlimit) ||
     Curl_rlimit_فعال(&ڈیٹا->پیش_رفت.ul.rlimit)) {
    /* check if our send/recv limits require idle waits */
    ثابت ساخت curltime *pnow = Curl_pgrs_now(ڈیٹا);
    timediff_t recv_ms, send_ms;

    send_ms = Curl_rlimit_wait_ms(&ڈیٹا->پیش_رفت.ul.rlimit, pnow);
    recv_ms = Curl_rlimit_wait_ms(&ڈیٹا->پیش_رفت.dl.rlimit, pnow);

    اگر(send_ms || recv_ms) {
      اگر(ڈیٹا->mstate != MSTATE_RATELIMITING) {
        multistate(ڈیٹا, MSTATE_RATELIMITING);
      }
      Curl_expire_سیٹ(ڈیٹا, EXPIRE_TOOFAST, CURLMAX(send_ms, recv_ms), pnow);
      Curl_multi_clear_dirty(ڈیٹا);
      CURL_TRC_M(ڈیٹا, "[RLIMIT] waiting %" FMT_TIMEDIFF_T "ms",
                 CURLMAX(send_ms, recv_ms));
      واپس CURLE_AGAIN;
    }
    ورنہ {
      /* when will the rate limits increase next? The transfer needs
       * to run again at that time or it may stall. */
      send_ms = Curl_rlimit_اگلا_قدم_ms(&ڈیٹا->پیش_رفت.ul.rlimit, pnow);
      recv_ms = Curl_rlimit_اگلا_قدم_ms(&ڈیٹا->پیش_رفت.dl.rlimit, pnow);
      اگر(send_ms || recv_ms) {
        timediff_t اگلا_ms = CURLMIN(send_ms, recv_ms);
        اگر(!اگلا_ms)
          اگلا_ms = CURLMAX(send_ms, recv_ms);
        Curl_expire_سیٹ(ڈیٹا, EXPIRE_TOOFAST, اگلا_ms, pnow);
        CURL_TRC_M(ڈیٹا, "[RLIMIT] next token update in %" FMT_TIMEDIFF_T "ms",
                   اگلا_ms);
      }
    }
  }

  اگر(ڈیٹا->mstate != MSTATE_PERFORMING) {
    CURL_TRC_M(ڈیٹا, "[RLIMIT] wait over, continue");
    multistate(ڈیٹا, MSTATE_PERFORMING);
  }
  واپس CURLE_OK;
}

ساکن CURLMcode multistate_performing(ساخت Curl_easy *ڈیٹا,
                                       bool *دھارا_errorp,
                                       CURLcode *resultp)
{
  حرف *newurl = تہی;
  bool retry = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجہ = *resultp = CURLE_OK;
  *دھارا_errorp = FALSE;

  اگر(mspeed_check(ڈیٹا) == CURLE_AGAIN)
    واپس CURLM_OK;

  /* read/write data if it is ready to do so */
  نتیجہ = Curl_sendrecv(ڈیٹا);

  اگر(ڈیٹا->req.مکمل || (نتیجہ == CURLE_RECV_ERROR)) {
    /* If CURLE_RECV_ERROR happens early enough, we assume it was a race
     * condition and the server closed the reused connection exactly when we
     * wanted to use it, so figure out if that is indeed the case.
     */
    CURLcode ret = Curl_retry_request(ڈیٹا, &newurl);
    اگر(!ret)
      retry = !!newurl;
    ورنہ اگر(!نتیجہ)
      نتیجہ = ret;

    اگر(retry) {
      /* if we are to retry, set the result to OK and consider the
         request as done */
      نتیجہ = CURLE_OK;
      ڈیٹا->req.مکمل = TRUE;
    }
  }
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  ورنہ اگر((نتیجہ == CURLE_HTTP2_دھارا) &&
          Curl_h2_http_1_1_error(ڈیٹا)) {
    CURLcode ret = Curl_retry_request(ڈیٹا, &newurl);

    اگر(!ret) {
      infof(ڈیٹا, "Downgrades to HTTP/1.1");
      streamclose(ڈیٹا->conn);
      ڈیٹا->state.http_neg.wanted = CURL_HTTP_V1x;
      ڈیٹا->state.http_neg.allowed = CURL_HTTP_V1x;
      /* clear the error message bit too as we ignore the one we got */
      ڈیٹا->state.errorbuf = FALSE;
      اگر(!newurl)
        /* typically for HTTP_1_1_REQUIRED error on first flight */
        newurl = Curl_bufref_dup(&ڈیٹا->state.url);
      اگر(!newurl) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
      }
      ورنہ {
        /* if we are to retry, set the result to OK and consider the request
          as done */
        retry = TRUE;
        نتیجہ = CURLE_OK;
        ڈیٹا->req.مکمل = TRUE;
      }
    }
    ورنہ
      نتیجہ = ret;
  }
#اگر_ختم

  اگر(نتیجہ) {
    /*
     * The transfer phase returned error, we mark the connection to get closed
     * to prevent being reused. This is because we cannot possibly know if the
     * connection is in a good shape or not now. Unless it is a protocol which
     * uses two "channels" like FTP, as then the error happened in the data
     * connection.
     */

    اگر(!(ڈیٹا->conn->scheme->flags & PROTOPT_DUAL) &&
       نتیجہ != CURLE_HTTP2_دھارا)
      streamclose(ڈیٹا->conn);

    multi_posttransfer(ڈیٹا);
    multi_مکمل(ڈیٹا, نتیجہ, TRUE);
  }
  ورنہ اگر(ڈیٹا->req.مکمل && !Curl_cwriter_is_paused(ڈیٹا)) {
    ثابت ساخت Curl_scheme *handler = ڈیٹا->conn->scheme;

    /* call this even if the readwrite function returned error */
    multi_posttransfer(ڈیٹا);

    /* When we follow redirects or is set to retry the connection, we must to
       go back to the CONNECT state */
    اگر(ڈیٹا->req.newurl || retry) {
      followtype follow = FOLLOW_NONE;
      اگر(!retry) {
        /* if the URL is a follow-location and not a retried request then
           figure out the URL here */
        curlx_free(newurl);
        newurl = ڈیٹا->req.newurl;
        ڈیٹا->req.newurl = تہی;
        follow = FOLLOW_REDIR;
      }
      ورنہ
        follow = FOLLOW_RETRY;
      (خالی)multi_مکمل(ڈیٹا, CURLE_OK, FALSE);
      /* multi_done() might return CURLE_GOT_NOTHING */
      نتیجہ = multi_follow(ڈیٹا, handler, newurl, follow);
      اگر(!نتیجہ) {
        multistate(ڈیٹا, MSTATE_تیاری);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    ورنہ {
      /* after the transfer is done, go DONE */

      /* but first check to see if we got a location info even though we are
         not following redirects */
      اگر(ڈیٹا->req.location) {
        curlx_free(newurl);
        newurl = ڈیٹا->req.location;
        ڈیٹا->req.location = تہی;
        نتیجہ = multi_follow(ڈیٹا, handler, newurl, FOLLOW_FAKE);
        اگر(نتیجہ) {
          *دھارا_errorp = TRUE;
          نتیجہ = multi_مکمل(ڈیٹا, نتیجہ, TRUE);
        }
      }

      اگر(!نتیجہ) {
        multistate(ڈیٹا, MSTATE_مکمل);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
  }
  ورنہ { /* not errored, not done */
    mspeed_check(ڈیٹا);
  }
  curlx_free(newurl);
  *resultp = نتیجہ;
  واپس mresult;
}

ساکن CURLMcode multistate_do(ساخت Curl_easy *ڈیٹا,
                               bool *دھارا_errorp,
                               CURLcode *resultp)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجہ = CURLE_OK;
  اگر(ڈیٹا->سیٹ.fprereq) {
    ساخت Curl_mapi_guard guard;
    صحیح prereq_rc;

    /* call the prerequest callback function */
    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_fprereq);
    prereq_rc = ڈیٹا->سیٹ.fprereq(ڈیٹا->سیٹ.prereq_userp,
                                  ڈیٹا->info.primary.remote_ip,
                                  ڈیٹا->info.primary.local_ip,
                                  ڈیٹا->info.primary.remote_پورٹ,
                                  ڈیٹا->info.primary.local_پورٹ);
    CURL_CBAPI_انتہا(&guard);
    اگر(prereq_rc != CURL_PREREQFUNC_OK) {
      failf(ڈیٹا, "operation aborted by pre-request callback");
      /* failure in pre-request callback - do not do any other processing */
      نتیجہ = CURLE_ABORTED_BY_CALLBACK;
      multi_posttransfer(ڈیٹا);
      multi_مکمل(ڈیٹا, نتیجہ, FALSE);
      *دھارا_errorp = TRUE;
      جاؤ انتہا;
    }
  }

  اگر(ڈیٹا->سیٹ.connect_only && !ڈیٹا->سیٹ.connect_only_ws) {
    multistate(ڈیٹا, MSTATE_مکمل);
    mresult = CURLM_CALL_MULTI_PERFORM;
  }
  ورنہ {
    bool dophase_مکمل = FALSE;
    /* Perform the protocol's DO action */
    نتیجہ = multi_do(ڈیٹا, &dophase_مکمل);

    اگر(!نتیجہ) {
      اگر(!dophase_مکمل) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
        /* some steps needed for wildcard matching */
        اگر(ڈیٹا->state.wildcardmatch) {
          ساخت Wildcard_ڈیٹا *wc = ڈیٹا->wildcard;
          اگر(wc->state == CURLWC_مکمل || wc->state == CURLWC_SKIP) {
            /* skip some states if it is important */
            multi_مکمل(ڈیٹا, CURLE_OK, FALSE);

            /* if there is no connection left, skip the DONE state */
            multistate(ڈیٹا, ڈیٹا->conn ? MSTATE_مکمل : MSTATE_COMPLETED);
            mresult = CURLM_CALL_MULTI_PERFORM;
            جاؤ انتہا;
          }
        }
#اگر_ختم
        /* DO was not completed in one function call, we must continue
           DOING... */
        multistate(ڈیٹا, MSTATE_DOING);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }

      /* after DO, go DO_DONE... or DO_MORE */
      ورنہ اگر(ڈیٹا->conn->بٹ.do_مزید) {
        /* we are supposed to do more, but we need to sit down, relax and wait
           a little while first */
        multistate(ڈیٹا, MSTATE_DOING_مزید);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
      ورنہ {
        /* we are done with the DO, now DID */
        multistate(ڈیٹا, MSTATE_DID);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    ورنہ اگر((نتیجہ == CURLE_SEND_ERROR) &&
            ڈیٹا->conn->بٹ.reuse) {
      /*
       * In this situation, a connection that we were trying to use may have
       * unexpectedly died. If possible, send the connection back to the
       * CONNECT phase so we can try again.
       */
      ثابت ساخت Curl_scheme *handler = ڈیٹا->conn->scheme;
      حرف *newurl = تہی;
      followtype follow = FOLLOW_NONE;
      CURLcode drc;

      drc = Curl_retry_request(ڈیٹا, &newurl);
      اگر(drc) {
        /* a failure here pretty much implies an out of memory */
        نتیجہ = drc;
        *دھارا_errorp = TRUE;
      }

      multi_posttransfer(ڈیٹا);
      drc = multi_مکمل(ڈیٹا, نتیجہ, FALSE);

      /* When set to retry the connection, we must go back to the CONNECT
       * state */
      اگر(newurl) {
        اگر(!drc || (drc == CURLE_SEND_ERROR)) {
          follow = FOLLOW_RETRY;
          drc = multi_follow(ڈیٹا, handler, newurl, follow);
          اگر(!drc) {
            multistate(ڈیٹا, MSTATE_تیاری);
            mresult = CURLM_CALL_MULTI_PERFORM;
            نتیجہ = CURLE_OK;
          }
          ورنہ {
            /* Follow failed */
            نتیجہ = drc;
          }
        }
        ورنہ {
          /* done did not return OK or SEND_ERROR */
          نتیجہ = drc;
        }
      }
      ورنہ {
        /* Have error handler disconnect conn if we cannot retry */
        *دھارا_errorp = TRUE;
      }
      curlx_free(newurl);
    }
    ورنہ {
      /* failure detected */
      multi_posttransfer(ڈیٹا);
      اگر(ڈیٹا->conn)
        multi_مکمل(ڈیٹا, نتیجہ, FALSE);
      *دھارا_errorp = TRUE;
    }
  }
انتہا:
  *resultp = نتیجہ;
  واپس mresult;
}

ساکن CURLMcode multistate_ratelimiting(ساخت Curl_easy *ڈیٹا,
                                         CURLcode *resultp)
{
  CURLcode نتیجہ = CURLE_OK;
  CURLMcode mresult = CURLM_OK;
  DEBUGASSERT(ڈیٹا->conn);
  /* if both rates are within spec, resume transfer */
  نتیجہ = Curl_pgrsCheck(ڈیٹا);

  اگر(نتیجہ) {
    اگر(!(ڈیٹا->conn->scheme->flags & PROTOPT_DUAL) &&
       نتیجہ != CURLE_HTTP2_دھارا)
      streamclose(ڈیٹا->conn);

    multi_posttransfer(ڈیٹا);
    multi_مکمل(ڈیٹا, نتیجہ, TRUE);
  }
  ورنہ {
    اگر(!mspeed_check(ڈیٹا))
      mresult = CURLM_CALL_MULTI_PERFORM;
  }
  *resultp = نتیجہ;
  واپس mresult;
}

ساکن CURLMcode multistate_connect(ساخت Curl_multi *multi,
                                    ساخت Curl_easy *ڈیٹا,
                                    CURLcode *resultp)
{
  /* Connect. We want to get a connection identifier filled in. This state can
     be entered from SETUP and from PENDING. */
  bool connected;
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجہ = Curl_connect(ڈیٹا, &connected);
  اگر(نتیجہ == CURLE_NO_CONNECTION_دستیاب) {
    /* There was no connection available. We will go to the pending state and
       wait for an available connection. */
    multistate(ڈیٹا, MSTATE_PENDING);
    /* move from process to pending set */
    Curl_uint32_bset_remove(&multi->process, ڈیٹا->mid);
    Curl_uint32_bset_remove(&multi->dirty, ڈیٹا->mid);
    Curl_uint32_bset_جوڑو(&multi->pending, ڈیٹا->mid);
    *resultp = CURLE_OK;
    واپس mresult;
  }
  ورنہ
    multi_جدول_وقت_pending(ڈیٹا->multi);

  اگر(!نتیجہ) {
    /* after the connect has been sent off, go WAITCONNECT unless the
       protocol connect is already done and we can go directly to WAITDO or
       DO! */
    mresult = CURLM_CALL_MULTI_PERFORM;

    اگر(connected) {
      اگر(!ڈیٹا->conn->بٹ.reuse &&
         Curl_conn_is_multiplex(ڈیٹا->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_جدول_وقت_pending(ڈیٹا->multi);
      }
      multistate(ڈیٹا, MSTATE_PROTOCONNECT);
    }
    ورنہ {
      multistate(ڈیٹا, MSTATE_CONNECTING);
    }
  }
  *resultp = نتیجہ;
  واپس mresult;
}

/* returns the possibly updated result */
ساکن CURLcode is_ختم_شدہ(ساخت Curl_multi *multi,
                            ساخت Curl_easy *ڈیٹا,
                            ثابت ساخت curltime *pnow,
                            bool دھارا_error,
                            CURLcode نتیجہ)
{
  اگر(ڈیٹا->mstate < MSTATE_COMPLETED) {
    اگر(نتیجہ) {
      /*
       * If an error was returned, and we are not in completed state now,
       * then we go to completed and consider this transfer aborted.
       */

      /* No attempt to disconnect connections must be made before this -
         connection detach and termination happens only here */

      /* Check if we can move pending requests to send pipe */
      multi_جدول_وقت_pending(multi); /* connection */

      اگر(ڈیٹا->conn) {
        اگر(دھارا_error) {
          /* Do not attempt to send data over a connection that timed out */
          bool dead_connection = نتیجہ == CURLE_OPERATION_TIMEDOUT;
          ساخت connectdata *conn = ڈیٹا->conn;

          /* This is where we make sure that the conn pointer is reset.
             We do not have to do this in every case block above where a
             failure is detected */
          Curl_detach_connection(ڈیٹا);
          Curl_conn_close(ڈیٹا, conn, dead_connection);
        }
      }
      ورنہ اگر(ڈیٹا->mstate == MSTATE_CONNECT) {
        /* Curl_connect() failed */
        multi_posttransfer(ڈیٹا);
        Curl_pgrsUpdate_nometer(ڈیٹا);
      }

      multistate(ڈیٹا, MSTATE_COMPLETED);
      واپس نتیجہ;
    }
    /* if there is still a connection to use, call the progress function */
    ورنہ اگر(ڈیٹا->conn && Curl_conn_is_connected(ڈیٹا->conn, FIRSTSOCKET)) {
      نتیجہ = Curl_pgrsUpdateX(ڈیٹا, pnow);
      اگر(نتیجہ) {
        /* aborted due to progress callback return code must close the
           connection */
        streamclose(ڈیٹا->conn);

        /* if not yet in DONE state, go there. */
        اگر(ڈیٹا->mstate < MSTATE_مکمل)
          multistate(ڈیٹا, MSTATE_مکمل);
        واپس نتیجہ;
      }
    }
  }
  واپس نتیجہ;
}

ساکن خالی handle_completed(ساخت Curl_multi *multi,
                             ساخت Curl_easy *ڈیٹا,
                             CURLcode نتیجہ)
{
  bool پیغام_مقصد_app = ڈیٹا->master_mid == UINT32_MAX;

  اگر(!پیغام_مقصد_app) {
    /* A sub transfer, not reported to the application. Is anyone still
     * interested in processing its results? */
    اگر(ڈیٹا->sub_xfer_مکمل) {
      ساخت Curl_easy *master = Curl_multi_get_easy(multi, ڈیٹا->master_mid);

      CURL_TRC_M(ڈیٹا, "sub xfer done for master %u", ڈیٹا->master_mid);
      اگر(master)
        ڈیٹا->sub_xfer_مکمل(ڈیٹا, master, نتیجہ);
      ورنہ
        CURL_TRC_M(ڈیٹا, "master easy %u already gone.", ڈیٹا->master_mid);
    }
  }
  ورنہ {
    /* now fill in the CURLMsg with this info */
    ساخت CURLMsg *پیغام = &ڈیٹا->پیغام;

    پیغام->پیغام = CURLMSG_مکمل;
    پیغام->easy_handle = ڈیٹا;
    پیغام->ڈیٹا.نتیجہ = نتیجہ;

    DEBUGASSERT(!ڈیٹا->conn);
  }
  multistate(ڈیٹا, MSTATE_MSGSENT);

  /* remove from the other sets */
  Curl_uint32_bset_remove(&multi->process, ڈیٹا->mid);
  Curl_uint32_bset_remove(&multi->dirty, ڈیٹا->mid);
  Curl_uint32_bset_remove(&multi->pending, ڈیٹا->mid);
  اگر(پیغام_مقصد_app)
    multi_addmsg(multi, ڈیٹا);
  اگر(ڈیٹا->state.really_alive) {
    ڈیٹا->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    اگر(!multi->xfers_really_alive)
      (خالی)multi_assess_wakeup(multi);
  }
  --multi->xfers_alive;
  اگر(!multi->xfers_alive)
    multi_assess_wakeup(multi);
}

ساکن CURLMcode multistate_init(ساخت Curl_easy *ڈیٹا, CURLcode *نتیجہ)
{
  اگر(!ڈیٹا->state.really_alive) {
    ڈیٹا->state.really_alive = TRUE;
    ++ڈیٹا->multi->xfers_really_alive;
    اگر(ڈیٹا->multi->xfers_really_alive == 1) {
      CURLMcode mresult = multi_assess_wakeup(ڈیٹا->multi);
      اگر(mresult) {
        failf(ڈیٹا, "error enabling wakeup listening: %d", mresult);
        واپس mresult;
      }
    }
  }

  *نتیجہ = Curl_pretransfer(ڈیٹا);
  اگر(*نتیجہ)
    واپس CURLM_OK;

  /* after init, go SETUP */
  multistate(ڈیٹا, MSTATE_تیاری);
  Curl_pgrs_وقت(ڈیٹا, TIMER_STARTOP);
  واپس CURLM_CALL_MULTI_PERFORM;
}

ساکن CURLMcode multistate_تیاری(ساخت Curl_easy *ڈیٹا)
{
  ثابت ساخت curltime *pnow = Curl_pgrs_now(ڈیٹا);
  Curl_pgrs_وقت_Was(ڈیٹا, TIMER_STARTSINGLE, *pnow);
  اگر(ڈیٹا->سیٹ.timeout)
    Curl_expire_سیٹ(ڈیٹا, EXPIRE_TIMEOUT, ڈیٹا->سیٹ.timeout, pnow);
  اگر(ڈیٹا->سیٹ.connecttimeout)
    /* Since a connection might go to pending and back to CONNECT several
       times before it actually takes off, we need to set the timeout once
       in SETUP before we enter CONNECT the first time. */
    Curl_expire_سیٹ(ڈیٹا, EXPIRE_CONNECTTIMEOUT,
                    ڈیٹا->سیٹ.connecttimeout, pnow);

  multistate(ڈیٹا, MSTATE_CONNECT);
  واپس CURLM_CALL_MULTI_PERFORM;
}

ساکن CURLMcode multistate_connecting(ساخت Curl_easy *ڈیٹا,
                                       bool *دھارا_error,
                                       CURLcode *نتیجہ)
{
  bool connected;

  اگر(!ڈیٹا->conn) {
    DEBUGASSERT(0);
    *نتیجہ = CURLE_FAILED_INIT;
    واپس CURLM_OK;
  }
  اگر(!Curl_xfer_recv_is_paused(ڈیٹا)) {
    *نتیجہ = Curl_conn_connect(ڈیٹا, FIRSTSOCKET, FALSE, &connected);
    اگر(connected && !*نتیجہ) {
      اگر(!ڈیٹا->conn->بٹ.reuse &&
         Curl_conn_is_multiplex(ڈیٹا->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_جدول_وقت_pending(ڈیٹا->multi);
      }
      multistate(ڈیٹا, MSTATE_PROTOCONNECT);
      واپس CURLM_CALL_MULTI_PERFORM;
    }
    ورنہ اگر(*نتیجہ) {
      /* failure detected */
      CURL_TRC_M(ڈیٹا, "connect failed -> %d", (صحیح)*نتیجہ);
      multi_posttransfer(ڈیٹا);
      multi_مکمل(ڈیٹا, *نتیجہ, TRUE);
      *دھارا_error = TRUE;
      واپس CURLM_OK;
    }
  }
  واپس CURLM_OK;
}

ساکن CURLMcode multistate_protoconnect(ساخت Curl_easy *ڈیٹا,
                                         bool *دھارا_error,
                                         CURLcode *نتیجہ)
{
  bool protocol_connected = FALSE;

  اگر(!*نتیجہ && ڈیٹا->conn->بٹ.reuse) {
    /* ftp seems to hang when protoconnect on reused connection since we
     * handle PROTOCONNECT in general inside the filters, it seems wrong to
     * restart this on a reused connection.
     */
    multistate(ڈیٹا, MSTATE_DO);
    واپس CURLM_CALL_MULTI_PERFORM;
  }
  اگر(!*نتیجہ)
    *نتیجہ = protocol_connect(ڈیٹا, &protocol_connected);
  اگر(!*نتیجہ && !protocol_connected) {
    /* switch to waiting state */
    multistate(ڈیٹا, MSTATE_PROTOCONNECTING);
    واپس CURLM_CALL_MULTI_PERFORM;
  }
  ورنہ اگر(!*نتیجہ) {
    /* protocol connect has completed, go WAITDO or DO */
    multistate(ڈیٹا, MSTATE_DO);
    واپس CURLM_CALL_MULTI_PERFORM;
  }

  /* failure detected */
  multi_posttransfer(ڈیٹا);
  multi_مکمل(ڈیٹا, *نتیجہ, TRUE);
  *دھارا_error = TRUE;
  واپس CURLM_OK;
}

ساکن CURLMcode multistate_protoconnecting(ساخت Curl_easy *ڈیٹا,
                                            bool *دھارا_error,
                                            CURLcode *نتیجہ)
{
  bool protocol_connected = FALSE;

  /* protocol-specific connect phase */
  *نتیجہ = protocol_connecting(ڈیٹا, &protocol_connected);
  اگر(!*نتیجہ && protocol_connected) {
    /* after the connect has completed, go WAITDO or DO */
    multistate(ڈیٹا, MSTATE_DO);
    واپس CURLM_CALL_MULTI_PERFORM;
  }
  ورنہ اگر(*نتیجہ) {
    /* failure detected */
    multi_posttransfer(ڈیٹا);
    multi_مکمل(ڈیٹا, *نتیجہ, TRUE);
    *دھارا_error = TRUE;
  }
  واپس CURLM_OK;
}

ساکن CURLMcode multistate_doing(ساخت Curl_easy *ڈیٹا,
                                  bool *دھارا_error,
                                  CURLcode *نتیجہ)
{
  bool dophase_مکمل = FALSE;

  /* we continue DOING until the DO phase is complete */
  DEBUGASSERT(ڈیٹا->conn);
  *نتیجہ = protocol_doing(ڈیٹا, &dophase_مکمل);
  اگر(!*نتیجہ) {
    اگر(dophase_مکمل) {
      /* after DO, go DO_DONE or DO_MORE */
      multistate(ڈیٹا, ڈیٹا->conn->بٹ.do_مزید ?
                 MSTATE_DOING_مزید : MSTATE_DID);
      واپس CURLM_CALL_MULTI_PERFORM;
    } /* dophase_done */
  }
  ورنہ {
    /* failure detected */
    multi_posttransfer(ڈیٹا);
    multi_مکمل(ڈیٹا, *نتیجہ, FALSE);
    *دھارا_error = TRUE;
  }
  واپس CURLM_OK;
}

ساکن CURLMcode multistate_doing_مزید(ساخت Curl_easy *ڈیٹا,
                                       bool *دھارا_error,
                                       CURLcode *نتیجہ)
{
  domore control;

  /*
   * When we are connected, DOING MORE and then go DID
   */
  DEBUGASSERT(ڈیٹا->conn);
  *نتیجہ = multi_do_مزید(ڈیٹا, &control);

  اگر(!*نتیجہ) {
    اگر(control != DOMORE_INCOMPLETE) {
      /* if DONE, advance to DO_DONE
         if GOBACK, go back to DOING */
      multistate(ڈیٹا, control == DOMORE_مکمل ? MSTATE_DID : MSTATE_DOING);
      واپس CURLM_CALL_MULTI_PERFORM;
    }
    /* else
       stay in DO_MORE */
  }
  ورنہ {
    /* failure detected */
    multi_posttransfer(ڈیٹا);
    multi_مکمل(ڈیٹا, *نتیجہ, FALSE);
    *دھارا_error = TRUE;
  }
  واپس CURLM_OK;
}

ساکن CURLMcode multistate_did(ساخت Curl_multi *multi,
                                ساخت Curl_easy *ڈیٹا)
{
  DEBUGASSERT(ڈیٹا->conn);
  اگر(ڈیٹا->conn->بٹ.multiplex)
    /* Check if we can move pending requests to send pipe */
    multi_جدول_وقت_pending(multi); /* multiplexed */

  /* Only perform the transfer if there is a good socket to work with.
     Having both BAD is a signal to skip immediately to DONE */
  اگر(CONN_SOCK_IDX_VALID(ڈیٹا->conn->recv_idx) ||
     CONN_SOCK_IDX_VALID(ڈیٹا->conn->send_idx)) {
    multistate(ڈیٹا, MSTATE_PERFORMING);
    /* Do not return CURLM_CALL_MULTI_PERFORM to give other transfers
     * a chance to send off their requests.
     * Note: Some SFTP handlers do not seem to like this.
     *       Restrict it to HTTP families. */
    واپس ((multi->xfers_alive > 1) &&
            (ڈیٹا->conn->scheme->protocol & PROTO_FAMILY_HTTP)) ?
           CURLM_OK : CURLM_CALL_MULTI_PERFORM;
  }
  ورنہ {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
    اگر(ڈیٹا->state.wildcardmatch &&
       ((ڈیٹا->conn->scheme->flags & PROTOPT_WILDCARD) == 0)) {
      ڈیٹا->wildcard->state = CURLWC_مکمل;
    }
#اگر_ختم
    multistate(ڈیٹا, MSTATE_مکمل);
    واپس CURLM_CALL_MULTI_PERFORM;
  }
}

ساکن CURLMcode multistate_مکمل(ساخت Curl_easy *ڈیٹا, CURLcode *presult)
{
  اگر(ڈیٹا->conn) {
    CURLcode نتیجہ;

    /* post-transfer command */
    نتیجہ = multi_مکمل(ڈیٹا, *presult, FALSE);

    /* allow a previously set error code take precedence */
    اگر(!(*presult))
      *presult = نتیجہ;
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
  اگر(ڈیٹا->state.wildcardmatch) {
    اگر(ڈیٹا->wildcard->state != CURLWC_مکمل) {
      /* if a wildcard is set and we are not ending -> lets start again
         with MSTATE_INIT */
      multistate(ڈیٹا, MSTATE_INIT);
      واپس CURLM_CALL_MULTI_PERFORM;
    }
  }
#اگر_ختم
  /* after we have DONE what we are supposed to do, go COMPLETED, and
     it does not matter what the multi_done() returned! */
  multistate(ڈیٹا, MSTATE_COMPLETED);
  واپس CURLM_CALL_MULTI_PERFORM;
}

ساکن CURLMcode multi_runsingle(ساخت Curl_multi *multi,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت Curl_sigpipe_ctx *sigpipe_ctx)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجہ = CURLE_OK;
  ثابت ساخت curltime *pnow = تہی;

  اگر(multi->dead) {
    /* a multi-level callback returned error before, meaning every individual
     transfer now has failed */
    نتیجہ = CURLE_ABORTED_BY_CALLBACK;
    multi_posttransfer(ڈیٹا);
    multi_مکمل(ڈیٹا, نتیجہ, FALSE);
    multistate(ڈیٹا, MSTATE_COMPLETED);
  }

  multi_warn_debug(multi, ڈیٹا);

  /* transfer runs now, clear the dirty bit. This may be set
   * again during processing, triggering a re-run later. */
  Curl_uint32_bset_remove(&multi->dirty, ڈیٹا->mid);

  اگر(ڈیٹا == multi->admin) {
#اگر_تعریف_شدہ ENABLE_اندرونی_WAKEUP
    /* Consume any pending wakeup signals before processing.
     * This is necessary for event based processing. See #21547 */
    (خالی)Curl_wakeup_consume(multi->wakeup_اندرونی, TRUE);
#اگر_ختم
#اگر_تعریف_شدہ USE_RESOLV_THREADED
    Curl_async_thrdd_multi_process(multi);
#اگر_ختم
    Curl_cshutdn_perform(&multi->cshutdn, multi->admin, sigpipe_ctx);
    جاؤ برآمد;
  }

  sigpipe_apply(ڈیٹا, sigpipe_ctx);
  کرو {
    /* A "stream" here is a logical stream if the protocol can handle that
       (HTTP/2), or the full connection for older protocols */
    bool دھارا_error = FALSE;
    mresult = CURLM_OK;
    pnow = تہی;

    اگر(multi_ischanged(multi, TRUE)) {
      CURL_TRC_M(ڈیٹا, "multi changed, check CONNECT_PEND queue");
      multi_جدول_وقت_pending(multi); /* multiplexed */
    }

    اگر(ڈیٹا->mstate > MSTATE_CONNECT &&
       ڈیٹا->mstate < MSTATE_COMPLETED) {
      /* Make sure we set the connection's current owner */
      DEBUGASSERT(ڈیٹا->conn);
      اگر(!ڈیٹا->conn) {
        mresult = CURLM_اندرونی_ERROR;
        جاؤ برآمد;
      }
    }

    /* Wait for the connect state as only then is the start time stored, but
       we must not check already completed handles */
    اگر((ڈیٹا->mstate >= MSTATE_CONNECT) && (ڈیٹا->mstate < MSTATE_COMPLETED)) {
      pnow = Curl_pgrs_now(ڈیٹا);
      اگر(multi_handle_timeout(ڈیٹا, pnow, &دھارا_error, &نتیجہ))
        /* Skip the statemachine and go directly to error handling section. */
        جاؤ statemachine_انتہا;
      pnow = تہی;
    }

    انتخاب(ڈیٹا->mstate) {
    حالت MSTATE_INIT:
      /* Transitional state. init this transfer. A handle never comes back to
         this state. */
      mresult = multistate_init(ڈیٹا, &نتیجہ);
      توڑو;

    حالت MSTATE_تیاری:
      /* Transitional state. Setup things for a new transfer. The handle
         can come back to this state on a redirect. */
      mresult = multistate_تیاری(ڈیٹا);
      توڑو;

    حالت MSTATE_CONNECT:
      mresult = multistate_connect(multi, ڈیٹا, &نتیجہ);
      توڑو;

    حالت MSTATE_CONNECTING:
      /* awaiting a completion of an asynch TCP connect */
      mresult = multistate_connecting(ڈیٹا, &دھارا_error, &نتیجہ);
      توڑو;

    حالت MSTATE_PROTOCONNECT:
      mresult = multistate_protoconnect(ڈیٹا, &دھارا_error, &نتیجہ);
      توڑو;

    حالت MSTATE_PROTOCONNECTING:
      /* protocol-specific connect phase */
      mresult = multistate_protoconnecting(ڈیٹا, &دھارا_error, &نتیجہ);
      توڑو;

    حالت MSTATE_DO:
      mresult = multistate_do(ڈیٹا, &دھارا_error, &نتیجہ);
      توڑو;

    حالت MSTATE_DOING:
      /* we continue DOING until the DO phase is complete */
      mresult = multistate_doing(ڈیٹا, &دھارا_error, &نتیجہ);
      توڑو;

    حالت MSTATE_DOING_مزید:
      /*
       * When we are connected, DOING MORE and then go DID
       */
      mresult = multistate_doing_مزید(ڈیٹا, &دھارا_error, &نتیجہ);
      توڑو;

    حالت MSTATE_DID:
      mresult = multistate_did(multi, ڈیٹا);
      توڑو;

    حالت MSTATE_RATELIMITING: /* limit-rate exceeded in either direction */
      mresult = multistate_ratelimiting(ڈیٹا, &نتیجہ);
      توڑو;

    حالت MSTATE_PERFORMING:
      mresult = multistate_performing(ڈیٹا, &دھارا_error, &نتیجہ);
      توڑو;

    حالت MSTATE_مکمل:
      mresult = multistate_مکمل(ڈیٹا, &نتیجہ);
      توڑو;

    حالت MSTATE_COMPLETED:
      توڑو;

    حالت MSTATE_PENDING:
    حالت MSTATE_MSGSENT:
      /* handles in these states should NOT be in this list */
      توڑو;

    طےشدہ:
      mresult = CURLM_اندرونی_ERROR;
      جاؤ برآمد;
    }

    اگر(ڈیٹا->mstate >= MSTATE_CONNECT &&
       ڈیٹا->mstate < MSTATE_DO &&
       mresult != CURLM_CALL_MULTI_PERFORM &&
       !multi_ischanged(multi, FALSE)) {
      /* We now handle stream timeouts if and only if this will be the last
       * loop iteration. We only check this on the last iteration to ensure
       * that if we know we have additional work to do immediately
       * (i.e. CURLM_CALL_MULTI_PERFORM == TRUE) then we should do that before
       * declaring the connection timed out as we may almost have a completed
       * connection. */
      pnow = Curl_pgrs_now(ڈیٹا);
      multi_handle_timeout(ڈیٹا, pnow, &دھارا_error, &نتیجہ);
    }

statemachine_انتہا:
    اگر(!pnow)
      pnow = Curl_pgrs_now(ڈیٹا);
    نتیجہ = is_ختم_شدہ(multi, ڈیٹا, pnow, دھارا_error, نتیجہ);
    اگر(نتیجہ)
      mresult = CURLM_CALL_MULTI_PERFORM;

    اگر(MSTATE_COMPLETED == ڈیٹا->mstate) {
      handle_completed(multi, ڈیٹا, نتیجہ);
      mresult = CURLM_OK;
      جاؤ برآمد;
    }
  } جب((mresult == CURLM_CALL_MULTI_PERFORM) ||
          multi_ischanged(multi, FALSE));

برآمد:
  ڈیٹا->نتیجہ = نتیجہ;
  واپس mresult;
}

ساکن CURLMcode multi_perform(ساخت Curl_multi *multi,
                               صحیح *running_handles)
{
  CURLMcode returncode = CURLM_OK;
  ساخت curltime آغاز;
  uint32_t mid;
  ساخت Curl_sigpipe_ctx sigpipe_ctx;

  curlx_pnow(&آغاز);
  sigpipe_init(&sigpipe_ctx);

  اگر(Curl_uint32_bset_پہلا(&multi->process, &mid)) {
    CURL_TRC_M(multi->admin, "multi_perform(running=%u)",
               multi_xfers_running(multi));
    کرو {
      ساخت Curl_easy *ڈیٹا = Curl_multi_get_easy(multi, mid);
      CURLMcode mresult;
      اگر(!ڈیٹا) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        جاری;
      }
      mresult = multi_runsingle(multi, ڈیٹا, &sigpipe_ctx);
      اگر(mresult)
        returncode = mresult;
    } جب(Curl_uint32_bset_اگلا(&multi->process, mid, &mid));
  }
  sigpipe_restore(&sigpipe_ctx);

  اگر(multi_ischanged(multi, TRUE))
    multi_جدول_وقت_pending(multi);

  اگر(!returncode && CURL_MNTFY_HAS_ENTRIES(multi))
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
  جب(Curl_timeouts_remove_expired(&multi->timeouts, &آغاز, &mid)) {
    /* the removed may have another timeout in queue */
    ساخت Curl_easy *ڈیٹا = Curl_multi_get_easy(multi, mid);
    اگر(!ڈیٹا) {
      DEBUGASSERT(0);
      جاری;
    }
    (خالی)جوڑو_اگلا_timeout(&آغاز, multi, ڈیٹا);
    اگر(ڈیٹا->mstate == MSTATE_PENDING) {
      bool دھارا_غیر_مستعمل;
      CURLcode نتیجہ_غیر_مستعمل;
      اگر(multi_handle_timeout(ڈیٹا, Curl_pgrs_now(ڈیٹا),
                              &دھارا_غیر_مستعمل, &نتیجہ_غیر_مستعمل)) {
        infof(ڈیٹا, "PENDING handle timeout");
        move_pending_مقصد_connect(multi, ڈیٹا);
      }
    }
  }

  اگر(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < زیادہ_سے_زیادہ_عدد) ? (صحیح)running : زیادہ_سے_زیادہ_عدد;
  }

  اگر(CURLM_OK >= returncode)
    returncode = Curl_update_timer(multi);

  واپس returncode;
}

CURLMcode curl_multi_perform(CURLM *m, صحیح *running_handles)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_perform, &mresult)) {
    mresult = multi_perform(m, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

CURLMcode curl_multi_cleanup(CURLM *m)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_cleanup, &mresult)) {
    ساخت Curl_multi *multi = m;
    خالی *اندراج_2;
    uint32_t mid;

    /* First remove all remaining easy handles,
     * close internal ones. admin handle is special */
    اگر(Curl_uint32_tbl_پہلا(&multi->xfers, &mid, &اندراج_2)) {
      کرو {
        ساخت Curl_easy *ڈیٹا = اندراج_2;
        اگر(!GOOD_EASY_HANDLE(ڈیٹا)) {
          mresult = CURLM_BAD_HANDLE;
          جاؤ برآمد;
        }

#اگر_تعریف_شدہ DEBUGBUILD
        اگر(mid != ڈیٹا->mid) {
          CURL_TRC_M(ڈیٹا, "multi_cleanup: still present with mid=%u, "
                     "but unexpected data->mid=%u\n", mid, ڈیٹا->mid);
          DEBUGASSERT(0);
        }
#اگر_ختم

        اگر(ڈیٹا == multi->admin)
          جاری;

        اگر(!ڈیٹا->state.مکمل && ڈیٹا->conn)
          /* if DONE was never called for this handle */
          (خالی)multi_مکمل(ڈیٹا, CURLE_OK, TRUE);

        ڈیٹا->multi = تہی; /* clear the association */
        Curl_uint32_tbl_remove(&multi->xfers, mid);
        ڈیٹا->mid = UINT32_MAX;

#اگر_تعریف_شدہ USE_LIBPSL
        اگر(ڈیٹا->psl == &multi->psl)
          ڈیٹا->psl = تہی;
#اگر_ختم
        اگر(ڈیٹا->state.اندرونی_2)
          Curl_close(&ڈیٹا);
      } جب(Curl_uint32_tbl_اگلا(&multi->xfers, mid, &mid, &اندراج_2));
    }

#اگر_تعریف_شدہ USE_RESOLV_THREADED
    Curl_async_thrdd_multi_destroy(multi, !multi->quick_exit);
#اگر_ختم
    Curl_cpool_destroy(&multi->cpool, multi->admin);
    Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
    اگر(multi->admin) {
      CURL_TRC_M(multi->admin, "multi_cleanup, closing admin handle, done");
      multi->admin->multi = تہی;
      Curl_uint32_tbl_remove(&multi->xfers, multi->admin->mid);
      Curl_close(&multi->admin);
    }

    multi->magic = 0; /* not good anymore */

    Curl_multi_ev_cleanup(multi);
    Curl_hash_destroy(&multi->proto_hash);
    Curl_dnscache_destroy(&multi->dnscache);
    Curl_psl_destroy(&multi->psl);
#اگر_تعریف_شدہ USE_SSL
    Curl_ssl_scache_destroy(multi->ssl_scache);
#اگر_ختم

#اگر_تعریف_شدہ USE_WINSOCK
    WSACloseEvent(multi->wsa_event);
#اگر_ختم
#اگر_تعریف_شدہ ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#اگر_ختم
#اگر_تعریف_شدہ ENABLE_اندرونی_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_اندرونی);
#اگر_ختم

    multi_xfer_bufs_free(multi);
    Curl_mntfy_cleanup(multi);
#اگر_تعریف_شدہ DEBUGBUILD
    اگر(Curl_uint32_tbl_گنتی(&multi->xfers)) {
      multi_xfer_tbl_dump(multi);
      DEBUGASSERT(0);
    }
#اگر_ختم
    Curl_uint32_bset_destroy(&multi->process);
    Curl_uint32_bset_destroy(&multi->dirty);
    Curl_uint32_bset_destroy(&multi->pending);
    Curl_uint32_bset_destroy(&multi->msgsent);
    Curl_uint32_tbl_destroy(&multi->xfers);
    curlx_memzero(multi, حجم(*multi));
    curlx_free(multi);

    mresult = CURLM_OK;
  }
برآمد:
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

/*
 * curl_multi_info_read()
 *
 * This function is the primary way for a multi/multi_socket application to
 * figure out if a transfer has ended.
 */

CURLMsg *curl_multi_info_read(CURLM *m, صحیح *msgs_اندر_قطار)
{
  ساخت Curl_mapi_guard guard;
  CURLMsg *پیغام_نتیجہ = تہی;

  *msgs_اندر_قطار = 0; /* default to none */
  اگر(CURL_MAPI_ENTER(&guard, m, multi_info_read, تہی)) {
    ساخت Curl_multi *multi = m;
    uint32_t mid;
    اگر(Curl_uint32_bset_پہلا(&multi->msgsent, &mid)) {
      ساخت Curl_easy *ڈیٹا = Curl_multi_get_easy(multi, mid);

      DEBUGASSERT(ڈیٹا);
      Curl_uint32_bset_remove(&multi->msgsent, mid);
      *msgs_اندر_قطار =
        curlx_uztosi(Curl_uint32_bset_گنتی(&multi->msgsent));
      اگر(ڈیٹا)
        پیغام_نتیجہ = &ڈیٹا->پیغام;
    }
  }
  CURL_MAPI_LEAVE(&guard);
  واپس پیغام_نتیجہ;
}

خالی Curl_multi_will_close(ساخت Curl_easy *ڈیٹا, curl_socket_t s)
{
  اگر(ڈیٹا) {
    ساخت Curl_multi *multi = ڈیٹا->multi;
    اگر(multi) {
      CURL_TRC_M(ڈیٹا, "Curl_multi_will_close fd=%" FMT_SOCKET_T, s);
      Curl_multi_ev_socket_مکمل(multi, ڈیٹا, s);
    }
  }
}

ساکن خالی multi_timeouts_init(ساخت Curl_easy *ڈیٹا)
{
  ڈیٹا->state.timeouts.پہلا = EXPIRE_آخری;
  ڈیٹا->state.timeouts.splaynode.registered = FALSE;
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
ساکن CURLMcode جوڑو_اگلا_timeout(ثابت ساخت curltime *pnow,
                                  ساخت Curl_multi *multi,
                                  ساخت Curl_easy *ڈیٹا)
{
  ساخت expire_timers *timeouts = &ڈیٹا->state.timeouts;
  timediff_t now_us = Curl_timeouts_انحراف_us(&multi->timeouts, pnow);

  جب(timeouts->پہلا < EXPIRE_آخری) {
    اگر(timeouts->انحراف_us[timeouts->پہلا] <= now_us)  /* already expired */
      timeouts->پہلا = timeouts->اگلا[timeouts->پہلا];
    ورنہ /* timeouts are sorted, first is first in the future now */
      توڑو;
  }

  اگر(timeouts->پہلا < EXPIRE_آخری) {
    /* Insert this node again into the splay. Keep the timer in the list in
       case we need to recompute future timers. */
    Curl_timeouts_جوڑو(&multi->timeouts, ڈیٹا,
                      timeouts->انحراف_us[timeouts->پہلا]);
  }
  واپس CURLM_OK;
}

ساکن خالی multi_mark_expired_as_dirty(ساخت Curl_multi *multi,
                                        ثابت ساخت curltime *ts)
{
  ساخت Curl_easy *ڈیٹا = تہی;
  uint32_t mid;

  /*
   * The loop following here will go on as long as there are expire-times left
   * to process (compared to `ts`) in the splay and 'data' will be
   * re-assigned for every expired handle we deal with.
   */
  جب(Curl_timeouts_remove_expired(&multi->timeouts, ts, &mid)) {
    /* Check if there is one (more) expired timer to deal with! This function
       extracts a matching node if there is one */
    ڈیٹا = Curl_multi_get_easy(multi, mid);
    اگر(!ڈیٹا) {
      DEBUGASSERT(0);
      جاری;
    }
#اگر_تعریف_شدہ CURLVERBOSE
    اگر(CURL_TRC_TIMER_is_verbose(ڈیٹا)) {
      اگر(ڈیٹا->state.timeouts.پہلا < EXPIRE_آخری) {
        CURL_TRC_TIMER(ڈیٹا, ڈیٹا->state.timeouts.پہلا, "has expired");
      }
    }
#اگر_ختم
    (خالی)جوڑو_اگلا_timeout(ts, multi, ڈیٹا);
    Curl_multi_mark_dirty(ڈیٹا);
  }
}

ساکن CURLMcode multi_تسلسل_dirty(ساخت Curl_multi *multi,
                                 ساخت Curl_sigpipe_ctx *sigpipe_ctx,
                                 uint32_t *pnum)
{
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;

  *pnum = 0;
  اگر(Curl_uint32_bset_پہلا(&multi->dirty, &mid)) {
    کرو {
      ساخت Curl_easy *ڈیٹا = Curl_multi_get_easy(multi, mid);
      اگر(ڈیٹا) {
        CURL_TRC_M(ڈیٹا, "multi_run_dirty");

        اگر(!Curl_uint32_bset_شامل_ہے(&multi->process, mid)) {
          اگر(Curl_uint32_bset_شامل_ہے(&multi->pending, mid)) {
            /* Something happened on the pending transfer, act on it. */
            move_pending_مقصد_connect(multi, ڈیٹا);
          }
          ورنہ {
            /* We are no longer processing this transfer */
            Curl_uint32_bset_remove(&multi->dirty, mid);
            جاری;
          }
        }

        (*pnum)++;
        /* runsingle() clears the dirty mid */
        mresult = multi_runsingle(multi, ڈیٹا, sigpipe_ctx);

        اگر(CURLM_OK >= mresult) {
          /* reassess event handling of data */
          mresult = Curl_multi_ev_assess_xfer(multi, ڈیٹا);
          اگر(mresult)
            جاؤ برآمد;
        }
      }
      ورنہ {
        CURL_TRC_M(multi->admin, "multi_run_dirty, %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } جب(Curl_uint32_bset_اگلا(&multi->dirty, mid, &mid));
  }

برآمد:
  واپس mresult;
}

ساکن CURLMcode multi_socket(ساخت Curl_multi *multi,
                              bool checkall,
                              curl_socket_t s,
                              صحیح ev_bitmask,
                              صحیح *running_handles)
{
  CURLMcode mresult = CURLM_OK;
  ساخت Curl_sigpipe_ctx pipe_ctx;
  ساخت curltime now;
  uint32_t تسلسل_xfers;

  (خالی)ev_bitmask;
  sigpipe_init(&pipe_ctx);

  اگر(checkall) {
    /* *perform() deals with running_handles on its own */
    mresult = multi_perform(multi, running_handles);

    اگر(mresult != CURLM_BAD_HANDLE) {
      /* Reassess event status of all active transfers, but keep any error
         from multi_perform() */
      CURLMcode mresult2 =
        Curl_multi_ev_assess_xfer_bset(multi, &multi->process);
      اگر(!mresult)
        mresult = mresult2;
    }
    جاؤ برآمد;
  }

  اگر(s != CURL_SOCKET_TIMEOUT) {
    /* Mark all transfers of that socket as dirty */
    Curl_multi_ev_dirty_xfers(multi, s);
  }
  ورنہ {
    /* Asked to run due to time-out. Clear the 'last_expire_ts' variable to
       force Curl_update_timer() to trigger a callback to the app again even
       if the same timeout is still the one to run after this call. That
       handles the case when the application asks libcurl to run the timeout
       prematurely. */
    multi->آخری_expire_انحراف_us = 0;

    /* Applications may set `socket_cb` *after* having added transfers
     * first. *Then* kick off processing with a
     * curl_multi_socket_action(TIMEOUT) afterwards. Make sure our
     * admin handle registers its pollset with the callbacks present. */
    mresult = multi_assess_wakeup(multi);
    اگر(mresult)
      جاؤ برآمد;
  }

  curlx_pnow(&now);
  multi_mark_expired_as_dirty(multi, &now);
  mresult = multi_تسلسل_dirty(multi, &pipe_ctx, &تسلسل_xfers);
  اگر(mresult)
    جاؤ برآمد;

  اگر(تسلسل_xfers) {
    /* Running transfers takes time. With a new timestamp, we might catch
     * other expires which are due now. Instead of telling the application
     * to set a 0 timeout and call us again, we run them here.
     * Do that only once or it might be unfair to transfers on other
     * sockets. */
    curlx_pnow(&now);
    multi_mark_expired_as_dirty(multi, &now);
    mresult = multi_تسلسل_dirty(multi, &pipe_ctx, &تسلسل_xfers);
  }

برآمد:
  sigpipe_restore(&pipe_ctx);

  اگر(multi_ischanged(multi, TRUE))
    multi_جدول_وقت_pending(multi);

  اگر(!mresult && CURL_MNTFY_HAS_ENTRIES(multi))
    mresult = Curl_mntfy_dispatch_all(multi);

  اگر(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < زیادہ_سے_زیادہ_عدد) ? (صحیح)running : زیادہ_سے_زیادہ_عدد;
  }

  اگر(CURLM_OK >= mresult)
    mresult = Curl_update_timer(multi);
  واپس mresult;
}

#تعریف_منسوخ curl_multi_setopt
CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_setopt, &mresult)) {
    ساخت Curl_multi *multi = m;
    va_list param;
    غیرعلامتی طویل uarg;
    size_t szarg;

    va_start(param, option);

    انتخاب(option) {
    حالت CURLMOPT_SOCKETFUNCTION:
      multi->socket_cb = va_arg(param, curl_socket_callback);
      توڑو;
    حالت CURLMOPT_SOCKETDATA:
      multi->socket_userp = va_arg(param, خالی *);
      توڑو;
    حالت CURLMOPT_PUSHFUNCTION:
      multi->ڈالو_cb = va_arg(param, curl_ڈالو_callback);
      توڑو;
    حالت CURLMOPT_PUSHDATA:
      multi->ڈالو_userp = va_arg(param, خالی *);
      توڑو;
    حالت CURLMOPT_PIPELINING:
      multi->multiplexing = va_arg(param, طویل) & CURLPIPE_MULTIPLEX ? 1 : 0;
      توڑو;
    حالت CURLMOPT_TIMERFUNCTION:
      multi->timer_cb = va_arg(param, curl_multi_timer_callback);
      توڑو;
    حالت CURLMOPT_TIMERDATA:
      multi->timer_userp = va_arg(param, خالی *);
      توڑو;
    حالت CURLMOPT_MAXCONNECTS:
      uarg = va_arg(param, غیرعلامتی طویل);
      اگر(uarg <= UINT32_MAX)
        multi->maxconnects = (uint32_t)uarg;
      توڑو;
    حالت CURLMOPT_MAX_HOST_CONNECTIONS:
      اگر(!curlx_sltouz(va_arg(param, طویل), &szarg))
        mresult = CURLM_BAD_FUNCTION_دلیل;
      multi->max_host_connections = (szarg < UINT32_MAX) ?
                                    (uint32_t)szarg : UINT32_MAX;
      توڑو;
    حالت CURLMOPT_MAX_کل_CONNECTIONS:
      اگر(!curlx_sltouz(va_arg(param, طویل), &szarg))
        mresult = CURLM_BAD_FUNCTION_دلیل;
      multi->max_کل_connections = (szarg < UINT32_MAX) ?
                                     (uint32_t)szarg : UINT32_MAX;
      توڑو;
      /* options formerly used for pipelining */
    حالت CURLMOPT_MAX_PIPELINE_لمبائی:
      توڑو;
    حالت CURLMOPT_CONTENT_لمبائی_PENALTY_سائز:
      توڑو;
    حالت CURLMOPT_CHUNK_لمبائی_PENALTY_سائز:
      توڑو;
    حالت CURLMOPT_PIPELINING_SITE_BL:
      توڑو;
    حالت CURLMOPT_PIPELINING_SERVER_BL:
      توڑو;
    حالت CURLMOPT_MAX_CONCURRENT_STREAMS: {
      اگر(!curlx_sltouz(va_arg(param, طویل), &szarg) ||
         !szarg || (szarg > (size_t)زیادہ_سے_زیادہ_عدد)) /* preserve previous cutoff */
        multi->max_concurrent_streams = 100;
      ورنہ
        multi->max_concurrent_streams = (szarg < UINT32_MAX) ?
                                       (uint32_t)szarg : UINT32_MAX;
      توڑو;
    }
    حالت CURLMOPT_جال_بدلا: {
      طویل val = va_arg(param, طویل);
      اگر(val & CURLMNWC_CLEAR_ALL)
        /* In the beginning, all values available to set were 1 by mistake. We
           converted this to mean "all", thus setting all the bits
           automatically */
        val = CURLMNWC_CLEAR_DNS | CURLMNWC_CLEAR_CONNS;
      اگر(val & CURLMNWC_CLEAR_DNS) {
        Curl_dnscache_clear(multi->admin);
      }
      اگر(val & CURLMNWC_CLEAR_CONNS) {
        Curl_cpool_nw_بدلا(&multi->cpool, multi->admin);
      }
      توڑو;
    }
    حالت CURLMOPT_NOTIFYFUNCTION:
      multi->ntfy.ntfy_cb = va_arg(param, curl_notify_callback);
      توڑو;
    حالت CURLMOPT_NOTIFYDATA:
      multi->ntfy.ntfy_cb_ڈیٹا = va_arg(param, خالی *);
      توڑو;
    حالت CURLMOPT_RESOLVE_دھاگے_MAX:
#اگر_تعریف_شدہ USE_RESOLV_THREADED
      uarg = va_arg(param, طویل);
      اگر((uarg <= 0) || (uarg > UINT32_MAX))
        mresult = CURLM_BAD_FUNCTION_دلیل;
      ورنہ {
        CURLcode نتیجہ = Curl_async_thrdd_multi_سیٹ_props(
          multi, 0, (uint32_t)uarg, 2000);
        انتخاب(نتیجہ) {
        حالت CURLE_OK:
          mresult = CURLM_OK;
          توڑو;
        حالت CURLE_BAD_FUNCTION_دلیل:
          mresult = CURLM_BAD_FUNCTION_دلیل;
          توڑو;
        حالت CURLE_برآمد_کا_حافظہ:
          mresult = CURLM_برآمد_کا_حافظہ;
          توڑو;
        طےشدہ:
          mresult = CURLM_اندرونی_ERROR;
          توڑو;
        }
      }
#اگر_ختم
      توڑو;
    حالت CURLMOPT_تیز_EXIT:
      multi->quick_exit = va_arg(param, طویل) ? 1 : 0;
      توڑو;
    طےشدہ:
      mresult = CURLM_UNKNOWN_OPTION;
      توڑو;
    }
    va_end(param);
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

/* we define curl_multi_socket() in the public multi.h header */
#تعریف_منسوخ curl_multi_socket

CURLMcode curl_multi_socket(CURLM *m, curl_socket_t s, صحیح *running_handles)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_socket, &mresult)) {
    mresult = multi_socket(m, FALSE, s, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

CURLMcode curl_multi_socket_action(CURLM *m, curl_socket_t s,
                                   صحیح ev_bitmask, صحیح *running_handles)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_socket_action, &mresult)) {
    mresult = multi_socket(m, FALSE, s, ev_bitmask, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

CURLMcode curl_multi_socket_all(CURLM *m, صحیح *running_handles)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_socket_all, &mresult)) {
    mresult = multi_socket(m, TRUE, CURL_SOCKET_BAD, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

ساکن bool multi_has_dirties(ساخت Curl_multi *multi)
{
  uint32_t mid;
  اگر(Curl_uint32_bset_پہلا(&multi->dirty, &mid)) {
    کرو {
      ساخت Curl_easy *ڈیٹا = Curl_multi_get_easy(multi, mid);
      اگر(ڈیٹا) {
        اگر(Curl_uint32_bset_شامل_ہے(&multi->process, mid))
          واپس TRUE;
        /* We are no longer processing this transfer */
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
      ورنہ {
        CURL_TRC_M(multi->admin, "dirty transfer %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } جب(Curl_uint32_bset_اگلا(&multi->dirty, mid, &mid));
  }
  واپس FALSE;
}

ساکن خالی multi_timeout(ساخت Curl_multi *multi,
                          timediff_t *pexire_انحراف_us,
                          صحیح *timeout_ms)
{
  اگر(multi->dead) {
    اگر(pexire_انحراف_us)
      *pexire_انحراف_us = 0;
    *timeout_ms = 0;
    واپس;
  }

  اگر(multi_has_dirties(multi)) {
    اگر(pexire_انحراف_us) {
      ساخت curltime now;
      curlx_pnow(&now);
      *pexire_انحراف_us = Curl_timeouts_انحراف_us(&multi->timeouts, &now);
    }
    *timeout_ms = 0;
    واپس;
  }
  ورنہ {
    ساخت curltime now;
    uint32_t mid;

    curlx_pnow(&now);
    *timeout_ms = Curl_timeouts_اگلا_ms(&multi->timeouts, &now,
                                        pexire_انحراف_us, &mid);
#اگر_تعریف_شدہ CURLVERBOSE
    اگر(mid != UINT32_MAX) {
      ساخت Curl_easy *ڈیٹا = Curl_multi_get_easy(multi, mid);
      اگر(ڈیٹا && CURL_TRC_TIMER_is_verbose(ڈیٹا) &&
         (ڈیٹا->state.timeouts.پہلا < EXPIRE_آخری)) {
        CURL_TRC_TIMER(ڈیٹا, ڈیٹا->state.timeouts.پہلا,
                       "gives multi timeout in %dms", *timeout_ms);
      }
    }
#اگر_ختم
  }
}

CURLMcode curl_multi_timeout(CURLM *m,
                             طویل *timeout_ms)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_timeout, &mresult)) {
    صحیح itimeout_ms;

    multi_timeout(m, تہی, &itimeout_ms);
    *timeout_ms = (طویل)itimeout_ms;
    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

/*
 * Tell the application it should update its timers, if it subscribes to the
 * update timer callback.
 */
CURLMcode Curl_update_timer(ساخت Curl_multi *multi)
{
  timediff_t timeouts_انحراف_us = 0;
  صحیح timeout_ms;
  صحیح rc;
  bool سیٹ_قدر = FALSE;

  اگر(!multi->timer_cb || multi->dead)
    واپس CURLM_OK;
  multi_timeout(multi, &timeouts_انحراف_us, &timeout_ms);

  اگر(timeout_ms < 0 && !multi->آخری_timeout_سیٹ) {
    /* nothing to do */
  }
  ورنہ اگر(timeout_ms < 0) {
    /* there is no timeout now but there was one previously */
    CURL_TRC_M(multi->admin, "[TIMER] clear");
    timeout_ms = -1; /* normalize */
    سیٹ_قدر = TRUE;
  }
  ورنہ اگر(!multi->آخری_timeout_سیٹ) {
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, none before", timeout_ms);
    سیٹ_قدر = TRUE;
  }
  ورنہ اگر(multi->آخری_expire_انحراف_us != timeouts_انحراف_us) {
    /* We had a timeout before and have one now, the absolute timestamp
     * differs. The relative timeout_ms may be the same, but the starting
     * point differs. Let the application restart its timer. */
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, replace previous",
               timeout_ms);
    سیٹ_قدر = TRUE;
  }
  ورنہ {
    /* We have same expire time as previously. Our relative 'timeout_ms'
     * may be different now, but the application has the timer running
     * and we do not to tell it to start this again. */
  }

  اگر(سیٹ_قدر) {
    ساخت Curl_mapi_guard guard;

    multi->آخری_expire_انحراف_us = timeouts_انحراف_us;
    multi->آخری_timeout_سیٹ = timeout_ms >= 0;
    CURL_CBAPI_MULTI_آغاز(&guard, multi, multi_timer_cb);
    rc = multi->timer_cb(multi, timeout_ms, multi->timer_userp);
    CURL_CBAPI_MULTI_انتہا(&guard);
    اگر(rc == -1) {
      multi->dead = TRUE;
      واپس CURLM_ABORTED_BY_CALLBACK;
    }
  }
  واپس CURLM_OK;
}

#اگر_تعریف_شدہ DEBUGBUILD
ساکن bool multi_timeouts_check(ساخت Curl_easy *ڈیٹا)
{
  ساخت expire_timers *timeouts = &ڈیٹا->state.timeouts;
  uint8_t id;
  صحیح i = 0;
  برائے(id = timeouts->پہلا; id < EXPIRE_آخری; id = timeouts->اگلا[id]) {
    اگر(++i >= EXPIRE_آخری) {
      failf(ڈیٹا, "expire timeouts looped: %d iterations and no end", i);
      واپس FALSE;
    }
    اگر(id == timeouts->اگلا[id]) {
      failf(ڈیٹا, "expire timeouts wrong: %d points to itself", (صحیح)id);
      واپس FALSE;
    }
    اگر((timeouts->اگلا[id] < EXPIRE_آخری) &&
       (timeouts->انحراف_us[id] > timeouts->انحراف_us[timeouts->اگلا[id]])) {
      failf(ڈیٹا, "expire timeouts not sorted: %d happens after %d but "
            "is listed before", (صحیح)id, (صحیح)timeouts->اگلا[id]);
      واپس FALSE;
    }
  }
  واپس TRUE;
}
#اگر_ختم

/*
 * Remove a given timestamp from the list of timeouts.
 */
ساکن خالی multi_clear_timeout(ساخت Curl_easy *ڈیٹا, expire_id eid)
{
  ساخت expire_timers *timeouts = &ڈیٹا->state.timeouts;
  uint8_t orig_پہلا = timeouts->پہلا;
  uint8_t *anchor = &timeouts->پہلا;
  uint8_t id = (uint8_t)eid;

  اگر((غیرعلامتی)eid >= EXPIRE_آخری) {
    DEBUGASSERT(0);
    واپس;
  }

  جب(*anchor < EXPIRE_آخری) {
    اگر(*anchor == id) {
      *anchor = timeouts->اگلا[id];
      توڑو;
    }
    anchor = &timeouts->اگلا[*anchor];
  }
  DEBUGASSERT(multi_timeouts_check(ڈیٹا));
  اگر(Curl_timeouts_has(ڈیٹا)) {
    ساخت Curl_multi *multi = ڈیٹا->multi;

    اگر(!multi) {
      DEBUGASSERT(0);
      واپس;
    }
    اگر((timeouts->پہلا >= EXPIRE_آخری) || /* no more timeouts */
       (timeouts->پہلا != orig_پہلا)) {  /* active timeout changed */
      Curl_timeouts_remove(&multi->timeouts, ڈیٹا);
    }
    اگر((timeouts->پہلا < EXPIRE_آخری) && !Curl_timeouts_has(ڈیٹا)) {
      Curl_timeouts_جوڑو(&multi->timeouts, ڈیٹا,
                        timeouts->انحراف_us[timeouts->پہلا]);
    }
  }
}

/*
 * Add a timestamp to the list of timeouts. Keep the list sorted so that head
 * of list is always the timeout nearest in time.
 */
ساکن CURLMcode multi_سیٹ_timeout(ساخت Curl_easy *ڈیٹا,
                                   ثابت ساخت curltime *stamp,
                                   expire_id eid)
{
  ساخت expire_timers *timeouts = &ڈیٹا->state.timeouts;
  uint8_t *anchor = &timeouts->پہلا;
  uint8_t id = (uint8_t)eid;

  اگر((غیرعلامتی)eid >= EXPIRE_آخری) {
    DEBUGASSERT(0);
    واپس CURLM_BAD_FUNCTION_دلیل;
  }
  /* remove from list, store time and re-insert */
  multi_clear_timeout(ڈیٹا, eid);
  timeouts->انحراف_us[id] =
    Curl_timeouts_انحراف_us(&ڈیٹا->multi->timeouts, stamp);

  جب(*anchor < EXPIRE_آخری) {
    اگر(timeouts->انحراف_us[*anchor] > timeouts->انحراف_us[id])
      توڑو;
    anchor = &timeouts->اگلا[*anchor];
  }
  timeouts->اگلا[eid] = *anchor;
  timeouts->اگلا[eid] = *anchor;
  *anchor = id;
  DEBUGASSERT(multi_timeouts_check(ڈیٹا));
  CURL_TRC_TIMER(ڈیٹا, eid, "set for %" FMT_TIMEDIFF_T "us",
                 curlx_ptimediff_us(stamp, Curl_pgrs_now(ڈیٹا)));
  واپس CURLM_OK;
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
خالی Curl_expire_سیٹ(ساخت Curl_easy *ڈیٹا,
                     expire_id eid, timediff_t ms,
                     ثابت ساخت curltime *pnow)
{
  ساخت Curl_multi *multi = ڈیٹا->multi;
  ساخت expire_timers *timeouts = &ڈیٹا->state.timeouts;
  uint8_t پچھلا_id = timeouts->پہلا;
  ساخت curltime سیٹ;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  اگر(!multi)
    واپس;
  DEBUGASSERT(eid < EXPIRE_آخری);
  اگر(ms > زیادہ_سے_زیادہ_عدد)
    /* Cap ridiculous timeouts, 31-bit ms is still 3.5 weeks. When the time
       goes to the user, it must fit in this size. */
    ms = زیادہ_سے_زیادہ_عدد;

  سیٹ = *pnow;
  سیٹ.tv_sec += (time_t)(ms / 1000); /* may be a 64 to 32-bit conversion */
  سیٹ.tv_usec += (صحیح)(ms % 1000) * 1000;
  اگر(سیٹ.tv_usec >= 1000000) {
    سیٹ.tv_sec++;
    سیٹ.tv_usec -= 1000000;
  }

  /* Add the timeout, will replace any previous value for this timer. */
  multi_سیٹ_timeout(ڈیٹا, &سیٹ, eid);
  DEBUGASSERT(timeouts->پہلا < EXPIRE_آخری);

  اگر(Curl_timeouts_has(ڈیٹا)) {
    /* data has already a timeout registered. If the first timer
     * was NOT the one we just set AND is still the first one,
     * nothing changed from the timeouts point of view. The
     * set timer triggers after the one already registered. Leave. */
    اگر((پچھلا_id != eid) && (پچھلا_id == timeouts->پہلا))
      واپس;

    /* Since this is an updated time, we must remove data from
     * timeouts and then add it again. */
    Curl_timeouts_remove(&multi->timeouts, ڈیٹا);
  }

  /* Insert the new timer expiry since it is our local minimum. */
  Curl_timeouts_جوڑو(&multi->timeouts, ڈیٹا,
                    timeouts->انحراف_us[timeouts->پہلا]);
}

خالی Curl_expire(ساخت Curl_easy *ڈیٹا,
                 timediff_t milli, expire_id eid)
{
  Curl_expire_سیٹ(ڈیٹا, eid, milli, Curl_pgrs_now(ڈیٹا));
}

/*
 * Removes the expire timer. Marks it as done.
 */
خالی Curl_expire_clear(ساخت Curl_easy *ڈیٹا, expire_id eid)
{
  /* remove the timer, if there */
  multi_clear_timeout(ڈیٹا, eid);
  CURL_TRC_TIMER(ڈیٹا, eid, "cleared");
}

/*
 * Clear ALL timeout values for this handle.
 */
خالی Curl_expire_clear_all(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_multi *multi = ڈیٹا->multi;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  اگر(!multi)
    واپس;

  اگر(Curl_timeouts_remove(&multi->timeouts, ڈیٹا)) {
    /* Since this is an cleared time, we must remove the previous entry from
       the splay tree */
    multi_timeouts_init(ڈیٹا);

    اگر(ڈیٹا->id >= 0)
      CURL_TRC_M(ڈیٹا, "[TIMEOUT] all cleared");
  }
}

CURLMcode curl_multi_تفویض(CURLM *m, curl_socket_t sockfd,
                            خالی *sockp)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_تفویض, &mresult)) {
    mresult = Curl_multi_ev_تفویض(m, sockfd, sockp);
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

ساکن خالی move_pending_مقصد_connect(ساخت Curl_multi *multi,
                                    ساخت Curl_easy *ڈیٹا)
{
  DEBUGASSERT(ڈیٹا->mstate == MSTATE_PENDING);

  /* Remove this node from the pending set, add into process set */
  Curl_uint32_bset_remove(&multi->pending, ڈیٹا->mid);
  Curl_uint32_bset_جوڑو(&multi->process, ڈیٹا->mid);

  multistate(ڈیٹا, MSTATE_CONNECT);
  Curl_multi_mark_dirty(ڈیٹا); /* make it run */
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
ساکن خالی multi_جدول_وقت_pending(ساخت Curl_multi *multi)
{
  uint32_t mid = multi->آخری_pending_mid;

  اگر(mid) {
    جب(Curl_uint32_bset_اگلا(&multi->pending, mid, &mid)) {
      ساخت Curl_easy *ڈیٹا = Curl_multi_get_easy(multi, mid);
      اگر(ڈیٹا) {
        move_pending_مقصد_connect(multi, ڈیٹا);
        multi->آخری_pending_mid = mid;
        واپس;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    }
    /* found no pending transfers with `mid` larger than `last_pending_mid`.
     * Start at the beginning of the pending set again. */
    multi->آخری_pending_mid = 0;
  }

  اگر(Curl_uint32_bset_پہلا(&multi->pending, &mid)) {
    کرو {
      ساخت Curl_easy *ڈیٹا = Curl_multi_get_easy(multi, mid);
      اگر(ڈیٹا) {
        move_pending_مقصد_connect(multi, ڈیٹا);
        multi->آخری_pending_mid = mid;
        واپس;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    } جب(Curl_uint32_bset_اگلا(&multi->pending, mid, &mid));
  }
}

uint32_t Curl_multi_max_concurrent_streams(ساخت Curl_multi *multi)
{
  DEBUGASSERT(multi);
  واپس multi->max_concurrent_streams;
}

CURL **curl_multi_get_handles(CURLM *m)
{
  ساخت Curl_mapi_guard guard;
  CURL **a = تہی;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_get_handles, تہی)) {
    ساخت Curl_multi *multi = m;
    خالی *اندراج_2;
    size_t گنتی = Curl_uint32_tbl_گنتی(&multi->xfers);

    a = curlx_malloc(حجم(ساخت Curl_easy *) * (گنتی + 1));
    اگر(a) {
      غیرعلامتی صحیح i = 0;
      uint32_t mid;

      اگر(Curl_uint32_tbl_پہلا(&multi->xfers, &mid, &اندراج_2)) {
        کرو {
          ساخت Curl_easy *ڈیٹا = اندراج_2;
          DEBUGASSERT(i < گنتی);
          اگر(!ڈیٹا->state.اندرونی_2)
            a[i++] = ڈیٹا;
        } جب(Curl_uint32_tbl_اگلا(&multi->xfers, mid, &mid, &اندراج_2));
      }
      a[i] = تہی; /* last entry is a NULL */
    }
  }
  CURL_MAPI_LEAVE(&guard);
  واپس a;
}

CURLMcode curl_multi_get_offt(CURLM *m,
                              CURLMinfo_offt info,
                              curl_off_t *pvalue)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_get_offt, &mresult)) {
    ساخت Curl_multi *multi = m;
    uint32_t n;

    اگر(!pvalue) {
      mresult = CURLM_BAD_FUNCTION_دلیل;
      جاؤ برآمد;
    }

    انتخاب(info) {
    حالت CURLMINFO_XFERS_CURRENT:
      n = Curl_uint32_tbl_گنتی(&multi->xfers);
      اگر(n && multi->admin)
        --n;
      *pvalue = (curl_off_t)n;
      توڑو;
    حالت CURLMINFO_XFERS_RUNNING:
      n = Curl_uint32_bset_گنتی(&multi->process);
      اگر(n && Curl_uint32_bset_شامل_ہے(&multi->process, multi->admin->mid))
        --n;
      *pvalue = (curl_off_t)n;
      توڑو;
    حالت CURLMINFO_XFERS_PENDING:
      *pvalue = (curl_off_t)Curl_uint32_bset_گنتی(&multi->pending);
      توڑو;
    حالت CURLMINFO_XFERS_مکمل:
      *pvalue = (curl_off_t)Curl_uint32_bset_گنتی(&multi->msgsent);
      توڑو;
    حالت CURLMINFO_XFERS_ADDED:
      *pvalue = multi->xfers_کل_ever;
      توڑو;
    طےشدہ:
      *pvalue = -1;
      mresult = CURLM_UNKNOWN_OPTION;
      توڑو;
    }
  }
برآمد:
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

ساکن ساخت Curl_fixed_buf *fixed_buf_create(size_t len)
{
  ساخت Curl_fixed_buf *fbuf;
  اگر((SIZE_MAX - حجم(*fbuf)) < len)
    واپس تہی; /* too large */
  fbuf = curlx_malloc(len + حجم(*fbuf));
  اگر(fbuf)
    fbuf->len = len;
  واپس fbuf;
}

CURLcode Curl_multi_xfer_buf_borrow(ساخت Curl_easy *ڈیٹا,
                                    حرف **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->multi);
  *pbuf = تہی;
  *pbuflen = 0;
  اگر(!ڈیٹا->multi) {
    failf(ڈیٹا, "transfer has no multi handle");
    واپس CURLE_FAILED_INIT;
  }
  اگر(!ڈیٹا->سیٹ.بفر_سائز) {
    failf(ڈیٹا, "transfer buffer size is 0");
    واپس CURLE_FAILED_INIT;
  }
  اگر(ڈیٹا->multi->xfer_buf_borrowed) {
    failf(ڈیٹا, "attempt to borrow xfer_buf when already borrowed");
    واپس CURLE_AGAIN;
  }

  اگر(ڈیٹا->multi->xfer_buf &&
     ڈیٹا->سیٹ.بفر_سائز > ڈیٹا->multi->xfer_buf->len) {
    /* not large enough, get a new one */
    curlx_safefree(ڈیٹا->multi->xfer_buf);
  }

  اگر(!ڈیٹا->multi->xfer_buf) {
    ڈیٹا->multi->xfer_buf =
      fixed_buf_create(curlx_uitouz(ڈیٹا->سیٹ.بفر_سائز));
    اگر(!ڈیٹا->multi->xfer_buf) {
      failf(ڈیٹا, "could not allocate xfer_buf of %u bytes",
            ڈیٹا->سیٹ.بفر_سائز);
      واپس CURLE_برآمد_کا_حافظہ;
    }
  }

  ڈیٹا->multi->xfer_buf_borrowed = TRUE;
  *pbuf = ڈیٹا->multi->xfer_buf->ڈیٹا;
  *pbuflen = ڈیٹا->multi->xfer_buf->len;
  واپس CURLE_OK;
}

خالی Curl_multi_xfer_buf_release(ساخت Curl_easy *ڈیٹا, حرف *buf)
{
  (خالی)buf;
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->multi);
  DEBUGASSERT(!buf || (ڈیٹا->multi->xfer_buf &&
                       ڈیٹا->multi->xfer_buf->ڈیٹا == buf));
  ڈیٹا->multi->xfer_buf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_ulbuf_borrow(ساخت Curl_easy *ڈیٹا,
                                      حرف **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->multi);
  *pbuf = تہی;
  *pbuflen = 0;
  اگر(!ڈیٹا->multi) {
    failf(ڈیٹا, "transfer has no multi handle");
    واپس CURLE_FAILED_INIT;
  }
  اگر(ڈیٹا->multi->xfer_ulbuf_borrowed) {
    failf(ڈیٹا, "attempt to borrow xfer_ulbuf when already borrowed");
    واپس CURLE_AGAIN;
  }

  اگر(ڈیٹا->multi->xfer_ulbuf &&
     ڈیٹا->سیٹ.upload_بفر_سائز > ڈیٹا->multi->xfer_ulbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(ڈیٹا->multi->xfer_ulbuf);
  }

  اگر(!ڈیٹا->multi->xfer_ulbuf) {
    ڈیٹا->multi->xfer_ulbuf =
      fixed_buf_create(curlx_uitouz(ڈیٹا->سیٹ.upload_بفر_سائز));
    اگر(!ڈیٹا->multi->xfer_ulbuf) {
      failf(ڈیٹا, "could not allocate xfer_ulbuf of %u bytes",
            ڈیٹا->سیٹ.upload_بفر_سائز);
      واپس CURLE_برآمد_کا_حافظہ;
    }
  }

  ڈیٹا->multi->xfer_ulbuf_borrowed = TRUE;
  *pbuf = ڈیٹا->multi->xfer_ulbuf->ڈیٹا;
  *pbuflen = ڈیٹا->multi->xfer_ulbuf->len;
  واپس CURLE_OK;
}

خالی Curl_multi_xfer_ulbuf_release(ساخت Curl_easy *ڈیٹا, حرف *buf)
{
  (خالی)buf;
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->multi);
  DEBUGASSERT(!buf || (ڈیٹا->multi->xfer_ulbuf &&
                       ڈیٹا->multi->xfer_ulbuf->ڈیٹا == buf));
  ڈیٹا->multi->xfer_ulbuf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_sockbuf_borrow(ساخت Curl_easy *ڈیٹا,
                                        size_t blen, حرف **pbuf)
{
  DEBUGASSERT(ڈیٹا);
  *pbuf = تہی;
  اگر(!ڈیٹا->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    *pbuf = curlx_malloc(blen);
    واپس *pbuf ? CURLE_OK : CURLE_برآمد_کا_حافظہ;
  }
  اگر(ڈیٹا->multi->xfer_sockbuf_borrowed) {
    failf(ڈیٹا, "attempt to borrow xfer_sockbuf when already borrowed");
    واپس CURLE_AGAIN;
  }

  اگر(ڈیٹا->multi->xfer_sockbuf && blen > ڈیٹا->multi->xfer_sockbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(ڈیٹا->multi->xfer_sockbuf);
  }

  اگر(!ڈیٹا->multi->xfer_sockbuf) {
    ڈیٹا->multi->xfer_sockbuf = fixed_buf_create(blen);
    اگر(!ڈیٹا->multi->xfer_sockbuf) {
      failf(ڈیٹا, "could not allocate xfer_sockbuf of %zu bytes", blen);
      واپس CURLE_برآمد_کا_حافظہ;
    }
  }

  ڈیٹا->multi->xfer_sockbuf_borrowed = TRUE;
  *pbuf = ڈیٹا->multi->xfer_sockbuf->ڈیٹا;
  واپس CURLE_OK;
}

خالی Curl_multi_xfer_sockbuf_release(ساخت Curl_easy *ڈیٹا, حرف *buf)
{
  DEBUGASSERT(ڈیٹا);
  اگر(!ڈیٹا->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    curlx_free(buf);
  }
  ورنہ {
    DEBUGASSERT(!buf || (ڈیٹا->multi->xfer_sockbuf &&
                         ڈیٹا->multi->xfer_sockbuf->ڈیٹا == buf));
    ڈیٹا->multi->xfer_sockbuf_borrowed = FALSE;
  }
}

ساکن خالی multi_xfer_bufs_free(ساخت Curl_multi *multi)
{
  DEBUGASSERT(multi);
  curlx_safefree(multi->xfer_buf);
  multi->xfer_buf_borrowed = FALSE;
  curlx_safefree(multi->xfer_ulbuf);
  multi->xfer_ulbuf_borrowed = FALSE;
  curlx_safefree(multi->xfer_sockbuf);
  multi->xfer_sockbuf_borrowed = FALSE;
}

ساخت Curl_easy *Curl_multi_get_easy(ساخت Curl_multi *multi,
                                      uint32_t mid)
{
  ساخت Curl_easy *ڈیٹا = Curl_uint32_tbl_get(&multi->xfers, mid);
  اگر(GOOD_EASY_HANDLE(ڈیٹا))
    واپس ڈیٹا;
  CURL_TRC_M(multi->admin, "invalid easy handle in xfer table for mid=%u",
             mid);
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  واپس تہی;
}

bool Curl_multi_knows_easy(ساخت Curl_multi *multi, ساخت Curl_easy *ڈیٹا)
{
  واپس Curl_uint32_tbl_get(&multi->xfers, ڈیٹا->mid) == ڈیٹا;
}

ساکن uint32_t multi_xfers_running(ساخت Curl_multi *multi)
{
  اگر(!multi) {
    DEBUGASSERT(0);
    واپس 0;
  }
  واپس multi->xfers_alive;
}

uint32_t Curl_multi_xfers_attached(ساخت Curl_multi *multi)
{
  اگر(!multi || !multi->admin) {
    DEBUGASSERT(0);
    واپس 0;
  }
  /* Discount the admin handle */
  واپس Curl_uint32_tbl_گنتی(&multi->xfers) - 1;
}

خالی Curl_multi_mark_dirty(ساخت Curl_easy *ڈیٹا)
{
  اگر(ڈیٹا->multi && ڈیٹا->mid != UINT32_MAX)
    Curl_uint32_bset_جوڑو(&ڈیٹا->multi->dirty, ڈیٹا->mid);
}

خالی Curl_multi_clear_dirty(ساخت Curl_easy *ڈیٹا)
{
  اگر(ڈیٹا->multi && ڈیٹا->mid != UINT32_MAX)
    Curl_uint32_bset_remove(&ڈیٹا->multi->dirty, ڈیٹا->mid);
}

CURLMcode curl_multi_notify_enable(CURLM *m, غیرعلامتی صحیح notification)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_notify_enable, &mresult)) {
    mresult = Curl_mntfy_enable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

CURLMcode curl_multi_notify_disable(CURLM *m, غیرعلامتی صحیح notification)
{
  ساخت Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  اگر(CURL_MAPI_ENTER(&guard, m, multi_notify_disable, &mresult)) {
    mresult = Curl_mntfy_disable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  واپس mresult;
}

#اگر_تعریف_شدہ DEBUGBUILD
ساکن خالی multi_xfer_dump(ساخت Curl_multi *multi, uint32_t mid,
                            خالی *اندراج_2)
{
  ساخت Curl_easy *ڈیٹا = اندراج_2;

  (خالی)multi;
  اگر(!ڈیٹا) {
    curl_mfprintf(stderr, "mid=%u, entry=NULL, bug in xfer table?\n", mid);
  }
  ورنہ {
    curl_mfprintf(stderr, "mid=%u, magic=%s, p=%p, id=%" FMT_OFF_T
                  ", url=%s\n",
                  mid,
                  (ڈیٹا->magic == CURLEASY_MAGIC_نمبر) ? "GOOD" : "BAD!",
                  (خالی *)ڈیٹا, ڈیٹا->id, Curl_bufref_ptr(&ڈیٹا->state.url));
  }
}

ساکن خالی multi_xfer_tbl_dump(ساخت Curl_multi *multi)
{
  uint32_t mid;
  خالی *اندراج_2;
  curl_mfprintf(stderr, "=== multi xfer table (count=%u, capacity=%u\n",
                Curl_uint32_tbl_گنتی(&multi->xfers),
                Curl_uint32_tbl_استعداد(&multi->xfers));
  اگر(Curl_uint32_tbl_پہلا(&multi->xfers, &mid, &اندراج_2)) {
    multi_xfer_dump(multi, mid, اندراج_2);
    جب(Curl_uint32_tbl_اگلا(&multi->xfers, mid, &mid, &اندراج_2))
      multi_xfer_dump(multi, mid, اندراج_2);
  }
  curl_mfprintf(stderr, "===\n");
  fflush(stderr);
}
#اگر_ختم /* DEBUGBUILD */
