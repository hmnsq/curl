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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "multi_ev.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "cshutdn.h"
#அந்தர்பாவ "sigpipe.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "curlx/strparse.h"


#define DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

வெறுமை Curl_cshutdn_start_timer(கட்டமைப்பு Curl_easy *data, int8_t sockindex,
                              முழு timeout_ms)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  மாறிலி கட்டமைப்பு curltime *pnow = Curl_pgrs_now(data);

  DEBUGASSERT(conn);
  conn->shutdown.start_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((data->set.shutdowntimeout > 0) ?
     data->set.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  எனில்(data->mid)
    Curl_expire_set(data, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(data, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t இடது_ms;

  எனில்(conn->shutdown.start_ms[sockindex] < 0)
    திரும்பு 0; /* not started or no limits */

  இடது_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(data), &conn->created) -
             conn->shutdown.start_ms[sockindex]);
  திரும்பு இடது_ms ? இடது_ms : -1;
}

நிலையான timediff_t cshutdn_conn_timeleft(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு connectdata *conn)
{
  timediff_t இடது_ms = 0, ms;
  int8_t i;

  க்கு(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    எனில்(conn->shutdown.start_ms[i] < 0)
      தொடர்;
    ms = Curl_cshutdn_timeleft_ms(data, conn, i);
    எனில்(ms && (!இடது_ms || ms < இடது_ms))
      இடது_ms = ms;
  }
  திரும்பு இடது_ms;
}

வெறுமை Curl_cshutdn_clear_timer(கட்டமைப்பு Curl_easy *data, int8_t sockindex)
{
  data->conn->shutdown.start_ms[sockindex] = -1;
}

