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

#जर !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#अन्तर्भाव <ngtcp2/ngtcp2.h>

#ifdef USE_OPENSSL
#अन्तर्भाव <openssl/err.h>
#जर defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_boringssl.h>
#किंवाजर defined(OPENSSL_QUIC_API2)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_ossl.h>
#अन्यथा
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#अन्तर्भाव "vtls/openssl.h"
#किंवाजर defined(USE_GNUTLS)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_gnutls.h>
#अन्तर्भाव "vtls/gtls.h"
#किंवाजर defined(USE_WOLFSSL)
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
शून्य Curl_ngtcp2_ver(अक्षर *p, माप_प्रकार len)
{
  स्थिरांक ngtcp2_info *ng2 = ngtcp2_version(0);
  स्थिरांक nghttp3_info *ht3 = nghttp3_version(0);
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

स्थिर bool cf_ngtcp2_h3_err_is_fatal(पूर्णांक code)
{
  परत (NGHTTP3_ERR_FATAL >= code) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM == code);
}

शून्य Curl_cf_ngtcp2_h3_err_set(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data, पूर्णांक code)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  जर(!ctx->last_error.error_code) {
    ngtcp2_ccerr_set_application_error(&ctx->last_error,
      nghttp3_err_infer_quic_app_error_code(code), रिक्त, 0);
  }
  जर(cf_ngtcp2_h3_err_is_fatal(code))
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
  परत Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

शून्य Curl_cf_ngtcp2_ctx_cleanup(संरचना cf_ngtcp2_ctx *ctx)
{
  जर(ctx && ctx->initialized) {
    जर(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    जर(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = रिक्त;
    }
    जर(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = रिक्त;
    }
#ifdef OPENSSL_QUIC_API2
    जर(ctx->ossl_ctx) {
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

  जर(pstream)
    *pstream = stream;
  जर(stream && (stream->mid != UINT32_MAX)) {
    संरचना Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    जर(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      परत रिक्त;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    जर(data && (data->id == stream->xfer_id))
      परत data;
    DEBUGASSERT(0);
  }
  परत रिक्त;
}

स्थिर ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  संरचना Curl_cfilter *cf = conn_ref->user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  परत ctx->qconn;
}

#ifdef DEBUG_NGTCP2
स्थिर शून्य quic_printf(शून्य *user_data, स्थिरांक अक्षर *fmt, ...)
{
  बहु_सूची ap;
  (शून्य)user_data;
  बहु_शुरू(ap, fmt);
  curl_mvfprintf(मानक_त्रुटि, fmt, ap);
  बहु_पूर्ण(ap);
  curl_mfprintf(मानक_त्रुटि, "\n");
}
#endif

स्थिर शून्य qlog_callback(शून्य *user_data, uint32_t flags,
                          स्थिरांक शून्य *data, माप_प्रकार datalen)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  (शून्य)flags;
  जर(ctx->qlogfd != -1) {
    ssize_t rc = लिख(ctx->qlogfd, data, datalen);
    जर(rc == -1) {
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
#अन्यथा
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
  जर(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#जर defined(_MSC_VER) && defined(_DLL)
#pragma warning(ढकला)
#pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

स्थिर पूर्णांक cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, शून्य *user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf ? cf->ctx : रिक्त;
  संरचना Curl_easy *data;

  (शून्य)tconn;
  DEBUGASSERT(ctx);
  data = CF_DATA_CURRENT(cf);
  DEBUGASSERT(data);
  जर(!ctx || !data)
    परत NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(data);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, data);

  ctx->tls_vrfy_result = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    data, &ctx->ssl_peer);
  जर(ctx->tls_vrfy_result)
    परत NGTCP2_ERR_CALLBACK_FAILURE;

#ifdef CURLVERBOSE
  जर(Curl_trc_is_verbose(data)) {
    स्थिरांक ngtcp2_transport_params *rp;
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
  जर(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  जर(ctx->use_earlydata) {
#जर defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    जर(SSL_get_early_data_status(ctx->tls.ossl.ssl) == SSL_EARLY_DATA_NOT_SENT)
      CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
#endif

#जर NGTCP2_VERSION_अंक >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_data_rejected2(ctx->qconn);
#अन्यथा /* older NGTCP2 */
    {
#जर defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      पूर्णांक ossl_early_status = SSL_get_early_data_status(ctx->tls.ossl.ssl);
      जर(ossl_early_status == SSL_EARLY_DATA_NOT_SENT)
        CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_status == SSL_EARLY_DATA_ACCEPTED);
#किंवाजर defined(USE_GNUTLS)
      पूर्णांक flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_DATA);
#किंवाजर defined(USE_WOLFSSL)
#ifdef WOLFSSL_EARLY_DATA
      ctx->earlydata_accepted =
        (wolfSSL_get_early_data_status(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_DATA_REJECTED);
#अन्यथा
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
  परत 0;
}

स्थिर पूर्णांक cb_recv_stream_data(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t stream_id, uint64_t offset,
                               स्थिरांक uint8_t *buf, माप_प्रकार buflen,
                               शून्य *user_data, शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  पूर्णांक fin = (flags & NGTCP2_STREAM_DATA_FLAG_FIN) ? 1 : 0;
  संरचना h3_stream_ctx *stream;
  संरचना Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (शून्य)offset;
  rc = nghttp3_conn_read_stream(ctx->h3conn, stream_id, buf, buflen, fin);
  जर(rc < 0) {
    जर(data && stream) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  stream_id, stream->reset, stream->closed);
    }
    परत NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  जर(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_stream_offset(tconn, stream_id, nconsumed);
    ngtcp2_conn_extend_max_offset(tconn, nconsumed);
    जर(stream) {
      stream->rx_offset += nconsumed;
      stream->rx_offset_max += nconsumed;
    }
  }
  परत 0;
}

स्थिर पूर्णांक cb_acked_stream_data_offset(ngtcp2_conn *tconn, int64_t stream_id,
                                       uint64_t offset, uint64_t datalen,
                                       शून्य *user_data, शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णांक rv;
  (शून्य)stream_id;
  (शून्य)tconn;
  (शून्य)offset;
  (शून्य)datalen;
  (शून्य)stream_user_data;

  rv = nghttp3_conn_add_ack_offset(ctx->h3conn, stream_id, datalen);
  जर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    परत NGTCP2_ERR_CALLBACK_FAILURE;
  }

  परत 0;
}

स्थिर पूर्णांक cb_stream_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id, uint64_t app_error_code,
                           शून्य *user_data, शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, रिक्त);
  पूर्णांक rv;

  (शून्य)tconn;
  /* stream is closed... */
  जर(!data)
    data = CF_DATA_CURRENT(cf);
  जर(!data)
    परत NGTCP2_ERR_CALLBACK_FAILURE;

  जर(!(flags & NGTCP2_STREAM_CLOSE_FLAG_APP_ERROR_CODE_SET)) {
    app_error_code = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, app_error_code, rv);
  जर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    परत NGTCP2_ERR_CALLBACK_FAILURE;
  }

  परत 0;
}

#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
स्थिर पूर्णांक cb_stream_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id,
                           uint64_t rx_app_error_code,
                           uint64_t tx_app_error_code,
                           शून्य *user_data, शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, रिक्त);
  uint64_t h3_app_error_code = NGHTTP3_H3_NO_ERROR;
  पूर्णांक rv;

  (शून्य)tconn;
  (शून्य)tx_app_error_code;
  /* stream is closed... */
  जर(!data)
    data = CF_DATA_CURRENT(cf);
  जर(!data)
    परत NGTCP2_ERR_CALLBACK_FAILURE;

  जर(flags & NGTCP2_STREAM_CLOSE2_FLAG_RX_APP_ERROR_CODE_SET)
    h3_app_error_code = rx_app_error_code;

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, h3_app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, h3_app_error_code, rv);
  जर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    परत NGTCP2_ERR_CALLBACK_FAILURE;
  }

  परत 0;
}
#endif

स्थिर पूर्णांक cb_stream_reset(ngtcp2_conn *tconn, int64_t stream_id,
                           uint64_t final_size, uint64_t app_error_code,
                           शून्य *user_data, शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, रिक्त);
  पूर्णांक rv;
  (शून्य)tconn;
  (शून्य)final_size;
  (शून्य)app_error_code;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  जर(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  जर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    परत NGTCP2_ERR_CALLBACK_FAILURE;
  }

  परत 0;
}

स्थिर पूर्णांक cb_stream_stop_sending(ngtcp2_conn *tconn, int64_t stream_id,
                                  uint64_t app_error_code, शून्य *user_data,
                                  शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णांक rv;
  (शून्य)tconn;
  (शून्य)app_error_code;
  (शून्य)stream_user_data;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  जर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    परत NGTCP2_ERR_CALLBACK_FAILURE;
  }

  परत 0;
}

स्थिर पूर्णांक cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            शून्य *user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);

  (शून्य)tconn;
  ctx->max_bidi_streams = max_streams;
  जर(data)
    CURL_TRC_CF(data, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->used_bidi_streams);
  परत 0;
}

स्थिर पूर्णांक cb_extend_max_stream_data(ngtcp2_conn *tconn, int64_t stream_id,
                                     uint64_t max_data, शून्य *user_data,
                                     शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना h3_stream_ctx *stream;
  संरचना Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  पूर्णांक rv;
  (शून्य)tconn;
  (शून्य)max_data;

  rv = nghttp3_conn_unblock_stream(ctx->h3conn, stream_id);
  जर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    परत NGTCP2_ERR_CALLBACK_FAILURE;
  }
  जर(stream && stream->quic_flow_blocked) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] unblock quic flow", stream_id);
    stream->quic_flow_blocked = FALSE;
    जर(data)
      Curl_multi_mark_dirty(data);
  }
  परत 0;
}

स्थिर शून्य cb_rand(uint8_t *dest, माप_प्रकार destlen,
                    स्थिरांक ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode result;
  (शून्य)rand_ctx;

  result = Curl_rand(रिक्त, dest, destlen);
  जर(result) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    स_रखो(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
स्थिर पूर्णांक cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, माप_प्रकार cidlen,
                                    शून्य *user_data)
{
  CURLcode result;
  (शून्य)tconn;
  (शून्य)user_data;

  result = Curl_rand(रिक्त, cid->data, cidlen);
  जर(result)
    परत NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(रिक्त, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  जर(result)
    परत NGTCP2_ERR_CALLBACK_FAILURE;

  परत 0;
}

#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
स्थिर पूर्णांक cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  संरचना ngtcp2_stateless_reset_token *token, माप_प्रकार cidlen, शून्य *user_data)
{
  CURLcode result;
  (शून्य)tconn;
  (शून्य)user_data;

  result = Curl_rand(रिक्त, cid->data, cidlen);
  जर(result)
    परत NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(रिक्त, token->data, आकार(token->data));
  जर(result)
    परत NGTCP2_ERR_CALLBACK_FAILURE;

  परत 0;
}
#endif

स्थिर पूर्णांक cb_recv_rx_किल्ली(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          शून्य *user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_ngtcp2_ctx *ctx = cf ? cf->ctx : रिक्त;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  (शून्य)tconn;

  जर(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    परत 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(data);
  जर(ctx && data && !ctx->h3conn && ctx->init_h3_conn_cb) {
    जर(ctx->init_h3_conn_cb(cf, data, ctx))
      परत NGTCP2_ERR_CALLBACK_FAILURE;
  }
  परत 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ढकला
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
  ngtcp2_crypto_update_किल्ली_cb, /* update_key */
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
  cb_recv_rx_किल्ली,
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
#pragma GCC diagnostic काढा
#endif

#जर defined(_MSC_VER) && defined(_DLL)
#pragma warning(काढा)
#endif

#ifdef USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
स्थिर पूर्णांक quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  संरचना Curl_cfilter *cf;
  संरचना cf_ngtcp2_ctx *ctx;
  संरचना Curl_easy *data;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_data(ssl);
  cf = cref ? cref->user_data : रिक्त;
  ctx = cf ? cf->ctx : रिक्त;
  data = cf ? CF_DATA_CURRENT(cf) : रिक्त;
  जर(cf && data && ctx) {
    अचिन्हित अक्षर *quic_tp = रिक्त;
    माप_प्रकार quic_tp_len = 0;
#ifdef HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     आकार(tpbuf));
    जर(tplen < 0)
      CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((पूर्णांक)tplen));
    अन्यथा {
      quic_tp = (अचिन्हित अक्षर *)tpbuf;
      quic_tp_len = (माप_प्रकार)tplen;
    }
#endif
    Curl_ossl_add_session(cf, data, &ctx->tls.ossl, ctx->ssl_peer.scache_किल्ली,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, रिक्त);
  }
  परत 0;
}
#endif /* USE_OPENSSL */

#ifdef USE_GNUTLS

#ifdef CURLVERBOSE
स्थिर स्थिरांक अक्षर *gtls_hs_संदेश_name(पूर्णांक mtype)
{
  निवड(mtype) {
  स्थिती 1:
    परत "ClientHello";
  स्थिती 2:
    परत "ServerHello";
  स्थिती 4:
    परत "SessionTicket";
  स्थिती 8:
    परत "EncryptedExtensions";
  स्थिती 11:
    परत "Certificate";
  स्थिती 13:
    परत "CertificateRequest";
  स्थिती 15:
    परत "CertificateVerify";
  स्थिती 20:
    परत "Finished";
  स्थिती 24:
    परत "KeyUpdate";
  स्थिती 254:
    परत "MessageHash";
  }
  परत "Unknown";
}
#endif

स्थिर पूर्णांक quic_gtls_handshake_cb(gnutls_session_t session, अचिन्हित पूर्णांक htype,
                                  अचिन्हित when, अचिन्हित पूर्णांक incoming,
                                  स्थिरांक gnutls_datum_t *संदेश)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  संरचना Curl_cfilter *cf = conn_ref ? conn_ref->user_data : रिक्त;
  संरचना cf_ngtcp2_ctx *ctx = cf ? cf->ctx : रिक्त;

  (शून्य)संदेश;
  (शून्य)incoming;
  जर(when && cf && ctx) { /* after message has been processed */
    संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    जर(!data)
      परत 0;
    CURL_TRC_CF(data, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_संदेश_name(htype), htype);
    निवड(htype) {
    स्थिती GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      अचिन्हित अक्षर *quic_tp = रिक्त;
      माप_प्रकार quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       आकार(tpbuf));
      जर(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((पूर्णांक)tplen));
      अन्यथा {
        quic_tp = (अचिन्हित अक्षर *)tpbuf;
        quic_tp_len = (माप_प्रकार)tplen;
      }
      (शून्य)Curl_gtls_cache_session(cf, data, ctx->ssl_peer.scache_किल्ली,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    रिक्त);
      थांबा;
    }
    पूर्वनिर्धारित:
      थांबा;
    }
  }
  परत 0;
}
#endif /* USE_GNUTLS */

#ifdef USE_WOLFSSL
स्थिर पूर्णांक wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolfSSL_get_app_data(ssl);
  संरचना Curl_cfilter *cf = conn_ref ? conn_ref->user_data : रिक्त;

  DEBUGASSERT(cf);
  जर(cf && session) {
    संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
    संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    जर(data && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      अचिन्हित अक्षर *quic_tp = रिक्त;
      माप_प्रकार quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       आकार(tpbuf));
      जर(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((पूर्णांक)tplen));
      अन्यथा {
        quic_tp = (अचिन्हित अक्षर *)tpbuf;
        quic_tp_len = (माप_प्रकार)tplen;
      }
      (शून्य)Curl_wssl_cache_session(cf, data, ctx->ssl_peer.scache_किल्ली,
                                    session, wolfSSL_version(ssl),
                                    "h3", quic_tp, quic_tp_len, रिक्त);
    }
  }
  परत 0;
}
#endif /* USE_WOLFSSL */

स्थिर CURLcode cf_ngtcp2_tls_ctx_setup(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        शून्य *user_data)
{
  संरचना curl_tls_ctx *ctx = user_data;

#ifdef USE_OPENSSL
#जर defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  जर(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(data, "ngtcp2_crypto_boringssl_configure_client_context failed");
    परत CURLE_FAILED_INIT;
  }
#किंवाजर defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#अन्यथा
  जर(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_quictls_configure_client_context failed");
    परत CURLE_FAILED_INIT;
  }
#endif /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  जर(Curl_ssl_scache_use(cf, data)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_set_session_cache_mode(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#किंवाजर defined(USE_GNUTLS)
  जर(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(data, "ngtcp2_crypto_gnutls_configure_client_session failed");
    परत CURLE_FAILED_INIT;
  }
  जर(Curl_ssl_scache_use(cf, data)) {
    gnutls_handshake_set_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#किंवाजर defined(USE_WOLFSSL)
  जर(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    परत CURLE_FAILED_INIT;
  }
  जर(Curl_ssl_scache_use(cf, data)) {
    /* Register to get notified when a new session is received */
    wolfSSL_CTX_sess_set_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#endif
  परत CURLE_OK;
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
#जर defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#endif
#ifdef USE_GNUTLS
  ctx->earlydata_max =
    gnutls_record_get_max_early_data_size(ctx->tls.gtls.session);
#endif
#ifdef USE_WOLFSSL
#ifdef WOLFSSL_EARLY_DATA
  ctx->earlydata_max = scs->earlydata_max;
#अन्यथा
  ctx->earlydata_max = 0;
#endif /* WOLFSSL_EARLY_DATA */
#endif
#जर defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  जर(!ctx->earlydata_max) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  अन्यथा जर(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session from different ALPN, no early data");
  }
  अन्यथा जर(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(data, cf, "no 0RTT transport parameters, no early data");
  }
  अन्यथा {
    पूर्णांक rv;
    rv = ngtcp2_conn_decode_and_set_0rtt_transport_params(
      ctx->qconn, (स्थिरांक uint8_t *)scs->quic_tp, scs->quic_tp_len);
    जर(rv)
      CURL_TRC_CF(data, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    अन्यथा जर(ctx->init_h3_conn_cb) {
      infof(data, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      result = ctx->init_h3_conn_cb(cf, data, ctx);
      जर(!result) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_data = TRUE;
      }
    }
    अन्यथा { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_data = TRUE;
    }
  }
#अन्यथा /* not supported in the TLS backend */
  (शून्य)data;
  (शून्य)ctx;
  (शून्य)scs;
  (शून्य)alpns;
#endif
  परत result;
}

/*
 * Might be called twice for happy eyeballs.
 */
स्थिर CURLcode cf_connect_start(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना cf_ngtcp2_io_ctx *pktx)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णांक rc;
  पूर्णांक rv;
  CURLcode result;
  स्थिरांक संरचना Curl_sockaddr_ex *sockaddr = रिक्त;
  पूर्णांक qfd;
  स्थिर स्थिरांक संरचना alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->dcid.data, NGTCP2_MAX_CIDLEN);
  जर(result)
    परत result;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->scid.data, NGTCP2_MAX_CIDLEN);
  जर(result)
    परत result;

  (शून्य)Curl_qlogdir(data, ctx->scid.data, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, data, pktx);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  जर(result)
    परत result;

  /* Query socket and remote address from sub-chain */
  जर(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, रिक्त)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  जर(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = आकार(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (संरचना sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    जर(rv == -1)
      परत CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (संरचना sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_path.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  अन्यथा {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    स्थिरांक संरचना Curl_sockaddr_ex *remote = रिक्त;
    जर(cf->next->cft->query(cf->next, data, CF_QUERY_REMOTE_ADDR, रिक्त,
                            CURL_UNCONST(&remote)))
      परत CURLE_QUIC_CONNECT_ERROR;
    जर(!remote)
      परत CURLE_QUIC_CONNECT_ERROR;

    स_रखो(&ctx->q.local_addr, 0, आकार(ctx->q.local_addr));
    निवड(remote->family) {
    स्थिती AF_INET:
      ((संरचना sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = आकार(संरचना sockaddr_in);
      थांबा;
#ifdef USE_IPV6
    स्थिती AF_INET6:
      ((संरचना sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = आकार(संरचना sockaddr_in6);
      थांबा;
#endif
    पूर्वनिर्धारित:
      परत CURLE_QUIC_CONNECT_ERROR;
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
  जर(rc)
    परत CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_data = cf;

  result = Curl_vquic_tls_init(&ctx->tls, cf, data,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_setup, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  जर(result)
    परत result;

#जर defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  जर(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_ctx_new failed");
    परत CURLE_FAILED_INIT;
  }
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  जर(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_configure_client_session failed");
    परत CURLE_FAILED_INIT;
  }
#किंवाजर defined(USE_OPENSSL)
  SSL_set_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#किंवाजर defined(USE_GNUTLS)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#किंवाजर defined(USE_WOLFSSL)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#अन्यथा
#error "ngtcp2 TLS backend not defined"
#endif

#जर defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  जर(ctx->use_earlydata &&
     !SSL_set_quic_tls_early_data_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(data, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#endif
  ngtcp2_ccerr_default(&ctx->last_error);

  परत CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    bool *done)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  संरचना cf_call_data save;
  संरचना cf_ngtcp2_io_ctx pktx;

  जर(cf->connected) {
    *done = TRUE;
    परत CURLE_OK;
  }

  /* Connect the sub-chain */
  जर(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    जर(result || !*done)
      परत result;
  }

  *done = FALSE;

  जर(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    परत CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  CF_DATA_SAVE(save, cf, data);

  जर(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(data);
    result = cf_connect_start(cf, data, &pktx);
    जर(result)
      जा out;
    जर(cf->connected) {
      *done = TRUE;
      जा out;
    }
    result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
    /* we do not expect to be able to recv anything yet */
    जा out;
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  जर(result)
    जा out;

  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
  जर(result)
    जा out;

  जर(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    result = ctx->tls_vrfy_result;
    जर(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      cf->connected = TRUE;
      *done = TRUE;
    }
  }

out:
  जर(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
  जर(ctx->qconn &&
     ((result == CURLE_RECV_ERROR) || (result == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_in_draining_period(ctx->qconn)) {
    स्थिरांक ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    result = CURLE_COULDNT_CONNECT;
    जर(cerr) {
      CURL_TRC_CF(data, cf, "connect error, type=%d, code=%" PRIu64,
                  (पूर्णांक)cerr->type, cerr->error_code);
      निवड(cerr->type) {
      स्थिती NGTCP2_CCERR_TYPE_VERSION_NEGOTIATION:
        CURL_TRC_CF(data, cf, "error in version negotiation");
        थांबा;
      पूर्वनिर्धारित:
        जर(cerr->error_code >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(data, cf, "crypto error, tls alert=%u",
                      (अचिन्हित पूर्णांक)(cerr->error_code & 0xffU));
        }
        अन्यथा जर(cerr->error_code == NGTCP2_CONNECTION_REFUSED) {
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
  जर(result) {
    जर(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      संरचना ip_quadruple ip;

      जर(!Curl_cf_socket_peek(cf->next, data, रिक्त, रिक्त, &ip))
        infof(data, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    }
  }
#endif
  जर(!result && ctx->qconn) {
    result = Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx);
  }
  जर(result || *done)
    CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (पूर्णांक)result, *done);
  CF_DATA_RESTORE(cf, save);
  परत result;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data, bool *done)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना cf_call_data save;
  संरचना cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  जर(cf->shutdown || !ctx->qconn) {
    *done = TRUE;
    परत CURLE_OK;
  }

  जर(!cf->next) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *done = TRUE;
    परत CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);
  *done = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  जर(!ctx->shutdown_started) {
    अक्षर buffer[NGTCP2_MAX_UDP_PAYLOAD_SIZE];
    ngtcp2_ssize nwritten;

    जर(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
      CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
      result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
      जर(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
        CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
        result = CURLE_OK;
        जा out;
      }
      अन्यथा जर(result) {
        CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                    (पूर्णांक)result);
        *done = TRUE;
        जा out;
      }
    }

    DEBUGASSERT(Curl_bufq_is_empty(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, रिक्त, /* path */
      रिक्त, /* pkt_info */
      (uint8_t *)buffer, आकार(buffer),
      &ctx->last_error, pktx.ts);
    CURL_TRC_CF(data, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (पूर्णांक)ctx->last_error.type,
                ctx->last_error.error_code, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    जर(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      माप_प्रकार n;
      result = Curl_bufq_write(&ctx->q.sendbuf, (स्थिरांक अचिन्हित अक्षर *)buffer,
                               (माप_प्रकार)nwritten, &n);
      जर(result) {
        CURL_TRC_CF(data, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (पूर्णांक)result);
        जा out;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (माप_प्रकार)nwritten;
      ctx->q.split_len = 0;
    }
  }

  जर(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing egress");
    result = Curl_vquic_flush(cf, data, &ctx->q);
    जर(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
      result = CURLE_OK;
      जा out;
    }
    अन्यथा जर(result) {
      CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                  (पूर्णांक)result);
      *done = TRUE;
      जा out;
    }
  }

  जर(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
    result = CURLE_OK;
  }
out:
  CF_DATA_RESTORE(cf, save);
  परत result;
}

शून्य Curl_cf_ngtcp2_cmn_conn_close(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  bool done;
  Curl_cf_ngtcp2_cmn_shutdown(cf, data, &done);
}

स्थिर bool cf_ngtcp2_err_is_fatal(पूर्णांक code)
{
  परत (NGTCP2_ERR_FATAL >= code) ||
         (NGTCP2_ERR_DROP_CONN == code) ||
         (NGTCP2_ERR_IDLE_CLOSE == code);
}

शून्य Curl_cf_ngtcp2_cmn_err_set(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data, पूर्णांक code)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  जर(!ctx->last_error.error_code) {
    जर(NGTCP2_ERR_CRYPTO == code) {
      ngtcp2_ccerr_set_tls_alert(&ctx->last_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 रिक्त, 0);
    }
    अन्यथा {
      ngtcp2_ccerr_set_liberr(&ctx->last_error, code, रिक्त, 0);
    }
  }
  जर(cf_ngtcp2_err_is_fatal(code))
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

#जर NGTCP2_VERSION_अंक < 0x011100
संरचना cf_ngtcp2_sfind_ctx {
  int64_t stream_id;
  संरचना h3_stream_ctx *stream;
  uint32_t mid;
};

स्थिर bool cf_ngtcp2_sfind(uint32_t mid, शून्य *मूल्य, शून्य *user_data)
{
  संरचना cf_ngtcp2_sfind_ctx *fctx = user_data;
  संरचना h3_stream_ctx *stream = मूल्य;

  जर(fctx->stream_id == stream->id) {
    fctx->mid = mid;
    fctx->stream = stream;
    परत FALSE;
  }
  परत TRUE; /* continue */
}

स्थिर संरचना h3_stream_ctx *cf_ngtcp2_get_stream(संरचना cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  संरचना cf_ngtcp2_sfind_ctx fctx;
  fctx.stream_id = stream_id;
  fctx.stream = रिक्त;
  Curl_u32_ptrset_visit(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  परत fctx.stream;
}
#अन्यथा
स्थिर संरचना h3_stream_ctx *cf_ngtcp2_get_stream(संरचना cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  परत ngtcp2_conn_get_stream_user_data(ctx->qconn, stream_id);
}
#endif

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
स्थिर CURLcode read_pkt_to_send(शून्य *userp,
                                 अचिन्हित अक्षर *buf, माप_प्रकार buflen,
                                 माप_प्रकार *pnread)
{
  संरचना cf_ngtcp2_io_ctx *x = userp;
  संरचना cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t stream_id;
  पूर्णांक fin;
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
  साठी(;;) {

    जर(ctx->h3conn && ngtcp2_conn_get_max_data_डावा(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_stream(ctx->h3conn, &stream_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      जर(veccnt < 0) {
        failf(x->data, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((पूर्णांक)veccnt));
        Curl_cf_ngtcp2_h3_err_set(x->cf, x->data, (पूर्णांक)veccnt);
        परत CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_STREAM_FLAG_MORE |
            (fin ? NGTCP2_WRITE_STREAM_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_stream(ctx->qconn, &x->ps.path,
                                  रिक्त, buf, buflen,
                                  &ndatalen, flags, stream_id,
                                  (स्थिरांक ngtcp2_vec *)vec, veccnt, x->ts);
    जर(n == 0) {
      /* nothing to send */
      परत CURLE_AGAIN;
    }
    अन्यथा जर(n < 0) {
      निवड(n) {
      स्थिती NGTCP2_ERR_STREAM_DATA_BLOCKED: {
        संरचना h3_stream_ctx *stream;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_block_stream(ctx->h3conn, stream_id);
        CURL_TRC_CF(x->data, x->cf, "[%" PRId64 "] block quic flow",
                    stream_id);
        stream = cf_ngtcp2_get_stream(ctx, stream_id);
        जर(stream) /* it might be not one of our h3 streams? */
          stream->quic_flow_blocked = TRUE;
        n = 0;
        थांबा;
      }
      स्थिती NGTCP2_ERR_STREAM_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_stream_write(ctx->h3conn, stream_id);
        n = 0;
        थांबा;
      स्थिती NGTCP2_ERR_WRITE_MORE:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        थांबा;
      पूर्वनिर्धारित:
        DEBUGASSERT(ndatalen == -1);
        failf(x->data, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((पूर्णांक)n));
        Curl_cf_ngtcp2_cmn_err_set(x->cf, x->data, (पूर्णांक)n);
        परत CURLE_SEND_ERROR;
      }
    }

    जर(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      पूर्णांक rv = nghttp3_conn_add_write_offset(ctx->h3conn, stream_id, ndatalen);
      जर(rv) {
        failf(x->data, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        परत CURLE_SEND_ERROR;
      }
    }

    जर(n > 0) {
      /* packet assembled, leave */
      *pnread = (माप_प्रकार)n;
      परत CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_progress_egress(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        संरचना cf_ngtcp2_io_ctx *pktx)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  माप_प्रकार nread;
  माप_प्रकार max_payload_size, path_max_payload_size;
  माप_प्रकार pktcnt = 0;
  माप_प्रकार gsolen = 0;  /* this disables gso until we have a clue */
  माप_प्रकार send_quantum;
  CURLcode result;
  संरचना cf_ngtcp2_io_ctx local_pktx;

  जर(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  अन्यथा {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  जर(result) {
    जर(result == CURLE_AGAIN) {
      Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
      परत CURLE_OK;
    }
    परत result;
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
  साठी(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_size,
                            read_pkt_to_send, pktx, &nread);
    जर(result == CURLE_AGAIN)
      थांबा;
    अन्यथा जर(result)
      परत result;
    अन्यथा {
      माप_प्रकार buflen = Curl_bufq_len(&ctx->q.sendbuf);
      जर((buflen >= send_quantum) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_size))
        थांबा;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      जर(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      अन्यथा जर(nread > gsolen ||
              (gsolen > path_max_payload_size && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        result = Curl_vquic_send_tail_split(cf, data, &ctx->q,
                                            gsolen, nread, nread);
        जर(result) {
          जर(result == CURLE_AGAIN) {
            Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
            परत CURLE_OK;
          }
          परत result;
        }
        pktcnt = 0;
      }
      अन्यथा जर(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        थांबा;
      }
    }
  }

  जर(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(data, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
    जर(result) {
      जर(result == CURLE_AGAIN) {
        Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
        परत CURLE_OK;
      }
      परत result;
    }
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_conn_update_pkt_tx_time(ctx->qconn, pktx->ts);
  }
  परत CURLE_OK;
}

संरचना cf_ngtcp2_recv_ctx {
  संरचना cf_ngtcp2_io_ctx *pktx;
  माप_प्रकार pkt_मोजणी;
};

स्थिर CURLcode cf_ngtcp2_recv_pkts(स्थिरांक अचिन्हित अक्षर *buf, माप_प्रकार buflen,
                                    माप_प्रकार gso_size,
                                    संरचना sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    शून्य *userp)
{
  संरचना cf_ngtcp2_recv_ctx *rctx = userp;
  संरचना cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  संरचना cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_path path;
  माप_प्रकार offset, pktlen;
  पूर्णांक rv;

  जर(!rctx->pkt_मोजणी) {
    Curl_cf_ngtcp2_io_ctx_update_time(pktx->data, pktx, pktx->cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  जर(ecn)
    CURL_TRC_CF(pktx->data, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_size, (अचिन्हित)ecn);
  ngtcp2_addr_init(&path.local, (संरचना sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&path.remote, (संरचना sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  साठी(offset = 0; offset < buflen; offset += gso_size) {
    rctx->pkt_मोजणी++;
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &path, &pi,
                              buf + offset, pktlen, pktx->ts);
    जर(rv) {
      CURL_TRC_CF(pktx->data, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_set(pktx->cf, pktx->data, rv);

      जर(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        परत CURLE_PEER_FAILED_VERIFICATION;
      परत CURLE_RECV_ERROR;
    }
  }
  परत CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_progress_ingress(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *data,
                                         संरचना cf_ngtcp2_io_ctx *pktx)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना cf_ngtcp2_io_ctx local_pktx;
  संरचना cf_ngtcp2_recv_ctx rctx;
  CURLcode result = CURLE_OK;

  जर(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }

  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  जर(result)
    परत result;

  rctx.pktx = pktx;
  rctx.pkt_मोजणी = 0;

  जर(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(data, cf, "progress_ingress(socket)");
    परत Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  अन्यथा {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    अचिन्हित अक्षर *buf;
    माप_प्रकार max_udp_payload = QUIC_TUNNEL_INBUF_SIZE;
    माप_प्रकार pkt_limit = QUIC_TUNNEL_INGRESS_PKT_LIMIT;
    माप_प्रकार nread;
    संरचना sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(data, cf, "progress_ingress(sub-filters)");
    जर(ctx->qconn) {
      माप_प्रकार max_path_payload;
      max_path_payload =
        ngtcp2_conn_get_path_max_tx_udp_payload_size(ctx->qconn);
      जर(max_path_payload > max_udp_payload)
        max_udp_payload = max_path_payload;
    }

    जर(ctx->tunnel_inbuf_len < max_udp_payload) {
      अचिन्हित अक्षर *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      जर(!newbuf)
        परत CURLE_OUT_OF_MEMORY;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    पर्यंत(pkt_limit--) {
      result = Curl_conn_cf_recv(cf->next, data, (अक्षर *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      जर(result == CURLE_AGAIN) {
        /* no more data available at the moment */
        परत CURLE_OK;
      }
      जर(result) {
        CURL_TRC_CF(data, cf, "ingress, recv from tunnel failed: %d",
                    (पूर्णांक)result);
        परत result;
      }
      जर(nread == 0) {
        /* tunnel closed */
        परत CURLE_OK;
      }

      स_नकल(&remote_addr, ctx->connected_path.remote.addr,
             ctx->connected_path.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_path.remote.addrlen;
      result = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      जर(result)
        परत result;

      जर(!ctx->q.got_first_byte) {
        ctx->q.got_first_byte = TRUE;
        ctx->q.first_byte_at = ctx->q.last_op;
      }
      ctx->q.last_io = ctx->q.last_op;
    }
    परत CURLE_OK;
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

  जर(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  अन्यथा {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  जर(expiry != UINT64_MAX) {
    जर(expiry <= pktx->ts) {
      CURLcode result;
      पूर्णांक rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      जर(rv) {
        failf(data, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_set(cf, data, rv);
        परत CURLE_SEND_ERROR;
      }
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, pktx);
      जर(!result)
        result = Curl_cf_ngtcp2_progress_egress(cf, data, pktx);
      जर(result) {
        /* a verify failure during ingress must win over generic errors */
        जर(ctx->tls_vrfy_result)
          result = ctx->tls_vrfy_result;
        परत result;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    जर(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      जर(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_set(data, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  परत CURLE_OK;
}

स्थिर शून्य cf_ngtcp2_setup_keep_alive(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  स्थिरांक ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  जर(!ctx->qconn)
    परत;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  जर(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no peer idle timeout, unset keep-alive");
  }
  अन्यथा जर(!Curl_u32_ptrset_मोजणी(&ctx->streams)) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no active streams, unset keep-alive");
  }
  अन्यथा {
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
  जर(!data)
    परत CURLE_FAILED_INIT;

  जर(stream) {
    *pstream = stream;
    परत CURLE_OK;
  }

  stream = curlx_calloc(1, आकार(*stream));
  जर(!stream)
    परत CURLE_OUT_OF_MEMORY;

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
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_ओळ_LEN);

  जर(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    Curl_cf_ngtcp2_h3_stream_ctx_free(stream);
    परत CURLE_OUT_OF_MEMORY;
  }

  जर(Curl_u32_ptrset_मोजणी(&ctx->streams) == 1)
    cf_ngtcp2_setup_keep_alive(cf, data);

  *pstream = stream;
  परत CURLE_OK;
}

शून्य Curl_cf_ngtcp2_h3_stream_close(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    संरचना h3_stream_ctx *stream)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(data);
  DEBUGASSERT(stream);
  जर(!stream->closed && ctx->qconn && ctx->h3conn) {
    CURLcode result;

    nghttp3_conn_set_stream_user_data(ctx->h3conn, stream->id, रिक्त);
    ngtcp2_conn_set_stream_user_data(ctx->qconn, stream->id, रिक्त);
    stream->closed = TRUE;
    (शून्य)ngtcp2_conn_shutdown_stream(ctx->qconn, 0, stream->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    result = Curl_cf_ngtcp2_progress_egress(cf, data, रिक्त);
    जर(result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cancel stream -> %d",
                  stream->id, (पूर्णांक)result);
  }
}

शून्य Curl_cf_ngtcp2_h3_stream_done(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  (शून्य)cf;
  जर(stream) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] easy handle is done", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
    जर(!Curl_u32_ptrset_मोजणी(&ctx->streams))
      cf_ngtcp2_setup_keep_alive(cf, data);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      bool *आदान_pending)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  स्थिरांक ngtcp2_transport_params *rp;
  संरचना cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  *आदान_pending = FALSE;
  जर(!ctx->qconn || ctx->shutdown_started)
    जा out;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  जर(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->q.last_io);
    जर(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      जर((uint64_t)idletime_ms > max_idle_ms)
        जा out;
    }
  }

  जर(!cf->next || !cf->next->cft->is_alive(cf->next, data, आदान_pending))
    जा out;

  alive = TRUE;
  जर(*आदान_pending) {
    CURLcode result;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *आदान_pending = FALSE;
    result = Curl_cf_ngtcp2_progress_ingress(cf, data, रिक्त);
    CURL_TRC_CF(data, cf, "is_alive, progress ingress -> %d", (पूर्णांक)result);
    alive = result ? FALSE : TRUE;
  }

out:
  CF_DATA_RESTORE(cf, save);
  परत alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(संरचना cf_ngtcp2_ctx *ctx,
                                      संरचना Curl_easy *data)
{
  int64_t ctrl_stream_id, qpack_enc_stream_id, qpack_dec_stream_id;
  पूर्णांक rc;

  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &ctrl_stream_id, रिक्त);
  जर(rc) {
    failf(data, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    परत CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_stream(ctx->h3conn, ctrl_stream_id);
  जर(rc) {
    failf(data, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    परत CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_enc_stream_id, रिक्त);
  जर(rc) {
    failf(data, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    परत CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_dec_stream_id, रिक्त);
  जर(rc) {
    failf(data, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    परत CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_stream_id,
                                       qpack_dec_stream_id);
  जर(rc) {
    failf(data, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    परत CURLE_QUIC_CONNECT_ERROR;
  }
  परत CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  पूर्णांक query, पूर्णांक *pres1, शून्य *pres2)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;

  निवड(query) {
  स्थिती CF_QUERY_CONNECT_REPLY_MS:
    जर((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < पूर्णांक_उच्च) ? (पूर्णांक)ms : पूर्णांक_उच्च;
      परत CURLE_OK;
    }
    थांबा;
  स्थिती CF_QUERY_REALLY_CONNECTED:
    जर(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_first_byte;
      परत CURLE_OK;
    }
    थांबा;
  पूर्वनिर्धारित:
    थांबा;
  }
  परत cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  पूर्णांक event, पूर्णांक arg1, शून्य *arg2)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (शून्य)arg1;
  (शून्य)arg2;
  निवड(event) {
  स्थिती CF_CTRL_REPORT_STATS:
    जर(cf->connected && !ctx->stats_reported) {
      जर((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        ctx->stats_reported = TRUE;
      }
      अन्यथा जर(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        जर(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    थांबा;
  पूर्वनिर्धारित:
    थांबा;
  }
  परत result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
