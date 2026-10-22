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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "strerror.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "curlx/strparse.h"

CURLcode Curl_cf_def_connect(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం;

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  ఒకవేళ(cf->తదుపరి) {
    ఫలితం = cf->తదుపరి->cft->do_connect(cf->తదుపరి, దత్తాంశం, పూర్తి);
    ఒకవేళ(ఫలితం || !*పూర్తి)
      తిరిగి ఫలితం;
  }

  cf->connected = TRUE;
  *పూర్తి = TRUE;
  తిరిగి CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  (శూన్య)cf;
  (శూన్య)దత్తాంశం;
  *పూర్తి = TRUE;
  తిరిగి CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం easy_pollset *ps)
{
  /* NOP */
  (శూన్య)cf;
  (శూన్య)దత్తాంశం;
  (శూన్య)ps;
  తిరిగి CURLE_OK;
}

bool Curl_cf_def_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                              స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->has_దత్తాంశం_pending(cf->తదుపరి, దత్తాంశం) : FALSE;
}

CURLcode Curl_cf_def_send(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                          స్థిరరాశి uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  ఒకవేళ(cf->తదుపరి)
    తిరిగి cf->తదుపరి->cft->do_send(cf->తదుపరి, దత్తాంశం, buf, len, eos, pnwritten);
  *pnwritten = 0;
  తిరిగి CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                          అక్షరం *buf, size_t len, size_t *pnread)
{
  ఒకవేళ(cf->తదుపరి)
    తిరిగి cf->తదుపరి->cft->do_recv(cf->తదుపరి, దత్తాంశం, buf, len, pnread);
  *pnread = 0;
  తిరిగి CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               bool *ఆదానం_pending)
{
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->is_alive(cf->తదుపరి, దత్తాంశం, ఆదానం_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->keep_alive(cf->తదుపరి, దత్తాంశం) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->query(cf->తదుపరి, దత్తాంశం, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#నిర్వచితమైతే CURLVERBOSE
శూన్య Curl_conn_trc_filters(నిర్మాణం Curl_easy *దత్తాంశం,
                           int8_t sockindex, స్థిరరాశి అక్షరం *info)
{
  ఒకవేళ(CURL_TRC_M_is_verbose(దత్తాంశం) && దత్తాంశం->conn) {
    నిర్మాణం Curl_cfilter *cf = దత్తాంశం->conn->cfilter[sockindex];

    ఒకవేళ(cf) {
      అక్షరం సందేశం[256], *buf;
      పూర్ణాంకం blen, n;

      buf = సందేశం;
      blen = పరిమాణం(సందేశం) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      కొరకు(; cf && blen; cf = cf->తదుపరి) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(దత్తాంశం, "%s%s", సందేశం, blen ? "" : "...");
    }
    లేకపోతే
      CURL_TRC_M(దత్తాంశం, "%s [%d][-]", info, sockindex);
  }
}
#అయితే_ముగింపు /* CURLVERBOSE */

శూన్య Curl_conn_cf_discard_chain(నిర్మాణం Curl_cfilter **pcf,
                                నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cfn, *cf = *pcf;

  ఒకవేళ(cf) {
    *pcf = ఖాళీ;
    అయితే(cf) {
      cfn = cf->తదుపరి;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->తదుపరి = ఖాళీ;
      cf->cft->destroy(cf, దత్తాంశం);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

శూన్య Curl_conn_cf_discard_all(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn, int8_t sockindex)
{
  conn->shutdown.ప్రారంభం_ms[sockindex] = -1;
  conn->connect_started.సమితి[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], దత్తాంశం);
}

CURLcode Curl_cf_recv(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex, అక్షరం *buf,
                      size_t len, size_t *pnread)
{
  నిర్మాణం Curl_cfilter *cf;

  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->conn);
  cf = దత్తాంశం->conn->cfilter[sockindex];
  అయితే(cf && !cf->connected)
    cf = cf->తదుపరి;
  ఒకవేళ(cf)
    తిరిగి cf->cft->do_recv(cf, దత్తాంశం, buf, len, pnread);
  failf(దత్తాంశం, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  తిరిగి CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                      స్థిరరాశి uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  నిర్మాణం Curl_cfilter *cf;

  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->conn);
  cf = దత్తాంశం->conn->cfilter[sockindex];
  అయితే(cf && !cf->connected)
    cf = cf->తదుపరి;
  ఒకవేళ(cf) {
    తిరిగి cf->cft->do_send(cf, దత్తాంశం, buf, len, eos, pnwritten);
  }
  failf(దత్తాంశం, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  తిరిగి CURLE_FAILED_INIT;
}

నిర్మాణం cf_io_ctx {
  నిర్మాణం Curl_easy *దత్తాంశం;
  నిర్మాణం Curl_cfilter *cf;
};

స్థిర CURLcode cf_bufq_reader(శూన్య *writer_ctx,
                               గుర్తులేని అక్షరం *buf, size_t blen,
                               size_t *pnread)
{
  నిర్మాణం cf_io_ctx *io = writer_ctx;
  తిరిగి Curl_conn_cf_recv(io->cf, io->దత్తాంశం, (అక్షరం *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  నిర్మాణం cf_io_ctx io;

  ఒకవేళ(!cf || !దత్తాంశం) {
    *pnread = 0;
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }
  io.దత్తాంశం = దత్తాంశం;
  io.cf = cf;
  తిరిగి Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

స్థిర CURLcode cf_bufq_writer(శూన్య *writer_ctx,
                               స్థిరరాశి uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  నిర్మాణం cf_io_ctx *io = writer_ctx;
  తిరిగి Curl_conn_cf_send(io->cf, io->దత్తాంశం, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం bufq *bufq,
                           స్థిరరాశి గుర్తులేని అక్షరం *buf, size_t blen,
                           size_t *pnwritten)
{
  నిర్మాణం cf_io_ctx io;

  ఒకవేళ(!cf || !దత్తాంశం) {
    *pnwritten = 0;
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }
  io.దత్తాంశం = దత్తాంశం;
  io.cf = cf;
  ఒకవేళ(buf && blen)
    తిరిగి Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  లేకపోతే
    తిరిగి Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(నిర్మాణం Curl_cfilter **pcf,
                        స్థిరరాశి నిర్మాణం Curl_cftype *cft,
                        శూన్య *ctx)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, పరిమాణం(*cf));
  ఒకవేళ(!cf)
    వెళ్ళు బహిర్గతం;

  cf->cft = cft;
  cf->ctx = ctx;
  ఫలితం = CURLE_OK;
బహిర్గతం:
  *pcf = cf;
  తిరిగి ఫలితం;
}

శూన్య Curl_conn_cf_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                      నిర్మాణం connectdata *conn,
                      int8_t sockindex,
                      నిర్మాణం Curl_cfilter *cf)
{
  DEBUGASSERT(conn);
  DEBUGASSERT(!cf->conn);
  DEBUGASSERT(!cf->తదుపరి);

  cf->తదుపరి = conn->cfilter[sockindex];
  cf->conn = conn;
  cf->sockindex = sockindex;
  conn->cfilter[sockindex] = cf;
  CURL_TRC_CF(దత్తాంశం, cf, "added");
}

శూన్య Curl_conn_cf_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                               నిర్మాణం Curl_cfilter *cf_new)
{
  నిర్మాణం Curl_cfilter *తోక, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  తోక = cf_at->తదుపరి;
  cf_at->తదుపరి = cf_new;
  చేయి {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->తదుపరి;
    cf_new = cf_new->తదుపరి;
  } అయితే(cf_new);
  *pnext = తోక;
}

bool Curl_conn_cf_discard(నిర్మాణం Curl_cfilter **pcf,
                          నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = pcf ? *pcf : ఖాళీ;
  bool దొరికింది = FALSE;
  ఒకవేళ(cf) {
    ఒకవేళ(cf->conn) {
      /* unlink if present in connection filter chain */
      నిర్మాణం Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      అయితే(*pprev) {
        ఒకవేళ(*pprev == *pcf) {
          *pprev = (*pcf)->తదుపరి;
          cf->తదుపరి = ఖాళీ;
          దొరికింది = TRUE;
          ఆపు;
        }
        pprev = &(*pprev)->తదుపరి;
      }
    }
    Curl_conn_cf_discard_chain(pcf, దత్తాంశం);
  }
  తిరిగి దొరికింది;
}

CURLcode Curl_conn_cf_connect(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              bool *పూర్తి)
{
  ఒకవేళ(cf)
    తిరిగి cf->cft->do_connect(cf, దత్తాంశం, పూర్తి);
  తిరిగి CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                           స్థిరరాశి uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ఒకవేళ(cf)
    తిరిగి cf->cft->do_send(cf, దత్తాంశం, buf, len, eos, pnwritten);
  *pnwritten = 0;
  తిరిగి CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                           అక్షరం *buf, size_t len, size_t *pnread)
{
  ఒకవేళ(cf)
    తిరిగి cf->cft->do_recv(cf, దత్తాంశం, buf, len, pnread);
  *pnread = 0;
  తిరిగి CURLE_RECV_ERROR;
}

స్థిర CURLcode cf_cntrl_all(నిర్మాణం connectdata *conn,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             bool ignore_ఫలితం,
                             పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం i;

  కొరకు(i = 0; i < (పూర్ణాంకం)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    ఫలితం = Curl_conn_cf_cntrl(conn->cfilter[i], దత్తాంశం, ignore_ఫలితం,
                                event, arg1, arg2);
    ఒకవేళ(!ignore_ఫలితం && ఫలితం)
      ఆపు;
  }
  తిరిగి ఫలితం;
}

bool Curl_conn_cf_is_ip_connected(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(cf) {
    పూర్ణాంకం విలువ = 0;
    ఒకవేళ(!cf->cft->query(cf, దత్తాంశం, CF_QUERY_REALLY_CONNECTED, &విలువ, ఖాళీ))
      తిరిగి !!విలువ;
  }
  తిరిగి FALSE;
}

శూన్య Curl_conn_cntrl_update_info(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం connectdata *conn)
{
  cf_cntrl_all(conn, దత్తాంశం, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, ఖాళీ);
}

శూన్య Curl_conn_cntrl_report_stats(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn,
                                  పూర్ణాంకం sockindex)
{
  ఒకవేళ((గుర్తులేని)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (శూన్య)Curl_conn_cf_cntrl(conn->cfilter[sockindex], దత్తాంశం, TRUE,
                             CF_CTRL_REPORT_STATS, 0, ఖాళీ);
}

శూన్య Curl_conn_remove_అమరిక_filters(నిర్మాణం Curl_easy *దత్తాంశం,
                                    int8_t sockindex)
{
  నిర్మాణం Curl_cfilter **anchor = &దత్తాంశం->conn->cfilter[sockindex];
  అయితే(*anchor) {
    నిర్మాణం Curl_cfilter *cf = *anchor;
    ఒకవేళ(cf->connected && (cf->cft->flags & CF_TYPE_అమరిక)) {
      *anchor = cf->తదుపరి;
      cf->తదుపరి = ఖాళీ;
      CURL_TRC_CF(దత్తాంశం, cf, "removing connected setup filter");
      cf->cft->destroy(cf, దత్తాంశం);
      curlx_free(cf);
    }
    లేకపోతే
      anchor = &cf->తదుపరి;
  }
}

bool Curl_conn_is_అమరిక(నిర్మాణం connectdata *conn, int8_t sockindex)
{
  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి FALSE;
  తిరిగి !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(నిర్మాణం connectdata *conn, int8_t sockindex)
{
  నిర్మాణం Curl_cfilter *cf;

  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి FALSE;
  cf = conn->cfilter[sockindex];
  ఒకవేళ(cf)
    తిరిగి (bool)cf->connected;
  లేకపోతే ఒకవేళ(conn->scheme->flags & PROTOPT_NONETWORK)
    తిరిగి TRUE;
  తిరిగి FALSE;
}

bool Curl_conn_is_ip_connected(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex)
{
  నిర్మాణం Curl_cfilter *cf;

  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి FALSE;
  cf = దత్తాంశం->conn->cfilter[sockindex];
  అయితే(cf) {
    ఒకవేళ(cf->connected)
      తిరిగి TRUE;
    ఒకవేళ(cf->cft->flags & CF_TYPE_IP_CONNECT)
      తిరిగి FALSE;
    cf = cf->తదుపరి;
  }
  తిరిగి FALSE;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
స్థిర bool cf_is_tunneling(నిర్మాణం Curl_cfilter *cf)
{
  కొరకు(; cf; cf = cf->తదుపరి) {
    ఒకవేళ((cf->cft->flags & CF_TYPE_PROXY))
      తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

bool Curl_conn_is_tunneling(నిర్మాణం connectdata *conn, int8_t sockindex)
{
  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి FALSE;
  తిరిగి conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#లేకపోతే
bool Curl_conn_is_tunneling(నిర్మాణం connectdata *conn, int8_t sockindex)
{
  (శూన్య)conn;
  (శూన్య)sockindex;
  తిరిగి FALSE;
}
#అయితే_ముగింపు /* CURL_DISABLE_PROXY */

స్థిర bool cf_is_ssl(నిర్మాణం Curl_cfilter *cf)
{
  కొరకు(; cf; cf = cf->తదుపరి) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    ఒకవేళ((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      తిరిగి TRUE;
    ఒకవేళ(cf->cft->flags & CF_TYPE_IP_CONNECT)
      తిరిగి FALSE;
  }
  తిరిగి FALSE;
}

bool Curl_conn_is_ssl(నిర్మాణం connectdata *conn, int8_t sockindex)
{
  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి FALSE;
  తిరిగి conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం connectdata *conn, int8_t sockindex,
                            పూర్ణాంకం query,
                            నిర్మాణం curl_tlssessioninfo *info)
{
  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి FALSE;
  ఒకవేళ(Curl_conn_is_ssl(conn, sockindex)) {
    నిర్మాణం Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode ఫలితం = cf ?
      cf->cft->query(cf, దత్తాంశం, query, ఖాళీ, (శూన్య *)info) :
      CURLE_UNKNOWN_OPTION;
    తిరిగి !ఫలితం;
  }
  తిరిగి FALSE;
}

CURLcode Curl_conn_get_ip_info(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, నిర్మాణం ip_quadruple *ipquad)
{
  నిర్మాణం Curl_cfilter *cf;
  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  cf = conn ? conn->cfilter[sockindex] : ఖాళీ;
  తిరిగి Curl_conn_cf_get_ip_info(cf, దత్తాంశం, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(నిర్మాణం connectdata *conn, int8_t sockindex)
{
  నిర్మాణం Curl_cfilter *cf;

  ఒకవేళ(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి FALSE;
  కొరకు(cf = conn->cfilter[sockindex]; cf; cf = cf->తదుపరి) {
    ఒకవేళ(cf->cft->flags & CF_TYPE_MULTIPLEX)
      తిరిగి TRUE;
    ఒకవేళ(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      తిరిగి FALSE;
  }
  తిరిగి FALSE;
}

గుర్తులేని అక్షరం Curl_conn_get_transport(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం connectdata *conn)
{
  నిర్మాణం Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  తిరిగి Curl_conn_cf_get_transport(cf, దత్తాంశం);
}

పూర్ణాంకం Curl_socktype_for_transport(uint8_t transport)
{
  ఎంపిక(transport) {
  స్థితి TRNSPRT_TCP:
    తిరిగి SOCK_STREAM;
  స్థితి TRNSPRT_UNIX:
    తిరిగి SOCK_STREAM;
  అప్రమేయ: /* UDP and QUIC */
    తిరిగి SOCK_DGRAM;
  }
}

పూర్ణాంకం Curl_protocol_for_transport(uint8_t transport)
{
  ఎంపిక(transport) {
  స్థితి TRNSPRT_TCP:
    తిరిగి IPPROTO_TCP;
  స్థితి TRNSPRT_UNIX:
    తిరిగి IPPROTO_IP;
  అప్రమేయ: /* UDP and QUIC */
    తిరిగి IPPROTO_UDP;
  }
}

స్థిరరాశి అక్షరం *Curl_conn_get_alpn_negotiated(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం connectdata *conn)
{
  నిర్మాణం Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  తిరిగి Curl_conn_cf_get_alpn_negotiated(cf, దత్తాంశం);
}

గుర్తులేని అక్షరం Curl_conn_http_సంస్కరణ(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం connectdata *conn)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం = CURLE_UNKNOWN_OPTION;
  గుర్తులేని అక్షరం v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  కొరకు(; cf; cf = cf->తదుపరి) {
    ఒకవేళ(cf->cft->flags & CF_TYPE_HTTP) {
      పూర్ణాంకం విలువ = 0;
      ఫలితం = cf->cft->query(cf, దత్తాంశం, CF_QUERY_HTTP_సంస్కరణ, &విలువ, ఖాళీ);
      ఒకవేళ(!ఫలితం && ((విలువ < 0) || (విలువ > 255)))
        ఫలితం = CURLE_FAILED_INIT;
      లేకపోతే
        v = (గుర్తులేని అక్షరం)విలువ;
      ఆపు;
    }
    ఒకవేళ(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ఆపు;
  }
  తిరిగి (గుర్తులేని అక్షరం)(ఫలితం ? 0 : v);
}

bool Curl_conn_దత్తాంశం_pending(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex)
{
  నిర్మాణం Curl_cfilter *cf;

  (శూన్య)దత్తాంశం;
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->conn);
  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి FALSE;

  cf = దత్తాంశం->conn->cfilter[sockindex];
  అయితే(cf && !cf->connected) {
    cf = cf->తదుపరి;
  }
  ఒకవేళ(cf) {
    తిరిగి cf->cft->has_దత్తాంశం_pending(cf, దత్తాంశం);
  }
  తిరిగి FALSE;
}

bool Curl_conn_cf_needs_flush(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం;
  పూర్ణాంకం pending = 0;
  ఫలితం = cf ? cf->cft->query(cf, దత్తాంశం, CF_QUERY_NEED_FLUSH,
                               &pending, ఖాళీ) : CURLE_UNKNOWN_OPTION;
  తిరిగి (ఫలితం || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex)
{
  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి FALSE;
  తిరిగి Curl_conn_cf_needs_flush(దత్తాంశం->conn->cfilter[sockindex], దత్తాంశం);
}

CURLcode Curl_conn_cf_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం easy_pollset *ps)
{
  CURLcode ఫలితం = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  కొరకు(; cf && !ఫలితం; cf = cf->తదుపరి) {
    ఒకవేళ(cf->shutdown)
      కొనసాగు;
    ఒకవేళ(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->తదుపరి || cf->తదుపరి->connected)
      ఫలితం = cf->cft->adjust_pollset(cf, దత్తాంశం, ps);
  }
  తిరిగి ఫలితం;
}

CURLcode Curl_conn_adjust_pollset(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn,
                                  నిర్మాణం easy_pollset *ps)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి bool want_io = !!ps->n;
  int8_t i;

  DEBUGASSERT(దత్తాంశం);
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
  కొరకు(i = 0; (i < (పూర్ణాంకం)CURL_ARRAYSIZE(conn->cfilter)) && !ఫలితం; ++i) {
    ఒకవేళ(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_లోపల_SHUTDOWN(conn, i)))
      ఫలితం = Curl_conn_cf_adjust_pollset(conn->cfilter[i], దత్తాంశం, ps);
  }
  తిరిగి ఫలితం;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
పూర్ణాంకం Curl_conn_cf_poll(నిర్మాణం Curl_cfilter *cf,
                      నిర్మాణం Curl_easy *దత్తాంశం,
                      timediff_t timeout_ms)
{
  నిర్మాణం easy_pollset ps;
  పూర్ణాంకం rc;
  CURLcode ఫలితం;

  DEBUGASSERT(cf);
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->conn);
  Curl_pollset_init(&ps);

  ఫలితం = Curl_conn_cf_adjust_pollset(cf, దత్తాంశం, &ps);
  ఒకవేళ(!ఫలితం)
    rc = Curl_pollset_poll(దత్తాంశం, &ps, timeout_ms);
  లేకపోతే
    rc = -1;
  Curl_pollset_cleanup(&ps);
  తిరిగి rc;
}

శూన్య Curl_conn_get_current_host(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                                స్థిరరాశి అక్షరం **phost, పూర్ణాంకం *pport)
{
  నిర్మాణం Curl_cfilter *cf, *cf_proxy = ఖాళీ;
  పూర్ణాంకం portarg = -1;

  ఒకవేళ(!దత్తాంశం->conn) {
    DEBUGASSERT(0);
    *phost = "";
    ఒకవేళ(pport)
      *pport = -1;
    తిరిగి;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? దత్తాంశం->conn->cfilter[sockindex] : ఖాళీ;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  అయితే(cf && !cf->connected) {
    ఒకవేళ((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->తదుపరి;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  ఒకవేళ(!cf_proxy || cf_proxy->cft->query(cf_proxy, దత్తాంశం, CF_QUERY_HOST_పోర్టు,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = దత్తాంశం->conn->origin->hostname;
    portarg = దత్తాంశం->conn->origin->పోర్టు;
  }
  ఒకవేళ(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  (శూన్య)cf;
  (శూన్య)దత్తాంశం;
  (శూన్య)event;
  (శూన్య)arg1;
  (శూన్య)arg2;
  తిరిగి CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            bool ignore_ఫలితం,
                            పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  CURLcode ఫలితం = CURLE_OK;

  కొరకు(; cf; cf = cf->తదుపరి) {
    ఒకవేళ(cf->cft->cntrl == Curl_cf_def_cntrl)
      కొనసాగు;
    ఫలితం = cf->cft->cntrl(cf, దత్తాంశం, event, arg1, arg2);
    ఒకవేళ(!ignore_ఫలితం && ఫలితం)
      ఆపు;
  }
  తిరిగి ఫలితం;
}

curl_socket_t Curl_conn_cf_get_socket(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  curl_socket_t sock;
  ఒకవేళ(cf && !cf->cft->query(cf, దత్తాంశం, CF_QUERY_SOCKET, ఖాళీ, &sock))
    తిరిగి sock;
  తిరిగి CURL_SOCKET_BAD;
}

గుర్తులేని అక్షరం Curl_conn_cf_get_transport(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం)
{
  పూర్ణాంకం transport = 0;
  ఒకవేళ(cf && !cf->cft->query(cf, దత్తాంశం, CF_QUERY_TRANSPORT, &transport, ఖాళీ))
    తిరిగి (గుర్తులేని అక్షరం)transport;
  తిరిగి (గుర్తులేని అక్షరం)(దత్తాంశం->conn ? దత్తాంశం->conn->transport_wanted : 0);
}

స్థిరరాశి అక్షరం *Curl_conn_cf_get_alpn_negotiated(నిర్మాణం Curl_cfilter *cf,
                                             నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి అక్షరం *alpn = ఖాళీ;
  CURL_TRC_CF(దత్తాంశం, cf, "query ALPN");
  ఒకవేళ(cf && !cf->cft->query(cf, దత్తాంశం, CF_QUERY_ALPN_NEGOTIATED, ఖాళీ,
                           CURL_UNCONST(&alpn)))
    తిరిగి alpn;
  తిరిగి ఖాళీ;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_sockaddr_ex *cf_get_remote_addr(
  నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి నిర్మాణం Curl_sockaddr_ex *remote_addr = ఖాళీ;
  ఒకవేళ(cf &&
     !cf->cft->query(cf, దత్తాంశం, CF_QUERY_REMOTE_ADDR, ఖాళీ,
                     CURL_UNCONST(&remote_addr)))
    తిరిగి remote_addr;
  తిరిగి ఖాళీ;
}

CURLcode Curl_conn_cf_get_ip_info(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  bool *is_ipv6, నిర్మాణం ip_quadruple *ipquad)
{
  CURLcode ఫలితం = CURLE_UNKNOWN_OPTION;
  ఒకవేళ(cf) {
    పూర్ణాంకం ipv6 = 0;
    ఫలితం = cf->cft->query(cf, దత్తాంశం, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  తిరిగి ఫలితం;
}

curl_socket_t Curl_conn_get_మొదటి_socket(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf;

  ఒకవేళ(!దత్తాంశం->conn)
    తిరిగి CURL_SOCKET_BAD;

  cf = దత్తాంశం->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  ఒకవేళ(cf && !cf->connected)
    తిరిగి Curl_conn_cf_get_socket(cf, దత్తాంశం);
  తిరిగి దత్తాంశం->conn->sock[FIRSTSOCKET];
}

స్థిరరాశి నిర్మాణం Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex)
{
  నిర్మాణం Curl_cfilter *cf =
    (దత్తాంశం->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    దత్తాంశం->conn->cfilter[sockindex] : ఖాళీ;
  తిరిగి cf ? cf_get_remote_addr(cf, దత్తాంశం) : ఖాళీ;
}

CURLcode Curl_conn_ev_దత్తాంశం_అమరిక(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి cf_cntrl_all(దత్తాంశం->conn, దత్తాంశం, FALSE, CF_CTRL_దత్తాంశం_అమరిక, 0, ఖాళీ);
}

CURLcode Curl_conn_flush(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex)
{
  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  తిరిగి Curl_conn_cf_cntrl(దత్తాంశం->conn->cfilter[sockindex], దత్తాంశం, FALSE,
                            CF_CTRL_FLUSH, 0, ఖాళీ);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
శూన్య Curl_conn_ev_దత్తాంశం_పూర్తి_send(నిర్మాణం Curl_easy *దత్తాంశం)
{
  cf_cntrl_all(దత్తాంశం->conn, దత్తాంశం, TRUE, CF_CTRL_దత్తాంశం_పూర్తి_SEND, 0, ఖాళీ);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
శూన్య Curl_conn_ev_దత్తాంశం_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, bool premature)
{
  cf_cntrl_all(దత్తాంశం->conn, దత్తాంశం, TRUE, CF_CTRL_దత్తాంశం_పూర్తి, premature, ఖాళీ);
}

CURLcode Curl_conn_ev_దత్తాంశం_pause(నిర్మాణం Curl_easy *దత్తాంశం, bool do_pause)
{
  తిరిగి cf_cntrl_all(దత్తాంశం->conn, దత్తాంశం, FALSE,
                      CF_CTRL_దత్తాంశం_PAUSE, do_pause, ఖాళీ);
}

bool Curl_conn_is_alive(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn,
                        bool *ఆదానం_pending)
{
  నిర్మాణం Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  తిరిగి cf && !cf->conn->బిట్లు.close &&
         cf->cft->is_alive(cf, దత్తాంశం, ఆదానం_pending);
}

CURLcode Curl_conn_keep_alive(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn)
{
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం i;

  కొరకు(i = 0; (i < (పూర్ణాంకం)CURL_ARRAYSIZE(conn->cfilter)) && !ఫలితం; ++i) {
    నిర్మాణం Curl_cfilter *cf = conn->cfilter[i];
    ఒకవేళ(cf)
      ఫలితం = cf->cft->keep_alive(cf, దత్తాంశం);
  }
  తిరిగి ఫలితం;
}

size_t Curl_conn_get_max_concurrent(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం connectdata *conn,
                                    int8_t sockindex)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం;
  పూర్ణాంకం n = -1;

  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి 0;

  cf = conn->cfilter[sockindex];
  ఫలితం = cf ? cf->cft->query(cf, దత్తాంశం, CF_QUERY_MAX_CONCURRENT,
                               &n, ఖాళీ) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  తిరిగి (ఫలితం || n < 0) ? 1 : (size_t)n;
}

పూర్ణాంకం Curl_conn_get_ప్రవాహం_error(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn,
                               int8_t sockindex)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం;
  పూర్ణాంకం n = 0;

  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి 0;

  cf = conn->cfilter[sockindex];
  ఫలితం = cf ? cf->cft->query(cf, దత్తాంశం, CF_QUERY_ప్రవాహం_ERROR,
                               &n, ఖాళీ) : CURLE_UNKNOWN_OPTION;
  తిరిగి (ఫలితం || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(నిర్మాణం Curl_easy *దత్తాంశం, curl_socket_t sockfd)
{
  ఒకవేళ(దత్తాంశం && దత్తాంశం->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == దత్తాంశం->conn->sock[SECONDARYSOCKET])
    తిరిగి SECONDARYSOCKET;
  తిరిగి FIRSTSOCKET;
}

CURLcode Curl_conn_recv(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                        అక్షరం *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->conn);
  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  ఒకవేళ(దత్తాంశం && దత్తాంశం->conn && దత్తాంశం->conn->recv[sockindex])
    తిరిగి దత్తాంశం->conn->recv[sockindex](దత్తాంశం, sockindex, buf, len, pnread);
  *pnread = 0;
  తిరిగి CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                        స్థిరరాశి శూన్య *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(దత్తాంశం->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  ఒకవేళ(!CONN_SOCK_IDX_VALID(sockindex))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    స్థిరరాశి అక్షరం *p = getenv("CURL_SMALLSENDS");
    ఒకవేళ(p) {
      curl_off_t altsize;
      ఒకవేళ(!curlx_str_సంఖ్య(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        ఒకవేళ(write_len != len)
          eos = FALSE;
      }
    }
  }
#అయితే_ముగింపు
  ఒకవేళ(దత్తాంశం && దత్తాంశం->conn && దత్తాంశం->conn->send[sockindex]) {
    CURLcode ఫలితం = దత్తాంశం->conn->send[sockindex](దత్తాంశం, sockindex, buf,
                                                  write_len, eos, pnwritten);
    ఒకవేళ(!ఫలితం && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      తిరిగి CURLE_SEND_ERROR;
    }
    తిరిగి ఫలితం;
  }
  *pnwritten = 0;
  తిరిగి CURLE_FAILED_INIT;
}
