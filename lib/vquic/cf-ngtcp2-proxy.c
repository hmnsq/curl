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

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/cf-ngtcp2-cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/cf-ngtcp2-proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_ਧਾਰਾ_CHUNK_ਮਾਪ)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

ਬਣਤਰ h3_tunnel_ਧਾਰਾ {
  ਬਣਤਰ Curl_peer *dest;  /* where the tunnel goes to */
  ਬਣਤਰ http_resp *resp;
  ਬਣਤਰ bufq recvbuf;
  ਅੱਖਰ *authority;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  h3_tunnel_state state;
  ਦੋਆਧਾਰੀ_ਅੰਕ(udp);
  ਦੋਆਧਾਰੀ_ਅੰਕ(has_final_response);
  ਦੋਆਧਾਰੀ_ਅੰਕ(closed);
};

ਸਥਿਰ CURLcode h3_tunnel_ਧਾਰਾ_init(ਬਣਤਰ h3_tunnel_ਧਾਰਾ *ts,
                                      ਬਣਤਰ Curl_peer *dest,
                                      bool udp)
{
  ts->state = H3_TUNNEL_INIT;
  Curl_peer_link(&ts->dest, dest);
  Curl_bufq_init2(&ts->recvbuf, H3_ਧਾਰਾ_CHUNK_ਮਾਪ,
                  PROXY_H3_TUNNEL_RECV_CHUNKS, BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);
  ts->udp = udp;
  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->ਪੋਰਟ);
  ਜੇ(!ts->authority)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ h3_tunnel_ਧਾਰਾ_reset(ਬਣਤਰ h3_tunnel_ਧਾਰਾ *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = ਨਿਰਾ;
  ts->ਧਾਰਾ = ਨਿਰਾ;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

ਸਥਿਰ ਖਾਲੀ h3_tunnel_ਧਾਰਾ_cleanup(ਬਣਤਰ h3_tunnel_ਧਾਰਾ *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

ਸਥਿਰ ਖਾਲੀ h3_tunnel_go_state(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ h3_tunnel_ਧਾਰਾ *ts,
                               h3_tunnel_state new_state,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  VERBOSE(int64_t ਧਾਰਾ_id = ts->ਧਾਰਾ ? ts->ਧਾਰਾ->id : -1);
  (ਖਾਲੀ)cf;

  ਜੇ(ts->state == new_state)
    ਵਾਪਸ;

  /* leaving this one */
  ਚੋਣ(ts->state) {
  ਹਾਲਤ H3_TUNNEL_CONNECT:
    ਅੰਕੜੇ->req.ignorebody = FALSE;
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }

  /* entering this one */
  ਚੋਣ(new_state) {
  ਹਾਲਤ H3_TUNNEL_INIT:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] -> [init]", ਧਾਰਾ_id);
    h3_tunnel_ਧਾਰਾ_reset(ts);
    ਰੋਕੋ;
  ਹਾਲਤ H3_TUNNEL_CONNECT:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] -> [connect]", ਧਾਰਾ_id);
    ts->state = H3_TUNNEL_CONNECT;
    ਰੋਕੋ;
  ਹਾਲਤ H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] -> [response]", ਧਾਰਾ_id);
    ts->state = H3_TUNNEL_RESPONSE;
    ਰੋਕੋ;
  ਹਾਲਤ H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] -> [established]", ਧਾਰਾ_id);
    infof(ਅੰਕੜੇ, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    ਅੰਕੜੇ->state.authproxy.ਪੂਰਾ = TRUE;
    ਅੰਕੜੇ->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(ਅੰਕੜੇ->req.hd_proxy_auth);
    ਰੋਕੋ;
  ਹਾਲਤ H3_TUNNEL_FAILED:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] -> [failed]", ਧਾਰਾ_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(ਅੰਕੜੇ->req.hd_proxy_auth);
    ਰੋਕੋ;
  }
}

ਬਣਤਰ cf_h3_proxy_ctx {
  ਬਣਤਰ cf_ngtcp2_ctx ngtcp2_ctx;
  ਬਣਤਰ h3_tunnel_ਧਾਰਾ tunnel; /* our tunnel CONNECT stream */
  ਦੋਆਧਾਰੀ_ਅੰਕ(connected);
};

ਸਥਿਰ CURLcode cf_ngtcp2_proxy_h3_init(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ cf_ngtcp2_ctx *ctx);

ਸਥਿਰ CURLcode cf_h3_proxy_ctx_init(ਬਣਤਰ cf_h3_proxy_ctx *ctx,
                                     ਬਣਤਰ Curl_peer *origin,
                                     ਬਣਤਰ Curl_peer *peer,
                                     ਬਣਤਰ ssl_filter_config *sslc,
                                     ਬਣਤਰ Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode ਨਤੀਜਾ;
  ਨਤੀਜਾ = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = h3_tunnel_ਧਾਰਾ_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_h3_proxy_ctx_free(ਬਣਤਰ cf_h3_proxy_ctx *ctx)
{
  ਜੇ(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_ਧਾਰਾ_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

ਸਥਿਰ ਪੂਰਨ cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                                      uint64_t datalen, ਖਾਲੀ *user_ਅੰਕੜੇ,
                                      ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  size_t skiplen;
  (ਖਾਲੀ)ਧਾਰਾ_user_ਅੰਕੜੇ;

  ਧਾਰਾ = pctx->tunnel.ਧਾਰਾ;
  ਜੇ(!ਧਾਰਾ || (ਧਾਰਾ->id != ਧਾਰਾ_id))
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

ਸਥਿਰ ਪੂਰਨ cb_h3_proxy_ਧਾਰਾ_close(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                                    uint64_t app_error_ਕੋਡ, ਖਾਲੀ *user_ਅੰਕੜੇ,
                                    ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;

  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)ਧਾਰਾ_user_ਅੰਕੜੇ;
  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;

  ਧਾਰਾ = pctx->tunnel.ਧਾਰਾ;
  ਜੇ(!ਧਾਰਾ || (ਧਾਰਾ->id != ਧਾਰਾ_id))
    ਵਾਪਸ 0;

  ਧਾਰਾ->closed = TRUE;
  ਧਾਰਾ->error3 = app_error_ਕੋਡ;
  ਜੇ(ਧਾਰਾ->error3 != NGHTTP3_H3_NO_ERROR) {
    ਧਾਰਾ->reset = TRUE;
    ਧਾਰਾ->send_closed = TRUE;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                ਧਾਰਾ->id, ਧਾਰਾ->error3);
  }
  ਨਹੀਂਤਾਂ
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] CLOSED", ਧਾਰਾ->id);
  pctx->tunnel.ਧਾਰਾ = ਨਿਰਾ;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(ਅੰਕੜੇ);
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ cf_h3_proxy_upd_rx_win(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ)
{
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  ਜੇ(!ਧਾਰਾ) /* already closed */
    ਵਾਪਸ;
  ਜੇ(ਧਾਰਾ->rx_ਵਿੱਥ_max < ਧਾਰਾ->rx_ਵਿੱਥ) {
    DEBUGASSERT(0);
    ਵਾਪਸ;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = ਧਾਰਾ->rx_ਵਿੱਥ_max - ਧਾਰਾ->rx_ਵਿੱਥ;
  ਜੇ(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    ਜੇ(UINT64_MAX - delta < ਧਾਰਾ->rx_ਵਿੱਥ_max)
      delta = UINT64_MAX - ਧਾਰਾ->rx_ਵਿੱਥ_max;
    ਜੇ(delta) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  ਧਾਰਾ->id, cur_win, rx_buffered, delta);
      ਧਾਰਾ->rx_ਵਿੱਥ_max += delta;
      ngtcp2_conn_extend_max_ਧਾਰਾ_ਵਿੱਥ(ctx->qconn, ਧਾਰਾ->id, delta);
      ngtcp2_conn_extend_max_ਵਿੱਥ(ctx->qconn, delta);
    }
  }
}

