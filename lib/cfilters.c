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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "strerror.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "curlx/strparse.h"

CURLcode Curl_cf_def_connect(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data, bool *done)
{
  CURLcode result;

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  എങ്കിൽ(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    എങ്കിൽ(result || !*done)
      തിരികെ result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  തിരികെ CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data, bool *done)
{
  (ശൂന്യം)cf;
  (ശൂന്യം)data;
  *done = TRUE;
  തിരികെ CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    ഘടന easy_pollset *ps)
{
  /* NOP */
  (ശൂന്യം)cf;
  (ശൂന്യം)data;
  (ശൂന്യം)ps;
  തിരികെ CURLE_OK;
}

bool Curl_cf_def_data_pending(ഘടന Curl_cfilter *cf,
                              സ്ഥിരം ഘടന Curl_easy *data)
{
  തിരികെ cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                          സ്ഥിരം uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  എങ്കിൽ(cf->next)
    തിരികെ cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  തിരികെ CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                          അക്ഷരം *buf, size_t len, size_t *pnread)
{
  എങ്കിൽ(cf->next)
    തിരികെ cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  തിരികെ CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               bool *നിവേശം_pending)
{
  തിരികെ cf->next ?
    cf->next->cft->is_alive(cf->next, data, നിവേശം_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(ഘടന Curl_cfilter *cf,
                                     ഘടന Curl_easy *data)
{
  തിരികെ cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(ഘടന Curl_cfilter *cf,
                           ഘടന Curl_easy *data,
                           പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  തിരികെ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
ശൂന്യം Curl_conn_trc_filters(ഘടന Curl_easy *data,
                           int8_t sockindex, സ്ഥിരം അക്ഷരം *info)
{
  എങ്കിൽ(CURL_TRC_M_is_verbose(data) && data->conn) {
    ഘടന Curl_cfilter *cf = data->conn->cfilter[sockindex];

    എങ്കിൽ(cf) {
      അക്ഷരം സന്ദേശം[256], *buf;
      പൂർണ്ണസംഖ്യ blen, n;

      buf = സന്ദേശം;
      blen = വലുപ്പം(സന്ദേശം) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      വേണ്ടി(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", സന്ദേശം, blen ? "" : "...");
    }
    അല്ലെങ്കിൽ
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

ശൂന്യം Curl_conn_cf_discard_chain(ഘടന Curl_cfilter **pcf,
                                ഘടന Curl_easy *data)
{
  ഘടന Curl_cfilter *cfn, *cf = *pcf;

  എങ്കിൽ(cf) {
    *pcf = ശൂന്യ;
    ആകുമ്പോൾ(cf) {
      cfn = cf->next;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->next = ശൂന്യ;
      cf->cft->destroy(cf, data);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

ശൂന്യം Curl_conn_cf_discard_all(ഘടന Curl_easy *data,
                              ഘടന connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.set[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], data);
}

CURLcode Curl_cf_recv(ഘടന Curl_easy *data, int8_t sockindex, അക്ഷരം *buf,
                      size_t len, size_t *pnread)
{
  ഘടന Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  ആകുമ്പോൾ(cf && !cf->connected)
    cf = cf->next;
  എങ്കിൽ(cf)
    തിരികെ cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  തിരികെ CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(ഘടന Curl_easy *data, int8_t sockindex,
                      സ്ഥിരം uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  ഘടന Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  ആകുമ്പോൾ(cf && !cf->connected)
    cf = cf->next;
  എങ്കിൽ(cf) {
    തിരികെ cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  തിരികെ CURLE_FAILED_INIT;
}

ഘടന cf_io_ctx {
  ഘടന Curl_easy *data;
  ഘടന Curl_cfilter *cf;
};

സ്ഥിതം CURLcode cf_bufq_reader(ശൂന്യം *writer_ctx,
                               unsigned അക്ഷരം *buf, size_t blen,
                               size_t *pnread)
{
  ഘടന cf_io_ctx *io = writer_ctx;
  തിരികെ Curl_conn_cf_recv(io->cf, io->data, (അക്ഷരം *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(ഘടന Curl_cfilter *cf,
                           ഘടന Curl_easy *data,
                           ഘടന bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  ഘടന cf_io_ctx io;

  എങ്കിൽ(!cf || !data) {
    *pnread = 0;
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  തിരികെ Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

സ്ഥിതം CURLcode cf_bufq_writer(ശൂന്യം *writer_ctx,
                               സ്ഥിരം uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  ഘടന cf_io_ctx *io = writer_ctx;
  തിരികെ Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(ഘടന Curl_cfilter *cf,
                           ഘടന Curl_easy *data,
                           ഘടന bufq *bufq,
                           സ്ഥിരം unsigned അക്ഷരം *buf, size_t blen,
                           size_t *pnwritten)
{
  ഘടന cf_io_ctx io;

  എങ്കിൽ(!cf || !data) {
    *pnwritten = 0;
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  എങ്കിൽ(buf && blen)
    തിരികെ Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  അല്ലെങ്കിൽ
    തിരികെ Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(ഘടന Curl_cfilter **pcf,
                        സ്ഥിരം ഘടന Curl_cftype *cft,
                        ശൂന്യം *ctx)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, വലുപ്പം(*cf));
  എങ്കിൽ(!cf)
    പോകുക out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  തിരികെ result;
}

ശൂന്യം Curl_conn_cf_add(ഘടന Curl_easy *data,
                      ഘടന connectdata *conn,
                      int8_t sockindex,
                      ഘടന Curl_cfilter *cf)
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

ശൂന്യം Curl_conn_cf_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                               ഘടന Curl_cfilter *cf_new)
{
  ഘടന Curl_cfilter *tail, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  tail = cf_at->next;
  cf_at->next = cf_new;
  ചെയ്യുക {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->next;
    cf_new = cf_new->next;
  } ആകുമ്പോൾ(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(ഘടന Curl_cfilter **pcf,
                          ഘടന Curl_easy *data)
{
  ഘടന Curl_cfilter *cf = pcf ? *pcf : ശൂന്യ;
  bool found = FALSE;
  എങ്കിൽ(cf) {
    എങ്കിൽ(cf->conn) {
      /* unlink if present in connection filter chain */
      ഘടന Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      ആകുമ്പോൾ(*pprev) {
        എങ്കിൽ(*pprev == *pcf) {
          *pprev = (*pcf)->next;
          cf->next = ശൂന്യ;
          found = TRUE;
          നിർത്തുക;
        }
        pprev = &(*pprev)->next;
      }
    }
    Curl_conn_cf_discard_chain(pcf, data);
  }
  തിരികെ found;
}

CURLcode Curl_conn_cf_connect(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              bool *done)
{
  എങ്കിൽ(cf)
    തിരികെ cf->cft->do_connect(cf, data, done);
  തിരികെ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                           സ്ഥിരം uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  എങ്കിൽ(cf)
    തിരികെ cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  തിരികെ CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                           അക്ഷരം *buf, size_t len, size_t *pnread)
{
  എങ്കിൽ(cf)
    തിരികെ cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  തിരികെ CURLE_RECV_ERROR;
}

സ്ഥിതം CURLcode cf_cntrl_all(ഘടന connectdata *conn,
                             ഘടന Curl_easy *data,
                             bool ignore_result,
                             പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ i;

  വേണ്ടി(i = 0; i < (പൂർണ്ണസംഖ്യ)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    എങ്കിൽ(!ignore_result && result)
      നിർത്തുക;
  }
  തിരികെ result;
}

bool Curl_conn_cf_is_ip_connected(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data)
{
  എങ്കിൽ(cf) {
    പൂർണ്ണസംഖ്യ മൂല്യം = 0;
    എങ്കിൽ(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &മൂല്യം, ശൂന്യ))
      തിരികെ !!മൂല്യം;
  }
  തിരികെ FALSE;
}

ശൂന്യം Curl_conn_cntrl_update_info(ഘടന Curl_easy *data,
                                ഘടന connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, ശൂന്യ);
}

ശൂന്യം Curl_conn_cntrl_report_stats(ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn,
                                  പൂർണ്ണസംഖ്യ sockindex)
{
  എങ്കിൽ((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (ശൂന്യം)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, ശൂന്യ);
}

ശൂന്യം Curl_conn_remove_setup_filters(ഘടന Curl_easy *data,
                                    int8_t sockindex)
{
  ഘടന Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  ആകുമ്പോൾ(*anchor) {
    ഘടന Curl_cfilter *cf = *anchor;
    എങ്കിൽ(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = ശൂന്യ;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    അല്ലെങ്കിൽ
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(ഘടന connectdata *conn, int8_t sockindex)
{
  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ FALSE;
  തിരികെ !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(ഘടന connectdata *conn, int8_t sockindex)
{
  ഘടന Curl_cfilter *cf;

  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ FALSE;
  cf = conn->cfilter[sockindex];
  എങ്കിൽ(cf)
    തിരികെ (bool)cf->connected;
  അല്ലെങ്കിൽ എങ്കിൽ(conn->scheme->flags & PROTOPT_NONETWORK)
    തിരികെ TRUE;
  തിരികെ FALSE;
}

bool Curl_conn_is_ip_connected(ഘടന Curl_easy *data, int8_t sockindex)
{
  ഘടന Curl_cfilter *cf;

  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ FALSE;
  cf = data->conn->cfilter[sockindex];
  ആകുമ്പോൾ(cf) {
    എങ്കിൽ(cf->connected)
      തിരികെ TRUE;
    എങ്കിൽ(cf->cft->flags & CF_TYPE_IP_CONNECT)
      തിരികെ FALSE;
    cf = cf->next;
  }
  തിരികെ FALSE;
}

#ifndef CURL_DISABLE_PROXY
സ്ഥിതം bool cf_is_tunneling(ഘടന Curl_cfilter *cf)
{
  വേണ്ടി(; cf; cf = cf->next) {
    എങ്കിൽ((cf->cft->flags & CF_TYPE_PROXY))
      തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

bool Curl_conn_is_tunneling(ഘടന connectdata *conn, int8_t sockindex)
{
  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ FALSE;
  തിരികെ conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#അല്ലെങ്കിൽ
bool Curl_conn_is_tunneling(ഘടന connectdata *conn, int8_t sockindex)
{
  (ശൂന്യം)conn;
  (ശൂന്യം)sockindex;
  തിരികെ FALSE;
}
#endif /* CURL_DISABLE_PROXY */

സ്ഥിതം bool cf_is_ssl(ഘടന Curl_cfilter *cf)
{
  വേണ്ടി(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    എങ്കിൽ((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      തിരികെ TRUE;
    എങ്കിൽ(cf->cft->flags & CF_TYPE_IP_CONNECT)
      തിരികെ FALSE;
  }
  തിരികെ FALSE;
}

bool Curl_conn_is_ssl(ഘടന connectdata *conn, int8_t sockindex)
{
  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ FALSE;
  തിരികെ conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(ഘടന Curl_easy *data,
                            ഘടന connectdata *conn, int8_t sockindex,
                            പൂർണ്ണസംഖ്യ query,
                            ഘടന curl_tlssessioninfo *info)
{
  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ FALSE;
  എങ്കിൽ(Curl_conn_is_ssl(conn, sockindex)) {
    ഘടന Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, ശൂന്യ, (ശൂന്യം *)info) :
      CURLE_UNKNOWN_OPTION;
    തിരികെ !result;
  }
  തിരികെ FALSE;
}

CURLcode Curl_conn_get_ip_info(ഘടന Curl_easy *data,
                               ഘടന connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, ഘടന ip_quadruple *ipquad)
{
  ഘടന Curl_cfilter *cf;
  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : ശൂന്യ;
  തിരികെ Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(ഘടന connectdata *conn, int8_t sockindex)
{
  ഘടന Curl_cfilter *cf;

  എങ്കിൽ(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ FALSE;
  വേണ്ടി(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    എങ്കിൽ(cf->cft->flags & CF_TYPE_MULTIPLEX)
      തിരികെ TRUE;
    എങ്കിൽ(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      തിരികെ FALSE;
  }
  തിരികെ FALSE;
}

unsigned അക്ഷരം Curl_conn_get_transport(ഘടന Curl_easy *data,
                                      ഘടന connectdata *conn)
{
  ഘടന Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  തിരികെ Curl_conn_cf_get_transport(cf, data);
}

പൂർണ്ണസംഖ്യ Curl_socktype_for_transport(uint8_t transport)
{
  തിരഞ്ഞെടുപ്പ്(transport) {
  അവസ്ഥ TRNSPRT_TCP:
    തിരികെ SOCK_STREAM;
  അവസ്ഥ TRNSPRT_UNIX:
    തിരികെ SOCK_STREAM;
  സ്ഥിരസ്ഥിതി: /* UDP and QUIC */
    തിരികെ SOCK_DGRAM;
  }
}

പൂർണ്ണസംഖ്യ Curl_protocol_for_transport(uint8_t transport)
{
  തിരഞ്ഞെടുപ്പ്(transport) {
  അവസ്ഥ TRNSPRT_TCP:
    തിരികെ IPPROTO_TCP;
  അവസ്ഥ TRNSPRT_UNIX:
    തിരികെ IPPROTO_IP;
  സ്ഥിരസ്ഥിതി: /* UDP and QUIC */
    തിരികെ IPPROTO_UDP;
  }
}

സ്ഥിരം അക്ഷരം *Curl_conn_get_alpn_negotiated(ഘടന Curl_easy *data,
                                          ഘടന connectdata *conn)
{
  ഘടന Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  തിരികെ Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned അക്ഷരം Curl_conn_http_version(ഘടന Curl_easy *data,
                                     ഘടന connectdata *conn)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned അക്ഷരം v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  വേണ്ടി(; cf; cf = cf->next) {
    എങ്കിൽ(cf->cft->flags & CF_TYPE_HTTP) {
      പൂർണ്ണസംഖ്യ മൂല്യം = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &മൂല്യം, ശൂന്യ);
      എങ്കിൽ(!result && ((മൂല്യം < 0) || (മൂല്യം > 255)))
        result = CURLE_FAILED_INIT;
      അല്ലെങ്കിൽ
        v = (unsigned അക്ഷരം)മൂല്യം;
      നിർത്തുക;
    }
    എങ്കിൽ(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      നിർത്തുക;
  }
  തിരികെ (unsigned അക്ഷരം)(result ? 0 : v);
}

bool Curl_conn_data_pending(ഘടന Curl_easy *data, int8_t sockindex)
{
  ഘടന Curl_cfilter *cf;

  (ശൂന്യം)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ FALSE;

  cf = data->conn->cfilter[sockindex];
  ആകുമ്പോൾ(cf && !cf->connected) {
    cf = cf->next;
  }
  എങ്കിൽ(cf) {
    തിരികെ cf->cft->has_data_pending(cf, data);
  }
  തിരികെ FALSE;
}

bool Curl_conn_cf_needs_flush(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data)
{
  CURLcode result;
  പൂർണ്ണസംഖ്യ pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, ശൂന്യ) : CURLE_UNKNOWN_OPTION;
  തിരികെ (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(ഘടന Curl_easy *data, int8_t sockindex)
{
  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ FALSE;
  തിരികെ Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
}

CURLcode Curl_conn_cf_adjust_pollset(ഘടന Curl_cfilter *cf,
                                     ഘടന Curl_easy *data,
                                     ഘടന easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  വേണ്ടി(; cf && !result; cf = cf->next) {
    എങ്കിൽ(cf->shutdown)
      തുടരുക;
    എങ്കിൽ(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  തിരികെ result;
}

CURLcode Curl_conn_adjust_pollset(ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn,
                                  ഘടന easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം bool want_io = !!ps->n;
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
  വേണ്ടി(i = 0; (i < (പൂർണ്ണസംഖ്യ)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    എങ്കിൽ(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  തിരികെ result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
പൂർണ്ണസംഖ്യ Curl_conn_cf_poll(ഘടന Curl_cfilter *cf,
                      ഘടന Curl_easy *data,
                      timediff_t timeout_ms)
{
  ഘടന easy_pollset ps;
  പൂർണ്ണസംഖ്യ rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  എങ്കിൽ(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  അല്ലെങ്കിൽ
    rc = -1;
  Curl_pollset_cleanup(&ps);
  തിരികെ rc;
}

ശൂന്യം Curl_conn_get_current_host(ഘടന Curl_easy *data, int8_t sockindex,
                                സ്ഥിരം അക്ഷരം **phost, പൂർണ്ണസംഖ്യ *pport)
{
  ഘടന Curl_cfilter *cf, *cf_proxy = ശൂന്യ;
  പൂർണ്ണസംഖ്യ portarg = -1;

  എങ്കിൽ(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    എങ്കിൽ(pport)
      *pport = -1;
    തിരികെ;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : ശൂന്യ;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  ആകുമ്പോൾ(cf && !cf->connected) {
    എങ്കിൽ((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  എങ്കിൽ(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  എങ്കിൽ(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(ഘടന Curl_cfilter *cf,
                           ഘടന Curl_easy *data,
                           പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  (ശൂന്യം)cf;
  (ശൂന്യം)data;
  (ശൂന്യം)event;
  (ശൂന്യം)arg1;
  (ശൂന്യം)arg2;
  തിരികെ CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            bool ignore_result,
                            പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  CURLcode result = CURLE_OK;

  വേണ്ടി(; cf; cf = cf->next) {
    എങ്കിൽ(cf->cft->cntrl == Curl_cf_def_cntrl)
      തുടരുക;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    എങ്കിൽ(!ignore_result && result)
      നിർത്തുക;
  }
  തിരികെ result;
}

curl_socket_t Curl_conn_cf_get_socket(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data)
{
  curl_socket_t sock;
  എങ്കിൽ(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, ശൂന്യ, &sock))
    തിരികെ sock;
  തിരികെ CURL_SOCKET_BAD;
}

unsigned അക്ഷരം Curl_conn_cf_get_transport(ഘടന Curl_cfilter *cf,
                                         ഘടന Curl_easy *data)
{
  പൂർണ്ണസംഖ്യ transport = 0;
  എങ്കിൽ(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, ശൂന്യ))
    തിരികെ (unsigned അക്ഷരം)transport;
  തിരികെ (unsigned അക്ഷരം)(data->conn ? data->conn->transport_wanted : 0);
}

സ്ഥിരം അക്ഷരം *Curl_conn_cf_get_alpn_negotiated(ഘടന Curl_cfilter *cf,
                                             ഘടന Curl_easy *data)
{
  സ്ഥിരം അക്ഷരം *alpn = ശൂന്യ;
  CURL_TRC_CF(data, cf, "query ALPN");
  എങ്കിൽ(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, ശൂന്യ,
                           CURL_UNCONST(&alpn)))
    തിരികെ alpn;
  തിരികെ ശൂന്യ;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_sockaddr_ex *cf_get_remote_addr(
  ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  സ്ഥിരം ഘടന Curl_sockaddr_ex *remote_addr = ശൂന്യ;
  എങ്കിൽ(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, ശൂന്യ,
                     CURL_UNCONST(&remote_addr)))
    തിരികെ remote_addr;
  തിരികെ ശൂന്യ;
}

CURLcode Curl_conn_cf_get_ip_info(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  bool *is_ipv6, ഘടന ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  എങ്കിൽ(cf) {
    പൂർണ്ണസംഖ്യ ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  തിരികെ result;
}

curl_socket_t Curl_conn_get_first_socket(ഘടന Curl_easy *data)
{
  ഘടന Curl_cfilter *cf;

  എങ്കിൽ(!data->conn)
    തിരികെ CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  എങ്കിൽ(cf && !cf->connected)
    തിരികെ Curl_conn_cf_get_socket(cf, data);
  തിരികെ data->conn->sock[FIRSTSOCKET];
}

സ്ഥിരം ഘടന Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  ഘടന Curl_easy *data, int8_t sockindex)
{
  ഘടന Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : ശൂന്യ;
  തിരികെ cf ? cf_get_remote_addr(cf, data) : ശൂന്യ;
}

CURLcode Curl_conn_ev_data_setup(ഘടന Curl_easy *data)
{
  തിരികെ cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, ശൂന്യ);
}

CURLcode Curl_conn_flush(ഘടന Curl_easy *data, int8_t sockindex)
{
  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  തിരികെ Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
                            CF_CTRL_FLUSH, 0, ശൂന്യ);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
ശൂന്യം Curl_conn_ev_data_done_send(ഘടന Curl_easy *data)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE_SEND, 0, ശൂന്യ);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
ശൂന്യം Curl_conn_ev_data_done(ഘടന Curl_easy *data, bool premature)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE, premature, ശൂന്യ);
}

CURLcode Curl_conn_ev_data_pause(ഘടന Curl_easy *data, bool do_pause)
{
  തിരികെ cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, ശൂന്യ);
}

bool Curl_conn_is_alive(ഘടന Curl_easy *data, ഘടന connectdata *conn,
                        bool *നിവേശം_pending)
{
  ഘടന Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  തിരികെ cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, നിവേശം_pending);
}

CURLcode Curl_conn_keep_alive(ഘടന Curl_easy *data,
                              ഘടന connectdata *conn)
{
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ i;

  വേണ്ടി(i = 0; (i < (പൂർണ്ണസംഖ്യ)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    ഘടന Curl_cfilter *cf = conn->cfilter[i];
    എങ്കിൽ(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  തിരികെ result;
}

size_t Curl_conn_get_max_concurrent(ഘടന Curl_easy *data,
                                    ഘടന connectdata *conn,
                                    int8_t sockindex)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result;
  പൂർണ്ണസംഖ്യ n = -1;

  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, ശൂന്യ) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  തിരികെ (result || n < 0) ? 1 : (size_t)n;
}

പൂർണ്ണസംഖ്യ Curl_conn_get_stream_error(ഘടന Curl_easy *data,
                               ഘടന connectdata *conn,
                               int8_t sockindex)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result;
  പൂർണ്ണസംഖ്യ n = 0;

  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, ശൂന്യ) : CURLE_UNKNOWN_OPTION;
  തിരികെ (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(ഘടന Curl_easy *data, curl_socket_t sockfd)
{
  എങ്കിൽ(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    തിരികെ SECONDARYSOCKET;
  തിരികെ FIRSTSOCKET;
}

CURLcode Curl_conn_recv(ഘടന Curl_easy *data, int8_t sockindex,
                        അക്ഷരം *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  എങ്കിൽ(data && data->conn && data->conn->recv[sockindex])
    തിരികെ data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  തിരികെ CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(ഘടന Curl_easy *data, int8_t sockindex,
                        സ്ഥിരം ശൂന്യം *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  എങ്കിൽ(!CONN_SOCK_IDX_VALID(sockindex))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  എങ്കിൽ(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    സ്ഥിരം അക്ഷരം *p = getenv("CURL_SMALLSENDS");
    എങ്കിൽ(p) {
      curl_off_t altsize;
      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        എങ്കിൽ(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  എങ്കിൽ(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    എങ്കിൽ(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      തിരികെ CURLE_SEND_ERROR;
    }
    തിരികെ result;
  }
  *pnwritten = 0;
  തിരികെ CURLE_FAILED_INIT;
}
