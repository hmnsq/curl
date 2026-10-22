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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#समावेश <ngtcp2/ngtcp2.h>

#अगर_घोषित USE_OPENSSL
#समावेश <openssl/err.h>
#अगर defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#समावेश <ngtcp2/ngtcp2_crypto_boringssl.h>
#या_अगर defined(OPENSSL_QUIC_API2)
#समावेश <ngtcp2/ngtcp2_crypto_ossl.h>
#अन्यथा
#समावेश <ngtcp2/ngtcp2_crypto_quictls.h>
#पूर्ण_अगर
#समावेश "vtls/openssl.h"
#या_अगर defined(USE_GNUTLS)
#समावेश <ngtcp2/ngtcp2_crypto_gnutls.h>
#समावेश "vtls/gtls.h"
#या_अगर defined(USE_WOLFSSL)
#समावेश <ngtcp2/ngtcp2_crypto_wolfssl.h>
#समावेश "vtls/wolfssl.h"
#पूर्ण_अगर

#समावेश <nghttp3/nghttp3.h>

#समावेश "urldata.h"
#समावेश "url.h"
#समावेश "u32_ptrset.h"
#समावेश "curl_trc.h"
#समावेश "rand.h"
#समावेश "multiif.h"
#समावेश "cfilters.h"
#समावेश "cf-socket.h"
#समावेश "connect.h"
#समावेश "progress.h"
#समावेश "curlx/win32-fopen.h"
#समावेश "curlx/dynbuf.h"
#समावेश "http1.h"
#समावेश "select.h"
#समावेश "sockaddr.h"
#समावेश "transfer.h"
#समावेश "bufref.h"
#समावेश "vdns/cf-dns.h"
#समावेश "vquic/vquic.h"
#समावेश "vquic/vquic_int.h"
#समावेश "vquic/vquic-tls.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/vtls_scache.h"
#समावेश "vquic/cf-ngtcp2-cmn.h"

/*
 * Store ngtcp2 version info in this buffer.
 */
व्योम Curl_ngtcp2_ver(अक्षर *p, माप_प्रकार len)
{
  स्थिर ngtcp2_info *ng2 = ngtcp2_version(0);
  स्थिर nghttp3_info *ht3 = nghttp3_version(0);
  (व्योम)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->version_str, ht3->version_str);
}

व्योम Curl_cf_ngtcp2_h3_stream_ctx_free(काष्ठा h3_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

अटल व्योम h3_stream_set_free(uint32_t id, व्योम *stream)
{
  (व्योम)id;
  DEBUGASSERT(stream);
  Curl_cf_ngtcp2_h3_stream_ctx_free((काष्ठा h3_stream_ctx *)stream);
}

अटल bool cf_ngtcp2_h3_err_is_fatal(पूर्णांक code)
{
  वापस (NGHTTP3_ERR_FATAL >= code) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM == code);
}

व्योम Curl_cf_ngtcp2_h3_err_set(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य, पूर्णांक code)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  अगर(!ctx->last_error.error_code) {
    ngtcp2_ccerr_set_application_error(&ctx->last_error,
      nghttp3_err_infer_quic_app_error_code(code), शून्य, 0);
  }
  अगर(cf_ngtcp2_h3_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, तथ्य);
}

