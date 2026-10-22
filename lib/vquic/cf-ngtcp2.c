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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "u32_ptrset.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-socket.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "http1.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "vdns/cf-dns.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic_int.h"
#ಅನ್ತರ್ಭಾವ "vquic/cf-ngtcp2-cmn.h"
#ಅನ್ತರ್ಭಾವ "vquic/cf-ngtcp2.h"


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, ಶೂನ್ಯ *user_data,
                                ಶೂನ್ಯ *stream_user_data);

ಸ್ಥಿರ CURLcode cf_ngtcp2_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                         ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  ಯದಿ(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  ಯದಿ(!want_send && !Curl_bufq_is_empty(&ctx->q.sendbuf))
    want_send = TRUE;

  ಯದಿ(want_recv || want_send) {
    ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    ಸಂರಚನಾ cf_call_data save;
    bool c_exhaust, s_exhaust;

    CF_DATA_SAVE(save, cf, data);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_ಎಡ(ctx->qconn) ||
                              !ngtcp2_conn_get_max_data_ಎಡ(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                (stream->quic_flow_blocked ||
                 stream->tx_in_flight_size >= stream->tx_in_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_stream_close(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, ಶೂನ್ಯ *user_data,
                              ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ h3_stream_ctx *stream;
  ಸಂರಚನಾ Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream_id;

  /* we might be called by nghttp3 after we already cleaned up */
  ಯದಿ(!data || !stream)
    ಪ್ರತ್ಯಾಗಮ 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  ಯದಿ(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  ಅನ್ಯಥಾ {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  }
  Curl_multi_mark_dirty(data);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_xfer_write_resp_hd(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ h3_stream_ctx *stream,
                                  ಧ್ರುವ ವರ್ಣ *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ಯದಿ(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, buflen, eos);
    ಯದಿ(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (ಪೂರ್ಣಾಙ್ಕ)stream->xfer_result,
                  buflen);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_xfer_write_resp(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ h3_stream_ctx *stream,
                               ಧ್ರುವ ವರ್ಣ *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ಯದಿ(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp(data, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    ಯದಿ(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  stream->id, (ಪೂರ್ಣಾಙ್ಕ)stream->xfer_result, buflen);
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ngtcp2_upd_rx_win(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ h3_stream_ctx *stream)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_STREAM_WINDOW_SIZE_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  ಯದಿ(Curl_rlimit_active(&data->progress.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    ಯದಿ(!stream->rx_offset)
      ಪ್ರತ್ಯಾಗಮ;

    avail = Curl_rlimit_avail(&data->progress.dl.rlimit, ರಿಕ್ತ);
    ಯದಿ(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", stream->id, avail);
      ಪ್ರತ್ಯಾಗಮ;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_STREAM_WINDOW_SIZE_MAX);
  }

  ಯದಿ(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ;
  }
  cur_win = stream->rx_offset_max - stream->rx_offset;

  ಯದಿ(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    ಯದಿ(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    ಯದಿ(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", stream->id, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
    }
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                           ಧ್ರುವ uint8_t *buf, size_t buflen,
                           ಶೂನ್ಯ *user_data, ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream;
  ಸಂರಚನಾ Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);

  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream3_id;

  ಯದಿ(!stream || !data)
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, data, stream, (ಧ್ರುವ ವರ್ಣ *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_offset(ctx->qconn, buflen);
  stream->rx_offset += buflen;
  ಯದಿ(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_ngtcp2_upd_rx_win(cf, data, stream);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, ಶೂನ್ಯ *user_data,
                                  ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = stream_user_data;
  (ಶೂನ್ಯ)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  ಯದಿ(stream) {
    stream->rx_offset += consumed;
    stream->rx_offset_max += consumed;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_end_headers(nghttp3_conn *conn, int64_t stream_id,
                             ಪೂರ್ಣಾಙ್ಕ fin, ಶೂನ್ಯ *user_data, ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ h3_stream_ctx *stream;
  ಸಂರಚನಾ Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream_id;
  (ಶೂನ್ಯ)fin;
  (ಶೂನ್ಯ)cf;

  ಯದಿ(!stream || !data)
    ಪ್ರತ್ಯಾಗಮ 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                        (bool)stream->closed);

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d",
              stream_id, stream->status_code);
  ಯದಿ(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(data);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_recv_header(nghttp3_conn *conn, int64_t stream_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *ಮೌಲ್ಯ, uint8_t flags,
                             ಶೂನ್ಯ *user_data, ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(ಮೌಲ್ಯ);
  ಸಂರಚನಾ h3_stream_ctx *stream;
  ಸಂರಚನಾ Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream_id;
  (ಶೂನ್ಯ)token;
  (ಶೂನ್ಯ)flags;
  (ಶೂನ್ಯ)cf;

  /* we might have cleaned up this transfer already */
  ಯದಿ(!stream || !data)
    ಪ್ರತ್ಯಾಗಮ 0;

  ಯದಿ(token == NGHTTP3_QPACK_TOKEN__STATUS) {

    result = Curl_http_decode_status(&stream->status_code,
                                     (ಧ್ರುವ ವರ್ಣ *)h3val.base, h3val.len);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    ಯದಿ(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (ಧ್ರುವ ವರ್ಣ *)h3val.base, h3val.len);
    ಯದಿ(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    ಯದಿ(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] status: %s",
                stream_id, curlx_dyn_ptr(&ctx->scratch));
    ಯದಿ(result) {
      ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ಅನ್ಯಥಾ {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s",
                stream_id, (ಪೂರ್ಣಾಙ್ಕ)h3name.len, h3name.base,
                (ಪೂರ್ಣಾಙ್ಕ)h3val.len, h3val.base);
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch,
                            (ಧ್ರುವ ವರ್ಣ *)h3name.base, h3name.len);
    ಯದಿ(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    ಯದಿ(!result)
      result = curlx_dyn_addn(&ctx->scratch,
                              (ಧ್ರುವ ವರ್ಣ *)h3val.base, h3val.len);
    ಯದಿ(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    ಯದಿ(!result)
      h3_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, ಶೂನ್ಯ *user_data,
                              ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಙ್ಕ rv;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream_user_data;

  rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                        app_error_code);
  ಯದಿ(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, ಶೂನ್ಯ *user_data,
                              ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, ರಿಕ್ತ);
  ಪೂರ್ಣಾಙ್ಕ rv;
  (ಶೂನ್ಯ)conn;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  ಯದಿ(data)
    CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  ಯದಿ(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
ಸ್ಥಿರ nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_stream_close,
  cb_h3_recv_data,
  cb_h3_deferred_consume,
  ರಿಕ್ತ, /* begin_headers */
  cb_h3_recv_header,
  cb_h3_end_headers,
  ರಿಕ್ತ, /* begin_trailers */
  cb_h3_recv_header,
  ರಿಕ್ತ, /* end_trailers */
  cb_h3_stop_sending,
  ರಿಕ್ತ, /* end_stream */
  cb_h3_reset_stream,
  ರಿಕ್ತ, /* shutdown */
  ರಿಕ್ತ, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  ರಿಕ್ತ, /* recv_origin */
  ರಿಕ್ತ, /* end_origin */
  ರಿಕ್ತ, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  ರಿಕ್ತ, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  ರಿಕ್ತ, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ತೆಗೆ
#endif

ಸ್ಥಿರ CURLcode init_ngh3_conn(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ cf_ngtcp2_ctx *ctx)
{
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(ngtcp2_conn_get_streams_uni_ಎಡ(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  ಯದಿ(rc) {
    failf(data, "error creating nghttp3 connection instance");
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಪ್ರತ್ಯಾಗಮ Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

ಸ್ಥಿರ CURLcode recv_closed_stream(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ h3_stream_ctx *stream,
                                   size_t *pnread)
{
  (ಶೂನ್ಯ)cf;
  *pnread = 0;
  ಯದಿ(stream->reset) {
    ಯದಿ(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ಅನ್ಯಥಾ ಯದಿ(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    ಪ್ರತ್ಯಾಗಮ data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  ಅನ್ಯಥಾ ಯದಿ(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    ಪ್ರತ್ಯಾಗಮ CURLE_HTTP3;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* incoming data frames on the h3 stream */
ಸ್ಥಿರ CURLcode cf_ngtcp2_recv(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                               ವರ್ಣ *buf, size_t buflen, size_t *pnread)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  ಸಂರಚನಾ cf_call_data save;
  ಸಂರಚನಾ cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ i;

  (ಶೂನ್ಯ)ctx;
  (ಶೂನ್ಯ)buf;
  NOVERBOSE((ಶೂನ್ಯ)buflen);

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  ಯದಿ(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    ಗಚ್ಛ denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  ಯದಿ(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    ಗಚ್ಛ out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  ಪ್ರತಿ(i = 0; i < 2; ++i) {
    ಯದಿ(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(stream->closed) {
      result = recv_closed_stream(cf, data, stream, pnread);
      ಗಚ್ಛ out;
    }

    ಯದಿ(!i && Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx)) {
      result = CURLE_RECV_ERROR;
      ಗಚ್ಛ out;
    }
  }

  result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_progress_egress(cf, data, &pktx));
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
  ಯದಿ(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              stream ? stream->id : -1, buflen, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, ಶೂನ್ಯ *user_data,
                                ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ h3_stream_ctx *stream = stream_user_data;
  size_t skiplen;

  (ಶೂನ್ಯ)cf;
  ಯದಿ(!stream)
    ಪ್ರತ್ಯಾಗಮ 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  ಯದಿ(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  ಅನ್ಯಥಾ
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  ಯದಿ(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    ಪೂರ್ಣಾಙ್ಕ rv = nghttp3_conn_resume_stream(conn, stream_id);
    ಯದಿ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t stream_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, ಶೂನ್ಯ *user_data,
                                         ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ h3_stream_ctx *stream;
  ಸಂರಚನಾ Curl_easy *data =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_data, &stream);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream_id;
  (ಶೂನ್ಯ)user_data;
  (ಶೂನ್ಯ)veccnt;

  ಯದಿ(!stream || !data)
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  ಯದಿ(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    ಯಾವತ್(nvecs < veccnt &&
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

  ಯದಿ(nwritten > 0 && stream->upload_ಎಡ != -1)
    stream->upload_ಎಡ -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  ಯದಿ(stream->upload_ಎಡ == 0) {
    *pflags = NGHTTP3_DATA_FLAG_EOF;
    stream->send_closed = TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN", stream->id);
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf),
              stream->upload_ಎಡ);
  ಪ್ರತ್ಯಾಗಮ (nghttp3_ssize)nvecs;
}

ಸ್ಥಿರ CURLcode h3_stream_open(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಧ್ರುವ uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = ರಿಕ್ತ;
  int64_t sid;
  ಸಂರಚನಾ dynhds h2_headers;
  size_t nheader;
  nghttp3_nv *nva = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ rc = 0;
  unsigned ಪೂರ್ಣಾಙ್ಕ i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = ರಿಕ್ತ;
  CURLcode result;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = Curl_h1_req_parse_read(&stream->h1, buf, len, ರಿಕ್ತ,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : ರಿಕ್ತ,
    0, pnwritten);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ಯದಿ(!stream->h1.done) {
    /* need more data */
    ಗಚ್ಛ out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_ಎಣಿಕೆ(&h2_headers);
  nva = curlx_malloc(ಪರಿಮಾಣ(nghttp3_nv) * nheader);
  ಯದಿ(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  ಪ್ರತಿ(i = 0; i < nheader; ++i) {
    ಸಂರಚನಾ dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned ವರ್ಣ *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].ಮೌಲ್ಯ = (unsigned ವರ್ಣ *)e->ಮೌಲ್ಯ;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, stream);
  ಯದಿ(rc) {
    failf(data, "cannot open bidi streams");
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ out;
  }
  stream->id = sid;
  ++ctx->used_bidi_streams;

  ವಿಕಲ್ಪ(data->state.httpreq) {
  ಪಕ್ಷ HTTPREQ_POST:
  ಪಕ್ಷ HTTPREQ_POST_FORM:
  ಪಕ್ಷ HTTPREQ_POST_MIME:
  ಪಕ್ಷ HTTPREQ_PUT:
    /* known request body size or -1 */
    ಯದಿ(data->state.infilesize != -1)
      stream->upload_ಎಡ = data->state.infilesize;
    ಅನ್ಯಥಾ
      /* data sending without specifying the data amount up front */
      stream->upload_ಎಡ = -1; /* unknown */
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    /* there is no request body */
    stream->upload_ಎಡ = 0; /* no request body */
    ಭಙ್ಗ;
  }

  stream->send_closed = (stream->upload_ಎಡ == 0);
  ಯದಿ(!stream->send_closed) {
    reader.read_data = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, stream->id,
                                   nva, nheader, preader, stream);
  ಯದಿ(rc) {
    ವಿಕಲ್ಪ(rc) {
    ಪಕ್ಷ NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", stream->id, rc, nghttp3_strerror(rc));
      ಭಙ್ಗ;
    }
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ out;
  }

  cf_ngtcp2_upd_rx_win(cf, data, stream);

  ಯದಿ(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    ಪ್ರತಿ(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", stream->id,
            (ಪೂರ್ಣಾಙ್ಕ)nva[i].namelen, nva[i].name,
            (ಪೂರ್ಣಾಙ್ಕ)nva[i].valuelen, nva[i].ಮೌಲ್ಯ);
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_ngtcp2_send(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                               ಧ್ರುವ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  ಸಂರಚನಾ cf_call_data save;
  ಸಂರಚನಾ cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  ಯದಿ(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    ಗಚ್ಛ denied;
  }

  (ಶೂನ್ಯ)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ಯದಿ(!stream || stream->id < 0) {
    ಯದಿ(ctx->shutdown_started) {
      CURL_TRC_CF(data, cf, "cannot open stream on closed connection");
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
    result = h3_stream_open(cf, data, buf, len, pnwritten);
    ಯದಿ(result) {
      CURL_TRC_CF(data, cf, "failed to open stream -> %d", (ಪೂರ್ಣಾಙ್ಕ)result);
      ಗಚ್ಛ out;
    }
    stream = H3_STREAM_CTX(ctx, data);
  }
  ಅನ್ಯಥಾ ಯದಿ(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
    result = stream->xfer_result;
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ ಯದಿ(stream->closed) {
    ಯದಿ(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      ಗಚ್ಛ out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ ಯದಿ(ctx->shutdown_started) {
    CURL_TRC_CF(data, cf, "cannot send on closed connection");
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ ಯದಿ(stream->tx_in_flight_size >= stream->tx_in_flight_ideal) {
    result = CURLE_AGAIN;
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ {
    result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    (ಶೂನ್ಯ)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  ಯದಿ(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

  ಯದಿ(*pnwritten > 0) {
    ಧ್ರುವ size_t delta = (2 * 1024);
    ಯದಿ(stream->tx_in_flight_size > (stream->tx_in_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      stream->tx_in_flight_ideal += delta;
      CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  stream->id, len, *pnwritten,
                  stream->tx_in_flight_size, stream->tx_in_flight_ideal);
    }
    ಅನ್ಯಥಾ ಯದಿ((stream->tx_in_flight_size + delta) < stream->tx_in_flight_ideal) {
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
  ಯದಿ(ctx->tls_vrfy_result)
    result = ctx->tls_vrfy_result;
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode h3_data_pause(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (ಶೂನ್ಯ)cf;
  ಯದಿ(!pause)
    Curl_multi_mark_dirty(data);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_ngtcp2_cntrl(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  ವಿಕಲ್ಪ(event) {
  ಪಕ್ಷ CF_CTRL_DATA_SETUP:
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_DATA_DONE:
    Curl_cf_ngtcp2_h3_stream_done(cf, data);
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_DATA_DONE_SEND: {
    ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    ಯದಿ(stream && !stream->send_closed) {
      stream->send_closed = TRUE;
      stream->upload_ಎಡ = Curl_bufq_len(&stream->sendbuf) -
        stream->tx_in_flight_size;
      (ಶೂನ್ಯ)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
    }
    ಭಙ್ಗ;
  }
  ಪಕ್ಷ CF_CTRL_CONN_INFO_UPDATE:
    ಯದಿ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    result = Curl_cf_ngtcp2_cmn_cntrl(cf, data, event, arg1, arg2);
    ಭಙ್ಗ;
  }
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ngtcp2_destroy(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  ಯದಿ(ctx) {
    ಯದಿ(ctx->qconn) {
      ಸಂರಚನಾ cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = ರಿಕ್ತ;
  }
}

ಸ್ಥಿರ CURLcode cf_ngtcp2_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  bool *done)
{
  ಪ್ರತ್ಯಾಗಮ Curl_cf_ngtcp2_cmn_connect(cf, data, done);
}

ಸ್ಥಿರ CURLcode cf_ngtcp2_query(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;

  ವಿಕಲ್ಪ(query) {
  ಪಕ್ಷ CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_DATA_SAVE(save, cf, data);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    ಯದಿ(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      ಯದಿ(ctx->max_bidi_streams > ctx->used_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->used_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > INT_MAX) ? INT_MAX : (ಪೂರ್ಣಾಙ್ಕ)max_streams;
    }
    ಅನ್ಯಥಾ {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(data->multi);
      *pres1 = (n < INT_MAX) ? (ಪೂರ್ಣಾಙ್ಕ)n : INT_MAX;
    }
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_DATA_RESTORE(cf, save);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಪಕ್ಷ CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CF_QUERY_SSL_INFO:
  ಪಕ್ಷ CF_QUERY_SSL_CTX_INFO: {
    ಸಂರಚನಾ curl_tlssessioninfo *info = pres2;
    ಯದಿ(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    ಭಙ್ಗ;
  }
  ಪಕ್ಷ CF_QUERY_ALPN_NEGOTIATED: {
    ಧ್ರುವ ವರ್ಣ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ Curl_cf_ngtcp2_cmn_query(cf, data, query, pres1, pres2);
}

ಸಂರಚನಾ Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_ngtcp2_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_peer *origin,
                               ಸಂರಚನಾ Curl_peer *peer,
                               ಸಂರಚನಾ connectdata *conn,
                               ಸಂರಚನಾ Curl_sockaddr_ex *addr)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  CURLcode result;

  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  ಯದಿ(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ರಿಕ್ತ, TRNSPRT_QUIC);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : ರಿಕ್ತ;
  ಯದಿ(result) {
    ಯದಿ(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    ಅನ್ಯಥಾ ಯದಿ(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_ngtcp2_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                     ಸಂರಚನಾ Curl_peer *origin,
                                     ಸಂರಚನಾ Curl_peer *peer,
                                     ಸಂರಚನಾ Curl_cfilter **pcf)
{
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  CURLcode result;

  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  result = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  ಯದಿ(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
out:
  ಯದಿ(result) {
    curlx_safefree(cf);
    ಯದಿ(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif
