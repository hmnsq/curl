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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#અન્તર્ભાવ <ngtcp2/ngtcp2.h>

#ifdef USE_OPENSSL
#અન્તર્ભાવ <openssl/err.h>
#જો defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#અન્તર્ભાવ <ngtcp2/ngtcp2_crypto_boringssl.h>
#અથવાજો defined(OPENSSL_QUIC_API2)
#અન્તર્ભાવ <ngtcp2/ngtcp2_crypto_ossl.h>
#નહીંતર
#અન્તર્ભાવ <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#અન્તર્ભાવ "vtls/openssl.h"
#અથવાજો defined(USE_GNUTLS)
#અન્તર્ભાવ <ngtcp2/ngtcp2_crypto_gnutls.h>
#અન્તર્ભાવ "vtls/gtls.h"
#અથવાજો defined(USE_WOLFSSL)
#અન્તર્ભાવ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#અન્તર્ભાવ "vtls/wolfssl.h"
#endif

#અન્તર્ભાવ <nghttp3/nghttp3.h>

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "u32_ptrset.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "cf-socket.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "curlx/win32-fopen.h"
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "http1.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "sockaddr.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "bufref.h"
#અન્તર્ભાવ "vdns/cf-dns.h"
#અન્તર્ભાવ "vquic/vquic.h"
#અન્તર્ભાવ "vquic/vquic_int.h"
#અન્તર્ભાવ "vquic/vquic-tls.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "vtls/vtls_scache.h"
#અન્તર્ભાવ "vquic/cf-ngtcp2-cmn.h"

/*
 * Store ngtcp2 version info in this buffer.
 */
શૂન્ય Curl_ngtcp2_ver(અક્ષર *p, size_t len)
{
  અચલ ngtcp2_info *ng2 = ngtcp2_version(0);
  અચલ nghttp3_info *ht3 = nghttp3_version(0);
  (શૂન્ય)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->version_str, ht3->version_str);
}

શૂન્ય Curl_cf_ngtcp2_h3_stream_ctx_free(સંરચના h3_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

સ્થિર શૂન્ય h3_stream_set_free(uint32_t id, શૂન્ય *stream)
{
  (શૂન્ય)id;
  DEBUGASSERT(stream);
  Curl_cf_ngtcp2_h3_stream_ctx_free((સંરચના h3_stream_ctx *)stream);
}

સ્થિર bool cf_ngtcp2_h3_err_is_fatal(પૂર્ણાંક code)
{
  પાછા (NGHTTP3_ERR_FATAL >= code) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM == code);
}

શૂન્ય Curl_cf_ngtcp2_h3_err_set(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data, પૂર્ણાંક code)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  જો(!ctx->last_error.error_code) {
    ngtcp2_ccerr_set_application_error(&ctx->last_error,
      nghttp3_err_infer_quic_app_error_code(code), ખાલી, 0);
  }
  જો(cf_ngtcp2_h3_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

CURLcode Curl_cf_ngtcp2_ctx_init(સંરચના cf_ngtcp2_ctx *ctx,
                                 સંરચના Curl_peer *origin,
                                 સંરચના Curl_peer *peer,
                                 સંરચના ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = ખાલી;
  ctx->tunnel_inbuf_len = 0;
  ctx->version = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->stream_bufcp, H3_STREAM_CHUNK_SIZE,
                  H3_STREAM_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  પાછા Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

શૂન્ય Curl_cf_ngtcp2_ctx_cleanup(સંરચના cf_ngtcp2_ctx *ctx)
{
  જો(ctx && ctx->initialized) {
    જો(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    જો(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = ખાલી;
    }
    જો(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = ખાલી;
    }
#ifdef OPENSSL_QUIC_API2
    જો(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = ખાલી;
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

સંરચના Curl_easy *Curl_cf_ngtcp2_get_xfer(સંરચના Curl_cfilter *cf,
                                          શૂન્ય *stream_user_data,
                                          સંરચના h3_stream_ctx **pstream)
{
  સંરચના h3_stream_ctx *stream = stream_user_data;

  જો(pstream)
    *pstream = stream;
  જો(stream && (stream->mid != UINT32_MAX)) {
    સંરચના Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    જો(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      પાછા ખાલી;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    જો(data && (data->id == stream->xfer_id))
      પાછા data;
    DEBUGASSERT(0);
  }
  પાછા ખાલી;
}

સ્થિર ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  સંરચના Curl_cfilter *cf = conn_ref->user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  પાછા ctx->qconn;
}

#ifdef DEBUG_NGTCP2
સ્થિર શૂન્ય quic_printf(શૂન્ય *user_data, અચલ અક્ષર *fmt, ...)
{
  va_list ap;
  (શૂન્ય)user_data;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#endif

સ્થિર શૂન્ય qlog_callback(શૂન્ય *user_data, uint32_t flags,
                          અચલ શૂન્ય *data, size_t datalen)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  (શૂન્ય)flags;
  જો(ctx->qlogfd != -1) {
    ssize_t rc = લિખ(ctx->qlogfd, data, datalen);
    જો(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

સ્થિર શૂન્ય quic_settings(સંરચના cf_ngtcp2_ctx *ctx,
                          સંરચના Curl_easy *data,
                          સંરચના cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#ifdef DEBUG_NGTCP2
  s->log_printf = quic_printf;
#નહીંતર
  s->log_printf = ખાલી;
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
  જો(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#જો defined(_MSC_VER) && defined(_DLL)
#pragma warning(ધકેલો)
#pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

સ્થિર પૂર્ણાંક cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, શૂન્ય *user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ખાલી;
  સંરચના Curl_easy *data;

  (શૂન્ય)tconn;
  DEBUGASSERT(ctx);
  data = CF_DATA_CURRENT(cf);
  DEBUGASSERT(data);
  જો(!ctx || !data)
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(data);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, data);

  ctx->tls_vrfy_result = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    data, &ctx->ssl_peer);
  જો(ctx->tls_vrfy_result)
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;

#ifdef CURLVERBOSE
  જો(Curl_trc_is_verbose(data)) {
    અચલ ngtcp2_transport_params *rp;
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
  જો(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  જો(ctx->use_earlydata) {
#જો defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    જો(SSL_get_early_data_status(ctx->tls.ossl.ssl) == SSL_EARLY_DATA_NOT_SENT)
      CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
#endif

#જો NGTCP2_VERSION_અંક >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_data_rejected2(ctx->qconn);
#નહીંતર /* older NGTCP2 */
    {
#જો defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      પૂર્ણાંક ossl_early_status = SSL_get_early_data_status(ctx->tls.ossl.ssl);
      જો(ossl_early_status == SSL_EARLY_DATA_NOT_SENT)
        CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_status == SSL_EARLY_DATA_ACCEPTED);
#અથવાજો defined(USE_GNUTLS)
      પૂર્ણાંક flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_DATA);
#અથવાજો defined(USE_WOLFSSL)
#ifdef WOLFSSL_EARLY_DATA
      ctx->earlydata_accepted =
        (wolfSSL_get_early_data_status(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_DATA_REJECTED);
#નહીંતર
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
  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_recv_stream_data(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t stream_id, uint64_t offset,
                               અચલ uint8_t *buf, size_t buflen,
                               શૂન્ય *user_data, શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  પૂર્ણાંક fin = (flags & NGTCP2_STREAM_DATA_FLAG_FIN) ? 1 : 0;
  સંરચના h3_stream_ctx *stream;
  સંરચના Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (શૂન્ય)offset;
  rc = nghttp3_conn_read_stream(ctx->h3conn, stream_id, buf, buflen, fin);
  જો(rc < 0) {
    જો(data && stream) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  stream_id, stream->reset, stream->closed);
    }
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  જો(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_stream_offset(tconn, stream_id, nconsumed);
    ngtcp2_conn_extend_max_offset(tconn, nconsumed);
    જો(stream) {
      stream->rx_offset += nconsumed;
      stream->rx_offset_max += nconsumed;
    }
  }
  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_acked_stream_data_offset(ngtcp2_conn *tconn, int64_t stream_id,
                                       uint64_t offset, uint64_t datalen,
                                       શૂન્ય *user_data, શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  પૂર્ણાંક rv;
  (શૂન્ય)stream_id;
  (શૂન્ય)tconn;
  (શૂન્ય)offset;
  (શૂન્ય)datalen;
  (શૂન્ય)stream_user_data;

  rv = nghttp3_conn_add_ack_offset(ctx->h3conn, stream_id, datalen);
  જો(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;
  }

  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_stream_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id, uint64_t app_error_code,
                           શૂન્ય *user_data, શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ખાલી);
  પૂર્ણાંક rv;

  (શૂન્ય)tconn;
  /* stream is closed... */
  જો(!data)
    data = CF_DATA_CURRENT(cf);
  જો(!data)
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;

  જો(!(flags & NGTCP2_STREAM_CLOSE_FLAG_APP_ERROR_CODE_SET)) {
    app_error_code = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, app_error_code, rv);
  જો(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;
  }

  પાછા 0;
}

#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
સ્થિર પૂર્ણાંક cb_stream_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id,
                           uint64_t rx_app_error_code,
                           uint64_t tx_app_error_code,
                           શૂન્ય *user_data, શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ખાલી);
  uint64_t h3_app_error_code = NGHTTP3_H3_NO_ERROR;
  પૂર્ણાંક rv;

  (શૂન્ય)tconn;
  (શૂન્ય)tx_app_error_code;
  /* stream is closed... */
  જો(!data)
    data = CF_DATA_CURRENT(cf);
  જો(!data)
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;

  જો(flags & NGTCP2_STREAM_CLOSE2_FLAG_RX_APP_ERROR_CODE_SET)
    h3_app_error_code = rx_app_error_code;

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, h3_app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, h3_app_error_code, rv);
  જો(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;
  }

  પાછા 0;
}
#endif

સ્થિર પૂર્ણાંક cb_stream_reset(ngtcp2_conn *tconn, int64_t stream_id,
                           uint64_t final_size, uint64_t app_error_code,
                           શૂન્ય *user_data, શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ખાલી);
  પૂર્ણાંક rv;
  (શૂન્ય)tconn;
  (શૂન્ય)final_size;
  (શૂન્ય)app_error_code;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  જો(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  જો(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;
  }

  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_stream_stop_sending(ngtcp2_conn *tconn, int64_t stream_id,
                                  uint64_t app_error_code, શૂન્ય *user_data,
                                  શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  પૂર્ણાંક rv;
  (શૂન્ય)tconn;
  (શૂન્ય)app_error_code;
  (શૂન્ય)stream_user_data;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  જો(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;
  }

  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            શૂન્ય *user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);

  (શૂન્ય)tconn;
  ctx->max_bidi_streams = max_streams;
  જો(data)
    CURL_TRC_CF(data, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->used_bidi_streams);
  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_extend_max_stream_data(ngtcp2_conn *tconn, int64_t stream_id,
                                     uint64_t max_data, શૂન્ય *user_data,
                                     શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream;
  સંરચના Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  પૂર્ણાંક rv;
  (શૂન્ય)tconn;
  (શૂન્ય)max_data;

  rv = nghttp3_conn_unblock_stream(ctx->h3conn, stream_id);
  જો(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;
  }
  જો(stream && stream->quic_flow_blocked) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] unblock quic flow", stream_id);
    stream->quic_flow_blocked = FALSE;
    જો(data)
      Curl_multi_mark_dirty(data);
  }
  પાછા 0;
}

સ્થિર શૂન્ય cb_rand(uint8_t *dest, size_t destlen,
                    અચલ ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode result;
  (શૂન્ય)rand_ctx;

  result = Curl_rand(ખાલી, dest, destlen);
  જો(result) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
સ્થિર પૂર્ણાંક cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    શૂન્ય *user_data)
{
  CURLcode result;
  (શૂન્ય)tconn;
  (શૂન્ય)user_data;

  result = Curl_rand(ખાલી, cid->data, cidlen);
  જો(result)
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(ખાલી, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  જો(result)
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;

  પાછા 0;
}

#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
સ્થિર પૂર્ણાંક cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  સંરચના ngtcp2_stateless_reset_token *token, size_t cidlen, શૂન્ય *user_data)
{
  CURLcode result;
  (શૂન્ય)tconn;
  (શૂન્ય)user_data;

  result = Curl_rand(ખાલી, cid->data, cidlen);
  જો(result)
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(ખાલી, token->data, કદ(token->data));
  જો(result)
    પાછા NGTCP2_ERR_CALLBACK_FAILURE;

  પાછા 0;
}
#endif

સ્થિર પૂર્ણાંક cb_recv_rx_ચાવી(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          શૂન્ય *user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ખાલી;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  (શૂન્ય)tconn;

  જો(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    પાછા 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(data);
  જો(ctx && data && !ctx->h3conn && ctx->init_h3_conn_cb) {
    જો(ctx->init_h3_conn_cb(cf, data, ctx))
      પાછા NGTCP2_ERR_CALLBACK_FAILURE;
  }
  પાછા 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ધકેલો
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
સ્થિર ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  ખાલી, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_data_cb,
  cb_ngtcp2_handshake_completed,
  ખાલી, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_mask_cb,
  cb_recv_stream_data,
  cb_acked_stream_data_offset,
  ખાલી, /* stream_open */
  cb_stream_close,
  ખાલી, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  ખાલી, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  ખાલી, /* remove_connection_id */
  ngtcp2_crypto_update_ચાવી_cb, /* update_key */
  ખાલી, /* path_validation */
  ખાલી, /* select_preferred_addr */
  cb_stream_reset,
  ખાલી, /* extend_max_remote_streams_bidi */
  ખાલી, /* extend_max_remote_streams_uni */
  cb_extend_max_stream_data,
  ખાલી, /* dcid_status */
  ખાલી, /* handshake_confirmed */
  ખાલી, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  ખાલી, /* recv_datagram */
  ખાલી, /* ack_datagram */
  ખાલી, /* lost_datagram */
  ngtcp2_crypto_get_path_challenge_data_cb,
  cb_stream_stop_sending,
  ખાલી, /* version_negotiation */
  cb_recv_rx_ચાવી,
  ખાલી, /* recv_tx_key */
  ખાલી, /* early_data_rejected */
#ifdef NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  ખાલી, /* begin_path_validation */
#endif
#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  ખાલી, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  ખાલી, /* dcid_status2 */
  ngtcp2_crypto_get_path_challenge_data2_cb, /* get_path_challenge_data2 */
#endif
#ifdef NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  ખાલી, /* recv_stop_sending */
#endif
#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_stream_close2, /* is called instead of cb_stream_close when set */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic કાઢો
#endif

#જો defined(_MSC_VER) && defined(_DLL)
#pragma warning(કાઢો)
#endif

#ifdef USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
સ્થિર પૂર્ણાંક quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  સંરચના Curl_cfilter *cf;
  સંરચના cf_ngtcp2_ctx *ctx;
  સંરચના Curl_easy *data;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_data(ssl);
  cf = cref ? cref->user_data : ખાલી;
  ctx = cf ? cf->ctx : ખાલી;
  data = cf ? CF_DATA_CURRENT(cf) : ખાલી;
  જો(cf && data && ctx) {
    unsigned અક્ષર *quic_tp = ખાલી;
    size_t quic_tp_len = 0;
#ifdef HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     કદ(tpbuf));
    જો(tplen < 0)
      CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((પૂર્ણાંક)tplen));
    નહીંતર {
      quic_tp = (unsigned અક્ષર *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#endif
    Curl_ossl_add_session(cf, data, &ctx->tls.ossl, ctx->ssl_peer.scache_ચાવી,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, ખાલી);
  }
  પાછા 0;
}
#endif /* USE_OPENSSL */

#ifdef USE_GNUTLS

#ifdef CURLVERBOSE
સ્થિર અચલ અક્ષર *gtls_hs_સંદેશ_name(પૂર્ણાંક mtype)
{
  પસંદગી(mtype) {
  સ્થિતિ 1:
    પાછા "ClientHello";
  સ્થિતિ 2:
    પાછા "ServerHello";
  સ્થિતિ 4:
    પાછા "SessionTicket";
  સ્થિતિ 8:
    પાછા "EncryptedExtensions";
  સ્થિતિ 11:
    પાછા "Certificate";
  સ્થિતિ 13:
    પાછા "CertificateRequest";
  સ્થિતિ 15:
    પાછા "CertificateVerify";
  સ્થિતિ 20:
    પાછા "Finished";
  સ્થિતિ 24:
    પાછા "KeyUpdate";
  સ્થિતિ 254:
    પાછા "MessageHash";
  }
  પાછા "Unknown";
}
#endif

સ્થિર પૂર્ણાંક quic_gtls_handshake_cb(gnutls_session_t session, unsigned પૂર્ણાંક htype,
                                  unsigned when, unsigned પૂર્ણાંક incoming,
                                  અચલ gnutls_datum_t *સંદેશ)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  સંરચના Curl_cfilter *cf = conn_ref ? conn_ref->user_data : ખાલી;
  સંરચના cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ખાલી;

  (શૂન્ય)સંદેશ;
  (શૂન્ય)incoming;
  જો(when && cf && ctx) { /* after message has been processed */
    સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    જો(!data)
      પાછા 0;
    CURL_TRC_CF(data, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_સંદેશ_name(htype), htype);
    પસંદગી(htype) {
    સ્થિતિ GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned અક્ષર *quic_tp = ખાલી;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       કદ(tpbuf));
      જો(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((પૂર્ણાંક)tplen));
      નહીંતર {
        quic_tp = (unsigned અક્ષર *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (શૂન્ય)Curl_gtls_cache_session(cf, data, ctx->ssl_peer.scache_ચાવી,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    ખાલી);
      તોડો;
    }
    મૂળભૂત:
      તોડો;
    }
  }
  પાછા 0;
}
#endif /* USE_GNUTLS */

#ifdef USE_WOLFSSL
સ્થિર પૂર્ણાંક wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolfSSL_get_app_data(ssl);
  સંરચના Curl_cfilter *cf = conn_ref ? conn_ref->user_data : ખાલી;

  DEBUGASSERT(cf);
  જો(cf && session) {
    સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
    સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    જો(data && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned અક્ષર *quic_tp = ખાલી;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       કદ(tpbuf));
      જો(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((પૂર્ણાંક)tplen));
      નહીંતર {
        quic_tp = (unsigned અક્ષર *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (શૂન્ય)Curl_wssl_cache_session(cf, data, ctx->ssl_peer.scache_ચાવી,
                                    session, wolfSSL_version(ssl),
                                    "h3", quic_tp, quic_tp_len, ખાલી);
    }
  }
  પાછા 0;
}
#endif /* USE_WOLFSSL */

સ્થિર CURLcode cf_ngtcp2_tls_ctx_setup(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        શૂન્ય *user_data)
{
  સંરચના curl_tls_ctx *ctx = user_data;

#ifdef USE_OPENSSL
#જો defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  જો(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(data, "ngtcp2_crypto_boringssl_configure_client_context failed");
    પાછા CURLE_FAILED_INIT;
  }
#અથવાજો defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#નહીંતર
  જો(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_quictls_configure_client_context failed");
    પાછા CURLE_FAILED_INIT;
  }
#endif /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  જો(Curl_ssl_scache_use(cf, data)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_set_session_cache_mode(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#અથવાજો defined(USE_GNUTLS)
  જો(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(data, "ngtcp2_crypto_gnutls_configure_client_session failed");
    પાછા CURLE_FAILED_INIT;
  }
  જો(Curl_ssl_scache_use(cf, data)) {
    gnutls_handshake_set_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#અથવાજો defined(USE_WOLFSSL)
  જો(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    પાછા CURLE_FAILED_INIT;
  }
  જો(Curl_ssl_scache_use(cf, data)) {
    /* Register to get notified when a new session is received */
    wolfSSL_CTX_sess_set_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#endif
  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_ngtcp2_on_session_reuse(સંરચના Curl_cfilter *cf,
                                           સંરચના Curl_easy *data,
                                           સંરચના alpn_spec *alpns,
                                           સંરચના Curl_ssl_session *scs,
                                           bool *do_early_data)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;
#જો defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#endif
#ifdef USE_GNUTLS
  ctx->earlydata_max =
    gnutls_record_get_max_early_data_size(ctx->tls.gtls.session);
#endif
#ifdef USE_WOLFSSL
#ifdef WOLFSSL_EARLY_DATA
  ctx->earlydata_max = scs->earlydata_max;
#નહીંતર
  ctx->earlydata_max = 0;
#endif /* WOLFSSL_EARLY_DATA */
#endif
#જો defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  જો(!ctx->earlydata_max) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  નહીંતર જો(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session from different ALPN, no early data");
  }
  નહીંતર જો(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(data, cf, "no 0RTT transport parameters, no early data");
  }
  નહીંતર {
    પૂર્ણાંક rv;
    rv = ngtcp2_conn_decode_and_set_0rtt_transport_params(
      ctx->qconn, (અચલ uint8_t *)scs->quic_tp, scs->quic_tp_len);
    જો(rv)
      CURL_TRC_CF(data, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    નહીંતર જો(ctx->init_h3_conn_cb) {
      infof(data, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      result = ctx->init_h3_conn_cb(cf, data, ctx);
      જો(!result) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_data = TRUE;
      }
    }
    નહીંતર { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_data = TRUE;
    }
  }
#નહીંતર /* not supported in the TLS backend */
  (શૂન્ય)data;
  (શૂન્ય)ctx;
  (શૂન્ય)scs;
  (શૂન્ય)alpns;
#endif
  પાછા result;
}

/*
 * Might be called twice for happy eyeballs.
 */
સ્થિર CURLcode cf_connect_start(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના cf_ngtcp2_io_ctx *pktx)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  પૂર્ણાંક rc;
  પૂર્ણાંક rv;
  CURLcode result;
  અચલ સંરચના Curl_sockaddr_ex *sockaddr = ખાલી;
  પૂર્ણાંક qfd;
  સ્થિર અચલ સંરચના alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->dcid.data, NGTCP2_MAX_CIDLEN);
  જો(result)
    પાછા result;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->scid.data, NGTCP2_MAX_CIDLEN);
  જો(result)
    પાછા result;

  (શૂન્ય)Curl_qlogdir(data, ctx->scid.data, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, data, pktx);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  જો(result)
    પાછા result;

  /* Query socket and remote address from sub-chain */
  જો(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, ખાલી)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  જો(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = કદ(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (સંરચના sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    જો(rv == -1)
      પાછા CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (સંરચના sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_path.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  નહીંતર {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    અચલ સંરચના Curl_sockaddr_ex *remote = ખાલી;
    જો(cf->next->cft->query(cf->next, data, CF_QUERY_REMOTE_ADDR, ખાલી,
                            CURL_UNCONST(&remote)))
      પાછા CURLE_QUIC_CONNECT_ERROR;
    જો(!remote)
      પાછા CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, કદ(ctx->q.local_addr));
    પસંદગી(remote->family) {
    સ્થિતિ AF_INET:
      ((સંરચના sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = કદ(સંરચના sockaddr_in);
      તોડો;
#ifdef USE_IPV6
    સ્થિતિ AF_INET6:
      ((સંરચના sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = કદ(સંરચના sockaddr_in6);
      તોડો;
#endif
    મૂળભૂત:
      પાછા CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (સંરચના sockaddr *)&ctx->q.local_addr,
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
  જો(rc)
    પાછા CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_data = cf;

  result = Curl_vquic_tls_init(&ctx->tls, cf, data,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_setup, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  જો(result)
    પાછા result;

#જો defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  જો(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_ctx_new failed");
    પાછા CURLE_FAILED_INIT;
  }
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  જો(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_configure_client_session failed");
    પાછા CURLE_FAILED_INIT;
  }
#અથવાજો defined(USE_OPENSSL)
  SSL_set_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#અથવાજો defined(USE_GNUTLS)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#અથવાજો defined(USE_WOLFSSL)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#નહીંતર
#error "ngtcp2 TLS backend not defined"
#endif

#જો defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  જો(ctx->use_earlydata &&
     !SSL_set_quic_tls_early_data_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(data, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#endif
  ngtcp2_ccerr_default(&ctx->last_error);

  પાછા CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    bool *done)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  સંરચના cf_call_data save;
  સંરચના cf_ngtcp2_io_ctx pktx;

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  /* Connect the sub-chain */
  જો(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    જો(result || !*done)
      પાછા result;
  }

  *done = FALSE;

  જો(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    પાછા CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  CF_DATA_SAVE(save, cf, data);

  જો(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(data);
    result = cf_connect_start(cf, data, &pktx);
    જો(result)
      જાઓ out;
    જો(cf->connected) {
      *done = TRUE;
      જાઓ out;
    }
    result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
    /* we do not expect to be able to recv anything yet */
    જાઓ out;
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  જો(result)
    જાઓ out;

  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
  જો(result)
    જાઓ out;

  જો(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    result = ctx->tls_vrfy_result;
    જો(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      cf->connected = TRUE;
      *done = TRUE;
    }
  }

out:
  જો(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
  જો(ctx->qconn &&
     ((result == CURLE_RECV_ERROR) || (result == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_in_draining_period(ctx->qconn)) {
    અચલ ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    result = CURLE_COULDNT_CONNECT;
    જો(cerr) {
      CURL_TRC_CF(data, cf, "connect error, type=%d, code=%" PRIu64,
                  (પૂર્ણાંક)cerr->type, cerr->error_code);
      પસંદગી(cerr->type) {
      સ્થિતિ NGTCP2_CCERR_TYPE_VERSION_NEGOTIATION:
        CURL_TRC_CF(data, cf, "error in version negotiation");
        તોડો;
      મૂળભૂત:
        જો(cerr->error_code >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(data, cf, "crypto error, tls alert=%u",
                      (unsigned પૂર્ણાંક)(cerr->error_code & 0xffU));
        }
        નહીંતર જો(cerr->error_code == NGTCP2_CONNECTION_REFUSED) {
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
  જો(result) {
    જો(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      સંરચના ip_quadruple ip;

      જો(!Curl_cf_socket_peek(cf->next, data, ખાલી, ખાલી, &ip))
        infof(data, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    }
  }
#endif
  જો(!result && ctx->qconn) {
    result = Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx);
  }
  જો(result || *done)
    CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (પૂર્ણાંક)result, *done);
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(સંરચના Curl_cfilter *cf,
                                     સંરચના Curl_easy *data, bool *done)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના cf_call_data save;
  સંરચના cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  જો(cf->shutdown || !ctx->qconn) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  જો(!cf->next) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *done = TRUE;
    પાછા CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);
  *done = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  જો(!ctx->shutdown_started) {
    અક્ષર buffer[NGTCP2_MAX_UDP_PAYLOAD_SIZE];
    ngtcp2_ssize nwritten;

    જો(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
      CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
      result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
      જો(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
        CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
        result = CURLE_OK;
        જાઓ out;
      }
      નહીંતર જો(result) {
        CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                    (પૂર્ણાંક)result);
        *done = TRUE;
        જાઓ out;
      }
    }

    DEBUGASSERT(Curl_bufq_is_empty(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, ખાલી, /* path */
      ખાલી, /* pkt_info */
      (uint8_t *)buffer, કદ(buffer),
      &ctx->last_error, pktx.ts);
    CURL_TRC_CF(data, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (પૂર્ણાંક)ctx->last_error.type,
                ctx->last_error.error_code, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    જો(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      result = Curl_bufq_write(&ctx->q.sendbuf, (અચલ unsigned અક્ષર *)buffer,
                               (size_t)nwritten, &n);
      જો(result) {
        CURL_TRC_CF(data, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (પૂર્ણાંક)result);
        જાઓ out;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.split_len = 0;
    }
  }

  જો(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing egress");
    result = Curl_vquic_flush(cf, data, &ctx->q);
    જો(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
      result = CURLE_OK;
      જાઓ out;
    }
    નહીંતર જો(result) {
      CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                  (પૂર્ણાંક)result);
      *done = TRUE;
      જાઓ out;
    }
  }

  જો(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
    result = CURLE_OK;
  }
out:
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

શૂન્ય Curl_cf_ngtcp2_cmn_conn_close(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  bool done;
  Curl_cf_ngtcp2_cmn_shutdown(cf, data, &done);
}

સ્થિર bool cf_ngtcp2_err_is_fatal(પૂર્ણાંક code)
{
  પાછા (NGTCP2_ERR_FATAL >= code) ||
         (NGTCP2_ERR_DROP_CONN == code) ||
         (NGTCP2_ERR_IDLE_CLOSE == code);
}

શૂન્ય Curl_cf_ngtcp2_cmn_err_set(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data, પૂર્ણાંક code)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  જો(!ctx->last_error.error_code) {
    જો(NGTCP2_ERR_CRYPTO == code) {
      ngtcp2_ccerr_set_tls_alert(&ctx->last_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 ખાલી, 0);
    }
    નહીંતર {
      ngtcp2_ccerr_set_liberr(&ctx->last_error, code, ખાલી, 0);
    }
  }
  જો(cf_ngtcp2_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

શૂન્ય Curl_cf_ngtcp2_io_ctx_init(સંરચના cf_ngtcp2_io_ctx *io_ctx,
                                સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->data = data;
  io_ctx->now = *Curl_pgrs_now(data);
  ngtcp2_path_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_set_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

શૂન્ય Curl_cf_ngtcp2_io_ctx_update_time(સંરચના Curl_easy *data,
                                       સંરચના cf_ngtcp2_io_ctx *io_ctx,
                                       સંરચના Curl_cfilter *cf)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(data);
  Curl_vquic_ctx_update_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#જો NGTCP2_VERSION_અંક < 0x011100
સંરચના cf_ngtcp2_sfind_ctx {
  int64_t stream_id;
  સંરચના h3_stream_ctx *stream;
  uint32_t mid;
};

સ્થિર bool cf_ngtcp2_sfind(uint32_t mid, શૂન્ય *કિંમત, શૂન્ય *user_data)
{
  સંરચના cf_ngtcp2_sfind_ctx *fctx = user_data;
  સંરચના h3_stream_ctx *stream = કિંમત;

  જો(fctx->stream_id == stream->id) {
    fctx->mid = mid;
    fctx->stream = stream;
    પાછા FALSE;
  }
  પાછા TRUE; /* continue */
}

સ્થિર સંરચના h3_stream_ctx *cf_ngtcp2_get_stream(સંરચના cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  સંરચના cf_ngtcp2_sfind_ctx fctx;
  fctx.stream_id = stream_id;
  fctx.stream = ખાલી;
  Curl_u32_ptrset_visit(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  પાછા fctx.stream;
}
#નહીંતર
સ્થિર સંરચના h3_stream_ctx *cf_ngtcp2_get_stream(સંરચના cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  પાછા ngtcp2_conn_get_stream_user_data(ctx->qconn, stream_id);
}
#endif

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
સ્થિર CURLcode read_pkt_to_send(શૂન્ય *userp,
                                 unsigned અક્ષર *buf, size_t buflen,
                                 size_t *pnread)
{
  સંરચના cf_ngtcp2_io_ctx *x = userp;
  સંરચના cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t stream_id;
  પૂર્ણાંક fin;
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
  માટે(;;) {

    જો(ctx->h3conn && ngtcp2_conn_get_max_data_ડાબું(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_stream(ctx->h3conn, &stream_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      જો(veccnt < 0) {
        failf(x->data, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((પૂર્ણાંક)veccnt));
        Curl_cf_ngtcp2_h3_err_set(x->cf, x->data, (પૂર્ણાંક)veccnt);
        પાછા CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_STREAM_FLAG_MORE |
            (fin ? NGTCP2_WRITE_STREAM_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_stream(ctx->qconn, &x->ps.path,
                                  ખાલી, buf, buflen,
                                  &ndatalen, flags, stream_id,
                                  (અચલ ngtcp2_vec *)vec, veccnt, x->ts);
    જો(n == 0) {
      /* nothing to send */
      પાછા CURLE_AGAIN;
    }
    નહીંતર જો(n < 0) {
      પસંદગી(n) {
      સ્થિતિ NGTCP2_ERR_STREAM_DATA_BLOCKED: {
        સંરચના h3_stream_ctx *stream;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_block_stream(ctx->h3conn, stream_id);
        CURL_TRC_CF(x->data, x->cf, "[%" PRId64 "] block quic flow",
                    stream_id);
        stream = cf_ngtcp2_get_stream(ctx, stream_id);
        જો(stream) /* it might be not one of our h3 streams? */
          stream->quic_flow_blocked = TRUE;
        n = 0;
        તોડો;
      }
      સ્થિતિ NGTCP2_ERR_STREAM_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_stream_write(ctx->h3conn, stream_id);
        n = 0;
        તોડો;
      સ્થિતિ NGTCP2_ERR_WRITE_MORE:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        તોડો;
      મૂળભૂત:
        DEBUGASSERT(ndatalen == -1);
        failf(x->data, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((પૂર્ણાંક)n));
        Curl_cf_ngtcp2_cmn_err_set(x->cf, x->data, (પૂર્ણાંક)n);
        પાછા CURLE_SEND_ERROR;
      }
    }

    જો(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      પૂર્ણાંક rv = nghttp3_conn_add_write_offset(ctx->h3conn, stream_id, ndatalen);
      જો(rv) {
        failf(x->data, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        પાછા CURLE_SEND_ERROR;
      }
    }

    જો(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      પાછા CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_progress_egress(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        સંરચના cf_ngtcp2_io_ctx *pktx)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_size, path_max_payload_size;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_quantum;
  CURLcode result;
  સંરચના cf_ngtcp2_io_ctx local_pktx;

  જો(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  નહીંતર {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  જો(result) {
    જો(result == CURLE_AGAIN) {
      Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
      પાછા CURLE_OK;
    }
    પાછા result;
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
  માટે(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_size,
                            read_pkt_to_send, pktx, &nread);
    જો(result == CURLE_AGAIN)
      તોડો;
    નહીંતર જો(result)
      પાછા result;
    નહીંતર {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      જો((buflen >= send_quantum) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_size))
        તોડો;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      જો(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      નહીંતર જો(nread > gsolen ||
              (gsolen > path_max_payload_size && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        result = Curl_vquic_send_tail_split(cf, data, &ctx->q,
                                            gsolen, nread, nread);
        જો(result) {
          જો(result == CURLE_AGAIN) {
            Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
            પાછા CURLE_OK;
          }
          પાછા result;
        }
        pktcnt = 0;
      }
      નહીંતર જો(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        તોડો;
      }
    }
  }

  જો(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(data, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
    જો(result) {
      જો(result == CURLE_AGAIN) {
        Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
        પાછા CURLE_OK;
      }
      પાછા result;
    }
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_conn_update_pkt_tx_time(ctx->qconn, pktx->ts);
  }
  પાછા CURLE_OK;
}

સંરચના cf_ngtcp2_recv_ctx {
  સંરચના cf_ngtcp2_io_ctx *pktx;
  size_t pkt_ગણતરી;
};

સ્થિર CURLcode cf_ngtcp2_recv_pkts(અચલ unsigned અક્ષર *buf, size_t buflen,
                                    size_t gso_size,
                                    સંરચના sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    શૂન્ય *userp)
{
  સંરચના cf_ngtcp2_recv_ctx *rctx = userp;
  સંરચના cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  સંરચના cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_path path;
  size_t offset, pktlen;
  પૂર્ણાંક rv;

  જો(!rctx->pkt_ગણતરી) {
    Curl_cf_ngtcp2_io_ctx_update_time(pktx->data, pktx, pktx->cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  જો(ecn)
    CURL_TRC_CF(pktx->data, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_size, (unsigned)ecn);
  ngtcp2_addr_init(&path.local, (સંરચના sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&path.remote, (સંરચના sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  માટે(offset = 0; offset < buflen; offset += gso_size) {
    rctx->pkt_ગણતરી++;
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &path, &pi,
                              buf + offset, pktlen, pktx->ts);
    જો(rv) {
      CURL_TRC_CF(pktx->data, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_set(pktx->cf, pktx->data, rv);

      જો(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        પાછા CURLE_PEER_FAILED_VERIFICATION;
      પાછા CURLE_RECV_ERROR;
    }
  }
  પાછા CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_progress_ingress(સંરચના Curl_cfilter *cf,
                                         સંરચના Curl_easy *data,
                                         સંરચના cf_ngtcp2_io_ctx *pktx)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના cf_ngtcp2_io_ctx local_pktx;
  સંરચના cf_ngtcp2_recv_ctx rctx;
  CURLcode result = CURLE_OK;

  જો(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }

  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  જો(result)
    પાછા result;

  rctx.pktx = pktx;
  rctx.pkt_ગણતરી = 0;

  જો(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(data, cf, "progress_ingress(socket)");
    પાછા Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  નહીંતર {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    unsigned અક્ષર *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_SIZE;
    size_t pkt_limit = QUIC_TUNNEL_INGRESS_PKT_LIMIT;
    size_t nread;
    સંરચના sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(data, cf, "progress_ingress(sub-filters)");
    જો(ctx->qconn) {
      size_t max_path_payload;
      max_path_payload =
        ngtcp2_conn_get_path_max_tx_udp_payload_size(ctx->qconn);
      જો(max_path_payload > max_udp_payload)
        max_udp_payload = max_path_payload;
    }

    જો(ctx->tunnel_inbuf_len < max_udp_payload) {
      unsigned અક્ષર *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      જો(!newbuf)
        પાછા CURLE_OUT_OF_MEMORY;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    જ્યારે(pkt_limit--) {
      result = Curl_conn_cf_recv(cf->next, data, (અક્ષર *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      જો(result == CURLE_AGAIN) {
        /* no more data available at the moment */
        પાછા CURLE_OK;
      }
      જો(result) {
        CURL_TRC_CF(data, cf, "ingress, recv from tunnel failed: %d",
                    (પૂર્ણાંક)result);
        પાછા result;
      }
      જો(nread == 0) {
        /* tunnel closed */
        પાછા CURLE_OK;
      }

      memcpy(&remote_addr, ctx->connected_path.remote.addr,
             ctx->connected_path.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_path.remote.addrlen;
      result = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      જો(result)
        પાછા result;

      જો(!ctx->q.got_first_byte) {
        ctx->q.got_first_byte = TRUE;
        ctx->q.first_byte_at = ctx->q.last_op;
      }
      ctx->q.last_io = ctx->q.last_op;
    }
    પાછા CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_set_expiry(સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data,
                                       સંરચના cf_ngtcp2_io_ctx *pktx)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  જો(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  નહીંતર {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  જો(expiry != UINT64_MAX) {
    જો(expiry <= pktx->ts) {
      CURLcode result;
      પૂર્ણાંક rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      જો(rv) {
        failf(data, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_set(cf, data, rv);
        પાછા CURLE_SEND_ERROR;
      }
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, pktx);
      જો(!result)
        result = Curl_cf_ngtcp2_progress_egress(cf, data, pktx);
      જો(result) {
        /* a verify failure during ingress must win over generic errors */
        જો(ctx->tls_vrfy_result)
          result = ctx->tls_vrfy_result;
        પાછા result;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    જો(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      જો(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_set(data, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cf_ngtcp2_setup_keep_alive(સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  અચલ ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  જો(!ctx->qconn)
    પાછા;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  જો(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no peer idle timeout, unset keep-alive");
  }
  નહીંતર જો(!Curl_u32_ptrset_ગણતરી(&ctx->streams)) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no active streams, unset keep-alive");
  }
  નહીંતર {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(data, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_stream_setup(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        સંરચના h3_stream_ctx **pstream)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  *pstream = ખાલી;
  જો(!data)
    પાછા CURLE_FAILED_INIT;

  જો(stream) {
    *pstream = stream;
    પાછા CURLE_OK;
  }

  stream = curlx_calloc(1, કદ(*stream));
  જો(!stream)
    પાછા CURLE_OUT_OF_MEMORY;

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
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_લીટી_LEN);

  જો(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    Curl_cf_ngtcp2_h3_stream_ctx_free(stream);
    પાછા CURLE_OUT_OF_MEMORY;
  }

  જો(Curl_u32_ptrset_ગણતરી(&ctx->streams) == 1)
    cf_ngtcp2_setup_keep_alive(cf, data);

  *pstream = stream;
  પાછા CURLE_OK;
}

શૂન્ય Curl_cf_ngtcp2_h3_stream_close(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    સંરચના h3_stream_ctx *stream)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(data);
  DEBUGASSERT(stream);
  જો(!stream->closed && ctx->qconn && ctx->h3conn) {
    CURLcode result;

    nghttp3_conn_set_stream_user_data(ctx->h3conn, stream->id, ખાલી);
    ngtcp2_conn_set_stream_user_data(ctx->qconn, stream->id, ખાલી);
    stream->closed = TRUE;
    (શૂન્ય)ngtcp2_conn_shutdown_stream(ctx->qconn, 0, stream->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    result = Curl_cf_ngtcp2_progress_egress(cf, data, ખાલી);
    જો(result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cancel stream -> %d",
                  stream->id, (પૂર્ણાંક)result);
  }
}

શૂન્ય Curl_cf_ngtcp2_h3_stream_done(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  (શૂન્ય)cf;
  જો(stream) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] easy handle is done", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
    જો(!Curl_u32_ptrset_ગણતરી(&ctx->streams))
      cf_ngtcp2_setup_keep_alive(cf, data);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      bool *આગત_pending)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  અચલ ngtcp2_transport_params *rp;
  સંરચના cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  *આગત_pending = FALSE;
  જો(!ctx->qconn || ctx->shutdown_started)
    જાઓ out;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  જો(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->q.last_io);
    જો(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      જો((uint64_t)idletime_ms > max_idle_ms)
        જાઓ out;
    }
  }

  જો(!cf->next || !cf->next->cft->is_alive(cf->next, data, આગત_pending))
    જાઓ out;

  alive = TRUE;
  જો(*આગત_pending) {
    CURLcode result;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *આગત_pending = FALSE;
    result = Curl_cf_ngtcp2_progress_ingress(cf, data, ખાલી);
    CURL_TRC_CF(data, cf, "is_alive, progress ingress -> %d", (પૂર્ણાંક)result);
    alive = result ? FALSE : TRUE;
  }

out:
  CF_DATA_RESTORE(cf, save);
  પાછા alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(સંરચના cf_ngtcp2_ctx *ctx,
                                      સંરચના Curl_easy *data)
{
  int64_t ctrl_stream_id, qpack_enc_stream_id, qpack_dec_stream_id;
  પૂર્ણાંક rc;

  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &ctrl_stream_id, ખાલી);
  જો(rc) {
    failf(data, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    પાછા CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_stream(ctx->h3conn, ctrl_stream_id);
  જો(rc) {
    failf(data, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    પાછા CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_enc_stream_id, ખાલી);
  જો(rc) {
    failf(data, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    પાછા CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_dec_stream_id, ખાલી);
  જો(rc) {
    failf(data, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    પાછા CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_stream_id,
                                       qpack_dec_stream_id);
  જો(rc) {
    failf(data, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    પાછા CURLE_QUIC_CONNECT_ERROR;
  }
  પાછા CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;

  પસંદગી(query) {
  સ્થિતિ CF_QUERY_CONNECT_REPLY_MS:
    જો((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (પૂર્ણાંક)ms : INT_MAX;
      પાછા CURLE_OK;
    }
    તોડો;
  સ્થિતિ CF_QUERY_REALLY_CONNECTED:
    જો(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_first_byte;
      પાછા CURLE_OK;
    }
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  પાછા cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (શૂન્ય)arg1;
  (શૂન્ય)arg2;
  પસંદગી(event) {
  સ્થિતિ CF_CTRL_REPORT_STATS:
    જો(cf->connected && !ctx->stats_reported) {
      જો((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        ctx->stats_reported = TRUE;
      }
      નહીંતર જો(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        જો(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  પાછા result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
