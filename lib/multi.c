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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_share.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "psl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multi_ev.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/wait.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "conncache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multihandle.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sigpipe.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_scache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "socketpair.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

/* initial multi->xfers table size for a full multi */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_XFER_ਸਾਰਣੀ_ਮਾਪ 128

/* CURL_SOCKET_HASH_TABLE_SIZE should be a prime number. Increasing it from 97
   to 911 takes on a 32-bit machine 4 x 804 = 3211 more bytes. Still, every
   curl handle takes 6K memory, therefore this 3K are not significant. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_SOCKET_HASH_ਸਾਰਣੀ_ਮਾਪ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SOCKET_HASH_ਸਾਰਣੀ_ਮਾਪ 911
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_CONNECTION_HASH_ਮਾਪ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CONNECTION_HASH_ਮਾਪ 97
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DNS_HASH_ਮਾਪ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_DNS_HASH_ਮਾਪ 71
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_TLS_SESSION_ਮਾਪ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TLS_SESSION_ਮਾਪ 25
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ move_pending_ਮੰਜ਼ਿਲ_connect(ਬਣਤਰ Curl_multi *multi,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
ਸਥਿਰ CURLMcode ਜੋੜੋ_ਅਗਲਾ_timeout(ਅਚਲ ਬਣਤਰ curltime *pnow,
                                  ਬਣਤਰ Curl_multi *multi,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ multi_timeout(ਬਣਤਰ Curl_multi *multi,
                          timediff_t *pexire_ਵਿੱਥ_us,
                          ਪੂਰਨ *timeout_ms);
ਸਥਿਰ ਖਾਲੀ multi_ਸਮਾਂ_ਸਾਰਣੀ_pending(ਬਣਤਰ Curl_multi *multi);
ਸਥਿਰ ਖਾਲੀ multi_xfer_bufs_free(ਬਣਤਰ Curl_multi *multi);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
ਸਥਿਰ ਖਾਲੀ multi_xfer_tbl_dump(ਬਣਤਰ Curl_multi *multi);
#ਜੇ_ਅੰਤ
/* Get the # of transfers current in process/pending. */
ਸਥਿਰ uint32_t multi_xfers_running(ਬਣਤਰ Curl_multi *multi);

/* function pointer called once when entering a state */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*mstate_enter_func)(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  CURLMstate from_state);

ਸਥਿਰ ਖਾਲੀ mstate_enter_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 CURLMstate from_state)
{
  (ਖਾਲੀ)from_state;
  Curl_init_CONNECT(ਅੰਕੜੇ);
}

ਸਥਿਰ ਖਾਲੀ mstate_enter_did(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             CURLMstate from_state)
{
  (ਖਾਲੀ)from_state;
  ਅੰਕੜੇ->req.chunk = FALSE;
  Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_PRETRANSFER);
  ਜੇ(!CURL_REQ_WANT_SEND(ਅੰਕੜੇ))
    Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_POSTRANSFER);
}

ਸਥਿਰ ਖਾਲੀ mstate_enter_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              CURLMstate from_state)
{
  (ਖਾਲੀ)from_state;
  CURLM_NTFY(ਅੰਕੜੇ, CURLMNOTIFY_EASY_ਪੂਰਾ);
}

ਸਥਿਰ ਖਾਲੀ mstate_enter_completed(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   CURLMstate from_state)
{
  /* we sometimes directly jump to COMPLETED, trigger things
   * we then missed. */
  ਜੇ(from_state < MSTATE_DID) {
    Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_PRETRANSFER);
    Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_POSTRANSFER);
    Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_STARTTRANSFER);
  }
  /* multi_done() should have run and detached any connection already */
  DEBUGASSERT(!ਅੰਕੜੇ->conn);
  Curl_pgrsCompleted(ਅੰਕੜੇ);
  ਜੇ(from_state < MSTATE_ਪੂਰਾ)
    CURLM_NTFY(ਅੰਕੜੇ, CURLMNOTIFY_EASY_ਪੂਰਾ);
  /* changing to COMPLETED means it is in process and needs to go */
  DEBUGASSERT(Curl_uint32_bset_ਸ਼ਾਮਲ_ਹੈ(&ਅੰਕੜੇ->multi->process, ਅੰਕੜੇ->mid));
  Curl_uint32_bset_remove(&ਅੰਕੜੇ->multi->process, ਅੰਕੜੇ->mid);
  Curl_uint32_bset_remove(&ਅੰਕੜੇ->multi->pending, ਅੰਕੜੇ->mid); /* to be sure */

  ਜੇ(Curl_uint32_bset_ਖਾਲੀ_ਥਾਵਾਂ(&ਅੰਕੜੇ->multi->process)) {
    /* free the transfer buffer when we have no more active transfers */
    multi_xfer_bufs_free(ਅੰਕੜੇ->multi);
  }
  /* Important: reset the conn pointer so that we do not point to memory
     that could be freed anytime */
  Curl_detach_connection(ਅੰਕੜੇ);
  Curl_expire_clear_all(ਅੰਕੜੇ); /* stop all timers */
}

/* always use this function to change state, to make debugging easier */
ਸਥਿਰ ਖਾਲੀ mstate(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLMstate state
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
                   , ਪੂਰਨ lineno
#ਜੇ_ਅੰਤ
)
{
  CURLMstate oldstate = ਅੰਕੜੇ->mstate;
  ਸਥਿਰ ਅਚਲ mstate_enter_func state_enter[MSTATE_ਆਖਰੀ] = {
    ਨਿਰਾ,                      /* INIT */
    ਨਿਰਾ,                      /* PENDING */
    ਨਿਰਾ,                      /*ਤਿਆਰੀ*/
    mstate_enter_connect,      /* CONNECT */
    ਨਿਰਾ,                      /* CONNECTING */
    ਨਿਰਾ,                      /* PROTOCONNECT */
    ਨਿਰਾ,                      /* PROTOCONNECTING */
    ਨਿਰਾ,                      /* DO */
    ਨਿਰਾ,                      /* DOING */
    ਨਿਰਾ,                      /* DOING_MORE */
    mstate_enter_did,          /* DID */
    ਨਿਰਾ,                      /* PERFORMING */
    ਨਿਰਾ,                      /* RATELIMITING */
    mstate_enter_ਪੂਰਾ,         /* DONE */
    mstate_enter_completed,    /* COMPLETED */
    ਨਿਰਾ                       /* MSGSENT */
  };

  ਜੇ(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  NOVERBOSE((ਖਾਲੀ)lineno);
  CURL_TRC_M(ਅੰਕੜੇ, "-> [%s] (line %d)", CURL_MSTATE_NAME(state), lineno);
#ਨਹੀਂਤਾਂ
  CURL_TRC_M(ਅੰਕੜੇ, "-> [%s]", CURL_MSTATE_NAME(state));
#ਜੇ_ਅੰਤ

  /* really switching state */
  ਅੰਕੜੇ->mstate = state;
  ਜੇ(state_enter[state])
    state_enter[state](ਅੰਕੜੇ, oldstate);
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ multistate(x, y) mstate(x, y)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ multistate(x, y) mstate(x, y, __LINE__)
#ਜੇ_ਅੰਤ

/* multi->proto_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
ਸਥਿਰ ਖਾਲੀ ph_freeentry(ਖਾਲੀ *p)
{
  (ਖਾਲੀ)p;
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
ਸਥਿਰ ਖਾਲੀ multi_addmsg(ਬਣਤਰ Curl_multi *multi, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(Curl_uint32_bset_ਖਾਲੀ_ਥਾਵਾਂ(&multi->msgsent))
    CURLM_NTFY(multi->admin, CURLMNOTIFY_INFO_READ);
  Curl_uint32_bset_ਜੋੜੋ(&multi->msgsent, ਅੰਕੜੇ->mid);
}

ਸਥਿਰ ਖਾਲੀ multi_timeouts_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

ਬਣਤਰ Curl_multi *Curl_multi_handle(uint32_t xfer_ਸਾਰਣੀ_ਮਾਪ,
                                     size_t ev_hashsize,  /* event hash */
                                     size_t chashsize, /* connection hash */
                                     size_t dnssize,   /* dns hash */
                                     size_t sesssize)  /* TLS session cache */
{
  ਬਣਤਰ curltime created;
  ਬਣਤਰ Curl_multi *multi = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_multi));

  ਜੇ(!multi)
    ਵਾਪਸ ਨਿਰਾ;

  multi->magic = CURLMULTI_MAGIC_ਸੰਖਿਆ;

  /* Initialisation order is important here!
   * easy_init() does a lazy check on curl_global_init() which sets
   * up platform specific things we need. For example calling curlx_pnow()
   * before this is not safe. */
  multi->admin = curl_easy_init();
  ਜੇ(!multi->admin) {
    curlx_free(multi);
    ਵਾਪਸ ਨਿਰਾ;
  }
  multi->admin->multi = multi;
  multi->admin->state.ਅੰਦਰੂਨੀ = TRUE;

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
  Curl_hash_init(&multi->proto_hash, 23, CURL_HASH_TYPE_ਬਾਈਟਾਂ, ph_freeentry);
  Curl_cshutdn_init(&multi->cshutdn);

  multi->multiplexing = TRUE;
  multi->max_concurrent_streams = 100;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_WAKEUP
  multi->wakeup_pair[0] = CURL_SOCKET_BAD;
  multi->wakeup_pair[1] = CURL_SOCKET_BAD;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_ਅੰਦਰੂਨੀ_WAKEUP
  multi->wakeup_ਅੰਦਰੂਨੀ[0] = CURL_SOCKET_BAD;
  multi->wakeup_ਅੰਦਰੂਨੀ[1] = CURL_SOCKET_BAD;
#ਜੇ_ਅੰਤ

  ਜੇ(Curl_uint32_bset_resize(&multi->process, xfer_ਸਾਰਣੀ_ਮਾਪ) ||
     Curl_uint32_bset_resize(&multi->pending, xfer_ਸਾਰਣੀ_ਮਾਪ) ||
     Curl_uint32_bset_resize(&multi->dirty, xfer_ਸਾਰਣੀ_ਮਾਪ) ||
     Curl_uint32_bset_resize(&multi->msgsent, xfer_ਸਾਰਣੀ_ਮਾਪ) ||
     Curl_uint32_tbl_resize(&multi->xfers, xfer_ਸਾਰਣੀ_ਮਾਪ))
    ਜਾਓ error;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  {
    ਅਚਲ ਅੱਖਰ *p;
    ਜੇ(getenv("CURL_DEBUG"))
      multi->admin->ਸੈੱਟ.verbose = TRUE;
    p = getenv("CURL_DBG_MAX_TOTAL_CONNECTIONS");
    ਜੇ(p) {
      curl_off_t l;
      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, UINT32_MAX)) {
        multi->max_ਕੁੱਲ_connections = (uint32_t)l;
      }
    }
  }
#ਜੇ_ਅੰਤ
  Curl_uint32_tbl_ਜੋੜੋ(&multi->xfers, multi->admin, &multi->admin->mid);
  Curl_uint32_bset_ਜੋੜੋ(&multi->process, multi->admin->mid);

  Curl_cpool_init(&multi->cpool, ਨਿਰਾ, chashsize);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  ਜੇ(Curl_ssl_scache_create(sesssize, 2, &multi->ssl_scache))
    ਜਾਓ error;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)sesssize;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
  multi->wsa_event = WSACreateEvent();
  ਜੇ(multi->wsa_event == WSA_INVALID_EVENT)
    ਜਾਓ error;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_WAKEUP
  /* When enabled, rely on this to work. We ignore this in previous
   * versions, but that seems an unnecessary complication. */
  ਜੇ(Curl_wakeup_init(multi->wakeup_pair, TRUE) < 0)
    ਜਾਓ error;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_ਅੰਦਰੂਨੀ_WAKEUP
  ਜੇ(Curl_wakeup_init(multi->wakeup_ਅੰਦਰੂਨੀ, TRUE) < 0)
    ਜਾਓ error;
#ਜੇ_ਅੰਤ

  ਜੇ(Curl_probeipv6(multi))
    ਜਾਓ error;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_THREADED
  ਜੇ(xfer_ਸਾਰਣੀ_ਮਾਪ < CURL_XFER_ਸਾਰਣੀ_ਮਾਪ) { /* easy multi */
    ਜੇ(Curl_async_thrdd_multi_init(multi, 0, 2, 10))
      ਜਾਓ error;
  }
  ਨਹੀਂਤਾਂ { /* real multi handle */
    ਜੇ(Curl_async_thrdd_multi_init(multi, 0, 20, 2000))
      ਜਾਓ error;
  }
#ਜੇ_ਅੰਤ

  ਵਾਪਸ multi;

error:

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_THREADED
  Curl_async_thrdd_multi_destroy(multi, TRUE);
#ਜੇ_ਅੰਤ
  Curl_multi_ev_cleanup(multi);
  Curl_hash_destroy(&multi->proto_hash);
  Curl_dnscache_destroy(&multi->dnscache);
  Curl_cpool_destroy(&multi->cpool, multi->admin);
  Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  Curl_ssl_scache_destroy(multi->ssl_scache);
#ਜੇ_ਅੰਤ
  ਜੇ(multi->admin) {
    Curl_multi_ev_xfer_ਪੂਰਾ(multi, multi->admin);
    multi->admin->multi = ਨਿਰਾ;
    Curl_close(&multi->admin);
  }
  Curl_mntfy_cleanup(multi);

  Curl_uint32_bset_destroy(&multi->process);
  Curl_uint32_bset_destroy(&multi->dirty);
  Curl_uint32_bset_destroy(&multi->pending);
  Curl_uint32_bset_destroy(&multi->msgsent);
  Curl_uint32_tbl_destroy(&multi->xfers);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_ਅੰਦਰੂਨੀ_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_ਅੰਦਰੂਨੀ);
#ਜੇ_ਅੰਤ

  curlx_free(multi);
  ਵਾਪਸ ਨਿਰਾ;
}

CURLM *curl_multi_init(ਖਾਲੀ)
{
  ਵਾਪਸ Curl_multi_handle(CURL_XFER_ਸਾਰਣੀ_ਮਾਪ,
                           CURL_SOCKET_HASH_ਸਾਰਣੀ_ਮਾਪ,
                           CURL_CONNECTION_HASH_ਮਾਪ,
                           CURL_DNS_HASH_ਮਾਪ,
                           CURL_TLS_SESSION_ਮਾਪ);
}

#ਜੇ defined(DEBUGBUILD) && defined(CURLVERBOSE)
ਸਥਿਰ ਖਾਲੀ multi_warn_debug(ਬਣਤਰ Curl_multi *multi, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(!multi->warned) {
    infof(ਅੰਕੜੇ, "!!! WARNING !!!");
    infof(ਅੰਕੜੇ, "This is a debug build of libcurl, "
                "do not use in production.");
    multi->warned = TRUE;
  }
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ multi_warn_debug(x, y) Curl_nop_stmt
#ਜੇ_ਅੰਤ

bool Curl_is_connecting(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ ਅੰਕੜੇ->mstate < MSTATE_DO;
}

ਸਥਿਰ CURLMcode multi_assess_wakeup(ਬਣਤਰ Curl_multi *multi)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_ਅੰਦਰੂਨੀ_WAKEUP
  ਜੇ(multi->socket_cb)
    ਵਾਪਸ Curl_multi_ev_assess_xfer(multi, multi->admin);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)multi;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLM_OK;
}

ਸਥਿਰ CURLMcode multi_xfers_ਜੋੜੋ(ਬਣਤਰ Curl_multi *multi,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  uint32_t ਸਮਰੱਥਾ = Curl_uint32_tbl_ਸਮਰੱਥਾ(&multi->xfers);
  uint32_t new_ਮਾਪ = 0;
  /* Prepare to make this into a CURLMOPT_MAX_TRANSFERS, because some
   * applications may want to prevent a run-away of their memory use. */
  /* UINT_MAX is our "invalid" id, do not let the table grow up to that. */
  ਅਚਲ uint32_t max_ਸਮਰੱਥਾ = UINT_MAX - 1;

  ਜੇ(ਸਮਰੱਥਾ < max_ਸਮਰੱਥਾ) {
    /* We want `multi->xfers` to have "sufficient" free rows, so that we do
     * not have to reuse the `mid` from a removed easy right away.
     * Check if an 8th of the capacity is still free */
    uint32_t ਵਰਤਿਆ = Curl_uint32_tbl_ਗਿਣਤੀ(&multi->xfers);
    uint32_t ਅਣਵਰਤਿਆ = ਸਮਰੱਥਾ - ਵਰਤਿਆ;
    uint32_t min_ਅਣਵਰਤਿਆ = CURLMAX(ਸਮਰੱਥਾ >> 3, 4);
    ਜੇ(ਅਣਵਰਤਿਆ < min_ਅਣਵਰਤਿਆ) {
      /* Grow by 50% of current capacity, in range of [128, 2048],
       * which means the table grows max by 16kb on 64-bit arch. */
      uint32_t growth = CURLMIN(CURLMAX(ਸਮਰੱਥਾ >> 1, 128), 2048);
      /* Make sure the uint arithmetic here works on the corner
       * cases where we are close to max_capacity or UINT_MAX */
      ਜੇ((max_ਸਮਰੱਥਾ - growth) <= ਸਮਰੱਥਾ)
        new_ਮਾਪ = max_ਸਮਰੱਥਾ;
      ਨਹੀਂਤਾਂ
        new_ਮਾਪ = ਸਮਰੱਥਾ + growth;
    }
  }

  ਜੇ(new_ਮਾਪ > ਸਮਰੱਥਾ) {
    /* Grow the bitsets first. Should one fail, we do not need
     * to downsize the already resized ones. The sets continue
     * to work properly when larger than the table, but not
     * the other way around. */
    CURL_TRC_M(ਅੰਕੜੇ, "increasing xfer table size to %u", new_ਮਾਪ);
    ਜੇ(Curl_uint32_bset_resize(&multi->process, new_ਮਾਪ) ||
       Curl_uint32_bset_resize(&multi->dirty, new_ਮਾਪ) ||
       Curl_uint32_bset_resize(&multi->pending, new_ਮਾਪ) ||
       Curl_uint32_bset_resize(&multi->msgsent, new_ਮਾਪ) ||
       Curl_uint32_tbl_resize(&multi->xfers, new_ਮਾਪ))
      ਵਾਪਸ CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  /* Insert the easy into the table now */
  ਜੇ(!Curl_uint32_tbl_ਜੋੜੋ(&multi->xfers, ਅੰਕੜੇ, &ਅੰਕੜੇ->mid)) {
    /* MUST only happen when table is full */
    DEBUGASSERT(Curl_uint32_tbl_ਸਮਰੱਥਾ(&multi->xfers) <=
                Curl_uint32_tbl_ਗਿਣਤੀ(&multi->xfers));
    ਵਾਪਸ CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਵਾਪਸ CURLM_OK;
}

CURLMcode Curl_multi_ਜੋੜੋ_handle(ਬਣਤਰ Curl_multi *multi,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLMcode mresult;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  ਜੇ(ਅੰਕੜੇ->multi)
    ਵਾਪਸ CURLM_ADDED_ALREADY;

  ਜੇ(multi->dead) {
    /* a "dead" handle cannot get added transfers while any existing easy
       handles are still alive - but if there are none alive anymore, it is
       fine to start over and unmark the "deadness" of this handle.
       This means only the admin handle MUST be present. */
    ਜੇ((Curl_uint32_tbl_ਗਿਣਤੀ(&multi->xfers) != 1) ||
       !Curl_uint32_tbl_ਸ਼ਾਮਲ_ਹੈ(&multi->xfers, 0))
      ਵਾਪਸ CURLM_ABORTED_BY_CALLBACK;
    multi->dead = FALSE;
    Curl_uint32_bset_clear(&multi->process);
    Curl_uint32_bset_clear(&multi->dirty);
    Curl_uint32_bset_clear(&multi->pending);
    Curl_uint32_bset_clear(&multi->msgsent);
  }

  ਜੇ(ਅੰਕੜੇ->multi_easy) {
    /* if this easy handle was previously used for curl_easy_perform(), there
       is a private multi handle here that we can kill */
    curl_multi_cleanup(ਅੰਕੜੇ->multi_easy);
    ਅੰਕੜੇ->multi_easy = ਨਿਰਾ;
  }

  /* Insert the easy into the multi->xfers table, assigning it a `mid`. */
  ਜੇ(multi_xfers_ਜੋੜੋ(multi, ਅੰਕੜੇ))
    ਵਾਪਸ CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Initialize timeouts for this handle */
  multi_timeouts_init(ਅੰਕੜੇ);

  /*
   * No failure allowed in this function beyond this point. No modification
   * of easy nor multi handle allowed before this except for potential
   * multi's connection pool growing which will not be undone in this
   * function no matter what.
   */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.errorbuffer)
    ਅੰਕੜੇ->ਸੈੱਟ.errorbuffer[0] = 0;

  ਅੰਕੜੇ->state.os_errno = 0;

  /* make the Curl_easy refer back to this multi handle - before
     Curl_expire() is called. */
  ਅੰਕੜੇ->multi = multi;

  /* set the easy handle */
  multistate(ਅੰਕੜੇ, MSTATE_INIT);
  /* not yet passed INIT state */
  ਅੰਕੜੇ->state.really_alive = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
  /* Do the same for PSL. */
  ਜੇ(ਅੰਕੜੇ->share && (ਅੰਕੜੇ->share->specifier & (1 << CURL_ਤਾਲਾ_ਅੰਕੜੇ_PSL)))
    ਅੰਕੜੇ->psl = &ਅੰਕੜੇ->share->psl;
  ਨਹੀਂਤਾਂ
    ਅੰਕੜੇ->psl = &multi->psl;
#ਜੇ_ਅੰਤ

  /* add the easy handle to the process set */
  Curl_uint32_bset_ਜੋੜੋ(&multi->process, ਅੰਕੜੇ->mid);
  ++multi->xfers_alive;
  ++multi->xfers_ਕੁੱਲ_ever;

  Curl_cpool_xfer_init(ਅੰਕੜੇ);
  multi_warn_debug(multi, ਅੰਕੜੇ);

  /* Make sure the new handle will run */
  Curl_multi_mark_dirty(ਅੰਕੜੇ);

  /* Necessary in event based processing, where dirty handles trigger
   * a timeout callback invocation. */
  mresult = Curl_update_timer(multi);
  ਜੇ(mresult) {
    /* undo the additions above */
    Curl_uint32_bset_remove(&multi->process, ਅੰਕੜੇ->mid);
    Curl_uint32_bset_remove(&multi->dirty, ਅੰਕੜੇ->mid);
    --multi->xfers_alive;
    ਅੰਕੜੇ->multi = ਨਿਰਾ; /* not anymore */
    Curl_uint32_tbl_remove(&multi->xfers, ਅੰਕੜੇ->mid);
    ਅੰਕੜੇ->mid = UINT32_MAX;
    ਵਾਪਸ mresult;
  }

  /* The admin handle only ever has default timeouts set. To improve the
     state somewhat we clone the timeouts from each added handle so that the
     admin handle always has the same timeouts as the most recently added
     easy handle. */
  multi->admin->ਸੈੱਟ.timeout = ਅੰਕੜੇ->ਸੈੱਟ.timeout;
  multi->admin->ਸੈੱਟ.server_response_timeout =
    ਅੰਕੜੇ->ਸੈੱਟ.server_response_timeout;
  multi->admin->ਸੈੱਟ.no_signal = ਅੰਕੜੇ->ਸੈੱਟ.no_signal;

  CURL_TRC_M(ਅੰਕੜੇ, "added to multi, mid=%u, running=%u, total=%u",
             ਅੰਕੜੇ->mid, multi_xfers_running(multi),
             Curl_uint32_tbl_ਗਿਣਤੀ(&multi->xfers));
  ਵਾਪਸ CURLM_OK;
}

CURLMcode curl_multi_ਜੋੜੋ_handle(CURLM *m, CURL *curl)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_ਜੋੜੋ_handle, &mresult)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
    /* Verify that we got a somewhat good easy handle too */
    ਜੇ(!GOOD_EASY_HANDLE(ਅੰਕੜੇ) || ਅੰਕੜੇ->state.ਅੰਦਰੂਨੀ)
      mresult = CURLM_BAD_EASY_HANDLE;
    ਨਹੀਂਤਾਂ
      mresult = Curl_multi_ਜੋੜੋ_handle(m, ਅੰਕੜੇ);
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

