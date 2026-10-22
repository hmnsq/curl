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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "u32_ptrset.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "cf-socket.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "curlx/win32-fopen.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "http1.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "vdns/cf-dns.h"
#அந்தர்பாவ "vquic/vquic.h"
#அந்தர்பாவ "vquic/vquic_int.h"
#அந்தர்பாவ "vquic/cf-ngtcp2-cmn.h"
#அந்தர்பாவ "vquic/cf-ngtcp2.h"


நிலையான முழு cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, வெறுமை *user_data,
                                வெறுமை *stream_user_data);

நிலையான CURLcode cf_ngtcp2_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                         கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  எனில்(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    திரும்பு CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  எனில்(!want_send && !Curl_bufq_is_empty(&ctx->q.sendbuf))
    want_send = TRUE;

  எனில்(want_recv || want_send) {
    கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    கட்டமைப்பு cf_call_data save;
    bool c_exhaust, s_exhaust;

    CF_DATA_SAVE(save, cf, data);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_இடது(ctx->qconn) ||
                              !ngtcp2_conn_get_max_data_இடது(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                (stream->quic_flow_blocked ||
                 stream->tx_in_flight_size >= stream->tx_in_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  திரும்பு result;
}

நிலையான முழு cb_h3_stream_close(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, வெறுமை *user_data,
                              வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு h3_stream_ctx *stream;
  கட்டமைப்பு Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (வெறுமை)conn;
  (வெறுமை)stream_id;

  /* we might be called by nghttp3 after we already cleaned up */
  எனில்(!data || !stream)
    திரும்பு 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  எனில்(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  இல்லையெனில் {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  }
  Curl_multi_mark_dirty(data);
  திரும்பு 0;
}

நிலையான வெறுமை h3_xfer_write_resp_hd(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு h3_stream_ctx *stream,
                                  மாறிலி எழுத்து *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  எனில்(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, buflen, eos);
    எனில்(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (முழு)stream->xfer_result,
                  buflen);
  }
}

நிலையான வெறுமை h3_xfer_write_resp(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு h3_stream_ctx *stream,
                               மாறிலி எழுத்து *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  எனில்(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp(data, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    எனில்(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  stream->id, (முழு)stream->xfer_result, buflen);
    }
  }
}

நிலையான வெறுமை cf_ngtcp2_upd_rx_win(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு h3_stream_ctx *stream)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_STREAM_WINDOW_SIZE_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  எனில்(Curl_rlimit_active(&data->progress.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    எனில்(!stream->rx_offset)
      திரும்பு;

    avail = Curl_rlimit_avail(&data->progress.dl.rlimit, வெற்று);
    எனில்(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", stream->id, avail);
      திரும்பு;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_STREAM_WINDOW_SIZE_MAX);
  }

  எனில்(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    திரும்பு;
  }
  cur_win = stream->rx_offset_max - stream->rx_offset;

  எனில்(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    எனில்(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    எனில்(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", stream->id, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
    }
  }
}

நிலையான முழு cb_h3_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                           மாறிலி uint8_t *buf, size_t buflen,
                           வெறுமை *user_data, வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream;
  கட்டமைப்பு Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (வெறுமை)conn;
  (வெறுமை)stream3_id;

  எனில்(!stream || !data)
    திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, data, stream, (மாறிலி எழுத்து *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_offset(ctx->qconn, buflen);
  stream->rx_offset += buflen;
  எனில்(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_ngtcp2_upd_rx_win(cf, data, stream);
  திரும்பு 0;
}

நிலையான முழு cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, வெறுமை *user_data,
                                  வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = stream_user_data;
  (வெறுமை)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  எனில்(stream) {
    stream->rx_offset += consumed;
    stream->rx_offset_max += consumed;
  }
  திரும்பு 0;
}

நிலையான முழு cb_h3_end_headers(nghttp3_conn *conn, int64_t stream_id,
                             முழு fin, வெறுமை *user_data, வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு h3_stream_ctx *stream;
  கட்டமைப்பு Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (வெறுமை)conn;
  (வெறுமை)stream_id;
  (வெறுமை)fin;
  (வெறுமை)cf;

  எனில்(!stream || !data)
    திரும்பு 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                        (bool)stream->closed);

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d",
              stream_id, stream->status_code);
  எனில்(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(data);
  திரும்பு 0;
}

நிலையான முழு cb_h3_recv_header(nghttp3_conn *conn, int64_t stream_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *மதிப்பு, uint8_t flags,
                             வெறுமை *user_data, வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(மதிப்பு);
  கட்டமைப்பு h3_stream_ctx *stream;
  கட்டமைப்பு Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  CURLcode result = CURLE_OK;
  (வெறுமை)conn;
  (வெறுமை)stream_id;
  (வெறுமை)token;
  (வெறுமை)flags;
  (வெறுமை)cf;

  /* we might have cleaned up this transfer already */
  எனில்(!stream || !data)
    திரும்பு 0;

  எனில்(token == NGHTTP3_QPACK_TOKEN__STATUS) {

    result = Curl_http_decode_status(&stream->status_code,
                                     (மாறிலி எழுத்து *)h3val.base, h3val.len);
    எனில்(result)
      திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    எனில்(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (மாறிலி எழுத்து *)h3val.base, h3val.len);
    எனில்(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    எனில்(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] status: %s",
                stream_id, curlx_dyn_ptr(&ctx->scratch));
    எனில்(result) {
      திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  இல்லையெனில் {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s",
                stream_id, (முழு)h3name.len, h3name.base,
                (முழு)h3val.len, h3val.base);
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch,
                            (மாறிலி எழுத்து *)h3name.base, h3name.len);
    எனில்(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    எனில்(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (மாறிலி எழுத்து *)h3val.base, h3val.len);
    எனில்(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    எனில்(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  திரும்பு 0;
}

நிலையான முழு cb_h3_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, வெறுமை *user_data,
                              வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  முழு rv;
  (வெறுமை)conn;
  (வெறுமை)stream_user_data;

  rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                        app_error_code);
  எனில்(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  திரும்பு 0;
}

நிலையான முழு cb_h3_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, வெறுமை *user_data,
                              வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, வெற்று);
  முழு rv;
  (வெறுமை)conn;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  எனில்(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  எனில்(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  திரும்பு 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
நிலையான nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_stream_close,
  cb_h3_recv_data,
  cb_h3_deferred_consume,
  வெற்று, /* begin_headers */
  cb_h3_recv_header,
  cb_h3_end_headers,
  வெற்று, /* begin_trailers */
  cb_h3_recv_header,
  வெற்று, /* end_trailers */
  cb_h3_stop_sending,
  வெற்று, /* end_stream */
  cb_h3_reset_stream,
  வெற்று, /* shutdown */
  வெற்று, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  வெற்று, /* recv_origin */
  வெற்று, /* end_origin */
  வெற்று, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  வெற்று, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  வெற்று, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic எடு
#endif

நிலையான CURLcode init_ngh3_conn(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு cf_ngtcp2_ctx *ctx)
{
  முழு rc;

  எனில்(ngtcp2_conn_get_streams_uni_இடது(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    திரும்பு CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  எனில்(rc) {
    failf(data, "error creating nghttp3 connection instance");
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  திரும்பு Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

நிலையான CURLcode recv_closed_stream(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு h3_stream_ctx *stream,
                                   size_t *pnread)
{
  (வெறுமை)cf;
  *pnread = 0;
  எனில்(stream->reset) {
    எனில்(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      திரும்பு CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    இல்லையெனில் எனில்(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        திரும்பு CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    திரும்பு data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  இல்லையெனில் எனில்(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    திரும்பு CURLE_HTTP3;
  }
  திரும்பு CURLE_OK;
}

/* incoming data frames on the h3 stream */
நிலையான CURLcode cf_ngtcp2_recv(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                               எழுத்து *buf, size_t buflen, size_t *pnread)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  கட்டமைப்பு cf_call_data save;
  கட்டமைப்பு cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;
  முழு i;

  (வெறுமை)ctx;
  (வெறுமை)buf;
  NOVERBOSE((வெறுமை)buflen);

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  எனில்(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    செல் denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  எனில்(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    செல் out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  க்கு(i = 0; i < 2; ++i) {
    எனில்(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      செல் out;
    }
    இல்லையெனில் எனில்(stream->closed) {
      result = recv_closed_stream(cf, data, stream, pnread);
      செல் out;
    }

    எனில்(!i && Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx)) {
      result = CURLE_RECV_ERROR;
      செல் out;
    }
  }

  result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_progress_egress(cf, data, &pktx));
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
  எனில்(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              stream ? stream->id : -1, buflen, (முழு)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான முழு cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, வெறுமை *user_data,
                                வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு h3_stream_ctx *stream = stream_user_data;
  size_t skiplen;

  (வெறுமை)cf;
  எனில்(!stream)
    திரும்பு 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  எனில்(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  இல்லையெனில்
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  எனில்(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    முழு rv = nghttp3_conn_resume_stream(conn, stream_id);
    எனில்(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  திரும்பு 0;
}

நிலையான nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t stream_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, வெறுமை *user_data,
                                         வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு h3_stream_ctx *stream;
  கட்டமைப்பு Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (வெறுமை)cf;
  (வெறுமை)conn;
  (வெறுமை)stream_id;
  (வெறுமை)user_data;
  (வெறுமை)veccnt;

  எனில்(!stream || !data)
    திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  எனில்(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    போது(nvecs < veccnt &&
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

  எனில்(nwritten > 0 && stream->upload_இடது != -1)
    stream->upload_இடது -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  எனில்(stream->upload_இடது == 0) {
    *pflags = NGHTTP3_DATA_FLAG_EOF;
    stream->send_closed = TRUE;
  }
  இல்லையெனில் எனில்(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN", stream->id);
    திரும்பு NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf),
              stream->upload_இடது);
  திரும்பு (nghttp3_ssize)nvecs;
}

நிலையான CURLcode h3_stream_open(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               மாறிலி uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = வெற்று;
  int64_t sid;
  கட்டமைப்பு dynhds h2_headers;
  size_t nheader;
  nghttp3_nv *nva = வெற்று;
  முழு rc = 0;
  unsigned முழு i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = வெற்று;
  CURLcode result;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  எனில்(result)
    செல் out;

  result = Curl_h1_req_parse_read(&stream->h1, buf, len, வெற்று,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : வெற்று,
    0, pnwritten);
  எனில்(result)
    செல் out;
  எனில்(!stream->h1.done) {
    /* need more data */
    செல் out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  எனில்(result)
    செல் out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_மொத்தம்(&h2_headers);
  nva = curlx_malloc(அளவு(nghttp3_nv) * nheader);
  எனில்(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  க்கு(i = 0; i < nheader; ++i) {
    கட்டமைப்பு dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned எழுத்து *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].மதிப்பு = (unsigned எழுத்து *)e->மதிப்பு;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, stream);
  எனில்(rc) {
    failf(data, "cannot open bidi streams");
    result = CURLE_SEND_ERROR;
    செல் out;
  }
  stream->id = sid;
  ++ctx->used_bidi_streams;

  தேர்வு(data->state.httpreq) {
  நிலை HTTPREQ_POST:
  நிலை HTTPREQ_POST_FORM:
  நிலை HTTPREQ_POST_MIME:
  நிலை HTTPREQ_PUT:
    /* known request body size or -1 */
    எனில்(data->state.infilesize != -1)
      stream->upload_இடது = data->state.infilesize;
    இல்லையெனில்
      /* data sending without specifying the data amount up front */
      stream->upload_இடது = -1; /* unknown */
    நிறுத்து;
  இயல்பு:
    /* there is no request body */
    stream->upload_இடது = 0; /* no request body */
    நிறுத்து;
  }

  stream->send_closed = (stream->upload_இடது == 0);
  எனில்(!stream->send_closed) {
    reader.read_data = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, stream->id,
                                   nva, nheader, preader, stream);
  எனில்(rc) {
    தேர்வு(rc) {
    நிலை NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      நிறுத்து;
    இயல்பு:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", stream->id, rc, nghttp3_strerror(rc));
      நிறுத்து;
    }
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = CURLE_SEND_ERROR;
    செல் out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  எனில்(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    க்கு(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", stream->id,
            (முழு)nva[i].namelen, nva[i].name,
            (முழு)nva[i].valuelen, nva[i].மதிப்பு);
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  திரும்பு result;
}

நிலையான CURLcode cf_ngtcp2_send(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                               மாறிலி uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  கட்டமைப்பு cf_call_data save;
  கட்டமைப்பு cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  எனில்(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    செல் denied;
  }

  (வெறுமை)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  எனில்(result)
    செல் out;

  எனில்(!stream || stream->id < 0) {
    எனில்(ctx->shutdown_started) {
      CURL_TRC_CF(data, cf, "cannot open stream on closed connection");
      result = CURLE_SEND_ERROR;
      செல் out;
    }
    result = h3_stream_open(cf, data, buf, len, pnwritten);
    எனில்(result) {
      CURL_TRC_CF(data, cf, "failed to open stream -> %d", (முழு)result);
      செல் out;
    }
    stream = H3_STREAM_CTX(ctx, data);
  }
  இல்லையெனில் எனில்(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = stream->xfer_result;
    செல் out;
  }
  இல்லையெனில் எனில்(stream->closed) {
    எனில்(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      செல் out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    செல் out;
  }
  இல்லையெனில் எனில்(ctx->shutdown_started) {
    CURL_TRC_CF(data, cf, "cannot send on closed connection");
    result = CURLE_SEND_ERROR;
    செல் out;
  }
  இல்லையெனில் எனில்(stream->tx_in_flight_size >= stream->tx_in_flight_ideal) {
    result = CURLE_AGAIN;
    செல் out;
  }
  இல்லையெனில் {
    result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (முழு)result, *pnwritten);
    எனில்(result)
      செல் out;
    (வெறுமை)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  எனில்(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

  எனில்(*pnwritten > 0) {
    மாறிலி size_t delta = (2 * 1024);
    எனில்(stream->tx_in_flight_size > (stream->tx_in_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      stream->tx_in_flight_ideal += delta;
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  stream->id, len, *pnwritten,
                  stream->tx_in_flight_size, stream->tx_in_flight_ideal);
    }
    இல்லையெனில் எனில்((stream->tx_in_flight_size + delta) < stream->tx_in_flight_ideal) {
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
  எனில்(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (முழு)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான CURLcode h3_data_pause(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (வெறுமை)cf;
  எனில்(!pause)
    Curl_multi_mark_dirty(data);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_ngtcp2_cntrl(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                முழு event, முழு arg1, வெறுமை *arg2)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  தேர்வு(event) {
  நிலை CF_CTRL_DATA_SETUP:
    நிறுத்து;
  நிலை CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    நிறுத்து;
  நிலை CF_CTRL_DATA_DONE:
    Curl_cf_ngtcp2_h3_stream_done(cf, data);
    நிறுத்து;
  நிலை CF_CTRL_DATA_DONE_SEND: {
    கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    எனில்(stream && !stream->send_closed) {
      stream->send_closed = TRUE;
      stream->upload_இடது = Curl_bufq_len(&stream->sendbuf) -
        stream->tx_in_flight_size;
      (வெறுமை)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
    }
    நிறுத்து;
  }
  நிலை CF_CTRL_CONN_INFO_UPDATE:
    எனில்(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    நிறுத்து;
  இயல்பு:
    result = Curl_cf_ngtcp2_cmn_cntrl(cf, data, event, arg1, arg2);
    நிறுத்து;
  }
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான வெறுமை cf_ngtcp2_destroy(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  எனில்(ctx) {
    எனில்(ctx->qconn) {
      கட்டமைப்பு cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = வெற்று;
  }
}

நிலையான CURLcode cf_ngtcp2_connect(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  bool *done)
{
  திரும்பு Curl_cf_ngtcp2_cmn_connect(cf, data, done);
}

நிலையான CURLcode cf_ngtcp2_query(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                முழு query, முழு *pres1, வெறுமை *pres2)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = cf->ctx;
  கட்டமைப்பு cf_call_data save;

  தேர்வு(query) {
  நிலை CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_DATA_SAVE(save, cf, data);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    எனில்(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    இல்லையெனில் எனில்(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      எனில்(ctx->max_bidi_streams > ctx->used_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->used_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > INT_MAX) ? INT_MAX : (முழு)max_streams;
    }
    இல்லையெனில் {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(data->multi);
      *pres1 = (n < INT_MAX) ? (முழு)n : INT_MAX;
    }
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_DATA_RESTORE(cf, save);
    திரும்பு CURLE_OK;
  }
  நிலை CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    திரும்பு CURLE_OK;
  நிலை CF_QUERY_SSL_INFO:
  நிலை CF_QUERY_SSL_CTX_INFO: {
    கட்டமைப்பு curl_tlssessioninfo *info = pres2;
    எனில்(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      திரும்பு CURLE_OK;
    நிறுத்து;
  }
  நிலை CF_QUERY_ALPN_NEGOTIATED: {
    மாறிலி எழுத்து **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : வெற்று;
    திரும்பு CURLE_OK;
  }
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு Curl_cf_ngtcp2_cmn_query(cf, data, query, pres1, pres2);
}

கட்டமைப்பு Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_ngtcp2_create(கட்டமைப்பு Curl_cfilter **pcf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_peer *origin,
                               கட்டமைப்பு Curl_peer *peer,
                               கட்டமைப்பு connectdata *conn,
                               கட்டமைப்பு Curl_sockaddr_ex *addr)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = வெற்று;
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  CURLcode result;

  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  எனில்(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  எனில்(result)
    செல் out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, வெற்று, TRNSPRT_QUIC);
  எனில்(result)
    செல் out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : வெற்று;
  எனில்(result) {
    எனில்(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    இல்லையெனில் எனில்(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  திரும்பு result;
}

CURLcode Curl_cf_ngtcp2_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                     கட்டமைப்பு Curl_peer *origin,
                                     கட்டமைப்பு Curl_peer *peer,
                                     கட்டமைப்பு Curl_cfilter **pcf)
{
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = வெற்று;
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  CURLcode result;

  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  எனில்(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  எனில்(result)
    செல் out;
  Curl_conn_cf_செருகு_after(cf_at, cf);
out:
  எனில்(result) {
    curlx_safefree(cf);
    எனில்(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  திரும்பு result;
}

#endif
