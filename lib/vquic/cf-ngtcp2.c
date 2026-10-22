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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#شامل "urldata.h"
#شامل "url.h"
#شامل "u32_ptrset.h"
#شامل "curl_trc.h"
#شامل "rand.h"
#شامل "multiif.h"
#شامل "cfilters.h"
#شامل "cf-socket.h"
#شامل "connect.h"
#شامل "progress.h"
#شامل "curlx/win32-fopen.h"
#شامل "curlx/dynbuf.h"
#شامل "http1.h"
#شامل "select.h"
#شامل "transfer.h"
#شامل "bufref.h"
#شامل "vdns/cf-dns.h"
#شامل "vquic/vquic.h"
#شامل "vquic/vquic_int.h"
#شامل "vquic/cf-ngtcp2-cmn.h"
#شامل "vquic/cf-ngtcp2.h"


ایستا صحیح cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, تهی *user_data,
                                تهی *stream_user_data);

ایستا CURLcode cf_ngtcp2_adjust_pollset(ساختار Curl_cfilter *cf,
                                         ساختار Curl_easy *data,
                                         ساختار easy_pollset *ps)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  اگر(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    بازگشت CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  اگر(!want_send && !Curl_bufq_is_empty(&ctx->q.sendbuf))
    want_send = TRUE;

  اگر(want_recv || want_send) {
    ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    ساختار cf_call_data save;
    bool c_exhaust, s_exhaust;

    CF_DATA_SAVE(save, cf, data);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_چپ(ctx->qconn) ||
                              !ngtcp2_conn_get_max_data_چپ(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                (stream->quic_flow_blocked ||
                 stream->tx_in_flight_size >= stream->tx_in_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  بازگشت result;
}

ایستا صحیح cb_h3_stream_close(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, تهی *user_data,
                              تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار h3_stream_ctx *stream;
  ساختار Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (تهی)conn;
  (تهی)stream_id;

  /* we might be called by nghttp3 after we already cleaned up */
  اگر(!data || !stream)
    بازگشت 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  اگر(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  وگرنه {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  }
  Curl_multi_mark_dirty(data);
  بازگشت 0;
}

ایستا تهی h3_xfer_write_resp_hd(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  ساختار h3_stream_ctx *stream,
                                  ثابت نویسه *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  اگر(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, buflen, eos);
    اگر(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (صحیح)stream->xfer_result,
                  buflen);
  }
}

ایستا تهی h3_xfer_write_resp(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ساختار h3_stream_ctx *stream,
                               ثابت نویسه *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  اگر(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp(data, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    اگر(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  stream->id, (صحیح)stream->xfer_result, buflen);
    }
  }
}

ایستا تهی cf_ngtcp2_upd_rx_win(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار h3_stream_ctx *stream)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_STREAM_WINDOW_SIZE_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  اگر(Curl_rlimit_active(&data->progress.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    اگر(!stream->rx_offset)
      بازگشت;

    avail = Curl_rlimit_avail(&data->progress.dl.rlimit, پوچ);
    اگر(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", stream->id, avail);
      بازگشت;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_STREAM_WINDOW_SIZE_MAX);
  }

  اگر(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    بازگشت;
  }
  cur_win = stream->rx_offset_max - stream->rx_offset;

  اگر(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    اگر(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    اگر(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", stream->id, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
    }
  }
}

ایستا صحیح cb_h3_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                           ثابت uint8_t *buf, size_t buflen,
                           تهی *user_data, تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream;
  ساختار Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (تهی)conn;
  (تهی)stream3_id;

  اگر(!stream || !data)
    بازگشت NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, data, stream, (ثابت نویسه *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_offset(ctx->qconn, buflen);
  stream->rx_offset += buflen;
  اگر(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_ngtcp2_upd_rx_win(cf, data, stream);
  بازگشت 0;
}

ایستا صحیح cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, تهی *user_data,
                                  تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = stream_user_data;
  (تهی)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  اگر(stream) {
    stream->rx_offset += consumed;
    stream->rx_offset_max += consumed;
  }
  بازگشت 0;
}

ایستا صحیح cb_h3_end_headers(nghttp3_conn *conn, int64_t stream_id,
                             صحیح fin, تهی *user_data, تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار h3_stream_ctx *stream;
  ساختار Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (تهی)conn;
  (تهی)stream_id;
  (تهی)fin;
  (تهی)cf;

  اگر(!stream || !data)
    بازگشت 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                        (bool)stream->closed);

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d",
              stream_id, stream->status_code);
  اگر(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(data);
  بازگشت 0;
}

ایستا صحیح cb_h3_recv_header(nghttp3_conn *conn, int64_t stream_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *مقدار, uint8_t flags,
                             تهی *user_data, تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(مقدار);
  ساختار h3_stream_ctx *stream;
  ساختار Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  CURLcode result = CURLE_OK;
  (تهی)conn;
  (تهی)stream_id;
  (تهی)token;
  (تهی)flags;
  (تهی)cf;

  /* we might have cleaned up this transfer already */
  اگر(!stream || !data)
    بازگشت 0;

  اگر(token == NGHTTP3_QPACK_TOKEN__STATUS) {

    result = Curl_http_decode_status(&stream->status_code,
                                     (ثابت نویسه *)h3val.base, h3val.len);
    اگر(result)
      بازگشت NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    اگر(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (ثابت نویسه *)h3val.base, h3val.len);
    اگر(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    اگر(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] status: %s",
                stream_id, curlx_dyn_ptr(&ctx->scratch));
    اگر(result) {
      بازگشت NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  وگرنه {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s",
                stream_id, (صحیح)h3name.len, h3name.base,
                (صحیح)h3val.len, h3val.base);
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch,
                            (ثابت نویسه *)h3name.base, h3name.len);
    اگر(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    اگر(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (ثابت نویسه *)h3val.base, h3val.len);
    اگر(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    اگر(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  بازگشت 0;
}

ایستا صحیح cb_h3_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, تهی *user_data,
                              تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  صحیح rv;
  (تهی)conn;
  (تهی)stream_user_data;

  rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                        app_error_code);
  اگر(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    بازگشت NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  بازگشت 0;
}

ایستا صحیح cb_h3_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, تهی *user_data,
                              تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, پوچ);
  صحیح rv;
  (تهی)conn;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  اگر(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  اگر(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    بازگشت NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  بازگشت 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
ایستا nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_stream_close,
  cb_h3_recv_data,
  cb_h3_deferred_consume,
  پوچ, /* begin_headers */
  cb_h3_recv_header,
  cb_h3_end_headers,
  پوچ, /* begin_trailers */
  cb_h3_recv_header,
  پوچ, /* end_trailers */
  cb_h3_stop_sending,
  پوچ, /* end_stream */
  cb_h3_reset_stream,
  پوچ, /* shutdown */
  پوچ, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  پوچ, /* recv_origin */
  پوچ, /* end_origin */
  پوچ, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  پوچ, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  پوچ, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic برداشت
#endif

ایستا CURLcode init_ngh3_conn(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ساختار cf_ngtcp2_ctx *ctx)
{
  صحیح rc;

  اگر(ngtcp2_conn_get_streams_uni_چپ(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    بازگشت CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  اگر(rc) {
    failf(data, "error creating nghttp3 connection instance");
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  بازگشت Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

ایستا CURLcode recv_closed_stream(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   ساختار h3_stream_ctx *stream,
                                   size_t *pnread)
{
  (تهی)cf;
  *pnread = 0;
  اگر(stream->reset) {
    اگر(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      بازگشت CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وگرنه اگر(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        بازگشت CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    بازگشت data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  وگرنه اگر(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    بازگشت CURLE_HTTP3;
  }
  بازگشت CURLE_OK;
}

/* incoming data frames on the h3 stream */
ایستا CURLcode cf_ngtcp2_recv(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                               نویسه *buf, size_t buflen, size_t *pnread)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  ساختار cf_call_data save;
  ساختار cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;
  صحیح i;

  (تهی)ctx;
  (تهی)buf;
  NOVERBOSE((تهی)buflen);

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  اگر(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    برو denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  اگر(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    برو out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  برای(i = 0; i < 2; ++i) {
    اگر(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      برو out;
    }
    وگرنه اگر(stream->closed) {
      result = recv_closed_stream(cf, data, stream, pnread);
      برو out;
    }

    اگر(!i && Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx)) {
      result = CURLE_RECV_ERROR;
      برو out;
    }
  }

  result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_progress_egress(cf, data, &pktx));
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
  اگر(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              stream ? stream->id : -1, buflen, (صحیح)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا صحیح cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, تهی *user_data,
                                تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار h3_stream_ctx *stream = stream_user_data;
  size_t skiplen;

  (تهی)cf;
  اگر(!stream)
    بازگشت 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  اگر(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  وگرنه
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  اگر(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    صحیح rv = nghttp3_conn_resume_stream(conn, stream_id);
    اگر(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      بازگشت NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  بازگشت 0;
}

ایستا nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t stream_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, تهی *user_data,
                                         تهی *stream_user_data)
{
  ساختار Curl_cfilter *cf = user_data;
  ساختار h3_stream_ctx *stream;
  ساختار Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (تهی)cf;
  (تهی)conn;
  (تهی)stream_id;
  (تهی)user_data;
  (تهی)veccnt;

  اگر(!stream || !data)
    بازگشت NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  اگر(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    وقتی(nvecs < veccnt &&
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

  اگر(nwritten > 0 && stream->upload_چپ != -1)
    stream->upload_چپ -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  اگر(stream->upload_چپ == 0) {
    *pflags = NGHTTP3_DATA_FLAG_EOF;
    stream->send_closed = TRUE;
  }
  وگرنه اگر(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN", stream->id);
    بازگشت NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf),
              stream->upload_چپ);
  بازگشت (nghttp3_ssize)nvecs;
}

ایستا CURLcode h3_stream_open(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ثابت uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = پوچ;
  int64_t sid;
  ساختار dynhds h2_headers;
  size_t nheader;
  nghttp3_nv *nva = پوچ;
  صحیح rc = 0;
  unsigned صحیح i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = پوچ;
  CURLcode result;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  اگر(result)
    برو out;

  result = Curl_h1_req_parse_read(&stream->h1, buf, len, پوچ,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : پوچ,
    0, pnwritten);
  اگر(result)
    برو out;
  اگر(!stream->h1.done) {
    /* need more data */
    برو out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  اگر(result)
    برو out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_شمار(&h2_headers);
  nva = curlx_malloc(اندازه(nghttp3_nv) * nheader);
  اگر(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  برای(i = 0; i < nheader; ++i) {
    ساختار dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned نویسه *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].مقدار = (unsigned نویسه *)e->مقدار;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, stream);
  اگر(rc) {
    failf(data, "cannot open bidi streams");
    result = CURLE_SEND_ERROR;
    برو out;
  }
  stream->id = sid;
  ++ctx->used_bidi_streams;

  انتخاب(data->state.httpreq) {
  حالت HTTPREQ_POST:
  حالت HTTPREQ_POST_FORM:
  حالت HTTPREQ_POST_MIME:
  حالت HTTPREQ_PUT:
    /* known request body size or -1 */
    اگر(data->state.infilesize != -1)
      stream->upload_چپ = data->state.infilesize;
    وگرنه
      /* data sending without specifying the data amount up front */
      stream->upload_چپ = -1; /* unknown */
    بشکن;
  پیشفرض:
    /* there is no request body */
    stream->upload_چپ = 0; /* no request body */
    بشکن;
  }

  stream->send_closed = (stream->upload_چپ == 0);
  اگر(!stream->send_closed) {
    reader.read_data = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, stream->id,
                                   nva, nheader, preader, stream);
  اگر(rc) {
    انتخاب(rc) {
    حالت NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      بشکن;
    پیشفرض:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", stream->id, rc, nghttp3_strerror(rc));
      بشکن;
    }
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = CURLE_SEND_ERROR;
    برو out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  اگر(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    برای(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", stream->id,
            (صحیح)nva[i].namelen, nva[i].name,
            (صحیح)nva[i].valuelen, nva[i].مقدار);
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  بازگشت result;
}

ایستا CURLcode cf_ngtcp2_send(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  ساختار cf_call_data save;
  ساختار cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  اگر(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    برو denied;
  }

  (تهی)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  اگر(result)
    برو out;

  اگر(!stream || stream->id < 0) {
    اگر(ctx->shutdown_started) {
      CURL_TRC_CF(data, cf, "cannot open stream on closed connection");
      result = CURLE_SEND_ERROR;
      برو out;
    }
    result = h3_stream_open(cf, data, buf, len, pnwritten);
    اگر(result) {
      CURL_TRC_CF(data, cf, "failed to open stream -> %d", (صحیح)result);
      برو out;
    }
    stream = H3_STREAM_CTX(ctx, data);
  }
  وگرنه اگر(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = stream->xfer_result;
    برو out;
  }
  وگرنه اگر(stream->closed) {
    اگر(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      برو out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    برو out;
  }
  وگرنه اگر(ctx->shutdown_started) {
    CURL_TRC_CF(data, cf, "cannot send on closed connection");
    result = CURLE_SEND_ERROR;
    برو out;
  }
  وگرنه اگر(stream->tx_in_flight_size >= stream->tx_in_flight_ideal) {
    result = CURLE_AGAIN;
    برو out;
  }
  وگرنه {
    result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (صحیح)result, *pnwritten);
    اگر(result)
      برو out;
    (تهی)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  اگر(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

  اگر(*pnwritten > 0) {
    ثابت size_t delta = (2 * 1024);
    اگر(stream->tx_in_flight_size > (stream->tx_in_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      stream->tx_in_flight_ideal += delta;
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  stream->id, len, *pnwritten,
                  stream->tx_in_flight_size, stream->tx_in_flight_ideal);
    }
    وگرنه اگر((stream->tx_in_flight_size + delta) < stream->tx_in_flight_ideal) {
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
  اگر(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (صحیح)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا CURLcode h3_data_pause(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (تهی)cf;
  اگر(!pause)
    Curl_multi_mark_dirty(data);
  بازگشت CURLE_OK;
}

ایستا CURLcode cf_ngtcp2_cntrl(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                صحیح event, صحیح arg1, تهی *arg2)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ساختار cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  انتخاب(event) {
  حالت CF_CTRL_DATA_SETUP:
    بشکن;
  حالت CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    بشکن;
  حالت CF_CTRL_DATA_DONE:
    Curl_cf_ngtcp2_h3_stream_done(cf, data);
    بشکن;
  حالت CF_CTRL_DATA_DONE_SEND: {
    ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    اگر(stream && !stream->send_closed) {
      stream->send_closed = TRUE;
      stream->upload_چپ = Curl_bufq_len(&stream->sendbuf) -
        stream->tx_in_flight_size;
      (تهی)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
    }
    بشکن;
  }
  حالت CF_CTRL_CONN_INFO_UPDATE:
    اگر(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    بشکن;
  پیشفرض:
    result = Curl_cf_ngtcp2_cmn_cntrl(cf, data, event, arg1, arg2);
    بشکن;
  }
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا تهی cf_ngtcp2_destroy(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  اگر(ctx) {
    اگر(ctx->qconn) {
      ساختار cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = پوچ;
  }
}

ایستا CURLcode cf_ngtcp2_connect(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  bool *done)
{
  بازگشت Curl_cf_ngtcp2_cmn_connect(cf, data, done);
}

ایستا CURLcode cf_ngtcp2_query(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                صحیح query, صحیح *pres1, تهی *pres2)
{
  ساختار cf_ngtcp2_ctx *ctx = cf->ctx;
  ساختار cf_call_data save;

  انتخاب(query) {
  حالت CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_DATA_SAVE(save, cf, data);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    اگر(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    وگرنه اگر(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      اگر(ctx->max_bidi_streams > ctx->used_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->used_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > INT_MAX) ? INT_MAX : (صحیح)max_streams;
    }
    وگرنه {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(data->multi);
      *pres1 = (n < INT_MAX) ? (صحیح)n : INT_MAX;
    }
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_DATA_RESTORE(cf, save);
    بازگشت CURLE_OK;
  }
  حالت CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    بازگشت CURLE_OK;
  حالت CF_QUERY_SSL_INFO:
  حالت CF_QUERY_SSL_CTX_INFO: {
    ساختار curl_tlssessioninfo *info = pres2;
    اگر(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      بازگشت CURLE_OK;
    بشکن;
  }
  حالت CF_QUERY_ALPN_NEGOTIATED: {
    ثابت نویسه **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : پوچ;
    بازگشت CURLE_OK;
  }
  پیشفرض:
    بشکن;
  }
  بازگشت Curl_cf_ngtcp2_cmn_query(cf, data, query, pres1, pres2);
}

ساختار Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_ngtcp2_create(ساختار Curl_cfilter **pcf,
                               ساختار Curl_easy *data,
                               ساختار Curl_peer *origin,
                               ساختار Curl_peer *peer,
                               ساختار connectdata *conn,
                               ساختار Curl_sockaddr_ex *addr)
{
  ساختار cf_ngtcp2_ctx *ctx = پوچ;
  ساختار Curl_cfilter *cf = پوچ;
  CURLcode result;

  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  اگر(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  اگر(result)
    برو out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, پوچ, TRNSPRT_QUIC);
  اگر(result)
    برو out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : پوچ;
  اگر(result) {
    اگر(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    وگرنه اگر(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  بازگشت result;
}

CURLcode Curl_cf_ngtcp2_درج_after(ساختار Curl_cfilter *cf_at,
                                     ساختار Curl_peer *origin,
                                     ساختار Curl_peer *peer,
                                     ساختار Curl_cfilter **pcf)
{
  ساختار cf_ngtcp2_ctx *ctx = پوچ;
  ساختار Curl_cfilter *cf = پوچ;
  CURLcode result;

  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  اگر(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  اگر(result)
    برو out;
  Curl_conn_cf_درج_after(cf_at, cf);
out:
  اگر(result) {
    curlx_safefree(cf);
    اگر(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  بازگشت result;
}

#endif
