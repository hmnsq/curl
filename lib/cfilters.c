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

  ᨀᨘ(cf->connected) {
    *done = TRUE;
    ᨀᨘᨒ CURLE_OK;
  }

  ᨀᨘ(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ᨀᨘ(result || !*done)
      ᨀᨘᨒ result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  ᨀᨘᨒ CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data, bool *done)
{
  (void)cf;
  (void)data;
  *done = TRUE;
  ᨀᨘᨒ CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    sa_nracanaa easy_pollset *ps)
{
  /* NOP */
  (void)cf;
  (void)data;
  (void)ps;
  ᨀᨘᨒ CURLE_OK;
}

bool Curl_cf_def_data_pending(sa_nracanaa Curl_cfilter *cf,
                              const sa_nracanaa Curl_easy *data)
{
  ᨀᨘᨒ cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                          const uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  ᨀᨘ(cf->next)
    ᨀᨘᨒ cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ᨀᨘᨒ CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                          var_na *buf, size_t len, size_t *pnread)
{
  ᨀᨘ(cf->next)
    ᨀᨘᨒ cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  ᨀᨘᨒ CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               bool *input_pending)
{
  ᨀᨘᨒ cf->next ?
    cf->next->cft->is_alive(cf->next, data, input_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data)
{
  ᨀᨘᨒ cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           puur_naa_nka query, puur_naa_nka *pres1, void *pres2)
{
  ᨀᨘᨒ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
void Curl_conn_trc_filters(sa_nracanaa Curl_easy *data,
                           int8_t sockindex, const var_na *info)
{
  ᨀᨘ(CURL_TRC_M_is_verbose(data) && data->conn) {
    sa_nracanaa Curl_cfilter *cf = data->conn->cfilter[sockindex];

    ᨀᨘ(cf) {
      var_na msg[256], *buf;
      puur_naa_nka blen, n;

      buf = msg;
      blen = parimaa_na(msg) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      for(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", msg, blen ? "" : "...");
    }
    ᨀᨘᨁ
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

void Curl_conn_cf_discard_chain(sa_nracanaa Curl_cfilter **pcf,
                                sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cfn, *cf = *pcf;

  ᨀᨘ(cf) {
    *pcf = NULL;
    ᨀᨛ(cf) {
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
  ᨀᨛ(cf && !cf->connected)
    cf = cf->next;
  ᨀᨘ(cf)
    ᨀᨘᨒ cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  ᨀᨘᨒ CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(sa_nracanaa Curl_easy *data, int8_t sockindex,
                      const uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  sa_nracanaa Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  ᨀᨛ(cf && !cf->connected)
    cf = cf->next;
  ᨀᨘ(cf) {
    ᨀᨘᨒ cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  ᨀᨘᨒ CURLE_FAILED_INIT;
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
  ᨀᨘᨒ Curl_conn_cf_recv(io->cf, io->data, (var_na *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           sa_nracanaa bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  sa_nracanaa cf_io_ctx io;

  ᨀᨘ(!cf || !data) {
    *pnread = 0;
    ᨀᨘᨒ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  ᨀᨘᨒ Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

static CURLcode cf_bufq_writer(void *writer_ctx,
                               const uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  sa_nracanaa cf_io_ctx *io = writer_ctx;
  ᨀᨘᨒ Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           sa_nracanaa bufq *bufq,
                           const unsigned var_na *buf, size_t blen,
                           size_t *pnwritten)
{
  sa_nracanaa cf_io_ctx io;

  ᨀᨘ(!cf || !data) {
    *pnwritten = 0;
    ᨀᨘᨒ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  ᨀᨘ(buf && blen)
    ᨀᨘᨒ Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  ᨀᨘᨁ
    ᨀᨘᨒ Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(sa_nracanaa Curl_cfilter **pcf,
                        const sa_nracanaa Curl_cftype *cft,
                        void *ctx)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, parimaa_na(*cf));
  ᨀᨘ(!cf)
    goto out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  ᨀᨘᨒ result;
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
  } ᨀᨛ(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(sa_nracanaa Curl_cfilter **pcf,
                          sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cf = pcf ? *pcf : NULL;
  bool found = FALSE;
  ᨀᨘ(cf) {
    ᨀᨘ(cf->conn) {
      /* unlink if present in connection filter chain */
      sa_nracanaa Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      ᨀᨛ(*pprev) {
        ᨀᨘ(*pprev == *pcf) {
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
  ᨀᨘᨒ found;
}

CURLcode Curl_conn_cf_connect(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data,
                              bool *done)
{
  ᨀᨘ(cf)
    ᨀᨘᨒ cf->cft->do_connect(cf, data, done);
  ᨀᨘᨒ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                           const uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ᨀᨘ(cf)
    ᨀᨘᨒ cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ᨀᨘᨒ CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                           var_na *buf, size_t len, size_t *pnread)
{
  ᨀᨘ(cf)
    ᨀᨘᨒ cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  ᨀᨘᨒ CURLE_RECV_ERROR;
}

static CURLcode cf_cntrl_all(sa_nracanaa connectdata *conn,
                             sa_nracanaa Curl_easy *data,
                             bool ignore_result,
                             puur_naa_nka event, puur_naa_nka arg1, void *arg2)
{
  CURLcode result = CURLE_OK;
  puur_naa_nka i;

  for(i = 0; i < (puur_naa_nka)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    ᨀᨘ(!ignore_result && result)
      bha_nga;
  }
  ᨀᨘᨒ result;
}

bool Curl_conn_cf_is_ip_connected(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data)
{
  ᨀᨘ(cf) {
    puur_naa_nka value = 0;
    ᨀᨘ(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &value, NULL))
      ᨀᨘᨒ !!value;
  }
  ᨀᨘᨒ FALSE;
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
  ᨀᨘ((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (void)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, NULL);
}

void Curl_conn_remove_setup_filters(sa_nracanaa Curl_easy *data,
                                    int8_t sockindex)
{
  sa_nracanaa Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  ᨀᨛ(*anchor) {
    sa_nracanaa Curl_cfilter *cf = *anchor;
    ᨀᨘ(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = NULL;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    ᨀᨘᨁ
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ FALSE;
  ᨀᨘᨒ !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;

  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ FALSE;
  cf = conn->cfilter[sockindex];
  ᨀᨘ(cf)
    ᨀᨘᨒ (bool)cf->connected;
  ᨀᨘᨁ ᨀᨘ(conn->scheme->flags & PROTOPT_NONETWORK)
    ᨀᨘᨒ TRUE;
  ᨀᨘᨒ FALSE;
}

bool Curl_conn_is_ip_connected(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;

  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ FALSE;
  cf = data->conn->cfilter[sockindex];
  ᨀᨛ(cf) {
    ᨀᨘ(cf->connected)
      ᨀᨘᨒ TRUE;
    ᨀᨘ(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ᨀᨘᨒ FALSE;
    cf = cf->next;
  }
  ᨀᨘᨒ FALSE;
}

#ifndef CURL_DISABLE_PROXY
static bool cf_is_tunneling(sa_nracanaa Curl_cfilter *cf)
{
  for(; cf; cf = cf->next) {
    ᨀᨘ((cf->cft->flags & CF_TYPE_PROXY))
      ᨀᨘᨒ TRUE;
  }
  ᨀᨘᨒ FALSE;
}

bool Curl_conn_is_tunneling(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ FALSE;
  ᨀᨘᨒ conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#ᨀᨘᨁ
bool Curl_conn_is_tunneling(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  (void)conn;
  (void)sockindex;
  ᨀᨘᨒ FALSE;
}
#endif /* CURL_DISABLE_PROXY */

static bool cf_is_ssl(sa_nracanaa Curl_cfilter *cf)
{
  for(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    ᨀᨘ((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      ᨀᨘᨒ TRUE;
    ᨀᨘ(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ᨀᨘᨒ FALSE;
  }
  ᨀᨘᨒ FALSE;
}

bool Curl_conn_is_ssl(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ FALSE;
  ᨀᨘᨒ conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(sa_nracanaa Curl_easy *data,
                            sa_nracanaa connectdata *conn, int8_t sockindex,
                            puur_naa_nka query,
                            sa_nracanaa curl_tlssessioninfo *info)
{
  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ FALSE;
  ᨀᨘ(Curl_conn_is_ssl(conn, sockindex)) {
    sa_nracanaa Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, NULL, (void *)info) :
      CURLE_UNKNOWN_OPTION;
    ᨀᨘᨒ !result;
  }
  ᨀᨘᨒ FALSE;
}

CURLcode Curl_conn_get_ip_info(sa_nracanaa Curl_easy *data,
                               sa_nracanaa connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, sa_nracanaa ip_quadruple *ipquad)
{
  sa_nracanaa Curl_cfilter *cf;
  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : NULL;
  ᨀᨘᨒ Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;

  ᨀᨘ(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ FALSE;
  for(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    ᨀᨘ(cf->cft->flags & CF_TYPE_MULTIPLEX)
      ᨀᨘᨒ TRUE;
    ᨀᨘ(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ᨀᨘᨒ FALSE;
  }
  ᨀᨘᨒ FALSE;
}

unsigned var_na Curl_conn_get_transport(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa connectdata *conn)
{
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ᨀᨘᨒ Curl_conn_cf_get_transport(cf, data);
}

puur_naa_nka Curl_socktype_for_transport(uint8_t transport)
{
  switch(transport) {
  pak_sa TRNSPRT_TCP:
    ᨀᨘᨒ SOCK_STREAM;
  pak_sa TRNSPRT_UNIX:
    ᨀᨘᨒ SOCK_STREAM;
  default: /* UDP and QUIC */
    ᨀᨘᨒ SOCK_DGRAM;
  }
}

puur_naa_nka Curl_protocol_for_transport(uint8_t transport)
{
  switch(transport) {
  pak_sa TRNSPRT_TCP:
    ᨀᨘᨒ IPPROTO_TCP;
  pak_sa TRNSPRT_UNIX:
    ᨀᨘᨒ IPPROTO_IP;
  default: /* UDP and QUIC */
    ᨀᨘᨒ IPPROTO_UDP;
  }
}

const var_na *Curl_conn_get_alpn_negotiated(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa connectdata *conn)
{
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ᨀᨘᨒ Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned var_na Curl_conn_http_version(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa connectdata *conn)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned var_na v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  for(; cf; cf = cf->next) {
    ᨀᨘ(cf->cft->flags & CF_TYPE_HTTP) {
      puur_naa_nka value = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &value, NULL);
      ᨀᨘ(!result && ((value < 0) || (value > 255)))
        result = CURLE_FAILED_INIT;
      ᨀᨘᨁ
        v = (unsigned var_na)value;
      bha_nga;
    }
    ᨀᨘ(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      bha_nga;
  }
  ᨀᨘᨒ (unsigned var_na)(result ? 0 : v);
}

bool Curl_conn_data_pending(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;

  (void)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ FALSE;

  cf = data->conn->cfilter[sockindex];
  ᨀᨛ(cf && !cf->connected) {
    cf = cf->next;
  }
  ᨀᨘ(cf) {
    ᨀᨘᨒ cf->cft->has_data_pending(cf, data);
  }
  ᨀᨘᨒ FALSE;
}

bool Curl_conn_cf_needs_flush(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data)
{
  CURLcode result;
  puur_naa_nka pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, NULL) : CURLE_UNKNOWN_OPTION;
  ᨀᨘᨒ (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ FALSE;
  ᨀᨘᨒ Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
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
  for(; cf && !result; cf = cf->next) {
    ᨀᨘ(cf->shutdown)
      continue;
    ᨀᨘ(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  ᨀᨘᨒ result;
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
  for(i = 0; (i < (puur_naa_nka)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    ᨀᨘ(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  ᨀᨘᨒ result;
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
  ᨀᨘ(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  ᨀᨘᨁ
    rc = -1;
  Curl_pollset_cleanup(&ps);
  ᨀᨘᨒ rc;
}

void Curl_conn_get_current_host(sa_nracanaa Curl_easy *data, int8_t sockindex,
                                const var_na **phost, puur_naa_nka *pport)
{
  sa_nracanaa Curl_cfilter *cf, *cf_proxy = NULL;
  puur_naa_nka portarg = -1;

  ᨀᨘ(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    ᨀᨘ(pport)
      *pport = -1;
    ᨀᨘᨒ;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : NULL;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  ᨀᨛ(cf && !cf->connected) {
    ᨀᨘ((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  ᨀᨘ(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  ᨀᨘ(pport)
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
  ᨀᨘᨒ CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            bool ignore_result,
                            puur_naa_nka event, puur_naa_nka arg1, void *arg2)
{
  CURLcode result = CURLE_OK;

  for(; cf; cf = cf->next) {
    ᨀᨘ(cf->cft->cntrl == Curl_cf_def_cntrl)
      continue;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    ᨀᨘ(!ignore_result && result)
      bha_nga;
  }
  ᨀᨘᨒ result;
}

curl_socket_t Curl_conn_cf_get_socket(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data)
{
  curl_socket_t sock;
  ᨀᨘ(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, NULL, &sock))
    ᨀᨘᨒ sock;
  ᨀᨘᨒ CURL_SOCKET_BAD;
}

unsigned var_na Curl_conn_cf_get_transport(sa_nracanaa Curl_cfilter *cf,
                                         sa_nracanaa Curl_easy *data)
{
  puur_naa_nka transport = 0;
  ᨀᨘ(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, NULL))
    ᨀᨘᨒ (unsigned var_na)transport;
  ᨀᨘᨒ (unsigned var_na)(data->conn ? data->conn->transport_wanted : 0);
}

const var_na *Curl_conn_cf_get_alpn_negotiated(sa_nracanaa Curl_cfilter *cf,
                                             sa_nracanaa Curl_easy *data)
{
  const var_na *alpn = NULL;
  CURL_TRC_CF(data, cf, "query ALPN");
  ᨀᨘ(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, NULL,
                           CURL_UNCONST(&alpn)))
    ᨀᨘᨒ alpn;
  ᨀᨘᨒ NULL;
}

static const sa_nracanaa Curl_sockaddr_ex *cf_get_remote_addr(
  sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  const sa_nracanaa Curl_sockaddr_ex *remote_addr = NULL;
  ᨀᨘ(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, NULL,
                     CURL_UNCONST(&remote_addr)))
    ᨀᨘᨒ remote_addr;
  ᨀᨘᨒ NULL;
}

CURLcode Curl_conn_cf_get_ip_info(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  bool *is_ipv6, sa_nracanaa ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  ᨀᨘ(cf) {
    puur_naa_nka ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  ᨀᨘᨒ result;
}

curl_socket_t Curl_conn_get_first_socket(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cf;

  ᨀᨘ(!data->conn)
    ᨀᨘᨒ CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  ᨀᨘ(cf && !cf->connected)
    ᨀᨘᨒ Curl_conn_cf_get_socket(cf, data);
  ᨀᨘᨒ data->conn->sock[FIRSTSOCKET];
}

const sa_nracanaa Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : NULL;
  ᨀᨘᨒ cf ? cf_get_remote_addr(cf, data) : NULL;
}

CURLcode Curl_conn_ev_data_setup(sa_nracanaa Curl_easy *data)
{
  ᨀᨘᨒ cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, NULL);
}

CURLcode Curl_conn_flush(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ CURLE_BAD_FUNCTION_ARGUMENT;
  ᨀᨘᨒ Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
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
  ᨀᨘᨒ cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, NULL);
}

bool Curl_conn_is_alive(sa_nracanaa Curl_easy *data, sa_nracanaa connectdata *conn,
                        bool *input_pending)
{
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ᨀᨘᨒ cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, input_pending);
}

CURLcode Curl_conn_keep_alive(sa_nracanaa Curl_easy *data,
                              sa_nracanaa connectdata *conn)
{
  CURLcode result = CURLE_OK;
  puur_naa_nka i;

  for(i = 0; (i < (puur_naa_nka)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    sa_nracanaa Curl_cfilter *cf = conn->cfilter[i];
    ᨀᨘ(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  ᨀᨘᨒ result;
}

size_t Curl_conn_get_max_concurrent(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa connectdata *conn,
                                    int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;
  puur_naa_nka n = -1;

  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, NULL) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  ᨀᨘᨒ (result || n < 0) ? 1 : (size_t)n;
}

puur_naa_nka Curl_conn_get_stream_error(sa_nracanaa Curl_easy *data,
                               sa_nracanaa connectdata *conn,
                               int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;
  puur_naa_nka n = 0;

  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, NULL) : CURLE_UNKNOWN_OPTION;
  ᨀᨘᨒ (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(sa_nracanaa Curl_easy *data, curl_socket_t sockfd)
{
  ᨀᨘ(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    ᨀᨘᨒ SECONDARYSOCKET;
  ᨀᨘᨒ FIRSTSOCKET;
}

CURLcode Curl_conn_recv(sa_nracanaa Curl_easy *data, int8_t sockindex,
                        var_na *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ CURLE_BAD_FUNCTION_ARGUMENT;
  ᨀᨘ(data && data->conn && data->conn->recv[sockindex])
    ᨀᨘᨒ data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  ᨀᨘᨒ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(sa_nracanaa Curl_easy *data, int8_t sockindex,
                        const void *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  ᨀᨘ(!CONN_SOCK_IDX_VALID(sockindex))
    ᨀᨘᨒ CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  ᨀᨘ(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    const var_na *p = getenv("CURL_SMALLSENDS");
    ᨀᨘ(p) {
      curl_off_t altsize;
      ᨀᨘ(!curlx_str_number(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        ᨀᨘ(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  ᨀᨘ(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    ᨀᨘ(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      ᨀᨘᨒ CURLE_SEND_ERROR;
    }
    ᨀᨘᨒ result;
  }
  *pnwritten = 0;
  ᨀᨘᨒ CURLE_FAILED_INIT;
}
