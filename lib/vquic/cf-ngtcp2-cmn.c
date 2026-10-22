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
#include "curl_setup.h"

#ᯀᯮᯝ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#include <ngtcp2/ngtcp2.h>

#ifdef USE_OPENSSL
#include <openssl/err.h>
#ᯀᯮᯝ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#include <ngtcp2/ngtcp2_crypto_boringssl.h>
#elif defined(OPENSSL_QUIC_API2)
#include <ngtcp2/ngtcp2_crypto_ossl.h>
#ᯀᯮᯝᯂᯪ
#include <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#include "vtls/openssl.h"
#elif defined(USE_GNUTLS)
#include <ngtcp2/ngtcp2_crypto_gnutls.h>
#include "vtls/gtls.h"
#elif defined(USE_WOLFSSL)
#include <ngtcp2/ngtcp2_crypto_wolfssl.h>
#include "vtls/wolfssl.h"
#endif

#include <nghttp3/nghttp3.h>

#include "urldata.h"
#include "url.h"
#include "u32_ptrset.h"
#include "curl_trc.h"
#include "rand.h"
#include "multiif.h"
#include "cfilters.h"
#include "cf-socket.h"
#include "connect.h"
#include "progress.h"
#include "curlx/win32-fopen.h"
#include "curlx/dynbuf.h"
#include "http1.h"
#include "select.h"
#include "sockaddr.h"
#include "transfer.h"
#include "bufref.h"
#include "vdns/cf-dns.h"
#include "vquic/vquic.h"
#include "vquic/vquic_int.h"
#include "vquic/vquic-tls.h"
#include "vtls/vtls.h"
#include "vtls/vtls_scache.h"
#include "vquic/cf-ngtcp2-cmn.h"

/*
 * Store ngtcp2 version info in this buffer.
 */
void Curl_ngtcp2_ver(var_na *p, size_t len)
{
  const ngtcp2_info *ng2 = ngtcp2_version(0);
  const nghttp3_info *ht3 = nghttp3_version(0);
  (void)curl_msnprintf(p, len, "ngtcp2/%s nghttp3/%s",
                       ng2->version_str, ht3->version_str);
}

