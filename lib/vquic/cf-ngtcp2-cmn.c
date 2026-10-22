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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#அந்தர்பாவ <ngtcp2/ngtcp2.h>

#ifdef USE_OPENSSL
#அந்தர்பாவ <openssl/err.h>
#எனில் defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#அந்தர்பாவ <ngtcp2/ngtcp2_crypto_boringssl.h>
#அல்லதெனில் defined(OPENSSL_QUIC_API2)
#அந்தர்பாவ <ngtcp2/ngtcp2_crypto_ossl.h>
#இல்லையெனில்
#அந்தர்பாவ <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#அந்தர்பாவ "vtls/openssl.h"
#அல்லதெனில் defined(USE_GNUTLS)
#அந்தர்பாவ <ngtcp2/ngtcp2_crypto_gnutls.h>
#அந்தர்பாவ "vtls/gtls.h"
#அல்லதெனில் defined(USE_WOLFSSL)
#அந்தர்பாவ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#அந்தர்பாவ "vtls/wolfssl.h"
#endif

#அந்தர்பாவ <nghttp3/nghttp3.h>

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "u32_ptrset.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "cf-socket.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "curlx/win32-fopen.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "http1.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "sockaddr.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "vdns/cf-dns.h"
#அந்தர்பாவ "vquic/vquic.h"
#அந்தர்பாவ "vquic/vquic_int.h"
#அந்தர்பாவ "vquic/vquic-tls.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "vtls/vtls_scache.h"
#அந்தர்பாவ "vquic/cf-ngtcp2-cmn.h"

/*
 * Store ngtcp2 version info in this buffer.
 */
வெறுமை Curl_ngtcp2_ver(எழுத்து *p, size_t len)
{
  மாறிலி ngtcp2_info *ng2 = ngtcp2_version(0);
  மாறிலி nghttp3_info *ht3 = nghttp3_version(0);
  (வெறுமை)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->version_str, ht3->version_str);
}

வெறுமை Curl_cf_ngtcp2_h3_stream_ctx_free(கட்டமைப்பு h3_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

நிலையான வெறுமை h3_stream_set_free(uint32_t id, வெறுமை *stream)
{
  (வெறுமை)id;
  DEBUGASSERT(stream);
  Curl_cf_ngtcp2_h3_stream_ctx_free((கட்டமைப்பு h3_stream_ctx *)stream);
}

நிலையான bool cf_ngtcp2_h3_err_is_fatal(முழு code)
{
  திரும்பு (NGHTTP3_ERR_FATAL >= code) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM == code);
}

வெறுமை Curl_cf_ngtcp2_h3_err_set(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data, முழு code)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  எனில்(!ctx->last_error.error_code) {
    ngtcp2_ccerr_set_application_error(&ctx->last_error,
      nghttp3_err_infer_quic_app_error_code(code), வெற்று, 0);
  }
  எனில்(cf_ngtcp2_h3_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

CURLcode Curl_cf_ngtcp2_ctx_init(கட்டமைப்பு cf_ngtcp2_ctx *ctx,
                                 கட்டமைப்பு Curl_peer *origin,
                                 கட்டமைப்பு Curl_peer *peer,
                                 கட்டமைப்பு ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = வெற்று;
  ctx->tunnel_inbuf_len = 0;
  ctx->version = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->stream_bufcp, H3_STREAM_CHUNK_SIZE,
                  H3_STREAM_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  திரும்பு Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

வெறுமை Curl_cf_ngtcp2_ctx_cleanup(கட்டமைப்பு cf_ngtcp2_ctx *ctx)
{
  எனில்(ctx && ctx->initialized) {
    எனில்(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    எனில்(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = வெற்று;
    }
    எனில்(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = வெற்று;
    }
#ifdef OPENSSL_QUIC_API2
    எனில்(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = வெற்று;
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

கட்டமைப்பு Curl_easy *Curl_cf_ngtcp2_get_xfer(கட்டமைப்பு Curl_cfilter *cf,
                                          வெறுமை *stream_user_data,
                                          கட்டமைப்பு h3_stream_ctx **pstream)
{
  கட்டமைப்பு h3_stream_ctx *stream = stream_user_data;

  எனில்(pstream)
    *pstream = stream;
  எனில்(stream && (stream->mid != UINT32_MAX)) {
    கட்டமைப்பு Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    எனில்(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      திரும்பு வெற்று;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    எனில்(data && (data->id == stream->xfer_id))
      திரும்பு data;
    DEBUGASSERT(0);
  }
  திரும்பு வெற்று;
}

நிலையான ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  கட்டமைப்பு Curl_cfilter *cf = conn_ref->user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  திரும்பு ctx->qconn;
}

#ifdef DEBUG_NGTCP2
நிலையான வெறுமை quic_printf(வெறுமை *user_data, மாறிலி எழுத்து *fmt, ...)
{
  va_list ap;
  (வெறுமை)user_data;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#endif

நிலையான வெறுமை qlog_callback(வெறுமை *user_data, uint32_t flags,
                          மாறிலி வெறுமை *data, size_t datalen)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  (வெறுமை)flags;
  எனில்(ctx->qlogfd != -1) {
    ssize_t rc = லிக(ctx->qlogfd, data, datalen);
    எனில்(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

நிலையான வெறுமை quic_settings(கட்டமைப்பு cf_ngtcp2_ctx *ctx,
                          கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#ifdef DEBUG_NGTCP2
  s->log_printf = quic_printf;
#இல்லையெனில்
  s->log_printf = வெற்று;
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
  எனில்(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#எனில் defined(_MSC_VER) && defined(_DLL)
#pragma warning(தள்ளு)
#pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

நிலையான முழு cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, வெறுமை *user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf ? cf->ctx : வெற்று;
  கட்டமைப்பு Curl_easy *data;

  (வெறுமை)tconn;
  DEBUGASSERT(ctx);
  data = CF_DATA_CURRENT(cf);
  DEBUGASSERT(data);
  எனில்(!ctx || !data)
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(data);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, data);

  ctx->tls_vrfy_result = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    data, &ctx->ssl_peer);
  எனில்(ctx->tls_vrfy_result)
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;

#ifdef CURLVERBOSE
  எனில்(Curl_trc_is_verbose(data)) {
    மாறிலி ngtcp2_transport_params *rp;
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
  எனில்(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  எனில்(ctx->use_earlydata) {
#எனில் defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    எனில்(SSL_get_early_data_status(ctx->tls.ossl.ssl) == SSL_EARLY_DATA_NOT_SENT)
      CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
#endif

#எனில் NGTCP2_VERSION_இலக்கம் >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_data_rejected2(ctx->qconn);
#இல்லையெனில் /* older NGTCP2 */
    {
#எனில் defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      முழு ossl_early_status = SSL_get_early_data_status(ctx->tls.ossl.ssl);
      எனில்(ossl_early_status == SSL_EARLY_DATA_NOT_SENT)
        CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_status == SSL_EARLY_DATA_ACCEPTED);
#அல்லதெனில் defined(USE_GNUTLS)
      முழு flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_DATA);
#அல்லதெனில் defined(USE_WOLFSSL)
#ifdef WOLFSSL_EARLY_DATA
      ctx->earlydata_accepted =
        (wolfSSL_get_early_data_status(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_DATA_REJECTED);
#இல்லையெனில்
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
  திரும்பு 0;
}

நிலையான முழு cb_recv_stream_data(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t stream_id, uint64_t offset,
                               மாறிலி uint8_t *buf, size_t buflen,
                               வெறுமை *user_data, வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  முழு fin = (flags & NGTCP2_STREAM_DATA_FLAG_FIN) ? 1 : 0;
  கட்டமைப்பு h3_stream_ctx *stream;
  கட்டமைப்பு Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (வெறுமை)offset;
  rc = nghttp3_conn_read_stream(ctx->h3conn, stream_id, buf, buflen, fin);
  எனில்(rc < 0) {
    எனில்(data && stream) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  stream_id, stream->reset, stream->closed);
    }
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  எனில்(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_stream_offset(tconn, stream_id, nconsumed);
    ngtcp2_conn_extend_max_offset(tconn, nconsumed);
    எனில்(stream) {
      stream->rx_offset += nconsumed;
      stream->rx_offset_max += nconsumed;
    }
  }
  திரும்பு 0;
}

நிலையான முழு cb_acked_stream_data_offset(ngtcp2_conn *tconn, int64_t stream_id,
                                       uint64_t offset, uint64_t datalen,
                                       வெறுமை *user_data, வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  முழு rv;
  (வெறுமை)stream_id;
  (வெறுமை)tconn;
  (வெறுமை)offset;
  (வெறுமை)datalen;
  (வெறுமை)stream_user_data;

  rv = nghttp3_conn_add_ack_offset(ctx->h3conn, stream_id, datalen);
  எனில்(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;
  }

  திரும்பு 0;
}

நிலையான முழு cb_stream_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id, uint64_t app_error_code,
                           வெறுமை *user_data, வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, வெற்று);
  முழு rv;

  (வெறுமை)tconn;
  /* stream is closed... */
  எனில்(!data)
    data = CF_DATA_CURRENT(cf);
  எனில்(!data)
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;

  எனில்(!(flags & NGTCP2_STREAM_CLOSE_FLAG_APP_ERROR_CODE_SET)) {
    app_error_code = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, app_error_code, rv);
  எனில்(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;
  }

  திரும்பு 0;
}

#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
நிலையான முழு cb_stream_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id,
                           uint64_t rx_app_error_code,
                           uint64_t tx_app_error_code,
                           வெறுமை *user_data, வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, வெற்று);
  uint64_t h3_app_error_code = NGHTTP3_H3_NO_ERROR;
  முழு rv;

  (வெறுமை)tconn;
  (வெறுமை)tx_app_error_code;
  /* stream is closed... */
  எனில்(!data)
    data = CF_DATA_CURRENT(cf);
  எனில்(!data)
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;

  எனில்(flags & NGTCP2_STREAM_CLOSE2_FLAG_RX_APP_ERROR_CODE_SET)
    h3_app_error_code = rx_app_error_code;

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, h3_app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, h3_app_error_code, rv);
  எனில்(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;
  }

  திரும்பு 0;
}
#endif

நிலையான முழு cb_stream_reset(ngtcp2_conn *tconn, int64_t stream_id,
                           uint64_t final_size, uint64_t app_error_code,
                           வெறுமை *user_data, வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, வெற்று);
  முழு rv;
  (வெறுமை)tconn;
  (வெறுமை)final_size;
  (வெறுமை)app_error_code;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  எனில்(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  எனில்(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;
  }

  திரும்பு 0;
}

நிலையான முழு cb_stream_stop_sending(ngtcp2_conn *tconn, int64_t stream_id,
                                  uint64_t app_error_code, வெறுமை *user_data,
                                  வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  முழு rv;
  (வெறுமை)tconn;
  (வெறுமை)app_error_code;
  (வெறுமை)stream_user_data;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  எனில்(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;
  }

  திரும்பு 0;
}

நிலையான முழு cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            வெறுமை *user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);

  (வெறுமை)tconn;
  ctx->max_bidi_streams = max_streams;
  எனில்(data)
    CURL_TRC_CF(data, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->used_bidi_streams);
  திரும்பு 0;
}

நிலையான முழு cb_extend_max_stream_data(ngtcp2_conn *tconn, int64_t stream_id,
                                     uint64_t max_data, வெறுமை *user_data,
                                     வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream;
  கட்டமைப்பு Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  முழு rv;
  (வெறுமை)tconn;
  (வெறுமை)max_data;

  rv = nghttp3_conn_unblock_stream(ctx->h3conn, stream_id);
  எனில்(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;
  }
  எனில்(stream && stream->quic_flow_blocked) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] unblock quic flow", stream_id);
    stream->quic_flow_blocked = FALSE;
    எனில்(data)
      Curl_multi_mark_dirty(data);
  }
  திரும்பு 0;
}

நிலையான வெறுமை cb_rand(uint8_t *dest, size_t destlen,
                    மாறிலி ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode result;
  (வெறுமை)rand_ctx;

  result = Curl_rand(வெற்று, dest, destlen);
  எனில்(result) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
நிலையான முழு cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    வெறுமை *user_data)
{
  CURLcode result;
  (வெறுமை)tconn;
  (வெறுமை)user_data;

  result = Curl_rand(வெற்று, cid->data, cidlen);
  எனில்(result)
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(வெற்று, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  எனில்(result)
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;

  திரும்பு 0;
}

#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
நிலையான முழு cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  கட்டமைப்பு ngtcp2_stateless_reset_token *token, size_t cidlen, வெறுமை *user_data)
{
  CURLcode result;
  (வெறுமை)tconn;
  (வெறுமை)user_data;

  result = Curl_rand(வெற்று, cid->data, cidlen);
  எனில்(result)
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(வெற்று, token->data, அளவு(token->data));
  எனில்(result)
    திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;

  திரும்பு 0;
}
#endif

நிலையான முழு cb_recv_rx_சாவி(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          வெறுமை *user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf ? cf->ctx : வெற்று;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  (வெறுமை)tconn;

  எனில்(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    திரும்பு 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(data);
  எனில்(ctx && data && !ctx->h3conn && ctx->init_h3_conn_cb) {
    எனில்(ctx->init_h3_conn_cb(cf, data, ctx))
      திரும்பு NGTCP2_ERR_CALLBACK_FAILURE;
  }
  திரும்பு 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
நிலையான ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  வெற்று, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_data_cb,
  cb_ngtcp2_handshake_completed,
  வெற்று, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_mask_cb,
  cb_recv_stream_data,
  cb_acked_stream_data_offset,
  வெற்று, /* stream_open */
  cb_stream_close,
  வெற்று, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  வெற்று, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  வெற்று, /* remove_connection_id */
  ngtcp2_crypto_update_சாவி_cb, /* update_key */
  வெற்று, /* path_validation */
  வெற்று, /* select_preferred_addr */
  cb_stream_reset,
  வெற்று, /* extend_max_remote_streams_bidi */
  வெற்று, /* extend_max_remote_streams_uni */
  cb_extend_max_stream_data,
  வெற்று, /* dcid_status */
  வெற்று, /* handshake_confirmed */
  வெற்று, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  வெற்று, /* recv_datagram */
  வெற்று, /* ack_datagram */
  வெற்று, /* lost_datagram */
  ngtcp2_crypto_get_path_challenge_data_cb,
  cb_stream_stop_sending,
  வெற்று, /* version_negotiation */
  cb_recv_rx_சாவி,
  வெற்று, /* recv_tx_key */
  வெற்று, /* early_data_rejected */
#ifdef NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  வெற்று, /* begin_path_validation */
#endif
#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  வெற்று, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  வெற்று, /* dcid_status2 */
  ngtcp2_crypto_get_path_challenge_data2_cb, /* get_path_challenge_data2 */
#endif
#ifdef NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  வெற்று, /* recv_stop_sending */
#endif
#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_stream_close2, /* is called instead of cb_stream_close when set */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic எடு
#endif

#எனில் defined(_MSC_VER) && defined(_DLL)
#pragma warning(எடு)
#endif

#ifdef USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
நிலையான முழு quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  கட்டமைப்பு Curl_cfilter *cf;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx;
  கட்டமைப்பு Curl_easy *data;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_data(ssl);
  cf = cref ? cref->user_data : வெற்று;
  ctx = cf ? cf->ctx : வெற்று;
  data = cf ? CF_DATA_CURRENT(cf) : வெற்று;
  எனில்(cf && data && ctx) {
    unsigned எழுத்து *quic_tp = வெற்று;
    size_t quic_tp_len = 0;
#ifdef HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     அளவு(tpbuf));
    எனில்(tplen < 0)
      CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((முழு)tplen));
    இல்லையெனில் {
      quic_tp = (unsigned எழுத்து *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#endif
    Curl_ossl_add_session(cf, data, &ctx->tls.ossl, ctx->ssl_peer.scache_சாவி,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, வெற்று);
  }
  திரும்பு 0;
}
#endif /* USE_OPENSSL */

#ifdef USE_GNUTLS

#ifdef CURLVERBOSE
நிலையான மாறிலி எழுத்து *gtls_hs_செய்தி_name(முழு mtype)
{
  தேர்வு(mtype) {
  நிலை 1:
    திரும்பு "ClientHello";
  நிலை 2:
    திரும்பு "ServerHello";
  நிலை 4:
    திரும்பு "SessionTicket";
  நிலை 8:
    திரும்பு "EncryptedExtensions";
  நிலை 11:
    திரும்பு "Certificate";
  நிலை 13:
    திரும்பு "CertificateRequest";
  நிலை 15:
    திரும்பு "CertificateVerify";
  நிலை 20:
    திரும்பு "Finished";
  நிலை 24:
    திரும்பு "KeyUpdate";
  நிலை 254:
    திரும்பு "MessageHash";
  }
  திரும்பு "Unknown";
}
#endif

நிலையான முழு quic_gtls_handshake_cb(gnutls_session_t session, unsigned முழு htype,
                                  unsigned when, unsigned முழு incoming,
                                  மாறிலி gnutls_datum_t *செய்தி)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  கட்டமைப்பு Curl_cfilter *cf = conn_ref ? conn_ref->user_data : வெற்று;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf ? cf->ctx : வெற்று;

  (வெறுமை)செய்தி;
  (வெறுமை)incoming;
  எனில்(when && cf && ctx) { /* after message has been processed */
    கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    எனில்(!data)
      திரும்பு 0;
    CURL_TRC_CF(data, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_செய்தி_name(htype), htype);
    தேர்வு(htype) {
    நிலை GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned எழுத்து *quic_tp = வெற்று;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       அளவு(tpbuf));
      எனில்(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((முழு)tplen));
      இல்லையெனில் {
        quic_tp = (unsigned எழுத்து *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (வெறுமை)Curl_gtls_cache_session(cf, data, ctx->ssl_peer.scache_சாவி,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    வெற்று);
      நிறுத்து;
    }
    இயல்பு:
      நிறுத்து;
    }
  }
  திரும்பு 0;
}
#endif /* USE_GNUTLS */

#ifdef USE_WOLFSSL
நிலையான முழு wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolfSSL_get_app_data(ssl);
  கட்டமைப்பு Curl_cfilter *cf = conn_ref ? conn_ref->user_data : வெற்று;

  DEBUGASSERT(cf);
  எனில்(cf && session) {
    கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
    கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    எனில்(data && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned எழுத்து *quic_tp = வெற்று;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       அளவு(tpbuf));
      எனில்(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((முழு)tplen));
      இல்லையெனில் {
        quic_tp = (unsigned எழுத்து *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (வெறுமை)Curl_wssl_cache_session(cf, data, ctx->ssl_peer.scache_சாவி,
                                    session, wolfSSL_version(ssl),
                                    "h3", quic_tp, quic_tp_len, வெற்று);
    }
  }
  திரும்பு 0;
}
#endif /* USE_WOLFSSL */

நிலையான CURLcode cf_ngtcp2_tls_ctx_setup(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data,
                                        வெறுமை *user_data)
{
  கட்டமைப்பு curl_tls_ctx *ctx = user_data;

#ifdef USE_OPENSSL
#எனில் defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  எனில்(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(data, "ngtcp2_crypto_boringssl_configure_client_context failed");
    திரும்பு CURLE_FAILED_INIT;
  }
#அல்லதெனில் defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#இல்லையெனில்
  எனில்(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_quictls_configure_client_context failed");
    திரும்பு CURLE_FAILED_INIT;
  }
#endif /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  எனில்(Curl_ssl_scache_use(cf, data)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_set_session_cache_mode(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#அல்லதெனில் defined(USE_GNUTLS)
  எனில்(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(data, "ngtcp2_crypto_gnutls_configure_client_session failed");
    திரும்பு CURLE_FAILED_INIT;
  }
  எனில்(Curl_ssl_scache_use(cf, data)) {
    gnutls_handshake_set_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#அல்லதெனில் defined(USE_WOLFSSL)
  எனில்(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    திரும்பு CURLE_FAILED_INIT;
  }
  எனில்(Curl_ssl_scache_use(cf, data)) {
    /* Register to get notified when a new session is received */
    wolfSSL_CTX_sess_set_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#endif
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_ngtcp2_on_session_reuse(கட்டமைப்பு Curl_cfilter *cf,
                                           கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு alpn_spec *alpns,
                                           கட்டமைப்பு Curl_ssl_session *scs,
                                           bool *do_early_data)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;
#எனில் defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#endif
#ifdef USE_GNUTLS
  ctx->earlydata_max =
    gnutls_record_get_max_early_data_size(ctx->tls.gtls.session);
#endif
#ifdef USE_WOLFSSL
#ifdef WOLFSSL_EARLY_DATA
  ctx->earlydata_max = scs->earlydata_max;
#இல்லையெனில்
  ctx->earlydata_max = 0;
#endif /* WOLFSSL_EARLY_DATA */
#endif
#எனில் defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  எனில்(!ctx->earlydata_max) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  இல்லையெனில் எனில்(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session from different ALPN, no early data");
  }
  இல்லையெனில் எனில்(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(data, cf, "no 0RTT transport parameters, no early data");
  }
  இல்லையெனில் {
    முழு rv;
    rv = ngtcp2_conn_decode_and_set_0rtt_transport_params(
      ctx->qconn, (மாறிலி uint8_t *)scs->quic_tp, scs->quic_tp_len);
    எனில்(rv)
      CURL_TRC_CF(data, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    இல்லையெனில் எனில்(ctx->init_h3_conn_cb) {
      infof(data, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      result = ctx->init_h3_conn_cb(cf, data, ctx);
      எனில்(!result) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_data = TRUE;
      }
    }
    இல்லையெனில் { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_data = TRUE;
    }
  }
#இல்லையெனில் /* not supported in the TLS backend */
  (வெறுமை)data;
  (வெறுமை)ctx;
  (வெறுமை)scs;
  (வெறுமை)alpns;
#endif
  திரும்பு result;
}

/*
 * Might be called twice for happy eyeballs.
 */
நிலையான CURLcode cf_connect_start(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு cf_ngtcp2_io_ctx *pktx)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  முழு rc;
  முழு rv;
  CURLcode result;
  மாறிலி கட்டமைப்பு Curl_sockaddr_ex *sockaddr = வெற்று;
  முழு qfd;
  நிலையான மாறிலி கட்டமைப்பு alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->dcid.data, NGTCP2_MAX_CIDLEN);
  எனில்(result)
    திரும்பு result;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->scid.data, NGTCP2_MAX_CIDLEN);
  எனில்(result)
    திரும்பு result;

  (வெறுமை)Curl_qlogdir(data, ctx->scid.data, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, data, pktx);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  எனில்(result)
    திரும்பு result;

  /* Query socket and remote address from sub-chain */
  எனில்(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, வெற்று)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  எனில்(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = அளவு(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (கட்டமைப்பு sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    எனில்(rv == -1)
      திரும்பு CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (கட்டமைப்பு sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_path.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  இல்லையெனில் {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    மாறிலி கட்டமைப்பு Curl_sockaddr_ex *remote = வெற்று;
    எனில்(cf->next->cft->query(cf->next, data, CF_QUERY_REMOTE_ADDR, வெற்று,
                            CURL_UNCONST(&remote)))
      திரும்பு CURLE_QUIC_CONNECT_ERROR;
    எனில்(!remote)
      திரும்பு CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, அளவு(ctx->q.local_addr));
    தேர்வு(remote->family) {
    நிலை AF_INET:
      ((கட்டமைப்பு sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = அளவு(கட்டமைப்பு sockaddr_in);
      நிறுத்து;
#ifdef USE_IPV6
    நிலை AF_INET6:
      ((கட்டமைப்பு sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = அளவு(கட்டமைப்பு sockaddr_in6);
      நிறுத்து;
#endif
    இயல்பு:
      திரும்பு CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (கட்டமைப்பு sockaddr *)&ctx->q.local_addr,
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
  எனில்(rc)
    திரும்பு CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_data = cf;

  result = Curl_vquic_tls_init(&ctx->tls, cf, data,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_setup, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  எனில்(result)
    திரும்பு result;

#எனில் defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  எனில்(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_ctx_new failed");
    திரும்பு CURLE_FAILED_INIT;
  }
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  எனில்(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_configure_client_session failed");
    திரும்பு CURLE_FAILED_INIT;
  }
#அல்லதெனில் defined(USE_OPENSSL)
  SSL_set_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#அல்லதெனில் defined(USE_GNUTLS)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#அல்லதெனில் defined(USE_WOLFSSL)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#இல்லையெனில்
#error "ngtcp2 TLS backend not defined"
#endif

#எனில் defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  எனில்(ctx->use_earlydata &&
     !SSL_set_quic_tls_early_data_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(data, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#endif
  ngtcp2_ccerr_default(&ctx->last_error);

  திரும்பு CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    bool *done)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு cf_call_data save;
  கட்டமைப்பு cf_ngtcp2_io_ctx pktx;

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  /* Connect the sub-chain */
  எனில்(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    எனில்(result || !*done)
      திரும்பு result;
  }

  *done = FALSE;

  எனில்(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    திரும்பு CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  CF_DATA_SAVE(save, cf, data);

  எனில்(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(data);
    result = cf_connect_start(cf, data, &pktx);
    எனில்(result)
      செல் out;
    எனில்(cf->connected) {
      *done = TRUE;
      செல் out;
    }
    result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
    /* we do not expect to be able to recv anything yet */
    செல் out;
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  எனில்(result)
    செல் out;

  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
  எனில்(result)
    செல் out;

  எனில்(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    result = ctx->tls_vrfy_result;
    எனில்(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      cf->connected = TRUE;
      *done = TRUE;
    }
  }

out:
  எனில்(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
  எனில்(ctx->qconn &&
     ((result == CURLE_RECV_ERROR) || (result == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_in_draining_period(ctx->qconn)) {
    மாறிலி ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    result = CURLE_COULDNT_CONNECT;
    எனில்(cerr) {
      CURL_TRC_CF(data, cf, "connect error, type=%d, code=%" PRIu64,
                  (முழு)cerr->type, cerr->error_code);
      தேர்வு(cerr->type) {
      நிலை NGTCP2_CCERR_TYPE_VERSION_NEGOTIATION:
        CURL_TRC_CF(data, cf, "error in version negotiation");
        நிறுத்து;
      இயல்பு:
        எனில்(cerr->error_code >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(data, cf, "crypto error, tls alert=%u",
                      (unsigned முழு)(cerr->error_code & 0xffU));
        }
        இல்லையெனில் எனில்(cerr->error_code == NGTCP2_CONNECTION_REFUSED) {
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
  எனில்(result) {
    எனில்(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      கட்டமைப்பு ip_quadruple ip;

      எனில்(!Curl_cf_socket_peek(cf->next, data, வெற்று, வெற்று, &ip))
        infof(data, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    }
  }
#endif
  எனில்(!result && ctx->qconn) {
    result = Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx);
  }
  எனில்(result || *done)
    CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (முழு)result, *done);
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                                     கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  கட்டமைப்பு cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  எனில்(cf->shutdown || !ctx->qconn) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  எனில்(!cf->next) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);
  *done = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  எனில்(!ctx->shutdown_started) {
    எழுத்து buffer[NGTCP2_MAX_UDP_PAYLOAD_SIZE];
    ngtcp2_ssize nwritten;

    எனில்(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
      CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
      result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
      எனில்(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
        CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
        result = CURLE_OK;
        செல் out;
      }
      இல்லையெனில் எனில்(result) {
        CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                    (முழு)result);
        *done = TRUE;
        செல் out;
      }
    }

    DEBUGASSERT(Curl_bufq_is_empty(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, வெற்று, /* path */
      வெற்று, /* pkt_info */
      (uint8_t *)buffer, அளவு(buffer),
      &ctx->last_error, pktx.ts);
    CURL_TRC_CF(data, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (முழு)ctx->last_error.type,
                ctx->last_error.error_code, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    எனில்(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      result = Curl_bufq_write(&ctx->q.sendbuf, (மாறிலி unsigned எழுத்து *)buffer,
                               (size_t)nwritten, &n);
      எனில்(result) {
        CURL_TRC_CF(data, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (முழு)result);
        செல் out;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.split_len = 0;
    }
  }

  எனில்(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing egress");
    result = Curl_vquic_flush(cf, data, &ctx->q);
    எனில்(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
      result = CURLE_OK;
      செல் out;
    }
    இல்லையெனில் எனில்(result) {
      CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                  (முழு)result);
      *done = TRUE;
      செல் out;
    }
  }

  எனில்(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
    result = CURLE_OK;
  }
out:
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

வெறுமை Curl_cf_ngtcp2_cmn_conn_close(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
  bool done;
  Curl_cf_ngtcp2_cmn_shutdown(cf, data, &done);
}

நிலையான bool cf_ngtcp2_err_is_fatal(முழு code)
{
  திரும்பு (NGTCP2_ERR_FATAL >= code) ||
         (NGTCP2_ERR_DROP_CONN == code) ||
         (NGTCP2_ERR_IDLE_CLOSE == code);
}

வெறுமை Curl_cf_ngtcp2_cmn_err_set(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data, முழு code)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  எனில்(!ctx->last_error.error_code) {
    எனில்(NGTCP2_ERR_CRYPTO == code) {
      ngtcp2_ccerr_set_tls_alert(&ctx->last_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 வெற்று, 0);
    }
    இல்லையெனில் {
      ngtcp2_ccerr_set_liberr(&ctx->last_error, code, வெற்று, 0);
    }
  }
  எனில்(cf_ngtcp2_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

வெறுமை Curl_cf_ngtcp2_io_ctx_init(கட்டமைப்பு cf_ngtcp2_io_ctx *io_ctx,
                                கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->data = data;
  io_ctx->now = *Curl_pgrs_now(data);
  ngtcp2_path_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_set_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

வெறுமை Curl_cf_ngtcp2_io_ctx_update_time(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு cf_ngtcp2_io_ctx *io_ctx,
                                       கட்டமைப்பு Curl_cfilter *cf)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(data);
  Curl_vquic_ctx_update_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#எனில் NGTCP2_VERSION_இலக்கம் < 0x011100
கட்டமைப்பு cf_ngtcp2_sfind_ctx {
  int64_t stream_id;
  கட்டமைப்பு h3_stream_ctx *stream;
  uint32_t mid;
};

நிலையான bool cf_ngtcp2_sfind(uint32_t mid, வெறுமை *மதிப்பு, வெறுமை *user_data)
{
  கட்டமைப்பு cf_ngtcp2_sfind_ctx *fctx = user_data;
  கட்டமைப்பு h3_stream_ctx *stream = மதிப்பு;

  எனில்(fctx->stream_id == stream->id) {
    fctx->mid = mid;
    fctx->stream = stream;
    திரும்பு FALSE;
  }
  திரும்பு TRUE; /* continue */
}

நிலையான கட்டமைப்பு h3_stream_ctx *cf_ngtcp2_get_stream(கட்டமைப்பு cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  கட்டமைப்பு cf_ngtcp2_sfind_ctx fctx;
  fctx.stream_id = stream_id;
  fctx.stream = வெற்று;
  Curl_u32_ptrset_visit(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  திரும்பு fctx.stream;
}
#இல்லையெனில்
நிலையான கட்டமைப்பு h3_stream_ctx *cf_ngtcp2_get_stream(கட்டமைப்பு cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  திரும்பு ngtcp2_conn_get_stream_user_data(ctx->qconn, stream_id);
}
#endif

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
நிலையான CURLcode read_pkt_to_send(வெறுமை *userp,
                                 unsigned எழுத்து *buf, size_t buflen,
                                 size_t *pnread)
{
  கட்டமைப்பு cf_ngtcp2_io_ctx *x = userp;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t stream_id;
  முழு fin;
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
  க்கு(;;) {

    எனில்(ctx->h3conn && ngtcp2_conn_get_max_data_இடது(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_stream(ctx->h3conn, &stream_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      எனில்(veccnt < 0) {
        failf(x->data, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((முழு)veccnt));
        Curl_cf_ngtcp2_h3_err_set(x->cf, x->data, (முழு)veccnt);
        திரும்பு CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_STREAM_FLAG_MORE |
            (fin ? NGTCP2_WRITE_STREAM_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_stream(ctx->qconn, &x->ps.path,
                                  வெற்று, buf, buflen,
                                  &ndatalen, flags, stream_id,
                                  (மாறிலி ngtcp2_vec *)vec, veccnt, x->ts);
    எனில்(n == 0) {
      /* nothing to send */
      திரும்பு CURLE_AGAIN;
    }
    இல்லையெனில் எனில்(n < 0) {
      தேர்வு(n) {
      நிலை NGTCP2_ERR_STREAM_DATA_BLOCKED: {
        கட்டமைப்பு h3_stream_ctx *stream;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_block_stream(ctx->h3conn, stream_id);
        CURL_TRC_CF(x->data, x->cf, "[%" PRId64 "] block quic flow",
                    stream_id);
        stream = cf_ngtcp2_get_stream(ctx, stream_id);
        எனில்(stream) /* it might be not one of our h3 streams? */
          stream->quic_flow_blocked = TRUE;
        n = 0;
        நிறுத்து;
      }
      நிலை NGTCP2_ERR_STREAM_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_stream_write(ctx->h3conn, stream_id);
        n = 0;
        நிறுத்து;
      நிலை NGTCP2_ERR_WRITE_MORE:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        நிறுத்து;
      இயல்பு:
        DEBUGASSERT(ndatalen == -1);
        failf(x->data, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((முழு)n));
        Curl_cf_ngtcp2_cmn_err_set(x->cf, x->data, (முழு)n);
        திரும்பு CURLE_SEND_ERROR;
      }
    }

    எனில்(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      முழு rv = nghttp3_conn_add_write_offset(ctx->h3conn, stream_id, ndatalen);
      எனில்(rv) {
        failf(x->data, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        திரும்பு CURLE_SEND_ERROR;
      }
    }

    எனில்(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      திரும்பு CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_progress_egress(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு cf_ngtcp2_io_ctx *pktx)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_size, path_max_payload_size;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_quantum;
  CURLcode result;
  கட்டமைப்பு cf_ngtcp2_io_ctx local_pktx;

  எனில்(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  இல்லையெனில் {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  எனில்(result) {
    எனில்(result == CURLE_AGAIN) {
      Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
      திரும்பு CURLE_OK;
    }
    திரும்பு result;
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
  க்கு(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_size,
                            read_pkt_to_send, pktx, &nread);
    எனில்(result == CURLE_AGAIN)
      நிறுத்து;
    இல்லையெனில் எனில்(result)
      திரும்பு result;
    இல்லையெனில் {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      எனில்((buflen >= send_quantum) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_size))
        நிறுத்து;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      எனில்(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      இல்லையெனில் எனில்(nread > gsolen ||
              (gsolen > path_max_payload_size && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        result = Curl_vquic_send_tail_split(cf, data, &ctx->q,
                                            gsolen, nread, nread);
        எனில்(result) {
          எனில்(result == CURLE_AGAIN) {
            Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
            திரும்பு CURLE_OK;
          }
          திரும்பு result;
        }
        pktcnt = 0;
      }
      இல்லையெனில் எனில்(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        நிறுத்து;
      }
    }
  }

  எனில்(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(data, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
    எனில்(result) {
      எனில்(result == CURLE_AGAIN) {
        Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
        திரும்பு CURLE_OK;
      }
      திரும்பு result;
    }
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_conn_update_pkt_tx_time(ctx->qconn, pktx->ts);
  }
  திரும்பு CURLE_OK;
}

கட்டமைப்பு cf_ngtcp2_recv_ctx {
  கட்டமைப்பு cf_ngtcp2_io_ctx *pktx;
  size_t pkt_மொத்தம்;
};

நிலையான CURLcode cf_ngtcp2_recv_pkts(மாறிலி unsigned எழுத்து *buf, size_t buflen,
                                    size_t gso_size,
                                    கட்டமைப்பு sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    வெறுமை *userp)
{
  கட்டமைப்பு cf_ngtcp2_recv_ctx *rctx = userp;
  கட்டமைப்பு cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_path path;
  size_t offset, pktlen;
  முழு rv;

  எனில்(!rctx->pkt_மொத்தம்) {
    Curl_cf_ngtcp2_io_ctx_update_time(pktx->data, pktx, pktx->cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  எனில்(ecn)
    CURL_TRC_CF(pktx->data, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_size, (unsigned)ecn);
  ngtcp2_addr_init(&path.local, (கட்டமைப்பு sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&path.remote, (கட்டமைப்பு sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  க்கு(offset = 0; offset < buflen; offset += gso_size) {
    rctx->pkt_மொத்தம்++;
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &path, &pi,
                              buf + offset, pktlen, pktx->ts);
    எனில்(rv) {
      CURL_TRC_CF(pktx->data, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_set(pktx->cf, pktx->data, rv);

      எனில்(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        திரும்பு CURLE_PEER_FAILED_VERIFICATION;
      திரும்பு CURLE_RECV_ERROR;
    }
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_progress_ingress(கட்டமைப்பு Curl_cfilter *cf,
                                         கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு cf_ngtcp2_io_ctx *pktx)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு cf_ngtcp2_io_ctx local_pktx;
  கட்டமைப்பு cf_ngtcp2_recv_ctx rctx;
  CURLcode result = CURLE_OK;

  எனில்(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }

  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  எனில்(result)
    திரும்பு result;

  rctx.pktx = pktx;
  rctx.pkt_மொத்தம் = 0;

  எனில்(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(data, cf, "progress_ingress(socket)");
    திரும்பு Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  இல்லையெனில் {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    unsigned எழுத்து *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_SIZE;
    size_t pkt_limit = QUIC_TUNNEL_INGRESS_PKT_LIMIT;
    size_t nread;
    கட்டமைப்பு sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(data, cf, "progress_ingress(sub-filters)");
    எனில்(ctx->qconn) {
      size_t max_path_payload;
      max_path_payload =
        ngtcp2_conn_get_path_max_tx_udp_payload_size(ctx->qconn);
      எனில்(max_path_payload > max_udp_payload)
        max_udp_payload = max_path_payload;
    }

    எனில்(ctx->tunnel_inbuf_len < max_udp_payload) {
      unsigned எழுத்து *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      எனில்(!newbuf)
        திரும்பு CURLE_OUT_OF_MEMORY;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    போது(pkt_limit--) {
      result = Curl_conn_cf_recv(cf->next, data, (எழுத்து *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      எனில்(result == CURLE_AGAIN) {
        /* no more data available at the moment */
        திரும்பு CURLE_OK;
      }
      எனில்(result) {
        CURL_TRC_CF(data, cf, "ingress, recv from tunnel failed: %d",
                    (முழு)result);
        திரும்பு result;
      }
      எனில்(nread == 0) {
        /* tunnel closed */
        திரும்பு CURLE_OK;
      }

      memcpy(&remote_addr, ctx->connected_path.remote.addr,
             ctx->connected_path.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_path.remote.addrlen;
      result = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      எனில்(result)
        திரும்பு result;

      எனில்(!ctx->q.got_first_byte) {
        ctx->q.got_first_byte = TRUE;
        ctx->q.first_byte_at = ctx->q.last_op;
      }
      ctx->q.last_io = ctx->q.last_op;
    }
    திரும்பு CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_set_expiry(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு cf_ngtcp2_io_ctx *pktx)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  எனில்(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  இல்லையெனில் {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  எனில்(expiry != UINT64_MAX) {
    எனில்(expiry <= pktx->ts) {
      CURLcode result;
      முழு rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      எனில்(rv) {
        failf(data, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_set(cf, data, rv);
        திரும்பு CURLE_SEND_ERROR;
      }
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, pktx);
      எனில்(!result)
        result = Curl_cf_ngtcp2_progress_egress(cf, data, pktx);
      எனில்(result) {
        /* a verify failure during ingress must win over generic errors */
        எனில்(ctx->tls_vrfy_result)
          result = ctx->tls_vrfy_result;
        திரும்பு result;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    எனில்(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      எனில்(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_set(data, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cf_ngtcp2_setup_keep_alive(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  மாறிலி ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  எனில்(!ctx->qconn)
    திரும்பு;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  எனில்(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no peer idle timeout, unset keep-alive");
  }
  இல்லையெனில் எனில்(!Curl_u32_ptrset_மொத்தம்(&ctx->streams)) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no active streams, unset keep-alive");
  }
  இல்லையெனில் {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(data, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_stream_setup(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு h3_stream_ctx **pstream)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  *pstream = வெற்று;
  எனில்(!data)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(stream) {
    *pstream = stream;
    திரும்பு CURLE_OK;
  }

  stream = curlx_calloc(1, அளவு(*stream));
  எனில்(!stream)
    திரும்பு CURLE_OUT_OF_MEMORY;

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
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_வரி_LEN);

  எனில்(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    Curl_cf_ngtcp2_h3_stream_ctx_free(stream);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  எனில்(Curl_u32_ptrset_மொத்தம்(&ctx->streams) == 1)
    cf_ngtcp2_setup_keep_alive(cf, data);

  *pstream = stream;
  திரும்பு CURLE_OK;
}

வெறுமை Curl_cf_ngtcp2_h3_stream_close(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு h3_stream_ctx *stream)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(data);
  DEBUGASSERT(stream);
  எனில்(!stream->closed && ctx->qconn && ctx->h3conn) {
    CURLcode result;

    nghttp3_conn_set_stream_user_data(ctx->h3conn, stream->id, வெற்று);
    ngtcp2_conn_set_stream_user_data(ctx->qconn, stream->id, வெற்று);
    stream->closed = TRUE;
    (வெறுமை)ngtcp2_conn_shutdown_stream(ctx->qconn, 0, stream->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    result = Curl_cf_ngtcp2_progress_egress(cf, data, வெற்று);
    எனில்(result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cancel stream -> %d",
                  stream->id, (முழு)result);
  }
}

வெறுமை Curl_cf_ngtcp2_h3_stream_done(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  (வெறுமை)cf;
  எனில்(stream) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] easy handle is done", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
    எனில்(!Curl_u32_ptrset_மொத்தம்(&ctx->streams))
      cf_ngtcp2_setup_keep_alive(cf, data);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      bool *உள்ளீடு_pending)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  மாறிலி ngtcp2_transport_params *rp;
  கட்டமைப்பு cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  *உள்ளீடு_pending = FALSE;
  எனில்(!ctx->qconn || ctx->shutdown_started)
    செல் out;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  எனில்(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->q.last_io);
    எனில்(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      எனில்((uint64_t)idletime_ms > max_idle_ms)
        செல் out;
    }
  }

  எனில்(!cf->next || !cf->next->cft->is_alive(cf->next, data, உள்ளீடு_pending))
    செல் out;

  alive = TRUE;
  எனில்(*உள்ளீடு_pending) {
    CURLcode result;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *உள்ளீடு_pending = FALSE;
    result = Curl_cf_ngtcp2_progress_ingress(cf, data, வெற்று);
    CURL_TRC_CF(data, cf, "is_alive, progress ingress -> %d", (முழு)result);
    alive = result ? FALSE : TRUE;
  }

out:
  CF_DATA_RESTORE(cf, save);
  திரும்பு alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(கட்டமைப்பு cf_ngtcp2_ctx *ctx,
                                      கட்டமைப்பு Curl_easy *data)
{
  int64_t ctrl_stream_id, qpack_enc_stream_id, qpack_dec_stream_id;
  முழு rc;

  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &ctrl_stream_id, வெற்று);
  எனில்(rc) {
    failf(data, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    திரும்பு CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_stream(ctx->h3conn, ctrl_stream_id);
  எனில்(rc) {
    failf(data, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    திரும்பு CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_enc_stream_id, வெற்று);
  எனில்(rc) {
    failf(data, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    திரும்பு CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_dec_stream_id, வெற்று);
  எனில்(rc) {
    failf(data, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    திரும்பு CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_stream_id,
                                       qpack_dec_stream_id);
  எனில்(rc) {
    failf(data, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    திரும்பு CURLE_QUIC_CONNECT_ERROR;
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  முழு query, முழு *pres1, வெறுமை *pres2)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;

  தேர்வு(query) {
  நிலை CF_QUERY_CONNECT_REPLY_MS:
    எனில்((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (முழு)ms : INT_MAX;
      திரும்பு CURLE_OK;
    }
    நிறுத்து;
  நிலை CF_QUERY_REALLY_CONNECTED:
    எனில்(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_first_byte;
      திரும்பு CURLE_OK;
    }
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  முழு event, முழு arg1, வெறுமை *arg2)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (வெறுமை)arg1;
  (வெறுமை)arg2;
  தேர்வு(event) {
  நிலை CF_CTRL_REPORT_STATS:
    எனில்(cf->connected && !ctx->stats_reported) {
      எனில்((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        ctx->stats_reported = TRUE;
      }
      இல்லையெனில் எனில்(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        எனில்(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
