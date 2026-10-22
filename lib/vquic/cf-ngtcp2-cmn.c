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

#ifdef USE_OPENSSL
#شامل <openssl/err.h>
#اگر defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#شامل <ngtcp2/ngtcp2_crypto_boringssl.h>
#وگرنهاگر defined(OPENSSL_QUIC_API2)
#شامل <ngtcp2/ngtcp2_crypto_ossl.h>
#وگرنه
#شامل <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#شامل "vtls/openssl.h"
#وگرنهاگر defined(USE_GNUTLS)
#شامل <ngtcp2/ngtcp2_crypto_gnutls.h>
#شامل "vtls/gtls.h"
#وگرنهاگر defined(USE_WOLFSSL)
#شامل <ngtcp2/ngtcp2_crypto_wolfssl.h>
#شامل "vtls/wolfssl.h"
#endif

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
تهی Curl_ngtcp2_ver(نویسه *p, size_t len)
{
  ثابت ngtcp2_info *ng2 = ngtcp2_version(0);
  ثابت nghttp3_info *ht3 = nghttp3_version(0);
  (تهی)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->version_str, ht3->version_str);
}

تهی Curl_cf_ngtcp2_h3_stream_ctx_free(ساختار h3_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

ایستا تهی h3_stream_set_free(uint32_t id, تهی *stream)
{
  (تهی)id;
  DEBUGASSERT(stream);
  Curl_cf_ngtcp2_h3_stream_ctx_free((ساختار h3_stream_ctx *)stream);
}

ایستا bool cf_ngtcp2_h3_err_is_fatal(صحیح code)
{
  بازگشت (NGHTTP3_ERR_FATAL >= code) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM == code);
}

تهی Curl_cf_ngtcp2_h3_err_set(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data, صحیح code)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  اگر(!ctx->last_error.error_code) {
    ngtcp2_ccerr_set_application_error(&ctx->last_error,
      nghttp3_err_infer_quic_app_error_code(code), پوچ, 0);
  }
  اگر(cf_ngtcp2_h3_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

CURLcode Curl_cf_ngtcp2_ctx_init(ساختار cf_ngtcp2_ctx *ctx,
                                 ساختار Curl_peer *origin,
                                 ساختار Curl_peer *peer,
                                 ساختار ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = پوچ;
  ctx->tunnel_inbuf_len = 0;
  ctx->version = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->stream_bufcp, H3_STREAM_CHUNK_SIZE,
                  H3_STREAM_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  بازگشت Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

تهی Curl_cf_ngtcp2_ctx_cleanup(ساختار cf_ngtcp2_ctx *ctx)
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
      ctx->h3conn = پوچ;
    }
    اگر(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = پوچ;
    }
#ifdef OPENSSL_QUIC_API2
    اگر(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = پوچ;
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

ساختار Curl_easy *Curl_cf_ngtcp2_get_xfer(ساختار Curl_cfilter *cf,
                                          تهی *stream_user_data,
                                          ساختار h3_stream_ctx **pstream)
{
  ساختار h3_stream_ctx *stream = stream_user_data;

  اگر(pstream)
    *pstream = stream;
  اگر(stream && (stream->mid != UINT32_MAX)) {
    ساختار Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    اگر(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      بازگشت پوچ;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    اگر(data && (data->id == stream->xfer_id))
      بازگشت data;
    DEBUGASSERT(0);
  }
  بازگشت پوچ;
}

ایستا ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  ساختار Curl_cfilter *cf = conn_ref->user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  بازگشت ctx->qconn;
}

#ifdef DEBUG_NGTCP2
ایستا تهی quic_printf(تهی *user_data, ثابت نویسه *fmt, ...)
{
  va_list ap;
  (تهی)user_data;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#endif

ایستا تهی qlog_callback(تهی *user_data, uint32_t flags,
                          ثابت تهی *data, size_t datalen)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  (تهی)flags;
  اگر(ctx->qlogfd != -1) {
    ssize_t rc = اكتب(ctx->qlogfd, data, datalen);
    اگر(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

ایستا تهی quic_settings(ساختار cf_ngtcp2_ctx *ctx,
                          ساختار Curl_easy *data,
                          ساختار cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#ifdef DEBUG_NGTCP2
  s->log_printf = quic_printf;
#وگرنه
  s->log_printf = پوچ;
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
  اگر(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#اگر defined(_MSC_VER) && defined(_DLL)
#pragma warning(افزودن)
#pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

ایستا صحیح cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, تهی *user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf ? cf->ctx : پوچ;
  ساختار Curl_easy *data;

  (تهی)tconn;
  DEBUGASSERT(ctx);
  data = CF_DATA_CURRENT(cf);
  DEBUGASSERT(data);
  اگر(!ctx || !data)
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(data);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, data);

  ctx->tls_vrfy_result = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    data, &ctx->ssl_peer);
  اگر(ctx->tls_vrfy_result)
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;

#ifdef CURLVERBOSE
  اگر(Curl_trc_is_verbose(data)) {
    ثابت ngtcp2_transport_params *rp;
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
  اگر(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  اگر(ctx->use_earlydata) {
#اگر defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    اگر(SSL_get_early_data_status(ctx->tls.ossl.ssl) == SSL_EARLY_DATA_NOT_SENT)
      CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
#endif

#اگر NGTCP2_VERSION_رقم >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_data_rejected2(ctx->qconn);
#وگرنه /* older NGTCP2 */
    {
#اگر defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      صحیح ossl_early_status = SSL_get_early_data_status(ctx->tls.ossl.ssl);
      اگر(ossl_early_status == SSL_EARLY_DATA_NOT_SENT)
        CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_status == SSL_EARLY_DATA_ACCEPTED);
#وگرنهاگر defined(USE_GNUTLS)
      صحیح flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_DATA);
#وگرنهاگر defined(USE_WOLFSSL)
#ifdef WOLFSSL_EARLY_DATA
      ctx->earlydata_accepted =
        (wolfSSL_get_early_data_status(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_DATA_REJECTED);
#وگرنه
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
  بازگشت 0;
}

ایستا صحیح cb_recv_stream_data(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t stream_id, uint64_t offset,
                               ثابت uint8_t *buf, size_t buflen,
                               تهی *user_data, تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  صحیح fin = (flags & NGTCP2_STREAM_DATA_FLAG_FIN) ? 1 : 0;
  ساختار h3_stream_ctx *stream;
  ساختار Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (تهی)offset;
  rc = nghttp3_conn_read_stream(ctx->h3conn, stream_id, buf, buflen, fin);
  اگر(rc < 0) {
    اگر(data && stream) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  stream_id, stream->reset, stream->closed);
    }
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  اگر(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_stream_offset(tconn, stream_id, nconsumed);
    ngtcp2_conn_extend_max_offset(tconn, nconsumed);
    اگر(stream) {
      stream->rx_offset += nconsumed;
      stream->rx_offset_max += nconsumed;
    }
  }
  بازگشت 0;
}

ایستا صحیح cb_acked_stream_data_offset(ngtcp2_conn *tconn, int64_t stream_id,
                                       uint64_t offset, uint64_t datalen,
                                       تهی *user_data, تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  صحیح rv;
  (تهی)stream_id;
  (تهی)tconn;
  (تهی)offset;
  (تهی)datalen;
  (تهی)stream_user_data;

  rv = nghttp3_conn_add_ack_offset(ctx->h3conn, stream_id, datalen);
  اگر(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;
  }

  بازگشت 0;
}

ایستا صحیح cb_stream_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id, uint64_t app_error_code,
                           تهی *user_data, تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, پوچ);
  صحیح rv;

  (تهی)tconn;
  /* stream is closed... */
  اگر(!data)
    data = CF_DATA_CURRENT(cf);
  اگر(!data)
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;

  اگر(!(flags & NGTCP2_STREAM_CLOSE_FLAG_APP_ERROR_CODE_SET)) {
    app_error_code = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, app_error_code, rv);
  اگر(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;
  }

  بازگشت 0;
}

#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
ایستا صحیح cb_stream_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id,
                           uint64_t rx_app_error_code,
                           uint64_t tx_app_error_code,
                           تهی *user_data, تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, پوچ);
  uint64_t h3_app_error_code = NGHTTP3_H3_NO_ERROR;
  صحیح rv;

  (تهی)tconn;
  (تهی)tx_app_error_code;
  /* stream is closed... */
  اگر(!data)
    data = CF_DATA_CURRENT(cf);
  اگر(!data)
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;

  اگر(flags & NGTCP2_STREAM_CLOSE2_FLAG_RX_APP_ERROR_CODE_SET)
    h3_app_error_code = rx_app_error_code;

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, h3_app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, h3_app_error_code, rv);
  اگر(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;
  }

  بازگشت 0;
}
#endif

ایستا صحیح cb_stream_reset(ngtcp2_conn *tconn, int64_t stream_id,
                           uint64_t final_size, uint64_t app_error_code,
                           تهی *user_data, تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, پوچ);
  صحیح rv;
  (تهی)tconn;
  (تهی)final_size;
  (تهی)app_error_code;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  اگر(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  اگر(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;
  }

  بازگشت 0;
}

ایستا صحیح cb_stream_stop_sending(ngtcp2_conn *tconn, int64_t stream_id,
                                  uint64_t app_error_code, تهی *user_data,
                                  تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  صحیح rv;
  (تهی)tconn;
  (تهی)app_error_code;
  (تهی)stream_user_data;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  اگر(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;
  }

  بازگشت 0;
}

ایستا صحیح cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            تهی *user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);

  (تهی)tconn;
  ctx->max_bidi_streams = max_streams;
  اگر(data)
    CURL_TRC_CF(data, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->used_bidi_streams);
  بازگشت 0;
}

ایستا صحیح cb_extend_max_stream_data(ngtcp2_conn *tconn, int64_t stream_id,
                                     uint64_t max_data, تهی *user_data,
                                     تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream;
  ساختار Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  صحیح rv;
  (تهی)tconn;
  (تهی)max_data;

  rv = nghttp3_conn_unblock_stream(ctx->h3conn, stream_id);
  اگر(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;
  }
  اگر(stream && stream->quic_flow_blocked) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] unblock quic flow", stream_id);
    stream->quic_flow_blocked = FALSE;
    اگر(data)
      Curl_multi_mark_dirty(data);
  }
  بازگشت 0;
}

ایستا تهی cb_rand(uint8_t *dest, size_t destlen,
                    ثابت ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode result;
  (تهی)rand_ctx;

  result = Curl_rand(پوچ, dest, destlen);
  اگر(result) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
ایستا صحیح cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    تهی *user_data)
{
  CURLcode result;
  (تهی)tconn;
  (تهی)user_data;

  result = Curl_rand(پوچ, cid->data, cidlen);
  اگر(result)
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(پوچ, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  اگر(result)
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;

  بازگشت 0;
}

#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
ایستا صحیح cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  ساختار ngtcp2_stateless_reset_token *token, size_t cidlen, تهی *user_data)
{
  CURLcode result;
  (تهی)tconn;
  (تهی)user_data;

  result = Curl_rand(پوچ, cid->data, cidlen);
  اگر(result)
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(پوچ, token->data, اندازه(token->data));
  اگر(result)
    بازگشت NGTCP2_ERR_CALLBACK_FAILURE;

  بازگشت 0;
}
#endif

ایستا صحیح cb_recv_rx_کلید(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          تهی *user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf ? cf->ctx : پوچ;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
  (تهی)tconn;

  اگر(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    بازگشت 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(data);
  اگر(ctx && data && !ctx->h3conn && ctx->init_h3_conn_cb) {
    اگر(ctx->init_h3_conn_cb(cf, data, ctx))
      بازگشت NGTCP2_ERR_CALLBACK_FAILURE;
  }
  بازگشت 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
ایستا ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  پوچ, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_data_cb,
  cb_ngtcp2_handshake_completed,
  پوچ, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_mask_cb,
  cb_recv_stream_data,
  cb_acked_stream_data_offset,
  پوچ, /* stream_open */
  cb_stream_close,
  پوچ, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  پوچ, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  پوچ, /* remove_connection_id */
  ngtcp2_crypto_update_کلید_cb, /* update_key */
  پوچ, /* path_validation */
  پوچ, /* select_preferred_addr */
  cb_stream_reset,
  پوچ, /* extend_max_remote_streams_bidi */
  پوچ, /* extend_max_remote_streams_uni */
  cb_extend_max_stream_data,
  پوچ, /* dcid_status */
  پوچ, /* handshake_confirmed */
  پوچ, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  پوچ, /* recv_datagram */
  پوچ, /* ack_datagram */
  پوچ, /* lost_datagram */
  ngtcp2_crypto_get_path_challenge_data_cb,
  cb_stream_stop_sending,
  پوچ, /* version_negotiation */
  cb_recv_rx_کلید,
  پوچ, /* recv_tx_key */
  پوچ, /* early_data_rejected */
#ifdef NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  پوچ, /* begin_path_validation */
#endif
#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  پوچ, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  پوچ, /* dcid_status2 */
  ngtcp2_crypto_get_path_challenge_data2_cb, /* get_path_challenge_data2 */
#endif
#ifdef NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  پوچ, /* recv_stop_sending */
#endif
#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_stream_close2, /* is called instead of cb_stream_close when set */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic برداشت
#endif

#اگر defined(_MSC_VER) && defined(_DLL)
#pragma warning(برداشت)
#endif

#ifdef USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ایستا صحیح quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ساختار Curl_cfilter *cf;
  ساختار cf_ngtcp2_ctx *ctx;
  ساختار Curl_easy *data;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_data(ssl);
  cf = cref ? cref->user_data : پوچ;
  ctx = cf ? cf->ctx : پوچ;
  data = cf ? CF_DATA_CURRENT(cf) : پوچ;
  اگر(cf && data && ctx) {
    unsigned نویسه *quic_tp = پوچ;
    size_t quic_tp_len = 0;
#ifdef HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     اندازه(tpbuf));
    اگر(tplen < 0)
      CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((صحیح)tplen));
    وگرنه {
      quic_tp = (unsigned نویسه *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#endif
    Curl_ossl_add_session(cf, data, &ctx->tls.ossl, ctx->ssl_peer.scache_کلید,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, پوچ);
  }
  بازگشت 0;
}
#endif /* USE_OPENSSL */

#ifdef USE_GNUTLS

#ifdef CURLVERBOSE
ایستا ثابت نویسه *gtls_hs_پیام_name(صحیح mtype)
{
  انتخاب(mtype) {
  حالت 1:
    بازگشت "ClientHello";
  حالت 2:
    بازگشت "ServerHello";
  حالت 4:
    بازگشت "SessionTicket";
  حالت 8:
    بازگشت "EncryptedExtensions";
  حالت 11:
    بازگشت "Certificate";
  حالت 13:
    بازگشت "CertificateRequest";
  حالت 15:
    بازگشت "CertificateVerify";
  حالت 20:
    بازگشت "Finished";
  حالت 24:
    بازگشت "KeyUpdate";
  حالت 254:
    بازگشت "MessageHash";
  }
  بازگشت "Unknown";
}
#endif

ایستا صحیح quic_gtls_handshake_cb(gnutls_session_t session, unsigned صحیح htype,
                                  unsigned when, unsigned صحیح incoming,
                                  ثابت gnutls_datum_t *پیام)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  ساختار Curl_cfilter *cf = conn_ref ? conn_ref->user_data : پوچ;
  ساختار cf_ngtcp2_ctx *ctx = cf ? cf->ctx : پوچ;

  (تهی)پیام;
  (تهی)incoming;
  اگر(when && cf && ctx) { /* after message has been processed */
    ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    اگر(!data)
      بازگشت 0;
    CURL_TRC_CF(data, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_پیام_name(htype), htype);
    انتخاب(htype) {
    حالت GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned نویسه *quic_tp = پوچ;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       اندازه(tpbuf));
      اگر(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحیح)tplen));
      وگرنه {
        quic_tp = (unsigned نویسه *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (تهی)Curl_gtls_cache_session(cf, data, ctx->ssl_peer.scache_کلید,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    پوچ);
      بشکن;
    }
    پیشفرض:
      بشکن;
    }
  }
  بازگشت 0;
}
#endif /* USE_GNUTLS */

#ifdef USE_WOLFSSL
ایستا صحیح wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolfSSL_get_app_data(ssl);
  ساختار Curl_cfilter *cf = conn_ref ? conn_ref->user_data : پوچ;

  DEBUGASSERT(cf);
  اگر(cf && session) {
    ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
    ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    اگر(data && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned نویسه *quic_tp = پوچ;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       اندازه(tpbuf));
      اگر(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحیح)tplen));
      وگرنه {
        quic_tp = (unsigned نویسه *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (تهی)Curl_wssl_cache_session(cf, data, ctx->ssl_peer.scache_کلید,
                                    session, wolfSSL_version(ssl),
                                    "h3", quic_tp, quic_tp_len, پوچ);
    }
  }
  بازگشت 0;
}
#endif /* USE_WOLFSSL */

ایستا CURLcode cf_ngtcp2_tls_ctx_setup(ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data,
                                        تهی *user_data)
{
  ساختار curl_tls_ctx *ctx = user_data;

#ifdef USE_OPENSSL
#اگر defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  اگر(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(data, "ngtcp2_crypto_boringssl_configure_client_context failed");
    بازگشت CURLE_FAILED_INIT;
  }
#وگرنهاگر defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#وگرنه
  اگر(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_quictls_configure_client_context failed");
    بازگشت CURLE_FAILED_INIT;
  }
#endif /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  اگر(Curl_ssl_scache_use(cf, data)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_set_session_cache_mode(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#وگرنهاگر defined(USE_GNUTLS)
  اگر(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(data, "ngtcp2_crypto_gnutls_configure_client_session failed");
    بازگشت CURLE_FAILED_INIT;
  }
  اگر(Curl_ssl_scache_use(cf, data)) {
    gnutls_handshake_set_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#وگرنهاگر defined(USE_WOLFSSL)
  اگر(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    بازگشت CURLE_FAILED_INIT;
  }
  اگر(Curl_ssl_scache_use(cf, data)) {
    /* Register to get notified when a new session is received */
    wolfSSL_CTX_sess_set_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#endif
  بازگشت CURLE_OK;
}

ایستا CURLcode cf_ngtcp2_on_session_reuse(ساختار Curl_cfilter *cf,
                                           ساختار Curl_easy *data,
                                           ساختار alpn_spec *alpns,
                                           ساختار Curl_ssl_session *scs,
                                           bool *do_early_data)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;
#اگر defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#endif
#ifdef USE_GNUTLS
  ctx->earlydata_max =
    gnutls_record_get_max_early_data_size(ctx->tls.gtls.session);
#endif
#ifdef USE_WOLFSSL
#ifdef WOLFSSL_EARLY_DATA
  ctx->earlydata_max = scs->earlydata_max;
#وگرنه
  ctx->earlydata_max = 0;
#endif /* WOLFSSL_EARLY_DATA */
#endif
#اگر defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  اگر(!ctx->earlydata_max) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  وگرنه اگر(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session from different ALPN, no early data");
  }
  وگرنه اگر(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(data, cf, "no 0RTT transport parameters, no early data");
  }
  وگرنه {
    صحیح rv;
    rv = ngtcp2_conn_decode_and_set_0rtt_transport_params(
      ctx->qconn, (ثابت uint8_t *)scs->quic_tp, scs->quic_tp_len);
    اگر(rv)
      CURL_TRC_CF(data, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    وگرنه اگر(ctx->init_h3_conn_cb) {
      infof(data, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      result = ctx->init_h3_conn_cb(cf, data, ctx);
      اگر(!result) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_data = TRUE;
      }
    }
    وگرنه { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_data = TRUE;
    }
  }
#وگرنه /* not supported in the TLS backend */
  (تهی)data;
  (تهی)ctx;
  (تهی)scs;
  (تهی)alpns;
#endif
  بازگشت result;
}

/*
 * Might be called twice for happy eyeballs.
 */
ایستا CURLcode cf_connect_start(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار cf_ngtcp2_io_ctx *pktx)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  صحیح rc;
  صحیح rv;
  CURLcode result;
  ثابت ساختار Curl_sockaddr_ex *sockaddr = پوچ;
  صحیح qfd;
  ایستا ثابت ساختار alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->dcid.data, NGTCP2_MAX_CIDLEN);
  اگر(result)
    بازگشت result;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->scid.data, NGTCP2_MAX_CIDLEN);
  اگر(result)
    بازگشت result;

  (تهی)Curl_qlogdir(data, ctx->scid.data, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, data, pktx);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  اگر(result)
    بازگشت result;

  /* Query socket and remote address from sub-chain */
  اگر(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, پوچ)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  اگر(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = اندازه(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (ساختار sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    اگر(rv == -1)
      بازگشت CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (ساختار sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_path.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  وگرنه {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ثابت ساختار Curl_sockaddr_ex *remote = پوچ;
    اگر(cf->next->cft->query(cf->next, data, CF_QUERY_REMOTE_ADDR, پوچ,
                            CURL_UNCONST(&remote)))
      بازگشت CURLE_QUIC_CONNECT_ERROR;
    اگر(!remote)
      بازگشت CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, اندازه(ctx->q.local_addr));
    انتخاب(remote->family) {
    حالت AF_INET:
      ((ساختار sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = اندازه(ساختار sockaddr_in);
      بشکن;
#ifdef USE_IPV6
    حالت AF_INET6:
      ((ساختار sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = اندازه(ساختار sockaddr_in6);
      بشکن;
#endif
    پیشفرض:
      بازگشت CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (ساختار sockaddr *)&ctx->q.local_addr,
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
  اگر(rc)
    بازگشت CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_data = cf;

  result = Curl_vquic_tls_init(&ctx->tls, cf, data,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_setup, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  اگر(result)
    بازگشت result;

#اگر defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  اگر(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_ctx_new failed");
    بازگشت CURLE_FAILED_INIT;
  }
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  اگر(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_configure_client_session failed");
    بازگشت CURLE_FAILED_INIT;
  }
#وگرنهاگر defined(USE_OPENSSL)
  SSL_set_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#وگرنهاگر defined(USE_GNUTLS)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#وگرنهاگر defined(USE_WOLFSSL)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#وگرنه
#error "ngtcp2 TLS backend not defined"
#endif

#اگر defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  اگر(ctx->use_earlydata &&
     !SSL_set_quic_tls_early_data_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(data, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#endif
  ngtcp2_ccerr_default(&ctx->last_error);

  بازگشت CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    bool *done)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ساختار cf_call_data save;
  ساختار cf_ngtcp2_io_ctx pktx;

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  /* Connect the sub-chain */
  اگر(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    اگر(result || !*done)
      بازگشت result;
  }

  *done = FALSE;

  اگر(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    بازگشت CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  CF_DATA_SAVE(save, cf, data);

  اگر(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(data);
    result = cf_connect_start(cf, data, &pktx);
    اگر(result)
      برو out;
    اگر(cf->connected) {
      *done = TRUE;
      برو out;
    }
    result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
    /* we do not expect to be able to recv anything yet */
    برو out;
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  اگر(result)
    برو out;

  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
  اگر(result)
    برو out;

  اگر(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    result = ctx->tls_vrfy_result;
    اگر(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      cf->connected = TRUE;
      *done = TRUE;
    }
  }

out:
  اگر(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
  اگر(ctx->qconn &&
     ((result == CURLE_RECV_ERROR) || (result == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_in_draining_period(ctx->qconn)) {
    ثابت ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    result = CURLE_COULDNT_CONNECT;
    اگر(cerr) {
      CURL_TRC_CF(data, cf, "connect error, type=%d, code=%" PRIu64,
                  (صحیح)cerr->type, cerr->error_code);
      انتخاب(cerr->type) {
      حالت NGTCP2_CCERR_TYPE_VERSION_NEGOTIATION:
        CURL_TRC_CF(data, cf, "error in version negotiation");
        بشکن;
      پیشفرض:
        اگر(cerr->error_code >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(data, cf, "crypto error, tls alert=%u",
                      (unsigned صحیح)(cerr->error_code & 0xffU));
        }
        وگرنه اگر(cerr->error_code == NGTCP2_CONNECTION_REFUSED) {
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
  اگر(result) {
    اگر(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      ساختار ip_quadruple ip;

      اگر(!Curl_cf_socket_peek(cf->next, data, پوچ, پوچ, &ip))
        infof(data, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    }
  }
#endif
  اگر(!result && ctx->qconn) {
    result = Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx);
  }
  اگر(result || *done)
    CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (صحیح)result, *done);
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(ساختار Curl_cfilter *cf,
                                     ساختار Curl_easy *data, bool *done)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار cf_call_data save;
  ساختار cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  اگر(cf->shutdown || !ctx->qconn) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  اگر(!cf->next) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);
  *done = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  اگر(!ctx->shutdown_started) {
    نویسه buffer[NGTCP2_MAX_UDP_PAYLOAD_SIZE];
    ngtcp2_ssize nwritten;

    اگر(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
      CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
      result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
      اگر(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
        CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
        result = CURLE_OK;
        برو out;
      }
      وگرنه اگر(result) {
        CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                    (صحیح)result);
        *done = TRUE;
        برو out;
      }
    }

    DEBUGASSERT(Curl_bufq_is_empty(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, پوچ, /* path */
      پوچ, /* pkt_info */
      (uint8_t *)buffer, اندازه(buffer),
      &ctx->last_error, pktx.ts);
    CURL_TRC_CF(data, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (صحیح)ctx->last_error.type,
                ctx->last_error.error_code, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    اگر(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      result = Curl_bufq_write(&ctx->q.sendbuf, (ثابت unsigned نویسه *)buffer,
                               (size_t)nwritten, &n);
      اگر(result) {
        CURL_TRC_CF(data, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (صحیح)result);
        برو out;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.split_len = 0;
    }
  }

  اگر(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing egress");
    result = Curl_vquic_flush(cf, data, &ctx->q);
    اگر(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
      result = CURLE_OK;
      برو out;
    }
    وگرنه اگر(result) {
      CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                  (صحیح)result);
      *done = TRUE;
      برو out;
    }
  }

  اگر(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
    result = CURLE_OK;
  }
out:
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

تهی Curl_cf_ngtcp2_cmn_conn_close(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  bool done;
  Curl_cf_ngtcp2_cmn_shutdown(cf, data, &done);
}

ایستا bool cf_ngtcp2_err_is_fatal(صحیح code)
{
  بازگشت (NGTCP2_ERR_FATAL >= code) ||
         (NGTCP2_ERR_DROP_CONN == code) ||
         (NGTCP2_ERR_IDLE_CLOSE == code);
}

تهی Curl_cf_ngtcp2_cmn_err_set(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data, صحیح code)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  اگر(!ctx->last_error.error_code) {
    اگر(NGTCP2_ERR_CRYPTO == code) {
      ngtcp2_ccerr_set_tls_alert(&ctx->last_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 پوچ, 0);
    }
    وگرنه {
      ngtcp2_ccerr_set_liberr(&ctx->last_error, code, پوچ, 0);
    }
  }
  اگر(cf_ngtcp2_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

تهی Curl_cf_ngtcp2_io_ctx_init(ساختار cf_ngtcp2_io_ctx *io_ctx,
                                ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->data = data;
  io_ctx->now = *Curl_pgrs_now(data);
  ngtcp2_path_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_set_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

تهی Curl_cf_ngtcp2_io_ctx_update_time(ساختار Curl_easy *data,
                                       ساختار cf_ngtcp2_io_ctx *io_ctx,
                                       ساختار Curl_cfilter *cf)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(data);
  Curl_vquic_ctx_update_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#اگر NGTCP2_VERSION_رقم < 0x011100
ساختار cf_ngtcp2_sfind_ctx {
  int64_t stream_id;
  ساختار h3_stream_ctx *stream;
  uint32_t mid;
};

ایستا bool cf_ngtcp2_sfind(uint32_t mid, تهی *مقدار, تهی *user_data)
{
  ساختار cf_ngtcp2_sfind_ctx *fctx = user_data;
  ساختار h3_stream_ctx *stream = مقدار;

  اگر(fctx->stream_id == stream->id) {
    fctx->mid = mid;
    fctx->stream = stream;
    بازگشت FALSE;
  }
  بازگشت TRUE; /* continue */
}

ایستا ساختار h3_stream_ctx *cf_ngtcp2_get_stream(ساختار cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  ساختار cf_ngtcp2_sfind_ctx fctx;
  fctx.stream_id = stream_id;
  fctx.stream = پوچ;
  Curl_u32_ptrset_visit(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  بازگشت fctx.stream;
}
#وگرنه
ایستا ساختار h3_stream_ctx *cf_ngtcp2_get_stream(ساختار cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  بازگشت ngtcp2_conn_get_stream_user_data(ctx->qconn, stream_id);
}
#endif

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
ایستا CURLcode read_pkt_to_send(تهی *userp,
                                 unsigned نویسه *buf, size_t buflen,
                                 size_t *pnread)
{
  ساختار cf_ngtcp2_io_ctx *x = userp;
  ساختار cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t stream_id;
  صحیح fin;
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
  برای(;;) {

    اگر(ctx->h3conn && ngtcp2_conn_get_max_data_چپ(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_stream(ctx->h3conn, &stream_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      اگر(veccnt < 0) {
        failf(x->data, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((صحیح)veccnt));
        Curl_cf_ngtcp2_h3_err_set(x->cf, x->data, (صحیح)veccnt);
        بازگشت CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_STREAM_FLAG_MORE |
            (fin ? NGTCP2_WRITE_STREAM_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_stream(ctx->qconn, &x->ps.path,
                                  پوچ, buf, buflen,
                                  &ndatalen, flags, stream_id,
                                  (ثابت ngtcp2_vec *)vec, veccnt, x->ts);
    اگر(n == 0) {
      /* nothing to send */
      بازگشت CURLE_AGAIN;
    }
    وگرنه اگر(n < 0) {
      انتخاب(n) {
      حالت NGTCP2_ERR_STREAM_DATA_BLOCKED: {
        ساختار h3_stream_ctx *stream;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_block_stream(ctx->h3conn, stream_id);
        CURL_TRC_CF(x->data, x->cf, "[%" PRId64 "] block quic flow",
                    stream_id);
        stream = cf_ngtcp2_get_stream(ctx, stream_id);
        اگر(stream) /* it might be not one of our h3 streams? */
          stream->quic_flow_blocked = TRUE;
        n = 0;
        بشکن;
      }
      حالت NGTCP2_ERR_STREAM_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_stream_write(ctx->h3conn, stream_id);
        n = 0;
        بشکن;
      حالت NGTCP2_ERR_WRITE_MORE:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        بشکن;
      پیشفرض:
        DEBUGASSERT(ndatalen == -1);
        failf(x->data, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((صحیح)n));
        Curl_cf_ngtcp2_cmn_err_set(x->cf, x->data, (صحیح)n);
        بازگشت CURLE_SEND_ERROR;
      }
    }

    اگر(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      صحیح rv = nghttp3_conn_add_write_offset(ctx->h3conn, stream_id, ndatalen);
      اگر(rv) {
        failf(x->data, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        بازگشت CURLE_SEND_ERROR;
      }
    }

    اگر(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      بازگشت CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_progress_egress(ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data,
                                        ساختار cf_ngtcp2_io_ctx *pktx)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_size, path_max_payload_size;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_quantum;
  CURLcode result;
  ساختار cf_ngtcp2_io_ctx local_pktx;

  اگر(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  وگرنه {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  اگر(result) {
    اگر(result == CURLE_AGAIN) {
      Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
      بازگشت CURLE_OK;
    }
    بازگشت result;
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
  برای(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_size,
                            read_pkt_to_send, pktx, &nread);
    اگر(result == CURLE_AGAIN)
      بشکن;
    وگرنه اگر(result)
      بازگشت result;
    وگرنه {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      اگر((buflen >= send_quantum) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_size))
        بشکن;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      اگر(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      وگرنه اگر(nread > gsolen ||
              (gsolen > path_max_payload_size && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        result = Curl_vquic_send_tail_split(cf, data, &ctx->q,
                                            gsolen, nread, nread);
        اگر(result) {
          اگر(result == CURLE_AGAIN) {
            Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
            بازگشت CURLE_OK;
          }
          بازگشت result;
        }
        pktcnt = 0;
      }
      وگرنه اگر(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        بشکن;
      }
    }
  }

  اگر(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(data, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
    اگر(result) {
      اگر(result == CURLE_AGAIN) {
        Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
        بازگشت CURLE_OK;
      }
      بازگشت result;
    }
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_conn_update_pkt_tx_time(ctx->qconn, pktx->ts);
  }
  بازگشت CURLE_OK;
}

ساختار cf_ngtcp2_recv_ctx {
  ساختار cf_ngtcp2_io_ctx *pktx;
  size_t pkt_شمار;
};

ایستا CURLcode cf_ngtcp2_recv_pkts(ثابت unsigned نویسه *buf, size_t buflen,
                                    size_t gso_size,
                                    ساختار sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    تهی *userp)
{
  ساختار cf_ngtcp2_recv_ctx *rctx = userp;
  ساختار cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  ساختار cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_path path;
  size_t offset, pktlen;
  صحیح rv;

  اگر(!rctx->pkt_شمار) {
    Curl_cf_ngtcp2_io_ctx_update_time(pktx->data, pktx, pktx->cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  اگر(ecn)
    CURL_TRC_CF(pktx->data, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_size, (unsigned)ecn);
  ngtcp2_addr_init(&path.local, (ساختار sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&path.remote, (ساختار sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  برای(offset = 0; offset < buflen; offset += gso_size) {
    rctx->pkt_شمار++;
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &path, &pi,
                              buf + offset, pktlen, pktx->ts);
    اگر(rv) {
      CURL_TRC_CF(pktx->data, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_set(pktx->cf, pktx->data, rv);

      اگر(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        بازگشت CURLE_PEER_FAILED_VERIFICATION;
      بازگشت CURLE_RECV_ERROR;
    }
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_progress_ingress(ساختار Curl_cfilter *cf,
                                         ساختار Curl_easy *data,
                                         ساختار cf_ngtcp2_io_ctx *pktx)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار cf_ngtcp2_io_ctx local_pktx;
  ساختار cf_ngtcp2_recv_ctx rctx;
  CURLcode result = CURLE_OK;

  اگر(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }

  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  اگر(result)
    بازگشت result;

  rctx.pktx = pktx;
  rctx.pkt_شمار = 0;

  اگر(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(data, cf, "progress_ingress(socket)");
    بازگشت Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  وگرنه {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    unsigned نویسه *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_SIZE;
    size_t pkt_limit = QUIC_TUNNEL_INGRESS_PKT_LIMIT;
    size_t nread;
    ساختار sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(data, cf, "progress_ingress(sub-filters)");
    اگر(ctx->qconn) {
      size_t max_path_payload;
      max_path_payload =
        ngtcp2_conn_get_path_max_tx_udp_payload_size(ctx->qconn);
      اگر(max_path_payload > max_udp_payload)
        max_udp_payload = max_path_payload;
    }

    اگر(ctx->tunnel_inbuf_len < max_udp_payload) {
      unsigned نویسه *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      اگر(!newbuf)
        بازگشت CURLE_OUT_OF_MEMORY;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    وقتی(pkt_limit--) {
      result = Curl_conn_cf_recv(cf->next, data, (نویسه *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      اگر(result == CURLE_AGAIN) {
        /* no more data available at the moment */
        بازگشت CURLE_OK;
      }
      اگر(result) {
        CURL_TRC_CF(data, cf, "ingress, recv from tunnel failed: %d",
                    (صحیح)result);
        بازگشت result;
      }
      اگر(nread == 0) {
        /* tunnel closed */
        بازگشت CURLE_OK;
      }

      memcpy(&remote_addr, ctx->connected_path.remote.addr,
             ctx->connected_path.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_path.remote.addrlen;
      result = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      اگر(result)
        بازگشت result;

      اگر(!ctx->q.got_first_byte) {
        ctx->q.got_first_byte = TRUE;
        ctx->q.first_byte_at = ctx->q.last_op;
      }
      ctx->q.last_io = ctx->q.last_op;
    }
    بازگشت CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_set_expiry(ساختار Curl_cfilter *cf,
                                       ساختار Curl_easy *data,
                                       ساختار cf_ngtcp2_io_ctx *pktx)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  اگر(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  وگرنه {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  اگر(expiry != UINT64_MAX) {
    اگر(expiry <= pktx->ts) {
      CURLcode result;
      صحیح rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      اگر(rv) {
        failf(data, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_set(cf, data, rv);
        بازگشت CURLE_SEND_ERROR;
      }
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, pktx);
      اگر(!result)
        result = Curl_cf_ngtcp2_progress_egress(cf, data, pktx);
      اگر(result) {
        /* a verify failure during ingress must win over generic errors */
        اگر(ctx->tls_vrfy_result)
          result = ctx->tls_vrfy_result;
        بازگشت result;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    اگر(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      اگر(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_set(data, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  بازگشت CURLE_OK;
}

ایستا تهی cf_ngtcp2_setup_keep_alive(ساختار Curl_cfilter *cf,
                                       ساختار Curl_easy *data)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
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
    بازگشت;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  اگر(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no peer idle timeout, unset keep-alive");
  }
  وگرنه اگر(!Curl_u32_ptrset_شمار(&ctx->streams)) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no active streams, unset keep-alive");
  }
  وگرنه {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(data, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_stream_setup(ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data,
                                        ساختار h3_stream_ctx **pstream)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  *pstream = پوچ;
  اگر(!data)
    بازگشت CURLE_FAILED_INIT;

  اگر(stream) {
    *pstream = stream;
    بازگشت CURLE_OK;
  }

  stream = curlx_calloc(1, اندازه(*stream));
  اگر(!stream)
    بازگشت CURLE_OUT_OF_MEMORY;

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
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_سطر_LEN);

  اگر(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    Curl_cf_ngtcp2_h3_stream_ctx_free(stream);
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  اگر(Curl_u32_ptrset_شمار(&ctx->streams) == 1)
    cf_ngtcp2_setup_keep_alive(cf, data);

  *pstream = stream;
  بازگشت CURLE_OK;
}

تهی Curl_cf_ngtcp2_h3_stream_close(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    ساختار h3_stream_ctx *stream)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(data);
  DEBUGASSERT(stream);
  اگر(!stream->closed && ctx->qconn && ctx->h3conn) {
    CURLcode result;

    nghttp3_conn_set_stream_user_data(ctx->h3conn, stream->id, پوچ);
    ngtcp2_conn_set_stream_user_data(ctx->qconn, stream->id, پوچ);
    stream->closed = TRUE;
    (تهی)ngtcp2_conn_shutdown_stream(ctx->qconn, 0, stream->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    result = Curl_cf_ngtcp2_progress_egress(cf, data, پوچ);
    اگر(result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cancel stream -> %d",
                  stream->id, (صحیح)result);
  }
}

تهی Curl_cf_ngtcp2_h3_stream_done(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  (تهی)cf;
  اگر(stream) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] easy handle is done", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
    اگر(!Curl_u32_ptrset_شمار(&ctx->streams))
      cf_ngtcp2_setup_keep_alive(cf, data);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      bool *درون_داد_pending)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ثابت ngtcp2_transport_params *rp;
  ساختار cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  *درون_داد_pending = FALSE;
  اگر(!ctx->qconn || ctx->shutdown_started)
    برو out;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  اگر(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->q.last_io);
    اگر(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      اگر((uint64_t)idletime_ms > max_idle_ms)
        برو out;
    }
  }

  اگر(!cf->next || !cf->next->cft->is_alive(cf->next, data, درون_داد_pending))
    برو out;

  alive = TRUE;
  اگر(*درون_داد_pending) {
    CURLcode result;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *درون_داد_pending = FALSE;
    result = Curl_cf_ngtcp2_progress_ingress(cf, data, پوچ);
    CURL_TRC_CF(data, cf, "is_alive, progress ingress -> %d", (صحیح)result);
    alive = result ? FALSE : TRUE;
  }

out:
  CF_DATA_RESTORE(cf, save);
  بازگشت alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(ساختار cf_ngtcp2_ctx *ctx,
                                      ساختار Curl_easy *data)
{
  int64_t ctrl_stream_id, qpack_enc_stream_id, qpack_dec_stream_id;
  صحیح rc;

  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &ctrl_stream_id, پوچ);
  اگر(rc) {
    failf(data, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    بازگشت CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_stream(ctx->h3conn, ctrl_stream_id);
  اگر(rc) {
    failf(data, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    بازگشت CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_enc_stream_id, پوچ);
  اگر(rc) {
    failf(data, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    بازگشت CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_dec_stream_id, پوچ);
  اگر(rc) {
    failf(data, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    بازگشت CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_stream_id,
                                       qpack_dec_stream_id);
  اگر(rc) {
    failf(data, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    بازگشت CURLE_QUIC_CONNECT_ERROR;
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  صحیح query, صحیح *pres1, تهی *pres2)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;

  انتخاب(query) {
  حالت CF_QUERY_CONNECT_REPLY_MS:
    اگر((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (صحیح)ms : INT_MAX;
      بازگشت CURLE_OK;
    }
    بشکن;
  حالت CF_QUERY_REALLY_CONNECTED:
    اگر(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_first_byte;
      بازگشت CURLE_OK;
    }
    بشکن;
  پیشفرض:
    بشکن;
  }
  بازگشت cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  صحیح event, صحیح arg1, تهی *arg2)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (تهی)arg1;
  (تهی)arg2;
  انتخاب(event) {
  حالت CF_CTRL_REPORT_STATS:
    اگر(cf->connected && !ctx->stats_reported) {
      اگر((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        ctx->stats_reported = TRUE;
      }
      وگرنه اگر(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        اگر(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    بشکن;
  پیشفرض:
    بشکن;
  }
  بازگشت result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
