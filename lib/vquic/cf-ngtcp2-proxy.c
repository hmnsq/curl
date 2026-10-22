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

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#ضمن "urldata.h"
#ضمن "url.h"
#ضمن "curl_trc.h"
#ضمن "sendf.h"
#ضمن "multiif.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "progress.h"
#ضمن "curlx/dynbuf.h"
#ضمن "http_proxy.h"
#ضمن "vquic/vquic.h"
#ضمن "vquic/cf-ngtcp2-cmn.h"
#ضمن "vquic/cf-ngtcp2-proxy.h"
#ضمن "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#تعریف_کرو PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#تعریف_کرو PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_دھارا_CHUNK_ناپ)

تعريفنوع تعداد {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

بنية h3_tunnel_دھارا {
  بنية Curl_peer *dest;  /* where the tunnel goes to */
  بنية http_resp *resp;
  بنية bufq recvbuf;
  حرف *authority;
  بنية h3_دھارا_ctx *دھارا;
  h3_tunnel_state state;
  ثنائی_ہندسہ(udp);
  ثنائی_ہندسہ(has_final_response);
  ثنائی_ہندسہ(closed);
};

ساكن CURLcode h3_tunnel_دھارا_init(بنية h3_tunnel_دھارا *ts,
                                      بنية Curl_peer *dest,
                                      bool udp)
{
  ts->state = H3_TUNNEL_INIT;
  Curl_peer_link(&ts->dest, dest);
  Curl_bufq_init2(&ts->recvbuf, H3_دھارا_CHUNK_ناپ,
                  PROXY_H3_TUNNEL_RECV_CHUNKS, BUFQ_OPT_SOFT_حد_بندی);
  ts->udp = udp;
  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->پورٹ);
  إذا(!ts->authority)
    أرجع CURLE_باہرلا_دا_حافظہ;

  أرجع CURLE_OK;
}

ساكن فراغ h3_tunnel_دھارا_reset(بنية h3_tunnel_دھارا *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = عدم;
  ts->دھارا = عدم;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

ساكن فراغ h3_tunnel_دھارا_cleanup(بنية h3_tunnel_دھارا *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

ساكن فراغ h3_tunnel_go_state(بنية Curl_cfilter *cf,
                               بنية h3_tunnel_دھارا *ts,
                               h3_tunnel_state new_state,
                               بنية Curl_easy *انکڑے)
{
  VERBOSE(int64_t دھارا_id = ts->دھارا ? ts->دھارا->id : -1);
  (فراغ)cf;

  إذا(ts->state == new_state)
    أرجع;

  /* leaving this one */
  بدل(ts->state) {
  حالة H3_TUNNEL_CONNECT:
    انکڑے->req.ignorebody = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }

  /* entering this one */
  بدل(new_state) {
  حالة H3_TUNNEL_INIT:
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] -> [init]", دھارا_id);
    h3_tunnel_دھارا_reset(ts);
    اكسر;
  حالة H3_TUNNEL_CONNECT:
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] -> [connect]", دھارا_id);
    ts->state = H3_TUNNEL_CONNECT;
    اكسر;
  حالة H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] -> [response]", دھارا_id);
    ts->state = H3_TUNNEL_RESPONSE;
    اكسر;
  حالة H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] -> [established]", دھارا_id);
    infof(انکڑے, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    انکڑے->state.authproxy.پورا = TRUE;
    انکڑے->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(انکڑے->req.hd_proxy_auth);
    اكسر;
  حالة H3_TUNNEL_FAILED:
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] -> [failed]", دھارا_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(انکڑے->req.hd_proxy_auth);
    اكسر;
  }
}

بنية cf_h3_proxy_ctx {
  بنية cf_ngtcp2_ctx ngtcp2_ctx;
  بنية h3_tunnel_دھارا tunnel; /* our tunnel CONNECT stream */
  ثنائی_ہندسہ(connected);
};

