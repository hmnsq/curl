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


ساکن صحیح cb_h3_acked_req_body(nghttp3_conn *conn, int64_t دھارا_id,
                                uint64_t datalen, خالی *user_ڈیٹا,
                                خالی *دھارا_user_ڈیٹا);

ساکن CURLcode cf_ngtcp2_adjust_pollset(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا,
                                         ساخت easy_pollset *ps)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode نتیجہ = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, ڈیٹا);

  اگر(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    واپس CURLE_OK;

  Curl_pollset_check(ڈیٹا, ps, sock, &want_recv, &want_send);
  اگر(!want_send && !Curl_bufq_is_خالی_خانے(&ctx->q.sendbuf))
    want_send = TRUE;

  اگر(want_recv || want_send) {
    ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
    ساخت cf_call_ڈیٹا save;
    bool c_exhaust, s_exhaust;

    CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_بایاں(ctx->qconn) ||
                              !ngtcp2_conn_get_max_ڈیٹا_بایاں(ctx->qconn));
    s_exhaust = want_send && دھارا && دھارا->id >= 0 &&
                (دھارا->quic_flow_blocked ||
                 دھارا->tx_اندر_flight_سائز >= دھارا->tx_اندر_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_خالی_خانے(&ctx->q.sendbuf);

    نتیجہ = Curl_pollset_سیٹ(ڈیٹا, ps, sock, want_recv, want_send);
    CF_ڈیٹا_RESTORE(cf, save);
  }
  واپس نتیجہ;
}

ساکن صحیح cb_h3_دھارا_close(nghttp3_conn *conn, int64_t دھارا_id,
                              uint64_t app_error_رمز, خالی *user_ڈیٹا,
                              خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت h3_دھارا_ctx *دھارا;
  ساخت Curl_easy *ڈیٹا =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_ڈیٹا, &دھارا);
  (خالی)conn;
  (خالی)دھارا_id;

  /* we might be called by nghttp3 after we already cleaned up */
  اگر(!ڈیٹا || !دھارا)
    واپس 0;

  دھارا->closed = TRUE;
  دھارا->error3 = app_error_رمز;
  اگر(دھارا->error3 != NGHTTP3_H3_NO_ERROR) {
    دھارا->reset = TRUE;
    دھارا->send_closed = TRUE;
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                دھارا->id, دھارا->error3);
  }
  ورنہ {
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] CLOSED", دھارا->id);
  }
  Curl_multi_mark_dirty(ڈیٹا);
  واپس 0;
}

ساکن خالی h3_xfer_write_resp_hd(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت h3_دھارا_ctx *دھارا,
                                  ثابت حرف *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  اگر(!دھارا->xfer_نتیجہ) {
    دھارا->xfer_نتیجہ = Curl_xfer_write_resp_hd(ڈیٹا, buf, buflen, eos);
    اگر(دھارا->xfer_نتیجہ)
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", دھارا->id, (صحیح)دھارا->xfer_نتیجہ,
                  buflen);
  }
}

ساکن خالی h3_xfer_write_resp(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت h3_دھارا_ctx *دھارا,
                               ثابت حرف *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  اگر(!دھارا->xfer_نتیجہ) {
    دھارا->xfer_نتیجہ = Curl_xfer_write_resp(ڈیٹا, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    اگر(دھارا->xfer_نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  دھارا->id, (صحیح)دھارا->xfer_نتیجہ, buflen);
    }
  }
}

ساکن خالی cf_ngtcp2_upd_rx_win(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت h3_دھارا_ctx *دھارا)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_دھارا_کھڑکی_سائز_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  اگر(Curl_rlimit_فعال(&ڈیٹا->پیش_رفت.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    اگر(!دھارا->rx_انحراف)
      واپس;

    avail = Curl_rlimit_avail(&ڈیٹا->پیش_رفت.dl.rlimit, تہی);
    اگر(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", دھارا->id, avail);
      واپس;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_دھارا_کھڑکی_سائز_MAX);
  }

  اگر(دھارا->rx_انحراف_max < دھارا->rx_انحراف) {
    DEBUGASSERT(0);
    واپس;
  }
  cur_win = دھارا->rx_انحراف_max - دھارا->rx_انحراف;

  اگر(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    اگر(UINT64_MAX - delta < دھارا->rx_انحراف_max)
      delta = UINT64_MAX - دھارا->rx_انحراف_max;
    اگر(delta) {
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", دھارا->id, delta);
      دھارا->rx_انحراف_max += delta;
      ngtcp2_conn_extend_max_دھارا_انحراف(ctx->qconn, دھارا->id, delta);
    }
  }
}

ساکن صحیح cb_h3_recv_ڈیٹا(nghttp3_conn *conn, int64_t stream3_id,
                           ثابت uint8_t *buf, size_t buflen,
                           خالی *user_ڈیٹا, خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا;
  ساخت Curl_easy *ڈیٹا =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_ڈیٹا, &دھارا);

  (خالی)conn;
  (خالی)stream3_id;

  اگر(!دھارا || !ڈیٹا)
    واپس NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, ڈیٹا, دھارا, (ثابت حرف *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_انحراف(ctx->qconn, buflen);
  دھارا->rx_انحراف += buflen;
  اگر(دھارا->rx_انحراف_max < دھارا->rx_انحراف)
    دھارا->rx_انحراف_max = دھارا->rx_انحراف;

  CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              دھارا->id, buflen, دھارا->rx_انحراف_max - دھارا->rx_انحراف);
  cf_ngtcp2_upd_rx_win(cf, ڈیٹا, دھارا);
  واپس 0;
}

ساکن صحیح cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, خالی *user_ڈیٹا,
                                  خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = دھارا_user_ڈیٹا;
  (خالی)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_دھارا_انحراف(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_انحراف(ctx->qconn, consumed);

  اگر(دھارا) {
    دھارا->rx_انحراف += consumed;
    دھارا->rx_انحراف_max += consumed;
  }
  واپس 0;
}

ساکن صحیح cb_h3_انتہا_سرخیاں(nghttp3_conn *conn, int64_t دھارا_id,
                             صحیح fin, خالی *user_ڈیٹا, خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت h3_دھارا_ctx *دھارا;
  ساخت Curl_easy *ڈیٹا =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_ڈیٹا, &دھارا);
  (خالی)conn;
  (خالی)دھارا_id;
  (خالی)fin;
  (خالی)cf;

  اگر(!دھارا || !ڈیٹا)
    واپس 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, ڈیٹا, دھارا, STRCONST("\r\n"),
                        (bool)دھارا->closed);

  CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] end_headers, status=%d",
              دھارا_id, دھارا->حالت_رمز);
  اگر(دھارا->حالت_رمز / 100 != 1) {
    دھارا->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(ڈیٹا);
  واپس 0;
}

ساکن صحیح cb_h3_recv_سرخی(nghttp3_conn *conn, int64_t دھارا_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *قدر, uint8_t flags,
                             خالی *user_ڈیٹا, خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(قدر);
  ساخت h3_دھارا_ctx *دھارا;
  ساخت Curl_easy *ڈیٹا =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_ڈیٹا, &دھارا);
  CURLcode نتیجہ = CURLE_OK;
  (خالی)conn;
  (خالی)دھارا_id;
  (خالی)token;
  (خالی)flags;
  (خالی)cf;

  /* we might have cleaned up this transfer already */
  اگر(!دھارا || !ڈیٹا)
    واپس 0;

  اگر(token == NGHTTP3_QPACK_TOKEN_حالت) {

    نتیجہ = Curl_http_decode_حالت(&دھارا->حالت_رمز,
                                     (ثابت حرف *)h3val.بنیاد, h3val.len);
    اگر(نتیجہ)
      واپس NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch,
                              (ثابت حرف *)h3val.بنیاد, h3val.len);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    اگر(!نتیجہ)
      h3_xfer_write_resp_hd(cf, ڈیٹا, دھارا, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] status: %s",
                دھارا_id, curlx_dyn_ptr(&ctx->scratch));
    اگر(نتیجہ) {
      واپس NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ورنہ {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] header: %.*s: %.*s",
                دھارا_id, (صحیح)h3name.len, h3name.بنیاد,
                (صحیح)h3val.len, h3val.بنیاد);
    curlx_dyn_reset(&ctx->scratch);
    نتیجہ = curlx_dyn_addn(&ctx->scratch,
                            (ثابت حرف *)h3name.بنیاد, h3name.len);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch,
                              (ثابت حرف *)h3val.بنیاد, h3val.len);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    اگر(!نتیجہ)
      h3_xfer_write_resp_hd(cf, ڈیٹا, دھارا, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  واپس 0;
}

ساکن صحیح cb_h3_stop_sending(nghttp3_conn *conn, int64_t دھارا_id,
                              uint64_t app_error_رمز, خالی *user_ڈیٹا,
                              خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  صحیح rv;
  (خالی)conn;
  (خالی)دھارا_user_ڈیٹا;

  rv = ngtcp2_conn_shutdown_دھارا_read(ctx->qconn, 0, دھارا_id,
                                        app_error_رمز);
  اگر(rv && rv != NGTCP2_ERR_دھارا_NOT_ملا) {
    واپس NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  واپس 0;
}

ساکن صحیح cb_h3_reset_دھارا(nghttp3_conn *conn, int64_t دھارا_id,
                              uint64_t app_error_رمز, خالی *user_ڈیٹا,
                              خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_ڈیٹا, تہی);
  صحیح rv;
  (خالی)conn;

  rv = ngtcp2_conn_shutdown_دھارا_write(ctx->qconn, 0, دھارا_id,
                                         app_error_رمز);
  اگر(ڈیٹا)
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] reset -> %d", دھارا_id, rv);
  اگر(rv && rv != NGTCP2_ERR_دھارا_NOT_ملا) {
    واپس NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  واپس 0;
}

#اگر_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wmissing-field-initializers"
#اگر_ختم
ساکن nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_دھارا_close,
  cb_h3_recv_ڈیٹا,
  cb_h3_deferred_consume,
  تہی, /* begin_headers */
  cb_h3_recv_سرخی,
  cb_h3_انتہا_سرخیاں,
  تہی, /* begin_trailers */
  cb_h3_recv_سرخی,
  تہی, /* end_trailers */
  cb_h3_stop_sending,
  تہی, /* end_stream */
  cb_h3_reset_دھارا,
  تہی, /* shutdown */
  تہی, /* recv_settings (deprecated) */
#اگر_تعریف_شدہ NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  تہی, /* recv_origin */
  تہی, /* end_origin */
  تہی, /* rand */
#اگر_ختم
#اگر_تعریف_شدہ NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  تہی, /* recv_settings2 */
#اگر_ختم
#اگر_تعریف_شدہ NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  تہی, /* stream_close2 */
#اگر_ختم
};
#اگر_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic نکالو
#اگر_ختم

ساکن CURLcode init_ngh3_conn(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت cf_ngtcp2_ctx *ctx)
{
  صحیح rc;

  اگر(ngtcp2_conn_get_streams_uni_بایاں(ctx->qconn) < 3) {
    failf(ڈیٹا, "QUIC connection lacks 3 uni streams to run HTTP/3");
    واپس CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  اگر(rc) {
    failf(ڈیٹا, "error creating nghttp3 connection instance");
    واپس CURLE_برآمد_کا_حافظہ;
  }

  واپس Curl_cf_ngtcp2_h3_init_ctrls(ctx, ڈیٹا);
}

ساکن CURLcode recv_closed_دھارا(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت h3_دھارا_ctx *دھارا,
                                   size_t *pnread)
{
  (خالی)cf;
  *pnread = 0;
  اگر(دھارا->reset) {
    اگر(دھارا->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(ڈیٹا, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", دھارا->id);
      connclose(cf->conn); /* do not use this anymore */
      ڈیٹا->state.refused_دھارا = TRUE;
      واپس CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ورنہ اگر(دھارا->resp_hds_complete && ڈیٹا->req.no_body) {
        CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", دھارا->id, دھارا->error3,
                    Curl_vquic_h3_err_str(دھارا->error3));
        واپس CURLE_OK;
    }
    failf(ڈیٹا, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", دھارا->id, دھارا->error3,
          Curl_vquic_h3_err_str(دھارا->error3));
    واپس ڈیٹا->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  ورنہ اگر(!دھارا->resp_hds_complete) {
    failf(ڈیٹا,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          دھارا->id);
    واپس CURLE_HTTP3;
  }
  واپس CURLE_OK;
}

/* incoming data frames on the h3 stream */
ساکن CURLcode cf_ngtcp2_recv(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                               حرف *buf, size_t buflen, size_t *pnread)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
  ساخت cf_call_ڈیٹا save;
  ساخت cf_ngtcp2_io_ctx pktx;
  CURLcode نتیجہ = CURLE_OK;
  صحیح i;

  (خالی)ctx;
  (خالی)buf;
  NOVERBOSE((خالی)buflen);

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  اگر(ctx->tls_vrfy_نتیجہ) {
    نتیجہ = ctx->tls_vrfy_نتیجہ;
    جاؤ denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, ڈیٹا);

  اگر(!دھارا || ctx->shutdown_started) {
    نتیجہ = CURLE_RECV_ERROR;
    جاؤ برآمد;
  }

  cf_ngtcp2_upd_rx_win(cf, ڈیٹا, دھارا);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  برائے(i = 0; i < 2; ++i) {
    اگر(دھارا->xfer_نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] xfer write failed", دھارا->id);
      Curl_cf_ngtcp2_h3_دھارا_close(cf, ڈیٹا, دھارا);
      نتیجہ = دھارا->xfer_نتیجہ;
      جاؤ برآمد;
    }
    ورنہ اگر(دھارا->closed) {
      نتیجہ = recv_closed_دھارا(cf, ڈیٹا, دھارا, pnread);
      جاؤ برآمد;
    }

    اگر(!i && Curl_cf_ngtcp2_پیش_رفت_ingress(cf, ڈیٹا, &pktx)) {
      نتیجہ = CURLE_RECV_ERROR;
      جاؤ برآمد;
    }
  }

  نتیجہ = CURLE_AGAIN;

برآمد:
  نتیجہ = Curl_1st_fatal(نتیجہ,
                          Curl_cf_ngtcp2_پیش_رفت_egress(cf, ڈیٹا, &pktx));
  نتیجہ = Curl_1st_fatal(نتیجہ,
                          Curl_cf_ngtcp2_cmn_سیٹ_expiry(cf, ڈیٹا, &pktx));
  اگر(ctx->tls_vrfy_نتیجہ)
    نتیجہ = ctx->tls_vrfy_نتیجہ;
denied:
  CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              دھارا ? دھارا->id : -1, buflen, (صحیح)نتیجہ, *pnread);
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن صحیح cb_h3_acked_req_body(nghttp3_conn *conn, int64_t دھارا_id,
                                uint64_t datalen, خالی *user_ڈیٹا,
                                خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت h3_دھارا_ctx *دھارا = دھارا_user_ڈیٹا;
  size_t skiplen;

  (خالی)cf;
  اگر(!دھارا)
    واپس 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  اگر(datalen >= (uint64_t)دھارا->tx_اندر_flight_سائز)
    skiplen = دھارا->tx_اندر_flight_سائز;
  ورنہ
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&دھارا->sendbuf, skiplen);
  دھارا->tx_اندر_flight_سائز -= skiplen;

  /* Resume upload processing if we have more data to send */
  اگر(دھارا->tx_اندر_flight_سائز < Curl_bufq_len(&دھارا->sendbuf)) {
    صحیح rv = nghttp3_conn_resume_دھارا(conn, دھارا_id);
    اگر(rv && rv != NGHTTP3_ERR_دھارا_NOT_ملا) {
      واپس NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  واپس 0;
}

ساکن nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t دھارا_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, خالی *user_ڈیٹا,
                                         خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت h3_دھارا_ctx *دھارا;
  ساخت Curl_easy *ڈیٹا =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_ڈیٹا, &دھارا);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (خالی)cf;
  (خالی)conn;
  (خالی)دھارا_id;
  (خالی)user_ڈیٹا;
  (خالی)veccnt;

  اگر(!دھارا || !ڈیٹا)
    واپس NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  اگر(دھارا->tx_اندر_flight_سائز < Curl_bufq_len(&دھارا->sendbuf)) {
    nvecs = 0;
    جب(nvecs < veccnt &&
          Curl_bufq_peek_at(&دھارا->sendbuf,
                            دھارا->tx_اندر_flight_سائز,
                            CURL_UNCONST(&vec[nvecs].بنیاد),
                            &vec[nvecs].len)) {
      دھارا->tx_اندر_flight_سائز += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been be able to peek */
  }

  اگر(nwritten > 0 && دھارا->upload_بایاں != -1)
    دھارا->upload_بایاں -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  اگر(دھارا->upload_بایاں == 0) {
    *pflags = NGHTTP3_ڈیٹا_FLAG_EOF;
    دھارا->send_closed = TRUE;
  }
  ورنہ اگر(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] read req body -> AGAIN", دھارا->id);
    واپس NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              دھارا->id, nvecs,
              *pflags == NGHTTP3_ڈیٹا_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&دھارا->sendbuf),
              دھارا->upload_بایاں);
  واپس (nghttp3_ssize)nvecs;
}

ساکن CURLcode h3_دھارا_open(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ثابت uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = تہی;
  int64_t sid;
  ساخت dynhds h2_سرخیاں;
  size_t nheader;
  nghttp3_nv *nva = تہی;
  صحیح rc = 0;
  غیرعلامتی صحیح i;
  nghttp3_ڈیٹا_reader reader;
  nghttp3_ڈیٹا_reader *preader = تہی;
  CURLcode نتیجہ;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_سرخیاں, 0, DYN_HTTP_REQUEST);

  نتیجہ = Curl_cf_ngtcp2_h3_دھارا_تیاری(cf, ڈیٹا, &دھارا);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = Curl_h1_req_parse_read(&دھارا->h1, buf, len, تہی,
    !ڈیٹا->state.http_ignorecustom ?
    CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST) : تہی,
    0, pnwritten);
  اگر(نتیجہ)
    جاؤ برآمد;
  اگر(!دھارا->h1.مکمل) {
    /* need more data */
    جاؤ برآمد;
  }
  DEBUGASSERT(دھارا->h1.req);

  نتیجہ = Curl_http_req_مقصد_h2(&h2_سرخیاں, دھارا->h1.req, ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* no longer needed */
  Curl_h1_req_parse_free(&دھارا->h1);

  nheader = Curl_dynhds_گنتی(&h2_سرخیاں);
  nva = curlx_malloc(حجم(nghttp3_nv) * nheader);
  اگر(!nva) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  برائے(i = 0; i < nheader; ++i) {
    ساخت dynhds_اندراج *e = Curl_dynhds_getn(&h2_سرخیاں, i);
    nva[i].name = (غیرعلامتی حرف *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].قدر = (غیرعلامتی حرف *)e->قدر;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_دھارا(ctx->qconn, &sid, دھارا);
  اگر(rc) {
    failf(ڈیٹا, "cannot open bidi streams");
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ برآمد;
  }
  دھارا->id = sid;
  ++ctx->مستعمل_bidi_streams;

  انتخاب(ڈیٹا->state.httpreq) {
  حالت HTTPREQ_POST:
  حالت HTTPREQ_POST_FORM:
  حالت HTTPREQ_POST_MIME:
  حالت HTTPREQ_PUT:
    /* known request body size or -1 */
    اگر(ڈیٹا->state.infilesize != -1)
      دھارا->upload_بایاں = ڈیٹا->state.infilesize;
    ورنہ
      /* data sending without specifying the data amount up front */
      دھارا->upload_بایاں = -1; /* unknown */
    توڑو;
  طےشدہ:
    /* there is no request body */
    دھارا->upload_بایاں = 0; /* no request body */
    توڑو;
  }

  دھارا->send_closed = (دھارا->upload_بایاں == 0);
  اگر(!دھارا->send_closed) {
    reader.read_ڈیٹا = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, دھارا->id,
                                   nva, nheader, preader, دھارا);
  اگر(rc) {
    انتخاب(rc) {
    حالت NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(ڈیٹا, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", دھارا->id);
      توڑو;
    طےشدہ:
      CURL_TRC_CF(ڈیٹا, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", دھارا->id, rc, nghttp3_strerror(rc));
      توڑو;
    }
    Curl_cf_ngtcp2_h3_دھارا_close(cf, ڈیٹا, دھارا);
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ برآمد;
  }

  cf_ngtcp2_upd_rx_win(cf, ڈیٹا, دھارا);

  اگر(Curl_trc_is_verbose(ڈیٹا)) {
    infof(ڈیٹا, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          دھارا->id, Curl_bufref_ptr(&ڈیٹا->state.url));
    برائے(i = 0; i < nheader; ++i) {
      infof(ڈیٹا, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", دھارا->id,
            (صحیح)nva[i].namelen, nva[i].name,
            (صحیح)nva[i].valuelen, nva[i].قدر);
    }
  }

برآمد:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرخیاں);
  واپس نتیجہ;
}

ساکن CURLcode cf_ngtcp2_send(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
  ساخت cf_call_ڈیٹا save;
  ساخت cf_ngtcp2_io_ctx pktx;
  CURLcode نتیجہ = CURLE_OK;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, ڈیٹا);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  اگر(ctx->tls_vrfy_نتیجہ) {
    نتیجہ = ctx->tls_vrfy_نتیجہ;
    جاؤ denied;
  }

  (خالی)eos; /* use for stream EOF and block handling */
  نتیجہ = Curl_cf_ngtcp2_پیش_رفت_ingress(cf, ڈیٹا, &pktx);
  اگر(نتیجہ)
    جاؤ برآمد;

  اگر(!دھارا || دھارا->id < 0) {
    اگر(ctx->shutdown_started) {
      CURL_TRC_CF(ڈیٹا, cf, "cannot open stream on closed connection");
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
    نتیجہ = h3_دھارا_open(cf, ڈیٹا, buf, len, pnwritten);
    اگر(نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "failed to open stream -> %d", (صحیح)نتیجہ);
      جاؤ برآمد;
    }
    دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
  }
  ورنہ اگر(دھارا->xfer_نتیجہ) {
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] xfer write failed", دھارا->id);
    Curl_cf_ngtcp2_h3_دھارا_close(cf, ڈیٹا, دھارا);
    نتیجہ = دھارا->xfer_نتیجہ;
    جاؤ برآمد;
  }
  ورنہ اگر(دھارا->closed) {
    اگر(دھارا->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", دھارا->id);
      نتیجہ = CURLE_OK;
      *pnwritten = len;
      جاؤ برآمد;
    }
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", دھارا->id, len);
    نتیجہ = CURLE_HTTP3;
    جاؤ برآمد;
  }
  ورنہ اگر(ctx->shutdown_started) {
    CURL_TRC_CF(ڈیٹا, cf, "cannot send on closed connection");
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ برآمد;
  }
  ورنہ اگر(دھارا->tx_اندر_flight_سائز >= دھارا->tx_اندر_flight_ideal) {
    نتیجہ = CURLE_AGAIN;
    جاؤ برآمد;
  }
  ورنہ {
    نتیجہ = Curl_bufq_write(&دھارا->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                دھارا->id, len, (صحیح)نتیجہ, *pnwritten);
    اگر(نتیجہ)
      جاؤ برآمد;
    (خالی)nghttp3_conn_resume_دھارا(ctx->h3conn, دھارا->id);
  }

  اگر(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!نتیجہ);
  نتیجہ = Curl_cf_ngtcp2_پیش_رفت_egress(cf, ڈیٹا, &pktx);

  اگر(*pnwritten > 0) {
    ثابت size_t delta = (2 * 1024);
    اگر(دھارا->tx_اندر_flight_سائز > (دھارا->tx_اندر_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      دھارا->tx_اندر_flight_ideal += delta;
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  دھارا->id, len, *pnwritten,
                  دھارا->tx_اندر_flight_سائز, دھارا->tx_اندر_flight_ideal);
    }
    ورنہ اگر((دھارا->tx_اندر_flight_سائز + delta) < دھارا->tx_اندر_flight_ideal) {
      /* ngtcp2 keeps less in flight than we try to provide for,
       * decrease our "ideal" tx buffer length */
      دھارا->tx_اندر_flight_ideal -= delta;
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, reduce tx_win=%zu",
                  دھارا->id, len, *pnwritten,
                  دھارا->tx_اندر_flight_سائز, دھارا->tx_اندر_flight_ideal);
    }
  }

برآمد:
  نتیجہ = Curl_1st_fatal(نتیجہ,
                          Curl_cf_ngtcp2_cmn_سیٹ_expiry(cf, ڈیٹا, &pktx));
  اگر(ctx->tls_vrfy_نتیجہ)
    نتیجہ = ctx->tls_vrfy_نتیجہ;
denied:
  CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              دھارا ? دھارا->id : -1, len, (صحیح)نتیجہ, *pnwritten);
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن CURLcode h3_ڈیٹا_pause(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (خالی)cf;
  اگر(!pause)
    Curl_multi_mark_dirty(ڈیٹا);
  واپس CURLE_OK;
}

ساکن CURLcode cf_ngtcp2_cntrl(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                صحیح event, صحیح arg1, خالی *arg2)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  ساخت cf_call_ڈیٹا save;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  انتخاب(event) {
  حالت CF_CTRL_ڈیٹا_تیاری:
    توڑو;
  حالت CF_CTRL_ڈیٹا_PAUSE:
    نتیجہ = h3_ڈیٹا_pause(cf, ڈیٹا, (arg1 != 0));
    توڑو;
  حالت CF_CTRL_ڈیٹا_مکمل:
    Curl_cf_ngtcp2_h3_دھارا_مکمل(cf, ڈیٹا);
    توڑو;
  حالت CF_CTRL_ڈیٹا_مکمل_SEND: {
    ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
    اگر(دھارا && !دھارا->send_closed) {
      دھارا->send_closed = TRUE;
      دھارا->upload_بایاں = Curl_bufq_len(&دھارا->sendbuf) -
        دھارا->tx_اندر_flight_سائز;
      (خالی)nghttp3_conn_resume_دھارا(ctx->h3conn, دھارا->id);
    }
    توڑو;
  }
  حالت CF_CTRL_CONN_INFO_UPDATE:
    اگر(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_نظر_آیا = 30;
      Curl_conn_سیٹ_multiplex(cf->conn);
    }
    توڑو;
  طےشدہ:
    نتیجہ = Curl_cf_ngtcp2_cmn_cntrl(cf, ڈیٹا, event, arg1, arg2);
    توڑو;
  }
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن خالی cf_ngtcp2_destroy(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ڈیٹا, cf, "destroy");
  اگر(ctx) {
    اگر(ctx->qconn) {
      ساخت cf_call_ڈیٹا save;
      CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
      Curl_cf_ngtcp2_cmn_conn_close(cf, ڈیٹا);
      CF_ڈیٹا_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = تہی;
  }
}

ساکن CURLcode cf_ngtcp2_connect(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  bool *مکمل)
{
  واپس Curl_cf_ngtcp2_cmn_connect(cf, ڈیٹا, مکمل);
}

ساکن CURLcode cf_ngtcp2_query(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                صحیح query, صحیح *pres1, خالی *pres2)
{
  ساخت cf_ngtcp2_ctx *ctx = cf->ctx;
  ساخت cf_call_ڈیٹا save;

  انتخاب(query) {
  حالت CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    اگر(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    ورنہ اگر(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      اگر(ctx->max_bidi_streams > ctx->مستعمل_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->مستعمل_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > زیادہ_سے_زیادہ_عدد) ? زیادہ_سے_زیادہ_عدد : (صحیح)max_streams;
    }
    ورنہ {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(ڈیٹا->multi);
      *pres1 = (n < زیادہ_سے_زیادہ_عدد) ? (صحیح)n : زیادہ_سے_زیادہ_عدد;
    }
    CURL_TRC_CF(ڈیٹا, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_ڈیٹا_RESTORE(cf, save);
    واپس CURLE_OK;
  }
  حالت CF_QUERY_HTTP_نسخہ:
    *pres1 = 30;
    واپس CURLE_OK;
  حالت CF_QUERY_SSL_INFO:
  حالت CF_QUERY_SSL_CTX_INFO: {
    ساخت curl_tlssessioninfo *info = pres2;
    اگر(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      واپس CURLE_OK;
    توڑو;
  }
  حالت CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : تہی;
    واپس CURLE_OK;
  }
  طےشدہ:
    توڑو;
  }
  واپس Curl_cf_ngtcp2_cmn_query(cf, ڈیٹا, query, pres1, pres2);
}

ساخت Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_ngtcp2_destroy,
  cf_ngtcp2_connect,
  Curl_cf_ngtcp2_cmn_shutdown,
  cf_ngtcp2_adjust_pollset,
  Curl_cf_def_ڈیٹا_pending,
  cf_ngtcp2_send,
  cf_ngtcp2_recv,
  cf_ngtcp2_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ngtcp2_query,
};

