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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "http_proxy.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"
#ಅನ್ತರ್ಭಾವ "vquic/cf-ngtcp2-cmn.h"
#ಅನ್ತರ್ಭಾವ "vquic/cf-ngtcp2-proxy.h"
#ಅನ್ತರ್ಭಾವ "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#ವ್ಯಾಖ್ಯಾನಿಸು PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_ಹರಿವು_CHUNK_ಗಾತ್ರ)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

ರಚನೆ h3_tunnel_ಹರಿವು {
  ರಚನೆ Curl_peer *dest;  /* where the tunnel goes to */
  ರಚನೆ http_resp *resp;
  ರಚನೆ bufq recvbuf;
  ಅಕ್ಷರ *authority;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  h3_tunnel_state state;
  ಬಿಟ್(udp);
  ಬಿಟ್(has_final_response);
  ಬಿಟ್(closed);
};

ಸ್ಥಿರ CURLcode h3_tunnel_ಹರಿವು_init(ರಚನೆ h3_tunnel_ಹರಿವು *ts,
                                      ರಚನೆ Curl_peer *dest,
                                      bool udp)
{
  ts->state = H3_TUNNEL_INIT;
  Curl_peer_link(&ts->dest, dest);
  Curl_bufq_init2(&ts->recvbuf, H3_ಹರಿವು_CHUNK_ಗಾತ್ರ,
                  PROXY_H3_TUNNEL_RECV_CHUNKS, BUFQ_OPT_SOFT_ಎಲ್ಲೆ);
  ts->udp = udp;
  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->ಪೋರ್ಟ್);
  ಆದರೆ(!ts->authority)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_tunnel_ಹರಿವು_reset(ರಚನೆ h3_tunnel_ಹರಿವು *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = ಖಾಲಿ;
  ts->ಹರಿವು = ಖಾಲಿ;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_tunnel_ಹರಿವು_cleanup(ರಚನೆ h3_tunnel_ಹರಿವು *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_tunnel_go_state(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ h3_tunnel_ಹರಿವು *ts,
                               h3_tunnel_state new_state,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  VERBOSE(int64_t ಹರಿವು_id = ts->ಹರಿವು ? ts->ಹರಿವು->id : -1);
  (ಶೂನ್ಯ)cf;

  ಆದರೆ(ts->state == new_state)
    ಹಿಂತಿರುಗಿ;

  /* leaving this one */
  ಆಯ್ಕೆ(ts->state) {
  ಸ್ಥಿತಿ H3_TUNNEL_CONNECT:
    ದತ್ತಾಂಶ->req.ignorebody = FALSE;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }

  /* entering this one */
  ಆಯ್ಕೆ(new_state) {
  ಸ್ಥಿತಿ H3_TUNNEL_INIT:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] -> [init]", ಹರಿವು_id);
    h3_tunnel_ಹರಿವು_reset(ts);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ H3_TUNNEL_CONNECT:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] -> [connect]", ಹರಿವು_id);
    ts->state = H3_TUNNEL_CONNECT;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] -> [response]", ಹರಿವು_id);
    ts->state = H3_TUNNEL_RESPONSE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] -> [established]", ಹರಿವು_id);
    infof(ದತ್ತಾಂಶ, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    ದತ್ತಾಂಶ->state.authproxy.ಮುಗಿದ = TRUE;
    ದತ್ತಾಂಶ->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(ದತ್ತಾಂಶ->req.hd_proxy_auth);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ H3_TUNNEL_FAILED:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] -> [failed]", ಹರಿವು_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(ದತ್ತಾಂಶ->req.hd_proxy_auth);
    ನಿಲ್ಲಿಸು;
  }
}

ರಚನೆ cf_h3_proxy_ctx {
  ರಚನೆ cf_ngtcp2_ctx ngtcp2_ctx;
  ರಚನೆ h3_tunnel_ಹರಿವು tunnel; /* our tunnel CONNECT stream */
  ಬಿಟ್(connected);
};

ಸ್ಥಿರ CURLcode cf_ngtcp2_proxy_h3_init(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ cf_ngtcp2_ctx *ctx);