ਬਣਤਰ multi_ਪੂਰਾ_ctx {
  ਦੋਆਧਾਰੀ_ਅੰਕ(premature);
};

ਸਥਿਰ bool multi_conn_should_close(ਬਣਤਰ connectdata *conn,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    bool premature)
{
  /* if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of everything else. */
  ਜੇ(conn->ਬਿੱਟ.close)
    ਵਾਪਸ TRUE;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM/NEGOTIATE authentication handshake. */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.reuse_forbid
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
     && !(conn->http_ntlm_state == NTLMSTATE_TYPE2 ||
          conn->proxy_ntlm_state == NTLMSTATE_TYPE2)
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
     && !(conn->http_negotiate_state == CURL_NEGO_AUTH_RECV ||
          conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)
#ਜੇ_ਅੰਤ
    )
    ਵਾਪਸ TRUE;

  /* Unless this connection is for a "connect-only" transfer, it
   * needs to be closed if the protocol handler does not support reuse. */
  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.connect_only && conn->scheme &&
     !(conn->scheme->flags & PROTOPT_CONN_REUSE))
    ਵਾਪਸ TRUE;

  /* if premature is TRUE, it means this connection was said to be DONE before
     the entire request operation is complete and thus we cannot know in what
     state it is for reusing, so we are forced to close it. In a perfect world
     we can add code that keep track of if we really must close it here or not,
     but currently we have no such detail knowledge. */
  ਜੇ(premature && !Curl_conn_is_multiplex(conn, FIRSTSOCKET))
    ਵਾਪਸ TRUE;

  ਵਾਪਸ FALSE;
}

ਸਥਿਰ cpool_do_ਨਤੀਜਾ multi_ਪੂਰਾ_locked(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ connectdata *conn,
                                         ਖਾਲੀ *userdata,
                                         ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਬਣਤਰ multi_ਪੂਰਾ_ctx *mdctx = userdata;

  (ਖਾਲੀ)pnow;
  CURL_TRC_M(ਅੰਕੜੇ, "multi_done_locked, in use=%u", conn->attached_xfers);
  ਜੇ(CONN_INUSE(conn)) {
    /* Stop if still used. */
    CURL_TRC_M(ਅੰਕੜੇ, "Connection still in use %u, no more multi_done now!",
               conn->attached_xfers);
    ਵਾਪਸ CPOOL_DO_KEEP;
  }

  ਜੇ(multi_conn_should_close(conn, ਅੰਕੜੇ, (bool)mdctx->premature)) {
    CURL_TRC_M(ਅੰਕੜੇ, "multi_done, terminating conn #%" FMT_OFF_T " to %s:%u, "
               "forbid=%d, close=%d, premature=%d, conn_multiplex=%d",
               conn->connection_id, conn->origin->user_hostname,
               conn->origin->ਪੋਰਟ,
               ਅੰਕੜੇ->ਸੈੱਟ.reuse_forbid, conn->ਬਿੱਟ.close, mdctx->premature,
               Curl_conn_is_multiplex(conn, FIRSTSOCKET));
    connclose(conn);
    ਵਾਪਸ mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!Curl_conn_get_max_concurrent(ਅੰਕੜੇ, conn, FIRSTSOCKET)) {
    CURL_TRC_M(ਅੰਕੜੇ, "multi_done, conn #%" FMT_OFF_T " to %s:%u was shutdown"
               " by server, not reusing", conn->connection_id,
               conn->origin->user_hostname, conn->origin->ਪੋਰਟ);
    connclose(conn);
    ਵਾਪਸ mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  ਨਹੀਂਤਾਂ {
    ਵਾਪਸ CPOOL_DO_IDLE;
  }
}

ਸਥਿਰ CURLcode multi_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           CURLcode ਸਥਿਤੀ,  /* an error if this is called
                                                after an error was detected */
                           bool premature)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਅਚਲ ਬਣਤਰ curltime *pnow = Curl_pgrs_now(ਅੰਕੜੇ);

  CURL_TRC_M(ਅੰਕੜੇ, "multi_done: status: %d prem: %d done: %d",
             (ਪੂਰਨ)ਸਥਿਤੀ, (ਪੂਰਨ)premature, ਅੰਕੜੇ->state.ਪੂਰਾ);

  ਜੇ(ਅੰਕੜੇ->state.ਪੂਰਾ) {
    /* Stop if multi_done() has already been called */
    ਵਾਪਸ CURLE_OK;
  }
  ਅੰਕੜੇ->state.ਪੂਰਾ = TRUE; /* called now! */

  /* Shut down any ongoing async resolver operation. */
  Curl_resolv_shutdown_all(ਅੰਕੜੇ);

  /* Cleanup possible redirect junk */
  curlx_safefree(ਅੰਕੜੇ->req.newurl);
  curlx_safefree(ਅੰਕੜੇ->req.location);

  ਚੋਣ(ਸਥਿਤੀ) {
  ਹਾਲਤ CURLE_ABORTED_BY_CALLBACK:
  ਹਾਲਤ CURLE_READ_ERROR:
  ਹਾਲਤ CURLE_WRITE_ERROR:
    /* When we are aborted due to a callback return code it has to be counted
       as premature as there is trouble ahead if we do not. We have many
       callbacks and protocols work differently, we could potentially do this
       more fine-grained in the future. */
    premature = TRUE;
    FALLTHROUGH();
  ਮੂਲ:
    ਰੋਕੋ;
  }

  /* this calls the protocol-specific function pointer previously set */
  ਜੇ(conn && conn->scheme->ਲੜੀਵਾਰ->ਪੂਰਾ && (ਅੰਕੜੇ->mstate >= MSTATE_PROTOCONNECT))
    ਨਤੀਜਾ = conn->scheme->ਲੜੀਵਾਰ->ਪੂਰਾ(ਅੰਕੜੇ, ਸਥਿਤੀ, premature);
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = ਸਥਿਤੀ;

  ਜੇ(ਅੰਕੜੇ->mstate > MSTATE_CONNECTING &&
     (ਨਤੀਜਾ != CURLE_ABORTED_BY_CALLBACK)) {
    /* avoid this if
     * - the transfer has not connected
     * - we already aborted by callback to avoid this calling another callback
     */
    ਪੂਰਨ rc = Curl_pgrs_ਪੂਰਾ(ਅੰਕੜੇ, pnow);
    ਜੇ(!ਨਤੀਜਾ && rc)
      ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
  }

  /* Make sure that transfer client writes are really done now. */
  ਨਤੀਜਾ = Curl_1st_fatal(ਨਤੀਜਾ, Curl_xfer_write_ਪੂਰਾ(ਅੰਕੜੇ, premature));

  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_req_ਪੂਰਾ(&ਅੰਕੜੇ->req, ਅੰਕੜੇ, premature);

  ਜੇ(conn) {
    /* Detach connection and decide what to do with it. */
    ਬਣਤਰ multi_ਪੂਰਾ_ctx mdctx;

    /* Inform connection filters that this transfer is done and they
     * need to give up any references to `data`. */
    Curl_conn_ev_ਅੰਕੜੇ_ਪੂਰਾ(ਅੰਕੜੇ, premature);
    Curl_detach_connection(ਅੰਕੜੇ);
    Curl_dnscache_prune(ਅੰਕੜੇ, pnow);

    memset(&mdctx, 0, ਆਕਾਰ(mdctx));
    mdctx.premature = premature;
    Curl_cpool_return(ਅੰਕੜੇ, conn, multi_ਪੂਰਾ_locked, &mdctx, pnow);

    ਜੇ(ਅੰਕੜੇ->multi)
      multi_ਸਮਾਂ_ਸਾਰਣੀ_pending(ਅੰਕੜੇ->multi);
  }

  /* flush the netrc cache */
  Curl_netrc_cleanup(&ਅੰਕੜੇ->state.netrc);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLMcode Curl_multi_remove_handle(ਬਣਤਰ Curl_multi *multi,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLMcode mresult;
  bool premature;
  uint32_t mid;

  /* Prevent users from trying to remove same easy handle more than once */
  ਜੇ(!ਅੰਕੜੇ->multi)
    ਵਾਪਸ CURLM_OK; /* it is already removed so let's say it is fine! */

  /* Prevent users from trying to remove an easy handle from the wrong multi */
  ਜੇ(ਅੰਕੜੇ->multi != multi)
    ਵਾਪਸ CURLM_BAD_EASY_HANDLE;

  ਜੇ(ਅੰਕੜੇ->mid == UINT32_MAX) {
    DEBUGASSERT(0);
    ਵਾਪਸ CURLM_ਅੰਦਰੂਨੀ_ERROR;
  }
  ਜੇ(Curl_uint32_tbl_get(&multi->xfers, ਅੰਕੜੇ->mid) != ਅੰਕੜੇ) {
    DEBUGASSERT(0);
    ਵਾਪਸ CURLM_ਅੰਦਰੂਨੀ_ERROR;
  }

  premature = (ਅੰਕੜੇ->mstate < MSTATE_COMPLETED);

  ਜੇ(ਅੰਕੜੇ->conn) {
    /* If the 'state' is not INIT or COMPLETED, we might need to do something
       nice to put the easy_handle in a good known state when this returns. */
    ਜੇ(premature && (ਅੰਕੜੇ->mstate > MSTATE_DO))
      streamclose(ਅੰਕੜੇ->conn);

    /* multi_done() clears the association between the easy handle and the
       connection.
       Note that this ignores the return code because there is
       nothing really useful to do with it anyway! */
    (ਖਾਲੀ)multi_ਪੂਰਾ(ਅੰਕੜੇ, ਅੰਕੜੇ->ਨਤੀਜਾ, premature);
  }

  /* The timer must be shut down before data->multi is set to NULL, else
     data's splaynode would remain in the splay tree after curl_easy_cleanup is
     called. Do it after multi_done() in case that sets another time! */
  Curl_expire_clear_all(ਅੰਕੜੇ);

  /* In MSGSENT, it was deducted from `multi->xfers_alive` already. */
  ਜੇ(ਅੰਕੜੇ->mstate != MSTATE_MSGSENT)
    --multi->xfers_alive;

  ਜੇ(ਅੰਕੜੇ->state.really_alive) {
    ਅੰਕੜੇ->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    ਜੇ(!multi->xfers_really_alive)
      (ਖਾਲੀ)multi_assess_wakeup(multi);
  }

  Curl_wildcard_dtor(&ਅੰਕੜੇ->wildcard);

  ਅੰਕੜੇ->mstate = MSTATE_COMPLETED;

  /* Remove the association between the connection and the handle */
  Curl_detach_connection(ਅੰਕੜੇ);

  /* Tell event handling that this transfer is definitely going away */
  Curl_multi_ev_xfer_ਪੂਰਾ(multi, ਅੰਕੜੇ);

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.connect_only) {
    ਜੇ(ਅੰਕੜੇ->multi_easy) {
      ਜੇ(ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id != -1) {
        /* Mark any connect-only connection for closure */
        ਬਣਤਰ connectdata *conn;
        (ਖਾਲੀ)Curl_getconnectinfo(ਅੰਕੜੇ, &conn);
        ਜੇ(conn && conn->ਬਿੱਟ.connect_only)
          connclose(conn);
      }
    }
    ਨਹੀਂਤਾਂ {
      /* This removes a handle that was part the multi interface that used
         CONNECT_ONLY, that connection is now left alive but since this handle
         has bits.close set nothing can use that connection anymore and it is
         forbidden from reuse. This easy handle cannot find the connection
         anymore once removed from the multi handle

         Better close the connection here, at once. */
      ਬਣਤਰ connectdata *conn;
      (ਖਾਲੀ)Curl_getconnectinfo(ਅੰਕੜੇ, &conn);
      ਜੇ(conn)
        Curl_conn_close(ਅੰਕੜੇ, conn, TRUE);
    }
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
  /* Remove the PSL association. */
  ਜੇ(ਅੰਕੜੇ->psl == &multi->psl)
    ਅੰਕੜੇ->psl = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

  /* clear the association to this multi handle */
  mid = ਅੰਕੜੇ->mid;
  DEBUGASSERT(Curl_uint32_tbl_ਸ਼ਾਮਲ_ਹੈ(&multi->xfers, mid));
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  Curl_uint32_bset_remove(&multi->process, mid);
  Curl_uint32_bset_remove(&multi->dirty, mid);
  Curl_uint32_bset_remove(&multi->pending, mid);
  Curl_uint32_bset_remove(&multi->msgsent, mid);
  ਅੰਕੜੇ->multi = ਨਿਰਾ;
  ਅੰਕੜੇ->mid = UINT32_MAX;
  ਅੰਕੜੇ->master_mid = UINT32_MAX;

  /* A pending transfer *might* be able to run now. */
  multi_ਸਮਾਂ_ਸਾਰਣੀ_pending(multi);
  mresult = Curl_update_timer(multi);
  ਜੇ(mresult)
    ਵਾਪਸ mresult;

  mresult = multi_assess_wakeup(multi);
  ਜੇ(mresult) {
    failf(ਅੰਕੜੇ, "error enabling wakeup listening: %d", mresult);
    ਵਾਪਸ mresult;
  }

  CURL_TRC_M(ਅੰਕੜੇ, "removed from multi, mid=%u, running=%u, total=%u",
             mid, multi_xfers_running(multi),
             Curl_uint32_tbl_ਗਿਣਤੀ(&multi->xfers));
  ਵਾਪਸ CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *curl)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_remove_handle, &mresult)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
    ਜੇ(!GOOD_EASY_HANDLE(ਅੰਕੜੇ) || ਅੰਕੜੇ->state.ਅੰਦਰੂਨੀ)
      mresult = CURLM_BAD_EASY_HANDLE;
    ਨਹੀਂਤਾਂ
      mresult = Curl_multi_remove_handle(m, ਅੰਕੜੇ);
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

/* Return TRUE if the application asked for multiplexing */
bool Curl_multiplex_wanted(ਅਚਲ ਬਣਤਰ Curl_multi *multi)
{
  ਵਾਪਸ multi && multi->multiplexing;
}

/*
 * Curl_detach_connection() removes the given transfer from the connection.
 *
 * This is the only function that should clear data->conn. This will
 * occasionally be called with the data->conn pointer already cleared.
 */
ਖਾਲੀ Curl_detach_connection(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਜੇ(conn) {
    /* this should never happen, prevent underflow */
    DEBUGASSERT(conn->attached_xfers);
    ਜੇ(conn->attached_xfers) {
      conn->attached_xfers--;
      ਜੇ(!conn->attached_xfers)
        conn->attached_multi = ਨਿਰਾ;
    }
  }
  ਅੰਕੜੇ->conn = ਨਿਰਾ;
}

/*
 * Curl_attach_connection() attaches this transfer to this connection.
 *
 * This is the only function that should assign data->conn.
 * `matched == TRUE` means the transfer's properties match this
 * connection and it is not a temporary attach for maintenance.
 */
ਖਾਲੀ Curl_attach_connection(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ connectdata *conn,
                            bool matched)
{
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(!ਅੰਕੜੇ->conn);
  DEBUGASSERT(conn);
  DEBUGASSERT(conn->attached_xfers < UINT32_MAX);
  ਅੰਕੜੇ->conn = conn;
  ਜੇ(matched) {
    ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id = conn->connection_id;
    ਅੰਕੜੇ->state.ਆਖਰੀ_cpid = conn->cpid;
  }
  ਨਹੀਂਤਾਂ
    DEBUGASSERT(!ਅੰਕੜੇ->mid); /* admin handle */
  conn->attached_xfers++;
  /* all attached transfers must be from the same multi */
  ਜੇ(!conn->attached_multi)
    conn->attached_multi = ਅੰਕੜੇ->multi;
  DEBUGASSERT(conn->attached_multi == ਅੰਕੜੇ->multi);

  ਜੇ(conn->scheme && conn->scheme->ਲੜੀਵਾਰ->attach)
    conn->scheme->ਲੜੀਵਾਰ->attach(ਅੰਕੜੇ, conn);
}

/* adjust pollset for rate limits/pauses */
ਸਥਿਰ CURLcode multi_adjust_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ easy_pollset *ps)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ps->n) {
    bool send_blocked, recv_blocked;

    recv_blocked = (Curl_rlimit_avail(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit, ਨਿਰਾ) <= 0);
    send_blocked = (Curl_rlimit_avail(&ਅੰਕੜੇ->ਤਰੱਕੀ.ul.rlimit, ਨਿਰਾ) <= 0);
    ਜੇ(send_blocked || recv_blocked) {
      ਪੂਰਨ i;
      ਲਈ(i = 0; i <= SECONDARYSOCKET; ++i) {
        curl_socket_t sock = ਅੰਕੜੇ->conn->sock[i];
        ਜੇ(sock == CURL_SOCKET_BAD)
          ਜਾਰੀ;
        ਜੇ(recv_blocked && Curl_pollset_want_recv(ਅੰਕੜੇ, ps, sock)) {
          ਨਤੀਜਾ = Curl_pollset_remove_ਵਿੱਚ(ਅੰਕੜੇ, ps, sock);
          ਜੇ(ਨਤੀਜਾ)
            ਰੋਕੋ;
        }
        ਜੇ(send_blocked && Curl_pollset_want_send(ਅੰਕੜੇ, ps, sock)) {
          ਨਤੀਜਾ = Curl_pollset_remove_ਬਾਹਰਲਾ(ਅੰਕੜੇ, ps, sock);
          ਜੇ(ਨਤੀਜਾ)
            ਰੋਕੋ;
        }
      }
    }

    /* Not blocked and wanting to receive. If there is data pending
     * in the connection filters, make transfer run again. */
    ਜੇ(!recv_blocked &&
       ((Curl_pollset_want_recv(ਅੰਕੜੇ, ps, ਅੰਕੜੇ->conn->sock[FIRSTSOCKET]) &&
         Curl_conn_ਅੰਕੜੇ_pending(ਅੰਕੜੇ, FIRSTSOCKET)) ||
        (Curl_pollset_want_recv(ਅੰਕੜੇ, ps, ਅੰਕੜੇ->conn->sock[SECONDARYSOCKET]) &&
         Curl_conn_ਅੰਕੜੇ_pending(ਅੰਕੜੇ, SECONDARYSOCKET)))) {
      CURL_TRC_M(ਅੰਕੜੇ, "pollset[] has POLLIN, but there is still "
                 "buffered input -> mark as dirty");
      Curl_multi_mark_dirty(ਅੰਕੜੇ);
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode mstate_connecting_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  curl_socket_t sockfd;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(Curl_xfer_recv_is_paused(ਅੰਕੜੇ))
    ਵਾਪਸ CURLE_OK;
  /* If a socket is set, receiving is default. If the socket
   * has not been determined yet (eyeballing), always ask the
   * connection filters for what to monitor. */
  sockfd = Curl_conn_get_ਪਹਿਲਾ_socket(ਅੰਕੜੇ);
  ਜੇ(sockfd != CURL_SOCKET_BAD) {
    ਨਤੀਜਾ = Curl_pollset_change(ਅੰਕੜੇ, ps, sockfd, CURL_POLL_ਵਿੱਚ, 0);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = multi_adjust_pollset(ਅੰਕੜੇ, ps);
  }
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_conn_adjust_pollset(ਅੰਕੜੇ, conn, ps);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode mstate_protocol_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(conn->scheme->ਲੜੀਵਾਰ->proto_pollset)
    ਨਤੀਜਾ = conn->scheme->ਲੜੀਵਾਰ->proto_pollset(ਅੰਕੜੇ, ps);
  ਨਹੀਂਤਾਂ {
    curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
    ਜੇ(sockfd != CURL_SOCKET_BAD) {
      /* Default is to wait to something from the server */
      ਨਤੀਜਾ = Curl_pollset_change(ਅੰਕੜੇ, ps, sockfd, CURL_POLL_ਵਿੱਚ, 0);
    }
  }
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = multi_adjust_pollset(ਅੰਕੜੇ, ps);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_conn_adjust_pollset(ਅੰਕੜੇ, conn, ps);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode mstate_do_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(conn->scheme->ਲੜੀਵਾਰ->doing_pollset)
    ਨਤੀਜਾ = conn->scheme->ਲੜੀਵਾਰ->doing_pollset(ਅੰਕੜੇ, ps);
  ਨਹੀਂਤਾਂ ਜੇ(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    ਨਤੀਜਾ = Curl_pollset_ਜੋੜੋ_ਬਾਹਰਲਾ(ਅੰਕੜੇ, ps, conn->sock[conn->send_idx]);
  }
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = multi_adjust_pollset(ਅੰਕੜੇ, ps);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_conn_adjust_pollset(ਅੰਕੜੇ, conn, ps);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode mstate_domore_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(conn->scheme->ਲੜੀਵਾਰ->domore_pollset)
    ਨਤੀਜਾ = conn->scheme->ਲੜੀਵਾਰ->domore_pollset(ਅੰਕੜੇ, ps);
  ਨਹੀਂਤਾਂ ਜੇ(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    ਨਤੀਜਾ = Curl_pollset_ਜੋੜੋ_ਬਾਹਰਲਾ(ਅੰਕੜੇ, ps, conn->sock[conn->send_idx]);
  }
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = multi_adjust_pollset(ਅੰਕੜੇ, ps);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_conn_adjust_pollset(ਅੰਕੜੇ, conn, ps);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode mstate_perform_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(conn->scheme->ਲੜੀਵਾਰ->perform_pollset)
    ਨਤੀਜਾ = conn->scheme->ਲੜੀਵਾਰ->perform_pollset(ਅੰਕੜੇ, ps);
  ਨਹੀਂਤਾਂ {
    /* Default is to obey the request flags for send/recv */
    ਜੇ(Curl_req_want_recv(ਅੰਕੜੇ) && CONN_SOCK_IDX_VALID(conn->recv_idx)) {
      ਨਤੀਜਾ = Curl_pollset_ਜੋੜੋ_ਵਿੱਚ(ਅੰਕੜੇ, ps, conn->sock[conn->recv_idx]);
    }
    ਜੇ(!ਨਤੀਜਾ && Curl_req_want_send(ਅੰਕੜੇ) &&
       CONN_SOCK_IDX_VALID(conn->send_idx)) {
      ਨਤੀਜਾ = Curl_pollset_ਜੋੜੋ_ਬਾਹਰਲਾ(ਅੰਕੜੇ, ps, conn->sock[conn->send_idx]);
    }
  }
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = multi_adjust_pollset(ਅੰਕੜੇ, ps);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_conn_adjust_pollset(ਅੰਕੜੇ, conn, ps);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਸਥਿਰ size_t multi_timeouts_ਗਿਣਤੀ(ਬਣਤਰ expire_timers *timeouts)
{
  size_t n = 0;
  uint8_t eid = timeouts->ਪਹਿਲਾ;
  ਲਈ(; eid < EXPIRE_ਆਖਰੀ; eid = timeouts->ਅਗਲਾ[eid])
    ++n;
  ਵਾਪਸ n;
}
#ਜੇ_ਅੰਤ

/* Initializes `poll_set` with the current socket poll actions needed
 * for transfer `data`. */
CURLMcode Curl_multi_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ easy_pollset *ps)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  Curl_pollset_reset(ps);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_ਅੰਦਰੂਨੀ_WAKEUP
  /* The admin handle always listens on the wakeup socket when there
   * are transfers alive. */
  ਜੇ(ਅੰਕੜੇ->multi && (ਅੰਕੜੇ == ਅੰਕੜੇ->multi->admin) &&
     ਅੰਕੜੇ->multi->xfers_really_alive) {
    CURL_TRC_M(ਅੰਕੜੇ, "adding wakeup, %u xfers really alive",
               ਅੰਕੜੇ->multi->xfers_really_alive);
    ਨਤੀਜਾ = Curl_pollset_ਜੋੜੋ_ਵਿੱਚ(ਅੰਕੜੇ, ps, ਅੰਕੜੇ->multi->wakeup_ਅੰਦਰੂਨੀ[0]);
  }
#ਜੇ_ਅੰਤ
  /* If the transfer has no connection, this is fine. Happens when
     called via curl_multi_remove_handle() => Curl_multi_ev_assess() =>
     Curl_multi_pollset(). */
  ਜੇ(!ਨਤੀਜਾ && ਅੰਕੜੇ->conn) {
    ਚੋਣ(ਅੰਕੜੇ->mstate) {
    ਹਾਲਤ MSTATE_INIT:
    ਹਾਲਤ MSTATE_PENDING:
    ਹਾਲਤ MSTATE_ਤਿਆਰੀ:
    ਹਾਲਤ MSTATE_CONNECT:
      /* nothing to poll for yet */
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_CONNECTING:
      ਨਤੀਜਾ = mstate_connecting_pollset(ਅੰਕੜੇ, ps);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_PROTOCONNECT:
    ਹਾਲਤ MSTATE_PROTOCONNECTING:
      ਨਤੀਜਾ = mstate_protocol_pollset(ਅੰਕੜੇ, ps);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_DO:
    ਹਾਲਤ MSTATE_DOING:
      ਨਤੀਜਾ = mstate_do_pollset(ਅੰਕੜੇ, ps);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_DOING_ਹੋਰ:
      ਨਤੀਜਾ = mstate_domore_pollset(ਅੰਕੜੇ, ps);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_DID: /* same as PERFORMING in regard to polling */
    ਹਾਲਤ MSTATE_PERFORMING:
      ਨਤੀਜਾ = mstate_perform_pollset(ਅੰਕੜੇ, ps);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_RATELIMITING:
      /* we need to let time pass, ignore socket(s) */
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_ਪੂਰਾ:
    ਹਾਲਤ MSTATE_COMPLETED:
    ਹਾਲਤ MSTATE_MSGSENT:
      /* nothing more to poll for */
      ਰੋਕੋ;

    ਮੂਲ:
      failf(ਅੰਕੜੇ, "multi_getsock: unexpected multi state %d",
            (ਪੂਰਨ)ਅੰਕੜੇ->mstate);
      DEBUGASSERT(0);
      ਰੋਕੋ;
    }
  }

  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
      ਵਾਪਸ CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    failf(ਅੰਕੜੇ, "error determining pollset: %d", (ਪੂਰਨ)ਨਤੀਜਾ);
    ਵਾਪਸ CURLM_ਅੰਦਰੂਨੀ_ERROR;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਜੇ(CURL_TRC_M_is_verbose(ਅੰਕੜੇ)) {
    size_t timeout_ਗਿਣਤੀ = multi_timeouts_ਗਿਣਤੀ(&ਅੰਕੜੇ->state.timeouts);
    ਚੋਣ(ps->n) {
    ਹਾਲਤ 0:
      CURL_TRC_M(ਅੰਕੜੇ, "pollset[], timeouts=%zu, paused %d/%d (r/w)",
                 timeout_ਗਿਣਤੀ,
                 Curl_xfer_send_is_paused(ਅੰਕੜੇ),
                 Curl_xfer_recv_is_paused(ਅੰਕੜੇ));
      ਰੋਕੋ;
    ਹਾਲਤ 1:
      CURL_TRC_M(ਅੰਕੜੇ, "pollset[fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_ਵਿੱਚ) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_ਬਾਹਰਲਾ) ? "OUT" : "",
                 timeout_ਗਿਣਤੀ);
      ਰੋਕੋ;
    ਹਾਲਤ 2:
      CURL_TRC_M(ਅੰਕੜੇ, "pollset[fd=%" FMT_SOCKET_T " %s%s, "
                 "fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_ਵਿੱਚ) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_ਬਾਹਰਲਾ) ? "OUT" : "",
                 ps->sockets[1],
                 (ps->actions[1] & CURL_POLL_ਵਿੱਚ) ? "IN" : "",
                 (ps->actions[1] & CURL_POLL_ਬਾਹਰਲਾ) ? "OUT" : "",
                 timeout_ਗਿਣਤੀ);
      ਰੋਕੋ;
    ਮੂਲ:
      CURL_TRC_M(ਅੰਕੜੇ, "pollset[fds=%u], timeouts=%zu", ps->n, timeout_ਗਿਣਤੀ);
      ਰੋਕੋ;
    }
    CURL_TRC_EASY_TIMERS(ਅੰਕੜੇ);
  }
#ਜੇ_ਅੰਤ

  ਵਾਪਸ CURLM_OK;
}

CURLMcode curl_multi_fdset(CURLM *m,
                           fd_set *read_fd_ਸੈੱਟ, fd_set *write_fd_ਸੈੱਟ,
                           fd_set *exc_fd_ਸੈੱਟ, ਪੂਰਨ *max_fd)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_fdset, &mresult)) {
    /* Scan through all the easy handles to get the file descriptors set.
       Some easy handles may not have connected to the remote host yet,
       and then we must make sure that is done. */
    ਬਣਤਰ Curl_multi *multi = m;
    ਬਣਤਰ easy_pollset ps;
    ਪੂਰਨ this_max_fd = -1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
    uint32_t mid;
    (ਖਾਲੀ)exc_fd_ਸੈੱਟ;

    Curl_pollset_init(&ps);
    ਜੇ(Curl_uint32_bset_ਪਹਿਲਾ(&multi->process, &mid)) {
      ਕਰੋ {
        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);

        ਜੇ(!ਅੰਕੜੇ) {
          DEBUGASSERT(0);
          ਜਾਰੀ;
        }

        Curl_multi_pollset(ਅੰਕੜੇ, &ps);
        ਲਈ(i = 0; i < ps.n; i++) {
          ਜੇ(!FDSET_SOCK(ps.sockets[i]))
            /* pretend it does not exist */
            ਜਾਰੀ;
          ਜੇ(ps.actions[i] & CURL_POLL_ਵਿੱਚ)
            FD_SET(ps.sockets[i], read_fd_ਸੈੱਟ);
          ਜੇ(ps.actions[i] & CURL_POLL_ਬਾਹਰਲਾ)
            FD_SET(ps.sockets[i], write_fd_ਸੈੱਟ);
          ਜੇ((ਪੂਰਨ)ps.sockets[i] > this_max_fd)
            this_max_fd = (ਪੂਰਨ)ps.sockets[i];
        }
      } ਜਦੋਂ(Curl_uint32_bset_ਅਗਲਾ(&multi->process, mid, &mid));
    }

    Curl_cshutdn_setfds(&multi->cshutdn, multi->admin,
                        read_fd_ਸੈੱਟ, write_fd_ਸੈੱਟ, &this_max_fd);

    *max_fd = this_max_fd;
    Curl_pollset_cleanup(&ps);

    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

CURLMcode curl_multi_waitfds(CURLM *m,
                             ਬਣਤਰ curl_waitfd *ufds,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਮਾਪ,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *fd_ਗਿਣਤੀ)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_waitfds, &mresult)) {
    ਬਣਤਰ Curl_waitfds cwfds;
    ਬਣਤਰ Curl_multi *multi = m;
    ਬਣਤਰ easy_pollset ps;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ need = 0;
    uint32_t mid;

    ਜੇ(!ufds && (ਮਾਪ || !fd_ਗਿਣਤੀ)) {
      mresult = CURLM_BAD_FUNCTION_ਦਲੀਲ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    Curl_pollset_init(&ps);
    Curl_waitfds_init(&cwfds, ufds, ਮਾਪ);
    mresult = CURLM_OK;
    ਜੇ(Curl_uint32_bset_ਪਹਿਲਾ(&multi->process, &mid)) {
      ਕਰੋ {
        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);
        ਜੇ(!ਅੰਕੜੇ) {
          DEBUGASSERT(0);
          Curl_uint32_bset_remove(&multi->process, mid);
          Curl_uint32_bset_remove(&multi->dirty, mid);
          ਜਾਰੀ;
        }
        Curl_multi_pollset(ਅੰਕੜੇ, &ps);
        need += Curl_waitfds_ਜੋੜੋ_ps(&cwfds, &ps);
      } ਜਦੋਂ(Curl_uint32_bset_ਅਗਲਾ(&multi->process, mid, &mid));
    }

    need += Curl_cshutdn_ਜੋੜੋ_waitfds(&multi->cshutdn, multi->admin, &cwfds);

    ਜੇ(need != cwfds.n && ufds)
      mresult = CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    ਜੇ(fd_ਗਿਣਤੀ)
      *fd_ਗਿਣਤੀ = need;
    Curl_pollset_cleanup(&ps);
  }
ਬਾਹਰਲਾ:
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
/* Reset FD_WRITE for TCP sockets. Nothing is actually sent. UDP sockets cannot
 * be reset this way because an empty datagram would be sent. #9203
 *
 * "On Windows the internal state of FD_WRITE as returned from
 * WSAEnumNetworkEvents is only reset after successful send()."
 */
ਸਥਿਰ ਖਾਲੀ reset_socket_fdwrite(curl_socket_t s)
{
  ਪੂਰਨ t;
  ਪੂਰਨ l = (ਪੂਰਨ)ਆਕਾਰ(t);
  ਜੇ(!getsockopt(s, SOL_SOCKET, SO_TYPE, (ਅੱਖਰ *)&t, &l) && t == SOCK_STREAM)
    swrite(s, ਨਿਰਾ, 0);
}

ਸਥਿਰ CURLMcode multi_winsock_select(ਬਣਤਰ Curl_multi *multi,
                                      ਬਣਤਰ curl_pollfds *cpfds,
                                      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ curl_nfds,
                                      ਬਣਤਰ curl_waitfd extra_fds[],
                                      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ extra_nfds,
                                      ਪੂਰਨ timeout_ms,
                                      bool extrawait,
                                      ਪੂਰਨ *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  WSANETWORKEVENTS wsa_events;
  ਪੂਰਨ nevents = 0;
  size_t i;

  DEBUGASSERT(multi->wsa_event != WSA_INVALID_EVENT);

  /* Set the WSA events based on the collected pollds */
  ਲਈ(i = 0; i < cpfds->n; i++) {
    ਲੰਮਾ ਨਕਾਬ = 0;
    ਜੇ(cpfds->pfds[i].events & POLLIN)
      ਨਕਾਬ |= FD_READ | FD_ACCEPT | FD_CLOSE;
    ਜੇ(cpfds->pfds[i].events & POLLPRI)
      ਨਕਾਬ |= FD_OOB;
    ਜੇ(cpfds->pfds[i].events & POLLOUT) {
      ਨਕਾਬ |= FD_WRITE | FD_CONNECT | FD_CLOSE;
      reset_socket_fdwrite(cpfds->pfds[i].fd);
    }
    ਜੇ(ਨਕਾਬ && WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, ਨਕਾਬ)) {
      mresult = CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਜੇ(cpfds->n || extrawait) {
    ਪੂਰਨ pollrc = 0;

    ਜੇ(cpfds->n) {         /* pre-check with Winsock */
      pollrc = Curl_poll(cpfds->pfds, cpfds->n, 0);
      ਜੇ(pollrc < 0) {
        mresult = CURLM_UNRECOVERABLE_POLL;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      nevents = pollrc;
    }

    ਜੇ(!nevents) {
      /* now wait... if not ready during the pre-check (pollrc == 0) */
      WSAWait_For_ਗੁਣਜ_Events(1, &multi->wsa_event, FALSE, (DWORD)timeout_ms,
                               FALSE);
    }

    /* With Winsock, we have to run the following section unconditionally
       to call WSAEventSelect(fd, event, 0) on all the sockets */
    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    ਲਈ(i = 0; i < extra_nfds; i++) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਨਕਾਬ = 0;
      curl_socket_t s = extra_fds[i].fd;

      wsa_events.l_ਜਾਲ_Events = 0;
      ਜੇ(WSAEnum_ਜਾਲ_Events(s, ਨਿਰਾ, &wsa_events) == 0) {
        ਜੇ(wsa_events.l_ਜਾਲ_Events & (FD_READ | FD_ACCEPT | FD_CLOSE))
          ਨਕਾਬ |= CURL_WAIT_POLLIN;
        ਜੇ(wsa_events.l_ਜਾਲ_Events & (FD_WRITE | FD_CONNECT | FD_CLOSE))
          ਨਕਾਬ |= CURL_WAIT_POLLOUT;
        ਜੇ(wsa_events.l_ਜਾਲ_Events & FD_OOB)
          ਨਕਾਬ |= CURL_WAIT_POLLPRI;
        ਜੇ(!pollrc && wsa_events.l_ਜਾਲ_Events)
          nevents++;
      }
      WSAEventSelect(s, multi->wsa_event, 0);
      ਜੇ(!pollrc) {
        extra_fds[i].revents = (ਛੋਟਾ)ਨਕਾਬ;
        ਜਾਰੀ;
      }
      ਨਹੀਂਤਾਂ {
        ਬਿਨਾਨਿਸ਼ਾਨੀ r = (ਬਿਨਾਨਿਸ਼ਾਨੀ)cpfds->pfds[curl_nfds + i].revents;
        ਜੇ(r & POLLIN)
          ਨਕਾਬ |= CURL_WAIT_POLLIN;
        ਜੇ(r & POLLOUT)
          ਨਕਾਬ |= CURL_WAIT_POLLOUT;
        ਜੇ(r & POLLPRI)
          ਨਕਾਬ |= CURL_WAIT_POLLPRI;
        extra_fds[i].revents = (ਛੋਟਾ)ਨਕਾਬ;
      }
    }

    /* Count up all our own sockets that had activity,
       and remove them from the event. */
    ਲਈ(i = 0; i < curl_nfds; ++i) {
      wsa_events.l_ਜਾਲ_Events = 0;
      ਜੇ(WSAEnum_ਜਾਲ_Events(cpfds->pfds[i].fd, ਨਿਰਾ, &wsa_events) == 0) {
        ਜੇ(!pollrc && wsa_events.l_ਜਾਲ_Events)
          nevents++;
      }
      WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, 0);
    }
    WSAResetEvent(multi->wsa_event);
  }

ਬਾਹਰਲਾ:
  *pnevents = nevents;
  ਵਾਪਸ mresult;
}

#ਨਹੀਂਤਾਂ /* USE_WINSOCK */

ਸਥਿਰ CURLMcode multi_posix_poll(ਬਣਤਰ Curl_multi *multi,
                                  ਬਣਤਰ curl_pollfds *cpfds,
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ curl_nfds,
                                  ਬਣਤਰ curl_waitfd extra_fds[],
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ extra_nfds,
                                  ਪੂਰਨ timeout_ms,
                                  bool extrawait,
                                  ਪੂਰਨ *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  ਪੂਰਨ nevents = 0;
  size_t i;

  (ਖਾਲੀ)multi;
  ਜੇ(cpfds->n) {
    ਪੂਰਨ pollrc = Curl_poll(cpfds->pfds, cpfds->n, timeout_ms); /* wait... */
    ਜੇ(pollrc < 0) {
      mresult = CURLM_UNRECOVERABLE_POLL;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    nevents = pollrc;

    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    ਲਈ(i = 0; i < extra_nfds; i++) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ r = (ਬਿਨਾਨਿਸ਼ਾਨੀ)cpfds->pfds[curl_nfds + i].revents;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਨਕਾਬ = 0;
      ਜੇ(r & POLLIN)
        ਨਕਾਬ |= CURL_WAIT_POLLIN;
      ਜੇ(r & POLLOUT)
        ਨਕਾਬ |= CURL_WAIT_POLLOUT;
      ਜੇ(r & POLLPRI)
        ਨਕਾਬ |= CURL_WAIT_POLLPRI;
      extra_fds[i].revents = (ਛੋਟਾ)ਨਕਾਬ;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(extrawait) {
    /* No fds to poll, but asked to obey timeout_ms anyway. We cannot
     * use Curl_poll() as it, on some platforms, returns immediately
     * without fds. */
    curlx_wait_ms(timeout_ms);
  }

ਬਾਹਰਲਾ:
  *pnevents = nevents;
  ਵਾਪਸ mresult;
}

#ਜੇ_ਅੰਤ /* !USE_WINSOCK */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਕ_POLLS_ON_ਢੇਰ 10

ਸਥਿਰ CURLMcode multi_wait(ਬਣਤਰ Curl_multi *multi,
                            ਬਣਤਰ curl_waitfd extra_fds[],
                            ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ extra_nfds,
                            ਪੂਰਨ timeout_ms,
                            ਪੂਰਨ *ret,
                            bool extrawait)  /* when no socket, wait */
{
  size_t i;
  ਪੂਰਨ timeout_ਅੰਦਰੂਨੀ;
  ਪੂਰਨ nevents = 0;
  ਬਣਤਰ easy_pollset ps;
  ਬਣਤਰ pollfd a_few_on_ਢੇਰ[ਅੰਕ_POLLS_ON_ਢੇਰ];
  ਬਣਤਰ curl_pollfds cpfds;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ curl_nfds = 0; /* how many pfds are for curl transfers */
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = ਨਿਰਾ;
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_WAKEUP
  ਪੂਰਨ wakeup_idx = -1;
#ਜੇ_ਅੰਤ

  ਜੇ(timeout_ms < 0)
    ਵਾਪਸ CURLM_BAD_FUNCTION_ਦਲੀਲ;

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ਢੇਰ, ਅੰਕ_POLLS_ON_ਢੇਰ);

  /* Add the curl handles to our pollfds first */
  ਜੇ(Curl_uint32_bset_ਪਹਿਲਾ(&multi->process, &mid)) {
    ਕਰੋ {
      ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);
      ਜੇ(!ਅੰਕੜੇ) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        ਜਾਰੀ;
      }
      Curl_multi_pollset(ਅੰਕੜੇ, &ps);
      ਜੇ(Curl_pollfds_ਜੋੜੋ_ps(&cpfds, &ps)) {
        mresult = CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    } ਜਦੋਂ(Curl_uint32_bset_ਅਗਲਾ(&multi->process, mid, &mid));
  }

  ਜੇ(Curl_cshutdn_ਜੋੜੋ_pollfds(&multi->cshutdn, multi->admin, &cpfds)) {
    mresult = CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_WAKEUP
  /* If `extrawait` is TRUE *or* we have `extra_fds`to poll *or* we
   * have transfer sockets to poll, we obey `timeout_ms`.
   * Then we need to also monitor the multi's wakeup
   * socket to catch calls to `curl_multi_wakeup()` during the wait. */
  ਜੇ(extrawait || cpfds.n || extra_nfds) {
    wakeup_idx = cpfds.n;
    ਜੇ(Curl_pollfds_ਜੋੜੋ_sock(&cpfds, multi->wakeup_pair[0], POLLIN)) {
      mresult = CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
#ਜੇ_ਅੰਤ

  curl_nfds = cpfds.n; /* what curl internally uses in cpfds */
  /* Add external file descriptions from poll-like struct curl_waitfd */
  ਲਈ(i = 0; i < extra_nfds; i++) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ events = 0;
    ਜੇ(extra_fds[i].events & CURL_WAIT_POLLIN)
      events |= POLLIN;
    ਜੇ(extra_fds[i].events & CURL_WAIT_POLLPRI)
      events |= POLLPRI;
    ਜੇ(extra_fds[i].events & CURL_WAIT_POLLOUT)
      events |= POLLOUT;
    ਜੇ(Curl_pollfds_ਜੋੜੋ_sock(&cpfds, extra_fds[i].fd, events)) {
      mresult = CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  /* We check the internal timeout *AFTER* we collected all sockets to
   * poll. Collecting the sockets may install new timers by protocols
   * and connection filters.
   * Use the shorter one of the internal and the caller requested timeout.
   * If we are called with `!extrawait` and multi_timeout() reports no
   * timeouts exist, do not wait. */
  multi_timeout(multi, ਨਿਰਾ, &timeout_ਅੰਦਰੂਨੀ);
  ਜੇ((timeout_ਅੰਦਰੂਨੀ >= 0) && (timeout_ਅੰਦਰੂਨੀ < timeout_ms))
    timeout_ms = timeout_ਅੰਦਰੂਨੀ;

  ਜੇ(ਅੰਕੜੇ)
    CURL_TRC_M(ਅੰਕੜੇ, "multi_wait(fds=%u, timeout=%d) tinternal=%d",
               cpfds.n, timeout_ms, timeout_ਅੰਦਰੂਨੀ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
  mresult = multi_winsock_select(multi, &cpfds, curl_nfds,
                                 extra_fds, extra_nfds,
                                 timeout_ms, extrawait, &nevents);
#ਨਹੀਂਤਾਂ
  mresult = multi_posix_poll(multi, &cpfds, curl_nfds,
                             extra_fds, extra_nfds,
                             timeout_ms, extrawait, &nevents);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_WAKEUP
    ਜੇ(nevents && (wakeup_idx >= 0)) {
      ਜੇ(cpfds.pfds[wakeup_idx].revents & POLLIN) {
        (ਖਾਲੀ)Curl_wakeup_consume(multi->wakeup_pair, TRUE);
        /* do not count the wakeup socket into the returned value */
        nevents--;
      }
    }
#ਜੇ_ਅੰਤ

ਬਾਹਰਲਾ:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  ਜੇ(ret)
    *ret = nevents;
  ਵਾਪਸ mresult;
}

CURLMcode curl_multi_wait(CURLM *m,
                          ਬਣਤਰ curl_waitfd extra_fds[],
                          ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ extra_nfds,
                          ਪੂਰਨ timeout_ms,
                          ਪੂਰਨ *ret)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_wait, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, FALSE);
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

CURLMcode curl_multi_poll(CURLM *m,
                          ਬਣਤਰ curl_waitfd extra_fds[],
                          ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ extra_nfds,
                          ਪੂਰਨ timeout_ms,
                          ਪੂਰਨ *ret)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_poll, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, TRUE);
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

CURLMcode curl_multi_wakeup(CURLM *m)
{
  /* this function is usually called from another thread,
     it has to be careful only to access parts of the
     Curl_multi struct that are constant */
  ਬਣਤਰ Curl_multi *multi = m;
  CURLMcode mresult = CURLM_WAKEUP_FAILURE;

  /* GOOD_MULTI_HANDLE can be safely called */
  ਜੇ(!GOOD_MULTI_HANDLE(multi))
    ਵਾਪਸ CURLM_BAD_HANDLE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_WAKEUP
  /* the wakeup_pair variable is only written during init and cleanup,
     making it safe to access from another thread after the init part
     and before cleanup */
  ਜੇ(!Curl_wakeup_signal(multi->wakeup_pair))
    mresult = CURLM_OK;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
  ਜੇ(WSASetEvent(multi->wsa_event))
    mresult = CURLM_OK;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ mresult;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_ਅੰਦਰੂਨੀ_WAKEUP
ਖਾਲੀ Curl_multi_wakeup_ਅੰਦਰੂਨੀ(ਬਣਤਰ Curl_multi *multi)
{
  /* This is expected to be invocable from another thread which
   * does NOT outlive the multi handle. Check for sanity. */
  ਜੇ(GOOD_MULTI_HANDLE(multi))
    Curl_wakeup_signal(multi->wakeup_ਅੰਦਰੂਨੀ);
  ਨਹੀਂਤਾਂ
    DEBUGASSERT(0);
}
#ਜੇ_ਅੰਤ

/*
 * multi_ischanged() is called
 *
 * Returns TRUE/FALSE whether the state is changed to trigger a CONNECT_PEND
 * => CONNECT action.
 *
 * Set 'clear' to TRUE to have it also clear the state variable.
 */
ਸਥਿਰ bool multi_ischanged(ਬਣਤਰ Curl_multi *multi, bool clear)
{
  bool retval = FALSE;
  DEBUGASSERT(multi);
  ਜੇ(multi) {
    retval = (bool)multi->recheckstate;
    ਜੇ(clear)
      multi->recheckstate = FALSE;
  }
  ਵਾਪਸ retval;
}

/*
 * Curl_multi_connchanged() is called to tell that there is a connection in
 * this multi handle that has changed state (multiplexing become possible, the
 * number of allowed streams changed or similar), and a subsequent use of this
 * multi handle should move CONNECT_PEND handles back to CONNECT to have them
 * retry.
 */
ਖਾਲੀ Curl_multi_connchanged(ਬਣਤਰ Curl_multi *multi)
{
  /* May be invoked for a `data->multi` when no multi is present. */
  ਜੇ(multi)
    multi->recheckstate = TRUE;
}

CURLMcode Curl_multi_ਜੋੜੋ_perform(ਬਣਤਰ Curl_multi *multi,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ connectdata *conn)
{
  CURLMcode mresult;

  mresult = Curl_multi_ਜੋੜੋ_handle(multi, ਅੰਕੜੇ);
  ਜੇ(!mresult) {
    CURLcode ਨਤੀਜਾ;

    /* pass in NULL for 'conn' here since we do not want to init the
       connection, only this transfer */
    ਨਤੀਜਾ = Curl_init_transfer(ਅੰਕੜੇ, ਨਿਰਾ);
    ਜੇ(ਨਤੀਜਾ) {
      Curl_multi_remove_handle(multi, ਅੰਕੜੇ);
      ਵਾਪਸ CURLM_ਅੰਦਰੂਨੀ_ERROR;
    }

    /* take this handle to the perform state right away */
    multistate(ਅੰਕੜੇ, MSTATE_PERFORMING);
    Curl_attach_connection(ਅੰਕੜੇ, conn, TRUE);
    CURL_REQ_ਸੈੱਟ_RECV(ਅੰਕੜੇ);
  }
  ਵਾਪਸ mresult;
}

ਸਥਿਰ CURLcode multi_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(conn->scheme);

  ਜੇ(conn->scheme->ਲੜੀਵਾਰ->do_it)
    ਨਤੀਜਾ = conn->scheme->ਲੜੀਵਾਰ->do_it(ਅੰਕੜੇ, ਪੂਰਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * multi_do_more() is called during the DO_MORE multi state. It is a second
 * stage DO state which (wrongly) was introduced to support FTP's second
 * connection.
 *
 * 'complete' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (to DOING state when there is more work to do)
 */

ਸਥਿਰ CURLcode multi_do_ਹੋਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, domore *complete)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  *complete = DOMORE_INCOMPLETE;

  ਜੇ(conn->scheme->ਲੜੀਵਾਰ->do_ਹੋਰ)
    ਨਤੀਜਾ = conn->scheme->ਲੜੀਵਾਰ->do_ਹੋਰ(ਅੰਕੜੇ, complete);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Check whether a timeout occurred, and handle it if it did
 */
ਸਥਿਰ bool multi_handle_timeout(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਬਣਤਰ curltime *pnow,
                                 bool *ਧਾਰਾ_error,
                                 CURLcode *ਨਤੀਜਾ)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_timeleft_now_ms(ਅੰਕੜੇ, pnow);
  ਜੇ(timeout_ms < 0) {
    /* Handle timed out */
    timerid ਅਧਾਰ_timer = Curl_is_connecting(ਅੰਕੜੇ) ?
                         TIMER_STARTSINGLE : TIMER_STARTOP;
    timediff_t elapsed_ms = Curl_pgrs_since_ms(ਅੰਕੜੇ, ਨਿਰਾ, ਅਧਾਰ_timer);
    ਜੇ(ਅੰਕੜੇ->mstate == MSTATE_CONNECTING) {
      failf(ਅੰਕੜੇ, "%s timed out after %" FMT_TIMEDIFF_T " milliseconds",
            ਅੰਕੜੇ->conn->ਬਿੱਟ.dns_resolved ? "Connection" : "Resolving",
            elapsed_ms);
      /* keep what we know about a partly done connect, e.g. TCP done
       * but TLS handshake still ongoing */
      Curl_conn_report_failed(ਅੰਕੜੇ, FIRSTSOCKET);
    }
    ਨਹੀਂਤਾਂ {
      ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
      ਜੇ(k->ਮਾਪ != -1) {
        failf(ਅੰਕੜੇ, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount, k->ਮਾਪ);
      }
      ਨਹੀਂਤਾਂ {
        failf(ਅੰਕੜੇ, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount);
      }
    }
    *ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
    ਜੇ(ਅੰਕੜੇ->conn) {
      /* Force connection closed if the connection has indeed been used */
      ਜੇ(ਅੰਕੜੇ->mstate > MSTATE_DO) {
        streamclose(ਅੰਕੜੇ->conn);
        *ਧਾਰਾ_error = TRUE;
      }
      (ਖਾਲੀ)multi_ਪੂਰਾ(ਅੰਕੜੇ, *ਨਤੀਜਾ, TRUE);
    }
    ਵਾਪਸ TRUE;
  }

  ਵਾਪਸ FALSE;
}

/*
 * We are doing protocol-specific connecting and this is being called over and
 * over from the multi interface until the connection phase is done on
 * protocol layer.
 */

ਸਥਿਰ CURLcode protocol_connecting(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  ਜੇ(conn && conn->scheme->ਲੜੀਵਾਰ->connecting) {
    *ਪੂਰਾ = FALSE;
    ਨਤੀਜਾ = conn->scheme->ਲੜੀਵਾਰ->connecting(ਅੰਕੜੇ, ਪੂਰਾ);
  }
  ਨਹੀਂਤਾਂ
    *ਪੂਰਾ = TRUE;

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * We are DOING this is being called over and over from the multi interface
 * until the DOING phase is done on protocol layer.
 */

ਸਥਿਰ CURLcode protocol_doing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  ਜੇ(conn && conn->scheme->ਲੜੀਵਾਰ->doing) {
    *ਪੂਰਾ = FALSE;
    ਨਤੀਜਾ = conn->scheme->ਲੜੀਵਾਰ->doing(ਅੰਕੜੇ, ਪੂਰਾ);
  }
  ਨਹੀਂਤਾਂ
    *ਪੂਰਾ = TRUE;

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * We have discovered that the TCP connection has been successful, we can now
 * proceed with some action.
 *
 */
ਸਥਿਰ CURLcode protocol_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *protocol_ਪੂਰਾ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(conn);
  DEBUGASSERT(protocol_ਪੂਰਾ);
  DEBUGASSERT(Curl_conn_is_connected(conn, FIRSTSOCKET));

  *protocol_ਪੂਰਾ = FALSE;
  ਜੇ(!conn->ਬਿੱਟ.protoconnstart) {
    ਜੇ(conn->scheme->ਲੜੀਵਾਰ->connect_it) {
      /* Call the protocol-specific connect function */
      ਨਤੀਜਾ = conn->scheme->ਲੜੀਵਾਰ->connect_it(ਅੰਕੜੇ, protocol_ਪੂਰਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    conn->ਬਿੱਟ.protoconnstart = TRUE;
  }

  /* Unless this protocol does not have any protocol-connect callback, as
     then we know we are done. */
  ਜੇ(!conn->scheme->ਲੜੀਵਾਰ->connecting)
    *protocol_ਪੂਰਾ = TRUE;
  ਵਾਪਸ CURLE_OK;
}

/*
 * posttransfer() is called immediately after a transfer ends
 */
ਸਥਿਰ ਖਾਲੀ multi_posttransfer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
#ਜੇ defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
  /* restore the signal handler for SIGPIPE before we get back */
  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.no_signal)
    ਸੰਕੇਤ(SIGPIPE, ਅੰਕੜੇ->state.prev_signal);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ
}

/*
 * multi_follow() handles the URL redirect magic. Pass in the 'newurl' string
 * as given by the remote server and set up the new URL to request.
 *
 * This function DOES NOT FREE the given URL.
 */
ਸਥਿਰ CURLcode multi_follow(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਬਣਤਰ Curl_scheme *handler,
                             ਅਚਲ ਅੱਖਰ *newurl, /* the Location: string */
                             followtype type) /* see transfer.h */
{
  ਜੇ(handler && handler->ਲੜੀਵਾਰ->follow)
    ਵਾਪਸ handler->ਲੜੀਵਾਰ->follow(ਅੰਕੜੇ, newurl, type);

  ਜੇ(type == FOLLOW_RETRY)
    /* Retries are generic and do not require protocol-specific redirect
       handling. */
    ਵਾਪਸ CURLE_OK;

  ਵਾਪਸ CURLE_TOO_MANY_REDIRECTS;
}

ਸਥਿਰ CURLcode mspeed_check(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(Curl_rlimit_ਸਰਗਰਮ(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit) ||
     Curl_rlimit_ਸਰਗਰਮ(&ਅੰਕੜੇ->ਤਰੱਕੀ.ul.rlimit)) {
    /* check if our send/recv limits require idle waits */
    ਅਚਲ ਬਣਤਰ curltime *pnow = Curl_pgrs_now(ਅੰਕੜੇ);
    timediff_t recv_ms, send_ms;

    send_ms = Curl_rlimit_wait_ms(&ਅੰਕੜੇ->ਤਰੱਕੀ.ul.rlimit, pnow);
    recv_ms = Curl_rlimit_wait_ms(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit, pnow);

    ਜੇ(send_ms || recv_ms) {
      ਜੇ(ਅੰਕੜੇ->mstate != MSTATE_RATELIMITING) {
        multistate(ਅੰਕੜੇ, MSTATE_RATELIMITING);
      }
      Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_TOOFAST, CURLMAX(send_ms, recv_ms), pnow);
      Curl_multi_clear_dirty(ਅੰਕੜੇ);
      CURL_TRC_M(ਅੰਕੜੇ, "[RLIMIT] waiting %" FMT_TIMEDIFF_T "ms",
                 CURLMAX(send_ms, recv_ms));
      ਵਾਪਸ CURLE_AGAIN;
    }
    ਨਹੀਂਤਾਂ {
      /* when will the rate limits increase next? The transfer needs
       * to run again at that time or it may stall. */
      send_ms = Curl_rlimit_ਅਗਲਾ_ਕਦਮ_ms(&ਅੰਕੜੇ->ਤਰੱਕੀ.ul.rlimit, pnow);
      recv_ms = Curl_rlimit_ਅਗਲਾ_ਕਦਮ_ms(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit, pnow);
      ਜੇ(send_ms || recv_ms) {
        timediff_t ਅਗਲਾ_ms = CURLMIN(send_ms, recv_ms);
        ਜੇ(!ਅਗਲਾ_ms)
          ਅਗਲਾ_ms = CURLMAX(send_ms, recv_ms);
        Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_TOOFAST, ਅਗਲਾ_ms, pnow);
        CURL_TRC_M(ਅੰਕੜੇ, "[RLIMIT] next token update in %" FMT_TIMEDIFF_T "ms",
                   ਅਗਲਾ_ms);
      }
    }
  }

  ਜੇ(ਅੰਕੜੇ->mstate != MSTATE_PERFORMING) {
    CURL_TRC_M(ਅੰਕੜੇ, "[RLIMIT] wait over, continue");
    multistate(ਅੰਕੜੇ, MSTATE_PERFORMING);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLMcode multistate_performing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       bool *ਧਾਰਾ_errorp,
                                       CURLcode *resultp)
{
  ਅੱਖਰ *newurl = ਨਿਰਾ;
  bool retry = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode ਨਤੀਜਾ = *resultp = CURLE_OK;
  *ਧਾਰਾ_errorp = FALSE;

  ਜੇ(mspeed_check(ਅੰਕੜੇ) == CURLE_AGAIN)
    ਵਾਪਸ CURLM_OK;

  /* read/write data if it is ready to do so */
  ਨਤੀਜਾ = Curl_sendrecv(ਅੰਕੜੇ);

  ਜੇ(ਅੰਕੜੇ->req.ਪੂਰਾ || (ਨਤੀਜਾ == CURLE_RECV_ERROR)) {
    /* If CURLE_RECV_ERROR happens early enough, we assume it was a race
     * condition and the server closed the reused connection exactly when we
     * wanted to use it, so figure out if that is indeed the case.
     */
    CURLcode ret = Curl_retry_request(ਅੰਕੜੇ, &newurl);
    ਜੇ(!ret)
      retry = !!newurl;
    ਨਹੀਂਤਾਂ ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = ret;

    ਜੇ(retry) {
      /* if we are to retry, set the result to OK and consider the
         request as done */
      ਨਤੀਜਾ = CURLE_OK;
      ਅੰਕੜੇ->req.ਪੂਰਾ = TRUE;
    }
  }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  ਨਹੀਂਤਾਂ ਜੇ((ਨਤੀਜਾ == CURLE_HTTP2_ਧਾਰਾ) &&
          Curl_h2_http_1_1_error(ਅੰਕੜੇ)) {
    CURLcode ret = Curl_retry_request(ਅੰਕੜੇ, &newurl);

    ਜੇ(!ret) {
      infof(ਅੰਕੜੇ, "Downgrades to HTTP/1.1");
      streamclose(ਅੰਕੜੇ->conn);
      ਅੰਕੜੇ->state.http_neg.wanted = CURL_HTTP_V1x;
      ਅੰਕੜੇ->state.http_neg.allowed = CURL_HTTP_V1x;
      /* clear the error message bit too as we ignore the one we got */
      ਅੰਕੜੇ->state.errorbuf = FALSE;
      ਜੇ(!newurl)
        /* typically for HTTP_1_1_REQUIRED error on first flight */
        newurl = Curl_bufref_dup(&ਅੰਕੜੇ->state.url);
      ਜੇ(!newurl) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      ਨਹੀਂਤਾਂ {
        /* if we are to retry, set the result to OK and consider the request
          as done */
        retry = TRUE;
        ਨਤੀਜਾ = CURLE_OK;
        ਅੰਕੜੇ->req.ਪੂਰਾ = TRUE;
      }
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = ret;
  }
#ਜੇ_ਅੰਤ

  ਜੇ(ਨਤੀਜਾ) {
    /*
     * The transfer phase returned error, we mark the connection to get closed
     * to prevent being reused. This is because we cannot possibly know if the
     * connection is in a good shape or not now. Unless it is a protocol which
     * uses two "channels" like FTP, as then the error happened in the data
     * connection.
     */

    ਜੇ(!(ਅੰਕੜੇ->conn->scheme->flags & PROTOPT_DUAL) &&
       ਨਤੀਜਾ != CURLE_HTTP2_ਧਾਰਾ)
      streamclose(ਅੰਕੜੇ->conn);

    multi_posttransfer(ਅੰਕੜੇ);
    multi_ਪੂਰਾ(ਅੰਕੜੇ, ਨਤੀਜਾ, TRUE);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->req.ਪੂਰਾ && !Curl_cwriter_is_paused(ਅੰਕੜੇ)) {
    ਅਚਲ ਬਣਤਰ Curl_scheme *handler = ਅੰਕੜੇ->conn->scheme;

    /* call this even if the readwrite function returned error */
    multi_posttransfer(ਅੰਕੜੇ);

    /* When we follow redirects or is set to retry the connection, we must to
       go back to the CONNECT state */
    ਜੇ(ਅੰਕੜੇ->req.newurl || retry) {
      followtype follow = FOLLOW_NONE;
      ਜੇ(!retry) {
        /* if the URL is a follow-location and not a retried request then
           figure out the URL here */
        curlx_free(newurl);
        newurl = ਅੰਕੜੇ->req.newurl;
        ਅੰਕੜੇ->req.newurl = ਨਿਰਾ;
        follow = FOLLOW_REDIR;
      }
      ਨਹੀਂਤਾਂ
        follow = FOLLOW_RETRY;
      (ਖਾਲੀ)multi_ਪੂਰਾ(ਅੰਕੜੇ, CURLE_OK, FALSE);
      /* multi_done() might return CURLE_GOT_NOTHING */
      ਨਤੀਜਾ = multi_follow(ਅੰਕੜੇ, handler, newurl, follow);
      ਜੇ(!ਨਤੀਜਾ) {
        multistate(ਅੰਕੜੇ, MSTATE_ਤਿਆਰੀ);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    ਨਹੀਂਤਾਂ {
      /* after the transfer is done, go DONE */

      /* but first check to see if we got a location info even though we are
         not following redirects */
      ਜੇ(ਅੰਕੜੇ->req.location) {
        curlx_free(newurl);
        newurl = ਅੰਕੜੇ->req.location;
        ਅੰਕੜੇ->req.location = ਨਿਰਾ;
        ਨਤੀਜਾ = multi_follow(ਅੰਕੜੇ, handler, newurl, FOLLOW_FAKE);
        ਜੇ(ਨਤੀਜਾ) {
          *ਧਾਰਾ_errorp = TRUE;
          ਨਤੀਜਾ = multi_ਪੂਰਾ(ਅੰਕੜੇ, ਨਤੀਜਾ, TRUE);
        }
      }

      ਜੇ(!ਨਤੀਜਾ) {
        multistate(ਅੰਕੜੇ, MSTATE_ਪੂਰਾ);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
  }
  ਨਹੀਂਤਾਂ { /* not errored, not done */
    mspeed_check(ਅੰਕੜੇ);
  }
  curlx_free(newurl);
  *resultp = ਨਤੀਜਾ;
  ਵਾਪਸ mresult;
}

ਸਥਿਰ CURLMcode multistate_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               bool *ਧਾਰਾ_errorp,
                               CURLcode *resultp)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.fprereq) {
    ਬਣਤਰ Curl_mapi_guard guard;
    ਪੂਰਨ prereq_rc;

    /* call the prerequest callback function */
    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_fprereq);
    prereq_rc = ਅੰਕੜੇ->ਸੈੱਟ.fprereq(ਅੰਕੜੇ->ਸੈੱਟ.prereq_userp,
                                  ਅੰਕੜੇ->info.primary.remote_ip,
                                  ਅੰਕੜੇ->info.primary.local_ip,
                                  ਅੰਕੜੇ->info.primary.remote_ਪੋਰਟ,
                                  ਅੰਕੜੇ->info.primary.local_ਪੋਰਟ);
    CURL_CBAPI_ਸਿਰਾ(&guard);
    ਜੇ(prereq_rc != CURL_PREREQFUNC_OK) {
      failf(ਅੰਕੜੇ, "operation aborted by pre-request callback");
      /* failure in pre-request callback - do not do any other processing */
      ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
      multi_posttransfer(ਅੰਕੜੇ);
      multi_ਪੂਰਾ(ਅੰਕੜੇ, ਨਤੀਜਾ, FALSE);
      *ਧਾਰਾ_errorp = TRUE;
      ਜਾਓ ਸਿਰਾ;
    }
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.connect_only && !ਅੰਕੜੇ->ਸੈੱਟ.connect_only_ws) {
    multistate(ਅੰਕੜੇ, MSTATE_ਪੂਰਾ);
    mresult = CURLM_CALL_MULTI_PERFORM;
  }
  ਨਹੀਂਤਾਂ {
    bool dophase_ਪੂਰਾ = FALSE;
    /* Perform the protocol's DO action */
    ਨਤੀਜਾ = multi_do(ਅੰਕੜੇ, &dophase_ਪੂਰਾ);

    ਜੇ(!ਨਤੀਜਾ) {
      ਜੇ(!dophase_ਪੂਰਾ) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
        /* some steps needed for wildcard matching */
        ਜੇ(ਅੰਕੜੇ->state.wildcardmatch) {
          ਬਣਤਰ Wildcard_ਅੰਕੜੇ *wc = ਅੰਕੜੇ->wildcard;
          ਜੇ(wc->state == CURLWC_ਪੂਰਾ || wc->state == CURLWC_SKIP) {
            /* skip some states if it is important */
            multi_ਪੂਰਾ(ਅੰਕੜੇ, CURLE_OK, FALSE);

            /* if there is no connection left, skip the DONE state */
            multistate(ਅੰਕੜੇ, ਅੰਕੜੇ->conn ? MSTATE_ਪੂਰਾ : MSTATE_COMPLETED);
            mresult = CURLM_CALL_MULTI_PERFORM;
            ਜਾਓ ਸਿਰਾ;
          }
        }
#ਜੇ_ਅੰਤ
        /* DO was not completed in one function call, we must continue
           DOING... */
        multistate(ਅੰਕੜੇ, MSTATE_DOING);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }

      /* after DO, go DO_DONE... or DO_MORE */
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->conn->ਬਿੱਟ.do_ਹੋਰ) {
        /* we are supposed to do more, but we need to sit down, relax and wait
           a little while first */
        multistate(ਅੰਕੜੇ, MSTATE_DOING_ਹੋਰ);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
      ਨਹੀਂਤਾਂ {
        /* we are done with the DO, now DID */
        multistate(ਅੰਕੜੇ, MSTATE_DID);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ((ਨਤੀਜਾ == CURLE_SEND_ERROR) &&
            ਅੰਕੜੇ->conn->ਬਿੱਟ.reuse) {
      /*
       * In this situation, a connection that we were trying to use may have
       * unexpectedly died. If possible, send the connection back to the
       * CONNECT phase so we can try again.
       */
      ਅਚਲ ਬਣਤਰ Curl_scheme *handler = ਅੰਕੜੇ->conn->scheme;
      ਅੱਖਰ *newurl = ਨਿਰਾ;
      followtype follow = FOLLOW_NONE;
      CURLcode drc;

      drc = Curl_retry_request(ਅੰਕੜੇ, &newurl);
      ਜੇ(drc) {
        /* a failure here pretty much implies an out of memory */
        ਨਤੀਜਾ = drc;
        *ਧਾਰਾ_errorp = TRUE;
      }

      multi_posttransfer(ਅੰਕੜੇ);
      drc = multi_ਪੂਰਾ(ਅੰਕੜੇ, ਨਤੀਜਾ, FALSE);

      /* When set to retry the connection, we must go back to the CONNECT
       * state */
      ਜੇ(newurl) {
        ਜੇ(!drc || (drc == CURLE_SEND_ERROR)) {
          follow = FOLLOW_RETRY;
          drc = multi_follow(ਅੰਕੜੇ, handler, newurl, follow);
          ਜੇ(!drc) {
            multistate(ਅੰਕੜੇ, MSTATE_ਤਿਆਰੀ);
            mresult = CURLM_CALL_MULTI_PERFORM;
            ਨਤੀਜਾ = CURLE_OK;
          }
          ਨਹੀਂਤਾਂ {
            /* Follow failed */
            ਨਤੀਜਾ = drc;
          }
        }
        ਨਹੀਂਤਾਂ {
          /* done did not return OK or SEND_ERROR */
          ਨਤੀਜਾ = drc;
        }
      }
      ਨਹੀਂਤਾਂ {
        /* Have error handler disconnect conn if we cannot retry */
        *ਧਾਰਾ_errorp = TRUE;
      }
      curlx_free(newurl);
    }
    ਨਹੀਂਤਾਂ {
      /* failure detected */
      multi_posttransfer(ਅੰਕੜੇ);
      ਜੇ(ਅੰਕੜੇ->conn)
        multi_ਪੂਰਾ(ਅੰਕੜੇ, ਨਤੀਜਾ, FALSE);
      *ਧਾਰਾ_errorp = TRUE;
    }
  }
ਸਿਰਾ:
  *resultp = ਨਤੀਜਾ;
  ਵਾਪਸ mresult;
}

ਸਥਿਰ CURLMcode multistate_ratelimiting(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         CURLcode *resultp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  CURLMcode mresult = CURLM_OK;
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  /* if both rates are within spec, resume transfer */
  ਨਤੀਜਾ = Curl_pgrsCheck(ਅੰਕੜੇ);

  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(!(ਅੰਕੜੇ->conn->scheme->flags & PROTOPT_DUAL) &&
       ਨਤੀਜਾ != CURLE_HTTP2_ਧਾਰਾ)
      streamclose(ਅੰਕੜੇ->conn);

    multi_posttransfer(ਅੰਕੜੇ);
    multi_ਪੂਰਾ(ਅੰਕੜੇ, ਨਤੀਜਾ, TRUE);
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(!mspeed_check(ਅੰਕੜੇ))
      mresult = CURLM_CALL_MULTI_PERFORM;
  }
  *resultp = ਨਤੀਜਾ;
  ਵਾਪਸ mresult;
}

ਸਥਿਰ CURLMcode multistate_connect(ਬਣਤਰ Curl_multi *multi,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    CURLcode *resultp)
{
  /* Connect. We want to get a connection identifier filled in. This state can
     be entered from SETUP and from PENDING. */
  bool connected;
  CURLMcode mresult = CURLM_OK;
  CURLcode ਨਤੀਜਾ = Curl_connect(ਅੰਕੜੇ, &connected);
  ਜੇ(ਨਤੀਜਾ == CURLE_NO_CONNECTION_ਉਪਲਬਧ) {
    /* There was no connection available. We will go to the pending state and
       wait for an available connection. */
    multistate(ਅੰਕੜੇ, MSTATE_PENDING);
    /* move from process to pending set */
    Curl_uint32_bset_remove(&multi->process, ਅੰਕੜੇ->mid);
    Curl_uint32_bset_remove(&multi->dirty, ਅੰਕੜੇ->mid);
    Curl_uint32_bset_ਜੋੜੋ(&multi->pending, ਅੰਕੜੇ->mid);
    *resultp = CURLE_OK;
    ਵਾਪਸ mresult;
  }
  ਨਹੀਂਤਾਂ
    multi_ਸਮਾਂ_ਸਾਰਣੀ_pending(ਅੰਕੜੇ->multi);

  ਜੇ(!ਨਤੀਜਾ) {
    /* after the connect has been sent off, go WAITCONNECT unless the
       protocol connect is already done and we can go directly to WAITDO or
       DO! */
    mresult = CURLM_CALL_MULTI_PERFORM;

    ਜੇ(connected) {
      ਜੇ(!ਅੰਕੜੇ->conn->ਬਿੱਟ.reuse &&
         Curl_conn_is_multiplex(ਅੰਕੜੇ->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_ਸਮਾਂ_ਸਾਰਣੀ_pending(ਅੰਕੜੇ->multi);
      }
      multistate(ਅੰਕੜੇ, MSTATE_PROTOCONNECT);
    }
    ਨਹੀਂਤਾਂ {
      multistate(ਅੰਕੜੇ, MSTATE_CONNECTING);
    }
  }
  *resultp = ਨਤੀਜਾ;
  ਵਾਪਸ mresult;
}

/* returns the possibly updated result */
ਸਥਿਰ CURLcode is_ਮੁਕੰਮਲ(ਬਣਤਰ Curl_multi *multi,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅਚਲ ਬਣਤਰ curltime *pnow,
                            bool ਧਾਰਾ_error,
                            CURLcode ਨਤੀਜਾ)
{
  ਜੇ(ਅੰਕੜੇ->mstate < MSTATE_COMPLETED) {
    ਜੇ(ਨਤੀਜਾ) {
      /*
       * If an error was returned, and we are not in completed state now,
       * then we go to completed and consider this transfer aborted.
       */

      /* No attempt to disconnect connections must be made before this -
         connection detach and termination happens only here */

      /* Check if we can move pending requests to send pipe */
      multi_ਸਮਾਂ_ਸਾਰਣੀ_pending(multi); /* connection */

      ਜੇ(ਅੰਕੜੇ->conn) {
        ਜੇ(ਧਾਰਾ_error) {
          /* Do not attempt to send data over a connection that timed out */
          bool dead_connection = ਨਤੀਜਾ == CURLE_OPERATION_TIMEDOUT;
          ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

          /* This is where we make sure that the conn pointer is reset.
             We do not have to do this in every case block above where a
             failure is detected */
          Curl_detach_connection(ਅੰਕੜੇ);
          Curl_conn_close(ਅੰਕੜੇ, conn, dead_connection);
        }
      }
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->mstate == MSTATE_CONNECT) {
        /* Curl_connect() failed */
        multi_posttransfer(ਅੰਕੜੇ);
        Curl_pgrsUpdate_nometer(ਅੰਕੜੇ);
      }

      multistate(ਅੰਕੜੇ, MSTATE_COMPLETED);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    /* if there is still a connection to use, call the progress function */
    ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->conn && Curl_conn_is_connected(ਅੰਕੜੇ->conn, FIRSTSOCKET)) {
      ਨਤੀਜਾ = Curl_pgrsUpdateX(ਅੰਕੜੇ, pnow);
      ਜੇ(ਨਤੀਜਾ) {
        /* aborted due to progress callback return code must close the
           connection */
        streamclose(ਅੰਕੜੇ->conn);

        /* if not yet in DONE state, go there. */
        ਜੇ(ਅੰਕੜੇ->mstate < MSTATE_ਪੂਰਾ)
          multistate(ਅੰਕੜੇ, MSTATE_ਪੂਰਾ);
        ਵਾਪਸ ਨਤੀਜਾ;
      }
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ handle_completed(ਬਣਤਰ Curl_multi *multi,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             CURLcode ਨਤੀਜਾ)
{
  bool ਸੁਨੇਹਾ_ਮੰਜ਼ਿਲ_app = ਅੰਕੜੇ->master_mid == UINT32_MAX;

  ਜੇ(!ਸੁਨੇਹਾ_ਮੰਜ਼ਿਲ_app) {
    /* A sub transfer, not reported to the application. Is anyone still
     * interested in processing its results? */
    ਜੇ(ਅੰਕੜੇ->sub_xfer_ਪੂਰਾ) {
      ਬਣਤਰ Curl_easy *master = Curl_multi_get_easy(multi, ਅੰਕੜੇ->master_mid);

      CURL_TRC_M(ਅੰਕੜੇ, "sub xfer done for master %u", ਅੰਕੜੇ->master_mid);
      ਜੇ(master)
        ਅੰਕੜੇ->sub_xfer_ਪੂਰਾ(ਅੰਕੜੇ, master, ਨਤੀਜਾ);
      ਨਹੀਂਤਾਂ
        CURL_TRC_M(ਅੰਕੜੇ, "master easy %u already gone.", ਅੰਕੜੇ->master_mid);
    }
  }
  ਨਹੀਂਤਾਂ {
    /* now fill in the CURLMsg with this info */
    ਬਣਤਰ CURLMsg *ਸੁਨੇਹਾ = &ਅੰਕੜੇ->ਸੁਨੇਹਾ;

    ਸੁਨੇਹਾ->ਸੁਨੇਹਾ = CURLMSG_ਪੂਰਾ;
    ਸੁਨੇਹਾ->easy_handle = ਅੰਕੜੇ;
    ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਨਤੀਜਾ = ਨਤੀਜਾ;

    DEBUGASSERT(!ਅੰਕੜੇ->conn);
  }
  multistate(ਅੰਕੜੇ, MSTATE_MSGSENT);

  /* remove from the other sets */
  Curl_uint32_bset_remove(&multi->process, ਅੰਕੜੇ->mid);
  Curl_uint32_bset_remove(&multi->dirty, ਅੰਕੜੇ->mid);
  Curl_uint32_bset_remove(&multi->pending, ਅੰਕੜੇ->mid);
  ਜੇ(ਸੁਨੇਹਾ_ਮੰਜ਼ਿਲ_app)
    multi_addmsg(multi, ਅੰਕੜੇ);
  ਜੇ(ਅੰਕੜੇ->state.really_alive) {
    ਅੰਕੜੇ->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    ਜੇ(!multi->xfers_really_alive)
      (ਖਾਲੀ)multi_assess_wakeup(multi);
  }
  --multi->xfers_alive;
  ਜੇ(!multi->xfers_alive)
    multi_assess_wakeup(multi);
}

ਸਥਿਰ CURLMcode multistate_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLcode *ਨਤੀਜਾ)
{
  ਜੇ(!ਅੰਕੜੇ->state.really_alive) {
    ਅੰਕੜੇ->state.really_alive = TRUE;
    ++ਅੰਕੜੇ->multi->xfers_really_alive;
    ਜੇ(ਅੰਕੜੇ->multi->xfers_really_alive == 1) {
      CURLMcode mresult = multi_assess_wakeup(ਅੰਕੜੇ->multi);
      ਜੇ(mresult) {
        failf(ਅੰਕੜੇ, "error enabling wakeup listening: %d", mresult);
        ਵਾਪਸ mresult;
      }
    }
  }

  *ਨਤੀਜਾ = Curl_pretransfer(ਅੰਕੜੇ);
  ਜੇ(*ਨਤੀਜਾ)
    ਵਾਪਸ CURLM_OK;

  /* after init, go SETUP */
  multistate(ਅੰਕੜੇ, MSTATE_ਤਿਆਰੀ);
  Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_STARTOP);
  ਵਾਪਸ CURLM_CALL_MULTI_PERFORM;
}

ਸਥਿਰ CURLMcode multistate_ਤਿਆਰੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਬਣਤਰ curltime *pnow = Curl_pgrs_now(ਅੰਕੜੇ);
  Curl_pgrs_ਸਮਾਂ_Was(ਅੰਕੜੇ, TIMER_STARTSINGLE, *pnow);
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.timeout)
    Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_TIMEOUT, ਅੰਕੜੇ->ਸੈੱਟ.timeout, pnow);
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.connecttimeout)
    /* Since a connection might go to pending and back to CONNECT several
       times before it actually takes off, we need to set the timeout once
       in SETUP before we enter CONNECT the first time. */
    Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_CONNECTTIMEOUT,
                    ਅੰਕੜੇ->ਸੈੱਟ.connecttimeout, pnow);

  multistate(ਅੰਕੜੇ, MSTATE_CONNECT);
  ਵਾਪਸ CURLM_CALL_MULTI_PERFORM;
}

ਸਥਿਰ CURLMcode multistate_connecting(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       bool *ਧਾਰਾ_error,
                                       CURLcode *ਨਤੀਜਾ)
{
  bool connected;

  ਜੇ(!ਅੰਕੜੇ->conn) {
    DEBUGASSERT(0);
    *ਨਤੀਜਾ = CURLE_FAILED_INIT;
    ਵਾਪਸ CURLM_OK;
  }
  ਜੇ(!Curl_xfer_recv_is_paused(ਅੰਕੜੇ)) {
    *ਨਤੀਜਾ = Curl_conn_connect(ਅੰਕੜੇ, FIRSTSOCKET, FALSE, &connected);
    ਜੇ(connected && !*ਨਤੀਜਾ) {
      ਜੇ(!ਅੰਕੜੇ->conn->ਬਿੱਟ.reuse &&
         Curl_conn_is_multiplex(ਅੰਕੜੇ->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_ਸਮਾਂ_ਸਾਰਣੀ_pending(ਅੰਕੜੇ->multi);
      }
      multistate(ਅੰਕੜੇ, MSTATE_PROTOCONNECT);
      ਵਾਪਸ CURLM_CALL_MULTI_PERFORM;
    }
    ਨਹੀਂਤਾਂ ਜੇ(*ਨਤੀਜਾ) {
      /* failure detected */
      CURL_TRC_M(ਅੰਕੜੇ, "connect failed -> %d", (ਪੂਰਨ)*ਨਤੀਜਾ);
      multi_posttransfer(ਅੰਕੜੇ);
      multi_ਪੂਰਾ(ਅੰਕੜੇ, *ਨਤੀਜਾ, TRUE);
      *ਧਾਰਾ_error = TRUE;
      ਵਾਪਸ CURLM_OK;
    }
  }
  ਵਾਪਸ CURLM_OK;
}

ਸਥਿਰ CURLMcode multistate_protoconnect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         bool *ਧਾਰਾ_error,
                                         CURLcode *ਨਤੀਜਾ)
{
  bool protocol_connected = FALSE;

  ਜੇ(!*ਨਤੀਜਾ && ਅੰਕੜੇ->conn->ਬਿੱਟ.reuse) {
    /* ftp seems to hang when protoconnect on reused connection since we
     * handle PROTOCONNECT in general inside the filters, it seems wrong to
     * restart this on a reused connection.
     */
    multistate(ਅੰਕੜੇ, MSTATE_DO);
    ਵਾਪਸ CURLM_CALL_MULTI_PERFORM;
  }
  ਜੇ(!*ਨਤੀਜਾ)
    *ਨਤੀਜਾ = protocol_connect(ਅੰਕੜੇ, &protocol_connected);
  ਜੇ(!*ਨਤੀਜਾ && !protocol_connected) {
    /* switch to waiting state */
    multistate(ਅੰਕੜੇ, MSTATE_PROTOCONNECTING);
    ਵਾਪਸ CURLM_CALL_MULTI_PERFORM;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!*ਨਤੀਜਾ) {
    /* protocol connect has completed, go WAITDO or DO */
    multistate(ਅੰਕੜੇ, MSTATE_DO);
    ਵਾਪਸ CURLM_CALL_MULTI_PERFORM;
  }

  /* failure detected */
  multi_posttransfer(ਅੰਕੜੇ);
  multi_ਪੂਰਾ(ਅੰਕੜੇ, *ਨਤੀਜਾ, TRUE);
  *ਧਾਰਾ_error = TRUE;
  ਵਾਪਸ CURLM_OK;
}

ਸਥਿਰ CURLMcode multistate_protoconnecting(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            bool *ਧਾਰਾ_error,
                                            CURLcode *ਨਤੀਜਾ)
{
  bool protocol_connected = FALSE;

  /* protocol-specific connect phase */
  *ਨਤੀਜਾ = protocol_connecting(ਅੰਕੜੇ, &protocol_connected);
  ਜੇ(!*ਨਤੀਜਾ && protocol_connected) {
    /* after the connect has completed, go WAITDO or DO */
    multistate(ਅੰਕੜੇ, MSTATE_DO);
    ਵਾਪਸ CURLM_CALL_MULTI_PERFORM;
  }
  ਨਹੀਂਤਾਂ ਜੇ(*ਨਤੀਜਾ) {
    /* failure detected */
    multi_posttransfer(ਅੰਕੜੇ);
    multi_ਪੂਰਾ(ਅੰਕੜੇ, *ਨਤੀਜਾ, TRUE);
    *ਧਾਰਾ_error = TRUE;
  }
  ਵਾਪਸ CURLM_OK;
}

ਸਥਿਰ CURLMcode multistate_doing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  bool *ਧਾਰਾ_error,
                                  CURLcode *ਨਤੀਜਾ)
{
  bool dophase_ਪੂਰਾ = FALSE;

  /* we continue DOING until the DO phase is complete */
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  *ਨਤੀਜਾ = protocol_doing(ਅੰਕੜੇ, &dophase_ਪੂਰਾ);
  ਜੇ(!*ਨਤੀਜਾ) {
    ਜੇ(dophase_ਪੂਰਾ) {
      /* after DO, go DO_DONE or DO_MORE */
      multistate(ਅੰਕੜੇ, ਅੰਕੜੇ->conn->ਬਿੱਟ.do_ਹੋਰ ?
                 MSTATE_DOING_ਹੋਰ : MSTATE_DID);
      ਵਾਪਸ CURLM_CALL_MULTI_PERFORM;
    } /* dophase_done */
  }
  ਨਹੀਂਤਾਂ {
    /* failure detected */
    multi_posttransfer(ਅੰਕੜੇ);
    multi_ਪੂਰਾ(ਅੰਕੜੇ, *ਨਤੀਜਾ, FALSE);
    *ਧਾਰਾ_error = TRUE;
  }
  ਵਾਪਸ CURLM_OK;
}

ਸਥਿਰ CURLMcode multistate_doing_ਹੋਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       bool *ਧਾਰਾ_error,
                                       CURLcode *ਨਤੀਜਾ)
{
  domore control;

  /*
   * When we are connected, DOING MORE and then go DID
   */
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  *ਨਤੀਜਾ = multi_do_ਹੋਰ(ਅੰਕੜੇ, &control);

  ਜੇ(!*ਨਤੀਜਾ) {
    ਜੇ(control != DOMORE_INCOMPLETE) {
      /* if DONE, advance to DO_DONE
         if GOBACK, go back to DOING */
      multistate(ਅੰਕੜੇ, control == DOMORE_ਪੂਰਾ ? MSTATE_DID : MSTATE_DOING);
      ਵਾਪਸ CURLM_CALL_MULTI_PERFORM;
    }
    /* else
       stay in DO_MORE */
  }
  ਨਹੀਂਤਾਂ {
    /* failure detected */
    multi_posttransfer(ਅੰਕੜੇ);
    multi_ਪੂਰਾ(ਅੰਕੜੇ, *ਨਤੀਜਾ, FALSE);
    *ਧਾਰਾ_error = TRUE;
  }
  ਵਾਪਸ CURLM_OK;
}

