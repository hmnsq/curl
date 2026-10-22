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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "multi_ev.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "cshutdn.h"
#അന്തര്ഭാവ "sigpipe.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "curlx/strparse.h"


#define DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

ശൂന്യം Curl_cshutdn_start_timer(ഘടന Curl_easy *data, int8_t sockindex,
                              പൂർണ്ണസംഖ്യ timeout_ms)
{
  ഘടന connectdata *conn = data->conn;
  സ്ഥിരം ഘടന curltime *pnow = Curl_pgrs_now(data);

  DEBUGASSERT(conn);
  conn->shutdown.start_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((data->set.shutdowntimeout > 0) ?
     data->set.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  എങ്കിൽ(data->mid)
    Curl_expire_set(data, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(data, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(ഘടന Curl_easy *data,
                                    ഘടന connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t ഇടത്_ms;

  എങ്കിൽ(conn->shutdown.start_ms[sockindex] < 0)
    തിരികെ 0; /* not started or no limits */

  ഇടത്_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(data), &conn->created) -
             conn->shutdown.start_ms[sockindex]);
  തിരികെ ഇടത്_ms ? ഇടത്_ms : -1;
}

സ്ഥിതം timediff_t cshutdn_conn_timeleft(ഘടന Curl_easy *data,
                                        ഘടന connectdata *conn)
{
  timediff_t ഇടത്_ms = 0, ms;
  int8_t i;

  വേണ്ടി(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    എങ്കിൽ(conn->shutdown.start_ms[i] < 0)
      തുടരുക;
    ms = Curl_cshutdn_timeleft_ms(data, conn, i);
    എങ്കിൽ(ms && (!ഇടത്_ms || ms < ഇടത്_ms))
      ഇടത്_ms = ms;
  }
  തിരികെ ഇടത്_ms;
}

ശൂന്യം Curl_cshutdn_clear_timer(ഘടന Curl_easy *data, int8_t sockindex)
{
  data->conn->shutdown.start_ms[sockindex] = -1;
}

സ്ഥിതം ശൂന്യം cshutdn_run_conn_handler(ഘടന Curl_easy *data,
                                     ഘടന connectdata *conn)
{
  എങ്കിൽ(!conn->bits.shutdown_handler) {

    എങ്കിൽ(conn->scheme && conn->scheme->run->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      എങ്കിൽ(data->state.internal) {
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

CURLcode Curl_cshutdn_try_once_idx(ഘടന Curl_easy *data,
                                   int8_t sockindex, bool *done)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(data->conn);

  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  /* Get the first connected filter that is not shut down already. */
  cf = data->conn->cfilter[sockindex];
  ആകുമ്പോൾ(cf && (!cf->connected || cf->shutdown))
    cf = cf->next;

  എങ്കിൽ(!cf) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  *done = FALSE;
  എങ്കിൽ(!CURL_CONN_IN_SHUTDOWN(data->conn, sockindex)) {
    Curl_cshutdn_start_timer(data, sockindex, 0);
  }
  അല്ലെങ്കിൽ {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, data->conn, sockindex);
    എങ്കിൽ(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(data, "shutdown timeout");
      തിരികെ CURLE_OPERATION_TIMEDOUT;
    }
  }

  ആകുമ്പോൾ(cf) {
    എങ്കിൽ(!cf->shutdown) {
      bool cfdone = FALSE;
      result = cf->cft->do_shutdown(cf, data, &cfdone);
      എങ്കിൽ(result) {
        CURL_TRC_CF(data, cf, "shut down failed with %d", (പൂർണ്ണസംഖ്യ)result);
        തിരികെ result;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(!cfdone) {
        CURL_TRC_CF(data, cf, "shut down not done yet");
        തിരികെ CURLE_OK;
      }
      CURL_TRC_CF(data, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->next;
  }
  *done = (!result);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cshutdn_run_once(ഘടന Curl_easy *data,
                             ഘടന connectdata *conn,
                             bool *done)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(data->conn == conn);

  എങ്കിൽ(!CURL_CONN_IN_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_start_timer(data, FIRSTSOCKET, 0);
  }

  cshutdn_run_conn_handler(data, conn);

  എങ്കിൽ(conn->bits.shutdown_filters) {
    *done = TRUE;
    തിരികെ;
  }

  എങ്കിൽ(!conn->bits.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(data, FIRSTSOCKET, &done1);
  അല്ലെങ്കിൽ {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  എങ്കിൽ(!conn->bits.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(data, SECONDARYSOCKET, &done2);
  അല്ലെങ്കിൽ {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *done = (r1 || r2 || (done1 && done2));
  എങ്കിൽ(*done)
    conn->bits.shutdown_filters = TRUE;
}

ശൂന്യം Curl_cshutdn_try_once(ഘടന Curl_easy *admin,
                           ഘടന connectdata *conn,
                           bool *done)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_run_once(admin, conn, done);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *done);
  Curl_detach_connection(admin);
}

ശൂന്യം Curl_cshutdn_terminate(ഘടന Curl_easy *admin,
                            ഘടന connectdata *conn,
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
  എങ്കിൽ(do_shutdown) {
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

  എങ്കിൽ(admin->multi)
    Curl_multi_ev_conn_done(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  എങ്കിൽ(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

സ്ഥിതം size_t cshutdn_destroy_oldest(ഘടന cshutdn *cshutdn,
                                     ഘടന Curl_easy *admin,
                                     സ്ഥിരം അക്ഷരം *destination,
                                     size_t എണ്ണം)
{
  ഘടന Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  ആകുമ്പോൾ((removed < എണ്ണം) && (i < CURL_PTRARRAY_എണ്ണം(&cshutdn->conns))) {
    ഘടന connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    എങ്കിൽ(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    അല്ലെങ്കിൽ
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  തിരികെ removed;
}

size_t Curl_cshutdn_close_oldest(ഘടന cshutdn *cshutdn,
                               ഘടന Curl_easy *admin,
                               സ്ഥിരം അക്ഷരം *destination,
                               size_t എണ്ണം)
{
  എങ്കിൽ(cshutdn) {
    തിരികെ cshutdn_destroy_oldest(cshutdn, admin, destination, എണ്ണം);
  }
  തിരികെ 0;
}

#define അക്കം_POLLS_ON_അട്ടി 10

സ്ഥിതം CURLcode cshutdn_wait(ഘടന cshutdn *cshutdn,
                             ഘടന Curl_easy *admin,
                             പൂർണ്ണസംഖ്യ timeout_ms)
{
  ഘടന pollfd a_few_on_അട്ടി[അക്കം_POLLS_ON_അട്ടി];
  ഘടന curl_pollfds cpfds;
  CURLcode result;

  Curl_pollfds_init(&cpfds, a_few_on_അട്ടി, അക്കം_POLLS_ON_അട്ടി);

  result = Curl_cshutdn_add_pollfds(cshutdn, admin, &cpfds);
  എങ്കിൽ(result)
    പോകുക out;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

out:
  Curl_pollfds_cleanup(&cpfds);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cshutdn_perform(ഘടന cshutdn *cshutdn,
                            ഘടന Curl_easy *admin,
                            ഘടന Curl_sigpipe_ctx *sigpipe_ctx)
{
  ഘടന connectdata *conn;
  timediff_t next_expire_ms = 0, ms;
  uint32_t i = 0;
  bool done;

  എങ്കിൽ(!CURL_PTRARRAY_എണ്ണം(&cshutdn->conns))
    തിരികെ;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_എണ്ണം(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  ആകുമ്പോൾ(i < CURL_PTRARRAY_എണ്ണം(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &done);
    എങ്കിൽ(done) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    അല്ലെങ്കിൽ {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      എങ്കിൽ(ms && (!next_expire_ms || (ms < next_expire_ms)))
        next_expire_ms = ms;
    }
  }

  എങ്കിൽ(next_expire_ms)
    Curl_expire(admin, next_expire_ms, EXPIRE_SHUTDOWN);
}

സ്ഥിതം ശൂന്യം cshutdn_terminate_all(ഘടന cshutdn *cshutdn,
                                  ഘടന Curl_easy *admin,
                                  പൂർണ്ണസംഖ്യ timeout_ms)
{
  ഘടന curltime started = *Curl_pgrs_now(admin);
  ഘടന Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  ആകുമ്പോൾ(CURL_PTRARRAY_എണ്ണം(&cshutdn->conns)) {
    timediff_t spent_ms;
    പൂർണ്ണസംഖ്യ remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    എങ്കിൽ(!CURL_PTRARRAY_എണ്ണം(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      നിർത്തുക;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    എങ്കിൽ(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      നിർത്തുക;
    }

    remain_ms = timeout_ms - (പൂർണ്ണസംഖ്യ)spent_ms;
    എങ്കിൽ(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      നിർത്തുക;
    }
  }

  /* Terminate any remaining. */
  ആകുമ്പോൾ(CURL_PTRARRAY_എണ്ണം(&cshutdn->conns)) {
    ഘടന connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_എണ്ണം(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

ശൂന്യം Curl_cshutdn_init(ഘടന cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

ശൂന്യം Curl_cshutdn_destroy(ഘടന cshutdn *cshutdn,
                          ഘടന Curl_easy *admin)
{
  എങ്കിൽ(admin) {
    പൂർണ്ണസംഖ്യ timeout_ms = 0;
    /* for testing, run graceful shutdown */
#ifdef DEBUGBUILD
    {
      സ്ഥിരം അക്ഷരം *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      എങ്കിൽ(p) {
        curl_off_t l;
        എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &l, INT_MAX))
          timeout_ms = (പൂർണ്ണസംഖ്യ)l;
      }
    }
#endif
    എങ്കിൽ(CURL_PTRARRAY_എണ്ണം(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_എണ്ണം(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_എണ്ണം(ഘടന cshutdn *cshutdn)
{
  എങ്കിൽ(cshutdn) {
    തിരികെ CURL_PTRARRAY_എണ്ണം(&cshutdn->conns);
  }
  തിരികെ 0;
}

size_t Curl_cshutdn_dest_എണ്ണം(ഘടന cshutdn *cshutdn,
                               സ്ഥിരം അക്ഷരം *destination)
{
  എങ്കിൽ(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    വേണ്ടി(i = 0; i < CURL_PTRARRAY_എണ്ണം(&cshutdn->conns); ++i) {
      ഘടന connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      എങ്കിൽ(!strcmp(destination, conn->destination))
        ++n;
    }
    തിരികെ n;
  }
  തിരികെ 0;
}

സ്ഥിതം CURLMcode cshutdn_update_ev(ഘടന Curl_multi *multi,
                                   ഘടന connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  തിരികെ mresult;
}

ശൂന്യം Curl_cshutdn_add(ഘടന cshutdn *cshutdn,
                      ഘടന Curl_multi *multi,
                      ഘടന connectdata *conn,
                      size_t max_shutdowns)
{
  ഘടന Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  എങ്കിൽ(max_shutdowns <= CURL_PTRARRAY_എണ്ണം(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, ശൂന്യ, 1);
  }

  എങ്കിൽ(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    തിരികെ;
  }

  എങ്കിൽ(Curl_ptrarray_add(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    തിരികെ;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_എണ്ണം(&cshutdn->conns));
}

ശൂന്യം Curl_cshutdn_perform(ഘടന cshutdn *cshutdn,
                          ഘടന Curl_easy *admin,
                          ഘടന Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
ശൂന്യം Curl_cshutdn_setfds(ഘടന cshutdn *cshutdn,
                         ഘടന Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         പൂർണ്ണസംഖ്യ *maxfd)
{
  എങ്കിൽ(CURL_PTRARRAY_എണ്ണം(&cshutdn->conns)) {
    ഘടന easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    വേണ്ടി(i = 0; i < CURL_PTRARRAY_എണ്ണം(&cshutdn->conns); ++i) {
      unsigned പൂർണ്ണസംഖ്യ j;
      ഘടന connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode result;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      എങ്കിൽ(result)
        തുടരുക;

      വേണ്ടി(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        എങ്കിൽ(!FDSET_SOCK(sock))
          തുടരുക;
        എങ്കിൽ(ps.actions[j] & CURL_POLL_IN)
          FD_SET(sock, read_fd_set);
        എങ്കിൽ(ps.actions[j] & CURL_POLL_OUT)
          FD_SET(sock, write_fd_set);
        എങ്കിൽ((ps.actions[j] & (CURL_POLL_OUT | CURL_POLL_IN)) &&
           ((പൂർണ്ണസംഖ്യ)sock > *maxfd))
          *maxfd = (പൂർണ്ണസംഖ്യ)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
unsigned പൂർണ്ണസംഖ്യ Curl_cshutdn_add_waitfds(ഘടന cshutdn *cshutdn,
                                      ഘടന Curl_easy *admin,
                                      ഘടന Curl_waitfds *cwfds)
{
  unsigned പൂർണ്ണസംഖ്യ need = 0;

  എങ്കിൽ(CURL_PTRARRAY_എണ്ണം(&cshutdn->conns)) {
    ഘടന easy_pollset ps;
    ഘടന connectdata *conn;
    uint32_t i;
    CURLcode result;

    Curl_pollset_init(&ps);
    വേണ്ടി(i = 0; i < CURL_PTRARRAY_എണ്ണം(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      എങ്കിൽ(!result)
        need += Curl_waitfds_add_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  തിരികെ need;
}

CURLcode Curl_cshutdn_add_pollfds(ഘടന cshutdn *cshutdn,
                                  ഘടന Curl_easy *admin,
                                  ഘടന curl_pollfds *cpfds)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(CURL_PTRARRAY_എണ്ണം(&cshutdn->conns)) {
    ഘടന easy_pollset ps;
    ഘടന connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    വേണ്ടി(i = 0; i < CURL_PTRARRAY_എണ്ണം(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      എങ്കിൽ(!result)
        result = Curl_pollfds_add_ps(cpfds, &ps);
      എങ്കിൽ(result) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        പോകുക out;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
out:
  തിരികെ result;
}