ساكن CURLcode cf_ngtcp2_proxy_h3_init(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *انکڑے,
                                        بنية cf_ngtcp2_ctx *ctx);

ساكن CURLcode cf_h3_proxy_ctx_init(بنية cf_h3_proxy_ctx *ctx,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     بنية ssl_filter_config *sslc,
                                     بنية Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode نتیجہ;
  نتیجہ = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  إذا(!نتیجہ)
    نتیجہ = h3_tunnel_دھارا_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  أرجع نتیجہ;
}

ساكن فراغ cf_h3_proxy_ctx_free(بنية cf_h3_proxy_ctx *ctx)
{
  إذا(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_دھارا_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

ساكن صحيح cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t دھارا_id,
                                      uint64_t datalen, فراغ *user_انکڑے,
                                      فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا;
  size_t skiplen;
  (فراغ)دھارا_user_انکڑے;

  دھارا = pctx->tunnel.دھارا;
  إذا(!دھارا || (دھارا->id != دھارا_id))
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

ساكن صحيح cb_h3_proxy_دھارا_close(nghttp3_conn *conn, int64_t دھارا_id,
                                    uint64_t app_error_کوڈ, فراغ *user_انکڑے,
                                    فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  بنية h3_دھارا_ctx *دھارا;

  (فراغ)conn;
  (فراغ)دھارا_user_انکڑے;
  إذا(!انکڑے)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  دھارا = pctx->tunnel.دھارا;
  إذا(!دھارا || (دھارا->id != دھارا_id))
    أرجع 0;

  دھارا->closed = TRUE;
  دھارا->error3 = app_error_کوڈ;
  إذا(دھارا->error3 != NGHTTP3_H3_NO_ERROR) {
    دھارا->reset = TRUE;
    دھارا->send_closed = TRUE;
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                دھارا->id, دھارا->error3);
  }
  وإلا
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] CLOSED", دھارا->id);
  pctx->tunnel.دھارا = عدم;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(انکڑے);
  أرجع 0;
}

ساكن فراغ cf_h3_proxy_upd_rx_win(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے,
                                   بنية h3_دھارا_ctx *دھارا)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  إذا(!دھارا) /* already closed */
    أرجع;
  إذا(دھارا->rx_وتھ_max < دھارا->rx_وتھ) {
    DEBUGASSERT(0);
    أرجع;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = دھارا->rx_وتھ_max - دھارا->rx_وتھ;
  إذا(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    إذا(UINT64_MAX - delta < دھارا->rx_وتھ_max)
      delta = UINT64_MAX - دھارا->rx_وتھ_max;
    إذا(delta) {
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  دھارا->id, cur_win, rx_buffered, delta);
      دھارا->rx_وتھ_max += delta;
      ngtcp2_conn_extend_max_دھارا_وتھ(ctx->qconn, دھارا->id, delta);
      ngtcp2_conn_extend_max_وتھ(ctx->qconn, delta);
    }
  }
}

ساكن صحيح cb_h3_proxy_recv_انکڑے(nghttp3_conn *conn, int64_t stream3_id,
                                 ثابت uint8_t *buf, size_t buflen,
                                 فراغ *user_انکڑے, فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  بنية h3_دھارا_ctx *دھارا;
  size_t nwritten;
  CURLcode نتیجہ = CURLE_OK;
  (فراغ)conn;
  (فراغ)stream3_id;
  (فراغ)دھارا_user_انکڑے;

  دھارا = pctx->tunnel.دھارا;
  إذا(!انکڑے || !دھارا || (دھارا->id != stream3_id)) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  دھارا->rx_کل += (curl_off_t)buflen;
  CURL_TRC_CF(انکڑے, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              دھارا->id, buflen, دھارا->rx_کل);

  نتیجہ = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  إذا(نتیجہ || (nwritten < buflen)) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  دھارا->rx_وتھ += buflen;
  إذا(دھارا->rx_وتھ_max < دھارا->rx_وتھ)
    دھارا->rx_وتھ_max = دھارا->rx_وتھ;

  CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              دھارا->id, buflen, دھارا->rx_وتھ_max - دھارا->rx_وتھ);
  cf_h3_proxy_upd_rx_win(cf, انکڑے, دھارا);

  Curl_multi_mark_dirty(انکڑے);
  أرجع 0;
}

ساكن صحيح cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t دھارا_id,
                                        size_t consumed, فراغ *user_انکڑے,
                                        فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (فراغ)conn;
  (فراغ)دھارا_user_انکڑے;

  إذا(!ctx)
    أرجع 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_دھارا_وتھ(ctx->qconn, دھارا_id, consumed);
  ngtcp2_conn_extend_max_وتھ(ctx->qconn, consumed);

  أرجع 0;
}

ساكن صحيح cb_h3_proxy_recv_سرخی(nghttp3_conn *conn, int64_t دھارا_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *قدر, uint8_t flags,
                                   فراغ *user_انکڑے, فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(قدر);
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  بنية h3_دھارا_ctx *دھارا;
  CURLcode نتیجہ = CURLE_OK;
  صحيح http_حالت;
  بنية http_resp *resp;
  (فراغ)conn;
  (فراغ)دھارا_id;
  (فراغ)token;
  (فراغ)flags;
  (فراغ)دھارا_user_انکڑے;

  /* stream_user_data might be NULL for control streams */
  إذا(!انکڑے)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  دھارا = pctx->tunnel.دھارا;
  إذا(!دھارا || (دھارا->id != دھارا_id)) {
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                دھارا_id, (فراغ *)انکڑے, انکڑے ? انکڑے->mid : 0);
    أرجع 0;
  }

  إذا(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    أرجع 0;
  }

  إذا(token == NGHTTP3_QPACK_TOKEN_حالت) {
    نتیجہ = Curl_http_decode_حالت(&دھارا->حالت_کوڈ,
                                     (ثابت حرف *)h3val.ادھار, h3val.len);
    إذا(نتیجہ)
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    http_حالت = دھارا->حالت_کوڈ;
    نتیجہ = Curl_http_resp_make(&resp, http_حالت, عدم);
    إذا(نتیجہ)
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    إذا(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  وإلا {
    إذا(!pctx->tunnel.resp) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] header: %.*s: %.*s", دھارا_id,
                (صحيح)h3name.len, h3name.ادھار, (صحيح)h3val.len, h3val.ادھار);
    نتیجہ = Curl_dynhds_جوڑو(&pctx->tunnel.resp->سرخیاں,
                             (ثابت حرف *)h3name.ادھار, h3name.len,
                             (ثابت حرف *)h3val.ادھار, h3val.len);
    إذا(نتیجہ) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  أرجع 0;
}

