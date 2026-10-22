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
                              بنية Curl_easy *معلومات, bool *بشپړ)
{
  CURLcode پایله;

  إذا(cf->connected) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  إذا(cf->بل) {
    پایله = cf->بل->cft->do_connect(cf->بل, معلومات, بشپړ);
    إذا(پایله || !*بشپړ)
      أرجع پایله;
  }

  cf->connected = TRUE;
  *بشپړ = TRUE;
  أرجع CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات, bool *بشپړ)
{
  (فراغ)cf;
  (فراغ)معلومات;
  *بشپړ = TRUE;
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
  أرجع cf->بل ?
    cf->بل->cft->has_معلومات_pending(cf->بل, معلومات) : FALSE;
}

CURLcode Curl_cf_def_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                          ثابت uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  إذا(cf->بل)
    أرجع cf->بل->cft->do_send(cf->بل, معلومات, buf, len, eos, pnwritten);
  *pnwritten = 0;
  أرجع CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                          حرف *buf, size_t len, size_t *pnread)
{
  إذا(cf->بل)
    أرجع cf->بل->cft->do_recv(cf->بل, معلومات, buf, len, pnread);
  *pnread = 0;
  أرجع CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               bool *ننوت_pending)
{
  أرجع cf->بل ?
    cf->بل->cft->is_alive(cf->بل, معلومات, ننوت_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات)
{
  أرجع cf->بل ?
    cf->بل->cft->keep_alive(cf->بل, معلومات) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات,
                           صحيح query, صحيح *pres1, فراغ *pres2)
{
  أرجع cf->بل ?
    cf->بل->cft->query(cf->بل, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#که_تعریف_شوی CURLVERBOSE
فراغ Curl_conn_trc_filters(بنية Curl_easy *معلومات,
                           int8_t sockindex, ثابت حرف *info)
{
  إذا(CURL_TRC_M_is_verbose(معلومات) && معلومات->conn) {
    بنية Curl_cfilter *cf = معلومات->conn->cfilter[sockindex];

    إذا(cf) {
      حرف پیغام[256], *buf;
      صحيح blen, n;

      buf = پیغام;
      blen = حجم(پیغام) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      لكل(; cf && blen; cf = cf->بل) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(معلومات, "%s%s", پیغام, blen ? "" : "...");
    }
    وإلا
      CURL_TRC_M(معلومات, "%s [%d][-]", info, sockindex);
  }
}
#د_که_پای /* CURLVERBOSE */

فراغ Curl_conn_cf_discard_chain(بنية Curl_cfilter **pcf,
                                بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cfn, *cf = *pcf;

  إذا(cf) {
    *pcf = عدم;
    طالما(cf) {
      cfn = cf->بل;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->بل = عدم;
      cf->cft->destroy(cf, معلومات);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

فراغ Curl_conn_cf_discard_all(بنية Curl_easy *معلومات,
                              بنية connectdata *conn, int8_t sockindex)
{
  conn->shutdown.پیل_ms[sockindex] = -1;
  conn->connect_started.سېټ[sockindex] = FALSE;
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
    cf = cf->بل;
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
    cf = cf->بل;
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
                               بینښه حرف *buf, size_t blen,
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
    أرجع CURLE_BAD_FUNCTION_دلیل;
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
                           ثابت بینښه حرف *buf, size_t blen,
                           size_t *pnwritten)
{
  بنية cf_io_ctx io;

  إذا(!cf || !معلومات) {
    *pnwritten = 0;
    أرجع CURLE_BAD_FUNCTION_دلیل;
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
  CURLcode پایله = CURLE_بهر_د_حافظه;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, حجم(*cf));
  إذا(!cf)
    اذهب بهر;

  cf->cft = cft;
  cf->ctx = ctx;
  پایله = CURLE_OK;
بهر:
  *pcf = cf;
  أرجع پایله;
}

فراغ Curl_conn_cf_جمع_کړه(بنية Curl_easy *معلومات,
                      بنية connectdata *conn,
                      int8_t sockindex,
                      بنية Curl_cfilter *cf)
{
  DEBUGASSERT(conn);
  DEBUGASSERT(!cf->conn);
  DEBUGASSERT(!cf->بل);

  cf->بل = conn->cfilter[sockindex];
  cf->conn = conn;
  cf->sockindex = sockindex;
  conn->cfilter[sockindex] = cf;
  CURL_TRC_CF(معلومات, cf, "added");
}

فراغ Curl_conn_cf_ورننباسه_after(بنية Curl_cfilter *cf_at,
                               بنية Curl_cfilter *cf_new)
{
  بنية Curl_cfilter *لکۍ, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  لکۍ = cf_at->بل;
  cf_at->بل = cf_new;
  افعل {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->بل;
    cf_new = cf_new->بل;
  } طالما(cf_new);
  *pnext = لکۍ;
}

bool Curl_conn_cf_discard(بنية Curl_cfilter **pcf,
                          بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cf = pcf ? *pcf : عدم;
  bool وموندل = FALSE;
  إذا(cf) {
    إذا(cf->conn) {
      /* unlink if present in connection filter chain */
      بنية Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      طالما(*pprev) {
        إذا(*pprev == *pcf) {
          *pprev = (*pcf)->بل;
          cf->بل = عدم;
          وموندل = TRUE;
          اكسر;
        }
        pprev = &(*pprev)->بل;
      }
    }
    Curl_conn_cf_discard_chain(pcf, معلومات);
  }
  أرجع وموندل;
}

CURLcode Curl_conn_cf_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              bool *بشپړ)
{
  إذا(cf)
    أرجع cf->cft->do_connect(cf, معلومات, بشپړ);
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
                             bool ignore_پایله,
                             صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode پایله = CURLE_OK;
  صحيح i;

  لكل(i = 0; i < (صحيح)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    پایله = Curl_conn_cf_cntrl(conn->cfilter[i], معلومات, ignore_پایله,
                                event, arg1, arg2);
    إذا(!ignore_پایله && پایله)
      اكسر;
  }
  أرجع پایله;
}

bool Curl_conn_cf_is_ip_connected(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات)
{
  إذا(cf) {
    صحيح ارزښت = 0;
    إذا(!cf->cft->query(cf, معلومات, CF_QUERY_REALLY_CONNECTED, &ارزښت, عدم))
      أرجع !!ارزښت;
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
  إذا((بینښه)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (فراغ)Curl_conn_cf_cntrl(conn->cfilter[sockindex], معلومات, TRUE,
                             CF_CTRL_REPORT_STATS, 0, عدم);
}

فراغ Curl_conn_remove_تیاری_filters(بنية Curl_easy *معلومات,
                                    int8_t sockindex)
{
  بنية Curl_cfilter **anchor = &معلومات->conn->cfilter[sockindex];
  طالما(*anchor) {
    بنية Curl_cfilter *cf = *anchor;
    إذا(cf->connected && (cf->cft->flags & CF_TYPE_تیاری)) {
      *anchor = cf->بل;
      cf->بل = عدم;
      CURL_TRC_CF(معلومات, cf, "removing connected setup filter");
      cf->cft->destroy(cf, معلومات);
      curlx_free(cf);
    }
    وإلا
      anchor = &cf->بل;
  }
}

bool Curl_conn_is_تیاری(بنية connectdata *conn, int8_t sockindex)
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
    cf = cf->بل;
  }
  أرجع FALSE;
}

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
ساكن bool cf_is_tunneling(بنية Curl_cfilter *cf)
{
  لكل(; cf; cf = cf->بل) {
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
#د_که_پای /* CURL_DISABLE_PROXY */

ساكن bool cf_is_ssl(بنية Curl_cfilter *cf)
{
  لكل(; cf; cf = cf->بل) {
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
    CURLcode پایله = cf ?
      cf->cft->query(cf, معلومات, query, عدم, (فراغ *)info) :
      CURLE_UNKNOWN_OPTION;
    أرجع !پایله;
  }
  أرجع FALSE;
}

CURLcode Curl_conn_get_ip_info(بنية Curl_easy *معلومات,
                               بنية connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, بنية ip_quadruple *ipquad)
{
  بنية Curl_cfilter *cf;
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_دلیل;
  cf = conn ? conn->cfilter[sockindex] : عدم;
  أرجع Curl_conn_cf_get_ip_info(cf, معلومات, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(بنية connectdata *conn, int8_t sockindex)
{
  بنية Curl_cfilter *cf;

  إذا(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    أرجع FALSE;
  لكل(cf = conn->cfilter[sockindex]; cf; cf = cf->بل) {
    إذا(cf->cft->flags & CF_TYPE_MULTIPLEX)
      أرجع TRUE;
    إذا(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      أرجع FALSE;
  }
  أرجع FALSE;
}

بینښه حرف Curl_conn_get_transport(بنية Curl_easy *معلومات,
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

بینښه حرف Curl_conn_http_نسخه(بنية Curl_easy *معلومات,
                                     بنية connectdata *conn)
{
  بنية Curl_cfilter *cf;
  CURLcode پایله = CURLE_UNKNOWN_OPTION;
  بینښه حرف v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  لكل(; cf; cf = cf->بل) {
    إذا(cf->cft->flags & CF_TYPE_HTTP) {
      صحيح ارزښت = 0;
      پایله = cf->cft->query(cf, معلومات, CF_QUERY_HTTP_نسخه, &ارزښت, عدم);
      إذا(!پایله && ((ارزښت < 0) || (ارزښت > 255)))
        پایله = CURLE_FAILED_INIT;
      وإلا
        v = (بینښه حرف)ارزښت;
      اكسر;
    }
    إذا(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      اكسر;
  }
  أرجع (بینښه حرف)(پایله ? 0 : v);
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
    cf = cf->بل;
  }
  إذا(cf) {
    أرجع cf->cft->has_معلومات_pending(cf, معلومات);
  }
  أرجع FALSE;
}

bool Curl_conn_cf_needs_flush(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات)
{
  CURLcode پایله;
  صحيح pending = 0;
  پایله = cf ? cf->cft->query(cf, معلومات, CF_QUERY_NEED_FLUSH,
                               &pending, عدم) : CURLE_UNKNOWN_OPTION;
  أرجع (پایله || !pending) ? FALSE : TRUE;
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
  CURLcode پایله = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  لكل(; cf && !پایله; cf = cf->بل) {
    إذا(cf->shutdown)
      استمر;
    إذا(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->بل || cf->بل->connected)
      پایله = cf->cft->adjust_pollset(cf, معلومات, ps);
  }
  أرجع پایله;
}

CURLcode Curl_conn_adjust_pollset(بنية Curl_easy *معلومات,
                                  بنية connectdata *conn,
                                  بنية easy_pollset *ps)
{
  CURLcode پایله = CURLE_OK;
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
  لكل(i = 0; (i < (صحيح)CURL_ARRAYSIZE(conn->cfilter)) && !پایله; ++i) {
    إذا(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_دننه_SHUTDOWN(conn, i)))
      پایله = Curl_conn_cf_adjust_pollset(conn->cfilter[i], معلومات, ps);
  }
  أرجع پایله;
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
  CURLcode پایله;

  DEBUGASSERT(cf);
  DEBUGASSERT(معلومات);
  DEBUGASSERT(معلومات->conn);
  Curl_pollset_init(&ps);

  پایله = Curl_conn_cf_adjust_pollset(cf, معلومات, &ps);
  إذا(!پایله)
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
    cf = cf->بل;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  إذا(!cf_proxy || cf_proxy->cft->query(cf_proxy, معلومات, CF_QUERY_HOST_پورټ,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = معلومات->conn->origin->hostname;
    portarg = معلومات->conn->origin->پورټ;
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
                            bool ignore_پایله,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode پایله = CURLE_OK;

  لكل(; cf; cf = cf->بل) {
    إذا(cf->cft->cntrl == Curl_cf_def_cntrl)
      استمر;
    پایله = cf->cft->cntrl(cf, معلومات, event, arg1, arg2);
    إذا(!ignore_پایله && پایله)
      اكسر;
  }
  أرجع پایله;
}

curl_socket_t Curl_conn_cf_get_socket(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات)
{
  curl_socket_t sock;
  إذا(cf && !cf->cft->query(cf, معلومات, CF_QUERY_SOCKET, عدم, &sock))
    أرجع sock;
  أرجع CURL_SOCKET_BAD;
}

بینښه حرف Curl_conn_cf_get_transport(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات)
{
  صحيح transport = 0;
  إذا(cf && !cf->cft->query(cf, معلومات, CF_QUERY_TRANSPORT, &transport, عدم))
    أرجع (بینښه حرف)transport;
  أرجع (بینښه حرف)(معلومات->conn ? معلومات->conn->transport_wanted : 0);
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
  CURLcode پایله = CURLE_UNKNOWN_OPTION;
  إذا(cf) {
    صحيح ipv6 = 0;
    پایله = cf->cft->query(cf, معلومات, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  أرجع پایله;
}

curl_socket_t Curl_conn_get_لومړی_socket(بنية Curl_easy *معلومات)
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

CURLcode Curl_conn_ev_معلومات_تیاری(بنية Curl_easy *معلومات)
{
  أرجع cf_cntrl_all(معلومات->conn, معلومات, FALSE, CF_CTRL_معلومات_تیاری, 0, عدم);
}

CURLcode Curl_conn_flush(بنية Curl_easy *معلومات, int8_t sockindex)
{
  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع CURLE_BAD_FUNCTION_دلیل;
  أرجع Curl_conn_cf_cntrl(معلومات->conn->cfilter[sockindex], معلومات, FALSE,
                            CF_CTRL_FLUSH, 0, عدم);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
فراغ Curl_conn_ev_معلومات_بشپړ_send(بنية Curl_easy *معلومات)
{
  cf_cntrl_all(معلومات->conn, معلومات, TRUE, CF_CTRL_معلومات_بشپړ_SEND, 0, عدم);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
فراغ Curl_conn_ev_معلومات_بشپړ(بنية Curl_easy *معلومات, bool premature)
{
  cf_cntrl_all(معلومات->conn, معلومات, TRUE, CF_CTRL_معلومات_بشپړ, premature, عدم);
}

CURLcode Curl_conn_ev_معلومات_pause(بنية Curl_easy *معلومات, bool do_pause)
{
  أرجع cf_cntrl_all(معلومات->conn, معلومات, FALSE,
                      CF_CTRL_معلومات_PAUSE, do_pause, عدم);
}

bool Curl_conn_is_alive(بنية Curl_easy *معلومات, بنية connectdata *conn,
                        bool *ننوت_pending)
{
  بنية Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  أرجع cf && !cf->conn->بټونه.close &&
         cf->cft->is_alive(cf, معلومات, ننوت_pending);
}

CURLcode Curl_conn_keep_alive(بنية Curl_easy *معلومات,
                              بنية connectdata *conn)
{
  CURLcode پایله = CURLE_OK;
  صحيح i;

  لكل(i = 0; (i < (صحيح)CURL_ARRAYSIZE(conn->cfilter)) && !پایله; ++i) {
    بنية Curl_cfilter *cf = conn->cfilter[i];
    إذا(cf)
      پایله = cf->cft->keep_alive(cf, معلومات);
  }
  أرجع پایله;
}

size_t Curl_conn_get_max_concurrent(بنية Curl_easy *معلومات,
                                    بنية connectdata *conn,
                                    int8_t sockindex)
{
  بنية Curl_cfilter *cf;
  CURLcode پایله;
  صحيح n = -1;

  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع 0;

  cf = conn->cfilter[sockindex];
  پایله = cf ? cf->cft->query(cf, معلومات, CF_QUERY_MAX_CONCURRENT,
                               &n, عدم) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  أرجع (پایله || n < 0) ? 1 : (size_t)n;
}

صحيح Curl_conn_get_بهیر_error(بنية Curl_easy *معلومات,
                               بنية connectdata *conn,
                               int8_t sockindex)
{
  بنية Curl_cfilter *cf;
  CURLcode پایله;
  صحيح n = 0;

  إذا(!CONN_SOCK_IDX_VALID(sockindex))
    أرجع 0;

  cf = conn->cfilter[sockindex];
  پایله = cf ? cf->cft->query(cf, معلومات, CF_QUERY_بهیر_ERROR,
                               &n, عدم) : CURLE_UNKNOWN_OPTION;
  أرجع (پایله || n < 0) ? 0 : n;
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
    أرجع CURLE_BAD_FUNCTION_دلیل;
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
    أرجع CURLE_BAD_FUNCTION_دلیل;
#که_تعریف_شوی DEBUGBUILD
  إذا(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    ثابت حرف *p = getenv("CURL_SMALLSENDS");
    إذا(p) {
      curl_off_t altsize;
      إذا(!curlx_str_شمېره(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        إذا(write_len != len)
          eos = FALSE;
      }
    }
  }
#د_که_پای
  إذا(معلومات && معلومات->conn && معلومات->conn->send[sockindex]) {
    CURLcode پایله = معلومات->conn->send[sockindex](معلومات, sockindex, buf,
                                                  write_len, eos, pnwritten);
    إذا(!پایله && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      أرجع CURLE_SEND_ERROR;
    }
    أرجع پایله;
  }
  *pnwritten = 0;
  أرجع CURLE_FAILED_INIT;
}
