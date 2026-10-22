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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#समावेश "urldata.h"
#समावेश "url.h"
#समावेश "u32_ptrset.h"
#समावेश "curl_trc.h"
#समावेश "rand.h"
#समावेश "multiif.h"
#समावेश "cfilters.h"
#समावेश "cf-socket.h"
#समावेश "connect.h"
#समावेश "progress.h"
#समावेश "curlx/win32-fopen.h"
#समावेश "curlx/dynbuf.h"
#समावेश "http1.h"
#समावेश "select.h"
#समावेश "transfer.h"
#समावेश "bufref.h"
#समावेश "vdns/cf-dns.h"
#समावेश "vquic/vquic.h"
#समावेश "vquic/vquic_int.h"
#समावेश "vquic/cf-ngtcp2-cmn.h"
#समावेश "vquic/cf-ngtcp2.h"


अटल पूर्णांक cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, व्योम *user_तथ्य,
                                व्योम *stream_user_तथ्य);

अटल CURLcode cf_ngtcp2_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य,
                                         काष्ठा easy_pollset *ps)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode परिणाम = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, तथ्य);

  अगर(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    वापस CURLE_OK;

  Curl_pollset_check(तथ्य, ps, sock, &want_recv, &want_send);
  अगर(!want_send && !Curl_bufq_is_empty(&ctx->q.sendbuf))
    want_send = TRUE;

  अगर(want_recv || want_send) {
    काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);
    काष्ठा cf_call_तथ्य save;
    bool c_exhaust, s_exhaust;

    CF_तथ्य_SAVE(save, cf, तथ्य);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_बायाँ(ctx->qconn) ||
                              !ngtcp2_conn_get_max_तथ्य_बायाँ(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                (stream->quic_flow_blocked ||
                 stream->tx_in_flight_आकार >= stream->tx_in_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    परिणाम = Curl_pollset_set(तथ्य, ps, sock, want_recv, want_send);
    CF_तथ्य_RESTORE(cf, save);
  }
  वापस परिणाम;
}

अटल पूर्णांक cb_h3_stream_close(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, व्योम *user_तथ्य,
                              व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा h3_stream_ctx *stream;
  काष्ठा Curl_easy *तथ्य =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_तथ्य, &stream);
  (व्योम)conn;
  (व्योम)stream_id;

  /* we might be called by nghttp3 after we already cleaned up */
  अगर(!तथ्य || !stream)
    वापस 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  अगर(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  अन्यथा {
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] CLOSED", stream->id);
  }
  Curl_multi_mark_dirty(तथ्य);
  वापस 0;
}

अटल व्योम h3_xfer_write_resp_hd(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा h3_stream_ctx *stream,
                                  स्थिर अक्षर *buf, माप_प्रकार buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  अगर(!stream->xfer_परिणाम) {
    stream->xfer_परिणाम = Curl_xfer_write_resp_hd(तथ्य, buf, buflen, eos);
    अगर(stream->xfer_परिणाम)
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (पूर्णांक)stream->xfer_परिणाम,
                  buflen);
  }
}

अटल व्योम h3_xfer_write_resp(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा h3_stream_ctx *stream,
                               स्थिर अक्षर *buf, माप_प्रकार buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  अगर(!stream->xfer_परिणाम) {
    stream->xfer_परिणाम = Curl_xfer_write_resp(तथ्य, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    अगर(stream->xfer_परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  stream->id, (पूर्णांक)stream->xfer_परिणाम, buflen);
    }
  }
}

अटल व्योम cf_ngtcp2_upd_rx_win(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा h3_stream_ctx *stream)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_STREAM_WINDOW_आकार_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  अगर(Curl_rlimit_active(&तथ्य->progress.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    अगर(!stream->rx_offset)
      वापस;

    avail = Curl_rlimit_avail(&तथ्य->progress.dl.rlimit, शून्य);
    अगर(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", stream->id, avail);
      वापस;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_STREAM_WINDOW_आकार_MAX);
  }

  अगर(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    वापस;
  }
  cur_win = stream->rx_offset_max - stream->rx_offset;

  अगर(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    अगर(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    अगर(delta) {
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", stream->id, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
    }
  }
}

अटल पूर्णांक cb_h3_recv_तथ्य(nghttp3_conn *conn, int64_t stream3_id,
                           स्थिर uint8_t *buf, माप_प्रकार buflen,
                           व्योम *user_तथ्य, व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream;
  काष्ठा Curl_easy *तथ्य =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_तथ्य, &stream);

  (व्योम)conn;
  (व्योम)stream3_id;

  अगर(!stream || !तथ्य)
    वापस NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, तथ्य, stream, (स्थिर अक्षर *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_offset(ctx->qconn, buflen);
  stream->rx_offset += buflen;
  अगर(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_ngtcp2_upd_rx_win(cf, तथ्य, stream);
  वापस 0;
}

अटल पूर्णांक cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  माप_प्रकार consumed, व्योम *user_तथ्य,
                                  व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = stream_user_तथ्य;
  (व्योम)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  अगर(stream) {
    stream->rx_offset += consumed;
    stream->rx_offset_max += consumed;
  }
  वापस 0;
}

अटल पूर्णांक cb_h3_end_headers(nghttp3_conn *conn, int64_t stream_id,
                             पूर्णांक fin, व्योम *user_तथ्य, व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा h3_stream_ctx *stream;
  काष्ठा Curl_easy *तथ्य =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_तथ्य, &stream);
  (व्योम)conn;
  (व्योम)stream_id;
  (व्योम)fin;
  (व्योम)cf;

  अगर(!stream || !तथ्य)
    वापस 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, तथ्य, stream, STRCONST("\r\n"),
                        (bool)stream->closed);

  CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] end_headers, status=%d",
              stream_id, stream->status_code);
  अगर(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(तथ्य);
  वापस 0;
}

अटल पूर्णांक cb_h3_recv_header(nghttp3_conn *conn, int64_t stream_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *मूल्य, uint8_t flags,
                             व्योम *user_तथ्य, व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(मूल्य);
  काष्ठा h3_stream_ctx *stream;
  काष्ठा Curl_easy *तथ्य =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_तथ्य, &stream);
  CURLcode परिणाम = CURLE_OK;
  (व्योम)conn;
  (व्योम)stream_id;
  (व्योम)token;
  (व्योम)flags;
  (व्योम)cf;

  /* we might have cleaned up this transfer already */
  अगर(!stream || !तथ्य)
    वापस 0;

  अगर(token == NGHTTP3_QPACK_TOKEN__STATUS) {

    परिणाम = Curl_http_decode_status(&stream->status_code,
                                     (स्थिर अक्षर *)h3val.base, h3val.len);
    अगर(परिणाम)
      वापस NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch,
                              (स्थिर अक्षर *)h3val.base, h3val.len);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    अगर(!परिणाम)
      h3_xfer_write_resp_hd(cf, तथ्य, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] status: %s",
                stream_id, curlx_dyn_ptr(&ctx->scratch));
    अगर(परिणाम) {
      वापस NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  अन्यथा {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] header: %.*s: %.*s",
                stream_id, (पूर्णांक)h3name.len, h3name.base,
                (पूर्णांक)h3val.len, h3val.base);
    curlx_dyn_reset(&ctx->scratch);
    परिणाम = curlx_dyn_addn(&ctx->scratch,
                            (स्थिर अक्षर *)h3name.base, h3name.len);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch,
                              (स्थिर अक्षर *)h3val.base, h3val.len);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    अगर(!परिणाम)
      h3_xfer_write_resp_hd(cf, तथ्य, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  वापस 0;
}

अटल पूर्णांक cb_h3_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, व्योम *user_तथ्य,
                              व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णांक rv;
  (व्योम)conn;
  (व्योम)stream_user_तथ्य;

  rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                        app_error_code);
  अगर(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    वापस NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  वापस 0;
}

अटल पूर्णांक cb_h3_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                              uint64_t app_error_code, व्योम *user_तथ्य,
                              व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = Curl_cf_ngtcp2_get_xfer(cf, stream_user_तथ्य, शून्य);
  पूर्णांक rv;
  (व्योम)conn;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  अगर(तथ्य)
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  अगर(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    वापस NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  वापस 0;
}

#अगर_घोषित CURL_HAVE_DIAG
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wmissing-field-initializers"
#पूर्ण_अगर
अटल nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_stream_close,
  cb_h3_recv_तथ्य,
  cb_h3_deferred_consume,
  शून्य, /* begin_headers */
  cb_h3_recv_header,
  cb_h3_end_headers,
  शून्य, /* begin_trailers */
  cb_h3_recv_header,
  शून्य, /* end_trailers */
  cb_h3_stop_sending,
  शून्य, /* end_stream */
  cb_h3_reset_stream,
  शून्य, /* shutdown */
  शून्य, /* recv_settings (deprecated) */
#अगर_घोषित NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  शून्य, /* recv_origin */
  शून्य, /* end_origin */
  शून्य, /* rand */
#पूर्ण_अगर
#अगर_घोषित NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  शून्य, /* recv_settings2 */
#पूर्ण_अगर
#अगर_घोषित NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  शून्य, /* stream_close2 */
#पूर्ण_अगर
};
#अगर_घोषित CURL_HAVE_DIAG
#आशय GCC diagnostic निकालो
#पूर्ण_अगर