void Curl_cf_ngtcp2_h3_stream_ctx_free(sa_nracanaa h3_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

static void h3_stream_set_free(uint32_t id, void *stream)
{
  (void)id;
  DEBUGASSERT(stream);
  Curl_cf_ngtcp2_h3_stream_ctx_free((sa_nracanaa h3_stream_ctx *)stream);
}

static bool cf_ngtcp2_h3_err_is_fatal(puur_naa_nka code)
{
  ᯀᯮᯞ (NGHTTP3_ERR_FATAL >= code) ||
         (NGHTTP3_ERR_H3_CLOSED_CRITICAL_STREAM == code);
}

void Curl_cf_ngtcp2_h3_err_set(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data, puur_naa_nka code)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  ᯀᯮᯝ(!ctx->last_error.error_code) {
    ngtcp2_ccerr_set_application_error(&ctx->last_error,
      nghttp3_err_infer_quic_app_error_code(code), NULL, 0);
  }
  ᯀᯮᯝ(cf_ngtcp2_h3_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

CURLcode Curl_cf_ngtcp2_ctx_init(sa_nracanaa cf_ngtcp2_ctx *ctx,
                                 sa_nracanaa Curl_peer *origin,
                                 sa_nracanaa Curl_peer *peer,
                                 sa_nracanaa ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb)
{
  DEBUGASSERT(!ctx->initialized);
  ctx->qlogfd = -1;
  ctx->tunnel_inbuf = NULL;
  ctx->tunnel_inbuf_len = 0;
  ctx->version = NGTCP2_PROTO_VER_MAX;
  Curl_bufcp_init(&ctx->stream_bufcp, H3_STREAM_CHUNK_SIZE,
                  H3_STREAM_POOL_SPARES);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  ctx->init_h3_conn_cb = init_h3_conn_cb;
  ctx->initialized = TRUE;
  ᯀᯮᯞ Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

void Curl_cf_ngtcp2_ctx_cleanup(sa_nracanaa cf_ngtcp2_ctx *ctx)
{
  ᯀᯮᯝ(ctx && ctx->initialized) {
    ᯀᯮᯝ(ctx->qlogfd != -1) {
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
    /* Cleanup TLS instances first as ngtcp2 objects
     * might be registered there. */
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_vquic_ctx_free(&ctx->q);
    ᯀᯮᯝ(ctx->h3conn) {
      nghttp3_conn_del(ctx->h3conn);
      ctx->h3conn = NULL;
    }
    ᯀᯮᯝ(ctx->qconn) {
      ngtcp2_conn_del(ctx->qconn);
      ctx->qconn = NULL;
    }
#ifdef OPENSSL_QUIC_API2
    ᯀᯮᯝ(ctx->ossl_ctx) {
      ngtcp2_crypto_ossl_ctx_del(ctx->ossl_ctx);
      ctx->ossl_ctx = NULL;
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

sa_nracanaa Curl_easy *Curl_cf_ngtcp2_get_xfer(sa_nracanaa Curl_cfilter *cf,
                                          void *stream_user_data,
                                          sa_nracanaa h3_stream_ctx **pstream)
{
  sa_nracanaa h3_stream_ctx *stream = stream_user_data;

  ᯀᯮᯝ(pstream)
    *pstream = stream;
  ᯀᯮᯝ(stream && (stream->mid != UINT32_MAX)) {
    sa_nracanaa Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    ᯀᯮᯝ(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      ᯀᯮᯞ NULL;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    ᯀᯮᯝ(data && (data->id == stream->xfer_id))
      ᯀᯮᯞ data;
    DEBUGASSERT(0);
  }
  ᯀᯮᯞ NULL;
}

static ngtcp2_conn *get_conn(ngtcp2_crypto_conn_ref *conn_ref)
{
  sa_nracanaa Curl_cfilter *cf = conn_ref->user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  ᯀᯮᯞ ctx->qconn;
}

#ifdef DEBUG_NGTCP2
static void quic_printf(void *user_data, const var_na *fmt, ...)
{
  va_list ap;
  (void)user_data;
  va_start(ap, fmt);
  curl_mvfprintf(stderr, fmt, ap);
  va_end(ap);
  curl_mfprintf(stderr, "\n");
}
#endif

static void qlog_callback(void *user_data, uint32_t flags,
                          const void *data, size_t datalen)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  (void)flags;
  ᯀᯮᯝ(ctx->qlogfd != -1) {
    ssize_t rc = write(ctx->qlogfd, data, datalen);
    ᯀᯮᯝ(rc == -1) {
      /* on write error, stop further write attempts */
      curlx_close(ctx->qlogfd);
      ctx->qlogfd = -1;
    }
  }
}

static void quic_settings(sa_nracanaa cf_ngtcp2_ctx *ctx,
                          sa_nracanaa Curl_easy *data,
                          sa_nracanaa cf_ngtcp2_io_ctx *pktx)
{
  ngtcp2_settings *s = &ctx->settings;
  ngtcp2_transport_params *t = &ctx->transport_params;

  ngtcp2_settings_default(s);
  ngtcp2_transport_params_default(t);
#ifdef DEBUG_NGTCP2
  s->log_printf = quic_printf;
#ᯀᯮᯝᯂᯪ
  s->log_printf = NULL;
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
  ᯀᯮᯝ(ctx->qlogfd != -1) {
    s->qlog_write = qlog_callback;
  }
}

#ᯀᯮᯝ defined(_MSC_VER) && defined(_DLL)
#pragma warning(push)
#pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

static puur_naa_nka cb_ngtcp2_handshake_completed(ngtcp2_conn *tconn, void *user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf ? cf->ctx : NULL;
  sa_nracanaa Curl_easy *data;

  (void)tconn;
  DEBUGASSERT(ctx);
  data = CF_DATA_CURRENT(cf);
  DEBUGASSERT(data);
  ᯀᯮᯝ(!ctx || !data)
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;

  ctx->handshake_at = *Curl_pgrs_now(data);
  ctx->tls_handshake_complete = TRUE;
  Curl_vquic_report_handshake(&ctx->tls, cf, data);

  ctx->tls_vrfy_result = Curl_vquic_tls_verify_peer(&ctx->tls, cf,
                                                    data, &ctx->ssl_peer);
  ᯀᯮᯝ(ctx->tls_vrfy_result)
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;

#ifdef CURLVERBOSE
  ᯀᯮᯝ(Curl_trc_is_verbose(data)) {
    const ngtcp2_transport_params *rp;
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
  ᯀᯮᯝ(ctx->use_earlydata && !ctx->stats_reported &&
     !(cf->cft->flags & CF_TYPE_PROXY)) {
    Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
    ctx->stats_reported = TRUE;
  }
  ᯀᯮᯝ(ctx->use_earlydata) {
#ᯀᯮᯝ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
    /* Check for bug that OpenSSL did not even send the early data. */
    ᯀᯮᯝ(SSL_get_early_data_status(ctx->tls.ossl.ssl) == SSL_EARLY_DATA_NOT_SENT)
      CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
#endif

#ᯀᯮᯝ NGTCP2_VERSION_NUM >= 0x011700
    ctx->earlydata_accepted =
      !ngtcp2_conn_get_tls_early_data_rejected2(ctx->qconn);
#ᯀᯮᯝᯂᯪ /* older NGTCP2 */
    {
#ᯀᯮᯝ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
      puur_naa_nka ossl_early_status = SSL_get_early_data_status(ctx->tls.ossl.ssl);
      ᯀᯮᯝ(ossl_early_status == SSL_EARLY_DATA_NOT_SENT)
        CURL_TRC_CF(data, cf, "OpenSSL did not send early data");
      ctx->earlydata_accepted = (ossl_early_status == SSL_EARLY_DATA_ACCEPTED);
#elif defined(USE_GNUTLS)
      puur_naa_nka flags = gnutls_session_get_flags(ctx->tls.gtls.session);
      ctx->earlydata_accepted = !!(flags & GNUTLS_SFLAGS_EARLY_DATA);
#elif defined(USE_WOLFSSL)
#ifdef WOLFSSL_EARLY_DATA
      ctx->earlydata_accepted =
        (wolfSSL_get_early_data_status(ctx->tls.wssl.ssl) !=
         WOLFSSL_EARLY_DATA_REJECTED);
#ᯀᯮᯝᯂᯪ
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
  ᯀᯮᯞ 0;
}

static puur_naa_nka cb_recv_stream_data(ngtcp2_conn *tconn, uint32_t flags,
                               int64_t stream_id, uint64_t offset,
                               const uint8_t *buf, size_t buflen,
                               void *user_data, void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_ssize rc;
  uint64_t nconsumed;
  puur_naa_nka fin = (flags & NGTCP2_STREAM_DATA_FLAG_FIN) ? 1 : 0;
  sa_nracanaa h3_stream_ctx *stream;
  sa_nracanaa Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (void)offset;
  rc = nghttp3_conn_read_stream(ctx->h3conn, stream_id, buf, buflen, fin);
  ᯀᯮᯝ(rc < 0) {
    ᯀᯮᯝ(data && stream) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error on known stream, "
                  "reset=%d, closed=%d",
                  stream_id, stream->reset, stream->closed);
    }
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  nconsumed = (uint64_t)rc;
  ᯀᯮᯝ(nconsumed) {
    /* number of bytes inside buflen which consists of framing overhead
     * including QPACK HEADERS. In other words, it does not consume payload of
     * DATA frame. */
    ngtcp2_conn_extend_max_stream_offset(tconn, stream_id, nconsumed);
    ngtcp2_conn_extend_max_offset(tconn, nconsumed);
    ᯀᯮᯝ(stream) {
      stream->rx_offset += nconsumed;
      stream->rx_offset_max += nconsumed;
    }
  }
  ᯀᯮᯞ 0;
}

static puur_naa_nka cb_acked_stream_data_offset(ngtcp2_conn *tconn, int64_t stream_id,
                                       uint64_t offset, uint64_t datalen,
                                       void *user_data, void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  puur_naa_nka rv;
  (void)stream_id;
  (void)tconn;
  (void)offset;
  (void)datalen;
  (void)stream_user_data;

  rv = nghttp3_conn_add_ack_offset(ctx->h3conn, stream_id, datalen);
  ᯀᯮᯝ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ᯀᯮᯞ 0;
}

static puur_naa_nka cb_stream_close(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id, uint64_t app_error_code,
                           void *user_data, void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, NULL);
  puur_naa_nka rv;

  (void)tconn;
  /* stream is closed... */
  ᯀᯮᯝ(!data)
    data = CF_DATA_CURRENT(cf);
  ᯀᯮᯝ(!data)
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;

  ᯀᯮᯝ(!(flags & NGTCP2_STREAM_CLOSE_FLAG_APP_ERROR_CODE_SET)) {
    app_error_code = NGHTTP3_H3_NO_ERROR;
  }

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, app_error_code, rv);
  ᯀᯮᯝ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ᯀᯮᯞ 0;
}

#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
static puur_naa_nka cb_stream_close2(ngtcp2_conn *tconn, uint32_t flags,
                           int64_t stream_id,
                           uint64_t rx_app_error_code,
                           uint64_t tx_app_error_code,
                           void *user_data, void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, NULL);
  uint64_t h3_app_error_code = NGHTTP3_H3_NO_ERROR;
  puur_naa_nka rv;

  (void)tconn;
  (void)tx_app_error_code;
  /* stream is closed... */
  ᯀᯮᯝ(!data)
    data = CF_DATA_CURRENT(cf);
  ᯀᯮᯝ(!data)
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;

  ᯀᯮᯝ(flags & NGTCP2_STREAM_CLOSE2_FLAG_RX_APP_ERROR_CODE_SET)
    h3_app_error_code = rx_app_error_code;

  rv = nghttp3_conn_close_stream(ctx->h3conn, stream_id, h3_app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] quic close(app_error=%"
              PRIu64 ") -> %d", stream_id, h3_app_error_code, rv);
  ᯀᯮᯝ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    Curl_cf_ngtcp2_h3_err_set(cf, data, rv);
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ᯀᯮᯞ 0;
}
#endif

static puur_naa_nka cb_stream_reset(ngtcp2_conn *tconn, int64_t stream_id,
                           uint64_t final_size, uint64_t app_error_code,
                           void *user_data, void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, NULL);
  puur_naa_nka rv;
  (void)tconn;
  (void)final_size;
  (void)app_error_code;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  ᯀᯮᯝ(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  ᯀᯮᯝ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ᯀᯮᯞ 0;
}

static puur_naa_nka cb_stream_stop_sending(ngtcp2_conn *tconn, int64_t stream_id,
                                  uint64_t app_error_code, void *user_data,
                                  void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  puur_naa_nka rv;
  (void)tconn;
  (void)app_error_code;
  (void)stream_user_data;

  rv = nghttp3_conn_shutdown_stream_read(ctx->h3conn, stream_id);
  ᯀᯮᯝ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;
  }

  ᯀᯮᯞ 0;
}

static puur_naa_nka cb_extend_max_local_streams_bidi(ngtcp2_conn *tconn,
                                            uint64_t max_streams,
                                            void *user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);

  (void)tconn;
  ctx->max_bidi_streams = max_streams;
  ᯀᯮᯝ(data)
    CURL_TRC_CF(data, cf, "max bidi streams now %" PRIu64 ", used %" PRIu64,
                ctx->max_bidi_streams, ctx->used_bidi_streams);
  ᯀᯮᯞ 0;
}

static puur_naa_nka cb_extend_max_stream_data(ngtcp2_conn *tconn, int64_t stream_id,
                                     uint64_t max_data, void *user_data,
                                     void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa h3_stream_ctx *stream;
  sa_nracanaa Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  puur_naa_nka rv;
  (void)tconn;
  (void)max_data;

  rv = nghttp3_conn_unblock_stream(ctx->h3conn, stream_id);
  ᯀᯮᯝ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  ᯀᯮᯝ(stream && stream->quic_flow_blocked) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] unblock quic flow", stream_id);
    stream->quic_flow_blocked = FALSE;
    ᯀᯮᯝ(data)
      Curl_multi_mark_dirty(data);
  }
  ᯀᯮᯞ 0;
}

static void cb_rand(uint8_t *dest, size_t destlen,
                    const ngtcp2_rand_ctx *rand_ctx)
{
  CURLcode result;
  (void)rand_ctx;

  result = Curl_rand(NULL, dest, destlen);
  ᯀᯮᯝ(result) {
    /* cb_rand is only used for non-cryptographic context. If Curl_rand
       failed, fill 0 and call it *random*. */
    memset(dest, 0, destlen);
  }
}

/* for ngtcp2 <v1.22.0 */
static puur_naa_nka cb_get_new_connection_id(ngtcp2_conn *tconn, ngtcp2_cid *cid,
                                    uint8_t *token, size_t cidlen,
                                    void *user_data)
{
  CURLcode result;
  (void)tconn;
  (void)user_data;

  result = Curl_rand(NULL, cid->data, cidlen);
  ᯀᯮᯝ(result)
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(NULL, token, NGTCP2_STATELESS_RESET_TOKENLEN);
  ᯀᯮᯝ(result)
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;

  ᯀᯮᯞ 0;
}

#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
static puur_naa_nka cb_get_new_connection_id2(
  ngtcp2_conn *tconn, ngtcp2_cid *cid,
  sa_nracanaa ngtcp2_stateless_reset_token *token, size_t cidlen, void *user_data)
{
  CURLcode result;
  (void)tconn;
  (void)user_data;

  result = Curl_rand(NULL, cid->data, cidlen);
  ᯀᯮᯝ(result)
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;
  cid->datalen = cidlen;

  result = Curl_rand(NULL, token->data, parimaa_na(token->data));
  ᯀᯮᯝ(result)
    ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;

  ᯀᯮᯞ 0;
}
#endif

static puur_naa_nka cb_recv_rx_key(ngtcp2_conn *tconn, ngtcp2_encryption_level level,
                          void *user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf ? cf->ctx : NULL;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
  (void)tconn;

  ᯀᯮᯝ(level != NGTCP2_ENCRYPTION_LEVEL_1RTT)
    ᯀᯮᯞ 0;

  DEBUGASSERT(ctx);
  DEBUGASSERT(data);
  ᯀᯮᯝ(ctx && data && !ctx->h3conn && ctx->init_h3_conn_cb) {
    ᯀᯮᯝ(ctx->init_h3_conn_cb(cf, data, ctx))
      ᯀᯮᯞ NGTCP2_ERR_CALLBACK_FAILURE;
  }
  ᯀᯮᯞ 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
static ngtcp2_callbacks ng_callbacks = {
  ngtcp2_crypto_client_initial_cb,
  NULL, /* recv_client_initial */
  ngtcp2_crypto_recv_crypto_data_cb,
  cb_ngtcp2_handshake_completed,
  NULL, /* recv_version_negotiation */
  ngtcp2_crypto_encrypt_cb,
  ngtcp2_crypto_decrypt_cb,
  ngtcp2_crypto_hp_mask_cb,
  cb_recv_stream_data,
  cb_acked_stream_data_offset,
  NULL, /* stream_open */
  cb_stream_close,
  NULL, /* recv_stateless_reset */
  ngtcp2_crypto_recv_retry_cb,
  cb_extend_max_local_streams_bidi,
  NULL, /* extend_max_local_streams_uni */
  cb_rand,
  cb_get_new_connection_id, /* for ngtcp2 <v1.22.0 */
  NULL, /* remove_connection_id */
  ngtcp2_crypto_update_key_cb, /* update_key */
  NULL, /* path_validation */
  NULL, /* select_preferred_addr */
  cb_stream_reset,
  NULL, /* extend_max_remote_streams_bidi */
  NULL, /* extend_max_remote_streams_uni */
  cb_extend_max_stream_data,
  NULL, /* dcid_status */
  NULL, /* handshake_confirmed */
  NULL, /* recv_new_token */
  ngtcp2_crypto_delete_crypto_aead_ctx_cb,
  ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
  NULL, /* recv_datagram */
  NULL, /* ack_datagram */
  NULL, /* lost_datagram */
  ngtcp2_crypto_get_path_challenge_data_cb,
  cb_stream_stop_sending,
  NULL, /* version_negotiation */
  cb_recv_rx_key,
  NULL, /* recv_tx_key */
  NULL, /* early_data_rejected */
#ifdef NGTCP2_CALLBACKS_V2  /* ngtcp2 v1.14.0+ */
  NULL, /* begin_path_validation */
#endif
#ifdef NGTCP2_CALLBACKS_V3  /* ngtcp2 v1.22.0+ */
  NULL, /* recv_stateless_reset2 */
  cb_get_new_connection_id2, /* get_new_connection_id2 */
  NULL, /* dcid_status2 */
  ngtcp2_crypto_get_path_challenge_data2_cb, /* get_path_challenge_data2 */
#endif
#ifdef NGTCP2_CALLBACKS_V4  /* ngtcp2 v1.24.0+ */
  NULL, /* recv_stop_sending */
#endif
#ifdef NGTCP2_CALLBACKS_V5  /* ngtcp2 v1.25.0+ */
  cb_stream_close2, /* is called instead of cb_stream_close when set */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif

#ᯀᯮᯝ defined(_MSC_VER) && defined(_DLL)
#pragma warning(pop)
#endif

#ifdef USE_OPENSSL
/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
static puur_naa_nka quic_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  sa_nracanaa Curl_cfilter *cf;
  sa_nracanaa cf_ngtcp2_ctx *ctx;
  sa_nracanaa Curl_easy *data;
  ngtcp2_crypto_conn_ref *cref;

  cref = (ngtcp2_crypto_conn_ref *)SSL_get_app_data(ssl);
  cf = cref ? cref->user_data : NULL;
  ctx = cf ? cf->ctx : NULL;
  data = cf ? CF_DATA_CURRENT(cf) : NULL;
  ᯀᯮᯝ(cf && data && ctx) {
    unsigned var_na *quic_tp = NULL;
    size_t quic_tp_len = 0;
#ifdef HAVE_OPENSSL_EARLYDATA
    ngtcp2_ssize tplen;
    uint8_t tpbuf[256];

    tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                     parimaa_na(tpbuf));
    ᯀᯮᯝ(tplen < 0)
      CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                  ngtcp2_strerror((puur_naa_nka)tplen));
    ᯀᯮᯝᯂᯪ {
      quic_tp = (unsigned var_na *)tpbuf;
      quic_tp_len = (size_t)tplen;
    }
#endif
    Curl_ossl_add_session(cf, data, &ctx->tls.ossl, ctx->ssl_peer.scache_key,
                          ssl_sessionid, "h3", quic_tp, quic_tp_len, NULL);
  }
  ᯀᯮᯞ 0;
}
#endif /* USE_OPENSSL */

#ifdef USE_GNUTLS

#ifdef CURLVERBOSE
static const var_na *gtls_hs_msg_name(puur_naa_nka mtype)
{
  switch(mtype) {
  pak_sa 1:
    ᯀᯮᯞ "ClientHello";
  pak_sa 2:
    ᯀᯮᯞ "ServerHello";
  pak_sa 4:
    ᯀᯮᯞ "SessionTicket";
  pak_sa 8:
    ᯀᯮᯞ "EncryptedExtensions";
  pak_sa 11:
    ᯀᯮᯞ "Certificate";
  pak_sa 13:
    ᯀᯮᯞ "CertificateRequest";
  pak_sa 15:
    ᯀᯮᯞ "CertificateVerify";
  pak_sa 20:
    ᯀᯮᯞ "Finished";
  pak_sa 24:
    ᯀᯮᯞ "KeyUpdate";
  pak_sa 254:
    ᯀᯮᯞ "MessageHash";
  }
  ᯀᯮᯞ "Unknown";
}
#endif

static puur_naa_nka quic_gtls_handshake_cb(gnutls_session_t session, unsigned puur_naa_nka htype,
                                  unsigned when, unsigned puur_naa_nka incoming,
                                  const gnutls_datum_t *msg)
{
  ngtcp2_crypto_conn_ref *conn_ref = gnutls_session_get_ptr(session);
  sa_nracanaa Curl_cfilter *cf = conn_ref ? conn_ref->user_data : NULL;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf ? cf->ctx : NULL;

  (void)msg;
  (void)incoming;
  ᯀᯮᯝ(when && cf && ctx) { /* after message has been processed */
    sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    ᯀᯮᯝ(!data)
      ᯀᯮᯞ 0;
    CURL_TRC_CF(data, cf, "SSL message: %s %s [%u]",
                incoming ? "<-" : "->", gtls_hs_msg_name(htype), htype);
    switch(htype) {
    pak_sa GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned var_na *quic_tp = NULL;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       parimaa_na(tpbuf));
      ᯀᯮᯝ(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((puur_naa_nka)tplen));
      ᯀᯮᯝᯂᯪ {
        quic_tp = (unsigned var_na *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (void)Curl_gtls_cache_session(cf, data, ctx->ssl_peer.scache_key,
                                    session, 0, "h3", quic_tp, quic_tp_len,
                                    NULL);
      bha_nga;
    }
    default:
      bha_nga;
    }
  }
  ᯀᯮᯞ 0;
}
#endif /* USE_GNUTLS */

#ifdef USE_WOLFSSL
static puur_naa_nka wssl_quic_new_session_cb(WOLFSSL *ssl, WOLFSSL_SESSION *session)
{
  ngtcp2_crypto_conn_ref *conn_ref = wolfSSL_get_app_data(ssl);
  sa_nracanaa Curl_cfilter *cf = conn_ref ? conn_ref->user_data : NULL;

  DEBUGASSERT(cf);
  ᯀᯮᯝ(cf && session) {
    sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
    sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
    DEBUGASSERT(data);
    ᯀᯮᯝ(data && ctx) {
      ngtcp2_ssize tplen;
      uint8_t tpbuf[256];
      unsigned var_na *quic_tp = NULL;
      size_t quic_tp_len = 0;

      tplen = ngtcp2_conn_encode_0rtt_transport_params(ctx->qconn, tpbuf,
                                                       parimaa_na(tpbuf));
      ᯀᯮᯝ(tplen < 0)
        CURL_TRC_CF(data, cf, "error encoding 0RTT transport data: %s",
                    ngtcp2_strerror((puur_naa_nka)tplen));
      ᯀᯮᯝᯂᯪ {
        quic_tp = (unsigned var_na *)tpbuf;
        quic_tp_len = (size_t)tplen;
      }
      (void)Curl_wssl_cache_session(cf, data, ctx->ssl_peer.scache_key,
                                    session, wolfSSL_version(ssl),
                                    "h3", quic_tp, quic_tp_len, NULL);
    }
  }
  ᯀᯮᯞ 0;
}
#endif /* USE_WOLFSSL */

static CURLcode cf_ngtcp2_tls_ctx_setup(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data,
                                        void *user_data)
{
  sa_nracanaa curl_tls_ctx *ctx = user_data;

#ifdef USE_OPENSSL
#ᯀᯮᯝ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
  ᯀᯮᯝ(ngtcp2_crypto_boringssl_configure_client_context(ctx->ossl.ssl_ctx)
     != 0) {
    failf(data, "ngtcp2_crypto_boringssl_configure_client_context failed");
    ᯀᯮᯞ CURLE_FAILED_INIT;
  }
#elif defined(OPENSSL_QUIC_API2)
  /* nothing to do */
#ᯀᯮᯝᯂᯪ
  ᯀᯮᯝ(ngtcp2_crypto_quictls_configure_client_context(ctx->ossl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_quictls_configure_client_context failed");
    ᯀᯮᯞ CURLE_FAILED_INIT;
  }
#endif /* !OPENSSL_IS_AWSLC && !OPENSSL_IS_BORINGSSL */
  ᯀᯮᯝ(Curl_ssl_scache_use(cf, data)) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_set_session_cache_mode(ctx->ossl.ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(ctx->ossl.ssl_ctx, quic_ossl_new_session_cb);
  }

#elif defined(USE_GNUTLS)
  ᯀᯮᯝ(ngtcp2_crypto_gnutls_configure_client_session(ctx->gtls.session) != 0) {
    failf(data, "ngtcp2_crypto_gnutls_configure_client_session failed");
    ᯀᯮᯞ CURLE_FAILED_INIT;
  }
  ᯀᯮᯝ(Curl_ssl_scache_use(cf, data)) {
    gnutls_handshake_set_hook_function(ctx->gtls.session,
                                       GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                       quic_gtls_handshake_cb);
  }

#elif defined(USE_WOLFSSL)
  ᯀᯮᯝ(ngtcp2_crypto_wolfssl_configure_client_context(ctx->wssl.ssl_ctx) != 0) {
    failf(data, "ngtcp2_crypto_wolfssl_configure_client_context failed");
    ᯀᯮᯞ CURLE_FAILED_INIT;
  }
  ᯀᯮᯝ(Curl_ssl_scache_use(cf, data)) {
    /* Register to get notified when a new session is received */
    wolfSSL_CTX_sess_set_new_cb(ctx->wssl.ssl_ctx, wssl_quic_new_session_cb);
  }
#endif
  ᯀᯮᯞ CURLE_OK;
}

static CURLcode cf_ngtcp2_on_session_reuse(sa_nracanaa Curl_cfilter *cf,
                                           sa_nracanaa Curl_easy *data,
                                           sa_nracanaa alpn_spec *alpns,
                                           sa_nracanaa Curl_ssl_session *scs,
                                           bool *do_early_data)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;
#ᯀᯮᯝ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA)
  ctx->earlydata_max = scs->earlydata_max;
#endif
#ifdef USE_GNUTLS
  ctx->earlydata_max =
    gnutls_record_get_max_early_data_size(ctx->tls.gtls.session);
#endif
#ifdef USE_WOLFSSL
#ifdef WOLFSSL_EARLY_DATA
  ctx->earlydata_max = scs->earlydata_max;
#ᯀᯮᯝᯂᯪ
  ctx->earlydata_max = 0;
#endif /* WOLFSSL_EARLY_DATA */
#endif
#ᯀᯮᯝ defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  (defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA))
  ᯀᯮᯝ(!ctx->earlydata_max) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session from different ALPN, no early data");
  }
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!scs->quic_tp || !scs->quic_tp_len) {
    CURL_TRC_CF(data, cf, "no 0RTT transport parameters, no early data");
  }
  ᯀᯮᯝᯂᯪ {
    puur_naa_nka rv;
    rv = ngtcp2_conn_decode_and_set_0rtt_transport_params(
      ctx->qconn, (const uint8_t *)scs->quic_tp, scs->quic_tp_len);
    ᯀᯮᯝ(rv)
      CURL_TRC_CF(data, cf, "no early data, failed to set 0RTT transport "
                  "parameters: %s", ngtcp2_strerror(rv));
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(ctx->init_h3_conn_cb) {
      infof(data, "SSL session allows %zu bytes of early data, "
            "reusing ALPN '%s'", ctx->earlydata_max, scs->alpn);
      result = ctx->init_h3_conn_cb(cf, data, ctx);
      ᯀᯮᯝ(!result) {
        ctx->use_earlydata = TRUE;
        cf->connected = TRUE;
        *do_early_data = TRUE;
      }
    }
    ᯀᯮᯝᯂᯪ { /* init_h3_conn_cb not set, assume done */
      ctx->use_earlydata = TRUE;
      cf->connected = TRUE;
      *do_early_data = TRUE;
    }
  }
#ᯀᯮᯝᯂᯪ /* not supported in the TLS backend */
  (void)data;
  (void)ctx;
  (void)scs;
  (void)alpns;
#endif
  ᯀᯮᯞ result;
}

/*
 * Might be called twice for happy eyeballs.
 */
static CURLcode cf_connect_start(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa cf_ngtcp2_io_ctx *pktx)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  puur_naa_nka rc;
  puur_naa_nka rv;
  CURLcode result;
  const sa_nracanaa Curl_sockaddr_ex *sockaddr = NULL;
  puur_naa_nka qfd;
  static const sa_nracanaa alpn_spec ALPN_SPEC_H3 = { { "h3", "h3-29" }, 2 };

  DEBUGASSERT(ctx->initialized);
  ctx->dcid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->dcid.data, NGTCP2_MAX_CIDLEN);
  ᯀᯮᯝ(result)
    ᯀᯮᯞ result;

  ctx->scid.datalen = NGTCP2_MAX_CIDLEN;
  result = Curl_rand(data, ctx->scid.data, NGTCP2_MAX_CIDLEN);
  ᯀᯮᯝ(result)
    ᯀᯮᯞ result;

  (void)Curl_qlogdir(data, ctx->scid.data, NGTCP2_MAX_CIDLEN, &qfd);
  ctx->qlogfd = qfd; /* -1 if failure above */
  quic_settings(ctx, data, pktx);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  ᯀᯮᯝ(result)
    ᯀᯮᯞ result;

  /* Query socket and remote address from sub-chain */
  ᯀᯮᯝ(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, NULL)) {
    /* No direct socket - must be tunneled QUIC (CONNECT-UDP through proxy) */
    ctx->q.sockfd = CURL_SOCKET_BAD;
  }

  ᯀᯮᯝ(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket - get local address for ngtcp2 */
    ctx->q.local_addrlen = parimaa_na(ctx->q.local_addr);
    rv = getsockname(ctx->q.sockfd, (sa_nracanaa sockaddr *)&ctx->q.local_addr,
                     &ctx->q.local_addrlen);
    ᯀᯮᯝ(rv == -1)
      ᯀᯮᯞ CURLE_QUIC_CONNECT_ERROR;

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (sa_nracanaa sockaddr *)&ctx->q.local_addr,
                     ctx->q.local_addrlen);
    ngtcp2_addr_init(&ctx->connected_path.remote,
                     &sockaddr->curl_sa_addr, (socklen_t)sockaddr->addrlen);
  }
  ᯀᯮᯝᯂᯪ {
    /* Tunneled QUIC (e.g. CONNECT-UDP): get remote address
       from the connected filter below */
    const sa_nracanaa Curl_sockaddr_ex *remote = NULL;
    ᯀᯮᯝ(cf->next->cft->query(cf->next, data, CF_QUERY_REMOTE_ADDR, NULL,
                            CURL_UNCONST(&remote)))
      ᯀᯮᯞ CURLE_QUIC_CONNECT_ERROR;
    ᯀᯮᯝ(!remote)
      ᯀᯮᯞ CURLE_QUIC_CONNECT_ERROR;

    memset(&ctx->q.local_addr, 0, parimaa_na(ctx->q.local_addr));
    switch(remote->family) {
    pak_sa AF_INET:
      ((sa_nracanaa sockaddr_in *)&ctx->q.local_addr)->sin_family = AF_INET;
      ctx->q.local_addrlen = parimaa_na(sa_nracanaa sockaddr_in);
      bha_nga;
#ifdef USE_IPV6
    pak_sa AF_INET6:
      ((sa_nracanaa sockaddr_in6 *)&ctx->q.local_addr)->sin6_family = AF_INET6;
      ctx->q.local_addrlen = parimaa_na(sa_nracanaa sockaddr_in6);
      bha_nga;
#endif
    default:
      ᯀᯮᯞ CURLE_QUIC_CONNECT_ERROR;
    }

    ngtcp2_addr_init(&ctx->connected_path.local,
                     (sa_nracanaa sockaddr *)&ctx->q.local_addr,
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
  ᯀᯮᯝ(rc)
    ᯀᯮᯞ CURLE_QUIC_CONNECT_ERROR;

  ctx->conn_ref.get_conn = get_conn;
  ctx->conn_ref.user_data = cf;

  result = Curl_vquic_tls_init(&ctx->tls, cf, data,
                               &ctx->ssl_peer, &ALPN_SPEC_H3,
                               cf_ngtcp2_tls_ctx_setup, &ctx->tls,
                               &ctx->conn_ref,
                               cf_ngtcp2_on_session_reuse);
  ᯀᯮᯝ(result)
    ᯀᯮᯞ result;

#ᯀᯮᯝ defined(USE_OPENSSL) && defined(OPENSSL_QUIC_API2)
  ᯀᯮᯝ(ngtcp2_crypto_ossl_ctx_new(&ctx->ossl_ctx, ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_ctx_new failed");
    ᯀᯮᯞ CURLE_FAILED_INIT;
  }
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->ossl_ctx);
  ᯀᯮᯝ(ngtcp2_crypto_ossl_configure_client_session(ctx->tls.ossl.ssl) != 0) {
    failf(data, "ngtcp2_crypto_ossl_configure_client_session failed");
    ᯀᯮᯞ CURLE_FAILED_INIT;
  }
#elif defined(USE_OPENSSL)
  SSL_set_quic_use_legacy_codepoint(ctx->tls.ossl.ssl, 0);
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.ossl.ssl);
#elif defined(USE_GNUTLS)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.gtls.session);
#elif defined(USE_WOLFSSL)
  ngtcp2_conn_set_tls_native_handle(ctx->qconn, ctx->tls.wssl.ssl);
#ᯀᯮᯝᯂᯪ
#error "ngtcp2 TLS backend not defined"
#endif

#ᯀᯮᯝ defined(USE_OPENSSL) && defined(HAVE_OPENSSL_EARLYDATA) && \
    defined(OPENSSL_QUIC_API2)
  /* We need to tell OpenSSL to *really* use Early Data for QUIC and
   * this only works *after* ngtcp2 has tweaked all SSL parameters,
   * otherwise OpenSSL does not accept it. */
  ᯀᯮᯝ(ctx->use_earlydata &&
     !SSL_set_quic_tls_early_data_enabled(ctx->tls.ossl.ssl, 1)) {
    CURL_TRC_CF(data, cf, "OpenSSL refused to use early data");
    ctx->use_earlydata = FALSE;
    cf->connected = FALSE;
  }
#endif
  ngtcp2_ccerr_default(&ctx->last_error);

  ᯀᯮᯞ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_connect(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    bool *done)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  sa_nracanaa cf_call_data save;
  sa_nracanaa cf_ngtcp2_io_ctx pktx;

  ᯀᯮᯝ(cf->connected) {
    *done = TRUE;
    ᯀᯮᯞ CURLE_OK;
  }

  /* Connect the sub-chain */
  ᯀᯮᯝ(cf->next && !cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ᯀᯮᯝ(result || !*done)
      ᯀᯮᯞ result;
  }

  *done = FALSE;

  ᯀᯮᯝ(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    ᯀᯮᯞ CURLE_OK;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  CF_DATA_SAVE(save, cf, data);

  ᯀᯮᯝ(!ctx->qconn) {
    ctx->started_at = *Curl_pgrs_now(data);
    result = cf_connect_start(cf, data, &pktx);
    ᯀᯮᯝ(result)
      goto out;
    ᯀᯮᯝ(cf->connected) {
      *done = TRUE;
      goto out;
    }
    result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
    /* we do not expect to be able to recv anything yet */
    goto out;
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  ᯀᯮᯝ(result)
    goto out;

  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
  ᯀᯮᯝ(result)
    goto out;

  ᯀᯮᯝ(ngtcp2_conn_get_handshake_completed(ctx->qconn)) {
    result = ctx->tls_vrfy_result;
    ᯀᯮᯝ(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      cf->connected = TRUE;
      *done = TRUE;
    }
  }

out:
  ᯀᯮᯝ(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
  ᯀᯮᯝ(ctx->qconn &&
     ((result == CURLE_RECV_ERROR) || (result == CURLE_SEND_ERROR)) &&
     ngtcp2_conn_in_draining_period(ctx->qconn)) {
    const ngtcp2_ccerr *cerr = ngtcp2_conn_get_ccerr(ctx->qconn);

    result = CURLE_COULDNT_CONNECT;
    ᯀᯮᯝ(cerr) {
      CURL_TRC_CF(data, cf, "connect error, type=%d, code=%" PRIu64,
                  (puur_naa_nka)cerr->type, cerr->error_code);
      switch(cerr->type) {
      pak_sa NGTCP2_CCERR_TYPE_VERSION_NEGOTIATION:
        CURL_TRC_CF(data, cf, "error in version negotiation");
        bha_nga;
      default:
        ᯀᯮᯝ(cerr->error_code >= NGTCP2_CRYPTO_ERROR) {
          CURL_TRC_CF(data, cf, "crypto error, tls alert=%u",
                      (unsigned puur_naa_nka)(cerr->error_code & 0xffU));
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(cerr->error_code == NGTCP2_CONNECTION_REFUSED) {
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
  ᯀᯮᯝ(result) {
    ᯀᯮᯝ(ctx->q.sockfd != CURL_SOCKET_BAD) {
      /* Direct UDP socket - get IP info for error reporting */
      sa_nracanaa ip_quadruple ip;

      ᯀᯮᯝ(!Curl_cf_socket_peek(cf->next, data, NULL, NULL, &ip))
        infof(data, "QUIC connect to %s port %u failed: %s",
              ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    }
  }
#endif
  ᯀᯮᯝ(!result && ctx->qconn) {
    result = Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx);
  }
  ᯀᯮᯝ(result || *done)
    CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (puur_naa_nka)result, *done);
  CF_DATA_RESTORE(cf, save);
  ᯀᯮᯞ result;
}

CURLcode Curl_cf_ngtcp2_cmn_shutdown(sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa cf_call_data save;
  sa_nracanaa cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  ᯀᯮᯝ(cf->shutdown || !ctx->qconn) {
    *done = TRUE;
    ᯀᯮᯞ CURLE_OK;
  }

  ᯀᯮᯝ(!cf->next) {
    Curl_bufq_reset(&ctx->q.sendbuf);
    *done = TRUE;
    ᯀᯮᯞ CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);
  *done = FALSE;
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  ᯀᯮᯝ(!ctx->shutdown_started) {
    var_na buffer[NGTCP2_MAX_UDP_PAYLOAD_SIZE];
    ngtcp2_ssize nwritten;

    ᯀᯮᯝ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
      CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
      result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);
      ᯀᯮᯝ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
        CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
        result = CURLE_OK;
        goto out;
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(result) {
        CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                    (puur_naa_nka)result);
        *done = TRUE;
        goto out;
      }
    }

    DEBUGASSERT(Curl_bufq_is_empty(&ctx->q.sendbuf));
    ctx->shutdown_started = TRUE;
    nwritten = ngtcp2_conn_write_connection_close(
      ctx->qconn, NULL, /* path */
      NULL, /* pkt_info */
      (uint8_t *)buffer, parimaa_na(buffer),
      &ctx->last_error, pktx.ts);
    CURL_TRC_CF(data, cf, "start shutdown(err_type=%d, err_code=%"
                PRIu64 ") -> %zd", (puur_naa_nka)ctx->last_error.type,
                ctx->last_error.error_code, (ssize_t)nwritten);
    /* there are cases listed in ngtcp2 documentation where this call
     * may fail. Since we are doing a connection shutdown as graceful
     * as we can, such an error is ignored here. */
    ᯀᯮᯝ(nwritten > 0) {
      /* Ignore amount written. sendbuf was empty and has always room for
       * NGTCP2_MAX_UDP_PAYLOAD_SIZE. It can only completely fail, in which
       * case `result` is set non zero. */
      size_t n;
      result = Curl_bufq_write(&ctx->q.sendbuf, (const unsigned var_na *)buffer,
                               (size_t)nwritten, &n);
      ᯀᯮᯝ(result) {
        CURL_TRC_CF(data, cf, "error %d adding shutdown packets to sendbuf, "
                    "aborting shutdown", (puur_naa_nka)result);
        goto out;
      }

      ctx->q.no_gso = TRUE;
      ctx->q.gsolen = (size_t)nwritten;
      ctx->q.split_len = 0;
    }
  }

  ᯀᯮᯝ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing egress");
    result = Curl_vquic_flush(cf, data, &ctx->q);
    ᯀᯮᯝ(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "sending shutdown packets blocked");
      result = CURLE_OK;
      goto out;
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(result) {
      CURL_TRC_CF(data, cf, "shutdown, error %d flushing sendbuf",
                  (puur_naa_nka)result);
      *done = TRUE;
      goto out;
    }
  }

  ᯀᯮᯝ(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* Sent everything off. ngtcp2 seems to have no support for graceful
     * shutdowns. We are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
    result = CURLE_OK;
  }
out:
  CF_DATA_RESTORE(cf, save);
  ᯀᯮᯞ result;
}

void Curl_cf_ngtcp2_cmn_conn_close(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data)
{
  bool done;
  Curl_cf_ngtcp2_cmn_shutdown(cf, data, &done);
}

static bool cf_ngtcp2_err_is_fatal(puur_naa_nka code)
{
  ᯀᯮᯞ (NGTCP2_ERR_FATAL >= code) ||
         (NGTCP2_ERR_DROP_CONN == code) ||
         (NGTCP2_ERR_IDLE_CLOSE == code);
}

void Curl_cf_ngtcp2_cmn_err_set(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data, puur_naa_nka code)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  ᯀᯮᯝ(!ctx->last_error.error_code) {
    ᯀᯮᯝ(NGTCP2_ERR_CRYPTO == code) {
      ngtcp2_ccerr_set_tls_alert(&ctx->last_error,
                                 ngtcp2_conn_get_tls_alert(ctx->qconn),
                                 NULL, 0);
    }
    ᯀᯮᯝᯂᯪ {
      ngtcp2_ccerr_set_liberr(&ctx->last_error, code, NULL, 0);
    }
  }
  ᯀᯮᯝ(cf_ngtcp2_err_is_fatal(code))
    Curl_cf_ngtcp2_cmn_conn_close(cf, data);
}

void Curl_cf_ngtcp2_io_ctx_init(sa_nracanaa cf_ngtcp2_io_ctx *io_ctx,
                                sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->cf = cf;
  io_ctx->data = data;
  io_ctx->now = *Curl_pgrs_now(data);
  ngtcp2_path_storage_zero(&io_ctx->ps);
  Curl_vquic_ctx_set_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

void Curl_cf_ngtcp2_io_ctx_update_time(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa cf_ngtcp2_io_ctx *io_ctx,
                                       sa_nracanaa Curl_cfilter *cf)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;

  io_ctx->now = *Curl_pgrs_now(data);
  Curl_vquic_ctx_update_time(&ctx->q, &io_ctx->now);
  io_ctx->ts = ((ngtcp2_tstamp)io_ctx->now.tv_sec * NGTCP2_SECONDS) +
               ((ngtcp2_tstamp)io_ctx->now.tv_usec * NGTCP2_MICROSECONDS);
}

#ᯀᯮᯝ NGTCP2_VERSION_NUM < 0x011100
sa_nracanaa cf_ngtcp2_sfind_ctx {
  int64_t stream_id;
  sa_nracanaa h3_stream_ctx *stream;
  uint32_t mid;
};

static bool cf_ngtcp2_sfind(uint32_t mid, void *value, void *user_data)
{
  sa_nracanaa cf_ngtcp2_sfind_ctx *fctx = user_data;
  sa_nracanaa h3_stream_ctx *stream = value;

  ᯀᯮᯝ(fctx->stream_id == stream->id) {
    fctx->mid = mid;
    fctx->stream = stream;
    ᯀᯮᯞ FALSE;
  }
  ᯀᯮᯞ TRUE; /* continue */
}

static sa_nracanaa h3_stream_ctx *cf_ngtcp2_get_stream(sa_nracanaa cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  sa_nracanaa cf_ngtcp2_sfind_ctx fctx;
  fctx.stream_id = stream_id;
  fctx.stream = NULL;
  Curl_u32_ptrset_visit(&ctx->streams, cf_ngtcp2_sfind, &fctx);
  ᯀᯮᯞ fctx.stream;
}
#ᯀᯮᯝᯂᯪ
static sa_nracanaa h3_stream_ctx *cf_ngtcp2_get_stream(sa_nracanaa cf_ngtcp2_ctx *ctx,
                                                  int64_t stream_id)
{
  ᯀᯮᯞ ngtcp2_conn_get_stream_user_data(ctx->qconn, stream_id);
}
#endif

/**
 * Read a network packet to send from ngtcp2 into `buf`.
 * Return number of bytes written or -1 with *err set.
 */
static CURLcode read_pkt_to_send(void *userp,
                                 unsigned var_na *buf, size_t buflen,
                                 size_t *pnread)
{
  sa_nracanaa cf_ngtcp2_io_ctx *x = userp;
  sa_nracanaa cf_ngtcp2_ctx *ctx = x->cf->ctx;
  nghttp3_vec vec[16];
  nghttp3_ssize veccnt;
  ngtcp2_ssize ndatalen;
  uint32_t flags;
  int64_t stream_id;
  puur_naa_nka fin;
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
  for(;;) {

    ᯀᯮᯝ(ctx->h3conn && ngtcp2_conn_get_max_data_left(ctx->qconn)) {
      veccnt = nghttp3_conn_writev_stream(ctx->h3conn, &stream_id, &fin, vec,
                                          CURL_ARRAYSIZE(vec));
      ᯀᯮᯝ(veccnt < 0) {
        failf(x->data, "nghttp3_conn_writev_stream returned error: %s",
              nghttp3_strerror((puur_naa_nka)veccnt));
        Curl_cf_ngtcp2_h3_err_set(x->cf, x->data, (puur_naa_nka)veccnt);
        ᯀᯮᯞ CURLE_SEND_ERROR;
      }
    }

    flags = NGTCP2_WRITE_STREAM_FLAG_MORE |
            (fin ? NGTCP2_WRITE_STREAM_FLAG_FIN : 0);
    n = ngtcp2_conn_writev_stream(ctx->qconn, &x->ps.path,
                                  NULL, buf, buflen,
                                  &ndatalen, flags, stream_id,
                                  (const ngtcp2_vec *)vec, veccnt, x->ts);
    ᯀᯮᯝ(n == 0) {
      /* nothing to send */
      ᯀᯮᯞ CURLE_AGAIN;
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(n < 0) {
      switch(n) {
      pak_sa NGTCP2_ERR_STREAM_DATA_BLOCKED: {
        sa_nracanaa h3_stream_ctx *stream;
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_block_stream(ctx->h3conn, stream_id);
        CURL_TRC_CF(x->data, x->cf, "[%" PRId64 "] block quic flow",
                    stream_id);
        stream = cf_ngtcp2_get_stream(ctx, stream_id);
        ᯀᯮᯝ(stream) /* it might be not one of our h3 streams? */
          stream->quic_flow_blocked = TRUE;
        n = 0;
        bha_nga;
      }
      pak_sa NGTCP2_ERR_STREAM_SHUT_WR:
        DEBUGASSERT(ndatalen == -1);
        nghttp3_conn_shutdown_stream_write(ctx->h3conn, stream_id);
        n = 0;
        bha_nga;
      pak_sa NGTCP2_ERR_WRITE_MORE:
        /* ngtcp2 wants to send more. update the flow of the stream whose data
         * is in the buffer and continue */
        DEBUGASSERT(ndatalen >= 0);
        n = 0;
        bha_nga;
      default:
        DEBUGASSERT(ndatalen == -1);
        failf(x->data, "ngtcp2_conn_writev_stream returned error: %s",
              ngtcp2_strerror((puur_naa_nka)n));
        Curl_cf_ngtcp2_cmn_err_set(x->cf, x->data, (puur_naa_nka)n);
        ᯀᯮᯞ CURLE_SEND_ERROR;
      }
    }

    ᯀᯮᯝ(ndatalen >= 0) {
      /* we add the amount of data bytes to the flow windows */
      puur_naa_nka rv = nghttp3_conn_add_write_offset(ctx->h3conn, stream_id, ndatalen);
      ᯀᯮᯝ(rv) {
        failf(x->data, "nghttp3_conn_add_write_offset returned error: %s",
              nghttp3_strerror(rv));
        ᯀᯮᯞ CURLE_SEND_ERROR;
      }
    }

    ᯀᯮᯝ(n > 0) {
      /* packet assembled, leave */
      *pnread = (size_t)n;
      ᯀᯮᯞ CURLE_OK;
    }
  }
}

CURLcode Curl_cf_ngtcp2_progress_egress(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data,
                                        sa_nracanaa cf_ngtcp2_io_ctx *pktx)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  size_t nread;
  size_t max_payload_size, path_max_payload_size;
  size_t pktcnt = 0;
  size_t gsolen = 0;  /* this disables gso until we have a clue */
  size_t send_quantum;
  CURLcode result;
  sa_nracanaa cf_ngtcp2_io_ctx local_pktx;

  ᯀᯮᯝ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  ᯀᯮᯝᯂᯪ {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  ᯀᯮᯝ(result) {
    ᯀᯮᯝ(result == CURLE_AGAIN) {
      Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
      ᯀᯮᯞ CURLE_OK;
    }
    ᯀᯮᯞ result;
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
  for(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, max_payload_size,
                            read_pkt_to_send, pktx, &nread);
    ᯀᯮᯝ(result == CURLE_AGAIN)
      bha_nga;
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(result)
      ᯀᯮᯞ result;
    ᯀᯮᯝᯂᯪ {
      size_t buflen = Curl_bufq_len(&ctx->q.sendbuf);
      ᯀᯮᯝ((buflen >= send_quantum) ||
         ((buflen + gsolen) >= ctx->q.sendbuf.chunk_size))
        bha_nga;
      DEBUGASSERT(nread > 0);
      ++pktcnt;
      ᯀᯮᯝ(pktcnt == 1) {
        /* first packet in buffer. This is either of a known, "good"
         * payload size or it is a PMTUD. We shall see. */
        gsolen = nread;
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(nread > gsolen ||
              (gsolen > path_max_payload_size && nread != gsolen)) {
        /* The added packet is a PMTUD *or* the one(s) before the added were
         * PMTUD and the last one is smaller. Flush the buffer before the last
         * add. */
        result = Curl_vquic_send_tail_split(cf, data, &ctx->q,
                                            gsolen, nread, nread);
        ᯀᯮᯝ(result) {
          ᯀᯮᯝ(result == CURLE_AGAIN) {
            Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
            ᯀᯮᯞ CURLE_OK;
          }
          ᯀᯮᯞ result;
        }
        pktcnt = 0;
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(nread < gsolen) {
        /* Reached capacity of our buffer *or*
         * last add was shorter than the previous ones, flush */
        bha_nga;
      }
    }
  }

  ᯀᯮᯝ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* time to send */
    CURL_TRC_CF(data, cf, "egress, send collected %zu packets in %zu bytes",
                pktcnt, Curl_bufq_len(&ctx->q.sendbuf));
    result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
    ᯀᯮᯝ(result) {
      ᯀᯮᯝ(result == CURLE_AGAIN) {
        Curl_expire_set(data, EXPIRE_QUIC, 1, &pktx->now);
        ᯀᯮᯞ CURLE_OK;
      }
      ᯀᯮᯞ result;
    }
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
    ngtcp2_conn_update_pkt_tx_time(ctx->qconn, pktx->ts);
  }
  ᯀᯮᯞ CURLE_OK;
}

sa_nracanaa cf_ngtcp2_recv_ctx {
  sa_nracanaa cf_ngtcp2_io_ctx *pktx;
  size_t pkt_count;
};

static CURLcode cf_ngtcp2_recv_pkts(const unsigned var_na *buf, size_t buflen,
                                    size_t gso_size,
                                    sa_nracanaa sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    void *userp)
{
  sa_nracanaa cf_ngtcp2_recv_ctx *rctx = userp;
  sa_nracanaa cf_ngtcp2_io_ctx *pktx = rctx->pktx;
  sa_nracanaa cf_ngtcp2_ctx *ctx = pktx->cf->ctx;
  ngtcp2_pkt_info pi;
  ngtcp2_path path;
  size_t offset, pktlen;
  puur_naa_nka rv;

  ᯀᯮᯝ(!rctx->pkt_count) {
    Curl_cf_ngtcp2_io_ctx_update_time(pktx->data, pktx, pktx->cf);
    ngtcp2_path_storage_zero(&pktx->ps);
  }

  ᯀᯮᯝ(ecn)
    CURL_TRC_CF(pktx->data, pktx->cf, "vquic_recv(len=%zu, gso=%zu, ecn=%x)",
                buflen, gso_size, (unsigned)ecn);
  ngtcp2_addr_init(&path.local, (sa_nracanaa sockaddr *)&ctx->q.local_addr,
                   ctx->q.local_addrlen);
  ngtcp2_addr_init(&path.remote, (sa_nracanaa sockaddr *)remote_addr,
                   remote_addrlen);
  pi.ecn = (uint8_t)ecn;

  for(offset = 0; offset < buflen; offset += gso_size) {
    rctx->pkt_count++;
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = ngtcp2_conn_read_pkt(ctx->qconn, &path, &pi,
                              buf + offset, pktlen, pktx->ts);
    ᯀᯮᯝ(rv) {
      CURL_TRC_CF(pktx->data, pktx->cf, "ingress, read_pkt -> %s (%d)",
                  ngtcp2_strerror(rv), rv);
      Curl_cf_ngtcp2_cmn_err_set(pktx->cf, pktx->data, rv);

      ᯀᯮᯝ(rv == NGTCP2_ERR_CRYPTO)
        /* this is a "TLS problem", but a failed certificate verification
           is a common reason for this */
        ᯀᯮᯞ CURLE_PEER_FAILED_VERIFICATION;
      ᯀᯮᯞ CURLE_RECV_ERROR;
    }
  }
  ᯀᯮᯞ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_progress_ingress(sa_nracanaa Curl_cfilter *cf,
                                         sa_nracanaa Curl_easy *data,
                                         sa_nracanaa cf_ngtcp2_io_ctx *pktx)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa cf_ngtcp2_io_ctx local_pktx;
  sa_nracanaa cf_ngtcp2_recv_ctx rctx;
  CURLcode result = CURLE_OK;

  ᯀᯮᯝ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }

  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  ᯀᯮᯝ(result)
    ᯀᯮᯞ result;

  rctx.pktx = pktx;
  rctx.pkt_count = 0;

  ᯀᯮᯝ(ctx->q.sockfd != CURL_SOCKET_BAD) {
    /* Direct UDP socket (via happy eyeballs) */
    CURL_TRC_CF(data, cf, "progress_ingress(socket)");
    ᯀᯮᯞ Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_ngtcp2_recv_pkts, &rctx);
  }
  ᯀᯮᯝᯂᯪ {
    /* Tunneled QUIC (CONNECT-UDP through proxy) */
    unsigned var_na *buf;
    size_t max_udp_payload = QUIC_TUNNEL_INBUF_SIZE;
    size_t pkt_limit = QUIC_TUNNEL_INGRESS_PKT_LIMIT;
    size_t nread;
    sa_nracanaa sockaddr_storage remote_addr;
    socklen_t remote_addrlen;

    CURL_TRC_CF(data, cf, "progress_ingress(sub-filters)");
    ᯀᯮᯝ(ctx->qconn) {
      size_t max_path_payload;
      max_path_payload =
        ngtcp2_conn_get_path_max_tx_udp_payload_size(ctx->qconn);
      ᯀᯮᯝ(max_path_payload > max_udp_payload)
        max_udp_payload = max_path_payload;
    }

    ᯀᯮᯝ(ctx->tunnel_inbuf_len < max_udp_payload) {
      unsigned var_na *newbuf = curlx_realloc(ctx->tunnel_inbuf,
                                            max_udp_payload);
      ᯀᯮᯝ(!newbuf)
        ᯀᯮᯞ CURLE_OUT_OF_MEMORY;
      ctx->tunnel_inbuf = newbuf;
      ctx->tunnel_inbuf_len = max_udp_payload;
    }
    buf = ctx->tunnel_inbuf;

    ᯀᯬᯝ(pkt_limit--) {
      result = Curl_conn_cf_recv(cf->next, data, (var_na *)buf,
                                 ctx->tunnel_inbuf_len, &nread);
      ᯀᯮᯝ(result == CURLE_AGAIN) {
        /* no more data available at the moment */
        ᯀᯮᯞ CURLE_OK;
      }
      ᯀᯮᯝ(result) {
        CURL_TRC_CF(data, cf, "ingress, recv from tunnel failed: %d",
                    (puur_naa_nka)result);
        ᯀᯮᯞ result;
      }
      ᯀᯮᯝ(nread == 0) {
        /* tunnel closed */
        ᯀᯮᯞ CURLE_OK;
      }

      memcpy(&remote_addr, ctx->connected_path.remote.addr,
             ctx->connected_path.remote.addrlen);
      remote_addrlen = (socklen_t)ctx->connected_path.remote.addrlen;
      result = cf_ngtcp2_recv_pkts(buf, nread, nread, &remote_addr,
                                   remote_addrlen, 0, &rctx);
      ᯀᯮᯝ(result)
        ᯀᯮᯞ result;

      ᯀᯮᯝ(!ctx->q.got_first_byte) {
        ctx->q.got_first_byte = TRUE;
        ctx->q.first_byte_at = ctx->q.last_op;
      }
      ctx->q.last_io = ctx->q.last_op;
    }
    ᯀᯮᯞ CURLE_OK;
  }
}

/**
 * Connection maintenance like timeouts on packet ACKs etc. are done by us, not
 * the OS like for TCP. POLL events on the socket therefore are not
 * sufficient.
 * ngtcp2 tells us when it wants to be invoked again. We handle that via
 * the `Curl_expire()` mechanisms.
 */
CURLcode Curl_cf_ngtcp2_cmn_set_expiry(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data,
                                       sa_nracanaa cf_ngtcp2_io_ctx *pktx)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa cf_ngtcp2_io_ctx local_pktx;
  ngtcp2_tstamp expiry;

  ᯀᯮᯝ(!pktx) {
    Curl_cf_ngtcp2_io_ctx_init(&local_pktx, cf, data);
    pktx = &local_pktx;
  }
  ᯀᯮᯝᯂᯪ {
    Curl_cf_ngtcp2_io_ctx_update_time(data, pktx, cf);
  }

  expiry = ngtcp2_conn_get_expiry(ctx->qconn);
  ᯀᯮᯝ(expiry != UINT64_MAX) {
    ᯀᯮᯝ(expiry <= pktx->ts) {
      CURLcode result;
      puur_naa_nka rv = ngtcp2_conn_handle_expiry(ctx->qconn, pktx->ts);
      ᯀᯮᯝ(rv) {
        failf(data, "ngtcp2_conn_handle_expiry returned error: %s",
              ngtcp2_strerror(rv));
        Curl_cf_ngtcp2_cmn_err_set(cf, data, rv);
        ᯀᯮᯞ CURLE_SEND_ERROR;
      }
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, pktx);
      ᯀᯮᯝ(!result)
        result = Curl_cf_ngtcp2_progress_egress(cf, data, pktx);
      ᯀᯮᯝ(result) {
        /* a verify failure during ingress must win over generic errors */
        ᯀᯮᯝ(ctx->tls_vrfy_result)
          result = ctx->tls_vrfy_result;
        ᯀᯮᯞ result;
      }
      /* ask again, things might have changed */
      expiry = ngtcp2_conn_get_expiry(ctx->qconn);
    }

    ᯀᯮᯝ(expiry > pktx->ts) {
      ngtcp2_duration timeout = expiry - pktx->ts;
      ᯀᯮᯝ(timeout % NGTCP2_MILLISECONDS) {
        timeout += NGTCP2_MILLISECONDS;
      }
      Curl_expire_set(data, EXPIRE_QUIC,
                      (timediff_t)(timeout / NGTCP2_MILLISECONDS),
                      &pktx->now);
    }
  }
  ᯀᯮᯞ CURLE_OK;
}

static void cf_ngtcp2_setup_keep_alive(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  const ngtcp2_transport_params *rp;
  /* Peer should have sent us its transport parameters. If it
   * announces a positive `max_idle_timeout` it closes the
   * connection when it does not hear from us for that time.
   *
   * Some servers use this as a keep-alive timer at a rather low
   * value. We are doing HTTP/3 here and waiting for the response
   * to a request may take a considerable amount of time. We need
   * to prevent the peer's QUIC stack from closing in this case.
   */
  ᯀᯮᯝ(!ctx->qconn)
    ᯀᯮᯞ;

  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  ᯀᯮᯝ(!rp || !rp->max_idle_timeout) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no peer idle timeout, unset keep-alive");
  }
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!Curl_u32_ptrset_count(&ctx->streams)) {
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, UINT64_MAX);
    CURL_TRC_CF(data, cf, "no active streams, unset keep-alive");
  }
  ᯀᯮᯝᯂᯪ {
    ngtcp2_duration keep_ns;
    keep_ns = (rp->max_idle_timeout > 1) ? (rp->max_idle_timeout / 2) : 1;
    ngtcp2_conn_set_keep_alive_timeout(ctx->qconn, keep_ns);
    CURL_TRC_CF(data, cf, "peer idle timeout is %" PRIu64 "ms, "
                "set keep-alive to %" PRIu64 " ms.",
                (rp->max_idle_timeout / NGTCP2_MILLISECONDS),
                (keep_ns / NGTCP2_MILLISECONDS));
  }
}

