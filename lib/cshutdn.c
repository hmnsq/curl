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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "multi_ev.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "cshutdn.h"
#अन्तर्भाव "sigpipe.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "curlx/strparse.h"


#define DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

शून्य Curl_cshutdn_start_timer(संरचना Curl_easy *data, int8_t sockindex,
                              पूर्णांक timeout_ms)
{
  संरचना connectdata *conn = data->conn;
  स्थिरांक संरचना curltime *pnow = Curl_pgrs_now(data);

  DEBUGASSERT(conn);
  conn->shutdown.start_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((data->set.shutdowntimeout > 0) ?
     data->set.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  जर(data->mid)
    Curl_expire_set(data, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(data, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(संरचना Curl_easy *data,
                                    संरचना connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t डावा_ms;

  जर(conn->shutdown.start_ms[sockindex] < 0)
    परत 0; /* not started or no limits */

  डावा_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(data), &conn->created) -
             conn->shutdown.start_ms[sockindex]);
  परत डावा_ms ? डावा_ms : -1;
}

स्थिर timediff_t cshutdn_conn_timeleft(संरचना Curl_easy *data,
                                        संरचना connectdata *conn)
{
  timediff_t डावा_ms = 0, ms;
  int8_t i;

  साठी(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    जर(conn->shutdown.start_ms[i] < 0)
      सुरूठेवा;
    ms = Curl_cshutdn_timeleft_ms(data, conn, i);
    जर(ms && (!डावा_ms || ms < डावा_ms))
      डावा_ms = ms;
  }
  परत डावा_ms;
}

शून्य Curl_cshutdn_clear_timer(संरचना Curl_easy *data, int8_t sockindex)
{
  data->conn->shutdown.start_ms[sockindex] = -1;
}

स्थिर शून्य cshutdn_run_conn_handler(संरचना Curl_easy *data,
                                     संरचना connectdata *conn)
{
  जर(!conn->bits.shutdown_handler) {

    जर(conn->scheme && conn->scheme->run->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      जर(data->state.internal) {
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

CURLcode Curl_cshutdn_try_once_idx(संरचना Curl_easy *data,
                                   int8_t sockindex, bool *done)
{
  संरचना Curl_cfilter *cf;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(data->conn);

  जर(!CONN_SOCK_IDX_VALID(sockindex))
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  /* Get the first connected filter that is not shut down already. */
  cf = data->conn->cfilter[sockindex];
  पर्यंत(cf && (!cf->connected || cf->shutdown))
    cf = cf->next;

  जर(!cf) {
    *done = TRUE;
    परत CURLE_OK;
  }

  *done = FALSE;
  जर(!CURL_CONN_IN_SHUTDOWN(data->conn, sockindex)) {
    Curl_cshutdn_start_timer(data, sockindex, 0);
  }
  अन्यथा {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, data->conn, sockindex);
    जर(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(data, "shutdown timeout");
      परत CURLE_OPERATION_TIMEDOUT;
    }
  }

  पर्यंत(cf) {
    जर(!cf->shutdown) {
      bool cfdone = FALSE;
      result = cf->cft->do_shutdown(cf, data, &cfdone);
      जर(result) {
        CURL_TRC_CF(data, cf, "shut down failed with %d", (पूर्णांक)result);
        परत result;
      }
      अन्यथा जर(!cfdone) {
        CURL_TRC_CF(data, cf, "shut down not done yet");
        परत CURLE_OK;
      }
      CURL_TRC_CF(data, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->next;
  }
  *done = (!result);
  परत result;
}

स्थिर शून्य cshutdn_run_once(संरचना Curl_easy *data,
                             संरचना connectdata *conn,
                             bool *done)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(data->conn == conn);

  जर(!CURL_CONN_IN_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_start_timer(data, FIRSTSOCKET, 0);
  }

  cshutdn_run_conn_handler(data, conn);

  जर(conn->bits.shutdown_filters) {
    *done = TRUE;
    परत;
  }

  जर(!conn->bits.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(data, FIRSTSOCKET, &done1);
  अन्यथा {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  जर(!conn->bits.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(data, SECONDARYSOCKET, &done2);
  अन्यथा {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *done = (r1 || r2 || (done1 && done2));
  जर(*done)
    conn->bits.shutdown_filters = TRUE;
}

शून्य Curl_cshutdn_try_once(संरचना Curl_easy *admin,
                           संरचना connectdata *conn,
                           bool *done)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_run_once(admin, conn, done);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *done);
  Curl_detach_connection(admin);
}

शून्य Curl_cshutdn_terminate(संरचना Curl_easy *admin,
                            संरचना connectdata *conn,
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
  जर(do_shutdown) {
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

  जर(admin->multi)
    Curl_multi_ev_conn_done(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  जर(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

स्थिर माप_प्रकार cshutdn_destroy_oldest(संरचना cshutdn *cshutdn,
                                     संरचना Curl_easy *admin,
                                     स्थिरांक अक्षर *destination,
                                     माप_प्रकार मोजणी)
{
  संरचना Curl_sigpipe_ctx sigpipe_ctx;
  माप_प्रकार removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  पर्यंत((removed < मोजणी) && (i < CURL_PTRARRAY_मोजणी(&cshutdn->conns))) {
    संरचना connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    जर(!destination || !म_भेद(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    अन्यथा
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  परत removed;
}

माप_प्रकार Curl_cshutdn_close_oldest(संरचना cshutdn *cshutdn,
                               संरचना Curl_easy *admin,
                               स्थिरांक अक्षर *destination,
                               माप_प्रकार मोजणी)
{
  जर(cshutdn) {
    परत cshutdn_destroy_oldest(cshutdn, admin, destination, मोजणी);
  }
  परत 0;
}

#define अंक_POLLS_ON_चळत 10

स्थिर CURLcode cshutdn_wait(संरचना cshutdn *cshutdn,
                             संरचना Curl_easy *admin,
                             पूर्णांक timeout_ms)
{
  संरचना pollfd a_few_on_चळत[अंक_POLLS_ON_चळत];
  संरचना curl_pollfds cpfds;
  CURLcode result;

  Curl_pollfds_init(&cpfds, a_few_on_चळत, अंक_POLLS_ON_चळत);

  result = Curl_cshutdn_add_pollfds(cshutdn, admin, &cpfds);
  जर(result)
    जा out;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

out:
  Curl_pollfds_cleanup(&cpfds);
  परत result;
}

स्थिर शून्य cshutdn_perform(संरचना cshutdn *cshutdn,
                            संरचना Curl_easy *admin,
                            संरचना Curl_sigpipe_ctx *sigpipe_ctx)
{
  संरचना connectdata *conn;
  timediff_t next_expire_ms = 0, ms;
  uint32_t i = 0;
  bool done;

  जर(!CURL_PTRARRAY_मोजणी(&cshutdn->conns))
    परत;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_मोजणी(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  पर्यंत(i < CURL_PTRARRAY_मोजणी(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &done);
    जर(done) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    अन्यथा {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      जर(ms && (!next_expire_ms || (ms < next_expire_ms)))
        next_expire_ms = ms;
    }
  }

  जर(next_expire_ms)
    Curl_expire(admin, next_expire_ms, EXPIRE_SHUTDOWN);
}

स्थिर शून्य cshutdn_terminate_all(संरचना cshutdn *cshutdn,
                                  संरचना Curl_easy *admin,
                                  पूर्णांक timeout_ms)
{
  संरचना curltime started = *Curl_pgrs_now(admin);
  संरचना Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  पर्यंत(CURL_PTRARRAY_मोजणी(&cshutdn->conns)) {
    timediff_t spent_ms;
    पूर्णांक remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    जर(!CURL_PTRARRAY_मोजणी(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      थांबा;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    जर(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      थांबा;
    }

    remain_ms = timeout_ms - (पूर्णांक)spent_ms;
    जर(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      थांबा;
    }
  }

  /* Terminate any remaining. */
  पर्यंत(CURL_PTRARRAY_मोजणी(&cshutdn->conns)) {
    संरचना connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_मोजणी(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

शून्य Curl_cshutdn_init(संरचना cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

शून्य Curl_cshutdn_destroy(संरचना cshutdn *cshutdn,
                          संरचना Curl_easy *admin)
{
  जर(admin) {
    पूर्णांक timeout_ms = 0;
    /* for testing, run graceful shutdown */
#ifdef DEBUGBUILD
    {
      स्थिरांक अक्षर *p = दो_पर्या("CURL_GRACEFUL_SHUTDOWN");
      जर(p) {
        curl_off_t l;
        जर(!curlx_str_संख्या(&p, &l, पूर्णांक_उच्च))
          timeout_ms = (पूर्णांक)l;
      }
    }
#endif
    जर(CURL_PTRARRAY_मोजणी(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_मोजणी(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

माप_प्रकार Curl_cshutdn_मोजणी(संरचना cshutdn *cshutdn)
{
  जर(cshutdn) {
    परत CURL_PTRARRAY_मोजणी(&cshutdn->conns);
  }
  परत 0;
}

माप_प्रकार Curl_cshutdn_dest_मोजणी(संरचना cshutdn *cshutdn,
                               स्थिरांक अक्षर *destination)
{
  जर(cshutdn) {
    माप_प्रकार n = 0;
    uint32_t i = 0;
    साठी(i = 0; i < CURL_PTRARRAY_मोजणी(&cshutdn->conns); ++i) {
      संरचना connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      जर(!म_भेद(destination, conn->destination))
        ++n;
    }
    परत n;
  }
  परत 0;
}

स्थिर CURLMcode cshutdn_update_ev(संरचना Curl_multi *multi,
                                   संरचना connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  परत mresult;
}

शून्य Curl_cshutdn_add(संरचना cshutdn *cshutdn,
                      संरचना Curl_multi *multi,
                      संरचना connectdata *conn,
                      माप_प्रकार max_shutdowns)
{
  संरचना Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  जर(max_shutdowns <= CURL_PTRARRAY_मोजणी(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, रिक्त, 1);
  }

  जर(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    परत;
  }

  जर(Curl_ptrarray_add(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    परत;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_मोजणी(&cshutdn->conns));
}

शून्य Curl_cshutdn_perform(संरचना cshutdn *cshutdn,
                          संरचना Curl_easy *admin,
                          संरचना Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
शून्य Curl_cshutdn_setfds(संरचना cshutdn *cshutdn,
                         संरचना Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         पूर्णांक *maxfd)
{
  जर(CURL_PTRARRAY_मोजणी(&cshutdn->conns)) {
    संरचना easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    साठी(i = 0; i < CURL_PTRARRAY_मोजणी(&cshutdn->conns); ++i) {
      अचिन्हित पूर्णांक j;
      संरचना connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode result;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      जर(result)
        सुरूठेवा;

      साठी(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        जर(!FDSET_SOCK(sock))
          सुरूठेवा;
        जर(ps.actions[j] & CURL_POLL_IN)
          FD_SET(sock, read_fd_set);
        जर(ps.actions[j] & CURL_POLL_OUT)
          FD_SET(sock, write_fd_set);
        जर((ps.actions[j] & (CURL_POLL_OUT | CURL_POLL_IN)) &&
           ((पूर्णांक)sock > *maxfd))
          *maxfd = (पूर्णांक)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
अचिन्हित पूर्णांक Curl_cshutdn_add_waitfds(संरचना cshutdn *cshutdn,
                                      संरचना Curl_easy *admin,
                                      संरचना Curl_waitfds *cwfds)
{
  अचिन्हित पूर्णांक need = 0;

  जर(CURL_PTRARRAY_मोजणी(&cshutdn->conns)) {
    संरचना easy_pollset ps;
    संरचना connectdata *conn;
    uint32_t i;
    CURLcode result;

    Curl_pollset_init(&ps);
    साठी(i = 0; i < CURL_PTRARRAY_मोजणी(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      जर(!result)
        need += Curl_waitfds_add_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  परत need;
}

CURLcode Curl_cshutdn_add_pollfds(संरचना cshutdn *cshutdn,
                                  संरचना Curl_easy *admin,
                                  संरचना curl_pollfds *cpfds)
{
  CURLcode result = CURLE_OK;

  जर(CURL_PTRARRAY_मोजणी(&cshutdn->conns)) {
    संरचना easy_pollset ps;
    संरचना connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    साठी(i = 0; i < CURL_PTRARRAY_मोजणी(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      जर(!result)
        result = Curl_pollfds_add_ps(cpfds, &ps);
      जर(result) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        जा out;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
out:
  परत result;
}
