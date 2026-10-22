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
#تعریف_کړه PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#تعریف_کړه PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_بهیر_CHUNK_اندازه)

تعريفنوع تعداد {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

بنية h3_tunnel_بهیر {
  بنية Curl_peer *dest;  /* where the tunnel goes to */
  بنية http_resp *resp;
  بنية bufq recvbuf;
  حرف *authority;
  بنية h3_بهیر_ctx *بهیر;
  h3_tunnel_state state;
  بټ(udp);
  بټ(has_final_response);
  بټ(closed);
};

ساكن CURLcode h3_tunnel_بهیر_init(بنية h3_tunnel_بهیر *ts,
                                      بنية Curl_peer *dest,
                                      bool udp)
{
  ts->state = H3_TUNNEL_INIT;
  Curl_peer_link(&ts->dest, dest);
  Curl_bufq_init2(&ts->recvbuf, H3_بهیر_CHUNK_اندازه,
                  PROXY_H3_TUNNEL_RECV_CHUNKS, BUFQ_OPT_SOFT_حد);
  ts->udp = udp;
  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->پورټ);
  إذا(!ts->authority)
    أرجع CURLE_بهر_د_حافظه;

  أرجع CURLE_OK;
}

ساكن فراغ h3_tunnel_بهیر_reset(بنية h3_tunnel_بهیر *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = عدم;
  ts->بهیر = عدم;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

ساكن فراغ h3_tunnel_بهیر_cleanup(بنية h3_tunnel_بهیر *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

ساكن فراغ h3_tunnel_go_state(بنية Curl_cfilter *cf,
                               بنية h3_tunnel_بهیر *ts,
                               h3_tunnel_state new_state,
                               بنية Curl_easy *معلومات)
{
  VERBOSE(int64_t بهیر_id = ts->بهیر ? ts->بهیر->id : -1);
  (فراغ)cf;

  إذا(ts->state == new_state)
    أرجع;

  /* leaving this one */
  بدل(ts->state) {
  حالة H3_TUNNEL_CONNECT:
    معلومات->req.ignorebody = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }

  /* entering this one */
  بدل(new_state) {
  حالة H3_TUNNEL_INIT:
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] -> [init]", بهیر_id);
    h3_tunnel_بهیر_reset(ts);
    اكسر;
  حالة H3_TUNNEL_CONNECT:
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] -> [connect]", بهیر_id);
    ts->state = H3_TUNNEL_CONNECT;
    اكسر;
  حالة H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] -> [response]", بهیر_id);
    ts->state = H3_TUNNEL_RESPONSE;
    اكسر;
  حالة H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] -> [established]", بهیر_id);
    infof(معلومات, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    معلومات->state.authproxy.بشپړ = TRUE;
    معلومات->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(معلومات->req.hd_proxy_auth);
    اكسر;
  حالة H3_TUNNEL_FAILED:
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] -> [failed]", بهیر_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(معلومات->req.hd_proxy_auth);
    اكسر;
  }
}

بنية cf_h3_proxy_ctx {
  بنية cf_ngtcp2_ctx ngtcp2_ctx;
  بنية h3_tunnel_بهیر tunnel; /* our tunnel CONNECT stream */
  بټ(connected);
};

ساكن CURLcode cf_ngtcp2_proxy_h3_init(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        بنية cf_ngtcp2_ctx *ctx);

ساكن CURLcode cf_h3_proxy_ctx_init(بنية cf_h3_proxy_ctx *ctx,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     بنية ssl_filter_config *sslc,
                                     بنية Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode پایله;
  پایله = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  إذا(!پایله)
    پایله = h3_tunnel_بهیر_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  أرجع پایله;
}

ساكن فراغ cf_h3_proxy_ctx_free(بنية cf_h3_proxy_ctx *ctx)
{
  إذا(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_بهیر_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

ساكن صحيح cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t بهیر_id,
                                      uint64_t datalen, فراغ *user_معلومات,
                                      فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية h3_بهیر_ctx *بهیر;
  size_t skiplen;
  (فراغ)بهیر_user_معلومات;

  بهیر = pctx->tunnel.بهیر;
  إذا(!بهیر || (بهیر->id != بهیر_id))
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

ساكن صحيح cb_h3_proxy_بهیر_close(nghttp3_conn *conn, int64_t بهیر_id,
                                    uint64_t app_error_کوډ, فراغ *user_معلومات,
                                    فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  بنية h3_بهیر_ctx *بهیر;

  (فراغ)conn;
  (فراغ)بهیر_user_معلومات;
  إذا(!معلومات)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  بهیر = pctx->tunnel.بهیر;
  إذا(!بهیر || (بهیر->id != بهیر_id))
    أرجع 0;

  بهیر->closed = TRUE;
  بهیر->error3 = app_error_کوډ;
  إذا(بهیر->error3 != NGHTTP3_H3_NO_ERROR) {
    بهیر->reset = TRUE;
    بهیر->send_closed = TRUE;
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                بهیر->id, بهیر->error3);
  }
  وإلا
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] CLOSED", بهیر->id);
  pctx->tunnel.بهیر = عدم;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(معلومات);
  أرجع 0;
}

ساكن فراغ cf_h3_proxy_upd_rx_win(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   بنية h3_بهیر_ctx *بهیر)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  إذا(!بهیر) /* already closed */
    أرجع;
  إذا(بهیر->rx_دننه_واټن_max < بهیر->rx_دننه_واټن) {
    DEBUGASSERT(0);
    أرجع;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = بهیر->rx_دننه_واټن_max - بهیر->rx_دننه_واټن;
  إذا(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    إذا(UINT64_MAX - delta < بهیر->rx_دننه_واټن_max)
      delta = UINT64_MAX - بهیر->rx_دننه_واټن_max;
    إذا(delta) {
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  بهیر->id, cur_win, rx_buffered, delta);
      بهیر->rx_دننه_واټن_max += delta;
      ngtcp2_conn_extend_max_بهیر_دننه_واټن(ctx->qconn, بهیر->id, delta);
      ngtcp2_conn_extend_max_دننه_واټن(ctx->qconn, delta);
    }
  }
}

ساكن صحيح cb_h3_proxy_recv_معلومات(nghttp3_conn *conn, int64_t stream3_id,
                                 ثابت uint8_t *buf, size_t buflen,
                                 فراغ *user_معلومات, فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  بنية h3_بهیر_ctx *بهیر;
  size_t nwritten;
  CURLcode پایله = CURLE_OK;
  (فراغ)conn;
  (فراغ)stream3_id;
  (فراغ)بهیر_user_معلومات;

  بهیر = pctx->tunnel.بهیر;
  إذا(!معلومات || !بهیر || (بهیر->id != stream3_id)) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  بهیر->rx_ټول += (curl_off_t)buflen;
  CURL_TRC_CF(معلومات, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              بهیر->id, buflen, بهیر->rx_ټول);

  پایله = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  إذا(پایله || (nwritten < buflen)) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  بهیر->rx_دننه_واټن += buflen;
  إذا(بهیر->rx_دننه_واټن_max < بهیر->rx_دننه_واټن)
    بهیر->rx_دننه_واټن_max = بهیر->rx_دننه_واټن;

  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              بهیر->id, buflen, بهیر->rx_دننه_واټن_max - بهیر->rx_دننه_واټن);
  cf_h3_proxy_upd_rx_win(cf, معلومات, بهیر);

  Curl_multi_mark_dirty(معلومات);
  أرجع 0;
}

ساكن صحيح cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t بهیر_id,
                                        size_t consumed, فراغ *user_معلومات,
                                        فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (فراغ)conn;
  (فراغ)بهیر_user_معلومات;

  إذا(!ctx)
    أرجع 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_بهیر_دننه_واټن(ctx->qconn, بهیر_id, consumed);
  ngtcp2_conn_extend_max_دننه_واټن(ctx->qconn, consumed);

  أرجع 0;
}

ساكن صحيح cb_h3_proxy_recv_سرلیک(nghttp3_conn *conn, int64_t بهیر_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *ارزښت, uint8_t flags,
                                   فراغ *user_معلومات, فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(ارزښت);
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  بنية h3_بهیر_ctx *بهیر;
  CURLcode پایله = CURLE_OK;
  صحيح http_حالت;
  بنية http_resp *resp;
  (فراغ)conn;
  (فراغ)بهیر_id;
  (فراغ)token;
  (فراغ)flags;
  (فراغ)بهیر_user_معلومات;

  /* stream_user_data might be NULL for control streams */
  إذا(!معلومات)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  بهیر = pctx->tunnel.بهیر;
  إذا(!بهیر || (بهیر->id != بهیر_id)) {
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                بهیر_id, (فراغ *)معلومات, معلومات ? معلومات->mid : 0);
    أرجع 0;
  }

  إذا(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    أرجع 0;
  }

  إذا(token == NGHTTP3_QPACK_TOKEN_حالت) {
    پایله = Curl_http_decode_حالت(&بهیر->حالت_کوډ,
                                     (ثابت حرف *)h3val.بنسټ, h3val.len);
    إذا(پایله)
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    http_حالت = بهیر->حالت_کوډ;
    پایله = Curl_http_resp_make(&resp, http_حالت, عدم);
    إذا(پایله)
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
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] header: %.*s: %.*s", بهیر_id,
                (صحيح)h3name.len, h3name.بنسټ, (صحيح)h3val.len, h3val.بنسټ);
    پایله = Curl_dynhds_جمع_کړه(&pctx->tunnel.resp->سرلیکونه,
                             (ثابت حرف *)h3name.بنسټ, h3name.len,
                             (ثابت حرف *)h3val.بنسټ, h3val.len);
    إذا(پایله) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  أرجع 0;
}

ساكن صحيح cb_h3_proxy_پای_ټکی_سرلیکونه(nghttp3_conn *conn, int64_t بهیر_id,
                                   صحيح fin, فراغ *user_معلومات,
                                   فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  بنية h3_بهیر_ctx *بهیر;
  (فراغ)conn;
  (فراغ)بهیر_id;
  (فراغ)fin;
  (فراغ)بهیر_user_معلومات;

  إذا(!معلومات)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  بهیر = pctx->tunnel.بهیر;
  إذا(!بهیر || (بهیر->id != بهیر_id)) {
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                بهیر_id, (فراغ *)معلومات, معلومات ? معلومات->mid : 0);
    أرجع 0;
  }

  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] end_headers, status=%d", بهیر_id,
              بهیر->حالت_کوډ);

  إذا(!pctx->tunnel.has_final_response) {
    إذا(بهیر->حالت_کوډ / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  إذا(بهیر->حالت_کوډ / 100 != 1) {
    بهیر->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(معلومات);
  أرجع 0;
}

ساكن صحيح cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t بهیر_id,
                                    uint64_t app_error_کوډ, فراغ *user_معلومات,
                                    فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (فراغ)conn;
  (فراغ)بهیر_user_معلومات;

  إذا(ctx) {
    صحيح rv = ngtcp2_conn_shutdown_بهیر_read(ctx->qconn, 0, بهیر_id,
                                              app_error_کوډ);

    إذا(rv && rv != NGTCP2_ERR_بهیر_NOT_وموندل) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  أرجع 0;
}

ساكن صحيح cb_h3_proxy_reset_بهیر(nghttp3_conn *conn, int64_t بهیر_id,
                                    uint64_t app_error_کوډ, فراغ *user_معلومات,
                                    فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  صحيح rv;

  (فراغ)conn;
  (فراغ)بهیر_user_معلومات;
  إذا(!معلومات)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  إذا(!pctx->tunnel.بهیر ||
     (بهیر_id != pctx->tunnel.بهیر->id))
    أرجع 0;

  rv = ngtcp2_conn_shutdown_بهیر_write(ctx->qconn, 0, بهیر_id,
                                         app_error_کوډ);
  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] reset -> %d", بهیر_id, rv);
  pctx->tunnel.بهیر = عدم;
  pctx->tunnel.closed = TRUE;
  إذا(rv && rv != NGTCP2_ERR_بهیر_NOT_وموندل) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  أرجع 0;
}

ساكن nghttp3_ssize cb_h3_tunnel_read_معلومات(nghttp3_conn *conn,
                                            int64_t بهیر_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            فراغ *user_معلومات,
                                            فراغ *بهیر_user_معلومات)
{
  بنية Curl_cfilter *cf = user_معلومات;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  بنية h3_بهیر_ctx *بهیر;
  size_t nwritten = 0;
  size_t nvecs = 0;
  ثابت بینښه حرف *buf_بنسټ;

  (فراغ)conn;
  (فراغ)بهیر_id;
  (فراغ)veccnt;
  (فراغ)بهیر_user_معلومات;
  (فراغ)pflags;

  بهیر = pctx->tunnel.بهیر;
  إذا(!معلومات || !بهیر || (بهیر->id != بهیر_id))
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  إذا(بهیر->tx_دننه_flight_اندازه < Curl_bufq_len(&بهیر->sendbuf)) {
    nvecs = 0;
    طالما(nvecs < veccnt) {
      إذا(!Curl_bufq_peek_at(&بهیر->sendbuf,
                           بهیر->tx_دننه_flight_اندازه,
                           &buf_بنسټ,
                           &vec[nvecs].len))
        اكسر;
      vec[nvecs].بنسټ = (uint8_t *)(uintptr_t)buf_بنسټ;
      بهیر->tx_دننه_flight_اندازه += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  إذا(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] read req body -> AGAIN",
                بهیر->id);
    أرجع NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              بهیر->id, nvecs,
              *pflags == NGHTTP3_معلومات_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&بهیر->sendbuf));
  أرجع (nghttp3_ssize)nvecs;
}

#که_تعریف_شوی CURL_HAVE_DIAG
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Wmissing-field-initializers"
#د_که_پای
ساكن nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_بهیر_close,
  cb_h3_proxy_recv_معلومات,
  cb_h3_proxy_deferred_consume,
  عدم, /* begin_headers */
  cb_h3_proxy_recv_سرلیک,
  cb_h3_proxy_پای_ټکی_سرلیکونه,
  عدم, /* begin_trailers */
  cb_h3_proxy_recv_سرلیک,
  عدم, /* end_trailers */
  cb_h3_proxy_stop_sending,
  عدم, /* end_stream */
  cb_h3_proxy_reset_بهیر,
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

ساكن CURLcode cf_ngtcp2_proxy_h3_init(بنية Curl_cfilter *cf,
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
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  إذا(rc) {
    failf(معلومات, "error creating nghttp3 connection instance");
    أرجع CURLE_بهر_د_حافظه;
  }

  أرجع Curl_cf_ngtcp2_h3_init_ctrls(ctx, معلومات);
}

ساكن ssize_t cf_h3_proxy_recv_closed_بهیر(بنية Curl_cfilter *cf,
                                              بنية Curl_easy *معلومات,
                                              بنية h3_بهیر_ctx *بهیر,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  إذا(بهیر->reset) {
    إذا(بهیر->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(معلومات, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", بهیر->id);
      connclose(cf->conn);
      معلومات->state.refused_بهیر = TRUE;
      *err = CURLE_RECV_ERROR;
      اذهب بهر;
    }
    وإلا إذا(بهیر->resp_hds_complete && معلومات->req.no_body) {
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", بهیر->id, بهیر->error3,
                  Curl_vquic_h3_err_str(بهیر->error3));
      nread = 0;
      اذهب بهر;
    }
    failf(معلومات, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", بهیر->id, بهیر->error3,
          Curl_vquic_h3_err_str(بهیر->error3));
    *err = معلومات->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    اذهب بهر;
  }
  وإلا إذا(!بهیر->resp_hds_complete) {
    failf(معلومات,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          بهیر->id);
    *err = CURLE_HTTP3;
    اذهب بهر;
  }
  nread = 0;

بهر:
  أرجع nread;
}

ساكن CURLcode cf_h3_proxy_sendbuf_جمع_کړه(بنية Curl_easy *معلومات,
                                        بنية h3_بهیر_ctx *بهیر,
                                        ثابت uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode پایله;
  *pnwritten = 0;
  (فراغ)معلومات;

  پایله = Curl_bufq_write(&بهیر->sendbuf, buf, len, pnwritten);
  أرجع پایله;
}

ساكن CURLcode cf_h3_proxy_send(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 ثابت uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  بنية h3_بهیر_ctx *بهیر = عدم;
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

  إذا(pctx->tunnel.closed) {
    پایله = CURLE_SEND_ERROR;
    اذهب denied;
  }

  بهیر = pctx->tunnel.بهیر;
  إذا(!بهیر) {
    پایله = CURLE_SEND_ERROR;
    اذهب denied;
  }

  إذا(بهیر->closed) {
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
  وإلا {
    پایله = cf_h3_proxy_sendbuf_جمع_کړه(معلومات, بهیر, buf, len, pnwritten);
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

بهر:
  پایله = Curl_1st_fatal(پایله,
                          Curl_cf_ngtcp2_cmn_سېټ_expiry(cf, معلومات, &pktx));
denied:
  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              بهیر ? بهیر->id : -1, len, (صحيح)پایله, *pnwritten);
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

/* incoming data frames on the h3 stream */
ساكن CURLcode cf_h3_proxy_recv(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  بنية h3_بهیر_ctx *بهیر = pctx->tunnel.بهیر;
  بنية cf_call_معلومات save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode پایله = CURLE_OK;

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

  إذا(!Curl_bufq_is_تشه(&pctx->tunnel.recvbuf)) {
    پایله = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    إذا(پایله)
      اذهب بهر;
    cf_h3_proxy_upd_rx_win(cf, معلومات, pctx->tunnel.بهیر);
  }

  پایله = Curl_cf_ngtcp2_پرمختګ_ingress(cf, معلومات, &pktx);
  إذا(پایله)
    اذهب بهر;

  /* inbufq had nothing before, maybe after progressing ingress? */
  إذا(!*pnread && !Curl_bufq_is_تشه(&pctx->tunnel.recvbuf)) {
    پایله = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    إذا(پایله) {
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  بهیر->id, len, *pnread, (صحيح)پایله);
      اذهب بهر;
    }
    cf_h3_proxy_upd_rx_win(cf, معلومات, pctx->tunnel.بهیر);
  }

  إذا(*pnread) {
    Curl_multi_mark_dirty(معلومات);
  }
  وإلا {
    إذا(بهیر->xfer_پایله) {
      CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] xfer write failed", بهیر->id);
      Curl_cf_ngtcp2_h3_بهیر_close(cf, معلومات, بهیر);
      پایله = بهیر->xfer_پایله;
      اذهب بهر;
    }
    وإلا إذا(بهیر->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_بهیر(cf, معلومات, بهیر, &پایله);
      إذا(nread > 0)
        *pnread = (size_t)nread;
      اذهب بهر;
    }
    پایله = CURLE_AGAIN;
  }

بهر:
  پایله = Curl_1st_fatal(پایله,
                          Curl_cf_ngtcp2_پرمختګ_egress(cf, معلومات, &pktx));
  پایله = Curl_1st_fatal(پایله,
                          Curl_cf_ngtcp2_cmn_سېټ_expiry(cf, معلومات, &pktx));
denied:
  CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] cf_recv(len=%zu) -> %d, %zu",
              بهیر ? بهیر->id : -1, len, (صحيح)پایله, *pnread);
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

ساكن CURLcode cf_h3_proxy_submit(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   بنية h3_tunnel_بهیر *ts,
                                   بنية httpreq *req)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  بنية h3_بهیر_ctx *بهیر = عدم;
  بنية dynhds h2_سرلیکونه;
  nghttp3_nv *nva = عدم;
  size_t nheader;
  صحيح rc = 0;
  بینښه صحيح i;
  nghttp3_معلومات_reader reader;
  nghttp3_معلومات_reader *preader = عدم;
  CURLcode پایله;

  Curl_dynhds_init(&h2_سرلیکونه, 0, DYN_HTTP_REQUEST);
  پایله = Curl_http_req_منزل_h2(&h2_سرلیکونه, req, معلومات);
  إذا(پایله)
    اذهب بهر;

  پایله = Curl_cf_ngtcp2_h3_بهیر_تیاری(cf, معلومات, &بهیر);
  إذا(پایله)
    اذهب بهر;

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

  /* Open a bidirectional stream */
  {
    int64_t sid;
    صحيح rv;

    DEBUGASSERT(بهیر->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_بهیر(ctx->qconn, &sid, عدم);
    إذا(rv) {
      failf(معلومات, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }
    بهیر->id = sid;
    ts->بهیر = بهیر;
    ++ctx->کارول_شوی_bidi_streams;
    CURL_TRC_CF(معلومات, cf, "[%" PRId64 "] opened bidi stream", sid);
  }

  /* CONNECT-UDP request stream remains open for capsules, no fixed EOF. */
  بهیر->send_closed = 0;
  reader.read_معلومات = cb_h3_tunnel_read_معلومات;
  preader = &reader;

  rc = nghttp3_conn_submit_request(ctx->h3conn, بهیر->id,
                                   nva, nheader, preader, بهیر);

  إذا(rc) {
    بدل(rc) {
    حالة NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(معلومات, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", بهیر->id);
      اكسر;
    افتراضي:
      CURL_TRC_CF(معلومات, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  بهیر->id, rc, nghttp3_strerror(rc));
      اكسر;
    }
    پایله = CURLE_SEND_ERROR;
    اذهب بهر;
  }

  إذا(Curl_trc_is_verbose(معلومات)) {
    CURL_TRC_CF(معلومات, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", بهیر->id,
                Curl_bufref_ptr(&معلومات->state.url));
  }

بهر:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرلیکونه);
  أرجع پایله;
}

ساكن CURLcode cf_h3_proxy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *معلومات,
                                           بنية easy_pollset *ps)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode پایله = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, معلومات);

  إذا(!ctx->qconn || !pctx->tunnel.بهیر || (sock == CURL_SOCKET_BAD))
    أرجع CURLE_OK;

  Curl_pollset_check(معلومات, ps, sock, &want_recv, &want_send);

  إذا(want_recv || want_send || !Curl_bufq_is_تشه(&ctx->q.sendbuf)) {
    بنية h3_بهیر_ctx *بهیر = pctx->tunnel.بهیر;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_کیڼ(ctx->qconn) ||
                 !ngtcp2_conn_get_max_معلومات_کیڼ(ctx->qconn));
    s_exhaust = want_send && بهیر && بهیر->id >= 0 &&
                بهیر->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_تشه(&ctx->q.sendbuf);

    پایله = Curl_pollset_سېټ(معلومات, ps, sock, want_recv, want_send);
  }
  أرجع پایله;
}

ساكن bool cf_h3_proxy_معلومات_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *معلومات)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  إذا(!Curl_bufq_is_تشه(&pctx->tunnel.recvbuf))
    أرجع TRUE;
  أرجع cf->بل ?
    cf->بل->cft->has_معلومات_pending(cf->بل, معلومات) : FALSE;
}

ساكن CURLcode cf_h3_proxy_submit_CONNECT(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *معلومات,
                                           بنية h3_tunnel_بهیر *ts)
{
  بنية cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode پایله;
  بنية httpreq *req = عدم;

  پایله = Curl_http_proxy_create_tunnel_request(
    &req, cf, معلومات, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  إذا(!پایله)
    پایله = Curl_creader_سېټ_null(معلومات);
  إذا(!پایله)
    پایله = cf_h3_proxy_submit(cf, معلومات, ts, req);

  إذا(req)
    Curl_http_req_free(req);
  إذا(پایله)
    failf(معلومات, "Failed sending CONNECT to proxy");
  أرجع پایله;
}

ساكن CURLcode cf_h3_proxy_inspect_response(بنية Curl_cfilter *cf,
                                             بنية Curl_easy *معلومات,
                                             بنية h3_tunnel_بهیر *ts)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_پایله res;
  CURLcode پایله;

  پایله = Curl_http_proxy_inspect_tunnel_response(
      cf, معلومات, ts->resp, (bool)pctx->tunnel.udp, &res);
  إذا(پایله)
    أرجع پایله;
  بدل(res) {
  حالة PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, معلومات);
    اكسر;
  حالة PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, معلومات);
    پایله = CURLE_COULDNT_CONNECT;
    اكسر;
  حالة PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, معلومات);
    اكسر;
  }
  أرجع پایله;
}

ساكن CURLcode cf_h3_proxy_tunnel(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   بنية h3_tunnel_بهیر *ts,
                                   bool *pdone)
{
  بنية cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  افعل {
    بدل(ts->state) {
    حالة H3_TUNNEL_INIT:
      CURL_TRC_CF(معلومات, cf, "[0] CONNECT start for %s", ts->authority);
      پایله = cf_h3_proxy_submit_CONNECT(cf, معلومات, ts);
      إذا(پایله)
        اذهب بهر;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, معلومات);

      پایله = Curl_cf_ngtcp2_پرمختګ_egress(cf, معلومات, عدم);
      إذا(پایله)
        اذهب بهر;
      FALLTHROUGH();

    حالة H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      پایله = Curl_cf_ngtcp2_پرمختګ_ingress(cf, معلومات, عدم);
      إذا(پایله)
        اذهب بهر;
      پایله = Curl_cf_ngtcp2_پرمختګ_egress(cf, معلومات, عدم);
      إذا(پایله && پایله != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, معلومات);
        اذهب بهر;
      }

      إذا(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, معلومات);
      }
      وإلا {
        /* Not done yet, return and let multi interface call us again */
        پایله = CURLE_OK;
        اذهب بهر;
      }
      FALLTHROUGH();

    حالة H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      پایله = cf_h3_proxy_inspect_response(cf, معلومات, ts);
      إذا(پایله)
        اذهب بهر;
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

بهر:
  إذا((پایله && (پایله != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, معلومات);
  أرجع پایله;
}

ساكن CURLcode cf_h3_proxy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات, bool *بشپړ)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_call_معلومات save = { 0 };
  CURLcode پایله = CURLE_OK;
  بنية h3_tunnel_بهیر *ts = &pctx->tunnel;
  bool معلومات_saved = FALSE;

  پایله = Curl_cf_ngtcp2_cmn_connect(cf, معلومات, بشپړ);
  إذا(پایله || !*بشپړ)
    اذهب بهر;

  CF_معلومات_SAVE(save, cf, معلومات);
  معلومات_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  پایله = cf_h3_proxy_tunnel(cf, معلومات, ts, بشپړ);

بهر:
  إذا(*بشپړ) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&معلومات->req, معلومات);
    Curl_client_reset(معلومات);
  }

  إذا(معلومات_saved)
    CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

ساكن فراغ cf_h3_proxy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات)
{
  بنية cf_h3_proxy_ctx *ctx = cf->ctx;

  (فراغ)معلومات;
  إذا(ctx) {
    CURL_TRC_CF(معلومات, cf, "cf_h3_proxy_destroy()");
    إذا(ctx->ngtcp2_ctx.qconn) {
      بنية cf_call_معلومات save;
      CF_معلومات_SAVE(save, cf, معلومات);
      Curl_cf_ngtcp2_cmn_conn_close(cf, معلومات);
      CF_معلومات_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_h3_proxy_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات, bool *بشپړ)
{
  أرجع Curl_cf_ngtcp2_cmn_shutdown(cf, معلومات, بشپړ);
}

بنية Curl_cftype Curl_cft_h3_proxy = {
  "H3-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  cf_h3_proxy_destroy,
  cf_h3_proxy_connect,
  cf_h3_proxy_shutdown,
  cf_h3_proxy_adjust_pollset,
  cf_h3_proxy_معلومات_pending,
  cf_h3_proxy_send,
  cf_h3_proxy_recv,
  Curl_cf_def_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_ngtcp2_cmn_query,
};

CURLcode Curl_cf_ngtcp2_proxy_create(بنية Curl_cfilter **pcf,
                                     بنية Curl_easy *معلومات,
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
  CURLcode پایله = CURLE_بهر_د_حافظه;

  إذا(!tunnel_peer)
    أرجع CURLE_FAILED_INIT;
  إذا((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    أرجع CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }
  پایله = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  إذا(پایله)
    اذهب بهر;

  پایله = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
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
    وإلا إذا(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  وإلا
    CURL_TRC_CF(معلومات, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  أرجع پایله;
}

CURLcode Curl_cf_ngtcp2_proxy_ورننباسه_after(بنية Curl_cfilter *cf_at,
                                           بنية Curl_easy *معلومات,
                                           بنية Curl_peer *origin,
                                           بنية Curl_peer *peer,
                                           بنية Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_h3_proxy_ctx *ctx;
  CURLcode پایله = CURLE_بهر_د_حافظه;
  (فراغ)معلومات;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب بهر;
  پایله = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  إذا(پایله)
    اذهب بهر;

  پایله = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  إذا(پایله)
    اذهب بهر;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_ورننباسه_after(cf_at, cf);

بهر:
  إذا(پایله) {
    إذا(cf)
      Curl_conn_cf_discard_chain(&cf, معلومات);
    وإلا إذا(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  أرجع پایله;
}

#د_که_پای
