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

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#अन्तर्भाव <ngtcp2/ngtcp2.h>

#यदि_परिभाषितम् USE_OPENSSL
#अन्तर्भाव <openssl/err.h>
#यदि defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_boringssl.h>
#नोचेद्यदि defined(OPENSSL_QUIC_API2)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_ossl.h>
#अन्यथा
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_quictls.h>
#यदि_अन्तः
#अन्तर्भाव "vtls/openssl.h"
#नोचेद्यदि defined(USE_GNUTLS)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_gnutls.h>
#अन्तर्भाव "vtls/gtls.h"
#नोचेद्यदि defined(USE_WOLFSSL)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_wolfssl.h>
#अन्तर्भाव "vtls/wolfssl.h"
#यदि_अन्तः

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
  ध्रुव ngtcp2_info *ng2 = ngtcp2_संस्करणम्(0);
  ध्रुव nghttp3_info *ht3 = nghttp3_संस्करणम्(0);
  (शून्य)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->संस्करणम्_str, ht3->संस्करणम्_str);
}

शून्य Curl_cf_ngtcp2_h3_प्रवाहः_ctx_free(संरचना h3_प्रवाहः_ctx *प्रवाहः)
{
  Curl_bufq_free(&प्रवाहः->sendbuf);
  Curl_h1_req_parse_free(&प्रवाहः->h1);
  curlx_free(प्रवाहः);
}

स्थिर शून्य h3_प्रवाहः_समुच्चयः_free(uint32_t id, शून्य *प्रवाहः)
{
  (शून्य)id;
  DEBUGASSERT(प्रवाहः);
  Curl_cf_ngtcp2_h3_प्रवाहः_ctx_free((संरचना h3_प्रवाहः_ctx *)प्रवाहः);
}

स्थिर bool cf_ngtcp2_h3_err_is_fatal(पूर्णाङ्क संहिता)
{
  प्रत्यागम (NGHTTP3_ERR_FATAL >= संहिता) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_प्रवाहः == संहिता);
}

शून्य Curl_cf_ngtcp2_h3_err_समुच्चयः(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त, पूर्णाङ्क संहिता)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  यदि(!ctx->चरमः_error.error_संहिता) {
    ngtcp2_ccerr_समुच्चयः_application_error(&ctx->चरमः_error,
      nghttp3_err_infer_quic_app_error_संहिता(संहिता), रिक्त, 0);
  }
  यदि(cf_ngtcp2_h3_err_is_fatal(संहिता))
    Curl_cf_ngtcp2_cmn_conn_close(cf, दत्त);
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
  ctx->संस्करणम्_2 = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->प्रवाहः_bufcp, H3_प्रवाहः_CHUNK_आकार,
                  H3_प्रवाहः_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_शीर्षकम्);
  Curl_u32_ptrset_init(&ctx->streams, h3_प्रवाहः_समुच्चयः_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  प्रत्यागम Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

शून्य Curl_cf_ngtcp2_ctx_cleanup(संरचना cf_ngtcp2_ctx *ctx)
{
  यदि(ctx && ctx->initialized) {
    यदि(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    यदि(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = रिक्त;
    }
    यदि(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = रिक्त;
    }
#यदि_परिभाषितम् OPENSSL_QUIC_API2
    यदि(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = रिक्त;
    }
#यदि_अन्तः
    Curl_bufcp_free(&ctx->प्रवाहः_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    curlx_safefree(ctx->tunnel_inbuf);
    ctx->tunnel_inbuf_len = 0;
  }
}

संरचना Curl_easy *Curl_cf_ngtcp2_get_xfer(संरचना Curl_cfilter *cf,
                                          शून्य *प्रवाहः_user_दत्त,
                                          संरचना h3_प्रवाहः_ctx **pstream)
{
  संरचना h3_प्रवाहः_ctx *प्रवाहः = प्रवाहः_user_दत्त;

  यदि(pstream)
    *pstream = प्रवाहः;
  यदि(प्रवाहः && (प्रवाहः->mid != UINT32_MAX)) {
    संरचना Curl_easy *दत्त, *call_दत्त = CF_दत्त_CURRENT(cf);
    यदि(!call_दत्त || !call_दत्त->multi) {
      DEBUGASSERT(0);
      प्रत्यागम रिक्त;
    }
    दत्त = Curl_multi_get_easy(call_दत्त->multi, प्रवाहः->mid);
    यदि(दत्त && (दत्त->id == प्रवाहः->xfer_id))
      प्रत्यागम दत्त;
    DEBUGASSERT(0);
  }
  प्रत्यागम रिक्त;
}

स्थिर ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  संरचना Curl_cfilter *cf = conn_ref->user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  प्रत्यागम ctx->qconn;
}

#यदि_परिभाषितम् DEBUG_NGTCP2
स्थिर शून्य quic_printf(शून्य *user_दत्त, ध्रुव वर्ण *fmt, ...)
{
  va_list ap;
  (शून्य)user_दत्त;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#यदि_अन्तः

स्थिर शून्य qlog_callback(शून्य *user_दत्त, uint32_t flags,
                          ध्रुव शून्य *दत्त, size_t datalen)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  (शून्य)flags;
  यदि(ctx->qlogfd != -1) {
    ssize_t rc = लिख(ctx->qlogfd, दत्त, datalen);
    यदि(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

स्थिर शून्य quic_settings(संरचना cf_ngtcp2_ctx *ctx,
                          संरचना Curl_easy *दत्त,
                          संरचना cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#यदि_परिभाषितम् DEBUG_NGTCP2
  s->log_printf = quic_printf;
#अन्यथा
  s->log_printf = रिक्त;
#यदि_अन्तः

  s->initial_ts = pktx->ts;
  s->handshake_timeout = (दत्त->समुच्चयः.connecttimeout > 0) ?
    दत्त->समुच्चयः.connecttimeout * NGTCP2_MILLISECONDS : QUIC_HANDSHAKE_TIMEOUT;
  s->max_गवाक्षः = H3_CONN_गवाक्षः_आकार_MAX;
  s->max_प्रवाहः_गवाक्षः = 0; /* disable ngtcp2 auto-tuning of window */
  s->no_pmtud = FALSE;
#यदि_परिभाषितम् NGTCP2_SETTINGS_V3
  /* try ten times the ngtcp2 defaults here for problems with Caddy */
  s->glitch_ratelim_burst = 1000 * 10;
  s->glitch_ratelim_दरः = 33 * 10;
#यदि_अन्तः
  t->initial_max_दत्त = s->max_गवाक्षः;
  t->initial_max_प्रवाहः_दत्त_bidi_local = H3_प्रवाहः_गवाक्षः_आकार_INITIAL;
  t->initial_max_प्रवाहः_दत्त_bidi_remote = H3_प्रवाहः_गवाक्षः_आकार_INITIAL;
  t->initial_max_प्रवाहः_दत्त_uni = t->initial_max_दत्त;
  t->initial_max_streams_bidi = QUIC_MAX_STREAMS;
  t->initial_max_streams_uni = QUIC_MAX_STREAMS;
  t->max_idle_timeout = 0; /* no idle timeout from our side */
  यदि(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#यदि defined(_MSC_VER) && defined(_DLL)
#निर्देशः warning(प्रक्षिप)
#निर्देशः warning(disable:4232) /* MSVC extension, dllimport identity */
#यदि_अन्तः

स्थिर पूर्णाङ्क cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, शून्य *user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf ? cf->ctx : रिक्त;
  संरचना Curl_easy *दत्त;

  (शून्य)tconn;
  DEBUGASSERT(ctx);
  दत्त = CF_दत्त_CURRENT(cf);
  DEBUGASSERT(दत्त);
  यदि(!ctx || !दत्त)
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(दत्त);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, दत्त);

  ctx->tls_vrfy_परिणाम = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    दत्त, &ctx->ssl_peer);
  यदि(ctx->tls_vrfy_परिणाम)
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;

#यदि_परिभाषितम् CURLVERBOSE
  यदि(Curl_trc_is_verbose(दत्त)) {
    ध्रुव ngtcp2_transport_params *rp;
    rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
    CURL_TRC_CF(दत्त, cf, "handshake complete after %" FMT_TIMEDIFF_T
                "ms, remote transport[max_udp_payload=%" PRIu64
                ", initial_max_data=%" PRIu64 "]",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at),
                rp->max_udp_payload_आकार, rp->initial_max_दत्त);
  }
#यदि_अन्तः

  /* In case of earlydata, where we simulate being connected, update
   * the handshake time when we really did connect */
  यदि(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrsTimeWas(दत्त, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  यदि(ctx->use_earlydata) {
#यदि defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    यदि(SSL_get_early_दत्त_स्थितिः(ctx->tls.ossl.ssl) == SSL_EARLY_दत्त_NOT_प्रेषितम्)
      CURL_TRC_CF(दत्त, cf, "OpenSSL did not send early data");
#यदि_अन्तः

#यदि NGTCP2_संस्करणम्_अङ्कः >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_दत्त_rejected2(ctx->qconn);
#अन्यथा /* older NGTCP2 */
    {
#यदि defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      पूर्णाङ्क ossl_early_स्थितिः = SSL_get_early_दत्त_स्थितिः(ctx->tls.ossl.ssl);
      यदि(ossl_early_स्थितिः == SSL_EARLY_दत्त_NOT_प्रेषितम्)
        CURL_TRC_CF(दत्त, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_स्थितिः == SSL_EARLY_दत्त_ACCEPTED);
#नोचेद्यदि defined(USE_GNUTLS)
      पूर्णाङ्क flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_दत्त);
#नोचेद्यदि defined(USE_WOLFSSL)
#यदि_परिभाषितम् WOLFSSL_EARLY_दत्त
      ctx->earlydata_accepted =
        (wolf_SSL_get_early_दत्त_स्थितिः(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_दत्त_REJECTED);
#अन्यथा
      DEBUGASSERT(0); /* should not come here if ED is disabled. */
      ctx->earlydata_accepted = FALSE;
#यदि_अन्तः /* WOLFSSL_EARLY_DATA */
#यदि_अन्तः /* OPENSSL or GNUTLS or WOLFSSL */
    }
#यदि_अन्तः /* older NGTCP2 */
    CURL_TRC_CF(दत्त, cf, "server did%s accept %zu bytes of early data",
                ctx->earlydata_accepted ? "" : " not", ctx->earlydata_skip);
    Curl_pgrs_Early_दत्त(दत्त, ctx->earlydata_accepted ?
                              (curl_off_t)ctx->earlydata_skip :
                             -(curl_off_t)ctx->earlydata_skip);
  }
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_recv_प्रवाहः_दत्त(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t प्रवाहः_id, uint64_t विस्थापनम्,
                               ध्रुव uint8_t *buf, size_t buflen,
                               शून्य *user_दत्त, शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  पूर्णाङ्क fin = (flags & NGTCP2_प्रवाहः_दत्त_FLAG_FIN) ? 1 : 0;
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  संरचना Curl_easy *दत्त =
    Curl_cf_ngtcp2_get_xfer(cf, प्रवाहः_user_दत्त, &प्रवाहः);

  (शून्य)विस्थापनम्;
  rc = nghttp3_conn_read_प्रवाहः(ctx->h3conn, प्रवाहः_id, buf, buflen, fin);
  यदि(rc < 0) {
    यदि(दत्त && प्रवाहः) {
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  प्रवाहः_id, प्रवाहः->reset, प्रवाहः->closed);
    }
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  यदि(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_प्रवाहः_विस्थापनम्(tconn, प्रवाहः_id, nconsumed);
    ngtcp2_conn_extend_max_विस्थापनम्(tconn, nconsumed);
    यदि(प्रवाहः) {
      प्रवाहः->rx_विस्थापनम् += nconsumed;
      प्रवाहः->rx_विस्थापनम्_max += nconsumed;
    }
  }
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_acked_प्रवाहः_दत्त_विस्थापनम्(ngtcp2_conn *tconn, int64_t प्रवाहः_id,
                                       uint64_t विस्थापनम्, uint64_t datalen,
                                       शून्य *user_दत्त, शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णाङ्क rv;
  (शून्य)प्रवाहः_id;
  (शून्य)tconn;
  (शून्य)विस्थापनम्;
  (शून्य)datalen;
  (शून्य)प्रवाहः_user_दत्त;

  rv = nghttp3_conn_सङ्कलय_ack_विस्थापनम्(ctx->h3conn, प्रवाहः_id, datalen);
  यदि(rv && rv != NGHTTP3_ERR_प्रवाहः_NOT_प्राप्तम्) {
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;
  }

  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_प्रवाहः_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t प्रवाहः_id, uint64_t app_error_संहिता,
                           शून्य *user_दत्त, शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त = Curl_cf_ngtcp2_get_xfer(cf, प्रवाहः_user_दत्त, रिक्त);
  पूर्णाङ्क rv;

  (शून्य)tconn;
  /* stream is closed... */
  यदि(!दत्त)
    दत्त = CF_दत्त_CURRENT(cf);
  यदि(!दत्त)
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;

  यदि(!(flags & NGTCP2_प्रवाहः_CLOSE_FLAG_APP_ERROR_संहिता_समुच्चयः)) {
    app_error_संहिता = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_प्रवाहः(ctx->h3conn, प्रवाहः_id, app_error_संहिता);
  CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", प्रवाहः_id, app_error_संहिता, rv);
  यदि(rv && rv != NGHTTP3_ERR_प्रवाहः_NOT_प्राप्तम्) {
    Curl_cf_ngtcp2_h3_err_समुच्चयः(cf, दत्त, rv);
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;
  }

  प्रत्यागम 0;
}

#यदि_परिभाषितम् NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
स्थिर पूर्णाङ्क cb_प्रवाहः_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t प्रवाहः_id,
                           uint64_t rx_app_error_संहिता,
                           uint64_t tx_app_error_संहिता,
                           शून्य *user_दत्त, शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त = Curl_cf_ngtcp2_get_xfer(cf, प्रवाहः_user_दत्त, रिक्त);
  uint64_t h3_app_error_संहिता = NGHTTP3_H3_NO_ERROR;
  पूर्णाङ्क rv;

  (शून्य)tconn;
  (शून्य)tx_app_error_संहिता;
  /* stream is closed... */
  यदि(!दत्त)
    दत्त = CF_दत्त_CURRENT(cf);
  यदि(!दत्त)
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;

  यदि(flags & NGTCP2_प्रवाहः_CLOSE2_FLAG_RX_APP_ERROR_संहिता_समुच्चयः)
    h3_app_error_संहिता = rx_app_error_संहिता;

  rv = nghttp3_conn_close_प्रवाहः(ctx->h3conn, प्रवाहः_id, h3_app_error_संहिता);
  CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", प्रवाहः_id, h3_app_error_संहिता, rv);
  यदि(rv && rv != NGHTTP3_ERR_प्रवाहः_NOT_प्राप्तम्) {
    Curl_cf_ngtcp2_h3_err_समुच्चयः(cf, दत्त, rv);
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;
  }

  प्रत्यागम 0;
}
#यदि_अन्तः

स्थिर पूर्णाङ्क cb_प्रवाहः_reset(ngtcp2_conn *tconn, int64_t प्रवाहः_id,
                           uint64_t final_आकार, uint64_t app_error_संहिता,
                           शून्य *user_दत्त, शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त = Curl_cf_ngtcp2_get_xfer(cf, प्रवाहः_user_दत्त, रिक्त);
  पूर्णाङ्क rv;
  (शून्य)tconn;
  (शून्य)final_आकार;
  (शून्य)app_error_संहिता;

  rv = nghttp3_conn_shutdown_प्रवाहः_read(ctx->h3conn, प्रवाहः_id);
  यदि(दत्त)
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] reset -> %d", प्रवाहः_id, rv);
  यदि(rv && rv != NGHTTP3_ERR_प्रवाहः_NOT_प्राप्तम्) {
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;
  }

  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_प्रवाहः_stop_sending(ngtcp2_conn *tconn, int64_t प्रवाहः_id,
                                  uint64_t app_error_संहिता, शून्य *user_दत्त,
                                  शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णाङ्क rv;
  (शून्य)tconn;
  (शून्य)app_error_संहिता;
  (शून्य)प्रवाहः_user_दत्त;

  rv = nghttp3_conn_shutdown_प्रवाहः_read(ctx->h3conn, प्रवाहः_id);
  यदि(rv && rv != NGHTTP3_ERR_प्रवाहः_NOT_प्राप्तम्) {
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;
  }

  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            शून्य *user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);

  (शून्य)tconn;
  ctx->max_bidi_streams = max_streams;
  यदि(दत्त)
    CURL_TRC_CF(दत्त, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->प्रयुक्तम्_bidi_streams);
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_extend_max_प्रवाहः_दत्त(ngtcp2_conn *tconn, int64_t प्रवाहः_id,
                                     uint64_t max_दत्त, शून्य *user_दत्त,
                                     शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  संरचना Curl_easy *दत्त =
    Curl_cf_ngtcp2_get_xfer(cf, प्रवाहः_user_दत्त, &प्रवाहः);
  पूर्णाङ्क rv;
  (शून्य)tconn;
  (शून्य)max_दत्त;

  rv = nghttp3_conn_unblock_प्रवाहः(ctx->h3conn, प्रवाहः_id);
  यदि(rv && rv != NGHTTP3_ERR_प्रवाहः_NOT_प्राप्तम्) {
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;
  }
  यदि(प्रवाहः && प्रवाहः->quic_flow_blocked) {
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] unblock quic flow", प्रवाहः_id);
    प्रवाहः->quic_flow_blocked = FALSE;
    यदि(दत्त)
      Curl_multi_mark_dirty(दत्त);
  }
  प्रत्यागम 0;
}

स्थिर शून्य cb_rand(uint8_t *dest, size_t destlen,
                    ध्रुव ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode परिणाम;
  (शून्य)rand_ctx;

  परिणाम = Curl_rand(रिक्त, dest, destlen);
  यदि(परिणाम) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    स्मृतिं_पूरय(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
स्थिर पूर्णाङ्क cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    शून्य *user_दत्त)
{
  CURLcode परिणाम;
  (शून्य)tconn;
  (शून्य)user_दत्त;

  परिणाम = Curl_rand(रिक्त, cid->दत्त, cidlen);
  यदि(परिणाम)
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  परिणाम = Curl_rand(रिक्त, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  यदि(परिणाम)
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;

  प्रत्यागम 0;
}

#यदि_परिभाषितम् NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
स्थिर पूर्णाङ्क cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  संरचना ngtcp2_stateless_reset_token *token, size_t cidlen, शून्य *user_दत्त)
{
  CURLcode परिणाम;
  (शून्य)tconn;
  (शून्य)user_दत्त;

  परिणाम = Curl_rand(रिक्त, cid->दत्त, cidlen);
  यदि(परिणाम)
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  परिणाम = Curl_rand(रिक्त, token->दत्त, परिमाण(token->दत्त));
  यदि(परिणाम)
    प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;

  प्रत्यागम 0;
}
#यदि_अन्तः

स्थिर पूर्णाङ्क cb_recv_rx_कुञ्चिका(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          शून्य *user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf ? cf->ctx : रिक्त;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  (शून्य)tconn;

  यदि(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    प्रत्यागम 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(दत्त);
  यदि(ctx && दत्त && !ctx->h3conn && ctx->init_h3_conn_cb) {
    यदि(ctx->init_h3_conn_cb(cf, दत्त, ctx))
      प्रत्यागम NGTCP2_ERR_CALLBACK_FAILURE;
  }
  प्रत्यागम 0;
}

#यदि_परिभाषितम् CURL_HAVE_DIAG
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wmissing-field-initializers"
#यदि_अन्तः
स्थिर ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  रिक्त, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_दत्त_cb,
  cb_ngtcp2_handshake_completed,
  रिक्त, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_आवरणम्_cb,
  cb_recv_प्रवाहः_दत्त,
  cb_acked_प्रवाहः_दत्त_विस्थापनम्,
  रिक्त, /* stream_open */
  cb_प्रवाहः_close,
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
  cb_प्रवाहः_reset,
  रिक्त, /* extend_max_remote_streams_bidi */
  रिक्त, /* extend_max_remote_streams_uni */
  cb_extend_max_प्रवाहः_दत्त,
  रिक्त, /* dcid_status */
  रिक्त, /* handshake_confirmed */
  रिक्त, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  रिक्त, /* recv_datagram */
  रिक्त, /* ack_datagram */
  रिक्त, /* lost_datagram */
  ngtcp2_crypto_get_मार्गः_challenge_दत्त_cb,
  cb_प्रवाहः_stop_sending,
  रिक्त, /* version_negotiation */
  cb_recv_rx_कुञ्चिका,
  रिक्त, /* recv_tx_key */
  रिक्त, /* early_data_rejected */
#यदि_परिभाषितम् NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  रिक्त, /* begin_path_validation */
#यदि_अन्तः
#यदि_परिभाषितम् NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  रिक्त, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  रिक्त, /* dcid_status2 */
  ngtcp2_crypto_get_मार्गः_challenge_data2_cb, /* get_path_challenge_data2 */
#यदि_अन्तः
#यदि_परिभाषितम् NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  रिक्त, /* recv_stop_sending */
#यदि_अन्तः
#यदि_परिभाषितम् NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_प्रवाहः_close2, /* is called instead of cb_stream_close when set */
#यदि_अन्तः
};
#यदि_परिभाषितम् CURL_HAVE_DIAG
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः

#यदि defined(_MSC_VER) && defined(_DLL)
#निर्देशः warning(निष्कासय)
#यदि_अन्तः

#यदि_परिभाषितम् USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
स्थिर पूर्णाङ्क quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  संरचना Curl_cfilter *cf;
  संरचना cf_ngtcp2_ctx *ctx;
  संरचना Curl_easy *दत्त;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_दत्त(ssl);
  cf = cref ? cref->user_दत्त : रिक्त;
  ctx = cf ? cf->ctx : रिक्त;
  दत्त = cf ? CF_दत्त_CURRENT(cf) : रिक्त;
  यदि(cf && दत्त && ctx) {
    अचिह्नित वर्ण *quic_tp = रिक्त;
    size_t quic_tp_len = 0;
#यदि_परिभाषितम् HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     परिमाण(tpbuf));
    यदि(tplen < 0)
      CURL_TRC_CF(दत्त, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((पूर्णाङ्क)tplen));
    अन्यथा {
      quic_tp = (अचिह्नित वर्ण *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#यदि_अन्तः
    Curl_ossl_सङ्कलय_session(cf, दत्त, &ctx->tls.ossl, ctx->ssl_peer.scache_कुञ्चिका,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, रिक्त);
  }
  प्रत्यागम 0;
}
#यदि_अन्तः /* USE_OPENSSL */

#यदि_परिभाषितम् USE_GNUTLS

#यदि_परिभाषितम् CURLVERBOSE
स्थिर ध्रुव वर्ण *gtls_hs_सन्देशः_name(पूर्णाङ्क mtype)
{
  विकल्प(mtype) {
  पक्ष 1:
    प्रत्यागम "ClientHello";
  पक्ष 2:
    प्रत्यागम "ServerHello";
  पक्ष 4:
    प्रत्यागम "SessionTicket";
  पक्ष 8:
    प्रत्यागम "EncryptedExtensions";
  पक्ष 11:
    प्रत्यागम "Certificate";
  पक्ष 13:
    प्रत्यागम "CertificateRequest";
  पक्ष 15:
    प्रत्यागम "CertificateVerify";
  पक्ष 20:
    प्रत्यागम "Finished";
  पक्ष 24:
    प्रत्यागम "KeyUpdate";
  पक्ष 254:
    प्रत्यागम "MessageHash";
  }
  प्रत्यागम "Unknown";
}
#यदि_अन्तः

स्थिर पूर्णाङ्क quic_gtls_handshake_cb(gnutls_session_t session, अचिह्नित पूर्णाङ्क htype,
                                  अचिह्नित when, अचिह्नित पूर्णाङ्क incoming,
                                  ध्रुव gnutls_datum_t *सन्देशः)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  संरचना Curl_cfilter *cf = conn_ref ? conn_ref->user_दत्त : रिक्त;
  संरचना cf_ngtcp2_ctx *ctx = cf ? cf->ctx : रिक्त;

  (शून्य)सन्देशः;
  (शून्य)incoming;
  यदि(when && cf && ctx) { /* after message has been processed */
    संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
    DEBUGASSERT(दत्त);
    यदि(!दत्त)
      प्रत्यागम 0;
    CURL_TRC_CF(दत्त, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_सन्देशः_name(htype), htype);
    विकल्प(htype) {
    पक्ष GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      अचिह्नित वर्ण *quic_tp = रिक्त;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       परिमाण(tpbuf));
      यदि(tplen < 0)
        CURL_TRC_CF(दत्त, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((पूर्णाङ्क)tplen));
      अन्यथा {
        quic_tp = (अचिह्नित वर्ण *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (शून्य)Curl_gtls_cache_session(cf, दत्त, ctx->ssl_peer.scache_कुञ्चिका,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    रिक्त);
      भङ्ग;
    }
    स्वभाव:
      भङ्ग;
    }
  }
  प्रत्यागम 0;
}
#यदि_अन्तः /* USE_GNUTLS */

#यदि_परिभाषितम् USE_WOLFSSL
स्थिर पूर्णाङ्क wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolf_SSL_get_app_दत्त(ssl);
  संरचना Curl_cfilter *cf = conn_ref ? conn_ref->user_दत्त : रिक्त;

  DEBUGASSERT(cf);
  यदि(cf && session) {
    संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
    संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
    DEBUGASSERT(दत्त);
    यदि(दत्त && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      अचिह्नित वर्ण *quic_tp = रिक्त;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       परिमाण(tpbuf));
      यदि(tplen < 0)
        CURL_TRC_CF(दत्त, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((पूर्णाङ्क)tplen));
      अन्यथा {
        quic_tp = (अचिह्नित वर्ण *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (शून्य)Curl_wssl_cache_session(cf, दत्त, ctx->ssl_peer.scache_कुञ्चिका,
                                    session, wolf_SSL_संस्करणम्(ssl),
                                    "h3", quic_tp, quic_tp_len, रिक्त);
    }
  }
  प्रत्यागम 0;
}
#यदि_अन्तः /* USE_WOLFSSL */

स्थिर CURLcode cf_ngtcp2_tls_ctx_सज्जता(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त,
                                        शून्य *user_दत्त)
{
  संरचना curl_tls_ctx *ctx = user_दत्त;

#यदि_परिभाषितम् USE_OPENSSL
#यदि defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  यदि(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(दत्त, "ngtcp2_crypto_boringssl_configure_client_context failed");
    प्रत्यागम CURLE_FAILED_INIT;
  }
#नोचेद्यदि defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#अन्यथा
  यदि(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(दत्त, "ngtcp2_crypto_quictls_configure_client_context failed");
    प्रत्यागम CURLE_FAILED_INIT;
  }
#यदि_अन्तः /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  यदि(Curl_ssl_scache_use(cf, दत्त)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_समुच्चयः_session_cache_बहुलकम्(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_आन्तरिकम्);
    SSL_CTX_sess_समुच्चयः_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#नोचेद्यदि defined(USE_GNUTLS)
  यदि(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(दत्त, "ngtcp2_crypto_gnutls_configure_client_session failed");
    प्रत्यागम CURLE_FAILED_INIT;
  }
  यदि(Curl_ssl_scache_use(cf, दत्त)) {
    gnutls_handshake_समुच्चयः_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#नोचेद्यदि defined(USE_WOLFSSL)
  यदि(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(दत्त, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    प्रत्यागम CURLE_FAILED_INIT;
  }
  यदि(Curl_ssl_scache_use(cf, दत्त)) {
    /* Register to get notified when a new session is received */
    wolf_SSL_CTX_sess_समुच्चयः_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#यदि_अन्तः
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_ngtcp2_on_session_reuse(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *दत्त,
                                           संरचना alpn_spec *alpns,
                                           संरचना Curl_ssl_session *scs,
                                           bool *do_early_दत्त)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  *do_early_दत्त = FALSE;
#यदि defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#यदि_अन्तः
#यदि_परिभाषितम् USE_GNUTLS
  ctx->earlydata_max =
    gnutls_लेखः_get_max_early_दत्त_आकार(ctx->tls.gtls.session);
#यदि_अन्तः
#यदि_परिभाषितम् USE_WOLFSSL
#यदि_परिभाषितम् WOLFSSL_EARLY_दत्त
  ctx->earlydata_max = scs->earlydata_max;
#अन्यथा
  ctx->earlydata_max = 0;
#यदि_अन्तः /* WOLFSSL_EARLY_DATA */
#यदि_अन्तः
#यदि defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  यदि(!ctx->earlydata_max) {
    CURL_TRC_CF(दत्त, cf, "SSL session does not allow earlydata");
  }
  अन्यथा यदि(!Curl_alpn_अन्तर्भवति_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(दत्त, cf, "SSL session from different ALPN, no early data");
  }
  अन्यथा यदि(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(दत्त, cf, "no 0RTT transport parameters, no early data");
  }
  अन्यथा {
    पूर्णाङ्क rv;
    rv = ngtcp2_conn_decode_and_समुच्चयः_0rtt_transport_params(
      ctx->qconn, (ध्रुव uint8_t *)scs->quic_tp, scs->quic_tp_len);
    यदि(rv)
      CURL_TRC_CF(दत्त, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    अन्यथा यदि(ctx->init_h3_conn_cb) {
      infof(दत्त, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      परिणाम = ctx->init_h3_conn_cb(cf, दत्त, ctx);
      यदि(!परिणाम) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_दत्त = TRUE;
      }
    }
    अन्यथा { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_दत्त = TRUE;
    }
  }
#अन्यथा /* not supported in the TLS backend */
  (शून्य)दत्त;
  (शून्य)ctx;
  (शून्य)scs;
  (शून्य)alpns;
#यदि_अन्तः
  प्रत्यागम परिणाम;
}

/*
 * Might be called twice for happy eyeballs.
 */
स्थिर CURLcode cf_connect_start(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना cf_ngtcp2_io_ctx *pktx)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णाङ्क rc;
  पूर्णाङ्क rv;
  CURLcode परिणाम;
  ध्रुव संरचना Curl_sockaddr_ex *sockaddr = रिक्त;
  पूर्णाङ्क qfd;
  स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  परिणाम = Curl_rand(दत्त, ctx->dcid.दत्त, NGTCP2_MAX_CIDLEN);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  परिणाम = Curl_rand(दत्त, ctx->scid.दत्त, NGTCP2_MAX_CIDLEN);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  (शून्य)Curl_qlogdir(दत्त, ctx->scid.दत्त, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, दत्त, pktx);

  परिणाम = Curl_vquic_ctx_init(दत्त, &ctx->q);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Query socket and remote address from sub-chain */
  यदि(Curl_cf_socket_peek(cf->अग्र, दत्त, &ctx->q.sockfd, &sockaddr, रिक्त)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  यदि(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = परिमाण(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (संरचना sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    यदि(rv == -1)
      प्रत्यागम CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_मार्गः.local,
                     (संरचना sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_मार्गः.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  अन्यथा {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    ध्रुव संरचना Curl_sockaddr_ex *remote = रिक्त;
    यदि(cf->अग्र->cft->query(cf->अग्र, दत्त, CF_QUERY_REMOTE_ADDR, रिक्त,
                            CURL_UNCONST(&remote)))
      प्रत्यागम CURLE_QUIC_CONNECT_ERROR;
    यदि(!remote)
      प्रत्यागम CURLE_QUIC_CONNECT_ERROR;

    स्मृतिं_पूरय(&ctx->q.local_addr, 0, परिमाण(ctx->q.local_addr));
    विकल्प(remote->family) {
    पक्ष AF_INET:
      ((संरचना sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = परिमाण(संरचना sockaddr_in);
      भङ्ग;
#यदि_परिभाषितम् USE_IPV6
    पक्ष AF_INET6:
      ((संरचना sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = परिमाण(संरचना sockaddr_in6);
      भङ्ग;
#यदि_अन्तः
    स्वभाव:
      प्रत्यागम CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_मार्गः.local,
                     (संरचना sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_मार्गः.remote,
                     &remote->curl_sa_addr,
                     (socklen_t)remote->addrlen);
  }

  rc = ngtcp2_conn_client_new(&ctx->qconn, &ctx->dcid, &ctx->scid,
                              &ctx->connected_मार्गः,
                              NGTCP2_PROTO_VER_V1, &ng_callbacks,
                              &ctx->settings, &ctx->transport_params,
                              Curl_ngtcp2_mem(), cf);
  यदि(rc)
    प्रत्यागम CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_दत्त = cf;

  परिणाम = Curl_vquic_tls_init(&ctx->tls, cf, दत्त,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_सज्जता, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

#यदि defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  यदि(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(दत्त, "ngtcp2_crypto_ossl_ctx_new failed");
    प्रत्यागम CURLE_FAILED_INIT;
  }
  ngtcp2_conn_समुच्चयः_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  यदि(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(दत्त, "ngtcp2_crypto_ossl_configure_client_session failed");
    प्रत्यागम CURLE_FAILED_INIT;
  }
#नोचेद्यदि defined(USE_OPENSSL)
  SSL_समुच्चयः_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_समुच्चयः_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#नोचेद्यदि defined(USE_GNUTLS)
  ngtcp2_conn_समुच्चयः_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#नोचेद्यदि defined(USE_WOLFSSL)
  ngtcp2_conn_समुच्चयः_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#अन्यथा
#दोषः "ngtcp2 TLS backend not defined"
#यदि_अन्तः

#यदि defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  यदि(ctx->use_earlydata &&
     !SSL_समुच्चयः_quic_tls_early_दत्त_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(दत्त, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#यदि_अन्तः
  ngtcp2_ccerr_default(&ctx->चरमः_error);

  प्रत्यागम CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    bool *समाप्तम्)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  संरचना cf_call_दत्त save;
  संरचना cf_ngtcp2_io_ctx pktx;

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  /* Connect the sub-chain */
  यदि(cf->अग्र && !cf->अग्र->connected) {
    परिणाम = Curl_conn_cf_connect(cf->अग्र, दत्त, समाप्तम्);
    यदि(परिणाम || !*समाप्तम्)
      प्रत्यागम परिणाम;
  }

  *समाप्तम् = FALSE;

  यदि(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(दत्त, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(दत्त, cf, "need HTTPS-RR, delaying connect");
    प्रत्यागम CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, दत्त);
  CF_दत्त_SAVE(save, cf, दत्त);

  यदि(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(दत्त);
    परिणाम = cf_connect_start(cf, दत्त, &pktx);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    यदि(cf->connected) {
      *समाप्तम् = TRUE;
      गच्छ बहिर्गमनम्;
    }
    परिणाम = Curl_cf_ngtcp2_प्रगतिः_egress(cf, दत्त, &pktx);
    /* we do not expect to be able to recv anything yet */
    गच्छ बहिर्गमनम्;
  }

  परिणाम = Curl_cf_ngtcp2_प्रगतिः_ingress(cf, दत्त, &pktx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = Curl_cf_ngtcp2_प्रगतिः_egress(cf, दत्त, &pktx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यदि(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    परिणाम = ctx->tls_vrfy_परिणाम;
    यदि(!परिणाम) {
      CURL_TRC_CF(दत्त, cf, "peer verified");
      cf->connected = TRUE;
      *समाप्तम् = TRUE;
    }
  }

बहिर्गमनम्:
  यदि(ctx->tls_vrfy_परिणाम)
    परिणाम = ctx->tls_vrfy_परिणाम;
  यदि(ctx->qconn &&
     ((परिणाम == CURLE_RECV_ERROR) || (परिणाम == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_अन्तर्_draining_period(ctx->qconn)) {
    ध्रुव ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    परिणाम = CURLE_COULDNT_CONNECT;
    यदि(cerr) {
      CURL_TRC_CF(दत्त, cf, "connect error, type=%d, code=%" PRIu64,
                  (पूर्णाङ्क)cerr->type, cerr->error_संहिता);
      विकल्प(cerr->type) {
      पक्ष NGTCP2_CCERR_TYPE_संस्करणम्_NEGOTIATION:
        CURL_TRC_CF(दत्त, cf, "error in version negotiation");
        भङ्ग;
      स्वभाव:
        यदि(cerr->error_संहिता >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(दत्त, cf, "crypto error, tls alert=%u",
                      (अचिह्नित पूर्णाङ्क)(cerr->error_संहिता & 0xffU));
        }
        अन्यथा यदि(cerr->error_संहिता == NGTCP2_CONNECTION_REFUSED) {
          CURL_TRC_CF(दत्त, cf, "connection refused by server");
          /* When a QUIC server instance is shutting down, it may send us a
           * CONNECTION_CLOSE with this code right away. We want
           * to keep on trying in this case. */
          परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
        }
      }
    }
  }

#यदि_परिभाषितम् CURLVERBOSE
  यदि(परिणाम) {
    यदि(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      संरचना ip_quadruple ip;

      यदि(!Curl_cf_socket_peek(cf->अग्र, दत्त, रिक्त, रिक्त, &ip))
        infof(दत्त, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_पत्तनम्, curl_easy_strerror(परिणाम));
    }
  }
#यदि_अन्तः
  यदि(!परिणाम && ctx->qconn) {
    परिणाम = Curl_cf_ngtcp2_cmn_समुच्चयः_expiry(cf, दत्त, &pktx);
  }
  यदि(परिणाम || *समाप्तम्)
    CURL_TRC_CF(दत्त, cf, "connect -> %d, done=%d", (पूर्णाङ्क)परिणाम, *समाप्तम्);
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना cf_call_दत्त save;
  संरचना cf_ngtcp2_io_ctx pktx;
  CURLcode परिणाम = CURLE_OK;

  यदि(cf->shutdown || !ctx->qconn) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  यदि(!cf->अग्र) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  CF_दत्त_SAVE(save, cf, दत्त);
  *समाप्तम् = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, दत्त);

  यदि(!ctx->shutdown_started) {
    वर्ण बफर[NGTCP2_MAX_UDP_PAYLOAD_आकार];
    ngtcp2_ssize nwritten;

    यदि(!Curl_bufq_is_रिक्तानि(&ctx->q.sendbuf)) {
      CURL_TRC_CF(दत्त, cf, "shutdown, flushing sendbuf");
      परिणाम = Curl_cf_ngtcp2_प्रगतिः_egress(cf, दत्त, &pktx);
      यदि(!Curl_bufq_is_रिक्तानि(&ctx->q.sendbuf)) {
        CURL_TRC_CF(दत्त, cf, "sending shutdown packets blocked");
        परिणाम = CURLE_OK;
        गच्छ बहिर्गमनम्;
      }
      अन्यथा यदि(परिणाम) {
        CURL_TRC_CF(दत्त, cf, "shutdown, error %d flushing sendbuf",
                    (पूर्णाङ्क)परिणाम);
        *समाप्तम् = TRUE;
        गच्छ बहिर्गमनम्;
      }
    }

    DEBUGASSERT(Curl_bufq_is_रिक्तानि(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, रिक्त, /*मार्गः*/
      रिक्त, /* pkt_info */
      (uint8_t *)बफर, परिमाण(बफर),
      &ctx->चरमः_error, pktx.ts);
    CURL_TRC_CF(दत्त, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (पूर्णाङ्क)ctx->चरमः_error.type,
                ctx->चरमः_error.error_संहिता, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    यदि(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      परिणाम = Curl_bufq_write(&ctx->q.sendbuf, (ध्रुव अचिह्नित वर्ण *)बफर,
                               (size_t)nwritten, &n);
      यदि(परिणाम) {
        CURL_TRC_CF(दत्त, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (पूर्णाङ्क)परिणाम);
        गच्छ बहिर्गमनम्;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.विभज_len = 0;
    }
  }

  यदि(!Curl_bufq_is_रिक्तानि(&ctx->q.sendbuf)) {
    CURL_TRC_CF(दत्त, cf, "shutdown, flushing egress");
    परिणाम = Curl_vquic_flush(cf, दत्त, &ctx->q);
    यदि(परिणाम == CURLE_AGAIN) {
      CURL_TRC_CF(दत्त, cf, "sending shutdown packets blocked");
      परिणाम = CURLE_OK;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(परिणाम) {
      CURL_TRC_CF(दत्त, cf, "shutdown, error %d flushing sendbuf",
                  (पूर्णाङ्क)परिणाम);
      *समाप्तम् = TRUE;
      गच्छ बहिर्गमनम्;
    }
  }

  यदि(Curl_bufq_is_रिक्तानि(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(दत्त, cf, "shutdown completely sent off, done");
    *समाप्तम् = TRUE;
    परिणाम = CURLE_OK;
  }
बहिर्गमनम्:
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

शून्य Curl_cf_ngtcp2_cmn_conn_close(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  bool समाप्तम्;
  Curl_cf_ngtcp2_cmn_shutdown(cf, दत्त, &समाप्तम्);
}

स्थिर bool cf_ngtcp2_err_is_fatal(पूर्णाङ्क संहिता)
{
  प्रत्यागम (NGTCP2_ERR_FATAL >= संहिता) ||
         (NGTCP2_ERR_DROP_CONN == संहिता) ||
         (NGTCP2_ERR_IDLE_CLOSE == संहिता);
}

शून्य Curl_cf_ngtcp2_cmn_err_समुच्चयः(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त, पूर्णाङ्क संहिता)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  यदि(!ctx->चरमः_error.error_संहिता) {
    यदि(NGTCP2_ERR_CRYPTO == संहिता) {
      ngtcp2_ccerr_समुच्चयः_tls_alert(&ctx->चरमः_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 रिक्त, 0);
    }
    अन्यथा {
      ngtcp2_ccerr_समुच्चयः_liberr(&ctx->चरमः_error, संहिता, रिक्त, 0);
    }
  }
  यदि(cf_ngtcp2_err_is_fatal(संहिता))
    Curl_cf_ngtcp2_cmn_conn_close(cf, दत्त);
}

शून्य Curl_cf_ngtcp2_io_ctx_init(संरचना cf_ngtcp2_io_ctx *io_ctx,
                                संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->दत्त = दत्त;
  io_ctx->now = *Curl_pgrs_now(दत्त);
  ngtcp2_मार्गः_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_समुच्चयः_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

शून्य Curl_cf_ngtcp2_io_ctx_update_time(संरचना Curl_easy *दत्त,
                                       संरचना cf_ngtcp2_io_ctx *io_ctx,
                                       संरचना Curl_cfilter *cf)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(दत्त);
  Curl_vquic_ctx_update_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#यदि NGTCP2_संस्करणम्_अङ्कः < 0x011100
संरचना cf_ngtcp2_sfind_ctx {
  int64_t प्रवाहः_id;
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  uint32_t mid;
};

स्थिर bool cf_ngtcp2_sfind(uint32_t mid, शून्य *मानम्, शून्य *user_दत्त)
{
  संरचना cf_ngtcp2_sfind_ctx *fctx = user_दत्त;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = मानम्;

  यदि(fctx->प्रवाहः_id == प्रवाहः->id) {
    fctx->mid = mid;
    fctx->प्रवाहः = प्रवाहः;
    प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE; /* continue */
}

स्थिर संरचना h3_प्रवाहः_ctx *cf_ngtcp2_get_प्रवाहः(संरचना cf_ngtcp2_ctx *ctx,
                                                  int64_t प्रवाहः_id)
{
  संरचना cf_ngtcp2_sfind_ctx fctx;
  fctx.प्रवाहः_id = प्रवाहः_id;
  fctx.प्रवाहः = रिक्त;
  Curl_u32_ptrset_दर्शय(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  प्रत्यागम fctx.प्रवाहः;
}
#अन्यथा
स्थिर संरचना h3_प्रवाहः_ctx *cf_ngtcp2_get_प्रवाहः(संरचना cf_ngtcp2_ctx *ctx,
                                                  int64_t प्रवाहः_id)
{
  प्रत्यागम ngtcp2_conn_get_प्रवाहः_user_दत्त(ctx->qconn, प्रवाहः_id);
}
#यदि_अन्तः

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
स्थिर CURLcode read_pkt_गन्तव्यम्_send(शून्य *userp,
                                 अचिह्नित वर्ण *buf, size_t buflen,
                                 size_t *pnread)
{
  संरचना cf_ngtcp2_io_ctx *x = userp;
  संरचना cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t प्रवाहः_id;
  पूर्णाङ्क fin;
  ssize_t n;

  *pnread = 0;
  veccnt = 0;
  प्रवाहः_id = -1;
  fin = 0;

  /* ngtcp2 may want to put several frames from different streams into
   * this packet. `NGTCP2_WRITE_STREAM_FLAG_MORE` tells it to do so.
   * When `NGTCP2_ERR_WRITE_MORE` is returned, we *need* to make
   * another iteration.
   * When ngtcp2 is happy (because it has no other frame that would fit
   * or it has nothing more to send), it returns the total length
   * of the assembled packet. This may be 0 if there was nothing to send. */
  प्रति(;;) {

    यदि(ctx->h3conn && ngtcp2_conn_get_max_दत्त_वामः(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_प्रवाहः(ctx->h3conn, &प्रवाहः_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      यदि(veccnt < 0) {
        failf(x->दत्त, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((पूर्णाङ्क)veccnt));
        Curl_cf_ngtcp2_h3_err_समुच्चयः(x->cf, x->दत्त, (पूर्णाङ्क)veccnt);
        प्रत्यागम CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_प्रवाहः_FLAG_अधिकम् |
            (fin ? NGTCP2_WRITE_प्रवाहः_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_प्रवाहः(ctx->qconn, &x->ps.मार्गः,
                                  रिक्त, buf, buflen,
                                  &ndatalen, flags, प्रवाहः_id,
                                  (ध्रुव ngtcp2_vec *)vec, veccnt, x->ts);
    यदि(n == 0) {
      /* nothing to send */
      प्रत्यागम CURLE_AGAIN;
    }
    अन्यथा यदि(n < 0) {
      विकल्प(n) {
      पक्ष NGTCP2_ERR_प्रवाहः_दत्त_BLOCKED: {
        संरचना h3_प्रवाहः_ctx *प्रवाहः;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_खण्डम्_प्रवाहः(ctx->h3conn, प्रवाहः_id);
        CURL_TRC_CF(x->दत्त, x->cf, "[%" PRId64 "] block quic flow",
                    प्रवाहः_id);
        प्रवाहः = cf_ngtcp2_get_प्रवाहः(ctx, प्रवाहः_id);
        यदि(प्रवाहः) /* it might be not one of our h3 streams? */
          प्रवाहः->quic_flow_blocked = TRUE;
        n = 0;
        भङ्ग;
      }
      पक्ष NGTCP2_ERR_प्रवाहः_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_प्रवाहः_write(ctx->h3conn, प्रवाहः_id);
        n = 0;
        भङ्ग;
      पक्ष NGTCP2_ERR_WRITE_अधिकम्:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        भङ्ग;
      स्वभाव:
        DEBUGASSERT(ndatalen == -1);
        failf(x->दत्त, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((पूर्णाङ्क)n));
        Curl_cf_ngtcp2_cmn_err_समुच्चयः(x->cf, x->दत्त, (पूर्णाङ्क)n);
        प्रत्यागम CURLE_SEND_ERROR;
      }
    }

    यदि(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      पूर्णाङ्क rv = nghttp3_conn_सङ्कलय_write_विस्थापनम्(ctx->h3conn, प्रवाहः_id, ndatalen);
      यदि(rv) {
        failf(x->दत्त, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        प्रत्यागम CURLE_SEND_ERROR;
      }
    }

    यदि(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      प्रत्यागम CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_प्रगतिः_egress(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त,
                                        संरचना cf_ngtcp2_io_ctx *pktx)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_आकार, मार्गः_max_payload_आकार;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_कालांशः;
  CURLcode परिणाम;
  संरचना cf_ngtcp2_io_ctx local_pktx;

  यदि(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, दत्त);
    pktx = &local_pktx;
  }
  अन्यथा {
    Curl_cf_ngtcp2_io_ctx_update_time(दत्त, pktx, cf);
    ngtcp2_मार्गः_storage_zero(&pktx->ps);
  }

  परिणाम = Curl_vquic_flush(cf, दत्त, &ctx->q);
  यदि(परिणाम) {
    यदि(परिणाम == CURLE_AGAIN) {
      Curl_expire_समुच्चयः(दत्त, EXPIRE_QUIC, 1, &pktx->now);
      प्रत्यागम CURLE_OK;
    }
    प्रत्यागम परिणाम;
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
  मार्गः_max_payload_आकार =
    ngtcp2_conn_get_मार्गः_max_tx_udp_payload_आकार(ctx->qconn);
  send_कालांशः = ngtcp2_conn_get_send_कालांशः(ctx->qconn);
  CURL_TRC_CF(दत्त, cf, "egress, collect and send packets, quantum=%zu",
              send_कालांशः);
  प्रति(;;) {
    /* add the next packet to send, if any, to our buffer */
    परिणाम = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_आकार,
                            read_pkt_गन्तव्यम्_send, pktx, &nread);
    यदि(परिणाम == CURLE_AGAIN)
      भङ्ग;
    अन्यथा यदि(परिणाम)
      प्रत्यागम परिणाम;
    अन्यथा {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      यदि((buflen >= send_कालांशः) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_आकार))
        भङ्ग;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      यदि(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      अन्यथा यदि(nread > gsolen ||
              (gsolen > मार्गः_max_payload_आकार && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        परिणाम = Curl_vquic_send_पुच्छ_विभज(cf, दत्त, &ctx->q,
                                            gsolen, nread, nread);
        यदि(परिणाम) {
          यदि(परिणाम == CURLE_AGAIN) {
            Curl_expire_समुच्चयः(दत्त, EXPIRE_QUIC, 1, &pktx->now);
            प्रत्यागम CURLE_OK;
          }
          प्रत्यागम परिणाम;
        }
        pktcnt = 0;
      }
      अन्यथा यदि(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        भङ्ग;
      }
    }
  }

  यदि(!Curl_bufq_is_रिक्तानि(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(दत्त, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    परिणाम = Curl_vquic_send(cf, दत्त, &ctx->q, gsolen);
    यदि(परिणाम) {
      यदि(परिणाम == CURLE_AGAIN) {
        Curl_expire_समुच्चयः(दत्त, EXPIRE_QUIC, 1, &pktx->now);
        प्रत्यागम CURLE_OK;
      }
      प्रत्यागम परिणाम;
    }
    Curl_cf_ngtcp2_io_ctx_update_time(दत्त, pktx, cf);
    ngtcp2_conn_update_pkt_tx_time(ctx->qconn, pktx->ts);
  }
  प्रत्यागम CURLE_OK;
}

संरचना cf_ngtcp2_recv_ctx {
  संरचना cf_ngtcp2_io_ctx *pktx;
  size_t pkt_गणनम्;
};

स्थिर CURLcode cf_ngtcp2_recv_pkts(ध्रुव अचिह्नित वर्ण *buf, size_t buflen,
                                    size_t gso_आकार,
                                    संरचना sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    शून्य *userp)
{
  संरचना cf_ngtcp2_recv_ctx *rctx = userp;
  संरचना cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  संरचना cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_मार्गः मार्गः;
  size_t विस्थापनम्, pktlen;
  पूर्णाङ्क rv;

  यदि(!rctx->pkt_गणनम्) {
    Curl_cf_ngtcp2_io_ctx_update_time(pktx->दत्त, pktx, pktx->cf);
    ngtcp2_मार्गः_storage_zero(&pktx->ps);
  }

  यदि(ecn)
    CURL_TRC_CF(pktx->दत्त, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_आकार, (अचिह्नित)ecn);
  ngtcp2_addr_init(&मार्गः.local, (संरचना sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&मार्गः.remote, (संरचना sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  प्रति(विस्थापनम् = 0; विस्थापनम् < buflen; विस्थापनम् += gso_आकार) {
    rctx->pkt_गणनम्++;
    pktlen = ((विस्थापनम् + gso_आकार) <= buflen) ? gso_आकार : (buflen - विस्थापनम्);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &मार्गः, &pi,
                              buf + विस्थापनम्, pktlen, pktx->ts);
    यदि(rv) {
      CURL_TRC_CF(pktx->दत्त, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_समुच्चयः(pktx->cf, pktx->दत्त, rv);

      यदि(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
      प्रत्यागम CURLE_RECV_ERROR;
    }
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_प्रगतिः_ingress(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *दत्त,
                                         संरचना cf_ngtcp2_io_ctx *pktx)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना cf_ngtcp2_io_ctx local_pktx;
  संरचना cf_ngtcp2_recv_ctx rctx;
  CURLcode परिणाम = CURLE_OK;

  यदि(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, दत्त);
    pktx = &local_pktx;
  }

  परिणाम = Curl_vquic_tls_before_recv(&ctx->tls, cf, दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  rctx.pktx = pktx;
  rctx.pkt_गणनम् = 0;

  यदि(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(दत्त, cf, "progress_ingress(socket)");
    प्रत्यागम Curl_vquic_recv_packets(cf, दत्त, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  अन्यथा {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    अचिह्नित वर्ण *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_आकार;
    size_t pkt_सीमा = QUIC_TUNNEL_INGRESS_PKT_सीमा;
    size_t nread;
    संरचना sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(दत्त, cf, "progress_ingress(sub-filters)");
    यदि(ctx->qconn) {
      size_t max_मार्गः_payload;
      max_मार्गः_payload =
        ngtcp2_conn_get_मार्गः_max_tx_udp_payload_आकार(ctx->qconn);
      यदि(max_मार्गः_payload > max_udp_payload)
        max_udp_payload = max_मार्गः_payload;
    }

    यदि(ctx->tunnel_inbuf_len < max_udp_payload) {
      अचिह्नित वर्ण *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      यदि(!newbuf)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    यावत्(pkt_सीमा--) {
      परिणाम = Curl_conn_cf_recv(cf->अग्र, दत्त, (वर्ण *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      यदि(परिणाम == CURLE_AGAIN) {
        /* no more data available at the moment */
        प्रत्यागम CURLE_OK;
      }
      यदि(परिणाम) {
        CURL_TRC_CF(दत्त, cf, "ingress, recv from tunnel failed: %d",
                    (पूर्णाङ्क)परिणाम);
        प्रत्यागम परिणाम;
      }
      यदि(nread == 0) {
        /* tunnel closed */
        प्रत्यागम CURLE_OK;
      }

      स्मृतिं_प्रतिलिख(&remote_addr, ctx->connected_मार्गः.remote.addr,
             ctx->connected_मार्गः.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_मार्गः.remote.addrlen;
      परिणाम = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      यदि(!ctx->q.got_first_byte) {
        ctx->q.got_first_byte = TRUE;
        ctx->q.first_byte_at = ctx->q.चरमः_op;
      }
      ctx->q.चरमः_io = ctx->q.चरमः_op;
    }
    प्रत्यागम CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_समुच्चयः_expiry(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त,
                                       संरचना cf_ngtcp2_io_ctx *pktx)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  यदि(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, दत्त);
    pktx = &local_pktx;
  }
  अन्यथा {
    Curl_cf_ngtcp2_io_ctx_update_time(दत्त, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  यदि(expiry != UINT64_MAX) {
    यदि(expiry <= pktx->ts) {
      CURLcode परिणाम;
      पूर्णाङ्क rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      यदि(rv) {
        failf(दत्त, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_समुच्चयः(cf, दत्त, rv);
        प्रत्यागम CURLE_SEND_ERROR;
      }
      परिणाम = Curl_cf_ngtcp2_प्रगतिः_ingress(cf, दत्त, pktx);
      यदि(!परिणाम)
        परिणाम = Curl_cf_ngtcp2_प्रगतिः_egress(cf, दत्त, pktx);
      यदि(परिणाम) {
        /* a verify failure during ingress must win over generic errors */
        यदि(ctx->tls_vrfy_परिणाम)
          परिणाम = ctx->tls_vrfy_परिणाम;
        प्रत्यागम परिणाम;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    यदि(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      यदि(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_समुच्चयः(दत्त, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cf_ngtcp2_सज्जता_keep_alive(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त)
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
  यदि(!ctx->qconn)
    प्रत्यागम;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  यदि(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_समुच्चयः_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(दत्त, cf, "no peer idle timeout, unset keep-alive");
  }
  अन्यथा यदि(!Curl_u32_ptrset_गणनम्(&ctx->streams)) {
    ngtcp2_conn_समुच्चयः_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(दत्त, cf, "no active streams, unset keep-alive");
  }
  अन्यथा {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_समुच्चयः_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(दत्त, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_प्रवाहः_सज्जता(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त,
                                        संरचना h3_प्रवाहः_ctx **pstream)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);

  *pstream = रिक्त;
  यदि(!दत्त)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(प्रवाहः) {
    *pstream = प्रवाहः;
    प्रत्यागम CURLE_OK;
  }

  प्रवाहः = curlx_calloc(1, परिमाण(*प्रवाहः));
  यदि(!प्रवाहः)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रवाहः->id = -1;
  प्रवाहः->xfer_id = दत्त->id;
  प्रवाहः->mid = दत्त->mid;
  प्रवाहः->rx_विस्थापनम् = 0;
  प्रवाहः->rx_विस्थापनम्_max = H3_प्रवाहः_गवाक्षः_आकार_INITIAL;
  प्रवाहः->tx_अन्तर्_flight_ideal = H3_प्रवाहः_SEND_BUF_INITIAL;

  /* on send, we control how much we put into the buffer */
  Curl_bufq_initp(&प्रवाहः->sendbuf, &ctx->प्रवाहः_bufcp,
                  H3_प्रवाहः_SEND_CHUNKS, BUFQ_OPT_NONE);
  प्रवाहः->tx_अन्तर्_flight_आकार = 0;
  Curl_h1_req_parse_init(&प्रवाहः->h1, H1_PARSE_DEFAULT_MAX_पङ्क्तिः_LEN);

  यदि(Curl_u32_ptrset_समुच्चयः(&ctx->streams, दत्त->mid, प्रवाहः)) {
    Curl_cf_ngtcp2_h3_प्रवाहः_ctx_free(प्रवाहः);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  यदि(Curl_u32_ptrset_गणनम्(&ctx->streams) == 1)
    cf_ngtcp2_सज्जता_keep_alive(cf, दत्त);

  *pstream = प्रवाहः;
  प्रत्यागम CURLE_OK;
}

शून्य Curl_cf_ngtcp2_h3_प्रवाहः_close(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    संरचना h3_प्रवाहः_ctx *प्रवाहः)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(दत्त);
  DEBUGASSERT(प्रवाहः);
  यदि(!प्रवाहः->closed && ctx->qconn && ctx->h3conn) {
    CURLcode परिणाम;

    nghttp3_conn_समुच्चयः_प्रवाहः_user_दत्त(ctx->h3conn, प्रवाहः->id, रिक्त);
    ngtcp2_conn_समुच्चयः_प्रवाहः_user_दत्त(ctx->qconn, प्रवाहः->id, रिक्त);
    प्रवाहः->closed = TRUE;
    (शून्य)ngtcp2_conn_shutdown_प्रवाहः(ctx->qconn, 0, प्रवाहः->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    परिणाम = Curl_cf_ngtcp2_प्रगतिः_egress(cf, दत्त, रिक्त);
    यदि(परिणाम)
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] cancel stream -> %d",
                  प्रवाहः->id, (पूर्णाङ्क)परिणाम);
  }
}

शून्य Curl_cf_ngtcp2_h3_प्रवाहः_समाप्तम्(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
  (शून्य)cf;
  यदि(प्रवाहः) {
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] easy handle is done", प्रवाहः->id);
    Curl_cf_ngtcp2_h3_प्रवाहः_close(cf, दत्त, प्रवाहः);
    Curl_u32_ptrset_unset(&ctx->streams, दत्त->mid);
    यदि(!Curl_u32_ptrset_गणनम्(&ctx->streams))
      cf_ngtcp2_सज्जता_keep_alive(cf, दत्त);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      bool *निवेशः_pending)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  ध्रुव ngtcp2_transport_params *rp;
  संरचना cf_call_दत्त save;

  CF_दत्त_SAVE(save, cf, दत्त);
  *निवेशः_pending = FALSE;
  यदि(!ctx->qconn || ctx->shutdown_started)
    गच्छ बहिर्गमनम्;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  यदि(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &ctx->q.चरमः_io);
    यदि(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      यदि((uint64_t)idletime_ms > max_idle_ms)
        गच्छ बहिर्गमनम्;
    }
  }

  यदि(!cf->अग्र || !cf->अग्र->cft->is_alive(cf->अग्र, दत्त, निवेशः_pending))
    गच्छ बहिर्गमनम्;

  alive = TRUE;
  यदि(*निवेशः_pending) {
    CURLcode परिणाम;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *निवेशः_pending = FALSE;
    परिणाम = Curl_cf_ngtcp2_प्रगतिः_ingress(cf, दत्त, रिक्त);
    CURL_TRC_CF(दत्त, cf, "is_alive, progress ingress -> %d", (पूर्णाङ्क)परिणाम);
    alive = परिणाम ? FALSE : TRUE;
  }

बहिर्गमनम्:
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(संरचना cf_ngtcp2_ctx *ctx,
                                      संरचना Curl_easy *दत्त)
{
  int64_t ctrl_प्रवाहः_id, qpack_enc_प्रवाहः_id, qpack_dec_प्रवाहः_id;
  पूर्णाङ्क rc;

  rc = ngtcp2_conn_open_uni_प्रवाहः(ctx->qconn, &ctrl_प्रवाहः_id, रिक्त);
  यदि(rc) {
    failf(दत्त, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    प्रत्यागम CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_प्रवाहः(ctx->h3conn, ctrl_प्रवाहः_id);
  यदि(rc) {
    failf(दत्त, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    प्रत्यागम CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_प्रवाहः(ctx->qconn, &qpack_enc_प्रवाहः_id, रिक्त);
  यदि(rc) {
    failf(दत्त, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    प्रत्यागम CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_प्रवाहः(ctx->qconn, &qpack_dec_प्रवाहः_id, रिक्त);
  यदि(rc) {
    failf(दत्त, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    प्रत्यागम CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_प्रवाहः_id,
                                       qpack_dec_प्रवाहः_id);
  यदि(rc) {
    failf(दत्त, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    प्रत्यागम CURLE_QUIC_CONNECT_ERROR;
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;

  विकल्प(query) {
  पक्ष CF_QUERY_CONNECT_प्रत्युत्तरम्_MS:
    यदि((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < अधिकतम_पूर्णाङ्कः) ? (पूर्णाङ्क)ms : अधिकतम_पूर्णाङ्कः;
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  पक्ष CF_QUERY_REALLY_CONNECTED:
    यदि(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_first_byte;
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, दत्त, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  (शून्य)arg1;
  (शून्य)arg2;
  विकल्प(event) {
  पक्ष CF_CTRL_REPORT_STATS:
    यदि(cf->connected && !ctx->stats_reported) {
      यदि((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
        Curl_pgrsTimeWas(दत्त, TIMER_CONNECT, ctx->q.first_byte_at);
        ctx->stats_reported = TRUE;
      }
      अन्यथा यदि(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        यदि(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrsTimeWas(दत्त, TIMER_CONNECT, ctx->q.first_byte_at);
        Curl_pgrsTimeWas(दत्त, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
