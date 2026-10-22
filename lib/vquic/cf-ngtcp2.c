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


স্থির পূর্ণসংখ্যা cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, শূন্য *user_তথ্য,
                                শূন্য *stream_user_তথ্য);

স্থির CURLcode cf_ngtcp2_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                         কাঠামো Curl_easy *তথ্য,
                                         কাঠামো easy_pollset *ps)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode ফলাফল = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, তথ্য);

  যদি(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    ফেরত CURLE_OK;

  Curl_pollset_check(তথ্য, ps, sock, &want_recv, &want_send);
  যদি(!want_send && !Curl_bufq_is_empty(&ctx->q.sendbuf))
    want_send = TRUE;

  যদি(want_recv || want_send) {
    কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);
    কাঠামো cf_call_তথ্য save;
    bool c_exhaust, s_exhaust;

    CF_তথ্য_SAVE(save, cf, তথ্য);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_বাম(ctx->qconn) ||
                              !ngtcp2_conn_get_max_তথ্য_বাম(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                (stream->quic_flow_blocked ||
                 stream->tx_in_flight_আকার >= stream->tx_in_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    ফলাফল = Curl_pollset_set(তথ্য, ps, sock, want_recv, want_send);
    CF_তথ্য_RESTORE(cf, save);
  }
  ফেরত ফলাফল;
}

স্থির পূর্ণসংখ্যা cb_h3_stream_close(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, শূন্য *user_তথ্য,
                              শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো h3_stream_ctx *stream;
  কাঠামো Curl_easy *তথ্য =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_তথ্য, &stream);
  (শূন্য)conn;
  (শূন্য)stream_id;

  /* we might be called by nghttp3 after we already cleaned up */
  যদি(!তথ্য || !stream)
    ফেরত 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  যদি(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  অন্যথা {
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] CLOSED", stream->id);
  }
  Curl_multi_mark_dirty(তথ্য);
  ফেরত 0;
}

স্থির শূন্য h3_xfer_write_resp_hd(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো h3_stream_ctx *stream,
                                  ধ্রুবক অক্ষর *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  যদি(!stream->xfer_ফলাফল) {
    stream->xfer_ফলাফল = Curl_xfer_write_resp_hd(তথ্য, buf, buflen, eos);
    যদি(stream->xfer_ফলাফল)
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (পূর্ণসংখ্যা)stream->xfer_ফলাফল,
                  buflen);
  }
}

স্থির শূন্য h3_xfer_write_resp(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো h3_stream_ctx *stream,
                               ধ্রুবক অক্ষর *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  যদি(!stream->xfer_ফলাফল) {
    stream->xfer_ফলাফল = Curl_xfer_write_resp(তথ্য, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    যদি(stream->xfer_ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  stream->id, (পূর্ণসংখ্যা)stream->xfer_ফলাফল, buflen);
    }
  }
}

