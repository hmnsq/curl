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

CURLcode Curl_cf_def_connect(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল;

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  যদি(cf->পরবর্তী) {
    ফলাফল = cf->পরবর্তী->cft->do_connect(cf->পরবর্তী, তথ্য, done);
    যদি(ফলাফল || !*done)
      ফেরত ফলাফল;
  }

  cf->connected = TRUE;
  *done = TRUE;
  ফেরত CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য, bool *done)
{
  (শূন্য)cf;
  (শূন্য)তথ্য;
  *done = TRUE;
  ফেরত CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    কাঠামো easy_pollset *ps)
{
  /* NOP */
  (শূন্য)cf;
  (শূন্য)তথ্য;
  (শূন্য)ps;
  ফেরত CURLE_OK;
}

bool Curl_cf_def_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                              ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->has_তথ্য_pending(cf->পরবর্তী, তথ্য) : FALSE;
}

CURLcode Curl_cf_def_send(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                          ধ্রুবক uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  যদি(cf->পরবর্তী)
    ফেরত cf->পরবর্তী->cft->do_send(cf->পরবর্তী, তথ্য, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ফেরত CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                          অক্ষর *buf, size_t len, size_t *pnread)
{
  যদি(cf->পরবর্তী)
    ফেরত cf->পরবর্তী->cft->do_recv(cf->পরবর্তী, তথ্য, buf, len, pnread);
  *pnread = 0;
  ফেরত CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               bool *প্রবেশ_pending)
{
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->is_alive(cf->পরবর্তী, তথ্য, প্রবেশ_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(কাঠামো Curl_cfilter *cf,
                                     কাঠামো Curl_easy *তথ্য)
{
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->keep_alive(cf->পরবর্তী, তথ্য) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(কাঠামো Curl_cfilter *cf,
                           কাঠামো Curl_easy *তথ্য,
                           পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#ifdef CURLVERBOSE
শূন্য Curl_conn_trc_filters(কাঠামো Curl_easy *তথ্য,
                           int8_t sockindex, ধ্রুবক অক্ষর *info)
{
  যদি(CURL_TRC_M_is_verbose(তথ্য) && তথ্য->conn) {
    কাঠামো Curl_cfilter *cf = তথ্য->conn->cfilter[sockindex];

    যদি(cf) {
      অক্ষর বার্তা[256], *buf;
      পূর্ণসংখ্যা blen, n;

      buf = বার্তা;
      blen = আকার(বার্তা) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      জন্য(; cf && blen; cf = cf->পরবর্তী) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(তথ্য, "%s%s", বার্তা, blen ? "" : "...");
    }
    অন্যথা
      CURL_TRC_M(তথ্য, "%s [%d][-]", info, sockindex);
  }
}
#endif /* CURLVERBOSE */

শূন্য Curl_conn_cf_discard_chain(কাঠামো Curl_cfilter **pcf,
                                কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cfilter *cfn, *cf = *pcf;

  যদি(cf) {
    *pcf = ফাঁকা;
    যখন(cf) {
      cfn = cf->পরবর্তী;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->পরবর্তী = ফাঁকা;
      cf->cft->destroy(cf, তথ্য);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

শূন্য Curl_conn_cf_discard_all(কাঠামো Curl_easy *তথ্য,
                              কাঠামো connectdata *conn, int8_t sockindex)
{
  conn->shutdown.start_ms[sockindex] = -1;
  conn->connect_started.set[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], তথ্য);
}

CURLcode Curl_cf_recv(কাঠামো Curl_easy *তথ্য, int8_t sockindex, অক্ষর *buf,
                      size_t len, size_t *pnread)
{
  কাঠামো Curl_cfilter *cf;

  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->conn);
  cf = তথ্য->conn->cfilter[sockindex];
  যখন(cf && !cf->connected)
    cf = cf->পরবর্তী;
  যদি(cf)
    ফেরত cf->cft->do_recv(cf, তথ্য, buf, len, pnread);
  failf(তথ্য, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  ফেরত CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                      ধ্রুবক uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  কাঠামো Curl_cfilter *cf;

  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->conn);
  cf = তথ্য->conn->cfilter[sockindex];
  যখন(cf && !cf->connected)
    cf = cf->পরবর্তী;
  যদি(cf) {
    ফেরত cf->cft->do_send(cf, তথ্য, buf, len, eos, pnwritten);
  }
  failf(তথ্য, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  ফেরত CURLE_FAILED_INIT;
}

কাঠামো cf_io_ctx {
  কাঠামো Curl_easy *তথ্য;
  কাঠামো Curl_cfilter *cf;
};

স্থির CURLcode cf_bufq_reader(শূন্য *writer_ctx,
                               unsigned অক্ষর *buf, size_t blen,
                               size_t *pnread)
{
  কাঠামো cf_io_ctx *io = writer_ctx;
  ফেরত Curl_conn_cf_recv(io->cf, io->তথ্য, (অক্ষর *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(কাঠামো Curl_cfilter *cf,
                           কাঠামো Curl_easy *তথ্য,
                           কাঠামো bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  কাঠামো cf_io_ctx io;

  যদি(!cf || !তথ্য) {
    *pnread = 0;
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.তথ্য = তথ্য;
  io.cf = cf;
  ফেরত Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

স্থির CURLcode cf_bufq_writer(শূন্য *writer_ctx,
                               ধ্রুবক uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  কাঠামো cf_io_ctx *io = writer_ctx;
  ফেরত Curl_conn_cf_send(io->cf, io->তথ্য, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(কাঠামো Curl_cfilter *cf,
                           কাঠামো Curl_easy *তথ্য,
                           কাঠামো bufq *bufq,
                           ধ্রুবক unsigned অক্ষর *buf, size_t blen,
                           size_t *pnwritten)
{
  কাঠামো cf_io_ctx io;

  যদি(!cf || !তথ্য) {
    *pnwritten = 0;
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }
  io.তথ্য = তথ্য;
  io.cf = cf;
  যদি(buf && blen)
    ফেরত Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  অন্যথা
    ফেরত Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(কাঠামো Curl_cfilter **pcf,
                        ধ্রুবক কাঠামো Curl_cftype *cft,
                        শূন্য *ctx)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, আকার(*cf));
  যদি(!cf)
    যাও out;

  cf->cft = cft;
  cf->ctx = ctx;
  ফলাফল = CURLE_OK;
out:
  *pcf = cf;
  ফেরত ফলাফল;
}

শূন্য Curl_conn_cf_add(কাঠামো Curl_easy *তথ্য,
                      কাঠামো connectdata *conn,
                      int8_t sockindex,
                      কাঠামো Curl_cfilter *cf)
{
  DEBUGASSERT(conn);
  DEBUGASSERT(!cf->conn);
  DEBUGASSERT(!cf->পরবর্তী);

  cf->পরবর্তী = conn->cfilter[sockindex];
  cf->conn = conn;
  cf->sockindex = sockindex;
  conn->cfilter[sockindex] = cf;
  CURL_TRC_CF(তথ্য, cf, "added");
}

শূন্য Curl_conn_cf_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                               কাঠামো Curl_cfilter *cf_new)
{
  কাঠামো Curl_cfilter *লেজ, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  লেজ = cf_at->পরবর্তী;
  cf_at->পরবর্তী = cf_new;
  করো {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->পরবর্তী;
    cf_new = cf_new->পরবর্তী;
  } যখন(cf_new);
  *pnext = লেজ;
}

bool Curl_conn_cf_discard(কাঠামো Curl_cfilter **pcf,
                          কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cfilter *cf = pcf ? *pcf : ফাঁকা;
  bool found = FALSE;
  যদি(cf) {
    যদি(cf->conn) {
      /* unlink if present in connection filter chain */
      কাঠামো Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      যখন(*pprev) {
        যদি(*pprev == *pcf) {
          *pprev = (*pcf)->পরবর্তী;
          cf->পরবর্তী = ফাঁকা;
          found = TRUE;
          বিরতি;
        }
        pprev = &(*pprev)->পরবর্তী;
      }
    }
    Curl_conn_cf_discard_chain(pcf, তথ্য);
  }
  ফেরত found;
}

CURLcode Curl_conn_cf_connect(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              bool *done)
{
  যদি(cf)
    ফেরত cf->cft->do_connect(cf, তথ্য, done);
  ফেরত CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                           ধ্রুবক uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  যদি(cf)
    ফেরত cf->cft->do_send(cf, তথ্য, buf, len, eos, pnwritten);
  *pnwritten = 0;
  ফেরত CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                           অক্ষর *buf, size_t len, size_t *pnread)
{
  যদি(cf)
    ফেরত cf->cft->do_recv(cf, তথ্য, buf, len, pnread);
  *pnread = 0;
  ফেরত CURLE_RECV_ERROR;
}

স্থির CURLcode cf_cntrl_all(কাঠামো connectdata *conn,
                             কাঠামো Curl_easy *তথ্য,
                             bool ignore_ফলাফল,
                             পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা i;

  জন্য(i = 0; i < (পূর্ণসংখ্যা)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    ফলাফল = Curl_conn_cf_cntrl(conn->cfilter[i], তথ্য, ignore_ফলাফল,
                                event, arg1, arg2);
    যদি(!ignore_ফলাফল && ফলাফল)
      বিরতি;
  }
  ফেরত ফলাফল;
}

bool Curl_conn_cf_is_ip_connected(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য)
{
  যদি(cf) {
    পূর্ণসংখ্যা মান = 0;
    যদি(!cf->cft->query(cf, তথ্য, CF_QUERY_REALLY_CONNECTED, &মান, ফাঁকা))
      ফেরত !!মান;
  }
  ফেরত FALSE;
}

শূন্য Curl_conn_cntrl_update_info(কাঠামো Curl_easy *তথ্য,
                                কাঠামো connectdata *conn)
{
  cf_cntrl_all(conn, তথ্য, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, ফাঁকা);
}

শূন্য Curl_conn_cntrl_report_stats(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata *conn,
                                  পূর্ণসংখ্যা sockindex)
{
  যদি((unsigned)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (শূন্য)Curl_conn_cf_cntrl(conn->cfilter[sockindex], তথ্য, TRUE,
                             CF_CTRL_REPORT_STATS, 0, ফাঁকা);
}

শূন্য Curl_conn_remove_setup_filters(কাঠামো Curl_easy *তথ্য,
                                    int8_t sockindex)
{
  কাঠামো Curl_cfilter **anchor = &তথ্য->conn->cfilter[sockindex];
  যখন(*anchor) {
    কাঠামো Curl_cfilter *cf = *anchor;
    যদি(cf->connected && (cf->cft->flags & CF_TYPE_SETUP)) {
      *anchor = cf->পরবর্তী;
      cf->পরবর্তী = ফাঁকা;
      CURL_TRC_CF(তথ্য, cf, "removing connected setup filter");
      cf->cft->destroy(cf, তথ্য);
      curlx_free(cf);
    }
    অন্যথা
      anchor = &cf->পরবর্তী;
  }
}

bool Curl_conn_is_setup(কাঠামো connectdata *conn, int8_t sockindex)
{
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত FALSE;
  ফেরত !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(কাঠামো connectdata *conn, int8_t sockindex)
{
  কাঠামো Curl_cfilter *cf;

  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত FALSE;
  cf = conn->cfilter[sockindex];
  যদি(cf)
    ফেরত (bool)cf->connected;
  অন্যথা যদি(conn->scheme->flags & PROTOPT_NONETWORK)
    ফেরত TRUE;
  ফেরত FALSE;
}

bool Curl_conn_is_ip_connected(কাঠামো Curl_easy *তথ্য, int8_t sockindex)
{
  কাঠামো Curl_cfilter *cf;

  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত FALSE;
  cf = তথ্য->conn->cfilter[sockindex];
  যখন(cf) {
    যদি(cf->connected)
      ফেরত TRUE;
    যদি(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ফেরত FALSE;
    cf = cf->পরবর্তী;
  }
  ফেরত FALSE;
}

#ifndef CURL_DISABLE_PROXY
স্থির bool cf_is_tunneling(কাঠামো Curl_cfilter *cf)
{
  জন্য(; cf; cf = cf->পরবর্তী) {
    যদি((cf->cft->flags & CF_TYPE_PROXY))
      ফেরত TRUE;
  }
  ফেরত FALSE;
}

bool Curl_conn_is_tunneling(কাঠামো connectdata *conn, int8_t sockindex)
{
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত FALSE;
  ফেরত conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#অন্যথা
bool Curl_conn_is_tunneling(কাঠামো connectdata *conn, int8_t sockindex)
{
  (শূন্য)conn;
  (শূন্য)sockindex;
  ফেরত FALSE;
}
#endif /* CURL_DISABLE_PROXY */

স্থির bool cf_is_ssl(কাঠামো Curl_cfilter *cf)
{
  জন্য(; cf; cf = cf->পরবর্তী) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    যদি((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      ফেরত TRUE;
    যদি(cf->cft->flags & CF_TYPE_IP_CONNECT)
      ফেরত FALSE;
  }
  ফেরত FALSE;
}

bool Curl_conn_is_ssl(কাঠামো connectdata *conn, int8_t sockindex)
{
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত FALSE;
  ফেরত conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(কাঠামো Curl_easy *তথ্য,
                            কাঠামো connectdata *conn, int8_t sockindex,
                            পূর্ণসংখ্যা query,
                            কাঠামো curl_tlssessioninfo *info)
{
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত FALSE;
  যদি(Curl_conn_is_ssl(conn, sockindex)) {
    কাঠামো Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode ফলাফল = cf ?
      cf->cft->query(cf, তথ্য, query, ফাঁকা, (শূন্য *)info) :
      CURLE_UNKNOWN_OPTION;
    ফেরত !ফলাফল;
  }
  ফেরত FALSE;
}

CURLcode Curl_conn_get_ip_info(কাঠামো Curl_easy *তথ্য,
                               কাঠামো connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, কাঠামো ip_quadruple *ipquad)
{
  কাঠামো Curl_cfilter *cf;
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  cf = conn ? conn->cfilter[sockindex] : ফাঁকা;
  ফেরত Curl_conn_cf_get_ip_info(cf, তথ্য, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(কাঠামো connectdata *conn, int8_t sockindex)
{
  কাঠামো Curl_cfilter *cf;

  যদি(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    ফেরত FALSE;
  জন্য(cf = conn->cfilter[sockindex]; cf; cf = cf->পরবর্তী) {
    যদি(cf->cft->flags & CF_TYPE_MULTIPLEX)
      ফেরত TRUE;
    যদি(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      ফেরত FALSE;
  }
  ফেরত FALSE;
}

unsigned অক্ষর Curl_conn_get_transport(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো connectdata *conn)
{
  কাঠামো Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ফেরত Curl_conn_cf_get_transport(cf, তথ্য);
}

পূর্ণসংখ্যা Curl_socktype_for_transport(uint8_t transport)
{
  নির্বাচন(transport) {
  ক্ষেত্র TRNSPRT_TCP:
    ফেরত SOCK_STREAM;
  ক্ষেত্র TRNSPRT_UNIX:
    ফেরত SOCK_STREAM;
  পূর্বনির্ধারিত: /* UDP and QUIC */
    ফেরত SOCK_DGRAM;
  }
}

পূর্ণসংখ্যা Curl_protocol_for_transport(uint8_t transport)
{
  নির্বাচন(transport) {
  ক্ষেত্র TRNSPRT_TCP:
    ফেরত IPPROTO_TCP;
  ক্ষেত্র TRNSPRT_UNIX:
    ফেরত IPPROTO_IP;
  পূর্বনির্ধারিত: /* UDP and QUIC */
    ফেরত IPPROTO_UDP;
  }
}

ধ্রুবক অক্ষর *Curl_conn_get_alpn_negotiated(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো connectdata *conn)
{
  কাঠামো Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ফেরত Curl_conn_cf_get_alpn_negotiated(cf, তথ্য);
}

unsigned অক্ষর Curl_conn_http_version(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো connectdata *conn)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল = CURLE_UNKNOWN_OPTION;
  unsigned অক্ষর v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  জন্য(; cf; cf = cf->পরবর্তী) {
    যদি(cf->cft->flags & CF_TYPE_HTTP) {
      পূর্ণসংখ্যা মান = 0;
      ফলাফল = cf->cft->query(cf, তথ্য, CF_QUERY_HTTP_VERSION, &মান, ফাঁকা);
      যদি(!ফলাফল && ((মান < 0) || (মান > 255)))
        ফলাফল = CURLE_FAILED_INIT;
      অন্যথা
        v = (unsigned অক্ষর)মান;
      বিরতি;
    }
    যদি(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      বিরতি;
  }
  ফেরত (unsigned অক্ষর)(ফলাফল ? 0 : v);
}

bool Curl_conn_তথ্য_pending(কাঠামো Curl_easy *তথ্য, int8_t sockindex)
{
  কাঠামো Curl_cfilter *cf;

  (শূন্য)তথ্য;
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->conn);
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত FALSE;

  cf = তথ্য->conn->cfilter[sockindex];
  যখন(cf && !cf->connected) {
    cf = cf->পরবর্তী;
  }
  যদি(cf) {
    ফেরত cf->cft->has_তথ্য_pending(cf, তথ্য);
  }
  ফেরত FALSE;
}

bool Curl_conn_cf_needs_flush(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল;
  পূর্ণসংখ্যা pending = 0;
  ফলাফল = cf ? cf->cft->query(cf, তথ্য, CF_QUERY_NEED_FLUSH,
                               &pending, ফাঁকা) : CURLE_UNKNOWN_OPTION;
  ফেরত (ফলাফল || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(কাঠামো Curl_easy *তথ্য, int8_t sockindex)
{
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত FALSE;
  ফেরত Curl_conn_cf_needs_flush(তথ্য->conn->cfilter[sockindex], তথ্য);
}

CURLcode Curl_conn_cf_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                     কাঠামো Curl_easy *তথ্য,
                                     কাঠামো easy_pollset *ps)
{
  CURLcode ফলাফল = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  জন্য(; cf && !ফলাফল; cf = cf->পরবর্তী) {
    যদি(cf->shutdown)
      চালু;
    যদি(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->পরবর্তী || cf->পরবর্তী->connected)
      ফলাফল = cf->cft->adjust_pollset(cf, তথ্য, ps);
  }
  ফেরত ফলাফল;
}

CURLcode Curl_conn_adjust_pollset(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata *conn,
                                  কাঠামো easy_pollset *ps)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক bool want_io = !!ps->n;
  int8_t i;

  DEBUGASSERT(তথ্য);
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
  জন্য(i = 0; (i < (পূর্ণসংখ্যা)CURL_ARRAYSIZE(conn->cfilter)) && !ফলাফল; ++i) {
    যদি(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_IN_SHUTDOWN(conn, i)))
      ফলাফল = Curl_conn_cf_adjust_pollset(conn->cfilter[i], তথ্য, ps);
  }
  ফেরত ফলাফল;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
পূর্ণসংখ্যা Curl_conn_cf_poll(কাঠামো Curl_cfilter *cf,
                      কাঠামো Curl_easy *তথ্য,
                      timediff_t timeout_ms)
{
  কাঠামো easy_pollset ps;
  পূর্ণসংখ্যা rc;
  CURLcode ফলাফল;

  DEBUGASSERT(cf);
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->conn);
  Curl_pollset_init(&ps);

  ফলাফল = Curl_conn_cf_adjust_pollset(cf, তথ্য, &ps);
  যদি(!ফলাফল)
    rc = Curl_pollset_poll(তথ্য, &ps, timeout_ms);
  অন্যথা
    rc = -1;
  Curl_pollset_cleanup(&ps);
  ফেরত rc;
}

শূন্য Curl_conn_get_current_host(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                                ধ্রুবক অক্ষর **phost, পূর্ণসংখ্যা *pport)
{
  কাঠামো Curl_cfilter *cf, *cf_proxy = ফাঁকা;
  পূর্ণসংখ্যা portarg = -1;

  যদি(!তথ্য->conn) {
    DEBUGASSERT(0);
    *phost = "";
    যদি(pport)
      *pport = -1;
    ফেরত;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? তথ্য->conn->cfilter[sockindex] : ফাঁকা;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  যখন(cf && !cf->connected) {
    যদি((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->পরবর্তী;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  যদি(!cf_proxy || cf_proxy->cft->query(cf_proxy, তথ্য, CF_QUERY_HOST_PORT,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = তথ্য->conn->origin->hostname;
    portarg = তথ্য->conn->origin->port;
  }
  যদি(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(কাঠামো Curl_cfilter *cf,
                           কাঠামো Curl_easy *তথ্য,
                           পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  (শূন্য)cf;
  (শূন্য)তথ্য;
  (শূন্য)event;
  (শূন্য)arg1;
  (শূন্য)arg2;
  ফেরত CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য,
                            bool ignore_ফলাফল,
                            পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  CURLcode ফলাফল = CURLE_OK;

  জন্য(; cf; cf = cf->পরবর্তী) {
    যদি(cf->cft->cntrl == Curl_cf_def_cntrl)
      চালু;
    ফলাফল = cf->cft->cntrl(cf, তথ্য, event, arg1, arg2);
    যদি(!ignore_ফলাফল && ফলাফল)
      বিরতি;
  }
  ফেরত ফলাফল;
}

curl_socket_t Curl_conn_cf_get_socket(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য)
{
  curl_socket_t sock;
  যদি(cf && !cf->cft->query(cf, তথ্য, CF_QUERY_SOCKET, ফাঁকা, &sock))
    ফেরত sock;
  ফেরত CURL_SOCKET_BAD;
}

unsigned অক্ষর Curl_conn_cf_get_transport(কাঠামো Curl_cfilter *cf,
                                         কাঠামো Curl_easy *তথ্য)
{
  পূর্ণসংখ্যা transport = 0;
  যদি(cf && !cf->cft->query(cf, তথ্য, CF_QUERY_TRANSPORT, &transport, ফাঁকা))
    ফেরত (unsigned অক্ষর)transport;
  ফেরত (unsigned অক্ষর)(তথ্য->conn ? তথ্য->conn->transport_wanted : 0);
}

ধ্রুবক অক্ষর *Curl_conn_cf_get_alpn_negotiated(কাঠামো Curl_cfilter *cf,
                                             কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক অক্ষর *alpn = ফাঁকা;
  CURL_TRC_CF(তথ্য, cf, "query ALPN");
  যদি(cf && !cf->cft->query(cf, তথ্য, CF_QUERY_ALPN_NEGOTIATED, ফাঁকা,
                           CURL_UNCONST(&alpn)))
    ফেরত alpn;
  ফেরত ফাঁকা;
}

স্থির ধ্রুবক কাঠামো Curl_sockaddr_ex *cf_get_remote_addr(
  কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক কাঠামো Curl_sockaddr_ex *remote_addr = ফাঁকা;
  যদি(cf &&
     !cf->cft->query(cf, তথ্য, CF_QUERY_REMOTE_ADDR, ফাঁকা,
                     CURL_UNCONST(&remote_addr)))
    ফেরত remote_addr;
  ফেরত ফাঁকা;
}

CURLcode Curl_conn_cf_get_ip_info(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  bool *is_ipv6, কাঠামো ip_quadruple *ipquad)
{
  CURLcode ফলাফল = CURLE_UNKNOWN_OPTION;
  যদি(cf) {
    পূর্ণসংখ্যা ipv6 = 0;
    ফলাফল = cf->cft->query(cf, তথ্য, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  ফেরত ফলাফল;
}

curl_socket_t Curl_conn_get_first_socket(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cfilter *cf;

  যদি(!তথ্য->conn)
    ফেরত CURL_SOCKET_BAD;

  cf = তথ্য->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  যদি(cf && !cf->connected)
    ফেরত Curl_conn_cf_get_socket(cf, তথ্য);
  ফেরত তথ্য->conn->sock[FIRSTSOCKET];
}

ধ্রুবক কাঠামো Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  কাঠামো Curl_easy *তথ্য, int8_t sockindex)
{
  কাঠামো Curl_cfilter *cf =
    (তথ্য->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    তথ্য->conn->cfilter[sockindex] : ফাঁকা;
  ফেরত cf ? cf_get_remote_addr(cf, তথ্য) : ফাঁকা;
}

CURLcode Curl_conn_ev_তথ্য_setup(কাঠামো Curl_easy *তথ্য)
{
  ফেরত cf_cntrl_all(তথ্য->conn, তথ্য, FALSE, CF_CTRL_তথ্য_SETUP, 0, ফাঁকা);
}

CURLcode Curl_conn_flush(কাঠামো Curl_easy *তথ্য, int8_t sockindex)
{
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  ফেরত Curl_conn_cf_cntrl(তথ্য->conn->cfilter[sockindex], তথ্য, FALSE,
                            CF_CTRL_FLUSH, 0, ফাঁকা);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
শূন্য Curl_conn_ev_তথ্য_done_send(কাঠামো Curl_easy *তথ্য)
{
  cf_cntrl_all(তথ্য->conn, তথ্য, TRUE, CF_CTRL_তথ্য_DONE_SEND, 0, ফাঁকা);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
শূন্য Curl_conn_ev_তথ্য_done(কাঠামো Curl_easy *তথ্য, bool premature)
{
  cf_cntrl_all(তথ্য->conn, তথ্য, TRUE, CF_CTRL_তথ্য_DONE, premature, ফাঁকা);
}

CURLcode Curl_conn_ev_তথ্য_pause(কাঠামো Curl_easy *তথ্য, bool do_pause)
{
  ফেরত cf_cntrl_all(তথ্য->conn, তথ্য, FALSE,
                      CF_CTRL_তথ্য_PAUSE, do_pause, ফাঁকা);
}

bool Curl_conn_is_alive(কাঠামো Curl_easy *তথ্য, কাঠামো connectdata *conn,
                        bool *প্রবেশ_pending)
{
  কাঠামো Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  ফেরত cf && !cf->conn->bits.close &&
         cf->cft->is_alive(cf, তথ্য, প্রবেশ_pending);
}

CURLcode Curl_conn_keep_alive(কাঠামো Curl_easy *তথ্য,
                              কাঠামো connectdata *conn)
{
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা i;

  জন্য(i = 0; (i < (পূর্ণসংখ্যা)CURL_ARRAYSIZE(conn->cfilter)) && !ফলাফল; ++i) {
    কাঠামো Curl_cfilter *cf = conn->cfilter[i];
    যদি(cf)
      ফলাফল = cf->cft->keep_alive(cf, তথ্য);
  }
  ফেরত ফলাফল;
}

size_t Curl_conn_get_max_concurrent(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো connectdata *conn,
                                    int8_t sockindex)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা n = -1;

  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত 0;

  cf = conn->cfilter[sockindex];
  ফলাফল = cf ? cf->cft->query(cf, তথ্য, CF_QUERY_MAX_CONCURRENT,
                               &n, ফাঁকা) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  ফেরত (ফলাফল || n < 0) ? 1 : (size_t)n;
}

পূর্ণসংখ্যা Curl_conn_get_stream_error(কাঠামো Curl_easy *তথ্য,
                               কাঠামো connectdata *conn,
                               int8_t sockindex)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা n = 0;

  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত 0;

  cf = conn->cfilter[sockindex];
  ফলাফল = cf ? cf->cft->query(cf, তথ্য, CF_QUERY_STREAM_ERROR,
                               &n, ফাঁকা) : CURLE_UNKNOWN_OPTION;
  ফেরত (ফলাফল || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(কাঠামো Curl_easy *তথ্য, curl_socket_t sockfd)
{
  যদি(তথ্য && তথ্য->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == তথ্য->conn->sock[SECONDARYSOCKET])
    ফেরত SECONDARYSOCKET;
  ফেরত FIRSTSOCKET;
}

CURLcode Curl_conn_recv(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                        অক্ষর *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->conn);
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  যদি(তথ্য && তথ্য->conn && তথ্য->conn->recv[sockindex])
    ফেরত তথ্য->conn->recv[sockindex](তথ্য, sockindex, buf, len, pnread);
  *pnread = 0;
  ফেরত CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                        ধ্রুবক শূন্য *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(তথ্য);
  DEBUGASSERT(তথ্য->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  যদি(!CONN_SOCK_IDX_VALID(sockindex))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef DEBUGBUILD
  যদি(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    ধ্রুবক অক্ষর *p = getenv("CURL_SMALLSENDS");
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
  যদি(তথ্য && তথ্য->conn && তথ্য->conn->send[sockindex]) {
    CURLcode ফলাফল = তথ্য->conn->send[sockindex](তথ্য, sockindex, buf,
                                                  write_len, eos, pnwritten);
    যদি(!ফলাফল && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      ফেরত CURLE_SEND_ERROR;
    }
    ফেরত ফলাফল;
  }
  *pnwritten = 0;
  ফেরত CURLE_FAILED_INIT;
}
