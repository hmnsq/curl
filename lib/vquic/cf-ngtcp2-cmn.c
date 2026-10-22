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

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ضمن <ngtcp2/ngtcp2.h>

#اگر_تعریف_شده USE_OPENSSL
#ضمن <openssl/err.h>
#إذا defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ضمن <ngtcp2/ngtcp2_crypto_boringssl.h>
#وإلاإذا defined(OPENSSL_QUIC_API2)
#ضمن <ngtcp2/ngtcp2_crypto_ossl.h>
#وإلا
#ضمن <ngtcp2/ngtcp2_crypto_quictls.h>
#پایان_اگر
#ضمن "vtls/openssl.h"
#وإلاإذا defined(USE_GNUTLS)
#ضمن <ngtcp2/ngtcp2_crypto_gnutls.h>
#ضمن "vtls/gtls.h"
#وإلاإذا defined(USE_WOLFSSL)
#ضمن <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ضمن "vtls/wolfssl.h"
#پایان_اگر

#ضمن <nghttp3/nghttp3.h>

#ضمن "urldata.h"
#ضمن "url.h"
#ضمن "u32_ptrset.h"
#ضمن "curl_trc.h"
#ضمن "rand.h"
#ضمن "multiif.h"
#ضمن "cfilters.h"
#ضمن "cf-socket.h"
#ضمن "connect.h"
#ضمن "progress.h"
#ضمن "curlx/win32-fopen.h"
#ضمن "curlx/dynbuf.h"
#ضمن "http1.h"
#ضمن "select.h"
#ضمن "sockaddr.h"
#ضمن "transfer.h"
#ضمن "bufref.h"
#ضمن "vdns/cf-dns.h"
#ضمن "vquic/vquic.h"
#ضمن "vquic/vquic_int.h"
#ضمن "vquic/vquic-tls.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "vquic/cf-ngtcp2-cmn.h"

/*
 * Store ngtcp2 version info in this buffer.
 */
فراغ Curl_ngtcp2_ver(حرف *p, size_t len)
{
  ثابت ngtcp2_info *ng2 = ngtcp2_نسخه(0);
  ثابت nghttp3_info *ht3 = nghttp3_نسخه(0);
  (فراغ)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->نسخه_str, ht3->نسخه_str);
}

فراغ Curl_cf_ngtcp2_h3_جریان_ctx_free(بنية h3_جریان_ctx *جریان)
{
  Curl_bufq_free(&جریان->sendbuf);
  Curl_h1_req_parse_free(&جریان->h1);
  curlx_free(جریان);
}

ساكن فراغ h3_جریان_مجموعه_free(uint32_t id, فراغ *جریان)
{
  (فراغ)id;
  DEBUGASSERT(جریان);
  Curl_cf_ngtcp2_h3_جریان_ctx_free((بنية h3_جریان_ctx *)جریان);
}

ساكن bool cf_ngtcp2_h3_err_is_fatal(صحيح کد)
{
  أرجع (NGHTTP3_ERR_FATAL >= کد) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_جریان == کد);
}

فراغ Curl_cf_ngtcp2_h3_err_مجموعه(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده, صحيح کد)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  إذا(!ctx->آخرین_error.error_کد) {
    ngtcp2_ccerr_مجموعه_application_error(&ctx->آخرین_error,
      nghttp3_err_infer_quic_app_error_کد(کد), عدم, 0);
  }
  إذا(cf_ngtcp2_h3_err_is_fatal(کد))
    Curl_cf_ngtcp2_cmn_conn_close(cf, داده);
}

CURLcode Curl_cf_ngtcp2_ctx_init(بنية cf_ngtcp2_ctx *ctx,
                                 بنية Curl_peer *origin,
                                 بنية Curl_peer *peer,
                                 بنية ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = عدم;
  ctx->tunnel_inbuf_len = 0;
  ctx->نسخه_2 = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->جریان_bufcp, H3_جریان_CHUNK_اندازه,
                  H3_جریان_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_سرآیند);
  Curl_u32_ptrset_init(&ctx->streams, h3_جریان_مجموعه_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  أرجع Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

فراغ Curl_cf_ngtcp2_ctx_cleanup(بنية cf_ngtcp2_ctx *ctx)
{
  إذا(ctx && ctx->initialized) {
    إذا(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    إذا(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = عدم;
    }
    إذا(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = عدم;
    }
#اگر_تعریف_شده OPENSSL_QUIC_API2
    إذا(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = عدم;
    }
#پایان_اگر
    Curl_bufcp_free(&ctx->جریان_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    curlx_safefree(ctx->tunnel_inbuf);
    ctx->tunnel_inbuf_len = 0;
  }
}

بنية Curl_easy *Curl_cf_ngtcp2_get_xfer(بنية Curl_cfilter *cf,
                                          فراغ *جریان_user_داده,
                                          بنية h3_جریان_ctx **pstream)
{
  بنية h3_جریان_ctx *جریان = جریان_user_داده;

  إذا(pstream)
    *pstream = جریان;
  إذا(جریان && (جریان->mid != UINT32_MAX)) {
    بنية Curl_easy *داده, *call_داده = CF_داده_CURRENT(cf);
    إذا(!call_داده || !call_داده->multi) {
      DEBUGASSERT(0);
      أرجع عدم;
    }
    داده = Curl_multi_get_easy(call_داده->multi, جریان->mid);
    إذا(داده && (داده->id == جریان->xfer_id))
      أرجع داده;
    DEBUGASSERT(0);
  }
  أرجع عدم;
}

ساكن ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  بنية Curl_cfilter *cf = conn_ref->user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  أرجع ctx->qconn;
}

#اگر_تعریف_شده DEBUG_NGTCP2
ساكن فراغ quic_printf(فراغ *user_داده, ثابت حرف *fmt, ...)
{
  va_list ap;
  (فراغ)user_داده;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#پایان_اگر

ساكن فراغ qlog_callback(فراغ *user_داده, uint32_t flags,
                          ثابت فراغ *داده, size_t datalen)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  (فراغ)flags;
  إذا(ctx->qlogfd != -1) {
    ssize_t rc = اكتب(ctx->qlogfd, داده, datalen);
    إذا(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

ساكن فراغ quic_settings(بنية cf_ngtcp2_ctx *ctx,
                          بنية Curl_easy *داده,
                          بنية cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#اگر_تعریف_شده DEBUG_NGTCP2
  s->log_printf = quic_printf;
#وإلا
  s->log_printf = عدم;
#پایان_اگر

  s->initial_ts = pktx->ts;
  s->handshake_timeout = (داده->مجموعه.connecttimeout > 0) ?
    داده->مجموعه.connecttimeout * NGTCP2_MILLISECONDS : QUIC_HANDSHAKE_TIMEOUT;
  s->max_پنجره = H3_CONN_پنجره_اندازه_MAX;
  s->max_جریان_پنجره = 0; /* disable ngtcp2 auto-tuning of window */
  s->no_pmtud = FALSE;
#اگر_تعریف_شده NGTCP2_SETTINGS_V3
  /* try ten times the ngtcp2 defaults here for problems with Caddy */
  s->glitch_ratelim_مدت = 1000 * 10;
  s->glitch_ratelim_نرخ = 33 * 10;
#پایان_اگر
  t->initial_max_داده = s->max_پنجره;
  t->initial_max_جریان_داده_bidi_local = H3_جریان_پنجره_اندازه_INITIAL;
  t->initial_max_جریان_داده_bidi_remote = H3_جریان_پنجره_اندازه_INITIAL;
  t->initial_max_جریان_داده_uni = t->initial_max_داده;
  t->initial_max_streams_bidi = QUIC_MAX_STREAMS;
  t->initial_max_streams_uni = QUIC_MAX_STREAMS;
  t->max_idle_timeout = 0; /* no idle timeout from our side */
  إذا(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#إذا defined(_MSC_VER) && defined(_DLL)
#رهنمود warning(افزودن)
#رهنمود warning(disable:4232) /* MSVC extension, dllimport identity */
#پایان_اگر

ساكن صحيح cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, فراغ *user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf ? cf->ctx : عدم;
  بنية Curl_easy *داده;

  (فراغ)tconn;
  DEBUGASSERT(ctx);
  داده = CF_داده_CURRENT(cf);
  DEBUGASSERT(داده);
  إذا(!ctx || !داده)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(داده);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, داده);

  ctx->tls_vrfy_نتیجه = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    داده, &ctx->ssl_peer);
  إذا(ctx->tls_vrfy_نتیجه)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

#اگر_تعریف_شده CURLVERBOSE
  إذا(Curl_trc_is_verbose(داده)) {
    ثابت ngtcp2_transport_params *rp;
    rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
    CURL_TRC_CF(داده, cf, "handshake complete after %" FMT_TIMEDIFF_T
                "ms, remote transport[max_udp_payload=%" PRIu64
                ", initial_max_data=%" PRIu64 "]",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at),
                rp->max_udp_payload_اندازه, rp->initial_max_داده);
  }
#پایان_اگر

  /* In case of earlydata, where we simulate being connected, update
   * the handshake time when we really did connect */
  إذا(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrs_زمان_Was(داده, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  إذا(ctx->use_earlydata) {
#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    إذا(SSL_get_early_داده_وضعیت(ctx->tls.ossl.ssl) == SSL_EARLY_داده_NOT_فرستاده)
      CURL_TRC_CF(داده, cf, "OpenSSL did not send early data");
#پایان_اگر

#إذا NGTCP2_نسخه_رقم >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_داده_rejected2(ctx->qconn);
#وإلا /* older NGTCP2 */
    {
#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      صحيح ossl_early_وضعیت = SSL_get_early_داده_وضعیت(ctx->tls.ossl.ssl);
      إذا(ossl_early_وضعیت == SSL_EARLY_داده_NOT_فرستاده)
        CURL_TRC_CF(داده, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_وضعیت == SSL_EARLY_داده_ACCEPTED);
#وإلاإذا defined(USE_GNUTLS)
      صحيح flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_داده);
#وإلاإذا defined(USE_WOLFSSL)
#اگر_تعریف_شده WOLFSSL_EARLY_داده
      ctx->earlydata_accepted =
        (wolf_SSL_get_early_داده_وضعیت(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_داده_REJECTED);
#وإلا
      DEBUGASSERT(0); /* should not come here if ED is disabled. */
      ctx->earlydata_accepted = FALSE;
#پایان_اگر /* WOLFSSL_EARLY_DATA */
#پایان_اگر /* OPENSSL or GNUTLS or WOLFSSL */
    }
#پایان_اگر /* older NGTCP2 */
    CURL_TRC_CF(داده, cf, "server did%s accept %zu bytes of early data",
                ctx->earlydata_accepted ? "" : " not", ctx->earlydata_skip);
    Curl_pgrs_Early_داده(داده, ctx->earlydata_accepted ?
                              (curl_off_t)ctx->earlydata_skip :
                             -(curl_off_t)ctx->earlydata_skip);
  }
  أرجع 0;
}

ساكن صحيح cb_recv_جریان_داده(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t جریان_id, uint64_t جابه_جایی,
                               ثابت uint8_t *buf, size_t buflen,
                               فراغ *user_داده, فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  صحيح fin = (flags & NGTCP2_جریان_داده_FLAG_FIN) ? 1 : 0;
  بنية h3_جریان_ctx *جریان;
  بنية Curl_easy *داده =
    Curl_cf_ngtcp2_get_xfer(cf, جریان_user_داده, &جریان);

  (فراغ)جابه_جایی;
  rc = nghttp3_conn_read_جریان(ctx->h3conn, جریان_id, buf, buflen, fin);
  إذا(rc < 0) {
    إذا(داده && جریان) {
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  جریان_id, جریان->reset, جریان->closed);
    }
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  إذا(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_جریان_جابه_جایی(tconn, جریان_id, nconsumed);
    ngtcp2_conn_extend_max_جابه_جایی(tconn, nconsumed);
    إذا(جریان) {
      جریان->rx_جابه_جایی += nconsumed;
      جریان->rx_جابه_جایی_max += nconsumed;
    }
  }
  أرجع 0;
}

ساكن صحيح cb_acked_جریان_داده_جابه_جایی(ngtcp2_conn *tconn, int64_t جریان_id,
                                       uint64_t جابه_جایی, uint64_t datalen,
                                       فراغ *user_داده, فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)جریان_id;
  (فراغ)tconn;
  (فراغ)جابه_جایی;
  (فراغ)datalen;
  (فراغ)جریان_user_داده;

  rv = nghttp3_conn_جمع_کن_ack_جابه_جایی(ctx->h3conn, جریان_id, datalen);
  إذا(rv && rv != NGHTTP3_ERR_جریان_NOT_یافت) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_جریان_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t جریان_id, uint64_t app_error_کد,
                           فراغ *user_داده, فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده = Curl_cf_ngtcp2_get_xfer(cf, جریان_user_داده, عدم);
  صحيح rv;

  (فراغ)tconn;
  /* stream is closed... */
  إذا(!داده)
    داده = CF_داده_CURRENT(cf);
  إذا(!داده)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  إذا(!(flags & NGTCP2_جریان_CLOSE_FLAG_APP_ERROR_کد_مجموعه)) {
    app_error_کد = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_جریان(ctx->h3conn, جریان_id, app_error_کد);
  CURL_TRC_CF(داده, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", جریان_id, app_error_کد, rv);
  إذا(rv && rv != NGHTTP3_ERR_جریان_NOT_یافت) {
    Curl_cf_ngtcp2_h3_err_مجموعه(cf, داده, rv);
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

#اگر_تعریف_شده NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
ساكن صحيح cb_جریان_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t جریان_id,
                           uint64_t rx_app_error_کد,
                           uint64_t tx_app_error_کد,
                           فراغ *user_داده, فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده = Curl_cf_ngtcp2_get_xfer(cf, جریان_user_داده, عدم);
  uint64_t h3_app_error_کد = NGHTTP3_H3_NO_ERROR;
  صحيح rv;

  (فراغ)tconn;
  (فراغ)tx_app_error_کد;
  /* stream is closed... */
  إذا(!داده)
    داده = CF_داده_CURRENT(cf);
  إذا(!داده)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  إذا(flags & NGTCP2_جریان_CLOSE2_FLAG_RX_APP_ERROR_کد_مجموعه)
    h3_app_error_کد = rx_app_error_کد;

  rv = nghttp3_conn_close_جریان(ctx->h3conn, جریان_id, h3_app_error_کد);
  CURL_TRC_CF(داده, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", جریان_id, h3_app_error_کد, rv);
  إذا(rv && rv != NGHTTP3_ERR_جریان_NOT_یافت) {
    Curl_cf_ngtcp2_h3_err_مجموعه(cf, داده, rv);
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}
#پایان_اگر

ساكن صحيح cb_جریان_reset(ngtcp2_conn *tconn, int64_t جریان_id,
                           uint64_t final_اندازه, uint64_t app_error_کد,
                           فراغ *user_داده, فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده = Curl_cf_ngtcp2_get_xfer(cf, جریان_user_داده, عدم);
  صحيح rv;
  (فراغ)tconn;
  (فراغ)final_اندازه;
  (فراغ)app_error_کد;

  rv = nghttp3_conn_shutdown_جریان_read(ctx->h3conn, جریان_id);
  إذا(داده)
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] reset -> %d", جریان_id, rv);
  إذا(rv && rv != NGHTTP3_ERR_جریان_NOT_یافت) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_جریان_stop_sending(ngtcp2_conn *tconn, int64_t جریان_id,
                                  uint64_t app_error_کد, فراغ *user_داده,
                                  فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)tconn;
  (فراغ)app_error_کد;
  (فراغ)جریان_user_داده;

  rv = nghttp3_conn_shutdown_جریان_read(ctx->h3conn, جریان_id);
  إذا(rv && rv != NGHTTP3_ERR_جریان_NOT_یافت) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            فراغ *user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);

  (فراغ)tconn;
  ctx->max_bidi_streams = max_streams;
  إذا(داده)
    CURL_TRC_CF(داده, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->استفاده_شده_bidi_streams);
  أرجع 0;
}

ساكن صحيح cb_extend_max_جریان_داده(ngtcp2_conn *tconn, int64_t جریان_id,
                                     uint64_t max_داده, فراغ *user_داده,
                                     فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان;
  بنية Curl_easy *داده =
    Curl_cf_ngtcp2_get_xfer(cf, جریان_user_داده, &جریان);
  صحيح rv;
  (فراغ)tconn;
  (فراغ)max_داده;

  rv = nghttp3_conn_unblock_جریان(ctx->h3conn, جریان_id);
  إذا(rv && rv != NGHTTP3_ERR_جریان_NOT_یافت) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  إذا(جریان && جریان->quic_flow_blocked) {
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] unblock quic flow", جریان_id);
    جریان->quic_flow_blocked = FALSE;
    إذا(داده)
      Curl_multi_mark_dirty(داده);
  }
  أرجع 0;
}

ساكن فراغ cb_rand(uint8_t *dest, size_t destlen,
                    ثابت ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode نتیجه;
  (فراغ)rand_ctx;

  نتیجه = Curl_rand(عدم, dest, destlen);
  إذا(نتیجه) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
ساكن صحيح cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    فراغ *user_داده)
{
  CURLcode نتیجه;
  (فراغ)tconn;
  (فراغ)user_داده;

  نتیجه = Curl_rand(عدم, cid->داده, cidlen);
  إذا(نتیجه)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  نتیجه = Curl_rand(عدم, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  إذا(نتیجه)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  أرجع 0;
}

#اگر_تعریف_شده NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
ساكن صحيح cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  بنية ngtcp2_stateless_reset_token *token, size_t cidlen, فراغ *user_داده)
{
  CURLcode نتیجه;
  (فراغ)tconn;
  (فراغ)user_داده;

  نتیجه = Curl_rand(عدم, cid->داده, cidlen);
  إذا(نتیجه)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  نتیجه = Curl_rand(عدم, token->داده, حجم(token->داده));
  إذا(نتیجه)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  أرجع 0;
}
#پایان_اگر

ساكن صحيح cb_recv_rx_کلید(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          فراغ *user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf ? cf->ctx : عدم;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  (فراغ)tconn;

  إذا(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    أرجع 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(داده);
  إذا(ctx && داده && !ctx->h3conn && ctx->init_h3_conn_cb) {
    إذا(ctx->init_h3_conn_cb(cf, داده, ctx))
      أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  أرجع 0;
}

#اگر_تعریف_شده CURL_HAVE_DIAG
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Wmissing-field-initializers"
#پایان_اگر
ساكن ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  عدم, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_داده_cb,
  cb_ngtcp2_handshake_completed,
  عدم, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_ماسک_cb,
  cb_recv_جریان_داده,
  cb_acked_جریان_داده_جابه_جایی,
  عدم, /* stream_open */
  cb_جریان_close,
  عدم, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  عدم, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  عدم, /* remove_connection_id */
  ngtcp2_crypto_update_کلید_cb, /* update_key */
  عدم, /* path_validation */
  عدم, /* select_preferred_addr */
  cb_جریان_reset,
  عدم, /* extend_max_remote_streams_bidi */
  عدم, /* extend_max_remote_streams_uni */
  cb_extend_max_جریان_داده,
  عدم, /* dcid_status */
  عدم, /* handshake_confirmed */
  عدم, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  عدم, /* recv_datagram */
  عدم, /* ack_datagram */
  عدم, /* lost_datagram */
  ngtcp2_crypto_get_مسیر_challenge_داده_cb,
  cb_جریان_stop_sending,
  عدم, /* version_negotiation */
  cb_recv_rx_کلید,
  عدم, /* recv_tx_key */
  عدم, /* early_data_rejected */
#اگر_تعریف_شده NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  عدم, /* begin_path_validation */
#پایان_اگر
#اگر_تعریف_شده NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  عدم, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  عدم, /* dcid_status2 */
  ngtcp2_crypto_get_مسیر_challenge_data2_cb, /* get_path_challenge_data2 */
#پایان_اگر
#اگر_تعریف_شده NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  عدم, /* recv_stop_sending */
#پایان_اگر
#اگر_تعریف_شده NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_جریان_close2, /* is called instead of cb_stream_close when set */
#پایان_اگر
};
#اگر_تعریف_شده CURL_HAVE_DIAG
#رهنمود GCC diagnostic برداشت
#پایان_اگر

#إذا defined(_MSC_VER) && defined(_DLL)
#رهنمود warning(برداشت)
#پایان_اگر

#اگر_تعریف_شده USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ساكن صحيح quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  بنية Curl_cfilter *cf;
  بنية cf_ngtcp2_ctx *ctx;
  بنية Curl_easy *داده;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_داده(ssl);
  cf = cref ? cref->user_داده : عدم;
  ctx = cf ? cf->ctx : عدم;
  داده = cf ? CF_داده_CURRENT(cf) : عدم;
  إذا(cf && داده && ctx) {
    بیعلامت حرف *quic_tp = عدم;
    size_t quic_tp_len = 0;
#اگر_تعریف_شده HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     حجم(tpbuf));
    إذا(tplen < 0)
      CURL_TRC_CF(داده, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((صحيح)tplen));
    وإلا {
      quic_tp = (بیعلامت حرف *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#پایان_اگر
    Curl_ossl_جمع_کن_session(cf, داده, &ctx->tls.ossl, ctx->ssl_peer.scache_کلید,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, عدم);
  }
  أرجع 0;
}
#پایان_اگر /* USE_OPENSSL */

#اگر_تعریف_شده USE_GNUTLS

#اگر_تعریف_شده CURLVERBOSE
ساكن ثابت حرف *gtls_hs_پیام_name(صحيح mtype)
{
  بدل(mtype) {
  حالة 1:
    أرجع "ClientHello";
  حالة 2:
    أرجع "ServerHello";
  حالة 4:
    أرجع "SessionTicket";
  حالة 8:
    أرجع "EncryptedExtensions";
  حالة 11:
    أرجع "Certificate";
  حالة 13:
    أرجع "CertificateRequest";
  حالة 15:
    أرجع "CertificateVerify";
  حالة 20:
    أرجع "Finished";
  حالة 24:
    أرجع "KeyUpdate";
  حالة 254:
    أرجع "MessageHash";
  }
  أرجع "Unknown";
}
#پایان_اگر

ساكن صحيح quic_gtls_handshake_cb(gnutls_session_t session, بیعلامت صحيح htype,
                                  بیعلامت when, بیعلامت صحيح incoming,
                                  ثابت gnutls_datum_t *پیام)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  بنية Curl_cfilter *cf = conn_ref ? conn_ref->user_داده : عدم;
  بنية cf_ngtcp2_ctx *ctx = cf ? cf->ctx : عدم;

  (فراغ)پیام;
  (فراغ)incoming;
  إذا(when && cf && ctx) { /* after message has been processed */
    بنية Curl_easy *داده = CF_داده_CURRENT(cf);
    DEBUGASSERT(داده);
    إذا(!داده)
      أرجع 0;
    CURL_TRC_CF(داده, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_پیام_name(htype), htype);
    بدل(htype) {
    حالة GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      بیعلامت حرف *quic_tp = عدم;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       حجم(tpbuf));
      إذا(tplen < 0)
        CURL_TRC_CF(داده, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحيح)tplen));
      وإلا {
        quic_tp = (بیعلامت حرف *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (فراغ)Curl_gtls_cache_session(cf, داده, ctx->ssl_peer.scache_کلید,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    عدم);
      اكسر;
    }
    افتراضي:
      اكسر;
    }
  }
  أرجع 0;
}
#پایان_اگر /* USE_GNUTLS */

#اگر_تعریف_شده USE_WOLFSSL
ساكن صحيح wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolf_SSL_get_app_داده(ssl);
  بنية Curl_cfilter *cf = conn_ref ? conn_ref->user_داده : عدم;

  DEBUGASSERT(cf);
  إذا(cf && session) {
    بنية cf_ngtcp2_ctx *ctx = cf->ctx;
    بنية Curl_easy *داده = CF_داده_CURRENT(cf);
    DEBUGASSERT(داده);
    إذا(داده && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      بیعلامت حرف *quic_tp = عدم;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       حجم(tpbuf));
      إذا(tplen < 0)
        CURL_TRC_CF(داده, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحيح)tplen));
      وإلا {
        quic_tp = (بیعلامت حرف *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (فراغ)Curl_wssl_cache_session(cf, داده, ctx->ssl_peer.scache_کلید,
                                    session, wolf_SSL_نسخه(ssl),
                                    "h3", quic_tp, quic_tp_len, عدم);
    }
  }
  أرجع 0;
}
#پایان_اگر /* USE_WOLFSSL */

ساكن CURLcode cf_ngtcp2_tls_ctx_راهاندازی(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *داده,
                                        فراغ *user_داده)
{
  بنية curl_tls_ctx *ctx = user_داده;

#اگر_تعریف_شده USE_OPENSSL
#إذا defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  إذا(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(داده, "ngtcp2_crypto_boringssl_configure_client_context failed");
    أرجع CURLE_FAILED_INIT;
  }
#وإلاإذا defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#وإلا
  إذا(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(داده, "ngtcp2_crypto_quictls_configure_client_context failed");
    أرجع CURLE_FAILED_INIT;
  }
#پایان_اگر /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  إذا(Curl_ssl_scache_use(cf, داده)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_مجموعه_session_cache_نما(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_داخلی);
    SSL_CTX_sess_مجموعه_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#وإلاإذا defined(USE_GNUTLS)
  إذا(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(داده, "ngtcp2_crypto_gnutls_configure_client_session failed");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(Curl_ssl_scache_use(cf, داده)) {
    gnutls_handshake_مجموعه_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#وإلاإذا defined(USE_WOLFSSL)
  إذا(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(داده, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(Curl_ssl_scache_use(cf, داده)) {
    /* Register to get notified when a new session is received */
    wolf_SSL_CTX_sess_مجموعه_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#پایان_اگر
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ngtcp2_on_session_reuse(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *داده,
                                           بنية alpn_spec *alpns,
                                           بنية Curl_ssl_session *scs,
                                           bool *do_early_داده)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  *do_early_داده = FALSE;
#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#پایان_اگر
#اگر_تعریف_شده USE_GNUTLS
  ctx->earlydata_max =
    gnutls_رکورد_get_max_early_داده_اندازه(ctx->tls.gtls.session);
#پایان_اگر
#اگر_تعریف_شده USE_WOLFSSL
#اگر_تعریف_شده WOLFSSL_EARLY_داده
  ctx->earlydata_max = scs->earlydata_max;
#وإلا
  ctx->earlydata_max = 0;
#پایان_اگر /* WOLFSSL_EARLY_DATA */
#پایان_اگر
#إذا defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  إذا(!ctx->earlydata_max) {
    CURL_TRC_CF(داده, cf, "SSL session does not allow earlydata");
  }
  وإلا إذا(!Curl_alpn_شامل_است_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(داده, cf, "SSL session from different ALPN, no early data");
  }
  وإلا إذا(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(داده, cf, "no 0RTT transport parameters, no early data");
  }
  وإلا {
    صحيح rv;
    rv = ngtcp2_conn_decode_and_مجموعه_0rtt_transport_params(
      ctx->qconn, (ثابت uint8_t *)scs->quic_tp, scs->quic_tp_len);
    إذا(rv)
      CURL_TRC_CF(داده, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    وإلا إذا(ctx->init_h3_conn_cb) {
      infof(داده, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      نتیجه = ctx->init_h3_conn_cb(cf, داده, ctx);
      إذا(!نتیجه) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_داده = TRUE;
      }
    }
    وإلا { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_داده = TRUE;
    }
  }
#وإلا /* not supported in the TLS backend */
  (فراغ)داده;
  (فراغ)ctx;
  (فراغ)scs;
  (فراغ)alpns;
#پایان_اگر
  أرجع نتیجه;
}

/*
 * Might be called twice for happy eyeballs.
 */
ساكن CURLcode cf_connect_شروع(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rc;
  صحيح rv;
  CURLcode نتیجه;
  ثابت بنية Curl_sockaddr_ex *sockaddr = عدم;
  صحيح qfd;
  ساكن ثابت بنية alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  نتیجه = Curl_rand(داده, ctx->dcid.داده, NGTCP2_MAX_CIDLEN);
  إذا(نتیجه)
    أرجع نتیجه;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  نتیجه = Curl_rand(داده, ctx->scid.داده, NGTCP2_MAX_CIDLEN);
  إذا(نتیجه)
    أرجع نتیجه;

  (فراغ)Curl_qlogdir(داده, ctx->scid.داده, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, داده, pktx);

  نتیجه = Curl_vquic_ctx_init(داده, &ctx->q);
  إذا(نتیجه)
    أرجع نتیجه;

  /* Query socket and remote address from sub-chain */
  إذا(Curl_cf_socket_peek(cf->بعدی, داده, &ctx->q.sockfd, &sockaddr, عدم)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  إذا(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = حجم(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (بنية sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    إذا(rv == -1)
      أرجع CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_مسیر.local,
                     (بنية sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_مسیر.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  وإلا {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ثابت بنية Curl_sockaddr_ex *remote = عدم;
    إذا(cf->بعدی->cft->query(cf->بعدی, داده, CF_QUERY_REMOTE_ADDR, عدم,
                            CURL_UNCONST(&remote)))
      أرجع CURLE_QUIC_CONNECT_ERROR;
    إذا(!remote)
      أرجع CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, حجم(ctx->q.local_addr));
    بدل(remote->family) {
    حالة AF_INET:
      ((بنية sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = حجم(بنية sockaddr_in);
      اكسر;
#اگر_تعریف_شده USE_IPV6
    حالة AF_INET6:
      ((بنية sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = حجم(بنية sockaddr_in6);
      اكسر;
#پایان_اگر
    افتراضي:
      أرجع CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_مسیر.local,
                     (بنية sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_مسیر.remote,
                     &remote->curl_sa_addr,
                     (socklen_t)remote->addrlen);
  }

  rc = ngtcp2_conn_client_new(&ctx->qconn, &ctx->dcid, &ctx->scid,
                              &ctx->connected_مسیر,
                              NGTCP2_PROTO_VER_V1, &ng_callbacks,
                              &ctx->settings, &ctx->transport_params,
                              Curl_ngtcp2_mem(), cf);
  إذا(rc)
    أرجع CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_داده = cf;

  نتیجه = Curl_vquic_tls_init(&ctx->tls, cf, داده,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_راهاندازی, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  إذا(نتیجه)
    أرجع نتیجه;

#إذا defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  إذا(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(داده, "ngtcp2_crypto_ossl_ctx_new failed");
    أرجع CURLE_FAILED_INIT;
  }
  ngtcp2_conn_مجموعه_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  إذا(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(داده, "ngtcp2_crypto_ossl_configure_client_session failed");
    أرجع CURLE_FAILED_INIT;
  }
#وإلاإذا defined(USE_OPENSSL)
  SSL_مجموعه_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_مجموعه_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#وإلاإذا defined(USE_GNUTLS)
  ngtcp2_conn_مجموعه_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#وإلاإذا defined(USE_WOLFSSL)
  ngtcp2_conn_مجموعه_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#وإلا
#خطا "ngtcp2 TLS backend not defined"
#پایان_اگر

#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  إذا(ctx->use_earlydata &&
     !SSL_مجموعه_quic_tls_early_داده_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(داده, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#پایان_اگر
  ngtcp2_ccerr_default(&ctx->آخرین_error);

  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    bool *انجامشده)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;
  بنية cf_call_داده save;
  بنية cf_ngtcp2_io_ctx pktx;

  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the sub-chain */
  إذا(cf->بعدی && !cf->بعدی->connected) {
    نتیجه = Curl_conn_cf_connect(cf->بعدی, داده, انجامشده);
    إذا(نتیجه || !*انجامشده)
      أرجع نتیجه;
  }

  *انجامشده = FALSE;

  إذا(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(داده, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(داده, cf, "need HTTPS-RR, delaying connect");
    أرجع CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, داده);
  CF_داده_SAVE(save, cf, داده);

  إذا(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(داده);
    نتیجه = cf_connect_شروع(cf, داده, &pktx);
    إذا(نتیجه)
      اذهب بیرون;
    إذا(cf->connected) {
      *انجامشده = TRUE;
      اذهب بیرون;
    }
    نتیجه = Curl_cf_ngtcp2_پیشرفت_egress(cf, داده, &pktx);
    /* we do not expect to be able to recv anything yet */
    اذهب بیرون;
  }

  نتیجه = Curl_cf_ngtcp2_پیشرفت_ingress(cf, داده, &pktx);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = Curl_cf_ngtcp2_پیشرفت_egress(cf, داده, &pktx);
  إذا(نتیجه)
    اذهب بیرون;

  إذا(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    نتیجه = ctx->tls_vrfy_نتیجه;
    إذا(!نتیجه) {
      CURL_TRC_CF(داده, cf, "peer verified");
      cf->connected = TRUE;
      *انجامشده = TRUE;
    }
  }

بیرون:
  إذا(ctx->tls_vrfy_نتیجه)
    نتیجه = ctx->tls_vrfy_نتیجه;
  إذا(ctx->qconn &&
     ((نتیجه == CURLE_RECV_ERROR) || (نتیجه == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_درون_draining_period(ctx->qconn)) {
    ثابت ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    نتیجه = CURLE_COULDNT_CONNECT;
    إذا(cerr) {
      CURL_TRC_CF(داده, cf, "connect error, type=%d, code=%" PRIu64,
                  (صحيح)cerr->type, cerr->error_کد);
      بدل(cerr->type) {
      حالة NGTCP2_CCERR_TYPE_نسخه_NEGOTIATION:
        CURL_TRC_CF(داده, cf, "error in version negotiation");
        اكسر;
      افتراضي:
        إذا(cerr->error_کد >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(داده, cf, "crypto error, tls alert=%u",
                      (بیعلامت صحيح)(cerr->error_کد & 0xffU));
        }
        وإلا إذا(cerr->error_کد == NGTCP2_CONNECTION_REFUSED) {
          CURL_TRC_CF(داده, cf, "connection refused by server");
          /* When a QUIC server instance is shutting down, it may send us a
           * CONNECTION_CLOSE with this code right away. We want
           * to keep on trying in this case. */
          نتیجه = CURLE_WEIRD_SERVER_پاسخ;
        }
      }
    }
  }

#اگر_تعریف_شده CURLVERBOSE
  إذا(نتیجه) {
    إذا(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      بنية ip_quadruple ip;

      إذا(!Curl_cf_socket_peek(cf->بعدی, داده, عدم, عدم, &ip))
        infof(داده, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_پورت, curl_easy_strerror(نتیجه));
    }
  }
#پایان_اگر
  إذا(!نتیجه && ctx->qconn) {
    نتیجه = Curl_cf_ngtcp2_cmn_مجموعه_expiry(cf, داده, &pktx);
  }
  إذا(نتیجه || *انجامشده)
    CURL_TRC_CF(داده, cf, "connect -> %d, done=%d", (صحيح)نتیجه, *انجامشده);
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *داده, bool *انجامشده)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_call_داده save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode نتیجه = CURLE_OK;

  إذا(cf->shutdown || !ctx->qconn) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  إذا(!cf->بعدی) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  CF_داده_SAVE(save, cf, داده);
  *انجامشده = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, داده);

  إذا(!ctx->shutdown_started) {
    حرف میانگیر[NGTCP2_MAX_UDP_PAYLOAD_اندازه];
    ngtcp2_ssize nwritten;

    إذا(!Curl_bufq_is_خالی(&ctx->q.sendbuf)) {
      CURL_TRC_CF(داده, cf, "shutdown, flushing sendbuf");
      نتیجه = Curl_cf_ngtcp2_پیشرفت_egress(cf, داده, &pktx);
      إذا(!Curl_bufq_is_خالی(&ctx->q.sendbuf)) {
        CURL_TRC_CF(داده, cf, "sending shutdown packets blocked");
        نتیجه = CURLE_OK;
        اذهب بیرون;
      }
      وإلا إذا(نتیجه) {
        CURL_TRC_CF(داده, cf, "shutdown, error %d flushing sendbuf",
                    (صحيح)نتیجه);
        *انجامشده = TRUE;
        اذهب بیرون;
      }
    }

    DEBUGASSERT(Curl_bufq_is_خالی(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, عدم, /*مسیر*/
      عدم, /* pkt_info */
      (uint8_t *)میانگیر, حجم(میانگیر),
      &ctx->آخرین_error, pktx.ts);
    CURL_TRC_CF(داده, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (صحيح)ctx->آخرین_error.type,
                ctx->آخرین_error.error_کد, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    إذا(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      نتیجه = Curl_bufq_write(&ctx->q.sendbuf, (ثابت بیعلامت حرف *)میانگیر,
                               (size_t)nwritten, &n);
      إذا(نتیجه) {
        CURL_TRC_CF(داده, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (صحيح)نتیجه);
        اذهب بیرون;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.تقسیم_len = 0;
    }
  }

  إذا(!Curl_bufq_is_خالی(&ctx->q.sendbuf)) {
    CURL_TRC_CF(داده, cf, "shutdown, flushing egress");
    نتیجه = Curl_vquic_flush(cf, داده, &ctx->q);
    إذا(نتیجه == CURLE_AGAIN) {
      CURL_TRC_CF(داده, cf, "sending shutdown packets blocked");
      نتیجه = CURLE_OK;
      اذهب بیرون;
    }
    وإلا إذا(نتیجه) {
      CURL_TRC_CF(داده, cf, "shutdown, error %d flushing sendbuf",
                  (صحيح)نتیجه);
      *انجامشده = TRUE;
      اذهب بیرون;
    }
  }

  إذا(Curl_bufq_is_خالی(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(داده, cf, "shutdown completely sent off, done");
    *انجامشده = TRUE;
    نتیجه = CURLE_OK;
  }
بیرون:
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

فراغ Curl_cf_ngtcp2_cmn_conn_close(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
  bool انجامشده;
  Curl_cf_ngtcp2_cmn_shutdown(cf, داده, &انجامشده);
}

ساكن bool cf_ngtcp2_err_is_fatal(صحيح کد)
{
  أرجع (NGTCP2_ERR_FATAL >= کد) ||
         (NGTCP2_ERR_DROP_CONN == کد) ||
         (NGTCP2_ERR_IDLE_CLOSE == کد);
}

فراغ Curl_cf_ngtcp2_cmn_err_مجموعه(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده, صحيح کد)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  إذا(!ctx->آخرین_error.error_کد) {
    إذا(NGTCP2_ERR_CRYPTO == کد) {
      ngtcp2_ccerr_مجموعه_tls_alert(&ctx->آخرین_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 عدم, 0);
    }
    وإلا {
      ngtcp2_ccerr_مجموعه_liberr(&ctx->آخرین_error, کد, عدم, 0);
    }
  }
  إذا(cf_ngtcp2_err_is_fatal(کد))
    Curl_cf_ngtcp2_cmn_conn_close(cf, داده);
}

فراغ Curl_cf_ngtcp2_io_ctx_init(بنية cf_ngtcp2_io_ctx *io_ctx,
                                بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->داده = داده;
  io_ctx->now = *Curl_pgrs_now(داده);
  ngtcp2_مسیر_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_مجموعه_زمان(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

فراغ Curl_cf_ngtcp2_io_ctx_update_زمان(بنية Curl_easy *داده,
                                       بنية cf_ngtcp2_io_ctx *io_ctx,
                                       بنية Curl_cfilter *cf)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(داده);
  Curl_vquic_ctx_update_زمان(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#إذا NGTCP2_نسخه_رقم < 0x011100
بنية cf_ngtcp2_sfind_ctx {
  int64_t جریان_id;
  بنية h3_جریان_ctx *جریان;
  uint32_t mid;
};

ساكن bool cf_ngtcp2_sfind(uint32_t mid, فراغ *مقدار, فراغ *user_داده)
{
  بنية cf_ngtcp2_sfind_ctx *fctx = user_داده;
  بنية h3_جریان_ctx *جریان = مقدار;

  إذا(fctx->جریان_id == جریان->id) {
    fctx->mid = mid;
    fctx->جریان = جریان;
    أرجع FALSE;
  }
  أرجع TRUE; /* continue */
}

ساكن بنية h3_جریان_ctx *cf_ngtcp2_get_جریان(بنية cf_ngtcp2_ctx *ctx,
                                                  int64_t جریان_id)
{
  بنية cf_ngtcp2_sfind_ctx fctx;
  fctx.جریان_id = جریان_id;
  fctx.جریان = عدم;
  Curl_u32_ptrset_بازدید(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  أرجع fctx.جریان;
}
#وإلا
ساكن بنية h3_جریان_ctx *cf_ngtcp2_get_جریان(بنية cf_ngtcp2_ctx *ctx,
                                                  int64_t جریان_id)
{
  أرجع ngtcp2_conn_get_جریان_user_داده(ctx->qconn, جریان_id);
}
#پایان_اگر

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
ساكن CURLcode read_pkt_مقصد_send(فراغ *userp,
                                 بیعلامت حرف *buf, size_t buflen,
                                 size_t *pnread)
{
  بنية cf_ngtcp2_io_ctx *x = userp;
  بنية cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t جریان_id;
  صحيح fin;
  ssize_t n;

  *pnread = 0;
  veccnt = 0;
  جریان_id = -1;
  fin = 0;

  /* ngtcp2 may want to put several frames from different streams into
   * this packet. `NGTCP2_WRITE_STREAM_FLAG_MORE` tells it to do so.
   * When `NGTCP2_ERR_WRITE_MORE` is returned, we *need* to make
   * another iteration.
   * When ngtcp2 is happy (because it has no other frame that would fit
   * or it has nothing more to send), it returns the total length
   * of the assembled packet. This may be 0 if there was nothing to send. */
  لكل(;;) {

    إذا(ctx->h3conn && ngtcp2_conn_get_max_داده_چپ(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_جریان(ctx->h3conn, &جریان_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      إذا(veccnt < 0) {
        failf(x->داده, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((صحيح)veccnt));
        Curl_cf_ngtcp2_h3_err_مجموعه(x->cf, x->داده, (صحيح)veccnt);
        أرجع CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_جریان_FLAG_بیشتر |
            (fin ? NGTCP2_WRITE_جریان_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_جریان(ctx->qconn, &x->ps.مسیر,
                                  عدم, buf, buflen,
                                  &ndatalen, flags, جریان_id,
                                  (ثابت ngtcp2_vec *)vec, veccnt, x->ts);
    إذا(n == 0) {
      /* nothing to send */
      أرجع CURLE_AGAIN;
    }
    وإلا إذا(n < 0) {
      بدل(n) {
      حالة NGTCP2_ERR_جریان_داده_BLOCKED: {
        بنية h3_جریان_ctx *جریان;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_بلوک_جریان(ctx->h3conn, جریان_id);
        CURL_TRC_CF(x->داده, x->cf, "[%" PRId64 "] block quic flow",
                    جریان_id);
        جریان = cf_ngtcp2_get_جریان(ctx, جریان_id);
        إذا(جریان) /* it might be not one of our h3 streams? */
          جریان->quic_flow_blocked = TRUE;
        n = 0;
        اكسر;
      }
      حالة NGTCP2_ERR_جریان_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_جریان_write(ctx->h3conn, جریان_id);
        n = 0;
        اكسر;
      حالة NGTCP2_ERR_WRITE_بیشتر:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        اكسر;
      افتراضي:
        DEBUGASSERT(ndatalen == -1);
        failf(x->داده, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((صحيح)n));
        Curl_cf_ngtcp2_cmn_err_مجموعه(x->cf, x->داده, (صحيح)n);
        أرجع CURLE_SEND_ERROR;
      }
    }

    إذا(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      صحيح rv = nghttp3_conn_جمع_کن_write_جابه_جایی(ctx->h3conn, جریان_id, ndatalen);
      إذا(rv) {
        failf(x->داده, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        أرجع CURLE_SEND_ERROR;
      }
    }

    إذا(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      أرجع CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_پیشرفت_egress(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *داده,
                                        بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_اندازه, مسیر_max_payload_اندازه;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_برش_زمانی;
  CURLcode نتیجه;
  بنية cf_ngtcp2_io_ctx local_pktx;

  إذا(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, داده);
    pktx = &local_pktx;
  }
  وإلا {
    Curl_cf_ngtcp2_io_ctx_update_زمان(داده, pktx, cf);
    ngtcp2_مسیر_storage_zero(&pktx->ps);
  }

  نتیجه = Curl_vquic_flush(cf, داده, &ctx->q);
  إذا(نتیجه) {
    إذا(نتیجه == CURLE_AGAIN) {
      Curl_expire_مجموعه(داده, EXPIRE_QUIC, 1, &pktx->now);
      أرجع CURLE_OK;
    }
    أرجع نتیجه;
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
  max_payload_اندازه = ngtcp2_conn_get_max_tx_udp_payload_اندازه(ctx->qconn);
  مسیر_max_payload_اندازه =
    ngtcp2_conn_get_مسیر_max_tx_udp_payload_اندازه(ctx->qconn);
  send_برش_زمانی = ngtcp2_conn_get_send_برش_زمانی(ctx->qconn);
  CURL_TRC_CF(داده, cf, "egress, collect and send packets, quantum=%zu",
              send_برش_زمانی);
  لكل(;;) {
    /* add the next packet to send, if any, to our buffer */
    نتیجه = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_اندازه,
                            read_pkt_مقصد_send, pktx, &nread);
    إذا(نتیجه == CURLE_AGAIN)
      اكسر;
    وإلا إذا(نتیجه)
      أرجع نتیجه;
    وإلا {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      إذا((buflen >= send_برش_زمانی) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_اندازه))
        اكسر;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      إذا(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      وإلا إذا(nread > gsolen ||
              (gsolen > مسیر_max_payload_اندازه && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        نتیجه = Curl_vquic_send_دم_تقسیم(cf, داده, &ctx->q,
                                            gsolen, nread, nread);
        إذا(نتیجه) {
          إذا(نتیجه == CURLE_AGAIN) {
            Curl_expire_مجموعه(داده, EXPIRE_QUIC, 1, &pktx->now);
            أرجع CURLE_OK;
          }
          أرجع نتیجه;
        }
        pktcnt = 0;
      }
      وإلا إذا(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        اكسر;
      }
    }
  }

  إذا(!Curl_bufq_is_خالی(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(داده, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    نتیجه = Curl_vquic_send(cf, داده, &ctx->q, gsolen);
    إذا(نتیجه) {
      إذا(نتیجه == CURLE_AGAIN) {
        Curl_expire_مجموعه(داده, EXPIRE_QUIC, 1, &pktx->now);
        أرجع CURLE_OK;
      }
      أرجع نتیجه;
    }
    Curl_cf_ngtcp2_io_ctx_update_زمان(داده, pktx, cf);
    ngtcp2_conn_update_pkt_tx_زمان(ctx->qconn, pktx->ts);
  }
  أرجع CURLE_OK;
}

بنية cf_ngtcp2_recv_ctx {
  بنية cf_ngtcp2_io_ctx *pktx;
  size_t pkt_شمار;
};

ساكن CURLcode cf_ngtcp2_recv_pkts(ثابت بیعلامت حرف *buf, size_t buflen,
                                    size_t gso_اندازه,
                                    بنية sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    فراغ *userp)
{
  بنية cf_ngtcp2_recv_ctx *rctx = userp;
  بنية cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  بنية cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_مسیر مسیر;
  size_t جابه_جایی, pktlen;
  صحيح rv;

  إذا(!rctx->pkt_شمار) {
    Curl_cf_ngtcp2_io_ctx_update_زمان(pktx->داده, pktx, pktx->cf);
    ngtcp2_مسیر_storage_zero(&pktx->ps);
  }

  إذا(ecn)
    CURL_TRC_CF(pktx->داده, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_اندازه, (بیعلامت)ecn);
  ngtcp2_addr_init(&مسیر.local, (بنية sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&مسیر.remote, (بنية sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  لكل(جابه_جایی = 0; جابه_جایی < buflen; جابه_جایی += gso_اندازه) {
    rctx->pkt_شمار++;
    pktlen = ((جابه_جایی + gso_اندازه) <= buflen) ? gso_اندازه : (buflen - جابه_جایی);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &مسیر, &pi,
                              buf + جابه_جایی, pktlen, pktx->ts);
    إذا(rv) {
      CURL_TRC_CF(pktx->داده, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_مجموعه(pktx->cf, pktx->داده, rv);

      إذا(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        أرجع CURLE_PEER_FAILED_VERIFICATION;
      أرجع CURLE_RECV_ERROR;
    }
  }
  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_پیشرفت_ingress(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *داده,
                                         بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_ngtcp2_io_ctx local_pktx;
  بنية cf_ngtcp2_recv_ctx rctx;
  CURLcode نتیجه = CURLE_OK;

  إذا(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, داده);
    pktx = &local_pktx;
  }

  نتیجه = Curl_vquic_tls_before_recv(&ctx->tls, cf, داده);
  إذا(نتیجه)
    أرجع نتیجه;

  rctx.pktx = pktx;
  rctx.pkt_شمار = 0;

  إذا(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(داده, cf, "progress_ingress(socket)");
    أرجع Curl_vquic_recv_packets(cf, داده, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  وإلا {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    بیعلامت حرف *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_اندازه;
    size_t pkt_حد = QUIC_TUNNEL_INGRESS_PKT_حد;
    size_t nread;
    بنية sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(داده, cf, "progress_ingress(sub-filters)");
    إذا(ctx->qconn) {
      size_t max_مسیر_payload;
      max_مسیر_payload =
        ngtcp2_conn_get_مسیر_max_tx_udp_payload_اندازه(ctx->qconn);
      إذا(max_مسیر_payload > max_udp_payload)
        max_udp_payload = max_مسیر_payload;
    }

    إذا(ctx->tunnel_inbuf_len < max_udp_payload) {
      بیعلامت حرف *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      إذا(!newbuf)
        أرجع CURLE_بیرون_از_حافظه;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    طالما(pkt_حد--) {
      نتیجه = Curl_conn_cf_recv(cf->بعدی, داده, (حرف *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      إذا(نتیجه == CURLE_AGAIN) {
        /* no more data available at the moment */
        أرجع CURLE_OK;
      }
      إذا(نتیجه) {
        CURL_TRC_CF(داده, cf, "ingress, recv from tunnel failed: %d",
                    (صحيح)نتیجه);
        أرجع نتیجه;
      }
      إذا(nread == 0) {
        /* tunnel closed */
        أرجع CURLE_OK;
      }

      کپی_حافظه(&remote_addr, ctx->connected_مسیر.remote.addr,
             ctx->connected_مسیر.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_مسیر.remote.addrlen;
      نتیجه = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      إذا(نتیجه)
        أرجع نتیجه;

      إذا(!ctx->q.got_اولی_byte) {
        ctx->q.got_اولی_byte = TRUE;
        ctx->q.اولی_byte_at = ctx->q.آخرین_op;
      }
      ctx->q.آخرین_io = ctx->q.آخرین_op;
    }
    أرجع CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_مجموعه_expiry(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *داده,
                                       بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  إذا(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, داده);
    pktx = &local_pktx;
  }
  وإلا {
    Curl_cf_ngtcp2_io_ctx_update_زمان(داده, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  إذا(expiry != UINT64_MAX) {
    إذا(expiry <= pktx->ts) {
      CURLcode نتیجه;
      صحيح rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      إذا(rv) {
        failf(داده, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_مجموعه(cf, داده, rv);
        أرجع CURLE_SEND_ERROR;
      }
      نتیجه = Curl_cf_ngtcp2_پیشرفت_ingress(cf, داده, pktx);
      إذا(!نتیجه)
        نتیجه = Curl_cf_ngtcp2_پیشرفت_egress(cf, داده, pktx);
      إذا(نتیجه) {
        /* a verify failure during ingress must win over generic errors */
        إذا(ctx->tls_vrfy_نتیجه)
          نتیجه = ctx->tls_vrfy_نتیجه;
        أرجع نتیجه;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    إذا(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      إذا(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_مجموعه(داده, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  أرجع CURLE_OK;
}

ساكن فراغ cf_ngtcp2_راهاندازی_keep_alive(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *داده)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
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
  إذا(!ctx->qconn)
    أرجع;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  إذا(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_مجموعه_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(داده, cf, "no peer idle timeout, unset keep-alive");
  }
  وإلا إذا(!Curl_u32_ptrset_شمار(&ctx->streams)) {
    ngtcp2_conn_مجموعه_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(داده, cf, "no active streams, unset keep-alive");
  }
  وإلا {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_مجموعه_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(داده, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_جریان_راهاندازی(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *داده,
                                        بنية h3_جریان_ctx **pstream)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);

  *pstream = عدم;
  إذا(!داده)
    أرجع CURLE_FAILED_INIT;

  إذا(جریان) {
    *pstream = جریان;
    أرجع CURLE_OK;
  }

  جریان = curlx_calloc(1, حجم(*جریان));
  إذا(!جریان)
    أرجع CURLE_بیرون_از_حافظه;

  جریان->id = -1;
  جریان->xfer_id = داده->id;
  جریان->mid = داده->mid;
  جریان->rx_جابه_جایی = 0;
  جریان->rx_جابه_جایی_max = H3_جریان_پنجره_اندازه_INITIAL;
  جریان->tx_درون_flight_ideal = H3_جریان_SEND_BUF_INITIAL;

  /* on send, we control how much we put into the buffer */
  Curl_bufq_initp(&جریان->sendbuf, &ctx->جریان_bufcp,
                  H3_جریان_SEND_CHUNKS, BUFQ_OPT_NONE);
  جریان->tx_درون_flight_اندازه = 0;
  Curl_h1_req_parse_init(&جریان->h1, H1_PARSE_DEFAULT_MAX_سطر_LEN);

  إذا(Curl_u32_ptrset_مجموعه(&ctx->streams, داده->mid, جریان)) {
    Curl_cf_ngtcp2_h3_جریان_ctx_free(جریان);
    أرجع CURLE_بیرون_از_حافظه;
  }

  إذا(Curl_u32_ptrset_شمار(&ctx->streams) == 1)
    cf_ngtcp2_راهاندازی_keep_alive(cf, داده);

  *pstream = جریان;
  أرجع CURLE_OK;
}

فراغ Curl_cf_ngtcp2_h3_جریان_close(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    بنية h3_جریان_ctx *جریان)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(داده);
  DEBUGASSERT(جریان);
  إذا(!جریان->closed && ctx->qconn && ctx->h3conn) {
    CURLcode نتیجه;

    nghttp3_conn_مجموعه_جریان_user_داده(ctx->h3conn, جریان->id, عدم);
    ngtcp2_conn_مجموعه_جریان_user_داده(ctx->qconn, جریان->id, عدم);
    جریان->closed = TRUE;
    (فراغ)ngtcp2_conn_shutdown_جریان(ctx->qconn, 0, جریان->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    نتیجه = Curl_cf_ngtcp2_پیشرفت_egress(cf, داده, عدم);
    إذا(نتیجه)
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] cancel stream -> %d",
                  جریان->id, (صحيح)نتیجه);
  }
}

فراغ Curl_cf_ngtcp2_h3_جریان_انجامشده(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);
  (فراغ)cf;
  إذا(جریان) {
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] easy handle is done", جریان->id);
    Curl_cf_ngtcp2_h3_جریان_close(cf, داده, جریان);
    Curl_u32_ptrset_unset(&ctx->streams, داده->mid);
    إذا(!Curl_u32_ptrset_شمار(&ctx->streams))
      cf_ngtcp2_راهاندازی_keep_alive(cf, داده);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده,
                                      bool *درون_داد_pending)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ثابت ngtcp2_transport_params *rp;
  بنية cf_call_داده save;

  CF_داده_SAVE(save, cf, داده);
  *درون_داد_pending = FALSE;
  إذا(!ctx->qconn || ctx->shutdown_started)
    اذهب بیرون;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  إذا(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(داده), &ctx->q.آخرین_io);
    إذا(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      إذا((uint64_t)idletime_ms > max_idle_ms)
        اذهب بیرون;
    }
  }

  إذا(!cf->بعدی || !cf->بعدی->cft->is_alive(cf->بعدی, داده, درون_داد_pending))
    اذهب بیرون;

  alive = TRUE;
  إذا(*درون_داد_pending) {
    CURLcode نتیجه;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *درون_داد_pending = FALSE;
    نتیجه = Curl_cf_ngtcp2_پیشرفت_ingress(cf, داده, عدم);
    CURL_TRC_CF(داده, cf, "is_alive, progress ingress -> %d", (صحيح)نتیجه);
    alive = نتیجه ? FALSE : TRUE;
  }

بیرون:
  CF_داده_RESTORE(cf, save);
  أرجع alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(بنية cf_ngtcp2_ctx *ctx,
                                      بنية Curl_easy *داده)
{
  int64_t ctrl_جریان_id, qpack_enc_جریان_id, qpack_dec_جریان_id;
  صحيح rc;

  rc = ngtcp2_conn_open_uni_جریان(ctx->qconn, &ctrl_جریان_id, عدم);
  إذا(rc) {
    failf(داده, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_جریان(ctx->h3conn, ctrl_جریان_id);
  إذا(rc) {
    failf(داده, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_جریان(ctx->qconn, &qpack_enc_جریان_id, عدم);
  إذا(rc) {
    failf(داده, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_جریان(ctx->qconn, &qpack_dec_جریان_id, عدم);
  إذا(rc) {
    failf(داده, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_جریان_id,
                                       qpack_dec_جریان_id);
  إذا(rc) {
    failf(داده, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_CONNECT_پاسخ_MS:
    إذا((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_اولی_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.اولی_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < بیشترین_صحیح) ? (صحيح)ms : بیشترین_صحیح;
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CF_QUERY_REALLY_CONNECTED:
    إذا(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_اولی_byte;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع cf->بعدی ?
    cf->بعدی->cft->query(cf->بعدی, داده, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_REPORT_STATS:
    إذا(cf->connected && !ctx->stats_reported) {
      إذا((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_اولی_byte) {
        Curl_pgrs_زمان_Was(داده, TIMER_CONNECT, ctx->q.اولی_byte_at);
        ctx->stats_reported = TRUE;
      }
      وإلا إذا(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        إذا(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrs_زمان_Was(داده, TIMER_CONNECT, ctx->q.اولی_byte_at);
        Curl_pgrs_زمان_Was(داده, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتیجه;
}

#پایان_اگر /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
