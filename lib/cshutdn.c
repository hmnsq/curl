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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "url.h"
#ضمن "cfilters.h"
#ضمن "progress.h"
#ضمن "multiif.h"
#ضمن "multi_ev.h"
#ضمن "curl_trc.h"
#ضمن "cshutdn.h"
#ضمن "sigpipe.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "curlx/strparse.h"


#تعریف_کن DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

فراغ Curl_cshutdn_شروع_timer(بنية Curl_easy *داده, int8_t sockindex,
                              صحيح timeout_ms)
{
  بنية connectdata *conn = داده->conn;
  ثابت بنية curltime *pnow = Curl_pgrs_now(داده);

  DEBUGASSERT(conn);
  conn->shutdown.شروع_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((داده->مجموعه.shutdowntimeout > 0) ?
     داده->مجموعه.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  إذا(داده->mid)
    Curl_expire_مجموعه(داده, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(داده, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(بنية Curl_easy *داده,
                                    بنية connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t چپ_ms;

  إذا(conn->shutdown.شروع_ms[sockindex] < 0)
    أرجع 0; /* not started or no limits */

  چپ_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(داده), &conn->created) -
             conn->shutdown.شروع_ms[sockindex]);
  أرجع چپ_ms ? چپ_ms : -1;
}

ساكن timediff_t cshutdn_conn_timeleft(بنية Curl_easy *داده,
                                        بنية connectdata *conn)
{
  timediff_t چپ_ms = 0, ms;
  int8_t i;

  لكل(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    إذا(conn->shutdown.شروع_ms[i] < 0)
      استمر;
    ms = Curl_cshutdn_timeleft_ms(داده, conn, i);
    إذا(ms && (!چپ_ms || ms < چپ_ms))
      چپ_ms = ms;
  }
  أرجع چپ_ms;
}

فراغ Curl_cshutdn_clear_timer(بنية Curl_easy *داده, int8_t sockindex)
{
  داده->conn->shutdown.شروع_ms[sockindex] = -1;
}

ساكن فراغ cshutdn_پیاپی_conn_handler(بنية Curl_easy *داده,
                                     بنية connectdata *conn)
{
  إذا(!conn->بیتها.shutdown_handler) {

    إذا(conn->scheme && conn->scheme->پیاپی->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      إذا(داده->state.داخلی) {
        داده->مجموعه.timeout = DEFAULT_SHUTDOWN_TIMEOUT_MS;
        Curl_pgrs_زمان(داده, TIMER_STARTOP);
      }

      /* This is set if protocol-specific cleanups should be made */
      DEBUGF(infof(داده, "connection #%" FMT_OFF_T
                   ", shutdown protocol handler (aborted=%d)",
                   conn->connection_id, conn->بیتها.aborted));
      /* There are protocol handlers that block on retrieving
       * server responses here (FTP). Set a short timeout. */
      conn->scheme->پیاپی->disconnect(داده, conn, (bool)conn->بیتها.aborted);
    }

    conn->بیتها.shutdown_handler = TRUE;
  }
}

CURLcode Curl_cshutdn_try_once_idx(بنية Curl_easy *داده,
                                   int8_t sockindex, bool *انجامشده)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجه = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(داده->conn);

  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  /* Get the first connected filter that is not shut down already. */
  cf = داده->conn->cfilter[sockindex];
  طالما(cf && (!cf->connected || cf->shutdown))
    cf = cf->بعدی;

  إذا(!cf) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  *انجامشده = FALSE;
  إذا(!CURL_CONN_درون_SHUTDOWN(داده->conn, sockindex)) {
    Curl_cshutdn_شروع_timer(داده, sockindex, 0);
  }
  وإلا {
    timeout_ms = Curl_cshutdn_timeleft_ms(داده, داده->conn, sockindex);
    إذا(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(داده, "shutdown timeout");
      أرجع CURLE_OPERATION_TIMEDOUT;
    }
  }

  طالما(cf) {
    إذا(!cf->shutdown) {
      bool cfdone = FALSE;
      نتیجه = cf->cft->do_shutdown(cf, داده, &cfdone);
      إذا(نتیجه) {
        CURL_TRC_CF(داده, cf, "shut down failed with %d", (صحيح)نتیجه);
        أرجع نتیجه;
      }
      وإلا إذا(!cfdone) {
        CURL_TRC_CF(داده, cf, "shut down not done yet");
        أرجع CURLE_OK;
      }
      CURL_TRC_CF(داده, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->بعدی;
  }
  *انجامشده = (!نتیجه);
  أرجع نتیجه;
}

ساكن فراغ cshutdn_پیاپی_once(بنية Curl_easy *داده,
                             بنية connectdata *conn,
                             bool *انجامشده)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(داده->conn == conn);

  إذا(!CURL_CONN_درون_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_شروع_timer(داده, FIRSTSOCKET, 0);
  }

  cshutdn_پیاپی_conn_handler(داده, conn);

  إذا(conn->بیتها.shutdown_filters) {
    *انجامشده = TRUE;
    أرجع;
  }

  إذا(!conn->بیتها.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(داده, FIRSTSOCKET, &done1);
  وإلا {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  إذا(!conn->بیتها.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(داده, SECONDARYSOCKET, &done2);
  وإلا {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *انجامشده = (r1 || r2 || (done1 && done2));
  إذا(*انجامشده)
    conn->بیتها.shutdown_filters = TRUE;
}

فراغ Curl_cshutdn_try_once(بنية Curl_easy *admin,
                           بنية connectdata *conn,
                           bool *انجامشده)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_پیاپی_once(admin, conn, انجامشده);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *انجامشده);
  Curl_detach_connection(admin);
}

فراغ Curl_cshutdn_terminate(بنية Curl_easy *admin,
                            بنية connectdata *conn,
                            bool do_shutdown)
{
  bool انجامشده;

  /* there must be a connection to close */
  DEBUGASSERT(conn);
  /* it must be removed from the connection pool */
  DEBUGASSERT(conn->cpid == UINT32_MAX);
  /* the transfer must be detached from the connection */
  DEBUGASSERT(admin && !admin->conn);
  DEBUGASSERT(!admin->mid);

  Curl_attach_connection(admin, conn, FALSE);

  cshutdn_پیاپی_conn_handler(admin, conn);
  إذا(do_shutdown) {
    /* Make a last attempt to shutdown handlers and filters, if
     * not done so already. */
    cshutdn_پیاپی_once(admin, conn, &انجامشده);
  }
  CURL_TRC_M(admin, "[SHUTDOWN] %sclosing connection #%" FMT_OFF_T,
             conn->بیتها.shutdown_filters ? "" : "force ",
             conn->connection_id);
  Curl_conn_cf_discard_all(admin, conn, SECONDARYSOCKET);
  Curl_conn_cf_discard_all(admin, conn, FIRSTSOCKET);
  Curl_detach_connection(admin);

  إذا(admin->multi)
    Curl_multi_ev_conn_انجامشده(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  إذا(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

ساكن size_t cshutdn_destroy_oldest(بنية cshutdn *cshutdn,
                                     بنية Curl_easy *admin,
                                     ثابت حرف *destination,
                                     size_t شمار)
{
  بنية Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  طالما((removed < شمار) && (i < CURL_PTRARRAY_شمار(&cshutdn->conns))) {
    بنية connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    إذا(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    وإلا
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  أرجع removed;
}

size_t Curl_cshutdn_close_oldest(بنية cshutdn *cshutdn,
                               بنية Curl_easy *admin,
                               ثابت حرف *destination,
                               size_t شمار)
{
  إذا(cshutdn) {
    أرجع cshutdn_destroy_oldest(cshutdn, admin, destination, شمار);
  }
  أرجع 0;
}

#تعریف_کن رقم_POLLS_ON_پشته 10

ساكن CURLcode cshutdn_wait(بنية cshutdn *cshutdn,
                             بنية Curl_easy *admin,
                             صحيح timeout_ms)
{
  بنية pollfd a_few_on_پشته[رقم_POLLS_ON_پشته];
  بنية curl_pollfds cpfds;
  CURLcode نتیجه;

  Curl_pollfds_init(&cpfds, a_few_on_پشته, رقم_POLLS_ON_پشته);

  نتیجه = Curl_cshutdn_جمع_کن_pollfds(cshutdn, admin, &cpfds);
  إذا(نتیجه)
    اذهب بیرون;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

بیرون:
  Curl_pollfds_cleanup(&cpfds);
  أرجع نتیجه;
}

ساكن فراغ cshutdn_perform(بنية cshutdn *cshutdn,
                            بنية Curl_easy *admin,
                            بنية Curl_sigpipe_ctx *sigpipe_ctx)
{
  بنية connectdata *conn;
  timediff_t بعدی_expire_ms = 0, ms;
  uint32_t i = 0;
  bool انجامشده;

  إذا(!CURL_PTRARRAY_شمار(&cshutdn->conns))
    أرجع;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_شمار(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  طالما(i < CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &انجامشده);
    إذا(انجامشده) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    وإلا {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      إذا(ms && (!بعدی_expire_ms || (ms < بعدی_expire_ms)))
        بعدی_expire_ms = ms;
    }
  }

  إذا(بعدی_expire_ms)
    Curl_expire(admin, بعدی_expire_ms, EXPIRE_SHUTDOWN);
}

ساكن فراغ cshutdn_terminate_all(بنية cshutdn *cshutdn,
                                  بنية Curl_easy *admin,
                                  صحيح timeout_ms)
{
  بنية curltime started = *Curl_pgrs_now(admin);
  بنية Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  طالما(CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    timediff_t spent_ms;
    صحيح remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    إذا(!CURL_PTRARRAY_شمار(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      اكسر;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    إذا(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      اكسر;
    }

    remain_ms = timeout_ms - (صحيح)spent_ms;
    إذا(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      اكسر;
    }
  }

  /* Terminate any remaining. */
  طالما(CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    بنية connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_شمار(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

فراغ Curl_cshutdn_init(بنية cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

فراغ Curl_cshutdn_destroy(بنية cshutdn *cshutdn,
                          بنية Curl_easy *admin)
{
  إذا(admin) {
    صحيح timeout_ms = 0;
    /* for testing, run graceful shutdown */
#اگر_تعریف_شده DEBUGBUILD
    {
      ثابت حرف *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      إذا(p) {
        curl_off_t l;
        إذا(!curlx_str_عدد(&p, &l, بیشترین_صحیح))
          timeout_ms = (صحيح)l;
      }
    }
#پایان_اگر
    إذا(CURL_PTRARRAY_شمار(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_شمار(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_شمار(بنية cshutdn *cshutdn)
{
  إذا(cshutdn) {
    أرجع CURL_PTRARRAY_شمار(&cshutdn->conns);
  }
  أرجع 0;
}

size_t Curl_cshutdn_dest_شمار(بنية cshutdn *cshutdn,
                               ثابت حرف *destination)
{
  إذا(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    لكل(i = 0; i < CURL_PTRARRAY_شمار(&cshutdn->conns); ++i) {
      بنية connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      إذا(!strcmp(destination, conn->destination))
        ++n;
    }
    أرجع n;
  }
  أرجع 0;
}

ساكن CURLMcode cshutdn_update_ev(بنية Curl_multi *multi,
                                   بنية connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  أرجع mresult;
}

فراغ Curl_cshutdn_جمع_کن(بنية cshutdn *cshutdn,
                      بنية Curl_multi *multi,
                      بنية connectdata *conn,
                      size_t max_shutdowns)
{
  بنية Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  إذا(max_shutdowns <= CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, عدم, 1);
  }

  إذا(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    أرجع;
  }

  إذا(Curl_ptrarray_جمع_کن(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    أرجع;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_شمار(&cshutdn->conns));
}

فراغ Curl_cshutdn_perform(بنية cshutdn *cshutdn,
                          بنية Curl_easy *admin,
                          بنية Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
فراغ Curl_cshutdn_setfds(بنية cshutdn *cshutdn,
                         بنية Curl_easy *admin,
                         fd_set *read_fd_مجموعه, fd_set *write_fd_مجموعه,
                         صحيح *maxfd)
{
  إذا(CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    بنية easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    لكل(i = 0; i < CURL_PTRARRAY_شمار(&cshutdn->conns); ++i) {
      بیعلامت صحيح j;
      بنية connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode نتیجه;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      نتیجه = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      إذا(نتیجه)
        استمر;

      لكل(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        إذا(!FDSET_SOCK(sock))
          استمر;
        إذا(ps.actions[j] & CURL_POLL_درون)
          FD_SET(sock, read_fd_مجموعه);
        إذا(ps.actions[j] & CURL_POLL_بیرون)
          FD_SET(sock, write_fd_مجموعه);
        إذا((ps.actions[j] & (CURL_POLL_بیرون | CURL_POLL_درون)) &&
           ((صحيح)sock > *maxfd))
          *maxfd = (صحيح)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
بیعلامت صحيح Curl_cshutdn_جمع_کن_waitfds(بنية cshutdn *cshutdn,
                                      بنية Curl_easy *admin,
                                      بنية Curl_waitfds *cwfds)
{
  بیعلامت صحيح need = 0;

  إذا(CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    بنية easy_pollset ps;
    بنية connectdata *conn;
    uint32_t i;
    CURLcode نتیجه;

    Curl_pollset_init(&ps);
    لكل(i = 0; i < CURL_PTRARRAY_شمار(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      نتیجه = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      إذا(!نتیجه)
        need += Curl_waitfds_جمع_کن_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  أرجع need;
}

CURLcode Curl_cshutdn_جمع_کن_pollfds(بنية cshutdn *cshutdn,
                                  بنية Curl_easy *admin,
                                  بنية curl_pollfds *cpfds)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(CURL_PTRARRAY_شمار(&cshutdn->conns)) {
    بنية easy_pollset ps;
    بنية connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    لكل(i = 0; i < CURL_PTRARRAY_شمار(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      نتیجه = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      إذا(!نتیجه)
        نتیجه = Curl_pollfds_جمع_کن_ps(cpfds, &ps);
      إذا(نتیجه) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        اذهب بیرون;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
بیرون:
  أرجع نتیجه;
}
