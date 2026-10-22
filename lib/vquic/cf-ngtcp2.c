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


ساكن صحيح cb_h3_acked_req_body(nghttp3_conn *conn, int64_t جریان_id,
                                uint64_t datalen, فراغ *user_داده,
                                فراغ *جریان_user_داده);

ساكن CURLcode cf_ngtcp2_adjust_pollset(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *داده,
                                         بنية easy_pollset *ps)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode نتیجه = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, داده);

  إذا(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    أرجع CURLE_OK;

  Curl_pollset_check(داده, ps, sock, &want_recv, &want_send);
  إذا(!want_send && !Curl_bufq_is_خالی(&ctx->q.sendbuf))
    want_send = TRUE;

  إذا(want_recv || want_send) {
    بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);
    بنية cf_call_داده save;
    bool c_exhaust, s_exhaust;

    CF_داده_SAVE(save, cf, داده);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_چپ(ctx->qconn) ||
                              !ngtcp2_conn_get_max_داده_چپ(ctx->qconn));
    s_exhaust = want_send && جریان && جریان->id >= 0 &&
                (جریان->quic_flow_blocked ||
                 جریان->tx_درون_flight_اندازه >= جریان->tx_درون_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_خالی(&ctx->q.sendbuf);

    نتیجه = Curl_pollset_مجموعه(داده, ps, sock, want_recv, want_send);
    CF_داده_RESTORE(cf, save);
  }
  أرجع نتیجه;
}

ساكن صحيح cb_h3_جریان_close(nghttp3_conn *conn, int64_t جریان_id,
                              uint64_t app_error_کد, فراغ *user_داده,
                              فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية h3_جریان_ctx *جریان;
  بنية Curl_easy *داده =
    Curl_cf_ngtcp2_get_xfer(cf, جریان_user_داده, &جریان);
  (فراغ)conn;
  (فراغ)جریان_id;

  /* we might be called by nghttp3 after we already cleaned up */
  إذا(!داده || !جریان)
    أرجع 0;

  جریان->closed = TRUE;
  جریان->error3 = app_error_کد;
  إذا(جریان->error3 != NGHTTP3_H3_NO_ERROR) {
    جریان->reset = TRUE;
    جریان->send_closed = TRUE;
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                جریان->id, جریان->error3);
  }
  وإلا {
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] CLOSED", جریان->id);
  }
  Curl_multi_mark_dirty(داده);
  أرجع 0;
}

ساكن فراغ h3_xfer_write_resp_hd(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  بنية h3_جریان_ctx *جریان,
                                  ثابت حرف *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  إذا(!جریان->xfer_نتیجه) {
    جریان->xfer_نتیجه = Curl_xfer_write_resp_hd(داده, buf, buflen, eos);
    إذا(جریان->xfer_نتیجه)
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", جریان->id, (صحيح)جریان->xfer_نتیجه,
                  buflen);
  }
}

ساكن فراغ h3_xfer_write_resp(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               بنية h3_جریان_ctx *جریان,
                               ثابت حرف *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  إذا(!جریان->xfer_نتیجه) {
    جریان->xfer_نتیجه = Curl_xfer_write_resp(داده, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    إذا(جریان->xfer_نتیجه) {
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  جریان->id, (صحيح)جریان->xfer_نتیجه, buflen);
    }
  }
}

ساكن فراغ cf_ngtcp2_upd_rx_win(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 بنية h3_جریان_ctx *جریان)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_جریان_پنجره_اندازه_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  إذا(Curl_rlimit_فعال(&داده->پیشرفت.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    إذا(!جریان->rx_جابه_جایی)
      أرجع;

    avail = Curl_rlimit_avail(&داده->پیشرفت.dl.rlimit, عدم);
    إذا(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", جریان->id, avail);
      أرجع;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_جریان_پنجره_اندازه_MAX);
  }

  إذا(جریان->rx_جابه_جایی_max < جریان->rx_جابه_جایی) {
    DEBUGASSERT(0);
    أرجع;
  }
  cur_win = جریان->rx_جابه_جایی_max - جریان->rx_جابه_جایی;

  إذا(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    إذا(UINT64_MAX - delta < جریان->rx_جابه_جایی_max)
      delta = UINT64_MAX - جریان->rx_جابه_جایی_max;
    إذا(delta) {
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", جریان->id, delta);
      جریان->rx_جابه_جایی_max += delta;
      ngtcp2_conn_extend_max_جریان_جابه_جایی(ctx->qconn, جریان->id, delta);
    }
  }
}

ساكن صحيح cb_h3_recv_داده(nghttp3_conn *conn, int64_t stream3_id,
                           ثابت uint8_t *buf, size_t buflen,
                           فراغ *user_داده, فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان;
  بنية Curl_easy *داده =
    Curl_cf_ngtcp2_get_xfer(cf, جریان_user_داده, &جریان);

  (فراغ)conn;
  (فراغ)stream3_id;

  إذا(!جریان || !داده)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, داده, جریان, (ثابت حرف *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_جابه_جایی(ctx->qconn, buflen);
  جریان->rx_جابه_جایی += buflen;
  إذا(جریان->rx_جابه_جایی_max < جریان->rx_جابه_جایی)
    جریان->rx_جابه_جایی_max = جریان->rx_جابه_جایی;

  CURL_TRC_CF(داده, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              جریان->id, buflen, جریان->rx_جابه_جایی_max - جریان->rx_جابه_جایی);
  cf_ngtcp2_upd_rx_win(cf, داده, جریان);
  أرجع 0;
}

ساكن صحيح cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, فراغ *user_داده,
                                  فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = جریان_user_داده;
  (فراغ)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_جریان_جابه_جایی(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_جابه_جایی(ctx->qconn, consumed);

  إذا(جریان) {
    جریان->rx_جابه_جایی += consumed;
    جریان->rx_جابه_جایی_max += consumed;
  }
  أرجع 0;
}

ساكن صحيح cb_h3_انتها_سرآیندها(nghttp3_conn *conn, int64_t جریان_id,
                             صحيح fin, فراغ *user_داده, فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية h3_جریان_ctx *جریان;
  بنية Curl_easy *داده =
    Curl_cf_ngtcp2_get_xfer(cf, جریان_user_داده, &جریان);
  (فراغ)conn;
  (فراغ)جریان_id;
  (فراغ)fin;
  (فراغ)cf;

  إذا(!جریان || !داده)
    أرجع 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, داده, جریان, STRCONST("\r\n"),
                        (bool)جریان->closed);

  CURL_TRC_CF(داده, cf, "[%" PRId64 "] end_headers, status=%d",
              جریان_id, جریان->وضعیت_کد);
  إذا(جریان->وضعیت_کد / 100 != 1) {
    جریان->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(داده);
  أرجع 0;
}

ساكن صحيح cb_h3_recv_سرآیند(nghttp3_conn *conn, int64_t جریان_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *مقدار, uint8_t flags,
                             فراغ *user_داده, فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(مقدار);
  بنية h3_جریان_ctx *جریان;
  بنية Curl_easy *داده =
    Curl_cf_ngtcp2_get_xfer(cf, جریان_user_داده, &جریان);
  CURLcode نتیجه = CURLE_OK;
  (فراغ)conn;
  (فراغ)جریان_id;
  (فراغ)token;
  (فراغ)flags;
  (فراغ)cf;

  /* we might have cleaned up this transfer already */
  إذا(!جریان || !داده)
    أرجع 0;

  إذا(token == NGHTTP3_QPACK_TOKEN_وضعیت) {

    نتیجه = Curl_http_decode_وضعیت(&جریان->وضعیت_کد,
                                     (ثابت حرف *)h3val.پایه, h3val.len);
    إذا(نتیجه)
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    نتیجه = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(&ctx->scratch,
                              (ثابت حرف *)h3val.پایه, h3val.len);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    إذا(!نتیجه)
      h3_xfer_write_resp_hd(cf, داده, جریان, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] status: %s",
                جریان_id, curlx_dyn_ptr(&ctx->scratch));
    إذا(نتیجه) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  وإلا {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] header: %.*s: %.*s",
                جریان_id, (صحيح)h3name.len, h3name.پایه,
                (صحيح)h3val.len, h3val.پایه);
    curlx_dyn_reset(&ctx->scratch);
    نتیجه = curlx_dyn_addn(&ctx->scratch,
                            (ثابت حرف *)h3name.پایه, h3name.len);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(&ctx->scratch,
                              (ثابت حرف *)h3val.پایه, h3val.len);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    إذا(!نتیجه)
      h3_xfer_write_resp_hd(cf, داده, جریان, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  أرجع 0;
}

ساكن صحيح cb_h3_stop_sending(nghttp3_conn *conn, int64_t جریان_id,
                              uint64_t app_error_کد, فراغ *user_داده,
                              فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)conn;
  (فراغ)جریان_user_داده;

  rv = ngtcp2_conn_shutdown_جریان_read(ctx->qconn, 0, جریان_id,
                                        app_error_کد);
  إذا(rv && rv != NGTCP2_ERR_جریان_NOT_یافت) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_h3_reset_جریان(nghttp3_conn *conn, int64_t جریان_id,
                              uint64_t app_error_کد, فراغ *user_داده,
                              فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده = Curl_cf_ngtcp2_get_xfer(cf, جریان_user_داده, عدم);
  صحيح rv;
  (فراغ)conn;

  rv = ngtcp2_conn_shutdown_جریان_write(ctx->qconn, 0, جریان_id,
                                         app_error_کد);
  إذا(داده)
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] reset -> %d", جریان_id, rv);
  إذا(rv && rv != NGTCP2_ERR_جریان_NOT_یافت) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

#اگر_تعریف_شده CURL_HAVE_DIAG
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Wmissing-field-initializers"
#پایان_اگر
ساكن nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_جریان_close,
  cb_h3_recv_داده,
  cb_h3_deferred_consume,
  عدم, /* begin_headers */
  cb_h3_recv_سرآیند,
  cb_h3_انتها_سرآیندها,
  عدم, /* begin_trailers */
  cb_h3_recv_سرآیند,
  عدم, /* end_trailers */
  cb_h3_stop_sending,
  عدم, /* end_stream */
  cb_h3_reset_جریان,
  عدم, /* shutdown */
  عدم, /* recv_settings (deprecated) */
#اگر_تعریف_شده NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  عدم, /* recv_origin */
  عدم, /* end_origin */
  عدم, /* rand */
#پایان_اگر
#اگر_تعریف_شده NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  عدم, /* recv_settings2 */
#پایان_اگر
#اگر_تعریف_شده NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  عدم, /* stream_close2 */
#پایان_اگر
};
#اگر_تعریف_شده CURL_HAVE_DIAG
#رهنمود GCC diagnostic برداشت
#پایان_اگر

