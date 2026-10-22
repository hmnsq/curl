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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "strerror.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "curlx/strparse.h"

CURLcode Curl_cf_def_connect(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data, bool *done)
{
  CURLcode result;

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  જો(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    જો(result || !*done)
      પાછા result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  પાછા CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data, bool *done)
{
  (શૂન્ય)cf;
  (શૂન્ય)data;
  *done = TRUE;
  પાછા CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    સંરચના easy_pollset *ps)
{
  /* NOP */
  (શૂન્ય)cf;
  (શૂન્ય)data;
  (શૂન્ય)ps;
  પાછા CURLE_OK;
}

bool Curl_cf_def_data_pending(સંરચના Curl_cfilter *cf,
                              અચલ સંરચના Curl_easy *data)
{
  પાછા cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                          અચલ uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  જો(cf->next)
    પાછા cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  પાછા CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                          અક્ષર *buf, size_t len, size_t *pnread)
{
  જો(cf->next)
    પાછા cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  પાછા CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               bool *આગત_pending)
{
  પાછા cf->next ?
    cf->next->cft->is_alive(cf->next, data, આગત_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(સંરચના Curl_cfilter *cf,
                                     સંરચના Curl_easy *data)
{
  પાછા cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(સંરચના Curl_cfilter *cf,
                           સંરચના Curl_easy *data,
                           પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  પાછા cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
શૂન્ય Curl_conn_trc_filters(સંરચના Curl_easy *data,
                           int8_t sockindex, અચલ અક્ષર *info)
{
  જો(CURL_TRC_M_is_verbose(data) && data->conn) {
    સંરચના Curl_cfilter *cf = data->conn->cfilter[sockindex];

    જો(cf) {
      અક્ષર સંદેશ[256], *buf;
      પૂર્ણાંક blen, n;

      buf = સંદેશ;
      blen = કદ(સંદેશ) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      માટે(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", સંદેશ, blen ? "" : "...");
    }
    નહીંતર
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

શૂન્ય Curl_conn_cf_discard_chain(સંરચના Curl_cfilter **pcf,
                                સંરચના Curl_easy *data)
{
  સંરચના Curl_cfilter *cfn, *cf = *pcf;

  જો(cf) {
    *pcf = ખાલી;
    જ્યારે(cf) {
      cfn = cf->next;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->next = ખાલી;
      cf->cft->destroy(cf, data);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

શૂન્ય Curl_conn_cf_discard_all(સંરચના Curl_easy *data,
                              સંરચના connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.set[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], data);
}

CURLcode Curl_cf_recv(સંરચના Curl_easy *data, int8_t sockindex, અક્ષર *buf,
                      size_t len, size_t *pnread)
{
  સંરચના Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  જ્યારે(cf && !cf->connected)
    cf = cf->next;
  જો(cf)
    પાછા cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  પાછા CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(સંરચના Curl_easy *data, int8_t sockindex,
                      અચલ uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  સંરચના Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  જ્યારે(cf && !cf->connected)
    cf = cf->next;
  જો(cf) {
    પાછા cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  પાછા CURLE_FAILED_INIT;
}

સંરચના cf_io_ctx {
  સંરચના Curl_easy *data;
  સંરચના Curl_cfilter *cf;
};

સ્થિર CURLcode cf_bufq_reader(શૂન્ય *writer_ctx,
                               unsigned અક્ષર *buf, size_t blen,
                               size_t *pnread)
{
  સંરચના cf_io_ctx *io = writer_ctx;
  પાછા Curl_conn_cf_recv(io->cf, io->data, (અક્ષર *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(સંરચના Curl_cfilter *cf,
                           સંરચના Curl_easy *data,
                           સંરચના bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  સંરચના cf_io_ctx io;

  જો(!cf || !data) {
    *pnread = 0;
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  પાછા Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

સ્થિર CURLcode cf_bufq_writer(શૂન્ય *writer_ctx,
                               અચલ uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  સંરચના cf_io_ctx *io = writer_ctx;
  પાછા Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(સંરચના Curl_cfilter *cf,
                           સંરચના Curl_easy *data,
                           સંરચના bufq *bufq,
                           અચલ unsigned અક્ષર *buf, size_t blen,
                           size_t *pnwritten)
{
  સંરચના cf_io_ctx io;

  જો(!cf || !data) {
    *pnwritten = 0;
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  જો(buf && blen)
    પાછા Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  નહીંતર
    પાછા Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(સંરચના Curl_cfilter **pcf,
                        અચલ સંરચના Curl_cftype *cft,
                        શૂન્ય *ctx)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, કદ(*cf));
  જો(!cf)
    જાઓ out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  પાછા result;
}

શૂન્ય Curl_conn_cf_add(સંરચના Curl_easy *data,
                      સંરચના connectdata *conn,
                      int8_t sockindex,
                      સંરચના Curl_cfilter *cf)
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

શૂન્ય Curl_conn_cf_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                               સંરચના Curl_cfilter *cf_new)
{
  સંરચના Curl_cfilter *tail, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  tail = cf_at->next;
  cf_at->next = cf_new;
  કરો {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->next;
    cf_new = cf_new->next;
  } જ્યારે(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(સંરચના Curl_cfilter **pcf,
                          સંરચના Curl_easy *data)
{
  સંરચના Curl_cfilter *cf = pcf ? *pcf : ખાલી;
  bool found = FALSE;
  જો(cf) {
    જો(cf->conn) {
      /* unlink if present in connection filter chain */
      સંરચના Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      જ્યારે(*pprev) {
        જો(*pprev == *pcf) {
          *pprev = (*pcf)->next;
          cf->next = ખાલી;
          found = TRUE;
          તોડો;
        }
        pprev = &(*pprev)->next;
      }
    }
    Curl_conn_cf_discard_chain(pcf, data);
  }
  પાછા found;
}

CURLcode Curl_conn_cf_connect(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              bool *done)
{
  જો(cf)
    પાછા cf->cft->do_connect(cf, data, done);
  પાછા CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                           અચલ uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  જો(cf)
    પાછા cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  પાછા CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                           અક્ષર *buf, size_t len, size_t *pnread)
{
  જો(cf)
    પાછા cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  પાછા CURLE_RECV_ERROR;
}

સ્થિર CURLcode cf_cntrl_all(સંરચના connectdata *conn,
                             સંરચના Curl_easy *data,
                             bool ignore_result,
                             પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  CURLcode result = CURLE_OK;
  પૂર્ણાંક i;

  માટે(i = 0; i < (પૂર્ણાંક)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    જો(!ignore_result && result)
      તોડો;
  }
  પાછા result;
}

bool Curl_conn_cf_is_ip_connected(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data)
{
  જો(cf) {
    પૂર્ણાંક કિંમત = 0;
    જો(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &કિંમત, ખાલી))
      પાછા !!કિંમત;
  }
  પાછા FALSE;
}

શૂન્ય Curl_conn_cntrl_update_info(સંરચના Curl_easy *data,
                                સંરચના connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, ખાલી);
}

શૂન્ય Curl_conn_cntrl_report_stats(સંરચના Curl_easy *data,
                                  સંરચના connectdata *conn,
                                  પૂર્ણાંક sockindex)
{
  જો((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (શૂન્ય)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, ખાલી);
}

શૂન્ય Curl_conn_remove_setup_filters(સંરચના Curl_easy *data,
                                    int8_t sockindex)
{
  સંરચના Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  જ્યારે(*anchor) {
    સંરચના Curl_cfilter *cf = *anchor;
    જો(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = ખાલી;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    નહીંતર
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(સંરચના connectdata *conn, int8_t sockindex)
{
  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા FALSE;
  પાછા !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(સંરચના connectdata *conn, int8_t sockindex)
{
  સંરચના Curl_cfilter *cf;

  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા FALSE;
  cf = conn->cfilter[sockindex];
  જો(cf)
    પાછા (bool)cf->connected;
  નહીંતર જો(conn->scheme->flags & PROTOPT_NONETWORK)
    પાછા TRUE;
  પાછા FALSE;
}

bool Curl_conn_is_ip_connected(સંરચના Curl_easy *data, int8_t sockindex)
{
  સંરચના Curl_cfilter *cf;

  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા FALSE;
  cf = data->conn->cfilter[sockindex];
  જ્યારે(cf) {
    જો(cf->connected)
      પાછા TRUE;
    જો(cf->cft->flags & CF_TYPE_IP_CONNECT)
      પાછા FALSE;
    cf = cf->next;
  }
  પાછા FALSE;
}

#ifndef CURL_DISABLE_PROXY
સ્થિર bool cf_is_tunneling(સંરચના Curl_cfilter *cf)
{
  માટે(; cf; cf = cf->next) {
    જો((cf->cft->flags & CF_TYPE_PROXY))
      પાછા TRUE;
  }
  પાછા FALSE;
}

bool Curl_conn_is_tunneling(સંરચના connectdata *conn, int8_t sockindex)
{
  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા FALSE;
  પાછા conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#નહીંતર
bool Curl_conn_is_tunneling(સંરચના connectdata *conn, int8_t sockindex)
{
  (શૂન્ય)conn;
  (શૂન્ય)sockindex;
  પાછા FALSE;
}
#endif /* CURL_DISABLE_PROXY */

સ્થિર bool cf_is_ssl(સંરચના Curl_cfilter *cf)
{
  માટે(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    જો((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      પાછા TRUE;
    જો(cf->cft->flags & CF_TYPE_IP_CONNECT)
      પાછા FALSE;
  }
  પાછા FALSE;
}

bool Curl_conn_is_ssl(સંરચના connectdata *conn, int8_t sockindex)
{
  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા FALSE;
  પાછા conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(સંરચના Curl_easy *data,
                            સંરચના connectdata *conn, int8_t sockindex,
                            પૂર્ણાંક query,
                            સંરચના curl_tlssessioninfo *info)
{
  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા FALSE;
  જો(Curl_conn_is_ssl(conn, sockindex)) {
    સંરચના Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, ખાલી, (શૂન્ય *)info) :
      CURLE_UNKNOWN_OPTION;
    પાછા !result;
  }
  પાછા FALSE;
}

CURLcode Curl_conn_get_ip_info(સંરચના Curl_easy *data,
                               સંરચના connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, સંરચના ip_quadruple *ipquad)
{
  સંરચના Curl_cfilter *cf;
  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : ખાલી;
  પાછા Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(સંરચના connectdata *conn, int8_t sockindex)
{
  સંરચના Curl_cfilter *cf;

  જો(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    પાછા FALSE;
  માટે(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    જો(cf->cft->flags & CF_TYPE_MULTIPLEX)
      પાછા TRUE;
    જો(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      પાછા FALSE;
  }
  પાછા FALSE;
}

unsigned અક્ષર Curl_conn_get_transport(સંરચના Curl_easy *data,
                                      સંરચના connectdata *conn)
{
  સંરચના Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  પાછા Curl_conn_cf_get_transport(cf, data);
}

પૂર્ણાંક Curl_socktype_for_transport(uint8_t transport)
{
  પસંદગી(transport) {
  સ્થિતિ TRNSPRT_TCP:
    પાછા SOCK_STREAM;
  સ્થિતિ TRNSPRT_UNIX:
    પાછા SOCK_STREAM;
  મૂળભૂત: /* UDP and QUIC */
    પાછા SOCK_DGRAM;
  }
}

પૂર્ણાંક Curl_protocol_for_transport(uint8_t transport)
{
  પસંદગી(transport) {
  સ્થિતિ TRNSPRT_TCP:
    પાછા IPPROTO_TCP;
  સ્થિતિ TRNSPRT_UNIX:
    પાછા IPPROTO_IP;
  મૂળભૂત: /* UDP and QUIC */
    પાછા IPPROTO_UDP;
  }
}

અચલ અક્ષર *Curl_conn_get_alpn_negotiated(સંરચના Curl_easy *data,
                                          સંરચના connectdata *conn)
{
  સંરચના Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  પાછા Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned અક્ષર Curl_conn_http_version(સંરચના Curl_easy *data,
                                     સંરચના connectdata *conn)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned અક્ષર v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  માટે(; cf; cf = cf->next) {
    જો(cf->cft->flags & CF_TYPE_HTTP) {
      પૂર્ણાંક કિંમત = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &કિંમત, ખાલી);
      જો(!result && ((કિંમત < 0) || (કિંમત > 255)))
        result = CURLE_FAILED_INIT;
      નહીંતર
        v = (unsigned અક્ષર)કિંમત;
      તોડો;
    }
    જો(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      તોડો;
  }
  પાછા (unsigned અક્ષર)(result ? 0 : v);
}

bool Curl_conn_data_pending(સંરચના Curl_easy *data, int8_t sockindex)
{
  સંરચના Curl_cfilter *cf;

  (શૂન્ય)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા FALSE;

  cf = data->conn->cfilter[sockindex];
  જ્યારે(cf && !cf->connected) {
    cf = cf->next;
  }
  જો(cf) {
    પાછા cf->cft->has_data_pending(cf, data);
  }
  પાછા FALSE;
}

bool Curl_conn_cf_needs_flush(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data)
{
  CURLcode result;
  પૂર્ણાંક pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, ખાલી) : CURLE_UNKNOWN_OPTION;
  પાછા (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(સંરચના Curl_easy *data, int8_t sockindex)
{
  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા FALSE;
  પાછા Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
}

CURLcode Curl_conn_cf_adjust_pollset(સંરચના Curl_cfilter *cf,
                                     સંરચના Curl_easy *data,
                                     સંરચના easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  માટે(; cf && !result; cf = cf->next) {
    જો(cf->shutdown)
      ચાલુ;
    જો(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  પાછા result;
}

CURLcode Curl_conn_adjust_pollset(સંરચના Curl_easy *data,
                                  સંરચના connectdata *conn,
                                  સંરચના easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  અચલ bool want_io = !!ps->n;
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
  માટે(i = 0; (i < (પૂર્ણાંક)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    જો(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  પાછા result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
પૂર્ણાંક Curl_conn_cf_poll(સંરચના Curl_cfilter *cf,
                      સંરચના Curl_easy *data,
                      timediff_t timeout_ms)
{
  સંરચના easy_pollset ps;
  પૂર્ણાંક rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  જો(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  નહીંતર
    rc = -1;
  Curl_pollset_cleanup(&ps);
  પાછા rc;
}

શૂન્ય Curl_conn_get_current_host(સંરચના Curl_easy *data, int8_t sockindex,
                                અચલ અક્ષર **phost, પૂર્ણાંક *pport)
{
  સંરચના Curl_cfilter *cf, *cf_proxy = ખાલી;
  પૂર્ણાંક portarg = -1;

  જો(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    જો(pport)
      *pport = -1;
    પાછા;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : ખાલી;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  જ્યારે(cf && !cf->connected) {
    જો((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  જો(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  જો(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(સંરચના Curl_cfilter *cf,
                           સંરચના Curl_easy *data,
                           પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  (શૂન્ય)cf;
  (શૂન્ય)data;
  (શૂન્ય)event;
  (શૂન્ય)arg1;
  (શૂન્ય)arg2;
  પાછા CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            bool ignore_result,
                            પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  CURLcode result = CURLE_OK;

  માટે(; cf; cf = cf->next) {
    જો(cf->cft->cntrl == Curl_cf_def_cntrl)
      ચાલુ;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    જો(!ignore_result && result)
      તોડો;
  }
  પાછા result;
}

curl_socket_t Curl_conn_cf_get_socket(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data)
{
  curl_socket_t sock;
  જો(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, ખાલી, &sock))
    પાછા sock;
  પાછા CURL_SOCKET_BAD;
}

unsigned અક્ષર Curl_conn_cf_get_transport(સંરચના Curl_cfilter *cf,
                                         સંરચના Curl_easy *data)
{
  પૂર્ણાંક transport = 0;
  જો(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, ખાલી))
    પાછા (unsigned અક્ષર)transport;
  પાછા (unsigned અક્ષર)(data->conn ? data->conn->transport_wanted : 0);
}

અચલ અક્ષર *Curl_conn_cf_get_alpn_negotiated(સંરચના Curl_cfilter *cf,
                                             સંરચના Curl_easy *data)
{
  અચલ અક્ષર *alpn = ખાલી;
  CURL_TRC_CF(data, cf, "query ALPN");
  જો(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, ખાલી,
                           CURL_UNCONST(&alpn)))
    પાછા alpn;
  પાછા ખાલી;
}

સ્થિર અચલ સંરચના Curl_sockaddr_ex *cf_get_remote_addr(
  સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  અચલ સંરચના Curl_sockaddr_ex *remote_addr = ખાલી;
  જો(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, ખાલી,
                     CURL_UNCONST(&remote_addr)))
    પાછા remote_addr;
  પાછા ખાલી;
}

CURLcode Curl_conn_cf_get_ip_info(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  bool *is_ipv6, સંરચના ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  જો(cf) {
    પૂર્ણાંક ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  પાછા result;
}

curl_socket_t Curl_conn_get_first_socket(સંરચના Curl_easy *data)
{
  સંરચના Curl_cfilter *cf;

  જો(!data->conn)
    પાછા CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  જો(cf && !cf->connected)
    પાછા Curl_conn_cf_get_socket(cf, data);
  પાછા data->conn->sock[FIRSTSOCKET];
}

અચલ સંરચના Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  સંરચના Curl_easy *data, int8_t sockindex)
{
  સંરચના Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : ખાલી;
  પાછા cf ? cf_get_remote_addr(cf, data) : ખાલી;
}

CURLcode Curl_conn_ev_data_setup(સંરચના Curl_easy *data)
{
  પાછા cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, ખાલી);
}

CURLcode Curl_conn_flush(સંરચના Curl_easy *data, int8_t sockindex)
{
  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  પાછા Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
                            CF_CTRL_FLUSH, 0, ખાલી);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
શૂન્ય Curl_conn_ev_data_done_send(સંરચના Curl_easy *data)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE_SEND, 0, ખાલી);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
શૂન્ય Curl_conn_ev_data_done(સંરચના Curl_easy *data, bool premature)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE, premature, ખાલી);
}

CURLcode Curl_conn_ev_data_pause(સંરચના Curl_easy *data, bool do_pause)
{
  પાછા cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, ખાલી);
}

bool Curl_conn_is_alive(સંરચના Curl_easy *data, સંરચના connectdata *conn,
                        bool *આગત_pending)
{
  સંરચના Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  પાછા cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, આગત_pending);
}

CURLcode Curl_conn_keep_alive(સંરચના Curl_easy *data,
                              સંરચના connectdata *conn)
{
  CURLcode result = CURLE_OK;
  પૂર્ણાંક i;

  માટે(i = 0; (i < (પૂર્ણાંક)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    સંરચના Curl_cfilter *cf = conn->cfilter[i];
    જો(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  પાછા result;
}

size_t Curl_conn_get_max_concurrent(સંરચના Curl_easy *data,
                                    સંરચના connectdata *conn,
                                    int8_t sockindex)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result;
  પૂર્ણાંક n = -1;

  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, ખાલી) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  પાછા (result || n < 0) ? 1 : (size_t)n;
}

પૂર્ણાંક Curl_conn_get_stream_error(સંરચના Curl_easy *data,
                               સંરચના connectdata *conn,
                               int8_t sockindex)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result;
  પૂર્ણાંક n = 0;

  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, ખાલી) : CURLE_UNKNOWN_OPTION;
  પાછા (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(સંરચના Curl_easy *data, curl_socket_t sockfd)
{
  જો(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    પાછા SECONDARYSOCKET;
  પાછા FIRSTSOCKET;
}

CURLcode Curl_conn_recv(સંરચના Curl_easy *data, int8_t sockindex,
                        અક્ષર *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  જો(data && data->conn && data->conn->recv[sockindex])
    પાછા data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  પાછા CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(સંરચના Curl_easy *data, int8_t sockindex,
                        અચલ શૂન્ય *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  જો(!CONN_SOCK_IDX_VALID(sockindex))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  જો(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    અચલ અક્ષર *p = getenv("CURL_SMALLSENDS");
    જો(p) {
      curl_off_t altsize;
      જો(!curlx_str_સંખ્યા(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        જો(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  જો(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    જો(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      પાછા CURLE_SEND_ERROR;
    }
    પાછા result;
  }
  *pnwritten = 0;
  પાછા CURLE_FAILED_INIT;
}