স্থির শূন্য cf_ngtcp2_upd_rx_win(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো h3_stream_ctx *stream)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_STREAM_WINDOW_আকার_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  যদি(Curl_rlimit_active(&তথ্য->progress.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    যদি(!stream->rx_offset)
      ফেরত;

    avail = Curl_rlimit_avail(&তথ্য->progress.dl.rlimit, ফাঁকা);
    যদি(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", stream->id, avail);
      ফেরত;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_STREAM_WINDOW_আকার_MAX);
  }

  যদি(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    ফেরত;
  }
  cur_win = stream->rx_offset_max - stream->rx_offset;

  যদি(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    যদি(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    যদি(delta) {
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", stream->id, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
    }
  }
}

স্থির পূর্ণসংখ্যা cb_h3_recv_তথ্য(nghttp3_conn *conn, int64_t stream3_id,
                           ধ্রুবক uint8_t *buf, size_t buflen,
                           শূন্য *user_তথ্য, শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream;
  কাঠামো Curl_easy *তথ্য =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_তথ্য, &stream);

  (শূন্য)conn;
  (শূন্য)stream3_id;

  যদি(!stream || !তথ্য)
    ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, তথ্য, stream, (ধ্রুবক অক্ষর *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_offset(ctx->qconn, buflen);
  stream->rx_offset += buflen;
  যদি(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_ngtcp2_upd_rx_win(cf, তথ্য, stream);
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, শূন্য *user_তথ্য,
                                  শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = stream_user_তথ্য;
  (শূন্য)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  যদি(stream) {
    stream->rx_offset += consumed;
    stream->rx_offset_max += consumed;
  }
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_h3_end_headers(nghttp3_conn *conn, int64_t stream_id,
                             পূর্ণসংখ্যা fin, শূন্য *user_তথ্য, শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো h3_stream_ctx *stream;
  কাঠামো Curl_easy *তথ্য =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_তথ্য, &stream);
  (শূন্য)conn;
  (শূন্য)stream_id;
  (শূন্য)fin;
  (শূন্য)cf;

  যদি(!stream || !তথ্য)
    ফেরত 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, তথ্য, stream, STRCONST("\r\n"),
                        (bool)stream->closed);

  CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] end_headers, status=%d",
              stream_id, stream->status_code);
  যদি(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(তথ্য);
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_h3_recv_header(nghttp3_conn *conn, int64_t stream_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *মান, uint8_t flags,
                             শূন্য *user_তথ্য, শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(মান);
  কাঠামো h3_stream_ctx *stream;
  কাঠামো Curl_easy *তথ্য =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_তথ্য, &stream);
  CURLcode ফলাফল = CURLE_OK;
  (শূন্য)conn;
  (শূন্য)stream_id;
  (শূন্য)token;
  (শূন্য)flags;
  (শূন্য)cf;

  /* we might have cleaned up this transfer already */
  যদি(!stream || !তথ্য)
    ফেরত 0;

  যদি(token == NGHTTP3_QPACK_TOKEN__STATUS) {

    ফলাফল = Curl_http_decode_status(&stream->status_code,
                                     (ধ্রুবক অক্ষর *)h3val.base, h3val.len);
    যদি(ফলাফল)
      ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    ফলাফল = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(&ctx->scratch,
                              (ধ্রুবক অক্ষর *)h3val.base, h3val.len);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    যদি(!ফলাফল)
      h3_xfer_write_resp_hd(cf, তথ্য, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] status: %s",
                stream_id, curlx_dyn_ptr(&ctx->scratch));
    যদি(ফলাফল) {
      ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  অন্যথা {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] header: %.*s: %.*s",
                stream_id, (পূর্ণসংখ্যা)h3name.len, h3name.base,
                (পূর্ণসংখ্যা)h3val.len, h3val.base);
    curlx_dyn_reset(&ctx->scratch);
    ফলাফল = curlx_dyn_addn(&ctx->scratch,
                            (ধ্রুবক অক্ষর *)h3name.base, h3name.len);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(&ctx->scratch,
                              (ধ্রুবক অক্ষর *)h3val.base, h3val.len);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    যদি(!ফলাফল)
      h3_xfer_write_resp_hd(cf, তথ্য, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_h3_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, শূন্য *user_তথ্য,
                              শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  পূর্ণসংখ্যা rv;
  (শূন্য)conn;
  (শূন্য)stream_user_তথ্য;

  rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                        app_error_code);
  যদি(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_h3_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, শূন্য *user_তথ্য,
                              শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = Curl_cf_ngtcp2_get_xfer(cf, stream_user_তথ্য, ফাঁকা);
  পূর্ণসংখ্যা rv;
  (শূন্য)conn;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  যদি(তথ্য)
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  যদি(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  ফেরত 0;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
স্থির nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_stream_close,
  cb_h3_recv_তথ্য,
  cb_h3_deferred_consume,
  ফাঁকা, /* begin_headers */
  cb_h3_recv_header,
  cb_h3_end_headers,
  ফাঁকা, /* begin_trailers */
  cb_h3_recv_header,
  ফাঁকা, /* end_trailers */
  cb_h3_stop_sending,
  ফাঁকা, /* end_stream */
  cb_h3_reset_stream,
  ফাঁকা, /* shutdown */
  ফাঁকা, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  ফাঁকা, /* recv_origin */
  ফাঁকা, /* end_origin */
  ফাঁকা, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  ফাঁকা, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  ফাঁকা, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic বের_করো
#endif

স্থির CURLcode init_ngh3_conn(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো cf_ngtcp2_ctx *ctx)
{
  পূর্ণসংখ্যা rc;

  যদি(ngtcp2_conn_get_streams_uni_বাম(ctx->qconn) < 3) {
    failf(তথ্য, "QUIC connection lacks 3 uni streams to run HTTP/3");
    ফেরত CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  যদি(rc) {
    failf(তথ্য, "error creating nghttp3 connection instance");
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  ফেরত Curl_cf_ngtcp2_h3_init_ctrls(ctx, তথ্য);
}

স্থির CURLcode recv_closed_stream(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো h3_stream_ctx *stream,
                                   size_t *pnread)
{
  (শূন্য)cf;
  *pnread = 0;
  যদি(stream->reset) {
    যদি(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(তথ্য, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      তথ্য->state.refused_stream = TRUE;
      ফেরত CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    অন্যথা যদি(stream->resp_hds_complete && তথ্য->req.no_body) {
        CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        ফেরত CURLE_OK;
    }
    failf(তথ্য, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    ফেরত তথ্য->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  অন্যথা যদি(!stream->resp_hds_complete) {
    failf(তথ্য,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    ফেরত CURLE_HTTP3;
  }
  ফেরত CURLE_OK;
}

/* incoming data frames on the h3 stream */
স্থির CURLcode cf_ngtcp2_recv(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                               অক্ষর *buf, size_t buflen, size_t *pnread)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);
  কাঠামো cf_call_তথ্য save;
  কাঠামো cf_ngtcp2_io_ctx pktx;
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা i;

  (শূন্য)ctx;
  (শূন্য)buf;
  NOVERBOSE((শূন্য)buflen);

  CF_তথ্য_SAVE(save, cf, তথ্য);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  যদি(ctx->tls_vrfy_ফলাফল) {
    ফলাফল = ctx->tls_vrfy_ফলাফল;
    যাও denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, তথ্য);

  যদি(!stream || ctx->shutdown_started) {
    ফলাফল = CURLE_RECV_ERROR;
    যাও out;
  }

  cf_ngtcp2_upd_rx_win(cf, তথ্য, stream);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  জন্য(i = 0; i < 2; ++i) {
    যদি(stream->xfer_ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, তথ্য, stream);
      ফলাফল = stream->xfer_ফলাফল;
      যাও out;
    }
    অন্যথা যদি(stream->closed) {
      ফলাফল = recv_closed_stream(cf, তথ্য, stream, pnread);
      যাও out;
    }

    যদি(!i && Curl_cf_ngtcp2_progress_ingress(cf, তথ্য, &pktx)) {
      ফলাফল = CURLE_RECV_ERROR;
      যাও out;
    }
  }

  ফলাফল = CURLE_AGAIN;

out:
  ফলাফল = Curl_1st_fatal(ফলাফল,
                          Curl_cf_ngtcp2_progress_egress(cf, তথ্য, &pktx));
  ফলাফল = Curl_1st_fatal(ফলাফল,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, তথ্য, &pktx));
  যদি(ctx->tls_vrfy_ফলাফল)
    ফলাফল = ctx->tls_vrfy_ফলাফল;
denied:
  CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              stream ? stream->id : -1, buflen, (পূর্ণসংখ্যা)ফলাফল, *pnread);
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির পূর্ণসংখ্যা cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, শূন্য *user_তথ্য,
                                শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো h3_stream_ctx *stream = stream_user_তথ্য;
  size_t skiplen;

  (শূন্য)cf;
  যদি(!stream)
    ফেরত 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  যদি(datalen >= (uint64_t)stream->tx_in_flight_আকার)
    skiplen = stream->tx_in_flight_আকার;
  অন্যথা
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_আকার -= skiplen;

  /* Resume upload processing if we have more data to send */
  যদি(stream->tx_in_flight_আকার < Curl_bufq_len(&stream->sendbuf)) {
    পূর্ণসংখ্যা rv = nghttp3_conn_resume_stream(conn, stream_id);
    যদি(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ফেরত 0;
}

স্থির nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t stream_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, শূন্য *user_তথ্য,
                                         শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো h3_stream_ctx *stream;
  কাঠামো Curl_easy *তথ্য =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_তথ্য, &stream);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (শূন্য)cf;
  (শূন্য)conn;
  (শূন্য)stream_id;
  (শূন্য)user_তথ্য;
  (শূন্য)veccnt;

  যদি(!stream || !তথ্য)
    ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  যদি(stream->tx_in_flight_আকার < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    যখন(nvecs < veccnt &&
          Curl_bufq_peek_at(&stream->sendbuf,
                            stream->tx_in_flight_আকার,
                            CURL_UNCONST(&vec[nvecs].base),
                            &vec[nvecs].len)) {
      stream->tx_in_flight_আকার += vec[nvecs].len;
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
    *pflags = NGHTTP3_তথ্য_FLAG_EOF;
    stream->send_closed = TRUE;
  }
  অন্যথা যদি(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] read req body -> AGAIN", stream->id);
    ফেরত NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              stream->id, nvecs,
              *pflags == NGHTTP3_তথ্য_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf),
              stream->upload_বাম);
  ফেরত (nghttp3_ssize)nvecs;
}

স্থির CURLcode h3_stream_open(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               ধ্রুবক uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = ফাঁকা;
  int64_t sid;
  কাঠামো dynhds h2_headers;
  size_t nheader;
  nghttp3_nv *nva = ফাঁকা;
  পূর্ণসংখ্যা rc = 0;
  unsigned পূর্ণসংখ্যা i;
  nghttp3_তথ্য_reader reader;
  nghttp3_তথ্য_reader *preader = ফাঁকা;
  CURLcode ফলাফল;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  ফলাফল = Curl_cf_ngtcp2_h3_stream_setup(cf, তথ্য, &stream);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = Curl_h1_req_parse_read(&stream->h1, buf, len, ফাঁকা,
    !তথ্য->state.http_ignorecustom ?
    CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST) : ফাঁকা,
    0, pnwritten);
  যদি(ফলাফল)
    যাও out;
  যদি(!stream->h1.done) {
    /* need more data */
    যাও out;
  }
  DEBUGASSERT(stream->h1.req);

  ফলাফল = Curl_http_req_to_h2(&h2_headers, stream->h1.req, তথ্য);
  যদি(ফলাফল)
    যাও out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_গুনতি(&h2_headers);
  nva = curlx_malloc(আকার(nghttp3_nv) * nheader);
  যদি(!nva) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  জন্য(i = 0; i < nheader; ++i) {
    কাঠামো dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned অক্ষর *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].মান = (unsigned অক্ষর *)e->মান;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, stream);
  যদি(rc) {
    failf(তথ্য, "cannot open bidi streams");
    ফলাফল = CURLE_SEND_ERROR;
    যাও out;
  }
  stream->id = sid;
  ++ctx->used_bidi_streams;

  নির্বাচন(তথ্য->state.httpreq) {
  ক্ষেত্র HTTPREQ_POST:
  ক্ষেত্র HTTPREQ_POST_FORM:
  ক্ষেত্র HTTPREQ_POST_MIME:
  ক্ষেত্র HTTPREQ_PUT:
    /* known request body size or -1 */
    যদি(তথ্য->state.infilesize != -1)
      stream->upload_বাম = তথ্য->state.infilesize;
    অন্যথা
      /* data sending without specifying the data amount up front */
      stream->upload_বাম = -1; /* unknown */
    বিরতি;
  পূর্বনির্ধারিত:
    /* there is no request body */
    stream->upload_বাম = 0; /* no request body */
    বিরতি;
  }

  stream->send_closed = (stream->upload_বাম == 0);
  যদি(!stream->send_closed) {
    reader.read_তথ্য = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, stream->id,
                                   nva, nheader, preader, stream);
  যদি(rc) {
    নির্বাচন(rc) {
    ক্ষেত্র NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(তথ্য, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      বিরতি;
    পূর্বনির্ধারিত:
      CURL_TRC_CF(তথ্য, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", stream->id, rc, nghttp3_strerror(rc));
      বিরতি;
    }
    Curl_cf_ngtcp2_h3_stream_close(cf, তথ্য, stream);
    ফলাফল = CURLE_SEND_ERROR;
    যাও out;
  }

  cf_ngtcp2_upd_rx_win(cf, তথ্য, stream);

  যদি(Curl_trc_is_verbose(তথ্য)) {
    infof(তথ্য, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&তথ্য->state.url));
    জন্য(i = 0; i < nheader; ++i) {
      infof(তথ্য, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", stream->id,
            (পূর্ণসংখ্যা)nva[i].namelen, nva[i].name,
            (পূর্ণসংখ্যা)nva[i].valuelen, nva[i].মান);
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_ngtcp2_send(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                               ধ্রুবক uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);
  কাঠামো cf_call_তথ্য save;
  কাঠামো cf_ngtcp2_io_ctx pktx;
  CURLcode ফলাফল = CURLE_OK;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, তথ্য);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  যদি(ctx->tls_vrfy_ফলাফল) {
    ফলাফল = ctx->tls_vrfy_ফলাফল;
    যাও denied;
  }

  (শূন্য)eos; /* use for stream EOF and block handling */
  ফলাফল = Curl_cf_ngtcp2_progress_ingress(cf, তথ্য, &pktx);
  যদি(ফলাফল)
    যাও out;

  যদি(!stream || stream->id < 0) {
    যদি(ctx->shutdown_started) {
      CURL_TRC_CF(তথ্য, cf, "cannot open stream on closed connection");
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
    ফলাফল = h3_stream_open(cf, তথ্য, buf, len, pnwritten);
    যদি(ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "failed to open stream -> %d", (পূর্ণসংখ্যা)ফলাফল);
      যাও out;
    }
    stream = H3_STREAM_CTX(ctx, তথ্য);
  }
  অন্যথা যদি(stream->xfer_ফলাফল) {
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] xfer write failed", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, তথ্য, stream);
    ফলাফল = stream->xfer_ফলাফল;
    যাও out;
  }
  অন্যথা যদি(stream->closed) {
    যদি(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      ফলাফল = CURLE_OK;
      *pnwritten = len;
      যাও out;
    }
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    ফলাফল = CURLE_HTTP3;
    যাও out;
  }
  অন্যথা যদি(ctx->shutdown_started) {
    CURL_TRC_CF(তথ্য, cf, "cannot send on closed connection");
    ফলাফল = CURLE_SEND_ERROR;
    যাও out;
  }
  অন্যথা যদি(stream->tx_in_flight_আকার >= stream->tx_in_flight_ideal) {
    ফলাফল = CURLE_AGAIN;
    যাও out;
  }
  অন্যথা {
    ফলাফল = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (পূর্ণসংখ্যা)ফলাফল, *pnwritten);
    যদি(ফলাফল)
      যাও out;
    (শূন্য)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  যদি(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!ফলাফল);
  ফলাফল = Curl_cf_ngtcp2_progress_egress(cf, তথ্য, &pktx);

  যদি(*pnwritten > 0) {
    ধ্রুবক size_t delta = (2 * 1024);
    যদি(stream->tx_in_flight_আকার > (stream->tx_in_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      stream->tx_in_flight_ideal += delta;
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  stream->id, len, *pnwritten,
                  stream->tx_in_flight_আকার, stream->tx_in_flight_ideal);
    }
    অন্যথা যদি((stream->tx_in_flight_আকার + delta) < stream->tx_in_flight_ideal) {
      /* ngtcp2 keeps less in flight than we try to provide for,
       * decrease our "ideal" tx buffer length */
      stream->tx_in_flight_ideal -= delta;
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, reduce tx_win=%zu",
                  stream->id, len, *pnwritten,
                  stream->tx_in_flight_আকার, stream->tx_in_flight_ideal);
    }
  }

out:
  ফলাফল = Curl_1st_fatal(ফলাফল,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, তথ্য, &pktx));
  যদি(ctx->tls_vrfy_ফলাফল)
    ফলাফল = ctx->tls_vrfy_ফলাফল;
denied:
  CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (পূর্ণসংখ্যা)ফলাফল, *pnwritten);
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির CURLcode h3_তথ্য_pause(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (শূন্য)cf;
  যদি(!pause)
    Curl_multi_mark_dirty(তথ্য);
  ফেরত CURLE_OK;
}

স্থির CURLcode cf_ngtcp2_cntrl(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো cf_call_তথ্য save;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  নির্বাচন(event) {
  ক্ষেত্র CF_CTRL_তথ্য_SETUP:
    বিরতি;
  ক্ষেত্র CF_CTRL_তথ্য_PAUSE:
    ফলাফল = h3_তথ্য_pause(cf, তথ্য, (arg1 != 0));
    বিরতি;
  ক্ষেত্র CF_CTRL_তথ্য_DONE:
    Curl_cf_ngtcp2_h3_stream_done(cf, তথ্য);
    বিরতি;
  ক্ষেত্র CF_CTRL_তথ্য_DONE_SEND: {
    কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);
    যদি(stream && !stream->send_closed) {
      stream->send_closed = TRUE;
      stream->upload_বাম = Curl_bufq_len(&stream->sendbuf) -
        stream->tx_in_flight_আকার;
      (শূন্য)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
    }
    বিরতি;
  }
  ক্ষেত্র CF_CTRL_CONN_INFO_UPDATE:
    যদি(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    বিরতি;
  পূর্বনির্ধারিত:
    ফলাফল = Curl_cf_ngtcp2_cmn_cntrl(cf, তথ্য, event, arg1, arg2);
    বিরতি;
  }
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির শূন্য cf_ngtcp2_destroy(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(তথ্য, cf, "destroy");
  যদি(ctx) {
    যদি(ctx->qconn) {
      কাঠামো cf_call_তথ্য save;
      CF_তথ্য_SAVE(save, cf, তথ্য);
      Curl_cf_ngtcp2_cmn_conn_close(cf, তথ্য);
      CF_তথ্য_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = ফাঁকা;
  }
}

স্থির CURLcode cf_ngtcp2_connect(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  bool *done)
{
  ফেরত Curl_cf_ngtcp2_cmn_connect(cf, তথ্য, done);
}

স্থির CURLcode cf_ngtcp2_query(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  কাঠামো cf_ngtcp2_ctx *ctx = cf->ctx;
  কাঠামো cf_call_তথ্য save;

  নির্বাচন(query) {
  ক্ষেত্র CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_তথ্য_SAVE(save, cf, তথ্য);
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
      *pres1 = (max_streams > INT_MAX) ? INT_MAX : (পূর্ণসংখ্যা)max_streams;
    }
    অন্যথা {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(তথ্য->multi);
      *pres1 = (n < INT_MAX) ? (পূর্ণসংখ্যা)n : INT_MAX;
    }
    CURL_TRC_CF(তথ্য, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_তথ্য_RESTORE(cf, save);
    ফেরত CURLE_OK;
  }
  ক্ষেত্র CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    ফেরত CURLE_OK;
  ক্ষেত্র CF_QUERY_SSL_INFO:
  ক্ষেত্র CF_QUERY_SSL_CTX_INFO: {
    কাঠামো curl_tlssessioninfo *info = pres2;
    যদি(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      ফেরত CURLE_OK;
    বিরতি;
  }
  ক্ষেত্র CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুবক অক্ষর **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ফাঁকা;
    ফেরত CURLE_OK;
  }
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত Curl_cf_ngtcp2_cmn_query(cf, তথ্য, query, pres1, pres2);
}

কাঠামো Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_ngtcp2_destroy,
  cf_ngtcp2_connect,
  Curl_cf_ngtcp2_cmn_shutdown,
  cf_ngtcp2_adjust_pollset,
  Curl_cf_def_তথ্য_pending,
  cf_ngtcp2_send,
  cf_ngtcp2_recv,
  cf_ngtcp2_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ngtcp2_query,
};

CURLcode Curl_cf_ngtcp2_create(কাঠামো Curl_cfilter **pcf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_peer *origin,
                               কাঠামো Curl_peer *peer,
                               কাঠামো connectdata *conn,
                               কাঠামো Curl_sockaddr_ex *addr)
{
  কাঠামো cf_ngtcp2_ctx *ctx = ফাঁকা;
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  CURLcode ফলাফল;

  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  ফলাফল = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  যদি(!ফলাফল)
    ফলাফল = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  যদি(ফলাফল)
    যাও out;
  cf->conn = conn;

  ফলাফল = Curl_cf_udp_create(&cf->পরবর্তী, তথ্য, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ফাঁকা, TRNSPRT_QUIC);
  যদি(ফলাফল)
    যাও out;
  cf->পরবর্তী->conn = cf->conn;
  cf->পরবর্তী->sockindex = cf->sockindex;

out:
  *pcf = (!ফলাফল) ? cf : ফাঁকা;
  যদি(ফলাফল) {
    যদি(cf)
      Curl_conn_cf_discard_chain(&cf, তথ্য);
    অন্যথা যদি(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  ফেরত ফলাফল;
}

CURLcode Curl_cf_ngtcp2_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                     কাঠামো Curl_peer *origin,
                                     কাঠামো Curl_peer *peer,
                                     কাঠামো Curl_cfilter **pcf)
{
  কাঠামো cf_ngtcp2_ctx *ctx = ফাঁকা;
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  CURLcode ফলাফল;

  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  ফলাফল = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  যদি(!ফলাফল)
    ফলাফল = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  যদি(ফলাফল)
    যাও out;
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);
out:
  যদি(ফলাফল) {
    curlx_safefree(cf);
    যদি(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  ফেরত ফলাফল;
}

#endif
