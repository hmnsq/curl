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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/err.h>
#ಆದರೆ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_boringssl.h>
#ಅಥವಾಆದರೆ defined(OPENSSL_QUIC_API2)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_ossl.h>
#ಇಲ್ಲದಿದ್ದರೆ
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_quictls.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ "vtls/openssl.h"
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_gnutls.h>
#ಅನ್ತರ್ಭಾವ "vtls/gtls.h"
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ಅನ್ತರ್ಭಾವ "vtls/wolfssl.h"
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <nghttp3/nghttp3.h>

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "u32_ptrset.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-socket.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "http1.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "sockaddr.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "vdns/cf-dns.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic_int.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic-tls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "vquic/cf-ngtcp2-cmn.h"

/*
 * Store ngtcp2 version info in this buffer.
 */
ಶೂನ್ಯ Curl_ngtcp2_ver(ಅಕ್ಷರ *p, size_t len)
{
  ಸ್ಥಿರಾಂಕ ngtcp2_info *ng2 = ngtcp2_ಆವೃತ್ತಿ(0);
  ಸ್ಥಿರಾಂಕ nghttp3_info *ht3 = nghttp3_ಆವೃತ್ತಿ(0);
  (ಶೂನ್ಯ)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->ಆವೃತ್ತಿ_str, ht3->ಆವೃತ್ತಿ_str);
}

ಶೂನ್ಯ Curl_cf_ngtcp2_h3_ಹರಿವು_ctx_free(ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು)
{
  Curl_bufq_free(&ಹರಿವು->sendbuf);
  Curl_h1_req_parse_free(&ಹರಿವು->h1);
  curlx_free(ಹರಿವು);
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_ಹರಿವು_ಗಣ_free(uint32_t id, ಶೂನ್ಯ *ಹರಿವು)
{
  (ಶೂನ್ಯ)id;
  DEBUGASSERT(ಹರಿವು);
  Curl_cf_ngtcp2_h3_ಹರಿವು_ctx_free((ರಚನೆ h3_ಹರಿವು_ctx *)ಹರಿವು);
}

ಸ್ಥಿರ bool cf_ngtcp2_h3_err_is_fatal(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2)
{
  ಹಿಂತಿರುಗಿ (NGHTTP3_ERR_FATAL >= ಸಂಕೇತ_2) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_ಹರಿವು == ಸಂಕೇತ_2);
}

ಶೂನ್ಯ Curl_cf_ngtcp2_h3_err_ಗಣ(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಆದರೆ(!ctx->ಕೊನೆಯ_error.error_ಸಂಕೇತ) {
    ngtcp2_ccerr_ಗಣ_application_error(&ctx->ಕೊನೆಯ_error,
      nghttp3_err_infer_quic_app_error_ಸಂಕೇತ(ಸಂಕೇತ_2), ಖಾಲಿ, 0);
  }
  ಆದರೆ(cf_ngtcp2_h3_err_is_fatal(ಸಂಕೇತ_2))
    Curl_cf_ngtcp2_cmn_conn_close(cf, ದತ್ತಾಂಶ);
}

CURLcode Curl_cf_ngtcp2_ctx_init(ರಚನೆ cf_ngtcp2_ctx *ctx,
                                 ರಚನೆ Curl_peer *origin,
                                 ರಚನೆ Curl_peer *peer,
                                 ರಚನೆ ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = ಖಾಲಿ;
  ctx->tunnel_inbuf_len = 0;
  ctx->ಆವೃತ್ತಿ_2 = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->ಹರಿವು_bufcp, H3_ಹರಿವು_CHUNK_ಗಾತ್ರ,
                  H3_ಹರಿವು_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);
  Curl_u32_ptrset_init(&ctx->streams, h3_ಹರಿವು_ಗಣ_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  ಹಿಂತಿರುಗಿ Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

ಶೂನ್ಯ Curl_cf_ngtcp2_ctx_cleanup(ರಚನೆ cf_ngtcp2_ctx *ctx)
{
  ಆದರೆ(ctx && ctx->initialized) {
    ಆದರೆ(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    ಆದರೆ(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = ಖಾಲಿ;
    }
    ಆದರೆ(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = ಖಾಲಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSSL_QUIC_API2
    ಆದರೆ(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = ಖಾಲಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    Curl_bufcp_free(&ctx->ಹರಿವು_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    curlx_safefree(ctx->tunnel_inbuf);
    ctx->tunnel_inbuf_len = 0;
  }
}

ರಚನೆ Curl_easy *Curl_cf_ngtcp2_get_xfer(ರಚನೆ Curl_cfilter *cf,
                                          ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ,
                                          ರಚನೆ h3_ಹರಿವು_ctx **pstream)
{
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = ಹರಿವು_user_ದತ್ತಾಂಶ;

  ಆದರೆ(pstream)
    *pstream = ಹರಿವು;
  ಆದರೆ(ಹರಿವು && (ಹರಿವು->mid != UINT32_MAX)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, *call_ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
    ಆದರೆ(!call_ದತ್ತಾಂಶ || !call_ದತ್ತಾಂಶ->multi) {
      DEBUGASSERT(0);
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ದತ್ತಾಂಶ = Curl_multi_get_easy(call_ದತ್ತಾಂಶ->multi, ಹರಿವು->mid);
    ಆದರೆ(ದತ್ತಾಂಶ && (ದತ್ತಾಂಶ->id == ಹರಿವು->xfer_id))
      ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
    DEBUGASSERT(0);
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  ರಚನೆ Curl_cfilter *cf = conn_ref->user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಹಿಂತಿರುಗಿ ctx->qconn;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_NGTCP2
ಸ್ಥಿರ ಶೂನ್ಯ quic_printf(ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  va_list ap;
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ qlog_callback(ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, uint32_t flags,
                          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t datalen)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  (ಶೂನ್ಯ)flags;
  ಆದರೆ(ctx->qlogfd != -1) {
    ssize_t rc = ಲಿಖ(ctx->qlogfd, ದತ್ತಾಂಶ, datalen);
    ಆದರೆ(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ quic_settings(ರಚನೆ cf_ngtcp2_ctx *ctx,
                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_NGTCP2
  s->log_printf = quic_printf;
#ಇಲ್ಲದಿದ್ದರೆ
  s->log_printf = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

  s->initial_ts = pktx->ts;
  s->handshake_timeout = (ದತ್ತಾಂಶ->ಗಣ.connecttimeout > 0) ?
    ದತ್ತಾಂಶ->ಗಣ.connecttimeout * NGTCP2_MILLISECONDS : QUIC_HANDSHAKE_TIMEOUT;
  s->max_ಕಿಟಕಿ = H3_CONN_ಕಿಟಕಿ_ಗಾತ್ರ_MAX;
  s->max_ಹರಿವು_ಕಿಟಕಿ = 0; /* disable ngtcp2 auto-tuning of window */
  s->no_pmtud = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NGTCP2_SETTINGS_V3
  /* try ten times the ngtcp2 defaults here for problems with Caddy */
  s->glitch_ratelim_ಅವಧಿ = 1000 * 10;
  s->glitch_ratelim_ದರ = 33 * 10;
#ಆದರೆ_ಅಂತ್ಯ
  t->initial_max_ದತ್ತಾಂಶ = s->max_ಕಿಟಕಿ;
  t->initial_max_ಹರಿವು_ದತ್ತಾಂಶ_bidi_local = H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_INITIAL;
  t->initial_max_ಹರಿವು_ದತ್ತಾಂಶ_bidi_remote = H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_INITIAL;
  t->initial_max_ಹರಿವು_ದತ್ತಾಂಶ_uni = t->initial_max_ದತ್ತಾಂಶ;
  t->initial_max_streams_bidi = QUIC_MAX_STREAMS;
  t->initial_max_streams_uni = QUIC_MAX_STREAMS;
  t->max_idle_timeout = 0; /* no idle timeout from our side */
  ಆದರೆ(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#ಆದರೆ defined(_MSC_VER) && defined(_DLL)
#ಸೂಚನೆ warning(ತಳ್ಳು)
#ಸೂಚನೆ warning(disable:4232) /* MSVC extension, dllimport identity */
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ಖಾಲಿ;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;

  (ಶೂನ್ಯ)tconn;
  DEBUGASSERT(ctx);
  ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಆದರೆ(!ctx || !ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, ದತ್ತಾಂಶ);

  ctx->tls_vrfy_ಫಲಿತಾಂಶ = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    ದತ್ತಾಂಶ, &ctx->ssl_peer);
  ಆದರೆ(ctx->tls_vrfy_ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆದರೆ(Curl_trc_is_verbose(ದತ್ತಾಂಶ)) {
    ಸ್ಥಿರಾಂಕ ngtcp2_transport_params *rp;
    rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "handshake complete after %" FMT_TIMEDIFF_T
                "ms, remote transport[max_udp_payload=%" PRIu64
                ", initial_max_data=%" PRIu64 "]",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at),
                rp->max_udp_payload_ಗಾತ್ರ, rp->initial_max_ದತ್ತಾಂಶ);
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* In case of earlydata, where we simulate being connected, update
   * the handshake time when we really did connect */
  ಆದರೆ(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrs_ಸಮಯ_Was(ದತ್ತಾಂಶ, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  ಆದರೆ(ctx->use_earlydata) {
#ಆದರೆ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    ಆದರೆ(SSL_get_early_ದತ್ತಾಂಶ_ಸ್ಥಿತಿ(ctx->tls.ossl.ssl) == SSL_EARLY_ದತ್ತಾಂಶ_NOT_ಕಳುಹಿಸಿದ)
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "OpenSSL did not send early data");
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ NGTCP2_ಆವೃತ್ತಿ_ಅಂಕೆ >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_ದತ್ತಾಂಶ_rejected2(ctx->qconn);
#ಇಲ್ಲದಿದ್ದರೆ /* older NGTCP2 */
    {
#ಆದರೆ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      ಪೂರ್ಣಾಂಕ ossl_early_ಸ್ಥಿತಿ = SSL_get_early_ದತ್ತಾಂಶ_ಸ್ಥಿತಿ(ctx->tls.ossl.ssl);
      ಆದರೆ(ossl_early_ಸ್ಥಿತಿ == SSL_EARLY_ದತ್ತಾಂಶ_NOT_ಕಳುಹಿಸಿದ)
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_ಸ್ಥಿತಿ == SSL_EARLY_ದತ್ತಾಂಶ_ACCEPTED);
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
      ಪೂರ್ಣಾಂಕ flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_ದತ್ತಾಂಶ);
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WOLFSSL_EARLY_ದತ್ತಾಂಶ
      ctx->earlydata_accepted =
        (wolf_SSL_get_early_ದತ್ತಾಂಶ_ಸ್ಥಿತಿ(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_ದತ್ತಾಂಶ_REJECTED);
#ಇಲ್ಲದಿದ್ದರೆ
      DEBUGASSERT(0); /* should not come here if ED is disabled. */
      ctx->earlydata_accepted = FALSE;
#ಆದರೆ_ಅಂತ್ಯ /* WOLFSSL_EARLY_DATA */
#ಆದರೆ_ಅಂತ್ಯ /* OPENSSL or GNUTLS or WOLFSSL */
    }
#ಆದರೆ_ಅಂತ್ಯ /* older NGTCP2 */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "server did%s accept %zu bytes of early data",
                ctx->earlydata_accepted ? "" : " not", ctx->earlydata_skip);
    Curl_pgrs_Early_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ, ctx->earlydata_accepted ?
                              (curl_off_t)ctx->earlydata_skip :
                             -(curl_off_t)ctx->earlydata_skip);
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_recv_ಹರಿವು_ದತ್ತಾಂಶ(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t ಹರಿವು_id, uint64_t ಸ್ಥಾನಾಂತರ,
                               ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t buflen,
                               ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  ಪೂರ್ಣಾಂಕ fin = (flags & NGTCP2_ಹರಿವು_ದತ್ತಾಂಶ_FLAG_FIN) ? 1 : 0;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ =
    Curl_cf_ngtcp2_get_xfer(cf, ಹರಿವು_user_ದತ್ತಾಂಶ, &ಹರಿವು);

  (ಶೂನ್ಯ)ಸ್ಥಾನಾಂತರ;
  rc = nghttp3_conn_read_ಹರಿವು(ctx->h3conn, ಹರಿವು_id, buf, buflen, fin);
  ಆದರೆ(rc < 0) {
    ಆದರೆ(ದತ್ತಾಂಶ && ಹರಿವು) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  ಹರಿವು_id, ಹರಿವು->reset, ಹರಿವು->closed);
    }
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  ಆದರೆ(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_ಹರಿವು_ಸ್ಥಾನಾಂತರ(tconn, ಹರಿವು_id, nconsumed);
    ngtcp2_conn_extend_max_ಸ್ಥಾನಾಂತರ(tconn, nconsumed);
    ಆದರೆ(ಹರಿವು) {
      ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ += nconsumed;
      ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max += nconsumed;
    }
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_acked_ಹರಿವು_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ(ngtcp2_conn *tconn, int64_t ಹರಿವು_id,
                                       uint64_t ಸ್ಥಾನಾಂತರ, uint64_t datalen,
                                       ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಂಕ rv;
  (ಶೂನ್ಯ)ಹರಿವು_id;
  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)ಸ್ಥಾನಾಂತರ;
  (ಶೂನ್ಯ)datalen;
  (ಶೂನ್ಯ)ಹರಿವು_user_ದತ್ತಾಂಶ;

  rv = nghttp3_conn_ಕೂಡಿಸು_ack_ಸ್ಥಾನಾಂತರ(ctx->h3conn, ಹರಿವು_id, datalen);
  ಆದರೆ(rv && rv != NGHTTP3_ERR_ಹರಿವು_NOT_ಸಿಕ್ಕಿತು) {
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_ಹರಿವು_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t ಹರಿವು_id, uint64_t app_error_ಸಂಕೇತ,
                           ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_cf_ngtcp2_get_xfer(cf, ಹರಿವು_user_ದತ್ತಾಂಶ, ಖಾಲಿ);
  ಪೂರ್ಣಾಂಕ rv;

  (ಶೂನ್ಯ)tconn;
  /* stream is closed... */
  ಆದರೆ(!ದತ್ತಾಂಶ)
    ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;

  ಆದರೆ(!(flags & NGTCP2_ಹರಿವು_CLOSE_FLAG_APP_ERROR_ಸಂಕೇತ_ಗಣ)) {
    app_error_ಸಂಕೇತ = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_ಹರಿವು(ctx->h3conn, ಹರಿವು_id, app_error_ಸಂಕೇತ);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", ಹರಿವು_id, app_error_ಸಂಕೇತ, rv);
  ಆದರೆ(rv && rv != NGHTTP3_ERR_ಹರಿವು_NOT_ಸಿಕ್ಕಿತು) {
    Curl_cf_ngtcp2_h3_err_ಗಣ(cf, ದತ್ತಾಂಶ, rv);
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_ಹರಿವು_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t ಹರಿವು_id,
                           uint64_t rx_app_error_ಸಂಕೇತ,
                           uint64_t tx_app_error_ಸಂಕೇತ,
                           ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_cf_ngtcp2_get_xfer(cf, ಹರಿವು_user_ದತ್ತಾಂಶ, ಖಾಲಿ);
  uint64_t h3_app_error_ಸಂಕೇತ = NGHTTP3_H3_NO_ERROR;
  ಪೂರ್ಣಾಂಕ rv;

  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)tx_app_error_ಸಂಕೇತ;
  /* stream is closed... */
  ಆದರೆ(!ದತ್ತಾಂಶ)
    ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;

  ಆದರೆ(flags & NGTCP2_ಹರಿವು_CLOSE2_FLAG_RX_APP_ERROR_ಸಂಕೇತ_ಗಣ)
    h3_app_error_ಸಂಕೇತ = rx_app_error_ಸಂಕೇತ;

  rv = nghttp3_conn_close_ಹರಿವು(ctx->h3conn, ಹರಿವು_id, h3_app_error_ಸಂಕೇತ);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", ಹರಿವು_id, h3_app_error_ಸಂಕೇತ, rv);
  ಆದರೆ(rv && rv != NGHTTP3_ERR_ಹರಿವು_NOT_ಸಿಕ್ಕಿತು) {
    Curl_cf_ngtcp2_h3_err_ಗಣ(cf, ದತ್ತಾಂಶ, rv);
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_ಹರಿವು_reset(ngtcp2_conn *tconn, int64_t ಹರಿವು_id,
                           uint64_t final_ಗಾತ್ರ, uint64_t app_error_ಸಂಕೇತ,
                           ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_cf_ngtcp2_get_xfer(cf, ಹರಿವು_user_ದತ್ತಾಂಶ, ಖಾಲಿ);
  ಪೂರ್ಣಾಂಕ rv;
  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)final_ಗಾತ್ರ;
  (ಶೂನ್ಯ)app_error_ಸಂಕೇತ;

  rv = nghttp3_conn_shutdown_ಹರಿವು_read(ctx->h3conn, ಹರಿವು_id);
  ಆದರೆ(ದತ್ತಾಂಶ)
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] reset -> %d", ಹರಿವು_id, rv);
  ಆದರೆ(rv && rv != NGHTTP3_ERR_ಹರಿವು_NOT_ಸಿಕ್ಕಿತು) {
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_ಹರಿವು_stop_sending(ngtcp2_conn *tconn, int64_t ಹರಿವು_id,
                                  uint64_t app_error_ಸಂಕೇತ, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                  ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಂಕ rv;
  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)app_error_ಸಂಕೇತ;
  (ಶೂನ್ಯ)ಹರಿವು_user_ದತ್ತಾಂಶ;

  rv = nghttp3_conn_shutdown_ಹರಿವು_read(ctx->h3conn, ಹರಿವು_id);
  ಆದರೆ(rv && rv != NGHTTP3_ERR_ಹರಿವು_NOT_ಸಿಕ್ಕಿತು) {
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);

  (ಶೂನ್ಯ)tconn;
  ctx->max_bidi_streams = max_streams;
  ಆದರೆ(ದತ್ತಾಂಶ)
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->ಬಳಸಿದ_bidi_streams);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_extend_max_ಹರಿವು_ದತ್ತಾಂಶ(ngtcp2_conn *tconn, int64_t ಹರಿವು_id,
                                     uint64_t max_ದತ್ತಾಂಶ, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                     ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ =
    Curl_cf_ngtcp2_get_xfer(cf, ಹರಿವು_user_ದತ್ತಾಂಶ, &ಹರಿವು);
  ಪೂರ್ಣಾಂಕ rv;
  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)max_ದತ್ತಾಂಶ;

  rv = nghttp3_conn_unblock_ಹರಿವು(ctx->h3conn, ಹರಿವು_id);
  ಆದರೆ(rv && rv != NGHTTP3_ERR_ಹರಿವು_NOT_ಸಿಕ್ಕಿತು) {
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  ಆದರೆ(ಹರಿವು && ಹರಿವು->quic_flow_blocked) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] unblock quic flow", ಹರಿವು_id);
    ಹರಿವು->quic_flow_blocked = FALSE;
    ಆದರೆ(ದತ್ತಾಂಶ)
      Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ cb_rand(uint8_t *dest, size_t destlen,
                    ಸ್ಥಿರಾಂಕ ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode ಫಲಿತಾಂಶ;
  (ಶೂನ್ಯ)rand_ctx;

  ಫಲಿತಾಂಶ = Curl_rand(ಖಾಲಿ, dest, destlen);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ;
  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_rand(ಖಾಲಿ, cid->ದತ್ತಾಂಶ, cidlen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  ಫಲಿತಾಂಶ = Curl_rand(ಖಾಲಿ, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;

  ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  ರಚನೆ ngtcp2_stateless_reset_token *token, size_t cidlen, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ;
  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_rand(ಖಾಲಿ, cid->ದತ್ತಾಂಶ, cidlen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  ಫಲಿತಾಂಶ = Curl_rand(ಖಾಲಿ, token->ದತ್ತಾಂಶ, ಗಾತ್ರ(token->ದತ್ತಾಂಶ));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;

  ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_recv_rx_ಕೀಲಿ(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ಖಾಲಿ;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  (ಶೂನ್ಯ)tconn;

  ಆದರೆ(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    ಹಿಂತಿರುಗಿ 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಆದರೆ(ctx && ದತ್ತಾಂಶ && !ctx->h3conn && ctx->init_h3_conn_cb) {
    ಆದರೆ(ctx->init_h3_conn_cb(cf, ದತ್ತಾಂಶ, ctx))
      ಹಿಂತಿರುಗಿ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_DIAG
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wmissing-field-initializers"
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  ಖಾಲಿ, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_ದತ್ತಾಂಶ_cb,
  cb_ngtcp2_handshake_completed,
  ಖಾಲಿ, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_ಮುಖವಾಡ_cb,
  cb_recv_ಹರಿವು_ದತ್ತಾಂಶ,
  cb_acked_ಹರಿವು_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ,
  ಖಾಲಿ, /* stream_open */
  cb_ಹರಿವು_close,
  ಖಾಲಿ, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  ಖಾಲಿ, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  ಖಾಲಿ, /* remove_connection_id */
  ngtcp2_crypto_update_ಕೀಲಿ_cb, /* update_key */
  ಖಾಲಿ, /* path_validation */
  ಖಾಲಿ, /* select_preferred_addr */
  cb_ಹರಿವು_reset,
  ಖಾಲಿ, /* extend_max_remote_streams_bidi */
  ಖಾಲಿ, /* extend_max_remote_streams_uni */
  cb_extend_max_ಹರಿವು_ದತ್ತಾಂಶ,
  ಖಾಲಿ, /* dcid_status */
  ಖಾಲಿ, /* handshake_confirmed */
  ಖಾಲಿ, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  ಖಾಲಿ, /* recv_datagram */
  ಖಾಲಿ, /* ack_datagram */
  ಖಾಲಿ, /* lost_datagram */
  ngtcp2_crypto_get_ಮಾರ್ಗ_challenge_ದತ್ತಾಂಶ_cb,
  cb_ಹರಿವು_stop_sending,
  ಖಾಲಿ, /* version_negotiation */
  cb_recv_rx_ಕೀಲಿ,
  ಖಾಲಿ, /* recv_tx_key */
  ಖಾಲಿ, /* early_data_rejected */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  ಖಾಲಿ, /* begin_path_validation */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  ಖಾಲಿ, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  ಖಾಲಿ, /* dcid_status2 */
  ngtcp2_crypto_get_ಮಾರ್ಗ_challenge_data2_cb, /* get_path_challenge_data2 */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  ಖಾಲಿ, /* recv_stop_sending */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_ಹರಿವು_close2, /* is called instead of cb_stream_close when set */
#ಆದರೆ_ಅಂತ್ಯ
};
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_DIAG
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(_MSC_VER) && defined(_DLL)
#ಸೂಚನೆ warning(ತೆಗೆ)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ cf_ngtcp2_ctx *ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_ದತ್ತಾಂಶ(ssl);
  cf = cref ? cref->user_ದತ್ತಾಂಶ : ಖಾಲಿ;
  ctx = cf ? cf->ctx : ಖಾಲಿ;
  ದತ್ತಾಂಶ = cf ? CF_ದತ್ತಾಂಶ_CURRENT(cf) : ಖಾಲಿ;
  ಆದರೆ(cf && ದತ್ತಾಂಶ && ctx) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *quic_tp = ಖಾಲಿ;
    size_t quic_tp_len = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     ಗಾತ್ರ(tpbuf));
    ಆದರೆ(tplen < 0)
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((ಪೂರ್ಣಾಂಕ)tplen));
    ಇಲ್ಲದಿದ್ದರೆ {
      quic_tp = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#ಆದರೆ_ಅಂತ್ಯ
    Curl_ossl_ಕೂಡಿಸು_session(cf, ದತ್ತಾಂಶ, &ctx->tls.ossl, ctx->ssl_peer.scache_ಕೀಲಿ,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, ಖಾಲಿ);
  }
  ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_OPENSSL */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GNUTLS

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *gtls_hs_ಸಂದೇಶ_name(ಪೂರ್ಣಾಂಕ mtype)
{
  ಆಯ್ಕೆ(mtype) {
  ಸ್ಥಿತಿ 1:
    ಹಿಂತಿರುಗಿ "ClientHello";
  ಸ್ಥಿತಿ 2:
    ಹಿಂತಿರುಗಿ "ServerHello";
  ಸ್ಥಿತಿ 4:
    ಹಿಂತಿರುಗಿ "SessionTicket";
  ಸ್ಥಿತಿ 8:
    ಹಿಂತಿರುಗಿ "EncryptedExtensions";
  ಸ್ಥಿತಿ 11:
    ಹಿಂತಿರುಗಿ "Certificate";
  ಸ್ಥಿತಿ 13:
    ಹಿಂತಿರುಗಿ "CertificateRequest";
  ಸ್ಥಿತಿ 15:
    ಹಿಂತಿರುಗಿ "CertificateVerify";
  ಸ್ಥಿತಿ 20:
    ಹಿಂತಿರುಗಿ "Finished";
  ಸ್ಥಿತಿ 24:
    ಹಿಂತಿರುಗಿ "KeyUpdate";
  ಸ್ಥಿತಿ 254:
    ಹಿಂತಿರುಗಿ "MessageHash";
  }
  ಹಿಂತಿರುಗಿ "Unknown";
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ quic_gtls_handshake_cb(gnutls_session_t session, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ htype,
                                  ಚಿಹ್ನೆರಹಿತ when, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ incoming,
                                  ಸ್ಥಿರಾಂಕ gnutls_datum_t *ಸಂದೇಶ)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  ರಚನೆ Curl_cfilter *cf = conn_ref ? conn_ref->user_ದತ್ತಾಂಶ : ಖಾಲಿ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ಖಾಲಿ;

  (ಶೂನ್ಯ)ಸಂದೇಶ;
  (ಶೂನ್ಯ)incoming;
  ಆದರೆ(when && cf && ctx) { /* after message has been processed */
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
    DEBUGASSERT(ದತ್ತಾಂಶ);
    ಆದರೆ(!ದತ್ತಾಂಶ)
      ಹಿಂತಿರುಗಿ 0;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_ಸಂದೇಶ_name(htype), htype);
    ಆಯ್ಕೆ(htype) {
    ಸ್ಥಿತಿ GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *quic_tp = ಖಾಲಿ;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       ಗಾತ್ರ(tpbuf));
      ಆದರೆ(tplen < 0)
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((ಪೂರ್ಣಾಂಕ)tplen));
      ಇಲ್ಲದಿದ್ದರೆ {
        quic_tp = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (ಶೂನ್ಯ)Curl_gtls_cache_session(cf, ದತ್ತಾಂಶ, ctx->ssl_peer.scache_ಕೀಲಿ,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    ಖಾಲಿ);
      ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }
  }
  ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_GNUTLS */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WOLFSSL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolf_SSL_get_app_ದತ್ತಾಂಶ(ssl);
  ರಚನೆ Curl_cfilter *cf = conn_ref ? conn_ref->user_ದತ್ತಾಂಶ : ಖಾಲಿ;

  DEBUGASSERT(cf);
  ಆದರೆ(cf && session) {
    ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
    DEBUGASSERT(ದತ್ತಾಂಶ);
    ಆದರೆ(ದತ್ತಾಂಶ && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *quic_tp = ಖಾಲಿ;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       ಗಾತ್ರ(tpbuf));
      ಆದರೆ(tplen < 0)
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((ಪೂರ್ಣಾಂಕ)tplen));
      ಇಲ್ಲದಿದ್ದರೆ {
        quic_tp = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (ಶೂನ್ಯ)Curl_wssl_cache_session(cf, ದತ್ತಾಂಶ, ctx->ssl_peer.scache_ಕೀಲಿ,
                                    session, wolf_SSL_ಆವೃತ್ತಿ(ssl),
                                    "h3", quic_tp, quic_tp_len, ಖಾಲಿ);
    }
  }
  ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_WOLFSSL */

ಸ್ಥಿರ CURLcode cf_ngtcp2_tls_ctx_ಸಿದ್ಧತೆ(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  ರಚನೆ curl_tls_ctx *ctx = user_ದತ್ತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
#ಆದರೆ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  ಆದರೆ(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(ದತ್ತಾಂಶ, "ngtcp2_crypto_boringssl_configure_client_context failed");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
#ಅಥವಾಆದರೆ defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(ದತ್ತಾಂಶ, "ngtcp2_crypto_quictls_configure_client_context failed");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
#ಆದರೆ_ಅಂತ್ಯ /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  ಆದರೆ(Curl_ssl_scache_use(cf, ದತ್ತಾಂಶ)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_ಗಣ_session_cache_ಬಹುಲಕ(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_ಆಂತರಿಕ);
    SSL_CTX_sess_ಗಣ_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
  ಆದರೆ(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(ದತ್ತಾಂಶ, "ngtcp2_crypto_gnutls_configure_client_session failed");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  ಆದರೆ(Curl_ssl_scache_use(cf, ದತ್ತಾಂಶ)) {
    gnutls_handshake_ಗಣ_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
  ಆದರೆ(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(ದತ್ತಾಂಶ, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  ಆದರೆ(Curl_ssl_scache_use(cf, ದತ್ತಾಂಶ)) {
    /* Register to get notified when a new session is received */
    wolf_SSL_CTX_sess_ಗಣ_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_ngtcp2_on_session_reuse(ರಚನೆ Curl_cfilter *cf,
                                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ alpn_spec *alpns,
                                           ರಚನೆ Curl_ssl_session *scs,
                                           bool *do_early_ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *do_early_ದತ್ತಾಂಶ = FALSE;
#ಆದರೆ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GNUTLS
  ctx->earlydata_max =
    gnutls_ದಾಖಲೆ_get_max_early_ದತ್ತಾಂಶ_ಗಾತ್ರ(ctx->tls.gtls.session);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WOLFSSL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WOLFSSL_EARLY_ದತ್ತಾಂಶ
  ctx->earlydata_max = scs->earlydata_max;
#ಇಲ್ಲದಿದ್ದರೆ
  ctx->earlydata_max = 0;
#ಆದರೆ_ಅಂತ್ಯ /* WOLFSSL_EARLY_DATA */
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  ಆದರೆ(!ctx->earlydata_max) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL session does not allow earlydata");
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!Curl_alpn_ಒಳಗೊಂಡಿದೆ_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL session from different ALPN, no early data");
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "no 0RTT transport parameters, no early data");
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಪೂರ್ಣಾಂಕ rv;
    rv = ngtcp2_conn_decode_and_ಗಣ_0rtt_transport_params(
      ctx->qconn, (ಸ್ಥಿರಾಂಕ uint8_t *)scs->quic_tp, scs->quic_tp_len);
    ಆದರೆ(rv)
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->init_h3_conn_cb) {
      infof(ದತ್ತಾಂಶ, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      ಫಲಿತಾಂಶ = ctx->init_h3_conn_cb(cf, ದತ್ತಾಂಶ, ctx);
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_ದತ್ತಾಂಶ = TRUE;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_ದತ್ತಾಂಶ = TRUE;
    }
  }
#ಇಲ್ಲದಿದ್ದರೆ /* not supported in the TLS backend */
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)ctx;
  (ಶೂನ್ಯ)scs;
  (ಶೂನ್ಯ)alpns;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Might be called twice for happy eyeballs.
 */
ಸ್ಥಿರ CURLcode cf_connect_ಆರಂಭ(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ cf_ngtcp2_io_ctx *pktx)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಂಕ rc;
  ಪೂರ್ಣಾಂಕ rv;
  CURLcode ಫಲಿತಾಂಶ;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_sockaddr_ex *sockaddr = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ qfd;
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  ಫಲಿತಾಂಶ = Curl_rand(ದತ್ತಾಂಶ, ctx->dcid.ದತ್ತಾಂಶ, NGTCP2_MAX_CIDLEN);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  ಫಲಿತಾಂಶ = Curl_rand(ದತ್ತಾಂಶ, ctx->scid.ದತ್ತಾಂಶ, NGTCP2_MAX_CIDLEN);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)Curl_qlogdir(ದತ್ತಾಂಶ, ctx->scid.ದತ್ತಾಂಶ, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, ದತ್ತಾಂಶ, pktx);

  ಫಲಿತಾಂಶ = Curl_vquic_ctx_init(ದತ್ತಾಂಶ, &ctx->q);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Query socket and remote address from sub-chain */
  ಆದರೆ(Curl_cf_socket_peek(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &ctx->q.sockfd, &sockaddr, ಖಾಲಿ)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  ಆದರೆ(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = ಗಾತ್ರ(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (ರಚನೆ sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    ಆದರೆ(rv == -1)
      ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_ಮಾರ್ಗ.local,
                     (ರಚನೆ sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_ಮಾರ್ಗ.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_sockaddr_ex *remote = ಖಾಲಿ;
    ಆದರೆ(cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, CF_QUERY_REMOTE_ADDR, ಖಾಲಿ,
                            CURL_UNCONST(&remote)))
      ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;
    ಆದರೆ(!remote)
      ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, ಗಾತ್ರ(ctx->q.local_addr));
    ಆಯ್ಕೆ(remote->family) {
    ಸ್ಥಿತಿ AF_INET:
      ((ರಚನೆ sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = ಗಾತ್ರ(ರಚನೆ sockaddr_in);
      ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಸ್ಥಿತಿ AF_INET6:
      ((ರಚನೆ sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = ಗಾತ್ರ(ರಚನೆ sockaddr_in6);
      ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_ಮಾರ್ಗ.local,
                     (ರಚನೆ sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_ಮಾರ್ಗ.remote,
                     &remote->curl_sa_addr,
                     (socklen_t)remote->addrlen);
  }

  rc = ngtcp2_conn_client_new(&ctx->qconn, &ctx->dcid, &ctx->scid,
                              &ctx->connected_ಮಾರ್ಗ,
                              NGTCP2_PROTO_VER_V1, &ng_callbacks,
                              &ctx->settings, &ctx->transport_params,
                              Curl_ngtcp2_mem(), cf);
  ಆದರೆ(rc)
    ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_ದತ್ತಾಂಶ = cf;

  ಫಲಿತಾಂಶ = Curl_vquic_tls_init(&ctx->tls, cf, ದತ್ತಾಂಶ,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_ಸಿದ್ಧತೆ, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

#ಆದರೆ defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  ಆದರೆ(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(ದತ್ತಾಂಶ, "ngtcp2_crypto_ossl_ctx_new failed");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  ngtcp2_conn_ಗಣ_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  ಆದರೆ(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(ದತ್ತಾಂಶ, "ngtcp2_crypto_ossl_configure_client_session failed");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
#ಅಥವಾಆದರೆ defined(USE_OPENSSL)
  SSL_ಗಣ_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_ಗಣ_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
  ngtcp2_conn_ಗಣ_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
  ngtcp2_conn_ಗಣ_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ "ngtcp2 TLS backend not defined"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  ಆದರೆ(ctx->use_earlydata &&
     !SSL_ಗಣ_quic_tls_early_ದತ್ತಾಂಶ_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ngtcp2_ccerr_default(&ctx->ಕೊನೆಯ_error);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    bool *ಮುಗಿದ)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  ರಚನೆ cf_ngtcp2_io_ctx pktx;

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  /* Connect the sub-chain */
  ಆದರೆ(cf->ಮುಂದಿನ && !cf->ಮುಂದಿನ->connected) {
    ಫಲಿತಾಂಶ = Curl_conn_cf_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ || !*ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  *ಮುಗಿದ = FALSE;

  ಆದರೆ(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(ದತ್ತಾಂಶ, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "need HTTPS-RR, delaying connect");
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, ದತ್ತಾಂಶ);
  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);

  ಆದರೆ(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(ದತ್ತಾಂಶ);
    ಫಲಿತಾಂಶ = cf_connect_ಆರಂಭ(cf, ದತ್ತಾಂಶ, &pktx);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಆದರೆ(cf->connected) {
      *ಮುಗಿದ = TRUE;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ, &pktx);
    /* we do not expect to be able to recv anything yet */
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ, &pktx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ, &pktx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    ಫಲಿತಾಂಶ = ctx->tls_vrfy_ಫಲಿತಾಂಶ;
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "peer verified");
      cf->connected = TRUE;
      *ಮುಗಿದ = TRUE;
    }
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ctx->tls_vrfy_ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = ctx->tls_vrfy_ಫಲಿತಾಂಶ;
  ಆದರೆ(ctx->qconn &&
     ((ಫಲಿತಾಂಶ == CURLE_RECV_ERROR) || (ಫಲಿತಾಂಶ == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_ಒಳ_draining_period(ctx->qconn)) {
    ಸ್ಥಿರಾಂಕ ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
    ಆದರೆ(cerr) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "connect error, type=%d, code=%" PRIu64,
                  (ಪೂರ್ಣಾಂಕ)cerr->type, cerr->error_ಸಂಕೇತ);
      ಆಯ್ಕೆ(cerr->type) {
      ಸ್ಥಿತಿ NGTCP2_CCERR_TYPE_ಆವೃತ್ತಿ_NEGOTIATION:
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "error in version negotiation");
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        ಆದರೆ(cerr->error_ಸಂಕೇತ >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "crypto error, tls alert=%u",
                      (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(cerr->error_ಸಂಕೇತ & 0xffU));
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cerr->error_ಸಂಕೇತ == NGTCP2_CONNECTION_REFUSED) {
          CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "connection refused by server");
          /* When a QUIC server instance is shutting down, it may send us a
           * CONNECTION_CLOSE with this code right away. We want
           * to keep on trying in this case. */
          ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
        }
      }
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      ರಚನೆ ip_quadruple ip;

      ಆದರೆ(!Curl_cf_socket_peek(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಖಾಲಿ, ಖಾಲಿ, &ip))
        infof(ದತ್ತಾಂಶ, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_ಪೋರ್ಟ್, curl_easy_strerror(ಫಲಿತಾಂಶ));
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!ಫಲಿತಾಂಶ && ctx->qconn) {
    ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_cmn_ಗಣ_expiry(cf, ದತ್ತಾಂಶ, &pktx);
  }
  ಆದರೆ(ಫಲಿತಾಂಶ || *ಮುಗಿದ)
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "connect -> %d, done=%d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *ಮುಗಿದ);
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  ರಚನೆ cf_ngtcp2_io_ctx pktx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(cf->shutdown || !ctx->qconn) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಆದರೆ(!cf->ಮುಂದಿನ) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  *ಮುಗಿದ = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, ದತ್ತಾಂಶ);

  ಆದರೆ(!ctx->shutdown_started) {
    ಅಕ್ಷರ ಬಫರ್[NGTCP2_MAX_UDP_PAYLOAD_ಗಾತ್ರ];
    ngtcp2_ssize nwritten;

    ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->q.sendbuf)) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown, flushing sendbuf");
      ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ, &pktx);
      ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->q.sendbuf)) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "sending shutdown packets blocked");
        ಫಲಿತಾಂಶ = CURLE_OK;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown, error %d flushing sendbuf",
                    (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        *ಮುಗಿದ = TRUE;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }

    DEBUGASSERT(Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, ಖಾಲಿ, /*ಮಾರ್ಗ*/
      ಖಾಲಿ, /* pkt_info */
      (uint8_t *)ಬಫರ್, ಗಾತ್ರ(ಬಫರ್),
      &ctx->ಕೊನೆಯ_error, pktx.ts);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (ಪೂರ್ಣಾಂಕ)ctx->ಕೊನೆಯ_error.type,
                ctx->ಕೊನೆಯ_error.error_ಸಂಕೇತ, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    ಆದರೆ(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      ಫಲಿತಾಂಶ = Curl_bufq_write(&ctx->q.sendbuf, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಬಫರ್,
                               (size_t)nwritten, &n);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ಹೋಗು ಹೊರಹೋಗು;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.ವಿಭಜಿಸು_len = 0;
    }
  }

  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->q.sendbuf)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown, flushing egress");
    ಫಲಿತಾಂಶ = Curl_vquic_flush(cf, ದತ್ತಾಂಶ, &ctx->q);
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "sending shutdown packets blocked");
      ಫಲಿತಾಂಶ = CURLE_OK;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown, error %d flushing sendbuf",
                  (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      *ಮುಗಿದ = TRUE;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown completely sent off, done");
    *ಮುಗಿದ = TRUE;
    ಫಲಿತಾಂಶ = CURLE_OK;
  }
ಹೊರಹೋಗು:
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_cf_ngtcp2_cmn_conn_close(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  bool ಮುಗಿದ;
  Curl_cf_ngtcp2_cmn_shutdown(cf, ದತ್ತಾಂಶ, &ಮುಗಿದ);
}

ಸ್ಥಿರ bool cf_ngtcp2_err_is_fatal(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2)
{
  ಹಿಂತಿರುಗಿ (NGTCP2_ERR_FATAL >= ಸಂಕೇತ_2) ||
         (NGTCP2_ERR_DROP_CONN == ಸಂಕೇತ_2) ||
         (NGTCP2_ERR_IDLE_CLOSE == ಸಂಕೇತ_2);
}

ಶೂನ್ಯ Curl_cf_ngtcp2_cmn_err_ಗಣ(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಆದರೆ(!ctx->ಕೊನೆಯ_error.error_ಸಂಕೇತ) {
    ಆದರೆ(NGTCP2_ERR_CRYPTO == ಸಂಕೇತ_2) {
      ngtcp2_ccerr_ಗಣ_tls_alert(&ctx->ಕೊನೆಯ_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 ಖಾಲಿ, 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ngtcp2_ccerr_ಗಣ_liberr(&ctx->ಕೊನೆಯ_error, ಸಂಕೇತ_2, ಖಾಲಿ, 0);
    }
  }
  ಆದರೆ(cf_ngtcp2_err_is_fatal(ಸಂಕೇತ_2))
    Curl_cf_ngtcp2_cmn_conn_close(cf, ದತ್ತಾಂಶ);
}

ಶೂನ್ಯ Curl_cf_ngtcp2_io_ctx_init(ರಚನೆ cf_ngtcp2_io_ctx *io_ctx,
                                ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
  io_ctx->now = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  ngtcp2_ಮಾರ್ಗ_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_ಗಣ_ಸಮಯ(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

ಶೂನ್ಯ Curl_cf_ngtcp2_io_ctx_update_ಸಮಯ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ cf_ngtcp2_io_ctx *io_ctx,
                                       ರಚನೆ Curl_cfilter *cf)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  Curl_vquic_ctx_update_ಸಮಯ(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#ಆದರೆ NGTCP2_ಆವೃತ್ತಿ_ಅಂಕೆ < 0x011100
ರಚನೆ cf_ngtcp2_sfind_ctx {
  int64_t ಹರಿವು_id;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  uint32_t mid;
};

ಸ್ಥಿರ bool cf_ngtcp2_sfind(uint32_t mid, ಶೂನ್ಯ *ಮೌಲ್ಯ, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ngtcp2_sfind_ctx *fctx = user_ದತ್ತಾಂಶ;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = ಮೌಲ್ಯ;

  ಆದರೆ(fctx->ಹರಿವು_id == ಹರಿವು->id) {
    fctx->mid = mid;
    fctx->ಹರಿವು = ಹರಿವು;
    ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE; /* continue */
}

ಸ್ಥಿರ ರಚನೆ h3_ಹರಿವು_ctx *cf_ngtcp2_get_ಹರಿವು(ರಚನೆ cf_ngtcp2_ctx *ctx,
                                                  int64_t ಹರಿವು_id)
{
  ರಚನೆ cf_ngtcp2_sfind_ctx fctx;
  fctx.ಹರಿವು_id = ಹರಿವು_id;
  fctx.ಹರಿವು = ಖಾಲಿ;
  Curl_u32_ptrset_ಭೇಟಿ(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  ಹಿಂತಿರುಗಿ fctx.ಹರಿವು;
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ರಚನೆ h3_ಹರಿವು_ctx *cf_ngtcp2_get_ಹರಿವು(ರಚನೆ cf_ngtcp2_ctx *ctx,
                                                  int64_t ಹರಿವು_id)
{
  ಹಿಂತಿರುಗಿ ngtcp2_conn_get_ಹರಿವು_user_ದತ್ತಾಂಶ(ctx->qconn, ಹರಿವು_id);
}
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
ಸ್ಥಿರ CURLcode read_pkt_ಗಮ್ಯ_send(ಶೂನ್ಯ *userp,
                                 ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t buflen,
                                 size_t *pnread)
{
  ರಚನೆ cf_ngtcp2_io_ctx *x = userp;
  ರಚನೆ cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t ಹರಿವು_id;
  ಪೂರ್ಣಾಂಕ fin;
  ssize_t n;

  *pnread = 0;
  veccnt = 0;
  ಹರಿವು_id = -1;
  fin = 0;

  /* ngtcp2 may want to put several frames from different streams into
   * this packet. `NGTCP2_WRITE_STREAM_FLAG_MORE` tells it to do so.
   * When `NGTCP2_ERR_WRITE_MORE` is returned, we *need* to make
   * another iteration.
   * When ngtcp2 is happy (because it has no other frame that would fit
   * or it has nothing more to send), it returns the total length
   * of the assembled packet. This may be 0 if there was nothing to send. */
  ಗಾಗಿ(;;) {

    ಆದರೆ(ctx->h3conn && ngtcp2_conn_get_max_ದತ್ತಾಂಶ_ಎಡ(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_ಹರಿವು(ctx->h3conn, &ಹರಿವು_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      ಆದರೆ(veccnt < 0) {
        failf(x->ದತ್ತಾಂಶ, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((ಪೂರ್ಣಾಂಕ)veccnt));
        Curl_cf_ngtcp2_h3_err_ಗಣ(x->cf, x->ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)veccnt);
        ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_ಹರಿವು_FLAG_ಇನ್ನಷ್ಟು |
            (fin ? NGTCP2_WRITE_ಹರಿವು_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_ಹರಿವು(ctx->qconn, &x->ps.ಮಾರ್ಗ,
                                  ಖಾಲಿ, buf, buflen,
                                  &ndatalen, flags, ಹರಿವು_id,
                                  (ಸ್ಥಿರಾಂಕ ngtcp2_vec *)vec, veccnt, x->ts);
    ಆದರೆ(n == 0) {
      /* nothing to send */
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(n < 0) {
      ಆಯ್ಕೆ(n) {
      ಸ್ಥಿತಿ NGTCP2_ERR_ಹರಿವು_ದತ್ತಾಂಶ_BLOCKED: {
        ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_ಖಂಡ_ಹರಿವು(ctx->h3conn, ಹರಿವು_id);
        CURL_TRC_CF(x->ದತ್ತಾಂಶ, x->cf, "[%" PRId64 "] block quic flow",
                    ಹರಿವು_id);
        ಹರಿವು = cf_ngtcp2_get_ಹರಿವು(ctx, ಹರಿವು_id);
        ಆದರೆ(ಹರಿವು) /* it might be not one of our h3 streams? */
          ಹರಿವು->quic_flow_blocked = TRUE;
        n = 0;
        ನಿಲ್ಲಿಸು;
      }
      ಸ್ಥಿತಿ NGTCP2_ERR_ಹರಿವು_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_ಹರಿವು_write(ctx->h3conn, ಹರಿವು_id);
        n = 0;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ NGTCP2_ERR_WRITE_ಇನ್ನಷ್ಟು:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        DEBUGASSERT(ndatalen == -1);
        failf(x->ದತ್ತಾಂಶ, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((ಪೂರ್ಣಾಂಕ)n));
        Curl_cf_ngtcp2_cmn_err_ಗಣ(x->cf, x->ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)n);
        ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
      }
    }

    ಆದರೆ(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      ಪೂರ್ಣಾಂಕ rv = nghttp3_conn_ಕೂಡಿಸು_write_ಸ್ಥಾನಾಂತರ(ctx->h3conn, ಹರಿವು_id, ndatalen);
      ಆದರೆ(rv) {
        failf(x->ದತ್ತಾಂಶ, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
      }
    }

    ಆದರೆ(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_ಪ್ರಗತಿ_egress(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ cf_ngtcp2_io_ctx *pktx)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_ಗಾತ್ರ, ಮಾರ್ಗ_max_payload_ಗಾತ್ರ;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_ಕಾಲಾಂಶ;
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ cf_ngtcp2_io_ctx local_pktx;

  ಆದರೆ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, ದತ್ತಾಂಶ);
    pktx = &local_pktx;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    Curl_cf_ngtcp2_io_ctx_update_ಸಮಯ(ದತ್ತಾಂಶ, pktx, cf);
    ngtcp2_ಮಾರ್ಗ_storage_zero(&pktx->ps);
  }

  ಫಲಿತಾಂಶ = Curl_vquic_flush(cf, ದತ್ತಾಂಶ, &ctx->q);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_QUIC, 1, &pktx->now);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
  max_payload_ಗಾತ್ರ = ngtcp2_conn_get_max_tx_udp_payload_ಗಾತ್ರ(ctx->qconn);
  ಮಾರ್ಗ_max_payload_ಗಾತ್ರ =
    ngtcp2_conn_get_ಮಾರ್ಗ_max_tx_udp_payload_ಗಾತ್ರ(ctx->qconn);
  send_ಕಾಲಾಂಶ = ngtcp2_conn_get_send_ಕಾಲಾಂಶ(ctx->qconn);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "egress, collect and send packets, quantum=%zu",
              send_ಕಾಲಾಂಶ);
  ಗಾಗಿ(;;) {
    /* add the next packet to send, if any, to our buffer */
    ಫಲಿತಾಂಶ = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_ಗಾತ್ರ,
                            read_pkt_ಗಮ್ಯ_send, pktx, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      ನಿಲ್ಲಿಸು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಇಲ್ಲದಿದ್ದರೆ {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      ಆದರೆ((buflen >= send_ಕಾಲಾಂಶ) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_ಗಾತ್ರ))
        ನಿಲ್ಲಿಸು;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      ಆದರೆ(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread > gsolen ||
              (gsolen > ಮಾರ್ಗ_max_payload_ಗಾತ್ರ && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        ಫಲಿತಾಂಶ = Curl_vquic_send_ಬಾಲ_ವಿಭಜಿಸು(cf, ದತ್ತಾಂಶ, &ctx->q,
                                            gsolen, nread, nread);
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
            Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_QUIC, 1, &pktx->now);
            ಹಿಂತಿರುಗಿ CURLE_OK;
          }
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        pktcnt = 0;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        ನಿಲ್ಲಿಸು;
      }
    }
  }

  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    ಫಲಿತಾಂಶ = Curl_vquic_send(cf, ದತ್ತಾಂಶ, &ctx->q, gsolen);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
        Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_QUIC, 1, &pktx->now);
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    Curl_cf_ngtcp2_io_ctx_update_ಸಮಯ(ದತ್ತಾಂಶ, pktx, cf);
    ngtcp2_conn_update_pkt_tx_ಸಮಯ(ctx->qconn, pktx->ts);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ರಚನೆ cf_ngtcp2_recv_ctx {
  ರಚನೆ cf_ngtcp2_io_ctx *pktx;
  size_t pkt_ಎಣಿಕೆ;
};

ಸ್ಥಿರ CURLcode cf_ngtcp2_recv_pkts(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t buflen,
                                    size_t gso_ಗಾತ್ರ,
                                    ರಚನೆ sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    ಶೂನ್ಯ *userp)
{
  ರಚನೆ cf_ngtcp2_recv_ctx *rctx = userp;
  ರಚನೆ cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  ರಚನೆ cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_ಮಾರ್ಗ ಮಾರ್ಗ;
  size_t ಸ್ಥಾನಾಂತರ, pktlen;
  ಪೂರ್ಣಾಂಕ rv;

  ಆದರೆ(!rctx->pkt_ಎಣಿಕೆ) {
    Curl_cf_ngtcp2_io_ctx_update_ಸಮಯ(pktx->ದತ್ತಾಂಶ, pktx, pktx->cf);
    ngtcp2_ಮಾರ್ಗ_storage_zero(&pktx->ps);
  }

  ಆದರೆ(ecn)
    CURL_TRC_CF(pktx->ದತ್ತಾಂಶ, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_ಗಾತ್ರ, (ಚಿಹ್ನೆರಹಿತ)ecn);
  ngtcp2_addr_init(&ಮಾರ್ಗ.local, (ರಚನೆ sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&ಮಾರ್ಗ.remote, (ರಚನೆ sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  ಗಾಗಿ(ಸ್ಥಾನಾಂತರ = 0; ಸ್ಥಾನಾಂತರ < buflen; ಸ್ಥಾನಾಂತರ += gso_ಗಾತ್ರ) {
    rctx->pkt_ಎಣಿಕೆ++;
    pktlen = ((ಸ್ಥಾನಾಂತರ + gso_ಗಾತ್ರ) <= buflen) ? gso_ಗಾತ್ರ : (buflen - ಸ್ಥಾನಾಂತರ);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &ಮಾರ್ಗ, &pi,
                              buf + ಸ್ಥಾನಾಂತರ, pktlen, pktx->ts);
    ಆದರೆ(rv) {
      CURL_TRC_CF(pktx->ದತ್ತಾಂಶ, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_ಗಣ(pktx->cf, pktx->ದತ್ತಾಂಶ, rv);

      ಆದರೆ(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_ಪ್ರಗತಿ_ingress(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ cf_ngtcp2_io_ctx *pktx)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ cf_ngtcp2_io_ctx local_pktx;
  ರಚನೆ cf_ngtcp2_recv_ctx rctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, ದತ್ತಾಂಶ);
    pktx = &local_pktx;
  }

  ಫಲಿತಾಂಶ = Curl_vquic_tls_before_recv(&ctx->tls, cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  rctx.pktx = pktx;
  rctx.pkt_ಎಣಿಕೆ = 0;

  ಆದರೆ(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "progress_ingress(socket)");
    ಹಿಂತಿರುಗಿ Curl_vquic_recv_packets(cf, ದತ್ತಾಂಶ, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_ಗಾತ್ರ;
    size_t pkt_ಎಲ್ಲೆ = QUIC_TUNNEL_INGRESS_PKT_ಎಲ್ಲೆ;
    size_t nread;
    ರಚನೆ sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "progress_ingress(sub-filters)");
    ಆದರೆ(ctx->qconn) {
      size_t max_ಮಾರ್ಗ_payload;
      max_ಮಾರ್ಗ_payload =
        ngtcp2_conn_get_ಮಾರ್ಗ_max_tx_udp_payload_ಗಾತ್ರ(ctx->qconn);
      ಆದರೆ(max_ಮಾರ್ಗ_payload > max_udp_payload)
        max_udp_payload = max_ಮಾರ್ಗ_payload;
    }

    ಆದರೆ(ctx->tunnel_inbuf_len < max_udp_payload) {
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      ಆದರೆ(!newbuf)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    ಆಗ(pkt_ಎಲ್ಲೆ--) {
      ಫಲಿತಾಂಶ = Curl_conn_cf_recv(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
        /* no more data available at the moment */
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "ingress, recv from tunnel failed: %d",
                    (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ಆದರೆ(nread == 0) {
        /* tunnel closed */
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }

      ಸ್ಮರಣೆ_ನಕಲು(&remote_addr, ctx->connected_ಮಾರ್ಗ.remote.addr,
             ctx->connected_ಮಾರ್ಗ.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_ಮಾರ್ಗ.remote.addrlen;
      ಫಲಿತಾಂಶ = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      ಆದರೆ(!ctx->q.got_ಮೊದಲ_byte) {
        ctx->q.got_ಮೊದಲ_byte = TRUE;
        ctx->q.ಮೊದಲ_byte_at = ctx->q.ಕೊನೆಯ_op;
      }
      ctx->q.ಕೊನೆಯ_io = ctx->q.ಕೊನೆಯ_op;
    }
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_ಗಣ_expiry(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ cf_ngtcp2_io_ctx *pktx)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  ಆದರೆ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, ದತ್ತಾಂಶ);
    pktx = &local_pktx;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    Curl_cf_ngtcp2_io_ctx_update_ಸಮಯ(ದತ್ತಾಂಶ, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  ಆದರೆ(expiry != UINT64_MAX) {
    ಆದರೆ(expiry <= pktx->ts) {
      CURLcode ಫಲಿತಾಂಶ;
      ಪೂರ್ಣಾಂಕ rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      ಆದರೆ(rv) {
        failf(ದತ್ತಾಂಶ, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_ಗಣ(cf, ದತ್ತಾಂಶ, rv);
        ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
      }
      ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ, pktx);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ, pktx);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        /* a verify failure during ingress must win over generic errors */
        ಆದರೆ(ctx->tls_vrfy_ಫಲಿತಾಂಶ)
          ಫಲಿತಾಂಶ = ctx->tls_vrfy_ಫಲಿತಾಂಶ;
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    ಆದರೆ(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      ಆದರೆ(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ngtcp2_ಸಿದ್ಧತೆ_keep_alive(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸ್ಥಿರಾಂಕ ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  ಆದರೆ(!ctx->qconn)
    ಹಿಂತಿರುಗಿ;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  ಆದರೆ(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_ಗಣ_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "no peer idle timeout, unset keep-alive");
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!Curl_u32_ptrset_ಎಣಿಕೆ(&ctx->streams)) {
    ngtcp2_conn_ಗಣ_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "no active streams, unset keep-alive");
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_ಗಣ_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_ಹರಿವು_ಸಿದ್ಧತೆ(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ h3_ಹರಿವು_ctx **pstream)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);

  *pstream = ಖಾಲಿ;
  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(ಹರಿವು) {
    *pstream = ಹರಿವು;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಹರಿವು = curlx_calloc(1, ಗಾತ್ರ(*ಹರಿವು));
  ಆದರೆ(!ಹರಿವು)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಹರಿವು->id = -1;
  ಹರಿವು->xfer_id = ದತ್ತಾಂಶ->id;
  ಹರಿವು->mid = ದತ್ತಾಂಶ->mid;
  ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ = 0;
  ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max = H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_INITIAL;
  ಹರಿವು->tx_ಒಳ_flight_ideal = H3_ಹರಿವು_SEND_BUF_INITIAL;

  /* on send, we control how much we put into the buffer */
  Curl_bufq_initp(&ಹರಿವು->sendbuf, &ctx->ಹರಿವು_bufcp,
                  H3_ಹರಿವು_SEND_CHUNKS, BUFQ_OPT_NONE);
  ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ = 0;
  Curl_h1_req_parse_init(&ಹರಿವು->h1, H1_PARSE_DEFAULT_MAX_ಸಾಲು_LEN);

  ಆದರೆ(Curl_u32_ptrset_ಗಣ(&ctx->streams, ದತ್ತಾಂಶ->mid, ಹರಿವು)) {
    Curl_cf_ngtcp2_h3_ಹರಿವು_ctx_free(ಹರಿವು);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಆದರೆ(Curl_u32_ptrset_ಎಣಿಕೆ(&ctx->streams) == 1)
    cf_ngtcp2_ಸಿದ್ಧತೆ_keep_alive(cf, ದತ್ತಾಂಶ);

  *pstream = ಹರಿವು;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_cf_ngtcp2_h3_ಹರಿವು_close(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(ಹರಿವು);
  ಆದರೆ(!ಹರಿವು->closed && ctx->qconn && ctx->h3conn) {
    CURLcode ಫಲಿತಾಂಶ;

    nghttp3_conn_ಗಣ_ಹರಿವು_user_ದತ್ತಾಂಶ(ctx->h3conn, ಹರಿವು->id, ಖಾಲಿ);
    ngtcp2_conn_ಗಣ_ಹರಿವು_user_ದತ್ತಾಂಶ(ctx->qconn, ಹರಿವು->id, ಖಾಲಿ);
    ಹರಿವು->closed = TRUE;
    (ಶೂನ್ಯ)ngtcp2_conn_shutdown_ಹರಿವು(ctx->qconn, 0, ಹರಿವು->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ, ಖಾಲಿ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] cancel stream -> %d",
                  ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  }
}

ಶೂನ್ಯ Curl_cf_ngtcp2_h3_ಹರಿವು_ಮುಗಿದ(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
  (ಶೂನ್ಯ)cf;
  ಆದರೆ(ಹರಿವು) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] easy handle is done", ಹರಿವು->id);
    Curl_cf_ngtcp2_h3_ಹರಿವು_close(cf, ದತ್ತಾಂಶ, ಹರಿವು);
    Curl_u32_ptrset_unset(&ctx->streams, ದತ್ತಾಂಶ->mid);
    ಆದರೆ(!Curl_u32_ptrset_ಎಣಿಕೆ(&ctx->streams))
      cf_ngtcp2_ಸಿದ್ಧತೆ_keep_alive(cf, ದತ್ತಾಂಶ);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      bool *ಒಳಹರಿವು_pending)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ಸ್ಥಿರಾಂಕ ngtcp2_transport_params *rp;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  *ಒಳಹರಿವು_pending = FALSE;
  ಆದರೆ(!ctx->qconn || ctx->shutdown_started)
    ಹೋಗು ಹೊರಹೋಗು;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  ಆದರೆ(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &ctx->q.ಕೊನೆಯ_io);
    ಆದರೆ(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      ಆದರೆ((uint64_t)idletime_ms > max_idle_ms)
        ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಆದರೆ(!cf->ಮುಂದಿನ || !cf->ಮುಂದಿನ->cft->is_alive(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಒಳಹರಿವು_pending))
    ಹೋಗು ಹೊರಹೋಗು;

  alive = TRUE;
  ಆದರೆ(*ಒಳಹರಿವು_pending) {
    CURLcode ಫಲಿತಾಂಶ;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *ಒಳಹರಿವು_pending = FALSE;
    ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ, ಖಾಲಿ);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "is_alive, progress ingress -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    alive = ಫಲಿತಾಂಶ ? FALSE : TRUE;
  }

ಹೊರಹೋಗು:
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(ರಚನೆ cf_ngtcp2_ctx *ctx,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  int64_t ctrl_ಹರಿವು_id, qpack_enc_ಹರಿವು_id, qpack_dec_ಹರಿವು_id;
  ಪೂರ್ಣಾಂಕ rc;

  rc = ngtcp2_conn_open_uni_ಹರಿವು(ctx->qconn, &ctrl_ಹರಿವು_id, ಖಾಲಿ);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_ಹರಿವು(ctx->h3conn, ctrl_ಹರಿವು_id);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_ಹರಿವು(ctx->qconn, &qpack_enc_ಹರಿವು_id, ಖಾಲಿ);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_ಹರಿವು(ctx->qconn, &qpack_dec_ಹರಿವು_id, ಖಾಲಿ);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_ಹರಿವು_id,
                                       qpack_dec_ಹರಿವು_id);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;

  ಆಯ್ಕೆ(query) {
  ಸ್ಥಿತಿ CF_QUERY_CONNECT_ಉತ್ತರ_MS:
    ಆದರೆ((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_ಮೊದಲ_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.ಮೊದಲ_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? (ಪೂರ್ಣಾಂಕ)ms : ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_QUERY_REALLY_CONNECTED:
    ಆದರೆ(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_ಮೊದಲ_byte;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  ಆಯ್ಕೆ(event) {
  ಸ್ಥಿತಿ CF_CTRL_REPORT_STATS:
    ಆದರೆ(cf->connected && !ctx->stats_reported) {
      ಆದರೆ((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_ಮೊದಲ_byte) {
        Curl_pgrs_ಸಮಯ_Was(ದತ್ತಾಂಶ, TIMER_CONNECT, ctx->q.ಮೊದಲ_byte_at);
        ctx->stats_reported = TRUE;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        ಆದರೆ(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrs_ಸಮಯ_Was(ದತ್ತಾಂಶ, TIMER_CONNECT, ctx->q.ಮೊದಲ_byte_at);
        Curl_pgrs_ಸಮಯ_Was(ದತ್ತಾಂಶ, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
