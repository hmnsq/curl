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


#परिभाषय DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

शून्य Curl_cshutdn_start_timer(संरचना Curl_easy *दत्त, int8_t sockindex,
                              पूर्णाङ्क timeout_ms)
{
  संरचना connectdata *conn = दत्त->conn;
  ध्रुव संरचना curltime *pnow = Curl_pgrs_now(दत्त);

  DEBUGASSERT(conn);
  conn->shutdown.start_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((दत्त->समुच्चयः.shutdowntimeout > 0) ?
     दत्त->समुच्चयः.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  यदि(दत्त->mid)
    Curl_expire_समुच्चयः(दत्त, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(दत्त, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(संरचना Curl_easy *दत्त,
                                    संरचना connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t वामः_ms;

  यदि(conn->shutdown.start_ms[sockindex] < 0)
    प्रत्यागम 0; /* not started or no limits */

  वामः_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &conn->created) -
             conn->shutdown.start_ms[sockindex]);
  प्रत्यागम वामः_ms ? वामः_ms : -1;
}

स्थिर timediff_t cshutdn_conn_timeleft(संरचना Curl_easy *दत्त,
                                        संरचना connectdata *conn)
{
  timediff_t वामः_ms = 0, ms;
  int8_t i;

  प्रति(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    यदि(conn->shutdown.start_ms[i] < 0)
      अनुवर्त;
    ms = Curl_cshutdn_timeleft_ms(दत्त, conn, i);
    यदि(ms && (!वामः_ms || ms < वामः_ms))
      वामः_ms = ms;
  }
  प्रत्यागम वामः_ms;
}

शून्य Curl_cshutdn_clear_timer(संरचना Curl_easy *दत्त, int8_t sockindex)
{
  दत्त->conn->shutdown.start_ms[sockindex] = -1;
}

स्थिर शून्य cshutdn_सततम्_conn_handler(संरचना Curl_easy *दत्त,
                                     संरचना connectdata *conn)
{
  यदि(!conn->द्वयङ्काः.shutdown_handler) {

    यदि(conn->scheme && conn->scheme->सततम्->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      यदि(दत्त->state.आन्तरिकम्) {
        दत्त->समुच्चयः.timeout = DEFAULT_SHUTDOWN_TIMEOUT_MS;
        Curl_pgrsTime(दत्त, TIMER_STARTOP);
      }

      /* This is set if protocol-specific cleanups should be made */
      DEBUGF(infof(दत्त, "connection #%" FMT_OFF_T
                   ", shutdown protocol handler (aborted=%d)",
                   conn->connection_id, conn->द्वयङ्काः.aborted));
      /* There are protocol handlers that block on retrieving
       * server responses here (FTP). Set a short timeout. */
      conn->scheme->सततम्->disconnect(दत्त, conn, (bool)conn->द्वयङ्काः.aborted);
    }

    conn->द्वयङ्काः.shutdown_handler = TRUE;
  }
}

CURLcode Curl_cshutdn_try_once_idx(संरचना Curl_easy *दत्त,
                                   int8_t sockindex, bool *समाप्तम्)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(दत्त->conn);

  यदि(!CONN_SOCK_IDX_VALID(sockindex))
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  /* Get the first connected filter that is not shut down already. */
  cf = दत्त->conn->cfilter[sockindex];
  यावत्(cf && (!cf->connected || cf->shutdown))
    cf = cf->अग्र;

  यदि(!cf) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *समाप्तम् = FALSE;
  यदि(!CURL_CONN_अन्तर्_SHUTDOWN(दत्त->conn, sockindex)) {
    Curl_cshutdn_start_timer(दत्त, sockindex, 0);
  }
  अन्यथा {
    timeout_ms = Curl_cshutdn_timeleft_ms(दत्त, दत्त->conn, sockindex);
    यदि(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(दत्त, "shutdown timeout");
      प्रत्यागम CURLE_OPERATION_TIMEDOUT;
    }
  }

  यावत्(cf) {
    यदि(!cf->shutdown) {
      bool cfdone = FALSE;
      परिणाम = cf->cft->do_shutdown(cf, दत्त, &cfdone);
      यदि(परिणाम) {
        CURL_TRC_CF(दत्त, cf, "shut down failed with %d", (पूर्णाङ्क)परिणाम);
        प्रत्यागम परिणाम;
      }
      अन्यथा यदि(!cfdone) {
        CURL_TRC_CF(दत्त, cf, "shut down not done yet");
        प्रत्यागम CURLE_OK;
      }
      CURL_TRC_CF(दत्त, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->अग्र;
  }
  *समाप्तम् = (!परिणाम);
  प्रत्यागम परिणाम;
}

स्थिर शून्य cshutdn_सततम्_once(संरचना Curl_easy *दत्त,
                             संरचना connectdata *conn,
                             bool *समाप्तम्)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(दत्त->conn == conn);

  यदि(!CURL_CONN_अन्तर्_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_start_timer(दत्त, FIRSTSOCKET, 0);
  }

  cshutdn_सततम्_conn_handler(दत्त, conn);

  यदि(conn->द्वयङ्काः.shutdown_filters) {
    *समाप्तम् = TRUE;
    प्रत्यागम;
  }

  यदि(!conn->द्वयङ्काः.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(दत्त, FIRSTSOCKET, &done1);
  अन्यथा {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  यदि(!conn->द्वयङ्काः.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(दत्त, SECONDARYSOCKET, &done2);
  अन्यथा {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *समाप्तम् = (r1 || r2 || (done1 && done2));
  यदि(*समाप्तम्)
    conn->द्वयङ्काः.shutdown_filters = TRUE;
}

शून्य Curl_cshutdn_try_once(संरचना Curl_easy *admin,
                           संरचना connectdata *conn,
                           bool *समाप्तम्)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_सततम्_once(admin, conn, समाप्तम्);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *समाप्तम्);
  Curl_detach_connection(admin);
}

शून्य Curl_cshutdn_terminate(संरचना Curl_easy *admin,
                            संरचना connectdata *conn,
                            bool do_shutdown)
{
  bool समाप्तम्;

  /* there must be a connection to close */
  DEBUGASSERT(conn);
  /* it must be removed from the connection pool */
  DEBUGASSERT(conn->cpid == UINT32_MAX);
  /* the transfer must be detached from the connection */
  DEBUGASSERT(admin && !admin->conn);
  DEBUGASSERT(!admin->mid);

  Curl_attach_connection(admin, conn, FALSE);

  cshutdn_सततम्_conn_handler(admin, conn);
  यदि(do_shutdown) {
    /* Make a last attempt to shutdown handlers and filters, if
     * not done so already. */
    cshutdn_सततम्_once(admin, conn, &समाप्तम्);
  }
  CURL_TRC_M(admin, "[SHUTDOWN] %sclosing connection #%" FMT_OFF_T,
             conn->द्वयङ्काः.shutdown_filters ? "" : "force ",
             conn->connection_id);
  Curl_conn_cf_discard_all(admin, conn, SECONDARYSOCKET);
  Curl_conn_cf_discard_all(admin, conn, FIRSTSOCKET);
  Curl_detach_connection(admin);

  यदि(admin->multi)
    Curl_multi_ev_conn_समाप्तम्(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  यदि(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

स्थिर size_t cshutdn_destroy_oldest(संरचना cshutdn *cshutdn,
                                     संरचना Curl_easy *admin,
                                     ध्रुव वर्ण *destination,
                                     size_t गणनम्)
{
  संरचना Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  यावत्((removed < गणनम्) && (i < CURL_PTRARRAY_गणनम्(&cshutdn->conns))) {
    संरचना connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    यदि(!destination || !वाक्यं_तुलय(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    अन्यथा
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  प्रत्यागम removed;
}

size_t Curl_cshutdn_close_oldest(संरचना cshutdn *cshutdn,
                               संरचना Curl_easy *admin,
                               ध्रुव वर्ण *destination,
                               size_t गणनम्)
{
  यदि(cshutdn) {
    प्रत्यागम cshutdn_destroy_oldest(cshutdn, admin, destination, गणनम्);
  }
  प्रत्यागम 0;
}

#परिभाषय अङ्कः_POLLS_ON_राशिः 10

स्थिर CURLcode cshutdn_wait(संरचना cshutdn *cshutdn,
                             संरचना Curl_easy *admin,
                             पूर्णाङ्क timeout_ms)
{
  संरचना pollfd a_few_on_राशिः[अङ्कः_POLLS_ON_राशिः];
  संरचना curl_pollfds cpfds;
  CURLcode परिणाम;

  Curl_pollfds_init(&cpfds, a_few_on_राशिः, अङ्कः_POLLS_ON_राशिः);

  परिणाम = Curl_cshutdn_सङ्कलय_pollfds(cshutdn, admin, &cpfds);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

बहिर्गमनम्:
  Curl_pollfds_cleanup(&cpfds);
  प्रत्यागम परिणाम;
}

स्थिर शून्य cshutdn_perform(संरचना cshutdn *cshutdn,
                            संरचना Curl_easy *admin,
                            संरचना Curl_sigpipe_ctx *sigpipe_ctx)
{
  संरचना connectdata *conn;
  timediff_t अग्र_expire_ms = 0, ms;
  uint32_t i = 0;
  bool समाप्तम्;

  यदि(!CURL_PTRARRAY_गणनम्(&cshutdn->conns))
    प्रत्यागम;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_गणनम्(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  यावत्(i < CURL_PTRARRAY_गणनम्(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &समाप्तम्);
    यदि(समाप्तम्) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    अन्यथा {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      यदि(ms && (!अग्र_expire_ms || (ms < अग्र_expire_ms)))
        अग्र_expire_ms = ms;
    }
  }

  यदि(अग्र_expire_ms)
    Curl_expire(admin, अग्र_expire_ms, EXPIRE_SHUTDOWN);
}

स्थिर शून्य cshutdn_terminate_all(संरचना cshutdn *cshutdn,
                                  संरचना Curl_easy *admin,
                                  पूर्णाङ्क timeout_ms)
{
  संरचना curltime started = *Curl_pgrs_now(admin);
  संरचना Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  यावत्(CURL_PTRARRAY_गणनम्(&cshutdn->conns)) {
    timediff_t spent_ms;
    पूर्णाङ्क remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    यदि(!CURL_PTRARRAY_गणनम्(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      भङ्ग;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    यदि(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      भङ्ग;
    }

    remain_ms = timeout_ms - (पूर्णाङ्क)spent_ms;
    यदि(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      भङ्ग;
    }
  }

  /* Terminate any remaining. */
  यावत्(CURL_PTRARRAY_गणनम्(&cshutdn->conns)) {
    संरचना connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_गणनम्(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

शून्य Curl_cshutdn_init(संरचना cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

शून्य Curl_cshutdn_destroy(संरचना cshutdn *cshutdn,
                          संरचना Curl_easy *admin)
{
  यदि(admin) {
    पूर्णाङ्क timeout_ms = 0;
    /* for testing, run graceful shutdown */
#यदि_परिभाषितम् DEBUGBUILD
    {
      ध्रुव वर्ण *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      यदि(p) {
        curl_off_t l;
        यदि(!curlx_str_सङ्ख्या(&p, &l, अधिकतम_पूर्णाङ्कः))
          timeout_ms = (पूर्णाङ्क)l;
      }
    }
#यदि_अन्तः
    यदि(CURL_PTRARRAY_गणनम्(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_गणनम्(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_गणनम्(संरचना cshutdn *cshutdn)
{
  यदि(cshutdn) {
    प्रत्यागम CURL_PTRARRAY_गणनम्(&cshutdn->conns);
  }
  प्रत्यागम 0;
}

size_t Curl_cshutdn_dest_गणनम्(संरचना cshutdn *cshutdn,
                               ध्रुव वर्ण *destination)
{
  यदि(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    प्रति(i = 0; i < CURL_PTRARRAY_गणनम्(&cshutdn->conns); ++i) {
      संरचना connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      यदि(!वाक्यं_तुलय(destination, conn->destination))
        ++n;
    }
    प्रत्यागम n;
  }
  प्रत्यागम 0;
}

स्थिर CURLMcode cshutdn_update_ev(संरचना Curl_multi *multi,
                                   संरचना connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  प्रत्यागम mresult;
}

शून्य Curl_cshutdn_सङ्कलय(संरचना cshutdn *cshutdn,
                      संरचना Curl_multi *multi,
                      संरचना connectdata *conn,
                      size_t max_shutdowns)
{
  संरचना Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  यदि(max_shutdowns <= CURL_PTRARRAY_गणनम्(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, रिक्त, 1);
  }

  यदि(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    प्रत्यागम;
  }

  यदि(Curl_ptrarray_सङ्कलय(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    प्रत्यागम;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_गणनम्(&cshutdn->conns));
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
                         fd_set *read_fd_समुच्चयः, fd_set *write_fd_समुच्चयः,
                         पूर्णाङ्क *maxfd)
{
  यदि(CURL_PTRARRAY_गणनम्(&cshutdn->conns)) {
    संरचना easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    प्रति(i = 0; i < CURL_PTRARRAY_गणनम्(&cshutdn->conns); ++i) {
      अचिह्नित पूर्णाङ्क j;
      संरचना connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode परिणाम;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      परिणाम = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      यदि(परिणाम)
        अनुवर्त;

      प्रति(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        यदि(!FDSET_SOCK(sock))
          अनुवर्त;
        यदि(ps.actions[j] & CURL_POLL_अन्तर्)
          FD_SET(sock, read_fd_समुच्चयः);
        यदि(ps.actions[j] & CURL_POLL_बहिर्गमनम्)
          FD_SET(sock, write_fd_समुच्चयः);
        यदि((ps.actions[j] & (CURL_POLL_बहिर्गमनम् | CURL_POLL_अन्तर्)) &&
           ((पूर्णाङ्क)sock > *maxfd))
          *maxfd = (पूर्णाङ्क)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
अचिह्नित पूर्णाङ्क Curl_cshutdn_सङ्कलय_waitfds(संरचना cshutdn *cshutdn,
                                      संरचना Curl_easy *admin,
                                      संरचना Curl_waitfds *cwfds)
{
  अचिह्नित पूर्णाङ्क need = 0;

  यदि(CURL_PTRARRAY_गणनम्(&cshutdn->conns)) {
    संरचना easy_pollset ps;
    संरचना connectdata *conn;
    uint32_t i;
    CURLcode परिणाम;

    Curl_pollset_init(&ps);
    प्रति(i = 0; i < CURL_PTRARRAY_गणनम्(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      परिणाम = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      यदि(!परिणाम)
        need += Curl_waitfds_सङ्कलय_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  प्रत्यागम need;
}

CURLcode Curl_cshutdn_सङ्कलय_pollfds(संरचना cshutdn *cshutdn,
                                  संरचना Curl_easy *admin,
                                  संरचना curl_pollfds *cpfds)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(CURL_PTRARRAY_गणनम्(&cshutdn->conns)) {
    संरचना easy_pollset ps;
    संरचना connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    प्रति(i = 0; i < CURL_PTRARRAY_गणनम्(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      परिणाम = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      यदि(!परिणाम)
        परिणाम = Curl_pollfds_सङ्कलय_ps(cpfds, &ps);
      यदि(परिणाम) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        गच्छ बहिर्गमनम्;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}
