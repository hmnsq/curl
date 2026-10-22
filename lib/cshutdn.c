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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "multi_ev.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "cshutdn.h"
#অন্তর্ভাব "sigpipe.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "curlx/strparse.h"


#define DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

শূন্য Curl_cshutdn_start_timer(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                              পূর্ণসংখ্যা timeout_ms)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  ধ্রুবক কাঠামো curltime *pnow = Curl_pgrs_now(তথ্য);

  DEBUGASSERT(conn);
  conn->shutdown.start_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((তথ্য->set.shutdowntimeout > 0) ?
     তথ্য->set.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  যদি(তথ্য->mid)
    Curl_expire_set(তথ্য, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(তথ্য, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t বাম_ms;

  যদি(conn->shutdown.start_ms[sockindex] < 0)
    ফেরত 0; /* not started or no limits */

  বাম_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(তথ্য), &conn->created) -
             conn->shutdown.start_ms[sockindex]);
  ফেরত বাম_ms ? বাম_ms : -1;
}

স্থির timediff_t cshutdn_conn_timeleft(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো connectdata *conn)
{
  timediff_t বাম_ms = 0, ms;
  int8_t i;

  জন্য(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    যদি(conn->shutdown.start_ms[i] < 0)
      চালু;
    ms = Curl_cshutdn_timeleft_ms(তথ্য, conn, i);
    যদি(ms && (!বাম_ms || ms < বাম_ms))
      বাম_ms = ms;
  }
  ফেরত বাম_ms;
}

শূন্য Curl_cshutdn_clear_timer(কাঠামো Curl_easy *তথ্য, int8_t sockindex)
{
  তথ্য->conn->shutdown.start_ms[sockindex] = -1;
}

স্থির শূন্য cshutdn_run_conn_handler(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো connectdata *conn)
{
  যদি(!conn->bits.shutdown_handler) {

    যদি(conn->scheme && conn->scheme->run->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      যদি(তথ্য->state.internal) {
        তথ্য->set.timeout = DEFAULT_SHUTDOWN_TIMEOUT_MS;
        Curl_pgrsTime(তথ্য, TIMER_STARTOP);
      }

      /* This is set if protocol-specific cleanups should be made */
      DEBUGF(infof(তথ্য, "connection #%" FMT_OFF_T
                   ", shutdown protocol handler (aborted=%d)",
                   conn->connection_id, conn->bits.aborted));
      /* There are protocol handlers that block on retrieving
       * server responses here (FTP). Set a short timeout. */
      conn->scheme->run->disconnect(তথ্য, conn, (bool)conn->bits.aborted);
    }

    conn->bits.shutdown_handler = TRUE;
  }
}

CURLcode Curl_cshutdn_try_once_idx(কাঠামো Curl_easy *তথ্য,
                                   int8_t sockindex, bool *done)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(তথ্য->conn);

  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  /* Get the first connected filter that is not shut down already. */
  cf = তথ্য->conn->cfilter[sockindex];
  যখন(cf && (!cf->connected || cf->shutdown))
    cf = cf->পরবর্তী;

  যদি(!cf) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  *done = FALSE;
  যদি(!CURL_CONN_IN_SHUTDOWN(তথ্য->conn, sockindex)) {
    Curl_cshutdn_start_timer(তথ্য, sockindex, 0);
  }
  অন্যথা {
    timeout_ms = Curl_cshutdn_timeleft_ms(তথ্য, তথ্য->conn, sockindex);
    যদি(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(তথ্য, "shutdown timeout");
      ফেরত CURLE_OPERATION_TIMEDOUT;
    }
  }

  যখন(cf) {
    যদি(!cf->shutdown) {
      bool cfdone = FALSE;
      ফলাফল = cf->cft->do_shutdown(cf, তথ্য, &cfdone);
      যদি(ফলাফল) {
        CURL_TRC_CF(তথ্য, cf, "shut down failed with %d", (পূর্ণসংখ্যা)ফলাফল);
        ফেরত ফলাফল;
      }
      অন্যথা যদি(!cfdone) {
        CURL_TRC_CF(তথ্য, cf, "shut down not done yet");
        ফেরত CURLE_OK;
      }
      CURL_TRC_CF(তথ্য, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->পরবর্তী;
  }
  *done = (!ফলাফল);
  ফেরত ফলাফল;
}

স্থির শূন্য cshutdn_run_once(কাঠামো Curl_easy *তথ্য,
                             কাঠামো connectdata *conn,
                             bool *done)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(তথ্য->conn == conn);

  যদি(!CURL_CONN_IN_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_start_timer(তথ্য, FIRSTSOCKET, 0);
  }

  cshutdn_run_conn_handler(তথ্য, conn);

  যদি(conn->bits.shutdown_filters) {
    *done = TRUE;
    ফেরত;
  }

  যদি(!conn->bits.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(তথ্য, FIRSTSOCKET, &done1);
  অন্যথা {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  যদি(!conn->bits.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(তথ্য, SECONDARYSOCKET, &done2);
  অন্যথা {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *done = (r1 || r2 || (done1 && done2));
  যদি(*done)
    conn->bits.shutdown_filters = TRUE;
}

শূন্য Curl_cshutdn_try_once(কাঠামো Curl_easy *admin,
                           কাঠামো connectdata *conn,
                           bool *done)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_run_once(admin, conn, done);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *done);
  Curl_detach_connection(admin);
}

শূন্য Curl_cshutdn_terminate(কাঠামো Curl_easy *admin,
                            কাঠামো connectdata *conn,
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
  যদি(do_shutdown) {
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

  যদি(admin->multi)
    Curl_multi_ev_conn_done(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  যদি(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

স্থির size_t cshutdn_destroy_oldest(কাঠামো cshutdn *cshutdn,
                                     কাঠামো Curl_easy *admin,
                                     ধ্রুবক অক্ষর *destination,
                                     size_t গুনতি)
{
  কাঠামো Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  যখন((removed < গুনতি) && (i < CURL_PTRARRAY_গুনতি(&cshutdn->conns))) {
    কাঠামো connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    যদি(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    অন্যথা
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  ফেরত removed;
}

size_t Curl_cshutdn_close_oldest(কাঠামো cshutdn *cshutdn,
                               কাঠামো Curl_easy *admin,
                               ধ্রুবক অক্ষর *destination,
                               size_t গুনতি)
{
  যদি(cshutdn) {
    ফেরত cshutdn_destroy_oldest(cshutdn, admin, destination, গুনতি);
  }
  ফেরত 0;
}

#define অঙ্ক_POLLS_ON_স্তূপ 10

স্থির CURLcode cshutdn_wait(কাঠামো cshutdn *cshutdn,
                             কাঠামো Curl_easy *admin,
                             পূর্ণসংখ্যা timeout_ms)
{
  কাঠামো pollfd a_few_on_স্তূপ[অঙ্ক_POLLS_ON_স্তূপ];
  কাঠামো curl_pollfds cpfds;
  CURLcode ফলাফল;

  Curl_pollfds_init(&cpfds, a_few_on_স্তূপ, অঙ্ক_POLLS_ON_স্তূপ);

  ফলাফল = Curl_cshutdn_add_pollfds(cshutdn, admin, &cpfds);
  যদি(ফলাফল)
    যাও out;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

out:
  Curl_pollfds_cleanup(&cpfds);
  ফেরত ফলাফল;
}

স্থির শূন্য cshutdn_perform(কাঠামো cshutdn *cshutdn,
                            কাঠামো Curl_easy *admin,
                            কাঠামো Curl_sigpipe_ctx *sigpipe_ctx)
{
  কাঠামো connectdata *conn;
  timediff_t পরবর্তী_expire_ms = 0, ms;
  uint32_t i = 0;
  bool done;

  যদি(!CURL_PTRARRAY_গুনতি(&cshutdn->conns))
    ফেরত;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_গুনতি(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  যখন(i < CURL_PTRARRAY_গুনতি(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &done);
    যদি(done) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    অন্যথা {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      যদি(ms && (!পরবর্তী_expire_ms || (ms < পরবর্তী_expire_ms)))
        পরবর্তী_expire_ms = ms;
    }
  }

  যদি(পরবর্তী_expire_ms)
    Curl_expire(admin, পরবর্তী_expire_ms, EXPIRE_SHUTDOWN);
}

স্থির শূন্য cshutdn_terminate_all(কাঠামো cshutdn *cshutdn,
                                  কাঠামো Curl_easy *admin,
                                  পূর্ণসংখ্যা timeout_ms)
{
  কাঠামো curltime started = *Curl_pgrs_now(admin);
  কাঠামো Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  যখন(CURL_PTRARRAY_গুনতি(&cshutdn->conns)) {
    timediff_t spent_ms;
    পূর্ণসংখ্যা remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    যদি(!CURL_PTRARRAY_গুনতি(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      বিরতি;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    যদি(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      বিরতি;
    }

    remain_ms = timeout_ms - (পূর্ণসংখ্যা)spent_ms;
    যদি(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      বিরতি;
    }
  }

  /* Terminate any remaining. */
  যখন(CURL_PTRARRAY_গুনতি(&cshutdn->conns)) {
    কাঠামো connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_গুনতি(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

শূন্য Curl_cshutdn_init(কাঠামো cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

শূন্য Curl_cshutdn_destroy(কাঠামো cshutdn *cshutdn,
                          কাঠামো Curl_easy *admin)
{
  যদি(admin) {
    পূর্ণসংখ্যা timeout_ms = 0;
    /* for testing, run graceful shutdown */
#ifdef DEBUGBUILD
    {
      ধ্রুবক অক্ষর *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      যদি(p) {
        curl_off_t l;
        যদি(!curlx_str_সংখ্যা(&p, &l, INT_MAX))
          timeout_ms = (পূর্ণসংখ্যা)l;
      }
    }
#endif
    যদি(CURL_PTRARRAY_গুনতি(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_গুনতি(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_গুনতি(কাঠামো cshutdn *cshutdn)
{
  যদি(cshutdn) {
    ফেরত CURL_PTRARRAY_গুনতি(&cshutdn->conns);
  }
  ফেরত 0;
}

size_t Curl_cshutdn_dest_গুনতি(কাঠামো cshutdn *cshutdn,
                               ধ্রুবক অক্ষর *destination)
{
  যদি(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    জন্য(i = 0; i < CURL_PTRARRAY_গুনতি(&cshutdn->conns); ++i) {
      কাঠামো connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      যদি(!strcmp(destination, conn->destination))
        ++n;
    }
    ফেরত n;
  }
  ফেরত 0;
}

স্থির CURLMcode cshutdn_update_ev(কাঠামো Curl_multi *multi,
                                   কাঠামো connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  ফেরত mresult;
}

শূন্য Curl_cshutdn_add(কাঠামো cshutdn *cshutdn,
                      কাঠামো Curl_multi *multi,
                      কাঠামো connectdata *conn,
                      size_t max_shutdowns)
{
  কাঠামো Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  যদি(max_shutdowns <= CURL_PTRARRAY_গুনতি(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, ফাঁকা, 1);
  }

  যদি(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    ফেরত;
  }

  যদি(Curl_ptrarray_add(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    ফেরত;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_গুনতি(&cshutdn->conns));
}

শূন্য Curl_cshutdn_perform(কাঠামো cshutdn *cshutdn,
                          কাঠামো Curl_easy *admin,
                          কাঠামো Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
শূন্য Curl_cshutdn_setfds(কাঠামো cshutdn *cshutdn,
                         কাঠামো Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         পূর্ণসংখ্যা *maxfd)
{
  যদি(CURL_PTRARRAY_গুনতি(&cshutdn->conns)) {
    কাঠামো easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    জন্য(i = 0; i < CURL_PTRARRAY_গুনতি(&cshutdn->conns); ++i) {
      unsigned পূর্ণসংখ্যা j;
      কাঠামো connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode ফলাফল;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      ফলাফল = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      যদি(ফলাফল)
        চালু;

      জন্য(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        যদি(!FDSET_SOCK(sock))
          চালু;
        যদি(ps.actions[j] & CURL_POLL_IN)
          FD_SET(sock, read_fd_set);
        যদি(ps.actions[j] & CURL_POLL_OUT)
          FD_SET(sock, write_fd_set);
        যদি((ps.actions[j] & (CURL_POLL_OUT | CURL_POLL_IN)) &&
           ((পূর্ণসংখ্যা)sock > *maxfd))
          *maxfd = (পূর্ণসংখ্যা)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
unsigned পূর্ণসংখ্যা Curl_cshutdn_add_waitfds(কাঠামো cshutdn *cshutdn,
                                      কাঠামো Curl_easy *admin,
                                      কাঠামো Curl_waitfds *cwfds)
{
  unsigned পূর্ণসংখ্যা need = 0;

  যদি(CURL_PTRARRAY_গুনতি(&cshutdn->conns)) {
    কাঠামো easy_pollset ps;
    কাঠামো connectdata *conn;
    uint32_t i;
    CURLcode ফলাফল;

    Curl_pollset_init(&ps);
    জন্য(i = 0; i < CURL_PTRARRAY_গুনতি(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      ফলাফল = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      যদি(!ফলাফল)
        need += Curl_waitfds_add_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  ফেরত need;
}

CURLcode Curl_cshutdn_add_pollfds(কাঠামো cshutdn *cshutdn,
                                  কাঠামো Curl_easy *admin,
                                  কাঠামো curl_pollfds *cpfds)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(CURL_PTRARRAY_গুনতি(&cshutdn->conns)) {
    কাঠামো easy_pollset ps;
    কাঠামো connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    জন্য(i = 0; i < CURL_PTRARRAY_গুনতি(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      ফলাফল = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      যদি(!ফলাফল)
        ফলাফল = Curl_pollfds_add_ps(cpfds, &ps);
      যদি(ফলাফল) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        যাও out;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
out:
  ফেরত ফলাফল;
}
