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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "u32_ptrset.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "cf-socket.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "curlx/win32-fopen.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "http1.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "vdns/cf-dns.h"
#അന്തര്ഭാവ "vquic/vquic.h"
#അന്തര്ഭാവ "vquic/vquic_int.h"
#അന്തര്ഭാവ "vquic/cf-ngtcp2-cmn.h"
#അന്തര്ഭാവ "vquic/cf-ngtcp2.h"


സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, ശൂന്യം *user_data,
                                ശൂന്യം *stream_user_data);

സ്ഥിതം CURLcode cf_ngtcp2_adjust_pollset(ഘടന Curl_cfilter *cf,
                                         ഘടന Curl_easy *data,
                                         ഘടന easy_pollset *ps)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  എങ്കിൽ(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    തിരികെ CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  എങ്കിൽ(!want_send && !Curl_bufq_is_empty(&ctx->q.sendbuf))
    want_send = TRUE;

  എങ്കിൽ(want_recv || want_send) {
    ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    ഘടന cf_call_data save;
    bool c_exhaust, s_exhaust;

    CF_DATA_SAVE(save, cf, data);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_ഇടത്(ctx->qconn) ||
                              !ngtcp2_conn_get_max_data_ഇടത്(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                (stream->quic_flow_blocked ||
                 stream->tx_in_flight_size >= stream->tx_in_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  തിരികെ result;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_stream_close(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, ശൂന്യം *user_data,
                              ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന h3_stream_ctx *stream;
  ഘടന Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (ശൂന്യം)conn;
  (ശൂന്യം)stream_id;

  /* we might be called by nghttp3 after we already cleaned up */
  എങ്കിൽ(!data || !stream)
    തിരികെ 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  എങ്കിൽ(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  അല്ലെങ്കിൽ {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  }
  Curl_multi_mark_dirty(data);
  തിരികെ 0;
}

സ്ഥിതം ശൂന്യം h3_xfer_write_resp_hd(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  ഘടന h3_stream_ctx *stream,
                                  സ്ഥിരം അക്ഷരം *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  എങ്കിൽ(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, buflen, eos);
    എങ്കിൽ(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (പൂർണ്ണസംഖ്യ)stream->xfer_result,
                  buflen);
  }
}

സ്ഥിതം ശൂന്യം h3_xfer_write_resp(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               ഘടന h3_stream_ctx *stream,
                               സ്ഥിരം അക്ഷരം *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  എങ്കിൽ(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp(data, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    എങ്കിൽ(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  stream->id, (പൂർണ്ണസംഖ്യ)stream->xfer_result, buflen);
    }
  }
}

സ്ഥിതം ശൂന്യം cf_ngtcp2_upd_rx_win(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന h3_stream_ctx *stream)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_STREAM_WINDOW_SIZE_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  എങ്കിൽ(Curl_rlimit_active(&data->progress.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    എങ്കിൽ(!stream->rx_offset)
      തിരികെ;

    avail = Curl_rlimit_avail(&data->progress.dl.rlimit, ശൂന്യ);
    എങ്കിൽ(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", stream->id, avail);
      തിരികെ;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_STREAM_WINDOW_SIZE_MAX);
  }

  എങ്കിൽ(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    തിരികെ;
  }
  cur_win = stream->rx_offset_max - stream->rx_offset;

  എങ്കിൽ(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    എങ്കിൽ(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    എങ്കിൽ(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", stream->id, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
    }
  }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                           സ്ഥിരം uint8_t *buf, size_t buflen,
                           ശൂന്യം *user_data, ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream;
  ഘടന Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (ശൂന്യം)conn;
  (ശൂന്യം)stream3_id;

  എങ്കിൽ(!stream || !data)
    തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, data, stream, (സ്ഥിരം അക്ഷരം *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_offset(ctx->qconn, buflen);
  stream->rx_offset += buflen;
  എങ്കിൽ(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_ngtcp2_upd_rx_win(cf, data, stream);
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, ശൂന്യം *user_data,
                                  ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = stream_user_data;
  (ശൂന്യം)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  എങ്കിൽ(stream) {
    stream->rx_offset += consumed;
    stream->rx_offset_max += consumed;
  }
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_end_headers(nghttp3_conn *conn, int64_t stream_id,
                             പൂർണ്ണസംഖ്യ fin, ശൂന്യം *user_data, ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന h3_stream_ctx *stream;
  ഘടന Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (ശൂന്യം)conn;
  (ശൂന്യം)stream_id;
  (ശൂന്യം)fin;
  (ശൂന്യം)cf;

  എങ്കിൽ(!stream || !data)
    തിരികെ 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                        (bool)stream->closed);

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d",
              stream_id, stream->status_code);
  എങ്കിൽ(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(data);
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_recv_header(nghttp3_conn *conn, int64_t stream_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *മൂല്യം, uint8_t flags,
                             ശൂന്യം *user_data, ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(മൂല്യം);
  ഘടന h3_stream_ctx *stream;
  ഘടന Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  CURLcode result = CURLE_OK;
  (ശൂന്യം)conn;
  (ശൂന്യം)stream_id;
  (ശൂന്യം)token;
  (ശൂന്യം)flags;
  (ശൂന്യം)cf;

  /* we might have cleaned up this transfer already */
  എങ്കിൽ(!stream || !data)
    തിരികെ 0;

  എങ്കിൽ(token == NGHTTP3_QPACK_TOKEN__STATUS) {

    result = Curl_http_decode_status(&stream->status_code,
                                     (സ്ഥിരം അക്ഷരം *)h3val.base, h3val.len);
    എങ്കിൽ(result)
      തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (സ്ഥിരം അക്ഷരം *)h3val.base, h3val.len);
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    എങ്കിൽ(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] status: %s",
                stream_id, curlx_dyn_ptr(&ctx->scratch));
    എങ്കിൽ(result) {
      തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  അല്ലെങ്കിൽ {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s",
                stream_id, (പൂർണ്ണസംഖ്യ)h3name.len, h3name.base,
                (പൂർണ്ണസംഖ്യ)h3val.len, h3val.base);
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch,
                            (സ്ഥിരം അക്ഷരം *)h3name.base, h3name.len);
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (സ്ഥിരം അക്ഷരം *)h3val.base, h3val.len);
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    എങ്കിൽ(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, ശൂന്യം *user_data,
                              ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  പൂർണ്ണസംഖ്യ rv;
  (ശൂന്യം)conn;
  (ശൂന്യം)stream_user_data;

  rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                        app_error_code);
  എങ്കിൽ(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, ശൂന്യം *user_data,
                              ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ശൂന്യ);
  പൂർണ്ണസംഖ്യ rv;
  (ശൂന്യം)conn;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  എങ്കിൽ(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  എങ്കിൽ(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  തിരികെ 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
സ്ഥിതം nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_stream_close,
  cb_h3_recv_data,
  cb_h3_deferred_consume,
  ശൂന്യ, /* begin_headers */
  cb_h3_recv_header,
  cb_h3_end_headers,
  ശൂന്യ, /* begin_trailers */
  cb_h3_recv_header,
  ശൂന്യ, /* end_trailers */
  cb_h3_stop_sending,
  ശൂന്യ, /* end_stream */
  cb_h3_reset_stream,
  ശൂന്യ, /* shutdown */
  ശൂന്യ, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  ശൂന്യ, /* recv_origin */
  ശൂന്യ, /* end_origin */
  ശൂന്യ, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  ശൂന്യ, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  ശൂന്യ, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic എടുക്കുക
#endif

സ്ഥിതം CURLcode init_ngh3_conn(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               ഘടന cf_ngtcp2_ctx *ctx)
{
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(ngtcp2_conn_get_streams_uni_ഇടത്(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    തിരികെ CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  എങ്കിൽ(rc) {
    failf(data, "error creating nghttp3 connection instance");
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  തിരികെ Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

സ്ഥിതം CURLcode recv_closed_stream(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ഘടന h3_stream_ctx *stream,
                                   size_t *pnread)
{
  (ശൂന്യം)cf;
  *pnread = 0;
  എങ്കിൽ(stream->reset) {
    എങ്കിൽ(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      തിരികെ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    അല്ലെങ്കിൽ എങ്കിൽ(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        തിരികെ CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    തിരികെ data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    തിരികെ CURLE_HTTP3;
  }
  തിരികെ CURLE_OK;
}

/* incoming data frames on the h3 stream */
സ്ഥിതം CURLcode cf_ngtcp2_recv(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                               അക്ഷരം *buf, size_t buflen, size_t *pnread)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  ഘടന cf_call_data save;
  ഘടന cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ i;

  (ശൂന്യം)ctx;
  (ശൂന്യം)buf;
  NOVERBOSE((ശൂന്യം)buflen);

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  എങ്കിൽ(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    പോകുക denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  എങ്കിൽ(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    പോകുക out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  വേണ്ടി(i = 0; i < 2; ++i) {
    എങ്കിൽ(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(stream->closed) {
      result = recv_closed_stream(cf, data, stream, pnread);
      പോകുക out;
    }

    എങ്കിൽ(!i && Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx)) {
      result = CURLE_RECV_ERROR;
      പോകുക out;
    }
  }

  result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_progress_egress(cf, data, &pktx));
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
  എങ്കിൽ(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              stream ? stream->id : -1, buflen, (പൂർണ്ണസംഖ്യ)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, ശൂന്യം *user_data,
                                ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന h3_stream_ctx *stream = stream_user_data;
  size_t skiplen;

  (ശൂന്യം)cf;
  എങ്കിൽ(!stream)
    തിരികെ 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  എങ്കിൽ(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  അല്ലെങ്കിൽ
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  എങ്കിൽ(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    പൂർണ്ണസംഖ്യ rv = nghttp3_conn_resume_stream(conn, stream_id);
    എങ്കിൽ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  തിരികെ 0;
}

സ്ഥിതം nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t stream_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, ശൂന്യം *user_data,
                                         ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന h3_stream_ctx *stream;
  ഘടന Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (ശൂന്യം)cf;
  (ശൂന്യം)conn;
  (ശൂന്യം)stream_id;
  (ശൂന്യം)user_data;
  (ശൂന്യം)veccnt;

  എങ്കിൽ(!stream || !data)
    തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  എങ്കിൽ(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    ആകുമ്പോൾ(nvecs < veccnt &&
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

  എങ്കിൽ(nwritten > 0 && stream->upload_ഇടത് != -1)
    stream->upload_ഇടത് -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  എങ്കിൽ(stream->upload_ഇടത് == 0) {
    *pflags = NGHTTP3_DATA_FLAG_EOF;
    stream->send_closed = TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN", stream->id);
    തിരികെ NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf),
              stream->upload_ഇടത്);
  തിരികെ (nghttp3_ssize)nvecs;
}

സ്ഥിതം CURLcode h3_stream_open(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               സ്ഥിരം uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = ശൂന്യ;
  int64_t sid;
  ഘടന dynhds h2_headers;
  size_t nheader;
  nghttp3_nv *nva = ശൂന്യ;
  പൂർണ്ണസംഖ്യ rc = 0;
  unsigned പൂർണ്ണസംഖ്യ i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = ശൂന്യ;
  CURLcode result;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  എങ്കിൽ(result)
    പോകുക out;

  result = Curl_h1_req_parse_read(&stream->h1, buf, len, ശൂന്യ,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : ശൂന്യ,
    0, pnwritten);
  എങ്കിൽ(result)
    പോകുക out;
  എങ്കിൽ(!stream->h1.done) {
    /* need more data */
    പോകുക out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  എങ്കിൽ(result)
    പോകുക out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_എണ്ണം(&h2_headers);
  nva = curlx_malloc(വലുപ്പം(nghttp3_nv) * nheader);
  എങ്കിൽ(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  വേണ്ടി(i = 0; i < nheader; ++i) {
    ഘടന dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned അക്ഷരം *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].മൂല്യം = (unsigned അക്ഷരം *)e->മൂല്യം;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, stream);
  എങ്കിൽ(rc) {
    failf(data, "cannot open bidi streams");
    result = CURLE_SEND_ERROR;
    പോകുക out;
  }
  stream->id = sid;
  ++ctx->used_bidi_streams;

  തിരഞ്ഞെടുപ്പ്(data->state.httpreq) {
  അവസ്ഥ HTTPREQ_POST:
  അവസ്ഥ HTTPREQ_POST_FORM:
  അവസ്ഥ HTTPREQ_POST_MIME:
  അവസ്ഥ HTTPREQ_PUT:
    /* known request body size or -1 */
    എങ്കിൽ(data->state.infilesize != -1)
      stream->upload_ഇടത് = data->state.infilesize;
    അല്ലെങ്കിൽ
      /* data sending without specifying the data amount up front */
      stream->upload_ഇടത് = -1; /* unknown */
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    /* there is no request body */
    stream->upload_ഇടത് = 0; /* no request body */
    നിർത്തുക;
  }

  stream->send_closed = (stream->upload_ഇടത് == 0);
  എങ്കിൽ(!stream->send_closed) {
    reader.read_data = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, stream->id,
                                   nva, nheader, preader, stream);
  എങ്കിൽ(rc) {
    തിരഞ്ഞെടുപ്പ്(rc) {
    അവസ്ഥ NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", stream->id, rc, nghttp3_strerror(rc));
      നിർത്തുക;
    }
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = CURLE_SEND_ERROR;
    പോകുക out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  എങ്കിൽ(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    വേണ്ടി(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", stream->id,
            (പൂർണ്ണസംഖ്യ)nva[i].namelen, nva[i].name,
            (പൂർണ്ണസംഖ്യ)nva[i].valuelen, nva[i].മൂല്യം);
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_ngtcp2_send(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                               സ്ഥിരം uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  ഘടന cf_call_data save;
  ഘടന cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  എങ്കിൽ(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    പോകുക denied;
  }

  (ശൂന്യം)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  എങ്കിൽ(result)
    പോകുക out;

  എങ്കിൽ(!stream || stream->id < 0) {
    എങ്കിൽ(ctx->shutdown_started) {
      CURL_TRC_CF(data, cf, "cannot open stream on closed connection");
      result = CURLE_SEND_ERROR;
      പോകുക out;
    }
    result = h3_stream_open(cf, data, buf, len, pnwritten);
    എങ്കിൽ(result) {
      CURL_TRC_CF(data, cf, "failed to open stream -> %d", (പൂർണ്ണസംഖ്യ)result);
      പോകുക out;
    }
    stream = H3_STREAM_CTX(ctx, data);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = stream->xfer_result;
    പോകുക out;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(stream->closed) {
    എങ്കിൽ(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      പോകുക out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    പോകുക out;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ctx->shutdown_started) {
    CURL_TRC_CF(data, cf, "cannot send on closed connection");
    result = CURLE_SEND_ERROR;
    പോകുക out;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(stream->tx_in_flight_size >= stream->tx_in_flight_ideal) {
    result = CURLE_AGAIN;
    പോകുക out;
  }
  അല്ലെങ്കിൽ {
    result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (പൂർണ്ണസംഖ്യ)result, *pnwritten);
    എങ്കിൽ(result)
      പോകുക out;
    (ശൂന്യം)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  എങ്കിൽ(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

  എങ്കിൽ(*pnwritten > 0) {
    സ്ഥിരം size_t delta = (2 * 1024);
    എങ്കിൽ(stream->tx_in_flight_size > (stream->tx_in_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      stream->tx_in_flight_ideal += delta;
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  stream->id, len, *pnwritten,
                  stream->tx_in_flight_size, stream->tx_in_flight_ideal);
    }
    അല്ലെങ്കിൽ എങ്കിൽ((stream->tx_in_flight_size + delta) < stream->tx_in_flight_ideal) {
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
  എങ്കിൽ(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (പൂർണ്ണസംഖ്യ)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം CURLcode h3_data_pause(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (ശൂന്യം)cf;
  എങ്കിൽ(!pause)
    Curl_multi_mark_dirty(data);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_ngtcp2_cntrl(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ഘടന cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  തിരഞ്ഞെടുപ്പ്(event) {
  അവസ്ഥ CF_CTRL_DATA_SETUP:
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_DATA_DONE:
    Curl_cf_ngtcp2_h3_stream_done(cf, data);
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_DATA_DONE_SEND: {
    ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    എങ്കിൽ(stream && !stream->send_closed) {
      stream->send_closed = TRUE;
      stream->upload_ഇടത് = Curl_bufq_len(&stream->sendbuf) -
        stream->tx_in_flight_size;
      (ശൂന്യം)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
    }
    നിർത്തുക;
  }
  അവസ്ഥ CF_CTRL_CONN_INFO_UPDATE:
    എങ്കിൽ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    result = Curl_cf_ngtcp2_cmn_cntrl(cf, data, event, arg1, arg2);
    നിർത്തുക;
  }
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_ngtcp2_destroy(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  എങ്കിൽ(ctx) {
    എങ്കിൽ(ctx->qconn) {
      ഘടന cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = ശൂന്യ;
  }
}

സ്ഥിതം CURLcode cf_ngtcp2_connect(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  bool *done)
{
  തിരികെ Curl_cf_ngtcp2_cmn_connect(cf, data, done);
}

സ്ഥിതം CURLcode cf_ngtcp2_query(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  ഘടന cf_ngtcp2_ctx *ctx = cf->ctx;
  ഘടന cf_call_data save;

  തിരഞ്ഞെടുപ്പ്(query) {
  അവസ്ഥ CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_DATA_SAVE(save, cf, data);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    എങ്കിൽ(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      എങ്കിൽ(ctx->max_bidi_streams > ctx->used_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->used_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > INT_MAX) ? INT_MAX : (പൂർണ്ണസംഖ്യ)max_streams;
    }
    അല്ലെങ്കിൽ {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(data->multi);
      *pres1 = (n < INT_MAX) ? (പൂർണ്ണസംഖ്യ)n : INT_MAX;
    }
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_DATA_RESTORE(cf, save);
    തിരികെ CURLE_OK;
  }
  അവസ്ഥ CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    തിരികെ CURLE_OK;
  അവസ്ഥ CF_QUERY_SSL_INFO:
  അവസ്ഥ CF_QUERY_SSL_CTX_INFO: {
    ഘടന curl_tlssessioninfo *info = pres2;
    എങ്കിൽ(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      തിരികെ CURLE_OK;
    നിർത്തുക;
  }
  അവസ്ഥ CF_QUERY_ALPN_NEGOTIATED: {
    സ്ഥിരം അക്ഷരം **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ശൂന്യ;
    തിരികെ CURLE_OK;
  }
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ Curl_cf_ngtcp2_cmn_query(cf, data, query, pres1, pres2);
}

ഘടന Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_ngtcp2_create(ഘടന Curl_cfilter **pcf,
                               ഘടന Curl_easy *data,
                               ഘടന Curl_peer *origin,
                               ഘടന Curl_peer *peer,
                               ഘടന connectdata *conn,
                               ഘടന Curl_sockaddr_ex *addr)
{
  ഘടന cf_ngtcp2_ctx *ctx = ശൂന്യ;
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  CURLcode result;

  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  എങ്കിൽ(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  എങ്കിൽ(result)
    പോകുക out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ശൂന്യ, TRNSPRT_QUIC);
  എങ്കിൽ(result)
    പോകുക out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : ശൂന്യ;
  എങ്കിൽ(result) {
    എങ്കിൽ(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    അല്ലെങ്കിൽ എങ്കിൽ(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  തിരികെ result;
}

CURLcode Curl_cf_ngtcp2_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                     ഘടന Curl_peer *origin,
                                     ഘടന Curl_peer *peer,
                                     ഘടന Curl_cfilter **pcf)
{
  ഘടന cf_ngtcp2_ctx *ctx = ശൂന്യ;
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  CURLcode result;

  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  എങ്കിൽ(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  എങ്കിൽ(result)
    പോകുക out;
  Curl_conn_cf_തിരുകുക_after(cf_at, cf);
out:
  എങ്കിൽ(result) {
    curlx_safefree(cf);
    എങ്കിൽ(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  തിരികെ result;
}

#endif
