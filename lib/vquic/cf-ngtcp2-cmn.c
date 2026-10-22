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

#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2.h>

#ifdef USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/err.h>
#ಯದಿ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_boringssl.h>
#ನೋಚೇದ್ಯದಿ defined(OPENSSL_QUIC_API2)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_ossl.h>
#ಅನ್ಯಥಾ
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#ಅನ್ತರ್ಭಾವ "vtls/openssl.h"
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_gnutls.h>
#ಅನ್ತರ್ಭಾವ "vtls/gtls.h"
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ಅನ್ತರ್ಭಾವ "vtls/wolfssl.h"
#endif

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
ಶೂನ್ಯ Curl_ngtcp2_ver(ವರ್ಣ *p, size_t len)
{
  ಧ್ರುವ ngtcp2_info *ng2 = ngtcp2_version(0);
  ಧ್ರುವ nghttp3_info *ht3 = nghttp3_version(0);
  (ಶೂನ್ಯ)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->version_str, ht3->version_str);
}

ಶೂನ್ಯ Curl_cf_ngtcp2_h3_stream_ctx_free(ಸಂರಚನಾ h3_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_stream_set_free(uint32_t id, ಶೂನ್ಯ *stream)
{
  (ಶೂನ್ಯ)id;
  DEBUGASSERT(stream);
  Curl_cf_ngtcp2_h3_stream_ctx_free((ಸಂರಚನಾ h3_stream_ctx *)stream);
}

ಸ್ಥಿರ bool cf_ngtcp2_h3_err_is_fatal(ಪೂರ್ಣಾಙ್ಕ code)
{
  ಪ್ರತ್ಯಾಗಮ (NGHTTP3_ERR_FATAL >= code) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM == code);
}

ಶೂನ್ಯ Curl_cf_ngtcp2_h3_err_set(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ code)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಯದಿ(!ctx->last_error.error_code) {
    ngtcp2_ccerr_set_application_error(&ctx->last_error,
      nghttp3_err_infer_quic_app_error_code(code), ರಿಕ್ತ, 0);
  }
  ಯದಿ(cf_ngtcp2_h3_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

CURLcode Curl_cf_ngtcp2_ctx_init(ಸಂರಚನಾ cf_ngtcp2_ctx *ctx,
                                 ಸಂರಚನಾ Curl_peer *origin,
                                 ಸಂರಚನಾ Curl_peer *peer,
                                 ಸಂರಚನಾ ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = ರಿಕ್ತ;
  ctx->tunnel_inbuf_len = 0;
  ctx->version = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->stream_bufcp, H3_STREAM_CHUNK_SIZE,
                  H3_STREAM_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  ಪ್ರತ್ಯಾಗಮ Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

ಶೂನ್ಯ Curl_cf_ngtcp2_ctx_cleanup(ಸಂರಚನಾ cf_ngtcp2_ctx *ctx)
{
  ಯದಿ(ctx && ctx->initialized) {
    ಯದಿ(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    ಯದಿ(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = ರಿಕ್ತ;
    }
    ಯದಿ(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = ರಿಕ್ತ;
    }
#ifdef OPENSSL_QUIC_API2
    ಯದಿ(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = ರಿಕ್ತ;
    }
#endif
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    curlx_safefree(ctx->tunnel_inbuf);
    ctx->tunnel_inbuf_len = 0;
  }
}

ಸಂರಚನಾ Curl_easy *Curl_cf_ngtcp2_get_xfer(ಸಂರಚನಾ Curl_cfilter *cf,
                                          ಶೂನ್ಯ *stream_user_data,
                                          ಸಂರಚನಾ h3_stream_ctx **pstream)
{
  ಸಂರಚನಾ h3_stream_ctx *stream = stream_user_data;

  ಯದಿ(pstream)
    *pstream = stream;
  ಯದಿ(stream && (stream->mid != UINT32_MAX)) {
    ಸಂರಚನಾ Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    ಯದಿ(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    ಯದಿ(data && (data->id == stream->xfer_id))
      ಪ್ರತ್ಯಾಗಮ data;
    DEBUGASSERT(0);
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  ಸಂರಚನಾ Curl_cfilter *cf = conn_ref->user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಪ್ರತ್ಯಾಗಮ ctx->qconn;
}

#ifdef DEBUG_NGTCP2
ಸ್ಥಿರ ಶೂನ್ಯ quic_printf(ಶೂನ್ಯ *user_data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  va_list ap;
  (ಶೂನ್ಯ)user_data;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#endif

ಸ್ಥಿರ ಶೂನ್ಯ qlog_callback(ಶೂನ್ಯ *user_data, uint32_t flags,
                          ಧ್ರುವ ಶೂನ್ಯ *data, size_t datalen)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  (ಶೂನ್ಯ)flags;
  ಯದಿ(ctx->qlogfd != -1) {
    ssize_t rc = ಲಿಖ(ctx->qlogfd, data, datalen);
    ಯದಿ(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ quic_settings(ಸಂರಚನಾ cf_ngtcp2_ctx *ctx,
                          ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#ifdef DEBUG_NGTCP2
  s->log_printf = quic_printf;
#ಅನ್ಯಥಾ
  s->log_printf = ರಿಕ್ತ;
#endif

  s->initial_ts = pktx->ts;
  s->handshake_timeout = (data->set.connecttimeout > 0) ?
    data->set.connecttimeout * NGTCP2_MILLISECONDS : QUIC_HANDSHAKE_TIMEOUT;
  s->max_window = H3_CONN_WINDOW_SIZE_MAX;
  s->max_stream_window = 0; /* disable ngtcp2 auto-tuning of window */
  s->no_pmtud = FALSE;
#ifdef NGTCP2_SETTINGS_V3
  /* try ten times the ngtcp2 defaults here for problems with Caddy */
  s->glitch_ratelim_burst = 1000 * 10;
  s->glitch_ratelim_rate = 33 * 10;
#endif
  t->initial_max_data = s->max_window;
  t->initial_max_stream_data_bidi_local = H3_STREAM_WINDOW_SIZE_INITIAL;
  t->initial_max_stream_data_bidi_remote = H3_STREAM_WINDOW_SIZE_INITIAL;
  t->initial_max_stream_data_uni = t->initial_max_data;
  t->initial_max_streams_bidi = QUIC_MAX_STREAMS;
  t->initial_max_streams_uni = QUIC_MAX_STREAMS;
  t->max_idle_timeout = 0; /* no idle timeout from our side */
  ಯದಿ(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#ಯದಿ defined(_MSC_VER) && defined(_DLL)
#pragma warning(ತಳ್ಳು)
#pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, ಶೂನ್ಯ *user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_easy *data;

  (ಶೂನ್ಯ)tconn;
  DEBUGASSERT(ctx);
  data = CF_DATA_CURRENT(cf);
  DEBUGASSERT(data);
  ಯದಿ(!ctx || !data)
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(data);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, data);

  ctx->tls_vrfy_result = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    data, &ctx->ssl_peer);
  ಯದಿ(ctx->tls_vrfy_result)
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;

#ifdef CURLVERBOSE
  ಯದಿ(Curl_trc_is_verbose(data)) {
    ಧ್ರುವ ngtcp2_transport_params *rp;
    rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
    CURL_TRC_CF(data, cf, "handshake complete after %" FMT_TIMEDIFF_T
                "ms, remote transport[max_udp_payload=%" PRIu64
                ", initial_max_data=%" PRIu64 "]",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at),
                rp->max_udp_payload_size, rp->initial_max_data);
  }
#endif

  /* In case of earlydata, where we simulate being connected, update
   * the handshake time when we really did connect */
  ಯದಿ(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  ಯದಿ(ctx->use_earlydata) {
#ಯದಿ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    ಯದಿ(SSL_get_early_data_status(ctx->tls.ossl.ssl) == SSL_EARLY_DATA_NOT_SENT)
      CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
#endif

#ಯದಿ NGTCP2_VERSION_ಅಂಕೆ >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_data_rejected2(ctx->qconn);
#ಅನ್ಯಥಾ /* older NGTCP2 */
    {
#ಯದಿ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      ಪೂರ್ಣಾಙ್ಕ ossl_early_status = SSL_get_early_data_status(ctx->tls.ossl.ssl);
      ಯದಿ(ossl_early_status == SSL_EARLY_DATA_NOT_SENT)
        CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_status == SSL_EARLY_DATA_ACCEPTED);
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
      ಪೂರ್ಣಾಙ್ಕ flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_DATA);
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
#ifdef WOLFSSL_EARLY_DATA
      ctx->earlydata_accepted =
        (wolfSSL_get_early_data_status(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_DATA_REJECTED);
#ಅನ್ಯಥಾ
      DEBUGASSERT(0); /* should not come here if ED is disabled. */
      ctx->earlydata_accepted = FALSE;
#endif /* WOLFSSL_EARLY_DATA */
#endif /* OPENSSL or GNUTLS or WOLFSSL */
    }
#endif /* older NGTCP2 */
    CURL_TRC_CF(data, cf, "server did%s accept %zu bytes of early data",
                ctx->earlydata_accepted ? "" : " not", ctx->earlydata_skip);
    Curl_pgrsEarlyData(data, ctx->earlydata_accepted ?
                              (curl_off_t)ctx->earlydata_skip :
                             -(curl_off_t)ctx->earlydata_skip);
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_recv_stream_data(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t stream_id, uint64_t offset,
                               ಧ್ರುವ uint8_t *buf, size_t buflen,
                               ಶೂನ್ಯ *user_data, ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  ಪೂರ್ಣಾಙ್ಕ fin = (flags & NGTCP2_STREAM_DATA_FLAG_FIN) ? 1 : 0;
  ಸಂರಚನಾ h3_stream_ctx *stream;
  ಸಂರಚನಾ Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (ಶೂನ್ಯ)offset;
  rc = nghttp3_conn_read_stream(ctx->h3conn, stream_id, buf, buflen, fin);
  ಯದಿ(rc < 0) {
    ಯದಿ(data && stream) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  stream_id, stream->reset, stream->closed);
    }
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  ಯದಿ(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_stream_offset(tconn, stream_id, nconsumed);
    ngtcp2_conn_extend_max_offset(tconn, nconsumed);
    ಯದಿ(stream) {
      stream->rx_offset += nconsumed;
      stream->rx_offset_max += nconsumed;
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_acked_stream_data_offset(ngtcp2_conn *tconn, int64_t stream_id,
                                       uint64_t offset, uint64_t datalen,
                                       ಶೂನ್ಯ *user_data, ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಙ್ಕ rv;
  (ಶೂನ್ಯ)stream_id;
  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)offset;
  (ಶೂನ್ಯ)datalen;
  (ಶೂನ್ಯ)stream_user_data;

  rv = nghttp3_conn_add_ack_offset(ctx->h3conn, stream_id, datalen);
  ಯದಿ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_stream_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id, uint64_t app_error_code,
                           ಶೂನ್ಯ *user_data, ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ರಿಕ್ತ);
  ಪೂರ್ಣಾಙ್ಕ rv;

  (ಶೂನ್ಯ)tconn;
  /* stream is closed... */
  ಯದಿ(!data)
    data = CF_DATA_CURRENT(cf);
  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;

  ಯದಿ(!(flags & NGTCP2_STREAM_CLOSE_FLAG_APP_ERROR_CODE_SET)) {
    app_error_code = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, app_error_code, rv);
  ಯದಿ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}

#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_stream_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id,
                           uint64_t rx_app_error_code,
                           uint64_t tx_app_error_code,
                           ಶೂನ್ಯ *user_data, ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ರಿಕ್ತ);
  uint64_t h3_app_error_code = NGHTTP3_H3_NO_ERROR;
  ಪೂರ್ಣಾಙ್ಕ rv;

  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)tx_app_error_code;
  /* stream is closed... */
  ಯದಿ(!data)
    data = CF_DATA_CURRENT(cf);
  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;

  ಯದಿ(flags & NGTCP2_STREAM_CLOSE2_FLAG_RX_APP_ERROR_CODE_SET)
    h3_app_error_code = rx_app_error_code;

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, h3_app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, h3_app_error_code, rv);
  ಯದಿ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_stream_reset(ngtcp2_conn *tconn, int64_t stream_id,
                           uint64_t final_size, uint64_t app_error_code,
                           ಶೂನ್ಯ *user_data, ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ರಿಕ್ತ);
  ಪೂರ್ಣಾಙ್ಕ rv;
  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)final_size;
  (ಶೂನ್ಯ)app_error_code;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  ಯದಿ(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  ಯದಿ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_stream_stop_sending(ngtcp2_conn *tconn, int64_t stream_id,
                                  uint64_t app_error_code, ಶೂನ್ಯ *user_data,
                                  ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಙ್ಕ rv;
  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)app_error_code;
  (ಶೂನ್ಯ)stream_user_data;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  ಯದಿ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            ಶೂನ್ಯ *user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);

  (ಶೂನ್ಯ)tconn;
  ctx->max_bidi_streams = max_streams;
  ಯದಿ(data)
    CURL_TRC_CF(data, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->used_bidi_streams);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_extend_max_stream_data(ngtcp2_conn *tconn, int64_t stream_id,
                                     uint64_t max_data, ಶೂನ್ಯ *user_data,
                                     ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream;
  ಸಂರಚನಾ Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  ಪೂರ್ಣಾಙ್ಕ rv;
  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)max_data;

  rv = nghttp3_conn_unblock_stream(ctx->h3conn, stream_id);
  ಯದಿ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  ಯದಿ(stream && stream->quic_flow_blocked) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] unblock quic flow", stream_id);
    stream->quic_flow_blocked = FALSE;
    ಯದಿ(data)
      Curl_multi_mark_dirty(data);
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ cb_rand(uint8_t *dest, size_t destlen,
                    ಧ್ರುವ ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode result;
  (ಶೂನ್ಯ)rand_ctx;

  result = Curl_rand(ರಿಕ್ತ, dest, destlen);
  ಯದಿ(result) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    ಶೂನ್ಯ *user_data)
{
  CURLcode result;
  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)user_data;

  result = Curl_rand(ರಿಕ್ತ, cid->data, cidlen);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(ರಿಕ್ತ, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;

  ಪ್ರತ್ಯಾಗಮ 0;
}

#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  ಸಂರಚನಾ ngtcp2_stateless_reset_token *token, size_t cidlen, ಶೂನ್ಯ *user_data)
{
  CURLcode result;
  (ಶೂನ್ಯ)tconn;
  (ಶೂನ್ಯ)user_data;

  result = Curl_rand(ರಿಕ್ತ, cid->data, cidlen);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(ರಿಕ್ತ, token->data, ಪರಿಮಾಣ(token->data));
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;

  ಪ್ರತ್ಯಾಗಮ 0;
}
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_recv_rx_ಕೀಲಿ(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          ಶೂನ್ಯ *user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  (ಶೂನ್ಯ)tconn;

  ಯದಿ(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    ಪ್ರತ್ಯಾಗಮ 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(data);
  ಯದಿ(ctx && data && !ctx->h3conn && ctx->init_h3_conn_cb) {
    ಯದಿ(ctx->init_h3_conn_cb(cf, data, ctx))
      ಪ್ರತ್ಯಾಗಮ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
ಸ್ಥಿರ ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  ರಿಕ್ತ, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_data_cb,
  cb_ngtcp2_handshake_completed,
  ರಿಕ್ತ, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_mask_cb,
  cb_recv_stream_data,
  cb_acked_stream_data_offset,
  ರಿಕ್ತ, /* stream_open */
  cb_stream_close,
  ರಿಕ್ತ, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  ರಿಕ್ತ, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  ರಿಕ್ತ, /* remove_connection_id */
  ngtcp2_crypto_update_ಕೀಲಿ_cb, /* update_key */
  ರಿಕ್ತ, /* path_validation */
  ರಿಕ್ತ, /* select_preferred_addr */
  cb_stream_reset,
  ರಿಕ್ತ, /* extend_max_remote_streams_bidi */
  ರಿಕ್ತ, /* extend_max_remote_streams_uni */
  cb_extend_max_stream_data,
  ರಿಕ್ತ, /* dcid_status */
  ರಿಕ್ತ, /* handshake_confirmed */
  ರಿಕ್ತ, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  ರಿಕ್ತ, /* recv_datagram */
  ರಿಕ್ತ, /* ack_datagram */
  ರಿಕ್ತ, /* lost_datagram */
  ngtcp2_crypto_get_path_challenge_data_cb,
  cb_stream_stop_sending,
  ರಿಕ್ತ, /* version_negotiation */
  cb_recv_rx_ಕೀಲಿ,
  ರಿಕ್ತ, /* recv_tx_key */
  ರಿಕ್ತ, /* early_data_rejected */
#ifdef NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  ರಿಕ್ತ, /* begin_path_validation */
#endif
#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  ರಿಕ್ತ, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  ರಿಕ್ತ, /* dcid_status2 */
  ngtcp2_crypto_get_path_challenge_data2_cb, /* get_path_challenge_data2 */
#endif
#ifdef NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  ರಿಕ್ತ, /* recv_stop_sending */
#endif
#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_stream_close2, /* is called instead of cb_stream_close when set */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ತೆಗೆ
#endif

#ಯದಿ defined(_MSC_VER) && defined(_DLL)
#pragma warning(ತೆಗೆ)
#endif

#ifdef USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx;
  ಸಂರಚನಾ Curl_easy *data;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_data(ssl);
  cf = cref ? cref->user_data : ರಿಕ್ತ;
  ctx = cf ? cf->ctx : ರಿಕ್ತ;
  data = cf ? CF_DATA_CURRENT(cf) : ರಿಕ್ತ;
  ಯದಿ(cf && data && ctx) {
    unsigned ವರ್ಣ *quic_tp = ರಿಕ್ತ;
    size_t quic_tp_len = 0;
#ifdef HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     ಪರಿಮಾಣ(tpbuf));
    ಯದಿ(tplen < 0)
      CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((ಪೂರ್ಣಾಙ್ಕ)tplen));
    ಅನ್ಯಥಾ {
      quic_tp = (unsigned ವರ್ಣ *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#endif
    Curl_ossl_add_session(cf, data, &ctx->tls.ossl, ctx->ssl_peer.scache_ಕೀಲಿ,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, ರಿಕ್ತ);
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}
#endif /* USE_OPENSSL */

#ifdef USE_GNUTLS

#ifdef CURLVERBOSE
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *gtls_hs_ಸಂದೇಶ_name(ಪೂರ್ಣಾಙ್ಕ mtype)
{
  ವಿಕಲ್ಪ(mtype) {
  ಪಕ್ಷ 1:
    ಪ್ರತ್ಯಾಗಮ "ClientHello";
  ಪಕ್ಷ 2:
    ಪ್ರತ್ಯಾಗಮ "ServerHello";
  ಪಕ್ಷ 4:
    ಪ್ರತ್ಯಾಗಮ "SessionTicket";
  ಪಕ್ಷ 8:
    ಪ್ರತ್ಯಾಗಮ "EncryptedExtensions";
  ಪಕ್ಷ 11:
    ಪ್ರತ್ಯಾಗಮ "Certificate";
  ಪಕ್ಷ 13:
    ಪ್ರತ್ಯಾಗಮ "CertificateRequest";
  ಪಕ್ಷ 15:
    ಪ್ರತ್ಯಾಗಮ "CertificateVerify";
  ಪಕ್ಷ 20:
    ಪ್ರತ್ಯಾಗಮ "Finished";
  ಪಕ್ಷ 24:
    ಪ್ರತ್ಯಾಗಮ "KeyUpdate";
  ಪಕ್ಷ 254:
    ಪ್ರತ್ಯಾಗಮ "MessageHash";
  }
  ಪ್ರತ್ಯಾಗಮ "Unknown";
}
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ quic_gtls_handshake_cb(gnutls_session_t session, unsigned ಪೂರ್ಣಾಙ್ಕ htype,
                                  unsigned when, unsigned ಪೂರ್ಣಾಙ್ಕ incoming,
                                  ಧ್ರುವ gnutls_datum_t *ಸಂದೇಶ)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  ಸಂರಚನಾ Curl_cfilter *cf = conn_ref ? conn_ref->user_data : ರಿಕ್ತ;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ರಿಕ್ತ;

  (ಶೂನ್ಯ)ಸಂದೇಶ;
  (ಶೂನ್ಯ)incoming;
  ಯದಿ(when && cf && ctx) { /* after message has been processed */
    ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    ಯದಿ(!data)
      ಪ್ರತ್ಯಾಗಮ 0;
    CURL_TRC_CF(data, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_ಸಂದೇಶ_name(htype), htype);
    ವಿಕಲ್ಪ(htype) {
    ಪಕ್ಷ GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned ವರ್ಣ *quic_tp = ರಿಕ್ತ;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       ಪರಿಮಾಣ(tpbuf));
      ಯದಿ(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((ಪೂರ್ಣಾಙ್ಕ)tplen));
      ಅನ್ಯಥಾ {
        quic_tp = (unsigned ವರ್ಣ *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (ಶೂನ್ಯ)Curl_gtls_cache_session(cf, data, ctx->ssl_peer.scache_ಕೀಲಿ,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    ರಿಕ್ತ);
      ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}
#endif /* USE_GNUTLS */

#ifdef USE_WOLFSSL
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolfSSL_get_app_data(ssl);
  ಸಂರಚನಾ Curl_cfilter *cf = conn_ref ? conn_ref->user_data : ರಿಕ್ತ;

  DEBUGASSERT(cf);
  ಯದಿ(cf && session) {
    ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
    ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    ಯದಿ(data && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned ವರ್ಣ *quic_tp = ರಿಕ್ತ;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       ಪರಿಮಾಣ(tpbuf));
      ಯದಿ(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((ಪೂರ್ಣಾಙ್ಕ)tplen));
      ಅನ್ಯಥಾ {
        quic_tp = (unsigned ವರ್ಣ *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (ಶೂನ್ಯ)Curl_wssl_cache_session(cf, data, ctx->ssl_peer.scache_ಕೀಲಿ,
                                    session, wolfSSL_version(ssl),
                                    "h3", quic_tp, quic_tp_len, ರಿಕ್ತ);
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}
#endif /* USE_WOLFSSL */

ಸ್ಥಿರ CURLcode cf_ngtcp2_tls_ctx_setup(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಶೂನ್ಯ *user_data)
{
  ಸಂರಚನಾ curl_tls_ctx *ctx = user_data;

#ifdef USE_OPENSSL
#ಯದಿ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  ಯದಿ(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(data, "ngtcp2_crypto_boringssl_configure_client_context failed");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
#ನೋಚೇದ್ಯದಿ defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#ಅನ್ಯಥಾ
  ಯದಿ(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_quictls_configure_client_context failed");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
#endif /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  ಯದಿ(Curl_ssl_scache_use(cf, data)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_set_session_cache_mode(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
  ಯದಿ(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(data, "ngtcp2_crypto_gnutls_configure_client_session failed");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  ಯದಿ(Curl_ssl_scache_use(cf, data)) {
    gnutls_handshake_set_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
  ಯದಿ(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  ಯದಿ(Curl_ssl_scache_use(cf, data)) {
    /* Register to get notified when a new session is received */
    wolfSSL_CTX_sess_set_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_ngtcp2_on_session_reuse(ಸಂರಚನಾ Curl_cfilter *cf,
                                           ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ alpn_spec *alpns,
                                           ಸಂರಚನಾ Curl_ssl_session *scs,
                                           bool *do_early_data)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;
#ಯದಿ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#endif
#ifdef USE_GNUTLS
  ctx->earlydata_max =
    gnutls_record_get_max_early_data_size(ctx->tls.gtls.session);
#endif
#ifdef USE_WOLFSSL
#ifdef WOLFSSL_EARLY_DATA
  ctx->earlydata_max = scs->earlydata_max;
#ಅನ್ಯಥಾ
  ctx->earlydata_max = 0;
#endif /* WOLFSSL_EARLY_DATA */
#endif
#ಯದಿ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  ಯದಿ(!ctx->earlydata_max) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  ಅನ್ಯಥಾ ಯದಿ(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session from different ALPN, no early data");
  }
  ಅನ್ಯಥಾ ಯದಿ(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(data, cf, "no 0RTT transport parameters, no early data");
  }
  ಅನ್ಯಥಾ {
    ಪೂರ್ಣಾಙ್ಕ rv;
    rv = ngtcp2_conn_decode_and_set_0rtt_transport_params(
      ctx->qconn, (ಧ್ರುವ uint8_t *)scs->quic_tp, scs->quic_tp_len);
    ಯದಿ(rv)
      CURL_TRC_CF(data, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    ಅನ್ಯಥಾ ಯದಿ(ctx->init_h3_conn_cb) {
      infof(data, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      result = ctx->init_h3_conn_cb(cf, data, ctx);
      ಯದಿ(!result) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_data = TRUE;
      }
    }
    ಅನ್ಯಥಾ { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_data = TRUE;
    }
  }
#ಅನ್ಯಥಾ /* not supported in the TLS backend */
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)ctx;
  (ಶೂನ್ಯ)scs;
  (ಶೂನ್ಯ)alpns;
#endif
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Might be called twice for happy eyeballs.
 */
ಸ್ಥಿರ CURLcode cf_connect_start(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ cf_ngtcp2_io_ctx *pktx)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಙ್ಕ rc;
  ಪೂರ್ಣಾಙ್ಕ rv;
  CURLcode result;
  ಧ್ರುವ ಸಂರಚನಾ Curl_sockaddr_ex *sockaddr = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ qfd;
  ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->dcid.data, NGTCP2_MAX_CIDLEN);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->scid.data, NGTCP2_MAX_CIDLEN);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  (ಶೂನ್ಯ)Curl_qlogdir(data, ctx->scid.data, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, data, pktx);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Query socket and remote address from sub-chain */
  ಯದಿ(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, ರಿಕ್ತ)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  ಯದಿ(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = ಪರಿಮಾಣ(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (ಸಂರಚನಾ sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    ಯದಿ(rv == -1)
      ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (ಸಂರಚನಾ sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_path.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  ಅನ್ಯಥಾ {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ಧ್ರುವ ಸಂರಚನಾ Curl_sockaddr_ex *remote = ರಿಕ್ತ;
    ಯದಿ(cf->next->cft->query(cf->next, data, CF_QUERY_REMOTE_ADDR, ರಿಕ್ತ,
                            CURL_UNCONST(&remote)))
      ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;
    ಯದಿ(!remote)
      ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, ಪರಿಮಾಣ(ctx->q.local_addr));
    ವಿಕಲ್ಪ(remote->family) {
    ಪಕ್ಷ AF_INET:
      ((ಸಂರಚನಾ sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in);
      ಭಙ್ಗ;
#ifdef USE_IPV6
    ಪಕ್ಷ AF_INET6:
      ((ಸಂರಚನಾ sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in6);
      ಭಙ್ಗ;
#endif
    ಸ್ವಭಾವ:
      ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (ಸಂರಚನಾ sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_path.remote,
                     &remote->curl_sa_addr,
                     (socklen_t)remote->addrlen);
  }

  rc = ngtcp2_conn_client_new(&ctx->qconn, &ctx->dcid, &ctx->scid,
                              &ctx->connected_path,
                              NGTCP2_PROTO_VER_V1, &ng_callbacks,
                              &ctx->settings, &ctx->transport_params,
                              Curl_ngtcp2_mem(), cf);
  ಯದಿ(rc)
    ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_data = cf;

  result = Curl_vquic_tls_init(&ctx->tls, cf, data,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_setup, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

#ಯದಿ defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  ಯದಿ(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_ctx_new failed");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  ಯದಿ(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_configure_client_session failed");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
#ನೋಚೇದ್ಯದಿ defined(USE_OPENSSL)
  SSL_set_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#ಅನ್ಯಥಾ
#error "ngtcp2 TLS backend not defined"
#endif

#ಯದಿ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  ಯದಿ(ctx->use_earlydata &&
     !SSL_set_quic_tls_early_data_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(data, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#endif
  ngtcp2_ccerr_default(&ctx->last_error);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    bool *done)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ cf_call_data save;
  ಸಂರಚನಾ cf_ngtcp2_io_ctx pktx;

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  /* Connect the sub-chain */
  ಯದಿ(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ಯದಿ(result || !*done)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  *done = FALSE;

  ಯದಿ(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  CF_DATA_SAVE(save, cf, data);

  ಯದಿ(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(data);
    result = cf_connect_start(cf, data, &pktx);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    ಯದಿ(cf->connected) {
      *done = TRUE;
      ಗಚ್ಛ out;
    }
    result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
    /* we do not expect to be able to recv anything yet */
    ಗಚ್ಛ out;
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ಯದಿ(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    result = ctx->tls_vrfy_result;
    ಯದಿ(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      cf->connected = TRUE;
      *done = TRUE;
    }
  }

out:
  ಯದಿ(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
  ಯದಿ(ctx->qconn &&
     ((result == CURLE_RECV_ERROR) || (result == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_in_draining_period(ctx->qconn)) {
    ಧ್ರುವ ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    result = CURLE_COULDNT_CONNECT;
    ಯದಿ(cerr) {
      CURL_TRC_CF(data, cf, "connect error, type=%d, code=%" PRIu64,
                  (ಪೂರ್ಣಾಙ್ಕ)cerr->type, cerr->error_code);
      ವಿಕಲ್ಪ(cerr->type) {
      ಪಕ್ಷ NGTCP2_CCERR_TYPE_VERSION_NEGOTIATION:
        CURL_TRC_CF(data, cf, "error in version negotiation");
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        ಯದಿ(cerr->error_code >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(data, cf, "crypto error, tls alert=%u",
                      (unsigned ಪೂರ್ಣಾಙ್ಕ)(cerr->error_code & 0xffU));
        }
        ಅನ್ಯಥಾ ಯದಿ(cerr->error_code == NGTCP2_CONNECTION_REFUSED) {
          CURL_TRC_CF(data, cf, "connection refused by server");
          /* When a QUIC server instance is shutting down, it may send us a
           * CONNECTION_CLOSE with this code right away. We want
           * to keep on trying in this case. */
          result = CURLE_WEIRD_SERVER_REPLY;
        }
      }
    }
  }

#ifdef CURLVERBOSE
  ಯದಿ(result) {
    ಯದಿ(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      ಸಂರಚನಾ ip_quadruple ip;

      ಯದಿ(!Curl_cf_socket_peek(cf->next, data, ರಿಕ್ತ, ರಿಕ್ತ, &ip))
        infof(data, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    }
  }
#endif
  ಯದಿ(!result && ctx->qconn) {
    result = Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx);
  }
  ಯದಿ(result || *done)
    CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (ಪೂರ್ಣಾಙ್ಕ)result, *done);
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  ಸಂರಚನಾ cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  ಯದಿ(cf->shutdown || !ctx->qconn) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  ಯದಿ(!cf->next) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);
  *done = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  ಯದಿ(!ctx->shutdown_started) {
    ವರ್ಣ buffer[NGTCP2_MAX_UDP_PAYLOAD_SIZE];
    ngtcp2_ssize nwritten;

    ಯದಿ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
      CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
      result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
      ಯದಿ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
        CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
        result = CURLE_OK;
        ಗಚ್ಛ out;
      }
      ಅನ್ಯಥಾ ಯದಿ(result) {
        CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                    (ಪೂರ್ಣಾಙ್ಕ)result);
        *done = TRUE;
        ಗಚ್ಛ out;
      }
    }

    DEBUGASSERT(Curl_bufq_is_empty(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, ರಿಕ್ತ, /* path */
      ರಿಕ್ತ, /* pkt_info */
      (uint8_t *)buffer, ಪರಿಮಾಣ(buffer),
      &ctx->last_error, pktx.ts);
    CURL_TRC_CF(data, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (ಪೂರ್ಣಾಙ್ಕ)ctx->last_error.type,
                ctx->last_error.error_code, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    ಯದಿ(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      result = Curl_bufq_write(&ctx->q.sendbuf, (ಧ್ರುವ unsigned ವರ್ಣ *)buffer,
                               (size_t)nwritten, &n);
      ಯದಿ(result) {
        CURL_TRC_CF(data, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (ಪೂರ್ಣಾಙ್ಕ)result);
        ಗಚ್ಛ out;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.split_len = 0;
    }
  }

  ಯದಿ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing egress");
    result = Curl_vquic_flush(cf, data, &ctx->q);
    ಯದಿ(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
      result = CURLE_OK;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(result) {
      CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                  (ಪೂರ್ಣಾಙ್ಕ)result);
      *done = TRUE;
      ಗಚ್ಛ out;
    }
  }

  ಯದಿ(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
    result = CURLE_OK;
  }
out:
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_cf_ngtcp2_cmn_conn_close(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  bool done;
  Curl_cf_ngtcp2_cmn_shutdown(cf, data, &done);
}

ಸ್ಥಿರ bool cf_ngtcp2_err_is_fatal(ಪೂರ್ಣಾಙ್ಕ code)
{
  ಪ್ರತ್ಯಾಗಮ (NGTCP2_ERR_FATAL >= code) ||
         (NGTCP2_ERR_DROP_CONN == code) ||
         (NGTCP2_ERR_IDLE_CLOSE == code);
}

ಶೂನ್ಯ Curl_cf_ngtcp2_cmn_err_set(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ code)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಯದಿ(!ctx->last_error.error_code) {
    ಯದಿ(NGTCP2_ERR_CRYPTO == code) {
      ngtcp2_ccerr_set_tls_alert(&ctx->last_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 ರಿಕ್ತ, 0);
    }
    ಅನ್ಯಥಾ {
      ngtcp2_ccerr_set_liberr(&ctx->last_error, code, ರಿಕ್ತ, 0);
    }
  }
  ಯದಿ(cf_ngtcp2_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

ಶೂನ್ಯ Curl_cf_ngtcp2_io_ctx_init(ಸಂರಚನಾ cf_ngtcp2_io_ctx *io_ctx,
                                ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->data = data;
  io_ctx->now = *Curl_pgrs_now(data);
  ngtcp2_path_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_set_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

ಶೂನ್ಯ Curl_cf_ngtcp2_io_ctx_update_time(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ cf_ngtcp2_io_ctx *io_ctx,
                                       ಸಂರಚನಾ Curl_cfilter *cf)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(data);
  Curl_vquic_ctx_update_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#ಯದಿ NGTCP2_VERSION_ಅಂಕೆ < 0x011100
ಸಂರಚನಾ cf_ngtcp2_sfind_ctx {
  int64_t stream_id;
  ಸಂರಚನಾ h3_stream_ctx *stream;
  uint32_t mid;
};

ಸ್ಥಿರ bool cf_ngtcp2_sfind(uint32_t mid, ಶೂನ್ಯ *ಮೌಲ್ಯ, ಶೂನ್ಯ *user_data)
{
  ಸಂರಚನಾ cf_ngtcp2_sfind_ctx *fctx = user_data;
  ಸಂರಚನಾ h3_stream_ctx *stream = ಮೌಲ್ಯ;

  ಯದಿ(fctx->stream_id == stream->id) {
    fctx->mid = mid;
    fctx->stream = stream;
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ TRUE; /* continue */
}

ಸ್ಥಿರ ಸಂರಚನಾ h3_stream_ctx *cf_ngtcp2_get_stream(ಸಂರಚನಾ cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  ಸಂರಚನಾ cf_ngtcp2_sfind_ctx fctx;
  fctx.stream_id = stream_id;
  fctx.stream = ರಿಕ್ತ;
  Curl_u32_ptrset_visit(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  ಪ್ರತ್ಯಾಗಮ fctx.stream;
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ ಸಂರಚನಾ h3_stream_ctx *cf_ngtcp2_get_stream(ಸಂರಚನಾ cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  ಪ್ರತ್ಯಾಗಮ ngtcp2_conn_get_stream_user_data(ctx->qconn, stream_id);
}
#endif

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
ಸ್ಥಿರ CURLcode read_pkt_to_send(ಶೂನ್ಯ *userp,
                                 unsigned ವರ್ಣ *buf, size_t buflen,
                                 size_t *pnread)
{
  ಸಂರಚನಾ cf_ngtcp2_io_ctx *x = userp;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t stream_id;
  ಪೂರ್ಣಾಙ್ಕ fin;
  ssize_t n;

  *pnread = 0;
  veccnt = 0;
  stream_id = -1;
  fin = 0;

  /* ngtcp2 may want to put several frames from different streams into
   * this packet. `NGTCP2_WRITE_STREAM_FLAG_MORE` tells it to do so.
   * When `NGTCP2_ERR_WRITE_MORE` is returned, we *need* to make
   * another iteration.
   * When ngtcp2 is happy (because it has no other frame that would fit
   * or it has nothing more to send), it returns the total length
   * of the assembled packet. This may be 0 if there was nothing to send. */
  ಪ್ರತಿ(;;) {

    ಯದಿ(ctx->h3conn && ngtcp2_conn_get_max_data_ಎಡ(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_stream(ctx->h3conn, &stream_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      ಯದಿ(veccnt < 0) {
        failf(x->data, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((ಪೂರ್ಣಾಙ್ಕ)veccnt));
        Curl_cf_ngtcp2_h3_err_set(x->cf, x->data, (ಪೂರ್ಣಾಙ್ಕ)veccnt);
        ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_STREAM_FLAG_MORE |
            (fin ? NGTCP2_WRITE_STREAM_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_stream(ctx->qconn, &x->ps.path,
                                  ರಿಕ್ತ, buf, buflen,
                                  &ndatalen, flags, stream_id,
                                  (ಧ್ರುವ ngtcp2_vec *)vec, veccnt, x->ts);
    ಯದಿ(n == 0) {
      /* nothing to send */
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    }
    ಅನ್ಯಥಾ ಯದಿ(n < 0) {
      ವಿಕಲ್ಪ(n) {
      ಪಕ್ಷ NGTCP2_ERR_STREAM_DATA_BLOCKED: {
        ಸಂರಚನಾ h3_stream_ctx *stream;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_block_stream(ctx->h3conn, stream_id);
        CURL_TRC_CF(x->data, x->cf, "[%" PRId64 "] block quic flow",
                    stream_id);
        stream = cf_ngtcp2_get_stream(ctx, stream_id);
        ಯದಿ(stream) /* it might be not one of our h3 streams? */
          stream->quic_flow_blocked = TRUE;
        n = 0;
        ಭಙ್ಗ;
      }
      ಪಕ್ಷ NGTCP2_ERR_STREAM_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_stream_write(ctx->h3conn, stream_id);
        n = 0;
        ಭಙ್ಗ;
      ಪಕ್ಷ NGTCP2_ERR_WRITE_MORE:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        DEBUGASSERT(ndatalen == -1);
        failf(x->data, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((ಪೂರ್ಣಾಙ್ಕ)n));
        Curl_cf_ngtcp2_cmn_err_set(x->cf, x->data, (ಪೂರ್ಣಾಙ್ಕ)n);
        ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
      }
    }

    ಯದಿ(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      ಪೂರ್ಣಾಙ್ಕ rv = nghttp3_conn_add_write_offset(ctx->h3conn, stream_id, ndatalen);
      ಯದಿ(rv) {
        failf(x->data, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
      }
    }

    ಯದಿ(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_progress_egress(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ cf_ngtcp2_io_ctx *pktx)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_size, path_max_payload_size;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_quantum;
  CURLcode result;
  ಸಂರಚನಾ cf_ngtcp2_io_ctx local_pktx;

  ಯದಿ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  ಅನ್ಯಥಾ {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  ಯದಿ(result) {
    ಯದಿ(result == CURLE_AGAIN) {
      Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಪ್ರತ್ಯಾಗಮ result;
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
  max_payload_size = ngtcp2_conn_get_max_tx_udp_payload_size(ctx->qconn);
  path_max_payload_size =
    ngtcp2_conn_get_path_max_tx_udp_payload_size(ctx->qconn);
  send_quantum = ngtcp2_conn_get_send_quantum(ctx->qconn);
  CURL_TRC_CF(data, cf, "egress, collect and send packets, quantum=%zu",
              send_quantum);
  ಪ್ರತಿ(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_size,
                            read_pkt_to_send, pktx, &nread);
    ಯದಿ(result == CURLE_AGAIN)
      ಭಙ್ಗ;
    ಅನ್ಯಥಾ ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ಅನ್ಯಥಾ {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      ಯದಿ((buflen >= send_quantum) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_size))
        ಭಙ್ಗ;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      ಯದಿ(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      ಅನ್ಯಥಾ ಯದಿ(nread > gsolen ||
              (gsolen > path_max_payload_size && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        result = Curl_vquic_send_tail_split(cf, data, &ctx->q,
                                            gsolen, nread, nread);
        ಯದಿ(result) {
          ಯದಿ(result == CURLE_AGAIN) {
            Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
            ಪ್ರತ್ಯಾಗಮ CURLE_OK;
          }
          ಪ್ರತ್ಯಾಗಮ result;
        }
        pktcnt = 0;
      }
      ಅನ್ಯಥಾ ಯದಿ(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        ಭಙ್ಗ;
      }
    }
  }

  ಯದಿ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(data, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
    ಯದಿ(result) {
      ಯದಿ(result == CURLE_AGAIN) {
        Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      ಪ್ರತ್ಯಾಗಮ result;
    }
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_conn_update_pkt_tx_time(ctx->qconn, pktx->ts);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸಂರಚನಾ cf_ngtcp2_recv_ctx {
  ಸಂರಚನಾ cf_ngtcp2_io_ctx *pktx;
  size_t pkt_ಎಣಿಕೆ;
};

ಸ್ಥಿರ CURLcode cf_ngtcp2_recv_pkts(ಧ್ರುವ unsigned ವರ್ಣ *buf, size_t buflen,
                                    size_t gso_size,
                                    ಸಂರಚನಾ sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ cf_ngtcp2_recv_ctx *rctx = userp;
  ಸಂರಚನಾ cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_path path;
  size_t offset, pktlen;
  ಪೂರ್ಣಾಙ್ಕ rv;

  ಯದಿ(!rctx->pkt_ಎಣಿಕೆ) {
    Curl_cf_ngtcp2_io_ctx_update_time(pktx->data, pktx, pktx->cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  ಯದಿ(ecn)
    CURL_TRC_CF(pktx->data, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_size, (unsigned)ecn);
  ngtcp2_addr_init(&path.local, (ಸಂರಚನಾ sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&path.remote, (ಸಂರಚನಾ sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  ಪ್ರತಿ(offset = 0; offset < buflen; offset += gso_size) {
    rctx->pkt_ಎಣಿಕೆ++;
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &path, &pi,
                              buf + offset, pktlen, pktx->ts);
    ಯದಿ(rv) {
      CURL_TRC_CF(pktx->data, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_set(pktx->cf, pktx->data, rv);

      ಯದಿ(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_progress_ingress(ಸಂರಚನಾ Curl_cfilter *cf,
                                         ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ cf_ngtcp2_io_ctx *pktx)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ cf_ngtcp2_io_ctx local_pktx;
  ಸಂರಚನಾ cf_ngtcp2_recv_ctx rctx;
  CURLcode result = CURLE_OK;

  ಯದಿ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }

  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  rctx.pktx = pktx;
  rctx.pkt_ಎಣಿಕೆ = 0;

  ಯದಿ(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(data, cf, "progress_ingress(socket)");
    ಪ್ರತ್ಯಾಗಮ Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  ಅನ್ಯಥಾ {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    unsigned ವರ್ಣ *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_SIZE;
    size_t pkt_limit = QUIC_TUNNEL_INGRESS_PKT_LIMIT;
    size_t nread;
    ಸಂರಚನಾ sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(data, cf, "progress_ingress(sub-filters)");
    ಯದಿ(ctx->qconn) {
      size_t max_path_payload;
      max_path_payload =
        ngtcp2_conn_get_path_max_tx_udp_payload_size(ctx->qconn);
      ಯದಿ(max_path_payload > max_udp_payload)
        max_udp_payload = max_path_payload;
    }

    ಯದಿ(ctx->tunnel_inbuf_len < max_udp_payload) {
      unsigned ವರ್ಣ *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      ಯದಿ(!newbuf)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    ಯಾವತ್(pkt_limit--) {
      result = Curl_conn_cf_recv(cf->next, data, (ವರ್ಣ *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      ಯದಿ(result == CURLE_AGAIN) {
        /* no more data available at the moment */
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      ಯದಿ(result) {
        CURL_TRC_CF(data, cf, "ingress, recv from tunnel failed: %d",
                    (ಪೂರ್ಣಾಙ್ಕ)result);
        ಪ್ರತ್ಯಾಗಮ result;
      }
      ಯದಿ(nread == 0) {
        /* tunnel closed */
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }

      memcpy(&remote_addr, ctx->connected_path.remote.addr,
             ctx->connected_path.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_path.remote.addrlen;
      result = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      ಯದಿ(!ctx->q.got_first_byte) {
        ctx->q.got_first_byte = TRUE;
        ctx->q.first_byte_at = ctx->q.last_op;
      }
      ctx->q.last_io = ctx->q.last_op;
    }
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_set_expiry(ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ cf_ngtcp2_io_ctx *pktx)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  ಯದಿ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  ಅನ್ಯಥಾ {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  ಯದಿ(expiry != UINT64_MAX) {
    ಯದಿ(expiry <= pktx->ts) {
      CURLcode result;
      ಪೂರ್ಣಾಙ್ಕ rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      ಯದಿ(rv) {
        failf(data, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_set(cf, data, rv);
        ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
      }
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, pktx);
      ಯದಿ(!result)
        result = Curl_cf_ngtcp2_progress_egress(cf, data, pktx);
      ಯದಿ(result) {
        /* a verify failure during ingress must win over generic errors */
        ಯದಿ(ctx->tls_vrfy_result)
          result = ctx->tls_vrfy_result;
        ಪ್ರತ್ಯಾಗಮ result;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    ಯದಿ(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      ಯದಿ(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_set(data, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ngtcp2_setup_keep_alive(ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಧ್ರುವ ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  ಯದಿ(!ctx->qconn)
    ಪ್ರತ್ಯಾಗಮ;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  ಯದಿ(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no peer idle timeout, unset keep-alive");
  }
  ಅನ್ಯಥಾ ಯದಿ(!Curl_u32_ptrset_ಎಣಿಕೆ(&ctx->streams)) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no active streams, unset keep-alive");
  }
  ಅನ್ಯಥಾ {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(data, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_stream_setup(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ h3_stream_ctx **pstream)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  *pstream = ರಿಕ್ತ;
  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(stream) {
    *pstream = stream;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  stream = curlx_calloc(1, ಪರಿಮಾಣ(*stream));
  ಯದಿ(!stream)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  stream->xfer_id = data->id;
  stream->mid = data->mid;
  stream->rx_offset = 0;
  stream->rx_offset_max = H3_STREAM_WINDOW_SIZE_INITIAL;
  stream->tx_in_flight_ideal = H3_STREAM_SEND_BUF_INITIAL;

  /* on send, we control how much we put into the buffer */
  Curl_bufq_initp(&stream->sendbuf, &ctx->stream_bufcp,
                  H3_STREAM_SEND_CHUNKS, BUFQ_OPT_NONE);
  stream->tx_in_flight_size = 0;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_ಸಾಲು_LEN);

  ಯದಿ(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    Curl_cf_ngtcp2_h3_stream_ctx_free(stream);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಯದಿ(Curl_u32_ptrset_ಎಣಿಕೆ(&ctx->streams) == 1)
    cf_ngtcp2_setup_keep_alive(cf, data);

  *pstream = stream;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_cf_ngtcp2_h3_stream_close(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ h3_stream_ctx *stream)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(data);
  DEBUGASSERT(stream);
  ಯದಿ(!stream->closed && ctx->qconn && ctx->h3conn) {
    CURLcode result;

    nghttp3_conn_set_stream_user_data(ctx->h3conn, stream->id, ರಿಕ್ತ);
    ngtcp2_conn_set_stream_user_data(ctx->qconn, stream->id, ರಿಕ್ತ);
    stream->closed = TRUE;
    (ಶೂನ್ಯ)ngtcp2_conn_shutdown_stream(ctx->qconn, 0, stream->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    result = Curl_cf_ngtcp2_progress_egress(cf, data, ರಿಕ್ತ);
    ಯದಿ(result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cancel stream -> %d",
                  stream->id, (ಪೂರ್ಣಾಙ್ಕ)result);
  }
}

ಶೂನ್ಯ Curl_cf_ngtcp2_h3_stream_done(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  (ಶೂನ್ಯ)cf;
  ಯದಿ(stream) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] easy handle is done", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
    ಯದಿ(!Curl_u32_ptrset_ಎಣಿಕೆ(&ctx->streams))
      cf_ngtcp2_setup_keep_alive(cf, data);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      bool *ಒಳಹರಿವು_pending)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ಧ್ರುವ ngtcp2_transport_params *rp;
  ಸಂರಚನಾ cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  *ಒಳಹರಿವು_pending = FALSE;
  ಯದಿ(!ctx->qconn || ctx->shutdown_started)
    ಗಚ್ಛ out;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  ಯದಿ(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->q.last_io);
    ಯದಿ(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      ಯದಿ((uint64_t)idletime_ms > max_idle_ms)
        ಗಚ್ಛ out;
    }
  }

  ಯದಿ(!cf->next || !cf->next->cft->is_alive(cf->next, data, ಒಳಹರಿವು_pending))
    ಗಚ್ಛ out;

  alive = TRUE;
  ಯದಿ(*ಒಳಹರಿವು_pending) {
    CURLcode result;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *ಒಳಹರಿವು_pending = FALSE;
    result = Curl_cf_ngtcp2_progress_ingress(cf, data, ರಿಕ್ತ);
    CURL_TRC_CF(data, cf, "is_alive, progress ingress -> %d", (ಪೂರ್ಣಾಙ್ಕ)result);
    alive = result ? FALSE : TRUE;
  }

out:
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(ಸಂರಚನಾ cf_ngtcp2_ctx *ctx,
                                      ಸಂರಚನಾ Curl_easy *data)
{
  int64_t ctrl_stream_id, qpack_enc_stream_id, qpack_dec_stream_id;
  ಪೂರ್ಣಾಙ್ಕ rc;

  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &ctrl_stream_id, ರಿಕ್ತ);
  ಯದಿ(rc) {
    failf(data, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_stream(ctx->h3conn, ctrl_stream_id);
  ಯದಿ(rc) {
    failf(data, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_enc_stream_id, ರಿಕ್ತ);
  ಯದಿ(rc) {
    failf(data, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_dec_stream_id, ರಿಕ್ತ);
  ಯದಿ(rc) {
    failf(data, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_stream_id,
                                       qpack_dec_stream_id);
  ಯದಿ(rc) {
    failf(data, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;

  ವಿಕಲ್ಪ(query) {
  ಪಕ್ಷ CF_QUERY_CONNECT_REPLY_MS:
    ಯದಿ((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (ಪೂರ್ಣಾಙ್ಕ)ms : INT_MAX;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_QUERY_REALLY_CONNECTED:
    ಯದಿ(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_first_byte;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  ವಿಕಲ್ಪ(event) {
  ಪಕ್ಷ CF_CTRL_REPORT_STATS:
    ಯದಿ(cf->connected && !ctx->stats_reported) {
      ಯದಿ((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        ctx->stats_reported = TRUE;
      }
      ಅನ್ಯಥಾ ಯದಿ(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        ಯದಿ(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
