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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "u32_ptrset.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cf-socket.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "curlx/win32-fopen.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "http1.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "vdns/cf-dns.h"
#అన్తర్భావ "vquic/vquic.h"
#అన్తర్భావ "vquic/vquic_int.h"
#అన్తర్భావ "vquic/cf-ngtcp2-cmn.h"
#అన్తర్భావ "vquic/cf-ngtcp2.h"


స్థిర పూర్ణాంకం cb_h3_acked_req_body(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                                uint64_t datalen, శూన్య *user_దత్తాంశం,
                                శూన్య *ప్రవాహం_user_దత్తాంశం);

స్థిర CURLcode cf_ngtcp2_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode ఫలితం = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, దత్తాంశం);

  ఒకవేళ(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    తిరిగి CURLE_OK;

  Curl_pollset_check(దత్తాంశం, ps, sock, &want_recv, &want_send);
  ఒకవేళ(!want_send && !Curl_bufq_is_ఖాళీలు(&ctx->q.sendbuf))
    want_send = TRUE;

  ఒకవేళ(want_recv || want_send) {
    నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
    నిర్మాణం cf_call_దత్తాంశం save;
    bool c_exhaust, s_exhaust;

    CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_ఎడమ(ctx->qconn) ||
                              !ngtcp2_conn_get_max_దత్తాంశం_ఎడమ(ctx->qconn));
    s_exhaust = want_send && ప్రవాహం && ప్రవాహం->id >= 0 &&
                (ప్రవాహం->quic_flow_blocked ||
                 ప్రవాహం->tx_లోపల_flight_కొలత >= ప్రవాహం->tx_లోపల_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_ఖాళీలు(&ctx->q.sendbuf);

    ఫలితం = Curl_pollset_సమితి(దత్తాంశం, ps, sock, want_recv, want_send);
    CF_దత్తాంశం_RESTORE(cf, save);
  }
  తిరిగి ఫలితం;
}

స్థిర పూర్ణాంకం cb_h3_ప్రవాహం_close(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                              uint64_t app_error_సంకేతం, శూన్య *user_దత్తాంశం,
                              శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  నిర్మాణం Curl_easy *దత్తాంశం =
    Curl_cf_ngtcp2_get_xfer(cf, ప్రవాహం_user_దత్తాంశం, &ప్రవాహం);
  (శూన్య)conn;
  (శూన్య)ప్రవాహం_id;

  /* we might be called by nghttp3 after we already cleaned up */
  ఒకవేళ(!దత్తాంశం || !ప్రవాహం)
    తిరిగి 0;

  ప్రవాహం->closed = TRUE;
  ప్రవాహం->error3 = app_error_సంకేతం;
  ఒకవేళ(ప్రవాహం->error3 != NGHTTP3_H3_NO_ERROR) {
    ప్రవాహం->reset = TRUE;
    ప్రవాహం->send_closed = TRUE;
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                ప్రవాహం->id, ప్రవాహం->error3);
  }
  లేకపోతే {
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] CLOSED", ప్రవాహం->id);
  }
  Curl_multi_mark_dirty(దత్తాంశం);
  తిరిగి 0;
}

స్థిర శూన్య h3_xfer_write_resp_hd(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం,
                                  స్థిరరాశి అక్షరం *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ఒకవేళ(!ప్రవాహం->xfer_ఫలితం) {
    ప్రవాహం->xfer_ఫలితం = Curl_xfer_write_resp_hd(దత్తాంశం, buf, buflen, eos);
    ఒకవేళ(ప్రవాహం->xfer_ఫలితం)
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", ప్రవాహం->id, (పూర్ణాంకం)ప్రవాహం->xfer_ఫలితం,
                  buflen);
  }
}

స్థిర శూన్య h3_xfer_write_resp(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం,
                               స్థిరరాశి అక్షరం *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ఒకవేళ(!ప్రవాహం->xfer_ఫలితం) {
    ప్రవాహం->xfer_ఫలితం = Curl_xfer_write_resp(దత్తాంశం, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    ఒకవేళ(ప్రవాహం->xfer_ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  ప్రవాహం->id, (పూర్ణాంకం)ప్రవాహం->xfer_ఫలితం, buflen);
    }
  }
}

స్థిర శూన్య cf_ngtcp2_upd_rx_win(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_ప్రవాహం_కిటికీ_కొలత_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  ఒకవేళ(Curl_rlimit_చురుకైన(&దత్తాంశం->పురోగతి.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    ఒకవేళ(!ప్రవాహం->rx_స్థానభ్రంశం)
      తిరిగి;

    avail = Curl_rlimit_avail(&దత్తాంశం->పురోగతి.dl.rlimit, ఖాళీ);
    ఒకవేళ(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", ప్రవాహం->id, avail);
      తిరిగి;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_ప్రవాహం_కిటికీ_కొలత_MAX);
  }

  ఒకవేళ(ప్రవాహం->rx_స్థానభ్రంశం_max < ప్రవాహం->rx_స్థానభ్రంశం) {
    DEBUGASSERT(0);
    తిరిగి;
  }
  cur_win = ప్రవాహం->rx_స్థానభ్రంశం_max - ప్రవాహం->rx_స్థానభ్రంశం;

  ఒకవేళ(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    ఒకవేళ(UINT64_MAX - delta < ప్రవాహం->rx_స్థానభ్రంశం_max)
      delta = UINT64_MAX - ప్రవాహం->rx_స్థానభ్రంశం_max;
    ఒకవేళ(delta) {
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", ప్రవాహం->id, delta);
      ప్రవాహం->rx_స్థానభ్రంశం_max += delta;
      ngtcp2_conn_extend_max_ప్రవాహం_స్థానభ్రంశం(ctx->qconn, ప్రవాహం->id, delta);
    }
  }
}

స్థిర పూర్ణాంకం cb_h3_recv_దత్తాంశం(nghttp3_conn *conn, int64_t stream3_id,
                           స్థిరరాశి uint8_t *buf, size_t buflen,
                           శూన్య *user_దత్తాంశం, శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  నిర్మాణం Curl_easy *దత్తాంశం =
    Curl_cf_ngtcp2_get_xfer(cf, ప్రవాహం_user_దత్తాంశం, &ప్రవాహం);

  (శూన్య)conn;
  (శూన్య)stream3_id;

  ఒకవేళ(!ప్రవాహం || !దత్తాంశం)
    తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, దత్తాంశం, ప్రవాహం, (స్థిరరాశి అక్షరం *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_స్థానభ్రంశం(ctx->qconn, buflen);
  ప్రవాహం->rx_స్థానభ్రంశం += buflen;
  ఒకవేళ(ప్రవాహం->rx_స్థానభ్రంశం_max < ప్రవాహం->rx_స్థానభ్రంశం)
    ప్రవాహం->rx_స్థానభ్రంశం_max = ప్రవాహం->rx_స్థానభ్రంశం;

  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              ప్రవాహం->id, buflen, ప్రవాహం->rx_స్థానభ్రంశం_max - ప్రవాహం->rx_స్థానభ్రంశం);
  cf_ngtcp2_upd_rx_win(cf, దత్తాంశం, ప్రవాహం);
  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, శూన్య *user_దత్తాంశం,
                                  శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = ప్రవాహం_user_దత్తాంశం;
  (శూన్య)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_ప్రవాహం_స్థానభ్రంశం(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_స్థానభ్రంశం(ctx->qconn, consumed);

  ఒకవేళ(ప్రవాహం) {
    ప్రవాహం->rx_స్థానభ్రంశం += consumed;
    ప్రవాహం->rx_స్థానభ్రంశం_max += consumed;
  }
  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_h3_చివర_శీర్షికలు(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                             పూర్ణాంకం fin, శూన్య *user_దత్తాంశం, శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  నిర్మాణం Curl_easy *దత్తాంశం =
    Curl_cf_ngtcp2_get_xfer(cf, ప్రవాహం_user_దత్తాంశం, &ప్రవాహం);
  (శూన్య)conn;
  (శూన్య)ప్రవాహం_id;
  (శూన్య)fin;
  (శూన్య)cf;

  ఒకవేళ(!ప్రవాహం || !దత్తాంశం)
    తిరిగి 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, దత్తాంశం, ప్రవాహం, STRCONST("\r\n"),
                        (bool)ప్రవాహం->closed);

  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] end_headers, status=%d",
              ప్రవాహం_id, ప్రవాహం->స్థితి_సంకేతం);
  ఒకవేళ(ప్రవాహం->స్థితి_సంకేతం / 100 != 1) {
    ప్రవాహం->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(దత్తాంశం);
  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_h3_recv_శీర్షిక(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *విలువ, uint8_t flags,
                             శూన్య *user_దత్తాంశం, శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(విలువ);
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  నిర్మాణం Curl_easy *దత్తాంశం =
    Curl_cf_ngtcp2_get_xfer(cf, ప్రవాహం_user_దత్తాంశం, &ప్రవాహం);
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)conn;
  (శూన్య)ప్రవాహం_id;
  (శూన్య)token;
  (శూన్య)flags;
  (శూన్య)cf;

  /* we might have cleaned up this transfer already */
  ఒకవేళ(!ప్రవాహం || !దత్తాంశం)
    తిరిగి 0;

  ఒకవేళ(token == NGHTTP3_QPACK_TOKEN_స్థితి) {

    ఫలితం = Curl_http_decode_స్థితి(&ప్రవాహం->స్థితి_సంకేతం,
                                     (స్థిరరాశి అక్షరం *)h3val.పునాది, h3val.len);
    ఒకవేళ(ఫలితం)
      తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    ఫలితం = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(&ctx->scratch,
                              (స్థిరరాశి అక్షరం *)h3val.పునాది, h3val.len);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    ఒకవేళ(!ఫలితం)
      h3_xfer_write_resp_hd(cf, దత్తాంశం, ప్రవాహం, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] status: %s",
                ప్రవాహం_id, curlx_dyn_ptr(&ctx->scratch));
    ఒకవేళ(ఫలితం) {
      తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  లేకపోతే {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] header: %.*s: %.*s",
                ప్రవాహం_id, (పూర్ణాంకం)h3name.len, h3name.పునాది,
                (పూర్ణాంకం)h3val.len, h3val.పునాది);
    curlx_dyn_reset(&ctx->scratch);
    ఫలితం = curlx_dyn_addn(&ctx->scratch,
                            (స్థిరరాశి అక్షరం *)h3name.పునాది, h3name.len);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(&ctx->scratch,
                              (స్థిరరాశి అక్షరం *)h3val.పునాది, h3val.len);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    ఒకవేళ(!ఫలితం)
      h3_xfer_write_resp_hd(cf, దత్తాంశం, ప్రవాహం, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_h3_stop_sending(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                              uint64_t app_error_సంకేతం, శూన్య *user_దత్తాంశం,
                              శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  పూర్ణాంకం rv;
  (శూన్య)conn;
  (శూన్య)ప్రవాహం_user_దత్తాంశం;

  rv = ngtcp2_conn_shutdown_ప్రవాహం_read(ctx->qconn, 0, ప్రవాహం_id,
                                        app_error_సంకేతం);
  ఒకవేళ(rv && rv != NGTCP2_ERR_ప్రవాహం_NOT_దొరికింది) {
    తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_h3_reset_ప్రవాహం(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                              uint64_t app_error_సంకేతం, శూన్య *user_దత్తాంశం,
                              శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = Curl_cf_ngtcp2_get_xfer(cf, ప్రవాహం_user_దత్తాంశం, ఖాళీ);
  పూర్ణాంకం rv;
  (శూన్య)conn;

  rv = ngtcp2_conn_shutdown_ప్రవాహం_write(ctx->qconn, 0, ప్రవాహం_id,
                                         app_error_సంకేతం);
  ఒకవేళ(దత్తాంశం)
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] reset -> %d", ప్రవాహం_id, rv);
  ఒకవేళ(rv && rv != NGTCP2_ERR_ప్రవాహం_NOT_దొరికింది) {
    తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  తిరిగి 0;
}

#నిర్వచితమైతే CURL_HAVE_DIAG
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wmissing-field-initializers"
#అయితే_ముగింపు
స్థిర nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_ప్రవాహం_close,
  cb_h3_recv_దత్తాంశం,
  cb_h3_deferred_consume,
  ఖాళీ, /* begin_headers */
  cb_h3_recv_శీర్షిక,
  cb_h3_చివర_శీర్షికలు,
  ఖాళీ, /* begin_trailers */
  cb_h3_recv_శీర్షిక,
  ఖాళీ, /* end_trailers */
  cb_h3_stop_sending,
  ఖాళీ, /* end_stream */
  cb_h3_reset_ప్రవాహం,
  ఖాళీ, /* shutdown */
  ఖాళీ, /* recv_settings (deprecated) */
#నిర్వచితమైతే NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  ఖాళీ, /* recv_origin */
  ఖాళీ, /* end_origin */
  ఖాళీ, /* rand */
#అయితే_ముగింపు
#నిర్వచితమైతే NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  ఖాళీ, /* recv_settings2 */
#అయితే_ముగింపు
#నిర్వచితమైతే NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  ఖాళీ, /* stream_close2 */
#అయితే_ముగింపు
};
#నిర్వచితమైతే CURL_HAVE_DIAG
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు

స్థిర CURLcode init_ngh3_conn(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం cf_ngtcp2_ctx *ctx)
{
  పూర్ణాంకం rc;

  ఒకవేళ(ngtcp2_conn_get_streams_uni_ఎడమ(ctx->qconn) < 3) {
    failf(దత్తాంశం, "QUIC connection lacks 3 uni streams to run HTTP/3");
    తిరిగి CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "error creating nghttp3 connection instance");
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  తిరిగి Curl_cf_ngtcp2_h3_init_ctrls(ctx, దత్తాంశం);
}

స్థిర CURLcode recv_closed_ప్రవాహం(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం,
                                   size_t *pnread)
{
  (శూన్య)cf;
  *pnread = 0;
  ఒకవేళ(ప్రవాహం->reset) {
    ఒకవేళ(ప్రవాహం->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(దత్తాంశం, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", ప్రవాహం->id);
      connclose(cf->conn); /* do not use this anymore */
      దత్తాంశం->state.refused_ప్రవాహం = TRUE;
      తిరిగి CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    లేకపోతే ఒకవేళ(ప్రవాహం->resp_hds_complete && దత్తాంశం->req.no_body) {
        CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", ప్రవాహం->id, ప్రవాహం->error3,
                    Curl_vquic_h3_err_str(ప్రవాహం->error3));
        తిరిగి CURLE_OK;
    }
    failf(దత్తాంశం, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", ప్రవాహం->id, ప్రవాహం->error3,
          Curl_vquic_h3_err_str(ప్రవాహం->error3));
    తిరిగి దత్తాంశం->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  లేకపోతే ఒకవేళ(!ప్రవాహం->resp_hds_complete) {
    failf(దత్తాంశం,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          ప్రవాహం->id);
    తిరిగి CURLE_HTTP3;
  }
  తిరిగి CURLE_OK;
}

/* incoming data frames on the h3 stream */
స్థిర CURLcode cf_ngtcp2_recv(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                               అక్షరం *buf, size_t buflen, size_t *pnread)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
  నిర్మాణం cf_call_దత్తాంశం save;
  నిర్మాణం cf_ngtcp2_io_ctx pktx;
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం i;

  (శూన్య)ctx;
  (శూన్య)buf;
  NOVERBOSE((శూన్య)buflen);

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  ఒకవేళ(ctx->tls_vrfy_ఫలితం) {
    ఫలితం = ctx->tls_vrfy_ఫలితం;
    వెళ్ళు denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, దత్తాంశం);

  ఒకవేళ(!ప్రవాహం || ctx->shutdown_started) {
    ఫలితం = CURLE_RECV_ERROR;
    వెళ్ళు బహిర్గతం;
  }

  cf_ngtcp2_upd_rx_win(cf, దత్తాంశం, ప్రవాహం);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  కొరకు(i = 0; i < 2; ++i) {
    ఒకవేళ(ప్రవాహం->xfer_ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] xfer write failed", ప్రవాహం->id);
      Curl_cf_ngtcp2_h3_ప్రవాహం_close(cf, దత్తాంశం, ప్రవాహం);
      ఫలితం = ప్రవాహం->xfer_ఫలితం;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(ప్రవాహం->closed) {
      ఫలితం = recv_closed_ప్రవాహం(cf, దత్తాంశం, ప్రవాహం, pnread);
      వెళ్ళు బహిర్గతం;
    }

    ఒకవేళ(!i && Curl_cf_ngtcp2_పురోగతి_ingress(cf, దత్తాంశం, &pktx)) {
      ఫలితం = CURLE_RECV_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఫలితం = CURLE_AGAIN;

బహిర్గతం:
  ఫలితం = Curl_1st_fatal(ఫలితం,
                          Curl_cf_ngtcp2_పురోగతి_egress(cf, దత్తాంశం, &pktx));
  ఫలితం = Curl_1st_fatal(ఫలితం,
                          Curl_cf_ngtcp2_cmn_సమితి_expiry(cf, దత్తాంశం, &pktx));
  ఒకవేళ(ctx->tls_vrfy_ఫలితం)
    ఫలితం = ctx->tls_vrfy_ఫలితం;
denied:
  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              ప్రవాహం ? ప్రవాహం->id : -1, buflen, (పూర్ణాంకం)ఫలితం, *pnread);
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర పూర్ణాంకం cb_h3_acked_req_body(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                                uint64_t datalen, శూన్య *user_దత్తాంశం,
                                శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = ప్రవాహం_user_దత్తాంశం;
  size_t skiplen;

  (శూన్య)cf;
  ఒకవేళ(!ప్రవాహం)
    తిరిగి 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  ఒకవేళ(datalen >= (uint64_t)ప్రవాహం->tx_లోపల_flight_కొలత)
    skiplen = ప్రవాహం->tx_లోపల_flight_కొలత;
  లేకపోతే
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&ప్రవాహం->sendbuf, skiplen);
  ప్రవాహం->tx_లోపల_flight_కొలత -= skiplen;

  /* Resume upload processing if we have more data to send */
  ఒకవేళ(ప్రవాహం->tx_లోపల_flight_కొలత < Curl_bufq_len(&ప్రవాహం->sendbuf)) {
    పూర్ణాంకం rv = nghttp3_conn_resume_ప్రవాహం(conn, ప్రవాహం_id);
    ఒకవేళ(rv && rv != NGHTTP3_ERR_ప్రవాహం_NOT_దొరికింది) {
      తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  తిరిగి 0;
}

స్థిర nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, శూన్య *user_దత్తాంశం,
                                         శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  నిర్మాణం Curl_easy *దత్తాంశం =
    Curl_cf_ngtcp2_get_xfer(cf, ప్రవాహం_user_దత్తాంశం, &ప్రవాహం);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (శూన్య)cf;
  (శూన్య)conn;
  (శూన్య)ప్రవాహం_id;
  (శూన్య)user_దత్తాంశం;
  (శూన్య)veccnt;

  ఒకవేళ(!ప్రవాహం || !దత్తాంశం)
    తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  ఒకవేళ(ప్రవాహం->tx_లోపల_flight_కొలత < Curl_bufq_len(&ప్రవాహం->sendbuf)) {
    nvecs = 0;
    అయితే(nvecs < veccnt &&
          Curl_bufq_peek_at(&ప్రవాహం->sendbuf,
                            ప్రవాహం->tx_లోపల_flight_కొలత,
                            CURL_UNCONST(&vec[nvecs].పునాది),
                            &vec[nvecs].len)) {
      ప్రవాహం->tx_లోపల_flight_కొలత += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been be able to peek */
  }

  ఒకవేళ(nwritten > 0 && ప్రవాహం->upload_ఎడమ != -1)
    ప్రవాహం->upload_ఎడమ -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  ఒకవేళ(ప్రవాహం->upload_ఎడమ == 0) {
    *pflags = NGHTTP3_దత్తాంశం_FLAG_EOF;
    ప్రవాహం->send_closed = TRUE;
  }
  లేకపోతే ఒకవేళ(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] read req body -> AGAIN", ప్రవాహం->id);
    తిరిగి NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              ప్రవాహం->id, nvecs,
              *pflags == NGHTTP3_దత్తాంశం_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&ప్రవాహం->sendbuf),
              ప్రవాహం->upload_ఎడమ);
  తిరిగి (nghttp3_ssize)nvecs;
}

స్థిర CURLcode h3_ప్రవాహం_open(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               స్థిరరాశి uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = ఖాళీ;
  int64_t sid;
  నిర్మాణం dynhds h2_శీర్షికలు;
  size_t nheader;
  nghttp3_nv *nva = ఖాళీ;
  పూర్ణాంకం rc = 0;
  గుర్తులేని పూర్ణాంకం i;
  nghttp3_దత్తాంశం_reader reader;
  nghttp3_దత్తాంశం_reader *preader = ఖాళీ;
  CURLcode ఫలితం;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_శీర్షికలు, 0, DYN_HTTP_REQUEST);

  ఫలితం = Curl_cf_ngtcp2_h3_ప్రవాహం_అమరిక(cf, దత్తాంశం, &ప్రవాహం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = Curl_h1_req_parse_read(&ప్రవాహం->h1, buf, len, ఖాళీ,
    !దత్తాంశం->state.http_ignorecustom ?
    CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST) : ఖాళీ,
    0, pnwritten);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ఒకవేళ(!ప్రవాహం->h1.పూర్తి) {
    /* need more data */
    వెళ్ళు బహిర్గతం;
  }
  DEBUGASSERT(ప్రవాహం->h1.req);

  ఫలితం = Curl_http_req_గమ్యం_h2(&h2_శీర్షికలు, ప్రవాహం->h1.req, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* no longer needed */
  Curl_h1_req_parse_free(&ప్రవాహం->h1);

  nheader = Curl_dynhds_లెక్క(&h2_శీర్షికలు);
  nva = curlx_malloc(పరిమాణం(nghttp3_nv) * nheader);
  ఒకవేళ(!nva) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  కొరకు(i = 0; i < nheader; ++i) {
    నిర్మాణం dynhds_నమోదు *e = Curl_dynhds_getn(&h2_శీర్షికలు, i);
    nva[i].name = (గుర్తులేని అక్షరం *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].విలువ = (గుర్తులేని అక్షరం *)e->విలువ;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_ప్రవాహం(ctx->qconn, &sid, ప్రవాహం);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "cannot open bidi streams");
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు బహిర్గతం;
  }
  ప్రవాహం->id = sid;
  ++ctx->వాడిన_bidi_streams;

  ఎంపిక(దత్తాంశం->state.httpreq) {
  స్థితి HTTPREQ_POST:
  స్థితి HTTPREQ_POST_FORM:
  స్థితి HTTPREQ_POST_MIME:
  స్థితి HTTPREQ_PUT:
    /* known request body size or -1 */
    ఒకవేళ(దత్తాంశం->state.infilesize != -1)
      ప్రవాహం->upload_ఎడమ = దత్తాంశం->state.infilesize;
    లేకపోతే
      /* data sending without specifying the data amount up front */
      ప్రవాహం->upload_ఎడమ = -1; /* unknown */
    ఆపు;
  అప్రమేయ:
    /* there is no request body */
    ప్రవాహం->upload_ఎడమ = 0; /* no request body */
    ఆపు;
  }

  ప్రవాహం->send_closed = (ప్రవాహం->upload_ఎడమ == 0);
  ఒకవేళ(!ప్రవాహం->send_closed) {
    reader.read_దత్తాంశం = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, ప్రవాహం->id,
                                   nva, nheader, preader, ప్రవాహం);
  ఒకవేళ(rc) {
    ఎంపిక(rc) {
    స్థితి NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(దత్తాంశం, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", ప్రవాహం->id);
      ఆపు;
    అప్రమేయ:
      CURL_TRC_CF(దత్తాంశం, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", ప్రవాహం->id, rc, nghttp3_strerror(rc));
      ఆపు;
    }
    Curl_cf_ngtcp2_h3_ప్రవాహం_close(cf, దత్తాంశం, ప్రవాహం);
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు బహిర్గతం;
  }

  cf_ngtcp2_upd_rx_win(cf, దత్తాంశం, ప్రవాహం);

  ఒకవేళ(Curl_trc_is_verbose(దత్తాంశం)) {
    infof(దత్తాంశం, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          ప్రవాహం->id, Curl_bufref_ptr(&దత్తాంశం->state.url));
    కొరకు(i = 0; i < nheader; ++i) {
      infof(దత్తాంశం, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", ప్రవాహం->id,
            (పూర్ణాంకం)nva[i].namelen, nva[i].name,
            (పూర్ణాంకం)nva[i].valuelen, nva[i].విలువ);
    }
  }

బహిర్గతం:
  curlx_free(nva);
  Curl_dynhds_free(&h2_శీర్షికలు);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_ngtcp2_send(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                               స్థిరరాశి uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
  నిర్మాణం cf_call_దత్తాంశం save;
  నిర్మాణం cf_ngtcp2_io_ctx pktx;
  CURLcode ఫలితం = CURLE_OK;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, దత్తాంశం);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  ఒకవేళ(ctx->tls_vrfy_ఫలితం) {
    ఫలితం = ctx->tls_vrfy_ఫలితం;
    వెళ్ళు denied;
  }

  (శూన్య)eos; /* use for stream EOF and block handling */
  ఫలితం = Curl_cf_ngtcp2_పురోగతి_ingress(cf, దత్తాంశం, &pktx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(!ప్రవాహం || ప్రవాహం->id < 0) {
    ఒకవేళ(ctx->shutdown_started) {
      CURL_TRC_CF(దత్తాంశం, cf, "cannot open stream on closed connection");
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    ఫలితం = h3_ప్రవాహం_open(cf, దత్తాంశం, buf, len, pnwritten);
    ఒకవేళ(ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "failed to open stream -> %d", (పూర్ణాంకం)ఫలితం);
      వెళ్ళు బహిర్గతం;
    }
    ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
  }
  లేకపోతే ఒకవేళ(ప్రవాహం->xfer_ఫలితం) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] xfer write failed", ప్రవాహం->id);
    Curl_cf_ngtcp2_h3_ప్రవాహం_close(cf, దత్తాంశం, ప్రవాహం);
    ఫలితం = ప్రవాహం->xfer_ఫలితం;
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే ఒకవేళ(ప్రవాహం->closed) {
    ఒకవేళ(ప్రవాహం->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", ప్రవాహం->id);
      ఫలితం = CURLE_OK;
      *pnwritten = len;
      వెళ్ళు బహిర్గతం;
    }
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", ప్రవాహం->id, len);
    ఫలితం = CURLE_HTTP3;
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే ఒకవేళ(ctx->shutdown_started) {
    CURL_TRC_CF(దత్తాంశం, cf, "cannot send on closed connection");
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే ఒకవేళ(ప్రవాహం->tx_లోపల_flight_కొలత >= ప్రవాహం->tx_లోపల_flight_ideal) {
    ఫలితం = CURLE_AGAIN;
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే {
    ఫలితం = Curl_bufq_write(&ప్రవాహం->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                ప్రవాహం->id, len, (పూర్ణాంకం)ఫలితం, *pnwritten);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    (శూన్య)nghttp3_conn_resume_ప్రవాహం(ctx->h3conn, ప్రవాహం->id);
  }

  ఒకవేళ(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!ఫలితం);
  ఫలితం = Curl_cf_ngtcp2_పురోగతి_egress(cf, దత్తాంశం, &pktx);

  ఒకవేళ(*pnwritten > 0) {
    స్థిరరాశి size_t delta = (2 * 1024);
    ఒకవేళ(ప్రవాహం->tx_లోపల_flight_కొలత > (ప్రవాహం->tx_లోపల_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      ప్రవాహం->tx_లోపల_flight_ideal += delta;
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  ప్రవాహం->id, len, *pnwritten,
                  ప్రవాహం->tx_లోపల_flight_కొలత, ప్రవాహం->tx_లోపల_flight_ideal);
    }
    లేకపోతే ఒకవేళ((ప్రవాహం->tx_లోపల_flight_కొలత + delta) < ప్రవాహం->tx_లోపల_flight_ideal) {
      /* ngtcp2 keeps less in flight than we try to provide for,
       * decrease our "ideal" tx buffer length */
      ప్రవాహం->tx_లోపల_flight_ideal -= delta;
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, reduce tx_win=%zu",
                  ప్రవాహం->id, len, *pnwritten,
                  ప్రవాహం->tx_లోపల_flight_కొలత, ప్రవాహం->tx_లోపల_flight_ideal);
    }
  }

బహిర్గతం:
  ఫలితం = Curl_1st_fatal(ఫలితం,
                          Curl_cf_ngtcp2_cmn_సమితి_expiry(cf, దత్తాంశం, &pktx));
  ఒకవేళ(ctx->tls_vrfy_ఫలితం)
    ఫలితం = ctx->tls_vrfy_ఫలితం;
denied:
  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              ప్రవాహం ? ప్రవాహం->id : -1, len, (పూర్ణాంకం)ఫలితం, *pnwritten);
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర CURLcode h3_దత్తాంశం_pause(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (శూన్య)cf;
  ఒకవేళ(!pause)
    Curl_multi_mark_dirty(దత్తాంశం);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_ngtcp2_cntrl(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం cf_call_దత్తాంశం save;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  ఎంపిక(event) {
  స్థితి CF_CTRL_దత్తాంశం_అమరిక:
    ఆపు;
  స్థితి CF_CTRL_దత్తాంశం_PAUSE:
    ఫలితం = h3_దత్తాంశం_pause(cf, దత్తాంశం, (arg1 != 0));
    ఆపు;
  స్థితి CF_CTRL_దత్తాంశం_పూర్తి:
    Curl_cf_ngtcp2_h3_ప్రవాహం_పూర్తి(cf, దత్తాంశం);
    ఆపు;
  స్థితి CF_CTRL_దత్తాంశం_పూర్తి_SEND: {
    నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
    ఒకవేళ(ప్రవాహం && !ప్రవాహం->send_closed) {
      ప్రవాహం->send_closed = TRUE;
      ప్రవాహం->upload_ఎడమ = Curl_bufq_len(&ప్రవాహం->sendbuf) -
        ప్రవాహం->tx_లోపల_flight_కొలత;
      (శూన్య)nghttp3_conn_resume_ప్రవాహం(ctx->h3conn, ప్రవాహం->id);
    }
    ఆపు;
  }
  స్థితి CF_CTRL_CONN_INFO_UPDATE:
    ఒకవేళ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_చూసిన = 30;
      Curl_conn_సమితి_multiplex(cf->conn);
    }
    ఆపు;
  అప్రమేయ:
    ఫలితం = Curl_cf_ngtcp2_cmn_cntrl(cf, దత్తాంశం, event, arg1, arg2);
    ఆపు;
  }
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_ngtcp2_destroy(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(దత్తాంశం, cf, "destroy");
  ఒకవేళ(ctx) {
    ఒకవేళ(ctx->qconn) {
      నిర్మాణం cf_call_దత్తాంశం save;
      CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
      Curl_cf_ngtcp2_cmn_conn_close(cf, దత్తాంశం);
      CF_దత్తాంశం_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = ఖాళీ;
  }
}

స్థిర CURLcode cf_ngtcp2_connect(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  bool *పూర్తి)
{
  తిరిగి Curl_cf_ngtcp2_cmn_connect(cf, దత్తాంశం, పూర్తి);
}

స్థిర CURLcode cf_ngtcp2_query(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;

  ఎంపిక(query) {
  స్థితి CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    ఒకవేళ(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    లేకపోతే ఒకవేళ(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      ఒకవేళ(ctx->max_bidi_streams > ctx->వాడిన_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->వాడిన_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > గరిష్ఠ_పూర్ణాంకం) ? గరిష్ఠ_పూర్ణాంకం : (పూర్ణాంకం)max_streams;
    }
    లేకపోతే {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(దత్తాంశం->multi);
      *pres1 = (n < గరిష్ఠ_పూర్ణాంకం) ? (పూర్ణాంకం)n : గరిష్ఠ_పూర్ణాంకం;
    }
    CURL_TRC_CF(దత్తాంశం, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_దత్తాంశం_RESTORE(cf, save);
    తిరిగి CURLE_OK;
  }
  స్థితి CF_QUERY_HTTP_సంస్కరణ:
    *pres1 = 30;
    తిరిగి CURLE_OK;
  స్థితి CF_QUERY_SSL_INFO:
  స్థితి CF_QUERY_SSL_CTX_INFO: {
    నిర్మాణం curl_tlssessioninfo *info = pres2;
    ఒకవేళ(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      తిరిగి CURLE_OK;
    ఆపు;
  }
  స్థితి CF_QUERY_ALPN_NEGOTIATED: {
    స్థిరరాశి అక్షరం **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ఖాళీ;
    తిరిగి CURLE_OK;
  }
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి Curl_cf_ngtcp2_cmn_query(cf, దత్తాంశం, query, pres1, pres2);
}

నిర్మాణం Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_ngtcp2_destroy,
  cf_ngtcp2_connect,
  Curl_cf_ngtcp2_cmn_shutdown,
  cf_ngtcp2_adjust_pollset,
  Curl_cf_def_దత్తాంశం_pending,
  cf_ngtcp2_send,
  cf_ngtcp2_recv,
  cf_ngtcp2_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ngtcp2_query,
};

CURLcode Curl_cf_ngtcp2_create(నిర్మాణం Curl_cfilter **pcf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_peer *origin,
                               నిర్మాణం Curl_peer *peer,
                               నిర్మాణం connectdata *conn,
                               నిర్మాణం Curl_sockaddr_ex *addr)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = ఖాళీ;
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  CURLcode ఫలితం;

  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  ఫలితం = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  cf->conn = conn;

  ఫలితం = Curl_cf_udp_create(&cf->తదుపరి, దత్తాంశం, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ఖాళీ, TRNSPRT_QUIC);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  cf->తదుపరి->conn = cf->conn;
  cf->తదుపరి->sockindex = cf->sockindex;

బహిర్గతం:
  *pcf = (!ఫలితం) ? cf : ఖాళీ;
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(cf)
      Curl_conn_cf_discard_chain(&cf, దత్తాంశం);
    లేకపోతే ఒకవేళ(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  తిరిగి ఫలితం;
}

CURLcode Curl_cf_ngtcp2_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                     నిర్మాణం Curl_peer *origin,
                                     నిర్మాణం Curl_peer *peer,
                                     నిర్మాణం Curl_cfilter **pcf)
{
  నిర్మాణం cf_ngtcp2_ctx *ctx = ఖాళీ;
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  CURLcode ఫలితం;

  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  ఫలితం = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  Curl_conn_cf_చొప్పించు_after(cf_at, cf);
బహిర్గతం:
  ఒకవేళ(ఫలితం) {
    curlx_safefree(cf);
    ఒకవేళ(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు
