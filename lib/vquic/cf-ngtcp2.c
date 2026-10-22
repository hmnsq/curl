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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

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


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_acked_req_body(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                                uint64_t datalen, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ);

ಸ್ಥಿರ CURLcode cf_ngtcp2_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ);

  ಆದರೆ(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  Curl_pollset_check(ದತ್ತಾಂಶ, ps, sock, &want_recv, &want_send);
  ಆದರೆ(!want_send && !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->q.sendbuf))
    want_send = TRUE;

  ಆದರೆ(want_recv || want_send) {
    ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
    ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
    bool c_exhaust, s_exhaust;

    CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_ಎಡ(ctx->qconn) ||
                              !ngtcp2_conn_get_max_ದತ್ತಾಂಶ_ಎಡ(ctx->qconn));
    s_exhaust = want_send && ಹರಿವು && ಹರಿವು->id >= 0 &&
                (ಹರಿವು->quic_flow_blocked ||
                 ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ >= ಹರಿವು->tx_ಒಳ_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->q.sendbuf);

    ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ(ದತ್ತಾಂಶ, ps, sock, want_recv, want_send);
    CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_ಹರಿವು_close(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                              uint64_t app_error_ಸಂಕೇತ, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                              ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ =
    Curl_cf_ngtcp2_get_xfer(cf, ಹರಿವು_user_ದತ್ತಾಂಶ, &ಹರಿವು);
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ಹರಿವು_id;

  /* we might be called by nghttp3 after we already cleaned up */
  ಆದರೆ(!ದತ್ತಾಂಶ || !ಹರಿವು)
    ಹಿಂತಿರುಗಿ 0;

  ಹರಿವು->closed = TRUE;
  ಹರಿವು->error3 = app_error_ಸಂಕೇತ;
  ಆದರೆ(ಹರಿವು->error3 != NGHTTP3_H3_NO_ERROR) {
    ಹರಿವು->reset = TRUE;
    ಹರಿವು->send_closed = TRUE;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                ಹರಿವು->id, ಹರಿವು->error3);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] CLOSED", ಹರಿವು->id);
  }
  Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_xfer_write_resp_hd(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ಆದರೆ(!ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
    ಹರಿವು->xfer_ಫಲಿತಾಂಶ = Curl_xfer_write_resp_hd(ದತ್ತಾಂಶ, buf, buflen, eos);
    ಆದರೆ(ಹರಿವು->xfer_ಫಲಿತಾಂಶ)
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಹರಿವು->xfer_ಫಲಿತಾಂಶ,
                  buflen);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_xfer_write_resp(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ಆದರೆ(!ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
    ಹರಿವು->xfer_ಫಲಿತಾಂಶ = Curl_xfer_write_resp(ದತ್ತಾಂಶ, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    ಆದರೆ(ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಹರಿವು->xfer_ಫಲಿತಾಂಶ, buflen);
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ngtcp2_upd_rx_win(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  ಆದರೆ(Curl_rlimit_ಸಕ್ರಿಯ(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    ಆದರೆ(!ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ)
      ಹಿಂತಿರುಗಿ;

    avail = Curl_rlimit_avail(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit, ಖಾಲಿ);
    ಆದರೆ(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", ಹರಿವು->id, avail);
      ಹಿಂತಿರುಗಿ;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_MAX);
  }

  ಆದರೆ(ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max < ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ;
  }
  cur_win = ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max - ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ;

  ಆದರೆ(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    ಆದರೆ(UINT64_MAX - delta < ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max)
      delta = UINT64_MAX - ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max;
    ಆದರೆ(delta) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", ಹರಿವು->id, delta);
      ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max += delta;
      ngtcp2_conn_extend_max_ಹರಿವು_ಸ್ಥಾನಾಂತರ(ctx->qconn, ಹರಿವು->id, delta);
    }
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_recv_ದತ್ತಾಂಶ(nghttp3_conn *conn, int64_t stream3_id,
                           ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t buflen,
                           ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ =
    Curl_cf_ngtcp2_get_xfer(cf, ಹರಿವು_user_ದತ್ತಾಂಶ, &ಹರಿವು);

  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream3_id;

  ಆದರೆ(!ಹರಿವು || !ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, ದತ್ತಾಂಶ, ಹರಿವು, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_ಸ್ಥಾನಾಂತರ(ctx->qconn, buflen);
  ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ += buflen;
  ಆದರೆ(ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max < ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ)
    ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max = ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              ಹರಿವು->id, buflen, ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max - ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ);
  cf_ngtcp2_upd_rx_win(cf, ದತ್ತಾಂಶ, ಹರಿವು);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                  ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = ಹರಿವು_user_ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_ಹರಿವು_ಸ್ಥಾನಾಂತರ(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_ಸ್ಥಾನಾಂತರ(ctx->qconn, consumed);

  ಆದರೆ(ಹರಿವು) {
    ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ += consumed;
    ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max += consumed;
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_ಕೊನೆ_ಶೀರ್ಷಿಕೆಗಳು(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                             ಪೂರ್ಣಾಂಕ fin, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ =
    Curl_cf_ngtcp2_get_xfer(cf, ಹರಿವು_user_ದತ್ತಾಂಶ, &ಹರಿವು);
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ಹರಿವು_id;
  (ಶೂನ್ಯ)fin;
  (ಶೂನ್ಯ)cf;

  ಆದರೆ(!ಹರಿವು || !ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, ದತ್ತಾಂಶ, ಹರಿವು, STRCONST("\r\n"),
                        (bool)ಹರಿವು->closed);

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] end_headers, status=%d",
              ಹರಿವು_id, ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ);
  ಆದರೆ(ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ / 100 != 1) {
    ಹರಿವು->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_recv_ಶೀರ್ಷಿಕೆ(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *ಮೌಲ್ಯ, uint8_t flags,
                             ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(ಮೌಲ್ಯ);
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ =
    Curl_cf_ngtcp2_get_xfer(cf, ಹರಿವು_user_ದತ್ತಾಂಶ, &ಹರಿವು);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ಹರಿವು_id;
  (ಶೂನ್ಯ)token;
  (ಶೂನ್ಯ)flags;
  (ಶೂನ್ಯ)cf;

  /* we might have cleaned up this transfer already */
  ಆದರೆ(!ಹರಿವು || !ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ 0;

  ಆದರೆ(token == NGHTTP3_QPACK_TOKEN_ಸ್ಥಿತಿ) {

    ಫಲಿತಾಂಶ = Curl_http_decode_ಸ್ಥಿತಿ(&ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ,
                                     (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)h3val.ತಳ, h3val.len);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch,
                              (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)h3val.ತಳ, h3val.len);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      h3_xfer_write_resp_hd(cf, ದತ್ತಾಂಶ, ಹರಿವು, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] status: %s",
                ಹರಿವು_id, curlx_dyn_ptr(&ctx->scratch));
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] header: %.*s: %.*s",
                ಹರಿವು_id, (ಪೂರ್ಣಾಂಕ)h3name.len, h3name.ತಳ,
                (ಪೂರ್ಣಾಂಕ)h3val.len, h3val.ತಳ);
    curlx_dyn_reset(&ctx->scratch);
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch,
                            (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)h3name.ತಳ, h3name.len);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch,
                              (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)h3val.ತಳ, h3val.len);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      h3_xfer_write_resp_hd(cf, ದತ್ತಾಂಶ, ಹರಿವು, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_stop_sending(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                              uint64_t app_error_ಸಂಕೇತ, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                              ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಂಕ rv;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ಹರಿವು_user_ದತ್ತಾಂಶ;

  rv = ngtcp2_conn_shutdown_ಹರಿವು_read(ctx->qconn, 0, ಹರಿವು_id,
                                        app_error_ಸಂಕೇತ);
  ಆದರೆ(rv && rv != NGTCP2_ERR_ಹರಿವು_NOT_ಸಿಕ್ಕಿತು) {
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_reset_ಹರಿವು(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                              uint64_t app_error_ಸಂಕೇತ, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                              ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = Curl_cf_ngtcp2_get_xfer(cf, ಹರಿವು_user_ದತ್ತಾಂಶ, ಖಾಲಿ);
  ಪೂರ್ಣಾಂಕ rv;
  (ಶೂನ್ಯ)conn;

  rv = ngtcp2_conn_shutdown_ಹರಿವು_write(ctx->qconn, 0, ಹರಿವು_id,
                                         app_error_ಸಂಕೇತ);
  ಆದರೆ(ದತ್ತಾಂಶ)
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] reset -> %d", ಹರಿವು_id, rv);
  ಆದರೆ(rv && rv != NGTCP2_ERR_ಹರಿವು_NOT_ಸಿಕ್ಕಿತು) {
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_DIAG
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wmissing-field-initializers"
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_ಹರಿವು_close,
  cb_h3_recv_ದತ್ತಾಂಶ,
  cb_h3_deferred_consume,
  ಖಾಲಿ, /* begin_headers */
  cb_h3_recv_ಶೀರ್ಷಿಕೆ,
  cb_h3_ಕೊನೆ_ಶೀರ್ಷಿಕೆಗಳು,
  ಖಾಲಿ, /* begin_trailers */
  cb_h3_recv_ಶೀರ್ಷಿಕೆ,
  ಖಾಲಿ, /* end_trailers */
  cb_h3_stop_sending,
  ಖಾಲಿ, /* end_stream */
  cb_h3_reset_ಹರಿವು,
  ಖಾಲಿ, /* shutdown */
  ಖಾಲಿ, /* recv_settings (deprecated) */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  ಖಾಲಿ, /* recv_origin */
  ಖಾಲಿ, /* end_origin */
  ಖಾಲಿ, /* rand */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  ಖಾಲಿ, /* recv_settings2 */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  ಖಾಲಿ, /* stream_close2 */
#ಆದರೆ_ಅಂತ್ಯ
};
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_DIAG
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode init_ngh3_conn(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ cf_ngtcp2_ctx *ctx)
{
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(ngtcp2_conn_get_streams_uni_ಎಡ(ctx->qconn) < 3) {
    failf(ದತ್ತಾಂಶ, "QUIC connection lacks 3 uni streams to run HTTP/3");
    ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "error creating nghttp3 connection instance");
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಹಿಂತಿರುಗಿ Curl_cf_ngtcp2_h3_init_ctrls(ctx, ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ CURLcode recv_closed_ಹರಿವು(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು,
                                   size_t *pnread)
{
  (ಶೂನ್ಯ)cf;
  *pnread = 0;
  ಆದರೆ(ಹರಿವು->reset) {
    ಆದರೆ(ಹರಿವು->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(ದತ್ತಾಂಶ, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", ಹರಿವು->id);
      connclose(cf->conn); /* do not use this anymore */
      ದತ್ತಾಂಶ->state.refused_ಹರಿವು = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->resp_hds_complete && ದತ್ತಾಂಶ->req.no_body) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", ಹರಿವು->id, ಹರಿವು->error3,
                    Curl_vquic_h3_err_str(ಹರಿವು->error3));
        ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    failf(ದತ್ತಾಂಶ, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", ಹರಿವು->id, ಹರಿವು->error3,
          Curl_vquic_h3_err_str(ಹರಿವು->error3));
    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಹರಿವು->resp_hds_complete) {
    failf(ದತ್ತಾಂಶ,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          ಹರಿವು->id);
    ಹಿಂತಿರುಗಿ CURLE_HTTP3;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* incoming data frames on the h3 stream */
ಸ್ಥಿರ CURLcode cf_ngtcp2_recv(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಅಕ್ಷರ *buf, size_t buflen, size_t *pnread)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  ರಚನೆ cf_ngtcp2_io_ctx pktx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ i;

  (ಶೂನ್ಯ)ctx;
  (ಶೂನ್ಯ)buf;
  NOVERBOSE((ಶೂನ್ಯ)buflen);

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  ಆದರೆ(ctx->tls_vrfy_ಫಲಿತಾಂಶ) {
    ಫಲಿತಾಂಶ = ctx->tls_vrfy_ಫಲಿತಾಂಶ;
    ಹೋಗು denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, ದತ್ತಾಂಶ);

  ಆದರೆ(!ಹರಿವು || ctx->shutdown_started) {
    ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  cf_ngtcp2_upd_rx_win(cf, ದತ್ತಾಂಶ, ಹರಿವು);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  ಗಾಗಿ(i = 0; i < 2; ++i) {
    ಆದರೆ(ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] xfer write failed", ಹರಿವು->id);
      Curl_cf_ngtcp2_h3_ಹರಿವು_close(cf, ದತ್ತಾಂಶ, ಹರಿವು);
      ಫಲಿತಾಂಶ = ಹರಿವು->xfer_ಫಲಿತಾಂಶ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->closed) {
      ಫಲಿತಾಂಶ = recv_closed_ಹರಿವು(cf, ದತ್ತಾಂಶ, ಹರಿವು, pnread);
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆದರೆ(!i && Curl_cf_ngtcp2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ, &pktx)) {
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಫಲಿತಾಂಶ = CURLE_AGAIN;

ಹೊರಹೋಗು:
  ಫಲಿತಾಂಶ = Curl_1st_fatal(ಫಲಿತಾಂಶ,
                          Curl_cf_ngtcp2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ, &pktx));
  ಫಲಿತಾಂಶ = Curl_1st_fatal(ಫಲಿತಾಂಶ,
                          Curl_cf_ngtcp2_cmn_ಗಣ_expiry(cf, ದತ್ತಾಂಶ, &pktx));
  ಆದರೆ(ctx->tls_vrfy_ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = ctx->tls_vrfy_ಫಲಿತಾಂಶ;
denied:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              ಹರಿವು ? ಹರಿವು->id : -1, buflen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread);
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_acked_req_body(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                                uint64_t datalen, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = ಹರಿವು_user_ದತ್ತಾಂಶ;
  size_t skiplen;

  (ಶೂನ್ಯ)cf;
  ಆದರೆ(!ಹರಿವು)
    ಹಿಂತಿರುಗಿ 0;
  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  ಆದರೆ(datalen >= (uint64_t)ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ)
    skiplen = ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ;
  ಇಲ್ಲದಿದ್ದರೆ
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&ಹರಿವು->sendbuf, skiplen);
  ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ -= skiplen;

  /* Resume upload processing if we have more data to send */
  ಆದರೆ(ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ < Curl_bufq_len(&ಹರಿವು->sendbuf)) {
    ಪೂರ್ಣಾಂಕ rv = nghttp3_conn_resume_ಹರಿವು(conn, ಹರಿವು_id);
    ಆದರೆ(rv && rv != NGHTTP3_ERR_ಹರಿವು_NOT_ಸಿಕ್ಕಿತು) {
      ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                         ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ =
    Curl_cf_ngtcp2_get_xfer(cf, ಹರಿವು_user_ದತ್ತಾಂಶ, &ಹರಿವು);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ಹರಿವು_id;
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)veccnt;

  ಆದರೆ(!ಹರಿವು || !ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  ಆದರೆ(ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ < Curl_bufq_len(&ಹರಿವು->sendbuf)) {
    nvecs = 0;
    ಆಗ(nvecs < veccnt &&
          Curl_bufq_peek_at(&ಹರಿವು->sendbuf,
                            ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ,
                            CURL_UNCONST(&vec[nvecs].ತಳ),
                            &vec[nvecs].len)) {
      ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been be able to peek */
  }

  ಆದರೆ(nwritten > 0 && ಹರಿವು->upload_ಎಡ != -1)
    ಹರಿವು->upload_ಎಡ -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  ಆದರೆ(ಹರಿವು->upload_ಎಡ == 0) {
    *pflags = NGHTTP3_ದತ್ತಾಂಶ_FLAG_EOF;
    ಹರಿವು->send_closed = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] read req body -> AGAIN", ಹರಿವು->id);
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              ಹರಿವು->id, nvecs,
              *pflags == NGHTTP3_ದತ್ತಾಂಶ_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&ಹರಿವು->sendbuf),
              ಹರಿವು->upload_ಎಡ);
  ಹಿಂತಿರುಗಿ (nghttp3_ssize)nvecs;
}

ಸ್ಥಿರ CURLcode h3_ಹರಿವು_open(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = ಖಾಲಿ;
  int64_t sid;
  ರಚನೆ dynhds h2_ಶೀರ್ಷಿಕೆಗಳು;
  size_t nheader;
  nghttp3_nv *nva = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ rc = 0;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
  nghttp3_ದತ್ತಾಂಶ_reader reader;
  nghttp3_ದತ್ತಾಂಶ_reader *preader = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_ಶೀರ್ಷಿಕೆಗಳು, 0, DYN_HTTP_REQUEST);

  ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_h3_ಹರಿವು_ಸಿದ್ಧತೆ(cf, ದತ್ತಾಂಶ, &ಹರಿವು);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = Curl_h1_req_parse_read(&ಹರಿವು->h1, buf, len, ಖಾಲಿ,
    !ದತ್ತಾಂಶ->state.http_ignorecustom ?
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST) : ಖಾಲಿ,
    0, pnwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಆದರೆ(!ಹರಿವು->h1.ಮುಗಿದ) {
    /* need more data */
    ಹೋಗು ಹೊರಹೋಗು;
  }
  DEBUGASSERT(ಹರಿವು->h1.req);

  ಫಲಿತಾಂಶ = Curl_http_req_ಗಮ್ಯ_h2(&h2_ಶೀರ್ಷಿಕೆಗಳು, ಹರಿವು->h1.req, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* no longer needed */
  Curl_h1_req_parse_free(&ಹರಿವು->h1);

  nheader = Curl_dynhds_ಎಣಿಕೆ(&h2_ಶೀರ್ಷಿಕೆಗಳು);
  nva = curlx_malloc(ಗಾತ್ರ(nghttp3_nv) * nheader);
  ಆದರೆ(!nva) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಗಾಗಿ(i = 0; i < nheader; ++i) {
    ರಚನೆ dynhds_ನಮೂದು *e = Curl_dynhds_getn(&h2_ಶೀರ್ಷಿಕೆಗಳು, i);
    nva[i].name = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].ಮೌಲ್ಯ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)e->ಮೌಲ್ಯ;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  rc = ngtcp2_conn_open_bidi_ಹರಿವು(ctx->qconn, &sid, ಹರಿವು);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "cannot open bidi streams");
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಹರಿವು->id = sid;
  ++ctx->ಬಳಸಿದ_bidi_streams;

  ಆಯ್ಕೆ(ದತ್ತಾಂಶ->state.httpreq) {
  ಸ್ಥಿತಿ HTTPREQ_POST:
  ಸ್ಥಿತಿ HTTPREQ_POST_FORM:
  ಸ್ಥಿತಿ HTTPREQ_POST_MIME:
  ಸ್ಥಿತಿ HTTPREQ_PUT:
    /* known request body size or -1 */
    ಆದರೆ(ದತ್ತಾಂಶ->state.infilesize != -1)
      ಹರಿವು->upload_ಎಡ = ದತ್ತಾಂಶ->state.infilesize;
    ಇಲ್ಲದಿದ್ದರೆ
      /* data sending without specifying the data amount up front */
      ಹರಿವು->upload_ಎಡ = -1; /* unknown */
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* there is no request body */
    ಹರಿವು->upload_ಎಡ = 0; /* no request body */
    ನಿಲ್ಲಿಸು;
  }

  ಹರಿವು->send_closed = (ಹರಿವು->upload_ಎಡ == 0);
  ಆದರೆ(!ಹರಿವು->send_closed) {
    reader.read_ದತ್ತಾಂಶ = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, ಹರಿವು->id,
                                   nva, nheader, preader, ಹರಿವು);
  ಆದರೆ(rc) {
    ಆಯ್ಕೆ(rc) {
    ಸ್ಥಿತಿ NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", ಹರಿವು->id);
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", ಹರಿವು->id, rc, nghttp3_strerror(rc));
      ನಿಲ್ಲಿಸು;
    }
    Curl_cf_ngtcp2_h3_ಹರಿವು_close(cf, ದತ್ತಾಂಶ, ಹರಿವು);
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  cf_ngtcp2_upd_rx_win(cf, ದತ್ತಾಂಶ, ಹರಿವು);

  ಆದರೆ(Curl_trc_is_verbose(ದತ್ತಾಂಶ)) {
    infof(ದತ್ತಾಂಶ, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          ಹರಿವು->id, Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url));
    ಗಾಗಿ(i = 0; i < nheader; ++i) {
      infof(ದತ್ತಾಂಶ, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", ಹರಿವು->id,
            (ಪೂರ್ಣಾಂಕ)nva[i].namelen, nva[i].name,
            (ಪೂರ್ಣಾಂಕ)nva[i].valuelen, nva[i].ಮೌಲ್ಯ);
    }
  }

ಹೊರಹೋಗು:
  curlx_free(nva);
  Curl_dynhds_free(&h2_ಶೀರ್ಷಿಕೆಗಳು);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_ngtcp2_send(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  ರಚನೆ cf_ngtcp2_io_ctx pktx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, ದತ್ತಾಂಶ);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  ಆದರೆ(ctx->tls_vrfy_ಫಲಿತಾಂಶ) {
    ಫಲಿತಾಂಶ = ctx->tls_vrfy_ಫಲಿತಾಂಶ;
    ಹೋಗು denied;
  }

  (ಶೂನ್ಯ)eos; /* use for stream EOF and block handling */
  ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ, &pktx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(!ಹರಿವು || ಹರಿವು->id < 0) {
    ಆದರೆ(ctx->shutdown_started) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cannot open stream on closed connection");
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಫಲಿತಾಂಶ = h3_ಹರಿವು_open(cf, ದತ್ತಾಂಶ, buf, len, pnwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "failed to open stream -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] xfer write failed", ಹರಿವು->id);
    Curl_cf_ngtcp2_h3_ಹರಿವು_close(cf, ದತ್ತಾಂಶ, ಹರಿವು);
    ಫಲಿತಾಂಶ = ಹರಿವು->xfer_ಫಲಿತಾಂಶ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->closed) {
    ಆದರೆ(ಹರಿವು->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", ಹರಿವು->id);
      ಫಲಿತಾಂಶ = CURLE_OK;
      *pnwritten = len;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", ಹರಿವು->id, len);
    ಫಲಿತಾಂಶ = CURLE_HTTP3;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->shutdown_started) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cannot send on closed connection");
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ >= ಹರಿವು->tx_ಒಳ_flight_ideal) {
    ಫಲಿತಾಂಶ = CURLE_AGAIN;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = Curl_bufq_write(&ಹರಿವು->sendbuf, buf, len, pnwritten);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                ಹರಿವು->id, len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    (ಶೂನ್ಯ)nghttp3_conn_resume_ಹರಿವು(ctx->h3conn, ಹರಿವು->id);
  }

  ಆದರೆ(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!ಫಲಿತಾಂಶ);
  ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ, &pktx);

  ಆದರೆ(*pnwritten > 0) {
    ಸ್ಥಿರಾಂಕ size_t delta = (2 * 1024);
    ಆದರೆ(ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ > (ಹರಿವು->tx_ಒಳ_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      ಹರಿವು->tx_ಒಳ_flight_ideal += delta;
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  ಹರಿವು->id, len, *pnwritten,
                  ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ, ಹರಿವು->tx_ಒಳ_flight_ideal);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ + delta) < ಹರಿವು->tx_ಒಳ_flight_ideal) {
      /* ngtcp2 keeps less in flight than we try to provide for,
       * decrease our "ideal" tx buffer length */
      ಹರಿವು->tx_ಒಳ_flight_ideal -= delta;
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, reduce tx_win=%zu",
                  ಹರಿವು->id, len, *pnwritten,
                  ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ, ಹರಿವು->tx_ಒಳ_flight_ideal);
    }
  }

ಹೊರಹೋಗು:
  ಫಲಿತಾಂಶ = Curl_1st_fatal(ಫಲಿತಾಂಶ,
                          Curl_cf_ngtcp2_cmn_ಗಣ_expiry(cf, ದತ್ತಾಂಶ, &pktx));
  ಆದರೆ(ctx->tls_vrfy_ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = ctx->tls_vrfy_ಫಲಿತಾಂಶ;
denied:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              ಹರಿವು ? ಹರಿವು->id : -1, len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten);
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode h3_ದತ್ತಾಂಶ_pause(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (ಶೂನ್ಯ)cf;
  ಆದರೆ(!pause)
    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_ngtcp2_cntrl(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  ಆಯ್ಕೆ(event) {
  ಸ್ಥಿತಿ CF_CTRL_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_ದತ್ತಾಂಶ_PAUSE:
    ಫಲಿತಾಂಶ = h3_ದತ್ತಾಂಶ_pause(cf, ದತ್ತಾಂಶ, (arg1 != 0));
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_ದತ್ತಾಂಶ_ಮುಗಿದ:
    Curl_cf_ngtcp2_h3_ಹರಿವು_ಮುಗಿದ(cf, ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_ದತ್ತಾಂಶ_ಮುಗಿದ_SEND: {
    ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
    ಆದರೆ(ಹರಿವು && !ಹರಿವು->send_closed) {
      ಹರಿವು->send_closed = TRUE;
      ಹರಿವು->upload_ಎಡ = Curl_bufq_len(&ಹರಿವು->sendbuf) -
        ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ;
      (ಶೂನ್ಯ)nghttp3_conn_resume_ಹರಿವು(ctx->h3conn, ಹರಿವು->id);
    }
    ನಿಲ್ಲಿಸು;
  }
  ಸ್ಥಿತಿ CF_CTRL_CONN_INFO_UPDATE:
    ಆದರೆ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_ನೋಡಿದ = 30;
      Curl_conn_ಗಣ_multiplex(cf->conn);
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_cmn_cntrl(cf, ದತ್ತಾಂಶ, event, arg1, arg2);
    ನಿಲ್ಲಿಸು;
  }
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ngtcp2_destroy(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "destroy");
  ಆದರೆ(ctx) {
    ಆದರೆ(ctx->qconn) {
      ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
      CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
      Curl_cf_ngtcp2_cmn_conn_close(cf, ದತ್ತಾಂಶ);
      CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = ಖಾಲಿ;
  }
}

ಸ್ಥಿರ CURLcode cf_ngtcp2_connect(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  bool *ಮುಗಿದ)
{
  ಹಿಂತಿರುಗಿ Curl_cf_ngtcp2_cmn_connect(cf, ದತ್ತಾಂಶ, ಮುಗಿದ);
}

ಸ್ಥಿರ CURLcode cf_ngtcp2_query(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;

  ಆಯ್ಕೆ(query) {
  ಸ್ಥಿತಿ CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    ಆದರೆ(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      ಆದರೆ(ctx->max_bidi_streams > ctx->ಬಳಸಿದ_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->ಬಳಸಿದ_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ : (ಪೂರ್ಣಾಂಕ)max_streams;
    }
    ಇಲ್ಲದಿದ್ದರೆ {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(ದತ್ತಾಂಶ->multi);
      *pres1 = (n < ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? (ಪೂರ್ಣಾಂಕ)n : ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
    }
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಸ್ಥಿತಿ CF_QUERY_HTTP_ಆವೃತ್ತಿ:
    *pres1 = 30;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CF_QUERY_SSL_INFO:
  ಸ್ಥಿತಿ CF_QUERY_SSL_CTX_INFO: {
    ರಚನೆ curl_tlssessioninfo *info = pres2;
    ಆದರೆ(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      ಹಿಂತಿರುಗಿ CURLE_OK;
    ನಿಲ್ಲಿಸು;
  }
  ಸ್ಥಿತಿ CF_QUERY_ALPN_NEGOTIATED: {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ Curl_cf_ngtcp2_cmn_query(cf, ದತ್ತಾಂಶ, query, pres1, pres2);
}

ರಚನೆ Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_ngtcp2_destroy,
  cf_ngtcp2_connect,
  Curl_cf_ngtcp2_cmn_shutdown,
  cf_ngtcp2_adjust_pollset,
  Curl_cf_def_ದತ್ತಾಂಶ_pending,
  cf_ngtcp2_send,
  cf_ngtcp2_recv,
  cf_ngtcp2_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ngtcp2_query,
};

CURLcode Curl_cf_ngtcp2_create(ರಚನೆ Curl_cfilter **pcf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_peer *origin,
                               ರಚನೆ Curl_peer *peer,
                               ರಚನೆ connectdata *conn,
                               ರಚನೆ Curl_sockaddr_ex *addr)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = ಖಾಲಿ;
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  cf->conn = conn;

  ಫಲಿತಾಂಶ = Curl_cf_udp_create(&cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ಖಾಲಿ, TRNSPRT_QUIC);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  cf->ಮುಂದಿನ->conn = cf->conn;
  cf->ಮುಂದಿನ->sockindex = cf->sockindex;

ಹೊರಹೋಗು:
  *pcf = (!ಫಲಿತಾಂಶ) ? cf : ಖಾಲಿ;
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(cf)
      Curl_conn_cf_discard_chain(&cf, ದತ್ತಾಂಶ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_ngtcp2_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                     ರಚನೆ Curl_peer *origin,
                                     ರಚನೆ Curl_peer *peer,
                                     ರಚನೆ Curl_cfilter **pcf)
{
  ರಚನೆ cf_ngtcp2_ctx *ctx = ಖಾಲಿ;
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    curlx_safefree(cf);
    ಆದರೆ(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ
