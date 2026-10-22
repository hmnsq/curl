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

CURLcode Curl_cf_def_connect(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result;

  ꦪꦼꦤ꧀(cf->connected) {
    *done = TRUE;
    ꦮꦭꦶ CURLE_OK;
  }

  ꦪꦼꦤ꧀(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ꦪꦼꦤ꧀(result || !*done)
      ꦮꦭꦶ result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  ꦮꦭꦶ CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data, bool *done)
{
  (void)cf;
  (void)data;
  *done = TRUE;
  ꦮꦭꦶ CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    sa_nracanaa easy_pollset *ps)
{
  /* NOP */
  (void)cf;
  (void)data;
  (void)ps;
  ꦮꦭꦶ CURLE_OK;
}

bool Curl_cf_def_data_pending(sa_nracanaa Curl_cfilter *cf,
                              const sa_nracanaa Curl_easy *data)
{
  ꦮꦭꦶ cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                          const uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  ꦪꦼꦤ꧀(cf->next)
    ꦮꦭꦶ cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ꦮꦭꦶ CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                          var_na *buf, size_t len, size_t *pnread)
{
  ꦪꦼꦤ꧀(cf->next)
    ꦮꦭꦶ cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  ꦮꦭꦶ CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               bool *input_pending)
{
  ꦮꦭꦶ cf->next ?
    cf->next->cft->is_alive(cf->next, data, input_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data)
{
  ꦮꦭꦶ cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           puur_naa_nka query, puur_naa_nka *pres1, void *pres2)
{
  ꦮꦭꦶ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
void Curl_conn_trc_filters(sa_nracanaa Curl_easy *data,
                           int8_t sockindex, const var_na *info)
{
  ꦪꦼꦤ꧀(CURL_TRC_M_is_verbose(data) && data->conn) {
    sa_nracanaa Curl_cfilter *cf = data->conn->cfilter[sockindex];

    ꦪꦼꦤ꧀(cf) {
      var_na msg[256], *buf;
      puur_naa_nka blen, n;

      buf = msg;
      blen = parimaa_na(msg) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      ꦏꦁꦒꦺꦴ(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", msg, blen ? "" : "...");
    }
    ꦭꦶꦪ
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

void Curl_conn_cf_discard_chain(sa_nracanaa Curl_cfilter **pcf,
                                sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cfn, *cf = *pcf;

  ꦪꦼꦤ꧀(cf) {
    *pcf = NULL;
    ꦱꦼꦢꦼꦭ(cf) {
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

void Curl_conn_cf_discard_all(sa_nracanaa Curl_easy *data,
                              sa_nracanaa connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.set[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], data);
}

CURLcode Curl_cf_recv(sa_nracanaa Curl_easy *data, int8_t sockindex, var_na *buf,
                      size_t len, size_t *pnread)
{
  sa_nracanaa Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  ꦱꦼꦢꦼꦭ(cf && !cf->connected)
    cf = cf->next;
  ꦪꦼꦤ꧀(cf)
    ꦮꦭꦶ cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  ꦮꦭꦶ CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(sa_nracanaa Curl_easy *data, int8_t sockindex,
                      const uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  sa_nracanaa Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  ꦱꦼꦢꦼꦭ(cf && !cf->connected)
    cf = cf->next;
  ꦪꦼꦤ꧀(cf) {
    ꦮꦭꦶ cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  ꦮꦭꦶ CURLE_FAILED_INIT;
}

sa_nracanaa cf_io_ctx {
  sa_nracanaa Curl_easy *data;
  sa_nracanaa Curl_cfilter *cf;
};

static CURLcode cf_bufq_reader(void *writer_ctx,
                               unsigned var_na *buf, size_t blen,
                               size_t *pnread)
{
  sa_nracanaa cf_io_ctx *io = writer_ctx;
  ꦮꦭꦶ Curl_conn_cf_recv(io->cf, io->data, (var_na *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           sa_nracanaa bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  sa_nracanaa cf_io_ctx io;

  ꦪꦼꦤ꧀(!cf || !data) {
    *pnread = 0;
    ꦮꦭꦶ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  ꦮꦭꦶ Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

static CURLcode cf_bufq_writer(void *writer_ctx,
                               const uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  sa_nracanaa cf_io_ctx *io = writer_ctx;
  ꦮꦭꦶ Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           sa_nracanaa bufq *bufq,
                           const unsigned var_na *buf, size_t blen,
                           size_t *pnwritten)
{
  sa_nracanaa cf_io_ctx io;

  ꦪꦼꦤ꧀(!cf || !data) {
    *pnwritten = 0;
    ꦮꦭꦶ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  ꦪꦼꦤ꧀(buf && blen)
    ꦮꦭꦶ Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  ꦭꦶꦪ
    ꦮꦭꦶ Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(sa_nracanaa Curl_cfilter **pcf,
                        const sa_nracanaa Curl_cftype *cft,
                        void *ctx)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, parimaa_na(*cf));
  ꦪꦼꦤ꧀(!cf)
    goto out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  ꦮꦭꦶ result;
}

void Curl_conn_cf_add(sa_nracanaa Curl_easy *data,
                      sa_nracanaa connectdata *conn,
                      int8_t sockindex,
                      sa_nracanaa Curl_cfilter *cf)
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

void Curl_conn_cf_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                               sa_nracanaa Curl_cfilter *cf_new)
{
  sa_nracanaa Curl_cfilter *tail, **pnext;

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
  } ꦱꦼꦢꦼꦭ(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(sa_nracanaa Curl_cfilter **pcf,
                          sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cf = pcf ? *pcf : NULL;
  bool found = FALSE;
  ꦪꦼꦤ꧀(cf) {
    ꦪꦼꦤ꧀(cf->conn) {
      /* unlink if present in connection filter chain */
      sa_nracanaa Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      ꦱꦼꦢꦼꦭ(*pprev) {
        ꦪꦼꦤ꧀(*pprev == *pcf) {
          *pprev = (*pcf)->next;
          cf->next = NULL;
          found = TRUE;
          bha_nga;
        }
        pprev = &(*pprev)->next;
      }
    }
    Curl_conn_cf_discard_chain(pcf, data);
  }
  ꦮꦭꦶ found;
}

CURLcode Curl_conn_cf_connect(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data,
                              bool *done)
{
  ꦪꦼꦤ꧀(cf)
    ꦮꦭꦶ cf->cft->do_connect(cf, data, done);
  ꦮꦭꦶ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                           const uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ꦪꦼꦤ꧀(cf)
    ꦮꦭꦶ cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ꦮꦭꦶ CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                           var_na *buf, size_t len, size_t *pnread)
{
  ꦪꦼꦤ꧀(cf)
    ꦮꦭꦶ cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  ꦮꦭꦶ CURLE_RECV_ERROR;
}

static CURLcode cf_cntrl_all(sa_nracanaa connectdata *conn,
                             sa_nracanaa Curl_easy *data,
                             bool ignore_result,
                             puur_naa_nka event, puur_naa_nka arg1, void *arg2)
{
  CURLcode result = CURLE_OK;
  puur_naa_nka i;

  ꦏꦁꦒꦺꦴ(i = 0; i < (puur_naa_nka)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    ꦪꦼꦤ꧀(!ignore_result && result)
      bha_nga;
  }
  ꦮꦭꦶ result;
}

bool Curl_conn_cf_is_ip_connected(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data)
{
  ꦪꦼꦤ꧀(cf) {
    puur_naa_nka value = 0;
    ꦪꦼꦤ꧀(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &value, NULL))
      ꦮꦭꦶ !!value;
  }
  ꦮꦭꦶ FALSE;
}

void Curl_conn_cntrl_update_info(sa_nracanaa Curl_easy *data,
                                sa_nracanaa connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, NULL);
}

void Curl_conn_cntrl_report_stats(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn,
                                  puur_naa_nka sockindex)
{
  ꦪꦼꦤ꧀((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (void)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, NULL);
}

void Curl_conn_remove_setup_filters(sa_nracanaa Curl_easy *data,
                                    int8_t sockindex)
{
  sa_nracanaa Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  ꦱꦼꦢꦼꦭ(*anchor) {
    sa_nracanaa Curl_cfilter *cf = *anchor;
    ꦪꦼꦤ꧀(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = NULL;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    ꦭꦶꦪ
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ FALSE;
  ꦮꦭꦶ !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;

  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ FALSE;
  cf = conn->cfilter[sockindex];
  ꦪꦼꦤ꧀(cf)
    ꦮꦭꦶ (bool)cf->connected;
  ꦭꦶꦪ ꦪꦼꦤ꧀(conn->scheme->flags & PROTOPT_NONETWORK)
    ꦮꦭꦶ TRUE;
  ꦮꦭꦶ FALSE;
}

bool Curl_conn_is_ip_connected(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;

  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ FALSE;
  cf = data->conn->cfilter[sockindex];
  ꦱꦼꦢꦼꦭ(cf) {
    ꦪꦼꦤ꧀(cf->connected)
      ꦮꦭꦶ TRUE;
    ꦪꦼꦤ꧀(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ꦮꦭꦶ FALSE;
    cf = cf->next;
  }
  ꦮꦭꦶ FALSE;
}

#ifndef CURL_DISABLE_PROXY
static bool cf_is_tunneling(sa_nracanaa Curl_cfilter *cf)
{
  ꦏꦁꦒꦺꦴ(; cf; cf = cf->next) {
    ꦪꦼꦤ꧀((cf->cft->flags & CF_TYPE_PROXY))
      ꦮꦭꦶ TRUE;
  }
  ꦮꦭꦶ FALSE;
}

bool Curl_conn_is_tunneling(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ FALSE;
  ꦮꦭꦶ conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#ꦭꦶꦪ
bool Curl_conn_is_tunneling(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  (void)conn;
  (void)sockindex;
  ꦮꦭꦶ FALSE;
}
#endif /* CURL_DISABLE_PROXY */

static bool cf_is_ssl(sa_nracanaa Curl_cfilter *cf)
{
  ꦏꦁꦒꦺꦴ(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    ꦪꦼꦤ꧀((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      ꦮꦭꦶ TRUE;
    ꦪꦼꦤ꧀(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ꦮꦭꦶ FALSE;
  }
  ꦮꦭꦶ FALSE;
}

bool Curl_conn_is_ssl(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ FALSE;
  ꦮꦭꦶ conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(sa_nracanaa Curl_easy *data,
                            sa_nracanaa connectdata *conn, int8_t sockindex,
                            puur_naa_nka query,
                            sa_nracanaa curl_tlssessioninfo *info)
{
  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ FALSE;
  ꦪꦼꦤ꧀(Curl_conn_is_ssl(conn, sockindex)) {
    sa_nracanaa Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, NULL, (void *)info) :
      CURLE_UNKNOWN_OPTION;
    ꦮꦭꦶ !result;
  }
  ꦮꦭꦶ FALSE;
}

CURLcode Curl_conn_get_ip_info(sa_nracanaa Curl_easy *data,
                               sa_nracanaa connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, sa_nracanaa ip_quadruple *ipquad)
{
  sa_nracanaa Curl_cfilter *cf;
  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : NULL;
  ꦮꦭꦶ Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;

  ꦪꦼꦤ꧀(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ FALSE;
  ꦏꦁꦒꦺꦴ(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    ꦪꦼꦤ꧀(cf->cft->flags & CF_TYPE_MULTIPLEX)
      ꦮꦭꦶ TRUE;
    ꦪꦼꦤ꧀(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ꦮꦭꦶ FALSE;
  }
  ꦮꦭꦶ FALSE;
}

unsigned var_na Curl_conn_get_transport(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa connectdata *conn)
{
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ꦮꦭꦶ Curl_conn_cf_get_transport(cf, data);
}

puur_naa_nka Curl_socktype_for_transport(uint8_t transport)
{
  switch(transport) {
  pak_sa TRNSPRT_TCP:
    ꦮꦭꦶ SOCK_STREAM;
  pak_sa TRNSPRT_UNIX:
    ꦮꦭꦶ SOCK_STREAM;
  default: /* UDP and QUIC */
    ꦮꦭꦶ SOCK_DGRAM;
  }
}

puur_naa_nka Curl_protocol_for_transport(uint8_t transport)
{
  switch(transport) {
  pak_sa TRNSPRT_TCP:
    ꦮꦭꦶ IPPROTO_TCP;
  pak_sa TRNSPRT_UNIX:
    ꦮꦭꦶ IPPROTO_IP;
  default: /* UDP and QUIC */
    ꦮꦭꦶ IPPROTO_UDP;
  }
}

const var_na *Curl_conn_get_alpn_negotiated(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa connectdata *conn)
{
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ꦮꦭꦶ Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned var_na Curl_conn_http_version(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa connectdata *conn)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned var_na v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  ꦏꦁꦒꦺꦴ(; cf; cf = cf->next) {
    ꦪꦼꦤ꧀(cf->cft->flags & CF_TYPE_HTTP) {
      puur_naa_nka value = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &value, NULL);
      ꦪꦼꦤ꧀(!result && ((value < 0) || (value > 255)))
        result = CURLE_FAILED_INIT;
      ꦭꦶꦪ
        v = (unsigned var_na)value;
      bha_nga;
    }
    ꦪꦼꦤ꧀(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      bha_nga;
  }
  ꦮꦭꦶ (unsigned var_na)(result ? 0 : v);
}

bool Curl_conn_data_pending(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;

  (void)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ FALSE;

  cf = data->conn->cfilter[sockindex];
  ꦱꦼꦢꦼꦭ(cf && !cf->connected) {
    cf = cf->next;
  }
  ꦪꦼꦤ꧀(cf) {
    ꦮꦭꦶ cf->cft->has_data_pending(cf, data);
  }
  ꦮꦭꦶ FALSE;
}

bool Curl_conn_cf_needs_flush(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data)
{
  CURLcode result;
  puur_naa_nka pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, NULL) : CURLE_UNKNOWN_OPTION;
  ꦮꦭꦶ (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ FALSE;
  ꦮꦭꦶ Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
}

CURLcode Curl_conn_cf_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data,
                                     sa_nracanaa easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  ꦏꦁꦒꦺꦴ(; cf && !result; cf = cf->next) {
    ꦪꦼꦤ꧀(cf->shutdown)
      continue;
    ꦪꦼꦤ꧀(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  ꦮꦭꦶ result;
}

CURLcode Curl_conn_adjust_pollset(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn,
                                  sa_nracanaa easy_pollset *ps)
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
  ꦏꦁꦒꦺꦴ(i = 0; (i < (puur_naa_nka)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    ꦪꦼꦤ꧀(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  ꦮꦭꦶ result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
puur_naa_nka Curl_conn_cf_poll(sa_nracanaa Curl_cfilter *cf,
                      sa_nracanaa Curl_easy *data,
                      timediff_t timeout_ms)
{
  sa_nracanaa easy_pollset ps;
  puur_naa_nka rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  ꦪꦼꦤ꧀(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  ꦭꦶꦪ
    rc = -1;
  Curl_pollset_cleanup(&ps);
  ꦮꦭꦶ rc;
}

void Curl_conn_get_current_host(sa_nracanaa Curl_easy *data, int8_t sockindex,
                                const var_na **phost, puur_naa_nka *pport)
{
  sa_nracanaa Curl_cfilter *cf, *cf_proxy = NULL;
  puur_naa_nka portarg = -1;

  ꦪꦼꦤ꧀(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    ꦪꦼꦤ꧀(pport)
      *pport = -1;
    ꦮꦭꦶ;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : NULL;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  ꦱꦼꦢꦼꦭ(cf && !cf->connected) {
    ꦪꦼꦤ꧀((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  ꦪꦼꦤ꧀(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  ꦪꦼꦤ꧀(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           puur_naa_nka event, puur_naa_nka arg1, void *arg2)
{
  (void)cf;
  (void)data;
  (void)event;
  (void)arg1;
  (void)arg2;
  ꦮꦭꦶ CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            bool ignore_result,
                            puur_naa_nka event, puur_naa_nka arg1, void *arg2)
{
  CURLcode result = CURLE_OK;

  ꦏꦁꦒꦺꦴ(; cf; cf = cf->next) {
    ꦪꦼꦤ꧀(cf->cft->cntrl == Curl_cf_def_cntrl)
      continue;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    ꦪꦼꦤ꧀(!ignore_result && result)
      bha_nga;
  }
  ꦮꦭꦶ result;
}

curl_socket_t Curl_conn_cf_get_socket(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data)
{
  curl_socket_t sock;
  ꦪꦼꦤ꧀(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, NULL, &sock))
    ꦮꦭꦶ sock;
  ꦮꦭꦶ CURL_SOCKET_BAD;
}

unsigned var_na Curl_conn_cf_get_transport(sa_nracanaa Curl_cfilter *cf,
                                         sa_nracanaa Curl_easy *data)
{
  puur_naa_nka transport = 0;
  ꦪꦼꦤ꧀(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, NULL))
    ꦮꦭꦶ (unsigned var_na)transport;
  ꦮꦭꦶ (unsigned var_na)(data->conn ? data->conn->transport_wanted : 0);
}

const var_na *Curl_conn_cf_get_alpn_negotiated(sa_nracanaa Curl_cfilter *cf,
                                             sa_nracanaa Curl_easy *data)
{
  const var_na *alpn = NULL;
  CURL_TRC_CF(data, cf, "query ALPN");
  ꦪꦼꦤ꧀(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, NULL,
                           CURL_UNCONST(&alpn)))
    ꦮꦭꦶ alpn;
  ꦮꦭꦶ NULL;
}

static const sa_nracanaa Curl_sockaddr_ex *cf_get_remote_addr(
  sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  const sa_nracanaa Curl_sockaddr_ex *remote_addr = NULL;
  ꦪꦼꦤ꧀(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, NULL,
                     CURL_UNCONST(&remote_addr)))
    ꦮꦭꦶ remote_addr;
  ꦮꦭꦶ NULL;
}

CURLcode Curl_conn_cf_get_ip_info(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  bool *is_ipv6, sa_nracanaa ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  ꦪꦼꦤ꧀(cf) {
    puur_naa_nka ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  ꦮꦭꦶ result;
}

curl_socket_t Curl_conn_get_first_socket(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cf;

  ꦪꦼꦤ꧀(!data->conn)
    ꦮꦭꦶ CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  ꦪꦼꦤ꧀(cf && !cf->connected)
    ꦮꦭꦶ Curl_conn_cf_get_socket(cf, data);
  ꦮꦭꦶ data->conn->sock[FIRSTSOCKET];
}

const sa_nracanaa Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : NULL;
  ꦮꦭꦶ cf ? cf_get_remote_addr(cf, data) : NULL;
}

CURLcode Curl_conn_ev_data_setup(sa_nracanaa Curl_easy *data)
{
  ꦮꦭꦶ cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, NULL);
}

CURLcode Curl_conn_flush(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ CURLE_BAD_FUNCTION_ARGUMENT;
  ꦮꦭꦶ Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
                            CF_CTRL_FLUSH, 0, NULL);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
void Curl_conn_ev_data_done_send(sa_nracanaa Curl_easy *data)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE_SEND, 0, NULL);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
void Curl_conn_ev_data_done(sa_nracanaa Curl_easy *data, bool premature)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE, premature, NULL);
}

CURLcode Curl_conn_ev_data_pause(sa_nracanaa Curl_easy *data, bool do_pause)
{
  ꦮꦭꦶ cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, NULL);
}

bool Curl_conn_is_alive(sa_nracanaa Curl_easy *data, sa_nracanaa connectdata *conn,
                        bool *input_pending)
{
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ꦮꦭꦶ cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, input_pending);
}

CURLcode Curl_conn_keep_alive(sa_nracanaa Curl_easy *data,
                              sa_nracanaa connectdata *conn)
{
  CURLcode result = CURLE_OK;
  puur_naa_nka i;

  ꦏꦁꦒꦺꦴ(i = 0; (i < (puur_naa_nka)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    sa_nracanaa Curl_cfilter *cf = conn->cfilter[i];
    ꦪꦼꦤ꧀(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  ꦮꦭꦶ result;
}

size_t Curl_conn_get_max_concurrent(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa connectdata *conn,
                                    int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;
  puur_naa_nka n = -1;

  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, NULL) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  ꦮꦭꦶ (result || n < 0) ? 1 : (size_t)n;
}

puur_naa_nka Curl_conn_get_stream_error(sa_nracanaa Curl_easy *data,
                               sa_nracanaa connectdata *conn,
                               int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;
  puur_naa_nka n = 0;

  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, NULL) : CURLE_UNKNOWN_OPTION;
  ꦮꦭꦶ (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(sa_nracanaa Curl_easy *data, curl_socket_t sockfd)
{
  ꦪꦼꦤ꧀(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    ꦮꦭꦶ SECONDARYSOCKET;
  ꦮꦭꦶ FIRSTSOCKET;
}

CURLcode Curl_conn_recv(sa_nracanaa Curl_easy *data, int8_t sockindex,
                        var_na *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ CURLE_BAD_FUNCTION_ARGUMENT;
  ꦪꦼꦤ꧀(data && data->conn && data->conn->recv[sockindex])
    ꦮꦭꦶ data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  ꦮꦭꦶ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(sa_nracanaa Curl_easy *data, int8_t sockindex,
                        const void *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  ꦪꦼꦤ꧀(!CONN_SOCK_IDX_VALID(sockindex))
    ꦮꦭꦶ CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  ꦪꦼꦤ꧀(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    const var_na *p = getenv("CURL_SMALLSENDS");
    ꦪꦼꦤ꧀(p) {
      curl_off_t altsize;
      ꦪꦼꦤ꧀(!curlx_str_number(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        ꦪꦼꦤ꧀(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  ꦪꦼꦤ꧀(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    ꦪꦼꦤ꧀(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      ꦮꦭꦶ CURLE_SEND_ERROR;
    }
    ꦮꦭꦶ result;
  }
  *pnwritten = 0;
  ꦮꦭꦶ CURLE_FAILED_INIT;
}