CURLcode Curl_cf_ngtcp2_create(ساخت Curl_cfilter **pcf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_peer *origin,
                               ساخت Curl_peer *peer,
                               ساخت connectdata *conn,
                               ساخت Curl_sockaddr_ex *addr)
{
  ساخت cf_ngtcp2_ctx *ctx = تہی;
  ساخت Curl_cfilter *cf = تہی;
  CURLcode نتیجہ;

  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  نتیجہ = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  اگر(!نتیجہ)
    نتیجہ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;
  cf->conn = conn;

  نتیجہ = Curl_cf_udp_create(&cf->اگلا, ڈیٹا, origin, peer, TRNSPRT_QUIC,
                              conn, addr, تہی, TRNSPRT_QUIC);
  اگر(نتیجہ)
    جاؤ برآمد;
  cf->اگلا->conn = cf->conn;
  cf->اگلا->sockindex = cf->sockindex;

برآمد:
  *pcf = (!نتیجہ) ? cf : تہی;
  اگر(نتیجہ) {
    اگر(cf)
      Curl_conn_cf_discard_chain(&cf, ڈیٹا);
    ورنہ اگر(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  واپس نتیجہ;
}

CURLcode Curl_cf_ngtcp2_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                     ساخت Curl_peer *origin,
                                     ساخت Curl_peer *peer,
                                     ساخت Curl_cfilter **pcf)
{
  ساخت cf_ngtcp2_ctx *ctx = تہی;
  ساخت Curl_cfilter *cf = تہی;
  CURLcode نتیجہ;

  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  نتیجہ = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  اگر(!نتیجہ)
    نتیجہ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);
برآمد:
  اگر(نتیجہ) {
    curlx_safefree(cf);
    اگر(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  واپس نتیجہ;
}

#اگر_ختم
