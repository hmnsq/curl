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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "multi_ev.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "cshutdn.h"
#અન્તર્ભાવ "sigpipe.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "curlx/strparse.h"


#define DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

શૂન્ય Curl_cshutdn_start_timer(સંરચના Curl_easy *data, int8_t sockindex,
                              પૂર્ણાંક timeout_ms)
{
  સંરચના connectdata *conn = data->conn;
  અચલ સંરચના curltime *pnow = Curl_pgrs_now(data);

  DEBUGASSERT(conn);
  conn->shutdown.start_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((data->set.shutdowntimeout > 0) ?
     data->set.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  જો(data->mid)
    Curl_expire_set(data, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(data, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(સંરચના Curl_easy *data,
                                    સંરચના connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t ડાબું_ms;

  જો(conn->shutdown.start_ms[sockindex] < 0)
    પાછા 0; /* not started or no limits */

  ડાબું_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(data), &conn->created) -
             conn->shutdown.start_ms[sockindex]);
  પાછા ડાબું_ms ? ડાબું_ms : -1;
}

સ્થિર timediff_t cshutdn_conn_timeleft(સંરચના Curl_easy *data,
                                        સંરચના connectdata *conn)
{
  timediff_t ડાબું_ms = 0, ms;
  int8_t i;

  માટે(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    જો(conn->shutdown.start_ms[i] < 0)
      ચાલુ;
    ms = Curl_cshutdn_timeleft_ms(data, conn, i);
    જો(ms && (!ડાબું_ms || ms < ડાબું_ms))
      ડાબું_ms = ms;
  }
  પાછા ડાબું_ms;
}

શૂન્ય Curl_cshutdn_clear_timer(સંરચના Curl_easy *data, int8_t sockindex)
{
  data->conn->shutdown.start_ms[sockindex] = -1;
}

સ્થિર શૂન્ય cshutdn_run_conn_handler(સંરચના Curl_easy *data,
                                     સંરચના connectdata *conn)
{
  જો(!conn->bits.shutdown_handler) {

    જો(conn->scheme && conn->scheme->run->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      જો(data->state.internal) {
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

CURLcode Curl_cshutdn_try_once_idx(સંરચના Curl_easy *data,
                                   int8_t sockindex, bool *done)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(data->conn);

  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  /* Get the first connected filter that is not shut down already. */
  cf = data->conn->cfilter[sockindex];
  જ્યારે(cf && (!cf->connected || cf->shutdown))
    cf = cf->next;

  જો(!cf) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  *done = FALSE;
  જો(!CURL_CONN_IN_SHUTDOWN(data->conn, sockindex)) {
    Curl_cshutdn_start_timer(data, sockindex, 0);
  }
  નહીંતર {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, data->conn, sockindex);
    જો(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(data, "shutdown timeout");
      પાછા CURLE_OPERATION_TIMEDOUT;
    }
  }

  જ્યારે(cf) {
    જો(!cf->shutdown) {
      bool cfdone = FALSE;
      result = cf->cft->do_shutdown(cf, data, &cfdone);
      જો(result) {
        CURL_TRC_CF(data, cf, "shut down failed with %d", (પૂર્ણાંક)result);
        પાછા result;
      }
      નહીંતર જો(!cfdone) {
        CURL_TRC_CF(data, cf, "shut down not done yet");
        પાછા CURLE_OK;
      }
      CURL_TRC_CF(data, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->next;
  }
  *done = (!result);
  પાછા result;
}

સ્થિર શૂન્ય cshutdn_run_once(સંરચના Curl_easy *data,
                             સંરચના connectdata *conn,
                             bool *done)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(data->conn == conn);

  જો(!CURL_CONN_IN_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_start_timer(data, FIRSTSOCKET, 0);
  }

  cshutdn_run_conn_handler(data, conn);

  જો(conn->bits.shutdown_filters) {
    *done = TRUE;
    પાછા;
  }

  જો(!conn->bits.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(data, FIRSTSOCKET, &done1);
  નહીંતર {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  જો(!conn->bits.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(data, SECONDARYSOCKET, &done2);
  નહીંતર {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *done = (r1 || r2 || (done1 && done2));
  જો(*done)
    conn->bits.shutdown_filters = TRUE;
}

શૂન્ય Curl_cshutdn_try_once(સંરચના Curl_easy *admin,
                           સંરચના connectdata *conn,
                           bool *done)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_run_once(admin, conn, done);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *done);
  Curl_detach_connection(admin);
}

શૂન્ય Curl_cshutdn_terminate(સંરચના Curl_easy *admin,
                            સંરચના connectdata *conn,
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
  જો(do_shutdown) {
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

  જો(admin->multi)
    Curl_multi_ev_conn_done(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  જો(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

સ્થિર size_t cshutdn_destroy_oldest(સંરચના cshutdn *cshutdn,
                                     સંરચના Curl_easy *admin,
                                     અચલ અક્ષર *destination,
                                     size_t ગણતરી)
{
  સંરચના Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  જ્યારે((removed < ગણતરી) && (i < CURL_PTRARRAY_ગણતરી(&cshutdn->conns))) {
    સંરચના connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    જો(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    નહીંતર
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  પાછા removed;
}

size_t Curl_cshutdn_close_oldest(સંરચના cshutdn *cshutdn,
                               સંરચના Curl_easy *admin,
                               અચલ અક્ષર *destination,
                               size_t ગણતરી)
{
  જો(cshutdn) {
    પાછા cshutdn_destroy_oldest(cshutdn, admin, destination, ગણતરી);
  }
  પાછા 0;
}

#define અંક_POLLS_ON_ઢગલો 10

સ્થિર CURLcode cshutdn_wait(સંરચના cshutdn *cshutdn,
                             સંરચના Curl_easy *admin,
                             પૂર્ણાંક timeout_ms)
{
  સંરચના pollfd a_few_on_ઢગલો[અંક_POLLS_ON_ઢગલો];
  સંરચના curl_pollfds cpfds;
  CURLcode result;

  Curl_pollfds_init(&cpfds, a_few_on_ઢગલો, અંક_POLLS_ON_ઢગલો);

  result = Curl_cshutdn_add_pollfds(cshutdn, admin, &cpfds);
  જો(result)
    જાઓ out;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

out:
  Curl_pollfds_cleanup(&cpfds);
  પાછા result;
}

સ્થિર શૂન્ય cshutdn_perform(સંરચના cshutdn *cshutdn,
                            સંરચના Curl_easy *admin,
                            સંરચના Curl_sigpipe_ctx *sigpipe_ctx)
{
  સંરચના connectdata *conn;
  timediff_t next_expire_ms = 0, ms;
  uint32_t i = 0;
  bool done;

  જો(!CURL_PTRARRAY_ગણતરી(&cshutdn->conns))
    પાછા;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_ગણતરી(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  જ્યારે(i < CURL_PTRARRAY_ગણતરી(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &done);
    જો(done) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    નહીંતર {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      જો(ms && (!next_expire_ms || (ms < next_expire_ms)))
        next_expire_ms = ms;
    }
  }

  જો(next_expire_ms)
    Curl_expire(admin, next_expire_ms, EXPIRE_SHUTDOWN);
}

સ્થિર શૂન્ય cshutdn_terminate_all(સંરચના cshutdn *cshutdn,
                                  સંરચના Curl_easy *admin,
                                  પૂર્ણાંક timeout_ms)
{
  સંરચના curltime started = *Curl_pgrs_now(admin);
  સંરચના Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  જ્યારે(CURL_PTRARRAY_ગણતરી(&cshutdn->conns)) {
    timediff_t spent_ms;
    પૂર્ણાંક remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    જો(!CURL_PTRARRAY_ગણતરી(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      તોડો;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    જો(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      તોડો;
    }

    remain_ms = timeout_ms - (પૂર્ણાંક)spent_ms;
    જો(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      તોડો;
    }
  }

  /* Terminate any remaining. */
  જ્યારે(CURL_PTRARRAY_ગણતરી(&cshutdn->conns)) {
    સંરચના connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_ગણતરી(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

શૂન્ય Curl_cshutdn_init(સંરચના cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

શૂન્ય Curl_cshutdn_destroy(સંરચના cshutdn *cshutdn,
                          સંરચના Curl_easy *admin)
{
  જો(admin) {
    પૂર્ણાંક timeout_ms = 0;
    /* for testing, run graceful shutdown */
#ifdef DEBUGBUILD
    {
      અચલ અક્ષર *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      જો(p) {
        curl_off_t l;
        જો(!curlx_str_સંખ્યા(&p, &l, INT_MAX))
          timeout_ms = (પૂર્ણાંક)l;
      }
    }
#endif
    જો(CURL_PTRARRAY_ગણતરી(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_ગણતરી(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_ગણતરી(સંરચના cshutdn *cshutdn)
{
  જો(cshutdn) {
    પાછા CURL_PTRARRAY_ગણતરી(&cshutdn->conns);
  }
  પાછા 0;
}

size_t Curl_cshutdn_dest_ગણતરી(સંરચના cshutdn *cshutdn,
                               અચલ અક્ષર *destination)
{
  જો(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    માટે(i = 0; i < CURL_PTRARRAY_ગણતરી(&cshutdn->conns); ++i) {
      સંરચના connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      જો(!strcmp(destination, conn->destination))
        ++n;
    }
    પાછા n;
  }
  પાછા 0;
}

સ્થિર CURLMcode cshutdn_update_ev(સંરચના Curl_multi *multi,
                                   સંરચના connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  પાછા mresult;
}

શૂન્ય Curl_cshutdn_add(સંરચના cshutdn *cshutdn,
                      સંરચના Curl_multi *multi,
                      સંરચના connectdata *conn,
                      size_t max_shutdowns)
{
  સંરચના Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  જો(max_shutdowns <= CURL_PTRARRAY_ગણતરી(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, ખાલી, 1);
  }

  જો(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    પાછા;
  }

  જો(Curl_ptrarray_add(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    પાછા;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_ગણતરી(&cshutdn->conns));
}

શૂન્ય Curl_cshutdn_perform(સંરચના cshutdn *cshutdn,
                          સંરચના Curl_easy *admin,
                          સંરચના Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
શૂન્ય Curl_cshutdn_setfds(સંરચના cshutdn *cshutdn,
                         સંરચના Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         પૂર્ણાંક *maxfd)
{
  જો(CURL_PTRARRAY_ગણતરી(&cshutdn->conns)) {
    સંરચના easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    માટે(i = 0; i < CURL_PTRARRAY_ગણતરી(&cshutdn->conns); ++i) {
      unsigned પૂર્ણાંક j;
      સંરચના connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode result;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      જો(result)
        ચાલુ;

      માટે(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        જો(!FDSET_SOCK(sock))
          ચાલુ;
        જો(ps.actions[j] & CURL_POLL_IN)
          FD_SET(sock, read_fd_set);
        જો(ps.actions[j] & CURL_POLL_OUT)
          FD_SET(sock, write_fd_set);
        જો((ps.actions[j] & (CURL_POLL_OUT | CURL_POLL_IN)) &&
           ((પૂર્ણાંક)sock > *maxfd))
          *maxfd = (પૂર્ણાંક)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
unsigned પૂર્ણાંક Curl_cshutdn_add_waitfds(સંરચના cshutdn *cshutdn,
                                      સંરચના Curl_easy *admin,
                                      સંરચના Curl_waitfds *cwfds)
{
  unsigned પૂર્ણાંક need = 0;

  જો(CURL_PTRARRAY_ગણતરી(&cshutdn->conns)) {
    સંરચના easy_pollset ps;
    સંરચના connectdata *conn;
    uint32_t i;
    CURLcode result;

    Curl_pollset_init(&ps);
    માટે(i = 0; i < CURL_PTRARRAY_ગણતરી(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      જો(!result)
        need += Curl_waitfds_add_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  પાછા need;
}

CURLcode Curl_cshutdn_add_pollfds(સંરચના cshutdn *cshutdn,
                                  સંરચના Curl_easy *admin,
                                  સંરચના curl_pollfds *cpfds)
{
  CURLcode result = CURLE_OK;

  જો(CURL_PTRARRAY_ગણતરી(&cshutdn->conns)) {
    સંરચના easy_pollset ps;
    સંરચના connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    માટે(i = 0; i < CURL_PTRARRAY_ગણતરી(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      જો(!result)
        result = Curl_pollfds_add_ps(cpfds, &ps);
      જો(result) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        જાઓ out;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
out:
  પાછા result;
}
