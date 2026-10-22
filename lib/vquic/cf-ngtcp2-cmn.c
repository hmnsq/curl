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

#جے_تعریف_شدہ USE_OPENSSL
#ضمن <openssl/err.h>
#إذا defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ضمن <ngtcp2/ngtcp2_crypto_boringssl.h>
#وإلاإذا defined(OPENSSL_QUIC_API2)
#ضمن <ngtcp2/ngtcp2_crypto_ossl.h>
#وإلا
#ضمن <ngtcp2/ngtcp2_crypto_quictls.h>
#جے_انت
#ضمن "vtls/openssl.h"
#وإلاإذا defined(USE_GNUTLS)
#ضمن <ngtcp2/ngtcp2_crypto_gnutls.h>
#ضمن "vtls/gtls.h"
#وإلاإذا defined(USE_WOLFSSL)
#ضمن <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ضمن "vtls/wolfssl.h"
#جے_انت

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
  ثابت ngtcp2_info *ng2 = ngtcp2_نسخہ(0);
  ثابت nghttp3_info *ht3 = nghttp3_نسخہ(0);
  (فراغ)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->نسخہ_str, ht3->نسخہ_str);
}

فراغ Curl_cf_ngtcp2_h3_دھارا_ctx_free(بنية h3_دھارا_ctx *دھارا)
{
  Curl_bufq_free(&دھارا->sendbuf);
  Curl_h1_req_parse_free(&دھارا->h1);
  curlx_free(دھارا);
}

ساكن فراغ h3_دھارا_سیٹ_free(uint32_t id, فراغ *دھارا)
{
  (فراغ)id;
  DEBUGASSERT(دھارا);
  Curl_cf_ngtcp2_h3_دھارا_ctx_free((بنية h3_دھارا_ctx *)دھارا);
}

ساكن bool cf_ngtcp2_h3_err_is_fatal(صحيح کوڈ)
{
  أرجع (NGHTTP3_ERR_FATAL >= کوڈ) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_دھارا == کوڈ);
}

فراغ Curl_cf_ngtcp2_h3_err_سیٹ(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے, صحيح کوڈ)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  إذا(!ctx->آخری_error.error_کوڈ) {
    ngtcp2_ccerr_سیٹ_application_error(&ctx->آخری_error,
      nghttp3_err_infer_quic_app_error_کوڈ(کوڈ), عدم, 0);
  }
  إذا(cf_ngtcp2_h3_err_is_fatal(کوڈ))
    Curl_cf_ngtcp2_cmn_conn_close(cf, انکڑے);
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
  ctx->نسخہ_2 = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->دھارا_bufcp, H3_دھارا_CHUNK_ناپ,
                  H3_دھارا_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_سرخی);
  Curl_u32_ptrset_init(&ctx->streams, h3_دھارا_سیٹ_free);
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
#جے_تعریف_شدہ OPENSSL_QUIC_API2
    إذا(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = عدم;
    }
#جے_انت
    Curl_bufcp_free(&ctx->دھارا_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    curlx_safefree(ctx->tunnel_inbuf);
    ctx->tunnel_inbuf_len = 0;
  }
}

بنية Curl_easy *Curl_cf_ngtcp2_get_xfer(بنية Curl_cfilter *cf,
                                          فراغ *دھارا_user_انکڑے,
                                          بنية h3_دھارا_ctx **pstream)
{
  بنية h3_دھارا_ctx *دھارا = دھارا_user_انکڑے;

  إذا(pstream)
    *pstream = دھارا;
  إذا(دھارا && (دھارا->mid != UINT32_MAX)) {
    بنية Curl_easy *انکڑے, *call_انکڑے = CF_انکڑے_CURRENT(cf);
    إذا(!call_انکڑے || !call_انکڑے->multi) {
      DEBUGASSERT(0);
      أرجع عدم;
    }
    انکڑے = Curl_multi_get_easy(call_انکڑے->multi, دھارا->mid);
    إذا(انکڑے && (انکڑے->id == دھارا->xfer_id))
      أرجع انکڑے;
    DEBUGASSERT(0);
  }
  أرجع عدم;
}

ساكن ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  بنية Curl_cfilter *cf = conn_ref->user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  أرجع ctx->qconn;
}

