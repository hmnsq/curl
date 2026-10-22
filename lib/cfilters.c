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

CURLcode Curl_cf_def_connect(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ;

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  اگر(cf->اگلا) {
    نتیجہ = cf->اگلا->cft->do_connect(cf->اگلا, ڈیٹا, مکمل);
    اگر(نتیجہ || !*مکمل)
      واپس نتیجہ;
  }

  cf->connected = TRUE;
  *مکمل = TRUE;
  واپس CURLE_OK;
}

CURLcode Curl_cf_def_shutdown(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  (خالی)cf;
  (خالی)ڈیٹا;
  *مکمل = TRUE;
  واپس CURLE_OK;
}

CURLcode Curl_cf_def_adjust_pollset(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت easy_pollset *ps)
{
  /* NOP */
  (خالی)cf;
  (خالی)ڈیٹا;
  (خالی)ps;
  واپس CURLE_OK;
}

bool Curl_cf_def_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                              ثابت ساخت Curl_easy *ڈیٹا)
{
  واپس cf->اگلا ?
    cf->اگلا->cft->has_ڈیٹا_pending(cf->اگلا, ڈیٹا) : FALSE;
}

CURLcode Curl_cf_def_send(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                          ثابت uint8_t *buf, size_t len, bool eos,
                          size_t *pnwritten)
{
  اگر(cf->اگلا)
    واپس cf->اگلا->cft->do_send(cf->اگلا, ڈیٹا, buf, len, eos, pnwritten);
  *pnwritten = 0;
  واپس CURLE_RECV_ERROR;
}

CURLcode Curl_cf_def_recv(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                          حرف *buf, size_t len, size_t *pnread)
{
  اگر(cf->اگلا)
    واپس cf->اگلا->cft->do_recv(cf->اگلا, ڈیٹا, buf, len, pnread);
  *pnread = 0;
  واپس CURLE_SEND_ERROR;
}

bool Curl_cf_def_conn_is_alive(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               bool *داخلہ_pending)
{
  واپس cf->اگلا ?
    cf->اگلا->cft->is_alive(cf->اگلا, ڈیٹا, داخلہ_pending) :
    FALSE; /* pessimistic in absence of data */
}

CURLcode Curl_cf_def_conn_keep_alive(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا)
{
  واپس cf->اگلا ?
    cf->اگلا->cft->keep_alive(cf->اگلا, ڈیٹا) :
    CURLE_OK;
}

CURLcode Curl_cf_def_query(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا,
                           صحیح query, صحیح *pres1, خالی *pres2)
{
  واپس cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, ڈیٹا, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

#اگر_تعریف_شدہ CURLVERBOSE
خالی Curl_conn_trc_filters(ساخت Curl_easy *ڈیٹا,
                           int8_t sockindex, ثابت حرف *info)
{
  اگر(CURL_TRC_M_is_verbose(ڈیٹا) && ڈیٹا->conn) {
    ساخت Curl_cfilter *cf = ڈیٹا->conn->cfilter[sockindex];

    اگر(cf) {
      حرف پیغام[256], *buf;
      صحیح blen, n;

      buf = پیغام;
      blen = حجم(پیغام) - 1;
      n = curl_msnprintf(buf, blen, "%s [%d]", info, sockindex);
      buf += n;
      blen -= n;
      برائے(; cf && blen; cf = cf->اگلا) {
        n = curl_msnprintf(buf, blen, "[%s%s]",
                           cf->connected ? "" : "!", cf->cft->name);
        buf += n;
        blen -= n;
      }
      CURL_TRC_M(ڈیٹا, "%s%s", پیغام, blen ? "" : "...");
    }
    ورنہ
      CURL_TRC_M(ڈیٹا, "%s [%d][-]", info, sockindex);
  }
}
#اگر_ختم /* CURLVERBOSE */

خالی Curl_conn_cf_discard_chain(ساخت Curl_cfilter **pcf,
                                ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cfilter *cfn, *cf = *pcf;

  اگر(cf) {
    *pcf = تہی;
    جب(cf) {
      cfn = cf->اگلا;
      /* prevent destroying filter to mess with its sub-chain, since
       * we have the reference now and will call destroy on it.
       */
      cf->اگلا = تہی;
      cf->cft->destroy(cf, ڈیٹا);
      curlx_free(cf);
      cf = cfn;
    }
  }
}

خالی Curl_conn_cf_discard_all(ساخت Curl_easy *ڈیٹا,
                              ساخت connectdata *conn, int8_t sockindex)
{
  conn->shutdown.آغاز_ms[sockindex] = -1;
  conn->connect_started.سیٹ[sockindex] = FALSE;
  Curl_conn_cf_discard_chain(&conn->cfilter[sockindex], ڈیٹا);
}

CURLcode Curl_cf_recv(ساخت Curl_easy *ڈیٹا, int8_t sockindex, حرف *buf,
                      size_t len, size_t *pnread)
{
  ساخت Curl_cfilter *cf;

  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->conn);
  cf = ڈیٹا->conn->cfilter[sockindex];
  جب(cf && !cf->connected)
    cf = cf->اگلا;
  اگر(cf)
    واپس cf->cft->do_recv(cf, ڈیٹا, buf, len, pnread);
  failf(ڈیٹا, "recv: no filter connected");
  DEBUGASSERT(0);
  *pnread = 0;
  واپس CURLE_FAILED_INIT;
}

CURLcode Curl_cf_send(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                      ثابت uint8_t *buf, size_t len, bool eos,
                      size_t *pnwritten)
{
  ساخت Curl_cfilter *cf;

  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->conn);
  cf = ڈیٹا->conn->cfilter[sockindex];
  جب(cf && !cf->connected)
    cf = cf->اگلا;
  اگر(cf) {
    واپس cf->cft->do_send(cf, ڈیٹا, buf, len, eos, pnwritten);
  }
  failf(ڈیٹا, "send: no filter connected");
  DEBUGASSERT(0);
  *pnwritten = 0;
  واپس CURLE_FAILED_INIT;
}

ساخت cf_io_ctx {
  ساخت Curl_easy *ڈیٹا;
  ساخت Curl_cfilter *cf;
};

ساکن CURLcode cf_bufq_reader(خالی *writer_ctx,
                               غیرعلامتی حرف *buf, size_t blen,
                               size_t *pnread)
{
  ساخت cf_io_ctx *io = writer_ctx;
  واپس Curl_conn_cf_recv(io->cf, io->ڈیٹا, (حرف *)buf, blen, pnread);
}

CURLcode Curl_cf_recv_bufq(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا,
                           ساخت bufq *bufq,
                           size_t maxlen,
                           size_t *pnread)
{
  ساخت cf_io_ctx io;

  اگر(!cf || !ڈیٹا) {
    *pnread = 0;
    واپس CURLE_BAD_FUNCTION_دلیل;
  }
  io.ڈیٹا = ڈیٹا;
  io.cf = cf;
  واپس Curl_bufq_sipn(bufq, maxlen, cf_bufq_reader, &io, pnread);
}

ساکن CURLcode cf_bufq_writer(خالی *writer_ctx,
                               ثابت uint8_t *buf, size_t buflen,
                               size_t *pnwritten)
{
  ساخت cf_io_ctx *io = writer_ctx;
  واپس Curl_conn_cf_send(io->cf, io->ڈیٹا, buf, buflen, FALSE, pnwritten);
}

CURLcode Curl_cf_send_bufq(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا,
                           ساخت bufq *bufq,
                           ثابت غیرعلامتی حرف *buf, size_t blen,
                           size_t *pnwritten)
{
  ساخت cf_io_ctx io;

  اگر(!cf || !ڈیٹا) {
    *pnwritten = 0;
    واپس CURLE_BAD_FUNCTION_دلیل;
  }
  io.ڈیٹا = ڈیٹا;
  io.cf = cf;
  اگر(buf && blen)
    واپس Curl_bufq_write_pass(bufq, buf, blen, cf_bufq_writer, &io,
                                pnwritten);
  ورنہ
    واپس Curl_bufq_pass(bufq, cf_bufq_writer, &io, pnwritten);
}

CURLcode Curl_cf_create(ساخت Curl_cfilter **pcf,
                        ثابت ساخت Curl_cftype *cft,
                        خالی *ctx)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  DEBUGASSERT(cft);
  cf = curlx_calloc(1, حجم(*cf));
  اگر(!cf)
    جاؤ برآمد;

  cf->cft = cft;
  cf->ctx = ctx;
  نتیجہ = CURLE_OK;
برآمد:
  *pcf = cf;
  واپس نتیجہ;
}

خالی Curl_conn_cf_جوڑو(ساخت Curl_easy *ڈیٹا,
                      ساخت connectdata *conn,
                      int8_t sockindex,
                      ساخت Curl_cfilter *cf)
{
  DEBUGASSERT(conn);
  DEBUGASSERT(!cf->conn);
  DEBUGASSERT(!cf->اگلا);

  cf->اگلا = conn->cfilter[sockindex];
  cf->conn = conn;
  cf->sockindex = sockindex;
  conn->cfilter[sockindex] = cf;
  CURL_TRC_CF(ڈیٹا, cf, "added");
}

خالی Curl_conn_cf_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                               ساخت Curl_cfilter *cf_new)
{
  ساخت Curl_cfilter *دم, **pnext;

  DEBUGASSERT(cf_at);
  DEBUGASSERT(cf_new);
  DEBUGASSERT(!cf_new->conn);

  دم = cf_at->اگلا;
  cf_at->اگلا = cf_new;
  کرو {
    cf_new->conn = cf_at->conn;
    cf_new->sockindex = cf_at->sockindex;
    pnext = &cf_new->اگلا;
    cf_new = cf_new->اگلا;
  } جب(cf_new);
  *pnext = دم;
}

bool Curl_conn_cf_discard(ساخت Curl_cfilter **pcf,
                          ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cfilter *cf = pcf ? *pcf : تہی;
  bool ملا = FALSE;
  اگر(cf) {
    اگر(cf->conn) {
      /* unlink if present in connection filter chain */
      ساخت Curl_cfilter **pprev = &cf->conn->cfilter[cf->sockindex];
      جب(*pprev) {
        اگر(*pprev == *pcf) {
          *pprev = (*pcf)->اگلا;
          cf->اگلا = تہی;
          ملا = TRUE;
          توڑو;
        }
        pprev = &(*pprev)->اگلا;
      }
    }
    Curl_conn_cf_discard_chain(pcf, ڈیٹا);
  }
  واپس ملا;
}

CURLcode Curl_conn_cf_connect(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              bool *مکمل)
{
  اگر(cf)
    واپس cf->cft->do_connect(cf, ڈیٹا, مکمل);
  واپس CURLE_FAILED_INIT;
}

CURLcode Curl_conn_cf_send(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                           ثابت uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  اگر(cf)
    واپس cf->cft->do_send(cf, ڈیٹا, buf, len, eos, pnwritten);
  *pnwritten = 0;
  واپس CURLE_SEND_ERROR;
}

CURLcode Curl_conn_cf_recv(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                           حرف *buf, size_t len, size_t *pnread)
{
  اگر(cf)
    واپس cf->cft->do_recv(cf, ڈیٹا, buf, len, pnread);
  *pnread = 0;
  واپس CURLE_RECV_ERROR;
}

ساکن CURLcode cf_cntrl_all(ساخت connectdata *conn,
                             ساخت Curl_easy *ڈیٹا,
                             bool ignore_نتیجہ,
                             صحیح event, صحیح arg1, خالی *arg2)
{
  CURLcode نتیجہ = CURLE_OK;
  صحیح i;

  برائے(i = 0; i < (صحیح)CURL_ARRAYSIZE(conn->cfilter); ++i) {
    نتیجہ = Curl_conn_cf_cntrl(conn->cfilter[i], ڈیٹا, ignore_نتیجہ,
                                event, arg1, arg2);
    اگر(!ignore_نتیجہ && نتیجہ)
      توڑو;
  }
  واپس نتیجہ;
}

bool Curl_conn_cf_is_ip_connected(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا)
{
  اگر(cf) {
    صحیح قدر = 0;
    اگر(!cf->cft->query(cf, ڈیٹا, CF_QUERY_REALLY_CONNECTED, &قدر, تہی))
      واپس !!قدر;
  }
  واپس FALSE;
}

خالی Curl_conn_cntrl_update_info(ساخت Curl_easy *ڈیٹا,
                                ساخت connectdata *conn)
{
  cf_cntrl_all(conn, ڈیٹا, TRUE, CF_CTRL_CONN_INFO_UPDATE, 0, تہی);
}

خالی Curl_conn_cntrl_report_stats(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn,
                                  صحیح sockindex)
{
  اگر((غیرعلامتی)sockindex < CURL_ARRAYSIZE(conn->cfilter))
    (خالی)Curl_conn_cf_cntrl(conn->cfilter[sockindex], ڈیٹا, TRUE,
                             CF_CTRL_REPORT_STATS, 0, تہی);
}

خالی Curl_conn_remove_تیاری_filters(ساخت Curl_easy *ڈیٹا,
                                    int8_t sockindex)
{
  ساخت Curl_cfilter **anchor = &ڈیٹا->conn->cfilter[sockindex];
  جب(*anchor) {
    ساخت Curl_cfilter *cf = *anchor;
    اگر(cf->connected && (cf->cft->flags & CF_TYPE_تیاری)) {
      *anchor = cf->اگلا;
      cf->اگلا = تہی;
      CURL_TRC_CF(ڈیٹا, cf, "removing connected setup filter");
      cf->cft->destroy(cf, ڈیٹا);
      curlx_free(cf);
    }
    ورنہ
      anchor = &cf->اگلا;
  }
}

bool Curl_conn_is_تیاری(ساخت connectdata *conn, int8_t sockindex)
{
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس FALSE;
  واپس !!conn->cfilter[sockindex];
}

bool Curl_conn_is_connected(ساخت connectdata *conn, int8_t sockindex)
{
  ساخت Curl_cfilter *cf;

  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس FALSE;
  cf = conn->cfilter[sockindex];
  اگر(cf)
    واپس (bool)cf->connected;
  ورنہ اگر(conn->scheme->flags & PROTOPT_NONETWORK)
    واپس TRUE;
  واپس FALSE;
}

bool Curl_conn_is_ip_connected(ساخت Curl_easy *ڈیٹا, int8_t sockindex)
{
  ساخت Curl_cfilter *cf;

  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس FALSE;
  cf = ڈیٹا->conn->cfilter[sockindex];
  جب(cf) {
    اگر(cf->connected)
      واپس TRUE;
    اگر(cf->cft->flags & CF_TYPE_IP_CONNECT)
      واپس FALSE;
    cf = cf->اگلا;
  }
  واپس FALSE;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
ساکن bool cf_is_tunneling(ساخت Curl_cfilter *cf)
{
  برائے(; cf; cf = cf->اگلا) {
    اگر((cf->cft->flags & CF_TYPE_PROXY))
      واپس TRUE;
  }
  واپس FALSE;
}

bool Curl_conn_is_tunneling(ساخت connectdata *conn, int8_t sockindex)
{
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس FALSE;
  واپس conn ? cf_is_tunneling(conn->cfilter[sockindex]) : FALSE;
}
#ورنہ
bool Curl_conn_is_tunneling(ساخت connectdata *conn, int8_t sockindex)
{
  (خالی)conn;
  (خالی)sockindex;
  واپس FALSE;
}
#اگر_ختم /* CURL_DISABLE_PROXY */

ساکن bool cf_is_ssl(ساخت Curl_cfilter *cf)
{
  برائے(; cf; cf = cf->اگلا) {
    /* A tunneling proxy does not offer end2end encryption, even if
     * it does SSL itself (e.g. QUIC H3 proxy) */
    اگر((cf->cft->flags & CF_TYPE_SSL) && !(cf->cft->flags & CF_TYPE_PROXY))
      واپس TRUE;
    اگر(cf->cft->flags & CF_TYPE_IP_CONNECT)
      واپس FALSE;
  }
  واپس FALSE;
}

bool Curl_conn_is_ssl(ساخت connectdata *conn, int8_t sockindex)
{
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس FALSE;
  واپس conn ? cf_is_ssl(conn->cfilter[sockindex]) : FALSE;
}

bool Curl_conn_get_ssl_info(ساخت Curl_easy *ڈیٹا,
                            ساخت connectdata *conn, int8_t sockindex,
                            صحیح query,
                            ساخت curl_tlssessioninfo *info)
{
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس FALSE;
  اگر(Curl_conn_is_ssl(conn, sockindex)) {
    ساخت Curl_cfilter *cf = conn->cfilter[sockindex];
    CURLcode نتیجہ = cf ?
      cf->cft->query(cf, ڈیٹا, query, تہی, (خالی *)info) :
      CURLE_UNKNOWN_OPTION;
    واپس !نتیجہ;
  }
  واپس FALSE;
}

CURLcode Curl_conn_get_ip_info(ساخت Curl_easy *ڈیٹا,
                               ساخت connectdata *conn, int8_t sockindex,
                               bool *is_ipv6, ساخت ip_quadruple *ipquad)
{
  ساخت Curl_cfilter *cf;
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس CURLE_BAD_FUNCTION_دلیل;
  cf = conn ? conn->cfilter[sockindex] : تہی;
  واپس Curl_conn_cf_get_ip_info(cf, ڈیٹا, is_ipv6, ipquad);
}

bool Curl_conn_is_multiplex(ساخت connectdata *conn, int8_t sockindex)
{
  ساخت Curl_cfilter *cf;

  اگر(!conn || !CONN_SOCK_IDX_VALID(sockindex))
    واپس FALSE;
  برائے(cf = conn->cfilter[sockindex]; cf; cf = cf->اگلا) {
    اگر(cf->cft->flags & CF_TYPE_MULTIPLEX)
      واپس TRUE;
    اگر(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      واپس FALSE;
  }
  واپس FALSE;
}

غیرعلامتی حرف Curl_conn_get_transport(ساخت Curl_easy *ڈیٹا,
                                      ساخت connectdata *conn)
{
  ساخت Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  واپس Curl_conn_cf_get_transport(cf, ڈیٹا);
}

صحیح Curl_socktype_for_transport(uint8_t transport)
{
  انتخاب(transport) {
  حالت TRNSPRT_TCP:
    واپس SOCK_STREAM;
  حالت TRNSPRT_UNIX:
    واپس SOCK_STREAM;
  طےشدہ: /* UDP and QUIC */
    واپس SOCK_DGRAM;
  }
}

صحیح Curl_protocol_for_transport(uint8_t transport)
{
  انتخاب(transport) {
  حالت TRNSPRT_TCP:
    واپس IPPROTO_TCP;
  حالت TRNSPRT_UNIX:
    واپس IPPROTO_IP;
  طےشدہ: /* UDP and QUIC */
    واپس IPPROTO_UDP;
  }
}

ثابت حرف *Curl_conn_get_alpn_negotiated(ساخت Curl_easy *ڈیٹا,
                                          ساخت connectdata *conn)
{
  ساخت Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  واپس Curl_conn_cf_get_alpn_negotiated(cf, ڈیٹا);
}

غیرعلامتی حرف Curl_conn_http_نسخہ(ساخت Curl_easy *ڈیٹا,
                                     ساخت connectdata *conn)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ = CURLE_UNKNOWN_OPTION;
  غیرعلامتی حرف v = 0;

  cf = conn->cfilter[FIRSTSOCKET];
  برائے(; cf; cf = cf->اگلا) {
    اگر(cf->cft->flags & CF_TYPE_HTTP) {
      صحیح قدر = 0;
      نتیجہ = cf->cft->query(cf, ڈیٹا, CF_QUERY_HTTP_نسخہ, &قدر, تہی);
      اگر(!نتیجہ && ((قدر < 0) || (قدر > 255)))
        نتیجہ = CURLE_FAILED_INIT;
      ورنہ
        v = (غیرعلامتی حرف)قدر;
      توڑو;
    }
    اگر(cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_SSL))
      توڑو;
  }
  واپس (غیرعلامتی حرف)(نتیجہ ? 0 : v);
}

