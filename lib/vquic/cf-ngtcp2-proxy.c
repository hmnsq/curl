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

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "http_proxy.h"
#అన్తర్భావ "vquic/vquic.h"
#అన్తర్భావ "vquic/cf-ngtcp2-cmn.h"
#అన్తర్భావ "vquic/cf-ngtcp2-proxy.h"
#అన్తర్భావ "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#నిర్వచించు PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#నిర్వచించు PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_ప్రవాహం_CHUNK_కొలత)

రకనిర్వచనం గణన {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

నిర్మాణం h3_tunnel_ప్రవాహం {
  నిర్మాణం Curl_peer *dest;  /* where the tunnel goes to */
  నిర్మాణం http_resp *resp;
  నిర్మాణం bufq recvbuf;
  అక్షరం *authority;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  h3_tunnel_state state;
  బిట్టు(udp);
  బిట్టు(has_final_response);
  బిట్టు(closed);
};

స్థిర CURLcode h3_tunnel_ప్రవాహం_init(నిర్మాణం h3_tunnel_ప్రవాహం *ts,
                                      నిర్మాణం Curl_peer *dest,
                                      bool udp)
{
  ts->state = H3_TUNNEL_INIT;
  Curl_peer_link(&ts->dest, dest);
  Curl_bufq_init2(&ts->recvbuf, H3_ప్రవాహం_CHUNK_కొలత,
                  PROXY_H3_TUNNEL_RECV_CHUNKS, BUFQ_OPT_SOFT_హద్దు);
  ts->udp = udp;
  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->పోర్టు);
  ఒకవేళ(!ts->authority)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  తిరిగి CURLE_OK;
}

స్థిర శూన్య h3_tunnel_ప్రవాహం_reset(నిర్మాణం h3_tunnel_ప్రవాహం *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = ఖాళీ;
  ts->ప్రవాహం = ఖాళీ;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

స్థిర శూన్య h3_tunnel_ప్రవాహం_cleanup(నిర్మాణం h3_tunnel_ప్రవాహం *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

స్థిర శూన్య h3_tunnel_go_state(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం h3_tunnel_ప్రవాహం *ts,
                               h3_tunnel_state new_state,
                               నిర్మాణం Curl_easy *దత్తాంశం)
{
  VERBOSE(int64_t ప్రవాహం_id = ts->ప్రవాహం ? ts->ప్రవాహం->id : -1);
  (శూన్య)cf;

  ఒకవేళ(ts->state == new_state)
    తిరిగి;

  /* leaving this one */
  ఎంపిక(ts->state) {
  స్థితి H3_TUNNEL_CONNECT:
    దత్తాంశం->req.ignorebody = FALSE;
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }

  /* entering this one */
  ఎంపిక(new_state) {
  స్థితి H3_TUNNEL_INIT:
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] -> [init]", ప్రవాహం_id);
    h3_tunnel_ప్రవాహం_reset(ts);
    ఆపు;
  స్థితి H3_TUNNEL_CONNECT:
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] -> [connect]", ప్రవాహం_id);
    ts->state = H3_TUNNEL_CONNECT;
    ఆపు;
  స్థితి H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] -> [response]", ప్రవాహం_id);
    ts->state = H3_TUNNEL_RESPONSE;
    ఆపు;
  స్థితి H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] -> [established]", ప్రవాహం_id);
    infof(దత్తాంశం, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    దత్తాంశం->state.authproxy.పూర్తి = TRUE;
    దత్తాంశం->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(దత్తాంశం->req.hd_proxy_auth);
    ఆపు;
  స్థితి H3_TUNNEL_FAILED:
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] -> [failed]", ప్రవాహం_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(దత్తాంశం->req.hd_proxy_auth);
    ఆపు;
  }
}

నిర్మాణం cf_h3_proxy_ctx {
  నిర్మాణం cf_ngtcp2_ctx ngtcp2_ctx;
  నిర్మాణం h3_tunnel_ప్రవాహం tunnel; /* our tunnel CONNECT stream */
  బిట్టు(connected);
};

స్థిర CURLcode cf_ngtcp2_proxy_h3_init(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం cf_ngtcp2_ctx *ctx);

స్థిర CURLcode cf_h3_proxy_ctx_init(నిర్మాణం cf_h3_proxy_ctx *ctx,
                                     నిర్మాణం Curl_peer *origin,
                                     నిర్మాణం Curl_peer *peer,
                                     నిర్మాణం ssl_filter_config *sslc,
                                     నిర్మాణం Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode ఫలితం;
  ఫలితం = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  ఒకవేళ(!ఫలితం)
    ఫలితం = h3_tunnel_ప్రవాహం_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_h3_proxy_ctx_free(నిర్మాణం cf_h3_proxy_ctx *ctx)
{
  ఒకవేళ(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_ప్రవాహం_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

స్థిర పూర్ణాంకం cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                                      uint64_t datalen, శూన్య *user_దత్తాంశం,
                                      శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  size_t skiplen;
  (శూన్య)ప్రవాహం_user_దత్తాంశం;

  ప్రవాహం = pctx->tunnel.ప్రవాహం;
  ఒకవేళ(!ప్రవాహం || (ప్రవాహం->id != ప్రవాహం_id))
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

స్థిర పూర్ణాంకం cb_h3_proxy_ప్రవాహం_close(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                                    uint64_t app_error_సంకేతం, శూన్య *user_దత్తాంశం,
                                    శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;

  (శూన్య)conn;
  (శూన్య)ప్రవాహం_user_దత్తాంశం;
  ఒకవేళ(!దత్తాంశం)
    తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;

  ప్రవాహం = pctx->tunnel.ప్రవాహం;
  ఒకవేళ(!ప్రవాహం || (ప్రవాహం->id != ప్రవాహం_id))
    తిరిగి 0;

  ప్రవాహం->closed = TRUE;
  ప్రవాహం->error3 = app_error_సంకేతం;
  ఒకవేళ(ప్రవాహం->error3 != NGHTTP3_H3_NO_ERROR) {
    ప్రవాహం->reset = TRUE;
    ప్రవాహం->send_closed = TRUE;
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                ప్రవాహం->id, ప్రవాహం->error3);
  }
  లేకపోతే
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] CLOSED", ప్రవాహం->id);
  pctx->tunnel.ప్రవాహం = ఖాళీ;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(దత్తాంశం);
  తిరిగి 0;
}

స్థిర శూన్య cf_h3_proxy_upd_rx_win(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం)
{
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  ఒకవేళ(!ప్రవాహం) /* already closed */
    తిరిగి;
  ఒకవేళ(ప్రవాహం->rx_స్థానభ్రంశం_max < ప్రవాహం->rx_స్థానభ్రంశం) {
    DEBUGASSERT(0);
    తిరిగి;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = ప్రవాహం->rx_స్థానభ్రంశం_max - ప్రవాహం->rx_స్థానభ్రంశం;
  ఒకవేళ(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    ఒకవేళ(UINT64_MAX - delta < ప్రవాహం->rx_స్థానభ్రంశం_max)
      delta = UINT64_MAX - ప్రవాహం->rx_స్థానభ్రంశం_max;
    ఒకవేళ(delta) {
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  ప్రవాహం->id, cur_win, rx_buffered, delta);
      ప్రవాహం->rx_స్థానభ్రంశం_max += delta;
      ngtcp2_conn_extend_max_ప్రవాహం_స్థానభ్రంశం(ctx->qconn, ప్రవాహం->id, delta);
      ngtcp2_conn_extend_max_స్థానభ్రంశం(ctx->qconn, delta);
    }
  }
}

స్థిర పూర్ణాంకం cb_h3_proxy_recv_దత్తాంశం(nghttp3_conn *conn, int64_t stream3_id,
                                 స్థిరరాశి uint8_t *buf, size_t buflen,
                                 శూన్య *user_దత్తాంశం, శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  size_t nwritten;
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)conn;
  (శూన్య)stream3_id;
  (శూన్య)ప్రవాహం_user_దత్తాంశం;

  ప్రవాహం = pctx->tunnel.ప్రవాహం;
  ఒకవేళ(!దత్తాంశం || !ప్రవాహం || (ప్రవాహం->id != stream3_id)) {
    తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  ప్రవాహం->rx_మొత్తం += (curl_off_t)buflen;
  CURL_TRC_CF(దత్తాంశం, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              ప్రవాహం->id, buflen, ప్రవాహం->rx_మొత్తం);

  ఫలితం = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  ఒకవేళ(ఫలితం || (nwritten < buflen)) {
    తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  ప్రవాహం->rx_స్థానభ్రంశం += buflen;
  ఒకవేళ(ప్రవాహం->rx_స్థానభ్రంశం_max < ప్రవాహం->rx_స్థానభ్రంశం)
    ప్రవాహం->rx_స్థానభ్రంశం_max = ప్రవాహం->rx_స్థానభ్రంశం;

  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              ప్రవాహం->id, buflen, ప్రవాహం->rx_స్థానభ్రంశం_max - ప్రవాహం->rx_స్థానభ్రంశం);
  cf_h3_proxy_upd_rx_win(cf, దత్తాంశం, ప్రవాహం);

  Curl_multi_mark_dirty(దత్తాంశం);
  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                                        size_t consumed, శూన్య *user_దత్తాంశం,
                                        శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (శూన్య)conn;
  (శూన్య)ప్రవాహం_user_దత్తాంశం;

  ఒకవేళ(!ctx)
    తిరిగి 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_ప్రవాహం_స్థానభ్రంశం(ctx->qconn, ప్రవాహం_id, consumed);
  ngtcp2_conn_extend_max_స్థానభ్రంశం(ctx->qconn, consumed);

  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_h3_proxy_recv_శీర్షిక(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *విలువ, uint8_t flags,
                                   శూన్య *user_దత్తాంశం, శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(విలువ);
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం http_స్థితి;
  నిర్మాణం http_resp *resp;
  (శూన్య)conn;
  (శూన్య)ప్రవాహం_id;
  (శూన్య)token;
  (శూన్య)flags;
  (శూన్య)ప్రవాహం_user_దత్తాంశం;

  /* stream_user_data might be NULL for control streams */
  ఒకవేళ(!దత్తాంశం)
    తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;

  ప్రవాహం = pctx->tunnel.ప్రవాహం;
  ఒకవేళ(!ప్రవాహం || (ప్రవాహం->id != ప్రవాహం_id)) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                ప్రవాహం_id, (శూన్య *)దత్తాంశం, దత్తాంశం ? దత్తాంశం->mid : 0);
    తిరిగి 0;
  }

  ఒకవేళ(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    తిరిగి 0;
  }

  ఒకవేళ(token == NGHTTP3_QPACK_TOKEN_స్థితి) {
    ఫలితం = Curl_http_decode_స్థితి(&ప్రవాహం->స్థితి_సంకేతం,
                                     (స్థిరరాశి అక్షరం *)h3val.పునాది, h3val.len);
    ఒకవేళ(ఫలితం)
      తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
    http_స్థితి = ప్రవాహం->స్థితి_సంకేతం;
    ఫలితం = Curl_http_resp_make(&resp, http_స్థితి, ఖాళీ);
    ఒకవేళ(ఫలితం)
      తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
    ఒకవేళ(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  లేకపోతే {
    ఒకవేళ(!pctx->tunnel.resp) {
      తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] header: %.*s: %.*s", ప్రవాహం_id,
                (పూర్ణాంకం)h3name.len, h3name.పునాది, (పూర్ణాంకం)h3val.len, h3val.పునాది);
    ఫలితం = Curl_dynhds_కూడు(&pctx->tunnel.resp->శీర్షికలు,
                             (స్థిరరాశి అక్షరం *)h3name.పునాది, h3name.len,
                             (స్థిరరాశి అక్షరం *)h3val.పునాది, h3val.len);
    ఒకవేళ(ఫలితం) {
      తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_h3_proxy_చివర_శీర్షికలు(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                                   పూర్ణాంకం fin, శూన్య *user_దత్తాంశం,
                                   శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  (శూన్య)conn;
  (శూన్య)ప్రవాహం_id;
  (శూన్య)fin;
  (శూన్య)ప్రవాహం_user_దత్తాంశం;

  ఒకవేళ(!దత్తాంశం)
    తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;

  ప్రవాహం = pctx->tunnel.ప్రవాహం;
  ఒకవేళ(!ప్రవాహం || (ప్రవాహం->id != ప్రవాహం_id)) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                ప్రవాహం_id, (శూన్య *)దత్తాంశం, దత్తాంశం ? దత్తాంశం->mid : 0);
    తిరిగి 0;
  }

  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] end_headers, status=%d", ప్రవాహం_id,
              ప్రవాహం->స్థితి_సంకేతం);

  ఒకవేళ(!pctx->tunnel.has_final_response) {
    ఒకవేళ(ప్రవాహం->స్థితి_సంకేతం / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  ఒకవేళ(ప్రవాహం->స్థితి_సంకేతం / 100 != 1) {
    ప్రవాహం->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(దత్తాంశం);
  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                                    uint64_t app_error_సంకేతం, శూన్య *user_దత్తాంశం,
                                    శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (శూన్య)conn;
  (శూన్య)ప్రవాహం_user_దత్తాంశం;

  ఒకవేళ(ctx) {
    పూర్ణాంకం rv = ngtcp2_conn_shutdown_ప్రవాహం_read(ctx->qconn, 0, ప్రవాహం_id,
                                              app_error_సంకేతం);

    ఒకవేళ(rv && rv != NGTCP2_ERR_ప్రవాహం_NOT_దొరికింది) {
      తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  తిరిగి 0;
}

స్థిర పూర్ణాంకం cb_h3_proxy_reset_ప్రవాహం(nghttp3_conn *conn, int64_t ప్రవాహం_id,
                                    uint64_t app_error_సంకేతం, శూన్య *user_దత్తాంశం,
                                    శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  పూర్ణాంకం rv;

  (శూన్య)conn;
  (శూన్య)ప్రవాహం_user_దత్తాంశం;
  ఒకవేళ(!దత్తాంశం)
    తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;

  ఒకవేళ(!pctx->tunnel.ప్రవాహం ||
     (ప్రవాహం_id != pctx->tunnel.ప్రవాహం->id))
    తిరిగి 0;

  rv = ngtcp2_conn_shutdown_ప్రవాహం_write(ctx->qconn, 0, ప్రవాహం_id,
                                         app_error_సంకేతం);
  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] reset -> %d", ప్రవాహం_id, rv);
  pctx->tunnel.ప్రవాహం = ఖాళీ;
  pctx->tunnel.closed = TRUE;
  ఒకవేళ(rv && rv != NGTCP2_ERR_ప్రవాహం_NOT_దొరికింది) {
    తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  తిరిగి 0;
}

స్థిర nghttp3_ssize cb_h3_tunnel_read_దత్తాంశం(nghttp3_conn *conn,
                                            int64_t ప్రవాహం_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            శూన్య *user_దత్తాంశం,
                                            శూన్య *ప్రవాహం_user_దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf = user_దత్తాంశం;
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
  size_t nwritten = 0;
  size_t nvecs = 0;
  స్థిరరాశి గుర్తులేని అక్షరం *buf_పునాది;

  (శూన్య)conn;
  (శూన్య)ప్రవాహం_id;
  (శూన్య)veccnt;
  (శూన్య)ప్రవాహం_user_దత్తాంశం;
  (శూన్య)pflags;

  ప్రవాహం = pctx->tunnel.ప్రవాహం;
  ఒకవేళ(!దత్తాంశం || !ప్రవాహం || (ప్రవాహం->id != ప్రవాహం_id))
    తిరిగి NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  ఒకవేళ(ప్రవాహం->tx_లోపల_flight_కొలత < Curl_bufq_len(&ప్రవాహం->sendbuf)) {
    nvecs = 0;
    అయితే(nvecs < veccnt) {
      ఒకవేళ(!Curl_bufq_peek_at(&ప్రవాహం->sendbuf,
                           ప్రవాహం->tx_లోపల_flight_కొలత,
                           &buf_పునాది,
                           &vec[nvecs].len))
        ఆపు;
      vec[nvecs].పునాది = (uint8_t *)(uintptr_t)buf_పునాది;
      ప్రవాహం->tx_లోపల_flight_కొలత += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  ఒకవేళ(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] read req body -> AGAIN",
                ప్రవాహం->id);
    తిరిగి NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              ప్రవాహం->id, nvecs,
              *pflags == NGHTTP3_దత్తాంశం_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&ప్రవాహం->sendbuf));
  తిరిగి (nghttp3_ssize)nvecs;
}

#నిర్వచితమైతే CURL_HAVE_DIAG
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wmissing-field-initializers"
#అయితే_ముగింపు
స్థిర nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_ప్రవాహం_close,
  cb_h3_proxy_recv_దత్తాంశం,
  cb_h3_proxy_deferred_consume,
  ఖాళీ, /* begin_headers */
  cb_h3_proxy_recv_శీర్షిక,
  cb_h3_proxy_చివర_శీర్షికలు,
  ఖాళీ, /* begin_trailers */
  cb_h3_proxy_recv_శీర్షిక,
  ఖాళీ, /* end_trailers */
  cb_h3_proxy_stop_sending,
  ఖాళీ, /* end_stream */
  cb_h3_proxy_reset_ప్రవాహం,
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

స్థిర CURLcode cf_ngtcp2_proxy_h3_init(నిర్మాణం Curl_cfilter *cf,
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
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "error creating nghttp3 connection instance");
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  తిరిగి Curl_cf_ngtcp2_h3_init_ctrls(ctx, దత్తాంశం);
}

స్థిర ssize_t cf_h3_proxy_recv_closed_ప్రవాహం(నిర్మాణం Curl_cfilter *cf,
                                              నిర్మాణం Curl_easy *దత్తాంశం,
                                              నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  ఒకవేళ(ప్రవాహం->reset) {
    ఒకవేళ(ప్రవాహం->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(దత్తాంశం, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", ప్రవాహం->id);
      connclose(cf->conn);
      దత్తాంశం->state.refused_ప్రవాహం = TRUE;
      *err = CURLE_RECV_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(ప్రవాహం->resp_hds_complete && దత్తాంశం->req.no_body) {
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", ప్రవాహం->id, ప్రవాహం->error3,
                  Curl_vquic_h3_err_str(ప్రవాహం->error3));
      nread = 0;
      వెళ్ళు బహిర్గతం;
    }
    failf(దత్తాంశం, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", ప్రవాహం->id, ప్రవాహం->error3,
          Curl_vquic_h3_err_str(ప్రవాహం->error3));
    *err = దత్తాంశం->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే ఒకవేళ(!ప్రవాహం->resp_hds_complete) {
    failf(దత్తాంశం,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          ప్రవాహం->id);
    *err = CURLE_HTTP3;
    వెళ్ళు బహిర్గతం;
  }
  nread = 0;

బహిర్గతం:
  తిరిగి nread;
}

స్థిర CURLcode cf_h3_proxy_sendbuf_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం,
                                        స్థిరరాశి uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode ఫలితం;
  *pnwritten = 0;
  (శూన్య)దత్తాంశం;

  ఫలితం = Curl_bufq_write(&ప్రవాహం->sendbuf, buf, len, pnwritten);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h3_proxy_send(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = ఖాళీ;
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

  ఒకవేళ(pctx->tunnel.closed) {
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు denied;
  }

  ప్రవాహం = pctx->tunnel.ప్రవాహం;
  ఒకవేళ(!ప్రవాహం) {
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు denied;
  }

  ఒకవేళ(ప్రవాహం->closed) {
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
  లేకపోతే {
    ఫలితం = cf_h3_proxy_sendbuf_కూడు(దత్తాంశం, ప్రవాహం, buf, len, pnwritten);
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

బహిర్గతం:
  ఫలితం = Curl_1st_fatal(ఫలితం,
                          Curl_cf_ngtcp2_cmn_సమితి_expiry(cf, దత్తాంశం, &pktx));
denied:
  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              ప్రవాహం ? ప్రవాహం->id : -1, len, (పూర్ణాంకం)ఫలితం, *pnwritten);
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

/* incoming data frames on the h3 stream */
స్థిర CURLcode cf_h3_proxy_recv(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 అక్షరం *buf, size_t len, size_t *pnread)
{
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = pctx->tunnel.ప్రవాహం;
  నిర్మాణం cf_call_దత్తాంశం save;
  నిర్మాణం cf_ngtcp2_io_ctx pktx;
  CURLcode ఫలితం = CURLE_OK;

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

  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&pctx->tunnel.recvbuf)) {
    ఫలితం = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    cf_h3_proxy_upd_rx_win(cf, దత్తాంశం, pctx->tunnel.ప్రవాహం);
  }

  ఫలితం = Curl_cf_ngtcp2_పురోగతి_ingress(cf, దత్తాంశం, &pktx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* inbufq had nothing before, maybe after progressing ingress? */
  ఒకవేళ(!*pnread && !Curl_bufq_is_ఖాళీలు(&pctx->tunnel.recvbuf)) {
    ఫలితం = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    ఒకవేళ(ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  ప్రవాహం->id, len, *pnread, (పూర్ణాంకం)ఫలితం);
      వెళ్ళు బహిర్గతం;
    }
    cf_h3_proxy_upd_rx_win(cf, దత్తాంశం, pctx->tunnel.ప్రవాహం);
  }

  ఒకవేళ(*pnread) {
    Curl_multi_mark_dirty(దత్తాంశం);
  }
  లేకపోతే {
    ఒకవేళ(ప్రవాహం->xfer_ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] xfer write failed", ప్రవాహం->id);
      Curl_cf_ngtcp2_h3_ప్రవాహం_close(cf, దత్తాంశం, ప్రవాహం);
      ఫలితం = ప్రవాహం->xfer_ఫలితం;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(ప్రవాహం->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_ప్రవాహం(cf, దత్తాంశం, ప్రవాహం, &ఫలితం);
      ఒకవేళ(nread > 0)
        *pnread = (size_t)nread;
      వెళ్ళు బహిర్గతం;
    }
    ఫలితం = CURLE_AGAIN;
  }

బహిర్గతం:
  ఫలితం = Curl_1st_fatal(ఫలితం,
                          Curl_cf_ngtcp2_పురోగతి_egress(cf, దత్తాంశం, &pktx));
  ఫలితం = Curl_1st_fatal(ఫలితం,
                          Curl_cf_ngtcp2_cmn_సమితి_expiry(cf, దత్తాంశం, &pktx));
denied:
  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] cf_recv(len=%zu) -> %d, %zu",
              ప్రవాహం ? ప్రవాహం->id : -1, len, (పూర్ణాంకం)ఫలితం, *pnread);
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h3_proxy_submit(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం h3_tunnel_ప్రవాహం *ts,
                                   నిర్మాణం httpreq *req)
{
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = ఖాళీ;
  నిర్మాణం dynhds h2_శీర్షికలు;
  nghttp3_nv *nva = ఖాళీ;
  size_t nheader;
  పూర్ణాంకం rc = 0;
  గుర్తులేని పూర్ణాంకం i;
  nghttp3_దత్తాంశం_reader reader;
  nghttp3_దత్తాంశం_reader *preader = ఖాళీ;
  CURLcode ఫలితం;

  Curl_dynhds_init(&h2_శీర్షికలు, 0, DYN_HTTP_REQUEST);
  ఫలితం = Curl_http_req_గమ్యం_h2(&h2_శీర్షికలు, req, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = Curl_cf_ngtcp2_h3_ప్రవాహం_అమరిక(cf, దత్తాంశం, &ప్రవాహం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

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

  /* Open a bidirectional stream */
  {
    int64_t sid;
    పూర్ణాంకం rv;

    DEBUGASSERT(ప్రవాహం->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_ప్రవాహం(ctx->qconn, &sid, ఖాళీ);
    ఒకవేళ(rv) {
      failf(దత్తాంశం, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    ప్రవాహం->id = sid;
    ts->ప్రవాహం = ప్రవాహం;
    ++ctx->వాడిన_bidi_streams;
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRId64 "] opened bidi stream", sid);
  }

  /* CONNECT-UDP request stream remains open for capsules, no fixed EOF. */
  ప్రవాహం->send_closed = 0;
  reader.read_దత్తాంశం = cb_h3_tunnel_read_దత్తాంశం;
  preader = &reader;

  rc = nghttp3_conn_submit_request(ctx->h3conn, ప్రవాహం->id,
                                   nva, nheader, preader, ప్రవాహం);

  ఒకవేళ(rc) {
    ఎంపిక(rc) {
    స్థితి NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(దత్తాంశం, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", ప్రవాహం->id);
      ఆపు;
    అప్రమేయ:
      CURL_TRC_CF(దత్తాంశం, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  ప్రవాహం->id, rc, nghttp3_strerror(rc));
      ఆపు;
    }
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(Curl_trc_is_verbose(దత్తాంశం)) {
    CURL_TRC_CF(దత్తాంశం, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", ప్రవాహం->id,
                Curl_bufref_ptr(&దత్తాంశం->state.url));
  }

బహిర్గతం:
  curlx_free(nva);
  Curl_dynhds_free(&h2_శీర్షికలు);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h3_proxy_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                           నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode ఫలితం = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, దత్తాంశం);

  ఒకవేళ(!ctx->qconn || !pctx->tunnel.ప్రవాహం || (sock == CURL_SOCKET_BAD))
    తిరిగి CURLE_OK;

  Curl_pollset_check(దత్తాంశం, ps, sock, &want_recv, &want_send);

  ఒకవేళ(want_recv || want_send || !Curl_bufq_is_ఖాళీలు(&ctx->q.sendbuf)) {
    నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = pctx->tunnel.ప్రవాహం;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_ఎడమ(ctx->qconn) ||
                 !ngtcp2_conn_get_max_దత్తాంశం_ఎడమ(ctx->qconn));
    s_exhaust = want_send && ప్రవాహం && ప్రవాహం->id >= 0 &&
                ప్రవాహం->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_ఖాళీలు(&ctx->q.sendbuf);

    ఫలితం = Curl_pollset_సమితి(దత్తాంశం, ps, sock, want_recv, want_send);
  }
  తిరిగి ఫలితం;
}

స్థిర bool cf_h3_proxy_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                                     స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&pctx->tunnel.recvbuf))
    తిరిగి TRUE;
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->has_దత్తాంశం_pending(cf->తదుపరి, దత్తాంశం) : FALSE;
}

స్థిర CURLcode cf_h3_proxy_submit_CONNECT(నిర్మాణం Curl_cfilter *cf,
                                           నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం h3_tunnel_ప్రవాహం *ts)
{
  నిర్మాణం cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం;
  నిర్మాణం httpreq *req = ఖాళీ;

  ఫలితం = Curl_http_proxy_create_tunnel_request(
    &req, cf, దత్తాంశం, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_creader_సమితి_null(దత్తాంశం);
  ఒకవేళ(!ఫలితం)
    ఫలితం = cf_h3_proxy_submit(cf, దత్తాంశం, ts, req);

  ఒకవేళ(req)
    Curl_http_req_free(req);
  ఒకవేళ(ఫలితం)
    failf(దత్తాంశం, "Failed sending CONNECT to proxy");
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h3_proxy_inspect_response(నిర్మాణం Curl_cfilter *cf,
                                             నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం h3_tunnel_ప్రవాహం *ts)
{
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_ఫలితం res;
  CURLcode ఫలితం;

  ఫలితం = Curl_http_proxy_inspect_tunnel_response(
      cf, దత్తాంశం, ts->resp, (bool)pctx->tunnel.udp, &res);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  ఎంపిక(res) {
  స్థితి PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, దత్తాంశం);
    ఆపు;
  స్థితి PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, దత్తాంశం);
    ఫలితం = CURLE_COULDNT_CONNECT;
    ఆపు;
  స్థితి PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, దత్తాంశం);
    ఆపు;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h3_proxy_tunnel(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం h3_tunnel_ప్రవాహం *ts,
                                   bool *pdone)
{
  నిర్మాణం cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  చేయి {
    ఎంపిక(ts->state) {
    స్థితి H3_TUNNEL_INIT:
      CURL_TRC_CF(దత్తాంశం, cf, "[0] CONNECT start for %s", ts->authority);
      ఫలితం = cf_h3_proxy_submit_CONNECT(cf, దత్తాంశం, ts);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, దత్తాంశం);

      ఫలితం = Curl_cf_ngtcp2_పురోగతి_egress(cf, దత్తాంశం, ఖాళీ);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      FALLTHROUGH();

    స్థితి H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      ఫలితం = Curl_cf_ngtcp2_పురోగతి_ingress(cf, దత్తాంశం, ఖాళీ);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      ఫలితం = Curl_cf_ngtcp2_పురోగతి_egress(cf, దత్తాంశం, ఖాళీ);
      ఒకవేళ(ఫలితం && ఫలితం != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, దత్తాంశం);
        వెళ్ళు బహిర్గతం;
      }

      ఒకవేళ(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, దత్తాంశం);
      }
      లేకపోతే {
        /* Not done yet, return and let multi interface call us again */
        ఫలితం = CURLE_OK;
        వెళ్ళు బహిర్గతం;
      }
      FALLTHROUGH();

    స్థితి H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      ఫలితం = cf_h3_proxy_inspect_response(cf, దత్తాంశం, ts);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      ctx->connected = TRUE;
      ఆపు;

    స్థితి H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      తిరిగి CURLE_OK;

    స్థితి H3_TUNNEL_FAILED:
      తిరిగి CURLE_RECV_ERROR;

    అప్రమేయ:
      ఆపు;
    }

  } అయితే(ts->state == H3_TUNNEL_INIT);

బహిర్గతం:
  ఒకవేళ((ఫలితం && (ఫలితం != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, దత్తాంశం);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h3_proxy_connect(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం cf_h3_proxy_ctx *pctx = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save = { 0 };
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం h3_tunnel_ప్రవాహం *ts = &pctx->tunnel;
  bool దత్తాంశం_saved = FALSE;

  ఫలితం = Curl_cf_ngtcp2_cmn_connect(cf, దత్తాంశం, పూర్తి);
  ఒకవేళ(ఫలితం || !*పూర్తి)
    వెళ్ళు బహిర్గతం;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  దత్తాంశం_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  ఫలితం = cf_h3_proxy_tunnel(cf, దత్తాంశం, ts, పూర్తి);

బహిర్గతం:
  ఒకవేళ(*పూర్తి) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&దత్తాంశం->req, దత్తాంశం);
    Curl_client_reset(దత్తాంశం);
  }

  ఒకవేళ(దత్తాంశం_saved)
    CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_h3_proxy_destroy(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h3_proxy_ctx *ctx = cf->ctx;

  (శూన్య)దత్తాంశం;
  ఒకవేళ(ctx) {
    CURL_TRC_CF(దత్తాంశం, cf, "cf_h3_proxy_destroy()");
    ఒకవేళ(ctx->ngtcp2_ctx.qconn) {
      నిర్మాణం cf_call_దత్తాంశం save;
      CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
      Curl_cf_ngtcp2_cmn_conn_close(cf, దత్తాంశం);
      CF_దత్తాంశం_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = ఖాళీ;
  }
}

స్థిర CURLcode cf_h3_proxy_shutdown(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  తిరిగి Curl_cf_ngtcp2_cmn_shutdown(cf, దత్తాంశం, పూర్తి);
}

నిర్మాణం Curl_cftype Curl_cft_h3_proxy = {
  "H3-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  cf_h3_proxy_destroy,
  cf_h3_proxy_connect,
  cf_h3_proxy_shutdown,
  cf_h3_proxy_adjust_pollset,
  cf_h3_proxy_దత్తాంశం_pending,
  cf_h3_proxy_send,
  cf_h3_proxy_recv,
  Curl_cf_def_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_ngtcp2_cmn_query,
};

CURLcode Curl_cf_ngtcp2_proxy_create(నిర్మాణం Curl_cfilter **pcf,
                                     నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం Curl_peer *origin,
                                     నిర్మాణం Curl_peer *peer,
                                     uint8_t transport_peer,
                                     నిర్మాణం connectdata *conn,
                                     నిర్మాణం Curl_sockaddr_ex *addr,
                                     నిర్మాణం Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  నిర్మాణం cf_h3_proxy_ctx *ctx;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(!tunnel_peer)
    తిరిగి CURLE_FAILED_INIT;
  ఒకవేళ((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    తిరిగి CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  ఫలితం = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
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
    లేకపోతే ఒకవేళ(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  లేకపోతే
    CURL_TRC_CF(దత్తాంశం, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  తిరిగి ఫలితం;
}

CURLcode Curl_cf_ngtcp2_proxy_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                           నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం Curl_peer *origin,
                                           నిర్మాణం Curl_peer *peer,
                                           నిర్మాణం Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  నిర్మాణం cf_h3_proxy_ctx *ctx;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  (శూన్య)దత్తాంశం;

  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx)
    వెళ్ళు బహిర్గతం;
  ఫలితం = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_చొప్పించు_after(cf_at, cf);

బహిర్గతం:
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(cf)
      Curl_conn_cf_discard_chain(&cf, దత్తాంశం);
    లేకపోతే ఒకవేళ(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు
