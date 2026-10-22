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

void Curl_cshutdn_start_timer(struct Curl_easy *data, int8_t sockindex,
                              整数 timeout_ms)
{
  struct connectdata *conn = data->conn;
  const struct curltime *pnow = Curl_pgrs_now(data);

  DEBUGASSERT(conn);
  conn->shutdown.start_ms[sockindex] =
    curlx_ptimediff_ms(pnow, &conn->created);
  conn->shutdown.timeout_ms = (timeout_ms > 0) ?
    (timediff_t)timeout_ms :
    ((data->set.shutdowntimeout > 0) ?
     data->set.shutdowntimeout : DEFAULT_SHUTDOWN_TIMEOUT_MS);
  /* Set a timer, unless we operate on the admin handle */
  如果(data->mid)
    Curl_expire_set(data, EXPIRE_SHUTDOWN, conn->shutdown.timeout_ms, pnow);
  CURL_TRC_M(data, "shutdown start on%s connection",
             sockindex ? " secondary" : "");
}

timediff_t Curl_cshutdn_timeleft_ms(struct Curl_easy *data,
                                    struct connectdata *conn,
                                    int8_t sockindex)
{
  timediff_t left_ms;

  如果(conn->shutdown.start_ms[sockindex] < 0)
    返回 0; /* not started or no limits */

  left_ms = conn->shutdown.timeout_ms -
            (curlx_ptimediff_ms(Curl_pgrs_now(data), &conn->created) -
             conn->shutdown.start_ms[sockindex]);
  返回 left_ms ? left_ms : -1;
}

static timediff_t cshutdn_conn_timeleft(struct Curl_easy *data,
                                        struct connectdata *conn)
{
  timediff_t left_ms = 0, ms;
  int8_t i;

  为(i = 0; conn->shutdown.timeout_ms && (i < 2); ++i) {
    如果(conn->shutdown.start_ms[i] < 0)
      continue;
    ms = Curl_cshutdn_timeleft_ms(data, conn, i);
    如果(ms && (!left_ms || ms < left_ms))
      left_ms = ms;
  }
  返回 left_ms;
}

void Curl_cshutdn_clear_timer(struct Curl_easy *data, int8_t sockindex)
{
  data->conn->shutdown.start_ms[sockindex] = -1;
}

static void cshutdn_run_conn_handler(struct Curl_easy *data,
                                     struct connectdata *conn)
{
  如果(!conn->bits.shutdown_handler) {

    如果(conn->scheme && conn->scheme->run->disconnect) {
      /* Some disconnect handlers do a blocking wait on server responses.
       * FTP/IMAP/SMTP and SFTP are among them. When using the internal
       * handle, set an overall short timeout so we do not hang for the
       * default 120 seconds. */
      如果(data->state.internal) {
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

CURLcode Curl_cshutdn_try_once_idx(struct Curl_easy *data,
                                   int8_t sockindex, bool *done)
{
  struct Curl_cfilter *cf;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;

  DEBUGASSERT(data->conn);

  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 CURLE_BAD_FUNCTION_ARGUMENT;

  /* Get the first connected filter that is not shut down already. */
  cf = data->conn->cfilter[sockindex];
  当(cf && (!cf->connected || cf->shutdown))
    cf = cf->next;

  如果(!cf) {
    *done = TRUE;
    返回 CURLE_OK;
  }

  *done = FALSE;
  如果(!CURL_CONN_IN_SHUTDOWN(data->conn, sockindex)) {
    Curl_cshutdn_start_timer(data, sockindex, 0);
  }
  否则 {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, data->conn, sockindex);
    如果(timeout_ms < 0) {
      /* info message, since this might be regarded as acceptable */
      infof(data, "shutdown timeout");
      返回 CURLE_OPERATION_TIMEDOUT;
    }
  }

  当(cf) {
    如果(!cf->shutdown) {
      bool cfdone = FALSE;
      result = cf->cft->do_shutdown(cf, data, &cfdone);
      如果(result) {
        CURL_TRC_CF(data, cf, "shut down failed with %d", (整数)result);
        返回 result;
      }
      否则 如果(!cfdone) {
        CURL_TRC_CF(data, cf, "shut down not done yet");
        返回 CURLE_OK;
      }
      CURL_TRC_CF(data, cf, "shut down successfully");
      cf->shutdown = TRUE;
    }
    cf = cf->next;
  }
  *done = (!result);
  返回 result;
}

static void cshutdn_run_once(struct Curl_easy *data,
                             struct connectdata *conn,
                             bool *done)
{
  CURLcode r1, r2;
  bool done1, done2;

  /* We expect to be attached when called */
  DEBUGASSERT(data->conn == conn);

  如果(!CURL_CONN_IN_SHUTDOWN(conn, FIRSTSOCKET)) {
    Curl_cshutdn_start_timer(data, FIRSTSOCKET, 0);
  }

  cshutdn_run_conn_handler(data, conn);

  如果(conn->bits.shutdown_filters) {
    *done = TRUE;
    返回;
  }

  如果(!conn->bits.connect_only && Curl_conn_is_connected(conn, FIRSTSOCKET))
    r1 = Curl_cshutdn_try_once_idx(data, FIRSTSOCKET, &done1);
  否则 {
    r1 = CURLE_OK;
    done1 = TRUE;
  }

  如果(!conn->bits.connect_only && Curl_conn_is_connected(conn, SECONDARYSOCKET))
    r2 = Curl_cshutdn_try_once_idx(data, SECONDARYSOCKET, &done2);
  否则 {
    r2 = CURLE_OK;
    done2 = TRUE;
  }

  /* we are done when any failed or both report success */
  *done = (r1 || r2 || (done1 && done2));
  如果(*done)
    conn->bits.shutdown_filters = TRUE;
}

void Curl_cshutdn_try_once(struct Curl_easy *admin,
                           struct connectdata *conn,
                           bool *done)
{
  DEBUGASSERT(!admin->conn);
  DEBUGASSERT(!admin->mid);
  Curl_attach_connection(admin, conn, FALSE);
  cshutdn_run_once(admin, conn, done);
  CURL_TRC_M(admin, "[SHUTDOWN] shutdown, done=%d", *done);
  Curl_detach_connection(admin);
}

void Curl_cshutdn_terminate(struct Curl_easy *admin,
                            struct connectdata *conn,
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
  如果(do_shutdown) {
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

  如果(admin->multi)
    Curl_multi_ev_conn_done(admin->multi, admin, conn);
  Curl_conn_free(admin, conn);

  如果(admin->multi) {
    CURL_TRC_M(admin, "[SHUTDOWN] trigger multi connchanged");
    Curl_multi_connchanged(admin->multi);
  }
}

static size_t cshutdn_destroy_oldest(struct cshutdn *cshutdn,
                                     struct Curl_easy *admin,
                                     const char *destination,
                                     size_t count)
{
  struct Curl_sigpipe_ctx sigpipe_ctx;
  size_t removed = 0;
  uint32_t i = 0;

  sigpipe_init(&sigpipe_ctx);
  当((removed < count) && (i < CURL_PTRARRAY_COUNT(&cshutdn->conns))) {
    struct connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    如果(!destination || !strcmp(destination, conn->destination)) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      sigpipe_apply(admin, &sigpipe_ctx);
      Curl_cshutdn_terminate(admin, conn, FALSE);
      ++removed;
    }
    否则
      ++i;
  }
  sigpipe_restore(&sigpipe_ctx);
  返回 removed;
}

size_t Curl_cshutdn_close_oldest(struct cshutdn *cshutdn,
                               struct Curl_easy *admin,
                               const char *destination,
                               size_t count)
{
  如果(cshutdn) {
    返回 cshutdn_destroy_oldest(cshutdn, admin, destination, count);
  }
  返回 0;
}

#define NUM_POLLS_ON_STACK 10

static CURLcode cshutdn_wait(struct cshutdn *cshutdn,
                             struct Curl_easy *admin,
                             整数 timeout_ms)
{
  struct pollfd a_few_on_stack[NUM_POLLS_ON_STACK];
  struct curl_pollfds cpfds;
  CURLcode result;

  Curl_pollfds_init(&cpfds, a_few_on_stack, NUM_POLLS_ON_STACK);

  result = Curl_cshutdn_add_pollfds(cshutdn, admin, &cpfds);
  如果(result)
    goto out;

  Curl_poll(cpfds.pfds, cpfds.n, CURLMIN(timeout_ms, 1000));

out:
  Curl_pollfds_cleanup(&cpfds);
  返回 result;
}

static void cshutdn_perform(struct cshutdn *cshutdn,
                            struct Curl_easy *admin,
                            struct Curl_sigpipe_ctx *sigpipe_ctx)
{
  struct connectdata *conn;
  timediff_t next_expire_ms = 0, ms;
  uint32_t i = 0;
  bool done;

  如果(!CURL_PTRARRAY_COUNT(&cshutdn->conns))
    返回;

  CURL_TRC_M(admin, "[SHUTDOWN] perform on %u connections",
             CURL_PTRARRAY_COUNT(&cshutdn->conns));
  sigpipe_apply(admin, sigpipe_ctx);
  当(i < CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
    Curl_cshutdn_try_once(admin, conn, &done);
    如果(done) {
      Curl_ptrarray_remove(&cshutdn->conns, i);
      Curl_cshutdn_terminate(admin, conn, FALSE);
    }
    否则 {
      ++i;
      /* idata has one timer list, but maybe more than one connection.
       * Set EXPIRE_SHUTDOWN to the smallest time left for all. */
      ms = cshutdn_conn_timeleft(admin, conn);
      如果(ms && (!next_expire_ms || (ms < next_expire_ms)))
        next_expire_ms = ms;
    }
  }

  如果(next_expire_ms)
    Curl_expire(admin, next_expire_ms, EXPIRE_SHUTDOWN);
}

static void cshutdn_terminate_all(struct cshutdn *cshutdn,
                                  struct Curl_easy *admin,
                                  整数 timeout_ms)
{
  struct curltime started = *Curl_pgrs_now(admin);
  struct Curl_sigpipe_ctx sigpipe_ctx;

  CURL_TRC_M(admin, "[SHUTDOWN] shutdown all");
  sigpipe_init(&sigpipe_ctx);

  当(CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    timediff_t spent_ms;
    整数 remain_ms;

    cshutdn_perform(cshutdn, admin, &sigpipe_ctx);

    如果(!CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished cleanly");
      break;
    }

    /* wait for activity, timeout or "nothing" */
    spent_ms = curlx_ptimediff_ms(Curl_pgrs_now(admin), &started);
    如果(spent_ms >= (timediff_t)timeout_ms) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, %s",
                 (timeout_ms > 0) ? "timeout" : "best effort done");
      break;
    }

    remain_ms = timeout_ms - (整数)spent_ms;
    如果(cshutdn_wait(cshutdn, admin, remain_ms)) {
      CURL_TRC_M(admin, "[SHUTDOWN] shutdown finished, aborted");
      break;
    }
  }

  /* Terminate any remaining. */
  当(CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    struct connectdata *conn = Curl_ptrarray_remove(&cshutdn->conns, 0);
    Curl_cshutdn_terminate(admin, conn, FALSE);
  }
  DEBUGASSERT(!CURL_PTRARRAY_COUNT(&cshutdn->conns));

  sigpipe_restore(&sigpipe_ctx);
}

void Curl_cshutdn_init(struct cshutdn *cshutdn)
{
  Curl_ptrarray_init(&cshutdn->conns);
}

void Curl_cshutdn_destroy(struct cshutdn *cshutdn,
                          struct Curl_easy *admin)
{
  如果(admin) {
    整数 timeout_ms = 0;
    /* for testing, run graceful shutdown */
#ifdef DEBUGBUILD
    {
      const char *p = getenv("CURL_GRACEFUL_SHUTDOWN");
      如果(p) {
        curl_off_t l;
        如果(!curlx_str_number(&p, &l, INT_MAX))
          timeout_ms = (整数)l;
      }
    }
#endif
    如果(CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
      CURL_TRC_M(admin, "[SHUTDOWN] destroy, %u connections, timeout=%dms",
                 CURL_PTRARRAY_COUNT(&cshutdn->conns), timeout_ms);
      cshutdn_terminate_all(cshutdn, admin, timeout_ms);
    }
  }
}

size_t Curl_cshutdn_count(struct cshutdn *cshutdn)
{
  如果(cshutdn) {
    返回 CURL_PTRARRAY_COUNT(&cshutdn->conns);
  }
  返回 0;
}

size_t Curl_cshutdn_dest_count(struct cshutdn *cshutdn,
                               const char *destination)
{
  如果(cshutdn) {
    size_t n = 0;
    uint32_t i = 0;
    为(i = 0; i < CURL_PTRARRAY_COUNT(&cshutdn->conns); ++i) {
      struct connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      如果(!strcmp(destination, conn->destination))
        ++n;
    }
    返回 n;
  }
  返回 0;
}

static CURLMcode cshutdn_update_ev(struct Curl_multi *multi,
                                   struct connectdata *conn)
{
  CURLMcode mresult;
  Curl_attach_connection(multi->admin, conn, FALSE);
  mresult = Curl_multi_ev_assess_conn(multi, multi->admin, conn);
  Curl_detach_connection(multi->admin);
  返回 mresult;
}

void Curl_cshutdn_add(struct cshutdn *cshutdn,
                      struct Curl_multi *multi,
                      struct connectdata *conn,
                      size_t max_shutdowns)
{
  struct Curl_easy *admin = multi->admin;

  /* Add the connection to our shutdown list for non-blocking shutdown
   * during multi processing. */
  如果(max_shutdowns <= CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    CURL_TRC_M(admin, "[SHUTDOWN] discarding oldest shutdown connection "
               "due to shutdown limit of %zu", max_shutdowns);
    cshutdn_destroy_oldest(cshutdn, admin, NULL, 1);
  }

  如果(multi->socket_cb && cshutdn_update_ev(multi, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] update events failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    返回;
  }

  如果(Curl_ptrarray_add(&cshutdn->conns, conn)) {
    CURL_TRC_M(admin, "[SHUTDOWN] adding failed, discarding #%"
               FMT_OFF_T, conn->connection_id);
    Curl_cshutdn_terminate(admin, conn, FALSE);
    返回;
  }
  CURL_TRC_M(admin, "[SHUTDOWN] added #%" FMT_OFF_T
             " to shutdowns, now %u conns in shutdown",
             conn->connection_id, CURL_PTRARRAY_COUNT(&cshutdn->conns));
}

void Curl_cshutdn_perform(struct cshutdn *cshutdn,
                          struct Curl_easy *admin,
                          struct Curl_sigpipe_ctx *sigpipe_ctx)
{
  cshutdn_perform(cshutdn, admin, sigpipe_ctx);
}

/* return fd_set info about the shutdown connections */
void Curl_cshutdn_setfds(struct cshutdn *cshutdn,
                         struct Curl_easy *admin,
                         fd_set *read_fd_set, fd_set *write_fd_set,
                         整数 *maxfd)
{
  如果(CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    struct easy_pollset ps;
    uint32_t i;

    Curl_pollset_init(&ps);
    为(i = 0; i < CURL_PTRARRAY_COUNT(&cshutdn->conns); ++i) {
      unsigned 整数 j;
      struct connectdata *conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      CURLcode result;

      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      如果(result)
        continue;

      为(j = 0; j < ps.n; j++) {
        curl_socket_t sock = ps.sockets[j];
        如果(!FDSET_SOCK(sock))
          continue;
        如果(ps.actions[j] & CURL_POLL_IN)
          FD_SET(sock, read_fd_set);
        如果(ps.actions[j] & CURL_POLL_OUT)
          FD_SET(sock, write_fd_set);
        如果((ps.actions[j] & (CURL_POLL_OUT | CURL_POLL_IN)) &&
           ((整数)sock > *maxfd))
          *maxfd = (整数)sock;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
}

/* return information about the shutdown connections */
unsigned 整数 Curl_cshutdn_add_waitfds(struct cshutdn *cshutdn,
                                      struct Curl_easy *admin,
                                      struct Curl_waitfds *cwfds)
{
  unsigned 整数 need = 0;

  如果(CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    struct easy_pollset ps;
    struct connectdata *conn;
    uint32_t i;
    CURLcode result;

    Curl_pollset_init(&ps);
    为(i = 0; i < CURL_PTRARRAY_COUNT(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      如果(!result)
        need += Curl_waitfds_add_ps(cwfds, &ps);
    }
    Curl_pollset_cleanup(&ps);
  }
  返回 need;
}

CURLcode Curl_cshutdn_add_pollfds(struct cshutdn *cshutdn,
                                  struct Curl_easy *admin,
                                  struct curl_pollfds *cpfds)
{
  CURLcode result = CURLE_OK;

  如果(CURL_PTRARRAY_COUNT(&cshutdn->conns)) {
    struct easy_pollset ps;
    struct connectdata *conn;
    uint32_t i;

    Curl_pollset_init(&ps);
    为(i = 0; i < CURL_PTRARRAY_COUNT(&cshutdn->conns); ++i) {
      conn = CURL_PTRARRAY_GET(&cshutdn->conns, i);
      Curl_pollset_reset(&ps);
      Curl_attach_connection(admin, conn, FALSE);
      result = Curl_conn_adjust_pollset(admin, conn, &ps);
      Curl_detach_connection(admin);

      如果(!result)
        result = Curl_pollfds_add_ps(cpfds, &ps);
      如果(result) {
        Curl_pollset_cleanup(&ps);
        Curl_pollfds_cleanup(cpfds);
        goto out;
      }
    }
    Curl_pollset_cleanup(&ps);
  }
out:
  返回 result;
}