நிலையான வெறுமை cshutdn_run_conn_handler(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு connectdata *conn)
{
  எனில்(!conn->bits.shutdown_handler) {

    எனில்(conn->scheme && conn->scheme->run->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      எனில்(data->state.internal) {
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

CURLcode Curl_cshutdn_try_once_idx(கட்டமைப்பு Curl_easy *data,
                                   int8_t sockindex, bool *done)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(data->conn);

  எனில்(!CONN_SOCK_IDX_VALID(sockindex))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  /* Get the first connected filter that is not shut down already. */
  cf = data->conn->cfilter[sockindex];
  போது(cf && (!cf->connected || cf->shutdown))
    cf = cf->next;

  எனில்(!cf) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  *done = FALSE;
  எனில்(!CURL_CONN_IN_SHUTDOWN(data->conn, sockindex)) {
    Curl_cshutdn_start_timer(data, sockindex, 0);
  }
  இல்லையெனில் {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, data->conn, sockindex);
    எனில்(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(data, "shutdown timeout");
      திரும்பு CURLE_OPERATION_TIMEDOUT;
    }
  }

  போது(cf) {
    எனில்(!cf->shutdown) {
      bool cfdone = FALSE;
      result = cf->cft->do_shutdown(cf, data, &cfdone);
      எனில்(result) {
        CURL_TRC_CF(data, cf, "shut down failed with %d", (முழு)result);
        திரும்பு result;
      }
      இல்லையெனில் எனில்(!cfdone) {
        CURL_TRC_CF(data, cf, "shut down not done yet");
        திரும்பு CURLE_OK;
      }
      CURL_TRC_CF(data, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->next;
  }
  *done = (!result);
  திரும்பு result;
}

நிலையான வெறுமை cshutdn_run_once(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு connectdata *conn,
                             bool *done)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(data->conn == conn);

  எனில்(!CURL_CONN_IN_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_start_timer(data, FIRSTSOCKET, 0);
  }

  cshutdn_run_conn_handler(data, conn);

  எனில்(conn->bits.shutdown_filters) {
    *done = TRUE;
    திரும்பு;
  }

  எனில்(!conn->bits.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(data, FIRSTSOCKET, &done1);
  இல்லையெனில் {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  எனில்(!conn->bits.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(data, SECONDARYSOCKET, &done2);
  இல்லையெனில் {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *done = (r1 || r2 || (done1 && done2));
  எனில்(*done)
    conn->bits.shutdown_filters = TRUE;
}

வெறுமை Curl_cshutdn_try_once(கட்டமைப்பு Curl_easy *admin,
                           கட்டமைப்பு connectdata *conn,
                           bool *done)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_run_once(admin, conn, done);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *done);
  Curl_detach_connection(admin);
}

வெறுமை Curl_cshutdn_terminate(கட்டமைப்பு Curl_easy *admin,
                            கட்டமைப்பு connectdata *conn,
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
  எனில்(do_shutdown) {
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

  எனில்(admin->multi)
    Curl_multi_ev_conn_done(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  எனில்(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

நிலையான size_t cshutdn_destroy_oldest(கட்டமைப்பு cshutdn *cshutdn,
                                     கட்டமைப்பு Curl_easy *admin,
                                     மாறிலி எழுத்து *destination,
                                     size_t மொத்தம்)
{
  கட்டமைப்பு Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  போது((removed < மொத்தம்) && (i < CURL_PTRARRAY_மொத்தம்(&cshutdn->conns))) {
    கட்டமைப்பு connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    எனில்(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    இல்லையெனில்
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  திரும்பு removed;
}

size_t Curl_cshutdn_close_oldest(கட்டமைப்பு cshutdn *cshutdn,
                               கட்டமைப்பு Curl_easy *admin,
                               மாறிலி எழுத்து *destination,
                               size_t மொத்தம்)
{
  எனில்(cshutdn) {
    திரும்பு cshutdn_destroy_oldest(cshutdn, admin, destination, மொத்தம்);
  }
  திரும்பு 0;
}

#define இலக்கம்_POLLS_ON_அடுக்கு 10

நிலையான CURLcode cshutdn_wait(கட்டமைப்பு cshutdn *cshutdn,
                             கட்டமைப்பு Curl_easy *admin,
                             முழு timeout_ms)
{
  கட்டமைப்பு pollfd a_few_on_அடுக்கு[இலக்கம்_POLLS_ON_அடுக்கு];
  கட்டமைப்பு curl_pollfds cpfds;
  CURLcode result;

  Curl_pollfds_init(&cpfds, a_few_on_அடுக்கு, இலக்கம்_POLLS_ON_அடுக்கு);

  result = Curl_cshutdn_add_pollfds(cshutdn, admin, &cpfds);
  எனில்(result)
    செல் out;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

out:
  Curl_pollfds_cleanup(&cpfds);
  திரும்பு result;
}

நிலையான வெறுமை cshutdn_perform(கட்டமைப்பு cshutdn *cshutdn,
                            கட்டமைப்பு Curl_easy *admin,
                            கட்டமைப்பு Curl_sigpipe_ctx *sigpipe_ctx)
{
  கட்டமைப்பு connectdata *conn;
  timediff_t next_expire_ms = 0, ms;
  uint32_t i = 0;
  bool done;

  எனில்(!CURL_PTRARRAY_மொத்தம்(&cshutdn->conns))
    திரும்பு;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_மொத்தம்(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  போது(i < CURL_PTRARRAY_மொத்தம்(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &done);
    எனில்(done) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    இல்லையெனில் {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      எனில்(ms && (!next_expire_ms || (ms < next_expire_ms)))
        next_expire_ms = ms;
    }
  }

  எனில்(next_expire_ms)
    Curl_expire(admin, next_expire_ms, EXPIRE_SHUTDOWN);
}

நிலையான வெறுமை cshutdn_terminate_all(கட்டமைப்பு cshutdn *cshutdn,
                                  கட்டமைப்பு Curl_easy *admin,
                                  முழு timeout_ms)
{
  கட்டமைப்பு curltime started = *Curl_pgrs_now(admin);
  கட்டமைப்பு Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  போது(CURL_PTRARRAY_மொத்தம்(&cshutdn->conns)) {
    timediff_t spent_ms;
    முழு remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    எனில்(!CURL_PTRARRAY_மொத்தம்(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      நிறுத்து;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    எனில்(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      நிறுத்து;
    }

    remain_ms = timeout_ms - (முழு)spent_ms;
    எனில்(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      நிறுத்து;
    }
  }

  /* Terminate any remaining. */
  போது(CURL_PTRARRAY_மொத்தம்(&cshutdn->conns)) {
    கட்டமைப்பு connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_மொத்தம்(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

வெறுமை Curl_cshutdn_init(கட்டமைப்பு cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

வெறுமை Curl_cshutdn_destroy(கட்டமைப்பு cshutdn *cshutdn,
                          கட்டமைப்பு Curl_easy *admin)
{
  எனில்(admin) {
    முழு timeout_ms = 0;
    /* for testing, run graceful shutdown */
#ifdef DEBUGBUILD
    {
      மாறிலி எழுத்து *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      எனில்(p) {
        curl_off_t l;
        எனில்(!curlx_str_எண்(&p, &l, INT_MAX))
          timeout_ms = (முழு)l;
      }
    }
#endif
    எனில்(CURL_PTRARRAY_மொத்தம்(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_மொத்தம்(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_மொத்தம்(கட்டமைப்பு cshutdn *cshutdn)
{
  எனில்(cshutdn) {
    திரும்பு CURL_PTRARRAY_மொத்தம்(&cshutdn->conns);
  }
  திரும்பு 0;
}

size_t Curl_cshutdn_dest_மொத்தம்(கட்டமைப்பு cshutdn *cshutdn,
                               மாறிலி எழுத்து *destination)
{
  எனில்(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    க்கு(i = 0; i < CURL_PTRARRAY_மொத்தம்(&cshutdn->conns); ++i) {
      கட்டமைப்பு connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      எனில்(!strcmp(destination, conn->destination))
        ++n;
    }
    திரும்பு n;
  }
  திரும்பு 0;
}

நிலையான CURLMcode cshutdn_update_ev(கட்டமைப்பு Curl_multi *multi,
                                   கட்டமைப்பு connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  திரும்பு mresult;
}

வெறுமை Curl_cshutdn_add(கட்டமைப்பு cshutdn *cshutdn,
                      கட்டமைப்பு Curl_multi *multi,
                      கட்டமைப்பு connectdata *conn,
                      size_t max_shutdowns)
{
  கட்டமைப்பு Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  எனில்(max_shutdowns <= CURL_PTRARRAY_மொத்தம்(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, வெற்று, 1);
  }

  எனில்(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    திரும்பு;
  }

  எனில்(Curl_ptrarray_add(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    திரும்பு;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_மொத்தம்(&cshutdn->conns));
}

வெறுமை Curl_cshutdn_perform(கட்டமைப்பு cshutdn *cshutdn,
                          கட்டமைப்பு Curl_easy *admin,
                          கட்டமைப்பு Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
வெறுமை Curl_cshutdn_setfds(கட்டமைப்பு cshutdn *cshutdn,
                         கட்டமைப்பு Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         முழு *maxfd)
{
  எனில்(CURL_PTRARRAY_மொத்தம்(&cshutdn->conns)) {
    கட்டமைப்பு easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    க்கு(i = 0; i < CURL_PTRARRAY_மொத்தம்(&cshutdn->conns); ++i) {
      unsigned முழு j;
      கட்டமைப்பு connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode result;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      எனில்(result)
        தொடர்;

      க்கு(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        எனில்(!FDSET_SOCK(sock))
          தொடர்;
        எனில்(ps.actions[j] & CURL_POLL_IN)
          FD_SET(sock, read_fd_set);
        எனில்(ps.actions[j] & CURL_POLL_OUT)
          FD_SET(sock, write_fd_set);
        எனில்((ps.actions[j] & (CURL_POLL_OUT | CURL_POLL_IN)) &&
           ((முழு)sock > *maxfd))
          *maxfd = (முழு)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
unsigned முழு Curl_cshutdn_add_waitfds(கட்டமைப்பு cshutdn *cshutdn,
                                      கட்டமைப்பு Curl_easy *admin,
                                      கட்டமைப்பு Curl_waitfds *cwfds)
{
  unsigned முழு need = 0;

  எனில்(CURL_PTRARRAY_மொத்தம்(&cshutdn->conns)) {
    கட்டமைப்பு easy_pollset ps;
    கட்டமைப்பு connectdata *conn;
    uint32_t i;
    CURLcode result;

    Curl_pollset_init(&ps);
    க்கு(i = 0; i < CURL_PTRARRAY_மொத்தம்(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      எனில்(!result)
        need += Curl_waitfds_add_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  திரும்பு need;
}

CURLcode Curl_cshutdn_add_pollfds(கட்டமைப்பு cshutdn *cshutdn,
                                  கட்டமைப்பு Curl_easy *admin,
                                  கட்டமைப்பு curl_pollfds *cpfds)
{
  CURLcode result = CURLE_OK;

  எனில்(CURL_PTRARRAY_மொத்தம்(&cshutdn->conns)) {
    கட்டமைப்பு easy_pollset ps;
    கட்டமைப்பு connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    க்கு(i = 0; i < CURL_PTRARRAY_மொத்தம்(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      எனில்(!result)
        result = Curl_pollfds_add_ps(cpfds, &ps);
      எனில்(result) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        செல் out;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
out:
  திரும்பு result;
}
