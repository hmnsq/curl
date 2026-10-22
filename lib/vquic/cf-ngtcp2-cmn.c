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

#که_تعریف_شوی USE_OPENSSL
#ضمن <openssl/err.h>
#إذا defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ضمن <ngtcp2/ngtcp2_crypto_boringssl.h>
#وإلاإذا defined(OPENSSL_QUIC_API2)
#ضمن <ngtcp2/ngtcp2_crypto_ossl.h>
#وإلا
#ضمن <ngtcp2/ngtcp2_crypto_quictls.h>
#د_که_پای
#ضمن "vtls/openssl.h"
#وإلاإذا defined(USE_GNUTLS)
#ضمن <ngtcp2/ngtcp2_crypto_gnutls.h>
#ضمن "vtls/gtls.h"
#وإلاإذا defined(USE_WOLFSSL)
#ضمن <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ضمن "vtls/wolfssl.h"
#د_که_پای

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

فراغ Curl_cf_ngtcp2_h3_بهیر_ctx_free(بنية h3_بهیر_ctx *بهیر)
{
  Curl_bufq_free(&بهیر->sendbuf);
  Curl_h1_req_parse_free(&بهیر->h1);
  curlx_free(بهیر);
}

ساكن فراغ h3_بهیر_سېټ_free(uint32_t id, فراغ *بهیر)
{
  (فراغ)id;
  DEBUGASSERT(بهیر);
  Curl_cf_ngtcp2_h3_بهیر_ctx_free((بنية h3_بهیر_ctx *)بهیر);
}

ساكن bool cf_ngtcp2_h3_err_is_fatal(صحيح کوډ)
{
  أرجع (NGHTTP3_ERR_FATAL >= کوډ) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_بهیر == کوډ);
}

فراغ Curl_cf_ngtcp2_h3_err_سېټ(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات, صحيح کوډ)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  إذا(!ctx->وروستی_error.error_کوډ) {
    ngtcp2_ccerr_سېټ_application_error(&ctx->وروستی_error,
      nghttp3_err_infer_quic_app_error_کوډ(کوډ), عدم, 0);
  }
  إذا(cf_ngtcp2_h3_err_is_fatal(کوډ))
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
  ctx->نسخه_2 = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->بهیر_bufcp, H3_بهیر_CHUNK_اندازه,
                  H3_بهیر_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_سرلیک);
  Curl_u32_ptrset_init(&ctx->streams, h3_بهیر_سېټ_free);
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
#که_تعریف_شوی OPENSSL_QUIC_API2
    إذا(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = عدم;
    }
#د_که_پای
    Curl_bufcp_free(&ctx->بهیر_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    curlx_safefree(ctx->tunnel_inbuf);
    ctx->tunnel_inbuf_len = 0;
  }
}

بنية Curl_easy *Curl_cf_ngtcp2_get_xfer(بنية Curl_cfilter *cf,
                                          فراغ *بهیر_user_معلومات,
                                          بنية h3_بهیر_ctx **pstream)
{
  بنية h3_بهیر_ctx *بهیر = بهیر_user_معلومات;

  إذا(pstream)
    *pstream = بهیر;
  إذا(بهیر && (بهیر->mid != UINT32_MAX)) {
    بنية Curl_easy *معلومات, *call_معلومات = CF_معلومات_CURRENT(cf);
    إذا(!call_معلومات || !call_معلومات->multi) {
      DEBUGASSERT(0);
      أرجع عدم;
    }
    معلومات = Curl_multi_get_easy(call_معلومات->multi, بهیر->mid);
    إذا(معلومات && (معلومات->id == بهیر->xfer_id))
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

#که_تعریف_شوی DEBUG_NGTCP2
ساكن فراغ quic_printf(فراغ *user_معلومات, ثابت حرف *fmt, ...)
{
  va_list ap;
  (فراغ)user_معلومات;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#د_که_پای

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
#که_تعریف_شوی DEBUG_NGTCP2
  s->log_printf = quic_printf;
#وإلا
  s->log_printf = عدم;
#د_که_پای

  s->initial_ts = pktx->ts;
  s->handshake_timeout = (معلومات->سېټ.connecttimeout > 0) ?
    معلومات->سېټ.connecttimeout * NGTCP2_MILLISECONDS : QUIC_HANDSHAKE_TIMEOUT;
  s->max_کړکۍ = H3_CONN_کړکۍ_اندازه_MAX;
  s->max_بهیر_کړکۍ = 0; /* disable ngtcp2 auto-tuning of window */
  s->no_pmtud = FALSE;
#که_تعریف_شوی NGTCP2_SETTINGS_V3
  /* try ten times the ngtcp2 defaults here for problems with Caddy */
  s->glitch_ratelim_موده = 1000 * 10;
  s->glitch_ratelim_شرح = 33 * 10;
#د_که_پای
  t->initial_max_معلومات = s->max_کړکۍ;
  t->initial_max_بهیر_معلومات_bidi_local = H3_بهیر_کړکۍ_اندازه_INITIAL;
  t->initial_max_بهیر_معلومات_bidi_remote = H3_بهیر_کړکۍ_اندازه_INITIAL;
  t->initial_max_بهیر_معلومات_uni = t->initial_max_معلومات;
  t->initial_max_streams_bidi = QUIC_MAX_STREAMS;
  t->initial_max_streams_uni = QUIC_MAX_STREAMS;
  t->max_idle_timeout = 0; /* no idle timeout from our side */
  إذا(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#إذا defined(_MSC_VER) && defined(_DLL)
#لارښوونه warning(ټېل)
#لارښوونه warning(disable:4232) /* MSVC extension, dllimport identity */
#د_که_پای

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

  ctx->tls_vrfy_پایله = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    معلومات, &ctx->ssl_peer);
  إذا(ctx->tls_vrfy_پایله)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

#که_تعریف_شوی CURLVERBOSE
  إذا(Curl_trc_is_verbose(معلومات)) {
    ثابت ngtcp2_transport_params *rp;
    rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
    CURL_TRC_CF(معلومات, cf, "handshake complete after %" FMT_TIMEDIFF_T
                "ms, remote transport[max_udp_payload=%" PRIu64
                ", initial_max_data=%" PRIu64 "]",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at),
                rp->max_udp_payload_اندازه, rp->initial_max_معلومات);
  }
#د_که_پای

  /* In case of earlydata, where we simulate being connected, update
   * the handshake time when we really did connect */
  إذا(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrs_وخت_Was(معلومات, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  إذا(ctx->use_earlydata) {
#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    إذا(SSL_get_early_معلومات_حالت(ctx->tls.ossl.ssl) == SSL_EARLY_معلومات_NOT_لېږل_شوی)
      CURL_TRC_CF(معلومات, cf, "OpenSSL did not send early data");
#د_که_پای

#إذا NGTCP2_نسخه_عدد >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_معلومات_rejected2(ctx->qconn);
#وإلا /* older NGTCP2 */
    {
#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      صحيح ossl_early_حالت = SSL_get_early_معلومات_حالت(ctx->tls.ossl.ssl);
      إذا(ossl_early_حالت == SSL_EARLY_معلومات_NOT_لېږل_شوی)
        CURL_TRC_CF(معلومات, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_حالت == SSL_EARLY_معلومات_ACCEPTED);
#وإلاإذا defined(USE_GNUTLS)
      صحيح flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_معلومات);
#وإلاإذا defined(USE_WOLFSSL)
#که_تعریف_شوی WOLFSSL_EARLY_معلومات
      ctx->earlydata_accepted =
        (wolf_SSL_get_early_معلومات_حالت(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_معلومات_REJECTED);
#وإلا
      DEBUGASSERT(0); /* should not come here if ED is disabled. */
      ctx->earlydata_accepted = FALSE;
#د_که_پای /* WOLFSSL_EARLY_DATA */
#د_که_پای /* OPENSSL or GNUTLS or WOLFSSL */
    }
#د_که_پای /* older NGTCP2 */
    CURL_TRC_CF(معلومات, cf, "server did%s accept %zu bytes of early data",
                ctx->earlydata_accepted ? "" : " not", ctx->earlydata_skip);
    Curl_pgrs_Early_معلومات(معلومات, ctx->earlydata_accepted ?
                              (curl_off_t)ctx->earlydata_skip :
                             -(curl_off_t)ctx->earlydata_skip);
  }
  أرجع 0;
}

ساكن صحيح cb_recv_بهیر_معلومات(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t بهیر_id, uint64_t دننه_واټن,
                               ثابت uint8_t *buf, size_t buflen,
                               فراغ *user_معلومات, فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  صحيح fin = (flags & NGTCP2_بهیر_معلومات_FLAG_FIN) ? 1 : 0;
  بنية h3_بهیر_ctx *بهیر;
  بنية Curl_easy *معلومات =
    Curl_cf_ngtcp2_get_xfer(cf, بهیر_user_معلومات, &بهیر);

  (فراغ)دننه_واټن;
  rc = nghttp3_conn_read_بهیر(ctx->h3conn, بهیر_id, buf, buflen, fin);
  إذا(rc < 0) {
    إذا(معلومات && بهیر) {
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  بهیر_id, بهیر->reset, بهیر->closed);
    }
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  إذا(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_بهیر_دننه_واټن(tconn, بهیر_id, nconsumed);
    ngtcp2_conn_extend_max_دننه_واټن(tconn, nconsumed);
    إذا(بهیر) {
      بهیر->rx_دننه_واټن += nconsumed;
      بهیر->rx_دننه_واټن_max += nconsumed;
    }
  }
  أرجع 0;
}

ساكن صحيح cb_acked_بهیر_معلومات_دننه_واټن(ngtcp2_conn *tconn, int64_t بهیر_id,
                                       uint64_t دننه_واټن, uint64_t datalen,
                                       فراغ *user_معلومات, فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)بهیر_id;
  (فراغ)tconn;
  (فراغ)دننه_واټن;
  (فراغ)datalen;
  (فراغ)بهیر_user_معلومات;

  rv = nghttp3_conn_جمع_کړه_ack_دننه_واټن(ctx->h3conn, بهیر_id, datalen);
  إذا(rv && rv != NGHTTP3_ERR_بهیر_NOT_وموندل) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_بهیر_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t بهیر_id, uint64_t app_error_کوډ,
                           فراغ *user_معلومات, فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = Curl_cf_ngtcp2_get_xfer(cf, بهیر_user_معلومات, عدم);
  صحيح rv;

  (فراغ)tconn;
  /* stream is closed... */
  إذا(!معلومات)
    معلومات = CF_معلومات_CURRENT(cf);
  إذا(!معلومات)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  إذا(!(flags & NGTCP2_بهیر_CLOSE_FLAG_APP_ERROR_کوډ_سېټ)) {
    app_error_کوډ = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_بهیر(ctx->h3conn, بهیر_id, app_error_کوډ);
  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", بهیر_id, app_error_کوډ, rv);
  إذا(rv && rv != NGHTTP3_ERR_بهیر_NOT_وموندل) {
    Curl_cf_ngtcp2_h3_err_سېټ(cf, معلومات, rv);
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

#که_تعریف_شوی NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
ساكن صحيح cb_بهیر_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t بهیر_id,
                           uint64_t rx_app_error_کوډ,
                           uint64_t tx_app_error_کوډ,
                           فراغ *user_معلومات, فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = Curl_cf_ngtcp2_get_xfer(cf, بهیر_user_معلومات, عدم);
  uint64_t h3_app_error_کوډ = NGHTTP3_H3_NO_ERROR;
  صحيح rv;

  (فراغ)tconn;
  (فراغ)tx_app_error_کوډ;
  /* stream is closed... */
  إذا(!معلومات)
    معلومات = CF_معلومات_CURRENT(cf);
  إذا(!معلومات)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  إذا(flags & NGTCP2_بهیر_CLOSE2_FLAG_RX_APP_ERROR_کوډ_سېټ)
    h3_app_error_کوډ = rx_app_error_کوډ;

  rv = nghttp3_conn_close_بهیر(ctx->h3conn, بهیر_id, h3_app_error_کوډ);
  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", بهیر_id, h3_app_error_کوډ, rv);
  إذا(rv && rv != NGHTTP3_ERR_بهیر_NOT_وموندل) {
    Curl_cf_ngtcp2_h3_err_سېټ(cf, معلومات, rv);
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}
#د_که_پای

ساكن صحيح cb_بهیر_reset(ngtcp2_conn *tconn, int64_t بهیر_id,
                           uint64_t final_اندازه, uint64_t app_error_کوډ,
                           فراغ *user_معلومات, فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = Curl_cf_ngtcp2_get_xfer(cf, بهیر_user_معلومات, عدم);
  صحيح rv;
  (فراغ)tconn;
  (فراغ)final_اندازه;
  (فراغ)app_error_کوډ;

  rv = nghttp3_conn_shutdown_بهیر_read(ctx->h3conn, بهیر_id);
  إذا(معلومات)
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] reset -> %d", بهیر_id, rv);
  إذا(rv && rv != NGHTTP3_ERR_بهیر_NOT_وموندل) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_بهیر_stop_sending(ngtcp2_conn *tconn, int64_t بهیر_id,
                                  uint64_t app_error_کوډ, فراغ *user_معلومات,
                                  فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)tconn;
  (فراغ)app_error_کوډ;
  (فراغ)بهیر_user_معلومات;

  rv = nghttp3_conn_shutdown_بهیر_read(ctx->h3conn, بهیر_id);
  إذا(rv && rv != NGHTTP3_ERR_بهیر_NOT_وموندل) {
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
                ctx->max_bidi_streams, ctx->کارول_شوی_bidi_streams);
  أرجع 0;
}

ساكن صحيح cb_extend_max_بهیر_معلومات(ngtcp2_conn *tconn, int64_t بهیر_id,
                                     uint64_t max_معلومات, فراغ *user_معلومات,
                                     فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_بهیر_ctx *بهیر;
  بنية Curl_easy *معلومات =
    Curl_cf_ngtcp2_get_xfer(cf, بهیر_user_معلومات, &بهیر);
  صحيح rv;
  (فراغ)tconn;
  (فراغ)max_معلومات;

  rv = nghttp3_conn_unblock_بهیر(ctx->h3conn, بهیر_id);
  إذا(rv && rv != NGHTTP3_ERR_بهیر_NOT_وموندل) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  إذا(بهیر && بهیر->quic_flow_blocked) {
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] unblock quic flow", بهیر_id);
    بهیر->quic_flow_blocked = FALSE;
    إذا(معلومات)
      Curl_multi_mark_dirty(معلومات);
  }
  أرجع 0;
}

ساكن فراغ cb_rand(uint8_t *dest, size_t destlen,
                    ثابت ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode پایله;
  (فراغ)rand_ctx;

  پایله = Curl_rand(عدم, dest, destlen);
  إذا(پایله) {
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
  CURLcode پایله;
  (فراغ)tconn;
  (فراغ)user_معلومات;

  پایله = Curl_rand(عدم, cid->معلومات, cidlen);
  إذا(پایله)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  پایله = Curl_rand(عدم, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  إذا(پایله)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  أرجع 0;
}

#که_تعریف_شوی NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
ساكن صحيح cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  بنية ngtcp2_stateless_reset_token *token, size_t cidlen, فراغ *user_معلومات)
{
  CURLcode پایله;
  (فراغ)tconn;
  (فراغ)user_معلومات;

  پایله = Curl_rand(عدم, cid->معلومات, cidlen);
  إذا(پایله)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  پایله = Curl_rand(عدم, token->معلومات, حجم(token->معلومات));
  إذا(پایله)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  أرجع 0;
}
#د_که_پای

ساكن صحيح cb_recv_rx_کلی(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
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

#که_تعریف_شوی CURL_HAVE_DIAG
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Wmissing-field-initializers"
#د_که_پای
ساكن ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  عدم, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_معلومات_cb,
  cb_ngtcp2_handshake_completed,
  عدم, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_ماسک_cb,
  cb_recv_بهیر_معلومات,
  cb_acked_بهیر_معلومات_دننه_واټن,
  عدم, /* stream_open */
  cb_بهیر_close,
  عدم, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  عدم, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  عدم, /* remove_connection_id */
  ngtcp2_crypto_update_کلی_cb, /* update_key */
  عدم, /* path_validation */
  عدم, /* select_preferred_addr */
  cb_بهیر_reset,
  عدم, /* extend_max_remote_streams_bidi */
  عدم, /* extend_max_remote_streams_uni */
  cb_extend_max_بهیر_معلومات,
  عدم, /* dcid_status */
  عدم, /* handshake_confirmed */
  عدم, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  عدم, /* recv_datagram */
  عدم, /* ack_datagram */
  عدم, /* lost_datagram */
  ngtcp2_crypto_get_لار_challenge_معلومات_cb,
  cb_بهیر_stop_sending,
  عدم, /* version_negotiation */
  cb_recv_rx_کلی,
  عدم, /* recv_tx_key */
  عدم, /* early_data_rejected */
#که_تعریف_شوی NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  عدم, /* begin_path_validation */
#د_که_پای
#که_تعریف_شوی NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  عدم, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  عدم, /* dcid_status2 */
  ngtcp2_crypto_get_لار_challenge_data2_cb, /* get_path_challenge_data2 */
#د_که_پای
#که_تعریف_شوی NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  عدم, /* recv_stop_sending */
#د_که_پای
#که_تعریف_شوی NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_بهیر_close2, /* is called instead of cb_stream_close when set */
#د_که_پای
};
#که_تعریف_شوی CURL_HAVE_DIAG
#لارښوونه GCC diagnostic راوباسه
#د_که_پای

#إذا defined(_MSC_VER) && defined(_DLL)
#لارښوونه warning(راوباسه)
#د_که_پای

#که_تعریف_شوی USE_OPENSSL
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
    بینښه حرف *quic_tp = عدم;
    size_t quic_tp_len = 0;
#که_تعریف_شوی HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     حجم(tpbuf));
    إذا(tplen < 0)
      CURL_TRC_CF(معلومات, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((صحيح)tplen));
    وإلا {
      quic_tp = (بینښه حرف *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#د_که_پای
    Curl_ossl_جمع_کړه_session(cf, معلومات, &ctx->tls.ossl, ctx->ssl_peer.scache_کلی,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, عدم);
  }
  أرجع 0;
}
#د_که_پای /* USE_OPENSSL */

#که_تعریف_شوی USE_GNUTLS

#که_تعریف_شوی CURLVERBOSE
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
#د_که_پای

ساكن صحيح quic_gtls_handshake_cb(gnutls_session_t session, بینښه صحيح htype,
                                  بینښه when, بینښه صحيح incoming,
                                  ثابت gnutls_datum_t *پیغام)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  بنية Curl_cfilter *cf = conn_ref ? conn_ref->user_معلومات : عدم;
  بنية cf_ngtcp2_ctx *ctx = cf ? cf->ctx : عدم;

  (فراغ)پیغام;
  (فراغ)incoming;
  إذا(when && cf && ctx) { /* after message has been processed */
    بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
    DEBUGASSERT(معلومات);
    إذا(!معلومات)
      أرجع 0;
    CURL_TRC_CF(معلومات, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_پیغام_name(htype), htype);
    بدل(htype) {
    حالة GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      بینښه حرف *quic_tp = عدم;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       حجم(tpbuf));
      إذا(tplen < 0)
        CURL_TRC_CF(معلومات, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحيح)tplen));
      وإلا {
        quic_tp = (بینښه حرف *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (فراغ)Curl_gtls_cache_session(cf, معلومات, ctx->ssl_peer.scache_کلی,
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
#د_که_پای /* USE_GNUTLS */

#که_تعریف_شوی USE_WOLFSSL
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
      بینښه حرف *quic_tp = عدم;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       حجم(tpbuf));
      إذا(tplen < 0)
        CURL_TRC_CF(معلومات, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحيح)tplen));
      وإلا {
        quic_tp = (بینښه حرف *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (فراغ)Curl_wssl_cache_session(cf, معلومات, ctx->ssl_peer.scache_کلی,
                                    session, wolf_SSL_نسخه(ssl),
                                    "h3", quic_tp, quic_tp_len, عدم);
    }
  }
  أرجع 0;
}
#د_که_پای /* USE_WOLFSSL */

ساكن CURLcode cf_ngtcp2_tls_ctx_تیاری(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        فراغ *user_معلومات)
{
  بنية curl_tls_ctx *ctx = user_معلومات;

#که_تعریف_شوی USE_OPENSSL
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
#د_که_پای /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  إذا(Curl_ssl_scache_use(cf, معلومات)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_سېټ_session_cache_ډېرپېښ(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_دننی);
    SSL_CTX_sess_سېټ_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#وإلاإذا defined(USE_GNUTLS)
  إذا(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(معلومات, "ngtcp2_crypto_gnutls_configure_client_session failed");
    أرجع CURLE_FAILED_INIT;
  }
  إذا(Curl_ssl_scache_use(cf, معلومات)) {
    gnutls_handshake_سېټ_hook_function(ctx->gtls.session,
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
    wolf_SSL_CTX_sess_سېټ_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#د_که_پای
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ngtcp2_on_session_reuse(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *معلومات,
                                           بنية alpn_spec *alpns,
                                           بنية Curl_ssl_session *scs,
                                           bool *do_early_معلومات)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;

  *do_early_معلومات = FALSE;
#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#د_که_پای
#که_تعریف_شوی USE_GNUTLS
  ctx->earlydata_max =
    gnutls_ریکارډ_get_max_early_معلومات_اندازه(ctx->tls.gtls.session);
#د_که_پای
#که_تعریف_شوی USE_WOLFSSL
#که_تعریف_شوی WOLFSSL_EARLY_معلومات
  ctx->earlydata_max = scs->earlydata_max;
#وإلا
  ctx->earlydata_max = 0;
#د_که_پای /* WOLFSSL_EARLY_DATA */
#د_که_پای
#إذا defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  إذا(!ctx->earlydata_max) {
    CURL_TRC_CF(معلومات, cf, "SSL session does not allow earlydata");
  }
  وإلا إذا(!Curl_alpn_لري_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(معلومات, cf, "SSL session from different ALPN, no early data");
  }
  وإلا إذا(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(معلومات, cf, "no 0RTT transport parameters, no early data");
  }
  وإلا {
    صحيح rv;
    rv = ngtcp2_conn_decode_and_سېټ_0rtt_transport_params(
      ctx->qconn, (ثابت uint8_t *)scs->quic_tp, scs->quic_tp_len);
    إذا(rv)
      CURL_TRC_CF(معلومات, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    وإلا إذا(ctx->init_h3_conn_cb) {
      infof(معلومات, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      پایله = ctx->init_h3_conn_cb(cf, معلومات, ctx);
      إذا(!پایله) {
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
#د_که_پای
  أرجع پایله;
}

/*
 * Might be called twice for happy eyeballs.
 */
ساكن CURLcode cf_connect_پیل(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rc;
  صحيح rv;
  CURLcode پایله;
  ثابت بنية Curl_sockaddr_ex *sockaddr = عدم;
  صحيح qfd;
  ساكن ثابت بنية alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  پایله = Curl_rand(معلومات, ctx->dcid.معلومات, NGTCP2_MAX_CIDLEN);
  إذا(پایله)
    أرجع پایله;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  پایله = Curl_rand(معلومات, ctx->scid.معلومات, NGTCP2_MAX_CIDLEN);
  إذا(پایله)
    أرجع پایله;

  (فراغ)Curl_qlogdir(معلومات, ctx->scid.معلومات, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, معلومات, pktx);

  پایله = Curl_vquic_ctx_init(معلومات, &ctx->q);
  إذا(پایله)
    أرجع پایله;

  /* Query socket and remote address from sub-chain */
  إذا(Curl_cf_socket_peek(cf->بل, معلومات, &ctx->q.sockfd, &sockaddr, عدم)) {
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

    ngtcp2_addr_init(&ctx->connected_لار.local,
                     (بنية sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_لار.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  وإلا {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ثابت بنية Curl_sockaddr_ex *remote = عدم;
    إذا(cf->بل->cft->query(cf->بل, معلومات, CF_QUERY_REMOTE_ADDR, عدم,
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
#که_تعریف_شوی USE_IPV6
    حالة AF_INET6:
      ((بنية sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = حجم(بنية sockaddr_in6);
      اكسر;
#د_که_پای
    افتراضي:
      أرجع CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_لار.local,
                     (بنية sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_لار.remote,
                     &remote->curl_sa_addr,
                     (socklen_t)remote->addrlen);
  }

  rc = ngtcp2_conn_client_new(&ctx->qconn, &ctx->dcid, &ctx->scid,
                              &ctx->connected_لار,
                              NGTCP2_PROTO_VER_V1, &ng_callbacks,
                              &ctx->settings, &ctx->transport_params,
                              Curl_ngtcp2_mem(), cf);
  إذا(rc)
    أرجع CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_معلومات = cf;

  پایله = Curl_vquic_tls_init(&ctx->tls, cf, معلومات,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_تیاری, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  إذا(پایله)
    أرجع پایله;

#إذا defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  إذا(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(معلومات, "ngtcp2_crypto_ossl_ctx_new failed");
    أرجع CURLE_FAILED_INIT;
  }
  ngtcp2_conn_سېټ_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  إذا(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(معلومات, "ngtcp2_crypto_ossl_configure_client_session failed");
    أرجع CURLE_FAILED_INIT;
  }
#وإلاإذا defined(USE_OPENSSL)
  SSL_سېټ_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_سېټ_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#وإلاإذا defined(USE_GNUTLS)
  ngtcp2_conn_سېټ_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#وإلاإذا defined(USE_WOLFSSL)
  ngtcp2_conn_سېټ_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#وإلا
#تېروتنه "ngtcp2 TLS backend not defined"
#د_که_پای

#إذا defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  إذا(ctx->use_earlydata &&
     !SSL_سېټ_quic_tls_early_معلومات_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(معلومات, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#د_که_پای
  ngtcp2_ccerr_default(&ctx->وروستی_error);

  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    bool *بشپړ)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;
  بنية cf_call_معلومات save;
  بنية cf_ngtcp2_io_ctx pktx;

  إذا(cf->connected) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the sub-chain */
  إذا(cf->بل && !cf->بل->connected) {
    پایله = Curl_conn_cf_connect(cf->بل, معلومات, بشپړ);
    إذا(پایله || !*بشپړ)
      أرجع پایله;
  }

  *بشپړ = FALSE;

  إذا(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(معلومات, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(معلومات, cf, "need HTTPS-RR, delaying connect");
    أرجع CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, معلومات);
  CF_معلومات_SAVE(save, cf, معلومات);

  إذا(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(معلومات);
    پایله = cf_connect_پیل(cf, معلومات, &pktx);
    إذا(پایله)
      اذهب بهر;
    إذا(cf->connected) {
      *بشپړ = TRUE;
      اذهب بهر;
    }
    پایله = Curl_cf_ngtcp2_پرمختګ_egress(cf, معلومات, &pktx);
    /* we do not expect to be able to recv anything yet */
    اذهب بهر;
  }

  پایله = Curl_cf_ngtcp2_پرمختګ_ingress(cf, معلومات, &pktx);
  إذا(پایله)
    اذهب بهر;

  پایله = Curl_cf_ngtcp2_پرمختګ_egress(cf, معلومات, &pktx);
  إذا(پایله)
    اذهب بهر;

  إذا(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    پایله = ctx->tls_vrfy_پایله;
    إذا(!پایله) {
      CURL_TRC_CF(معلومات, cf, "peer verified");
      cf->connected = TRUE;
      *بشپړ = TRUE;
    }
  }

بهر:
  إذا(ctx->tls_vrfy_پایله)
    پایله = ctx->tls_vrfy_پایله;
  إذا(ctx->qconn &&
     ((پایله == CURLE_RECV_ERROR) || (پایله == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_دننه_draining_period(ctx->qconn)) {
    ثابت ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    پایله = CURLE_COULDNT_CONNECT;
    إذا(cerr) {
      CURL_TRC_CF(معلومات, cf, "connect error, type=%d, code=%" PRIu64,
                  (صحيح)cerr->type, cerr->error_کوډ);
      بدل(cerr->type) {
      حالة NGTCP2_CCERR_TYPE_نسخه_NEGOTIATION:
        CURL_TRC_CF(معلومات, cf, "error in version negotiation");
        اكسر;
      افتراضي:
        إذا(cerr->error_کوډ >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(معلومات, cf, "crypto error, tls alert=%u",
                      (بینښه صحيح)(cerr->error_کوډ & 0xffU));
        }
        وإلا إذا(cerr->error_کوډ == NGTCP2_CONNECTION_REFUSED) {
          CURL_TRC_CF(معلومات, cf, "connection refused by server");
          /* When a QUIC server instance is shutting down, it may send us a
           * CONNECTION_CLOSE with this code right away. We want
           * to keep on trying in this case. */
          پایله = CURLE_WEIRD_SERVER_ځواب;
        }
      }
    }
  }

#که_تعریف_شوی CURLVERBOSE
  إذا(پایله) {
    إذا(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      بنية ip_quadruple ip;

      إذا(!Curl_cf_socket_peek(cf->بل, معلومات, عدم, عدم, &ip))
        infof(معلومات, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_پورټ, curl_easy_strerror(پایله));
    }
  }
#د_که_پای
  إذا(!پایله && ctx->qconn) {
    پایله = Curl_cf_ngtcp2_cmn_سېټ_expiry(cf, معلومات, &pktx);
  }
  إذا(پایله || *بشپړ)
    CURL_TRC_CF(معلومات, cf, "connect -> %d, done=%d", (صحيح)پایله, *بشپړ);
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات, bool *بشپړ)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode پایله = CURLE_OK;

  إذا(cf->shutdown || !ctx->qconn) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  إذا(!cf->بل) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  CF_معلومات_SAVE(save, cf, معلومات);
  *بشپړ = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, معلومات);

  إذا(!ctx->shutdown_started) {
    حرف بفر[NGTCP2_MAX_UDP_PAYLOAD_اندازه];
    ngtcp2_ssize nwritten;

    إذا(!Curl_bufq_is_تشه(&ctx->q.sendbuf)) {
      CURL_TRC_CF(معلومات, cf, "shutdown, flushing sendbuf");
      پایله = Curl_cf_ngtcp2_پرمختګ_egress(cf, معلومات, &pktx);
      إذا(!Curl_bufq_is_تشه(&ctx->q.sendbuf)) {
        CURL_TRC_CF(معلومات, cf, "sending shutdown packets blocked");
        پایله = CURLE_OK;
        اذهب بهر;
      }
      وإلا إذا(پایله) {
        CURL_TRC_CF(معلومات, cf, "shutdown, error %d flushing sendbuf",
                    (صحيح)پایله);
        *بشپړ = TRUE;
        اذهب بهر;
      }
    }

    DEBUGASSERT(Curl_bufq_is_تشه(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, عدم, /*لار*/
      عدم, /* pkt_info */
      (uint8_t *)بفر, حجم(بفر),
      &ctx->وروستی_error, pktx.ts);
    CURL_TRC_CF(معلومات, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (صحيح)ctx->وروستی_error.type,
                ctx->وروستی_error.error_کوډ, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    إذا(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      پایله = Curl_bufq_write(&ctx->q.sendbuf, (ثابت بینښه حرف *)بفر,
                               (size_t)nwritten, &n);
      إذا(پایله) {
        CURL_TRC_CF(معلومات, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (صحيح)پایله);
        اذهب بهر;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.وېشل_len = 0;
    }
  }

  إذا(!Curl_bufq_is_تشه(&ctx->q.sendbuf)) {
    CURL_TRC_CF(معلومات, cf, "shutdown, flushing egress");
    پایله = Curl_vquic_flush(cf, معلومات, &ctx->q);
    إذا(پایله == CURLE_AGAIN) {
      CURL_TRC_CF(معلومات, cf, "sending shutdown packets blocked");
      پایله = CURLE_OK;
      اذهب بهر;
    }
    وإلا إذا(پایله) {
      CURL_TRC_CF(معلومات, cf, "shutdown, error %d flushing sendbuf",
                  (صحيح)پایله);
      *بشپړ = TRUE;
      اذهب بهر;
    }
  }

  إذا(Curl_bufq_is_تشه(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(معلومات, cf, "shutdown completely sent off, done");
    *بشپړ = TRUE;
    پایله = CURLE_OK;
  }
بهر:
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

فراغ Curl_cf_ngtcp2_cmn_conn_close(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  bool بشپړ;
  Curl_cf_ngtcp2_cmn_shutdown(cf, معلومات, &بشپړ);
}

ساكن bool cf_ngtcp2_err_is_fatal(صحيح کوډ)
{
  أرجع (NGTCP2_ERR_FATAL >= کوډ) ||
         (NGTCP2_ERR_DROP_CONN == کوډ) ||
         (NGTCP2_ERR_IDLE_CLOSE == کوډ);
}

فراغ Curl_cf_ngtcp2_cmn_err_سېټ(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات, صحيح کوډ)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  إذا(!ctx->وروستی_error.error_کوډ) {
    إذا(NGTCP2_ERR_CRYPTO == کوډ) {
      ngtcp2_ccerr_سېټ_tls_alert(&ctx->وروستی_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 عدم, 0);
    }
    وإلا {
      ngtcp2_ccerr_سېټ_liberr(&ctx->وروستی_error, کوډ, عدم, 0);
    }
  }
  إذا(cf_ngtcp2_err_is_fatal(کوډ))
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
  ngtcp2_لار_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_سېټ_وخت(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

فراغ Curl_cf_ngtcp2_io_ctx_update_وخت(بنية Curl_easy *معلومات,
                                       بنية cf_ngtcp2_io_ctx *io_ctx,
                                       بنية Curl_cfilter *cf)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(معلومات);
  Curl_vquic_ctx_update_وخت(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#إذا NGTCP2_نسخه_عدد < 0x011100
بنية cf_ngtcp2_sfind_ctx {
  int64_t بهیر_id;
  بنية h3_بهیر_ctx *بهیر;
  uint32_t mid;
};

ساكن bool cf_ngtcp2_sfind(uint32_t mid, فراغ *ارزښت, فراغ *user_معلومات)
{
  بنية cf_ngtcp2_sfind_ctx *fctx = user_معلومات;
  بنية h3_بهیر_ctx *بهیر = ارزښت;

  إذا(fctx->بهیر_id == بهیر->id) {
    fctx->mid = mid;
    fctx->بهیر = بهیر;
    أرجع FALSE;
  }
  أرجع TRUE; /* continue */
}

ساكن بنية h3_بهیر_ctx *cf_ngtcp2_get_بهیر(بنية cf_ngtcp2_ctx *ctx,
                                                  int64_t بهیر_id)
{
  بنية cf_ngtcp2_sfind_ctx fctx;
  fctx.بهیر_id = بهیر_id;
  fctx.بهیر = عدم;
  Curl_u32_ptrset_لیدنه(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  أرجع fctx.بهیر;
}
#وإلا
ساكن بنية h3_بهیر_ctx *cf_ngtcp2_get_بهیر(بنية cf_ngtcp2_ctx *ctx,
                                                  int64_t بهیر_id)
{
  أرجع ngtcp2_conn_get_بهیر_user_معلومات(ctx->qconn, بهیر_id);
}
#د_که_پای

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
ساكن CURLcode read_pkt_منزل_send(فراغ *userp,
                                 بینښه حرف *buf, size_t buflen,
                                 size_t *pnread)
{
  بنية cf_ngtcp2_io_ctx *x = userp;
  بنية cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t بهیر_id;
  صحيح fin;
  ssize_t n;

  *pnread = 0;
  veccnt = 0;
  بهیر_id = -1;
  fin = 0;

  /* ngtcp2 may want to put several frames from different streams into
   * this packet. `NGTCP2_WRITE_STREAM_FLAG_MORE` tells it to do so.
   * When `NGTCP2_ERR_WRITE_MORE` is returned, we *need* to make
   * another iteration.
   * When ngtcp2 is happy (because it has no other frame that would fit
   * or it has nothing more to send), it returns the total length
   * of the assembled packet. This may be 0 if there was nothing to send. */
  لكل(;;) {

    إذا(ctx->h3conn && ngtcp2_conn_get_max_معلومات_کیڼ(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_بهیر(ctx->h3conn, &بهیر_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      إذا(veccnt < 0) {
        failf(x->معلومات, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((صحيح)veccnt));
        Curl_cf_ngtcp2_h3_err_سېټ(x->cf, x->معلومات, (صحيح)veccnt);
        أرجع CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_بهیر_FLAG_نور |
            (fin ? NGTCP2_WRITE_بهیر_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_بهیر(ctx->qconn, &x->ps.لار,
                                  عدم, buf, buflen,
                                  &ndatalen, flags, بهیر_id,
                                  (ثابت ngtcp2_vec *)vec, veccnt, x->ts);
    إذا(n == 0) {
      /* nothing to send */
      أرجع CURLE_AGAIN;
    }
    وإلا إذا(n < 0) {
      بدل(n) {
      حالة NGTCP2_ERR_بهیر_معلومات_BLOCKED: {
        بنية h3_بهیر_ctx *بهیر;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_بلاک_بهیر(ctx->h3conn, بهیر_id);
        CURL_TRC_CF(x->معلومات, x->cf, "[%" PRId64 "] block quic flow",
                    بهیر_id);
        بهیر = cf_ngtcp2_get_بهیر(ctx, بهیر_id);
        إذا(بهیر) /* it might be not one of our h3 streams? */
          بهیر->quic_flow_blocked = TRUE;
        n = 0;
        اكسر;
      }
      حالة NGTCP2_ERR_بهیر_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_بهیر_write(ctx->h3conn, بهیر_id);
        n = 0;
        اكسر;
      حالة NGTCP2_ERR_WRITE_نور:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        اكسر;
      افتراضي:
        DEBUGASSERT(ndatalen == -1);
        failf(x->معلومات, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((صحيح)n));
        Curl_cf_ngtcp2_cmn_err_سېټ(x->cf, x->معلومات, (صحيح)n);
        أرجع CURLE_SEND_ERROR;
      }
    }

    إذا(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      صحيح rv = nghttp3_conn_جمع_کړه_write_دننه_واټن(ctx->h3conn, بهیر_id, ndatalen);
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

CURLcode Curl_cf_ngtcp2_پرمختګ_egress(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_اندازه, لار_max_payload_اندازه;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_وخت_ټوټه;
  CURLcode پایله;
  بنية cf_ngtcp2_io_ctx local_pktx;

  إذا(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, معلومات);
    pktx = &local_pktx;
  }
  وإلا {
    Curl_cf_ngtcp2_io_ctx_update_وخت(معلومات, pktx, cf);
    ngtcp2_لار_storage_zero(&pktx->ps);
  }

  پایله = Curl_vquic_flush(cf, معلومات, &ctx->q);
  إذا(پایله) {
    إذا(پایله == CURLE_AGAIN) {
      Curl_expire_سېټ(معلومات, EXPIRE_QUIC, 1, &pktx->now);
      أرجع CURLE_OK;
    }
    أرجع پایله;
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
  لار_max_payload_اندازه =
    ngtcp2_conn_get_لار_max_tx_udp_payload_اندازه(ctx->qconn);
  send_وخت_ټوټه = ngtcp2_conn_get_send_وخت_ټوټه(ctx->qconn);
  CURL_TRC_CF(معلومات, cf, "egress, collect and send packets, quantum=%zu",
              send_وخت_ټوټه);
  لكل(;;) {
    /* add the next packet to send, if any, to our buffer */
    پایله = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_اندازه,
                            read_pkt_منزل_send, pktx, &nread);
    إذا(پایله == CURLE_AGAIN)
      اكسر;
    وإلا إذا(پایله)
      أرجع پایله;
    وإلا {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      إذا((buflen >= send_وخت_ټوټه) ||
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
              (gsolen > لار_max_payload_اندازه && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        پایله = Curl_vquic_send_لکۍ_وېشل(cf, معلومات, &ctx->q,
                                            gsolen, nread, nread);
        إذا(پایله) {
          إذا(پایله == CURLE_AGAIN) {
            Curl_expire_سېټ(معلومات, EXPIRE_QUIC, 1, &pktx->now);
            أرجع CURLE_OK;
          }
          أرجع پایله;
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

  إذا(!Curl_bufq_is_تشه(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(معلومات, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    پایله = Curl_vquic_send(cf, معلومات, &ctx->q, gsolen);
    إذا(پایله) {
      إذا(پایله == CURLE_AGAIN) {
        Curl_expire_سېټ(معلومات, EXPIRE_QUIC, 1, &pktx->now);
        أرجع CURLE_OK;
      }
      أرجع پایله;
    }
    Curl_cf_ngtcp2_io_ctx_update_وخت(معلومات, pktx, cf);
    ngtcp2_conn_update_pkt_tx_وخت(ctx->qconn, pktx->ts);
  }
  أرجع CURLE_OK;
}

بنية cf_ngtcp2_recv_ctx {
  بنية cf_ngtcp2_io_ctx *pktx;
  size_t pkt_شمېر;
};

ساكن CURLcode cf_ngtcp2_recv_pkts(ثابت بینښه حرف *buf, size_t buflen,
                                    size_t gso_اندازه,
                                    بنية sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    فراغ *userp)
{
  بنية cf_ngtcp2_recv_ctx *rctx = userp;
  بنية cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  بنية cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_لار لار;
  size_t دننه_واټن, pktlen;
  صحيح rv;

  إذا(!rctx->pkt_شمېر) {
    Curl_cf_ngtcp2_io_ctx_update_وخت(pktx->معلومات, pktx, pktx->cf);
    ngtcp2_لار_storage_zero(&pktx->ps);
  }

  إذا(ecn)
    CURL_TRC_CF(pktx->معلومات, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_اندازه, (بینښه)ecn);
  ngtcp2_addr_init(&لار.local, (بنية sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&لار.remote, (بنية sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  لكل(دننه_واټن = 0; دننه_واټن < buflen; دننه_واټن += gso_اندازه) {
    rctx->pkt_شمېر++;
    pktlen = ((دننه_واټن + gso_اندازه) <= buflen) ? gso_اندازه : (buflen - دننه_واټن);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &لار, &pi,
                              buf + دننه_واټن, pktlen, pktx->ts);
    إذا(rv) {
      CURL_TRC_CF(pktx->معلومات, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_سېټ(pktx->cf, pktx->معلومات, rv);

      إذا(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        أرجع CURLE_PEER_FAILED_VERIFICATION;
      أرجع CURLE_RECV_ERROR;
    }
  }
  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_پرمختګ_ingress(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات,
                                         بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_ngtcp2_io_ctx local_pktx;
  بنية cf_ngtcp2_recv_ctx rctx;
  CURLcode پایله = CURLE_OK;

  إذا(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, معلومات);
    pktx = &local_pktx;
  }

  پایله = Curl_vquic_tls_before_recv(&ctx->tls, cf, معلومات);
  إذا(پایله)
    أرجع پایله;

  rctx.pktx = pktx;
  rctx.pkt_شمېر = 0;

  إذا(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(معلومات, cf, "progress_ingress(socket)");
    أرجع Curl_vquic_recv_packets(cf, معلومات, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  وإلا {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    بینښه حرف *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_اندازه;
    size_t pkt_حد = QUIC_TUNNEL_INGRESS_PKT_حد;
    size_t nread;
    بنية sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(معلومات, cf, "progress_ingress(sub-filters)");
    إذا(ctx->qconn) {
      size_t max_لار_payload;
      max_لار_payload =
        ngtcp2_conn_get_لار_max_tx_udp_payload_اندازه(ctx->qconn);
      إذا(max_لار_payload > max_udp_payload)
        max_udp_payload = max_لار_payload;
    }

    إذا(ctx->tunnel_inbuf_len < max_udp_payload) {
      بینښه حرف *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      إذا(!newbuf)
        أرجع CURLE_بهر_د_حافظه;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    طالما(pkt_حد--) {
      پایله = Curl_conn_cf_recv(cf->بل, معلومات, (حرف *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      إذا(پایله == CURLE_AGAIN) {
        /* no more data available at the moment */
        أرجع CURLE_OK;
      }
      إذا(پایله) {
        CURL_TRC_CF(معلومات, cf, "ingress, recv from tunnel failed: %d",
                    (صحيح)پایله);
        أرجع پایله;
      }
      إذا(nread == 0) {
        /* tunnel closed */
        أرجع CURLE_OK;
      }

      حافظه_نقل(&remote_addr, ctx->connected_لار.remote.addr,
             ctx->connected_لار.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_لار.remote.addrlen;
      پایله = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      إذا(پایله)
        أرجع پایله;

      إذا(!ctx->q.got_لومړی_byte) {
        ctx->q.got_لومړی_byte = TRUE;
        ctx->q.لومړی_byte_at = ctx->q.وروستی_op;
      }
      ctx->q.وروستی_io = ctx->q.وروستی_op;
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
CURLcode Curl_cf_ngtcp2_cmn_سېټ_expiry(بنية Curl_cfilter *cf,
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
    Curl_cf_ngtcp2_io_ctx_update_وخت(معلومات, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  إذا(expiry != UINT64_MAX) {
    إذا(expiry <= pktx->ts) {
      CURLcode پایله;
      صحيح rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      إذا(rv) {
        failf(معلومات, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_سېټ(cf, معلومات, rv);
        أرجع CURLE_SEND_ERROR;
      }
      پایله = Curl_cf_ngtcp2_پرمختګ_ingress(cf, معلومات, pktx);
      إذا(!پایله)
        پایله = Curl_cf_ngtcp2_پرمختګ_egress(cf, معلومات, pktx);
      إذا(پایله) {
        /* a verify failure during ingress must win over generic errors */
        إذا(ctx->tls_vrfy_پایله)
          پایله = ctx->tls_vrfy_پایله;
        أرجع پایله;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    إذا(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      إذا(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_سېټ(معلومات, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  أرجع CURLE_OK;
}

ساكن فراغ cf_ngtcp2_تیاری_keep_alive(بنية Curl_cfilter *cf,
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
    ngtcp2_conn_سېټ_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(معلومات, cf, "no peer idle timeout, unset keep-alive");
  }
  وإلا إذا(!Curl_u32_ptrset_شمېر(&ctx->streams)) {
    ngtcp2_conn_سېټ_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(معلومات, cf, "no active streams, unset keep-alive");
  }
  وإلا {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_سېټ_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(معلومات, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_بهیر_تیاری(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        بنية h3_بهیر_ctx **pstream)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_بهیر_ctx *بهیر = H3_بهیر_CTX(ctx, معلومات);

  *pstream = عدم;
  إذا(!معلومات)
    أرجع CURLE_FAILED_INIT;

  إذا(بهیر) {
    *pstream = بهیر;
    أرجع CURLE_OK;
  }

  بهیر = curlx_calloc(1, حجم(*بهیر));
  إذا(!بهیر)
    أرجع CURLE_بهر_د_حافظه;

  بهیر->id = -1;
  بهیر->xfer_id = معلومات->id;
  بهیر->mid = معلومات->mid;
  بهیر->rx_دننه_واټن = 0;
  بهیر->rx_دننه_واټن_max = H3_بهیر_کړکۍ_اندازه_INITIAL;
  بهیر->tx_دننه_flight_ideal = H3_بهیر_SEND_BUF_INITIAL;

  /* on send, we control how much we put into the buffer */
  Curl_bufq_initp(&بهیر->sendbuf, &ctx->بهیر_bufcp,
                  H3_بهیر_SEND_CHUNKS, BUFQ_OPT_NONE);
  بهیر->tx_دننه_flight_اندازه = 0;
  Curl_h1_req_parse_init(&بهیر->h1, H1_PARSE_DEFAULT_MAX_کرښه_LEN);

  إذا(Curl_u32_ptrset_سېټ(&ctx->streams, معلومات->mid, بهیر)) {
    Curl_cf_ngtcp2_h3_بهیر_ctx_free(بهیر);
    أرجع CURLE_بهر_د_حافظه;
  }

  إذا(Curl_u32_ptrset_شمېر(&ctx->streams) == 1)
    cf_ngtcp2_تیاری_keep_alive(cf, معلومات);

  *pstream = بهیر;
  أرجع CURLE_OK;
}

فراغ Curl_cf_ngtcp2_h3_بهیر_close(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية h3_بهیر_ctx *بهیر)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(معلومات);
  DEBUGASSERT(بهیر);
  إذا(!بهیر->closed && ctx->qconn && ctx->h3conn) {
    CURLcode پایله;

    nghttp3_conn_سېټ_بهیر_user_معلومات(ctx->h3conn, بهیر->id, عدم);
    ngtcp2_conn_سېټ_بهیر_user_معلومات(ctx->qconn, بهیر->id, عدم);
    بهیر->closed = TRUE;
    (فراغ)ngtcp2_conn_shutdown_بهیر(ctx->qconn, 0, بهیر->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    پایله = Curl_cf_ngtcp2_پرمختګ_egress(cf, معلومات, عدم);
    إذا(پایله)
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] cancel stream -> %d",
                  بهیر->id, (صحيح)پایله);
  }
}

فراغ Curl_cf_ngtcp2_h3_بهیر_بشپړ(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_بهیر_ctx *بهیر = H3_بهیر_CTX(ctx, معلومات);
  (فراغ)cf;
  إذا(بهیر) {
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] easy handle is done", بهیر->id);
    Curl_cf_ngtcp2_h3_بهیر_close(cf, معلومات, بهیر);
    Curl_u32_ptrset_unset(&ctx->streams, معلومات->mid);
    إذا(!Curl_u32_ptrset_شمېر(&ctx->streams))
      cf_ngtcp2_تیاری_keep_alive(cf, معلومات);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      bool *ننوت_pending)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ثابت ngtcp2_transport_params *rp;
  بنية cf_call_معلومات save;

  CF_معلومات_SAVE(save, cf, معلومات);
  *ننوت_pending = FALSE;
  إذا(!ctx->qconn || ctx->shutdown_started)
    اذهب بهر;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  إذا(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &ctx->q.وروستی_io);
    إذا(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      إذا((uint64_t)idletime_ms > max_idle_ms)
        اذهب بهر;
    }
  }

  إذا(!cf->بل || !cf->بل->cft->is_alive(cf->بل, معلومات, ننوت_pending))
    اذهب بهر;

  alive = TRUE;
  إذا(*ننوت_pending) {
    CURLcode پایله;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *ننوت_pending = FALSE;
    پایله = Curl_cf_ngtcp2_پرمختګ_ingress(cf, معلومات, عدم);
    CURL_TRC_CF(معلومات, cf, "is_alive, progress ingress -> %d", (صحيح)پایله);
    alive = پایله ? FALSE : TRUE;
  }

بهر:
  CF_معلومات_RESTORE(cf, save);
  أرجع alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(بنية cf_ngtcp2_ctx *ctx,
                                      بنية Curl_easy *معلومات)
{
  int64_t ctrl_بهیر_id, qpack_enc_بهیر_id, qpack_dec_بهیر_id;
  صحيح rc;

  rc = ngtcp2_conn_open_uni_بهیر(ctx->qconn, &ctrl_بهیر_id, عدم);
  إذا(rc) {
    failf(معلومات, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_بهیر(ctx->h3conn, ctrl_بهیر_id);
  إذا(rc) {
    failf(معلومات, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_بهیر(ctx->qconn, &qpack_enc_بهیر_id, عدم);
  إذا(rc) {
    failf(معلومات, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_بهیر(ctx->qconn, &qpack_dec_بهیر_id, عدم);
  إذا(rc) {
    failf(معلومات, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_بهیر_id,
                                       qpack_dec_بهیر_id);
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
  حالة CF_QUERY_CONNECT_ځواب_MS:
    إذا((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_لومړی_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.لومړی_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < لوی_عدد) ? (صحيح)ms : لوی_عدد;
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CF_QUERY_REALLY_CONNECTED:
    إذا(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_لومړی_byte;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع cf->بل ?
    cf->بل->cft->query(cf->بل, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_REPORT_STATS:
    إذا(cf->connected && !ctx->stats_reported) {
      إذا((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_لومړی_byte) {
        Curl_pgrs_وخت_Was(معلومات, TIMER_CONNECT, ctx->q.لومړی_byte_at);
        ctx->stats_reported = TRUE;
      }
      وإلا إذا(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        إذا(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrs_وخت_Was(معلومات, TIMER_CONNECT, ctx->q.لومړی_byte_at);
        Curl_pgrs_وخت_Was(معلومات, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع پایله;
}

#د_که_پای /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
