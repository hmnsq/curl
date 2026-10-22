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


ساكن صحيح cb_h3_acked_req_body(nghttp3_conn *conn, int64_t بهیر_id,
                                uint64_t datalen, فراغ *user_معلومات,
                                فراغ *بهیر_user_معلومات);

ساكن CURLcode cf_ngtcp2_adjust_pollset(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات,
                                         بنية easy_pollset *ps)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode پایله = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, معلومات);

  إذا(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    أرجع CURLE_OK;

  Curl_pollset_check(معلومات, ps, sock, &want_recv, &want_send);
  إذا(!want_send && !Curl_bufq_is_تشه(&ctx->q.sendbuf))
    want_send = TRUE;

  إذا(want_recv || want_send) {
    بنية h3_بهیر_ctx *بهیر = H3_بهیر_CTX(ctx, معلومات);
    بنية cf_call_معلومات save;
    bool c_exhaust, s_exhaust;

    CF_معلومات_SAVE(save, cf, معلومات);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_کیڼ(ctx->qconn) ||
                              !ngtcp2_conn_get_max_معلومات_کیڼ(ctx->qconn));
    s_exhaust = want_send && بهیر && بهیر->id >= 0 &&
                (بهیر->quic_flow_blocked ||
                 بهیر->tx_دننه_flight_اندازه >= بهیر->tx_دننه_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_تشه(&ctx->q.sendbuf);

    پایله = Curl_pollset_سېټ(معلومات, ps, sock, want_recv, want_send);
    CF_معلومات_RESTORE(cf, save);
  }
  أرجع پایله;
}

ساكن صحيح cb_h3_بهیر_close(nghttp3_conn *conn, int64_t بهیر_id,
                              uint64_t app_error_کوډ, فراغ *user_معلومات,
                              فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية h3_بهیر_ctx *بهیر;
  بنية Curl_easy *معلومات =
    Curl_cf_ngtcp2_get_xfer(cf, بهیر_user_معلومات, &بهیر);
  (فراغ)conn;
  (فراغ)بهیر_id;

  /* we might be called by nghttp3 after we already cleaned up */
  إذا(!معلومات || !بهیر)
    أرجع 0;

  بهیر->closed = TRUE;
  بهیر->error3 = app_error_کوډ;
  إذا(بهیر->error3 != NGHTTP3_H3_NO_ERROR) {
    بهیر->reset = TRUE;
    بهیر->send_closed = TRUE;
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                بهیر->id, بهیر->error3);
  }
  وإلا {
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] CLOSED", بهیر->id);
  }
  Curl_multi_mark_dirty(معلومات);
  أرجع 0;
}

ساكن فراغ h3_xfer_write_resp_hd(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  بنية h3_بهیر_ctx *بهیر,
                                  ثابت حرف *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  إذا(!بهیر->xfer_پایله) {
    بهیر->xfer_پایله = Curl_xfer_write_resp_hd(معلومات, buf, buflen, eos);
    إذا(بهیر->xfer_پایله)
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", بهیر->id, (صحيح)بهیر->xfer_پایله,
                  buflen);
  }
}

ساكن فراغ h3_xfer_write_resp(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية h3_بهیر_ctx *بهیر,
                               ثابت حرف *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  إذا(!بهیر->xfer_پایله) {
    بهیر->xfer_پایله = Curl_xfer_write_resp(معلومات, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    إذا(بهیر->xfer_پایله) {
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  بهیر->id, (صحيح)بهیر->xfer_پایله, buflen);
    }
  }
}

ساكن فراغ cf_ngtcp2_upd_rx_win(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية h3_بهیر_ctx *بهیر)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_بهیر_کړکۍ_اندازه_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  إذا(Curl_rlimit_فعال(&معلومات->پرمختګ.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    إذا(!بهیر->rx_دننه_واټن)
      أرجع;

    avail = Curl_rlimit_avail(&معلومات->پرمختګ.dl.rlimit, عدم);
    إذا(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", بهیر->id, avail);
      أرجع;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_بهیر_کړکۍ_اندازه_MAX);
  }

  إذا(بهیر->rx_دننه_واټن_max < بهیر->rx_دننه_واټن) {
    DEBUGASSERT(0);
    أرجع;
  }
  cur_win = بهیر->rx_دننه_واټن_max - بهیر->rx_دننه_واټن;

  إذا(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    إذا(UINT64_MAX - delta < بهیر->rx_دننه_واټن_max)
      delta = UINT64_MAX - بهیر->rx_دننه_واټن_max;
    إذا(delta) {
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", بهیر->id, delta);
      بهیر->rx_دننه_واټن_max += delta;
      ngtcp2_conn_extend_max_بهیر_دننه_واټن(ctx->qconn, بهیر->id, delta);
    }
  }
}

ساكن صحيح cb_h3_recv_معلومات(nghttp3_conn *conn, int64_t stream3_id,
                           ثابت uint8_t *buf, size_t buflen,
                           فراغ *user_معلومات, فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_بهیر_ctx *بهیر;
  بنية Curl_easy *معلومات =
    Curl_cf_ngtcp2_get_xfer(cf, بهیر_user_معلومات, &بهیر);

  (فراغ)conn;
  (فراغ)stream3_id;

  إذا(!بهیر || !معلومات)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, معلومات, بهیر, (ثابت حرف *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_دننه_واټن(ctx->qconn, buflen);
  بهیر->rx_دننه_واټن += buflen;
  إذا(بهیر->rx_دننه_واټن_max < بهیر->rx_دننه_واټن)
    بهیر->rx_دننه_واټن_max = بهیر->rx_دننه_واټن;

  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              بهیر->id, buflen, بهیر->rx_دننه_واټن_max - بهیر->rx_دننه_واټن);
  cf_ngtcp2_upd_rx_win(cf, معلومات, بهیر);
  أرجع 0;
}

ساكن صحيح cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, فراغ *user_معلومات,
                                  فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_بهیر_ctx *بهیر = بهیر_user_معلومات;
  (فراغ)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_بهیر_دننه_واټن(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_دننه_واټن(ctx->qconn, consumed);

  إذا(بهیر) {
    بهیر->rx_دننه_واټن += consumed;
    بهیر->rx_دننه_واټن_max += consumed;
  }
  أرجع 0;
}

ساكن صحيح cb_h3_پای_ټکی_سرلیکونه(nghttp3_conn *conn, int64_t بهیر_id,
                             صحيح fin, فراغ *user_معلومات, فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية h3_بهیر_ctx *بهیر;
  بنية Curl_easy *معلومات =
    Curl_cf_ngtcp2_get_xfer(cf, بهیر_user_معلومات, &بهیر);
  (فراغ)conn;
  (فراغ)بهیر_id;
  (فراغ)fin;
  (فراغ)cf;

  إذا(!بهیر || !معلومات)
    أرجع 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, معلومات, بهیر, STRCONST("\r\n"),
                        (bool)بهیر->closed);

  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] end_headers, status=%d",
              بهیر_id, بهیر->حالت_کوډ);
  إذا(بهیر->حالت_کوډ / 100 != 1) {
    بهیر->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(معلومات);
  أرجع 0;
}

ساكن صحيح cb_h3_recv_سرلیک(nghttp3_conn *conn, int64_t بهیر_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *ارزښت, uint8_t flags,
                             فراغ *user_معلومات, فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(ارزښت);
  بنية h3_بهیر_ctx *بهیر;
  بنية Curl_easy *معلومات =
    Curl_cf_ngtcp2_get_xfer(cf, بهیر_user_معلومات, &بهیر);
  CURLcode پایله = CURLE_OK;
  (فراغ)conn;
  (فراغ)بهیر_id;
  (فراغ)token;
  (فراغ)flags;
  (فراغ)cf;

  /* we might have cleaned up this transfer already */
  إذا(!بهیر || !معلومات)
    أرجع 0;

  إذا(token == NGHTTP3_QPACK_TOKEN_حالت) {

    پایله = Curl_http_decode_حالت(&بهیر->حالت_کوډ,
                                     (ثابت حرف *)h3val.بنسټ, h3val.len);
    إذا(پایله)
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    پایله = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    إذا(!پایله)
      پایله = curlx_dyn_addn(&ctx->scratch,
                              (ثابت حرف *)h3val.بنسټ, h3val.len);
    إذا(!پایله)
      پایله = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    إذا(!پایله)
      h3_xfer_write_resp_hd(cf, معلومات, بهیر, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] status: %s",
                بهیر_id, curlx_dyn_ptr(&ctx->scratch));
    إذا(پایله) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  وإلا {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] header: %.*s: %.*s",
                بهیر_id, (صحيح)h3name.len, h3name.بنسټ,
                (صحيح)h3val.len, h3val.بنسټ);
    curlx_dyn_reset(&ctx->scratch);
    پایله = curlx_dyn_addn(&ctx->scratch,
                            (ثابت حرف *)h3name.بنسټ, h3name.len);
    إذا(!پایله)
      پایله = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    إذا(!پایله)
      پایله = curlx_dyn_addn(&ctx->scratch,
                              (ثابت حرف *)h3val.بنسټ, h3val.len);
    إذا(!پایله)
      پایله = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    إذا(!پایله)
      h3_xfer_write_resp_hd(cf, معلومات, بهیر, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  أرجع 0;
}

ساكن صحيح cb_h3_stop_sending(nghttp3_conn *conn, int64_t بهیر_id,
                              uint64_t app_error_کوډ, فراغ *user_معلومات,
                              فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)conn;
  (فراغ)بهیر_user_معلومات;

  rv = ngtcp2_conn_shutdown_بهیر_read(ctx->qconn, 0, بهیر_id,
                                        app_error_کوډ);
  إذا(rv && rv != NGTCP2_ERR_بهیر_NOT_وموندل) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_h3_reset_بهیر(nghttp3_conn *conn, int64_t بهیر_id,
                              uint64_t app_error_کوډ, فراغ *user_معلومات,
                              فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = Curl_cf_ngtcp2_get_xfer(cf, بهیر_user_معلومات, عدم);
  صحيح rv;
  (فراغ)conn;

  rv = ngtcp2_conn_shutdown_بهیر_write(ctx->qconn, 0, بهیر_id,
                                         app_error_کوډ);
  إذا(معلومات)
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] reset -> %d", بهیر_id, rv);
  إذا(rv && rv != NGTCP2_ERR_بهیر_NOT_وموندل) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

#که_تعریف_شوی CURL_HAVE_DIAG
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Wmissing-field-initializers"
#د_که_پای
ساكن nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_بهیر_close,
  cb_h3_recv_معلومات,
  cb_h3_deferred_consume,
  عدم, /* begin_headers */
  cb_h3_recv_سرلیک,
  cb_h3_پای_ټکی_سرلیکونه,
  عدم, /* begin_trailers */
  cb_h3_recv_سرلیک,
  عدم, /* end_trailers */
  cb_h3_stop_sending,
  عدم, /* end_stream */
  cb_h3_reset_بهیر,
  عدم, /* shutdown */
  عدم, /* recv_settings (deprecated) */
#که_تعریف_شوی NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  عدم, /* recv_origin */
  عدم, /* end_origin */
  عدم, /* rand */
#د_که_پای
#که_تعریف_شوی NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  عدم, /* recv_settings2 */
#د_که_پای
#که_تعریف_شوی NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  عدم, /* stream_close2 */
#د_که_پای
};
#که_تعریف_شوی CURL_HAVE_DIAG
#لارښوونه GCC diagnostic راوباسه
#د_که_پای

ساكن CURLcode init_ngh3_conn(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية cf_ngtcp2_ctx *ctx)
{
  صحيح rc;

  إذا(ngtcp2_conn_get_streams_uni_کیڼ(ctx->qconn) < 3) {
    failf(معلومات, "QUIC connection lacks 3 uni streams to run HTTP/3");
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  إذا(rc) {
    failf(معلومات, "error creating nghttp3 connection instance");
    أرجع CURLE_بهر_د_حافظه;
  }

  أرجع Curl_cf_ngtcp2_h3_init_ctrls(ctx, معلومات);
}

ساكن CURLcode recv_closed_بهیر(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   بنية h3_بهیر_ctx *بهیر,
                                   size_t *pnread)
{
  (فراغ)cf;
  *pnread = 0;
  إذا(بهیر->reset) {
    إذا(بهیر->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(معلومات, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", بهیر->id);
      connclose(cf->conn); /* do not use this anymore */
      معلومات->state.refused_بهیر = TRUE;
      أرجع CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وإلا إذا(بهیر->resp_hds_complete && معلومات->req.no_body) {
        CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", بهیر->id, بهیر->error3,
                    Curl_vquic_h3_err_str(بهیر->error3));
        أرجع CURLE_OK;
    }
    failf(معلومات, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", بهیر->id, بهیر->error3,
          Curl_vquic_h3_err_str(بهیر->error3));
    أرجع معلومات->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  وإلا إذا(!بهیر->resp_hds_complete) {
    failf(معلومات,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          بهیر->id);
    أرجع CURLE_HTTP3;
  }
  أرجع CURLE_OK;
}

/* incoming data frames on the h3 stream */
ساكن CURLcode cf_ngtcp2_recv(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                               حرف *buf, size_t buflen, size_t *pnread)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_بهیر_ctx *بهیر = H3_بهیر_CTX(ctx, معلومات);
  بنية cf_call_معلومات save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode پایله = CURLE_OK;
  صحيح i;

  (فراغ)ctx;
  (فراغ)buf;
  NOVERBOSE((فراغ)buflen);

  CF_معلومات_SAVE(save, cf, معلومات);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  إذا(ctx->tls_vrfy_پایله) {
    پایله = ctx->tls_vrfy_پایله;
    اذهب denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, معلومات);

  إذا(!بهیر || ctx->shutdown_started) {
    پایله = CURLE_RECV_ERROR;
    اذهب بهر;
  }

  cf_ngtcp2_upd_rx_win(cf, معلومات, بهیر);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  لكل(i = 0; i < 2; ++i) {
    إذا(بهیر->xfer_پایله) {
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] xfer write failed", بهیر->id);
      Curl_cf_ngtcp2_h3_بهیر_close(cf, معلومات, بهیر);
      پایله = بهیر->xfer_پایله;
      اذهب بهر;
    }
    وإلا إذا(بهیر->closed) {
      پایله = recv_closed_بهیر(cf, معلومات, بهیر, pnread);
      اذهب بهر;
    }

    إذا(!i && Curl_cf_ngtcp2_پرمختګ_ingress(cf, معلومات, &pktx)) {
      پایله = CURLE_RECV_ERROR;
      اذهب بهر;
    }
  }

  پایله = CURLE_AGAIN;

بهر:
  پایله = Curl_1st_fatal(پایله,
                          Curl_cf_ngtcp2_پرمختګ_egress(cf, معلومات, &pktx));
  پایله = Curl_1st_fatal(پایله,
                          Curl_cf_ngtcp2_cmn_سېټ_expiry(cf, معلومات, &pktx));
  إذا(ctx->tls_vrfy_پایله)
    پایله = ctx->tls_vrfy_پایله;
denied:
  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              بهیر ? بهیر->id : -1, buflen, (صحيح)پایله, *pnread);
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

ساكن صحيح cb_h3_acked_req_body(nghttp3_conn *conn, int64_t بهیر_id,
                                uint64_t datalen, فراغ *user_معلومات,
                                فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية h3_بهیر_ctx *بهیر = بهیر_user_معلومات;
  size_t skiplen;

  (فراغ)cf;
  إذا(!بهیر)
    أرجع 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  إذا(datalen >= (uint64_t)بهیر->tx_دننه_flight_اندازه)
    skiplen = بهیر->tx_دننه_flight_اندازه;
  وإلا
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&بهیر->sendbuf, skiplen);
  بهیر->tx_دننه_flight_اندازه -= skiplen;

  /* Resume upload processing if we have more data to send */
  إذا(بهیر->tx_دننه_flight_اندازه < Curl_bufq_len(&بهیر->sendbuf)) {
    صحيح rv = nghttp3_conn_resume_بهیر(conn, بهیر_id);
    إذا(rv && rv != NGHTTP3_ERR_بهیر_NOT_وموندل) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  أرجع 0;
}

ساكن nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t بهیر_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, فراغ *user_معلومات,
                                         فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية h3_بهیر_ctx *بهیر;
  بنية Curl_easy *معلومات =
    Curl_cf_ngtcp2_get_xfer(cf, بهیر_user_معلومات, &بهیر);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (فراغ)cf;
  (فراغ)conn;
  (فراغ)بهیر_id;
  (فراغ)user_معلومات;
  (فراغ)veccnt;

  إذا(!بهیر || !معلومات)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  إذا(بهیر->tx_دننه_flight_اندازه < Curl_bufq_len(&بهیر->sendbuf)) {
    nvecs = 0;
    طالما(nvecs < veccnt &&
          Curl_bufq_peek_at(&بهیر->sendbuf,
                            بهیر->tx_دننه_flight_اندازه,
                            CURL_UNCONST(&vec[nvecs].بنسټ),
                            &vec[nvecs].len)) {
      بهیر->tx_دننه_flight_اندازه += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been be able to peek */
  }

  إذا(nwritten > 0 && بهیر->upload_کیڼ != -1)
    بهیر->upload_کیڼ -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  إذا(بهیر->upload_کیڼ == 0) {
    *pflags = NGHTTP3_معلومات_FLAG_EOF;
    بهیر->send_closed = TRUE;
  }
  وإلا إذا(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] read req body -> AGAIN", بهیر->id);
    أرجع NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              بهیر->id, nvecs,
              *pflags == NGHTTP3_معلومات_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&بهیر->sendbuf),
              بهیر->upload_کیڼ);
  أرجع (nghttp3_ssize)nvecs;
}

ساكن CURLcode h3_بهیر_open(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               ثابت uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_بهیر_ctx *بهیر = عدم;
  int64_t sid;
  بنية dynhds h2_سرلیکونه;
  size_t nheader;
  nghttp3_nv *nva = عدم;
  صحيح rc = 0;
  بینښه صحيح i;
  nghttp3_معلومات_reader reader;
  nghttp3_معلومات_reader *preader = عدم;
  CURLcode پایله;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_سرلیکونه, 0, DYN_HTTP_REQUEST);

  پایله = Curl_cf_ngtcp2_h3_بهیر_تیاری(cf, معلومات, &بهیر);
  إذا(پایله)
    اذهب بهر;

  پایله = Curl_h1_req_parse_read(&بهیر->h1, buf, len, عدم,
    !معلومات->state.http_ignorecustom ?
    CURL_EASY_STR(معلومات, STRING_CUSTOMREQUEST) : عدم,
    0, pnwritten);
  إذا(پایله)
    اذهب بهر;
  إذا(!بهیر->h1.بشپړ) {
    /* need more data */
    اذهب بهر;
  }
  DEBUGASSERT(بهیر->h1.req);

  پایله = Curl_http_req_منزل_h2(&h2_سرلیکونه, بهیر->h1.req, معلومات);
  إذا(پایله)
    اذهب بهر;

  /* no longer needed */
  Curl_h1_req_parse_free(&بهیر->h1);

  nheader = Curl_dynhds_شمېر(&h2_سرلیکونه);
  nva = curlx_malloc(حجم(nghttp3_nv) * nheader);
  إذا(!nva) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  لكل(i = 0; i < nheader; ++i) {
    بنية dynhds_ثبت *e = Curl_dynhds_getn(&h2_سرلیکونه, i);
    nva[i].name = (بینښه حرف *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].ارزښت = (بینښه حرف *)e->ارزښت;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_بهیر(ctx->qconn, &sid, بهیر);
  إذا(rc) {
    failf(معلومات, "cannot open bidi streams");
    پایله = CURLE_SEND_ERROR;
    اذهب بهر;
  }
  بهیر->id = sid;
  ++ctx->کارول_شوی_bidi_streams;

  بدل(معلومات->state.httpreq) {
  حالة HTTPREQ_POST:
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
  حالة HTTPREQ_PUT:
    /* known request body size or -1 */
    إذا(معلومات->state.infilesize != -1)
      بهیر->upload_کیڼ = معلومات->state.infilesize;
    وإلا
      /* data sending without specifying the data amount up front */
      بهیر->upload_کیڼ = -1; /* unknown */
    اكسر;
  افتراضي:
    /* there is no request body */
    بهیر->upload_کیڼ = 0; /* no request body */
    اكسر;
  }

  بهیر->send_closed = (بهیر->upload_کیڼ == 0);
  إذا(!بهیر->send_closed) {
    reader.read_معلومات = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, بهیر->id,
                                   nva, nheader, preader, بهیر);
  إذا(rc) {
    بدل(rc) {
    حالة NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(معلومات, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", بهیر->id);
      اكسر;
    افتراضي:
      CURL_TRC_CF(معلومات, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", بهیر->id, rc, nghttp3_strerror(rc));
      اكسر;
    }
    Curl_cf_ngtcp2_h3_بهیر_close(cf, معلومات, بهیر);
    پایله = CURLE_SEND_ERROR;
    اذهب بهر;
  }

  cf_ngtcp2_upd_rx_win(cf, معلومات, بهیر);

  إذا(Curl_trc_is_verbose(معلومات)) {
    infof(معلومات, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          بهیر->id, Curl_bufref_ptr(&معلومات->state.url));
    لكل(i = 0; i < nheader; ++i) {
      infof(معلومات, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", بهیر->id,
            (صحيح)nva[i].namelen, nva[i].name,
            (صحيح)nva[i].valuelen, nva[i].ارزښت);
    }
  }

بهر:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرلیکونه);
  أرجع پایله;
}

ساكن CURLcode cf_ngtcp2_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_بهیر_ctx *بهیر = H3_بهیر_CTX(ctx, معلومات);
  بنية cf_call_معلومات save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode پایله = CURLE_OK;

  CF_معلومات_SAVE(save, cf, معلومات);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, معلومات);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  إذا(ctx->tls_vrfy_پایله) {
    پایله = ctx->tls_vrfy_پایله;
    اذهب denied;
  }

  (فراغ)eos; /* use for stream EOF and block handling */
  پایله = Curl_cf_ngtcp2_پرمختګ_ingress(cf, معلومات, &pktx);
  إذا(پایله)
    اذهب بهر;

  إذا(!بهیر || بهیر->id < 0) {
    إذا(ctx->shutdown_started) {
      CURL_TRC_CF(معلومات, cf, "cannot open stream on closed connection");
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }
    پایله = h3_بهیر_open(cf, معلومات, buf, len, pnwritten);
    إذا(پایله) {
      CURL_TRC_CF(معلومات, cf, "failed to open stream -> %d", (صحيح)پایله);
      اذهب بهر;
    }
    بهیر = H3_بهیر_CTX(ctx, معلومات);
  }
  وإلا إذا(بهیر->xfer_پایله) {
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] xfer write failed", بهیر->id);
    Curl_cf_ngtcp2_h3_بهیر_close(cf, معلومات, بهیر);
    پایله = بهیر->xfer_پایله;
    اذهب بهر;
  }
  وإلا إذا(بهیر->closed) {
    إذا(بهیر->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", بهیر->id);
      پایله = CURLE_OK;
      *pnwritten = len;
      اذهب بهر;
    }
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", بهیر->id, len);
    پایله = CURLE_HTTP3;
    اذهب بهر;
  }
  وإلا إذا(ctx->shutdown_started) {
    CURL_TRC_CF(معلومات, cf, "cannot send on closed connection");
    پایله = CURLE_SEND_ERROR;
    اذهب بهر;
  }
  وإلا إذا(بهیر->tx_دننه_flight_اندازه >= بهیر->tx_دننه_flight_ideal) {
    پایله = CURLE_AGAIN;
    اذهب بهر;
  }
  وإلا {
    پایله = Curl_bufq_write(&بهیر->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                بهیر->id, len, (صحيح)پایله, *pnwritten);
    إذا(پایله)
      اذهب بهر;
    (فراغ)nghttp3_conn_resume_بهیر(ctx->h3conn, بهیر->id);
  }

  إذا(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!پایله);
  پایله = Curl_cf_ngtcp2_پرمختګ_egress(cf, معلومات, &pktx);

  إذا(*pnwritten > 0) {
    ثابت size_t delta = (2 * 1024);
    إذا(بهیر->tx_دننه_flight_اندازه > (بهیر->tx_دننه_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      بهیر->tx_دننه_flight_ideal += delta;
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  بهیر->id, len, *pnwritten,
                  بهیر->tx_دننه_flight_اندازه, بهیر->tx_دننه_flight_ideal);
    }
    وإلا إذا((بهیر->tx_دننه_flight_اندازه + delta) < بهیر->tx_دننه_flight_ideal) {
      /* ngtcp2 keeps less in flight than we try to provide for,
       * decrease our "ideal" tx buffer length */
      بهیر->tx_دننه_flight_ideal -= delta;
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, reduce tx_win=%zu",
                  بهیر->id, len, *pnwritten,
                  بهیر->tx_دننه_flight_اندازه, بهیر->tx_دننه_flight_ideal);
    }
  }

بهر:
  پایله = Curl_1st_fatal(پایله,
                          Curl_cf_ngtcp2_cmn_سېټ_expiry(cf, معلومات, &pktx));
  إذا(ctx->tls_vrfy_پایله)
    پایله = ctx->tls_vrfy_پایله;
denied:
  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              بهیر ? بهیر->id : -1, len, (صحيح)پایله, *pnwritten);
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

ساكن CURLcode h3_معلومات_pause(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (فراغ)cf;
  إذا(!pause)
    Curl_multi_mark_dirty(معلومات);
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ngtcp2_cntrl(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;
  بنية cf_call_معلومات save;

  CF_معلومات_SAVE(save, cf, معلومات);
  بدل(event) {
  حالة CF_CTRL_معلومات_تیاری:
    اكسر;
  حالة CF_CTRL_معلومات_PAUSE:
    پایله = h3_معلومات_pause(cf, معلومات, (arg1 != 0));
    اكسر;
  حالة CF_CTRL_معلومات_بشپړ:
    Curl_cf_ngtcp2_h3_بهیر_بشپړ(cf, معلومات);
    اكسر;
  حالة CF_CTRL_معلومات_بشپړ_SEND: {
    بنية h3_بهیر_ctx *بهیر = H3_بهیر_CTX(ctx, معلومات);
    إذا(بهیر && !بهیر->send_closed) {
      بهیر->send_closed = TRUE;
      بهیر->upload_کیڼ = Curl_bufq_len(&بهیر->sendbuf) -
        بهیر->tx_دننه_flight_اندازه;
      (فراغ)nghttp3_conn_resume_بهیر(ctx->h3conn, بهیر->id);
    }
    اكسر;
  }
  حالة CF_CTRL_CONN_INFO_UPDATE:
    إذا(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_کتل_شوی = 30;
      Curl_conn_سېټ_multiplex(cf->conn);
    }
    اكسر;
  افتراضي:
    پایله = Curl_cf_ngtcp2_cmn_cntrl(cf, معلومات, event, arg1, arg2);
    اكسر;
  }
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

ساكن فراغ cf_ngtcp2_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(معلومات, cf, "destroy");
  إذا(ctx) {
    إذا(ctx->qconn) {
      بنية cf_call_معلومات save;
      CF_معلومات_SAVE(save, cf, معلومات);
      Curl_cf_ngtcp2_cmn_conn_close(cf, معلومات);
      CF_معلومات_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_ngtcp2_connect(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  bool *بشپړ)
{
  أرجع Curl_cf_ngtcp2_cmn_connect(cf, معلومات, بشپړ);
}

ساكن CURLcode cf_ngtcp2_query(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;

  بدل(query) {
  حالة CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_معلومات_SAVE(save, cf, معلومات);
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
      إذا(ctx->max_bidi_streams > ctx->کارول_شوی_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->کارول_شوی_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > لوی_عدد) ? لوی_عدد : (صحيح)max_streams;
    }
    وإلا {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(معلومات->multi);
      *pres1 = (n < لوی_عدد) ? (صحيح)n : لوی_عدد;
    }
    CURL_TRC_CF(معلومات, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_معلومات_RESTORE(cf, save);
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
  أرجع Curl_cf_ngtcp2_cmn_query(cf, معلومات, query, pres1, pres2);
}

بنية Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_ngtcp2_destroy,
  cf_ngtcp2_connect,
  Curl_cf_ngtcp2_cmn_shutdown,
  cf_ngtcp2_adjust_pollset,
  Curl_cf_def_معلومات_pending,
  cf_ngtcp2_send,
  cf_ngtcp2_recv,
  cf_ngtcp2_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ngtcp2_query,
};

CURLcode Curl_cf_ngtcp2_create(بنية Curl_cfilter **pcf,
                               بنية Curl_easy *معلومات,
                               بنية Curl_peer *origin,
                               بنية Curl_peer *peer,
                               بنية connectdata *conn,
                               بنية Curl_sockaddr_ex *addr)
{
  بنية cf_ngtcp2_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode پایله;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }
  پایله = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  إذا(!پایله)
    پایله = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(پایله)
    اذهب بهر;
  cf->conn = conn;

  پایله = Curl_cf_udp_create(&cf->بل, معلومات, origin, peer, TRNSPRT_QUIC,
                              conn, addr, عدم, TRNSPRT_QUIC);
  إذا(پایله)
    اذهب بهر;
  cf->بل->conn = cf->conn;
  cf->بل->sockindex = cf->sockindex;

بهر:
  *pcf = (!پایله) ? cf : عدم;
  إذا(پایله) {
    إذا(cf)
      Curl_conn_cf_discard_chain(&cf, معلومات);
    وإلا إذا(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  أرجع پایله;
}

CURLcode Curl_cf_ngtcp2_ورننباسه_after(بنية Curl_cfilter *cf_at,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     بنية Curl_cfilter **pcf)
{
  بنية cf_ngtcp2_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode پایله;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }
  پایله = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  إذا(!پایله)
    پایله = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(پایله)
    اذهب بهر;
  Curl_conn_cf_ورننباسه_after(cf_at, cf);
بهر:
  إذا(پایله) {
    curlx_safefree(cf);
    إذا(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  أرجع پایله;
}

#د_که_پای
