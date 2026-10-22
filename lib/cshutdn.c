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


#تعریف_کرو DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

خالی Curl_cshutdn_آغاز_timer(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                              صحیح timeout_ms)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ثابت ساخت curltime *pnow = Curl_pgrs_now(ڈیٹا);

  DEBUGASSERT(conn);
  conn->shutdown.آغاز_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((ڈیٹا->سیٹ.shutdowntimeout > 0) ?
     ڈیٹا->سیٹ.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  اگر(ڈیٹا->mid)
    Curl_expire_سیٹ(ڈیٹا, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(ڈیٹا, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(ساخت Curl_easy *ڈیٹا,
                                    ساخت connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t بایاں_ms;

  اگر(conn->shutdown.آغاز_ms[sockindex] < 0)
    واپس 0; /* not started or no limits */

  بایاں_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &conn->created) -
             conn->shutdown.آغاز_ms[sockindex]);
  واپس بایاں_ms ? بایاں_ms : -1;
}

ساکن timediff_t cshutdn_conn_timeleft(ساخت Curl_easy *ڈیٹا,
                                        ساخت connectdata *conn)
{
  timediff_t بایاں_ms = 0, ms;
  int8_t i;

  برائے(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    اگر(conn->shutdown.آغاز_ms[i] < 0)
      جاری;
    ms = Curl_cshutdn_timeleft_ms(ڈیٹا, conn, i);
    اگر(ms && (!بایاں_ms || ms < بایاں_ms))
      بایاں_ms = ms;
  }
  واپس بایاں_ms;
}

خالی Curl_cshutdn_clear_timer(ساخت Curl_easy *ڈیٹا, int8_t sockindex)
{
  ڈیٹا->conn->shutdown.آغاز_ms[sockindex] = -1;
}

ساکن خالی cshutdn_تسلسل_conn_handler(ساخت Curl_easy *ڈیٹا,
                                     ساخت connectdata *conn)
{
  اگر(!conn->بٹ.shutdown_handler) {

    اگر(conn->scheme && conn->scheme->تسلسل->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      اگر(ڈیٹا->state.اندرونی_2) {
        ڈیٹا->سیٹ.timeout = DEFAULT_SHUTDOWN_TIMEOUT_MS;
        Curl_pgrs_وقت(ڈیٹا, TIMER_STARTOP);
      }

      /* This is set if protocol-specific cleanups should be made */
      DEBUGF(infof(ڈیٹا, "connection #%" FMT_OFF_T
                   ", shutdown protocol handler (aborted=%d)",
                   conn->connection_id, conn->بٹ.aborted));
      /* There are protocol handlers that block on retrieving
       * server responses here (FTP). Set a short timeout. */
      conn->scheme->تسلسل->disconnect(ڈیٹا, conn, (bool)conn->بٹ.aborted);
    }

    conn->بٹ.shutdown_handler = TRUE;
  }
}

CURLcode Curl_cshutdn_try_once_idx(ساخت Curl_easy *ڈیٹا,
                                   int8_t sockindex, bool *مکمل)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(ڈیٹا->conn);

  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس CURLE_BAD_FUNCTION_دلیل;

  /* Get the first connected filter that is not shut down already. */
  cf = ڈیٹا->conn->cfilter[sockindex];
  جب(cf && (!cf->connected || cf->shutdown))
    cf = cf->اگلا;

  اگر(!cf) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  *مکمل = FALSE;
  اگر(!CURL_CONN_اندر_SHUTDOWN(ڈیٹا->conn, sockindex)) {
    Curl_cshutdn_آغاز_timer(ڈیٹا, sockindex, 0);
  }
  ورنہ {
    timeout_ms = Curl_cshutdn_timeleft_ms(ڈیٹا, ڈیٹا->conn, sockindex);
    اگر(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(ڈیٹا, "shutdown timeout");
      واپس CURLE_OPERATION_TIMEDOUT;
    }
  }

  جب(cf) {
    اگر(!cf->shutdown) {
      bool cfdone = FALSE;
      نتیجہ = cf->cft->do_shutdown(cf, ڈیٹا, &cfdone);
      اگر(نتیجہ) {
        CURL_TRC_CF(ڈیٹا, cf, "shut down failed with %d", (صحیح)نتیجہ);
        واپس نتیجہ;
      }
      ورنہ اگر(!cfdone) {
        CURL_TRC_CF(ڈیٹا, cf, "shut down not done yet");
        واپس CURLE_OK;
      }
      CURL_TRC_CF(ڈیٹا, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->اگلا;
  }
  *مکمل = (!نتیجہ);
  واپس نتیجہ;
}

ساکن خالی cshutdn_تسلسل_once(ساخت Curl_easy *ڈیٹا,
                             ساخت connectdata *conn,
                             bool *مکمل)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(ڈیٹا->conn == conn);

  اگر(!CURL_CONN_اندر_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_آغاز_timer(ڈیٹا, FIRSTSOCKET, 0);
  }

  cshutdn_تسلسل_conn_handler(ڈیٹا, conn);

  اگر(conn->بٹ.shutdown_filters) {
    *مکمل = TRUE;
    واپس;
  }

  اگر(!conn->بٹ.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(ڈیٹا, FIRSTSOCKET, &done1);
  ورنہ {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  اگر(!conn->بٹ.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(ڈیٹا, SECONDARYSOCKET, &done2);
  ورنہ {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *مکمل = (r1 || r2 || (done1 && done2));
  اگر(*مکمل)
    conn->بٹ.shutdown_filters = TRUE;
}

خالی Curl_cshutdn_try_once(ساخت Curl_easy *admin,
                           ساخت connectdata *conn,
                           bool *مکمل)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_تسلسل_once(admin, conn, مکمل);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *مکمل);
  Curl_detach_connection(admin);
}

خالی Curl_cshutdn_terminate(ساخت Curl_easy *admin,
                            ساخت connectdata *conn,
                            bool do_shutdown)
{
  bool مکمل;

  /* there must be a connection to close */
  DEBUGASSERT(conn);
  /* it must be removed from the connection pool */
  DEBUGASSERT(conn->cpid == UINT32_MAX);
  /* the transfer must be detached from the connection */
  DEBUGASSERT(admin && !admin->conn);
  DEBUGASSERT(!admin->mid);

  Curl_attach_connection(admin, conn, FALSE);

  cshutdn_تسلسل_conn_handler(admin, conn);
  اگر(do_shutdown) {
    /* Make a last attempt to shutdown handlers and filters, if
     * not done so already. */
    cshutdn_تسلسل_once(admin, conn, &مکمل);
  }
  CURL_TRC_M(admin, "[SHUTDOWN] %sclosing connection #%" FMT_OFF_T,
             conn->بٹ.shutdown_filters ? "" : "force ",
             conn->connection_id);
  Curl_conn_cf_discard_all(admin, conn, SECONDARYSOCKET);
  Curl_conn_cf_discard_all(admin, conn, FIRSTSOCKET);
  Curl_detach_connection(admin);

  اگر(admin->multi)
    Curl_multi_ev_conn_مکمل(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  اگر(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

ساکن size_t cshutdn_destroy_oldest(ساخت cshutdn *cshutdn,
                                     ساخت Curl_easy *admin,
                                     ثابت حرف *destination,
                                     size_t گنتی)
{
  ساخت Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  جب((removed < گنتی) && (i < CURL_PTRARRAY_گنتی(&cshutdn->conns))) {
    ساخت connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    اگر(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    ورنہ
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  واپس removed;
}

size_t Curl_cshutdn_close_oldest(ساخت cshutdn *cshutdn,
                               ساخت Curl_easy *admin,
                               ثابت حرف *destination,
                               size_t گنتی)
{
  اگر(cshutdn) {
    واپس cshutdn_destroy_oldest(cshutdn, admin, destination, گنتی);
  }
  واپس 0;
}

#تعریف_کرو عدد_POLLS_ON_ڈھیر 10

ساکن CURLcode cshutdn_wait(ساخت cshutdn *cshutdn,
                             ساخت Curl_easy *admin,
                             صحیح timeout_ms)
{
  ساخت pollfd a_few_on_ڈھیر[عدد_POLLS_ON_ڈھیر];
  ساخت curl_pollfds cpfds;
  CURLcode نتیجہ;

  Curl_pollfds_init(&cpfds, a_few_on_ڈھیر, عدد_POLLS_ON_ڈھیر);

  نتیجہ = Curl_cshutdn_جوڑو_pollfds(cshutdn, admin, &cpfds);
  اگر(نتیجہ)
    جاؤ برآمد;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

برآمد:
  Curl_pollfds_cleanup(&cpfds);
  واپس نتیجہ;
}

ساکن خالی cshutdn_perform(ساخت cshutdn *cshutdn,
                            ساخت Curl_easy *admin,
                            ساخت Curl_sigpipe_ctx *sigpipe_ctx)
{
  ساخت connectdata *conn;
  timediff_t اگلا_expire_ms = 0, ms;
  uint32_t i = 0;
  bool مکمل;

  اگر(!CURL_PTRARRAY_گنتی(&cshutdn->conns))
    واپس;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_گنتی(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  جب(i < CURL_PTRARRAY_گنتی(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &مکمل);
    اگر(مکمل) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    ورنہ {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      اگر(ms && (!اگلا_expire_ms || (ms < اگلا_expire_ms)))
        اگلا_expire_ms = ms;
    }
  }

  اگر(اگلا_expire_ms)
    Curl_expire(admin, اگلا_expire_ms, EXPIRE_SHUTDOWN);
}

ساکن خالی cshutdn_terminate_all(ساخت cshutdn *cshutdn,
                                  ساخت Curl_easy *admin,
                                  صحیح timeout_ms)
{
  ساخت curltime started = *Curl_pgrs_now(admin);
  ساخت Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  جب(CURL_PTRARRAY_گنتی(&cshutdn->conns)) {
    timediff_t spent_ms;
    صحیح remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    اگر(!CURL_PTRARRAY_گنتی(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      توڑو;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    اگر(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      توڑو;
    }

    remain_ms = timeout_ms - (صحیح)spent_ms;
    اگر(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      توڑو;
    }
  }

  /* Terminate any remaining. */
  جب(CURL_PTRARRAY_گنتی(&cshutdn->conns)) {
    ساخت connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_گنتی(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

خالی Curl_cshutdn_init(ساخت cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

خالی Curl_cshutdn_destroy(ساخت cshutdn *cshutdn,
                          ساخت Curl_easy *admin)
{
  اگر(admin) {
    صحیح timeout_ms = 0;
    /* for testing, run graceful shutdown */
#اگر_تعریف_شدہ DEBUGBUILD
    {
      ثابت حرف *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      اگر(p) {
        curl_off_t l;
        اگر(!curlx_str_نمبر(&p, &l, زیادہ_سے_زیادہ_عدد))
          timeout_ms = (صحیح)l;
      }
    }
#اگر_ختم
    اگر(CURL_PTRARRAY_گنتی(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_گنتی(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_گنتی(ساخت cshutdn *cshutdn)
{
  اگر(cshutdn) {
    واپس CURL_PTRARRAY_گنتی(&cshutdn->conns);
  }
  واپس 0;
}

size_t Curl_cshutdn_dest_گنتی(ساخت cshutdn *cshutdn,
                               ثابت حرف *destination)
{
  اگر(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    برائے(i = 0; i < CURL_PTRARRAY_گنتی(&cshutdn->conns); ++i) {
      ساخت connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      اگر(!strcmp(destination, conn->destination))
        ++n;
    }
    واپس n;
  }
  واپس 0;
}

ساکن CURLMcode cshutdn_update_ev(ساخت Curl_multi *multi,
                                   ساخت connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  واپس mresult;
}

خالی Curl_cshutdn_جوڑو(ساخت cshutdn *cshutdn,
                      ساخت Curl_multi *multi,
                      ساخت connectdata *conn,
                      size_t max_shutdowns)
{
  ساخت Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  اگر(max_shutdowns <= CURL_PTRARRAY_گنتی(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, تہی, 1);
  }

  اگر(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    واپس;
  }

  اگر(Curl_ptrarray_جوڑو(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    واپس;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_گنتی(&cshutdn->conns));
}

خالی Curl_cshutdn_perform(ساخت cshutdn *cshutdn,
                          ساخت Curl_easy *admin,
                          ساخت Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
خالی Curl_cshutdn_setfds(ساخت cshutdn *cshutdn,
                         ساخت Curl_easy *admin,
                         fd_set *read_fd_سیٹ, fd_set *write_fd_سیٹ,
                         صحیح *maxfd)
{
  اگر(CURL_PTRARRAY_گنتی(&cshutdn->conns)) {
    ساخت easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    برائے(i = 0; i < CURL_PTRARRAY_گنتی(&cshutdn->conns); ++i) {
      غیرعلامتی صحیح j;
      ساخت connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode نتیجہ;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      نتیجہ = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      اگر(نتیجہ)
        جاری;

      برائے(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        اگر(!FDSET_SOCK(sock))
          جاری;
        اگر(ps.actions[j] & CURL_POLL_اندر)
          FD_SET(sock, read_fd_سیٹ);
        اگر(ps.actions[j] & CURL_POLL_برآمد)
          FD_SET(sock, write_fd_سیٹ);
        اگر((ps.actions[j] & (CURL_POLL_برآمد | CURL_POLL_اندر)) &&
           ((صحیح)sock > *maxfd))
          *maxfd = (صحیح)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
غیرعلامتی صحیح Curl_cshutdn_جوڑو_waitfds(ساخت cshutdn *cshutdn,
                                      ساخت Curl_easy *admin,
                                      ساخت Curl_waitfds *cwfds)
{
  غیرعلامتی صحیح need = 0;

  اگر(CURL_PTRARRAY_گنتی(&cshutdn->conns)) {
    ساخت easy_pollset ps;
    ساخت connectdata *conn;
    uint32_t i;
    CURLcode نتیجہ;

    Curl_pollset_init(&ps);
    برائے(i = 0; i < CURL_PTRARRAY_گنتی(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      نتیجہ = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      اگر(!نتیجہ)
        need += Curl_waitfds_جوڑو_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  واپس need;
}

CURLcode Curl_cshutdn_جوڑو_pollfds(ساخت cshutdn *cshutdn,
                                  ساخت Curl_easy *admin,
                                  ساخت curl_pollfds *cpfds)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(CURL_PTRARRAY_گنتی(&cshutdn->conns)) {
    ساخت easy_pollset ps;
    ساخت connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    برائے(i = 0; i < CURL_PTRARRAY_گنتی(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      نتیجہ = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      اگر(!نتیجہ)
        نتیجہ = Curl_pollfds_جوڑو_ps(cpfds, &ps);
      اگر(نتیجہ) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        جاؤ برآمد;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
برآمد:
  واپس نتیجہ;
}
