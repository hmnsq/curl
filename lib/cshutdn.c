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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "multi_ev.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "cshutdn.h"
#అన్తర్భావ "sigpipe.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "curlx/strparse.h"


#నిర్వచించు DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

శూన్య Curl_cshutdn_ప్రారంభం_timer(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                              పూర్ణాంకం timeout_ms)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  స్థిరరాశి నిర్మాణం curltime *pnow = Curl_pgrs_now(దత్తాంశం);

  DEBUGASSERT(conn);
  conn->shutdown.ప్రారంభం_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((దత్తాంశం->సమితి.shutdowntimeout > 0) ?
     దత్తాంశం->సమితి.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  ఒకవేళ(దత్తాంశం->mid)
    Curl_expire_సమితి(దత్తాంశం, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(దత్తాంశం, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t ఎడమ_ms;

  ఒకవేళ(conn->shutdown.ప్రారంభం_ms[sockindex] < 0)
    తిరిగి 0; /* not started or no limits */

  ఎడమ_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &conn->created) -
             conn->shutdown.ప్రారంభం_ms[sockindex]);
  తిరిగి ఎడమ_ms ? ఎడమ_ms : -1;
}

స్థిర timediff_t cshutdn_conn_timeleft(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం connectdata *conn)
{
  timediff_t ఎడమ_ms = 0, ms;
  int8_t i;

  కొరకు(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    ఒకవేళ(conn->shutdown.ప్రారంభం_ms[i] < 0)
      కొనసాగు;
    ms = Curl_cshutdn_timeleft_ms(దత్తాంశం, conn, i);
    ఒకవేళ(ms && (!ఎడమ_ms || ms < ఎడమ_ms))
      ఎడమ_ms = ms;
  }
  తిరిగి ఎడమ_ms;
}

శూన్య Curl_cshutdn_clear_timer(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex)
{
  దత్తాంశం->conn->shutdown.ప్రారంభం_ms[sockindex] = -1;
}

స్థిర శూన్య cshutdn_నిరంతరం_conn_handler(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం connectdata *conn)
{
  ఒకవేళ(!conn->బిట్లు.shutdown_handler) {

    ఒకవేళ(conn->scheme && conn->scheme->నిరంతరం->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      ఒకవేళ(దత్తాంశం->state.అంతర్గత) {
        దత్తాంశం->సమితి.timeout = DEFAULT_SHUTDOWN_TIMEOUT_MS;
        Curl_pgrs_సమయం(దత్తాంశం, TIMER_STARTOP);
      }

      /* This is set if protocol-specific cleanups should be made */
      DEBUGF(infof(దత్తాంశం, "connection #%" FMT_OFF_T
                   ", shutdown protocol handler (aborted=%d)",
                   conn->connection_id, conn->బిట్లు.aborted));
      /* There are protocol handlers that block on retrieving
       * server responses here (FTP). Set a short timeout. */
      conn->scheme->నిరంతరం->disconnect(దత్తాంశం, conn, (bool)conn->బిట్లు.aborted);
    }

    conn->బిట్లు.shutdown_handler = TRUE;
  }
}

CURLcode Curl_cshutdn_try_once_idx(నిర్మాణం Curl_easy *దత్తాంశం,
                                   int8_t sockindex, bool *పూర్తి)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(దత్తాంశం->conn);

  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  /* Get the first connected filter that is not shut down already. */
  cf = దత్తాంశం->conn->cfilter[sockindex];
  అయితే(cf && (!cf->connected || cf->shutdown))
    cf = cf->తదుపరి;

  ఒకవేళ(!cf) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  *పూర్తి = FALSE;
  ఒకవేళ(!CURL_CONN_లోపల_SHUTDOWN(దత్తాంశం->conn, sockindex)) {
    Curl_cshutdn_ప్రారంభం_timer(దత్తాంశం, sockindex, 0);
  }
  లేకపోతే {
    timeout_ms = Curl_cshutdn_timeleft_ms(దత్తాంశం, దత్తాంశం->conn, sockindex);
    ఒకవేళ(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(దత్తాంశం, "shutdown timeout");
      తిరిగి CURLE_OPERATION_TIMEDOUT;
    }
  }

  అయితే(cf) {
    ఒకవేళ(!cf->shutdown) {
      bool cfdone = FALSE;
      ఫలితం = cf->cft->do_shutdown(cf, దత్తాంశం, &cfdone);
      ఒకవేళ(ఫలితం) {
        CURL_TRC_CF(దత్తాంశం, cf, "shut down failed with %d", (పూర్ణాంకం)ఫలితం);
        తిరిగి ఫలితం;
      }
      లేకపోతే ఒకవేళ(!cfdone) {
        CURL_TRC_CF(దత్తాంశం, cf, "shut down not done yet");
        తిరిగి CURLE_OK;
      }
      CURL_TRC_CF(దత్తాంశం, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->తదుపరి;
  }
  *పూర్తి = (!ఫలితం);
  తిరిగి ఫలితం;
}

స్థిర శూన్య cshutdn_నిరంతరం_once(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం connectdata *conn,
                             bool *పూర్తి)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(దత్తాంశం->conn == conn);

  ఒకవేళ(!CURL_CONN_లోపల_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_ప్రారంభం_timer(దత్తాంశం, FIRSTSOCKET, 0);
  }

  cshutdn_నిరంతరం_conn_handler(దత్తాంశం, conn);

  ఒకవేళ(conn->బిట్లు.shutdown_filters) {
    *పూర్తి = TRUE;
    తిరిగి;
  }

  ఒకవేళ(!conn->బిట్లు.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(దత్తాంశం, FIRSTSOCKET, &done1);
  లేకపోతే {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  ఒకవేళ(!conn->బిట్లు.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(దత్తాంశం, SECONDARYSOCKET, &done2);
  లేకపోతే {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *పూర్తి = (r1 || r2 || (done1 && done2));
  ఒకవేళ(*పూర్తి)
    conn->బిట్లు.shutdown_filters = TRUE;
}

శూన్య Curl_cshutdn_try_once(నిర్మాణం Curl_easy *admin,
                           నిర్మాణం connectdata *conn,
                           bool *పూర్తి)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_నిరంతరం_once(admin, conn, పూర్తి);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *పూర్తి);
  Curl_detach_connection(admin);
}

శూన్య Curl_cshutdn_terminate(నిర్మాణం Curl_easy *admin,
                            నిర్మాణం connectdata *conn,
                            bool do_shutdown)
{
  bool పూర్తి;

  /* there must be a connection to close */
  DEBUGASSERT(conn);
  /* it must be removed from the connection pool */
  DEBUGASSERT(conn->cpid == UINT32_MAX);
  /* the transfer must be detached from the connection */
  DEBUGASSERT(admin && !admin->conn);
  DEBUGASSERT(!admin->mid);

  Curl_attach_connection(admin, conn, FALSE);

  cshutdn_నిరంతరం_conn_handler(admin, conn);
  ఒకవేళ(do_shutdown) {
    /* Make a last attempt to shutdown handlers and filters, if
     * not done so already. */
    cshutdn_నిరంతరం_once(admin, conn, &పూర్తి);
  }
  CURL_TRC_M(admin, "[SHUTDOWN] %sclosing connection #%" FMT_OFF_T,
             conn->బిట్లు.shutdown_filters ? "" : "force ",
             conn->connection_id);
  Curl_conn_cf_discard_all(admin, conn, SECONDARYSOCKET);
  Curl_conn_cf_discard_all(admin, conn, FIRSTSOCKET);
  Curl_detach_connection(admin);

  ఒకవేళ(admin->multi)
    Curl_multi_ev_conn_పూర్తి(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  ఒకవేళ(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

స్థిర size_t cshutdn_destroy_oldest(నిర్మాణం cshutdn *cshutdn,
                                     నిర్మాణం Curl_easy *admin,
                                     స్థిరరాశి అక్షరం *destination,
                                     size_t లెక్క)
{
  నిర్మాణం Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  అయితే((removed < లెక్క) && (i < CURL_PTRARRAY_లెక్క(&cshutdn->conns))) {
    నిర్మాణం connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    ఒకవేళ(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    లేకపోతే
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  తిరిగి removed;
}

size_t Curl_cshutdn_close_oldest(నిర్మాణం cshutdn *cshutdn,
                               నిర్మాణం Curl_easy *admin,
                               స్థిరరాశి అక్షరం *destination,
                               size_t లెక్క)
{
  ఒకవేళ(cshutdn) {
    తిరిగి cshutdn_destroy_oldest(cshutdn, admin, destination, లెక్క);
  }
  తిరిగి 0;
}

#నిర్వచించు అంకె_POLLS_ON_దొంతర 10

స్థిర CURLcode cshutdn_wait(నిర్మాణం cshutdn *cshutdn,
                             నిర్మాణం Curl_easy *admin,
                             పూర్ణాంకం timeout_ms)
{
  నిర్మాణం pollfd a_few_on_దొంతర[అంకె_POLLS_ON_దొంతర];
  నిర్మాణం curl_pollfds cpfds;
  CURLcode ఫలితం;

  Curl_pollfds_init(&cpfds, a_few_on_దొంతర, అంకె_POLLS_ON_దొంతర);

  ఫలితం = Curl_cshutdn_కూడు_pollfds(cshutdn, admin, &cpfds);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

బహిర్గతం:
  Curl_pollfds_cleanup(&cpfds);
  తిరిగి ఫలితం;
}

స్థిర శూన్య cshutdn_perform(నిర్మాణం cshutdn *cshutdn,
                            నిర్మాణం Curl_easy *admin,
                            నిర్మాణం Curl_sigpipe_ctx *sigpipe_ctx)
{
  నిర్మాణం connectdata *conn;
  timediff_t తదుపరి_expire_ms = 0, ms;
  uint32_t i = 0;
  bool పూర్తి;

  ఒకవేళ(!CURL_PTRARRAY_లెక్క(&cshutdn->conns))
    తిరిగి;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_లెక్క(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  అయితే(i < CURL_PTRARRAY_లెక్క(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &పూర్తి);
    ఒకవేళ(పూర్తి) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    లేకపోతే {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      ఒకవేళ(ms && (!తదుపరి_expire_ms || (ms < తదుపరి_expire_ms)))
        తదుపరి_expire_ms = ms;
    }
  }

  ఒకవేళ(తదుపరి_expire_ms)
    Curl_expire(admin, తదుపరి_expire_ms, EXPIRE_SHUTDOWN);
}

స్థిర శూన్య cshutdn_terminate_all(నిర్మాణం cshutdn *cshutdn,
                                  నిర్మాణం Curl_easy *admin,
                                  పూర్ణాంకం timeout_ms)
{
  నిర్మాణం curltime started = *Curl_pgrs_now(admin);
  నిర్మాణం Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  అయితే(CURL_PTRARRAY_లెక్క(&cshutdn->conns)) {
    timediff_t spent_ms;
    పూర్ణాంకం remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    ఒకవేళ(!CURL_PTRARRAY_లెక్క(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      ఆపు;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    ఒకవేళ(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      ఆపు;
    }

    remain_ms = timeout_ms - (పూర్ణాంకం)spent_ms;
    ఒకవేళ(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      ఆపు;
    }
  }

  /* Terminate any remaining. */
  అయితే(CURL_PTRARRAY_లెక్క(&cshutdn->conns)) {
    నిర్మాణం connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_లెక్క(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

శూన్య Curl_cshutdn_init(నిర్మాణం cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

శూన్య Curl_cshutdn_destroy(నిర్మాణం cshutdn *cshutdn,
                          నిర్మాణం Curl_easy *admin)
{
  ఒకవేళ(admin) {
    పూర్ణాంకం timeout_ms = 0;
    /* for testing, run graceful shutdown */
#నిర్వచితమైతే DEBUGBUILD
    {
      స్థిరరాశి అక్షరం *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      ఒకవేళ(p) {
        curl_off_t l;
        ఒకవేళ(!curlx_str_సంఖ్య(&p, &l, గరిష్ఠ_పూర్ణాంకం))
          timeout_ms = (పూర్ణాంకం)l;
      }
    }
#అయితే_ముగింపు
    ఒకవేళ(CURL_PTRARRAY_లెక్క(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_లెక్క(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_లెక్క(నిర్మాణం cshutdn *cshutdn)
{
  ఒకవేళ(cshutdn) {
    తిరిగి CURL_PTRARRAY_లెక్క(&cshutdn->conns);
  }
  తిరిగి 0;
}

size_t Curl_cshutdn_dest_లెక్క(నిర్మాణం cshutdn *cshutdn,
                               స్థిరరాశి అక్షరం *destination)
{
  ఒకవేళ(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    కొరకు(i = 0; i < CURL_PTRARRAY_లెక్క(&cshutdn->conns); ++i) {
      నిర్మాణం connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      ఒకవేళ(!strcmp(destination, conn->destination))
        ++n;
    }
    తిరిగి n;
  }
  తిరిగి 0;
}

స్థిర CURLMcode cshutdn_update_ev(నిర్మాణం Curl_multi *multi,
                                   నిర్మాణం connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  తిరిగి mresult;
}

శూన్య Curl_cshutdn_కూడు(నిర్మాణం cshutdn *cshutdn,
                      నిర్మాణం Curl_multi *multi,
                      నిర్మాణం connectdata *conn,
                      size_t max_shutdowns)
{
  నిర్మాణం Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  ఒకవేళ(max_shutdowns <= CURL_PTRARRAY_లెక్క(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, ఖాళీ, 1);
  }

  ఒకవేళ(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    తిరిగి;
  }

  ఒకవేళ(Curl_ptrarray_కూడు(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    తిరిగి;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_లెక్క(&cshutdn->conns));
}

శూన్య Curl_cshutdn_perform(నిర్మాణం cshutdn *cshutdn,
                          నిర్మాణం Curl_easy *admin,
                          నిర్మాణం Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
శూన్య Curl_cshutdn_setfds(నిర్మాణం cshutdn *cshutdn,
                         నిర్మాణం Curl_easy *admin,
                         fd_set *read_fd_సమితి, fd_set *write_fd_సమితి,
                         పూర్ణాంకం *maxfd)
{
  ఒకవేళ(CURL_PTRARRAY_లెక్క(&cshutdn->conns)) {
    నిర్మాణం easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    కొరకు(i = 0; i < CURL_PTRARRAY_లెక్క(&cshutdn->conns); ++i) {
      గుర్తులేని పూర్ణాంకం j;
      నిర్మాణం connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode ఫలితం;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      ఫలితం = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ఒకవేళ(ఫలితం)
        కొనసాగు;

      కొరకు(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        ఒకవేళ(!FDSET_SOCK(sock))
          కొనసాగు;
        ఒకవేళ(ps.actions[j] & CURL_POLL_లోపల)
          FD_SET(sock, read_fd_సమితి);
        ఒకవేళ(ps.actions[j] & CURL_POLL_బహిర్గతం)
          FD_SET(sock, write_fd_సమితి);
        ఒకవేళ((ps.actions[j] & (CURL_POLL_బహిర్గతం | CURL_POLL_లోపల)) &&
           ((పూర్ణాంకం)sock > *maxfd))
          *maxfd = (పూర్ణాంకం)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
గుర్తులేని పూర్ణాంకం Curl_cshutdn_కూడు_waitfds(నిర్మాణం cshutdn *cshutdn,
                                      నిర్మాణం Curl_easy *admin,
                                      నిర్మాణం Curl_waitfds *cwfds)
{
  గుర్తులేని పూర్ణాంకం need = 0;

  ఒకవేళ(CURL_PTRARRAY_లెక్క(&cshutdn->conns)) {
    నిర్మాణం easy_pollset ps;
    నిర్మాణం connectdata *conn;
    uint32_t i;
    CURLcode ఫలితం;

    Curl_pollset_init(&ps);
    కొరకు(i = 0; i < CURL_PTRARRAY_లెక్క(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      ఫలితం = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ఒకవేళ(!ఫలితం)
        need += Curl_waitfds_కూడు_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  తిరిగి need;
}

CURLcode Curl_cshutdn_కూడు_pollfds(నిర్మాణం cshutdn *cshutdn,
                                  నిర్మాణం Curl_easy *admin,
                                  నిర్మాణం curl_pollfds *cpfds)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(CURL_PTRARRAY_లెక్క(&cshutdn->conns)) {
    నిర్మాణం easy_pollset ps;
    నిర్మాణం connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    కొరకు(i = 0; i < CURL_PTRARRAY_లెక్క(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      ఫలితం = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ఒకవేళ(!ఫలితం)
        ఫలితం = Curl_pollfds_కూడు_ps(cpfds, &ps);
      ఒకవేళ(ఫలితం) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        వెళ్ళు బహిర్గతం;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
బహిర్గతం:
  తిరిగి ఫలితం;
}
