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


#define DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

ಶೂನ್ಯ Curl_cshutdn_start_timer(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex,
                              ಪೂರ್ಣಾಙ್ಕ timeout_ms)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow = Curl_pgrs_now(data);

  DEBUGASSERT(conn);
  conn->shutdown.start_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((data->set.shutdowntimeout > 0) ?
     data->set.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  ಯದಿ(data->mid)
    Curl_expire_set(data, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(data, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t ಎಡ_ms;

  ಯದಿ(conn->shutdown.start_ms[sockindex] < 0)
    ಪ್ರತ್ಯಾಗಮ 0; /* not started or no limits */

  ಎಡ_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(data), &conn->created) -
             conn->shutdown.start_ms[sockindex]);
  ಪ್ರತ್ಯಾಗಮ ಎಡ_ms ? ಎಡ_ms : -1;
}

ಸ್ಥಿರ timediff_t cshutdn_conn_timeleft(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ connectdata *conn)
{
  timediff_t ಎಡ_ms = 0, ms;
  int8_t i;

  ಪ್ರತಿ(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    ಯದಿ(conn->shutdown.start_ms[i] < 0)
      ಅನುವರ್ತ;
    ms = Curl_cshutdn_timeleft_ms(data, conn, i);
    ಯದಿ(ms && (!ಎಡ_ms || ms < ಎಡ_ms))
      ಎಡ_ms = ms;
  }
  ಪ್ರತ್ಯಾಗಮ ಎಡ_ms;
}

ಶೂನ್ಯ Curl_cshutdn_clear_timer(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex)
{
  data->conn->shutdown.start_ms[sockindex] = -1;
}

ಸ್ಥಿರ ಶೂನ್ಯ cshutdn_run_conn_handler(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ connectdata *conn)
{
  ಯದಿ(!conn->bits.shutdown_handler) {

    ಯದಿ(conn->scheme && conn->scheme->run->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      ಯದಿ(data->state.internal) {
        data->set.timeout = DEFAULT_SHUTDOWN_TIMEOUT_MS;
        Curl_pgrsTime(data, TIMER_STARTOP);
      }

      /* This is set if protocol-specific cleanups should be made */
      DEBUGF(infof(data, "connection #%" FMT_OFF_T
                   ", shutdown protocol handler (aborted=%d)",
                   conn->connection_id, conn->bits.aborted));
      /* There are protocol handlers that block on retrieving
       * server responses here (FTP). Set a short timeout. */
      conn->scheme->run->disconnect(data, conn, (bool)conn->bits.aborted);
    }

    conn->bits.shutdown_handler = TRUE;
  }
}

CURLcode Curl_cshutdn_try_once_idx(ಸಂರಚನಾ Curl_easy *data,
                                   int8_t sockindex, bool *done)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(data->conn);

  ಯದಿ(!CONN_SOCK_IDX_VALID(sockindex))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Get the first connected filter that is not shut down already. */
  cf = data->conn->cfilter[sockindex];
  ಯಾವತ್(cf && (!cf->connected || cf->shutdown))
    cf = cf->next;

  ಯದಿ(!cf) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  *done = FALSE;
  ಯದಿ(!CURL_CONN_IN_SHUTDOWN(data->conn, sockindex)) {
    Curl_cshutdn_start_timer(data, sockindex, 0);
  }
  ಅನ್ಯಥಾ {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, data->conn, sockindex);
    ಯದಿ(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(data, "shutdown timeout");
      ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
    }
  }

  ಯಾವತ್(cf) {
    ಯದಿ(!cf->shutdown) {
      bool cfdone = FALSE;
      result = cf->cft->do_shutdown(cf, data, &cfdone);
      ಯದಿ(result) {
        CURL_TRC_CF(data, cf, "shut down failed with %d", (ಪೂರ್ಣಾಙ್ಕ)result);
        ಪ್ರತ್ಯಾಗಮ result;
      }
      ಅನ್ಯಥಾ ಯದಿ(!cfdone) {
        CURL_TRC_CF(data, cf, "shut down not done yet");
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      CURL_TRC_CF(data, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->next;
  }
  *done = (!result);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cshutdn_run_once(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ connectdata *conn,
                             bool *done)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(data->conn == conn);

  ಯದಿ(!CURL_CONN_IN_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_start_timer(data, FIRSTSOCKET, 0);
  }

  cshutdn_run_conn_handler(data, conn);

  ಯದಿ(conn->bits.shutdown_filters) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ;
  }

  ಯದಿ(!conn->bits.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(data, FIRSTSOCKET, &done1);
  ಅನ್ಯಥಾ {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  ಯದಿ(!conn->bits.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(data, SECONDARYSOCKET, &done2);
  ಅನ್ಯಥಾ {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *done = (r1 || r2 || (done1 && done2));
  ಯದಿ(*done)
    conn->bits.shutdown_filters = TRUE;
}

ಶೂನ್ಯ Curl_cshutdn_try_once(ಸಂರಚನಾ Curl_easy *admin,
                           ಸಂರಚನಾ connectdata *conn,
                           bool *done)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_run_once(admin, conn, done);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *done);
  Curl_detach_connection(admin);
}

ಶೂನ್ಯ Curl_cshutdn_terminate(ಸಂರಚನಾ Curl_easy *admin,
                            ಸಂರಚನಾ connectdata *conn,
                            bool do_shutdown)
{
  bool done;

  /* there must be a connection to close */
  DEBUGASSERT(conn);
  /* it must be removed from the connection pool */
  DEBUGASSERT(conn->cpid == UINT32_MAX);
  /* the transfer must be detached from the connection */
  DEBUGASSERT(admin && !admin->conn);
  DEBUGASSERT(!admin->mid);

  Curl_attach_connection(admin, conn, FALSE);

  cshutdn_run_conn_handler(admin, conn);
  ಯದಿ(do_shutdown) {
    /* Make a last attempt to shutdown handlers and filters, if
     * not done so already. */
    cshutdn_run_once(admin, conn, &done);
  }
  CURL_TRC_M(admin, "[SHUTDOWN] %sclosing connection #%" FMT_OFF_T,
             conn->bits.shutdown_filters ? "" : "force ",
             conn->connection_id);
  Curl_conn_cf_discard_all(admin, conn, SECONDARYSOCKET);
  Curl_conn_cf_discard_all(admin, conn, FIRSTSOCKET);
  Curl_detach_connection(admin);

  ಯದಿ(admin->multi)
    Curl_multi_ev_conn_done(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  ಯದಿ(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

ಸ್ಥಿರ size_t cshutdn_destroy_oldest(ಸಂರಚನಾ cshutdn *cshutdn,
                                     ಸಂರಚನಾ Curl_easy *admin,
                                     ಧ್ರುವ ವರ್ಣ *destination,
                                     size_t ಎಣಿಕೆ)
{
  ಸಂರಚನಾ Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  ಯಾವತ್((removed < ಎಣಿಕೆ) && (i < CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns))) {
    ಸಂರಚನಾ connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    ಯದಿ(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    ಅನ್ಯಥಾ
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  ಪ್ರತ್ಯಾಗಮ removed;
}

size_t Curl_cshutdn_close_oldest(ಸಂರಚನಾ cshutdn *cshutdn,
                               ಸಂರಚನಾ Curl_easy *admin,
                               ಧ್ರುವ ವರ್ಣ *destination,
                               size_t ಎಣಿಕೆ)
{
  ಯದಿ(cshutdn) {
    ಪ್ರತ್ಯಾಗಮ cshutdn_destroy_oldest(cshutdn, admin, destination, ಎಣಿಕೆ);
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

#define ಅಂಕೆ_POLLS_ON_ರಾಶಿ 10

ಸ್ಥಿರ CURLcode cshutdn_wait(ಸಂರಚನಾ cshutdn *cshutdn,
                             ಸಂರಚನಾ Curl_easy *admin,
                             ಪೂರ್ಣಾಙ್ಕ timeout_ms)
{
  ಸಂರಚನಾ pollfd a_few_on_ರಾಶಿ[ಅಂಕೆ_POLLS_ON_ರಾಶಿ];
  ಸಂರಚನಾ curl_pollfds cpfds;
  CURLcode result;

  Curl_pollfds_init(&cpfds, a_few_on_ರಾಶಿ, ಅಂಕೆ_POLLS_ON_ರಾಶಿ);

  result = Curl_cshutdn_add_pollfds(cshutdn, admin, &cpfds);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

out:
  Curl_pollfds_cleanup(&cpfds);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cshutdn_perform(ಸಂರಚನಾ cshutdn *cshutdn,
                            ಸಂರಚನಾ Curl_easy *admin,
                            ಸಂರಚನಾ Curl_sigpipe_ctx *sigpipe_ctx)
{
  ಸಂರಚನಾ connectdata *conn;
  timediff_t next_expire_ms = 0, ms;
  uint32_t i = 0;
  bool done;

  ಯದಿ(!CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns))
    ಪ್ರತ್ಯಾಗಮ;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  ಯಾವತ್(i < CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &done);
    ಯದಿ(done) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    ಅನ್ಯಥಾ {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      ಯದಿ(ms && (!next_expire_ms || (ms < next_expire_ms)))
        next_expire_ms = ms;
    }
  }

  ಯದಿ(next_expire_ms)
    Curl_expire(admin, next_expire_ms, EXPIRE_SHUTDOWN);
}

ಸ್ಥಿರ ಶೂನ್ಯ cshutdn_terminate_all(ಸಂರಚನಾ cshutdn *cshutdn,
                                  ಸಂರಚನಾ Curl_easy *admin,
                                  ಪೂರ್ಣಾಙ್ಕ timeout_ms)
{
  ಸಂರಚನಾ curltime started = *Curl_pgrs_now(admin);
  ಸಂರಚನಾ Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  ಯಾವತ್(CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    timediff_t spent_ms;
    ಪೂರ್ಣಾಙ್ಕ remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    ಯದಿ(!CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      ಭಙ್ಗ;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    ಯದಿ(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      ಭಙ್ಗ;
    }

    remain_ms = timeout_ms - (ಪೂರ್ಣಾಙ್ಕ)spent_ms;
    ಯದಿ(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      ಭಙ್ಗ;
    }
  }

  /* Terminate any remaining. */
  ಯಾವತ್(CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    ಸಂರಚನಾ connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

ಶೂನ್ಯ Curl_cshutdn_init(ಸಂರಚನಾ cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

ಶೂನ್ಯ Curl_cshutdn_destroy(ಸಂರಚನಾ cshutdn *cshutdn,
                          ಸಂರಚನಾ Curl_easy *admin)
{
  ಯದಿ(admin) {
    ಪೂರ್ಣಾಙ್ಕ timeout_ms = 0;
    /* for testing, run graceful shutdown */
#ifdef DEBUGBUILD
    {
      ಧ್ರುವ ವರ್ಣ *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      ಯದಿ(p) {
        curl_off_t l;
        ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &l, INT_MAX))
          timeout_ms = (ಪೂರ್ಣಾಙ್ಕ)l;
      }
    }
#endif
    ಯದಿ(CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_ಎಣಿಕೆ(ಸಂರಚನಾ cshutdn *cshutdn)
{
  ಯದಿ(cshutdn) {
    ಪ್ರತ್ಯಾಗಮ CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns);
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

size_t Curl_cshutdn_dest_ಎಣಿಕೆ(ಸಂರಚನಾ cshutdn *cshutdn,
                               ಧ್ರುವ ವರ್ಣ *destination)
{
  ಯದಿ(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    ಪ್ರತಿ(i = 0; i < CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns); ++i) {
      ಸಂರಚನಾ connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      ಯದಿ(!strcmp(destination, conn->destination))
        ++n;
    }
    ಪ್ರತ್ಯಾಗಮ n;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ CURLMcode cshutdn_update_ev(ಸಂರಚನಾ Curl_multi *multi,
                                   ಸಂರಚನಾ connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  ಪ್ರತ್ಯಾಗಮ mresult;
}

ಶೂನ್ಯ Curl_cshutdn_add(ಸಂರಚನಾ cshutdn *cshutdn,
                      ಸಂರಚನಾ Curl_multi *multi,
                      ಸಂರಚನಾ connectdata *conn,
                      size_t max_shutdowns)
{
  ಸಂರಚನಾ Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  ಯದಿ(max_shutdowns <= CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, ರಿಕ್ತ, 1);
  }

  ಯದಿ(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    ಪ್ರತ್ಯಾಗಮ;
  }

  ಯದಿ(Curl_ptrarray_add(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    ಪ್ರತ್ಯಾಗಮ;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns));
}

ಶೂನ್ಯ Curl_cshutdn_perform(ಸಂರಚನಾ cshutdn *cshutdn,
                          ಸಂರಚನಾ Curl_easy *admin,
                          ಸಂರಚನಾ Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
ಶೂನ್ಯ Curl_cshutdn_setfds(ಸಂರಚನಾ cshutdn *cshutdn,
                         ಸಂರಚನಾ Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         ಪೂರ್ಣಾಙ್ಕ *maxfd)
{
  ಯದಿ(CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    ಸಂರಚನಾ easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    ಪ್ರತಿ(i = 0; i < CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns); ++i) {
      unsigned ಪೂರ್ಣಾಙ್ಕ j;
      ಸಂರಚನಾ connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode result;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ಯದಿ(result)
        ಅನುವರ್ತ;

      ಪ್ರತಿ(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        ಯದಿ(!FDSET_SOCK(sock))
          ಅನುವರ್ತ;
        ಯದಿ(ps.actions[j] & CURL_POLL_IN)
          FD_SET(sock, read_fd_set);
        ಯದಿ(ps.actions[j] & CURL_POLL_OUT)
          FD_SET(sock, write_fd_set);
        ಯದಿ((ps.actions[j] & (CURL_POLL_OUT | CURL_POLL_IN)) &&
           ((ಪೂರ್ಣಾಙ್ಕ)sock > *maxfd))
          *maxfd = (ಪೂರ್ಣಾಙ್ಕ)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
unsigned ಪೂರ್ಣಾಙ್ಕ Curl_cshutdn_add_waitfds(ಸಂರಚನಾ cshutdn *cshutdn,
                                      ಸಂರಚನಾ Curl_easy *admin,
                                      ಸಂರಚನಾ Curl_waitfds *cwfds)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ need = 0;

  ಯದಿ(CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    ಸಂರಚನಾ easy_pollset ps;
    ಸಂರಚನಾ connectdata *conn;
    uint32_t i;
    CURLcode result;

    Curl_pollset_init(&ps);
    ಪ್ರತಿ(i = 0; i < CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ಯದಿ(!result)
        need += Curl_waitfds_add_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  ಪ್ರತ್ಯಾಗಮ need;
}

CURLcode Curl_cshutdn_add_pollfds(ಸಂರಚನಾ cshutdn *cshutdn,
                                  ಸಂರಚನಾ Curl_easy *admin,
                                  ಸಂರಚನಾ curl_pollfds *cpfds)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns)) {
    ಸಂರಚನಾ easy_pollset ps;
    ಸಂರಚನಾ connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    ಪ್ರತಿ(i = 0; i < CURL_PTRARRAY_ಎಣಿಕೆ(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ಯದಿ(!result)
        result = Curl_pollfds_add_ps(cpfds, &ps);
      ಯದಿ(result) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        ಗಚ್ಛ out;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
out:
  ಪ್ರತ್ಯಾಗಮ result;
}
