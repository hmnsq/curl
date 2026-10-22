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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "http_proxy.h"
#অন্তর্ভাব "vquic/vquic.h"
#অন্তর্ভাব "vquic/cf-ngtcp2-cmn.h"
#অন্তর্ভাব "vquic/cf-ngtcp2-proxy.h"
#অন্তর্ভাব "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#define PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#define PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_STREAM_CHUNK_আকার)

প্রকারলেখ গণনা {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

কাঠামো h3_tunnel_stream {
  কাঠামো Curl_peer *dest;  /* where the tunnel goes to */
  কাঠামো http_resp *resp;
  কাঠামো bufq recvbuf;
  অক্ষর *authority;
  কাঠামো h3_stream_ctx *stream;
  h3_tunnel_state state;
  BIT(udp);
  BIT(has_final_response);
  BIT(closed);
};

স্থির CURLcode h3_tunnel_stream_init(কাঠামো h3_tunnel_stream *ts,
                                      কাঠামো Curl_peer *dest,
                                      bool udp)
{
  ts->state = H3_TUNNEL_INIT;
  Curl_peer_link(&ts->dest, dest);
  Curl_bufq_init2(&ts->recvbuf, H3_STREAM_CHUNK_আকার,
                  PROXY_H3_TUNNEL_RECV_CHUNKS, BUFQ_OPT_SOFT_LIMIT);
  ts->udp = udp;
  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->port);
  যদি(!ts->authority)
    ফেরত CURLE_OUT_OF_MEMORY;

  ফেরত CURLE_OK;
}

স্থির শূন্য h3_tunnel_stream_reset(কাঠামো h3_tunnel_stream *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = ফাঁকা;
  ts->stream = ফাঁকা;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

স্থির শূন্য h3_tunnel_stream_cleanup(কাঠামো h3_tunnel_stream *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

স্থির শূন্য h3_tunnel_go_state(কাঠামো Curl_cfilter *cf,
                               কাঠামো h3_tunnel_stream *ts,
                               h3_tunnel_state new_state,
                               কাঠামো Curl_easy *তথ্য)
{
  VERBOSE(int64_t stream_id = ts->stream ? ts->stream->id : -1);
  (শূন্য)cf;

  যদি(ts->state == new_state)
    ফেরত;

  /* leaving this one */
  নির্বাচন(ts->state) {
  ক্ষেত্র H3_TUNNEL_CONNECT:
    তথ্য->req.ignorebody = FALSE;
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }

  /* entering this one */
  নির্বাচন(new_state) {
  ক্ষেত্র H3_TUNNEL_INIT:
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] -> [init]", stream_id);
    h3_tunnel_stream_reset(ts);
    বিরতি;
  ক্ষেত্র H3_TUNNEL_CONNECT:
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] -> [connect]", stream_id);
    ts->state = H3_TUNNEL_CONNECT;
    বিরতি;
  ক্ষেত্র H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] -> [response]", stream_id);
    ts->state = H3_TUNNEL_RESPONSE;
    বিরতি;
  ক্ষেত্র H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] -> [established]", stream_id);
    infof(তথ্য, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    তথ্য->state.authproxy.done = TRUE;
    তথ্য->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(তথ্য->req.hd_proxy_auth);
    বিরতি;
  ক্ষেত্র H3_TUNNEL_FAILED:
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] -> [failed]", stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(তথ্য->req.hd_proxy_auth);
    বিরতি;
  }
}

কাঠামো cf_h3_proxy_ctx {
  কাঠামো cf_ngtcp2_ctx ngtcp2_ctx;
  কাঠামো h3_tunnel_stream tunnel; /* our tunnel CONNECT stream */
  BIT(connected);
};

স্থির CURLcode cf_ngtcp2_proxy_h3_init(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        কাঠামো cf_ngtcp2_ctx *ctx);

স্থির CURLcode cf_h3_proxy_ctx_init(কাঠামো cf_h3_proxy_ctx *ctx,
                                     কাঠামো Curl_peer *origin,
                                     কাঠামো Curl_peer *peer,
                                     কাঠামো ssl_filter_config *sslc,
                                     কাঠামো Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode ফলাফল;
  ফলাফল = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  যদি(!ফলাফল)
    ফলাফল = h3_tunnel_stream_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  ফেরত ফলাফল;
}