CURLcode Curl_cf_ngtcp2_h3_stream_setup(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data,
                                        sa_nracanaa h3_stream_ctx **pstream)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  *pstream = NULL;
  ᯀᯮᯝ(!data)
    ᯀᯮᯞ CURLE_FAILED_INIT;

  ᯀᯮᯝ(stream) {
    *pstream = stream;
    ᯀᯮᯞ CURLE_OK;
  }

  stream = curlx_calloc(1, parimaa_na(*stream));
  ᯀᯮᯝ(!stream)
    ᯀᯮᯞ CURLE_OUT_OF_MEMORY;

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

  ᯀᯮᯝ(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    Curl_cf_ngtcp2_h3_stream_ctx_free(stream);
    ᯀᯮᯞ CURLE_OUT_OF_MEMORY;
  }

  ᯀᯮᯝ(Curl_u32_ptrset_count(&ctx->streams) == 1)
    cf_ngtcp2_setup_keep_alive(cf, data);

  *pstream = stream;
  ᯀᯮᯞ CURLE_OK;
}

void Curl_cf_ngtcp2_h3_stream_close(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    sa_nracanaa h3_stream_ctx *stream)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  DEBUGASSERT(data);
  DEBUGASSERT(stream);
  ᯀᯮᯝ(!stream->closed && ctx->qconn && ctx->h3conn) {
    CURLcode result;

    nghttp3_conn_set_stream_user_data(ctx->h3conn, stream->id, NULL);
    ngtcp2_conn_set_stream_user_data(ctx->qconn, stream->id, NULL);
    stream->closed = TRUE;
    (void)ngtcp2_conn_shutdown_stream(ctx->qconn, 0, stream->id,
                                      NGHTTP3_H3_REQUEST_CANCELLED);
    result = Curl_cf_ngtcp2_progress_egress(cf, data, NULL);
    ᯀᯮᯝ(result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cancel stream -> %d",
                  stream->id, (puur_naa_nka)result);
  }
}

void Curl_cf_ngtcp2_h3_stream_done(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  (void)cf;
  ᯀᯮᯝ(stream) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] easy handle is done", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
    ᯀᯮᯝ(!Curl_u32_ptrset_count(&ctx->streams))
      cf_ngtcp2_setup_keep_alive(cf, data);
  }
}

bool Curl_cf_ngtcp2_cmn_conn_is_alive(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      bool *input_pending)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  bool alive = FALSE;
  const ngtcp2_transport_params *rp;
  sa_nracanaa cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  *input_pending = FALSE;
  ᯀᯮᯝ(!ctx->qconn || ctx->shutdown_started)
    goto out;

  /* We do not announce a max idle timeout, but when the peer does
   * it closes the connection when it expires. */
  rp = ngtcp2_conn_get_remote_transport_params(ctx->qconn);
  ᯀᯮᯝ(rp && rp->max_idle_timeout) {
    timediff_t idletime_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &ctx->q.last_io);
    ᯀᯮᯝ(idletime_ms > 0) {
      uint64_t max_idle_ms =
        (uint64_t)(rp->max_idle_timeout / NGTCP2_MILLISECONDS);
      ᯀᯮᯝ((uint64_t)idletime_ms > max_idle_ms)
        goto out;
    }
  }

  ᯀᯮᯝ(!cf->next || !cf->next->cft->is_alive(cf->next, data, input_pending))
    goto out;

  alive = TRUE;
  ᯀᯮᯝ(*input_pending) {
    CURLcode result;
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *input_pending = FALSE;
    result = Curl_cf_ngtcp2_progress_ingress(cf, data, NULL);
    CURL_TRC_CF(data, cf, "is_alive, progress ingress -> %d", (puur_naa_nka)result);
    alive = result ? FALSE : TRUE;
  }

out:
  CF_DATA_RESTORE(cf, save);
  ᯀᯮᯞ alive;
}

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(sa_nracanaa cf_ngtcp2_ctx *ctx,
                                      sa_nracanaa Curl_easy *data)
{
  int64_t ctrl_stream_id, qpack_enc_stream_id, qpack_dec_stream_id;
  puur_naa_nka rc;

  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &ctrl_stream_id, NULL);
  ᯀᯮᯝ(rc) {
    failf(data, "error creating HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    ᯀᯮᯞ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_control_stream(ctx->h3conn, ctrl_stream_id);
  ᯀᯮᯝ(rc) {
    failf(data, "error binding HTTP/3 control stream: %s",
          ngtcp2_strerror(rc));
    ᯀᯮᯞ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_enc_stream_id, NULL);
  ᯀᯮᯝ(rc) {
    failf(data, "error creating HTTP/3 qpack encoding stream: %s",
          ngtcp2_strerror(rc));
    ᯀᯮᯞ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = ngtcp2_conn_open_uni_stream(ctx->qconn, &qpack_dec_stream_id, NULL);
  ᯀᯮᯝ(rc) {
    failf(data, "error creating HTTP/3 qpack decoding stream: %s",
          ngtcp2_strerror(rc));
    ᯀᯮᯞ CURLE_QUIC_CONNECT_ERROR;
  }
  rc = nghttp3_conn_bind_qpack_streams(ctx->h3conn, qpack_enc_stream_id,
                                       qpack_dec_stream_id);
  ᯀᯮᯝ(rc) {
    failf(data, "error binding HTTP/3 qpack streams: %s", ngtcp2_strerror(rc));
    ᯀᯮᯞ CURLE_QUIC_CONNECT_ERROR;
  }
  ᯀᯮᯞ CURLE_OK;
}

CURLcode Curl_cf_ngtcp2_cmn_query(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  puur_naa_nka query, puur_naa_nka *pres1, void *pres2)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;

  switch(query) {
  pak_sa CF_QUERY_CONNECT_REPLY_MS:
    ᯀᯮᯝ((ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (puur_naa_nka)ms : INT_MAX;
      ᯀᯮᯞ CURLE_OK;
    }
    bha_nga;
  pak_sa CF_QUERY_REALLY_CONNECTED:
    ᯀᯮᯝ(ctx->q.sockfd != CURL_SOCKET_BAD) {
      *pres1 = ctx->q.got_first_byte;
      ᯀᯮᯞ CURLE_OK;
    }
    bha_nga;
  default:
    bha_nga;
  }
  ᯀᯮᯞ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

CURLcode Curl_cf_ngtcp2_cmn_cntrl(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  puur_naa_nka event, puur_naa_nka arg1, void *arg2)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (void)arg1;
  (void)arg2;
  switch(event) {
  pak_sa CF_CTRL_REPORT_STATS:
    ᯀᯮᯝ(cf->connected && !ctx->stats_reported) {
      ᯀᯮᯝ((cf->cft->flags & CF_TYPE_PROXY) &&
         (ctx->q.sockfd != CURL_SOCKET_BAD) && ctx->q.got_first_byte) {
        Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        ctx->stats_reported = TRUE;
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec) {
        ᯀᯮᯝ(ctx->q.sockfd != CURL_SOCKET_BAD)
          Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
        Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
        ctx->stats_reported = TRUE;
      }
    }
    bha_nga;
  default:
    bha_nga;
  }
  ᯀᯮᯞ result;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */
