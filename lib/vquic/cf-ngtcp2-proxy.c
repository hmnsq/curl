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

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#شامل "urldata.h"
#شامل "url.h"
#شامل "curl_trc.h"
#شامل "sendf.h"
#شامل "multiif.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "progress.h"
#شامل "curlx/dynbuf.h"
#شامل "http_proxy.h"
#شامل "vquic/vquic.h"
#شامل "vquic/cf-ngtcp2-cmn.h"
#شامل "vquic/cf-ngtcp2-proxy.h"
#شامل "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#تعریف_کرو PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#تعریف_کرو PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_دھارا_CHUNK_سائز)

قسمتعریف شمار {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

ساخت h3_tunnel_دھارا {
  ساخت Curl_peer *dest;  /* where the tunnel goes to */
  ساخت http_resp *resp;
  ساخت bufq recvbuf;
  حرف *authority;
  ساخت h3_دھارا_ctx *دھارا;
  h3_tunnel_state state;
  ثنائی_ہندسہ(udp);
  ثنائی_ہندسہ(has_final_response);
  ثنائی_ہندسہ(closed);
};

ساکن CURLcode h3_tunnel_دھارا_init(ساخت h3_tunnel_دھارا *ts,
                                      ساخت Curl_peer *dest,
                                      bool udp)
{
  ts->state = H3_TUNNEL_INIT;
  Curl_peer_link(&ts->dest, dest);
  Curl_bufq_init2(&ts->recvbuf, H3_دھارا_CHUNK_سائز,
                  PROXY_H3_TUNNEL_RECV_CHUNKS, BUFQ_OPT_SOFT_حد_بندی);
  ts->udp = udp;
  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->پورٹ);
  اگر(!ts->authority)
    واپس CURLE_برآمد_کا_حافظہ;

  واپس CURLE_OK;
}

ساکن خالی h3_tunnel_دھارا_reset(ساخت h3_tunnel_دھارا *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = تہی;
  ts->دھارا = تہی;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

ساکن خالی h3_tunnel_دھارا_cleanup(ساخت h3_tunnel_دھارا *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

ساکن خالی h3_tunnel_go_state(ساخت Curl_cfilter *cf,
                               ساخت h3_tunnel_دھارا *ts,
                               h3_tunnel_state new_state,
                               ساخت Curl_easy *ڈیٹا)
{
  VERBOSE(int64_t دھارا_id = ts->دھارا ? ts->دھارا->id : -1);
  (خالی)cf;

  اگر(ts->state == new_state)
    واپس;

  /* leaving this one */
  انتخاب(ts->state) {
  حالت H3_TUNNEL_CONNECT:
    ڈیٹا->req.ignorebody = FALSE;
    توڑو;
  طےشدہ:
    توڑو;
  }

  /* entering this one */
  انتخاب(new_state) {
  حالت H3_TUNNEL_INIT:
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] -> [init]", دھارا_id);
    h3_tunnel_دھارا_reset(ts);
    توڑو;
  حالت H3_TUNNEL_CONNECT:
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] -> [connect]", دھارا_id);
    ts->state = H3_TUNNEL_CONNECT;
    توڑو;
  حالت H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] -> [response]", دھارا_id);
    ts->state = H3_TUNNEL_RESPONSE;
    توڑو;
  حالت H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] -> [established]", دھارا_id);
    infof(ڈیٹا, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    ڈیٹا->state.authproxy.مکمل = TRUE;
    ڈیٹا->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(ڈیٹا->req.hd_proxy_auth);
    توڑو;
  حالت H3_TUNNEL_FAILED:
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] -> [failed]", دھارا_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(ڈیٹا->req.hd_proxy_auth);
    توڑو;
  }
}

ساخت cf_h3_proxy_ctx {
  ساخت cf_ngtcp2_ctx ngtcp2_ctx;
  ساخت h3_tunnel_دھارا tunnel; /* our tunnel CONNECT stream */
  ثنائی_ہندسہ(connected);
};

ساکن CURLcode cf_ngtcp2_proxy_h3_init(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        ساخت cf_ngtcp2_ctx *ctx);

ساکن CURLcode cf_h3_proxy_ctx_init(ساخت cf_h3_proxy_ctx *ctx,
                                     ساخت Curl_peer *origin,
                                     ساخت Curl_peer *peer,
                                     ساخت ssl_filter_config *sslc,
                                     ساخت Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode نتیجہ;
  نتیجہ = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  اگر(!نتیجہ)
    نتیجہ = h3_tunnel_دھارا_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  واپس نتیجہ;
}

ساکن خالی cf_h3_proxy_ctx_free(ساخت cf_h3_proxy_ctx *ctx)
{
  اگر(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_دھارا_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

ساکن صحیح cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t دھارا_id,
                                      uint64_t datalen, خالی *user_ڈیٹا,
                                      خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا;
  size_t skiplen;
  (خالی)دھارا_user_ڈیٹا;

  دھارا = pctx->tunnel.دھارا;
  اگر(!دھارا || (دھارا->id != دھارا_id))
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

ساکن صحیح cb_h3_proxy_دھارا_close(nghttp3_conn *conn, int64_t دھارا_id,
                                    uint64_t app_error_رمز, خالی *user_ڈیٹا,
                                    خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  ساخت h3_دھارا_ctx *دھارا;

  (خالی)conn;
  (خالی)دھارا_user_ڈیٹا;
  اگر(!ڈیٹا)
    واپس NGHTTP3_ERR_CALLBACK_FAILURE;

  دھارا = pctx->tunnel.دھارا;
  اگر(!دھارا || (دھارا->id != دھارا_id))
    واپس 0;

  دھارا->closed = TRUE;
  دھارا->error3 = app_error_رمز;
  اگر(دھارا->error3 != NGHTTP3_H3_NO_ERROR) {
    دھارا->reset = TRUE;
    دھارا->send_closed = TRUE;
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                دھارا->id, دھارا->error3);
  }
  ورنہ
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] CLOSED", دھارا->id);
  pctx->tunnel.دھارا = تہی;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(ڈیٹا);
  واپس 0;
}

ساکن خالی cf_h3_proxy_upd_rx_win(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت h3_دھارا_ctx *دھارا)
{
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  اگر(!دھارا) /* already closed */
    واپس;
  اگر(دھارا->rx_انحراف_max < دھارا->rx_انحراف) {
    DEBUGASSERT(0);
    واپس;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = دھارا->rx_انحراف_max - دھارا->rx_انحراف;
  اگر(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    اگر(UINT64_MAX - delta < دھارا->rx_انحراف_max)
      delta = UINT64_MAX - دھارا->rx_انحراف_max;
    اگر(delta) {
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  دھارا->id, cur_win, rx_buffered, delta);
      دھارا->rx_انحراف_max += delta;
      ngtcp2_conn_extend_max_دھارا_انحراف(ctx->qconn, دھارا->id, delta);
      ngtcp2_conn_extend_max_انحراف(ctx->qconn, delta);
    }
  }
}

ساکن صحیح cb_h3_proxy_recv_ڈیٹا(nghttp3_conn *conn, int64_t stream3_id,
                                 ثابت uint8_t *buf, size_t buflen,
                                 خالی *user_ڈیٹا, خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  ساخت h3_دھارا_ctx *دھارا;
  size_t nwritten;
  CURLcode نتیجہ = CURLE_OK;
  (خالی)conn;
  (خالی)stream3_id;
  (خالی)دھارا_user_ڈیٹا;

  دھارا = pctx->tunnel.دھارا;
  اگر(!ڈیٹا || !دھارا || (دھارا->id != stream3_id)) {
    واپس NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  دھارا->rx_کل += (curl_off_t)buflen;
  CURL_TRC_CF(ڈیٹا, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              دھارا->id, buflen, دھارا->rx_کل);

  نتیجہ = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  اگر(نتیجہ || (nwritten < buflen)) {
    واپس NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  دھارا->rx_انحراف += buflen;
  اگر(دھارا->rx_انحراف_max < دھارا->rx_انحراف)
    دھارا->rx_انحراف_max = دھارا->rx_انحراف;

  CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              دھارا->id, buflen, دھارا->rx_انحراف_max - دھارا->rx_انحراف);
  cf_h3_proxy_upd_rx_win(cf, ڈیٹا, دھارا);

  Curl_multi_mark_dirty(ڈیٹا);
  واپس 0;
}

ساکن صحیح cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t دھارا_id,
                                        size_t consumed, خالی *user_ڈیٹا,
                                        خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (خالی)conn;
  (خالی)دھارا_user_ڈیٹا;

  اگر(!ctx)
    واپس 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_دھارا_انحراف(ctx->qconn, دھارا_id, consumed);
  ngtcp2_conn_extend_max_انحراف(ctx->qconn, consumed);

  واپس 0;
}

ساکن صحیح cb_h3_proxy_recv_سرخی(nghttp3_conn *conn, int64_t دھارا_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *قدر, uint8_t flags,
                                   خالی *user_ڈیٹا, خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(قدر);
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  ساخت h3_دھارا_ctx *دھارا;
  CURLcode نتیجہ = CURLE_OK;
  صحیح http_حالت;
  ساخت http_resp *resp;
  (خالی)conn;
  (خالی)دھارا_id;
  (خالی)token;
  (خالی)flags;
  (خالی)دھارا_user_ڈیٹا;

  /* stream_user_data might be NULL for control streams */
  اگر(!ڈیٹا)
    واپس NGHTTP3_ERR_CALLBACK_FAILURE;

  دھارا = pctx->tunnel.دھارا;
  اگر(!دھارا || (دھارا->id != دھارا_id)) {
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                دھارا_id, (خالی *)ڈیٹا, ڈیٹا ? ڈیٹا->mid : 0);
    واپس 0;
  }

  اگر(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    واپس 0;
  }

  اگر(token == NGHTTP3_QPACK_TOKEN_حالت) {
    نتیجہ = Curl_http_decode_حالت(&دھارا->حالت_رمز,
                                     (ثابت حرف *)h3val.بنیاد, h3val.len);
    اگر(نتیجہ)
      واپس NGHTTP3_ERR_CALLBACK_FAILURE;
    http_حالت = دھارا->حالت_رمز;
    نتیجہ = Curl_http_resp_make(&resp, http_حالت, تہی);
    اگر(نتیجہ)
      واپس NGHTTP3_ERR_CALLBACK_FAILURE;
    اگر(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  ورنہ {
    اگر(!pctx->tunnel.resp) {
      واپس NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] header: %.*s: %.*s", دھارا_id,
                (صحیح)h3name.len, h3name.بنیاد, (صحیح)h3val.len, h3val.بنیاد);
    نتیجہ = Curl_dynhds_جوڑو(&pctx->tunnel.resp->سرخیاں,
                             (ثابت حرف *)h3name.بنیاد, h3name.len,
                             (ثابت حرف *)h3val.بنیاد, h3val.len);
    اگر(نتیجہ) {
      واپس NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  واپس 0;
}

ساکن صحیح cb_h3_proxy_انتہا_سرخیاں(nghttp3_conn *conn, int64_t دھارا_id,
                                   صحیح fin, خالی *user_ڈیٹا,
                                   خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  ساخت h3_دھارا_ctx *دھارا;
  (خالی)conn;
  (خالی)دھارا_id;
  (خالی)fin;
  (خالی)دھارا_user_ڈیٹا;

  اگر(!ڈیٹا)
    واپس NGHTTP3_ERR_CALLBACK_FAILURE;

  دھارا = pctx->tunnel.دھارا;
  اگر(!دھارا || (دھارا->id != دھارا_id)) {
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                دھارا_id, (خالی *)ڈیٹا, ڈیٹا ? ڈیٹا->mid : 0);
    واپس 0;
  }

  CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] end_headers, status=%d", دھارا_id,
              دھارا->حالت_رمز);

  اگر(!pctx->tunnel.has_final_response) {
    اگر(دھارا->حالت_رمز / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  اگر(دھارا->حالت_رمز / 100 != 1) {
    دھارا->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(ڈیٹا);
  واپس 0;
}

ساکن صحیح cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t دھارا_id,
                                    uint64_t app_error_رمز, خالی *user_ڈیٹا,
                                    خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (خالی)conn;
  (خالی)دھارا_user_ڈیٹا;

  اگر(ctx) {
    صحیح rv = ngtcp2_conn_shutdown_دھارا_read(ctx->qconn, 0, دھارا_id,
                                              app_error_رمز);

    اگر(rv && rv != NGTCP2_ERR_دھارا_NOT_ملا) {
      واپس NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  واپس 0;
}

ساکن صحیح cb_h3_proxy_reset_دھارا(nghttp3_conn *conn, int64_t دھارا_id,
                                    uint64_t app_error_رمز, خالی *user_ڈیٹا,
                                    خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  صحیح rv;

  (خالی)conn;
  (خالی)دھارا_user_ڈیٹا;
  اگر(!ڈیٹا)
    واپس NGHTTP3_ERR_CALLBACK_FAILURE;

  اگر(!pctx->tunnel.دھارا ||
     (دھارا_id != pctx->tunnel.دھارا->id))
    واپس 0;

  rv = ngtcp2_conn_shutdown_دھارا_write(ctx->qconn, 0, دھارا_id,
                                         app_error_رمز);
  CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] reset -> %d", دھارا_id, rv);
  pctx->tunnel.دھارا = تہی;
  pctx->tunnel.closed = TRUE;
  اگر(rv && rv != NGTCP2_ERR_دھارا_NOT_ملا) {
    واپس NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  واپس 0;
}

ساکن nghttp3_ssize cb_h3_tunnel_read_ڈیٹا(nghttp3_conn *conn,
                                            int64_t دھارا_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            خالی *user_ڈیٹا,
                                            خالی *دھارا_user_ڈیٹا)
{
  ساخت Curl_cfilter *cf = user_ڈیٹا;
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  ساخت h3_دھارا_ctx *دھارا;
  size_t nwritten = 0;
  size_t nvecs = 0;
  ثابت غیرعلامتی حرف *buf_بنیاد;

  (خالی)conn;
  (خالی)دھارا_id;
  (خالی)veccnt;
  (خالی)دھارا_user_ڈیٹا;
  (خالی)pflags;

  دھارا = pctx->tunnel.دھارا;
  اگر(!ڈیٹا || !دھارا || (دھارا->id != دھارا_id))
    واپس NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  اگر(دھارا->tx_اندر_flight_سائز < Curl_bufq_len(&دھارا->sendbuf)) {
    nvecs = 0;
    جب(nvecs < veccnt) {
      اگر(!Curl_bufq_peek_at(&دھارا->sendbuf,
                           دھارا->tx_اندر_flight_سائز,
                           &buf_بنیاد,
                           &vec[nvecs].len))
        توڑو;
      vec[nvecs].بنیاد = (uint8_t *)(uintptr_t)buf_بنیاد;
      دھارا->tx_اندر_flight_سائز += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  اگر(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] read req body -> AGAIN",
                دھارا->id);
    واپس NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              دھارا->id, nvecs,
              *pflags == NGHTTP3_ڈیٹا_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&دھارا->sendbuf));
  واپس (nghttp3_ssize)nvecs;
}

#اگر_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wmissing-field-initializers"
#اگر_ختم
ساکن nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_دھارا_close,
  cb_h3_proxy_recv_ڈیٹا,
  cb_h3_proxy_deferred_consume,
  تہی, /* begin_headers */
  cb_h3_proxy_recv_سرخی,
  cb_h3_proxy_انتہا_سرخیاں,
  تہی, /* begin_trailers */
  cb_h3_proxy_recv_سرخی,
  تہی, /* end_trailers */
  cb_h3_proxy_stop_sending,
  تہی, /* end_stream */
  cb_h3_proxy_reset_دھارا,
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

ساکن CURLcode cf_ngtcp2_proxy_h3_init(ساخت Curl_cfilter *cf,
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
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  اگر(rc) {
    failf(ڈیٹا, "error creating nghttp3 connection instance");
    واپس CURLE_برآمد_کا_حافظہ;
  }

  واپس Curl_cf_ngtcp2_h3_init_ctrls(ctx, ڈیٹا);
}

ساکن ssize_t cf_h3_proxy_recv_closed_دھارا(ساخت Curl_cfilter *cf,
                                              ساخت Curl_easy *ڈیٹا,
                                              ساخت h3_دھارا_ctx *دھارا,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  اگر(دھارا->reset) {
    اگر(دھارا->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(ڈیٹا, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", دھارا->id);
      connclose(cf->conn);
      ڈیٹا->state.refused_دھارا = TRUE;
      *err = CURLE_RECV_ERROR;
      جاؤ برآمد;
    }
    ورنہ اگر(دھارا->resp_hds_complete && ڈیٹا->req.no_body) {
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", دھارا->id, دھارا->error3,
                  Curl_vquic_h3_err_str(دھارا->error3));
      nread = 0;
      جاؤ برآمد;
    }
    failf(ڈیٹا, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", دھارا->id, دھارا->error3,
          Curl_vquic_h3_err_str(دھارا->error3));
    *err = ڈیٹا->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    جاؤ برآمد;
  }
  ورنہ اگر(!دھارا->resp_hds_complete) {
    failf(ڈیٹا,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          دھارا->id);
    *err = CURLE_HTTP3;
    جاؤ برآمد;
  }
  nread = 0;

برآمد:
  واپس nread;
}

ساکن CURLcode cf_h3_proxy_sendbuf_جوڑو(ساخت Curl_easy *ڈیٹا,
                                        ساخت h3_دھارا_ctx *دھارا,
                                        ثابت uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode نتیجہ;
  *pnwritten = 0;
  (خالی)ڈیٹا;

  نتیجہ = Curl_bufq_write(&دھارا->sendbuf, buf, len, pnwritten);
  واپس نتیجہ;
}

ساکن CURLcode cf_h3_proxy_send(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ثابت uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ساخت h3_دھارا_ctx *دھارا = تہی;
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

  اگر(pctx->tunnel.closed) {
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ denied;
  }

  دھارا = pctx->tunnel.دھارا;
  اگر(!دھارا) {
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ denied;
  }

  اگر(دھارا->closed) {
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
  ورنہ {
    نتیجہ = cf_h3_proxy_sendbuf_جوڑو(ڈیٹا, دھارا, buf, len, pnwritten);
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

برآمد:
  نتیجہ = Curl_1st_fatal(نتیجہ,
                          Curl_cf_ngtcp2_cmn_سیٹ_expiry(cf, ڈیٹا, &pktx));
denied:
  CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              دھارا ? دھارا->id : -1, len, (صحیح)نتیجہ, *pnwritten);
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

/* incoming data frames on the h3 stream */
ساکن CURLcode cf_h3_proxy_recv(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 حرف *buf, size_t len, size_t *pnread)
{
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ساخت h3_دھارا_ctx *دھارا = pctx->tunnel.دھارا;
  ساخت cf_call_ڈیٹا save;
  ساخت cf_ngtcp2_io_ctx pktx;
  CURLcode نتیجہ = CURLE_OK;

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

  اگر(!Curl_bufq_is_خالی_خانے(&pctx->tunnel.recvbuf)) {
    نتیجہ = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    اگر(نتیجہ)
      جاؤ برآمد;
    cf_h3_proxy_upd_rx_win(cf, ڈیٹا, pctx->tunnel.دھارا);
  }

  نتیجہ = Curl_cf_ngtcp2_پیش_رفت_ingress(cf, ڈیٹا, &pktx);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* inbufq had nothing before, maybe after progressing ingress? */
  اگر(!*pnread && !Curl_bufq_is_خالی_خانے(&pctx->tunnel.recvbuf)) {
    نتیجہ = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    اگر(نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  دھارا->id, len, *pnread, (صحیح)نتیجہ);
      جاؤ برآمد;
    }
    cf_h3_proxy_upd_rx_win(cf, ڈیٹا, pctx->tunnel.دھارا);
  }

  اگر(*pnread) {
    Curl_multi_mark_dirty(ڈیٹا);
  }
  ورنہ {
    اگر(دھارا->xfer_نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] xfer write failed", دھارا->id);
      Curl_cf_ngtcp2_h3_دھارا_close(cf, ڈیٹا, دھارا);
      نتیجہ = دھارا->xfer_نتیجہ;
      جاؤ برآمد;
    }
    ورنہ اگر(دھارا->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_دھارا(cf, ڈیٹا, دھارا, &نتیجہ);
      اگر(nread > 0)
        *pnread = (size_t)nread;
      جاؤ برآمد;
    }
    نتیجہ = CURLE_AGAIN;
  }

برآمد:
  نتیجہ = Curl_1st_fatal(نتیجہ,
                          Curl_cf_ngtcp2_پیش_رفت_egress(cf, ڈیٹا, &pktx));
  نتیجہ = Curl_1st_fatal(نتیجہ,
                          Curl_cf_ngtcp2_cmn_سیٹ_expiry(cf, ڈیٹا, &pktx));
denied:
  CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] cf_recv(len=%zu) -> %d, %zu",
              دھارا ? دھارا->id : -1, len, (صحیح)نتیجہ, *pnread);
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن CURLcode cf_h3_proxy_submit(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت h3_tunnel_دھارا *ts,
                                   ساخت httpreq *req)
{
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ساخت h3_دھارا_ctx *دھارا = تہی;
  ساخت dynhds h2_سرخیاں;
  nghttp3_nv *nva = تہی;
  size_t nheader;
  صحیح rc = 0;
  غیرعلامتی صحیح i;
  nghttp3_ڈیٹا_reader reader;
  nghttp3_ڈیٹا_reader *preader = تہی;
  CURLcode نتیجہ;

  Curl_dynhds_init(&h2_سرخیاں, 0, DYN_HTTP_REQUEST);
  نتیجہ = Curl_http_req_مقصد_h2(&h2_سرخیاں, req, ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = Curl_cf_ngtcp2_h3_دھارا_تیاری(cf, ڈیٹا, &دھارا);
  اگر(نتیجہ)
    جاؤ برآمد;

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

  /* Open a bidirectional stream */
  {
    int64_t sid;
    صحیح rv;

    DEBUGASSERT(دھارا->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_دھارا(ctx->qconn, &sid, تہی);
    اگر(rv) {
      failf(ڈیٹا, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
    دھارا->id = sid;
    ts->دھارا = دھارا;
    ++ctx->مستعمل_bidi_streams;
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRId64 "] opened bidi stream", sid);
  }

  /* CONNECT-UDP request stream remains open for capsules, no fixed EOF. */
  دھارا->send_closed = 0;
  reader.read_ڈیٹا = cb_h3_tunnel_read_ڈیٹا;
  preader = &reader;

  rc = nghttp3_conn_submit_request(ctx->h3conn, دھارا->id,
                                   nva, nheader, preader, دھارا);

  اگر(rc) {
    انتخاب(rc) {
    حالت NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(ڈیٹا, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", دھارا->id);
      توڑو;
    طےشدہ:
      CURL_TRC_CF(ڈیٹا, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  دھارا->id, rc, nghttp3_strerror(rc));
      توڑو;
    }
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ برآمد;
  }

  اگر(Curl_trc_is_verbose(ڈیٹا)) {
    CURL_TRC_CF(ڈیٹا, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", دھارا->id,
                Curl_bufref_ptr(&ڈیٹا->state.url));
  }

برآمد:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرخیاں);
  واپس نتیجہ;
}

ساکن CURLcode cf_h3_proxy_adjust_pollset(ساخت Curl_cfilter *cf,
                                           ساخت Curl_easy *ڈیٹا,
                                           ساخت easy_pollset *ps)
{
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode نتیجہ = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, ڈیٹا);

  اگر(!ctx->qconn || !pctx->tunnel.دھارا || (sock == CURL_SOCKET_BAD))
    واپس CURLE_OK;

  Curl_pollset_check(ڈیٹا, ps, sock, &want_recv, &want_send);

  اگر(want_recv || want_send || !Curl_bufq_is_خالی_خانے(&ctx->q.sendbuf)) {
    ساخت h3_دھارا_ctx *دھارا = pctx->tunnel.دھارا;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_بایاں(ctx->qconn) ||
                 !ngtcp2_conn_get_max_ڈیٹا_بایاں(ctx->qconn));
    s_exhaust = want_send && دھارا && دھارا->id >= 0 &&
                دھارا->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_خالی_خانے(&ctx->q.sendbuf);

    نتیجہ = Curl_pollset_سیٹ(ڈیٹا, ps, sock, want_recv, want_send);
  }
  واپس نتیجہ;
}

ساکن bool cf_h3_proxy_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                                     ثابت ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  اگر(!Curl_bufq_is_خالی_خانے(&pctx->tunnel.recvbuf))
    واپس TRUE;
  واپس cf->اگلا ?
    cf->اگلا->cft->has_ڈیٹا_pending(cf->اگلا, ڈیٹا) : FALSE;
}

ساکن CURLcode cf_h3_proxy_submit_CONNECT(ساخت Curl_cfilter *cf,
                                           ساخت Curl_easy *ڈیٹا,
                                           ساخت h3_tunnel_دھارا *ts)
{
  ساخت cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;
  ساخت httpreq *req = تہی;

  نتیجہ = Curl_http_proxy_create_tunnel_request(
    &req, cf, ڈیٹا, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  اگر(!نتیجہ)
    نتیجہ = Curl_creader_سیٹ_null(ڈیٹا);
  اگر(!نتیجہ)
    نتیجہ = cf_h3_proxy_submit(cf, ڈیٹا, ts, req);

  اگر(req)
    Curl_http_req_free(req);
  اگر(نتیجہ)
    failf(ڈیٹا, "Failed sending CONNECT to proxy");
  واپس نتیجہ;
}

ساکن CURLcode cf_h3_proxy_inspect_response(ساخت Curl_cfilter *cf,
                                             ساخت Curl_easy *ڈیٹا,
                                             ساخت h3_tunnel_دھارا *ts)
{
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_نتیجہ res;
  CURLcode نتیجہ;

  نتیجہ = Curl_http_proxy_inspect_tunnel_response(
      cf, ڈیٹا, ts->resp, (bool)pctx->tunnel.udp, &res);
  اگر(نتیجہ)
    واپس نتیجہ;
  انتخاب(res) {
  حالت PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, ڈیٹا);
    توڑو;
  حالت PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, ڈیٹا);
    نتیجہ = CURLE_COULDNT_CONNECT;
    توڑو;
  حالت PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, ڈیٹا);
    توڑو;
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_h3_proxy_tunnel(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت h3_tunnel_دھارا *ts,
                                   bool *pdone)
{
  ساخت cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  کرو {
    انتخاب(ts->state) {
    حالت H3_TUNNEL_INIT:
      CURL_TRC_CF(ڈیٹا, cf, "[0] CONNECT start for %s", ts->authority);
      نتیجہ = cf_h3_proxy_submit_CONNECT(cf, ڈیٹا, ts);
      اگر(نتیجہ)
        جاؤ برآمد;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, ڈیٹا);

      نتیجہ = Curl_cf_ngtcp2_پیش_رفت_egress(cf, ڈیٹا, تہی);
      اگر(نتیجہ)
        جاؤ برآمد;
      FALLTHROUGH();

    حالت H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      نتیجہ = Curl_cf_ngtcp2_پیش_رفت_ingress(cf, ڈیٹا, تہی);
      اگر(نتیجہ)
        جاؤ برآمد;
      نتیجہ = Curl_cf_ngtcp2_پیش_رفت_egress(cf, ڈیٹا, تہی);
      اگر(نتیجہ && نتیجہ != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, ڈیٹا);
        جاؤ برآمد;
      }

      اگر(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, ڈیٹا);
      }
      ورنہ {
        /* Not done yet, return and let multi interface call us again */
        نتیجہ = CURLE_OK;
        جاؤ برآمد;
      }
      FALLTHROUGH();

    حالت H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      نتیجہ = cf_h3_proxy_inspect_response(cf, ڈیٹا, ts);
      اگر(نتیجہ)
        جاؤ برآمد;
      ctx->connected = TRUE;
      توڑو;

    حالت H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      واپس CURLE_OK;

    حالت H3_TUNNEL_FAILED:
      واپس CURLE_RECV_ERROR;

    طےشدہ:
      توڑو;
    }

  } جب(ts->state == H3_TUNNEL_INIT);

برآمد:
  اگر((نتیجہ && (نتیجہ != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, ڈیٹا);
  واپس نتیجہ;
}

ساکن CURLcode cf_h3_proxy_connect(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت cf_h3_proxy_ctx *pctx = cf->ctx;
  ساخت cf_call_ڈیٹا save = { 0 };
  CURLcode نتیجہ = CURLE_OK;
  ساخت h3_tunnel_دھارا *ts = &pctx->tunnel;
  bool ڈیٹا_saved = FALSE;

  نتیجہ = Curl_cf_ngtcp2_cmn_connect(cf, ڈیٹا, مکمل);
  اگر(نتیجہ || !*مکمل)
    جاؤ برآمد;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  ڈیٹا_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  نتیجہ = cf_h3_proxy_tunnel(cf, ڈیٹا, ts, مکمل);

برآمد:
  اگر(*مکمل) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&ڈیٹا->req, ڈیٹا);
    Curl_client_reset(ڈیٹا);
  }

  اگر(ڈیٹا_saved)
    CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن خالی cf_h3_proxy_destroy(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h3_proxy_ctx *ctx = cf->ctx;

  (خالی)ڈیٹا;
  اگر(ctx) {
    CURL_TRC_CF(ڈیٹا, cf, "cf_h3_proxy_destroy()");
    اگر(ctx->ngtcp2_ctx.qconn) {
      ساخت cf_call_ڈیٹا save;
      CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
      Curl_cf_ngtcp2_cmn_conn_close(cf, ڈیٹا);
      CF_ڈیٹا_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = تہی;
  }
}

ساکن CURLcode cf_h3_proxy_shutdown(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  واپس Curl_cf_ngtcp2_cmn_shutdown(cf, ڈیٹا, مکمل);
}

ساخت Curl_cftype Curl_cft_h3_proxy = {
  "H3-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  cf_h3_proxy_destroy,
  cf_h3_proxy_connect,
  cf_h3_proxy_shutdown,
  cf_h3_proxy_adjust_pollset,
  cf_h3_proxy_ڈیٹا_pending,
  cf_h3_proxy_send,
  cf_h3_proxy_recv,
  Curl_cf_def_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_ngtcp2_cmn_query,
};

CURLcode Curl_cf_ngtcp2_proxy_create(ساخت Curl_cfilter **pcf,
                                     ساخت Curl_easy *ڈیٹا,
                                     ساخت Curl_peer *origin,
                                     ساخت Curl_peer *peer,
                                     uint8_t transport_peer,
                                     ساخت connectdata *conn,
                                     ساخت Curl_sockaddr_ex *addr,
                                     ساخت Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  ساخت Curl_cfilter *cf = تہی;
  ساخت cf_h3_proxy_ctx *ctx;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  اگر(!tunnel_peer)
    واپس CURLE_FAILED_INIT;
  اگر((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    واپس CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  نتیجہ = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
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
    ورنہ اگر(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  ورنہ
    CURL_TRC_CF(ڈیٹا, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  واپس نتیجہ;
}

CURLcode Curl_cf_ngtcp2_proxy_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                           ساخت Curl_easy *ڈیٹا,
                                           ساخت Curl_peer *origin,
                                           ساخت Curl_peer *peer,
                                           ساخت Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  ساخت Curl_cfilter *cf = تہی;
  ساخت cf_h3_proxy_ctx *ctx;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  (خالی)ڈیٹا;

  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx)
    جاؤ برآمد;
  نتیجہ = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);

برآمد:
  اگر(نتیجہ) {
    اگر(cf)
      Curl_conn_cf_discard_chain(&cf, ڈیٹا);
    ورنہ اگر(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  واپس نتیجہ;
}

#اگر_ختم