CURLcode Curl_cf_ngtcp2_ctx_init(काष्ठा cf_ngtcp2_ctx *ctx,
                                 काष्ठा Curl_peer *origin,
                                 काष्ठा Curl_peer *peer,
                                 काष्ठा ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = शून्य;
  ctx->tunnel_inbuf_len = 0;
  ctx->version = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->stream_bufcp, H3_STREAM_CHUNK_आकार,
                  H3_STREAM_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  वापस Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

व्योम Curl_cf_ngtcp2_ctx_cleanup(काष्ठा cf_ngtcp2_ctx *ctx)
{
  अगर(ctx && ctx->initialized) {
    अगर(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    अगर(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = शून्य;
    }
    अगर(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = शून्य;
    }
#अगर_घोषित OPENSSL_QUIC_API2
    अगर(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = शून्य;
    }
#पूर्ण_अगर
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    curlx_safefree(ctx->tunnel_inbuf);
    ctx->tunnel_inbuf_len = 0;
  }
}

काष्ठा Curl_easy *Curl_cf_ngtcp2_get_xfer(काष्ठा Curl_cfilter *cf,
                                          व्योम *stream_user_तथ्य,
                                          काष्ठा h3_stream_ctx **pstream)
{
  काष्ठा h3_stream_ctx *stream = stream_user_तथ्य;

  अगर(pstream)
    *pstream = stream;
  अगर(stream && (stream->mid != UINT32_MAX)) {
    काष्ठा Curl_easy *तथ्य, *call_तथ्य = CF_तथ्य_CURRENT(cf);
    अगर(!call_तथ्य || !call_तथ्य->multi) {
      DEBUGASSERT(0);
      वापस शून्य;
    }
    तथ्य = Curl_multi_get_easy(call_तथ्य->multi, stream->mid);
    अगर(तथ्य && (तथ्य->id == stream->xfer_id))
      वापस तथ्य;
    DEBUGASSERT(0);
  }
  वापस शून्य;
}

अटल ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  काष्ठा Curl_cfilter *cf = conn_ref->user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  वापस ctx->qconn;
}

#अगर_घोषित DEBUG_NGTCP2
अटल व्योम quic_printf(व्योम *user_तथ्य, स्थिर अक्षर *fmt, ...)
{
  बहु_सूची ap;
  (व्योम)user_तथ्य;
  बहु_शुरू(ap, fmt);
  curl_mvfprintf(मानक_त्रुटि, fmt, ap);
  बहु_पूर्ण(ap);
  curl_mfprintf(मानक_त्रुटि, "\n");
}
#पूर्ण_अगर

अटल व्योम qlog_callback(व्योम *user_तथ्य, uint32_t flags,
                          स्थिर व्योम *तथ्य, माप_प्रकार datalen)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  (व्योम)flags;
  अगर(ctx->qlogfd != -1) {
    ssize_t rc = ग_लिखो(ctx->qlogfd, तथ्य, datalen);
    अगर(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

अटल व्योम quic_settings(काष्ठा cf_ngtcp2_ctx *ctx,
                          काष्ठा Curl_easy *तथ्य,
                          काष्ठा cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#अगर_घोषित DEBUG_NGTCP2
  s->log_printf = quic_printf;
#अन्यथा
  s->log_printf = शून्य;
#पूर्ण_अगर

  s->initial_ts = pktx->ts;
  s->handshake_timeout = (तथ्य->set.connecttimeout > 0) ?
    तथ्य->set.connecttimeout * NGTCP2_MILLISECONDS : QUIC_HANDSHAKE_TIMEOUT;
  s->max_window = H3_CONN_WINDOW_आकार_MAX;
  s->max_stream_window = 0; /* disable ngtcp2 auto-tuning of window */
  s->no_pmtud = FALSE;
#अगर_घोषित NGTCP2_SETTINGS_V3
  /* try ten times the ngtcp2 defaults here for problems with Caddy */
  s->glitch_ratelim_burst = 1000 * 10;
  s->glitch_ratelim_rate = 33 * 10;
#पूर्ण_अगर
  t->initial_max_तथ्य = s->max_window;
  t->initial_max_stream_तथ्य_bidi_local = H3_STREAM_WINDOW_आकार_INITIAL;
  t->initial_max_stream_तथ्य_bidi_remote = H3_STREAM_WINDOW_आकार_INITIAL;
  t->initial_max_stream_तथ्य_uni = t->initial_max_तथ्य;
  t->initial_max_streams_bidi = QUIC_MAX_STREAMS;
  t->initial_max_streams_uni = QUIC_MAX_STREAMS;
  t->max_idle_timeout = 0; /* no idle timeout from our side */
  अगर(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#अगर defined(_MSC_VER) && defined(_DLL)
#आशय warning(धकेलो)
#आशय warning(disable:4232) /* MSVC extension, dllimport identity */
#पूर्ण_अगर

अटल पूर्णांक cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, व्योम *user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf ? cf->ctx : शून्य;
  काष्ठा Curl_easy *तथ्य;

  (व्योम)tconn;
  DEBUGASSERT(ctx);
  तथ्य = CF_तथ्य_CURRENT(cf);
  DEBUGASSERT(तथ्य);
  अगर(!ctx || !तथ्य)
    वापस NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(तथ्य);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, तथ्य);

  ctx->tls_vrfy_परिणाम = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    तथ्य, &ctx->ssl_peer);
  अगर(ctx->tls_vrfy_परिणाम)
    वापस NGTCP2_ERR_CALLBACK_FAILURE;

#अगर_घोषित CURLVERBOSE
  अगर(Curl_trc_is_verbose(तथ्य)) {
    स्थिर ngtcp2_transport_params *rp;
    rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
    CURL_TRC_CF(तथ्य, cf, "handshake complete after %" FMT_TIMEDIFF_T
                "ms, remote transport[max_udp_payload=%" PRIu64
                ", initial_max_data=%" PRIu64 "]",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at),
                rp->max_udp_payload_आकार, rp->initial_max_तथ्य);
  }
#पूर्ण_अगर

  /* In case of earlydata, where we simulate being connected, update
   * the handshake time when we really did connect */
  अगर(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrsTimeWas(तथ्य, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  अगर(ctx->use_earlydata) {
#अगर defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    अगर(SSL_get_early_तथ्य_status(ctx->tls.ossl.ssl) == SSL_EARLY_तथ्य_NOT_SENT)
      CURL_TRC_CF(तथ्य, cf, "OpenSSL did not send early data");
#पूर्ण_अगर

#अगर NGTCP2_VERSION_अंक >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_तथ्य_rejected2(ctx->qconn);
#अन्यथा /* older NGTCP2 */
    {
#अगर defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      पूर्णांक ossl_early_status = SSL_get_early_तथ्य_status(ctx->tls.ossl.ssl);
      अगर(ossl_early_status == SSL_EARLY_तथ्य_NOT_SENT)
        CURL_TRC_CF(तथ्य, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_status == SSL_EARLY_तथ्य_ACCEPTED);
#या_अगर defined(USE_GNUTLS)
      पूर्णांक flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_तथ्य);
#या_अगर defined(USE_WOLFSSL)
#अगर_घोषित WOLFSSL_EARLY_तथ्य
      ctx->earlydata_accepted =
        (wolf_SSL_get_early_तथ्य_status(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_तथ्य_REJECTED);
#अन्यथा
      DEBUGASSERT(0); /* should not come here if ED is disabled. */
      ctx->earlydata_accepted = FALSE;
#पूर्ण_अगर /* WOLFSSL_EARLY_DATA */
#पूर्ण_अगर /* OPENSSL or GNUTLS or WOLFSSL */
    }
#पूर्ण_अगर /* older NGTCP2 */
    CURL_TRC_CF(तथ्य, cf, "server did%s accept %zu bytes of early data",
                ctx->earlydata_accepted ? "" : " not", ctx->earlydata_skip);
    Curl_pgrs_Early_तथ्य(तथ्य, ctx->earlydata_accepted ?
                              (curl_off_t)ctx->earlydata_skip :
                             -(curl_off_t)ctx->earlydata_skip);
  }
  वापस 0;
}

अटल पूर्णांक cb_recv_stream_तथ्य(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t stream_id, uint64_t offset,
                               स्थिर uint8_t *buf, माप_प्रकार buflen,
                               व्योम *user_तथ्य, व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  पूर्णांक fin = (flags & NGTCP2_STREAM_तथ्य_FLAG_FIN) ? 1 : 0;
  काष्ठा h3_stream_ctx *stream;
  काष्ठा Curl_easy *तथ्य =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_तथ्य, &stream);

  (व्योम)offset;
  rc = nghttp3_conn_read_stream(ctx->h3conn, stream_id, buf, buflen, fin);
  अगर(rc < 0) {
    अगर(तथ्य && stream) {
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  stream_id, stream->reset, stream->closed);
    }
    वापस NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  अगर(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_stream_offset(tconn, stream_id, nconsumed);
    ngtcp2_conn_extend_max_offset(tconn, nconsumed);
    अगर(stream) {
      stream->rx_offset += nconsumed;
      stream->rx_offset_max += nconsumed;
    }
  }
  वापस 0;
}

अटल पूर्णांक cb_acked_stream_तथ्य_offset(ngtcp2_conn *tconn, int64_t stream_id,
                                       uint64_t offset, uint64_t datalen,
                                       व्योम *user_तथ्य, व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णांक rv;
  (व्योम)stream_id;
  (व्योम)tconn;
  (व्योम)offset;
  (व्योम)datalen;
  (व्योम)stream_user_तथ्य;

  rv = nghttp3_conn_add_ack_offset(ctx->h3conn, stream_id, datalen);
  अगर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    वापस NGTCP2_ERR_CALLBACK_FAILURE;
  }

  वापस 0;
}

अटल पूर्णांक cb_stream_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id, uint64_t app_error_code,
                           व्योम *user_तथ्य, व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = Curl_cf_ngtcp2_get_xfer(cf, stream_user_तथ्य, शून्य);
  पूर्णांक rv;

  (व्योम)tconn;
  /* stream is closed... */
  अगर(!तथ्य)
    तथ्य = CF_तथ्य_CURRENT(cf);
  अगर(!तथ्य)
    वापस NGTCP2_ERR_CALLBACK_FAILURE;

  अगर(!(flags & NGTCP2_STREAM_CLOSE_FLAG_APP_ERROR_CODE_SET)) {
    app_error_code = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, app_error_code);
  CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, app_error_code, rv);
  अगर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, तथ्य, rv);
    वापस NGTCP2_ERR_CALLBACK_FAILURE;
  }

  वापस 0;
}

#अगर_घोषित NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
अटल पूर्णांक cb_stream_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id,
                           uint64_t rx_app_error_code,
                           uint64_t tx_app_error_code,
                           व्योम *user_तथ्य, व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = Curl_cf_ngtcp2_get_xfer(cf, stream_user_तथ्य, शून्य);
  uint64_t h3_app_error_code = NGHTTP3_H3_NO_ERROR;
  पूर्णांक rv;

  (व्योम)tconn;
  (व्योम)tx_app_error_code;
  /* stream is closed... */
  अगर(!तथ्य)
    तथ्य = CF_तथ्य_CURRENT(cf);
  अगर(!तथ्य)
    वापस NGTCP2_ERR_CALLBACK_FAILURE;

  अगर(flags & NGTCP2_STREAM_CLOSE2_FLAG_RX_APP_ERROR_CODE_SET)
    h3_app_error_code = rx_app_error_code;

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, h3_app_error_code);
  CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, h3_app_error_code, rv);
  अगर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, तथ्य, rv);
    वापस NGTCP2_ERR_CALLBACK_FAILURE;
  }

  वापस 0;
}
#पूर्ण_अगर

