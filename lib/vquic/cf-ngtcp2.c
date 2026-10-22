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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "u32_ptrset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-socket.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http1.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/cf-dns.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/cf-ngtcp2-cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/cf-ngtcp2.h"


ਸਥਿਰ ਪੂਰਨ cb_h3_acked_req_body(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                                uint64_t datalen, ਖਾਲੀ *user_ਅੰਕੜੇ,
                                ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ);

ਸਥਿਰ CURLcode cf_ngtcp2_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ);

  ਜੇ(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    ਵਾਪਸ CURLE_OK;

  Curl_pollset_check(ਅੰਕੜੇ, ps, sock, &want_recv, &want_send);
  ਜੇ(!want_send && !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->q.sendbuf))
    want_send = TRUE;

  ਜੇ(want_recv || want_send) {
    ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
    ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
    bool c_exhaust, s_exhaust;

    CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_ਖੱਬਾ(ctx->qconn) ||
                              !ngtcp2_conn_get_max_ਅੰਕੜੇ_ਖੱਬਾ(ctx->qconn));
    s_exhaust = want_send && ਧਾਰਾ && ਧਾਰਾ->id >= 0 &&
                (ਧਾਰਾ->quic_flow_blocked ||
                 ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ >= ਧਾਰਾ->tx_ਵਿੱਚ_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->q.sendbuf);

    ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ(ਅੰਕੜੇ, ps, sock, want_recv, want_send);
    CF_ਅੰਕੜੇ_RESTORE(cf, save);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਪੂਰਨ cb_h3_ਧਾਰਾ_close(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                              uint64_t app_error_ਕੋਡ, ਖਾਲੀ *user_ਅੰਕੜੇ,
                              ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ =
    Curl_cf_ngtcp2_get_xfer(cf, ਧਾਰਾ_user_ਅੰਕੜੇ, &ਧਾਰਾ);
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)ਧਾਰਾ_id;

  /* we might be called by nghttp3 after we already cleaned up */
  ਜੇ(!ਅੰਕੜੇ || !ਧਾਰਾ)
    ਵਾਪਸ 0;

  ਧਾਰਾ->closed = TRUE;
  ਧਾਰਾ->error3 = app_error_ਕੋਡ;
  ਜੇ(ਧਾਰਾ->error3 != NGHTTP3_H3_NO_ERROR) {
    ਧਾਰਾ->reset = TRUE;
    ਧਾਰਾ->send_closed = TRUE;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                ਧਾਰਾ->id, ਧਾਰਾ->error3);
  }
  ਨਹੀਂਤਾਂ {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] CLOSED", ਧਾਰਾ->id);
  }
  Curl_multi_mark_dirty(ਅੰਕੜੇ);
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ h3_xfer_write_resp_hd(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ,
                                  ਅਚਲ ਅੱਖਰ *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ਜੇ(!ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
    ਧਾਰਾ->xfer_ਨਤੀਜਾ = Curl_xfer_write_resp_hd(ਅੰਕੜੇ, buf, buflen, eos);
    ਜੇ(ਧਾਰਾ->xfer_ਨਤੀਜਾ)
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", ਧਾਰਾ->id, (ਪੂਰਨ)ਧਾਰਾ->xfer_ਨਤੀਜਾ,
                  buflen);
  }
}

ਸਥਿਰ ਖਾਲੀ h3_xfer_write_resp(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ,
                               ਅਚਲ ਅੱਖਰ *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ਜੇ(!ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
    ਧਾਰਾ->xfer_ਨਤੀਜਾ = Curl_xfer_write_resp(ਅੰਕੜੇ, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    ਜੇ(ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  ਧਾਰਾ->id, (ਪੂਰਨ)ਧਾਰਾ->xfer_ਨਤੀਜਾ, buflen);
    }
  }
}

ਸਥਿਰ ਖਾਲੀ cf_ngtcp2_upd_rx_win(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  ਜੇ(Curl_rlimit_ਸਰਗਰਮ(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    ਜੇ(!ਧਾਰਾ->rx_ਵਿੱਥ)
      ਵਾਪਸ;

    avail = Curl_rlimit_avail(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit, ਨਿਰਾ);
    ਜੇ(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", ਧਾਰਾ->id, avail);
      ਵਾਪਸ;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_MAX);
  }

  ਜੇ(ਧਾਰਾ->rx_ਵਿੱਥ_max < ਧਾਰਾ->rx_ਵਿੱਥ) {
    DEBUGASSERT(0);
    ਵਾਪਸ;
  }
  cur_win = ਧਾਰਾ->rx_ਵਿੱਥ_max - ਧਾਰਾ->rx_ਵਿੱਥ;

  ਜੇ(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    ਜੇ(UINT64_MAX - delta < ਧਾਰਾ->rx_ਵਿੱਥ_max)
      delta = UINT64_MAX - ਧਾਰਾ->rx_ਵਿੱਥ_max;
    ਜੇ(delta) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", ਧਾਰਾ->id, delta);
      ਧਾਰਾ->rx_ਵਿੱਥ_max += delta;
      ngtcp2_conn_extend_max_ਧਾਰਾ_ਵਿੱਥ(ctx->qconn, ਧਾਰਾ->id, delta);
    }
  }
}

ਸਥਿਰ ਪੂਰਨ cb_h3_recv_ਅੰਕੜੇ(nghttp3_conn *conn, int64_t stream3_id,
                           ਅਚਲ uint8_t *buf, size_t buflen,
                           ਖਾਲੀ *user_ਅੰਕੜੇ, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ =
    Curl_cf_ngtcp2_get_xfer(cf, ਧਾਰਾ_user_ਅੰਕੜੇ, &ਧਾਰਾ);

  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)stream3_id;

  ਜੇ(!ਧਾਰਾ || !ਅੰਕੜੇ)
    ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, ਅੰਕੜੇ, ਧਾਰਾ, (ਅਚਲ ਅੱਖਰ *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_ਵਿੱਥ(ctx->qconn, buflen);
  ਧਾਰਾ->rx_ਵਿੱਥ += buflen;
  ਜੇ(ਧਾਰਾ->rx_ਵਿੱਥ_max < ਧਾਰਾ->rx_ਵਿੱਥ)
    ਧਾਰਾ->rx_ਵਿੱਥ_max = ਧਾਰਾ->rx_ਵਿੱਥ;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              ਧਾਰਾ->id, buflen, ਧਾਰਾ->rx_ਵਿੱਥ_max - ਧਾਰਾ->rx_ਵਿੱਥ);
  cf_ngtcp2_upd_rx_win(cf, ਅੰਕੜੇ, ਧਾਰਾ);
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, ਖਾਲੀ *user_ਅੰਕੜੇ,
                                  ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = ਧਾਰਾ_user_ਅੰਕੜੇ;
  (ਖਾਲੀ)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_ਧਾਰਾ_ਵਿੱਥ(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_ਵਿੱਥ(ctx->qconn, consumed);

  ਜੇ(ਧਾਰਾ) {
    ਧਾਰਾ->rx_ਵਿੱਥ += consumed;
    ਧਾਰਾ->rx_ਵਿੱਥ_max += consumed;
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_h3_ਸਿਰਾ_ਸਿਰਲੇਖਾਂ(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                             ਪੂਰਨ fin, ਖਾਲੀ *user_ਅੰਕੜੇ, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ =
    Curl_cf_ngtcp2_get_xfer(cf, ਧਾਰਾ_user_ਅੰਕੜੇ, &ਧਾਰਾ);
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)ਧਾਰਾ_id;
  (ਖਾਲੀ)fin;
  (ਖਾਲੀ)cf;

  ਜੇ(!ਧਾਰਾ || !ਅੰਕੜੇ)
    ਵਾਪਸ 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, ਅੰਕੜੇ, ਧਾਰਾ, STRCONST("\r\n"),
                        (bool)ਧਾਰਾ->closed);

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] end_headers, status=%d",
              ਧਾਰਾ_id, ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ);
  ਜੇ(ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ / 100 != 1) {
    ਧਾਰਾ->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(ਅੰਕੜੇ);
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_h3_recv_ਸਿਰਲੇਖ(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *ਮੁੱਲ, uint8_t flags,
                             ਖਾਲੀ *user_ਅੰਕੜੇ, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(ਮੁੱਲ);
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ =
    Curl_cf_ngtcp2_get_xfer(cf, ਧਾਰਾ_user_ਅੰਕੜੇ, &ਧਾਰਾ);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)ਧਾਰਾ_id;
  (ਖਾਲੀ)token;
  (ਖਾਲੀ)flags;
  (ਖਾਲੀ)cf;

  /* we might have cleaned up this transfer already */
  ਜੇ(!ਧਾਰਾ || !ਅੰਕੜੇ)
    ਵਾਪਸ 0;

  ਜੇ(token == NGHTTP3_QPACK_TOKEN_ਸਥਿਤੀ) {

    ਨਤੀਜਾ = Curl_http_decode_ਸਥਿਤੀ(&ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ,
                                     (ਅਚਲ ਅੱਖਰ *)h3val.ਅਧਾਰ, h3val.len);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch,
                              (ਅਚਲ ਅੱਖਰ *)h3val.ਅਧਾਰ, h3val.len);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    ਜੇ(!ਨਤੀਜਾ)
      h3_xfer_write_resp_hd(cf, ਅੰਕੜੇ, ਧਾਰਾ, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] status: %s",
                ਧਾਰਾ_id, curlx_dyn_ptr(&ctx->scratch));
    ਜੇ(ਨਤੀਜਾ) {
      ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ਨਹੀਂਤਾਂ {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] header: %.*s: %.*s",
                ਧਾਰਾ_id, (ਪੂਰਨ)h3name.len, h3name.ਅਧਾਰ,
                (ਪੂਰਨ)h3val.len, h3val.ਅਧਾਰ);
    curlx_dyn_reset(&ctx->scratch);
    ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch,
                            (ਅਚਲ ਅੱਖਰ *)h3name.ਅਧਾਰ, h3name.len);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch,
                              (ਅਚਲ ਅੱਖਰ *)h3val.ਅਧਾਰ, h3val.len);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    ਜੇ(!ਨਤੀਜਾ)
      h3_xfer_write_resp_hd(cf, ਅੰਕੜੇ, ਧਾਰਾ, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_h3_stop_sending(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                              uint64_t app_error_ਕੋਡ, ਖਾਲੀ *user_ਅੰਕੜੇ,
                              ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਪੂਰਨ rv;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)ਧਾਰਾ_user_ਅੰਕੜੇ;

  rv = ngtcp2_conn_shutdown_ਧਾਰਾ_read(ctx->qconn, 0, ਧਾਰਾ_id,
                                        app_error_ਕੋਡ);
  ਜੇ(rv && rv != NGTCP2_ERR_ਧਾਰਾ_NOT_ਮਿਲਿਆ) {
    ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_h3_reset_ਧਾਰਾ(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                              uint64_t app_error_ਕੋਡ, ਖਾਲੀ *user_ਅੰਕੜੇ,
                              ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = Curl_cf_ngtcp2_get_xfer(cf, ਧਾਰਾ_user_ਅੰਕੜੇ, ਨਿਰਾ);
  ਪੂਰਨ rv;
  (ਖਾਲੀ)conn;

  rv = ngtcp2_conn_shutdown_ਧਾਰਾ_write(ctx->qconn, 0, ਧਾਰਾ_id,
                                         app_error_ਕੋਡ);
  ਜੇ(ਅੰਕੜੇ)
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] reset -> %d", ਧਾਰਾ_id, rv);
  ਜੇ(rv && rv != NGTCP2_ERR_ਧਾਰਾ_NOT_ਮਿਲਿਆ) {
    ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  ਵਾਪਸ 0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_DIAG
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wmissing-field-initializers"
#ਜੇ_ਅੰਤ
ਸਥਿਰ nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_ਧਾਰਾ_close,
  cb_h3_recv_ਅੰਕੜੇ,
  cb_h3_deferred_consume,
  ਨਿਰਾ, /* begin_headers */
  cb_h3_recv_ਸਿਰਲੇਖ,
  cb_h3_ਸਿਰਾ_ਸਿਰਲੇਖਾਂ,
  ਨਿਰਾ, /* begin_trailers */
  cb_h3_recv_ਸਿਰਲੇਖ,
  ਨਿਰਾ, /* end_trailers */
  cb_h3_stop_sending,
  ਨਿਰਾ, /* end_stream */
  cb_h3_reset_ਧਾਰਾ,
  ਨਿਰਾ, /* shutdown */
  ਨਿਰਾ, /* recv_settings (deprecated) */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  ਨਿਰਾ, /* recv_origin */
  ਨਿਰਾ, /* end_origin */
  ਨਿਰਾ, /* rand */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  ਨਿਰਾ, /* recv_settings2 */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  ਨਿਰਾ, /* stream_close2 */
#ਜੇ_ਅੰਤ
};
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_DIAG
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode init_ngh3_conn(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ cf_ngtcp2_ctx *ctx)
{
  ਪੂਰਨ rc;

  ਜੇ(ngtcp2_conn_get_streams_uni_ਖੱਬਾ(ctx->qconn) < 3) {
    failf(ਅੰਕੜੇ, "QUIC connection lacks 3 uni streams to run HTTP/3");
    ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "error creating nghttp3 connection instance");
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਵਾਪਸ Curl_cf_ngtcp2_h3_init_ctrls(ctx, ਅੰਕੜੇ);
}

ਸਥਿਰ CURLcode recv_closed_ਧਾਰਾ(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ,
                                   size_t *pnread)
{
  (ਖਾਲੀ)cf;
  *pnread = 0;
  ਜੇ(ਧਾਰਾ->reset) {
    ਜੇ(ਧਾਰਾ->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(ਅੰਕੜੇ, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", ਧਾਰਾ->id);
      connclose(cf->conn); /* do not use this anymore */
      ਅੰਕੜੇ->state.refused_ਧਾਰਾ = TRUE;
      ਵਾਪਸ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->resp_hds_complete && ਅੰਕੜੇ->req.no_body) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", ਧਾਰਾ->id, ਧਾਰਾ->error3,
                    Curl_vquic_h3_err_str(ਧਾਰਾ->error3));
        ਵਾਪਸ CURLE_OK;
    }
    failf(ਅੰਕੜੇ, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", ਧਾਰਾ->id, ਧਾਰਾ->error3,
          Curl_vquic_h3_err_str(ਧਾਰਾ->error3));
    ਵਾਪਸ ਅੰਕੜੇ->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਧਾਰਾ->resp_hds_complete) {
    failf(ਅੰਕੜੇ,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          ਧਾਰਾ->id);
    ਵਾਪਸ CURLE_HTTP3;
  }
  ਵਾਪਸ CURLE_OK;
}

/* incoming data frames on the h3 stream */
ਸਥਿਰ CURLcode cf_ngtcp2_recv(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅੱਖਰ *buf, size_t buflen, size_t *pnread)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  ਬਣਤਰ cf_ngtcp2_io_ctx pktx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ i;

  (ਖਾਲੀ)ctx;
  (ਖਾਲੀ)buf;
  NOVERBOSE((ਖਾਲੀ)buflen);

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  ਜੇ(ctx->tls_vrfy_ਨਤੀਜਾ) {
    ਨਤੀਜਾ = ctx->tls_vrfy_ਨਤੀਜਾ;
    ਜਾਓ denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, ਅੰਕੜੇ);

  ਜੇ(!ਧਾਰਾ || ctx->shutdown_started) {
    ਨਤੀਜਾ = CURLE_RECV_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  cf_ngtcp2_upd_rx_win(cf, ਅੰਕੜੇ, ਧਾਰਾ);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  ਲਈ(i = 0; i < 2; ++i) {
    ਜੇ(ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] xfer write failed", ਧਾਰਾ->id);
      Curl_cf_ngtcp2_h3_ਧਾਰਾ_close(cf, ਅੰਕੜੇ, ਧਾਰਾ);
      ਨਤੀਜਾ = ਧਾਰਾ->xfer_ਨਤੀਜਾ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->closed) {
      ਨਤੀਜਾ = recv_closed_ਧਾਰਾ(cf, ਅੰਕੜੇ, ਧਾਰਾ, pnread);
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਜੇ(!i && Curl_cf_ngtcp2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ, &pktx)) {
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਨਤੀਜਾ = CURLE_AGAIN;

ਬਾਹਰਲਾ:
  ਨਤੀਜਾ = Curl_1st_fatal(ਨਤੀਜਾ,
                          Curl_cf_ngtcp2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ, &pktx));
  ਨਤੀਜਾ = Curl_1st_fatal(ਨਤੀਜਾ,
                          Curl_cf_ngtcp2_cmn_ਸੈੱਟ_expiry(cf, ਅੰਕੜੇ, &pktx));
  ਜੇ(ctx->tls_vrfy_ਨਤੀਜਾ)
    ਨਤੀਜਾ = ctx->tls_vrfy_ਨਤੀਜਾ;
denied:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              ਧਾਰਾ ? ਧਾਰਾ->id : -1, buflen, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread);
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਪੂਰਨ cb_h3_acked_req_body(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                                uint64_t datalen, ਖਾਲੀ *user_ਅੰਕੜੇ,
                                ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = ਧਾਰਾ_user_ਅੰਕੜੇ;
  size_t skiplen;

  (ਖਾਲੀ)cf;
  ਜੇ(!ਧਾਰਾ)
    ਵਾਪਸ 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  ਜੇ(datalen >= (uint64_t)ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ)
    skiplen = ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ;
  ਨਹੀਂਤਾਂ
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&ਧਾਰਾ->sendbuf, skiplen);
  ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ -= skiplen;

  /* Resume upload processing if we have more data to send */
  ਜੇ(ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ < Curl_bufq_len(&ਧਾਰਾ->sendbuf)) {
    ਪੂਰਨ rv = nghttp3_conn_resume_ਧਾਰਾ(conn, ਧਾਰਾ_id);
    ਜੇ(rv && rv != NGHTTP3_ERR_ਧਾਰਾ_NOT_ਮਿਲਿਆ) {
      ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, ਖਾਲੀ *user_ਅੰਕੜੇ,
                                         ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ =
    Curl_cf_ngtcp2_get_xfer(cf, ਧਾਰਾ_user_ਅੰਕੜੇ, &ਧਾਰਾ);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)ਧਾਰਾ_id;
  (ਖਾਲੀ)user_ਅੰਕੜੇ;
  (ਖਾਲੀ)veccnt;

  ਜੇ(!ਧਾਰਾ || !ਅੰਕੜੇ)
    ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  ਜੇ(ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ < Curl_bufq_len(&ਧਾਰਾ->sendbuf)) {
    nvecs = 0;
    ਜਦੋਂ(nvecs < veccnt &&
          Curl_bufq_peek_at(&ਧਾਰਾ->sendbuf,
                            ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ,
                            CURL_UNCONST(&vec[nvecs].ਅਧਾਰ),
                            &vec[nvecs].len)) {
      ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been be able to peek */
  }

  ਜੇ(nwritten > 0 && ਧਾਰਾ->upload_ਖੱਬਾ != -1)
    ਧਾਰਾ->upload_ਖੱਬਾ -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  ਜੇ(ਧਾਰਾ->upload_ਖੱਬਾ == 0) {
    *pflags = NGHTTP3_ਅੰਕੜੇ_FLAG_EOF;
    ਧਾਰਾ->send_closed = TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] read req body -> AGAIN", ਧਾਰਾ->id);
    ਵਾਪਸ NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              ਧਾਰਾ->id, nvecs,
              *pflags == NGHTTP3_ਅੰਕੜੇ_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&ਧਾਰਾ->sendbuf),
              ਧਾਰਾ->upload_ਖੱਬਾ);
  ਵਾਪਸ (nghttp3_ssize)nvecs;
}

ਸਥਿਰ CURLcode h3_ਧਾਰਾ_open(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅਚਲ uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = ਨਿਰਾ;
  int64_t sid;
  ਬਣਤਰ dynhds h2_ਸਿਰਲੇਖਾਂ;
  size_t nheader;
  nghttp3_nv *nva = ਨਿਰਾ;
  ਪੂਰਨ rc = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
  nghttp3_ਅੰਕੜੇ_reader reader;
  nghttp3_ਅੰਕੜੇ_reader *preader = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_ਸਿਰਲੇਖਾਂ, 0, DYN_HTTP_REQUEST);

  ਨਤੀਜਾ = Curl_cf_ngtcp2_h3_ਧਾਰਾ_ਤਿਆਰੀ(cf, ਅੰਕੜੇ, &ਧਾਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = Curl_h1_req_parse_read(&ਧਾਰਾ->h1, buf, len, ਨਿਰਾ,
    !ਅੰਕੜੇ->state.http_ignorecustom ?
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST) : ਨਿਰਾ,
    0, pnwritten);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਜੇ(!ਧਾਰਾ->h1.ਪੂਰਾ) {
    /* need more data */
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  DEBUGASSERT(ਧਾਰਾ->h1.req);

  ਨਤੀਜਾ = Curl_http_req_ਮੰਜ਼ਿਲ_h2(&h2_ਸਿਰਲੇਖਾਂ, ਧਾਰਾ->h1.req, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* no longer needed */
  Curl_h1_req_parse_free(&ਧਾਰਾ->h1);

  nheader = Curl_dynhds_ਗਿਣਤੀ(&h2_ਸਿਰਲੇਖਾਂ);
  nva = curlx_malloc(ਆਕਾਰ(nghttp3_nv) * nheader);
  ਜੇ(!nva) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਲਈ(i = 0; i < nheader; ++i) {
    ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *e = Curl_dynhds_getn(&h2_ਸਿਰਲੇਖਾਂ, i);
    nva[i].name = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].ਮੁੱਲ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)e->ਮੁੱਲ;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_ਧਾਰਾ(ctx->qconn, &sid, ਧਾਰਾ);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "cannot open bidi streams");
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਧਾਰਾ->id = sid;
  ++ctx->ਵਰਤਿਆ_bidi_streams;

  ਚੋਣ(ਅੰਕੜੇ->state.httpreq) {
  ਹਾਲਤ HTTPREQ_POST:
  ਹਾਲਤ HTTPREQ_POST_FORM:
  ਹਾਲਤ HTTPREQ_POST_MIME:
  ਹਾਲਤ HTTPREQ_PUT:
    /* known request body size or -1 */
    ਜੇ(ਅੰਕੜੇ->state.infilesize != -1)
      ਧਾਰਾ->upload_ਖੱਬਾ = ਅੰਕੜੇ->state.infilesize;
    ਨਹੀਂਤਾਂ
      /* data sending without specifying the data amount up front */
      ਧਾਰਾ->upload_ਖੱਬਾ = -1; /* unknown */
    ਰੋਕੋ;
  ਮੂਲ:
    /* there is no request body */
    ਧਾਰਾ->upload_ਖੱਬਾ = 0; /* no request body */
    ਰੋਕੋ;
  }

  ਧਾਰਾ->send_closed = (ਧਾਰਾ->upload_ਖੱਬਾ == 0);
  ਜੇ(!ਧਾਰਾ->send_closed) {
    reader.read_ਅੰਕੜੇ = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, ਧਾਰਾ->id,
                                   nva, nheader, preader, ਧਾਰਾ);
  ਜੇ(rc) {
    ਚੋਣ(rc) {
    ਹਾਲਤ NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", ਧਾਰਾ->id);
      ਰੋਕੋ;
    ਮੂਲ:
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", ਧਾਰਾ->id, rc, nghttp3_strerror(rc));
      ਰੋਕੋ;
    }
    Curl_cf_ngtcp2_h3_ਧਾਰਾ_close(cf, ਅੰਕੜੇ, ਧਾਰਾ);
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  cf_ngtcp2_upd_rx_win(cf, ਅੰਕੜੇ, ਧਾਰਾ);

  ਜੇ(Curl_trc_is_verbose(ਅੰਕੜੇ)) {
    infof(ਅੰਕੜੇ, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          ਧਾਰਾ->id, Curl_bufref_ptr(&ਅੰਕੜੇ->state.url));
    ਲਈ(i = 0; i < nheader; ++i) {
      infof(ਅੰਕੜੇ, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", ਧਾਰਾ->id,
            (ਪੂਰਨ)nva[i].namelen, nva[i].name,
            (ਪੂਰਨ)nva[i].valuelen, nva[i].ਮੁੱਲ);
    }
  }

ਬਾਹਰਲਾ:
  curlx_free(nva);
  Curl_dynhds_free(&h2_ਸਿਰਲੇਖਾਂ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_ngtcp2_send(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅਚਲ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  ਬਣਤਰ cf_ngtcp2_io_ctx pktx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, ਅੰਕੜੇ);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  ਜੇ(ctx->tls_vrfy_ਨਤੀਜਾ) {
    ਨਤੀਜਾ = ctx->tls_vrfy_ਨਤੀਜਾ;
    ਜਾਓ denied;
  }

  (ਖਾਲੀ)eos; /* use for stream EOF and block handling */
  ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ, &pktx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(!ਧਾਰਾ || ਧਾਰਾ->id < 0) {
    ਜੇ(ctx->shutdown_started) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "cannot open stream on closed connection");
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਤੀਜਾ = h3_ਧਾਰਾ_open(cf, ਅੰਕੜੇ, buf, len, pnwritten);
    ਜੇ(ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "failed to open stream -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] xfer write failed", ਧਾਰਾ->id);
    Curl_cf_ngtcp2_h3_ਧਾਰਾ_close(cf, ਅੰਕੜੇ, ਧਾਰਾ);
    ਨਤੀਜਾ = ਧਾਰਾ->xfer_ਨਤੀਜਾ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->closed) {
    ਜੇ(ਧਾਰਾ->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", ਧਾਰਾ->id);
      ਨਤੀਜਾ = CURLE_OK;
      *pnwritten = len;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", ਧਾਰਾ->id, len);
    ਨਤੀਜਾ = CURLE_HTTP3;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ctx->shutdown_started) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "cannot send on closed connection");
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ >= ਧਾਰਾ->tx_ਵਿੱਚ_flight_ideal) {
    ਨਤੀਜਾ = CURLE_AGAIN;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = Curl_bufq_write(&ਧਾਰਾ->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                ਧਾਰਾ->id, len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    (ਖਾਲੀ)nghttp3_conn_resume_ਧਾਰਾ(ctx->h3conn, ਧਾਰਾ->id);
  }

  ਜੇ(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!ਨਤੀਜਾ);
  ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ, &pktx);

  ਜੇ(*pnwritten > 0) {
    ਅਚਲ size_t delta = (2 * 1024);
    ਜੇ(ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ > (ਧਾਰਾ->tx_ਵਿੱਚ_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      ਧਾਰਾ->tx_ਵਿੱਚ_flight_ideal += delta;
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  ਧਾਰਾ->id, len, *pnwritten,
                  ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ, ਧਾਰਾ->tx_ਵਿੱਚ_flight_ideal);
    }
    ਨਹੀਂਤਾਂ ਜੇ((ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ + delta) < ਧਾਰਾ->tx_ਵਿੱਚ_flight_ideal) {
      /* ngtcp2 keeps less in flight than we try to provide for,
       * decrease our "ideal" tx buffer length */
      ਧਾਰਾ->tx_ਵਿੱਚ_flight_ideal -= delta;
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, reduce tx_win=%zu",
                  ਧਾਰਾ->id, len, *pnwritten,
                  ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ, ਧਾਰਾ->tx_ਵਿੱਚ_flight_ideal);
    }
  }