ಸ್ಥಿರ CURLcode cf_h3_proxy_ctx_init(ರಚನೆ cf_h3_proxy_ctx *ctx,
                                     ರಚನೆ Curl_peer *origin,
                                     ರಚನೆ Curl_peer *peer,
                                     ರಚನೆ ssl_filter_config *sslc,
                                     ರಚನೆ Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = h3_tunnel_ಹರಿವು_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h3_proxy_ctx_free(ರಚನೆ cf_h3_proxy_ctx *ctx)
{
  ಆದರೆ(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_ಹರಿವು_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                                      uint64_t datalen, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                      ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  size_t skiplen;
  (ಶೂನ್ಯ)ಹರಿವು_user_ದತ್ತಾಂಶ;

  ಹರಿವು = pctx->tunnel.ಹರಿವು;
  ಆದರೆ(!ಹರಿವು || (ಹರಿವು->id != ಹರಿವು_id))
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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_proxy_ಹರಿವು_close(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                                    uint64_t app_error_ಸಂಕೇತ, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                    ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;

  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ಹರಿವು_user_ದತ್ತಾಂಶ;
  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;

  ಹರಿವು = pctx->tunnel.ಹರಿವು;
  ಆದರೆ(!ಹರಿವು || (ಹರಿವು->id != ಹರಿವು_id))
    ಹಿಂತಿರುಗಿ 0;

  ಹರಿವು->closed = TRUE;
  ಹರಿವು->error3 = app_error_ಸಂಕೇತ;
  ಆದರೆ(ಹರಿವು->error3 != NGHTTP3_H3_NO_ERROR) {
    ಹರಿವು->reset = TRUE;
    ಹರಿವು->send_closed = TRUE;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                ಹರಿವು->id, ಹರಿವು->error3);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] CLOSED", ಹರಿವು->id);
  pctx->tunnel.ಹರಿವು = ಖಾಲಿ;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h3_proxy_upd_rx_win(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು)
{
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  ಆದರೆ(!ಹರಿವು) /* already closed */
    ಹಿಂತಿರುಗಿ;
  ಆದರೆ(ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max < ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max - ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ;
  ಆದರೆ(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    ಆದರೆ(UINT64_MAX - delta < ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max)
      delta = UINT64_MAX - ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max;
    ಆದರೆ(delta) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  ಹರಿವು->id, cur_win, rx_buffered, delta);
      ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max += delta;
      ngtcp2_conn_extend_max_ಹರಿವು_ಸ್ಥಾನಾಂತರ(ctx->qconn, ಹರಿವು->id, delta);
      ngtcp2_conn_extend_max_ಸ್ಥಾನಾಂತರ(ctx->qconn, delta);
    }
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_proxy_recv_ದತ್ತಾಂಶ(nghttp3_conn *conn, int64_t stream3_id,
                                 ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t buflen,
                                 ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream3_id;
  (ಶೂನ್ಯ)ಹರಿವು_user_ದತ್ತಾಂಶ;

  ಹರಿವು = pctx->tunnel.ಹರಿವು;
  ಆದರೆ(!ದತ್ತಾಂಶ || !ಹರಿವು || (ಹರಿವು->id != stream3_id)) {
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  ಹರಿವು->rx_ಒಟ್ಟು += (curl_off_t)buflen;
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              ಹರಿವು->id, buflen, ಹರಿವು->rx_ಒಟ್ಟು);

  ಫಲಿತಾಂಶ = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ || (nwritten < buflen)) {
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ += buflen;
  ಆದರೆ(ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max < ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ)
    ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max = ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              ಹರಿವು->id, buflen, ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ_max - ಹರಿವು->rx_ಸ್ಥಾನಾಂತರ);
  cf_h3_proxy_upd_rx_win(cf, ದತ್ತಾಂಶ, ಹರಿವು);

  Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                                        size_t consumed, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                        ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ಹರಿವು_user_ದತ್ತಾಂಶ;

  ಆದರೆ(!ctx)
    ಹಿಂತಿರುಗಿ 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_ಹರಿವು_ಸ್ಥಾನಾಂತರ(ctx->qconn, ಹರಿವು_id, consumed);
  ngtcp2_conn_extend_max_ಸ್ಥಾನಾಂತರ(ctx->qconn, consumed);

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_proxy_recv_ಶೀರ್ಷಿಕೆ(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *ಮೌಲ್ಯ, uint8_t flags,
                                   ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(ಮೌಲ್ಯ);
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ http_ಸ್ಥಿತಿ;
  ರಚನೆ http_resp *resp;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ಹರಿವು_id;
  (ಶೂನ್ಯ)token;
  (ಶೂನ್ಯ)flags;
  (ಶೂನ್ಯ)ಹರಿವು_user_ದತ್ತಾಂಶ;

  /* stream_user_data might be NULL for control streams */
  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;

  ಹರಿವು = pctx->tunnel.ಹರಿವು;
  ಆದರೆ(!ಹರಿವು || (ಹರಿವು->id != ಹರಿವು_id)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                ಹರಿವು_id, (ಶೂನ್ಯ *)ದತ್ತಾಂಶ, ದತ್ತಾಂಶ ? ದತ್ತಾಂಶ->mid : 0);
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(token == NGHTTP3_QPACK_TOKEN_ಸ್ಥಿತಿ) {
    ಫಲಿತಾಂಶ = Curl_http_decode_ಸ್ಥಿತಿ(&ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ,
                                     (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)h3val.ತಳ, h3val.len);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
    http_ಸ್ಥಿತಿ = ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ;
    ಫಲಿತಾಂಶ = Curl_http_resp_make(&resp, http_ಸ್ಥಿತಿ, ಖಾಲಿ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
    ಆದರೆ(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(!pctx->tunnel.resp) {
      ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] header: %.*s: %.*s", ಹರಿವು_id,
                (ಪೂರ್ಣಾಂಕ)h3name.len, h3name.ತಳ, (ಪೂರ್ಣಾಂಕ)h3val.len, h3val.ತಳ);
    ಫಲಿತಾಂಶ = Curl_dynhds_ಕೂಡಿಸು(&pctx->tunnel.resp->ಶೀರ್ಷಿಕೆಗಳು,
                             (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)h3name.ತಳ, h3name.len,
                             (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)h3val.ತಳ, h3val.len);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_proxy_ಕೊನೆ_ಶೀರ್ಷಿಕೆಗಳು(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                                   ಪೂರ್ಣಾಂಕ fin, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                   ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ಹರಿವು_id;
  (ಶೂನ್ಯ)fin;
  (ಶೂನ್ಯ)ಹರಿವು_user_ದತ್ತಾಂಶ;

  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;

  ಹರಿವು = pctx->tunnel.ಹರಿವು;
  ಆದರೆ(!ಹರಿವು || (ಹರಿವು->id != ಹರಿವು_id)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                ಹರಿವು_id, (ಶೂನ್ಯ *)ದತ್ತಾಂಶ, ದತ್ತಾಂಶ ? ದತ್ತಾಂಶ->mid : 0);
    ಹಿಂತಿರುಗಿ 0;
  }

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] end_headers, status=%d", ಹರಿವು_id,
              ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ);

  ಆದರೆ(!pctx->tunnel.has_final_response) {
    ಆದರೆ(ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  ಆದರೆ(ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ / 100 != 1) {
    ಹರಿವು->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                                    uint64_t app_error_ಸಂಕೇತ, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                    ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ಹರಿವು_user_ದತ್ತಾಂಶ;

  ಆದರೆ(ctx) {
    ಪೂರ್ಣಾಂಕ rv = ngtcp2_conn_shutdown_ಹರಿವು_read(ctx->qconn, 0, ಹರಿವು_id,
                                              app_error_ಸಂಕೇತ);

    ಆದರೆ(rv && rv != NGTCP2_ERR_ಹರಿವು_NOT_ಸಿಕ್ಕಿತು) {
      ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_h3_proxy_reset_ಹರಿವು(nghttp3_conn *conn, int64_t ಹರಿವು_id,
                                    uint64_t app_error_ಸಂಕೇತ, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                    ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  ಪೂರ್ಣಾಂಕ rv;

  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ಹರಿವು_user_ದತ್ತಾಂಶ;
  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;

  ಆದರೆ(!pctx->tunnel.ಹರಿವು ||
     (ಹರಿವು_id != pctx->tunnel.ಹರಿವು->id))
    ಹಿಂತಿರುಗಿ 0;

  rv = ngtcp2_conn_shutdown_ಹರಿವು_write(ctx->qconn, 0, ಹರಿವು_id,
                                         app_error_ಸಂಕೇತ);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] reset -> %d", ಹರಿವು_id, rv);
  pctx->tunnel.ಹರಿವು = ಖಾಲಿ;
  pctx->tunnel.closed = TRUE;
  ಆದರೆ(rv && rv != NGTCP2_ERR_ಹರಿವು_NOT_ಸಿಕ್ಕಿತು) {
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ nghttp3_ssize cb_h3_tunnel_read_ದತ್ತಾಂಶ(nghttp3_conn *conn,
                                            int64_t ಹರಿವು_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                            ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf = user_ದತ್ತಾಂಶ;
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
  size_t nwritten = 0;
  size_t nvecs = 0;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf_ತಳ;

  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)ಹರಿವು_id;
  (ಶೂನ್ಯ)veccnt;
  (ಶೂನ್ಯ)ಹರಿವು_user_ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)pflags;

  ಹರಿವು = pctx->tunnel.ಹರಿವು;
  ಆದರೆ(!ದತ್ತಾಂಶ || !ಹರಿವು || (ಹರಿವು->id != ಹರಿವು_id))
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  ಆದರೆ(ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ < Curl_bufq_len(&ಹರಿವು->sendbuf)) {
    nvecs = 0;
    ಆಗ(nvecs < veccnt) {
      ಆದರೆ(!Curl_bufq_peek_at(&ಹರಿವು->sendbuf,
                           ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ,
                           &buf_ತಳ,
                           &vec[nvecs].len))
        ನಿಲ್ಲಿಸು;
      vec[nvecs].ತಳ = (uint8_t *)(uintptr_t)buf_ತಳ;
      ಹರಿವು->tx_ಒಳ_flight_ಗಾತ್ರ += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  ಆದರೆ(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] read req body -> AGAIN",
                ಹರಿವು->id);
    ಹಿಂತಿರುಗಿ NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              ಹರಿವು->id, nvecs,
              *pflags == NGHTTP3_ದತ್ತಾಂಶ_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&ಹರಿವು->sendbuf));
  ಹಿಂತಿರುಗಿ (nghttp3_ssize)nvecs;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_DIAG
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wmissing-field-initializers"
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_ಹರಿವು_close,
  cb_h3_proxy_recv_ದತ್ತಾಂಶ,
  cb_h3_proxy_deferred_consume,
  ಖಾಲಿ, /* begin_headers */
  cb_h3_proxy_recv_ಶೀರ್ಷಿಕೆ,
  cb_h3_proxy_ಕೊನೆ_ಶೀರ್ಷಿಕೆಗಳು,
  ಖಾಲಿ, /* begin_trailers */
  cb_h3_proxy_recv_ಶೀರ್ಷಿಕೆ,
  ಖಾಲಿ, /* end_trailers */
  cb_h3_proxy_stop_sending,
  ಖಾಲಿ, /* end_stream */
  cb_h3_proxy_reset_ಹರಿವು,
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

ಸ್ಥಿರ CURLcode cf_ngtcp2_proxy_h3_init(ರಚನೆ Curl_cfilter *cf,
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
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "error creating nghttp3 connection instance");
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಹಿಂತಿರುಗಿ Curl_cf_ngtcp2_h3_init_ctrls(ctx, ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ ssize_t cf_h3_proxy_recv_closed_ಹರಿವು(ರಚನೆ Curl_cfilter *cf,
                                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                              ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  ಆದರೆ(ಹರಿವು->reset) {
    ಆದರೆ(ಹರಿವು->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(ದತ್ತಾಂಶ, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", ಹರಿವು->id);
      connclose(cf->conn);
      ದತ್ತಾಂಶ->state.refused_ಹರಿವು = TRUE;
      *err = CURLE_RECV_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->resp_hds_complete && ದತ್ತಾಂಶ->req.no_body) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", ಹರಿವು->id, ಹರಿವು->error3,
                  Curl_vquic_h3_err_str(ಹರಿವು->error3));
      nread = 0;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    failf(ದತ್ತಾಂಶ, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", ಹರಿವು->id, ಹರಿವು->error3,
          Curl_vquic_h3_err_str(ಹರಿವು->error3));
    *err = ದತ್ತಾಂಶ->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಹರಿವು->resp_hds_complete) {
    failf(ದತ್ತಾಂಶ,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          ಹರಿವು->id);
    *err = CURLE_HTTP3;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  nread = 0;

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ nread;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_sendbuf_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು,
                                        ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode ಫಲಿತಾಂಶ;
  *pnwritten = 0;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_bufq_write(&ಹರಿವು->sendbuf, buf, len, pnwritten);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_send(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = ಖಾಲಿ;
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

  ಆದರೆ(pctx->tunnel.closed) {
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು denied;
  }

  ಹರಿವು = pctx->tunnel.ಹರಿವು;
  ಆದರೆ(!ಹರಿವು) {
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು denied;
  }

  ಆದರೆ(ಹರಿವು->closed) {
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
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = cf_h3_proxy_sendbuf_ಕೂಡಿಸು(ದತ್ತಾಂಶ, ಹರಿವು, buf, len, pnwritten);
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

ಹೊರಹೋಗು:
  ಫಲಿತಾಂಶ = Curl_1st_fatal(ಫಲಿತಾಂಶ,
                          Curl_cf_ngtcp2_cmn_ಗಣ_expiry(cf, ದತ್ತಾಂಶ, &pktx));
denied:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              ಹರಿವು ? ಹರಿವು->id : -1, len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten);
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* incoming data frames on the h3 stream */
ಸ್ಥಿರ CURLcode cf_h3_proxy_recv(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಅಕ್ಷರ *buf, size_t len, size_t *pnread)
{
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = pctx->tunnel.ಹರಿವು;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  ರಚನೆ cf_ngtcp2_io_ctx pktx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

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

  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&pctx->tunnel.recvbuf)) {
    ಫಲಿತಾಂಶ = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    cf_h3_proxy_upd_rx_win(cf, ದತ್ತಾಂಶ, pctx->tunnel.ಹರಿವು);
  }

  ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ, &pktx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* inbufq had nothing before, maybe after progressing ingress? */
  ಆದರೆ(!*pnread && !Curl_bufq_is_ಖಾಲಿಗಳು(&pctx->tunnel.recvbuf)) {
    ಫಲಿತಾಂಶ = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  ಹರಿವು->id, len, *pnread, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಹೋಗು ಹೊರಹೋಗು;
    }
    cf_h3_proxy_upd_rx_win(cf, ದತ್ತಾಂಶ, pctx->tunnel.ಹರಿವು);
  }

  ಆದರೆ(*pnread) {
    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] xfer write failed", ಹರಿವು->id);
      Curl_cf_ngtcp2_h3_ಹರಿವು_close(cf, ದತ್ತಾಂಶ, ಹರಿವು);
      ಫಲಿತಾಂಶ = ಹರಿವು->xfer_ಫಲಿತಾಂಶ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_ಹರಿವು(cf, ದತ್ತಾಂಶ, ಹರಿವು, &ಫಲಿತಾಂಶ);
      ಆದರೆ(nread > 0)
        *pnread = (size_t)nread;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಫಲಿತಾಂಶ = CURLE_AGAIN;
  }

ಹೊರಹೋಗು:
  ಫಲಿತಾಂಶ = Curl_1st_fatal(ಫಲಿತಾಂಶ,
                          Curl_cf_ngtcp2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ, &pktx));
  ಫಲಿತಾಂಶ = Curl_1st_fatal(ಫಲಿತಾಂಶ,
                          Curl_cf_ngtcp2_cmn_ಗಣ_expiry(cf, ದತ್ತಾಂಶ, &pktx));
denied:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] cf_recv(len=%zu) -> %d, %zu",
              ಹರಿವು ? ಹರಿವು->id : -1, len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread);
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_submit(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ h3_tunnel_ಹರಿವು *ts,
                                   ರಚನೆ httpreq *req)
{
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = ಖಾಲಿ;
  ರಚನೆ dynhds h2_ಶೀರ್ಷಿಕೆಗಳು;
  nghttp3_nv *nva = ಖಾಲಿ;
  size_t nheader;
  ಪೂರ್ಣಾಂಕ rc = 0;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
  nghttp3_ದತ್ತಾಂಶ_reader reader;
  nghttp3_ದತ್ತಾಂಶ_reader *preader = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  Curl_dynhds_init(&h2_ಶೀರ್ಷಿಕೆಗಳು, 0, DYN_HTTP_REQUEST);
  ಫಲಿತಾಂಶ = Curl_http_req_ಗಮ್ಯ_h2(&h2_ಶೀರ್ಷಿಕೆಗಳು, req, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_h3_ಹರಿವು_ಸಿದ್ಧತೆ(cf, ದತ್ತಾಂಶ, &ಹರಿವು);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

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

  /* Open a bidirectional stream */
  {
    int64_t sid;
    ಪೂರ್ಣಾಂಕ rv;

    DEBUGASSERT(ಹರಿವು->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_ಹರಿವು(ctx->qconn, &sid, ಖಾಲಿ);
    ಆದರೆ(rv) {
      failf(ದತ್ತಾಂಶ, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಹರಿವು->id = sid;
    ts->ಹರಿವು = ಹರಿವು;
    ++ctx->ಬಳಸಿದ_bidi_streams;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRId64 "] opened bidi stream", sid);
  }

  /* CONNECT-UDP request stream remains open for capsules, no fixed EOF. */
  ಹರಿವು->send_closed = 0;
  reader.read_ದತ್ತಾಂಶ = cb_h3_tunnel_read_ದತ್ತಾಂಶ;
  preader = &reader;

  rc = nghttp3_conn_submit_request(ctx->h3conn, ಹರಿವು->id,
                                   nva, nheader, preader, ಹರಿವು);

  ಆದರೆ(rc) {
    ಆಯ್ಕೆ(rc) {
    ಸ್ಥಿತಿ NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", ಹರಿವು->id);
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  ಹರಿವು->id, rc, nghttp3_strerror(rc));
      ನಿಲ್ಲಿಸು;
    }
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(Curl_trc_is_verbose(ದತ್ತಾಂಶ)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", ಹರಿವು->id,
                Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url));
  }

ಹೊರಹೋಗು:
  curlx_free(nva);
  Curl_dynhds_free(&h2_ಶೀರ್ಷಿಕೆಗಳು);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ);

  ಆದರೆ(!ctx->qconn || !pctx->tunnel.ಹರಿವು || (sock == CURL_SOCKET_BAD))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  Curl_pollset_check(ದತ್ತಾಂಶ, ps, sock, &want_recv, &want_send);

  ಆದರೆ(want_recv || want_send || !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->q.sendbuf)) {
    ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = pctx->tunnel.ಹರಿವು;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_ಎಡ(ctx->qconn) ||
                 !ngtcp2_conn_get_max_ದತ್ತಾಂಶ_ಎಡ(ctx->qconn));
    s_exhaust = want_send && ಹರಿವು && ಹರಿವು->id >= 0 &&
                ಹರಿವು->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->q.sendbuf);

    ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ(ದತ್ತಾಂಶ, ps, sock, want_recv, want_send);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool cf_h3_proxy_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                                     ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&pctx->tunnel.recvbuf))
    ಹಿಂತಿರುಗಿ TRUE;
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->has_ದತ್ತಾಂಶ_pending(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ) : FALSE;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_submit_CONNECT(ರಚನೆ Curl_cfilter *cf,
                                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ h3_tunnel_ಹರಿವು *ts)
{
  ರಚನೆ cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ httpreq *req = ಖಾಲಿ;

  ಫಲಿತಾಂಶ = Curl_http_proxy_create_tunnel_request(
    &req, cf, ದತ್ತಾಂಶ, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_null(ದತ್ತಾಂಶ);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = cf_h3_proxy_submit(cf, ದತ್ತಾಂಶ, ts, req);

  ಆದರೆ(req)
    Curl_http_req_free(req);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    failf(ದತ್ತಾಂಶ, "Failed sending CONNECT to proxy");
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_inspect_response(ರಚನೆ Curl_cfilter *cf,
                                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ h3_tunnel_ಹರಿವು *ts)
{
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_ಫಲಿತಾಂಶ res;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_http_proxy_inspect_tunnel_response(
      cf, ದತ್ತಾಂಶ, ts->resp, (bool)pctx->tunnel.udp, &res);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಆಯ್ಕೆ(res) {
  ಸ್ಥಿತಿ PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, ದತ್ತಾಂಶ);
    ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_tunnel(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ h3_tunnel_ಹರಿವು *ts,
                                   bool *pdone)
{
  ರಚನೆ cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  ಮಾಡು {
    ಆಯ್ಕೆ(ts->state) {
    ಸ್ಥಿತಿ H3_TUNNEL_INIT:
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] CONNECT start for %s", ts->authority);
      ಫಲಿತಾಂಶ = cf_h3_proxy_submit_CONNECT(cf, ದತ್ತಾಂಶ, ts);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, ದತ್ತಾಂಶ);

      ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ, ಖಾಲಿ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      FALLTHROUGH();

    ಸ್ಥಿತಿ H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ, ಖಾಲಿ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ, ಖಾಲಿ);
      ಆದರೆ(ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, ದತ್ತಾಂಶ);
        ಹೋಗು ಹೊರಹೋಗು;
      }

      ಆದರೆ(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, ದತ್ತಾಂಶ);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* Not done yet, return and let multi interface call us again */
        ಫಲಿತಾಂಶ = CURLE_OK;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      FALLTHROUGH();

    ಸ್ಥಿತಿ H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      ಫಲಿತಾಂಶ = cf_h3_proxy_inspect_response(cf, ದತ್ತಾಂಶ, ts);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      ctx->connected = TRUE;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_OK;

    ಸ್ಥಿತಿ H3_TUNNEL_FAILED:
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;

    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }

  } ಆಗ(ts->state == H3_TUNNEL_INIT);

ಹೊರಹೋಗು:
  ಆದರೆ((ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_connect(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ cf_h3_proxy_ctx *pctx = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save = { 0 };
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ h3_tunnel_ಹರಿವು *ts = &pctx->tunnel;
  bool ದತ್ತಾಂಶ_saved = FALSE;

  ಫಲಿತಾಂಶ = Curl_cf_ngtcp2_cmn_connect(cf, ದತ್ತಾಂಶ, ಮುಗಿದ);
  ಆದರೆ(ಫಲಿತಾಂಶ || !*ಮುಗಿದ)
    ಹೋಗು ಹೊರಹೋಗು;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  ದತ್ತಾಂಶ_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  ಫಲಿತಾಂಶ = cf_h3_proxy_tunnel(cf, ದತ್ತಾಂಶ, ts, ಮುಗಿದ);

ಹೊರಹೋಗು:
  ಆದರೆ(*ಮುಗಿದ) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&ದತ್ತಾಂಶ->req, ದತ್ತಾಂಶ);
    Curl_client_reset(ದತ್ತಾಂಶ);
  }

  ಆದರೆ(ದತ್ತಾಂಶ_saved)
    CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h3_proxy_destroy(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h3_proxy_ctx *ctx = cf->ctx;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(ctx) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_h3_proxy_destroy()");
    ಆದರೆ(ctx->ngtcp2_ctx.qconn) {
      ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
      CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
      Curl_cf_ngtcp2_cmn_conn_close(cf, ದತ್ತಾಂಶ);
      CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = ಖಾಲಿ;
  }
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_shutdown(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ಹಿಂತಿರುಗಿ Curl_cf_ngtcp2_cmn_shutdown(cf, ದತ್ತಾಂಶ, ಮುಗಿದ);
}

ರಚನೆ Curl_cftype Curl_cft_h3_proxy = {
  "H3-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  cf_h3_proxy_destroy,
  cf_h3_proxy_connect,
  cf_h3_proxy_shutdown,
  cf_h3_proxy_adjust_pollset,
  cf_h3_proxy_ದತ್ತಾಂಶ_pending,
  cf_h3_proxy_send,
  cf_h3_proxy_recv,
  Curl_cf_def_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_ngtcp2_cmn_query,
};

CURLcode Curl_cf_ngtcp2_proxy_create(ರಚನೆ Curl_cfilter **pcf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ Curl_peer *origin,
                                     ರಚನೆ Curl_peer *peer,
                                     uint8_t transport_peer,
                                     ರಚನೆ connectdata *conn,
                                     ರಚನೆ Curl_sockaddr_ex *addr,
                                     ರಚನೆ Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  ರಚನೆ cf_h3_proxy_ctx *ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(!tunnel_peer)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಆದರೆ((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಫಲಿತಾಂಶ = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
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
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_ngtcp2_proxy_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ Curl_peer *origin,
                                           ರಚನೆ Curl_peer *peer,
                                           ರಚನೆ Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  ರಚನೆ cf_h3_proxy_ctx *ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;

  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx)
    ಹೋಗು ಹೊರಹೋಗು;
  ಫಲಿತಾಂಶ = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(cf)
      Curl_conn_cf_discard_chain(&cf, ದತ್ತಾಂಶ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ
