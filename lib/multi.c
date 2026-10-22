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
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_XFER_ಕೋಷ್ಟಕ_ಗಾತ್ರ 128

/* CURL_SOCKET_HASH_TABLE_SIZE should be a prime number. Increasing it from 97
   to 911 takes on a 32-bit machine 4 x 804 = 3211 more bytes. Still, every
   curl handle takes 6K memory, therefore this 3K are not significant. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_SOCKET_HASH_ಕೋಷ್ಟಕ_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SOCKET_HASH_ಕೋಷ್ಟಕ_ಗಾತ್ರ 911
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_CONNECTION_HASH_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_CONNECTION_HASH_ಗಾತ್ರ 97
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DNS_HASH_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_DNS_HASH_ಗಾತ್ರ 71
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_TLS_SESSION_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TLS_SESSION_ಗಾತ್ರ 25
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ move_pending_ಗಮ್ಯ_connect(ರಚನೆ Curl_multi *multi,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
ಸ್ಥಿರ CURLMcode ಕೂಡಿಸು_ಮುಂದಿನ_timeout(ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow,
                                  ರಚನೆ Curl_multi *multi,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ multi_timeout(ರಚನೆ Curl_multi *multi,
                          timediff_t *pexire_ಸ್ಥಾನಾಂತರ_us,
                          ಪೂರ್ಣಾಂಕ *timeout_ms);
ಸ್ಥಿರ ಶೂನ್ಯ multi_ವೇಳಾಪಟ್ಟಿ_pending(ರಚನೆ Curl_multi *multi);
ಸ್ಥಿರ ಶೂನ್ಯ multi_xfer_bufs_free(ರಚನೆ Curl_multi *multi);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
ಸ್ಥಿರ ಶೂನ್ಯ multi_xfer_tbl_dump(ರಚನೆ Curl_multi *multi);
#ಆದರೆ_ಅಂತ್ಯ
/* Get the # of transfers current in process/pending. */
ಸ್ಥಿರ uint32_t multi_xfers_running(ರಚನೆ Curl_multi *multi);

/* function pointer called once when entering a state */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*mstate_enter_func)(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  CURLMstate from_state);

ಸ್ಥಿರ ಶೂನ್ಯ mstate_enter_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 CURLMstate from_state)
{
  (ಶೂನ್ಯ)from_state;
  Curl_init_CONNECT(ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ ಶೂನ್ಯ mstate_enter_did(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             CURLMstate from_state)
{
  (ಶೂನ್ಯ)from_state;
  ದತ್ತಾಂಶ->req.chunk = FALSE;
  Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_PRETRANSFER);
  ಆದರೆ(!CURL_REQ_WANT_SEND(ದತ್ತಾಂಶ))
    Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_POSTRANSFER);
}

ಸ್ಥಿರ ಶೂನ್ಯ mstate_enter_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              CURLMstate from_state)
{
  (ಶೂನ್ಯ)from_state;
  CURLM_NTFY(ದತ್ತಾಂಶ, CURLMNOTIFY_EASY_ಮುಗಿದ);
}

ಸ್ಥಿರ ಶೂನ್ಯ mstate_enter_completed(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   CURLMstate from_state)
{
  /* we sometimes directly jump to COMPLETED, trigger things
   * we then missed. */
  ಆದರೆ(from_state < MSTATE_DID) {
    Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_PRETRANSFER);
    Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_POSTRANSFER);
    Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_STARTTRANSFER);
  }
  /* multi_done() should have run and detached any connection already */
  DEBUGASSERT(!ದತ್ತಾಂಶ->conn);
  Curl_pgrsCompleted(ದತ್ತಾಂಶ);
  ಆದರೆ(from_state < MSTATE_ಮುಗಿದ)
    CURLM_NTFY(ದತ್ತಾಂಶ, CURLMNOTIFY_EASY_ಮುಗಿದ);
  /* changing to COMPLETED means it is in process and needs to go */
  DEBUGASSERT(Curl_uint32_bset_ಒಳಗೊಂಡಿದೆ(&ದತ್ತಾಂಶ->multi->process, ದತ್ತಾಂಶ->mid));
  Curl_uint32_bset_remove(&ದತ್ತಾಂಶ->multi->process, ದತ್ತಾಂಶ->mid);
  Curl_uint32_bset_remove(&ದತ್ತಾಂಶ->multi->pending, ದತ್ತಾಂಶ->mid); /* to be sure */

  ಆದರೆ(Curl_uint32_bset_ಖಾಲಿಗಳು(&ದತ್ತಾಂಶ->multi->process)) {
    /* free the transfer buffer when we have no more active transfers */
    multi_xfer_bufs_free(ದತ್ತಾಂಶ->multi);
  }
  /* Important: reset the conn pointer so that we do not point to memory
     that could be freed anytime */
  Curl_detach_connection(ದತ್ತಾಂಶ);
  Curl_expire_clear_all(ದತ್ತಾಂಶ); /* stop all timers */
}

/* always use this function to change state, to make debugging easier */
ಸ್ಥಿರ ಶೂನ್ಯ mstate(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLMstate state
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
                   , ಪೂರ್ಣಾಂಕ lineno
#ಆದರೆ_ಅಂತ್ಯ
)
{
  CURLMstate oldstate = ದತ್ತಾಂಶ->mstate;
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ mstate_enter_func state_enter[MSTATE_ಕೊನೆಯ] = {
    ಖಾಲಿ,                      /* INIT */
    ಖಾಲಿ,                      /* PENDING */
    ಖಾಲಿ,                      /*ಸಿದ್ಧತೆ*/
    mstate_enter_connect,      /* CONNECT */
    ಖಾಲಿ,                      /* CONNECTING */
    ಖಾಲಿ,                      /* PROTOCONNECT */
    ಖಾಲಿ,                      /* PROTOCONNECTING */
    ಖಾಲಿ,                      /* DO */
    ಖಾಲಿ,                      /* DOING */
    ಖಾಲಿ,                      /* DOING_MORE */
    mstate_enter_did,          /* DID */
    ಖಾಲಿ,                      /* PERFORMING */
    ಖಾಲಿ,                      /* RATELIMITING */
    mstate_enter_ಮುಗಿದ,         /* DONE */
    mstate_enter_completed,    /* COMPLETED */
    ಖಾಲಿ                       /* MSGSENT */
  };

  ಆದರೆ(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  NOVERBOSE((ಶೂನ್ಯ)lineno);
  CURL_TRC_M(ದತ್ತಾಂಶ, "-> [%s] (line %d)", CURL_MSTATE_NAME(state), lineno);
#ಇಲ್ಲದಿದ್ದರೆ
  CURL_TRC_M(ದತ್ತಾಂಶ, "-> [%s]", CURL_MSTATE_NAME(state));
#ಆದರೆ_ಅಂತ್ಯ

  /* really switching state */
  ದತ್ತಾಂಶ->mstate = state;
  ಆದರೆ(state_enter[state])
    state_enter[state](ದತ್ತಾಂಶ, oldstate);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು multistate(x, y) mstate(x, y)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು multistate(x, y) mstate(x, y, __LINE__)
#ಆದರೆ_ಅಂತ್ಯ

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
ಸ್ಥಿರ ಶೂನ್ಯ multi_addmsg(ರಚನೆ Curl_multi *multi, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(Curl_uint32_bset_ಖಾಲಿಗಳು(&multi->msgsent))
    CURLM_NTFY(multi->admin, CURLMNOTIFY_INFO_READ);
  Curl_uint32_bset_ಕೂಡಿಸು(&multi->msgsent, ದತ್ತಾಂಶ->mid);
}

ಸ್ಥಿರ ಶೂನ್ಯ multi_timeouts_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ರಚನೆ Curl_multi *Curl_multi_handle(uint32_t xfer_ಕೋಷ್ಟಕ_ಗಾತ್ರ,
                                     size_t ev_hashsize,  /* event hash */
                                     size_t chashsize, /* connection hash */
                                     size_t dnssize,   /* dns hash */
                                     size_t sesssize)  /* TLS session cache */
{
  ರಚನೆ curltime created;
  ರಚನೆ Curl_multi *multi = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_multi));

  ಆದರೆ(!multi)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  multi->magic = CURLMULTI_MAGIC_ಸಂಖ್ಯೆ;

  /* Initialisation order is important here!
   * easy_init() does a lazy check on curl_global_init() which sets
   * up platform specific things we need. For example calling curlx_pnow()
   * before this is not safe. */
  multi->admin = curl_easy_init();
  ಆದರೆ(!multi->admin) {
    curlx_free(multi);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }
  multi->admin->multi = multi;
  multi->admin->state.ಆಂತರಿಕ = TRUE;

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
  Curl_hash_init(&multi->proto_hash, 23, CURL_HASH_TYPE_ಬೈಟುಗಳು, ph_freeentry);
  Curl_cshutdn_init(&multi->cshutdn);

  multi->multiplexing = TRUE;
  multi->max_concurrent_streams = 100;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_WAKEUP
  multi->wakeup_pair[0] = CURL_SOCKET_BAD;
  multi->wakeup_pair[1] = CURL_SOCKET_BAD;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_ಆಂತರಿಕ_WAKEUP
  multi->wakeup_ಆಂತರಿಕ[0] = CURL_SOCKET_BAD;
  multi->wakeup_ಆಂತರಿಕ[1] = CURL_SOCKET_BAD;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(Curl_uint32_bset_resize(&multi->process, xfer_ಕೋಷ್ಟಕ_ಗಾತ್ರ) ||
     Curl_uint32_bset_resize(&multi->pending, xfer_ಕೋಷ್ಟಕ_ಗಾತ್ರ) ||
     Curl_uint32_bset_resize(&multi->dirty, xfer_ಕೋಷ್ಟಕ_ಗಾತ್ರ) ||
     Curl_uint32_bset_resize(&multi->msgsent, xfer_ಕೋಷ್ಟಕ_ಗಾತ್ರ) ||
     Curl_uint32_tbl_resize(&multi->xfers, xfer_ಕೋಷ್ಟಕ_ಗಾತ್ರ))
    ಹೋಗು error;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
    ಆದರೆ(getenv("CURL_DEBUG"))
      multi->admin->ಗಣ.verbose = TRUE;
    p = getenv("CURL_DBG_MAX_TOTAL_CONNECTIONS");
    ಆದರೆ(p) {
      curl_off_t l;
      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, UINT32_MAX)) {
        multi->max_ಒಟ್ಟು_connections = (uint32_t)l;
      }
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  Curl_uint32_tbl_ಕೂಡಿಸು(&multi->xfers, multi->admin, &multi->admin->mid);
  Curl_uint32_bset_ಕೂಡಿಸು(&multi->process, multi->admin->mid);

  Curl_cpool_init(&multi->cpool, ಖಾಲಿ, chashsize);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  ಆದರೆ(Curl_ssl_scache_create(sesssize, 2, &multi->ssl_scache))
    ಹೋಗು error;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)sesssize;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
  multi->wsa_event = WSACreateEvent();
  ಆದರೆ(multi->wsa_event == WSA_INVALID_EVENT)
    ಹೋಗು error;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_WAKEUP
  /* When enabled, rely on this to work. We ignore this in previous
   * versions, but that seems an unnecessary complication. */
  ಆದರೆ(Curl_wakeup_init(multi->wakeup_pair, TRUE) < 0)
    ಹೋಗು error;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_ಆಂತರಿಕ_WAKEUP
  ಆದರೆ(Curl_wakeup_init(multi->wakeup_ಆಂತರಿಕ, TRUE) < 0)
    ಹೋಗು error;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(Curl_probeipv6(multi))
    ಹೋಗು error;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_THREADED
  ಆದರೆ(xfer_ಕೋಷ್ಟಕ_ಗಾತ್ರ < CURL_XFER_ಕೋಷ್ಟಕ_ಗಾತ್ರ) { /* easy multi */
    ಆದರೆ(Curl_async_thrdd_multi_init(multi, 0, 2, 10))
      ಹೋಗು error;
  }
  ಇಲ್ಲದಿದ್ದರೆ { /* real multi handle */
    ಆದರೆ(Curl_async_thrdd_multi_init(multi, 0, 20, 2000))
      ಹೋಗು error;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ multi;

error:

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_THREADED
  Curl_async_thrdd_multi_destroy(multi, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
  Curl_multi_ev_cleanup(multi);
  Curl_hash_destroy(&multi->proto_hash);
  Curl_dnscache_destroy(&multi->dnscache);
  Curl_cpool_destroy(&multi->cpool, multi->admin);
  Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  Curl_ssl_scache_destroy(multi->ssl_scache);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(multi->admin) {
    Curl_multi_ev_xfer_ಮುಗಿದ(multi, multi->admin);
    multi->admin->multi = ಖಾಲಿ;
    Curl_close(&multi->admin);
  }
  Curl_mntfy_cleanup(multi);

  Curl_uint32_bset_destroy(&multi->process);
  Curl_uint32_bset_destroy(&multi->dirty);
  Curl_uint32_bset_destroy(&multi->pending);
  Curl_uint32_bset_destroy(&multi->msgsent);
  Curl_uint32_tbl_destroy(&multi->xfers);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_ಆಂತರಿಕ_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_ಆಂತರಿಕ);
#ಆದರೆ_ಅಂತ್ಯ

  curlx_free(multi);
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

CURLM *curl_multi_init(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ Curl_multi_handle(CURL_XFER_ಕೋಷ್ಟಕ_ಗಾತ್ರ,
                           CURL_SOCKET_HASH_ಕೋಷ್ಟಕ_ಗಾತ್ರ,
                           CURL_CONNECTION_HASH_ಗಾತ್ರ,
                           CURL_DNS_HASH_ಗಾತ್ರ,
                           CURL_TLS_SESSION_ಗಾತ್ರ);
}

#ಆದರೆ defined(DEBUGBUILD) && defined(CURLVERBOSE)
ಸ್ಥಿರ ಶೂನ್ಯ multi_warn_debug(ರಚನೆ Curl_multi *multi, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(!multi->warned) {
    infof(ದತ್ತಾಂಶ, "!!! WARNING !!!");
    infof(ದತ್ತಾಂಶ, "This is a debug build of libcurl, "
                "do not use in production.");
    multi->warned = TRUE;
  }
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು multi_warn_debug(x, y) Curl_nop_stmt
#ಆದರೆ_ಅಂತ್ಯ

bool Curl_is_connecting(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->mstate < MSTATE_DO;
}

ಸ್ಥಿರ CURLMcode multi_assess_wakeup(ರಚನೆ Curl_multi *multi)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_ಆಂತರಿಕ_WAKEUP
  ಆದರೆ(multi->socket_cb)
    ಹಿಂತಿರುಗಿ Curl_multi_ev_assess_xfer(multi, multi->admin);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)multi;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode multi_xfers_ಕೂಡಿಸು(ರಚನೆ Curl_multi *multi,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  uint32_t ಸಾಮರ್ಥ್ಯ = Curl_uint32_tbl_ಸಾಮರ್ಥ್ಯ(&multi->xfers);
  uint32_t new_ಗಾತ್ರ = 0;
  /* Prepare to make this into a CURLMOPT_MAX_TRANSFERS, because some
   * applications may want to prevent a run-away of their memory use. */
  /* UINT_MAX is our "invalid" id, do not let the table grow up to that. */
  ಸ್ಥಿರಾಂಕ uint32_t max_ಸಾಮರ್ಥ್ಯ = UINT_MAX - 1;

  ಆದರೆ(ಸಾಮರ್ಥ್ಯ < max_ಸಾಮರ್ಥ್ಯ) {
    /* We want `multi->xfers` to have "sufficient" free rows, so that we do
     * not have to reuse the `mid` from a removed easy right away.
     * Check if an 8th of the capacity is still free */
    uint32_t ಬಳಸಿದ = Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers);
    uint32_t ಬಳಸದ = ಸಾಮರ್ಥ್ಯ - ಬಳಸಿದ;
    uint32_t min_ಬಳಸದ = CURLMAX(ಸಾಮರ್ಥ್ಯ >> 3, 4);
    ಆದರೆ(ಬಳಸದ < min_ಬಳಸದ) {
      /* Grow by 50% of current capacity, in range of [128, 2048],
       * which means the table grows max by 16kb on 64-bit arch. */
      uint32_t growth = CURLMIN(CURLMAX(ಸಾಮರ್ಥ್ಯ >> 1, 128), 2048);
      /* Make sure the uint arithmetic here works on the corner
       * cases where we are close to max_capacity or UINT_MAX */
      ಆದರೆ((max_ಸಾಮರ್ಥ್ಯ - growth) <= ಸಾಮರ್ಥ್ಯ)
        new_ಗಾತ್ರ = max_ಸಾಮರ್ಥ್ಯ;
      ಇಲ್ಲದಿದ್ದರೆ
        new_ಗಾತ್ರ = ಸಾಮರ್ಥ್ಯ + growth;
    }
  }

  ಆದರೆ(new_ಗಾತ್ರ > ಸಾಮರ್ಥ್ಯ) {
    /* Grow the bitsets first. Should one fail, we do not need
     * to downsize the already resized ones. The sets continue
     * to work properly when larger than the table, but not
     * the other way around. */
    CURL_TRC_M(ದತ್ತಾಂಶ, "increasing xfer table size to %u", new_ಗಾತ್ರ);
    ಆದರೆ(Curl_uint32_bset_resize(&multi->process, new_ಗಾತ್ರ) ||
       Curl_uint32_bset_resize(&multi->dirty, new_ಗಾತ್ರ) ||
       Curl_uint32_bset_resize(&multi->pending, new_ಗಾತ್ರ) ||
       Curl_uint32_bset_resize(&multi->msgsent, new_ಗಾತ್ರ) ||
       Curl_uint32_tbl_resize(&multi->xfers, new_ಗಾತ್ರ))
      ಹಿಂತಿರುಗಿ CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  /* Insert the easy into the table now */
  ಆದರೆ(!Curl_uint32_tbl_ಕೂಡಿಸು(&multi->xfers, ದತ್ತಾಂಶ, &ದತ್ತಾಂಶ->mid)) {
    /* MUST only happen when table is full */
    DEBUGASSERT(Curl_uint32_tbl_ಸಾಮರ್ಥ್ಯ(&multi->xfers) <=
                Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers));
    ಹಿಂತಿರುಗಿ CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

CURLMcode Curl_multi_ಕೂಡಿಸು_handle(ರಚನೆ Curl_multi *multi,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLMcode mresult;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  ಆದರೆ(ದತ್ತಾಂಶ->multi)
    ಹಿಂತಿರುಗಿ CURLM_ADDED_ALREADY;

  ಆದರೆ(multi->dead) {
    /* a "dead" handle cannot get added transfers while any existing easy
       handles are still alive - but if there are none alive anymore, it is
       fine to start over and unmark the "deadness" of this handle.
       This means only the admin handle MUST be present. */
    ಆದರೆ((Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers) != 1) ||
       !Curl_uint32_tbl_ಒಳಗೊಂಡಿದೆ(&multi->xfers, 0))
      ಹಿಂತಿರುಗಿ CURLM_ABORTED_BY_CALLBACK;
    multi->dead = FALSE;
    Curl_uint32_bset_clear(&multi->process);
    Curl_uint32_bset_clear(&multi->dirty);
    Curl_uint32_bset_clear(&multi->pending);
    Curl_uint32_bset_clear(&multi->msgsent);
  }

  ಆದರೆ(ದತ್ತಾಂಶ->multi_easy) {
    /* if this easy handle was previously used for curl_easy_perform(), there
       is a private multi handle here that we can kill */
    curl_multi_cleanup(ದತ್ತಾಂಶ->multi_easy);
    ದತ್ತಾಂಶ->multi_easy = ಖಾಲಿ;
  }

  /* Insert the easy into the multi->xfers table, assigning it a `mid`. */
  ಆದರೆ(multi_xfers_ಕೂಡಿಸು(multi, ದತ್ತಾಂಶ))
    ಹಿಂತಿರುಗಿ CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Initialize timeouts for this handle */
  multi_timeouts_init(ದತ್ತಾಂಶ);

  /*
   * No failure allowed in this function beyond this point. No modification
   * of easy nor multi handle allowed before this except for potential
   * multi's connection pool growing which will not be undone in this
   * function no matter what.
   */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.errorbuffer)
    ದತ್ತಾಂಶ->ಗಣ.errorbuffer[0] = 0;

  ದತ್ತಾಂಶ->state.os_errno = 0;

  /* make the Curl_easy refer back to this multi handle - before
     Curl_expire() is called. */
  ದತ್ತಾಂಶ->multi = multi;

  /* set the easy handle */
  multistate(ದತ್ತಾಂಶ, MSTATE_INIT);
  /* not yet passed INIT state */
  ದತ್ತಾಂಶ->state.really_alive = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
  /* Do the same for PSL. */
  ಆದರೆ(ದತ್ತಾಂಶ->share && (ದತ್ತಾಂಶ->share->specifier & (1 << CURL_ಬೀಗ_ದತ್ತಾಂಶ_PSL)))
    ದತ್ತಾಂಶ->psl = &ದತ್ತಾಂಶ->share->psl;
  ಇಲ್ಲದಿದ್ದರೆ
    ದತ್ತಾಂಶ->psl = &multi->psl;
#ಆದರೆ_ಅಂತ್ಯ

  /* add the easy handle to the process set */
  Curl_uint32_bset_ಕೂಡಿಸು(&multi->process, ದತ್ತಾಂಶ->mid);
  ++multi->xfers_alive;
  ++multi->xfers_ಒಟ್ಟು_ever;

  Curl_cpool_xfer_init(ದತ್ತಾಂಶ);
  multi_warn_debug(multi, ದತ್ತಾಂಶ);

  /* Make sure the new handle will run */
  Curl_multi_mark_dirty(ದತ್ತಾಂಶ);

  /* Necessary in event based processing, where dirty handles trigger
   * a timeout callback invocation. */
  mresult = Curl_update_timer(multi);
  ಆದರೆ(mresult) {
    /* undo the additions above */
    Curl_uint32_bset_remove(&multi->process, ದತ್ತಾಂಶ->mid);
    Curl_uint32_bset_remove(&multi->dirty, ದತ್ತಾಂಶ->mid);
    --multi->xfers_alive;
    ದತ್ತಾಂಶ->multi = ಖಾಲಿ; /* not anymore */
    Curl_uint32_tbl_remove(&multi->xfers, ದತ್ತಾಂಶ->mid);
    ದತ್ತಾಂಶ->mid = UINT32_MAX;
    ಹಿಂತಿರುಗಿ mresult;
  }

  /* The admin handle only ever has default timeouts set. To improve the
     state somewhat we clone the timeouts from each added handle so that the
     admin handle always has the same timeouts as the most recently added
     easy handle. */
  multi->admin->ಗಣ.timeout = ದತ್ತಾಂಶ->ಗಣ.timeout;
  multi->admin->ಗಣ.server_response_timeout =
    ದತ್ತಾಂಶ->ಗಣ.server_response_timeout;
  multi->admin->ಗಣ.no_signal = ದತ್ತಾಂಶ->ಗಣ.no_signal;

  CURL_TRC_M(ದತ್ತಾಂಶ, "added to multi, mid=%u, running=%u, total=%u",
             ದತ್ತಾಂಶ->mid, multi_xfers_running(multi),
             Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers));
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

