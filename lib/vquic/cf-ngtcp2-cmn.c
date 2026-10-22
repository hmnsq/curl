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

#جيڪڏهن_وصف_ٿيل USE_OPENSSL
#ضمن <openssl/err.h>
#إذا defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ضمن <ngtcp2/ngtcp2_crypto_boringssl.h>
#وإلاإذا defined(OPENSSL_QUIC_API2)
#ضمن <ngtcp2/ngtcp2_crypto_ossl.h>
#وإلا
#ضمن <ngtcp2/ngtcp2_crypto_quictls.h>
#جيڪڏهن_پورو
#ضمن "vtls/openssl.h"
#وإلاإذا defined(USE_GNUTLS)
#ضمن <ngtcp2/ngtcp2_crypto_gnutls.h>
#ضمن "vtls/gtls.h"
#وإلاإذا defined(USE_WOLFSSL)
#ضمن <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ضمن "vtls/wolfssl.h"
#جيڪڏهن_پورو

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
  ثابت ngtcp2_info *ng2 = ngtcp2_نسخو(0);
  ثابت nghttp3_info *ht3 = nghttp3_نسخو(0);
  (فراغ)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->نسخو_str, ht3->نسخو_str);
}

فراغ Curl_cf_ngtcp2_h3_وهڪرو_ctx_free(بنية h3_وهڪرو_ctx *وهڪرو)
{
  Curl_bufq_free(&وهڪرو->sendbuf);
  Curl_h1_req_parse_free(&وهڪرو->h1);
  curlx_free(وهڪرو);
}

ساكن فراغ h3_وهڪرو_سيٽ_free(uint32_t id, فراغ *وهڪرو)
{
  (فراغ)id;
  DEBUGASSERT(وهڪرو);
  Curl_cf_ngtcp2_h3_وهڪرو_ctx_free((بنية h3_وهڪرو_ctx *)وهڪرو);
}

ساكن bool cf_ngtcp2_h3_err_is_fatal(صحيح ڪوڊ)
{
  أرجع (NGHTTP3_ERR_FATAL >= ڪوڊ) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_وهڪرو == ڪوڊ);
}

فراغ Curl_cf_ngtcp2_h3_err_سيٽ(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات, صحيح ڪوڊ)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  إذا(!ctx->آخري_error.error_ڪوڊ) {
    ngtcp2_ccerr_سيٽ_application_error(&ctx->آخري_error,
      nghttp3_err_infer_quic_app_error_ڪوڊ(ڪوڊ), عدم, 0);
  }
  إذا(cf_ngtcp2_h3_err_is_fatal(ڪوڊ))
    Curl_cf_ngtcp2_cmn_conn_close(cf, معلومات);
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
  ctx->نسخو_2 = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->وهڪرو_bufcp, H3_وهڪرو_CHUNK_سائيز,
                  H3_وهڪرو_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_سرو);
  Curl_u32_ptrset_init(&ctx->streams, h3_وهڪرو_سيٽ_free);
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
#جيڪڏهن_وصف_ٿيل OPENSSL_QUIC_API2
    إذا(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = عدم;
    }
#جيڪڏهن_پورو
    Curl_bufcp_free(&ctx->وهڪرو_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    curlx_safefree(ctx->tunnel_inbuf);
    ctx->tunnel_inbuf_len = 0;
  }
}

بنية Curl_easy *Curl_cf_ngtcp2_get_xfer(بنية Curl_cfilter *cf,
                                          فراغ *وهڪرو_user_معلومات,
                                          بنية h3_وهڪرو_ctx **pstream)
{
  بنية h3_وهڪرو_ctx *وهڪرو = وهڪرو_user_معلومات;

  إذا(pstream)
    *pstream = وهڪرو;
  إذا(وهڪرو && (وهڪرو->mid != UINT32_MAX)) {
    بنية Curl_easy *معلومات, *call_معلومات = CF_معلومات_CURRENT(cf);
    إذا(!call_معلومات || !call_معلومات->multi) {
      DEBUGASSERT(0);
      أرجع عدم;
    }
    معلومات = Curl_multi_get_easy(call_معلومات->multi, وهڪرو->mid);
    إذا(معلومات && (معلومات->id == وهڪرو->xfer_id))
      أرجع معلومات;
    DEBUGASSERT(0);
  }
  أرجع عدم;
}

ساكن ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  بنية Curl_cfilter *cf = conn_ref->user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  أرجع ctx->qconn;
}

#جيڪڏهن_وصف_ٿيل DEBUG_NGTCP2
ساكن فراغ quic_printf(فراغ *user_معلومات, ثابت حرف *fmt, ...)
{
  va_list ap;
  (فراغ)user_معلومات;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#جيڪڏهن_پورو

ساكن فراغ qlog_callback(فراغ *user_معلومات, uint32_t flags,
                          ثابت فراغ *معلومات, size_t datalen)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  (فراغ)flags;
  إذا(ctx->qlogfd != -1) {
    ssize_t rc = اكتب(ctx->qlogfd, معلومات, datalen);
    إذا(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

ساكن فراغ quic_settings(بنية cf_ngtcp2_ctx *ctx,
                          بنية Curl_easy *معلومات,
                          بنية cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#جيڪڏهن_وصف_ٿيل DEBUG_NGTCP2
  s->log_printf = quic_printf;
#وإلا
  s->log_printf = عدم;
#جيڪڏهن_پورو

  s->initial_ts = pktx->ts;
  s->handshake_timeout = (معلومات->سيٽ.connecttimeout > 0) ?
    معلومات->سيٽ.connecttimeout * NGTCP2_MILLISECONDS : QUIC_HANDSHAKE_TIMEOUT;
  s->max_دري = H3_CONN_دري_سائيز_MAX;
  s->max_وهڪرو_دري = 0; /* disable ngtcp2 auto-tuning of window */
  s->no_pmtud = FALSE;
#جيڪڏهن_وصف_ٿيل NGTCP2_SETTINGS_V3
  /* try ten times the ngtcp2 defaults here for problems with Caddy */
  s->glitch_ratelim_مدو = 1000 * 10;
  s->glitch_ratelim_شرح = 33 * 10;
#جيڪڏهن_پورو
  t->initial_max_معلومات = s->max_دري;
  t->initial_max_وهڪرو_معلومات_bidi_local = H3_وهڪرو_دري_سائيز_INITIAL;
  t->initial_max_وهڪرو_معلومات_bidi_remote = H3_وهڪرو_دري_سائيز_INITIAL;
  t->initial_max_وهڪرو_معلومات_uni = t->initial_max_معلومات;
  t->initial_max_streams_bidi = QUIC_MAX_STREAMS;
  t->initial_max_streams_uni = QUIC_MAX_STREAMS;
  t->max_idle_timeout = 0; /* no idle timeout from our side */
  إذا(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#إذا defined(_MSC_VER) && defined(_DLL)
#هدايت warning(ڌڪو)
#هدايت warning(disable:4232) /* MSVC extension, dllimport identity */
#جيڪڏهن_پورو

ساكن صحيح cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, فراغ *user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf ? cf->ctx : عدم;
  بنية Curl_easy *معلومات;

  (فراغ)tconn;
  DEBUGASSERT(ctx);
  معلومات = CF_معلومات_CURRENT(cf);
  DEBUGASSERT(معلومات);
  إذا(!ctx || !معلومات)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(معلومات);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, معلومات);

  ctx->tls_vrfy_نتيجو = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    معلومات, &ctx->ssl_peer);
  إذا(ctx->tls_vrfy_نتيجو)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  إذا(Curl_trc_is_verbose(معلومات)) {
    ثابت ngtcp2_transport_params *rp;
    rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
    CURL_TRC_CF(معلومات, cf, "handshake complete after %" FMT_TIMEDIFF_T
                "ms, remote transport[max_udp_payload=%" PRIu64
                ", initial_max_data=%" PRIu64 "]",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at),
                rp->max_udp_payload_سائيز, rp->initial_max_معلومات);
  }
#جيڪڏهن_پورو

  /* In case of earlydata, where we simulate being connected, update
   * the handshake time when we really did connect */
  إذا(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrs_وقت_Was(معلومات, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  إذا(ctx->use_earlydata) {
#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    إذا(SSL_get_early_معلومات_حالت(ctx->tls.ossl.ssl) == SSL_EARLY_معلومات_NOT_موڪليل)
      CURL_TRC_CF(معلومات, cf, "OpenSSL did not send early data");
#جيڪڏهن_پورو

#إذا NGTCP2_نسخو_انگ >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_معلومات_rejected2(ctx->qconn);
#وإلا /* older NGTCP2 */
    {
#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      صحيح ossl_early_حالت = SSL_get_early_معلومات_حالت(ctx->tls.ossl.ssl);
      إذا(ossl_early_حالت == SSL_EARLY_معلومات_NOT_موڪليل)
        CURL_TRC_CF(معلومات, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_حالت == SSL_EARLY_معلومات_ACCEPTED);
#وإلاإذا defined(USE_GNUTLS)
      صحيح flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_معلومات);
#وإلاإذا defined(USE_WOLFSSL)
#جيڪڏهن_وصف_ٿيل WOLFSSL_EARLY_معلومات
      ctx->earlydata_accepted =
        (wolf_SSL_get_early_معلومات_حالت(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_معلومات_REJECTED);
#وإلا
      DEBUGASSERT(0); /* should not come here if ED is disabled. */
      ctx->earlydata_accepted = FALSE;
#جيڪڏهن_پورو /* WOLFSSL_EARLY_DATA */
#جيڪڏهن_پورو /* OPENSSL or GNUTLS or WOLFSSL */
    }
#جيڪڏهن_پورو /* older NGTCP2 */
    CURL_TRC_CF(معلومات, cf, "server did%s accept %zu bytes of early data",
                ctx->earlydata_accepted ? "" : " not", ctx->earlydata_skip);
    Curl_pgrs_Early_معلومات(معلومات, ctx->earlydata_accepted ?
                              (curl_off_t)ctx->earlydata_skip :
                             -(curl_off_t)ctx->earlydata_skip);
  }
  أرجع 0;
}

ساكن صحيح cb_recv_وهڪرو_معلومات(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t وهڪرو_id, uint64_t وٿي,
                               ثابت uint8_t *buf, size_t buflen,
                               فراغ *user_معلومات, فراغ *وهڪرو_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  صحيح fin = (flags & NGTCP2_وهڪرو_معلومات_FLAG_FIN) ? 1 : 0;
  بنية h3_وهڪرو_ctx *وهڪرو;
  بنية Curl_easy *معلومات =
    Curl_cf_ngtcp2_get_xfer(cf, وهڪرو_user_معلومات, &وهڪرو);

  (فراغ)وٿي;
  rc = nghttp3_conn_read_وهڪرو(ctx->h3conn, وهڪرو_id, buf, buflen, fin);
  إذا(rc < 0) {
    إذا(معلومات && وهڪرو) {
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  وهڪرو_id, وهڪرو->reset, وهڪرو->closed);
    }
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  إذا(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_وهڪرو_وٿي(tconn, وهڪرو_id, nconsumed);
    ngtcp2_conn_extend_max_وٿي(tconn, nconsumed);
    إذا(وهڪرو) {
      وهڪرو->rx_وٿي += nconsumed;
      وهڪرو->rx_وٿي_max += nconsumed;
    }
  }
  أرجع 0;
}

ساكن صحيح cb_acked_وهڪرو_معلومات_وٿي(ngtcp2_conn *tconn, int64_t وهڪرو_id,
                                       uint64_t وٿي, uint64_t datalen,
                                       فراغ *user_معلومات, فراغ *وهڪرو_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)وهڪرو_id;
  (فراغ)tconn;
  (فراغ)وٿي;
  (فراغ)datalen;
  (فراغ)وهڪرو_user_معلومات;

  rv = nghttp3_conn_جوڙيو_ack_وٿي(ctx->h3conn, وهڪرو_id, datalen);
  إذا(rv && rv != NGHTTP3_ERR_وهڪرو_NOT_مليو) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_وهڪرو_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t وهڪرو_id, uint64_t app_error_ڪوڊ,
                           فراغ *user_معلومات, فراغ *وهڪرو_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = Curl_cf_ngtcp2_get_xfer(cf, وهڪرو_user_معلومات, عدم);
  صحيح rv;

  (فراغ)tconn;
  /* stream is closed... */
  إذا(!معلومات)
    معلومات = CF_معلومات_CURRENT(cf);
  إذا(!معلومات)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  إذا(!(flags & NGTCP2_وهڪرو_CLOSE_FLAG_APP_ERROR_ڪوڊ_سيٽ)) {
    app_error_ڪوڊ = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_وهڪرو(ctx->h3conn, وهڪرو_id, app_error_ڪوڊ);
  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", وهڪرو_id, app_error_ڪوڊ, rv);
  إذا(rv && rv != NGHTTP3_ERR_وهڪرو_NOT_مليو) {
    Curl_cf_ngtcp2_h3_err_سيٽ(cf, معلومات, rv);
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

#جيڪڏهن_وصف_ٿيل NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
ساكن صحيح cb_وهڪرو_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t وهڪرو_id,
                           uint64_t rx_app_error_ڪوڊ,
                           uint64_t tx_app_error_ڪوڊ,
                           فراغ *user_معلومات, فراغ *وهڪرو_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = Curl_cf_ngtcp2_get_xfer(cf, وهڪرو_user_معلومات, عدم);
  uint64_t h3_app_error_ڪوڊ = NGHTTP3_H3_NO_ERROR;
  صحيح rv;

  (فراغ)tconn;
  (فراغ)tx_app_error_ڪوڊ;
  /* stream is closed... */
  إذا(!معلومات)
    معلومات = CF_معلومات_CURRENT(cf);
  إذا(!معلومات)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  إذا(flags & NGTCP2_وهڪرو_CLOSE2_FLAG_RX_APP_ERROR_ڪوڊ_سيٽ)
    h3_app_error_ڪوڊ = rx_app_error_ڪوڊ;

  rv = nghttp3_conn_close_وهڪرو(ctx->h3conn, وهڪرو_id, h3_app_error_ڪوڊ);
  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", وهڪرو_id, h3_app_error_ڪوڊ, rv);
  إذا(rv && rv != NGHTTP3_ERR_وهڪرو_NOT_مليو) {
    Curl_cf_ngtcp2_h3_err_سيٽ(cf, معلومات, rv);
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}
#جيڪڏهن_پورو

ساكن صحيح cb_وهڪرو_reset(ngtcp2_conn *tconn, int64_t وهڪرو_id,
                           uint64_t final_سائيز, uint64_t app_error_ڪوڊ,
                           فراغ *user_معلومات, فراغ *وهڪرو_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = Curl_cf_ngtcp2_get_xfer(cf, وهڪرو_user_معلومات, عدم);
  صحيح rv;
  (فراغ)tconn;
  (فراغ)final_سائيز;
  (فراغ)app_error_ڪوڊ;

  rv = nghttp3_conn_shutdown_وهڪرو_read(ctx->h3conn, وهڪرو_id);
  إذا(معلومات)
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] reset -> %d", وهڪرو_id, rv);
  إذا(rv && rv != NGHTTP3_ERR_وهڪرو_NOT_مليو) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_وهڪرو_stop_sending(ngtcp2_conn *tconn, int64_t وهڪرو_id,
                                  uint64_t app_error_ڪوڊ, فراغ *user_معلومات,
                                  فراغ *وهڪرو_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)tconn;
  (فراغ)app_error_ڪوڊ;
  (فراغ)وهڪرو_user_معلومات;

  rv = nghttp3_conn_shutdown_وهڪرو_read(ctx->h3conn, وهڪرو_id);
  إذا(rv && rv != NGHTTP3_ERR_وهڪرو_NOT_مليو) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            فراغ *user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);

  (فراغ)tconn;
  ctx->max_bidi_streams = max_streams;
  إذا(معلومات)
    CURL_TRC_CF(معلومات, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->استعمال_ٿيل_bidi_streams);
  أرجع 0;
}

ساكن صحيح cb_extend_max_وهڪرو_معلومات(ngtcp2_conn *tconn, int64_t وهڪرو_id,
                                     uint64_t max_معلومات, فراغ *user_معلومات,
                                     فراغ *وهڪرو_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_وهڪرو_ctx *وهڪرو;
  بنية Curl_easy *معلومات =
    Curl_cf_ngtcp2_get_xfer(cf, وهڪرو_user_معلومات, &وهڪرو);
  صحيح rv;
  (فراغ)tconn;
  (فراغ)max_معلومات;

  rv = nghttp3_conn_unblock_وهڪرو(ctx->h3conn, وهڪرو_id);
  إذا(rv && rv != NGHTTP3_ERR_وهڪرو_NOT_مليو) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  إذا(وهڪرو && وهڪرو->quic_flow_blocked) {
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] unblock quic flow", وهڪرو_id);
    وهڪرو->quic_flow_blocked = FALSE;
    إذا(معلومات)
      Curl_multi_mark_dirty(معلومات);
  }
  أرجع 0;
}

ساكن فراغ cb_rand(uint8_t *dest, size_t destlen,
                    ثابت ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode نتيجو;
  (فراغ)rand_ctx;

  نتيجو = Curl_rand(عدم, dest, destlen);
  إذا(نتيجو) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
ساكن صحيح cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    فراغ *user_معلومات)
{
  CURLcode نتيجو;
  (فراغ)tconn;
  (فراغ)user_معلومات;

  نتيجو = Curl_rand(عدم, cid->معلومات, cidlen);
  إذا(نتيجو)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  نتيجو = Curl_rand(عدم, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  إذا(نتيجو)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  أرجع 0;
}

#جيڪڏهن_وصف_ٿيل NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
ساكن صحيح cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  بنية ngtcp2_stateless_reset_token *token, size_t cidlen, فراغ *user_معلومات)
{
  CURLcode نتيجو;
  (فراغ)tconn;
  (فراغ)user_معلومات;

  نتيجو = Curl_rand(عدم, cid->معلومات, cidlen);
  إذا(نتيجو)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  نتيجو = Curl_rand(عدم, token->معلومات, حجم(token->معلومات));
  إذا(نتيجو)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  أرجع 0;
}
#جيڪڏهن_پورو

ساكن صحيح cb_recv_rx_ڪنجي(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          فراغ *user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf ? cf->ctx : عدم;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  (فراغ)tconn;

  إذا(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    أرجع 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(معلومات);
  إذا(ctx && معلومات && !ctx->h3conn && ctx->init_h3_conn_cb) {
    إذا(ctx->init_h3_conn_cb(cf, معلومات, ctx))
      أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  أرجع 0;
}

#جيڪڏهن_وصف_ٿيل CURL_HAVE_DIAG
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Wmissing-field-initializers"
#جيڪڏهن_پورو
ساكن ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  عدم, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_معلومات_cb,
  cb_ngtcp2_handshake_completed,
  عدم, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_نقاب_cb,
  cb_recv_وهڪرو_معلومات,
  cb_acked_وهڪرو_معلومات_وٿي,
  عدم, /* stream_open */
  cb_وهڪرو_close,
  عدم, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  عدم, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  عدم, /* remove_connection_id */
  ngtcp2_crypto_update_ڪنجي_cb, /* update_key */
  عدم, /* path_validation */
  عدم, /* select_preferred_addr */
  cb_وهڪرو_reset,
  عدم, /* extend_max_remote_streams_bidi */
  عدم, /* extend_max_remote_streams_uni */
  cb_extend_max_وهڪرو_معلومات,
  عدم, /* dcid_status */
  عدم, /* handshake_confirmed */
  عدم, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  عدم, /* recv_datagram */
  عدم, /* ack_datagram */
  عدم, /* lost_datagram */
  ngtcp2_crypto_get_رستو_challenge_معلومات_cb,
  cb_وهڪرو_stop_sending,
  عدم, /* version_negotiation */
  cb_recv_rx_ڪنجي,
  عدم, /* recv_tx_key */
  عدم, /* early_data_rejected */
#جيڪڏهن_وصف_ٿيل NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  عدم, /* begin_path_validation */
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  عدم, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  عدم, /* dcid_status2 */
  ngtcp2_crypto_get_رستو_challenge_data2_cb, /* get_path_challenge_data2 */
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  عدم, /* recv_stop_sending */
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_وهڪرو_close2, /* is called instead of cb_stream_close when set */
#جيڪڏهن_پورو
};
#جيڪڏهن_وصف_ٿيل CURL_HAVE_DIAG
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو

#إذا defined(_MSC_VER) && defined(_DLL)
#هدايت warning(ڪڍو)
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ساكن صحيح quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  بنية Curl_cfilter *cf;
  بنية cf_ngtcp2_ctx *ctx;
  بنية Curl_easy *معلومات;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_معلومات(ssl);
  cf = cref ? cref->user_معلومات : عدم;
  ctx = cf ? cf->ctx : عدم;
  معلومات = cf ? CF_معلومات_CURRENT(cf) : عدم;
  إذا(cf && معلومات && ctx) {
    بنانشان حرف *quic_tp = عدم;
    size_t quic_tp_len = 0;
#جيڪڏهن_وصف_ٿيل HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     حجم(tpbuf));
    إذا(tplen < 0)
      CURL_TRC_CF(معلومات, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((صحيح)tplen));
    وإلا {
      quic_tp = (بنانشان حرف *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#جيڪڏهن_پورو
    Curl_ossl_جوڙيو_session(cf, معلومات, &ctx->tls.ossl, ctx->ssl_peer.scache_ڪنجي,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, عدم);
  }
  أرجع 0;
}
#جيڪڏهن_پورو /* USE_OPENSSL */

#جيڪڏهن_وصف_ٿيل USE_GNUTLS

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
ساكن ثابت حرف *gtls_hs_نياپو_name(صحيح mtype)
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
#جيڪڏهن_پورو

ساكن صحيح quic_gtls_handshake_cb(gnutls_session_t session, بنانشان صحيح htype,
                                  بنانشان when, بنانشان صحيح incoming,
                                  ثابت gnutls_datum_t *نياپو)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  بنية Curl_cfilter *cf = conn_ref ? conn_ref->user_معلومات : عدم;
  بنية cf_ngtcp2_ctx *ctx = cf ? cf->ctx : عدم;

  (فراغ)نياپو;
  (فراغ)incoming;
  إذا(when && cf && ctx) { /* after message has been processed */
    بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
    DEBUGASSERT(معلومات);
    إذا(!معلومات)
      أرجع 0;
    CURL_TRC_CF(معلومات, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_نياپو_name(htype), htype);
    بدل(htype) {
    حالة GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      بنانشان حرف *quic_tp = عدم;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       حجم(tpbuf));
      إذا(tplen < 0)
        CURL_TRC_CF(معلومات, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحيح)tplen));
      وإلا {
        quic_tp = (بنانشان حرف *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (فراغ)Curl_gtls_cache_session(cf, معلومات, ctx->ssl_peer.scache_ڪنجي,
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
#جيڪڏهن_پورو /* USE_GNUTLS */

#جيڪڏهن_وصف_ٿيل USE_WOLFSSL
ساكن صحيح wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolf_SSL_get_app_معلومات(ssl);
  بنية Curl_cfilter *cf = conn_ref ? conn_ref->user_معلومات : عدم;

  DEBUGASSERT(cf);
  إذا(cf && session) {
    بنية cf_ngtcp2_ctx *ctx = cf->ctx;
    بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
    DEBUGASSERT(معلومات);
    إذا(معلومات && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      بنانشان حرف *quic_tp = عدم;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       حجم(tpbuf));
      إذا(tplen < 0)
        CURL_TRC_CF(معلومات, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحيح)tplen));
      وإلا {
        quic_tp = (بنانشان حرف *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (فراغ)Curl_wssl_cache_session(cf, معلومات, ctx->ssl_peer.scache_ڪنجي,
                                    session, wolf_SSL_نسخو(ssl),
                                    "h3", quic_tp, quic_tp_len, عدم);
    }
  }
  أرجع 0;
}
#جيڪڏهن_پورو /* USE_WOLFSSL */

ساكن CURLcode cf_ngtcp2_tls_ctx_تياري(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        فراغ *user_معلومات)
{
  بنية curl_tls_ctx *ctx = user_معلومات;

#جيڪڏهن_وصف_ٿيل USE_OPENSSL
#إذا defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  إذا(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(معلومات, "ngtcp2_crypto_boringssl_configure_client_context failed");
    أرجع CURLE_FAILED_INIT;
  }
#وإلاإذا defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#وإلا
  إذا(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(معلومات, "ngtcp2_crypto_quictls_configure_client_context failed");
    أرجع CURLE_FAILED_INIT;
  }
#جيڪڏهن_پورو /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  إذا(Curl_ssl_scache_use(cf, معلومات)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_سيٽ_session_cache_گهڻائي(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_اندروني);
    SSL_CTX_sess_سيٽ_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#وإلاإذا defined(USE_GNUTLS)
  إذا(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(معلومات, "ngtcp2_crypto_gnutls_configure_client_session failed");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(Curl_ssl_scache_use(cf, معلومات)) {
    gnutls_handshake_سيٽ_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#وإلاإذا defined(USE_WOLFSSL)
  إذا(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(معلومات, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(Curl_ssl_scache_use(cf, معلومات)) {
    /* Register to get notified when a new session is received */
    wolf_SSL_CTX_sess_سيٽ_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#جيڪڏهن_پورو
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ngtcp2_on_session_reuse(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *معلومات,
                                           بنية alpn_spec *alpns,
                                           بنية Curl_ssl_session *scs,
                                           bool *do_early_معلومات)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  *do_early_معلومات = FALSE;
#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_GNUTLS
  ctx->earlydata_max =
    gnutls_رڪارڊ_get_max_early_معلومات_سائيز(ctx->tls.gtls.session);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_WOLFSSL
#جيڪڏهن_وصف_ٿيل WOLFSSL_EARLY_معلومات
  ctx->earlydata_max = scs->earlydata_max;
#وإلا
  ctx->earlydata_max = 0;
#جيڪڏهن_پورو /* WOLFSSL_EARLY_DATA */
#جيڪڏهن_پورو
#إذا defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  إذا(!ctx->earlydata_max) {
    CURL_TRC_CF(معلومات, cf, "SSL session does not allow earlydata");
  }
  وإلا إذا(!Curl_alpn_شامل_آهي_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(معلومات, cf, "SSL session from different ALPN, no early data");
  }
  وإلا إذا(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(معلومات, cf, "no 0RTT transport parameters, no early data");
  }
  وإلا {
    صحيح rv;
    rv = ngtcp2_conn_decode_and_سيٽ_0rtt_transport_params(
      ctx->qconn, (ثابت uint8_t *)scs->quic_tp, scs->quic_tp_len);
    إذا(rv)
      CURL_TRC_CF(معلومات, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    وإلا إذا(ctx->init_h3_conn_cb) {
      infof(معلومات, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      نتيجو = ctx->init_h3_conn_cb(cf, معلومات, ctx);
      إذا(!نتيجو) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_معلومات = TRUE;
      }
    }
    وإلا { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_معلومات = TRUE;
    }
  }
#وإلا /* not supported in the TLS backend */
  (فراغ)معلومات;
  (فراغ)ctx;
  (فراغ)scs;
  (فراغ)alpns;
#جيڪڏهن_پورو
  أرجع نتيجو;
}

/*
 * Might be called twice for happy eyeballs.
 */
ساكن CURLcode cf_connect_شروعات(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rc;
  صحيح rv;
  CURLcode نتيجو;
  ثابت بنية Curl_sockaddr_ex *sockaddr = عدم;
  صحيح qfd;
  ساكن ثابت بنية alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  نتيجو = Curl_rand(معلومات, ctx->dcid.معلومات, NGTCP2_MAX_CIDLEN);
  إذا(نتيجو)
    أرجع نتيجو;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  نتيجو = Curl_rand(معلومات, ctx->scid.معلومات, NGTCP2_MAX_CIDLEN);
  إذا(نتيجو)
    أرجع نتيجو;

  (فراغ)Curl_qlogdir(معلومات, ctx->scid.معلومات, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, معلومات, pktx);

  نتيجو = Curl_vquic_ctx_init(معلومات, &ctx->q);
  إذا(نتيجو)
    أرجع نتيجو;

  /* Query socket and remote address from sub-chain */
  إذا(Curl_cf_socket_peek(cf->ايندڙ, معلومات, &ctx->q.sockfd, &sockaddr, عدم)) {
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

    ngtcp2_addr_init(&ctx->connected_رستو.local,
                     (بنية sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_رستو.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  وإلا {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ثابت بنية Curl_sockaddr_ex *remote = عدم;
    إذا(cf->ايندڙ->cft->query(cf->ايندڙ, معلومات, CF_QUERY_REMOTE_ADDR, عدم,
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
#جيڪڏهن_وصف_ٿيل USE_IPV6
    حالة AF_INET6:
      ((بنية sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = حجم(بنية sockaddr_in6);
      اكسر;
#جيڪڏهن_پورو
    افتراضي:
      أرجع CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_رستو.local,
                     (بنية sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_رستو.remote,
                     &remote->curl_sa_addr,
                     (socklen_t)remote->addrlen);
  }

  rc = ngtcp2_conn_client_new(&ctx->qconn, &ctx->dcid, &ctx->scid,
                              &ctx->connected_رستو,
                              NGTCP2_PROTO_VER_V1, &ng_callbacks,
                              &ctx->settings, &ctx->transport_params,
                              Curl_ngtcp2_mem(), cf);
  إذا(rc)
    أرجع CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_معلومات = cf;

  نتيجو = Curl_vquic_tls_init(&ctx->tls, cf, معلومات,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_تياري, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  إذا(نتيجو)
    أرجع نتيجو;

#إذا defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  إذا(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(معلومات, "ngtcp2_crypto_ossl_ctx_new failed");
    أرجع CURLE_FAILED_INIT;
  }
  ngtcp2_conn_سيٽ_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  إذا(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(معلومات, "ngtcp2_crypto_ossl_configure_client_session failed");
    أرجع CURLE_FAILED_INIT;
  }
#وإلاإذا defined(USE_OPENSSL)
  SSL_سيٽ_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_سيٽ_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#وإلاإذا defined(USE_GNUTLS)
  ngtcp2_conn_سيٽ_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#وإلاإذا defined(USE_WOLFSSL)
  ngtcp2_conn_سيٽ_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#وإلا
#غلطي "ngtcp2 TLS backend not defined"
#جيڪڏهن_پورو

#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  إذا(ctx->use_earlydata &&
     !SSL_سيٽ_quic_tls_early_معلومات_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(معلومات, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#جيڪڏهن_پورو
  ngtcp2_ccerr_default(&ctx->آخري_error);

  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    bool *پورو)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;
  بنية cf_call_معلومات save;
  بنية cf_ngtcp2_io_ctx pktx;

  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the sub-chain */
  إذا(cf->ايندڙ && !cf->ايندڙ->connected) {
    نتيجو = Curl_conn_cf_connect(cf->ايندڙ, معلومات, پورو);
    إذا(نتيجو || !*پورو)
      أرجع نتيجو;
  }

  *پورو = FALSE;

  إذا(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(معلومات, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(معلومات, cf, "need HTTPS-RR, delaying connect");
    أرجع CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, معلومات);
  CF_معلومات_SAVE(save, cf, معلومات);

  إذا(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(معلومات);
    نتيجو = cf_connect_شروعات(cf, معلومات, &pktx);
    إذا(نتيجو)
      اذهب ٻاهريون;
    إذا(cf->connected) {
      *پورو = TRUE;
      اذهب ٻاهريون;
    }
    نتيجو = Curl_cf_ngtcp2_ترقي_egress(cf, معلومات, &pktx);
    /* we do not expect to be able to recv anything yet */
    اذهب ٻاهريون;
  }

  نتيجو = Curl_cf_ngtcp2_ترقي_ingress(cf, معلومات, &pktx);
  إذا(نتيجو)
    اذهب ٻاهريون;

  نتيجو = Curl_cf_ngtcp2_ترقي_egress(cf, معلومات, &pktx);
  إذا(نتيجو)
    اذهب ٻاهريون;

  إذا(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    نتيجو = ctx->tls_vrfy_نتيجو;
    إذا(!نتيجو) {
      CURL_TRC_CF(معلومات, cf, "peer verified");
      cf->connected = TRUE;
      *پورو = TRUE;
    }
  }

ٻاهريون:
  إذا(ctx->tls_vrfy_نتيجو)
    نتيجو = ctx->tls_vrfy_نتيجو;
  إذا(ctx->qconn &&
     ((نتيجو == CURLE_RECV_ERROR) || (نتيجو == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_اندريون_draining_period(ctx->qconn)) {
    ثابت ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    نتيجو = CURLE_COULDNT_CONNECT;
    إذا(cerr) {
      CURL_TRC_CF(معلومات, cf, "connect error, type=%d, code=%" PRIu64,
                  (صحيح)cerr->type, cerr->error_ڪوڊ);
      بدل(cerr->type) {
      حالة NGTCP2_CCERR_TYPE_نسخو_NEGOTIATION:
        CURL_TRC_CF(معلومات, cf, "error in version negotiation");
        اكسر;
      افتراضي:
        إذا(cerr->error_ڪوڊ >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(معلومات, cf, "crypto error, tls alert=%u",
                      (بنانشان صحيح)(cerr->error_ڪوڊ & 0xffU));
        }
        وإلا إذا(cerr->error_ڪوڊ == NGTCP2_CONNECTION_REFUSED) {
          CURL_TRC_CF(معلومات, cf, "connection refused by server");
          /* When a QUIC server instance is shutting down, it may send us a
           * CONNECTION_CLOSE with this code right away. We want
           * to keep on trying in this case. */
          نتيجو = CURLE_WEIRD_SERVER_جواب;
        }
      }
    }
  }

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  إذا(نتيجو) {
    إذا(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      بنية ip_quadruple ip;

      إذا(!Curl_cf_socket_peek(cf->ايندڙ, معلومات, عدم, عدم, &ip))
        infof(معلومات, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_پورٽ, curl_easy_strerror(نتيجو));
    }
  }
#جيڪڏهن_پورو
  إذا(!نتيجو && ctx->qconn) {
    نتيجو = Curl_cf_ngtcp2_cmn_سيٽ_expiry(cf, معلومات, &pktx);
  }
  إذا(نتيجو || *پورو)
    CURL_TRC_CF(معلومات, cf, "connect -> %d, done=%d", (صحيح)نتيجو, *پورو);
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات, bool *پورو)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode نتيجو = CURLE_OK;

  إذا(cf->shutdown || !ctx->qconn) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  إذا(!cf->ايندڙ) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  CF_معلومات_SAVE(save, cf, معلومات);
  *پورو = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, معلومات);

  إذا(!ctx->shutdown_started) {
    حرف بفر[NGTCP2_MAX_UDP_PAYLOAD_سائيز];
    ngtcp2_ssize nwritten;

    إذا(!Curl_bufq_is_خالي_جايون(&ctx->q.sendbuf)) {
      CURL_TRC_CF(معلومات, cf, "shutdown, flushing sendbuf");
      نتيجو = Curl_cf_ngtcp2_ترقي_egress(cf, معلومات, &pktx);
      إذا(!Curl_bufq_is_خالي_جايون(&ctx->q.sendbuf)) {
        CURL_TRC_CF(معلومات, cf, "sending shutdown packets blocked");
        نتيجو = CURLE_OK;
        اذهب ٻاهريون;
      }
      وإلا إذا(نتيجو) {
        CURL_TRC_CF(معلومات, cf, "shutdown, error %d flushing sendbuf",
                    (صحيح)نتيجو);
        *پورو = TRUE;
        اذهب ٻاهريون;
      }
    }

    DEBUGASSERT(Curl_bufq_is_خالي_جايون(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, عدم, /*رستو*/
      عدم, /* pkt_info */
      (uint8_t *)بفر, حجم(بفر),
      &ctx->آخري_error, pktx.ts);
    CURL_TRC_CF(معلومات, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (صحيح)ctx->آخري_error.type,
                ctx->آخري_error.error_ڪوڊ, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    إذا(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      نتيجو = Curl_bufq_write(&ctx->q.sendbuf, (ثابت بنانشان حرف *)بفر,
                               (size_t)nwritten, &n);
      إذا(نتيجو) {
        CURL_TRC_CF(معلومات, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (صحيح)نتيجو);
        اذهب ٻاهريون;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.ورهايو_len = 0;
    }
  }

  إذا(!Curl_bufq_is_خالي_جايون(&ctx->q.sendbuf)) {
    CURL_TRC_CF(معلومات, cf, "shutdown, flushing egress");
    نتيجو = Curl_vquic_flush(cf, معلومات, &ctx->q);
    إذا(نتيجو == CURLE_AGAIN) {
      CURL_TRC_CF(معلومات, cf, "sending shutdown packets blocked");
      نتيجو = CURLE_OK;
      اذهب ٻاهريون;
    }
    وإلا إذا(نتيجو) {
      CURL_TRC_CF(معلومات, cf, "shutdown, error %d flushing sendbuf",
                  (صحيح)نتيجو);
      *پورو = TRUE;
      اذهب ٻاهريون;
    }
  }

  إذا(Curl_bufq_is_خالي_جايون(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(معلومات, cf, "shutdown completely sent off, done");
    *پورو = TRUE;
    نتيجو = CURLE_OK;
  }
ٻاهريون:
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

فراغ Curl_cf_ngtcp2_cmn_conn_close(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  bool پورو;
  Curl_cf_ngtcp2_cmn_shutdown(cf, معلومات, &پورو);
}

ساكن bool cf_ngtcp2_err_is_fatal(صحيح ڪوڊ)
{
  أرجع (NGTCP2_ERR_FATAL >= ڪوڊ) ||
         (NGTCP2_ERR_DROP_CONN == ڪوڊ) ||
         (NGTCP2_ERR_IDLE_CLOSE == ڪوڊ);
}

فراغ Curl_cf_ngtcp2_cmn_err_سيٽ(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات, صحيح ڪوڊ)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  إذا(!ctx->آخري_error.error_ڪوڊ) {
    إذا(NGTCP2_ERR_CRYPTO == ڪوڊ) {
      ngtcp2_ccerr_سيٽ_tls_alert(&ctx->آخري_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 عدم, 0);
    }
    وإلا {
      ngtcp2_ccerr_سيٽ_liberr(&ctx->آخري_error, ڪوڊ, عدم, 0);
    }
  }
  إذا(cf_ngtcp2_err_is_fatal(ڪوڊ))
    Curl_cf_ngtcp2_cmn_conn_close(cf, معلومات);
}

فراغ Curl_cf_ngtcp2_io_ctx_init(بنية cf_ngtcp2_io_ctx *io_ctx,
                                بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->معلومات = معلومات;
  io_ctx->now = *Curl_pgrs_now(معلومات);
  ngtcp2_رستو_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_سيٽ_وقت(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

فراغ Curl_cf_ngtcp2_io_ctx_update_وقت(بنية Curl_easy *معلومات,
                                       بنية cf_ngtcp2_io_ctx *io_ctx,
                                       بنية Curl_cfilter *cf)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(معلومات);
  Curl_vquic_ctx_update_وقت(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#إذا NGTCP2_نسخو_انگ < 0x011100
بنية cf_ngtcp2_sfind_ctx {
  int64_t وهڪرو_id;
  بنية h3_وهڪرو_ctx *وهڪرو;
  uint32_t mid;
};

ساكن bool cf_ngtcp2_sfind(uint32_t mid, فراغ *قدر, فراغ *user_معلومات)
{
  بنية cf_ngtcp2_sfind_ctx *fctx = user_معلومات;
  بنية h3_وهڪرو_ctx *وهڪرو = قدر;

  إذا(fctx->وهڪرو_id == وهڪرو->id) {
    fctx->mid = mid;
    fctx->وهڪرو = وهڪرو;
    أرجع FALSE;
  }
  أرجع TRUE; /* continue */
}

ساكن بنية h3_وهڪرو_ctx *cf_ngtcp2_get_وهڪرو(بنية cf_ngtcp2_ctx *ctx,
                                                  int64_t وهڪرو_id)
{
  بنية cf_ngtcp2_sfind_ctx fctx;
  fctx.وهڪرو_id = وهڪرو_id;
  fctx.وهڪرو = عدم;
  Curl_u32_ptrset_گهمو(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  أرجع fctx.وهڪرو;
}
#وإلا
ساكن بنية h3_وهڪرو_ctx *cf_ngtcp2_get_وهڪرو(بنية cf_ngtcp2_ctx *ctx,
                                                  int64_t وهڪرو_id)
{
  أرجع ngtcp2_conn_get_وهڪرو_user_معلومات(ctx->qconn, وهڪرو_id);
}
#جيڪڏهن_پورو

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
ساكن CURLcode read_pkt_منزل_send(فراغ *userp,
                                 بنانشان حرف *buf, size_t buflen,
                                 size_t *pnread)
{
  بنية cf_ngtcp2_io_ctx *x = userp;
  بنية cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t وهڪرو_id;
  صحيح fin;
  ssize_t n;

  *pnread = 0;
  veccnt = 0;
  وهڪرو_id = -1;
  fin = 0;

  /* ngtcp2 may want to put several frames from different streams into
   * this packet. `NGTCP2_WRITE_STREAM_FLAG_MORE` tells it to do so.
   * When `NGTCP2_ERR_WRITE_MORE` is returned, we *need* to make
   * another iteration.
   * When ngtcp2 is happy (because it has no other frame that would fit
   * or it has nothing more to send), it returns the total length
   * of the assembled packet. This may be 0 if there was nothing to send. */
  لكل(;;) {

    إذا(ctx->h3conn && ngtcp2_conn_get_max_معلومات_کاٻو(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_وهڪرو(ctx->h3conn, &وهڪرو_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      إذا(veccnt < 0) {
        failf(x->معلومات, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((صحيح)veccnt));
        Curl_cf_ngtcp2_h3_err_سيٽ(x->cf, x->معلومات, (صحيح)veccnt);
        أرجع CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_وهڪرو_FLAG_وڌيڪ |
            (fin ? NGTCP2_WRITE_وهڪرو_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_وهڪرو(ctx->qconn, &x->ps.رستو,
                                  عدم, buf, buflen,
                                  &ndatalen, flags, وهڪرو_id,
                                  (ثابت ngtcp2_vec *)vec, veccnt, x->ts);
    إذا(n == 0) {
      /* nothing to send */
      أرجع CURLE_AGAIN;
    }
    وإلا إذا(n < 0) {
      بدل(n) {
      حالة NGTCP2_ERR_وهڪرو_معلومات_BLOCKED: {
        بنية h3_وهڪرو_ctx *وهڪرو;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_بلاڪ_وهڪرو(ctx->h3conn, وهڪرو_id);
        CURL_TRC_CF(x->معلومات, x->cf, "[%" PRId64 "] block quic flow",
                    وهڪرو_id);
        وهڪرو = cf_ngtcp2_get_وهڪرو(ctx, وهڪرو_id);
        إذا(وهڪرو) /* it might be not one of our h3 streams? */
          وهڪرو->quic_flow_blocked = TRUE;
        n = 0;
        اكسر;
      }
      حالة NGTCP2_ERR_وهڪرو_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_وهڪرو_write(ctx->h3conn, وهڪرو_id);
        n = 0;
        اكسر;
      حالة NGTCP2_ERR_WRITE_وڌيڪ:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        اكسر;
      افتراضي:
        DEBUGASSERT(ndatalen == -1);
        failf(x->معلومات, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((صحيح)n));
        Curl_cf_ngtcp2_cmn_err_سيٽ(x->cf, x->معلومات, (صحيح)n);
        أرجع CURLE_SEND_ERROR;
      }
    }

    إذا(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      صحيح rv = nghttp3_conn_جوڙيو_write_وٿي(ctx->h3conn, وهڪرو_id, ndatalen);
      إذا(rv) {
        failf(x->معلومات, "nghttp3_conn_add_write_offset returned error: %s",
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

CURLcode Curl_cf_ngtcp2_ترقي_egress(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_سائيز, رستو_max_payload_سائيز;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_وقتي_حصو;
  CURLcode نتيجو;
  بنية cf_ngtcp2_io_ctx local_pktx;

  إذا(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, معلومات);
    pktx = &local_pktx;
  }
  وإلا {
    Curl_cf_ngtcp2_io_ctx_update_وقت(معلومات, pktx, cf);
    ngtcp2_رستو_storage_zero(&pktx->ps);
  }

  نتيجو = Curl_vquic_flush(cf, معلومات, &ctx->q);
  إذا(نتيجو) {
    إذا(نتيجو == CURLE_AGAIN) {
      Curl_expire_سيٽ(معلومات, EXPIRE_QUIC, 1, &pktx->now);
      أرجع CURLE_OK;
    }
    أرجع نتيجو;
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
  max_payload_سائيز = ngtcp2_conn_get_max_tx_udp_payload_سائيز(ctx->qconn);
  رستو_max_payload_سائيز =
    ngtcp2_conn_get_رستو_max_tx_udp_payload_سائيز(ctx->qconn);
  send_وقتي_حصو = ngtcp2_conn_get_send_وقتي_حصو(ctx->qconn);
  CURL_TRC_CF(معلومات, cf, "egress, collect and send packets, quantum=%zu",
              send_وقتي_حصو);
  لكل(;;) {
    /* add the next packet to send, if any, to our buffer */
    نتيجو = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_سائيز,
                            read_pkt_منزل_send, pktx, &nread);
    إذا(نتيجو == CURLE_AGAIN)
      اكسر;
    وإلا إذا(نتيجو)
      أرجع نتيجو;
    وإلا {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      إذا((buflen >= send_وقتي_حصو) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_سائيز))
        اكسر;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      إذا(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      وإلا إذا(nread > gsolen ||
              (gsolen > رستو_max_payload_سائيز && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        نتيجو = Curl_vquic_send_پڇ_ورهايو(cf, معلومات, &ctx->q,
                                            gsolen, nread, nread);
        إذا(نتيجو) {
          إذا(نتيجو == CURLE_AGAIN) {
            Curl_expire_سيٽ(معلومات, EXPIRE_QUIC, 1, &pktx->now);
            أرجع CURLE_OK;
          }
          أرجع نتيجو;
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

  إذا(!Curl_bufq_is_خالي_جايون(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(معلومات, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    نتيجو = Curl_vquic_send(cf, معلومات, &ctx->q, gsolen);
    إذا(نتيجو) {
      إذا(نتيجو == CURLE_AGAIN) {
        Curl_expire_سيٽ(معلومات, EXPIRE_QUIC, 1, &pktx->now);
        أرجع CURLE_OK;
      }
      أرجع نتيجو;
    }
    Curl_cf_ngtcp2_io_ctx_update_وقت(معلومات, pktx, cf);
    ngtcp2_conn_update_pkt_tx_وقت(ctx->qconn, pktx->ts);
  }
  أرجع CURLE_OK;
}

بنية cf_ngtcp2_recv_ctx {
  بنية cf_ngtcp2_io_ctx *pktx;
  size_t pkt_ڳڻپ;
};

ساكن CURLcode cf_ngtcp2_recv_pkts(ثابت بنانشان حرف *buf, size_t buflen,
                                    size_t gso_سائيز,
                                    بنية sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    فراغ *userp)
{
  بنية cf_ngtcp2_recv_ctx *rctx = userp;
  بنية cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  بنية cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_رستو رستو;
  size_t وٿي, pktlen;
  صحيح rv;

  إذا(!rctx->pkt_ڳڻپ) {
    Curl_cf_ngtcp2_io_ctx_update_وقت(pktx->معلومات, pktx, pktx->cf);
    ngtcp2_رستو_storage_zero(&pktx->ps);
  }

  إذا(ecn)
    CURL_TRC_CF(pktx->معلومات, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_سائيز, (بنانشان)ecn);
  ngtcp2_addr_init(&رستو.local, (بنية sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&رستو.remote, (بنية sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  لكل(وٿي = 0; وٿي < buflen; وٿي += gso_سائيز) {
    rctx->pkt_ڳڻپ++;
    pktlen = ((وٿي + gso_سائيز) <= buflen) ? gso_سائيز : (buflen - وٿي);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &رستو, &pi,
                              buf + وٿي, pktlen, pktx->ts);
    إذا(rv) {
      CURL_TRC_CF(pktx->معلومات, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_سيٽ(pktx->cf, pktx->معلومات, rv);

      إذا(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        أرجع CURLE_PEER_FAILED_VERIFICATION;
      أرجع CURLE_RECV_ERROR;
    }
  }
  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_ترقي_ingress(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات,
                                         بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_ngtcp2_io_ctx local_pktx;
  بنية cf_ngtcp2_recv_ctx rctx;
  CURLcode نتيجو = CURLE_OK;

  إذا(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, معلومات);
    pktx = &local_pktx;
  }

  نتيجو = Curl_vquic_tls_before_recv(&ctx->tls, cf, معلومات);
  إذا(نتيجو)
    أرجع نتيجو;

  rctx.pktx = pktx;
  rctx.pkt_ڳڻپ = 0;

  إذا(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(معلومات, cf, "progress_ingress(socket)");
    أرجع Curl_vquic_recv_packets(cf, معلومات, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  وإلا {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    بنانشان حرف *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_سائيز;
    size_t pkt_حد_بندي = QUIC_TUNNEL_INGRESS_PKT_حد_بندي;
    size_t nread;
    بنية sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(معلومات, cf, "progress_ingress(sub-filters)");
    إذا(ctx->qconn) {
      size_t max_رستو_payload;
      max_رستو_payload =
        ngtcp2_conn_get_رستو_max_tx_udp_payload_سائيز(ctx->qconn);
      إذا(max_رستو_payload > max_udp_payload)
        max_udp_payload = max_رستو_payload;
    }

    إذا(ctx->tunnel_inbuf_len < max_udp_payload) {
      بنانشان حرف *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      إذا(!newbuf)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    طالما(pkt_حد_بندي--) {
      نتيجو = Curl_conn_cf_recv(cf->ايندڙ, معلومات, (حرف *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      إذا(نتيجو == CURLE_AGAIN) {
        /* no more data available at the moment */
        أرجع CURLE_OK;
      }
      إذا(نتيجو) {
        CURL_TRC_CF(معلومات, cf, "ingress, recv from tunnel failed: %d",
                    (صحيح)نتيجو);
        أرجع نتيجو;
      }
      إذا(nread == 0) {
        /* tunnel closed */
        أرجع CURLE_OK;
      }

      يادگيري_نقل(&remote_addr, ctx->connected_رستو.remote.addr,
             ctx->connected_رستو.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_رستو.remote.addrlen;
      نتيجو = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      إذا(نتيجو)
        أرجع نتيجو;

      إذا(!ctx->q.got_پهريون_byte) {
        ctx->q.got_پهريون_byte = TRUE;
        ctx->q.پهريون_byte_at = ctx->q.آخري_op;
      }
      ctx->q.آخري_io = ctx->q.آخري_op;
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
CURLcode Curl_cf_ngtcp2_cmn_سيٽ_expiry(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  إذا(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, معلومات);
    pktx = &local_pktx;
  }
  وإلا {
    Curl_cf_ngtcp2_io_ctx_update_وقت(معلومات, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  إذا(expiry != UINT64_MAX) {
    إذا(expiry <= pktx->ts) {
      CURLcode نتيجو;
      صحيح rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      إذا(rv) {
        failf(معلومات, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_سيٽ(cf, معلومات, rv);
        أرجع CURLE_SEND_ERROR;
      }
      نتيجو = Curl_cf_ngtcp2_ترقي_ingress(cf, معلومات, pktx);
      إذا(!نتيجو)
        نتيجو = Curl_cf_ngtcp2_ترقي_egress(cf, معلومات, pktx);
      إذا(نتيجو) {
        /* a verify failure during ingress must win over generic errors */
        إذا(ctx->tls_vrfy_نتيجو)
          نتيجو = ctx->tls_vrfy_نتيجو;
        أرجع نتيجو;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    إذا(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      إذا(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_سيٽ(معلومات, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  أرجع CURLE_OK;
}

ساكن فراغ cf_ngtcp2_تياري_keep_alive(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات)
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
    ngtcp2_conn_سيٽ_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(معلومات, cf, "no peer idle timeout, unset keep-alive");
  }
  وإلا إذا(!Curl_u32_ptrset_ڳڻپ(&ctx->streams)) {
    ngtcp2_conn_سيٽ_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(معلومات, cf, "no active streams, unset keep-alive");
  }
  وإلا {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_سيٽ_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(معلومات, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_وهڪرو_تياري(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        بنية h3_وهڪرو_ctx **pstream)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_وهڪرو_ctx *وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);

  *pstream = عدم;
  إذا(!معلومات)
    أرجع CURLE_FAILED_INIT;

  إذا(وهڪرو) {
    *pstream = وهڪرو;
    أرجع CURLE_OK;
  }

  وهڪرو = curlx_calloc(1, حجم(*وهڪرو));
  إذا(!وهڪرو)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  وهڪرو->id = -1;
  وهڪرو->xfer_id = معلومات->id;
  وهڪرو->mid = معلومات->mid;
  وهڪرو->rx_وٿي = 0;
  وهڪرو->rx_وٿي_max = H3_وهڪرو_دري_سائيز_INITIAL;
  وهڪرو->tx_اندريون_flight_ideal = H3_وهڪرو_SEND_BUF_INITIAL;

  /* on send, we control how much we put into the buffer */
  Curl_bufq_initp(&وهڪرو->sendbuf, &ctx->وهڪرو_bufcp,
                  H3_وهڪرو_SEND_CHUNKS, BUFQ_OPT_NONE);
  وهڪرو->tx_اندريون_flight_سائيز = 0;
  Curl_h1_req_parse_init(&وهڪرو->h1, H1_PARSE_DEFAULT_MAX_سٽ_LEN);

  إذا(Curl_u32_ptrset_سيٽ(&ctx->streams, معلومات->mid, وهڪرو)) {
    Curl_cf_ngtcp2_h3_وهڪرو_ctx_free(وهڪرو);
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  إذا(Curl_u32_ptrset_ڳڻپ(&ctx->streams) == 1)
    cf_ngtcp2_تياري_keep_alive(cf, معلومات);

  *pstream = وهڪرو;
  أرجع CURLE_OK;
}

فراغ Curl_cf_ngtcp2_h3_وهڪرو_close(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية h3_وهڪرو_ctx *وهڪرو)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(معلومات);
  DEBUGASSERT(وهڪرو);
  إذا(!وهڪرو->closed && ctx->qconn && ctx->h3conn) {
    CURLcode نتيجو;

    nghttp3_conn_سيٽ_وهڪرو_user_معلومات(ctx->h3conn, وهڪرو->id, عدم);
    ngtcp2_conn_سيٽ_وهڪرو_user_معلومات(ctx->qconn, وهڪرو->id, عدم);
    وهڪرو->closed = TRUE;
    (فراغ)ngtcp2_conn_shutdown_وهڪرو(ctx->qconn, 0, وهڪرو->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    نتيجو = Curl_cf_ngtcp2_ترقي_egress(cf, معلومات, عدم);
    إذا(نتيجو)
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] cancel stream -> %d",
                  وهڪرو->id, (صحيح)نتيجو);
  }
}

فراغ Curl_cf_ngtcp2_h3_وهڪرو_پورو(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_وهڪرو_ctx *وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);
  (فراغ)cf;
  إذا(وهڪرو) {
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] easy handle is done", وهڪرو->id);
    Curl_cf_ngtcp2_h3_وهڪرو_close(cf, معلومات, وهڪرو);
    Curl_u32_ptrset_unset(&ctx->streams, معلومات->mid);
    إذا(!Curl_u32_ptrset_ڳڻپ(&ctx->streams))
      cf_ngtcp2_تياري_keep_alive(cf, معلومات);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      bool *داخلا_pending)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ثابت ngtcp2_transport_params *rp;
  بنية cf_call_معلومات save;

  CF_معلومات_SAVE(save, cf, معلومات);
  *داخلا_pending = FALSE;
  إذا(!ctx->qconn || ctx->shutdown_started)
    اذهب ٻاهريون;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  إذا(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &ctx->q.آخري_io);
    إذا(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      إذا((uint64_t)idletime_ms > max_idle_ms)
        اذهب ٻاهريون;
    }
  }

  إذا(!cf->ايندڙ || !cf->ايندڙ->cft->is_alive(cf->ايندڙ, معلومات, داخلا_pending))
    اذهب ٻاهريون;

  alive = TRUE;
  إذا(*داخلا_pending) {
    CURLcode نتيجو;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *داخلا_pending = FALSE;
    نتيجو = Curl_cf_ngtcp2_ترقي_ingress(cf, معلومات, عدم);
    CURL_TRC_CF(معلومات, cf, "is_alive, progress ingress -> %d", (صحيح)نتيجو);
    alive = نتيجو ? FALSE : TRUE;
  }

ٻاهريون:
  CF_معلومات_RESTORE(cf, save);
  أرجع alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(بنية cf_ngtcp2_ctx *ctx,
                                      بنية Curl_easy *معلومات)
{
  int64_t ctrl_وهڪرو_id, qpack_enc_وهڪرو_id, qpack_dec_وهڪرو_id;
  صحيح rc;

  rc = ngtcp2_conn_open_uni_وهڪرو(ctx->qconn, &ctrl_وهڪرو_id, عدم);
  إذا(rc) {
    failf(معلومات, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_وهڪرو(ctx->h3conn, ctrl_وهڪرو_id);
  إذا(rc) {
    failf(معلومات, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_وهڪرو(ctx->qconn, &qpack_enc_وهڪرو_id, عدم);
  إذا(rc) {
    failf(معلومات, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_وهڪرو(ctx->qconn, &qpack_dec_وهڪرو_id, عدم);
  إذا(rc) {
    failf(معلومات, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_وهڪرو_id,
                                       qpack_dec_وهڪرو_id);
  إذا(rc) {
    failf(معلومات, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_CONNECT_جواب_MS:
    إذا((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_پهريون_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.پهريون_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < وڌ_کان_وڌ_عدد) ? (صحيح)ms : وڌ_کان_وڌ_عدد;
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CF_QUERY_REALLY_CONNECTED:
    إذا(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_پهريون_byte;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->query(cf->ايندڙ, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_REPORT_STATS:
    إذا(cf->connected && !ctx->stats_reported) {
      إذا((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_پهريون_byte) {
        Curl_pgrs_وقت_Was(معلومات, TIMER_CONNECT, ctx->q.پهريون_byte_at);
        ctx->stats_reported = TRUE;
      }
      وإلا إذا(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        إذا(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrs_وقت_Was(معلومات, TIMER_CONNECT, ctx->q.پهريون_byte_at);
        Curl_pgrs_وقت_Was(معلومات, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
