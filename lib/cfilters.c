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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "strerror.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "curlx/strparse.h"

CURLcode Curl_cf_def_connect(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result;

  যদি(cf->connected) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  যদি(cf->next) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    যদি(result || !*done)
      প্রত্যাগম result;
  }

  cf->connected = TRUE;
  *done = TRUE;
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data, bool *done)
{
  (শূন্য)cf;
  (শূন্য)data;
  *done = TRUE;
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    সংরচনা easy_pollset *ps)
{
  /* NOP */
  (শূন্য)cf;
  (শূন্য)data;
  (শূন্য)ps;
  প্রত্যাগম CURLE_OK;
}

bool Curl_cf_def_data_pending(সংরচনা Curl_cfilter *cf,
                              ধ্রুব সংরচনা Curl_easy *data)
{
  প্রত্যাগম cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

CURLcode Curl_cf_def_send(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                          ধ্রুব uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  যদি(cf->next)
    প্রত্যাগম cf->next->cft->do_send(cf->next, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  প্রত্যাগম CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                          বর্ণ *buf, size_t len, size_t *pnread)
{
  যদি(cf->next)
    প্রত্যাগম cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  প্রত্যাগম CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               bool *প্রবেশ_pending)
{
  প্রত্যাগম cf->next ?
    cf->next->cft->is_alive(cf->next, data, প্রবেশ_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(সংরচনা Curl_cfilter *cf,
                                     সংরচনা Curl_easy *data)
{
  প্রত্যাগম cf->next ?
    cf->next->cft->keep_alive(cf->next, data) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(সংরচনা Curl_cfilter *cf,
                           সংরচনা Curl_easy *data,
                           পূর্ণাঙ্ক query, পূর্ণাঙ্ক *pres1, শূন্য *pres2)
{
  প্রত্যাগম cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
শূন্য Curl_conn_trc_filters(সংরচনা Curl_easy *data,
                           int8_t sockindex, ধ্রুব বর্ণ *info)
{
  যদি(CURL_TRC_M_is_verbose(data) && data->conn) {
    সংরচনা Curl_cfilter *cf = data->conn->cfilter[sockindex];

    যদি(cf) {
      বর্ণ বার্তা[256], *buf;
      পূর্ণাঙ্ক blen, n;

      buf = বার্তা;
      blen = পরিমাণ(বার্তা) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      প্রতি(; cf && blen; cf = cf->next) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(data, "%s%s", বার্তা, blen ? "" : "...");
    }
    অন্যথা
      CURL_TRC_M(data, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

শূন্য Curl_conn_cf_discard_chain(সংরচনা Curl_cfilter **pcf,
                                সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cfilter *cfn, *cf = *pcf;

  যদি(cf) {
    *pcf = রিক্ত;
    যাবত্(cf) {
      cfn = cf->next;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->next = রিক্ত;
      cf->cft->destroy(cf, data);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

শূন্য Curl_conn_cf_discard_all(সংরচনা Curl_easy *data,
                              সংরচনা connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.set[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], data);
}

CURLcode Curl_cf_recv(সংরচনা Curl_easy *data, int8_t sockindex, বর্ণ *buf,
                      size_t len, size_t *pnread)
{
  সংরচনা Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  যাবত্(cf && !cf->connected)
    cf = cf->next;
  যদি(cf)
    প্রত্যাগম cf->cft->do_recv(cf, data, buf, len, pnread);
  failf(data, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  প্রত্যাগম CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(সংরচনা Curl_easy *data, int8_t sockindex,
                      ধ্রুব uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  সংরচনা Curl_cfilter *cf;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  cf = data->conn->cfilter[sockindex];
  যাবত্(cf && !cf->connected)
    cf = cf->next;
  যদি(cf) {
    প্রত্যাগম cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  }
  failf(data, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  প্রত্যাগম CURLE_FAILED_INIT;
}

সংরচনা cf_io_ctx {
  সংরচনা Curl_easy *data;
  সংরচনা Curl_cfilter *cf;
};

স্থির CURLcode cf_bufq_reader(শূন্য *writer_ctx,
                               unsigned বর্ণ *buf, size_t blen,
                               size_t *pnread)
{
  সংরচনা cf_io_ctx *io = writer_ctx;
  প্রত্যাগম Curl_conn_cf_recv(io->cf, io->data, (বর্ণ *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(সংরচনা Curl_cfilter *cf,
                           সংরচনা Curl_easy *data,
                           সংরচনা bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  সংরচনা cf_io_ctx io;

  যদি(!cf || !data) {
    *pnread = 0;
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  প্রত্যাগম Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

স্থির CURLcode cf_bufq_writer(শূন্য *writer_ctx,
                               ধ্রুব uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  সংরচনা cf_io_ctx *io = writer_ctx;
  প্রত্যাগম Curl_conn_cf_send(io->cf, io->data, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(সংরচনা Curl_cfilter *cf,
                           সংরচনা Curl_easy *data,
                           সংরচনা bufq *bufq,
                           ধ্রুব unsigned বর্ণ *buf, size_t blen,
                           size_t *pnwritten)
{
  সংরচনা cf_io_ctx io;

  যদি(!cf || !data) {
    *pnwritten = 0;
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.data = data;
  io.cf = cf;
  যদি(buf && blen)
    প্রত্যাগম Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  অন্যথা
    প্রত্যাগম Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(সংরচনা Curl_cfilter **pcf,
                        ধ্রুব সংরচনা Curl_cftype *cft,
                        শূন্য *ctx)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, পরিমাণ(*cf));
  যদি(!cf)
    গচ্ছ out;

  cf->cft = cft;
  cf->ctx = ctx;
  result = CURLE_OK;
out:
  *pcf = cf;
  প্রত্যাগম result;
}

শূন্য Curl_conn_cf_add(সংরচনা Curl_easy *data,
                      সংরচনা connectdata *conn,
                      int8_t sockindex,
                      সংরচনা Curl_cfilter *cf)
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

শূন্য Curl_conn_cf_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                               সংরচনা Curl_cfilter *cf_new)
{
  সংরচনা Curl_cfilter *tail, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  tail = cf_at->next;
  cf_at->next = cf_new;
  কুরু {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->next;
    cf_new = cf_new->next;
  } যাবত্(cf_new);
  *pnext = tail;
}

bool Curl_conn_cf_discard(সংরচনা Curl_cfilter **pcf,
                          সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cfilter *cf = pcf ? *pcf : রিক্ত;
  bool found = FALSE;
  যদি(cf) {
    যদি(cf->conn) {
      /* unlink if present in connection filter chain */
      সংরচনা Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      যাবত্(*pprev) {
        যদি(*pprev == *pcf) {
          *pprev = (*pcf)->next;
          cf->next = রিক্ত;
          found = TRUE;
          ভঙ্গ;
        }
        pprev = &(*pprev)->next;
      }
    }
    Curl_conn_cf_discard_chain(pcf, data);
  }
  প্রত্যাগম found;
}

CURLcode Curl_conn_cf_connect(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              bool *done)
{
  যদি(cf)
    প্রত্যাগম cf->cft->do_connect(cf, data, done);
  প্রত্যাগম CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                           ধ্রুব uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  যদি(cf)
    প্রত্যাগম cf->cft->do_send(cf, data, buf, len, eos, pnwritten);
  *pnwritten = 0;
  প্রত্যাগম CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                           বর্ণ *buf, size_t len, size_t *pnread)
{
  যদি(cf)
    প্রত্যাগম cf->cft->do_recv(cf, data, buf, len, pnread);
  *pnread = 0;
  প্রত্যাগম CURLE_RECV_ERROR;
}

স্থির CURLcode cf_cntrl_all(সংরচনা connectdata *conn,
                             সংরচনা Curl_easy *data,
                             bool ignore_result,
                             পূর্ণাঙ্ক event, পূর্ণাঙ্ক arg1, শূন্য *arg2)
{
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক i;

  প্রতি(i = 0; i < (পূর্ণাঙ্ক)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    result = Curl_conn_cf_cntrl(conn->cfilter[i], data, ignore_result,
                                event, arg1, arg2);
    যদি(!ignore_result && result)
      ভঙ্গ;
  }
  প্রত্যাগম result;
}

bool Curl_conn_cf_is_ip_connected(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data)
{
  যদি(cf) {
    পূর্ণাঙ্ক মান = 0;
    যদি(!cf->cft->query(cf, data, CF_QUERY_REALLY_CONNECTED, &মান, রিক্ত))
      প্রত্যাগম !!মান;
  }
  প্রত্যাগম FALSE;
}

শূন্য Curl_conn_cntrl_update_info(সংরচনা Curl_easy *data,
                                সংরচনা connectdata *conn)
{
  cf_cntrl_all(conn, data, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, রিক্ত);
}

শূন্য Curl_conn_cntrl_report_stats(সংরচনা Curl_easy *data,
                                  সংরচনা connectdata *conn,
                                  পূর্ণাঙ্ক sockindex)
{
  যদি((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (শূন্য)Curl_conn_cf_cntrl(conn->cfilter[sockindex], data, TRUE,
                             CF_CTRL_REPORT_STATS, 0, রিক্ত);
}

শূন্য Curl_conn_remove_setup_filters(সংরচনা Curl_easy *data,
                                    int8_t sockindex)
{
  সংরচনা Curl_cfilter **anchor = &data->conn->cfilter[sockindex];
  যাবত্(*anchor) {
    সংরচনা Curl_cfilter *cf = *anchor;
    যদি(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->next;
      cf->next = রিক্ত;
      CURL_TRC_CF(data, cf, "removing connected setup filter");
      cf->cft->destroy(cf, data);
      curlx_free(cf);
    }
    অন্যথা
      anchor = &cf->next;
  }
}

bool Curl_conn_is_setup(সংরচনা connectdata *conn, int8_t sockindex)
{
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম FALSE;
  প্রত্যাগম !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(সংরচনা connectdata *conn, int8_t sockindex)
{
  সংরচনা Curl_cfilter *cf;

  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম FALSE;
  cf = conn->cfilter[sockindex];
  যদি(cf)
    প্রত্যাগম (bool)cf->connected;
  অন্যথা যদি(conn->scheme->flags & PROTOPT_NONETWORK)
    প্রত্যাগম TRUE;
  প্রত্যাগম FALSE;
}

bool Curl_conn_is_ip_connected(সংরচনা Curl_easy *data, int8_t sockindex)
{
  সংরচনা Curl_cfilter *cf;

  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম FALSE;
  cf = data->conn->cfilter[sockindex];
  যাবত্(cf) {
    যদি(cf->connected)
      প্রত্যাগম TRUE;
    যদি(cf->cft->flags & CF_TYPE_IP_CONNECT)
      প্রত্যাগম FALSE;
    cf = cf->next;
  }
  প্রত্যাগম FALSE;
}

#ifndef CURL_DISABLE_PROXY
স্থির bool cf_is_tunneling(সংরচনা Curl_cfilter *cf)
{
  প্রতি(; cf; cf = cf->next) {
    যদি((cf->cft->flags & CF_TYPE_PROXY))
      প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}

bool Curl_conn_is_tunneling(সংরচনা connectdata *conn, int8_t sockindex)
{
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম FALSE;
  প্রত্যাগম conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#অন্যথা
bool Curl_conn_is_tunneling(সংরচনা connectdata *conn, int8_t sockindex)
{
  (শূন্য)conn;
  (শূন্য)sockindex;
  প্রত্যাগম FALSE;
}
#endif /* CURL_DISABLE_PROXY */

স্থির bool cf_is_ssl(সংরচনা Curl_cfilter *cf)
{
  প্রতি(; cf; cf = cf->next) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    যদি((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      প্রত্যাগম TRUE;
    যদি(cf->cft->flags & CF_TYPE_IP_CONNECT)
      প্রত্যাগম FALSE;
  }
  প্রত্যাগম FALSE;
}

bool Curl_conn_is_ssl(সংরচনা connectdata *conn, int8_t sockindex)
{
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম FALSE;
  প্রত্যাগম conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(সংরচনা Curl_easy *data,
                            সংরচনা connectdata *conn, int8_t sockindex,
                            পূর্ণাঙ্ক query,
                            সংরচনা curl_tlssessioninfo *info)
{
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম FALSE;
  যদি(Curl_conn_is_ssl(conn, sockindex)) {
    সংরচনা Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode result = cf ?
      cf->cft->query(cf, data, query, রিক্ত, (শূন্য *)info) :
      CURLE_UNKNOWN_OPTION;
    প্রত্যাগম !result;
  }
  প্রত্যাগম FALSE;
}

CURLcode Curl_conn_get_ip_info(সংরচনা Curl_easy *data,
                               সংরচনা connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, সংরচনা ip_quadruple *ipquad)
{
  সংরচনা Curl_cfilter *cf;
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : রিক্ত;
  প্রত্যাগম Curl_conn_cf_get_ip_info(cf, data, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(সংরচনা connectdata *conn, int8_t sockindex)
{
  সংরচনা Curl_cfilter *cf;

  যদি(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম FALSE;
  প্রতি(cf = conn->cfilter[sockindex]; cf; cf = cf->next) {
    যদি(cf->cft->flags & CF_TYPE_MULTIPLEX)
      প্রত্যাগম TRUE;
    যদি(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      প্রত্যাগম FALSE;
  }
  প্রত্যাগম FALSE;
}

unsigned বর্ণ Curl_conn_get_transport(সংরচনা Curl_easy *data,
                                      সংরচনা connectdata *conn)
{
  সংরচনা Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  প্রত্যাগম Curl_conn_cf_get_transport(cf, data);
}

পূর্ণাঙ্ক Curl_socktype_for_transport(uint8_t transport)
{
  বিকল্প(transport) {
  পক্ষ TRNSPRT_TCP:
    প্রত্যাগম SOCK_STREAM;
  পক্ষ TRNSPRT_UNIX:
    প্রত্যাগম SOCK_STREAM;
  স্বভাব: /* UDP and QUIC */
    প্রত্যাগম SOCK_DGRAM;
  }
}

পূর্ণাঙ্ক Curl_protocol_for_transport(uint8_t transport)
{
  বিকল্প(transport) {
  পক্ষ TRNSPRT_TCP:
    প্রত্যাগম IPPROTO_TCP;
  পক্ষ TRNSPRT_UNIX:
    প্রত্যাগম IPPROTO_IP;
  স্বভাব: /* UDP and QUIC */
    প্রত্যাগম IPPROTO_UDP;
  }
}

ধ্রুব বর্ণ *Curl_conn_get_alpn_negotiated(সংরচনা Curl_easy *data,
                                          সংরচনা connectdata *conn)
{
  সংরচনা Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  প্রত্যাগম Curl_conn_cf_get_alpn_negotiated(cf, data);
}

unsigned বর্ণ Curl_conn_http_version(সংরচনা Curl_easy *data,
                                     সংরচনা connectdata *conn)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result = CURLE_UNKNOWN_OPTION;
  unsigned বর্ণ v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  প্রতি(; cf; cf = cf->next) {
    যদি(cf->cft->flags & CF_TYPE_HTTP) {
      পূর্ণাঙ্ক মান = 0;
      result = cf->cft->query(cf, data, CF_QUERY_HTTP_VERSION, &মান, রিক্ত);
      যদি(!result && ((মান < 0) || (মান > 255)))
        result = CURLE_FAILED_INIT;
      অন্যথা
        v = (unsigned বর্ণ)মান;
      ভঙ্গ;
    }
    যদি(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ভঙ্গ;
  }
  প্রত্যাগম (unsigned বর্ণ)(result ? 0 : v);
}

bool Curl_conn_data_pending(সংরচনা Curl_easy *data, int8_t sockindex)
{
  সংরচনা Curl_cfilter *cf;

  (শূন্য)data;
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম FALSE;

  cf = data->conn->cfilter[sockindex];
  যাবত্(cf && !cf->connected) {
    cf = cf->next;
  }
  যদি(cf) {
    প্রত্যাগম cf->cft->has_data_pending(cf, data);
  }
  প্রত্যাগম FALSE;
}

bool Curl_conn_cf_needs_flush(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data)
{
  CURLcode result;
  পূর্ণাঙ্ক pending = 0;
  result = cf ? cf->cft->query(cf, data, CF_QUERY_NEED_FLUSH,
                               &pending, রিক্ত) : CURLE_UNKNOWN_OPTION;
  প্রত্যাগম (result || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(সংরচনা Curl_easy *data, int8_t sockindex)
{
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম FALSE;
  প্রত্যাগম Curl_conn_cf_needs_flush(data->conn->cfilter[sockindex], data);
}

CURLcode Curl_conn_cf_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                     সংরচনা Curl_easy *data,
                                     সংরচনা easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  প্রতি(; cf && !result; cf = cf->next) {
    যদি(cf->shutdown)
      অনুবর্ত;
    যদি(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->next || cf->next->connected)
      result = cf->cft->adjust_pollset(cf, data, ps);
  }
  প্রত্যাগম result;
}

CURLcode Curl_conn_adjust_pollset(সংরচনা Curl_easy *data,
                                  সংরচনা connectdata *conn,
                                  সংরচনা easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  ধ্রুব bool want_io = !!ps->n;
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
  প্রতি(i = 0; (i < (পূর্ণাঙ্ক)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    যদি(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      result = Curl_conn_cf_adjust_pollset(conn->cfilter[i], data, ps);
  }
  প্রত্যাগম result;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
পূর্ণাঙ্ক Curl_conn_cf_poll(সংরচনা Curl_cfilter *cf,
                      সংরচনা Curl_easy *data,
                      timediff_t timeout_ms)
{
  সংরচনা easy_pollset ps;
  পূর্ণাঙ্ক rc;
  CURLcode result;

  DEBUGASSERT(cf);
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  Curl_pollset_init(&ps);

  result = Curl_conn_cf_adjust_pollset(cf, data, &ps);
  যদি(!result)
    rc = Curl_pollset_poll(data, &ps, timeout_ms);
  অন্যথা
    rc = -1;
  Curl_pollset_cleanup(&ps);
  প্রত্যাগম rc;
}

শূন্য Curl_conn_get_current_host(সংরচনা Curl_easy *data, int8_t sockindex,
                                ধ্রুব বর্ণ **phost, পূর্ণাঙ্ক *pport)
{
  সংরচনা Curl_cfilter *cf, *cf_proxy = রিক্ত;
  পূর্ণাঙ্ক portarg = -1;

  যদি(!data->conn) {
    DEBUGASSERT(0);
    *phost = "";
    যদি(pport)
      *pport = -1;
    প্রত্যাগম;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? data->conn->cfilter[sockindex] : রিক্ত;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  যাবত্(cf && !cf->connected) {
    যদি((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->next;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  যদি(!cf_proxy || cf_proxy->cft->query(cf_proxy, data, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = data->conn->origin->hostname;
    portarg = data->conn->origin->port;
  }
  যদি(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(সংরচনা Curl_cfilter *cf,
                           সংরচনা Curl_easy *data,
                           পূর্ণাঙ্ক event, পূর্ণাঙ্ক arg1, শূন্য *arg2)
{
  (শূন্য)cf;
  (শূন্য)data;
  (শূন্য)event;
  (শূন্য)arg1;
  (শূন্য)arg2;
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data,
                            bool ignore_result,
                            পূর্ণাঙ্ক event, পূর্ণাঙ্ক arg1, শূন্য *arg2)
{
  CURLcode result = CURLE_OK;

  প্রতি(; cf; cf = cf->next) {
    যদি(cf->cft->cntrl == Curl_cf_def_cntrl)
      অনুবর্ত;
    result = cf->cft->cntrl(cf, data, event, arg1, arg2);
    যদি(!ignore_result && result)
      ভঙ্গ;
  }
  প্রত্যাগম result;
}

curl_socket_t Curl_conn_cf_get_socket(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data)
{
  curl_socket_t sock;
  যদি(cf && !cf->cft->query(cf, data, CF_QUERY_SOCKET, রিক্ত, &sock))
    প্রত্যাগম sock;
  প্রত্যাগম CURL_SOCKET_BAD;
}

unsigned বর্ণ Curl_conn_cf_get_transport(সংরচনা Curl_cfilter *cf,
                                         সংরচনা Curl_easy *data)
{
  পূর্ণাঙ্ক transport = 0;
  যদি(cf && !cf->cft->query(cf, data, CF_QUERY_TRANSPORT, &transport, রিক্ত))
    প্রত্যাগম (unsigned বর্ণ)transport;
  প্রত্যাগম (unsigned বর্ণ)(data->conn ? data->conn->transport_wanted : 0);
}

ধ্রুব বর্ণ *Curl_conn_cf_get_alpn_negotiated(সংরচনা Curl_cfilter *cf,
                                             সংরচনা Curl_easy *data)
{
  ধ্রুব বর্ণ *alpn = রিক্ত;
  CURL_TRC_CF(data, cf, "query ALPN");
  যদি(cf && !cf->cft->query(cf, data, CF_QUERY_ALPN_NEGOTIATED, রিক্ত,
                           CURL_UNCONST(&alpn)))
    প্রত্যাগম alpn;
  প্রত্যাগম রিক্ত;
}

স্থির ধ্রুব সংরচনা Curl_sockaddr_ex *cf_get_remote_addr(
  সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  ধ্রুব সংরচনা Curl_sockaddr_ex *remote_addr = রিক্ত;
  যদি(cf &&
     !cf->cft->query(cf, data, CF_QUERY_REMOTE_ADDR, রিক্ত,
                     CURL_UNCONST(&remote_addr)))
    প্রত্যাগম remote_addr;
  প্রত্যাগম রিক্ত;
}

CURLcode Curl_conn_cf_get_ip_info(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  bool *is_ipv6, সংরচনা ip_quadruple *ipquad)
{
  CURLcode result = CURLE_UNKNOWN_OPTION;
  যদি(cf) {
    পূর্ণাঙ্ক ipv6 = 0;
    result = cf->cft->query(cf, data, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  প্রত্যাগম result;
}

curl_socket_t Curl_conn_get_first_socket(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cfilter *cf;

  যদি(!data->conn)
    প্রত্যাগম CURL_SOCKET_BAD;

  cf = data->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  যদি(cf && !cf->connected)
    প্রত্যাগম Curl_conn_cf_get_socket(cf, data);
  প্রত্যাগম data->conn->sock[FIRSTSOCKET];
}

ধ্রুব সংরচনা Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  সংরচনা Curl_easy *data, int8_t sockindex)
{
  সংরচনা Curl_cfilter *cf =
    (data->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    data->conn->cfilter[sockindex] : রিক্ত;
  প্রত্যাগম cf ? cf_get_remote_addr(cf, data) : রিক্ত;
}

CURLcode Curl_conn_ev_data_setup(সংরচনা Curl_easy *data)
{
  প্রত্যাগম cf_cntrl_all(data->conn, data, FALSE, CF_CTRL_DATA_SETUP, 0, রিক্ত);
}

CURLcode Curl_conn_flush(সংরচনা Curl_easy *data, int8_t sockindex)
{
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  প্রত্যাগম Curl_conn_cf_cntrl(data->conn->cfilter[sockindex], data, FALSE,
                            CF_CTRL_FLUSH, 0, রিক্ত);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
শূন্য Curl_conn_ev_data_done_send(সংরচনা Curl_easy *data)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE_SEND, 0, রিক্ত);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
শূন্য Curl_conn_ev_data_done(সংরচনা Curl_easy *data, bool premature)
{
  cf_cntrl_all(data->conn, data, TRUE, CF_CTRL_DATA_DONE, premature, রিক্ত);
}

CURLcode Curl_conn_ev_data_pause(সংরচনা Curl_easy *data, bool do_pause)
{
  প্রত্যাগম cf_cntrl_all(data->conn, data, FALSE,
                      CF_CTRL_DATA_PAUSE, do_pause, রিক্ত);
}

bool Curl_conn_is_alive(সংরচনা Curl_easy *data, সংরচনা connectdata *conn,
                        bool *প্রবেশ_pending)
{
  সংরচনা Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  প্রত্যাগম cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, data, প্রবেশ_pending);
}

CURLcode Curl_conn_keep_alive(সংরচনা Curl_easy *data,
                              সংরচনা connectdata *conn)
{
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক i;

  প্রতি(i = 0; (i < (পূর্ণাঙ্ক)CURL_ARRAYSIZE(conn->cfilter)) && !result; ++i) {
    সংরচনা Curl_cfilter *cf = conn->cfilter[i];
    যদি(cf)
      result = cf->cft->keep_alive(cf, data);
  }
  প্রত্যাগম result;
}

size_t Curl_conn_get_max_concurrent(সংরচনা Curl_easy *data,
                                    সংরচনা connectdata *conn,
                                    int8_t sockindex)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result;
  পূর্ণাঙ্ক n = -1;

  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_MAX_CONCURRENT,
                               &n, রিক্ত) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  প্রত্যাগম (result || n < 0) ? 1 : (size_t)n;
}

পূর্ণাঙ্ক Curl_conn_get_stream_error(সংরচনা Curl_easy *data,
                               সংরচনা connectdata *conn,
                               int8_t sockindex)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result;
  পূর্ণাঙ্ক n = 0;

  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম 0;

  cf = conn->cfilter[sockindex];
  result = cf ? cf->cft->query(cf, data, CF_QUERY_STREAM_ERROR,
                               &n, রিক্ত) : CURLE_UNKNOWN_OPTION;
  প্রত্যাগম (result || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(সংরচনা Curl_easy *data, curl_socket_t sockfd)
{
  যদি(data && data->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == data->conn->sock[SECONDARYSOCKET])
    প্রত্যাগম SECONDARYSOCKET;
  প্রত্যাগম FIRSTSOCKET;
}

CURLcode Curl_conn_recv(সংরচনা Curl_easy *data, int8_t sockindex,
                        বর্ণ *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  যদি(data && data->conn && data->conn->recv[sockindex])
    প্রত্যাগম data->conn->recv[sockindex](data, sockindex, buf, len, pnread);
  *pnread = 0;
  প্রত্যাগম CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(সংরচনা Curl_easy *data, int8_t sockindex,
                        ধ্রুব শূন্য *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(data);
  DEBUGASSERT(data->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  যদি(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    ধ্রুব বর্ণ *p = getenv("CURL_SMALLSENDS");
    যদি(p) {
      curl_off_t altsize;
      যদি(!curlx_str_সংখ্যা(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        যদি(write_len != len)
          eos = FALSE;
      }
    }
  }
#endif
  যদি(data && data->conn && data->conn->send[sockindex]) {
    CURLcode result = data->conn->send[sockindex](data, sockindex, buf,
                                                  write_len, eos, pnwritten);
    যদি(!result && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      প্রত্যাগম CURLE_SEND_ERROR;
    }
    প্রত্যাগম result;
  }
  *pnwritten = 0;
  প্রত্যাগম CURLE_FAILED_INIT;
}
