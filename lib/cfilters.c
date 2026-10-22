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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "strerror.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "url.h"
#شامل "curl_trc.h"
#شامل "progress.h"
#شامل "select.h"
#شامل "curlx/strparse.h"

CURLcode Curl_cf_def_connect(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data, bool *done)
{
  CURLcode result;

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  اگر(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    اگر(result || !*done)
      بازگشت result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  بازگشت CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data, bool *done)
{
  (تهی)cf;
  (تهی)data;
  *done = TRUE;
  بازگشت CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    ساختار easy_pollset *ps)
{
  /* NOP */
  (تهی)cf;
  (تهی)data;
  (تهی)ps;
  بازگشت CURLE_OK;
}

bool Curl_cf_def_data_pending(ساختار Curl_cfilter *cf,
                              ثابت ساختار Curl_easy *data)
{
  بازگشت cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                          ثابت uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  اگر(cf->next)
    بازگشت cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  بازگشت CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                          نویسه *buf, size_t len, size_t *pnread)
{
  اگر(cf->next)
    بازگشت cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  بازگشت CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               bool *درون_داد_pending)
{
  بازگشت cf->next ?
    cf->next->cft->is_alive(cf->next, data, درون_داد_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(ساختار Curl_cfilter *cf,
                                     ساختار Curl_easy *data)
{
  بازگشت cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(ساختار Curl_cfilter *cf,
                           ساختار Curl_easy *data,
                           صحیح query, صحیح *pres1, تهی *pres2)
{
  بازگشت cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
تهی Curl_conn_trc_filters(ساختار Curl_easy *data,
                           int8_t sockindex, ثابت نویسه *info)
{
  اگر(CURL_TRC_M_is_verbose(data) && data->conn) {
    ساختار Curl_cfilter *cf = data->conn->cfilter[sockindex];

    اگر(cf) {
      نویسه پیام[256], *buf;
      صحیح blen, n;

      buf = پیام;
      blen = اندازه(پیام) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      برای(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", پیام, blen ? "" : "...");
    }
    وگرنه
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

تهی Curl_conn_cf_discard_chain(ساختار Curl_cfilter **pcf,
                                ساختار Curl_easy *data)
{
  ساختار Curl_cfilter *cfn, *cf = *pcf;

  اگر(cf) {
    *pcf = پوچ;
    وقتی(cf) {
      cfn = cf->next;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->next = پوچ;
      cf->cft->destroy(cf, data);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

تهی Curl_conn_cf_discard_all(ساختار Curl_easy *data,
                              ساختار connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.set[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], data);
}

CURLcode Curl_cf_recv(ساختار Curl_easy *data, int8_t sockindex, نویسه *buf,
                      size_t len, size_t *pnread)
{
  ساختار Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  وقتی(cf && !cf->connected)
    cf = cf->next;
  اگر(cf)
    بازگشت cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  بازگشت CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(ساختار Curl_easy *data, int8_t sockindex,
                      ثابت uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  ساختار Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  وقتی(cf && !cf->connected)
    cf = cf->next;
  اگر(cf) {
    بازگشت cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  بازگشت CURLE_FAILED_INIT;
}

ساختار cf_io_ctx {
  ساختار Curl_easy *data;
  ساختار Curl_cfilter *cf;
};

ایستا CURLcode cf_bufq_reader(تهی *writer_ctx,
                               unsigned نویسه *buf, size_t blen,
                               size_t *pnread)
{
  ساختار cf_io_ctx *io = writer_ctx;
  بازگشت Curl_conn_cf_recv(io->cf, io->data, (نویسه *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(ساختار Curl_cfilter *cf,
                           ساختار Curl_easy *data,
                           ساختار bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  ساختار cf_io_ctx io;

  اگر(!cf || !data) {
    *pnread = 0;
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  بازگشت Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

ایستا CURLcode cf_bufq_writer(تهی *writer_ctx,
                               ثابت uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  ساختار cf_io_ctx *io = writer_ctx;
  بازگشت Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(ساختار Curl_cfilter *cf,
                           ساختار Curl_easy *data,
                           ساختار bufq *bufq,
                           ثابت unsigned نویسه *buf, size_t blen,
                           size_t *pnwritten)
{
  ساختار cf_io_ctx io;

  اگر(!cf || !data) {
    *pnwritten = 0;
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  اگر(buf && blen)
    بازگشت Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  وگرنه
    بازگشت Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(ساختار Curl_cfilter **pcf,
                        ثابت ساختار Curl_cftype *cft,
                        تهی *ctx)
{
  ساختار Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, اندازه(*cf));
  اگر(!cf)
    برو out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  بازگشت result;
}

تهی Curl_conn_cf_add(ساختار Curl_easy *data,
                      ساختار connectdata *conn,
                      int8_t sockindex,
                      ساختار Curl_cfilter *cf)
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

تهی Curl_conn_cf_درج_after(ساختار Curl_cfilter *cf_at,
                               ساختار Curl_cfilter *cf_new)
{
  ساختار Curl_cfilter *tail, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  tail = cf_at->next;
  cf_at->next = cf_new;
  انجامبده {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->next;
    cf_new = cf_new->next;
  } وقتی(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(ساختار Curl_cfilter **pcf,
                          ساختار Curl_easy *data)
{
  ساختار Curl_cfilter *cf = pcf ? *pcf : پوچ;
  bool found = FALSE;
  اگر(cf) {
    اگر(cf->conn) {
      /* unlink if present in connection filter chain */
      ساختار Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      وقتی(*pprev) {
        اگر(*pprev == *pcf) {
          *pprev = (*pcf)->next;
          cf->next = پوچ;
          found = TRUE;
          بشکن;
        }
        pprev = &(*pprev)->next;
      }
    }
    Curl_conn_cf_discard_chain(pcf, data);
  }
  بازگشت found;
}

CURLcode Curl_conn_cf_connect(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              bool *done)
{
  اگر(cf)
    بازگشت cf->cft->do_connect(cf, data, done);
  بازگشت CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                           ثابت uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  اگر(cf)
    بازگشت cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  بازگشت CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                           نویسه *buf, size_t len, size_t *pnread)
{
  اگر(cf)
    بازگشت cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  بازگشت CURLE_RECV_ERROR;
}

ایستا CURLcode cf_cntrl_all(ساختار connectdata *conn,
                             ساختار Curl_easy *data,
                             bool ignore_result,
                             صحیح event, صحیح arg1, تهی *arg2)
{
  CURLcode result = CURLE_OK;
  صحیح i;

  برای(i = 0; i < (صحیح)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    اگر(!ignore_result && result)
      بشکن;
  }
  بازگشت result;
}

bool Curl_conn_cf_is_ip_connected(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data)
{
  اگر(cf) {
    صحیح مقدار = 0;
    اگر(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &مقدار, پوچ))
      بازگشت !!مقدار;
  }
  بازگشت FALSE;
}

تهی Curl_conn_cntrl_update_info(ساختار Curl_easy *data,
                                ساختار connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, پوچ);
}

تهی Curl_conn_cntrl_report_stats(ساختار Curl_easy *data,
                                  ساختار connectdata *conn,
                                  صحیح sockindex)
{
  اگر((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (تهی)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, پوچ);
}

تهی Curl_conn_remove_setup_filters(ساختار Curl_easy *data,
                                    int8_t sockindex)
{
  ساختار Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  وقتی(*anchor) {
    ساختار Curl_cfilter *cf = *anchor;
    اگر(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = پوچ;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    وگرنه
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(ساختار connectdata *conn, int8_t sockindex)
{
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت FALSE;
  بازگشت !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(ساختار connectdata *conn, int8_t sockindex)
{
  ساختار Curl_cfilter *cf;

  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت FALSE;
  cf = conn->cfilter[sockindex];
  اگر(cf)
    بازگشت (bool)cf->connected;
  وگرنه اگر(conn->scheme->flags & PROTOPT_NONETWORK)
    بازگشت TRUE;
  بازگشت FALSE;
}

bool Curl_conn_is_ip_connected(ساختار Curl_easy *data, int8_t sockindex)
{
  ساختار Curl_cfilter *cf;

  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت FALSE;
  cf = data->conn->cfilter[sockindex];
  وقتی(cf) {
    اگر(cf->connected)
      بازگشت TRUE;
    اگر(cf->cft->flags & CF_TYPE_IP_CONNECT)
      بازگشت FALSE;
    cf = cf->next;
  }
  بازگشت FALSE;
}

#ifndef CURL_DISABLE_PROXY
ایستا bool cf_is_tunneling(ساختار Curl_cfilter *cf)
{
  برای(; cf; cf = cf->next) {
    اگر((cf->cft->flags & CF_TYPE_PROXY))
      بازگشت TRUE;
  }
  بازگشت FALSE;
}

bool Curl_conn_is_tunneling(ساختار connectdata *conn, int8_t sockindex)
{
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت FALSE;
  بازگشت conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#وگرنه
bool Curl_conn_is_tunneling(ساختار connectdata *conn, int8_t sockindex)
{
  (تهی)conn;
  (تهی)sockindex;
  بازگشت FALSE;
}
#endif /* CURL_DISABLE_PROXY */

ایستا bool cf_is_ssl(ساختار Curl_cfilter *cf)
{
  برای(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    اگر((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      بازگشت TRUE;
    اگر(cf->cft->flags & CF_TYPE_IP_CONNECT)
      بازگشت FALSE;
  }
  بازگشت FALSE;
}

bool Curl_conn_is_ssl(ساختار connectdata *conn, int8_t sockindex)
{
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت FALSE;
  بازگشت conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(ساختار Curl_easy *data,
                            ساختار connectdata *conn, int8_t sockindex,
                            صحیح query,
                            ساختار curl_tlssessioninfo *info)
{
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت FALSE;
  اگر(Curl_conn_is_ssl(conn, sockindex)) {
    ساختار Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, پوچ, (تهی *)info) :
      CURLE_UNKNOWN_OPTION;
    بازگشت !result;
  }
  بازگشت FALSE;
}

CURLcode Curl_conn_get_ip_info(ساختار Curl_easy *data,
                               ساختار connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, ساختار ip_quadruple *ipquad)
{
  ساختار Curl_cfilter *cf;
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : پوچ;
  بازگشت Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(ساختار connectdata *conn, int8_t sockindex)
{
  ساختار Curl_cfilter *cf;

  اگر(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    بازگشت FALSE;
  برای(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    اگر(cf->cft->flags & CF_TYPE_MULTIPLEX)
      بازگشت TRUE;
    اگر(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      بازگشت FALSE;
  }
  بازگشت FALSE;
}

unsigned نویسه Curl_conn_get_transport(ساختار Curl_easy *data,
                                      ساختار connectdata *conn)
{
  ساختار Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  بازگشت Curl_conn_cf_get_transport(cf, data);
}

صحیح Curl_socktype_for_transport(uint8_t transport)
{
  انتخاب(transport) {
  حالت TRNSPRT_TCP:
    بازگشت SOCK_STREAM;
  حالت TRNSPRT_UNIX:
    بازگشت SOCK_STREAM;
  پیشفرض: /* UDP and QUIC */
    بازگشت SOCK_DGRAM;
  }
}

صحیح Curl_protocol_for_transport(uint8_t transport)
{
  انتخاب(transport) {
  حالت TRNSPRT_TCP:
    بازگشت IPPROTO_TCP;
  حالت TRNSPRT_UNIX:
    بازگشت IPPROTO_IP;
  پیشفرض: /* UDP and QUIC */
    بازگشت IPPROTO_UDP;
  }
}

ثابت نویسه *Curl_conn_get_alpn_negotiated(ساختار Curl_easy *data,
                                          ساختار connectdata *conn)
{
  ساختار Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  بازگشت Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned نویسه Curl_conn_http_version(ساختار Curl_easy *data,
                                     ساختار connectdata *conn)
{
  ساختار Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned نویسه v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  برای(; cf; cf = cf->next) {
    اگر(cf->cft->flags & CF_TYPE_HTTP) {
      صحیح مقدار = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &مقدار, پوچ);
      اگر(!result && ((مقدار < 0) || (مقدار > 255)))
        result = CURLE_FAILED_INIT;
      وگرنه
        v = (unsigned نویسه)مقدار;
      بشکن;
    }
    اگر(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      بشکن;
  }
  بازگشت (unsigned نویسه)(result ? 0 : v);
}

bool Curl_conn_data_pending(ساختار Curl_easy *data, int8_t sockindex)
{
  ساختار Curl_cfilter *cf;

  (تهی)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت FALSE;

  cf = data->conn->cfilter[sockindex];
  وقتی(cf && !cf->connected) {
    cf = cf->next;
  }
  اگر(cf) {
    بازگشت cf->cft->has_data_pending(cf, data);
  }
  بازگشت FALSE;
}

bool Curl_conn_cf_needs_flush(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data)
{
  CURLcode result;
  صحیح pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, پوچ) : CURLE_UNKNOWN_OPTION;
  بازگشت (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(ساختار Curl_easy *data, int8_t sockindex)
{
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت FALSE;
  بازگشت Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
}

CURLcode Curl_conn_cf_adjust_pollset(ساختار Curl_cfilter *cf,
                                     ساختار Curl_easy *data,
                                     ساختار easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  برای(; cf && !result; cf = cf->next) {
    اگر(cf->shutdown)
      ادامه;
    اگر(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  بازگشت result;
}

CURLcode Curl_conn_adjust_pollset(ساختار Curl_easy *data,
                                  ساختار connectdata *conn,
                                  ساختار easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  ثابت bool want_io = !!ps->n;
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
  برای(i = 0; (i < (صحیح)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    اگر(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  بازگشت result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
صحیح Curl_conn_cf_poll(ساختار Curl_cfilter *cf,
                      ساختار Curl_easy *data,
                      timediff_t timeout_ms)
{
  ساختار easy_pollset ps;
  صحیح rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  اگر(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  وگرنه
    rc = -1;
  Curl_pollset_cleanup(&ps);
  بازگشت rc;
}

تهی Curl_conn_get_current_host(ساختار Curl_easy *data, int8_t sockindex,
                                ثابت نویسه **phost, صحیح *pport)
{
  ساختار Curl_cfilter *cf, *cf_proxy = پوچ;
  صحیح portarg = -1;

  اگر(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    اگر(pport)
      *pport = -1;
    بازگشت;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : پوچ;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  وقتی(cf && !cf->connected) {
    اگر((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  اگر(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  اگر(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(ساختار Curl_cfilter *cf,
                           ساختار Curl_easy *data,
                           صحیح event, صحیح arg1, تهی *arg2)
{
  (تهی)cf;
  (تهی)data;
  (تهی)event;
  (تهی)arg1;
  (تهی)arg2;
  بازگشت CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data,
                            bool ignore_result,
                            صحیح event, صحیح arg1, تهی *arg2)
{
  CURLcode result = CURLE_OK;

  برای(; cf; cf = cf->next) {
    اگر(cf->cft->cntrl == Curl_cf_def_cntrl)
      ادامه;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    اگر(!ignore_result && result)
      بشکن;
  }
  بازگشت result;
}

curl_socket_t Curl_conn_cf_get_socket(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data)
{
  curl_socket_t sock;
  اگر(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, پوچ, &sock))
    بازگشت sock;
  بازگشت CURL_SOCKET_BAD;
}

unsigned نویسه Curl_conn_cf_get_transport(ساختار Curl_cfilter *cf,
                                         ساختار Curl_easy *data)
{
  صحیح transport = 0;
  اگر(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, پوچ))
    بازگشت (unsigned نویسه)transport;
  بازگشت (unsigned نویسه)(data->conn ? data->conn->transport_wanted : 0);
}

ثابت نویسه *Curl_conn_cf_get_alpn_negotiated(ساختار Curl_cfilter *cf,
                                             ساختار Curl_easy *data)
{
  ثابت نویسه *alpn = پوچ;
  CURL_TRC_CF(data, cf, "query ALPN");
  اگر(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, پوچ,
                           CURL_UNCONST(&alpn)))
    بازگشت alpn;
  بازگشت پوچ;
}

ایستا ثابت ساختار Curl_sockaddr_ex *cf_get_remote_addr(
  ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ثابت ساختار Curl_sockaddr_ex *remote_addr = پوچ;
  اگر(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, پوچ,
                     CURL_UNCONST(&remote_addr)))
    بازگشت remote_addr;
  بازگشت پوچ;
}

CURLcode Curl_conn_cf_get_ip_info(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  bool *is_ipv6, ساختار ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  اگر(cf) {
    صحیح ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  بازگشت result;
}

curl_socket_t Curl_conn_get_first_socket(ساختار Curl_easy *data)
{
  ساختار Curl_cfilter *cf;

  اگر(!data->conn)
    بازگشت CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  اگر(cf && !cf->connected)
    بازگشت Curl_conn_cf_get_socket(cf, data);
  بازگشت data->conn->sock[FIRSTSOCKET];
}

ثابت ساختار Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  ساختار Curl_easy *data, int8_t sockindex)
{
  ساختار Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : پوچ;
  بازگشت cf ? cf_get_remote_addr(cf, data) : پوچ;
}

CURLcode Curl_conn_ev_data_setup(ساختار Curl_easy *data)
{
  بازگشت cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, پوچ);
}

CURLcode Curl_conn_flush(ساختار Curl_easy *data, int8_t sockindex)
{
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  بازگشت Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
                            CF_CTRL_FLUSH, 0, پوچ);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
تهی Curl_conn_ev_data_done_send(ساختار Curl_easy *data)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE_SEND, 0, پوچ);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
تهی Curl_conn_ev_data_done(ساختار Curl_easy *data, bool premature)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE, premature, پوچ);
}

CURLcode Curl_conn_ev_data_pause(ساختار Curl_easy *data, bool do_pause)
{
  بازگشت cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, پوچ);
}

bool Curl_conn_is_alive(ساختار Curl_easy *data, ساختار connectdata *conn,
                        bool *درون_داد_pending)
{
  ساختار Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  بازگشت cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, درون_داد_pending);
}

CURLcode Curl_conn_keep_alive(ساختار Curl_easy *data,
                              ساختار connectdata *conn)
{
  CURLcode result = CURLE_OK;
  صحیح i;

  برای(i = 0; (i < (صحیح)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    ساختار Curl_cfilter *cf = conn->cfilter[i];
    اگر(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  بازگشت result;
}

size_t Curl_conn_get_max_concurrent(ساختار Curl_easy *data,
                                    ساختار connectdata *conn,
                                    int8_t sockindex)
{
  ساختار Curl_cfilter *cf;
  CURLcode result;
  صحیح n = -1;

  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, پوچ) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  بازگشت (result || n < 0) ? 1 : (size_t)n;
}

صحیح Curl_conn_get_stream_error(ساختار Curl_easy *data,
                               ساختار connectdata *conn,
                               int8_t sockindex)
{
  ساختار Curl_cfilter *cf;
  CURLcode result;
  صحیح n = 0;

  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, پوچ) : CURLE_UNKNOWN_OPTION;
  بازگشت (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(ساختار Curl_easy *data, curl_socket_t sockfd)
{
  اگر(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    بازگشت SECONDARYSOCKET;
  بازگشت FIRSTSOCKET;
}

CURLcode Curl_conn_recv(ساختار Curl_easy *data, int8_t sockindex,
                        نویسه *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  اگر(data && data->conn && data->conn->recv[sockindex])
    بازگشت data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  بازگشت CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(ساختار Curl_easy *data, int8_t sockindex,
                        ثابت تهی *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  اگر(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    ثابت نویسه *p = getenv("CURL_SMALLSENDS");
    اگر(p) {
      curl_off_t altsize;
      اگر(!curlx_str_عدد(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        اگر(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  اگر(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    اگر(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      بازگشت CURLE_SEND_ERROR;
    }
    بازگشت result;
  }
  *pnwritten = 0;
  بازگشت CURLE_FAILED_INIT;
}