bool Curl_conn_ڈیٹا_pending(ساخت Curl_easy *ڈیٹا, int8_t sockindex)
{
  ساخت Curl_cfilter *cf;

  (خالی)ڈیٹا;
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->conn);
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس FALSE;

  cf = ڈیٹا->conn->cfilter[sockindex];
  جب(cf && !cf->connected) {
    cf = cf->اگلا;
  }
  اگر(cf) {
    واپس cf->cft->has_ڈیٹا_pending(cf, ڈیٹا);
  }
  واپس FALSE;
}

bool Curl_conn_cf_needs_flush(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ;
  صحیح pending = 0;
  نتیجہ = cf ? cf->cft->query(cf, ڈیٹا, CF_QUERY_NEED_FLUSH,
                               &pending, تہی) : CURLE_UNKNOWN_OPTION;
  واپس (نتیجہ || !pending) ? FALSE : TRUE;
}

bool Curl_conn_needs_flush(ساخت Curl_easy *ڈیٹا, int8_t sockindex)
{
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس FALSE;
  واپس Curl_conn_cf_needs_flush(ڈیٹا->conn->cfilter[sockindex], ڈیٹا);
}

CURLcode Curl_conn_cf_adjust_pollset(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا,
                                     ساخت easy_pollset *ps)
{
  CURLcode نتیجہ = CURLE_OK;
  /* Go through all filters, top to bottom, and let them manage the pollset
   * - connected filters can do so
   * - CF_TYPE_DNS filters can
   * - unconnected filters without next or connect next can
   */
  برائے(; cf && !نتیجہ; cf = cf->اگلا) {
    اگر(cf->shutdown)
      جاری;
    اگر(cf->connected || (cf->cft->flags & CF_TYPE_DNS) ||
       !cf->اگلا || cf->اگلا->connected)
      نتیجہ = cf->cft->adjust_pollset(cf, ڈیٹا, ps);
  }
  واپس نتیجہ;
}

CURLcode Curl_conn_adjust_pollset(ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn,
                                  ساخت easy_pollset *ps)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت bool want_io = !!ps->n;
  int8_t i;

  DEBUGASSERT(ڈیٹا);
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
  برائے(i = 0; (i < (صحیح)CURL_ARRAYSIZE(conn->cfilter)) && !نتیجہ; ++i) {
    اگر(conn->cfilter[i] &&
       (want_io || !conn->cfilter[i]->connected ||
        CURL_CONN_اندر_SHUTDOWN(conn, i)))
      نتیجہ = Curl_conn_cf_adjust_pollset(conn->cfilter[i], ڈیٹا, ps);
  }
  واپس نتیجہ;
}

/*
 * Return values:
 *   -1 = error
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
صحیح Curl_conn_cf_poll(ساخت Curl_cfilter *cf,
                      ساخت Curl_easy *ڈیٹا,
                      timediff_t timeout_ms)
{
  ساخت easy_pollset ps;
  صحیح rc;
  CURLcode نتیجہ;

  DEBUGASSERT(cf);
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->conn);
  Curl_pollset_init(&ps);

  نتیجہ = Curl_conn_cf_adjust_pollset(cf, ڈیٹا, &ps);
  اگر(!نتیجہ)
    rc = Curl_pollset_poll(ڈیٹا, &ps, timeout_ms);
  ورنہ
    rc = -1;
  Curl_pollset_cleanup(&ps);
  واپس rc;
}

خالی Curl_conn_get_current_host(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                                ثابت حرف **phost, صحیح *pport)
{
  ساخت Curl_cfilter *cf, *cf_proxy = تہی;
  صحیح portarg = -1;

  اگر(!ڈیٹا->conn) {
    DEBUGASSERT(0);
    *phost = "";
    اگر(pport)
      *pport = -1;
    واپس;
  }

  cf = CONN_SOCK_IDX_VALID(sockindex) ? ڈیٹا->conn->cfilter[sockindex] : تہی;
  /* Find the "lowest" tunneling proxy filter that has not connected yet. */
  جب(cf && !cf->connected) {
    اگر((cf->cft->flags & (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY)) ==
       (CF_TYPE_IP_CONNECT | CF_TYPE_PROXY))
      cf_proxy = cf;
    cf = cf->اگلا;
  }
  /* cf_proxy (!= NULL) is not connected yet. It is talking
   * to an interim host and any authentication or other things apply
   * to this interim host and port. */
  اگر(!cf_proxy || cf_proxy->cft->query(cf_proxy, ڈیٹا, CF_QUERY_HOST_پورٹ,
                                       &portarg, CURL_UNCONST(phost))) {
    /* Everything connected or query unsuccessful, the overall
     * connection's destination is the answer */
    *phost = ڈیٹا->conn->origin->hostname;
    portarg = ڈیٹا->conn->origin->پورٹ;
  }
  اگر(pport)
    *pport = portarg;
}

CURLcode Curl_cf_def_cntrl(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا,
                           صحیح event, صحیح arg1, خالی *arg2)
{
  (خالی)cf;
  (خالی)ڈیٹا;
  (خالی)event;
  (خالی)arg1;
  (خالی)arg2;
  واپس CURLE_OK;
}

CURLcode Curl_conn_cf_cntrl(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            bool ignore_نتیجہ,
                            صحیح event, صحیح arg1, خالی *arg2)
{
  CURLcode نتیجہ = CURLE_OK;

  برائے(; cf; cf = cf->اگلا) {
    اگر(cf->cft->cntrl == Curl_cf_def_cntrl)
      جاری;
    نتیجہ = cf->cft->cntrl(cf, ڈیٹا, event, arg1, arg2);
    اگر(!ignore_نتیجہ && نتیجہ)
      توڑو;
  }
  واپس نتیجہ;
}

curl_socket_t Curl_conn_cf_get_socket(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا)
{
  curl_socket_t sock;
  اگر(cf && !cf->cft->query(cf, ڈیٹا, CF_QUERY_SOCKET, تہی, &sock))
    واپس sock;
  واپس CURL_SOCKET_BAD;
}

غیرعلامتی حرف Curl_conn_cf_get_transport(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا)
{
  صحیح transport = 0;
  اگر(cf && !cf->cft->query(cf, ڈیٹا, CF_QUERY_TRANSPORT, &transport, تہی))
    واپس (غیرعلامتی حرف)transport;
  واپس (غیرعلامتی حرف)(ڈیٹا->conn ? ڈیٹا->conn->transport_wanted : 0);
}

ثابت حرف *Curl_conn_cf_get_alpn_negotiated(ساخت Curl_cfilter *cf,
                                             ساخت Curl_easy *ڈیٹا)
{
  ثابت حرف *alpn = تہی;
  CURL_TRC_CF(ڈیٹا, cf, "query ALPN");
  اگر(cf && !cf->cft->query(cf, ڈیٹا, CF_QUERY_ALPN_NEGOTIATED, تہی,
                           CURL_UNCONST(&alpn)))
    واپس alpn;
  واپس تہی;
}

ساکن ثابت ساخت Curl_sockaddr_ex *cf_get_remote_addr(
  ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ثابت ساخت Curl_sockaddr_ex *remote_addr = تہی;
  اگر(cf &&
     !cf->cft->query(cf, ڈیٹا, CF_QUERY_REMOTE_ADDR, تہی,
                     CURL_UNCONST(&remote_addr)))
    واپس remote_addr;
  واپس تہی;
}

CURLcode Curl_conn_cf_get_ip_info(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  bool *is_ipv6, ساخت ip_quadruple *ipquad)
{
  CURLcode نتیجہ = CURLE_UNKNOWN_OPTION;
  اگر(cf) {
    صحیح ipv6 = 0;
    نتیجہ = cf->cft->query(cf, ڈیٹا, CF_QUERY_IP_INFO, &ipv6, ipquad);
    *is_ipv6 = !!ipv6;
  }
  واپس نتیجہ;
}

curl_socket_t Curl_conn_get_پہلا_socket(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cfilter *cf;

  اگر(!ڈیٹا->conn)
    واپس CURL_SOCKET_BAD;

  cf = ڈیٹا->conn->cfilter[FIRSTSOCKET];
  /* if the top filter has not connected, ask it (and its sub-filters)
   * for the socket. Otherwise conn->sock[sockindex] should have it. */
  اگر(cf && !cf->connected)
    واپس Curl_conn_cf_get_socket(cf, ڈیٹا);
  واپس ڈیٹا->conn->sock[FIRSTSOCKET];
}

ثابت ساخت Curl_sockaddr_ex *Curl_conn_get_remote_addr(
  ساخت Curl_easy *ڈیٹا, int8_t sockindex)
{
  ساخت Curl_cfilter *cf =
    (ڈیٹا->conn && CONN_SOCK_IDX_VALID(sockindex)) ?
    ڈیٹا->conn->cfilter[sockindex] : تہی;
  واپس cf ? cf_get_remote_addr(cf, ڈیٹا) : تہی;
}

CURLcode Curl_conn_ev_ڈیٹا_تیاری(ساخت Curl_easy *ڈیٹا)
{
  واپس cf_cntrl_all(ڈیٹا->conn, ڈیٹا, FALSE, CF_CTRL_ڈیٹا_تیاری, 0, تہی);
}

CURLcode Curl_conn_flush(ساخت Curl_easy *ڈیٹا, int8_t sockindex)
{
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس CURLE_BAD_FUNCTION_دلیل;
  واپس Curl_conn_cf_cntrl(ڈیٹا->conn->cfilter[sockindex], ڈیٹا, FALSE,
                            CF_CTRL_FLUSH, 0, تہی);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is done with sending data (e.g. has uploaded everything).
 */
خالی Curl_conn_ev_ڈیٹا_مکمل_send(ساخت Curl_easy *ڈیٹا)
{
  cf_cntrl_all(ڈیٹا->conn, ڈیٹا, TRUE, CF_CTRL_ڈیٹا_مکمل_SEND, 0, تہی);
}

/**
 * Notify connection filters that the transfer represented by `data`
 * is finished - eventually premature, e.g. before being complete.
 */
خالی Curl_conn_ev_ڈیٹا_مکمل(ساخت Curl_easy *ڈیٹا, bool premature)
{
  cf_cntrl_all(ڈیٹا->conn, ڈیٹا, TRUE, CF_CTRL_ڈیٹا_مکمل, premature, تہی);
}

CURLcode Curl_conn_ev_ڈیٹا_pause(ساخت Curl_easy *ڈیٹا, bool do_pause)
{
  واپس cf_cntrl_all(ڈیٹا->conn, ڈیٹا, FALSE,
                      CF_CTRL_ڈیٹا_PAUSE, do_pause, تہی);
}

bool Curl_conn_is_alive(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn,
                        bool *داخلہ_pending)
{
  ساخت Curl_cfilter *cf = conn->cfilter[FIRSTSOCKET];
  واپس cf && !cf->conn->بٹ.close &&
         cf->cft->is_alive(cf, ڈیٹا, داخلہ_pending);
}

CURLcode Curl_conn_keep_alive(ساخت Curl_easy *ڈیٹا,
                              ساخت connectdata *conn)
{
  CURLcode نتیجہ = CURLE_OK;
  صحیح i;

  برائے(i = 0; (i < (صحیح)CURL_ARRAYSIZE(conn->cfilter)) && !نتیجہ; ++i) {
    ساخت Curl_cfilter *cf = conn->cfilter[i];
    اگر(cf)
      نتیجہ = cf->cft->keep_alive(cf, ڈیٹا);
  }
  واپس نتیجہ;
}

size_t Curl_conn_get_max_concurrent(ساخت Curl_easy *ڈیٹا,
                                    ساخت connectdata *conn,
                                    int8_t sockindex)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ;
  صحیح n = -1;

  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس 0;

  cf = conn->cfilter[sockindex];
  نتیجہ = cf ? cf->cft->query(cf, ڈیٹا, CF_QUERY_MAX_CONCURRENT,
                               &n, تہی) : CURLE_UNKNOWN_OPTION;
  /* If no filter answered the query, the default is a non-multiplexed
   * connection with limit 1. Otherwise, the query may return 0
   * for connections that are in shutdown, e.g. server HTTP/2 GOAWAY. */
  واپس (نتیجہ || n < 0) ? 1 : (size_t)n;
}

صحیح Curl_conn_get_دھارا_error(ساخت Curl_easy *ڈیٹا,
                               ساخت connectdata *conn,
                               int8_t sockindex)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ;
  صحیح n = 0;

  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس 0;

  cf = conn->cfilter[sockindex];
  نتیجہ = cf ? cf->cft->query(cf, ڈیٹا, CF_QUERY_دھارا_ERROR,
                               &n, تہی) : CURLE_UNKNOWN_OPTION;
  واپس (نتیجہ || n < 0) ? 0 : n;
}

int8_t Curl_conn_sockindex(ساخت Curl_easy *ڈیٹا, curl_socket_t sockfd)
{
  اگر(ڈیٹا && ڈیٹا->conn &&
     sockfd != CURL_SOCKET_BAD && sockfd == ڈیٹا->conn->sock[SECONDARYSOCKET])
    واپس SECONDARYSOCKET;
  واپس FIRSTSOCKET;
}

CURLcode Curl_conn_recv(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                        حرف *buf, size_t len, size_t *pnread)
{
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->conn);
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس CURLE_BAD_FUNCTION_دلیل;
  اگر(ڈیٹا && ڈیٹا->conn && ڈیٹا->conn->recv[sockindex])
    واپس ڈیٹا->conn->recv[sockindex](ڈیٹا, sockindex, buf, len, pnread);
  *pnread = 0;
  واپس CURLE_FAILED_INIT;
}

CURLcode Curl_conn_send(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                        ثابت خالی *buf, size_t len, bool eos,
                        size_t *pnwritten)
{
  size_t write_len = len;

  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(ڈیٹا->conn);
  DEBUGASSERT(CONN_SOCK_IDX_VALID(sockindex));
  اگر(!CONN_SOCK_IDX_VALID(sockindex))
    واپس CURLE_BAD_FUNCTION_دلیل;
#اگر_تعریف_شدہ DEBUGBUILD
  اگر(write_len) {
    /* Allow debug builds to override this logic to force short sends */
    ثابت حرف *p = getenv("CURL_SMALLSENDS");
    اگر(p) {
      curl_off_t altsize;
      اگر(!curlx_str_نمبر(&p, &altsize, write_len)) {
        write_len = (size_t)altsize;
        اگر(write_len != len)
          eos = FALSE;
      }
    }
  }
#اگر_ختم
  اگر(ڈیٹا && ڈیٹا->conn && ڈیٹا->conn->send[sockindex]) {
    CURLcode نتیجہ = ڈیٹا->conn->send[sockindex](ڈیٹا, sockindex, buf,
                                                  write_len, eos, pnwritten);
    اگر(!نتیجہ && (*pnwritten > write_len)) {
      DEBUGASSERT(0);
      *pnwritten = 0;
      واپس CURLE_SEND_ERROR;
    }
    واپس نتیجہ;
  }
  *pnwritten = 0;
  واپس CURLE_FAILED_INIT;
}