ਸਥਿਰ ਪੂਰਨ cb_h3_proxy_recv_ਅੰਕੜੇ(nghttp3_conn *conn, int64_t stream3_id,
                                 ਅਚਲ uint8_t *buf, size_t buflen,
                                 ਖਾਲੀ *user_ਅੰਕੜੇ, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  size_t nwritten;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)stream3_id;
  (ਖਾਲੀ)ਧਾਰਾ_user_ਅੰਕੜੇ;

  ਧਾਰਾ = pctx->tunnel.ਧਾਰਾ;
  ਜੇ(!ਅੰਕੜੇ || !ਧਾਰਾ || (ਧਾਰਾ->id != stream3_id)) {
    ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  ਧਾਰਾ->rx_ਕੁੱਲ += (curl_off_t)buflen;
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              ਧਾਰਾ->id, buflen, ਧਾਰਾ->rx_ਕੁੱਲ);

  ਨਤੀਜਾ = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  ਜੇ(ਨਤੀਜਾ || (nwritten < buflen)) {
    ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  ਧਾਰਾ->rx_ਵਿੱਥ += buflen;
  ਜੇ(ਧਾਰਾ->rx_ਵਿੱਥ_max < ਧਾਰਾ->rx_ਵਿੱਥ)
    ਧਾਰਾ->rx_ਵਿੱਥ_max = ਧਾਰਾ->rx_ਵਿੱਥ;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              ਧਾਰਾ->id, buflen, ਧਾਰਾ->rx_ਵਿੱਥ_max - ਧਾਰਾ->rx_ਵਿੱਥ);
  cf_h3_proxy_upd_rx_win(cf, ਅੰਕੜੇ, ਧਾਰਾ);

  Curl_multi_mark_dirty(ਅੰਕੜੇ);
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                                        size_t consumed, ਖਾਲੀ *user_ਅੰਕੜੇ,
                                        ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)ਧਾਰਾ_user_ਅੰਕੜੇ;

  ਜੇ(!ctx)
    ਵਾਪਸ 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_ਧਾਰਾ_ਵਿੱਥ(ctx->qconn, ਧਾਰਾ_id, consumed);
  ngtcp2_conn_extend_max_ਵਿੱਥ(ctx->qconn, consumed);

  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_h3_proxy_recv_ਸਿਰਲੇਖ(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *ਮੁੱਲ, uint8_t flags,
                                   ਖਾਲੀ *user_ਅੰਕੜੇ, ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(ਮੁੱਲ);
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ http_ਸਥਿਤੀ;
  ਬਣਤਰ http_resp *resp;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)ਧਾਰਾ_id;
  (ਖਾਲੀ)token;
  (ਖਾਲੀ)flags;
  (ਖਾਲੀ)ਧਾਰਾ_user_ਅੰਕੜੇ;

  /* stream_user_data might be NULL for control streams */
  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;

  ਧਾਰਾ = pctx->tunnel.ਧਾਰਾ;
  ਜੇ(!ਧਾਰਾ || (ਧਾਰਾ->id != ਧਾਰਾ_id)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                ਧਾਰਾ_id, (ਖਾਲੀ *)ਅੰਕੜੇ, ਅੰਕੜੇ ? ਅੰਕੜੇ->mid : 0);
    ਵਾਪਸ 0;
  }

  ਜੇ(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    ਵਾਪਸ 0;
  }

  ਜੇ(token == NGHTTP3_QPACK_TOKEN_ਸਥਿਤੀ) {
    ਨਤੀਜਾ = Curl_http_decode_ਸਥਿਤੀ(&ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ,
                                     (ਅਚਲ ਅੱਖਰ *)h3val.ਅਧਾਰ, h3val.len);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
    http_ਸਥਿਤੀ = ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ;
    ਨਤੀਜਾ = Curl_http_resp_make(&resp, http_ਸਥਿਤੀ, ਨਿਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
    ਜੇ(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(!pctx->tunnel.resp) {
      ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] header: %.*s: %.*s", ਧਾਰਾ_id,
                (ਪੂਰਨ)h3name.len, h3name.ਅਧਾਰ, (ਪੂਰਨ)h3val.len, h3val.ਅਧਾਰ);
    ਨਤੀਜਾ = Curl_dynhds_ਜੋੜੋ(&pctx->tunnel.resp->ਸਿਰਲੇਖਾਂ,
                             (ਅਚਲ ਅੱਖਰ *)h3name.ਅਧਾਰ, h3name.len,
                             (ਅਚਲ ਅੱਖਰ *)h3val.ਅਧਾਰ, h3val.len);
    ਜੇ(ਨਤੀਜਾ) {
      ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_h3_proxy_ਸਿਰਾ_ਸਿਰਲੇਖਾਂ(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                                   ਪੂਰਨ fin, ਖਾਲੀ *user_ਅੰਕੜੇ,
                                   ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)ਧਾਰਾ_id;
  (ਖਾਲੀ)fin;
  (ਖਾਲੀ)ਧਾਰਾ_user_ਅੰਕੜੇ;

  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;

  ਧਾਰਾ = pctx->tunnel.ਧਾਰਾ;
  ਜੇ(!ਧਾਰਾ || (ਧਾਰਾ->id != ਧਾਰਾ_id)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                ਧਾਰਾ_id, (ਖਾਲੀ *)ਅੰਕੜੇ, ਅੰਕੜੇ ? ਅੰਕੜੇ->mid : 0);
    ਵਾਪਸ 0;
  }

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] end_headers, status=%d", ਧਾਰਾ_id,
              ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ);

  ਜੇ(!pctx->tunnel.has_final_response) {
    ਜੇ(ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  ਜੇ(ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ / 100 != 1) {
    ਧਾਰਾ->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(ਅੰਕੜੇ);
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                                    uint64_t app_error_ਕੋਡ, ਖਾਲੀ *user_ਅੰਕੜੇ,
                                    ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)ਧਾਰਾ_user_ਅੰਕੜੇ;

  ਜੇ(ctx) {
    ਪੂਰਨ rv = ngtcp2_conn_shutdown_ਧਾਰਾ_read(ctx->qconn, 0, ਧਾਰਾ_id,
                                              app_error_ਕੋਡ);

    ਜੇ(rv && rv != NGTCP2_ERR_ਧਾਰਾ_NOT_ਮਿਲਿਆ) {
      ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ cb_h3_proxy_reset_ਧਾਰਾ(nghttp3_conn *conn, int64_t ਧਾਰਾ_id,
                                    uint64_t app_error_ਕੋਡ, ਖਾਲੀ *user_ਅੰਕੜੇ,
                                    ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  ਪੂਰਨ rv;

  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)ਧਾਰਾ_user_ਅੰਕੜੇ;
  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;

  ਜੇ(!pctx->tunnel.ਧਾਰਾ ||
     (ਧਾਰਾ_id != pctx->tunnel.ਧਾਰਾ->id))
    ਵਾਪਸ 0;

  rv = ngtcp2_conn_shutdown_ਧਾਰਾ_write(ctx->qconn, 0, ਧਾਰਾ_id,
                                         app_error_ਕੋਡ);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] reset -> %d", ਧਾਰਾ_id, rv);
  pctx->tunnel.ਧਾਰਾ = ਨਿਰਾ;
  pctx->tunnel.closed = TRUE;
  ਜੇ(rv && rv != NGTCP2_ERR_ਧਾਰਾ_NOT_ਮਿਲਿਆ) {
    ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ nghttp3_ssize cb_h3_tunnel_read_ਅੰਕੜੇ(nghttp3_conn *conn,
                                            int64_t ਧਾਰਾ_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            ਖਾਲੀ *user_ਅੰਕੜੇ,
                                            ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf = user_ਅੰਕੜੇ;
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
  size_t nwritten = 0;
  size_t nvecs = 0;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf_ਅਧਾਰ;

  (ਖਾਲੀ)conn;
  (ਖਾਲੀ)ਧਾਰਾ_id;
  (ਖਾਲੀ)veccnt;
  (ਖਾਲੀ)ਧਾਰਾ_user_ਅੰਕੜੇ;
  (ਖਾਲੀ)pflags;

  ਧਾਰਾ = pctx->tunnel.ਧਾਰਾ;
  ਜੇ(!ਅੰਕੜੇ || !ਧਾਰਾ || (ਧਾਰਾ->id != ਧਾਰਾ_id))
    ਵਾਪਸ NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  ਜੇ(ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ < Curl_bufq_len(&ਧਾਰਾ->sendbuf)) {
    nvecs = 0;
    ਜਦੋਂ(nvecs < veccnt) {
      ਜੇ(!Curl_bufq_peek_at(&ਧਾਰਾ->sendbuf,
                           ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ,
                           &buf_ਅਧਾਰ,
                           &vec[nvecs].len))
        ਰੋਕੋ;
      vec[nvecs].ਅਧਾਰ = (uint8_t *)(uintptr_t)buf_ਅਧਾਰ;
      ਧਾਰਾ->tx_ਵਿੱਚ_flight_ਮਾਪ += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  ਜੇ(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] read req body -> AGAIN",
                ਧਾਰਾ->id);
    ਵਾਪਸ NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              ਧਾਰਾ->id, nvecs,
              *pflags == NGHTTP3_ਅੰਕੜੇ_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&ਧਾਰਾ->sendbuf));
  ਵਾਪਸ (nghttp3_ssize)nvecs;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_DIAG
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wmissing-field-initializers"
#ਜੇ_ਅੰਤ
ਸਥਿਰ nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_ਧਾਰਾ_close,
  cb_h3_proxy_recv_ਅੰਕੜੇ,
  cb_h3_proxy_deferred_consume,
  ਨਿਰਾ, /* begin_headers */
  cb_h3_proxy_recv_ਸਿਰਲੇਖ,
  cb_h3_proxy_ਸਿਰਾ_ਸਿਰਲੇਖਾਂ,
  ਨਿਰਾ, /* begin_trailers */
  cb_h3_proxy_recv_ਸਿਰਲੇਖ,
  ਨਿਰਾ, /* end_trailers */
  cb_h3_proxy_stop_sending,
  ਨਿਰਾ, /* end_stream */
  cb_h3_proxy_reset_ਧਾਰਾ,
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

ਸਥਿਰ CURLcode cf_ngtcp2_proxy_h3_init(ਬਣਤਰ Curl_cfilter *cf,
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
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "error creating nghttp3 connection instance");
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਵਾਪਸ Curl_cf_ngtcp2_h3_init_ctrls(ctx, ਅੰਕੜੇ);
}

ਸਥਿਰ ssize_t cf_h3_proxy_recv_closed_ਧਾਰਾ(ਬਣਤਰ Curl_cfilter *cf,
                                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  ਜੇ(ਧਾਰਾ->reset) {
    ਜੇ(ਧਾਰਾ->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(ਅੰਕੜੇ, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", ਧਾਰਾ->id);
      connclose(cf->conn);
      ਅੰਕੜੇ->state.refused_ਧਾਰਾ = TRUE;
      *err = CURLE_RECV_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->resp_hds_complete && ਅੰਕੜੇ->req.no_body) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", ਧਾਰਾ->id, ਧਾਰਾ->error3,
                  Curl_vquic_h3_err_str(ਧਾਰਾ->error3));
      nread = 0;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    failf(ਅੰਕੜੇ, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", ਧਾਰਾ->id, ਧਾਰਾ->error3,
          Curl_vquic_h3_err_str(ਧਾਰਾ->error3));
    *err = ਅੰਕੜੇ->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਧਾਰਾ->resp_hds_complete) {
    failf(ਅੰਕੜੇ,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          ਧਾਰਾ->id);
    *err = CURLE_HTTP3;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  nread = 0;

ਬਾਹਰਲਾ:
  ਵਾਪਸ nread;
}

ਸਥਿਰ CURLcode cf_h3_proxy_sendbuf_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ,
                                        ਅਚਲ uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode ਨਤੀਜਾ;
  *pnwritten = 0;
  (ਖਾਲੀ)ਅੰਕੜੇ;

  ਨਤੀਜਾ = Curl_bufq_write(&ਧਾਰਾ->sendbuf, buf, len, pnwritten);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h3_proxy_send(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = ਨਿਰਾ;
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

  ਜੇ(pctx->tunnel.closed) {
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ denied;
  }

  ਧਾਰਾ = pctx->tunnel.ਧਾਰਾ;
  ਜੇ(!ਧਾਰਾ) {
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ denied;
  }

  ਜੇ(ਧਾਰਾ->closed) {
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
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = cf_h3_proxy_sendbuf_ਜੋੜੋ(ਅੰਕੜੇ, ਧਾਰਾ, buf, len, pnwritten);
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

ਬਾਹਰਲਾ:
  ਨਤੀਜਾ = Curl_1st_fatal(ਨਤੀਜਾ,
                          Curl_cf_ngtcp2_cmn_ਸੈੱਟ_expiry(cf, ਅੰਕੜੇ, &pktx));
denied:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              ਧਾਰਾ ? ਧਾਰਾ->id : -1, len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten);
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* incoming data frames on the h3 stream */
ਸਥਿਰ CURLcode cf_h3_proxy_recv(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅੱਖਰ *buf, size_t len, size_t *pnread)
{
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = pctx->tunnel.ਧਾਰਾ;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  ਬਣਤਰ cf_ngtcp2_io_ctx pktx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

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

  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&pctx->tunnel.recvbuf)) {
    ਨਤੀਜਾ = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    cf_h3_proxy_upd_rx_win(cf, ਅੰਕੜੇ, pctx->tunnel.ਧਾਰਾ);
  }

  ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ, &pktx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* inbufq had nothing before, maybe after progressing ingress? */
  ਜੇ(!*pnread && !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&pctx->tunnel.recvbuf)) {
    ਨਤੀਜਾ = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    ਜੇ(ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  ਧਾਰਾ->id, len, *pnread, (ਪੂਰਨ)ਨਤੀਜਾ);
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    cf_h3_proxy_upd_rx_win(cf, ਅੰਕੜੇ, pctx->tunnel.ਧਾਰਾ);
  }

  ਜੇ(*pnread) {
    Curl_multi_mark_dirty(ਅੰਕੜੇ);
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] xfer write failed", ਧਾਰਾ->id);
      Curl_cf_ngtcp2_h3_ਧਾਰਾ_close(cf, ਅੰਕੜੇ, ਧਾਰਾ);
      ਨਤੀਜਾ = ਧਾਰਾ->xfer_ਨਤੀਜਾ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_ਧਾਰਾ(cf, ਅੰਕੜੇ, ਧਾਰਾ, &ਨਤੀਜਾ);
      ਜੇ(nread > 0)
        *pnread = (size_t)nread;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਤੀਜਾ = CURLE_AGAIN;
  }

ਬਾਹਰਲਾ:
  ਨਤੀਜਾ = Curl_1st_fatal(ਨਤੀਜਾ,
                          Curl_cf_ngtcp2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ, &pktx));
  ਨਤੀਜਾ = Curl_1st_fatal(ਨਤੀਜਾ,
                          Curl_cf_ngtcp2_cmn_ਸੈੱਟ_expiry(cf, ਅੰਕੜੇ, &pktx));
denied:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] cf_recv(len=%zu) -> %d, %zu",
              ਧਾਰਾ ? ਧਾਰਾ->id : -1, len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread);
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h3_proxy_submit(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ h3_tunnel_ਧਾਰਾ *ts,
                                   ਬਣਤਰ httpreq *req)
{
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = ਨਿਰਾ;
  ਬਣਤਰ dynhds h2_ਸਿਰਲੇਖਾਂ;
  nghttp3_nv *nva = ਨਿਰਾ;
  size_t nheader;
  ਪੂਰਨ rc = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
  nghttp3_ਅੰਕੜੇ_reader reader;
  nghttp3_ਅੰਕੜੇ_reader *preader = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  Curl_dynhds_init(&h2_ਸਿਰਲੇਖਾਂ, 0, DYN_HTTP_REQUEST);
  ਨਤੀਜਾ = Curl_http_req_ਮੰਜ਼ਿਲ_h2(&h2_ਸਿਰਲੇਖਾਂ, req, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = Curl_cf_ngtcp2_h3_ਧਾਰਾ_ਤਿਆਰੀ(cf, ਅੰਕੜੇ, &ਧਾਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

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

  /* Open a bidirectional stream */
  {
    int64_t sid;
    ਪੂਰਨ rv;

    DEBUGASSERT(ਧਾਰਾ->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_ਧਾਰਾ(ctx->qconn, &sid, ਨਿਰਾ);
    ਜੇ(rv) {
      failf(ਅੰਕੜੇ, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਧਾਰਾ->id = sid;
    ts->ਧਾਰਾ = ਧਾਰਾ;
    ++ctx->ਵਰਤਿਆ_bidi_streams;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRId64 "] opened bidi stream", sid);
  }

  /* CONNECT-UDP request stream remains open for capsules, no fixed EOF. */
  ਧਾਰਾ->send_closed = 0;
  reader.read_ਅੰਕੜੇ = cb_h3_tunnel_read_ਅੰਕੜੇ;
  preader = &reader;

  rc = nghttp3_conn_submit_request(ctx->h3conn, ਧਾਰਾ->id,
                                   nva, nheader, preader, ਧਾਰਾ);

  ਜੇ(rc) {
    ਚੋਣ(rc) {
    ਹਾਲਤ NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", ਧਾਰਾ->id);
      ਰੋਕੋ;
    ਮੂਲ:
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  ਧਾਰਾ->id, rc, nghttp3_strerror(rc));
      ਰੋਕੋ;
    }
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(Curl_trc_is_verbose(ਅੰਕੜੇ)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", ਧਾਰਾ->id,
                Curl_bufref_ptr(&ਅੰਕੜੇ->state.url));
  }

ਬਾਹਰਲਾ:
  curlx_free(nva);
  Curl_dynhds_free(&h2_ਸਿਰਲੇਖਾਂ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h3_proxy_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ);

  ਜੇ(!ctx->qconn || !pctx->tunnel.ਧਾਰਾ || (sock == CURL_SOCKET_BAD))
    ਵਾਪਸ CURLE_OK;

  Curl_pollset_check(ਅੰਕੜੇ, ps, sock, &want_recv, &want_send);

  ਜੇ(want_recv || want_send || !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->q.sendbuf)) {
    ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = pctx->tunnel.ਧਾਰਾ;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_ਖੱਬਾ(ctx->qconn) ||
                 !ngtcp2_conn_get_max_ਅੰਕੜੇ_ਖੱਬਾ(ctx->qconn));
    s_exhaust = want_send && ਧਾਰਾ && ਧਾਰਾ->id >= 0 &&
                ਧਾਰਾ->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->q.sendbuf);

    ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ(ਅੰਕੜੇ, ps, sock, want_recv, want_send);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool cf_h3_proxy_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                                     ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&pctx->tunnel.recvbuf))
    ਵਾਪਸ TRUE;
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->has_ਅੰਕੜੇ_pending(cf->ਅਗਲਾ, ਅੰਕੜੇ) : FALSE;
}