ਬਾਹਰਲਾ:
  ਨਤੀਜਾ = Curl_1st_fatal(ਨਤੀਜਾ,
                          Curl_cf_ngtcp2_cmn_ਸੈੱਟ_expiry(cf, ਅੰਕੜੇ, &pktx));
  ਜੇ(ctx->tls_vrfy_ਨਤੀਜਾ)
    ਨਤੀਜਾ = ctx->tls_vrfy_ਨਤੀਜਾ;
denied:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              ਧਾਰਾ ? ਧਾਰਾ->id : -1, len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten);
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode h3_ਅੰਕੜੇ_pause(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (ਖਾਲੀ)cf;
  ਜੇ(!pause)
    Curl_multi_mark_dirty(ਅੰਕੜੇ);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_ngtcp2_cntrl(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  ਚੋਣ(event) {
  ਹਾਲਤ CF_CTRL_ਅੰਕੜੇ_ਤਿਆਰੀ:
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_ਅੰਕੜੇ_PAUSE:
    ਨਤੀਜਾ = h3_ਅੰਕੜੇ_pause(cf, ਅੰਕੜੇ, (arg1 != 0));
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_ਅੰਕੜੇ_ਪੂਰਾ:
    Curl_cf_ngtcp2_h3_ਧਾਰਾ_ਪੂਰਾ(cf, ਅੰਕੜੇ);
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_ਅੰਕੜੇ_ਪੂਰਾ_SEND: {
    ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
    ਜੇ(ਧਾਰਾ && !ਧਾਰਾ->send_closed) {
      ਧਾਰਾ->send_closed = TRUE;
      ਧਾਰਾ->upload_ਖੱਬਾ = Curl_bufq_len(&ਧਾਰਾ->sendbuf) -
        ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ;
      (ਖਾਲੀ)nghttp3_conn_resume_ਧਾਰਾ(ctx->h3conn, ਧਾਰਾ->id);
    }
    ਰੋਕੋ;
  }
  ਹਾਲਤ CF_CTRL_CONN_INFO_UPDATE:
    ਜੇ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_ਵੇਖਿਆ = 30;
      Curl_conn_ਸੈੱਟ_multiplex(cf->conn);
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ਨਤੀਜਾ = Curl_cf_ngtcp2_cmn_cntrl(cf, ਅੰਕੜੇ, event, arg1, arg2);
    ਰੋਕੋ;
  }
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_ngtcp2_destroy(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "destroy");
  ਜੇ(ctx) {
    ਜੇ(ctx->qconn) {
      ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
      CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
      Curl_cf_ngtcp2_cmn_conn_close(cf, ਅੰਕੜੇ);
      CF_ਅੰਕੜੇ_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = ਨਿਰਾ;
  }
}

ਸਥਿਰ CURLcode cf_ngtcp2_connect(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  bool *ਪੂਰਾ)
{
  ਵਾਪਸ Curl_cf_ngtcp2_cmn_connect(cf, ਅੰਕੜੇ, ਪੂਰਾ);
}

ਸਥਿਰ CURLcode cf_ngtcp2_query(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;

  ਚੋਣ(query) {
  ਹਾਲਤ CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    ਜੇ(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      ਜੇ(ctx->max_bidi_streams > ctx->ਵਰਤਿਆ_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->ਵਰਤਿਆ_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ : (ਪੂਰਨ)max_streams;
    }
    ਨਹੀਂਤਾਂ {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(ਅੰਕੜੇ->multi);
      *pres1 = (n < ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? (ਪੂਰਨ)n : ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
    }
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_ਅੰਕੜੇ_RESTORE(cf, save);
    ਵਾਪਸ CURLE_OK;
  }
  ਹਾਲਤ CF_QUERY_HTTP_ਸੰਸਕਰਣ:
    *pres1 = 30;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CF_QUERY_SSL_INFO:
  ਹਾਲਤ CF_QUERY_SSL_CTX_INFO: {
    ਬਣਤਰ curl_tlssessioninfo *info = pres2;
    ਜੇ(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      ਵਾਪਸ CURLE_OK;
    ਰੋਕੋ;
  }
  ਹਾਲਤ CF_QUERY_ALPN_NEGOTIATED: {
    ਅਚਲ ਅੱਖਰ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ਨਿਰਾ;
    ਵਾਪਸ CURLE_OK;
  }
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ Curl_cf_ngtcp2_cmn_query(cf, ਅੰਕੜੇ, query, pres1, pres2);
}

ਬਣਤਰ Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_ngtcp2_destroy,
  cf_ngtcp2_connect,
  Curl_cf_ngtcp2_cmn_shutdown,
  cf_ngtcp2_adjust_pollset,
  Curl_cf_def_ਅੰਕੜੇ_pending,
  cf_ngtcp2_send,
  cf_ngtcp2_recv,
  cf_ngtcp2_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ngtcp2_query,
};

CURLcode Curl_cf_ngtcp2_create(ਬਣਤਰ Curl_cfilter **pcf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_peer *origin,
                               ਬਣਤਰ Curl_peer *peer,
                               ਬਣਤਰ connectdata *conn,
                               ਬਣਤਰ Curl_sockaddr_ex *addr)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = ਨਿਰਾ;
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਤੀਜਾ = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  cf->conn = conn;

  ਨਤੀਜਾ = Curl_cf_udp_create(&cf->ਅਗਲਾ, ਅੰਕੜੇ, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ਨਿਰਾ, TRNSPRT_QUIC);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  cf->ਅਗਲਾ->conn = cf->conn;
  cf->ਅਗਲਾ->sockindex = cf->sockindex;

ਬਾਹਰਲਾ:
  *pcf = (!ਨਤੀਜਾ) ? cf : ਨਿਰਾ;
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(cf)
      Curl_conn_cf_discard_chain(&cf, ਅੰਕੜੇ);
    ਨਹੀਂਤਾਂ ਜੇ(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_ngtcp2_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                     ਬਣਤਰ Curl_peer *origin,
                                     ਬਣਤਰ Curl_peer *peer,
                                     ਬਣਤਰ Curl_cfilter **pcf)
{
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = ਨਿਰਾ;
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਤੀਜਾ = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  Curl_conn_cf_ਪਾਓ_after(cf_at, cf);
ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ) {
    curlx_safefree(cf);
    ਜੇ(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ
