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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/err.h>
#ਜੇ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2_crypto_boringssl.h>
#ਜਾਂਜੇ defined(OPENSSL_QUIC_API2)
#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2_crypto_ossl.h>
#ਨਹੀਂਤਾਂ
#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2_crypto_quictls.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/openssl.h"
#ਜਾਂਜੇ defined(USE_GNUTLS)
#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2_crypto_gnutls.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/gtls.h"
#ਜਾਂਜੇ defined(USE_WOLFSSL)
#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/wolfssl.h"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <nghttp3/nghttp3.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "u32_ptrset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-socket.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http1.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sockaddr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/cf-dns.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic-tls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_scache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/cf-ngtcp2-cmn.h"

/*
 * Store ngtcp2 version info in this buffer.
 */
ਖਾਲੀ Curl_ngtcp2_ver(ਅੱਖਰ *p, size_t len)
{
  ਅਚਲ ngtcp2_info *ng2 = ngtcp2_ਸੰਸਕਰਣ(0);
  ਅਚਲ nghttp3_info *ht3 = nghttp3_ਸੰਸਕਰਣ(0);
  (ਖਾਲੀ)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->ਸੰਸਕਰਣ_str, ht3->ਸੰਸਕਰਣ_str);
}

ਖਾਲੀ Curl_cf_ngtcp2_h3_ਧਾਰਾ_ctx_free(ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ)
{
  Curl_bufq_free(&ਧਾਰਾ->sendbuf);
  Curl_h1_req_parse_free(&ਧਾਰਾ->h1);
  curlx_free(ਧਾਰਾ);
}

ਸਥਿਰ ਖਾਲੀ h3_ਧਾਰਾ_ਸੈੱਟ_free(uint32_t id, ਖਾਲੀ *ਧਾਰਾ)
{
  (ਖਾਲੀ)id;
  DEBUGASSERT(ਧਾਰਾ);
  Curl_cf_ngtcp2_h3_ਧਾਰਾ_ctx_free((ਬਣਤਰ h3_ਧਾਰਾ_ctx *)ਧਾਰਾ);
}

ਸਥਿਰ bool cf_ngtcp2_h3_err_is_fatal(ਪੂਰਨ ਕੋਡ)
{
  ਵਾਪਸ (NGHTTP3_ERR_FATAL >= ਕੋਡ) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_ਧਾਰਾ == ਕੋਡ);
}

ਖਾਲੀ Curl_cf_ngtcp2_h3_err_ਸੈੱਟ(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ ਕੋਡ)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਜੇ(!ctx->ਆਖਰੀ_error.error_ਕੋਡ) {
    ngtcp2_ccerr_ਸੈੱਟ_application_error(&ctx->ਆਖਰੀ_error,
      nghttp3_err_infer_quic_app_error_ਕੋਡ(ਕੋਡ), ਨਿਰਾ, 0);
  }
  ਜੇ(cf_ngtcp2_h3_err_is_fatal(ਕੋਡ))
    Curl_cf_ngtcp2_cmn_conn_close(cf, ਅੰਕੜੇ);
}

