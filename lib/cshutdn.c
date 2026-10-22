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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "multi_ev.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "cshutdn.h"
#ଅନ୍ତର୍ଭାଵ "sigpipe.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"


#define DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

ଶୂନ୍ୟ Curl_cshutdn_start_timer(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                              ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout_ms)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ଅଚଳ ସଂରଚନା curltime *pnow = Curl_pgrs_now(data);

  DEBUGASSERT(conn);
  conn->shutdown.start_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((data->set.shutdowntimeout > 0) ?
     data->set.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  ଯଦି(data->mid)
    Curl_expire_set(data, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(data, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t ବାମ_ms;

  ଯଦି(conn->shutdown.start_ms[sockindex] < 0)
    ଫେରନ୍ତୁ 0; /* not started or no limits */

  ବାମ_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(data), &conn->created) -
             conn->shutdown.start_ms[sockindex]);
  ଫେରନ୍ତୁ ବାମ_ms ? ବାମ_ms : -1;
}

ସ୍ଥିର timediff_t cshutdn_conn_timeleft(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା connectdata *conn)
{
  timediff_t ବାମ_ms = 0, ms;
  int8_t i;

  ପାଇଁ(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    ଯଦି(conn->shutdown.start_ms[i] < 0)
      ଜାରି;
    ms = Curl_cshutdn_timeleft_ms(data, conn, i);
    ଯଦି(ms && (!ବାମ_ms || ms < ବାମ_ms))
      ବାମ_ms = ms;
  }
  ଫେରନ୍ତୁ ବାମ_ms;
}

ଶୂନ୍ୟ Curl_cshutdn_clear_timer(ସଂରଚନା Curl_easy *data, int8_t sockindex)
{
  data->conn->shutdown.start_ms[sockindex] = -1;
}

ସ୍ଥିର ଶୂନ୍ୟ cshutdn_run_conn_handler(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା connectdata *conn)
{
  ଯଦି(!conn->bits.shutdown_handler) {

    ଯଦି(conn->scheme && conn->scheme->run->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      ଯଦି(data->state.internal) {
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

CURLcode Curl_cshutdn_try_once_idx(ସଂରଚନା Curl_easy *data,
                                   int8_t sockindex, bool *done)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(data->conn);

  ଯଦି(!CONN_SOCK_IDX_VALID(sockindex))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Get the first connected filter that is not shut down already. */
  cf = data->conn->cfilter[sockindex];
  ଯେତେବେଳେ(cf && (!cf->connected || cf->shutdown))
    cf = cf->next;

  ଯଦି(!cf) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  *done = FALSE;
  ଯଦି(!CURL_CONN_IN_SHUTDOWN(data->conn, sockindex)) {
    Curl_cshutdn_start_timer(data, sockindex, 0);
  }
  ଅନ୍ୟଥା {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, data->conn, sockindex);
    ଯଦି(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(data, "shutdown timeout");
      ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
    }
  }

  ଯେତେବେଳେ(cf) {
    ଯଦି(!cf->shutdown) {
      bool cfdone = FALSE;
      result = cf->cft->do_shutdown(cf, data, &cfdone);
      ଯଦି(result) {
        CURL_TRC_CF(data, cf, "shut down failed with %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        ଫେରନ୍ତୁ result;
      }
      ଅନ୍ୟଥା ଯଦି(!cfdone) {
        CURL_TRC_CF(data, cf, "shut down not done yet");
        ଫେରନ୍ତୁ CURLE_OK;
      }
      CURL_TRC_CF(data, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->next;
  }
  *done = (!result);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cshutdn_run_once(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା connectdata *conn,
                             bool *done)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(data->conn == conn);

  ଯଦି(!CURL_CONN_IN_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_start_timer(data, FIRSTSOCKET, 0);
  }

  cshutdn_run_conn_handler(data, conn);

  ଯଦି(conn->bits.shutdown_filters) {
    *done = TRUE;
    ଫେରନ୍ତୁ;
  }

  ଯଦି(!conn->bits.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(data, FIRSTSOCKET, &done1);
  ଅନ୍ୟଥା {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  ଯଦି(!conn->bits.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(data, SECONDARYSOCKET, &done2);
  ଅନ୍ୟଥା {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *done = (r1 || r2 || (done1 && done2));
  ଯଦି(*done)
    conn->bits.shutdown_filters = TRUE;
}

ଶୂନ୍ୟ Curl_cshutdn_try_once(ସଂରଚନା Curl_easy *admin,
                           ସଂରଚନା connectdata *conn,
                           bool *done)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_run_once(admin, conn, done);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *done);
  Curl_detach_connection(admin);
}

ଶୂନ୍ୟ Curl_cshutdn_terminate(ସଂରଚନା Curl_easy *admin,
                            ସଂରଚନା connectdata *conn,
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
  ଯଦି(do_shutdown) {
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

  ଯଦି(admin->multi)
    Curl_multi_ev_conn_done(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  ଯଦି(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

ସ୍ଥିର size_t cshutdn_destroy_oldest(ସଂରଚନା cshutdn *cshutdn,
                                     ସଂରଚନା Curl_easy *admin,
                                     ଅଚଳ ଅକ୍ଷର *destination,
                                     size_t ଗଣନା_2)
{
  ସଂରଚନା Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  ଯେତେବେଳେ((removed < ଗଣନା_2) && (i < CURL_PTRARRAY_ଗଣନା(&cshutdn->conns))) {
    ସଂରଚନା connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    ଯଦି(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    ଅନ୍ୟଥା
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  ଫେରନ୍ତୁ removed;
}

size_t Curl_cshutdn_close_oldest(ସଂରଚନା cshutdn *cshutdn,
                               ସଂରଚନା Curl_easy *admin,
                               ଅଚଳ ଅକ୍ଷର *destination,
                               size_t ଗଣନା_2)
{
  ଯଦି(cshutdn) {
    ଫେରନ୍ତୁ cshutdn_destroy_oldest(cshutdn, admin, destination, ଗଣନା_2);
  }
  ଫେରନ୍ତୁ 0;
}

#define ଅଙ୍କ_POLLS_ON_ଗଦା 10

ସ୍ଥିର CURLcode cshutdn_wait(ସଂରଚନା cshutdn *cshutdn,
                             ସଂରଚନା Curl_easy *admin,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout_ms)
{
  ସଂରଚନା pollfd a_few_on_ଗଦା[ଅଙ୍କ_POLLS_ON_ଗଦା];
  ସଂରଚନା curl_pollfds cpfds;
  CURLcode result;

  Curl_pollfds_init(&cpfds, a_few_on_ଗଦା, ଅଙ୍କ_POLLS_ON_ଗଦା);

  result = Curl_cshutdn_add_pollfds(cshutdn, admin, &cpfds);
  ଯଦି(result)
    ଯାଅ out;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

out:
  Curl_pollfds_cleanup(&cpfds);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cshutdn_perform(ସଂରଚନା cshutdn *cshutdn,
                            ସଂରଚନା Curl_easy *admin,
                            ସଂରଚନା Curl_sigpipe_ctx *sigpipe_ctx)
{
  ସଂରଚନା connectdata *conn;
  timediff_t next_expire_ms = 0, ms;
  uint32_t i = 0;
  bool done;

  ଯଦି(!CURL_PTRARRAY_ଗଣନା(&cshutdn->conns))
    ଫେରନ୍ତୁ;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_ଗଣନା(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  ଯେତେବେଳେ(i < CURL_PTRARRAY_ଗଣନା(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &done);
    ଯଦି(done) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    ଅନ୍ୟଥା {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      ଯଦି(ms && (!next_expire_ms || (ms < next_expire_ms)))
        next_expire_ms = ms;
    }
  }

  ଯଦି(next_expire_ms)
    Curl_expire(admin, next_expire_ms, EXPIRE_SHUTDOWN);
}

ସ୍ଥିର ଶୂନ୍ୟ cshutdn_terminate_all(ସଂରଚନା cshutdn *cshutdn,
                                  ସଂରଚନା Curl_easy *admin,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout_ms)
{
  ସଂରଚନା curltime started = *Curl_pgrs_now(admin);
  ସଂରଚନା Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  ଯେତେବେଳେ(CURL_PTRARRAY_ଗଣନା(&cshutdn->conns)) {
    timediff_t spent_ms;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    ଯଦି(!CURL_PTRARRAY_ଗଣନା(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      ଭାଙ୍ଗ;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    ଯଦି(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      ଭାଙ୍ଗ;
    }

    remain_ms = timeout_ms - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)spent_ms;
    ଯଦି(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      ଭାଙ୍ଗ;
    }
  }

  /* Terminate any remaining. */
  ଯେତେବେଳେ(CURL_PTRARRAY_ଗଣନା(&cshutdn->conns)) {
    ସଂରଚନା connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_ଗଣନା(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

ଶୂନ୍ୟ Curl_cshutdn_init(ସଂରଚନା cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

ଶୂନ୍ୟ Curl_cshutdn_destroy(ସଂରଚନା cshutdn *cshutdn,
                          ସଂରଚନା Curl_easy *admin)
{
  ଯଦି(admin) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout_ms = 0;
    /* for testing, run graceful shutdown */
#ifdef DEBUGBUILD
    {
      ଅଚଳ ଅକ୍ଷର *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      ଯଦି(p) {
        curl_off_t l;
        ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &l, INT_MAX))
          timeout_ms = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)l;
      }
    }
#endif
    ଯଦି(CURL_PTRARRAY_ଗଣନା(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_ଗଣନା(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_ଗଣନା(ସଂରଚନା cshutdn *cshutdn)
{
  ଯଦି(cshutdn) {
    ଫେରନ୍ତୁ CURL_PTRARRAY_ଗଣନା(&cshutdn->conns);
  }
  ଫେରନ୍ତୁ 0;
}

size_t Curl_cshutdn_dest_ଗଣନା(ସଂରଚନା cshutdn *cshutdn,
                               ଅଚଳ ଅକ୍ଷର *destination)
{
  ଯଦି(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    ପାଇଁ(i = 0; i < CURL_PTRARRAY_ଗଣନା(&cshutdn->conns); ++i) {
      ସଂରଚନା connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      ଯଦି(!strcmp(destination, conn->destination))
        ++n;
    }
    ଫେରନ୍ତୁ n;
  }
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର CURLMcode cshutdn_update_ev(ସଂରଚନା Curl_multi *multi,
                                   ସଂରଚନା connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  ଫେରନ୍ତୁ mresult;
}

ଶୂନ୍ୟ Curl_cshutdn_add(ସଂରଚନା cshutdn *cshutdn,
                      ସଂରଚନା Curl_multi *multi,
                      ସଂରଚନା connectdata *conn,
                      size_t max_shutdowns)
{
  ସଂରଚନା Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  ଯଦି(max_shutdowns <= CURL_PTRARRAY_ଗଣନା(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, ଖାଲି, 1);
  }

  ଯଦି(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    ଫେରନ୍ତୁ;
  }

  ଯଦି(Curl_ptrarray_add(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    ଫେରନ୍ତୁ;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_ଗଣନା(&cshutdn->conns));
}

ଶୂନ୍ୟ Curl_cshutdn_perform(ସଂରଚନା cshutdn *cshutdn,
                          ସଂରଚନା Curl_easy *admin,
                          ସଂରଚନା Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
ଶୂନ୍ୟ Curl_cshutdn_setfds(ସଂରଚନା cshutdn *cshutdn,
                         ସଂରଚନା Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା *maxfd)
{
  ଯଦି(CURL_PTRARRAY_ଗଣନା(&cshutdn->conns)) {
    ସଂରଚନା easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    ପାଇଁ(i = 0; i < CURL_PTRARRAY_ଗଣନା(&cshutdn->conns); ++i) {
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;
      ସଂରଚନା connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode result;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ଯଦି(result)
        ଜାରି;

      ପାଇଁ(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        ଯଦି(!FDSET_SOCK(sock))
          ଜାରି;
        ଯଦି(ps.actions[j] & CURL_POLL_IN)
          FD_SET(sock, read_fd_set);
        ଯଦି(ps.actions[j] & CURL_POLL_OUT)
          FD_SET(sock, write_fd_set);
        ଯଦି((ps.actions[j] & (CURL_POLL_OUT | CURL_POLL_IN)) &&
           ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sock > *maxfd))
          *maxfd = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_cshutdn_add_waitfds(ସଂରଚନା cshutdn *cshutdn,
                                      ସଂରଚନା Curl_easy *admin,
                                      ସଂରଚନା Curl_waitfds *cwfds)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା need = 0;

  ଯଦି(CURL_PTRARRAY_ଗଣନା(&cshutdn->conns)) {
    ସଂରଚନା easy_pollset ps;
    ସଂରଚନା connectdata *conn;
    uint32_t i;
    CURLcode result;

    Curl_pollset_init(&ps);
    ପାଇଁ(i = 0; i < CURL_PTRARRAY_ଗଣନା(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ଯଦି(!result)
        need += Curl_waitfds_add_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  ଫେରନ୍ତୁ need;
}

CURLcode Curl_cshutdn_add_pollfds(ସଂରଚନା cshutdn *cshutdn,
                                  ସଂରଚନା Curl_easy *admin,
                                  ସଂରଚନା curl_pollfds *cpfds)
{
  CURLcode result = CURLE_OK;

  ଯଦି(CURL_PTRARRAY_ଗଣନା(&cshutdn->conns)) {
    ସଂରଚନା easy_pollset ps;
    ସଂରଚନା connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    ପାଇଁ(i = 0; i < CURL_PTRARRAY_ଗଣନା(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      ଯଦି(!result)
        result = Curl_pollfds_add_ps(cpfds, &ps);
      ଯଦି(result) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        ଯାଅ out;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
out:
  ଫେରନ୍ତୁ result;
}
