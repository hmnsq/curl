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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#അന്തര്ഭാവ <ngtcp2/ngtcp2.h>

#ifdef USE_OPENSSL
#അന്തര്ഭാവ <openssl/err.h>
#എങ്കിൽ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#അന്തര്ഭാവ <ngtcp2/ngtcp2_crypto_boringssl.h>
#അല്ലെങ്കിലെങ്കിൽ defined(OPENSSL_QUIC_API2)
#അന്തര്ഭാവ <ngtcp2/ngtcp2_crypto_ossl.h>
#അല്ലെങ്കിൽ
#അന്തര്ഭാവ <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#അന്തര്ഭാവ "vtls/openssl.h"
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
#അന്തര്ഭാവ <ngtcp2/ngtcp2_crypto_gnutls.h>
#അന്തര്ഭാവ "vtls/gtls.h"
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
#അന്തര്ഭാവ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#അന്തര്ഭാവ "vtls/wolfssl.h"
#endif

#അന്തര്ഭാവ <nghttp3/nghttp3.h>

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "u32_ptrset.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "cf-socket.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "curlx/win32-fopen.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "http1.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "sockaddr.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "vdns/cf-dns.h"
#അന്തര്ഭാവ "vquic/vquic.h"
#അന്തര്ഭാവ "vquic/vquic_int.h"
#അന്തര്ഭാവ "vquic/vquic-tls.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "vtls/vtls_scache.h"
#അന്തര്ഭാവ "vquic/cf-ngtcp2-cmn.h"

/*
 * Store ngtcp2 version info in this buffer.
 */
ശൂന്യം Curl_ngtcp2_ver(അക്ഷരം *p, size_t len)
{
  സ്ഥിരം ngtcp2_info *ng2 = ngtcp2_version(0);
  സ്ഥിരം nghttp3_info *ht3 = nghttp3_version(0);
  (ശൂന്യം)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->version_str, ht3->version_str);
}

ശൂന്യം Curl_cf_ngtcp2_h3_stream_ctx_free(ഘടന h3_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

സ്ഥിതം ശൂന്യം h3_stream_set_free(uint32_t id, ശൂന്യം *stream)
{
  (ശൂന്യം)id;
  DEBUGASSERT(stream);
  Curl_cf_ngtcp2_h3_stream_ctx_free((ഘടന h3_stream_ctx *)stream);
}

സ്ഥിതം bool cf_ngtcp2_h3_err_is_fatal(പൂർണ്ണസംഖ്യ code)
{
  തിരികെ (NGHTTP3_ERR_FATAL >= code) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM == code);
}

ശൂന്യം Curl_cf_ngtcp2_h3_err_set(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ code)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  എങ്കിൽ(!ctx->last_error.error_code) {
    ngtcp2_ccerr_set_application_error(&ctx->last_error,
      nghttp3_err_infer_quic_app_error_code(code), ശൂന്യ, 0);
  }
  എങ്കിൽ(cf_ngtcp2_h3_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

CURLcode Curl_cf_ngtcp2_ctx_init(ഘടന cf_ngtcp2_ctx *ctx,
                                 ഘടന Curl_peer *origin,
                                 ഘടന Curl_peer *peer,
                                 ഘടന ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = ശൂന്യ;
  ctx->tunnel_inbuf_len = 0;
  ctx->version = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->stream_bufcp, H3_STREAM_CHUNK_SIZE,
                  H3_STREAM_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  തിരികെ Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

ശൂന്യം Curl_cf_ngtcp2_ctx_cleanup(ഘടന cf_ngtcp2_ctx *ctx)
{
  എങ്കിൽ(ctx && ctx->initialized) {
    എങ്കിൽ(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    എങ്കിൽ(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = ശൂന്യ;
    }
    എങ്കിൽ(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = ശൂന്യ;
    }
#ifdef OPENSSL_QUIC_API2
    എങ്കിൽ(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = ശൂന്യ;
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

ഘടന Curl_easy *Curl_cf_ngtcp2_get_xfer(ഘടന Curl_cfilter *cf,
                                          ശൂന്യം *stream_user_data,
                                          ഘടന h3_stream_ctx **pstream)
{
  ഘടന h3_stream_ctx *stream = stream_user_data;

  എങ്കിൽ(pstream)
    *pstream = stream;
  എങ്കിൽ(stream && (stream->mid != UINT32_MAX)) {
    ഘടന Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    എങ്കിൽ(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      തിരികെ ശൂന്യ;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    എങ്കിൽ(data && (data->id == stream->xfer_id))
      തിരികെ data;
    DEBUGASSERT(0);
  }
  തിരികെ ശൂന്യ;
}

സ്ഥിതം ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  ഘടന Curl_cfilter *cf = conn_ref->user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  തിരികെ ctx->qconn;
}

#ifdef DEBUG_NGTCP2
സ്ഥിതം ശൂന്യം quic_printf(ശൂന്യം *user_data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  va_list ap;
  (ശൂന്യം)user_data;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#endif

സ്ഥിതം ശൂന്യം qlog_callback(ശൂന്യം *user_data, uint32_t flags,
                          സ്ഥിരം ശൂന്യം *data, size_t datalen)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  (ശൂന്യം)flags;
  എങ്കിൽ(ctx->qlogfd != -1) {
    ssize_t rc = ലിഖ(ctx->qlogfd, data, datalen);
    എങ്കിൽ(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

സ്ഥിതം ശൂന്യം quic_settings(ഘടന cf_ngtcp2_ctx *ctx,
                          ഘടന Curl_easy *data,
                          ഘടന cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#ifdef DEBUG_NGTCP2
  s->log_printf = quic_printf;
#അല്ലെങ്കിൽ
  s->log_printf = ശൂന്യ;
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
  എങ്കിൽ(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#എങ്കിൽ defined(_MSC_VER) && defined(_DLL)
#pragma warning(തള്ളുക)
#pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, ശൂന്യം *user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ശൂന്യ;
  ഘടന Curl_easy *data;

  (ശൂന്യം)tconn;
  DEBUGASSERT(ctx);
  data = CF_DATA_CURRENT(cf);
  DEBUGASSERT(data);
  എങ്കിൽ(!ctx || !data)
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(data);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, data);

  ctx->tls_vrfy_result = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    data, &ctx->ssl_peer);
  എങ്കിൽ(ctx->tls_vrfy_result)
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;

#ifdef CURLVERBOSE
  എങ്കിൽ(Curl_trc_is_verbose(data)) {
    സ്ഥിരം ngtcp2_transport_params *rp;
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
  എങ്കിൽ(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  എങ്കിൽ(ctx->use_earlydata) {
#എങ്കിൽ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    എങ്കിൽ(SSL_get_early_data_status(ctx->tls.ossl.ssl) == SSL_EARLY_DATA_NOT_SENT)
      CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
#endif

#എങ്കിൽ NGTCP2_VERSION_അക്കം >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_data_rejected2(ctx->qconn);
#അല്ലെങ്കിൽ /* older NGTCP2 */
    {
#എങ്കിൽ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      പൂർണ്ണസംഖ്യ ossl_early_status = SSL_get_early_data_status(ctx->tls.ossl.ssl);
      എങ്കിൽ(ossl_early_status == SSL_EARLY_DATA_NOT_SENT)
        CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_status == SSL_EARLY_DATA_ACCEPTED);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
      പൂർണ്ണസംഖ്യ flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_DATA);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
#ifdef WOLFSSL_EARLY_DATA
      ctx->earlydata_accepted =
        (wolfSSL_get_early_data_status(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_DATA_REJECTED);
#അല്ലെങ്കിൽ
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
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_recv_stream_data(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t stream_id, uint64_t offset,
                               സ്ഥിരം uint8_t *buf, size_t buflen,
                               ശൂന്യം *user_data, ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  പൂർണ്ണസംഖ്യ fin = (flags & NGTCP2_STREAM_DATA_FLAG_FIN) ? 1 : 0;
  ഘടന h3_stream_ctx *stream;
  ഘടന Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (ശൂന്യം)offset;
  rc = nghttp3_conn_read_stream(ctx->h3conn, stream_id, buf, buflen, fin);
  എങ്കിൽ(rc < 0) {
    എങ്കിൽ(data && stream) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  stream_id, stream->reset, stream->closed);
    }
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  എങ്കിൽ(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_stream_offset(tconn, stream_id, nconsumed);
    ngtcp2_conn_extend_max_offset(tconn, nconsumed);
    എങ്കിൽ(stream) {
      stream->rx_offset += nconsumed;
      stream->rx_offset_max += nconsumed;
    }
  }
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_acked_stream_data_offset(ngtcp2_conn *tconn, int64_t stream_id,
                                       uint64_t offset, uint64_t datalen,
                                       ശൂന്യം *user_data, ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  പൂർണ്ണസംഖ്യ rv;
  (ശൂന്യം)stream_id;
  (ശൂന്യം)tconn;
  (ശൂന്യം)offset;
  (ശൂന്യം)datalen;
  (ശൂന്യം)stream_user_data;

  rv = nghttp3_conn_add_ack_offset(ctx->h3conn, stream_id, datalen);
  എങ്കിൽ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_stream_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id, uint64_t app_error_code,
                           ശൂന്യം *user_data, ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ശൂന്യ);
  പൂർണ്ണസംഖ്യ rv;

  (ശൂന്യം)tconn;
  /* stream is closed... */
  എങ്കിൽ(!data)
    data = CF_DATA_CURRENT(cf);
  എങ്കിൽ(!data)
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;

  എങ്കിൽ(!(flags & NGTCP2_STREAM_CLOSE_FLAG_APP_ERROR_CODE_SET)) {
    app_error_code = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, app_error_code, rv);
  എങ്കിൽ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  തിരികെ 0;
}

#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
സ്ഥിതം പൂർണ്ണസംഖ്യ cb_stream_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id,
                           uint64_t rx_app_error_code,
                           uint64_t tx_app_error_code,
                           ശൂന്യം *user_data, ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ശൂന്യ);
  uint64_t h3_app_error_code = NGHTTP3_H3_NO_ERROR;
  പൂർണ്ണസംഖ്യ rv;

  (ശൂന്യം)tconn;
  (ശൂന്യം)tx_app_error_code;
  /* stream is closed... */
  എങ്കിൽ(!data)
    data = CF_DATA_CURRENT(cf);
  എങ്കിൽ(!data)
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;

  എങ്കിൽ(flags & NGTCP2_STREAM_CLOSE2_FLAG_RX_APP_ERROR_CODE_SET)
    h3_app_error_code = rx_app_error_code;

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, h3_app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, h3_app_error_code, rv);
  എങ്കിൽ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  തിരികെ 0;
}
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_stream_reset(ngtcp2_conn *tconn, int64_t stream_id,
                           uint64_t final_size, uint64_t app_error_code,
                           ശൂന്യം *user_data, ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ശൂന്യ);
  പൂർണ്ണസംഖ്യ rv;
  (ശൂന്യം)tconn;
  (ശൂന്യം)final_size;
  (ശൂന്യം)app_error_code;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  എങ്കിൽ(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  എങ്കിൽ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_stream_stop_sending(ngtcp2_conn *tconn, int64_t stream_id,
                                  uint64_t app_error_code, ശൂന്യം *user_data,
                                  ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  പൂർണ്ണസംഖ്യ rv;
  (ശൂന്യം)tconn;
  (ശൂന്യം)app_error_code;
  (ശൂന്യം)stream_user_data;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  എങ്കിൽ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            ശൂന്യം *user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);

  (ശൂന്യം)tconn;
  ctx->max_bidi_streams = max_streams;
  എങ്കിൽ(data)
    CURL_TRC_CF(data, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->used_bidi_streams);
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_extend_max_stream_data(ngtcp2_conn *tconn, int64_t stream_id,
                                     uint64_t max_data, ശൂന്യം *user_data,
                                     ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream;
  ഘടന Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  പൂർണ്ണസംഖ്യ rv;
  (ശൂന്യം)tconn;
  (ശൂന്യം)max_data;

  rv = nghttp3_conn_unblock_stream(ctx->h3conn, stream_id);
  എങ്കിൽ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  എങ്കിൽ(stream && stream->quic_flow_blocked) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] unblock quic flow", stream_id);
    stream->quic_flow_blocked = FALSE;
    എങ്കിൽ(data)
      Curl_multi_mark_dirty(data);
  }
  തിരികെ 0;
}

സ്ഥിതം ശൂന്യം cb_rand(uint8_t *dest, size_t destlen,
                    സ്ഥിരം ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode result;
  (ശൂന്യം)rand_ctx;

  result = Curl_rand(ശൂന്യ, dest, destlen);
  എങ്കിൽ(result) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
സ്ഥിതം പൂർണ്ണസംഖ്യ cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    ശൂന്യം *user_data)
{
  CURLcode result;
  (ശൂന്യം)tconn;
  (ശൂന്യം)user_data;

  result = Curl_rand(ശൂന്യ, cid->data, cidlen);
  എങ്കിൽ(result)
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(ശൂന്യ, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  എങ്കിൽ(result)
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;

  തിരികെ 0;
}

#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
സ്ഥിതം പൂർണ്ണസംഖ്യ cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  ഘടന ngtcp2_stateless_reset_token *token, size_t cidlen, ശൂന്യം *user_data)
{
  CURLcode result;
  (ശൂന്യം)tconn;
  (ശൂന്യം)user_data;

  result = Curl_rand(ശൂന്യ, cid->data, cidlen);
  എങ്കിൽ(result)
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(ശൂന്യ, token->data, വലുപ്പം(token->data));
  എങ്കിൽ(result)
    തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;

  തിരികെ 0;
}
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_recv_rx_താക്കോൽ(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          ശൂന്യം *user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ശൂന്യ;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  (ശൂന്യം)tconn;

  എങ്കിൽ(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    തിരികെ 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(data);
  എങ്കിൽ(ctx && data && !ctx->h3conn && ctx->init_h3_conn_cb) {
    എങ്കിൽ(ctx->init_h3_conn_cb(cf, data, ctx))
      തിരികെ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  തിരികെ 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
സ്ഥിതം ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  ശൂന്യ, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_data_cb,
  cb_ngtcp2_handshake_completed,
  ശൂന്യ, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_mask_cb,
  cb_recv_stream_data,
  cb_acked_stream_data_offset,
  ശൂന്യ, /* stream_open */
  cb_stream_close,
  ശൂന്യ, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  ശൂന്യ, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  ശൂന്യ, /* remove_connection_id */
  ngtcp2_crypto_update_താക്കോൽ_cb, /* update_key */
  ശൂന്യ, /* path_validation */
  ശൂന്യ, /* select_preferred_addr */
  cb_stream_reset,
  ശൂന്യ, /* extend_max_remote_streams_bidi */
  ശൂന്യ, /* extend_max_remote_streams_uni */
  cb_extend_max_stream_data,
  ശൂന്യ, /* dcid_status */
  ശൂന്യ, /* handshake_confirmed */
  ശൂന്യ, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  ശൂന്യ, /* recv_datagram */
  ശൂന്യ, /* ack_datagram */
  ശൂന്യ, /* lost_datagram */
  ngtcp2_crypto_get_path_challenge_data_cb,
  cb_stream_stop_sending,
  ശൂന്യ, /* version_negotiation */
  cb_recv_rx_താക്കോൽ,
  ശൂന്യ, /* recv_tx_key */
  ശൂന്യ, /* early_data_rejected */
#ifdef NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  ശൂന്യ, /* begin_path_validation */
#endif
#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  ശൂന്യ, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  ശൂന്യ, /* dcid_status2 */
  ngtcp2_crypto_get_path_challenge_data2_cb, /* get_path_challenge_data2 */
#endif
#ifdef NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  ശൂന്യ, /* recv_stop_sending */
#endif
#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_stream_close2, /* is called instead of cb_stream_close when set */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic എടുക്കുക
#endif

#എങ്കിൽ defined(_MSC_VER) && defined(_DLL)
#pragma warning(എടുക്കുക)
#endif

#ifdef USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ഘടന Curl_cfilter *cf;
  ഘടന cf_ngtcp2_ctx *ctx;
  ഘടന Curl_easy *data;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_data(ssl);
  cf = cref ? cref->user_data : ശൂന്യ;
  ctx = cf ? cf->ctx : ശൂന്യ;
  data = cf ? CF_DATA_CURRENT(cf) : ശൂന്യ;
  എങ്കിൽ(cf && data && ctx) {
    unsigned അക്ഷരം *quic_tp = ശൂന്യ;
    size_t quic_tp_len = 0;
#ifdef HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     വലുപ്പം(tpbuf));
    എങ്കിൽ(tplen < 0)
      CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((പൂർണ്ണസംഖ്യ)tplen));
    അല്ലെങ്കിൽ {
      quic_tp = (unsigned അക്ഷരം *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#endif
    Curl_ossl_add_session(cf, data, &ctx->tls.ossl, ctx->ssl_peer.scache_താക്കോൽ,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, ശൂന്യ);
  }
  തിരികെ 0;
}
#endif /* USE_OPENSSL */

#ifdef USE_GNUTLS

#ifdef CURLVERBOSE
സ്ഥിതം സ്ഥിരം അക്ഷരം *gtls_hs_സന്ദേശം_name(പൂർണ്ണസംഖ്യ mtype)
{
  തിരഞ്ഞെടുപ്പ്(mtype) {
  അവസ്ഥ 1:
    തിരികെ "ClientHello";
  അവസ്ഥ 2:
    തിരികെ "ServerHello";
  അവസ്ഥ 4:
    തിരികെ "SessionTicket";
  അവസ്ഥ 8:
    തിരികെ "EncryptedExtensions";
  അവസ്ഥ 11:
    തിരികെ "Certificate";
  അവസ്ഥ 13:
    തിരികെ "CertificateRequest";
  അവസ്ഥ 15:
    തിരികെ "CertificateVerify";
  അവസ്ഥ 20:
    തിരികെ "Finished";
  അവസ്ഥ 24:
    തിരികെ "KeyUpdate";
  അവസ്ഥ 254:
    തിരികെ "MessageHash";
  }
  തിരികെ "Unknown";
}
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ quic_gtls_handshake_cb(gnutls_session_t session, unsigned പൂർണ്ണസംഖ്യ htype,
                                  unsigned when, unsigned പൂർണ്ണസംഖ്യ incoming,
                                  സ്ഥിരം gnutls_datum_t *സന്ദേശം)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  ഘടന Curl_cfilter *cf = conn_ref ? conn_ref->user_data : ശൂന്യ;
  ഘടന cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ശൂന്യ;

  (ശൂന്യം)സന്ദേശം;
  (ശൂന്യം)incoming;
  എങ്കിൽ(when && cf && ctx) { /* after message has been processed */
    ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    എങ്കിൽ(!data)
      തിരികെ 0;
    CURL_TRC_CF(data, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_സന്ദേശം_name(htype), htype);
    തിരഞ്ഞെടുപ്പ്(htype) {
    അവസ്ഥ GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned അക്ഷരം *quic_tp = ശൂന്യ;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       വലുപ്പം(tpbuf));
      എങ്കിൽ(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((പൂർണ്ണസംഖ്യ)tplen));
      അല്ലെങ്കിൽ {
        quic_tp = (unsigned അക്ഷരം *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (ശൂന്യം)Curl_gtls_cache_session(cf, data, ctx->ssl_peer.scache_താക്കോൽ,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    ശൂന്യ);
      നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }
  }
  തിരികെ 0;
}
#endif /* USE_GNUTLS */

#ifdef USE_WOLFSSL
സ്ഥിതം പൂർണ്ണസംഖ്യ wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolfSSL_get_app_data(ssl);
  ഘടന Curl_cfilter *cf = conn_ref ? conn_ref->user_data : ശൂന്യ;

  DEBUGASSERT(cf);
  എങ്കിൽ(cf && session) {
    ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
    ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    എങ്കിൽ(data && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned അക്ഷരം *quic_tp = ശൂന്യ;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       വലുപ്പം(tpbuf));
      എങ്കിൽ(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((പൂർണ്ണസംഖ്യ)tplen));
      അല്ലെങ്കിൽ {
        quic_tp = (unsigned അക്ഷരം *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (ശൂന്യം)Curl_wssl_cache_session(cf, data, ctx->ssl_peer.scache_താക്കോൽ,
                                    session, wolfSSL_version(ssl),
                                    "h3", quic_tp, quic_tp_len, ശൂന്യ);
    }
  }
  തിരികെ 0;
}
#endif /* USE_WOLFSSL */

സ്ഥിതം CURLcode cf_ngtcp2_tls_ctx_setup(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        ശൂന്യം *user_data)
{
  ഘടന curl_tls_ctx *ctx = user_data;

#ifdef USE_OPENSSL
#എങ്കിൽ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  എങ്കിൽ(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(data, "ngtcp2_crypto_boringssl_configure_client_context failed");
    തിരികെ CURLE_FAILED_INIT;
  }
#അല്ലെങ്കിലെങ്കിൽ defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#അല്ലെങ്കിൽ
  എങ്കിൽ(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_quictls_configure_client_context failed");
    തിരികെ CURLE_FAILED_INIT;
  }
#endif /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  എങ്കിൽ(Curl_ssl_scache_use(cf, data)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_set_session_cache_mode(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
  എങ്കിൽ(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(data, "ngtcp2_crypto_gnutls_configure_client_session failed");
    തിരികെ CURLE_FAILED_INIT;
  }
  എങ്കിൽ(Curl_ssl_scache_use(cf, data)) {
    gnutls_handshake_set_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
  എങ്കിൽ(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    തിരികെ CURLE_FAILED_INIT;
  }
  എങ്കിൽ(Curl_ssl_scache_use(cf, data)) {
    /* Register to get notified when a new session is received */
    wolfSSL_CTX_sess_set_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#endif
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_ngtcp2_on_session_reuse(ഘടന Curl_cfilter *cf,
                                           ഘടന Curl_easy *data,
                                           ഘടന alpn_spec *alpns,
                                           ഘടന Curl_ssl_session *scs,
                                           bool *do_early_data)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;
#എങ്കിൽ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#endif
#ifdef USE_GNUTLS
  ctx->earlydata_max =
    gnutls_record_get_max_early_data_size(ctx->tls.gtls.session);
#endif
#ifdef USE_WOLFSSL
#ifdef WOLFSSL_EARLY_DATA
  ctx->earlydata_max = scs->earlydata_max;
#അല്ലെങ്കിൽ
  ctx->earlydata_max = 0;
#endif /* WOLFSSL_EARLY_DATA */
#endif
#എങ്കിൽ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  എങ്കിൽ(!ctx->earlydata_max) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session from different ALPN, no early data");
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(data, cf, "no 0RTT transport parameters, no early data");
  }
  അല്ലെങ്കിൽ {
    പൂർണ്ണസംഖ്യ rv;
    rv = ngtcp2_conn_decode_and_set_0rtt_transport_params(
      ctx->qconn, (സ്ഥിരം uint8_t *)scs->quic_tp, scs->quic_tp_len);
    എങ്കിൽ(rv)
      CURL_TRC_CF(data, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    അല്ലെങ്കിൽ എങ്കിൽ(ctx->init_h3_conn_cb) {
      infof(data, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      result = ctx->init_h3_conn_cb(cf, data, ctx);
      എങ്കിൽ(!result) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_data = TRUE;
      }
    }
    അല്ലെങ്കിൽ { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_data = TRUE;
    }
  }
#അല്ലെങ്കിൽ /* not supported in the TLS backend */
  (ശൂന്യം)data;
  (ശൂന്യം)ctx;
  (ശൂന്യം)scs;
  (ശൂന്യം)alpns;
#endif
  തിരികെ result;
}

/*
 * Might be called twice for happy eyeballs.
 */
സ്ഥിതം CURLcode cf_connect_start(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന cf_ngtcp2_io_ctx *pktx)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  പൂർണ്ണസംഖ്യ rc;
  പൂർണ്ണസംഖ്യ rv;
  CURLcode result;
  സ്ഥിരം ഘടന Curl_sockaddr_ex *sockaddr = ശൂന്യ;
  പൂർണ്ണസംഖ്യ qfd;
  സ്ഥിതം സ്ഥിരം ഘടന alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->dcid.data, NGTCP2_MAX_CIDLEN);
  എങ്കിൽ(result)
    തിരികെ result;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->scid.data, NGTCP2_MAX_CIDLEN);
  എങ്കിൽ(result)
    തിരികെ result;

  (ശൂന്യം)Curl_qlogdir(data, ctx->scid.data, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, data, pktx);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  എങ്കിൽ(result)
    തിരികെ result;

  /* Query socket and remote address from sub-chain */
  എങ്കിൽ(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, ശൂന്യ)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  എങ്കിൽ(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = വലുപ്പം(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (ഘടന sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    എങ്കിൽ(rv == -1)
      തിരികെ CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (ഘടന sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_path.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  അല്ലെങ്കിൽ {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    സ്ഥിരം ഘടന Curl_sockaddr_ex *remote = ശൂന്യ;
    എങ്കിൽ(cf->next->cft->query(cf->next, data, CF_QUERY_REMOTE_ADDR, ശൂന്യ,
                            CURL_UNCONST(&remote)))
      തിരികെ CURLE_QUIC_CONNECT_ERROR;
    എങ്കിൽ(!remote)
      തിരികെ CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, വലുപ്പം(ctx->q.local_addr));
    തിരഞ്ഞെടുപ്പ്(remote->family) {
    അവസ്ഥ AF_INET:
      ((ഘടന sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = വലുപ്പം(ഘടന sockaddr_in);
      നിർത്തുക;
#ifdef USE_IPV6
    അവസ്ഥ AF_INET6:
      ((ഘടന sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = വലുപ്പം(ഘടന sockaddr_in6);
      നിർത്തുക;
#endif
    സ്ഥിരസ്ഥിതി:
      തിരികെ CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (ഘടന sockaddr *)&ctx->q.local_addr,
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
  എങ്കിൽ(rc)
    തിരികെ CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_data = cf;

  result = Curl_vquic_tls_init(&ctx->tls, cf, data,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_setup, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  എങ്കിൽ(result)
    തിരികെ result;

#എങ്കിൽ defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  എങ്കിൽ(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_ctx_new failed");
    തിരികെ CURLE_FAILED_INIT;
  }
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  എങ്കിൽ(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_configure_client_session failed");
    തിരികെ CURLE_FAILED_INIT;
  }
#അല്ലെങ്കിലെങ്കിൽ defined(USE_OPENSSL)
  SSL_set_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#അല്ലെങ്കിൽ
#error "ngtcp2 TLS backend not defined"
#endif

#എങ്കിൽ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  എങ്കിൽ(ctx->use_earlydata &&
     !SSL_set_quic_tls_early_data_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(data, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#endif
  ngtcp2_ccerr_default(&ctx->last_error);

  തിരികെ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    bool *done)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ഘടന cf_call_data save;
  ഘടന cf_ngtcp2_io_ctx pktx;

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  /* Connect the sub-chain */
  എങ്കിൽ(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    എങ്കിൽ(result || !*done)
      തിരികെ result;
  }

  *done = FALSE;

  എങ്കിൽ(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    തിരികെ CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  CF_DATA_SAVE(save, cf, data);

  എങ്കിൽ(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(data);
    result = cf_connect_start(cf, data, &pktx);
    എങ്കിൽ(result)
      പോകുക out;
    എങ്കിൽ(cf->connected) {
      *done = TRUE;
      പോകുക out;
    }
    result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
    /* we do not expect to be able to recv anything yet */
    പോകുക out;
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  എങ്കിൽ(result)
    പോകുക out;

  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
  എങ്കിൽ(result)
    പോകുക out;

  എങ്കിൽ(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    result = ctx->tls_vrfy_result;
    എങ്കിൽ(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      cf->connected = TRUE;
      *done = TRUE;
    }
  }

out:
  എങ്കിൽ(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
  എങ്കിൽ(ctx->qconn &&
     ((result == CURLE_RECV_ERROR) || (result == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_in_draining_period(ctx->qconn)) {
    സ്ഥിരം ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    result = CURLE_COULDNT_CONNECT;
    എങ്കിൽ(cerr) {
      CURL_TRC_CF(data, cf, "connect error, type=%d, code=%" PRIu64,
                  (പൂർണ്ണസംഖ്യ)cerr->type, cerr->error_code);
      തിരഞ്ഞെടുപ്പ്(cerr->type) {
      അവസ്ഥ NGTCP2_CCERR_TYPE_VERSION_NEGOTIATION:
        CURL_TRC_CF(data, cf, "error in version negotiation");
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        എങ്കിൽ(cerr->error_code >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(data, cf, "crypto error, tls alert=%u",
                      (unsigned പൂർണ്ണസംഖ്യ)(cerr->error_code & 0xffU));
        }
        അല്ലെങ്കിൽ എങ്കിൽ(cerr->error_code == NGTCP2_CONNECTION_REFUSED) {
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
  എങ്കിൽ(result) {
    എങ്കിൽ(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      ഘടന ip_quadruple ip;

      എങ്കിൽ(!Curl_cf_socket_peek(cf->next, data, ശൂന്യ, ശൂന്യ, &ip))
        infof(data, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    }
  }
#endif
  എങ്കിൽ(!result && ctx->qconn) {
    result = Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx);
  }
  എങ്കിൽ(result || *done)
    CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (പൂർണ്ണസംഖ്യ)result, *done);
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(ഘടന Curl_cfilter *cf,
                                     ഘടന Curl_easy *data, bool *done)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന cf_call_data save;
  ഘടന cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(cf->shutdown || !ctx->qconn) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  എങ്കിൽ(!cf->next) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);
  *done = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  എങ്കിൽ(!ctx->shutdown_started) {
    അക്ഷരം buffer[NGTCP2_MAX_UDP_PAYLOAD_SIZE];
    ngtcp2_ssize nwritten;

    എങ്കിൽ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
      CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
      result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
      എങ്കിൽ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
        CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
        result = CURLE_OK;
        പോകുക out;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(result) {
        CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                    (പൂർണ്ണസംഖ്യ)result);
        *done = TRUE;
        പോകുക out;
      }
    }

    DEBUGASSERT(Curl_bufq_is_empty(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, ശൂന്യ, /* path */
      ശൂന്യ, /* pkt_info */
      (uint8_t *)buffer, വലുപ്പം(buffer),
      &ctx->last_error, pktx.ts);
    CURL_TRC_CF(data, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (പൂർണ്ണസംഖ്യ)ctx->last_error.type,
                ctx->last_error.error_code, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    എങ്കിൽ(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      result = Curl_bufq_write(&ctx->q.sendbuf, (സ്ഥിരം unsigned അക്ഷരം *)buffer,
                               (size_t)nwritten, &n);
      എങ്കിൽ(result) {
        CURL_TRC_CF(data, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (പൂർണ്ണസംഖ്യ)result);
        പോകുക out;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.split_len = 0;
    }
  }

  എങ്കിൽ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing egress");
    result = Curl_vquic_flush(cf, data, &ctx->q);
    എങ്കിൽ(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
      result = CURLE_OK;
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(result) {
      CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                  (പൂർണ്ണസംഖ്യ)result);
      *done = TRUE;
      പോകുക out;
    }
  }

  എങ്കിൽ(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
    result = CURLE_OK;
  }
out:
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

ശൂന്യം Curl_cf_ngtcp2_cmn_conn_close(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  bool done;
  Curl_cf_ngtcp2_cmn_shutdown(cf, data, &done);
}

സ്ഥിതം bool cf_ngtcp2_err_is_fatal(പൂർണ്ണസംഖ്യ code)
{
  തിരികെ (NGTCP2_ERR_FATAL >= code) ||
         (NGTCP2_ERR_DROP_CONN == code) ||
         (NGTCP2_ERR_IDLE_CLOSE == code);
}

ശൂന്യം Curl_cf_ngtcp2_cmn_err_set(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ code)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  എങ്കിൽ(!ctx->last_error.error_code) {
    എങ്കിൽ(NGTCP2_ERR_CRYPTO == code) {
      ngtcp2_ccerr_set_tls_alert(&ctx->last_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 ശൂന്യ, 0);
    }
    അല്ലെങ്കിൽ {
      ngtcp2_ccerr_set_liberr(&ctx->last_error, code, ശൂന്യ, 0);
    }
  }
  എങ്കിൽ(cf_ngtcp2_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

ശൂന്യം Curl_cf_ngtcp2_io_ctx_init(ഘടന cf_ngtcp2_io_ctx *io_ctx,
                                ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->data = data;
  io_ctx->now = *Curl_pgrs_now(data);
  ngtcp2_path_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_set_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

ശൂന്യം Curl_cf_ngtcp2_io_ctx_update_time(ഘടന Curl_easy *data,
                                       ഘടന cf_ngtcp2_io_ctx *io_ctx,
                                       ഘടന Curl_cfilter *cf)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(data);
  Curl_vquic_ctx_update_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#എങ്കിൽ NGTCP2_VERSION_അക്കം < 0x011100
ഘടന cf_ngtcp2_sfind_ctx {
  int64_t stream_id;
  ഘടന h3_stream_ctx *stream;
  uint32_t mid;
};

സ്ഥിതം bool cf_ngtcp2_sfind(uint32_t mid, ശൂന്യം *മൂല്യം, ശൂന്യം *user_data)
{
  ഘടന cf_ngtcp2_sfind_ctx *fctx = user_data;
  ഘടന h3_stream_ctx *stream = മൂല്യം;

  എങ്കിൽ(fctx->stream_id == stream->id) {
    fctx->mid = mid;
    fctx->stream = stream;
    തിരികെ FALSE;
  }
  തിരികെ TRUE; /* continue */
}

സ്ഥിതം ഘടന h3_stream_ctx *cf_ngtcp2_get_stream(ഘടന cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  ഘടന cf_ngtcp2_sfind_ctx fctx;
  fctx.stream_id = stream_id;
  fctx.stream = ശൂന്യ;
  Curl_u32_ptrset_visit(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  തിരികെ fctx.stream;
}
#അല്ലെങ്കിൽ
സ്ഥിതം ഘടന h3_stream_ctx *cf_ngtcp2_get_stream(ഘടന cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  തിരികെ ngtcp2_conn_get_stream_user_data(ctx->qconn, stream_id);
}
#endif

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
സ്ഥിതം CURLcode read_pkt_to_send(ശൂന്യം *userp,
                                 unsigned അക്ഷരം *buf, size_t buflen,
                                 size_t *pnread)
{
  ഘടന cf_ngtcp2_io_ctx *x = userp;
  ഘടന cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t stream_id;
  പൂർണ്ണസംഖ്യ fin;
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
  വേണ്ടി(;;) {

    എങ്കിൽ(ctx->h3conn && ngtcp2_conn_get_max_data_ഇടത്(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_stream(ctx->h3conn, &stream_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      എങ്കിൽ(veccnt < 0) {
        failf(x->data, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((പൂർണ്ണസംഖ്യ)veccnt));
        Curl_cf_ngtcp2_h3_err_set(x->cf, x->data, (പൂർണ്ണസംഖ്യ)veccnt);
        തിരികെ CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_STREAM_FLAG_MORE |
            (fin ? NGTCP2_WRITE_STREAM_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_stream(ctx->qconn, &x->ps.path,
                                  ശൂന്യ, buf, buflen,
                                  &ndatalen, flags, stream_id,
                                  (സ്ഥിരം ngtcp2_vec *)vec, veccnt, x->ts);
    എങ്കിൽ(n == 0) {
      /* nothing to send */
      തിരികെ CURLE_AGAIN;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(n < 0) {
      തിരഞ്ഞെടുപ്പ്(n) {
      അവസ്ഥ NGTCP2_ERR_STREAM_DATA_BLOCKED: {
        ഘടന h3_stream_ctx *stream;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_block_stream(ctx->h3conn, stream_id);
        CURL_TRC_CF(x->data, x->cf, "[%" PRId64 "] block quic flow",
                    stream_id);
        stream = cf_ngtcp2_get_stream(ctx, stream_id);
        എങ്കിൽ(stream) /* it might be not one of our h3 streams? */
          stream->quic_flow_blocked = TRUE;
        n = 0;
        നിർത്തുക;
      }
      അവസ്ഥ NGTCP2_ERR_STREAM_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_stream_write(ctx->h3conn, stream_id);
        n = 0;
        നിർത്തുക;
      അവസ്ഥ NGTCP2_ERR_WRITE_MORE:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        DEBUGASSERT(ndatalen == -1);
        failf(x->data, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((പൂർണ്ണസംഖ്യ)n));
        Curl_cf_ngtcp2_cmn_err_set(x->cf, x->data, (പൂർണ്ണസംഖ്യ)n);
        തിരികെ CURLE_SEND_ERROR;
      }
    }

    എങ്കിൽ(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      പൂർണ്ണസംഖ്യ rv = nghttp3_conn_add_write_offset(ctx->h3conn, stream_id, ndatalen);
      എങ്കിൽ(rv) {
        failf(x->data, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        തിരികെ CURLE_SEND_ERROR;
      }
    }

    എങ്കിൽ(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      തിരികെ CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_progress_egress(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        ഘടന cf_ngtcp2_io_ctx *pktx)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_size, path_max_payload_size;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_quantum;
  CURLcode result;
  ഘടന cf_ngtcp2_io_ctx local_pktx;

  എങ്കിൽ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  അല്ലെങ്കിൽ {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  എങ്കിൽ(result) {
    എങ്കിൽ(result == CURLE_AGAIN) {
      Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
      തിരികെ CURLE_OK;
    }
    തിരികെ result;
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
  വേണ്ടി(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_size,
                            read_pkt_to_send, pktx, &nread);
    എങ്കിൽ(result == CURLE_AGAIN)
      നിർത്തുക;
    അല്ലെങ്കിൽ എങ്കിൽ(result)
      തിരികെ result;
    അല്ലെങ്കിൽ {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      എങ്കിൽ((buflen >= send_quantum) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_size))
        നിർത്തുക;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      എങ്കിൽ(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(nread > gsolen ||
              (gsolen > path_max_payload_size && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        result = Curl_vquic_send_tail_split(cf, data, &ctx->q,
                                            gsolen, nread, nread);
        എങ്കിൽ(result) {
          എങ്കിൽ(result == CURLE_AGAIN) {
            Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
            തിരികെ CURLE_OK;
          }
          തിരികെ result;
        }
        pktcnt = 0;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        നിർത്തുക;
      }
    }
  }

  എങ്കിൽ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(data, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
    എങ്കിൽ(result) {
      എങ്കിൽ(result == CURLE_AGAIN) {
        Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
        തിരികെ CURLE_OK;
      }
      തിരികെ result;
    }
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_conn_update_pkt_tx_time(ctx->qconn, pktx->ts);
  }
  തിരികെ CURLE_OK;
}

ഘടന cf_ngtcp2_recv_ctx {
  ഘടന cf_ngtcp2_io_ctx *pktx;
  size_t pkt_എണ്ണം;
};

സ്ഥിതം CURLcode cf_ngtcp2_recv_pkts(സ്ഥിരം unsigned അക്ഷരം *buf, size_t buflen,
                                    size_t gso_size,
                                    ഘടന sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    ശൂന്യം *userp)
{
  ഘടന cf_ngtcp2_recv_ctx *rctx = userp;
  ഘടന cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  ഘടന cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_path path;
  size_t offset, pktlen;
  പൂർണ്ണസംഖ്യ rv;

  എങ്കിൽ(!rctx->pkt_എണ്ണം) {
    Curl_cf_ngtcp2_io_ctx_update_time(pktx->data, pktx, pktx->cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  എങ്കിൽ(ecn)
    CURL_TRC_CF(pktx->data, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_size, (unsigned)ecn);
  ngtcp2_addr_init(&path.local, (ഘടന sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&path.remote, (ഘടന sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  വേണ്ടി(offset = 0; offset < buflen; offset += gso_size) {
    rctx->pkt_എണ്ണം++;
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &path, &pi,
                              buf + offset, pktlen, pktx->ts);
    എങ്കിൽ(rv) {
      CURL_TRC_CF(pktx->data, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_set(pktx->cf, pktx->data, rv);

      എങ്കിൽ(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        തിരികെ CURLE_PEER_FAILED_VERIFICATION;
      തിരികെ CURLE_RECV_ERROR;
    }
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_progress_ingress(ഘടന Curl_cfilter *cf,
                                         ഘടന Curl_easy *data,
                                         ഘടന cf_ngtcp2_io_ctx *pktx)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന cf_ngtcp2_io_ctx local_pktx;
  ഘടന cf_ngtcp2_recv_ctx rctx;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }

  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  എങ്കിൽ(result)
    തിരികെ result;

  rctx.pktx = pktx;
  rctx.pkt_എണ്ണം = 0;

  എങ്കിൽ(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(data, cf, "progress_ingress(socket)");
    തിരികെ Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  അല്ലെങ്കിൽ {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    unsigned അക്ഷരം *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_SIZE;
    size_t pkt_limit = QUIC_TUNNEL_INGRESS_PKT_LIMIT;
    size_t nread;
    ഘടന sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(data, cf, "progress_ingress(sub-filters)");
    എങ്കിൽ(ctx->qconn) {
      size_t max_path_payload;
      max_path_payload =
        ngtcp2_conn_get_path_max_tx_udp_payload_size(ctx->qconn);
      എങ്കിൽ(max_path_payload > max_udp_payload)
        max_udp_payload = max_path_payload;
    }

    എങ്കിൽ(ctx->tunnel_inbuf_len < max_udp_payload) {
      unsigned അക്ഷരം *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      എങ്കിൽ(!newbuf)
        തിരികെ CURLE_OUT_OF_MEMORY;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    ആകുമ്പോൾ(pkt_limit--) {
      result = Curl_conn_cf_recv(cf->next, data, (അക്ഷരം *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      എങ്കിൽ(result == CURLE_AGAIN) {
        /* no more data available at the moment */
        തിരികെ CURLE_OK;
      }
      എങ്കിൽ(result) {
        CURL_TRC_CF(data, cf, "ingress, recv from tunnel failed: %d",
                    (പൂർണ്ണസംഖ്യ)result);
        തിരികെ result;
      }
      എങ്കിൽ(nread == 0) {
        /* tunnel closed */
        തിരികെ CURLE_OK;
      }

      memcpy(&remote_addr, ctx->connected_path.remote.addr,
             ctx->connected_path.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_path.remote.addrlen;
      result = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      എങ്കിൽ(result)
        തിരികെ result;

      എങ്കിൽ(!ctx->q.got_first_byte) {
        ctx->q.got_first_byte = TRUE;
        ctx->q.first_byte_at = ctx->q.last_op;
      }
      ctx->q.last_io = ctx->q.last_op;
    }
    തിരികെ CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_set_expiry(ഘടന Curl_cfilter *cf,
                                       ഘടന Curl_easy *data,
                                       ഘടന cf_ngtcp2_io_ctx *pktx)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  എങ്കിൽ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  അല്ലെങ്കിൽ {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  എങ്കിൽ(expiry != UINT64_MAX) {
    എങ്കിൽ(expiry <= pktx->ts) {
      CURLcode result;
      പൂർണ്ണസംഖ്യ rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      എങ്കിൽ(rv) {
        failf(data, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_set(cf, data, rv);
        തിരികെ CURLE_SEND_ERROR;
      }
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, pktx);
      എങ്കിൽ(!result)
        result = Curl_cf_ngtcp2_progress_egress(cf, data, pktx);
      എങ്കിൽ(result) {
        /* a verify failure during ingress must win over generic errors */
        എങ്കിൽ(ctx->tls_vrfy_result)
          result = ctx->tls_vrfy_result;
        തിരികെ result;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    എങ്കിൽ(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      എങ്കിൽ(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_set(data, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cf_ngtcp2_setup_keep_alive(ഘടന Curl_cfilter *cf,
                                       ഘടന Curl_easy *data)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  സ്ഥിരം ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  എങ്കിൽ(!ctx->qconn)
    തിരികെ;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  എങ്കിൽ(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no peer idle timeout, unset keep-alive");
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!Curl_u32_ptrset_എണ്ണം(&ctx->streams)) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no active streams, unset keep-alive");
  }
  അല്ലെങ്കിൽ {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(data, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_stream_setup(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        ഘടന h3_stream_ctx **pstream)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  *pstream = ശൂന്യ;
  എങ്കിൽ(!data)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(stream) {
    *pstream = stream;
    തിരികെ CURLE_OK;
  }

  stream = curlx_calloc(1, വലുപ്പം(*stream));
  എങ്കിൽ(!stream)
    തിരികെ CURLE_OUT_OF_MEMORY;

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
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_വരി_LEN);

  എങ്കിൽ(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    Curl_cf_ngtcp2_h3_stream_ctx_free(stream);
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  എങ്കിൽ(Curl_u32_ptrset_എണ്ണം(&ctx->streams) == 1)
    cf_ngtcp2_setup_keep_alive(cf, data);

  *pstream = stream;
  തിരികെ CURLE_OK;
}

ശൂന്യം Curl_cf_ngtcp2_h3_stream_close(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    ഘടന h3_stream_ctx *stream)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(data);
  DEBUGASSERT(stream);
  എങ്കിൽ(!stream->closed && ctx->qconn && ctx->h3conn) {
    CURLcode result;

    nghttp3_conn_set_stream_user_data(ctx->h3conn, stream->id, ശൂന്യ);
    ngtcp2_conn_set_stream_user_data(ctx->qconn, stream->id, ശൂന്യ);
    stream->closed = TRUE;
    (ശൂന്യം)ngtcp2_conn_shutdown_stream(ctx->qconn, 0, stream->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    result = Curl_cf_ngtcp2_progress_egress(cf, data, ശൂന്യ);
    എങ്കിൽ(result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cancel stream -> %d",
                  stream->id, (പൂർണ്ണസംഖ്യ)result);
  }
}

ശൂന്യം Curl_cf_ngtcp2_h3_stream_done(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  (ശൂന്യം)cf;
  എങ്കിൽ(stream) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] easy handle is done", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
    എങ്കിൽ(!Curl_u32_ptrset_എണ്ണം(&ctx->streams))
      cf_ngtcp2_setup_keep_alive(cf, data);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      bool *നിവേശം_pending)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  സ്ഥിരം ngtcp2_transport_params *rp;
  ഘടന cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  *നിവേശം_pending = FALSE;
  എങ്കിൽ(!ctx->qconn || ctx->shutdown_started)
    പോകുക out;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  എങ്കിൽ(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->q.last_io);
    എങ്കിൽ(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      എങ്കിൽ((uint64_t)idletime_ms > max_idle_ms)
        പോകുക out;
    }
  }

  എങ്കിൽ(!cf->next || !cf->next->cft->is_alive(cf->next, data, നിവേശം_pending))
    പോകുക out;

  alive = TRUE;
  എങ്കിൽ(*നിവേശം_pending) {
    CURLcode result;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *നിവേശം_pending = FALSE;
    result = Curl_cf_ngtcp2_progress_ingress(cf, data, ശൂന്യ);
    CURL_TRC_CF(data, cf, "is_alive, progress ingress -> %d", (പൂർണ്ണസംഖ്യ)result);
    alive = result ? FALSE : TRUE;
  }

out:
  CF_DATA_RESTORE(cf, save);
  തിരികെ alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(ഘടന cf_ngtcp2_ctx *ctx,
                                      ഘടന Curl_easy *data)
{
  int64_t ctrl_stream_id, qpack_enc_stream_id, qpack_dec_stream_id;
  പൂർണ്ണസംഖ്യ rc;

  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &ctrl_stream_id, ശൂന്യ);
  എങ്കിൽ(rc) {
    failf(data, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    തിരികെ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_stream(ctx->h3conn, ctrl_stream_id);
  എങ്കിൽ(rc) {
    failf(data, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    തിരികെ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_enc_stream_id, ശൂന്യ);
  എങ്കിൽ(rc) {
    failf(data, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    തിരികെ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_dec_stream_id, ശൂന്യ);
  എങ്കിൽ(rc) {
    failf(data, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    തിരികെ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_stream_id,
                                       qpack_dec_stream_id);
  എങ്കിൽ(rc) {
    failf(data, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    തിരികെ CURLE_QUIC_CONNECT_ERROR;
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;

  തിരഞ്ഞെടുപ്പ്(query) {
  അവസ്ഥ CF_QUERY_CONNECT_REPLY_MS:
    എങ്കിൽ((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (പൂർണ്ണസംഖ്യ)ms : INT_MAX;
      തിരികെ CURLE_OK;
    }
    നിർത്തുക;
  അവസ്ഥ CF_QUERY_REALLY_CONNECTED:
    എങ്കിൽ(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_first_byte;
      തിരികെ CURLE_OK;
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (ശൂന്യം)arg1;
  (ശൂന്യം)arg2;
  തിരഞ്ഞെടുപ്പ്(event) {
  അവസ്ഥ CF_CTRL_REPORT_STATS:
    എങ്കിൽ(cf->connected && !ctx->stats_reported) {
      എങ്കിൽ((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        ctx->stats_reported = TRUE;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        എങ്കിൽ(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
