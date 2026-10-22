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
#تعریف_کن PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#تعریف_کن PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_جریان_CHUNK_اندازه)

تعريفنوع تعداد {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

بنية h3_tunnel_جریان {
  بنية Curl_peer *dest;  /* where the tunnel goes to */
  بنية http_resp *resp;
  بنية bufq recvbuf;
  حرف *authority;
  بنية h3_جریان_ctx *جریان;
  h3_tunnel_state state;
  بیت(udp);
  بیت(has_final_response);
  بیت(closed);
};

ساكن CURLcode h3_tunnel_جریان_init(بنية h3_tunnel_جریان *ts,
                                      بنية Curl_peer *dest,
                                      bool udp)
{
  ts->state = H3_TUNNEL_INIT;
  Curl_peer_link(&ts->dest, dest);
  Curl_bufq_init2(&ts->recvbuf, H3_جریان_CHUNK_اندازه,
                  PROXY_H3_TUNNEL_RECV_CHUNKS, BUFQ_OPT_SOFT_حد);
  ts->udp = udp;
  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->پورت);
  إذا(!ts->authority)
    أرجع CURLE_بیرون_از_حافظه;

  أرجع CURLE_OK;
}

ساكن فراغ h3_tunnel_جریان_reset(بنية h3_tunnel_جریان *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = عدم;
  ts->جریان = عدم;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

ساكن فراغ h3_tunnel_جریان_cleanup(بنية h3_tunnel_جریان *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

ساكن فراغ h3_tunnel_go_state(بنية Curl_cfilter *cf,
                               بنية h3_tunnel_جریان *ts,
                               h3_tunnel_state new_state,
                               بنية Curl_easy *داده)
{
  VERBOSE(int64_t جریان_id = ts->جریان ? ts->جریان->id : -1);
  (فراغ)cf;

  إذا(ts->state == new_state)
    أرجع;

  /* leaving this one */
  بدل(ts->state) {
  حالة H3_TUNNEL_CONNECT:
    داده->req.ignorebody = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }

  /* entering this one */
  بدل(new_state) {
  حالة H3_TUNNEL_INIT:
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] -> [init]", جریان_id);
    h3_tunnel_جریان_reset(ts);
    اكسر;
  حالة H3_TUNNEL_CONNECT:
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] -> [connect]", جریان_id);
    ts->state = H3_TUNNEL_CONNECT;
    اكسر;
  حالة H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] -> [response]", جریان_id);
    ts->state = H3_TUNNEL_RESPONSE;
    اكسر;
  حالة H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] -> [established]", جریان_id);
    infof(داده, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    داده->state.authproxy.انجامشده = TRUE;
    داده->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(داده->req.hd_proxy_auth);
    اكسر;
  حالة H3_TUNNEL_FAILED:
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] -> [failed]", جریان_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(داده->req.hd_proxy_auth);
    اكسر;
  }
}

بنية cf_h3_proxy_ctx {
  بنية cf_ngtcp2_ctx ngtcp2_ctx;
  بنية h3_tunnel_جریان tunnel; /* our tunnel CONNECT stream */
  بیت(connected);
};

ساكن CURLcode cf_ngtcp2_proxy_h3_init(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *داده,
                                        بنية cf_ngtcp2_ctx *ctx);

ساكن CURLcode cf_h3_proxy_ctx_init(بنية cf_h3_proxy_ctx *ctx,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     بنية ssl_filter_config *sslc,
                                     بنية Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode نتیجه;
  نتیجه = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  إذا(!نتیجه)
    نتیجه = h3_tunnel_جریان_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  أرجع نتیجه;
}

ساكن فراغ cf_h3_proxy_ctx_free(بنية cf_h3_proxy_ctx *ctx)
{
  إذا(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_جریان_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

ساكن صحيح cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t جریان_id,
                                      uint64_t datalen, فراغ *user_داده,
                                      فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية h3_جریان_ctx *جریان;
  size_t skiplen;
  (فراغ)جریان_user_داده;

  جریان = pctx->tunnel.جریان;
  إذا(!جریان || (جریان->id != جریان_id))
    أرجع 0;

  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  إذا(datalen >= (uint64_t)جریان->tx_درون_flight_اندازه)
    skiplen = جریان->tx_درون_flight_اندازه;
  وإلا
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&جریان->sendbuf, skiplen);
  جریان->tx_درون_flight_اندازه -= skiplen;

  /* Resume upload processing if we have more data to send */
  إذا(جریان->tx_درون_flight_اندازه < Curl_bufq_len(&جریان->sendbuf)) {
    صحيح rv = nghttp3_conn_resume_جریان(conn, جریان_id);
    إذا(rv && rv != NGHTTP3_ERR_جریان_NOT_یافت) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  أرجع 0;
}

ساكن صحيح cb_h3_proxy_جریان_close(nghttp3_conn *conn, int64_t جریان_id,
                                    uint64_t app_error_کد, فراغ *user_داده,
                                    فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  بنية h3_جریان_ctx *جریان;

  (فراغ)conn;
  (فراغ)جریان_user_داده;
  إذا(!داده)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  جریان = pctx->tunnel.جریان;
  إذا(!جریان || (جریان->id != جریان_id))
    أرجع 0;

  جریان->closed = TRUE;
  جریان->error3 = app_error_کد;
  إذا(جریان->error3 != NGHTTP3_H3_NO_ERROR) {
    جریان->reset = TRUE;
    جریان->send_closed = TRUE;
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                جریان->id, جریان->error3);
  }
  وإلا
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] CLOSED", جریان->id);
  pctx->tunnel.جریان = عدم;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(داده);
  أرجع 0;
}

ساكن فراغ cf_h3_proxy_upd_rx_win(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   بنية h3_جریان_ctx *جریان)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  إذا(!جریان) /* already closed */
    أرجع;
  إذا(جریان->rx_جابه_جایی_max < جریان->rx_جابه_جایی) {
    DEBUGASSERT(0);
    أرجع;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = جریان->rx_جابه_جایی_max - جریان->rx_جابه_جایی;
  إذا(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    إذا(UINT64_MAX - delta < جریان->rx_جابه_جایی_max)
      delta = UINT64_MAX - جریان->rx_جابه_جایی_max;
    إذا(delta) {
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  جریان->id, cur_win, rx_buffered, delta);
      جریان->rx_جابه_جایی_max += delta;
      ngtcp2_conn_extend_max_جریان_جابه_جایی(ctx->qconn, جریان->id, delta);
      ngtcp2_conn_extend_max_جابه_جایی(ctx->qconn, delta);
    }
  }
}

ساكن صحيح cb_h3_proxy_recv_داده(nghttp3_conn *conn, int64_t stream3_id,
                                 ثابت uint8_t *buf, size_t buflen,
                                 فراغ *user_داده, فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  بنية h3_جریان_ctx *جریان;
  size_t nwritten;
  CURLcode نتیجه = CURLE_OK;
  (فراغ)conn;
  (فراغ)stream3_id;
  (فراغ)جریان_user_داده;

  جریان = pctx->tunnel.جریان;
  إذا(!داده || !جریان || (جریان->id != stream3_id)) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  جریان->rx_مجموع += (curl_off_t)buflen;
  CURL_TRC_CF(داده, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              جریان->id, buflen, جریان->rx_مجموع);

  نتیجه = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  إذا(نتیجه || (nwritten < buflen)) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  جریان->rx_جابه_جایی += buflen;
  إذا(جریان->rx_جابه_جایی_max < جریان->rx_جابه_جایی)
    جریان->rx_جابه_جایی_max = جریان->rx_جابه_جایی;

  CURL_TRC_CF(داده, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              جریان->id, buflen, جریان->rx_جابه_جایی_max - جریان->rx_جابه_جایی);
  cf_h3_proxy_upd_rx_win(cf, داده, جریان);

  Curl_multi_mark_dirty(داده);
  أرجع 0;
}

ساكن صحيح cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t جریان_id,
                                        size_t consumed, فراغ *user_داده,
                                        فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (فراغ)conn;
  (فراغ)جریان_user_داده;

  إذا(!ctx)
    أرجع 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_جریان_جابه_جایی(ctx->qconn, جریان_id, consumed);
  ngtcp2_conn_extend_max_جابه_جایی(ctx->qconn, consumed);

  أرجع 0;
}

ساكن صحيح cb_h3_proxy_recv_سرآیند(nghttp3_conn *conn, int64_t جریان_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *مقدار, uint8_t flags,
                                   فراغ *user_داده, فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(مقدار);
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  بنية h3_جریان_ctx *جریان;
  CURLcode نتیجه = CURLE_OK;
  صحيح http_وضعیت;
  بنية http_resp *resp;
  (فراغ)conn;
  (فراغ)جریان_id;
  (فراغ)token;
  (فراغ)flags;
  (فراغ)جریان_user_داده;

  /* stream_user_data might be NULL for control streams */
  إذا(!داده)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  جریان = pctx->tunnel.جریان;
  إذا(!جریان || (جریان->id != جریان_id)) {
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                جریان_id, (فراغ *)داده, داده ? داده->mid : 0);
    أرجع 0;
  }

  إذا(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    أرجع 0;
  }

  إذا(token == NGHTTP3_QPACK_TOKEN_وضعیت) {
    نتیجه = Curl_http_decode_وضعیت(&جریان->وضعیت_کد,
                                     (ثابت حرف *)h3val.پایه, h3val.len);
    إذا(نتیجه)
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    http_وضعیت = جریان->وضعیت_کد;
    نتیجه = Curl_http_resp_make(&resp, http_وضعیت, عدم);
    إذا(نتیجه)
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
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] header: %.*s: %.*s", جریان_id,
                (صحيح)h3name.len, h3name.پایه, (صحيح)h3val.len, h3val.پایه);
    نتیجه = Curl_dynhds_جمع_کن(&pctx->tunnel.resp->سرآیندها,
                             (ثابت حرف *)h3name.پایه, h3name.len,
                             (ثابت حرف *)h3val.پایه, h3val.len);
    إذا(نتیجه) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  أرجع 0;
}

ساكن صحيح cb_h3_proxy_انتها_سرآیندها(nghttp3_conn *conn, int64_t جریان_id,
                                   صحيح fin, فراغ *user_داده,
                                   فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  بنية h3_جریان_ctx *جریان;
  (فراغ)conn;
  (فراغ)جریان_id;
  (فراغ)fin;
  (فراغ)جریان_user_داده;

  إذا(!داده)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  جریان = pctx->tunnel.جریان;
  إذا(!جریان || (جریان->id != جریان_id)) {
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                جریان_id, (فراغ *)داده, داده ? داده->mid : 0);
    أرجع 0;
  }

  CURL_TRC_CF(داده, cf, "[%" PRId64 "] end_headers, status=%d", جریان_id,
              جریان->وضعیت_کد);

  إذا(!pctx->tunnel.has_final_response) {
    إذا(جریان->وضعیت_کد / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  إذا(جریان->وضعیت_کد / 100 != 1) {
    جریان->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(داده);
  أرجع 0;
}

ساكن صحيح cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t جریان_id,
                                    uint64_t app_error_کد, فراغ *user_داده,
                                    فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (فراغ)conn;
  (فراغ)جریان_user_داده;

  إذا(ctx) {
    صحيح rv = ngtcp2_conn_shutdown_جریان_read(ctx->qconn, 0, جریان_id,
                                              app_error_کد);

    إذا(rv && rv != NGTCP2_ERR_جریان_NOT_یافت) {
      أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  أرجع 0;
}

ساكن صحيح cb_h3_proxy_reset_جریان(nghttp3_conn *conn, int64_t جریان_id,
                                    uint64_t app_error_کد, فراغ *user_داده,
                                    فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  صحيح rv;

  (فراغ)conn;
  (فراغ)جریان_user_داده;
  إذا(!داده)
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  إذا(!pctx->tunnel.جریان ||
     (جریان_id != pctx->tunnel.جریان->id))
    أرجع 0;

  rv = ngtcp2_conn_shutdown_جریان_write(ctx->qconn, 0, جریان_id,
                                         app_error_کد);
  CURL_TRC_CF(داده, cf, "[%" PRId64 "] reset -> %d", جریان_id, rv);
  pctx->tunnel.جریان = عدم;
  pctx->tunnel.closed = TRUE;
  إذا(rv && rv != NGTCP2_ERR_جریان_NOT_یافت) {
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  أرجع 0;
}

ساكن nghttp3_ssize cb_h3_tunnel_read_داده(nghttp3_conn *conn,
                                            int64_t جریان_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            فراغ *user_داده,
                                            فراغ *جریان_user_داده)
{
  بنية Curl_cfilter *cf = user_داده;
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  بنية h3_جریان_ctx *جریان;
  size_t nwritten = 0;
  size_t nvecs = 0;
  ثابت بیعلامت حرف *buf_پایه;

  (فراغ)conn;
  (فراغ)جریان_id;
  (فراغ)veccnt;
  (فراغ)جریان_user_داده;
  (فراغ)pflags;

  جریان = pctx->tunnel.جریان;
  إذا(!داده || !جریان || (جریان->id != جریان_id))
    أرجع NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  إذا(جریان->tx_درون_flight_اندازه < Curl_bufq_len(&جریان->sendbuf)) {
    nvecs = 0;
    طالما(nvecs < veccnt) {
      إذا(!Curl_bufq_peek_at(&جریان->sendbuf,
                           جریان->tx_درون_flight_اندازه,
                           &buf_پایه,
                           &vec[nvecs].len))
        اكسر;
      vec[nvecs].پایه = (uint8_t *)(uintptr_t)buf_پایه;
      جریان->tx_درون_flight_اندازه += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  إذا(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] read req body -> AGAIN",
                جریان->id);
    أرجع NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(داده, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              جریان->id, nvecs,
              *pflags == NGHTTP3_داده_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&جریان->sendbuf));
  أرجع (nghttp3_ssize)nvecs;
}

#اگر_تعریف_شده CURL_HAVE_DIAG
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Wmissing-field-initializers"
#پایان_اگر
ساكن nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_جریان_close,
  cb_h3_proxy_recv_داده,
  cb_h3_proxy_deferred_consume,
  عدم, /* begin_headers */
  cb_h3_proxy_recv_سرآیند,
  cb_h3_proxy_انتها_سرآیندها,
  عدم, /* begin_trailers */
  cb_h3_proxy_recv_سرآیند,
  عدم, /* end_trailers */
  cb_h3_proxy_stop_sending,
  عدم, /* end_stream */
  cb_h3_proxy_reset_جریان,
  عدم, /* shutdown */
  عدم, /* recv_settings (deprecated) */
#اگر_تعریف_شده NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  عدم, /* recv_origin */
  عدم, /* end_origin */
  عدم, /* rand */
#پایان_اگر
#اگر_تعریف_شده NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  عدم, /* recv_settings2 */
#پایان_اگر
#اگر_تعریف_شده NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  عدم, /* stream_close2 */
#پایان_اگر
};
#اگر_تعریف_شده CURL_HAVE_DIAG
#رهنمود GCC diagnostic برداشت
#پایان_اگر

ساكن CURLcode cf_ngtcp2_proxy_h3_init(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *داده,
                                        بنية cf_ngtcp2_ctx *ctx)
{
  صحيح rc;

  إذا(ngtcp2_conn_get_streams_uni_چپ(ctx->qconn) < 3) {
    failf(داده, "QUIC connection lacks 3 uni streams to run HTTP/3");
    أرجع CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  إذا(rc) {
    failf(داده, "error creating nghttp3 connection instance");
    أرجع CURLE_بیرون_از_حافظه;
  }

  أرجع Curl_cf_ngtcp2_h3_init_ctrls(ctx, داده);
}

ساكن ssize_t cf_h3_proxy_recv_closed_جریان(بنية Curl_cfilter *cf,
                                              بنية Curl_easy *داده,
                                              بنية h3_جریان_ctx *جریان,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  إذا(جریان->reset) {
    إذا(جریان->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(داده, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", جریان->id);
      connclose(cf->conn);
      داده->state.refused_جریان = TRUE;
      *err = CURLE_RECV_ERROR;
      اذهب بیرون;
    }
    وإلا إذا(جریان->resp_hds_complete && داده->req.no_body) {
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", جریان->id, جریان->error3,
                  Curl_vquic_h3_err_str(جریان->error3));
      nread = 0;
      اذهب بیرون;
    }
    failf(داده, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", جریان->id, جریان->error3,
          Curl_vquic_h3_err_str(جریان->error3));
    *err = داده->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    اذهب بیرون;
  }
  وإلا إذا(!جریان->resp_hds_complete) {
    failf(داده,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          جریان->id);
    *err = CURLE_HTTP3;
    اذهب بیرون;
  }
  nread = 0;

بیرون:
  أرجع nread;
}

ساكن CURLcode cf_h3_proxy_sendbuf_جمع_کن(بنية Curl_easy *داده,
                                        بنية h3_جریان_ctx *جریان,
                                        ثابت uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode نتیجه;
  *pnwritten = 0;
  (فراغ)داده;

  نتیجه = Curl_bufq_write(&جریان->sendbuf, buf, len, pnwritten);
  أرجع نتیجه;
}

ساكن CURLcode cf_h3_proxy_send(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 ثابت uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  بنية h3_جریان_ctx *جریان = عدم;
  بنية cf_call_داده save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode نتیجه = CURLE_OK;

  CF_داده_SAVE(save, cf, داده);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, داده);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  إذا(ctx->tls_vrfy_نتیجه) {
    نتیجه = ctx->tls_vrfy_نتیجه;
    اذهب denied;
  }

  (فراغ)eos; /* use for stream EOF and block handling */
  نتیجه = Curl_cf_ngtcp2_پیشرفت_ingress(cf, داده, &pktx);
  إذا(نتیجه)
    اذهب بیرون;

  إذا(pctx->tunnel.closed) {
    نتیجه = CURLE_SEND_ERROR;
    اذهب denied;
  }

  جریان = pctx->tunnel.جریان;
  إذا(!جریان) {
    نتیجه = CURLE_SEND_ERROR;
    اذهب denied;
  }

  إذا(جریان->closed) {
    إذا(جریان->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", جریان->id);
      نتیجه = CURLE_OK;
      *pnwritten = len;
      اذهب بیرون;
    }
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", جریان->id, len);
    نتیجه = CURLE_HTTP3;
    اذهب بیرون;
  }
  وإلا {
    نتیجه = cf_h3_proxy_sendbuf_جمع_کن(داده, جریان, buf, len, pnwritten);
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                جریان->id, len, (صحيح)نتیجه, *pnwritten);
    إذا(نتیجه)
      اذهب بیرون;
    (فراغ)nghttp3_conn_resume_جریان(ctx->h3conn, جریان->id);
  }

  إذا(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!نتیجه);
  نتیجه = Curl_cf_ngtcp2_پیشرفت_egress(cf, داده, &pktx);

بیرون:
  نتیجه = Curl_1st_fatal(نتیجه,
                          Curl_cf_ngtcp2_cmn_مجموعه_expiry(cf, داده, &pktx));
denied:
  CURL_TRC_CF(داده, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              جریان ? جریان->id : -1, len, (صحيح)نتیجه, *pnwritten);
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

/* incoming data frames on the h3 stream */
ساكن CURLcode cf_h3_proxy_recv(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  بنية h3_جریان_ctx *جریان = pctx->tunnel.جریان;
  بنية cf_call_داده save;
  بنية cf_ngtcp2_io_ctx pktx;
  CURLcode نتیجه = CURLE_OK;

  CF_داده_SAVE(save, cf, داده);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  إذا(ctx->tls_vrfy_نتیجه) {
    نتیجه = ctx->tls_vrfy_نتیجه;
    اذهب denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, داده);

  إذا(!جریان || ctx->shutdown_started) {
    نتیجه = CURLE_RECV_ERROR;
    اذهب بیرون;
  }

  إذا(!Curl_bufq_is_خالی(&pctx->tunnel.recvbuf)) {
    نتیجه = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    إذا(نتیجه)
      اذهب بیرون;
    cf_h3_proxy_upd_rx_win(cf, داده, pctx->tunnel.جریان);
  }

  نتیجه = Curl_cf_ngtcp2_پیشرفت_ingress(cf, داده, &pktx);
  إذا(نتیجه)
    اذهب بیرون;

  /* inbufq had nothing before, maybe after progressing ingress? */
  إذا(!*pnread && !Curl_bufq_is_خالی(&pctx->tunnel.recvbuf)) {
    نتیجه = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    إذا(نتیجه) {
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  جریان->id, len, *pnread, (صحيح)نتیجه);
      اذهب بیرون;
    }
    cf_h3_proxy_upd_rx_win(cf, داده, pctx->tunnel.جریان);
  }

  إذا(*pnread) {
    Curl_multi_mark_dirty(داده);
  }
  وإلا {
    إذا(جریان->xfer_نتیجه) {
      CURL_TRC_CF(داده, cf, "[%" PRId64 "] xfer write failed", جریان->id);
      Curl_cf_ngtcp2_h3_جریان_close(cf, داده, جریان);
      نتیجه = جریان->xfer_نتیجه;
      اذهب بیرون;
    }
    وإلا إذا(جریان->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_جریان(cf, داده, جریان, &نتیجه);
      إذا(nread > 0)
        *pnread = (size_t)nread;
      اذهب بیرون;
    }
    نتیجه = CURLE_AGAIN;
  }

بیرون:
  نتیجه = Curl_1st_fatal(نتیجه,
                          Curl_cf_ngtcp2_پیشرفت_egress(cf, داده, &pktx));
  نتیجه = Curl_1st_fatal(نتیجه,
                          Curl_cf_ngtcp2_cmn_مجموعه_expiry(cf, داده, &pktx));
denied:
  CURL_TRC_CF(داده, cf, "[%" PRId64 "] cf_recv(len=%zu) -> %d, %zu",
              جریان ? جریان->id : -1, len, (صحيح)نتیجه, *pnread);
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن CURLcode cf_h3_proxy_submit(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   بنية h3_tunnel_جریان *ts,
                                   بنية httpreq *req)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  بنية h3_جریان_ctx *جریان = عدم;
  بنية dynhds h2_سرآیندها;
  nghttp3_nv *nva = عدم;
  size_t nheader;
  صحيح rc = 0;
  بیعلامت صحيح i;
  nghttp3_داده_reader reader;
  nghttp3_داده_reader *preader = عدم;
  CURLcode نتیجه;

  Curl_dynhds_init(&h2_سرآیندها, 0, DYN_HTTP_REQUEST);
  نتیجه = Curl_http_req_مقصد_h2(&h2_سرآیندها, req, داده);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = Curl_cf_ngtcp2_h3_جریان_راهاندازی(cf, داده, &جریان);
  إذا(نتیجه)
    اذهب بیرون;

  nheader = Curl_dynhds_شمار(&h2_سرآیندها);
  nva = curlx_malloc(حجم(nghttp3_nv) * nheader);
  إذا(!nva) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  لكل(i = 0; i < nheader; ++i) {
    بنية dynhds_مدخل *e = Curl_dynhds_getn(&h2_سرآیندها, i);
    nva[i].name = (بیعلامت حرف *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].مقدار = (بیعلامت حرف *)e->مقدار;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  /* Open a bidirectional stream */
  {
    int64_t sid;
    صحيح rv;

    DEBUGASSERT(جریان->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_جریان(ctx->qconn, &sid, عدم);
    إذا(rv) {
      failf(داده, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
    جریان->id = sid;
    ts->جریان = جریان;
    ++ctx->استفاده_شده_bidi_streams;
    CURL_TRC_CF(داده, cf, "[%" PRId64 "] opened bidi stream", sid);
  }

  /* CONNECT-UDP request stream remains open for capsules, no fixed EOF. */
  جریان->send_closed = 0;
  reader.read_داده = cb_h3_tunnel_read_داده;
  preader = &reader;

  rc = nghttp3_conn_submit_request(ctx->h3conn, جریان->id,
                                   nva, nheader, preader, جریان);

  إذا(rc) {
    بدل(rc) {
    حالة NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(داده, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", جریان->id);
      اكسر;
    افتراضي:
      CURL_TRC_CF(داده, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  جریان->id, rc, nghttp3_strerror(rc));
      اكسر;
    }
    نتیجه = CURLE_SEND_ERROR;
    اذهب بیرون;
  }

  إذا(Curl_trc_is_verbose(داده)) {
    CURL_TRC_CF(داده, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", جریان->id,
                Curl_bufref_ptr(&داده->state.url));
  }

بیرون:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرآیندها);
  أرجع نتیجه;
}

ساكن CURLcode cf_h3_proxy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *داده,
                                           بنية easy_pollset *ps)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode نتیجه = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, داده);

  إذا(!ctx->qconn || !pctx->tunnel.جریان || (sock == CURL_SOCKET_BAD))
    أرجع CURLE_OK;

  Curl_pollset_check(داده, ps, sock, &want_recv, &want_send);

  إذا(want_recv || want_send || !Curl_bufq_is_خالی(&ctx->q.sendbuf)) {
    بنية h3_جریان_ctx *جریان = pctx->tunnel.جریان;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_چپ(ctx->qconn) ||
                 !ngtcp2_conn_get_max_داده_چپ(ctx->qconn));
    s_exhaust = want_send && جریان && جریان->id >= 0 &&
                جریان->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_خالی(&ctx->q.sendbuf);

    نتیجه = Curl_pollset_مجموعه(داده, ps, sock, want_recv, want_send);
  }
  أرجع نتیجه;
}

ساكن bool cf_h3_proxy_داده_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *داده)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  إذا(!Curl_bufq_is_خالی(&pctx->tunnel.recvbuf))
    أرجع TRUE;
  أرجع cf->بعدی ?
    cf->بعدی->cft->has_داده_pending(cf->بعدی, داده) : FALSE;
}

ساكن CURLcode cf_h3_proxy_submit_CONNECT(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *داده,
                                           بنية h3_tunnel_جریان *ts)
{
  بنية cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجه;
  بنية httpreq *req = عدم;

  نتیجه = Curl_http_proxy_create_tunnel_request(
    &req, cf, داده, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  إذا(!نتیجه)
    نتیجه = Curl_creader_مجموعه_null(داده);
  إذا(!نتیجه)
    نتیجه = cf_h3_proxy_submit(cf, داده, ts, req);

  إذا(req)
    Curl_http_req_free(req);
  إذا(نتیجه)
    failf(داده, "Failed sending CONNECT to proxy");
  أرجع نتیجه;
}

ساكن CURLcode cf_h3_proxy_inspect_response(بنية Curl_cfilter *cf,
                                             بنية Curl_easy *داده,
                                             بنية h3_tunnel_جریان *ts)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_نتیجه res;
  CURLcode نتیجه;

  نتیجه = Curl_http_proxy_inspect_tunnel_response(
      cf, داده, ts->resp, (bool)pctx->tunnel.udp, &res);
  إذا(نتیجه)
    أرجع نتیجه;
  بدل(res) {
  حالة PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, داده);
    اكسر;
  حالة PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, داده);
    نتیجه = CURLE_COULDNT_CONNECT;
    اكسر;
  حالة PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, داده);
    اكسر;
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_h3_proxy_tunnel(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   بنية h3_tunnel_جریان *ts,
                                   bool *pdone)
{
  بنية cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  افعل {
    بدل(ts->state) {
    حالة H3_TUNNEL_INIT:
      CURL_TRC_CF(داده, cf, "[0] CONNECT start for %s", ts->authority);
      نتیجه = cf_h3_proxy_submit_CONNECT(cf, داده, ts);
      إذا(نتیجه)
        اذهب بیرون;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, داده);

      نتیجه = Curl_cf_ngtcp2_پیشرفت_egress(cf, داده, عدم);
      إذا(نتیجه)
        اذهب بیرون;
      FALLTHROUGH();

    حالة H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      نتیجه = Curl_cf_ngtcp2_پیشرفت_ingress(cf, داده, عدم);
      إذا(نتیجه)
        اذهب بیرون;
      نتیجه = Curl_cf_ngtcp2_پیشرفت_egress(cf, داده, عدم);
      إذا(نتیجه && نتیجه != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, داده);
        اذهب بیرون;
      }

      إذا(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, داده);
      }
      وإلا {
        /* Not done yet, return and let multi interface call us again */
        نتیجه = CURLE_OK;
        اذهب بیرون;
      }
      FALLTHROUGH();

    حالة H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      نتیجه = cf_h3_proxy_inspect_response(cf, داده, ts);
      إذا(نتیجه)
        اذهب بیرون;
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

بیرون:
  إذا((نتیجه && (نتیجه != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, داده);
  أرجع نتیجه;
}

ساكن CURLcode cf_h3_proxy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده, bool *انجامشده)
{
  بنية cf_h3_proxy_ctx *pctx = cf->ctx;
  بنية cf_call_داده save = { 0 };
  CURLcode نتیجه = CURLE_OK;
  بنية h3_tunnel_جریان *ts = &pctx->tunnel;
  bool داده_saved = FALSE;

  نتیجه = Curl_cf_ngtcp2_cmn_connect(cf, داده, انجامشده);
  إذا(نتیجه || !*انجامشده)
    اذهب بیرون;

  CF_داده_SAVE(save, cf, داده);
  داده_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  نتیجه = cf_h3_proxy_tunnel(cf, داده, ts, انجامشده);

بیرون:
  إذا(*انجامشده) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&داده->req, داده);
    Curl_client_reset(داده);
  }

  إذا(داده_saved)
    CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن فراغ cf_h3_proxy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده)
{
  بنية cf_h3_proxy_ctx *ctx = cf->ctx;

  (فراغ)داده;
  إذا(ctx) {
    CURL_TRC_CF(داده, cf, "cf_h3_proxy_destroy()");
    إذا(ctx->ngtcp2_ctx.qconn) {
      بنية cf_call_داده save;
      CF_داده_SAVE(save, cf, داده);
      Curl_cf_ngtcp2_cmn_conn_close(cf, داده);
      CF_داده_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_h3_proxy_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *داده, bool *انجامشده)
{
  أرجع Curl_cf_ngtcp2_cmn_shutdown(cf, داده, انجامشده);
}

بنية Curl_cftype Curl_cft_h3_proxy = {
  "H3-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  cf_h3_proxy_destroy,
  cf_h3_proxy_connect,
  cf_h3_proxy_shutdown,
  cf_h3_proxy_adjust_pollset,
  cf_h3_proxy_داده_pending,
  cf_h3_proxy_send,
  cf_h3_proxy_recv,
  Curl_cf_def_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_ngtcp2_cmn_query,
};

CURLcode Curl_cf_ngtcp2_proxy_create(بنية Curl_cfilter **pcf,
                                     بنية Curl_easy *داده,
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
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;

  إذا(!tunnel_peer)
    أرجع CURLE_FAILED_INIT;
  إذا((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    أرجع CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  نتیجه = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  إذا(نتیجه)
    اذهب بیرون;
  cf->conn = conn;

  نتیجه = Curl_cf_udp_create(&cf->بعدی, داده, origin, peer, TRNSPRT_QUIC,
                              conn, addr, عدم, TRNSPRT_QUIC);
  إذا(نتیجه)
    اذهب بیرون;
  cf->بعدی->conn = cf->conn;
  cf->بعدی->sockindex = cf->sockindex;

بیرون:
  *pcf = (!نتیجه) ? cf : عدم;
  إذا(نتیجه) {
    إذا(cf)
      Curl_conn_cf_discard_chain(&cf, داده);
    وإلا إذا(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  وإلا
    CURL_TRC_CF(داده, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  أرجع نتیجه;
}

CURLcode Curl_cf_ngtcp2_proxy_درج_after(بنية Curl_cfilter *cf_at,
                                           بنية Curl_easy *داده,
                                           بنية Curl_peer *origin,
                                           بنية Curl_peer *peer,
                                           بنية Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_h3_proxy_ctx *ctx;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  (فراغ)داده;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب بیرون;
  نتیجه = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  إذا(نتیجه)
    اذهب بیرون;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_درج_after(cf_at, cf);

بیرون:
  إذا(نتیجه) {
    إذا(cf)
      Curl_conn_cf_discard_chain(&cf, داده);
    وإلا إذا(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  أرجع نتیجه;
}

#پایان_اگر
