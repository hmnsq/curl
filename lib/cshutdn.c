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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "url.h"
#شامل "cfilters.h"
#شامل "progress.h"
#شامل "multiif.h"
#شامل "multi_ev.h"
#شامل "curl_trc.h"
#شامل "cshutdn.h"
#شامل "sigpipe.h"
#شامل "connect.h"
#شامل "select.h"
#شامل "curlx/strparse.h"


#define DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

تهی Curl_cshutdn_start_timer(ساختار Curl_easy *data, int8_t sockindex,
                              صحیح timeout_ms)
{
  ساختار connectdata *conn = data->conn;
  ثابت ساختار curltime *pnow = Curl_pgrs_now(data);

  DEBUGASSERT(conn);
  conn->shutdown.start_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((data->set.shutdowntimeout > 0) ?
     data->set.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  اگر(data->mid)
    Curl_expire_set(data, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(data, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(ساختار Curl_easy *data,
                                    ساختار connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t چپ_ms;

  اگر(conn->shutdown.start_ms[sockindex] < 0)
    بازگشت 0; /* not started or no limits */

  چپ_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(data), &conn->created) -
             conn->shutdown.start_ms[sockindex]);
  بازگشت چپ_ms ? چپ_ms : -1;
}

ایستا timediff_t cshutdn_conn_timeleft(ساختار Curl_easy *data,
                                        ساختار connectdata *conn)
{
  timediff_t چپ_ms = 0, ms;
  int8_t i;

  برای(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    اگر(conn->shutdown.start_ms[i] < 0)
      ادامه;
    ms = Curl_cshutdn_timeleft_ms(data, conn, i);
    اگر(ms && (!چپ_ms || ms < چپ_ms))
      چپ_ms = ms;
  }
  بازگشت چپ_ms;
}

تهی Curl_cshutdn_clear_timer(ساختار Curl_easy *data, int8_t sockindex)
{
  data->conn->shutdown.start_ms[sockindex] = -1;
}

ایستا تهی cshutdn_run_conn_handler(ساختار Curl_easy *data,
                                     ساختار connectdata *conn)
{
  اگر(!conn->bits.shutdown_handler) {

    اگر(conn->scheme && conn->scheme->run->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      اگر(data->state.internal) {
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

CURLcode Curl_cshutdn_try_once_idx(ساختار Curl_easy *data,
                                   int8_t sockindex, bool *done)
{
  ساختار Curl_cfilter *cf;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(data->conn);

  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  /* Get the first connected filter that is not shut down already. */
  cf = data->conn->cfilter[sockindex];
  وقتی(cf && (!cf->connected || cf->shutdown))
    cf = cf->next;

  اگر(!cf) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  *done = FALSE;
  اگر(!CURL_CONN_IN_SHUTDOWN(data->conn, sockindex)) {
    Curl_cshutdn_start_timer(data, sockindex, 0);
  }
  وگرنه {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, data->conn, sockindex);
    اگر(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(data, "shutdown timeout");
      بازگشت CURLE_OPERATION_TIMEDOUT;
    }
  }

  وقتی(cf) {
    اگر(!cf->shutdown) {
      bool cfdone = FALSE;
      result = cf->cft->do_shutdown(cf, data, &cfdone);
      اگر(result) {
        CURL_TRC_CF(data, cf, "shut down failed with %d", (صحیح)result);
        بازگشت result;
      }
      وگرنه اگر(!cfdone) {
        CURL_TRC_CF(data, cf, "shut down not done yet");
        بازگشت CURLE_OK;
      }
      CURL_TRC_CF(data, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->next;
  }
  *done = (!result);
  بازگشت result;
}

ایستا تهی cshutdn_run_once(ساختار Curl_easy *data,
                             ساختار connectdata *conn,
                             bool *done)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(data->conn == conn);

  اگر(!CURL_CONN_IN_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_start_timer(data, FIRSTSOCKET, 0);
  }

  cshutdn_run_conn_handler(data, conn);

  اگر(conn->bits.shutdown_filters) {
    *done = TRUE;
    بازگشت;
  }

  اگر(!conn->bits.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(data, FIRSTSOCKET, &done1);
  وگرنه {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  اگر(!conn->bits.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(data, SECONDARYSOCKET, &done2);
  وگرنه {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *done = (r1 || r2 || (done1 && done2));
  اگر(*done)
    conn->bits.shutdown_filters = TRUE;
}

تهی Curl_cshutdn_try_once(ساختار Curl_easy *admin,
                           ساختار connectdata *conn,
                           bool *done)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_run_once(admin, conn, done);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *done);
  Curl_detach_connection(admin);
}

تهی Curl_cshutdn_terminate(ساختار Curl_easy *admin,
                            ساختار connectdata *conn,
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
  اگر(do_shutdown) {
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

  اگر(admin->multi)
    Curl_multi_ev_conn_done(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  اگر(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

ایستا size_t cshutdn_destroy_oldest(ساختار cshutdn *cshutdn,
                                     ساختار Curl_easy *admin,
                                     ثابت نویسه *destination,
                                     size_t شمار)
{
  ساختار Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  وقتی((removed < شمار) && (i < CURL_PTRARRAY_شمار(&cshutdn->conns))) {
    ساختار connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    اگر(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    وگرنه
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  بازگشت removed;
}

size_t Curl_cshutdn_close_oldest(ساختار cshutdn *cshutdn,
                               ساختار Curl_easy *admin,
                               ثابت نویسه *destination,
                               size_t شمار)
{
  اگر(cshutdn) {
    بازگشت cshutdn_destroy_oldest(cshutdn, admin, destination, شمار);
  }
  بازگشت 0;
}

#define رقم_POLLS_ON_پشته 10

ایستا CURLcode cshutdn_wait(ساختار cshutdn *cshutdn,
                             ساختار Curl_easy *admin,
                             صحیح timeout_ms)
{
  ساختار pollfd a_few_on_پشته[رقم_POLLS_ON_پشته];
  ساختار curl_pollfds cpfds;
  CURLcode result;

  Curl_pollfds_init(&cpfds, a_few_on_پشته, رقم_POLLS_ON_پشته);

  result = Curl_cshutdn_add_pollfds(cshutdn, admin, &cpfds);
  اگر(result)
    برو out;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

out:
  Curl_pollfds_cleanup(&cpfds);
  بازگشت result;
}

ایستا تهی cshutdn_perform(ساختار cshutdn *cshutdn,
                            ساختار Curl_easy *admin,
                            ساختار Curl_sigpipe_ctx *sigpipe_ctx)
{
  ساختار connectdata *conn;
  timediff_t next_expire_ms = 0, ms;
  uint32_t i = 0;
  bool done;

  اگر(!CURL_PTRARRAY_شمار(&cshutdn->conns))
    بازگشت;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_شمار(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  وقتی(i < CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &done);
    اگر(done) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    وگرنه {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      اگر(ms && (!next_expire_ms || (ms < next_expire_ms)))
        next_expire_ms = ms;
    }
  }

  اگر(next_expire_ms)
    Curl_expire(admin, next_expire_ms, EXPIRE_SHUTDOWN);
}

ایستا تهی cshutdn_terminate_all(ساختار cshutdn *cshutdn,
                                  ساختار Curl_easy *admin,
                                  صحیح timeout_ms)
{
  ساختار curltime started = *Curl_pgrs_now(admin);
  ساختار Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  وقتی(CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    timediff_t spent_ms;
    صحیح remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    اگر(!CURL_PTRARRAY_شمار(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      بشکن;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    اگر(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      بشکن;
    }

    remain_ms = timeout_ms - (صحیح)spent_ms;
    اگر(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      بشکن;
    }
  }

  /* Terminate any remaining. */
  وقتی(CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    ساختار connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_شمار(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

تهی Curl_cshutdn_init(ساختار cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

تهی Curl_cshutdn_destroy(ساختار cshutdn *cshutdn,
                          ساختار Curl_easy *admin)
{
  اگر(admin) {
    صحیح timeout_ms = 0;
    /* for testing, run graceful shutdown */
#ifdef DEBUGBUILD
    {
      ثابت نویسه *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      اگر(p) {
        curl_off_t l;
        اگر(!curlx_str_عدد(&p, &l, INT_MAX))
          timeout_ms = (صحیح)l;
      }
    }
#endif
    اگر(CURL_PTRARRAY_شمار(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_شمار(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_شمار(ساختار cshutdn *cshutdn)
{
  اگر(cshutdn) {
    بازگشت CURL_PTRARRAY_شمار(&cshutdn->conns);
  }
  بازگشت 0;
}

size_t Curl_cshutdn_dest_شمار(ساختار cshutdn *cshutdn,
                               ثابت نویسه *destination)
{
  اگر(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    برای(i = 0; i < CURL_PTRARRAY_شمار(&cshutdn->conns); ++i) {
      ساختار connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      اگر(!strcmp(destination, conn->destination))
        ++n;
    }
    بازگشت n;
  }
  بازگشت 0;
}

ایستا CURLMcode cshutdn_update_ev(ساختار Curl_multi *multi,
                                   ساختار connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  بازگشت mresult;
}

تهی Curl_cshutdn_add(ساختار cshutdn *cshutdn,
                      ساختار Curl_multi *multi,
                      ساختار connectdata *conn,
                      size_t max_shutdowns)
{
  ساختار Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  اگر(max_shutdowns <= CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, پوچ, 1);
  }

  اگر(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    بازگشت;
  }

  اگر(Curl_ptrarray_add(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    بازگشت;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_شمار(&cshutdn->conns));
}

تهی Curl_cshutdn_perform(ساختار cshutdn *cshutdn,
                          ساختار Curl_easy *admin,
                          ساختار Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
تهی Curl_cshutdn_setfds(ساختار cshutdn *cshutdn,
                         ساختار Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         صحیح *maxfd)
{
  اگر(CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    ساختار easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    برای(i = 0; i < CURL_PTRARRAY_شمار(&cshutdn->conns); ++i) {
      unsigned صحیح j;
      ساختار connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode result;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      اگر(result)
        ادامه;

      برای(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        اگر(!FDSET_SOCK(sock))
          ادامه;
        اگر(ps.actions[j] & CURL_POLL_IN)
          FD_SET(sock, read_fd_set);
        اگر(ps.actions[j] & CURL_POLL_OUT)
          FD_SET(sock, write_fd_set);
        اگر((ps.actions[j] & (CURL_POLL_OUT | CURL_POLL_IN)) &&
           ((صحیح)sock > *maxfd))
          *maxfd = (صحیح)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
unsigned صحیح Curl_cshutdn_add_waitfds(ساختار cshutdn *cshutdn,
                                      ساختار Curl_easy *admin,
                                      ساختار Curl_waitfds *cwfds)
{
  unsigned صحیح need = 0;

  اگر(CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    ساختار easy_pollset ps;
    ساختار connectdata *conn;
    uint32_t i;
    CURLcode result;

    Curl_pollset_init(&ps);
    برای(i = 0; i < CURL_PTRARRAY_شمار(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      اگر(!result)
        need += Curl_waitfds_add_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  بازگشت need;
}

CURLcode Curl_cshutdn_add_pollfds(ساختار cshutdn *cshutdn,
                                  ساختار Curl_easy *admin,
                                  ساختار curl_pollfds *cpfds)
{
  CURLcode result = CURLE_OK;

  اگر(CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    ساختار easy_pollset ps;
    ساختار connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    برای(i = 0; i < CURL_PTRARRAY_شمار(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      اگر(!result)
        result = Curl_pollfds_add_ps(cpfds, &ps);
      اگر(result) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        برو out;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
out:
  بازگشت result;
}