अटल पूर्णांक cb_stream_reset(ngtcp2_conn *tconn, int64_t stream_id,
                           uint64_t final_आकार, uint64_t app_error_code,
                           व्योम *user_तथ्य, व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = Curl_cf_ngtcp2_get_xfer(cf, stream_user_तथ्य, शून्य);
  पूर्णांक rv;
  (व्योम)tconn;
  (व्योम)final_आकार;
  (व्योम)app_error_code;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  अगर(तथ्य)
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  अगर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    वापस NGTCP2_ERR_CALLBACK_FAILURE;
  }

  वापस 0;
}

अटल पूर्णांक cb_stream_stop_sending(ngtcp2_conn *tconn, int64_t stream_id,
                                  uint64_t app_error_code, व्योम *user_तथ्य,
                                  व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णांक rv;
  (व्योम)tconn;
  (व्योम)app_error_code;
  (व्योम)stream_user_तथ्य;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  अगर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    वापस NGTCP2_ERR_CALLBACK_FAILURE;
  }

  वापस 0;
}

अटल पूर्णांक cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            व्योम *user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);

  (व्योम)tconn;
  ctx->max_bidi_streams = max_streams;
  अगर(तथ्य)
    CURL_TRC_CF(तथ्य, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->used_bidi_streams);
  वापस 0;
}

अटल पूर्णांक cb_extend_max_stream_तथ्य(ngtcp2_conn *tconn, int64_t stream_id,
                                     uint64_t max_तथ्य, व्योम *user_तथ्य,
                                     व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream;
  काष्ठा Curl_easy *तथ्य =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_तथ्य, &stream);
  पूर्णांक rv;
  (व्योम)tconn;
  (व्योम)max_तथ्य;

  rv = nghttp3_conn_unblock_stream(ctx->h3conn, stream_id);
  अगर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    वापस NGTCP2_ERR_CALLBACK_FAILURE;
  }
  अगर(stream && stream->quic_flow_blocked) {
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] unblock quic flow", stream_id);
    stream->quic_flow_blocked = FALSE;
    अगर(तथ्य)
      Curl_multi_mark_dirty(तथ्य);
  }
  वापस 0;
}

अटल व्योम cb_rand(uint8_t *dest, माप_प्रकार destlen,
                    स्थिर ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode परिणाम;
  (व्योम)rand_ctx;

  परिणाम = Curl_rand(शून्य, dest, destlen);
  अगर(परिणाम) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    स_रखो(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
अटल पूर्णांक cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, माप_प्रकार cidlen,
                                    व्योम *user_तथ्य)
{
  CURLcode परिणाम;
  (व्योम)tconn;
  (व्योम)user_तथ्य;

  परिणाम = Curl_rand(शून्य, cid->तथ्य, cidlen);
  अगर(परिणाम)
    वापस NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  परिणाम = Curl_rand(शून्य, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  अगर(परिणाम)
    वापस NGTCP2_ERR_CALLBACK_FAILURE;

  वापस 0;
}

#अगर_घोषित NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
अटल पूर्णांक cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  काष्ठा ngtcp2_stateless_reset_token *token, माप_प्रकार cidlen, व्योम *user_तथ्य)
{
  CURLcode परिणाम;
  (व्योम)tconn;
  (व्योम)user_तथ्य;

  परिणाम = Curl_rand(शून्य, cid->तथ्य, cidlen);
  अगर(परिणाम)
    वापस NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  परिणाम = Curl_rand(शून्य, token->तथ्य, माप(token->तथ्य));
  अगर(परिणाम)
    वापस NGTCP2_ERR_CALLBACK_FAILURE;

  वापस 0;
}
#पूर्ण_अगर

अटल पूर्णांक cb_recv_rx_कुंजी(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          व्योम *user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf ? cf->ctx : शून्य;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  (व्योम)tconn;

  अगर(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    वापस 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(तथ्य);
  अगर(ctx && तथ्य && !ctx->h3conn && ctx->init_h3_conn_cb) {
    अगर(ctx->init_h3_conn_cb(cf, तथ्य, ctx))
      वापस NGTCP2_ERR_CALLBACK_FAILURE;
  }
  वापस 0;
}

#अगर_घोषित CURL_HAVE_DIAG
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wmissing-field-initializers"
#पूर्ण_अगर
अटल ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  शून्य, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_तथ्य_cb,
  cb_ngtcp2_handshake_completed,
  शून्य, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_mask_cb,
  cb_recv_stream_तथ्य,
  cb_acked_stream_तथ्य_offset,
  शून्य, /* stream_open */
  cb_stream_close,
  शून्य, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  शून्य, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  शून्य, /* remove_connection_id */
  ngtcp2_crypto_update_कुंजी_cb, /* update_key */
  शून्य, /* path_validation */
  शून्य, /* select_preferred_addr */
  cb_stream_reset,
  शून्य, /* extend_max_remote_streams_bidi */
  शून्य, /* extend_max_remote_streams_uni */
  cb_extend_max_stream_तथ्य,
  शून्य, /* dcid_status */
  शून्य, /* handshake_confirmed */
  शून्य, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  शून्य, /* recv_datagram */
  शून्य, /* ack_datagram */
  शून्य, /* lost_datagram */
  ngtcp2_crypto_get_path_challenge_तथ्य_cb,
  cb_stream_stop_sending,
  शून्य, /* version_negotiation */
  cb_recv_rx_कुंजी,
  शून्य, /* recv_tx_key */
  शून्य, /* early_data_rejected */
#अगर_घोषित NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  शून्य, /* begin_path_validation */
#पूर्ण_अगर
#अगर_घोषित NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  शून्य, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  शून्य, /* dcid_status2 */
  ngtcp2_crypto_get_path_challenge_data2_cb, /* get_path_challenge_data2 */
#पूर्ण_अगर
#अगर_घोषित NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  शून्य, /* recv_stop_sending */
#पूर्ण_अगर
#अगर_घोषित NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_stream_close2, /* is called instead of cb_stream_close when set */
#पूर्ण_अगर
};
#अगर_घोषित CURL_HAVE_DIAG
#आशय GCC diagnostic निकालो
#पूर्ण_अगर

#अगर defined(_MSC_VER) && defined(_DLL)
#आशय warning(निकालो)
#पूर्ण_अगर

#अगर_घोषित USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
अटल पूर्णांक quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  काष्ठा Curl_cfilter *cf;
  काष्ठा cf_ngtcp2_ctx *ctx;
  काष्ठा Curl_easy *तथ्य;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_तथ्य(ssl);
  cf = cref ? cref->user_तथ्य : शून्य;
  ctx = cf ? cf->ctx : शून्य;
  तथ्य = cf ? CF_तथ्य_CURRENT(cf) : शून्य;
  अगर(cf && तथ्य && ctx) {
    अचिन्हित अक्षर *quic_tp = शून्य;
    माप_प्रकार quic_tp_len = 0;
#अगर_घोषित HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     माप(tpbuf));
    अगर(tplen < 0)
      CURL_TRC_CF(तथ्य, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((पूर्णांक)tplen));
    अन्यथा {
      quic_tp = (अचिन्हित अक्षर *)tpbuf;
      quic_tp_len = (माप_प्रकार)tplen;
    }
#पूर्ण_अगर
    Curl_ossl_add_session(cf, तथ्य, &ctx->tls.ossl, ctx->ssl_peer.scache_कुंजी,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, शून्य);
  }
  वापस 0;
}
#पूर्ण_अगर /* USE_OPENSSL */

#अगर_घोषित USE_GNUTLS

#अगर_घोषित CURLVERBOSE
अटल स्थिर अक्षर *gtls_hs_संदेश_name(पूर्णांक mtype)
{
  चयन(mtype) {
  हाल 1:
    वापस "ClientHello";
  हाल 2:
    वापस "ServerHello";
  हाल 4:
    वापस "SessionTicket";
  हाल 8:
    वापस "EncryptedExtensions";
  हाल 11:
    वापस "Certificate";
  हाल 13:
    वापस "CertificateRequest";
  हाल 15:
    वापस "CertificateVerify";
  हाल 20:
    वापस "Finished";
  हाल 24:
    वापस "KeyUpdate";
  हाल 254:
    वापस "MessageHash";
  }
  वापस "Unknown";
}
#पूर्ण_अगर

अटल पूर्णांक quic_gtls_handshake_cb(gnutls_session_t session, अचिन्हित पूर्णांक htype,
                                  अचिन्हित when, अचिन्हित पूर्णांक incoming,
                                  स्थिर gnutls_datum_t *संदेश)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  काष्ठा Curl_cfilter *cf = conn_ref ? conn_ref->user_तथ्य : शून्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf ? cf->ctx : शून्य;

  (व्योम)संदेश;
  (व्योम)incoming;
  अगर(when && cf && ctx) { /* after message has been processed */
    काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
    DEBUGASSERT(तथ्य);
    अगर(!तथ्य)
      वापस 0;
    CURL_TRC_CF(तथ्य, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_संदेश_name(htype), htype);
    चयन(htype) {
    हाल GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      अचिन्हित अक्षर *quic_tp = शून्य;
      माप_प्रकार quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       माप(tpbuf));
      अगर(tplen < 0)
        CURL_TRC_CF(तथ्य, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((पूर्णांक)tplen));
      अन्यथा {
        quic_tp = (अचिन्हित अक्षर *)tpbuf;
        quic_tp_len = (माप_प्रकार)tplen;
      }
      (व्योम)Curl_gtls_cache_session(cf, तथ्य, ctx->ssl_peer.scache_कुंजी,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    शून्य);
      अवरोध;
    }
    शेष:
      अवरोध;
    }
  }
  वापस 0;
}
#पूर्ण_अगर /* USE_GNUTLS */

#अगर_घोषित USE_WOLFSSL
अटल पूर्णांक wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolf_SSL_get_app_तथ्य(ssl);
  काष्ठा Curl_cfilter *cf = conn_ref ? conn_ref->user_तथ्य : शून्य;

  DEBUGASSERT(cf);
  अगर(cf && session) {
    काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
    काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
    DEBUGASSERT(तथ्य);
    अगर(तथ्य && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      अचिन्हित अक्षर *quic_tp = शून्य;
      माप_प्रकार quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       माप(tpbuf));
      अगर(tplen < 0)
        CURL_TRC_CF(तथ्य, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((पूर्णांक)tplen));
      अन्यथा {
        quic_tp = (अचिन्हित अक्षर *)tpbuf;
        quic_tp_len = (माप_प्रकार)tplen;
      }
      (व्योम)Curl_wssl_cache_session(cf, तथ्य, ctx->ssl_peer.scache_कुंजी,
                                    session, wolfSSL_version(ssl),
                                    "h3", quic_tp, quic_tp_len, शून्य);
    }
  }
  वापस 0;
}
#पूर्ण_अगर /* USE_WOLFSSL */

अटल CURLcode cf_ngtcp2_tls_ctx_setup(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        व्योम *user_तथ्य)
{
  काष्ठा curl_tls_ctx *ctx = user_तथ्य;

#अगर_घोषित USE_OPENSSL
#अगर defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  अगर(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(तथ्य, "ngtcp2_crypto_boringssl_configure_client_context failed");
    वापस CURLE_FAILED_INIT;
  }
#या_अगर defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#अन्यथा
  अगर(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(तथ्य, "ngtcp2_crypto_quictls_configure_client_context failed");
    वापस CURLE_FAILED_INIT;
  }
#पूर्ण_अगर /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  अगर(Curl_ssl_scache_use(cf, तथ्य)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_set_session_cache_mode(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#या_अगर defined(USE_GNUTLS)
  अगर(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(तथ्य, "ngtcp2_crypto_gnutls_configure_client_session failed");
    वापस CURLE_FAILED_INIT;
  }
  अगर(Curl_ssl_scache_use(cf, तथ्य)) {
    gnutls_handshake_set_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#या_अगर defined(USE_WOLFSSL)
  अगर(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(तथ्य, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    वापस CURLE_FAILED_INIT;
  }
  अगर(Curl_ssl_scache_use(cf, तथ्य)) {
    /* Register to get notified when a new session is received */
    wolfSSL_CTX_sess_set_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#पूर्ण_अगर
  वापस CURLE_OK;
}

अटल CURLcode cf_ngtcp2_on_session_reuse(काष्ठा Curl_cfilter *cf,
                                           काष्ठा Curl_easy *तथ्य,
                                           काष्ठा alpn_spec *alpns,
                                           काष्ठा Curl_ssl_session *scs,
                                           bool *do_early_तथ्य)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  *do_early_तथ्य = FALSE;
#अगर defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#पूर्ण_अगर
#अगर_घोषित USE_GNUTLS
  ctx->earlydata_max =
    gnutls_record_get_max_early_तथ्य_आकार(ctx->tls.gtls.session);
#पूर्ण_अगर
#अगर_घोषित USE_WOLFSSL
#अगर_घोषित WOLFSSL_EARLY_तथ्य
  ctx->earlydata_max = scs->earlydata_max;
#अन्यथा
  ctx->earlydata_max = 0;
#पूर्ण_अगर /* WOLFSSL_EARLY_DATA */
#पूर्ण_अगर
#अगर defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  अगर(!ctx->earlydata_max) {
    CURL_TRC_CF(तथ्य, cf, "SSL session does not allow earlydata");
  }
  अन्यथा अगर(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(तथ्य, cf, "SSL session from different ALPN, no early data");
  }
  अन्यथा अगर(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(तथ्य, cf, "no 0RTT transport parameters, no early data");
  }
  अन्यथा {
    पूर्णांक rv;
    rv = ngtcp2_conn_decode_and_set_0rtt_transport_params(
      ctx->qconn, (स्थिर uint8_t *)scs->quic_tp, scs->quic_tp_len);
    अगर(rv)
      CURL_TRC_CF(तथ्य, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    अन्यथा अगर(ctx->init_h3_conn_cb) {
      infof(तथ्य, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      परिणाम = ctx->init_h3_conn_cb(cf, तथ्य, ctx);
      अगर(!परिणाम) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_तथ्य = TRUE;
      }
    }
    अन्यथा { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_तथ्य = TRUE;
    }
  }
#अन्यथा /* not supported in the TLS backend */
  (व्योम)तथ्य;
  (व्योम)ctx;
  (व्योम)scs;
  (व्योम)alpns;
#पूर्ण_अगर
  वापस परिणाम;
}

/*
 * Might be called twice for happy eyeballs.
 */
अटल CURLcode cf_connect_start(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा cf_ngtcp2_io_ctx *pktx)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णांक rc;
  पूर्णांक rv;
  CURLcode परिणाम;
  स्थिर काष्ठा Curl_sockaddr_ex *sockaddr = शून्य;
  पूर्णांक qfd;
  अटल स्थिर काष्ठा alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  परिणाम = Curl_rand(तथ्य, ctx->dcid.तथ्य, NGTCP2_MAX_CIDLEN);
  अगर(परिणाम)
    वापस परिणाम;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  परिणाम = Curl_rand(तथ्य, ctx->scid.तथ्य, NGTCP2_MAX_CIDLEN);
  अगर(परिणाम)
    वापस परिणाम;

  (व्योम)Curl_qlogdir(तथ्य, ctx->scid.तथ्य, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, तथ्य, pktx);

  परिणाम = Curl_vquic_ctx_init(तथ्य, &ctx->q);
  अगर(परिणाम)
    वापस परिणाम;

  /* Query socket and remote address from sub-chain */
  अगर(Curl_cf_socket_peek(cf->अग्र, तथ्य, &ctx->q.sockfd, &sockaddr, शून्य)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  अगर(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = माप(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (काष्ठा sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    अगर(rv == -1)
      वापस CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (काष्ठा sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_path.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  अन्यथा {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    स्थिर काष्ठा Curl_sockaddr_ex *remote = शून्य;
    अगर(cf->अग्र->cft->query(cf->अग्र, तथ्य, CF_QUERY_REMOTE_ADDR, शून्य,
                            CURL_UNCONST(&remote)))
      वापस CURLE_QUIC_CONNECT_ERROR;
    अगर(!remote)
      वापस CURLE_QUIC_CONNECT_ERROR;

    स_रखो(&ctx->q.local_addr, 0, माप(ctx->q.local_addr));
    चयन(remote->family) {
    हाल AF_INET:
      ((काष्ठा sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = माप(काष्ठा sockaddr_in);
      अवरोध;
#अगर_घोषित USE_IPV6
    हाल AF_INET6:
      ((काष्ठा sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = माप(काष्ठा sockaddr_in6);
      अवरोध;
#पूर्ण_अगर
    शेष:
      वापस CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (काष्ठा sockaddr *)&ctx->q.local_addr,
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
  अगर(rc)
    वापस CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_तथ्य = cf;

  परिणाम = Curl_vquic_tls_init(&ctx->tls, cf, तथ्य,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_setup, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  अगर(परिणाम)
    वापस परिणाम;

#अगर defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  अगर(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(तथ्य, "ngtcp2_crypto_ossl_ctx_new failed");
    वापस CURLE_FAILED_INIT;
  }
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  अगर(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(तथ्य, "ngtcp2_crypto_ossl_configure_client_session failed");
    वापस CURLE_FAILED_INIT;
  }
#या_अगर defined(USE_OPENSSL)
  SSL_set_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#या_अगर defined(USE_GNUTLS)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#या_अगर defined(USE_WOLFSSL)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#अन्यथा
#त्रुटि "ngtcp2 TLS backend not defined"
#पूर्ण_अगर

#अगर defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  अगर(ctx->use_earlydata &&
     !SSL_set_quic_tls_early_तथ्य_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(तथ्य, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#पूर्ण_अगर
  ngtcp2_ccerr_default(&ctx->last_error);

  वापस CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    bool *done)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा cf_call_तथ्य save;
  काष्ठा cf_ngtcp2_io_ctx pktx;

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  /* Connect the sub-chain */
  अगर(cf->अग्र && !cf->अग्र->connected) {
    परिणाम = Curl_conn_cf_connect(cf->अग्र, तथ्य, done);
    अगर(परिणाम || !*done)
      वापस परिणाम;
  }

  *done = FALSE;

  अगर(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(तथ्य, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(तथ्य, cf, "need HTTPS-RR, delaying connect");
    वापस CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, तथ्य);
  CF_तथ्य_SAVE(save, cf, तथ्य);

  अगर(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(तथ्य);
    परिणाम = cf_connect_start(cf, तथ्य, &pktx);
    अगर(परिणाम)
      जाओ out;
    अगर(cf->connected) {
      *done = TRUE;
      जाओ out;
    }
    परिणाम = Curl_cf_ngtcp2_progress_egress(cf, तथ्य, &pktx);
    /* we do not expect to be able to recv anything yet */
    जाओ out;
  }

  परिणाम = Curl_cf_ngtcp2_progress_ingress(cf, तथ्य, &pktx);
  अगर(परिणाम)
    जाओ out;

  परिणाम = Curl_cf_ngtcp2_progress_egress(cf, तथ्य, &pktx);
  अगर(परिणाम)
    जाओ out;

  अगर(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    परिणाम = ctx->tls_vrfy_परिणाम;
    अगर(!परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "peer verified");
      cf->connected = TRUE;
      *done = TRUE;
    }
  }

out:
  अगर(ctx->tls_vrfy_परिणाम)
    परिणाम = ctx->tls_vrfy_परिणाम;
  अगर(ctx->qconn &&
     ((परिणाम == CURLE_RECV_ERROR) || (परिणाम == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_in_draining_period(ctx->qconn)) {
    स्थिर ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    परिणाम = CURLE_COULDNT_CONNECT;
    अगर(cerr) {
      CURL_TRC_CF(तथ्य, cf, "connect error, type=%d, code=%" PRIu64,
                  (पूर्णांक)cerr->type, cerr->error_code);
      चयन(cerr->type) {
      हाल NGTCP2_CCERR_TYPE_VERSION_NEGOTIATION:
        CURL_TRC_CF(तथ्य, cf, "error in version negotiation");
        अवरोध;
      शेष:
        अगर(cerr->error_code >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(तथ्य, cf, "crypto error, tls alert=%u",
                      (अचिन्हित पूर्णांक)(cerr->error_code & 0xffU));
        }
        अन्यथा अगर(cerr->error_code == NGTCP2_CONNECTION_REFUSED) {
          CURL_TRC_CF(तथ्य, cf, "connection refused by server");
          /* When a QUIC server instance is shutting down, it may send us a
           * CONNECTION_CLOSE with this code right away. We want
           * to keep on trying in this case. */
          परिणाम = CURLE_WEIRD_SERVER_REPLY;
        }
      }
    }
  }

#अगर_घोषित CURLVERBOSE
  अगर(परिणाम) {
    अगर(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      काष्ठा ip_quadruple ip;

      अगर(!Curl_cf_socket_peek(cf->अग्र, तथ्य, शून्य, शून्य, &ip))
        infof(तथ्य, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_port, curl_easy_strerror(परिणाम));
    }
  }
#पूर्ण_अगर
  अगर(!परिणाम && ctx->qconn) {
    परिणाम = Curl_cf_ngtcp2_cmn_set_expiry(cf, तथ्य, &pktx);
  }
  अगर(परिणाम || *done)
    CURL_TRC_CF(तथ्य, cf, "connect -> %d, done=%d", (पूर्णांक)परिणाम, *done);
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  काष्ठा cf_ngtcp2_io_ctx pktx;
  CURLcode परिणाम = CURLE_OK;

  अगर(cf->shutdown || !ctx->qconn) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  अगर(!cf->अग्र) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *done = TRUE;
    वापस CURLE_OK;
  }

  CF_तथ्य_SAVE(save, cf, तथ्य);
  *done = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, तथ्य);

  अगर(!ctx->shutdown_started) {
    अक्षर बफर[NGTCP2_MAX_UDP_PAYLOAD_आकार];
    ngtcp2_ssize nwritten;

    अगर(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
      CURL_TRC_CF(तथ्य, cf, "shutdown, flushing sendbuf");
      परिणाम = Curl_cf_ngtcp2_progress_egress(cf, तथ्य, &pktx);
      अगर(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
        CURL_TRC_CF(तथ्य, cf, "sending shutdown packets blocked");
        परिणाम = CURLE_OK;
        जाओ out;
      }
      अन्यथा अगर(परिणाम) {
        CURL_TRC_CF(तथ्य, cf, "shutdown, error %d flushing sendbuf",
                    (पूर्णांक)परिणाम);
        *done = TRUE;
        जाओ out;
      }
    }

    DEBUGASSERT(Curl_bufq_is_empty(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, शून्य, /* path */
      शून्य, /* pkt_info */
      (uint8_t *)बफर, माप(बफर),
      &ctx->last_error, pktx.ts);
    CURL_TRC_CF(तथ्य, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (पूर्णांक)ctx->last_error.type,
                ctx->last_error.error_code, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    अगर(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      माप_प्रकार n;
      परिणाम = Curl_bufq_write(&ctx->q.sendbuf, (स्थिर अचिन्हित अक्षर *)बफर,
                               (माप_प्रकार)nwritten, &n);
      अगर(परिणाम) {
        CURL_TRC_CF(तथ्य, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (पूर्णांक)परिणाम);
        जाओ out;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (माप_प्रकार)nwritten;
      ctx->q.split_len = 0;
    }
  }

  अगर(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(तथ्य, cf, "shutdown, flushing egress");
    परिणाम = Curl_vquic_flush(cf, तथ्य, &ctx->q);
    अगर(परिणाम == CURLE_AGAIN) {
      CURL_TRC_CF(तथ्य, cf, "sending shutdown packets blocked");
      परिणाम = CURLE_OK;
      जाओ out;
    }
    अन्यथा अगर(परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "shutdown, error %d flushing sendbuf",
                  (पूर्णांक)परिणाम);
      *done = TRUE;
      जाओ out;
    }
  }

  अगर(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(तथ्य, cf, "shutdown completely sent off, done");
    *done = TRUE;
    परिणाम = CURLE_OK;
  }
out:
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

व्योम Curl_cf_ngtcp2_cmn_conn_close(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  bool done;
  Curl_cf_ngtcp2_cmn_shutdown(cf, तथ्य, &done);
}

अटल bool cf_ngtcp2_err_is_fatal(पूर्णांक code)
{
  वापस (NGTCP2_ERR_FATAL >= code) ||
         (NGTCP2_ERR_DROP_CONN == code) ||
         (NGTCP2_ERR_IDLE_CLOSE == code);
}

व्योम Curl_cf_ngtcp2_cmn_err_set(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य, पूर्णांक code)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  अगर(!ctx->last_error.error_code) {
    अगर(NGTCP2_ERR_CRYPTO == code) {
      ngtcp2_ccerr_set_tls_alert(&ctx->last_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 शून्य, 0);
    }
    अन्यथा {
      ngtcp2_ccerr_set_liberr(&ctx->last_error, code, शून्य, 0);
    }
  }
  अगर(cf_ngtcp2_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, तथ्य);
}

व्योम Curl_cf_ngtcp2_io_ctx_init(काष्ठा cf_ngtcp2_io_ctx *io_ctx,
                                काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->तथ्य = तथ्य;
  io_ctx->now = *Curl_pgrs_now(तथ्य);
  ngtcp2_path_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_set_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

व्योम Curl_cf_ngtcp2_io_ctx_update_time(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा cf_ngtcp2_io_ctx *io_ctx,
                                       काष्ठा Curl_cfilter *cf)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(तथ्य);
  Curl_vquic_ctx_update_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#अगर NGTCP2_VERSION_अंक < 0x011100
काष्ठा cf_ngtcp2_sfind_ctx {
  int64_t stream_id;
  काष्ठा h3_stream_ctx *stream;
  uint32_t mid;
};

अटल bool cf_ngtcp2_sfind(uint32_t mid, व्योम *मूल्य, व्योम *user_तथ्य)
{
  काष्ठा cf_ngtcp2_sfind_ctx *fctx = user_तथ्य;
  काष्ठा h3_stream_ctx *stream = मूल्य;

  अगर(fctx->stream_id == stream->id) {
    fctx->mid = mid;
    fctx->stream = stream;
    वापस FALSE;
  }
  वापस TRUE; /* continue */
}

अटल काष्ठा h3_stream_ctx *cf_ngtcp2_get_stream(काष्ठा cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  काष्ठा cf_ngtcp2_sfind_ctx fctx;
  fctx.stream_id = stream_id;
  fctx.stream = शून्य;
  Curl_u32_ptrset_visit(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  वापस fctx.stream;
}
#अन्यथा
अटल काष्ठा h3_stream_ctx *cf_ngtcp2_get_stream(काष्ठा cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  वापस ngtcp2_conn_get_stream_user_तथ्य(ctx->qconn, stream_id);
}
#पूर्ण_अगर

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
अटल CURLcode read_pkt_to_send(व्योम *userp,
                                 अचिन्हित अक्षर *buf, माप_प्रकार buflen,
                                 माप_प्रकार *pnread)
{
  काष्ठा cf_ngtcp2_io_ctx *x = userp;
  काष्ठा cf_ngtcp2_ctx *ctx = x->cf->ctx;
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
  क्रम(;;) {

    अगर(ctx->h3conn && ngtcp2_conn_get_max_तथ्य_बायाँ(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_stream(ctx->h3conn, &stream_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      अगर(veccnt < 0) {
        failf(x->तथ्य, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((पूर्णांक)veccnt));
        Curl_cf_ngtcp2_h3_err_set(x->cf, x->तथ्य, (पूर्णांक)veccnt);
        वापस CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_STREAM_FLAG_MORE |
            (fin ? NGTCP2_WRITE_STREAM_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_stream(ctx->qconn, &x->ps.path,
                                  शून्य, buf, buflen,
                                  &ndatalen, flags, stream_id,
                                  (स्थिर ngtcp2_vec *)vec, veccnt, x->ts);
    अगर(n == 0) {
      /* nothing to send */
      वापस CURLE_AGAIN;
    }
    अन्यथा अगर(n < 0) {
      चयन(n) {
      हाल NGTCP2_ERR_STREAM_तथ्य_BLOCKED: {
        काष्ठा h3_stream_ctx *stream;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_block_stream(ctx->h3conn, stream_id);
        CURL_TRC_CF(x->तथ्य, x->cf, "[%" PRId64 "] block quic flow",
                    stream_id);
        stream = cf_ngtcp2_get_stream(ctx, stream_id);
        अगर(stream) /* it might be not one of our h3 streams? */
          stream->quic_flow_blocked = TRUE;
        n = 0;
        अवरोध;
      }
      हाल NGTCP2_ERR_STREAM_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_stream_write(ctx->h3conn, stream_id);
        n = 0;
        अवरोध;
      हाल NGTCP2_ERR_WRITE_MORE:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        अवरोध;
      शेष:
        DEBUGASSERT(ndatalen == -1);
        failf(x->तथ्य, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((पूर्णांक)n));
        Curl_cf_ngtcp2_cmn_err_set(x->cf, x->तथ्य, (पूर्णांक)n);
        वापस CURLE_SEND_ERROR;
      }
    }

    अगर(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      पूर्णांक rv = nghttp3_conn_add_write_offset(ctx->h3conn, stream_id, ndatalen);
      अगर(rv) {
        failf(x->तथ्य, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        वापस CURLE_SEND_ERROR;
      }
    }

    अगर(n > 0) {
      /* packet assembled, leave */
      *pnread = (माप_प्रकार)n;
      वापस CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_progress_egress(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        काष्ठा cf_ngtcp2_io_ctx *pktx)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  माप_प्रकार nread;
  माप_प्रकार max_payload_आकार, path_max_payload_आकार;
  माप_प्रकार pktcnt = 0;
  माप_प्रकार gsolen = 0;  /* this disables gso until we have a clue */
  माप_प्रकार send_quantum;
  CURLcode परिणाम;
  काष्ठा cf_ngtcp2_io_ctx local_pktx;

  अगर(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, तथ्य);
    pktx = &local_pktx;
  }
  अन्यथा {
    Curl_cf_ngtcp2_io_ctx_update_time(तथ्य, pktx, cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  परिणाम = Curl_vquic_flush(cf, तथ्य, &ctx->q);
  अगर(परिणाम) {
    अगर(परिणाम == CURLE_AGAIN) {
      Curl_expire_set(तथ्य, EXPIRE_QUIC, 1, &pktx->now);
      वापस CURLE_OK;
    }
    वापस परिणाम;
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
  max_payload_आकार = ngtcp2_conn_get_max_tx_udp_payload_आकार(ctx->qconn);
  path_max_payload_आकार =
    ngtcp2_conn_get_path_max_tx_udp_payload_आकार(ctx->qconn);
  send_quantum = ngtcp2_conn_get_send_quantum(ctx->qconn);
  CURL_TRC_CF(तथ्य, cf, "egress, collect and send packets, quantum=%zu",
              send_quantum);
  क्रम(;;) {
    /* add the next packet to send, if any, to our buffer */
    परिणाम = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_आकार,
                            read_pkt_to_send, pktx, &nread);
    अगर(परिणाम == CURLE_AGAIN)
      अवरोध;
    अन्यथा अगर(परिणाम)
      वापस परिणाम;
    अन्यथा {
      माप_प्रकार buflen = Curl_bufq_len(&ctx->q.sendbuf);
      अगर((buflen >= send_quantum) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_आकार))
        अवरोध;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      अगर(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      अन्यथा अगर(nread > gsolen ||
              (gsolen > path_max_payload_आकार && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        परिणाम = Curl_vquic_send_पुच्छ_split(cf, तथ्य, &ctx->q,
                                            gsolen, nread, nread);
        अगर(परिणाम) {
          अगर(परिणाम == CURLE_AGAIN) {
            Curl_expire_set(तथ्य, EXPIRE_QUIC, 1, &pktx->now);
            वापस CURLE_OK;
          }
          वापस परिणाम;
        }
        pktcnt = 0;
      }
      अन्यथा अगर(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        अवरोध;
      }
    }
  }

  अगर(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(तथ्य, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    परिणाम = Curl_vquic_send(cf, तथ्य, &ctx->q, gsolen);
    अगर(परिणाम) {
      अगर(परिणाम == CURLE_AGAIN) {
        Curl_expire_set(तथ्य, EXPIRE_QUIC, 1, &pktx->now);
        वापस CURLE_OK;
      }
      वापस परिणाम;
    }
    Curl_cf_ngtcp2_io_ctx_update_time(तथ्य, pktx, cf);
    ngtcp2_conn_update_pkt_tx_time(ctx->qconn, pktx->ts);
  }
  वापस CURLE_OK;
}

काष्ठा cf_ngtcp2_recv_ctx {
  काष्ठा cf_ngtcp2_io_ctx *pktx;
  माप_प्रकार pkt_गिनती;
};

अटल CURLcode cf_ngtcp2_recv_pkts(स्थिर अचिन्हित अक्षर *buf, माप_प्रकार buflen,
                                    माप_प्रकार gso_आकार,
                                    काष्ठा sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    व्योम *userp)
{
  काष्ठा cf_ngtcp2_recv_ctx *rctx = userp;
  काष्ठा cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  काष्ठा cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_path path;
  माप_प्रकार offset, pktlen;
  पूर्णांक rv;

  अगर(!rctx->pkt_गिनती) {
    Curl_cf_ngtcp2_io_ctx_update_time(pktx->तथ्य, pktx, pktx->cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  अगर(ecn)
    CURL_TRC_CF(pktx->तथ्य, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_आकार, (अचिन्हित)ecn);
  ngtcp2_addr_init(&path.local, (काष्ठा sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&path.remote, (काष्ठा sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  क्रम(offset = 0; offset < buflen; offset += gso_आकार) {
    rctx->pkt_गिनती++;
    pktlen = ((offset + gso_आकार) <= buflen) ? gso_आकार : (buflen - offset);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &path, &pi,
                              buf + offset, pktlen, pktx->ts);
    अगर(rv) {
      CURL_TRC_CF(pktx->तथ्य, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_set(pktx->cf, pktx->तथ्य, rv);

      अगर(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        वापस CURLE_PEER_FAILED_VERIFICATION;
      वापस CURLE_RECV_ERROR;
    }
  }
  वापस CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_progress_ingress(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य,
                                         काष्ठा cf_ngtcp2_io_ctx *pktx)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा cf_ngtcp2_io_ctx local_pktx;
  काष्ठा cf_ngtcp2_recv_ctx rctx;
  CURLcode परिणाम = CURLE_OK;

  अगर(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, तथ्य);
    pktx = &local_pktx;
  }

  परिणाम = Curl_vquic_tls_before_recv(&ctx->tls, cf, तथ्य);
  अगर(परिणाम)
    वापस परिणाम;

  rctx.pktx = pktx;
  rctx.pkt_गिनती = 0;

  अगर(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(तथ्य, cf, "progress_ingress(socket)");
    वापस Curl_vquic_recv_packets(cf, तथ्य, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  अन्यथा {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    अचिन्हित अक्षर *buf;
    माप_प्रकार max_udp_payload = QUIC_TUNNEL_INBUF_आकार;
    माप_प्रकार pkt_limit = QUIC_TUNNEL_INGRESS_PKT_LIMIT;
    माप_प्रकार nread;
    काष्ठा sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(तथ्य, cf, "progress_ingress(sub-filters)");
    अगर(ctx->qconn) {
      माप_प्रकार max_path_payload;
      max_path_payload =
        ngtcp2_conn_get_path_max_tx_udp_payload_आकार(ctx->qconn);
      अगर(max_path_payload > max_udp_payload)
        max_udp_payload = max_path_payload;
    }

    अगर(ctx->tunnel_inbuf_len < max_udp_payload) {
      अचिन्हित अक्षर *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      अगर(!newbuf)
        वापस CURLE_OUT_OF_MEMORY;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    जबतक(pkt_limit--) {
      परिणाम = Curl_conn_cf_recv(cf->अग्र, तथ्य, (अक्षर *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      अगर(परिणाम == CURLE_AGAIN) {
        /* no more data available at the moment */
        वापस CURLE_OK;
      }
      अगर(परिणाम) {
        CURL_TRC_CF(तथ्य, cf, "ingress, recv from tunnel failed: %d",
                    (पूर्णांक)परिणाम);
        वापस परिणाम;
      }
      अगर(nread == 0) {
        /* tunnel closed */
        वापस CURLE_OK;
      }

      स_नकल(&remote_addr, ctx->connected_path.remote.addr,
             ctx->connected_path.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_path.remote.addrlen;
      परिणाम = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      अगर(परिणाम)
        वापस परिणाम;

      अगर(!ctx->q.got_first_byte) {
        ctx->q.got_first_byte = TRUE;
        ctx->q.first_byte_at = ctx->q.last_op;
      }
      ctx->q.last_io = ctx->q.last_op;
    }
    वापस CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_set_expiry(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य,
                                       काष्ठा cf_ngtcp2_io_ctx *pktx)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  अगर(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, तथ्य);
    pktx = &local_pktx;
  }
  अन्यथा {
    Curl_cf_ngtcp2_io_ctx_update_time(तथ्य, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  अगर(expiry != UINT64_MAX) {
    अगर(expiry <= pktx->ts) {
      CURLcode परिणाम;
      पूर्णांक rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      अगर(rv) {
        failf(तथ्य, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_set(cf, तथ्य, rv);
        वापस CURLE_SEND_ERROR;
      }
      परिणाम = Curl_cf_ngtcp2_progress_ingress(cf, तथ्य, pktx);
      अगर(!परिणाम)
        परिणाम = Curl_cf_ngtcp2_progress_egress(cf, तथ्य, pktx);
      अगर(परिणाम) {
        /* a verify failure during ingress must win over generic errors */
        अगर(ctx->tls_vrfy_परिणाम)
          परिणाम = ctx->tls_vrfy_परिणाम;
        वापस परिणाम;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    अगर(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      अगर(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_set(तथ्य, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  वापस CURLE_OK;
}

अटल व्योम cf_ngtcp2_setup_keep_alive(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  स्थिर ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  अगर(!ctx->qconn)
    वापस;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  अगर(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(तथ्य, cf, "no peer idle timeout, unset keep-alive");
  }
  अन्यथा अगर(!Curl_u32_ptrset_गिनती(&ctx->streams)) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(तथ्य, cf, "no active streams, unset keep-alive");
  }
  अन्यथा {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(तथ्य, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_stream_setup(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        काष्ठा h3_stream_ctx **pstream)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);

  *pstream = शून्य;
  अगर(!तथ्य)
    वापस CURLE_FAILED_INIT;

  अगर(stream) {
    *pstream = stream;
    वापस CURLE_OK;
  }

  stream = curlx_calloc(1, माप(*stream));
  अगर(!stream)
    वापस CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  stream->xfer_id = तथ्य->id;
  stream->mid = तथ्य->mid;
  stream->rx_offset = 0;
  stream->rx_offset_max = H3_STREAM_WINDOW_आकार_INITIAL;
  stream->tx_in_flight_ideal = H3_STREAM_SEND_BUF_INITIAL;

  /* on send, we control how much we put into the buffer */
  Curl_bufq_initp(&stream->sendbuf, &ctx->stream_bufcp,
                  H3_STREAM_SEND_CHUNKS, BUFQ_OPT_NONE);
  stream->tx_in_flight_आकार = 0;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_पंक्ति_LEN);

  अगर(Curl_u32_ptrset_set(&ctx->streams, तथ्य->mid, stream)) {
    Curl_cf_ngtcp2_h3_stream_ctx_free(stream);
    वापस CURLE_OUT_OF_MEMORY;
  }

  अगर(Curl_u32_ptrset_गिनती(&ctx->streams) == 1)
    cf_ngtcp2_setup_keep_alive(cf, तथ्य);

  *pstream = stream;
  वापस CURLE_OK;
}

व्योम Curl_cf_ngtcp2_h3_stream_close(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा h3_stream_ctx *stream)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(stream);
  अगर(!stream->closed && ctx->qconn && ctx->h3conn) {
    CURLcode परिणाम;

    nghttp3_conn_set_stream_user_तथ्य(ctx->h3conn, stream->id, शून्य);
    ngtcp2_conn_set_stream_user_तथ्य(ctx->qconn, stream->id, शून्य);
    stream->closed = TRUE;
    (व्योम)ngtcp2_conn_shutdown_stream(ctx->qconn, 0, stream->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    परिणाम = Curl_cf_ngtcp2_progress_egress(cf, तथ्य, शून्य);
    अगर(परिणाम)
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] cancel stream -> %d",
                  stream->id, (पूर्णांक)परिणाम);
  }
}

व्योम Curl_cf_ngtcp2_h3_stream_done(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);
  (व्योम)cf;
  अगर(stream) {
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] easy handle is done", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, तथ्य, stream);
    Curl_u32_ptrset_unset(&ctx->streams, तथ्य->mid);
    अगर(!Curl_u32_ptrset_गिनती(&ctx->streams))
      cf_ngtcp2_setup_keep_alive(cf, तथ्य);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      bool *निवेश_pending)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  स्थिर ngtcp2_transport_params *rp;
  काष्ठा cf_call_तथ्य save;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  *निवेश_pending = FALSE;
  अगर(!ctx->qconn || ctx->shutdown_started)
    जाओ out;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  अगर(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &ctx->q.last_io);
    अगर(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      अगर((uint64_t)idletime_ms > max_idle_ms)
        जाओ out;
    }
  }

  अगर(!cf->अग्र || !cf->अग्र->cft->is_alive(cf->अग्र, तथ्य, निवेश_pending))
    जाओ out;

  alive = TRUE;
  अगर(*निवेश_pending) {
    CURLcode परिणाम;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *निवेश_pending = FALSE;
    परिणाम = Curl_cf_ngtcp2_progress_ingress(cf, तथ्य, शून्य);
    CURL_TRC_CF(तथ्य, cf, "is_alive, progress ingress -> %d", (पूर्णांक)परिणाम);
    alive = परिणाम ? FALSE : TRUE;
  }

out:
  CF_तथ्य_RESTORE(cf, save);
  वापस alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(काष्ठा cf_ngtcp2_ctx *ctx,
                                      काष्ठा Curl_easy *तथ्य)
{
  int64_t ctrl_stream_id, qpack_enc_stream_id, qpack_dec_stream_id;
  पूर्णांक rc;

  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &ctrl_stream_id, शून्य);
  अगर(rc) {
    failf(तथ्य, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    वापस CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_stream(ctx->h3conn, ctrl_stream_id);
  अगर(rc) {
    failf(तथ्य, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    वापस CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_enc_stream_id, शून्य);
  अगर(rc) {
    failf(तथ्य, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    वापस CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_dec_stream_id, शून्य);
  अगर(rc) {
    failf(तथ्य, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    वापस CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_stream_id,
                                       qpack_dec_stream_id);
  अगर(rc) {
    failf(तथ्य, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    वापस CURLE_QUIC_CONNECT_ERROR;
  }
  वापस CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;

  चयन(query) {
  हाल CF_QUERY_CONNECT_REPLY_MS:
    अगर((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < पूर्णांक_उच्च) ? (पूर्णांक)ms : पूर्णांक_उच्च;
      वापस CURLE_OK;
    }
    अवरोध;
  हाल CF_QUERY_REALLY_CONNECTED:
    अगर(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_first_byte;
      वापस CURLE_OK;
    }
    अवरोध;
  शेष:
    अवरोध;
  }
  वापस cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, तथ्य, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  (व्योम)arg1;
  (व्योम)arg2;
  चयन(event) {
  हाल CF_CTRL_REPORT_STATS:
    अगर(cf->connected && !ctx->stats_reported) {
      अगर((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
        Curl_pgrsTimeWas(तथ्य, TIMER_CONNECT, ctx->q.first_byte_at);
        ctx->stats_reported = TRUE;
      }
      अन्यथा अगर(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        अगर(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrsTimeWas(तथ्य, TIMER_CONNECT, ctx->q.first_byte_at);
        Curl_pgrsTimeWas(तथ्य, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    अवरोध;
  शेष:
    अवरोध;
  }
  वापस परिणाम;
}

#पूर्ण_अगर /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
