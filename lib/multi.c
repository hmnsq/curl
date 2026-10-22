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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "transfer.h"
#समावेश "url.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "progress.h"
#समावेश "curl_share.h"
#समावेश "psl.h"
#समावेश "multiif.h"
#समावेश "multi_ev.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "http.h"
#समावेश "select.h"
#समावेश "curlx/wait.h"
#समावेश "conncache.h"
#समावेश "multihandle.h"
#समावेश "sigpipe.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/vtls_scache.h"
#समावेश "http_proxy.h"
#समावेश "http2.h"
#समावेश "socketpair.h"
#समावेश "bufref.h"
#समावेश "curlx/strparse.h"

/* initial multi->xfers table size for a full multi */
#घोषणा CURL_XFER_TABLE_आकार 128

/* CURL_SOCKET_HASH_TABLE_SIZE should be a prime number. Increasing it from 97
   to 911 takes on a 32-bit machine 4 x 804 = 3211 more bytes. Still, every
   curl handle takes 6K memory, therefore this 3K are not significant. */
#अगर_अघोषित CURL_SOCKET_HASH_TABLE_आकार
#घोषणा CURL_SOCKET_HASH_TABLE_आकार 911
#पूर्ण_अगर

#अगर_अघोषित CURL_CONNECTION_HASH_आकार
#घोषणा CURL_CONNECTION_HASH_आकार 97
#पूर्ण_अगर

#अगर_अघोषित CURL_DNS_HASH_आकार
#घोषणा CURL_DNS_HASH_आकार 71
#पूर्ण_अगर

#अगर_अघोषित CURL_TLS_SESSION_आकार
#घोषणा CURL_TLS_SESSION_आकार 25
#पूर्ण_अगर

अटल व्योम move_pending_to_connect(काष्ठा Curl_multi *multi,
                                    काष्ठा Curl_easy *तथ्य);
अटल CURLMcode add_अग्र_timeout(स्थिर काष्ठा curltime *pnow,
                                  काष्ठा Curl_multi *multi,
                                  काष्ठा Curl_easy *तथ्य);
अटल व्योम multi_timeout(काष्ठा Curl_multi *multi,
                          timediff_t *pexire_offset_us,
                          पूर्णांक *timeout_ms);
अटल व्योम multi_schedule_pending(काष्ठा Curl_multi *multi);
अटल व्योम multi_xfer_bufs_free(काष्ठा Curl_multi *multi);
#अगर_घोषित DEBUGBUILD
अटल व्योम multi_xfer_tbl_dump(काष्ठा Curl_multi *multi);
#पूर्ण_अगर
/* Get the # of transfers current in process/pending. */
अटल uint32_t multi_xfers_running(काष्ठा Curl_multi *multi);

/* function pointer called once when entering a state */
प्रकार व्योम (*mstate_enter_func)(काष्ठा Curl_easy *तथ्य,
                                  CURLMstate from_state);

अटल व्योम mstate_enter_connect(काष्ठा Curl_easy *तथ्य,
                                 CURLMstate from_state)
{
  (व्योम)from_state;
  Curl_init_CONNECT(तथ्य);
}

अटल व्योम mstate_enter_did(काष्ठा Curl_easy *तथ्य,
                             CURLMstate from_state)
{
  (व्योम)from_state;
  तथ्य->req.chunk = FALSE;
  Curl_pgrsTime(तथ्य, TIMER_PRETRANSFER);
  अगर(!CURL_REQ_WANT_SEND(तथ्य))
    Curl_pgrsTime(तथ्य, TIMER_POSTRANSFER);
}

अटल व्योम mstate_enter_done(काष्ठा Curl_easy *तथ्य,
                              CURLMstate from_state)
{
  (व्योम)from_state;
  CURLM_NTFY(तथ्य, CURLMNOTIFY_EASY_DONE);
}

अटल व्योम mstate_enter_completed(काष्ठा Curl_easy *तथ्य,
                                   CURLMstate from_state)
{
  /* we sometimes directly jump to COMPLETED, trigger things
   * we then missed. */
  अगर(from_state < MSTATE_DID) {
    Curl_pgrsTime(तथ्य, TIMER_PRETRANSFER);
    Curl_pgrsTime(तथ्य, TIMER_POSTRANSFER);
    Curl_pgrsTime(तथ्य, TIMER_STARTTRANSFER);
  }
  /* multi_done() should have run and detached any connection already */
  DEBUGASSERT(!तथ्य->conn);
  Curl_pgrsCompleted(तथ्य);
  अगर(from_state < MSTATE_DONE)
    CURLM_NTFY(तथ्य, CURLMNOTIFY_EASY_DONE);
  /* changing to COMPLETED means it is in process and needs to go */
  DEBUGASSERT(Curl_uint32_bset_contains(&तथ्य->multi->process, तथ्य->mid));
  Curl_uint32_bset_remove(&तथ्य->multi->process, तथ्य->mid);
  Curl_uint32_bset_remove(&तथ्य->multi->pending, तथ्य->mid); /* to be sure */

  अगर(Curl_uint32_bset_empty(&तथ्य->multi->process)) {
    /* free the transfer buffer when we have no more active transfers */
    multi_xfer_bufs_free(तथ्य->multi);
  }
  /* Important: reset the conn pointer so that we do not point to memory
     that could be freed anytime */
  Curl_detach_connection(तथ्य);
  Curl_expire_clear_all(तथ्य); /* stop all timers */
}

