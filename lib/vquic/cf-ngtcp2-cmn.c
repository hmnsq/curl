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

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#شامل <ngtcp2/ngtcp2.h>

#اگر_تعریف_شدہ USE_OPENSSL
#شامل <openssl/err.h>
#اگر defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#شامل <ngtcp2/ngtcp2_crypto_boringssl.h>
#وگرنہ_اگر defined(OPENSSL_QUIC_API2)
#شامل <ngtcp2/ngtcp2_crypto_ossl.h>
#ورنہ
#شامل <ngtcp2/ngtcp2_crypto_quictls.h>
#اگر_ختم
#شامل "vtls/openssl.h"
#وگرنہ_اگر defined(USE_GNUTLS)
#شامل <ngtcp2/ngtcp2_crypto_gnutls.h>
#شامل "vtls/gtls.h"
#وگرنہ_اگر defined(USE_WOLFSSL)
#شامل <ngtcp2/ngtcp2_crypto_wolfssl.h>
#شامل "vtls/wolfssl.h"
#اگر_ختم

#شامل <nghttp3/nghttp3.h>

#شامل "urldata.h"
#شامل "url.h"
#شامل "u32_ptrset.h"
#شامل "curl_trc.h"
#شامل "rand.h"
#شامل "multiif.h"
#شامل "cfilters.h"
#شامل "cf-socket.h"
#شامل "connect.h"
#شامل "progress.h"
#شامل "curlx/win32-fopen.h"
#شامل "curlx/dynbuf.h"
#شامل "http1.h"
#شامل "select.h"
#شامل "sockaddr.h"
#شامل "transfer.h"
#شامل "bufref.h"
#شامل "vdns/cf-dns.h"
#شامل "vquic/vquic.h"
#شامل "vquic/vquic_int.h"
#شامل "vquic/vquic-tls.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_scache.h"
#شامل "vquic/cf-ngtcp2-cmn.h"

/*
 * Store ngtcp2 version info in this buffer.
 */
خالی Curl_ngtcp2_ver(حرف *p, size_t len)
{
  ثابت ngtcp2_info *ng2 = ngtcp2_نسخہ(0);
  ثابت nghttp3_info *ht3 = nghttp3_نسخہ(0);
  (خالی)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->نسخہ_str, ht3->نسخہ_str);
}

خالی Curl_cf_ngtcp2_h3_دھارا_ctx_free(ساخت h3_دھارا_ctx *دھارا)
{
  Curl_bufq_free(&دھارا->sendbuf);
  Curl_h1_req_parse_free(&دھارا->h1);
  curlx_free(دھارا);
}

ساکن خالی h3_دھارا_سیٹ_free(uint32_t id, خالی *دھارا)
{
  (خالی)id;
  DEBUGASSERT(دھارا);
  Curl_cf_ngtcp2_h3_دھارا_ctx_free((ساخت h3_دھارا_ctx *)دھارا);
}

ساکن bool cf_ngtcp2_h3_err_is_fatal(صحیح رمز)
{
  واپس (NGHTTP3_ERR_FATAL >= رمز) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_دھارا == رمز);
}

خالی Curl_cf_ngtcp2_h3_err_سیٹ(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا, صحیح رمز)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  اگر(!ctx->آخری_error.error_رمز) {
    ngtcp2_ccerr_سیٹ_application_error(&ctx->آخری_error,
      nghttp3_err_infer_quic_app_error_رمز(رمز), تہی, 0);
  }
  اگر(cf_ngtcp2_h3_err_is_fatal(رمز))
    Curl_cf_ngtcp2_cmn_conn_close(cf, ڈیٹا);
}

CURLcode Curl_cf_ngtcp2_ctx_init(ساخت cf_ngtcp2_ctx *ctx,
                                 ساخت Curl_peer *origin,
                                 ساخت Curl_peer *peer,
                                 ساخت ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = تہی;
  ctx->tunnel_inbuf_len = 0;
  ctx->نسخہ_2 = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->دھارا_bufcp, H3_دھارا_CHUNK_سائز,
                  H3_دھارا_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_سرخی);
  Curl_u32_ptrset_init(&ctx->streams, h3_دھارا_سیٹ_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  واپس Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

خالی Curl_cf_ngtcp2_ctx_cleanup(ساخت cf_ngtcp2_ctx *ctx)
{
  اگر(ctx && ctx->initialized) {
    اگر(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    اگر(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = تہی;
    }
    اگر(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = تہی;
    }
#اگر_تعریف_شدہ OPENSSL_QUIC_API2
    اگر(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = تہی;
    }
#اگر_ختم
    Curl_bufcp_free(&ctx->دھارا_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    curlx_safefree(ctx->tunnel_inbuf);
    ctx->tunnel_inbuf_len = 0;
  }
}

ساخت Curl_easy *Curl_cf_ngtcp2_get_xfer(ساخت Curl_cfilter *cf,
                                          خالی *دھارا_user_ڈیٹا,
                                          ساخت h3_دھارا_ctx **pstream)
{
  ساخت h3_دھارا_ctx *دھارا = دھارا_user_ڈیٹا;

  اگر(pstream)
    *pstream = دھارا;
  اگر(دھارا && (دھارا->mid != UINT32_MAX)) {
    ساخت Curl_easy *ڈیٹا, *call_ڈیٹا = CF_ڈیٹا_CURRENT(cf);
    اگر(!call_ڈیٹا || !call_ڈیٹا->multi) {
      DEBUGASSERT(0);
      واپس تہی;
    }
    ڈیٹا = Curl_multi_get_easy(call_ڈیٹا->multi, دھارا->mid);
    اگر(ڈیٹا && (ڈیٹا->id == دھارا->xfer_id))
      واپس ڈیٹا;
    DEBUGASSERT(0);
  }
  واپس تہی;
}

ساکن ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  ساخت Curl_cfilter *cf = conn_ref->user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  واپس ctx->qconn;
}

#اگر_تعریف_شدہ DEBUG_NGTCP2
ساکن خالی quic_printf(خالی *user_ڈیٹا, ثابت حرف *fmt, ...)
{
  va_list ap;
  (خالی)user_ڈیٹا;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#اگر_ختم

ساکن خالی qlog_callback(خالی *user_ڈیٹا, uint32_t flags,
                          ثابت خالی *ڈیٹا, size_t datalen)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  (خالی)flags;
  اگر(ctx->qlogfd != -1) {
    ssize_t rc = اكتب(ctx->qlogfd, ڈیٹا, datalen);
    اگر(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

ساکن خالی quic_settings(ساخت cf_ngtcp2_ctx *ctx,
                          ساخت Curl_easy *ڈیٹا,
                          ساخت cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#اگر_تعریف_شدہ DEBUG_NGTCP2
  s->log_printf = quic_printf;
#ورنہ
  s->log_printf = تہی;
#اگر_ختم

  s->initial_ts = pktx->ts;
  s->handshake_timeout = (ڈیٹا->سیٹ.connecttimeout > 0) ?
    ڈیٹا->سیٹ.connecttimeout * NGTCP2_MILLISECONDS : QUIC_HANDSHAKE_TIMEOUT;
  s->max_کھڑکی = H3_CONN_کھڑکی_سائز_MAX;
  s->max_دھارا_کھڑکی = 0; /* disable ngtcp2 auto-tuning of window */
  s->no_pmtud = FALSE;
#اگر_تعریف_شدہ NGTCP2_SETTINGS_V3
  /* try ten times the ngtcp2 defaults here for problems with Caddy */
  s->glitch_ratelim_مدت = 1000 * 10;
  s->glitch_ratelim_شرح = 33 * 10;
#اگر_ختم
  t->initial_max_ڈیٹا = s->max_کھڑکی;
  t->initial_max_دھارا_ڈیٹا_bidi_local = H3_دھارا_کھڑکی_سائز_INITIAL;
  t->initial_max_دھارا_ڈیٹا_bidi_remote = H3_دھارا_کھڑکی_سائز_INITIAL;
  t->initial_max_دھارا_ڈیٹا_uni = t->initial_max_ڈیٹا;
  t->initial_max_streams_bidi = QUIC_MAX_STREAMS;
  t->initial_max_streams_uni = QUIC_MAX_STREAMS;
  t->max_idle_timeout = 0; /* no idle timeout from our side */
  اگر(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#اگر defined(_MSC_VER) && defined(_DLL)
#ہدایت warning(ڈالو)
#ہدایت warning(disable:4232) /* MSVC extension, dllimport identity */
#اگر_ختم

ساکن صحیح cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, خالی *user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf ? cf->ctx : تہی;
  ساخت Curl_easy *ڈیٹا;

  (خالی)tconn;
  DEBUGASSERT(ctx);
  ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  DEBUGASSERT(ڈیٹا);
  اگر(!ctx || !ڈیٹا)
    واپس NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(ڈیٹا);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, ڈیٹا);

  ctx->tls_vrfy_نتیجہ = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    ڈیٹا, &ctx->ssl_peer);
  اگر(ctx->tls_vrfy_نتیجہ)
    واپس NGTCP2_ERR_CALLBACK_FAILURE;

#اگر_تعریف_شدہ CURLVERBOSE
  اگر(Curl_trc_is_verbose(ڈیٹا)) {
    ثابت ngtcp2_transport_params *rp;
    rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
    CURL_TRC_CF(ڈیٹا, cf, "handshake complete after %" FMT_TIMEDIFF_T
                "ms, remote transport[max_udp_payload=%" PRIu64
                ", initial_max_data=%" PRIu64 "]",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at),
                rp->max_udp_payload_سائز, rp->initial_max_ڈیٹا);
  }
#اگر_ختم

  /* In case of earlydata, where we simulate being connected, update
   * the handshake time when we really did connect */
  اگر(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrs_وقت_Was(ڈیٹا, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  اگر(ctx->use_earlydata) {
#اگر defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    اگر(SSL_get_early_ڈیٹا_حالت(ctx->tls.ossl.ssl) == SSL_EARLY_ڈیٹا_NOT_بھیجا)
      CURL_TRC_CF(ڈیٹا, cf, "OpenSSL did not send early data");
#اگر_ختم

#اگر NGTCP2_نسخہ_عدد >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_ڈیٹا_rejected2(ctx->qconn);
#ورنہ /* older NGTCP2 */
    {
#اگر defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      صحیح ossl_early_حالت = SSL_get_early_ڈیٹا_حالت(ctx->tls.ossl.ssl);
      اگر(ossl_early_حالت == SSL_EARLY_ڈیٹا_NOT_بھیجا)
        CURL_TRC_CF(ڈیٹا, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_حالت == SSL_EARLY_ڈیٹا_ACCEPTED);
#وگرنہ_اگر defined(USE_GNUTLS)
      صحیح flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_ڈیٹا);
#وگرنہ_اگر defined(USE_WOLFSSL)
#اگر_تعریف_شدہ WOLFSSL_EARLY_ڈیٹا
      ctx->earlydata_accepted =
        (wolf_SSL_get_early_ڈیٹا_حالت(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_ڈیٹا_REJECTED);
#ورنہ
      DEBUGASSERT(0); /* should not come here if ED is disabled. */
      ctx->earlydata_accepted = FALSE;
#اگر_ختم /* WOLFSSL_EARLY_DATA */
#اگر_ختم /* OPENSSL or GNUTLS or WOLFSSL */
    }
#اگر_ختم /* older NGTCP2 */
    CURL_TRC_CF(ڈیٹا, cf, "server did%s accept %zu bytes of early data",
                ctx->earlydata_accepted ? "" : " not", ctx->earlydata_skip);
    Curl_pgrs_Early_ڈیٹا(ڈیٹا, ctx->earlydata_accepted ?
                              (curl_off_t)ctx->earlydata_skip :
                             -(curl_off_t)ctx->earlydata_skip);
  }
  واپس 0;
}

ساکن صحیح cb_recv_دھارا_ڈیٹا(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t دھارا_id, uint64_t انحراف,
                               ثابت uint8_t *buf, size_t buflen,
                               خالی *user_ڈیٹا, خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  صحیح fin = (flags & NGTCP2_دھارا_ڈیٹا_FLAG_FIN) ? 1 : 0;
  ساخت h3_دھارا_ctx *دھارا;
  ساخت Curl_easy *ڈیٹا =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_ڈیٹا, &دھارا);

  (خالی)انحراف;
  rc = nghttp3_conn_read_دھارا(ctx->h3conn, دھارا_id, buf, buflen, fin);
  اگر(rc < 0) {
    اگر(ڈیٹا && دھارا) {
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  دھارا_id, دھارا->reset, دھارا->closed);
    }
    واپس NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  اگر(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_دھارا_انحراف(tconn, دھارا_id, nconsumed);
    ngtcp2_conn_extend_max_انحراف(tconn, nconsumed);
    اگر(دھارا) {
      دھارا->rx_انحراف += nconsumed;
      دھارا->rx_انحراف_max += nconsumed;
    }
  }
  واپس 0;
}

ساکن صحیح cb_acked_دھارا_ڈیٹا_انحراف(ngtcp2_conn *tconn, int64_t دھارا_id,
                                       uint64_t انحراف, uint64_t datalen,
                                       خالی *user_ڈیٹا, خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  صحیح rv;
  (خالی)دھارا_id;
  (خالی)tconn;
  (خالی)انحراف;
  (خالی)datalen;
  (خالی)دھارا_user_ڈیٹا;

  rv = nghttp3_conn_جوڑو_ack_انحراف(ctx->h3conn, دھارا_id, datalen);
  اگر(rv && rv != NGHTTP3_ERR_دھارا_NOT_ملا) {
    واپس NGTCP2_ERR_CALLBACK_FAILURE;
  }

  واپس 0;
}

ساکن صحیح cb_دھارا_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t دھارا_id, uint64_t app_error_رمز,
                           خالی *user_ڈیٹا, خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_ڈیٹا, تہی);
  صحیح rv;

  (خالی)tconn;
  /* stream is closed... */
  اگر(!ڈیٹا)
    ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  اگر(!ڈیٹا)
    واپس NGTCP2_ERR_CALLBACK_FAILURE;

  اگر(!(flags & NGTCP2_دھارا_CLOSE_FLAG_APP_ERROR_رمز_سیٹ)) {
    app_error_رمز = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_دھارا(ctx->h3conn, دھارا_id, app_error_رمز);
  CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", دھارا_id, app_error_رمز, rv);
  اگر(rv && rv != NGHTTP3_ERR_دھارا_NOT_ملا) {
    Curl_cf_ngtcp2_h3_err_سیٹ(cf, ڈیٹا, rv);
    واپس NGTCP2_ERR_CALLBACK_FAILURE;
  }

  واپس 0;
}

#اگر_تعریف_شدہ NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
ساکن صحیح cb_دھارا_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t دھارا_id,
                           uint64_t rx_app_error_رمز,
                           uint64_t tx_app_error_رمز,
                           خالی *user_ڈیٹا, خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_ڈیٹا, تہی);
  uint64_t h3_app_error_رمز = NGHTTP3_H3_NO_ERROR;
  صحیح rv;

  (خالی)tconn;
  (خالی)tx_app_error_رمز;
  /* stream is closed... */
  اگر(!ڈیٹا)
    ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  اگر(!ڈیٹا)
    واپس NGTCP2_ERR_CALLBACK_FAILURE;

  اگر(flags & NGTCP2_دھارا_CLOSE2_FLAG_RX_APP_ERROR_رمز_سیٹ)
    h3_app_error_رمز = rx_app_error_رمز;

  rv = nghttp3_conn_close_دھارا(ctx->h3conn, دھارا_id, h3_app_error_رمز);
  CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", دھارا_id, h3_app_error_رمز, rv);
  اگر(rv && rv != NGHTTP3_ERR_دھارا_NOT_ملا) {
    Curl_cf_ngtcp2_h3_err_سیٹ(cf, ڈیٹا, rv);
    واپس NGTCP2_ERR_CALLBACK_FAILURE;
  }

  واپس 0;
}
#اگر_ختم

ساکن صحیح cb_دھارا_reset(ngtcp2_conn *tconn, int64_t دھارا_id,
                           uint64_t final_سائز, uint64_t app_error_رمز,
                           خالی *user_ڈیٹا, خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_ڈیٹا, تہی);
  صحیح rv;
  (خالی)tconn;
  (خالی)final_سائز;
  (خالی)app_error_رمز;

  rv = nghttp3_conn_shutdown_دھارا_read(ctx->h3conn, دھارا_id);
  اگر(ڈیٹا)
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] reset -> %d", دھارا_id, rv);
  اگر(rv && rv != NGHTTP3_ERR_دھارا_NOT_ملا) {
    واپس NGTCP2_ERR_CALLBACK_FAILURE;
  }

  واپس 0;
}

ساکن صحیح cb_دھارا_stop_sending(ngtcp2_conn *tconn, int64_t دھارا_id,
                                  uint64_t app_error_رمز, خالی *user_ڈیٹا,
                                  خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  صحیح rv;
  (خالی)tconn;
  (خالی)app_error_رمز;
  (خالی)دھارا_user_ڈیٹا;

  rv = nghttp3_conn_shutdown_دھارا_read(ctx->h3conn, دھارا_id);
  اگر(rv && rv != NGHTTP3_ERR_دھارا_NOT_ملا) {
    واپس NGTCP2_ERR_CALLBACK_FAILURE;
  }

  واپس 0;
}

ساکن صحیح cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            خالی *user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);

  (خالی)tconn;
  ctx->max_bidi_streams = max_streams;
  اگر(ڈیٹا)
    CURL_TRC_CF(ڈیٹا, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->مستعمل_bidi_streams);
  واپس 0;
}

ساکن صحیح cb_extend_max_دھارا_ڈیٹا(ngtcp2_conn *tconn, int64_t دھارا_id,
                                     uint64_t max_ڈیٹا, خالی *user_ڈیٹا,
                                     خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا;
  ساخت Curl_easy *ڈیٹا =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_ڈیٹا, &دھارا);
  صحیح rv;
  (خالی)tconn;
  (خالی)max_ڈیٹا;

  rv = nghttp3_conn_unblock_دھارا(ctx->h3conn, دھارا_id);
  اگر(rv && rv != NGHTTP3_ERR_دھارا_NOT_ملا) {
    واپس NGTCP2_ERR_CALLBACK_FAILURE;
  }
  اگر(دھارا && دھارا->quic_flow_blocked) {
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] unblock quic flow", دھارا_id);
    دھارا->quic_flow_blocked = FALSE;
    اگر(ڈیٹا)
      Curl_multi_mark_dirty(ڈیٹا);
  }
  واپس 0;
}

ساکن خالی cb_rand(uint8_t *dest, size_t destlen,
                    ثابت ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode نتیجہ;
  (خالی)rand_ctx;

  نتیجہ = Curl_rand(تہی, dest, destlen);
  اگر(نتیجہ) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
ساکن صحیح cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    خالی *user_ڈیٹا)
{
  CURLcode نتیجہ;
  (خالی)tconn;
  (خالی)user_ڈیٹا;

  نتیجہ = Curl_rand(تہی, cid->ڈیٹا, cidlen);
  اگر(نتیجہ)
    واپس NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  نتیجہ = Curl_rand(تہی, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  اگر(نتیجہ)
    واپس NGTCP2_ERR_CALLBACK_FAILURE;

  واپس 0;
}

#اگر_تعریف_شدہ NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
ساکن صحیح cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  ساخت ngtcp2_stateless_reset_token *token, size_t cidlen, خالی *user_ڈیٹا)
{
  CURLcode نتیجہ;
  (خالی)tconn;
  (خالی)user_ڈیٹا;

  نتیجہ = Curl_rand(تہی, cid->ڈیٹا, cidlen);
  اگر(نتیجہ)
    واپس NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  نتیجہ = Curl_rand(تہی, token->ڈیٹا, حجم(token->ڈیٹا));
  اگر(نتیجہ)
    واپس NGTCP2_ERR_CALLBACK_FAILURE;

  واپس 0;
}
#اگر_ختم

ساکن صحیح cb_recv_rx_کلید(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          خالی *user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf ? cf->ctx : تہی;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  (خالی)tconn;

  اگر(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    واپس 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ڈیٹا);
  اگر(ctx && ڈیٹا && !ctx->h3conn && ctx->init_h3_conn_cb) {
    اگر(ctx->init_h3_conn_cb(cf, ڈیٹا, ctx))
      واپس NGTCP2_ERR_CALLBACK_FAILURE;
  }
  واپس 0;
}

#اگر_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wmissing-field-initializers"
#اگر_ختم
ساکن ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  تہی, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_ڈیٹا_cb,
  cb_ngtcp2_handshake_completed,
  تہی, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_نقاب_cb,
  cb_recv_دھارا_ڈیٹا,
  cb_acked_دھارا_ڈیٹا_انحراف,
  تہی, /* stream_open */
  cb_دھارا_close,
  تہی, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  تہی, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  تہی, /* remove_connection_id */
  ngtcp2_crypto_update_کلید_cb, /* update_key */
  تہی, /* path_validation */
  تہی, /* select_preferred_addr */
  cb_دھارا_reset,
  تہی, /* extend_max_remote_streams_bidi */
  تہی, /* extend_max_remote_streams_uni */
  cb_extend_max_دھارا_ڈیٹا,
  تہی, /* dcid_status */
  تہی, /* handshake_confirmed */
  تہی, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  تہی, /* recv_datagram */
  تہی, /* ack_datagram */
  تہی, /* lost_datagram */
  ngtcp2_crypto_get_راستہ_challenge_ڈیٹا_cb,
  cb_دھارا_stop_sending,
  تہی, /* version_negotiation */
  cb_recv_rx_کلید,
  تہی, /* recv_tx_key */
  تہی, /* early_data_rejected */
#اگر_تعریف_شدہ NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  تہی, /* begin_path_validation */
#اگر_ختم
#اگر_تعریف_شدہ NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  تہی, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  تہی, /* dcid_status2 */
  ngtcp2_crypto_get_راستہ_challenge_data2_cb, /* get_path_challenge_data2 */
#اگر_ختم
#اگر_تعریف_شدہ NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  تہی, /* recv_stop_sending */
#اگر_ختم
#اگر_تعریف_شدہ NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_دھارا_close2, /* is called instead of cb_stream_close when set */
#اگر_ختم
};
#اگر_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic نکالو
#اگر_ختم

#اگر defined(_MSC_VER) && defined(_DLL)
#ہدایت warning(نکالو)
#اگر_ختم

#اگر_تعریف_شدہ USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ساکن صحیح quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ساخت Curl_cfilter *cf;
  ساخت cf_ngtcp2_ctx *ctx;
  ساخت Curl_easy *ڈیٹا;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_ڈیٹا(ssl);
  cf = cref ? cref->user_ڈیٹا : تہی;
  ctx = cf ? cf->ctx : تہی;
  ڈیٹا = cf ? CF_ڈیٹا_CURRENT(cf) : تہی;
  اگر(cf && ڈیٹا && ctx) {
    غیرعلامتی حرف *quic_tp = تہی;
    size_t quic_tp_len = 0;
#اگر_تعریف_شدہ HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     حجم(tpbuf));
    اگر(tplen < 0)
      CURL_TRC_CF(ڈیٹا, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((صحیح)tplen));
    ورنہ {
      quic_tp = (غیرعلامتی حرف *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#اگر_ختم
    Curl_ossl_جوڑو_session(cf, ڈیٹا, &ctx->tls.ossl, ctx->ssl_peer.scache_کلید,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, تہی);
  }
  واپس 0;
}
#اگر_ختم /* USE_OPENSSL */

#اگر_تعریف_شدہ USE_GNUTLS

#اگر_تعریف_شدہ CURLVERBOSE
ساکن ثابت حرف *gtls_hs_پیغام_name(صحیح mtype)
{
  انتخاب(mtype) {
  حالت 1:
    واپس "ClientHello";
  حالت 2:
    واپس "ServerHello";
  حالت 4:
    واپس "SessionTicket";
  حالت 8:
    واپس "EncryptedExtensions";
  حالت 11:
    واپس "Certificate";
  حالت 13:
    واپس "CertificateRequest";
  حالت 15:
    واپس "CertificateVerify";
  حالت 20:
    واپس "Finished";
  حالت 24:
    واپس "KeyUpdate";
  حالت 254:
    واپس "MessageHash";
  }
  واپس "Unknown";
}
#اگر_ختم

ساکن صحیح quic_gtls_handshake_cb(gnutls_session_t session, غیرعلامتی صحیح htype,
                                  غیرعلامتی when, غیرعلامتی صحیح incoming,
                                  ثابت gnutls_datum_t *پیغام)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  ساخت Curl_cfilter *cf = conn_ref ? conn_ref->user_ڈیٹا : تہی;
  ساخت cf_ngtcp2_ctx *ctx = cf ? cf->ctx : تہی;

  (خالی)پیغام;
  (خالی)incoming;
  اگر(when && cf && ctx) { /* after message has been processed */
    ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
    DEBUGASSERT(ڈیٹا);
    اگر(!ڈیٹا)
      واپس 0;
    CURL_TRC_CF(ڈیٹا, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_پیغام_name(htype), htype);
    انتخاب(htype) {
    حالت GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      غیرعلامتی حرف *quic_tp = تہی;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       حجم(tpbuf));
      اگر(tplen < 0)
        CURL_TRC_CF(ڈیٹا, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحیح)tplen));
      ورنہ {
        quic_tp = (غیرعلامتی حرف *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (خالی)Curl_gtls_cache_session(cf, ڈیٹا, ctx->ssl_peer.scache_کلید,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    تہی);
      توڑو;
    }
    طےشدہ:
      توڑو;
    }
  }
  واپس 0;
}
#اگر_ختم /* USE_GNUTLS */

#اگر_تعریف_شدہ USE_WOLFSSL
ساکن صحیح wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolf_SSL_get_app_ڈیٹا(ssl);
  ساخت Curl_cfilter *cf = conn_ref ? conn_ref->user_ڈیٹا : تہی;

  DEBUGASSERT(cf);
  اگر(cf && session) {
    ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
    ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
    DEBUGASSERT(ڈیٹا);
    اگر(ڈیٹا && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      غیرعلامتی حرف *quic_tp = تہی;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       حجم(tpbuf));
      اگر(tplen < 0)
        CURL_TRC_CF(ڈیٹا, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحیح)tplen));
      ورنہ {
        quic_tp = (غیرعلامتی حرف *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (خالی)Curl_wssl_cache_session(cf, ڈیٹا, ctx->ssl_peer.scache_کلید,
                                    session, wolf_SSL_نسخہ(ssl),
                                    "h3", quic_tp, quic_tp_len, تہی);
    }
  }
  واپس 0;
}
#اگر_ختم /* USE_WOLFSSL */

ساکن CURLcode cf_ngtcp2_tls_ctx_تیاری(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        خالی *user_ڈیٹا)
{
  ساخت curl_tls_ctx *ctx = user_ڈیٹا;

#اگر_تعریف_شدہ USE_OPENSSL
#اگر defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  اگر(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(ڈیٹا, "ngtcp2_crypto_boringssl_configure_client_context failed");
    واپس CURLE_FAILED_INIT;
  }
#وگرنہ_اگر defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#ورنہ
  اگر(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(ڈیٹا, "ngtcp2_crypto_quictls_configure_client_context failed");
    واپس CURLE_FAILED_INIT;
  }
#اگر_ختم /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  اگر(Curl_ssl_scache_use(cf, ڈیٹا)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_سیٹ_session_cache_منوال(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_اندرونی);
    SSL_CTX_sess_سیٹ_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#وگرنہ_اگر defined(USE_GNUTLS)
  اگر(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(ڈیٹا, "ngtcp2_crypto_gnutls_configure_client_session failed");
    واپس CURLE_FAILED_INIT;
  }
  اگر(Curl_ssl_scache_use(cf, ڈیٹا)) {
    gnutls_handshake_سیٹ_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#وگرنہ_اگر defined(USE_WOLFSSL)
  اگر(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(ڈیٹا, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    واپس CURLE_FAILED_INIT;
  }
  اگر(Curl_ssl_scache_use(cf, ڈیٹا)) {
    /* Register to get notified when a new session is received */
    wolf_SSL_CTX_sess_سیٹ_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#اگر_ختم
  واپس CURLE_OK;
}

ساکن CURLcode cf_ngtcp2_on_session_reuse(ساخت Curl_cfilter *cf,
                                           ساخت Curl_easy *ڈیٹا,
                                           ساخت alpn_spec *alpns,
                                           ساخت Curl_ssl_session *scs,
                                           bool *do_early_ڈیٹا)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  *do_early_ڈیٹا = FALSE;
#اگر defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#اگر_ختم
#اگر_تعریف_شدہ USE_GNUTLS
  ctx->earlydata_max =
    gnutls_ریکارڈ_get_max_early_ڈیٹا_سائز(ctx->tls.gtls.session);
#اگر_ختم
#اگر_تعریف_شدہ USE_WOLFSSL
#اگر_تعریف_شدہ WOLFSSL_EARLY_ڈیٹا
  ctx->earlydata_max = scs->earlydata_max;
#ورنہ
  ctx->earlydata_max = 0;
#اگر_ختم /* WOLFSSL_EARLY_DATA */
#اگر_ختم
#اگر defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  اگر(!ctx->earlydata_max) {
    CURL_TRC_CF(ڈیٹا, cf, "SSL session does not allow earlydata");
  }
  ورنہ اگر(!Curl_alpn_شامل_ہے_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(ڈیٹا, cf, "SSL session from different ALPN, no early data");
  }
  ورنہ اگر(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(ڈیٹا, cf, "no 0RTT transport parameters, no early data");
  }
  ورنہ {
    صحیح rv;
    rv = ngtcp2_conn_decode_and_سیٹ_0rtt_transport_params(
      ctx->qconn, (ثابت uint8_t *)scs->quic_tp, scs->quic_tp_len);
    اگر(rv)
      CURL_TRC_CF(ڈیٹا, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    ورنہ اگر(ctx->init_h3_conn_cb) {
      infof(ڈیٹا, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      نتیجہ = ctx->init_h3_conn_cb(cf, ڈیٹا, ctx);
      اگر(!نتیجہ) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_ڈیٹا = TRUE;
      }
    }
    ورنہ { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_ڈیٹا = TRUE;
    }
  }
#ورنہ /* not supported in the TLS backend */
  (خالی)ڈیٹا;
  (خالی)ctx;
  (خالی)scs;
  (خالی)alpns;
#اگر_ختم
  واپس نتیجہ;
}

/*
 * Might be called twice for happy eyeballs.
 */
ساکن CURLcode cf_connect_آغاز(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت cf_ngtcp2_io_ctx *pktx)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  صحیح rc;
  صحیح rv;
  CURLcode نتیجہ;
  ثابت ساخت Curl_sockaddr_ex *sockaddr = تہی;
  صحیح qfd;
  ساکن ثابت ساخت alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  نتیجہ = Curl_rand(ڈیٹا, ctx->dcid.ڈیٹا, NGTCP2_MAX_CIDLEN);
  اگر(نتیجہ)
    واپس نتیجہ;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  نتیجہ = Curl_rand(ڈیٹا, ctx->scid.ڈیٹا, NGTCP2_MAX_CIDLEN);
  اگر(نتیجہ)
    واپس نتیجہ;

  (خالی)Curl_qlogdir(ڈیٹا, ctx->scid.ڈیٹا, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, ڈیٹا, pktx);

  نتیجہ = Curl_vquic_ctx_init(ڈیٹا, &ctx->q);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Query socket and remote address from sub-chain */
  اگر(Curl_cf_socket_peek(cf->اگلا, ڈیٹا, &ctx->q.sockfd, &sockaddr, تہی)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  اگر(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = حجم(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (ساخت sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    اگر(rv == -1)
      واپس CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_راستہ.local,
                     (ساخت sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_راستہ.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  ورنہ {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ثابت ساخت Curl_sockaddr_ex *remote = تہی;
    اگر(cf->اگلا->cft->query(cf->اگلا, ڈیٹا, CF_QUERY_REMOTE_ADDR, تہی,
                            CURL_UNCONST(&remote)))
      واپس CURLE_QUIC_CONNECT_ERROR;
    اگر(!remote)
      واپس CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, حجم(ctx->q.local_addr));
    انتخاب(remote->family) {
    حالت AF_INET:
      ((ساخت sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = حجم(ساخت sockaddr_in);
      توڑو;
#اگر_تعریف_شدہ USE_IPV6
    حالت AF_INET6:
      ((ساخت sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = حجم(ساخت sockaddr_in6);
      توڑو;
#اگر_ختم
    طےشدہ:
      واپس CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_راستہ.local,
                     (ساخت sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_راستہ.remote,
                     &remote->curl_sa_addr,
                     (socklen_t)remote->addrlen);
  }

  rc = ngtcp2_conn_client_new(&ctx->qconn, &ctx->dcid, &ctx->scid,
                              &ctx->connected_راستہ,
                              NGTCP2_PROTO_VER_V1, &ng_callbacks,
                              &ctx->settings, &ctx->transport_params,
                              Curl_ngtcp2_mem(), cf);
  اگر(rc)
    واپس CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_ڈیٹا = cf;

  نتیجہ = Curl_vquic_tls_init(&ctx->tls, cf, ڈیٹا,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_تیاری, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  اگر(نتیجہ)
    واپس نتیجہ;

#اگر defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  اگر(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(ڈیٹا, "ngtcp2_crypto_ossl_ctx_new failed");
    واپس CURLE_FAILED_INIT;
  }
  ngtcp2_conn_سیٹ_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  اگر(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(ڈیٹا, "ngtcp2_crypto_ossl_configure_client_session failed");
    واپس CURLE_FAILED_INIT;
  }
#وگرنہ_اگر defined(USE_OPENSSL)
  SSL_سیٹ_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_سیٹ_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#وگرنہ_اگر defined(USE_GNUTLS)
  ngtcp2_conn_سیٹ_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#وگرنہ_اگر defined(USE_WOLFSSL)
  ngtcp2_conn_سیٹ_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#ورنہ
#غلطی "ngtcp2 TLS backend not defined"
#اگر_ختم

#اگر defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  اگر(ctx->use_earlydata &&
     !SSL_سیٹ_quic_tls_early_ڈیٹا_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(ڈیٹا, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#اگر_ختم
  ngtcp2_ccerr_default(&ctx->آخری_error);

  واپس CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    bool *مکمل)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  ساخت cf_call_ڈیٹا save;
  ساخت cf_ngtcp2_io_ctx pktx;

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  /* Connect the sub-chain */
  اگر(cf->اگلا && !cf->اگلا->connected) {
    نتیجہ = Curl_conn_cf_connect(cf->اگلا, ڈیٹا, مکمل);
    اگر(نتیجہ || !*مکمل)
      واپس نتیجہ;
  }

  *مکمل = FALSE;

  اگر(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(ڈیٹا, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(ڈیٹا, cf, "need HTTPS-RR, delaying connect");
    واپس CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, ڈیٹا);
  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);

  اگر(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(ڈیٹا);
    نتیجہ = cf_connect_آغاز(cf, ڈیٹا, &pktx);
    اگر(نتیجہ)
      جاؤ برآمد;
    اگر(cf->connected) {
      *مکمل = TRUE;
      جاؤ برآمد;
    }
    نتیجہ = Curl_cf_ngtcp2_پیش_رفت_egress(cf, ڈیٹا, &pktx);
    /* we do not expect to be able to recv anything yet */
    جاؤ برآمد;
  }

  نتیجہ = Curl_cf_ngtcp2_پیش_رفت_ingress(cf, ڈیٹا, &pktx);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = Curl_cf_ngtcp2_پیش_رفت_egress(cf, ڈیٹا, &pktx);
  اگر(نتیجہ)
    جاؤ برآمد;

  اگر(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    نتیجہ = ctx->tls_vrfy_نتیجہ;
    اگر(!نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "peer verified");
      cf->connected = TRUE;
      *مکمل = TRUE;
    }
  }

برآمد:
  اگر(ctx->tls_vrfy_نتیجہ)
    نتیجہ = ctx->tls_vrfy_نتیجہ;
  اگر(ctx->qconn &&
     ((نتیجہ == CURLE_RECV_ERROR) || (نتیجہ == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_اندر_draining_period(ctx->qconn)) {
    ثابت ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    نتیجہ = CURLE_COULDNT_CONNECT;
    اگر(cerr) {
      CURL_TRC_CF(ڈیٹا, cf, "connect error, type=%d, code=%" PRIu64,
                  (صحیح)cerr->type, cerr->error_رمز);
      انتخاب(cerr->type) {
      حالت NGTCP2_CCERR_TYPE_نسخہ_NEGOTIATION:
        CURL_TRC_CF(ڈیٹا, cf, "error in version negotiation");
        توڑو;
      طےشدہ:
        اگر(cerr->error_رمز >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(ڈیٹا, cf, "crypto error, tls alert=%u",
                      (غیرعلامتی صحیح)(cerr->error_رمز & 0xffU));
        }
        ورنہ اگر(cerr->error_رمز == NGTCP2_CONNECTION_REFUSED) {
          CURL_TRC_CF(ڈیٹا, cf, "connection refused by server");
          /* When a QUIC server instance is shutting down, it may send us a
           * CONNECTION_CLOSE with this code right away. We want
           * to keep on trying in this case. */
          نتیجہ = CURLE_WEIRD_SERVER_جواب;
        }
      }
    }
  }

#اگر_تعریف_شدہ CURLVERBOSE
  اگر(نتیجہ) {
    اگر(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      ساخت ip_quadruple ip;

      اگر(!Curl_cf_socket_peek(cf->اگلا, ڈیٹا, تہی, تہی, &ip))
        infof(ڈیٹا, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_پورٹ, curl_easy_strerror(نتیجہ));
    }
  }
#اگر_ختم
  اگر(!نتیجہ && ctx->qconn) {
    نتیجہ = Curl_cf_ngtcp2_cmn_سیٹ_expiry(cf, ڈیٹا, &pktx);
  }
  اگر(نتیجہ || *مکمل)
    CURL_TRC_CF(ڈیٹا, cf, "connect -> %d, done=%d", (صحیح)نتیجہ, *مکمل);
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  ساخت cf_ngtcp2_io_ctx pktx;
  CURLcode نتیجہ = CURLE_OK;

  اگر(cf->shutdown || !ctx->qconn) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  اگر(!cf->اگلا) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  *مکمل = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, ڈیٹا);

  اگر(!ctx->shutdown_started) {
    حرف بفر[NGTCP2_MAX_UDP_PAYLOAD_سائز];
    ngtcp2_ssize nwritten;

    اگر(!Curl_bufq_is_خالی_خانے(&ctx->q.sendbuf)) {
      CURL_TRC_CF(ڈیٹا, cf, "shutdown, flushing sendbuf");
      نتیجہ = Curl_cf_ngtcp2_پیش_رفت_egress(cf, ڈیٹا, &pktx);
      اگر(!Curl_bufq_is_خالی_خانے(&ctx->q.sendbuf)) {
        CURL_TRC_CF(ڈیٹا, cf, "sending shutdown packets blocked");
        نتیجہ = CURLE_OK;
        جاؤ برآمد;
      }
      ورنہ اگر(نتیجہ) {
        CURL_TRC_CF(ڈیٹا, cf, "shutdown, error %d flushing sendbuf",
                    (صحیح)نتیجہ);
        *مکمل = TRUE;
        جاؤ برآمد;
      }
    }

    DEBUGASSERT(Curl_bufq_is_خالی_خانے(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, تہی, /*راستہ*/
      تہی, /* pkt_info */
      (uint8_t *)بفر, حجم(بفر),
      &ctx->آخری_error, pktx.ts);
    CURL_TRC_CF(ڈیٹا, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (صحیح)ctx->آخری_error.type,
                ctx->آخری_error.error_رمز, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    اگر(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      نتیجہ = Curl_bufq_write(&ctx->q.sendbuf, (ثابت غیرعلامتی حرف *)بفر,
                               (size_t)nwritten, &n);
      اگر(نتیجہ) {
        CURL_TRC_CF(ڈیٹا, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (صحیح)نتیجہ);
        جاؤ برآمد;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.بانٹو_len = 0;
    }
  }

  اگر(!Curl_bufq_is_خالی_خانے(&ctx->q.sendbuf)) {
    CURL_TRC_CF(ڈیٹا, cf, "shutdown, flushing egress");
    نتیجہ = Curl_vquic_flush(cf, ڈیٹا, &ctx->q);
    اگر(نتیجہ == CURLE_AGAIN) {
      CURL_TRC_CF(ڈیٹا, cf, "sending shutdown packets blocked");
      نتیجہ = CURLE_OK;
      جاؤ برآمد;
    }
    ورنہ اگر(نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "shutdown, error %d flushing sendbuf",
                  (صحیح)نتیجہ);
      *مکمل = TRUE;
      جاؤ برآمد;
    }
  }

  اگر(Curl_bufq_is_خالی_خانے(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(ڈیٹا, cf, "shutdown completely sent off, done");
    *مکمل = TRUE;
    نتیجہ = CURLE_OK;
  }
برآمد:
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

خالی Curl_cf_ngtcp2_cmn_conn_close(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  bool مکمل;
  Curl_cf_ngtcp2_cmn_shutdown(cf, ڈیٹا, &مکمل);
}

ساکن bool cf_ngtcp2_err_is_fatal(صحیح رمز)
{
  واپس (NGTCP2_ERR_FATAL >= رمز) ||
         (NGTCP2_ERR_DROP_CONN == رمز) ||
         (NGTCP2_ERR_IDLE_CLOSE == رمز);
}

خالی Curl_cf_ngtcp2_cmn_err_سیٹ(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا, صحیح رمز)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  اگر(!ctx->آخری_error.error_رمز) {
    اگر(NGTCP2_ERR_CRYPTO == رمز) {
      ngtcp2_ccerr_سیٹ_tls_alert(&ctx->آخری_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 تہی, 0);
    }
    ورنہ {
      ngtcp2_ccerr_سیٹ_liberr(&ctx->آخری_error, رمز, تہی, 0);
    }
  }
  اگر(cf_ngtcp2_err_is_fatal(رمز))
    Curl_cf_ngtcp2_cmn_conn_close(cf, ڈیٹا);
}

خالی Curl_cf_ngtcp2_io_ctx_init(ساخت cf_ngtcp2_io_ctx *io_ctx,
                                ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->ڈیٹا = ڈیٹا;
  io_ctx->now = *Curl_pgrs_now(ڈیٹا);
  ngtcp2_راستہ_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_سیٹ_وقت(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

خالی Curl_cf_ngtcp2_io_ctx_update_وقت(ساخت Curl_easy *ڈیٹا,
                                       ساخت cf_ngtcp2_io_ctx *io_ctx,
                                       ساخت Curl_cfilter *cf)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(ڈیٹا);
  Curl_vquic_ctx_update_وقت(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#اگر NGTCP2_نسخہ_عدد < 0x011100
ساخت cf_ngtcp2_sfind_ctx {
  int64_t دھارا_id;
  ساخت h3_دھارا_ctx *دھارا;
  uint32_t mid;
};

ساکن bool cf_ngtcp2_sfind(uint32_t mid, خالی *قدر, خالی *user_ڈیٹا)
{
  ساخت cf_ngtcp2_sfind_ctx *fctx = user_ڈیٹا;
  ساخت h3_دھارا_ctx *دھارا = قدر;

  اگر(fctx->دھارا_id == دھارا->id) {
    fctx->mid = mid;
    fctx->دھارا = دھارا;
    واپس FALSE;
  }
  واپس TRUE; /* continue */
}

ساکن ساخت h3_دھارا_ctx *cf_ngtcp2_get_دھارا(ساخت cf_ngtcp2_ctx *ctx,
                                                  int64_t دھارا_id)
{
  ساخت cf_ngtcp2_sfind_ctx fctx;
  fctx.دھارا_id = دھارا_id;
  fctx.دھارا = تہی;
  Curl_u32_ptrset_دورہ(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  واپس fctx.دھارا;
}
#ورنہ
ساکن ساخت h3_دھارا_ctx *cf_ngtcp2_get_دھارا(ساخت cf_ngtcp2_ctx *ctx,
                                                  int64_t دھارا_id)
{
  واپس ngtcp2_conn_get_دھارا_user_ڈیٹا(ctx->qconn, دھارا_id);
}
#اگر_ختم

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
ساکن CURLcode read_pkt_مقصد_send(خالی *userp,
                                 غیرعلامتی حرف *buf, size_t buflen,
                                 size_t *pnread)
{
  ساخت cf_ngtcp2_io_ctx *x = userp;
  ساخت cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t دھارا_id;
  صحیح fin;
  ssize_t n;

  *pnread = 0;
  veccnt = 0;
  دھارا_id = -1;
  fin = 0;

  /* ngtcp2 may want to put several frames from different streams into
   * this packet. `NGTCP2_WRITE_STREAM_FLAG_MORE` tells it to do so.
   * When `NGTCP2_ERR_WRITE_MORE` is returned, we *need* to make
   * another iteration.
   * When ngtcp2 is happy (because it has no other frame that would fit
   * or it has nothing more to send), it returns the total length
   * of the assembled packet. This may be 0 if there was nothing to send. */
  برائے(;;) {

    اگر(ctx->h3conn && ngtcp2_conn_get_max_ڈیٹا_بایاں(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_دھارا(ctx->h3conn, &دھارا_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      اگر(veccnt < 0) {
        failf(x->ڈیٹا, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((صحیح)veccnt));
        Curl_cf_ngtcp2_h3_err_سیٹ(x->cf, x->ڈیٹا, (صحیح)veccnt);
        واپس CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_دھارا_FLAG_مزید |
            (fin ? NGTCP2_WRITE_دھارا_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_دھارا(ctx->qconn, &x->ps.راستہ,
                                  تہی, buf, buflen,
                                  &ndatalen, flags, دھارا_id,
                                  (ثابت ngtcp2_vec *)vec, veccnt, x->ts);
    اگر(n == 0) {
      /* nothing to send */
      واپس CURLE_AGAIN;
    }
    ورنہ اگر(n < 0) {
      انتخاب(n) {
      حالت NGTCP2_ERR_دھارا_ڈیٹا_BLOCKED: {
        ساخت h3_دھارا_ctx *دھارا;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_بلاک_دھارا(ctx->h3conn, دھارا_id);
        CURL_TRC_CF(x->ڈیٹا, x->cf, "[%" PRId64 "] block quic flow",
                    دھارا_id);
        دھارا = cf_ngtcp2_get_دھارا(ctx, دھارا_id);
        اگر(دھارا) /* it might be not one of our h3 streams? */
          دھارا->quic_flow_blocked = TRUE;
        n = 0;
        توڑو;
      }
      حالت NGTCP2_ERR_دھارا_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_دھارا_write(ctx->h3conn, دھارا_id);
        n = 0;
        توڑو;
      حالت NGTCP2_ERR_WRITE_مزید:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        توڑو;
      طےشدہ:
        DEBUGASSERT(ndatalen == -1);
        failf(x->ڈیٹا, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((صحیح)n));
        Curl_cf_ngtcp2_cmn_err_سیٹ(x->cf, x->ڈیٹا, (صحیح)n);
        واپس CURLE_SEND_ERROR;
      }
    }

    اگر(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      صحیح rv = nghttp3_conn_جوڑو_write_انحراف(ctx->h3conn, دھارا_id, ndatalen);
      اگر(rv) {
        failf(x->ڈیٹا, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        واپس CURLE_SEND_ERROR;
      }
    }

    اگر(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      واپس CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_پیش_رفت_egress(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        ساخت cf_ngtcp2_io_ctx *pktx)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_سائز, راستہ_max_payload_سائز;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_وقتی_حصہ;
  CURLcode نتیجہ;
  ساخت cf_ngtcp2_io_ctx local_pktx;

  اگر(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, ڈیٹا);
    pktx = &local_pktx;
  }
  ورنہ {
    Curl_cf_ngtcp2_io_ctx_update_وقت(ڈیٹا, pktx, cf);
    ngtcp2_راستہ_storage_zero(&pktx->ps);
  }

  نتیجہ = Curl_vquic_flush(cf, ڈیٹا, &ctx->q);
  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_AGAIN) {
      Curl_expire_سیٹ(ڈیٹا, EXPIRE_QUIC, 1, &pktx->now);
      واپس CURLE_OK;
    }
    واپس نتیجہ;
  }

  /* In UDP, there is a maximum theoretical packet payload length and
   * a minimum payload length that is "guaranteed" to work.
   * To detect if this minimum payload can be increased, ngtcp2 sends
   * now and then a packet payload larger than the minimum. It that
   * is ACKed by the peer, both parties know that it works and
   * the subsequent packets can use a larger one.
   * This is called PMTUD (Path Maximum Transmission Unit Discovery).
   * Since a PMTUD might be rejected right on send, we do not want it
   * be followed by other packets of lesser size. Because those would
   * also fail then. If we detect a PMTUD while buffering, we flush.
   */
  max_payload_سائز = ngtcp2_conn_get_max_tx_udp_payload_سائز(ctx->qconn);
  راستہ_max_payload_سائز =
    ngtcp2_conn_get_راستہ_max_tx_udp_payload_سائز(ctx->qconn);
  send_وقتی_حصہ = ngtcp2_conn_get_send_وقتی_حصہ(ctx->qconn);
  CURL_TRC_CF(ڈیٹا, cf, "egress, collect and send packets, quantum=%zu",
              send_وقتی_حصہ);
  برائے(;;) {
    /* add the next packet to send, if any, to our buffer */
    نتیجہ = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_سائز,
                            read_pkt_مقصد_send, pktx, &nread);
    اگر(نتیجہ == CURLE_AGAIN)
      توڑو;
    ورنہ اگر(نتیجہ)
      واپس نتیجہ;
    ورنہ {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      اگر((buflen >= send_وقتی_حصہ) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_سائز))
        توڑو;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      اگر(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      ورنہ اگر(nread > gsolen ||
              (gsolen > راستہ_max_payload_سائز && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        نتیجہ = Curl_vquic_send_دم_بانٹو(cf, ڈیٹا, &ctx->q,
                                            gsolen, nread, nread);
        اگر(نتیجہ) {
          اگر(نتیجہ == CURLE_AGAIN) {
            Curl_expire_سیٹ(ڈیٹا, EXPIRE_QUIC, 1, &pktx->now);
            واپس CURLE_OK;
          }
          واپس نتیجہ;
        }
        pktcnt = 0;
      }
      ورنہ اگر(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        توڑو;
      }
    }
  }

  اگر(!Curl_bufq_is_خالی_خانے(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(ڈیٹا, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    نتیجہ = Curl_vquic_send(cf, ڈیٹا, &ctx->q, gsolen);
    اگر(نتیجہ) {
      اگر(نتیجہ == CURLE_AGAIN) {
        Curl_expire_سیٹ(ڈیٹا, EXPIRE_QUIC, 1, &pktx->now);
        واپس CURLE_OK;
      }
      واپس نتیجہ;
    }
    Curl_cf_ngtcp2_io_ctx_update_وقت(ڈیٹا, pktx, cf);
    ngtcp2_conn_update_pkt_tx_وقت(ctx->qconn, pktx->ts);
  }
  واپس CURLE_OK;
}

ساخت cf_ngtcp2_recv_ctx {
  ساخت cf_ngtcp2_io_ctx *pktx;
  size_t pkt_گنتی;
};

ساکن CURLcode cf_ngtcp2_recv_pkts(ثابت غیرعلامتی حرف *buf, size_t buflen,
                                    size_t gso_سائز,
                                    ساخت sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    خالی *userp)
{
  ساخت cf_ngtcp2_recv_ctx *rctx = userp;
  ساخت cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  ساخت cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_راستہ راستہ;
  size_t انحراف, pktlen;
  صحیح rv;

  اگر(!rctx->pkt_گنتی) {
    Curl_cf_ngtcp2_io_ctx_update_وقت(pktx->ڈیٹا, pktx, pktx->cf);
    ngtcp2_راستہ_storage_zero(&pktx->ps);
  }

  اگر(ecn)
    CURL_TRC_CF(pktx->ڈیٹا, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_سائز, (غیرعلامتی)ecn);
  ngtcp2_addr_init(&راستہ.local, (ساخت sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&راستہ.remote, (ساخت sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  برائے(انحراف = 0; انحراف < buflen; انحراف += gso_سائز) {
    rctx->pkt_گنتی++;
    pktlen = ((انحراف + gso_سائز) <= buflen) ? gso_سائز : (buflen - انحراف);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &راستہ, &pi,
                              buf + انحراف, pktlen, pktx->ts);
    اگر(rv) {
      CURL_TRC_CF(pktx->ڈیٹا, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_سیٹ(pktx->cf, pktx->ڈیٹا, rv);

      اگر(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        واپس CURLE_PEER_FAILED_VERIFICATION;
      واپس CURLE_RECV_ERROR;
    }
  }
  واپس CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_پیش_رفت_ingress(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا,
                                         ساخت cf_ngtcp2_io_ctx *pktx)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت cf_ngtcp2_io_ctx local_pktx;
  ساخت cf_ngtcp2_recv_ctx rctx;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, ڈیٹا);
    pktx = &local_pktx;
  }

  نتیجہ = Curl_vquic_tls_before_recv(&ctx->tls, cf, ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;

  rctx.pktx = pktx;
  rctx.pkt_گنتی = 0;

  اگر(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(ڈیٹا, cf, "progress_ingress(socket)");
    واپس Curl_vquic_recv_packets(cf, ڈیٹا, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  ورنہ {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    غیرعلامتی حرف *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_سائز;
    size_t pkt_حد_بندی = QUIC_TUNNEL_INGRESS_PKT_حد_بندی;
    size_t nread;
    ساخت sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(ڈیٹا, cf, "progress_ingress(sub-filters)");
    اگر(ctx->qconn) {
      size_t max_راستہ_payload;
      max_راستہ_payload =
        ngtcp2_conn_get_راستہ_max_tx_udp_payload_سائز(ctx->qconn);
      اگر(max_راستہ_payload > max_udp_payload)
        max_udp_payload = max_راستہ_payload;
    }

    اگر(ctx->tunnel_inbuf_len < max_udp_payload) {
      غیرعلامتی حرف *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      اگر(!newbuf)
        واپس CURLE_برآمد_کا_حافظہ;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    جب(pkt_حد_بندی--) {
      نتیجہ = Curl_conn_cf_recv(cf->اگلا, ڈیٹا, (حرف *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      اگر(نتیجہ == CURLE_AGAIN) {
        /* no more data available at the moment */
        واپس CURLE_OK;
      }
      اگر(نتیجہ) {
        CURL_TRC_CF(ڈیٹا, cf, "ingress, recv from tunnel failed: %d",
                    (صحیح)نتیجہ);
        واپس نتیجہ;
      }
      اگر(nread == 0) {
        /* tunnel closed */
        واپس CURLE_OK;
      }

      حافظہ_نقل(&remote_addr, ctx->connected_راستہ.remote.addr,
             ctx->connected_راستہ.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_راستہ.remote.addrlen;
      نتیجہ = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      اگر(نتیجہ)
        واپس نتیجہ;

      اگر(!ctx->q.got_پہلا_byte) {
        ctx->q.got_پہلا_byte = TRUE;
        ctx->q.پہلا_byte_at = ctx->q.آخری_op;
      }
      ctx->q.آخری_io = ctx->q.آخری_op;
    }
    واپس CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_سیٹ_expiry(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا,
                                       ساخت cf_ngtcp2_io_ctx *pktx)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  اگر(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, ڈیٹا);
    pktx = &local_pktx;
  }
  ورنہ {
    Curl_cf_ngtcp2_io_ctx_update_وقت(ڈیٹا, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  اگر(expiry != UINT64_MAX) {
    اگر(expiry <= pktx->ts) {
      CURLcode نتیجہ;
      صحیح rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      اگر(rv) {
        failf(ڈیٹا, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_سیٹ(cf, ڈیٹا, rv);
        واپس CURLE_SEND_ERROR;
      }
      نتیجہ = Curl_cf_ngtcp2_پیش_رفت_ingress(cf, ڈیٹا, pktx);
      اگر(!نتیجہ)
        نتیجہ = Curl_cf_ngtcp2_پیش_رفت_egress(cf, ڈیٹا, pktx);
      اگر(نتیجہ) {
        /* a verify failure during ingress must win over generic errors */
        اگر(ctx->tls_vrfy_نتیجہ)
          نتیجہ = ctx->tls_vrfy_نتیجہ;
        واپس نتیجہ;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    اگر(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      اگر(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_سیٹ(ڈیٹا, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  واپس CURLE_OK;
}

ساکن خالی cf_ngtcp2_تیاری_keep_alive(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ثابت ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  اگر(!ctx->qconn)
    واپس;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  اگر(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_سیٹ_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(ڈیٹا, cf, "no peer idle timeout, unset keep-alive");
  }
  ورنہ اگر(!Curl_u32_ptrset_گنتی(&ctx->streams)) {
    ngtcp2_conn_سیٹ_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(ڈیٹا, cf, "no active streams, unset keep-alive");
  }
  ورنہ {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_سیٹ_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(ڈیٹا, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_دھارا_تیاری(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        ساخت h3_دھارا_ctx **pstream)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);

  *pstream = تہی;
  اگر(!ڈیٹا)
    واپس CURLE_FAILED_INIT;

  اگر(دھارا) {
    *pstream = دھارا;
    واپس CURLE_OK;
  }

  دھارا = curlx_calloc(1, حجم(*دھارا));
  اگر(!دھارا)
    واپس CURLE_برآمد_کا_حافظہ;

  دھارا->id = -1;
  دھارا->xfer_id = ڈیٹا->id;
  دھارا->mid = ڈیٹا->mid;
  دھارا->rx_انحراف = 0;
  دھارا->rx_انحراف_max = H3_دھارا_کھڑکی_سائز_INITIAL;
  دھارا->tx_اندر_flight_ideal = H3_دھارا_SEND_BUF_INITIAL;

  /* on send, we control how much we put into the buffer */
  Curl_bufq_initp(&دھارا->sendbuf, &ctx->دھارا_bufcp,
                  H3_دھارا_SEND_CHUNKS, BUFQ_OPT_NONE);
  دھارا->tx_اندر_flight_سائز = 0;
  Curl_h1_req_parse_init(&دھارا->h1, H1_PARSE_DEFAULT_MAX_سطر_LEN);

  اگر(Curl_u32_ptrset_سیٹ(&ctx->streams, ڈیٹا->mid, دھارا)) {
    Curl_cf_ngtcp2_h3_دھارا_ctx_free(دھارا);
    واپس CURLE_برآمد_کا_حافظہ;
  }

  اگر(Curl_u32_ptrset_گنتی(&ctx->streams) == 1)
    cf_ngtcp2_تیاری_keep_alive(cf, ڈیٹا);

  *pstream = دھارا;
  واپس CURLE_OK;
}

خالی Curl_cf_ngtcp2_h3_دھارا_close(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت h3_دھارا_ctx *دھارا)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(دھارا);
  اگر(!دھارا->closed && ctx->qconn && ctx->h3conn) {
    CURLcode نتیجہ;

    nghttp3_conn_سیٹ_دھارا_user_ڈیٹا(ctx->h3conn, دھارا->id, تہی);
    ngtcp2_conn_سیٹ_دھارا_user_ڈیٹا(ctx->qconn, دھارا->id, تہی);
    دھارا->closed = TRUE;
    (خالی)ngtcp2_conn_shutdown_دھارا(ctx->qconn, 0, دھارا->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    نتیجہ = Curl_cf_ngtcp2_پیش_رفت_egress(cf, ڈیٹا, تہی);
    اگر(نتیجہ)
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] cancel stream -> %d",
                  دھارا->id, (صحیح)نتیجہ);
  }
}

خالی Curl_cf_ngtcp2_h3_دھارا_مکمل(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
  (خالی)cf;
  اگر(دھارا) {
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] easy handle is done", دھارا->id);
    Curl_cf_ngtcp2_h3_دھارا_close(cf, ڈیٹا, دھارا);
    Curl_u32_ptrset_unset(&ctx->streams, ڈیٹا->mid);
    اگر(!Curl_u32_ptrset_گنتی(&ctx->streams))
      cf_ngtcp2_تیاری_keep_alive(cf, ڈیٹا);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      bool *داخلہ_pending)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ثابت ngtcp2_transport_params *rp;
  ساخت cf_call_ڈیٹا save;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  *داخلہ_pending = FALSE;
  اگر(!ctx->qconn || ctx->shutdown_started)
    جاؤ برآمد;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  اگر(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &ctx->q.آخری_io);
    اگر(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      اگر((uint64_t)idletime_ms > max_idle_ms)
        جاؤ برآمد;
    }
  }

  اگر(!cf->اگلا || !cf->اگلا->cft->is_alive(cf->اگلا, ڈیٹا, داخلہ_pending))
    جاؤ برآمد;

  alive = TRUE;
  اگر(*داخلہ_pending) {
    CURLcode نتیجہ;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *داخلہ_pending = FALSE;
    نتیجہ = Curl_cf_ngtcp2_پیش_رفت_ingress(cf, ڈیٹا, تہی);
    CURL_TRC_CF(ڈیٹا, cf, "is_alive, progress ingress -> %d", (صحیح)نتیجہ);
    alive = نتیجہ ? FALSE : TRUE;
  }

برآمد:
  CF_ڈیٹا_RESTORE(cf, save);
  واپس alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(ساخت cf_ngtcp2_ctx *ctx,
                                      ساخت Curl_easy *ڈیٹا)
{
  int64_t ctrl_دھارا_id, qpack_enc_دھارا_id, qpack_dec_دھارا_id;
  صحیح rc;

  rc = ngtcp2_conn_open_uni_دھارا(ctx->qconn, &ctrl_دھارا_id, تہی);
  اگر(rc) {
    failf(ڈیٹا, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    واپس CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_دھارا(ctx->h3conn, ctrl_دھارا_id);
  اگر(rc) {
    failf(ڈیٹا, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    واپس CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_دھارا(ctx->qconn, &qpack_enc_دھارا_id, تہی);
  اگر(rc) {
    failf(ڈیٹا, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    واپس CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_دھارا(ctx->qconn, &qpack_dec_دھارا_id, تہی);
  اگر(rc) {
    failf(ڈیٹا, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    واپس CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_دھارا_id,
                                       qpack_dec_دھارا_id);
  اگر(rc) {
    failf(ڈیٹا, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    واپس CURLE_QUIC_CONNECT_ERROR;
  }
  واپس CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  صحیح query, صحیح *pres1, خالی *pres2)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;

  انتخاب(query) {
  حالت CF_QUERY_CONNECT_جواب_MS:
    اگر((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_پہلا_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.پہلا_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < زیادہ_سے_زیادہ_عدد) ? (صحیح)ms : زیادہ_سے_زیادہ_عدد;
      واپس CURLE_OK;
    }
    توڑو;
  حالت CF_QUERY_REALLY_CONNECTED:
    اگر(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_پہلا_byte;
      واپس CURLE_OK;
    }
    توڑو;
  طےشدہ:
    توڑو;
  }
  واپس cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, ڈیٹا, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  صحیح event, صحیح arg1, خالی *arg2)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  (خالی)arg1;
  (خالی)arg2;
  انتخاب(event) {
  حالت CF_CTRL_REPORT_STATS:
    اگر(cf->connected && !ctx->stats_reported) {
      اگر((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_پہلا_byte) {
        Curl_pgrs_وقت_Was(ڈیٹا, TIMER_CONNECT, ctx->q.پہلا_byte_at);
        ctx->stats_reported = TRUE;
      }
      ورنہ اگر(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        اگر(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrs_وقت_Was(ڈیٹا, TIMER_CONNECT, ctx->q.پہلا_byte_at);
        Curl_pgrs_وقت_Was(ڈیٹا, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    توڑو;
  طےشدہ:
    توڑو;
  }
  واپس نتیجہ;
}

#اگر_ختم /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
