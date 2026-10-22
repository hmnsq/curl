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

  ако(cf->connected) {
    *done = TRUE;
    врати CURLE_OK;
  }

  ако(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ако(result || !*done)
      врати result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  врати CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(struct Curl_cfilter *cf,
                              struct Curl_easy *data, bool *done)
{
  (void)cf;
  (void)data;
  *done = TRUE;
  врати CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    struct easy_pollset *ps)
{
  /* NOP */
  (void)cf;
  (void)data;
  (void)ps;
  врати CURLE_OK;
}

bool Curl_cf_def_data_pending(struct Curl_cfilter *cf,
                              const struct Curl_easy *data)
{
  врати cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(struct Curl_cfilter *cf, struct Curl_easy *data,
                          const uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  ако(cf->next)
    врати cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  врати CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(struct Curl_cfilter *cf, struct Curl_easy *data,
                          char *buf, size_t len, size_t *pnread)
{
  ако(cf->next)
    врати cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  врати CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(struct Curl_cfilter *cf,
                               struct Curl_easy *data,
                               bool *input_pending)
{
  врати cf->next ?
    cf->next->cft->is_alive(cf->next, data, input_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(struct Curl_cfilter *cf,
                                     struct Curl_easy *data)
{
  врати cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(struct Curl_cfilter *cf,
                           struct Curl_easy *data,
                           цео query, цео *pres1, void *pres2)
{
  врати cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
void Curl_conn_trc_filters(struct Curl_easy *data,
                           int8_t sockindex, const char *info)
{
  ако(CURL_TRC_M_is_verbose(data) && data->conn) {
    struct Curl_cfilter *cf = data->conn->cfilter[sockindex];

    ако(cf) {
      char msg[256], *buf;
      цео blen, n;

      buf = msg;
      blen = sizeof(msg) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      за(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", msg, blen ? "" : "...");
    }
    инако
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

void Curl_conn_cf_discard_chain(struct Curl_cfilter **pcf,
                                struct Curl_easy *data)
{
  struct Curl_cfilter *cfn, *cf = *pcf;

  ако(cf) {
    *pcf = NULL;
    док(cf) {
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
  док(cf && !cf->connected)
    cf = cf->next;
  ако(cf)
    врати cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  врати CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(struct Curl_easy *data, int8_t sockindex,
                      const uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  struct Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  док(cf && !cf->connected)
    cf = cf->next;
  ако(cf) {
    врати cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  врати CURLE_FAILED_INIT;
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
  врати Curl_conn_cf_recv(io->cf, io->data, (char *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(struct Curl_cfilter *cf,
                           struct Curl_easy *data,
                           struct bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  struct cf_io_ctx io;

  ако(!cf || !data) {
    *pnread = 0;
    врати CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  врати Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

static CURLcode cf_bufq_writer(void *writer_ctx,
                               const uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  struct cf_io_ctx *io = writer_ctx;
  врати Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(struct Curl_cfilter *cf,
                           struct Curl_easy *data,
                           struct bufq *bufq,
                           const unsigned char *buf, size_t blen,
                           size_t *pnwritten)
{
  struct cf_io_ctx io;

  ако(!cf || !data) {
    *pnwritten = 0;
    врати CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  ако(buf && blen)
    врати Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  инако
    врати Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(struct Curl_cfilter **pcf,
                        const struct Curl_cftype *cft,
                        void *ctx)
{
  struct Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, sizeof(*cf));
  ако(!cf)
    goto out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  врати result;
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
  } док(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(struct Curl_cfilter **pcf,
                          struct Curl_easy *data)
{
  struct Curl_cfilter *cf = pcf ? *pcf : NULL;
  bool found = FALSE;
  ако(cf) {
    ако(cf->conn) {
      /* unlink if present in connection filter chain */
      struct Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      док(*pprev) {
        ако(*pprev == *pcf) {
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
  врати found;
}

CURLcode Curl_conn_cf_connect(struct Curl_cfilter *cf,
                              struct Curl_easy *data,
                              bool *done)
{
  ако(cf)
    врати cf->cft->do_connect(cf, data, done);
  врати CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(struct Curl_cfilter *cf, struct Curl_easy *data,
                           const uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ако(cf)
    врати cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  врати CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(struct Curl_cfilter *cf, struct Curl_easy *data,
                           char *buf, size_t len, size_t *pnread)
{
  ако(cf)
    врати cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  врати CURLE_RECV_ERROR;
}

static CURLcode cf_cntrl_all(struct connectdata *conn,
                             struct Curl_easy *data,
                             bool ignore_result,
                             цео event, цео arg1, void *arg2)
{
  CURLcode result = CURLE_OK;
  цео i;

  за(i = 0; i < (цео)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    ако(!ignore_result && result)
      break;
  }
  врати result;
}

bool Curl_conn_cf_is_ip_connected(struct Curl_cfilter *cf,
                                  struct Curl_easy *data)
{
  ако(cf) {
    цео value = 0;
    ако(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &value, NULL))
      врати !!value;
  }
  врати FALSE;
}

void Curl_conn_cntrl_update_info(struct Curl_easy *data,
                                struct connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, NULL);
}

void Curl_conn_cntrl_report_stats(struct Curl_easy *data,
                                  struct connectdata *conn,
                                  цео sockindex)
{
  ако((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (void)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, NULL);
}

void Curl_conn_remove_setup_filters(struct Curl_easy *data,
                                    int8_t sockindex)
{
  struct Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  док(*anchor) {
    struct Curl_cfilter *cf = *anchor;
    ако(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = NULL;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    инако
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(struct connectdata *conn, int8_t sockindex)
{
  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати FALSE;
  врати !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(struct connectdata *conn, int8_t sockindex)
{
  struct Curl_cfilter *cf;

  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати FALSE;
  cf = conn->cfilter[sockindex];
  ако(cf)
    врати (bool)cf->connected;
  инако ако(conn->scheme->flags & PROTOPT_NONETWORK)
    врати TRUE;
  врати FALSE;
}

bool Curl_conn_is_ip_connected(struct Curl_easy *data, int8_t sockindex)
{
  struct Curl_cfilter *cf;

  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати FALSE;
  cf = data->conn->cfilter[sockindex];
  док(cf) {
    ако(cf->connected)
      врати TRUE;
    ако(cf->cft->flags & CF_TYPE_IP_CONNECT)
      врати FALSE;
    cf = cf->next;
  }
  врати FALSE;
}

#ifndef CURL_DISABLE_PROXY
static bool cf_is_tunneling(struct Curl_cfilter *cf)
{
  за(; cf; cf = cf->next) {
    ако((cf->cft->flags & CF_TYPE_PROXY))
      врати TRUE;
  }
  врати FALSE;
}

bool Curl_conn_is_tunneling(struct connectdata *conn, int8_t sockindex)
{
  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати FALSE;
  врати conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#инако
bool Curl_conn_is_tunneling(struct connectdata *conn, int8_t sockindex)
{
  (void)conn;
  (void)sockindex;
  врати FALSE;
}
#endif /* CURL_DISABLE_PROXY */

static bool cf_is_ssl(struct Curl_cfilter *cf)
{
  за(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    ако((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      врати TRUE;
    ако(cf->cft->flags & CF_TYPE_IP_CONNECT)
      врати FALSE;
  }
  врати FALSE;
}

bool Curl_conn_is_ssl(struct connectdata *conn, int8_t sockindex)
{
  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати FALSE;
  врати conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(struct Curl_easy *data,
                            struct connectdata *conn, int8_t sockindex,
                            цео query,
                            struct curl_tlssessioninfo *info)
{
  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати FALSE;
  ако(Curl_conn_is_ssl(conn, sockindex)) {
    struct Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, NULL, (void *)info) :
      CURLE_UNKNOWN_OPTION;
    врати !result;
  }
  врати FALSE;
}

CURLcode Curl_conn_get_ip_info(struct Curl_easy *data,
                               struct connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, struct ip_quadruple *ipquad)
{
  struct Curl_cfilter *cf;
  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : NULL;
  врати Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(struct connectdata *conn, int8_t sockindex)
{
  struct Curl_cfilter *cf;

  ако(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    врати FALSE;
  за(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    ако(cf->cft->flags & CF_TYPE_MULTIPLEX)
      врати TRUE;
    ако(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      врати FALSE;
  }
  врати FALSE;
}

unsigned char Curl_conn_get_transport(struct Curl_easy *data,
                                      struct connectdata *conn)
{
  struct Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  врати Curl_conn_cf_get_transport(cf, data);
}

цео Curl_socktype_for_transport(uint8_t transport)
{
  switch(transport) {
  case TRNSPRT_TCP:
    врати SOCK_STREAM;
  case TRNSPRT_UNIX:
    врати SOCK_STREAM;
  default: /* UDP and QUIC */
    врати SOCK_DGRAM;
  }
}

цео Curl_protocol_for_transport(uint8_t transport)
{
  switch(transport) {
  case TRNSPRT_TCP:
    врати IPPROTO_TCP;
  case TRNSPRT_UNIX:
    врати IPPROTO_IP;
  default: /* UDP and QUIC */
    врати IPPROTO_UDP;
  }
}

const char *Curl_conn_get_alpn_negotiated(struct Curl_easy *data,
                                          struct connectdata *conn)
{
  struct Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  врати Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned char Curl_conn_http_version(struct Curl_easy *data,
                                     struct connectdata *conn)
{
  struct Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned char v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  за(; cf; cf = cf->next) {
    ако(cf->cft->flags & CF_TYPE_HTTP) {
      цео value = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &value, NULL);
      ако(!result && ((value < 0) || (value > 255)))
        result = CURLE_FAILED_INIT;
      инако
        v = (unsigned char)value;
      break;
    }
    ако(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      break;
  }
  врати (unsigned char)(result ? 0 : v);
}

bool Curl_conn_data_pending(struct Curl_easy *data, int8_t sockindex)
{
  struct Curl_cfilter *cf;

  (void)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати FALSE;

  cf = data->conn->cfilter[sockindex];
  док(cf && !cf->connected) {
    cf = cf->next;
  }
  ако(cf) {
    врати cf->cft->has_data_pending(cf, data);
  }
  врати FALSE;
}

bool Curl_conn_cf_needs_flush(struct Curl_cfilter *cf,
                              struct Curl_easy *data)
{
  CURLcode result;
  цео pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, NULL) : CURLE_UNKNOWN_OPTION;
  врати (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(struct Curl_easy *data, int8_t sockindex)
{
  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати FALSE;
  врати Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
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
  за(; cf && !result; cf = cf->next) {
    ако(cf->shutdown)
      continue;
    ако(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  врати result;
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
  за(i = 0; (i < (цео)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    ако(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  врати result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
цео Curl_conn_cf_poll(struct Curl_cfilter *cf,
                      struct Curl_easy *data,
                      timediff_t timeout_ms)
{
  struct easy_pollset ps;
  цео rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  ако(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  инако
    rc = -1;
  Curl_pollset_cleanup(&ps);
  врати rc;
}

void Curl_conn_get_current_host(struct Curl_easy *data, int8_t sockindex,
                                const char **phost, цео *pport)
{
  struct Curl_cfilter *cf, *cf_proxy = NULL;
  цео portarg = -1;

  ако(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    ако(pport)
      *pport = -1;
    врати;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : NULL;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  док(cf && !cf->connected) {
    ако((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  ако(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  ако(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(struct Curl_cfilter *cf,
                           struct Curl_easy *data,
                           цео event, цео arg1, void *arg2)
{
  (void)cf;
  (void)data;
  (void)event;
  (void)arg1;
  (void)arg2;
  врати CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(struct Curl_cfilter *cf,
                            struct Curl_easy *data,
                            bool ignore_result,
                            цео event, цео arg1, void *arg2)
{
  CURLcode result = CURLE_OK;

  за(; cf; cf = cf->next) {
    ако(cf->cft->cntrl == Curl_cf_def_cntrl)
      continue;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    ако(!ignore_result && result)
      break;
  }
  врати result;
}

curl_socket_t Curl_conn_cf_get_socket(struct Curl_cfilter *cf,
                                      struct Curl_easy *data)
{
  curl_socket_t sock;
  ако(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, NULL, &sock))
    врати sock;
  врати CURL_SOCKET_BAD;
}

unsigned char Curl_conn_cf_get_transport(struct Curl_cfilter *cf,
                                         struct Curl_easy *data)
{
  цео transport = 0;
  ако(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, NULL))
    врати (unsigned char)transport;
  врати (unsigned char)(data->conn ? data->conn->transport_wanted : 0);
}

const char *Curl_conn_cf_get_alpn_negotiated(struct Curl_cfilter *cf,
                                             struct Curl_easy *data)
{
  const char *alpn = NULL;
  CURL_TRC_CF(data, cf, "query ALPN");
  ако(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, NULL,
                           CURL_UNCONST(&alpn)))
    врати alpn;
  врати NULL;
}

static const struct Curl_sockaddr_ex *cf_get_remote_addr(
  struct Curl_cfilter *cf, struct Curl_easy *data)
{
  const struct Curl_sockaddr_ex *remote_addr = NULL;
  ако(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, NULL,
                     CURL_UNCONST(&remote_addr)))
    врати remote_addr;
  врати NULL;
}

CURLcode Curl_conn_cf_get_ip_info(struct Curl_cfilter *cf,
                                  struct Curl_easy *data,
                                  bool *is_ipv6, struct ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  ако(cf) {
    цео ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  врати result;
}

curl_socket_t Curl_conn_get_first_socket(struct Curl_easy *data)
{
  struct Curl_cfilter *cf;

  ако(!data->conn)
    врати CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  ако(cf && !cf->connected)
    врати Curl_conn_cf_get_socket(cf, data);
  врати data->conn->sock[FIRSTSOCKET];
}

const struct Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  struct Curl_easy *data, int8_t sockindex)
{
  struct Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : NULL;
  врати cf ? cf_get_remote_addr(cf, data) : NULL;
}

CURLcode Curl_conn_ev_data_setup(struct Curl_easy *data)
{
  врати cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, NULL);
}

CURLcode Curl_conn_flush(struct Curl_easy *data, int8_t sockindex)
{
  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати CURLE_BAD_FUNCTION_ARGUMENT;
  врати Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
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
  врати cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, NULL);
}

bool Curl_conn_is_alive(struct Curl_easy *data, struct connectdata *conn,
                        bool *input_pending)
{
  struct Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  врати cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, input_pending);
}

CURLcode Curl_conn_keep_alive(struct Curl_easy *data,
                              struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  цео i;

  за(i = 0; (i < (цео)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    struct Curl_cfilter *cf = conn->cfilter[i];
    ако(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  врати result;
}

size_t Curl_conn_get_max_concurrent(struct Curl_easy *data,
                                    struct connectdata *conn,
                                    int8_t sockindex)
{
  struct Curl_cfilter *cf;
  CURLcode result;
  цео n = -1;

  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, NULL) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  врати (result || n < 0) ? 1 : (size_t)n;
}

цео Curl_conn_get_stream_error(struct Curl_easy *data,
                               struct connectdata *conn,
                               int8_t sockindex)
{
  struct Curl_cfilter *cf;
  CURLcode result;
  цео n = 0;

  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, NULL) : CURLE_UNKNOWN_OPTION;
  врати (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(struct Curl_easy *data, curl_socket_t sockfd)
{
  ако(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    врати SECONDARYSOCKET;
  врати FIRSTSOCKET;
}

CURLcode Curl_conn_recv(struct Curl_easy *data, int8_t sockindex,
                        char *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати CURLE_BAD_FUNCTION_ARGUMENT;
  ако(data && data->conn && data->conn->recv[sockindex])
    врати data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  врати CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(struct Curl_easy *data, int8_t sockindex,
                        const void *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  ако(!CONN_SOCK_IDX_VALID(sockindex))
    врати CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  ако(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    const char *p = getenv("CURL_SMALLSENDS");
    ако(p) {
      curl_off_t altsize;
      ако(!curlx_str_number(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        ако(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  ако(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    ако(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      врати CURLE_SEND_ERROR;
    }
    врати result;
  }
  *pnwritten = 0;
  врати CURLE_FAILED_INIT;
}