স্থির শূন্য cf_h3_proxy_ctx_free(কাঠামো cf_h3_proxy_ctx *ctx)
{
  যদি(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_stream_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

স্থির পূর্ণসংখ্যা cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                      uint64_t datalen, শূন্য *user_তথ্য,
                                      শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream;
  size_t skiplen;
  (শূন্য)stream_user_তথ্য;

  stream = pctx->tunnel.stream;
  যদি(!stream || (stream->id != stream_id))
    ফেরত 0;

  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  যদি(datalen >= (uint64_t)stream->tx_in_flight_আকার)
    skiplen = stream->tx_in_flight_আকার;
  অন্যথা
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_আকার -= skiplen;

  /* Resume upload processing if we have more data to send */
  যদি(stream->tx_in_flight_আকার < Curl_bufq_len(&stream->sendbuf)) {
    পূর্ণসংখ্যা rv = nghttp3_conn_resume_stream(conn, stream_id);
    যদি(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_h3_proxy_stream_close(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, শূন্য *user_তথ্য,
                                    শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  কাঠামো h3_stream_ctx *stream;

  (শূন্য)conn;
  (শূন্য)stream_user_তথ্য;
  যদি(!তথ্য)
    ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  যদি(!stream || (stream->id != stream_id))
    ফেরত 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  যদি(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  অন্যথা
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] CLOSED", stream->id);
  pctx->tunnel.stream = ফাঁকা;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(তথ্য);
  ফেরত 0;
}

স্থির শূন্য cf_h3_proxy_upd_rx_win(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো h3_stream_ctx *stream)
{
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  যদি(!stream) /* already closed */
    ফেরত;
  যদি(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    ফেরত;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = stream->rx_offset_max - stream->rx_offset;
  যদি(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    যদি(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    যদি(delta) {
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  stream->id, cur_win, rx_buffered, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
      ngtcp2_conn_extend_max_offset(ctx->qconn, delta);
    }
  }
}

স্থির পূর্ণসংখ্যা cb_h3_proxy_recv_তথ্য(nghttp3_conn *conn, int64_t stream3_id,
                                 ধ্রুবক uint8_t *buf, size_t buflen,
                                 শূন্য *user_তথ্য, শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  কাঠামো h3_stream_ctx *stream;
  size_t nwritten;
  CURLcode ফলাফল = CURLE_OK;
  (শূন্য)conn;
  (শূন্য)stream3_id;
  (শূন্য)stream_user_তথ্য;

  stream = pctx->tunnel.stream;
  যদি(!তথ্য || !stream || (stream->id != stream3_id)) {
    ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  stream->rx_যোগ += (curl_off_t)buflen;
  CURL_TRC_CF(তথ্য, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              stream->id, buflen, stream->rx_যোগ);

  ফলাফল = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  যদি(ফলাফল || (nwritten < buflen)) {
    ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  stream->rx_offset += buflen;
  যদি(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_h3_proxy_upd_rx_win(cf, তথ্য, stream);

  Curl_multi_mark_dirty(তথ্য);
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t stream_id,
                                        size_t consumed, শূন্য *user_তথ্য,
                                        শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (শূন্য)conn;
  (শূন্য)stream_user_তথ্য;

  যদি(!ctx)
    ফেরত 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_h3_proxy_recv_header(nghttp3_conn *conn, int64_t stream_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *মান, uint8_t flags,
                                   শূন্য *user_তথ্য, শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(মান);
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  কাঠামো h3_stream_ctx *stream;
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা http_status;
  কাঠামো http_resp *resp;
  (শূন্য)conn;
  (শূন্য)stream_id;
  (শূন্য)token;
  (শূন্য)flags;
  (শূন্য)stream_user_তথ্য;

  /* stream_user_data might be NULL for control streams */
  যদি(!তথ্য)
    ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  যদি(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (শূন্য *)তথ্য, তথ্য ? তথ্য->mid : 0);
    ফেরত 0;
  }

  যদি(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    ফেরত 0;
  }

  যদি(token == NGHTTP3_QPACK_TOKEN__STATUS) {
    ফলাফল = Curl_http_decode_status(&stream->status_code,
                                     (ধ্রুবক অক্ষর *)h3val.base, h3val.len);
    যদি(ফলাফল)
      ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
    http_status = stream->status_code;
    ফলাফল = Curl_http_resp_make(&resp, http_status, ফাঁকা);
    যদি(ফলাফল)
      ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
    যদি(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  অন্যথা {
    যদি(!pctx->tunnel.resp) {
      ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] header: %.*s: %.*s", stream_id,
                (পূর্ণসংখ্যা)h3name.len, h3name.base, (পূর্ণসংখ্যা)h3val.len, h3val.base);
    ফলাফল = Curl_dynhds_add(&pctx->tunnel.resp->headers,
                             (ধ্রুবক অক্ষর *)h3name.base, h3name.len,
                             (ধ্রুবক অক্ষর *)h3val.base, h3val.len);
    যদি(ফলাফল) {
      ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_h3_proxy_end_headers(nghttp3_conn *conn, int64_t stream_id,
                                   পূর্ণসংখ্যা fin, শূন্য *user_তথ্য,
                                   শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  কাঠামো h3_stream_ctx *stream;
  (শূন্য)conn;
  (শূন্য)stream_id;
  (শূন্য)fin;
  (শূন্য)stream_user_তথ্য;

  যদি(!তথ্য)
    ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  যদি(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (শূন্য *)তথ্য, তথ্য ? তথ্য->mid : 0);
    ফেরত 0;
  }

  CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] end_headers, status=%d", stream_id,
              stream->status_code);

  যদি(!pctx->tunnel.has_final_response) {
    যদি(stream->status_code / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  যদি(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(তথ্য);
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, শূন্য *user_তথ্য,
                                    শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (শূন্য)conn;
  (শূন্য)stream_user_তথ্য;

  যদি(ctx) {
    পূর্ণসংখ্যা rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                              app_error_code);

    যদি(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
      ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  ফেরত 0;
}

স্থির পূর্ণসংখ্যা cb_h3_proxy_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, শূন্য *user_তথ্য,
                                    শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  পূর্ণসংখ্যা rv;

  (শূন্য)conn;
  (শূন্য)stream_user_তথ্য;
  যদি(!তথ্য)
    ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;

  যদি(!pctx->tunnel.stream ||
     (stream_id != pctx->tunnel.stream->id))
    ফেরত 0;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  pctx->tunnel.stream = ফাঁকা;
  pctx->tunnel.closed = TRUE;
  যদি(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  ফেরত 0;
}

স্থির nghttp3_ssize cb_h3_tunnel_read_তথ্য(nghttp3_conn *conn,
                                            int64_t stream_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            শূন্য *user_তথ্য,
                                            শূন্য *stream_user_তথ্য)
{
  কাঠামো Curl_cfilter *cf = user_তথ্য;
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  কাঠামো h3_stream_ctx *stream;
  size_t nwritten = 0;
  size_t nvecs = 0;
  ধ্রুবক unsigned অক্ষর *buf_base;

  (শূন্য)conn;
  (শূন্য)stream_id;
  (শূন্য)veccnt;
  (শূন্য)stream_user_তথ্য;
  (শূন্য)pflags;

  stream = pctx->tunnel.stream;
  যদি(!তথ্য || !stream || (stream->id != stream_id))
    ফেরত NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  যদি(stream->tx_in_flight_আকার < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    যখন(nvecs < veccnt) {
      যদি(!Curl_bufq_peek_at(&stream->sendbuf,
                           stream->tx_in_flight_আকার,
                           &buf_base,
                           &vec[nvecs].len))
        বিরতি;
      vec[nvecs].base = (uint8_t *)(uintptr_t)buf_base;
      stream->tx_in_flight_আকার += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  যদি(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] read req body -> AGAIN",
                stream->id);
    ফেরত NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              stream->id, nvecs,
              *pflags == NGHTTP3_তথ্য_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf));
  ফেরত (nghttp3_ssize)nvecs;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
স্থির nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_stream_close,
  cb_h3_proxy_recv_তথ্য,
  cb_h3_proxy_deferred_consume,
  ফাঁকা, /* begin_headers */
  cb_h3_proxy_recv_header,
  cb_h3_proxy_end_headers,
  ফাঁকা, /* begin_trailers */
  cb_h3_proxy_recv_header,
  ফাঁকা, /* end_trailers */
  cb_h3_proxy_stop_sending,
  ফাঁকা, /* end_stream */
  cb_h3_proxy_reset_stream,
  ফাঁকা, /* shutdown */
  ফাঁকা, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  ফাঁকা, /* recv_origin */
  ফাঁকা, /* end_origin */
  ফাঁকা, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  ফাঁকা, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  ফাঁকা, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic বের_করো
#endif

স্থির CURLcode cf_ngtcp2_proxy_h3_init(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        কাঠামো cf_ngtcp2_ctx *ctx)
{
  পূর্ণসংখ্যা rc;

  যদি(ngtcp2_conn_get_streams_uni_বাম(ctx->qconn) < 3) {
    failf(তথ্য, "QUIC connection lacks 3 uni streams to run HTTP/3");
    ফেরত CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  যদি(rc) {
    failf(তথ্য, "error creating nghttp3 connection instance");
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  ফেরত Curl_cf_ngtcp2_h3_init_ctrls(ctx, তথ্য);
}

স্থির ssize_t cf_h3_proxy_recv_closed_stream(কাঠামো Curl_cfilter *cf,
                                              কাঠামো Curl_easy *তথ্য,
                                              কাঠামো h3_stream_ctx *stream,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  যদি(stream->reset) {
    যদি(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(তথ্য, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn);
      তথ্য->state.refused_stream = TRUE;
      *err = CURLE_RECV_ERROR;
      যাও out;
    }
    অন্যথা যদি(stream->resp_hds_complete && তথ্য->req.no_body) {
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", stream->id, stream->error3,
                  Curl_vquic_h3_err_str(stream->error3));
      nread = 0;
      যাও out;
    }
    failf(তথ্য, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    *err = তথ্য->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    যাও out;
  }
  অন্যথা যদি(!stream->resp_hds_complete) {
    failf(তথ্য,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    *err = CURLE_HTTP3;
    যাও out;
  }
  nread = 0;

out:
  ফেরত nread;
}

স্থির CURLcode cf_h3_proxy_sendbuf_add(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো h3_stream_ctx *stream,
                                        ধ্রুবক uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode ফলাফল;
  *pnwritten = 0;
  (শূন্য)তথ্য;

  ফলাফল = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h3_proxy_send(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 ধ্রুবক uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  কাঠামো h3_stream_ctx *stream = ফাঁকা;
  কাঠামো cf_call_তথ্য save;
  কাঠামো cf_ngtcp2_io_ctx pktx;
  CURLcode ফলাফল = CURLE_OK;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, তথ্য);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  যদি(ctx->tls_vrfy_ফলাফল) {
    ফলাফল = ctx->tls_vrfy_ফলাফল;
    যাও denied;
  }

  (শূন্য)eos; /* use for stream EOF and block handling */
  ফলাফল = Curl_cf_ngtcp2_progress_ingress(cf, তথ্য, &pktx);
  যদি(ফলাফল)
    যাও out;

  যদি(pctx->tunnel.closed) {
    ফলাফল = CURLE_SEND_ERROR;
    যাও denied;
  }

  stream = pctx->tunnel.stream;
  যদি(!stream) {
    ফলাফল = CURLE_SEND_ERROR;
    যাও denied;
  }

  যদি(stream->closed) {
    যদি(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      ফলাফল = CURLE_OK;
      *pnwritten = len;
      যাও out;
    }
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    ফলাফল = CURLE_HTTP3;
    যাও out;
  }
  অন্যথা {
    ফলাফল = cf_h3_proxy_sendbuf_add(তথ্য, stream, buf, len, pnwritten);
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (পূর্ণসংখ্যা)ফলাফল, *pnwritten);
    যদি(ফলাফল)
      যাও out;
    (শূন্য)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  যদি(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!ফলাফল);
  ফলাফল = Curl_cf_ngtcp2_progress_egress(cf, তথ্য, &pktx);

out:
  ফলাফল = Curl_1st_fatal(ফলাফল,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, তথ্য, &pktx));
denied:
  CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (পূর্ণসংখ্যা)ফলাফল, *pnwritten);
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

/* incoming data frames on the h3 stream */
স্থির CURLcode cf_h3_proxy_recv(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 অক্ষর *buf, size_t len, size_t *pnread)
{
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  কাঠামো h3_stream_ctx *stream = pctx->tunnel.stream;
  কাঠামো cf_call_তথ্য save;
  কাঠামো cf_ngtcp2_io_ctx pktx;
  CURLcode ফলাফল = CURLE_OK;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  যদি(ctx->tls_vrfy_ফলাফল) {
    ফলাফল = ctx->tls_vrfy_ফলাফল;
    যাও denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, তথ্য);

  যদি(!stream || ctx->shutdown_started) {
    ফলাফল = CURLE_RECV_ERROR;
    যাও out;
  }

  যদি(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    ফলাফল = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    যদি(ফলাফল)
      যাও out;
    cf_h3_proxy_upd_rx_win(cf, তথ্য, pctx->tunnel.stream);
  }

  ফলাফল = Curl_cf_ngtcp2_progress_ingress(cf, তথ্য, &pktx);
  যদি(ফলাফল)
    যাও out;

  /* inbufq had nothing before, maybe after progressing ingress? */
  যদি(!*pnread && !Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    ফলাফল = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    যদি(ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  stream->id, len, *pnread, (পূর্ণসংখ্যা)ফলাফল);
      যাও out;
    }
    cf_h3_proxy_upd_rx_win(cf, তথ্য, pctx->tunnel.stream);
  }

  যদি(*pnread) {
    Curl_multi_mark_dirty(তথ্য);
  }
  অন্যথা {
    যদি(stream->xfer_ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, তথ্য, stream);
      ফলাফল = stream->xfer_ফলাফল;
      যাও out;
    }
    অন্যথা যদি(stream->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_stream(cf, তথ্য, stream, &ফলাফল);
      যদি(nread > 0)
        *pnread = (size_t)nread;
      যাও out;
    }
    ফলাফল = CURLE_AGAIN;
  }

out:
  ফলাফল = Curl_1st_fatal(ফলাফল,
                          Curl_cf_ngtcp2_progress_egress(cf, তথ্য, &pktx));
  ফলাফল = Curl_1st_fatal(ফলাফল,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, তথ্য, &pktx));
denied:
  CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] cf_recv(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (পূর্ণসংখ্যা)ফলাফল, *pnread);
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h3_proxy_submit(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো h3_tunnel_stream *ts,
                                   কাঠামো httpreq *req)
{
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  কাঠামো h3_stream_ctx *stream = ফাঁকা;
  কাঠামো dynhds h2_headers;
  nghttp3_nv *nva = ফাঁকা;
  size_t nheader;
  পূর্ণসংখ্যা rc = 0;
  unsigned পূর্ণসংখ্যা i;
  nghttp3_তথ্য_reader reader;
  nghttp3_তথ্য_reader *preader = ফাঁকা;
  CURLcode ফলাফল;

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);
  ফলাফল = Curl_http_req_to_h2(&h2_headers, req, তথ্য);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = Curl_cf_ngtcp2_h3_stream_setup(cf, তথ্য, &stream);
  যদি(ফলাফল)
    যাও out;

  nheader = Curl_dynhds_গুনতি(&h2_headers);
  nva = curlx_malloc(আকার(nghttp3_nv) * nheader);
  যদি(!nva) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  জন্য(i = 0; i < nheader; ++i) {
    কাঠামো dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned অক্ষর *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].মান = (unsigned অক্ষর *)e->মান;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  /* Open a bidirectional stream */
  {
    int64_t sid;
    পূর্ণসংখ্যা rv;

    DEBUGASSERT(stream->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, ফাঁকা);
    যদি(rv) {
      failf(তথ্য, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
    stream->id = sid;
    ts->stream = stream;
    ++ctx->used_bidi_streams;
    CURL_TRC_CF(তথ্য, cf, "[%" PRId64 "] opened bidi stream", sid);
  }

  /* CONNECT-UDP request stream remains open for capsules, no fixed EOF. */
  stream->send_closed = 0;
  reader.read_তথ্য = cb_h3_tunnel_read_তথ্য;
  preader = &reader;

  rc = nghttp3_conn_submit_request(ctx->h3conn, stream->id,
                                   nva, nheader, preader, stream);

  যদি(rc) {
    নির্বাচন(rc) {
    ক্ষেত্র NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(তথ্য, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      বিরতি;
    পূর্বনির্ধারিত:
      CURL_TRC_CF(তথ্য, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  stream->id, rc, nghttp3_strerror(rc));
      বিরতি;
    }
    ফলাফল = CURLE_SEND_ERROR;
    যাও out;
  }

  যদি(Curl_trc_is_verbose(তথ্য)) {
    CURL_TRC_CF(তথ্য, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", stream->id,
                Curl_bufref_ptr(&তথ্য->state.url));
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h3_proxy_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                           কাঠামো Curl_easy *তথ্য,
                                           কাঠামো easy_pollset *ps)
{
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode ফলাফল = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, তথ্য);

  যদি(!ctx->qconn || !pctx->tunnel.stream || (sock == CURL_SOCKET_BAD))
    ফেরত CURLE_OK;

  Curl_pollset_check(তথ্য, ps, sock, &want_recv, &want_send);

  যদি(want_recv || want_send || !Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    কাঠামো h3_stream_ctx *stream = pctx->tunnel.stream;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_বাম(ctx->qconn) ||
                 !ngtcp2_conn_get_max_তথ্য_বাম(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                stream->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    ফলাফল = Curl_pollset_set(তথ্য, ps, sock, want_recv, want_send);
  }
  ফেরত ফলাফল;
}

স্থির bool cf_h3_proxy_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                                     ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  যদি(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf))
    ফেরত TRUE;
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->has_তথ্য_pending(cf->পরবর্তী, তথ্য) : FALSE;
}

স্থির CURLcode cf_h3_proxy_submit_CONNECT(কাঠামো Curl_cfilter *cf,
                                           কাঠামো Curl_easy *তথ্য,
                                           কাঠামো h3_tunnel_stream *ts)
{
  কাঠামো cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল;
  কাঠামো httpreq *req = ফাঁকা;

  ফলাফল = Curl_http_proxy_create_tunnel_request(
    &req, cf, তথ্য, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  যদি(!ফলাফল)
    ফলাফল = Curl_creader_set_null(তথ্য);
  যদি(!ফলাফল)
    ফলাফল = cf_h3_proxy_submit(cf, তথ্য, ts, req);

  যদি(req)
    Curl_http_req_free(req);
  যদি(ফলাফল)
    failf(তথ্য, "Failed sending CONNECT to proxy");
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h3_proxy_inspect_response(কাঠামো Curl_cfilter *cf,
                                             কাঠামো Curl_easy *তথ্য,
                                             কাঠামো h3_tunnel_stream *ts)
{
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_ফলাফল res;
  CURLcode ফলাফল;

  ফলাফল = Curl_http_proxy_inspect_tunnel_response(
      cf, তথ্য, ts->resp, (bool)pctx->tunnel.udp, &res);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  নির্বাচন(res) {
  ক্ষেত্র PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, তথ্য);
    বিরতি;
  ক্ষেত্র PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, তথ্য);
    ফলাফল = CURLE_COULDNT_CONNECT;
    বিরতি;
  ক্ষেত্র PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, তথ্য);
    বিরতি;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h3_proxy_tunnel(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো h3_tunnel_stream *ts,
                                   bool *pdone)
{
  কাঠামো cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  করো {
    নির্বাচন(ts->state) {
    ক্ষেত্র H3_TUNNEL_INIT:
      CURL_TRC_CF(তথ্য, cf, "[0] CONNECT start for %s", ts->authority);
      ফলাফল = cf_h3_proxy_submit_CONNECT(cf, তথ্য, ts);
      যদি(ফলাফল)
        যাও out;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, তথ্য);

      ফলাফল = Curl_cf_ngtcp2_progress_egress(cf, তথ্য, ফাঁকা);
      যদি(ফলাফল)
        যাও out;
      FALLTHROUGH();

    ক্ষেত্র H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      ফলাফল = Curl_cf_ngtcp2_progress_ingress(cf, তথ্য, ফাঁকা);
      যদি(ফলাফল)
        যাও out;
      ফলাফল = Curl_cf_ngtcp2_progress_egress(cf, তথ্য, ফাঁকা);
      যদি(ফলাফল && ফলাফল != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, তথ্য);
        যাও out;
      }

      যদি(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, তথ্য);
      }
      অন্যথা {
        /* Not done yet, return and let multi interface call us again */
        ফলাফল = CURLE_OK;
        যাও out;
      }
      FALLTHROUGH();

    ক্ষেত্র H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      ফলাফল = cf_h3_proxy_inspect_response(cf, তথ্য, ts);
      যদি(ফলাফল)
        যাও out;
      ctx->connected = TRUE;
      বিরতি;

    ক্ষেত্র H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      ফেরত CURLE_OK;

    ক্ষেত্র H3_TUNNEL_FAILED:
      ফেরত CURLE_RECV_ERROR;

    পূর্বনির্ধারিত:
      বিরতি;
    }

  } যখন(ts->state == H3_TUNNEL_INIT);

out:
  যদি((ফলাফল && (ফলাফল != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, তথ্য);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h3_proxy_connect(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো cf_h3_proxy_ctx *pctx = cf->ctx;
  কাঠামো cf_call_তথ্য save = { 0 };
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো h3_tunnel_stream *ts = &pctx->tunnel;
  bool তথ্য_saved = FALSE;

  ফলাফল = Curl_cf_ngtcp2_cmn_connect(cf, তথ্য, done);
  যদি(ফলাফল || !*done)
    যাও out;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  তথ্য_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  ফলাফল = cf_h3_proxy_tunnel(cf, তথ্য, ts, done);

out:
  যদি(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&তথ্য->req, তথ্য);
    Curl_client_reset(তথ্য);
  }

  যদি(তথ্য_saved)
    CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির শূন্য cf_h3_proxy_destroy(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h3_proxy_ctx *ctx = cf->ctx;

  (শূন্য)তথ্য;
  যদি(ctx) {
    CURL_TRC_CF(তথ্য, cf, "cf_h3_proxy_destroy()");
    যদি(ctx->ngtcp2_ctx.qconn) {
      কাঠামো cf_call_তথ্য save;
      CF_তথ্য_SAVE(save, cf, তথ্য);
      Curl_cf_ngtcp2_cmn_conn_close(cf, তথ্য);
      CF_তথ্য_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = ফাঁকা;
  }
}

স্থির CURLcode cf_h3_proxy_shutdown(কাঠামো Curl_cfilter *cf,
                                     কাঠামো Curl_easy *তথ্য, bool *done)
{
  ফেরত Curl_cf_ngtcp2_cmn_shutdown(cf, তথ্য, done);
}

কাঠামো Curl_cftype Curl_cft_h3_proxy = {
  "H3-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  cf_h3_proxy_destroy,
  cf_h3_proxy_connect,
  cf_h3_proxy_shutdown,
  cf_h3_proxy_adjust_pollset,
  cf_h3_proxy_তথ্য_pending,
  cf_h3_proxy_send,
  cf_h3_proxy_recv,
  Curl_cf_def_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_ngtcp2_cmn_query,
};

CURLcode Curl_cf_ngtcp2_proxy_create(কাঠামো Curl_cfilter **pcf,
                                     কাঠামো Curl_easy *তথ্য,
                                     কাঠামো Curl_peer *origin,
                                     কাঠামো Curl_peer *peer,
                                     uint8_t transport_peer,
                                     কাঠামো connectdata *conn,
                                     কাঠামো Curl_sockaddr_ex *addr,
                                     কাঠামো Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  কাঠামো cf_h3_proxy_ctx *ctx;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  যদি(!tunnel_peer)
    ফেরত CURLE_FAILED_INIT;
  যদি((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    ফেরত CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  ফলাফল = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  যদি(ফলাফল)
    যাও out;
  cf->conn = conn;

  ফলাফল = Curl_cf_udp_create(&cf->পরবর্তী, তথ্য, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ফাঁকা, TRNSPRT_QUIC);
  যদি(ফলাফল)
    যাও out;
  cf->পরবর্তী->conn = cf->conn;
  cf->পরবর্তী->sockindex = cf->sockindex;

out:
  *pcf = (!ফলাফল) ? cf : ফাঁকা;
  যদি(ফলাফল) {
    যদি(cf)
      Curl_conn_cf_discard_chain(&cf, তথ্য);
    অন্যথা যদি(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  অন্যথা
    CURL_TRC_CF(তথ্য, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  ফেরত ফলাফল;
}

CURLcode Curl_cf_ngtcp2_proxy_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                           কাঠামো Curl_easy *তথ্য,
                                           কাঠামো Curl_peer *origin,
                                           কাঠামো Curl_peer *peer,
                                           কাঠামো Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  কাঠামো cf_h3_proxy_ctx *ctx;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  (শূন্য)তথ্য;

  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx)
    যাও out;
  ফলাফল = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  যদি(ফলাফল)
    যাও out;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);

out:
  যদি(ফলাফল) {
    যদি(cf)
      Curl_conn_cf_discard_chain(&cf, তথ্য);
    অন্যথা যদি(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  ফেরত ফলাফল;
}

#endif
