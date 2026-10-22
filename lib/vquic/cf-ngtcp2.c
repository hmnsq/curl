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


ساكن صحيح cb_h3_acked_req_body(nghttp3_conn *conn, int64_t دھارا_id,
                                uint64_t datalen, فراغ *user_انکڑے,
                                فراغ *دھارا_user_انکڑے);

ساكن CURLcode cf_ngtcp2_adjust_pollset(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *انکڑے,
                                         بنية easy_pollset *ps)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode نتیجہ = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, انکڑے);

  إذا(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    أرجع CURLE_OK;

  Curl_pollset_check(انکڑے, ps, sock, &want_recv, &want_send);
  إذا(!want_send && !Curl_bufq_is_خالی_تھاواں(&ctx->q.sendbuf))
    want_send = TRUE;

  إذا(want_recv || want_send) {
    بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);
    بنية cf_call_انکڑے save;
    bool c_exhaust, s_exhaust;

    CF_انکڑے_SAVE(save, cf, انکڑے);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_بایاں(ctx->qconn) ||
                              !ngtcp2_conn_get_max_انکڑے_بایاں(ctx->qconn));
    s_exhaust = want_send && دھارا && دھارا->id >= 0 &&
                (دھارا->quic_flow_blocked ||
                 دھارا->tx_وچ_flight_ناپ >= دھارا->tx_وچ_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_خالی_تھاواں(&ctx->q.sendbuf);

    نتیجہ = Curl_pollset_سیٹ(انکڑے, ps, sock, want_recv, want_send);
    CF_انکڑے_RESTORE(cf, save);
  }
  أرجع نتیجہ;
}

ساكن صحيح cb_h3_دھارا_close(nghttp3_conn *conn, int64_t دھارا_id,
                              uint64_t app_error_کوڈ, فراغ *user_انکڑے,
                              فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية h3_دھارا_ctx *دھارا;
  بنية Curl_easy *انکڑے =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_انکڑے, &دھارا);
  (فراغ)conn;
  (فراغ)دھارا_id;

  /* we might be called by nghttp3 after we already cleaned up */
  إذا(!انکڑے || !دھارا)
    أرجع 0;

  دھارا->closed = TRUE;
  دھارا->error3 = app_error_کوڈ;
  إذا(دھارا->error3 != NGHTTP3_H3_NO_ERROR) {
    دھارا->reset = TRUE;
    دھارا->send_closed = TRUE;
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                دھارا->id, دھارا->error3);
  }
  وإلا {
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] CLOSED", دھارا->id);
  }
  Curl_multi_mark_dirty(انکڑے);
  أرجع 0;
}

ساكن فراغ h3_xfer_write_resp_hd(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  بنية h3_دھارا_ctx *دھارا,
                                  ثابت حرف *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  إذا(!دھارا->xfer_نتیجہ) {
    دھارا->xfer_نتیجہ = Curl_xfer_write_resp_hd(انکڑے, buf, buflen, eos);
    إذا(دھارا->xfer_نتیجہ)
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", دھارا->id, (صحيح)دھارا->xfer_نتیجہ,
                  buflen);
  }
}

ساكن فراغ h3_xfer_write_resp(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے,
                               بنية h3_دھارا_ctx *دھارا,
                               ثابت حرف *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  إذا(!دھارا->xfer_نتیجہ) {
    دھارا->xfer_نتیجہ = Curl_xfer_write_resp(انکڑے, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    إذا(دھارا->xfer_نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  دھارا->id, (صحيح)دھارا->xfer_نتیجہ, buflen);
    }
  }
}

ساكن فراغ cf_ngtcp2_upd_rx_win(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 بنية h3_دھارا_ctx *دھارا)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_دھارا_کھڑکی_ناپ_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  إذا(Curl_rlimit_سرگرم(&انکڑے->ترقی.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    إذا(!دھارا->rx_وتھ)
      أرجع;

    avail = Curl_rlimit_avail(&انکڑے->ترقی.dl.rlimit, عدم);
    إذا(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", دھارا->id, avail);
      أرجع;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_دھارا_کھڑکی_ناپ_MAX);
  }

  إذا(دھارا->rx_وتھ_max < دھارا->rx_وتھ) {
    DEBUGASSERT(0);
    أرجع;
  }
  cur_win = دھارا->rx_وتھ_max - دھارا->rx_وتھ;

  إذا(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    إذا(UINT64_MAX - delta < دھارا->rx_وتھ_max)
      delta = UINT64_MAX - دھارا->rx_وتھ_max;
    إذا(delta) {
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", دھارا->id, delta);
      دھارا->rx_وتھ_max += delta;
      ngtcp2_conn_extend_max_دھارا_وتھ(ctx->qconn, دھارا->id, delta);
    }
  }
}

