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
                              بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه;

  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  إذا(cf->بعدی) {
    نتیجه = cf->بعدی->cft->do_connect(cf->بعدی, داده, انجامشده);
    إذا(نتیجه || !*انجامشده)
      أرجع نتیجه;
  }

  cf->connected = TRUE;
  *انجامشده = TRUE;
  أرجع CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده, bool *انجامشده)
{
  (فراغ)cf;
  (فراغ)داده;
  *انجامشده = TRUE;
  أرجع CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    بنية easy_pollset *ps)
{
  /* NOP */
  (فراغ)cf;
  (فراغ)داده;
  (فراغ)ps;
  أرجع CURLE_OK;
}

bool Curl_cf_def_داده_pending(بنية Curl_cfilter *cf,
                              ثابت بنية Curl_easy *داده)
{
  أرجع cf->بعدی ?
    cf->بعدی->cft->has_داده_pending(cf->بعدی, داده) : FALSE;
}

CURLcode Curl_cf_def_send(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                          ثابت uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  إذا(cf->بعدی)
    أرجع cf->بعدی->cft->do_send(cf->بعدی, داده, buf, len, eos, pnwritten);
  *pnwritten = 0;
  أرجع CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                          حرف *buf, size_t len, size_t *pnread)
{
  إذا(cf->بعدی)
    أرجع cf->بعدی->cft->do_recv(cf->بعدی, داده, buf, len, pnread);
  *pnread = 0;
  أرجع CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               bool *درون_داد_pending)
{
  أرجع cf->بعدی ?
    cf->بعدی->cft->is_alive(cf->بعدی, داده, درون_داد_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *داده)
{
  أرجع cf->بعدی ?
    cf->بعدی->cft->keep_alive(cf->بعدی, داده) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(بنية Curl_cfilter *cf,
                           بنية Curl_easy *داده,
                           صحيح query, صحيح *pres1, فراغ *pres2)
{
  أرجع cf->بعدی ?
    cf->بعدی->cft->query(cf->بعدی, داده, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#اگر_تعریف_شده CURLVERBOSE
فراغ Curl_conn_trc_filters(بنية Curl_easy *داده,
                           int8_t sockindex, ثابت حرف *info)
{
  إذا(CURL_TRC_M_is_verbose(داده) && داده->conn) {
    بنية Curl_cfilter *cf = داده->conn->cfilter[sockindex];

    إذا(cf) {
      حرف پیام[256], *buf;
      صحيح blen, n;

      buf = پیام;
      blen = حجم(پیام) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      لكل(; cf && blen; cf = cf->بعدی) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(داده, "%s%s", پیام, blen ? "" : "...");
    }
    وإلا
      CURL_TRC_M(داده, "%s [%d][-]", info, sockindex);
  }
}
#پایان_اگر /* CURLVERBOSE */

فراغ Curl_conn_cf_discard_chain(بنية Curl_cfilter **pcf,
                                بنية Curl_easy *داده)
{
  بنية Curl_cfilter *cfn, *cf = *pcf;

  إذا(cf) {
    *pcf = عدم;
    طالما(cf) {
      cfn = cf->بعدی;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->بعدی = عدم;
      cf->cft->destroy(cf, داده);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

فراغ Curl_conn_cf_discard_all(بنية Curl_easy *داده,
                              بنية connectdata *conn, int8_t sockindex)
{
  conn->shutdown.شروع_ms[sockindex] = -1;
  conn->connect_started.مجموعه[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], داده);
}

CURLcode Curl_cf_recv(بنية Curl_easy *داده, int8_t sockindex, حرف *buf,
                      size_t len, size_t *pnread)
{
  بنية Curl_cfilter *cf;

  DEBUGASSERT(داده);
  DEBUGASSERT(داده->conn);
  cf = داده->conn->cfilter[sockindex];
  طالما(cf && !cf->connected)
    cf = cf->بعدی;
  إذا(cf)
    أرجع cf->cft->do_recv(cf, داده, buf, len, pnread);
  failf(داده, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  أرجع CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(بنية Curl_easy *داده, int8_t sockindex,
                      ثابت uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  بنية Curl_cfilter *cf;

  DEBUGASSERT(داده);
  DEBUGASSERT(داده->conn);
  cf = داده->conn->cfilter[sockindex];
  طالما(cf && !cf->connected)
    cf = cf->بعدی;
  إذا(cf) {
    أرجع cf->cft->do_send(cf, داده, buf, len, eos, pnwritten);
  }
  failf(داده, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  أرجع CURLE_FAILED_INIT;
}

بنية cf_io_ctx {
  بنية Curl_easy *داده;
  بنية Curl_cfilter *cf;
};

ساكن CURLcode cf_bufq_reader(فراغ *writer_ctx,
                               بیعلامت حرف *buf, size_t blen,
                               size_t *pnread)
{
  بنية cf_io_ctx *io = writer_ctx;
  أرجع Curl_conn_cf_recv(io->cf, io->داده, (حرف *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(بنية Curl_cfilter *cf,
                           بنية Curl_easy *داده,
                           بنية bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  بنية cf_io_ctx io;

  إذا(!cf || !داده) {
    *pnread = 0;
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }
  io.داده = داده;
  io.cf = cf;
  أرجع Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

ساكن CURLcode cf_bufq_writer(فراغ *writer_ctx,
                               ثابت uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  بنية cf_io_ctx *io = writer_ctx;
  أرجع Curl_conn_cf_send(io->cf, io->داده, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(بنية Curl_cfilter *cf,
                           بنية Curl_easy *داده,
                           بنية bufq *bufq,
                           ثابت بیعلامت حرف *buf, size_t blen,
                           size_t *pnwritten)
{
  بنية cf_io_ctx io;

  إذا(!cf || !داده) {
    *pnwritten = 0;
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }
  io.داده = داده;
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
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, حجم(*cf));
  إذا(!cf)
    اذهب بیرون;

  cf->cft = cft;
  cf->ctx = ctx;
  نتیجه = CURLE_OK;
بیرون:
  *pcf = cf;
  أرجع نتیجه;
}

فراغ Curl_conn_cf_جمع_کن(بنية Curl_easy *داده,
                      بنية connectdata *conn,
                      int8_t sockindex,
                      بنية Curl_cfilter *cf)
{
  DEBUGASSERT(conn);
  DEBUGASSERT(!cf->conn);
  DEBUGASSERT(!cf->بعدی);

  cf->بعدی = conn->cfilter[sockindex];
  cf->conn = conn;
  cf->sockindex = sockindex;
  conn->cfilter[sockindex] = cf;
  CURL_TRC_CF(داده, cf, "added");
}

فراغ Curl_conn_cf_درج_after(بنية Curl_cfilter *cf_at,
                               بنية Curl_cfilter *cf_new)
{
  بنية Curl_cfilter *دم, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  دم = cf_at->بعدی;
  cf_at->بعدی = cf_new;
  افعل {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->بعدی;
    cf_new = cf_new->بعدی;
  } طالما(cf_new);
  *pnext = دم;
}

bool Curl_conn_cf_discard(بنية Curl_cfilter **pcf,
                          بنية Curl_easy *داده)
{
  بنية Curl_cfilter *cf = pcf ? *pcf : عدم;
  bool یافت = FALSE;
  إذا(cf) {
    إذا(cf->conn) {
      /* unlink if present in connection filter chain */
      بنية Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      طالما(*pprev) {
        إذا(*pprev == *pcf) {
          *pprev = (*pcf)->بعدی;
          cf->بعدی = عدم;
          یافت = TRUE;
          اكسر;
        }
        pprev = &(*pprev)->بعدی;
      }
    }
    Curl_conn_cf_discard_chain(pcf, داده);
  }
  أرجع یافت;
}

CURLcode Curl_conn_cf_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده,
                              bool *انجامشده)
{
  إذا(cf)
    أرجع cf->cft->do_connect(cf, داده, انجامشده);
  أرجع CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                           ثابت uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  إذا(cf)
    أرجع cf->cft->do_send(cf, داده, buf, len, eos, pnwritten);
  *pnwritten = 0;
  أرجع CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                           حرف *buf, size_t len, size_t *pnread)
{
  إذا(cf)
    أرجع cf->cft->do_recv(cf, داده, buf, len, pnread);
  *pnread = 0;
  أرجع CURLE_RECV_ERROR;
}

ساكن CURLcode cf_cntrl_all(بنية connectdata *conn,
                             بنية Curl_easy *داده,
                             bool ignore_نتیجه,
                             صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode نتیجه = CURLE_OK;
  صحيح i;

  لكل(i = 0; i < (صحيح)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    نتیجه = Curl_conn_cf_cntrl(conn->cfilter[i], داده, ignore_نتیجه,
                                event, arg1, arg2);
    إذا(!ignore_نتیجه && نتیجه)
      اكسر;
  }
  أرجع نتیجه;
}

bool Curl_conn_cf_is_ip_connected(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده)
{
  إذا(cf) {
    صحيح مقدار = 0;
    إذا(!cf->cft->query(cf, داده, CF_QUERY_REALLY_CONNECTED, &مقدار, عدم))
      أرجع !!مقدار;
  }
  أرجع FALSE;
}

فراغ Curl_conn_cntrl_update_info(بنية Curl_easy *داده,
                                بنية connectdata *conn)
{
  cf_cntrl_all(conn, داده, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, عدم);
}

فراغ Curl_conn_cntrl_report_stats(بنية Curl_easy *داده,
                                  بنية connectdata *conn,
                                  صحيح sockindex)
{
  إذا((بیعلامت)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (فراغ)Curl_conn_cf_cntrl(conn->cfilter[sockindex], داده, TRUE,
                             CF_CTRL_REPORT_STATS, 0, عدم);
}

فراغ Curl_conn_remove_راهاندازی_filters(بنية Curl_easy *داده,
                                    int8_t sockindex)
{
  بنية Curl_cfilter **anchor = &داده->conn->cfilter[sockindex];
  طالما(*anchor) {
    بنية Curl_cfilter *cf = *anchor;
    إذا(cf->connected && (cf->cft->flags & CF_TYPE_راهاندازی)) {
      *anchor = cf->بعدی;
      cf->بعدی = عدم;
      CURL_TRC_CF(داده, cf, "removing connected setup filter");
      cf->cft->destroy(cf, داده);
      curlx_free(cf);
    }
    وإلا
      anchor = &cf->بعدی;
  }
}

bool Curl_conn_is_راهاندازی(بنية connectdata *conn, int8_t sockindex)
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

bool Curl_conn_is_ip_connected(بنية Curl_easy *داده, int8_t sockindex)
{
  بنية Curl_cfilter *cf;

  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  cf = داده->conn->cfilter[sockindex];
  طالما(cf) {
    إذا(cf->connected)
      أرجع TRUE;
    إذا(cf->cft->flags & CF_TYPE_IP_CONNECT)
      أرجع FALSE;
    cf = cf->بعدی;
  }
  أرجع FALSE;
}

#اگر_تعریف_نشده CURL_DISABLE_PROXY
ساكن bool cf_is_tunneling(بنية Curl_cfilter *cf)
{
  لكل(; cf; cf = cf->بعدی) {
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
#پایان_اگر /* CURL_DISABLE_PROXY */

ساكن bool cf_is_ssl(بنية Curl_cfilter *cf)
{
  لكل(; cf; cf = cf->بعدی) {
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

bool Curl_conn_get_ssl_info(بنية Curl_easy *داده,
                            بنية connectdata *conn, int8_t sockindex,
                            صحيح query,
                            بنية curl_tlssessioninfo *info)
{
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  إذا(Curl_conn_is_ssl(conn, sockindex)) {
    بنية Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode نتیجه = cf ?
      cf->cft->query(cf, داده, query, عدم, (فراغ *)info) :
      CURLE_UNKNOWN_OPTION;
    أرجع !نتیجه;
  }
  أرجع FALSE;
}

CURLcode Curl_conn_get_ip_info(بنية Curl_easy *داده,
                               بنية connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, بنية ip_quadruple *ipquad)
{
  بنية Curl_cfilter *cf;
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  cf = conn ? conn->cfilter[sockindex] : عدم;
  أرجع Curl_conn_cf_get_ip_info(cf, داده, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(بنية connectdata *conn, int8_t sockindex)
{
  بنية Curl_cfilter *cf;

  إذا(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  لكل(cf = conn->cfilter[sockindex]; cf; cf = cf->بعدی) {
    إذا(cf->cft->flags & CF_TYPE_MULTIPLEX)
      أرجع TRUE;
    إذا(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      أرجع FALSE;
  }
  أرجع FALSE;
}

بیعلامت حرف Curl_conn_get_transport(بنية Curl_easy *داده,
                                      بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  أرجع Curl_conn_cf_get_transport(cf, داده);
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

ثابت حرف *Curl_conn_get_alpn_negotiated(بنية Curl_easy *داده,
                                          بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  أرجع Curl_conn_cf_get_alpn_negotiated(cf, داده);
}

بیعلامت حرف Curl_conn_http_نسخه(بنية Curl_easy *داده,
                                     بنية connectdata *conn)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجه = CURLE_UNKNOWN_OPTION;
  بیعلامت حرف v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  لكل(; cf; cf = cf->بعدی) {
    إذا(cf->cft->flags & CF_TYPE_HTTP) {
      صحيح مقدار = 0;
      نتیجه = cf->cft->query(cf, داده, CF_QUERY_HTTP_نسخه, &مقدار, عدم);
      إذا(!نتیجه && ((مقدار < 0) || (مقدار > 255)))
        نتیجه = CURLE_FAILED_INIT;
      وإلا
        v = (بیعلامت حرف)مقدار;
      اكسر;
    }
    إذا(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      اكسر;
  }
  أرجع (بیعلامت حرف)(نتیجه ? 0 : v);
}

bool Curl_conn_داده_pending(بنية Curl_easy *داده, int8_t sockindex)
{
  بنية Curl_cfilter *cf;

  (فراغ)داده;
  DEBUGASSERT(داده);
  DEBUGASSERT(داده->conn);
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;

  cf = داده->conn->cfilter[sockindex];
  طالما(cf && !cf->connected) {
    cf = cf->بعدی;
  }
  إذا(cf) {
    أرجع cf->cft->has_داده_pending(cf, داده);
  }
  أرجع FALSE;
}

bool Curl_conn_cf_needs_flush(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده)
{
  CURLcode نتیجه;
  صحيح pending = 0;
  نتیجه = cf ? cf->cft->query(cf, داده, CF_QUERY_NEED_FLUSH,
                               &pending, عدم) : CURLE_UNKNOWN_OPTION;
  أرجع (نتیجه || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(بنية Curl_easy *داده, int8_t sockindex)
{
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  أرجع Curl_conn_cf_needs_flush(داده->conn->cfilter[sockindex], داده);
}

CURLcode Curl_conn_cf_adjust_pollset(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *داده,
                                     بنية easy_pollset *ps)
{
  CURLcode نتیجه = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  لكل(; cf && !نتیجه; cf = cf->بعدی) {
    إذا(cf->shutdown)
      استمر;
    إذا(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->بعدی || cf->بعدی->connected)
      نتیجه = cf->cft->adjust_pollset(cf, داده, ps);
  }
  أرجع نتیجه;
}

CURLcode Curl_conn_adjust_pollset(بنية Curl_easy *داده,
                                  بنية connectdata *conn,
                                  بنية easy_pollset *ps)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت bool want_io = !!ps->n;
  int8_t i;

  DEBUGASSERT(داده);
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
  لكل(i = 0; (i < (صحيح)CURL_ARRAYSIZE(conn->cfilter)) && !نتیجه; ++i) {
    إذا(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_درون_SHUTDOWN(conn, i)))
      نتیجه = Curl_conn_cf_adjust_pollset(conn->cfilter[i], داده, ps);
  }
  أرجع نتیجه;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
صحيح Curl_conn_cf_poll(بنية Curl_cfilter *cf,
                      بنية Curl_easy *داده,
                      timediff_t timeout_ms)
{
  بنية easy_pollset ps;
  صحيح rc;
  CURLcode نتیجه;

  DEBUGASSERT(cf);
  DEBUGASSERT(داده);
  DEBUGASSERT(داده->conn);
  Curl_pollset_init(&ps);

  نتیجه = Curl_conn_cf_adjust_pollset(cf, داده, &ps);
  إذا(!نتیجه)
    rc = Curl_pollset_poll(داده, &ps, timeout_ms);
  وإلا
    rc = -1;
  Curl_pollset_cleanup(&ps);
  أرجع rc;
}

فراغ Curl_conn_get_current_host(بنية Curl_easy *داده, int8_t sockindex,
                                ثابت حرف **phost, صحيح *pport)
{
  بنية Curl_cfilter *cf, *cf_proxy = عدم;
  صحيح portarg = -1;

  إذا(!داده->conn) {
    DEBUGASSERT(0);
    *phost = "";
    إذا(pport)
      *pport = -1;
    أرجع;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? داده->conn->cfilter[sockindex] : عدم;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  طالما(cf && !cf->connected) {
    إذا((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->بعدی;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  إذا(!cf_proxy || cf_proxy->cft->query(cf_proxy, داده, CF_QUERY_HOST_پورت,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = داده->conn->origin->hostname;
    portarg = داده->conn->origin->پورت;
  }
  إذا(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(بنية Curl_cfilter *cf,
                           بنية Curl_easy *داده,
                           صحيح event, صحيح arg1, فراغ *arg2)
{
  (فراغ)cf;
  (فراغ)داده;
  (فراغ)event;
  (فراغ)arg1;
  (فراغ)arg2;
  أرجع CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(بنية Curl_cfilter *cf,
                            بنية Curl_easy *داده,
                            bool ignore_نتیجه,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode نتیجه = CURLE_OK;

  لكل(; cf; cf = cf->بعدی) {
    إذا(cf->cft->cntrl == Curl_cf_def_cntrl)
      استمر;
    نتیجه = cf->cft->cntrl(cf, داده, event, arg1, arg2);
    إذا(!ignore_نتیجه && نتیجه)
      اكسر;
  }
  أرجع نتیجه;
}

curl_socket_t Curl_conn_cf_get_socket(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده)
{
  curl_socket_t sock;
  إذا(cf && !cf->cft->query(cf, داده, CF_QUERY_SOCKET, عدم, &sock))
    أرجع sock;
  أرجع CURL_SOCKET_BAD;
}

بیعلامت حرف Curl_conn_cf_get_transport(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *داده)
{
  صحيح transport = 0;
  إذا(cf && !cf->cft->query(cf, داده, CF_QUERY_TRANSPORT, &transport, عدم))
    أرجع (بیعلامت حرف)transport;
  أرجع (بیعلامت حرف)(داده->conn ? داده->conn->transport_wanted : 0);
}

ثابت حرف *Curl_conn_cf_get_alpn_negotiated(بنية Curl_cfilter *cf,
                                             بنية Curl_easy *داده)
{
  ثابت حرف *alpn = عدم;
  CURL_TRC_CF(داده, cf, "query ALPN");
  إذا(cf && !cf->cft->query(cf, داده, CF_QUERY_ALPN_NEGOTIATED, عدم,
                           CURL_UNCONST(&alpn)))
    أرجع alpn;
  أرجع عدم;
}

ساكن ثابت بنية Curl_sockaddr_ex *cf_get_remote_addr(
  بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  ثابت بنية Curl_sockaddr_ex *remote_addr = عدم;
  إذا(cf &&
     !cf->cft->query(cf, داده, CF_QUERY_REMOTE_ADDR, عدم,
                     CURL_UNCONST(&remote_addr)))
    أرجع remote_addr;
  أرجع عدم;
}

CURLcode Curl_conn_cf_get_ip_info(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  bool *is_ipv6, بنية ip_quadruple *ipquad)
{
  CURLcode نتیجه = CURLE_UNKNOWN_OPTION;
  إذا(cf) {
    صحيح ipv6 = 0;
    نتیجه = cf->cft->query(cf, داده, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  أرجع نتیجه;
}

curl_socket_t Curl_conn_get_اولی_socket(بنية Curl_easy *داده)
{
  بنية Curl_cfilter *cf;

  إذا(!داده->conn)
    أرجع CURL_SOCKET_BAD;

  cf = داده->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  إذا(cf && !cf->connected)
    أرجع Curl_conn_cf_get_socket(cf, داده);
  أرجع داده->conn->sock[FIRSTSOCKET];
}

ثابت بنية Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  بنية Curl_easy *داده, int8_t sockindex)
{
  بنية Curl_cfilter *cf =
    (داده->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    داده->conn->cfilter[sockindex] : عدم;
  أرجع cf ? cf_get_remote_addr(cf, داده) : عدم;
}

CURLcode Curl_conn_ev_داده_راهاندازی(بنية Curl_easy *داده)
{
  أرجع cf_cntrl_all(داده->conn, داده, FALSE, CF_CTRL_داده_راهاندازی, 0, عدم);
}

CURLcode Curl_conn_flush(بنية Curl_easy *داده, int8_t sockindex)
{
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  أرجع Curl_conn_cf_cntrl(داده->conn->cfilter[sockindex], داده, FALSE,
                            CF_CTRL_FLUSH, 0, عدم);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
فراغ Curl_conn_ev_داده_انجامشده_send(بنية Curl_easy *داده)
{
  cf_cntrl_all(داده->conn, داده, TRUE, CF_CTRL_داده_انجامشده_SEND, 0, عدم);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
فراغ Curl_conn_ev_داده_انجامشده(بنية Curl_easy *داده, bool premature)
{
  cf_cntrl_all(داده->conn, داده, TRUE, CF_CTRL_داده_انجامشده, premature, عدم);
}

CURLcode Curl_conn_ev_داده_pause(بنية Curl_easy *داده, bool do_pause)
{
  أرجع cf_cntrl_all(داده->conn, داده, FALSE,
                      CF_CTRL_داده_PAUSE, do_pause, عدم);
}

bool Curl_conn_is_alive(بنية Curl_easy *داده, بنية connectdata *conn,
                        bool *درون_داد_pending)
{
  بنية Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  أرجع cf && !cf->conn->بیتها.close &&
         cf->cft->is_alive(cf, داده, درون_داد_pending);
}

CURLcode Curl_conn_keep_alive(بنية Curl_easy *داده,
                              بنية connectdata *conn)
{
  CURLcode نتیجه = CURLE_OK;
  صحيح i;

  لكل(i = 0; (i < (صحيح)CURL_ARRAYSIZE(conn->cfilter)) && !نتیجه; ++i) {
    بنية Curl_cfilter *cf = conn->cfilter[i];
    إذا(cf)
      نتیجه = cf->cft->keep_alive(cf, داده);
  }
  أرجع نتیجه;
}

size_t Curl_conn_get_max_concurrent(بنية Curl_easy *داده,
                                    بنية connectdata *conn,
                                    int8_t sockindex)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجه;
  صحيح n = -1;

  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع 0;

  cf = conn->cfilter[sockindex];
  نتیجه = cf ? cf->cft->query(cf, داده, CF_QUERY_MAX_CONCURRENT,
                               &n, عدم) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  أرجع (نتیجه || n < 0) ? 1 : (size_t)n;
}

صحيح Curl_conn_get_جریان_error(بنية Curl_easy *داده,
                               بنية connectdata *conn,
                               int8_t sockindex)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجه;
  صحيح n = 0;

  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع 0;

  cf = conn->cfilter[sockindex];
  نتیجه = cf ? cf->cft->query(cf, داده, CF_QUERY_جریان_ERROR,
                               &n, عدم) : CURLE_UNKNOWN_OPTION;
  أرجع (نتیجه || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(بنية Curl_easy *داده, curl_socket_t sockfd)
{
  إذا(داده && داده->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == داده->conn->sock[SECONDARYSOCKET])
    أرجع SECONDARYSOCKET;
  أرجع FIRSTSOCKET;
}

CURLcode Curl_conn_recv(بنية Curl_easy *داده, int8_t sockindex,
                        حرف *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(داده);
  DEBUGASSERT(داده->conn);
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  إذا(داده && داده->conn && داده->conn->recv[sockindex])
    أرجع داده->conn->recv[sockindex](داده, sockindex, buf, len, pnread);
  *pnread = 0;
  أرجع CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(بنية Curl_easy *داده, int8_t sockindex,
                        ثابت فراغ *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(داده);
  DEBUGASSERT(داده->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_آرگومان;
#اگر_تعریف_شده DEBUGBUILD
  إذا(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    ثابت حرف *p = getenv("CURL_SMALLSENDS");
    إذا(p) {
      curl_off_t altsize;
      إذا(!curlx_str_عدد(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        إذا(write_len != len)
          eos = FALSE;
      }
    }
  }
#پایان_اگر
  إذا(داده && داده->conn && داده->conn->send[sockindex]) {
    CURLcode نتیجه = داده->conn->send[sockindex](داده, sockindex, buf,
                                                  write_len, eos, pnwritten);
    إذا(!نتیجه && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      أرجع CURLE_SEND_ERROR;
    }
    أرجع نتیجه;
  }
  *pnwritten = 0;
  أرجع CURLE_FAILED_INIT;
}
