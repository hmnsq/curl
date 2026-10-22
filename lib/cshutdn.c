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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "multi_ev.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "cshutdn.h"
#ಅನ್ತರ್ಭಾವ "sigpipe.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"


#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

ಶೂನ್ಯ Curl_cshutdn_ಆರಂಭ_timer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                              ಪೂರ್ಣಾಂಕ timeout_ms)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);

  DEBUGASSERT(conn);
  conn->shutdown.ಆರಂಭ_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((ದತ್ತಾಂಶ->ಗಣ.shutdowntimeout > 0) ?
     ದತ್ತಾಂಶ->ಗಣ.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  ಆದರೆ(ದತ್ತಾಂಶ->mid)
    Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(ದತ್ತಾಂಶ, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t ಎಡ_ms;

  ಆದರೆ(conn->shutdown.ಆರಂಭ_ms[sockindex] < 0)
    ಹಿಂತಿರುಗಿ 0; /* not started or no limits */

  ಎಡ_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &conn->created) -
             conn->shutdown.ಆರಂಭ_ms[sockindex]);
  ಹಿಂತಿರುಗಿ ಎಡ_ms ? ಎಡ_ms : -1;
}

ಸ್ಥಿರ timediff_t cshutdn_conn_timeleft(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ connectdata *conn)
{
  timediff_t ಎಡ_ms = 0, ms;
  int8_t i;

  ಗಾಗಿ(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    ಆದರೆ(conn->shutdown.ಆರಂಭ_ms[i] < 0)
      ಮುಂದುವರಿ;
    ms = Curl_cshutdn_timeleft_ms(ದತ್ತಾಂಶ, conn, i);
    ಆದರೆ(ms && (!ಎಡ_ms || ms < ಎಡ_ms))
      ಎಡ_ms = ms;
  }
  ಹಿಂತಿರುಗಿ ಎಡ_ms;
}

ಶೂನ್ಯ Curl_cshutdn_clear_timer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex)
{
  ದತ್ತಾಂಶ->conn->shutdown.ಆರಂಭ_ms[sockindex] = -1;
}

ಸ್ಥಿರ ಶೂನ್ಯ cshutdn_ಸತತ_conn_handler(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ connectdata *conn)
{
  ಆದರೆ(!conn->ಬಿಟ್ಟುಗಳು.shutdown_handler) {

    ಆದರೆ(conn->scheme && conn->scheme->ಸತತ->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      ಆದರೆ(ದತ್ತಾಂಶ->state.ಆಂತರಿಕ) {
        ದತ್ತಾಂಶ->ಗಣ.timeout = DEFAULT_SHUTDOWN_TIMEOUT_MS;
        Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_STARTOP);
      }

      /* This is set if protocol-specific cleanups should be made */
      DEBUGF(infof(ದತ್ತಾಂಶ, "connection #%" FMT_OFF_T
                   ", shutdown protocol handler (aborted=%d)",
                   conn->connection_id, conn->ಬಿಟ್ಟುಗಳು.aborted));
      /* There are protocol handlers that block on retrieving
       * server responses here (FTP). Set a short timeout. */
      conn->scheme->ಸತತ->disconnect(ದತ್ತಾಂಶ, conn, (bool)conn->ಬಿಟ್ಟುಗಳು.aborted);
    }

    conn->ಬಿಟ್ಟುಗಳು.shutdown_handler = TRUE;
  }
}

CURLcode Curl_cshutdn_try_once_idx(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   int8_t sockindex, bool *ಮುಗಿದ)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(ದತ್ತಾಂಶ->conn);

  ಆದರೆ(!CONN_SOCK_IDX_VALID(sockindex))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  /* Get the first connected filter that is not shut down already. */
  cf = ದತ್ತಾಂಶ->conn->cfilter[sockindex];
  ಆಗ(cf && (!cf->connected || cf->shutdown))
    cf = cf->ಮುಂದಿನ;

  ಆದರೆ(!cf) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  *ಮುಗಿದ = FALSE;
  ಆದರೆ(!CURL_CONN_ಒಳ_SHUTDOWN(ದತ್ತಾಂಶ->conn, sockindex)) {
    Curl_cshutdn_ಆರಂಭ_timer(ದತ್ತಾಂಶ, sockindex, 0);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    timeout_ms = Curl_cshutdn_timeleft_ms(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn, sockindex);
    ಆದರೆ(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(ದತ್ತಾಂಶ, "shutdown timeout");
      ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
    }
  }

  ಆಗ(cf) {
    ಆದರೆ(!cf->shutdown) {
      bool cfdone = FALSE;
      ಫಲಿತಾಂಶ = cf->cft->do_shutdown(cf, ದತ್ತಾಂಶ, &cfdone);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shut down failed with %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!cfdone) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shut down not done yet");
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->ಮುಂದಿನ;
  }
  *ಮುಗಿದ = (!ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cshutdn_ಸತತ_once(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ connectdata *conn,
                             bool *ಮುಗಿದ)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(ದತ್ತಾಂಶ->conn == conn);

  ಆದರೆ(!CURL_CONN_ಒಳ_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_ಆರಂಭ_timer(ದತ್ತಾಂಶ, FIRSTSOCKET, 0);
  }

  cshutdn_ಸತತ_conn_handler(ದತ್ತಾಂಶ, conn);

  ಆದರೆ(conn->ಬಿಟ್ಟುಗಳು.shutdown_filters) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ(!conn->ಬಿಟ್ಟುಗಳು.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(ದತ್ತಾಂಶ, FIRSTSOCKET, &done1);
  ಇಲ್ಲದಿದ್ದರೆ {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  ಆದರೆ(!conn->ಬಿಟ್ಟುಗಳು.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(ದತ್ತಾಂಶ, SECONDARYSOCKET, &done2);
  ಇಲ್ಲದಿದ್ದರೆ {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *ಮುಗಿದ = (r1 || r2 || (done1 && done2));
  ಆದರೆ(*ಮುಗಿದ)
    conn->ಬಿಟ್ಟುಗಳು.shutdown_filters = TRUE;
}

ಶೂನ್ಯ Curl_cshutdn_try_once(ರಚನೆ Curl_easy *admin,
                           ರಚನೆ connectdata *conn,
                           bool *ಮುಗಿದ)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_ಸತತ_once(admin, conn, ಮುಗಿದ);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *ಮುಗಿದ);
  Curl_detach_connection(admin);
}

ಶೂನ್ಯ Curl_cshutdn_terminate(ರಚನೆ Curl_easy *admin,
                            ರಚನೆ connectdata *conn,
                            bool do_shutdown)
{
  bool ಮುಗಿದ;

  /* there must be a connection to close */
  DEBUGASSERT(conn);
  /* it must be removed from the connection pool */
  DEBUGASSERT(conn->cpid == UINT32_MAX);
  /* the transfer must be detached from the connection */
  DEBUGASSERT(admin && !admin->conn);
  DEBUGASSERT(!admin->mid);

  Curl_attach_connection(admin, conn, FALSE);

  cshutdn_ಸತತ_conn_handler(admin, conn);
  ಆದರೆ(do_shutdown) {
    /* Make a last attempt to shutdown handlers and filters, if
     * not done so already. */
    cshutdn_ಸತತ_once(admin, conn, &ಮುಗಿದ);
  }
  CURL_TRC_M(admin, "[SHUTDOWN] %sclosing connection #%" FMT_OFF_T,
             conn->ಬಿಟ್ಟುಗಳು.shutdown_filters ? "" : "force ",
             conn->connection_id);
  Curl_conn_cf_discard_all(admin, conn, SECONDARYSOCKET);
  Curl_conn_cf_discard_all(admin, conn, FIRSTSOCKET);
  Curl_detach_connection(admin);

  ಆದರೆ(admin->multi)
    Curl_multi_ev_conn_ಮುಗಿದ(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  ಆದರೆ(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

ಸ್ಥಿರ size_t cshutdn_destroy_oldest(ರಚನೆ cshutdn *cshutdn,
                                     ರಚನೆ Curl_easy *admin,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *destination,
                                     size_t ಎಣಿಕೆ_2)
{
  ರಚನೆ Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  ಆಗ((removed < ಎಣಿಕೆ_2) && (i < CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns))) {
    ರಚನೆ connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    ಆದರೆ(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  ಹಿಂತಿರುಗಿ removed;
}

size_t Curl_cshutdn_close_oldest(ರಚನೆ cshutdn *cshutdn,
                               ರಚನೆ Curl_easy *admin,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *destination,
                               size_t ಎಣಿಕೆ_2)
{
  ಆದರೆ(cshutdn) {
    ಹಿಂತಿರುಗಿ cshutdn_destroy_oldest(cshutdn, admin, destination, ಎಣಿಕೆ_2);
  }
  ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಅಂಕೆ_POLLS_ON_ರಾಶಿ 10

ಸ್ಥಿರ CURLcode cshutdn_wait(ರಚನೆ cshutdn *cshutdn,
                             ರಚನೆ Curl_easy *admin,
                             ಪೂರ್ಣಾಂಕ timeout_ms)
{
  ರಚನೆ pollfd a_few_on_ರಾಶಿ[ಅಂಕೆ_POLLS_ON_ರಾಶಿ];
  ರಚನೆ curl_pollfds cpfds;
  CURLcode ಫಲಿತಾಂಶ;

  Curl_pollfds_init(&cpfds, a_few_on_ರಾಶಿ, ಅಂಕೆ_POLLS_ON_ರಾಶಿ);

  ಫಲಿತಾಂಶ = Curl_cshutdn_ಕೂಡಿಸು_pollfds(cshutdn, admin, &cpfds);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

ಹೊರಹೋಗು:
  Curl_pollfds_cleanup(&cpfds);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cshutdn_perform(ರಚನೆ cshutdn *cshutdn,
                            ರಚನೆ Curl_easy *admin,
                            ರಚನೆ Curl_sigpipe_ctx *sigpipe_ctx)
{
  ರಚನೆ connectdata *conn;
  timediff_t ಮುಂದಿನ_expire_ms = 0, ms;
  uint32_t i = 0;
  bool ಮುಗಿದ;

  ಆದರೆ(!CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns))
    ಹಿಂತಿರುಗಿ;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  ಆಗ(i < CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &ಮುಗಿದ);
    ಆದರೆ(ಮುಗಿದ) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      ಆದರೆ(ms && (!ಮುಂದಿನ_expire_ms || (ms < ಮುಂದಿನ_expire_ms)))
        ಮುಂದಿನ_expire_ms = ms;
    }
  }

  ಆದರೆ(ಮುಂದಿನ_expire_ms)
    Curl_expire(admin, ಮುಂದಿನ_expire_ms, EXPIRE_SHUTDOWN);
}

ಸ್ಥಿರ ಶೂನ್ಯ cshutdn_terminate_all(ರಚನೆ cshutdn *cshutdn,
                                  ರಚನೆ Curl_easy *admin,
                                  ಪೂರ್ಣಾಂಕ timeout_ms)
{
  ರಚನೆ curltime started = *Curl_pgrs_now(admin);
  ರಚನೆ Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  ಆಗ(CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    timediff_t spent_ms;
    ಪೂರ್ಣಾಂಕ remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    ಆದರೆ(!CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      ನಿಲ್ಲಿಸು;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    ಆದರೆ(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      ನಿಲ್ಲಿಸು;
    }

    remain_ms = timeout_ms - (ಪೂರ್ಣಾಂಕ)spent_ms;
    ಆದರೆ(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      ನಿಲ್ಲಿಸು;
    }
  }

  /* Terminate any remaining. */
  ಆಗ(CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    ರಚನೆ connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

ಶೂನ್ಯ Curl_cshutdn_init(ರಚನೆ cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

ಶೂನ್ಯ Curl_cshutdn_destroy(ರಚನೆ cshutdn *cshutdn,
                          ರಚನೆ Curl_easy *admin)
{
  ಆದರೆ(admin) {
    ಪೂರ್ಣಾಂಕ timeout_ms = 0;
    /* for testing, run graceful shutdown */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      ಆದರೆ(p) {
        curl_off_t l;
        ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ))
          timeout_ms = (ಪೂರ್ಣಾಂಕ)l;
      }
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_ಎಣಿಕೆ(ರಚನೆ cshutdn *cshutdn)
{
  ಆದರೆ(cshutdn) {
    ಹಿಂತಿರುಗಿ CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns);
  }
  ಹಿಂತಿರುಗಿ 0;
}

size_t Curl_cshutdn_dest_ಎಣಿಕೆ(ರಚನೆ cshutdn *cshutdn,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *destination)
{
  ಆದರೆ(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    ಗಾಗಿ(i = 0; i < CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns); ++i) {
      ರಚನೆ connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      ಆದರೆ(!strcmp(destination, conn->destination))
        ++n;
    }
    ಹಿಂತಿರುಗಿ n;
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ CURLMcode cshutdn_update_ev(ರಚನೆ Curl_multi *multi,
                                   ರಚನೆ connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  ಹಿಂತಿರುಗಿ mresult;
}

ಶೂನ್ಯ Curl_cshutdn_ಕೂಡಿಸು(ರಚನೆ cshutdn *cshutdn,
                      ರಚನೆ Curl_multi *multi,
                      ರಚನೆ connectdata *conn,
                      size_t max_shutdowns)
{
  ರಚನೆ Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  ಆದರೆ(max_shutdowns <= CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, ಖಾಲಿ, 1);
  }

  ಆದರೆ(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ(Curl_ptrarray_ಕೂಡಿಸು(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    ಹಿಂತಿರುಗಿ;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns));
}

ಶೂನ್ಯ Curl_cshutdn_perform(ರಚನೆ cshutdn *cshutdn,
                          ರಚನೆ Curl_easy *admin,
                          ರಚನೆ Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
ಶೂನ್ಯ Curl_cshutdn_setfds(ರಚನೆ cshutdn *cshutdn,
                         ರಚನೆ Curl_easy *admin,
                         fd_set *read_fd_ಗಣ, fd_set *write_fd_ಗಣ,
                         ಪೂರ್ಣಾಂಕ *maxfd)
{
  ಆದರೆ(CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    ರಚನೆ easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    ಗಾಗಿ(i = 0; i < CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns); ++i) {
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ j;
      ರಚನೆ connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode ಫಲಿತಾಂಶ;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      ಫಲಿತಾಂಶ = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಮುಂದುವರಿ;

      ಗಾಗಿ(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        ಆದರೆ(!FDSET_SOCK(sock))
          ಮುಂದುವರಿ;
        ಆದರೆ(ps.actions[j] & CURL_POLL_ಒಳ)
          FD_SET(sock, read_fd_ಗಣ);
        ಆದರೆ(ps.actions[j] & CURL_POLL_ಹೊರಹೋಗು)
          FD_SET(sock, write_fd_ಗಣ);
        ಆದರೆ((ps.actions[j] & (CURL_POLL_ಹೊರಹೋಗು | CURL_POLL_ಒಳ)) &&
           ((ಪೂರ್ಣಾಂಕ)sock > *maxfd))
          *maxfd = (ಪೂರ್ಣಾಂಕ)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ Curl_cshutdn_ಕೂಡಿಸು_waitfds(ರಚನೆ cshutdn *cshutdn,
                                      ರಚನೆ Curl_easy *admin,
                                      ರಚನೆ Curl_waitfds *cwfds)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ need = 0;

  ಆದರೆ(CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    ರಚನೆ easy_pollset ps;
    ರಚನೆ connectdata *conn;
    uint32_t i;
    CURLcode ಫಲಿತಾಂಶ;

    Curl_pollset_init(&ps);
    ಗಾಗಿ(i = 0; i < CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      ಫಲಿತಾಂಶ = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ಆದರೆ(!ಫಲಿತಾಂಶ)
        need += Curl_waitfds_ಕೂಡಿಸು_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  ಹಿಂತಿರುಗಿ need;
}

CURLcode Curl_cshutdn_ಕೂಡಿಸು_pollfds(ರಚನೆ cshutdn *cshutdn,
                                  ರಚನೆ Curl_easy *admin,
                                  ರಚನೆ curl_pollfds *cpfds)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    ರಚನೆ easy_pollset ps;
    ರಚನೆ connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    ಗಾಗಿ(i = 0; i < CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      ಫಲಿತಾಂಶ = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = Curl_pollfds_ಕೂಡಿಸು_ps(cpfds, &ps);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