अटल CURLcode init_ngh3_conn(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा cf_ngtcp2_ctx *ctx)
{
  पूर्णांक rc;

  अगर(ngtcp2_conn_get_streams_uni_बायाँ(ctx->qconn) < 3) {
    failf(तथ्य, "QUIC connection lacks 3 uni streams to run HTTP/3");
    वापस CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  अगर(rc) {
    failf(तथ्य, "error creating nghttp3 connection instance");
    वापस CURLE_OUT_OF_MEMORY;
  }

  वापस Curl_cf_ngtcp2_h3_init_ctrls(ctx, तथ्य);
}

अटल CURLcode recv_closed_stream(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा h3_stream_ctx *stream,
                                   माप_प्रकार *pnread)
{
  (व्योम)cf;
  *pnread = 0;
  अगर(stream->reset) {
    अगर(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(तथ्य, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      तथ्य->state.refused_stream = TRUE;
      वापस CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    अन्यथा अगर(stream->resp_hds_complete && तथ्य->req.no_body) {
        CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        वापस CURLE_OK;
    }
    failf(तथ्य, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    वापस तथ्य->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  अन्यथा अगर(!stream->resp_hds_complete) {
    failf(तथ्य,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    वापस CURLE_HTTP3;
  }
  वापस CURLE_OK;
}

/* incoming data frames on the h3 stream */
अटल CURLcode cf_ngtcp2_recv(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                               अक्षर *buf, माप_प्रकार buflen, माप_प्रकार *pnread)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);
  काष्ठा cf_call_तथ्य save;
  काष्ठा cf_ngtcp2_io_ctx pktx;
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक i;

  (व्योम)ctx;
  (व्योम)buf;
  NOVERBOSE((व्योम)buflen);

  CF_तथ्य_SAVE(save, cf, तथ्य);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  अगर(ctx->tls_vrfy_परिणाम) {
    परिणाम = ctx->tls_vrfy_परिणाम;
    जाओ denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, तथ्य);

  अगर(!stream || ctx->shutdown_started) {
    परिणाम = CURLE_RECV_ERROR;
    जाओ out;
  }

  cf_ngtcp2_upd_rx_win(cf, तथ्य, stream);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  क्रम(i = 0; i < 2; ++i) {
    अगर(stream->xfer_परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, तथ्य, stream);
      परिणाम = stream->xfer_परिणाम;
      जाओ out;
    }
    अन्यथा अगर(stream->closed) {
      परिणाम = recv_closed_stream(cf, तथ्य, stream, pnread);
      जाओ out;
    }

    अगर(!i && Curl_cf_ngtcp2_progress_ingress(cf, तथ्य, &pktx)) {
      परिणाम = CURLE_RECV_ERROR;
      जाओ out;
    }
  }

  परिणाम = CURLE_AGAIN;

out:
  परिणाम = Curl_1st_fatal(परिणाम,
                          Curl_cf_ngtcp2_progress_egress(cf, तथ्य, &pktx));
  परिणाम = Curl_1st_fatal(परिणाम,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, तथ्य, &pktx));
  अगर(ctx->tls_vrfy_परिणाम)
    परिणाम = ctx->tls_vrfy_परिणाम;
denied:
  CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              stream ? stream->id : -1, buflen, (पूर्णांक)परिणाम, *pnread);
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल पूर्णांक cb_h3_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                uint64_t datalen, व्योम *user_तथ्य,
                                व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा h3_stream_ctx *stream = stream_user_तथ्य;
  माप_प्रकार skiplen;

  (व्योम)cf;
  अगर(!stream)
    वापस 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  अगर(datalen >= (uint64_t)stream->tx_in_flight_आकार)
    skiplen = stream->tx_in_flight_आकार;
  अन्यथा
    skiplen = (माप_प्रकार)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_आकार -= skiplen;

  /* Resume upload processing if we have more data to send */
  अगर(stream->tx_in_flight_आकार < Curl_bufq_len(&stream->sendbuf)) {
    पूर्णांक rv = nghttp3_conn_resume_stream(conn, stream_id);
    अगर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      वापस NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  वापस 0;
}

अटल nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t stream_id,
                                         nghttp3_vec *vec, माप_प्रकार veccnt,
                                         uint32_t *pflags, व्योम *user_तथ्य,
                                         व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा h3_stream_ctx *stream;
  काष्ठा Curl_easy *तथ्य =
    Curl_cf_ngtcp2_get_xfer(cf, stream_user_तथ्य, &stream);
  माप_प्रकार nwritten = 0;
  माप_प्रकार nvecs = 0;
  (व्योम)cf;
  (व्योम)conn;
  (व्योम)stream_id;
  (व्योम)user_तथ्य;
  (व्योम)veccnt;

  अगर(!stream || !तथ्य)
    वापस NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  अगर(stream->tx_in_flight_आकार < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    जबतक(nvecs < veccnt &&
          Curl_bufq_peek_at(&stream->sendbuf,
                            stream->tx_in_flight_आकार,
                            CURL_UNCONST(&vec[nvecs].base),
                            &vec[nvecs].len)) {
      stream->tx_in_flight_आकार += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been be able to peek */
  }

  अगर(nwritten > 0 && stream->upload_बायाँ != -1)
    stream->upload_बायाँ -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  अगर(stream->upload_बायाँ == 0) {
    *pflags = NGHTTP3_तथ्य_FLAG_EOF;
    stream->send_closed = TRUE;
  }
  अन्यथा अगर(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] read req body -> AGAIN", stream->id);
    वापस NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              stream->id, nvecs,
              *pflags == NGHTTP3_तथ्य_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf),
              stream->upload_बायाँ);
  वापस (nghttp3_ssize)nvecs;
}

अटल CURLcode h3_stream_open(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               स्थिर uint8_t *buf, माप_प्रकार len,
                               माप_प्रकार *pnwritten)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = शून्य;
  int64_t sid;
  काष्ठा dynhds h2_headers;
  माप_प्रकार nheader;
  nghttp3_nv *nva = शून्य;
  पूर्णांक rc = 0;
  अचिन्हित पूर्णांक i;
  nghttp3_तथ्य_reader reader;
  nghttp3_तथ्य_reader *preader = शून्य;
  CURLcode परिणाम;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  परिणाम = Curl_cf_ngtcp2_h3_stream_setup(cf, तथ्य, &stream);
  अगर(परिणाम)
    जाओ out;

  परिणाम = Curl_h1_req_parse_read(&stream->h1, buf, len, शून्य,
    !तथ्य->state.http_ignorecustom ?
    CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST) : शून्य,
    0, pnwritten);
  अगर(परिणाम)
    जाओ out;
  अगर(!stream->h1.done) {
    /* need more data */
    जाओ out;
  }
  DEBUGASSERT(stream->h1.req);

  परिणाम = Curl_http_req_to_h2(&h2_headers, stream->h1.req, तथ्य);
  अगर(परिणाम)
    जाओ out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_गिनती(&h2_headers);
  nva = curlx_malloc(माप(nghttp3_nv) * nheader);
  अगर(!nva) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  क्रम(i = 0; i < nheader; ++i) {
    काष्ठा dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (अचिन्हित अक्षर *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].मूल्य = (अचिन्हित अक्षर *)e->मूल्य;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, stream);
  अगर(rc) {
    failf(तथ्य, "cannot open bidi streams");
    परिणाम = CURLE_SEND_ERROR;
    जाओ out;
  }
  stream->id = sid;
  ++ctx->used_bidi_streams;

  चयन(तथ्य->state.httpreq) {
  हाल HTTPREQ_POST:
  हाल HTTPREQ_POST_FORM:
  हाल HTTPREQ_POST_MIME:
  हाल HTTPREQ_PUT:
    /* known request body size or -1 */
    अगर(तथ्य->state.infilesize != -1)
      stream->upload_बायाँ = तथ्य->state.infilesize;
    अन्यथा
      /* data sending without specifying the data amount up front */
      stream->upload_बायाँ = -1; /* unknown */
    अवरोध;
  शेष:
    /* there is no request body */
    stream->upload_बायाँ = 0; /* no request body */
    अवरोध;
  }

  stream->send_closed = (stream->upload_बायाँ == 0);
  अगर(!stream->send_closed) {
    reader.read_तथ्य = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, stream->id,
                                   nva, nheader, preader, stream);
  अगर(rc) {
    चयन(rc) {
    हाल NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(तथ्य, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      अवरोध;
    शेष:
      CURL_TRC_CF(तथ्य, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", stream->id, rc, nghttp3_strerror(rc));
      अवरोध;
    }
    Curl_cf_ngtcp2_h3_stream_close(cf, तथ्य, stream);
    परिणाम = CURLE_SEND_ERROR;
    जाओ out;
  }

  cf_ngtcp2_upd_rx_win(cf, तथ्य, stream);

  अगर(Curl_trc_is_verbose(तथ्य)) {
    infof(तथ्य, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&तथ्य->state.url));
    क्रम(i = 0; i < nheader; ++i) {
      infof(तथ्य, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", stream->id,
            (पूर्णांक)nva[i].namelen, nva[i].name,
            (पूर्णांक)nva[i].valuelen, nva[i].मूल्य);
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  वापस परिणाम;
}

अटल CURLcode cf_ngtcp2_send(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                               स्थिर uint8_t *buf, माप_प्रकार len, bool eos,
                               माप_प्रकार *pnwritten)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);
  काष्ठा cf_call_तथ्य save;
  काष्ठा cf_ngtcp2_io_ctx pktx;
  CURLcode परिणाम = CURLE_OK;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, तथ्य);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  अगर(ctx->tls_vrfy_परिणाम) {
    परिणाम = ctx->tls_vrfy_परिणाम;
    जाओ denied;
  }

  (व्योम)eos; /* use for stream EOF and block handling */
  परिणाम = Curl_cf_ngtcp2_progress_ingress(cf, तथ्य, &pktx);
  अगर(परिणाम)
    जाओ out;

  अगर(!stream || stream->id < 0) {
    अगर(ctx->shutdown_started) {
      CURL_TRC_CF(तथ्य, cf, "cannot open stream on closed connection");
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
    परिणाम = h3_stream_open(cf, तथ्य, buf, len, pnwritten);
    अगर(परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "failed to open stream -> %d", (पूर्णांक)परिणाम);
      जाओ out;
    }
    stream = H3_STREAM_CTX(ctx, तथ्य);
  }
  अन्यथा अगर(stream->xfer_परिणाम) {
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] xfer write failed", stream->id);
    Curl_cf_ngtcp2_h3_stream_close(cf, तथ्य, stream);
    परिणाम = stream->xfer_परिणाम;
    जाओ out;
  }
  अन्यथा अगर(stream->closed) {
    अगर(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      परिणाम = CURLE_OK;
      *pnwritten = len;
      जाओ out;
    }
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    परिणाम = CURLE_HTTP3;
    जाओ out;
  }
  अन्यथा अगर(ctx->shutdown_started) {
    CURL_TRC_CF(तथ्य, cf, "cannot send on closed connection");
    परिणाम = CURLE_SEND_ERROR;
    जाओ out;
  }
  अन्यथा अगर(stream->tx_in_flight_आकार >= stream->tx_in_flight_ideal) {
    परिणाम = CURLE_AGAIN;
    जाओ out;
  }
  अन्यथा {
    परिणाम = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (पूर्णांक)परिणाम, *pnwritten);
    अगर(परिणाम)
      जाओ out;
    (व्योम)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  अगर(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!परिणाम);
  परिणाम = Curl_cf_ngtcp2_progress_egress(cf, तथ्य, &pktx);

  अगर(*pnwritten > 0) {
    स्थिर माप_प्रकार delta = (2 * 1024);
    अगर(stream->tx_in_flight_आकार > (stream->tx_in_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      stream->tx_in_flight_ideal += delta;
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  stream->id, len, *pnwritten,
                  stream->tx_in_flight_आकार, stream->tx_in_flight_ideal);
    }
    अन्यथा अगर((stream->tx_in_flight_आकार + delta) < stream->tx_in_flight_ideal) {
      /* ngtcp2 keeps less in flight than we try to provide for,
       * decrease our "ideal" tx buffer length */
      stream->tx_in_flight_ideal -= delta;
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, reduce tx_win=%zu",
                  stream->id, len, *pnwritten,
                  stream->tx_in_flight_आकार, stream->tx_in_flight_ideal);
    }
  }

out:
  परिणाम = Curl_1st_fatal(परिणाम,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, तथ्य, &pktx));
  अगर(ctx->tls_vrfy_परिणाम)
    परिणाम = ctx->tls_vrfy_परिणाम;
denied:
  CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (पूर्णांक)परिणाम, *pnwritten);
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल CURLcode h3_तथ्य_pause(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              bool छोड़ो)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (व्योम)cf;
  अगर(!छोड़ो)
    Curl_multi_mark_dirty(तथ्य);
  वापस CURLE_OK;
}

अटल CURLcode cf_ngtcp2_cntrl(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा cf_call_तथ्य save;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  चयन(event) {
  हाल CF_CTRL_तथ्य_SETUP:
    अवरोध;
  हाल CF_CTRL_तथ्य_PAUSE:
    परिणाम = h3_तथ्य_pause(cf, तथ्य, (arg1 != 0));
    अवरोध;
  हाल CF_CTRL_तथ्य_DONE:
    Curl_cf_ngtcp2_h3_stream_done(cf, तथ्य);
    अवरोध;
  हाल CF_CTRL_तथ्य_DONE_SEND: {
    काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);
    अगर(stream && !stream->send_closed) {
      stream->send_closed = TRUE;
      stream->upload_बायाँ = Curl_bufq_len(&stream->sendbuf) -
        stream->tx_in_flight_आकार;
      (व्योम)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
    }
    अवरोध;
  }
  हाल CF_CTRL_CONN_INFO_UPDATE:
    अगर(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    अवरोध;
  शेष:
    परिणाम = Curl_cf_ngtcp2_cmn_cntrl(cf, तथ्य, event, arg1, arg2);
    अवरोध;
  }
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल व्योम cf_ngtcp2_destroy(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(तथ्य, cf, "destroy");
  अगर(ctx) {
    अगर(ctx->qconn) {
      काष्ठा cf_call_तथ्य save;
      CF_तथ्य_SAVE(save, cf, तथ्य);
      Curl_cf_ngtcp2_cmn_conn_close(cf, तथ्य);
      CF_तथ्य_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = शून्य;
  }
}

अटल CURLcode cf_ngtcp2_connect(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  bool *done)
{
  वापस Curl_cf_ngtcp2_cmn_connect(cf, तथ्य, done);
}

अटल CURLcode cf_ngtcp2_query(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  काष्ठा cf_ngtcp2_ctx *ctx = cf->ctx;
  काष्ठा cf_call_तथ्य save;

  चयन(query) {
  हाल CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_तथ्य_SAVE(save, cf, तथ्य);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    अगर(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    अन्यथा अगर(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      अगर(ctx->max_bidi_streams > ctx->used_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->used_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > पूर्णांक_उच्च) ? पूर्णांक_उच्च : (पूर्णांक)max_streams;
    }
    अन्यथा {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(तथ्य->multi);
      *pres1 = (n < पूर्णांक_उच्च) ? (पूर्णांक)n : पूर्णांक_उच्च;
    }
    CURL_TRC_CF(तथ्य, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_तथ्य_RESTORE(cf, save);
    वापस CURLE_OK;
  }
  हाल CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    वापस CURLE_OK;
  हाल CF_QUERY_SSL_INFO:
  हाल CF_QUERY_SSL_CTX_INFO: {
    काष्ठा curl_tlssessioninfo *info = pres2;
    अगर(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      वापस CURLE_OK;
    अवरोध;
  }
  हाल CF_QUERY_ALPN_NEGOTIATED: {
    स्थिर अक्षर **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : शून्य;
    वापस CURLE_OK;
  }
  शेष:
    अवरोध;
  }
  वापस Curl_cf_ngtcp2_cmn_query(cf, तथ्य, query, pres1, pres2);
}

काष्ठा Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_ngtcp2_destroy,
  cf_ngtcp2_connect,
  Curl_cf_ngtcp2_cmn_shutdown,
  cf_ngtcp2_adjust_pollset,
  Curl_cf_def_तथ्य_pending,
  cf_ngtcp2_send,
  cf_ngtcp2_recv,
  cf_ngtcp2_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ngtcp2_query,
};

CURLcode Curl_cf_ngtcp2_create(काष्ठा Curl_cfilter **pcf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_peer *origin,
                               काष्ठा Curl_peer *peer,
                               काष्ठा connectdata *conn,
                               काष्ठा Curl_sockaddr_ex *addr)
{
  काष्ठा cf_ngtcp2_ctx *ctx = शून्य;
  काष्ठा Curl_cfilter *cf = शून्य;
  CURLcode परिणाम;

  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  परिणाम = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  अगर(!परिणाम)
    परिणाम = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  अगर(परिणाम)
    जाओ out;
  cf->conn = conn;

  परिणाम = Curl_cf_udp_create(&cf->अग्र, तथ्य, origin, peer, TRNSPRT_QUIC,
                              conn, addr, शून्य, TRNSPRT_QUIC);
  अगर(परिणाम)
    जाओ out;
  cf->अग्र->conn = cf->conn;
  cf->अग्र->sockindex = cf->sockindex;

out:
  *pcf = (!परिणाम) ? cf : शून्य;
  अगर(परिणाम) {
    अगर(cf)
      Curl_conn_cf_discard_chain(&cf, तथ्य);
    अन्यथा अगर(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  वापस परिणाम;
}

CURLcode Curl_cf_ngtcp2_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                     काष्ठा Curl_peer *origin,
                                     काष्ठा Curl_peer *peer,
                                     काष्ठा Curl_cfilter **pcf)
{
  काष्ठा cf_ngtcp2_ctx *ctx = शून्य;
  काष्ठा Curl_cfilter *cf = शून्य;
  CURLcode परिणाम;

  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  परिणाम = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  अगर(!परिणाम)
    परिणाम = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  अगर(परिणाम)
    जाओ out;
  Curl_conn_cf_जोड़ो_after(cf_at, cf);
out:
  अगर(परिणाम) {
    curlx_safefree(cf);
    अगर(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  वापस परिणाम;
}

#पूर्ण_अगर
