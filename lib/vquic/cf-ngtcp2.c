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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "u32_ptrset.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "cf-socket.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "curlx/win32-fopen.h"
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "http1.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "bufref.h"
#અન્તર્ભાવ "vdns/cf-dns.h"
#અન્તર્ભાવ "vquic/vquic.h"
#અન્તર્ભાવ "vquic/vquic_int.h"
#અન્તર્ભાવ "vquic/cf-ngtcp2-cmn.h"
#અન્તર્ભાવ "vquic/cf-ngtcp2.h"


સ્થિર પૂર્ણાંક cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, શૂન્ય *user_data,
                                શૂન્ય *stream_user_data);

સ્થિર CURLcode cf_ngtcp2_adjust_pollset(સંરચના Curl_cfilter *cf,
                                         સંરચના Curl_easy *data,
                                         સંરચના easy_pollset *ps)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  જો(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    પાછા CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  જો(!want_send && !Curl_bufq_is_empty(&ctx->q.sendbuf))
    want_send = TRUE;

  જો(want_recv || want_send) {
    સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    સંરચના cf_call_data save;
    bool c_exhaust, s_exhaust;

    CF_DATA_SAVE(save, cf, data);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_ડાબું(ctx->qconn) ||
                              !ngtcp2_conn_get_max_data_ડાબું(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                (stream->quic_flow_blocked ||
                 stream->tx_in_flight_size >= stream->tx_in_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  પાછા result;
}

સ્થિર પૂર્ણાંક cb_h3_stream_close(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, શૂન્ય *user_data,
                              શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના h3_stream_ctx *stream;
  સંરચના Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (શૂન્ય)conn;
  (શૂન્ય)stream_id;

  /* we might be called by nghttp3 after we already cleaned up */
  જો(!data || !stream)
    પાછા 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  જો(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  નહીંતર {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  }
  Curl_multi_mark_dirty(data);
  પાછા 0;
}

સ્થિર શૂન્ય h3_xfer_write_resp_hd(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  સંરચના h3_stream_ctx *stream,
                                  અચલ અક્ષર *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  જો(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, buflen, eos);
    જો(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (પૂર્ણાંક)stream->xfer_result,
                  buflen);
  }
}

સ્થિર શૂન્ય h3_xfer_write_resp(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               સંરચના h3_stream_ctx *stream,
                               અચલ અક્ષર *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  જો(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp(data, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    જો(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  stream->id, (પૂર્ણાંક)stream->xfer_result, buflen);
    }
  }
}

સ્થિર શૂન્ય cf_ngtcp2_upd_rx_win(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના h3_stream_ctx *stream)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_STREAM_WINDOW_SIZE_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  જો(Curl_rlimit_active(&data->progress.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    જો(!stream->rx_offset)
      પાછા;

    avail = Curl_rlimit_avail(&data->progress.dl.rlimit, ખાલી);
    જો(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", stream->id, avail);
      પાછા;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_STREAM_WINDOW_SIZE_MAX);
  }

  જો(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    પાછા;
  }
  cur_win = stream->rx_offset_max - stream->rx_offset;

  જો(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    જો(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    જો(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", stream->id, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
    }
  }
}

સ્થિર પૂર્ણાંક cb_h3_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                           અચલ uint8_t *buf, size_t buflen,
                           શૂન્ય *user_data, શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream;
  સંરચના Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (શૂન્ય)conn;
  (શૂન્ય)stream3_id;

  જો(!stream || !data)
    પાછા NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, data, stream, (અચલ અક્ષર *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_offset(ctx->qconn, buflen);
  stream->rx_offset += buflen;
  જો(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_ngtcp2_upd_rx_win(cf, data, stream);
  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, શૂન્ય *user_data,
                                  શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = stream_user_data;
  (શૂન્ય)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  જો(stream) {
    stream->rx_offset += consumed;
    stream->rx_offset_max += consumed;
  }
  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_h3_end_headers(nghttp3_conn *conn, int64_t stream_id,
                             પૂર્ણાંક fin, શૂન્ય *user_data, શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના h3_stream_ctx *stream;
  સંરચના Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (શૂન્ય)conn;
  (શૂન્ય)stream_id;
  (શૂન્ય)fin;
  (શૂન્ય)cf;

  જો(!stream || !data)
    પાછા 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                        (bool)stream->closed);

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d",
              stream_id, stream->status_code);
  જો(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(data);
  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_h3_recv_header(nghttp3_conn *conn, int64_t stream_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *કિંમત, uint8_t flags,
                             શૂન્ય *user_data, શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(કિંમત);
  સંરચના h3_stream_ctx *stream;
  સંરચના Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  CURLcode result = CURLE_OK;
  (શૂન્ય)conn;
  (શૂન્ય)stream_id;
  (શૂન્ય)token;
  (શૂન્ય)flags;
  (શૂન્ય)cf;

  /* we might have cleaned up this transfer already */
  જો(!stream || !data)
    પાછા 0;

  જો(token == NGHTTP3_QPACK_TOKEN__STATUS) {

    result = Curl_http_decode_status(&stream->status_code,
                                     (અચલ અક્ષર *)h3val.base, h3val.len);
    જો(result)
      પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    જો(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (અચલ અક્ષર *)h3val.base, h3val.len);
    જો(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    જો(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] status: %s",
                stream_id, curlx_dyn_ptr(&ctx->scratch));
    જો(result) {
      પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  નહીંતર {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s",
                stream_id, (પૂર્ણાંક)h3name.len, h3name.base,
                (પૂર્ણાંક)h3val.len, h3val.base);
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch,
                            (અચલ અક્ષર *)h3name.base, h3name.len);
    જો(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    જો(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (અચલ અક્ષર *)h3val.base, h3val.len);
    જો(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    જો(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_h3_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, શૂન્ય *user_data,
                              શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  પૂર્ણાંક rv;
  (શૂન્ય)conn;
  (શૂન્ય)stream_user_data;

  rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                        app_error_code);
  જો(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_h3_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, શૂન્ય *user_data,
                              શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ખાલી);
  પૂર્ણાંક rv;
  (શૂન્ય)conn;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  જો(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  જો(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  પાછા 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ધકેલો
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
સ્થિર nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_stream_close,
  cb_h3_recv_data,
  cb_h3_deferred_consume,
  ખાલી, /* begin_headers */
  cb_h3_recv_header,
  cb_h3_end_headers,
  ખાલી, /* begin_trailers */
  cb_h3_recv_header,
  ખાલી, /* end_trailers */
  cb_h3_stop_sending,
  ખાલી, /* end_stream */
  cb_h3_reset_stream,
  ખાલી, /* shutdown */
  ખાલી, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  ખાલી, /* recv_origin */
  ખાલી, /* end_origin */
  ખાલી, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  ખાલી, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  ખાલી, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic કાઢો
#endif

સ્થિર CURLcode init_ngh3_conn(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               સંરચના cf_ngtcp2_ctx *ctx)
{
  પૂર્ણાંક rc;

  જો(ngtcp2_conn_get_streams_uni_ડાબું(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    પાછા CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  જો(rc) {
    failf(data, "error creating nghttp3 connection instance");
    પાછા CURLE_OUT_OF_MEMORY;
  }

  પાછા Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

સ્થિર CURLcode recv_closed_stream(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   સંરચના h3_stream_ctx *stream,
                                   size_t *pnread)
{
  (શૂન્ય)cf;
  *pnread = 0;
  જો(stream->reset) {
    જો(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      પાછા CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    નહીંતર જો(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        પાછા CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    પાછા data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  નહીંતર જો(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    પાછા CURLE_HTTP3;
  }
  પાછા CURLE_OK;
}

/* incoming data frames on the h3 stream */
સ્થિર CURLcode cf_ngtcp2_recv(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                               અક્ષર *buf, size_t buflen, size_t *pnread)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  સંરચના cf_call_data save;
  સંરચના cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;
  પૂર્ણાંક i;

  (શૂન્ય)ctx;
  (શૂન્ય)buf;
  NOVERBOSE((શૂન્ય)buflen);

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  જો(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    જાઓ denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  જો(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    જાઓ out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  માટે(i = 0; i < 2; ++i) {
    જો(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      જાઓ out;
    }
    નહીંતર જો(stream->closed) {
      result = recv_closed_stream(cf, data, stream, pnread);
      જાઓ out;
    }

    જો(!i && Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx)) {
      result = CURLE_RECV_ERROR;
      જાઓ out;
    }
  }

  result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_progress_egress(cf, data, &pktx));
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
  જો(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              stream ? stream->id : -1, buflen, (પૂર્ણાંક)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર પૂર્ણાંક cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, શૂન્ય *user_data,
                                શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના h3_stream_ctx *stream = stream_user_data;
  size_t skiplen;

  (શૂન્ય)cf;
  જો(!stream)
    પાછા 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  જો(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  નહીંતર
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  જો(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    પૂર્ણાંક rv = nghttp3_conn_resume_stream(conn, stream_id);
    જો(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  પાછા 0;
}

સ્થિર nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t stream_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, શૂન્ય *user_data,
                                         શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના h3_stream_ctx *stream;
  સંરચના Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (શૂન્ય)cf;
  (શૂન્ય)conn;
  (શૂન્ય)stream_id;
  (શૂન્ય)user_data;
  (શૂન્ય)veccnt;

  જો(!stream || !data)
    પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  જો(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    જ્યારે(nvecs < veccnt &&
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

  જો(nwritten > 0 && stream->upload_ડાબું != -1)
    stream->upload_ડાબું -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  જો(stream->upload_ડાબું == 0) {
    *pflags = NGHTTP3_DATA_FLAG_EOF;
    stream->send_closed = TRUE;
  }
  નહીંતર જો(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN", stream->id);
    પાછા NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf),
              stream->upload_ડાબું);
  પાછા (nghttp3_ssize)nvecs;
}

સ્થિર CURLcode h3_stream_open(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               અચલ uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = ખાલી;
  int64_t sid;
  સંરચના dynhds h2_headers;
  size_t nheader;
  nghttp3_nv *nva = ખાલી;
  પૂર્ણાંક rc = 0;
  unsigned પૂર્ણાંક i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = ખાલી;
  CURLcode result;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  જો(result)
    જાઓ out;

  result = Curl_h1_req_parse_read(&stream->h1, buf, len, ખાલી,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : ખાલી,
    0, pnwritten);
  જો(result)
    જાઓ out;
  જો(!stream->h1.done) {
    /* need more data */
    જાઓ out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  જો(result)
    જાઓ out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_ગણતરી(&h2_headers);
  nva = curlx_malloc(કદ(nghttp3_nv) * nheader);
  જો(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  માટે(i = 0; i < nheader; ++i) {
    સંરચના dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned અક્ષર *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].કિંમત = (unsigned અક્ષર *)e->કિંમત;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, stream);
  જો(rc) {
    failf(data, "cannot open bidi streams");
    result = CURLE_SEND_ERROR;
    જાઓ out;
  }
  stream->id = sid;
  ++ctx->used_bidi_streams;

  પસંદગી(data->state.httpreq) {
  સ્થિતિ HTTPREQ_POST:
  સ્થિતિ HTTPREQ_POST_FORM:
  સ્થિતિ HTTPREQ_POST_MIME:
  સ્થિતિ HTTPREQ_PUT:
    /* known request body size or -1 */
    જો(data->state.infilesize != -1)
      stream->upload_ડાબું = data->state.infilesize;
    નહીંતર
      /* data sending without specifying the data amount up front */
      stream->upload_ડાબું = -1; /* unknown */
    તોડો;
  મૂળભૂત:
    /* there is no request body */
    stream->upload_ડાબું = 0; /* no request body */
    તોડો;
  }

  stream->send_closed = (stream->upload_ડાબું == 0);
  જો(!stream->send_closed) {
    reader.read_data = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, stream->id,
                                   nva, nheader, preader, stream);
  જો(rc) {
    પસંદગી(rc) {
    સ્થિતિ NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      તોડો;
    મૂળભૂત:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", stream->id, rc, nghttp3_strerror(rc));
      તોડો;
    }
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = CURLE_SEND_ERROR;
    જાઓ out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  જો(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    માટે(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", stream->id,
            (પૂર્ણાંક)nva[i].namelen, nva[i].name,
            (પૂર્ણાંક)nva[i].valuelen, nva[i].કિંમત);
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  પાછા result;
}

સ્થિર CURLcode cf_ngtcp2_send(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                               અચલ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  સંરચના cf_call_data save;
  સંરચના cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  જો(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    જાઓ denied;
  }

  (શૂન્ય)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  જો(result)
    જાઓ out;

  જો(!stream || stream->id < 0) {
    જો(ctx->shutdown_started) {
      CURL_TRC_CF(data, cf, "cannot open stream on closed connection");
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
    result = h3_stream_open(cf, data, buf, len, pnwritten);
    જો(result) {
      CURL_TRC_CF(data, cf, "failed to open stream -> %d", (પૂર્ણાંક)result);
      જાઓ out;
    }
    stream = H3_STREAM_CTX(ctx, data);
  }
  નહીંતર જો(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = stream->xfer_result;
    જાઓ out;
  }
  નહીંતર જો(stream->closed) {
    જો(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      જાઓ out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    જાઓ out;
  }
  નહીંતર જો(ctx->shutdown_started) {
    CURL_TRC_CF(data, cf, "cannot send on closed connection");
    result = CURLE_SEND_ERROR;
    જાઓ out;
  }
  નહીંતર જો(stream->tx_in_flight_size >= stream->tx_in_flight_ideal) {
    result = CURLE_AGAIN;
    જાઓ out;
  }
  નહીંતર {
    result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (પૂર્ણાંક)result, *pnwritten);
    જો(result)
      જાઓ out;
    (શૂન્ય)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  જો(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

  જો(*pnwritten > 0) {
    અચલ size_t delta = (2 * 1024);
    જો(stream->tx_in_flight_size > (stream->tx_in_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      stream->tx_in_flight_ideal += delta;
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  stream->id, len, *pnwritten,
                  stream->tx_in_flight_size, stream->tx_in_flight_ideal);
    }
    નહીંતર જો((stream->tx_in_flight_size + delta) < stream->tx_in_flight_ideal) {
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
  જો(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (પૂર્ણાંક)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર CURLcode h3_data_pause(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (શૂન્ય)cf;
  જો(!pause)
    Curl_multi_mark_dirty(data);
  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_ngtcp2_cntrl(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  સંરચના cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  પસંદગી(event) {
  સ્થિતિ CF_CTRL_DATA_SETUP:
    તોડો;
  સ્થિતિ CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    તોડો;
  સ્થિતિ CF_CTRL_DATA_DONE:
    Curl_cf_ngtcp2_h3_stream_done(cf, data);
    તોડો;
  સ્થિતિ CF_CTRL_DATA_DONE_SEND: {
    સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    જો(stream && !stream->send_closed) {
      stream->send_closed = TRUE;
      stream->upload_ડાબું = Curl_bufq_len(&stream->sendbuf) -
        stream->tx_in_flight_size;
      (શૂન્ય)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
    }
    તોડો;
  }
  સ્થિતિ CF_CTRL_CONN_INFO_UPDATE:
    જો(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    તોડો;
  મૂળભૂત:
    result = Curl_cf_ngtcp2_cmn_cntrl(cf, data, event, arg1, arg2);
    તોડો;
  }
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર શૂન્ય cf_ngtcp2_destroy(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  જો(ctx) {
    જો(ctx->qconn) {
      સંરચના cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = ખાલી;
  }
}

સ્થિર CURLcode cf_ngtcp2_connect(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  bool *done)
{
  પાછા Curl_cf_ngtcp2_cmn_connect(cf, data, done);
}

સ્થિર CURLcode cf_ngtcp2_query(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  સંરચના cf_ngtcp2_ctx *ctx = cf->ctx;
  સંરચના cf_call_data save;

  પસંદગી(query) {
  સ્થિતિ CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_DATA_SAVE(save, cf, data);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    જો(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    નહીંતર જો(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      જો(ctx->max_bidi_streams > ctx->used_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->used_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > INT_MAX) ? INT_MAX : (પૂર્ણાંક)max_streams;
    }
    નહીંતર {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(data->multi);
      *pres1 = (n < INT_MAX) ? (પૂર્ણાંક)n : INT_MAX;
    }
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_DATA_RESTORE(cf, save);
    પાછા CURLE_OK;
  }
  સ્થિતિ CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    પાછા CURLE_OK;
  સ્થિતિ CF_QUERY_SSL_INFO:
  સ્થિતિ CF_QUERY_SSL_CTX_INFO: {
    સંરચના curl_tlssessioninfo *info = pres2;
    જો(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      પાછા CURLE_OK;
    તોડો;
  }
  સ્થિતિ CF_QUERY_ALPN_NEGOTIATED: {
    અચલ અક્ષર **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ખાલી;
    પાછા CURLE_OK;
  }
  મૂળભૂત:
    તોડો;
  }
  પાછા Curl_cf_ngtcp2_cmn_query(cf, data, query, pres1, pres2);
}

સંરચના Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_ngtcp2_create(સંરચના Curl_cfilter **pcf,
                               સંરચના Curl_easy *data,
                               સંરચના Curl_peer *origin,
                               સંરચના Curl_peer *peer,
                               સંરચના connectdata *conn,
                               સંરચના Curl_sockaddr_ex *addr)
{
  સંરચના cf_ngtcp2_ctx *ctx = ખાલી;
  સંરચના Curl_cfilter *cf = ખાલી;
  CURLcode result;

  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  જો(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  જો(result)
    જાઓ out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ખાલી, TRNSPRT_QUIC);
  જો(result)
    જાઓ out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : ખાલી;
  જો(result) {
    જો(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    નહીંતર જો(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  પાછા result;
}

CURLcode Curl_cf_ngtcp2_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                     સંરચના Curl_peer *origin,
                                     સંરચના Curl_peer *peer,
                                     સંરચના Curl_cfilter **pcf)
{
  સંરચના cf_ngtcp2_ctx *ctx = ખાલી;
  સંરચના Curl_cfilter *cf = ખાલી;
  CURLcode result;

  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  જો(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  જો(result)
    જાઓ out;
  Curl_conn_cf_ઉમેરો_after(cf_at, cf);
out:
  જો(result) {
    curlx_safefree(cf);
    જો(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  પાછા result;
}

#endif
