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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "strerror.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "url.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "select.h"
#ضمن "curlx/strparse.h"

CURLcode Curl_cf_def_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data, bool *done)
{
  CURLcode result;

  إذا(cf->connected) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  إذا(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    إذا(result || !*done)
      أرجع result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  أرجع CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data, bool *done)
{
  (فراغ)cf;
  (فراغ)data;
  *done = TRUE;
  أرجع CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    بنية easy_pollset *ps)
{
  /* NOP */
  (فراغ)cf;
  (فراغ)data;
  (فراغ)ps;
  أرجع CURLE_OK;
}

bool Curl_cf_def_data_pending(بنية Curl_cfilter *cf,
                              ثابت بنية Curl_easy *data)
{
  أرجع cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                          ثابت uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  إذا(cf->next)
    أرجع cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  أرجع CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                          حرف *buf, size_t len, size_t *pnread)
{
  إذا(cf->next)
    أرجع cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  أرجع CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               bool *شغرو_pending)
{
  أرجع cf->next ?
    cf->next->cft->is_alive(cf->next, data, شغرو_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *data)
{
  أرجع cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(بنية Curl_cfilter *cf,
                           بنية Curl_easy *data,
                           صحيح query, صحيح *pres1, فراغ *pres2)
{
  أرجع cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
فراغ Curl_conn_trc_filters(بنية Curl_easy *data,
                           int8_t sockindex, ثابت حرف *info)
{
  إذا(CURL_TRC_M_is_verbose(data) && data->conn) {
    بنية Curl_cfilter *cf = data->conn->cfilter[sockindex];

    إذا(cf) {
      حرف سكو[256], *buf;
      صحيح blen, n;

      buf = سكو;
      blen = حجم(سكو) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      لكل(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", سكو, blen ? "" : "...");
    }
    وإلا
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

فراغ Curl_conn_cf_discard_chain(بنية Curl_cfilter **pcf,
                                بنية Curl_easy *data)
{
  بنية Curl_cfilter *cfn, *cf = *pcf;

  إذا(cf) {
    *pcf = عدم;
    طالما(cf) {
      cfn = cf->next;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->next = عدم;
      cf->cft->destroy(cf, data);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

فراغ Curl_conn_cf_discard_all(بنية Curl_easy *data,
                              بنية connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.set[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], data);
}

CURLcode Curl_cf_recv(بنية Curl_easy *data, int8_t sockindex, حرف *buf,
                      size_t len, size_t *pnread)
{
  بنية Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  طالما(cf && !cf->connected)
    cf = cf->next;
  إذا(cf)
    أرجع cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  أرجع CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(بنية Curl_easy *data, int8_t sockindex,
                      ثابت uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  بنية Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  طالما(cf && !cf->connected)
    cf = cf->next;
  إذا(cf) {
    أرجع cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  أرجع CURLE_FAILED_INIT;
}

بنية cf_io_ctx {
  بنية Curl_easy *data;
  بنية Curl_cfilter *cf;
};

ساكن CURLcode cf_bufq_reader(فراغ *writer_ctx,
                               unsigned حرف *buf, size_t blen,
                               size_t *pnread)
{
  بنية cf_io_ctx *io = writer_ctx;
  أرجع Curl_conn_cf_recv(io->cf, io->data, (حرف *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(بنية Curl_cfilter *cf,
                           بنية Curl_easy *data,
                           بنية bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  بنية cf_io_ctx io;

  إذا(!cf || !data) {
    *pnread = 0;
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  أرجع Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

ساكن CURLcode cf_bufq_writer(فراغ *writer_ctx,
                               ثابت uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  بنية cf_io_ctx *io = writer_ctx;
  أرجع Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(بنية Curl_cfilter *cf,
                           بنية Curl_easy *data,
                           بنية bufq *bufq,
                           ثابت unsigned حرف *buf, size_t blen,
                           size_t *pnwritten)
{
  بنية cf_io_ctx io;

  إذا(!cf || !data) {
    *pnwritten = 0;
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  إذا(buf && blen)
    أرجع Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  وإلا
    أرجع Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(بنية Curl_cfilter **pcf,
                        ثابت بنية Curl_cftype *cft,
                        فراغ *ctx)
{
  بنية Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, حجم(*cf));
  إذا(!cf)
    اذهب out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  أرجع result;
}

فراغ Curl_conn_cf_add(بنية Curl_easy *data,
                      بنية connectdata *conn,
                      int8_t sockindex,
                      بنية Curl_cfilter *cf)
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

فراغ Curl_conn_cf_سك_after(بنية Curl_cfilter *cf_at,
                               بنية Curl_cfilter *cf_new)
{
  بنية Curl_cfilter *tail, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  tail = cf_at->next;
  cf_at->next = cf_new;
  افعل {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->next;
    cf_new = cf_new->next;
  } طالما(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(بنية Curl_cfilter **pcf,
                          بنية Curl_easy *data)
{
  بنية Curl_cfilter *cf = pcf ? *pcf : عدم;
  bool found = FALSE;
  إذا(cf) {
    إذا(cf->conn) {
      /* unlink if present in connection filter chain */
      بنية Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      طالما(*pprev) {
        إذا(*pprev == *pcf) {
          *pprev = (*pcf)->next;
          cf->next = عدم;
          found = TRUE;
          اكسر;
        }
        pprev = &(*pprev)->next;
      }
    }
    Curl_conn_cf_discard_chain(pcf, data);
  }
  أرجع found;
}

CURLcode Curl_conn_cf_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data,
                              bool *done)
{
  إذا(cf)
    أرجع cf->cft->do_connect(cf, data, done);
  أرجع CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                           ثابت uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  إذا(cf)
    أرجع cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  أرجع CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                           حرف *buf, size_t len, size_t *pnread)
{
  إذا(cf)
    أرجع cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  أرجع CURLE_RECV_ERROR;
}

ساكن CURLcode cf_cntrl_all(بنية connectdata *conn,
                             بنية Curl_easy *data,
                             bool ignore_result,
                             صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode result = CURLE_OK;
  صحيح i;

  لكل(i = 0; i < (صحيح)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    إذا(!ignore_result && result)
      اكسر;
  }
  أرجع result;
}

bool Curl_conn_cf_is_ip_connected(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data)
{
  إذا(cf) {
    صحيح درج = 0;
    إذا(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &درج, عدم))
      أرجع !!درج;
  }
  أرجع FALSE;
}

فراغ Curl_conn_cntrl_update_info(بنية Curl_easy *data,
                                بنية connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, عدم);
}

فراغ Curl_conn_cntrl_report_stats(بنية Curl_easy *data,
                                  بنية connectdata *conn,
                                  صحيح sockindex)
{
  إذا((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (فراغ)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, عدم);
}

فراغ Curl_conn_remove_setup_filters(بنية Curl_easy *data,
                                    int8_t sockindex)
{
  بنية Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  طالما(*anchor) {
    بنية Curl_cfilter *cf = *anchor;
    إذا(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = عدم;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    وإلا
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(بنية connectdata *conn, int8_t sockindex)
{
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  أرجع !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(بنية connectdata *conn, int8_t sockindex)
{
  بنية Curl_cfilter *cf;

  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  cf = conn->cfilter[sockindex];
  إذا(cf)
    أرجع (bool)cf->connected;
  وإلا إذا(conn->scheme->flags & PROTOPT_NONETWORK)
    أرجع TRUE;
  أرجع FALSE;
}

bool Curl_conn_is_ip_connected(بنية Curl_easy *data, int8_t sockindex)
{
  بنية Curl_cfilter *cf;

  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  cf = data->conn->cfilter[sockindex];
  طالما(cf) {
    إذا(cf->connected)
      أرجع TRUE;
    إذا(cf->cft->flags & CF_TYPE_IP_CONNECT)
      أرجع FALSE;
    cf = cf->next;
  }
  أرجع FALSE;
}

#ifndef CURL_DISABLE_PROXY
ساكن bool cf_is_tunneling(بنية Curl_cfilter *cf)
{
  لكل(; cf; cf = cf->next) {
    إذا((cf->cft->flags & CF_TYPE_PROXY))
      أرجع TRUE;
  }
  أرجع FALSE;
}

bool Curl_conn_is_tunneling(بنية connectdata *conn, int8_t sockindex)
{
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  أرجع conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#وإلا
bool Curl_conn_is_tunneling(بنية connectdata *conn, int8_t sockindex)
{
  (فراغ)conn;
  (فراغ)sockindex;
  أرجع FALSE;
}
#endif /* CURL_DISABLE_PROXY */

ساكن bool cf_is_ssl(بنية Curl_cfilter *cf)
{
  لكل(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    إذا((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      أرجع TRUE;
    إذا(cf->cft->flags & CF_TYPE_IP_CONNECT)
      أرجع FALSE;
  }
  أرجع FALSE;
}

bool Curl_conn_is_ssl(بنية connectdata *conn, int8_t sockindex)
{
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  أرجع conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(بنية Curl_easy *data,
                            بنية connectdata *conn, int8_t sockindex,
                            صحيح query,
                            بنية curl_tlssessioninfo *info)
{
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  إذا(Curl_conn_is_ssl(conn, sockindex)) {
    بنية Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, عدم, (فراغ *)info) :
      CURLE_UNKNOWN_OPTION;
    أرجع !result;
  }
  أرجع FALSE;
}

CURLcode Curl_conn_get_ip_info(بنية Curl_easy *data,
                               بنية connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, بنية ip_quadruple *ipquad)
{
  بنية Curl_cfilter *cf;
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : عدم;
  أرجع Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(بنية connectdata *conn, int8_t sockindex)
{
  بنية Curl_cfilter *cf;

  إذا(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  لكل(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    إذا(cf->cft->flags & CF_TYPE_MULTIPLEX)
      أرجع TRUE;
    إذا(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      أرجع FALSE;
  }
  أرجع FALSE;
}

unsigned حرف Curl_conn_get_transport(بنية Curl_easy *data,
                                      بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  أرجع Curl_conn_cf_get_transport(cf, data);
}

صحيح Curl_socktype_for_transport(uint8_t transport)
{
  بدل(transport) {
  حالة TRNSPRT_TCP:
    أرجع SOCK_STREAM;
  حالة TRNSPRT_UNIX:
    أرجع SOCK_STREAM;
  افتراضي: /* UDP and QUIC */
    أرجع SOCK_DGRAM;
  }
}

صحيح Curl_protocol_for_transport(uint8_t transport)
{
  بدل(transport) {
  حالة TRNSPRT_TCP:
    أرجع IPPROTO_TCP;
  حالة TRNSPRT_UNIX:
    أرجع IPPROTO_IP;
  افتراضي: /* UDP and QUIC */
    أرجع IPPROTO_UDP;
  }
}

ثابت حرف *Curl_conn_get_alpn_negotiated(بنية Curl_easy *data,
                                          بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  أرجع Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned حرف Curl_conn_http_version(بنية Curl_easy *data,
                                     بنية connectdata *conn)
{
  بنية Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned حرف v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  لكل(; cf; cf = cf->next) {
    إذا(cf->cft->flags & CF_TYPE_HTTP) {
      صحيح درج = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &درج, عدم);
      إذا(!result && ((درج < 0) || (درج > 255)))
        result = CURLE_FAILED_INIT;
      وإلا
        v = (unsigned حرف)درج;
      اكسر;
    }
    إذا(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      اكسر;
  }
  أرجع (unsigned حرف)(result ? 0 : v);
}

bool Curl_conn_data_pending(بنية Curl_easy *data, int8_t sockindex)
{
  بنية Curl_cfilter *cf;

  (فراغ)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;

  cf = data->conn->cfilter[sockindex];
  طالما(cf && !cf->connected) {
    cf = cf->next;
  }
  إذا(cf) {
    أرجع cf->cft->has_data_pending(cf, data);
  }
  أرجع FALSE;
}

bool Curl_conn_cf_needs_flush(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data)
{
  CURLcode result;
  صحيح pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, عدم) : CURLE_UNKNOWN_OPTION;
  أرجع (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(بنية Curl_easy *data, int8_t sockindex)
{
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  أرجع Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
}

CURLcode Curl_conn_cf_adjust_pollset(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *data,
                                     بنية easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  لكل(; cf && !result; cf = cf->next) {
    إذا(cf->shutdown)
      استمر;
    إذا(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  أرجع result;
}

CURLcode Curl_conn_adjust_pollset(بنية Curl_easy *data,
                                  بنية connectdata *conn,
                                  بنية easy_pollset *ps)
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
  لكل(i = 0; (i < (صحيح)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    إذا(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  أرجع result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
صحيح Curl_conn_cf_poll(بنية Curl_cfilter *cf,
                      بنية Curl_easy *data,
                      timediff_t timeout_ms)
{
  بنية easy_pollset ps;
  صحيح rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  إذا(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  وإلا
    rc = -1;
  Curl_pollset_cleanup(&ps);
  أرجع rc;
}

فراغ Curl_conn_get_current_host(بنية Curl_easy *data, int8_t sockindex,
                                ثابت حرف **phost, صحيح *pport)
{
  بنية Curl_cfilter *cf, *cf_proxy = عدم;
  صحيح portarg = -1;

  إذا(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    إذا(pport)
      *pport = -1;
    أرجع;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : عدم;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  طالما(cf && !cf->connected) {
    إذا((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  إذا(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  إذا(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(بنية Curl_cfilter *cf,
                           بنية Curl_easy *data,
                           صحيح event, صحيح arg1, فراغ *arg2)
{
  (فراغ)cf;
  (فراغ)data;
  (فراغ)event;
  (فراغ)arg1;
  (فراغ)arg2;
  أرجع CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data,
                            bool ignore_result,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode result = CURLE_OK;

  لكل(; cf; cf = cf->next) {
    إذا(cf->cft->cntrl == Curl_cf_def_cntrl)
      استمر;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    إذا(!ignore_result && result)
      اكسر;
  }
  أرجع result;
}

curl_socket_t Curl_conn_cf_get_socket(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data)
{
  curl_socket_t sock;
  إذا(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, عدم, &sock))
    أرجع sock;
  أرجع CURL_SOCKET_BAD;
}

unsigned حرف Curl_conn_cf_get_transport(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *data)
{
  صحيح transport = 0;
  إذا(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, عدم))
    أرجع (unsigned حرف)transport;
  أرجع (unsigned حرف)(data->conn ? data->conn->transport_wanted : 0);
}

ثابت حرف *Curl_conn_cf_get_alpn_negotiated(بنية Curl_cfilter *cf,
                                             بنية Curl_easy *data)
{
  ثابت حرف *alpn = عدم;
  CURL_TRC_CF(data, cf, "query ALPN");
  إذا(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, عدم,
                           CURL_UNCONST(&alpn)))
    أرجع alpn;
  أرجع عدم;
}

ساكن ثابت بنية Curl_sockaddr_ex *cf_get_remote_addr(
  بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  ثابت بنية Curl_sockaddr_ex *remote_addr = عدم;
  إذا(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, عدم,
                     CURL_UNCONST(&remote_addr)))
    أرجع remote_addr;
  أرجع عدم;
}

CURLcode Curl_conn_cf_get_ip_info(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  bool *is_ipv6, بنية ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  إذا(cf) {
    صحيح ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  أرجع result;
}

curl_socket_t Curl_conn_get_first_socket(بنية Curl_easy *data)
{
  بنية Curl_cfilter *cf;

  إذا(!data->conn)
    أرجع CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  إذا(cf && !cf->connected)
    أرجع Curl_conn_cf_get_socket(cf, data);
  أرجع data->conn->sock[FIRSTSOCKET];
}

ثابت بنية Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  بنية Curl_easy *data, int8_t sockindex)
{
  بنية Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : عدم;
  أرجع cf ? cf_get_remote_addr(cf, data) : عدم;
}

CURLcode Curl_conn_ev_data_setup(بنية Curl_easy *data)
{
  أرجع cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, عدم);
}

CURLcode Curl_conn_flush(بنية Curl_easy *data, int8_t sockindex)
{
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  أرجع Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
                            CF_CTRL_FLUSH, 0, عدم);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
فراغ Curl_conn_ev_data_done_send(بنية Curl_easy *data)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE_SEND, 0, عدم);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
فراغ Curl_conn_ev_data_done(بنية Curl_easy *data, bool premature)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE, premature, عدم);
}

CURLcode Curl_conn_ev_data_pause(بنية Curl_easy *data, bool do_pause)
{
  أرجع cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, عدم);
}

bool Curl_conn_is_alive(بنية Curl_easy *data, بنية connectdata *conn,
                        bool *شغرو_pending)
{
  بنية Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  أرجع cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, شغرو_pending);
}

CURLcode Curl_conn_keep_alive(بنية Curl_easy *data,
                              بنية connectdata *conn)
{
  CURLcode result = CURLE_OK;
  صحيح i;

  لكل(i = 0; (i < (صحيح)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    بنية Curl_cfilter *cf = conn->cfilter[i];
    إذا(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  أرجع result;
}

size_t Curl_conn_get_max_concurrent(بنية Curl_easy *data,
                                    بنية connectdata *conn,
                                    int8_t sockindex)
{
  بنية Curl_cfilter *cf;
  CURLcode result;
  صحيح n = -1;

  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, عدم) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  أرجع (result || n < 0) ? 1 : (size_t)n;
}

صحيح Curl_conn_get_stream_error(بنية Curl_easy *data,
                               بنية connectdata *conn,
                               int8_t sockindex)
{
  بنية Curl_cfilter *cf;
  CURLcode result;
  صحيح n = 0;

  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, عدم) : CURLE_UNKNOWN_OPTION;
  أرجع (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(بنية Curl_easy *data, curl_socket_t sockfd)
{
  إذا(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    أرجع SECONDARYSOCKET;
  أرجع FIRSTSOCKET;
}

CURLcode Curl_conn_recv(بنية Curl_easy *data, int8_t sockindex,
                        حرف *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  إذا(data && data->conn && data->conn->recv[sockindex])
    أرجع data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  أرجع CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(بنية Curl_easy *data, int8_t sockindex,
                        ثابت فراغ *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  إذا(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    ثابت حرف *p = getenv("CURL_SMALLSENDS");
    إذا(p) {
      curl_off_t altsize;
      إذا(!curlx_str_لمب(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        إذا(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  إذا(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    إذا(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      أرجع CURLE_SEND_ERROR;
    }
    أرجع result;
  }
  *pnwritten = 0;
  أرجع CURLE_FAILED_INIT;
}
