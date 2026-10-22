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
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "vquic/vquic.h"
#অন্তর্ভাব "vquic/vquic_int.h"
#অন্তর্ভাব "vquic/cf-ngtcp2-cmn.h"
#অন্তর্ভাব "vquic/cf-ngtcp2.h"


স্থির পূর্ণাঙ্ক cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, শূন্য *user_data,
                                শূন্য *stream_user_data);

স্থির CURLcode cf_ngtcp2_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                         সংরচনা Curl_easy *data,
                                         সংরচনা easy_pollset *ps)
{
  সংরচনা cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  যদি(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    প্রত্যাগম CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  যদি(!want_send && !Curl_bufq_is_empty(&ctx->q.sendbuf))
    want_send = TRUE;

  যদি(want_recv || want_send) {
    সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    সংরচনা cf_call_data save;
    bool c_exhaust, s_exhaust;

    CF_DATA_SAVE(save, cf, data);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_বাম(ctx->qconn) ||
                              !ngtcp2_conn_get_max_data_বাম(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                (stream->quic_flow_blocked ||
                 stream->tx_in_flight_size >= stream->tx_in_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  প্রত্যাগম result;
}

স্থির পূর্ণাঙ্ক cb_h3_stream_close(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, শূন্য *user_data,
                              শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা h3_stream_ctx *stream;
  সংরচনা Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (শূন্য)conn;
  (শূন্য)stream_id;

  /* we might be called by nghttp3 after we already cleaned up */
  যদি(!data || !stream)
    প্রত্যাগম 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  যদি(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  অন্যথা {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  }
  Curl_multi_mark_dirty(data);
  প্রত্যাগম 0;
}

স্থির শূন্য h3_xfer_write_resp_hd(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা h3_stream_ctx *stream,
                                  ধ্রুব বর্ণ *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  যদি(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, buflen, eos);
    যদি(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (পূর্ণাঙ্ক)stream->xfer_result,
                  buflen);
  }
}

স্থির শূন্য h3_xfer_write_resp(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               সংরচনা h3_stream_ctx *stream,
                               ধ্রুব বর্ণ *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  যদি(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp(data, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    যদি(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  stream->id, (পূর্ণাঙ্ক)stream->xfer_result, buflen);
    }
  }
}

স্থির শূন্য cf_ngtcp2_upd_rx_win(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা h3_stream_ctx *stream)
{
  সংরচনা cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_STREAM_WINDOW_SIZE_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  যদি(Curl_rlimit_active(&data->progress.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    যদি(!stream->rx_offset)
      প্রত্যাগম;

    avail = Curl_rlimit_avail(&data->progress.dl.rlimit, রিক্ত);
    যদি(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", stream->id, avail);
      প্রত্যাগম;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_STREAM_WINDOW_SIZE_MAX);
  }

  যদি(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    প্রত্যাগম;
  }
  cur_win = stream->rx_offset_max - stream->rx_offset;

  যদি(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    যদি(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    যদি(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", stream->id, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
    }
  }
}

স্থির পূর্ণাঙ্ক cb_h3_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                           ধ্রুব uint8_t *buf, size_t buflen,
                           শূন্য *user_data, শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_ngtcp2_ctx *ctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream;
  সংরচনা Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (শূন্য)conn;
  (শূন্য)stream3_id;

  যদি(!stream || !data)
    প্রত্যাগম NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, data, stream, (ধ্রুব বর্ণ *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_offset(ctx->qconn, buflen);
  stream->rx_offset += buflen;
  যদি(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_ngtcp2_upd_rx_win(cf, data, stream);
  প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, শূন্য *user_data,
                                  শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_ngtcp2_ctx *ctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream = stream_user_data;
  (শূন্য)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  যদি(stream) {
    stream->rx_offset += consumed;
    stream->rx_offset_max += consumed;
  }
  প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক cb_h3_end_headers(nghttp3_conn *conn, int64_t stream_id,
                             পূর্ণাঙ্ক fin, শূন্য *user_data, শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা h3_stream_ctx *stream;
  সংরচনা Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (শূন্য)conn;
  (শূন্য)stream_id;
  (শূন্য)fin;
  (শূন্য)cf;

  যদি(!stream || !data)
    প্রত্যাগম 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                        (bool)stream->closed);

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d",
              stream_id, stream->status_code);
  যদি(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(data);
  প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক cb_h3_recv_header(nghttp3_conn *conn, int64_t stream_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *মান, uint8_t flags,
                             শূন্য *user_data, শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(মান);
  সংরচনা h3_stream_ctx *stream;
  সংরচনা Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  CURLcode result = CURLE_OK;
  (শূন্য)conn;
  (শূন্য)stream_id;
  (শূন্য)token;
  (শূন্য)flags;
  (শূন্য)cf;

  /* we might have cleaned up this transfer already */
  যদি(!stream || !data)
    প্রত্যাগম 0;

  যদি(token == NGHTTP3_QPACK_TOKEN__STATUS) {

    result = Curl_http_decode_status(&stream->status_code,
                                     (ধ্রুব বর্ণ *)h3val.base, h3val.len);
    যদি(result)
      প্রত্যাগম NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    যদি(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (ধ্রুব বর্ণ *)h3val.base, h3val.len);
    যদি(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    যদি(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] status: %s",
                stream_id, curlx_dyn_ptr(&ctx->scratch));
    যদি(result) {
      প্রত্যাগম NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  অন্যথা {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s",
                stream_id, (পূর্ণাঙ্ক)h3name.len, h3name.base,
                (পূর্ণাঙ্ক)h3val.len, h3val.base);
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch,
                            (ধ্রুব বর্ণ *)h3name.base, h3name.len);
    যদি(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    যদি(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (ধ্রুব বর্ণ *)h3val.base, h3val.len);
    যদি(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    যদি(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক cb_h3_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, শূন্য *user_data,
                              শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_ngtcp2_ctx *ctx = cf->ctx;
  পূর্ণাঙ্ক rv;
  (শূন্য)conn;
  (শূন্য)stream_user_data;

  rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                        app_error_code);
  যদি(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    প্রত্যাগম NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক cb_h3_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, শূন্য *user_data,
                              শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_ngtcp2_ctx *ctx = cf->ctx;
  সংরচনা Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, রিক্ত);
  পূর্ণাঙ্ক rv;
  (শূন্য)conn;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  যদি(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  যদি(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    প্রত্যাগম NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  প্রত্যাগম 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
স্থির nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_stream_close,
  cb_h3_recv_data,
  cb_h3_deferred_consume,
  রিক্ত, /* begin_headers */
  cb_h3_recv_header,
  cb_h3_end_headers,
  রিক্ত, /* begin_trailers */
  cb_h3_recv_header,
  রিক্ত, /* end_trailers */
  cb_h3_stop_sending,
  রিক্ত, /* end_stream */
  cb_h3_reset_stream,
  রিক্ত, /* shutdown */
  রিক্ত, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  রিক্ত, /* recv_origin */
  রিক্ত, /* end_origin */
  রিক্ত, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  রিক্ত, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  রিক্ত, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic বের_করো
#endif

স্থির CURLcode init_ngh3_conn(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               সংরচনা cf_ngtcp2_ctx *ctx)
{
  পূর্ণাঙ্ক rc;

  যদি(ngtcp2_conn_get_streams_uni_বাম(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    প্রত্যাগম CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  যদি(rc) {
    failf(data, "error creating nghttp3 connection instance");
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  প্রত্যাগম Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

স্থির CURLcode recv_closed_stream(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা h3_stream_ctx *stream,
                                   size_t *pnread)
{
  (শূন্য)cf;
  *pnread = 0;
  যদি(stream->reset) {
    যদি(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      প্রত্যাগম CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    অন্যথা যদি(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        প্রত্যাগম CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    প্রত্যাগম data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  অন্যথা যদি(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    প্রত্যাগম CURLE_HTTP3;
  }
  প্রত্যাগম CURLE_OK;
}

/* incoming data frames on the h3 stream */
স্থির CURLcode cf_ngtcp2_recv(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                               বর্ণ *buf, size_t buflen, size_t *pnread)
{
  সংরচনা cf_ngtcp2_ctx *ctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  সংরচনা cf_call_data save;
  সংরচনা cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক i;

  (শূন্য)ctx;
  (শূন্য)buf;
  NOVERBOSE((শূন্য)buflen);

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  যদি(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    গচ্ছ denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  যদি(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    গচ্ছ out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  প্রতি(i = 0; i < 2; ++i) {
    যদি(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      গচ্ছ out;
    }
    অন্যথা যদি(stream->closed) {
      result = recv_closed_stream(cf, data, stream, pnread);
      গচ্ছ out;
    }

    যদি(!i && Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx)) {
      result = CURLE_RECV_ERROR;
      গচ্ছ out;
    }
  }

  result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_progress_egress(cf, data, &pktx));
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
  যদি(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              stream ? stream->id : -1, buflen, (পূর্ণাঙ্ক)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম result;
}

স্থির পূর্ণাঙ্ক cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, শূন্য *user_data,
                                শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা h3_stream_ctx *stream = stream_user_data;
  size_t skiplen;

  (শূন্য)cf;
  যদি(!stream)
    প্রত্যাগম 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  যদি(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  অন্যথা
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  যদি(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    পূর্ণাঙ্ক rv = nghttp3_conn_resume_stream(conn, stream_id);
    যদি(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      প্রত্যাগম NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  প্রত্যাগম 0;
}

স্থির nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t stream_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, শূন্য *user_data,
                                         শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা h3_stream_ctx *stream;
  সংরচনা Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (শূন্য)cf;
  (শূন্য)conn;
  (শূন্য)stream_id;
  (শূন্য)user_data;
  (শূন্য)veccnt;

  যদি(!stream || !data)
    প্রত্যাগম NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  যদি(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    যাবত্(nvecs < veccnt &&
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

  যদি(nwritten > 0 && stream->upload_বাম != -1)
    stream->upload_বাম -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  যদি(stream->upload_বাম == 0) {
    *pflags = NGHTTP3_DATA_FLAG_EOF;
    stream->send_closed = TRUE;
  }
  অন্যথা যদি(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN", stream->id);
    প্রত্যাগম NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf),
              stream->upload_বাম);
  প্রত্যাগম (nghttp3_ssize)nvecs;
}

স্থির CURLcode h3_stream_open(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               ধ্রুব uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  সংরচনা cf_ngtcp2_ctx *ctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream = রিক্ত;
  int64_t sid;
  সংরচনা dynhds h2_headers;
  size_t nheader;
  nghttp3_nv *nva = রিক্ত;
  পূর্ণাঙ্ক rc = 0;
  unsigned পূর্ণাঙ্ক i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = রিক্ত;
  CURLcode result;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  যদি(result)
    গচ্ছ out;

  result = Curl_h1_req_parse_read(&stream->h1, buf, len, রিক্ত,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : রিক্ত,
    0, pnwritten);
  যদি(result)
    গচ্ছ out;
  যদি(!stream->h1.done) {
    /* need more data */
    গচ্ছ out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  যদি(result)
    গচ্ছ out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_গুনতি(&h2_headers);
  nva = curlx_malloc(পরিমাণ(nghttp3_nv) * nheader);
  যদি(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  প্রতি(i = 0; i < nheader; ++i) {
    সংরচনা dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned বর্ণ *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].মান = (unsigned বর্ণ *)e->মান;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, stream);
  যদি(rc) {
    failf(data, "cannot open bidi streams");
    result = CURLE_SEND_ERROR;
    গচ্ছ out;
  }
  stream->id = sid;
  ++ctx->used_bidi_streams;

  বিকল্প(data->state.httpreq) {
  পক্ষ HTTPREQ_POST:
  পক্ষ HTTPREQ_POST_FORM:
  পক্ষ HTTPREQ_POST_MIME:
  পক্ষ HTTPREQ_PUT:
    /* known request body size or -1 */
    যদি(data->state.infilesize != -1)
      stream->upload_বাম = data->state.infilesize;
    অন্যথা
      /* data sending without specifying the data amount up front */
      stream->upload_বাম = -1; /* unknown */
    ভঙ্গ;
  স্বভাব:
    /* there is no request body */
    stream->upload_বাম = 0; /* no request body */
    ভঙ্গ;
  }

  stream->send_closed = (stream->upload_বাম == 0);
  যদি(!stream->send_closed) {
    reader.read_data = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, stream->id,
                                   nva, nheader, preader, stream);
  যদি(rc) {
    বিকল্প(rc) {
    পক্ষ NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      ভঙ্গ;
    স্বভাব:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", stream->id, rc, nghttp3_strerror(rc));
      ভঙ্গ;
    }
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = CURLE_SEND_ERROR;
    গচ্ছ out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  যদি(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    প্রতি(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", stream->id,
            (পূর্ণাঙ্ক)nva[i].namelen, nva[i].name,
            (পূর্ণাঙ্ক)nva[i].valuelen, nva[i].মান);
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  প্রত্যাগম result;
}

স্থির CURLcode cf_ngtcp2_send(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                               ধ্রুব uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  সংরচনা cf_ngtcp2_ctx *ctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  সংরচনা cf_call_data save;
  সংরচনা cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  যদি(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    গচ্ছ denied;
  }

  (শূন্য)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  যদি(result)
    গচ্ছ out;

  যদি(!stream || stream->id < 0) {
    যদি(ctx->shutdown_started) {
      CURL_TRC_CF(data, cf, "cannot open stream on closed connection");
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
    result = h3_stream_open(cf, data, buf, len, pnwritten);
    যদি(result) {
      CURL_TRC_CF(data, cf, "failed to open stream -> %d", (পূর্ণাঙ্ক)result);
      গচ্ছ out;
    }
    stream = H3_STREAM_CTX(ctx, data);
  }
  অন্যথা যদি(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = stream->xfer_result;
    গচ্ছ out;
  }
  অন্যথা যদি(stream->closed) {
    যদি(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      গচ্ছ out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    গচ্ছ out;
  }
  অন্যথা যদি(ctx->shutdown_started) {
    CURL_TRC_CF(data, cf, "cannot send on closed connection");
    result = CURLE_SEND_ERROR;
    গচ্ছ out;
  }
  অন্যথা যদি(stream->tx_in_flight_size >= stream->tx_in_flight_ideal) {
    result = CURLE_AGAIN;
    গচ্ছ out;
  }
  অন্যথা {
    result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (পূর্ণাঙ্ক)result, *pnwritten);
    যদি(result)
      গচ্ছ out;
    (শূন্য)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  যদি(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

  যদি(*pnwritten > 0) {
    ধ্রুব size_t delta = (2 * 1024);
    যদি(stream->tx_in_flight_size > (stream->tx_in_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      stream->tx_in_flight_ideal += delta;
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  stream->id, len, *pnwritten,
                  stream->tx_in_flight_size, stream->tx_in_flight_ideal);
    }
    অন্যথা যদি((stream->tx_in_flight_size + delta) < stream->tx_in_flight_ideal) {
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
  যদি(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (পূর্ণাঙ্ক)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম result;
}

স্থির CURLcode h3_data_pause(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (শূন্য)cf;
  যদি(!pause)
    Curl_multi_mark_dirty(data);
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode cf_ngtcp2_cntrl(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                পূর্ণাঙ্ক event, পূর্ণাঙ্ক arg1, শূন্য *arg2)
{
  সংরচনা cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  সংরচনা cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  বিকল্প(event) {
  পক্ষ CF_CTRL_DATA_SETUP:
    ভঙ্গ;
  পক্ষ CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    ভঙ্গ;
  পক্ষ CF_CTRL_DATA_DONE:
    Curl_cf_ngtcp2_h3_stream_done(cf, data);
    ভঙ্গ;
  পক্ষ CF_CTRL_DATA_DONE_SEND: {
    সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    যদি(stream && !stream->send_closed) {
      stream->send_closed = TRUE;
      stream->upload_বাম = Curl_bufq_len(&stream->sendbuf) -
        stream->tx_in_flight_size;
      (শূন্য)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
    }
    ভঙ্গ;
  }
  পক্ষ CF_CTRL_CONN_INFO_UPDATE:
    যদি(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    ভঙ্গ;
  স্বভাব:
    result = Curl_cf_ngtcp2_cmn_cntrl(cf, data, event, arg1, arg2);
    ভঙ্গ;
  }
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম result;
}

স্থির শূন্য cf_ngtcp2_destroy(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  সংরচনা cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  যদি(ctx) {
    যদি(ctx->qconn) {
      সংরচনা cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = রিক্ত;
  }
}

স্থির CURLcode cf_ngtcp2_connect(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  bool *done)
{
  প্রত্যাগম Curl_cf_ngtcp2_cmn_connect(cf, data, done);
}

স্থির CURLcode cf_ngtcp2_query(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                পূর্ণাঙ্ক query, পূর্ণাঙ্ক *pres1, শূন্য *pres2)
{
  সংরচনা cf_ngtcp2_ctx *ctx = cf->ctx;
  সংরচনা cf_call_data save;

  বিকল্প(query) {
  পক্ষ CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_DATA_SAVE(save, cf, data);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    যদি(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    অন্যথা যদি(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      যদি(ctx->max_bidi_streams > ctx->used_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->used_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > INT_MAX) ? INT_MAX : (পূর্ণাঙ্ক)max_streams;
    }
    অন্যথা {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(data->multi);
      *pres1 = (n < INT_MAX) ? (পূর্ণাঙ্ক)n : INT_MAX;
    }
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_DATA_RESTORE(cf, save);
    প্রত্যাগম CURLE_OK;
  }
  পক্ষ CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    প্রত্যাগম CURLE_OK;
  পক্ষ CF_QUERY_SSL_INFO:
  পক্ষ CF_QUERY_SSL_CTX_INFO: {
    সংরচনা curl_tlssessioninfo *info = pres2;
    যদি(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      প্রত্যাগম CURLE_OK;
    ভঙ্গ;
  }
  পক্ষ CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুব বর্ণ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : রিক্ত;
    প্রত্যাগম CURLE_OK;
  }
  স্বভাব:
    ভঙ্গ;
  }
  প্রত্যাগম Curl_cf_ngtcp2_cmn_query(cf, data, query, pres1, pres2);
}

সংরচনা Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_ngtcp2_create(সংরচনা Curl_cfilter **pcf,
                               সংরচনা Curl_easy *data,
                               সংরচনা Curl_peer *origin,
                               সংরচনা Curl_peer *peer,
                               সংরচনা connectdata *conn,
                               সংরচনা Curl_sockaddr_ex *addr)
{
  সংরচনা cf_ngtcp2_ctx *ctx = রিক্ত;
  সংরচনা Curl_cfilter *cf = রিক্ত;
  CURLcode result;

  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  যদি(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  যদি(result)
    গচ্ছ out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, রিক্ত, TRNSPRT_QUIC);
  যদি(result)
    গচ্ছ out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : রিক্ত;
  যদি(result) {
    যদি(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    অন্যথা যদি(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  প্রত্যাগম result;
}

CURLcode Curl_cf_ngtcp2_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                     সংরচনা Curl_peer *origin,
                                     সংরচনা Curl_peer *peer,
                                     সংরচনা Curl_cfilter **pcf)
{
  সংরচনা cf_ngtcp2_ctx *ctx = রিক্ত;
  সংরচনা Curl_cfilter *cf = রিক্ত;
  CURLcode result;

  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  যদি(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  যদি(result)
    গচ্ছ out;
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);
out:
  যদি(result) {
    curlx_safefree(cf);
    যদি(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  প্রত্যাগম result;
}

#endif
