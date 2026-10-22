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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "url.h"
#समावेश "cfilters.h"
#समावेश "progress.h"
#समावेश "multiif.h"
#समावेश "multi_ev.h"
#समावेश "curl_trc.h"
#समावेश "cshutdn.h"
#समावेश "sigpipe.h"
#समावेश "connect.h"
#समावेश "select.h"
#समावेश "curlx/strparse.h"


#घोषणा DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

व्योम Curl_cshutdn_start_timer(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                              पूर्णांक timeout_ms)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  स्थिर काष्ठा curltime *pnow = Curl_pgrs_now(तथ्य);

  DEBUGASSERT(conn);
  conn->shutdown.start_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((तथ्य->set.shutdowntimeout > 0) ?
     तथ्य->set.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  अगर(तथ्य->mid)
    Curl_expire_set(तथ्य, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(तथ्य, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t बायाँ_ms;

  अगर(conn->shutdown.start_ms[sockindex] < 0)
    वापस 0; /* not started or no limits */

  बायाँ_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &conn->created) -
             conn->shutdown.start_ms[sockindex]);
  वापस बायाँ_ms ? बायाँ_ms : -1;
}

अटल timediff_t cshutdn_conn_timeleft(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा connectdata *conn)
{
  timediff_t बायाँ_ms = 0, ms;
  int8_t i;

  क्रम(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    अगर(conn->shutdown.start_ms[i] < 0)
      जारी;
    ms = Curl_cshutdn_timeleft_ms(तथ्य, conn, i);
    अगर(ms && (!बायाँ_ms || ms < बायाँ_ms))
      बायाँ_ms = ms;
  }
  वापस बायाँ_ms;
}

व्योम Curl_cshutdn_clear_timer(काष्ठा Curl_easy *तथ्य, int8_t sockindex)
{
  तथ्य->conn->shutdown.start_ms[sockindex] = -1;
}

अटल व्योम cshutdn_run_conn_handler(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा connectdata *conn)
{
  अगर(!conn->bits.shutdown_handler) {

    अगर(conn->scheme && conn->scheme->run->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      अगर(तथ्य->state.internal) {
        तथ्य->set.timeout = DEFAULT_SHUTDOWN_TIMEOUT_MS;
        Curl_pgrsTime(तथ्य, TIMER_STARTOP);
      }

      /* This is set if protocol-specific cleanups should be made */
      DEBUGF(infof(तथ्य, "connection #%" FMT_OFF_T
                   ", shutdown protocol handler (aborted=%d)",
                   conn->connection_id, conn->bits.aborted));
      /* There are protocol handlers that block on retrieving
       * server responses here (FTP). Set a short timeout. */
      conn->scheme->run->disconnect(तथ्य, conn, (bool)conn->bits.aborted);
    }

    conn->bits.shutdown_handler = TRUE;
  }
}

CURLcode Curl_cshutdn_try_once_idx(काष्ठा Curl_easy *तथ्य,
                                   int8_t sockindex, bool *done)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(तथ्य->conn);

  अगर(!CONN_SOCK_IDX_VALID(sockindex))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  /* Get the first connected filter that is not shut down already. */
  cf = तथ्य->conn->cfilter[sockindex];
  जबतक(cf && (!cf->connected || cf->shutdown))
    cf = cf->अग्र;

  अगर(!cf) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  *done = FALSE;
  अगर(!CURL_CONN_IN_SHUTDOWN(तथ्य->conn, sockindex)) {
    Curl_cshutdn_start_timer(तथ्य, sockindex, 0);
  }
  अन्यथा {
    timeout_ms = Curl_cshutdn_timeleft_ms(तथ्य, तथ्य->conn, sockindex);
    अगर(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(तथ्य, "shutdown timeout");
      वापस CURLE_OPERATION_TIMEDOUT;
    }
  }

  जबतक(cf) {
    अगर(!cf->shutdown) {
      bool cfdone = FALSE;
      परिणाम = cf->cft->do_shutdown(cf, तथ्य, &cfdone);
      अगर(परिणाम) {
        CURL_TRC_CF(तथ्य, cf, "shut down failed with %d", (पूर्णांक)परिणाम);
        वापस परिणाम;
      }
      अन्यथा अगर(!cfdone) {
        CURL_TRC_CF(तथ्य, cf, "shut down not done yet");
        वापस CURLE_OK;
      }
      CURL_TRC_CF(तथ्य, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->अग्र;
  }
  *done = (!परिणाम);
  वापस परिणाम;
}

अटल व्योम cshutdn_run_once(काष्ठा Curl_easy *तथ्य,
                             काष्ठा connectdata *conn,
                             bool *done)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(तथ्य->conn == conn);

  अगर(!CURL_CONN_IN_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_start_timer(तथ्य, FIRSTSOCKET, 0);
  }

  cshutdn_run_conn_handler(तथ्य, conn);

  अगर(conn->bits.shutdown_filters) {
    *done = TRUE;
    वापस;
  }

  अगर(!conn->bits.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(तथ्य, FIRSTSOCKET, &done1);
  अन्यथा {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  अगर(!conn->bits.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(तथ्य, SECONDARYSOCKET, &done2);
  अन्यथा {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *done = (r1 || r2 || (done1 && done2));
  अगर(*done)
    conn->bits.shutdown_filters = TRUE;
}

व्योम Curl_cshutdn_try_once(काष्ठा Curl_easy *admin,
                           काष्ठा connectdata *conn,
                           bool *done)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_run_once(admin, conn, done);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *done);
  Curl_detach_connection(admin);
}

व्योम Curl_cshutdn_terminate(काष्ठा Curl_easy *admin,
                            काष्ठा connectdata *conn,
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
  अगर(do_shutdown) {
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

  अगर(admin->multi)
    Curl_multi_ev_conn_done(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  अगर(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

अटल माप_प्रकार cshutdn_destroy_oldest(काष्ठा cshutdn *cshutdn,
                                     काष्ठा Curl_easy *admin,
                                     स्थिर अक्षर *destination,
                                     माप_प्रकार गिनती)
{
  काष्ठा Curl_sigpipe_ctx sigpipe_ctx;
  माप_प्रकार removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  जबतक((removed < गिनती) && (i < CURL_PTRARRAY_गिनती(&cshutdn->conns))) {
    काष्ठा connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    अगर(!destination || !म_भेद(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    अन्यथा
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  वापस removed;
}

माप_प्रकार Curl_cshutdn_close_oldest(काष्ठा cshutdn *cshutdn,
                               काष्ठा Curl_easy *admin,
                               स्थिर अक्षर *destination,
                               माप_प्रकार गिनती)
{
  अगर(cshutdn) {
    वापस cshutdn_destroy_oldest(cshutdn, admin, destination, गिनती);
  }
  वापस 0;
}

#घोषणा अंक_POLLS_ON_ढेर 10

अटल CURLcode cshutdn_wait(काष्ठा cshutdn *cshutdn,
                             काष्ठा Curl_easy *admin,
                             पूर्णांक timeout_ms)
{
  काष्ठा pollfd a_few_on_ढेर[अंक_POLLS_ON_ढेर];
  काष्ठा curl_pollfds cpfds;
  CURLcode परिणाम;

  Curl_pollfds_init(&cpfds, a_few_on_ढेर, अंक_POLLS_ON_ढेर);

  परिणाम = Curl_cshutdn_add_pollfds(cshutdn, admin, &cpfds);
  अगर(परिणाम)
    जाओ out;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

out:
  Curl_pollfds_cleanup(&cpfds);
  वापस परिणाम;
}

अटल व्योम cshutdn_perform(काष्ठा cshutdn *cshutdn,
                            काष्ठा Curl_easy *admin,
                            काष्ठा Curl_sigpipe_ctx *sigpipe_ctx)
{
  काष्ठा connectdata *conn;
  timediff_t अग्र_expire_ms = 0, ms;
  uint32_t i = 0;
  bool done;

  अगर(!CURL_PTRARRAY_गिनती(&cshutdn->conns))
    वापस;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_गिनती(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  जबतक(i < CURL_PTRARRAY_गिनती(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &done);
    अगर(done) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    अन्यथा {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      अगर(ms && (!अग्र_expire_ms || (ms < अग्र_expire_ms)))
        अग्र_expire_ms = ms;
    }
  }

  अगर(अग्र_expire_ms)
    Curl_expire(admin, अग्र_expire_ms, EXPIRE_SHUTDOWN);
}

अटल व्योम cshutdn_terminate_all(काष्ठा cshutdn *cshutdn,
                                  काष्ठा Curl_easy *admin,
                                  पूर्णांक timeout_ms)
{
  काष्ठा curltime started = *Curl_pgrs_now(admin);
  काष्ठा Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  जबतक(CURL_PTRARRAY_गिनती(&cshutdn->conns)) {
    timediff_t spent_ms;
    पूर्णांक remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    अगर(!CURL_PTRARRAY_गिनती(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      अवरोध;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    अगर(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      अवरोध;
    }

    remain_ms = timeout_ms - (पूर्णांक)spent_ms;
    अगर(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      अवरोध;
    }
  }

  /* Terminate any remaining. */
  जबतक(CURL_PTRARRAY_गिनती(&cshutdn->conns)) {
    काष्ठा connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_गिनती(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

व्योम Curl_cshutdn_init(काष्ठा cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

व्योम Curl_cshutdn_destroy(काष्ठा cshutdn *cshutdn,
                          काष्ठा Curl_easy *admin)
{
  अगर(admin) {
    पूर्णांक timeout_ms = 0;
    /* for testing, run graceful shutdown */
#अगर_घोषित DEBUGBUILD
    {
      स्थिर अक्षर *p = दो_पर्या("CURL_GRACEFUL_SHUTDOWN");
      अगर(p) {
        curl_off_t l;
        अगर(!curlx_str_संख्या(&p, &l, पूर्णांक_उच्च))
          timeout_ms = (पूर्णांक)l;
      }
    }
#पूर्ण_अगर
    अगर(CURL_PTRARRAY_गिनती(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_गिनती(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

माप_प्रकार Curl_cshutdn_गिनती(काष्ठा cshutdn *cshutdn)
{
  अगर(cshutdn) {
    वापस CURL_PTRARRAY_गिनती(&cshutdn->conns);
  }
  वापस 0;
}

माप_प्रकार Curl_cshutdn_dest_गिनती(काष्ठा cshutdn *cshutdn,
                               स्थिर अक्षर *destination)
{
  अगर(cshutdn) {
    माप_प्रकार n = 0;
    uint32_t i = 0;
    क्रम(i = 0; i < CURL_PTRARRAY_गिनती(&cshutdn->conns); ++i) {
      काष्ठा connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      अगर(!म_भेद(destination, conn->destination))
        ++n;
    }
    वापस n;
  }
  वापस 0;
}

अटल CURLMcode cshutdn_update_ev(काष्ठा Curl_multi *multi,
                                   काष्ठा connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  वापस mresult;
}

व्योम Curl_cshutdn_add(काष्ठा cshutdn *cshutdn,
                      काष्ठा Curl_multi *multi,
                      काष्ठा connectdata *conn,
                      माप_प्रकार max_shutdowns)
{
  काष्ठा Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  अगर(max_shutdowns <= CURL_PTRARRAY_गिनती(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, शून्य, 1);
  }

  अगर(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    वापस;
  }

  अगर(Curl_ptrarray_add(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    वापस;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_गिनती(&cshutdn->conns));
}

व्योम Curl_cshutdn_perform(काष्ठा cshutdn *cshutdn,
                          काष्ठा Curl_easy *admin,
                          काष्ठा Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
व्योम Curl_cshutdn_setfds(काष्ठा cshutdn *cshutdn,
                         काष्ठा Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         पूर्णांक *maxfd)
{
  अगर(CURL_PTRARRAY_गिनती(&cshutdn->conns)) {
    काष्ठा easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    क्रम(i = 0; i < CURL_PTRARRAY_गिनती(&cshutdn->conns); ++i) {
      अचिन्हित पूर्णांक j;
      काष्ठा connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode परिणाम;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      परिणाम = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      अगर(परिणाम)
        जारी;

      क्रम(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        अगर(!FDSET_SOCK(sock))
          जारी;
        अगर(ps.actions[j] & CURL_POLL_IN)
          FD_SET(sock, read_fd_set);
        अगर(ps.actions[j] & CURL_POLL_OUT)
          FD_SET(sock, write_fd_set);
        अगर((ps.actions[j] & (CURL_POLL_OUT | CURL_POLL_IN)) &&
           ((पूर्णांक)sock > *maxfd))
          *maxfd = (पूर्णांक)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
अचिन्हित पूर्णांक Curl_cshutdn_add_waitfds(काष्ठा cshutdn *cshutdn,
                                      काष्ठा Curl_easy *admin,
                                      काष्ठा Curl_waitfds *cwfds)
{
  अचिन्हित पूर्णांक need = 0;

  अगर(CURL_PTRARRAY_गिनती(&cshutdn->conns)) {
    काष्ठा easy_pollset ps;
    काष्ठा connectdata *conn;
    uint32_t i;
    CURLcode परिणाम;

    Curl_pollset_init(&ps);
    क्रम(i = 0; i < CURL_PTRARRAY_गिनती(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      परिणाम = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      अगर(!परिणाम)
        need += Curl_waitfds_add_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  वापस need;
}

CURLcode Curl_cshutdn_add_pollfds(काष्ठा cshutdn *cshutdn,
                                  काष्ठा Curl_easy *admin,
                                  काष्ठा curl_pollfds *cpfds)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(CURL_PTRARRAY_गिनती(&cshutdn->conns)) {
    काष्ठा easy_pollset ps;
    काष्ठा connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    क्रम(i = 0; i < CURL_PTRARRAY_गिनती(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      परिणाम = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      अगर(!परिणाम)
        परिणाम = Curl_pollfds_add_ps(cpfds, &ps);
      अगर(परिणाम) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        जाओ out;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
out:
  वापस परिणाम;
}