CURLMcode curl_multi_ಕೂಡಿಸು_handle(CURLM *m, CURL *curl)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_ಕೂಡಿಸು_handle, &mresult)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
    /* Verify that we got a somewhat good easy handle too */
    ಆದರೆ(!GOOD_EASY_HANDLE(ದತ್ತಾಂಶ) || ದತ್ತಾಂಶ->state.ಆಂತರಿಕ)
      mresult = CURLM_BAD_EASY_HANDLE;
    ಇಲ್ಲದಿದ್ದರೆ
      mresult = Curl_multi_ಕೂಡಿಸು_handle(m, ದತ್ತಾಂಶ);
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

ರಚನೆ multi_ಮುಗಿದ_ctx {
  ಬಿಟ್(premature);
};

ಸ್ಥಿರ bool multi_conn_should_close(ರಚನೆ connectdata *conn,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    bool premature)
{
  /* if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of everything else. */
  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.close)
    ಹಿಂತಿರುಗಿ TRUE;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM/NEGOTIATE authentication handshake. */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.reuse_forbid
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
     && !(conn->http_ntlm_state == NTLMSTATE_TYPE2 ||
          conn->proxy_ntlm_state == NTLMSTATE_TYPE2)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO
     && !(conn->http_negotiate_state == CURL_NEGO_AUTH_RECV ||
          conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)
#ಆದರೆ_ಅಂತ್ಯ
    )
    ಹಿಂತಿರುಗಿ TRUE;

  /* Unless this connection is for a "connect-only" transfer, it
   * needs to be closed if the protocol handler does not support reuse. */
  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.connect_only && conn->scheme &&
     !(conn->scheme->flags & PROTOPT_CONN_REUSE))
    ಹಿಂತಿರುಗಿ TRUE;

  /* if premature is TRUE, it means this connection was said to be DONE before
     the entire request operation is complete and thus we cannot know in what
     state it is for reusing, so we are forced to close it. In a perfect world
     we can add code that keep track of if we really must close it here or not,
     but currently we have no such detail knowledge. */
  ಆದರೆ(premature && !Curl_conn_is_multiplex(conn, FIRSTSOCKET))
    ಹಿಂತಿರುಗಿ TRUE;

  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ cpool_do_ಫಲಿತಾಂಶ multi_ಮುಗಿದ_locked(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ connectdata *conn,
                                         ಶೂನ್ಯ *userdata,
                                         ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ರಚನೆ multi_ಮುಗಿದ_ctx *mdctx = userdata;

  (ಶೂನ್ಯ)pnow;
  CURL_TRC_M(ದತ್ತಾಂಶ, "multi_done_locked, in use=%u", conn->attached_xfers);
  ಆದರೆ(CONN_INUSE(conn)) {
    /* Stop if still used. */
    CURL_TRC_M(ದತ್ತಾಂಶ, "Connection still in use %u, no more multi_done now!",
               conn->attached_xfers);
    ಹಿಂತಿರುಗಿ CPOOL_DO_KEEP;
  }

  ಆದರೆ(multi_conn_should_close(conn, ದತ್ತಾಂಶ, (bool)mdctx->premature)) {
    CURL_TRC_M(ದತ್ತಾಂಶ, "multi_done, terminating conn #%" FMT_OFF_T " to %s:%u, "
               "forbid=%d, close=%d, premature=%d, conn_multiplex=%d",
               conn->connection_id, conn->origin->user_hostname,
               conn->origin->ಪೋರ್ಟ್,
               ದತ್ತಾಂಶ->ಗಣ.reuse_forbid, conn->ಬಿಟ್ಟುಗಳು.close, mdctx->premature,
               Curl_conn_is_multiplex(conn, FIRSTSOCKET));
    connclose(conn);
    ಹಿಂತಿರುಗಿ mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!Curl_conn_get_max_concurrent(ದತ್ತಾಂಶ, conn, FIRSTSOCKET)) {
    CURL_TRC_M(ದತ್ತಾಂಶ, "multi_done, conn #%" FMT_OFF_T " to %s:%u was shutdown"
               " by server, not reusing", conn->connection_id,
               conn->origin->user_hostname, conn->origin->ಪೋರ್ಟ್);
    connclose(conn);
    ಹಿಂತಿರುಗಿ mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ CPOOL_DO_IDLE;
  }
}

ಸ್ಥಿರ CURLcode multi_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           CURLcode ಸ್ಥಿತಿ_2,  /* an error if this is called
                                                after an error was detected */
                           bool premature)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);

  CURL_TRC_M(ದತ್ತಾಂಶ, "multi_done: status: %d prem: %d done: %d",
             (ಪೂರ್ಣಾಂಕ)ಸ್ಥಿತಿ_2, (ಪೂರ್ಣಾಂಕ)premature, ದತ್ತಾಂಶ->state.ಮುಗಿದ);

  ಆದರೆ(ದತ್ತಾಂಶ->state.ಮುಗಿದ) {
    /* Stop if multi_done() has already been called */
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ದತ್ತಾಂಶ->state.ಮುಗಿದ = TRUE; /* called now! */

  /* Shut down any ongoing async resolver operation. */
  Curl_resolv_shutdown_all(ದತ್ತಾಂಶ);

  /* Cleanup possible redirect junk */
  curlx_safefree(ದತ್ತಾಂಶ->req.newurl);
  curlx_safefree(ದತ್ತಾಂಶ->req.location);

  ಆಯ್ಕೆ(ಸ್ಥಿತಿ_2) {
  ಸ್ಥಿತಿ CURLE_ABORTED_BY_CALLBACK:
  ಸ್ಥಿತಿ CURLE_READ_ERROR:
  ಸ್ಥಿತಿ CURLE_WRITE_ERROR:
    /* When we are aborted due to a callback return code it has to be counted
       as premature as there is trouble ahead if we do not. We have many
       callbacks and protocols work differently, we could potentially do this
       more fine-grained in the future. */
    premature = TRUE;
    FALLTHROUGH();
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }

  /* this calls the protocol-specific function pointer previously set */
  ಆದರೆ(conn && conn->scheme->ಸತತ->ಮುಗಿದ && (ದತ್ತಾಂಶ->mstate >= MSTATE_PROTOCONNECT))
    ಫಲಿತಾಂಶ = conn->scheme->ಸತತ->ಮುಗಿದ(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2, premature);
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = ಸ್ಥಿತಿ_2;

  ಆದರೆ(ದತ್ತಾಂಶ->mstate > MSTATE_CONNECTING &&
     (ಫಲಿತಾಂಶ != CURLE_ABORTED_BY_CALLBACK)) {
    /* avoid this if
     * - the transfer has not connected
     * - we already aborted by callback to avoid this calling another callback
     */
    ಪೂರ್ಣಾಂಕ rc = Curl_pgrs_ಮುಗಿದ(ದತ್ತಾಂಶ, pnow);
    ಆದರೆ(!ಫಲಿತಾಂಶ && rc)
      ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
  }

  /* Make sure that transfer client writes are really done now. */
  ಫಲಿತಾಂಶ = Curl_1st_fatal(ಫಲಿತಾಂಶ, Curl_xfer_write_ಮುಗಿದ(ದತ್ತಾಂಶ, premature));

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_req_ಮುಗಿದ(&ದತ್ತಾಂಶ->req, ದತ್ತಾಂಶ, premature);

  ಆದರೆ(conn) {
    /* Detach connection and decide what to do with it. */
    ರಚನೆ multi_ಮುಗಿದ_ctx mdctx;

    /* Inform connection filters that this transfer is done and they
     * need to give up any references to `data`. */
    Curl_conn_ev_ದತ್ತಾಂಶ_ಮುಗಿದ(ದತ್ತಾಂಶ, premature);
    Curl_detach_connection(ದತ್ತಾಂಶ);
    Curl_dnscache_prune(ದತ್ತಾಂಶ, pnow);

    memset(&mdctx, 0, ಗಾತ್ರ(mdctx));
    mdctx.premature = premature;
    Curl_cpool_return(ದತ್ತಾಂಶ, conn, multi_ಮುಗಿದ_locked, &mdctx, pnow);

    ಆದರೆ(ದತ್ತಾಂಶ->multi)
      multi_ವೇಳಾಪಟ್ಟಿ_pending(ದತ್ತಾಂಶ->multi);
  }

  /* flush the netrc cache */
  Curl_netrc_cleanup(&ದತ್ತಾಂಶ->state.netrc);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLMcode Curl_multi_remove_handle(ರಚನೆ Curl_multi *multi,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLMcode mresult;
  bool premature;
  uint32_t mid;

  /* Prevent users from trying to remove same easy handle more than once */
  ಆದರೆ(!ದತ್ತಾಂಶ->multi)
    ಹಿಂತಿರುಗಿ CURLM_OK; /* it is already removed so let's say it is fine! */

  /* Prevent users from trying to remove an easy handle from the wrong multi */
  ಆದರೆ(ದತ್ತಾಂಶ->multi != multi)
    ಹಿಂತಿರುಗಿ CURLM_BAD_EASY_HANDLE;

  ಆದರೆ(ದತ್ತಾಂಶ->mid == UINT32_MAX) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ CURLM_ಆಂತರಿಕ_ERROR;
  }
  ಆದರೆ(Curl_uint32_tbl_get(&multi->xfers, ದತ್ತಾಂಶ->mid) != ದತ್ತಾಂಶ) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ CURLM_ಆಂತರಿಕ_ERROR;
  }

  premature = (ದತ್ತಾಂಶ->mstate < MSTATE_COMPLETED);

  ಆದರೆ(ದತ್ತಾಂಶ->conn) {
    /* If the 'state' is not INIT or COMPLETED, we might need to do something
       nice to put the easy_handle in a good known state when this returns. */
    ಆದರೆ(premature && (ದತ್ತಾಂಶ->mstate > MSTATE_DO))
      streamclose(ದತ್ತಾಂಶ->conn);

    /* multi_done() clears the association between the easy handle and the
       connection.
       Note that this ignores the return code because there is
       nothing really useful to do with it anyway! */
    (ಶೂನ್ಯ)multi_ಮುಗಿದ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->ಫಲಿತಾಂಶ, premature);
  }

  /* The timer must be shut down before data->multi is set to NULL, else
     data's splaynode would remain in the splay tree after curl_easy_cleanup is
     called. Do it after multi_done() in case that sets another time! */
  Curl_expire_clear_all(ದತ್ತಾಂಶ);

  /* In MSGSENT, it was deducted from `multi->xfers_alive` already. */
  ಆದರೆ(ದತ್ತಾಂಶ->mstate != MSTATE_MSGSENT)
    --multi->xfers_alive;

  ಆದರೆ(ದತ್ತಾಂಶ->state.really_alive) {
    ದತ್ತಾಂಶ->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    ಆದರೆ(!multi->xfers_really_alive)
      (ಶೂನ್ಯ)multi_assess_wakeup(multi);
  }

  Curl_wildcard_dtor(&ದತ್ತಾಂಶ->wildcard);

  ದತ್ತಾಂಶ->mstate = MSTATE_COMPLETED;

  /* Remove the association between the connection and the handle */
  Curl_detach_connection(ದತ್ತಾಂಶ);

  /* Tell event handling that this transfer is definitely going away */
  Curl_multi_ev_xfer_ಮುಗಿದ(multi, ದತ್ತಾಂಶ);

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.connect_only) {
    ಆದರೆ(ದತ್ತಾಂಶ->multi_easy) {
      ಆದರೆ(ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id != -1) {
        /* Mark any connect-only connection for closure */
        ರಚನೆ connectdata *conn;
        (ಶೂನ್ಯ)Curl_getconnectinfo(ದತ್ತಾಂಶ, &conn);
        ಆದರೆ(conn && conn->ಬಿಟ್ಟುಗಳು.connect_only)
          connclose(conn);
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* This removes a handle that was part the multi interface that used
         CONNECT_ONLY, that connection is now left alive but since this handle
         has bits.close set nothing can use that connection anymore and it is
         forbidden from reuse. This easy handle cannot find the connection
         anymore once removed from the multi handle

         Better close the connection here, at once. */
      ರಚನೆ connectdata *conn;
      (ಶೂನ್ಯ)Curl_getconnectinfo(ದತ್ತಾಂಶ, &conn);
      ಆದರೆ(conn)
        Curl_conn_close(ದತ್ತಾಂಶ, conn, TRUE);
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
  /* Remove the PSL association. */
  ಆದರೆ(ದತ್ತಾಂಶ->psl == &multi->psl)
    ದತ್ತಾಂಶ->psl = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

  /* clear the association to this multi handle */
  mid = ದತ್ತಾಂಶ->mid;
  DEBUGASSERT(Curl_uint32_tbl_ಒಳಗೊಂಡಿದೆ(&multi->xfers, mid));
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  Curl_uint32_bset_remove(&multi->process, mid);
  Curl_uint32_bset_remove(&multi->dirty, mid);
  Curl_uint32_bset_remove(&multi->pending, mid);
  Curl_uint32_bset_remove(&multi->msgsent, mid);
  ದತ್ತಾಂಶ->multi = ಖಾಲಿ;
  ದತ್ತಾಂಶ->mid = UINT32_MAX;
  ದತ್ತಾಂಶ->master_mid = UINT32_MAX;

  /* A pending transfer *might* be able to run now. */
  multi_ವೇಳಾಪಟ್ಟಿ_pending(multi);
  mresult = Curl_update_timer(multi);
  ಆದರೆ(mresult)
    ಹಿಂತಿರುಗಿ mresult;

  mresult = multi_assess_wakeup(multi);
  ಆದರೆ(mresult) {
    failf(ದತ್ತಾಂಶ, "error enabling wakeup listening: %d", mresult);
    ಹಿಂತಿರುಗಿ mresult;
  }

  CURL_TRC_M(ದತ್ತಾಂಶ, "removed from multi, mid=%u, running=%u, total=%u",
             mid, multi_xfers_running(multi),
             Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers));
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *curl)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_remove_handle, &mresult)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
    ಆದರೆ(!GOOD_EASY_HANDLE(ದತ್ತಾಂಶ) || ದತ್ತಾಂಶ->state.ಆಂತರಿಕ)
      mresult = CURLM_BAD_EASY_HANDLE;
    ಇಲ್ಲದಿದ್ದರೆ
      mresult = Curl_multi_remove_handle(m, ದತ್ತಾಂಶ);
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

/* Return TRUE if the application asked for multiplexing */
bool Curl_multiplex_wanted(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_multi *multi)
{
  ಹಿಂತಿರುಗಿ multi && multi->multiplexing;
}

/*
 * Curl_detach_connection() removes the given transfer from the connection.
 *
 * This is the only function that should clear data->conn. This will
 * occasionally be called with the data->conn pointer already cleared.
 */
ಶೂನ್ಯ Curl_detach_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಆದರೆ(conn) {
    /* this should never happen, prevent underflow */
    DEBUGASSERT(conn->attached_xfers);
    ಆದರೆ(conn->attached_xfers) {
      conn->attached_xfers--;
      ಆದರೆ(!conn->attached_xfers)
        conn->attached_multi = ಖಾಲಿ;
    }
  }
  ದತ್ತಾಂಶ->conn = ಖಾಲಿ;
}

/*
 * Curl_attach_connection() attaches this transfer to this connection.
 *
 * This is the only function that should assign data->conn.
 * `matched == TRUE` means the transfer's properties match this
 * connection and it is not a temporary attach for maintenance.
 */
ಶೂನ್ಯ Curl_attach_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ connectdata *conn,
                            bool matched)
{
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(!ದತ್ತಾಂಶ->conn);
  DEBUGASSERT(conn);
  DEBUGASSERT(conn->attached_xfers < UINT32_MAX);
  ದತ್ತಾಂಶ->conn = conn;
  ಆದರೆ(matched) {
    ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id = conn->connection_id;
    ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid = conn->cpid;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    DEBUGASSERT(!ದತ್ತಾಂಶ->mid); /* admin handle */
  conn->attached_xfers++;
  /* all attached transfers must be from the same multi */
  ಆದರೆ(!conn->attached_multi)
    conn->attached_multi = ದತ್ತಾಂಶ->multi;
  DEBUGASSERT(conn->attached_multi == ದತ್ತಾಂಶ->multi);

  ಆದರೆ(conn->scheme && conn->scheme->ಸತತ->attach)
    conn->scheme->ಸತತ->attach(ದತ್ತಾಂಶ, conn);
}

/* adjust pollset for rate limits/pauses */
ಸ್ಥಿರ CURLcode multi_adjust_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ easy_pollset *ps)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ps->n) {
    bool send_blocked, recv_blocked;

    recv_blocked = (Curl_rlimit_avail(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit, ಖಾಲಿ) <= 0);
    send_blocked = (Curl_rlimit_avail(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.rlimit, ಖಾಲಿ) <= 0);
    ಆದರೆ(send_blocked || recv_blocked) {
      ಪೂರ್ಣಾಂಕ i;
      ಗಾಗಿ(i = 0; i <= SECONDARYSOCKET; ++i) {
        curl_socket_t sock = ದತ್ತಾಂಶ->conn->sock[i];
        ಆದರೆ(sock == CURL_SOCKET_BAD)
          ಮುಂದುವರಿ;
        ಆದರೆ(recv_blocked && Curl_pollset_want_recv(ದತ್ತಾಂಶ, ps, sock)) {
          ಫಲಿತಾಂಶ = Curl_pollset_remove_ಒಳ(ದತ್ತಾಂಶ, ps, sock);
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ(send_blocked && Curl_pollset_want_send(ದತ್ತಾಂಶ, ps, sock)) {
          ಫಲಿತಾಂಶ = Curl_pollset_remove_ಹೊರಹೋಗು(ದತ್ತಾಂಶ, ps, sock);
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ನಿಲ್ಲಿಸು;
        }
      }
    }

    /* Not blocked and wanting to receive. If there is data pending
     * in the connection filters, make transfer run again. */
    ಆದರೆ(!recv_blocked &&
       ((Curl_pollset_want_recv(ದತ್ತಾಂಶ, ps, ದತ್ತಾಂಶ->conn->sock[FIRSTSOCKET]) &&
         Curl_conn_ದತ್ತಾಂಶ_pending(ದತ್ತಾಂಶ, FIRSTSOCKET)) ||
        (Curl_pollset_want_recv(ದತ್ತಾಂಶ, ps, ದತ್ತಾಂಶ->conn->sock[SECONDARYSOCKET]) &&
         Curl_conn_ದತ್ತಾಂಶ_pending(ದತ್ತಾಂಶ, SECONDARYSOCKET)))) {
      CURL_TRC_M(ದತ್ತಾಂಶ, "pollset[] has POLLIN, but there is still "
                 "buffered input -> mark as dirty");
      Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode mstate_connecting_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  curl_socket_t sockfd;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(Curl_xfer_recv_is_paused(ದತ್ತಾಂಶ))
    ಹಿಂತಿರುಗಿ CURLE_OK;
  /* If a socket is set, receiving is default. If the socket
   * has not been determined yet (eyeballing), always ask the
   * connection filters for what to monitor. */
  sockfd = Curl_conn_get_ಮೊದಲ_socket(ದತ್ತಾಂಶ);
  ಆದರೆ(sockfd != CURL_SOCKET_BAD) {
    ಫಲಿತಾಂಶ = Curl_pollset_change(ದತ್ತಾಂಶ, ps, sockfd, CURL_POLL_ಒಳ, 0);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = multi_adjust_pollset(ದತ್ತಾಂಶ, ps);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_conn_adjust_pollset(ದತ್ತಾಂಶ, conn, ps);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode mstate_protocol_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(conn->scheme->ಸತತ->proto_pollset)
    ಫಲಿತಾಂಶ = conn->scheme->ಸತತ->proto_pollset(ದತ್ತಾಂಶ, ps);
  ಇಲ್ಲದಿದ್ದರೆ {
    curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
    ಆದರೆ(sockfd != CURL_SOCKET_BAD) {
      /* Default is to wait to something from the server */
      ಫಲಿತಾಂಶ = Curl_pollset_change(ದತ್ತಾಂಶ, ps, sockfd, CURL_POLL_ಒಳ, 0);
    }
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = multi_adjust_pollset(ದತ್ತಾಂಶ, ps);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_conn_adjust_pollset(ದತ್ತಾಂಶ, conn, ps);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode mstate_do_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(conn->scheme->ಸತತ->doing_pollset)
    ಫಲಿತಾಂಶ = conn->scheme->ಸತತ->doing_pollset(ದತ್ತಾಂಶ, ps);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    ಫಲಿತಾಂಶ = Curl_pollset_ಕೂಡಿಸು_ಹೊರಹೋಗು(ದತ್ತಾಂಶ, ps, conn->sock[conn->send_idx]);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = multi_adjust_pollset(ದತ್ತಾಂಶ, ps);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_conn_adjust_pollset(ದತ್ತಾಂಶ, conn, ps);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode mstate_domore_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(conn->scheme->ಸತತ->domore_pollset)
    ಫಲಿತಾಂಶ = conn->scheme->ಸತತ->domore_pollset(ದತ್ತಾಂಶ, ps);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    ಫಲಿತಾಂಶ = Curl_pollset_ಕೂಡಿಸು_ಹೊರಹೋಗು(ದತ್ತಾಂಶ, ps, conn->sock[conn->send_idx]);
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = multi_adjust_pollset(ದತ್ತಾಂಶ, ps);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_conn_adjust_pollset(ದತ್ತಾಂಶ, conn, ps);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode mstate_perform_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(conn->scheme->ಸತತ->perform_pollset)
    ಫಲಿತಾಂಶ = conn->scheme->ಸತತ->perform_pollset(ದತ್ತಾಂಶ, ps);
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Default is to obey the request flags for send/recv */
    ಆದರೆ(Curl_req_want_recv(ದತ್ತಾಂಶ) && CONN_SOCK_IDX_VALID(conn->recv_idx)) {
      ಫಲಿತಾಂಶ = Curl_pollset_ಕೂಡಿಸು_ಒಳ(ದತ್ತಾಂಶ, ps, conn->sock[conn->recv_idx]);
    }
    ಆದರೆ(!ಫಲಿತಾಂಶ && Curl_req_want_send(ದತ್ತಾಂಶ) &&
       CONN_SOCK_IDX_VALID(conn->send_idx)) {
      ಫಲಿತಾಂಶ = Curl_pollset_ಕೂಡಿಸು_ಹೊರಹೋಗು(ದತ್ತಾಂಶ, ps, conn->sock[conn->send_idx]);
    }
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = multi_adjust_pollset(ದತ್ತಾಂಶ, ps);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_conn_adjust_pollset(ದತ್ತಾಂಶ, conn, ps);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರ size_t multi_timeouts_ಎಣಿಕೆ(ರಚನೆ expire_timers *timeouts)
{
  size_t n = 0;
  uint8_t eid = timeouts->ಮೊದಲ;
  ಗಾಗಿ(; eid < EXPIRE_ಕೊನೆಯ; eid = timeouts->ಮುಂದಿನ[eid])
    ++n;
  ಹಿಂತಿರುಗಿ n;
}
#ಆದರೆ_ಅಂತ್ಯ

/* Initializes `poll_set` with the current socket poll actions needed
 * for transfer `data`. */
CURLMcode Curl_multi_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ easy_pollset *ps)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  Curl_pollset_reset(ps);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_ಆಂತರಿಕ_WAKEUP
  /* The admin handle always listens on the wakeup socket when there
   * are transfers alive. */
  ಆದರೆ(ದತ್ತಾಂಶ->multi && (ದತ್ತಾಂಶ == ದತ್ತಾಂಶ->multi->admin) &&
     ದತ್ತಾಂಶ->multi->xfers_really_alive) {
    CURL_TRC_M(ದತ್ತಾಂಶ, "adding wakeup, %u xfers really alive",
               ದತ್ತಾಂಶ->multi->xfers_really_alive);
    ಫಲಿತಾಂಶ = Curl_pollset_ಕೂಡಿಸು_ಒಳ(ದತ್ತಾಂಶ, ps, ದತ್ತಾಂಶ->multi->wakeup_ಆಂತರಿಕ[0]);
  }
#ಆದರೆ_ಅಂತ್ಯ
  /* If the transfer has no connection, this is fine. Happens when
     called via curl_multi_remove_handle() => Curl_multi_ev_assess() =>
     Curl_multi_pollset(). */
  ಆದರೆ(!ಫಲಿತಾಂಶ && ದತ್ತಾಂಶ->conn) {
    ಆಯ್ಕೆ(ದತ್ತಾಂಶ->mstate) {
    ಸ್ಥಿತಿ MSTATE_INIT:
    ಸ್ಥಿತಿ MSTATE_PENDING:
    ಸ್ಥಿತಿ MSTATE_ಸಿದ್ಧತೆ:
    ಸ್ಥಿತಿ MSTATE_CONNECT:
      /* nothing to poll for yet */
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_CONNECTING:
      ಫಲಿತಾಂಶ = mstate_connecting_pollset(ದತ್ತಾಂಶ, ps);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_PROTOCONNECT:
    ಸ್ಥಿತಿ MSTATE_PROTOCONNECTING:
      ಫಲಿತಾಂಶ = mstate_protocol_pollset(ದತ್ತಾಂಶ, ps);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_DO:
    ಸ್ಥಿತಿ MSTATE_DOING:
      ಫಲಿತಾಂಶ = mstate_do_pollset(ದತ್ತಾಂಶ, ps);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_DOING_ಇನ್ನಷ್ಟು:
      ಫಲಿತಾಂಶ = mstate_domore_pollset(ದತ್ತಾಂಶ, ps);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_DID: /* same as PERFORMING in regard to polling */
    ಸ್ಥಿತಿ MSTATE_PERFORMING:
      ಫಲಿತಾಂಶ = mstate_perform_pollset(ದತ್ತಾಂಶ, ps);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_RATELIMITING:
      /* we need to let time pass, ignore socket(s) */
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_ಮುಗಿದ:
    ಸ್ಥಿತಿ MSTATE_COMPLETED:
    ಸ್ಥಿತಿ MSTATE_MSGSENT:
      /* nothing more to poll for */
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      failf(ದತ್ತಾಂಶ, "multi_getsock: unexpected multi state %d",
            (ಪೂರ್ಣಾಂಕ)ದತ್ತಾಂಶ->mstate);
      DEBUGASSERT(0);
      ನಿಲ್ಲಿಸು;
    }
  }

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
      ಹಿಂತಿರುಗಿ CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    failf(ದತ್ತಾಂಶ, "error determining pollset: %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಹಿಂತಿರುಗಿ CURLM_ಆಂತರಿಕ_ERROR;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆದರೆ(CURL_TRC_M_is_verbose(ದತ್ತಾಂಶ)) {
    size_t timeout_ಎಣಿಕೆ = multi_timeouts_ಎಣಿಕೆ(&ದತ್ತಾಂಶ->state.timeouts);
    ಆಯ್ಕೆ(ps->n) {
    ಸ್ಥಿತಿ 0:
      CURL_TRC_M(ದತ್ತಾಂಶ, "pollset[], timeouts=%zu, paused %d/%d (r/w)",
                 timeout_ಎಣಿಕೆ,
                 Curl_xfer_send_is_paused(ದತ್ತಾಂಶ),
                 Curl_xfer_recv_is_paused(ದತ್ತಾಂಶ));
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 1:
      CURL_TRC_M(ದತ್ತಾಂಶ, "pollset[fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_ಒಳ) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_ಹೊರಹೋಗು) ? "OUT" : "",
                 timeout_ಎಣಿಕೆ);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 2:
      CURL_TRC_M(ದತ್ತಾಂಶ, "pollset[fd=%" FMT_SOCKET_T " %s%s, "
                 "fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_ಒಳ) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_ಹೊರಹೋಗು) ? "OUT" : "",
                 ps->sockets[1],
                 (ps->actions[1] & CURL_POLL_ಒಳ) ? "IN" : "",
                 (ps->actions[1] & CURL_POLL_ಹೊರಹೋಗು) ? "OUT" : "",
                 timeout_ಎಣಿಕೆ);
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      CURL_TRC_M(ದತ್ತಾಂಶ, "pollset[fds=%u], timeouts=%zu", ps->n, timeout_ಎಣಿಕೆ);
      ನಿಲ್ಲಿಸು;
    }
    CURL_TRC_EASY_TIMERS(ದತ್ತಾಂಶ);
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ CURLM_OK;
}

CURLMcode curl_multi_fdset(CURLM *m,
                           fd_set *read_fd_ಗಣ, fd_set *write_fd_ಗಣ,
                           fd_set *exc_fd_ಗಣ, ಪೂರ್ಣಾಂಕ *max_fd)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_fdset, &mresult)) {
    /* Scan through all the easy handles to get the file descriptors set.
       Some easy handles may not have connected to the remote host yet,
       and then we must make sure that is done. */
    ರಚನೆ Curl_multi *multi = m;
    ರಚನೆ easy_pollset ps;
    ಪೂರ್ಣಾಂಕ this_max_fd = -1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
    uint32_t mid;
    (ಶೂನ್ಯ)exc_fd_ಗಣ;

    Curl_pollset_init(&ps);
    ಆದರೆ(Curl_uint32_bset_ಮೊದಲ(&multi->process, &mid)) {
      ಮಾಡು {
        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);

        ಆದರೆ(!ದತ್ತಾಂಶ) {
          DEBUGASSERT(0);
          ಮುಂದುವರಿ;
        }

        Curl_multi_pollset(ದತ್ತಾಂಶ, &ps);
        ಗಾಗಿ(i = 0; i < ps.n; i++) {
          ಆದರೆ(!FDSET_SOCK(ps.sockets[i]))
            /* pretend it does not exist */
            ಮುಂದುವರಿ;
          ಆದರೆ(ps.actions[i] & CURL_POLL_ಒಳ)
            FD_SET(ps.sockets[i], read_fd_ಗಣ);
          ಆದರೆ(ps.actions[i] & CURL_POLL_ಹೊರಹೋಗು)
            FD_SET(ps.sockets[i], write_fd_ಗಣ);
          ಆದರೆ((ಪೂರ್ಣಾಂಕ)ps.sockets[i] > this_max_fd)
            this_max_fd = (ಪೂರ್ಣಾಂಕ)ps.sockets[i];
        }
      } ಆಗ(Curl_uint32_bset_ಮುಂದಿನ(&multi->process, mid, &mid));
    }

    Curl_cshutdn_setfds(&multi->cshutdn, multi->admin,
                        read_fd_ಗಣ, write_fd_ಗಣ, &this_max_fd);

    *max_fd = this_max_fd;
    Curl_pollset_cleanup(&ps);

    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

CURLMcode curl_multi_waitfds(CURLM *m,
                             ರಚನೆ curl_waitfd *ufds,
                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2,
                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *fd_ಎಣಿಕೆ)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_waitfds, &mresult)) {
    ರಚನೆ Curl_waitfds cwfds;
    ರಚನೆ Curl_multi *multi = m;
    ರಚನೆ easy_pollset ps;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ need = 0;
    uint32_t mid;

    ಆದರೆ(!ufds && (ಗಾತ್ರ_2 || !fd_ಎಣಿಕೆ)) {
      mresult = CURLM_BAD_FUNCTION_ವಾದ;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    Curl_pollset_init(&ps);
    Curl_waitfds_init(&cwfds, ufds, ಗಾತ್ರ_2);
    mresult = CURLM_OK;
    ಆದರೆ(Curl_uint32_bset_ಮೊದಲ(&multi->process, &mid)) {
      ಮಾಡು {
        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);
        ಆದರೆ(!ದತ್ತಾಂಶ) {
          DEBUGASSERT(0);
          Curl_uint32_bset_remove(&multi->process, mid);
          Curl_uint32_bset_remove(&multi->dirty, mid);
          ಮುಂದುವರಿ;
        }
        Curl_multi_pollset(ದತ್ತಾಂಶ, &ps);
        need += Curl_waitfds_ಕೂಡಿಸು_ps(&cwfds, &ps);
      } ಆಗ(Curl_uint32_bset_ಮುಂದಿನ(&multi->process, mid, &mid));
    }

    need += Curl_cshutdn_ಕೂಡಿಸು_waitfds(&multi->cshutdn, multi->admin, &cwfds);

    ಆದರೆ(need != cwfds.n && ufds)
      mresult = CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    ಆದರೆ(fd_ಎಣಿಕೆ)
      *fd_ಎಣಿಕೆ = need;
    Curl_pollset_cleanup(&ps);
  }
ಹೊರಹೋಗು:
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
/* Reset FD_WRITE for TCP sockets. Nothing is actually sent. UDP sockets cannot
 * be reset this way because an empty datagram would be sent. #9203
 *
 * "On Windows the internal state of FD_WRITE as returned from
 * WSAEnumNetworkEvents is only reset after successful send()."
 */
ಸ್ಥಿರ ಶೂನ್ಯ reset_socket_fdwrite(curl_socket_t s)
{
  ಪೂರ್ಣಾಂಕ t;
  ಪೂರ್ಣಾಂಕ l = (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(t);
  ಆದರೆ(!getsockopt(s, SOL_SOCKET, SO_TYPE, (ಅಕ್ಷರ *)&t, &l) && t == SOCK_STREAM)
    swrite(s, ಖಾಲಿ, 0);
}

ಸ್ಥಿರ CURLMcode multi_winsock_select(ರಚನೆ Curl_multi *multi,
                                      ರಚನೆ curl_pollfds *cpfds,
                                      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ curl_nfds,
                                      ರಚನೆ curl_waitfd extra_fds[],
                                      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ extra_nfds,
                                      ಪೂರ್ಣಾಂಕ timeout_ms,
                                      bool extrawait,
                                      ಪೂರ್ಣಾಂಕ *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  WSANETWORKEVENTS wsa_events;
  ಪೂರ್ಣಾಂಕ nevents = 0;
  size_t i;

  DEBUGASSERT(multi->wsa_event != WSA_INVALID_EVENT);

  /* Set the WSA events based on the collected pollds */
  ಗಾಗಿ(i = 0; i < cpfds->n; i++) {
    ದೀರ್ಘ ಮುಖವಾಡ = 0;
    ಆದರೆ(cpfds->pfds[i].events & POLLIN)
      ಮುಖವಾಡ |= FD_READ | FD_ACCEPT | FD_CLOSE;
    ಆದರೆ(cpfds->pfds[i].events & POLLPRI)
      ಮುಖವಾಡ |= FD_OOB;
    ಆದರೆ(cpfds->pfds[i].events & POLLOUT) {
      ಮುಖವಾಡ |= FD_WRITE | FD_CONNECT | FD_CLOSE;
      reset_socket_fdwrite(cpfds->pfds[i].fd);
    }
    ಆದರೆ(ಮುಖವಾಡ && WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, ಮುಖವಾಡ)) {
      mresult = CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಆದರೆ(cpfds->n || extrawait) {
    ಪೂರ್ಣಾಂಕ pollrc = 0;

    ಆದರೆ(cpfds->n) {         /* pre-check with Winsock */
      pollrc = Curl_poll(cpfds->pfds, cpfds->n, 0);
      ಆದರೆ(pollrc < 0) {
        mresult = CURLM_UNRECOVERABLE_POLL;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      nevents = pollrc;
    }

    ಆದರೆ(!nevents) {
      /* now wait... if not ready during the pre-check (pollrc == 0) */
      WSAWait_For_ಅಪವರ್ತ್ಯ_Events(1, &multi->wsa_event, FALSE, (DWORD)timeout_ms,
                               FALSE);
    }

    /* With Winsock, we have to run the following section unconditionally
       to call WSAEventSelect(fd, event, 0) on all the sockets */
    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    ಗಾಗಿ(i = 0; i < extra_nfds; i++) {
      ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಮುಖವಾಡ = 0;
      curl_socket_t s = extra_fds[i].fd;

      wsa_events.l_ಜಾಲ_Events = 0;
      ಆದರೆ(WSAEnum_ಜಾಲ_Events(s, ಖಾಲಿ, &wsa_events) == 0) {
        ಆದರೆ(wsa_events.l_ಜಾಲ_Events & (FD_READ | FD_ACCEPT | FD_CLOSE))
          ಮುಖವಾಡ |= CURL_WAIT_POLLIN;
        ಆದರೆ(wsa_events.l_ಜಾಲ_Events & (FD_WRITE | FD_CONNECT | FD_CLOSE))
          ಮುಖವಾಡ |= CURL_WAIT_POLLOUT;
        ಆದರೆ(wsa_events.l_ಜಾಲ_Events & FD_OOB)
          ಮುಖವಾಡ |= CURL_WAIT_POLLPRI;
        ಆದರೆ(!pollrc && wsa_events.l_ಜಾಲ_Events)
          nevents++;
      }
      WSAEventSelect(s, multi->wsa_event, 0);
      ಆದರೆ(!pollrc) {
        extra_fds[i].revents = (ಗಿಡ್ಡ)ಮುಖವಾಡ;
        ಮುಂದುವರಿ;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಚಿಹ್ನೆರಹಿತ r = (ಚಿಹ್ನೆರಹಿತ)cpfds->pfds[curl_nfds + i].revents;
        ಆದರೆ(r & POLLIN)
          ಮುಖವಾಡ |= CURL_WAIT_POLLIN;
        ಆದರೆ(r & POLLOUT)
          ಮುಖವಾಡ |= CURL_WAIT_POLLOUT;
        ಆದರೆ(r & POLLPRI)
          ಮುಖವಾಡ |= CURL_WAIT_POLLPRI;
        extra_fds[i].revents = (ಗಿಡ್ಡ)ಮುಖವಾಡ;
      }
    }

    /* Count up all our own sockets that had activity,
       and remove them from the event. */
    ಗಾಗಿ(i = 0; i < curl_nfds; ++i) {
      wsa_events.l_ಜಾಲ_Events = 0;
      ಆದರೆ(WSAEnum_ಜಾಲ_Events(cpfds->pfds[i].fd, ಖಾಲಿ, &wsa_events) == 0) {
        ಆದರೆ(!pollrc && wsa_events.l_ಜಾಲ_Events)
          nevents++;
      }
      WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, 0);
    }
    WSAResetEvent(multi->wsa_event);
  }

ಹೊರಹೋಗು:
  *pnevents = nevents;
  ಹಿಂತಿರುಗಿ mresult;
}

#ಇಲ್ಲದಿದ್ದರೆ /* USE_WINSOCK */

ಸ್ಥಿರ CURLMcode multi_posix_poll(ರಚನೆ Curl_multi *multi,
                                  ರಚನೆ curl_pollfds *cpfds,
                                  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ curl_nfds,
                                  ರಚನೆ curl_waitfd extra_fds[],
                                  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ extra_nfds,
                                  ಪೂರ್ಣಾಂಕ timeout_ms,
                                  bool extrawait,
                                  ಪೂರ್ಣಾಂಕ *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  ಪೂರ್ಣಾಂಕ nevents = 0;
  size_t i;

  (ಶೂನ್ಯ)multi;
  ಆದರೆ(cpfds->n) {
    ಪೂರ್ಣಾಂಕ pollrc = Curl_poll(cpfds->pfds, cpfds->n, timeout_ms); /* wait... */
    ಆದರೆ(pollrc < 0) {
      mresult = CURLM_UNRECOVERABLE_POLL;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    nevents = pollrc;

    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    ಗಾಗಿ(i = 0; i < extra_nfds; i++) {
      ಚಿಹ್ನೆರಹಿತ r = (ಚಿಹ್ನೆರಹಿತ)cpfds->pfds[curl_nfds + i].revents;
      ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಮುಖವಾಡ = 0;
      ಆದರೆ(r & POLLIN)
        ಮುಖವಾಡ |= CURL_WAIT_POLLIN;
      ಆದರೆ(r & POLLOUT)
        ಮುಖವಾಡ |= CURL_WAIT_POLLOUT;
      ಆದರೆ(r & POLLPRI)
        ಮುಖವಾಡ |= CURL_WAIT_POLLPRI;
      extra_fds[i].revents = (ಗಿಡ್ಡ)ಮುಖವಾಡ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(extrawait) {
    /* No fds to poll, but asked to obey timeout_ms anyway. We cannot
     * use Curl_poll() as it, on some platforms, returns immediately
     * without fds. */
    curlx_wait_ms(timeout_ms);
  }

ಹೊರಹೋಗು:
  *pnevents = nevents;
  ಹಿಂತಿರುಗಿ mresult;
}

#ಆದರೆ_ಅಂತ್ಯ /* !USE_WINSOCK */

#ವ್ಯಾಖ್ಯಾನಿಸು ಅಂಕೆ_POLLS_ON_ರಾಶಿ 10

ಸ್ಥಿರ CURLMcode multi_wait(ರಚನೆ Curl_multi *multi,
                            ರಚನೆ curl_waitfd extra_fds[],
                            ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ extra_nfds,
                            ಪೂರ್ಣಾಂಕ timeout_ms,
                            ಪೂರ್ಣಾಂಕ *ret,
                            bool extrawait)  /* when no socket, wait */
{
  size_t i;
  ಪೂರ್ಣಾಂಕ timeout_ಆಂತರಿಕ;
  ಪೂರ್ಣಾಂಕ nevents = 0;
  ರಚನೆ easy_pollset ps;
  ರಚನೆ pollfd a_few_on_ರಾಶಿ[ಅಂಕೆ_POLLS_ON_ರಾಶಿ];
  ರಚನೆ curl_pollfds cpfds;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ curl_nfds = 0; /* how many pfds are for curl transfers */
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = ಖಾಲಿ;
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_WAKEUP
  ಪೂರ್ಣಾಂಕ wakeup_idx = -1;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(timeout_ms < 0)
    ಹಿಂತಿರುಗಿ CURLM_BAD_FUNCTION_ವಾದ;

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ರಾಶಿ, ಅಂಕೆ_POLLS_ON_ರಾಶಿ);

  /* Add the curl handles to our pollfds first */
  ಆದರೆ(Curl_uint32_bset_ಮೊದಲ(&multi->process, &mid)) {
    ಮಾಡು {
      ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);
      ಆದರೆ(!ದತ್ತಾಂಶ) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        ಮುಂದುವರಿ;
      }
      Curl_multi_pollset(ದತ್ತಾಂಶ, &ps);
      ಆದರೆ(Curl_pollfds_ಕೂಡಿಸು_ps(&cpfds, &ps)) {
        mresult = CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    } ಆಗ(Curl_uint32_bset_ಮುಂದಿನ(&multi->process, mid, &mid));
  }

  ಆದರೆ(Curl_cshutdn_ಕೂಡಿಸು_pollfds(&multi->cshutdn, multi->admin, &cpfds)) {
    mresult = CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_WAKEUP
  /* If `extrawait` is TRUE *or* we have `extra_fds`to poll *or* we
   * have transfer sockets to poll, we obey `timeout_ms`.
   * Then we need to also monitor the multi's wakeup
   * socket to catch calls to `curl_multi_wakeup()` during the wait. */
  ಆದರೆ(extrawait || cpfds.n || extra_nfds) {
    wakeup_idx = cpfds.n;
    ಆದರೆ(Curl_pollfds_ಕೂಡಿಸು_sock(&cpfds, multi->wakeup_pair[0], POLLIN)) {
      mresult = CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  curl_nfds = cpfds.n; /* what curl internally uses in cpfds */
  /* Add external file descriptions from poll-like struct curl_waitfd */
  ಗಾಗಿ(i = 0; i < extra_nfds; i++) {
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ events = 0;
    ಆದರೆ(extra_fds[i].events & CURL_WAIT_POLLIN)
      events |= POLLIN;
    ಆದರೆ(extra_fds[i].events & CURL_WAIT_POLLPRI)
      events |= POLLPRI;
    ಆದರೆ(extra_fds[i].events & CURL_WAIT_POLLOUT)
      events |= POLLOUT;
    ಆದರೆ(Curl_pollfds_ಕೂಡಿಸು_sock(&cpfds, extra_fds[i].fd, events)) {
      mresult = CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  /* We check the internal timeout *AFTER* we collected all sockets to
   * poll. Collecting the sockets may install new timers by protocols
   * and connection filters.
   * Use the shorter one of the internal and the caller requested timeout.
   * If we are called with `!extrawait` and multi_timeout() reports no
   * timeouts exist, do not wait. */
  multi_timeout(multi, ಖಾಲಿ, &timeout_ಆಂತರಿಕ);
  ಆದರೆ((timeout_ಆಂತರಿಕ >= 0) && (timeout_ಆಂತರಿಕ < timeout_ms))
    timeout_ms = timeout_ಆಂತರಿಕ;

  ಆದರೆ(ದತ್ತಾಂಶ)
    CURL_TRC_M(ದತ್ತಾಂಶ, "multi_wait(fds=%u, timeout=%d) tinternal=%d",
               cpfds.n, timeout_ms, timeout_ಆಂತರಿಕ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
  mresult = multi_winsock_select(multi, &cpfds, curl_nfds,
                                 extra_fds, extra_nfds,
                                 timeout_ms, extrawait, &nevents);
#ಇಲ್ಲದಿದ್ದರೆ
  mresult = multi_posix_poll(multi, &cpfds, curl_nfds,
                             extra_fds, extra_nfds,
                             timeout_ms, extrawait, &nevents);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_WAKEUP
    ಆದರೆ(nevents && (wakeup_idx >= 0)) {
      ಆದರೆ(cpfds.pfds[wakeup_idx].revents & POLLIN) {
        (ಶೂನ್ಯ)Curl_wakeup_consume(multi->wakeup_pair, TRUE);
        /* do not count the wakeup socket into the returned value */
        nevents--;
      }
    }
#ಆದರೆ_ಅಂತ್ಯ

ಹೊರಹೋಗು:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  ಆದರೆ(ret)
    *ret = nevents;
  ಹಿಂತಿರುಗಿ mresult;
}

CURLMcode curl_multi_wait(CURLM *m,
                          ರಚನೆ curl_waitfd extra_fds[],
                          ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ extra_nfds,
                          ಪೂರ್ಣಾಂಕ timeout_ms,
                          ಪೂರ್ಣಾಂಕ *ret)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_wait, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, FALSE);
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

CURLMcode curl_multi_poll(CURLM *m,
                          ರಚನೆ curl_waitfd extra_fds[],
                          ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ extra_nfds,
                          ಪೂರ್ಣಾಂಕ timeout_ms,
                          ಪೂರ್ಣಾಂಕ *ret)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_poll, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, TRUE);
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

CURLMcode curl_multi_wakeup(CURLM *m)
{
  /* this function is usually called from another thread,
     it has to be careful only to access parts of the
     Curl_multi struct that are constant */
  ರಚನೆ Curl_multi *multi = m;
  CURLMcode mresult = CURLM_WAKEUP_FAILURE;

  /* GOOD_MULTI_HANDLE can be safely called */
  ಆದರೆ(!GOOD_MULTI_HANDLE(multi))
    ಹಿಂತಿರುಗಿ CURLM_BAD_HANDLE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_WAKEUP
  /* the wakeup_pair variable is only written during init and cleanup,
     making it safe to access from another thread after the init part
     and before cleanup */
  ಆದರೆ(!Curl_wakeup_signal(multi->wakeup_pair))
    mresult = CURLM_OK;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
  ಆದರೆ(WSASetEvent(multi->wsa_event))
    mresult = CURLM_OK;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ mresult;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_ಆಂತರಿಕ_WAKEUP
ಶೂನ್ಯ Curl_multi_wakeup_ಆಂತರಿಕ(ರಚನೆ Curl_multi *multi)
{
  /* This is expected to be invocable from another thread which
   * does NOT outlive the multi handle. Check for sanity. */
  ಆದರೆ(GOOD_MULTI_HANDLE(multi))
    Curl_wakeup_signal(multi->wakeup_ಆಂತರಿಕ);
  ಇಲ್ಲದಿದ್ದರೆ
    DEBUGASSERT(0);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * multi_ischanged() is called
 *
 * Returns TRUE/FALSE whether the state is changed to trigger a CONNECT_PEND
 * => CONNECT action.
 *
 * Set 'clear' to TRUE to have it also clear the state variable.
 */
ಸ್ಥಿರ bool multi_ischanged(ರಚನೆ Curl_multi *multi, bool clear)
{
  bool retval = FALSE;
  DEBUGASSERT(multi);
  ಆದರೆ(multi) {
    retval = (bool)multi->recheckstate;
    ಆದರೆ(clear)
      multi->recheckstate = FALSE;
  }
  ಹಿಂತಿರುಗಿ retval;
}

/*
 * Curl_multi_connchanged() is called to tell that there is a connection in
 * this multi handle that has changed state (multiplexing become possible, the
 * number of allowed streams changed or similar), and a subsequent use of this
 * multi handle should move CONNECT_PEND handles back to CONNECT to have them
 * retry.
 */
ಶೂನ್ಯ Curl_multi_connchanged(ರಚನೆ Curl_multi *multi)
{
  /* May be invoked for a `data->multi` when no multi is present. */
  ಆದರೆ(multi)
    multi->recheckstate = TRUE;
}

CURLMcode Curl_multi_ಕೂಡಿಸು_perform(ರಚನೆ Curl_multi *multi,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ connectdata *conn)
{
  CURLMcode mresult;

  mresult = Curl_multi_ಕೂಡಿಸು_handle(multi, ದತ್ತಾಂಶ);
  ಆದರೆ(!mresult) {
    CURLcode ಫಲಿತಾಂಶ;

    /* pass in NULL for 'conn' here since we do not want to init the
       connection, only this transfer */
    ಫಲಿತಾಂಶ = Curl_init_transfer(ದತ್ತಾಂಶ, ಖಾಲಿ);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      Curl_multi_remove_handle(multi, ದತ್ತಾಂಶ);
      ಹಿಂತಿರುಗಿ CURLM_ಆಂತರಿಕ_ERROR;
    }

    /* take this handle to the perform state right away */
    multistate(ದತ್ತಾಂಶ, MSTATE_PERFORMING);
    Curl_attach_connection(ದತ್ತಾಂಶ, conn, TRUE);
    CURL_REQ_ಗಣ_RECV(ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ mresult;
}

ಸ್ಥಿರ CURLcode multi_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(conn->scheme);

  ಆದರೆ(conn->scheme->ಸತತ->do_it)
    ಫಲಿತಾಂಶ = conn->scheme->ಸತತ->do_it(ದತ್ತಾಂಶ, ಮುಗಿದ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * multi_do_more() is called during the DO_MORE multi state. It is a second
 * stage DO state which (wrongly) was introduced to support FTP's second
 * connection.
 *
 * 'complete' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (to DOING state when there is more work to do)
 */

ಸ್ಥಿರ CURLcode multi_do_ಇನ್ನಷ್ಟು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, domore *complete)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  *complete = DOMORE_INCOMPLETE;

  ಆದರೆ(conn->scheme->ಸತತ->do_ಇನ್ನಷ್ಟು)
    ಫಲಿತಾಂಶ = conn->scheme->ಸತತ->do_ಇನ್ನಷ್ಟು(ದತ್ತಾಂಶ, complete);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Check whether a timeout occurred, and handle it if it did
 */
ಸ್ಥಿರ bool multi_handle_timeout(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow,
                                 bool *ಹರಿವು_error,
                                 CURLcode *ಫಲಿತಾಂಶ)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_timeleft_now_ms(ದತ್ತಾಂಶ, pnow);
  ಆದರೆ(timeout_ms < 0) {
    /* Handle timed out */
    timerid ತಳ_timer = Curl_is_connecting(ದತ್ತಾಂಶ) ?
                         TIMER_STARTSINGLE : TIMER_STARTOP;
    timediff_t elapsed_ms = Curl_pgrs_since_ms(ದತ್ತಾಂಶ, ಖಾಲಿ, ತಳ_timer);
    ಆದರೆ(ದತ್ತಾಂಶ->mstate == MSTATE_CONNECTING) {
      failf(ದತ್ತಾಂಶ, "%s timed out after %" FMT_TIMEDIFF_T " milliseconds",
            ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.dns_resolved ? "Connection" : "Resolving",
            elapsed_ms);
      /* keep what we know about a partly done connect, e.g. TCP done
       * but TLS handshake still ongoing */
      Curl_conn_report_failed(ದತ್ತಾಂಶ, FIRSTSOCKET);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
      ಆದರೆ(k->ಗಾತ್ರ_2 != -1) {
        failf(ದತ್ತಾಂಶ, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount, k->ಗಾತ್ರ_2);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        failf(ದತ್ತಾಂಶ, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount);
      }
    }
    *ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
    ಆದರೆ(ದತ್ತಾಂಶ->conn) {
      /* Force connection closed if the connection has indeed been used */
      ಆದರೆ(ದತ್ತಾಂಶ->mstate > MSTATE_DO) {
        streamclose(ದತ್ತಾಂಶ->conn);
        *ಹರಿವು_error = TRUE;
      }
      (ಶೂನ್ಯ)multi_ಮುಗಿದ(ದತ್ತಾಂಶ, *ಫಲಿತಾಂಶ, TRUE);
    }
    ಹಿಂತಿರುಗಿ TRUE;
  }

  ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * We are doing protocol-specific connecting and this is being called over and
 * over from the multi interface until the connection phase is done on
 * protocol layer.
 */

ಸ್ಥಿರ CURLcode protocol_connecting(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  ಆದರೆ(conn && conn->scheme->ಸತತ->connecting) {
    *ಮುಗಿದ = FALSE;
    ಫಲಿತಾಂಶ = conn->scheme->ಸತತ->connecting(ದತ್ತಾಂಶ, ಮುಗಿದ);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    *ಮುಗಿದ = TRUE;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * We are DOING this is being called over and over from the multi interface
 * until the DOING phase is done on protocol layer.
 */

ಸ್ಥಿರ CURLcode protocol_doing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  ಆದರೆ(conn && conn->scheme->ಸತತ->doing) {
    *ಮುಗಿದ = FALSE;
    ಫಲಿತಾಂಶ = conn->scheme->ಸತತ->doing(ದತ್ತಾಂಶ, ಮುಗಿದ);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    *ಮುಗಿದ = TRUE;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * We have discovered that the TCP connection has been successful, we can now
 * proceed with some action.
 *
 */
ಸ್ಥಿರ CURLcode protocol_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *protocol_ಮುಗಿದ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(conn);
  DEBUGASSERT(protocol_ಮುಗಿದ);
  DEBUGASSERT(Curl_conn_is_connected(conn, FIRSTSOCKET));

  *protocol_ಮುಗಿದ = FALSE;
  ಆದರೆ(!conn->ಬಿಟ್ಟುಗಳು.protoconnstart) {
    ಆದರೆ(conn->scheme->ಸತತ->connect_it) {
      /* Call the protocol-specific connect function */
      ಫಲಿತಾಂಶ = conn->scheme->ಸತತ->connect_it(ದತ್ತಾಂಶ, protocol_ಮುಗಿದ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    conn->ಬಿಟ್ಟುಗಳು.protoconnstart = TRUE;
  }

  /* Unless this protocol does not have any protocol-connect callback, as
     then we know we are done. */
  ಆದರೆ(!conn->scheme->ಸತತ->connecting)
    *protocol_ಮುಗಿದ = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * posttransfer() is called immediately after a transfer ends
 */
ಸ್ಥಿರ ಶೂನ್ಯ multi_posttransfer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
#ಆದರೆ defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
  /* restore the signal handler for SIGPIPE before we get back */
  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.no_signal)
    ಸಂಕೇತ(SIGPIPE, ದತ್ತಾಂಶ->state.prev_signal);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * multi_follow() handles the URL redirect magic. Pass in the 'newurl' string
 * as given by the remote server and set up the new URL to request.
 *
 * This function DOES NOT FREE the given URL.
 */
ಸ್ಥಿರ CURLcode multi_follow(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *handler,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newurl, /* the Location: string */
                             followtype type) /* see transfer.h */
{
  ಆದರೆ(handler && handler->ಸತತ->follow)
    ಹಿಂತಿರುಗಿ handler->ಸತತ->follow(ದತ್ತಾಂಶ, newurl, type);

  ಆದರೆ(type == FOLLOW_RETRY)
    /* Retries are generic and do not require protocol-specific redirect
       handling. */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಹಿಂತಿರುಗಿ CURLE_TOO_MANY_REDIRECTS;
}

ಸ್ಥಿರ CURLcode mspeed_check(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(Curl_rlimit_ಸಕ್ರಿಯ(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit) ||
     Curl_rlimit_ಸಕ್ರಿಯ(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.rlimit)) {
    /* check if our send/recv limits require idle waits */
    ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
    timediff_t recv_ms, send_ms;

    send_ms = Curl_rlimit_wait_ms(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.rlimit, pnow);
    recv_ms = Curl_rlimit_wait_ms(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit, pnow);

    ಆದರೆ(send_ms || recv_ms) {
      ಆದರೆ(ದತ್ತಾಂಶ->mstate != MSTATE_RATELIMITING) {
        multistate(ದತ್ತಾಂಶ, MSTATE_RATELIMITING);
      }
      Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_TOOFAST, CURLMAX(send_ms, recv_ms), pnow);
      Curl_multi_clear_dirty(ದತ್ತಾಂಶ);
      CURL_TRC_M(ದತ್ತಾಂಶ, "[RLIMIT] waiting %" FMT_TIMEDIFF_T "ms",
                 CURLMAX(send_ms, recv_ms));
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* when will the rate limits increase next? The transfer needs
       * to run again at that time or it may stall. */
      send_ms = Curl_rlimit_ಮುಂದಿನ_ಹೆಜ್ಜೆ_ms(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.rlimit, pnow);
      recv_ms = Curl_rlimit_ಮುಂದಿನ_ಹೆಜ್ಜೆ_ms(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit, pnow);
      ಆದರೆ(send_ms || recv_ms) {
        timediff_t ಮುಂದಿನ_ms = CURLMIN(send_ms, recv_ms);
        ಆದರೆ(!ಮುಂದಿನ_ms)
          ಮುಂದಿನ_ms = CURLMAX(send_ms, recv_ms);
        Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_TOOFAST, ಮುಂದಿನ_ms, pnow);
        CURL_TRC_M(ದತ್ತಾಂಶ, "[RLIMIT] next token update in %" FMT_TIMEDIFF_T "ms",
                   ಮುಂದಿನ_ms);
      }
    }
  }

  ಆದರೆ(ದತ್ತಾಂಶ->mstate != MSTATE_PERFORMING) {
    CURL_TRC_M(ದತ್ತಾಂಶ, "[RLIMIT] wait over, continue");
    multistate(ದತ್ತಾಂಶ, MSTATE_PERFORMING);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLMcode multistate_performing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       bool *ಹರಿವು_errorp,
                                       CURLcode *resultp)
{
  ಅಕ್ಷರ *newurl = ಖಾಲಿ;
  bool retry = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode ಫಲಿತಾಂಶ = *resultp = CURLE_OK;
  *ಹರಿವು_errorp = FALSE;

  ಆದರೆ(mspeed_check(ದತ್ತಾಂಶ) == CURLE_AGAIN)
    ಹಿಂತಿರುಗಿ CURLM_OK;

  /* read/write data if it is ready to do so */
  ಫಲಿತಾಂಶ = Curl_sendrecv(ದತ್ತಾಂಶ);

  ಆದರೆ(ದತ್ತಾಂಶ->req.ಮುಗಿದ || (ಫಲಿತಾಂಶ == CURLE_RECV_ERROR)) {
    /* If CURLE_RECV_ERROR happens early enough, we assume it was a race
     * condition and the server closed the reused connection exactly when we
     * wanted to use it, so figure out if that is indeed the case.
     */
    CURLcode ret = Curl_retry_request(ದತ್ತಾಂಶ, &newurl);
    ಆದರೆ(!ret)
      retry = !!newurl;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = ret;

    ಆದರೆ(retry) {
      /* if we are to retry, set the result to OK and consider the
         request as done */
      ಫಲಿತಾಂಶ = CURLE_OK;
      ದತ್ತಾಂಶ->req.ಮುಗಿದ = TRUE;
    }
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಫಲಿತಾಂಶ == CURLE_HTTP2_ಹರಿವು) &&
          Curl_h2_http_1_1_error(ದತ್ತಾಂಶ)) {
    CURLcode ret = Curl_retry_request(ದತ್ತಾಂಶ, &newurl);

    ಆದರೆ(!ret) {
      infof(ದತ್ತಾಂಶ, "Downgrades to HTTP/1.1");
      streamclose(ದತ್ತಾಂಶ->conn);
      ದತ್ತಾಂಶ->state.http_neg.wanted = CURL_HTTP_V1x;
      ದತ್ತಾಂಶ->state.http_neg.allowed = CURL_HTTP_V1x;
      /* clear the error message bit too as we ignore the one we got */
      ದತ್ತಾಂಶ->state.errorbuf = FALSE;
      ಆದರೆ(!newurl)
        /* typically for HTTP_1_1_REQUIRED error on first flight */
        newurl = Curl_bufref_dup(&ದತ್ತಾಂಶ->state.url);
      ಆದರೆ(!newurl) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* if we are to retry, set the result to OK and consider the request
          as done */
        retry = TRUE;
        ಫಲಿತಾಂಶ = CURLE_OK;
        ದತ್ತಾಂಶ->req.ಮುಗಿದ = TRUE;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = ret;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    /*
     * The transfer phase returned error, we mark the connection to get closed
     * to prevent being reused. This is because we cannot possibly know if the
     * connection is in a good shape or not now. Unless it is a protocol which
     * uses two "channels" like FTP, as then the error happened in the data
     * connection.
     */

    ಆದರೆ(!(ದತ್ತಾಂಶ->conn->scheme->flags & PROTOPT_DUAL) &&
       ಫಲಿತಾಂಶ != CURLE_HTTP2_ಹರಿವು)
      streamclose(ದತ್ತಾಂಶ->conn);

    multi_posttransfer(ದತ್ತಾಂಶ);
    multi_ಮುಗಿದ(ದತ್ತಾಂಶ, ಫಲಿತಾಂಶ, TRUE);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->req.ಮುಗಿದ && !Curl_cwriter_is_paused(ದತ್ತಾಂಶ)) {
    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *handler = ದತ್ತಾಂಶ->conn->scheme;

    /* call this even if the readwrite function returned error */
    multi_posttransfer(ದತ್ತಾಂಶ);

    /* When we follow redirects or is set to retry the connection, we must to
       go back to the CONNECT state */
    ಆದರೆ(ದತ್ತಾಂಶ->req.newurl || retry) {
      followtype follow = FOLLOW_NONE;
      ಆದರೆ(!retry) {
        /* if the URL is a follow-location and not a retried request then
           figure out the URL here */
        curlx_free(newurl);
        newurl = ದತ್ತಾಂಶ->req.newurl;
        ದತ್ತಾಂಶ->req.newurl = ಖಾಲಿ;
        follow = FOLLOW_REDIR;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        follow = FOLLOW_RETRY;
      (ಶೂನ್ಯ)multi_ಮುಗಿದ(ದತ್ತಾಂಶ, CURLE_OK, FALSE);
      /* multi_done() might return CURLE_GOT_NOTHING */
      ಫಲಿತಾಂಶ = multi_follow(ದತ್ತಾಂಶ, handler, newurl, follow);
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        multistate(ದತ್ತಾಂಶ, MSTATE_ಸಿದ್ಧತೆ);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* after the transfer is done, go DONE */

      /* but first check to see if we got a location info even though we are
         not following redirects */
      ಆದರೆ(ದತ್ತಾಂಶ->req.location) {
        curlx_free(newurl);
        newurl = ದತ್ತಾಂಶ->req.location;
        ದತ್ತಾಂಶ->req.location = ಖಾಲಿ;
        ಫಲಿತಾಂಶ = multi_follow(ದತ್ತಾಂಶ, handler, newurl, FOLLOW_FAKE);
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          *ಹರಿವು_errorp = TRUE;
          ಫಲಿತಾಂಶ = multi_ಮುಗಿದ(ದತ್ತಾಂಶ, ಫಲಿತಾಂಶ, TRUE);
        }
      }

      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        multistate(ದತ್ತಾಂಶ, MSTATE_ಮುಗಿದ);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ { /* not errored, not done */
    mspeed_check(ದತ್ತಾಂಶ);
  }
  curlx_free(newurl);
  *resultp = ಫಲಿತಾಂಶ;
  ಹಿಂತಿರುಗಿ mresult;
}

ಸ್ಥಿರ CURLMcode multistate_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               bool *ಹರಿವು_errorp,
                               CURLcode *resultp)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.fprereq) {
    ರಚನೆ Curl_mapi_guard guard;
    ಪೂರ್ಣಾಂಕ prereq_rc;

    /* call the prerequest callback function */
    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_fprereq);
    prereq_rc = ದತ್ತಾಂಶ->ಗಣ.fprereq(ದತ್ತಾಂಶ->ಗಣ.prereq_userp,
                                  ದತ್ತಾಂಶ->info.primary.remote_ip,
                                  ದತ್ತಾಂಶ->info.primary.local_ip,
                                  ದತ್ತಾಂಶ->info.primary.remote_ಪೋರ್ಟ್,
                                  ದತ್ತಾಂಶ->info.primary.local_ಪೋರ್ಟ್);
    CURL_CBAPI_ಕೊನೆ(&guard);
    ಆದರೆ(prereq_rc != CURL_PREREQFUNC_OK) {
      failf(ದತ್ತಾಂಶ, "operation aborted by pre-request callback");
      /* failure in pre-request callback - do not do any other processing */
      ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
      multi_posttransfer(ದತ್ತಾಂಶ);
      multi_ಮುಗಿದ(ದತ್ತಾಂಶ, ಫಲಿತಾಂಶ, FALSE);
      *ಹರಿವು_errorp = TRUE;
      ಹೋಗು ಕೊನೆ;
    }
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.connect_only && !ದತ್ತಾಂಶ->ಗಣ.connect_only_ws) {
    multistate(ದತ್ತಾಂಶ, MSTATE_ಮುಗಿದ);
    mresult = CURLM_CALL_MULTI_PERFORM;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    bool dophase_ಮುಗಿದ = FALSE;
    /* Perform the protocol's DO action */
    ಫಲಿತಾಂಶ = multi_do(ದತ್ತಾಂಶ, &dophase_ಮುಗಿದ);

    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಆದರೆ(!dophase_ಮುಗಿದ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
        /* some steps needed for wildcard matching */
        ಆದರೆ(ದತ್ತಾಂಶ->state.wildcardmatch) {
          ರಚನೆ Wildcard_ದತ್ತಾಂಶ *wc = ದತ್ತಾಂಶ->wildcard;
          ಆದರೆ(wc->state == CURLWC_ಮುಗಿದ || wc->state == CURLWC_SKIP) {
            /* skip some states if it is important */
            multi_ಮುಗಿದ(ದತ್ತಾಂಶ, CURLE_OK, FALSE);

            /* if there is no connection left, skip the DONE state */
            multistate(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn ? MSTATE_ಮುಗಿದ : MSTATE_COMPLETED);
            mresult = CURLM_CALL_MULTI_PERFORM;
            ಹೋಗು ಕೊನೆ;
          }
        }
#ಆದರೆ_ಅಂತ್ಯ
        /* DO was not completed in one function call, we must continue
           DOING... */
        multistate(ದತ್ತಾಂಶ, MSTATE_DOING);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }

      /* after DO, go DO_DONE... or DO_MORE */
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.do_ಇನ್ನಷ್ಟು) {
        /* we are supposed to do more, but we need to sit down, relax and wait
           a little while first */
        multistate(ದತ್ತಾಂಶ, MSTATE_DOING_ಇನ್ನಷ್ಟು);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* we are done with the DO, now DID */
        multistate(ದತ್ತಾಂಶ, MSTATE_DID);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಫಲಿತಾಂಶ == CURLE_SEND_ERROR) &&
            ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.reuse) {
      /*
       * In this situation, a connection that we were trying to use may have
       * unexpectedly died. If possible, send the connection back to the
       * CONNECT phase so we can try again.
       */
      ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *handler = ದತ್ತಾಂಶ->conn->scheme;
      ಅಕ್ಷರ *newurl = ಖಾಲಿ;
      followtype follow = FOLLOW_NONE;
      CURLcode drc;

      drc = Curl_retry_request(ದತ್ತಾಂಶ, &newurl);
      ಆದರೆ(drc) {
        /* a failure here pretty much implies an out of memory */
        ಫಲಿತಾಂಶ = drc;
        *ಹರಿವು_errorp = TRUE;
      }

      multi_posttransfer(ದತ್ತಾಂಶ);
      drc = multi_ಮುಗಿದ(ದತ್ತಾಂಶ, ಫಲಿತಾಂಶ, FALSE);

      /* When set to retry the connection, we must go back to the CONNECT
       * state */
      ಆದರೆ(newurl) {
        ಆದರೆ(!drc || (drc == CURLE_SEND_ERROR)) {
          follow = FOLLOW_RETRY;
          drc = multi_follow(ದತ್ತಾಂಶ, handler, newurl, follow);
          ಆದರೆ(!drc) {
            multistate(ದತ್ತಾಂಶ, MSTATE_ಸಿದ್ಧತೆ);
            mresult = CURLM_CALL_MULTI_PERFORM;
            ಫಲಿತಾಂಶ = CURLE_OK;
          }
          ಇಲ್ಲದಿದ್ದರೆ {
            /* Follow failed */
            ಫಲಿತಾಂಶ = drc;
          }
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          /* done did not return OK or SEND_ERROR */
          ಫಲಿತಾಂಶ = drc;
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* Have error handler disconnect conn if we cannot retry */
        *ಹರಿವು_errorp = TRUE;
      }
      curlx_free(newurl);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* failure detected */
      multi_posttransfer(ದತ್ತಾಂಶ);
      ಆದರೆ(ದತ್ತಾಂಶ->conn)
        multi_ಮುಗಿದ(ದತ್ತಾಂಶ, ಫಲಿತಾಂಶ, FALSE);
      *ಹರಿವು_errorp = TRUE;
    }
  }
ಕೊನೆ:
  *resultp = ಫಲಿತಾಂಶ;
  ಹಿಂತಿರುಗಿ mresult;
}

ಸ್ಥಿರ CURLMcode multistate_ratelimiting(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         CURLcode *resultp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  CURLMcode mresult = CURLM_OK;
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  /* if both rates are within spec, resume transfer */
  ಫಲಿತಾಂಶ = Curl_pgrsCheck(ದತ್ತಾಂಶ);

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(!(ದತ್ತಾಂಶ->conn->scheme->flags & PROTOPT_DUAL) &&
       ಫಲಿತಾಂಶ != CURLE_HTTP2_ಹರಿವು)
      streamclose(ದತ್ತಾಂಶ->conn);

    multi_posttransfer(ದತ್ತಾಂಶ);
    multi_ಮುಗಿದ(ದತ್ತಾಂಶ, ಫಲಿತಾಂಶ, TRUE);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(!mspeed_check(ದತ್ತಾಂಶ))
      mresult = CURLM_CALL_MULTI_PERFORM;
  }
  *resultp = ಫಲಿತಾಂಶ;
  ಹಿಂತಿರುಗಿ mresult;
}

ಸ್ಥಿರ CURLMcode multistate_connect(ರಚನೆ Curl_multi *multi,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    CURLcode *resultp)
{
  /* Connect. We want to get a connection identifier filled in. This state can
     be entered from SETUP and from PENDING. */
  bool connected;
  CURLMcode mresult = CURLM_OK;
  CURLcode ಫಲಿತಾಂಶ = Curl_connect(ದತ್ತಾಂಶ, &connected);
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_NO_CONNECTION_ಲಭ್ಯ) {
    /* There was no connection available. We will go to the pending state and
       wait for an available connection. */
    multistate(ದತ್ತಾಂಶ, MSTATE_PENDING);
    /* move from process to pending set */
    Curl_uint32_bset_remove(&multi->process, ದತ್ತಾಂಶ->mid);
    Curl_uint32_bset_remove(&multi->dirty, ದತ್ತಾಂಶ->mid);
    Curl_uint32_bset_ಕೂಡಿಸು(&multi->pending, ದತ್ತಾಂಶ->mid);
    *resultp = CURLE_OK;
    ಹಿಂತಿರುಗಿ mresult;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    multi_ವೇಳಾಪಟ್ಟಿ_pending(ದತ್ತಾಂಶ->multi);

  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    /* after the connect has been sent off, go WAITCONNECT unless the
       protocol connect is already done and we can go directly to WAITDO or
       DO! */
    mresult = CURLM_CALL_MULTI_PERFORM;

    ಆದರೆ(connected) {
      ಆದರೆ(!ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.reuse &&
         Curl_conn_is_multiplex(ದತ್ತಾಂಶ->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_ವೇಳಾಪಟ್ಟಿ_pending(ದತ್ತಾಂಶ->multi);
      }
      multistate(ದತ್ತಾಂಶ, MSTATE_PROTOCONNECT);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      multistate(ದತ್ತಾಂಶ, MSTATE_CONNECTING);
    }
  }
  *resultp = ಫಲಿತಾಂಶ;
  ಹಿಂತಿರುಗಿ mresult;
}

/* returns the possibly updated result */
ಸ್ಥಿರ CURLcode is_ಪೂರ್ಣಗೊಂಡ(ರಚನೆ Curl_multi *multi,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow,
                            bool ಹರಿವು_error,
                            CURLcode ಫಲಿತಾಂಶ)
{
  ಆದರೆ(ದತ್ತಾಂಶ->mstate < MSTATE_COMPLETED) {
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      /*
       * If an error was returned, and we are not in completed state now,
       * then we go to completed and consider this transfer aborted.
       */

      /* No attempt to disconnect connections must be made before this -
         connection detach and termination happens only here */

      /* Check if we can move pending requests to send pipe */
      multi_ವೇಳಾಪಟ್ಟಿ_pending(multi); /* connection */

      ಆದರೆ(ದತ್ತಾಂಶ->conn) {
        ಆದರೆ(ಹರಿವು_error) {
          /* Do not attempt to send data over a connection that timed out */
          bool dead_connection = ಫಲಿತಾಂಶ == CURLE_OPERATION_TIMEDOUT;
          ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

          /* This is where we make sure that the conn pointer is reset.
             We do not have to do this in every case block above where a
             failure is detected */
          Curl_detach_connection(ದತ್ತಾಂಶ);
          Curl_conn_close(ದತ್ತಾಂಶ, conn, dead_connection);
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->mstate == MSTATE_CONNECT) {
        /* Curl_connect() failed */
        multi_posttransfer(ದತ್ತಾಂಶ);
        Curl_pgrsUpdate_nometer(ದತ್ತಾಂಶ);
      }

      multistate(ದತ್ತಾಂಶ, MSTATE_COMPLETED);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    /* if there is still a connection to use, call the progress function */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->conn && Curl_conn_is_connected(ದತ್ತಾಂಶ->conn, FIRSTSOCKET)) {
      ಫಲಿತಾಂಶ = Curl_pgrsUpdateX(ದತ್ತಾಂಶ, pnow);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        /* aborted due to progress callback return code must close the
           connection */
        streamclose(ದತ್ತಾಂಶ->conn);

        /* if not yet in DONE state, go there. */
        ಆದರೆ(ದತ್ತಾಂಶ->mstate < MSTATE_ಮುಗಿದ)
          multistate(ದತ್ತಾಂಶ, MSTATE_ಮುಗಿದ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ handle_completed(ರಚನೆ Curl_multi *multi,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             CURLcode ಫಲಿತಾಂಶ)
{
  bool ಸಂದೇಶ_ಗಮ್ಯ_app = ದತ್ತಾಂಶ->master_mid == UINT32_MAX;

  ಆದರೆ(!ಸಂದೇಶ_ಗಮ್ಯ_app) {
    /* A sub transfer, not reported to the application. Is anyone still
     * interested in processing its results? */
    ಆದರೆ(ದತ್ತಾಂಶ->sub_xfer_ಮುಗಿದ) {
      ರಚನೆ Curl_easy *master = Curl_multi_get_easy(multi, ದತ್ತಾಂಶ->master_mid);

      CURL_TRC_M(ದತ್ತಾಂಶ, "sub xfer done for master %u", ದತ್ತಾಂಶ->master_mid);
      ಆದರೆ(master)
        ದತ್ತಾಂಶ->sub_xfer_ಮುಗಿದ(ದತ್ತಾಂಶ, master, ಫಲಿತಾಂಶ);
      ಇಲ್ಲದಿದ್ದರೆ
        CURL_TRC_M(ದತ್ತಾಂಶ, "master easy %u already gone.", ದತ್ತಾಂಶ->master_mid);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* now fill in the CURLMsg with this info */
    ರಚನೆ CURLMsg *ಸಂದೇಶ = &ದತ್ತಾಂಶ->ಸಂದೇಶ;

    ಸಂದೇಶ->ಸಂದೇಶ = CURLMSG_ಮುಗಿದ;
    ಸಂದೇಶ->easy_handle = ದತ್ತಾಂಶ;
    ಸಂದೇಶ->ದತ್ತಾಂಶ.ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;

    DEBUGASSERT(!ದತ್ತಾಂಶ->conn);
  }
  multistate(ದತ್ತಾಂಶ, MSTATE_MSGSENT);

  /* remove from the other sets */
  Curl_uint32_bset_remove(&multi->process, ದತ್ತಾಂಶ->mid);
  Curl_uint32_bset_remove(&multi->dirty, ದತ್ತಾಂಶ->mid);
  Curl_uint32_bset_remove(&multi->pending, ದತ್ತಾಂಶ->mid);
  ಆದರೆ(ಸಂದೇಶ_ಗಮ್ಯ_app)
    multi_addmsg(multi, ದತ್ತಾಂಶ);
  ಆದರೆ(ದತ್ತಾಂಶ->state.really_alive) {
    ದತ್ತಾಂಶ->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    ಆದರೆ(!multi->xfers_really_alive)
      (ಶೂನ್ಯ)multi_assess_wakeup(multi);
  }
  --multi->xfers_alive;
  ಆದರೆ(!multi->xfers_alive)
    multi_assess_wakeup(multi);
}

ಸ್ಥಿರ CURLMcode multistate_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLcode *ಫಲಿತಾಂಶ)
{
  ಆದರೆ(!ದತ್ತಾಂಶ->state.really_alive) {
    ದತ್ತಾಂಶ->state.really_alive = TRUE;
    ++ದತ್ತಾಂಶ->multi->xfers_really_alive;
    ಆದರೆ(ದತ್ತಾಂಶ->multi->xfers_really_alive == 1) {
      CURLMcode mresult = multi_assess_wakeup(ದತ್ತಾಂಶ->multi);
      ಆದರೆ(mresult) {
        failf(ದತ್ತಾಂಶ, "error enabling wakeup listening: %d", mresult);
        ಹಿಂತಿರುಗಿ mresult;
      }
    }
  }

  *ಫಲಿತಾಂಶ = Curl_pretransfer(ದತ್ತಾಂಶ);
  ಆದರೆ(*ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ CURLM_OK;

  /* after init, go SETUP */
  multistate(ದತ್ತಾಂಶ, MSTATE_ಸಿದ್ಧತೆ);
  Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_STARTOP);
  ಹಿಂತಿರುಗಿ CURLM_CALL_MULTI_PERFORM;
}

ಸ್ಥಿರ CURLMcode multistate_ಸಿದ್ಧತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
  Curl_pgrs_ಸಮಯ_Was(ದತ್ತಾಂಶ, TIMER_STARTSINGLE, *pnow);
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.timeout)
    Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_TIMEOUT, ದತ್ತಾಂಶ->ಗಣ.timeout, pnow);
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.connecttimeout)
    /* Since a connection might go to pending and back to CONNECT several
       times before it actually takes off, we need to set the timeout once
       in SETUP before we enter CONNECT the first time. */
    Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_CONNECTTIMEOUT,
                    ದತ್ತಾಂಶ->ಗಣ.connecttimeout, pnow);

  multistate(ದತ್ತಾಂಶ, MSTATE_CONNECT);
  ಹಿಂತಿರುಗಿ CURLM_CALL_MULTI_PERFORM;
}

ಸ್ಥಿರ CURLMcode multistate_connecting(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       bool *ಹರಿವು_error,
                                       CURLcode *ಫಲಿತಾಂಶ)
{
  bool connected;

  ಆದರೆ(!ದತ್ತಾಂಶ->conn) {
    DEBUGASSERT(0);
    *ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
    ಹಿಂತಿರುಗಿ CURLM_OK;
  }
  ಆದರೆ(!Curl_xfer_recv_is_paused(ದತ್ತಾಂಶ)) {
    *ಫಲಿತಾಂಶ = Curl_conn_connect(ದತ್ತಾಂಶ, FIRSTSOCKET, FALSE, &connected);
    ಆದರೆ(connected && !*ಫಲಿತಾಂಶ) {
      ಆದರೆ(!ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.reuse &&
         Curl_conn_is_multiplex(ದತ್ತಾಂಶ->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_ವೇಳಾಪಟ್ಟಿ_pending(ದತ್ತಾಂಶ->multi);
      }
      multistate(ದತ್ತಾಂಶ, MSTATE_PROTOCONNECT);
      ಹಿಂತಿರುಗಿ CURLM_CALL_MULTI_PERFORM;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*ಫಲಿತಾಂಶ) {
      /* failure detected */
      CURL_TRC_M(ದತ್ತಾಂಶ, "connect failed -> %d", (ಪೂರ್ಣಾಂಕ)*ಫಲಿತಾಂಶ);
      multi_posttransfer(ದತ್ತಾಂಶ);
      multi_ಮುಗಿದ(ದತ್ತಾಂಶ, *ಫಲಿತಾಂಶ, TRUE);
      *ಹರಿವು_error = TRUE;
      ಹಿಂತಿರುಗಿ CURLM_OK;
    }
  }
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode multistate_protoconnect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         bool *ಹರಿವು_error,
                                         CURLcode *ಫಲಿತಾಂಶ)
{
  bool protocol_connected = FALSE;

  ಆದರೆ(!*ಫಲಿತಾಂಶ && ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.reuse) {
    /* ftp seems to hang when protoconnect on reused connection since we
     * handle PROTOCONNECT in general inside the filters, it seems wrong to
     * restart this on a reused connection.
     */
    multistate(ದತ್ತಾಂಶ, MSTATE_DO);
    ಹಿಂತಿರುಗಿ CURLM_CALL_MULTI_PERFORM;
  }
  ಆದರೆ(!*ಫಲಿತಾಂಶ)
    *ಫಲಿತಾಂಶ = protocol_connect(ದತ್ತಾಂಶ, &protocol_connected);
  ಆದರೆ(!*ಫಲಿತಾಂಶ && !protocol_connected) {
    /* switch to waiting state */
    multistate(ದತ್ತಾಂಶ, MSTATE_PROTOCONNECTING);
    ಹಿಂತಿರುಗಿ CURLM_CALL_MULTI_PERFORM;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!*ಫಲಿತಾಂಶ) {
    /* protocol connect has completed, go WAITDO or DO */
    multistate(ದತ್ತಾಂಶ, MSTATE_DO);
    ಹಿಂತಿರುಗಿ CURLM_CALL_MULTI_PERFORM;
  }

  /* failure detected */
  multi_posttransfer(ದತ್ತಾಂಶ);
  multi_ಮುಗಿದ(ದತ್ತಾಂಶ, *ಫಲಿತಾಂಶ, TRUE);
  *ಹರಿವು_error = TRUE;
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode multistate_protoconnecting(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            bool *ಹರಿವು_error,
                                            CURLcode *ಫಲಿತಾಂಶ)
{
  bool protocol_connected = FALSE;

  /* protocol-specific connect phase */
  *ಫಲಿತಾಂಶ = protocol_connecting(ದತ್ತಾಂಶ, &protocol_connected);
  ಆದರೆ(!*ಫಲಿತಾಂಶ && protocol_connected) {
    /* after the connect has completed, go WAITDO or DO */
    multistate(ದತ್ತಾಂಶ, MSTATE_DO);
    ಹಿಂತಿರುಗಿ CURLM_CALL_MULTI_PERFORM;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*ಫಲಿತಾಂಶ) {
    /* failure detected */
    multi_posttransfer(ದತ್ತಾಂಶ);
    multi_ಮುಗಿದ(ದತ್ತಾಂಶ, *ಫಲಿತಾಂಶ, TRUE);
    *ಹರಿವು_error = TRUE;
  }
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode multistate_doing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  bool *ಹರಿವು_error,
                                  CURLcode *ಫಲಿತಾಂಶ)
{
  bool dophase_ಮುಗಿದ = FALSE;

  /* we continue DOING until the DO phase is complete */
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  *ಫಲಿತಾಂಶ = protocol_doing(ದತ್ತಾಂಶ, &dophase_ಮುಗಿದ);
  ಆದರೆ(!*ಫಲಿತಾಂಶ) {
    ಆದರೆ(dophase_ಮುಗಿದ) {
      /* after DO, go DO_DONE or DO_MORE */
      multistate(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.do_ಇನ್ನಷ್ಟು ?
                 MSTATE_DOING_ಇನ್ನಷ್ಟು : MSTATE_DID);
      ಹಿಂತಿರುಗಿ CURLM_CALL_MULTI_PERFORM;
    } /* dophase_done */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* failure detected */
    multi_posttransfer(ದತ್ತಾಂಶ);
    multi_ಮುಗಿದ(ದತ್ತಾಂಶ, *ಫಲಿತಾಂಶ, FALSE);
    *ಹರಿವು_error = TRUE;
  }
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode multistate_doing_ಇನ್ನಷ್ಟು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       bool *ಹರಿವು_error,
                                       CURLcode *ಫಲಿತಾಂಶ)
{
  domore control;

  /*
   * When we are connected, DOING MORE and then go DID
   */
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  *ಫಲಿತಾಂಶ = multi_do_ಇನ್ನಷ್ಟು(ದತ್ತಾಂಶ, &control);

  ಆದರೆ(!*ಫಲಿತಾಂಶ) {
    ಆದರೆ(control != DOMORE_INCOMPLETE) {
      /* if DONE, advance to DO_DONE
         if GOBACK, go back to DOING */
      multistate(ದತ್ತಾಂಶ, control == DOMORE_ಮುಗಿದ ? MSTATE_DID : MSTATE_DOING);
      ಹಿಂತಿರುಗಿ CURLM_CALL_MULTI_PERFORM;
    }
    /* else
       stay in DO_MORE */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* failure detected */
    multi_posttransfer(ದತ್ತಾಂಶ);
    multi_ಮುಗಿದ(ದತ್ತಾಂಶ, *ಫಲಿತಾಂಶ, FALSE);
    *ಹರಿವು_error = TRUE;
  }
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

ಸ್ಥಿರ CURLMcode multistate_did(ರಚನೆ Curl_multi *multi,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  ಆದರೆ(ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.multiplex)
    /* Check if we can move pending requests to send pipe */
    multi_ವೇಳಾಪಟ್ಟಿ_pending(multi); /* multiplexed */

  /* Only perform the transfer if there is a good socket to work with.
     Having both BAD is a signal to skip immediately to DONE */
  ಆದರೆ(CONN_SOCK_IDX_VALID(ದತ್ತಾಂಶ->conn->recv_idx) ||
     CONN_SOCK_IDX_VALID(ದತ್ತಾಂಶ->conn->send_idx)) {
    multistate(ದತ್ತಾಂಶ, MSTATE_PERFORMING);
    /* Do not return CURLM_CALL_MULTI_PERFORM to give other transfers
     * a chance to send off their requests.
     * Note: Some SFTP handlers do not seem to like this.
     *       Restrict it to HTTP families. */
    ಹಿಂತಿರುಗಿ ((multi->xfers_alive > 1) &&
            (ದತ್ತಾಂಶ->conn->scheme->protocol & PROTO_FAMILY_HTTP)) ?
           CURLM_OK : CURLM_CALL_MULTI_PERFORM;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
    ಆದರೆ(ದತ್ತಾಂಶ->state.wildcardmatch &&
       ((ದತ್ತಾಂಶ->conn->scheme->flags & PROTOPT_WILDCARD) == 0)) {
      ದತ್ತಾಂಶ->wildcard->state = CURLWC_ಮುಗಿದ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    multistate(ದತ್ತಾಂಶ, MSTATE_ಮುಗಿದ);
    ಹಿಂತಿರುಗಿ CURLM_CALL_MULTI_PERFORM;
  }
}

ಸ್ಥಿರ CURLMcode multistate_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLcode *presult)
{
  ಆದರೆ(ದತ್ತಾಂಶ->conn) {
    CURLcode ಫಲಿತಾಂಶ;

    /* post-transfer command */
    ಫಲಿತಾಂಶ = multi_ಮುಗಿದ(ದತ್ತಾಂಶ, *presult, FALSE);

    /* allow a previously set error code take precedence */
    ಆದರೆ(!(*presult))
      *presult = ಫಲಿತಾಂಶ;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  ಆದರೆ(ದತ್ತಾಂಶ->state.wildcardmatch) {
    ಆದರೆ(ದತ್ತಾಂಶ->wildcard->state != CURLWC_ಮುಗಿದ) {
      /* if a wildcard is set and we are not ending -> lets start again
         with MSTATE_INIT */
      multistate(ದತ್ತಾಂಶ, MSTATE_INIT);
      ಹಿಂತಿರುಗಿ CURLM_CALL_MULTI_PERFORM;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  /* after we have DONE what we are supposed to do, go COMPLETED, and
     it does not matter what the multi_done() returned! */
  multistate(ದತ್ತಾಂಶ, MSTATE_COMPLETED);
  ಹಿಂತಿರುಗಿ CURLM_CALL_MULTI_PERFORM;
}

ಸ್ಥಿರ CURLMcode multi_runsingle(ರಚನೆ Curl_multi *multi,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ Curl_sigpipe_ctx *sigpipe_ctx)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow = ಖಾಲಿ;

  ಆದರೆ(multi->dead) {
    /* a multi-level callback returned error before, meaning every individual
     transfer now has failed */
    ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
    multi_posttransfer(ದತ್ತಾಂಶ);
    multi_ಮುಗಿದ(ದತ್ತಾಂಶ, ಫಲಿತಾಂಶ, FALSE);
    multistate(ದತ್ತಾಂಶ, MSTATE_COMPLETED);
  }

  multi_warn_debug(multi, ದತ್ತಾಂಶ);

  /* transfer runs now, clear the dirty bit. This may be set
   * again during processing, triggering a re-run later. */
  Curl_uint32_bset_remove(&multi->dirty, ದತ್ತಾಂಶ->mid);

  ಆದರೆ(ದತ್ತಾಂಶ == multi->admin) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_ಆಂತರಿಕ_WAKEUP
    /* Consume any pending wakeup signals before processing.
     * This is necessary for event based processing. See #21547 */
    (ಶೂನ್ಯ)Curl_wakeup_consume(multi->wakeup_ಆಂತರಿಕ, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_THREADED
    Curl_async_thrdd_multi_process(multi);
#ಆದರೆ_ಅಂತ್ಯ
    Curl_cshutdn_perform(&multi->cshutdn, multi->admin, sigpipe_ctx);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  sigpipe_apply(ದತ್ತಾಂಶ, sigpipe_ctx);
  ಮಾಡು {
    /* A "stream" here is a logical stream if the protocol can handle that
       (HTTP/2), or the full connection for older protocols */
    bool ಹರಿವು_error = FALSE;
    mresult = CURLM_OK;
    pnow = ಖಾಲಿ;

    ಆದರೆ(multi_ischanged(multi, TRUE)) {
      CURL_TRC_M(ದತ್ತಾಂಶ, "multi changed, check CONNECT_PEND queue");
      multi_ವೇಳಾಪಟ್ಟಿ_pending(multi); /* multiplexed */
    }

    ಆದರೆ(ದತ್ತಾಂಶ->mstate > MSTATE_CONNECT &&
       ದತ್ತಾಂಶ->mstate < MSTATE_COMPLETED) {
      /* Make sure we set the connection's current owner */
      DEBUGASSERT(ದತ್ತಾಂಶ->conn);
      ಆದರೆ(!ದತ್ತಾಂಶ->conn) {
        mresult = CURLM_ಆಂತರಿಕ_ERROR;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }

    /* Wait for the connect state as only then is the start time stored, but
       we must not check already completed handles */
    ಆದರೆ((ದತ್ತಾಂಶ->mstate >= MSTATE_CONNECT) && (ದತ್ತಾಂಶ->mstate < MSTATE_COMPLETED)) {
      pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
      ಆದರೆ(multi_handle_timeout(ದತ್ತಾಂಶ, pnow, &ಹರಿವು_error, &ಫಲಿತಾಂಶ))
        /* Skip the statemachine and go directly to error handling section. */
        ಹೋಗು statemachine_ಕೊನೆ;
      pnow = ಖಾಲಿ;
    }

    ಆಯ್ಕೆ(ದತ್ತಾಂಶ->mstate) {
    ಸ್ಥಿತಿ MSTATE_INIT:
      /* Transitional state. init this transfer. A handle never comes back to
         this state. */
      mresult = multistate_init(ದತ್ತಾಂಶ, &ಫಲಿತಾಂಶ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_ಸಿದ್ಧತೆ:
      /* Transitional state. Setup things for a new transfer. The handle
         can come back to this state on a redirect. */
      mresult = multistate_ಸಿದ್ಧತೆ(ದತ್ತಾಂಶ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_CONNECT:
      mresult = multistate_connect(multi, ದತ್ತಾಂಶ, &ಫಲಿತಾಂಶ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_CONNECTING:
      /* awaiting a completion of an asynch TCP connect */
      mresult = multistate_connecting(ದತ್ತಾಂಶ, &ಹರಿವು_error, &ಫಲಿತಾಂಶ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_PROTOCONNECT:
      mresult = multistate_protoconnect(ದತ್ತಾಂಶ, &ಹರಿವು_error, &ಫಲಿತಾಂಶ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_PROTOCONNECTING:
      /* protocol-specific connect phase */
      mresult = multistate_protoconnecting(ದತ್ತಾಂಶ, &ಹರಿವು_error, &ಫಲಿತಾಂಶ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_DO:
      mresult = multistate_do(ದತ್ತಾಂಶ, &ಹರಿವು_error, &ಫಲಿತಾಂಶ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_DOING:
      /* we continue DOING until the DO phase is complete */
      mresult = multistate_doing(ದತ್ತಾಂಶ, &ಹರಿವು_error, &ಫಲಿತಾಂಶ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_DOING_ಇನ್ನಷ್ಟು:
      /*
       * When we are connected, DOING MORE and then go DID
       */
      mresult = multistate_doing_ಇನ್ನಷ್ಟು(ದತ್ತಾಂಶ, &ಹರಿವು_error, &ಫಲಿತಾಂಶ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_DID:
      mresult = multistate_did(multi, ದತ್ತಾಂಶ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_RATELIMITING: /* limit-rate exceeded in either direction */
      mresult = multistate_ratelimiting(ದತ್ತಾಂಶ, &ಫಲಿತಾಂಶ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_PERFORMING:
      mresult = multistate_performing(ದತ್ತಾಂಶ, &ಹರಿವು_error, &ಫಲಿತಾಂಶ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_ಮುಗಿದ:
      mresult = multistate_ಮುಗಿದ(ದತ್ತಾಂಶ, &ಫಲಿತಾಂಶ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_COMPLETED:
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ MSTATE_PENDING:
    ಸ್ಥಿತಿ MSTATE_MSGSENT:
      /* handles in these states should NOT be in this list */
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      mresult = CURLM_ಆಂತರಿಕ_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆದರೆ(ದತ್ತಾಂಶ->mstate >= MSTATE_CONNECT &&
       ದತ್ತಾಂಶ->mstate < MSTATE_DO &&
       mresult != CURLM_CALL_MULTI_PERFORM &&
       !multi_ischanged(multi, FALSE)) {
      /* We now handle stream timeouts if and only if this will be the last
       * loop iteration. We only check this on the last iteration to ensure
       * that if we know we have additional work to do immediately
       * (i.e. CURLM_CALL_MULTI_PERFORM == TRUE) then we should do that before
       * declaring the connection timed out as we may almost have a completed
       * connection. */
      pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
      multi_handle_timeout(ದತ್ತಾಂಶ, pnow, &ಹರಿವು_error, &ಫಲಿತಾಂಶ);
    }

statemachine_ಕೊನೆ:
    ಆದರೆ(!pnow)
      pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
    ಫಲಿತಾಂಶ = is_ಪೂರ್ಣಗೊಂಡ(multi, ದತ್ತಾಂಶ, pnow, ಹರಿವು_error, ಫಲಿತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      mresult = CURLM_CALL_MULTI_PERFORM;

    ಆದರೆ(MSTATE_COMPLETED == ದತ್ತಾಂಶ->mstate) {
      handle_completed(multi, ದತ್ತಾಂಶ, ಫಲಿತಾಂಶ);
      mresult = CURLM_OK;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  } ಆಗ((mresult == CURLM_CALL_MULTI_PERFORM) ||
          multi_ischanged(multi, FALSE));

ಹೊರಹೋಗು:
  ದತ್ತಾಂಶ->ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
  ಹಿಂತಿರುಗಿ mresult;
}

ಸ್ಥಿರ CURLMcode multi_perform(ರಚನೆ Curl_multi *multi,
                               ಪೂರ್ಣಾಂಕ *running_handles)
{
  CURLMcode returncode = CURLM_OK;
  ರಚನೆ curltime ಆರಂಭ;
  uint32_t mid;
  ರಚನೆ Curl_sigpipe_ctx sigpipe_ctx;

  curlx_pnow(&ಆರಂಭ);
  sigpipe_init(&sigpipe_ctx);

  ಆದರೆ(Curl_uint32_bset_ಮೊದಲ(&multi->process, &mid)) {
    CURL_TRC_M(multi->admin, "multi_perform(running=%u)",
               multi_xfers_running(multi));
    ಮಾಡು {
      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);
      CURLMcode mresult;
      ಆದರೆ(!ದತ್ತಾಂಶ) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        ಮುಂದುವರಿ;
      }
      mresult = multi_runsingle(multi, ದತ್ತಾಂಶ, &sigpipe_ctx);
      ಆದರೆ(mresult)
        returncode = mresult;
    } ಆಗ(Curl_uint32_bset_ಮುಂದಿನ(&multi->process, mid, &mid));
  }
  sigpipe_restore(&sigpipe_ctx);

  ಆದರೆ(multi_ischanged(multi, TRUE))
    multi_ವೇಳಾಪಟ್ಟಿ_pending(multi);

  ಆದರೆ(!returncode && CURL_MNTFY_HAS_ENTRIES(multi))
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
  ಆಗ(Curl_timeouts_remove_expired(&multi->timeouts, &ಆರಂಭ, &mid)) {
    /* the removed may have another timeout in queue */
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);
    ಆದರೆ(!ದತ್ತಾಂಶ) {
      DEBUGASSERT(0);
      ಮುಂದುವರಿ;
    }
    (ಶೂನ್ಯ)ಕೂಡಿಸು_ಮುಂದಿನ_timeout(&ಆರಂಭ, multi, ದತ್ತಾಂಶ);
    ಆದರೆ(ದತ್ತಾಂಶ->mstate == MSTATE_PENDING) {
      bool ಹರಿವು_ಬಳಸದ;
      CURLcode ಫಲಿತಾಂಶ_ಬಳಸದ;
      ಆದರೆ(multi_handle_timeout(ದತ್ತಾಂಶ, Curl_pgrs_now(ದತ್ತಾಂಶ),
                              &ಹರಿವು_ಬಳಸದ, &ಫಲಿತಾಂಶ_ಬಳಸದ)) {
        infof(ದತ್ತಾಂಶ, "PENDING handle timeout");
        move_pending_ಗಮ್ಯ_connect(multi, ದತ್ತಾಂಶ);
      }
    }
  }

  ಆದರೆ(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? (ಪೂರ್ಣಾಂಕ)running : ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
  }

  ಆದರೆ(CURLM_OK >= returncode)
    returncode = Curl_update_timer(multi);

  ಹಿಂತಿರುಗಿ returncode;
}

CURLMcode curl_multi_perform(CURLM *m, ಪೂರ್ಣಾಂಕ *running_handles)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_perform, &mresult)) {
    mresult = multi_perform(m, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

CURLMcode curl_multi_cleanup(CURLM *m)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_cleanup, &mresult)) {
    ರಚನೆ Curl_multi *multi = m;
    ಶೂನ್ಯ *ನಮೂದು;
    uint32_t mid;

    /* First remove all remaining easy handles,
     * close internal ones. admin handle is special */
    ಆದರೆ(Curl_uint32_tbl_ಮೊದಲ(&multi->xfers, &mid, &ನಮೂದು)) {
      ಮಾಡು {
        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = ನಮೂದು;
        ಆದರೆ(!GOOD_EASY_HANDLE(ದತ್ತಾಂಶ)) {
          mresult = CURLM_BAD_HANDLE;
          ಹೋಗು ಹೊರಹೋಗು;
        }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
        ಆದರೆ(mid != ದತ್ತಾಂಶ->mid) {
          CURL_TRC_M(ದತ್ತಾಂಶ, "multi_cleanup: still present with mid=%u, "
                     "but unexpected data->mid=%u\n", mid, ದತ್ತಾಂಶ->mid);
          DEBUGASSERT(0);
        }
#ಆದರೆ_ಅಂತ್ಯ

        ಆದರೆ(ದತ್ತಾಂಶ == multi->admin)
          ಮುಂದುವರಿ;

        ಆದರೆ(!ದತ್ತಾಂಶ->state.ಮುಗಿದ && ದತ್ತಾಂಶ->conn)
          /* if DONE was never called for this handle */
          (ಶೂನ್ಯ)multi_ಮುಗಿದ(ದತ್ತಾಂಶ, CURLE_OK, TRUE);

        ದತ್ತಾಂಶ->multi = ಖಾಲಿ; /* clear the association */
        Curl_uint32_tbl_remove(&multi->xfers, mid);
        ದತ್ತಾಂಶ->mid = UINT32_MAX;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
        ಆದರೆ(ದತ್ತಾಂಶ->psl == &multi->psl)
          ದತ್ತಾಂಶ->psl = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ(ದತ್ತಾಂಶ->state.ಆಂತರಿಕ)
          Curl_close(&ದತ್ತಾಂಶ);
      } ಆಗ(Curl_uint32_tbl_ಮುಂದಿನ(&multi->xfers, mid, &mid, &ನಮೂದು));
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_THREADED
    Curl_async_thrdd_multi_destroy(multi, !multi->quick_exit);
#ಆದರೆ_ಅಂತ್ಯ
    Curl_cpool_destroy(&multi->cpool, multi->admin);
    Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
    ಆದರೆ(multi->admin) {
      CURL_TRC_M(multi->admin, "multi_cleanup, closing admin handle, done");
      multi->admin->multi = ಖಾಲಿ;
      Curl_uint32_tbl_remove(&multi->xfers, multi->admin->mid);
      Curl_close(&multi->admin);
    }

    multi->magic = 0; /* not good anymore */

    Curl_multi_ev_cleanup(multi);
    Curl_hash_destroy(&multi->proto_hash);
    Curl_dnscache_destroy(&multi->dnscache);
    Curl_psl_destroy(&multi->psl);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
    Curl_ssl_scache_destroy(multi->ssl_scache);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
    WSACloseEvent(multi->wsa_event);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_ಆಂತರಿಕ_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_ಆಂತರಿಕ);
#ಆದರೆ_ಅಂತ್ಯ

    multi_xfer_bufs_free(multi);
    Curl_mntfy_cleanup(multi);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    ಆದರೆ(Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers)) {
      multi_xfer_tbl_dump(multi);
      DEBUGASSERT(0);
    }
#ಆದರೆ_ಅಂತ್ಯ
    Curl_uint32_bset_destroy(&multi->process);
    Curl_uint32_bset_destroy(&multi->dirty);
    Curl_uint32_bset_destroy(&multi->pending);
    Curl_uint32_bset_destroy(&multi->msgsent);
    Curl_uint32_tbl_destroy(&multi->xfers);
    curlx_memzero(multi, ಗಾತ್ರ(*multi));
    curlx_free(multi);

    mresult = CURLM_OK;
  }
ಹೊರಹೋಗು:
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

/*
 * curl_multi_info_read()
 *
 * This function is the primary way for a multi/multi_socket application to
 * figure out if a transfer has ended.
 */

CURLMsg *curl_multi_info_read(CURLM *m, ಪೂರ್ಣಾಂಕ *msgs_ಒಳ_ಸರತಿ)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMsg *ಸಂದೇಶ_ಫಲಿತಾಂಶ = ಖಾಲಿ;

  *msgs_ಒಳ_ಸರತಿ = 0; /* default to none */
  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_info_read, ಖಾಲಿ)) {
    ರಚನೆ Curl_multi *multi = m;
    uint32_t mid;
    ಆದರೆ(Curl_uint32_bset_ಮೊದಲ(&multi->msgsent, &mid)) {
      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);

      DEBUGASSERT(ದತ್ತಾಂಶ);
      Curl_uint32_bset_remove(&multi->msgsent, mid);
      *msgs_ಒಳ_ಸರತಿ =
        curlx_uztosi(Curl_uint32_bset_ಎಣಿಕೆ(&multi->msgsent));
      ಆದರೆ(ದತ್ತಾಂಶ)
        ಸಂದೇಶ_ಫಲಿತಾಂಶ = &ದತ್ತಾಂಶ->ಸಂದೇಶ;
    }
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಸಂದೇಶ_ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_multi_will_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_socket_t s)
{
  ಆದರೆ(ದತ್ತಾಂಶ) {
    ರಚನೆ Curl_multi *multi = ದತ್ತಾಂಶ->multi;
    ಆದರೆ(multi) {
      CURL_TRC_M(ದತ್ತಾಂಶ, "Curl_multi_will_close fd=%" FMT_SOCKET_T, s);
      Curl_multi_ev_socket_ಮುಗಿದ(multi, ದತ್ತಾಂಶ, s);
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ multi_timeouts_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ದತ್ತಾಂಶ->state.timeouts.ಮೊದಲ = EXPIRE_ಕೊನೆಯ;
  ದತ್ತಾಂಶ->state.timeouts.splaynode.registered = FALSE;
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
ಸ್ಥಿರ CURLMcode ಕೂಡಿಸು_ಮುಂದಿನ_timeout(ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow,
                                  ರಚನೆ Curl_multi *multi,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ expire_timers *timeouts = &ದತ್ತಾಂಶ->state.timeouts;
  timediff_t now_us = Curl_timeouts_ಸ್ಥಾನಾಂತರ_us(&multi->timeouts, pnow);

  ಆಗ(timeouts->ಮೊದಲ < EXPIRE_ಕೊನೆಯ) {
    ಆದರೆ(timeouts->ಸ್ಥಾನಾಂತರ_us[timeouts->ಮೊದಲ] <= now_us)  /* already expired */
      timeouts->ಮೊದಲ = timeouts->ಮುಂದಿನ[timeouts->ಮೊದಲ];
    ಇಲ್ಲದಿದ್ದರೆ /* timeouts are sorted, first is first in the future now */
      ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(timeouts->ಮೊದಲ < EXPIRE_ಕೊನೆಯ) {
    /* Insert this node again into the splay. Keep the timer in the list in
       case we need to recompute future timers. */
    Curl_timeouts_ಕೂಡಿಸು(&multi->timeouts, ದತ್ತಾಂಶ,
                      timeouts->ಸ್ಥಾನಾಂತರ_us[timeouts->ಮೊದಲ]);
  }
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ multi_mark_expired_as_dirty(ರಚನೆ Curl_multi *multi,
                                        ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *ts)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = ಖಾಲಿ;
  uint32_t mid;

  /*
   * The loop following here will go on as long as there are expire-times left
   * to process (compared to `ts`) in the splay and 'data' will be
   * re-assigned for every expired handle we deal with.
   */
  ಆಗ(Curl_timeouts_remove_expired(&multi->timeouts, ts, &mid)) {
    /* Check if there is one (more) expired timer to deal with! This function
       extracts a matching node if there is one */
    ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);
    ಆದರೆ(!ದತ್ತಾಂಶ) {
      DEBUGASSERT(0);
      ಮುಂದುವರಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
    ಆದರೆ(CURL_TRC_TIMER_is_verbose(ದತ್ತಾಂಶ)) {
      ಆದರೆ(ದತ್ತಾಂಶ->state.timeouts.ಮೊದಲ < EXPIRE_ಕೊನೆಯ) {
        CURL_TRC_TIMER(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.timeouts.ಮೊದಲ, "has expired");
      }
    }
#ಆದರೆ_ಅಂತ್ಯ
    (ಶೂನ್ಯ)ಕೂಡಿಸು_ಮುಂದಿನ_timeout(ts, multi, ದತ್ತಾಂಶ);
    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
  }
}

ಸ್ಥಿರ CURLMcode multi_ಸತತ_dirty(ರಚನೆ Curl_multi *multi,
                                 ರಚನೆ Curl_sigpipe_ctx *sigpipe_ctx,
                                 uint32_t *pnum)
{
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;

  *pnum = 0;
  ಆದರೆ(Curl_uint32_bset_ಮೊದಲ(&multi->dirty, &mid)) {
    ಮಾಡು {
      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);
      ಆದರೆ(ದತ್ತಾಂಶ) {
        CURL_TRC_M(ದತ್ತಾಂಶ, "multi_run_dirty");

        ಆದರೆ(!Curl_uint32_bset_ಒಳಗೊಂಡಿದೆ(&multi->process, mid)) {
          ಆದರೆ(Curl_uint32_bset_ಒಳಗೊಂಡಿದೆ(&multi->pending, mid)) {
            /* Something happened on the pending transfer, act on it. */
            move_pending_ಗಮ್ಯ_connect(multi, ದತ್ತಾಂಶ);
          }
          ಇಲ್ಲದಿದ್ದರೆ {
            /* We are no longer processing this transfer */
            Curl_uint32_bset_remove(&multi->dirty, mid);
            ಮುಂದುವರಿ;
          }
        }

        (*pnum)++;
        /* runsingle() clears the dirty mid */
        mresult = multi_runsingle(multi, ದತ್ತಾಂಶ, sigpipe_ctx);

        ಆದರೆ(CURLM_OK >= mresult) {
          /* reassess event handling of data */
          mresult = Curl_multi_ev_assess_xfer(multi, ದತ್ತಾಂಶ);
          ಆದರೆ(mresult)
            ಹೋಗು ಹೊರಹೋಗು;
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        CURL_TRC_M(multi->admin, "multi_run_dirty, %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } ಆಗ(Curl_uint32_bset_ಮುಂದಿನ(&multi->dirty, mid, &mid));
  }

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ mresult;
}

ಸ್ಥಿರ CURLMcode multi_socket(ರಚನೆ Curl_multi *multi,
                              bool checkall,
                              curl_socket_t s,
                              ಪೂರ್ಣಾಂಕ ev_bitmask,
                              ಪೂರ್ಣಾಂಕ *running_handles)
{
  CURLMcode mresult = CURLM_OK;
  ರಚನೆ Curl_sigpipe_ctx pipe_ctx;
  ರಚನೆ curltime now;
  uint32_t ಸತತ_xfers;

  (ಶೂನ್ಯ)ev_bitmask;
  sigpipe_init(&pipe_ctx);

  ಆದರೆ(checkall) {
    /* *perform() deals with running_handles on its own */
    mresult = multi_perform(multi, running_handles);

    ಆದರೆ(mresult != CURLM_BAD_HANDLE) {
      /* Reassess event status of all active transfers, but keep any error
         from multi_perform() */
      CURLMcode mresult2 =
        Curl_multi_ev_assess_xfer_bset(multi, &multi->process);
      ಆದರೆ(!mresult)
        mresult = mresult2;
    }
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(s != CURL_SOCKET_TIMEOUT) {
    /* Mark all transfers of that socket as dirty */
    Curl_multi_ev_dirty_xfers(multi, s);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Asked to run due to time-out. Clear the 'last_expire_ts' variable to
       force Curl_update_timer() to trigger a callback to the app again even
       if the same timeout is still the one to run after this call. That
       handles the case when the application asks libcurl to run the timeout
       prematurely. */
    multi->ಕೊನೆಯ_expire_ಸ್ಥಾನಾಂತರ_us = 0;

    /* Applications may set `socket_cb` *after* having added transfers
     * first. *Then* kick off processing with a
     * curl_multi_socket_action(TIMEOUT) afterwards. Make sure our
     * admin handle registers its pollset with the callbacks present. */
    mresult = multi_assess_wakeup(multi);
    ಆದರೆ(mresult)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  curlx_pnow(&now);
  multi_mark_expired_as_dirty(multi, &now);
  mresult = multi_ಸತತ_dirty(multi, &pipe_ctx, &ಸತತ_xfers);
  ಆದರೆ(mresult)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(ಸತತ_xfers) {
    /* Running transfers takes time. With a new timestamp, we might catch
     * other expires which are due now. Instead of telling the application
     * to set a 0 timeout and call us again, we run them here.
     * Do that only once or it might be unfair to transfers on other
     * sockets. */
    curlx_pnow(&now);
    multi_mark_expired_as_dirty(multi, &now);
    mresult = multi_ಸತತ_dirty(multi, &pipe_ctx, &ಸತತ_xfers);
  }

ಹೊರಹೋಗು:
  sigpipe_restore(&pipe_ctx);

  ಆದರೆ(multi_ischanged(multi, TRUE))
    multi_ವೇಳಾಪಟ್ಟಿ_pending(multi);

  ಆದರೆ(!mresult && CURL_MNTFY_HAS_ENTRIES(multi))
    mresult = Curl_mntfy_dispatch_all(multi);

  ಆದರೆ(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? (ಪೂರ್ಣಾಂಕ)running : ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
  }

  ಆದರೆ(CURLM_OK >= mresult)
    mresult = Curl_update_timer(multi);
  ಹಿಂತಿರುಗಿ mresult;
}

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು curl_multi_setopt
CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_setopt, &mresult)) {
    ರಚನೆ Curl_multi *multi = m;
    va_list param;
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ uarg;
    size_t szarg;

    va_start(param, option);

    ಆಯ್ಕೆ(option) {
    ಸ್ಥಿತಿ CURLMOPT_SOCKETFUNCTION:
      multi->socket_cb = va_arg(param, curl_socket_callback);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_SOCKETDATA:
      multi->socket_userp = va_arg(param, ಶೂನ್ಯ *);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_PUSHFUNCTION:
      multi->ತಳ್ಳು_cb = va_arg(param, curl_ತಳ್ಳು_callback);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_PUSHDATA:
      multi->ತಳ್ಳು_userp = va_arg(param, ಶೂನ್ಯ *);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_PIPELINING:
      multi->multiplexing = va_arg(param, ದೀರ್ಘ) & CURLPIPE_MULTIPLEX ? 1 : 0;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_TIMERFUNCTION:
      multi->timer_cb = va_arg(param, curl_multi_timer_callback);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_TIMERDATA:
      multi->timer_userp = va_arg(param, ಶೂನ್ಯ *);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_MAXCONNECTS:
      uarg = va_arg(param, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ);
      ಆದರೆ(uarg <= UINT32_MAX)
        multi->maxconnects = (uint32_t)uarg;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_MAX_HOST_CONNECTIONS:
      ಆದರೆ(!curlx_sltouz(va_arg(param, ದೀರ್ಘ), &szarg))
        mresult = CURLM_BAD_FUNCTION_ವಾದ;
      multi->max_host_connections = (szarg < UINT32_MAX) ?
                                    (uint32_t)szarg : UINT32_MAX;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_MAX_ಒಟ್ಟು_CONNECTIONS:
      ಆದರೆ(!curlx_sltouz(va_arg(param, ದೀರ್ಘ), &szarg))
        mresult = CURLM_BAD_FUNCTION_ವಾದ;
      multi->max_ಒಟ್ಟು_connections = (szarg < UINT32_MAX) ?
                                     (uint32_t)szarg : UINT32_MAX;
      ನಿಲ್ಲಿಸು;
      /* options formerly used for pipelining */
    ಸ್ಥಿತಿ CURLMOPT_MAX_PIPELINE_ಉದ್ದಳತೆ:
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_CONTENT_ಉದ್ದಳತೆ_PENALTY_ಗಾತ್ರ:
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_CHUNK_ಉದ್ದಳತೆ_PENALTY_ಗಾತ್ರ:
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_PIPELINING_SITE_BL:
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_PIPELINING_SERVER_BL:
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_MAX_CONCURRENT_STREAMS: {
      ಆದರೆ(!curlx_sltouz(va_arg(param, ದೀರ್ಘ), &szarg) ||
         !szarg || (szarg > (size_t)ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)) /* preserve previous cutoff */
        multi->max_concurrent_streams = 100;
      ಇಲ್ಲದಿದ್ದರೆ
        multi->max_concurrent_streams = (szarg < UINT32_MAX) ?
                                       (uint32_t)szarg : UINT32_MAX;
      ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ CURLMOPT_ಜಾಲ_ಬದಲಾಯಿತು: {
      ದೀರ್ಘ val = va_arg(param, ದೀರ್ಘ);
      ಆದರೆ(val & CURLMNWC_CLEAR_ALL)
        /* In the beginning, all values available to set were 1 by mistake. We
           converted this to mean "all", thus setting all the bits
           automatically */
        val = CURLMNWC_CLEAR_DNS | CURLMNWC_CLEAR_CONNS;
      ಆದರೆ(val & CURLMNWC_CLEAR_DNS) {
        Curl_dnscache_clear(multi->admin);
      }
      ಆದರೆ(val & CURLMNWC_CLEAR_CONNS) {
        Curl_cpool_nw_ಬದಲಾಯಿತು(&multi->cpool, multi->admin);
      }
      ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ CURLMOPT_NOTIFYFUNCTION:
      multi->ntfy.ntfy_cb = va_arg(param, curl_notify_callback);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_NOTIFYDATA:
      multi->ntfy.ntfy_cb_ದತ್ತಾಂಶ = va_arg(param, ಶೂನ್ಯ *);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_RESOLVE_ಎಳೆಗಳು_MAX:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_THREADED
      uarg = va_arg(param, ದೀರ್ಘ);
      ಆದರೆ((uarg <= 0) || (uarg > UINT32_MAX))
        mresult = CURLM_BAD_FUNCTION_ವಾದ;
      ಇಲ್ಲದಿದ್ದರೆ {
        CURLcode ಫಲಿತಾಂಶ = Curl_async_thrdd_multi_ಗಣ_props(
          multi, 0, (uint32_t)uarg, 2000);
        ಆಯ್ಕೆ(ಫಲಿತಾಂಶ) {
        ಸ್ಥಿತಿ CURLE_OK:
          mresult = CURLM_OK;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ CURLE_BAD_FUNCTION_ವಾದ:
          mresult = CURLM_BAD_FUNCTION_ವಾದ;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ:
          mresult = CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
          ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
          mresult = CURLM_ಆಂತರಿಕ_ERROR;
          ನಿಲ್ಲಿಸು;
        }
      }
#ಆದರೆ_ಅಂತ್ಯ
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMOPT_ಶೀಘ್ರ_EXIT:
      multi->quick_exit = va_arg(param, ದೀರ್ಘ) ? 1 : 0;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      mresult = CURLM_UNKNOWN_OPTION;
      ನಿಲ್ಲಿಸು;
    }
    va_end(param);
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

/* we define curl_multi_socket() in the public multi.h header */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು curl_multi_socket

CURLMcode curl_multi_socket(CURLM *m, curl_socket_t s, ಪೂರ್ಣಾಂಕ *running_handles)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_socket, &mresult)) {
    mresult = multi_socket(m, FALSE, s, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

CURLMcode curl_multi_socket_action(CURLM *m, curl_socket_t s,
                                   ಪೂರ್ಣಾಂಕ ev_bitmask, ಪೂರ್ಣಾಂಕ *running_handles)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_socket_action, &mresult)) {
    mresult = multi_socket(m, FALSE, s, ev_bitmask, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

CURLMcode curl_multi_socket_all(CURLM *m, ಪೂರ್ಣಾಂಕ *running_handles)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_socket_all, &mresult)) {
    mresult = multi_socket(m, TRUE, CURL_SOCKET_BAD, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

ಸ್ಥಿರ bool multi_has_dirties(ರಚನೆ Curl_multi *multi)
{
  uint32_t mid;
  ಆದರೆ(Curl_uint32_bset_ಮೊದಲ(&multi->dirty, &mid)) {
    ಮಾಡು {
      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);
      ಆದರೆ(ದತ್ತಾಂಶ) {
        ಆದರೆ(Curl_uint32_bset_ಒಳಗೊಂಡಿದೆ(&multi->process, mid))
          ಹಿಂತಿರುಗಿ TRUE;
        /* We are no longer processing this transfer */
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        CURL_TRC_M(multi->admin, "dirty transfer %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } ಆಗ(Curl_uint32_bset_ಮುಂದಿನ(&multi->dirty, mid, &mid));
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ multi_timeout(ರಚನೆ Curl_multi *multi,
                          timediff_t *pexire_ಸ್ಥಾನಾಂತರ_us,
                          ಪೂರ್ಣಾಂಕ *timeout_ms)
{
  ಆದರೆ(multi->dead) {
    ಆದರೆ(pexire_ಸ್ಥಾನಾಂತರ_us)
      *pexire_ಸ್ಥಾನಾಂತರ_us = 0;
    *timeout_ms = 0;
    ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ(multi_has_dirties(multi)) {
    ಆದರೆ(pexire_ಸ್ಥಾನಾಂತರ_us) {
      ರಚನೆ curltime now;
      curlx_pnow(&now);
      *pexire_ಸ್ಥಾನಾಂತರ_us = Curl_timeouts_ಸ್ಥಾನಾಂತರ_us(&multi->timeouts, &now);
    }
    *timeout_ms = 0;
    ಹಿಂತಿರುಗಿ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ರಚನೆ curltime now;
    uint32_t mid;

    curlx_pnow(&now);
    *timeout_ms = Curl_timeouts_ಮುಂದಿನ_ms(&multi->timeouts, &now,
                                        pexire_ಸ್ಥಾನಾಂತರ_us, &mid);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
    ಆದರೆ(mid != UINT32_MAX) {
      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);
      ಆದರೆ(ದತ್ತಾಂಶ && CURL_TRC_TIMER_is_verbose(ದತ್ತಾಂಶ) &&
         (ದತ್ತಾಂಶ->state.timeouts.ಮೊದಲ < EXPIRE_ಕೊನೆಯ)) {
        CURL_TRC_TIMER(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.timeouts.ಮೊದಲ,
                       "gives multi timeout in %dms", *timeout_ms);
      }
    }
#ಆದರೆ_ಅಂತ್ಯ
  }
}

CURLMcode curl_multi_timeout(CURLM *m,
                             ದೀರ್ಘ *timeout_ms)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_timeout, &mresult)) {
    ಪೂರ್ಣಾಂಕ itimeout_ms;

    multi_timeout(m, ಖಾಲಿ, &itimeout_ms);
    *timeout_ms = (ದೀರ್ಘ)itimeout_ms;
    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

/*
 * Tell the application it should update its timers, if it subscribes to the
 * update timer callback.
 */
CURLMcode Curl_update_timer(ರಚನೆ Curl_multi *multi)
{
  timediff_t timeouts_ಸ್ಥಾನಾಂತರ_us = 0;
  ಪೂರ್ಣಾಂಕ timeout_ms;
  ಪೂರ್ಣಾಂಕ rc;
  bool ಗಣ_ಮೌಲ್ಯ = FALSE;

  ಆದರೆ(!multi->timer_cb || multi->dead)
    ಹಿಂತಿರುಗಿ CURLM_OK;
  multi_timeout(multi, &timeouts_ಸ್ಥಾನಾಂತರ_us, &timeout_ms);

  ಆದರೆ(timeout_ms < 0 && !multi->ಕೊನೆಯ_timeout_ಗಣ) {
    /* nothing to do */
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(timeout_ms < 0) {
    /* there is no timeout now but there was one previously */
    CURL_TRC_M(multi->admin, "[TIMER] clear");
    timeout_ms = -1; /* normalize */
    ಗಣ_ಮೌಲ್ಯ = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!multi->ಕೊನೆಯ_timeout_ಗಣ) {
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, none before", timeout_ms);
    ಗಣ_ಮೌಲ್ಯ = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(multi->ಕೊನೆಯ_expire_ಸ್ಥಾನಾಂತರ_us != timeouts_ಸ್ಥಾನಾಂತರ_us) {
    /* We had a timeout before and have one now, the absolute timestamp
     * differs. The relative timeout_ms may be the same, but the starting
     * point differs. Let the application restart its timer. */
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, replace previous",
               timeout_ms);
    ಗಣ_ಮೌಲ್ಯ = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* We have same expire time as previously. Our relative 'timeout_ms'
     * may be different now, but the application has the timer running
     * and we do not to tell it to start this again. */
  }

  ಆದರೆ(ಗಣ_ಮೌಲ್ಯ) {
    ರಚನೆ Curl_mapi_guard guard;

    multi->ಕೊನೆಯ_expire_ಸ್ಥಾನಾಂತರ_us = timeouts_ಸ್ಥಾನಾಂತರ_us;
    multi->ಕೊನೆಯ_timeout_ಗಣ = timeout_ms >= 0;
    CURL_CBAPI_MULTI_ಆರಂಭ(&guard, multi, multi_timer_cb);
    rc = multi->timer_cb(multi, timeout_ms, multi->timer_userp);
    CURL_CBAPI_MULTI_ಕೊನೆ(&guard);
    ಆದರೆ(rc == -1) {
      multi->dead = TRUE;
      ಹಿಂತಿರುಗಿ CURLM_ABORTED_BY_CALLBACK;
    }
  }
  ಹಿಂತಿರುಗಿ CURLM_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
ಸ್ಥಿರ bool multi_timeouts_check(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ expire_timers *timeouts = &ದತ್ತಾಂಶ->state.timeouts;
  uint8_t id;
  ಪೂರ್ಣಾಂಕ i = 0;
  ಗಾಗಿ(id = timeouts->ಮೊದಲ; id < EXPIRE_ಕೊನೆಯ; id = timeouts->ಮುಂದಿನ[id]) {
    ಆದರೆ(++i >= EXPIRE_ಕೊನೆಯ) {
      failf(ದತ್ತಾಂಶ, "expire timeouts looped: %d iterations and no end", i);
      ಹಿಂತಿರುಗಿ FALSE;
    }
    ಆದರೆ(id == timeouts->ಮುಂದಿನ[id]) {
      failf(ದತ್ತಾಂಶ, "expire timeouts wrong: %d points to itself", (ಪೂರ್ಣಾಂಕ)id);
      ಹಿಂತಿರುಗಿ FALSE;
    }
    ಆದರೆ((timeouts->ಮುಂದಿನ[id] < EXPIRE_ಕೊನೆಯ) &&
       (timeouts->ಸ್ಥಾನಾಂತರ_us[id] > timeouts->ಸ್ಥಾನಾಂತರ_us[timeouts->ಮುಂದಿನ[id]])) {
      failf(ದತ್ತಾಂಶ, "expire timeouts not sorted: %d happens after %d but "
            "is listed before", (ಪೂರ್ಣಾಂಕ)id, (ಪೂರ್ಣಾಂಕ)timeouts->ಮುಂದಿನ[id]);
      ಹಿಂತಿರುಗಿ FALSE;
    }
  }
  ಹಿಂತಿರುಗಿ TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Remove a given timestamp from the list of timeouts.
 */
ಸ್ಥಿರ ಶೂನ್ಯ multi_clear_timeout(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, expire_id eid)
{
  ರಚನೆ expire_timers *timeouts = &ದತ್ತಾಂಶ->state.timeouts;
  uint8_t orig_ಮೊದಲ = timeouts->ಮೊದಲ;
  uint8_t *anchor = &timeouts->ಮೊದಲ;
  uint8_t id = (uint8_t)eid;

  ಆದರೆ((ಚಿಹ್ನೆರಹಿತ)eid >= EXPIRE_ಕೊನೆಯ) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ;
  }

  ಆಗ(*anchor < EXPIRE_ಕೊನೆಯ) {
    ಆದರೆ(*anchor == id) {
      *anchor = timeouts->ಮುಂದಿನ[id];
      ನಿಲ್ಲಿಸು;
    }
    anchor = &timeouts->ಮುಂದಿನ[*anchor];
  }
  DEBUGASSERT(multi_timeouts_check(ದತ್ತಾಂಶ));
  ಆದರೆ(Curl_timeouts_has(ದತ್ತಾಂಶ)) {
    ರಚನೆ Curl_multi *multi = ದತ್ತಾಂಶ->multi;

    ಆದರೆ(!multi) {
      DEBUGASSERT(0);
      ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ((timeouts->ಮೊದಲ >= EXPIRE_ಕೊನೆಯ) || /* no more timeouts */
       (timeouts->ಮೊದಲ != orig_ಮೊದಲ)) {  /* active timeout changed */
      Curl_timeouts_remove(&multi->timeouts, ದತ್ತಾಂಶ);
    }
    ಆದರೆ((timeouts->ಮೊದಲ < EXPIRE_ಕೊನೆಯ) && !Curl_timeouts_has(ದತ್ತಾಂಶ)) {
      Curl_timeouts_ಕೂಡಿಸು(&multi->timeouts, ದತ್ತಾಂಶ,
                        timeouts->ಸ್ಥಾನಾಂತರ_us[timeouts->ಮೊದಲ]);
    }
  }
}

/*
 * Add a timestamp to the list of timeouts. Keep the list sorted so that head
 * of list is always the timeout nearest in time.
 */
ಸ್ಥಿರ CURLMcode multi_ಗಣ_timeout(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *stamp,
                                   expire_id eid)
{
  ರಚನೆ expire_timers *timeouts = &ದತ್ತಾಂಶ->state.timeouts;
  uint8_t *anchor = &timeouts->ಮೊದಲ;
  uint8_t id = (uint8_t)eid;

  ಆದರೆ((ಚಿಹ್ನೆರಹಿತ)eid >= EXPIRE_ಕೊನೆಯ) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ CURLM_BAD_FUNCTION_ವಾದ;
  }
  /* remove from list, store time and re-insert */
  multi_clear_timeout(ದತ್ತಾಂಶ, eid);
  timeouts->ಸ್ಥಾನಾಂತರ_us[id] =
    Curl_timeouts_ಸ್ಥಾನಾಂತರ_us(&ದತ್ತಾಂಶ->multi->timeouts, stamp);

  ಆಗ(*anchor < EXPIRE_ಕೊನೆಯ) {
    ಆದರೆ(timeouts->ಸ್ಥಾನಾಂತರ_us[*anchor] > timeouts->ಸ್ಥಾನಾಂತರ_us[id])
      ನಿಲ್ಲಿಸು;
    anchor = &timeouts->ಮುಂದಿನ[*anchor];
  }
  timeouts->ಮುಂದಿನ[eid] = *anchor;
  timeouts->ಮುಂದಿನ[eid] = *anchor;
  *anchor = id;
  DEBUGASSERT(multi_timeouts_check(ದತ್ತಾಂಶ));
  CURL_TRC_TIMER(ದತ್ತಾಂಶ, eid, "set for %" FMT_TIMEDIFF_T "us",
                 curlx_ptimediff_us(stamp, Curl_pgrs_now(ದತ್ತಾಂಶ)));
  ಹಿಂತಿರುಗಿ CURLM_OK;
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
ಶೂನ್ಯ Curl_expire_ಗಣ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                     expire_id eid, timediff_t ms,
                     ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ರಚನೆ Curl_multi *multi = ದತ್ತಾಂಶ->multi;
  ರಚನೆ expire_timers *timeouts = &ದತ್ತಾಂಶ->state.timeouts;
  uint8_t prev_id = timeouts->ಮೊದಲ;
  ರಚನೆ curltime ಗಣ;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  ಆದರೆ(!multi)
    ಹಿಂತಿರುಗಿ;
  DEBUGASSERT(eid < EXPIRE_ಕೊನೆಯ);
  ಆದರೆ(ms > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
    /* Cap ridiculous timeouts, 31-bit ms is still 3.5 weeks. When the time
       goes to the user, it must fit in this size. */
    ms = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;

  ಗಣ = *pnow;
  ಗಣ.tv_sec += (time_t)(ms / 1000); /* may be a 64 to 32-bit conversion */
  ಗಣ.tv_usec += (ಪೂರ್ಣಾಂಕ)(ms % 1000) * 1000;
  ಆದರೆ(ಗಣ.tv_usec >= 1000000) {
    ಗಣ.tv_sec++;
    ಗಣ.tv_usec -= 1000000;
  }

  /* Add the timeout, will replace any previous value for this timer. */
  multi_ಗಣ_timeout(ದತ್ತಾಂಶ, &ಗಣ, eid);
  DEBUGASSERT(timeouts->ಮೊದಲ < EXPIRE_ಕೊನೆಯ);

  ಆದರೆ(Curl_timeouts_has(ದತ್ತಾಂಶ)) {
    /* data has already a timeout registered. If the first timer
     * was NOT the one we just set AND is still the first one,
     * nothing changed from the timeouts point of view. The
     * set timer triggers after the one already registered. Leave. */
    ಆದರೆ((prev_id != eid) && (prev_id == timeouts->ಮೊದಲ))
      ಹಿಂತಿರುಗಿ;

    /* Since this is an updated time, we must remove data from
     * timeouts and then add it again. */
    Curl_timeouts_remove(&multi->timeouts, ದತ್ತಾಂಶ);
  }

  /* Insert the new timer expiry since it is our local minimum. */
  Curl_timeouts_ಕೂಡಿಸು(&multi->timeouts, ದತ್ತಾಂಶ,
                    timeouts->ಸ್ಥಾನಾಂತರ_us[timeouts->ಮೊದಲ]);
}

ಶೂನ್ಯ Curl_expire(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                 timediff_t milli, expire_id eid)
{
  Curl_expire_ಗಣ(ದತ್ತಾಂಶ, eid, milli, Curl_pgrs_now(ದತ್ತಾಂಶ));
}

/*
 * Removes the expire timer. Marks it as done.
 */
ಶೂನ್ಯ Curl_expire_clear(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, expire_id eid)
{
  /* remove the timer, if there */
  multi_clear_timeout(ದತ್ತಾಂಶ, eid);
  CURL_TRC_TIMER(ದತ್ತಾಂಶ, eid, "cleared");
}

/*
 * Clear ALL timeout values for this handle.
 */
ಶೂನ್ಯ Curl_expire_clear_all(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_multi *multi = ದತ್ತಾಂಶ->multi;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  ಆದರೆ(!multi)
    ಹಿಂತಿರುಗಿ;

  ಆದರೆ(Curl_timeouts_remove(&multi->timeouts, ದತ್ತಾಂಶ)) {
    /* Since this is an cleared time, we must remove the previous entry from
       the splay tree */
    multi_timeouts_init(ದತ್ತಾಂಶ);

    ಆದರೆ(ದತ್ತಾಂಶ->id >= 0)
      CURL_TRC_M(ದತ್ತಾಂಶ, "[TIMEOUT] all cleared");
  }
}

CURLMcode curl_multi_ನಿಯೋಜಿಸು(CURLM *m, curl_socket_t sockfd,
                            ಶೂನ್ಯ *sockp)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_ನಿಯೋಜಿಸು, &mresult)) {
    mresult = Curl_multi_ev_ನಿಯೋಜಿಸು(m, sockfd, sockp);
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

ಸ್ಥಿರ ಶೂನ್ಯ move_pending_ಗಮ್ಯ_connect(ರಚನೆ Curl_multi *multi,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  DEBUGASSERT(ದತ್ತಾಂಶ->mstate == MSTATE_PENDING);

  /* Remove this node from the pending set, add into process set */
  Curl_uint32_bset_remove(&multi->pending, ದತ್ತಾಂಶ->mid);
  Curl_uint32_bset_ಕೂಡಿಸು(&multi->process, ದತ್ತಾಂಶ->mid);

  multistate(ದತ್ತಾಂಶ, MSTATE_CONNECT);
  Curl_multi_mark_dirty(ದತ್ತಾಂಶ); /* make it run */
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
ಸ್ಥಿರ ಶೂನ್ಯ multi_ವೇಳಾಪಟ್ಟಿ_pending(ರಚನೆ Curl_multi *multi)
{
  uint32_t mid = multi->ಕೊನೆಯ_pending_mid;

  ಆದರೆ(mid) {
    ಆಗ(Curl_uint32_bset_ಮುಂದಿನ(&multi->pending, mid, &mid)) {
      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);
      ಆದರೆ(ದತ್ತಾಂಶ) {
        move_pending_ಗಮ್ಯ_connect(multi, ದತ್ತಾಂಶ);
        multi->ಕೊನೆಯ_pending_mid = mid;
        ಹಿಂತಿರುಗಿ;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    }
    /* found no pending transfers with `mid` larger than `last_pending_mid`.
     * Start at the beginning of the pending set again. */
    multi->ಕೊನೆಯ_pending_mid = 0;
  }

  ಆದರೆ(Curl_uint32_bset_ಮೊದಲ(&multi->pending, &mid)) {
    ಮಾಡು {
      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_multi_get_easy(multi, mid);
      ಆದರೆ(ದತ್ತಾಂಶ) {
        move_pending_ಗಮ್ಯ_connect(multi, ದತ್ತಾಂಶ);
        multi->ಕೊನೆಯ_pending_mid = mid;
        ಹಿಂತಿರುಗಿ;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    } ಆಗ(Curl_uint32_bset_ಮುಂದಿನ(&multi->pending, mid, &mid));
  }
}

uint32_t Curl_multi_max_concurrent_streams(ರಚನೆ Curl_multi *multi)
{
  DEBUGASSERT(multi);
  ಹಿಂತಿರುಗಿ multi->max_concurrent_streams;
}

CURL **curl_multi_get_handles(CURLM *m)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURL **a = ಖಾಲಿ;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_get_handles, ಖಾಲಿ)) {
    ರಚನೆ Curl_multi *multi = m;
    ಶೂನ್ಯ *ನಮೂದು;
    size_t ಎಣಿಕೆ_2 = Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers);

    a = curlx_malloc(ಗಾತ್ರ(ರಚನೆ Curl_easy *) * (ಎಣಿಕೆ_2 + 1));
    ಆದರೆ(a) {
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i = 0;
      uint32_t mid;

      ಆದರೆ(Curl_uint32_tbl_ಮೊದಲ(&multi->xfers, &mid, &ನಮೂದು)) {
        ಮಾಡು {
          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = ನಮೂದು;
          DEBUGASSERT(i < ಎಣಿಕೆ_2);
          ಆದರೆ(!ದತ್ತಾಂಶ->state.ಆಂತರಿಕ)
            a[i++] = ದತ್ತಾಂಶ;
        } ಆಗ(Curl_uint32_tbl_ಮುಂದಿನ(&multi->xfers, mid, &mid, &ನಮೂದು));
      }
      a[i] = ಖಾಲಿ; /* last entry is a NULL */
    }
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ a;
}

CURLMcode curl_multi_get_offt(CURLM *m,
                              CURLMinfo_offt info,
                              curl_off_t *pvalue)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_get_offt, &mresult)) {
    ರಚನೆ Curl_multi *multi = m;
    uint32_t n;

    ಆದರೆ(!pvalue) {
      mresult = CURLM_BAD_FUNCTION_ವಾದ;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆಯ್ಕೆ(info) {
    ಸ್ಥಿತಿ CURLMINFO_XFERS_CURRENT:
      n = Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers);
      ಆದರೆ(n && multi->admin)
        --n;
      *pvalue = (curl_off_t)n;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMINFO_XFERS_RUNNING:
      n = Curl_uint32_bset_ಎಣಿಕೆ(&multi->process);
      ಆದರೆ(n && Curl_uint32_bset_ಒಳಗೊಂಡಿದೆ(&multi->process, multi->admin->mid))
        --n;
      *pvalue = (curl_off_t)n;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMINFO_XFERS_PENDING:
      *pvalue = (curl_off_t)Curl_uint32_bset_ಎಣಿಕೆ(&multi->pending);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMINFO_XFERS_ಮುಗಿದ:
      *pvalue = (curl_off_t)Curl_uint32_bset_ಎಣಿಕೆ(&multi->msgsent);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLMINFO_XFERS_ADDED:
      *pvalue = multi->xfers_ಒಟ್ಟು_ever;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      *pvalue = -1;
      mresult = CURLM_UNKNOWN_OPTION;
      ನಿಲ್ಲಿಸು;
    }
  }
ಹೊರಹೋಗು:
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

ಸ್ಥಿರ ರಚನೆ Curl_fixed_buf *fixed_buf_create(size_t len)
{
  ರಚನೆ Curl_fixed_buf *fbuf;
  ಆದರೆ((SIZE_MAX - ಗಾತ್ರ(*fbuf)) < len)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* too large */
  fbuf = curlx_malloc(len + ಗಾತ್ರ(*fbuf));
  ಆದರೆ(fbuf)
    fbuf->len = len;
  ಹಿಂತಿರುಗಿ fbuf;
}

CURLcode Curl_multi_xfer_buf_borrow(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಅಕ್ಷರ **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->multi);
  *pbuf = ಖಾಲಿ;
  *pbuflen = 0;
  ಆದರೆ(!ದತ್ತಾಂಶ->multi) {
    failf(ದತ್ತಾಂಶ, "transfer has no multi handle");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.ಬಫರ್_ಗಾತ್ರ) {
    failf(ದತ್ತಾಂಶ, "transfer buffer size is 0");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  ಆದರೆ(ದತ್ತಾಂಶ->multi->xfer_buf_borrowed) {
    failf(ದತ್ತಾಂಶ, "attempt to borrow xfer_buf when already borrowed");
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->multi->xfer_buf &&
     ದತ್ತಾಂಶ->ಗಣ.ಬಫರ್_ಗಾತ್ರ > ದತ್ತಾಂಶ->multi->xfer_buf->len) {
    /* not large enough, get a new one */
    curlx_safefree(ದತ್ತಾಂಶ->multi->xfer_buf);
  }

  ಆದರೆ(!ದತ್ತಾಂಶ->multi->xfer_buf) {
    ದತ್ತಾಂಶ->multi->xfer_buf =
      fixed_buf_create(curlx_uitouz(ದತ್ತಾಂಶ->ಗಣ.ಬಫರ್_ಗಾತ್ರ));
    ಆದರೆ(!ದತ್ತಾಂಶ->multi->xfer_buf) {
      failf(ದತ್ತಾಂಶ, "could not allocate xfer_buf of %u bytes",
            ದತ್ತಾಂಶ->ಗಣ.ಬಫರ್_ಗಾತ್ರ);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }

  ದತ್ತಾಂಶ->multi->xfer_buf_borrowed = TRUE;
  *pbuf = ದತ್ತಾಂಶ->multi->xfer_buf->ದತ್ತಾಂಶ;
  *pbuflen = ದತ್ತಾಂಶ->multi->xfer_buf->len;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_multi_xfer_buf_release(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಅಕ್ಷರ *buf)
{
  (ಶೂನ್ಯ)buf;
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->multi);
  DEBUGASSERT(!buf || (ದತ್ತಾಂಶ->multi->xfer_buf &&
                       ದತ್ತಾಂಶ->multi->xfer_buf->ದತ್ತಾಂಶ == buf));
  ದತ್ತಾಂಶ->multi->xfer_buf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_ulbuf_borrow(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ಅಕ್ಷರ **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->multi);
  *pbuf = ಖಾಲಿ;
  *pbuflen = 0;
  ಆದರೆ(!ದತ್ತಾಂಶ->multi) {
    failf(ದತ್ತಾಂಶ, "transfer has no multi handle");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  ಆದರೆ(ದತ್ತಾಂಶ->multi->xfer_ulbuf_borrowed) {
    failf(ದತ್ತಾಂಶ, "attempt to borrow xfer_ulbuf when already borrowed");
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->multi->xfer_ulbuf &&
     ದತ್ತಾಂಶ->ಗಣ.upload_ಬಫರ್_ಗಾತ್ರ > ದತ್ತಾಂಶ->multi->xfer_ulbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(ದತ್ತಾಂಶ->multi->xfer_ulbuf);
  }

  ಆದರೆ(!ದತ್ತಾಂಶ->multi->xfer_ulbuf) {
    ದತ್ತಾಂಶ->multi->xfer_ulbuf =
      fixed_buf_create(curlx_uitouz(ದತ್ತಾಂಶ->ಗಣ.upload_ಬಫರ್_ಗಾತ್ರ));
    ಆದರೆ(!ದತ್ತಾಂಶ->multi->xfer_ulbuf) {
      failf(ದತ್ತಾಂಶ, "could not allocate xfer_ulbuf of %u bytes",
            ದತ್ತಾಂಶ->ಗಣ.upload_ಬಫರ್_ಗಾತ್ರ);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }

  ದತ್ತಾಂಶ->multi->xfer_ulbuf_borrowed = TRUE;
  *pbuf = ದತ್ತಾಂಶ->multi->xfer_ulbuf->ದತ್ತಾಂಶ;
  *pbuflen = ದತ್ತಾಂಶ->multi->xfer_ulbuf->len;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_multi_xfer_ulbuf_release(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಅಕ್ಷರ *buf)
{
  (ಶೂನ್ಯ)buf;
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ದತ್ತಾಂಶ->multi);
  DEBUGASSERT(!buf || (ದತ್ತಾಂಶ->multi->xfer_ulbuf &&
                       ದತ್ತಾಂಶ->multi->xfer_ulbuf->ದತ್ತಾಂಶ == buf));
  ದತ್ತಾಂಶ->multi->xfer_ulbuf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_sockbuf_borrow(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        size_t blen, ಅಕ್ಷರ **pbuf)
{
  DEBUGASSERT(ದತ್ತಾಂಶ);
  *pbuf = ಖಾಲಿ;
  ಆದರೆ(!ದತ್ತಾಂಶ->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    *pbuf = curlx_malloc(blen);
    ಹಿಂತಿರುಗಿ *pbuf ? CURLE_OK : CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಆದರೆ(ದತ್ತಾಂಶ->multi->xfer_sockbuf_borrowed) {
    failf(ದತ್ತಾಂಶ, "attempt to borrow xfer_sockbuf when already borrowed");
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->multi->xfer_sockbuf && blen > ದತ್ತಾಂಶ->multi->xfer_sockbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(ದತ್ತಾಂಶ->multi->xfer_sockbuf);
  }

  ಆದರೆ(!ದತ್ತಾಂಶ->multi->xfer_sockbuf) {
    ದತ್ತಾಂಶ->multi->xfer_sockbuf = fixed_buf_create(blen);
    ಆದರೆ(!ದತ್ತಾಂಶ->multi->xfer_sockbuf) {
      failf(ದತ್ತಾಂಶ, "could not allocate xfer_sockbuf of %zu bytes", blen);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }

  ದತ್ತಾಂಶ->multi->xfer_sockbuf_borrowed = TRUE;
  *pbuf = ದತ್ತಾಂಶ->multi->xfer_sockbuf->ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_multi_xfer_sockbuf_release(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಅಕ್ಷರ *buf)
{
  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಆದರೆ(!ದತ್ತಾಂಶ->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    curlx_free(buf);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    DEBUGASSERT(!buf || (ದತ್ತಾಂಶ->multi->xfer_sockbuf &&
                         ದತ್ತಾಂಶ->multi->xfer_sockbuf->ದತ್ತಾಂಶ == buf));
    ದತ್ತಾಂಶ->multi->xfer_sockbuf_borrowed = FALSE;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ multi_xfer_bufs_free(ರಚನೆ Curl_multi *multi)
{
  DEBUGASSERT(multi);
  curlx_safefree(multi->xfer_buf);
  multi->xfer_buf_borrowed = FALSE;
  curlx_safefree(multi->xfer_ulbuf);
  multi->xfer_ulbuf_borrowed = FALSE;
  curlx_safefree(multi->xfer_sockbuf);
  multi->xfer_sockbuf_borrowed = FALSE;
}

ರಚನೆ Curl_easy *Curl_multi_get_easy(ರಚನೆ Curl_multi *multi,
                                      uint32_t mid)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_uint32_tbl_get(&multi->xfers, mid);
  ಆದರೆ(GOOD_EASY_HANDLE(ದತ್ತಾಂಶ))
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
  CURL_TRC_M(multi->admin, "invalid easy handle in xfer table for mid=%u",
             mid);
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

bool Curl_multi_knows_easy(ರಚನೆ Curl_multi *multi, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ Curl_uint32_tbl_get(&multi->xfers, ದತ್ತಾಂಶ->mid) == ದತ್ತಾಂಶ;
}

ಸ್ಥಿರ uint32_t multi_xfers_running(ರಚನೆ Curl_multi *multi)
{
  ಆದರೆ(!multi) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ multi->xfers_alive;
}

uint32_t Curl_multi_xfers_attached(ರಚನೆ Curl_multi *multi)
{
  ಆದರೆ(!multi || !multi->admin) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ 0;
  }
  /* Discount the admin handle */
  ಹಿಂತಿರುಗಿ Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers) - 1;
}

ಶೂನ್ಯ Curl_multi_mark_dirty(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(ದತ್ತಾಂಶ->multi && ದತ್ತಾಂಶ->mid != UINT32_MAX)
    Curl_uint32_bset_ಕೂಡಿಸು(&ದತ್ತಾಂಶ->multi->dirty, ದತ್ತಾಂಶ->mid);
}

ಶೂನ್ಯ Curl_multi_clear_dirty(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(ದತ್ತಾಂಶ->multi && ದತ್ತಾಂಶ->mid != UINT32_MAX)
    Curl_uint32_bset_remove(&ದತ್ತಾಂಶ->multi->dirty, ದತ್ತಾಂಶ->mid);
}

CURLMcode curl_multi_notify_enable(CURLM *m, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ notification)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_notify_enable, &mresult)) {
    mresult = Curl_mntfy_enable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

CURLMcode curl_multi_notify_disable(CURLM *m, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ notification)
{
  ರಚನೆ Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  ಆದರೆ(CURL_MAPI_ENTER(&guard, m, multi_notify_disable, &mresult)) {
    mresult = Curl_mntfy_disable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ mresult;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
ಸ್ಥಿರ ಶೂನ್ಯ multi_xfer_dump(ರಚನೆ Curl_multi *multi, uint32_t mid,
                            ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = ನಮೂದು;

  (ಶೂನ್ಯ)multi;
  ಆದರೆ(!ದತ್ತಾಂಶ) {
    curl_mfprintf(stderr, "mid=%u, entry=NULL, bug in xfer table?\n", mid);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    curl_mfprintf(stderr, "mid=%u, magic=%s, p=%p, id=%" FMT_OFF_T
                  ", url=%s\n",
                  mid,
                  (ದತ್ತಾಂಶ->magic == CURLEASY_MAGIC_ಸಂಖ್ಯೆ) ? "GOOD" : "BAD!",
                  (ಶೂನ್ಯ *)ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->id, Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url));
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ multi_xfer_tbl_dump(ರಚನೆ Curl_multi *multi)
{
  uint32_t mid;
  ಶೂನ್ಯ *ನಮೂದು;
  curl_mfprintf(stderr, "=== multi xfer table (count=%u, capacity=%u\n",
                Curl_uint32_tbl_ಎಣಿಕೆ(&multi->xfers),
                Curl_uint32_tbl_ಸಾಮರ್ಥ್ಯ(&multi->xfers));
  ಆದರೆ(Curl_uint32_tbl_ಮೊದಲ(&multi->xfers, &mid, &ನಮೂದು)) {
    multi_xfer_dump(multi, mid, ನಮೂದು);
    ಆಗ(Curl_uint32_tbl_ಮುಂದಿನ(&multi->xfers, mid, &mid, &ನಮೂದು))
      multi_xfer_dump(multi, mid, ನಮೂದು);
  }
  curl_mfprintf(stderr, "===\n");
  fflush(stderr);
}
#ಆದರೆ_ಅಂತ್ಯ /* DEBUGBUILD */
