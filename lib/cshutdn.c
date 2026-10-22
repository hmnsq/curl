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
#include "curl_setup.h"

#include "urldata.h"
#include "url.h"
#include "cfilters.h"
#include "progress.h"
#include "multiif.h"
#include "multi_ev.h"
#include "curl_trc.h"
#include "cshutdn.h"
#include "sigpipe.h"
#include "connect.h"
#include "select.h"
#include "curlx/strparse.h"


#define DEFAULT_SHUTDOWN_TIMEOUT_MS   (2 * 1000)

void Curl_cshutdn_start_timer(sa_nracanaa Curl_easy *data, int8_t sockindex,
                              ཧྲིལ timeout_ms)
{
  sa_nracanaa connectdata *conn = data->conn;
  const sa_nracanaa curltime *pnow = Curl_pgrs_now(data);

  DEBUGASSERT(conn);
  conn->shutdown.start_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((data->set.shutdowntimeout > 0) ?
     data->set.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  གལ་སྲིད(data->mid)
    Curl_expire_set(data, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(data, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t left_ms;

  གལ་སྲིད(conn->shutdown.start_ms[sockindex] < 0)
    ལོག 0; /* not started or no limits */

  left_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(data), &conn->created) -
             conn->shutdown.start_ms[sockindex]);
  ལོག left_ms ? left_ms : -1;
}

static timediff_t cshutdn_conn_timeleft(sa_nracanaa Curl_easy *data,
                                        sa_nracanaa connectdata *conn)
{
  timediff_t left_ms = 0, ms;
  int8_t i;

  དོན་ལུ(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    གལ་སྲིད(conn->shutdown.start_ms[i] < 0)
      continue;
    ms = Curl_cshutdn_timeleft_ms(data, conn, i);
    གལ་སྲིད(ms && (!left_ms || ms < left_ms))
      left_ms = ms;
  }
  ལོག left_ms;
}

void Curl_cshutdn_clear_timer(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  data->conn->shutdown.start_ms[sockindex] = -1;
}

static void cshutdn_run_conn_handler(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa connectdata *conn)
{
  གལ་སྲིད(!conn->bits.shutdown_handler) {

    གལ་སྲིད(conn->scheme && conn->scheme->run->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      གལ་སྲིད(data->state.internal) {
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

CURLcode Curl_cshutdn_try_once_idx(sa_nracanaa Curl_easy *data,
                                   int8_t sockindex, bool *done)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(data->conn);

  གལ་སྲིད(!CONN_SOCK_IDX_VALID(sockindex))
    ལོག CURLE_BAD_FUNCTION_ARGUMENT;

  /* Get the first connected filter that is not shut down already. */
  cf = data->conn->cfilter[sockindex];
  སྐབས(cf && (!cf->connected || cf->shutdown))
    cf = cf->next;

  གལ་སྲིད(!cf) {
    *done = TRUE;
    ལོག CURLE_OK;
  }

  *done = FALSE;
  གལ་སྲིད(!CURL_CONN_IN_SHUTDOWN(data->conn, sockindex)) {
    Curl_cshutdn_start_timer(data, sockindex, 0);
  }
  དེ་མིན {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, data->conn, sockindex);
    གལ་སྲིད(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(data, "shutdown timeout");
      ལོག CURLE_OPERATION_TIMEDOUT;
    }
  }

  སྐབས(cf) {
    གལ་སྲིད(!cf->shutdown) {
      bool cfdone = FALSE;
      result = cf->cft->do_shutdown(cf, data, &cfdone);
      གལ་སྲིད(result) {
        CURL_TRC_CF(data, cf, "shut down failed with %d", (ཧྲིལ)result);
        ལོག result;
      }
      དེ་མིན གལ་སྲིད(!cfdone) {
        CURL_TRC_CF(data, cf, "shut down not done yet");
        ལོག CURLE_OK;
      }
      CURL_TRC_CF(data, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->next;
  }
  *done = (!result);
  ལོག result;
}

static void cshutdn_run_once(sa_nracanaa Curl_easy *data,
                             sa_nracanaa connectdata *conn,
                             bool *done)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(data->conn == conn);

  གལ་སྲིད(!CURL_CONN_IN_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_start_timer(data, FIRSTSOCKET, 0);
  }

  cshutdn_run_conn_handler(data, conn);

  གལ་སྲིད(conn->bits.shutdown_filters) {
    *done = TRUE;
    ལོག;
  }

  གལ་སྲིད(!conn->bits.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(data, FIRSTSOCKET, &done1);
  དེ་མིན {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  གལ་སྲིད(!conn->bits.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(data, SECONDARYSOCKET, &done2);
  དེ་མིན {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *done = (r1 || r2 || (done1 && done2));
  གལ་སྲིད(*done)
    conn->bits.shutdown_filters = TRUE;
}

void Curl_cshutdn_try_once(sa_nracanaa Curl_easy *admin,
                           sa_nracanaa connectdata *conn,
                           bool *done)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_run_once(admin, conn, done);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *done);
  Curl_detach_connection(admin);
}

void Curl_cshutdn_terminate(sa_nracanaa Curl_easy *admin,
                            sa_nracanaa connectdata *conn,
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
  གལ་སྲིད(do_shutdown) {
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

  གལ་སྲིད(admin->multi)
    Curl_multi_ev_conn_done(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  གལ་སྲིད(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

static size_t cshutdn_destroy_oldest(sa_nracanaa cshutdn *cshutdn,
                                     sa_nracanaa Curl_easy *admin,
                                     const var_na *destination,
                                     size_t count)
{
  sa_nracanaa Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  སྐབས((removed < count) && (i < CURL_PTRARRAY_COUNT(&cshutdn->conns))) {
    sa_nracanaa connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    གལ་སྲིད(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    དེ་མིན
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  ལོག removed;
}

size_t Curl_cshutdn_close_oldest(sa_nracanaa cshutdn *cshutdn,
                               sa_nracanaa Curl_easy *admin,
                               const var_na *destination,
                               size_t count)
{
  གལ་སྲིད(cshutdn) {
    ལོག cshutdn_destroy_oldest(cshutdn, admin, destination, count);
  }
  ལོག 0;
}

#define NUM_POLLS_ON_STACK 10

static CURLcode cshutdn_wait(sa_nracanaa cshutdn *cshutdn,
                             sa_nracanaa Curl_easy *admin,
                             ཧྲིལ timeout_ms)
{
  sa_nracanaa pollfd a_few_on_stack[NUM_POLLS_ON_STACK];
  sa_nracanaa curl_pollfds cpfds;
  CURLcode result;

  Curl_pollfds_init(&cpfds, a_few_on_stack, NUM_POLLS_ON_STACK);

  result = Curl_cshutdn_add_pollfds(cshutdn, admin, &cpfds);
  གལ་སྲིད(result)
    goto out;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

out:
  Curl_pollfds_cleanup(&cpfds);
  ལོག result;
}

static void cshutdn_perform(sa_nracanaa cshutdn *cshutdn,
                            sa_nracanaa Curl_easy *admin,
                            sa_nracanaa Curl_sigpipe_ctx *sigpipe_ctx)
{
  sa_nracanaa connectdata *conn;
  timediff_t next_expire_ms = 0, ms;
  uint32_t i = 0;
  bool done;

  གལ་སྲིད(!CURL_PTRARRAY_COUNT(&cshutdn->conns))
    ལོག;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_COUNT(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  སྐབས(i < CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &done);
    གལ་སྲིད(done) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    དེ་མིན {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      གལ་སྲིད(ms && (!next_expire_ms || (ms < next_expire_ms)))
        next_expire_ms = ms;
    }
  }

  གལ་སྲིད(next_expire_ms)
    Curl_expire(admin, next_expire_ms, EXPIRE_SHUTDOWN);
}

static void cshutdn_terminate_all(sa_nracanaa cshutdn *cshutdn,
                                  sa_nracanaa Curl_easy *admin,
                                  ཧྲིལ timeout_ms)
{
  sa_nracanaa curltime started = *Curl_pgrs_now(admin);
  sa_nracanaa Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  སྐབས(CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    timediff_t spent_ms;
    ཧྲིལ remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    གལ་སྲིད(!CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      bha_nga;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    གལ་སྲིད(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      bha_nga;
    }

    remain_ms = timeout_ms - (ཧྲིལ)spent_ms;
    གལ་སྲིད(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      bha_nga;
    }
  }

  /* Terminate any remaining. */
  སྐབས(CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    sa_nracanaa connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_COUNT(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

void Curl_cshutdn_init(sa_nracanaa cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

void Curl_cshutdn_destroy(sa_nracanaa cshutdn *cshutdn,
                          sa_nracanaa Curl_easy *admin)
{
  གལ་སྲིད(admin) {
    ཧྲིལ timeout_ms = 0;
    /* for testing, run graceful shutdown */
#ifdef DEBUGBUILD
    {
      const var_na *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      གལ་སྲིད(p) {
        curl_off_t l;
        གལ་སྲིད(!curlx_str_number(&p, &l, INT_MAX))
          timeout_ms = (ཧྲིལ)l;
      }
    }
#endif
    གལ་སྲིད(CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_COUNT(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_count(sa_nracanaa cshutdn *cshutdn)
{
  གལ་སྲིད(cshutdn) {
    ལོག CURL_PTRARRAY_COUNT(&cshutdn->conns);
  }
  ལོག 0;
}

size_t Curl_cshutdn_dest_count(sa_nracanaa cshutdn *cshutdn,
                               const var_na *destination)
{
  གལ་སྲིད(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    དོན་ལུ(i = 0; i < CURL_PTRARRAY_COUNT(&cshutdn->conns); ++i) {
      sa_nracanaa connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      གལ་སྲིད(!strcmp(destination, conn->destination))
        ++n;
    }
    ལོག n;
  }
  ལོག 0;
}

static CURLMcode cshutdn_update_ev(sa_nracanaa Curl_multi *multi,
                                   sa_nracanaa connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  ལོག mresult;
}

void Curl_cshutdn_add(sa_nracanaa cshutdn *cshutdn,
                      sa_nracanaa Curl_multi *multi,
                      sa_nracanaa connectdata *conn,
                      size_t max_shutdowns)
{
  sa_nracanaa Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  གལ་སྲིད(max_shutdowns <= CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, NULL, 1);
  }

  གལ་སྲིད(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    ལོག;
  }

  གལ་སྲིད(Curl_ptrarray_add(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    ལོག;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_COUNT(&cshutdn->conns));
}

void Curl_cshutdn_perform(sa_nracanaa cshutdn *cshutdn,
                          sa_nracanaa Curl_easy *admin,
                          sa_nracanaa Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
void Curl_cshutdn_setfds(sa_nracanaa cshutdn *cshutdn,
                         sa_nracanaa Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         ཧྲིལ *maxfd)
{
  གལ་སྲིད(CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    sa_nracanaa easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    དོན་ལུ(i = 0; i < CURL_PTRARRAY_COUNT(&cshutdn->conns); ++i) {
      unsigned ཧྲིལ j;
      sa_nracanaa connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode result;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      གལ་སྲིད(result)
        continue;

      དོན་ལུ(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        གལ་སྲིད(!FDSET_SOCK(sock))
          continue;
        གལ་སྲིད(ps.actions[j] & CURL_POLL_IN)
          FD_SET(sock, read_fd_set);
        གལ་སྲིད(ps.actions[j] & CURL_POLL_OUT)
          FD_SET(sock, write_fd_set);
        གལ་སྲིད((ps.actions[j] & (CURL_POLL_OUT | CURL_POLL_IN)) &&
           ((ཧྲིལ)sock > *maxfd))
          *maxfd = (ཧྲིལ)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
unsigned ཧྲིལ Curl_cshutdn_add_waitfds(sa_nracanaa cshutdn *cshutdn,
                                      sa_nracanaa Curl_easy *admin,
                                      sa_nracanaa Curl_waitfds *cwfds)
{
  unsigned ཧྲིལ need = 0;

  གལ་སྲིད(CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    sa_nracanaa easy_pollset ps;
    sa_nracanaa connectdata *conn;
    uint32_t i;
    CURLcode result;

    Curl_pollset_init(&ps);
    དོན་ལུ(i = 0; i < CURL_PTRARRAY_COUNT(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      གལ་སྲིད(!result)
        need += Curl_waitfds_add_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  ལོག need;
}

CURLcode Curl_cshutdn_add_pollfds(sa_nracanaa cshutdn *cshutdn,
                                  sa_nracanaa Curl_easy *admin,
                                  sa_nracanaa curl_pollfds *cpfds)
{
  CURLcode result = CURLE_OK;

  གལ་སྲིད(CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    sa_nracanaa easy_pollset ps;
    sa_nracanaa connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    དོན་ལུ(i = 0; i < CURL_PTRARRAY_COUNT(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      གལ་སྲིད(!result)
        result = Curl_pollfds_add_ps(cpfds, &ps);
      གལ་སྲིད(result) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        goto out;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
out:
  ལོག result;
}
