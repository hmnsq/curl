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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "curl_share.h"
#अन्तर्भाव "psl.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "multi_ev.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "curlx/wait.h"
#अन्तर्भाव "conncache.h"
#अन्तर्भाव "multihandle.h"
#अन्तर्भाव "sigpipe.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "http2.h"
#अन्तर्भाव "socketpair.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curlx/strparse.h"

/* initial multi->xfers table size for a full multi */
#परिभाषय CURL_XFER_सारणी_आकार 128

/* CURL_SOCKET_HASH_TABLE_SIZE should be a prime number. Increasing it from 97
   to 911 takes on a 32-bit machine 4 x 804 = 3211 more bytes. Still, every
   curl handle takes 6K memory, therefore this 3K are not significant. */
#यदि_अपरिभाषितम् CURL_SOCKET_HASH_सारणी_आकार
#परिभाषय CURL_SOCKET_HASH_सारणी_आकार 911
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_CONNECTION_HASH_आकार
#परिभाषय CURL_CONNECTION_HASH_आकार 97
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DNS_HASH_आकार
#परिभाषय CURL_DNS_HASH_आकार 71
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_TLS_SESSION_आकार
#परिभाषय CURL_TLS_SESSION_आकार 25
#यदि_अन्तः

स्थिर शून्य move_pending_गन्तव्यम्_connect(संरचना Curl_multi *multi,
                                    संरचना Curl_easy *दत्त);
स्थिर CURLMcode सङ्कलय_अग्र_timeout(ध्रुव संरचना curltime *pnow,
                                  संरचना Curl_multi *multi,
                                  संरचना Curl_easy *दत्त);
स्थिर शून्य multi_timeout(संरचना Curl_multi *multi,
                          timediff_t *pexire_विस्थापनम्_us,
                          पूर्णाङ्क *timeout_ms);
स्थिर शून्य multi_कालसारणी_pending(संरचना Curl_multi *multi);
स्थिर शून्य multi_xfer_bufs_free(संरचना Curl_multi *multi);
#यदि_परिभाषितम् DEBUGBUILD
स्थिर शून्य multi_xfer_tbl_dump(संरचना Curl_multi *multi);
#यदि_अन्तः
/* Get the # of transfers current in process/pending. */
स्थिर uint32_t multi_xfers_running(संरचना Curl_multi *multi);

/* function pointer called once when entering a state */
प्रकारनिर्देश शून्य (*mstate_enter_func)(संरचना Curl_easy *दत्त,
                                  CURLMstate from_state);

स्थिर शून्य mstate_enter_connect(संरचना Curl_easy *दत्त,
                                 CURLMstate from_state)
{
  (शून्य)from_state;
  Curl_init_CONNECT(दत्त);
}

स्थिर शून्य mstate_enter_did(संरचना Curl_easy *दत्त,
                             CURLMstate from_state)
{
  (शून्य)from_state;
  दत्त->req.chunk = FALSE;
  Curl_pgrsTime(दत्त, TIMER_PRETRANSFER);
  यदि(!CURL_REQ_WANT_SEND(दत्त))
    Curl_pgrsTime(दत्त, TIMER_POSTRANSFER);
}

स्थिर शून्य mstate_enter_समाप्तम्(संरचना Curl_easy *दत्त,
                              CURLMstate from_state)
{
  (शून्य)from_state;
  CURLM_NTFY(दत्त, CURLMNOTIFY_EASY_समाप्तम्);
}

स्थिर शून्य mstate_enter_completed(संरचना Curl_easy *दत्त,
                                   CURLMstate from_state)
{
  /* we sometimes directly jump to COMPLETED, trigger things
   * we then missed. */
  यदि(from_state < MSTATE_DID) {
    Curl_pgrsTime(दत्त, TIMER_PRETRANSFER);
    Curl_pgrsTime(दत्त, TIMER_POSTRANSFER);
    Curl_pgrsTime(दत्त, TIMER_STARTTRANSFER);
  }
  /* multi_done() should have run and detached any connection already */
  DEBUGASSERT(!दत्त->conn);
  Curl_pgrsCompleted(दत्त);
  यदि(from_state < MSTATE_समाप्तम्)
    CURLM_NTFY(दत्त, CURLMNOTIFY_EASY_समाप्तम्);
  /* changing to COMPLETED means it is in process and needs to go */
  DEBUGASSERT(Curl_uint32_bset_अन्तर्भवति(&दत्त->multi->process, दत्त->mid));
  Curl_uint32_bset_remove(&दत्त->multi->process, दत्त->mid);
  Curl_uint32_bset_remove(&दत्त->multi->pending, दत्त->mid); /* to be sure */

  यदि(Curl_uint32_bset_रिक्तानि(&दत्त->multi->process)) {
    /* free the transfer buffer when we have no more active transfers */
    multi_xfer_bufs_free(दत्त->multi);
  }
  /* Important: reset the conn pointer so that we do not point to memory
     that could be freed anytime */
  Curl_detach_connection(दत्त);
  Curl_expire_clear_all(दत्त); /* stop all timers */
}

/* always use this function to change state, to make debugging easier */
स्थिर शून्य mstate(संरचना Curl_easy *दत्त, CURLMstate state
#यदि_परिभाषितम् DEBUGBUILD
                   , पूर्णाङ्क lineno
#यदि_अन्तः
)
{
  CURLMstate oldstate = दत्त->mstate;
  स्थिर ध्रुव mstate_enter_func state_enter[MSTATE_चरमः] = {
    रिक्त,                      /* INIT */
    रिक्त,                      /* PENDING */
    रिक्त,                      /*सज्जता*/
    mstate_enter_connect,      /* CONNECT */
    रिक्त,                      /* CONNECTING */
    रिक्त,                      /* PROTOCONNECT */
    रिक्त,                      /* PROTOCONNECTING */
    रिक्त,                      /* DO */
    रिक्त,                      /* DOING */
    रिक्त,                      /* DOING_MORE */
    mstate_enter_did,          /* DID */
    रिक्त,                      /* PERFORMING */
    रिक्त,                      /* RATELIMITING */
    mstate_enter_समाप्तम्,         /* DONE */
    mstate_enter_completed,    /* COMPLETED */
    रिक्त                       /* MSGSENT */
  };

  यदि(oldstate == state)
    /* do not bother when the new state is the same as the old state */
    प्रत्यागम;

#यदि_परिभाषितम् DEBUGBUILD
  NOVERBOSE((शून्य)lineno);
  CURL_TRC_M(दत्त, "-> [%s] (line %d)", CURL_MSTATE_NAME(state), lineno);
#अन्यथा
  CURL_TRC_M(दत्त, "-> [%s]", CURL_MSTATE_NAME(state));
#यदि_अन्तः

  /* really switching state */
  दत्त->mstate = state;
  यदि(state_enter[state])
    state_enter[state](दत्त, oldstate);
}

#यदि_अपरिभाषितम् DEBUGBUILD
#परिभाषय multistate(x, y) mstate(x, y)
#अन्यथा
#परिभाषय multistate(x, y) mstate(x, y, __LINE__)
#यदि_अन्तः

/* multi->proto_hash destructor. Should never be called as elements
 * MUST be added with their own destructor */
स्थिर शून्य ph_freeentry(शून्य *p)
{
  (शून्य)p;
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
स्थिर शून्य multi_addmsg(संरचना Curl_multi *multi, संरचना Curl_easy *दत्त)
{
  यदि(Curl_uint32_bset_रिक्तानि(&multi->msgsent))
    CURLM_NTFY(multi->admin, CURLMNOTIFY_INFO_READ);
  Curl_uint32_bset_सङ्कलय(&multi->msgsent, दत्त->mid);
}

स्थिर शून्य multi_timeouts_init(संरचना Curl_easy *दत्त);

संरचना Curl_multi *Curl_multi_handle(uint32_t xfer_सारणी_आकार,
                                     size_t ev_hashsize,  /* event hash */
                                     size_t chashsize, /* connection hash */
                                     size_t dnssize,   /* dns hash */
                                     size_t sesssize)  /* TLS session cache */
{
  संरचना curltime created;
  संरचना Curl_multi *multi = curlx_calloc(1, परिमाण(संरचना Curl_multi));

  यदि(!multi)
    प्रत्यागम रिक्त;

  multi->magic = CURLMULTI_MAGIC_सङ्ख्या;

  /* Initialisation order is important here!
   * easy_init() does a lazy check on curl_global_init() which sets
   * up platform specific things we need. For example calling curlx_pnow()
   * before this is not safe. */
  multi->admin = curl_easy_init();
  यदि(!multi->admin) {
    curlx_free(multi);
    प्रत्यागम रिक्त;
  }
  multi->admin->multi = multi;
  multi->admin->state.आन्तरिकम् = TRUE;

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
  Curl_hash_init(&multi->proto_hash, 23, CURL_HASH_TYPE_अष्टकानि, ph_freeentry);
  Curl_cshutdn_init(&multi->cshutdn);

  multi->multiplexing = TRUE;
  multi->max_concurrent_streams = 100;
#यदि_परिभाषितम् ENABLE_WAKEUP
  multi->wakeup_pair[0] = CURL_SOCKET_BAD;
  multi->wakeup_pair[1] = CURL_SOCKET_BAD;
#यदि_अन्तः
#यदि_परिभाषितम् ENABLE_आन्तरिकम्_WAKEUP
  multi->wakeup_आन्तरिकम्[0] = CURL_SOCKET_BAD;
  multi->wakeup_आन्तरिकम्[1] = CURL_SOCKET_BAD;
#यदि_अन्तः

  यदि(Curl_uint32_bset_resize(&multi->process, xfer_सारणी_आकार) ||
     Curl_uint32_bset_resize(&multi->pending, xfer_सारणी_आकार) ||
     Curl_uint32_bset_resize(&multi->dirty, xfer_सारणी_आकार) ||
     Curl_uint32_bset_resize(&multi->msgsent, xfer_सारणी_आकार) ||
     Curl_uint32_tbl_resize(&multi->xfers, xfer_सारणी_आकार))
    गच्छ error;

#यदि_परिभाषितम् DEBUGBUILD
  {
    ध्रुव वर्ण *p;
    यदि(getenv("CURL_DEBUG"))
      multi->admin->समुच्चयः.verbose = TRUE;
    p = getenv("CURL_DBG_MAX_TOTAL_CONNECTIONS");
    यदि(p) {
      curl_off_t l;
      यदि(!curlx_str_सङ्ख्या(&p, &l, UINT32_MAX)) {
        multi->max_योग_connections = (uint32_t)l;
      }
    }
  }
#यदि_अन्तः
  Curl_uint32_tbl_सङ्कलय(&multi->xfers, multi->admin, &multi->admin->mid);
  Curl_uint32_bset_सङ्कलय(&multi->process, multi->admin->mid);

  Curl_cpool_init(&multi->cpool, रिक्त, chashsize);

#यदि_परिभाषितम् USE_SSL
  यदि(Curl_ssl_scache_create(sesssize, 2, &multi->ssl_scache))
    गच्छ error;
#अन्यथा
  (शून्य)sesssize;
#यदि_अन्तः

#यदि_परिभाषितम् USE_WINSOCK
  multi->wsa_event = WSACreateEvent();
  यदि(multi->wsa_event == WSA_INVALID_EVENT)
    गच्छ error;
#यदि_अन्तः
#यदि_परिभाषितम् ENABLE_WAKEUP
  /* When enabled, rely on this to work. We ignore this in previous
   * versions, but that seems an unnecessary complication. */
  यदि(Curl_wakeup_init(multi->wakeup_pair, TRUE) < 0)
    गच्छ error;
#यदि_अन्तः
#यदि_परिभाषितम् ENABLE_आन्तरिकम्_WAKEUP
  यदि(Curl_wakeup_init(multi->wakeup_आन्तरिकम्, TRUE) < 0)
    गच्छ error;
#यदि_अन्तः

  यदि(Curl_probeipv6(multi))
    गच्छ error;

#यदि_परिभाषितम् USE_RESOLV_THREADED
  यदि(xfer_सारणी_आकार < CURL_XFER_सारणी_आकार) { /* easy multi */
    यदि(Curl_async_thrdd_multi_init(multi, 0, 2, 10))
      गच्छ error;
  }
  अन्यथा { /* real multi handle */
    यदि(Curl_async_thrdd_multi_init(multi, 0, 20, 2000))
      गच्छ error;
  }
#यदि_अन्तः

  प्रत्यागम multi;

error:

#यदि_परिभाषितम् USE_RESOLV_THREADED
  Curl_async_thrdd_multi_destroy(multi, TRUE);
#यदि_अन्तः
  Curl_multi_ev_cleanup(multi);
  Curl_hash_destroy(&multi->proto_hash);
  Curl_dnscache_destroy(&multi->dnscache);
  Curl_cpool_destroy(&multi->cpool, multi->admin);
  Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
#यदि_परिभाषितम् USE_SSL
  Curl_ssl_scache_destroy(multi->ssl_scache);
#यदि_अन्तः
  यदि(multi->admin) {
    Curl_multi_ev_xfer_समाप्तम्(multi, multi->admin);
    multi->admin->multi = रिक्त;
    Curl_close(&multi->admin);
  }
  Curl_mntfy_cleanup(multi);

  Curl_uint32_bset_destroy(&multi->process);
  Curl_uint32_bset_destroy(&multi->dirty);
  Curl_uint32_bset_destroy(&multi->pending);
  Curl_uint32_bset_destroy(&multi->msgsent);
  Curl_uint32_tbl_destroy(&multi->xfers);
#यदि_परिभाषितम् ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#यदि_अन्तः
#यदि_परिभाषितम् ENABLE_आन्तरिकम्_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_आन्तरिकम्);
#यदि_अन्तः

  curlx_free(multi);
  प्रत्यागम रिक्त;
}

CURLM *curl_multi_init(शून्य)
{
  प्रत्यागम Curl_multi_handle(CURL_XFER_सारणी_आकार,
                           CURL_SOCKET_HASH_सारणी_आकार,
                           CURL_CONNECTION_HASH_आकार,
                           CURL_DNS_HASH_आकार,
                           CURL_TLS_SESSION_आकार);
}

#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
स्थिर शून्य multi_warn_debug(संरचना Curl_multi *multi, संरचना Curl_easy *दत्त)
{
  यदि(!multi->warned) {
    infof(दत्त, "!!! WARNING !!!");
    infof(दत्त, "This is a debug build of libcurl, "
                "do not use in production.");
    multi->warned = TRUE;
  }
}
#अन्यथा
#परिभाषय multi_warn_debug(x, y) Curl_nop_stmt
#यदि_अन्तः

bool Curl_is_connecting(संरचना Curl_easy *दत्त)
{
  प्रत्यागम दत्त->mstate < MSTATE_DO;
}

स्थिर CURLMcode multi_assess_wakeup(संरचना Curl_multi *multi)
{
#यदि_परिभाषितम् ENABLE_आन्तरिकम्_WAKEUP
  यदि(multi->socket_cb)
    प्रत्यागम Curl_multi_ev_assess_xfer(multi, multi->admin);
#अन्यथा
  (शून्य)multi;
#यदि_अन्तः
  प्रत्यागम CURLM_OK;
}

स्थिर CURLMcode multi_xfers_सङ्कलय(संरचना Curl_multi *multi,
                                 संरचना Curl_easy *दत्त)
{
  uint32_t क्षमता = Curl_uint32_tbl_क्षमता(&multi->xfers);
  uint32_t new_आकार = 0;
  /* Prepare to make this into a CURLMOPT_MAX_TRANSFERS, because some
   * applications may want to prevent a run-away of their memory use. */
  /* UINT_MAX is our "invalid" id, do not let the table grow up to that. */
  ध्रुव uint32_t max_क्षमता = UINT_MAX - 1;

  यदि(क्षमता < max_क्षमता) {
    /* We want `multi->xfers` to have "sufficient" free rows, so that we do
     * not have to reuse the `mid` from a removed easy right away.
     * Check if an 8th of the capacity is still free */
    uint32_t प्रयुक्तम् = Curl_uint32_tbl_गणनम्(&multi->xfers);
    uint32_t अप्रयुक्तम् = क्षमता - प्रयुक्तम्;
    uint32_t min_अप्रयुक्तम् = CURLMAX(क्षमता >> 3, 4);
    यदि(अप्रयुक्तम् < min_अप्रयुक्तम्) {
      /* Grow by 50% of current capacity, in range of [128, 2048],
       * which means the table grows max by 16kb on 64-bit arch. */
      uint32_t growth = CURLMIN(CURLMAX(क्षमता >> 1, 128), 2048);
      /* Make sure the uint arithmetic here works on the corner
       * cases where we are close to max_capacity or UINT_MAX */
      यदि((max_क्षमता - growth) <= क्षमता)
        new_आकार = max_क्षमता;
      अन्यथा
        new_आकार = क्षमता + growth;
    }
  }

  यदि(new_आकार > क्षमता) {
    /* Grow the bitsets first. Should one fail, we do not need
     * to downsize the already resized ones. The sets continue
     * to work properly when larger than the table, but not
     * the other way around. */
    CURL_TRC_M(दत्त, "increasing xfer table size to %u", new_आकार);
    यदि(Curl_uint32_bset_resize(&multi->process, new_आकार) ||
       Curl_uint32_bset_resize(&multi->dirty, new_आकार) ||
       Curl_uint32_bset_resize(&multi->pending, new_आकार) ||
       Curl_uint32_bset_resize(&multi->msgsent, new_आकार) ||
       Curl_uint32_tbl_resize(&multi->xfers, new_आकार))
      प्रत्यागम CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  /* Insert the easy into the table now */
  यदि(!Curl_uint32_tbl_सङ्कलय(&multi->xfers, दत्त, &दत्त->mid)) {
    /* MUST only happen when table is full */
    DEBUGASSERT(Curl_uint32_tbl_क्षमता(&multi->xfers) <=
                Curl_uint32_tbl_गणनम्(&multi->xfers));
    प्रत्यागम CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  प्रत्यागम CURLM_OK;
}

CURLMcode Curl_multi_सङ्कलय_handle(संरचना Curl_multi *multi,
                                संरचना Curl_easy *दत्त)
{
  CURLMcode mresult;

  /* Prevent users from adding same easy handle more than once and prevent
     adding to more than one multi stack */
  यदि(दत्त->multi)
    प्रत्यागम CURLM_ADDED_ALREADY;

  यदि(multi->dead) {
    /* a "dead" handle cannot get added transfers while any existing easy
       handles are still alive - but if there are none alive anymore, it is
       fine to start over and unmark the "deadness" of this handle.
       This means only the admin handle MUST be present. */
    यदि((Curl_uint32_tbl_गणनम्(&multi->xfers) != 1) ||
       !Curl_uint32_tbl_अन्तर्भवति(&multi->xfers, 0))
      प्रत्यागम CURLM_ABORTED_BY_CALLBACK;
    multi->dead = FALSE;
    Curl_uint32_bset_clear(&multi->process);
    Curl_uint32_bset_clear(&multi->dirty);
    Curl_uint32_bset_clear(&multi->pending);
    Curl_uint32_bset_clear(&multi->msgsent);
  }

  यदि(दत्त->multi_easy) {
    /* if this easy handle was previously used for curl_easy_perform(), there
       is a private multi handle here that we can kill */
    curl_multi_cleanup(दत्त->multi_easy);
    दत्त->multi_easy = रिक्त;
  }

  /* Insert the easy into the multi->xfers table, assigning it a `mid`. */
  यदि(multi_xfers_सङ्कलय(multi, दत्त))
    प्रत्यागम CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Initialize timeouts for this handle */
  multi_timeouts_init(दत्त);

  /*
   * No failure allowed in this function beyond this point. No modification
   * of easy nor multi handle allowed before this except for potential
   * multi's connection pool growing which will not be undone in this
   * function no matter what.
   */
  यदि(दत्त->समुच्चयः.errorbuffer)
    दत्त->समुच्चयः.errorbuffer[0] = 0;

  दत्त->state.os_errno = 0;

  /* make the Curl_easy refer back to this multi handle - before
     Curl_expire() is called. */
  दत्त->multi = multi;

  /* set the easy handle */
  multistate(दत्त, MSTATE_INIT);
  /* not yet passed INIT state */
  दत्त->state.really_alive = FALSE;

#यदि_परिभाषितम् USE_LIBPSL
  /* Do the same for PSL. */
  यदि(दत्त->share && (दत्त->share->specifier & (1 << CURL_तालकम्_दत्त_PSL)))
    दत्त->psl = &दत्त->share->psl;
  अन्यथा
    दत्त->psl = &multi->psl;
#यदि_अन्तः

  /* add the easy handle to the process set */
  Curl_uint32_bset_सङ्कलय(&multi->process, दत्त->mid);
  ++multi->xfers_alive;
  ++multi->xfers_योग_ever;

  Curl_cpool_xfer_init(दत्त);
  multi_warn_debug(multi, दत्त);

  /* Make sure the new handle will run */
  Curl_multi_mark_dirty(दत्त);

  /* Necessary in event based processing, where dirty handles trigger
   * a timeout callback invocation. */
  mresult = Curl_update_timer(multi);
  यदि(mresult) {
    /* undo the additions above */
    Curl_uint32_bset_remove(&multi->process, दत्त->mid);
    Curl_uint32_bset_remove(&multi->dirty, दत्त->mid);
    --multi->xfers_alive;
    दत्त->multi = रिक्त; /* not anymore */
    Curl_uint32_tbl_remove(&multi->xfers, दत्त->mid);
    दत्त->mid = UINT32_MAX;
    प्रत्यागम mresult;
  }

  /* The admin handle only ever has default timeouts set. To improve the
     state somewhat we clone the timeouts from each added handle so that the
     admin handle always has the same timeouts as the most recently added
     easy handle. */
  multi->admin->समुच्चयः.timeout = दत्त->समुच्चयः.timeout;
  multi->admin->समुच्चयः.server_response_timeout =
    दत्त->समुच्चयः.server_response_timeout;
  multi->admin->समुच्चयः.no_signal = दत्त->समुच्चयः.no_signal;

  CURL_TRC_M(दत्त, "added to multi, mid=%u, running=%u, total=%u",
             दत्त->mid, multi_xfers_running(multi),
             Curl_uint32_tbl_गणनम्(&multi->xfers));
  प्रत्यागम CURLM_OK;
}

CURLMcode curl_multi_सङ्कलय_handle(CURLM *m, CURL *curl)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_सङ्कलय_handle, &mresult)) {
    संरचना Curl_easy *दत्त = curl;
    /* Verify that we got a somewhat good easy handle too */
    यदि(!GOOD_EASY_HANDLE(दत्त) || दत्त->state.आन्तरिकम्)
      mresult = CURLM_BAD_EASY_HANDLE;
    अन्यथा
      mresult = Curl_multi_सङ्कलय_handle(m, दत्त);
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

संरचना multi_समाप्तम्_ctx {
  BIT(premature);
};

स्थिर bool multi_conn_should_close(संरचना connectdata *conn,
                                    संरचना Curl_easy *दत्त,
                                    bool premature)
{
  /* if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of everything else. */
  यदि(conn->द्वयङ्काः.close)
    प्रत्यागम TRUE;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM/NEGOTIATE authentication handshake. */
  यदि(दत्त->समुच्चयः.reuse_forbid
#यदि_परिभाषितम् USE_NTLM
     && !(conn->http_ntlm_state == NTLMSTATE_TYPE2 ||
          conn->proxy_ntlm_state == NTLMSTATE_TYPE2)
#यदि_अन्तः
#यदि_परिभाषितम् USE_SPNEGO
     && !(conn->http_negotiate_state == CURL_NEGO_AUTH_RECV ||
          conn->proxy_negotiate_state == CURL_NEGO_AUTH_RECV)
#यदि_अन्तः
    )
    प्रत्यागम TRUE;

  /* Unless this connection is for a "connect-only" transfer, it
   * needs to be closed if the protocol handler does not support reuse. */
  यदि(!दत्त->समुच्चयः.connect_only && conn->scheme &&
     !(conn->scheme->flags & PROTOPT_CONN_REUSE))
    प्रत्यागम TRUE;

  /* if premature is TRUE, it means this connection was said to be DONE before
     the entire request operation is complete and thus we cannot know in what
     state it is for reusing, so we are forced to close it. In a perfect world
     we can add code that keep track of if we really must close it here or not,
     but currently we have no such detail knowledge. */
  यदि(premature && !Curl_conn_is_multiplex(conn, FIRSTSOCKET))
    प्रत्यागम TRUE;

  प्रत्यागम FALSE;
}

स्थिर cpool_do_परिणाम multi_समाप्तम्_locked(संरचना Curl_easy *दत्त,
                                         संरचना connectdata *conn,
                                         शून्य *userdata,
                                         ध्रुव संरचना curltime *pnow)
{
  संरचना multi_समाप्तम्_ctx *mdctx = userdata;

  (शून्य)pnow;
  CURL_TRC_M(दत्त, "multi_done_locked, in use=%u", conn->attached_xfers);
  यदि(CONN_INUSE(conn)) {
    /* Stop if still used. */
    CURL_TRC_M(दत्त, "Connection still in use %u, no more multi_done now!",
               conn->attached_xfers);
    प्रत्यागम CPOOL_DO_KEEP;
  }

  यदि(multi_conn_should_close(conn, दत्त, (bool)mdctx->premature)) {
    CURL_TRC_M(दत्त, "multi_done, terminating conn #%" FMT_OFF_T " to %s:%u, "
               "forbid=%d, close=%d, premature=%d, conn_multiplex=%d",
               conn->connection_id, conn->origin->user_hostname,
               conn->origin->पत्तनम्,
               दत्त->समुच्चयः.reuse_forbid, conn->द्वयङ्काः.close, mdctx->premature,
               Curl_conn_is_multiplex(conn, FIRSTSOCKET));
    connclose(conn);
    प्रत्यागम mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  अन्यथा यदि(!Curl_conn_get_max_concurrent(दत्त, conn, FIRSTSOCKET)) {
    CURL_TRC_M(दत्त, "multi_done, conn #%" FMT_OFF_T " to %s:%u was shutdown"
               " by server, not reusing", conn->connection_id,
               conn->origin->user_hostname, conn->origin->पत्तनम्);
    connclose(conn);
    प्रत्यागम mdctx->premature ? CPOOL_DO_TERMINATE : CPOOL_DO_CLOSE;
  }
  अन्यथा {
    प्रत्यागम CPOOL_DO_IDLE;
  }
}

स्थिर CURLcode multi_समाप्तम्(संरचना Curl_easy *दत्त,
                           CURLcode स्थितिः,  /* an error if this is called
                                                after an error was detected */
                           bool premature)
{
  CURLcode परिणाम;
  संरचना connectdata *conn = दत्त->conn;
  ध्रुव संरचना curltime *pnow = Curl_pgrs_now(दत्त);

  CURL_TRC_M(दत्त, "multi_done: status: %d prem: %d done: %d",
             (पूर्णाङ्क)स्थितिः, (पूर्णाङ्क)premature, दत्त->state.समाप्तम्);

  यदि(दत्त->state.समाप्तम्) {
    /* Stop if multi_done() has already been called */
    प्रत्यागम CURLE_OK;
  }
  दत्त->state.समाप्तम् = TRUE; /* called now! */

  /* Shut down any ongoing async resolver operation. */
  Curl_resolv_shutdown_all(दत्त);

  /* Cleanup possible redirect junk */
  curlx_safefree(दत्त->req.newurl);
  curlx_safefree(दत्त->req.location);

  विकल्प(स्थितिः) {
  पक्ष CURLE_ABORTED_BY_CALLBACK:
  पक्ष CURLE_READ_ERROR:
  पक्ष CURLE_WRITE_ERROR:
    /* When we are aborted due to a callback return code it has to be counted
       as premature as there is trouble ahead if we do not. We have many
       callbacks and protocols work differently, we could potentially do this
       more fine-grained in the future. */
    premature = TRUE;
    FALLTHROUGH();
  स्वभाव:
    भङ्ग;
  }

  /* this calls the protocol-specific function pointer previously set */
  यदि(conn && conn->scheme->सततम्->समाप्तम् && (दत्त->mstate >= MSTATE_PROTOCONNECT))
    परिणाम = conn->scheme->सततम्->समाप्तम्(दत्त, स्थितिः, premature);
  अन्यथा
    परिणाम = स्थितिः;

  यदि(दत्त->mstate > MSTATE_CONNECTING &&
     (परिणाम != CURLE_ABORTED_BY_CALLBACK)) {
    /* avoid this if
     * - the transfer has not connected
     * - we already aborted by callback to avoid this calling another callback
     */
    पूर्णाङ्क rc = Curl_pgrs_समाप्तम्(दत्त, pnow);
    यदि(!परिणाम && rc)
      परिणाम = CURLE_ABORTED_BY_CALLBACK;
  }

  /* Make sure that transfer client writes are really done now. */
  परिणाम = Curl_1st_fatal(परिणाम, Curl_xfer_write_समाप्तम्(दत्त, premature));

  यदि(!परिणाम)
    परिणाम = Curl_req_समाप्तम्(&दत्त->req, दत्त, premature);

  यदि(conn) {
    /* Detach connection and decide what to do with it. */
    संरचना multi_समाप्तम्_ctx mdctx;

    /* Inform connection filters that this transfer is done and they
     * need to give up any references to `data`. */
    Curl_conn_ev_दत्त_समाप्तम्(दत्त, premature);
    Curl_detach_connection(दत्त);
    Curl_dnscache_prune(दत्त, pnow);

    स्मृतिं_पूरय(&mdctx, 0, परिमाण(mdctx));
    mdctx.premature = premature;
    Curl_cpool_return(दत्त, conn, multi_समाप्तम्_locked, &mdctx, pnow);

    यदि(दत्त->multi)
      multi_कालसारणी_pending(दत्त->multi);
  }

  /* flush the netrc cache */
  Curl_netrc_cleanup(&दत्त->state.netrc);
  प्रत्यागम परिणाम;
}

CURLMcode Curl_multi_remove_handle(संरचना Curl_multi *multi,
                                   संरचना Curl_easy *दत्त)
{
  CURLMcode mresult;
  bool premature;
  uint32_t mid;

  /* Prevent users from trying to remove same easy handle more than once */
  यदि(!दत्त->multi)
    प्रत्यागम CURLM_OK; /* it is already removed so let's say it is fine! */

  /* Prevent users from trying to remove an easy handle from the wrong multi */
  यदि(दत्त->multi != multi)
    प्रत्यागम CURLM_BAD_EASY_HANDLE;

  यदि(दत्त->mid == UINT32_MAX) {
    DEBUGASSERT(0);
    प्रत्यागम CURLM_आन्तरिकम्_ERROR;
  }
  यदि(Curl_uint32_tbl_get(&multi->xfers, दत्त->mid) != दत्त) {
    DEBUGASSERT(0);
    प्रत्यागम CURLM_आन्तरिकम्_ERROR;
  }

  premature = (दत्त->mstate < MSTATE_COMPLETED);

  यदि(दत्त->conn) {
    /* If the 'state' is not INIT or COMPLETED, we might need to do something
       nice to put the easy_handle in a good known state when this returns. */
    यदि(premature && (दत्त->mstate > MSTATE_DO))
      streamclose(दत्त->conn);

    /* multi_done() clears the association between the easy handle and the
       connection.
       Note that this ignores the return code because there is
       nothing really useful to do with it anyway! */
    (शून्य)multi_समाप्तम्(दत्त, दत्त->परिणाम, premature);
  }

  /* The timer must be shut down before data->multi is set to NULL, else
     data's splaynode would remain in the splay tree after curl_easy_cleanup is
     called. Do it after multi_done() in case that sets another time! */
  Curl_expire_clear_all(दत्त);

  /* In MSGSENT, it was deducted from `multi->xfers_alive` already. */
  यदि(दत्त->mstate != MSTATE_MSGSENT)
    --multi->xfers_alive;

  यदि(दत्त->state.really_alive) {
    दत्त->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    यदि(!multi->xfers_really_alive)
      (शून्य)multi_assess_wakeup(multi);
  }

  Curl_wildcard_dtor(&दत्त->wildcard);

  दत्त->mstate = MSTATE_COMPLETED;

  /* Remove the association between the connection and the handle */
  Curl_detach_connection(दत्त);

  /* Tell event handling that this transfer is definitely going away */
  Curl_multi_ev_xfer_समाप्तम्(multi, दत्त);

  यदि(दत्त->समुच्चयः.connect_only) {
    यदि(दत्त->multi_easy) {
      यदि(दत्त->state.चरमः_conn_id != -1) {
        /* Mark any connect-only connection for closure */
        संरचना connectdata *conn;
        (शून्य)Curl_getconnectinfo(दत्त, &conn);
        यदि(conn && conn->द्वयङ्काः.connect_only)
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
      संरचना connectdata *conn;
      (शून्य)Curl_getconnectinfo(दत्त, &conn);
      यदि(conn)
        Curl_conn_close(दत्त, conn, TRUE);
    }
  }

#यदि_परिभाषितम् USE_LIBPSL
  /* Remove the PSL association. */
  यदि(दत्त->psl == &multi->psl)
    दत्त->psl = रिक्त;
#यदि_अन्तः

  /* clear the association to this multi handle */
  mid = दत्त->mid;
  DEBUGASSERT(Curl_uint32_tbl_अन्तर्भवति(&multi->xfers, mid));
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  Curl_uint32_bset_remove(&multi->process, mid);
  Curl_uint32_bset_remove(&multi->dirty, mid);
  Curl_uint32_bset_remove(&multi->pending, mid);
  Curl_uint32_bset_remove(&multi->msgsent, mid);
  दत्त->multi = रिक्त;
  दत्त->mid = UINT32_MAX;
  दत्त->master_mid = UINT32_MAX;

  /* A pending transfer *might* be able to run now. */
  multi_कालसारणी_pending(multi);
  mresult = Curl_update_timer(multi);
  यदि(mresult)
    प्रत्यागम mresult;

  mresult = multi_assess_wakeup(multi);
  यदि(mresult) {
    failf(दत्त, "error enabling wakeup listening: %d", mresult);
    प्रत्यागम mresult;
  }

  CURL_TRC_M(दत्त, "removed from multi, mid=%u, running=%u, total=%u",
             mid, multi_xfers_running(multi),
             Curl_uint32_tbl_गणनम्(&multi->xfers));
  प्रत्यागम CURLM_OK;
}

CURLMcode curl_multi_remove_handle(CURLM *m, CURL *curl)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_remove_handle, &mresult)) {
    संरचना Curl_easy *दत्त = curl;
    यदि(!GOOD_EASY_HANDLE(दत्त) || दत्त->state.आन्तरिकम्)
      mresult = CURLM_BAD_EASY_HANDLE;
    अन्यथा
      mresult = Curl_multi_remove_handle(m, दत्त);
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

/* Return TRUE if the application asked for multiplexing */
bool Curl_multiplex_wanted(ध्रुव संरचना Curl_multi *multi)
{
  प्रत्यागम multi && multi->multiplexing;
}

/*
 * Curl_detach_connection() removes the given transfer from the connection.
 *
 * This is the only function that should clear data->conn. This will
 * occasionally be called with the data->conn pointer already cleared.
 */
शून्य Curl_detach_connection(संरचना Curl_easy *दत्त)
{
  संरचना connectdata *conn = दत्त->conn;
  यदि(conn) {
    /* this should never happen, prevent underflow */
    DEBUGASSERT(conn->attached_xfers);
    यदि(conn->attached_xfers) {
      conn->attached_xfers--;
      यदि(!conn->attached_xfers)
        conn->attached_multi = रिक्त;
    }
  }
  दत्त->conn = रिक्त;
}

/*
 * Curl_attach_connection() attaches this transfer to this connection.
 *
 * This is the only function that should assign data->conn.
 * `matched == TRUE` means the transfer's properties match this
 * connection and it is not a temporary attach for maintenance.
 */
शून्य Curl_attach_connection(संरचना Curl_easy *दत्त,
                            संरचना connectdata *conn,
                            bool matched)
{
  DEBUGASSERT(दत्त);
  DEBUGASSERT(!दत्त->conn);
  DEBUGASSERT(conn);
  DEBUGASSERT(conn->attached_xfers < UINT32_MAX);
  दत्त->conn = conn;
  यदि(matched) {
    दत्त->state.चरमः_conn_id = conn->connection_id;
    दत्त->state.चरमः_cpid = conn->cpid;
  }
  अन्यथा
    DEBUGASSERT(!दत्त->mid); /* admin handle */
  conn->attached_xfers++;
  /* all attached transfers must be from the same multi */
  यदि(!conn->attached_multi)
    conn->attached_multi = दत्त->multi;
  DEBUGASSERT(conn->attached_multi == दत्त->multi);

  यदि(conn->scheme && conn->scheme->सततम्->attach)
    conn->scheme->सततम्->attach(दत्त, conn);
}

/* adjust pollset for rate limits/pauses */
स्थिर CURLcode multi_adjust_pollset(संरचना Curl_easy *दत्त,
                                     संरचना easy_pollset *ps)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(ps->n) {
    bool send_blocked, recv_blocked;

    recv_blocked = (Curl_rlimit_avail(&दत्त->प्रगतिः.dl.rlimit, रिक्त) <= 0);
    send_blocked = (Curl_rlimit_avail(&दत्त->प्रगतिः.ul.rlimit, रिक्त) <= 0);
    यदि(send_blocked || recv_blocked) {
      पूर्णाङ्क i;
      प्रति(i = 0; i <= SECONDARYSOCKET; ++i) {
        curl_socket_t sock = दत्त->conn->sock[i];
        यदि(sock == CURL_SOCKET_BAD)
          अनुवर्त;
        यदि(recv_blocked && Curl_pollset_want_recv(दत्त, ps, sock)) {
          परिणाम = Curl_pollset_remove_अन्तर्(दत्त, ps, sock);
          यदि(परिणाम)
            भङ्ग;
        }
        यदि(send_blocked && Curl_pollset_want_send(दत्त, ps, sock)) {
          परिणाम = Curl_pollset_remove_बहिर्गमनम्(दत्त, ps, sock);
          यदि(परिणाम)
            भङ्ग;
        }
      }
    }

    /* Not blocked and wanting to receive. If there is data pending
     * in the connection filters, make transfer run again. */
    यदि(!recv_blocked &&
       ((Curl_pollset_want_recv(दत्त, ps, दत्त->conn->sock[FIRSTSOCKET]) &&
         Curl_conn_दत्त_pending(दत्त, FIRSTSOCKET)) ||
        (Curl_pollset_want_recv(दत्त, ps, दत्त->conn->sock[SECONDARYSOCKET]) &&
         Curl_conn_दत्त_pending(दत्त, SECONDARYSOCKET)))) {
      CURL_TRC_M(दत्त, "pollset[] has POLLIN, but there is still "
                 "buffered input -> mark as dirty");
      Curl_multi_mark_dirty(दत्त);
    }
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode mstate_connecting_pollset(संरचना Curl_easy *दत्त,
                                          संरचना easy_pollset *ps)
{
  संरचना connectdata *conn = दत्त->conn;
  curl_socket_t sockfd;
  CURLcode परिणाम = CURLE_OK;

  यदि(Curl_xfer_recv_is_paused(दत्त))
    प्रत्यागम CURLE_OK;
  /* If a socket is set, receiving is default. If the socket
   * has not been determined yet (eyeballing), always ask the
   * connection filters for what to monitor. */
  sockfd = Curl_conn_get_first_socket(दत्त);
  यदि(sockfd != CURL_SOCKET_BAD) {
    परिणाम = Curl_pollset_change(दत्त, ps, sockfd, CURL_POLL_अन्तर्, 0);
    यदि(!परिणाम)
      परिणाम = multi_adjust_pollset(दत्त, ps);
  }
  यदि(!परिणाम)
    परिणाम = Curl_conn_adjust_pollset(दत्त, conn, ps);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode mstate_protocol_pollset(संरचना Curl_easy *दत्त,
                                        संरचना easy_pollset *ps)
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम = CURLE_OK;

  यदि(conn->scheme->सततम्->proto_pollset)
    परिणाम = conn->scheme->सततम्->proto_pollset(दत्त, ps);
  अन्यथा {
    curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
    यदि(sockfd != CURL_SOCKET_BAD) {
      /* Default is to wait to something from the server */
      परिणाम = Curl_pollset_change(दत्त, ps, sockfd, CURL_POLL_अन्तर्, 0);
    }
  }
  यदि(!परिणाम)
    परिणाम = multi_adjust_pollset(दत्त, ps);
  यदि(!परिणाम)
    परिणाम = Curl_conn_adjust_pollset(दत्त, conn, ps);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode mstate_do_pollset(संरचना Curl_easy *दत्त,
                                  संरचना easy_pollset *ps)
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम = CURLE_OK;

  यदि(conn->scheme->सततम्->doing_pollset)
    परिणाम = conn->scheme->सततम्->doing_pollset(दत्त, ps);
  अन्यथा यदि(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    परिणाम = Curl_pollset_सङ्कलय_बहिर्गमनम्(दत्त, ps, conn->sock[conn->send_idx]);
  }
  यदि(!परिणाम)
    परिणाम = multi_adjust_pollset(दत्त, ps);
  यदि(!परिणाम)
    परिणाम = Curl_conn_adjust_pollset(दत्त, conn, ps);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode mstate_domore_pollset(संरचना Curl_easy *दत्त,
                                      संरचना easy_pollset *ps)
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम = CURLE_OK;

  यदि(conn->scheme->सततम्->domore_pollset)
    परिणाम = conn->scheme->सततम्->domore_pollset(दत्त, ps);
  अन्यथा यदि(CONN_SOCK_IDX_VALID(conn->send_idx)) {
    /* Default is that we want to send something to the server */
    परिणाम = Curl_pollset_सङ्कलय_बहिर्गमनम्(दत्त, ps, conn->sock[conn->send_idx]);
  }
  यदि(!परिणाम)
    परिणाम = multi_adjust_pollset(दत्त, ps);
  यदि(!परिणाम)
    परिणाम = Curl_conn_adjust_pollset(दत्त, conn, ps);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode mstate_perform_pollset(संरचना Curl_easy *दत्त,
                                       संरचना easy_pollset *ps)
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम = CURLE_OK;

  यदि(conn->scheme->सततम्->perform_pollset)
    परिणाम = conn->scheme->सततम्->perform_pollset(दत्त, ps);
  अन्यथा {
    /* Default is to obey the request flags for send/recv */
    यदि(Curl_req_want_recv(दत्त) && CONN_SOCK_IDX_VALID(conn->recv_idx)) {
      परिणाम = Curl_pollset_सङ्कलय_अन्तर्(दत्त, ps, conn->sock[conn->recv_idx]);
    }
    यदि(!परिणाम && Curl_req_want_send(दत्त) &&
       CONN_SOCK_IDX_VALID(conn->send_idx)) {
      परिणाम = Curl_pollset_सङ्कलय_बहिर्गमनम्(दत्त, ps, conn->sock[conn->send_idx]);
    }
  }
  यदि(!परिणाम)
    परिणाम = multi_adjust_pollset(दत्त, ps);
  यदि(!परिणाम)
    परिणाम = Curl_conn_adjust_pollset(दत्त, conn, ps);
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् CURLVERBOSE
स्थिर size_t multi_timeouts_गणनम्(संरचना expire_timers *timeouts)
{
  size_t n = 0;
  uint8_t eid = timeouts->first;
  प्रति(; eid < EXPIRE_चरमः; eid = timeouts->अग्र[eid])
    ++n;
  प्रत्यागम n;
}
#यदि_अन्तः

/* Initializes `poll_set` with the current socket poll actions needed
 * for transfer `data`. */
CURLMcode Curl_multi_pollset(संरचना Curl_easy *दत्त,
                             संरचना easy_pollset *ps)
{
  CURLcode परिणाम = CURLE_OK;

  Curl_pollset_reset(ps);
#यदि_परिभाषितम् ENABLE_आन्तरिकम्_WAKEUP
  /* The admin handle always listens on the wakeup socket when there
   * are transfers alive. */
  यदि(दत्त->multi && (दत्त == दत्त->multi->admin) &&
     दत्त->multi->xfers_really_alive) {
    CURL_TRC_M(दत्त, "adding wakeup, %u xfers really alive",
               दत्त->multi->xfers_really_alive);
    परिणाम = Curl_pollset_सङ्कलय_अन्तर्(दत्त, ps, दत्त->multi->wakeup_आन्तरिकम्[0]);
  }
#यदि_अन्तः
  /* If the transfer has no connection, this is fine. Happens when
     called via curl_multi_remove_handle() => Curl_multi_ev_assess() =>
     Curl_multi_pollset(). */
  यदि(!परिणाम && दत्त->conn) {
    विकल्प(दत्त->mstate) {
    पक्ष MSTATE_INIT:
    पक्ष MSTATE_PENDING:
    पक्ष MSTATE_सज्जता:
    पक्ष MSTATE_CONNECT:
      /* nothing to poll for yet */
      भङ्ग;

    पक्ष MSTATE_CONNECTING:
      परिणाम = mstate_connecting_pollset(दत्त, ps);
      भङ्ग;

    पक्ष MSTATE_PROTOCONNECT:
    पक्ष MSTATE_PROTOCONNECTING:
      परिणाम = mstate_protocol_pollset(दत्त, ps);
      भङ्ग;

    पक्ष MSTATE_DO:
    पक्ष MSTATE_DOING:
      परिणाम = mstate_do_pollset(दत्त, ps);
      भङ्ग;

    पक्ष MSTATE_DOING_अधिकम्:
      परिणाम = mstate_domore_pollset(दत्त, ps);
      भङ्ग;

    पक्ष MSTATE_DID: /* same as PERFORMING in regard to polling */
    पक्ष MSTATE_PERFORMING:
      परिणाम = mstate_perform_pollset(दत्त, ps);
      भङ्ग;

    पक्ष MSTATE_RATELIMITING:
      /* we need to let time pass, ignore socket(s) */
      भङ्ग;

    पक्ष MSTATE_समाप्तम्:
    पक्ष MSTATE_COMPLETED:
    पक्ष MSTATE_MSGSENT:
      /* nothing more to poll for */
      भङ्ग;

    स्वभाव:
      failf(दत्त, "multi_getsock: unexpected multi state %d",
            (पूर्णाङ्क)दत्त->mstate);
      DEBUGASSERT(0);
      भङ्ग;
    }
  }

  यदि(परिणाम) {
    यदि(परिणाम == CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
      प्रत्यागम CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    failf(दत्त, "error determining pollset: %d", (पूर्णाङ्क)परिणाम);
    प्रत्यागम CURLM_आन्तरिकम्_ERROR;
  }

#यदि_परिभाषितम् CURLVERBOSE
  यदि(CURL_TRC_M_is_verbose(दत्त)) {
    size_t timeout_गणनम् = multi_timeouts_गणनम्(&दत्त->state.timeouts);
    विकल्प(ps->n) {
    पक्ष 0:
      CURL_TRC_M(दत्त, "pollset[], timeouts=%zu, paused %d/%d (r/w)",
                 timeout_गणनम्,
                 Curl_xfer_send_is_paused(दत्त),
                 Curl_xfer_recv_is_paused(दत्त));
      भङ्ग;
    पक्ष 1:
      CURL_TRC_M(दत्त, "pollset[fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_अन्तर्) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_बहिर्गमनम्) ? "OUT" : "",
                 timeout_गणनम्);
      भङ्ग;
    पक्ष 2:
      CURL_TRC_M(दत्त, "pollset[fd=%" FMT_SOCKET_T " %s%s, "
                 "fd=%" FMT_SOCKET_T " %s%s], timeouts=%zu",
                 ps->sockets[0],
                 (ps->actions[0] & CURL_POLL_अन्तर्) ? "IN" : "",
                 (ps->actions[0] & CURL_POLL_बहिर्गमनम्) ? "OUT" : "",
                 ps->sockets[1],
                 (ps->actions[1] & CURL_POLL_अन्तर्) ? "IN" : "",
                 (ps->actions[1] & CURL_POLL_बहिर्गमनम्) ? "OUT" : "",
                 timeout_गणनम्);
      भङ्ग;
    स्वभाव:
      CURL_TRC_M(दत्त, "pollset[fds=%u], timeouts=%zu", ps->n, timeout_गणनम्);
      भङ्ग;
    }
    CURL_TRC_EASY_TIMERS(दत्त);
  }
#यदि_अन्तः

  प्रत्यागम CURLM_OK;
}

CURLMcode curl_multi_fdset(CURLM *m,
                           fd_set *read_fd_समुच्चयः, fd_set *write_fd_समुच्चयः,
                           fd_set *exc_fd_समुच्चयः, पूर्णाङ्क *max_fd)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_fdset, &mresult)) {
    /* Scan through all the easy handles to get the file descriptors set.
       Some easy handles may not have connected to the remote host yet,
       and then we must make sure that is done. */
    संरचना Curl_multi *multi = m;
    संरचना easy_pollset ps;
    पूर्णाङ्क this_max_fd = -1;
    अचिह्नित पूर्णाङ्क i;
    uint32_t mid;
    (शून्य)exc_fd_समुच्चयः;

    Curl_pollset_init(&ps);
    यदि(Curl_uint32_bset_first(&multi->process, &mid)) {
      कुरु {
        संरचना Curl_easy *दत्त = Curl_multi_get_easy(multi, mid);

        यदि(!दत्त) {
          DEBUGASSERT(0);
          अनुवर्त;
        }

        Curl_multi_pollset(दत्त, &ps);
        प्रति(i = 0; i < ps.n; i++) {
          यदि(!FDSET_SOCK(ps.sockets[i]))
            /* pretend it does not exist */
            अनुवर्त;
          यदि(ps.actions[i] & CURL_POLL_अन्तर्)
            FD_SET(ps.sockets[i], read_fd_समुच्चयः);
          यदि(ps.actions[i] & CURL_POLL_बहिर्गमनम्)
            FD_SET(ps.sockets[i], write_fd_समुच्चयः);
          यदि((पूर्णाङ्क)ps.sockets[i] > this_max_fd)
            this_max_fd = (पूर्णाङ्क)ps.sockets[i];
        }
      } यावत्(Curl_uint32_bset_अग्र(&multi->process, mid, &mid));
    }

    Curl_cshutdn_setfds(&multi->cshutdn, multi->admin,
                        read_fd_समुच्चयः, write_fd_समुच्चयः, &this_max_fd);

    *max_fd = this_max_fd;
    Curl_pollset_cleanup(&ps);

    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

CURLMcode curl_multi_waitfds(CURLM *m,
                             संरचना curl_waitfd *ufds,
                             अचिह्नित पूर्णाङ्क आकार,
                             अचिह्नित पूर्णाङ्क *fd_गणनम्)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_waitfds, &mresult)) {
    संरचना Curl_waitfds cwfds;
    संरचना Curl_multi *multi = m;
    संरचना easy_pollset ps;
    अचिह्नित पूर्णाङ्क need = 0;
    uint32_t mid;

    यदि(!ufds && (आकार || !fd_गणनम्)) {
      mresult = CURLM_BAD_FUNCTION_तर्कः;
      गच्छ बहिर्गमनम्;
    }

    Curl_pollset_init(&ps);
    Curl_waitfds_init(&cwfds, ufds, आकार);
    mresult = CURLM_OK;
    यदि(Curl_uint32_bset_first(&multi->process, &mid)) {
      कुरु {
        संरचना Curl_easy *दत्त = Curl_multi_get_easy(multi, mid);
        यदि(!दत्त) {
          DEBUGASSERT(0);
          Curl_uint32_bset_remove(&multi->process, mid);
          Curl_uint32_bset_remove(&multi->dirty, mid);
          अनुवर्त;
        }
        Curl_multi_pollset(दत्त, &ps);
        need += Curl_waitfds_सङ्कलय_ps(&cwfds, &ps);
      } यावत्(Curl_uint32_bset_अग्र(&multi->process, mid, &mid));
    }

    need += Curl_cshutdn_सङ्कलय_waitfds(&multi->cshutdn, multi->admin, &cwfds);

    यदि(need != cwfds.n && ufds)
      mresult = CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    यदि(fd_गणनम्)
      *fd_गणनम् = need;
    Curl_pollset_cleanup(&ps);
  }
बहिर्गमनम्:
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

#यदि_परिभाषितम् USE_WINSOCK
/* Reset FD_WRITE for TCP sockets. Nothing is actually sent. UDP sockets cannot
 * be reset this way because an empty datagram would be sent. #9203
 *
 * "On Windows the internal state of FD_WRITE as returned from
 * WSAEnumNetworkEvents is only reset after successful send()."
 */
स्थिर शून्य reset_socket_fdwrite(curl_socket_t s)
{
  पूर्णाङ्क t;
  पूर्णाङ्क l = (पूर्णाङ्क)परिमाण(t);
  यदि(!getsockopt(s, SOL_SOCKET, SO_TYPE, (वर्ण *)&t, &l) && t == SOCK_STREAM)
    swrite(s, रिक्त, 0);
}

स्थिर CURLMcode multi_winsock_select(संरचना Curl_multi *multi,
                                      संरचना curl_pollfds *cpfds,
                                      अचिह्नित पूर्णाङ्क curl_nfds,
                                      संरचना curl_waitfd extra_fds[],
                                      अचिह्नित पूर्णाङ्क extra_nfds,
                                      पूर्णाङ्क timeout_ms,
                                      bool extrawait,
                                      पूर्णाङ्क *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  WSANETWORKEVENTS wsa_events;
  पूर्णाङ्क nevents = 0;
  size_t i;

  DEBUGASSERT(multi->wsa_event != WSA_INVALID_EVENT);

  /* Set the WSA events based on the collected pollds */
  प्रति(i = 0; i < cpfds->n; i++) {
    दीर्घ आवरणम् = 0;
    यदि(cpfds->pfds[i].events & POLLIN)
      आवरणम् |= FD_READ | FD_ACCEPT | FD_CLOSE;
    यदि(cpfds->pfds[i].events & POLLPRI)
      आवरणम् |= FD_OOB;
    यदि(cpfds->pfds[i].events & POLLOUT) {
      आवरणम् |= FD_WRITE | FD_CONNECT | FD_CLOSE;
      reset_socket_fdwrite(cpfds->pfds[i].fd);
    }
    यदि(आवरणम् && WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, आवरणम्)) {
      mresult = CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }

  यदि(cpfds->n || extrawait) {
    पूर्णाङ्क pollrc = 0;

    यदि(cpfds->n) {         /* pre-check with Winsock */
      pollrc = Curl_poll(cpfds->pfds, cpfds->n, 0);
      यदि(pollrc < 0) {
        mresult = CURLM_UNRECOVERABLE_POLL;
        गच्छ बहिर्गमनम्;
      }
      nevents = pollrc;
    }

    यदि(!nevents) {
      /* now wait... if not ready during the pre-check (pollrc == 0) */
      WSAWait_For_गुणजः_Events(1, &multi->wsa_event, FALSE, (DWORD)timeout_ms,
                               FALSE);
    }

    /* With Winsock, we have to run the following section unconditionally
       to call WSAEventSelect(fd, event, 0) on all the sockets */
    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    प्रति(i = 0; i < extra_nfds; i++) {
      अचिह्नित ह्रस्व आवरणम् = 0;
      curl_socket_t s = extra_fds[i].fd;

      wsa_events.l_जालम्_Events = 0;
      यदि(WSAEnum_जालम्_Events(s, रिक्त, &wsa_events) == 0) {
        यदि(wsa_events.l_जालम्_Events & (FD_READ | FD_ACCEPT | FD_CLOSE))
          आवरणम् |= CURL_WAIT_POLLIN;
        यदि(wsa_events.l_जालम्_Events & (FD_WRITE | FD_CONNECT | FD_CLOSE))
          आवरणम् |= CURL_WAIT_POLLOUT;
        यदि(wsa_events.l_जालम्_Events & FD_OOB)
          आवरणम् |= CURL_WAIT_POLLPRI;
        यदि(!pollrc && wsa_events.l_जालम्_Events)
          nevents++;
      }
      WSAEventSelect(s, multi->wsa_event, 0);
      यदि(!pollrc) {
        extra_fds[i].revents = (ह्रस्व)आवरणम्;
        अनुवर्त;
      }
      अन्यथा {
        अचिह्नित r = (अचिह्नित)cpfds->pfds[curl_nfds + i].revents;
        यदि(r & POLLIN)
          आवरणम् |= CURL_WAIT_POLLIN;
        यदि(r & POLLOUT)
          आवरणम् |= CURL_WAIT_POLLOUT;
        यदि(r & POLLPRI)
          आवरणम् |= CURL_WAIT_POLLPRI;
        extra_fds[i].revents = (ह्रस्व)आवरणम्;
      }
    }

    /* Count up all our own sockets that had activity,
       and remove them from the event. */
    प्रति(i = 0; i < curl_nfds; ++i) {
      wsa_events.l_जालम्_Events = 0;
      यदि(WSAEnum_जालम्_Events(cpfds->pfds[i].fd, रिक्त, &wsa_events) == 0) {
        यदि(!pollrc && wsa_events.l_जालम्_Events)
          nevents++;
      }
      WSAEventSelect(cpfds->pfds[i].fd, multi->wsa_event, 0);
    }
    WSAResetEvent(multi->wsa_event);
  }

बहिर्गमनम्:
  *pnevents = nevents;
  प्रत्यागम mresult;
}

#अन्यथा /* USE_WINSOCK */

स्थिर CURLMcode multi_posix_poll(संरचना Curl_multi *multi,
                                  संरचना curl_pollfds *cpfds,
                                  अचिह्नित पूर्णाङ्क curl_nfds,
                                  संरचना curl_waitfd extra_fds[],
                                  अचिह्नित पूर्णाङ्क extra_nfds,
                                  पूर्णाङ्क timeout_ms,
                                  bool extrawait,
                                  पूर्णाङ्क *pnevents)
{
  CURLMcode mresult = CURLM_OK;
  पूर्णाङ्क nevents = 0;
  size_t i;

  (शून्य)multi;
  यदि(cpfds->n) {
    पूर्णाङ्क pollrc = Curl_poll(cpfds->pfds, cpfds->n, timeout_ms); /* wait... */
    यदि(pollrc < 0) {
      mresult = CURLM_UNRECOVERABLE_POLL;
      गच्छ बहिर्गमनम्;
    }
    nevents = pollrc;

    /* copy revents results from the poll to the curl_multi_wait poll
       struct, the bit values of the actual underlying poll() implementation
       may not be the same as the ones in the public libcurl API! */
    प्रति(i = 0; i < extra_nfds; i++) {
      अचिह्नित r = (अचिह्नित)cpfds->pfds[curl_nfds + i].revents;
      अचिह्नित ह्रस्व आवरणम् = 0;
      यदि(r & POLLIN)
        आवरणम् |= CURL_WAIT_POLLIN;
      यदि(r & POLLOUT)
        आवरणम् |= CURL_WAIT_POLLOUT;
      यदि(r & POLLPRI)
        आवरणम् |= CURL_WAIT_POLLPRI;
      extra_fds[i].revents = (ह्रस्व)आवरणम्;
    }
  }
  अन्यथा यदि(extrawait) {
    /* No fds to poll, but asked to obey timeout_ms anyway. We cannot
     * use Curl_poll() as it, on some platforms, returns immediately
     * without fds. */
    curlx_wait_ms(timeout_ms);
  }

बहिर्गमनम्:
  *pnevents = nevents;
  प्रत्यागम mresult;
}

#यदि_अन्तः /* !USE_WINSOCK */

#परिभाषय अङ्कः_POLLS_ON_राशिः 10

स्थिर CURLMcode multi_wait(संरचना Curl_multi *multi,
                            संरचना curl_waitfd extra_fds[],
                            अचिह्नित पूर्णाङ्क extra_nfds,
                            पूर्णाङ्क timeout_ms,
                            पूर्णाङ्क *ret,
                            bool extrawait)  /* when no socket, wait */
{
  size_t i;
  पूर्णाङ्क timeout_आन्तरिकम्;
  पूर्णाङ्क nevents = 0;
  संरचना easy_pollset ps;
  संरचना pollfd a_few_on_राशिः[अङ्कः_POLLS_ON_राशिः];
  संरचना curl_pollfds cpfds;
  अचिह्नित पूर्णाङ्क curl_nfds = 0; /* how many pfds are for curl transfers */
  संरचना Curl_easy *दत्त = रिक्त;
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;
#यदि_परिभाषितम् ENABLE_WAKEUP
  पूर्णाङ्क wakeup_idx = -1;
#यदि_अन्तः

  यदि(timeout_ms < 0)
    प्रत्यागम CURLM_BAD_FUNCTION_तर्कः;

  Curl_pollset_init(&ps);
  Curl_pollfds_init(&cpfds, a_few_on_राशिः, अङ्कः_POLLS_ON_राशिः);

  /* Add the curl handles to our pollfds first */
  यदि(Curl_uint32_bset_first(&multi->process, &mid)) {
    कुरु {
      दत्त = Curl_multi_get_easy(multi, mid);
      यदि(!दत्त) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        अनुवर्त;
      }
      Curl_multi_pollset(दत्त, &ps);
      यदि(Curl_pollfds_सङ्कलय_ps(&cpfds, &ps)) {
        mresult = CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
    } यावत्(Curl_uint32_bset_अग्र(&multi->process, mid, &mid));
  }

  यदि(Curl_cshutdn_सङ्कलय_pollfds(&multi->cshutdn, multi->admin, &cpfds)) {
    mresult = CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

#यदि_परिभाषितम् ENABLE_WAKEUP
  /* If `extrawait` is TRUE *or* we have `extra_fds`to poll *or* we
   * have transfer sockets to poll, we obey `timeout_ms`.
   * Then we need to also monitor the multi's wakeup
   * socket to catch calls to `curl_multi_wakeup()` during the wait. */
  यदि(extrawait || cpfds.n || extra_nfds) {
    wakeup_idx = cpfds.n;
    यदि(Curl_pollfds_सङ्कलय_sock(&cpfds, multi->wakeup_pair[0], POLLIN)) {
      mresult = CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }
#यदि_अन्तः

  curl_nfds = cpfds.n; /* what curl internally uses in cpfds */
  /* Add external file descriptions from poll-like struct curl_waitfd */
  प्रति(i = 0; i < extra_nfds; i++) {
    अचिह्नित ह्रस्व events = 0;
    यदि(extra_fds[i].events & CURL_WAIT_POLLIN)
      events |= POLLIN;
    यदि(extra_fds[i].events & CURL_WAIT_POLLPRI)
      events |= POLLPRI;
    यदि(extra_fds[i].events & CURL_WAIT_POLLOUT)
      events |= POLLOUT;
    यदि(Curl_pollfds_सङ्कलय_sock(&cpfds, extra_fds[i].fd, events)) {
      mresult = CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }

  /* We check the internal timeout *AFTER* we collected all sockets to
   * poll. Collecting the sockets may install new timers by protocols
   * and connection filters.
   * Use the shorter one of the internal and the caller requested timeout.
   * If we are called with `!extrawait` and multi_timeout() reports no
   * timeouts exist, do not wait. */
  multi_timeout(multi, रिक्त, &timeout_आन्तरिकम्);
  यदि((timeout_आन्तरिकम् >= 0) && (timeout_आन्तरिकम् < timeout_ms))
    timeout_ms = timeout_आन्तरिकम्;

  यदि(दत्त)
    CURL_TRC_M(दत्त, "multi_wait(fds=%u, timeout=%d) tinternal=%d",
               cpfds.n, timeout_ms, timeout_आन्तरिकम्);

#यदि_परिभाषितम् USE_WINSOCK
  mresult = multi_winsock_select(multi, &cpfds, curl_nfds,
                                 extra_fds, extra_nfds,
                                 timeout_ms, extrawait, &nevents);
#अन्यथा
  mresult = multi_posix_poll(multi, &cpfds, curl_nfds,
                             extra_fds, extra_nfds,
                             timeout_ms, extrawait, &nevents);
#यदि_अन्तः

#यदि_परिभाषितम् ENABLE_WAKEUP
    यदि(nevents && (wakeup_idx >= 0)) {
      यदि(cpfds.pfds[wakeup_idx].revents & POLLIN) {
        (शून्य)Curl_wakeup_consume(multi->wakeup_pair, TRUE);
        /* do not count the wakeup socket into the returned value */
        nevents--;
      }
    }
#यदि_अन्तः

बहिर्गमनम्:
  Curl_pollset_cleanup(&ps);
  Curl_pollfds_cleanup(&cpfds);
  यदि(ret)
    *ret = nevents;
  प्रत्यागम mresult;
}

CURLMcode curl_multi_wait(CURLM *m,
                          संरचना curl_waitfd extra_fds[],
                          अचिह्नित पूर्णाङ्क extra_nfds,
                          पूर्णाङ्क timeout_ms,
                          पूर्णाङ्क *ret)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_wait, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, FALSE);
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

CURLMcode curl_multi_poll(CURLM *m,
                          संरचना curl_waitfd extra_fds[],
                          अचिह्नित पूर्णाङ्क extra_nfds,
                          पूर्णाङ्क timeout_ms,
                          पूर्णाङ्क *ret)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_poll, &mresult)) {
    mresult = multi_wait(m, extra_fds, extra_nfds, timeout_ms, ret, TRUE);
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

CURLMcode curl_multi_wakeup(CURLM *m)
{
  /* this function is usually called from another thread,
     it has to be careful only to access parts of the
     Curl_multi struct that are constant */
  संरचना Curl_multi *multi = m;
  CURLMcode mresult = CURLM_WAKEUP_FAILURE;

  /* GOOD_MULTI_HANDLE can be safely called */
  यदि(!GOOD_MULTI_HANDLE(multi))
    प्रत्यागम CURLM_BAD_HANDLE;

#यदि_परिभाषितम् ENABLE_WAKEUP
  /* the wakeup_pair variable is only written during init and cleanup,
     making it safe to access from another thread after the init part
     and before cleanup */
  यदि(!Curl_wakeup_signal(multi->wakeup_pair))
    mresult = CURLM_OK;
#यदि_अन्तः
#यदि_परिभाषितम् USE_WINSOCK
  यदि(WSASetEvent(multi->wsa_event))
    mresult = CURLM_OK;
#यदि_अन्तः
  प्रत्यागम mresult;
}

#यदि_परिभाषितम् ENABLE_आन्तरिकम्_WAKEUP
शून्य Curl_multi_wakeup_आन्तरिकम्(संरचना Curl_multi *multi)
{
  /* This is expected to be invocable from another thread which
   * does NOT outlive the multi handle. Check for sanity. */
  यदि(GOOD_MULTI_HANDLE(multi))
    Curl_wakeup_signal(multi->wakeup_आन्तरिकम्);
  अन्यथा
    DEBUGASSERT(0);
}
#यदि_अन्तः

/*
 * multi_ischanged() is called
 *
 * Returns TRUE/FALSE whether the state is changed to trigger a CONNECT_PEND
 * => CONNECT action.
 *
 * Set 'clear' to TRUE to have it also clear the state variable.
 */
स्थिर bool multi_ischanged(संरचना Curl_multi *multi, bool clear)
{
  bool retval = FALSE;
  DEBUGASSERT(multi);
  यदि(multi) {
    retval = (bool)multi->recheckstate;
    यदि(clear)
      multi->recheckstate = FALSE;
  }
  प्रत्यागम retval;
}

/*
 * Curl_multi_connchanged() is called to tell that there is a connection in
 * this multi handle that has changed state (multiplexing become possible, the
 * number of allowed streams changed or similar), and a subsequent use of this
 * multi handle should move CONNECT_PEND handles back to CONNECT to have them
 * retry.
 */
शून्य Curl_multi_connchanged(संरचना Curl_multi *multi)
{
  /* May be invoked for a `data->multi` when no multi is present. */
  यदि(multi)
    multi->recheckstate = TRUE;
}

CURLMcode Curl_multi_सङ्कलय_perform(संरचना Curl_multi *multi,
                                 संरचना Curl_easy *दत्त,
                                 संरचना connectdata *conn)
{
  CURLMcode mresult;

  mresult = Curl_multi_सङ्कलय_handle(multi, दत्त);
  यदि(!mresult) {
    CURLcode परिणाम;

    /* pass in NULL for 'conn' here since we do not want to init the
       connection, only this transfer */
    परिणाम = Curl_init_transfer(दत्त, रिक्त);
    यदि(परिणाम) {
      Curl_multi_remove_handle(multi, दत्त);
      प्रत्यागम CURLM_आन्तरिकम्_ERROR;
    }

    /* take this handle to the perform state right away */
    multistate(दत्त, MSTATE_PERFORMING);
    Curl_attach_connection(दत्त, conn, TRUE);
    CURL_REQ_समुच्चयः_RECV(दत्त);
  }
  प्रत्यागम mresult;
}

स्थिर CURLcode multi_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;

  DEBUGASSERT(conn);
  DEBUGASSERT(conn->scheme);

  यदि(conn->scheme->सततम्->do_it)
    परिणाम = conn->scheme->सततम्->do_it(दत्त, समाप्तम्);

  प्रत्यागम परिणाम;
}

/*
 * multi_do_more() is called during the DO_MORE multi state. It is a second
 * stage DO state which (wrongly) was introduced to support FTP's second
 * connection.
 *
 * 'complete' can return DOMORE_INCOMPLETE, DOMORE_DONE or DOMORE_GOBACK
 * (to DOING state when there is more work to do)
 */

स्थिर CURLcode multi_do_अधिकम्(संरचना Curl_easy *दत्त, domore *complete)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;

  *complete = DOMORE_INCOMPLETE;

  यदि(conn->scheme->सततम्->do_अधिकम्)
    परिणाम = conn->scheme->सततम्->do_अधिकम्(दत्त, complete);

  प्रत्यागम परिणाम;
}

/*
 * Check whether a timeout occurred, and handle it if it did
 */
स्थिर bool multi_handle_timeout(संरचना Curl_easy *दत्त,
                                 ध्रुव संरचना curltime *pnow,
                                 bool *प्रवाहः_error,
                                 CURLcode *परिणाम)
{
  timediff_t timeout_ms;

  timeout_ms = Curl_timeleft_now_ms(दत्त, pnow);
  यदि(timeout_ms < 0) {
    /* Handle timed out */
    timerid आधारः_timer = Curl_is_connecting(दत्त) ?
                         TIMER_STARTSINGLE : TIMER_STARTOP;
    timediff_t elapsed_ms = Curl_pgrs_since_ms(दत्त, रिक्त, आधारः_timer);
    यदि(दत्त->mstate == MSTATE_CONNECTING) {
      failf(दत्त, "%s timed out after %" FMT_TIMEDIFF_T " milliseconds",
            दत्त->conn->द्वयङ्काः.dns_resolved ? "Connection" : "Resolving",
            elapsed_ms);
      /* keep what we know about a partly done connect, e.g. TCP done
       * but TLS handshake still ongoing */
      Curl_conn_report_failed(दत्त, FIRSTSOCKET);
    }
    अन्यथा {
      संरचना SingleRequest *k = &दत्त->req;
      यदि(k->आकार != -1) {
        failf(दत्त, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " out of %"
              FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount, k->आकार);
      }
      अन्यथा {
        failf(दत्त, "Operation timed out after %" FMT_TIMEDIFF_T
              " milliseconds with %" FMT_OFF_T " bytes received",
              elapsed_ms, k->bytecount);
      }
    }
    *परिणाम = CURLE_OPERATION_TIMEDOUT;
    यदि(दत्त->conn) {
      /* Force connection closed if the connection has indeed been used */
      यदि(दत्त->mstate > MSTATE_DO) {
        streamclose(दत्त->conn);
        *प्रवाहः_error = TRUE;
      }
      (शून्य)multi_समाप्तम्(दत्त, *परिणाम, TRUE);
    }
    प्रत्यागम TRUE;
  }

  प्रत्यागम FALSE;
}

/*
 * We are doing protocol-specific connecting and this is being called over and
 * over from the multi interface until the connection phase is done on
 * protocol layer.
 */

स्थिर CURLcode protocol_connecting(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;

  यदि(conn && conn->scheme->सततम्->connecting) {
    *समाप्तम् = FALSE;
    परिणाम = conn->scheme->सततम्->connecting(दत्त, समाप्तम्);
  }
  अन्यथा
    *समाप्तम् = TRUE;

  प्रत्यागम परिणाम;
}

/*
 * We are DOING this is being called over and over from the multi interface
 * until the DOING phase is done on protocol layer.
 */

स्थिर CURLcode protocol_doing(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना connectdata *conn = दत्त->conn;

  यदि(conn && conn->scheme->सततम्->doing) {
    *समाप्तम् = FALSE;
    परिणाम = conn->scheme->सततम्->doing(दत्त, समाप्तम्);
  }
  अन्यथा
    *समाप्तम् = TRUE;

  प्रत्यागम परिणाम;
}

/*
 * We have discovered that the TCP connection has been successful, we can now
 * proceed with some action.
 *
 */
स्थिर CURLcode protocol_connect(संरचना Curl_easy *दत्त, bool *protocol_समाप्तम्)
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(conn);
  DEBUGASSERT(protocol_समाप्तम्);
  DEBUGASSERT(Curl_conn_is_connected(conn, FIRSTSOCKET));

  *protocol_समाप्तम् = FALSE;
  यदि(!conn->द्वयङ्काः.protoconnstart) {
    यदि(conn->scheme->सततम्->connect_it) {
      /* Call the protocol-specific connect function */
      परिणाम = conn->scheme->सततम्->connect_it(दत्त, protocol_समाप्तम्);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    conn->द्वयङ्काः.protoconnstart = TRUE;
  }

  /* Unless this protocol does not have any protocol-connect callback, as
     then we know we are done. */
  यदि(!conn->scheme->सततम्->connecting)
    *protocol_समाप्तम् = TRUE;
  प्रत्यागम CURLE_OK;
}

/*
 * posttransfer() is called immediately after a transfer ends
 */
स्थिर शून्य multi_posttransfer(संरचना Curl_easy *दत्त)
{
#यदि defined(HAVE_SIGNAL) && defined(SIGPIPE) && !defined(MSG_NOSIGNAL)
  /* restore the signal handler for SIGPIPE before we get back */
  यदि(!दत्त->समुच्चयः.no_signal)
    सङ्केत(SIGPIPE, दत्त->state.पूर्व_signal);
#अन्यथा
  (शून्य)दत्त;
#यदि_अन्तः
}

/*
 * multi_follow() handles the URL redirect magic. Pass in the 'newurl' string
 * as given by the remote server and set up the new URL to request.
 *
 * This function DOES NOT FREE the given URL.
 */
स्थिर CURLcode multi_follow(संरचना Curl_easy *दत्त,
                             ध्रुव संरचना Curl_scheme *handler,
                             ध्रुव वर्ण *newurl, /* the Location: string */
                             followtype type) /* see transfer.h */
{
  यदि(handler && handler->सततम्->follow)
    प्रत्यागम handler->सततम्->follow(दत्त, newurl, type);

  यदि(type == FOLLOW_RETRY)
    /* Retries are generic and do not require protocol-specific redirect
       handling. */
    प्रत्यागम CURLE_OK;

  प्रत्यागम CURLE_TOO_MANY_REDIRECTS;
}

स्थिर CURLcode mspeed_check(संरचना Curl_easy *दत्त)
{
  यदि(Curl_rlimit_सक्रिय(&दत्त->प्रगतिः.dl.rlimit) ||
     Curl_rlimit_सक्रिय(&दत्त->प्रगतिः.ul.rlimit)) {
    /* check if our send/recv limits require idle waits */
    ध्रुव संरचना curltime *pnow = Curl_pgrs_now(दत्त);
    timediff_t recv_ms, send_ms;

    send_ms = Curl_rlimit_wait_ms(&दत्त->प्रगतिः.ul.rlimit, pnow);
    recv_ms = Curl_rlimit_wait_ms(&दत्त->प्रगतिः.dl.rlimit, pnow);

    यदि(send_ms || recv_ms) {
      यदि(दत्त->mstate != MSTATE_RATELIMITING) {
        multistate(दत्त, MSTATE_RATELIMITING);
      }
      Curl_expire_समुच्चयः(दत्त, EXPIRE_TOOFAST, CURLMAX(send_ms, recv_ms), pnow);
      Curl_multi_clear_dirty(दत्त);
      CURL_TRC_M(दत्त, "[RLIMIT] waiting %" FMT_TIMEDIFF_T "ms",
                 CURLMAX(send_ms, recv_ms));
      प्रत्यागम CURLE_AGAIN;
    }
    अन्यथा {
      /* when will the rate limits increase next? The transfer needs
       * to run again at that time or it may stall. */
      send_ms = Curl_rlimit_अग्र_चरणः_ms(&दत्त->प्रगतिः.ul.rlimit, pnow);
      recv_ms = Curl_rlimit_अग्र_चरणः_ms(&दत्त->प्रगतिः.dl.rlimit, pnow);
      यदि(send_ms || recv_ms) {
        timediff_t अग्र_ms = CURLMIN(send_ms, recv_ms);
        यदि(!अग्र_ms)
          अग्र_ms = CURLMAX(send_ms, recv_ms);
        Curl_expire_समुच्चयः(दत्त, EXPIRE_TOOFAST, अग्र_ms, pnow);
        CURL_TRC_M(दत्त, "[RLIMIT] next token update in %" FMT_TIMEDIFF_T "ms",
                   अग्र_ms);
      }
    }
  }

  यदि(दत्त->mstate != MSTATE_PERFORMING) {
    CURL_TRC_M(दत्त, "[RLIMIT] wait over, continue");
    multistate(दत्त, MSTATE_PERFORMING);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLMcode multistate_performing(संरचना Curl_easy *दत्त,
                                       bool *प्रवाहः_errorp,
                                       CURLcode *resultp)
{
  वर्ण *newurl = रिक्त;
  bool retry = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode परिणाम = *resultp = CURLE_OK;
  *प्रवाहः_errorp = FALSE;

  यदि(mspeed_check(दत्त) == CURLE_AGAIN)
    प्रत्यागम CURLM_OK;

  /* read/write data if it is ready to do so */
  परिणाम = Curl_sendrecv(दत्त);

  यदि(दत्त->req.समाप्तम् || (परिणाम == CURLE_RECV_ERROR)) {
    /* If CURLE_RECV_ERROR happens early enough, we assume it was a race
     * condition and the server closed the reused connection exactly when we
     * wanted to use it, so figure out if that is indeed the case.
     */
    CURLcode ret = Curl_retry_request(दत्त, &newurl);
    यदि(!ret)
      retry = !!newurl;
    अन्यथा यदि(!परिणाम)
      परिणाम = ret;

    यदि(retry) {
      /* if we are to retry, set the result to OK and consider the
         request as done */
      परिणाम = CURLE_OK;
      दत्त->req.समाप्तम् = TRUE;
    }
  }
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  अन्यथा यदि((परिणाम == CURLE_HTTP2_प्रवाहः) &&
          Curl_h2_http_1_1_error(दत्त)) {
    CURLcode ret = Curl_retry_request(दत्त, &newurl);

    यदि(!ret) {
      infof(दत्त, "Downgrades to HTTP/1.1");
      streamclose(दत्त->conn);
      दत्त->state.http_neg.wanted = CURL_HTTP_V1x;
      दत्त->state.http_neg.allowed = CURL_HTTP_V1x;
      /* clear the error message bit too as we ignore the one we got */
      दत्त->state.errorbuf = FALSE;
      यदि(!newurl)
        /* typically for HTTP_1_1_REQUIRED error on first flight */
        newurl = Curl_bufref_dup(&दत्त->state.url);
      यदि(!newurl) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
      अन्यथा {
        /* if we are to retry, set the result to OK and consider the request
          as done */
        retry = TRUE;
        परिणाम = CURLE_OK;
        दत्त->req.समाप्तम् = TRUE;
      }
    }
    अन्यथा
      परिणाम = ret;
  }
#यदि_अन्तः

  यदि(परिणाम) {
    /*
     * The transfer phase returned error, we mark the connection to get closed
     * to prevent being reused. This is because we cannot possibly know if the
     * connection is in a good shape or not now. Unless it is a protocol which
     * uses two "channels" like FTP, as then the error happened in the data
     * connection.
     */

    यदि(!(दत्त->conn->scheme->flags & PROTOPT_DUAL) &&
       परिणाम != CURLE_HTTP2_प्रवाहः)
      streamclose(दत्त->conn);

    multi_posttransfer(दत्त);
    multi_समाप्तम्(दत्त, परिणाम, TRUE);
  }
  अन्यथा यदि(दत्त->req.समाप्तम् && !Curl_cwriter_is_paused(दत्त)) {
    ध्रुव संरचना Curl_scheme *handler = दत्त->conn->scheme;

    /* call this even if the readwrite function returned error */
    multi_posttransfer(दत्त);

    /* When we follow redirects or is set to retry the connection, we must to
       go back to the CONNECT state */
    यदि(दत्त->req.newurl || retry) {
      followtype follow = FOLLOW_NONE;
      यदि(!retry) {
        /* if the URL is a follow-location and not a retried request then
           figure out the URL here */
        curlx_free(newurl);
        newurl = दत्त->req.newurl;
        दत्त->req.newurl = रिक्त;
        follow = FOLLOW_REDIR;
      }
      अन्यथा
        follow = FOLLOW_RETRY;
      (शून्य)multi_समाप्तम्(दत्त, CURLE_OK, FALSE);
      /* multi_done() might return CURLE_GOT_NOTHING */
      परिणाम = multi_follow(दत्त, handler, newurl, follow);
      यदि(!परिणाम) {
        multistate(दत्त, MSTATE_सज्जता);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    अन्यथा {
      /* after the transfer is done, go DONE */

      /* but first check to see if we got a location info even though we are
         not following redirects */
      यदि(दत्त->req.location) {
        curlx_free(newurl);
        newurl = दत्त->req.location;
        दत्त->req.location = रिक्त;
        परिणाम = multi_follow(दत्त, handler, newurl, FOLLOW_FAKE);
        यदि(परिणाम) {
          *प्रवाहः_errorp = TRUE;
          परिणाम = multi_समाप्तम्(दत्त, परिणाम, TRUE);
        }
      }

      यदि(!परिणाम) {
        multistate(दत्त, MSTATE_समाप्तम्);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
  }
  अन्यथा { /* not errored, not done */
    mspeed_check(दत्त);
  }
  curlx_free(newurl);
  *resultp = परिणाम;
  प्रत्यागम mresult;
}

स्थिर CURLMcode multistate_do(संरचना Curl_easy *दत्त,
                               bool *प्रवाहः_errorp,
                               CURLcode *resultp)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode परिणाम = CURLE_OK;
  यदि(दत्त->समुच्चयः.fprereq) {
    संरचना Curl_mapi_guard guard;
    पूर्णाङ्क prereq_rc;

    /* call the prerequest callback function */
    CURL_CBAPI_START(&guard, दत्त, easy_fprereq);
    prereq_rc = दत्त->समुच्चयः.fprereq(दत्त->समुच्चयः.prereq_userp,
                                  दत्त->info.primary.remote_ip,
                                  दत्त->info.primary.local_ip,
                                  दत्त->info.primary.remote_पत्तनम्,
                                  दत्त->info.primary.local_पत्तनम्);
    CURL_CBAPI_अन्तः(&guard);
    यदि(prereq_rc != CURL_PREREQFUNC_OK) {
      failf(दत्त, "operation aborted by pre-request callback");
      /* failure in pre-request callback - do not do any other processing */
      परिणाम = CURLE_ABORTED_BY_CALLBACK;
      multi_posttransfer(दत्त);
      multi_समाप्तम्(दत्त, परिणाम, FALSE);
      *प्रवाहः_errorp = TRUE;
      गच्छ अन्तः;
    }
  }

  यदि(दत्त->समुच्चयः.connect_only && !दत्त->समुच्चयः.connect_only_ws) {
    multistate(दत्त, MSTATE_समाप्तम्);
    mresult = CURLM_CALL_MULTI_PERFORM;
  }
  अन्यथा {
    bool dophase_समाप्तम् = FALSE;
    /* Perform the protocol's DO action */
    परिणाम = multi_do(दत्त, &dophase_समाप्तम्);

    यदि(!परिणाम) {
      यदि(!dophase_समाप्तम्) {
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
        /* some steps needed for wildcard matching */
        यदि(दत्त->state.wildcardmatch) {
          संरचना Wildcard_दत्त *wc = दत्त->wildcard;
          यदि(wc->state == CURLWC_समाप्तम् || wc->state == CURLWC_SKIP) {
            /* skip some states if it is important */
            multi_समाप्तम्(दत्त, CURLE_OK, FALSE);

            /* if there is no connection left, skip the DONE state */
            multistate(दत्त, दत्त->conn ? MSTATE_समाप्तम् : MSTATE_COMPLETED);
            mresult = CURLM_CALL_MULTI_PERFORM;
            गच्छ अन्तः;
          }
        }
#यदि_अन्तः
        /* DO was not completed in one function call, we must continue
           DOING... */
        multistate(दत्त, MSTATE_DOING);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }

      /* after DO, go DO_DONE... or DO_MORE */
      अन्यथा यदि(दत्त->conn->द्वयङ्काः.do_अधिकम्) {
        /* we are supposed to do more, but we need to sit down, relax and wait
           a little while first */
        multistate(दत्त, MSTATE_DOING_अधिकम्);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
      अन्यथा {
        /* we are done with the DO, now DID */
        multistate(दत्त, MSTATE_DID);
        mresult = CURLM_CALL_MULTI_PERFORM;
      }
    }
    अन्यथा यदि((परिणाम == CURLE_SEND_ERROR) &&
            दत्त->conn->द्वयङ्काः.reuse) {
      /*
       * In this situation, a connection that we were trying to use may have
       * unexpectedly died. If possible, send the connection back to the
       * CONNECT phase so we can try again.
       */
      ध्रुव संरचना Curl_scheme *handler = दत्त->conn->scheme;
      वर्ण *newurl = रिक्त;
      followtype follow = FOLLOW_NONE;
      CURLcode drc;

      drc = Curl_retry_request(दत्त, &newurl);
      यदि(drc) {
        /* a failure here pretty much implies an out of memory */
        परिणाम = drc;
        *प्रवाहः_errorp = TRUE;
      }

      multi_posttransfer(दत्त);
      drc = multi_समाप्तम्(दत्त, परिणाम, FALSE);

      /* When set to retry the connection, we must go back to the CONNECT
       * state */
      यदि(newurl) {
        यदि(!drc || (drc == CURLE_SEND_ERROR)) {
          follow = FOLLOW_RETRY;
          drc = multi_follow(दत्त, handler, newurl, follow);
          यदि(!drc) {
            multistate(दत्त, MSTATE_सज्जता);
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
        *प्रवाहः_errorp = TRUE;
      }
      curlx_free(newurl);
    }
    अन्यथा {
      /* failure detected */
      multi_posttransfer(दत्त);
      यदि(दत्त->conn)
        multi_समाप्तम्(दत्त, परिणाम, FALSE);
      *प्रवाहः_errorp = TRUE;
    }
  }
अन्तः:
  *resultp = परिणाम;
  प्रत्यागम mresult;
}

स्थिर CURLMcode multistate_ratelimiting(संरचना Curl_easy *दत्त,
                                         CURLcode *resultp)
{
  CURLcode परिणाम = CURLE_OK;
  CURLMcode mresult = CURLM_OK;
  DEBUGASSERT(दत्त->conn);
  /* if both rates are within spec, resume transfer */
  परिणाम = Curl_pgrsCheck(दत्त);

  यदि(परिणाम) {
    यदि(!(दत्त->conn->scheme->flags & PROTOPT_DUAL) &&
       परिणाम != CURLE_HTTP2_प्रवाहः)
      streamclose(दत्त->conn);

    multi_posttransfer(दत्त);
    multi_समाप्तम्(दत्त, परिणाम, TRUE);
  }
  अन्यथा {
    यदि(!mspeed_check(दत्त))
      mresult = CURLM_CALL_MULTI_PERFORM;
  }
  *resultp = परिणाम;
  प्रत्यागम mresult;
}

स्थिर CURLMcode multistate_connect(संरचना Curl_multi *multi,
                                    संरचना Curl_easy *दत्त,
                                    CURLcode *resultp)
{
  /* Connect. We want to get a connection identifier filled in. This state can
     be entered from SETUP and from PENDING. */
  bool connected;
  CURLMcode mresult = CURLM_OK;
  CURLcode परिणाम = Curl_connect(दत्त, &connected);
  यदि(परिणाम == CURLE_NO_CONNECTION_उपलब्धम्) {
    /* There was no connection available. We will go to the pending state and
       wait for an available connection. */
    multistate(दत्त, MSTATE_PENDING);
    /* move from process to pending set */
    Curl_uint32_bset_remove(&multi->process, दत्त->mid);
    Curl_uint32_bset_remove(&multi->dirty, दत्त->mid);
    Curl_uint32_bset_सङ्कलय(&multi->pending, दत्त->mid);
    *resultp = CURLE_OK;
    प्रत्यागम mresult;
  }
  अन्यथा
    multi_कालसारणी_pending(दत्त->multi);

  यदि(!परिणाम) {
    /* after the connect has been sent off, go WAITCONNECT unless the
       protocol connect is already done and we can go directly to WAITDO or
       DO! */
    mresult = CURLM_CALL_MULTI_PERFORM;

    यदि(connected) {
      यदि(!दत्त->conn->द्वयङ्काः.reuse &&
         Curl_conn_is_multiplex(दत्त->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_कालसारणी_pending(दत्त->multi);
      }
      multistate(दत्त, MSTATE_PROTOCONNECT);
    }
    अन्यथा {
      multistate(दत्त, MSTATE_CONNECTING);
    }
  }
  *resultp = परिणाम;
  प्रत्यागम mresult;
}

/* returns the possibly updated result */
स्थिर CURLcode is_परिसमाप्तम्(संरचना Curl_multi *multi,
                            संरचना Curl_easy *दत्त,
                            ध्रुव संरचना curltime *pnow,
                            bool प्रवाहः_error,
                            CURLcode परिणाम)
{
  यदि(दत्त->mstate < MSTATE_COMPLETED) {
    यदि(परिणाम) {
      /*
       * If an error was returned, and we are not in completed state now,
       * then we go to completed and consider this transfer aborted.
       */

      /* No attempt to disconnect connections must be made before this -
         connection detach and termination happens only here */

      /* Check if we can move pending requests to send pipe */
      multi_कालसारणी_pending(multi); /* connection */

      यदि(दत्त->conn) {
        यदि(प्रवाहः_error) {
          /* Do not attempt to send data over a connection that timed out */
          bool dead_connection = परिणाम == CURLE_OPERATION_TIMEDOUT;
          संरचना connectdata *conn = दत्त->conn;

          /* This is where we make sure that the conn pointer is reset.
             We do not have to do this in every case block above where a
             failure is detected */
          Curl_detach_connection(दत्त);
          Curl_conn_close(दत्त, conn, dead_connection);
        }
      }
      अन्यथा यदि(दत्त->mstate == MSTATE_CONNECT) {
        /* Curl_connect() failed */
        multi_posttransfer(दत्त);
        Curl_pgrsUpdate_nometer(दत्त);
      }

      multistate(दत्त, MSTATE_COMPLETED);
      प्रत्यागम परिणाम;
    }
    /* if there is still a connection to use, call the progress function */
    अन्यथा यदि(दत्त->conn && Curl_conn_is_connected(दत्त->conn, FIRSTSOCKET)) {
      परिणाम = Curl_pgrsUpdateX(दत्त, pnow);
      यदि(परिणाम) {
        /* aborted due to progress callback return code must close the
           connection */
        streamclose(दत्त->conn);

        /* if not yet in DONE state, go there. */
        यदि(दत्त->mstate < MSTATE_समाप्तम्)
          multistate(दत्त, MSTATE_समाप्तम्);
        प्रत्यागम परिणाम;
      }
    }
  }
  प्रत्यागम परिणाम;
}

स्थिर शून्य handle_completed(संरचना Curl_multi *multi,
                             संरचना Curl_easy *दत्त,
                             CURLcode परिणाम)
{
  bool सन्देशः_गन्तव्यम्_app = दत्त->master_mid == UINT32_MAX;

  यदि(!सन्देशः_गन्तव्यम्_app) {
    /* A sub transfer, not reported to the application. Is anyone still
     * interested in processing its results? */
    यदि(दत्त->sub_xfer_समाप्तम्) {
      संरचना Curl_easy *master = Curl_multi_get_easy(multi, दत्त->master_mid);

      CURL_TRC_M(दत्त, "sub xfer done for master %u", दत्त->master_mid);
      यदि(master)
        दत्त->sub_xfer_समाप्तम्(दत्त, master, परिणाम);
      अन्यथा
        CURL_TRC_M(दत्त, "master easy %u already gone.", दत्त->master_mid);
    }
  }
  अन्यथा {
    /* now fill in the CURLMsg with this info */
    संरचना CURLMsg *सन्देशः = &दत्त->सन्देशः;

    सन्देशः->सन्देशः = CURLMSG_समाप्तम्;
    सन्देशः->easy_handle = दत्त;
    सन्देशः->दत्त.परिणाम = परिणाम;

    DEBUGASSERT(!दत्त->conn);
  }
  multistate(दत्त, MSTATE_MSGSENT);

  /* remove from the other sets */
  Curl_uint32_bset_remove(&multi->process, दत्त->mid);
  Curl_uint32_bset_remove(&multi->dirty, दत्त->mid);
  Curl_uint32_bset_remove(&multi->pending, दत्त->mid);
  यदि(सन्देशः_गन्तव्यम्_app)
    multi_addmsg(multi, दत्त);
  यदि(दत्त->state.really_alive) {
    दत्त->state.really_alive = FALSE;
    --multi->xfers_really_alive;
    यदि(!multi->xfers_really_alive)
      (शून्य)multi_assess_wakeup(multi);
  }
  --multi->xfers_alive;
  यदि(!multi->xfers_alive)
    multi_assess_wakeup(multi);
}

स्थिर CURLMcode multistate_init(संरचना Curl_easy *दत्त, CURLcode *परिणाम)
{
  यदि(!दत्त->state.really_alive) {
    दत्त->state.really_alive = TRUE;
    ++दत्त->multi->xfers_really_alive;
    यदि(दत्त->multi->xfers_really_alive == 1) {
      CURLMcode mresult = multi_assess_wakeup(दत्त->multi);
      यदि(mresult) {
        failf(दत्त, "error enabling wakeup listening: %d", mresult);
        प्रत्यागम mresult;
      }
    }
  }

  *परिणाम = Curl_pretransfer(दत्त);
  यदि(*परिणाम)
    प्रत्यागम CURLM_OK;

  /* after init, go SETUP */
  multistate(दत्त, MSTATE_सज्जता);
  Curl_pgrsTime(दत्त, TIMER_STARTOP);
  प्रत्यागम CURLM_CALL_MULTI_PERFORM;
}

स्थिर CURLMcode multistate_सज्जता(संरचना Curl_easy *दत्त)
{
  ध्रुव संरचना curltime *pnow = Curl_pgrs_now(दत्त);
  Curl_pgrsTimeWas(दत्त, TIMER_STARTSINGLE, *pnow);
  यदि(दत्त->समुच्चयः.timeout)
    Curl_expire_समुच्चयः(दत्त, EXPIRE_TIMEOUT, दत्त->समुच्चयः.timeout, pnow);
  यदि(दत्त->समुच्चयः.connecttimeout)
    /* Since a connection might go to pending and back to CONNECT several
       times before it actually takes off, we need to set the timeout once
       in SETUP before we enter CONNECT the first time. */
    Curl_expire_समुच्चयः(दत्त, EXPIRE_CONNECTTIMEOUT,
                    दत्त->समुच्चयः.connecttimeout, pnow);

  multistate(दत्त, MSTATE_CONNECT);
  प्रत्यागम CURLM_CALL_MULTI_PERFORM;
}

स्थिर CURLMcode multistate_connecting(संरचना Curl_easy *दत्त,
                                       bool *प्रवाहः_error,
                                       CURLcode *परिणाम)
{
  bool connected;

  यदि(!दत्त->conn) {
    DEBUGASSERT(0);
    *परिणाम = CURLE_FAILED_INIT;
    प्रत्यागम CURLM_OK;
  }
  यदि(!Curl_xfer_recv_is_paused(दत्त)) {
    *परिणाम = Curl_conn_connect(दत्त, FIRSTSOCKET, FALSE, &connected);
    यदि(connected && !*परिणाम) {
      यदि(!दत्त->conn->द्वयङ्काः.reuse &&
         Curl_conn_is_multiplex(दत्त->conn, FIRSTSOCKET)) {
        /* new connection, can multiplex, wake pending handles */
        multi_कालसारणी_pending(दत्त->multi);
      }
      multistate(दत्त, MSTATE_PROTOCONNECT);
      प्रत्यागम CURLM_CALL_MULTI_PERFORM;
    }
    अन्यथा यदि(*परिणाम) {
      /* failure detected */
      CURL_TRC_M(दत्त, "connect failed -> %d", (पूर्णाङ्क)*परिणाम);
      multi_posttransfer(दत्त);
      multi_समाप्तम्(दत्त, *परिणाम, TRUE);
      *प्रवाहः_error = TRUE;
      प्रत्यागम CURLM_OK;
    }
  }
  प्रत्यागम CURLM_OK;
}

स्थिर CURLMcode multistate_protoconnect(संरचना Curl_easy *दत्त,
                                         bool *प्रवाहः_error,
                                         CURLcode *परिणाम)
{
  bool protocol_connected = FALSE;

  यदि(!*परिणाम && दत्त->conn->द्वयङ्काः.reuse) {
    /* ftp seems to hang when protoconnect on reused connection since we
     * handle PROTOCONNECT in general inside the filters, it seems wrong to
     * restart this on a reused connection.
     */
    multistate(दत्त, MSTATE_DO);
    प्रत्यागम CURLM_CALL_MULTI_PERFORM;
  }
  यदि(!*परिणाम)
    *परिणाम = protocol_connect(दत्त, &protocol_connected);
  यदि(!*परिणाम && !protocol_connected) {
    /* switch to waiting state */
    multistate(दत्त, MSTATE_PROTOCONNECTING);
    प्रत्यागम CURLM_CALL_MULTI_PERFORM;
  }
  अन्यथा यदि(!*परिणाम) {
    /* protocol connect has completed, go WAITDO or DO */
    multistate(दत्त, MSTATE_DO);
    प्रत्यागम CURLM_CALL_MULTI_PERFORM;
  }

  /* failure detected */
  multi_posttransfer(दत्त);
  multi_समाप्तम्(दत्त, *परिणाम, TRUE);
  *प्रवाहः_error = TRUE;
  प्रत्यागम CURLM_OK;
}

स्थिर CURLMcode multistate_protoconnecting(संरचना Curl_easy *दत्त,
                                            bool *प्रवाहः_error,
                                            CURLcode *परिणाम)
{
  bool protocol_connected = FALSE;

  /* protocol-specific connect phase */
  *परिणाम = protocol_connecting(दत्त, &protocol_connected);
  यदि(!*परिणाम && protocol_connected) {
    /* after the connect has completed, go WAITDO or DO */
    multistate(दत्त, MSTATE_DO);
    प्रत्यागम CURLM_CALL_MULTI_PERFORM;
  }
  अन्यथा यदि(*परिणाम) {
    /* failure detected */
    multi_posttransfer(दत्त);
    multi_समाप्तम्(दत्त, *परिणाम, TRUE);
    *प्रवाहः_error = TRUE;
  }
  प्रत्यागम CURLM_OK;
}

स्थिर CURLMcode multistate_doing(संरचना Curl_easy *दत्त,
                                  bool *प्रवाहः_error,
                                  CURLcode *परिणाम)
{
  bool dophase_समाप्तम् = FALSE;

  /* we continue DOING until the DO phase is complete */
  DEBUGASSERT(दत्त->conn);
  *परिणाम = protocol_doing(दत्त, &dophase_समाप्तम्);
  यदि(!*परिणाम) {
    यदि(dophase_समाप्तम्) {
      /* after DO, go DO_DONE or DO_MORE */
      multistate(दत्त, दत्त->conn->द्वयङ्काः.do_अधिकम् ?
                 MSTATE_DOING_अधिकम् : MSTATE_DID);
      प्रत्यागम CURLM_CALL_MULTI_PERFORM;
    } /* dophase_done */
  }
  अन्यथा {
    /* failure detected */
    multi_posttransfer(दत्त);
    multi_समाप्तम्(दत्त, *परिणाम, FALSE);
    *प्रवाहः_error = TRUE;
  }
  प्रत्यागम CURLM_OK;
}

स्थिर CURLMcode multistate_doing_अधिकम्(संरचना Curl_easy *दत्त,
                                       bool *प्रवाहः_error,
                                       CURLcode *परिणाम)
{
  domore control;

  /*
   * When we are connected, DOING MORE and then go DID
   */
  DEBUGASSERT(दत्त->conn);
  *परिणाम = multi_do_अधिकम्(दत्त, &control);

  यदि(!*परिणाम) {
    यदि(control != DOMORE_INCOMPLETE) {
      /* if DONE, advance to DO_DONE
         if GOBACK, go back to DOING */
      multistate(दत्त, control == DOMORE_समाप्तम् ? MSTATE_DID : MSTATE_DOING);
      प्रत्यागम CURLM_CALL_MULTI_PERFORM;
    }
    /* else
       stay in DO_MORE */
  }
  अन्यथा {
    /* failure detected */
    multi_posttransfer(दत्त);
    multi_समाप्तम्(दत्त, *परिणाम, FALSE);
    *प्रवाहः_error = TRUE;
  }
  प्रत्यागम CURLM_OK;
}

स्थिर CURLMcode multistate_did(संरचना Curl_multi *multi,
                                संरचना Curl_easy *दत्त)
{
  DEBUGASSERT(दत्त->conn);
  यदि(दत्त->conn->द्वयङ्काः.multiplex)
    /* Check if we can move pending requests to send pipe */
    multi_कालसारणी_pending(multi); /* multiplexed */

  /* Only perform the transfer if there is a good socket to work with.
     Having both BAD is a signal to skip immediately to DONE */
  यदि(CONN_SOCK_IDX_VALID(दत्त->conn->recv_idx) ||
     CONN_SOCK_IDX_VALID(दत्त->conn->send_idx)) {
    multistate(दत्त, MSTATE_PERFORMING);
    /* Do not return CURLM_CALL_MULTI_PERFORM to give other transfers
     * a chance to send off their requests.
     * Note: Some SFTP handlers do not seem to like this.
     *       Restrict it to HTTP families. */
    प्रत्यागम ((multi->xfers_alive > 1) &&
            (दत्त->conn->scheme->protocol & PROTO_FAMILY_HTTP)) ?
           CURLM_OK : CURLM_CALL_MULTI_PERFORM;
  }
  अन्यथा {
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
    यदि(दत्त->state.wildcardmatch &&
       ((दत्त->conn->scheme->flags & PROTOPT_WILDCARD) == 0)) {
      दत्त->wildcard->state = CURLWC_समाप्तम्;
    }
#यदि_अन्तः
    multistate(दत्त, MSTATE_समाप्तम्);
    प्रत्यागम CURLM_CALL_MULTI_PERFORM;
  }
}

स्थिर CURLMcode multistate_समाप्तम्(संरचना Curl_easy *दत्त, CURLcode *presult)
{
  यदि(दत्त->conn) {
    CURLcode परिणाम;

    /* post-transfer command */
    परिणाम = multi_समाप्तम्(दत्त, *presult, FALSE);

    /* allow a previously set error code take precedence */
    यदि(!(*presult))
      *presult = परिणाम;
  }

#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  यदि(दत्त->state.wildcardmatch) {
    यदि(दत्त->wildcard->state != CURLWC_समाप्तम्) {
      /* if a wildcard is set and we are not ending -> lets start again
         with MSTATE_INIT */
      multistate(दत्त, MSTATE_INIT);
      प्रत्यागम CURLM_CALL_MULTI_PERFORM;
    }
  }
#यदि_अन्तः
  /* after we have DONE what we are supposed to do, go COMPLETED, and
     it does not matter what the multi_done() returned! */
  multistate(दत्त, MSTATE_COMPLETED);
  प्रत्यागम CURLM_CALL_MULTI_PERFORM;
}

स्थिर CURLMcode multi_runsingle(संरचना Curl_multi *multi,
                                 संरचना Curl_easy *दत्त,
                                 संरचना Curl_sigpipe_ctx *sigpipe_ctx)
{
  CURLMcode mresult = CURLM_OK;
  CURLcode परिणाम = CURLE_OK;
  ध्रुव संरचना curltime *pnow = रिक्त;

  यदि(multi->dead) {
    /* a multi-level callback returned error before, meaning every individual
     transfer now has failed */
    परिणाम = CURLE_ABORTED_BY_CALLBACK;
    multi_posttransfer(दत्त);
    multi_समाप्तम्(दत्त, परिणाम, FALSE);
    multistate(दत्त, MSTATE_COMPLETED);
  }

  multi_warn_debug(multi, दत्त);

  /* transfer runs now, clear the dirty bit. This may be set
   * again during processing, triggering a re-run later. */
  Curl_uint32_bset_remove(&multi->dirty, दत्त->mid);

  यदि(दत्त == multi->admin) {
#यदि_परिभाषितम् ENABLE_आन्तरिकम्_WAKEUP
    /* Consume any pending wakeup signals before processing.
     * This is necessary for event based processing. See #21547 */
    (शून्य)Curl_wakeup_consume(multi->wakeup_आन्तरिकम्, TRUE);
#यदि_अन्तः
#यदि_परिभाषितम् USE_RESOLV_THREADED
    Curl_async_thrdd_multi_process(multi);
#यदि_अन्तः
    Curl_cshutdn_perform(&multi->cshutdn, multi->admin, sigpipe_ctx);
    गच्छ बहिर्गमनम्;
  }

  sigpipe_apply(दत्त, sigpipe_ctx);
  कुरु {
    /* A "stream" here is a logical stream if the protocol can handle that
       (HTTP/2), or the full connection for older protocols */
    bool प्रवाहः_error = FALSE;
    mresult = CURLM_OK;
    pnow = रिक्त;

    यदि(multi_ischanged(multi, TRUE)) {
      CURL_TRC_M(दत्त, "multi changed, check CONNECT_PEND queue");
      multi_कालसारणी_pending(multi); /* multiplexed */
    }

    यदि(दत्त->mstate > MSTATE_CONNECT &&
       दत्त->mstate < MSTATE_COMPLETED) {
      /* Make sure we set the connection's current owner */
      DEBUGASSERT(दत्त->conn);
      यदि(!दत्त->conn) {
        mresult = CURLM_आन्तरिकम्_ERROR;
        गच्छ बहिर्गमनम्;
      }
    }

    /* Wait for the connect state as only then is the start time stored, but
       we must not check already completed handles */
    यदि((दत्त->mstate >= MSTATE_CONNECT) && (दत्त->mstate < MSTATE_COMPLETED)) {
      pnow = Curl_pgrs_now(दत्त);
      यदि(multi_handle_timeout(दत्त, pnow, &प्रवाहः_error, &परिणाम))
        /* Skip the statemachine and go directly to error handling section. */
        गच्छ statemachine_अन्तः;
      pnow = रिक्त;
    }

    विकल्प(दत्त->mstate) {
    पक्ष MSTATE_INIT:
      /* Transitional state. init this transfer. A handle never comes back to
         this state. */
      mresult = multistate_init(दत्त, &परिणाम);
      भङ्ग;

    पक्ष MSTATE_सज्जता:
      /* Transitional state. Setup things for a new transfer. The handle
         can come back to this state on a redirect. */
      mresult = multistate_सज्जता(दत्त);
      भङ्ग;

    पक्ष MSTATE_CONNECT:
      mresult = multistate_connect(multi, दत्त, &परिणाम);
      भङ्ग;

    पक्ष MSTATE_CONNECTING:
      /* awaiting a completion of an asynch TCP connect */
      mresult = multistate_connecting(दत्त, &प्रवाहः_error, &परिणाम);
      भङ्ग;

    पक्ष MSTATE_PROTOCONNECT:
      mresult = multistate_protoconnect(दत्त, &प्रवाहः_error, &परिणाम);
      भङ्ग;

    पक्ष MSTATE_PROTOCONNECTING:
      /* protocol-specific connect phase */
      mresult = multistate_protoconnecting(दत्त, &प्रवाहः_error, &परिणाम);
      भङ्ग;

    पक्ष MSTATE_DO:
      mresult = multistate_do(दत्त, &प्रवाहः_error, &परिणाम);
      भङ्ग;

    पक्ष MSTATE_DOING:
      /* we continue DOING until the DO phase is complete */
      mresult = multistate_doing(दत्त, &प्रवाहः_error, &परिणाम);
      भङ्ग;

    पक्ष MSTATE_DOING_अधिकम्:
      /*
       * When we are connected, DOING MORE and then go DID
       */
      mresult = multistate_doing_अधिकम्(दत्त, &प्रवाहः_error, &परिणाम);
      भङ्ग;

    पक्ष MSTATE_DID:
      mresult = multistate_did(multi, दत्त);
      भङ्ग;

    पक्ष MSTATE_RATELIMITING: /* limit-rate exceeded in either direction */
      mresult = multistate_ratelimiting(दत्त, &परिणाम);
      भङ्ग;

    पक्ष MSTATE_PERFORMING:
      mresult = multistate_performing(दत्त, &प्रवाहः_error, &परिणाम);
      भङ्ग;

    पक्ष MSTATE_समाप्तम्:
      mresult = multistate_समाप्तम्(दत्त, &परिणाम);
      भङ्ग;

    पक्ष MSTATE_COMPLETED:
      भङ्ग;

    पक्ष MSTATE_PENDING:
    पक्ष MSTATE_MSGSENT:
      /* handles in these states should NOT be in this list */
      भङ्ग;

    स्वभाव:
      mresult = CURLM_आन्तरिकम्_ERROR;
      गच्छ बहिर्गमनम्;
    }

    यदि(दत्त->mstate >= MSTATE_CONNECT &&
       दत्त->mstate < MSTATE_DO &&
       mresult != CURLM_CALL_MULTI_PERFORM &&
       !multi_ischanged(multi, FALSE)) {
      /* We now handle stream timeouts if and only if this will be the last
       * loop iteration. We only check this on the last iteration to ensure
       * that if we know we have additional work to do immediately
       * (i.e. CURLM_CALL_MULTI_PERFORM == TRUE) then we should do that before
       * declaring the connection timed out as we may almost have a completed
       * connection. */
      pnow = Curl_pgrs_now(दत्त);
      multi_handle_timeout(दत्त, pnow, &प्रवाहः_error, &परिणाम);
    }

statemachine_अन्तः:
    यदि(!pnow)
      pnow = Curl_pgrs_now(दत्त);
    परिणाम = is_परिसमाप्तम्(multi, दत्त, pnow, प्रवाहः_error, परिणाम);
    यदि(परिणाम)
      mresult = CURLM_CALL_MULTI_PERFORM;

    यदि(MSTATE_COMPLETED == दत्त->mstate) {
      handle_completed(multi, दत्त, परिणाम);
      mresult = CURLM_OK;
      गच्छ बहिर्गमनम्;
    }
  } यावत्((mresult == CURLM_CALL_MULTI_PERFORM) ||
          multi_ischanged(multi, FALSE));

बहिर्गमनम्:
  दत्त->परिणाम = परिणाम;
  प्रत्यागम mresult;
}

स्थिर CURLMcode multi_perform(संरचना Curl_multi *multi,
                               पूर्णाङ्क *running_handles)
{
  CURLMcode returncode = CURLM_OK;
  संरचना curltime start;
  uint32_t mid;
  संरचना Curl_sigpipe_ctx sigpipe_ctx;

  curlx_pnow(&start);
  sigpipe_init(&sigpipe_ctx);

  यदि(Curl_uint32_bset_first(&multi->process, &mid)) {
    CURL_TRC_M(multi->admin, "multi_perform(running=%u)",
               multi_xfers_running(multi));
    कुरु {
      संरचना Curl_easy *दत्त = Curl_multi_get_easy(multi, mid);
      CURLMcode mresult;
      यदि(!दत्त) {
        DEBUGASSERT(0);
        Curl_uint32_bset_remove(&multi->process, mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
        अनुवर्त;
      }
      mresult = multi_runsingle(multi, दत्त, &sigpipe_ctx);
      यदि(mresult)
        returncode = mresult;
    } यावत्(Curl_uint32_bset_अग्र(&multi->process, mid, &mid));
  }
  sigpipe_restore(&sigpipe_ctx);

  यदि(multi_ischanged(multi, TRUE))
    multi_कालसारणी_pending(multi);

  यदि(!returncode && CURL_MNTFY_HAS_ENTRIES(multi))
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
  यावत्(Curl_timeouts_remove_expired(&multi->timeouts, &start, &mid)) {
    /* the removed may have another timeout in queue */
    संरचना Curl_easy *दत्त = Curl_multi_get_easy(multi, mid);
    यदि(!दत्त) {
      DEBUGASSERT(0);
      अनुवर्त;
    }
    (शून्य)सङ्कलय_अग्र_timeout(&start, multi, दत्त);
    यदि(दत्त->mstate == MSTATE_PENDING) {
      bool प्रवाहः_अप्रयुक्तम्;
      CURLcode परिणाम_अप्रयुक्तम्;
      यदि(multi_handle_timeout(दत्त, Curl_pgrs_now(दत्त),
                              &प्रवाहः_अप्रयुक्तम्, &परिणाम_अप्रयुक्तम्)) {
        infof(दत्त, "PENDING handle timeout");
        move_pending_गन्तव्यम्_connect(multi, दत्त);
      }
    }
  }

  यदि(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < अधिकतम_पूर्णाङ्कः) ? (पूर्णाङ्क)running : अधिकतम_पूर्णाङ्कः;
  }

  यदि(CURLM_OK >= returncode)
    returncode = Curl_update_timer(multi);

  प्रत्यागम returncode;
}

CURLMcode curl_multi_perform(CURLM *m, पूर्णाङ्क *running_handles)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_perform, &mresult)) {
    mresult = multi_perform(m, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

CURLMcode curl_multi_cleanup(CURLM *m)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_cleanup, &mresult)) {
    संरचना Curl_multi *multi = m;
    शून्य *प्रविष्टिः;
    uint32_t mid;

    /* First remove all remaining easy handles,
     * close internal ones. admin handle is special */
    यदि(Curl_uint32_tbl_first(&multi->xfers, &mid, &प्रविष्टिः)) {
      कुरु {
        संरचना Curl_easy *दत्त = प्रविष्टिः;
        यदि(!GOOD_EASY_HANDLE(दत्त)) {
          mresult = CURLM_BAD_HANDLE;
          गच्छ बहिर्गमनम्;
        }

#यदि_परिभाषितम् DEBUGBUILD
        यदि(mid != दत्त->mid) {
          CURL_TRC_M(दत्त, "multi_cleanup: still present with mid=%u, "
                     "but unexpected data->mid=%u\n", mid, दत्त->mid);
          DEBUGASSERT(0);
        }
#यदि_अन्तः

        यदि(दत्त == multi->admin)
          अनुवर्त;

        यदि(!दत्त->state.समाप्तम् && दत्त->conn)
          /* if DONE was never called for this handle */
          (शून्य)multi_समाप्तम्(दत्त, CURLE_OK, TRUE);

        दत्त->multi = रिक्त; /* clear the association */
        Curl_uint32_tbl_remove(&multi->xfers, mid);
        दत्त->mid = UINT32_MAX;

#यदि_परिभाषितम् USE_LIBPSL
        यदि(दत्त->psl == &multi->psl)
          दत्त->psl = रिक्त;
#यदि_अन्तः
        यदि(दत्त->state.आन्तरिकम्)
          Curl_close(&दत्त);
      } यावत्(Curl_uint32_tbl_अग्र(&multi->xfers, mid, &mid, &प्रविष्टिः));
    }

#यदि_परिभाषितम् USE_RESOLV_THREADED
    Curl_async_thrdd_multi_destroy(multi, !multi->quick_exit);
#यदि_अन्तः
    Curl_cpool_destroy(&multi->cpool, multi->admin);
    Curl_cshutdn_destroy(&multi->cshutdn, multi->admin);
    यदि(multi->admin) {
      CURL_TRC_M(multi->admin, "multi_cleanup, closing admin handle, done");
      multi->admin->multi = रिक्त;
      Curl_uint32_tbl_remove(&multi->xfers, multi->admin->mid);
      Curl_close(&multi->admin);
    }

    multi->magic = 0; /* not good anymore */

    Curl_multi_ev_cleanup(multi);
    Curl_hash_destroy(&multi->proto_hash);
    Curl_dnscache_destroy(&multi->dnscache);
    Curl_psl_destroy(&multi->psl);
#यदि_परिभाषितम् USE_SSL
    Curl_ssl_scache_destroy(multi->ssl_scache);
#यदि_अन्तः

#यदि_परिभाषितम् USE_WINSOCK
    WSACloseEvent(multi->wsa_event);
#यदि_अन्तः
#यदि_परिभाषितम् ENABLE_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_pair);
#यदि_अन्तः
#यदि_परिभाषितम् ENABLE_आन्तरिकम्_WAKEUP
  Curl_wakeup_destroy(multi->wakeup_आन्तरिकम्);
#यदि_अन्तः

    multi_xfer_bufs_free(multi);
    Curl_mntfy_cleanup(multi);
#यदि_परिभाषितम् DEBUGBUILD
    यदि(Curl_uint32_tbl_गणनम्(&multi->xfers)) {
      multi_xfer_tbl_dump(multi);
      DEBUGASSERT(0);
    }
#यदि_अन्तः
    Curl_uint32_bset_destroy(&multi->process);
    Curl_uint32_bset_destroy(&multi->dirty);
    Curl_uint32_bset_destroy(&multi->pending);
    Curl_uint32_bset_destroy(&multi->msgsent);
    Curl_uint32_tbl_destroy(&multi->xfers);
    curlx_memzero(multi, परिमाण(*multi));
    curlx_free(multi);

    mresult = CURLM_OK;
  }
बहिर्गमनम्:
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

/*
 * curl_multi_info_read()
 *
 * This function is the primary way for a multi/multi_socket application to
 * figure out if a transfer has ended.
 */

CURLMsg *curl_multi_info_read(CURLM *m, पूर्णाङ्क *msgs_अन्तर्_queue)
{
  संरचना Curl_mapi_guard guard;
  CURLMsg *सन्देशः_परिणाम = रिक्त;

  *msgs_अन्तर्_queue = 0; /* default to none */
  यदि(CURL_MAPI_ENTER(&guard, m, multi_info_read, रिक्त)) {
    संरचना Curl_multi *multi = m;
    uint32_t mid;
    यदि(Curl_uint32_bset_first(&multi->msgsent, &mid)) {
      संरचना Curl_easy *दत्त = Curl_multi_get_easy(multi, mid);

      DEBUGASSERT(दत्त);
      Curl_uint32_bset_remove(&multi->msgsent, mid);
      *msgs_अन्तर्_queue =
        curlx_uztosi(Curl_uint32_bset_गणनम्(&multi->msgsent));
      यदि(दत्त)
        सन्देशः_परिणाम = &दत्त->सन्देशः;
    }
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम सन्देशः_परिणाम;
}

शून्य Curl_multi_will_close(संरचना Curl_easy *दत्त, curl_socket_t s)
{
  यदि(दत्त) {
    संरचना Curl_multi *multi = दत्त->multi;
    यदि(multi) {
      CURL_TRC_M(दत्त, "Curl_multi_will_close fd=%" FMT_SOCKET_T, s);
      Curl_multi_ev_socket_समाप्तम्(multi, दत्त, s);
    }
  }
}

स्थिर शून्य multi_timeouts_init(संरचना Curl_easy *दत्त)
{
  दत्त->state.timeouts.first = EXPIRE_चरमः;
  दत्त->state.timeouts.splaynode.registered = FALSE;
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
स्थिर CURLMcode सङ्कलय_अग्र_timeout(ध्रुव संरचना curltime *pnow,
                                  संरचना Curl_multi *multi,
                                  संरचना Curl_easy *दत्त)
{
  संरचना expire_timers *timeouts = &दत्त->state.timeouts;
  timediff_t now_us = Curl_timeouts_विस्थापनम्_us(&multi->timeouts, pnow);

  यावत्(timeouts->first < EXPIRE_चरमः) {
    यदि(timeouts->विस्थापनम्_us[timeouts->first] <= now_us)  /* already expired */
      timeouts->first = timeouts->अग्र[timeouts->first];
    अन्यथा /* timeouts are sorted, first is first in the future now */
      भङ्ग;
  }

  यदि(timeouts->first < EXPIRE_चरमः) {
    /* Insert this node again into the splay. Keep the timer in the list in
       case we need to recompute future timers. */
    Curl_timeouts_सङ्कलय(&multi->timeouts, दत्त,
                      timeouts->विस्थापनम्_us[timeouts->first]);
  }
  प्रत्यागम CURLM_OK;
}

स्थिर शून्य multi_mark_expired_as_dirty(संरचना Curl_multi *multi,
                                        ध्रुव संरचना curltime *ts)
{
  संरचना Curl_easy *दत्त = रिक्त;
  uint32_t mid;

  /*
   * The loop following here will go on as long as there are expire-times left
   * to process (compared to `ts`) in the splay and 'data' will be
   * re-assigned for every expired handle we deal with.
   */
  यावत्(Curl_timeouts_remove_expired(&multi->timeouts, ts, &mid)) {
    /* Check if there is one (more) expired timer to deal with! This function
       extracts a matching node if there is one */
    दत्त = Curl_multi_get_easy(multi, mid);
    यदि(!दत्त) {
      DEBUGASSERT(0);
      अनुवर्त;
    }
#यदि_परिभाषितम् CURLVERBOSE
    यदि(CURL_TRC_TIMER_is_verbose(दत्त)) {
      यदि(दत्त->state.timeouts.first < EXPIRE_चरमः) {
        CURL_TRC_TIMER(दत्त, दत्त->state.timeouts.first, "has expired");
      }
    }
#यदि_अन्तः
    (शून्य)सङ्कलय_अग्र_timeout(ts, multi, दत्त);
    Curl_multi_mark_dirty(दत्त);
  }
}

स्थिर CURLMcode multi_सततम्_dirty(संरचना Curl_multi *multi,
                                 संरचना Curl_sigpipe_ctx *sigpipe_ctx,
                                 uint32_t *pnum)
{
  CURLMcode mresult = CURLM_OK;
  uint32_t mid;

  *pnum = 0;
  यदि(Curl_uint32_bset_first(&multi->dirty, &mid)) {
    कुरु {
      संरचना Curl_easy *दत्त = Curl_multi_get_easy(multi, mid);
      यदि(दत्त) {
        CURL_TRC_M(दत्त, "multi_run_dirty");

        यदि(!Curl_uint32_bset_अन्तर्भवति(&multi->process, mid)) {
          यदि(Curl_uint32_bset_अन्तर्भवति(&multi->pending, mid)) {
            /* Something happened on the pending transfer, act on it. */
            move_pending_गन्तव्यम्_connect(multi, दत्त);
          }
          अन्यथा {
            /* We are no longer processing this transfer */
            Curl_uint32_bset_remove(&multi->dirty, mid);
            अनुवर्त;
          }
        }

        (*pnum)++;
        /* runsingle() clears the dirty mid */
        mresult = multi_runsingle(multi, दत्त, sigpipe_ctx);

        यदि(CURLM_OK >= mresult) {
          /* reassess event handling of data */
          mresult = Curl_multi_ev_assess_xfer(multi, दत्त);
          यदि(mresult)
            गच्छ बहिर्गमनम्;
        }
      }
      अन्यथा {
        CURL_TRC_M(multi->admin, "multi_run_dirty, %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } यावत्(Curl_uint32_bset_अग्र(&multi->dirty, mid, &mid));
  }

बहिर्गमनम्:
  प्रत्यागम mresult;
}

स्थिर CURLMcode multi_socket(संरचना Curl_multi *multi,
                              bool checkall,
                              curl_socket_t s,
                              पूर्णाङ्क ev_bitmask,
                              पूर्णाङ्क *running_handles)
{
  CURLMcode mresult = CURLM_OK;
  संरचना Curl_sigpipe_ctx pipe_ctx;
  संरचना curltime now;
  uint32_t सततम्_xfers;

  (शून्य)ev_bitmask;
  sigpipe_init(&pipe_ctx);

  यदि(checkall) {
    /* *perform() deals with running_handles on its own */
    mresult = multi_perform(multi, running_handles);

    यदि(mresult != CURLM_BAD_HANDLE) {
      /* Reassess event status of all active transfers, but keep any error
         from multi_perform() */
      CURLMcode mresult2 =
        Curl_multi_ev_assess_xfer_bset(multi, &multi->process);
      यदि(!mresult)
        mresult = mresult2;
    }
    गच्छ बहिर्गमनम्;
  }

  यदि(s != CURL_SOCKET_TIMEOUT) {
    /* Mark all transfers of that socket as dirty */
    Curl_multi_ev_dirty_xfers(multi, s);
  }
  अन्यथा {
    /* Asked to run due to time-out. Clear the 'last_expire_ts' variable to
       force Curl_update_timer() to trigger a callback to the app again even
       if the same timeout is still the one to run after this call. That
       handles the case when the application asks libcurl to run the timeout
       prematurely. */
    multi->चरमः_expire_विस्थापनम्_us = 0;

    /* Applications may set `socket_cb` *after* having added transfers
     * first. *Then* kick off processing with a
     * curl_multi_socket_action(TIMEOUT) afterwards. Make sure our
     * admin handle registers its pollset with the callbacks present. */
    mresult = multi_assess_wakeup(multi);
    यदि(mresult)
      गच्छ बहिर्गमनम्;
  }

  curlx_pnow(&now);
  multi_mark_expired_as_dirty(multi, &now);
  mresult = multi_सततम्_dirty(multi, &pipe_ctx, &सततम्_xfers);
  यदि(mresult)
    गच्छ बहिर्गमनम्;

  यदि(सततम्_xfers) {
    /* Running transfers takes time. With a new timestamp, we might catch
     * other expires which are due now. Instead of telling the application
     * to set a 0 timeout and call us again, we run them here.
     * Do that only once or it might be unfair to transfers on other
     * sockets. */
    curlx_pnow(&now);
    multi_mark_expired_as_dirty(multi, &now);
    mresult = multi_सततम्_dirty(multi, &pipe_ctx, &सततम्_xfers);
  }

बहिर्गमनम्:
  sigpipe_restore(&pipe_ctx);

  यदि(multi_ischanged(multi, TRUE))
    multi_कालसारणी_pending(multi);

  यदि(!mresult && CURL_MNTFY_HAS_ENTRIES(multi))
    mresult = Curl_mntfy_dispatch_all(multi);

  यदि(running_handles) {
    uint32_t running = multi_xfers_running(multi);
    *running_handles = (running < अधिकतम_पूर्णाङ्कः) ? (पूर्णाङ्क)running : अधिकतम_पूर्णाङ्कः;
  }

  यदि(CURLM_OK >= mresult)
    mresult = Curl_update_timer(multi);
  प्रत्यागम mresult;
}

#अपरिभाषय curl_multi_setopt
CURLMcode curl_multi_setopt(CURLM *m, CURLMoption option, ...)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_setopt, &mresult)) {
    संरचना Curl_multi *multi = m;
    va_list param;
    अचिह्नित दीर्घ uarg;
    size_t szarg;

    va_start(param, option);

    विकल्प(option) {
    पक्ष CURLMOPT_SOCKETFUNCTION:
      multi->socket_cb = va_arg(param, curl_socket_callback);
      भङ्ग;
    पक्ष CURLMOPT_SOCKETDATA:
      multi->socket_userp = va_arg(param, शून्य *);
      भङ्ग;
    पक्ष CURLMOPT_PUSHFUNCTION:
      multi->प्रक्षिप_cb = va_arg(param, curl_प्रक्षिप_callback);
      भङ्ग;
    पक्ष CURLMOPT_PUSHDATA:
      multi->प्रक्षिप_userp = va_arg(param, शून्य *);
      भङ्ग;
    पक्ष CURLMOPT_PIPELINING:
      multi->multiplexing = va_arg(param, दीर्घ) & CURLPIPE_MULTIPLEX ? 1 : 0;
      भङ्ग;
    पक्ष CURLMOPT_TIMERFUNCTION:
      multi->timer_cb = va_arg(param, curl_multi_timer_callback);
      भङ्ग;
    पक्ष CURLMOPT_TIMERDATA:
      multi->timer_userp = va_arg(param, शून्य *);
      भङ्ग;
    पक्ष CURLMOPT_MAXCONNECTS:
      uarg = va_arg(param, अचिह्नित दीर्घ);
      यदि(uarg <= UINT32_MAX)
        multi->maxconnects = (uint32_t)uarg;
      भङ्ग;
    पक्ष CURLMOPT_MAX_HOST_CONNECTIONS:
      यदि(!curlx_sltouz(va_arg(param, दीर्घ), &szarg))
        mresult = CURLM_BAD_FUNCTION_तर्कः;
      multi->max_host_connections = (szarg < UINT32_MAX) ?
                                    (uint32_t)szarg : UINT32_MAX;
      भङ्ग;
    पक्ष CURLMOPT_MAX_योग_CONNECTIONS:
      यदि(!curlx_sltouz(va_arg(param, दीर्घ), &szarg))
        mresult = CURLM_BAD_FUNCTION_तर्कः;
      multi->max_योग_connections = (szarg < UINT32_MAX) ?
                                     (uint32_t)szarg : UINT32_MAX;
      भङ्ग;
      /* options formerly used for pipelining */
    पक्ष CURLMOPT_MAX_PIPELINE_दीर्घता:
      भङ्ग;
    पक्ष CURLMOPT_CONTENT_दीर्घता_PENALTY_आकार:
      भङ्ग;
    पक्ष CURLMOPT_CHUNK_दीर्घता_PENALTY_आकार:
      भङ्ग;
    पक्ष CURLMOPT_PIPELINING_SITE_BL:
      भङ्ग;
    पक्ष CURLMOPT_PIPELINING_SERVER_BL:
      भङ्ग;
    पक्ष CURLMOPT_MAX_CONCURRENT_STREAMS: {
      यदि(!curlx_sltouz(va_arg(param, दीर्घ), &szarg) ||
         !szarg || (szarg > (size_t)अधिकतम_पूर्णाङ्कः)) /* preserve previous cutoff */
        multi->max_concurrent_streams = 100;
      अन्यथा
        multi->max_concurrent_streams = (szarg < UINT32_MAX) ?
                                       (uint32_t)szarg : UINT32_MAX;
      भङ्ग;
    }
    पक्ष CURLMOPT_जालम्_परिवर्तितम्: {
      दीर्घ val = va_arg(param, दीर्घ);
      यदि(val & CURLMNWC_CLEAR_ALL)
        /* In the beginning, all values available to set were 1 by mistake. We
           converted this to mean "all", thus setting all the bits
           automatically */
        val = CURLMNWC_CLEAR_DNS | CURLMNWC_CLEAR_CONNS;
      यदि(val & CURLMNWC_CLEAR_DNS) {
        Curl_dnscache_clear(multi->admin);
      }
      यदि(val & CURLMNWC_CLEAR_CONNS) {
        Curl_cpool_nw_परिवर्तितम्(&multi->cpool, multi->admin);
      }
      भङ्ग;
    }
    पक्ष CURLMOPT_NOTIFYFUNCTION:
      multi->ntfy.ntfy_cb = va_arg(param, curl_notify_callback);
      भङ्ग;
    पक्ष CURLMOPT_NOTIFYDATA:
      multi->ntfy.ntfy_cb_दत्त = va_arg(param, शून्य *);
      भङ्ग;
    पक्ष CURLMOPT_RESOLVE_तन्तवः_MAX:
#यदि_परिभाषितम् USE_RESOLV_THREADED
      uarg = va_arg(param, दीर्घ);
      यदि((uarg <= 0) || (uarg > UINT32_MAX))
        mresult = CURLM_BAD_FUNCTION_तर्कः;
      अन्यथा {
        CURLcode परिणाम = Curl_async_thrdd_multi_समुच्चयः_props(
          multi, 0, (uint32_t)uarg, 2000);
        विकल्प(परिणाम) {
        पक्ष CURLE_OK:
          mresult = CURLM_OK;
          भङ्ग;
        पक्ष CURLE_BAD_FUNCTION_तर्कः:
          mresult = CURLM_BAD_FUNCTION_तर्कः;
          भङ्ग;
        पक्ष CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः:
          mresult = CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
          भङ्ग;
        स्वभाव:
          mresult = CURLM_आन्तरिकम्_ERROR;
          भङ्ग;
        }
      }
#यदि_अन्तः
      भङ्ग;
    पक्ष CURLMOPT_शीघ्र_EXIT:
      multi->quick_exit = va_arg(param, दीर्घ) ? 1 : 0;
      भङ्ग;
    स्वभाव:
      mresult = CURLM_UNKNOWN_OPTION;
      भङ्ग;
    }
    va_end(param);
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

/* we define curl_multi_socket() in the public multi.h header */
#अपरिभाषय curl_multi_socket

CURLMcode curl_multi_socket(CURLM *m, curl_socket_t s, पूर्णाङ्क *running_handles)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_socket, &mresult)) {
    mresult = multi_socket(m, FALSE, s, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

CURLMcode curl_multi_socket_action(CURLM *m, curl_socket_t s,
                                   पूर्णाङ्क ev_bitmask, पूर्णाङ्क *running_handles)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_socket_action, &mresult)) {
    mresult = multi_socket(m, FALSE, s, ev_bitmask, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

CURLMcode curl_multi_socket_all(CURLM *m, पूर्णाङ्क *running_handles)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_socket_all, &mresult)) {
    mresult = multi_socket(m, TRUE, CURL_SOCKET_BAD, 0, running_handles);
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

स्थिर bool multi_has_dirties(संरचना Curl_multi *multi)
{
  uint32_t mid;
  यदि(Curl_uint32_bset_first(&multi->dirty, &mid)) {
    कुरु {
      संरचना Curl_easy *दत्त = Curl_multi_get_easy(multi, mid);
      यदि(दत्त) {
        यदि(Curl_uint32_bset_अन्तर्भवति(&multi->process, mid))
          प्रत्यागम TRUE;
        /* We are no longer processing this transfer */
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
      अन्यथा {
        CURL_TRC_M(multi->admin, "dirty transfer %u no longer found", mid);
        Curl_uint32_bset_remove(&multi->dirty, mid);
      }
    } यावत्(Curl_uint32_bset_अग्र(&multi->dirty, mid, &mid));
  }
  प्रत्यागम FALSE;
}

स्थिर शून्य multi_timeout(संरचना Curl_multi *multi,
                          timediff_t *pexire_विस्थापनम्_us,
                          पूर्णाङ्क *timeout_ms)
{
  यदि(multi->dead) {
    यदि(pexire_विस्थापनम्_us)
      *pexire_विस्थापनम्_us = 0;
    *timeout_ms = 0;
    प्रत्यागम;
  }

  यदि(multi_has_dirties(multi)) {
    यदि(pexire_विस्थापनम्_us) {
      संरचना curltime now;
      curlx_pnow(&now);
      *pexire_विस्थापनम्_us = Curl_timeouts_विस्थापनम्_us(&multi->timeouts, &now);
    }
    *timeout_ms = 0;
    प्रत्यागम;
  }
  अन्यथा {
    संरचना curltime now;
    uint32_t mid;

    curlx_pnow(&now);
    *timeout_ms = Curl_timeouts_अग्र_ms(&multi->timeouts, &now,
                                        pexire_विस्थापनम्_us, &mid);
#यदि_परिभाषितम् CURLVERBOSE
    यदि(mid != UINT32_MAX) {
      संरचना Curl_easy *दत्त = Curl_multi_get_easy(multi, mid);
      यदि(दत्त && CURL_TRC_TIMER_is_verbose(दत्त) &&
         (दत्त->state.timeouts.first < EXPIRE_चरमः)) {
        CURL_TRC_TIMER(दत्त, दत्त->state.timeouts.first,
                       "gives multi timeout in %dms", *timeout_ms);
      }
    }
#यदि_अन्तः
  }
}

CURLMcode curl_multi_timeout(CURLM *m,
                             दीर्घ *timeout_ms)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_timeout, &mresult)) {
    पूर्णाङ्क itimeout_ms;

    multi_timeout(m, रिक्त, &itimeout_ms);
    *timeout_ms = (दीर्घ)itimeout_ms;
    mresult = CURLM_OK;
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

/*
 * Tell the application it should update its timers, if it subscribes to the
 * update timer callback.
 */
CURLMcode Curl_update_timer(संरचना Curl_multi *multi)
{
  timediff_t timeouts_विस्थापनम्_us = 0;
  पूर्णाङ्क timeout_ms;
  पूर्णाङ्क rc;
  bool समुच्चयः_मानम् = FALSE;

  यदि(!multi->timer_cb || multi->dead)
    प्रत्यागम CURLM_OK;
  multi_timeout(multi, &timeouts_विस्थापनम्_us, &timeout_ms);

  यदि(timeout_ms < 0 && !multi->चरमः_timeout_समुच्चयः) {
    /* nothing to do */
  }
  अन्यथा यदि(timeout_ms < 0) {
    /* there is no timeout now but there was one previously */
    CURL_TRC_M(multi->admin, "[TIMER] clear");
    timeout_ms = -1; /* normalize */
    समुच्चयः_मानम् = TRUE;
  }
  अन्यथा यदि(!multi->चरमः_timeout_समुच्चयः) {
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, none before", timeout_ms);
    समुच्चयः_मानम् = TRUE;
  }
  अन्यथा यदि(multi->चरमः_expire_विस्थापनम्_us != timeouts_विस्थापनम्_us) {
    /* We had a timeout before and have one now, the absolute timestamp
     * differs. The relative timeout_ms may be the same, but the starting
     * point differs. Let the application restart its timer. */
    CURL_TRC_M(multi->admin, "[TIMER] set %dms, replace previous",
               timeout_ms);
    समुच्चयः_मानम् = TRUE;
  }
  अन्यथा {
    /* We have same expire time as previously. Our relative 'timeout_ms'
     * may be different now, but the application has the timer running
     * and we do not to tell it to start this again. */
  }

  यदि(समुच्चयः_मानम्) {
    संरचना Curl_mapi_guard guard;

    multi->चरमः_expire_विस्थापनम्_us = timeouts_विस्थापनम्_us;
    multi->चरमः_timeout_समुच्चयः = timeout_ms >= 0;
    CURL_CBAPI_MULTI_START(&guard, multi, multi_timer_cb);
    rc = multi->timer_cb(multi, timeout_ms, multi->timer_userp);
    CURL_CBAPI_MULTI_अन्तः(&guard);
    यदि(rc == -1) {
      multi->dead = TRUE;
      प्रत्यागम CURLM_ABORTED_BY_CALLBACK;
    }
  }
  प्रत्यागम CURLM_OK;
}

#यदि_परिभाषितम् DEBUGBUILD
स्थिर bool multi_timeouts_check(संरचना Curl_easy *दत्त)
{
  संरचना expire_timers *timeouts = &दत्त->state.timeouts;
  uint8_t id;
  पूर्णाङ्क i = 0;
  प्रति(id = timeouts->first; id < EXPIRE_चरमः; id = timeouts->अग्र[id]) {
    यदि(++i >= EXPIRE_चरमः) {
      failf(दत्त, "expire timeouts looped: %d iterations and no end", i);
      प्रत्यागम FALSE;
    }
    यदि(id == timeouts->अग्र[id]) {
      failf(दत्त, "expire timeouts wrong: %d points to itself", (पूर्णाङ्क)id);
      प्रत्यागम FALSE;
    }
    यदि((timeouts->अग्र[id] < EXPIRE_चरमः) &&
       (timeouts->विस्थापनम्_us[id] > timeouts->विस्थापनम्_us[timeouts->अग्र[id]])) {
      failf(दत्त, "expire timeouts not sorted: %d happens after %d but "
            "is listed before", (पूर्णाङ्क)id, (पूर्णाङ्क)timeouts->अग्र[id]);
      प्रत्यागम FALSE;
    }
  }
  प्रत्यागम TRUE;
}
#यदि_अन्तः

/*
 * Remove a given timestamp from the list of timeouts.
 */
स्थिर शून्य multi_clear_timeout(संरचना Curl_easy *दत्त, expire_id eid)
{
  संरचना expire_timers *timeouts = &दत्त->state.timeouts;
  uint8_t orig_first = timeouts->first;
  uint8_t *anchor = &timeouts->first;
  uint8_t id = (uint8_t)eid;

  यदि((अचिह्नित)eid >= EXPIRE_चरमः) {
    DEBUGASSERT(0);
    प्रत्यागम;
  }

  यावत्(*anchor < EXPIRE_चरमः) {
    यदि(*anchor == id) {
      *anchor = timeouts->अग्र[id];
      भङ्ग;
    }
    anchor = &timeouts->अग्र[*anchor];
  }
  DEBUGASSERT(multi_timeouts_check(दत्त));
  यदि(Curl_timeouts_has(दत्त)) {
    संरचना Curl_multi *multi = दत्त->multi;

    यदि(!multi) {
      DEBUGASSERT(0);
      प्रत्यागम;
    }
    यदि((timeouts->first >= EXPIRE_चरमः) || /* no more timeouts */
       (timeouts->first != orig_first)) {  /* active timeout changed */
      Curl_timeouts_remove(&multi->timeouts, दत्त);
    }
    यदि((timeouts->first < EXPIRE_चरमः) && !Curl_timeouts_has(दत्त)) {
      Curl_timeouts_सङ्कलय(&multi->timeouts, दत्त,
                        timeouts->विस्थापनम्_us[timeouts->first]);
    }
  }
}

/*
 * Add a timestamp to the list of timeouts. Keep the list sorted so that head
 * of list is always the timeout nearest in time.
 */
स्थिर CURLMcode multi_समुच्चयः_timeout(संरचना Curl_easy *दत्त,
                                   ध्रुव संरचना curltime *stamp,
                                   expire_id eid)
{
  संरचना expire_timers *timeouts = &दत्त->state.timeouts;
  uint8_t *anchor = &timeouts->first;
  uint8_t id = (uint8_t)eid;

  यदि((अचिह्नित)eid >= EXPIRE_चरमः) {
    DEBUGASSERT(0);
    प्रत्यागम CURLM_BAD_FUNCTION_तर्कः;
  }
  /* remove from list, store time and re-insert */
  multi_clear_timeout(दत्त, eid);
  timeouts->विस्थापनम्_us[id] =
    Curl_timeouts_विस्थापनम्_us(&दत्त->multi->timeouts, stamp);

  यावत्(*anchor < EXPIRE_चरमः) {
    यदि(timeouts->विस्थापनम्_us[*anchor] > timeouts->विस्थापनम्_us[id])
      भङ्ग;
    anchor = &timeouts->अग्र[*anchor];
  }
  timeouts->अग्र[eid] = *anchor;
  timeouts->अग्र[eid] = *anchor;
  *anchor = id;
  DEBUGASSERT(multi_timeouts_check(दत्त));
  CURL_TRC_TIMER(दत्त, eid, "set for %" FMT_TIMEDIFF_T "us",
                 curlx_ptimediff_us(stamp, Curl_pgrs_now(दत्त)));
  प्रत्यागम CURLM_OK;
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
शून्य Curl_expire_समुच्चयः(संरचना Curl_easy *दत्त,
                     expire_id eid, timediff_t ms,
                     ध्रुव संरचना curltime *pnow)
{
  संरचना Curl_multi *multi = दत्त->multi;
  संरचना expire_timers *timeouts = &दत्त->state.timeouts;
  uint8_t पूर्व_id = timeouts->first;
  संरचना curltime समुच्चयः;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  यदि(!multi)
    प्रत्यागम;
  DEBUGASSERT(eid < EXPIRE_चरमः);
  यदि(ms > अधिकतम_पूर्णाङ्कः)
    /* Cap ridiculous timeouts, 31-bit ms is still 3.5 weeks. When the time
       goes to the user, it must fit in this size. */
    ms = अधिकतम_पूर्णाङ्कः;

  समुच्चयः = *pnow;
  समुच्चयः.tv_sec += (time_t)(ms / 1000); /* may be a 64 to 32-bit conversion */
  समुच्चयः.tv_usec += (पूर्णाङ्क)(ms % 1000) * 1000;
  यदि(समुच्चयः.tv_usec >= 1000000) {
    समुच्चयः.tv_sec++;
    समुच्चयः.tv_usec -= 1000000;
  }

  /* Add the timeout, will replace any previous value for this timer. */
  multi_समुच्चयः_timeout(दत्त, &समुच्चयः, eid);
  DEBUGASSERT(timeouts->first < EXPIRE_चरमः);

  यदि(Curl_timeouts_has(दत्त)) {
    /* data has already a timeout registered. If the first timer
     * was NOT the one we just set AND is still the first one,
     * nothing changed from the timeouts point of view. The
     * set timer triggers after the one already registered. Leave. */
    यदि((पूर्व_id != eid) && (पूर्व_id == timeouts->first))
      प्रत्यागम;

    /* Since this is an updated time, we must remove data from
     * timeouts and then add it again. */
    Curl_timeouts_remove(&multi->timeouts, दत्त);
  }

  /* Insert the new timer expiry since it is our local minimum. */
  Curl_timeouts_सङ्कलय(&multi->timeouts, दत्त,
                    timeouts->विस्थापनम्_us[timeouts->first]);
}

शून्य Curl_expire(संरचना Curl_easy *दत्त,
                 timediff_t milli, expire_id eid)
{
  Curl_expire_समुच्चयः(दत्त, eid, milli, Curl_pgrs_now(दत्त));
}

/*
 * Removes the expire timer. Marks it as done.
 */
शून्य Curl_expire_clear(संरचना Curl_easy *दत्त, expire_id eid)
{
  /* remove the timer, if there */
  multi_clear_timeout(दत्त, eid);
  CURL_TRC_TIMER(दत्त, eid, "cleared");
}

/*
 * Clear ALL timeout values for this handle.
 */
शून्य Curl_expire_clear_all(संरचना Curl_easy *दत्त)
{
  संरचना Curl_multi *multi = दत्त->multi;

  /* this is only interesting while there is still an associated multi struct
     remaining! */
  यदि(!multi)
    प्रत्यागम;

  यदि(Curl_timeouts_remove(&multi->timeouts, दत्त)) {
    /* Since this is an cleared time, we must remove the previous entry from
       the splay tree */
    multi_timeouts_init(दत्त);

    यदि(दत्त->id >= 0)
      CURL_TRC_M(दत्त, "[TIMEOUT] all cleared");
  }
}

CURLMcode curl_multi_नियोजय(CURLM *m, curl_socket_t sockfd,
                            शून्य *sockp)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_नियोजय, &mresult)) {
    mresult = Curl_multi_ev_नियोजय(m, sockfd, sockp);
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

स्थिर शून्य move_pending_गन्तव्यम्_connect(संरचना Curl_multi *multi,
                                    संरचना Curl_easy *दत्त)
{
  DEBUGASSERT(दत्त->mstate == MSTATE_PENDING);

  /* Remove this node from the pending set, add into process set */
  Curl_uint32_bset_remove(&multi->pending, दत्त->mid);
  Curl_uint32_bset_सङ्कलय(&multi->process, दत्त->mid);

  multistate(दत्त, MSTATE_CONNECT);
  Curl_multi_mark_dirty(दत्त); /* make it run */
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
स्थिर शून्य multi_कालसारणी_pending(संरचना Curl_multi *multi)
{
  uint32_t mid = multi->चरमः_pending_mid;

  यदि(mid) {
    यावत्(Curl_uint32_bset_अग्र(&multi->pending, mid, &mid)) {
      संरचना Curl_easy *दत्त = Curl_multi_get_easy(multi, mid);
      यदि(दत्त) {
        move_pending_गन्तव्यम्_connect(multi, दत्त);
        multi->चरमः_pending_mid = mid;
        प्रत्यागम;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    }
    /* found no pending transfers with `mid` larger than `last_pending_mid`.
     * Start at the beginning of the pending set again. */
    multi->चरमः_pending_mid = 0;
  }

  यदि(Curl_uint32_bset_first(&multi->pending, &mid)) {
    कुरु {
      संरचना Curl_easy *दत्त = Curl_multi_get_easy(multi, mid);
      यदि(दत्त) {
        move_pending_गन्तव्यम्_connect(multi, दत्त);
        multi->चरमः_pending_mid = mid;
        प्रत्यागम;
      }
      /* transfer no longer known, should not happen */
      Curl_uint32_bset_remove(&multi->pending, mid);
      DEBUGASSERT(0);
    } यावत्(Curl_uint32_bset_अग्र(&multi->pending, mid, &mid));
  }
}

uint32_t Curl_multi_max_concurrent_streams(संरचना Curl_multi *multi)
{
  DEBUGASSERT(multi);
  प्रत्यागम multi->max_concurrent_streams;
}

CURL **curl_multi_get_handles(CURLM *m)
{
  संरचना Curl_mapi_guard guard;
  CURL **a = रिक्त;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_get_handles, रिक्त)) {
    संरचना Curl_multi *multi = m;
    शून्य *प्रविष्टिः;
    size_t गणनम् = Curl_uint32_tbl_गणनम्(&multi->xfers);

    a = curlx_malloc(परिमाण(संरचना Curl_easy *) * (गणनम् + 1));
    यदि(a) {
      अचिह्नित पूर्णाङ्क i = 0;
      uint32_t mid;

      यदि(Curl_uint32_tbl_first(&multi->xfers, &mid, &प्रविष्टिः)) {
        कुरु {
          संरचना Curl_easy *दत्त = प्रविष्टिः;
          DEBUGASSERT(i < गणनम्);
          यदि(!दत्त->state.आन्तरिकम्)
            a[i++] = दत्त;
        } यावत्(Curl_uint32_tbl_अग्र(&multi->xfers, mid, &mid, &प्रविष्टिः));
      }
      a[i] = रिक्त; /* last entry is a NULL */
    }
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम a;
}

CURLMcode curl_multi_get_offt(CURLM *m,
                              CURLMinfo_offt info,
                              curl_off_t *pvalue)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_get_offt, &mresult)) {
    संरचना Curl_multi *multi = m;
    uint32_t n;

    यदि(!pvalue) {
      mresult = CURLM_BAD_FUNCTION_तर्कः;
      गच्छ बहिर्गमनम्;
    }

    विकल्प(info) {
    पक्ष CURLMINFO_XFERS_CURRENT:
      n = Curl_uint32_tbl_गणनम्(&multi->xfers);
      यदि(n && multi->admin)
        --n;
      *pvalue = (curl_off_t)n;
      भङ्ग;
    पक्ष CURLMINFO_XFERS_RUNNING:
      n = Curl_uint32_bset_गणनम्(&multi->process);
      यदि(n && Curl_uint32_bset_अन्तर्भवति(&multi->process, multi->admin->mid))
        --n;
      *pvalue = (curl_off_t)n;
      भङ्ग;
    पक्ष CURLMINFO_XFERS_PENDING:
      *pvalue = (curl_off_t)Curl_uint32_bset_गणनम्(&multi->pending);
      भङ्ग;
    पक्ष CURLMINFO_XFERS_समाप्तम्:
      *pvalue = (curl_off_t)Curl_uint32_bset_गणनम्(&multi->msgsent);
      भङ्ग;
    पक्ष CURLMINFO_XFERS_ADDED:
      *pvalue = multi->xfers_योग_ever;
      भङ्ग;
    स्वभाव:
      *pvalue = -1;
      mresult = CURLM_UNKNOWN_OPTION;
      भङ्ग;
    }
  }
बहिर्गमनम्:
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

स्थिर संरचना Curl_fixed_buf *fixed_buf_create(size_t len)
{
  संरचना Curl_fixed_buf *fbuf;
  यदि((SIZE_MAX - परिमाण(*fbuf)) < len)
    प्रत्यागम रिक्त; /* too large */
  fbuf = curlx_malloc(len + परिमाण(*fbuf));
  यदि(fbuf)
    fbuf->len = len;
  प्रत्यागम fbuf;
}

CURLcode Curl_multi_xfer_buf_borrow(संरचना Curl_easy *दत्त,
                                    वर्ण **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->multi);
  *pbuf = रिक्त;
  *pbuflen = 0;
  यदि(!दत्त->multi) {
    failf(दत्त, "transfer has no multi handle");
    प्रत्यागम CURLE_FAILED_INIT;
  }
  यदि(!दत्त->समुच्चयः.बफर_आकार) {
    failf(दत्त, "transfer buffer size is 0");
    प्रत्यागम CURLE_FAILED_INIT;
  }
  यदि(दत्त->multi->xfer_buf_borrowed) {
    failf(दत्त, "attempt to borrow xfer_buf when already borrowed");
    प्रत्यागम CURLE_AGAIN;
  }

  यदि(दत्त->multi->xfer_buf &&
     दत्त->समुच्चयः.बफर_आकार > दत्त->multi->xfer_buf->len) {
    /* not large enough, get a new one */
    curlx_safefree(दत्त->multi->xfer_buf);
  }

  यदि(!दत्त->multi->xfer_buf) {
    दत्त->multi->xfer_buf =
      fixed_buf_create(curlx_uitouz(दत्त->समुच्चयः.बफर_आकार));
    यदि(!दत्त->multi->xfer_buf) {
      failf(दत्त, "could not allocate xfer_buf of %u bytes",
            दत्त->समुच्चयः.बफर_आकार);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }

  दत्त->multi->xfer_buf_borrowed = TRUE;
  *pbuf = दत्त->multi->xfer_buf->दत्त;
  *pbuflen = दत्त->multi->xfer_buf->len;
  प्रत्यागम CURLE_OK;
}

शून्य Curl_multi_xfer_buf_release(संरचना Curl_easy *दत्त, वर्ण *buf)
{
  (शून्य)buf;
  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->multi);
  DEBUGASSERT(!buf || (दत्त->multi->xfer_buf &&
                       दत्त->multi->xfer_buf->दत्त == buf));
  दत्त->multi->xfer_buf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_ulbuf_borrow(संरचना Curl_easy *दत्त,
                                      वर्ण **pbuf, size_t *pbuflen)
{
  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->multi);
  *pbuf = रिक्त;
  *pbuflen = 0;
  यदि(!दत्त->multi) {
    failf(दत्त, "transfer has no multi handle");
    प्रत्यागम CURLE_FAILED_INIT;
  }
  यदि(दत्त->multi->xfer_ulbuf_borrowed) {
    failf(दत्त, "attempt to borrow xfer_ulbuf when already borrowed");
    प्रत्यागम CURLE_AGAIN;
  }

  यदि(दत्त->multi->xfer_ulbuf &&
     दत्त->समुच्चयः.upload_बफर_आकार > दत्त->multi->xfer_ulbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(दत्त->multi->xfer_ulbuf);
  }

  यदि(!दत्त->multi->xfer_ulbuf) {
    दत्त->multi->xfer_ulbuf =
      fixed_buf_create(curlx_uitouz(दत्त->समुच्चयः.upload_बफर_आकार));
    यदि(!दत्त->multi->xfer_ulbuf) {
      failf(दत्त, "could not allocate xfer_ulbuf of %u bytes",
            दत्त->समुच्चयः.upload_बफर_आकार);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }

  दत्त->multi->xfer_ulbuf_borrowed = TRUE;
  *pbuf = दत्त->multi->xfer_ulbuf->दत्त;
  *pbuflen = दत्त->multi->xfer_ulbuf->len;
  प्रत्यागम CURLE_OK;
}

शून्य Curl_multi_xfer_ulbuf_release(संरचना Curl_easy *दत्त, वर्ण *buf)
{
  (शून्य)buf;
  DEBUGASSERT(दत्त);
  DEBUGASSERT(दत्त->multi);
  DEBUGASSERT(!buf || (दत्त->multi->xfer_ulbuf &&
                       दत्त->multi->xfer_ulbuf->दत्त == buf));
  दत्त->multi->xfer_ulbuf_borrowed = FALSE;
}

CURLcode Curl_multi_xfer_sockbuf_borrow(संरचना Curl_easy *दत्त,
                                        size_t blen, वर्ण **pbuf)
{
  DEBUGASSERT(दत्त);
  *pbuf = रिक्त;
  यदि(!दत्त->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    *pbuf = curlx_malloc(blen);
    प्रत्यागम *pbuf ? CURLE_OK : CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  यदि(दत्त->multi->xfer_sockbuf_borrowed) {
    failf(दत्त, "attempt to borrow xfer_sockbuf when already borrowed");
    प्रत्यागम CURLE_AGAIN;
  }

  यदि(दत्त->multi->xfer_sockbuf && blen > दत्त->multi->xfer_sockbuf->len) {
    /* not large enough, get a new one */
    curlx_safefree(दत्त->multi->xfer_sockbuf);
  }

  यदि(!दत्त->multi->xfer_sockbuf) {
    दत्त->multi->xfer_sockbuf = fixed_buf_create(blen);
    यदि(!दत्त->multi->xfer_sockbuf) {
      failf(दत्त, "could not allocate xfer_sockbuf of %zu bytes", blen);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }

  दत्त->multi->xfer_sockbuf_borrowed = TRUE;
  *pbuf = दत्त->multi->xfer_sockbuf->दत्त;
  प्रत्यागम CURLE_OK;
}

शून्य Curl_multi_xfer_sockbuf_release(संरचना Curl_easy *दत्त, वर्ण *buf)
{
  DEBUGASSERT(दत्त);
  यदि(!दत्त->multi) {
    /* When a SHARE gets destroyed and has a connection pool, we get
     * call with share->admin which does not have a multi handle. */
    curlx_free(buf);
  }
  अन्यथा {
    DEBUGASSERT(!buf || (दत्त->multi->xfer_sockbuf &&
                         दत्त->multi->xfer_sockbuf->दत्त == buf));
    दत्त->multi->xfer_sockbuf_borrowed = FALSE;
  }
}

स्थिर शून्य multi_xfer_bufs_free(संरचना Curl_multi *multi)
{
  DEBUGASSERT(multi);
  curlx_safefree(multi->xfer_buf);
  multi->xfer_buf_borrowed = FALSE;
  curlx_safefree(multi->xfer_ulbuf);
  multi->xfer_ulbuf_borrowed = FALSE;
  curlx_safefree(multi->xfer_sockbuf);
  multi->xfer_sockbuf_borrowed = FALSE;
}

संरचना Curl_easy *Curl_multi_get_easy(संरचना Curl_multi *multi,
                                      uint32_t mid)
{
  संरचना Curl_easy *दत्त = Curl_uint32_tbl_get(&multi->xfers, mid);
  यदि(GOOD_EASY_HANDLE(दत्त))
    प्रत्यागम दत्त;
  CURL_TRC_M(multi->admin, "invalid easy handle in xfer table for mid=%u",
             mid);
  Curl_uint32_tbl_remove(&multi->xfers, mid);
  प्रत्यागम रिक्त;
}

bool Curl_multi_knows_easy(संरचना Curl_multi *multi, संरचना Curl_easy *दत्त)
{
  प्रत्यागम Curl_uint32_tbl_get(&multi->xfers, दत्त->mid) == दत्त;
}

स्थिर uint32_t multi_xfers_running(संरचना Curl_multi *multi)
{
  यदि(!multi) {
    DEBUGASSERT(0);
    प्रत्यागम 0;
  }
  प्रत्यागम multi->xfers_alive;
}

uint32_t Curl_multi_xfers_attached(संरचना Curl_multi *multi)
{
  यदि(!multi || !multi->admin) {
    DEBUGASSERT(0);
    प्रत्यागम 0;
  }
  /* Discount the admin handle */
  प्रत्यागम Curl_uint32_tbl_गणनम्(&multi->xfers) - 1;
}

शून्य Curl_multi_mark_dirty(संरचना Curl_easy *दत्त)
{
  यदि(दत्त->multi && दत्त->mid != UINT32_MAX)
    Curl_uint32_bset_सङ्कलय(&दत्त->multi->dirty, दत्त->mid);
}

शून्य Curl_multi_clear_dirty(संरचना Curl_easy *दत्त)
{
  यदि(दत्त->multi && दत्त->mid != UINT32_MAX)
    Curl_uint32_bset_remove(&दत्त->multi->dirty, दत्त->mid);
}

CURLMcode curl_multi_notify_enable(CURLM *m, अचिह्नित पूर्णाङ्क notification)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_notify_enable, &mresult)) {
    mresult = Curl_mntfy_enable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

CURLMcode curl_multi_notify_disable(CURLM *m, अचिह्नित पूर्णाङ्क notification)
{
  संरचना Curl_mapi_guard guard;
  CURLMcode mresult = CURLM_OK;

  यदि(CURL_MAPI_ENTER(&guard, m, multi_notify_disable, &mresult)) {
    mresult = Curl_mntfy_disable(m, notification);
  }
  CURL_MAPI_LEAVE(&guard);
  प्रत्यागम mresult;
}

#यदि_परिभाषितम् DEBUGBUILD
स्थिर शून्य multi_xfer_dump(संरचना Curl_multi *multi, uint32_t mid,
                            शून्य *प्रविष्टिः)
{
  संरचना Curl_easy *दत्त = प्रविष्टिः;

  (शून्य)multi;
  यदि(!दत्त) {
    curl_mfprintf(stderr, "mid=%u, entry=NULL, bug in xfer table?\n", mid);
  }
  अन्यथा {
    curl_mfprintf(stderr, "mid=%u, magic=%s, p=%p, id=%" FMT_OFF_T
                  ", url=%s\n",
                  mid,
                  (दत्त->magic == CURLEASY_MAGIC_सङ्ख्या) ? "GOOD" : "BAD!",
                  (शून्य *)दत्त, दत्त->id, Curl_bufref_ptr(&दत्त->state.url));
  }
}

स्थिर शून्य multi_xfer_tbl_dump(संरचना Curl_multi *multi)
{
  uint32_t mid;
  शून्य *प्रविष्टिः;
  curl_mfprintf(stderr, "=== multi xfer table (count=%u, capacity=%u\n",
                Curl_uint32_tbl_गणनम्(&multi->xfers),
                Curl_uint32_tbl_क्षमता(&multi->xfers));
  यदि(Curl_uint32_tbl_first(&multi->xfers, &mid, &प्रविष्टिः)) {
    multi_xfer_dump(multi, mid, प्रविष्टिः);
    यावत्(Curl_uint32_tbl_अग्र(&multi->xfers, mid, &mid, &प्रविष्टिः))
      multi_xfer_dump(multi, mid, प्रविष्टिः);
  }
  curl_mfprintf(stderr, "===\n");
  fflush(stderr);
}
#यदि_अन्तः /* DEBUGBUILD */