ਸਥਿਰ CURLMcode multistate_did(ਬਣਤਰ Curl_multi *multi,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  DEBUGASSERT(ਅੰਕੜੇ->conn);
  ਜੇ(ਅੰਕੜੇ->conn->ਬਿੱਟ.multiplex)
    /* Check if we can move pending requests to send pipe */
    multi_ਸਮਾਂ_ਸਾਰਣੀ_pending(multi); /* multiplexed */

  /* Only perform the transfer if there is a good socket to work with.
     Having both BAD is a signal to skip immediately to DONE */
  ਜੇ(CONN_SOCK_IDX_VALID(ਅੰਕੜੇ->conn->recv_idx) ||
     CONN_SOCK_IDX_VALID(ਅੰਕੜੇ->conn->send_idx)) {
    multistate(ਅੰਕੜੇ, MSTATE_PERFORMING);
    /* Do not return CURLM_CALL_MULTI_PERFORM to give other transfers
     * a chance to send off their requests.
     * Note: Some SFTP handlers do not seem to like this.
     *       Restrict it to HTTP families. */
    ਵਾਪਸ ((multi->xfers_alive > 1) &&
            (ਅੰਕੜੇ->conn->scheme->protocol & PROTO_FAMILY_HTTP)) ?
           CURLM_OK : CURLM_CALL_MULTI_PERFORM;
  }
  ਨਹੀਂਤਾਂ {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
    ਜੇ(ਅੰਕੜੇ->state.wildcardmatch &&
       ((ਅੰਕੜੇ->conn->scheme->flags & PROTOPT_WILDCARD) == 0)) {
      ਅੰਕੜੇ->wildcard->state = CURLWC_ਪੂਰਾ;
    }
#ਜੇ_ਅੰਤ
    multistate(ਅੰਕੜੇ, MSTATE_ਪੂਰਾ);
    ਵਾਪਸ CURLM_CALL_MULTI_PERFORM;
  }
}

