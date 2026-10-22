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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#অন্তর্ভাব <ngtcp2/ngtcp2.h>

#ifdef USE_OPENSSL
#অন্তর্ভাব <openssl/err.h>
#যদি defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#অন্তর্ভাব <ngtcp2/ngtcp2_crypto_boringssl.h>
#নতুবা_যদি defined(OPENSSL_QUIC_API2)
#অন্তর্ভাব <ngtcp2/ngtcp2_crypto_ossl.h>
#অন্যথা
#অন্তর্ভাব <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#অন্তর্ভাব "vtls/openssl.h"
#নতুবা_যদি defined(USE_GNUTLS)
#অন্তর্ভাব <ngtcp2/ngtcp2_crypto_gnutls.h>
#অন্তর্ভাব "vtls/gtls.h"
#নতুবা_যদি defined(USE_WOLFSSL)
#অন্তর্ভাব <ngtcp2/ngtcp2_crypto_wolfssl.h>
#অন্তর্ভাব "vtls/wolfssl.h"
#endif

#অন্তর্ভাব <nghttp3/nghttp3.h>

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "u32_ptrset.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-socket.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "curlx/win32-fopen.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "http1.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "sockaddr.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "vquic/vquic.h"
#অন্তর্ভাব "vquic/vquic_int.h"
#অন্তর্ভাব "vquic/vquic-tls.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "vquic/cf-ngtcp2-cmn.h"

/*
 * Store ngtcp2 version info in this buffer.
 */
শূন্য Curl_ngtcp2_ver(অক্ষর *p, size_t len)
{
  ধ্রুবক ngtcp2_info *ng2 = ngtcp2_version(0);
  ধ্রুবক nghttp3_info *ht3 = nghttp3_version(0);
  (শূন্য)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->version_str, ht3->version_str);
}

শূন্য Curl_cf_ngtcp2_h3_stream_ctx_free(কাঠামো h3_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

স্থির শূন্য h3_stream_set_free(uint32_t id, শূন্য *stream)
{
  (শূন্য)id;
  DEBUGASSERT(stream);
  Curl_cf_ngtcp2_h3_stream_ctx_free((কাঠামো h3_stream_ctx *)stream);
}

স্থির bool cf_ngtcp2_h3_err_is_fatal(পূর্ণসংখ্যা code)
{
  ফেরত (NGHTTP3_ERR_FATAL >= code) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM == code);
}

শূন্য Curl_cf_ngtcp2_h3_err_set(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা code)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  যদি(!ctx->last_error.error_code) {
    ngtcp2_ccerr_set_application_error(&ctx->last_error,
      nghttp3_err_infer_quic_app_error_code(code), ফাঁকা, 0);
  }
  যদি(cf_ngtcp2_h3_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, তথ্য);
}

CURLcode Curl_cf_ngtcp2_ctx_init(কাঠামো cf_ngtcp2_ctx *ctx,
                                 কাঠামো Curl_peer *origin,
                                 কাঠামো Curl_peer *peer,
                                 কাঠামো ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = ফাঁকা;
  ctx->tunnel_inbuf_len = 0;
  ctx->version = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->stream_bufcp, H3_STREAM_CHUNK_আকার,
                  H3_STREAM_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  ফেরত Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

শূন্য Curl_cf_ngtcp2_ctx_cleanup(কাঠামো cf_ngtcp2_ctx *ctx)
{
  যদি(ctx && ctx->initialized) {
    যদি(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    যদি(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = ফাঁকা;
    }
    যদি(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = ফাঁকা;
    }
#ifdef OPENSSL_QUIC_API2
    যদি(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = ফাঁকা;
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

কাঠামো Curl_easy *Curl_cf_ngtcp2_get_xfer(কাঠামো Curl_cfilter *cf,
                                          শূন্য *stream_user_তথ্য,
                                          কাঠামো h3_stream_ctx **pstream)
{
  কাঠামো h3_stream_ctx *stream = stream_user_তথ্য;

  যদি(pstream)
    *pstream = stream;
  যদি(stream && (stream->mid != UINT32_MAX)) {
    কাঠামো Curl_easy *তথ্য, *call_তথ্য = CF_তথ্য_CURRENT(cf);
    যদি(!call_তথ্য || !call_তথ্য->multi) {
      DEBUGASSERT(0);
      ফেরত ফাঁকা;
    }
    তথ্য = Curl_multi_get_easy(call_তথ্য->multi, stream->mid);
    যদি(তথ্য && (তথ্য->id == stream->xfer_id))
      ফেরত তথ্য;
    DEBUGASSERT(0);
  }
  ফেরত ফাঁকা;
}

স্থির ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  কাঠামো Curl_cfilter *cf = conn_ref->user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  ফেরত ctx->qconn;
}

#ifdef DEBUG_NGTCP2
স্থির শূন্য quic_printf(শূন্য *user_তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  va_list ap;
  (শূন্য)user_তথ্য;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#endif

স্থির শূন্য qlog_callback(শূন্য *user_তথ্য, uint32_t flags,
                          ধ্রুবক শূন্য *তথ্য, size_t datalen)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  (শূন্য)flags;
  যদি(ctx->qlogfd != -1) {
    ssize_t rc = লিখ(ctx->qlogfd, তথ্য, datalen);
    যদি(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

স্থির শূন্য quic_settings(কাঠামো cf_ngtcp2_ctx *ctx,
                          কাঠামো Curl_easy *তথ্য,
                          কাঠামো cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#ifdef DEBUG_NGTCP2
  s->log_printf = quic_printf;
#অন্যথা
  s->log_printf = ফাঁকা;
#endif

  s->initial_ts = pktx->ts;
  s->handshake_timeout = (তথ্য->set.connecttimeout > 0) ?
    তথ্য->set.connecttimeout * NGTCP2_MILLISECONDS : QUIC_HANDSHAKE_TIMEOUT;
  s->max_window = H3_CONN_WINDOW_আকার_MAX;
  s->max_stream_window = 0; /* disable ngtcp2 auto-tuning of window */
  s->no_pmtud = FALSE;
#ifdef NGTCP2_SETTINGS_V3
  /* try ten times the ngtcp2 defaults here for problems with Caddy */
  s->glitch_ratelim_burst = 1000 * 10;
  s->glitch_ratelim_rate = 33 * 10;
#endif
  t->initial_max_তথ্য = s->max_window;
  t->initial_max_stream_তথ্য_bidi_local = H3_STREAM_WINDOW_আকার_INITIAL;
  t->initial_max_stream_তথ্য_bidi_remote = H3_STREAM_WINDOW_আকার_INITIAL;
  t->initial_max_stream_তথ্য_uni = t->initial_max_তথ্য;
  t->initial_max_streams_bidi = QUIC_MAX_STREAMS;
  t->initial_max_streams_uni = QUIC_MAX_STREAMS;
  t->max_idle_timeout = 0; /* no idle timeout from our side */
  যদি(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#যদি defined(_MSC_VER) && defined(_DLL)
#pragma warning(ঠেলো)
#pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

স্থির পূর্ণসংখ্যা cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, শূন্য *user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ফাঁকা;
  কাঠামো Curl_easy *তথ্য;

  (শূন্য)tconn;
  DEBUGASSERT(ctx);
  তথ্য = CF_তথ্য_CURRENT(cf);
  DEBUGASSERT(তথ্য);
  যদি(!ctx || !তথ্য)
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(তথ্য);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, তথ্য);

  ctx->tls_vrfy_ফলাফল = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    তথ্য, &ctx->ssl_peer);
  যদি(ctx->tls_vrfy_ফলাফল)
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;

#ifdef CURLVERBOSE
  যদি(Curl_trc_is_verbose(তথ্য)) {
    ধ্রুবক ngtcp2_transport_params *rp;
    rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
    CURL_TRC_CF(তথ্য, cf, "handshake complete after %" FMT_TIMEDIFF_T
                "ms, remote transport[max_udp_payload=%" PRIu64
                ", initial_max_data=%" PRIu64 "]",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at),
                rp->max_udp_payload_আকার, rp->initial_max_তথ্য);
  }
#endif

  /* In case of earlydata, where we simulate being connected, update
   * the handshake time when we really did connect */
  যদি(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrsTimeWas(তথ্য, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  যদি(ctx->use_earlydata) {
#যদি defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    যদি(SSL_get_early_তথ্য_status(ctx->tls.ossl.ssl) == SSL_EARLY_তথ্য_NOT_SENT)
      CURL_TRC_CF(তথ্য, cf, "OpenSSL did not send early data");
#endif

#যদি NGTCP2_VERSION_অঙ্ক >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_তথ্য_rejected2(ctx->qconn);
#অন্যথা /* older NGTCP2 */
    {
#যদি defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      পূর্ণসংখ্যা ossl_early_status = SSL_get_early_তথ্য_status(ctx->tls.ossl.ssl);
      যদি(ossl_early_status == SSL_EARLY_তথ্য_NOT_SENT)
        CURL_TRC_CF(তথ্য, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_status == SSL_EARLY_তথ্য_ACCEPTED);
#নতুবা_যদি defined(USE_GNUTLS)
      পূর্ণসংখ্যা flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_তথ্য);
#নতুবা_যদি defined(USE_WOLFSSL)
#ifdef WOLFSSL_EARLY_তথ্য
      ctx->earlydata_accepted =
        (wolf_SSL_get_early_তথ্য_status(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_তথ্য_REJECTED);
#অন্যথা
      DEBUGASSERT(0); /* should not come here if ED is disabled. */
      ctx->earlydata_accepted = FALSE;
#endif /* WOLFSSL_EARLY_DATA */
#endif /* OPENSSL or GNUTLS or WOLFSSL */
    }
#endif /* older NGTCP2 */
    CURL_TRC_CF(তথ্য, cf, "server did%s accept %zu bytes of early data",
                ctx->earlydata_accepted ? "" : " not", ctx->earlydata_skip);
    Curl_pgrs_Early_তথ্য(তথ্য, ctx->earlydata_accepted ?
                              (curl_off_t)ctx->earlydata_skip :
                             -(curl_off_t)ctx->earlydata_skip);
  }
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_recv_stream_তথ্য(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t stream_id, uint64_t offset,
                               ধ্রুবক uint8_t *buf, size_t buflen,
                               শূন্য *user_তথ্য, শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  পূর্ণসংখ্যা fin = (flags & NGTCP2_STREAM_তথ্য_FLAG_FIN) ? 1 : 0;
  কাঠামো h3_stream_ctx *stream;
  কাঠামো Curl_easy *তথ্য =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_তথ্য, &stream);

  (শূন্য)offset;
  rc = nghttp3_conn_read_stream(ctx->h3conn, stream_id, buf, buflen, fin);
  যদি(rc < 0) {
    যদি(তথ্য && stream) {
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  stream_id, stream->reset, stream->closed);
    }
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  যদি(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_stream_offset(tconn, stream_id, nconsumed);
    ngtcp2_conn_extend_max_offset(tconn, nconsumed);
    যদি(stream) {
      stream->rx_offset += nconsumed;
      stream->rx_offset_max += nconsumed;
    }
  }
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_acked_stream_তথ্য_offset(ngtcp2_conn *tconn, int64_t stream_id,
                                       uint64_t offset, uint64_t datalen,
                                       শূন্য *user_তথ্য, শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  পূর্ণসংখ্যা rv;
  (শূন্য)stream_id;
  (শূন্য)tconn;
  (শূন্য)offset;
  (শূন্য)datalen;
  (শূন্য)stream_user_তথ্য;

  rv = nghttp3_conn_add_ack_offset(ctx->h3conn, stream_id, datalen);
  যদি(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_stream_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id, uint64_t app_error_code,
                           শূন্য *user_তথ্য, শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = Curl_cf_ngtcp2_get_xfer(cf, stream_user_তথ্য, ফাঁকা);
  পূর্ণসংখ্যা rv;

  (শূন্য)tconn;
  /* stream is closed... */
  যদি(!তথ্য)
    তথ্য = CF_তথ্য_CURRENT(cf);
  যদি(!তথ্য)
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;

  যদি(!(flags & NGTCP2_STREAM_CLOSE_FLAG_APP_ERROR_CODE_SET)) {
    app_error_code = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, app_error_code);
  CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, app_error_code, rv);
  যদি(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, তথ্য, rv);
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ফেরত 0;
}

#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
স্থির পূর্ণসংখ্যা cb_stream_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id,
                           uint64_t rx_app_error_code,
                           uint64_t tx_app_error_code,
                           শূন্য *user_তথ্য, শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = Curl_cf_ngtcp2_get_xfer(cf, stream_user_তথ্য, ফাঁকা);
  uint64_t h3_app_error_code = NGHTTP3_H3_NO_ERROR;
  পূর্ণসংখ্যা rv;

  (শূন্য)tconn;
  (শূন্য)tx_app_error_code;
  /* stream is closed... */
  যদি(!তথ্য)
    তথ্য = CF_তথ্য_CURRENT(cf);
  যদি(!তথ্য)
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;

  যদি(flags & NGTCP2_STREAM_CLOSE2_FLAG_RX_APP_ERROR_CODE_SET)
    h3_app_error_code = rx_app_error_code;

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, h3_app_error_code);
  CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, h3_app_error_code, rv);
  যদি(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, তথ্য, rv);
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ফেরত 0;
}
#endif

স্থির পূর্ণসংখ্যা cb_stream_reset(ngtcp2_conn *tconn, int64_t stream_id,
                           uint64_t final_আকার, uint64_t app_error_code,
                           শূন্য *user_তথ্য, শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = Curl_cf_ngtcp2_get_xfer(cf, stream_user_তথ্য, ফাঁকা);
  পূর্ণসংখ্যা rv;
  (শূন্য)tconn;
  (শূন্য)final_আকার;
  (শূন্য)app_error_code;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  যদি(তথ্য)
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  যদি(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_stream_stop_sending(ngtcp2_conn *tconn, int64_t stream_id,
                                  uint64_t app_error_code, শূন্য *user_তথ্য,
                                  শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  পূর্ণসংখ্যা rv;
  (শূন্য)tconn;
  (শূন্য)app_error_code;
  (শূন্য)stream_user_তথ্য;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  যদি(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            শূন্য *user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);

  (শূন্য)tconn;
  ctx->max_bidi_streams = max_streams;
  যদি(তথ্য)
    CURL_TRC_CF(তথ্য, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->used_bidi_streams);
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_extend_max_stream_তথ্য(ngtcp2_conn *tconn, int64_t stream_id,
                                     uint64_t max_তথ্য, শূন্য *user_তথ্য,
                                     শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream;
  কাঠামো Curl_easy *তথ্য =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_তথ্য, &stream);
  পূর্ণসংখ্যা rv;
  (শূন্য)tconn;
  (শূন্য)max_তথ্য;

  rv = nghttp3_conn_unblock_stream(ctx->h3conn, stream_id);
  যদি(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;
  }
  যদি(stream && stream->quic_flow_blocked) {
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] unblock quic flow", stream_id);
    stream->quic_flow_blocked = FALSE;
    যদি(তথ্য)
      Curl_multi_mark_dirty(তথ্য);
  }
  ফেরত 0;
}

স্থির শূন্য cb_rand(uint8_t *dest, size_t destlen,
                    ধ্রুবক ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode ফলাফল;
  (শূন্য)rand_ctx;

  ফলাফল = Curl_rand(ফাঁকা, dest, destlen);
  যদি(ফলাফল) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
স্থির পূর্ণসংখ্যা cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    শূন্য *user_তথ্য)
{
  CURLcode ফলাফল;
  (শূন্য)tconn;
  (শূন্য)user_তথ্য;

  ফলাফল = Curl_rand(ফাঁকা, cid->তথ্য, cidlen);
  যদি(ফলাফল)
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  ফলাফল = Curl_rand(ফাঁকা, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  যদি(ফলাফল)
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;

  ফেরত 0;
}

#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
স্থির পূর্ণসংখ্যা cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  কাঠামো ngtcp2_stateless_reset_token *token, size_t cidlen, শূন্য *user_তথ্য)
{
  CURLcode ফলাফল;
  (শূন্য)tconn;
  (শূন্য)user_তথ্য;

  ফলাফল = Curl_rand(ফাঁকা, cid->তথ্য, cidlen);
  যদি(ফলাফল)
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  ফলাফল = Curl_rand(ফাঁকা, token->তথ্য, আকার(token->তথ্য));
  যদি(ফলাফল)
    ফেরত NGTCP2_ERR_CALLBACK_FAILURE;

  ফেরত 0;
}
#endif

স্থির পূর্ণসংখ্যা cb_recv_rx_চাবি(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          শূন্য *user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ফাঁকা;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  (শূন্য)tconn;

  যদি(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    ফেরত 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(তথ্য);
  যদি(ctx && তথ্য && !ctx->h3conn && ctx->init_h3_conn_cb) {
    যদি(ctx->init_h3_conn_cb(cf, তথ্য, ctx))
      ফেরত NGTCP2_ERR_CALLBACK_FAILURE;
  }
  ফেরত 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
স্থির ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  ফাঁকা, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_তথ্য_cb,
  cb_ngtcp2_handshake_completed,
  ফাঁকা, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_mask_cb,
  cb_recv_stream_তথ্য,
  cb_acked_stream_তথ্য_offset,
  ফাঁকা, /* stream_open */
  cb_stream_close,
  ফাঁকা, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  ফাঁকা, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  ফাঁকা, /* remove_connection_id */
  ngtcp2_crypto_update_চাবি_cb, /* update_key */
  ফাঁকা, /* path_validation */
  ফাঁকা, /* select_preferred_addr */
  cb_stream_reset,
  ফাঁকা, /* extend_max_remote_streams_bidi */
  ফাঁকা, /* extend_max_remote_streams_uni */
  cb_extend_max_stream_তথ্য,
  ফাঁকা, /* dcid_status */
  ফাঁকা, /* handshake_confirmed */
  ফাঁকা, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  ফাঁকা, /* recv_datagram */
  ফাঁকা, /* ack_datagram */
  ফাঁকা, /* lost_datagram */
  ngtcp2_crypto_get_path_challenge_তথ্য_cb,
  cb_stream_stop_sending,
  ফাঁকা, /* version_negotiation */
  cb_recv_rx_চাবি,
  ফাঁকা, /* recv_tx_key */
  ফাঁকা, /* early_data_rejected */
#ifdef NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  ফাঁকা, /* begin_path_validation */
#endif
#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  ফাঁকা, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  ফাঁকা, /* dcid_status2 */
  ngtcp2_crypto_get_path_challenge_data2_cb, /* get_path_challenge_data2 */
#endif
#ifdef NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  ফাঁকা, /* recv_stop_sending */
#endif
#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_stream_close2, /* is called instead of cb_stream_close when set */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic বের_করো
#endif

#যদি defined(_MSC_VER) && defined(_DLL)
#pragma warning(বের_করো)
#endif

#ifdef USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
স্থির পূর্ণসংখ্যা quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  কাঠামো Curl_cfilter *cf;
  কাঠামো cf_ngtcp2_ctx *ctx;
  কাঠামো Curl_easy *তথ্য;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_তথ্য(ssl);
  cf = cref ? cref->user_তথ্য : ফাঁকা;
  ctx = cf ? cf->ctx : ফাঁকা;
  তথ্য = cf ? CF_তথ্য_CURRENT(cf) : ফাঁকা;
  যদি(cf && তথ্য && ctx) {
    unsigned অক্ষর *quic_tp = ফাঁকা;
    size_t quic_tp_len = 0;
#ifdef HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     আকার(tpbuf));
    যদি(tplen < 0)
      CURL_TRC_CF(তথ্য, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((পূর্ণসংখ্যা)tplen));
    অন্যথা {
      quic_tp = (unsigned অক্ষর *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#endif
    Curl_ossl_add_session(cf, তথ্য, &ctx->tls.ossl, ctx->ssl_peer.scache_চাবি,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, ফাঁকা);
  }
  ফেরত 0;
}
#endif /* USE_OPENSSL */

#ifdef USE_GNUTLS

#ifdef CURLVERBOSE
স্থির ধ্রুবক অক্ষর *gtls_hs_বার্তা_name(পূর্ণসংখ্যা mtype)
{
  নির্বাচন(mtype) {
  ক্ষেত্র 1:
    ফেরত "ClientHello";
  ক্ষেত্র 2:
    ফেরত "ServerHello";
  ক্ষেত্র 4:
    ফেরত "SessionTicket";
  ক্ষেত্র 8:
    ফেরত "EncryptedExtensions";
  ক্ষেত্র 11:
    ফেরত "Certificate";
  ক্ষেত্র 13:
    ফেরত "CertificateRequest";
  ক্ষেত্র 15:
    ফেরত "CertificateVerify";
  ক্ষেত্র 20:
    ফেরত "Finished";
  ক্ষেত্র 24:
    ফেরত "KeyUpdate";
  ক্ষেত্র 254:
    ফেরত "MessageHash";
  }
  ফেরত "Unknown";
}
#endif

স্থির পূর্ণসংখ্যা quic_gtls_handshake_cb(gnutls_session_t session, unsigned পূর্ণসংখ্যা htype,
                                  unsigned when, unsigned পূর্ণসংখ্যা incoming,
                                  ধ্রুবক gnutls_datum_t *বার্তা)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  কাঠামো Curl_cfilter *cf = conn_ref ? conn_ref->user_তথ্য : ফাঁকা;
  কাঠামো cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ফাঁকা;

  (শূন্য)বার্তা;
  (শূন্য)incoming;
  যদি(when && cf && ctx) { /* after message has been processed */
    কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
    DEBUGASSERT(তথ্য);
    যদি(!তথ্য)
      ফেরত 0;
    CURL_TRC_CF(তথ্য, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_বার্তা_name(htype), htype);
    নির্বাচন(htype) {
    ক্ষেত্র GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned অক্ষর *quic_tp = ফাঁকা;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       আকার(tpbuf));
      যদি(tplen < 0)
        CURL_TRC_CF(তথ্য, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((পূর্ণসংখ্যা)tplen));
      অন্যথা {
        quic_tp = (unsigned অক্ষর *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (শূন্য)Curl_gtls_cache_session(cf, তথ্য, ctx->ssl_peer.scache_চাবি,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    ফাঁকা);
      বিরতি;
    }
    পূর্বনির্ধারিত:
      বিরতি;
    }
  }
  ফেরত 0;
}
#endif /* USE_GNUTLS */

#ifdef USE_WOLFSSL
স্থির পূর্ণসংখ্যা wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolf_SSL_get_app_তথ্য(ssl);
  কাঠামো Curl_cfilter *cf = conn_ref ? conn_ref->user_তথ্য : ফাঁকা;

  DEBUGASSERT(cf);
  যদি(cf && session) {
    কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
    কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
    DEBUGASSERT(তথ্য);
    যদি(তথ্য && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned অক্ষর *quic_tp = ফাঁকা;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       আকার(tpbuf));
      যদি(tplen < 0)
        CURL_TRC_CF(তথ্য, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((পূর্ণসংখ্যা)tplen));
      অন্যথা {
        quic_tp = (unsigned অক্ষর *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (শূন্য)Curl_wssl_cache_session(cf, তথ্য, ctx->ssl_peer.scache_চাবি,
                                    session, wolfSSL_version(ssl),
                                    "h3", quic_tp, quic_tp_len, ফাঁকা);
    }
  }
  ফেরত 0;
}
#endif /* USE_WOLFSSL */

স্থির CURLcode cf_ngtcp2_tls_ctx_setup(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        শূন্য *user_তথ্য)
{
  কাঠামো curl_tls_ctx *ctx = user_তথ্য;

#ifdef USE_OPENSSL
#যদি defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  যদি(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(তথ্য, "ngtcp2_crypto_boringssl_configure_client_context failed");
    ফেরত CURLE_FAILED_INIT;
  }
#নতুবা_যদি defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#অন্যথা
  যদি(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(তথ্য, "ngtcp2_crypto_quictls_configure_client_context failed");
    ফেরত CURLE_FAILED_INIT;
  }
#endif /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  যদি(Curl_ssl_scache_use(cf, তথ্য)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_set_session_cache_mode(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#নতুবা_যদি defined(USE_GNUTLS)
  যদি(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(তথ্য, "ngtcp2_crypto_gnutls_configure_client_session failed");
    ফেরত CURLE_FAILED_INIT;
  }
  যদি(Curl_ssl_scache_use(cf, তথ্য)) {
    gnutls_handshake_set_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#নতুবা_যদি defined(USE_WOLFSSL)
  যদি(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(তথ্য, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    ফেরত CURLE_FAILED_INIT;
  }
  যদি(Curl_ssl_scache_use(cf, তথ্য)) {
    /* Register to get notified when a new session is received */
    wolfSSL_CTX_sess_set_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#endif
  ফেরত CURLE_OK;
}

স্থির CURLcode cf_ngtcp2_on_session_reuse(কাঠামো Curl_cfilter *cf,
                                           কাঠামো Curl_easy *তথ্য,
                                           কাঠামো alpn_spec *alpns,
                                           কাঠামো Curl_ssl_session *scs,
                                           bool *do_early_তথ্য)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  *do_early_তথ্য = FALSE;
#যদি defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#endif
#ifdef USE_GNUTLS
  ctx->earlydata_max =
    gnutls_record_get_max_early_তথ্য_আকার(ctx->tls.gtls.session);
#endif
#ifdef USE_WOLFSSL
#ifdef WOLFSSL_EARLY_তথ্য
  ctx->earlydata_max = scs->earlydata_max;
#অন্যথা
  ctx->earlydata_max = 0;
#endif /* WOLFSSL_EARLY_DATA */
#endif
#যদি defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  যদি(!ctx->earlydata_max) {
    CURL_TRC_CF(তথ্য, cf, "SSL session does not allow earlydata");
  }
  অন্যথা যদি(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(তথ্য, cf, "SSL session from different ALPN, no early data");
  }
  অন্যথা যদি(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(তথ্য, cf, "no 0RTT transport parameters, no early data");
  }
  অন্যথা {
    পূর্ণসংখ্যা rv;
    rv = ngtcp2_conn_decode_and_set_0rtt_transport_params(
      ctx->qconn, (ধ্রুবক uint8_t *)scs->quic_tp, scs->quic_tp_len);
    যদি(rv)
      CURL_TRC_CF(তথ্য, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    অন্যথা যদি(ctx->init_h3_conn_cb) {
      infof(তথ্য, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      ফলাফল = ctx->init_h3_conn_cb(cf, তথ্য, ctx);
      যদি(!ফলাফল) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_তথ্য = TRUE;
      }
    }
    অন্যথা { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_তথ্য = TRUE;
    }
  }
#অন্যথা /* not supported in the TLS backend */
  (শূন্য)তথ্য;
  (শূন্য)ctx;
  (শূন্য)scs;
  (শূন্য)alpns;
#endif
  ফেরত ফলাফল;
}

/*
 * Might be called twice for happy eyeballs.
 */
স্থির CURLcode cf_connect_start(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো cf_ngtcp2_io_ctx *pktx)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  পূর্ণসংখ্যা rc;
  পূর্ণসংখ্যা rv;
  CURLcode ফলাফল;
  ধ্রুবক কাঠামো Curl_sockaddr_ex *sockaddr = ফাঁকা;
  পূর্ণসংখ্যা qfd;
  স্থির ধ্রুবক কাঠামো alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  ফলাফল = Curl_rand(তথ্য, ctx->dcid.তথ্য, NGTCP2_MAX_CIDLEN);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  ফলাফল = Curl_rand(তথ্য, ctx->scid.তথ্য, NGTCP2_MAX_CIDLEN);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  (শূন্য)Curl_qlogdir(তথ্য, ctx->scid.তথ্য, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, তথ্য, pktx);

  ফলাফল = Curl_vquic_ctx_init(তথ্য, &ctx->q);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Query socket and remote address from sub-chain */
  যদি(Curl_cf_socket_peek(cf->পরবর্তী, তথ্য, &ctx->q.sockfd, &sockaddr, ফাঁকা)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  যদি(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = আকার(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (কাঠামো sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    যদি(rv == -1)
      ফেরত CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (কাঠামো sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_path.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  অন্যথা {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ধ্রুবক কাঠামো Curl_sockaddr_ex *remote = ফাঁকা;
    যদি(cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, CF_QUERY_REMOTE_ADDR, ফাঁকা,
                            CURL_UNCONST(&remote)))
      ফেরত CURLE_QUIC_CONNECT_ERROR;
    যদি(!remote)
      ফেরত CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, আকার(ctx->q.local_addr));
    নির্বাচন(remote->family) {
    ক্ষেত্র AF_INET:
      ((কাঠামো sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = আকার(কাঠামো sockaddr_in);
      বিরতি;
#ifdef USE_IPV6
    ক্ষেত্র AF_INET6:
      ((কাঠামো sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = আকার(কাঠামো sockaddr_in6);
      বিরতি;
#endif
    পূর্বনির্ধারিত:
      ফেরত CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (কাঠামো sockaddr *)&ctx->q.local_addr,
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
  যদি(rc)
    ফেরত CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_তথ্য = cf;

  ফলাফল = Curl_vquic_tls_init(&ctx->tls, cf, তথ্য,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_setup, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  যদি(ফলাফল)
    ফেরত ফলাফল;

#যদি defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  যদি(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(তথ্য, "ngtcp2_crypto_ossl_ctx_new failed");
    ফেরত CURLE_FAILED_INIT;
  }
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  যদি(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(তথ্য, "ngtcp2_crypto_ossl_configure_client_session failed");
    ফেরত CURLE_FAILED_INIT;
  }
#নতুবা_যদি defined(USE_OPENSSL)
  SSL_set_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#নতুবা_যদি defined(USE_GNUTLS)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#নতুবা_যদি defined(USE_WOLFSSL)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#অন্যথা
#error "ngtcp2 TLS backend not defined"
#endif

#যদি defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  যদি(ctx->use_earlydata &&
     !SSL_set_quic_tls_early_তথ্য_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(তথ্য, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#endif
  ngtcp2_ccerr_default(&ctx->last_error);

  ফেরত CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    bool *done)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো cf_call_তথ্য save;
  কাঠামো cf_ngtcp2_io_ctx pktx;

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  /* Connect the sub-chain */
  যদি(cf->পরবর্তী && !cf->পরবর্তী->connected) {
    ফলাফল = Curl_conn_cf_connect(cf->পরবর্তী, তথ্য, done);
    যদি(ফলাফল || !*done)
      ফেরত ফলাফল;
  }

  *done = FALSE;

  যদি(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(তথ্য, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(তথ্য, cf, "need HTTPS-RR, delaying connect");
    ফেরত CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, তথ্য);
  CF_তথ্য_SAVE(save, cf, তথ্য);

  যদি(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(তথ্য);
    ফলাফল = cf_connect_start(cf, তথ্য, &pktx);
    যদি(ফলাফল)
      যাও out;
    যদি(cf->connected) {
      *done = TRUE;
      যাও out;
    }
    ফলাফল = Curl_cf_ngtcp2_progress_egress(cf, তথ্য, &pktx);
    /* we do not expect to be able to recv anything yet */
    যাও out;
  }

  ফলাফল = Curl_cf_ngtcp2_progress_ingress(cf, তথ্য, &pktx);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = Curl_cf_ngtcp2_progress_egress(cf, তথ্য, &pktx);
  যদি(ফলাফল)
    যাও out;

  যদি(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    ফলাফল = ctx->tls_vrfy_ফলাফল;
    যদি(!ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "peer verified");
      cf->connected = TRUE;
      *done = TRUE;
    }
  }

out:
  যদি(ctx->tls_vrfy_ফলাফল)
    ফলাফল = ctx->tls_vrfy_ফলাফল;
  যদি(ctx->qconn &&
     ((ফলাফল == CURLE_RECV_ERROR) || (ফলাফল == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_in_draining_period(ctx->qconn)) {
    ধ্রুবক ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    ফলাফল = CURLE_COULDNT_CONNECT;
    যদি(cerr) {
      CURL_TRC_CF(তথ্য, cf, "connect error, type=%d, code=%" PRIu64,
                  (পূর্ণসংখ্যা)cerr->type, cerr->error_code);
      নির্বাচন(cerr->type) {
      ক্ষেত্র NGTCP2_CCERR_TYPE_VERSION_NEGOTIATION:
        CURL_TRC_CF(তথ্য, cf, "error in version negotiation");
        বিরতি;
      পূর্বনির্ধারিত:
        যদি(cerr->error_code >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(তথ্য, cf, "crypto error, tls alert=%u",
                      (unsigned পূর্ণসংখ্যা)(cerr->error_code & 0xffU));
        }
        অন্যথা যদি(cerr->error_code == NGTCP2_CONNECTION_REFUSED) {
          CURL_TRC_CF(তথ্য, cf, "connection refused by server");
          /* When a QUIC server instance is shutting down, it may send us a
           * CONNECTION_CLOSE with this code right away. We want
           * to keep on trying in this case. */
          ফলাফল = CURLE_WEIRD_SERVER_REPLY;
        }
      }
    }
  }

#ifdef CURLVERBOSE
  যদি(ফলাফল) {
    যদি(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      কাঠামো ip_quadruple ip;

      যদি(!Curl_cf_socket_peek(cf->পরবর্তী, তথ্য, ফাঁকা, ফাঁকা, &ip))
        infof(তথ্য, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_port, curl_easy_strerror(ফলাফল));
    }
  }
#endif
  যদি(!ফলাফল && ctx->qconn) {
    ফলাফল = Curl_cf_ngtcp2_cmn_set_expiry(cf, তথ্য, &pktx);
  }
  যদি(ফলাফল || *done)
    CURL_TRC_CF(তথ্য, cf, "connect -> %d, done=%d", (পূর্ণসংখ্যা)ফলাফল, *done);
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(কাঠামো Curl_cfilter *cf,
                                     কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  কাঠামো cf_ngtcp2_io_ctx pktx;
  CURLcode ফলাফল = CURLE_OK;

  যদি(cf->shutdown || !ctx->qconn) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  যদি(!cf->পরবর্তী) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  CF_তথ্য_SAVE(save, cf, তথ্য);
  *done = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, তথ্য);

  যদি(!ctx->shutdown_started) {
    অক্ষর বাফার[NGTCP2_MAX_UDP_PAYLOAD_আকার];
    ngtcp2_ssize nwritten;

    যদি(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
      CURL_TRC_CF(তথ্য, cf, "shutdown, flushing sendbuf");
      ফলাফল = Curl_cf_ngtcp2_progress_egress(cf, তথ্য, &pktx);
      যদি(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
        CURL_TRC_CF(তথ্য, cf, "sending shutdown packets blocked");
        ফলাফল = CURLE_OK;
        যাও out;
      }
      অন্যথা যদি(ফলাফল) {
        CURL_TRC_CF(তথ্য, cf, "shutdown, error %d flushing sendbuf",
                    (পূর্ণসংখ্যা)ফলাফল);
        *done = TRUE;
        যাও out;
      }
    }

    DEBUGASSERT(Curl_bufq_is_empty(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, ফাঁকা, /* path */
      ফাঁকা, /* pkt_info */
      (uint8_t *)বাফার, আকার(বাফার),
      &ctx->last_error, pktx.ts);
    CURL_TRC_CF(তথ্য, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (পূর্ণসংখ্যা)ctx->last_error.type,
                ctx->last_error.error_code, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    যদি(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      ফলাফল = Curl_bufq_write(&ctx->q.sendbuf, (ধ্রুবক unsigned অক্ষর *)বাফার,
                               (size_t)nwritten, &n);
      যদি(ফলাফল) {
        CURL_TRC_CF(তথ্য, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (পূর্ণসংখ্যা)ফলাফল);
        যাও out;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.split_len = 0;
    }
  }

  যদি(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(তথ্য, cf, "shutdown, flushing egress");
    ফলাফল = Curl_vquic_flush(cf, তথ্য, &ctx->q);
    যদি(ফলাফল == CURLE_AGAIN) {
      CURL_TRC_CF(তথ্য, cf, "sending shutdown packets blocked");
      ফলাফল = CURLE_OK;
      যাও out;
    }
    অন্যথা যদি(ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "shutdown, error %d flushing sendbuf",
                  (পূর্ণসংখ্যা)ফলাফল);
      *done = TRUE;
      যাও out;
    }
  }

  যদি(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(তথ্য, cf, "shutdown completely sent off, done");
    *done = TRUE;
    ফলাফল = CURLE_OK;
  }
out:
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

শূন্য Curl_cf_ngtcp2_cmn_conn_close(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  bool done;
  Curl_cf_ngtcp2_cmn_shutdown(cf, তথ্য, &done);
}

স্থির bool cf_ngtcp2_err_is_fatal(পূর্ণসংখ্যা code)
{
  ফেরত (NGTCP2_ERR_FATAL >= code) ||
         (NGTCP2_ERR_DROP_CONN == code) ||
         (NGTCP2_ERR_IDLE_CLOSE == code);
}

শূন্য Curl_cf_ngtcp2_cmn_err_set(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা code)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  যদি(!ctx->last_error.error_code) {
    যদি(NGTCP2_ERR_CRYPTO == code) {
      ngtcp2_ccerr_set_tls_alert(&ctx->last_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 ফাঁকা, 0);
    }
    অন্যথা {
      ngtcp2_ccerr_set_liberr(&ctx->last_error, code, ফাঁকা, 0);
    }
  }
  যদি(cf_ngtcp2_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, তথ্য);
}

শূন্য Curl_cf_ngtcp2_io_ctx_init(কাঠামো cf_ngtcp2_io_ctx *io_ctx,
                                কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->তথ্য = তথ্য;
  io_ctx->now = *Curl_pgrs_now(তথ্য);
  ngtcp2_path_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_set_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

শূন্য Curl_cf_ngtcp2_io_ctx_update_time(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো cf_ngtcp2_io_ctx *io_ctx,
                                       কাঠামো Curl_cfilter *cf)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(তথ্য);
  Curl_vquic_ctx_update_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#যদি NGTCP2_VERSION_অঙ্ক < 0x011100
কাঠামো cf_ngtcp2_sfind_ctx {
  int64_t stream_id;
  কাঠামো h3_stream_ctx *stream;
  uint32_t mid;
};

স্থির bool cf_ngtcp2_sfind(uint32_t mid, শূন্য *মান, শূন্য *user_তথ্য)
{
  কাঠামো cf_ngtcp2_sfind_ctx *fctx = user_তথ্য;
  কাঠামো h3_stream_ctx *stream = মান;

  যদি(fctx->stream_id == stream->id) {
    fctx->mid = mid;
    fctx->stream = stream;
    ফেরত FALSE;
  }
  ফেরত TRUE; /* continue */
}

স্থির কাঠামো h3_stream_ctx *cf_ngtcp2_get_stream(কাঠামো cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  কাঠামো cf_ngtcp2_sfind_ctx fctx;
  fctx.stream_id = stream_id;
  fctx.stream = ফাঁকা;
  Curl_u32_ptrset_visit(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  ফেরত fctx.stream;
}
#অন্যথা
স্থির কাঠামো h3_stream_ctx *cf_ngtcp2_get_stream(কাঠামো cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  ফেরত ngtcp2_conn_get_stream_user_তথ্য(ctx->qconn, stream_id);
}
#endif

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
স্থির CURLcode read_pkt_to_send(শূন্য *userp,
                                 unsigned অক্ষর *buf, size_t buflen,
                                 size_t *pnread)
{
  কাঠামো cf_ngtcp2_io_ctx *x = userp;
  কাঠামো cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t stream_id;
  পূর্ণসংখ্যা fin;
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
  জন্য(;;) {

    যদি(ctx->h3conn && ngtcp2_conn_get_max_তথ্য_বাম(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_stream(ctx->h3conn, &stream_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      যদি(veccnt < 0) {
        failf(x->তথ্য, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((পূর্ণসংখ্যা)veccnt));
        Curl_cf_ngtcp2_h3_err_set(x->cf, x->তথ্য, (পূর্ণসংখ্যা)veccnt);
        ফেরত CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_STREAM_FLAG_MORE |
            (fin ? NGTCP2_WRITE_STREAM_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_stream(ctx->qconn, &x->ps.path,
                                  ফাঁকা, buf, buflen,
                                  &ndatalen, flags, stream_id,
                                  (ধ্রুবক ngtcp2_vec *)vec, veccnt, x->ts);
    যদি(n == 0) {
      /* nothing to send */
      ফেরত CURLE_AGAIN;
    }
    অন্যথা যদি(n < 0) {
      নির্বাচন(n) {
      ক্ষেত্র NGTCP2_ERR_STREAM_তথ্য_BLOCKED: {
        কাঠামো h3_stream_ctx *stream;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_block_stream(ctx->h3conn, stream_id);
        CURL_TRC_CF(x->তথ্য, x->cf, "[%" PRId64 "] block quic flow",
                    stream_id);
        stream = cf_ngtcp2_get_stream(ctx, stream_id);
        যদি(stream) /* it might be not one of our h3 streams? */
          stream->quic_flow_blocked = TRUE;
        n = 0;
        বিরতি;
      }
      ক্ষেত্র NGTCP2_ERR_STREAM_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_stream_write(ctx->h3conn, stream_id);
        n = 0;
        বিরতি;
      ক্ষেত্র NGTCP2_ERR_WRITE_MORE:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        বিরতি;
      পূর্বনির্ধারিত:
        DEBUGASSERT(ndatalen == -1);
        failf(x->তথ্য, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((পূর্ণসংখ্যা)n));
        Curl_cf_ngtcp2_cmn_err_set(x->cf, x->তথ্য, (পূর্ণসংখ্যা)n);
        ফেরত CURLE_SEND_ERROR;
      }
    }

    যদি(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      পূর্ণসংখ্যা rv = nghttp3_conn_add_write_offset(ctx->h3conn, stream_id, ndatalen);
      যদি(rv) {
        failf(x->তথ্য, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        ফেরত CURLE_SEND_ERROR;
      }
    }

    যদি(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      ফেরত CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_progress_egress(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        কাঠামো cf_ngtcp2_io_ctx *pktx)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_আকার, path_max_payload_আকার;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_quantum;
  CURLcode ফলাফল;
  কাঠামো cf_ngtcp2_io_ctx local_pktx;

  যদি(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, তথ্য);
    pktx = &local_pktx;
  }
  অন্যথা {
    Curl_cf_ngtcp2_io_ctx_update_time(তথ্য, pktx, cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  ফলাফল = Curl_vquic_flush(cf, তথ্য, &ctx->q);
  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_AGAIN) {
      Curl_expire_set(তথ্য, EXPIRE_QUIC, 1, &pktx->now);
      ফেরত CURLE_OK;
    }
    ফেরত ফলাফল;
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
  max_payload_আকার = ngtcp2_conn_get_max_tx_udp_payload_আকার(ctx->qconn);
  path_max_payload_আকার =
    ngtcp2_conn_get_path_max_tx_udp_payload_আকার(ctx->qconn);
  send_quantum = ngtcp2_conn_get_send_quantum(ctx->qconn);
  CURL_TRC_CF(তথ্য, cf, "egress, collect and send packets, quantum=%zu",
              send_quantum);
  জন্য(;;) {
    /* add the next packet to send, if any, to our buffer */
    ফলাফল = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_আকার,
                            read_pkt_to_send, pktx, &nread);
    যদি(ফলাফল == CURLE_AGAIN)
      বিরতি;
    অন্যথা যদি(ফলাফল)
      ফেরত ফলাফল;
    অন্যথা {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      যদি((buflen >= send_quantum) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_আকার))
        বিরতি;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      যদি(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      অন্যথা যদি(nread > gsolen ||
              (gsolen > path_max_payload_আকার && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        ফলাফল = Curl_vquic_send_লেজ_split(cf, তথ্য, &ctx->q,
                                            gsolen, nread, nread);
        যদি(ফলাফল) {
          যদি(ফলাফল == CURLE_AGAIN) {
            Curl_expire_set(তথ্য, EXPIRE_QUIC, 1, &pktx->now);
            ফেরত CURLE_OK;
          }
          ফেরত ফলাফল;
        }
        pktcnt = 0;
      }
      অন্যথা যদি(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        বিরতি;
      }
    }
  }

  যদি(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(তথ্য, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    ফলাফল = Curl_vquic_send(cf, তথ্য, &ctx->q, gsolen);
    যদি(ফলাফল) {
      যদি(ফলাফল == CURLE_AGAIN) {
        Curl_expire_set(তথ্য, EXPIRE_QUIC, 1, &pktx->now);
        ফেরত CURLE_OK;
      }
      ফেরত ফলাফল;
    }
    Curl_cf_ngtcp2_io_ctx_update_time(তথ্য, pktx, cf);
    ngtcp2_conn_update_pkt_tx_time(ctx->qconn, pktx->ts);
  }
  ফেরত CURLE_OK;
}

কাঠামো cf_ngtcp2_recv_ctx {
  কাঠামো cf_ngtcp2_io_ctx *pktx;
  size_t pkt_গুনতি;
};

স্থির CURLcode cf_ngtcp2_recv_pkts(ধ্রুবক unsigned অক্ষর *buf, size_t buflen,
                                    size_t gso_আকার,
                                    কাঠামো sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    শূন্য *userp)
{
  কাঠামো cf_ngtcp2_recv_ctx *rctx = userp;
  কাঠামো cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  কাঠামো cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_path path;
  size_t offset, pktlen;
  পূর্ণসংখ্যা rv;

  যদি(!rctx->pkt_গুনতি) {
    Curl_cf_ngtcp2_io_ctx_update_time(pktx->তথ্য, pktx, pktx->cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  যদি(ecn)
    CURL_TRC_CF(pktx->তথ্য, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_আকার, (unsigned)ecn);
  ngtcp2_addr_init(&path.local, (কাঠামো sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&path.remote, (কাঠামো sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  জন্য(offset = 0; offset < buflen; offset += gso_আকার) {
    rctx->pkt_গুনতি++;
    pktlen = ((offset + gso_আকার) <= buflen) ? gso_আকার : (buflen - offset);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &path, &pi,
                              buf + offset, pktlen, pktx->ts);
    যদি(rv) {
      CURL_TRC_CF(pktx->তথ্য, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_set(pktx->cf, pktx->তথ্য, rv);

      যদি(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        ফেরত CURLE_PEER_FAILED_VERIFICATION;
      ফেরত CURLE_RECV_ERROR;
    }
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_progress_ingress(কাঠামো Curl_cfilter *cf,
                                         কাঠামো Curl_easy *তথ্য,
                                         কাঠামো cf_ngtcp2_io_ctx *pktx)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো cf_ngtcp2_io_ctx local_pktx;
  কাঠামো cf_ngtcp2_recv_ctx rctx;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, তথ্য);
    pktx = &local_pktx;
  }

  ফলাফল = Curl_vquic_tls_before_recv(&ctx->tls, cf, তথ্য);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  rctx.pktx = pktx;
  rctx.pkt_গুনতি = 0;

  যদি(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(তথ্য, cf, "progress_ingress(socket)");
    ফেরত Curl_vquic_recv_packets(cf, তথ্য, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  অন্যথা {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    unsigned অক্ষর *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_আকার;
    size_t pkt_limit = QUIC_TUNNEL_INGRESS_PKT_LIMIT;
    size_t nread;
    কাঠামো sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(তথ্য, cf, "progress_ingress(sub-filters)");
    যদি(ctx->qconn) {
      size_t max_path_payload;
      max_path_payload =
        ngtcp2_conn_get_path_max_tx_udp_payload_আকার(ctx->qconn);
      যদি(max_path_payload > max_udp_payload)
        max_udp_payload = max_path_payload;
    }

    যদি(ctx->tunnel_inbuf_len < max_udp_payload) {
      unsigned অক্ষর *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      যদি(!newbuf)
        ফেরত CURLE_OUT_OF_MEMORY;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    যখন(pkt_limit--) {
      ফলাফল = Curl_conn_cf_recv(cf->পরবর্তী, তথ্য, (অক্ষর *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      যদি(ফলাফল == CURLE_AGAIN) {
        /* no more data available at the moment */
        ফেরত CURLE_OK;
      }
      যদি(ফলাফল) {
        CURL_TRC_CF(তথ্য, cf, "ingress, recv from tunnel failed: %d",
                    (পূর্ণসংখ্যা)ফলাফল);
        ফেরত ফলাফল;
      }
      যদি(nread == 0) {
        /* tunnel closed */
        ফেরত CURLE_OK;
      }

      memcpy(&remote_addr, ctx->connected_path.remote.addr,
             ctx->connected_path.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_path.remote.addrlen;
      ফলাফল = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      যদি(!ctx->q.got_first_byte) {
        ctx->q.got_first_byte = TRUE;
        ctx->q.first_byte_at = ctx->q.last_op;
      }
      ctx->q.last_io = ctx->q.last_op;
    }
    ফেরত CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_set_expiry(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য,
                                       কাঠামো cf_ngtcp2_io_ctx *pktx)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  যদি(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, তথ্য);
    pktx = &local_pktx;
  }
  অন্যথা {
    Curl_cf_ngtcp2_io_ctx_update_time(তথ্য, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  যদি(expiry != UINT64_MAX) {
    যদি(expiry <= pktx->ts) {
      CURLcode ফলাফল;
      পূর্ণসংখ্যা rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      যদি(rv) {
        failf(তথ্য, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_set(cf, তথ্য, rv);
        ফেরত CURLE_SEND_ERROR;
      }
      ফলাফল = Curl_cf_ngtcp2_progress_ingress(cf, তথ্য, pktx);
      যদি(!ফলাফল)
        ফলাফল = Curl_cf_ngtcp2_progress_egress(cf, তথ্য, pktx);
      যদি(ফলাফল) {
        /* a verify failure during ingress must win over generic errors */
        যদি(ctx->tls_vrfy_ফলাফল)
          ফলাফল = ctx->tls_vrfy_ফলাফল;
        ফেরত ফলাফল;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    যদি(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      যদি(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_set(তথ্য, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  ফেরত CURLE_OK;
}

স্থির শূন্য cf_ngtcp2_setup_keep_alive(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  ধ্রুবক ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  যদি(!ctx->qconn)
    ফেরত;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  যদি(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(তথ্য, cf, "no peer idle timeout, unset keep-alive");
  }
  অন্যথা যদি(!Curl_u32_ptrset_গুনতি(&ctx->streams)) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(তথ্য, cf, "no active streams, unset keep-alive");
  }
  অন্যথা {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(তথ্য, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_stream_setup(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        কাঠামো h3_stream_ctx **pstream)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);

  *pstream = ফাঁকা;
  যদি(!তথ্য)
    ফেরত CURLE_FAILED_INIT;

  যদি(stream) {
    *pstream = stream;
    ফেরত CURLE_OK;
  }

  stream = curlx_calloc(1, আকার(*stream));
  যদি(!stream)
    ফেরত CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  stream->xfer_id = তথ্য->id;
  stream->mid = তথ্য->mid;
  stream->rx_offset = 0;
  stream->rx_offset_max = H3_STREAM_WINDOW_আকার_INITIAL;
  stream->tx_in_flight_ideal = H3_STREAM_SEND_BUF_INITIAL;

  /* on send, we control how much we put into the buffer */
  Curl_bufq_initp(&stream->sendbuf, &ctx->stream_bufcp,
                  H3_STREAM_SEND_CHUNKS, BUFQ_OPT_NONE);
  stream->tx_in_flight_আকার = 0;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_সারি_LEN);

  যদি(Curl_u32_ptrset_set(&ctx->streams, তথ্য->mid, stream)) {
    Curl_cf_ngtcp2_h3_stream_ctx_free(stream);
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  যদি(Curl_u32_ptrset_গুনতি(&ctx->streams) == 1)
    cf_ngtcp2_setup_keep_alive(cf, তথ্য);

  *pstream = stream;
  ফেরত CURLE_OK;
}

শূন্য Curl_cf_ngtcp2_h3_stream_close(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    কাঠামো h3_stream_ctx *stream)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(stream);
  যদি(!stream->closed && ctx->qconn && ctx->h3conn) {
    CURLcode ফলাফল;

    nghttp3_conn_set_stream_user_তথ্য(ctx->h3conn, stream->id, ফাঁকা);
    ngtcp2_conn_set_stream_user_তথ্য(ctx->qconn, stream->id, ফাঁকা);
    stream->closed = TRUE;
    (শূন্য)ngtcp2_conn_shutdown_stream(ctx->qconn, 0, stream->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    ফলাফল = Curl_cf_ngtcp2_progress_egress(cf, তথ্য, ফাঁকা);
    যদি(ফলাফল)
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] cancel stream -> %d",
                  stream->id, (পূর্ণসংখ্যা)ফলাফল);
  }
}

শূন্য Curl_cf_ngtcp2_h3_stream_done(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);
  (শূন্য)cf;
  যদি(stream) {
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] easy handle is done", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, তথ্য, stream);
    Curl_u32_ptrset_unset(&ctx->streams, তথ্য->mid);
    যদি(!Curl_u32_ptrset_গুনতি(&ctx->streams))
      cf_ngtcp2_setup_keep_alive(cf, তথ্য);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      bool *প্রবেশ_pending)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ধ্রুবক ngtcp2_transport_params *rp;
  কাঠামো cf_call_তথ্য save;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  *প্রবেশ_pending = FALSE;
  যদি(!ctx->qconn || ctx->shutdown_started)
    যাও out;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  যদি(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(তথ্য), &ctx->q.last_io);
    যদি(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      যদি((uint64_t)idletime_ms > max_idle_ms)
        যাও out;
    }
  }

  যদি(!cf->পরবর্তী || !cf->পরবর্তী->cft->is_alive(cf->পরবর্তী, তথ্য, প্রবেশ_pending))
    যাও out;

  alive = TRUE;
  যদি(*প্রবেশ_pending) {
    CURLcode ফলাফল;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *প্রবেশ_pending = FALSE;
    ফলাফল = Curl_cf_ngtcp2_progress_ingress(cf, তথ্য, ফাঁকা);
    CURL_TRC_CF(তথ্য, cf, "is_alive, progress ingress -> %d", (পূর্ণসংখ্যা)ফলাফল);
    alive = ফলাফল ? FALSE : TRUE;
  }

out:
  CF_তথ্য_RESTORE(cf, save);
  ফেরত alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(কাঠামো cf_ngtcp2_ctx *ctx,
                                      কাঠামো Curl_easy *তথ্য)
{
  int64_t ctrl_stream_id, qpack_enc_stream_id, qpack_dec_stream_id;
  পূর্ণসংখ্যা rc;

  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &ctrl_stream_id, ফাঁকা);
  যদি(rc) {
    failf(তথ্য, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    ফেরত CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_stream(ctx->h3conn, ctrl_stream_id);
  যদি(rc) {
    failf(তথ্য, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    ফেরত CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_enc_stream_id, ফাঁকা);
  যদি(rc) {
    failf(তথ্য, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    ফেরত CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_dec_stream_id, ফাঁকা);
  যদি(rc) {
    failf(তথ্য, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    ফেরত CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_stream_id,
                                       qpack_dec_stream_id);
  যদি(rc) {
    failf(তথ্য, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    ফেরত CURLE_QUIC_CONNECT_ERROR;
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;

  নির্বাচন(query) {
  ক্ষেত্র CF_QUERY_CONNECT_REPLY_MS:
    যদি((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (পূর্ণসংখ্যা)ms : INT_MAX;
      ফেরত CURLE_OK;
    }
    বিরতি;
  ক্ষেত্র CF_QUERY_REALLY_CONNECTED:
    যদি(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_first_byte;
      ফেরত CURLE_OK;
    }
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)arg1;
  (শূন্য)arg2;
  নির্বাচন(event) {
  ক্ষেত্র CF_CTRL_REPORT_STATS:
    যদি(cf->connected && !ctx->stats_reported) {
      যদি((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
        Curl_pgrsTimeWas(তথ্য, TIMER_CONNECT, ctx->q.first_byte_at);
        ctx->stats_reported = TRUE;
      }
      অন্যথা যদি(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        যদি(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrsTimeWas(তথ্য, TIMER_CONNECT, ctx->q.first_byte_at);
        Curl_pgrsTimeWas(তথ্য, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত ফলাফল;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