ساكن صحيح cb_h3_proxy_سرا_سرخیاں(nghttp3_conn *conn, int64_t دھارا_id,
                                   صحيح fin, فراغ *user_انکڑے,
                                   فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  بنية h3_دھارا_ctx *دھارا;
  (فراغ)conn;
  (فراغ)دھارا_id;
  (فراغ)fin;
  (فراغ)دھارا_user_انکڑے;

  إذا(!انکڑے)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  دھارا = pctx->tunnel.دھارا;
  إذا(!دھارا || (دھارا->id != دھارا_id)) {
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                دھارا_id, (فراغ *)انکڑے, انکڑے ? انکڑے->mid : 0);
    أرجع 0;
  }

  CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] end_headers, status=%d", دھارا_id,
              دھارا->حالت_کوڈ);

  إذا(!pctx->tunnel.has_final_response) {
    إذا(دھارا->حالت_کوڈ / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  إذا(دھارا->حالت_کوڈ / 100 != 1) {
    دھارا->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(انکڑے);
  أرجع 0;
}

ساكن صحيح cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t دھارا_id,
                                    uint64_t app_error_کوڈ, فراغ *user_انکڑے,
                                    فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (فراغ)conn;
  (فراغ)دھارا_user_انکڑے;

  إذا(ctx) {
    صحيح rv = ngtcp2_conn_shutdown_دھارا_read(ctx->qconn, 0, دھارا_id,
                                              app_error_کوڈ);

    إذا(rv && rv != NGTCP2_ERR_دھارا_NOT_لبھیا) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  أرجع 0;
}

ساكن صحيح cb_h3_proxy_reset_دھارا(nghttp3_conn *conn, int64_t دھارا_id,
                                    uint64_t app_error_کوڈ, فراغ *user_انکڑے,
                                    فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  صحيح rv;

  (فراغ)conn;
  (فراغ)دھارا_user_انکڑے;
  إذا(!انکڑے)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  إذا(!pctx->tunnel.دھارا ||
     (دھارا_id != pctx->tunnel.دھارا->id))
    أرجع 0;

  rv = ngtcp2_conn_shutdown_دھارا_write(ctx->qconn, 0, دھارا_id,
                                         app_error_کوڈ);
  CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] reset -> %d", دھارا_id, rv);
  pctx->tunnel.دھارا = عدم;
  pctx->tunnel.closed = TRUE;
  إذا(rv && rv != NGTCP2_ERR_دھارا_NOT_لبھیا) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  أرجع 0;
}

ساكن nghttp3_ssize cb_h3_tunnel_read_انکڑے(nghttp3_conn *conn,
                                            int64_t دھارا_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            فراغ *user_انکڑے,
                                            فراغ *دھارا_user_انکڑے)
{
  بنية Curl_cfilter *cf = user_انکڑے;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  بنية h3_دھارا_ctx *دھارا;
  size_t nwritten = 0;
  size_t nvecs = 0;
  ثابت بنانشانی حرف *buf_ادھار;

  (فراغ)conn;
  (فراغ)دھارا_id;
  (فراغ)veccnt;
  (فراغ)دھارا_user_انکڑے;
  (فراغ)pflags;

  دھارا = pctx->tunnel.دھارا;
  إذا(!انکڑے || !دھارا || (دھارا->id != دھارا_id))
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  إذا(دھارا->tx_وچ_flight_ناپ < Curl_bufq_len(&دھارا->sendbuf)) {
    nvecs = 0;
    طالما(nvecs < veccnt) {
      إذا(!Curl_bufq_peek_at(&دھارا->sendbuf,
                           دھارا->tx_وچ_flight_ناپ,
                           &buf_ادھار,
                           &vec[nvecs].len))
        اكسر;
      vec[nvecs].ادھار = (uint8_t *)(uintptr_t)buf_ادھار;
      دھارا->tx_وچ_flight_ناپ += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  إذا(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] read req body -> AGAIN",
                دھارا->id);
    أرجع NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              دھارا->id, nvecs,
              *pflags == NGHTTP3_انکڑے_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&دھارا->sendbuf));
  أرجع (nghttp3_ssize)nvecs;
}

#جے_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wmissing-field-initializers"
#جے_انت
ساكن nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_دھارا_close,
  cb_h3_proxy_recv_انکڑے,
  cb_h3_proxy_deferred_consume,
  عدم, /* begin_headers */
  cb_h3_proxy_recv_سرخی,
  cb_h3_proxy_سرا_سرخیاں,
  عدم, /* begin_trailers */
  cb_h3_proxy_recv_سرخی,
  عدم, /* end_trailers */
  cb_h3_proxy_stop_sending,
  عدم, /* end_stream */
  cb_h3_proxy_reset_دھارا,
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

ساكن CURLcode cf_ngtcp2_proxy_h3_init(بنية Curl_cfilter *cf,
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
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  إذا(rc) {
    failf(انکڑے, "error creating nghttp3 connection instance");
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  أرجع Curl_cf_ngtcp2_h3_init_ctrls(ctx, انکڑے);
}

ساكن ssize_t cf_h3_proxy_recv_closed_دھارا(بنية Curl_cfilter *cf,
                                              بنية Curl_easy *انکڑے,
                                              بنية h3_دھارا_ctx *دھارا,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  إذا(دھارا->reset) {
    إذا(دھارا->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(انکڑے, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", دھارا->id);
      connclose(cf->conn);
      انکڑے->state.refused_دھارا = TRUE;
      *err = CURLE_RECV_ERROR;
      اذهب باہرلا;
    }
    وإلا إذا(دھارا->resp_hds_complete && انکڑے->req.no_body) {
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", دھارا->id, دھارا->error3,
                  Curl_vquic_h3_err_str(دھارا->error3));
      nread = 0;
      اذهب باہرلا;
    }
    failf(انکڑے, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", دھارا->id, دھارا->error3,
          Curl_vquic_h3_err_str(دھارا->error3));
    *err = انکڑے->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    اذهب باہرلا;
  }
  وإلا إذا(!دھارا->resp_hds_complete) {
    failf(انکڑے,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          دھارا->id);
    *err = CURLE_HTTP3;
    اذهب باہرلا;
  }
  nread = 0;

باہرلا:
  أرجع nread;
}

ساكن CURLcode cf_h3_proxy_sendbuf_جوڑو(بنية Curl_easy *انکڑے,
                                        بنية h3_دھارا_ctx *دھارا,
                                        ثابت uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode نتیجہ;
  *pnwritten = 0;
  (فراغ)انکڑے;

  نتیجہ = Curl_bufq_write(&دھارا->sendbuf, buf, len, pnwritten);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h3_proxy_send(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 ثابت uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  بنية h3_دھارا_ctx *دھارا = عدم;
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

  إذا(pctx->tunnel.closed) {
    نتیجہ = CURLE_SEND_ERROR;
    اذهب denied;
  }

  دھارا = pctx->tunnel.دھارا;
  إذا(!دھارا) {
    نتیجہ = CURLE_SEND_ERROR;
    اذهب denied;
  }

  إذا(دھارا->closed) {
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
  وإلا {
    نتیجہ = cf_h3_proxy_sendbuf_جوڑو(انکڑے, دھارا, buf, len, pnwritten);
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

باہرلا:
  نتیجہ = Curl_1st_fatal(نتیجہ,
                          Curl_cf_ngtcp2_cmn_سیٹ_expiry(cf, انکڑے, &pktx));
denied:
  CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              دھارا ? دھارا->id : -1, len, (صحيح)نتیجہ, *pnwritten);
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

/* incoming data frames on the h3 stream */
ساكن CURLcode cf_h3_proxy_recv(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  بنية h3_دھارا_ctx *دھارا = pctx->tunnel.دھارا;
  بنية cf_call_انکڑے save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode نتیجہ = CURLE_OK;

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

  إذا(!Curl_bufq_is_خالی_تھاواں(&pctx->tunnel.recvbuf)) {
    نتیجہ = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    إذا(نتیجہ)
      اذهب باہرلا;
    cf_h3_proxy_upd_rx_win(cf, انکڑے, pctx->tunnel.دھارا);
  }

  نتیجہ = Curl_cf_ngtcp2_ترقی_ingress(cf, انکڑے, &pktx);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* inbufq had nothing before, maybe after progressing ingress? */
  إذا(!*pnread && !Curl_bufq_is_خالی_تھاواں(&pctx->tunnel.recvbuf)) {
    نتیجہ = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    إذا(نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  دھارا->id, len, *pnread, (صحيح)نتیجہ);
      اذهب باہرلا;
    }
    cf_h3_proxy_upd_rx_win(cf, انکڑے, pctx->tunnel.دھارا);
  }

  إذا(*pnread) {
    Curl_multi_mark_dirty(انکڑے);
  }
  وإلا {
    إذا(دھارا->xfer_نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] xfer write failed", دھارا->id);
      Curl_cf_ngtcp2_h3_دھارا_close(cf, انکڑے, دھارا);
      نتیجہ = دھارا->xfer_نتیجہ;
      اذهب باہرلا;
    }
    وإلا إذا(دھارا->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_دھارا(cf, انکڑے, دھارا, &نتیجہ);
      إذا(nread > 0)
        *pnread = (size_t)nread;
      اذهب باہرلا;
    }
    نتیجہ = CURLE_AGAIN;
  }

باہرلا:
  نتیجہ = Curl_1st_fatal(نتیجہ,
                          Curl_cf_ngtcp2_ترقی_egress(cf, انکڑے, &pktx));
  نتیجہ = Curl_1st_fatal(نتیجہ,
                          Curl_cf_ngtcp2_cmn_سیٹ_expiry(cf, انکڑے, &pktx));
denied:
  CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] cf_recv(len=%zu) -> %d, %zu",
              دھارا ? دھارا->id : -1, len, (صحيح)نتیجہ, *pnread);
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h3_proxy_submit(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے,
                                   بنية h3_tunnel_دھارا *ts,
                                   بنية httpreq *req)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  بنية h3_دھارا_ctx *دھارا = عدم;
  بنية dynhds h2_سرخیاں;
  nghttp3_nv *nva = عدم;
  size_t nheader;
  صحيح rc = 0;
  بنانشانی صحيح i;
  nghttp3_انکڑے_reader reader;
  nghttp3_انکڑے_reader *preader = عدم;
  CURLcode نتیجہ;

  Curl_dynhds_init(&h2_سرخیاں, 0, DYN_HTTP_REQUEST);
  نتیجہ = Curl_http_req_منزل_h2(&h2_سرخیاں, req, انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = Curl_cf_ngtcp2_h3_دھارا_تیاری(cf, انکڑے, &دھارا);
  إذا(نتیجہ)
    اذهب باہرلا;

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

  /* Open a bidirectional stream */
  {
    int64_t sid;
    صحيح rv;

    DEBUGASSERT(دھارا->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_دھارا(ctx->qconn, &sid, عدم);
    إذا(rv) {
      failf(انکڑے, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      نتیجہ = CURLE_SEND_ERROR;
      اذهب باہرلا;
    }
    دھارا->id = sid;
    ts->دھارا = دھارا;
    ++ctx->ورتیا_bidi_streams;
    CURL_TRC_CF(انکڑے, cf, "[%" PRId64 "] opened bidi stream", sid);
  }

  /* CONNECT-UDP request stream remains open for capsules, no fixed EOF. */
  دھارا->send_closed = 0;
  reader.read_انکڑے = cb_h3_tunnel_read_انکڑے;
  preader = &reader;

  rc = nghttp3_conn_submit_request(ctx->h3conn, دھارا->id,
                                   nva, nheader, preader, دھارا);

  إذا(rc) {
    بدل(rc) {
    حالة NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(انکڑے, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", دھارا->id);
      اكسر;
    افتراضي:
      CURL_TRC_CF(انکڑے, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  دھارا->id, rc, nghttp3_strerror(rc));
      اكسر;
    }
    نتیجہ = CURLE_SEND_ERROR;
    اذهب باہرلا;
  }

  إذا(Curl_trc_is_verbose(انکڑے)) {
    CURL_TRC_CF(انکڑے, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", دھارا->id,
                Curl_bufref_ptr(&انکڑے->state.url));
  }

باہرلا:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرخیاں);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h3_proxy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *انکڑے,
                                           بنية easy_pollset *ps)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode نتیجہ = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, انکڑے);

  إذا(!ctx->qconn || !pctx->tunnel.دھارا || (sock == CURL_SOCKET_BAD))
    أرجع CURLE_OK;

  Curl_pollset_check(انکڑے, ps, sock, &want_recv, &want_send);

  إذا(want_recv || want_send || !Curl_bufq_is_خالی_تھاواں(&ctx->q.sendbuf)) {
    بنية h3_دھارا_ctx *دھارا = pctx->tunnel.دھارا;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_بایاں(ctx->qconn) ||
                 !ngtcp2_conn_get_max_انکڑے_بایاں(ctx->qconn));
    s_exhaust = want_send && دھارا && دھارا->id >= 0 &&
                دھارا->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_خالی_تھاواں(&ctx->q.sendbuf);

    نتیجہ = Curl_pollset_سیٹ(انکڑے, ps, sock, want_recv, want_send);
  }
  أرجع نتیجہ;
}

ساكن bool cf_h3_proxy_انکڑے_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *انکڑے)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  إذا(!Curl_bufq_is_خالی_تھاواں(&pctx->tunnel.recvbuf))
    أرجع TRUE;
  أرجع cf->اگلا ?
    cf->اگلا->cft->has_انکڑے_pending(cf->اگلا, انکڑے) : FALSE;
}

ساكن CURLcode cf_h3_proxy_submit_CONNECT(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *انکڑے,
                                           بنية h3_tunnel_دھارا *ts)
{
  بنية cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;
  بنية httpreq *req = عدم;

  نتیجہ = Curl_http_proxy_create_tunnel_request(
    &req, cf, انکڑے, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  إذا(!نتیجہ)
    نتیجہ = Curl_creader_سیٹ_null(انکڑے);
  إذا(!نتیجہ)
    نتیجہ = cf_h3_proxy_submit(cf, انکڑے, ts, req);

  إذا(req)
    Curl_http_req_free(req);
  إذا(نتیجہ)
    failf(انکڑے, "Failed sending CONNECT to proxy");
  أرجع نتیجہ;
}

ساكن CURLcode cf_h3_proxy_inspect_response(بنية Curl_cfilter *cf,
                                             بنية Curl_easy *انکڑے,
                                             بنية h3_tunnel_دھارا *ts)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_نتیجہ res;
  CURLcode نتیجہ;

  نتیجہ = Curl_http_proxy_inspect_tunnel_response(
      cf, انکڑے, ts->resp, (bool)pctx->tunnel.udp, &res);
  إذا(نتیجہ)
    أرجع نتیجہ;
  بدل(res) {
  حالة PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, انکڑے);
    اكسر;
  حالة PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, انکڑے);
    نتیجہ = CURLE_COULDNT_CONNECT;
    اكسر;
  حالة PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, انکڑے);
    اكسر;
  }
  أرجع نتیجہ;
}