/* always use this function to change state, to make debugging easier */
अटल व्योम mstate(काष्ठा Curl_easy *तथ्य, CURLMstate state
#अगर_घोषित DEBUGBUILD
                   , पूर्णांक lineno
#पूर्ण_अगर
)
{
  CURLMstate oldstate = तथ्य->mstate;
  अटल स्थिर mstate_enter_func state_enter[MSTATE_LAST] = {
    शून्य,                      /* INIT */
    शून्य,                      /* PENDING */
    शून्य,                      /* SETUP */
    mstate_enter_connect,      /* CONNECT */
    शून्य,                      /* CONNECTING */
    शून्य,                      /* PROTOCONNECT */
    शून्य,                      /* PROTOCONNECTING */
    शून्य,                      /* DO */
    शून्य,                      /* DOING */
    शून्य,                      /* DOING_MORE */
    mstate_enter_did,          /* DID */
    शून्य,                      /* PERFORMING */
    शून्य,                      /* RATELIMITING */
    mstate_enter_done,         /* DONE */
    mstate_enter_completed,    /* COMPLETED */
    शून्य                       /* MSGSENT */
  };

  अगर(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    वापस;

#अगर_घोषित DEBUGBUILD
  NOVERBOSE((व्योम)lineno);
  CURL_TRC_M(तथ्य, "-> [%s] (line %d)", CURL_MSTATE_NAME(state), lineno);
#अन्यथा
  CURL_TRC_M(तथ्य, "-> [%s]", CURL_MSTATE_NAME(state));
#पूर्ण_अगर

  /* really switching state */
  तथ्य->mstate = state;
  अगर(state_enter[state])
    state_enter[state](तथ्य, oldstate);
}

#अगर_अघोषित DEBUGBUILD
#घोषणा multistate(x, y) mstate(x, y)
#अन्यथा
#घोषणा multistate(x, y) mstate(x, y, __LINE__)
#पूर्ण_अगर

/* multi->proto_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
अटल व्योम ph_freeentry(व्योम *p)
{
  (व्योम)p;
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
अटल व्योम multi_addmsg(काष्ठा Curl_multi *multi, काष्ठा Curl_easy *तथ्य)
{
  अगर(Curl_uint32_bset_empty(&multi->msgsent))
    CURLM_NTFY(multi->admin, CURLMNOTIFY_INFO_READ);
  Curl_uint32_bset_add(&multi->msgsent, तथ्य->mid);
}

अटल व्योम multi_timeouts_init(काष्ठा Curl_easy *तथ्य);

काष्ठा Curl_multi *Curl_multi_handle(uint32_t xfer_table_आकार,
                                     माप_प्रकार ev_hashsize,  /* event hash */
                                     माप_प्रकार chashsize, /* connection hash */
                                     माप_प्रकार dnssize,   /* dns hash */
                                     माप_प्रकार sesssize)  /* TLS session cache */
{
  काष्ठा curltime created;
  काष्ठा Curl_multi *multi = curlx_calloc(1, माप(काष्ठा Curl_multi));

  अगर(!multi)
    वापस शून्य;

  multi->magic = CURLMULTI_MAGIC_संख्या;

  /* Initialisation order is important here!
   * easy_init() does a lazy check on curl_global_init() which sets
   * up platform specific things we need. For example calling curlx_pnow()
   * before this is not safe. */
  multi->admin = curl_easy_init();
  अगर(!multi->admin) {
    curlx_free(multi);
    वापस शून्य;
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
#अगर_घोषित ENABLE_WAKEUP
  multi->wakeup_pair[0] = CURL_SOCKET_BAD;
  multi->wakeup_pair[1] = CURL_SOCKET_BAD;
#पूर्ण_अगर
#अगर_घोषित ENABLE_INTERNAL_WAKEUP
  multi->wakeup_internal[0] = CURL_SOCKET_BAD;
  multi->wakeup_internal[1] = CURL_SOCKET_BAD;
#पूर्ण_अगर

  अगर(Curl_uint32_bset_resize(&multi->process, xfer_table_आकार) ||
     Curl_uint32_bset_resize(&multi->pending, xfer_table_आकार) ||
     Curl_uint32_bset_resize(&multi->dirty, xfer_table_आकार) ||
     Curl_uint32_bset_resize(&multi->msgsent, xfer_table_आकार) ||
     Curl_uint32_tbl_resize(&multi->xfers, xfer_table_आकार))
    जाओ error;

#अगर_घोषित DEBUGBUILD
  {
    स्थिर अक्षर *p;
    अगर(दो_पर्या("CURL_DEBUG"))
      multi->admin->set.verbose = TRUE;
    p = दो_पर्या("CURL_DBG_MAX_TOTAL_CONNECTIONS");
    अगर(p) {
      curl_off_t l;
      अगर(!curlx_str_संख्या(&p, &l, UINT32_MAX)) {
        multi->max_योग_connections = (uint32_t)l;
      }
    }
  }
#पूर्ण_अगर
  Curl_uint32_tbl_add(&multi->xfers, multi->admin, &multi->admin->mid);
  Curl_uint32_bset_add(&multi->process, multi->admin->mid);

  Curl_cpool_init(&multi->cpool, शून्य, chashsize);

#अगर_घोषित USE_SSL
  अगर(Curl_ssl_scache_create(sesssize, 2, &multi->ssl_scache))
    जाओ error;
#अन्यथा
  (व्योम)sesssize;
#पूर्ण_अगर

#अगर_घोषित USE_WINSOCK
  multi->wsa_event = WSACreateEvent();
  अगर(multi->wsa_event == WSA_INVALID_EVENT)
    जाओ error;
#पूर्ण_अगर
#अगर_घोषित ENABLE_WAKEUP
  /* When enabled, rely on this to work. We ignore this in previous
   * versions, but that seems an unnecessary complication. */
  अगर(Curl_wakeup_init(multi->wakeup_pair, TRUE) < 0)
    जाओ error;
#पूर्ण_अगर
#अगर_घोषित ENABLE_INTERNAL_WAKEUP
  अगर(Curl_wakeup_init(multi->wakeup_internal, TRUE) < 0)
    जाओ error;
#पूर्ण_अगर

  अगर(Curl_probeipv6(multi))
    जाओ error;

#अगर_घोषित USE_RESOLV_THREADED
  अगर(xfer_table_आकार < CURL_XFER_TABLE_आकार) { /* easy multi */
    अगर(Curl_async_thrdd_multi_init(multi, 0, 2, 10))
      जाओ error;
  }
  अन्यथा { /* real multi handle */
    अगर(Curl_async_thrdd_multi_init(multi, 0, 20, 2000))
      जाओ error;
  }
#पूर्ण_अगर

  वापस multi;

error:

#अगर_घोषित USE_RESOLV_THREADED
  Curl_async_thrdd_multi_destroy(multi, TRUE);
#पूर्ण_अगर
  Curl_multi_ev_cleanup(multi);
  Curl_hash_destroy(&multi->proto_hash);
  Curl_dnscache_destroy(&multi->dnscache);
  Curl_cpool_destroy(&multi->cpool, multi->admin);
  Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
#अगर_घोषित USE_SSL
  Curl_ssl_scache_destroy(multi->ssl_scache);
#पूर्ण_अगर
  अगर(multi->admin) {
    Curl_multi_ev_xfer_done(multi, multi->admin);
    multi->admin->multi = शून्य;
    Curl_close(&multi->admin);
  }
  Curl_mntfy_cleanup(multi);

  Curl_uint32_bset_destroy(&multi->process);
  Curl_uint32_bset_destroy(&multi->dirty);
  Curl_uint32_bset_destroy(&multi->pending);
  Curl_uint32_bset_destroy(&multi->msgsent);
  Curl_uint32_tbl_destroy(&multi->xfers);
#अगर_घोषित ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#पूर्ण_अगर
#अगर_घोषित ENABLE_INTERNAL_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_internal);
#पूर्ण_अगर

  curlx_free(multi);
  वापस शून्य;
}

CURLM *curl_multi_init(व्योम)
{
  वापस Curl_multi_handle(CURL_XFER_TABLE_आकार,
                           CURL_SOCKET_HASH_TABLE_आकार,
                           CURL_CONNECTION_HASH_आकार,
                           CURL_DNS_HASH_आकार,
                           CURL_TLS_SESSION_आकार);
}

#अगर defined(DEBUGBUILD) && defined(CURLVERBOSE)
अटल व्योम multi_warn_debug(काष्ठा Curl_multi *multi, काष्ठा Curl_easy *तथ्य)
{
  अगर(!multi->warned) {
    infof(तथ्य, "!!! WARNING !!!");
    infof(तथ्य, "This is a debug build of libcurl, "
                "do not use in production.");
    multi->warned = TRUE;
  }
}
#अन्यथा
#घोषणा multi_warn_debug(x, y) Curl_nop_stmt
#पूर्ण_अगर

bool Curl_is_connecting(काष्ठा Curl_easy *तथ्य)
{
  वापस तथ्य->mstate < MSTATE_DO;
}

अटल CURLMcode multi_assess_wakeup(काष्ठा Curl_multi *multi)
{
#अगर_घोषित ENABLE_INTERNAL_WAKEUP
  अगर(multi->socket_cb)
    वापस Curl_multi_ev_assess_xfer(multi, multi->admin);
#अन्यथा
  (व्योम)multi;
#पूर्ण_अगर
  वापस CURLM_OK;
}

अटल CURLMcode multi_xfers_add(काष्ठा Curl_multi *multi,
                                 काष्ठा Curl_easy *तथ्य)
{
  uint32_t capacity = Curl_uint32_tbl_capacity(&multi->xfers);
  uint32_t new_आकार = 0;
  /* Prepare to make this into a CURLMOPT_MAX_TRANSFERS, because some
   * applications may want to prevent a run-away of their memory use. */
  /* UINT_MAX is our "invalid" id, do not let the table grow up to that. */
  स्थिर uint32_t max_capacity = अच_पूर्णांक_उच्च - 1;

  अगर(capacity < max_capacity) {
    /* We want `multi->xfers` to have "sufficient" free rows, so that we do
     * not have to reuse the `mid` from a removed easy right away.
     * Check if an 8th of the capacity is still free */
    uint32_t used = Curl_uint32_tbl_गिनती(&multi->xfers);
    uint32_t unused = capacity - used;
    uint32_t min_unused = CURLMAX(capacity >> 3, 4);
    अगर(unused < min_unused) {
      /* Grow by 50% of current capacity, in range of [128, 2048],
       * which means the table grows max by 16kb on 64-bit arch. */
      uint32_t growth = CURLMIN(CURLMAX(capacity >> 1, 128), 2048);
      /* Make sure the uint arithmetic here works on the corner
       * cases where we are close to max_capacity or UINT_MAX */
      अगर((max_capacity - growth) <= capacity)
        new_आकार = max_capacity;
      अन्यथा
        new_आकार = capacity + growth;
    }
  }

  अगर(new_आकार > capacity) {
    /* Grow the bitsets first. Should one fail, we do not need
     * to downsize the already resized ones. The sets continue
     * to work properly when larger than the table, but not
     * the other way around. */
    CURL_TRC_M(तथ्य, "increasing xfer table size to %u", new_आकार);
    अगर(Curl_uint32_bset_resize(&multi->process, new_आकार) ||
       Curl_uint32_bset_resize(&multi->dirty, new_आकार) ||
       Curl_uint32_bset_resize(&multi->pending, new_आकार) ||
       Curl_uint32_bset_resize(&multi->msgsent, new_आकार) ||
       Curl_uint32_tbl_resize(&multi->xfers, new_आकार))
      वापस CURLM_OUT_OF_MEMORY;
  }

  /* Insert the easy into the table now */
  अगर(!Curl_uint32_tbl_add(&multi->xfers, तथ्य, &तथ्य->mid)) {
    /* MUST only happen when table is full */
    DEBUGASSERT(Curl_uint32_tbl_capacity(&multi->xfers) <=
                Curl_uint32_tbl_गिनती(&multi->xfers));
    वापस CURLM_OUT_OF_MEMORY;
  }
  वापस CURLM_OK;
}

CURLMcode Curl_multi_add_handle(काष्ठा Curl_multi *multi,
                                काष्ठा Curl_easy *तथ्य)
{
  CURLMcode mresult;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  अगर(तथ्य->multi)
    वापस CURLM_ADDED_ALREADY;

  अगर(multi->dead) {
    /* a "dead" handle cannot get added transfers while any existing easy
       handles are still alive - but if there are none alive anymore, it is
       fine to start over and unmark the "deadness" of this handle.
       This means only the admin handle MUST be present. */
    अगर((Curl_uint32_tbl_गिनती(&multi->xfers) != 1) ||
       !Curl_uint32_tbl_contains(&multi->xfers, 0))
      वापस CURLM_ABORTED_BY_CALLBACK;
    multi->dead = FALSE;
    Curl_uint32_bset_clear(&multi->process);
    Curl_uint32_bset_clear(&multi->dirty);
    Curl_uint32_bset_clear(&multi->pending);
    Curl_uint32_bset_clear(&multi->msgsent);
  }

  अगर(तथ्य->multi_easy) {
    /* if this easy handle was previously used for curl_easy_perform(), there
       is a private multi handle here that we can kill */
    curl_multi_cleanup(तथ्य->multi_easy);
    तथ्य->multi_easy = शून्य;
  }

  /* Insert the easy into the multi->xfers table, assigning it a `mid`. */
  अगर(multi_xfers_add(multi, तथ्य))
    वापस CURLM_OUT_OF_MEMORY;

  /* Initialize timeouts for this handle */
  multi_timeouts_init(तथ्य);

  /*
   * No failure allowed in this function beyond this point. No modification
   * of easy nor multi handle allowed before this except for potential
   * multi's connection pool growing which will not be undone in this
   * function no matter what.
   */
  अगर(तथ्य->set.errorbuffer)
    तथ्य->set.errorbuffer[0] = 0;

  तथ्य->state.os_errno = 0;

  /* make the Curl_easy refer back to this multi handle - before
     Curl_expire() is called. */
  तथ्य->multi = multi;

  /* set the easy handle */
  multistate(तथ्य, MSTATE_INIT);
  /* not yet passed INIT state */
  तथ्य->state.really_alive = FALSE;

#अगर_घोषित USE_LIBPSL
  /* Do the same for PSL. */
  अगर(तथ्य->share && (तथ्य->share->specifier & (1 << CURL_LOCK_तथ्य_PSL)))
    तथ्य->psl = &तथ्य->share->psl;
  अन्यथा
    तथ्य->psl = &multi->psl;
#पूर्ण_अगर

  /* add the easy handle to the process set */
  Curl_uint32_bset_add(&multi->process, तथ्य->mid);
  ++multi->xfers_alive;
  ++multi->xfers_योग_ever;

  Curl_cpool_xfer_init(तथ्य);
  multi_warn_debug(multi, तथ्य);

  /* Make sure the new handle will run */
  Curl_multi_mark_dirty(तथ्य);

  /* Necessary in event based processing, where dirty handles trigger
   * a timeout callback invocation. */
  mresult = Curl_update_timer(multi);
  अगर(mresult) {
    /* undo the additions above */
    Curl_uint32_bset_remove(&multi->process, तथ्य->mid);
    Curl_uint32_bset_remove(&multi->dirty, तथ्य->mid);
    --multi->xfers_alive;
    तथ्य->multi = शून्य; /* not anymore */
    Curl_uint32_tbl_remove(&multi->xfers, तथ्य->mid);
    तथ्य->mid = UINT32_MAX;
    वापस mresult;
  }

  /* The admin handle only ever has default timeouts set. To improve the
     state somewhat we clone the timeouts from each added handle so that the
     admin handle always has the same timeouts as the most recently added
     easy handle. */
  multi->admin->set.timeout = तथ्य->set.timeout;
  multi->admin->set.server_response_timeout =
    तथ्य->set.server_response_timeout;
  multi->admin->set.no_signal = तथ्य->set.no_signal;

  CURL_TRC_M(तथ्य, "added to multi, mid=%u, running=%u, total=%u",
             तथ्य->mid, multi_xfers_running(multi),
             Curl_uint32_tbl_गिनती(&multi->xfers));
  वापस CURLM_OK;
}

CURLMcode curl_multi_add_handle(CURLM *m, CURL *curl)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_add_handle, &mresult)) {
    काष्ठा Curl_easy *तथ्य = curl;
    /* Verify that we got a somewhat good easy handle too */
    अगर(!GOOD_EASY_HANDLE(तथ्य) || तथ्य->state.internal)
      mresult = CURLM_BAD_EASY_HANDLE;
    अन्यथा
      mresult = Curl_multi_add_handle(m, तथ्य);
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

काष्ठा multi_done_ctx {
  BIT(premature);
};

अटल bool multi_conn_should_close(काष्ठा connectdata *conn,
                                    काष्ठा Curl_easy *तथ्य,
                                    bool premature)
{
  /* if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of everything else. */
  अगर(conn->bits.बंद)
    वापस TRUE;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM/NEGOTIATE authentication handshake. */
  अगर(तथ्य->set.reuse_forbid
#अगर_घोषित USE_NTLM
     && !(conn->http_ntlm_state == NTLMSTATE_TYPE2 ||
          conn->proxy_ntlm_state == NTLMSTATE_TYPE2)
#पूर्ण_अगर
#अगर_घोषित USE_SPNEGO
     && !(conn->http_negotiate_state == CURL_NEGO_AUTH_RECV ||
          conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)
#पूर्ण_अगर
    )
    वापस TRUE;

  /* Unless this connection is for a "connect-only" transfer, it
   * needs to be closed if the protocol handler does not support reuse. */
  अगर(!तथ्य->set.connect_only && conn->scheme &&
     !(conn->scheme->flags & PROTOPT_CONN_REUSE))
    वापस TRUE;

  /* if premature is TRUE, it means this connection was said to be DONE before
     the entire request operation is complete and thus we cannot know in what
     state it is for reusing, so we are forced to close it. In a perfect world
     we can add code that keep track of if we really must close it here or not,
     but currently we have no such detail knowledge. */
  अगर(premature && !Curl_conn_is_multiplex(conn, FIRSTSOCKET))
    वापस TRUE;

  वापस FALSE;
}

अटल cpool_do_परिणाम multi_done_locked(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा connectdata *conn,
                                         व्योम *userdata,
                                         स्थिर काष्ठा curltime *pnow)
{
  काष्ठा multi_done_ctx *mdctx = userdata;

  (व्योम)pnow;
  CURL_TRC_M(तथ्य, "multi_done_locked, in use=%u", conn->attached_xfers);
  अगर(CONN_INUSE(conn)) {
    /* Stop if still used. */
    CURL_TRC_M(तथ्य, "Connection still in use %u, no more multi_done now!",
               conn->attached_xfers);
    वापस CPOOL_DO_KEEP;
  }

  अगर(multi_conn_should_close(conn, तथ्य, (bool)mdctx->premature)) {
    CURL_TRC_M(तथ्य, "multi_done, terminating conn #%" FMT_OFF_T " to %s:%u, "
               "forbid=%d, close=%d, premature=%d, conn_multiplex=%d",
               conn->connection_id, conn->origin->user_hostname,
               conn->origin->port,
               तथ्य->set.reuse_forbid, conn->bits.बंद, mdctx->premature,
               Curl_conn_is_multiplex(conn, FIRSTSOCKET));
    connclose(conn);
    वापस mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  अन्यथा अगर(!Curl_conn_get_max_concurrent(तथ्य, conn, FIRSTSOCKET)) {
    CURL_TRC_M(तथ्य, "multi_done, conn #%" FMT_OFF_T " to %s:%u was shutdown"
               " by server, not reusing", conn->connection_id,
               conn->origin->user_hostname, conn->origin->port);
    connclose(conn);
    वापस mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  अन्यथा {
    वापस CPOOL_DO_IDLE;
  }
}

अटल CURLcode multi_done(काष्ठा Curl_easy *तथ्य,
                           CURLcode status,  /* an error if this is called
                                                after an error was detected */
                           bool premature)
{
  CURLcode परिणाम;
  काष्ठा connectdata *conn = तथ्य->conn;
  स्थिर काष्ठा curltime *pnow = Curl_pgrs_now(तथ्य);

  CURL_TRC_M(तथ्य, "multi_done: status: %d prem: %d done: %d",
             (पूर्णांक)status, (पूर्णांक)premature, तथ्य->state.done);

  अगर(तथ्य->state.done) {
    /* Stop if multi_done() has already been called */
    वापस CURLE_OK;
  }
  तथ्य->state.done = TRUE; /* called now! */

  /* Shut down any ongoing async resolver operation. */
  Curl_resolv_shutdown_all(तथ्य);

  /* Cleanup possible redirect junk */
  curlx_safefree(तथ्य->req.newurl);
  curlx_safefree(तथ्य->req.location);

  चयन(status) {
  हाल CURLE_ABORTED_BY_CALLBACK:
  हाल CURLE_READ_ERROR:
  हाल CURLE_WRITE_ERROR:
    /* When we are aborted due to a callback return code it has to be counted
       as premature as there is trouble ahead if we do not. We have many
       callbacks and protocols work differently, we could potentially do this
       more fine-grained in the future. */
    premature = TRUE;
    FALLTHROUGH();
  शेष:
    अवरोध;
  }

  /* this calls the protocol-specific function pointer previously set */
  अगर(conn && conn->scheme->run->done && (तथ्य->mstate >= MSTATE_PROTOCONNECT))
    परिणाम = conn->scheme->run->done(तथ्य, status, premature);
  अन्यथा
    परिणाम = status;

  अगर(तथ्य->mstate > MSTATE_CONNECTING &&
     (परिणाम != CURLE_ABORTED_BY_CALLBACK)) {
    /* avoid this if
     * - the transfer has not connected
     * - we already aborted by callback to avoid this calling another callback
     */
    पूर्णांक rc = Curl_pgrsDone(तथ्य, pnow);
    अगर(!परिणाम && rc)
      परिणाम = CURLE_ABORTED_BY_CALLBACK;
  }

  /* Make sure that transfer client writes are really done now. */
  परिणाम = Curl_1st_fatal(परिणाम, Curl_xfer_write_done(तथ्य, premature));

  अगर(!परिणाम)
    परिणाम = Curl_req_done(&तथ्य->req, तथ्य, premature);

  अगर(conn) {
    /* Detach connection and decide what to do with it. */
    काष्ठा multi_done_ctx mdctx;

    /* Inform connection filters that this transfer is done and they
     * need to give up any references to `data`. */
    Curl_conn_ev_तथ्य_done(तथ्य, premature);
    Curl_detach_connection(तथ्य);
    Curl_dnscache_prune(तथ्य, pnow);

    स_रखो(&mdctx, 0, माप(mdctx));
    mdctx.premature = premature;
    Curl_cpool_return(तथ्य, conn, multi_done_locked, &mdctx, pnow);

    अगर(तथ्य->multi)
      multi_schedule_pending(तथ्य->multi);
  }

  /* flush the netrc cache */
  Curl_netrc_cleanup(&तथ्य->state.netrc);
  वापस परिणाम;
}

CURLMcode Curl_multi_remove_handle(काष्ठा Curl_multi *multi,
                                   काष्ठा Curl_easy *तथ्य)
{
  CURLMcode mresult;
  bool premature;
  uint32_t mid;

  /* Prevent users from trying to remove same easy handle more than once */
  अगर(!तथ्य->multi)
    वापस CURLM_OK; /* it is already removed so let's say it is fine! */

  /* Prevent users from trying to remove an easy handle from the wrong multi */
  अगर(तथ्य->multi != multi)
    वापस CURLM_BAD_EASY_HANDLE;

  अगर(तथ्य->mid == UINT32_MAX) {
    DEBUGASSERT(0);
    वापस CURLM_INTERNAL_ERROR;
  }
  अगर(Curl_uint32_tbl_get(&multi->xfers, तथ्य->mid) != तथ्य) {
    DEBUGASSERT(0);
    वापस CURLM_INTERNAL_ERROR;
  }

  premature = (तथ्य->mstate < MSTATE_COMPLETED);

  अगर(तथ्य->conn) {
    /* If the 'state' is not INIT or COMPLETED, we might need to do something
       nice to put the easy_handle in a good known state when this returns. */
    अगर(premature && (तथ्य->mstate > MSTATE_DO))
      streamclose(तथ्य->conn);

    /* multi_done() clears the association between the easy handle and the
       connection.
       Note that this ignores the return code because there is
       nothing really useful to do with it anyway! */
    (व्योम)multi_done(तथ्य, तथ्य->परिणाम, premature);
  }

  /* The timer must be shut down before data->multi is set to NULL, else
     data's splaynode would remain in the splay tree after curl_easy_cleanup is
     called. Do it after multi_done() in case that sets another time! */
  Curl_expire_clear_all(तथ्य);

  /* In MSGSENT, it was deducted from `multi->xfers_alive` already. */
  अगर(तथ्य->mstate != MSTATE_MSGSENT)
    --multi->xfers_alive;

  अगर(तथ्य->state.really_alive) {
    तथ्य->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    अगर(!multi->xfers_really_alive)
      (व्योम)multi_assess_wakeup(multi);
  }

  Curl_wildcard_dtor(&तथ्य->wildcard);

  तथ्य->mstate = MSTATE_COMPLETED;

  /* Remove the association between the connection and the handle */
  Curl_detach_connection(तथ्य);

  /* Tell event handling that this transfer is definitely going away */
  Curl_multi_ev_xfer_done(multi, तथ्य);

  अगर(तथ्य->set.connect_only) {
    अगर(तथ्य->multi_easy) {
      अगर(तथ्य->state.last_conn_id != -1) {
        /* Mark any connect-only connection for closure */
        काष्ठा connectdata *conn;
        (व्योम)Curl_getconnectinfo(तथ्य, &conn);
        अगर(conn && conn->bits.connect_only)
          connclose(conn);
      }
    }
    अन्यथा {
      /* This removes a handle that was part the multi interface that used
         CONNECT_ONLY, that connection is now left alive but since this handle
         has bits.close set nothing can use that connection anymore and it is
         forbidden from reuse. This easy handle cannot find the connection
         anymore once removed from the multi handle

         Better close the connection here, at once. */
      काष्ठा connectdata *conn;
      (व्योम)Curl_getconnectinfo(तथ्य, &conn);
      अगर(conn)
        Curl_conn_close(तथ्य, conn, TRUE);
    }
  }

#अगर_घोषित USE_LIBPSL
  /* Remove the PSL association. */
  अगर(तथ्य->psl == &multi->psl)
    तथ्य->psl = शून्य;
#पूर्ण_अगर

  /* clear the association to this multi handle */
  mid = तथ्य->mid;
  DEBUGASSERT(Curl_uint32_tbl_contains(&multi->xfers, mid));
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  Curl_uint32_bset_remove(&multi->process, mid);
  Curl_uint32_bset_remove(&multi->dirty, mid);
  Curl_uint32_bset_remove(&multi->pending, mid);
  Curl_uint32_bset_remove(&multi->msgsent, mid);
  तथ्य->multi = शून्य;
  तथ्य->mid = UINT32_MAX;
  तथ्य->master_mid = UINT32_MAX;

  /* A pending transfer *might* be able to run now. */
  multi_schedule_pending(multi);
  mresult = Curl_update_timer(multi);
  अगर(mresult)
    वापस mresult;

  mresult = multi_assess_wakeup(multi);
  अगर(mresult) {
    failf(तथ्य, "error enabling wakeup listening: %d", mresult);
    वापस mresult;
  }

  CURL_TRC_M(तथ्य, "removed from multi, mid=%u, running=%u, total=%u",
             mid, multi_xfers_running(multi),
             Curl_uint32_tbl_गिनती(&multi->xfers));
  वापस CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *curl)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_remove_handle, &mresult)) {
    काष्ठा Curl_easy *तथ्य = curl;
    अगर(!GOOD_EASY_HANDLE(तथ्य) || तथ्य->state.internal)
      mresult = CURLM_BAD_EASY_HANDLE;
    अन्यथा
      mresult = Curl_multi_remove_handle(m, तथ्य);
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

/* Return TRUE if the application asked for multiplexing */
bool Curl_multiplex_wanted(स्थिर काष्ठा Curl_multi *multi)
{
  वापस multi && multi->multiplexing;
}

/*
 * Curl_detach_connection() removes the given transfer from the connection.
 *
 * This is the only function that should clear data->conn. This will
 * occasionally be called with the data->conn pointer already cleared.
 */
व्योम Curl_detach_connection(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  अगर(conn) {
    /* this should never happen, prevent underflow */
    DEBUGASSERT(conn->attached_xfers);
    अगर(conn->attached_xfers) {
      conn->attached_xfers--;
      अगर(!conn->attached_xfers)
        conn->attached_multi = शून्य;
    }
  }
  तथ्य->conn = शून्य;
}

/*
 * Curl_attach_connection() attaches this transfer to this connection.
 *
 * This is the only function that should assign data->conn.
 * `matched == TRUE` means the transfer's properties match this
 * connection and it is not a temporary attach for maintenance.
 */
व्योम Curl_attach_connection(काष्ठा Curl_easy *तथ्य,
                            काष्ठा connectdata *conn,
                            bool matched)
{
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(!तथ्य->conn);
  DEBUGASSERT(conn);
  DEBUGASSERT(conn->attached_xfers < UINT32_MAX);
  तथ्य->conn = conn;
  अगर(matched) {
    तथ्य->state.last_conn_id = conn->connection_id;
    तथ्य->state.last_cpid = conn->cpid;
  }
  अन्यथा
    DEBUGASSERT(!तथ्य->mid); /* admin handle */
  conn->attached_xfers++;
  /* all attached transfers must be from the same multi */
  अगर(!conn->attached_multi)
    conn->attached_multi = तथ्य->multi;
  DEBUGASSERT(conn->attached_multi == तथ्य->multi);

  अगर(conn->scheme && conn->scheme->run->attach)
    conn->scheme->run->attach(तथ्य, conn);
}

/* adjust pollset for rate limits/pauses */
अटल CURLcode multi_adjust_pollset(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा easy_pollset *ps)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(ps->n) {
    bool send_blocked, recv_blocked;

    recv_blocked = (Curl_rlimit_avail(&तथ्य->progress.dl.rlimit, शून्य) <= 0);
    send_blocked = (Curl_rlimit_avail(&तथ्य->progress.ul.rlimit, शून्य) <= 0);
    अगर(send_blocked || recv_blocked) {
      पूर्णांक i;
      क्रम(i = 0; i <= SECONDARYSOCKET; ++i) {
        curl_socket_t sock = तथ्य->conn->sock[i];
        अगर(sock == CURL_SOCKET_BAD)
          जारी;
        अगर(recv_blocked && Curl_pollset_want_recv(तथ्य, ps, sock)) {
          परिणाम = Curl_pollset_remove_in(तथ्य, ps, sock);
          अगर(परिणाम)
            अवरोध;
        }
        अगर(send_blocked && Curl_pollset_want_send(तथ्य, ps, sock)) {
          परिणाम = Curl_pollset_remove_out(तथ्य, ps, sock);
          अगर(परिणाम)
            अवरोध;
        }
      }
    }

    /* Not blocked and wanting to receive. If there is data pending
     * in the connection filters, make transfer run again. */
    अगर(!recv_blocked &&
       ((Curl_pollset_want_recv(तथ्य, ps, तथ्य->conn->sock[FIRSTSOCKET]) &&
         Curl_conn_तथ्य_pending(तथ्य, FIRSTSOCKET)) ||
        (Curl_pollset_want_recv(तथ्य, ps, तथ्य->conn->sock[SECONDARYSOCKET]) &&
         Curl_conn_तथ्य_pending(तथ्य, SECONDARYSOCKET)))) {
      CURL_TRC_M(तथ्य, "pollset[] has POLLIN, but there is still "
                 "buffered input -> mark as dirty");
      Curl_multi_mark_dirty(तथ्य);
    }
  }
  वापस परिणाम;
}

अटल CURLcode mstate_connecting_pollset(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा easy_pollset *ps)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  curl_socket_t sockfd;
  CURLcode परिणाम = CURLE_OK;

  अगर(Curl_xfer_recv_is_paused(तथ्य))
    वापस CURLE_OK;
  /* If a socket is set, receiving is default. If the socket
   * has not been determined yet (eyeballing), always ask the
   * connection filters for what to monitor. */
  sockfd = Curl_conn_get_first_socket(तथ्य);
  अगर(sockfd != CURL_SOCKET_BAD) {
    परिणाम = Curl_pollset_change(तथ्य, ps, sockfd, CURL_POLL_IN, 0);
    अगर(!परिणाम)
      परिणाम = multi_adjust_pollset(तथ्य, ps);
  }
  अगर(!परिणाम)
    परिणाम = Curl_conn_adjust_pollset(तथ्य, conn, ps);
  वापस परिणाम;
}

अटल CURLcode mstate_protocol_pollset(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा easy_pollset *ps)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम = CURLE_OK;

  अगर(conn->scheme->run->proto_pollset)
    परिणाम = conn->scheme->run->proto_pollset(तथ्य, ps);
  अन्यथा {
    curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
    अगर(sockfd != CURL_SOCKET_BAD) {
      /* Default is to wait to something from the server */
      परिणाम = Curl_pollset_change(तथ्य, ps, sockfd, CURL_POLL_IN, 0);
    }
  }
  अगर(!परिणाम)
    परिणाम = multi_adjust_pollset(तथ्य, ps);
  अगर(!परिणाम)
    परिणाम = Curl_conn_adjust_pollset(तथ्य, conn, ps);
  वापस परिणाम;
}

अटल CURLcode mstate_do_pollset(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा easy_pollset *ps)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम = CURLE_OK;

  अगर(conn->scheme->run->doing_pollset)
    परिणाम = conn->scheme->run->doing_pollset(तथ्य, ps);
  अन्यथा अगर(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    परिणाम = Curl_pollset_add_out(तथ्य, ps, conn->sock[conn->send_idx]);
  }
  अगर(!परिणाम)
    परिणाम = multi_adjust_pollset(तथ्य, ps);
  अगर(!परिणाम)
    परिणाम = Curl_conn_adjust_pollset(तथ्य, conn, ps);
  वापस परिणाम;
}

अटल CURLcode mstate_domore_pollset(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा easy_pollset *ps)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम = CURLE_OK;

  अगर(conn->scheme->run->domore_pollset)
    परिणाम = conn->scheme->run->domore_pollset(तथ्य, ps);
  अन्यथा अगर(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    परिणाम = Curl_pollset_add_out(तथ्य, ps, conn->sock[conn->send_idx]);
  }
  अगर(!परिणाम)
    परिणाम = multi_adjust_pollset(तथ्य, ps);
  अगर(!परिणाम)
    परिणाम = Curl_conn_adjust_pollset(तथ्य, conn, ps);
  वापस परिणाम;
}

अटल CURLcode mstate_perform_pollset(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा easy_pollset *ps)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम = CURLE_OK;

  अगर(conn->scheme->run->perform_pollset)
    परिणाम = conn->scheme->run->perform_pollset(तथ्य, ps);
  अन्यथा {
    /* Default is to obey the request flags for send/recv */
    अगर(Curl_req_want_recv(तथ्य) && CONN_SOCK_IDX_VALID(conn->recv_idx)) {
      परिणाम = Curl_pollset_add_in(तथ्य, ps, conn->sock[conn->recv_idx]);
    }
    अगर(!परिणाम && Curl_req_want_send(तथ्य) &&
       CONN_SOCK_IDX_VALID(conn->send_idx)) {
      परिणाम = Curl_pollset_add_out(तथ्य, ps, conn->sock[conn->send_idx]);
    }
  }
  अगर(!परिणाम)
    परिणाम = multi_adjust_pollset(तथ्य, ps);
  अगर(!परिणाम)
    परिणाम = Curl_conn_adjust_pollset(तथ्य, conn, ps);
  वापस परिणाम;
}

#अगर_घोषित CURLVERBOSE
अटल माप_प्रकार multi_timeouts_गिनती(काष्ठा expire_timers *timeouts)
{
  माप_प्रकार n = 0;
  uint8_t eid = timeouts->first;
  क्रम(; eid < EXPIRE_LAST; eid = timeouts->अग्र[eid])
    ++n;
  वापस n;
}
#पूर्ण_अगर

/* Initializes `poll_set` with the current socket poll actions needed
 * for transfer `data`. */
CURLMcode Curl_multi_pollset(काष्ठा Curl_easy *तथ्य,
                             काष्ठा easy_pollset *ps)
{
  CURLcode परिणाम = CURLE_OK;

  Curl_pollset_reset(ps);
#अगर_घोषित ENABLE_INTERNAL_WAKEUP
  /* The admin handle always listens on the wakeup socket when there
   * are transfers alive. */
  अगर(तथ्य->multi && (तथ्य == तथ्य->multi->admin) &&
     तथ्य->multi->xfers_really_alive) {
    CURL_TRC_M(तथ्य, "adding wakeup, %u xfers really alive",
               तथ्य->multi->xfers_really_alive);
    परिणाम = Curl_pollset_add_in(तथ्य, ps, तथ्य->multi->wakeup_internal[0]);
  }
#पूर्ण_अगर
  /* If the transfer has no connection, this is fine. Happens when
     called via curl_multi_remove_handle() => Curl_multi_ev_assess() =>
     Curl_multi_pollset(). */
  अगर(!परिणाम && तथ्य->conn) {
    चयन(तथ्य->mstate) {
    हाल MSTATE_INIT:
    हाल MSTATE_PENDING:
    हाल MSTATE_SETUP:
    हाल MSTATE_CONNECT:
      /* nothing to poll for yet */
      अवरोध;

    हाल MSTATE_CONNECTING:
      परिणाम = mstate_connecting_pollset(तथ्य, ps);
      अवरोध;

    हाल MSTATE_PROTOCONNECT:
    हाल MSTATE_PROTOCONNECTING:
      परिणाम = mstate_protocol_pollset(तथ्य, ps);
      अवरोध;

    हाल MSTATE_DO:
    हाल MSTATE_DOING:
      परिणाम = mstate_do_pollset(तथ्य, ps);
      अवरोध;

    हाल MSTATE_DOING_MORE:
      परिणाम = mstate_domore_pollset(तथ्य, ps);
      अवरोध;

    हाल MSTATE_DID: /* same as PERFORMING in regard to polling */
    हाल MSTATE_PERFORMING:
      परिणाम = mstate_perform_pollset(तथ्य, ps);
      अवरोध;

    हाल MSTATE_RATELIMITING:
      /* we need to let time pass, ignore socket(s) */
      अवरोध;

    हाल MSTATE_DONE:
    हाल MSTATE_COMPLETED:
    हाल MSTATE_MSGSENT:
      /* nothing more to poll for */
      अवरोध;

    शेष:
      failf(तथ्य, "multi_getsock: unexpected multi state %d",
            (पूर्णांक)तथ्य->mstate);
      DEBUGASSERT(0);
      अवरोध;
    }
  }

  अगर(परिणाम) {
    अगर(परिणाम == CURLE_OUT_OF_MEMORY)
      वापस CURLM_OUT_OF_MEMORY;
    failf(तथ्य, "error determining pollset: %d", (पूर्णांक)परिणाम);
    वापस CURLM_INTERNAL_ERROR;
  }

#अगर_घोषित CURLVERBOSE
  अगर(CURL_TRC_M_is_verbose(तथ्य)) {
    माप_प्रकार timeout_गिनती = multi_timeouts_गिनती(&तथ्य->state.timeouts);
    चयन(ps->n) {
    हाल 0:
      CURL_TRC_M(तथ्य, "pollset[], timeouts=%zu, paused %d/%d (r/w)",
                 timeout_गिनती,
                 Curl_xfer_send_is_paused(तथ्य),
                 Curl_xfer_recv_is_paused(तथ्य));
      अवरोध;
    हाल 1:
      CURL_TRC_M(तथ्य, "pollset[fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_OUT) ? "OUT" : "",
                 timeout_गिनती);
      अवरोध;
    हाल 2:
      CURL_TRC_M(तथ्य, "pollset[fd=%" FMT_SOCKET_T " %s%s, "
                 "fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_OUT) ? "OUT" : "",
                 ps->sockets[1],
                 (ps->actions[1] & CURL_POLL_IN) ? "IN" : "",
                 (ps->actions[1] & CURL_POLL_OUT) ? "OUT" : "",
                 timeout_गिनती);
      अवरोध;
    शेष:
      CURL_TRC_M(तथ्य, "pollset[fds=%u], timeouts=%zu", ps->n, timeout_गिनती);
      अवरोध;
    }
    CURL_TRC_EASY_TIMERS(तथ्य);
  }
#पूर्ण_अगर

  वापस CURLM_OK;
}

CURLMcode curl_multi_fdset(CURLM *m,
                           fd_set *read_fd_set, fd_set *write_fd_set,
                           fd_set *exc_fd_set, पूर्णांक *max_fd)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_fdset, &mresult)) {
    /* Scan through all the easy handles to get the file descriptors set.
       Some easy handles may not have connected to the remote host yet,
       and then we must make sure that is done. */
    काष्ठा Curl_multi *multi = m;
    काष्ठा easy_pollset ps;
    पूर्णांक this_max_fd = -1;
    अचिन्हित पूर्णांक i;
    uint32_t mid;
    (व्योम)exc_fd_set;

    Curl_pollset_init(&ps);
    अगर(Curl_uint32_bset_first(&multi->process, &mid)) {
      करो {
        काष्ठा Curl_easy *तथ्य = Curl_multi_get_easy(multi, mid);

        अगर(!तथ्य) {
          DEBUGASSERT(0);
          जारी;
        }

        Curl_multi_pollset(तथ्य, &ps);
        क्रम(i = 0; i < ps.n; i++) {
          अगर(!FDSET_SOCK(ps.sockets[i]))
            /* pretend it does not exist */
            जारी;
          अगर(ps.actions[i] & CURL_POLL_IN)
            FD_SET(ps.sockets[i], read_fd_set);
          अगर(ps.actions[i] & CURL_POLL_OUT)
            FD_SET(ps.sockets[i], write_fd_set);
          अगर((पूर्णांक)ps.sockets[i] > this_max_fd)
            this_max_fd = (पूर्णांक)ps.sockets[i];
        }
      } जबतक(Curl_uint32_bset_अग्र(&multi->process, mid, &mid));
    }

    Curl_cshutdn_setfds(&multi->cshutdn, multi->admin,
                        read_fd_set, write_fd_set, &this_max_fd);

    *max_fd = this_max_fd;
    Curl_pollset_cleanup(&ps);

    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

CURLMcode curl_multi_waitfds(CURLM *m,
                             काष्ठा curl_waitfd *ufds,
                             अचिन्हित पूर्णांक आकार,
                             अचिन्हित पूर्णांक *fd_गिनती)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_waitfds, &mresult)) {
    काष्ठा Curl_waitfds cwfds;
    काष्ठा Curl_multi *multi = m;
    काष्ठा easy_pollset ps;
    अचिन्हित पूर्णांक need = 0;
    uint32_t mid;

    अगर(!ufds && (आकार || !fd_गिनती)) {
      mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      जाओ out;
    }

    Curl_pollset_init(&ps);
    Curl_waitfds_init(&cwfds, ufds, आकार);
    mresult = CURLM_OK;
    अगर(Curl_uint32_bset_first(&multi->process, &mid)) {
      करो {
        काष्ठा Curl_easy *तथ्य = Curl_multi_get_easy(multi, mid);
        अगर(!तथ्य) {
          DEBUGASSERT(0);
          Curl_uint32_bset_remove(&multi->process, mid);
          Curl_uint32_bset_remove(&multi->dirty, mid);
          जारी;
        }
        Curl_multi_pollset(तथ्य, &ps);
        need += Curl_waitfds_add_ps(&cwfds, &ps);
      } जबतक(Curl_uint32_bset_अग्र(&multi->process, mid, &mid));
    }

    need += Curl_cshutdn_add_waitfds(&multi->cshutdn, multi->admin, &cwfds);

    अगर(need != cwfds.n && ufds)
      mresult = CURLM_OUT_OF_MEMORY;

    अगर(fd_गिनती)
      *fd_गिनती = need;
    Curl_pollset_cleanup(&ps);
  }
out:
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

#अगर_घोषित USE_WINSOCK
/* Reset FD_WRITE for TCP sockets. Nothing is actually sent. UDP sockets cannot
 * be reset this way because an empty datagram would be sent. #9203
 *
 * "On Windows the internal state of FD_WRITE as returned from
 * WSAEnumNetworkEvents is only reset after successful send()."
 */
अटल व्योम reset_socket_fdwrite(curl_socket_t s)
{
  पूर्णांक t;
  पूर्णांक l = (पूर्णांक)माप(t);
  अगर(!getsockopt(s, SOL_SOCKET, SO_TYPE, (अक्षर *)&t, &l) && t == SOCK_STREAM)
    swrite(s, शून्य, 0);
}

अटल CURLMcode multi_winsock_select(काष्ठा Curl_multi *multi,
                                      काष्ठा curl_pollfds *cpfds,
                                      अचिन्हित पूर्णांक curl_nfds,
                                      काष्ठा curl_waitfd extra_fds[],
                                      अचिन्हित पूर्णांक extra_nfds,
                                      पूर्णांक timeout_ms,
                                      bool extrawait,
                                      पूर्णांक *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  WSANETWORKEVENTS wsa_events;
  पूर्णांक nevents = 0;
  माप_प्रकार i;

  DEBUGASSERT(multi->wsa_event != WSA_INVALID_EVENT);

  /* Set the WSA events based on the collected pollds */
  क्रम(i = 0; i < cpfds->n; i++) {
    दीर्घ mask = 0;
    अगर(cpfds->pfds[i].events & POLLIN)
      mask |= FD_READ | FD_ACCEPT | FD_CLOSE;
    अगर(cpfds->pfds[i].events & POLLPRI)
      mask |= FD_OOB;
    अगर(cpfds->pfds[i].events & POLLOUT) {
      mask |= FD_WRITE | FD_CONNECT | FD_CLOSE;
      reset_socket_fdwrite(cpfds->pfds[i].fd);
    }
    अगर(mask && WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, mask)) {
      mresult = CURLM_OUT_OF_MEMORY;
      जाओ out;
    }
  }

  अगर(cpfds->n || extrawait) {
    पूर्णांक pollrc = 0;

    अगर(cpfds->n) {         /* pre-check with Winsock */
      pollrc = Curl_poll(cpfds->pfds, cpfds->n, 0);
      अगर(pollrc < 0) {
        mresult = CURLM_UNRECOVERABLE_POLL;
        जाओ out;
      }
      nevents = pollrc;
    }

    अगर(!nevents) {
      /* now wait... if not ready during the pre-check (pollrc == 0) */
      WSAWaitForMultipleEvents(1, &multi->wsa_event, FALSE, (DWORD)timeout_ms,
                               FALSE);
    }

    /* With Winsock, we have to run the following section unconditionally
       to call WSAEventSelect(fd, event, 0) on all the sockets */
    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    क्रम(i = 0; i < extra_nfds; i++) {
      अचिन्हित लघु mask = 0;
      curl_socket_t s = extra_fds[i].fd;

      wsa_events.lNetworkEvents = 0;
      अगर(WSAEnumNetworkEvents(s, शून्य, &wsa_events) == 0) {
        अगर(wsa_events.lNetworkEvents & (FD_READ | FD_ACCEPT | FD_CLOSE))
          mask |= CURL_WAIT_POLLIN;
        अगर(wsa_events.lNetworkEvents & (FD_WRITE | FD_CONNECT | FD_CLOSE))
          mask |= CURL_WAIT_POLLOUT;
        अगर(wsa_events.lNetworkEvents & FD_OOB)
          mask |= CURL_WAIT_POLLPRI;
        अगर(!pollrc && wsa_events.lNetworkEvents)
          nevents++;
      }
      WSAEventSelect(s, multi->wsa_event, 0);
      अगर(!pollrc) {
        extra_fds[i].revents = (लघु)mask;
        जारी;
      }
      अन्यथा {
        अचिन्हित r = (अचिन्हित)cpfds->pfds[curl_nfds + i].revents;
        अगर(r & POLLIN)
          mask |= CURL_WAIT_POLLIN;
        अगर(r & POLLOUT)
          mask |= CURL_WAIT_POLLOUT;
        अगर(r & POLLPRI)
          mask |= CURL_WAIT_POLLPRI;
        extra_fds[i].revents = (लघु)mask;
      }
    }

    /* Count up all our own sockets that had activity,
       and remove them from the event. */
    क्रम(i = 0; i < curl_nfds; ++i) {
      wsa_events.lNetworkEvents = 0;
      अगर(WSAEnumNetworkEvents(cpfds->pfds[i].fd, शून्य, &wsa_events) == 0) {
        अगर(!pollrc && wsa_events.lNetworkEvents)
          nevents++;
      }
      WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, 0);
    }
    WSAResetEvent(multi->wsa_event);
  }

out:
  *pnevents = nevents;
  वापस mresult;
}

#अन्यथा /* USE_WINSOCK */

अटल CURLMcode multi_posix_poll(काष्ठा Curl_multi *multi,
                                  काष्ठा curl_pollfds *cpfds,
                                  अचिन्हित पूर्णांक curl_nfds,
                                  काष्ठा curl_waitfd extra_fds[],
                                  अचिन्हित पूर्णांक extra_nfds,
                                  पूर्णांक timeout_ms,
                                  bool extrawait,
                                  पूर्णांक *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  पूर्णांक nevents = 0;
  माप_प्रकार i;

  (व्योम)multi;
  अगर(cpfds->n) {
    पूर्णांक pollrc = Curl_poll(cpfds->pfds, cpfds->n, timeout_ms); /* wait... */
    अगर(pollrc < 0) {
      mresult = CURLM_UNRECOVERABLE_POLL;
      जाओ out;
    }
    nevents = pollrc;

    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    क्रम(i = 0; i < extra_nfds; i++) {
      अचिन्हित r = (अचिन्हित)cpfds->pfds[curl_nfds + i].revents;
      अचिन्हित लघु mask = 0;
      अगर(r & POLLIN)
        mask |= CURL_WAIT_POLLIN;
      अगर(r & POLLOUT)
        mask |= CURL_WAIT_POLLOUT;
      अगर(r & POLLPRI)
        mask |= CURL_WAIT_POLLPRI;
      extra_fds[i].revents = (लघु)mask;
    }
  }
  अन्यथा अगर(extrawait) {
    /* No fds to poll, but asked to obey timeout_ms anyway. We cannot
     * use Curl_poll() as it, on some platforms, returns immediately
     * without fds. */
    curlx_wait_ms(timeout_ms);
  }

out:
  *pnevents = nevents;
  वापस mresult;
}

#पूर्ण_अगर /* !USE_WINSOCK */

#घोषणा अंक_POLLS_ON_ढेर 10

अटल CURLMcode multi_wait(काष्ठा Curl_multi *multi,
                            काष्ठा curl_waitfd extra_fds[],
                            अचिन्हित पूर्णांक extra_nfds,
                            पूर्णांक timeout_ms,
                            पूर्णांक *ret,
                            bool extrawait)  /* when no socket, wait */
{
  माप_प्रकार i;
  पूर्णांक timeout_internal;
  पूर्णांक nevents = 0;
  काष्ठा easy_pollset ps;
  काष्ठा pollfd a_few_on_ढेर[अंक_POLLS_ON_ढेर];
  काष्ठा curl_pollfds cpfds;
  अचिन्हित पूर्णांक curl_nfds = 0; /* how many pfds are for curl transfers */
  काष्ठा Curl_easy *तथ्य = शून्य;
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;
#अगर_घोषित ENABLE_WAKEUP
  पूर्णांक wakeup_idx = -1;
#पूर्ण_अगर

  अगर(timeout_ms < 0)
    वापस CURLM_BAD_FUNCTION_ARGUMENT;

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_ढेर, अंक_POLLS_ON_ढेर);

  /* Add the curl handles to our pollfds first */
  अगर(Curl_uint32_bset_first(&multi->process, &mid)) {
    करो {
      तथ्य = Curl_multi_get_easy(multi, mid);
      अगर(!तथ्य) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        जारी;
      }
      Curl_multi_pollset(तथ्य, &ps);
      अगर(Curl_pollfds_add_ps(&cpfds, &ps)) {
        mresult = CURLM_OUT_OF_MEMORY;
        जाओ out;
      }
    } जबतक(Curl_uint32_bset_अग्र(&multi->process, mid, &mid));
  }

  अगर(Curl_cshutdn_add_pollfds(&multi->cshutdn, multi->admin, &cpfds)) {
    mresult = CURLM_OUT_OF_MEMORY;
    जाओ out;
  }

#अगर_घोषित ENABLE_WAKEUP
  /* If `extrawait` is TRUE *or* we have `extra_fds`to poll *or* we
   * have transfer sockets to poll, we obey `timeout_ms`.
   * Then we need to also monitor the multi's wakeup
   * socket to catch calls to `curl_multi_wakeup()` during the wait. */
  अगर(extrawait || cpfds.n || extra_nfds) {
    wakeup_idx = cpfds.n;
    अगर(Curl_pollfds_add_sock(&cpfds, multi->wakeup_pair[0], POLLIN)) {
      mresult = CURLM_OUT_OF_MEMORY;
      जाओ out;
    }
  }
#पूर्ण_अगर

  curl_nfds = cpfds.n; /* what curl internally uses in cpfds */
  /* Add external file descriptions from poll-like struct curl_waitfd */
  क्रम(i = 0; i < extra_nfds; i++) {
    अचिन्हित लघु events = 0;
    अगर(extra_fds[i].events & CURL_WAIT_POLLIN)
      events |= POLLIN;
    अगर(extra_fds[i].events & CURL_WAIT_POLLPRI)
      events |= POLLPRI;
    अगर(extra_fds[i].events & CURL_WAIT_POLLOUT)
      events |= POLLOUT;
    अगर(Curl_pollfds_add_sock(&cpfds, extra_fds[i].fd, events)) {
      mresult = CURLM_OUT_OF_MEMORY;
      जाओ out;
    }
  }

  /* We check the internal timeout *AFTER* we collected all sockets to
   * poll. Collecting the sockets may install new timers by protocols
   * and connection filters.
   * Use the shorter one of the internal and the caller requested timeout.
   * If we are called with `!extrawait` and multi_timeout() reports no
   * timeouts exist, do not wait. */
  multi_timeout(multi, शून्य, &timeout_internal);
  अगर((timeout_internal >= 0) && (timeout_internal < timeout_ms))
    timeout_ms = timeout_internal;

  अगर(तथ्य)
    CURL_TRC_M(तथ्य, "multi_wait(fds=%u, timeout=%d) tinternal=%d",
               cpfds.n, timeout_ms, timeout_internal);

#अगर_घोषित USE_WINSOCK
  mresult = multi_winsock_select(multi, &cpfds, curl_nfds,
                                 extra_fds, extra_nfds,
                                 timeout_ms, extrawait, &nevents);
#अन्यथा
  mresult = multi_posix_poll(multi, &cpfds, curl_nfds,
                             extra_fds, extra_nfds,
                             timeout_ms, extrawait, &nevents);
#पूर्ण_अगर

#अगर_घोषित ENABLE_WAKEUP
    अगर(nevents && (wakeup_idx >= 0)) {
      अगर(cpfds.pfds[wakeup_idx].revents & POLLIN) {
        (व्योम)Curl_wakeup_consume(multi->wakeup_pair, TRUE);
        /* do not count the wakeup socket into the returned value */
        nevents--;
      }
    }
#पूर्ण_अगर

out:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  अगर(ret)
    *ret = nevents;
  वापस mresult;
}

CURLMcode curl_multi_wait(CURLM *m,
                          काष्ठा curl_waitfd extra_fds[],
                          अचिन्हित पूर्णांक extra_nfds,
                          पूर्णांक timeout_ms,
                          पूर्णांक *ret)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_wait, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, FALSE);
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

CURLMcode curl_multi_poll(CURLM *m,
                          काष्ठा curl_waitfd extra_fds[],
                          अचिन्हित पूर्णांक extra_nfds,
                          पूर्णांक timeout_ms,
                          पूर्णांक *ret)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_poll, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, TRUE);
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

CURLMcode curl_multi_wakeup(CURLM *m)
{
  /* this function is usually called from another thread,
     it has to be careful only to access parts of the
     Curl_multi struct that are constant */
  काष्ठा Curl_multi *multi = m;
  CURLMcode mresult = CURLM_WAKEUP_FAILURE;

  /* GOOD_MULTI_HANDLE can be safely called */
  अगर(!GOOD_MULTI_HANDLE(multi))
    वापस CURLM_BAD_HANDLE;

#अगर_घोषित ENABLE_WAKEUP
  /* the wakeup_pair variable is only written during init and cleanup,
     making it safe to access from another thread after the init part
     and before cleanup */
  अगर(!Curl_wakeup_signal(multi->wakeup_pair))
    mresult = CURLM_OK;
#पूर्ण_अगर
#अगर_घोषित USE_WINSOCK
  अगर(WSASetEvent(multi->wsa_event))
    mresult = CURLM_OK;
#पूर्ण_अगर
  वापस mresult;
}

#अगर_घोषित ENABLE_INTERNAL_WAKEUP
व्योम Curl_multi_wakeup_internal(काष्ठा Curl_multi *multi)
{
  /* This is expected to be invocable from another thread which
   * does NOT outlive the multi handle. Check for sanity. */
  अगर(GOOD_MULTI_HANDLE(multi))
    Curl_wakeup_signal(multi->wakeup_internal);
  अन्यथा
    DEBUGASSERT(0);
}
#पूर्ण_अगर

/*
 * multi_ischanged() is called
 *
 * Returns TRUE/FALSE whether the state is changed to trigger a CONNECT_PEND
 * => CONNECT action.
 *
 * Set 'clear' to TRUE to have it also clear the state variable.
 */
अटल bool multi_ischanged(काष्ठा Curl_multi *multi, bool clear)
{
  bool retval = FALSE;
  DEBUGASSERT(multi);
  अगर(multi) {
    retval = (bool)multi->recheckstate;
    अगर(clear)
      multi->recheckstate = FALSE;
  }
  वापस retval;
}

/*
 * Curl_multi_connchanged() is called to tell that there is a connection in
 * this multi handle that has changed state (multiplexing become possible, the
 * number of allowed streams changed or similar), and a subsequent use of this
 * multi handle should move CONNECT_PEND handles back to CONNECT to have them
 * retry.
 */
व्योम Curl_multi_connchanged(काष्ठा Curl_multi *multi)
{
  /* May be invoked for a `data->multi` when no multi is present. */
  अगर(multi)
    multi->recheckstate = TRUE;
}

CURLMcode Curl_multi_add_perform(काष्ठा Curl_multi *multi,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा connectdata *conn)
{
  CURLMcode mresult;

  mresult = Curl_multi_add_handle(multi, तथ्य);
  अगर(!mresult) {
    CURLcode परिणाम;

    /* pass in NULL for 'conn' here since we do not want to init the
       connection, only this transfer */
    परिणाम = Curl_init_transfer(तथ्य, शून्य);
    अगर(परिणाम) {
      Curl_multi_remove_handle(multi, तथ्य);
      वापस CURLM_INTERNAL_ERROR;
    }

    /* take this handle to the perform state right away */
    multistate(तथ्य, MSTATE_PERFORMING);
    Curl_attach_connection(तथ्य, conn, TRUE);
    CURL_REQ_SET_RECV(तथ्य);
  }
  वापस mresult;
}

अटल CURLcode multi_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(conn->scheme);

  अगर(conn->scheme->run->do_it)
    परिणाम = conn->scheme->run->do_it(तथ्य, done);

  वापस परिणाम;
}

/*
 * multi_do_more() is called during the DO_MORE multi state. It is a second
 * stage DO state which (wrongly) was introduced to support FTP's second
 * connection.
 *
 * 'complete' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (to DOING state when there is more work to do)
 */

अटल CURLcode multi_do_more(काष्ठा Curl_easy *तथ्य, domore *complete)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;

  *complete = DOMORE_INCOMPLETE;

  अगर(conn->scheme->run->do_more)
    परिणाम = conn->scheme->run->do_more(तथ्य, complete);

  वापस परिणाम;
}

/*
 * Check whether a timeout occurred, and handle it if it did
 */
अटल bool multi_handle_timeout(काष्ठा Curl_easy *तथ्य,
                                 स्थिर काष्ठा curltime *pnow,
                                 bool *stream_error,
                                 CURLcode *परिणाम)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_timeleft_now_ms(तथ्य, pnow);
  अगर(timeout_ms < 0) {
    /* Handle timed out */
    timerid base_timer = Curl_is_connecting(तथ्य) ?
                         TIMER_STARTSINGLE : TIMER_STARTOP;
    timediff_t elapsed_ms = Curl_pgrs_since_ms(तथ्य, शून्य, base_timer);
    अगर(तथ्य->mstate == MSTATE_CONNECTING) {
      failf(तथ्य, "%s timed out after %" FMT_TIMEDIFF_T " milliseconds",
            तथ्य->conn->bits.dns_resolved ? "Connection" : "Resolving",
            elapsed_ms);
      /* keep what we know about a partly done connect, e.g. TCP done
       * but TLS handshake still ongoing */
      Curl_conn_report_failed(तथ्य, FIRSTSOCKET);
    }
    अन्यथा {
      काष्ठा SingleRequest *k = &तथ्य->req;
      अगर(k->आकार != -1) {
        failf(तथ्य, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount, k->आकार);
      }
      अन्यथा {
        failf(तथ्य, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount);
      }
    }
    *परिणाम = CURLE_OPERATION_TIMEDOUT;
    अगर(तथ्य->conn) {
      /* Force connection closed if the connection has indeed been used */
      अगर(तथ्य->mstate > MSTATE_DO) {
        streamclose(तथ्य->conn);
        *stream_error = TRUE;
      }
      (व्योम)multi_done(तथ्य, *परिणाम, TRUE);
    }
    वापस TRUE;
  }

  वापस FALSE;
}

/*
 * We are doing protocol-specific connecting and this is being called over and
 * over from the multi interface until the connection phase is done on
 * protocol layer.
 */

अटल CURLcode protocol_connecting(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;

  अगर(conn && conn->scheme->run->connecting) {
    *done = FALSE;
    परिणाम = conn->scheme->run->connecting(तथ्य, done);
  }
  अन्यथा
    *done = TRUE;

  वापस परिणाम;
}

/*
 * We are DOING this is being called over and over from the multi interface
 * until the DOING phase is done on protocol layer.
 */

अटल CURLcode protocol_doing(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा connectdata *conn = तथ्य->conn;

  अगर(conn && conn->scheme->run->doing) {
    *done = FALSE;
    परिणाम = conn->scheme->run->doing(तथ्य, done);
  }
  अन्यथा
    *done = TRUE;

  वापस परिणाम;
}

/*
 * We have discovered that the TCP connection has been successful, we can now
 * proceed with some action.
 *
 */
अटल CURLcode protocol_connect(काष्ठा Curl_easy *तथ्य, bool *protocol_done)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(conn);
  DEBUGASSERT(protocol_done);
  DEBUGASSERT(Curl_conn_is_connected(conn, FIRSTSOCKET));

  *protocol_done = FALSE;
  अगर(!conn->bits.protoconnstart) {
    अगर(conn->scheme->run->connect_it) {
      /* Call the protocol-specific connect function */
      परिणाम = conn->scheme->run->connect_it(तथ्य, protocol_done);
      अगर(परिणाम)
        वापस परिणाम;
    }
    conn->bits.protoconnstart = TRUE;
  }

  /* Unless this protocol does not have any protocol-connect callback, as
     then we know we are done. */
  अगर(!conn->scheme->run->connecting)
    *protocol_done = TRUE;
  वापस CURLE_OK;
}

/*
 * posttransfer() is called immediately after a transfer ends
 */
अटल व्योम multi_posttransfer(काष्ठा Curl_easy *तथ्य)
{
#अगर defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
  /* restore the signal handler for SIGPIPE before we get back */
  अगर(!तथ्य->set.no_signal)
    संकेत(SIGPIPE, तथ्य->state.पूर्व_signal);
#अन्यथा
  (व्योम)तथ्य;
#पूर्ण_अगर
}

/*
 * multi_follow() handles the URL redirect magic. Pass in the 'newurl' string
 * as given by the remote server and set up the new URL to request.
 *
 * This function DOES NOT FREE the given URL.
 */
अटल CURLcode multi_follow(काष्ठा Curl_easy *तथ्य,
                             स्थिर काष्ठा Curl_scheme *handler,
                             स्थिर अक्षर *newurl, /* the Location: string */
                             followtype type) /* see transfer.h */
{
  अगर(handler && handler->run->follow)
    वापस handler->run->follow(तथ्य, newurl, type);

  अगर(type == FOLLOW_RETRY)
    /* Retries are generic and do not require protocol-specific redirect
       handling. */
    वापस CURLE_OK;

  वापस CURLE_TOO_MANY_REDIRECTS;
}

अटल CURLcode mspeed_check(काष्ठा Curl_easy *तथ्य)
{
  अगर(Curl_rlimit_active(&तथ्य->progress.dl.rlimit) ||
     Curl_rlimit_active(&तथ्य->progress.ul.rlimit)) {
    /* check if our send/recv limits require idle waits */
    स्थिर काष्ठा curltime *pnow = Curl_pgrs_now(तथ्य);
    timediff_t recv_ms, send_ms;

    send_ms = Curl_rlimit_wait_ms(&तथ्य->progress.ul.rlimit, pnow);
    recv_ms = Curl_rlimit_wait_ms(&तथ्य->progress.dl.rlimit, pnow);

    अगर(send_ms || recv_ms) {
      अगर(तथ्य->mstate != MSTATE_RATELIMITING) {
        multistate(तथ्य, MSTATE_RATELIMITING);
      }
      Curl_expire_set(तथ्य, EXPIRE_TOOFAST, CURLMAX(send_ms, recv_ms), pnow);
      Curl_multi_clear_dirty(तथ्य);
      CURL_TRC_M(तथ्य, "[RLIMIT] waiting %" FMT_TIMEDIFF_T "ms",
                 CURLMAX(send_ms, recv_ms));
      वापस CURLE_AGAIN;
    }
    अन्यथा {
      /* when will the rate limits increase next? The transfer needs
       * to run again at that time or it may stall. */
      send_ms = Curl_rlimit_अग्र_step_ms(&तथ्य->progress.ul.rlimit, pnow);
      recv_ms = Curl_rlimit_अग्र_step_ms(&तथ्य->progress.dl.rlimit, pnow);
      अगर(send_ms || recv_ms) {
        timediff_t अग्र_ms = CURLMIN(send_ms, recv_ms);
        अगर(!अग्र_ms)
          अग्र_ms = CURLMAX(send_ms, recv_ms);
        Curl_expire_set(तथ्य, EXPIRE_TOOFAST, अग्र_ms, pnow);
        CURL_TRC_M(तथ्य, "[RLIMIT] next token update in %" FMT_TIMEDIFF_T "ms",
                   अग्र_ms);
      }
    }
  }

  अगर(तथ्य->mstate != MSTATE_PERFORMING) {
    CURL_TRC_M(तथ्य, "[RLIMIT] wait over, continue");
    multistate(तथ्य, MSTATE_PERFORMING);
  }
  वापस CURLE_OK;
}

अटल CURLMcode multistate_performing(काष्ठा Curl_easy *तथ्य,
                                       bool *stream_errorp,
                                       CURLcode *resultp)
{
  अक्षर *newurl = शून्य;
  bool retry = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode परिणाम = *resultp = CURLE_OK;
  *stream_errorp = FALSE;

  अगर(mspeed_check(तथ्य) == CURLE_AGAIN)
    वापस CURLM_OK;

  /* read/write data if it is ready to do so */
  परिणाम = Curl_sendrecv(तथ्य);

  अगर(तथ्य->req.done || (परिणाम == CURLE_RECV_ERROR)) {
    /* If CURLE_RECV_ERROR happens early enough, we assume it was a race
     * condition and the server closed the reused connection exactly when we
     * wanted to use it, so figure out if that is indeed the case.
     */
    CURLcode ret = Curl_retry_request(तथ्य, &newurl);
    अगर(!ret)
      retry = !!newurl;
    अन्यथा अगर(!परिणाम)
      परिणाम = ret;

    अगर(retry) {
      /* if we are to retry, set the result to OK and consider the
         request as done */
      परिणाम = CURLE_OK;
      तथ्य->req.done = TRUE;
    }
  }
#अगर_अघोषित CURL_DISABLE_HTTP
  अन्यथा अगर((परिणाम == CURLE_HTTP2_STREAM) &&
          Curl_h2_http_1_1_error(तथ्य)) {
    CURLcode ret = Curl_retry_request(तथ्य, &newurl);

    अगर(!ret) {
      infof(तथ्य, "Downgrades to HTTP/1.1");
      streamclose(तथ्य->conn);
      तथ्य->state.http_neg.wanted = CURL_HTTP_V1x;
      तथ्य->state.http_neg.allowed = CURL_HTTP_V1x;
      /* clear the error message bit too as we ignore the one we got */
      तथ्य->state.errorbuf = FALSE;
      अगर(!newurl)
        /* typically for HTTP_1_1_REQUIRED error on first flight */
        newurl = Curl_bufref_dup(&तथ्य->state.url);
      अगर(!newurl) {
        परिणाम = CURLE_OUT_OF_MEMORY;
      }
      अन्यथा {
        /* if we are to retry, set the result to OK and consider the request
          as done */
        retry = TRUE;
        परिणाम = CURLE_OK;
        तथ्य->req.done = TRUE;
      }
    }
    अन्यथा
      परिणाम = ret;
  }
#पूर्ण_अगर

  अगर(परिणाम) {
    /*
     * The transfer phase returned error, we mark the connection to get closed
     * to prevent being reused. This is because we cannot possibly know if the
     * connection is in a good shape or not now. Unless it is a protocol which
     * uses two "channels" like FTP, as then the error happened in the data
     * connection.
     */

    अगर(!(तथ्य->conn->scheme->flags & PROTOPT_DUAL) &&
       परिणाम != CURLE_HTTP2_STREAM)
      streamclose(तथ्य->conn);

    multi_posttransfer(तथ्य);
    multi_done(तथ्य, परिणाम, TRUE);
  }
  अन्यथा अगर(तथ्य->req.done && !Curl_cwriter_is_paused(तथ्य)) {
    स्थिर काष्ठा Curl_scheme *handler = तथ्य->conn->scheme;

    /* call this even if the readwrite function returned error */
    multi_posttransfer(तथ्य);

    /* When we follow redirects or is set to retry the connection, we must to
       go back to the CONNECT state */
    अगर(तथ्य->req.newurl || retry) {
      followtype follow = FOLLOW_NONE;
      अगर(!retry) {
        /* if the URL is a follow-location and not a retried request then
           figure out the URL here */
        curlx_free(newurl);
        newurl = तथ्य->req.newurl;
        तथ्य->req.newurl = शून्य;
        follow = FOLLOW_REDIR;
      }
      अन्यथा
        follow = FOLLOW_RETRY;
      (व्योम)multi_done(तथ्य, CURLE_OK, FALSE);
      /* multi_done() might return CURLE_GOT_NOTHING */
      परिणाम = multi_follow(तथ्य, handler, newurl, follow);
      अगर(!परिणाम) {
        multistate(तथ्य, MSTATE_SETUP);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    अन्यथा {
      /* after the transfer is done, go DONE */

      /* but first check to see if we got a location info even though we are
         not following redirects */
      अगर(तथ्य->req.location) {
        curlx_free(newurl);
        newurl = तथ्य->req.location;
        तथ्य->req.location = शून्य;
        परिणाम = multi_follow(तथ्य, handler, newurl, FOLLOW_FAKE);
        अगर(परिणाम) {
          *stream_errorp = TRUE;
          परिणाम = multi_done(तथ्य, परिणाम, TRUE);
        }
      }

      अगर(!परिणाम) {
        multistate(तथ्य, MSTATE_DONE);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
  }
  अन्यथा { /* not errored, not done */
    mspeed_check(तथ्य);
  }
  curlx_free(newurl);
  *resultp = परिणाम;
  वापस mresult;
}

अटल CURLMcode multistate_do(काष्ठा Curl_easy *तथ्य,
                               bool *stream_errorp,
                               CURLcode *resultp)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode परिणाम = CURLE_OK;
  अगर(तथ्य->set.fprereq) {
    काष्ठा Curl_mapi_guard guard;
    पूर्णांक prereq_rc;

    /* call the prerequest callback function */
    CURL_CBAPI_START(&guard, तथ्य, easy_fprereq);
    prereq_rc = तथ्य->set.fprereq(तथ्य->set.prereq_userp,
                                  तथ्य->info.primary.remote_ip,
                                  तथ्य->info.primary.local_ip,
                                  तथ्य->info.primary.remote_port,
                                  तथ्य->info.primary.local_port);
    CURL_CBAPI_END(&guard);
    अगर(prereq_rc != CURL_PREREQFUNC_OK) {
      failf(तथ्य, "operation aborted by pre-request callback");
      /* failure in pre-request callback - do not do any other processing */
      परिणाम = CURLE_ABORTED_BY_CALLBACK;
      multi_posttransfer(तथ्य);
      multi_done(तथ्य, परिणाम, FALSE);
      *stream_errorp = TRUE;
      जाओ end;
    }
  }

  अगर(तथ्य->set.connect_only && !तथ्य->set.connect_only_ws) {
    multistate(तथ्य, MSTATE_DONE);
    mresult = CURLM_CALL_MULTI_PERFORM;
  }
  अन्यथा {
    bool dophase_done = FALSE;
    /* Perform the protocol's DO action */
    परिणाम = multi_do(तथ्य, &dophase_done);

    अगर(!परिणाम) {
      अगर(!dophase_done) {
#अगर_अघोषित CURL_DISABLE_FTP
        /* some steps needed for wildcard matching */
        अगर(तथ्य->state.wildcardmatch) {
          काष्ठा Wildcard_तथ्य *wc = तथ्य->wildcard;
          अगर(wc->state == CURLWC_DONE || wc->state == CURLWC_SKIP) {
            /* skip some states if it is important */
            multi_done(तथ्य, CURLE_OK, FALSE);

            /* if there is no connection left, skip the DONE state */
            multistate(तथ्य, तथ्य->conn ? MSTATE_DONE : MSTATE_COMPLETED);
            mresult = CURLM_CALL_MULTI_PERFORM;
            जाओ end;
          }
        }
#पूर्ण_अगर
        /* DO was not completed in one function call, we must continue
           DOING... */
        multistate(तथ्य, MSTATE_DOING);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }

      /* after DO, go DO_DONE... or DO_MORE */
      अन्यथा अगर(तथ्य->conn->bits.do_more) {
        /* we are supposed to do more, but we need to sit down, relax and wait
           a little while first */
        multistate(तथ्य, MSTATE_DOING_MORE);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
      अन्यथा {
        /* we are done with the DO, now DID */
        multistate(तथ्य, MSTATE_DID);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    अन्यथा अगर((परिणाम == CURLE_SEND_ERROR) &&
            तथ्य->conn->bits.reuse) {
      /*
       * In this situation, a connection that we were trying to use may have
       * unexpectedly died. If possible, send the connection back to the
       * CONNECT phase so we can try again.
       */
      स्थिर काष्ठा Curl_scheme *handler = तथ्य->conn->scheme;
      अक्षर *newurl = शून्य;
      followtype follow = FOLLOW_NONE;
      CURLcode drc;

      drc = Curl_retry_request(तथ्य, &newurl);
      अगर(drc) {
        /* a failure here pretty much implies an out of memory */
        परिणाम = drc;
        *stream_errorp = TRUE;
      }

      multi_posttransfer(तथ्य);
      drc = multi_done(तथ्य, परिणाम, FALSE);

      /* When set to retry the connection, we must go back to the CONNECT
       * state */
      अगर(newurl) {
        अगर(!drc || (drc == CURLE_SEND_ERROR)) {
          follow = FOLLOW_RETRY;
          drc = multi_follow(तथ्य, handler, newurl, follow);
          अगर(!drc) {
            multistate(तथ्य, MSTATE_SETUP);
            mresult = CURLM_CALL_MULTI_PERFORM;
            परिणाम = CURLE_OK;
          }
          अन्यथा {
            /* Follow failed */
            परिणाम = drc;
          }
        }
        अन्यथा {
          /* done did not return OK or SEND_ERROR */
          परिणाम = drc;
        }
      }
      अन्यथा {
        /* Have error handler disconnect conn if we cannot retry */
        *stream_errorp = TRUE;
      }
      curlx_free(newurl);
    }
    अन्यथा {
      /* failure detected */
      multi_posttransfer(तथ्य);
      अगर(तथ्य->conn)
        multi_done(तथ्य, परिणाम, FALSE);
      *stream_errorp = TRUE;
    }
  }
end:
  *resultp = परिणाम;
  वापस mresult;
}

अटल CURLMcode multistate_ratelimiting(काष्ठा Curl_easy *तथ्य,
                                         CURLcode *resultp)
{
  CURLcode परिणाम = CURLE_OK;
  CURLMcode mresult = CURLM_OK;
  DEBUGASSERT(तथ्य->conn);
  /* if both rates are within spec, resume transfer */
  परिणाम = Curl_pgrsCheck(तथ्य);

  अगर(परिणाम) {
    अगर(!(तथ्य->conn->scheme->flags & PROTOPT_DUAL) &&
       परिणाम != CURLE_HTTP2_STREAM)
      streamclose(तथ्य->conn);

    multi_posttransfer(तथ्य);
    multi_done(तथ्य, परिणाम, TRUE);
  }
  अन्यथा {
    अगर(!mspeed_check(तथ्य))
      mresult = CURLM_CALL_MULTI_PERFORM;
  }
  *resultp = परिणाम;
  वापस mresult;
}

अटल CURLMcode multistate_connect(काष्ठा Curl_multi *multi,
                                    काष्ठा Curl_easy *तथ्य,
                                    CURLcode *resultp)
{
  /* Connect. We want to get a connection identifier filled in. This state can
     be entered from SETUP and from PENDING. */
  bool connected;
  CURLMcode mresult = CURLM_OK;
  CURLcode परिणाम = Curl_connect(तथ्य, &connected);
  अगर(परिणाम == CURLE_NO_CONNECTION_AVAILABLE) {
    /* There was no connection available. We will go to the pending state and
       wait for an available connection. */
    multistate(तथ्य, MSTATE_PENDING);
    /* move from process to pending set */
    Curl_uint32_bset_remove(&multi->process, तथ्य->mid);
    Curl_uint32_bset_remove(&multi->dirty, तथ्य->mid);
    Curl_uint32_bset_add(&multi->pending, तथ्य->mid);
    *resultp = CURLE_OK;
    वापस mresult;
  }
  अन्यथा
    multi_schedule_pending(तथ्य->multi);

  अगर(!परिणाम) {
    /* after the connect has been sent off, go WAITCONNECT unless the
       protocol connect is already done and we can go directly to WAITDO or
       DO! */
    mresult = CURLM_CALL_MULTI_PERFORM;

    अगर(connected) {
      अगर(!तथ्य->conn->bits.reuse &&
         Curl_conn_is_multiplex(तथ्य->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_schedule_pending(तथ्य->multi);
      }
      multistate(तथ्य, MSTATE_PROTOCONNECT);
    }
    अन्यथा {
      multistate(तथ्य, MSTATE_CONNECTING);
    }
  }
  *resultp = परिणाम;
  वापस mresult;
}

/* returns the possibly updated result */
अटल CURLcode is_finished(काष्ठा Curl_multi *multi,
                            काष्ठा Curl_easy *तथ्य,
                            स्थिर काष्ठा curltime *pnow,
                            bool stream_error,
                            CURLcode परिणाम)
{
  अगर(तथ्य->mstate < MSTATE_COMPLETED) {
    अगर(परिणाम) {
      /*
       * If an error was returned, and we are not in completed state now,
       * then we go to completed and consider this transfer aborted.
       */

      /* No attempt to disconnect connections must be made before this -
         connection detach and termination happens only here */

      /* Check if we can move pending requests to send pipe */
      multi_schedule_pending(multi); /* connection */

      अगर(तथ्य->conn) {
        अगर(stream_error) {
          /* Do not attempt to send data over a connection that timed out */
          bool dead_connection = परिणाम == CURLE_OPERATION_TIMEDOUT;
          काष्ठा connectdata *conn = तथ्य->conn;

          /* This is where we make sure that the conn pointer is reset.
             We do not have to do this in every case block above where a
             failure is detected */
          Curl_detach_connection(तथ्य);
          Curl_conn_close(तथ्य, conn, dead_connection);
        }
      }
      अन्यथा अगर(तथ्य->mstate == MSTATE_CONNECT) {
        /* Curl_connect() failed */
        multi_posttransfer(तथ्य);
        Curl_pgrsUpdate_nometer(तथ्य);
      }

      multistate(तथ्य, MSTATE_COMPLETED);
      वापस परिणाम;
    }
    /* if there is still a connection to use, call the progress function */
    अन्यथा अगर(तथ्य->conn && Curl_conn_is_connected(तथ्य->conn, FIRSTSOCKET)) {
      परिणाम = Curl_pgrsUpdateX(तथ्य, pnow);
      अगर(परिणाम) {
        /* aborted due to progress callback return code must close the
           connection */
        streamclose(तथ्य->conn);

        /* if not yet in DONE state, go there. */
        अगर(तथ्य->mstate < MSTATE_DONE)
          multistate(तथ्य, MSTATE_DONE);
        वापस परिणाम;
      }
    }
  }
  वापस परिणाम;
}

अटल व्योम handle_completed(काष्ठा Curl_multi *multi,
                             काष्ठा Curl_easy *तथ्य,
                             CURLcode परिणाम)
{
  bool संदेश_to_app = तथ्य->master_mid == UINT32_MAX;

  अगर(!संदेश_to_app) {
    /* A sub transfer, not reported to the application. Is anyone still
     * interested in processing its results? */
    अगर(तथ्य->sub_xfer_done) {
      काष्ठा Curl_easy *master = Curl_multi_get_easy(multi, तथ्य->master_mid);

      CURL_TRC_M(तथ्य, "sub xfer done for master %u", तथ्य->master_mid);
      अगर(master)
        तथ्य->sub_xfer_done(तथ्य, master, परिणाम);
      अन्यथा
        CURL_TRC_M(तथ्य, "master easy %u already gone.", तथ्य->master_mid);
    }
  }
  अन्यथा {
    /* now fill in the CURLMsg with this info */
    काष्ठा CURLMsg *संदेश = &तथ्य->संदेश;

    संदेश->संदेश = CURLMSG_DONE;
    संदेश->easy_handle = तथ्य;
    संदेश->तथ्य.परिणाम = परिणाम;

    DEBUGASSERT(!तथ्य->conn);
  }
  multistate(तथ्य, MSTATE_MSGSENT);

  /* remove from the other sets */
  Curl_uint32_bset_remove(&multi->process, तथ्य->mid);
  Curl_uint32_bset_remove(&multi->dirty, तथ्य->mid);
  Curl_uint32_bset_remove(&multi->pending, तथ्य->mid);
  अगर(संदेश_to_app)
    multi_addmsg(multi, तथ्य);
  अगर(तथ्य->state.really_alive) {
    तथ्य->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    अगर(!multi->xfers_really_alive)
      (व्योम)multi_assess_wakeup(multi);
  }
  --multi->xfers_alive;
  अगर(!multi->xfers_alive)
    multi_assess_wakeup(multi);
}

अटल CURLMcode multistate_init(काष्ठा Curl_easy *तथ्य, CURLcode *परिणाम)
{
  अगर(!तथ्य->state.really_alive) {
    तथ्य->state.really_alive = TRUE;
    ++तथ्य->multi->xfers_really_alive;
    अगर(तथ्य->multi->xfers_really_alive == 1) {
      CURLMcode mresult = multi_assess_wakeup(तथ्य->multi);
      अगर(mresult) {
        failf(तथ्य, "error enabling wakeup listening: %d", mresult);
        वापस mresult;
      }
    }
  }

  *परिणाम = Curl_pretransfer(तथ्य);
  अगर(*परिणाम)
    वापस CURLM_OK;

  /* after init, go SETUP */
  multistate(तथ्य, MSTATE_SETUP);
  Curl_pgrsTime(तथ्य, TIMER_STARTOP);
  वापस CURLM_CALL_MULTI_PERFORM;
}

अटल CURLMcode multistate_setup(काष्ठा Curl_easy *तथ्य)
{
  स्थिर काष्ठा curltime *pnow = Curl_pgrs_now(तथ्य);
  Curl_pgrsTimeWas(तथ्य, TIMER_STARTSINGLE, *pnow);
  अगर(तथ्य->set.timeout)
    Curl_expire_set(तथ्य, EXPIRE_TIMEOUT, तथ्य->set.timeout, pnow);
  अगर(तथ्य->set.connecttimeout)
    /* Since a connection might go to pending and back to CONNECT several
       times before it actually takes off, we need to set the timeout once
       in SETUP before we enter CONNECT the first time. */
    Curl_expire_set(तथ्य, EXPIRE_CONNECTTIMEOUT,
                    तथ्य->set.connecttimeout, pnow);

  multistate(तथ्य, MSTATE_CONNECT);
  वापस CURLM_CALL_MULTI_PERFORM;
}

अटल CURLMcode multistate_connecting(काष्ठा Curl_easy *तथ्य,
                                       bool *stream_error,
                                       CURLcode *परिणाम)
{
  bool connected;

  अगर(!तथ्य->conn) {
    DEBUGASSERT(0);
    *परिणाम = CURLE_FAILED_INIT;
    वापस CURLM_OK;
  }
  अगर(!Curl_xfer_recv_is_paused(तथ्य)) {
    *परिणाम = Curl_conn_connect(तथ्य, FIRSTSOCKET, FALSE, &connected);
    अगर(connected && !*परिणाम) {
      अगर(!तथ्य->conn->bits.reuse &&
         Curl_conn_is_multiplex(तथ्य->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_schedule_pending(तथ्य->multi);
      }
      multistate(तथ्य, MSTATE_PROTOCONNECT);
      वापस CURLM_CALL_MULTI_PERFORM;
    }
    अन्यथा अगर(*परिणाम) {
      /* failure detected */
      CURL_TRC_M(तथ्य, "connect failed -> %d", (पूर्णांक)*परिणाम);
      multi_posttransfer(तथ्य);
      multi_done(तथ्य, *परिणाम, TRUE);
      *stream_error = TRUE;
      वापस CURLM_OK;
    }
  }
  वापस CURLM_OK;
}

अटल CURLMcode multistate_protoconnect(काष्ठा Curl_easy *तथ्य,
                                         bool *stream_error,
                                         CURLcode *परिणाम)
{
  bool protocol_connected = FALSE;

  अगर(!*परिणाम && तथ्य->conn->bits.reuse) {
    /* ftp seems to hang when protoconnect on reused connection since we
     * handle PROTOCONNECT in general inside the filters, it seems wrong to
     * restart this on a reused connection.
     */
    multistate(तथ्य, MSTATE_DO);
    वापस CURLM_CALL_MULTI_PERFORM;
  }
  अगर(!*परिणाम)
    *परिणाम = protocol_connect(तथ्य, &protocol_connected);
  अगर(!*परिणाम && !protocol_connected) {
    /* switch to waiting state */
    multistate(तथ्य, MSTATE_PROTOCONNECTING);
    वापस CURLM_CALL_MULTI_PERFORM;
  }
  अन्यथा अगर(!*परिणाम) {
    /* protocol connect has completed, go WAITDO or DO */
    multistate(तथ्य, MSTATE_DO);
    वापस CURLM_CALL_MULTI_PERFORM;
  }

  /* failure detected */
  multi_posttransfer(तथ्य);
  multi_done(तथ्य, *परिणाम, TRUE);
  *stream_error = TRUE;
  वापस CURLM_OK;
}

अटल CURLMcode multistate_protoconnecting(काष्ठा Curl_easy *तथ्य,
                                            bool *stream_error,
                                            CURLcode *परिणाम)
{
  bool protocol_connected = FALSE;

  /* protocol-specific connect phase */
  *परिणाम = protocol_connecting(तथ्य, &protocol_connected);
  अगर(!*परिणाम && protocol_connected) {
    /* after the connect has completed, go WAITDO or DO */
    multistate(तथ्य, MSTATE_DO);
    वापस CURLM_CALL_MULTI_PERFORM;
  }
  अन्यथा अगर(*परिणाम) {
    /* failure detected */
    multi_posttransfer(तथ्य);
    multi_done(तथ्य, *परिणाम, TRUE);
    *stream_error = TRUE;
  }
  वापस CURLM_OK;
}

अटल CURLMcode multistate_doing(काष्ठा Curl_easy *तथ्य,
                                  bool *stream_error,
                                  CURLcode *परिणाम)
{
  bool dophase_done = FALSE;

  /* we continue DOING until the DO phase is complete */
  DEBUGASSERT(तथ्य->conn);
  *परिणाम = protocol_doing(तथ्य, &dophase_done);
  अगर(!*परिणाम) {
    अगर(dophase_done) {
      /* after DO, go DO_DONE or DO_MORE */
      multistate(तथ्य, तथ्य->conn->bits.do_more ?
                 MSTATE_DOING_MORE : MSTATE_DID);
      वापस CURLM_CALL_MULTI_PERFORM;
    } /* dophase_done */
  }
  अन्यथा {
    /* failure detected */
    multi_posttransfer(तथ्य);
    multi_done(तथ्य, *परिणाम, FALSE);
    *stream_error = TRUE;
  }
  वापस CURLM_OK;
}

अटल CURLMcode multistate_doing_more(काष्ठा Curl_easy *तथ्य,
                                       bool *stream_error,
                                       CURLcode *परिणाम)
{
  domore control;

  /*
   * When we are connected, DOING MORE and then go DID
   */
  DEBUGASSERT(तथ्य->conn);
  *परिणाम = multi_do_more(तथ्य, &control);

  अगर(!*परिणाम) {
    अगर(control != DOMORE_INCOMPLETE) {
      /* if DONE, advance to DO_DONE
         if GOBACK, go back to DOING */
      multistate(तथ्य, control == DOMORE_DONE ? MSTATE_DID : MSTATE_DOING);
      वापस CURLM_CALL_MULTI_PERFORM;
    }
    /* else
       stay in DO_MORE */
  }
  अन्यथा {
    /* failure detected */
    multi_posttransfer(तथ्य);
    multi_done(तथ्य, *परिणाम, FALSE);
    *stream_error = TRUE;
  }
  वापस CURLM_OK;
}

अटल CURLMcode multistate_did(काष्ठा Curl_multi *multi,
                                काष्ठा Curl_easy *तथ्य)
{
  DEBUGASSERT(तथ्य->conn);
  अगर(तथ्य->conn->bits.multiplex)
    /* Check if we can move pending requests to send pipe */
    multi_schedule_pending(multi); /* multiplexed */

  /* Only perform the transfer if there is a good socket to work with.
     Having both BAD is a signal to skip immediately to DONE */
  अगर(CONN_SOCK_IDX_VALID(तथ्य->conn->recv_idx) ||
     CONN_SOCK_IDX_VALID(तथ्य->conn->send_idx)) {
    multistate(तथ्य, MSTATE_PERFORMING);
    /* Do not return CURLM_CALL_MULTI_PERFORM to give other transfers
     * a chance to send off their requests.
     * Note: Some SFTP handlers do not seem to like this.
     *       Restrict it to HTTP families. */
    वापस ((multi->xfers_alive > 1) &&
            (तथ्य->conn->scheme->protocol & PROTO_FAMILY_HTTP)) ?
           CURLM_OK : CURLM_CALL_MULTI_PERFORM;
  }
  अन्यथा {
#अगर_अघोषित CURL_DISABLE_FTP
    अगर(तथ्य->state.wildcardmatch &&
       ((तथ्य->conn->scheme->flags & PROTOPT_WILDCARD) == 0)) {
      तथ्य->wildcard->state = CURLWC_DONE;
    }
#पूर्ण_अगर
    multistate(तथ्य, MSTATE_DONE);
    वापस CURLM_CALL_MULTI_PERFORM;
  }
}

अटल CURLMcode multistate_done(काष्ठा Curl_easy *तथ्य, CURLcode *presult)
{
  अगर(तथ्य->conn) {
    CURLcode परिणाम;

    /* post-transfer command */
    परिणाम = multi_done(तथ्य, *presult, FALSE);

    /* allow a previously set error code take precedence */
    अगर(!(*presult))
      *presult = परिणाम;
  }

#अगर_अघोषित CURL_DISABLE_FTP
  अगर(तथ्य->state.wildcardmatch) {
    अगर(तथ्य->wildcard->state != CURLWC_DONE) {
      /* if a wildcard is set and we are not ending -> lets start again
         with MSTATE_INIT */
      multistate(तथ्य, MSTATE_INIT);
      वापस CURLM_CALL_MULTI_PERFORM;
    }
  }
#पूर्ण_अगर
  /* after we have DONE what we are supposed to do, go COMPLETED, and
     it does not matter what the multi_done() returned! */
  multistate(तथ्य, MSTATE_COMPLETED);
  वापस CURLM_CALL_MULTI_PERFORM;
}

अटल CURLMcode multi_runsingle(काष्ठा Curl_multi *multi,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा Curl_sigpipe_ctx *sigpipe_ctx)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode परिणाम = CURLE_OK;
  स्थिर काष्ठा curltime *pnow = शून्य;

  अगर(multi->dead) {
    /* a multi-level callback returned error before, meaning every individual
     transfer now has failed */
    परिणाम = CURLE_ABORTED_BY_CALLBACK;
    multi_posttransfer(तथ्य);
    multi_done(तथ्य, परिणाम, FALSE);
    multistate(तथ्य, MSTATE_COMPLETED);
  }

  multi_warn_debug(multi, तथ्य);

  /* transfer runs now, clear the dirty bit. This may be set
   * again during processing, triggering a re-run later. */
  Curl_uint32_bset_remove(&multi->dirty, तथ्य->mid);

  अगर(तथ्य == multi->admin) {
#अगर_घोषित ENABLE_INTERNAL_WAKEUP
    /* Consume any pending wakeup signals before processing.
     * This is necessary for event based processing. See #21547 */
    (व्योम)Curl_wakeup_consume(multi->wakeup_internal, TRUE);
#पूर्ण_अगर
#अगर_घोषित USE_RESOLV_THREADED
    Curl_async_thrdd_multi_process(multi);
#पूर्ण_अगर
    Curl_cshutdn_perform(&multi->cshutdn, multi->admin, sigpipe_ctx);
    जाओ out;
  }

  sigpipe_apply(तथ्य, sigpipe_ctx);
  करो {
    /* A "stream" here is a logical stream if the protocol can handle that
       (HTTP/2), or the full connection for older protocols */
    bool stream_error = FALSE;
    mresult = CURLM_OK;
    pnow = शून्य;

    अगर(multi_ischanged(multi, TRUE)) {
      CURL_TRC_M(तथ्य, "multi changed, check CONNECT_PEND queue");
      multi_schedule_pending(multi); /* multiplexed */
    }

    अगर(तथ्य->mstate > MSTATE_CONNECT &&
       तथ्य->mstate < MSTATE_COMPLETED) {
      /* Make sure we set the connection's current owner */
      DEBUGASSERT(तथ्य->conn);
      अगर(!तथ्य->conn) {
        mresult = CURLM_INTERNAL_ERROR;
        जाओ out;
      }
    }

    /* Wait for the connect state as only then is the start time stored, but
       we must not check already completed handles */
    अगर((तथ्य->mstate >= MSTATE_CONNECT) && (तथ्य->mstate < MSTATE_COMPLETED)) {
      pnow = Curl_pgrs_now(तथ्य);
      अगर(multi_handle_timeout(तथ्य, pnow, &stream_error, &परिणाम))
        /* Skip the statemachine and go directly to error handling section. */
        जाओ statemachine_end;
      pnow = शून्य;
    }

    चयन(तथ्य->mstate) {
    हाल MSTATE_INIT:
      /* Transitional state. init this transfer. A handle never comes back to
         this state. */
      mresult = multistate_init(तथ्य, &परिणाम);
      अवरोध;

    हाल MSTATE_SETUP:
      /* Transitional state. Setup things for a new transfer. The handle
         can come back to this state on a redirect. */
      mresult = multistate_setup(तथ्य);
      अवरोध;

    हाल MSTATE_CONNECT:
      mresult = multistate_connect(multi, तथ्य, &परिणाम);
      अवरोध;

    हाल MSTATE_CONNECTING:
      /* awaiting a completion of an asynch TCP connect */
      mresult = multistate_connecting(तथ्य, &stream_error, &परिणाम);
      अवरोध;

    हाल MSTATE_PROTOCONNECT:
      mresult = multistate_protoconnect(तथ्य, &stream_error, &परिणाम);
      अवरोध;

    हाल MSTATE_PROTOCONNECTING:
      /* protocol-specific connect phase */
      mresult = multistate_protoconnecting(तथ्य, &stream_error, &परिणाम);
      अवरोध;

    हाल MSTATE_DO:
      mresult = multistate_do(तथ्य, &stream_error, &परिणाम);
      अवरोध;

    हाल MSTATE_DOING:
      /* we continue DOING until the DO phase is complete */
      mresult = multistate_doing(तथ्य, &stream_error, &परिणाम);
      अवरोध;

    हाल MSTATE_DOING_MORE:
      /*
       * When we are connected, DOING MORE and then go DID
       */
      mresult = multistate_doing_more(तथ्य, &stream_error, &परिणाम);
      अवरोध;

    हाल MSTATE_DID:
      mresult = multistate_did(multi, तथ्य);
      अवरोध;

    हाल MSTATE_RATELIMITING: /* limit-rate exceeded in either direction */
      mresult = multistate_ratelimiting(तथ्य, &परिणाम);
      अवरोध;

    हाल MSTATE_PERFORMING:
      mresult = multistate_performing(तथ्य, &stream_error, &परिणाम);
      अवरोध;

    हाल MSTATE_DONE:
      mresult = multistate_done(तथ्य, &परिणाम);
      अवरोध;

    हाल MSTATE_COMPLETED:
      अवरोध;

    हाल MSTATE_PENDING:
    हाल MSTATE_MSGSENT:
      /* handles in these states should NOT be in this list */
      अवरोध;

    शेष:
      mresult = CURLM_INTERNAL_ERROR;
      जाओ out;
    }

    अगर(तथ्य->mstate >= MSTATE_CONNECT &&
       तथ्य->mstate < MSTATE_DO &&
       mresult != CURLM_CALL_MULTI_PERFORM &&
       !multi_ischanged(multi, FALSE)) {
      /* We now handle stream timeouts if and only if this will be the last
       * loop iteration. We only check this on the last iteration to ensure
       * that if we know we have additional work to do immediately
       * (i.e. CURLM_CALL_MULTI_PERFORM == TRUE) then we should do that before
       * declaring the connection timed out as we may almost have a completed
       * connection. */
      pnow = Curl_pgrs_now(तथ्य);
      multi_handle_timeout(तथ्य, pnow, &stream_error, &परिणाम);
    }

statemachine_end:
    अगर(!pnow)
      pnow = Curl_pgrs_now(तथ्य);
    परिणाम = is_finished(multi, तथ्य, pnow, stream_error, परिणाम);
    अगर(परिणाम)
      mresult = CURLM_CALL_MULTI_PERFORM;

    अगर(MSTATE_COMPLETED == तथ्य->mstate) {
      handle_completed(multi, तथ्य, परिणाम);
      mresult = CURLM_OK;
      जाओ out;
    }
  } जबतक((mresult == CURLM_CALL_MULTI_PERFORM) ||
          multi_ischanged(multi, FALSE));

out:
  तथ्य->परिणाम = परिणाम;
  वापस mresult;
}

अटल CURLMcode multi_perform(काष्ठा Curl_multi *multi,
                               पूर्णांक *running_handles)
{
  CURLMcode returncode = CURLM_OK;
  काष्ठा curltime start;
  uint32_t mid;
  काष्ठा Curl_sigpipe_ctx sigpipe_ctx;

  curlx_pnow(&start);
  sigpipe_init(&sigpipe_ctx);

  अगर(Curl_uint32_bset_first(&multi->process, &mid)) {
    CURL_TRC_M(multi->admin, "multi_perform(running=%u)",
               multi_xfers_running(multi));
    करो {
      काष्ठा Curl_easy *तथ्य = Curl_multi_get_easy(multi, mid);
      CURLMcode mresult;
      अगर(!तथ्य) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        जारी;
      }
      mresult = multi_runsingle(multi, तथ्य, &sigpipe_ctx);
      अगर(mresult)
        returncode = mresult;
    } जबतक(Curl_uint32_bset_अग्र(&multi->process, mid, &mid));
  }
  sigpipe_restore(&sigpipe_ctx);

  अगर(multi_ischanged(multi, TRUE))
    multi_schedule_pending(multi);

  अगर(!returncode && CURL_MNTFY_HAS_ENTRIES(multi))
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
  जबतक(Curl_timeouts_remove_expired(&multi->timeouts, &start, &mid)) {
    /* the removed may have another timeout in queue */
    काष्ठा Curl_easy *तथ्य = Curl_multi_get_easy(multi, mid);
    अगर(!तथ्य) {
      DEBUGASSERT(0);
      जारी;
    }
    (व्योम)add_अग्र_timeout(&start, multi, तथ्य);
    अगर(तथ्य->mstate == MSTATE_PENDING) {
      bool stream_unused;
      CURLcode परिणाम_unused;
      अगर(multi_handle_timeout(तथ्य, Curl_pgrs_now(तथ्य),
                              &stream_unused, &परिणाम_unused)) {
        infof(तथ्य, "PENDING handle timeout");
        move_pending_to_connect(multi, तथ्य);
      }
    }
  }

  अगर(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < पूर्णांक_उच्च) ? (पूर्णांक)running : पूर्णांक_उच्च;
  }

  अगर(CURLM_OK >= returncode)
    returncode = Curl_update_timer(multi);

  वापस returncode;
}

CURLMcode curl_multi_perform(CURLM *m, पूर्णांक *running_handles)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_perform, &mresult)) {
    mresult = multi_perform(m, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

CURLMcode curl_multi_cleanup(CURLM *m)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_cleanup, &mresult)) {
    काष्ठा Curl_multi *multi = m;
    व्योम *entry;
    uint32_t mid;

    /* First remove all remaining easy handles,
     * close internal ones. admin handle is special */
    अगर(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
      करो {
        काष्ठा Curl_easy *तथ्य = entry;
        अगर(!GOOD_EASY_HANDLE(तथ्य)) {
          mresult = CURLM_BAD_HANDLE;
          जाओ out;
        }

#अगर_घोषित DEBUGBUILD
        अगर(mid != तथ्य->mid) {
          CURL_TRC_M(तथ्य, "multi_cleanup: still present with mid=%u, "
                     "but unexpected data->mid=%u\n", mid, तथ्य->mid);
          DEBUGASSERT(0);
        }
#पूर्ण_अगर

        अगर(तथ्य == multi->admin)
          जारी;

        अगर(!तथ्य->state.done && तथ्य->conn)
          /* if DONE was never called for this handle */
          (व्योम)multi_done(तथ्य, CURLE_OK, TRUE);

        तथ्य->multi = शून्य; /* clear the association */
        Curl_uint32_tbl_remove(&multi->xfers, mid);
        तथ्य->mid = UINT32_MAX;

#अगर_घोषित USE_LIBPSL
        अगर(तथ्य->psl == &multi->psl)
          तथ्य->psl = शून्य;
#पूर्ण_अगर
        अगर(तथ्य->state.internal)
          Curl_close(&तथ्य);
      } जबतक(Curl_uint32_tbl_अग्र(&multi->xfers, mid, &mid, &entry));
    }

#अगर_घोषित USE_RESOLV_THREADED
    Curl_async_thrdd_multi_destroy(multi, !multi->quick_exit);
#पूर्ण_अगर
    Curl_cpool_destroy(&multi->cpool, multi->admin);
    Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
    अगर(multi->admin) {
      CURL_TRC_M(multi->admin, "multi_cleanup, closing admin handle, done");
      multi->admin->multi = शून्य;
      Curl_uint32_tbl_remove(&multi->xfers, multi->admin->mid);
      Curl_close(&multi->admin);
    }

    multi->magic = 0; /* not good anymore */

    Curl_multi_ev_cleanup(multi);
    Curl_hash_destroy(&multi->proto_hash);
    Curl_dnscache_destroy(&multi->dnscache);
    Curl_psl_destroy(&multi->psl);
#अगर_घोषित USE_SSL
    Curl_ssl_scache_destroy(multi->ssl_scache);
#पूर्ण_अगर

#अगर_घोषित USE_WINSOCK
    WSACloseEvent(multi->wsa_event);
#पूर्ण_अगर
#अगर_घोषित ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#पूर्ण_अगर
#अगर_घोषित ENABLE_INTERNAL_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_internal);
#पूर्ण_अगर

    multi_xfer_bufs_free(multi);
    Curl_mntfy_cleanup(multi);
#अगर_घोषित DEBUGBUILD
    अगर(Curl_uint32_tbl_गिनती(&multi->xfers)) {
      multi_xfer_tbl_dump(multi);
      DEBUGASSERT(0);
    }
#पूर्ण_अगर
    Curl_uint32_bset_destroy(&multi->process);
    Curl_uint32_bset_destroy(&multi->dirty);
    Curl_uint32_bset_destroy(&multi->pending);
    Curl_uint32_bset_destroy(&multi->msgsent);
    Curl_uint32_tbl_destroy(&multi->xfers);
    curlx_memzero(multi, माप(*multi));
    curlx_free(multi);

    mresult = CURLM_OK;
  }
out:
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

/*
 * curl_multi_info_read()
 *
 * This function is the primary way for a multi/multi_socket application to
 * figure out if a transfer has ended.
 */

CURLMsg *curl_multi_info_read(CURLM *m, पूर्णांक *msgs_in_queue)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMsg *संदेश_परिणाम = शून्य;

  *msgs_in_queue = 0; /* default to none */
  अगर(CURL_MAPI_ENTER(&guard, m, multi_info_read, शून्य)) {
    काष्ठा Curl_multi *multi = m;
    uint32_t mid;
    अगर(Curl_uint32_bset_first(&multi->msgsent, &mid)) {
      काष्ठा Curl_easy *तथ्य = Curl_multi_get_easy(multi, mid);

      DEBUGASSERT(तथ्य);
      Curl_uint32_bset_remove(&multi->msgsent, mid);
      *msgs_in_queue =
        curlx_uztosi(Curl_uint32_bset_गिनती(&multi->msgsent));
      अगर(तथ्य)
        संदेश_परिणाम = &तथ्य->संदेश;
    }
  }
  CURL_MAPI_LEAVE(&guard);
  वापस संदेश_परिणाम;
}

व्योम Curl_multi_will_close(काष्ठा Curl_easy *तथ्य, curl_socket_t s)
{
  अगर(तथ्य) {
    काष्ठा Curl_multi *multi = तथ्य->multi;
    अगर(multi) {
      CURL_TRC_M(तथ्य, "Curl_multi_will_close fd=%" FMT_SOCKET_T, s);
      Curl_multi_ev_socket_done(multi, तथ्य, s);
    }
  }
}

अटल व्योम multi_timeouts_init(काष्ठा Curl_easy *तथ्य)
{
  तथ्य->state.timeouts.first = EXPIRE_LAST;
  तथ्य->state.timeouts.splaynode.registered = FALSE;
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
अटल CURLMcode add_अग्र_timeout(स्थिर काष्ठा curltime *pnow,
                                  काष्ठा Curl_multi *multi,
                                  काष्ठा Curl_easy *तथ्य)
{
  काष्ठा expire_timers *timeouts = &तथ्य->state.timeouts;
  timediff_t now_us = Curl_timeouts_offset_us(&multi->timeouts, pnow);

  जबतक(timeouts->first < EXPIRE_LAST) {
    अगर(timeouts->offset_us[timeouts->first] <= now_us)  /* already expired */
      timeouts->first = timeouts->अग्र[timeouts->first];
    अन्यथा /* timeouts are sorted, first is first in the future now */
      अवरोध;
  }

  अगर(timeouts->first < EXPIRE_LAST) {
    /* Insert this node again into the splay. Keep the timer in the list in
       case we need to recompute future timers. */
    Curl_timeouts_add(&multi->timeouts, तथ्य,
                      timeouts->offset_us[timeouts->first]);
  }
  वापस CURLM_OK;
}

अटल व्योम multi_mark_expired_as_dirty(काष्ठा Curl_multi *multi,
                                        स्थिर काष्ठा curltime *ts)
{
  काष्ठा Curl_easy *तथ्य = शून्य;
  uint32_t mid;

  /*
   * The loop following here will go on as long as there are expire-times left
   * to process (compared to `ts`) in the splay and 'data' will be
   * re-assigned for every expired handle we deal with.
   */
  जबतक(Curl_timeouts_remove_expired(&multi->timeouts, ts, &mid)) {
    /* Check if there is one (more) expired timer to deal with! This function
       extracts a matching node if there is one */
    तथ्य = Curl_multi_get_easy(multi, mid);
    अगर(!तथ्य) {
      DEBUGASSERT(0);
      जारी;
    }
#अगर_घोषित CURLVERBOSE
    अगर(CURL_TRC_TIMER_is_verbose(तथ्य)) {
      अगर(तथ्य->state.timeouts.first < EXPIRE_LAST) {
        CURL_TRC_TIMER(तथ्य, तथ्य->state.timeouts.first, "has expired");
      }
    }
#पूर्ण_अगर
    (व्योम)add_अग्र_timeout(ts, multi, तथ्य);
    Curl_multi_mark_dirty(तथ्य);
  }
}

अटल CURLMcode multi_run_dirty(काष्ठा Curl_multi *multi,
                                 काष्ठा Curl_sigpipe_ctx *sigpipe_ctx,
                                 uint32_t *pnum)
{
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;

  *pnum = 0;
  अगर(Curl_uint32_bset_first(&multi->dirty, &mid)) {
    करो {
      काष्ठा Curl_easy *तथ्य = Curl_multi_get_easy(multi, mid);
      अगर(तथ्य) {
        CURL_TRC_M(तथ्य, "multi_run_dirty");

        अगर(!Curl_uint32_bset_contains(&multi->process, mid)) {
          अगर(Curl_uint32_bset_contains(&multi->pending, mid)) {
            /* Something happened on the pending transfer, act on it. */
            move_pending_to_connect(multi, तथ्य);
          }
          अन्यथा {
            /* We are no longer processing this transfer */
            Curl_uint32_bset_remove(&multi->dirty, mid);
            जारी;
          }
        }

        (*pnum)++;
        /* runsingle() clears the dirty mid */
        mresult = multi_runsingle(multi, तथ्य, sigpipe_ctx);

        अगर(CURLM_OK >= mresult) {
          /* reassess event handling of data */
          mresult = Curl_multi_ev_assess_xfer(multi, तथ्य);
          अगर(mresult)
            जाओ out;
        }
      }
      अन्यथा {
        CURL_TRC_M(multi->admin, "multi_run_dirty, %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } जबतक(Curl_uint32_bset_अग्र(&multi->dirty, mid, &mid));
  }

out:
  वापस mresult;
}

अटल CURLMcode multi_socket(काष्ठा Curl_multi *multi,
                              bool checkall,
                              curl_socket_t s,
                              पूर्णांक ev_bitmask,
                              पूर्णांक *running_handles)
{
  CURLMcode mresult = CURLM_OK;
  काष्ठा Curl_sigpipe_ctx pipe_ctx;
  काष्ठा curltime now;
  uint32_t run_xfers;

  (व्योम)ev_bitmask;
  sigpipe_init(&pipe_ctx);

  अगर(checkall) {
    /* *perform() deals with running_handles on its own */
    mresult = multi_perform(multi, running_handles);

    अगर(mresult != CURLM_BAD_HANDLE) {
      /* Reassess event status of all active transfers, but keep any error
         from multi_perform() */
      CURLMcode mresult2 =
        Curl_multi_ev_assess_xfer_bset(multi, &multi->process);
      अगर(!mresult)
        mresult = mresult2;
    }
    जाओ out;
  }

  अगर(s != CURL_SOCKET_TIMEOUT) {
    /* Mark all transfers of that socket as dirty */
    Curl_multi_ev_dirty_xfers(multi, s);
  }
  अन्यथा {
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
    अगर(mresult)
      जाओ out;
  }

  curlx_pnow(&now);
  multi_mark_expired_as_dirty(multi, &now);
  mresult = multi_run_dirty(multi, &pipe_ctx, &run_xfers);
  अगर(mresult)
    जाओ out;

  अगर(run_xfers) {
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

  अगर(multi_ischanged(multi, TRUE))
    multi_schedule_pending(multi);

  अगर(!mresult && CURL_MNTFY_HAS_ENTRIES(multi))
    mresult = Curl_mntfy_dispatch_all(multi);

  अगर(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < पूर्णांक_उच्च) ? (पूर्णांक)running : पूर्णांक_उच्च;
  }

  अगर(CURLM_OK >= mresult)
    mresult = Curl_update_timer(multi);
  वापस mresult;
}

#अघोषित curl_multi_setopt
CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_setopt, &mresult)) {
    काष्ठा Curl_multi *multi = m;
    बहु_सूची param;
    अचिन्हित दीर्घ uarg;
    माप_प्रकार szarg;

    बहु_शुरू(param, option);

    चयन(option) {
    हाल CURLMOPT_SOCKETFUNCTION:
      multi->socket_cb = बहु_तर्क(param, curl_socket_callback);
      अवरोध;
    हाल CURLMOPT_SOCKETDATA:
      multi->socket_userp = बहु_तर्क(param, व्योम *);
      अवरोध;
    हाल CURLMOPT_PUSHFUNCTION:
      multi->धकेलो_cb = बहु_तर्क(param, curl_धकेलो_callback);
      अवरोध;
    हाल CURLMOPT_PUSHDATA:
      multi->धकेलो_userp = बहु_तर्क(param, व्योम *);
      अवरोध;
    हाल CURLMOPT_PIPELINING:
      multi->multiplexing = बहु_तर्क(param, दीर्घ) & CURLPIPE_MULTIPLEX ? 1 : 0;
      अवरोध;
    हाल CURLMOPT_TIMERFUNCTION:
      multi->timer_cb = बहु_तर्क(param, curl_multi_timer_callback);
      अवरोध;
    हाल CURLMOPT_TIMERDATA:
      multi->timer_userp = बहु_तर्क(param, व्योम *);
      अवरोध;
    हाल CURLMOPT_MAXCONNECTS:
      uarg = बहु_तर्क(param, अचिन्हित दीर्घ);
      अगर(uarg <= UINT32_MAX)
        multi->maxconnects = (uint32_t)uarg;
      अवरोध;
    हाल CURLMOPT_MAX_HOST_CONNECTIONS:
      अगर(!curlx_sltouz(बहु_तर्क(param, दीर्घ), &szarg))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      multi->max_host_connections = (szarg < UINT32_MAX) ?
                                    (uint32_t)szarg : UINT32_MAX;
      अवरोध;
    हाल CURLMOPT_MAX_योग_CONNECTIONS:
      अगर(!curlx_sltouz(बहु_तर्क(param, दीर्घ), &szarg))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      multi->max_योग_connections = (szarg < UINT32_MAX) ?
                                     (uint32_t)szarg : UINT32_MAX;
      अवरोध;
      /* options formerly used for pipelining */
    हाल CURLMOPT_MAX_PIPELINE_लंबाई:
      अवरोध;
    हाल CURLMOPT_CONTENT_लंबाई_PENALTY_आकार:
      अवरोध;
    हाल CURLMOPT_CHUNK_लंबाई_PENALTY_आकार:
      अवरोध;
    हाल CURLMOPT_PIPELINING_SITE_BL:
      अवरोध;
    हाल CURLMOPT_PIPELINING_SERVER_BL:
      अवरोध;
    हाल CURLMOPT_MAX_CONCURRENT_STREAMS: {
      अगर(!curlx_sltouz(बहु_तर्क(param, दीर्घ), &szarg) ||
         !szarg || (szarg > (माप_प्रकार)पूर्णांक_उच्च)) /* preserve previous cutoff */
        multi->max_concurrent_streams = 100;
      अन्यथा
        multi->max_concurrent_streams = (szarg < UINT32_MAX) ?
                                       (uint32_t)szarg : UINT32_MAX;
      अवरोध;
    }
    हाल CURLMOPT_NETWORK_CHANGED: {
      दीर्घ val = बहु_तर्क(param, दीर्घ);
      अगर(val & CURLMNWC_CLEAR_ALL)
        /* In the beginning, all values available to set were 1 by mistake. We
           converted this to mean "all", thus setting all the bits
           automatically */
        val = CURLMNWC_CLEAR_DNS | CURLMNWC_CLEAR_CONNS;
      अगर(val & CURLMNWC_CLEAR_DNS) {
        Curl_dnscache_clear(multi->admin);
      }
      अगर(val & CURLMNWC_CLEAR_CONNS) {
        Curl_cpool_nw_changed(&multi->cpool, multi->admin);
      }
      अवरोध;
    }
    हाल CURLMOPT_NOTIFYFUNCTION:
      multi->ntfy.ntfy_cb = बहु_तर्क(param, curl_notify_callback);
      अवरोध;
    हाल CURLMOPT_NOTIFYDATA:
      multi->ntfy.ntfy_cb_तथ्य = बहु_तर्क(param, व्योम *);
      अवरोध;
    हाल CURLMOPT_RESOLVE_THREADS_MAX:
#अगर_घोषित USE_RESOLV_THREADED
      uarg = बहु_तर्क(param, दीर्घ);
      अगर((uarg <= 0) || (uarg > UINT32_MAX))
        mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      अन्यथा {
        CURLcode परिणाम = Curl_async_thrdd_multi_set_props(
          multi, 0, (uint32_t)uarg, 2000);
        चयन(परिणाम) {
        हाल CURLE_OK:
          mresult = CURLM_OK;
          अवरोध;
        हाल CURLE_BAD_FUNCTION_ARGUMENT:
          mresult = CURLM_BAD_FUNCTION_ARGUMENT;
          अवरोध;
        हाल CURLE_OUT_OF_MEMORY:
          mresult = CURLM_OUT_OF_MEMORY;
          अवरोध;
        शेष:
          mresult = CURLM_INTERNAL_ERROR;
          अवरोध;
        }
      }
#पूर्ण_अगर
      अवरोध;
    हाल CURLMOPT_QUICK_EXIT:
      multi->quick_exit = बहु_तर्क(param, दीर्घ) ? 1 : 0;
      अवरोध;
    शेष:
      mresult = CURLM_UNKNOWN_OPTION;
      अवरोध;
    }
    बहु_पूर्ण(param);
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

/* we define curl_multi_socket() in the public multi.h header */
#अघोषित curl_multi_socket

CURLMcode curl_multi_socket(CURLM *m, curl_socket_t s, पूर्णांक *running_handles)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_socket, &mresult)) {
    mresult = multi_socket(m, FALSE, s, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

CURLMcode curl_multi_socket_action(CURLM *m, curl_socket_t s,
                                   पूर्णांक ev_bitmask, पूर्णांक *running_handles)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_socket_action, &mresult)) {
    mresult = multi_socket(m, FALSE, s, ev_bitmask, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

CURLMcode curl_multi_socket_all(CURLM *m, पूर्णांक *running_handles)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_socket_all, &mresult)) {
    mresult = multi_socket(m, TRUE, CURL_SOCKET_BAD, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

अटल bool multi_has_dirties(काष्ठा Curl_multi *multi)
{
  uint32_t mid;
  अगर(Curl_uint32_bset_first(&multi->dirty, &mid)) {
    करो {
      काष्ठा Curl_easy *तथ्य = Curl_multi_get_easy(multi, mid);
      अगर(तथ्य) {
        अगर(Curl_uint32_bset_contains(&multi->process, mid))
          वापस TRUE;
        /* We are no longer processing this transfer */
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
      अन्यथा {
        CURL_TRC_M(multi->admin, "dirty transfer %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } जबतक(Curl_uint32_bset_अग्र(&multi->dirty, mid, &mid));
  }
  वापस FALSE;
}

अटल व्योम multi_timeout(काष्ठा Curl_multi *multi,
                          timediff_t *pexire_offset_us,
                          पूर्णांक *timeout_ms)
{
  अगर(multi->dead) {
    अगर(pexire_offset_us)
      *pexire_offset_us = 0;
    *timeout_ms = 0;
    वापस;
  }

  अगर(multi_has_dirties(multi)) {
    अगर(pexire_offset_us) {
      काष्ठा curltime now;
      curlx_pnow(&now);
      *pexire_offset_us = Curl_timeouts_offset_us(&multi->timeouts, &now);
    }
    *timeout_ms = 0;
    वापस;
  }
  अन्यथा {
    काष्ठा curltime now;
    uint32_t mid;

    curlx_pnow(&now);
    *timeout_ms = Curl_timeouts_अग्र_ms(&multi->timeouts, &now,
                                        pexire_offset_us, &mid);
#अगर_घोषित CURLVERBOSE
    अगर(mid != UINT32_MAX) {
      काष्ठा Curl_easy *तथ्य = Curl_multi_get_easy(multi, mid);
      अगर(तथ्य && CURL_TRC_TIMER_is_verbose(तथ्य) &&
         (तथ्य->state.timeouts.first < EXPIRE_LAST)) {
        CURL_TRC_TIMER(तथ्य, तथ्य->state.timeouts.first,
                       "gives multi timeout in %dms", *timeout_ms);
      }
    }
#पूर्ण_अगर
  }
}

CURLMcode curl_multi_timeout(CURLM *m,
                             दीर्घ *timeout_ms)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_timeout, &mresult)) {
    पूर्णांक itimeout_ms;

    multi_timeout(m, शून्य, &itimeout_ms);
    *timeout_ms = (दीर्घ)itimeout_ms;
    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

/*
 * Tell the application it should update its timers, if it subscribes to the
 * update timer callback.
 */
CURLMcode Curl_update_timer(काष्ठा Curl_multi *multi)
{
  timediff_t timeouts_offset_us = 0;
  पूर्णांक timeout_ms;
  पूर्णांक rc;
  bool set_मूल्य = FALSE;

  अगर(!multi->timer_cb || multi->dead)
    वापस CURLM_OK;
  multi_timeout(multi, &timeouts_offset_us, &timeout_ms);

  अगर(timeout_ms < 0 && !multi->last_timeout_set) {
    /* nothing to do */
  }
  अन्यथा अगर(timeout_ms < 0) {
    /* there is no timeout now but there was one previously */
    CURL_TRC_M(multi->admin, "[TIMER] clear");
    timeout_ms = -1; /* normalize */
    set_मूल्य = TRUE;
  }
  अन्यथा अगर(!multi->last_timeout_set) {
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, none before", timeout_ms);
    set_मूल्य = TRUE;
  }
  अन्यथा अगर(multi->last_expire_offset_us != timeouts_offset_us) {
    /* We had a timeout before and have one now, the absolute timestamp
     * differs. The relative timeout_ms may be the same, but the starting
     * point differs. Let the application restart its timer. */
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, replace previous",
               timeout_ms);
    set_मूल्य = TRUE;
  }
  अन्यथा {
    /* We have same expire time as previously. Our relative 'timeout_ms'
     * may be different now, but the application has the timer running
     * and we do not to tell it to start this again. */
  }

  अगर(set_मूल्य) {
    काष्ठा Curl_mapi_guard guard;

    multi->last_expire_offset_us = timeouts_offset_us;
    multi->last_timeout_set = timeout_ms >= 0;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_timer_cb);
    rc = multi->timer_cb(multi, timeout_ms, multi->timer_userp);
    CURL_CBAPI_MULTI_END(&guard);
    अगर(rc == -1) {
      multi->dead = TRUE;
      वापस CURLM_ABORTED_BY_CALLBACK;
    }
  }
  वापस CURLM_OK;
}

#अगर_घोषित DEBUGBUILD
अटल bool multi_timeouts_check(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा expire_timers *timeouts = &तथ्य->state.timeouts;
  uint8_t id;
  पूर्णांक i = 0;
  क्रम(id = timeouts->first; id < EXPIRE_LAST; id = timeouts->अग्र[id]) {
    अगर(++i >= EXPIRE_LAST) {
      failf(तथ्य, "expire timeouts looped: %d iterations and no end", i);
      वापस FALSE;
    }
    अगर(id == timeouts->अग्र[id]) {
      failf(तथ्य, "expire timeouts wrong: %d points to itself", (पूर्णांक)id);
      वापस FALSE;
    }
    अगर((timeouts->अग्र[id] < EXPIRE_LAST) &&
       (timeouts->offset_us[id] > timeouts->offset_us[timeouts->अग्र[id]])) {
      failf(तथ्य, "expire timeouts not sorted: %d happens after %d but "
            "is listed before", (पूर्णांक)id, (पूर्णांक)timeouts->अग्र[id]);
      वापस FALSE;
    }
  }
  वापस TRUE;
}
#पूर्ण_अगर

/*
 * Remove a given timestamp from the list of timeouts.
 */
अटल व्योम multi_clear_timeout(काष्ठा Curl_easy *तथ्य, expire_id eid)
{
  काष्ठा expire_timers *timeouts = &तथ्य->state.timeouts;
  uint8_t orig_first = timeouts->first;
  uint8_t *anchor = &timeouts->first;
  uint8_t id = (uint8_t)eid;

  अगर((अचिन्हित)eid >= EXPIRE_LAST) {
    DEBUGASSERT(0);
    वापस;
  }

  जबतक(*anchor < EXPIRE_LAST) {
    अगर(*anchor == id) {
      *anchor = timeouts->अग्र[id];
      अवरोध;
    }
    anchor = &timeouts->अग्र[*anchor];
  }
  DEBUGASSERT(multi_timeouts_check(तथ्य));
  अगर(Curl_timeouts_has(तथ्य)) {
    काष्ठा Curl_multi *multi = तथ्य->multi;

    अगर(!multi) {
      DEBUGASSERT(0);
      वापस;
    }
    अगर((timeouts->first >= EXPIRE_LAST) || /* no more timeouts */
       (timeouts->first != orig_first)) {  /* active timeout changed */
      Curl_timeouts_remove(&multi->timeouts, तथ्य);
    }
    अगर((timeouts->first < EXPIRE_LAST) && !Curl_timeouts_has(तथ्य)) {
      Curl_timeouts_add(&multi->timeouts, तथ्य,
                        timeouts->offset_us[timeouts->first]);
    }
  }
}

/*
 * Add a timestamp to the list of timeouts. Keep the list sorted so that head
 * of list is always the timeout nearest in time.
 */
अटल CURLMcode multi_set_timeout(काष्ठा Curl_easy *तथ्य,
                                   स्थिर काष्ठा curltime *stamp,
                                   expire_id eid)
{
  काष्ठा expire_timers *timeouts = &तथ्य->state.timeouts;
  uint8_t *anchor = &timeouts->first;
  uint8_t id = (uint8_t)eid;

  अगर((अचिन्हित)eid >= EXPIRE_LAST) {
    DEBUGASSERT(0);
    वापस CURLM_BAD_FUNCTION_ARGUMENT;
  }
  /* remove from list, store time and re-insert */
  multi_clear_timeout(तथ्य, eid);
  timeouts->offset_us[id] =
    Curl_timeouts_offset_us(&तथ्य->multi->timeouts, stamp);

  जबतक(*anchor < EXPIRE_LAST) {
    अगर(timeouts->offset_us[*anchor] > timeouts->offset_us[id])
      अवरोध;
    anchor = &timeouts->अग्र[*anchor];
  }
  timeouts->अग्र[eid] = *anchor;
  timeouts->अग्र[eid] = *anchor;
  *anchor = id;
  DEBUGASSERT(multi_timeouts_check(तथ्य));
  CURL_TRC_TIMER(तथ्य, eid, "set for %" FMT_TIMEDIFF_T "us",
                 curlx_ptimediff_us(stamp, Curl_pgrs_now(तथ्य)));
  वापस CURLM_OK;
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
व्योम Curl_expire_set(काष्ठा Curl_easy *तथ्य,
                     expire_id eid, timediff_t ms,
                     स्थिर काष्ठा curltime *pnow)
{
  काष्ठा Curl_multi *multi = तथ्य->multi;
  काष्ठा expire_timers *timeouts = &तथ्य->state.timeouts;
  uint8_t पूर्व_id = timeouts->first;
  काष्ठा curltime set;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  अगर(!multi)
    वापस;
  DEBUGASSERT(eid < EXPIRE_LAST);
  अगर(ms > पूर्णांक_उच्च)
    /* Cap ridiculous timeouts, 31-bit ms is still 3.5 weeks. When the time
       goes to the user, it must fit in this size. */
    ms = पूर्णांक_उच्च;

  set = *pnow;
  set.tv_sec += (समय_प्रकार)(ms / 1000); /* may be a 64 to 32-bit conversion */
  set.tv_usec += (पूर्णांक)(ms % 1000) * 1000;
  अगर(set.tv_usec >= 1000000) {
    set.tv_sec++;
    set.tv_usec -= 1000000;
  }

  /* Add the timeout, will replace any previous value for this timer. */
  multi_set_timeout(तथ्य, &set, eid);
  DEBUGASSERT(timeouts->first < EXPIRE_LAST);

  अगर(Curl_timeouts_has(तथ्य)) {
    /* data has already a timeout registered. If the first timer
     * was NOT the one we just set AND is still the first one,
     * nothing changed from the timeouts point of view. The
     * set timer triggers after the one already registered. Leave. */
    अगर((पूर्व_id != eid) && (पूर्व_id == timeouts->first))
      वापस;

    /* Since this is an updated time, we must remove data from
     * timeouts and then add it again. */
    Curl_timeouts_remove(&multi->timeouts, तथ्य);
  }

  /* Insert the new timer expiry since it is our local minimum. */
  Curl_timeouts_add(&multi->timeouts, तथ्य,
                    timeouts->offset_us[timeouts->first]);
}

व्योम Curl_expire(काष्ठा Curl_easy *तथ्य,
                 timediff_t milli, expire_id eid)
{
  Curl_expire_set(तथ्य, eid, milli, Curl_pgrs_now(तथ्य));
}

/*
 * Removes the expire timer. Marks it as done.
 */
व्योम Curl_expire_clear(काष्ठा Curl_easy *तथ्य, expire_id eid)
{
  /* remove the timer, if there */
  multi_clear_timeout(तथ्य, eid);
  CURL_TRC_TIMER(तथ्य, eid, "cleared");
}

/*
 * Clear ALL timeout values for this handle.
 */
व्योम Curl_expire_clear_all(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_multi *multi = तथ्य->multi;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  अगर(!multi)
    वापस;

  अगर(Curl_timeouts_remove(&multi->timeouts, तथ्य)) {
    /* Since this is an cleared time, we must remove the previous entry from
       the splay tree */
    multi_timeouts_init(तथ्य);

    अगर(तथ्य->id >= 0)
      CURL_TRC_M(तथ्य, "[TIMEOUT] all cleared");
  }
}

CURLMcode curl_multi_assign(CURLM *m, curl_socket_t sockfd,
                            व्योम *sockp)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_assign, &mresult)) {
    mresult = Curl_multi_ev_assign(m, sockfd, sockp);
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

अटल व्योम move_pending_to_connect(काष्ठा Curl_multi *multi,
                                    काष्ठा Curl_easy *तथ्य)
{
  DEBUGASSERT(तथ्य->mstate == MSTATE_PENDING);

  /* Remove this node from the pending set, add into process set */
  Curl_uint32_bset_remove(&multi->pending, तथ्य->mid);
  Curl_uint32_bset_add(&multi->process, तथ्य->mid);

  multistate(तथ्य, MSTATE_CONNECT);
  Curl_multi_mark_dirty(तथ्य); /* make it run */
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
अटल व्योम multi_schedule_pending(काष्ठा Curl_multi *multi)
{
  uint32_t mid = multi->last_pending_mid;

  अगर(mid) {
    जबतक(Curl_uint32_bset_अग्र(&multi->pending, mid, &mid)) {
      काष्ठा Curl_easy *तथ्य = Curl_multi_get_easy(multi, mid);
      अगर(तथ्य) {
        move_pending_to_connect(multi, तथ्य);
        multi->last_pending_mid = mid;
        वापस;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    }
    /* found no pending transfers with `mid` larger than `last_pending_mid`.
     * Start at the beginning of the pending set again. */
    multi->last_pending_mid = 0;
  }

  अगर(Curl_uint32_bset_first(&multi->pending, &mid)) {
    करो {
      काष्ठा Curl_easy *तथ्य = Curl_multi_get_easy(multi, mid);
      अगर(तथ्य) {
        move_pending_to_connect(multi, तथ्य);
        multi->last_pending_mid = mid;
        वापस;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    } जबतक(Curl_uint32_bset_अग्र(&multi->pending, mid, &mid));
  }
}

uint32_t Curl_multi_max_concurrent_streams(काष्ठा Curl_multi *multi)
{
  DEBUGASSERT(multi);
  वापस multi->max_concurrent_streams;
}

CURL **curl_multi_get_handles(CURLM *m)
{
  काष्ठा Curl_mapi_guard guard;
  CURL **a = शून्य;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_get_handles, शून्य)) {
    काष्ठा Curl_multi *multi = m;
    व्योम *entry;
    माप_प्रकार गिनती = Curl_uint32_tbl_गिनती(&multi->xfers);

    a = curlx_malloc(माप(काष्ठा Curl_easy *) * (गिनती + 1));
    अगर(a) {
      अचिन्हित पूर्णांक i = 0;
      uint32_t mid;

      अगर(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
        करो {
          काष्ठा Curl_easy *तथ्य = entry;
          DEBUGASSERT(i < गिनती);
          अगर(!तथ्य->state.internal)
            a[i++] = तथ्य;
        } जबतक(Curl_uint32_tbl_अग्र(&multi->xfers, mid, &mid, &entry));
      }
      a[i] = शून्य; /* last entry is a NULL */
    }
  }
  CURL_MAPI_LEAVE(&guard);
  वापस a;
}

CURLMcode curl_multi_get_offt(CURLM *m,
                              CURLMinfo_offt info,
                              curl_off_t *pvalue)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_get_offt, &mresult)) {
    काष्ठा Curl_multi *multi = m;
    uint32_t n;

    अगर(!pvalue) {
      mresult = CURLM_BAD_FUNCTION_ARGUMENT;
      जाओ out;
    }

    चयन(info) {
    हाल CURLMINFO_XFERS_CURRENT:
      n = Curl_uint32_tbl_गिनती(&multi->xfers);
      अगर(n && multi->admin)
        --n;
      *pvalue = (curl_off_t)n;
      अवरोध;
    हाल CURLMINFO_XFERS_RUNNING:
      n = Curl_uint32_bset_गिनती(&multi->process);
      अगर(n && Curl_uint32_bset_contains(&multi->process, multi->admin->mid))
        --n;
      *pvalue = (curl_off_t)n;
      अवरोध;
    हाल CURLMINFO_XFERS_PENDING:
      *pvalue = (curl_off_t)Curl_uint32_bset_गिनती(&multi->pending);
      अवरोध;
    हाल CURLMINFO_XFERS_DONE:
      *pvalue = (curl_off_t)Curl_uint32_bset_गिनती(&multi->msgsent);
      अवरोध;
    हाल CURLMINFO_XFERS_ADDED:
      *pvalue = multi->xfers_योग_ever;
      अवरोध;
    शेष:
      *pvalue = -1;
      mresult = CURLM_UNKNOWN_OPTION;
      अवरोध;
    }
  }
out:
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

अटल काष्ठा Curl_fixed_buf *fixed_buf_create(माप_प्रकार len)
{
  काष्ठा Curl_fixed_buf *fbuf;
  अगर((SIZE_MAX - माप(*fbuf)) < len)
    वापस शून्य; /* too large */
  fbuf = curlx_malloc(len + माप(*fbuf));
  अगर(fbuf)
    fbuf->len = len;
  वापस fbuf;
}

CURLcode Curl_multi_xfer_buf_borrow(काष्ठा Curl_easy *तथ्य,
                                    अक्षर **pbuf, माप_प्रकार *pbuflen)
{
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->multi);
  *pbuf = शून्य;
  *pbuflen = 0;
  अगर(!तथ्य->multi) {
    failf(तथ्य, "transfer has no multi handle");
    वापस CURLE_FAILED_INIT;
  }
  अगर(!तथ्य->set.बफर_आकार) {
    failf(तथ्य, "transfer buffer size is 0");
    वापस CURLE_FAILED_INIT;
  }
  अगर(तथ्य->multi->xfer_buf_borrowed) {
    failf(तथ्य, "attempt to borrow xfer_buf when already borrowed");
    वापस CURLE_AGAIN;
  }

  अगर(तथ्य->multi->xfer_buf &&
     तथ्य->set.बफर_आकार > तथ्य->multi->xfer_buf->len) {
    /* not large enough, get a new one */
    curlx_safefree(तथ्य->multi->xfer_buf);
  }

  अगर(!तथ्य->multi->xfer_buf) {
    तथ्य->multi->xfer_buf =
      fixed_buf_create(curlx_uitouz(तथ्य->set.बफर_आकार));
    अगर(!तथ्य->multi->xfer_buf) {
      failf(तथ्य, "could not allocate xfer_buf of %u bytes",
            तथ्य->set.बफर_आकार);
      वापस CURLE_OUT_OF_MEMORY;
    }
  }

  तथ्य->multi->xfer_buf_borrowed = TRUE;
  *pbuf = तथ्य->multi->xfer_buf->तथ्य;
  *pbuflen = तथ्य->multi->xfer_buf->len;
  वापस CURLE_OK;
}

व्योम Curl_multi_xfer_buf_release(काष्ठा Curl_easy *तथ्य, अक्षर *buf)
{
  (व्योम)buf;
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->multi);
  DEBUGASSERT(!buf || (तथ्य->multi->xfer_buf &&
                       तथ्य->multi->xfer_buf->तथ्य == buf));
  तथ्य->multi->xfer_buf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_ulbuf_borrow(काष्ठा Curl_easy *तथ्य,
                                      अक्षर **pbuf, माप_प्रकार *pbuflen)
{
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->multi);
  *pbuf = शून्य;
  *pbuflen = 0;
  अगर(!तथ्य->multi) {
    failf(तथ्य, "transfer has no multi handle");
    वापस CURLE_FAILED_INIT;
  }
  अगर(तथ्य->multi->xfer_ulbuf_borrowed) {
    failf(तथ्य, "attempt to borrow xfer_ulbuf when already borrowed");
    वापस CURLE_AGAIN;
  }

  अगर(तथ्य->multi->xfer_ulbuf &&
     तथ्य->set.upload_बफर_आकार > तथ्य->multi->xfer_ulbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(तथ्य->multi->xfer_ulbuf);
  }

  अगर(!तथ्य->multi->xfer_ulbuf) {
    तथ्य->multi->xfer_ulbuf =
      fixed_buf_create(curlx_uitouz(तथ्य->set.upload_बफर_आकार));
    अगर(!तथ्य->multi->xfer_ulbuf) {
      failf(तथ्य, "could not allocate xfer_ulbuf of %u bytes",
            तथ्य->set.upload_बफर_आकार);
      वापस CURLE_OUT_OF_MEMORY;
    }
  }

  तथ्य->multi->xfer_ulbuf_borrowed = TRUE;
  *pbuf = तथ्य->multi->xfer_ulbuf->तथ्य;
  *pbuflen = तथ्य->multi->xfer_ulbuf->len;
  वापस CURLE_OK;
}

व्योम Curl_multi_xfer_ulbuf_release(काष्ठा Curl_easy *तथ्य, अक्षर *buf)
{
  (व्योम)buf;
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(तथ्य->multi);
  DEBUGASSERT(!buf || (तथ्य->multi->xfer_ulbuf &&
                       तथ्य->multi->xfer_ulbuf->तथ्य == buf));
  तथ्य->multi->xfer_ulbuf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_sockbuf_borrow(काष्ठा Curl_easy *तथ्य,
                                        माप_प्रकार blen, अक्षर **pbuf)
{
  DEBUGASSERT(तथ्य);
  *pbuf = शून्य;
  अगर(!तथ्य->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    *pbuf = curlx_malloc(blen);
    वापस *pbuf ? CURLE_OK : CURLE_OUT_OF_MEMORY;
  }
  अगर(तथ्य->multi->xfer_sockbuf_borrowed) {
    failf(तथ्य, "attempt to borrow xfer_sockbuf when already borrowed");
    वापस CURLE_AGAIN;
  }

  अगर(तथ्य->multi->xfer_sockbuf && blen > तथ्य->multi->xfer_sockbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(तथ्य->multi->xfer_sockbuf);
  }

  अगर(!तथ्य->multi->xfer_sockbuf) {
    तथ्य->multi->xfer_sockbuf = fixed_buf_create(blen);
    अगर(!तथ्य->multi->xfer_sockbuf) {
      failf(तथ्य, "could not allocate xfer_sockbuf of %zu bytes", blen);
      वापस CURLE_OUT_OF_MEMORY;
    }
  }

  तथ्य->multi->xfer_sockbuf_borrowed = TRUE;
  *pbuf = तथ्य->multi->xfer_sockbuf->तथ्य;
  वापस CURLE_OK;
}

व्योम Curl_multi_xfer_sockbuf_release(काष्ठा Curl_easy *तथ्य, अक्षर *buf)
{
  DEBUGASSERT(तथ्य);
  अगर(!तथ्य->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    curlx_free(buf);
  }
  अन्यथा {
    DEBUGASSERT(!buf || (तथ्य->multi->xfer_sockbuf &&
                         तथ्य->multi->xfer_sockbuf->तथ्य == buf));
    तथ्य->multi->xfer_sockbuf_borrowed = FALSE;
  }
}

अटल व्योम multi_xfer_bufs_free(काष्ठा Curl_multi *multi)
{
  DEBUGASSERT(multi);
  curlx_safefree(multi->xfer_buf);
  multi->xfer_buf_borrowed = FALSE;
  curlx_safefree(multi->xfer_ulbuf);
  multi->xfer_ulbuf_borrowed = FALSE;
  curlx_safefree(multi->xfer_sockbuf);
  multi->xfer_sockbuf_borrowed = FALSE;
}

काष्ठा Curl_easy *Curl_multi_get_easy(काष्ठा Curl_multi *multi,
                                      uint32_t mid)
{
  काष्ठा Curl_easy *तथ्य = Curl_uint32_tbl_get(&multi->xfers, mid);
  अगर(GOOD_EASY_HANDLE(तथ्य))
    वापस तथ्य;
  CURL_TRC_M(multi->admin, "invalid easy handle in xfer table for mid=%u",
             mid);
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  वापस शून्य;
}

bool Curl_multi_knows_easy(काष्ठा Curl_multi *multi, काष्ठा Curl_easy *तथ्य)
{
  वापस Curl_uint32_tbl_get(&multi->xfers, तथ्य->mid) == तथ्य;
}

अटल uint32_t multi_xfers_running(काष्ठा Curl_multi *multi)
{
  अगर(!multi) {
    DEBUGASSERT(0);
    वापस 0;
  }
  वापस multi->xfers_alive;
}

uint32_t Curl_multi_xfers_attached(काष्ठा Curl_multi *multi)
{
  अगर(!multi || !multi->admin) {
    DEBUGASSERT(0);
    वापस 0;
  }
  /* Discount the admin handle */
  वापस Curl_uint32_tbl_गिनती(&multi->xfers) - 1;
}

व्योम Curl_multi_mark_dirty(काष्ठा Curl_easy *तथ्य)
{
  अगर(तथ्य->multi && तथ्य->mid != UINT32_MAX)
    Curl_uint32_bset_add(&तथ्य->multi->dirty, तथ्य->mid);
}

व्योम Curl_multi_clear_dirty(काष्ठा Curl_easy *तथ्य)
{
  अगर(तथ्य->multi && तथ्य->mid != UINT32_MAX)
    Curl_uint32_bset_remove(&तथ्य->multi->dirty, तथ्य->mid);
}

CURLMcode curl_multi_notify_enable(CURLM *m, अचिन्हित पूर्णांक notification)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_notify_enable, &mresult)) {
    mresult = Curl_mntfy_enable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

CURLMcode curl_multi_notify_disable(CURLM *m, अचिन्हित पूर्णांक notification)
{
  काष्ठा Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  अगर(CURL_MAPI_ENTER(&guard, m, multi_notify_disable, &mresult)) {
    mresult = Curl_mntfy_disable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  वापस mresult;
}

#अगर_घोषित DEBUGBUILD
अटल व्योम multi_xfer_dump(काष्ठा Curl_multi *multi, uint32_t mid,
                            व्योम *entry)
{
  काष्ठा Curl_easy *तथ्य = entry;

  (व्योम)multi;
  अगर(!तथ्य) {
    curl_mfprintf(मानक_त्रुटि, "mid=%u, entry=NULL, bug in xfer table?\n", mid);
  }
  अन्यथा {
    curl_mfprintf(मानक_त्रुटि, "mid=%u, magic=%s, p=%p, id=%" FMT_OFF_T
                  ", url=%s\n",
                  mid,
                  (तथ्य->magic == CURLEASY_MAGIC_संख्या) ? "GOOD" : "BAD!",
                  (व्योम *)तथ्य, तथ्य->id, Curl_bufref_ptr(&तथ्य->state.url));
  }
}

अटल व्योम multi_xfer_tbl_dump(काष्ठा Curl_multi *multi)
{
  uint32_t mid;
  व्योम *entry;
  curl_mfprintf(मानक_त्रुटि, "=== multi xfer table (count=%u, capacity=%u\n",
                Curl_uint32_tbl_गिनती(&multi->xfers),
                Curl_uint32_tbl_capacity(&multi->xfers));
  अगर(Curl_uint32_tbl_first(&multi->xfers, &mid, &entry)) {
    multi_xfer_dump(multi, mid, entry);
    जबतक(Curl_uint32_tbl_अग्र(&multi->xfers, mid, &mid, &entry))
      multi_xfer_dump(multi, mid, entry);
  }
  curl_mfprintf(मानक_त्रुटि, "===\n");
  ख_साफ(मानक_त्रुटि);
}
#पूर्ण_अगर /* DEBUGBUILD */