ਸਥਿਰ CURLcode cf_h3_proxy_submit_CONNECT(ਬਣਤਰ Curl_cfilter *cf,
                                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ h3_tunnel_ਧਾਰਾ *ts)
{
  ਬਣਤਰ cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ httpreq *req = ਨਿਰਾ;

  ਨਤੀਜਾ = Curl_http_proxy_create_tunnel_request(
    &req, cf, ਅੰਕੜੇ, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_null(ਅੰਕੜੇ);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = cf_h3_proxy_submit(cf, ਅੰਕੜੇ, ts, req);

  ਜੇ(req)
    Curl_http_req_free(req);
  ਜੇ(ਨਤੀਜਾ)
    failf(ਅੰਕੜੇ, "Failed sending CONNECT to proxy");
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h3_proxy_inspect_response(ਬਣਤਰ Curl_cfilter *cf,
                                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ h3_tunnel_ਧਾਰਾ *ts)
{
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_ਨਤੀਜਾ res;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_http_proxy_inspect_tunnel_response(
      cf, ਅੰਕੜੇ, ts->resp, (bool)pctx->tunnel.udp, &res);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਚੋਣ(res) {
  ਹਾਲਤ PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, ਅੰਕੜੇ);
    ਰੋਕੋ;
  ਹਾਲਤ PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, ਅੰਕੜੇ);
    ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
    ਰੋਕੋ;
  ਹਾਲਤ PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, ਅੰਕੜੇ);
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h3_proxy_tunnel(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ h3_tunnel_ਧਾਰਾ *ts,
                                   bool *pdone)
{
  ਬਣਤਰ cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  ਕਰੋ {
    ਚੋਣ(ts->state) {
    ਹਾਲਤ H3_TUNNEL_INIT:
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] CONNECT start for %s", ts->authority);
      ਨਤੀਜਾ = cf_h3_proxy_submit_CONNECT(cf, ਅੰਕੜੇ, ts);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, ਅੰਕੜੇ);

      ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ, ਨਿਰਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      FALLTHROUGH();

    ਹਾਲਤ H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ, ਨਿਰਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      ਨਤੀਜਾ = Curl_cf_ngtcp2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ, ਨਿਰਾ);
      ਜੇ(ਨਤੀਜਾ && ਨਤੀਜਾ != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, ਅੰਕੜੇ);
        ਜਾਓ ਬਾਹਰਲਾ;
      }

      ਜੇ(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, ਅੰਕੜੇ);
      }
      ਨਹੀਂਤਾਂ {
        /* Not done yet, return and let multi interface call us again */
        ਨਤੀਜਾ = CURLE_OK;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      FALLTHROUGH();

    ਹਾਲਤ H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      ਨਤੀਜਾ = cf_h3_proxy_inspect_response(cf, ਅੰਕੜੇ, ts);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      ctx->connected = TRUE;
      ਰੋਕੋ;

    ਹਾਲਤ H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      ਵਾਪਸ CURLE_OK;

    ਹਾਲਤ H3_TUNNEL_FAILED:
      ਵਾਪਸ CURLE_RECV_ERROR;

    ਮੂਲ:
      ਰੋਕੋ;
    }

  } ਜਦੋਂ(ts->state == H3_TUNNEL_INIT);

ਬਾਹਰਲਾ:
  ਜੇ((ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, ਅੰਕੜੇ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h3_proxy_connect(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_h3_proxy_ctx *pctx = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save = { 0 };
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ h3_tunnel_ਧਾਰਾ *ts = &pctx->tunnel;
  bool ਅੰਕੜੇ_saved = FALSE;

  ਨਤੀਜਾ = Curl_cf_ngtcp2_cmn_connect(cf, ਅੰਕੜੇ, ਪੂਰਾ);
  ਜੇ(ਨਤੀਜਾ || !*ਪੂਰਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  ਅੰਕੜੇ_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  ਨਤੀਜਾ = cf_h3_proxy_tunnel(cf, ਅੰਕੜੇ, ts, ਪੂਰਾ);

ਬਾਹਰਲਾ:
  ਜੇ(*ਪੂਰਾ) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&ਅੰਕੜੇ->req, ਅੰਕੜੇ);
    Curl_client_reset(ਅੰਕੜੇ);
  }

  ਜੇ(ਅੰਕੜੇ_saved)
    CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_h3_proxy_destroy(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h3_proxy_ctx *ctx = cf->ctx;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(ctx) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_h3_proxy_destroy()");
    ਜੇ(ctx->ngtcp2_ctx.qconn) {
      ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
      CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
      Curl_cf_ngtcp2_cmn_conn_close(cf, ਅੰਕੜੇ);
      CF_ਅੰਕੜੇ_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = ਨਿਰਾ;
  }
}

ਸਥਿਰ CURLcode cf_h3_proxy_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਵਾਪਸ Curl_cf_ngtcp2_cmn_shutdown(cf, ਅੰਕੜੇ, ਪੂਰਾ);
}

ਬਣਤਰ Curl_cftype Curl_cft_h3_proxy = {
  "H3-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  cf_h3_proxy_destroy,
  cf_h3_proxy_connect,
  cf_h3_proxy_shutdown,
  cf_h3_proxy_adjust_pollset,
  cf_h3_proxy_ਅੰਕੜੇ_pending,
  cf_h3_proxy_send,
  cf_h3_proxy_recv,
  Curl_cf_def_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_ngtcp2_cmn_query,
};

CURLcode Curl_cf_ngtcp2_proxy_create(ਬਣਤਰ Curl_cfilter **pcf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ Curl_peer *origin,
                                     ਬਣਤਰ Curl_peer *peer,
                                     uint8_t transport_peer,
                                     ਬਣਤਰ connectdata *conn,
                                     ਬਣਤਰ Curl_sockaddr_ex *addr,
                                     ਬਣਤਰ Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  ਬਣਤਰ cf_h3_proxy_ctx *ctx;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(!tunnel_peer)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਜੇ((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    ਵਾਪਸ CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਤੀਜਾ = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
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
    ਨਹੀਂਤਾਂ ਜੇ(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  ਨਹੀਂਤਾਂ
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_ngtcp2_proxy_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ Curl_peer *origin,
                                           ਬਣਤਰ Curl_peer *peer,
                                           ਬਣਤਰ Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  ਬਣਤਰ cf_h3_proxy_ctx *ctx;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  (ਖਾਲੀ)ਅੰਕੜੇ;

  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਨਤੀਜਾ = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_ਪਾਓ_after(cf_at, cf);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(cf)
      Curl_conn_cf_discard_chain(&cf, ਅੰਕੜੇ);
    ਨਹੀਂਤਾਂ ਜੇ(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ
