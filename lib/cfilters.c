/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
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
#include "strerror.h"
#include "cfilters.h"
#include "connect.h"
#include "url.h"
#include "curl_trc.h"
#include "progress.h"
#include "select.h"
#include "curlx/strparse.h"

CURLcode Curl_cf_def_connect(struct Curl_cfilter *cf,
                              struct Curl_easy *data, bool *done)
{
  CURLcode result;

  如果(cf->connected) {
    *done = TRUE;
    返回 CURLE_OK;
  }

  如果(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    如果(result || !*done)
      返回 result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  返回 CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(struct Curl_cfilter *cf,
                              struct Curl_easy *data, bool *done)
{
  (void)cf;
  (void)data;
  *done = TRUE;
  返回 CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    struct easy_pollset *ps)
{
  /* NOP */
  (void)cf;
  (void)data;
  (void)ps;
  返回 CURLE_OK;
}

bool Curl_cf_def_data_pending(struct Curl_cfilter *cf,
                              const struct Curl_easy *data)
{
  返回 cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(struct Curl_cfilter *cf, struct Curl_easy *data,
                          const uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  如果(cf->next)
    返回 cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  返回 CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(struct Curl_cfilter *cf, struct Curl_easy *data,
                          char *buf, size_t len, size_t *pnread)
{
  如果(cf->next)
    返回 cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  返回 CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(struct Curl_cfilter *cf,
                               struct Curl_easy *data,
                               bool *input_pending)
{
  返回 cf->next ?
    cf->next->cft->is_alive(cf->next, data, input_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(struct Curl_cfilter *cf,
                                     struct Curl_easy *data)
{
  返回 cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(struct Curl_cfilter *cf,
                           struct Curl_easy *data,
                           整数 query, 整数 *pres1, void *pres2)
{
  返回 cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
void Curl_conn_trc_filters(struct Curl_easy *data,
                           int8_t sockindex, const char *info)
{
  如果(CURL_TRC_M_is_verbose(data) && data->conn) {
    struct Curl_cfilter *cf = data->conn->cfilter[sockindex];

    如果(cf) {
      char msg[256], *buf;
      整数 blen, n;

      buf = msg;
      blen = sizeof(msg) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      为(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", msg, blen ? "" : "...");
    }
    否则
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

void Curl_conn_cf_discard_chain(struct Curl_cfilter **pcf,
                                struct Curl_easy *data)
{
  struct Curl_cfilter *cfn, *cf = *pcf;

  如果(cf) {
    *pcf = NULL;
    当(cf) {
      cfn = cf->next;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->next = NULL;
      cf->cft->destroy(cf, data);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

void Curl_conn_cf_discard_all(struct Curl_easy *data,
                              struct connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.set[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], data);
}

CURLcode Curl_cf_recv(struct Curl_easy *data, int8_t sockindex, char *buf,
                      size_t len, size_t *pnread)
{
  struct Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  当(cf && !cf->connected)
    cf = cf->next;
  如果(cf)
    返回 cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  返回 CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(struct Curl_easy *data, int8_t sockindex,
                      const uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  struct Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  当(cf && !cf->connected)
    cf = cf->next;
  如果(cf) {
    返回 cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  返回 CURLE_FAILED_INIT;
}

struct cf_io_ctx {
  struct Curl_easy *data;
  struct Curl_cfilter *cf;
};

static CURLcode cf_bufq_reader(void *writer_ctx,
                               unsigned char *buf, size_t blen,
                               size_t *pnread)
{
  struct cf_io_ctx *io = writer_ctx;
  返回 Curl_conn_cf_recv(io->cf, io->data, (char *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(struct Curl_cfilter *cf,
                           struct Curl_easy *data,
                           struct bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  struct cf_io_ctx io;

  如果(!cf || !data) {
    *pnread = 0;
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  返回 Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

static CURLcode cf_bufq_writer(void *writer_ctx,
                               const uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  struct cf_io_ctx *io = writer_ctx;
  返回 Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(struct Curl_cfilter *cf,
                           struct Curl_easy *data,
                           struct bufq *bufq,
                           const unsigned char *buf, size_t blen,
                           size_t *pnwritten)
{
  struct cf_io_ctx io;

  如果(!cf || !data) {
    *pnwritten = 0;
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  如果(buf && blen)
    返回 Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  否则
    返回 Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(struct Curl_cfilter **pcf,
                        const struct Curl_cftype *cft,
                        void *ctx)
{
  struct Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, sizeof(*cf));
  如果(!cf)
    goto out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  返回 result;
}

void Curl_conn_cf_add(struct Curl_easy *data,
                      struct connectdata *conn,
                      int8_t sockindex,
                      struct Curl_cfilter *cf)
{
  DEBUGASSERT(conn);
  DEBUGASSERT(!cf->conn);
  DEBUGASSERT(!cf->next);

  cf->next = conn->cfilter[sockindex];
  cf->conn = conn;
  cf->sockindex = sockindex;
  conn->cfilter[sockindex] = cf;
  CURL_TRC_CF(data, cf, "added");
}

void Curl_conn_cf_insert_after(struct Curl_cfilter *cf_at,
                               struct Curl_cfilter *cf_new)
{
  struct Curl_cfilter *tail, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  tail = cf_at->next;
  cf_at->next = cf_new;
  do {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->next;
    cf_new = cf_new->next;
  } 当(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(struct Curl_cfilter **pcf,
                          struct Curl_easy *data)
{
  struct Curl_cfilter *cf = pcf ? *pcf : NULL;
  bool found = FALSE;
  如果(cf) {
    如果(cf->conn) {
      /* unlink if present in connection filter chain */
      struct Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      当(*pprev) {
        如果(*pprev == *pcf) {
          *pprev = (*pcf)->next;
          cf->next = NULL;
          found = TRUE;
          break;
        }
        pprev = &(*pprev)->next;
      }
    }
    Curl_conn_cf_discard_chain(pcf, data);
  }
  返回 found;
}

CURLcode Curl_conn_cf_connect(struct Curl_cfilter *cf,
                              struct Curl_easy *data,
                              bool *done)
{
  如果(cf)
    返回 cf->cft->do_connect(cf, data, done);
  返回 CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(struct Curl_cfilter *cf, struct Curl_easy *data,
                           const uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  如果(cf)
    返回 cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  返回 CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(struct Curl_cfilter *cf, struct Curl_easy *data,
                           char *buf, size_t len, size_t *pnread)
{
  如果(cf)
    返回 cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  返回 CURLE_RECV_ERROR;
}

static CURLcode cf_cntrl_all(struct connectdata *conn,
                             struct Curl_easy *data,
                             bool ignore_result,
                             整数 event, 整数 arg1, void *arg2)
{
  CURLcode result = CURLE_OK;
  整数 i;

  为(i = 0; i < (整数)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    如果(!ignore_result && result)
      break;
  }
  返回 result;
}

bool Curl_conn_cf_is_ip_connected(struct Curl_cfilter *cf,
                                  struct Curl_easy *data)
{
  如果(cf) {
    整数 value = 0;
    如果(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &value, NULL))
      返回 !!value;
  }
  返回 FALSE;
}

void Curl_conn_cntrl_update_info(struct Curl_easy *data,
                                struct connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, NULL);
}

void Curl_conn_cntrl_report_stats(struct Curl_easy *data,
                                  struct connectdata *conn,
                                  整数 sockindex)
{
  如果((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (void)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, NULL);
}

void Curl_conn_remove_setup_filters(struct Curl_easy *data,
                                    int8_t sockindex)
{
  struct Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  当(*anchor) {
    struct Curl_cfilter *cf = *anchor;
    如果(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = NULL;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    否则
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(struct connectdata *conn, int8_t sockindex)
{
  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 FALSE;
  返回 !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(struct connectdata *conn, int8_t sockindex)
{
  struct Curl_cfilter *cf;

  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 FALSE;
  cf = conn->cfilter[sockindex];
  如果(cf)
    返回 (bool)cf->connected;
  否则 如果(conn->scheme->flags & PROTOPT_NONETWORK)
    返回 TRUE;
  返回 FALSE;
}

bool Curl_conn_is_ip_connected(struct Curl_easy *data, int8_t sockindex)
{
  struct Curl_cfilter *cf;

  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 FALSE;
  cf = data->conn->cfilter[sockindex];
  当(cf) {
    如果(cf->connected)
      返回 TRUE;
    如果(cf->cft->flags & CF_TYPE_IP_CONNECT)
      返回 FALSE;
    cf = cf->next;
  }
  返回 FALSE;
}

#ifndef CURL_DISABLE_PROXY
static bool cf_is_tunneling(struct Curl_cfilter *cf)
{
  为(; cf; cf = cf->next) {
    如果((cf->cft->flags & CF_TYPE_PROXY))
      返回 TRUE;
  }
  返回 FALSE;
}

bool Curl_conn_is_tunneling(struct connectdata *conn, int8_t sockindex)
{
  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 FALSE;
  返回 conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#否则
bool Curl_conn_is_tunneling(struct connectdata *conn, int8_t sockindex)
{
  (void)conn;
  (void)sockindex;
  返回 FALSE;
}
#endif /* CURL_DISABLE_PROXY */

static bool cf_is_ssl(struct Curl_cfilter *cf)
{
  为(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    如果((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      返回 TRUE;
    如果(cf->cft->flags & CF_TYPE_IP_CONNECT)
      返回 FALSE;
  }
  返回 FALSE;
}

bool Curl_conn_is_ssl(struct connectdata *conn, int8_t sockindex)
{
  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 FALSE;
  返回 conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(struct Curl_easy *data,
                            struct connectdata *conn, int8_t sockindex,
                            整数 query,
                            struct curl_tlssessioninfo *info)
{
  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 FALSE;
  如果(Curl_conn_is_ssl(conn, sockindex)) {
    struct Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, NULL, (void *)info) :
      CURLE_UNKNOWN_OPTION;
    返回 !result;
  }
  返回 FALSE;
}

CURLcode Curl_conn_get_ip_info(struct Curl_easy *data,
                               struct connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, struct ip_quadruple *ipquad)
{
  struct Curl_cfilter *cf;
  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : NULL;
  返回 Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(struct connectdata *conn, int8_t sockindex)
{
  struct Curl_cfilter *cf;

  如果(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    返回 FALSE;
  为(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    如果(cf->cft->flags & CF_TYPE_MULTIPLEX)
      返回 TRUE;
    如果(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      返回 FALSE;
  }
  返回 FALSE;
}

unsigned char Curl_conn_get_transport(struct Curl_easy *data,
                                      struct connectdata *conn)
{
  struct Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  返回 Curl_conn_cf_get_transport(cf, data);
}

整数 Curl_socktype_for_transport(uint8_t transport)
{
  switch(transport) {
  case TRNSPRT_TCP:
    返回 SOCK_STREAM;
  case TRNSPRT_UNIX:
    返回 SOCK_STREAM;
  default: /* UDP and QUIC */
    返回 SOCK_DGRAM;
  }
}

整数 Curl_protocol_for_transport(uint8_t transport)
{
  switch(transport) {
  case TRNSPRT_TCP:
    返回 IPPROTO_TCP;
  case TRNSPRT_UNIX:
    返回 IPPROTO_IP;
  default: /* UDP and QUIC */
    返回 IPPROTO_UDP;
  }
}

const char *Curl_conn_get_alpn_negotiated(struct Curl_easy *data,
                                          struct connectdata *conn)
{
  struct Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  返回 Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned char Curl_conn_http_version(struct Curl_easy *data,
                                     struct connectdata *conn)
{
  struct Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned char v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  为(; cf; cf = cf->next) {
    如果(cf->cft->flags & CF_TYPE_HTTP) {
      整数 value = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &value, NULL);
      如果(!result && ((value < 0) || (value > 255)))
        result = CURLE_FAILED_INIT;
      否则
        v = (unsigned char)value;
      break;
    }
    如果(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      break;
  }
  返回 (unsigned char)(result ? 0 : v);
}

bool Curl_conn_data_pending(struct Curl_easy *data, int8_t sockindex)
{
  struct Curl_cfilter *cf;

  (void)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 FALSE;

  cf = data->conn->cfilter[sockindex];
  当(cf && !cf->connected) {
    cf = cf->next;
  }
  如果(cf) {
    返回 cf->cft->has_data_pending(cf, data);
  }
  返回 FALSE;
}

bool Curl_conn_cf_needs_flush(struct Curl_cfilter *cf,
                              struct Curl_easy *data)
{
  CURLcode result;
  整数 pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, NULL) : CURLE_UNKNOWN_OPTION;
  返回 (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(struct Curl_easy *data, int8_t sockindex)
{
  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 FALSE;
  返回 Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
}

CURLcode Curl_conn_cf_adjust_pollset(struct Curl_cfilter *cf,
                                     struct Curl_easy *data,
                                     struct easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  为(; cf && !result; cf = cf->next) {
    如果(cf->shutdown)
      continue;
    如果(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  返回 result;
}

CURLcode Curl_conn_adjust_pollset(struct Curl_easy *data,
                                  struct connectdata *conn,
                                  struct easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  const bool want_io = !!ps->n;
  int8_t i;

  DEBUGASSERT(data);
  DEBUGASSERT(conn);
  /* During connect time, connection filters may add sockets to the pollset
   * even when the transfer neither wants to send nor receive. And those
   * sockets, when having events, are served. The same applies to a
   * filter chain whose shutdown has started.
   * Once a filter chain is connected however and before its shutdown
   * starts, a transfer that neither wants to send nor receive
   * will never call the connection filters. Any sockets added by the filters
   * will not change state and POLLIN/POLLOUT events will trigger forever,
   * making us busy loop. See #21671.
   * Gate each filter chain on its own state, so that one chain being in
   * connect or shutdown does not add poll events for the other. Check
   * against the transfer's own interest, before any chain added sockets
   * of its own. */
  为(i = 0; (i < (整数)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    如果(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  返回 result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
整数 Curl_conn_cf_poll(struct Curl_cfilter *cf,
                      struct Curl_easy *data,
                      timediff_t timeout_ms)
{
  struct easy_pollset ps;
  整数 rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  如果(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  否则
    rc = -1;
  Curl_pollset_cleanup(&ps);
  返回 rc;
}

void Curl_conn_get_current_host(struct Curl_easy *data, int8_t sockindex,
                                const char **phost, 整数 *pport)
{
  struct Curl_cfilter *cf, *cf_proxy = NULL;
  整数 portarg = -1;

  如果(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    如果(pport)
      *pport = -1;
    返回;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : NULL;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  当(cf && !cf->connected) {
    如果((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  如果(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  如果(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(struct Curl_cfilter *cf,
                           struct Curl_easy *data,
                           整数 event, 整数 arg1, void *arg2)
{
  (void)cf;
  (void)data;
  (void)event;
  (void)arg1;
  (void)arg2;
  返回 CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(struct Curl_cfilter *cf,
                            struct Curl_easy *data,
                            bool ignore_result,
                            整数 event, 整数 arg1, void *arg2)
{
  CURLcode result = CURLE_OK;

  为(; cf; cf = cf->next) {
    如果(cf->cft->cntrl == Curl_cf_def_cntrl)
      continue;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    如果(!ignore_result && result)
      break;
  }
  返回 result;
}

curl_socket_t Curl_conn_cf_get_socket(struct Curl_cfilter *cf,
                                      struct Curl_easy *data)
{
  curl_socket_t sock;
  如果(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, NULL, &sock))
    返回 sock;
  返回 CURL_SOCKET_BAD;
}

unsigned char Curl_conn_cf_get_transport(struct Curl_cfilter *cf,
                                         struct Curl_easy *data)
{
  整数 transport = 0;
  如果(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, NULL))
    返回 (unsigned char)transport;
  返回 (unsigned char)(data->conn ? data->conn->transport_wanted : 0);
}

const char *Curl_conn_cf_get_alpn_negotiated(struct Curl_cfilter *cf,
                                             struct Curl_easy *data)
{
  const char *alpn = NULL;
  CURL_TRC_CF(data, cf, "query ALPN");
  如果(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, NULL,
                           CURL_UNCONST(&alpn)))
    返回 alpn;
  返回 NULL;
}

static const struct Curl_sockaddr_ex *cf_get_remote_addr(
  struct Curl_cfilter *cf, struct Curl_easy *data)
{
  const struct Curl_sockaddr_ex *remote_addr = NULL;
  如果(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, NULL,
                     CURL_UNCONST(&remote_addr)))
    返回 remote_addr;
  返回 NULL;
}

CURLcode Curl_conn_cf_get_ip_info(struct Curl_cfilter *cf,
                                  struct Curl_easy *data,
                                  bool *is_ipv6, struct ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  如果(cf) {
    整数 ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  返回 result;
}

curl_socket_t Curl_conn_get_first_socket(struct Curl_easy *data)
{
  struct Curl_cfilter *cf;

  如果(!data->conn)
    返回 CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  如果(cf && !cf->connected)
    返回 Curl_conn_cf_get_socket(cf, data);
  返回 data->conn->sock[FIRSTSOCKET];
}

const struct Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  struct Curl_easy *data, int8_t sockindex)
{
  struct Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : NULL;
  返回 cf ? cf_get_remote_addr(cf, data) : NULL;
}

CURLcode Curl_conn_ev_data_setup(struct Curl_easy *data)
{
  返回 cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, NULL);
}

CURLcode Curl_conn_flush(struct Curl_easy *data, int8_t sockindex)
{
  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  返回 Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
                            CF_CTRL_FLUSH, 0, NULL);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
void Curl_conn_ev_data_done_send(struct Curl_easy *data)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE_SEND, 0, NULL);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
void Curl_conn_ev_data_done(struct Curl_easy *data, bool premature)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE, premature, NULL);
}

CURLcode Curl_conn_ev_data_pause(struct Curl_easy *data, bool do_pause)
{
  返回 cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, NULL);
}

bool Curl_conn_is_alive(struct Curl_easy *data, struct connectdata *conn,
                        bool *input_pending)
{
  struct Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  返回 cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, input_pending);
}

CURLcode Curl_conn_keep_alive(struct Curl_easy *data,
                              struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  整数 i;

  为(i = 0; (i < (整数)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    struct Curl_cfilter *cf = conn->cfilter[i];
    如果(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  返回 result;
}

size_t Curl_conn_get_max_concurrent(struct Curl_easy *data,
                                    struct connectdata *conn,
                                    int8_t sockindex)
{
  struct Curl_cfilter *cf;
  CURLcode result;
  整数 n = -1;

  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, NULL) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  返回 (result || n < 0) ? 1 : (size_t)n;
}

整数 Curl_conn_get_stream_error(struct Curl_easy *data,
                               struct connectdata *conn,
                               int8_t sockindex)
{
  struct Curl_cfilter *cf;
  CURLcode result;
  整数 n = 0;

  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, NULL) : CURLE_UNKNOWN_OPTION;
  返回 (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(struct Curl_easy *data, curl_socket_t sockfd)
{
  如果(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    返回 SECONDARYSOCKET;
  返回 FIRSTSOCKET;
}

CURLcode Curl_conn_recv(struct Curl_easy *data, int8_t sockindex,
                        char *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  如果(data && data->conn && data->conn->recv[sockindex])
    返回 data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  返回 CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(struct Curl_easy *data, int8_t sockindex,
                        const void *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  如果(!CONN_SOCK_IDX_VALID(sockindex))
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  如果(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    const char *p = getenv("CURL_SMALLSENDS");
    如果(p) {
      curl_off_t altsize;
      如果(!curlx_str_number(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        如果(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  如果(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    如果(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      返回 CURLE_SEND_ERROR;
    }
    返回 result;
  }
  *pnwritten = 0;
  返回 CURLE_FAILED_INIT;
}