#جے_تعریف_شدہ DEBUG_NGTCP2
ساكن فراغ quic_printf(فراغ *user_انکڑے, ثابت حرف *fmt, ...)
{
  va_list ap;
  (فراغ)user_انکڑے;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#جے_انت

ساكن فراغ qlog_callback(فراغ *user_انکڑے, uint32_t flags,
                          ثابت فراغ *انکڑے, size_t datalen)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  (فراغ)flags;
  إذا(ctx->qlogfd != -1) {
    ssize_t rc = اكتب(ctx->qlogfd, انکڑے, datalen);
    إذا(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

ساكن فراغ quic_settings(بنية cf_ngtcp2_ctx *ctx,
                          بنية Curl_easy *انکڑے,
                          بنية cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#جے_تعریف_شدہ DEBUG_NGTCP2
  s->log_printf = quic_printf;
#وإلا
  s->log_printf = عدم;
#جے_انت

  s->initial_ts = pktx->ts;
  s->handshake_timeout = (انکڑے->سیٹ.connecttimeout > 0) ?
    انکڑے->سیٹ.connecttimeout * NGTCP2_MILLISECONDS : QUIC_HANDSHAKE_TIMEOUT;
  s->max_کھڑکی = H3_CONN_کھڑکی_ناپ_MAX;
  s->max_دھارا_کھڑکی = 0; /* disable ngtcp2 auto-tuning of window */
  s->no_pmtud = FALSE;
#جے_تعریف_شدہ NGTCP2_SETTINGS_V3
  /* try ten times the ngtcp2 defaults here for problems with Caddy */
  s->glitch_ratelim_مدت = 1000 * 10;
  s->glitch_ratelim_شرح = 33 * 10;
#جے_انت
  t->initial_max_انکڑے = s->max_کھڑکی;
  t->initial_max_دھارا_انکڑے_bidi_local = H3_دھارا_کھڑکی_ناپ_INITIAL;
  t->initial_max_دھارا_انکڑے_bidi_remote = H3_دھارا_کھڑکی_ناپ_INITIAL;
  t->initial_max_دھارا_انکڑے_uni = t->initial_max_انکڑے;
  t->initial_max_streams_bidi = QUIC_MAX_STREAMS;
  t->initial_max_streams_uni = QUIC_MAX_STREAMS;
  t->max_idle_timeout = 0; /* no idle timeout from our side */
  إذا(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#إذا defined(_MSC_VER) && defined(_DLL)
#ہدایت warning(ڈالو)
#ہدایت warning(disable:4232) /* MSVC extension, dllimport identity */
#جے_انت

ساكن صحيح cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, فراغ *user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf ? cf->ctx : عدم;
  بنية Curl_easy *انکڑے;

  (فراغ)tconn;
  DEBUGASSERT(ctx);
  انکڑے = CF_انکڑے_CURRENT(cf);
  DEBUGASSERT(انکڑے);
  إذا(!ctx || !انکڑے)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(انکڑے);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, انکڑے);

  ctx->tls_vrfy_نتیجہ = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    انکڑے, &ctx->ssl_peer);
  إذا(ctx->tls_vrfy_نتیجہ)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

#جے_تعریف_شدہ CURLVERBOSE
  إذا(Curl_trc_is_verbose(انکڑے)) {
    ثابت ngtcp2_transport_params *rp;
    rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
    CURL_TRC_CF(انکڑے, cf, "handshake complete after %" FMT_TIMEDIFF_T
                "ms, remote transport[max_udp_payload=%" PRIu64
                ", initial_max_data=%" PRIu64 "]",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at),
                rp->max_udp_payload_ناپ, rp->initial_max_انکڑے);
  }
#جے_انت

  /* In case of earlydata, where we simulate being connected, update
   * the handshake time when we really did connect */
  إذا(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrs_ویلا_Was(انکڑے, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  إذا(ctx->use_earlydata) {
#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    إذا(SSL_get_early_انکڑے_حالت(ctx->tls.ossl.ssl) == SSL_EARLY_انکڑے_NOT_گھلیا)
      CURL_TRC_CF(انکڑے, cf, "OpenSSL did not send early data");
#جے_انت

#إذا NGTCP2_نسخہ_عدد >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_انکڑے_rejected2(ctx->qconn);
#وإلا /* older NGTCP2 */
    {
#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      صحيح ossl_early_حالت = SSL_get_early_انکڑے_حالت(ctx->tls.ossl.ssl);
      إذا(ossl_early_حالت == SSL_EARLY_انکڑے_NOT_گھلیا)
        CURL_TRC_CF(انکڑے, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_حالت == SSL_EARLY_انکڑے_ACCEPTED);
#وإلاإذا defined(USE_GNUTLS)
      صحيح flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_انکڑے);
#وإلاإذا defined(USE_WOLFSSL)
#جے_تعریف_شدہ WOLFSSL_EARLY_انکڑے
      ctx->earlydata_accepted =
        (wolf_SSL_get_early_انکڑے_حالت(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_انکڑے_REJECTED);
#وإلا
      DEBUGASSERT(0); /* should not come here if ED is disabled. */
      ctx->earlydata_accepted = FALSE;
#جے_انت /* WOLFSSL_EARLY_DATA */
#جے_انت /* OPENSSL or GNUTLS or WOLFSSL */
    }
#جے_انت /* older NGTCP2 */
    CURL_TRC_CF(انکڑے, cf, "server did%s accept %zu bytes of early data",
                ctx->earlydata_accepted ? "" : " not", ctx->earlydata_skip);
    Curl_pgrs_Early_انکڑے(انکڑے, ctx->earlydata_accepted ?
                              (curl_off_t)ctx->earlydata_skip :
                             -(curl_off_t)ctx->earlydata_skip);
  }
  أرجع 0;
}

ساكن صحيح cb_recv_دھارا_انکڑے(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t دھارا_id, uint64_t وتھ,
                               ثابت uint8_t *buf, size_t buflen,
                               فراغ *user_انکڑے, فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  صحيح fin = (flags & NGTCP2_دھارا_انکڑے_FLAG_FIN) ? 1 : 0;
  بنية h3_دھارا_ctx *دھارا;
  بنية Curl_easy *انکڑے =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_انکڑے, &دھارا);

  (فراغ)وتھ;
  rc = nghttp3_conn_read_دھارا(ctx->h3conn, دھارا_id, buf, buflen, fin);
  إذا(rc < 0) {
    إذا(انکڑے && دھارا) {
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  دھارا_id, دھارا->reset, دھارا->closed);
    }
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  إذا(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_دھارا_وتھ(tconn, دھارا_id, nconsumed);
    ngtcp2_conn_extend_max_وتھ(tconn, nconsumed);
    إذا(دھارا) {
      دھارا->rx_وتھ += nconsumed;
      دھارا->rx_وتھ_max += nconsumed;
    }
  }
  أرجع 0;
}

ساكن صحيح cb_acked_دھارا_انکڑے_وتھ(ngtcp2_conn *tconn, int64_t دھارا_id,
                                       uint64_t وتھ, uint64_t datalen,
                                       فراغ *user_انکڑے, فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)دھارا_id;
  (فراغ)tconn;
  (فراغ)وتھ;
  (فراغ)datalen;
  (فراغ)دھارا_user_انکڑے;

  rv = nghttp3_conn_جوڑو_ack_وتھ(ctx->h3conn, دھارا_id, datalen);
  إذا(rv && rv != NGHTTP3_ERR_دھارا_NOT_لبھیا) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_دھارا_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t دھارا_id, uint64_t app_error_کوڈ,
                           فراغ *user_انکڑے, فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے = Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_انکڑے, عدم);
  صحيح rv;

  (فراغ)tconn;
  /* stream is closed... */
  إذا(!انکڑے)
    انکڑے = CF_انکڑے_CURRENT(cf);
  إذا(!انکڑے)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  إذا(!(flags & NGTCP2_دھارا_CLOSE_FLAG_APP_ERROR_کوڈ_سیٹ)) {
    app_error_کوڈ = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_دھارا(ctx->h3conn, دھارا_id, app_error_کوڈ);
  CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", دھارا_id, app_error_کوڈ, rv);
  إذا(rv && rv != NGHTTP3_ERR_دھارا_NOT_لبھیا) {
    Curl_cf_ngtcp2_h3_err_سیٹ(cf, انکڑے, rv);
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

#جے_تعریف_شدہ NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
ساكن صحيح cb_دھارا_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t دھارا_id,
                           uint64_t rx_app_error_کوڈ,
                           uint64_t tx_app_error_کوڈ,
                           فراغ *user_انکڑے, فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے = Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_انکڑے, عدم);
  uint64_t h3_app_error_کوڈ = NGHTTP3_H3_NO_ERROR;
  صحيح rv;

  (فراغ)tconn;
  (فراغ)tx_app_error_کوڈ;
  /* stream is closed... */
  إذا(!انکڑے)
    انکڑے = CF_انکڑے_CURRENT(cf);
  إذا(!انکڑے)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  إذا(flags & NGTCP2_دھارا_CLOSE2_FLAG_RX_APP_ERROR_کوڈ_سیٹ)
    h3_app_error_کوڈ = rx_app_error_کوڈ;

  rv = nghttp3_conn_close_دھارا(ctx->h3conn, دھارا_id, h3_app_error_کوڈ);
  CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", دھارا_id, h3_app_error_کوڈ, rv);
  إذا(rv && rv != NGHTTP3_ERR_دھارا_NOT_لبھیا) {
    Curl_cf_ngtcp2_h3_err_سیٹ(cf, انکڑے, rv);
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}
#جے_انت

ساكن صحيح cb_دھارا_reset(ngtcp2_conn *tconn, int64_t دھارا_id,
                           uint64_t final_ناپ, uint64_t app_error_کوڈ,
                           فراغ *user_انکڑے, فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے = Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_انکڑے, عدم);
  صحيح rv;
  (فراغ)tconn;
  (فراغ)final_ناپ;
  (فراغ)app_error_کوڈ;

  rv = nghttp3_conn_shutdown_دھارا_read(ctx->h3conn, دھارا_id);
  إذا(انکڑے)
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] reset -> %d", دھارا_id, rv);
  إذا(rv && rv != NGHTTP3_ERR_دھارا_NOT_لبھیا) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_دھارا_stop_sending(ngtcp2_conn *tconn, int64_t دھارا_id,
                                  uint64_t app_error_کوڈ, فراغ *user_انکڑے,
                                  فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)tconn;
  (فراغ)app_error_کوڈ;
  (فراغ)دھارا_user_انکڑے;

  rv = nghttp3_conn_shutdown_دھارا_read(ctx->h3conn, دھارا_id);
  إذا(rv && rv != NGHTTP3_ERR_دھارا_NOT_لبھیا) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            فراغ *user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);

  (فراغ)tconn;
  ctx->max_bidi_streams = max_streams;
  إذا(انکڑے)
    CURL_TRC_CF(انکڑے, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->ورتیا_bidi_streams);
  أرجع 0;
}

ساكن صحيح cb_extend_max_دھارا_انکڑے(ngtcp2_conn *tconn, int64_t دھارا_id,
                                     uint64_t max_انکڑے, فراغ *user_انکڑے,
                                     فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا;
  بنية Curl_easy *انکڑے =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_انکڑے, &دھارا);
  صحيح rv;
  (فراغ)tconn;
  (فراغ)max_انکڑے;

  rv = nghttp3_conn_unblock_دھارا(ctx->h3conn, دھارا_id);
  إذا(rv && rv != NGHTTP3_ERR_دھارا_NOT_لبھیا) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  إذا(دھارا && دھارا->quic_flow_blocked) {
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] unblock quic flow", دھارا_id);
    دھارا->quic_flow_blocked = FALSE;
    إذا(انکڑے)
      Curl_multi_mark_dirty(انکڑے);
  }
  أرجع 0;
}

ساكن فراغ cb_rand(uint8_t *dest, size_t destlen,
                    ثابت ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode نتیجہ;
  (فراغ)rand_ctx;

  نتیجہ = Curl_rand(عدم, dest, destlen);
  إذا(نتیجہ) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
ساكن صحيح cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    فراغ *user_انکڑے)
{
  CURLcode نتیجہ;
  (فراغ)tconn;
  (فراغ)user_انکڑے;

  نتیجہ = Curl_rand(عدم, cid->انکڑے, cidlen);
  إذا(نتیجہ)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  نتیجہ = Curl_rand(عدم, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  إذا(نتیجہ)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  أرجع 0;
}

#جے_تعریف_شدہ NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
ساكن صحيح cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  بنية ngtcp2_stateless_reset_token *token, size_t cidlen, فراغ *user_انکڑے)
{
  CURLcode نتیجہ;
  (فراغ)tconn;
  (فراغ)user_انکڑے;

  نتیجہ = Curl_rand(عدم, cid->انکڑے, cidlen);
  إذا(نتیجہ)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  نتیجہ = Curl_rand(عدم, token->انکڑے, حجم(token->انکڑے));
  إذا(نتیجہ)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  أرجع 0;
}
#جے_انت

ساكن صحيح cb_recv_rx_کلید(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          فراغ *user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf ? cf->ctx : عدم;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  (فراغ)tconn;

  إذا(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    أرجع 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(انکڑے);
  إذا(ctx && انکڑے && !ctx->h3conn && ctx->init_h3_conn_cb) {
    إذا(ctx->init_h3_conn_cb(cf, انکڑے, ctx))
      أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  أرجع 0;
}

#جے_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wmissing-field-initializers"
#جے_انت
ساكن ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  عدم, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_انکڑے_cb,
  cb_ngtcp2_handshake_completed,
  عدم, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_نقاب_cb,
  cb_recv_دھارا_انکڑے,
  cb_acked_دھارا_انکڑے_وتھ,
  عدم, /* stream_open */
  cb_دھارا_close,
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
  cb_دھارا_reset,
  عدم, /* extend_max_remote_streams_bidi */
  عدم, /* extend_max_remote_streams_uni */
  cb_extend_max_دھارا_انکڑے,
  عدم, /* dcid_status */
  عدم, /* handshake_confirmed */
  عدم, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  عدم, /* recv_datagram */
  عدم, /* ack_datagram */
  عدم, /* lost_datagram */
  ngtcp2_crypto_get_رستہ_challenge_انکڑے_cb,
  cb_دھارا_stop_sending,
  عدم, /* version_negotiation */
  cb_recv_rx_کلید,
  عدم, /* recv_tx_key */
  عدم, /* early_data_rejected */
#جے_تعریف_شدہ NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  عدم, /* begin_path_validation */
#جے_انت
#جے_تعریف_شدہ NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  عدم, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  عدم, /* dcid_status2 */
  ngtcp2_crypto_get_رستہ_challenge_data2_cb, /* get_path_challenge_data2 */
#جے_انت
#جے_تعریف_شدہ NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  عدم, /* recv_stop_sending */
#جے_انت
#جے_تعریف_شدہ NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_دھارا_close2, /* is called instead of cb_stream_close when set */
#جے_انت
};
#جے_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic نکالو
#جے_انت

#إذا defined(_MSC_VER) && defined(_DLL)
#ہدایت warning(نکالو)
#جے_انت

#جے_تعریف_شدہ USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ساكن صحيح quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  بنية Curl_cfilter *cf;
  بنية cf_ngtcp2_ctx *ctx;
  بنية Curl_easy *انکڑے;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_انکڑے(ssl);
  cf = cref ? cref->user_انکڑے : عدم;
  ctx = cf ? cf->ctx : عدم;
  انکڑے = cf ? CF_انکڑے_CURRENT(cf) : عدم;
  إذا(cf && انکڑے && ctx) {
    بنانشانی حرف *quic_tp = عدم;
    size_t quic_tp_len = 0;
#جے_تعریف_شدہ HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     حجم(tpbuf));
    إذا(tplen < 0)
      CURL_TRC_CF(انکڑے, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((صحيح)tplen));
    وإلا {
      quic_tp = (بنانشانی حرف *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#جے_انت
    Curl_ossl_جوڑو_session(cf, انکڑے, &ctx->tls.ossl, ctx->ssl_peer.scache_کلید,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, عدم);
  }
  أرجع 0;
}
#جے_انت /* USE_OPENSSL */

#جے_تعریف_شدہ USE_GNUTLS

#جے_تعریف_شدہ CURLVERBOSE
ساكن ثابت حرف *gtls_hs_پیغام_name(صحيح mtype)
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
#جے_انت

ساكن صحيح quic_gtls_handshake_cb(gnutls_session_t session, بنانشانی صحيح htype,
                                  بنانشانی when, بنانشانی صحيح incoming,
                                  ثابت gnutls_datum_t *پیغام)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  بنية Curl_cfilter *cf = conn_ref ? conn_ref->user_انکڑے : عدم;
  بنية cf_ngtcp2_ctx *ctx = cf ? cf->ctx : عدم;

  (فراغ)پیغام;
  (فراغ)incoming;
  إذا(when && cf && ctx) { /* after message has been processed */
    بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
    DEBUGASSERT(انکڑے);
    إذا(!انکڑے)
      أرجع 0;
    CURL_TRC_CF(انکڑے, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_پیغام_name(htype), htype);
    بدل(htype) {
    حالة GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      بنانشانی حرف *quic_tp = عدم;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       حجم(tpbuf));
      إذا(tplen < 0)
        CURL_TRC_CF(انکڑے, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحيح)tplen));
      وإلا {
        quic_tp = (بنانشانی حرف *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (فراغ)Curl_gtls_cache_session(cf, انکڑے, ctx->ssl_peer.scache_کلید,
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
#جے_انت /* USE_GNUTLS */

#جے_تعریف_شدہ USE_WOLFSSL
ساكن صحيح wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolf_SSL_get_app_انکڑے(ssl);
  بنية Curl_cfilter *cf = conn_ref ? conn_ref->user_انکڑے : عدم;

  DEBUGASSERT(cf);
  إذا(cf && session) {
    بنية cf_ngtcp2_ctx *ctx = cf->ctx;
    بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
    DEBUGASSERT(انکڑے);
    إذا(انکڑے && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      بنانشانی حرف *quic_tp = عدم;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       حجم(tpbuf));
      إذا(tplen < 0)
        CURL_TRC_CF(انکڑے, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحيح)tplen));
      وإلا {
        quic_tp = (بنانشانی حرف *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (فراغ)Curl_wssl_cache_session(cf, انکڑے, ctx->ssl_peer.scache_کلید,
                                    session, wolf_SSL_نسخہ(ssl),
                                    "h3", quic_tp, quic_tp_len, عدم);
    }
  }
  أرجع 0;
}
#جے_انت /* USE_WOLFSSL */

ساكن CURLcode cf_ngtcp2_tls_ctx_تیاری(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *انکڑے,
                                        فراغ *user_انکڑے)
{
  بنية curl_tls_ctx *ctx = user_انکڑے;

#جے_تعریف_شدہ USE_OPENSSL
#إذا defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  إذا(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(انکڑے, "ngtcp2_crypto_boringssl_configure_client_context failed");
    أرجع CURLE_FAILED_INIT;
  }
#وإلاإذا defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#وإلا
  إذا(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(انکڑے, "ngtcp2_crypto_quictls_configure_client_context failed");
    أرجع CURLE_FAILED_INIT;
  }
#جے_انت /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  إذا(Curl_ssl_scache_use(cf, انکڑے)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_سیٹ_session_cache_منوال(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_اندرونی);
    SSL_CTX_sess_سیٹ_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#وإلاإذا defined(USE_GNUTLS)
  إذا(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(انکڑے, "ngtcp2_crypto_gnutls_configure_client_session failed");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(Curl_ssl_scache_use(cf, انکڑے)) {
    gnutls_handshake_سیٹ_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#وإلاإذا defined(USE_WOLFSSL)
  إذا(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(انکڑے, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(Curl_ssl_scache_use(cf, انکڑے)) {
    /* Register to get notified when a new session is received */
    wolf_SSL_CTX_sess_سیٹ_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#جے_انت
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ngtcp2_on_session_reuse(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *انکڑے,
                                           بنية alpn_spec *alpns,
                                           بنية Curl_ssl_session *scs,
                                           bool *do_early_انکڑے)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  *do_early_انکڑے = FALSE;
#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#جے_انت
#جے_تعریف_شدہ USE_GNUTLS
  ctx->earlydata_max =
    gnutls_ریکارڈ_get_max_early_انکڑے_ناپ(ctx->tls.gtls.session);
#جے_انت
#جے_تعریف_شدہ USE_WOLFSSL
#جے_تعریف_شدہ WOLFSSL_EARLY_انکڑے
  ctx->earlydata_max = scs->earlydata_max;
#وإلا
  ctx->earlydata_max = 0;
#جے_انت /* WOLFSSL_EARLY_DATA */
#جے_انت
#إذا defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  إذا(!ctx->earlydata_max) {
    CURL_TRC_CF(انکڑے, cf, "SSL session does not allow earlydata");
  }
  وإلا إذا(!Curl_alpn_شامل_اے_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(انکڑے, cf, "SSL session from different ALPN, no early data");
  }
  وإلا إذا(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(انکڑے, cf, "no 0RTT transport parameters, no early data");
  }
  وإلا {
    صحيح rv;
    rv = ngtcp2_conn_decode_and_سیٹ_0rtt_transport_params(
      ctx->qconn, (ثابت uint8_t *)scs->quic_tp, scs->quic_tp_len);
    إذا(rv)
      CURL_TRC_CF(انکڑے, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    وإلا إذا(ctx->init_h3_conn_cb) {
      infof(انکڑے, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      نتیجہ = ctx->init_h3_conn_cb(cf, انکڑے, ctx);
      إذا(!نتیجہ) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_انکڑے = TRUE;
      }
    }
    وإلا { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_انکڑے = TRUE;
    }
  }
#وإلا /* not supported in the TLS backend */
  (فراغ)انکڑے;
  (فراغ)ctx;
  (فراغ)scs;
  (فراغ)alpns;
#جے_انت
  أرجع نتیجہ;
}

/*
 * Might be called twice for happy eyeballs.
 */
ساكن CURLcode cf_connect_شروع(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rc;
  صحيح rv;
  CURLcode نتیجہ;
  ثابت بنية Curl_sockaddr_ex *sockaddr = عدم;
  صحيح qfd;
  ساكن ثابت بنية alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  نتیجہ = Curl_rand(انکڑے, ctx->dcid.انکڑے, NGTCP2_MAX_CIDLEN);
  إذا(نتیجہ)
    أرجع نتیجہ;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  نتیجہ = Curl_rand(انکڑے, ctx->scid.انکڑے, NGTCP2_MAX_CIDLEN);
  إذا(نتیجہ)
    أرجع نتیجہ;

  (فراغ)Curl_qlogdir(انکڑے, ctx->scid.انکڑے, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, انکڑے, pktx);

  نتیجہ = Curl_vquic_ctx_init(انکڑے, &ctx->q);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* Query socket and remote address from sub-chain */
  إذا(Curl_cf_socket_peek(cf->اگلا, انکڑے, &ctx->q.sockfd, &sockaddr, عدم)) {
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

    ngtcp2_addr_init(&ctx->connected_رستہ.local,
                     (بنية sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_رستہ.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  وإلا {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ثابت بنية Curl_sockaddr_ex *remote = عدم;
    إذا(cf->اگلا->cft->query(cf->اگلا, انکڑے, CF_QUERY_REMOTE_ADDR, عدم,
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
#جے_تعریف_شدہ USE_IPV6
    حالة AF_INET6:
      ((بنية sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = حجم(بنية sockaddr_in6);
      اكسر;
#جے_انت
    افتراضي:
      أرجع CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_رستہ.local,
                     (بنية sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_رستہ.remote,
                     &remote->curl_sa_addr,
                     (socklen_t)remote->addrlen);
  }

  rc = ngtcp2_conn_client_new(&ctx->qconn, &ctx->dcid, &ctx->scid,
                              &ctx->connected_رستہ,
                              NGTCP2_PROTO_VER_V1, &ng_callbacks,
                              &ctx->settings, &ctx->transport_params,
                              Curl_ngtcp2_mem(), cf);
  إذا(rc)
    أرجع CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_انکڑے = cf;

  نتیجہ = Curl_vquic_tls_init(&ctx->tls, cf, انکڑے,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_تیاری, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  إذا(نتیجہ)
    أرجع نتیجہ;

#إذا defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  إذا(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(انکڑے, "ngtcp2_crypto_ossl_ctx_new failed");
    أرجع CURLE_FAILED_INIT;
  }
  ngtcp2_conn_سیٹ_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  إذا(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(انکڑے, "ngtcp2_crypto_ossl_configure_client_session failed");
    أرجع CURLE_FAILED_INIT;
  }
#وإلاإذا defined(USE_OPENSSL)
  SSL_سیٹ_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_سیٹ_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#وإلاإذا defined(USE_GNUTLS)
  ngtcp2_conn_سیٹ_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#وإلاإذا defined(USE_WOLFSSL)
  ngtcp2_conn_سیٹ_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#وإلا
#غلطی "ngtcp2 TLS backend not defined"
#جے_انت

#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  إذا(ctx->use_earlydata &&
     !SSL_سیٹ_quic_tls_early_انکڑے_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(انکڑے, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#جے_انت
  ngtcp2_ccerr_default(&ctx->آخری_error);

  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے,
                                    bool *پورا)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  بنية cf_call_انکڑے save;
  بنية cf_ngtcp2_io_ctx pktx;

  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the sub-chain */
  إذا(cf->اگلا && !cf->اگلا->connected) {
    نتیجہ = Curl_conn_cf_connect(cf->اگلا, انکڑے, پورا);
    إذا(نتیجہ || !*پورا)
      أرجع نتیجہ;
  }

  *پورا = FALSE;

  إذا(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(انکڑے, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(انکڑے, cf, "need HTTPS-RR, delaying connect");
    أرجع CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, انکڑے);
  CF_انکڑے_SAVE(save, cf, انکڑے);

  إذا(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(انکڑے);
    نتیجہ = cf_connect_شروع(cf, انکڑے, &pktx);
    إذا(نتیجہ)
      اذهب باہرلا;
    إذا(cf->connected) {
      *پورا = TRUE;
      اذهب باہرلا;
    }
    نتیجہ = Curl_cf_ngtcp2_ترقی_egress(cf, انکڑے, &pktx);
    /* we do not expect to be able to recv anything yet */
    اذهب باہرلا;
  }

  نتیجہ = Curl_cf_ngtcp2_ترقی_ingress(cf, انکڑے, &pktx);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = Curl_cf_ngtcp2_ترقی_egress(cf, انکڑے, &pktx);
  إذا(نتیجہ)
    اذهب باہرلا;

  إذا(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    نتیجہ = ctx->tls_vrfy_نتیجہ;
    إذا(!نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "peer verified");
      cf->connected = TRUE;
      *پورا = TRUE;
    }
  }

باہرلا:
  إذا(ctx->tls_vrfy_نتیجہ)
    نتیجہ = ctx->tls_vrfy_نتیجہ;
  إذا(ctx->qconn &&
     ((نتیجہ == CURLE_RECV_ERROR) || (نتیجہ == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_وچ_draining_period(ctx->qconn)) {
    ثابت ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    نتیجہ = CURLE_COULDNT_CONNECT;
    إذا(cerr) {
      CURL_TRC_CF(انکڑے, cf, "connect error, type=%d, code=%" PRIu64,
                  (صحيح)cerr->type, cerr->error_کوڈ);
      بدل(cerr->type) {
      حالة NGTCP2_CCERR_TYPE_نسخہ_NEGOTIATION:
        CURL_TRC_CF(انکڑے, cf, "error in version negotiation");
        اكسر;
      افتراضي:
        إذا(cerr->error_کوڈ >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(انکڑے, cf, "crypto error, tls alert=%u",
                      (بنانشانی صحيح)(cerr->error_کوڈ & 0xffU));
        }
        وإلا إذا(cerr->error_کوڈ == NGTCP2_CONNECTION_REFUSED) {
          CURL_TRC_CF(انکڑے, cf, "connection refused by server");
          /* When a QUIC server instance is shutting down, it may send us a
           * CONNECTION_CLOSE with this code right away. We want
           * to keep on trying in this case. */
          نتیجہ = CURLE_WEIRD_SERVER_جواب;
        }
      }
    }
  }

#جے_تعریف_شدہ CURLVERBOSE
  إذا(نتیجہ) {
    إذا(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      بنية ip_quadruple ip;

      إذا(!Curl_cf_socket_peek(cf->اگلا, انکڑے, عدم, عدم, &ip))
        infof(انکڑے, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_پورٹ, curl_easy_strerror(نتیجہ));
    }
  }
#جے_انت
  إذا(!نتیجہ && ctx->qconn) {
    نتیجہ = Curl_cf_ngtcp2_cmn_سیٹ_expiry(cf, انکڑے, &pktx);
  }
  إذا(نتیجہ || *پورا)
    CURL_TRC_CF(انکڑے, cf, "connect -> %d, done=%d", (صحيح)نتیجہ, *پورا);
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_call_انکڑے save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode نتیجہ = CURLE_OK;

  إذا(cf->shutdown || !ctx->qconn) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  إذا(!cf->اگلا) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  CF_انکڑے_SAVE(save, cf, انکڑے);
  *پورا = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, انکڑے);

  إذا(!ctx->shutdown_started) {
    حرف بفر[NGTCP2_MAX_UDP_PAYLOAD_ناپ];
    ngtcp2_ssize nwritten;

    إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->q.sendbuf)) {
      CURL_TRC_CF(انکڑے, cf, "shutdown, flushing sendbuf");
      نتیجہ = Curl_cf_ngtcp2_ترقی_egress(cf, انکڑے, &pktx);
      إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->q.sendbuf)) {
        CURL_TRC_CF(انکڑے, cf, "sending shutdown packets blocked");
        نتیجہ = CURLE_OK;
        اذهب باہرلا;
      }
      وإلا إذا(نتیجہ) {
        CURL_TRC_CF(انکڑے, cf, "shutdown, error %d flushing sendbuf",
                    (صحيح)نتیجہ);
        *پورا = TRUE;
        اذهب باہرلا;
      }
    }

    DEBUGASSERT(Curl_bufq_is_خالی_تھاواں(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, عدم, /*رستہ*/
      عدم, /* pkt_info */
      (uint8_t *)بفر, حجم(بفر),
      &ctx->آخری_error, pktx.ts);
    CURL_TRC_CF(انکڑے, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (صحيح)ctx->آخری_error.type,
                ctx->آخری_error.error_کوڈ, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    إذا(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      نتیجہ = Curl_bufq_write(&ctx->q.sendbuf, (ثابت بنانشانی حرف *)بفر,
                               (size_t)nwritten, &n);
      إذا(نتیجہ) {
        CURL_TRC_CF(انکڑے, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (صحيح)نتیجہ);
        اذهب باہرلا;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.ونڈو_len = 0;
    }
  }

  إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->q.sendbuf)) {
    CURL_TRC_CF(انکڑے, cf, "shutdown, flushing egress");
    نتیجہ = Curl_vquic_flush(cf, انکڑے, &ctx->q);
    إذا(نتیجہ == CURLE_AGAIN) {
      CURL_TRC_CF(انکڑے, cf, "sending shutdown packets blocked");
      نتیجہ = CURLE_OK;
      اذهب باہرلا;
    }
    وإلا إذا(نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "shutdown, error %d flushing sendbuf",
                  (صحيح)نتیجہ);
      *پورا = TRUE;
      اذهب باہرلا;
    }
  }

  إذا(Curl_bufq_is_خالی_تھاواں(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(انکڑے, cf, "shutdown completely sent off, done");
    *پورا = TRUE;
    نتیجہ = CURLE_OK;
  }
باہرلا:
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

فراغ Curl_cf_ngtcp2_cmn_conn_close(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے)
{
  bool پورا;
  Curl_cf_ngtcp2_cmn_shutdown(cf, انکڑے, &پورا);
}

ساكن bool cf_ngtcp2_err_is_fatal(صحيح کوڈ)
{
  أرجع (NGTCP2_ERR_FATAL >= کوڈ) ||
         (NGTCP2_ERR_DROP_CONN == کوڈ) ||
         (NGTCP2_ERR_IDLE_CLOSE == کوڈ);
}

فراغ Curl_cf_ngtcp2_cmn_err_سیٹ(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے, صحيح کوڈ)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  إذا(!ctx->آخری_error.error_کوڈ) {
    إذا(NGTCP2_ERR_CRYPTO == کوڈ) {
      ngtcp2_ccerr_سیٹ_tls_alert(&ctx->آخری_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 عدم, 0);
    }
    وإلا {
      ngtcp2_ccerr_سیٹ_liberr(&ctx->آخری_error, کوڈ, عدم, 0);
    }
  }
  إذا(cf_ngtcp2_err_is_fatal(کوڈ))
    Curl_cf_ngtcp2_cmn_conn_close(cf, انکڑے);
}

فراغ Curl_cf_ngtcp2_io_ctx_init(بنية cf_ngtcp2_io_ctx *io_ctx,
                                بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->انکڑے = انکڑے;
  io_ctx->now = *Curl_pgrs_now(انکڑے);
  ngtcp2_رستہ_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_سیٹ_ویلا(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

فراغ Curl_cf_ngtcp2_io_ctx_update_ویلا(بنية Curl_easy *انکڑے,
                                       بنية cf_ngtcp2_io_ctx *io_ctx,
                                       بنية Curl_cfilter *cf)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(انکڑے);
  Curl_vquic_ctx_update_ویلا(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#إذا NGTCP2_نسخہ_عدد < 0x011100
بنية cf_ngtcp2_sfind_ctx {
  int64_t دھارا_id;
  بنية h3_دھارا_ctx *دھارا;
  uint32_t mid;
};

ساكن bool cf_ngtcp2_sfind(uint32_t mid, فراغ *قدر, فراغ *user_انکڑے)
{
  بنية cf_ngtcp2_sfind_ctx *fctx = user_انکڑے;
  بنية h3_دھارا_ctx *دھارا = قدر;

  إذا(fctx->دھارا_id == دھارا->id) {
    fctx->mid = mid;
    fctx->دھارا = دھارا;
    أرجع FALSE;
  }
  أرجع TRUE; /* continue */
}

ساكن بنية h3_دھارا_ctx *cf_ngtcp2_get_دھارا(بنية cf_ngtcp2_ctx *ctx,
                                                  int64_t دھارا_id)
{
  بنية cf_ngtcp2_sfind_ctx fctx;
  fctx.دھارا_id = دھارا_id;
  fctx.دھارا = عدم;
  Curl_u32_ptrset_ویکھو(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  أرجع fctx.دھارا;
}
#وإلا
ساكن بنية h3_دھارا_ctx *cf_ngtcp2_get_دھارا(بنية cf_ngtcp2_ctx *ctx,
                                                  int64_t دھارا_id)
{
  أرجع ngtcp2_conn_get_دھارا_user_انکڑے(ctx->qconn, دھارا_id);
}
#جے_انت

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
ساكن CURLcode read_pkt_منزل_send(فراغ *userp,
                                 بنانشانی حرف *buf, size_t buflen,
                                 size_t *pnread)
{
  بنية cf_ngtcp2_io_ctx *x = userp;
  بنية cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t دھارا_id;
  صحيح fin;
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
  لكل(;;) {

    إذا(ctx->h3conn && ngtcp2_conn_get_max_انکڑے_بایاں(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_دھارا(ctx->h3conn, &دھارا_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      إذا(veccnt < 0) {
        failf(x->انکڑے, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((صحيح)veccnt));
        Curl_cf_ngtcp2_h3_err_سیٹ(x->cf, x->انکڑے, (صحيح)veccnt);
        أرجع CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_دھارا_FLAG_ہور |
            (fin ? NGTCP2_WRITE_دھارا_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_دھارا(ctx->qconn, &x->ps.رستہ,
                                  عدم, buf, buflen,
                                  &ndatalen, flags, دھارا_id,
                                  (ثابت ngtcp2_vec *)vec, veccnt, x->ts);
    إذا(n == 0) {
      /* nothing to send */
      أرجع CURLE_AGAIN;
    }
    وإلا إذا(n < 0) {
      بدل(n) {
      حالة NGTCP2_ERR_دھارا_انکڑے_BLOCKED: {
        بنية h3_دھارا_ctx *دھارا;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_بلاک_دھارا(ctx->h3conn, دھارا_id);
        CURL_TRC_CF(x->انکڑے, x->cf, "[%" PRId64 "] block quic flow",
                    دھارا_id);
        دھارا = cf_ngtcp2_get_دھارا(ctx, دھارا_id);
        إذا(دھارا) /* it might be not one of our h3 streams? */
          دھارا->quic_flow_blocked = TRUE;
        n = 0;
        اكسر;
      }
      حالة NGTCP2_ERR_دھارا_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_دھارا_write(ctx->h3conn, دھارا_id);
        n = 0;
        اكسر;
      حالة NGTCP2_ERR_WRITE_ہور:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        اكسر;
      افتراضي:
        DEBUGASSERT(ndatalen == -1);
        failf(x->انکڑے, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((صحيح)n));
        Curl_cf_ngtcp2_cmn_err_سیٹ(x->cf, x->انکڑے, (صحيح)n);
        أرجع CURLE_SEND_ERROR;
      }
    }

    إذا(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      صحيح rv = nghttp3_conn_جوڑو_write_وتھ(ctx->h3conn, دھارا_id, ndatalen);
      إذا(rv) {
        failf(x->انکڑے, "nghttp3_conn_add_write_offset returned error: %s",
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

CURLcode Curl_cf_ngtcp2_ترقی_egress(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *انکڑے,
                                        بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_ناپ, رستہ_max_payload_ناپ;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_ویلا_حصہ;
  CURLcode نتیجہ;
  بنية cf_ngtcp2_io_ctx local_pktx;

  إذا(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, انکڑے);
    pktx = &local_pktx;
  }
  وإلا {
    Curl_cf_ngtcp2_io_ctx_update_ویلا(انکڑے, pktx, cf);
    ngtcp2_رستہ_storage_zero(&pktx->ps);
  }

  نتیجہ = Curl_vquic_flush(cf, انکڑے, &ctx->q);
  إذا(نتیجہ) {
    إذا(نتیجہ == CURLE_AGAIN) {
      Curl_expire_سیٹ(انکڑے, EXPIRE_QUIC, 1, &pktx->now);
      أرجع CURLE_OK;
    }
    أرجع نتیجہ;
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
  max_payload_ناپ = ngtcp2_conn_get_max_tx_udp_payload_ناپ(ctx->qconn);
  رستہ_max_payload_ناپ =
    ngtcp2_conn_get_رستہ_max_tx_udp_payload_ناپ(ctx->qconn);
  send_ویلا_حصہ = ngtcp2_conn_get_send_ویلا_حصہ(ctx->qconn);
  CURL_TRC_CF(انکڑے, cf, "egress, collect and send packets, quantum=%zu",
              send_ویلا_حصہ);
  لكل(;;) {
    /* add the next packet to send, if any, to our buffer */
    نتیجہ = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_ناپ,
                            read_pkt_منزل_send, pktx, &nread);
    إذا(نتیجہ == CURLE_AGAIN)
      اكسر;
    وإلا إذا(نتیجہ)
      أرجع نتیجہ;
    وإلا {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      إذا((buflen >= send_ویلا_حصہ) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_ناپ))
        اكسر;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      إذا(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      وإلا إذا(nread > gsolen ||
              (gsolen > رستہ_max_payload_ناپ && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        نتیجہ = Curl_vquic_send_پوچھ_ونڈو(cf, انکڑے, &ctx->q,
                                            gsolen, nread, nread);
        إذا(نتیجہ) {
          إذا(نتیجہ == CURLE_AGAIN) {
            Curl_expire_سیٹ(انکڑے, EXPIRE_QUIC, 1, &pktx->now);
            أرجع CURLE_OK;
          }
          أرجع نتیجہ;
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

  إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(انکڑے, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    نتیجہ = Curl_vquic_send(cf, انکڑے, &ctx->q, gsolen);
    إذا(نتیجہ) {
      إذا(نتیجہ == CURLE_AGAIN) {
        Curl_expire_سیٹ(انکڑے, EXPIRE_QUIC, 1, &pktx->now);
        أرجع CURLE_OK;
      }
      أرجع نتیجہ;
    }
    Curl_cf_ngtcp2_io_ctx_update_ویلا(انکڑے, pktx, cf);
    ngtcp2_conn_update_pkt_tx_ویلا(ctx->qconn, pktx->ts);
  }
  أرجع CURLE_OK;
}

بنية cf_ngtcp2_recv_ctx {
  بنية cf_ngtcp2_io_ctx *pktx;
  size_t pkt_گنتی;
};

ساكن CURLcode cf_ngtcp2_recv_pkts(ثابت بنانشانی حرف *buf, size_t buflen,
                                    size_t gso_ناپ,
                                    بنية sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    فراغ *userp)
{
  بنية cf_ngtcp2_recv_ctx *rctx = userp;
  بنية cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  بنية cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_رستہ رستہ;
  size_t وتھ, pktlen;
  صحيح rv;

  إذا(!rctx->pkt_گنتی) {
    Curl_cf_ngtcp2_io_ctx_update_ویلا(pktx->انکڑے, pktx, pktx->cf);
    ngtcp2_رستہ_storage_zero(&pktx->ps);
  }

  إذا(ecn)
    CURL_TRC_CF(pktx->انکڑے, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_ناپ, (بنانشانی)ecn);
  ngtcp2_addr_init(&رستہ.local, (بنية sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&رستہ.remote, (بنية sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  لكل(وتھ = 0; وتھ < buflen; وتھ += gso_ناپ) {
    rctx->pkt_گنتی++;
    pktlen = ((وتھ + gso_ناپ) <= buflen) ? gso_ناپ : (buflen - وتھ);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &رستہ, &pi,
                              buf + وتھ, pktlen, pktx->ts);
    إذا(rv) {
      CURL_TRC_CF(pktx->انکڑے, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_سیٹ(pktx->cf, pktx->انکڑے, rv);

      إذا(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        أرجع CURLE_PEER_FAILED_VERIFICATION;
      أرجع CURLE_RECV_ERROR;
    }
  }
  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_ترقی_ingress(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *انکڑے,
                                         بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_ngtcp2_io_ctx local_pktx;
  بنية cf_ngtcp2_recv_ctx rctx;
  CURLcode نتیجہ = CURLE_OK;

  إذا(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, انکڑے);
    pktx = &local_pktx;
  }

  نتیجہ = Curl_vquic_tls_before_recv(&ctx->tls, cf, انکڑے);
  إذا(نتیجہ)
    أرجع نتیجہ;

  rctx.pktx = pktx;
  rctx.pkt_گنتی = 0;

  إذا(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(انکڑے, cf, "progress_ingress(socket)");
    أرجع Curl_vquic_recv_packets(cf, انکڑے, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  وإلا {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    بنانشانی حرف *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_ناپ;
    size_t pkt_حد_بندی = QUIC_TUNNEL_INGRESS_PKT_حد_بندی;
    size_t nread;
    بنية sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(انکڑے, cf, "progress_ingress(sub-filters)");
    إذا(ctx->qconn) {
      size_t max_رستہ_payload;
      max_رستہ_payload =
        ngtcp2_conn_get_رستہ_max_tx_udp_payload_ناپ(ctx->qconn);
      إذا(max_رستہ_payload > max_udp_payload)
        max_udp_payload = max_رستہ_payload;
    }

    إذا(ctx->tunnel_inbuf_len < max_udp_payload) {
      بنانشانی حرف *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      إذا(!newbuf)
        أرجع CURLE_باہرلا_دا_حافظہ;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    طالما(pkt_حد_بندی--) {
      نتیجہ = Curl_conn_cf_recv(cf->اگلا, انکڑے, (حرف *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      إذا(نتیجہ == CURLE_AGAIN) {
        /* no more data available at the moment */
        أرجع CURLE_OK;
      }
      إذا(نتیجہ) {
        CURL_TRC_CF(انکڑے, cf, "ingress, recv from tunnel failed: %d",
                    (صحيح)نتیجہ);
        أرجع نتیجہ;
      }
      إذا(nread == 0) {
        /* tunnel closed */
        أرجع CURLE_OK;
      }

      حافظہ_نقل(&remote_addr, ctx->connected_رستہ.remote.addr,
             ctx->connected_رستہ.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_رستہ.remote.addrlen;
      نتیجہ = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      إذا(نتیجہ)
        أرجع نتیجہ;

      إذا(!ctx->q.got_پہلا_byte) {
        ctx->q.got_پہلا_byte = TRUE;
        ctx->q.پہلا_byte_at = ctx->q.آخری_op;
      }
      ctx->q.آخری_io = ctx->q.آخری_op;
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
CURLcode Curl_cf_ngtcp2_cmn_سیٹ_expiry(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *انکڑے,
                                       بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  إذا(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, انکڑے);
    pktx = &local_pktx;
  }
  وإلا {
    Curl_cf_ngtcp2_io_ctx_update_ویلا(انکڑے, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  إذا(expiry != UINT64_MAX) {
    إذا(expiry <= pktx->ts) {
      CURLcode نتیجہ;
      صحيح rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      إذا(rv) {
        failf(انکڑے, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_سیٹ(cf, انکڑے, rv);
        أرجع CURLE_SEND_ERROR;
      }
      نتیجہ = Curl_cf_ngtcp2_ترقی_ingress(cf, انکڑے, pktx);
      إذا(!نتیجہ)
        نتیجہ = Curl_cf_ngtcp2_ترقی_egress(cf, انکڑے, pktx);
      إذا(نتیجہ) {
        /* a verify failure during ingress must win over generic errors */
        إذا(ctx->tls_vrfy_نتیجہ)
          نتیجہ = ctx->tls_vrfy_نتیجہ;
        أرجع نتیجہ;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    إذا(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      إذا(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_سیٹ(انکڑے, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  أرجع CURLE_OK;
}

ساكن فراغ cf_ngtcp2_تیاری_keep_alive(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *انکڑے)
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
    ngtcp2_conn_سیٹ_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(انکڑے, cf, "no peer idle timeout, unset keep-alive");
  }
  وإلا إذا(!Curl_u32_ptrset_گنتی(&ctx->streams)) {
    ngtcp2_conn_سیٹ_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(انکڑے, cf, "no active streams, unset keep-alive");
  }
  وإلا {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_سیٹ_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(انکڑے, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_دھارا_تیاری(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *انکڑے,
                                        بنية h3_دھارا_ctx **pstream)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);

  *pstream = عدم;
  إذا(!انکڑے)
    أرجع CURLE_FAILED_INIT;

  إذا(دھارا) {
    *pstream = دھارا;
    أرجع CURLE_OK;
  }

  دھارا = curlx_calloc(1, حجم(*دھارا));
  إذا(!دھارا)
    أرجع CURLE_باہرلا_دا_حافظہ;

  دھارا->id = -1;
  دھارا->xfer_id = انکڑے->id;
  دھارا->mid = انکڑے->mid;
  دھارا->rx_وتھ = 0;
  دھارا->rx_وتھ_max = H3_دھارا_کھڑکی_ناپ_INITIAL;
  دھارا->tx_وچ_flight_ideal = H3_دھارا_SEND_BUF_INITIAL;

  /* on send, we control how much we put into the buffer */
  Curl_bufq_initp(&دھارا->sendbuf, &ctx->دھارا_bufcp,
                  H3_دھارا_SEND_CHUNKS, BUFQ_OPT_NONE);
  دھارا->tx_وچ_flight_ناپ = 0;
  Curl_h1_req_parse_init(&دھارا->h1, H1_PARSE_DEFAULT_MAX_سطر_LEN);

  إذا(Curl_u32_ptrset_سیٹ(&ctx->streams, انکڑے->mid, دھارا)) {
    Curl_cf_ngtcp2_h3_دھارا_ctx_free(دھارا);
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  إذا(Curl_u32_ptrset_گنتی(&ctx->streams) == 1)
    cf_ngtcp2_تیاری_keep_alive(cf, انکڑے);

  *pstream = دھارا;
  أرجع CURLE_OK;
}

فراغ Curl_cf_ngtcp2_h3_دھارا_close(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے,
                                    بنية h3_دھارا_ctx *دھارا)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(انکڑے);
  DEBUGASSERT(دھارا);
  إذا(!دھارا->closed && ctx->qconn && ctx->h3conn) {
    CURLcode نتیجہ;

    nghttp3_conn_سیٹ_دھارا_user_انکڑے(ctx->h3conn, دھارا->id, عدم);
    ngtcp2_conn_سیٹ_دھارا_user_انکڑے(ctx->qconn, دھارا->id, عدم);
    دھارا->closed = TRUE;
    (فراغ)ngtcp2_conn_shutdown_دھارا(ctx->qconn, 0, دھارا->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    نتیجہ = Curl_cf_ngtcp2_ترقی_egress(cf, انکڑے, عدم);
    إذا(نتیجہ)
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] cancel stream -> %d",
                  دھارا->id, (صحيح)نتیجہ);
  }
}

فراغ Curl_cf_ngtcp2_h3_دھارا_پورا(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);
  (فراغ)cf;
  إذا(دھارا) {
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] easy handle is done", دھارا->id);
    Curl_cf_ngtcp2_h3_دھارا_close(cf, انکڑے, دھارا);
    Curl_u32_ptrset_unset(&ctx->streams, انکڑے->mid);
    إذا(!Curl_u32_ptrset_گنتی(&ctx->streams))
      cf_ngtcp2_تیاری_keep_alive(cf, انکڑے);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *انکڑے,
                                      bool *داخلہ_pending)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ثابت ngtcp2_transport_params *rp;
  بنية cf_call_انکڑے save;

  CF_انکڑے_SAVE(save, cf, انکڑے);
  *داخلہ_pending = FALSE;
  إذا(!ctx->qconn || ctx->shutdown_started)
    اذهب باہرلا;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  إذا(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(انکڑے), &ctx->q.آخری_io);
    إذا(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      إذا((uint64_t)idletime_ms > max_idle_ms)
        اذهب باہرلا;
    }
  }

  إذا(!cf->اگلا || !cf->اگلا->cft->is_alive(cf->اگلا, انکڑے, داخلہ_pending))
    اذهب باہرلا;

  alive = TRUE;
  إذا(*داخلہ_pending) {
    CURLcode نتیجہ;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *داخلہ_pending = FALSE;
    نتیجہ = Curl_cf_ngtcp2_ترقی_ingress(cf, انکڑے, عدم);
    CURL_TRC_CF(انکڑے, cf, "is_alive, progress ingress -> %d", (صحيح)نتیجہ);
    alive = نتیجہ ? FALSE : TRUE;
  }

باہرلا:
  CF_انکڑے_RESTORE(cf, save);
  أرجع alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(بنية cf_ngtcp2_ctx *ctx,
                                      بنية Curl_easy *انکڑے)
{
  int64_t ctrl_دھارا_id, qpack_enc_دھارا_id, qpack_dec_دھارا_id;
  صحيح rc;

  rc = ngtcp2_conn_open_uni_دھارا(ctx->qconn, &ctrl_دھارا_id, عدم);
  إذا(rc) {
    failf(انکڑے, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_دھارا(ctx->h3conn, ctrl_دھارا_id);
  إذا(rc) {
    failf(انکڑے, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_دھارا(ctx->qconn, &qpack_enc_دھارا_id, عدم);
  إذا(rc) {
    failf(انکڑے, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_دھارا(ctx->qconn, &qpack_dec_دھارا_id, عدم);
  إذا(rc) {
    failf(انکڑے, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_دھارا_id,
                                       qpack_dec_دھارا_id);
  إذا(rc) {
    failf(انکڑے, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_CONNECT_جواب_MS:
    إذا((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_پہلا_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.پہلا_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < ودھ_توں_ودھ_عدد) ? (صحيح)ms : ودھ_توں_ودھ_عدد;
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CF_QUERY_REALLY_CONNECTED:
    إذا(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_پہلا_byte;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, انکڑے, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_REPORT_STATS:
    إذا(cf->connected && !ctx->stats_reported) {
      إذا((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_پہلا_byte) {
        Curl_pgrs_ویلا_Was(انکڑے, TIMER_CONNECT, ctx->q.پہلا_byte_at);
        ctx->stats_reported = TRUE;
      }
      وإلا إذا(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        إذا(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrs_ویلا_Was(انکڑے, TIMER_CONNECT, ctx->q.پہلا_byte_at);
        Curl_pgrs_ویلا_Was(انکڑے, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتیجہ;
}

#جے_انت /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
