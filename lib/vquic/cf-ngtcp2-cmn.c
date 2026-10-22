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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#అన్తర్భావ <ngtcp2/ngtcp2.h>

#నిర్వచితమైతే USE_OPENSSL
#అన్తర్భావ <openssl/err.h>
#ఒకవేళ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#అన్తర్భావ <ngtcp2/ngtcp2_crypto_boringssl.h>
#లేదావేళ defined(OPENSSL_QUIC_API2)
#అన్తర్భావ <ngtcp2/ngtcp2_crypto_ossl.h>
#లేకపోతే
#అన్తర్భావ <ngtcp2/ngtcp2_crypto_quictls.h>
#అయితే_ముగింపు
#అన్తర్భావ "vtls/openssl.h"
#లేదావేళ defined(USE_GNUTLS)
#అన్తర్భావ <ngtcp2/ngtcp2_crypto_gnutls.h>
#అన్తర్భావ "vtls/gtls.h"
#లేదావేళ defined(USE_WOLFSSL)
#అన్తర్భావ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#అన్తర్భావ "vtls/wolfssl.h"
#అయితే_ముగింపు

#అన్తర్భావ <nghttp3/nghttp3.h>

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "u32_ptrset.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cf-socket.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "curlx/win32-fopen.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "http1.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "sockaddr.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "vdns/cf-dns.h"
#అన్తర్భావ "vquic/vquic.h"
#అన్తర్భావ "vquic/vquic_int.h"
#అన్తర్భావ "vquic/vquic-tls.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/vtls_scache.h"
#అన్తర్భావ "vquic/cf-ngtcp2-cmn.h"

/*
 * Store ngtcp2 version info in this buffer.
 */
శూన్య Curl_ngtcp2_ver(అక్షరం *p, size_t len)
{
  స్థిరరాశి ngtcp2_info *ng2 = ngtcp2_సంస్కరణ(0);
  స్థిరరాశి nghttp3_info *ht3 = nghttp3_సంస్కరణ(0);
  (శూన్య)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->సంస్కరణ_str, ht3->సంస్కరణ_str);
}

శూన్య Curl_cf_ngtcp2_h3_ప్రవాహం_ctx_free(నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం)
{
  Curl_bufq_free(&ప్రవాహం->sendbuf);
  Curl_h1_req_parse_free(&ప్రవాహం->h1);
  curlx_free(ప్రవాహం);
}

స్థిర శూన్య h3_ప్రవాహం_సమితి_free(uint32_t id, శూన్య *ప్రవాహం)
{
  (శూన్య)id;
  DEBUGASSERT(ప్రవాహం);
  Curl_cf_ngtcp2_h3_ప్రవాహం_ctx_free((నిర్మాణం h3_ప్రవాహం_ctx *)ప్రవాహం);
}

స్థిర bool cf_ngtcp2_h3_err_is_fatal(పూర్ణాంకం సంకేతం_2)
{
  తిరిగి (NGHTTP3_ERR_FATAL >= సంకేతం_2) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_ప్రవాహం == సంకేతం_2);
}

శూన్య Curl_cf_ngtcp2_h3_err_సమితి(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం సంకేతం_2)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  ఒకవేళ(!ctx->చివరి_error.error_సంకేతం) {
    ngtcp2_ccerr_సమితి_application_error(&ctx->చివరి_error,
      nghttp3_err_infer_quic_app_error_సంకేతం(సంకేతం_2), ఖాళీ, 0);
  }
  ఒకవేళ(cf_ngtcp2_h3_err_is_fatal(సంకేతం_2))
    Curl_cf_ngtcp2_cmn_conn_close(cf, దత్తాంశం);
}

CURLcode Curl_cf_ngtcp2_ctx_init(నిర్మాణం cf_ngtcp2_ctx *ctx,
                                 నిర్మాణం Curl_peer *origin,
                                 నిర్మాణం Curl_peer *peer,
                                 నిర్మాణం ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = ఖాళీ;
  ctx->tunnel_inbuf_len = 0;
  ctx->సంస్కరణ_2 = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->ప్రవాహం_bufcp, H3_ప్రవాహం_CHUNK_కొలత,
                  H3_ప్రవాహం_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_శీర్షిక);
  Curl_u32_ptrset_init(&ctx->streams, h3_ప్రవాహం_సమితి_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  తిరిగి Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

శూన్య Curl_cf_ngtcp2_ctx_cleanup(నిర్మాణం cf_ngtcp2_ctx *ctx)
{
  ఒకవేళ(ctx && ctx->initialized) {
    ఒకవేళ(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    ఒకవేళ(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = ఖాళీ;
    }
    ఒకవేళ(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = ఖాళీ;
    }
#నిర్వచితమైతే OPENSSL_QUIC_API2
    ఒకవేళ(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = ఖాళీ;
    }
#అయితే_ముగింపు
    Curl_bufcp_free(&ctx->ప్రవాహం_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    curlx_safefree(ctx->tunnel_inbuf);
    ctx->tunnel_inbuf_len = 0;
  }
}

నిర్మాణం Curl_easy *Curl_cf_ngtcp2_get_xfer(నిర్మాణం Curl_cfilter *cf,
                                          శూన్య *ప్రవాహం_user_దత్తాంశం,
                                          నిర్మాణం h3_ప్రవాహం_ctx **pstream)
{
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = ప్రవాహం_user_దత్తాంశం;

  ఒకవేళ(pstream)
    *pstream = ప్రవాహం;
  ఒకవేళ(ప్రవాహం && (ప్రవాహం->mid != UINT32_MAX)) {
    నిర్మాణం Curl_easy *దత్తాంశం, *call_దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
    ఒకవేళ(!call_దత్తాంశం || !call_దత్తాంశం->multi) {
      DEBUGASSERT(0);
      తిరిగి ఖాళీ;
    }
    దత్తాంశం = Curl_multi_get_easy(call_దత్తాంశం->multi, ప్రవాహం->mid);
    ఒకవేళ(దత్తాంశం && (దత్తాంశం->id == ప్రవాహం->xfer_id))
      తిరిగి దత్తాంశం;
    DEBUGASSERT(0);
  }
  తిరిగి ఖాళీ;
}

స్థిర ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  నిర్మాణం Curl_cfilter *cf = conn_ref->user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  తిరిగి ctx->qconn;
}

#నిర్వచితమైతే DEBUG_NGTCP2
స్థిర శూన్య quic_printf(శూన్య *user_దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  va_list ap;
  (శూన్య)user_దత్తాంశం;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#అయితే_ముగింపు

స్థిర శూన్య qlog_callback(శూన్య *user_దత్తాంశం, uint32_t flags,
                          స్థిరరాశి శూన్య *దత్తాంశం, size_t datalen)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  (శూన్య)flags;
  ఒకవేళ(ctx->qlogfd != -1) {
    ssize_t rc = లిఖ(ctx->qlogfd, దత్తాంశం, datalen);
    ఒకవేళ(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

స్థిర శూన్య quic_settings(నిర్మాణం cf_ngtcp2_ctx *ctx,
                          నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#నిర్వచితమైతే DEBUG_NGTCP2
  s->log_printf = quic_printf;
#లేకపోతే
  s->log_printf = ఖాళీ;
#అయితే_ముగింపు

  s->initial_ts = pktx->ts;
  s->handshake_timeout = (దత్తాంశం->సమితి.connecttimeout > 0) ?
    దత్తాంశం->సమితి.connecttimeout * NGTCP2_MILLISECONDS : QUIC_HANDSHAKE_TIMEOUT;
  s->max_కిటికీ = H3_CONN_కిటికీ_కొలత_MAX;
  s->max_ప్రవాహం_కిటికీ = 0; /* disable ngtcp2 auto-tuning of window */
  s->no_pmtud = FALSE;
#నిర్వచితమైతే NGTCP2_SETTINGS_V3
  /* try ten times the ngtcp2 defaults here for problems with Caddy */
  s->glitch_ratelim_వ్యవధి = 1000 * 10;
  s->glitch_ratelim_రేటు = 33 * 10;
#అయితే_ముగింపు
  t->initial_max_దత్తాంశం = s->max_కిటికీ;
  t->initial_max_ప్రవాహం_దత్తాంశం_bidi_local = H3_ప్రవాహం_కిటికీ_కొలత_INITIAL;
  t->initial_max_ప్రవాహం_దత్తాంశం_bidi_remote = H3_ప్రవాహం_కిటికీ_కొలత_INITIAL;
  t->initial_max_ప్రవాహం_దత్తాంశం_uni = t->initial_max_దత్తాంశం;
  t->initial_max_streams_bidi = QUIC_MAX_STREAMS;
  t->initial_max_streams_uni = QUIC_MAX_STREAMS;
  t->max_idle_timeout = 0; /* no idle timeout from our side */
  ఒకవేళ(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#ఒకవేళ defined(_MSC_VER) && defined(_DLL)
#సూచన warning(నెట్టు)
#సూచన warning(disable:4232) /* MSVC extension, dllimport identity */
#అయితే_ముగింపు

స్థిర పూర్ణాంకం cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, శూన్య *user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ఖాళీ;
  నిర్మాణం Curl_easy *దత్తాంశం;

  (శూన్య)tconn;
  DEBUGASSERT(ctx);
  దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  DEBUGASSERT(దత్తాంశం);
  ఒకవేళ(!ctx || !దత్తాంశం)
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(దత్తాంశం);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, దత్తాంశం);

  ctx->tls_vrfy_ఫలితం = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    దత్తాంశం, &ctx->ssl_peer);
  ఒకవేళ(ctx->tls_vrfy_ఫలితం)
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;

#నిర్వచితమైతే CURLVERBOSE
  ఒకవేళ(Curl_trc_is_verbose(దత్తాంశం)) {
    స్థిరరాశి ngtcp2_transport_params *rp;
    rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
    CURL_TRC_CF(దత్తాంశం, cf, "handshake complete after %" FMT_TIMEDIFF_T
                "ms, remote transport[max_udp_payload=%" PRIu64
                ", initial_max_data=%" PRIu64 "]",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at),
                rp->max_udp_payload_కొలత, rp->initial_max_దత్తాంశం);
  }
#అయితే_ముగింపు

  /* In case of earlydata, where we simulate being connected, update
   * the handshake time when we really did connect */
  ఒకవేళ(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrs_సమయం_Was(దత్తాంశం, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  ఒకవేళ(ctx->use_earlydata) {
#ఒకవేళ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    ఒకవేళ(SSL_get_early_దత్తాంశం_స్థితి(ctx->tls.ossl.ssl) == SSL_EARLY_దత్తాంశం_NOT_పంపిన)
      CURL_TRC_CF(దత్తాంశం, cf, "OpenSSL did not send early data");
#అయితే_ముగింపు

#ఒకవేళ NGTCP2_సంస్కరణ_అంకె >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_దత్తాంశం_rejected2(ctx->qconn);
#లేకపోతే /* older NGTCP2 */
    {
#ఒకవేళ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      పూర్ణాంకం ossl_early_స్థితి = SSL_get_early_దత్తాంశం_స్థితి(ctx->tls.ossl.ssl);
      ఒకవేళ(ossl_early_స్థితి == SSL_EARLY_దత్తాంశం_NOT_పంపిన)
        CURL_TRC_CF(దత్తాంశం, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_స్థితి == SSL_EARLY_దత్తాంశం_ACCEPTED);
#లేదావేళ defined(USE_GNUTLS)
      పూర్ణాంకం flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_దత్తాంశం);
#లేదావేళ defined(USE_WOLFSSL)
#నిర్వచితమైతే WOLFSSL_EARLY_దత్తాంశం
      ctx->earlydata_accepted =
        (wolf_SSL_get_early_దత్తాంశం_స్థితి(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_దత్తాంశం_REJECTED);
#లేకపోతే
      DEBUGASSERT(0); /* should not come here if ED is disabled. */
      ctx->earlydata_accepted = FALSE;
#అయితే_ముగింపు /* WOLFSSL_EARLY_DATA */
#అయితే_ముగింపు /* OPENSSL or GNUTLS or WOLFSSL */
    }
#అయితే_ముగింపు /* older NGTCP2 */
    CURL_TRC_CF(దత్తాంశం, cf, "server did%s accept %zu bytes of early data",
                ctx->earlydata_accepted ? "" : " not", ctx->earlydata_skip);
    Curl_pgrs_Early_దత్తాంశం(దత్తాంశం, ctx->earlydata_accepted ?
                              (curl_off_t)ctx->earlydata_skip :
                             -(curl_off_t)ctx->earlydata_skip);
  }
  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_recv_ప్రవాహం_దత్తాంశం(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t ప్రవాహం_id, uint64_t స్థానభ్రంశం,
                               స్థిరరాశి uint8_t *buf, size_t buflen,
                               శూన్య *user_దత్తాంశం, శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  పూర్ణాంకం fin = (flags & NGTCP2_ప్రవాహం_దత్తాంశం_FLAG_FIN) ? 1 : 0;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  నిర్మాణం Curl_easy *దత్తాంశం =
    Curl_cf_ngtcp2_get_xfer(cf, ప్రవాహం_user_దత్తాంశం, &ప్రవాహం);

  (శూన్య)స్థానభ్రంశం;
  rc = nghttp3_conn_read_ప్రవాహం(ctx->h3conn, ప్రవాహం_id, buf, buflen, fin);
  ఒకవేళ(rc < 0) {
    ఒకవేళ(దత్తాంశం && ప్రవాహం) {
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  ప్రవాహం_id, ప్రవాహం->reset, ప్రవాహం->closed);
    }
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  ఒకవేళ(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_ప్రవాహం_స్థానభ్రంశం(tconn, ప్రవాహం_id, nconsumed);
    ngtcp2_conn_extend_max_స్థానభ్రంశం(tconn, nconsumed);
    ఒకవేళ(ప్రవాహం) {
      ప్రవాహం->rx_స్థానభ్రంశం += nconsumed;
      ప్రవాహం->rx_స్థానభ్రంశం_max += nconsumed;
    }
  }
  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_acked_ప్రవాహం_దత్తాంశం_స్థానభ్రంశం(ngtcp2_conn *tconn, int64_t ప్రవాహం_id,
                                       uint64_t స్థానభ్రంశం, uint64_t datalen,
                                       శూన్య *user_దత్తాంశం, శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  పూర్ణాంకం rv;
  (శూన్య)ప్రవాహం_id;
  (శూన్య)tconn;
  (శూన్య)స్థానభ్రంశం;
  (శూన్య)datalen;
  (శూన్య)ప్రవాహం_user_దత్తాంశం;

  rv = nghttp3_conn_కూడు_ack_స్థానభ్రంశం(ctx->h3conn, ప్రవాహం_id, datalen);
  ఒకవేళ(rv && rv != NGHTTP3_ERR_ప్రవాహం_NOT_దొరికింది) {
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;
  }

  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_ప్రవాహం_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t ప్రవాహం_id, uint64_t app_error_సంకేతం,
                           శూన్య *user_దత్తాంశం, శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = Curl_cf_ngtcp2_get_xfer(cf, ప్రవాహం_user_దత్తాంశం, ఖాళీ);
  పూర్ణాంకం rv;

  (శూన్య)tconn;
  /* stream is closed... */
  ఒకవేళ(!దత్తాంశం)
    దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  ఒకవేళ(!దత్తాంశం)
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;

  ఒకవేళ(!(flags & NGTCP2_ప్రవాహం_CLOSE_FLAG_APP_ERROR_సంకేతం_సమితి)) {
    app_error_సంకేతం = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_ప్రవాహం(ctx->h3conn, ప్రవాహం_id, app_error_సంకేతం);
  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", ప్రవాహం_id, app_error_సంకేతం, rv);
  ఒకవేళ(rv && rv != NGHTTP3_ERR_ప్రవాహం_NOT_దొరికింది) {
    Curl_cf_ngtcp2_h3_err_సమితి(cf, దత్తాంశం, rv);
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;
  }

  తిరిగి 0;
}

#నిర్వచితమైతే NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
స్థిర పూర్ణాంకం cb_ప్రవాహం_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t ప్రవాహం_id,
                           uint64_t rx_app_error_సంకేతం,
                           uint64_t tx_app_error_సంకేతం,
                           శూన్య *user_దత్తాంశం, శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = Curl_cf_ngtcp2_get_xfer(cf, ప్రవాహం_user_దత్తాంశం, ఖాళీ);
  uint64_t h3_app_error_సంకేతం = NGHTTP3_H3_NO_ERROR;
  పూర్ణాంకం rv;

  (శూన్య)tconn;
  (శూన్య)tx_app_error_సంకేతం;
  /* stream is closed... */
  ఒకవేళ(!దత్తాంశం)
    దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  ఒకవేళ(!దత్తాంశం)
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;

  ఒకవేళ(flags & NGTCP2_ప్రవాహం_CLOSE2_FLAG_RX_APP_ERROR_సంకేతం_సమితి)
    h3_app_error_సంకేతం = rx_app_error_సంకేతం;

  rv = nghttp3_conn_close_ప్రవాహం(ctx->h3conn, ప్రవాహం_id, h3_app_error_సంకేతం);
  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", ప్రవాహం_id, h3_app_error_సంకేతం, rv);
  ఒకవేళ(rv && rv != NGHTTP3_ERR_ప్రవాహం_NOT_దొరికింది) {
    Curl_cf_ngtcp2_h3_err_సమితి(cf, దత్తాంశం, rv);
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;
  }

  తిరిగి 0;
}
#అయితే_ముగింపు

స్థిర పూర్ణాంకం cb_ప్రవాహం_reset(ngtcp2_conn *tconn, int64_t ప్రవాహం_id,
                           uint64_t final_కొలత, uint64_t app_error_సంకేతం,
                           శూన్య *user_దత్తాంశం, శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = Curl_cf_ngtcp2_get_xfer(cf, ప్రవాహం_user_దత్తాంశం, ఖాళీ);
  పూర్ణాంకం rv;
  (శూన్య)tconn;
  (శూన్య)final_కొలత;
  (శూన్య)app_error_సంకేతం;

  rv = nghttp3_conn_shutdown_ప్రవాహం_read(ctx->h3conn, ప్రవాహం_id);
  ఒకవేళ(దత్తాంశం)
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] reset -> %d", ప్రవాహం_id, rv);
  ఒకవేళ(rv && rv != NGHTTP3_ERR_ప్రవాహం_NOT_దొరికింది) {
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;
  }

  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_ప్రవాహం_stop_sending(ngtcp2_conn *tconn, int64_t ప్రవాహం_id,
                                  uint64_t app_error_సంకేతం, శూన్య *user_దత్తాంశం,
                                  శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  పూర్ణాంకం rv;
  (శూన్య)tconn;
  (శూన్య)app_error_సంకేతం;
  (శూన్య)ప్రవాహం_user_దత్తాంశం;

  rv = nghttp3_conn_shutdown_ప్రవాహం_read(ctx->h3conn, ప్రవాహం_id);
  ఒకవేళ(rv && rv != NGHTTP3_ERR_ప్రవాహం_NOT_దొరికింది) {
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;
  }

  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            శూన్య *user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);

  (శూన్య)tconn;
  ctx->max_bidi_streams = max_streams;
  ఒకవేళ(దత్తాంశం)
    CURL_TRC_CF(దత్తాంశం, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->వాడిన_bidi_streams);
  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_extend_max_ప్రవాహం_దత్తాంశం(ngtcp2_conn *tconn, int64_t ప్రవాహం_id,
                                     uint64_t max_దత్తాంశం, శూన్య *user_దత్తాంశం,
                                     శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  నిర్మాణం Curl_easy *దత్తాంశం =
    Curl_cf_ngtcp2_get_xfer(cf, ప్రవాహం_user_దత్తాంశం, &ప్రవాహం);
  పూర్ణాంకం rv;
  (శూన్య)tconn;
  (శూన్య)max_దత్తాంశం;

  rv = nghttp3_conn_unblock_ప్రవాహం(ctx->h3conn, ప్రవాహం_id);
  ఒకవేళ(rv && rv != NGHTTP3_ERR_ప్రవాహం_NOT_దొరికింది) {
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;
  }
  ఒకవేళ(ప్రవాహం && ప్రవాహం->quic_flow_blocked) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] unblock quic flow", ప్రవాహం_id);
    ప్రవాహం->quic_flow_blocked = FALSE;
    ఒకవేళ(దత్తాంశం)
      Curl_multi_mark_dirty(దత్తాంశం);
  }
  తిరిగి 0;
}

స్థిర శూన్య cb_rand(uint8_t *dest, size_t destlen,
                    స్థిరరాశి ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode ఫలితం;
  (శూన్య)rand_ctx;

  ఫలితం = Curl_rand(ఖాళీ, dest, destlen);
  ఒకవేళ(ఫలితం) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
స్థిర పూర్ణాంకం cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    శూన్య *user_దత్తాంశం)
{
  CURLcode ఫలితం;
  (శూన్య)tconn;
  (శూన్య)user_దత్తాంశం;

  ఫలితం = Curl_rand(ఖాళీ, cid->దత్తాంశం, cidlen);
  ఒకవేళ(ఫలితం)
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  ఫలితం = Curl_rand(ఖాళీ, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  ఒకవేళ(ఫలితం)
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;

  తిరిగి 0;
}

#నిర్వచితమైతే NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
స్థిర పూర్ణాంకం cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  నిర్మాణం ngtcp2_stateless_reset_token *token, size_t cidlen, శూన్య *user_దత్తాంశం)
{
  CURLcode ఫలితం;
  (శూన్య)tconn;
  (శూన్య)user_దత్తాంశం;

  ఫలితం = Curl_rand(ఖాళీ, cid->దత్తాంశం, cidlen);
  ఒకవేళ(ఫలితం)
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  ఫలితం = Curl_rand(ఖాళీ, token->దత్తాంశం, పరిమాణం(token->దత్తాంశం));
  ఒకవేళ(ఫలితం)
    తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;

  తిరిగి 0;
}
#అయితే_ముగింపు

స్థిర పూర్ణాంకం cb_recv_rx_కీలకం(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          శూన్య *user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ఖాళీ;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  (శూన్య)tconn;

  ఒకవేళ(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    తిరిగి 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(దత్తాంశం);
  ఒకవేళ(ctx && దత్తాంశం && !ctx->h3conn && ctx->init_h3_conn_cb) {
    ఒకవేళ(ctx->init_h3_conn_cb(cf, దత్తాంశం, ctx))
      తిరిగి NGTCP2_ERR_CALLBACK_FAILURE;
  }
  తిరిగి 0;
}

#నిర్వచితమైతే CURL_HAVE_DIAG
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wmissing-field-initializers"
#అయితే_ముగింపు
స్థిర ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  ఖాళీ, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_దత్తాంశం_cb,
  cb_ngtcp2_handshake_completed,
  ఖాళీ, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_ముసుగు_cb,
  cb_recv_ప్రవాహం_దత్తాంశం,
  cb_acked_ప్రవాహం_దత్తాంశం_స్థానభ్రంశం,
  ఖాళీ, /* stream_open */
  cb_ప్రవాహం_close,
  ఖాళీ, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  ఖాళీ, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  ఖాళీ, /* remove_connection_id */
  ngtcp2_crypto_update_కీలకం_cb, /* update_key */
  ఖాళీ, /* path_validation */
  ఖాళీ, /* select_preferred_addr */
  cb_ప్రవాహం_reset,
  ఖాళీ, /* extend_max_remote_streams_bidi */
  ఖాళీ, /* extend_max_remote_streams_uni */
  cb_extend_max_ప్రవాహం_దత్తాంశం,
  ఖాళీ, /* dcid_status */
  ఖాళీ, /* handshake_confirmed */
  ఖాళీ, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  ఖాళీ, /* recv_datagram */
  ఖాళీ, /* ack_datagram */
  ఖాళీ, /* lost_datagram */
  ngtcp2_crypto_get_మార్గం_challenge_దత్తాంశం_cb,
  cb_ప్రవాహం_stop_sending,
  ఖాళీ, /* version_negotiation */
  cb_recv_rx_కీలకం,
  ఖాళీ, /* recv_tx_key */
  ఖాళీ, /* early_data_rejected */
#నిర్వచితమైతే NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  ఖాళీ, /* begin_path_validation */
#అయితే_ముగింపు
#నిర్వచితమైతే NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  ఖాళీ, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  ఖాళీ, /* dcid_status2 */
  ngtcp2_crypto_get_మార్గం_challenge_data2_cb, /* get_path_challenge_data2 */
#అయితే_ముగింపు
#నిర్వచితమైతే NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  ఖాళీ, /* recv_stop_sending */
#అయితే_ముగింపు
#నిర్వచితమైతే NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_ప్రవాహం_close2, /* is called instead of cb_stream_close when set */
#అయితే_ముగింపు
};
#నిర్వచితమైతే CURL_HAVE_DIAG
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు

#ఒకవేళ defined(_MSC_VER) && defined(_DLL)
#సూచన warning(తీయి)
#అయితే_ముగింపు

#నిర్వచితమైతే USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
స్థిర పూర్ణాంకం quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం cf_ngtcp2_ctx *ctx;
  నిర్మాణం Curl_easy *దత్తాంశం;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_దత్తాంశం(ssl);
  cf = cref ? cref->user_దత్తాంశం : ఖాళీ;
  ctx = cf ? cf->ctx : ఖాళీ;
  దత్తాంశం = cf ? CF_దత్తాంశం_CURRENT(cf) : ఖాళీ;
  ఒకవేళ(cf && దత్తాంశం && ctx) {
    గుర్తులేని అక్షరం *quic_tp = ఖాళీ;
    size_t quic_tp_len = 0;
#నిర్వచితమైతే HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     పరిమాణం(tpbuf));
    ఒకవేళ(tplen < 0)
      CURL_TRC_CF(దత్తాంశం, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((పూర్ణాంకం)tplen));
    లేకపోతే {
      quic_tp = (గుర్తులేని అక్షరం *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#అయితే_ముగింపు
    Curl_ossl_కూడు_session(cf, దత్తాంశం, &ctx->tls.ossl, ctx->ssl_peer.scache_కీలకం,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, ఖాళీ);
  }
  తిరిగి 0;
}
#అయితే_ముగింపు /* USE_OPENSSL */

#నిర్వచితమైతే USE_GNUTLS

#నిర్వచితమైతే CURLVERBOSE
స్థిర స్థిరరాశి అక్షరం *gtls_hs_సందేశం_name(పూర్ణాంకం mtype)
{
  ఎంపిక(mtype) {
  స్థితి 1:
    తిరిగి "ClientHello";
  స్థితి 2:
    తిరిగి "ServerHello";
  స్థితి 4:
    తిరిగి "SessionTicket";
  స్థితి 8:
    తిరిగి "EncryptedExtensions";
  స్థితి 11:
    తిరిగి "Certificate";
  స్థితి 13:
    తిరిగి "CertificateRequest";
  స్థితి 15:
    తిరిగి "CertificateVerify";
  స్థితి 20:
    తిరిగి "Finished";
  స్థితి 24:
    తిరిగి "KeyUpdate";
  స్థితి 254:
    తిరిగి "MessageHash";
  }
  తిరిగి "Unknown";
}
#అయితే_ముగింపు

స్థిర పూర్ణాంకం quic_gtls_handshake_cb(gnutls_session_t session, గుర్తులేని పూర్ణాంకం htype,
                                  గుర్తులేని when, గుర్తులేని పూర్ణాంకం incoming,
                                  స్థిరరాశి gnutls_datum_t *సందేశం)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  నిర్మాణం Curl_cfilter *cf = conn_ref ? conn_ref->user_దత్తాంశం : ఖాళీ;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf ? cf->ctx : ఖాళీ;

  (శూన్య)సందేశం;
  (శూన్య)incoming;
  ఒకవేళ(when && cf && ctx) { /* after message has been processed */
    నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
    DEBUGASSERT(దత్తాంశం);
    ఒకవేళ(!దత్తాంశం)
      తిరిగి 0;
    CURL_TRC_CF(దత్తాంశం, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_సందేశం_name(htype), htype);
    ఎంపిక(htype) {
    స్థితి GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      గుర్తులేని అక్షరం *quic_tp = ఖాళీ;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       పరిమాణం(tpbuf));
      ఒకవేళ(tplen < 0)
        CURL_TRC_CF(దత్తాంశం, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((పూర్ణాంకం)tplen));
      లేకపోతే {
        quic_tp = (గుర్తులేని అక్షరం *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (శూన్య)Curl_gtls_cache_session(cf, దత్తాంశం, ctx->ssl_peer.scache_కీలకం,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    ఖాళీ);
      ఆపు;
    }
    అప్రమేయ:
      ఆపు;
    }
  }
  తిరిగి 0;
}
#అయితే_ముగింపు /* USE_GNUTLS */

#నిర్వచితమైతే USE_WOLFSSL
స్థిర పూర్ణాంకం wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolf_SSL_get_app_దత్తాంశం(ssl);
  నిర్మాణం Curl_cfilter *cf = conn_ref ? conn_ref->user_దత్తాంశం : ఖాళీ;

  DEBUGASSERT(cf);
  ఒకవేళ(cf && session) {
    నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
    నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
    DEBUGASSERT(దత్తాంశం);
    ఒకవేళ(దత్తాంశం && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      గుర్తులేని అక్షరం *quic_tp = ఖాళీ;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       పరిమాణం(tpbuf));
      ఒకవేళ(tplen < 0)
        CURL_TRC_CF(దత్తాంశం, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((పూర్ణాంకం)tplen));
      లేకపోతే {
        quic_tp = (గుర్తులేని అక్షరం *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (శూన్య)Curl_wssl_cache_session(cf, దత్తాంశం, ctx->ssl_peer.scache_కీలకం,
                                    session, wolf_SSL_సంస్కరణ(ssl),
                                    "h3", quic_tp, quic_tp_len, ఖాళీ);
    }
  }
  తిరిగి 0;
}
#అయితే_ముగింపు /* USE_WOLFSSL */

స్థిర CURLcode cf_ngtcp2_tls_ctx_అమరిక(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        శూన్య *user_దత్తాంశం)
{
  నిర్మాణం curl_tls_ctx *ctx = user_దత్తాంశం;

#నిర్వచితమైతే USE_OPENSSL
#ఒకవేళ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  ఒకవేళ(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(దత్తాంశం, "ngtcp2_crypto_boringssl_configure_client_context failed");
    తిరిగి CURLE_FAILED_INIT;
  }
#లేదావేళ defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#లేకపోతే
  ఒకవేళ(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(దత్తాంశం, "ngtcp2_crypto_quictls_configure_client_context failed");
    తిరిగి CURLE_FAILED_INIT;
  }
#అయితే_ముగింపు /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  ఒకవేళ(Curl_ssl_scache_use(cf, దత్తాంశం)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_సమితి_session_cache_బహుళకం(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_అంతర్గత);
    SSL_CTX_sess_సమితి_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#లేదావేళ defined(USE_GNUTLS)
  ఒకవేళ(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(దత్తాంశం, "ngtcp2_crypto_gnutls_configure_client_session failed");
    తిరిగి CURLE_FAILED_INIT;
  }
  ఒకవేళ(Curl_ssl_scache_use(cf, దత్తాంశం)) {
    gnutls_handshake_సమితి_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#లేదావేళ defined(USE_WOLFSSL)
  ఒకవేళ(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(దత్తాంశం, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    తిరిగి CURLE_FAILED_INIT;
  }
  ఒకవేళ(Curl_ssl_scache_use(cf, దత్తాంశం)) {
    /* Register to get notified when a new session is received */
    wolf_SSL_CTX_sess_సమితి_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_ngtcp2_on_session_reuse(నిర్మాణం Curl_cfilter *cf,
                                           నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం alpn_spec *alpns,
                                           నిర్మాణం Curl_ssl_session *scs,
                                           bool *do_early_దత్తాంశం)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  *do_early_దత్తాంశం = FALSE;
#ఒకవేళ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_GNUTLS
  ctx->earlydata_max =
    gnutls_రికార్డు_get_max_early_దత్తాంశం_కొలత(ctx->tls.gtls.session);
#అయితే_ముగింపు
#నిర్వచితమైతే USE_WOLFSSL
#నిర్వచితమైతే WOLFSSL_EARLY_దత్తాంశం
  ctx->earlydata_max = scs->earlydata_max;
#లేకపోతే
  ctx->earlydata_max = 0;
#అయితే_ముగింపు /* WOLFSSL_EARLY_DATA */
#అయితే_ముగింపు
#ఒకవేళ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  ఒకవేళ(!ctx->earlydata_max) {
    CURL_TRC_CF(దత్తాంశం, cf, "SSL session does not allow earlydata");
  }
  లేకపోతే ఒకవేళ(!Curl_alpn_కలిగియుంది_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(దత్తాంశం, cf, "SSL session from different ALPN, no early data");
  }
  లేకపోతే ఒకవేళ(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(దత్తాంశం, cf, "no 0RTT transport parameters, no early data");
  }
  లేకపోతే {
    పూర్ణాంకం rv;
    rv = ngtcp2_conn_decode_and_సమితి_0rtt_transport_params(
      ctx->qconn, (స్థిరరాశి uint8_t *)scs->quic_tp, scs->quic_tp_len);
    ఒకవేళ(rv)
      CURL_TRC_CF(దత్తాంశం, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    లేకపోతే ఒకవేళ(ctx->init_h3_conn_cb) {
      infof(దత్తాంశం, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      ఫలితం = ctx->init_h3_conn_cb(cf, దత్తాంశం, ctx);
      ఒకవేళ(!ఫలితం) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_దత్తాంశం = TRUE;
      }
    }
    లేకపోతే { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_దత్తాంశం = TRUE;
    }
  }
#లేకపోతే /* not supported in the TLS backend */
  (శూన్య)దత్తాంశం;
  (శూన్య)ctx;
  (శూన్య)scs;
  (శూన్య)alpns;
#అయితే_ముగింపు
  తిరిగి ఫలితం;
}

/*
 * Might be called twice for happy eyeballs.
 */
స్థిర CURLcode cf_connect_ప్రారంభం(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం cf_ngtcp2_io_ctx *pktx)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  పూర్ణాంకం rc;
  పూర్ణాంకం rv;
  CURLcode ఫలితం;
  స్థిరరాశి నిర్మాణం Curl_sockaddr_ex *sockaddr = ఖాళీ;
  పూర్ణాంకం qfd;
  స్థిర స్థిరరాశి నిర్మాణం alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  ఫలితం = Curl_rand(దత్తాంశం, ctx->dcid.దత్తాంశం, NGTCP2_MAX_CIDLEN);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  ఫలితం = Curl_rand(దత్తాంశం, ctx->scid.దత్తాంశం, NGTCP2_MAX_CIDLEN);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  (శూన్య)Curl_qlogdir(దత్తాంశం, ctx->scid.దత్తాంశం, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, దత్తాంశం, pktx);

  ఫలితం = Curl_vquic_ctx_init(దత్తాంశం, &ctx->q);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Query socket and remote address from sub-chain */
  ఒకవేళ(Curl_cf_socket_peek(cf->తదుపరి, దత్తాంశం, &ctx->q.sockfd, &sockaddr, ఖాళీ)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  ఒకవేళ(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = పరిమాణం(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (నిర్మాణం sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    ఒకవేళ(rv == -1)
      తిరిగి CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_మార్గం.local,
                     (నిర్మాణం sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_మార్గం.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  లేకపోతే {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    స్థిరరాశి నిర్మాణం Curl_sockaddr_ex *remote = ఖాళీ;
    ఒకవేళ(cf->తదుపరి->cft->query(cf->తదుపరి, దత్తాంశం, CF_QUERY_REMOTE_ADDR, ఖాళీ,
                            CURL_UNCONST(&remote)))
      తిరిగి CURLE_QUIC_CONNECT_ERROR;
    ఒకవేళ(!remote)
      తిరిగి CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, పరిమాణం(ctx->q.local_addr));
    ఎంపిక(remote->family) {
    స్థితి AF_INET:
      ((నిర్మాణం sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = పరిమాణం(నిర్మాణం sockaddr_in);
      ఆపు;
#నిర్వచితమైతే USE_IPV6
    స్థితి AF_INET6:
      ((నిర్మాణం sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = పరిమాణం(నిర్మాణం sockaddr_in6);
      ఆపు;
#అయితే_ముగింపు
    అప్రమేయ:
      తిరిగి CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_మార్గం.local,
                     (నిర్మాణం sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_మార్గం.remote,
                     &remote->curl_sa_addr,
                     (socklen_t)remote->addrlen);
  }

  rc = ngtcp2_conn_client_new(&ctx->qconn, &ctx->dcid, &ctx->scid,
                              &ctx->connected_మార్గం,
                              NGTCP2_PROTO_VER_V1, &ng_callbacks,
                              &ctx->settings, &ctx->transport_params,
                              Curl_ngtcp2_mem(), cf);
  ఒకవేళ(rc)
    తిరిగి CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_దత్తాంశం = cf;

  ఫలితం = Curl_vquic_tls_init(&ctx->tls, cf, దత్తాంశం,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_అమరిక, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

#ఒకవేళ defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  ఒకవేళ(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(దత్తాంశం, "ngtcp2_crypto_ossl_ctx_new failed");
    తిరిగి CURLE_FAILED_INIT;
  }
  ngtcp2_conn_సమితి_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  ఒకవేళ(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(దత్తాంశం, "ngtcp2_crypto_ossl_configure_client_session failed");
    తిరిగి CURLE_FAILED_INIT;
  }
#లేదావేళ defined(USE_OPENSSL)
  SSL_సమితి_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_సమితి_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#లేదావేళ defined(USE_GNUTLS)
  ngtcp2_conn_సమితి_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#లేదావేళ defined(USE_WOLFSSL)
  ngtcp2_conn_సమితి_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#లేకపోతే
#దోషం "ngtcp2 TLS backend not defined"
#అయితే_ముగింపు

#ఒకవేళ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  ఒకవేళ(ctx->use_earlydata &&
     !SSL_సమితి_quic_tls_early_దత్తాంశం_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(దత్తాంశం, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#అయితే_ముగింపు
  ngtcp2_ccerr_default(&ctx->చివరి_error);

  తిరిగి CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    bool *పూర్తి)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం cf_call_దత్తాంశం save;
  నిర్మాణం cf_ngtcp2_io_ctx pktx;

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  /* Connect the sub-chain */
  ఒకవేళ(cf->తదుపరి && !cf->తదుపరి->connected) {
    ఫలితం = Curl_conn_cf_connect(cf->తదుపరి, దత్తాంశం, పూర్తి);
    ఒకవేళ(ఫలితం || !*పూర్తి)
      తిరిగి ఫలితం;
  }

  *పూర్తి = FALSE;

  ఒకవేళ(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(దత్తాంశం, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(దత్తాంశం, cf, "need HTTPS-RR, delaying connect");
    తిరిగి CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, దత్తాంశం);
  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);

  ఒకవేళ(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(దత్తాంశం);
    ఫలితం = cf_connect_ప్రారంభం(cf, దత్తాంశం, &pktx);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ఒకవేళ(cf->connected) {
      *పూర్తి = TRUE;
      వెళ్ళు బహిర్గతం;
    }
    ఫలితం = Curl_cf_ngtcp2_పురోగతి_egress(cf, దత్తాంశం, &pktx);
    /* we do not expect to be able to recv anything yet */
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = Curl_cf_ngtcp2_పురోగతి_ingress(cf, దత్తాంశం, &pktx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = Curl_cf_ngtcp2_పురోగతి_egress(cf, దత్తాంశం, &pktx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    ఫలితం = ctx->tls_vrfy_ఫలితం;
    ఒకవేళ(!ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "peer verified");
      cf->connected = TRUE;
      *పూర్తి = TRUE;
    }
  }

బహిర్గతం:
  ఒకవేళ(ctx->tls_vrfy_ఫలితం)
    ఫలితం = ctx->tls_vrfy_ఫలితం;
  ఒకవేళ(ctx->qconn &&
     ((ఫలితం == CURLE_RECV_ERROR) || (ఫలితం == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_లోపల_draining_period(ctx->qconn)) {
    స్థిరరాశి ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    ఫలితం = CURLE_COULDNT_CONNECT;
    ఒకవేళ(cerr) {
      CURL_TRC_CF(దత్తాంశం, cf, "connect error, type=%d, code=%" PRIu64,
                  (పూర్ణాంకం)cerr->type, cerr->error_సంకేతం);
      ఎంపిక(cerr->type) {
      స్థితి NGTCP2_CCERR_TYPE_సంస్కరణ_NEGOTIATION:
        CURL_TRC_CF(దత్తాంశం, cf, "error in version negotiation");
        ఆపు;
      అప్రమేయ:
        ఒకవేళ(cerr->error_సంకేతం >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(దత్తాంశం, cf, "crypto error, tls alert=%u",
                      (గుర్తులేని పూర్ణాంకం)(cerr->error_సంకేతం & 0xffU));
        }
        లేకపోతే ఒకవేళ(cerr->error_సంకేతం == NGTCP2_CONNECTION_REFUSED) {
          CURL_TRC_CF(దత్తాంశం, cf, "connection refused by server");
          /* When a QUIC server instance is shutting down, it may send us a
           * CONNECTION_CLOSE with this code right away. We want
           * to keep on trying in this case. */
          ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
        }
      }
    }
  }

#నిర్వచితమైతే CURLVERBOSE
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      నిర్మాణం ip_quadruple ip;

      ఒకవేళ(!Curl_cf_socket_peek(cf->తదుపరి, దత్తాంశం, ఖాళీ, ఖాళీ, &ip))
        infof(దత్తాంశం, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_పోర్టు, curl_easy_strerror(ఫలితం));
    }
  }
#అయితే_ముగింపు
  ఒకవేళ(!ఫలితం && ctx->qconn) {
    ఫలితం = Curl_cf_ngtcp2_cmn_సమితి_expiry(cf, దత్తాంశం, &pktx);
  }
  ఒకవేళ(ఫలితం || *పూర్తి)
    CURL_TRC_CF(దత్తాంశం, cf, "connect -> %d, done=%d", (పూర్ణాంకం)ఫలితం, *పూర్తి);
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  నిర్మాణం cf_ngtcp2_io_ctx pktx;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(cf->shutdown || !ctx->qconn) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  ఒకవేళ(!cf->తదుపరి) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  *పూర్తి = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, దత్తాంశం);

  ఒకవేళ(!ctx->shutdown_started) {
    అక్షరం బఫరు[NGTCP2_MAX_UDP_PAYLOAD_కొలత];
    ngtcp2_ssize nwritten;

    ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->q.sendbuf)) {
      CURL_TRC_CF(దత్తాంశం, cf, "shutdown, flushing sendbuf");
      ఫలితం = Curl_cf_ngtcp2_పురోగతి_egress(cf, దత్తాంశం, &pktx);
      ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->q.sendbuf)) {
        CURL_TRC_CF(దత్తాంశం, cf, "sending shutdown packets blocked");
        ఫలితం = CURLE_OK;
        వెళ్ళు బహిర్గతం;
      }
      లేకపోతే ఒకవేళ(ఫలితం) {
        CURL_TRC_CF(దత్తాంశం, cf, "shutdown, error %d flushing sendbuf",
                    (పూర్ణాంకం)ఫలితం);
        *పూర్తి = TRUE;
        వెళ్ళు బహిర్గతం;
      }
    }

    DEBUGASSERT(Curl_bufq_is_ఖాళీలు(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, ఖాళీ, /*మార్గం*/
      ఖాళీ, /* pkt_info */
      (uint8_t *)బఫరు, పరిమాణం(బఫరు),
      &ctx->చివరి_error, pktx.ts);
    CURL_TRC_CF(దత్తాంశం, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (పూర్ణాంకం)ctx->చివరి_error.type,
                ctx->చివరి_error.error_సంకేతం, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    ఒకవేళ(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      ఫలితం = Curl_bufq_write(&ctx->q.sendbuf, (స్థిరరాశి గుర్తులేని అక్షరం *)బఫరు,
                               (size_t)nwritten, &n);
      ఒకవేళ(ఫలితం) {
        CURL_TRC_CF(దత్తాంశం, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (పూర్ణాంకం)ఫలితం);
        వెళ్ళు బహిర్గతం;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.విభజించు_len = 0;
    }
  }

  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->q.sendbuf)) {
    CURL_TRC_CF(దత్తాంశం, cf, "shutdown, flushing egress");
    ఫలితం = Curl_vquic_flush(cf, దత్తాంశం, &ctx->q);
    ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      CURL_TRC_CF(దత్తాంశం, cf, "sending shutdown packets blocked");
      ఫలితం = CURLE_OK;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "shutdown, error %d flushing sendbuf",
                  (పూర్ణాంకం)ఫలితం);
      *పూర్తి = TRUE;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఒకవేళ(Curl_bufq_is_ఖాళీలు(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(దత్తాంశం, cf, "shutdown completely sent off, done");
    *పూర్తి = TRUE;
    ఫలితం = CURLE_OK;
  }
బహిర్గతం:
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

శూన్య Curl_cf_ngtcp2_cmn_conn_close(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  bool పూర్తి;
  Curl_cf_ngtcp2_cmn_shutdown(cf, దత్తాంశం, &పూర్తి);
}

స్థిర bool cf_ngtcp2_err_is_fatal(పూర్ణాంకం సంకేతం_2)
{
  తిరిగి (NGTCP2_ERR_FATAL >= సంకేతం_2) ||
         (NGTCP2_ERR_DROP_CONN == సంకేతం_2) ||
         (NGTCP2_ERR_IDLE_CLOSE == సంకేతం_2);
}

శూన్య Curl_cf_ngtcp2_cmn_err_సమితి(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం సంకేతం_2)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  ఒకవేళ(!ctx->చివరి_error.error_సంకేతం) {
    ఒకవేళ(NGTCP2_ERR_CRYPTO == సంకేతం_2) {
      ngtcp2_ccerr_సమితి_tls_alert(&ctx->చివరి_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 ఖాళీ, 0);
    }
    లేకపోతే {
      ngtcp2_ccerr_సమితి_liberr(&ctx->చివరి_error, సంకేతం_2, ఖాళీ, 0);
    }
  }
  ఒకవేళ(cf_ngtcp2_err_is_fatal(సంకేతం_2))
    Curl_cf_ngtcp2_cmn_conn_close(cf, దత్తాంశం);
}

శూన్య Curl_cf_ngtcp2_io_ctx_init(నిర్మాణం cf_ngtcp2_io_ctx *io_ctx,
                                నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->దత్తాంశం = దత్తాంశం;
  io_ctx->now = *Curl_pgrs_now(దత్తాంశం);
  ngtcp2_మార్గం_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_సమితి_సమయం(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

శూన్య Curl_cf_ngtcp2_io_ctx_update_సమయం(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం cf_ngtcp2_io_ctx *io_ctx,
                                       నిర్మాణం Curl_cfilter *cf)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(దత్తాంశం);
  Curl_vquic_ctx_update_సమయం(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#ఒకవేళ NGTCP2_సంస్కరణ_అంకె < 0x011100
నిర్మాణం cf_ngtcp2_sfind_ctx {
  int64_t ప్రవాహం_id;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  uint32_t mid;
};

స్థిర bool cf_ngtcp2_sfind(uint32_t mid, శూన్య *విలువ, శూన్య *user_దత్తాంశం)
{
  నిర్మాణం cf_ngtcp2_sfind_ctx *fctx = user_దత్తాంశం;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = విలువ;

  ఒకవేళ(fctx->ప్రవాహం_id == ప్రవాహం->id) {
    fctx->mid = mid;
    fctx->ప్రవాహం = ప్రవాహం;
    తిరిగి FALSE;
  }
  తిరిగి TRUE; /* continue */
}

స్థిర నిర్మాణం h3_ప్రవాహం_ctx *cf_ngtcp2_get_ప్రవాహం(నిర్మాణం cf_ngtcp2_ctx *ctx,
                                                  int64_t ప్రవాహం_id)
{
  నిర్మాణం cf_ngtcp2_sfind_ctx fctx;
  fctx.ప్రవాహం_id = ప్రవాహం_id;
  fctx.ప్రవాహం = ఖాళీ;
  Curl_u32_ptrset_సందర్శించు(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  తిరిగి fctx.ప్రవాహం;
}
#లేకపోతే
స్థిర నిర్మాణం h3_ప్రవాహం_ctx *cf_ngtcp2_get_ప్రవాహం(నిర్మాణం cf_ngtcp2_ctx *ctx,
                                                  int64_t ప్రవాహం_id)
{
  తిరిగి ngtcp2_conn_get_ప్రవాహం_user_దత్తాంశం(ctx->qconn, ప్రవాహం_id);
}
#అయితే_ముగింపు

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
స్థిర CURLcode read_pkt_గమ్యం_send(శూన్య *userp,
                                 గుర్తులేని అక్షరం *buf, size_t buflen,
                                 size_t *pnread)
{
  నిర్మాణం cf_ngtcp2_io_ctx *x = userp;
  నిర్మాణం cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t ప్రవాహం_id;
  పూర్ణాంకం fin;
  ssize_t n;

  *pnread = 0;
  veccnt = 0;
  ప్రవాహం_id = -1;
  fin = 0;

  /* ngtcp2 may want to put several frames from different streams into
   * this packet. `NGTCP2_WRITE_STREAM_FLAG_MORE` tells it to do so.
   * When `NGTCP2_ERR_WRITE_MORE` is returned, we *need* to make
   * another iteration.
   * When ngtcp2 is happy (because it has no other frame that would fit
   * or it has nothing more to send), it returns the total length
   * of the assembled packet. This may be 0 if there was nothing to send. */
  కొరకు(;;) {

    ఒకవేళ(ctx->h3conn && ngtcp2_conn_get_max_దత్తాంశం_ఎడమ(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_ప్రవాహం(ctx->h3conn, &ప్రవాహం_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      ఒకవేళ(veccnt < 0) {
        failf(x->దత్తాంశం, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((పూర్ణాంకం)veccnt));
        Curl_cf_ngtcp2_h3_err_సమితి(x->cf, x->దత్తాంశం, (పూర్ణాంకం)veccnt);
        తిరిగి CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_ప్రవాహం_FLAG_మరిన్ని |
            (fin ? NGTCP2_WRITE_ప్రవాహం_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_ప్రవాహం(ctx->qconn, &x->ps.మార్గం,
                                  ఖాళీ, buf, buflen,
                                  &ndatalen, flags, ప్రవాహం_id,
                                  (స్థిరరాశి ngtcp2_vec *)vec, veccnt, x->ts);
    ఒకవేళ(n == 0) {
      /* nothing to send */
      తిరిగి CURLE_AGAIN;
    }
    లేకపోతే ఒకవేళ(n < 0) {
      ఎంపిక(n) {
      స్థితి NGTCP2_ERR_ప్రవాహం_దత్తాంశం_BLOCKED: {
        నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_ఖండం_ప్రవాహం(ctx->h3conn, ప్రవాహం_id);
        CURL_TRC_CF(x->దత్తాంశం, x->cf, "[%" PRId64 "] block quic flow",
                    ప్రవాహం_id);
        ప్రవాహం = cf_ngtcp2_get_ప్రవాహం(ctx, ప్రవాహం_id);
        ఒకవేళ(ప్రవాహం) /* it might be not one of our h3 streams? */
          ప్రవాహం->quic_flow_blocked = TRUE;
        n = 0;
        ఆపు;
      }
      స్థితి NGTCP2_ERR_ప్రవాహం_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_ప్రవాహం_write(ctx->h3conn, ప్రవాహం_id);
        n = 0;
        ఆపు;
      స్థితి NGTCP2_ERR_WRITE_మరిన్ని:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        ఆపు;
      అప్రమేయ:
        DEBUGASSERT(ndatalen == -1);
        failf(x->దత్తాంశం, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((పూర్ణాంకం)n));
        Curl_cf_ngtcp2_cmn_err_సమితి(x->cf, x->దత్తాంశం, (పూర్ణాంకం)n);
        తిరిగి CURLE_SEND_ERROR;
      }
    }

    ఒకవేళ(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      పూర్ణాంకం rv = nghttp3_conn_కూడు_write_స్థానభ్రంశం(ctx->h3conn, ప్రవాహం_id, ndatalen);
      ఒకవేళ(rv) {
        failf(x->దత్తాంశం, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        తిరిగి CURLE_SEND_ERROR;
      }
    }

    ఒకవేళ(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      తిరిగి CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_పురోగతి_egress(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం cf_ngtcp2_io_ctx *pktx)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_కొలత, మార్గం_max_payload_కొలత;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_కాలాంశం;
  CURLcode ఫలితం;
  నిర్మాణం cf_ngtcp2_io_ctx local_pktx;

  ఒకవేళ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, దత్తాంశం);
    pktx = &local_pktx;
  }
  లేకపోతే {
    Curl_cf_ngtcp2_io_ctx_update_సమయం(దత్తాంశం, pktx, cf);
    ngtcp2_మార్గం_storage_zero(&pktx->ps);
  }

  ఫలితం = Curl_vquic_flush(cf, దత్తాంశం, &ctx->q);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      Curl_expire_సమితి(దత్తాంశం, EXPIRE_QUIC, 1, &pktx->now);
      తిరిగి CURLE_OK;
    }
    తిరిగి ఫలితం;
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
  max_payload_కొలత = ngtcp2_conn_get_max_tx_udp_payload_కొలత(ctx->qconn);
  మార్గం_max_payload_కొలత =
    ngtcp2_conn_get_మార్గం_max_tx_udp_payload_కొలత(ctx->qconn);
  send_కాలాంశం = ngtcp2_conn_get_send_కాలాంశం(ctx->qconn);
  CURL_TRC_CF(దత్తాంశం, cf, "egress, collect and send packets, quantum=%zu",
              send_కాలాంశం);
  కొరకు(;;) {
    /* add the next packet to send, if any, to our buffer */
    ఫలితం = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_కొలత,
                            read_pkt_గమ్యం_send, pktx, &nread);
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      ఆపు;
    లేకపోతే ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    లేకపోతే {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      ఒకవేళ((buflen >= send_కాలాంశం) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_కొలత))
        ఆపు;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      ఒకవేళ(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      లేకపోతే ఒకవేళ(nread > gsolen ||
              (gsolen > మార్గం_max_payload_కొలత && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        ఫలితం = Curl_vquic_send_తోక_విభజించు(cf, దత్తాంశం, &ctx->q,
                                            gsolen, nread, nread);
        ఒకవేళ(ఫలితం) {
          ఒకవేళ(ఫలితం == CURLE_AGAIN) {
            Curl_expire_సమితి(దత్తాంశం, EXPIRE_QUIC, 1, &pktx->now);
            తిరిగి CURLE_OK;
          }
          తిరిగి ఫలితం;
        }
        pktcnt = 0;
      }
      లేకపోతే ఒకవేళ(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        ఆపు;
      }
    }
  }

  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(దత్తాంశం, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    ఫలితం = Curl_vquic_send(cf, దత్తాంశం, &ctx->q, gsolen);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం == CURLE_AGAIN) {
        Curl_expire_సమితి(దత్తాంశం, EXPIRE_QUIC, 1, &pktx->now);
        తిరిగి CURLE_OK;
      }
      తిరిగి ఫలితం;
    }
    Curl_cf_ngtcp2_io_ctx_update_సమయం(దత్తాంశం, pktx, cf);
    ngtcp2_conn_update_pkt_tx_సమయం(ctx->qconn, pktx->ts);
  }
  తిరిగి CURLE_OK;
}

నిర్మాణం cf_ngtcp2_recv_ctx {
  నిర్మాణం cf_ngtcp2_io_ctx *pktx;
  size_t pkt_లెక్క;
};

స్థిర CURLcode cf_ngtcp2_recv_pkts(స్థిరరాశి గుర్తులేని అక్షరం *buf, size_t buflen,
                                    size_t gso_కొలత,
                                    నిర్మాణం sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    శూన్య *userp)
{
  నిర్మాణం cf_ngtcp2_recv_ctx *rctx = userp;
  నిర్మాణం cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  నిర్మాణం cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_మార్గం మార్గం;
  size_t స్థానభ్రంశం, pktlen;
  పూర్ణాంకం rv;

  ఒకవేళ(!rctx->pkt_లెక్క) {
    Curl_cf_ngtcp2_io_ctx_update_సమయం(pktx->దత్తాంశం, pktx, pktx->cf);
    ngtcp2_మార్గం_storage_zero(&pktx->ps);
  }

  ఒకవేళ(ecn)
    CURL_TRC_CF(pktx->దత్తాంశం, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_కొలత, (గుర్తులేని)ecn);
  ngtcp2_addr_init(&మార్గం.local, (నిర్మాణం sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&మార్గం.remote, (నిర్మాణం sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  కొరకు(స్థానభ్రంశం = 0; స్థానభ్రంశం < buflen; స్థానభ్రంశం += gso_కొలత) {
    rctx->pkt_లెక్క++;
    pktlen = ((స్థానభ్రంశం + gso_కొలత) <= buflen) ? gso_కొలత : (buflen - స్థానభ్రంశం);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &మార్గం, &pi,
                              buf + స్థానభ్రంశం, pktlen, pktx->ts);
    ఒకవేళ(rv) {
      CURL_TRC_CF(pktx->దత్తాంశం, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_సమితి(pktx->cf, pktx->దత్తాంశం, rv);

      ఒకవేళ(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        తిరిగి CURLE_PEER_FAILED_VERIFICATION;
      తిరిగి CURLE_RECV_ERROR;
    }
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_పురోగతి_ingress(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం cf_ngtcp2_io_ctx *pktx)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం cf_ngtcp2_io_ctx local_pktx;
  నిర్మాణం cf_ngtcp2_recv_ctx rctx;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, దత్తాంశం);
    pktx = &local_pktx;
  }

  ఫలితం = Curl_vquic_tls_before_recv(&ctx->tls, cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  rctx.pktx = pktx;
  rctx.pkt_లెక్క = 0;

  ఒకవేళ(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(దత్తాంశం, cf, "progress_ingress(socket)");
    తిరిగి Curl_vquic_recv_packets(cf, దత్తాంశం, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  లేకపోతే {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    గుర్తులేని అక్షరం *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_కొలత;
    size_t pkt_హద్దు = QUIC_TUNNEL_INGRESS_PKT_హద్దు;
    size_t nread;
    నిర్మాణం sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(దత్తాంశం, cf, "progress_ingress(sub-filters)");
    ఒకవేళ(ctx->qconn) {
      size_t max_మార్గం_payload;
      max_మార్గం_payload =
        ngtcp2_conn_get_మార్గం_max_tx_udp_payload_కొలత(ctx->qconn);
      ఒకవేళ(max_మార్గం_payload > max_udp_payload)
        max_udp_payload = max_మార్గం_payload;
    }

    ఒకవేళ(ctx->tunnel_inbuf_len < max_udp_payload) {
      గుర్తులేని అక్షరం *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      ఒకవేళ(!newbuf)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    అయితే(pkt_హద్దు--) {
      ఫలితం = Curl_conn_cf_recv(cf->తదుపరి, దత్తాంశం, (అక్షరం *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      ఒకవేళ(ఫలితం == CURLE_AGAIN) {
        /* no more data available at the moment */
        తిరిగి CURLE_OK;
      }
      ఒకవేళ(ఫలితం) {
        CURL_TRC_CF(దత్తాంశం, cf, "ingress, recv from tunnel failed: %d",
                    (పూర్ణాంకం)ఫలితం);
        తిరిగి ఫలితం;
      }
      ఒకవేళ(nread == 0) {
        /* tunnel closed */
        తిరిగి CURLE_OK;
      }

      జ్ఞాపకం_నకలు(&remote_addr, ctx->connected_మార్గం.remote.addr,
             ctx->connected_మార్గం.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_మార్గం.remote.addrlen;
      ఫలితం = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      ఒకవేళ(!ctx->q.got_మొదటి_byte) {
        ctx->q.got_మొదటి_byte = TRUE;
        ctx->q.మొదటి_byte_at = ctx->q.చివరి_op;
      }
      ctx->q.చివరి_io = ctx->q.చివరి_op;
    }
    తిరిగి CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_సమితి_expiry(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం cf_ngtcp2_io_ctx *pktx)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  ఒకవేళ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, దత్తాంశం);
    pktx = &local_pktx;
  }
  లేకపోతే {
    Curl_cf_ngtcp2_io_ctx_update_సమయం(దత్తాంశం, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  ఒకవేళ(expiry != UINT64_MAX) {
    ఒకవేళ(expiry <= pktx->ts) {
      CURLcode ఫలితం;
      పూర్ణాంకం rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      ఒకవేళ(rv) {
        failf(దత్తాంశం, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_సమితి(cf, దత్తాంశం, rv);
        తిరిగి CURLE_SEND_ERROR;
      }
      ఫలితం = Curl_cf_ngtcp2_పురోగతి_ingress(cf, దత్తాంశం, pktx);
      ఒకవేళ(!ఫలితం)
        ఫలితం = Curl_cf_ngtcp2_పురోగతి_egress(cf, దత్తాంశం, pktx);
      ఒకవేళ(ఫలితం) {
        /* a verify failure during ingress must win over generic errors */
        ఒకవేళ(ctx->tls_vrfy_ఫలితం)
          ఫలితం = ctx->tls_vrfy_ఫలితం;
        తిరిగి ఫలితం;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    ఒకవేళ(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      ఒకవేళ(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_సమితి(దత్తాంశం, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cf_ngtcp2_అమరిక_keep_alive(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  స్థిరరాశి ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  ఒకవేళ(!ctx->qconn)
    తిరిగి;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  ఒకవేళ(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_సమితి_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(దత్తాంశం, cf, "no peer idle timeout, unset keep-alive");
  }
  లేకపోతే ఒకవేళ(!Curl_u32_ptrset_లెక్క(&ctx->streams)) {
    ngtcp2_conn_సమితి_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(దత్తాంశం, cf, "no active streams, unset keep-alive");
  }
  లేకపోతే {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_సమితి_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(దత్తాంశం, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_ప్రవాహం_అమరిక(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం h3_ప్రవాహం_ctx **pstream)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);

  *pstream = ఖాళీ;
  ఒకవేళ(!దత్తాంశం)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(ప్రవాహం) {
    *pstream = ప్రవాహం;
    తిరిగి CURLE_OK;
  }

  ప్రవాహం = curlx_calloc(1, పరిమాణం(*ప్రవాహం));
  ఒకవేళ(!ప్రవాహం)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ప్రవాహం->id = -1;
  ప్రవాహం->xfer_id = దత్తాంశం->id;
  ప్రవాహం->mid = దత్తాంశం->mid;
  ప్రవాహం->rx_స్థానభ్రంశం = 0;
  ప్రవాహం->rx_స్థానభ్రంశం_max = H3_ప్రవాహం_కిటికీ_కొలత_INITIAL;
  ప్రవాహం->tx_లోపల_flight_ideal = H3_ప్రవాహం_SEND_BUF_INITIAL;

  /* on send, we control how much we put into the buffer */
  Curl_bufq_initp(&ప్రవాహం->sendbuf, &ctx->ప్రవాహం_bufcp,
                  H3_ప్రవాహం_SEND_CHUNKS, BUFQ_OPT_NONE);
  ప్రవాహం->tx_లోపల_flight_కొలత = 0;
  Curl_h1_req_parse_init(&ప్రవాహం->h1, H1_PARSE_DEFAULT_MAX_పంక్తి_LEN);

  ఒకవేళ(Curl_u32_ptrset_సమితి(&ctx->streams, దత్తాంశం->mid, ప్రవాహం)) {
    Curl_cf_ngtcp2_h3_ప్రవాహం_ctx_free(ప్రవాహం);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ఒకవేళ(Curl_u32_ptrset_లెక్క(&ctx->streams) == 1)
    cf_ngtcp2_అమరిక_keep_alive(cf, దత్తాంశం);

  *pstream = ప్రవాహం;
  తిరిగి CURLE_OK;
}

శూన్య Curl_cf_ngtcp2_h3_ప్రవాహం_close(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(ప్రవాహం);
  ఒకవేళ(!ప్రవాహం->closed && ctx->qconn && ctx->h3conn) {
    CURLcode ఫలితం;

    nghttp3_conn_సమితి_ప్రవాహం_user_దత్తాంశం(ctx->h3conn, ప్రవాహం->id, ఖాళీ);
    ngtcp2_conn_సమితి_ప్రవాహం_user_దత్తాంశం(ctx->qconn, ప్రవాహం->id, ఖాళీ);
    ప్రవాహం->closed = TRUE;
    (శూన్య)ngtcp2_conn_shutdown_ప్రవాహం(ctx->qconn, 0, ప్రవాహం->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    ఫలితం = Curl_cf_ngtcp2_పురోగతి_egress(cf, దత్తాంశం, ఖాళీ);
    ఒకవేళ(ఫలితం)
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] cancel stream -> %d",
                  ప్రవాహం->id, (పూర్ణాంకం)ఫలితం);
  }
}

శూన్య Curl_cf_ngtcp2_h3_ప్రవాహం_పూర్తి(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
  (శూన్య)cf;
  ఒకవేళ(ప్రవాహం) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] easy handle is done", ప్రవాహం->id);
    Curl_cf_ngtcp2_h3_ప్రవాహం_close(cf, దత్తాంశం, ప్రవాహం);
    Curl_u32_ptrset_unset(&ctx->streams, దత్తాంశం->mid);
    ఒకవేళ(!Curl_u32_ptrset_లెక్క(&ctx->streams))
      cf_ngtcp2_అమరిక_keep_alive(cf, దత్తాంశం);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      bool *ఆదానం_pending)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  స్థిరరాశి ngtcp2_transport_params *rp;
  నిర్మాణం cf_call_దత్తాంశం save;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  *ఆదానం_pending = FALSE;
  ఒకవేళ(!ctx->qconn || ctx->shutdown_started)
    వెళ్ళు బహిర్గతం;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  ఒకవేళ(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &ctx->q.చివరి_io);
    ఒకవేళ(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      ఒకవేళ((uint64_t)idletime_ms > max_idle_ms)
        వెళ్ళు బహిర్గతం;
    }
  }

  ఒకవేళ(!cf->తదుపరి || !cf->తదుపరి->cft->is_alive(cf->తదుపరి, దత్తాంశం, ఆదానం_pending))
    వెళ్ళు బహిర్గతం;

  alive = TRUE;
  ఒకవేళ(*ఆదానం_pending) {
    CURLcode ఫలితం;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *ఆదానం_pending = FALSE;
    ఫలితం = Curl_cf_ngtcp2_పురోగతి_ingress(cf, దత్తాంశం, ఖాళీ);
    CURL_TRC_CF(దత్తాంశం, cf, "is_alive, progress ingress -> %d", (పూర్ణాంకం)ఫలితం);
    alive = ఫలితం ? FALSE : TRUE;
  }

బహిర్గతం:
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(నిర్మాణం cf_ngtcp2_ctx *ctx,
                                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  int64_t ctrl_ప్రవాహం_id, qpack_enc_ప్రవాహం_id, qpack_dec_ప్రవాహం_id;
  పూర్ణాంకం rc;

  rc = ngtcp2_conn_open_uni_ప్రవాహం(ctx->qconn, &ctrl_ప్రవాహం_id, ఖాళీ);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    తిరిగి CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_ప్రవాహం(ctx->h3conn, ctrl_ప్రవాహం_id);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    తిరిగి CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_ప్రవాహం(ctx->qconn, &qpack_enc_ప్రవాహం_id, ఖాళీ);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    తిరిగి CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_ప్రవాహం(ctx->qconn, &qpack_dec_ప్రవాహం_id, ఖాళీ);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    తిరిగి CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_ప్రవాహం_id,
                                       qpack_dec_ప్రవాహం_id);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    తిరిగి CURLE_QUIC_CONNECT_ERROR;
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;

  ఎంపిక(query) {
  స్థితి CF_QUERY_CONNECT_జవాబు_MS:
    ఒకవేళ((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_మొదటి_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.మొదటి_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < గరిష్ఠ_పూర్ణాంకం) ? (పూర్ణాంకం)ms : గరిష్ఠ_పూర్ణాంకం;
      తిరిగి CURLE_OK;
    }
    ఆపు;
  స్థితి CF_QUERY_REALLY_CONNECTED:
    ఒకవేళ(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_మొదటి_byte;
      తిరిగి CURLE_OK;
    }
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->query(cf->తదుపరి, దత్తాంశం, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)arg1;
  (శూన్య)arg2;
  ఎంపిక(event) {
  స్థితి CF_CTRL_REPORT_STATS:
    ఒకవేళ(cf->connected && !ctx->stats_reported) {
      ఒకవేళ((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_మొదటి_byte) {
        Curl_pgrs_సమయం_Was(దత్తాంశం, TIMER_CONNECT, ctx->q.మొదటి_byte_at);
        ctx->stats_reported = TRUE;
      }
      లేకపోతే ఒకవేళ(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        ఒకవేళ(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrs_సమయం_Was(దత్తాంశం, TIMER_CONNECT, ctx->q.మొదటి_byte_at);
        Curl_pgrs_సమయం_Was(దత్తాంశం, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
