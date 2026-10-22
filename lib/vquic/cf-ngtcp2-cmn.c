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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2.h>

#ifdef USE_OPENSSL
#ଅନ୍ତର୍ଭାଵ <openssl/err.h>
#ଯଦି defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2_crypto_boringssl.h>
#ନଚେତଯଦି defined(OPENSSL_QUIC_API2)
#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2_crypto_ossl.h>
#ଅନ୍ୟଥା
#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#ଅନ୍ତର୍ଭାଵ "vtls/openssl.h"
#ନଚେତଯଦି defined(USE_GNUTLS)
#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2_crypto_gnutls.h>
#ଅନ୍ତର୍ଭାଵ "vtls/gtls.h"
#ନଚେତଯଦି defined(USE_WOLFSSL)
#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ଅନ୍ତର୍ଭାଵ "vtls/wolfssl.h"
#endif

#ଅନ୍ତର୍ଭାଵ <nghttp3/nghttp3.h>

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "u32_ptrset.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "cf-socket.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "http1.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "sockaddr.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "vdns/cf-dns.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic_int.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic-tls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_scache.h"
#ଅନ୍ତର୍ଭାଵ "vquic/cf-ngtcp2-cmn.h"

/*
 * Store ngtcp2 version info in this buffer.
 */
ଶୂନ୍ୟ Curl_ngtcp2_ver(ଅକ୍ଷର *p, size_t len)
{
  ଅଚଳ ngtcp2_info *ng2 = ngtcp2_version(0);
  ଅଚଳ nghttp3_info *ht3 = nghttp3_version(0);
  (ଶୂନ୍ୟ)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->version_str, ht3->version_str);
}

ଶୂନ୍ୟ Curl_cf_ngtcp2_h3_stream_ctx_free(ସଂରଚନା h3_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

ସ୍ଥିର ଶୂନ୍ୟ h3_stream_set_free(uint32_t id, ଶୂନ୍ୟ *stream)
{
  (ଶୂନ୍ୟ)id;
  DEBUGASSERT(stream);
  Curl_cf_ngtcp2_h3_stream_ctx_free((ସଂରଚନା h3_stream_ctx *)stream);
}

ସ୍ଥିର bool cf_ngtcp2_h3_err_is_fatal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
  ଫେରନ୍ତୁ (NGHTTP3_ERR_FATAL >= code) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM == code);
}

ଶୂନ୍ୟ Curl_cf_ngtcp2_h3_err_set(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ଯଦି(!ctx->last_error.error_code) {
    ngtcp2_ccerr_set_application_error(&ctx->last_error,
      nghttp3_err_infer_quic_app_error_code(code), ଖାଲି, 0);
  }
  ଯଦି(cf_ngtcp2_h3_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

CURLcode Curl_cf_ngtcp2_ctx_init(ସଂରଚନା cf_ngtcp2_ctx *ctx,
                                 ସଂରଚନା Curl_peer *origin,
                                 ସଂରଚନା Curl_peer *peer,
                                 ସଂରଚନା ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = ଖାଲି;
  ctx->tunnel_inbuf_len = 0;
  ctx->version = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->stream_bufcp, H3_STREAM_CHUNK_SIZE,
                  H3_STREAM_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  ଫେରନ୍ତୁ Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

ଶୂନ୍ୟ Curl_cf_ngtcp2_ctx_cleanup(ସଂରଚନା cf_ngtcp2_ctx *ctx)
{
  ଯଦି(ctx && ctx->initialized) {
    ଯଦି(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    ଯଦି(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = ଖାଲି;
    }
    ଯଦି(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = ଖାଲି;
    }
#ifdef OPENSSL_QUIC_API2
    ଯଦି(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = ଖାଲି;
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

ସଂରଚନା Curl_easy *Curl_cf_ngtcp2_get_xfer(ସଂରଚନା Curl_cfilter *cf,
                                          ଶୂନ୍ୟ *stream_user_data,
                                          ସଂରଚନା h3_stream_ctx **pstream)
{
  ସଂରଚନା h3_stream_ctx *stream = stream_user_data;

  ଯଦି(pstream)
    *pstream = stream;
  ଯଦି(stream && (stream->mid != UINT32_MAX)) {
    ସଂରଚନା Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    ଯଦି(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      ଫେରନ୍ତୁ ଖାଲି;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    ଯଦି(data && (data->id == stream->xfer_id))
      ଫେରନ୍ତୁ data;
    DEBUGASSERT(0);
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  ସଂରଚନା Curl_cfilter *cf = conn_ref->user_data;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ଫେରନ୍ତୁ ctx->qconn;
}

#ifdef DEBUG_NGTCP2
ସ୍ଥିର ଶୂନ୍ୟ quic_printf(ଶୂନ୍ୟ *user_data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  va_list ap;
  (ଶୂନ୍ୟ)user_data;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#endif

ସ୍ଥିର ଶୂନ୍ୟ qlog_callback(ଶୂନ୍ୟ *user_data, uint32_t flags,
                          ଅଚଳ ଶୂନ୍ୟ *data, size_t datalen)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  (ଶୂନ୍ୟ)flags;
  ଯଦି(ctx->qlogfd != -1) {
    ssize_t rc = ଲିଖ(ctx->qlogfd, data, datalen);
    ଯଦି(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

ସ୍ଥିର ଶୂନ୍ୟ quic_settings(ସଂରଚନା cf_ngtcp2_ctx *ctx,
                          ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#ifdef DEBUG_NGTCP2
  s->log_printf = quic_printf;
#ଅନ୍ୟଥା
  s->log_printf = ଖାଲି;
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
  ଯଦି(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#ଯଦି defined(_MSC_VER) && defined(_DLL)
#pragma warning(ଠେଲ)
#pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, ଶୂନ୍ୟ *user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ଖାଲି;
  ସଂରଚନା Curl_easy *data;

  (ଶୂନ୍ୟ)tconn;
  DEBUGASSERT(ctx);
  data = CF_DATA_CURRENT(cf);
  DEBUGASSERT(data);
  ଯଦି(!ctx || !data)
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(data);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, data);

  ctx->tls_vrfy_result = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    data, &ctx->ssl_peer);
  ଯଦି(ctx->tls_vrfy_result)
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;

#ifdef CURLVERBOSE
  ଯଦି(Curl_trc_is_verbose(data)) {
    ଅଚଳ ngtcp2_transport_params *rp;
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
  ଯଦି(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  ଯଦି(ctx->use_earlydata) {
#ଯଦି defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    ଯଦି(SSL_get_early_data_status(ctx->tls.ossl.ssl) == SSL_EARLY_DATA_NOT_SENT)
      CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
#endif

#ଯଦି NGTCP2_VERSION_ଅଙ୍କ >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_data_rejected2(ctx->qconn);
#ଅନ୍ୟଥା /* older NGTCP2 */
    {
#ଯଦି defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ossl_early_status = SSL_get_early_data_status(ctx->tls.ossl.ssl);
      ଯଦି(ossl_early_status == SSL_EARLY_DATA_NOT_SENT)
        CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_status == SSL_EARLY_DATA_ACCEPTED);
#ନଚେତଯଦି defined(USE_GNUTLS)
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_DATA);
#ନଚେତଯଦି defined(USE_WOLFSSL)
#ifdef WOLFSSL_EARLY_DATA
      ctx->earlydata_accepted =
        (wolfSSL_get_early_data_status(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_DATA_REJECTED);
#ଅନ୍ୟଥା
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
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_recv_stream_data(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t stream_id, uint64_t offset,
                               ଅଚଳ uint8_t *buf, size_t buflen,
                               ଶୂନ୍ୟ *user_data, ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fin = (flags & NGTCP2_STREAM_DATA_FLAG_FIN) ? 1 : 0;
  ସଂରଚନା h3_stream_ctx *stream;
  ସଂରଚନା Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (ଶୂନ୍ୟ)offset;
  rc = nghttp3_conn_read_stream(ctx->h3conn, stream_id, buf, buflen, fin);
  ଯଦି(rc < 0) {
    ଯଦି(data && stream) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  stream_id, stream->reset, stream->closed);
    }
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  ଯଦି(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_stream_offset(tconn, stream_id, nconsumed);
    ngtcp2_conn_extend_max_offset(tconn, nconsumed);
    ଯଦି(stream) {
      stream->rx_offset += nconsumed;
      stream->rx_offset_max += nconsumed;
    }
  }
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_acked_stream_data_offset(ngtcp2_conn *tconn, int64_t stream_id,
                                       uint64_t offset, uint64_t datalen,
                                       ଶୂନ୍ୟ *user_data, ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;
  (ଶୂନ୍ୟ)stream_id;
  (ଶୂନ୍ୟ)tconn;
  (ଶୂନ୍ୟ)offset;
  (ଶୂନ୍ୟ)datalen;
  (ଶୂନ୍ୟ)stream_user_data;

  rv = nghttp3_conn_add_ack_offset(ctx->h3conn, stream_id, datalen);
  ଯଦି(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_stream_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id, uint64_t app_error_code,
                           ଶୂନ୍ୟ *user_data, ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ଖାଲି);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;

  (ଶୂନ୍ୟ)tconn;
  /* stream is closed... */
  ଯଦି(!data)
    data = CF_DATA_CURRENT(cf);
  ଯଦି(!data)
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;

  ଯଦି(!(flags & NGTCP2_STREAM_CLOSE_FLAG_APP_ERROR_CODE_SET)) {
    app_error_code = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, app_error_code, rv);
  ଯଦି(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ଫେରନ୍ତୁ 0;
}

#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_stream_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id,
                           uint64_t rx_app_error_code,
                           uint64_t tx_app_error_code,
                           ଶୂନ୍ୟ *user_data, ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ଖାଲି);
  uint64_t h3_app_error_code = NGHTTP3_H3_NO_ERROR;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;

  (ଶୂନ୍ୟ)tconn;
  (ଶୂନ୍ୟ)tx_app_error_code;
  /* stream is closed... */
  ଯଦି(!data)
    data = CF_DATA_CURRENT(cf);
  ଯଦି(!data)
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;

  ଯଦି(flags & NGTCP2_STREAM_CLOSE2_FLAG_RX_APP_ERROR_CODE_SET)
    h3_app_error_code = rx_app_error_code;

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, h3_app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, h3_app_error_code, rv);
  ଯଦି(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ଫେରନ୍ତୁ 0;
}
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_stream_reset(ngtcp2_conn *tconn, int64_t stream_id,
                           uint64_t final_size, uint64_t app_error_code,
                           ଶୂନ୍ୟ *user_data, ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ଖାଲି);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;
  (ଶୂନ୍ୟ)tconn;
  (ଶୂନ୍ୟ)final_size;
  (ଶୂନ୍ୟ)app_error_code;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  ଯଦି(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  ଯଦି(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_stream_stop_sending(ngtcp2_conn *tconn, int64_t stream_id,
                                  uint64_t app_error_code, ଶୂନ୍ୟ *user_data,
                                  ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;
  (ଶୂନ୍ୟ)tconn;
  (ଶୂନ୍ୟ)app_error_code;
  (ଶୂନ୍ୟ)stream_user_data;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  ଯଦି(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            ଶୂନ୍ୟ *user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);

  (ଶୂନ୍ୟ)tconn;
  ctx->max_bidi_streams = max_streams;
  ଯଦି(data)
    CURL_TRC_CF(data, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->used_bidi_streams);
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_extend_max_stream_data(ngtcp2_conn *tconn, int64_t stream_id,
                                     uint64_t max_data, ଶୂନ୍ୟ *user_data,
                                     ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ସଂରଚନା h3_stream_ctx *stream;
  ସଂରଚନା Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;
  (ଶୂନ୍ୟ)tconn;
  (ଶୂନ୍ୟ)max_data;

  rv = nghttp3_conn_unblock_stream(ctx->h3conn, stream_id);
  ଯଦି(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  ଯଦି(stream && stream->quic_flow_blocked) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] unblock quic flow", stream_id);
    stream->quic_flow_blocked = FALSE;
    ଯଦି(data)
      Curl_multi_mark_dirty(data);
  }
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ cb_rand(uint8_t *dest, size_t destlen,
                    ଅଚଳ ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode result;
  (ଶୂନ୍ୟ)rand_ctx;

  result = Curl_rand(ଖାଲି, dest, destlen);
  ଯଦି(result) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    ଶୂନ୍ୟ *user_data)
{
  CURLcode result;
  (ଶୂନ୍ୟ)tconn;
  (ଶୂନ୍ୟ)user_data;

  result = Curl_rand(ଖାଲି, cid->data, cidlen);
  ଯଦି(result)
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(ଖାଲି, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  ଯଦି(result)
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;

  ଫେରନ୍ତୁ 0;
}

#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  ସଂରଚନା ngtcp2_stateless_reset_token *token, size_t cidlen, ଶୂନ୍ୟ *user_data)
{
  CURLcode result;
  (ଶୂନ୍ୟ)tconn;
  (ଶୂନ୍ୟ)user_data;

  result = Curl_rand(ଖାଲି, cid->data, cidlen);
  ଯଦି(result)
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(ଖାଲି, token->data, ଆକାର(token->data));
  ଯଦି(result)
    ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;

  ଫେରନ୍ତୁ 0;
}
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_recv_rx_ଚାବି(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          ଶୂନ୍ୟ *user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ଖାଲି;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  (ଶୂନ୍ୟ)tconn;

  ଯଦି(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    ଫେରନ୍ତୁ 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(data);
  ଯଦି(ctx && data && !ctx->h3conn && ctx->init_h3_conn_cb) {
    ଯଦି(ctx->init_h3_conn_cb(cf, data, ctx))
      ଫେରନ୍ତୁ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  ଫେରନ୍ତୁ 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
ସ୍ଥିର ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  ଖାଲି, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_data_cb,
  cb_ngtcp2_handshake_completed,
  ଖାଲି, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_mask_cb,
  cb_recv_stream_data,
  cb_acked_stream_data_offset,
  ଖାଲି, /* stream_open */
  cb_stream_close,
  ଖାଲି, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  ଖାଲି, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  ଖାଲି, /* remove_connection_id */
  ngtcp2_crypto_update_ଚାବି_cb, /* update_key */
  ଖାଲି, /* path_validation */
  ଖାଲି, /* select_preferred_addr */
  cb_stream_reset,
  ଖାଲି, /* extend_max_remote_streams_bidi */
  ଖାଲି, /* extend_max_remote_streams_uni */
  cb_extend_max_stream_data,
  ଖାଲି, /* dcid_status */
  ଖାଲି, /* handshake_confirmed */
  ଖାଲି, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  ଖାଲି, /* recv_datagram */
  ଖାଲି, /* ack_datagram */
  ଖାଲି, /* lost_datagram */
  ngtcp2_crypto_get_path_challenge_data_cb,
  cb_stream_stop_sending,
  ଖାଲି, /* version_negotiation */
  cb_recv_rx_ଚାବି,
  ଖାଲି, /* recv_tx_key */
  ଖାଲି, /* early_data_rejected */
#ifdef NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  ଖାଲି, /* begin_path_validation */
#endif
#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  ଖାଲି, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  ଖାଲି, /* dcid_status2 */
  ngtcp2_crypto_get_path_challenge_data2_cb, /* get_path_challenge_data2 */
#endif
#ifdef NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  ଖାଲି, /* recv_stop_sending */
#endif
#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_stream_close2, /* is called instead of cb_stream_close when set */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic କାଢ଼
#endif

#ଯଦି defined(_MSC_VER) && defined(_DLL)
#pragma warning(କାଢ଼)
#endif

#ifdef USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା cf_ngtcp2_ctx *ctx;
  ସଂରଚନା Curl_easy *data;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_data(ssl);
  cf = cref ? cref->user_data : ଖାଲି;
  ctx = cf ? cf->ctx : ଖାଲି;
  data = cf ? CF_DATA_CURRENT(cf) : ଖାଲି;
  ଯଦି(cf && data && ctx) {
    unsigned ଅକ୍ଷର *quic_tp = ଖାଲି;
    size_t quic_tp_len = 0;
#ifdef HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     ଆକାର(tpbuf));
    ଯଦି(tplen < 0)
      CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tplen));
    ଅନ୍ୟଥା {
      quic_tp = (unsigned ଅକ୍ଷର *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#endif
    Curl_ossl_add_session(cf, data, &ctx->tls.ossl, ctx->ssl_peer.scache_ଚାବି,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, ଖାଲି);
  }
  ଫେରନ୍ତୁ 0;
}
#endif /* USE_OPENSSL */

#ifdef USE_GNUTLS

#ifdef CURLVERBOSE
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *gtls_hs_ସନ୍ଦେଶ_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା mtype)
{
  ବାଛ(mtype) {
  ସ୍ଥିତି 1:
    ଫେରନ୍ତୁ "ClientHello";
  ସ୍ଥିତି 2:
    ଫେରନ୍ତୁ "ServerHello";
  ସ୍ଥିତି 4:
    ଫେରନ୍ତୁ "SessionTicket";
  ସ୍ଥିତି 8:
    ଫେରନ୍ତୁ "EncryptedExtensions";
  ସ୍ଥିତି 11:
    ଫେରନ୍ତୁ "Certificate";
  ସ୍ଥିତି 13:
    ଫେରନ୍ତୁ "CertificateRequest";
  ସ୍ଥିତି 15:
    ଫେରନ୍ତୁ "CertificateVerify";
  ସ୍ଥିତି 20:
    ଫେରନ୍ତୁ "Finished";
  ସ୍ଥିତି 24:
    ଫେରନ୍ତୁ "KeyUpdate";
  ସ୍ଥିତି 254:
    ଫେରନ୍ତୁ "MessageHash";
  }
  ଫେରନ୍ତୁ "Unknown";
}
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା quic_gtls_handshake_cb(gnutls_session_t session, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା htype,
                                  unsigned when, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା incoming,
                                  ଅଚଳ gnutls_datum_t *ସନ୍ଦେଶ)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  ସଂରଚନା Curl_cfilter *cf = conn_ref ? conn_ref->user_data : ଖାଲି;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ଖାଲି;

  (ଶୂନ୍ୟ)ସନ୍ଦେଶ;
  (ଶୂନ୍ୟ)incoming;
  ଯଦି(when && cf && ctx) { /* after message has been processed */
    ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    ଯଦି(!data)
      ଫେରନ୍ତୁ 0;
    CURL_TRC_CF(data, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_ସନ୍ଦେଶ_name(htype), htype);
    ବାଛ(htype) {
    ସ୍ଥିତି GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned ଅକ୍ଷର *quic_tp = ଖାଲି;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       ଆକାର(tpbuf));
      ଯଦି(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tplen));
      ଅନ୍ୟଥା {
        quic_tp = (unsigned ଅକ୍ଷର *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (ଶୂନ୍ୟ)Curl_gtls_cache_session(cf, data, ctx->ssl_peer.scache_ଚାବି,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    ଖାଲି);
      ଭାଙ୍ଗ;
    }
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }
  }
  ଫେରନ୍ତୁ 0;
}
#endif /* USE_GNUTLS */

#ifdef USE_WOLFSSL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolfSSL_get_app_data(ssl);
  ସଂରଚନା Curl_cfilter *cf = conn_ref ? conn_ref->user_data : ଖାଲି;

  DEBUGASSERT(cf);
  ଯଦି(cf && session) {
    ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
    ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    ଯଦି(data && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned ଅକ୍ଷର *quic_tp = ଖାଲି;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       ଆକାର(tpbuf));
      ଯଦି(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tplen));
      ଅନ୍ୟଥା {
        quic_tp = (unsigned ଅକ୍ଷର *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (ଶୂନ୍ୟ)Curl_wssl_cache_session(cf, data, ctx->ssl_peer.scache_ଚାବି,
                                    session, wolfSSL_version(ssl),
                                    "h3", quic_tp, quic_tp_len, ଖାଲି);
    }
  }
  ଫେରନ୍ତୁ 0;
}
#endif /* USE_WOLFSSL */

ସ୍ଥିର CURLcode cf_ngtcp2_tls_ctx_setup(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        ଶୂନ୍ୟ *user_data)
{
  ସଂରଚନା curl_tls_ctx *ctx = user_data;

#ifdef USE_OPENSSL
#ଯଦି defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  ଯଦି(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(data, "ngtcp2_crypto_boringssl_configure_client_context failed");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
#ନଚେତଯଦି defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#ଅନ୍ୟଥା
  ଯଦି(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_quictls_configure_client_context failed");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
#endif /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  ଯଦି(Curl_ssl_scache_use(cf, data)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_set_session_cache_mode(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#ନଚେତଯଦି defined(USE_GNUTLS)
  ଯଦି(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(data, "ngtcp2_crypto_gnutls_configure_client_session failed");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
  ଯଦି(Curl_ssl_scache_use(cf, data)) {
    gnutls_handshake_set_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#ନଚେତଯଦି defined(USE_WOLFSSL)
  ଯଦି(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
  ଯଦି(Curl_ssl_scache_use(cf, data)) {
    /* Register to get notified when a new session is received */
    wolfSSL_CTX_sess_set_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#endif
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_ngtcp2_on_session_reuse(ସଂରଚନା Curl_cfilter *cf,
                                           ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା alpn_spec *alpns,
                                           ସଂରଚନା Curl_ssl_session *scs,
                                           bool *do_early_data)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;
#ଯଦି defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#endif
#ifdef USE_GNUTLS
  ctx->earlydata_max =
    gnutls_record_get_max_early_data_size(ctx->tls.gtls.session);
#endif
#ifdef USE_WOLFSSL
#ifdef WOLFSSL_EARLY_DATA
  ctx->earlydata_max = scs->earlydata_max;
#ଅନ୍ୟଥା
  ctx->earlydata_max = 0;
#endif /* WOLFSSL_EARLY_DATA */
#endif
#ଯଦି defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  ଯଦି(!ctx->earlydata_max) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  ଅନ୍ୟଥା ଯଦି(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session from different ALPN, no early data");
  }
  ଅନ୍ୟଥା ଯଦି(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(data, cf, "no 0RTT transport parameters, no early data");
  }
  ଅନ୍ୟଥା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;
    rv = ngtcp2_conn_decode_and_set_0rtt_transport_params(
      ctx->qconn, (ଅଚଳ uint8_t *)scs->quic_tp, scs->quic_tp_len);
    ଯଦି(rv)
      CURL_TRC_CF(data, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    ଅନ୍ୟଥା ଯଦି(ctx->init_h3_conn_cb) {
      infof(data, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      result = ctx->init_h3_conn_cb(cf, data, ctx);
      ଯଦି(!result) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_data = TRUE;
      }
    }
    ଅନ୍ୟଥା { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_data = TRUE;
    }
  }
#ଅନ୍ୟଥା /* not supported in the TLS backend */
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)ctx;
  (ଶୂନ୍ୟ)scs;
  (ଶୂନ୍ୟ)alpns;
#endif
  ଫେରନ୍ତୁ result;
}

/*
 * Might be called twice for happy eyeballs.
 */
ସ୍ଥିର CURLcode cf_connect_start(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା cf_ngtcp2_io_ctx *pktx)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;
  CURLcode result;
  ଅଚଳ ସଂରଚନା Curl_sockaddr_ex *sockaddr = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା qfd;
  ସ୍ଥିର ଅଚଳ ସଂରଚନା alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->dcid.data, NGTCP2_MAX_CIDLEN);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->scid.data, NGTCP2_MAX_CIDLEN);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  (ଶୂନ୍ୟ)Curl_qlogdir(data, ctx->scid.data, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, data, pktx);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Query socket and remote address from sub-chain */
  ଯଦି(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, ଖାଲି)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  ଯଦି(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = ଆକାର(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (ସଂରଚନା sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    ଯଦି(rv == -1)
      ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (ସଂରଚନା sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_path.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  ଅନ୍ୟଥା {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ଅଚଳ ସଂରଚନା Curl_sockaddr_ex *remote = ଖାଲି;
    ଯଦି(cf->next->cft->query(cf->next, data, CF_QUERY_REMOTE_ADDR, ଖାଲି,
                            CURL_UNCONST(&remote)))
      ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;
    ଯଦି(!remote)
      ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, ଆକାର(ctx->q.local_addr));
    ବାଛ(remote->family) {
    ସ୍ଥିତି AF_INET:
      ((ସଂରଚନା sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = ଆକାର(ସଂରଚନା sockaddr_in);
      ଭାଙ୍ଗ;
#ifdef USE_IPV6
    ସ୍ଥିତି AF_INET6:
      ((ସଂରଚନା sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = ଆକାର(ସଂରଚନା sockaddr_in6);
      ଭାଙ୍ଗ;
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (ସଂରଚନା sockaddr *)&ctx->q.local_addr,
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
  ଯଦି(rc)
    ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_data = cf;

  result = Curl_vquic_tls_init(&ctx->tls, cf, data,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_setup, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

#ଯଦି defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  ଯଦି(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_ctx_new failed");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  ଯଦି(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_configure_client_session failed");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
#ନଚେତଯଦି defined(USE_OPENSSL)
  SSL_set_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#ନଚେତଯଦି defined(USE_GNUTLS)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#ନଚେତଯଦି defined(USE_WOLFSSL)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#ଅନ୍ୟଥା
#error "ngtcp2 TLS backend not defined"
#endif

#ଯଦି defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  ଯଦି(ctx->use_earlydata &&
     !SSL_set_quic_tls_early_data_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(data, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#endif
  ngtcp2_ccerr_default(&ctx->last_error);

  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    bool *done)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ସଂରଚନା cf_call_data save;
  ସଂରଚନା cf_ngtcp2_io_ctx pktx;

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  /* Connect the sub-chain */
  ଯଦି(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ଯଦି(result || !*done)
      ଫେରନ୍ତୁ result;
  }

  *done = FALSE;

  ଯଦି(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    ଫେରନ୍ତୁ CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  CF_DATA_SAVE(save, cf, data);

  ଯଦି(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(data);
    result = cf_connect_start(cf, data, &pktx);
    ଯଦି(result)
      ଯାଅ out;
    ଯଦି(cf->connected) {
      *done = TRUE;
      ଯାଅ out;
    }
    result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
    /* we do not expect to be able to recv anything yet */
    ଯାଅ out;
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  ଯଦି(result)
    ଯାଅ out;

  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
  ଯଦି(result)
    ଯାଅ out;

  ଯଦି(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    result = ctx->tls_vrfy_result;
    ଯଦି(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      cf->connected = TRUE;
      *done = TRUE;
    }
  }

out:
  ଯଦି(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
  ଯଦି(ctx->qconn &&
     ((result == CURLE_RECV_ERROR) || (result == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_in_draining_period(ctx->qconn)) {
    ଅଚଳ ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    result = CURLE_COULDNT_CONNECT;
    ଯଦି(cerr) {
      CURL_TRC_CF(data, cf, "connect error, type=%d, code=%" PRIu64,
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cerr->type, cerr->error_code);
      ବାଛ(cerr->type) {
      ସ୍ଥିତି NGTCP2_CCERR_TYPE_VERSION_NEGOTIATION:
        CURL_TRC_CF(data, cf, "error in version negotiation");
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଯଦି(cerr->error_code >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(data, cf, "crypto error, tls alert=%u",
                      (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(cerr->error_code & 0xffU));
        }
        ଅନ୍ୟଥା ଯଦି(cerr->error_code == NGTCP2_CONNECTION_REFUSED) {
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
  ଯଦି(result) {
    ଯଦି(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      ସଂରଚନା ip_quadruple ip;

      ଯଦି(!Curl_cf_socket_peek(cf->next, data, ଖାଲି, ଖାଲି, &ip))
        infof(data, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    }
  }
#endif
  ଯଦି(!result && ctx->qconn) {
    result = Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx);
  }
  ଯଦି(result || *done)
    CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *done);
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(ସଂରଚନା Curl_cfilter *cf,
                                     ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ସଂରଚନା cf_call_data save;
  ସଂରଚନା cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  ଯଦି(cf->shutdown || !ctx->qconn) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ଯଦି(!cf->next) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);
  *done = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  ଯଦି(!ctx->shutdown_started) {
    ଅକ୍ଷର buffer[NGTCP2_MAX_UDP_PAYLOAD_SIZE];
    ngtcp2_ssize nwritten;

    ଯଦି(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
      CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
      result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
      ଯଦି(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
        CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
        result = CURLE_OK;
        ଯାଅ out;
      }
      ଅନ୍ୟଥା ଯଦି(result) {
        CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        *done = TRUE;
        ଯାଅ out;
      }
    }

    DEBUGASSERT(Curl_bufq_is_empty(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, ଖାଲି, /* path */
      ଖାଲି, /* pkt_info */
      (uint8_t *)buffer, ଆକାର(buffer),
      &ctx->last_error, pktx.ts);
    CURL_TRC_CF(data, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ctx->last_error.type,
                ctx->last_error.error_code, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    ଯଦି(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      result = Curl_bufq_write(&ctx->q.sendbuf, (ଅଚଳ unsigned ଅକ୍ଷର *)buffer,
                               (size_t)nwritten, &n);
      ଯଦି(result) {
        CURL_TRC_CF(data, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        ଯାଅ out;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.split_len = 0;
    }
  }

  ଯଦି(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing egress");
    result = Curl_vquic_flush(cf, data, &ctx->q);
    ଯଦି(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
      result = CURLE_OK;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(result) {
      CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      *done = TRUE;
      ଯାଅ out;
    }
  }

  ଯଦି(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
    result = CURLE_OK;
  }
out:
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_cf_ngtcp2_cmn_conn_close(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  bool done;
  Curl_cf_ngtcp2_cmn_shutdown(cf, data, &done);
}

ସ୍ଥିର bool cf_ngtcp2_err_is_fatal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
  ଫେରନ୍ତୁ (NGTCP2_ERR_FATAL >= code) ||
         (NGTCP2_ERR_DROP_CONN == code) ||
         (NGTCP2_ERR_IDLE_CLOSE == code);
}

ଶୂନ୍ୟ Curl_cf_ngtcp2_cmn_err_set(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ଯଦି(!ctx->last_error.error_code) {
    ଯଦି(NGTCP2_ERR_CRYPTO == code) {
      ngtcp2_ccerr_set_tls_alert(&ctx->last_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 ଖାଲି, 0);
    }
    ଅନ୍ୟଥା {
      ngtcp2_ccerr_set_liberr(&ctx->last_error, code, ଖାଲି, 0);
    }
  }
  ଯଦି(cf_ngtcp2_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

ଶୂନ୍ୟ Curl_cf_ngtcp2_io_ctx_init(ସଂରଚନା cf_ngtcp2_io_ctx *io_ctx,
                                ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->data = data;
  io_ctx->now = *Curl_pgrs_now(data);
  ngtcp2_path_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_set_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

ଶୂନ୍ୟ Curl_cf_ngtcp2_io_ctx_update_time(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା cf_ngtcp2_io_ctx *io_ctx,
                                       ସଂରଚନା Curl_cfilter *cf)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(data);
  Curl_vquic_ctx_update_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#ଯଦି NGTCP2_VERSION_ଅଙ୍କ < 0x011100
ସଂରଚନା cf_ngtcp2_sfind_ctx {
  int64_t stream_id;
  ସଂରଚନା h3_stream_ctx *stream;
  uint32_t mid;
};

ସ୍ଥିର bool cf_ngtcp2_sfind(uint32_t mid, ଶୂନ୍ୟ *ମୂଲ୍ୟ, ଶୂନ୍ୟ *user_data)
{
  ସଂରଚନା cf_ngtcp2_sfind_ctx *fctx = user_data;
  ସଂରଚନା h3_stream_ctx *stream = ମୂଲ୍ୟ;

  ଯଦି(fctx->stream_id == stream->id) {
    fctx->mid = mid;
    fctx->stream = stream;
    ଫେରନ୍ତୁ FALSE;
  }
  ଫେରନ୍ତୁ TRUE; /* continue */
}

ସ୍ଥିର ସଂରଚନା h3_stream_ctx *cf_ngtcp2_get_stream(ସଂରଚନା cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  ସଂରଚନା cf_ngtcp2_sfind_ctx fctx;
  fctx.stream_id = stream_id;
  fctx.stream = ଖାଲି;
  Curl_u32_ptrset_visit(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  ଫେରନ୍ତୁ fctx.stream;
}
#ଅନ୍ୟଥା
ସ୍ଥିର ସଂରଚନା h3_stream_ctx *cf_ngtcp2_get_stream(ସଂରଚନା cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  ଫେରନ୍ତୁ ngtcp2_conn_get_stream_user_data(ctx->qconn, stream_id);
}
#endif

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
ସ୍ଥିର CURLcode read_pkt_to_send(ଶୂନ୍ୟ *userp,
                                 unsigned ଅକ୍ଷର *buf, size_t buflen,
                                 size_t *pnread)
{
  ସଂରଚନା cf_ngtcp2_io_ctx *x = userp;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t stream_id;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fin;
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
  ପାଇଁ(;;) {

    ଯଦି(ctx->h3conn && ngtcp2_conn_get_max_data_ବାମ(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_stream(ctx->h3conn, &stream_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      ଯଦି(veccnt < 0) {
        failf(x->data, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)veccnt));
        Curl_cf_ngtcp2_h3_err_set(x->cf, x->data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)veccnt);
        ଫେରନ୍ତୁ CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_STREAM_FLAG_MORE |
            (fin ? NGTCP2_WRITE_STREAM_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_stream(ctx->qconn, &x->ps.path,
                                  ଖାଲି, buf, buflen,
                                  &ndatalen, flags, stream_id,
                                  (ଅଚଳ ngtcp2_vec *)vec, veccnt, x->ts);
    ଯଦି(n == 0) {
      /* nothing to send */
      ଫେରନ୍ତୁ CURLE_AGAIN;
    }
    ଅନ୍ୟଥା ଯଦି(n < 0) {
      ବାଛ(n) {
      ସ୍ଥିତି NGTCP2_ERR_STREAM_DATA_BLOCKED: {
        ସଂରଚନା h3_stream_ctx *stream;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_block_stream(ctx->h3conn, stream_id);
        CURL_TRC_CF(x->data, x->cf, "[%" PRId64 "] block quic flow",
                    stream_id);
        stream = cf_ngtcp2_get_stream(ctx, stream_id);
        ଯଦି(stream) /* it might be not one of our h3 streams? */
          stream->quic_flow_blocked = TRUE;
        n = 0;
        ଭାଙ୍ଗ;
      }
      ସ୍ଥିତି NGTCP2_ERR_STREAM_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_stream_write(ctx->h3conn, stream_id);
        n = 0;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି NGTCP2_ERR_WRITE_MORE:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        DEBUGASSERT(ndatalen == -1);
        failf(x->data, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n));
        Curl_cf_ngtcp2_cmn_err_set(x->cf, x->data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n);
        ଫେରନ୍ତୁ CURLE_SEND_ERROR;
      }
    }

    ଯଦି(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv = nghttp3_conn_add_write_offset(ctx->h3conn, stream_id, ndatalen);
      ଯଦି(rv) {
        failf(x->data, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        ଫେରନ୍ତୁ CURLE_SEND_ERROR;
      }
    }

    ଯଦି(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      ଫେରନ୍ତୁ CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_progress_egress(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା cf_ngtcp2_io_ctx *pktx)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_size, path_max_payload_size;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_quantum;
  CURLcode result;
  ସଂରଚନା cf_ngtcp2_io_ctx local_pktx;

  ଯଦି(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  ଅନ୍ୟଥା {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  ଯଦି(result) {
    ଯଦି(result == CURLE_AGAIN) {
      Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଫେରନ୍ତୁ result;
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
  ପାଇଁ(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_size,
                            read_pkt_to_send, pktx, &nread);
    ଯଦି(result == CURLE_AGAIN)
      ଭାଙ୍ଗ;
    ଅନ୍ୟଥା ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ଅନ୍ୟଥା {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      ଯଦି((buflen >= send_quantum) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_size))
        ଭାଙ୍ଗ;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      ଯଦି(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      ଅନ୍ୟଥା ଯଦି(nread > gsolen ||
              (gsolen > path_max_payload_size && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        result = Curl_vquic_send_tail_split(cf, data, &ctx->q,
                                            gsolen, nread, nread);
        ଯଦି(result) {
          ଯଦି(result == CURLE_AGAIN) {
            Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
            ଫେରନ୍ତୁ CURLE_OK;
          }
          ଫେରନ୍ତୁ result;
        }
        pktcnt = 0;
      }
      ଅନ୍ୟଥା ଯଦି(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        ଭାଙ୍ଗ;
      }
    }
  }

  ଯଦି(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(data, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
    ଯଦି(result) {
      ଯଦି(result == CURLE_AGAIN) {
        Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
        ଫେରନ୍ତୁ CURLE_OK;
      }
      ଫେରନ୍ତୁ result;
    }
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_conn_update_pkt_tx_time(ctx->qconn, pktx->ts);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସଂରଚନା cf_ngtcp2_recv_ctx {
  ସଂରଚନା cf_ngtcp2_io_ctx *pktx;
  size_t pkt_ଗଣନା;
};

ସ୍ଥିର CURLcode cf_ngtcp2_recv_pkts(ଅଚଳ unsigned ଅକ୍ଷର *buf, size_t buflen,
                                    size_t gso_size,
                                    ସଂରଚନା sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା cf_ngtcp2_recv_ctx *rctx = userp;
  ସଂରଚନା cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_path path;
  size_t offset, pktlen;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;

  ଯଦି(!rctx->pkt_ଗଣନା) {
    Curl_cf_ngtcp2_io_ctx_update_time(pktx->data, pktx, pktx->cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  ଯଦି(ecn)
    CURL_TRC_CF(pktx->data, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_size, (unsigned)ecn);
  ngtcp2_addr_init(&path.local, (ସଂରଚନା sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&path.remote, (ସଂରଚନା sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  ପାଇଁ(offset = 0; offset < buflen; offset += gso_size) {
    rctx->pkt_ଗଣନା++;
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &path, &pi,
                              buf + offset, pktlen, pktx->ts);
    ଯଦି(rv) {
      CURL_TRC_CF(pktx->data, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_set(pktx->cf, pktx->data, rv);

      ଯଦି(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_progress_ingress(ସଂରଚନା Curl_cfilter *cf,
                                         ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା cf_ngtcp2_io_ctx *pktx)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ସଂରଚନା cf_ngtcp2_io_ctx local_pktx;
  ସଂରଚନା cf_ngtcp2_recv_ctx rctx;
  CURLcode result = CURLE_OK;

  ଯଦି(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }

  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  rctx.pktx = pktx;
  rctx.pkt_ଗଣନା = 0;

  ଯଦି(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(data, cf, "progress_ingress(socket)");
    ଫେରନ୍ତୁ Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  ଅନ୍ୟଥା {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    unsigned ଅକ୍ଷର *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_SIZE;
    size_t pkt_limit = QUIC_TUNNEL_INGRESS_PKT_LIMIT;
    size_t nread;
    ସଂରଚନା sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(data, cf, "progress_ingress(sub-filters)");
    ଯଦି(ctx->qconn) {
      size_t max_path_payload;
      max_path_payload =
        ngtcp2_conn_get_path_max_tx_udp_payload_size(ctx->qconn);
      ଯଦି(max_path_payload > max_udp_payload)
        max_udp_payload = max_path_payload;
    }

    ଯଦି(ctx->tunnel_inbuf_len < max_udp_payload) {
      unsigned ଅକ୍ଷର *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      ଯଦି(!newbuf)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    ଯେତେବେଳେ(pkt_limit--) {
      result = Curl_conn_cf_recv(cf->next, data, (ଅକ୍ଷର *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      ଯଦି(result == CURLE_AGAIN) {
        /* no more data available at the moment */
        ଫେରନ୍ତୁ CURLE_OK;
      }
      ଯଦି(result) {
        CURL_TRC_CF(data, cf, "ingress, recv from tunnel failed: %d",
                    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
        ଫେରନ୍ତୁ result;
      }
      ଯଦି(nread == 0) {
        /* tunnel closed */
        ଫେରନ୍ତୁ CURLE_OK;
      }

      memcpy(&remote_addr, ctx->connected_path.remote.addr,
             ctx->connected_path.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_path.remote.addrlen;
      result = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      ଯଦି(!ctx->q.got_first_byte) {
        ctx->q.got_first_byte = TRUE;
        ctx->q.first_byte_at = ctx->q.last_op;
      }
      ctx->q.last_io = ctx->q.last_op;
    }
    ଫେରନ୍ତୁ CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_set_expiry(ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା cf_ngtcp2_io_ctx *pktx)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ସଂରଚନା cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  ଯଦି(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  ଅନ୍ୟଥା {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  ଯଦି(expiry != UINT64_MAX) {
    ଯଦି(expiry <= pktx->ts) {
      CURLcode result;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      ଯଦି(rv) {
        failf(data, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_set(cf, data, rv);
        ଫେରନ୍ତୁ CURLE_SEND_ERROR;
      }
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, pktx);
      ଯଦି(!result)
        result = Curl_cf_ngtcp2_progress_egress(cf, data, pktx);
      ଯଦି(result) {
        /* a verify failure during ingress must win over generic errors */
        ଯଦି(ctx->tls_vrfy_result)
          result = ctx->tls_vrfy_result;
        ଫେରନ୍ତୁ result;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    ଯଦି(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      ଯଦି(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_set(data, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_ngtcp2_setup_keep_alive(ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ଅଚଳ ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  ଯଦି(!ctx->qconn)
    ଫେରନ୍ତୁ;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  ଯଦି(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no peer idle timeout, unset keep-alive");
  }
  ଅନ୍ୟଥା ଯଦି(!Curl_u32_ptrset_ଗଣନା(&ctx->streams)) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no active streams, unset keep-alive");
  }
  ଅନ୍ୟଥା {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(data, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_stream_setup(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା h3_stream_ctx **pstream)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ସଂରଚନା h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  *pstream = ଖାଲି;
  ଯଦି(!data)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(stream) {
    *pstream = stream;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  stream = curlx_calloc(1, ଆକାର(*stream));
  ଯଦି(!stream)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

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
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_ଧାଡ଼ି_LEN);

  ଯଦି(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    Curl_cf_ngtcp2_h3_stream_ctx_free(stream);
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  ଯଦି(Curl_u32_ptrset_ଗଣନା(&ctx->streams) == 1)
    cf_ngtcp2_setup_keep_alive(cf, data);

  *pstream = stream;
  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_cf_ngtcp2_h3_stream_close(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା h3_stream_ctx *stream)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(data);
  DEBUGASSERT(stream);
  ଯଦି(!stream->closed && ctx->qconn && ctx->h3conn) {
    CURLcode result;

    nghttp3_conn_set_stream_user_data(ctx->h3conn, stream->id, ଖାଲି);
    ngtcp2_conn_set_stream_user_data(ctx->qconn, stream->id, ଖାଲି);
    stream->closed = TRUE;
    (ଶୂନ୍ୟ)ngtcp2_conn_shutdown_stream(ctx->qconn, 0, stream->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    result = Curl_cf_ngtcp2_progress_egress(cf, data, ଖାଲି);
    ଯଦି(result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cancel stream -> %d",
                  stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  }
}

ଶୂନ୍ୟ Curl_cf_ngtcp2_h3_stream_done(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  ସଂରଚନା h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  (ଶୂନ୍ୟ)cf;
  ଯଦି(stream) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] easy handle is done", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
    ଯଦି(!Curl_u32_ptrset_ଗଣନା(&ctx->streams))
      cf_ngtcp2_setup_keep_alive(cf, data);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      bool *ନିବେଶ_pending)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ଅଚଳ ngtcp2_transport_params *rp;
  ସଂରଚନା cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  *ନିବେଶ_pending = FALSE;
  ଯଦି(!ctx->qconn || ctx->shutdown_started)
    ଯାଅ out;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  ଯଦି(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->q.last_io);
    ଯଦି(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      ଯଦି((uint64_t)idletime_ms > max_idle_ms)
        ଯାଅ out;
    }
  }

  ଯଦି(!cf->next || !cf->next->cft->is_alive(cf->next, data, ନିବେଶ_pending))
    ଯାଅ out;

  alive = TRUE;
  ଯଦି(*ନିବେଶ_pending) {
    CURLcode result;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *ନିବେଶ_pending = FALSE;
    result = Curl_cf_ngtcp2_progress_ingress(cf, data, ଖାଲି);
    CURL_TRC_CF(data, cf, "is_alive, progress ingress -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    alive = result ? FALSE : TRUE;
  }

out:
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(ସଂରଚନା cf_ngtcp2_ctx *ctx,
                                      ସଂରଚନା Curl_easy *data)
{
  int64_t ctrl_stream_id, qpack_enc_stream_id, qpack_dec_stream_id;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &ctrl_stream_id, ଖାଲି);
  ଯଦି(rc) {
    failf(data, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_stream(ctx->h3conn, ctrl_stream_id);
  ଯଦି(rc) {
    failf(data, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_enc_stream_id, ଖାଲି);
  ଯଦି(rc) {
    failf(data, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_dec_stream_id, ଖାଲି);
  ଯଦି(rc) {
    failf(data, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_stream_id,
                                       qpack_dec_stream_id);
  ଯଦି(rc) {
    failf(data, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;

  ବାଛ(query) {
  ସ୍ଥିତି CF_QUERY_CONNECT_REPLY_MS:
    ଯଦି((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ms : INT_MAX;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_QUERY_REALLY_CONNECTED:
    ଯଦି(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_first_byte;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2)
{
  ସଂରଚନା cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)arg1;
  (ଶୂନ୍ୟ)arg2;
  ବାଛ(event) {
  ସ୍ଥିତି CF_CTRL_REPORT_STATS:
    ଯଦି(cf->connected && !ctx->stats_reported) {
      ଯଦି((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        ctx->stats_reported = TRUE;
      }
      ଅନ୍ୟଥା ଯଦି(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        ଯଦି(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