CURLcode Curl_cf_ngtcp2_ctx_init(ਬਣਤਰ cf_ngtcp2_ctx *ctx,
                                 ਬਣਤਰ Curl_peer *origin,
                                 ਬਣਤਰ Curl_peer *peer,
                                 ਬਣਤਰ ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = ਨਿਰਾ;
  ctx->tunnel_inbuf_len = 0;
  ctx->ਸੰਸਕਰਣ_2 = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->ਧਾਰਾ_bufcp, H3_ਧਾਰਾ_CHUNK_ਮਾਪ,
                  H3_ਧਾਰਾ_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_ਸਿਰਲੇਖ);
  Curl_u32_ptrset_init(&ctx->streams, h3_ਧਾਰਾ_ਸੈੱਟ_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  ਵਾਪਸ Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

ਖਾਲੀ Curl_cf_ngtcp2_ctx_cleanup(ਬਣਤਰ cf_ngtcp2_ctx *ctx)
{
  ਜੇ(ctx && ctx->initialized) {
    ਜੇ(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    ਜੇ(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = ਨਿਰਾ;
    }
    ਜੇ(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = ਨਿਰਾ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSSL_QUIC_API2
    ਜੇ(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = ਨਿਰਾ;
    }
#ਜੇ_ਅੰਤ
    Curl_bufcp_free(&ctx->ਧਾਰਾ_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    curlx_safefree(ctx->tunnel_inbuf);
    ctx->tunnel_inbuf_len = 0;
  }
}

ਬਣਤਰ Curl_easy *Curl_cf_ngtcp2_get_xfer(ਬਣਤਰ Curl_cfilter *cf,
                                          ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ,
                                          ਬਣਤਰ h3_ਧਾਰਾ_ctx **pstream)
{
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = ਧਾਰਾ_user_ਅੰਕੜੇ;

  ਜੇ(pstream)
    *pstream = ਧਾਰਾ;
  ਜੇ(ਧਾਰਾ && (ਧਾਰਾ->mid != UINT32_MAX)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, *call_ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
    ਜੇ(!call_ਅੰਕੜੇ || !call_ਅੰਕੜੇ->multi) {
      DEBUGASSERT(0);
      ਵਾਪਸ ਨਿਰਾ;
    }
    ਅੰਕੜੇ = Curl_multi_get_easy(call_ਅੰਕੜੇ->multi, ਧਾਰਾ->mid);
    ਜੇ(ਅੰਕੜੇ && (ਅੰਕੜੇ->id == ਧਾਰਾ->xfer_id))
      ਵਾਪਸ ਅੰਕੜੇ;
    DEBUGASSERT(0);
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  ਬਣਤਰ Curl_cfilter *cf = conn_ref->user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਵਾਪਸ ctx->qconn;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_NGTCP2
ਸਥਿਰ ਖਾਲੀ quic_printf(ਖਾਲੀ *user_ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  va_list ap;
  (ਖਾਲੀ)user_ਅੰਕੜੇ;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ qlog_callback(ਖਾਲੀ *user_ਅੰਕੜੇ, uint32_t flags,
                          ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t datalen)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  (ਖਾਲੀ)flags;
  ਜੇ(ctx->qlogfd != -1) {
    ssize_t rc = ਲਿਖ(ctx->qlogfd, ਅੰਕੜੇ, datalen);
    ਜੇ(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

ਸਥਿਰ ਖਾਲੀ quic_settings(ਬਣਤਰ cf_ngtcp2_ctx *ctx,
                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_NGTCP2
  s->log_printf = quic_printf;
#ਨਹੀਂਤਾਂ
  s->log_printf = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

  s->initial_ts = pktx->ts;
  s->handshake_timeout = (ਅੰਕੜੇ->ਸੈੱਟ.connecttimeout > 0) ?
    ਅੰਕੜੇ->ਸੈੱਟ.connecttimeout * NGTCP2_MILLISECONDS : QUIC_HANDSHAKE_TIMEOUT;
  s->max_ਖਿੜਕੀ = H3_CONN_ਖਿੜਕੀ_ਮਾਪ_MAX;
  s->max_ਧਾਰਾ_ਖਿੜਕੀ = 0; /* disable ngtcp2 auto-tuning of window */
  s->no_pmtud = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NGTCP2_SETTINGS_V3
  /* try ten times the ngtcp2 defaults here for problems with Caddy */
  s->glitch_ratelim_ਮਿਆਦ = 1000 * 10;
  s->glitch_ratelim_ਦਰ = 33 * 10;
#ਜੇ_ਅੰਤ
  t->initial_max_ਅੰਕੜੇ = s->max_ਖਿੜਕੀ;
  t->initial_max_ਧਾਰਾ_ਅੰਕੜੇ_bidi_local = H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_INITIAL;
  t->initial_max_ਧਾਰਾ_ਅੰਕੜੇ_bidi_remote = H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_INITIAL;
  t->initial_max_ਧਾਰਾ_ਅੰਕੜੇ_uni = t->initial_max_ਅੰਕੜੇ;
  t->initial_max_streams_bidi = QUIC_MAX_STREAMS;
  t->initial_max_streams_uni = QUIC_MAX_STREAMS;
  t->max_idle_timeout = 0; /* no idle timeout from our side */
  ਜੇ(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#ਜੇ defined(_MSC_VER) && defined(_DLL)
#ਹਦਾਇਤ warning(ਧੱਕੋ)
#ਹਦਾਇਤ warning(disable:4232) /* MSVC extension, dllimport identity */
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ਨਿਰਾ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;

  (ਖਾਲੀ)tconn;
  DEBUGASSERT(ctx);
  ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  DEBUGASSERT(ਅੰਕੜੇ);
  ਜੇ(!ctx || !ਅੰਕੜੇ)
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(ਅੰਕੜੇ);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, ਅੰਕੜੇ);

  ctx->tls_vrfy_ਨਤੀਜਾ = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    ਅੰਕੜੇ, &ctx->ssl_peer);
  ਜੇ(ctx->tls_vrfy_ਨਤੀਜਾ)
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਜੇ(Curl_trc_is_verbose(ਅੰਕੜੇ)) {
    ਅਚਲ ngtcp2_transport_params *rp;
    rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "handshake complete after %" FMT_TIMEDIFF_T
                "ms, remote transport[max_udp_payload=%" PRIu64
                ", initial_max_data=%" PRIu64 "]",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at),
                rp->max_udp_payload_ਮਾਪ, rp->initial_max_ਅੰਕੜੇ);
  }
#ਜੇ_ਅੰਤ

  /* In case of earlydata, where we simulate being connected, update
   * the handshake time when we really did connect */
  ਜੇ(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrs_ਸਮਾਂ_Was(ਅੰਕੜੇ, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  ਜੇ(ctx->use_earlydata) {
#ਜੇ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    ਜੇ(SSL_get_early_ਅੰਕੜੇ_ਸਥਿਤੀ(ctx->tls.ossl.ssl) == SSL_EARLY_ਅੰਕੜੇ_NOT_ਭੇਜਿਆ)
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "OpenSSL did not send early data");
#ਜੇ_ਅੰਤ

#ਜੇ NGTCP2_ਸੰਸਕਰਣ_ਅੰਕ >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_ਅੰਕੜੇ_rejected2(ctx->qconn);
#ਨਹੀਂਤਾਂ /* older NGTCP2 */
    {
#ਜੇ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      ਪੂਰਨ ossl_early_ਸਥਿਤੀ = SSL_get_early_ਅੰਕੜੇ_ਸਥਿਤੀ(ctx->tls.ossl.ssl);
      ਜੇ(ossl_early_ਸਥਿਤੀ == SSL_EARLY_ਅੰਕੜੇ_NOT_ਭੇਜਿਆ)
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_ਸਥਿਤੀ == SSL_EARLY_ਅੰਕੜੇ_ACCEPTED);
#ਜਾਂਜੇ defined(USE_GNUTLS)
      ਪੂਰਨ flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_ਅੰਕੜੇ);
#ਜਾਂਜੇ defined(USE_WOLFSSL)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WOLFSSL_EARLY_ਅੰਕੜੇ
      ctx->earlydata_accepted =
        (wolf_SSL_get_early_ਅੰਕੜੇ_ਸਥਿਤੀ(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_ਅੰਕੜੇ_REJECTED);
#ਨਹੀਂਤਾਂ
      DEBUGASSERT(0); /* should not come here if ED is disabled. */
      ctx->earlydata_accepted = FALSE;
#ਜੇ_ਅੰਤ /* WOLFSSL_EARLY_DATA */
#ਜੇ_ਅੰਤ /* OPENSSL or GNUTLS or WOLFSSL */
    }
#ਜੇ_ਅੰਤ /* older NGTCP2 */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "server did%s accept %zu bytes of early data",
                ctx->earlydata_accepted ? "" : " not", ctx->earlydata_skip);
    Curl_pgrs_Early_ਅੰਕੜੇ(ਅੰਕੜੇ, ctx->earlydata_accepted ?
                              (curl_off_t)ctx->earlydata_skip :
                             -(curl_off_t)ctx->earlydata_skip);
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_recv_ਧਾਰਾ_ਅੰਕੜੇ(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t ਧਾਰਾ_id, uint64_t ਵਿੱਥ,
                               ਅਚਲ uint8_t *buf, size_t buflen,
                               ਖਾਲੀ *user_ਅੰਕੜੇ, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  ਪੂਰਨ fin = (flags & NGTCP2_ਧਾਰਾ_ਅੰਕੜੇ_FLAG_FIN) ? 1 : 0;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ =
    Curl_cf_ngtcp2_get_xfer(cf, ਧਾਰਾ_user_ਅੰਕੜੇ, &ਧਾਰਾ);

  (ਖਾਲੀ)ਵਿੱਥ;
  rc = nghttp3_conn_read_ਧਾਰਾ(ctx->h3conn, ਧਾਰਾ_id, buf, buflen, fin);
  ਜੇ(rc < 0) {
    ਜੇ(ਅੰਕੜੇ && ਧਾਰਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  ਧਾਰਾ_id, ਧਾਰਾ->reset, ਧਾਰਾ->closed);
    }
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  ਜੇ(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_ਧਾਰਾ_ਵਿੱਥ(tconn, ਧਾਰਾ_id, nconsumed);
    ngtcp2_conn_extend_max_ਵਿੱਥ(tconn, nconsumed);
    ਜੇ(ਧਾਰਾ) {
      ਧਾਰਾ->rx_ਵਿੱਥ += nconsumed;
      ਧਾਰਾ->rx_ਵਿੱਥ_max += nconsumed;
    }
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_acked_ਧਾਰਾ_ਅੰਕੜੇ_ਵਿੱਥ(ngtcp2_conn *tconn, int64_t ਧਾਰਾ_id,
                                       uint64_t ਵਿੱਥ, uint64_t datalen,
                                       ਖਾਲੀ *user_ਅੰਕੜੇ, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਪੂਰਨ rv;
  (ਖਾਲੀ)ਧਾਰਾ_id;
  (ਖਾਲੀ)tconn;
  (ਖਾਲੀ)ਵਿੱਥ;
  (ਖਾਲੀ)datalen;
  (ਖਾਲੀ)ਧਾਰਾ_user_ਅੰਕੜੇ;

  rv = nghttp3_conn_ਜੋੜੋ_ack_ਵਿੱਥ(ctx->h3conn, ਧਾਰਾ_id, datalen);
  ਜੇ(rv && rv != NGHTTP3_ERR_ਧਾਰਾ_NOT_ਮਿਲਿਆ) {
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_ਧਾਰਾ_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t ਧਾਰਾ_id, uint64_t app_error_ਕੋਡ,
                           ਖਾਲੀ *user_ਅੰਕੜੇ, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_cf_ngtcp2_get_xfer(cf, ਧਾਰਾ_user_ਅੰਕੜੇ, ਨਿਰਾ);
  ਪੂਰਨ rv;

  (ਖਾਲੀ)tconn;
  /* stream is closed... */
  ਜੇ(!ਅੰਕੜੇ)
    ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;

  ਜੇ(!(flags & NGTCP2_ਧਾਰਾ_CLOSE_FLAG_APP_ERROR_ਕੋਡ_ਸੈੱਟ)) {
    app_error_ਕੋਡ = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_ਧਾਰਾ(ctx->h3conn, ਧਾਰਾ_id, app_error_ਕੋਡ);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", ਧਾਰਾ_id, app_error_ਕੋਡ, rv);
  ਜੇ(rv && rv != NGHTTP3_ERR_ਧਾਰਾ_NOT_ਮਿਲਿਆ) {
    Curl_cf_ngtcp2_h3_err_ਸੈੱਟ(cf, ਅੰਕੜੇ, rv);
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ਵਾਪਸ 0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
ਸਥਿਰ ਪੂਰਨ cb_ਧਾਰਾ_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t ਧਾਰਾ_id,
                           uint64_t rx_app_error_ਕੋਡ,
                           uint64_t tx_app_error_ਕੋਡ,
                           ਖਾਲੀ *user_ਅੰਕੜੇ, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_cf_ngtcp2_get_xfer(cf, ਧਾਰਾ_user_ਅੰਕੜੇ, ਨਿਰਾ);
  uint64_t h3_app_error_ਕੋਡ = NGHTTP3_H3_NO_ERROR;
  ਪੂਰਨ rv;

  (ਖਾਲੀ)tconn;
  (ਖਾਲੀ)tx_app_error_ਕੋਡ;
  /* stream is closed... */
  ਜੇ(!ਅੰਕੜੇ)
    ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;

  ਜੇ(flags & NGTCP2_ਧਾਰਾ_CLOSE2_FLAG_RX_APP_ERROR_ਕੋਡ_ਸੈੱਟ)
    h3_app_error_ਕੋਡ = rx_app_error_ਕੋਡ;

  rv = nghttp3_conn_close_ਧਾਰਾ(ctx->h3conn, ਧਾਰਾ_id, h3_app_error_ਕੋਡ);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", ਧਾਰਾ_id, h3_app_error_ਕੋਡ, rv);
  ਜੇ(rv && rv != NGHTTP3_ERR_ਧਾਰਾ_NOT_ਮਿਲਿਆ) {
    Curl_cf_ngtcp2_h3_err_ਸੈੱਟ(cf, ਅੰਕੜੇ, rv);
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ cb_ਧਾਰਾ_reset(ngtcp2_conn *tconn, int64_t ਧਾਰਾ_id,
                           uint64_t final_ਮਾਪ, uint64_t app_error_ਕੋਡ,
                           ਖਾਲੀ *user_ਅੰਕੜੇ, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_cf_ngtcp2_get_xfer(cf, ਧਾਰਾ_user_ਅੰਕੜੇ, ਨਿਰਾ);
  ਪੂਰਨ rv;
  (ਖਾਲੀ)tconn;
  (ਖਾਲੀ)final_ਮਾਪ;
  (ਖਾਲੀ)app_error_ਕੋਡ;

  rv = nghttp3_conn_shutdown_ਧਾਰਾ_read(ctx->h3conn, ਧਾਰਾ_id);
  ਜੇ(ਅੰਕੜੇ)
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] reset -> %d", ਧਾਰਾ_id, rv);
  ਜੇ(rv && rv != NGHTTP3_ERR_ਧਾਰਾ_NOT_ਮਿਲਿਆ) {
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_ਧਾਰਾ_stop_sending(ngtcp2_conn *tconn, int64_t ਧਾਰਾ_id,
                                  uint64_t app_error_ਕੋਡ, ਖਾਲੀ *user_ਅੰਕੜੇ,
                                  ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਪੂਰਨ rv;
  (ਖਾਲੀ)tconn;
  (ਖਾਲੀ)app_error_ਕੋਡ;
  (ਖਾਲੀ)ਧਾਰਾ_user_ਅੰਕੜੇ;

  rv = nghttp3_conn_shutdown_ਧਾਰਾ_read(ctx->h3conn, ਧਾਰਾ_id);
  ਜੇ(rv && rv != NGHTTP3_ERR_ਧਾਰਾ_NOT_ਮਿਲਿਆ) {
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);

  (ਖਾਲੀ)tconn;
  ctx->max_bidi_streams = max_streams;
  ਜੇ(ਅੰਕੜੇ)
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->ਵਰਤਿਆ_bidi_streams);
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_extend_max_ਧਾਰਾ_ਅੰਕੜੇ(ngtcp2_conn *tconn, int64_t ਧਾਰਾ_id,
                                     uint64_t max_ਅੰਕੜੇ, ਖਾਲੀ *user_ਅੰਕੜੇ,
                                     ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ =
    Curl_cf_ngtcp2_get_xfer(cf, ਧਾਰਾ_user_ਅੰਕੜੇ, &ਧਾਰਾ);
  ਪੂਰਨ rv;
  (ਖਾਲੀ)tconn;
  (ਖਾਲੀ)max_ਅੰਕੜੇ;

  rv = nghttp3_conn_unblock_ਧਾਰਾ(ctx->h3conn, ਧਾਰਾ_id);
  ਜੇ(rv && rv != NGHTTP3_ERR_ਧਾਰਾ_NOT_ਮਿਲਿਆ) {
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  ਜੇ(ਧਾਰਾ && ਧਾਰਾ->quic_flow_blocked) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] unblock quic flow", ਧਾਰਾ_id);
    ਧਾਰਾ->quic_flow_blocked = FALSE;
    ਜੇ(ਅੰਕੜੇ)
      Curl_multi_mark_dirty(ਅੰਕੜੇ);
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ cb_rand(uint8_t *dest, size_t destlen,
                    ਅਚਲ ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode ਨਤੀਜਾ;
  (ਖਾਲੀ)rand_ctx;

  ਨਤੀਜਾ = Curl_rand(ਨਿਰਾ, dest, destlen);
  ਜੇ(ਨਤੀਜਾ) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
ਸਥਿਰ ਪੂਰਨ cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ;
  (ਖਾਲੀ)tconn;
  (ਖਾਲੀ)user_ਅੰਕੜੇ;

  ਨਤੀਜਾ = Curl_rand(ਨਿਰਾ, cid->ਅੰਕੜੇ, cidlen);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  ਨਤੀਜਾ = Curl_rand(ਨਿਰਾ, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;

  ਵਾਪਸ 0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
ਸਥਿਰ ਪੂਰਨ cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  ਬਣਤਰ ngtcp2_stateless_reset_token *token, size_t cidlen, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ;
  (ਖਾਲੀ)tconn;
  (ਖਾਲੀ)user_ਅੰਕੜੇ;

  ਨਤੀਜਾ = Curl_rand(ਨਿਰਾ, cid->ਅੰਕੜੇ, cidlen);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  ਨਤੀਜਾ = Curl_rand(ਨਿਰਾ, token->ਅੰਕੜੇ, ਆਕਾਰ(token->ਅੰਕੜੇ));
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;

  ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ cb_recv_rx_ਕੁੰਜੀ(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ਨਿਰਾ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  (ਖਾਲੀ)tconn;

  ਜੇ(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    ਵਾਪਸ 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ਅੰਕੜੇ);
  ਜੇ(ctx && ਅੰਕੜੇ && !ctx->h3conn && ctx->init_h3_conn_cb) {
    ਜੇ(ctx->init_h3_conn_cb(cf, ਅੰਕੜੇ, ctx))
      ਵਾਪਸ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  ਵਾਪਸ 0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_DIAG
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wmissing-field-initializers"
#ਜੇ_ਅੰਤ
ਸਥਿਰ ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  ਨਿਰਾ, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_ਅੰਕੜੇ_cb,
  cb_ngtcp2_handshake_completed,
  ਨਿਰਾ, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_ਨਕਾਬ_cb,
  cb_recv_ਧਾਰਾ_ਅੰਕੜੇ,
  cb_acked_ਧਾਰਾ_ਅੰਕੜੇ_ਵਿੱਥ,
  ਨਿਰਾ, /* stream_open */
  cb_ਧਾਰਾ_close,
  ਨਿਰਾ, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  ਨਿਰਾ, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  ਨਿਰਾ, /* remove_connection_id */
  ngtcp2_crypto_update_ਕੁੰਜੀ_cb, /* update_key */
  ਨਿਰਾ, /* path_validation */
  ਨਿਰਾ, /* select_preferred_addr */
  cb_ਧਾਰਾ_reset,
  ਨਿਰਾ, /* extend_max_remote_streams_bidi */
  ਨਿਰਾ, /* extend_max_remote_streams_uni */
  cb_extend_max_ਧਾਰਾ_ਅੰਕੜੇ,
  ਨਿਰਾ, /* dcid_status */
  ਨਿਰਾ, /* handshake_confirmed */
  ਨਿਰਾ, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  ਨਿਰਾ, /* recv_datagram */
  ਨਿਰਾ, /* ack_datagram */
  ਨਿਰਾ, /* lost_datagram */
  ngtcp2_crypto_get_ਰਸਤਾ_challenge_ਅੰਕੜੇ_cb,
  cb_ਧਾਰਾ_stop_sending,
  ਨਿਰਾ, /* version_negotiation */
  cb_recv_rx_ਕੁੰਜੀ,
  ਨਿਰਾ, /* recv_tx_key */
  ਨਿਰਾ, /* early_data_rejected */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  ਨਿਰਾ, /* begin_path_validation */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  ਨਿਰਾ, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  ਨਿਰਾ, /* dcid_status2 */
  ngtcp2_crypto_get_ਰਸਤਾ_challenge_data2_cb, /* get_path_challenge_data2 */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  ਨਿਰਾ, /* recv_stop_sending */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_ਧਾਰਾ_close2, /* is called instead of cb_stream_close when set */
#ਜੇ_ਅੰਤ
};
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_DIAG
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ

#ਜੇ defined(_MSC_VER) && defined(_DLL)
#ਹਦਾਇਤ warning(ਕੱਢੋ)
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ਸਥਿਰ ਪੂਰਨ quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_ਅੰਕੜੇ(ssl);
  cf = cref ? cref->user_ਅੰਕੜੇ : ਨਿਰਾ;
  ctx = cf ? cf->ctx : ਨਿਰਾ;
  ਅੰਕੜੇ = cf ? CF_ਅੰਕੜੇ_CURRENT(cf) : ਨਿਰਾ;
  ਜੇ(cf && ਅੰਕੜੇ && ctx) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *quic_tp = ਨਿਰਾ;
    size_t quic_tp_len = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     ਆਕਾਰ(tpbuf));
    ਜੇ(tplen < 0)
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((ਪੂਰਨ)tplen));
    ਨਹੀਂਤਾਂ {
      quic_tp = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#ਜੇ_ਅੰਤ
    Curl_ossl_ਜੋੜੋ_session(cf, ਅੰਕੜੇ, &ctx->tls.ossl, ctx->ssl_peer.scache_ਕੁੰਜੀ,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, ਨਿਰਾ);
  }
  ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ /* USE_OPENSSL */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GNUTLS

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *gtls_hs_ਸੁਨੇਹਾ_name(ਪੂਰਨ mtype)
{
  ਚੋਣ(mtype) {
  ਹਾਲਤ 1:
    ਵਾਪਸ "ClientHello";
  ਹਾਲਤ 2:
    ਵਾਪਸ "ServerHello";
  ਹਾਲਤ 4:
    ਵਾਪਸ "SessionTicket";
  ਹਾਲਤ 8:
    ਵਾਪਸ "EncryptedExtensions";
  ਹਾਲਤ 11:
    ਵਾਪਸ "Certificate";
  ਹਾਲਤ 13:
    ਵਾਪਸ "CertificateRequest";
  ਹਾਲਤ 15:
    ਵਾਪਸ "CertificateVerify";
  ਹਾਲਤ 20:
    ਵਾਪਸ "Finished";
  ਹਾਲਤ 24:
    ਵਾਪਸ "KeyUpdate";
  ਹਾਲਤ 254:
    ਵਾਪਸ "MessageHash";
  }
  ਵਾਪਸ "Unknown";
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ quic_gtls_handshake_cb(gnutls_session_t session, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ htype,
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ when, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ incoming,
                                  ਅਚਲ gnutls_datum_t *ਸੁਨੇਹਾ)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  ਬਣਤਰ Curl_cfilter *cf = conn_ref ? conn_ref->user_ਅੰਕੜੇ : ਨਿਰਾ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ਨਿਰਾ;

  (ਖਾਲੀ)ਸੁਨੇਹਾ;
  (ਖਾਲੀ)incoming;
  ਜੇ(when && cf && ctx) { /* after message has been processed */
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
    DEBUGASSERT(ਅੰਕੜੇ);
    ਜੇ(!ਅੰਕੜੇ)
      ਵਾਪਸ 0;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_ਸੁਨੇਹਾ_name(htype), htype);
    ਚੋਣ(htype) {
    ਹਾਲਤ GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *quic_tp = ਨਿਰਾ;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       ਆਕਾਰ(tpbuf));
      ਜੇ(tplen < 0)
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((ਪੂਰਨ)tplen));
      ਨਹੀਂਤਾਂ {
        quic_tp = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (ਖਾਲੀ)Curl_gtls_cache_session(cf, ਅੰਕੜੇ, ctx->ssl_peer.scache_ਕੁੰਜੀ,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    ਨਿਰਾ);
      ਰੋਕੋ;
    }
    ਮੂਲ:
      ਰੋਕੋ;
    }
  }
  ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ /* USE_GNUTLS */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WOLFSSL
ਸਥਿਰ ਪੂਰਨ wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolf_SSL_get_app_ਅੰਕੜੇ(ssl);
  ਬਣਤਰ Curl_cfilter *cf = conn_ref ? conn_ref->user_ਅੰਕੜੇ : ਨਿਰਾ;

  DEBUGASSERT(cf);
  ਜੇ(cf && session) {
    ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
    DEBUGASSERT(ਅੰਕੜੇ);
    ਜੇ(ਅੰਕੜੇ && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *quic_tp = ਨਿਰਾ;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       ਆਕਾਰ(tpbuf));
      ਜੇ(tplen < 0)
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((ਪੂਰਨ)tplen));
      ਨਹੀਂਤਾਂ {
        quic_tp = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (ਖਾਲੀ)Curl_wssl_cache_session(cf, ਅੰਕੜੇ, ctx->ssl_peer.scache_ਕੁੰਜੀ,
                                    session, wolf_SSL_ਸੰਸਕਰਣ(ssl),
                                    "h3", quic_tp, quic_tp_len, ਨਿਰਾ);
    }
  }
  ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ /* USE_WOLFSSL */

ਸਥਿਰ CURLcode cf_ngtcp2_tls_ctx_ਤਿਆਰੀ(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  ਬਣਤਰ curl_tls_ctx *ctx = user_ਅੰਕੜੇ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
#ਜੇ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  ਜੇ(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(ਅੰਕੜੇ, "ngtcp2_crypto_boringssl_configure_client_context failed");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
#ਜਾਂਜੇ defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#ਨਹੀਂਤਾਂ
  ਜੇ(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(ਅੰਕੜੇ, "ngtcp2_crypto_quictls_configure_client_context failed");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
#ਜੇ_ਅੰਤ /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  ਜੇ(Curl_ssl_scache_use(cf, ਅੰਕੜੇ)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_ਸੈੱਟ_session_cache_ਬਹੁਲਕ(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_ਅੰਦਰੂਨੀ);
    SSL_CTX_sess_ਸੈੱਟ_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#ਜਾਂਜੇ defined(USE_GNUTLS)
  ਜੇ(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(ਅੰਕੜੇ, "ngtcp2_crypto_gnutls_configure_client_session failed");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  ਜੇ(Curl_ssl_scache_use(cf, ਅੰਕੜੇ)) {
    gnutls_handshake_ਸੈੱਟ_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#ਜਾਂਜੇ defined(USE_WOLFSSL)
  ਜੇ(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(ਅੰਕੜੇ, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  ਜੇ(Curl_ssl_scache_use(cf, ਅੰਕੜੇ)) {
    /* Register to get notified when a new session is received */
    wolf_SSL_CTX_sess_ਸੈੱਟ_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_ngtcp2_on_session_reuse(ਬਣਤਰ Curl_cfilter *cf,
                                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ alpn_spec *alpns,
                                           ਬਣਤਰ Curl_ssl_session *scs,
                                           bool *do_early_ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *do_early_ਅੰਕੜੇ = FALSE;
#ਜੇ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GNUTLS
  ctx->earlydata_max =
    gnutls_ਰਿਕਾਰਡ_get_max_early_ਅੰਕੜੇ_ਮਾਪ(ctx->tls.gtls.session);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WOLFSSL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WOLFSSL_EARLY_ਅੰਕੜੇ
  ctx->earlydata_max = scs->earlydata_max;
#ਨਹੀਂਤਾਂ
  ctx->earlydata_max = 0;
#ਜੇ_ਅੰਤ /* WOLFSSL_EARLY_DATA */
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  ਜੇ(!ctx->earlydata_max) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL session does not allow earlydata");
  }
  ਨਹੀਂਤਾਂ ਜੇ(!Curl_alpn_ਸ਼ਾਮਲ_ਹੈ_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL session from different ALPN, no early data");
  }
  ਨਹੀਂਤਾਂ ਜੇ(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "no 0RTT transport parameters, no early data");
  }
  ਨਹੀਂਤਾਂ {
    ਪੂਰਨ rv;
    rv = ngtcp2_conn_decode_and_ਸੈੱਟ_0rtt_transport_params(
      ctx->qconn, (ਅਚਲ uint8_t *)scs->quic_tp, scs->quic_tp_len);
    ਜੇ(rv)
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    ਨਹੀਂਤਾਂ ਜੇ(ctx->init_h3_conn_cb) {
      infof(ਅੰਕੜੇ, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      ਨਤੀਜਾ = ctx->init_h3_conn_cb(cf, ਅੰਕੜੇ, ctx);
      ਜੇ(!ਨਤੀਜਾ) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_ਅੰਕੜੇ = TRUE;
      }
    }
    ਨਹੀਂਤਾਂ { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_ਅੰਕੜੇ = TRUE;
    }
  }
#ਨਹੀਂਤਾਂ /* not supported in the TLS backend */
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)ctx;
  (ਖਾਲੀ)scs;
  (ਖਾਲੀ)alpns;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Might be called twice for happy eyeballs.
 */
ਸਥਿਰ CURLcode cf_connect_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ cf_ngtcp2_io_ctx *pktx)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਪੂਰਨ rc;
  ਪੂਰਨ rv;
  CURLcode ਨਤੀਜਾ;
  ਅਚਲ ਬਣਤਰ Curl_sockaddr_ex *sockaddr = ਨਿਰਾ;
  ਪੂਰਨ qfd;
  ਸਥਿਰ ਅਚਲ ਬਣਤਰ alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  ਨਤੀਜਾ = Curl_rand(ਅੰਕੜੇ, ctx->dcid.ਅੰਕੜੇ, NGTCP2_MAX_CIDLEN);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  ਨਤੀਜਾ = Curl_rand(ਅੰਕੜੇ, ctx->scid.ਅੰਕੜੇ, NGTCP2_MAX_CIDLEN);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  (ਖਾਲੀ)Curl_qlogdir(ਅੰਕੜੇ, ctx->scid.ਅੰਕੜੇ, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, ਅੰਕੜੇ, pktx);

  ਨਤੀਜਾ = Curl_vquic_ctx_init(ਅੰਕੜੇ, &ctx->q);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Query socket and remote address from sub-chain */
  ਜੇ(Curl_cf_socket_peek(cf->ਅਗਲਾ, ਅੰਕੜੇ, &ctx->q.sockfd, &sockaddr, ਨਿਰਾ)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  ਜੇ(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = ਆਕਾਰ(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (ਬਣਤਰ sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    ਜੇ(rv == -1)
      ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_ਰਸਤਾ.local,
                     (ਬਣਤਰ sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_ਰਸਤਾ.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  ਨਹੀਂਤਾਂ {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ਅਚਲ ਬਣਤਰ Curl_sockaddr_ex *remote = ਨਿਰਾ;
    ਜੇ(cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, CF_QUERY_REMOTE_ADDR, ਨਿਰਾ,
                            CURL_UNCONST(&remote)))
      ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;
    ਜੇ(!remote)
      ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, ਆਕਾਰ(ctx->q.local_addr));
    ਚੋਣ(remote->family) {
    ਹਾਲਤ AF_INET:
      ((ਬਣਤਰ sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = ਆਕਾਰ(ਬਣਤਰ sockaddr_in);
      ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਹਾਲਤ AF_INET6:
      ((ਬਣਤਰ sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = ਆਕਾਰ(ਬਣਤਰ sockaddr_in6);
      ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    ਮੂਲ:
      ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_ਰਸਤਾ.local,
                     (ਬਣਤਰ sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_ਰਸਤਾ.remote,
                     &remote->curl_sa_addr,
                     (socklen_t)remote->addrlen);
  }

  rc = ngtcp2_conn_client_new(&ctx->qconn, &ctx->dcid, &ctx->scid,
                              &ctx->connected_ਰਸਤਾ,
                              NGTCP2_PROTO_VER_V1, &ng_callbacks,
                              &ctx->settings, &ctx->transport_params,
                              Curl_ngtcp2_mem(), cf);
  ਜੇ(rc)
    ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_ਅੰਕੜੇ = cf;

  ਨਤੀਜਾ = Curl_vquic_tls_init(&ctx->tls, cf, ਅੰਕੜੇ,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_ਤਿਆਰੀ, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

#ਜੇ defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  ਜੇ(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(ਅੰਕੜੇ, "ngtcp2_crypto_ossl_ctx_new failed");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  ngtcp2_conn_ਸੈੱਟ_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  ਜੇ(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(ਅੰਕੜੇ, "ngtcp2_crypto_ossl_configure_client_session failed");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
#ਜਾਂਜੇ defined(USE_OPENSSL)
  SSL_ਸੈੱਟ_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_ਸੈੱਟ_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#ਜਾਂਜੇ defined(USE_GNUTLS)
  ngtcp2_conn_ਸੈੱਟ_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#ਜਾਂਜੇ defined(USE_WOLFSSL)
  ngtcp2_conn_ਸੈੱਟ_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#ਨਹੀਂਤਾਂ
#ਗਲਤੀ "ngtcp2 TLS backend not defined"
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  ਜੇ(ctx->use_earlydata &&
     !SSL_ਸੈੱਟ_quic_tls_early_ਅੰਕੜੇ_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#ਜੇ_ਅੰਤ
  ngtcp2_ccerr_default(&ctx->ਆਖਰੀ_error);

  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  ਬਣਤਰ cf_ngtcp2_io_ctx pktx;

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  /* Connect the sub-chain */
  ਜੇ(cf->ਅਗਲਾ && !cf->ਅਗਲਾ->connected) {
    ਨਤੀਜਾ = Curl_conn_cf_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ || !*ਪੂਰਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  *ਪੂਰਾ = FALSE;

  ਜੇ(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(ਅੰਕੜੇ, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "need HTTPS-RR, delaying connect");
    ਵਾਪਸ CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, ਅੰਕੜੇ);
  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);

  ਜੇ(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(ਅੰਕੜੇ);
    ਨਤੀਜਾ = cf_connect_ਸ਼ੁਰੂ(cf, ਅੰਕੜੇ, &pktx);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਜੇ(cf->connected) {
      *ਪੂਰਾ = TRUE;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ, &pktx);
    /* we do not expect to be able to recv anything yet */
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ, &pktx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ, &pktx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    ਨਤੀਜਾ = ctx->tls_vrfy_ਨਤੀਜਾ;
    ਜੇ(!ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "peer verified");
      cf->connected = TRUE;
      *ਪੂਰਾ = TRUE;
    }
  }

ਬਾਹਰਲਾ:
  ਜੇ(ctx->tls_vrfy_ਨਤੀਜਾ)
    ਨਤੀਜਾ = ctx->tls_vrfy_ਨਤੀਜਾ;
  ਜੇ(ctx->qconn &&
     ((ਨਤੀਜਾ == CURLE_RECV_ERROR) || (ਨਤੀਜਾ == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_ਵਿੱਚ_draining_period(ctx->qconn)) {
    ਅਚਲ ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
    ਜੇ(cerr) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "connect error, type=%d, code=%" PRIu64,
                  (ਪੂਰਨ)cerr->type, cerr->error_ਕੋਡ);
      ਚੋਣ(cerr->type) {
      ਹਾਲਤ NGTCP2_CCERR_TYPE_ਸੰਸਕਰਣ_NEGOTIATION:
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "error in version negotiation");
        ਰੋਕੋ;
      ਮੂਲ:
        ਜੇ(cerr->error_ਕੋਡ >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(ਅੰਕੜੇ, cf, "crypto error, tls alert=%u",
                      (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(cerr->error_ਕੋਡ & 0xffU));
        }
        ਨਹੀਂਤਾਂ ਜੇ(cerr->error_ਕੋਡ == NGTCP2_CONNECTION_REFUSED) {
          CURL_TRC_CF(ਅੰਕੜੇ, cf, "connection refused by server");
          /* When a QUIC server instance is shutting down, it may send us a
           * CONNECTION_CLOSE with this code right away. We want
           * to keep on trying in this case. */
          ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
        }
      }
    }
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      ਬਣਤਰ ip_quadruple ip;

      ਜੇ(!Curl_cf_socket_peek(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਨਿਰਾ, ਨਿਰਾ, &ip))
        infof(ਅੰਕੜੇ, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_ਪੋਰਟ, curl_easy_strerror(ਨਤੀਜਾ));
    }
  }
#ਜੇ_ਅੰਤ
  ਜੇ(!ਨਤੀਜਾ && ctx->qconn) {
    ਨਤੀਜਾ = Curl_cf_ngtcp2_cmn_ਸੈੱਟ_expiry(cf, ਅੰਕੜੇ, &pktx);
  }
  ਜੇ(ਨਤੀਜਾ || *ਪੂਰਾ)
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "connect -> %d, done=%d", (ਪੂਰਨ)ਨਤੀਜਾ, *ਪੂਰਾ);
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  ਬਣਤਰ cf_ngtcp2_io_ctx pktx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(cf->shutdown || !ctx->qconn) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  ਜੇ(!cf->ਅਗਲਾ) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  *ਪੂਰਾ = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, ਅੰਕੜੇ);

  ਜੇ(!ctx->shutdown_started) {
    ਅੱਖਰ ਬਫਰ[NGTCP2_MAX_UDP_PAYLOAD_ਮਾਪ];
    ngtcp2_ssize nwritten;

    ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->q.sendbuf)) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown, flushing sendbuf");
      ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ, &pktx);
      ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->q.sendbuf)) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "sending shutdown packets blocked");
        ਨਤੀਜਾ = CURLE_OK;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown, error %d flushing sendbuf",
                    (ਪੂਰਨ)ਨਤੀਜਾ);
        *ਪੂਰਾ = TRUE;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }

    DEBUGASSERT(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, ਨਿਰਾ, /*ਰਸਤਾ*/
      ਨਿਰਾ, /* pkt_info */
      (uint8_t *)ਬਫਰ, ਆਕਾਰ(ਬਫਰ),
      &ctx->ਆਖਰੀ_error, pktx.ts);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (ਪੂਰਨ)ctx->ਆਖਰੀ_error.type,
                ctx->ਆਖਰੀ_error.error_ਕੋਡ, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    ਜੇ(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      ਨਤੀਜਾ = Curl_bufq_write(&ctx->q.sendbuf, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਬਫਰ,
                               (size_t)nwritten, &n);
      ਜੇ(ਨਤੀਜਾ) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (ਪੂਰਨ)ਨਤੀਜਾ);
        ਜਾਓ ਬਾਹਰਲਾ;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.ਵੰਡੋ_len = 0;
    }
  }

  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->q.sendbuf)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown, flushing egress");
    ਨਤੀਜਾ = Curl_vquic_flush(cf, ਅੰਕੜੇ, &ctx->q);
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "sending shutdown packets blocked");
      ਨਤੀਜਾ = CURLE_OK;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown, error %d flushing sendbuf",
                  (ਪੂਰਨ)ਨਤੀਜਾ);
      *ਪੂਰਾ = TRUE;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown completely sent off, done");
    *ਪੂਰਾ = TRUE;
    ਨਤੀਜਾ = CURLE_OK;
  }
ਬਾਹਰਲਾ:
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_cf_ngtcp2_cmn_conn_close(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  bool ਪੂਰਾ;
  Curl_cf_ngtcp2_cmn_shutdown(cf, ਅੰਕੜੇ, &ਪੂਰਾ);
}

ਸਥਿਰ bool cf_ngtcp2_err_is_fatal(ਪੂਰਨ ਕੋਡ)
{
  ਵਾਪਸ (NGTCP2_ERR_FATAL >= ਕੋਡ) ||
         (NGTCP2_ERR_DROP_CONN == ਕੋਡ) ||
         (NGTCP2_ERR_IDLE_CLOSE == ਕੋਡ);
}

ਖਾਲੀ Curl_cf_ngtcp2_cmn_err_ਸੈੱਟ(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ ਕੋਡ)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਜੇ(!ctx->ਆਖਰੀ_error.error_ਕੋਡ) {
    ਜੇ(NGTCP2_ERR_CRYPTO == ਕੋਡ) {
      ngtcp2_ccerr_ਸੈੱਟ_tls_alert(&ctx->ਆਖਰੀ_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 ਨਿਰਾ, 0);
    }
    ਨਹੀਂਤਾਂ {
      ngtcp2_ccerr_ਸੈੱਟ_liberr(&ctx->ਆਖਰੀ_error, ਕੋਡ, ਨਿਰਾ, 0);
    }
  }
  ਜੇ(cf_ngtcp2_err_is_fatal(ਕੋਡ))
    Curl_cf_ngtcp2_cmn_conn_close(cf, ਅੰਕੜੇ);
}

ਖਾਲੀ Curl_cf_ngtcp2_io_ctx_init(ਬਣਤਰ cf_ngtcp2_io_ctx *io_ctx,
                                ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->ਅੰਕੜੇ = ਅੰਕੜੇ;
  io_ctx->now = *Curl_pgrs_now(ਅੰਕੜੇ);
  ngtcp2_ਰਸਤਾ_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_ਸੈੱਟ_ਸਮਾਂ(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

ਖਾਲੀ Curl_cf_ngtcp2_io_ctx_update_ਸਮਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ cf_ngtcp2_io_ctx *io_ctx,
                                       ਬਣਤਰ Curl_cfilter *cf)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(ਅੰਕੜੇ);
  Curl_vquic_ctx_update_ਸਮਾਂ(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#ਜੇ NGTCP2_ਸੰਸਕਰਣ_ਅੰਕ < 0x011100
ਬਣਤਰ cf_ngtcp2_sfind_ctx {
  int64_t ਧਾਰਾ_id;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  uint32_t mid;
};

ਸਥਿਰ bool cf_ngtcp2_sfind(uint32_t mid, ਖਾਲੀ *ਮੁੱਲ, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ngtcp2_sfind_ctx *fctx = user_ਅੰਕੜੇ;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = ਮੁੱਲ;

  ਜੇ(fctx->ਧਾਰਾ_id == ਧਾਰਾ->id) {
    fctx->mid = mid;
    fctx->ਧਾਰਾ = ਧਾਰਾ;
    ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE; /* continue */
}

ਸਥਿਰ ਬਣਤਰ h3_ਧਾਰਾ_ctx *cf_ngtcp2_get_ਧਾਰਾ(ਬਣਤਰ cf_ngtcp2_ctx *ctx,
                                                  int64_t ਧਾਰਾ_id)
{
  ਬਣਤਰ cf_ngtcp2_sfind_ctx fctx;
  fctx.ਧਾਰਾ_id = ਧਾਰਾ_id;
  fctx.ਧਾਰਾ = ਨਿਰਾ;
  Curl_u32_ptrset_ਦੇਖੋ(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  ਵਾਪਸ fctx.ਧਾਰਾ;
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਬਣਤਰ h3_ਧਾਰਾ_ctx *cf_ngtcp2_get_ਧਾਰਾ(ਬਣਤਰ cf_ngtcp2_ctx *ctx,
                                                  int64_t ਧਾਰਾ_id)
{
  ਵਾਪਸ ngtcp2_conn_get_ਧਾਰਾ_user_ਅੰਕੜੇ(ctx->qconn, ਧਾਰਾ_id);
}
#ਜੇ_ਅੰਤ

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
ਸਥਿਰ CURLcode read_pkt_ਮੰਜ਼ਿਲ_send(ਖਾਲੀ *userp,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t buflen,
                                 size_t *pnread)
{
  ਬਣਤਰ cf_ngtcp2_io_ctx *x = userp;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t ਧਾਰਾ_id;
  ਪੂਰਨ fin;
  ssize_t n;

  *pnread = 0;
  veccnt = 0;
  ਧਾਰਾ_id = -1;
  fin = 0;

  /* ngtcp2 may want to put several frames from different streams into
   * this packet. `NGTCP2_WRITE_STREAM_FLAG_MORE` tells it to do so.
   * When `NGTCP2_ERR_WRITE_MORE` is returned, we *need* to make
   * another iteration.
   * When ngtcp2 is happy (because it has no other frame that would fit
   * or it has nothing more to send), it returns the total length
   * of the assembled packet. This may be 0 if there was nothing to send. */
  ਲਈ(;;) {

    ਜੇ(ctx->h3conn && ngtcp2_conn_get_max_ਅੰਕੜੇ_ਖੱਬਾ(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_ਧਾਰਾ(ctx->h3conn, &ਧਾਰਾ_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      ਜੇ(veccnt < 0) {
        failf(x->ਅੰਕੜੇ, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((ਪੂਰਨ)veccnt));
        Curl_cf_ngtcp2_h3_err_ਸੈੱਟ(x->cf, x->ਅੰਕੜੇ, (ਪੂਰਨ)veccnt);
        ਵਾਪਸ CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_ਧਾਰਾ_FLAG_ਹੋਰ |
            (fin ? NGTCP2_WRITE_ਧਾਰਾ_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_ਧਾਰਾ(ctx->qconn, &x->ps.ਰਸਤਾ,
                                  ਨਿਰਾ, buf, buflen,
                                  &ndatalen, flags, ਧਾਰਾ_id,
                                  (ਅਚਲ ngtcp2_vec *)vec, veccnt, x->ts);
    ਜੇ(n == 0) {
      /* nothing to send */
      ਵਾਪਸ CURLE_AGAIN;
    }
    ਨਹੀਂਤਾਂ ਜੇ(n < 0) {
      ਚੋਣ(n) {
      ਹਾਲਤ NGTCP2_ERR_ਧਾਰਾ_ਅੰਕੜੇ_BLOCKED: {
        ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_ਖੰਡ_ਧਾਰਾ(ctx->h3conn, ਧਾਰਾ_id);
        CURL_TRC_CF(x->ਅੰਕੜੇ, x->cf, "[%" PRId64 "] block quic flow",
                    ਧਾਰਾ_id);
        ਧਾਰਾ = cf_ngtcp2_get_ਧਾਰਾ(ctx, ਧਾਰਾ_id);
        ਜੇ(ਧਾਰਾ) /* it might be not one of our h3 streams? */
          ਧਾਰਾ->quic_flow_blocked = TRUE;
        n = 0;
        ਰੋਕੋ;
      }
      ਹਾਲਤ NGTCP2_ERR_ਧਾਰਾ_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_ਧਾਰਾ_write(ctx->h3conn, ਧਾਰਾ_id);
        n = 0;
        ਰੋਕੋ;
      ਹਾਲਤ NGTCP2_ERR_WRITE_ਹੋਰ:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        ਰੋਕੋ;
      ਮੂਲ:
        DEBUGASSERT(ndatalen == -1);
        failf(x->ਅੰਕੜੇ, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((ਪੂਰਨ)n));
        Curl_cf_ngtcp2_cmn_err_ਸੈੱਟ(x->cf, x->ਅੰਕੜੇ, (ਪੂਰਨ)n);
        ਵਾਪਸ CURLE_SEND_ERROR;
      }
    }

    ਜੇ(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      ਪੂਰਨ rv = nghttp3_conn_ਜੋੜੋ_write_ਵਿੱਥ(ctx->h3conn, ਧਾਰਾ_id, ndatalen);
      ਜੇ(rv) {
        failf(x->ਅੰਕੜੇ, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        ਵਾਪਸ CURLE_SEND_ERROR;
      }
    }

    ਜੇ(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      ਵਾਪਸ CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_ਤਰੱਕੀ_egress(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ cf_ngtcp2_io_ctx *pktx)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_ਮਾਪ, ਰਸਤਾ_max_payload_ਮਾਪ;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_ਸਮਾਂ_ਹਿੱਸਾ;
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ cf_ngtcp2_io_ctx local_pktx;

  ਜੇ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, ਅੰਕੜੇ);
    pktx = &local_pktx;
  }
  ਨਹੀਂਤਾਂ {
    Curl_cf_ngtcp2_io_ctx_update_ਸਮਾਂ(ਅੰਕੜੇ, pktx, cf);
    ngtcp2_ਰਸਤਾ_storage_zero(&pktx->ps);
  }

  ਨਤੀਜਾ = Curl_vquic_flush(cf, ਅੰਕੜੇ, &ctx->q);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_QUIC, 1, &pktx->now);
      ਵਾਪਸ CURLE_OK;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
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
  max_payload_ਮਾਪ = ngtcp2_conn_get_max_tx_udp_payload_ਮਾਪ(ctx->qconn);
  ਰਸਤਾ_max_payload_ਮਾਪ =
    ngtcp2_conn_get_ਰਸਤਾ_max_tx_udp_payload_ਮਾਪ(ctx->qconn);
  send_ਸਮਾਂ_ਹਿੱਸਾ = ngtcp2_conn_get_send_ਸਮਾਂ_ਹਿੱਸਾ(ctx->qconn);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "egress, collect and send packets, quantum=%zu",
              send_ਸਮਾਂ_ਹਿੱਸਾ);
  ਲਈ(;;) {
    /* add the next packet to send, if any, to our buffer */
    ਨਤੀਜਾ = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_ਮਾਪ,
                            read_pkt_ਮੰਜ਼ਿਲ_send, pktx, &nread);
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      ਰੋਕੋ;
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਨਹੀਂਤਾਂ {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      ਜੇ((buflen >= send_ਸਮਾਂ_ਹਿੱਸਾ) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_ਮਾਪ))
        ਰੋਕੋ;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      ਜੇ(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      ਨਹੀਂਤਾਂ ਜੇ(nread > gsolen ||
              (gsolen > ਰਸਤਾ_max_payload_ਮਾਪ && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        ਨਤੀਜਾ = Curl_vquic_send_ਪੂਛ_ਵੰਡੋ(cf, ਅੰਕੜੇ, &ctx->q,
                                            gsolen, nread, nread);
        ਜੇ(ਨਤੀਜਾ) {
          ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
            Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_QUIC, 1, &pktx->now);
            ਵਾਪਸ CURLE_OK;
          }
          ਵਾਪਸ ਨਤੀਜਾ;
        }
        pktcnt = 0;
      }
      ਨਹੀਂਤਾਂ ਜੇ(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        ਰੋਕੋ;
      }
    }
  }

  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    ਨਤੀਜਾ = Curl_vquic_send(cf, ਅੰਕੜੇ, &ctx->q, gsolen);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
        Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_QUIC, 1, &pktx->now);
        ਵਾਪਸ CURLE_OK;
      }
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    Curl_cf_ngtcp2_io_ctx_update_ਸਮਾਂ(ਅੰਕੜੇ, pktx, cf);
    ngtcp2_conn_update_pkt_tx_ਸਮਾਂ(ctx->qconn, pktx->ts);
  }
  ਵਾਪਸ CURLE_OK;
}

ਬਣਤਰ cf_ngtcp2_recv_ctx {
  ਬਣਤਰ cf_ngtcp2_io_ctx *pktx;
  size_t pkt_ਗਿਣਤੀ;
};

ਸਥਿਰ CURLcode cf_ngtcp2_recv_pkts(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t buflen,
                                    size_t gso_ਮਾਪ,
                                    ਬਣਤਰ sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    ਖਾਲੀ *userp)
{
  ਬਣਤਰ cf_ngtcp2_recv_ctx *rctx = userp;
  ਬਣਤਰ cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_ਰਸਤਾ ਰਸਤਾ;
  size_t ਵਿੱਥ, pktlen;
  ਪੂਰਨ rv;

  ਜੇ(!rctx->pkt_ਗਿਣਤੀ) {
    Curl_cf_ngtcp2_io_ctx_update_ਸਮਾਂ(pktx->ਅੰਕੜੇ, pktx, pktx->cf);
    ngtcp2_ਰਸਤਾ_storage_zero(&pktx->ps);
  }

  ਜੇ(ecn)
    CURL_TRC_CF(pktx->ਅੰਕੜੇ, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_ਮਾਪ, (ਬਿਨਾਨਿਸ਼ਾਨੀ)ecn);
  ngtcp2_addr_init(&ਰਸਤਾ.local, (ਬਣਤਰ sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&ਰਸਤਾ.remote, (ਬਣਤਰ sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  ਲਈ(ਵਿੱਥ = 0; ਵਿੱਥ < buflen; ਵਿੱਥ += gso_ਮਾਪ) {
    rctx->pkt_ਗਿਣਤੀ++;
    pktlen = ((ਵਿੱਥ + gso_ਮਾਪ) <= buflen) ? gso_ਮਾਪ : (buflen - ਵਿੱਥ);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &ਰਸਤਾ, &pi,
                              buf + ਵਿੱਥ, pktlen, pktx->ts);
    ਜੇ(rv) {
      CURL_TRC_CF(pktx->ਅੰਕੜੇ, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_ਸੈੱਟ(pktx->cf, pktx->ਅੰਕੜੇ, rv);

      ਜੇ(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
      ਵਾਪਸ CURLE_RECV_ERROR;
    }
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_ਤਰੱਕੀ_ingress(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ cf_ngtcp2_io_ctx *pktx)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ cf_ngtcp2_io_ctx local_pktx;
  ਬਣਤਰ cf_ngtcp2_recv_ctx rctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, ਅੰਕੜੇ);
    pktx = &local_pktx;
  }

  ਨਤੀਜਾ = Curl_vquic_tls_before_recv(&ctx->tls, cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  rctx.pktx = pktx;
  rctx.pkt_ਗਿਣਤੀ = 0;

  ਜੇ(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "progress_ingress(socket)");
    ਵਾਪਸ Curl_vquic_recv_packets(cf, ਅੰਕੜੇ, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  ਨਹੀਂਤਾਂ {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_ਮਾਪ;
    size_t pkt_ਹੱਦਬੰਦੀ = QUIC_TUNNEL_INGRESS_PKT_ਹੱਦਬੰਦੀ;
    size_t nread;
    ਬਣਤਰ sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(ਅੰਕੜੇ, cf, "progress_ingress(sub-filters)");
    ਜੇ(ctx->qconn) {
      size_t max_ਰਸਤਾ_payload;
      max_ਰਸਤਾ_payload =
        ngtcp2_conn_get_ਰਸਤਾ_max_tx_udp_payload_ਮਾਪ(ctx->qconn);
      ਜੇ(max_ਰਸਤਾ_payload > max_udp_payload)
        max_udp_payload = max_ਰਸਤਾ_payload;
    }

    ਜੇ(ctx->tunnel_inbuf_len < max_udp_payload) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      ਜੇ(!newbuf)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    ਜਦੋਂ(pkt_ਹੱਦਬੰਦੀ--) {
      ਨਤੀਜਾ = Curl_conn_cf_recv(cf->ਅਗਲਾ, ਅੰਕੜੇ, (ਅੱਖਰ *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
        /* no more data available at the moment */
        ਵਾਪਸ CURLE_OK;
      }
      ਜੇ(ਨਤੀਜਾ) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "ingress, recv from tunnel failed: %d",
                    (ਪੂਰਨ)ਨਤੀਜਾ);
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਜੇ(nread == 0) {
        /* tunnel closed */
        ਵਾਪਸ CURLE_OK;
      }

      ਸਮ੍ਰਿਤੀ_ਨਕਲ(&remote_addr, ctx->connected_ਰਸਤਾ.remote.addr,
             ctx->connected_ਰਸਤਾ.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_ਰਸਤਾ.remote.addrlen;
      ਨਤੀਜਾ = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      ਜੇ(!ctx->q.got_ਪਹਿਲਾ_byte) {
        ctx->q.got_ਪਹਿਲਾ_byte = TRUE;
        ctx->q.ਪਹਿਲਾ_byte_at = ctx->q.ਆਖਰੀ_op;
      }
      ctx->q.ਆਖਰੀ_io = ctx->q.ਆਖਰੀ_op;
    }
    ਵਾਪਸ CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_ਸੈੱਟ_expiry(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ cf_ngtcp2_io_ctx *pktx)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  ਜੇ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, ਅੰਕੜੇ);
    pktx = &local_pktx;
  }
  ਨਹੀਂਤਾਂ {
    Curl_cf_ngtcp2_io_ctx_update_ਸਮਾਂ(ਅੰਕੜੇ, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  ਜੇ(expiry != UINT64_MAX) {
    ਜੇ(expiry <= pktx->ts) {
      CURLcode ਨਤੀਜਾ;
      ਪੂਰਨ rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      ਜੇ(rv) {
        failf(ਅੰਕੜੇ, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_ਸੈੱਟ(cf, ਅੰਕੜੇ, rv);
        ਵਾਪਸ CURLE_SEND_ERROR;
      }
      ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ, pktx);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ, pktx);
      ਜੇ(ਨਤੀਜਾ) {
        /* a verify failure during ingress must win over generic errors */
        ਜੇ(ctx->tls_vrfy_ਨਤੀਜਾ)
          ਨਤੀਜਾ = ctx->tls_vrfy_ਨਤੀਜਾ;
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    ਜੇ(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      ਜੇ(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cf_ngtcp2_ਤਿਆਰੀ_keep_alive(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਅਚਲ ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  ਜੇ(!ctx->qconn)
    ਵਾਪਸ;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  ਜੇ(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_ਸੈੱਟ_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "no peer idle timeout, unset keep-alive");
  }
  ਨਹੀਂਤਾਂ ਜੇ(!Curl_u32_ptrset_ਗਿਣਤੀ(&ctx->streams)) {
    ngtcp2_conn_ਸੈੱਟ_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "no active streams, unset keep-alive");
  }
  ਨਹੀਂਤਾਂ {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_ਸੈੱਟ_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_ਧਾਰਾ_ਤਿਆਰੀ(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ h3_ਧਾਰਾ_ctx **pstream)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);

  *pstream = ਨਿਰਾ;
  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(ਧਾਰਾ) {
    *pstream = ਧਾਰਾ;
    ਵਾਪਸ CURLE_OK;
  }

  ਧਾਰਾ = curlx_calloc(1, ਆਕਾਰ(*ਧਾਰਾ));
  ਜੇ(!ਧਾਰਾ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਧਾਰਾ->id = -1;
  ਧਾਰਾ->xfer_id = ਅੰਕੜੇ->id;
  ਧਾਰਾ->mid = ਅੰਕੜੇ->mid;
  ਧਾਰਾ->rx_ਵਿੱਥ = 0;
  ਧਾਰਾ->rx_ਵਿੱਥ_max = H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_INITIAL;
  ਧਾਰਾ->tx_ਵਿੱਚ_flight_ideal = H3_ਧਾਰਾ_SEND_BUF_INITIAL;

  /* on send, we control how much we put into the buffer */
  Curl_bufq_initp(&ਧਾਰਾ->sendbuf, &ctx->ਧਾਰਾ_bufcp,
                  H3_ਧਾਰਾ_SEND_CHUNKS, BUFQ_OPT_NONE);
  ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ = 0;
  Curl_h1_req_parse_init(&ਧਾਰਾ->h1, H1_PARSE_DEFAULT_MAX_ਸਤਰ_LEN);

  ਜੇ(Curl_u32_ptrset_ਸੈੱਟ(&ctx->streams, ਅੰਕੜੇ->mid, ਧਾਰਾ)) {
    Curl_cf_ngtcp2_h3_ਧਾਰਾ_ctx_free(ਧਾਰਾ);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਜੇ(Curl_u32_ptrset_ਗਿਣਤੀ(&ctx->streams) == 1)
    cf_ngtcp2_ਤਿਆਰੀ_keep_alive(cf, ਅੰਕੜੇ);

  *pstream = ਧਾਰਾ;
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_cf_ngtcp2_h3_ਧਾਰਾ_close(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(ਧਾਰਾ);
  ਜੇ(!ਧਾਰਾ->closed && ctx->qconn && ctx->h3conn) {
    CURLcode ਨਤੀਜਾ;

    nghttp3_conn_ਸੈੱਟ_ਧਾਰਾ_user_ਅੰਕੜੇ(ctx->h3conn, ਧਾਰਾ->id, ਨਿਰਾ);
    ngtcp2_conn_ਸੈੱਟ_ਧਾਰਾ_user_ਅੰਕੜੇ(ctx->qconn, ਧਾਰਾ->id, ਨਿਰਾ);
    ਧਾਰਾ->closed = TRUE;
    (ਖਾਲੀ)ngtcp2_conn_shutdown_ਧਾਰਾ(ctx->qconn, 0, ਧਾਰਾ->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ, ਨਿਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] cancel stream -> %d",
                  ਧਾਰਾ->id, (ਪੂਰਨ)ਨਤੀਜਾ);
  }
}

ਖਾਲੀ Curl_cf_ngtcp2_h3_ਧਾਰਾ_ਪੂਰਾ(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
  (ਖਾਲੀ)cf;
  ਜੇ(ਧਾਰਾ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] easy handle is done", ਧਾਰਾ->id);
    Curl_cf_ngtcp2_h3_ਧਾਰਾ_close(cf, ਅੰਕੜੇ, ਧਾਰਾ);
    Curl_u32_ptrset_unset(&ctx->streams, ਅੰਕੜੇ->mid);
    ਜੇ(!Curl_u32_ptrset_ਗਿਣਤੀ(&ctx->streams))
      cf_ngtcp2_ਤਿਆਰੀ_keep_alive(cf, ਅੰਕੜੇ);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      bool *ਨਿਵੇਸ਼_pending)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ਅਚਲ ngtcp2_transport_params *rp;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  *ਨਿਵੇਸ਼_pending = FALSE;
  ਜੇ(!ctx->qconn || ctx->shutdown_started)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  ਜੇ(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &ctx->q.ਆਖਰੀ_io);
    ਜੇ(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      ਜੇ((uint64_t)idletime_ms > max_idle_ms)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਜੇ(!cf->ਅਗਲਾ || !cf->ਅਗਲਾ->cft->is_alive(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਨਿਵੇਸ਼_pending))
    ਜਾਓ ਬਾਹਰਲਾ;

  alive = TRUE;
  ਜੇ(*ਨਿਵੇਸ਼_pending) {
    CURLcode ਨਤੀਜਾ;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *ਨਿਵੇਸ਼_pending = FALSE;
    ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ, ਨਿਰਾ);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "is_alive, progress ingress -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);
    alive = ਨਤੀਜਾ ? FALSE : TRUE;
  }

ਬਾਹਰਲਾ:
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(ਬਣਤਰ cf_ngtcp2_ctx *ctx,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  int64_t ctrl_ਧਾਰਾ_id, qpack_enc_ਧਾਰਾ_id, qpack_dec_ਧਾਰਾ_id;
  ਪੂਰਨ rc;

  rc = ngtcp2_conn_open_uni_ਧਾਰਾ(ctx->qconn, &ctrl_ਧਾਰਾ_id, ਨਿਰਾ);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_ਧਾਰਾ(ctx->h3conn, ctrl_ਧਾਰਾ_id);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_ਧਾਰਾ(ctx->qconn, &qpack_enc_ਧਾਰਾ_id, ਨਿਰਾ);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_ਧਾਰਾ(ctx->qconn, &qpack_dec_ਧਾਰਾ_id, ਨਿਰਾ);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_ਧਾਰਾ_id,
                                       qpack_dec_ਧਾਰਾ_id);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;

  ਚੋਣ(query) {
  ਹਾਲਤ CF_QUERY_CONNECT_ਜਵਾਬ_MS:
    ਜੇ((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_ਪਹਿਲਾ_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.ਪਹਿਲਾ_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? (ਪੂਰਨ)ms : ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
      ਵਾਪਸ CURLE_OK;
    }
    ਰੋਕੋ;
  ਹਾਲਤ CF_QUERY_REALLY_CONNECTED:
    ਜੇ(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_ਪਹਿਲਾ_byte;
      ਵਾਪਸ CURLE_OK;
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)arg1;
  (ਖਾਲੀ)arg2;
  ਚੋਣ(event) {
  ਹਾਲਤ CF_CTRL_REPORT_STATS:
    ਜੇ(cf->connected && !ctx->stats_reported) {
      ਜੇ((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_ਪਹਿਲਾ_byte) {
        Curl_pgrs_ਸਮਾਂ_Was(ਅੰਕੜੇ, TIMER_CONNECT, ctx->q.ਪਹਿਲਾ_byte_at);
        ctx->stats_reported = TRUE;
      }
      ਨਹੀਂਤਾਂ ਜੇ(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        ਜੇ(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrs_ਸਮਾਂ_Was(ਅੰਕੜੇ, TIMER_CONNECT, ctx->q.ਪਹਿਲਾ_byte_at);
        Curl_pgrs_ਸਮਾਂ_Was(ਅੰਕੜੇ, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
