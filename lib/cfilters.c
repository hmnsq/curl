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
                              بنية Curl_easy *معلومات, bool *پورو)
{
  CURLcode نتيجو;

  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  إذا(cf->ايندڙ) {
    نتيجو = cf->ايندڙ->cft->do_connect(cf->ايندڙ, معلومات, پورو);
    إذا(نتيجو || !*پورو)
      أرجع نتيجو;
  }

  cf->connected = TRUE;
  *پورو = TRUE;
  أرجع CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات, bool *پورو)
{
  (فراغ)cf;
  (فراغ)معلومات;
  *پورو = TRUE;
  أرجع CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية easy_pollset *ps)
{
  /* NOP */
  (فراغ)cf;
  (فراغ)معلومات;
  (فراغ)ps;
  أرجع CURLE_OK;
}

bool Curl_cf_def_معلومات_pending(بنية Curl_cfilter *cf,
                              ثابت بنية Curl_easy *معلومات)
{
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->has_معلومات_pending(cf->ايندڙ, معلومات) : FALSE;
}

CURLcode Curl_cf_def_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                          ثابت uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  إذا(cf->ايندڙ)
    أرجع cf->ايندڙ->cft->do_send(cf->ايندڙ, معلومات, buf, len, eos, pnwritten);
  *pnwritten = 0;
  أرجع CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                          حرف *buf, size_t len, size_t *pnread)
{
  إذا(cf->ايندڙ)
    أرجع cf->ايندڙ->cft->do_recv(cf->ايندڙ, معلومات, buf, len, pnread);
  *pnread = 0;
  أرجع CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               bool *داخلا_pending)
{
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->is_alive(cf->ايندڙ, معلومات, داخلا_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات)
{
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->keep_alive(cf->ايندڙ, معلومات) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات,
                           صحيح query, صحيح *pres1, فراغ *pres2)
{
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->query(cf->ايندڙ, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
فراغ Curl_conn_trc_filters(بنية Curl_easy *معلومات,
                           int8_t sockindex, ثابت حرف *info)
{
  إذا(CURL_TRC_M_is_verbose(معلومات) && معلومات->conn) {
    بنية Curl_cfilter *cf = معلومات->conn->cfilter[sockindex];

    إذا(cf) {
      حرف نياپو[256], *buf;
      صحيح blen, n;

      buf = نياپو;
      blen = حجم(نياپو) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      لكل(; cf && blen; cf = cf->ايندڙ) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(معلومات, "%s%s", نياپو, blen ? "" : "...");
    }
    وإلا
      CURL_TRC_M(معلومات, "%s [%d][-]", info, sockindex);
  }
}
#جيڪڏهن_پورو /* CURLVERBOSE */

فراغ Curl_conn_cf_discard_chain(بنية Curl_cfilter **pcf,
                                بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cfn, *cf = *pcf;

  إذا(cf) {
    *pcf = عدم;
    طالما(cf) {
      cfn = cf->ايندڙ;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->ايندڙ = عدم;
      cf->cft->destroy(cf, معلومات);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

فراغ Curl_conn_cf_discard_all(بنية Curl_easy *معلومات,
                              بنية connectdata *conn, int8_t sockindex)
{
  conn->shutdown.شروعات_ms[sockindex] = -1;
  conn->connect_started.سيٽ[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], معلومات);
}

CURLcode Curl_cf_recv(بنية Curl_easy *معلومات, int8_t sockindex, حرف *buf,
                      size_t len, size_t *pnread)
{
  بنية Curl_cfilter *cf;

  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->conn);
  cf = معلومات->conn->cfilter[sockindex];
  طالما(cf && !cf->connected)
    cf = cf->ايندڙ;
  إذا(cf)
    أرجع cf->cft->do_recv(cf, معلومات, buf, len, pnread);
  failf(معلومات, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  أرجع CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(بنية Curl_easy *معلومات, int8_t sockindex,
                      ثابت uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  بنية Curl_cfilter *cf;

  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->conn);
  cf = معلومات->conn->cfilter[sockindex];
  طالما(cf && !cf->connected)
    cf = cf->ايندڙ;
  إذا(cf) {
    أرجع cf->cft->do_send(cf, معلومات, buf, len, eos, pnwritten);
  }
  failf(معلومات, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  أرجع CURLE_FAILED_INIT;
}

بنية cf_io_ctx {
  بنية Curl_easy *معلومات;
  بنية Curl_cfilter *cf;
};

ساكن CURLcode cf_bufq_reader(فراغ *writer_ctx,
                               بنانشان حرف *buf, size_t blen,
                               size_t *pnread)
{
  بنية cf_io_ctx *io = writer_ctx;
  أرجع Curl_conn_cf_recv(io->cf, io->معلومات, (حرف *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات,
                           بنية bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  بنية cf_io_ctx io;

  إذا(!cf || !معلومات) {
    *pnread = 0;
    أرجع CURLE_BAD_FUNCTION_دليل;
  }
  io.معلومات = معلومات;
  io.cf = cf;
  أرجع Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

ساكن CURLcode cf_bufq_writer(فراغ *writer_ctx,
                               ثابت uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  بنية cf_io_ctx *io = writer_ctx;
  أرجع Curl_conn_cf_send(io->cf, io->معلومات, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات,
                           بنية bufq *bufq,
                           ثابت بنانشان حرف *buf, size_t blen,
                           size_t *pnwritten)
{
  بنية cf_io_ctx io;

  إذا(!cf || !معلومات) {
    *pnwritten = 0;
    أرجع CURLE_BAD_FUNCTION_دليل;
  }
  io.معلومات = معلومات;
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
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, حجم(*cf));
  إذا(!cf)
    اذهب ٻاهريون;

  cf->cft = cft;
  cf->ctx = ctx;
  نتيجو = CURLE_OK;
ٻاهريون:
  *pcf = cf;
  أرجع نتيجو;
}

فراغ Curl_conn_cf_جوڙيو(بنية Curl_easy *معلومات,
                      بنية connectdata *conn,
                      int8_t sockindex,
                      بنية Curl_cfilter *cf)
{
  DEBUGASSERT(conn);
  DEBUGASSERT(!cf->conn);
  DEBUGASSERT(!cf->ايندڙ);

  cf->ايندڙ = conn->cfilter[sockindex];
  cf->conn = conn;
  cf->sockindex = sockindex;
  conn->cfilter[sockindex] = cf;
  CURL_TRC_CF(معلومات, cf, "added");
}

فراغ Curl_conn_cf_وجهو_after(بنية Curl_cfilter *cf_at,
                               بنية Curl_cfilter *cf_new)
{
  بنية Curl_cfilter *پڇ, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  پڇ = cf_at->ايندڙ;
  cf_at->ايندڙ = cf_new;
  افعل {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->ايندڙ;
    cf_new = cf_new->ايندڙ;
  } طالما(cf_new);
  *pnext = پڇ;
}

bool Curl_conn_cf_discard(بنية Curl_cfilter **pcf,
                          بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cf = pcf ? *pcf : عدم;
  bool مليو = FALSE;
  إذا(cf) {
    إذا(cf->conn) {
      /* unlink if present in connection filter chain */
      بنية Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      طالما(*pprev) {
        إذا(*pprev == *pcf) {
          *pprev = (*pcf)->ايندڙ;
          cf->ايندڙ = عدم;
          مليو = TRUE;
          اكسر;
        }
        pprev = &(*pprev)->ايندڙ;
      }
    }
    Curl_conn_cf_discard_chain(pcf, معلومات);
  }
  أرجع مليو;
}

CURLcode Curl_conn_cf_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              bool *پورو)
{
  إذا(cf)
    أرجع cf->cft->do_connect(cf, معلومات, پورو);
  أرجع CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                           ثابت uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  إذا(cf)
    أرجع cf->cft->do_send(cf, معلومات, buf, len, eos, pnwritten);
  *pnwritten = 0;
  أرجع CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                           حرف *buf, size_t len, size_t *pnread)
{
  إذا(cf)
    أرجع cf->cft->do_recv(cf, معلومات, buf, len, pnread);
  *pnread = 0;
  أرجع CURLE_RECV_ERROR;
}

ساكن CURLcode cf_cntrl_all(بنية connectdata *conn,
                             بنية Curl_easy *معلومات,
                             bool ignore_نتيجو,
                             صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode نتيجو = CURLE_OK;
  صحيح i;

  لكل(i = 0; i < (صحيح)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    نتيجو = Curl_conn_cf_cntrl(conn->cfilter[i], معلومات, ignore_نتيجو,
                                event, arg1, arg2);
    إذا(!ignore_نتيجو && نتيجو)
      اكسر;
  }
  أرجع نتيجو;
}

bool Curl_conn_cf_is_ip_connected(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات)
{
  إذا(cf) {
    صحيح قدر = 0;
    إذا(!cf->cft->query(cf, معلومات, CF_QUERY_REALLY_CONNECTED, &قدر, عدم))
      أرجع !!قدر;
  }
  أرجع FALSE;
}

فراغ Curl_conn_cntrl_update_info(بنية Curl_easy *معلومات,
                                بنية connectdata *conn)
{
  cf_cntrl_all(conn, معلومات, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, عدم);
}

فراغ Curl_conn_cntrl_report_stats(بنية Curl_easy *معلومات,
                                  بنية connectdata *conn,
                                  صحيح sockindex)
{
  إذا((بنانشان)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (فراغ)Curl_conn_cf_cntrl(conn->cfilter[sockindex], معلومات, TRUE,
                             CF_CTRL_REPORT_STATS, 0, عدم);
}

فراغ Curl_conn_remove_تياري_filters(بنية Curl_easy *معلومات,
                                    int8_t sockindex)
{
  بنية Curl_cfilter **anchor = &معلومات->conn->cfilter[sockindex];
  طالما(*anchor) {
    بنية Curl_cfilter *cf = *anchor;
    إذا(cf->connected && (cf->cft->flags & CF_TYPE_تياري)) {
      *anchor = cf->ايندڙ;
      cf->ايندڙ = عدم;
      CURL_TRC_CF(معلومات, cf, "removing connected setup filter");
      cf->cft->destroy(cf, معلومات);
      curlx_free(cf);
    }
    وإلا
      anchor = &cf->ايندڙ;
  }
}

bool Curl_conn_is_تياري(بنية connectdata *conn, int8_t sockindex)
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

bool Curl_conn_is_ip_connected(بنية Curl_easy *معلومات, int8_t sockindex)
{
  بنية Curl_cfilter *cf;

  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  cf = معلومات->conn->cfilter[sockindex];
  طالما(cf) {
    إذا(cf->connected)
      أرجع TRUE;
    إذا(cf->cft->flags & CF_TYPE_IP_CONNECT)
      أرجع FALSE;
    cf = cf->ايندڙ;
  }
  أرجع FALSE;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
ساكن bool cf_is_tunneling(بنية Curl_cfilter *cf)
{
  لكل(; cf; cf = cf->ايندڙ) {
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
#جيڪڏهن_پورو /* CURL_DISABLE_PROXY */

ساكن bool cf_is_ssl(بنية Curl_cfilter *cf)
{
  لكل(; cf; cf = cf->ايندڙ) {
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

bool Curl_conn_get_ssl_info(بنية Curl_easy *معلومات,
                            بنية connectdata *conn, int8_t sockindex,
                            صحيح query,
                            بنية curl_tlssessioninfo *info)
{
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  إذا(Curl_conn_is_ssl(conn, sockindex)) {
    بنية Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode نتيجو = cf ?
      cf->cft->query(cf, معلومات, query, عدم, (فراغ *)info) :
      CURLE_UNKNOWN_OPTION;
    أرجع !نتيجو;
  }
  أرجع FALSE;
}

CURLcode Curl_conn_get_ip_info(بنية Curl_easy *معلومات,
                               بنية connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, بنية ip_quadruple *ipquad)
{
  بنية Curl_cfilter *cf;
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_دليل;
  cf = conn ? conn->cfilter[sockindex] : عدم;
  أرجع Curl_conn_cf_get_ip_info(cf, معلومات, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(بنية connectdata *conn, int8_t sockindex)
{
  بنية Curl_cfilter *cf;

  إذا(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  لكل(cf = conn->cfilter[sockindex]; cf; cf = cf->ايندڙ) {
    إذا(cf->cft->flags & CF_TYPE_MULTIPLEX)
      أرجع TRUE;
    إذا(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      أرجع FALSE;
  }
  أرجع FALSE;
}

بنانشان حرف Curl_conn_get_transport(بنية Curl_easy *معلومات,
                                      بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  أرجع Curl_conn_cf_get_transport(cf, معلومات);
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

ثابت حرف *Curl_conn_get_alpn_negotiated(بنية Curl_easy *معلومات,
                                          بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  أرجع Curl_conn_cf_get_alpn_negotiated(cf, معلومات);
}

بنانشان حرف Curl_conn_http_نسخو(بنية Curl_easy *معلومات,
                                     بنية connectdata *conn)
{
  بنية Curl_cfilter *cf;
  CURLcode نتيجو = CURLE_UNKNOWN_OPTION;
  بنانشان حرف v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  لكل(; cf; cf = cf->ايندڙ) {
    إذا(cf->cft->flags & CF_TYPE_HTTP) {
      صحيح قدر = 0;
      نتيجو = cf->cft->query(cf, معلومات, CF_QUERY_HTTP_نسخو, &قدر, عدم);
      إذا(!نتيجو && ((قدر < 0) || (قدر > 255)))
        نتيجو = CURLE_FAILED_INIT;
      وإلا
        v = (بنانشان حرف)قدر;
      اكسر;
    }
    إذا(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      اكسر;
  }
  أرجع (بنانشان حرف)(نتيجو ? 0 : v);
}

bool Curl_conn_معلومات_pending(بنية Curl_easy *معلومات, int8_t sockindex)
{
  بنية Curl_cfilter *cf;

  (فراغ)معلومات;
  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->conn);
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;

  cf = معلومات->conn->cfilter[sockindex];
  طالما(cf && !cf->connected) {
    cf = cf->ايندڙ;
  }
  إذا(cf) {
    أرجع cf->cft->has_معلومات_pending(cf, معلومات);
  }
  أرجع FALSE;
}

bool Curl_conn_cf_needs_flush(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات)
{
  CURLcode نتيجو;
  صحيح pending = 0;
  نتيجو = cf ? cf->cft->query(cf, معلومات, CF_QUERY_NEED_FLUSH,
                               &pending, عدم) : CURLE_UNKNOWN_OPTION;
  أرجع (نتيجو || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(بنية Curl_easy *معلومات, int8_t sockindex)
{
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  أرجع Curl_conn_cf_needs_flush(معلومات->conn->cfilter[sockindex], معلومات);
}

CURLcode Curl_conn_cf_adjust_pollset(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات,
                                     بنية easy_pollset *ps)
{
  CURLcode نتيجو = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  لكل(; cf && !نتيجو; cf = cf->ايندڙ) {
    إذا(cf->shutdown)
      استمر;
    إذا(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->ايندڙ || cf->ايندڙ->connected)
      نتيجو = cf->cft->adjust_pollset(cf, معلومات, ps);
  }
  أرجع نتيجو;
}

CURLcode Curl_conn_adjust_pollset(بنية Curl_easy *معلومات,
                                  بنية connectdata *conn,
                                  بنية easy_pollset *ps)
{
  CURLcode نتيجو = CURLE_OK;
  ثابت bool want_io = !!ps->n;
  int8_t i;

  DEBUGASSERT(معلومات);
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
  لكل(i = 0; (i < (صحيح)CURL_ARRAYSIZE(conn->cfilter)) && !نتيجو; ++i) {
    إذا(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_اندريون_SHUTDOWN(conn, i)))
      نتيجو = Curl_conn_cf_adjust_pollset(conn->cfilter[i], معلومات, ps);
  }
  أرجع نتيجو;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
صحيح Curl_conn_cf_poll(بنية Curl_cfilter *cf,
                      بنية Curl_easy *معلومات,
                      timediff_t timeout_ms)
{
  بنية easy_pollset ps;
  صحيح rc;
  CURLcode نتيجو;

  DEBUGASSERT(cf);
  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->conn);
  Curl_pollset_init(&ps);

  نتيجو = Curl_conn_cf_adjust_pollset(cf, معلومات, &ps);
  إذا(!نتيجو)
    rc = Curl_pollset_poll(معلومات, &ps, timeout_ms);
  وإلا
    rc = -1;
  Curl_pollset_cleanup(&ps);
  أرجع rc;
}

فراغ Curl_conn_get_current_host(بنية Curl_easy *معلومات, int8_t sockindex,
                                ثابت حرف **phost, صحيح *pport)
{
  بنية Curl_cfilter *cf, *cf_proxy = عدم;
  صحيح portarg = -1;

  إذا(!معلومات->conn) {
    DEBUGASSERT(0);
    *phost = "";
    إذا(pport)
      *pport = -1;
    أرجع;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? معلومات->conn->cfilter[sockindex] : عدم;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  طالما(cf && !cf->connected) {
    إذا((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->ايندڙ;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  إذا(!cf_proxy || cf_proxy->cft->query(cf_proxy, معلومات, CF_QUERY_HOST_پورٽ,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = معلومات->conn->origin->hostname;
    portarg = معلومات->conn->origin->پورٽ;
  }
  إذا(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات,
                           صحيح event, صحيح arg1, فراغ *arg2)
{
  (فراغ)cf;
  (فراغ)معلومات;
  (فراغ)event;
  (فراغ)arg1;
  (فراغ)arg2;
  أرجع CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            bool ignore_نتيجو,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode نتيجو = CURLE_OK;

  لكل(; cf; cf = cf->ايندڙ) {
    إذا(cf->cft->cntrl == Curl_cf_def_cntrl)
      استمر;
    نتيجو = cf->cft->cntrl(cf, معلومات, event, arg1, arg2);
    إذا(!ignore_نتيجو && نتيجو)
      اكسر;
  }
  أرجع نتيجو;
}

curl_socket_t Curl_conn_cf_get_socket(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات)
{
  curl_socket_t sock;
  إذا(cf && !cf->cft->query(cf, معلومات, CF_QUERY_SOCKET, عدم, &sock))
    أرجع sock;
  أرجع CURL_SOCKET_BAD;
}

بنانشان حرف Curl_conn_cf_get_transport(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات)
{
  صحيح transport = 0;
  إذا(cf && !cf->cft->query(cf, معلومات, CF_QUERY_TRANSPORT, &transport, عدم))
    أرجع (بنانشان حرف)transport;
  أرجع (بنانشان حرف)(معلومات->conn ? معلومات->conn->transport_wanted : 0);
}

ثابت حرف *Curl_conn_cf_get_alpn_negotiated(بنية Curl_cfilter *cf,
                                             بنية Curl_easy *معلومات)
{
  ثابت حرف *alpn = عدم;
  CURL_TRC_CF(معلومات, cf, "query ALPN");
  إذا(cf && !cf->cft->query(cf, معلومات, CF_QUERY_ALPN_NEGOTIATED, عدم,
                           CURL_UNCONST(&alpn)))
    أرجع alpn;
  أرجع عدم;
}

ساكن ثابت بنية Curl_sockaddr_ex *cf_get_remote_addr(
  بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  ثابت بنية Curl_sockaddr_ex *remote_addr = عدم;
  إذا(cf &&
     !cf->cft->query(cf, معلومات, CF_QUERY_REMOTE_ADDR, عدم,
                     CURL_UNCONST(&remote_addr)))
    أرجع remote_addr;
  أرجع عدم;
}

CURLcode Curl_conn_cf_get_ip_info(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  bool *is_ipv6, بنية ip_quadruple *ipquad)
{
  CURLcode نتيجو = CURLE_UNKNOWN_OPTION;
  إذا(cf) {
    صحيح ipv6 = 0;
    نتيجو = cf->cft->query(cf, معلومات, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  أرجع نتيجو;
}

curl_socket_t Curl_conn_get_پهريون_socket(بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cf;

  إذا(!معلومات->conn)
    أرجع CURL_SOCKET_BAD;

  cf = معلومات->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  إذا(cf && !cf->connected)
    أرجع Curl_conn_cf_get_socket(cf, معلومات);
  أرجع معلومات->conn->sock[FIRSTSOCKET];
}

ثابت بنية Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  بنية Curl_easy *معلومات, int8_t sockindex)
{
  بنية Curl_cfilter *cf =
    (معلومات->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    معلومات->conn->cfilter[sockindex] : عدم;
  أرجع cf ? cf_get_remote_addr(cf, معلومات) : عدم;
}

CURLcode Curl_conn_ev_معلومات_تياري(بنية Curl_easy *معلومات)
{
  أرجع cf_cntrl_all(معلومات->conn, معلومات, FALSE, CF_CTRL_معلومات_تياري, 0, عدم);
}

CURLcode Curl_conn_flush(بنية Curl_easy *معلومات, int8_t sockindex)
{
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_دليل;
  أرجع Curl_conn_cf_cntrl(معلومات->conn->cfilter[sockindex], معلومات, FALSE,
                            CF_CTRL_FLUSH, 0, عدم);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
فراغ Curl_conn_ev_معلومات_پورو_send(بنية Curl_easy *معلومات)
{
  cf_cntrl_all(معلومات->conn, معلومات, TRUE, CF_CTRL_معلومات_پورو_SEND, 0, عدم);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
فراغ Curl_conn_ev_معلومات_پورو(بنية Curl_easy *معلومات, bool premature)
{
  cf_cntrl_all(معلومات->conn, معلومات, TRUE, CF_CTRL_معلومات_پورو, premature, عدم);
}

CURLcode Curl_conn_ev_معلومات_pause(بنية Curl_easy *معلومات, bool do_pause)
{
  أرجع cf_cntrl_all(معلومات->conn, معلومات, FALSE,
                      CF_CTRL_معلومات_PAUSE, do_pause, عدم);
}

bool Curl_conn_is_alive(بنية Curl_easy *معلومات, بنية connectdata *conn,
                        bool *داخلا_pending)
{
  بنية Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  أرجع cf && !cf->conn->بٽ.close &&
         cf->cft->is_alive(cf, معلومات, داخلا_pending);
}

CURLcode Curl_conn_keep_alive(بنية Curl_easy *معلومات,
                              بنية connectdata *conn)
{
  CURLcode نتيجو = CURLE_OK;
  صحيح i;

  لكل(i = 0; (i < (صحيح)CURL_ARRAYSIZE(conn->cfilter)) && !نتيجو; ++i) {
    بنية Curl_cfilter *cf = conn->cfilter[i];
    إذا(cf)
      نتيجو = cf->cft->keep_alive(cf, معلومات);
  }
  أرجع نتيجو;
}

size_t Curl_conn_get_max_concurrent(بنية Curl_easy *معلومات,
                                    بنية connectdata *conn,
                                    int8_t sockindex)
{
  بنية Curl_cfilter *cf;
  CURLcode نتيجو;
  صحيح n = -1;

  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع 0;

  cf = conn->cfilter[sockindex];
  نتيجو = cf ? cf->cft->query(cf, معلومات, CF_QUERY_MAX_CONCURRENT,
                               &n, عدم) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  أرجع (نتيجو || n < 0) ? 1 : (size_t)n;
}

صحيح Curl_conn_get_وهڪرو_error(بنية Curl_easy *معلومات,
                               بنية connectdata *conn,
                               int8_t sockindex)
{
  بنية Curl_cfilter *cf;
  CURLcode نتيجو;
  صحيح n = 0;

  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع 0;

  cf = conn->cfilter[sockindex];
  نتيجو = cf ? cf->cft->query(cf, معلومات, CF_QUERY_وهڪرو_ERROR,
                               &n, عدم) : CURLE_UNKNOWN_OPTION;
  أرجع (نتيجو || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(بنية Curl_easy *معلومات, curl_socket_t sockfd)
{
  إذا(معلومات && معلومات->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == معلومات->conn->sock[SECONDARYSOCKET])
    أرجع SECONDARYSOCKET;
  أرجع FIRSTSOCKET;
}

CURLcode Curl_conn_recv(بنية Curl_easy *معلومات, int8_t sockindex,
                        حرف *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->conn);
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_دليل;
  إذا(معلومات && معلومات->conn && معلومات->conn->recv[sockindex])
    أرجع معلومات->conn->recv[sockindex](معلومات, sockindex, buf, len, pnread);
  *pnread = 0;
  أرجع CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(بنية Curl_easy *معلومات, int8_t sockindex,
                        ثابت فراغ *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_دليل;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
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
#جيڪڏهن_پورو
  إذا(معلومات && معلومات->conn && معلومات->conn->send[sockindex]) {
    CURLcode نتيجو = معلومات->conn->send[sockindex](معلومات, sockindex, buf,
                                                  write_len, eos, pnwritten);
    إذا(!نتيجو && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      أرجع CURLE_SEND_ERROR;
    }
    أرجع نتيجو;
  }
  *pnwritten = 0;
  أرجع CURLE_FAILED_INIT;
}
