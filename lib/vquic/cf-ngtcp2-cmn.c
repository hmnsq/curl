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
#अन्तर्भाव "curl_setup.h"

#सचे !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#अन्तर्भाव <ngtcp2/ngtcp2.h>

#ifdef USE_OPENSSL
#अन्तर्भाव <openssl/err.h>
#सचे defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_boringssl.h>
#नोचेद्यदि defined(OPENSSL_QUIC_API2)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_ossl.h>
#अञ्ञथा
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#अन्तर्भाव "vtls/openssl.h"
#नोचेद्यदि defined(USE_GNUTLS)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_gnutls.h>
#अन्तर्भाव "vtls/gtls.h"
#नोचेद्यदि defined(USE_WOLFSSL)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_wolfssl.h>
#अन्तर्भाव "vtls/wolfssl.h"
#endif

#अन्तर्भाव <nghttp3/nghttp3.h>

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "u32_ptrset.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-socket.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "curlx/win32-fopen.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "http1.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "sockaddr.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "vquic/vquic_int.h"
#अन्तर्भाव "vquic/vquic-tls.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "vquic/cf-ngtcp2-cmn.h"

/*
 * Store ngtcp2 version info in this buffer.
 */
शून्य Curl_ngtcp2_ver(वर्ण *p, size_t len)
{
  ध्रुव ngtcp2_info *ng2 = ngtcp2_version(0);
  ध्रुव nghttp3_info *ht3 = nghttp3_version(0);
  (शून्य)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->version_str, ht3->version_str);
}

शून्य Curl_cf_ngtcp2_h3_stream_ctx_free(संरचना h3_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

स्थिर शून्य h3_stream_set_free(uint32_t id, शून्य *stream)
{
  (शून्य)id;
  DEBUGASSERT(stream);
  Curl_cf_ngtcp2_h3_stream_ctx_free((संरचना h3_stream_ctx *)stream);
}

स्थिर bool cf_ngtcp2_h3_err_is_fatal(पूर्णाङ्क code)
{
  पटिनिवत्त (NGHTTP3_ERR_FATAL >= code) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM == code);
}