ساكن CURLcode init_ngh3_conn(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               بنية cf_ngtcp2_ctx *ctx)
{
  صحيح rc;

  إذا(ngtcp2_conn_get_streams_uni_چپ(ctx->qconn) < 3) {
    failf(داده, "QUIC connection lacks 3 uni streams to run HTTP/3");
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  إذا(rc) {
    failf(داده, "error creating nghttp3 connection instance");
    أرجع CURLE_بیرون_از_حافظه;
  }

  أرجع Curl_cf_ngtcp2_h3_init_ctrls(ctx, داده);
}

ساكن CURLcode recv_closed_جریان(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   بنية h3_جریان_ctx *جریان,
                                   size_t *pnread)
{
  (فراغ)cf;
  *pnread = 0;
  إذا(جریان->reset) {
    إذا(جریان->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(داده, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", جریان->id);
      connclose(cf->conn); /* do not use this anymore */
      داده->state.refused_جریان = TRUE;
      أرجع CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وإلا إذا(جریان->resp_hds_complete && داده->req.no_body) {
        CURL_TRC_CF(داده, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", جریان->id, جریان->error3,
                    Curl_vquic_h3_err_str(جریان->error3));
        أرجع CURLE_OK;
    }
    failf(داده, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", جریان->id, جریان->error3,
          Curl_vquic_h3_err_str(جریان->error3));
    أرجع داده->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  وإلا إذا(!جریان->resp_hds_complete) {
    failf(داده,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          جریان->id);
    أرجع CURLE_HTTP3;
  }
  أرجع CURLE_OK;
}

/* incoming data frames on the h3 stream */
ساكن CURLcode cf_ngtcp2_recv(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                               حرف *buf, size_t buflen, size_t *pnread)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);
  بنية cf_call_داده save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode نتیجه = CURLE_OK;
  صحيح i;

  (فراغ)ctx;
  (فراغ)buf;
  NOVERBOSE((فراغ)buflen);

  CF_داده_SAVE(save, cf, داده);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  إذا(ctx->tls_vrfy_نتیجه) {
    نتیجه = ctx->tls_vrfy_نتیجه;
    اذهب denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, داده);

  إذا(!جریان || ctx->shutdown_started) {
    نتیجه = CURLE_RECV_ERROR;
    اذهب بیرون;
  }

  cf_ngtcp2_upd_rx_win(cf, داده, جریان);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  لكل(i = 0; i < 2; ++i) {
    إذا(جریان->xfer_نتیجه) {
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] xfer write failed", جریان->id);
      Curl_cf_ngtcp2_h3_جریان_close(cf, داده, جریان);
      نتیجه = جریان->xfer_نتیجه;
      اذهب بیرون;
    }
    وإلا إذا(جریان->closed) {
      نتیجه = recv_closed_جریان(cf, داده, جریان, pnread);
      اذهب بیرون;
    }

    إذا(!i && Curl_cf_ngtcp2_پیشرفت_ingress(cf, داده, &pktx)) {
      نتیجه = CURLE_RECV_ERROR;
      اذهب بیرون;
    }
  }

  نتیجه = CURLE_AGAIN;

بیرون:
  نتیجه = Curl_1st_fatal(نتیجه,
                          Curl_cf_ngtcp2_پیشرفت_egress(cf, داده, &pktx));
  نتیجه = Curl_1st_fatal(نتیجه,
                          Curl_cf_ngtcp2_cmn_مجموعه_expiry(cf, داده, &pktx));
  إذا(ctx->tls_vrfy_نتیجه)
    نتیجه = ctx->tls_vrfy_نتیجه;
denied:
  CURL_TRC_CF(داده, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              جریان ? جریان->id : -1, buflen, (صحيح)نتیجه, *pnread);
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن صحيح cb_h3_acked_req_body(nghttp3_conn *conn, int64_t جریان_id,
                                uint64_t datalen, فراغ *user_داده,
                                فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية h3_جریان_ctx *جریان = جریان_user_داده;
  size_t skiplen;

  (فراغ)cf;
  إذا(!جریان)
    أرجع 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  إذا(datalen >= (uint64_t)جریان->tx_درون_flight_اندازه)
    skiplen = جریان->tx_درون_flight_اندازه;
  وإلا
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&جریان->sendbuf, skiplen);
  جریان->tx_درون_flight_اندازه -= skiplen;

  /* Resume upload processing if we have more data to send */
  إذا(جریان->tx_درون_flight_اندازه < Curl_bufq_len(&جریان->sendbuf)) {
    صحيح rv = nghttp3_conn_resume_جریان(conn, جریان_id);
    إذا(rv && rv != NGHTTP3_ERR_جریان_NOT_یافت) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  أرجع 0;
}

ساكن nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t جریان_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, فراغ *user_داده,
                                         فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية h3_جریان_ctx *جریان;
  بنية Curl_easy *داده =
    Curl_cf_ngtcp2_get_xfer(cf, جریان_user_داده, &جریان);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (فراغ)cf;
  (فراغ)conn;
  (فراغ)جریان_id;
  (فراغ)user_داده;
  (فراغ)veccnt;

  إذا(!جریان || !داده)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  إذا(جریان->tx_درون_flight_اندازه < Curl_bufq_len(&جریان->sendbuf)) {
    nvecs = 0;
    طالما(nvecs < veccnt &&
          Curl_bufq_peek_at(&جریان->sendbuf,
                            جریان->tx_درون_flight_اندازه,
                            CURL_UNCONST(&vec[nvecs].پایه),
                            &vec[nvecs].len)) {
      جریان->tx_درون_flight_اندازه += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been be able to peek */
  }

  إذا(nwritten > 0 && جریان->upload_چپ != -1)
    جریان->upload_چپ -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  إذا(جریان->upload_چپ == 0) {
    *pflags = NGHTTP3_داده_FLAG_EOF;
    جریان->send_closed = TRUE;
  }
  وإلا إذا(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] read req body -> AGAIN", جریان->id);
    أرجع NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(داده, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              جریان->id, nvecs,
              *pflags == NGHTTP3_داده_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&جریان->sendbuf),
              جریان->upload_چپ);
  أرجع (nghttp3_ssize)nvecs;
}

ساكن CURLcode h3_جریان_open(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               ثابت uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = عدم;
  int64_t sid;
  بنية dynhds h2_سرآیندها;
  size_t nheader;
  nghttp3_nv *nva = عدم;
  صحيح rc = 0;
  بیعلامت صحيح i;
  nghttp3_داده_reader reader;
  nghttp3_داده_reader *preader = عدم;
  CURLcode نتیجه;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_سرآیندها, 0, DYN_HTTP_REQUEST);

  نتیجه = Curl_cf_ngtcp2_h3_جریان_راهاندازی(cf, داده, &جریان);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = Curl_h1_req_parse_read(&جریان->h1, buf, len, عدم,
    !داده->state.http_ignorecustom ?
    CURL_EASY_STR(داده, STRING_CUSTOMREQUEST) : عدم,
    0, pnwritten);
  إذا(نتیجه)
    اذهب بیرون;
  إذا(!جریان->h1.انجامشده) {
    /* need more data */
    اذهب بیرون;
  }
  DEBUGASSERT(جریان->h1.req);

  نتیجه = Curl_http_req_مقصد_h2(&h2_سرآیندها, جریان->h1.req, داده);
  إذا(نتیجه)
    اذهب بیرون;

  /* no longer needed */
  Curl_h1_req_parse_free(&جریان->h1);

  nheader = Curl_dynhds_شمار(&h2_سرآیندها);
  nva = curlx_malloc(حجم(nghttp3_nv) * nheader);
  إذا(!nva) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  لكل(i = 0; i < nheader; ++i) {
    بنية dynhds_مدخل *e = Curl_dynhds_getn(&h2_سرآیندها, i);
    nva[i].name = (بیعلامت حرف *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].مقدار = (بیعلامت حرف *)e->مقدار;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_جریان(ctx->qconn, &sid, جریان);
  إذا(rc) {
    failf(داده, "cannot open bidi streams");
    نتیجه = CURLE_SEND_ERROR;
    اذهب بیرون;
  }
  جریان->id = sid;
  ++ctx->استفاده_شده_bidi_streams;

  بدل(داده->state.httpreq) {
  حالة HTTPREQ_POST:
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
  حالة HTTPREQ_PUT:
    /* known request body size or -1 */
    إذا(داده->state.infilesize != -1)
      جریان->upload_چپ = داده->state.infilesize;
    وإلا
      /* data sending without specifying the data amount up front */
      جریان->upload_چپ = -1; /* unknown */
    اكسر;
  افتراضي:
    /* there is no request body */
    جریان->upload_چپ = 0; /* no request body */
    اكسر;
  }

  جریان->send_closed = (جریان->upload_چپ == 0);
  إذا(!جریان->send_closed) {
    reader.read_داده = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, جریان->id,
                                   nva, nheader, preader, جریان);
  إذا(rc) {
    بدل(rc) {
    حالة NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(داده, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", جریان->id);
      اكسر;
    افتراضي:
      CURL_TRC_CF(داده, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", جریان->id, rc, nghttp3_strerror(rc));
      اكسر;
    }
    Curl_cf_ngtcp2_h3_جریان_close(cf, داده, جریان);
    نتیجه = CURLE_SEND_ERROR;
    اذهب بیرون;
  }

  cf_ngtcp2_upd_rx_win(cf, داده, جریان);

  إذا(Curl_trc_is_verbose(داده)) {
    infof(داده, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          جریان->id, Curl_bufref_ptr(&داده->state.url));
    لكل(i = 0; i < nheader; ++i) {
      infof(داده, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", جریان->id,
            (صحيح)nva[i].namelen, nva[i].name,
            (صحيح)nva[i].valuelen, nva[i].مقدار);
    }
  }

بیرون:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرآیندها);
  أرجع نتیجه;
}

ساكن CURLcode cf_ngtcp2_send(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);
  بنية cf_call_داده save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode نتیجه = CURLE_OK;

  CF_داده_SAVE(save, cf, داده);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, داده);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  إذا(ctx->tls_vrfy_نتیجه) {
    نتیجه = ctx->tls_vrfy_نتیجه;
    اذهب denied;
  }

  (فراغ)eos; /* use for stream EOF and block handling */
  نتیجه = Curl_cf_ngtcp2_پیشرفت_ingress(cf, داده, &pktx);
  إذا(نتیجه)
    اذهب بیرون;

  إذا(!جریان || جریان->id < 0) {
    إذا(ctx->shutdown_started) {
      CURL_TRC_CF(داده, cf, "cannot open stream on closed connection");
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
    نتیجه = h3_جریان_open(cf, داده, buf, len, pnwritten);
    إذا(نتیجه) {
      CURL_TRC_CF(داده, cf, "failed to open stream -> %d", (صحيح)نتیجه);
      اذهب بیرون;
    }
    جریان = H3_جریان_CTX(ctx, داده);
  }
  وإلا إذا(جریان->xfer_نتیجه) {
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] xfer write failed", جریان->id);
    Curl_cf_ngtcp2_h3_جریان_close(cf, داده, جریان);
    نتیجه = جریان->xfer_نتیجه;
    اذهب بیرون;
  }
  وإلا إذا(جریان->closed) {
    إذا(جریان->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", جریان->id);
      نتیجه = CURLE_OK;
      *pnwritten = len;
      اذهب بیرون;
    }
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", جریان->id, len);
    نتیجه = CURLE_HTTP3;
    اذهب بیرون;
  }
  وإلا إذا(ctx->shutdown_started) {
    CURL_TRC_CF(داده, cf, "cannot send on closed connection");
    نتیجه = CURLE_SEND_ERROR;
    اذهب بیرون;
  }
  وإلا إذا(جریان->tx_درون_flight_اندازه >= جریان->tx_درون_flight_ideal) {
    نتیجه = CURLE_AGAIN;
    اذهب بیرون;
  }
  وإلا {
    نتیجه = Curl_bufq_write(&جریان->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                جریان->id, len, (صحيح)نتیجه, *pnwritten);
    إذا(نتیجه)
      اذهب بیرون;
    (فراغ)nghttp3_conn_resume_جریان(ctx->h3conn, جریان->id);
  }

  إذا(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!نتیجه);
  نتیجه = Curl_cf_ngtcp2_پیشرفت_egress(cf, داده, &pktx);

  إذا(*pnwritten > 0) {
    ثابت size_t delta = (2 * 1024);
    إذا(جریان->tx_درون_flight_اندازه > (جریان->tx_درون_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      جریان->tx_درون_flight_ideal += delta;
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  جریان->id, len, *pnwritten,
                  جریان->tx_درون_flight_اندازه, جریان->tx_درون_flight_ideal);
    }
    وإلا إذا((جریان->tx_درون_flight_اندازه + delta) < جریان->tx_درون_flight_ideal) {
      /* ngtcp2 keeps less in flight than we try to provide for,
       * decrease our "ideal" tx buffer length */
      جریان->tx_درون_flight_ideal -= delta;
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, reduce tx_win=%zu",
                  جریان->id, len, *pnwritten,
                  جریان->tx_درون_flight_اندازه, جریان->tx_درون_flight_ideal);
    }
  }

بیرون:
  نتیجه = Curl_1st_fatal(نتیجه,
                          Curl_cf_ngtcp2_cmn_مجموعه_expiry(cf, داده, &pktx));
  إذا(ctx->tls_vrfy_نتیجه)
    نتیجه = ctx->tls_vrfy_نتیجه;
denied:
  CURL_TRC_CF(داده, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              جریان ? جریان->id : -1, len, (صحيح)نتیجه, *pnwritten);
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن CURLcode h3_داده_pause(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (فراغ)cf;
  إذا(!pause)
    Curl_multi_mark_dirty(داده);
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ngtcp2_cntrl(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;
  بنية cf_call_داده save;

  CF_داده_SAVE(save, cf, داده);
  بدل(event) {
  حالة CF_CTRL_داده_راهاندازی:
    اكسر;
  حالة CF_CTRL_داده_PAUSE:
    نتیجه = h3_داده_pause(cf, داده, (arg1 != 0));
    اكسر;
  حالة CF_CTRL_داده_انجامشده:
    Curl_cf_ngtcp2_h3_جریان_انجامشده(cf, داده);
    اكسر;
  حالة CF_CTRL_داده_انجامشده_SEND: {
    بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);
    إذا(جریان && !جریان->send_closed) {
      جریان->send_closed = TRUE;
      جریان->upload_چپ = Curl_bufq_len(&جریان->sendbuf) -
        جریان->tx_درون_flight_اندازه;
      (فراغ)nghttp3_conn_resume_جریان(ctx->h3conn, جریان->id);
    }
    اكسر;
  }
  حالة CF_CTRL_CONN_INFO_UPDATE:
    إذا(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_دیده = 30;
      Curl_conn_مجموعه_multiplex(cf->conn);
    }
    اكسر;
  افتراضي:
    نتیجه = Curl_cf_ngtcp2_cmn_cntrl(cf, داده, event, arg1, arg2);
    اكسر;
  }
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن فراغ cf_ngtcp2_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(داده, cf, "destroy");
  إذا(ctx) {
    إذا(ctx->qconn) {
      بنية cf_call_داده save;
      CF_داده_SAVE(save, cf, داده);
      Curl_cf_ngtcp2_cmn_conn_close(cf, داده);
      CF_داده_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_ngtcp2_connect(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  bool *انجامشده)
{
  أرجع Curl_cf_ngtcp2_cmn_connect(cf, داده, انجامشده);
}

ساكن CURLcode cf_ngtcp2_query(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_call_داده save;

  بدل(query) {
  حالة CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_داده_SAVE(save, cf, داده);
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
      إذا(ctx->max_bidi_streams > ctx->استفاده_شده_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->استفاده_شده_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > بیشترین_صحیح) ? بیشترین_صحیح : (صحيح)max_streams;
    }
    وإلا {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(داده->multi);
      *pres1 = (n < بیشترین_صحیح) ? (صحيح)n : بیشترین_صحیح;
    }
    CURL_TRC_CF(داده, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_داده_RESTORE(cf, save);
    أرجع CURLE_OK;
  }
  حالة CF_QUERY_HTTP_نسخه:
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
  أرجع Curl_cf_ngtcp2_cmn_query(cf, داده, query, pres1, pres2);
}

بنية Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_ngtcp2_destroy,
  cf_ngtcp2_connect,
  Curl_cf_ngtcp2_cmn_shutdown,
  cf_ngtcp2_adjust_pollset,
  Curl_cf_def_داده_pending,
  cf_ngtcp2_send,
  cf_ngtcp2_recv,
  cf_ngtcp2_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ngtcp2_query,
};

CURLcode Curl_cf_ngtcp2_create(بنية Curl_cfilter **pcf,
                               بنية Curl_easy *داده,
                               بنية Curl_peer *origin,
                               بنية Curl_peer *peer,
                               بنية connectdata *conn,
                               بنية Curl_sockaddr_ex *addr)
{
  بنية cf_ngtcp2_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode نتیجه;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  نتیجه = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  إذا(!نتیجه)
    نتیجه = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(نتیجه)
    اذهب بیرون;
  cf->conn = conn;

  نتیجه = Curl_cf_udp_create(&cf->بعدی, داده, origin, peer, TRNSPRT_QUIC,
                              conn, addr, عدم, TRNSPRT_QUIC);
  إذا(نتیجه)
    اذهب بیرون;
  cf->بعدی->conn = cf->conn;
  cf->بعدی->sockindex = cf->sockindex;

بیرون:
  *pcf = (!نتیجه) ? cf : عدم;
  إذا(نتیجه) {
    إذا(cf)
      Curl_conn_cf_discard_chain(&cf, داده);
    وإلا إذا(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  أرجع نتیجه;
}

CURLcode Curl_cf_ngtcp2_درج_after(بنية Curl_cfilter *cf_at,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     بنية Curl_cfilter **pcf)
{
  بنية cf_ngtcp2_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode نتیجه;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  نتیجه = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  إذا(!نتیجه)
    نتیجه = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(نتیجه)
    اذهب بیرون;
  Curl_conn_cf_درج_after(cf_at, cf);
بیرون:
  إذا(نتیجه) {
    curlx_safefree(cf);
    إذا(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  أرجع نتیجه;
}

#پایان_اگر
