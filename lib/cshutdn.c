/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Linus Nielsen Feltzing, <linus@haxx.se>
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
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multi_ev.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cshutdn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sigpipe.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

ਖਾਲੀ Curl_cshutdn_ਸ਼ੁਰੂ_timer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                              ਪੂਰਨ timeout_ms)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਅਚਲ ਬਣਤਰ curltime *pnow = Curl_pgrs_now(ਅੰਕੜੇ);

  DEBUGASSERT(conn);
  conn->shutdown.ਸ਼ੁਰੂ_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((ਅੰਕੜੇ->ਸੈੱਟ.shutdowntimeout > 0) ?
     ਅੰਕੜੇ->ਸੈੱਟ.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  ਜੇ(ਅੰਕੜੇ->mid)
    Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(ਅੰਕੜੇ, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t ਖੱਬਾ_ms;

  ਜੇ(conn->shutdown.ਸ਼ੁਰੂ_ms[sockindex] < 0)
    ਵਾਪਸ 0; /* not started or no limits */

  ਖੱਬਾ_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &conn->created) -
             conn->shutdown.ਸ਼ੁਰੂ_ms[sockindex]);
  ਵਾਪਸ ਖੱਬਾ_ms ? ਖੱਬਾ_ms : -1;
}

ਸਥਿਰ timediff_t cshutdn_conn_timeleft(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ connectdata *conn)
{
  timediff_t ਖੱਬਾ_ms = 0, ms;
  int8_t i;

  ਲਈ(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    ਜੇ(conn->shutdown.ਸ਼ੁਰੂ_ms[i] < 0)
      ਜਾਰੀ;
    ms = Curl_cshutdn_timeleft_ms(ਅੰਕੜੇ, conn, i);
    ਜੇ(ms && (!ਖੱਬਾ_ms || ms < ਖੱਬਾ_ms))
      ਖੱਬਾ_ms = ms;
  }
  ਵਾਪਸ ਖੱਬਾ_ms;
}

ਖਾਲੀ Curl_cshutdn_clear_timer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex)
{
  ਅੰਕੜੇ->conn->shutdown.ਸ਼ੁਰੂ_ms[sockindex] = -1;
}

ਸਥਿਰ ਖਾਲੀ cshutdn_ਲੜੀਵਾਰ_conn_handler(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ connectdata *conn)
{
  ਜੇ(!conn->ਬਿੱਟ.shutdown_handler) {

    ਜੇ(conn->scheme && conn->scheme->ਲੜੀਵਾਰ->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      ਜੇ(ਅੰਕੜੇ->state.ਅੰਦਰੂਨੀ) {
        ਅੰਕੜੇ->ਸੈੱਟ.timeout = DEFAULT_SHUTDOWN_TIMEOUT_MS;
        Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_STARTOP);
      }

      /* This is set if protocol-specific cleanups should be made */
      DEBUGF(infof(ਅੰਕੜੇ, "connection #%" FMT_OFF_T
                   ", shutdown protocol handler (aborted=%d)",
                   conn->connection_id, conn->ਬਿੱਟ.aborted));
      /* There are protocol handlers that block on retrieving
       * server responses here (FTP). Set a short timeout. */
      conn->scheme->ਲੜੀਵਾਰ->disconnect(ਅੰਕੜੇ, conn, (bool)conn->ਬਿੱਟ.aborted);
    }

    conn->ਬਿੱਟ.shutdown_handler = TRUE;
  }
}

CURLcode Curl_cshutdn_try_once_idx(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   int8_t sockindex, bool *ਪੂਰਾ)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(ਅੰਕੜੇ->conn);

  ਜੇ(!CONN_SOCK_IDX_VALID(sockindex))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  /* Get the first connected filter that is not shut down already. */
  cf = ਅੰਕੜੇ->conn->cfilter[sockindex];
  ਜਦੋਂ(cf && (!cf->connected || cf->shutdown))
    cf = cf->ਅਗਲਾ;

  ਜੇ(!cf) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  *ਪੂਰਾ = FALSE;
  ਜੇ(!CURL_CONN_ਵਿੱਚ_SHUTDOWN(ਅੰਕੜੇ->conn, sockindex)) {
    Curl_cshutdn_ਸ਼ੁਰੂ_timer(ਅੰਕੜੇ, sockindex, 0);
  }
  ਨਹੀਂਤਾਂ {
    timeout_ms = Curl_cshutdn_timeleft_ms(ਅੰਕੜੇ, ਅੰਕੜੇ->conn, sockindex);
    ਜੇ(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(ਅੰਕੜੇ, "shutdown timeout");
      ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
    }
  }

  ਜਦੋਂ(cf) {
    ਜੇ(!cf->shutdown) {
      bool cfdone = FALSE;
      ਨਤੀਜਾ = cf->cft->do_shutdown(cf, ਅੰਕੜੇ, &cfdone);
      ਜੇ(ਨਤੀਜਾ) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "shut down failed with %d", (ਪੂਰਨ)ਨਤੀਜਾ);
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(!cfdone) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "shut down not done yet");
        ਵਾਪਸ CURLE_OK;
      }
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->ਅਗਲਾ;
  }
  *ਪੂਰਾ = (!ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cshutdn_ਲੜੀਵਾਰ_once(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ connectdata *conn,
                             bool *ਪੂਰਾ)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(ਅੰਕੜੇ->conn == conn);

  ਜੇ(!CURL_CONN_ਵਿੱਚ_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_ਸ਼ੁਰੂ_timer(ਅੰਕੜੇ, FIRSTSOCKET, 0);
  }

  cshutdn_ਲੜੀਵਾਰ_conn_handler(ਅੰਕੜੇ, conn);

  ਜੇ(conn->ਬਿੱਟ.shutdown_filters) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ;
  }

  ਜੇ(!conn->ਬਿੱਟ.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(ਅੰਕੜੇ, FIRSTSOCKET, &done1);
  ਨਹੀਂਤਾਂ {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  ਜੇ(!conn->ਬਿੱਟ.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(ਅੰਕੜੇ, SECONDARYSOCKET, &done2);
  ਨਹੀਂਤਾਂ {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *ਪੂਰਾ = (r1 || r2 || (done1 && done2));
  ਜੇ(*ਪੂਰਾ)
    conn->ਬਿੱਟ.shutdown_filters = TRUE;
}

ਖਾਲੀ Curl_cshutdn_try_once(ਬਣਤਰ Curl_easy *admin,
                           ਬਣਤਰ connectdata *conn,
                           bool *ਪੂਰਾ)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_ਲੜੀਵਾਰ_once(admin, conn, ਪੂਰਾ);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *ਪੂਰਾ);
  Curl_detach_connection(admin);
}

ਖਾਲੀ Curl_cshutdn_terminate(ਬਣਤਰ Curl_easy *admin,
                            ਬਣਤਰ connectdata *conn,
                            bool do_shutdown)
{
  bool ਪੂਰਾ;

  /* there must be a connection to close */
  DEBUGASSERT(conn);
  /* it must be removed from the connection pool */
  DEBUGASSERT(conn->cpid == UINT32_MAX);
  /* the transfer must be detached from the connection */
  DEBUGASSERT(admin && !admin->conn);
  DEBUGASSERT(!admin->mid);

  Curl_attach_connection(admin, conn, FALSE);

  cshutdn_ਲੜੀਵਾਰ_conn_handler(admin, conn);
  ਜੇ(do_shutdown) {
    /* Make a last attempt to shutdown handlers and filters, if
     * not done so already. */
    cshutdn_ਲੜੀਵਾਰ_once(admin, conn, &ਪੂਰਾ);
  }
  CURL_TRC_M(admin, "[SHUTDOWN] %sclosing connection #%" FMT_OFF_T,
             conn->ਬਿੱਟ.shutdown_filters ? "" : "force ",
             conn->connection_id);
  Curl_conn_cf_discard_all(admin, conn, SECONDARYSOCKET);
  Curl_conn_cf_discard_all(admin, conn, FIRSTSOCKET);
  Curl_detach_connection(admin);

  ਜੇ(admin->multi)
    Curl_multi_ev_conn_ਪੂਰਾ(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  ਜੇ(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

ਸਥਿਰ size_t cshutdn_destroy_oldest(ਬਣਤਰ cshutdn *cshutdn,
                                     ਬਣਤਰ Curl_easy *admin,
                                     ਅਚਲ ਅੱਖਰ *destination,
                                     size_t ਗਿਣਤੀ_2)
{
  ਬਣਤਰ Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  ਜਦੋਂ((removed < ਗਿਣਤੀ_2) && (i < CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns))) {
    ਬਣਤਰ connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    ਜੇ(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    ਨਹੀਂਤਾਂ
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  ਵਾਪਸ removed;
}

size_t Curl_cshutdn_close_oldest(ਬਣਤਰ cshutdn *cshutdn,
                               ਬਣਤਰ Curl_easy *admin,
                               ਅਚਲ ਅੱਖਰ *destination,
                               size_t ਗਿਣਤੀ_2)
{
  ਜੇ(cshutdn) {
    ਵਾਪਸ cshutdn_destroy_oldest(cshutdn, admin, destination, ਗਿਣਤੀ_2);
  }
  ਵਾਪਸ 0;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਕ_POLLS_ON_ਢੇਰ 10

ਸਥਿਰ CURLcode cshutdn_wait(ਬਣਤਰ cshutdn *cshutdn,
                             ਬਣਤਰ Curl_easy *admin,
                             ਪੂਰਨ timeout_ms)
{
  ਬਣਤਰ pollfd a_few_on_ਢੇਰ[ਅੰਕ_POLLS_ON_ਢੇਰ];
  ਬਣਤਰ curl_pollfds cpfds;
  CURLcode ਨਤੀਜਾ;

  Curl_pollfds_init(&cpfds, a_few_on_ਢੇਰ, ਅੰਕ_POLLS_ON_ਢੇਰ);

  ਨਤੀਜਾ = Curl_cshutdn_ਜੋੜੋ_pollfds(cshutdn, admin, &cpfds);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

ਬਾਹਰਲਾ:
  Curl_pollfds_cleanup(&cpfds);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cshutdn_perform(ਬਣਤਰ cshutdn *cshutdn,
                            ਬਣਤਰ Curl_easy *admin,
                            ਬਣਤਰ Curl_sigpipe_ctx *sigpipe_ctx)
{
  ਬਣਤਰ connectdata *conn;
  timediff_t ਅਗਲਾ_expire_ms = 0, ms;
  uint32_t i = 0;
  bool ਪੂਰਾ;

  ਜੇ(!CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns))
    ਵਾਪਸ;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  ਜਦੋਂ(i < CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &ਪੂਰਾ);
    ਜੇ(ਪੂਰਾ) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    ਨਹੀਂਤਾਂ {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      ਜੇ(ms && (!ਅਗਲਾ_expire_ms || (ms < ਅਗਲਾ_expire_ms)))
        ਅਗਲਾ_expire_ms = ms;
    }
  }

  ਜੇ(ਅਗਲਾ_expire_ms)
    Curl_expire(admin, ਅਗਲਾ_expire_ms, EXPIRE_SHUTDOWN);
}

ਸਥਿਰ ਖਾਲੀ cshutdn_terminate_all(ਬਣਤਰ cshutdn *cshutdn,
                                  ਬਣਤਰ Curl_easy *admin,
                                  ਪੂਰਨ timeout_ms)
{
  ਬਣਤਰ curltime started = *Curl_pgrs_now(admin);
  ਬਣਤਰ Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  ਜਦੋਂ(CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns)) {
    timediff_t spent_ms;
    ਪੂਰਨ remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    ਜੇ(!CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      ਰੋਕੋ;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    ਜੇ(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      ਰੋਕੋ;
    }

    remain_ms = timeout_ms - (ਪੂਰਨ)spent_ms;
    ਜੇ(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      ਰੋਕੋ;
    }
  }

  /* Terminate any remaining. */
  ਜਦੋਂ(CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns)) {
    ਬਣਤਰ connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

ਖਾਲੀ Curl_cshutdn_init(ਬਣਤਰ cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

ਖਾਲੀ Curl_cshutdn_destroy(ਬਣਤਰ cshutdn *cshutdn,
                          ਬਣਤਰ Curl_easy *admin)
{
  ਜੇ(admin) {
    ਪੂਰਨ timeout_ms = 0;
    /* for testing, run graceful shutdown */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    {
      ਅਚਲ ਅੱਖਰ *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      ਜੇ(p) {
        curl_off_t l;
        ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &l, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ))
          timeout_ms = (ਪੂਰਨ)l;
      }
    }
#ਜੇ_ਅੰਤ
    ਜੇ(CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_ਗਿਣਤੀ(ਬਣਤਰ cshutdn *cshutdn)
{
  ਜੇ(cshutdn) {
    ਵਾਪਸ CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns);
  }
  ਵਾਪਸ 0;
}

size_t Curl_cshutdn_dest_ਗਿਣਤੀ(ਬਣਤਰ cshutdn *cshutdn,
                               ਅਚਲ ਅੱਖਰ *destination)
{
  ਜੇ(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    ਲਈ(i = 0; i < CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns); ++i) {
      ਬਣਤਰ connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      ਜੇ(!strcmp(destination, conn->destination))
        ++n;
    }
    ਵਾਪਸ n;
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ CURLMcode cshutdn_update_ev(ਬਣਤਰ Curl_multi *multi,
                                   ਬਣਤਰ connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  ਵਾਪਸ mresult;
}

ਖਾਲੀ Curl_cshutdn_ਜੋੜੋ(ਬਣਤਰ cshutdn *cshutdn,
                      ਬਣਤਰ Curl_multi *multi,
                      ਬਣਤਰ connectdata *conn,
                      size_t max_shutdowns)
{
  ਬਣਤਰ Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  ਜੇ(max_shutdowns <= CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, ਨਿਰਾ, 1);
  }

  ਜੇ(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    ਵਾਪਸ;
  }

  ਜੇ(Curl_ptrarray_ਜੋੜੋ(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    ਵਾਪਸ;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns));
}

ਖਾਲੀ Curl_cshutdn_perform(ਬਣਤਰ cshutdn *cshutdn,
                          ਬਣਤਰ Curl_easy *admin,
                          ਬਣਤਰ Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
ਖਾਲੀ Curl_cshutdn_setfds(ਬਣਤਰ cshutdn *cshutdn,
                         ਬਣਤਰ Curl_easy *admin,
                         fd_set *read_fd_ਸੈੱਟ, fd_set *write_fd_ਸੈੱਟ,
                         ਪੂਰਨ *maxfd)
{
  ਜੇ(CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns)) {
    ਬਣਤਰ easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    ਲਈ(i = 0; i < CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns); ++i) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ j;
      ਬਣਤਰ connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode ਨਤੀਜਾ;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      ਨਤੀਜਾ = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ਜੇ(ਨਤੀਜਾ)
        ਜਾਰੀ;

      ਲਈ(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        ਜੇ(!FDSET_SOCK(sock))
          ਜਾਰੀ;
        ਜੇ(ps.actions[j] & CURL_POLL_ਵਿੱਚ)
          FD_SET(sock, read_fd_ਸੈੱਟ);
        ਜੇ(ps.actions[j] & CURL_POLL_ਬਾਹਰਲਾ)
          FD_SET(sock, write_fd_ਸੈੱਟ);
        ਜੇ((ps.actions[j] & (CURL_POLL_ਬਾਹਰਲਾ | CURL_POLL_ਵਿੱਚ)) &&
           ((ਪੂਰਨ)sock > *maxfd))
          *maxfd = (ਪੂਰਨ)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ Curl_cshutdn_ਜੋੜੋ_waitfds(ਬਣਤਰ cshutdn *cshutdn,
                                      ਬਣਤਰ Curl_easy *admin,
                                      ਬਣਤਰ Curl_waitfds *cwfds)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ need = 0;

  ਜੇ(CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns)) {
    ਬਣਤਰ easy_pollset ps;
    ਬਣਤਰ connectdata *conn;
    uint32_t i;
    CURLcode ਨਤੀਜਾ;

    Curl_pollset_init(&ps);
    ਲਈ(i = 0; i < CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      ਨਤੀਜਾ = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ਜੇ(!ਨਤੀਜਾ)
        need += Curl_waitfds_ਜੋੜੋ_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  ਵਾਪਸ need;
}

CURLcode Curl_cshutdn_ਜੋੜੋ_pollfds(ਬਣਤਰ cshutdn *cshutdn,
                                  ਬਣਤਰ Curl_easy *admin,
                                  ਬਣਤਰ curl_pollfds *cpfds)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns)) {
    ਬਣਤਰ easy_pollset ps;
    ਬਣਤਰ connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    ਲਈ(i = 0; i < CURL_PTRARRAY_ਗਿਣਤੀ(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      ਨਤੀਜਾ = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = Curl_pollfds_ਜੋੜੋ_ps(cpfds, &ps);
      ਜੇ(ਨਤੀਜਾ) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}
