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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ضمن "urldata.h"
#ضمن "url.h"
#ضمن "u32_ptrset.h"
#ضمن "curl_trc.h"
#ضمن "rand.h"
#ضمن "multiif.h"
#ضمن "cfilters.h"
#ضمن "cf-socket.h"
#ضمن "connect.h"
#ضمن "progress.h"
#ضمن "curlx/win32-fopen.h"
#ضمن "curlx/dynbuf.h"
#ضمن "http1.h"
#ضمن "select.h"
#ضمن "transfer.h"
#ضمن "bufref.h"
#ضمن "vdns/cf-dns.h"
#ضمن "vquic/vquic.h"
#ضمن "vquic/vquic_int.h"
#ضمن "vquic/cf-ngtcp2-cmn.h"
#ضمن "vquic/cf-ngtcp2.h"


ساكن صحيح cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, فراغ *user_data,
                                فراغ *stream_user_data);

ساكن CURLcode cf_ngtcp2_adjust_pollset(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *data,
                                         بنية easy_pollset *ps)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  إذا(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    أرجع CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  إذا(!want_send && !Curl_bufq_is_empty(&ctx->q.sendbuf))
    want_send = TRUE;

  إذا(want_recv || want_send) {
    بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    بنية cf_call_data save;
    bool c_exhaust, s_exhaust;

    CF_DATA_SAVE(save, cf, data);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_يسار(ctx->qconn) ||
                              !ngtcp2_conn_get_max_data_يسار(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                (stream->quic_flow_blocked ||
                 stream->tx_in_flight_size >= stream->tx_in_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  أرجع result;
}

ساكن صحيح cb_h3_stream_close(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, فراغ *user_data,
                              فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية h3_stream_ctx *stream;
  بنية Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (فراغ)conn;
  (فراغ)stream_id;

  /* we might be called by nghttp3 after we already cleaned up */
  إذا(!data || !stream)
    أرجع 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  إذا(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  وإلا {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  }
  Curl_multi_mark_dirty(data);
  أرجع 0;
}

ساكن فراغ h3_xfer_write_resp_hd(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  بنية h3_stream_ctx *stream,
                                  ثابت حرف *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  إذا(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, buflen, eos);
    إذا(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (صحيح)stream->xfer_result,
                  buflen);
  }
}

ساكن فراغ h3_xfer_write_resp(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               بنية h3_stream_ctx *stream,
                               ثابت حرف *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  إذا(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp(data, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    إذا(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  stream->id, (صحيح)stream->xfer_result, buflen);
    }
  }
}

ساكن فراغ cf_ngtcp2_upd_rx_win(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 بنية h3_stream_ctx *stream)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_STREAM_WINDOW_SIZE_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  إذا(Curl_rlimit_active(&data->progress.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    إذا(!stream->rx_offset)
      أرجع;

    avail = Curl_rlimit_avail(&data->progress.dl.rlimit, عدم);
    إذا(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", stream->id, avail);
      أرجع;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_STREAM_WINDOW_SIZE_MAX);
  }

  إذا(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    أرجع;
  }
  cur_win = stream->rx_offset_max - stream->rx_offset;

  إذا(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    إذا(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    إذا(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", stream->id, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
    }
  }
}

ساكن صحيح cb_h3_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                           ثابت uint8_t *buf, size_t buflen,
                           فراغ *user_data, فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream;
  بنية Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (فراغ)conn;
  (فراغ)stream3_id;

  إذا(!stream || !data)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, data, stream, (ثابت حرف *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_offset(ctx->qconn, buflen);
  stream->rx_offset += buflen;
  إذا(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_ngtcp2_upd_rx_win(cf, data, stream);
  أرجع 0;
}

ساكن صحيح cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, فراغ *user_data,
                                  فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = stream_user_data;
  (فراغ)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  إذا(stream) {
    stream->rx_offset += consumed;
    stream->rx_offset_max += consumed;
  }
  أرجع 0;
}

ساكن صحيح cb_h3_end_headers(nghttp3_conn *conn, int64_t stream_id,
                             صحيح fin, فراغ *user_data, فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية h3_stream_ctx *stream;
  بنية Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (فراغ)conn;
  (فراغ)stream_id;
  (فراغ)fin;
  (فراغ)cf;

  إذا(!stream || !data)
    أرجع 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                        (bool)stream->closed);

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d",
              stream_id, stream->status_code);
  إذا(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(data);
  أرجع 0;
}

ساكن صحيح cb_h3_recv_header(nghttp3_conn *conn, int64_t stream_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *قيمة, uint8_t flags,
                             فراغ *user_data, فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(قيمة);
  بنية h3_stream_ctx *stream;
  بنية Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  CURLcode result = CURLE_OK;
  (فراغ)conn;
  (فراغ)stream_id;
  (فراغ)token;
  (فراغ)flags;
  (فراغ)cf;

  /* we might have cleaned up this transfer already */
  إذا(!stream || !data)
    أرجع 0;

  إذا(token == NGHTTP3_QPACK_TOKEN__STATUS) {

    result = Curl_http_decode_status(&stream->status_code,
                                     (ثابت حرف *)h3val.base, h3val.len);
    إذا(result)
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    إذا(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (ثابت حرف *)h3val.base, h3val.len);
    إذا(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    إذا(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] status: %s",
                stream_id, curlx_dyn_ptr(&ctx->scratch));
    إذا(result) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  وإلا {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s",
                stream_id, (صحيح)h3name.len, h3name.base,
                (صحيح)h3val.len, h3val.base);
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch,
                            (ثابت حرف *)h3name.base, h3name.len);
    إذا(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    إذا(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (ثابت حرف *)h3val.base, h3val.len);
    إذا(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    إذا(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  أرجع 0;
}

ساكن صحيح cb_h3_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, فراغ *user_data,
                              فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)conn;
  (فراغ)stream_user_data;

  rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                        app_error_code);
  إذا(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_h3_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, فراغ *user_data,
                              فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, عدم);
  صحيح rv;
  (فراغ)conn;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  إذا(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  إذا(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ادفع
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
ساكن nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_stream_close,
  cb_h3_recv_data,
  cb_h3_deferred_consume,
  عدم, /* begin_headers */
  cb_h3_recv_header,
  cb_h3_end_headers,
  عدم, /* begin_trailers */
  cb_h3_recv_header,
  عدم, /* end_trailers */
  cb_h3_stop_sending,
  عدم, /* end_stream */
  cb_h3_reset_stream,
  عدم, /* shutdown */
  عدم, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  عدم, /* recv_origin */
  عدم, /* end_origin */
  عدم, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  عدم, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  عدم, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic اسحب
#endif

ساكن CURLcode init_ngh3_conn(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               بنية cf_ngtcp2_ctx *ctx)
{
  صحيح rc;

  إذا(ngtcp2_conn_get_streams_uni_يسار(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  إذا(rc) {
    failf(data, "error creating nghttp3 connection instance");
    أرجع CURLE_OUT_OF_MEMORY;
  }

  أرجع Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

ساكن CURLcode recv_closed_stream(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data,
                                   بنية h3_stream_ctx *stream,
                                   size_t *pnread)
{
  (فراغ)cf;
  *pnread = 0;
  إذا(stream->reset) {
    إذا(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      أرجع CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وإلا إذا(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        أرجع CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    أرجع data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  وإلا إذا(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    أرجع CURLE_HTTP3;
  }
  أرجع CURLE_OK;
}

/* incoming data frames on the h3 stream */
ساكن CURLcode cf_ngtcp2_recv(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                               حرف *buf, size_t buflen, size_t *pnread)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  بنية cf_call_data save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;
  صحيح i;

  (فراغ)ctx;
  (فراغ)buf;
  NOVERBOSE((فراغ)buflen);

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  إذا(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    اذهب denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  إذا(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    اذهب out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  لكل(i = 0; i < 2; ++i) {
    إذا(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      اذهب out;
    }
    وإلا إذا(stream->closed) {
      result = recv_closed_stream(cf, data, stream, pnread);
      اذهب out;
    }

    إذا(!i && Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx)) {
      result = CURLE_RECV_ERROR;
      اذهب out;
    }
  }

  result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_progress_egress(cf, data, &pktx));
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
  إذا(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              stream ? stream->id : -1, buflen, (صحيح)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  أرجع result;
}

ساكن صحيح cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, فراغ *user_data,
                                فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية h3_stream_ctx *stream = stream_user_data;
  size_t skiplen;

  (فراغ)cf;
  إذا(!stream)
    أرجع 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  إذا(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  وإلا
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  إذا(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    صحيح rv = nghttp3_conn_resume_stream(conn, stream_id);
    إذا(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  أرجع 0;
}

ساكن nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t stream_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, فراغ *user_data,
                                         فراغ *stream_user_data)
{
  بنية Curl_cfilter *cf = user_data;
  بنية h3_stream_ctx *stream;
  بنية Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (فراغ)cf;
  (فراغ)conn;
  (فراغ)stream_id;
  (فراغ)user_data;
  (فراغ)veccnt;

  إذا(!stream || !data)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  إذا(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    طالما(nvecs < veccnt &&
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

  إذا(nwritten > 0 && stream->upload_يسار != -1)
    stream->upload_يسار -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  إذا(stream->upload_يسار == 0) {
    *pflags = NGHTTP3_DATA_FLAG_EOF;
    stream->send_closed = TRUE;
  }
  وإلا إذا(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN", stream->id);
    أرجع NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf),
              stream->upload_يسار);
  أرجع (nghttp3_ssize)nvecs;
}

ساكن CURLcode h3_stream_open(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               ثابت uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = عدم;
  int64_t sid;
  بنية dynhds h2_headers;
  size_t nheader;
  nghttp3_nv *nva = عدم;
  صحيح rc = 0;
  unsigned صحيح i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = عدم;
  CURLcode result;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  إذا(result)
    اذهب out;

  result = Curl_h1_req_parse_read(&stream->h1, buf, len, عدم,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : عدم,
    0, pnwritten);
  إذا(result)
    اذهب out;
  إذا(!stream->h1.done) {
    /* need more data */
    اذهب out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  إذا(result)
    اذهب out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_عدد(&h2_headers);
  nva = curlx_malloc(حجم(nghttp3_nv) * nheader);
  إذا(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  لكل(i = 0; i < nheader; ++i) {
    بنية dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned حرف *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].قيمة = (unsigned حرف *)e->قيمة;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, stream);
  إذا(rc) {
    failf(data, "cannot open bidi streams");
    result = CURLE_SEND_ERROR;
    اذهب out;
  }
  stream->id = sid;
  ++ctx->used_bidi_streams;

  بدل(data->state.httpreq) {
  حالة HTTPREQ_POST:
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
  حالة HTTPREQ_PUT:
    /* known request body size or -1 */
    إذا(data->state.infilesize != -1)
      stream->upload_يسار = data->state.infilesize;
    وإلا
      /* data sending without specifying the data amount up front */
      stream->upload_يسار = -1; /* unknown */
    اكسر;
  افتراضي:
    /* there is no request body */
    stream->upload_يسار = 0; /* no request body */
    اكسر;
  }

  stream->send_closed = (stream->upload_يسار == 0);
  إذا(!stream->send_closed) {
    reader.read_data = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, stream->id,
                                   nva, nheader, preader, stream);
  إذا(rc) {
    بدل(rc) {
    حالة NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      اكسر;
    افتراضي:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", stream->id, rc, nghttp3_strerror(rc));
      اكسر;
    }
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = CURLE_SEND_ERROR;
    اذهب out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  إذا(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    لكل(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", stream->id,
            (صحيح)nva[i].namelen, nva[i].name,
            (صحيح)nva[i].valuelen, nva[i].قيمة);
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  أرجع result;
}

ساكن CURLcode cf_ngtcp2_send(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  بنية cf_call_data save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  إذا(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    اذهب denied;
  }

  (فراغ)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  إذا(result)
    اذهب out;

  إذا(!stream || stream->id < 0) {
    إذا(ctx->shutdown_started) {
      CURL_TRC_CF(data, cf, "cannot open stream on closed connection");
      result = CURLE_SEND_ERROR;
      اذهب out;
    }
    result = h3_stream_open(cf, data, buf, len, pnwritten);
    إذا(result) {
      CURL_TRC_CF(data, cf, "failed to open stream -> %d", (صحيح)result);
      اذهب out;
    }
    stream = H3_STREAM_CTX(ctx, data);
  }
  وإلا إذا(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = stream->xfer_result;
    اذهب out;
  }
  وإلا إذا(stream->closed) {
    إذا(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      اذهب out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    اذهب out;
  }
  وإلا إذا(ctx->shutdown_started) {
    CURL_TRC_CF(data, cf, "cannot send on closed connection");
    result = CURLE_SEND_ERROR;
    اذهب out;
  }
  وإلا إذا(stream->tx_in_flight_size >= stream->tx_in_flight_ideal) {
    result = CURLE_AGAIN;
    اذهب out;
  }
  وإلا {
    result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (صحيح)result, *pnwritten);
    إذا(result)
      اذهب out;
    (فراغ)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  إذا(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

  إذا(*pnwritten > 0) {
    ثابت size_t delta = (2 * 1024);
    إذا(stream->tx_in_flight_size > (stream->tx_in_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      stream->tx_in_flight_ideal += delta;
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  stream->id, len, *pnwritten,
                  stream->tx_in_flight_size, stream->tx_in_flight_ideal);
    }
    وإلا إذا((stream->tx_in_flight_size + delta) < stream->tx_in_flight_ideal) {
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
  إذا(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (صحيح)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  أرجع result;
}

ساكن CURLcode h3_data_pause(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (فراغ)cf;
  إذا(!pause)
    Curl_multi_mark_dirty(data);
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ngtcp2_cntrl(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  بنية cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  بدل(event) {
  حالة CF_CTRL_DATA_SETUP:
    اكسر;
  حالة CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    اكسر;
  حالة CF_CTRL_DATA_DONE:
    Curl_cf_ngtcp2_h3_stream_done(cf, data);
    اكسر;
  حالة CF_CTRL_DATA_DONE_SEND: {
    بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    إذا(stream && !stream->send_closed) {
      stream->send_closed = TRUE;
      stream->upload_يسار = Curl_bufq_len(&stream->sendbuf) -
        stream->tx_in_flight_size;
      (فراغ)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
    }
    اكسر;
  }
  حالة CF_CTRL_CONN_INFO_UPDATE:
    إذا(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    اكسر;
  افتراضي:
    result = Curl_cf_ngtcp2_cmn_cntrl(cf, data, event, arg1, arg2);
    اكسر;
  }
  CF_DATA_RESTORE(cf, save);
  أرجع result;
}

ساكن فراغ cf_ngtcp2_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  إذا(ctx) {
    إذا(ctx->qconn) {
      بنية cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_ngtcp2_connect(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  bool *done)
{
  أرجع Curl_cf_ngtcp2_cmn_connect(cf, data, done);
}

ساكن CURLcode cf_ngtcp2_query(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_call_data save;

  بدل(query) {
  حالة CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_DATA_SAVE(save, cf, data);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    إذا(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    وإلا إذا(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      إذا(ctx->max_bidi_streams > ctx->used_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->used_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > INT_MAX) ? INT_MAX : (صحيح)max_streams;
    }
    وإلا {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(data->multi);
      *pres1 = (n < INT_MAX) ? (صحيح)n : INT_MAX;
    }
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_DATA_RESTORE(cf, save);
    أرجع CURLE_OK;
  }
  حالة CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    أرجع CURLE_OK;
  حالة CF_QUERY_SSL_INFO:
  حالة CF_QUERY_SSL_CTX_INFO: {
    بنية curl_tlssessioninfo *info = pres2;
    إذا(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      أرجع CURLE_OK;
    اكسر;
  }
  حالة CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : عدم;
    أرجع CURLE_OK;
  }
  افتراضي:
    اكسر;
  }
  أرجع Curl_cf_ngtcp2_cmn_query(cf, data, query, pres1, pres2);
}

بنية Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_ngtcp2_create(بنية Curl_cfilter **pcf,
                               بنية Curl_easy *data,
                               بنية Curl_peer *origin,
                               بنية Curl_peer *peer,
                               بنية connectdata *conn,
                               بنية Curl_sockaddr_ex *addr)
{
  بنية cf_ngtcp2_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode result;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  إذا(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(result)
    اذهب out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, عدم, TRNSPRT_QUIC);
  إذا(result)
    اذهب out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : عدم;
  إذا(result) {
    إذا(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    وإلا إذا(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  أرجع result;
}

CURLcode Curl_cf_ngtcp2_أدرج_after(بنية Curl_cfilter *cf_at,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     بنية Curl_cfilter **pcf)
{
  بنية cf_ngtcp2_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode result;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  إذا(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(result)
    اذهب out;
  Curl_conn_cf_أدرج_after(cf_at, cf);
out:
  إذا(result) {
    curlx_safefree(cf);
    إذا(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  أرجع result;
}

#endif
