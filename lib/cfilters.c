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

  အကယ်၍(cf->connected) {
    *done = TRUE;
    ပြန်ပေး CURLE_OK;
  }

  အကယ်၍(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    အကယ်၍(result || !*done)
      ပြန်ပေး result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  ပြန်ပေး CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data, bool *done)
{
  (void)cf;
  (void)data;
  *done = TRUE;
  ပြန်ပေး CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    sa_nracanaa easy_pollset *ps)
{
  /* NOP */
  (void)cf;
  (void)data;
  (void)ps;
  ပြန်ပေး CURLE_OK;
}

bool Curl_cf_def_data_pending(sa_nracanaa Curl_cfilter *cf,
                              const sa_nracanaa Curl_easy *data)
{
  ပြန်ပေး cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                          const uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  အကယ်၍(cf->next)
    ပြန်ပေး cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ပြန်ပေး CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                          var_na *buf, size_t len, size_t *pnread)
{
  အကယ်၍(cf->next)
    ပြန်ပေး cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  ပြန်ပေး CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               bool *input_pending)
{
  ပြန်ပေး cf->next ?
    cf->next->cft->is_alive(cf->next, data, input_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data)
{
  ပြန်ပေး cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           ကိန်းပြည့် query, ကိန်းပြည့် *pres1, void *pres2)
{
  ပြန်ပေး cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
void Curl_conn_trc_filters(sa_nracanaa Curl_easy *data,
                           int8_t sockindex, const var_na *info)
{
  အကယ်၍(CURL_TRC_M_is_verbose(data) && data->conn) {
    sa_nracanaa Curl_cfilter *cf = data->conn->cfilter[sockindex];

    အကယ်၍(cf) {
      var_na msg[256], *buf;
      ကိန်းပြည့် blen, n;

      buf = msg;
      blen = parimaa_na(msg) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      အတွက်(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", msg, blen ? "" : "...");
    }
    မဟုတ်ပါက
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

void Curl_conn_cf_discard_chain(sa_nracanaa Curl_cfilter **pcf,
                                sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cfn, *cf = *pcf;

  အကယ်၍(cf) {
    *pcf = NULL;
    နေစဉ်(cf) {
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
  နေစဉ်(cf && !cf->connected)
    cf = cf->next;
  အကယ်၍(cf)
    ပြန်ပေး cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  ပြန်ပေး CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(sa_nracanaa Curl_easy *data, int8_t sockindex,
                      const uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  sa_nracanaa Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  နေစဉ်(cf && !cf->connected)
    cf = cf->next;
  အကယ်၍(cf) {
    ပြန်ပေး cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  ပြန်ပေး CURLE_FAILED_INIT;
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
  ပြန်ပေး Curl_conn_cf_recv(io->cf, io->data, (var_na *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           sa_nracanaa bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  sa_nracanaa cf_io_ctx io;

  အကယ်၍(!cf || !data) {
    *pnread = 0;
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  ပြန်ပေး Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

static CURLcode cf_bufq_writer(void *writer_ctx,
                               const uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  sa_nracanaa cf_io_ctx *io = writer_ctx;
  ပြန်ပေး Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           sa_nracanaa bufq *bufq,
                           const unsigned var_na *buf, size_t blen,
                           size_t *pnwritten)
{
  sa_nracanaa cf_io_ctx io;

  အကယ်၍(!cf || !data) {
    *pnwritten = 0;
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  အကယ်၍(buf && blen)
    ပြန်ပေး Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  မဟုတ်ပါက
    ပြန်ပေး Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(sa_nracanaa Curl_cfilter **pcf,
                        const sa_nracanaa Curl_cftype *cft,
                        void *ctx)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, parimaa_na(*cf));
  အကယ်၍(!cf)
    goto out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  ပြန်ပေး result;
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
  } နေစဉ်(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(sa_nracanaa Curl_cfilter **pcf,
                          sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cf = pcf ? *pcf : NULL;
  bool found = FALSE;
  အကယ်၍(cf) {
    အကယ်၍(cf->conn) {
      /* unlink if present in connection filter chain */
      sa_nracanaa Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      နေစဉ်(*pprev) {
        အကယ်၍(*pprev == *pcf) {
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
  ပြန်ပေး found;
}

CURLcode Curl_conn_cf_connect(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data,
                              bool *done)
{
  အကယ်၍(cf)
    ပြန်ပေး cf->cft->do_connect(cf, data, done);
  ပြန်ပေး CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                           const uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  အကယ်၍(cf)
    ပြန်ပေး cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ပြန်ပေး CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                           var_na *buf, size_t len, size_t *pnread)
{
  အကယ်၍(cf)
    ပြန်ပေး cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  ပြန်ပေး CURLE_RECV_ERROR;
}

static CURLcode cf_cntrl_all(sa_nracanaa connectdata *conn,
                             sa_nracanaa Curl_easy *data,
                             bool ignore_result,
                             ကိန်းပြည့် event, ကိန်းပြည့် arg1, void *arg2)
{
  CURLcode result = CURLE_OK;
  ကိန်းပြည့် i;

  အတွက်(i = 0; i < (ကိန်းပြည့်)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    အကယ်၍(!ignore_result && result)
      bha_nga;
  }
  ပြန်ပေး result;
}

bool Curl_conn_cf_is_ip_connected(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data)
{
  အကယ်၍(cf) {
    ကိန်းပြည့် value = 0;
    အကယ်၍(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &value, NULL))
      ပြန်ပေး !!value;
  }
  ပြန်ပေး FALSE;
}

void Curl_conn_cntrl_update_info(sa_nracanaa Curl_easy *data,
                                sa_nracanaa connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, NULL);
}

void Curl_conn_cntrl_report_stats(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn,
                                  ကိန်းပြည့် sockindex)
{
  အကယ်၍((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (void)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, NULL);
}

void Curl_conn_remove_setup_filters(sa_nracanaa Curl_easy *data,
                                    int8_t sockindex)
{
  sa_nracanaa Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  နေစဉ်(*anchor) {
    sa_nracanaa Curl_cfilter *cf = *anchor;
    အကယ်၍(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = NULL;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    မဟုတ်ပါက
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး FALSE;
  ပြန်ပေး !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;

  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး FALSE;
  cf = conn->cfilter[sockindex];
  အကယ်၍(cf)
    ပြန်ပေး (bool)cf->connected;
  မဟုတ်ပါက အကယ်၍(conn->scheme->flags & PROTOPT_NONETWORK)
    ပြန်ပေး TRUE;
  ပြန်ပေး FALSE;
}

bool Curl_conn_is_ip_connected(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;

  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး FALSE;
  cf = data->conn->cfilter[sockindex];
  နေစဉ်(cf) {
    အကယ်၍(cf->connected)
      ပြန်ပေး TRUE;
    အကယ်၍(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ပြန်ပေး FALSE;
    cf = cf->next;
  }
  ပြန်ပေး FALSE;
}

#ifndef CURL_DISABLE_PROXY
static bool cf_is_tunneling(sa_nracanaa Curl_cfilter *cf)
{
  အတွက်(; cf; cf = cf->next) {
    အကယ်၍((cf->cft->flags & CF_TYPE_PROXY))
      ပြန်ပေး TRUE;
  }
  ပြန်ပေး FALSE;
}

bool Curl_conn_is_tunneling(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး FALSE;
  ပြန်ပေး conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#မဟုတ်ပါက
bool Curl_conn_is_tunneling(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  (void)conn;
  (void)sockindex;
  ပြန်ပေး FALSE;
}
#endif /* CURL_DISABLE_PROXY */

static bool cf_is_ssl(sa_nracanaa Curl_cfilter *cf)
{
  အတွက်(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    အကယ်၍((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      ပြန်ပေး TRUE;
    အကယ်၍(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ပြန်ပေး FALSE;
  }
  ပြန်ပေး FALSE;
}

bool Curl_conn_is_ssl(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး FALSE;
  ပြန်ပေး conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(sa_nracanaa Curl_easy *data,
                            sa_nracanaa connectdata *conn, int8_t sockindex,
                            ကိန်းပြည့် query,
                            sa_nracanaa curl_tlssessioninfo *info)
{
  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး FALSE;
  အကယ်၍(Curl_conn_is_ssl(conn, sockindex)) {
    sa_nracanaa Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, NULL, (void *)info) :
      CURLE_UNKNOWN_OPTION;
    ပြန်ပေး !result;
  }
  ပြန်ပေး FALSE;
}

CURLcode Curl_conn_get_ip_info(sa_nracanaa Curl_easy *data,
                               sa_nracanaa connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, sa_nracanaa ip_quadruple *ipquad)
{
  sa_nracanaa Curl_cfilter *cf;
  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : NULL;
  ပြန်ပေး Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(sa_nracanaa connectdata *conn, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;

  အကယ်၍(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး FALSE;
  အတွက်(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    အကယ်၍(cf->cft->flags & CF_TYPE_MULTIPLEX)
      ပြန်ပေး TRUE;
    အကယ်၍(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ပြန်ပေး FALSE;
  }
  ပြန်ပေး FALSE;
}

unsigned var_na Curl_conn_get_transport(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa connectdata *conn)
{
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ပြန်ပေး Curl_conn_cf_get_transport(cf, data);
}

ကိန်းပြည့် Curl_socktype_for_transport(uint8_t transport)
{
  switch(transport) {
  pak_sa TRNSPRT_TCP:
    ပြန်ပေး SOCK_STREAM;
  pak_sa TRNSPRT_UNIX:
    ပြန်ပေး SOCK_STREAM;
  default: /* UDP and QUIC */
    ပြန်ပေး SOCK_DGRAM;
  }
}

ကိန်းပြည့် Curl_protocol_for_transport(uint8_t transport)
{
  switch(transport) {
  pak_sa TRNSPRT_TCP:
    ပြန်ပေး IPPROTO_TCP;
  pak_sa TRNSPRT_UNIX:
    ပြန်ပေး IPPROTO_IP;
  default: /* UDP and QUIC */
    ပြန်ပေး IPPROTO_UDP;
  }
}

const var_na *Curl_conn_get_alpn_negotiated(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa connectdata *conn)
{
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ပြန်ပေး Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned var_na Curl_conn_http_version(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa connectdata *conn)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned var_na v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  အတွက်(; cf; cf = cf->next) {
    အကယ်၍(cf->cft->flags & CF_TYPE_HTTP) {
      ကိန်းပြည့် value = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &value, NULL);
      အကယ်၍(!result && ((value < 0) || (value > 255)))
        result = CURLE_FAILED_INIT;
      မဟုတ်ပါက
        v = (unsigned var_na)value;
      bha_nga;
    }
    အကယ်၍(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      bha_nga;
  }
  ပြန်ပေး (unsigned var_na)(result ? 0 : v);
}

bool Curl_conn_data_pending(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;

  (void)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး FALSE;

  cf = data->conn->cfilter[sockindex];
  နေစဉ်(cf && !cf->connected) {
    cf = cf->next;
  }
  အကယ်၍(cf) {
    ပြန်ပေး cf->cft->has_data_pending(cf, data);
  }
  ပြန်ပေး FALSE;
}

bool Curl_conn_cf_needs_flush(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data)
{
  CURLcode result;
  ကိန်းပြည့် pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, NULL) : CURLE_UNKNOWN_OPTION;
  ပြန်ပေး (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး FALSE;
  ပြန်ပေး Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
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
  အတွက်(; cf && !result; cf = cf->next) {
    အကယ်၍(cf->shutdown)
      continue;
    အကယ်၍(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  ပြန်ပေး result;
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
  အတွက်(i = 0; (i < (ကိန်းပြည့်)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    အကယ်၍(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  ပြန်ပေး result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
ကိန်းပြည့် Curl_conn_cf_poll(sa_nracanaa Curl_cfilter *cf,
                      sa_nracanaa Curl_easy *data,
                      timediff_t timeout_ms)
{
  sa_nracanaa easy_pollset ps;
  ကိန်းပြည့် rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  အကယ်၍(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  မဟုတ်ပါက
    rc = -1;
  Curl_pollset_cleanup(&ps);
  ပြန်ပေး rc;
}

void Curl_conn_get_current_host(sa_nracanaa Curl_easy *data, int8_t sockindex,
                                const var_na **phost, ကိန်းပြည့် *pport)
{
  sa_nracanaa Curl_cfilter *cf, *cf_proxy = NULL;
  ကိန်းပြည့် portarg = -1;

  အကယ်၍(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    အကယ်၍(pport)
      *pport = -1;
    ပြန်ပေး;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : NULL;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  နေစဉ်(cf && !cf->connected) {
    အကယ်၍((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  အကယ်၍(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  အကယ်၍(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           ကိန်းပြည့် event, ကိန်းပြည့် arg1, void *arg2)
{
  (void)cf;
  (void)data;
  (void)event;
  (void)arg1;
  (void)arg2;
  ပြန်ပေး CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            bool ignore_result,
                            ကိန်းပြည့် event, ကိန်းပြည့် arg1, void *arg2)
{
  CURLcode result = CURLE_OK;

  အတွက်(; cf; cf = cf->next) {
    အကယ်၍(cf->cft->cntrl == Curl_cf_def_cntrl)
      continue;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    အကယ်၍(!ignore_result && result)
      bha_nga;
  }
  ပြန်ပေး result;
}

curl_socket_t Curl_conn_cf_get_socket(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data)
{
  curl_socket_t sock;
  အကယ်၍(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, NULL, &sock))
    ပြန်ပေး sock;
  ပြန်ပေး CURL_SOCKET_BAD;
}

unsigned var_na Curl_conn_cf_get_transport(sa_nracanaa Curl_cfilter *cf,
                                         sa_nracanaa Curl_easy *data)
{
  ကိန်းပြည့် transport = 0;
  အကယ်၍(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, NULL))
    ပြန်ပေး (unsigned var_na)transport;
  ပြန်ပေး (unsigned var_na)(data->conn ? data->conn->transport_wanted : 0);
}

const var_na *Curl_conn_cf_get_alpn_negotiated(sa_nracanaa Curl_cfilter *cf,
                                             sa_nracanaa Curl_easy *data)
{
  const var_na *alpn = NULL;
  CURL_TRC_CF(data, cf, "query ALPN");
  အကယ်၍(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, NULL,
                           CURL_UNCONST(&alpn)))
    ပြန်ပေး alpn;
  ပြန်ပေး NULL;
}

static const sa_nracanaa Curl_sockaddr_ex *cf_get_remote_addr(
  sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  const sa_nracanaa Curl_sockaddr_ex *remote_addr = NULL;
  အကယ်၍(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, NULL,
                     CURL_UNCONST(&remote_addr)))
    ပြန်ပေး remote_addr;
  ပြန်ပေး NULL;
}

CURLcode Curl_conn_cf_get_ip_info(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  bool *is_ipv6, sa_nracanaa ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  အကယ်၍(cf) {
    ကိန်းပြည့် ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  ပြန်ပေး result;
}

curl_socket_t Curl_conn_get_first_socket(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cf;

  အကယ်၍(!data->conn)
    ပြန်ပေး CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  အကယ်၍(cf && !cf->connected)
    ပြန်ပေး Curl_conn_cf_get_socket(cf, data);
  ပြန်ပေး data->conn->sock[FIRSTSOCKET];
}

const sa_nracanaa Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : NULL;
  ပြန်ပေး cf ? cf_get_remote_addr(cf, data) : NULL;
}

CURLcode Curl_conn_ev_data_setup(sa_nracanaa Curl_easy *data)
{
  ပြန်ပေး cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, NULL);
}

CURLcode Curl_conn_flush(sa_nracanaa Curl_easy *data, int8_t sockindex)
{
  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
  ပြန်ပေး Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
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
  ပြန်ပေး cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, NULL);
}

bool Curl_conn_is_alive(sa_nracanaa Curl_easy *data, sa_nracanaa connectdata *conn,
                        bool *input_pending)
{
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ပြန်ပေး cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, input_pending);
}

CURLcode Curl_conn_keep_alive(sa_nracanaa Curl_easy *data,
                              sa_nracanaa connectdata *conn)
{
  CURLcode result = CURLE_OK;
  ကိန်းပြည့် i;

  အတွက်(i = 0; (i < (ကိန်းပြည့်)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    sa_nracanaa Curl_cfilter *cf = conn->cfilter[i];
    အကယ်၍(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  ပြန်ပေး result;
}

size_t Curl_conn_get_max_concurrent(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa connectdata *conn,
                                    int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;
  ကိန်းပြည့် n = -1;

  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, NULL) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  ပြန်ပေး (result || n < 0) ? 1 : (size_t)n;
}

ကိန်းပြည့် Curl_conn_get_stream_error(sa_nracanaa Curl_easy *data,
                               sa_nracanaa connectdata *conn,
                               int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;
  ကိန်းပြည့် n = 0;

  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, NULL) : CURLE_UNKNOWN_OPTION;
  ပြန်ပေး (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(sa_nracanaa Curl_easy *data, curl_socket_t sockfd)
{
  အကယ်၍(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    ပြန်ပေး SECONDARYSOCKET;
  ပြန်ပေး FIRSTSOCKET;
}

CURLcode Curl_conn_recv(sa_nracanaa Curl_easy *data, int8_t sockindex,
                        var_na *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
  အကယ်၍(data && data->conn && data->conn->recv[sockindex])
    ပြန်ပေး data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  ပြန်ပေး CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(sa_nracanaa Curl_easy *data, int8_t sockindex,
                        const void *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  အကယ်၍(!CONN_SOCK_IDX_VALID(sockindex))
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  အကယ်၍(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    const var_na *p = getenv("CURL_SMALLSENDS");
    အကယ်၍(p) {
      curl_off_t altsize;
      အကယ်၍(!curlx_str_number(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        အကယ်၍(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  အကယ်၍(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    အကယ်၍(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      ပြန်ပေး CURLE_SEND_ERROR;
    }
    ပြန်ပေး result;
  }
  *pnwritten = 0;
  ပြန်ပေး CURLE_FAILED_INIT;
}