ਸਥਿਰ CURLMcode multistate_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLcode *presult)
{
  ਜੇ(ਅੰਕੜੇ->conn) {
    CURLcode ਨਤੀਜਾ;

    /* post-transfer command */
    ਨਤੀਜਾ = multi_ਪੂਰਾ(ਅੰਕੜੇ, *presult, FALSE);

    /* allow a previously set error code take precedence */
    ਜੇ(!(*presult))
      *presult = ਨਤੀਜਾ;
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  ਜੇ(ਅੰਕੜੇ->state.wildcardmatch) {
    ਜੇ(ਅੰਕੜੇ->wildcard->state != CURLWC_ਪੂਰਾ) {
      /* if a wildcard is set and we are not ending -> lets start again
         with MSTATE_INIT */
      multistate(ਅੰਕੜੇ, MSTATE_INIT);
      ਵਾਪਸ CURLM_CALL_MULTI_PERFORM;
    }
  }
#ਜੇ_ਅੰਤ
  /* after we have DONE what we are supposed to do, go COMPLETED, and
     it does not matter what the multi_done() returned! */
  multistate(ਅੰਕੜੇ, MSTATE_COMPLETED);
  ਵਾਪਸ CURLM_CALL_MULTI_PERFORM;
}

ਸਥਿਰ CURLMcode multi_runsingle(ਬਣਤਰ Curl_multi *multi,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ Curl_sigpipe_ctx *sigpipe_ctx)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਬਣਤਰ curltime *pnow = ਨਿਰਾ;

  ਜੇ(multi->dead) {
    /* a multi-level callback returned error before, meaning every individual
     transfer now has failed */
    ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
    multi_posttransfer(ਅੰਕੜੇ);
    multi_ਪੂਰਾ(ਅੰਕੜੇ, ਨਤੀਜਾ, FALSE);
    multistate(ਅੰਕੜੇ, MSTATE_COMPLETED);
  }

  multi_warn_debug(multi, ਅੰਕੜੇ);

  /* transfer runs now, clear the dirty bit. This may be set
   * again during processing, triggering a re-run later. */
  Curl_uint32_bset_remove(&multi->dirty, ਅੰਕੜੇ->mid);

  ਜੇ(ਅੰਕੜੇ == multi->admin) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_ਅੰਦਰੂਨੀ_WAKEUP
    /* Consume any pending wakeup signals before processing.
     * This is necessary for event based processing. See #21547 */
    (ਖਾਲੀ)Curl_wakeup_consume(multi->wakeup_ਅੰਦਰੂਨੀ, TRUE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_THREADED
    Curl_async_thrdd_multi_process(multi);
#ਜੇ_ਅੰਤ
    Curl_cshutdn_perform(&multi->cshutdn, multi->admin, sigpipe_ctx);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  sigpipe_apply(ਅੰਕੜੇ, sigpipe_ctx);
  ਕਰੋ {
    /* A "stream" here is a logical stream if the protocol can handle that
       (HTTP/2), or the full connection for older protocols */
    bool ਧਾਰਾ_error = FALSE;
    mresult = CURLM_OK;
    pnow = ਨਿਰਾ;

    ਜੇ(multi_ischanged(multi, TRUE)) {
      CURL_TRC_M(ਅੰਕੜੇ, "multi changed, check CONNECT_PEND queue");
      multi_ਸਮਾਂ_ਸਾਰਣੀ_pending(multi); /* multiplexed */
    }

    ਜੇ(ਅੰਕੜੇ->mstate > MSTATE_CONNECT &&
       ਅੰਕੜੇ->mstate < MSTATE_COMPLETED) {
      /* Make sure we set the connection's current owner */
      DEBUGASSERT(ਅੰਕੜੇ->conn);
      ਜੇ(!ਅੰਕੜੇ->conn) {
        mresult = CURLM_ਅੰਦਰੂਨੀ_ERROR;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }

    /* Wait for the connect state as only then is the start time stored, but
       we must not check already completed handles */
    ਜੇ((ਅੰਕੜੇ->mstate >= MSTATE_CONNECT) && (ਅੰਕੜੇ->mstate < MSTATE_COMPLETED)) {
      pnow = Curl_pgrs_now(ਅੰਕੜੇ);
      ਜੇ(multi_handle_timeout(ਅੰਕੜੇ, pnow, &ਧਾਰਾ_error, &ਨਤੀਜਾ))
        /* Skip the statemachine and go directly to error handling section. */
        ਜਾਓ statemachine_ਸਿਰਾ;
      pnow = ਨਿਰਾ;
    }

    ਚੋਣ(ਅੰਕੜੇ->mstate) {
    ਹਾਲਤ MSTATE_INIT:
      /* Transitional state. init this transfer. A handle never comes back to
         this state. */
      mresult = multistate_init(ਅੰਕੜੇ, &ਨਤੀਜਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_ਤਿਆਰੀ:
      /* Transitional state. Setup things for a new transfer. The handle
         can come back to this state on a redirect. */
      mresult = multistate_ਤਿਆਰੀ(ਅੰਕੜੇ);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_CONNECT:
      mresult = multistate_connect(multi, ਅੰਕੜੇ, &ਨਤੀਜਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_CONNECTING:
      /* awaiting a completion of an asynch TCP connect */
      mresult = multistate_connecting(ਅੰਕੜੇ, &ਧਾਰਾ_error, &ਨਤੀਜਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_PROTOCONNECT:
      mresult = multistate_protoconnect(ਅੰਕੜੇ, &ਧਾਰਾ_error, &ਨਤੀਜਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_PROTOCONNECTING:
      /* protocol-specific connect phase */
      mresult = multistate_protoconnecting(ਅੰਕੜੇ, &ਧਾਰਾ_error, &ਨਤੀਜਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_DO:
      mresult = multistate_do(ਅੰਕੜੇ, &ਧਾਰਾ_error, &ਨਤੀਜਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_DOING:
      /* we continue DOING until the DO phase is complete */
      mresult = multistate_doing(ਅੰਕੜੇ, &ਧਾਰਾ_error, &ਨਤੀਜਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_DOING_ਹੋਰ:
      /*
       * When we are connected, DOING MORE and then go DID
       */
      mresult = multistate_doing_ਹੋਰ(ਅੰਕੜੇ, &ਧਾਰਾ_error, &ਨਤੀਜਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_DID:
      mresult = multistate_did(multi, ਅੰਕੜੇ);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_RATELIMITING: /* limit-rate exceeded in either direction */
      mresult = multistate_ratelimiting(ਅੰਕੜੇ, &ਨਤੀਜਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_PERFORMING:
      mresult = multistate_performing(ਅੰਕੜੇ, &ਧਾਰਾ_error, &ਨਤੀਜਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_ਪੂਰਾ:
      mresult = multistate_ਪੂਰਾ(ਅੰਕੜੇ, &ਨਤੀਜਾ);
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_COMPLETED:
      ਰੋਕੋ;

    ਹਾਲਤ MSTATE_PENDING:
    ਹਾਲਤ MSTATE_MSGSENT:
      /* handles in these states should NOT be in this list */
      ਰੋਕੋ;

    ਮੂਲ:
      mresult = CURLM_ਅੰਦਰੂਨੀ_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਜੇ(ਅੰਕੜੇ->mstate >= MSTATE_CONNECT &&
       ਅੰਕੜੇ->mstate < MSTATE_DO &&
       mresult != CURLM_CALL_MULTI_PERFORM &&
       !multi_ischanged(multi, FALSE)) {
      /* We now handle stream timeouts if and only if this will be the last
       * loop iteration. We only check this on the last iteration to ensure
       * that if we know we have additional work to do immediately
       * (i.e. CURLM_CALL_MULTI_PERFORM == TRUE) then we should do that before
       * declaring the connection timed out as we may almost have a completed
       * connection. */
      pnow = Curl_pgrs_now(ਅੰਕੜੇ);
      multi_handle_timeout(ਅੰਕੜੇ, pnow, &ਧਾਰਾ_error, &ਨਤੀਜਾ);
    }

statemachine_ਸਿਰਾ:
    ਜੇ(!pnow)
      pnow = Curl_pgrs_now(ਅੰਕੜੇ);
    ਨਤੀਜਾ = is_ਮੁਕੰਮਲ(multi, ਅੰਕੜੇ, pnow, ਧਾਰਾ_error, ਨਤੀਜਾ);
    ਜੇ(ਨਤੀਜਾ)
      mresult = CURLM_CALL_MULTI_PERFORM;

    ਜੇ(MSTATE_COMPLETED == ਅੰਕੜੇ->mstate) {
      handle_completed(multi, ਅੰਕੜੇ, ਨਤੀਜਾ);
      mresult = CURLM_OK;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  } ਜਦੋਂ((mresult == CURLM_CALL_MULTI_PERFORM) ||
          multi_ischanged(multi, FALSE));

ਬਾਹਰਲਾ:
  ਅੰਕੜੇ->ਨਤੀਜਾ = ਨਤੀਜਾ;
  ਵਾਪਸ mresult;
}

ਸਥਿਰ CURLMcode multi_perform(ਬਣਤਰ Curl_multi *multi,
                               ਪੂਰਨ *running_handles)
{
  CURLMcode returncode = CURLM_OK;
  ਬਣਤਰ curltime ਸ਼ੁਰੂ;
  uint32_t mid;
  ਬਣਤਰ Curl_sigpipe_ctx sigpipe_ctx;

  curlx_pnow(&ਸ਼ੁਰੂ);
  sigpipe_init(&sigpipe_ctx);

  ਜੇ(Curl_uint32_bset_ਪਹਿਲਾ(&multi->process, &mid)) {
    CURL_TRC_M(multi->admin, "multi_perform(running=%u)",
               multi_xfers_running(multi));
    ਕਰੋ {
      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);
      CURLMcode mresult;
      ਜੇ(!ਅੰਕੜੇ) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        ਜਾਰੀ;
      }
      mresult = multi_runsingle(multi, ਅੰਕੜੇ, &sigpipe_ctx);
      ਜੇ(mresult)
        returncode = mresult;
    } ਜਦੋਂ(Curl_uint32_bset_ਅਗਲਾ(&multi->process, mid, &mid));
  }
  sigpipe_restore(&sigpipe_ctx);

  ਜੇ(multi_ischanged(multi, TRUE))
    multi_ਸਮਾਂ_ਸਾਰਣੀ_pending(multi);

  ਜੇ(!returncode && CURL_MNTFY_HAS_ENTRIES(multi))
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
  ਜਦੋਂ(Curl_timeouts_remove_expired(&multi->timeouts, &ਸ਼ੁਰੂ, &mid)) {
    /* the removed may have another timeout in queue */
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);
    ਜੇ(!ਅੰਕੜੇ) {
      DEBUGASSERT(0);
      ਜਾਰੀ;
    }
    (ਖਾਲੀ)ਜੋੜੋ_ਅਗਲਾ_timeout(&ਸ਼ੁਰੂ, multi, ਅੰਕੜੇ);
    ਜੇ(ਅੰਕੜੇ->mstate == MSTATE_PENDING) {
      bool ਧਾਰਾ_ਅਣਵਰਤਿਆ;
      CURLcode ਨਤੀਜਾ_ਅਣਵਰਤਿਆ;
      ਜੇ(multi_handle_timeout(ਅੰਕੜੇ, Curl_pgrs_now(ਅੰਕੜੇ),
                              &ਧਾਰਾ_ਅਣਵਰਤਿਆ, &ਨਤੀਜਾ_ਅਣਵਰਤਿਆ)) {
        infof(ਅੰਕੜੇ, "PENDING handle timeout");
        move_pending_ਮੰਜ਼ਿਲ_connect(multi, ਅੰਕੜੇ);
      }
    }
  }

  ਜੇ(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? (ਪੂਰਨ)running : ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
  }

  ਜੇ(CURLM_OK >= returncode)
    returncode = Curl_update_timer(multi);

  ਵਾਪਸ returncode;
}

CURLMcode curl_multi_perform(CURLM *m, ਪੂਰਨ *running_handles)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_perform, &mresult)) {
    mresult = multi_perform(m, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

CURLMcode curl_multi_cleanup(CURLM *m)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_cleanup, &mresult)) {
    ਬਣਤਰ Curl_multi *multi = m;
    ਖਾਲੀ *ਇੰਦਰਾਜ;
    uint32_t mid;

    /* First remove all remaining easy handles,
     * close internal ones. admin handle is special */
    ਜੇ(Curl_uint32_tbl_ਪਹਿਲਾ(&multi->xfers, &mid, &ਇੰਦਰਾਜ)) {
      ਕਰੋ {
        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = ਇੰਦਰਾਜ;
        ਜੇ(!GOOD_EASY_HANDLE(ਅੰਕੜੇ)) {
          mresult = CURLM_BAD_HANDLE;
          ਜਾਓ ਬਾਹਰਲਾ;
        }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
        ਜੇ(mid != ਅੰਕੜੇ->mid) {
          CURL_TRC_M(ਅੰਕੜੇ, "multi_cleanup: still present with mid=%u, "
                     "but unexpected data->mid=%u\n", mid, ਅੰਕੜੇ->mid);
          DEBUGASSERT(0);
        }
#ਜੇ_ਅੰਤ

        ਜੇ(ਅੰਕੜੇ == multi->admin)
          ਜਾਰੀ;

        ਜੇ(!ਅੰਕੜੇ->state.ਪੂਰਾ && ਅੰਕੜੇ->conn)
          /* if DONE was never called for this handle */
          (ਖਾਲੀ)multi_ਪੂਰਾ(ਅੰਕੜੇ, CURLE_OK, TRUE);

        ਅੰਕੜੇ->multi = ਨਿਰਾ; /* clear the association */
        Curl_uint32_tbl_remove(&multi->xfers, mid);
        ਅੰਕੜੇ->mid = UINT32_MAX;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
        ਜੇ(ਅੰਕੜੇ->psl == &multi->psl)
          ਅੰਕੜੇ->psl = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
        ਜੇ(ਅੰਕੜੇ->state.ਅੰਦਰੂਨੀ)
          Curl_close(&ਅੰਕੜੇ);
      } ਜਦੋਂ(Curl_uint32_tbl_ਅਗਲਾ(&multi->xfers, mid, &mid, &ਇੰਦਰਾਜ));
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_THREADED
    Curl_async_thrdd_multi_destroy(multi, !multi->quick_exit);
#ਜੇ_ਅੰਤ
    Curl_cpool_destroy(&multi->cpool, multi->admin);
    Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
    ਜੇ(multi->admin) {
      CURL_TRC_M(multi->admin, "multi_cleanup, closing admin handle, done");
      multi->admin->multi = ਨਿਰਾ;
      Curl_uint32_tbl_remove(&multi->xfers, multi->admin->mid);
      Curl_close(&multi->admin);
    }

    multi->magic = 0; /* not good anymore */

    Curl_multi_ev_cleanup(multi);
    Curl_hash_destroy(&multi->proto_hash);
    Curl_dnscache_destroy(&multi->dnscache);
    Curl_psl_destroy(&multi->psl);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
    Curl_ssl_scache_destroy(multi->ssl_scache);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
    WSACloseEvent(multi->wsa_event);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_ਅੰਦਰੂਨੀ_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_ਅੰਦਰੂਨੀ);
#ਜੇ_ਅੰਤ

    multi_xfer_bufs_free(multi);
    Curl_mntfy_cleanup(multi);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    ਜੇ(Curl_uint32_tbl_ਗਿਣਤੀ(&multi->xfers)) {
      multi_xfer_tbl_dump(multi);
      DEBUGASSERT(0);
    }
#ਜੇ_ਅੰਤ
    Curl_uint32_bset_destroy(&multi->process);
    Curl_uint32_bset_destroy(&multi->dirty);
    Curl_uint32_bset_destroy(&multi->pending);
    Curl_uint32_bset_destroy(&multi->msgsent);
    Curl_uint32_tbl_destroy(&multi->xfers);
    curlx_memzero(multi, ਆਕਾਰ(*multi));
    curlx_free(multi);

    mresult = CURLM_OK;
  }
ਬਾਹਰਲਾ:
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

/*
 * curl_multi_info_read()
 *
 * This function is the primary way for a multi/multi_socket application to
 * figure out if a transfer has ended.
 */

CURLMsg *curl_multi_info_read(CURLM *m, ਪੂਰਨ *msgs_ਵਿੱਚ_ਕਤਾਰ)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMsg *ਸੁਨੇਹਾ_ਨਤੀਜਾ = ਨਿਰਾ;

  *msgs_ਵਿੱਚ_ਕਤਾਰ = 0; /* default to none */
  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_info_read, ਨਿਰਾ)) {
    ਬਣਤਰ Curl_multi *multi = m;
    uint32_t mid;
    ਜੇ(Curl_uint32_bset_ਪਹਿਲਾ(&multi->msgsent, &mid)) {
      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);

      DEBUGASSERT(ਅੰਕੜੇ);
      Curl_uint32_bset_remove(&multi->msgsent, mid);
      *msgs_ਵਿੱਚ_ਕਤਾਰ =
        curlx_uztosi(Curl_uint32_bset_ਗਿਣਤੀ(&multi->msgsent));
      ਜੇ(ਅੰਕੜੇ)
        ਸੁਨੇਹਾ_ਨਤੀਜਾ = &ਅੰਕੜੇ->ਸੁਨੇਹਾ;
    }
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ ਸੁਨੇਹਾ_ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_multi_will_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_socket_t s)
{
  ਜੇ(ਅੰਕੜੇ) {
    ਬਣਤਰ Curl_multi *multi = ਅੰਕੜੇ->multi;
    ਜੇ(multi) {
      CURL_TRC_M(ਅੰਕੜੇ, "Curl_multi_will_close fd=%" FMT_SOCKET_T, s);
      Curl_multi_ev_socket_ਪੂਰਾ(multi, ਅੰਕੜੇ, s);
    }
  }
}

ਸਥਿਰ ਖਾਲੀ multi_timeouts_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅੰਕੜੇ->state.timeouts.ਪਹਿਲਾ = EXPIRE_ਆਖਰੀ;
  ਅੰਕੜੇ->state.timeouts.splaynode.registered = FALSE;
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
ਸਥਿਰ CURLMcode ਜੋੜੋ_ਅਗਲਾ_timeout(ਅਚਲ ਬਣਤਰ curltime *pnow,
                                  ਬਣਤਰ Curl_multi *multi,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ expire_timers *timeouts = &ਅੰਕੜੇ->state.timeouts;
  timediff_t now_us = Curl_timeouts_ਵਿੱਥ_us(&multi->timeouts, pnow);

  ਜਦੋਂ(timeouts->ਪਹਿਲਾ < EXPIRE_ਆਖਰੀ) {
    ਜੇ(timeouts->ਵਿੱਥ_us[timeouts->ਪਹਿਲਾ] <= now_us)  /* already expired */
      timeouts->ਪਹਿਲਾ = timeouts->ਅਗਲਾ[timeouts->ਪਹਿਲਾ];
    ਨਹੀਂਤਾਂ /* timeouts are sorted, first is first in the future now */
      ਰੋਕੋ;
  }

  ਜੇ(timeouts->ਪਹਿਲਾ < EXPIRE_ਆਖਰੀ) {
    /* Insert this node again into the splay. Keep the timer in the list in
       case we need to recompute future timers. */
    Curl_timeouts_ਜੋੜੋ(&multi->timeouts, ਅੰਕੜੇ,
                      timeouts->ਵਿੱਥ_us[timeouts->ਪਹਿਲਾ]);
  }
  ਵਾਪਸ CURLM_OK;
}

ਸਥਿਰ ਖਾਲੀ multi_mark_expired_as_dirty(ਬਣਤਰ Curl_multi *multi,
                                        ਅਚਲ ਬਣਤਰ curltime *ts)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = ਨਿਰਾ;
  uint32_t mid;

  /*
   * The loop following here will go on as long as there are expire-times left
   * to process (compared to `ts`) in the splay and 'data' will be
   * re-assigned for every expired handle we deal with.
   */
  ਜਦੋਂ(Curl_timeouts_remove_expired(&multi->timeouts, ts, &mid)) {
    /* Check if there is one (more) expired timer to deal with! This function
       extracts a matching node if there is one */
    ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);
    ਜੇ(!ਅੰਕੜੇ) {
      DEBUGASSERT(0);
      ਜਾਰੀ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
    ਜੇ(CURL_TRC_TIMER_is_verbose(ਅੰਕੜੇ)) {
      ਜੇ(ਅੰਕੜੇ->state.timeouts.ਪਹਿਲਾ < EXPIRE_ਆਖਰੀ) {
        CURL_TRC_TIMER(ਅੰਕੜੇ, ਅੰਕੜੇ->state.timeouts.ਪਹਿਲਾ, "has expired");
      }
    }
#ਜੇ_ਅੰਤ
    (ਖਾਲੀ)ਜੋੜੋ_ਅਗਲਾ_timeout(ts, multi, ਅੰਕੜੇ);
    Curl_multi_mark_dirty(ਅੰਕੜੇ);
  }
}

ਸਥਿਰ CURLMcode multi_ਲੜੀਵਾਰ_dirty(ਬਣਤਰ Curl_multi *multi,
                                 ਬਣਤਰ Curl_sigpipe_ctx *sigpipe_ctx,
                                 uint32_t *pnum)
{
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;

  *pnum = 0;
  ਜੇ(Curl_uint32_bset_ਪਹਿਲਾ(&multi->dirty, &mid)) {
    ਕਰੋ {
      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);
      ਜੇ(ਅੰਕੜੇ) {
        CURL_TRC_M(ਅੰਕੜੇ, "multi_run_dirty");

        ਜੇ(!Curl_uint32_bset_ਸ਼ਾਮਲ_ਹੈ(&multi->process, mid)) {
          ਜੇ(Curl_uint32_bset_ਸ਼ਾਮਲ_ਹੈ(&multi->pending, mid)) {
            /* Something happened on the pending transfer, act on it. */
            move_pending_ਮੰਜ਼ਿਲ_connect(multi, ਅੰਕੜੇ);
          }
          ਨਹੀਂਤਾਂ {
            /* We are no longer processing this transfer */
            Curl_uint32_bset_remove(&multi->dirty, mid);
            ਜਾਰੀ;
          }
        }

        (*pnum)++;
        /* runsingle() clears the dirty mid */
        mresult = multi_runsingle(multi, ਅੰਕੜੇ, sigpipe_ctx);

        ਜੇ(CURLM_OK >= mresult) {
          /* reassess event handling of data */
          mresult = Curl_multi_ev_assess_xfer(multi, ਅੰਕੜੇ);
          ਜੇ(mresult)
            ਜਾਓ ਬਾਹਰਲਾ;
        }
      }
      ਨਹੀਂਤਾਂ {
        CURL_TRC_M(multi->admin, "multi_run_dirty, %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } ਜਦੋਂ(Curl_uint32_bset_ਅਗਲਾ(&multi->dirty, mid, &mid));
  }

ਬਾਹਰਲਾ:
  ਵਾਪਸ mresult;
}

ਸਥਿਰ CURLMcode multi_socket(ਬਣਤਰ Curl_multi *multi,
                              bool checkall,
                              curl_socket_t s,
                              ਪੂਰਨ ev_bitmask,
                              ਪੂਰਨ *running_handles)
{
  CURLMcode mresult = CURLM_OK;
  ਬਣਤਰ Curl_sigpipe_ctx pipe_ctx;
  ਬਣਤਰ curltime now;
  uint32_t ਲੜੀਵਾਰ_xfers;

  (ਖਾਲੀ)ev_bitmask;
  sigpipe_init(&pipe_ctx);

  ਜੇ(checkall) {
    /* *perform() deals with running_handles on its own */
    mresult = multi_perform(multi, running_handles);

    ਜੇ(mresult != CURLM_BAD_HANDLE) {
      /* Reassess event status of all active transfers, but keep any error
         from multi_perform() */
      CURLMcode mresult2 =
        Curl_multi_ev_assess_xfer_bset(multi, &multi->process);
      ਜੇ(!mresult)
        mresult = mresult2;
    }
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(s != CURL_SOCKET_TIMEOUT) {
    /* Mark all transfers of that socket as dirty */
    Curl_multi_ev_dirty_xfers(multi, s);
  }
  ਨਹੀਂਤਾਂ {
    /* Asked to run due to time-out. Clear the 'last_expire_ts' variable to
       force Curl_update_timer() to trigger a callback to the app again even
       if the same timeout is still the one to run after this call. That
       handles the case when the application asks libcurl to run the timeout
       prematurely. */
    multi->ਆਖਰੀ_expire_ਵਿੱਥ_us = 0;

    /* Applications may set `socket_cb` *after* having added transfers
     * first. *Then* kick off processing with a
     * curl_multi_socket_action(TIMEOUT) afterwards. Make sure our
     * admin handle registers its pollset with the callbacks present. */
    mresult = multi_assess_wakeup(multi);
    ਜੇ(mresult)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  curlx_pnow(&now);
  multi_mark_expired_as_dirty(multi, &now);
  mresult = multi_ਲੜੀਵਾਰ_dirty(multi, &pipe_ctx, &ਲੜੀਵਾਰ_xfers);
  ਜੇ(mresult)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(ਲੜੀਵਾਰ_xfers) {
    /* Running transfers takes time. With a new timestamp, we might catch
     * other expires which are due now. Instead of telling the application
     * to set a 0 timeout and call us again, we run them here.
     * Do that only once or it might be unfair to transfers on other
     * sockets. */
    curlx_pnow(&now);
    multi_mark_expired_as_dirty(multi, &now);
    mresult = multi_ਲੜੀਵਾਰ_dirty(multi, &pipe_ctx, &ਲੜੀਵਾਰ_xfers);
  }

ਬਾਹਰਲਾ:
  sigpipe_restore(&pipe_ctx);

  ਜੇ(multi_ischanged(multi, TRUE))
    multi_ਸਮਾਂ_ਸਾਰਣੀ_pending(multi);

  ਜੇ(!mresult && CURL_MNTFY_HAS_ENTRIES(multi))
    mresult = Curl_mntfy_dispatch_all(multi);

  ਜੇ(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? (ਪੂਰਨ)running : ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
  }

  ਜੇ(CURLM_OK >= mresult)
    mresult = Curl_update_timer(multi);
  ਵਾਪਸ mresult;
}

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ curl_multi_setopt
CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_setopt, &mresult)) {
    ਬਣਤਰ Curl_multi *multi = m;
    va_list param;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ uarg;
    size_t szarg;

    va_start(param, option);

    ਚੋਣ(option) {
    ਹਾਲਤ CURLMOPT_SOCKETFUNCTION:
      multi->socket_cb = va_arg(param, curl_socket_callback);
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_SOCKETDATA:
      multi->socket_userp = va_arg(param, ਖਾਲੀ *);
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_PUSHFUNCTION:
      multi->ਧੱਕੋ_cb = va_arg(param, curl_ਧੱਕੋ_callback);
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_PUSHDATA:
      multi->ਧੱਕੋ_userp = va_arg(param, ਖਾਲੀ *);
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_PIPELINING:
      multi->multiplexing = va_arg(param, ਲੰਮਾ) & CURLPIPE_MULTIPLEX ? 1 : 0;
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_TIMERFUNCTION:
      multi->timer_cb = va_arg(param, curl_multi_timer_callback);
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_TIMERDATA:
      multi->timer_userp = va_arg(param, ਖਾਲੀ *);
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_MAXCONNECTS:
      uarg = va_arg(param, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ);
      ਜੇ(uarg <= UINT32_MAX)
        multi->maxconnects = (uint32_t)uarg;
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_MAX_HOST_CONNECTIONS:
      ਜੇ(!curlx_sltouz(va_arg(param, ਲੰਮਾ), &szarg))
        mresult = CURLM_BAD_FUNCTION_ਦਲੀਲ;
      multi->max_host_connections = (szarg < UINT32_MAX) ?
                                    (uint32_t)szarg : UINT32_MAX;
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_MAX_ਕੁੱਲ_CONNECTIONS:
      ਜੇ(!curlx_sltouz(va_arg(param, ਲੰਮਾ), &szarg))
        mresult = CURLM_BAD_FUNCTION_ਦਲੀਲ;
      multi->max_ਕੁੱਲ_connections = (szarg < UINT32_MAX) ?
                                     (uint32_t)szarg : UINT32_MAX;
      ਰੋਕੋ;
      /* options formerly used for pipelining */
    ਹਾਲਤ CURLMOPT_MAX_PIPELINE_ਲੰਮਾਈ:
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_CONTENT_ਲੰਮਾਈ_PENALTY_ਮਾਪ:
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_CHUNK_ਲੰਮਾਈ_PENALTY_ਮਾਪ:
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_PIPELINING_SITE_BL:
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_PIPELINING_SERVER_BL:
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_MAX_CONCURRENT_STREAMS: {
      ਜੇ(!curlx_sltouz(va_arg(param, ਲੰਮਾ), &szarg) ||
         !szarg || (szarg > (size_t)ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)) /* preserve previous cutoff */
        multi->max_concurrent_streams = 100;
      ਨਹੀਂਤਾਂ
        multi->max_concurrent_streams = (szarg < UINT32_MAX) ?
                                       (uint32_t)szarg : UINT32_MAX;
      ਰੋਕੋ;
    }
    ਹਾਲਤ CURLMOPT_ਜਾਲ_ਬਦਲਿਆ: {
      ਲੰਮਾ val = va_arg(param, ਲੰਮਾ);
      ਜੇ(val & CURLMNWC_CLEAR_ALL)
        /* In the beginning, all values available to set were 1 by mistake. We
           converted this to mean "all", thus setting all the bits
           automatically */
        val = CURLMNWC_CLEAR_DNS | CURLMNWC_CLEAR_CONNS;
      ਜੇ(val & CURLMNWC_CLEAR_DNS) {
        Curl_dnscache_clear(multi->admin);
      }
      ਜੇ(val & CURLMNWC_CLEAR_CONNS) {
        Curl_cpool_nw_ਬਦਲਿਆ(&multi->cpool, multi->admin);
      }
      ਰੋਕੋ;
    }
    ਹਾਲਤ CURLMOPT_NOTIFYFUNCTION:
      multi->ntfy.ntfy_cb = va_arg(param, curl_notify_callback);
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_NOTIFYDATA:
      multi->ntfy.ntfy_cb_ਅੰਕੜੇ = va_arg(param, ਖਾਲੀ *);
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_RESOLVE_ਧਾਗੇ_MAX:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_THREADED
      uarg = va_arg(param, ਲੰਮਾ);
      ਜੇ((uarg <= 0) || (uarg > UINT32_MAX))
        mresult = CURLM_BAD_FUNCTION_ਦਲੀਲ;
      ਨਹੀਂਤਾਂ {
        CURLcode ਨਤੀਜਾ = Curl_async_thrdd_multi_ਸੈੱਟ_props(
          multi, 0, (uint32_t)uarg, 2000);
        ਚੋਣ(ਨਤੀਜਾ) {
        ਹਾਲਤ CURLE_OK:
          mresult = CURLM_OK;
          ਰੋਕੋ;
        ਹਾਲਤ CURLE_BAD_FUNCTION_ਦਲੀਲ:
          mresult = CURLM_BAD_FUNCTION_ਦਲੀਲ;
          ਰੋਕੋ;
        ਹਾਲਤ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ:
          mresult = CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
          ਰੋਕੋ;
        ਮੂਲ:
          mresult = CURLM_ਅੰਦਰੂਨੀ_ERROR;
          ਰੋਕੋ;
        }
      }
#ਜੇ_ਅੰਤ
      ਰੋਕੋ;
    ਹਾਲਤ CURLMOPT_ਤੇਜ਼_EXIT:
      multi->quick_exit = va_arg(param, ਲੰਮਾ) ? 1 : 0;
      ਰੋਕੋ;
    ਮੂਲ:
      mresult = CURLM_UNKNOWN_OPTION;
      ਰੋਕੋ;
    }
    va_end(param);
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

/* we define curl_multi_socket() in the public multi.h header */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ curl_multi_socket

CURLMcode curl_multi_socket(CURLM *m, curl_socket_t s, ਪੂਰਨ *running_handles)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_socket, &mresult)) {
    mresult = multi_socket(m, FALSE, s, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

CURLMcode curl_multi_socket_action(CURLM *m, curl_socket_t s,
                                   ਪੂਰਨ ev_bitmask, ਪੂਰਨ *running_handles)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_socket_action, &mresult)) {
    mresult = multi_socket(m, FALSE, s, ev_bitmask, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

CURLMcode curl_multi_socket_all(CURLM *m, ਪੂਰਨ *running_handles)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_socket_all, &mresult)) {
    mresult = multi_socket(m, TRUE, CURL_SOCKET_BAD, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

ਸਥਿਰ bool multi_has_dirties(ਬਣਤਰ Curl_multi *multi)
{
  uint32_t mid;
  ਜੇ(Curl_uint32_bset_ਪਹਿਲਾ(&multi->dirty, &mid)) {
    ਕਰੋ {
      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);
      ਜੇ(ਅੰਕੜੇ) {
        ਜੇ(Curl_uint32_bset_ਸ਼ਾਮਲ_ਹੈ(&multi->process, mid))
          ਵਾਪਸ TRUE;
        /* We are no longer processing this transfer */
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
      ਨਹੀਂਤਾਂ {
        CURL_TRC_M(multi->admin, "dirty transfer %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } ਜਦੋਂ(Curl_uint32_bset_ਅਗਲਾ(&multi->dirty, mid, &mid));
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਖਾਲੀ multi_timeout(ਬਣਤਰ Curl_multi *multi,
                          timediff_t *pexire_ਵਿੱਥ_us,
                          ਪੂਰਨ *timeout_ms)
{
  ਜੇ(multi->dead) {
    ਜੇ(pexire_ਵਿੱਥ_us)
      *pexire_ਵਿੱਥ_us = 0;
    *timeout_ms = 0;
    ਵਾਪਸ;
  }

  ਜੇ(multi_has_dirties(multi)) {
    ਜੇ(pexire_ਵਿੱਥ_us) {
      ਬਣਤਰ curltime now;
      curlx_pnow(&now);
      *pexire_ਵਿੱਥ_us = Curl_timeouts_ਵਿੱਥ_us(&multi->timeouts, &now);
    }
    *timeout_ms = 0;
    ਵਾਪਸ;
  }
  ਨਹੀਂਤਾਂ {
    ਬਣਤਰ curltime now;
    uint32_t mid;

    curlx_pnow(&now);
    *timeout_ms = Curl_timeouts_ਅਗਲਾ_ms(&multi->timeouts, &now,
                                        pexire_ਵਿੱਥ_us, &mid);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
    ਜੇ(mid != UINT32_MAX) {
      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);
      ਜੇ(ਅੰਕੜੇ && CURL_TRC_TIMER_is_verbose(ਅੰਕੜੇ) &&
         (ਅੰਕੜੇ->state.timeouts.ਪਹਿਲਾ < EXPIRE_ਆਖਰੀ)) {
        CURL_TRC_TIMER(ਅੰਕੜੇ, ਅੰਕੜੇ->state.timeouts.ਪਹਿਲਾ,
                       "gives multi timeout in %dms", *timeout_ms);
      }
    }
#ਜੇ_ਅੰਤ
  }
}

CURLMcode curl_multi_timeout(CURLM *m,
                             ਲੰਮਾ *timeout_ms)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_timeout, &mresult)) {
    ਪੂਰਨ itimeout_ms;

    multi_timeout(m, ਨਿਰਾ, &itimeout_ms);
    *timeout_ms = (ਲੰਮਾ)itimeout_ms;
    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

/*
 * Tell the application it should update its timers, if it subscribes to the
 * update timer callback.
 */
CURLMcode Curl_update_timer(ਬਣਤਰ Curl_multi *multi)
{
  timediff_t timeouts_ਵਿੱਥ_us = 0;
  ਪੂਰਨ timeout_ms;
  ਪੂਰਨ rc;
  bool ਸੈੱਟ_ਮੁੱਲ = FALSE;

  ਜੇ(!multi->timer_cb || multi->dead)
    ਵਾਪਸ CURLM_OK;
  multi_timeout(multi, &timeouts_ਵਿੱਥ_us, &timeout_ms);

  ਜੇ(timeout_ms < 0 && !multi->ਆਖਰੀ_timeout_ਸੈੱਟ) {
    /* nothing to do */
  }
  ਨਹੀਂਤਾਂ ਜੇ(timeout_ms < 0) {
    /* there is no timeout now but there was one previously */
    CURL_TRC_M(multi->admin, "[TIMER] clear");
    timeout_ms = -1; /* normalize */
    ਸੈੱਟ_ਮੁੱਲ = TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!multi->ਆਖਰੀ_timeout_ਸੈੱਟ) {
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, none before", timeout_ms);
    ਸੈੱਟ_ਮੁੱਲ = TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(multi->ਆਖਰੀ_expire_ਵਿੱਥ_us != timeouts_ਵਿੱਥ_us) {
    /* We had a timeout before and have one now, the absolute timestamp
     * differs. The relative timeout_ms may be the same, but the starting
     * point differs. Let the application restart its timer. */
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, replace previous",
               timeout_ms);
    ਸੈੱਟ_ਮੁੱਲ = TRUE;
  }
  ਨਹੀਂਤਾਂ {
    /* We have same expire time as previously. Our relative 'timeout_ms'
     * may be different now, but the application has the timer running
     * and we do not to tell it to start this again. */
  }

  ਜੇ(ਸੈੱਟ_ਮੁੱਲ) {
    ਬਣਤਰ Curl_mapi_guard guard;

    multi->ਆਖਰੀ_expire_ਵਿੱਥ_us = timeouts_ਵਿੱਥ_us;
    multi->ਆਖਰੀ_timeout_ਸੈੱਟ = timeout_ms >= 0;
    CURL_CBAPI_MULTI_ਸ਼ੁਰੂ(&guard, multi, multi_timer_cb);
    rc = multi->timer_cb(multi, timeout_ms, multi->timer_userp);
    CURL_CBAPI_MULTI_ਸਿਰਾ(&guard);
    ਜੇ(rc == -1) {
      multi->dead = TRUE;
      ਵਾਪਸ CURLM_ABORTED_BY_CALLBACK;
    }
  }
  ਵਾਪਸ CURLM_OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
ਸਥਿਰ bool multi_timeouts_check(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ expire_timers *timeouts = &ਅੰਕੜੇ->state.timeouts;
  uint8_t id;
  ਪੂਰਨ i = 0;
  ਲਈ(id = timeouts->ਪਹਿਲਾ; id < EXPIRE_ਆਖਰੀ; id = timeouts->ਅਗਲਾ[id]) {
    ਜੇ(++i >= EXPIRE_ਆਖਰੀ) {
      failf(ਅੰਕੜੇ, "expire timeouts looped: %d iterations and no end", i);
      ਵਾਪਸ FALSE;
    }
    ਜੇ(id == timeouts->ਅਗਲਾ[id]) {
      failf(ਅੰਕੜੇ, "expire timeouts wrong: %d points to itself", (ਪੂਰਨ)id);
      ਵਾਪਸ FALSE;
    }
    ਜੇ((timeouts->ਅਗਲਾ[id] < EXPIRE_ਆਖਰੀ) &&
       (timeouts->ਵਿੱਥ_us[id] > timeouts->ਵਿੱਥ_us[timeouts->ਅਗਲਾ[id]])) {
      failf(ਅੰਕੜੇ, "expire timeouts not sorted: %d happens after %d but "
            "is listed before", (ਪੂਰਨ)id, (ਪੂਰਨ)timeouts->ਅਗਲਾ[id]);
      ਵਾਪਸ FALSE;
    }
  }
  ਵਾਪਸ TRUE;
}
#ਜੇ_ਅੰਤ

/*
 * Remove a given timestamp from the list of timeouts.
 */
ਸਥਿਰ ਖਾਲੀ multi_clear_timeout(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, expire_id eid)
{
  ਬਣਤਰ expire_timers *timeouts = &ਅੰਕੜੇ->state.timeouts;
  uint8_t orig_ਪਹਿਲਾ = timeouts->ਪਹਿਲਾ;
  uint8_t *anchor = &timeouts->ਪਹਿਲਾ;
  uint8_t id = (uint8_t)eid;

  ਜੇ((ਬਿਨਾਨਿਸ਼ਾਨੀ)eid >= EXPIRE_ਆਖਰੀ) {
    DEBUGASSERT(0);
    ਵਾਪਸ;
  }

  ਜਦੋਂ(*anchor < EXPIRE_ਆਖਰੀ) {
    ਜੇ(*anchor == id) {
      *anchor = timeouts->ਅਗਲਾ[id];
      ਰੋਕੋ;
    }
    anchor = &timeouts->ਅਗਲਾ[*anchor];
  }
  DEBUGASSERT(multi_timeouts_check(ਅੰਕੜੇ));
  ਜੇ(Curl_timeouts_has(ਅੰਕੜੇ)) {
    ਬਣਤਰ Curl_multi *multi = ਅੰਕੜੇ->multi;

    ਜੇ(!multi) {
      DEBUGASSERT(0);
      ਵਾਪਸ;
    }
    ਜੇ((timeouts->ਪਹਿਲਾ >= EXPIRE_ਆਖਰੀ) || /* no more timeouts */
       (timeouts->ਪਹਿਲਾ != orig_ਪਹਿਲਾ)) {  /* active timeout changed */
      Curl_timeouts_remove(&multi->timeouts, ਅੰਕੜੇ);
    }
    ਜੇ((timeouts->ਪਹਿਲਾ < EXPIRE_ਆਖਰੀ) && !Curl_timeouts_has(ਅੰਕੜੇ)) {
      Curl_timeouts_ਜੋੜੋ(&multi->timeouts, ਅੰਕੜੇ,
                        timeouts->ਵਿੱਥ_us[timeouts->ਪਹਿਲਾ]);
    }
  }
}

/*
 * Add a timestamp to the list of timeouts. Keep the list sorted so that head
 * of list is always the timeout nearest in time.
 */
ਸਥਿਰ CURLMcode multi_ਸੈੱਟ_timeout(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਅਚਲ ਬਣਤਰ curltime *stamp,
                                   expire_id eid)
{
  ਬਣਤਰ expire_timers *timeouts = &ਅੰਕੜੇ->state.timeouts;
  uint8_t *anchor = &timeouts->ਪਹਿਲਾ;
  uint8_t id = (uint8_t)eid;

  ਜੇ((ਬਿਨਾਨਿਸ਼ਾਨੀ)eid >= EXPIRE_ਆਖਰੀ) {
    DEBUGASSERT(0);
    ਵਾਪਸ CURLM_BAD_FUNCTION_ਦਲੀਲ;
  }
  /* remove from list, store time and re-insert */
  multi_clear_timeout(ਅੰਕੜੇ, eid);
  timeouts->ਵਿੱਥ_us[id] =
    Curl_timeouts_ਵਿੱਥ_us(&ਅੰਕੜੇ->multi->timeouts, stamp);

  ਜਦੋਂ(*anchor < EXPIRE_ਆਖਰੀ) {
    ਜੇ(timeouts->ਵਿੱਥ_us[*anchor] > timeouts->ਵਿੱਥ_us[id])
      ਰੋਕੋ;
    anchor = &timeouts->ਅਗਲਾ[*anchor];
  }
  timeouts->ਅਗਲਾ[eid] = *anchor;
  timeouts->ਅਗਲਾ[eid] = *anchor;
  *anchor = id;
  DEBUGASSERT(multi_timeouts_check(ਅੰਕੜੇ));
  CURL_TRC_TIMER(ਅੰਕੜੇ, eid, "set for %" FMT_TIMEDIFF_T "us",
                 curlx_ptimediff_us(stamp, Curl_pgrs_now(ਅੰਕੜੇ)));
  ਵਾਪਸ CURLM_OK;
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
ਖਾਲੀ Curl_expire_ਸੈੱਟ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                     expire_id eid, timediff_t ms,
                     ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਬਣਤਰ Curl_multi *multi = ਅੰਕੜੇ->multi;
  ਬਣਤਰ expire_timers *timeouts = &ਅੰਕੜੇ->state.timeouts;
  uint8_t prev_id = timeouts->ਪਹਿਲਾ;
  ਬਣਤਰ curltime ਸੈੱਟ;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  ਜੇ(!multi)
    ਵਾਪਸ;
  DEBUGASSERT(eid < EXPIRE_ਆਖਰੀ);
  ਜੇ(ms > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
    /* Cap ridiculous timeouts, 31-bit ms is still 3.5 weeks. When the time
       goes to the user, it must fit in this size. */
    ms = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;

  ਸੈੱਟ = *pnow;
  ਸੈੱਟ.tv_sec += (time_t)(ms / 1000); /* may be a 64 to 32-bit conversion */
  ਸੈੱਟ.tv_usec += (ਪੂਰਨ)(ms % 1000) * 1000;
  ਜੇ(ਸੈੱਟ.tv_usec >= 1000000) {
    ਸੈੱਟ.tv_sec++;
    ਸੈੱਟ.tv_usec -= 1000000;
  }

  /* Add the timeout, will replace any previous value for this timer. */
  multi_ਸੈੱਟ_timeout(ਅੰਕੜੇ, &ਸੈੱਟ, eid);
  DEBUGASSERT(timeouts->ਪਹਿਲਾ < EXPIRE_ਆਖਰੀ);

  ਜੇ(Curl_timeouts_has(ਅੰਕੜੇ)) {
    /* data has already a timeout registered. If the first timer
     * was NOT the one we just set AND is still the first one,
     * nothing changed from the timeouts point of view. The
     * set timer triggers after the one already registered. Leave. */
    ਜੇ((prev_id != eid) && (prev_id == timeouts->ਪਹਿਲਾ))
      ਵਾਪਸ;

    /* Since this is an updated time, we must remove data from
     * timeouts and then add it again. */
    Curl_timeouts_remove(&multi->timeouts, ਅੰਕੜੇ);
  }

  /* Insert the new timer expiry since it is our local minimum. */
  Curl_timeouts_ਜੋੜੋ(&multi->timeouts, ਅੰਕੜੇ,
                    timeouts->ਵਿੱਥ_us[timeouts->ਪਹਿਲਾ]);
}

ਖਾਲੀ Curl_expire(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                 timediff_t milli, expire_id eid)
{
  Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, eid, milli, Curl_pgrs_now(ਅੰਕੜੇ));
}

/*
 * Removes the expire timer. Marks it as done.
 */
ਖਾਲੀ Curl_expire_clear(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, expire_id eid)
{
  /* remove the timer, if there */
  multi_clear_timeout(ਅੰਕੜੇ, eid);
  CURL_TRC_TIMER(ਅੰਕੜੇ, eid, "cleared");
}

/*
 * Clear ALL timeout values for this handle.
 */
ਖਾਲੀ Curl_expire_clear_all(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_multi *multi = ਅੰਕੜੇ->multi;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  ਜੇ(!multi)
    ਵਾਪਸ;

  ਜੇ(Curl_timeouts_remove(&multi->timeouts, ਅੰਕੜੇ)) {
    /* Since this is an cleared time, we must remove the previous entry from
       the splay tree */
    multi_timeouts_init(ਅੰਕੜੇ);

    ਜੇ(ਅੰਕੜੇ->id >= 0)
      CURL_TRC_M(ਅੰਕੜੇ, "[TIMEOUT] all cleared");
  }
}

CURLMcode curl_multi_ਸੌਂਪੋ(CURLM *m, curl_socket_t sockfd,
                            ਖਾਲੀ *sockp)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_ਸੌਂਪੋ, &mresult)) {
    mresult = Curl_multi_ev_ਸੌਂਪੋ(m, sockfd, sockp);
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

ਸਥਿਰ ਖਾਲੀ move_pending_ਮੰਜ਼ਿਲ_connect(ਬਣਤਰ Curl_multi *multi,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  DEBUGASSERT(ਅੰਕੜੇ->mstate == MSTATE_PENDING);

  /* Remove this node from the pending set, add into process set */
  Curl_uint32_bset_remove(&multi->pending, ਅੰਕੜੇ->mid);
  Curl_uint32_bset_ਜੋੜੋ(&multi->process, ਅੰਕੜੇ->mid);

  multistate(ਅੰਕੜੇ, MSTATE_CONNECT);
  Curl_multi_mark_dirty(ਅੰਕੜੇ); /* make it run */
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
ਸਥਿਰ ਖਾਲੀ multi_ਸਮਾਂ_ਸਾਰਣੀ_pending(ਬਣਤਰ Curl_multi *multi)
{
  uint32_t mid = multi->ਆਖਰੀ_pending_mid;

  ਜੇ(mid) {
    ਜਦੋਂ(Curl_uint32_bset_ਅਗਲਾ(&multi->pending, mid, &mid)) {
      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);
      ਜੇ(ਅੰਕੜੇ) {
        move_pending_ਮੰਜ਼ਿਲ_connect(multi, ਅੰਕੜੇ);
        multi->ਆਖਰੀ_pending_mid = mid;
        ਵਾਪਸ;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    }
    /* found no pending transfers with `mid` larger than `last_pending_mid`.
     * Start at the beginning of the pending set again. */
    multi->ਆਖਰੀ_pending_mid = 0;
  }

  ਜੇ(Curl_uint32_bset_ਪਹਿਲਾ(&multi->pending, &mid)) {
    ਕਰੋ {
      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_multi_get_easy(multi, mid);
      ਜੇ(ਅੰਕੜੇ) {
        move_pending_ਮੰਜ਼ਿਲ_connect(multi, ਅੰਕੜੇ);
        multi->ਆਖਰੀ_pending_mid = mid;
        ਵਾਪਸ;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    } ਜਦੋਂ(Curl_uint32_bset_ਅਗਲਾ(&multi->pending, mid, &mid));
  }
}

uint32_t Curl_multi_max_concurrent_streams(ਬਣਤਰ Curl_multi *multi)
{
  DEBUGASSERT(multi);
  ਵਾਪਸ multi->max_concurrent_streams;
}

CURL **curl_multi_get_handles(CURLM *m)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURL **a = ਨਿਰਾ;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_get_handles, ਨਿਰਾ)) {
    ਬਣਤਰ Curl_multi *multi = m;
    ਖਾਲੀ *ਇੰਦਰਾਜ;
    size_t ਗਿਣਤੀ_2 = Curl_uint32_tbl_ਗਿਣਤੀ(&multi->xfers);

    a = curlx_malloc(ਆਕਾਰ(ਬਣਤਰ Curl_easy *) * (ਗਿਣਤੀ_2 + 1));
    ਜੇ(a) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i = 0;
      uint32_t mid;

      ਜੇ(Curl_uint32_tbl_ਪਹਿਲਾ(&multi->xfers, &mid, &ਇੰਦਰਾਜ)) {
        ਕਰੋ {
          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = ਇੰਦਰਾਜ;
          DEBUGASSERT(i < ਗਿਣਤੀ_2);
          ਜੇ(!ਅੰਕੜੇ->state.ਅੰਦਰੂਨੀ)
            a[i++] = ਅੰਕੜੇ;
        } ਜਦੋਂ(Curl_uint32_tbl_ਅਗਲਾ(&multi->xfers, mid, &mid, &ਇੰਦਰਾਜ));
      }
      a[i] = ਨਿਰਾ; /* last entry is a NULL */
    }
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ a;
}

CURLMcode curl_multi_get_offt(CURLM *m,
                              CURLMinfo_offt info,
                              curl_off_t *pvalue)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_get_offt, &mresult)) {
    ਬਣਤਰ Curl_multi *multi = m;
    uint32_t n;

    ਜੇ(!pvalue) {
      mresult = CURLM_BAD_FUNCTION_ਦਲੀਲ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਚੋਣ(info) {
    ਹਾਲਤ CURLMINFO_XFERS_CURRENT:
      n = Curl_uint32_tbl_ਗਿਣਤੀ(&multi->xfers);
      ਜੇ(n && multi->admin)
        --n;
      *pvalue = (curl_off_t)n;
      ਰੋਕੋ;
    ਹਾਲਤ CURLMINFO_XFERS_RUNNING:
      n = Curl_uint32_bset_ਗਿਣਤੀ(&multi->process);
      ਜੇ(n && Curl_uint32_bset_ਸ਼ਾਮਲ_ਹੈ(&multi->process, multi->admin->mid))
        --n;
      *pvalue = (curl_off_t)n;
      ਰੋਕੋ;
    ਹਾਲਤ CURLMINFO_XFERS_PENDING:
      *pvalue = (curl_off_t)Curl_uint32_bset_ਗਿਣਤੀ(&multi->pending);
      ਰੋਕੋ;
    ਹਾਲਤ CURLMINFO_XFERS_ਪੂਰਾ:
      *pvalue = (curl_off_t)Curl_uint32_bset_ਗਿਣਤੀ(&multi->msgsent);
      ਰੋਕੋ;
    ਹਾਲਤ CURLMINFO_XFERS_ADDED:
      *pvalue = multi->xfers_ਕੁੱਲ_ever;
      ਰੋਕੋ;
    ਮੂਲ:
      *pvalue = -1;
      mresult = CURLM_UNKNOWN_OPTION;
      ਰੋਕੋ;
    }
  }
ਬਾਹਰਲਾ:
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

ਸਥਿਰ ਬਣਤਰ Curl_fixed_buf *fixed_buf_create(size_t len)
{
  ਬਣਤਰ Curl_fixed_buf *fbuf;
  ਜੇ((SIZE_MAX - ਆਕਾਰ(*fbuf)) < len)
    ਵਾਪਸ ਨਿਰਾ; /* too large */
  fbuf = curlx_malloc(len + ਆਕਾਰ(*fbuf));
  ਜੇ(fbuf)
    fbuf->len = len;
  ਵਾਪਸ fbuf;
}

CURLcode Curl_multi_xfer_buf_borrow(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਅੱਖਰ **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->multi);
  *pbuf = ਨਿਰਾ;
  *pbuflen = 0;
  ਜੇ(!ਅੰਕੜੇ->multi) {
    failf(ਅੰਕੜੇ, "transfer has no multi handle");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.ਬਫਰ_ਮਾਪ) {
    failf(ਅੰਕੜੇ, "transfer buffer size is 0");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  ਜੇ(ਅੰਕੜੇ->multi->xfer_buf_borrowed) {
    failf(ਅੰਕੜੇ, "attempt to borrow xfer_buf when already borrowed");
    ਵਾਪਸ CURLE_AGAIN;
  }

  ਜੇ(ਅੰਕੜੇ->multi->xfer_buf &&
     ਅੰਕੜੇ->ਸੈੱਟ.ਬਫਰ_ਮਾਪ > ਅੰਕੜੇ->multi->xfer_buf->len) {
    /* not large enough, get a new one */
    curlx_safefree(ਅੰਕੜੇ->multi->xfer_buf);
  }

  ਜੇ(!ਅੰਕੜੇ->multi->xfer_buf) {
    ਅੰਕੜੇ->multi->xfer_buf =
      fixed_buf_create(curlx_uitouz(ਅੰਕੜੇ->ਸੈੱਟ.ਬਫਰ_ਮਾਪ));
    ਜੇ(!ਅੰਕੜੇ->multi->xfer_buf) {
      failf(ਅੰਕੜੇ, "could not allocate xfer_buf of %u bytes",
            ਅੰਕੜੇ->ਸੈੱਟ.ਬਫਰ_ਮਾਪ);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }

  ਅੰਕੜੇ->multi->xfer_buf_borrowed = TRUE;
  *pbuf = ਅੰਕੜੇ->multi->xfer_buf->ਅੰਕੜੇ;
  *pbuflen = ਅੰਕੜੇ->multi->xfer_buf->len;
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_multi_xfer_buf_release(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅੱਖਰ *buf)
{
  (ਖਾਲੀ)buf;
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->multi);
  DEBUGASSERT(!buf || (ਅੰਕੜੇ->multi->xfer_buf &&
                       ਅੰਕੜੇ->multi->xfer_buf->ਅੰਕੜੇ == buf));
  ਅੰਕੜੇ->multi->xfer_buf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_ulbuf_borrow(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਅੱਖਰ **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->multi);
  *pbuf = ਨਿਰਾ;
  *pbuflen = 0;
  ਜੇ(!ਅੰਕੜੇ->multi) {
    failf(ਅੰਕੜੇ, "transfer has no multi handle");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  ਜੇ(ਅੰਕੜੇ->multi->xfer_ulbuf_borrowed) {
    failf(ਅੰਕੜੇ, "attempt to borrow xfer_ulbuf when already borrowed");
    ਵਾਪਸ CURLE_AGAIN;
  }

  ਜੇ(ਅੰਕੜੇ->multi->xfer_ulbuf &&
     ਅੰਕੜੇ->ਸੈੱਟ.upload_ਬਫਰ_ਮਾਪ > ਅੰਕੜੇ->multi->xfer_ulbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(ਅੰਕੜੇ->multi->xfer_ulbuf);
  }

  ਜੇ(!ਅੰਕੜੇ->multi->xfer_ulbuf) {
    ਅੰਕੜੇ->multi->xfer_ulbuf =
      fixed_buf_create(curlx_uitouz(ਅੰਕੜੇ->ਸੈੱਟ.upload_ਬਫਰ_ਮਾਪ));
    ਜੇ(!ਅੰਕੜੇ->multi->xfer_ulbuf) {
      failf(ਅੰਕੜੇ, "could not allocate xfer_ulbuf of %u bytes",
            ਅੰਕੜੇ->ਸੈੱਟ.upload_ਬਫਰ_ਮਾਪ);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }

  ਅੰਕੜੇ->multi->xfer_ulbuf_borrowed = TRUE;
  *pbuf = ਅੰਕੜੇ->multi->xfer_ulbuf->ਅੰਕੜੇ;
  *pbuflen = ਅੰਕੜੇ->multi->xfer_ulbuf->len;
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_multi_xfer_ulbuf_release(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅੱਖਰ *buf)
{
  (ਖਾਲੀ)buf;
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਅੰਕੜੇ->multi);
  DEBUGASSERT(!buf || (ਅੰਕੜੇ->multi->xfer_ulbuf &&
                       ਅੰਕੜੇ->multi->xfer_ulbuf->ਅੰਕੜੇ == buf));
  ਅੰਕੜੇ->multi->xfer_ulbuf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_sockbuf_borrow(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        size_t blen, ਅੱਖਰ **pbuf)
{
  DEBUGASSERT(ਅੰਕੜੇ);
  *pbuf = ਨਿਰਾ;
  ਜੇ(!ਅੰਕੜੇ->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    *pbuf = curlx_malloc(blen);
    ਵਾਪਸ *pbuf ? CURLE_OK : CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਜੇ(ਅੰਕੜੇ->multi->xfer_sockbuf_borrowed) {
    failf(ਅੰਕੜੇ, "attempt to borrow xfer_sockbuf when already borrowed");
    ਵਾਪਸ CURLE_AGAIN;
  }

  ਜੇ(ਅੰਕੜੇ->multi->xfer_sockbuf && blen > ਅੰਕੜੇ->multi->xfer_sockbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(ਅੰਕੜੇ->multi->xfer_sockbuf);
  }

  ਜੇ(!ਅੰਕੜੇ->multi->xfer_sockbuf) {
    ਅੰਕੜੇ->multi->xfer_sockbuf = fixed_buf_create(blen);
    ਜੇ(!ਅੰਕੜੇ->multi->xfer_sockbuf) {
      failf(ਅੰਕੜੇ, "could not allocate xfer_sockbuf of %zu bytes", blen);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }

  ਅੰਕੜੇ->multi->xfer_sockbuf_borrowed = TRUE;
  *pbuf = ਅੰਕੜੇ->multi->xfer_sockbuf->ਅੰਕੜੇ;
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_multi_xfer_sockbuf_release(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅੱਖਰ *buf)
{
  DEBUGASSERT(ਅੰਕੜੇ);
  ਜੇ(!ਅੰਕੜੇ->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    curlx_free(buf);
  }
  ਨਹੀਂਤਾਂ {
    DEBUGASSERT(!buf || (ਅੰਕੜੇ->multi->xfer_sockbuf &&
                         ਅੰਕੜੇ->multi->xfer_sockbuf->ਅੰਕੜੇ == buf));
    ਅੰਕੜੇ->multi->xfer_sockbuf_borrowed = FALSE;
  }
}

ਸਥਿਰ ਖਾਲੀ multi_xfer_bufs_free(ਬਣਤਰ Curl_multi *multi)
{
  DEBUGASSERT(multi);
  curlx_safefree(multi->xfer_buf);
  multi->xfer_buf_borrowed = FALSE;
  curlx_safefree(multi->xfer_ulbuf);
  multi->xfer_ulbuf_borrowed = FALSE;
  curlx_safefree(multi->xfer_sockbuf);
  multi->xfer_sockbuf_borrowed = FALSE;
}

ਬਣਤਰ Curl_easy *Curl_multi_get_easy(ਬਣਤਰ Curl_multi *multi,
                                      uint32_t mid)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_uint32_tbl_get(&multi->xfers, mid);
  ਜੇ(GOOD_EASY_HANDLE(ਅੰਕੜੇ))
    ਵਾਪਸ ਅੰਕੜੇ;
  CURL_TRC_M(multi->admin, "invalid easy handle in xfer table for mid=%u",
             mid);
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  ਵਾਪਸ ਨਿਰਾ;
}

bool Curl_multi_knows_easy(ਬਣਤਰ Curl_multi *multi, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ Curl_uint32_tbl_get(&multi->xfers, ਅੰਕੜੇ->mid) == ਅੰਕੜੇ;
}

ਸਥਿਰ uint32_t multi_xfers_running(ਬਣਤਰ Curl_multi *multi)
{
  ਜੇ(!multi) {
    DEBUGASSERT(0);
    ਵਾਪਸ 0;
  }
  ਵਾਪਸ multi->xfers_alive;
}

uint32_t Curl_multi_xfers_attached(ਬਣਤਰ Curl_multi *multi)
{
  ਜੇ(!multi || !multi->admin) {
    DEBUGASSERT(0);
    ਵਾਪਸ 0;
  }
  /* Discount the admin handle */
  ਵਾਪਸ Curl_uint32_tbl_ਗਿਣਤੀ(&multi->xfers) - 1;
}

ਖਾਲੀ Curl_multi_mark_dirty(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(ਅੰਕੜੇ->multi && ਅੰਕੜੇ->mid != UINT32_MAX)
    Curl_uint32_bset_ਜੋੜੋ(&ਅੰਕੜੇ->multi->dirty, ਅੰਕੜੇ->mid);
}

ਖਾਲੀ Curl_multi_clear_dirty(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(ਅੰਕੜੇ->multi && ਅੰਕੜੇ->mid != UINT32_MAX)
    Curl_uint32_bset_remove(&ਅੰਕੜੇ->multi->dirty, ਅੰਕੜੇ->mid);
}

CURLMcode curl_multi_notify_enable(CURLM *m, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ notification)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_notify_enable, &mresult)) {
    mresult = Curl_mntfy_enable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

CURLMcode curl_multi_notify_disable(CURLM *m, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ notification)
{
  ਬਣਤਰ Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ਜੇ(CURL_MAPI_ENTER(&guard, m, multi_notify_disable, &mresult)) {
    mresult = Curl_mntfy_disable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  ਵਾਪਸ mresult;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
ਸਥਿਰ ਖਾਲੀ multi_xfer_dump(ਬਣਤਰ Curl_multi *multi, uint32_t mid,
                            ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = ਇੰਦਰਾਜ;

  (ਖਾਲੀ)multi;
  ਜੇ(!ਅੰਕੜੇ) {
    curl_mfprintf(stderr, "mid=%u, entry=NULL, bug in xfer table?\n", mid);
  }
  ਨਹੀਂਤਾਂ {
    curl_mfprintf(stderr, "mid=%u, magic=%s, p=%p, id=%" FMT_OFF_T
                  ", url=%s\n",
                  mid,
                  (ਅੰਕੜੇ->magic == CURLEASY_MAGIC_ਸੰਖਿਆ) ? "GOOD" : "BAD!",
                  (ਖਾਲੀ *)ਅੰਕੜੇ, ਅੰਕੜੇ->id, Curl_bufref_ptr(&ਅੰਕੜੇ->state.url));
  }
}

ਸਥਿਰ ਖਾਲੀ multi_xfer_tbl_dump(ਬਣਤਰ Curl_multi *multi)
{
  uint32_t mid;
  ਖਾਲੀ *ਇੰਦਰਾਜ;
  curl_mfprintf(stderr, "=== multi xfer table (count=%u, capacity=%u\n",
                Curl_uint32_tbl_ਗਿਣਤੀ(&multi->xfers),
                Curl_uint32_tbl_ਸਮਰੱਥਾ(&multi->xfers));
  ਜੇ(Curl_uint32_tbl_ਪਹਿਲਾ(&multi->xfers, &mid, &ਇੰਦਰਾਜ)) {
    multi_xfer_dump(multi, mid, ਇੰਦਰਾਜ);
    ਜਦੋਂ(Curl_uint32_tbl_ਅਗਲਾ(&multi->xfers, mid, &mid, &ਇੰਦਰਾਜ))
      multi_xfer_dump(multi, mid, ਇੰਦਰਾਜ);
  }
  curl_mfprintf(stderr, "===\n");
  fflush(stderr);
}
#ਜੇ_ਅੰਤ /* DEBUGBUILD */