ساكن صحيح cb_h3_recv_انکڑے(nghttp3_conn *conn, int64_t stream3_id,
                           ثابت uint8_t *buf, size_t buflen,
                           فراغ *user_انکڑے, فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا;
  بنية Curl_easy *انکڑے =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_انکڑے, &دھارا);

  (فراغ)conn;
  (فراغ)stream3_id;

  إذا(!دھارا || !انکڑے)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, انکڑے, دھارا, (ثابت حرف *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_وتھ(ctx->qconn, buflen);
  دھارا->rx_وتھ += buflen;
  إذا(دھارا->rx_وتھ_max < دھارا->rx_وتھ)
    دھارا->rx_وتھ_max = دھارا->rx_وتھ;

  CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              دھارا->id, buflen, دھارا->rx_وتھ_max - دھارا->rx_وتھ);
  cf_ngtcp2_upd_rx_win(cf, انکڑے, دھارا);
  أرجع 0;
}

ساكن صحيح cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, فراغ *user_انکڑے,
                                  فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = دھارا_user_انکڑے;
  (فراغ)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_دھارا_وتھ(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_وتھ(ctx->qconn, consumed);

  إذا(دھارا) {
    دھارا->rx_وتھ += consumed;
    دھارا->rx_وتھ_max += consumed;
  }
  أرجع 0;
}

ساكن صحيح cb_h3_سرا_سرخیاں(nghttp3_conn *conn, int64_t دھارا_id,
                             صحيح fin, فراغ *user_انکڑے, فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية h3_دھارا_ctx *دھارا;
  بنية Curl_easy *انکڑے =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_انکڑے, &دھارا);
  (فراغ)conn;
  (فراغ)دھارا_id;
  (فراغ)fin;
  (فراغ)cf;

  إذا(!دھارا || !انکڑے)
    أرجع 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, انکڑے, دھارا, STRCONST("\r\n"),
                        (bool)دھارا->closed);

  CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] end_headers, status=%d",
              دھارا_id, دھارا->حالت_کوڈ);
  إذا(دھارا->حالت_کوڈ / 100 != 1) {
    دھارا->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(انکڑے);
  أرجع 0;
}

ساكن صحيح cb_h3_recv_سرخی(nghttp3_conn *conn, int64_t دھارا_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *قدر, uint8_t flags,
                             فراغ *user_انکڑے, فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(قدر);
  بنية h3_دھارا_ctx *دھارا;
  بنية Curl_easy *انکڑے =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_انکڑے, &دھارا);
  CURLcode نتیجہ = CURLE_OK;
  (فراغ)conn;
  (فراغ)دھارا_id;
  (فراغ)token;
  (فراغ)flags;
  (فراغ)cf;

  /* we might have cleaned up this transfer already */
  إذا(!دھارا || !انکڑے)
    أرجع 0;

  إذا(token == NGHTTP3_QPACK_TOKEN_حالت) {

    نتیجہ = Curl_http_decode_حالت(&دھارا->حالت_کوڈ,
                                     (ثابت حرف *)h3val.ادھار, h3val.len);
    إذا(نتیجہ)
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch,
                              (ثابت حرف *)h3val.ادھار, h3val.len);
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    إذا(!نتیجہ)
      h3_xfer_write_resp_hd(cf, انکڑے, دھارا, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] status: %s",
                دھارا_id, curlx_dyn_ptr(&ctx->scratch));
    إذا(نتیجہ) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  وإلا {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] header: %.*s: %.*s",
                دھارا_id, (صحيح)h3name.len, h3name.ادھار,
                (صحيح)h3val.len, h3val.ادھار);
    curlx_dyn_reset(&ctx->scratch);
    نتیجہ = curlx_dyn_addn(&ctx->scratch,
                            (ثابت حرف *)h3name.ادھار, h3name.len);
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch,
                              (ثابت حرف *)h3val.ادھار, h3val.len);
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    إذا(!نتیجہ)
      h3_xfer_write_resp_hd(cf, انکڑے, دھارا, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  أرجع 0;
}

ساكن صحيح cb_h3_stop_sending(nghttp3_conn *conn, int64_t دھارا_id,
                              uint64_t app_error_کوڈ, فراغ *user_انکڑے,
                              فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  صحيح rv;
  (فراغ)conn;
  (فراغ)دھارا_user_انکڑے;

  rv = ngtcp2_conn_shutdown_دھارا_read(ctx->qconn, 0, دھارا_id,
                                        app_error_کوڈ);
  إذا(rv && rv != NGTCP2_ERR_دھارا_NOT_لبھیا) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

ساكن صحيح cb_h3_reset_دھارا(nghttp3_conn *conn, int64_t دھارا_id,
                              uint64_t app_error_کوڈ, فراغ *user_انکڑے,
                              فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے = Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_انکڑے, عدم);
  صحيح rv;
  (فراغ)conn;

  rv = ngtcp2_conn_shutdown_دھارا_write(ctx->qconn, 0, دھارا_id,
                                         app_error_کوڈ);
  إذا(انکڑے)
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] reset -> %d", دھارا_id, rv);
  إذا(rv && rv != NGTCP2_ERR_دھارا_NOT_لبھیا) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  أرجع 0;
}

#جے_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wmissing-field-initializers"
#جے_انت
ساكن nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_دھارا_close,
  cb_h3_recv_انکڑے,
  cb_h3_deferred_consume,
  عدم, /* begin_headers */
  cb_h3_recv_سرخی,
  cb_h3_سرا_سرخیاں,
  عدم, /* begin_trailers */
  cb_h3_recv_سرخی,
  عدم, /* end_trailers */
  cb_h3_stop_sending,
  عدم, /* end_stream */
  cb_h3_reset_دھارا,
  عدم, /* shutdown */
  عدم, /* recv_settings (deprecated) */
#جے_تعریف_شدہ NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  عدم, /* recv_origin */
  عدم, /* end_origin */
  عدم, /* rand */
#جے_انت
#جے_تعریف_شدہ NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  عدم, /* recv_settings2 */
#جے_انت
#جے_تعریف_شدہ NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  عدم, /* stream_close2 */
#جے_انت
};
#جے_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic نکالو
#جے_انت

ساكن CURLcode init_ngh3_conn(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے,
                               بنية cf_ngtcp2_ctx *ctx)
{
  صحيح rc;

  إذا(ngtcp2_conn_get_streams_uni_بایاں(ctx->qconn) < 3) {
    failf(انکڑے, "QUIC connection lacks 3 uni streams to run HTTP/3");
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  إذا(rc) {
    failf(انکڑے, "error creating nghttp3 connection instance");
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  أرجع Curl_cf_ngtcp2_h3_init_ctrls(ctx, انکڑے);
}

ساكن CURLcode recv_closed_دھارا(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے,
                                   بنية h3_دھارا_ctx *دھارا,
                                   size_t *pnread)
{
  (فراغ)cf;
  *pnread = 0;
  إذا(دھارا->reset) {
    إذا(دھارا->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(انکڑے, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", دھارا->id);
      connclose(cf->conn); /* do not use this anymore */
      انکڑے->state.refused_دھارا = TRUE;
      أرجع CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وإلا إذا(دھارا->resp_hds_complete && انکڑے->req.no_body) {
        CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", دھارا->id, دھارا->error3,
                    Curl_vquic_h3_err_str(دھارا->error3));
        أرجع CURLE_OK;
    }
    failf(انکڑے, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", دھارا->id, دھارا->error3,
          Curl_vquic_h3_err_str(دھارا->error3));
    أرجع انکڑے->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  وإلا إذا(!دھارا->resp_hds_complete) {
    failf(انکڑے,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          دھارا->id);
    أرجع CURLE_HTTP3;
  }
  أرجع CURLE_OK;
}

/* incoming data frames on the h3 stream */
ساكن CURLcode cf_ngtcp2_recv(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                               حرف *buf, size_t buflen, size_t *pnread)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);
  بنية cf_call_انکڑے save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode نتیجہ = CURLE_OK;
  صحيح i;

  (فراغ)ctx;
  (فراغ)buf;
  NOVERBOSE((فراغ)buflen);

  CF_انکڑے_SAVE(save, cf, انکڑے);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  إذا(ctx->tls_vrfy_نتیجہ) {
    نتیجہ = ctx->tls_vrfy_نتیجہ;
    اذهب denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, انکڑے);

  إذا(!دھارا || ctx->shutdown_started) {
    نتیجہ = CURLE_RECV_ERROR;
    اذهب باہرلا;
  }

  cf_ngtcp2_upd_rx_win(cf, انکڑے, دھارا);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  لكل(i = 0; i < 2; ++i) {
    إذا(دھارا->xfer_نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] xfer write failed", دھارا->id);
      Curl_cf_ngtcp2_h3_دھارا_close(cf, انکڑے, دھارا);
      نتیجہ = دھارا->xfer_نتیجہ;
      اذهب باہرلا;
    }
    وإلا إذا(دھارا->closed) {
      نتیجہ = recv_closed_دھارا(cf, انکڑے, دھارا, pnread);
      اذهب باہرلا;
    }

    إذا(!i && Curl_cf_ngtcp2_ترقی_ingress(cf, انکڑے, &pktx)) {
      نتیجہ = CURLE_RECV_ERROR;
      اذهب باہرلا;
    }
  }

  نتیجہ = CURLE_AGAIN;

باہرلا:
  نتیجہ = Curl_1st_fatal(نتیجہ,
                          Curl_cf_ngtcp2_ترقی_egress(cf, انکڑے, &pktx));
  نتیجہ = Curl_1st_fatal(نتیجہ,
                          Curl_cf_ngtcp2_cmn_سیٹ_expiry(cf, انکڑے, &pktx));
  إذا(ctx->tls_vrfy_نتیجہ)
    نتیجہ = ctx->tls_vrfy_نتیجہ;
denied:
  CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              دھارا ? دھارا->id : -1, buflen, (صحيح)نتیجہ, *pnread);
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن صحيح cb_h3_acked_req_body(nghttp3_conn *conn, int64_t دھارا_id,
                                uint64_t datalen, فراغ *user_انکڑے,
                                فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية h3_دھارا_ctx *دھارا = دھارا_user_انکڑے;
  size_t skiplen;

  (فراغ)cf;
  إذا(!دھارا)
    أرجع 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  إذا(datalen >= (uint64_t)دھارا->tx_وچ_flight_ناپ)
    skiplen = دھارا->tx_وچ_flight_ناپ;
  وإلا
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&دھارا->sendbuf, skiplen);
  دھارا->tx_وچ_flight_ناپ -= skiplen;

  /* Resume upload processing if we have more data to send */
  إذا(دھارا->tx_وچ_flight_ناپ < Curl_bufq_len(&دھارا->sendbuf)) {
    صحيح rv = nghttp3_conn_resume_دھارا(conn, دھارا_id);
    إذا(rv && rv != NGHTTP3_ERR_دھارا_NOT_لبھیا) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  أرجع 0;
}

ساكن nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t دھارا_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, فراغ *user_انکڑے,
                                         فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية h3_دھارا_ctx *دھارا;
  بنية Curl_easy *انکڑے =
    Curl_cf_ngtcp2_get_xfer(cf, دھارا_user_انکڑے, &دھارا);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (فراغ)cf;
  (فراغ)conn;
  (فراغ)دھارا_id;
  (فراغ)user_انکڑے;
  (فراغ)veccnt;

  إذا(!دھارا || !انکڑے)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  إذا(دھارا->tx_وچ_flight_ناپ < Curl_bufq_len(&دھارا->sendbuf)) {
    nvecs = 0;
    طالما(nvecs < veccnt &&
          Curl_bufq_peek_at(&دھارا->sendbuf,
                            دھارا->tx_وچ_flight_ناپ,
                            CURL_UNCONST(&vec[nvecs].ادھار),
                            &vec[nvecs].len)) {
      دھارا->tx_وچ_flight_ناپ += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been be able to peek */
  }

  إذا(nwritten > 0 && دھارا->upload_بایاں != -1)
    دھارا->upload_بایاں -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  إذا(دھارا->upload_بایاں == 0) {
    *pflags = NGHTTP3_انکڑے_FLAG_EOF;
    دھارا->send_closed = TRUE;
  }
  وإلا إذا(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] read req body -> AGAIN", دھارا->id);
    أرجع NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              دھارا->id, nvecs,
              *pflags == NGHTTP3_انکڑے_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&دھارا->sendbuf),
              دھارا->upload_بایاں);
  أرجع (nghttp3_ssize)nvecs;
}

ساكن CURLcode h3_دھارا_open(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے,
                               ثابت uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = عدم;
  int64_t sid;
  بنية dynhds h2_سرخیاں;
  size_t nheader;
  nghttp3_nv *nva = عدم;
  صحيح rc = 0;
  بنانشانی صحيح i;
  nghttp3_انکڑے_reader reader;
  nghttp3_انکڑے_reader *preader = عدم;
  CURLcode نتیجہ;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_سرخیاں, 0, DYN_HTTP_REQUEST);

  نتیجہ = Curl_cf_ngtcp2_h3_دھارا_تیاری(cf, انکڑے, &دھارا);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = Curl_h1_req_parse_read(&دھارا->h1, buf, len, عدم,
    !انکڑے->state.http_ignorecustom ?
    CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST) : عدم,
    0, pnwritten);
  إذا(نتیجہ)
    اذهب باہرلا;
  إذا(!دھارا->h1.پورا) {
    /* need more data */
    اذهب باہرلا;
  }
  DEBUGASSERT(دھارا->h1.req);

  نتیجہ = Curl_http_req_منزل_h2(&h2_سرخیاں, دھارا->h1.req, انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* no longer needed */
  Curl_h1_req_parse_free(&دھارا->h1);

  nheader = Curl_dynhds_گنتی(&h2_سرخیاں);
  nva = curlx_malloc(حجم(nghttp3_nv) * nheader);
  إذا(!nva) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  لكل(i = 0; i < nheader; ++i) {
    بنية dynhds_اندراج *e = Curl_dynhds_getn(&h2_سرخیاں, i);
    nva[i].name = (بنانشانی حرف *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].قدر = (بنانشانی حرف *)e->قدر;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_دھارا(ctx->qconn, &sid, دھارا);
  إذا(rc) {
    failf(انکڑے, "cannot open bidi streams");
    نتیجہ = CURLE_SEND_ERROR;
    اذهب باہرلا;
  }
  دھارا->id = sid;
  ++ctx->ورتیا_bidi_streams;

  بدل(انکڑے->state.httpreq) {
  حالة HTTPREQ_POST:
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
  حالة HTTPREQ_PUT:
    /* known request body size or -1 */
    إذا(انکڑے->state.infilesize != -1)
      دھارا->upload_بایاں = انکڑے->state.infilesize;
    وإلا
      /* data sending without specifying the data amount up front */
      دھارا->upload_بایاں = -1; /* unknown */
    اكسر;
  افتراضي:
    /* there is no request body */
    دھارا->upload_بایاں = 0; /* no request body */
    اكسر;
  }

  دھارا->send_closed = (دھارا->upload_بایاں == 0);
  إذا(!دھارا->send_closed) {
    reader.read_انکڑے = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, دھارا->id,
                                   nva, nheader, preader, دھارا);
  إذا(rc) {
    بدل(rc) {
    حالة NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(انکڑے, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", دھارا->id);
      اكسر;
    افتراضي:
      CURL_TRC_CF(انکڑے, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", دھارا->id, rc, nghttp3_strerror(rc));
      اكسر;
    }
    Curl_cf_ngtcp2_h3_دھارا_close(cf, انکڑے, دھارا);
    نتیجہ = CURLE_SEND_ERROR;
    اذهب باہرلا;
  }

  cf_ngtcp2_upd_rx_win(cf, انکڑے, دھارا);

  إذا(Curl_trc_is_verbose(انکڑے)) {
    infof(انکڑے, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          دھارا->id, Curl_bufref_ptr(&انکڑے->state.url));
    لكل(i = 0; i < nheader; ++i) {
      infof(انکڑے, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", دھارا->id,
            (صحيح)nva[i].namelen, nva[i].name,
            (صحيح)nva[i].valuelen, nva[i].قدر);
    }
  }

باہرلا:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرخیاں);
  أرجع نتیجہ;
}

ساكن CURLcode cf_ngtcp2_send(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);
  بنية cf_call_انکڑے save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode نتیجہ = CURLE_OK;

  CF_انکڑے_SAVE(save, cf, انکڑے);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, انکڑے);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  إذا(ctx->tls_vrfy_نتیجہ) {
    نتیجہ = ctx->tls_vrfy_نتیجہ;
    اذهب denied;
  }

  (فراغ)eos; /* use for stream EOF and block handling */
  نتیجہ = Curl_cf_ngtcp2_ترقی_ingress(cf, انکڑے, &pktx);
  إذا(نتیجہ)
    اذهب باہرلا;

  إذا(!دھارا || دھارا->id < 0) {
    إذا(ctx->shutdown_started) {
      CURL_TRC_CF(انکڑے, cf, "cannot open stream on closed connection");
      نتیجہ = CURLE_SEND_ERROR;
      اذهب باہرلا;
    }
    نتیجہ = h3_دھارا_open(cf, انکڑے, buf, len, pnwritten);
    إذا(نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "failed to open stream -> %d", (صحيح)نتیجہ);
      اذهب باہرلا;
    }
    دھارا = H3_دھارا_CTX(ctx, انکڑے);
  }
  وإلا إذا(دھارا->xfer_نتیجہ) {
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] xfer write failed", دھارا->id);
    Curl_cf_ngtcp2_h3_دھارا_close(cf, انکڑے, دھارا);
    نتیجہ = دھارا->xfer_نتیجہ;
    اذهب باہرلا;
  }
  وإلا إذا(دھارا->closed) {
    إذا(دھارا->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", دھارا->id);
      نتیجہ = CURLE_OK;
      *pnwritten = len;
      اذهب باہرلا;
    }
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", دھارا->id, len);
    نتیجہ = CURLE_HTTP3;
    اذهب باہرلا;
  }
  وإلا إذا(ctx->shutdown_started) {
    CURL_TRC_CF(انکڑے, cf, "cannot send on closed connection");
    نتیجہ = CURLE_SEND_ERROR;
    اذهب باہرلا;
  }
  وإلا إذا(دھارا->tx_وچ_flight_ناپ >= دھارا->tx_وچ_flight_ideal) {
    نتیجہ = CURLE_AGAIN;
    اذهب باہرلا;
  }
  وإلا {
    نتیجہ = Curl_bufq_write(&دھارا->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                دھارا->id, len, (صحيح)نتیجہ, *pnwritten);
    إذا(نتیجہ)
      اذهب باہرلا;
    (فراغ)nghttp3_conn_resume_دھارا(ctx->h3conn, دھارا->id);
  }

  إذا(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!نتیجہ);
  نتیجہ = Curl_cf_ngtcp2_ترقی_egress(cf, انکڑے, &pktx);

  إذا(*pnwritten > 0) {
    ثابت size_t delta = (2 * 1024);
    إذا(دھارا->tx_وچ_flight_ناپ > (دھارا->tx_وچ_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      دھارا->tx_وچ_flight_ideal += delta;
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  دھارا->id, len, *pnwritten,
                  دھارا->tx_وچ_flight_ناپ, دھارا->tx_وچ_flight_ideal);
    }
    وإلا إذا((دھارا->tx_وچ_flight_ناپ + delta) < دھارا->tx_وچ_flight_ideal) {
      /* ngtcp2 keeps less in flight than we try to provide for,
       * decrease our "ideal" tx buffer length */
      دھارا->tx_وچ_flight_ideal -= delta;
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, reduce tx_win=%zu",
                  دھارا->id, len, *pnwritten,
                  دھارا->tx_وچ_flight_ناپ, دھارا->tx_وچ_flight_ideal);
    }
  }

باہرلا:
  نتیجہ = Curl_1st_fatal(نتیجہ,
                          Curl_cf_ngtcp2_cmn_سیٹ_expiry(cf, انکڑے, &pktx));
  إذا(ctx->tls_vrfy_نتیجہ)
    نتیجہ = ctx->tls_vrfy_نتیجہ;
denied:
  CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              دھارا ? دھارا->id : -1, len, (صحيح)نتیجہ, *pnwritten);
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن CURLcode h3_انکڑے_pause(بنية Curl_cfilter *cf,
                              بنية Curl_easy *انکڑے,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (فراغ)cf;
  إذا(!pause)
    Curl_multi_mark_dirty(انکڑے);
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ngtcp2_cntrl(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  بنية cf_call_انکڑے save;

  CF_انکڑے_SAVE(save, cf, انکڑے);
  بدل(event) {
  حالة CF_CTRL_انکڑے_تیاری:
    اكسر;
  حالة CF_CTRL_انکڑے_PAUSE:
    نتیجہ = h3_انکڑے_pause(cf, انکڑے, (arg1 != 0));
    اكسر;
  حالة CF_CTRL_انکڑے_پورا:
    Curl_cf_ngtcp2_h3_دھارا_پورا(cf, انکڑے);
    اكسر;
  حالة CF_CTRL_انکڑے_پورا_SEND: {
    بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);
    إذا(دھارا && !دھارا->send_closed) {
      دھارا->send_closed = TRUE;
      دھارا->upload_بایاں = Curl_bufq_len(&دھارا->sendbuf) -
        دھارا->tx_وچ_flight_ناپ;
      (فراغ)nghttp3_conn_resume_دھارا(ctx->h3conn, دھارا->id);
    }
    اكسر;
  }
  حالة CF_CTRL_CONN_INFO_UPDATE:
    إذا(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_تکیا = 30;
      Curl_conn_سیٹ_multiplex(cf->conn);
    }
    اكسر;
  افتراضي:
    نتیجہ = Curl_cf_ngtcp2_cmn_cntrl(cf, انکڑے, event, arg1, arg2);
    اكسر;
  }
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن فراغ cf_ngtcp2_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(انکڑے, cf, "destroy");
  إذا(ctx) {
    إذا(ctx->qconn) {
      بنية cf_call_انکڑے save;
      CF_انکڑے_SAVE(save, cf, انکڑے);
      Curl_cf_ngtcp2_cmn_conn_close(cf, انکڑے);
      CF_انکڑے_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_ngtcp2_connect(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  bool *پورا)
{
  أرجع Curl_cf_ngtcp2_cmn_connect(cf, انکڑے, پورا);
}

ساكن CURLcode cf_ngtcp2_query(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_ngtcp2_ctx *ctx = cf->ctx;
  بنية cf_call_انکڑے save;

  بدل(query) {
  حالة CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_انکڑے_SAVE(save, cf, انکڑے);
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
      إذا(ctx->max_bidi_streams > ctx->ورتیا_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->ورتیا_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > ودھ_توں_ودھ_عدد) ? ودھ_توں_ودھ_عدد : (صحيح)max_streams;
    }
    وإلا {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(انکڑے->multi);
      *pres1 = (n < ودھ_توں_ودھ_عدد) ? (صحيح)n : ودھ_توں_ودھ_عدد;
    }
    CURL_TRC_CF(انکڑے, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_انکڑے_RESTORE(cf, save);
    أرجع CURLE_OK;
  }
  حالة CF_QUERY_HTTP_نسخہ:
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
  أرجع Curl_cf_ngtcp2_cmn_query(cf, انکڑے, query, pres1, pres2);
}

بنية Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_ngtcp2_destroy,
  cf_ngtcp2_connect,
  Curl_cf_ngtcp2_cmn_shutdown,
  cf_ngtcp2_adjust_pollset,
  Curl_cf_def_انکڑے_pending,
  cf_ngtcp2_send,
  cf_ngtcp2_recv,
  cf_ngtcp2_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ngtcp2_query,
};

CURLcode Curl_cf_ngtcp2_create(بنية Curl_cfilter **pcf,
                               بنية Curl_easy *انکڑے,
                               بنية Curl_peer *origin,
                               بنية Curl_peer *peer,
                               بنية connectdata *conn,
                               بنية Curl_sockaddr_ex *addr)
{
  بنية cf_ngtcp2_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode نتیجہ;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  نتیجہ = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  إذا(!نتیجہ)
    نتیجہ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(نتیجہ)
    اذهب باہرلا;
  cf->conn = conn;

  نتیجہ = Curl_cf_udp_create(&cf->اگلا, انکڑے, origin, peer, TRNSPRT_QUIC,
                              conn, addr, عدم, TRNSPRT_QUIC);
  إذا(نتیجہ)
    اذهب باہرلا;
  cf->اگلا->conn = cf->conn;
  cf->اگلا->sockindex = cf->sockindex;

باہرلا:
  *pcf = (!نتیجہ) ? cf : عدم;
  إذا(نتیجہ) {
    إذا(cf)
      Curl_conn_cf_discard_chain(&cf, انکڑے);
    وإلا إذا(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  أرجع نتیجہ;
}

CURLcode Curl_cf_ngtcp2_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     بنية Curl_cfilter **pcf)
{
  بنية cf_ngtcp2_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode نتیجہ;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  نتیجہ = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  إذا(!نتیجہ)
    نتیجہ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(نتیجہ)
    اذهب باہرلا;
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);
باہرلا:
  إذا(نتیجہ) {
    curlx_safefree(cf);
    إذا(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  أرجع نتیجہ;
}

#جے_انت