शून्य Curl_cf_ngtcp2_h3_err_set(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data, पूर्णाङ्क code)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  सचे(!ctx->last_error.error_code) {
    ngtcp2_ccerr_set_application_error(&ctx->last_error,
      nghttp3_err_infer_quic_app_error_code(code), रिक्त, 0);
  }
  सचे(cf_ngtcp2_h3_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

CURLcode Curl_cf_ngtcp2_ctx_init(संरचना cf_ngtcp2_ctx *ctx,
                                 संरचना Curl_peer *origin,
                                 संरचना Curl_peer *peer,
                                 संरचना ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = रिक्त;
  ctx->tunnel_inbuf_len = 0;
  ctx->version = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->stream_bufcp, H3_STREAM_CHUNK_SIZE,
                  H3_STREAM_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  पटिनिवत्त Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

शून्य Curl_cf_ngtcp2_ctx_cleanup(संरचना cf_ngtcp2_ctx *ctx)
{
  सचे(ctx && ctx->initialized) {
    सचे(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    सचे(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = रिक्त;
    }
    सचे(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = रिक्त;
    }
#ifdef OPENSSL_QUIC_API2
    सचे(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = रिक्त;
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

संरचना Curl_easy *Curl_cf_ngtcp2_get_xfer(संरचना Curl_cfilter *cf,
                                          शून्य *stream_user_data,
                                          संरचना h3_stream_ctx **pstream)
{
  संरचना h3_stream_ctx *stream = stream_user_data;

  सचे(pstream)
    *pstream = stream;
  सचे(stream && (stream->mid != UINT32_MAX)) {
    संरचना Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    सचे(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      पटिनिवत्त रिक्त;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    सचे(data && (data->id == stream->xfer_id))
      पटिनिवत्त data;
    DEBUGASSERT(0);
  }
  पटिनिवत्त रिक्त;
}

स्थिर ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  संरचना Curl_cfilter *cf = conn_ref->user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  पटिनिवत्त ctx->qconn;
}

#ifdef DEBUG_NGTCP2
स्थिर शून्य quic_printf(शून्य *user_data, ध्रुव वर्ण *fmt, ...)
{
  va_list ap;
  (शून्य)user_data;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#endif

स्थिर शून्य qlog_callback(शून्य *user_data, uint32_t flags,
                          ध्रुव शून्य *data, size_t datalen)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  (शून्य)flags;
  सचे(ctx->qlogfd != -1) {
    ssize_t rc = लिख(ctx->qlogfd, data, datalen);
    सचे(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

स्थिर शून्य quic_settings(संरचना cf_ngtcp2_ctx *ctx,
                          संरचना Curl_easy *data,
                          संरचना cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#ifdef DEBUG_NGTCP2
  s->log_printf = quic_printf;
#अञ्ञथा
  s->log_printf = रिक्त;
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
  सचे(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#सचे defined(_MSC_VER) && defined(_DLL)
#pragma warning(प्रक्षिप)
#pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

स्थिर पूर्णाङ्क cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, शून्य *user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf ? cf->ctx : रिक्त;
  संरचना Curl_easy *data;

  (शून्य)tconn;
  DEBUGASSERT(ctx);
  data = CF_DATA_CURRENT(cf);
  DEBUGASSERT(data);
  सचे(!ctx || !data)
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(data);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, data);

  ctx->tls_vrfy_result = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    data, &ctx->ssl_peer);
  सचे(ctx->tls_vrfy_result)
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;

#ifdef CURLVERBOSE
  सचे(Curl_trc_is_verbose(data)) {
    ध्रुव ngtcp2_transport_params *rp;
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
  सचे(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  सचे(ctx->use_earlydata) {
#सचे defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    सचे(SSL_get_early_data_status(ctx->tls.ossl.ssl) == SSL_EARLY_DATA_NOT_SENT)
      CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
#endif

#सचे NGTCP2_VERSION_अङ्कः >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_data_rejected2(ctx->qconn);
#अञ्ञथा /* older NGTCP2 */
    {
#सचे defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      पूर्णाङ्क ossl_early_status = SSL_get_early_data_status(ctx->tls.ossl.ssl);
      सचे(ossl_early_status == SSL_EARLY_DATA_NOT_SENT)
        CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_status == SSL_EARLY_DATA_ACCEPTED);
#नोचेद्यदि defined(USE_GNUTLS)
      पूर्णाङ्क flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_DATA);
#नोचेद्यदि defined(USE_WOLFSSL)
#ifdef WOLFSSL_EARLY_DATA
      ctx->earlydata_accepted =
        (wolfSSL_get_early_data_status(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_DATA_REJECTED);
#अञ्ञथा
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
  पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क cb_recv_stream_data(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t stream_id, uint64_t offset,
                               ध्रुव uint8_t *buf, size_t buflen,
                               शून्य *user_data, शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  पूर्णाङ्क fin = (flags & NGTCP2_STREAM_DATA_FLAG_FIN) ? 1 : 0;
  संरचना h3_stream_ctx *stream;
  संरचना Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (शून्य)offset;
  rc = nghttp3_conn_read_stream(ctx->h3conn, stream_id, buf, buflen, fin);
  सचे(rc < 0) {
    सचे(data && stream) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  stream_id, stream->reset, stream->closed);
    }
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  सचे(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_stream_offset(tconn, stream_id, nconsumed);
    ngtcp2_conn_extend_max_offset(tconn, nconsumed);
    सचे(stream) {
      stream->rx_offset += nconsumed;
      stream->rx_offset_max += nconsumed;
    }
  }
  पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क cb_acked_stream_data_offset(ngtcp2_conn *tconn, int64_t stream_id,
                                       uint64_t offset, uint64_t datalen,
                                       शून्य *user_data, शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णाङ्क rv;
  (शून्य)stream_id;
  (शून्य)tconn;
  (शून्य)offset;
  (शून्य)datalen;
  (शून्य)stream_user_data;

  rv = nghttp3_conn_add_ack_offset(ctx->h3conn, stream_id, datalen);
  सचे(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;
  }

  पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क cb_stream_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id, uint64_t app_error_code,
                           शून्य *user_data, शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, रिक्त);
  पूर्णाङ्क rv;

  (शून्य)tconn;
  /* stream is closed... */
  सचे(!data)
    data = CF_DATA_CURRENT(cf);
  सचे(!data)
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;

  सचे(!(flags & NGTCP2_STREAM_CLOSE_FLAG_APP_ERROR_CODE_SET)) {
    app_error_code = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, app_error_code, rv);
  सचे(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;
  }

  पटिनिवत्त 0;
}

#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
स्थिर पूर्णाङ्क cb_stream_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id,
                           uint64_t rx_app_error_code,
                           uint64_t tx_app_error_code,
                           शून्य *user_data, शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, रिक्त);
  uint64_t h3_app_error_code = NGHTTP3_H3_NO_ERROR;
  पूर्णाङ्क rv;

  (शून्य)tconn;
  (शून्य)tx_app_error_code;
  /* stream is closed... */
  सचे(!data)
    data = CF_DATA_CURRENT(cf);
  सचे(!data)
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;

  सचे(flags & NGTCP2_STREAM_CLOSE2_FLAG_RX_APP_ERROR_CODE_SET)
    h3_app_error_code = rx_app_error_code;

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, h3_app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, h3_app_error_code, rv);
  सचे(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;
  }

  पटिनिवत्त 0;
}
#endif

स्थिर पूर्णाङ्क cb_stream_reset(ngtcp2_conn *tconn, int64_t stream_id,
                           uint64_t final_size, uint64_t app_error_code,
                           शून्य *user_data, शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, रिक्त);
  पूर्णाङ्क rv;
  (शून्य)tconn;
  (शून्य)final_size;
  (शून्य)app_error_code;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  सचे(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  सचे(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;
  }

  पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क cb_stream_stop_sending(ngtcp2_conn *tconn, int64_t stream_id,
                                  uint64_t app_error_code, शून्य *user_data,
                                  शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णाङ्क rv;
  (शून्य)tconn;
  (शून्य)app_error_code;
  (शून्य)stream_user_data;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  सचे(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;
  }

  पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            शून्य *user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);

  (शून्य)tconn;
  ctx->max_bidi_streams = max_streams;
  सचे(data)
    CURL_TRC_CF(data, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->used_bidi_streams);
  पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क cb_extend_max_stream_data(ngtcp2_conn *tconn, int64_t stream_id,
                                     uint64_t max_data, शून्य *user_data,
                                     शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना h3_stream_ctx *stream;
  संरचना Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  पूर्णाङ्क rv;
  (शून्य)tconn;
  (शून्य)max_data;

  rv = nghttp3_conn_unblock_stream(ctx->h3conn, stream_id);
  सचे(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;
  }
  सचे(stream && stream->quic_flow_blocked) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] unblock quic flow", stream_id);
    stream->quic_flow_blocked = FALSE;
    सचे(data)
      Curl_multi_mark_dirty(data);
  }
  पटिनिवत्त 0;
}

स्थिर शून्य cb_rand(uint8_t *dest, size_t destlen,
                    ध्रुव ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode result;
  (शून्य)rand_ctx;

  result = Curl_rand(रिक्त, dest, destlen);
  सचे(result) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
स्थिर पूर्णाङ्क cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    शून्य *user_data)
{
  CURLcode result;
  (शून्य)tconn;
  (शून्य)user_data;

  result = Curl_rand(रिक्त, cid->data, cidlen);
  सचे(result)
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(रिक्त, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  सचे(result)
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;

  पटिनिवत्त 0;
}

#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
स्थिर पूर्णाङ्क cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  संरचना ngtcp2_stateless_reset_token *token, size_t cidlen, शून्य *user_data)
{
  CURLcode result;
  (शून्य)tconn;
  (शून्य)user_data;

  result = Curl_rand(रिक्त, cid->data, cidlen);
  सचे(result)
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(रिक्त, token->data, परिमाण(token->data));
  सचे(result)
    पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;

  पटिनिवत्त 0;
}
#endif

स्थिर पूर्णाङ्क cb_recv_rx_कुञ्चिका(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          शून्य *user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf ? cf->ctx : रिक्त;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  (शून्य)tconn;

  सचे(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    पटिनिवत्त 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(data);
  सचे(ctx && data && !ctx->h3conn && ctx->init_h3_conn_cb) {
    सचे(ctx->init_h3_conn_cb(cf, data, ctx))
      पटिनिवत्त NGTCP2_ERR_CALLBACK_FAILURE;
  }
  पटिनिवत्त 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic प्रक्षिप
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
स्थिर ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  रिक्त, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_data_cb,
  cb_ngtcp2_handshake_completed,
  रिक्त, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_mask_cb,
  cb_recv_stream_data,
  cb_acked_stream_data_offset,
  रिक्त, /* stream_open */
  cb_stream_close,
  रिक्त, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  रिक्त, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  रिक्त, /* remove_connection_id */
  ngtcp2_crypto_update_कुञ्चिका_cb, /* update_key */
  रिक्त, /* path_validation */
  रिक्त, /* select_preferred_addr */
  cb_stream_reset,
  रिक्त, /* extend_max_remote_streams_bidi */
  रिक्त, /* extend_max_remote_streams_uni */
  cb_extend_max_stream_data,
  रिक्त, /* dcid_status */
  रिक्त, /* handshake_confirmed */
  रिक्त, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  रिक्त, /* recv_datagram */
  रिक्त, /* ack_datagram */
  रिक्त, /* lost_datagram */
  ngtcp2_crypto_get_path_challenge_data_cb,
  cb_stream_stop_sending,
  रिक्त, /* version_negotiation */
  cb_recv_rx_कुञ्चिका,
  रिक्त, /* recv_tx_key */
  रिक्त, /* early_data_rejected */
#ifdef NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  रिक्त, /* begin_path_validation */
#endif
#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  रिक्त, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  रिक्त, /* dcid_status2 */
  ngtcp2_crypto_get_path_challenge_data2_cb, /* get_path_challenge_data2 */
#endif
#ifdef NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  रिक्त, /* recv_stop_sending */
#endif
#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_stream_close2, /* is called instead of cb_stream_close when set */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic निष्कासय
#endif

#सचे defined(_MSC_VER) && defined(_DLL)
#pragma warning(निष्कासय)
#endif

#ifdef USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
स्थिर पूर्णाङ्क quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  संरचना Curl_cfilter *cf;
  संरचना cf_ngtcp2_ctx *ctx;
  संरचना Curl_easy *data;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_data(ssl);
  cf = cref ? cref->user_data : रिक्त;
  ctx = cf ? cf->ctx : रिक्त;
  data = cf ? CF_DATA_CURRENT(cf) : रिक्त;
  सचे(cf && data && ctx) {
    unsigned वर्ण *quic_tp = रिक्त;
    size_t quic_tp_len = 0;
#ifdef HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     परिमाण(tpbuf));
    सचे(tplen < 0)
      CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((पूर्णाङ्क)tplen));
    अञ्ञथा {
      quic_tp = (unsigned वर्ण *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#endif
    Curl_ossl_add_session(cf, data, &ctx->tls.ossl, ctx->ssl_peer.scache_कुञ्चिका,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, रिक्त);
  }
  पटिनिवत्त 0;
}
#endif /* USE_OPENSSL */

#ifdef USE_GNUTLS

#ifdef CURLVERBOSE
स्थिर ध्रुव वर्ण *gtls_hs_सन्देशः_name(पूर्णाङ्क mtype)
{
  विकल्प(mtype) {
  पक्ष 1:
    पटिनिवत्त "ClientHello";
  पक्ष 2:
    पटिनिवत्त "ServerHello";
  पक्ष 4:
    पटिनिवत्त "SessionTicket";
  पक्ष 8:
    पटिनिवत्त "EncryptedExtensions";
  पक्ष 11:
    पटिनिवत्त "Certificate";
  पक्ष 13:
    पटिनिवत्त "CertificateRequest";
  पक्ष 15:
    पटिनिवत्त "CertificateVerify";
  पक्ष 20:
    पटिनिवत्त "Finished";
  पक्ष 24:
    पटिनिवत्त "KeyUpdate";
  पक्ष 254:
    पटिनिवत्त "MessageHash";
  }
  पटिनिवत्त "Unknown";
}
#endif

स्थिर पूर्णाङ्क quic_gtls_handshake_cb(gnutls_session_t session, unsigned पूर्णाङ्क htype,
                                  unsigned when, unsigned पूर्णाङ्क incoming,
                                  ध्रुव gnutls_datum_t *सन्देशः)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  संरचना Curl_cfilter *cf = conn_ref ? conn_ref->user_data : रिक्त;
  संरचना cf_ngtcp2_ctx *ctx = cf ? cf->ctx : रिक्त;

  (शून्य)सन्देशः;
  (शून्य)incoming;
  सचे(when && cf && ctx) { /* after message has been processed */
    संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    सचे(!data)
      पटिनिवत्त 0;
    CURL_TRC_CF(data, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_सन्देशः_name(htype), htype);
    विकल्प(htype) {
    पक्ष GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned वर्ण *quic_tp = रिक्त;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       परिमाण(tpbuf));
      सचे(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((पूर्णाङ्क)tplen));
      अञ्ञथा {
        quic_tp = (unsigned वर्ण *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (शून्य)Curl_gtls_cache_session(cf, data, ctx->ssl_peer.scache_कुञ्चिका,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    रिक्त);
      भङ्ग;
    }
    स्वभाव:
      भङ्ग;
    }
  }
  पटिनिवत्त 0;
}
#endif /* USE_GNUTLS */

#ifdef USE_WOLFSSL
स्थिर पूर्णाङ्क wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolfSSL_get_app_data(ssl);
  संरचना Curl_cfilter *cf = conn_ref ? conn_ref->user_data : रिक्त;

  DEBUGASSERT(cf);
  सचे(cf && session) {
    संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
    संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    सचे(data && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned वर्ण *quic_tp = रिक्त;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       परिमाण(tpbuf));
      सचे(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((पूर्णाङ्क)tplen));
      अञ्ञथा {
        quic_tp = (unsigned वर्ण *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (शून्य)Curl_wssl_cache_session(cf, data, ctx->ssl_peer.scache_कुञ्चिका,
                                    session, wolfSSL_version(ssl),
                                    "h3", quic_tp, quic_tp_len, रिक्त);
    }
  }
  पटिनिवत्त 0;
}
#endif /* USE_WOLFSSL */

स्थिर CURLcode cf_ngtcp2_tls_ctx_setup(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        शून्य *user_data)
{
  संरचना curl_tls_ctx *ctx = user_data;

#ifdef USE_OPENSSL
#सचे defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  सचे(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(data, "ngtcp2_crypto_boringssl_configure_client_context failed");
    पटिनिवत्त CURLE_FAILED_INIT;
  }
#नोचेद्यदि defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#अञ्ञथा
  सचे(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_quictls_configure_client_context failed");
    पटिनिवत्त CURLE_FAILED_INIT;
  }
#endif /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  सचे(Curl_ssl_scache_use(cf, data)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_set_session_cache_mode(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#नोचेद्यदि defined(USE_GNUTLS)
  सचे(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(data, "ngtcp2_crypto_gnutls_configure_client_session failed");
    पटिनिवत्त CURLE_FAILED_INIT;
  }
  सचे(Curl_ssl_scache_use(cf, data)) {
    gnutls_handshake_set_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#नोचेद्यदि defined(USE_WOLFSSL)
  सचे(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    पटिनिवत्त CURLE_FAILED_INIT;
  }
  सचे(Curl_ssl_scache_use(cf, data)) {
    /* Register to get notified when a new session is received */
    wolfSSL_CTX_sess_set_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#endif
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode cf_ngtcp2_on_session_reuse(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *data,
                                           संरचना alpn_spec *alpns,
                                           संरचना Curl_ssl_session *scs,
                                           bool *do_early_data)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;
#सचे defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#endif
#ifdef USE_GNUTLS
  ctx->earlydata_max =
    gnutls_record_get_max_early_data_size(ctx->tls.gtls.session);
#endif
#ifdef USE_WOLFSSL
#ifdef WOLFSSL_EARLY_DATA
  ctx->earlydata_max = scs->earlydata_max;
#अञ्ञथा
  ctx->earlydata_max = 0;
#endif /* WOLFSSL_EARLY_DATA */
#endif
#सचे defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  सचे(!ctx->earlydata_max) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  अञ्ञथा सचे(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session from different ALPN, no early data");
  }
  अञ्ञथा सचे(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(data, cf, "no 0RTT transport parameters, no early data");
  }
  अञ्ञथा {
    पूर्णाङ्क rv;
    rv = ngtcp2_conn_decode_and_set_0rtt_transport_params(
      ctx->qconn, (ध्रुव uint8_t *)scs->quic_tp, scs->quic_tp_len);
    सचे(rv)
      CURL_TRC_CF(data, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    अञ्ञथा सचे(ctx->init_h3_conn_cb) {
      infof(data, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      result = ctx->init_h3_conn_cb(cf, data, ctx);
      सचे(!result) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_data = TRUE;
      }
    }
    अञ्ञथा { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_data = TRUE;
    }
  }
#अञ्ञथा /* not supported in the TLS backend */
  (शून्य)data;
  (शून्य)ctx;
  (शून्य)scs;
  (शून्य)alpns;
#endif
  पटिनिवत्त result;
}

/*
 * Might be called twice for happy eyeballs.
 */
स्थिर CURLcode cf_connect_start(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना cf_ngtcp2_io_ctx *pktx)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णाङ्क rc;
  पूर्णाङ्क rv;
  CURLcode result;
  ध्रुव संरचना Curl_sockaddr_ex *sockaddr = रिक्त;
  पूर्णाङ्क qfd;
  स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->dcid.data, NGTCP2_MAX_CIDLEN);
  सचे(result)
    पटिनिवत्त result;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->scid.data, NGTCP2_MAX_CIDLEN);
  सचे(result)
    पटिनिवत्त result;

  (शून्य)Curl_qlogdir(data, ctx->scid.data, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, data, pktx);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  सचे(result)
    पटिनिवत्त result;

  /* Query socket and remote address from sub-chain */
  सचे(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, रिक्त)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  सचे(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = परिमाण(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (संरचना sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    सचे(rv == -1)
      पटिनिवत्त CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (संरचना sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_path.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  अञ्ञथा {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ध्रुव संरचना Curl_sockaddr_ex *remote = रिक्त;
    सचे(cf->next->cft->query(cf->next, data, CF_QUERY_REMOTE_ADDR, रिक्त,
                            CURL_UNCONST(&remote)))
      पटिनिवत्त CURLE_QUIC_CONNECT_ERROR;
    सचे(!remote)
      पटिनिवत्त CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, परिमाण(ctx->q.local_addr));
    विकल्प(remote->family) {
    पक्ष AF_INET:
      ((संरचना sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = परिमाण(संरचना sockaddr_in);
      भङ्ग;
#ifdef USE_IPV6
    पक्ष AF_INET6:
      ((संरचना sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = परिमाण(संरचना sockaddr_in6);
      भङ्ग;
#endif
    स्वभाव:
      पटिनिवत्त CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (संरचना sockaddr *)&ctx->q.local_addr,
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
  सचे(rc)
    पटिनिवत्त CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_data = cf;

  result = Curl_vquic_tls_init(&ctx->tls, cf, data,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_setup, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  सचे(result)
    पटिनिवत्त result;

#सचे defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  सचे(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_ctx_new failed");
    पटिनिवत्त CURLE_FAILED_INIT;
  }
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  सचे(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_configure_client_session failed");
    पटिनिवत्त CURLE_FAILED_INIT;
  }
#नोचेद्यदि defined(USE_OPENSSL)
  SSL_set_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#नोचेद्यदि defined(USE_GNUTLS)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#नोचेद्यदि defined(USE_WOLFSSL)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#अञ्ञथा
#error "ngtcp2 TLS backend not defined"
#endif

#सचे defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  सचे(ctx->use_earlydata &&
     !SSL_set_quic_tls_early_data_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(data, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#endif
  ngtcp2_ccerr_default(&ctx->last_error);

  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    bool *done)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  संरचना cf_call_data save;
  संरचना cf_ngtcp2_io_ctx pktx;

  सचे(cf->connected) {
    *done = TRUE;
    पटिनिवत्त CURLE_OK;
  }

  /* Connect the sub-chain */
  सचे(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    सचे(result || !*done)
      पटिनिवत्त result;
  }

  *done = FALSE;

  सचे(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    पटिनिवत्त CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  CF_DATA_SAVE(save, cf, data);

  सचे(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(data);
    result = cf_connect_start(cf, data, &pktx);
    सचे(result)
      गच्छ out;
    सचे(cf->connected) {
      *done = TRUE;
      गच्छ out;
    }
    result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
    /* we do not expect to be able to recv anything yet */
    गच्छ out;
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  सचे(result)
    गच्छ out;

  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
  सचे(result)
    गच्छ out;

  सचे(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    result = ctx->tls_vrfy_result;
    सचे(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      cf->connected = TRUE;
      *done = TRUE;
    }
  }

out:
  सचे(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
  सचे(ctx->qconn &&
     ((result == CURLE_RECV_ERROR) || (result == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_in_draining_period(ctx->qconn)) {
    ध्रुव ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    result = CURLE_COULDNT_CONNECT;
    सचे(cerr) {
      CURL_TRC_CF(data, cf, "connect error, type=%d, code=%" PRIu64,
                  (पूर्णाङ्क)cerr->type, cerr->error_code);
      विकल्प(cerr->type) {
      पक्ष NGTCP2_CCERR_TYPE_VERSION_NEGOTIATION:
        CURL_TRC_CF(data, cf, "error in version negotiation");
        भङ्ग;
      स्वभाव:
        सचे(cerr->error_code >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(data, cf, "crypto error, tls alert=%u",
                      (unsigned पूर्णाङ्क)(cerr->error_code & 0xffU));
        }
        अञ्ञथा सचे(cerr->error_code == NGTCP2_CONNECTION_REFUSED) {
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
  सचे(result) {
    सचे(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      संरचना ip_quadruple ip;

      सचे(!Curl_cf_socket_peek(cf->next, data, रिक्त, रिक्त, &ip))
        infof(data, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    }
  }
#endif
  सचे(!result && ctx->qconn) {
    result = Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx);
  }
  सचे(result || *done)
    CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (पूर्णाङ्क)result, *done);
  CF_DATA_RESTORE(cf, save);
  पटिनिवत्त result;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data, bool *done)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना cf_call_data save;
  संरचना cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  सचे(cf->shutdown || !ctx->qconn) {
    *done = TRUE;
    पटिनिवत्त CURLE_OK;
  }

  सचे(!cf->next) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *done = TRUE;
    पटिनिवत्त CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);
  *done = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  सचे(!ctx->shutdown_started) {
    वर्ण buffer[NGTCP2_MAX_UDP_PAYLOAD_SIZE];
    ngtcp2_ssize nwritten;

    सचे(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
      CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
      result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
      सचे(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
        CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
        result = CURLE_OK;
        गच्छ out;
      }
      अञ्ञथा सचे(result) {
        CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                    (पूर्णाङ्क)result);
        *done = TRUE;
        गच्छ out;
      }
    }

    DEBUGASSERT(Curl_bufq_is_empty(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, रिक्त, /* path */
      रिक्त, /* pkt_info */
      (uint8_t *)buffer, परिमाण(buffer),
      &ctx->last_error, pktx.ts);
    CURL_TRC_CF(data, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (पूर्णाङ्क)ctx->last_error.type,
                ctx->last_error.error_code, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    सचे(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      result = Curl_bufq_write(&ctx->q.sendbuf, (ध्रुव unsigned वर्ण *)buffer,
                               (size_t)nwritten, &n);
      सचे(result) {
        CURL_TRC_CF(data, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (पूर्णाङ्क)result);
        गच्छ out;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.split_len = 0;
    }
  }

  सचे(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing egress");
    result = Curl_vquic_flush(cf, data, &ctx->q);
    सचे(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
      result = CURLE_OK;
      गच्छ out;
    }
    अञ्ञथा सचे(result) {
      CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                  (पूर्णाङ्क)result);
      *done = TRUE;
      गच्छ out;
    }
  }

  सचे(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
    result = CURLE_OK;
  }
out:
  CF_DATA_RESTORE(cf, save);
  पटिनिवत्त result;
}

शून्य Curl_cf_ngtcp2_cmn_conn_close(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  bool done;
  Curl_cf_ngtcp2_cmn_shutdown(cf, data, &done);
}

स्थिर bool cf_ngtcp2_err_is_fatal(पूर्णाङ्क code)
{
  पटिनिवत्त (NGTCP2_ERR_FATAL >= code) ||
         (NGTCP2_ERR_DROP_CONN == code) ||
         (NGTCP2_ERR_IDLE_CLOSE == code);
}

शून्य Curl_cf_ngtcp2_cmn_err_set(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data, पूर्णाङ्क code)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  सचे(!ctx->last_error.error_code) {
    सचे(NGTCP2_ERR_CRYPTO == code) {
      ngtcp2_ccerr_set_tls_alert(&ctx->last_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 रिक्त, 0);
    }
    अञ्ञथा {
      ngtcp2_ccerr_set_liberr(&ctx->last_error, code, रिक्त, 0);
    }
  }
  सचे(cf_ngtcp2_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

शून्य Curl_cf_ngtcp2_io_ctx_init(संरचना cf_ngtcp2_io_ctx *io_ctx,
                                संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->data = data;
  io_ctx->now = *Curl_pgrs_now(data);
  ngtcp2_path_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_set_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

शून्य Curl_cf_ngtcp2_io_ctx_update_time(संरचना Curl_easy *data,
                                       संरचना cf_ngtcp2_io_ctx *io_ctx,
                                       संरचना Curl_cfilter *cf)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(data);
  Curl_vquic_ctx_update_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#सचे NGTCP2_VERSION_अङ्कः < 0x011100
संरचना cf_ngtcp2_sfind_ctx {
  int64_t stream_id;
  संरचना h3_stream_ctx *stream;
  uint32_t mid;
};

स्थिर bool cf_ngtcp2_sfind(uint32_t mid, शून्य *मानम्, शून्य *user_data)
{
  संरचना cf_ngtcp2_sfind_ctx *fctx = user_data;
  संरचना h3_stream_ctx *stream = मानम्;

  सचे(fctx->stream_id == stream->id) {
    fctx->mid = mid;
    fctx->stream = stream;
    पटिनिवत्त FALSE;
  }
  पटिनिवत्त TRUE; /* continue */
}

स्थिर संरचना h3_stream_ctx *cf_ngtcp2_get_stream(संरचना cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  संरचना cf_ngtcp2_sfind_ctx fctx;
  fctx.stream_id = stream_id;
  fctx.stream = रिक्त;
  Curl_u32_ptrset_visit(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  पटिनिवत्त fctx.stream;
}
#अञ्ञथा
स्थिर संरचना h3_stream_ctx *cf_ngtcp2_get_stream(संरचना cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  पटिनिवत्त ngtcp2_conn_get_stream_user_data(ctx->qconn, stream_id);
}
#endif

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
स्थिर CURLcode read_pkt_to_send(शून्य *userp,
                                 unsigned वर्ण *buf, size_t buflen,
                                 size_t *pnread)
{
  संरचना cf_ngtcp2_io_ctx *x = userp;
  संरचना cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t stream_id;
  पूर्णाङ्क fin;
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
  अत्थाय(;;) {

    सचे(ctx->h3conn && ngtcp2_conn_get_max_data_वामः(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_stream(ctx->h3conn, &stream_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      सचे(veccnt < 0) {
        failf(x->data, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((पूर्णाङ्क)veccnt));
        Curl_cf_ngtcp2_h3_err_set(x->cf, x->data, (पूर्णाङ्क)veccnt);
        पटिनिवत्त CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_STREAM_FLAG_MORE |
            (fin ? NGTCP2_WRITE_STREAM_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_stream(ctx->qconn, &x->ps.path,
                                  रिक्त, buf, buflen,
                                  &ndatalen, flags, stream_id,
                                  (ध्रुव ngtcp2_vec *)vec, veccnt, x->ts);
    सचे(n == 0) {
      /* nothing to send */
      पटिनिवत्त CURLE_AGAIN;
    }
    अञ्ञथा सचे(n < 0) {
      विकल्प(n) {
      पक्ष NGTCP2_ERR_STREAM_DATA_BLOCKED: {
        संरचना h3_stream_ctx *stream;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_block_stream(ctx->h3conn, stream_id);
        CURL_TRC_CF(x->data, x->cf, "[%" PRId64 "] block quic flow",
                    stream_id);
        stream = cf_ngtcp2_get_stream(ctx, stream_id);
        सचे(stream) /* it might be not one of our h3 streams? */
          stream->quic_flow_blocked = TRUE;
        n = 0;
        भङ्ग;
      }
      पक्ष NGTCP2_ERR_STREAM_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_stream_write(ctx->h3conn, stream_id);
        n = 0;
        भङ्ग;
      पक्ष NGTCP2_ERR_WRITE_MORE:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        भङ्ग;
      स्वभाव:
        DEBUGASSERT(ndatalen == -1);
        failf(x->data, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((पूर्णाङ्क)n));
        Curl_cf_ngtcp2_cmn_err_set(x->cf, x->data, (पूर्णाङ्क)n);
        पटिनिवत्त CURLE_SEND_ERROR;
      }
    }

    सचे(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      पूर्णाङ्क rv = nghttp3_conn_add_write_offset(ctx->h3conn, stream_id, ndatalen);
      सचे(rv) {
        failf(x->data, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        पटिनिवत्त CURLE_SEND_ERROR;
      }
    }

    सचे(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      पटिनिवत्त CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_progress_egress(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        संरचना cf_ngtcp2_io_ctx *pktx)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_size, path_max_payload_size;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_quantum;
  CURLcode result;
  संरचना cf_ngtcp2_io_ctx local_pktx;

  सचे(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  अञ्ञथा {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  सचे(result) {
    सचे(result == CURLE_AGAIN) {
      Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
      पटिनिवत्त CURLE_OK;
    }
    पटिनिवत्त result;
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
  अत्थाय(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_size,
                            read_pkt_to_send, pktx, &nread);
    सचे(result == CURLE_AGAIN)
      भङ्ग;
    अञ्ञथा सचे(result)
      पटिनिवत्त result;
    अञ्ञथा {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      सचे((buflen >= send_quantum) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_size))
        भङ्ग;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      सचे(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      अञ्ञथा सचे(nread > gsolen ||
              (gsolen > path_max_payload_size && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        result = Curl_vquic_send_tail_split(cf, data, &ctx->q,
                                            gsolen, nread, nread);
        सचे(result) {
          सचे(result == CURLE_AGAIN) {
            Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
            पटिनिवत्त CURLE_OK;
          }
          पटिनिवत्त result;
        }
        pktcnt = 0;
      }
      अञ्ञथा सचे(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        भङ्ग;
      }
    }
  }

  सचे(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(data, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
    सचे(result) {
      सचे(result == CURLE_AGAIN) {
        Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
        पटिनिवत्त CURLE_OK;
      }
      पटिनिवत्त result;
    }
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_conn_update_pkt_tx_time(ctx->qconn, pktx->ts);
  }
  पटिनिवत्त CURLE_OK;
}

संरचना cf_ngtcp2_recv_ctx {
  संरचना cf_ngtcp2_io_ctx *pktx;
  size_t pkt_गणनम्;
};

स्थिर CURLcode cf_ngtcp2_recv_pkts(ध्रुव unsigned वर्ण *buf, size_t buflen,
                                    size_t gso_size,
                                    संरचना sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    शून्य *userp)
{
  संरचना cf_ngtcp2_recv_ctx *rctx = userp;
  संरचना cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  संरचना cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_path path;
  size_t offset, pktlen;
  पूर्णाङ्क rv;

  सचे(!rctx->pkt_गणनम्) {
    Curl_cf_ngtcp2_io_ctx_update_time(pktx->data, pktx, pktx->cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  सचे(ecn)
    CURL_TRC_CF(pktx->data, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_size, (unsigned)ecn);
  ngtcp2_addr_init(&path.local, (संरचना sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&path.remote, (संरचना sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  अत्थाय(offset = 0; offset < buflen; offset += gso_size) {
    rctx->pkt_गणनम्++;
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &path, &pi,
                              buf + offset, pktlen, pktx->ts);
    सचे(rv) {
      CURL_TRC_CF(pktx->data, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_set(pktx->cf, pktx->data, rv);

      सचे(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        पटिनिवत्त CURLE_PEER_FAILED_VERIFICATION;
      पटिनिवत्त CURLE_RECV_ERROR;
    }
  }
  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_progress_ingress(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *data,
                                         संरचना cf_ngtcp2_io_ctx *pktx)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना cf_ngtcp2_io_ctx local_pktx;
  संरचना cf_ngtcp2_recv_ctx rctx;
  CURLcode result = CURLE_OK;

  सचे(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }

  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  सचे(result)
    पटिनिवत्त result;

  rctx.pktx = pktx;
  rctx.pkt_गणनम् = 0;

  सचे(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(data, cf, "progress_ingress(socket)");
    पटिनिवत्त Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  अञ्ञथा {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    unsigned वर्ण *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_SIZE;
    size_t pkt_limit = QUIC_TUNNEL_INGRESS_PKT_LIMIT;
    size_t nread;
    संरचना sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(data, cf, "progress_ingress(sub-filters)");
    सचे(ctx->qconn) {
      size_t max_path_payload;
      max_path_payload =
        ngtcp2_conn_get_path_max_tx_udp_payload_size(ctx->qconn);
      सचे(max_path_payload > max_udp_payload)
        max_udp_payload = max_path_payload;
    }

    सचे(ctx->tunnel_inbuf_len < max_udp_payload) {
      unsigned वर्ण *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      सचे(!newbuf)
        पटिनिवत्त CURLE_OUT_OF_MEMORY;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    याव(pkt_limit--) {
      result = Curl_conn_cf_recv(cf->next, data, (वर्ण *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      सचे(result == CURLE_AGAIN) {
        /* no more data available at the moment */
        पटिनिवत्त CURLE_OK;
      }
      सचे(result) {
        CURL_TRC_CF(data, cf, "ingress, recv from tunnel failed: %d",
                    (पूर्णाङ्क)result);
        पटिनिवत्त result;
      }
      सचे(nread == 0) {
        /* tunnel closed */
        पटिनिवत्त CURLE_OK;
      }

      memcpy(&remote_addr, ctx->connected_path.remote.addr,
             ctx->connected_path.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_path.remote.addrlen;
      result = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      सचे(result)
        पटिनिवत्त result;

      सचे(!ctx->q.got_first_byte) {
        ctx->q.got_first_byte = TRUE;
        ctx->q.first_byte_at = ctx->q.last_op;
      }
      ctx->q.last_io = ctx->q.last_op;
    }
    पटिनिवत्त CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_set_expiry(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data,
                                       संरचना cf_ngtcp2_io_ctx *pktx)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  सचे(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  अञ्ञथा {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  सचे(expiry != UINT64_MAX) {
    सचे(expiry <= pktx->ts) {
      CURLcode result;
      पूर्णाङ्क rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      सचे(rv) {
        failf(data, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_set(cf, data, rv);
        पटिनिवत्त CURLE_SEND_ERROR;
      }
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, pktx);
      सचे(!result)
        result = Curl_cf_ngtcp2_progress_egress(cf, data, pktx);
      सचे(result) {
        /* a verify failure during ingress must win over generic errors */
        सचे(ctx->tls_vrfy_result)
          result = ctx->tls_vrfy_result;
        पटिनिवत्त result;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    सचे(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      सचे(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_set(data, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर शून्य cf_ngtcp2_setup_keep_alive(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  ध्रुव ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  सचे(!ctx->qconn)
    पटिनिवत्त;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  सचे(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no peer idle timeout, unset keep-alive");
  }
  अञ्ञथा सचे(!Curl_u32_ptrset_गणनम्(&ctx->streams)) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no active streams, unset keep-alive");
  }
  अञ्ञथा {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(data, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_stream_setup(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        संरचना h3_stream_ctx **pstream)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  *pstream = रिक्त;
  सचे(!data)
    पटिनिवत्त CURLE_FAILED_INIT;

  सचे(stream) {
    *pstream = stream;
    पटिनिवत्त CURLE_OK;
  }

  stream = curlx_calloc(1, परिमाण(*stream));
  सचे(!stream)
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

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
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_पङ्क्तिः_LEN);

  सचे(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    Curl_cf_ngtcp2_h3_stream_ctx_free(stream);
    पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }

  सचे(Curl_u32_ptrset_गणनम्(&ctx->streams) == 1)
    cf_ngtcp2_setup_keep_alive(cf, data);

  *pstream = stream;
  पटिनिवत्त CURLE_OK;
}

शून्य Curl_cf_ngtcp2_h3_stream_close(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    संरचना h3_stream_ctx *stream)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(data);
  DEBUGASSERT(stream);
  सचे(!stream->closed && ctx->qconn && ctx->h3conn) {
    CURLcode result;

    nghttp3_conn_set_stream_user_data(ctx->h3conn, stream->id, रिक्त);
    ngtcp2_conn_set_stream_user_data(ctx->qconn, stream->id, रिक्त);
    stream->closed = TRUE;
    (शून्य)ngtcp2_conn_shutdown_stream(ctx->qconn, 0, stream->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    result = Curl_cf_ngtcp2_progress_egress(cf, data, रिक्त);
    सचे(result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cancel stream -> %d",
                  stream->id, (पूर्णाङ्क)result);
  }
}

शून्य Curl_cf_ngtcp2_h3_stream_done(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  (शून्य)cf;
  सचे(stream) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] easy handle is done", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
    सचे(!Curl_u32_ptrset_गणनम्(&ctx->streams))
      cf_ngtcp2_setup_keep_alive(cf, data);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      bool *निवेशः_pending)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ध्रुव ngtcp2_transport_params *rp;
  संरचना cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  *निवेशः_pending = FALSE;
  सचे(!ctx->qconn || ctx->shutdown_started)
    गच्छ out;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  सचे(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->q.last_io);
    सचे(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      सचे((uint64_t)idletime_ms > max_idle_ms)
        गच्छ out;
    }
  }

  सचे(!cf->next || !cf->next->cft->is_alive(cf->next, data, निवेशः_pending))
    गच्छ out;

  alive = TRUE;
  सचे(*निवेशः_pending) {
    CURLcode result;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *निवेशः_pending = FALSE;
    result = Curl_cf_ngtcp2_progress_ingress(cf, data, रिक्त);
    CURL_TRC_CF(data, cf, "is_alive, progress ingress -> %d", (पूर्णाङ्क)result);
    alive = result ? FALSE : TRUE;
  }

out:
  CF_DATA_RESTORE(cf, save);
  पटिनिवत्त alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(संरचना cf_ngtcp2_ctx *ctx,
                                      संरचना Curl_easy *data)
{
  int64_t ctrl_stream_id, qpack_enc_stream_id, qpack_dec_stream_id;
  पूर्णाङ्क rc;

  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &ctrl_stream_id, रिक्त);
  सचे(rc) {
    failf(data, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    पटिनिवत्त CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_stream(ctx->h3conn, ctrl_stream_id);
  सचे(rc) {
    failf(data, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    पटिनिवत्त CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_enc_stream_id, रिक्त);
  सचे(rc) {
    failf(data, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    पटिनिवत्त CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_dec_stream_id, रिक्त);
  सचे(rc) {
    failf(data, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    पटिनिवत्त CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_stream_id,
                                       qpack_dec_stream_id);
  सचे(rc) {
    failf(data, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    पटिनिवत्त CURLE_QUIC_CONNECT_ERROR;
  }
  पटिनिवत्त CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;

  विकल्प(query) {
  पक्ष CF_QUERY_CONNECT_REPLY_MS:
    सचे((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (पूर्णाङ्क)ms : INT_MAX;
      पटिनिवत्त CURLE_OK;
    }
    भङ्ग;
  पक्ष CF_QUERY_REALLY_CONNECTED:
    सचे(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_first_byte;
      पटिनिवत्त CURLE_OK;
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  पटिनिवत्त cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (शून्य)arg1;
  (शून्य)arg2;
  विकल्प(event) {
  पक्ष CF_CTRL_REPORT_STATS:
    सचे(cf->connected && !ctx->stats_reported) {
      सचे((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        ctx->stats_reported = TRUE;
      }
      अञ्ञथा सचे(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        सचे(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  पटिनिवत्त result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