ساكن CURLcode cf_h3_proxy_tunnel(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے,
                                   بنية h3_tunnel_دھارا *ts,
                                   bool *pdone)
{
  بنية cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  افعل {
    بدل(ts->state) {
    حالة H3_TUNNEL_INIT:
      CURL_TRC_CF(انکڑے, cf, "[0] CONNECT start for %s", ts->authority);
      نتیجہ = cf_h3_proxy_submit_CONNECT(cf, انکڑے, ts);
      إذا(نتیجہ)
        اذهب باہرلا;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, انکڑے);

      نتیجہ = Curl_cf_ngtcp2_ترقی_egress(cf, انکڑے, عدم);
      إذا(نتیجہ)
        اذهب باہرلا;
      FALLTHROUGH();

    حالة H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      نتیجہ = Curl_cf_ngtcp2_ترقی_ingress(cf, انکڑے, عدم);
      إذا(نتیجہ)
        اذهب باہرلا;
      نتیجہ = Curl_cf_ngtcp2_ترقی_egress(cf, انکڑے, عدم);
      إذا(نتیجہ && نتیجہ != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, انکڑے);
        اذهب باہرلا;
      }

      إذا(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, انکڑے);
      }
      وإلا {
        /* Not done yet, return and let multi interface call us again */
        نتیجہ = CURLE_OK;
        اذهب باہرلا;
      }
      FALLTHROUGH();

    حالة H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      نتیجہ = cf_h3_proxy_inspect_response(cf, انکڑے, ts);
      إذا(نتیجہ)
        اذهب باہرلا;
      ctx->connected = TRUE;
      اكسر;

    حالة H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      أرجع CURLE_OK;

    حالة H3_TUNNEL_FAILED:
      أرجع CURLE_RECV_ERROR;

    افتراضي:
      اكسر;
    }

  } طالما(ts->state == H3_TUNNEL_INIT);

باہرلا:
  إذا((نتیجہ && (نتیجہ != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, انکڑے);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h3_proxy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_call_انکڑے save = { 0 };
  CURLcode نتیجہ = CURLE_OK;
  بنية h3_tunnel_دھارا *ts = &pctx->tunnel;
  bool انکڑے_saved = FALSE;

  نتیجہ = Curl_cf_ngtcp2_cmn_connect(cf, انکڑے, پورا);
  إذا(نتیجہ || !*پورا)
    اذهب باہرلا;

  CF_انکڑے_SAVE(save, cf, انکڑے);
  انکڑے_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  نتیجہ = cf_h3_proxy_tunnel(cf, انکڑے, ts, پورا);

باہرلا:
  إذا(*پورا) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&انکڑے->req, انکڑے);
    Curl_client_reset(انکڑے);
  }

  إذا(انکڑے_saved)
    CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن فراغ cf_h3_proxy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے)
{
  بنية cf_h3_proxy_ctx *ctx = cf->ctx;

  (فراغ)انکڑے;
  إذا(ctx) {
    CURL_TRC_CF(انکڑے, cf, "cf_h3_proxy_destroy()");
    إذا(ctx->ngtcp2_ctx.qconn) {
      بنية cf_call_انکڑے save;
      CF_انکڑے_SAVE(save, cf, انکڑے);
      Curl_cf_ngtcp2_cmn_conn_close(cf, انکڑے);
      CF_انکڑے_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_h3_proxy_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *انکڑے, bool *پورا)
{
  أرجع Curl_cf_ngtcp2_cmn_shutdown(cf, انکڑے, پورا);
}

بنية Curl_cftype Curl_cft_h3_proxy = {
  "H3-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  cf_h3_proxy_destroy,
  cf_h3_proxy_connect,
  cf_h3_proxy_shutdown,
  cf_h3_proxy_adjust_pollset,
  cf_h3_proxy_انکڑے_pending,
  cf_h3_proxy_send,
  cf_h3_proxy_recv,
  Curl_cf_def_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_ngtcp2_cmn_query,
};

CURLcode Curl_cf_ngtcp2_proxy_create(بنية Curl_cfilter **pcf,
                                     بنية Curl_easy *انکڑے,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     uint8_t transport_peer,
                                     بنية connectdata *conn,
                                     بنية Curl_sockaddr_ex *addr,
                                     بنية Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_h3_proxy_ctx *ctx;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;

  إذا(!tunnel_peer)
    أرجع CURLE_FAILED_INIT;
  إذا((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    أرجع CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  نتیجہ = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
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
    وإلا إذا(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  وإلا
    CURL_TRC_CF(انکڑے, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  أرجع نتیجہ;
}

CURLcode Curl_cf_ngtcp2_proxy_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                           بنية Curl_easy *انکڑے,
                                           بنية Curl_peer *origin,
                                           بنية Curl_peer *peer,
                                           بنية Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_h3_proxy_ctx *ctx;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  (فراغ)انکڑے;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب باہرلا;
  نتیجہ = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);

باہرلا:
  إذا(نتیجہ) {
    إذا(cf)
      Curl_conn_cf_discard_chain(&cf, انکڑے);
    وإلا إذا(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  أرجع نتیجہ;
}

#جے_انت
