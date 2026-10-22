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

#𐤀𐤌 !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ضمن <ngtcp2/ngtcp2.h>

#ifdef USE_OPENSSL
#ضمن <openssl/err.h>
#𐤀𐤌 defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ضمن <ngtcp2/ngtcp2_crypto_boringssl.h>
#وإلاإذا defined(OPENSSL_QUIC_API2)
#ضمن <ngtcp2/ngtcp2_crypto_ossl.h>
#وإلا
#ضمن <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#ضمن "vtls/openssl.h"
#وإلاإذا defined(USE_GNUTLS)
#ضمن <ngtcp2/ngtcp2_crypto_gnutls.h>
#ضمن "vtls/gtls.h"
#وإلاإذا defined(USE_WOLFSSL)
#ضمن <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ضمن "vtls/wolfssl.h"
#endif

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
  ثابت ngtcp2_info *ng2 = ngtcp2_version(0);
  ثابت nghttp3_info *ht3 = nghttp3_version(0);
  (فراغ)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->version_str, ht3->version_str);
}

فراغ Curl_cf_ngtcp2_h3_stream_ctx_free(بنية h3_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

ساكن فراغ h3_stream_set_free(uint32_t id, فراغ *stream)
{
  (فراغ)id;
  DEBUGASSERT(stream);
  Curl_cf_ngtcp2_h3_stream_ctx_free((بنية h3_stream_ctx *)stream);
}

ساكن bool cf_ngtcp2_h3_err_is_fatal(صحيح code)
{
  أرجع (NGHTTP3_ERR_FATAL >= code) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM == code);
}

فراغ Curl_cf_ngtcp2_h3_err_set(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data, صحيح code)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  𐤀𐤌(!ctx->last_error.error_code) {
    ngtcp2_ccerr_set_application_error(&ctx->last_error,
      nghttp3_err_infer_quic_app_error_code(code), عدم, 0);
  }
  𐤀𐤌(cf_ngtcp2_h3_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
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
  ctx->version = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->stream_bufcp, H3_STREAM_CHUNK_SIZE,
                  H3_STREAM_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  أرجع Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

فراغ Curl_cf_ngtcp2_ctx_cleanup(بنية cf_ngtcp2_ctx *ctx)
{
  𐤀𐤌(ctx && ctx->initialized) {
    𐤀𐤌(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    𐤀𐤌(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = عدم;
    }
    𐤀𐤌(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = عدم;
    }
#ifdef OPENSSL_QUIC_API2
    𐤀𐤌(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = عدم;
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

بنية Curl_easy *Curl_cf_ngtcp2_get_xfer(بنية Curl_cfilter *cf,
                                          فراغ *stream_user_data,
                                          بنية h3_stream_ctx **pstream)
{
  بنية h3_stream_ctx *stream = stream_user_data;

  𐤀𐤌(pstream)
    *pstream = stream;
  𐤀𐤌(stream && (stream->mid != UINT32_MAX)) {
    بنية Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    𐤀𐤌(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      أرجع عدم;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    𐤀𐤌(data && (data->id == stream->xfer_id))
      أرجع data;
    DEBUGASSERT(0);
  }
  أرجع عدم;
}

ساكن ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  بنية Curl_cfilter *cf = conn_ref->user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  أرجع ctx->qconn;
}

#ifdef DEBUG_NGTCP2
ساكن فراغ quic_printf(فراغ *user_data, ثابت حرف *fmt, ...)
{
  va_list ap;
  (فراغ)user_data;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#endif

ساكن فراغ qlog_callback(فراغ *user_data, uint32_t flags,
                          ثابت فراغ *data, size_t datalen)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  (فراغ)flags;
  𐤀𐤌(ctx->qlogfd != -1) {
    ssize_t rc = اكتب(ctx->qlogfd, data, datalen);
    𐤀𐤌(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

ساكن فراغ quic_settings(بنية cf_ngtcp2_ctx *ctx,
                          بنية Curl_easy *data,
                          بنية cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#ifdef DEBUG_NGTCP2
  s->log_printf = quic_printf;
#وإلا
  s->log_printf = عدم;
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
  𐤀𐤌(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#𐤀𐤌 defined(_MSC_VER) && defined(_DLL)
#pragma warning(push)
#pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

ساكن صحيح cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, فراغ *user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf ? cf->ctx : عدم;
  بنية Curl_easy *data;

  (فراغ)tconn;
  DEBUGASSERT(ctx);
  data = CF_DATA_CURRENT(cf);
  DEBUGASSERT(data);
  𐤀𐤌(!ctx || !data)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(data);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, data);

  ctx->tls_vrfy_result = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    data, &ctx->ssl_peer);
  𐤀𐤌(ctx->tls_vrfy_result)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

#ifdef CURLVERBOSE
  𐤀𐤌(Curl_trc_is_verbose(data)) {
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
  𐤀𐤌(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  𐤀𐤌(ctx->use_earlydata) {
#𐤀𐤌 defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    𐤀𐤌(SSL_get_early_data_status(ctx->tls.ossl.ssl) == SSL_EARLY_DATA_NOT_SENT)
      CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
#endif

#𐤀𐤌 NGTCP2_VERSION_NUM >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_data_rejected2(ctx->qconn);
#وإلا /* older NGTCP2 */
    {
#𐤀𐤌 defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      صحيح ossl_early_status = SSL_get_early_data_status(ctx->tls.ossl.ssl);
      𐤀𐤌(ossl_early_status == SSL_EARLY_DATA_NOT_SENT)
        CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_status == SSL_EARLY_DATA_ACCEPTED);
#وإلاإذا defined(USE_GNUTLS)
      صحيح flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_DATA);
#وإلاإذا defined(USE_WOLFSSL)
#ifdef WOLFSSL_EARLY_DATA
      ctx->earlydata_accepted =
        (wolfSSL_get_early_data_status(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_DATA_REJECTED);
#وإلا
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
  أرجع 0;
}

ساكن صحيح cb_recv_stream_data(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t stream_id, uint64_t offset,
                               ثابت uint8_t *buf, size_t buflen,
                               فراغ *user_data, فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  صحيح fin = (flags & NGTCP2_STREAM_DATA_FLAG_FIN) ? 1 : 0;
  بنية h3_stream_ctx *stream;
  بنية Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (فراغ)offset;
  rc = nghttp3_conn_read_stream(ctx->h3conn, stream_id, buf, buflen, fin);
  𐤀𐤌(rc < 0) {
    𐤀𐤌(data && stream) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  stream_id, stream->reset, stream->closed);
    }
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  𐤀𐤌(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_stream_offset(tconn, stream_id, nconsumed);
    ngtcp2_conn_extend_max_offset(tconn, nconsumed);
    𐤀𐤌(stream) {
      stream->rx_offset += nconsumed;
      stream->rx_offset_max += nconsumed;
    }
  }
  أرجع 0;
}

ساكن صحيح cb_acked_stream_data_offset(ngtcp2_conn *tconn, int64_t stream_id,
                                       uint64_t offset, uint64_t datalen,
                                       فراغ *user_data, فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)stream_id;
  (فراغ)tconn;
  (فراغ)offset;
  (فراغ)datalen;
  (فراغ)stream_user_data;

  rv = nghttp3_conn_add_ack_offset(ctx->h3conn, stream_id, datalen);
  𐤀𐤌(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_stream_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id, uint64_t app_error_code,
                           فراغ *user_data, فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, عدم);
  صحيح rv;

  (فراغ)tconn;
  /* stream is closed... */
  𐤀𐤌(!data)
    data = CF_DATA_CURRENT(cf);
  𐤀𐤌(!data)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  𐤀𐤌(!(flags & NGTCP2_STREAM_CLOSE_FLAG_APP_ERROR_CODE_SET)) {
    app_error_code = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, app_error_code, rv);
  𐤀𐤌(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
ساكن صحيح cb_stream_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id,
                           uint64_t rx_app_error_code,
                           uint64_t tx_app_error_code,
                           فراغ *user_data, فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, عدم);
  uint64_t h3_app_error_code = NGHTTP3_H3_NO_ERROR;
  صحيح rv;

  (فراغ)tconn;
  (فراغ)tx_app_error_code;
  /* stream is closed... */
  𐤀𐤌(!data)
    data = CF_DATA_CURRENT(cf);
  𐤀𐤌(!data)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  𐤀𐤌(flags & NGTCP2_STREAM_CLOSE2_FLAG_RX_APP_ERROR_CODE_SET)
    h3_app_error_code = rx_app_error_code;

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, h3_app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, h3_app_error_code, rv);
  𐤀𐤌(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}
#endif

ساكن صحيح cb_stream_reset(ngtcp2_conn *tconn, int64_t stream_id,
                           uint64_t final_size, uint64_t app_error_code,
                           فراغ *user_data, فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, عدم);
  صحيح rv;
  (فراغ)tconn;
  (فراغ)final_size;
  (فراغ)app_error_code;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  𐤀𐤌(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  𐤀𐤌(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_stream_stop_sending(ngtcp2_conn *tconn, int64_t stream_id,
                                  uint64_t app_error_code, فراغ *user_data,
                                  فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)tconn;
  (فراغ)app_error_code;
  (فراغ)stream_user_data;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  𐤀𐤌(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            فراغ *user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);

  (فراغ)tconn;
  ctx->max_bidi_streams = max_streams;
  𐤀𐤌(data)
    CURL_TRC_CF(data, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->used_bidi_streams);
  أرجع 0;
}

ساكن صحيح cb_extend_max_stream_data(ngtcp2_conn *tconn, int64_t stream_id,
                                     uint64_t max_data, فراغ *user_data,
                                     فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream;
  بنية Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  صحيح rv;
  (فراغ)tconn;
  (فراغ)max_data;

  rv = nghttp3_conn_unblock_stream(ctx->h3conn, stream_id);
  𐤀𐤌(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  𐤀𐤌(stream && stream->quic_flow_blocked) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] unblock quic flow", stream_id);
    stream->quic_flow_blocked = FALSE;
    𐤀𐤌(data)
      Curl_multi_mark_dirty(data);
  }
  أرجع 0;
}

ساكن فراغ cb_rand(uint8_t *dest, size_t destlen,
                    ثابت ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode result;
  (فراغ)rand_ctx;

  result = Curl_rand(عدم, dest, destlen);
  𐤀𐤌(result) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
ساكن صحيح cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    فراغ *user_data)
{
  CURLcode result;
  (فراغ)tconn;
  (فراغ)user_data;

  result = Curl_rand(عدم, cid->data, cidlen);
  𐤀𐤌(result)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(عدم, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  𐤀𐤌(result)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  أرجع 0;
}

#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
ساكن صحيح cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  بنية ngtcp2_stateless_reset_token *token, size_t cidlen, فراغ *user_data)
{
  CURLcode result;
  (فراغ)tconn;
  (فراغ)user_data;

  result = Curl_rand(عدم, cid->data, cidlen);
  𐤀𐤌(result)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(عدم, token->data, حجم(token->data));
  𐤀𐤌(result)
    أرجع NGTCP2_ERR_CALLBACK_FAILURE;

  أرجع 0;
}
#endif

ساكن صحيح cb_recv_rx_key(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          فراغ *user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf ? cf->ctx : عدم;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);
  (فراغ)tconn;

  𐤀𐤌(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    أرجع 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(data);
  𐤀𐤌(ctx && data && !ctx->h3conn && ctx->init_h3_conn_cb) {
    𐤀𐤌(ctx->init_h3_conn_cb(cf, data, ctx))
      أرجع NGTCP2_ERR_CALLBACK_FAILURE;
  }
  أرجع 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
ساكن ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  عدم, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_data_cb,
  cb_ngtcp2_handshake_completed,
  عدم, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_mask_cb,
  cb_recv_stream_data,
  cb_acked_stream_data_offset,
  عدم, /* stream_open */
  cb_stream_close,
  عدم, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  عدم, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  عدم, /* remove_connection_id */
  ngtcp2_crypto_update_key_cb, /* update_key */
  عدم, /* path_validation */
  عدم, /* select_preferred_addr */
  cb_stream_reset,
  عدم, /* extend_max_remote_streams_bidi */
  عدم, /* extend_max_remote_streams_uni */
  cb_extend_max_stream_data,
  عدم, /* dcid_status */
  عدم, /* handshake_confirmed */
  عدم, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  عدم, /* recv_datagram */
  عدم, /* ack_datagram */
  عدم, /* lost_datagram */
  ngtcp2_crypto_get_path_challenge_data_cb,
  cb_stream_stop_sending,
  عدم, /* version_negotiation */
  cb_recv_rx_key,
  عدم, /* recv_tx_key */
  عدم, /* early_data_rejected */
#ifdef NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  عدم, /* begin_path_validation */
#endif
#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  عدم, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  عدم, /* dcid_status2 */
  ngtcp2_crypto_get_path_challenge_data2_cb, /* get_path_challenge_data2 */
#endif
#ifdef NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  عدم, /* recv_stop_sending */
#endif
#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_stream_close2, /* is called instead of cb_stream_close when set */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif

#𐤀𐤌 defined(_MSC_VER) && defined(_DLL)
#pragma warning(pop)
#endif

#ifdef USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ساكن صحيح quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  بنية Curl_cfilter *cf;
  بنية cf_ngtcp2_ctx *ctx;
  بنية Curl_easy *data;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_data(ssl);
  cf = cref ? cref->user_data : عدم;
  ctx = cf ? cf->ctx : عدم;
  data = cf ? CF_DATA_CURRENT(cf) : عدم;
  𐤀𐤌(cf && data && ctx) {
    unsigned حرف *quic_tp = عدم;
    size_t quic_tp_len = 0;
#ifdef HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     حجم(tpbuf));
    𐤀𐤌(tplen < 0)
      CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((صحيح)tplen));
    وإلا {
      quic_tp = (unsigned حرف *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#endif
    Curl_ossl_add_session(cf, data, &ctx->tls.ossl, ctx->ssl_peer.scache_key,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, عدم);
  }
  أرجع 0;
}
#endif /* USE_OPENSSL */

#ifdef USE_GNUTLS

#ifdef CURLVERBOSE
ساكن ثابت حرف *gtls_hs_msg_name(صحيح mtype)
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
#endif

ساكن صحيح quic_gtls_handshake_cb(gnutls_session_t session, unsigned صحيح htype,
                                  unsigned when, unsigned صحيح incoming,
                                  ثابت gnutls_datum_t *msg)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  بنية Curl_cfilter *cf = conn_ref ? conn_ref->user_data : عدم;
  بنية cf_ngtcp2_ctx *ctx = cf ? cf->ctx : عدم;

  (فراغ)msg;
  (فراغ)incoming;
  𐤀𐤌(when && cf && ctx) { /* after message has been processed */
    بنية Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    𐤀𐤌(!data)
      أرجع 0;
    CURL_TRC_CF(data, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_msg_name(htype), htype);
    بدل(htype) {
    حالة GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned حرف *quic_tp = عدم;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       حجم(tpbuf));
      𐤀𐤌(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحيح)tplen));
      وإلا {
        quic_tp = (unsigned حرف *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (فراغ)Curl_gtls_cache_session(cf, data, ctx->ssl_peer.scache_key,
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
#endif /* USE_GNUTLS */

#ifdef USE_WOLFSSL
ساكن صحيح wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolfSSL_get_app_data(ssl);
  بنية Curl_cfilter *cf = conn_ref ? conn_ref->user_data : عدم;

  DEBUGASSERT(cf);
  𐤀𐤌(cf && session) {
    بنية cf_ngtcp2_ctx *ctx = cf->ctx;
    بنية Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    𐤀𐤌(data && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned حرف *quic_tp = عدم;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       حجم(tpbuf));
      𐤀𐤌(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((صحيح)tplen));
      وإلا {
        quic_tp = (unsigned حرف *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (فراغ)Curl_wssl_cache_session(cf, data, ctx->ssl_peer.scache_key,
                                    session, wolfSSL_version(ssl),
                                    "h3", quic_tp, quic_tp_len, عدم);
    }
  }
  أرجع 0;
}
#endif /* USE_WOLFSSL */

ساكن CURLcode cf_ngtcp2_tls_ctx_setup(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *data,
                                        فراغ *user_data)
{
  بنية curl_tls_ctx *ctx = user_data;

#ifdef USE_OPENSSL
#𐤀𐤌 defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  𐤀𐤌(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(data, "ngtcp2_crypto_boringssl_configure_client_context failed");
    أرجع CURLE_FAILED_INIT;
  }
#وإلاإذا defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#وإلا
  𐤀𐤌(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_quictls_configure_client_context failed");
    أرجع CURLE_FAILED_INIT;
  }
#endif /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  𐤀𐤌(Curl_ssl_scache_use(cf, data)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_set_session_cache_mode(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#وإلاإذا defined(USE_GNUTLS)
  𐤀𐤌(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(data, "ngtcp2_crypto_gnutls_configure_client_session failed");
    أرجع CURLE_FAILED_INIT;
  }
  𐤀𐤌(Curl_ssl_scache_use(cf, data)) {
    gnutls_handshake_set_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#وإلاإذا defined(USE_WOLFSSL)
  𐤀𐤌(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    أرجع CURLE_FAILED_INIT;
  }
  𐤀𐤌(Curl_ssl_scache_use(cf, data)) {
    /* Register to get notified when a new session is received */
    wolfSSL_CTX_sess_set_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#endif
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ngtcp2_on_session_reuse(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *data,
                                           بنية alpn_spec *alpns,
                                           بنية Curl_ssl_session *scs,
                                           bool *do_early_data)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;
#𐤀𐤌 defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#endif
#ifdef USE_GNUTLS
  ctx->earlydata_max =
    gnutls_record_get_max_early_data_size(ctx->tls.gtls.session);
#endif
#ifdef USE_WOLFSSL
#ifdef WOLFSSL_EARLY_DATA
  ctx->earlydata_max = scs->earlydata_max;
#وإلا
  ctx->earlydata_max = 0;
#endif /* WOLFSSL_EARLY_DATA */
#endif
#𐤀𐤌 defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  𐤀𐤌(!ctx->earlydata_max) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  وإلا 𐤀𐤌(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session from different ALPN, no early data");
  }
  وإلا 𐤀𐤌(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(data, cf, "no 0RTT transport parameters, no early data");
  }
  وإلا {
    صحيح rv;
    rv = ngtcp2_conn_decode_and_set_0rtt_transport_params(
      ctx->qconn, (ثابت uint8_t *)scs->quic_tp, scs->quic_tp_len);
    𐤀𐤌(rv)
      CURL_TRC_CF(data, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    وإلا 𐤀𐤌(ctx->init_h3_conn_cb) {
      infof(data, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      result = ctx->init_h3_conn_cb(cf, data, ctx);
      𐤀𐤌(!result) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_data = TRUE;
      }
    }
    وإلا { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_data = TRUE;
    }
  }
#وإلا /* not supported in the TLS backend */
  (فراغ)data;
  (فراغ)ctx;
  (فراغ)scs;
  (فراغ)alpns;
#endif
  أرجع result;
}

/*
 * Might be called twice for happy eyeballs.
 */
ساكن CURLcode cf_connect_start(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rc;
  صحيح rv;
  CURLcode result;
  ثابت بنية Curl_sockaddr_ex *sockaddr = عدم;
  صحيح qfd;
  ساكن ثابت بنية alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->dcid.data, NGTCP2_MAX_CIDLEN);
  𐤀𐤌(result)
    أرجع result;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->scid.data, NGTCP2_MAX_CIDLEN);
  𐤀𐤌(result)
    أرجع result;

  (فراغ)Curl_qlogdir(data, ctx->scid.data, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, data, pktx);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  𐤀𐤌(result)
    أرجع result;

  /* Query socket and remote address from sub-chain */
  𐤀𐤌(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, عدم)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  𐤀𐤌(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = حجم(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (بنية sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    𐤀𐤌(rv == -1)
      أرجع CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (بنية sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_path.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  وإلا {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ثابت بنية Curl_sockaddr_ex *remote = عدم;
    𐤀𐤌(cf->next->cft->query(cf->next, data, CF_QUERY_REMOTE_ADDR, عدم,
                            CURL_UNCONST(&remote)))
      أرجع CURLE_QUIC_CONNECT_ERROR;
    𐤀𐤌(!remote)
      أرجع CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, حجم(ctx->q.local_addr));
    بدل(remote->family) {
    حالة AF_INET:
      ((بنية sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = حجم(بنية sockaddr_in);
      اكسر;
#ifdef USE_IPV6
    حالة AF_INET6:
      ((بنية sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = حجم(بنية sockaddr_in6);
      اكسر;
#endif
    افتراضي:
      أرجع CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (بنية sockaddr *)&ctx->q.local_addr,
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
  𐤀𐤌(rc)
    أرجع CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_data = cf;

  result = Curl_vquic_tls_init(&ctx->tls, cf, data,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_setup, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  𐤀𐤌(result)
    أرجع result;

#𐤀𐤌 defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  𐤀𐤌(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_ctx_new failed");
    أرجع CURLE_FAILED_INIT;
  }
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  𐤀𐤌(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_configure_client_session failed");
    أرجع CURLE_FAILED_INIT;
  }
#وإلاإذا defined(USE_OPENSSL)
  SSL_set_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#وإلاإذا defined(USE_GNUTLS)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#وإلاإذا defined(USE_WOLFSSL)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#وإلا
#error "ngtcp2 TLS backend not defined"
#endif

#𐤀𐤌 defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  𐤀𐤌(ctx->use_earlydata &&
     !SSL_set_quic_tls_early_data_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(data, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#endif
  ngtcp2_ccerr_default(&ctx->last_error);

  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    bool *done)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  بنية cf_call_data save;
  بنية cf_ngtcp2_io_ctx pktx;

  𐤀𐤌(cf->connected) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the sub-chain */
  𐤀𐤌(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    𐤀𐤌(result || !*done)
      أرجع result;
  }

  *done = FALSE;

  𐤀𐤌(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    أرجع CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  CF_DATA_SAVE(save, cf, data);

  𐤀𐤌(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(data);
    result = cf_connect_start(cf, data, &pktx);
    𐤀𐤌(result)
      اذهب out;
    𐤀𐤌(cf->connected) {
      *done = TRUE;
      اذهب out;
    }
    result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
    /* we do not expect to be able to recv anything yet */
    اذهب out;
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  𐤀𐤌(result)
    اذهب out;

  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
  𐤀𐤌(result)
    اذهب out;

  𐤀𐤌(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    result = ctx->tls_vrfy_result;
    𐤀𐤌(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      cf->connected = TRUE;
      *done = TRUE;
    }
  }

out:
  𐤀𐤌(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
  𐤀𐤌(ctx->qconn &&
     ((result == CURLE_RECV_ERROR) || (result == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_in_draining_period(ctx->qconn)) {
    ثابت ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    result = CURLE_COULDNT_CONNECT;
    𐤀𐤌(cerr) {
      CURL_TRC_CF(data, cf, "connect error, type=%d, code=%" PRIu64,
                  (صحيح)cerr->type, cerr->error_code);
      بدل(cerr->type) {
      حالة NGTCP2_CCERR_TYPE_VERSION_NEGOTIATION:
        CURL_TRC_CF(data, cf, "error in version negotiation");
        اكسر;
      افتراضي:
        𐤀𐤌(cerr->error_code >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(data, cf, "crypto error, tls alert=%u",
                      (unsigned صحيح)(cerr->error_code & 0xffU));
        }
        وإلا 𐤀𐤌(cerr->error_code == NGTCP2_CONNECTION_REFUSED) {
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
  𐤀𐤌(result) {
    𐤀𐤌(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      بنية ip_quadruple ip;

      𐤀𐤌(!Curl_cf_socket_peek(cf->next, data, عدم, عدم, &ip))
        infof(data, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    }
  }
#endif
  𐤀𐤌(!result && ctx->qconn) {
    result = Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx);
  }
  𐤀𐤌(result || *done)
    CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (صحيح)result, *done);
  CF_DATA_RESTORE(cf, save);
  أرجع result;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *data, bool *done)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_call_data save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  𐤀𐤌(cf->shutdown || !ctx->qconn) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  𐤀𐤌(!cf->next) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *done = TRUE;
    أرجع CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);
  *done = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  𐤀𐤌(!ctx->shutdown_started) {
    حرف buffer[NGTCP2_MAX_UDP_PAYLOAD_SIZE];
    ngtcp2_ssize nwritten;

    𐤀𐤌(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
      CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
      result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
      𐤀𐤌(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
        CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
        result = CURLE_OK;
        اذهب out;
      }
      وإلا 𐤀𐤌(result) {
        CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                    (صحيح)result);
        *done = TRUE;
        اذهب out;
      }
    }

    DEBUGASSERT(Curl_bufq_is_empty(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, عدم, /* path */
      عدم, /* pkt_info */
      (uint8_t *)buffer, حجم(buffer),
      &ctx->last_error, pktx.ts);
    CURL_TRC_CF(data, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (صحيح)ctx->last_error.type,
                ctx->last_error.error_code, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    𐤀𐤌(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      result = Curl_bufq_write(&ctx->q.sendbuf, (ثابت unsigned حرف *)buffer,
                               (size_t)nwritten, &n);
      𐤀𐤌(result) {
        CURL_TRC_CF(data, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (صحيح)result);
        اذهب out;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.split_len = 0;
    }
  }

  𐤀𐤌(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing egress");
    result = Curl_vquic_flush(cf, data, &ctx->q);
    𐤀𐤌(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
      result = CURLE_OK;
      اذهب out;
    }
    وإلا 𐤀𐤌(result) {
      CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                  (صحيح)result);
      *done = TRUE;
      اذهب out;
    }
  }

  𐤀𐤌(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
    result = CURLE_OK;
  }
out:
  CF_DATA_RESTORE(cf, save);
  أرجع result;
}

فراغ Curl_cf_ngtcp2_cmn_conn_close(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data)
{
  bool done;
  Curl_cf_ngtcp2_cmn_shutdown(cf, data, &done);
}

ساكن bool cf_ngtcp2_err_is_fatal(صحيح code)
{
  أرجع (NGTCP2_ERR_FATAL >= code) ||
         (NGTCP2_ERR_DROP_CONN == code) ||
         (NGTCP2_ERR_IDLE_CLOSE == code);
}

فراغ Curl_cf_ngtcp2_cmn_err_set(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data, صحيح code)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  𐤀𐤌(!ctx->last_error.error_code) {
    𐤀𐤌(NGTCP2_ERR_CRYPTO == code) {
      ngtcp2_ccerr_set_tls_alert(&ctx->last_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 عدم, 0);
    }
    وإلا {
      ngtcp2_ccerr_set_liberr(&ctx->last_error, code, عدم, 0);
    }
  }
  𐤀𐤌(cf_ngtcp2_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

فراغ Curl_cf_ngtcp2_io_ctx_init(بنية cf_ngtcp2_io_ctx *io_ctx,
                                بنية Curl_cfilter *cf,
                                بنية Curl_easy *data)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->data = data;
  io_ctx->now = *Curl_pgrs_now(data);
  ngtcp2_path_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_set_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

فراغ Curl_cf_ngtcp2_io_ctx_update_time(بنية Curl_easy *data,
                                       بنية cf_ngtcp2_io_ctx *io_ctx,
                                       بنية Curl_cfilter *cf)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(data);
  Curl_vquic_ctx_update_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#𐤀𐤌 NGTCP2_VERSION_NUM < 0x011100
بنية cf_ngtcp2_sfind_ctx {
  int64_t stream_id;
  بنية h3_stream_ctx *stream;
  uint32_t mid;
};

ساكن bool cf_ngtcp2_sfind(uint32_t mid, فراغ *value, فراغ *user_data)
{
  بنية cf_ngtcp2_sfind_ctx *fctx = user_data;
  بنية h3_stream_ctx *stream = value;

  𐤀𐤌(fctx->stream_id == stream->id) {
    fctx->mid = mid;
    fctx->stream = stream;
    أرجع FALSE;
  }
  أرجع TRUE; /* continue */
}

ساكن بنية h3_stream_ctx *cf_ngtcp2_get_stream(بنية cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  بنية cf_ngtcp2_sfind_ctx fctx;
  fctx.stream_id = stream_id;
  fctx.stream = عدم;
  Curl_u32_ptrset_visit(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  أرجع fctx.stream;
}
#وإلا
ساكن بنية h3_stream_ctx *cf_ngtcp2_get_stream(بنية cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  أرجع ngtcp2_conn_get_stream_user_data(ctx->qconn, stream_id);
}
#endif

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
ساكن CURLcode read_pkt_to_send(فراغ *userp,
                                 unsigned حرف *buf, size_t buflen,
                                 size_t *pnread)
{
  بنية cf_ngtcp2_io_ctx *x = userp;
  بنية cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t stream_id;
  صحيح fin;
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
  لكل(;;) {

    𐤀𐤌(ctx->h3conn && ngtcp2_conn_get_max_data_left(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_stream(ctx->h3conn, &stream_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      𐤀𐤌(veccnt < 0) {
        failf(x->data, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((صحيح)veccnt));
        Curl_cf_ngtcp2_h3_err_set(x->cf, x->data, (صحيح)veccnt);
        أرجع CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_STREAM_FLAG_MORE |
            (fin ? NGTCP2_WRITE_STREAM_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_stream(ctx->qconn, &x->ps.path,
                                  عدم, buf, buflen,
                                  &ndatalen, flags, stream_id,
                                  (ثابت ngtcp2_vec *)vec, veccnt, x->ts);
    𐤀𐤌(n == 0) {
      /* nothing to send */
      أرجع CURLE_AGAIN;
    }
    وإلا 𐤀𐤌(n < 0) {
      بدل(n) {
      حالة NGTCP2_ERR_STREAM_DATA_BLOCKED: {
        بنية h3_stream_ctx *stream;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_block_stream(ctx->h3conn, stream_id);
        CURL_TRC_CF(x->data, x->cf, "[%" PRId64 "] block quic flow",
                    stream_id);
        stream = cf_ngtcp2_get_stream(ctx, stream_id);
        𐤀𐤌(stream) /* it might be not one of our h3 streams? */
          stream->quic_flow_blocked = TRUE;
        n = 0;
        اكسر;
      }
      حالة NGTCP2_ERR_STREAM_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_stream_write(ctx->h3conn, stream_id);
        n = 0;
        اكسر;
      حالة NGTCP2_ERR_WRITE_MORE:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        اكسر;
      افتراضي:
        DEBUGASSERT(ndatalen == -1);
        failf(x->data, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((صحيح)n));
        Curl_cf_ngtcp2_cmn_err_set(x->cf, x->data, (صحيح)n);
        أرجع CURLE_SEND_ERROR;
      }
    }

    𐤀𐤌(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      صحيح rv = nghttp3_conn_add_write_offset(ctx->h3conn, stream_id, ndatalen);
      𐤀𐤌(rv) {
        failf(x->data, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        أرجع CURLE_SEND_ERROR;
      }
    }

    𐤀𐤌(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      أرجع CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_progress_egress(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *data,
                                        بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_size, path_max_payload_size;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_quantum;
  CURLcode result;
  بنية cf_ngtcp2_io_ctx local_pktx;

  𐤀𐤌(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  وإلا {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  𐤀𐤌(result) {
    𐤀𐤌(result == CURLE_AGAIN) {
      Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
      أرجع CURLE_OK;
    }
    أرجع result;
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
  لكل(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_size,
                            read_pkt_to_send, pktx, &nread);
    𐤀𐤌(result == CURLE_AGAIN)
      اكسر;
    وإلا 𐤀𐤌(result)
      أرجع result;
    وإلا {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      𐤀𐤌((buflen >= send_quantum) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_size))
        اكسر;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      𐤀𐤌(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      وإلا 𐤀𐤌(nread > gsolen ||
              (gsolen > path_max_payload_size && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        result = Curl_vquic_send_tail_split(cf, data, &ctx->q,
                                            gsolen, nread, nread);
        𐤀𐤌(result) {
          𐤀𐤌(result == CURLE_AGAIN) {
            Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
            أرجع CURLE_OK;
          }
          أرجع result;
        }
        pktcnt = 0;
      }
      وإلا 𐤀𐤌(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        اكسر;
      }
    }
  }

  𐤀𐤌(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(data, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
    𐤀𐤌(result) {
      𐤀𐤌(result == CURLE_AGAIN) {
        Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
        أرجع CURLE_OK;
      }
      أرجع result;
    }
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_conn_update_pkt_tx_time(ctx->qconn, pktx->ts);
  }
  أرجع CURLE_OK;
}

بنية cf_ngtcp2_recv_ctx {
  بنية cf_ngtcp2_io_ctx *pktx;
  size_t pkt_count;
};

ساكن CURLcode cf_ngtcp2_recv_pkts(ثابت unsigned حرف *buf, size_t buflen,
                                    size_t gso_size,
                                    بنية sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    فراغ *userp)
{
  بنية cf_ngtcp2_recv_ctx *rctx = userp;
  بنية cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  بنية cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_path path;
  size_t offset, pktlen;
  صحيح rv;

  𐤀𐤌(!rctx->pkt_count) {
    Curl_cf_ngtcp2_io_ctx_update_time(pktx->data, pktx, pktx->cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  𐤀𐤌(ecn)
    CURL_TRC_CF(pktx->data, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_size, (unsigned)ecn);
  ngtcp2_addr_init(&path.local, (بنية sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&path.remote, (بنية sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  لكل(offset = 0; offset < buflen; offset += gso_size) {
    rctx->pkt_count++;
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &path, &pi,
                              buf + offset, pktlen, pktx->ts);
    𐤀𐤌(rv) {
      CURL_TRC_CF(pktx->data, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_set(pktx->cf, pktx->data, rv);

      𐤀𐤌(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        أرجع CURLE_PEER_FAILED_VERIFICATION;
      أرجع CURLE_RECV_ERROR;
    }
  }
  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_progress_ingress(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *data,
                                         بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_ngtcp2_io_ctx local_pktx;
  بنية cf_ngtcp2_recv_ctx rctx;
  CURLcode result = CURLE_OK;

  𐤀𐤌(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }

  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  𐤀𐤌(result)
    أرجع result;

  rctx.pktx = pktx;
  rctx.pkt_count = 0;

  𐤀𐤌(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(data, cf, "progress_ingress(socket)");
    أرجع Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  وإلا {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    unsigned حرف *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_SIZE;
    size_t pkt_limit = QUIC_TUNNEL_INGRESS_PKT_LIMIT;
    size_t nread;
    بنية sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(data, cf, "progress_ingress(sub-filters)");
    𐤀𐤌(ctx->qconn) {
      size_t max_path_payload;
      max_path_payload =
        ngtcp2_conn_get_path_max_tx_udp_payload_size(ctx->qconn);
      𐤀𐤌(max_path_payload > max_udp_payload)
        max_udp_payload = max_path_payload;
    }

    𐤀𐤌(ctx->tunnel_inbuf_len < max_udp_payload) {
      unsigned حرف *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      𐤀𐤌(!newbuf)
        أرجع CURLE_OUT_OF_MEMORY;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    طالما(pkt_limit--) {
      result = Curl_conn_cf_recv(cf->next, data, (حرف *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      𐤀𐤌(result == CURLE_AGAIN) {
        /* no more data available at the moment */
        أرجع CURLE_OK;
      }
      𐤀𐤌(result) {
        CURL_TRC_CF(data, cf, "ingress, recv from tunnel failed: %d",
                    (صحيح)result);
        أرجع result;
      }
      𐤀𐤌(nread == 0) {
        /* tunnel closed */
        أرجع CURLE_OK;
      }

      memcpy(&remote_addr, ctx->connected_path.remote.addr,
             ctx->connected_path.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_path.remote.addrlen;
      result = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      𐤀𐤌(result)
        أرجع result;

      𐤀𐤌(!ctx->q.got_first_byte) {
        ctx->q.got_first_byte = TRUE;
        ctx->q.first_byte_at = ctx->q.last_op;
      }
      ctx->q.last_io = ctx->q.last_op;
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
CURLcode Curl_cf_ngtcp2_cmn_set_expiry(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *data,
                                       بنية cf_ngtcp2_io_ctx *pktx)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  𐤀𐤌(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  وإلا {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  𐤀𐤌(expiry != UINT64_MAX) {
    𐤀𐤌(expiry <= pktx->ts) {
      CURLcode result;
      صحيح rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      𐤀𐤌(rv) {
        failf(data, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_set(cf, data, rv);
        أرجع CURLE_SEND_ERROR;
      }
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, pktx);
      𐤀𐤌(!result)
        result = Curl_cf_ngtcp2_progress_egress(cf, data, pktx);
      𐤀𐤌(result) {
        /* a verify failure during ingress must win over generic errors */
        𐤀𐤌(ctx->tls_vrfy_result)
          result = ctx->tls_vrfy_result;
        أرجع result;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    𐤀𐤌(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      𐤀𐤌(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_set(data, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  أرجع CURLE_OK;
}

ساكن فراغ cf_ngtcp2_setup_keep_alive(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *data)
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
  𐤀𐤌(!ctx->qconn)
    أرجع;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  𐤀𐤌(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no peer idle timeout, unset keep-alive");
  }
  وإلا 𐤀𐤌(!Curl_u32_ptrset_count(&ctx->streams)) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no active streams, unset keep-alive");
  }
  وإلا {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(data, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_stream_setup(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *data,
                                        بنية h3_stream_ctx **pstream)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  *pstream = عدم;
  𐤀𐤌(!data)
    أرجع CURLE_FAILED_INIT;

  𐤀𐤌(stream) {
    *pstream = stream;
    أرجع CURLE_OK;
  }

  stream = curlx_calloc(1, حجم(*stream));
  𐤀𐤌(!stream)
    أرجع CURLE_OUT_OF_MEMORY;

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
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_LINE_LEN);

  𐤀𐤌(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    Curl_cf_ngtcp2_h3_stream_ctx_free(stream);
    أرجع CURLE_OUT_OF_MEMORY;
  }

  𐤀𐤌(Curl_u32_ptrset_count(&ctx->streams) == 1)
    cf_ngtcp2_setup_keep_alive(cf, data);

  *pstream = stream;
  أرجع CURLE_OK;
}

فراغ Curl_cf_ngtcp2_h3_stream_close(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    بنية h3_stream_ctx *stream)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(data);
  DEBUGASSERT(stream);
  𐤀𐤌(!stream->closed && ctx->qconn && ctx->h3conn) {
    CURLcode result;

    nghttp3_conn_set_stream_user_data(ctx->h3conn, stream->id, عدم);
    ngtcp2_conn_set_stream_user_data(ctx->qconn, stream->id, عدم);
    stream->closed = TRUE;
    (فراغ)ngtcp2_conn_shutdown_stream(ctx->qconn, 0, stream->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    result = Curl_cf_ngtcp2_progress_egress(cf, data, عدم);
    𐤀𐤌(result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cancel stream -> %d",
                  stream->id, (صحيح)result);
  }
}

فراغ Curl_cf_ngtcp2_h3_stream_done(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  (فراغ)cf;
  𐤀𐤌(stream) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] easy handle is done", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
    𐤀𐤌(!Curl_u32_ptrset_count(&ctx->streams))
      cf_ngtcp2_setup_keep_alive(cf, data);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      bool *input_pending)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ثابت ngtcp2_transport_params *rp;
  بنية cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  *input_pending = FALSE;
  𐤀𐤌(!ctx->qconn || ctx->shutdown_started)
    اذهب out;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  𐤀𐤌(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->q.last_io);
    𐤀𐤌(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      𐤀𐤌((uint64_t)idletime_ms > max_idle_ms)
        اذهب out;
    }
  }

  𐤀𐤌(!cf->next || !cf->next->cft->is_alive(cf->next, data, input_pending))
    اذهب out;

  alive = TRUE;
  𐤀𐤌(*input_pending) {
    CURLcode result;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *input_pending = FALSE;
    result = Curl_cf_ngtcp2_progress_ingress(cf, data, عدم);
    CURL_TRC_CF(data, cf, "is_alive, progress ingress -> %d", (صحيح)result);
    alive = result ? FALSE : TRUE;
  }

out:
  CF_DATA_RESTORE(cf, save);
  أرجع alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(بنية cf_ngtcp2_ctx *ctx,
                                      بنية Curl_easy *data)
{
  int64_t ctrl_stream_id, qpack_enc_stream_id, qpack_dec_stream_id;
  صحيح rc;

  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &ctrl_stream_id, عدم);
  𐤀𐤌(rc) {
    failf(data, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_stream(ctx->h3conn, ctrl_stream_id);
  𐤀𐤌(rc) {
    failf(data, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_enc_stream_id, عدم);
  𐤀𐤌(rc) {
    failf(data, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_dec_stream_id, عدم);
  𐤀𐤌(rc) {
    failf(data, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_stream_id,
                                       qpack_dec_stream_id);
  𐤀𐤌(rc) {
    failf(data, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_CONNECT_REPLY_MS:
    𐤀𐤌((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (صحيح)ms : INT_MAX;
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CF_QUERY_REALLY_CONNECTED:
    𐤀𐤌(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_first_byte;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_REPORT_STATS:
    𐤀𐤌(cf->connected && !ctx->stats_reported) {
      𐤀𐤌((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        ctx->stats_reported = TRUE;
      }
      وإلا 𐤀𐤌(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        𐤀𐤌(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
