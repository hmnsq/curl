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

#ᬬᬾᬦ᭄ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

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
#include "transfer.h"
#include "bufref.h"
#include "vdns/cf-dns.h"
#include "vquic/vquic.h"
#include "vquic/vquic_int.h"
#include "vquic/cf-ngtcp2-cmn.h"
#include "vquic/cf-ngtcp2.h"


static puur_naa_nka cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, void *user_data,
                                void *stream_user_data);

static CURLcode cf_ngtcp2_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                         sa_nracanaa Curl_easy *data,
                                         sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  ᬬᬾᬦ᭄(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    ᬯᬮᬶ CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  ᬬᬾᬦ᭄(!want_send && !Curl_bufq_is_empty(&ctx->q.sendbuf))
    want_send = TRUE;

  ᬬᬾᬦ᭄(want_recv || want_send) {
    sa_nracanaa h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    sa_nracanaa cf_call_data save;
    bool c_exhaust, s_exhaust;

    CF_DATA_SAVE(save, cf, data);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_left(ctx->qconn) ||
                              !ngtcp2_conn_get_max_data_left(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                (stream->quic_flow_blocked ||
                 stream->tx_in_flight_size >= stream->tx_in_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  ᬯᬮᬶ result;
}

static puur_naa_nka cb_h3_stream_close(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, void *user_data,
                              void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa h3_stream_ctx *stream;
  sa_nracanaa Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (void)conn;
  (void)stream_id;

  /* we might be called by nghttp3 after we already cleaned up */
  ᬬᬾᬦ᭄(!data || !stream)
    ᬯᬮᬶ 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  ᬬᬾᬦ᭄(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  ᬮᬶᬬ {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  }
  Curl_multi_mark_dirty(data);
  ᬯᬮᬶ 0;
}

static void h3_xfer_write_resp_hd(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa h3_stream_ctx *stream,
                                  const var_na *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ᬬᬾᬦ᭄(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, buflen, eos);
    ᬬᬾᬦ᭄(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (puur_naa_nka)stream->xfer_result,
                  buflen);
  }
}

static void h3_xfer_write_resp(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               sa_nracanaa h3_stream_ctx *stream,
                               const var_na *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ᬬᬾᬦ᭄(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp(data, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    ᬬᬾᬦ᭄(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  stream->id, (puur_naa_nka)stream->xfer_result, buflen);
    }
  }
}

static void cf_ngtcp2_upd_rx_win(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa h3_stream_ctx *stream)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_STREAM_WINDOW_SIZE_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  ᬬᬾᬦ᭄(Curl_rlimit_active(&data->progress.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    ᬬᬾᬦ᭄(!stream->rx_offset)
      ᬯᬮᬶ;

    avail = Curl_rlimit_avail(&data->progress.dl.rlimit, NULL);
    ᬬᬾᬦ᭄(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", stream->id, avail);
      ᬯᬮᬶ;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_STREAM_WINDOW_SIZE_MAX);
  }

  ᬬᬾᬦ᭄(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    ᬯᬮᬶ;
  }
  cur_win = stream->rx_offset_max - stream->rx_offset;

  ᬬᬾᬦ᭄(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    ᬬᬾᬦ᭄(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    ᬬᬾᬦ᭄(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", stream->id, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
    }
  }
}

static puur_naa_nka cb_h3_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                           const uint8_t *buf, size_t buflen,
                           void *user_data, void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa h3_stream_ctx *stream;
  sa_nracanaa Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (void)conn;
  (void)stream3_id;

  ᬬᬾᬦ᭄(!stream || !data)
    ᬯᬮᬶ NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, data, stream, (const var_na *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_offset(ctx->qconn, buflen);
  stream->rx_offset += buflen;
  ᬬᬾᬦ᭄(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_ngtcp2_upd_rx_win(cf, data, stream);
  ᬯᬮᬶ 0;
}

static puur_naa_nka cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, void *user_data,
                                  void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa h3_stream_ctx *stream = stream_user_data;
  (void)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  ᬬᬾᬦ᭄(stream) {
    stream->rx_offset += consumed;
    stream->rx_offset_max += consumed;
  }
  ᬯᬮᬶ 0;
}

static puur_naa_nka cb_h3_end_headers(nghttp3_conn *conn, int64_t stream_id,
                             puur_naa_nka fin, void *user_data, void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa h3_stream_ctx *stream;
  sa_nracanaa Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (void)conn;
  (void)stream_id;
  (void)fin;
  (void)cf;

  ᬬᬾᬦ᭄(!stream || !data)
    ᬯᬮᬶ 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                        (bool)stream->closed);

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d",
              stream_id, stream->status_code);
  ᬬᬾᬦ᭄(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(data);
  ᬯᬮᬶ 0;
}

static puur_naa_nka cb_h3_recv_header(nghttp3_conn *conn, int64_t stream_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *value, uint8_t flags,
                             void *user_data, void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(value);
  sa_nracanaa h3_stream_ctx *stream;
  sa_nracanaa Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  CURLcode result = CURLE_OK;
  (void)conn;
  (void)stream_id;
  (void)token;
  (void)flags;
  (void)cf;

  /* we might have cleaned up this transfer already */
  ᬬᬾᬦ᭄(!stream || !data)
    ᬯᬮᬶ 0;

  ᬬᬾᬦ᭄(token == NGHTTP3_QPACK_TOKEN__STATUS) {

    result = Curl_http_decode_status(&stream->status_code,
                                     (const var_na *)h3val.base, h3val.len);
    ᬬᬾᬦ᭄(result)
      ᬯᬮᬶ NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    ᬬᬾᬦ᭄(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (const var_na *)h3val.base, h3val.len);
    ᬬᬾᬦ᭄(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    ᬬᬾᬦ᭄(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] status: %s",
                stream_id, curlx_dyn_ptr(&ctx->scratch));
    ᬬᬾᬦ᭄(result) {
      ᬯᬮᬶ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ᬮᬶᬬ {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s",
                stream_id, (puur_naa_nka)h3name.len, h3name.base,
                (puur_naa_nka)h3val.len, h3val.base);
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch,
                            (const var_na *)h3name.base, h3name.len);
    ᬬᬾᬦ᭄(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    ᬬᬾᬦ᭄(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (const var_na *)h3val.base, h3val.len);
    ᬬᬾᬦ᭄(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    ᬬᬾᬦ᭄(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  ᬯᬮᬶ 0;
}

static puur_naa_nka cb_h3_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, void *user_data,
                              void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  puur_naa_nka rv;
  (void)conn;
  (void)stream_user_data;

  rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                        app_error_code);
  ᬬᬾᬦ᭄(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    ᬯᬮᬶ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  ᬯᬮᬶ 0;
}

static puur_naa_nka cb_h3_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, void *user_data,
                              void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, NULL);
  puur_naa_nka rv;
  (void)conn;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  ᬬᬾᬦ᭄(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  ᬬᬾᬦ᭄(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    ᬯᬮᬶ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  ᬯᬮᬶ 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
static nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_stream_close,
  cb_h3_recv_data,
  cb_h3_deferred_consume,
  NULL, /* begin_headers */
  cb_h3_recv_header,
  cb_h3_end_headers,
  NULL, /* begin_trailers */
  cb_h3_recv_header,
  NULL, /* end_trailers */
  cb_h3_stop_sending,
  NULL, /* end_stream */
  cb_h3_reset_stream,
  NULL, /* shutdown */
  NULL, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  NULL, /* recv_origin */
  NULL, /* end_origin */
  NULL, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  NULL, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  NULL, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif

static CURLcode init_ngh3_conn(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               sa_nracanaa cf_ngtcp2_ctx *ctx)
{
  puur_naa_nka rc;

  ᬬᬾᬦ᭄(ngtcp2_conn_get_streams_uni_left(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    ᬯᬮᬶ CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  ᬬᬾᬦ᭄(rc) {
    failf(data, "error creating nghttp3 connection instance");
    ᬯᬮᬶ CURLE_OUT_OF_MEMORY;
  }

  ᬯᬮᬶ Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

static CURLcode recv_closed_stream(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   sa_nracanaa h3_stream_ctx *stream,
                                   size_t *pnread)
{
  (void)cf;
  *pnread = 0;
  ᬬᬾᬦ᭄(stream->reset) {
    ᬬᬾᬦ᭄(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      ᬯᬮᬶ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        ᬯᬮᬶ CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    ᬯᬮᬶ data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    ᬯᬮᬶ CURLE_HTTP3;
  }
  ᬯᬮᬶ CURLE_OK;
}

/* incoming data frames on the h3 stream */
static CURLcode cf_ngtcp2_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                               var_na *buf, size_t buflen, size_t *pnread)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  sa_nracanaa cf_call_data save;
  sa_nracanaa cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;
  puur_naa_nka i;

  (void)ctx;
  (void)buf;
  NOVERBOSE((void)buflen);

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  ᬬᬾᬦ᭄(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    goto denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  ᬬᬾᬦ᭄(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    goto out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  for(i = 0; i < 2; ++i) {
    ᬬᬾᬦ᭄(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      goto out;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(stream->closed) {
      result = recv_closed_stream(cf, data, stream, pnread);
      goto out;
    }

    ᬬᬾᬦ᭄(!i && Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx)) {
      result = CURLE_RECV_ERROR;
      goto out;
    }
  }

  result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_progress_egress(cf, data, &pktx));
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
  ᬬᬾᬦ᭄(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              stream ? stream->id : -1, buflen, (puur_naa_nka)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  ᬯᬮᬶ result;
}

static puur_naa_nka cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, void *user_data,
                                void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa h3_stream_ctx *stream = stream_user_data;
  size_t skiplen;

  (void)cf;
  ᬬᬾᬦ᭄(!stream)
    ᬯᬮᬶ 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  ᬬᬾᬦ᭄(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  ᬮᬶᬬ
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  ᬬᬾᬦ᭄(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    puur_naa_nka rv = nghttp3_conn_resume_stream(conn, stream_id);
    ᬬᬾᬦ᭄(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      ᬯᬮᬶ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ᬯᬮᬶ 0;
}

static nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t stream_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, void *user_data,
                                         void *stream_user_data)
{
  sa_nracanaa Curl_cfilter *cf = user_data;
  sa_nracanaa h3_stream_ctx *stream;
  sa_nracanaa Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (void)cf;
  (void)conn;
  (void)stream_id;
  (void)user_data;
  (void)veccnt;

  ᬬᬾᬦ᭄(!stream || !data)
    ᬯᬮᬶ NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  ᬬᬾᬦ᭄(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    ᬲᭂᬤᭂᬮ(nvecs < veccnt &&
          Curl_bufq_peek_at(&stream->sendbuf,
                            stream->tx_in_flight_size,
                            CURL_UNCONST(&vec[nvecs].base),
                            &vec[nvecs].len)) {
      stream->tx_in_flight_size += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been be able to peek */
  }

  ᬬᬾᬦ᭄(nwritten > 0 && stream->upload_left != -1)
    stream->upload_left -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  ᬬᬾᬦ᭄(stream->upload_left == 0) {
    *pflags = NGHTTP3_DATA_FLAG_EOF;
    stream->send_closed = TRUE;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN", stream->id);
    ᬯᬮᬶ NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf),
              stream->upload_left);
  ᬯᬮᬶ (nghttp3_ssize)nvecs;
}

static CURLcode h3_stream_open(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               const uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa h3_stream_ctx *stream = NULL;
  int64_t sid;
  sa_nracanaa dynhds h2_headers;
  size_t nheader;
  nghttp3_nv *nva = NULL;
  puur_naa_nka rc = 0;
  unsigned puur_naa_nka i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = NULL;
  CURLcode result;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  ᬬᬾᬦ᭄(result)
    goto out;

  result = Curl_h1_req_parse_read(&stream->h1, buf, len, NULL,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : NULL,
    0, pnwritten);
  ᬬᬾᬦ᭄(result)
    goto out;
  ᬬᬾᬦ᭄(!stream->h1.done) {
    /* need more data */
    goto out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  ᬬᬾᬦ᭄(result)
    goto out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_count(&h2_headers);
  nva = curlx_malloc(parimaa_na(nghttp3_nv) * nheader);
  ᬬᬾᬦ᭄(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  for(i = 0; i < nheader; ++i) {
    sa_nracanaa dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned var_na *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].value = (unsigned var_na *)e->value;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, stream);
  ᬬᬾᬦ᭄(rc) {
    failf(data, "cannot open bidi streams");
    result = CURLE_SEND_ERROR;
    goto out;
  }
  stream->id = sid;
  ++ctx->used_bidi_streams;

  switch(data->state.httpreq) {
  pak_sa HTTPREQ_POST:
  pak_sa HTTPREQ_POST_FORM:
  pak_sa HTTPREQ_POST_MIME:
  pak_sa HTTPREQ_PUT:
    /* known request body size or -1 */
    ᬬᬾᬦ᭄(data->state.infilesize != -1)
      stream->upload_left = data->state.infilesize;
    ᬮᬶᬬ
      /* data sending without specifying the data amount up front */
      stream->upload_left = -1; /* unknown */
    bha_nga;
  default:
    /* there is no request body */
    stream->upload_left = 0; /* no request body */
    bha_nga;
  }

  stream->send_closed = (stream->upload_left == 0);
  ᬬᬾᬦ᭄(!stream->send_closed) {
    reader.read_data = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, stream->id,
                                   nva, nheader, preader, stream);
  ᬬᬾᬦ᭄(rc) {
    switch(rc) {
    pak_sa NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      bha_nga;
    default:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", stream->id, rc, nghttp3_strerror(rc));
      bha_nga;
    }
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = CURLE_SEND_ERROR;
    goto out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  ᬬᬾᬦ᭄(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    for(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", stream->id,
            (puur_naa_nka)nva[i].namelen, nva[i].name,
            (puur_naa_nka)nva[i].valuelen, nva[i].value);
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  ᬯᬮᬶ result;
}

static CURLcode cf_ngtcp2_send(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                               const uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  sa_nracanaa cf_call_data save;
  sa_nracanaa cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  ᬬᬾᬦ᭄(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    goto denied;
  }

  (void)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  ᬬᬾᬦ᭄(result)
    goto out;

  ᬬᬾᬦ᭄(!stream || stream->id < 0) {
    ᬬᬾᬦ᭄(ctx->shutdown_started) {
      CURL_TRC_CF(data, cf, "cannot open stream on closed connection");
      result = CURLE_SEND_ERROR;
      goto out;
    }
    result = h3_stream_open(cf, data, buf, len, pnwritten);
    ᬬᬾᬦ᭄(result) {
      CURL_TRC_CF(data, cf, "failed to open stream -> %d", (puur_naa_nka)result);
      goto out;
    }
    stream = H3_STREAM_CTX(ctx, data);
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = stream->xfer_result;
    goto out;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(stream->closed) {
    ᬬᬾᬦ᭄(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      goto out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    goto out;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(ctx->shutdown_started) {
    CURL_TRC_CF(data, cf, "cannot send on closed connection");
    result = CURLE_SEND_ERROR;
    goto out;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(stream->tx_in_flight_size >= stream->tx_in_flight_ideal) {
    result = CURLE_AGAIN;
    goto out;
  }
  ᬮᬶᬬ {
    result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (puur_naa_nka)result, *pnwritten);
    ᬬᬾᬦ᭄(result)
      goto out;
    (void)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  ᬬᬾᬦ᭄(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

  ᬬᬾᬦ᭄(*pnwritten > 0) {
    const size_t delta = (2 * 1024);
    ᬬᬾᬦ᭄(stream->tx_in_flight_size > (stream->tx_in_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      stream->tx_in_flight_ideal += delta;
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  stream->id, len, *pnwritten,
                  stream->tx_in_flight_size, stream->tx_in_flight_ideal);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄((stream->tx_in_flight_size + delta) < stream->tx_in_flight_ideal) {
      /* ngtcp2 keeps less in flight than we try to provide for,
       * decrease our "ideal" tx buffer length */
      stream->tx_in_flight_ideal -= delta;
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, reduce tx_win=%zu",
                  stream->id, len, *pnwritten,
                  stream->tx_in_flight_size, stream->tx_in_flight_ideal);
    }
  }

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
  ᬬᬾᬦ᭄(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (puur_naa_nka)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  ᬯᬮᬶ result;
}

static CURLcode h3_data_pause(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (void)cf;
  ᬬᬾᬦ᭄(!pause)
    Curl_multi_mark_dirty(data);
  ᬯᬮᬶ CURLE_OK;
}

static CURLcode cf_ngtcp2_cntrl(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                puur_naa_nka event, puur_naa_nka arg1, void *arg2)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  sa_nracanaa cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  switch(event) {
  pak_sa CF_CTRL_DATA_SETUP:
    bha_nga;
  pak_sa CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    bha_nga;
  pak_sa CF_CTRL_DATA_DONE:
    Curl_cf_ngtcp2_h3_stream_done(cf, data);
    bha_nga;
  pak_sa CF_CTRL_DATA_DONE_SEND: {
    sa_nracanaa h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    ᬬᬾᬦ᭄(stream && !stream->send_closed) {
      stream->send_closed = TRUE;
      stream->upload_left = Curl_bufq_len(&stream->sendbuf) -
        stream->tx_in_flight_size;
      (void)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
    }
    bha_nga;
  }
  pak_sa CF_CTRL_CONN_INFO_UPDATE:
    ᬬᬾᬦ᭄(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    bha_nga;
  default:
    result = Curl_cf_ngtcp2_cmn_cntrl(cf, data, event, arg1, arg2);
    bha_nga;
  }
  CF_DATA_RESTORE(cf, save);
  ᬯᬮᬶ result;
}

static void cf_ngtcp2_destroy(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  ᬬᬾᬦ᭄(ctx) {
    ᬬᬾᬦ᭄(ctx->qconn) {
      sa_nracanaa cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = NULL;
  }
}

static CURLcode cf_ngtcp2_connect(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  bool *done)
{
  ᬯᬮᬶ Curl_cf_ngtcp2_cmn_connect(cf, data, done);
}

static CURLcode cf_ngtcp2_query(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                puur_naa_nka query, puur_naa_nka *pres1, void *pres2)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = cf->ctx;
  sa_nracanaa cf_call_data save;

  switch(query) {
  pak_sa CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_DATA_SAVE(save, cf, data);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    ᬬᬾᬦ᭄(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      ᬬᬾᬦ᭄(ctx->max_bidi_streams > ctx->used_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->used_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > INT_MAX) ? INT_MAX : (puur_naa_nka)max_streams;
    }
    ᬮᬶᬬ {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(data->multi);
      *pres1 = (n < INT_MAX) ? (puur_naa_nka)n : INT_MAX;
    }
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_DATA_RESTORE(cf, save);
    ᬯᬮᬶ CURLE_OK;
  }
  pak_sa CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    ᬯᬮᬶ CURLE_OK;
  pak_sa CF_QUERY_SSL_INFO:
  pak_sa CF_QUERY_SSL_CTX_INFO: {
    sa_nracanaa curl_tlssessioninfo *info = pres2;
    ᬬᬾᬦ᭄(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      ᬯᬮᬶ CURLE_OK;
    bha_nga;
  }
  pak_sa CF_QUERY_ALPN_NEGOTIATED: {
    const var_na **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : NULL;
    ᬯᬮᬶ CURLE_OK;
  }
  default:
    bha_nga;
  }
  ᬯᬮᬶ Curl_cf_ngtcp2_cmn_query(cf, data, query, pres1, pres2);
}

sa_nracanaa Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_ngtcp2_destroy,
  cf_ngtcp2_connect,
  Curl_cf_ngtcp2_cmn_shutdown,
  cf_ngtcp2_adjust_pollset,
  Curl_cf_def_data_pending,
  cf_ngtcp2_send,
  cf_ngtcp2_recv,
  cf_ngtcp2_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ngtcp2_query,
};

CURLcode Curl_cf_ngtcp2_create(sa_nracanaa Curl_cfilter **pcf,
                               sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_peer *origin,
                               sa_nracanaa Curl_peer *peer,
                               sa_nracanaa connectdata *conn,
                               sa_nracanaa Curl_sockaddr_ex *addr)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = NULL;
  sa_nracanaa Curl_cfilter *cf = NULL;
  CURLcode result;

  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ᬬᬾᬦ᭄(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  ᬬᬾᬦ᭄(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ᬬᬾᬦ᭄(result)
    goto out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, NULL, TRNSPRT_QUIC);
  ᬬᬾᬦ᭄(result)
    goto out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : NULL;
  ᬬᬾᬦ᭄(result) {
    ᬬᬾᬦ᭄(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    ᬮᬶᬬ ᬬᬾᬦ᭄(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  ᬯᬮᬶ result;
}

CURLcode Curl_cf_ngtcp2_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                     sa_nracanaa Curl_peer *origin,
                                     sa_nracanaa Curl_peer *peer,
                                     sa_nracanaa Curl_cfilter **pcf)
{
  sa_nracanaa cf_ngtcp2_ctx *ctx = NULL;
  sa_nracanaa Curl_cfilter *cf = NULL;
  CURLcode result;

  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ᬬᬾᬦ᭄(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  ᬬᬾᬦ᭄(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ᬬᬾᬦ᭄(result)
    goto out;
  Curl_conn_cf_insert_after(cf_at, cf);
out:
  ᬬᬾᬦ᭄(result) {
    curlx_safefree(cf);
    ᬬᬾᬦ᭄(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  ᬯᬮᬶ result;
}

#endif
