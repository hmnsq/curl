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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "http_proxy.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic.h"
#ଅନ୍ତର୍ଭାଵ "vquic/cf-ngtcp2-cmn.h"
#ଅନ୍ତର୍ଭାଵ "vquic/cf-ngtcp2-proxy.h"
#ଅନ୍ତର୍ଭାଵ "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#define PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#define PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_STREAM_CHUNK_SIZE)

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

ସଂରଚନା h3_tunnel_stream {
  ସଂରଚନା Curl_peer *dest;  /* where the tunnel goes to */
  ସଂରଚନା http_resp *resp;
  ସଂରଚନା bufq recvbuf;
  ଅକ୍ଷର *authority;
  ସଂରଚନା h3_stream_ctx *stream;
  h3_tunnel_state state;
  BIT(udp);
  BIT(has_final_response);
  BIT(closed);
};

ସ୍ଥିର CURLcode h3_tunnel_stream_init(ସଂରଚନା h3_tunnel_stream *ts,
                                      ସଂରଚନା Curl_peer *dest,
                                      bool udp)
{
  ts->state = H3_TUNNEL_INIT;
  Curl_peer_link(&ts->dest, dest);
  Curl_bufq_init2(&ts->recvbuf, H3_STREAM_CHUNK_SIZE,
                  PROXY_H3_TUNNEL_RECV_CHUNKS, BUFQ_OPT_SOFT_LIMIT);
  ts->udp = udp;
  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->port);
  ଯଦି(!ts->authority)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ h3_tunnel_stream_reset(ସଂରଚନା h3_tunnel_stream *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = ଖାଲି;
  ts->stream = ଖାଲି;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

ସ୍ଥିର ଶୂନ୍ୟ h3_tunnel_stream_cleanup(ସଂରଚନା h3_tunnel_stream *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

ସ୍ଥିର ଶୂନ୍ୟ h3_tunnel_go_state(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା h3_tunnel_stream *ts,
                               h3_tunnel_state new_state,
                               ସଂରଚନା Curl_easy *data)
{
  VERBOSE(int64_t stream_id = ts->stream ? ts->stream->id : -1);
  (ଶୂନ୍ୟ)cf;

  ଯଦି(ts->state == new_state)
    ଫେରନ୍ତୁ;

  /* leaving this one */
  ବାଛ(ts->state) {
  ସ୍ଥିତି H3_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }

  /* entering this one */
  ବାଛ(new_state) {
  ସ୍ଥିତି H3_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [init]", stream_id);
    h3_tunnel_stream_reset(ts);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି H3_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [connect]", stream_id);
    ts->state = H3_TUNNEL_CONNECT;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [response]", stream_id);
    ts->state = H3_TUNNEL_RESPONSE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [established]", stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(data->req.hd_proxy_auth);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି H3_TUNNEL_FAILED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [failed]", stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    ଭାଙ୍ଗ;
  }
}

ସଂରଚନା cf_h3_proxy_ctx {
  ସଂରଚନା cf_ngtcp2_ctx ngtcp2_ctx;
  ସଂରଚନା h3_tunnel_stream tunnel; /* our tunnel CONNECT stream */
  BIT(connected);
};

ସ୍ଥିର CURLcode cf_ngtcp2_proxy_h3_init(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା cf_ngtcp2_ctx *ctx);

ସ୍ଥିର CURLcode cf_h3_proxy_ctx_init(ସଂରଚନା cf_h3_proxy_ctx *ctx,
                                     ସଂରଚନା Curl_peer *origin,
                                     ସଂରଚନା Curl_peer *peer,
                                     ସଂରଚନା ssl_filter_config *sslc,
                                     ସଂରଚନା Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode result;
  result = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  ଯଦି(!result)
    result = h3_tunnel_stream_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_h3_proxy_ctx_free(ସଂରଚନା cf_h3_proxy_ctx *ctx)
{
  ଯଦି(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_stream_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                      uint64_t datalen, ଶୂନ୍ୟ *user_data,
                                      ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା h3_stream_ctx *stream;
  size_t skiplen;
  (ଶୂନ୍ୟ)stream_user_data;

  stream = pctx->tunnel.stream;
  ଯଦି(!stream || (stream->id != stream_id))
    ଫେରନ୍ତୁ 0;

  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  ଯଦି(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  ଅନ୍ୟଥା
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  ଯଦି(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv = nghttp3_conn_resume_stream(conn, stream_id);
    ଯଦି(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_h3_proxy_stream_close(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, ଶୂନ୍ୟ *user_data,
                                    ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  ସଂରଚନା h3_stream_ctx *stream;

  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)stream_user_data;
  ଯଦି(!data)
    ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  ଯଦି(!stream || (stream->id != stream_id))
    ଫେରନ୍ତୁ 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  ଯଦି(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  ଅନ୍ୟଥା
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  pctx->tunnel.stream = ଖାଲି;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(data);
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_h3_proxy_upd_rx_win(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା h3_stream_ctx *stream)
{
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  ଯଦି(!stream) /* already closed */
    ଫେରନ୍ତୁ;
  ଯଦି(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = stream->rx_offset_max - stream->rx_offset;
  ଯଦି(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    ଯଦି(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    ଯଦି(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  stream->id, cur_win, rx_buffered, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
      ngtcp2_conn_extend_max_offset(ctx->qconn, delta);
    }
  }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_h3_proxy_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                                 ଅଚଳ uint8_t *buf, size_t buflen,
                                 ଶୂନ୍ୟ *user_data, ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  ସଂରଚନା h3_stream_ctx *stream;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)stream3_id;
  (ଶୂନ୍ୟ)stream_user_data;

  stream = pctx->tunnel.stream;
  ଯଦି(!data || !stream || (stream->id != stream3_id)) {
    ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  stream->rx_total += (curl_off_t)buflen;
  CURL_TRC_CF(data, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              stream->id, buflen, stream->rx_total);

  result = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  ଯଦି(result || (nwritten < buflen)) {
    ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  stream->rx_offset += buflen;
  ଯଦି(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_h3_proxy_upd_rx_win(cf, data, stream);

  Curl_multi_mark_dirty(data);
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t stream_id,
                                        size_t consumed, ଶୂନ୍ୟ *user_data,
                                        ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)stream_user_data;

  ଯଦି(!ctx)
    ଫେରନ୍ତୁ 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_h3_proxy_recv_header(nghttp3_conn *conn, int64_t stream_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *ମୂଲ୍ୟ, uint8_t flags,
                                   ଶୂନ୍ୟ *user_data, ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(ମୂଲ୍ୟ);
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  ସଂରଚନା h3_stream_ctx *stream;
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା http_status;
  ସଂରଚନା http_resp *resp;
  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)stream_id;
  (ଶୂନ୍ୟ)token;
  (ଶୂନ୍ୟ)flags;
  (ଶୂନ୍ୟ)stream_user_data;

  /* stream_user_data might be NULL for control streams */
  ଯଦି(!data)
    ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  ଯଦି(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (ଶୂନ୍ୟ *)data, data ? data->mid : 0);
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(token == NGHTTP3_QPACK_TOKEN__STATUS) {
    result = Curl_http_decode_status(&stream->status_code,
                                     (ଅଚଳ ଅକ୍ଷର *)h3val.base, h3val.len);
    ଯଦି(result)
      ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;
    http_status = stream->status_code;
    result = Curl_http_resp_make(&resp, http_status, ଖାଲି);
    ଯଦି(result)
      ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;
    ଯଦି(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  ଅନ୍ୟଥା {
    ଯଦି(!pctx->tunnel.resp) {
      ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s", stream_id,
                (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)h3name.len, h3name.base, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)h3val.len, h3val.base);
    result = Curl_dynhds_add(&pctx->tunnel.resp->headers,
                             (ଅଚଳ ଅକ୍ଷର *)h3name.base, h3name.len,
                             (ଅଚଳ ଅକ୍ଷର *)h3val.base, h3val.len);
    ଯଦି(result) {
      ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_h3_proxy_end_headers(nghttp3_conn *conn, int64_t stream_id,
                                   ପୂର୍ଣ୍ଣସଂଖ୍ୟା fin, ଶୂନ୍ୟ *user_data,
                                   ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  ସଂରଚନା h3_stream_ctx *stream;
  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)stream_id;
  (ଶୂନ୍ୟ)fin;
  (ଶୂନ୍ୟ)stream_user_data;

  ଯଦି(!data)
    ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  ଯଦି(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (ଶୂନ୍ୟ *)data, data ? data->mid : 0);
    ଫେରନ୍ତୁ 0;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d", stream_id,
              stream->status_code);

  ଯଦି(!pctx->tunnel.has_final_response) {
    ଯଦି(stream->status_code / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  ଯଦି(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(data);
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, ଶୂନ୍ୟ *user_data,
                                    ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)stream_user_data;

  ଯଦି(ctx) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                              app_error_code);

    ଯଦି(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
      ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_h3_proxy_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, ଶୂନ୍ୟ *user_data,
                                    ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;

  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)stream_user_data;
  ଯଦି(!data)
    ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;

  ଯଦି(!pctx->tunnel.stream ||
     (stream_id != pctx->tunnel.stream->id))
    ଫେରନ୍ତୁ 0;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  pctx->tunnel.stream = ଖାଲି;
  pctx->tunnel.closed = TRUE;
  ଯଦି(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର nghttp3_ssize cb_h3_tunnel_read_data(nghttp3_conn *conn,
                                            int64_t stream_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            ଶୂନ୍ୟ *user_data,
                                            ଶୂନ୍ୟ *stream_user_data)
{
  ସଂରଚନା Curl_cfilter *cf = user_data;
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  ସଂରଚନା h3_stream_ctx *stream;
  size_t nwritten = 0;
  size_t nvecs = 0;
  ଅଚଳ unsigned ଅକ୍ଷର *buf_base;

  (ଶୂନ୍ୟ)conn;
  (ଶୂନ୍ୟ)stream_id;
  (ଶୂନ୍ୟ)veccnt;
  (ଶୂନ୍ୟ)stream_user_data;
  (ଶୂନ୍ୟ)pflags;

  stream = pctx->tunnel.stream;
  ଯଦି(!data || !stream || (stream->id != stream_id))
    ଫେରନ୍ତୁ NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  ଯଦି(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    ଯେତେବେଳେ(nvecs < veccnt) {
      ଯଦି(!Curl_bufq_peek_at(&stream->sendbuf,
                           stream->tx_in_flight_size,
                           &buf_base,
                           &vec[nvecs].len))
        ଭାଙ୍ଗ;
      vec[nvecs].base = (uint8_t *)(uintptr_t)buf_base;
      stream->tx_in_flight_size += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  ଯଦି(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN",
                stream->id);
    ଫେରନ୍ତୁ NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf));
  ଫେରନ୍ତୁ (nghttp3_ssize)nvecs;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
ସ୍ଥିର nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_stream_close,
  cb_h3_proxy_recv_data,
  cb_h3_proxy_deferred_consume,
  ଖାଲି, /* begin_headers */
  cb_h3_proxy_recv_header,
  cb_h3_proxy_end_headers,
  ଖାଲି, /* begin_trailers */
  cb_h3_proxy_recv_header,
  ଖାଲି, /* end_trailers */
  cb_h3_proxy_stop_sending,
  ଖାଲି, /* end_stream */
  cb_h3_proxy_reset_stream,
  ଖାଲି, /* shutdown */
  ଖାଲି, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  ଖାଲି, /* recv_origin */
  ଖାଲି, /* end_origin */
  ଖାଲି, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  ଖାଲି, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  ଖାଲି, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic କାଢ଼
#endif

ସ୍ଥିର CURLcode cf_ngtcp2_proxy_h3_init(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା cf_ngtcp2_ctx *ctx)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(ngtcp2_conn_get_streams_uni_ବାମ(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  ଯଦି(rc) {
    failf(data, "error creating nghttp3 connection instance");
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  ଫେରନ୍ତୁ Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

ସ୍ଥିର ssize_t cf_h3_proxy_recv_closed_stream(ସଂରଚନା Curl_cfilter *cf,
                                              ସଂରଚନା Curl_easy *data,
                                              ସଂରଚନା h3_stream_ctx *stream,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  ଯଦି(stream->reset) {
    ଯଦି(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn);
      data->state.refused_stream = TRUE;
      *err = CURLE_RECV_ERROR;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", stream->id, stream->error3,
                  Curl_vquic_h3_err_str(stream->error3));
      nread = 0;
      ଯାଅ out;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    *err = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    ଯାଅ out;
  }
  ଅନ୍ୟଥା ଯଦି(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    *err = CURLE_HTTP3;
    ଯାଅ out;
  }
  nread = 0;

out:
  ଫେରନ୍ତୁ nread;
}

ସ୍ଥିର CURLcode cf_h3_proxy_sendbuf_add(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା h3_stream_ctx *stream,
                                        ଅଚଳ uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode result;
  *pnwritten = 0;
  (ଶୂନ୍ୟ)data;

  result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h3_proxy_send(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ସଂରଚନା h3_stream_ctx *stream = ଖାଲି;
  ସଂରଚନା cf_call_data save;
  ସଂରଚନା cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  ଯଦି(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    ଯାଅ denied;
  }

  (ଶୂନ୍ୟ)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  ଯଦି(result)
    ଯାଅ out;

  ଯଦି(pctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    ଯାଅ denied;
  }

  stream = pctx->tunnel.stream;
  ଯଦି(!stream) {
    result = CURLE_SEND_ERROR;
    ଯାଅ denied;
  }

  ଯଦି(stream->closed) {
    ଯଦି(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      ଯାଅ out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    ଯାଅ out;
  }
  ଅନ୍ୟଥା {
    result = cf_h3_proxy_sendbuf_add(data, stream, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten);
    ଯଦି(result)
      ଯାଅ out;
    (ଶୂନ୍ୟ)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  ଯଦି(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

/* incoming data frames on the h3 stream */
ସ୍ଥିର CURLcode cf_h3_proxy_recv(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ଅକ୍ଷର *buf, size_t len, size_t *pnread)
{
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ସଂରଚନା h3_stream_ctx *stream = pctx->tunnel.stream;
  ସଂରଚନା cf_call_data save;
  ସଂରଚନା cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  ଯଦି(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    ଯାଅ denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  ଯଦି(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    ଯାଅ out;
  }

  ଯଦି(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    ଯଦି(result)
      ଯାଅ out;
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  ଯଦି(result)
    ଯାଅ out;

  /* inbufq had nothing before, maybe after progressing ingress? */
  ଯଦି(!*pnread && !Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    ଯଦି(result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  stream->id, len, *pnread, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      ଯାଅ out;
    }
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  ଯଦି(*pnread) {
    Curl_multi_mark_dirty(data);
  }
  ଅନ୍ୟଥା {
    ଯଦି(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(stream->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_stream(cf, data, stream, &result);
      ଯଦି(nread > 0)
        *pnread = (size_t)nread;
      ଯାଅ out;
    }
    result = CURLE_AGAIN;
  }

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_progress_egress(cf, data, &pktx));
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_recv(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h3_proxy_submit(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା h3_tunnel_stream *ts,
                                   ସଂରଚନା httpreq *req)
{
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ସଂରଚନା h3_stream_ctx *stream = ଖାଲି;
  ସଂରଚନା dynhds h2_headers;
  nghttp3_nv *nva = ଖାଲି;
  size_t nheader;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = ଖାଲି;
  CURLcode result;

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  ଯଦି(result)
    ଯାଅ out;

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  ଯଦି(result)
    ଯାଅ out;

  nheader = Curl_dynhds_ଗଣନା(&h2_headers);
  nva = curlx_malloc(ଆକାର(nghttp3_nv) * nheader);
  ଯଦି(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  ପାଇଁ(i = 0; i < nheader; ++i) {
    ସଂରଚନା dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned ଅକ୍ଷର *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].ମୂଲ୍ୟ = (unsigned ଅକ୍ଷର *)e->ମୂଲ୍ୟ;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  /* Open a bidirectional stream */
  {
    int64_t sid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;

    DEBUGASSERT(stream->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, ଖାଲି);
    ଯଦି(rv) {
      failf(data, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
    stream->id = sid;
    ts->stream = stream;
    ++ctx->used_bidi_streams;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] opened bidi stream", sid);
  }

  /* CONNECT-UDP request stream remains open for capsules, no fixed EOF. */
  stream->send_closed = 0;
  reader.read_data = cb_h3_tunnel_read_data;
  preader = &reader;

  rc = nghttp3_conn_submit_request(ctx->h3conn, stream->id,
                                   nva, nheader, preader, stream);

  ଯଦି(rc) {
    ବାଛ(rc) {
    ସ୍ଥିତି NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  stream->id, rc, nghttp3_strerror(rc));
      ଭାଙ୍ଗ;
    }
    result = CURLE_SEND_ERROR;
    ଯାଅ out;
  }

  ଯଦି(Curl_trc_is_verbose(data)) {
    CURL_TRC_CF(data, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", stream->id,
                Curl_bufref_ptr(&data->state.url));
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h3_proxy_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                           ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  ଯଦି(!ctx->qconn || !pctx->tunnel.stream || (sock == CURL_SOCKET_BAD))
    ଫେରନ୍ତୁ CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  ଯଦି(want_recv || want_send || !Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    ସଂରଚନା h3_stream_ctx *stream = pctx->tunnel.stream;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_ବାମ(ctx->qconn) ||
                 !ngtcp2_conn_get_max_data_ବାମ(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                stream->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool cf_h3_proxy_data_pending(ସଂରଚନା Curl_cfilter *cf,
                                     ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ଯଦି(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf))
    ଫେରନ୍ତୁ TRUE;
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ସ୍ଥିର CURLcode cf_h3_proxy_submit_CONNECT(ସଂରଚନା Curl_cfilter *cf,
                                           ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା h3_tunnel_stream *ts)
{
  ସଂରଚନା cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  ସଂରଚନା httpreq *req = ଖାଲି;

  result = Curl_http_proxy_create_tunnel_request(
    &req, cf, data, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  ଯଦି(!result)
    result = Curl_creader_set_null(data);
  ଯଦି(!result)
    result = cf_h3_proxy_submit(cf, data, ts, req);

  ଯଦି(req)
    Curl_http_req_free(req);
  ଯଦି(result)
    failf(data, "Failed sending CONNECT to proxy");
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h3_proxy_inspect_response(ସଂରଚନା Curl_cfilter *cf,
                                             ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା h3_tunnel_stream *ts)
{
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)pctx->tunnel.udp, &res);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ବାଛ(res) {
  ସ୍ଥିତି PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, data);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
    result = CURLE_COULDNT_CONNECT;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, data);
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h3_proxy_tunnel(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା h3_tunnel_stream *ts,
                                   bool *pdone)
{
  ସଂରଚନା cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  କର {
    ବାଛ(ts->state) {
    ସ୍ଥିତି H3_TUNNEL_INIT:
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = cf_h3_proxy_submit_CONNECT(cf, data, ts);
      ଯଦି(result)
        ଯାଅ out;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, data);

      result = Curl_cf_ngtcp2_progress_egress(cf, data, ଖାଲି);
      ଯଦି(result)
        ଯାଅ out;
      FALLTHROUGH();

    ସ୍ଥିତି H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, ଖାଲି);
      ଯଦି(result)
        ଯାଅ out;
      result = Curl_cf_ngtcp2_progress_egress(cf, data, ଖାଲି);
      ଯଦି(result && result != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
        ଯାଅ out;
      }

      ଯଦି(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, data);
      }
      ଅନ୍ୟଥା {
        /* Not done yet, return and let multi interface call us again */
        result = CURLE_OK;
        ଯାଅ out;
      }
      FALLTHROUGH();

    ସ୍ଥିତି H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = cf_h3_proxy_inspect_response(cf, data, ts);
      ଯଦି(result)
        ଯାଅ out;
      ctx->connected = TRUE;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      ଫେରନ୍ତୁ CURLE_OK;

    ସ୍ଥିତି H3_TUNNEL_FAILED:
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }

  } ଯେତେବେଳେ(ts->state == H3_TUNNEL_INIT);

out:
  ଯଦି((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h3_proxy_connect(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା cf_h3_proxy_ctx *pctx = cf->ctx;
  ସଂରଚନା cf_call_data save = { 0 };
  CURLcode result = CURLE_OK;
  ସଂରଚନା h3_tunnel_stream *ts = &pctx->tunnel;
  bool data_saved = FALSE;

  result = Curl_cf_ngtcp2_cmn_connect(cf, data, done);
  ଯଦି(result || !*done)
    ଯାଅ out;

  CF_DATA_SAVE(save, cf, data);
  data_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  result = cf_h3_proxy_tunnel(cf, data, ts, done);

out:
  ଯଦି(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }

  ଯଦି(data_saved)
    CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_h3_proxy_destroy(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h3_proxy_ctx *ctx = cf->ctx;

  (ଶୂନ୍ୟ)data;
  ଯଦି(ctx) {
    CURL_TRC_CF(data, cf, "cf_h3_proxy_destroy()");
    ଯଦି(ctx->ngtcp2_ctx.qconn) {
      ସଂରଚନା cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = ଖାଲି;
  }
}

ସ୍ଥିର CURLcode cf_h3_proxy_shutdown(ସଂରଚନା Curl_cfilter *cf,
                                     ସଂରଚନା Curl_easy *data, bool *done)
{
  ଫେରନ୍ତୁ Curl_cf_ngtcp2_cmn_shutdown(cf, data, done);
}

ସଂରଚନା Curl_cftype Curl_cft_h3_proxy = {
  "H3-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  cf_h3_proxy_destroy,
  cf_h3_proxy_connect,
  cf_h3_proxy_shutdown,
  cf_h3_proxy_adjust_pollset,
  cf_h3_proxy_data_pending,
  cf_h3_proxy_send,
  cf_h3_proxy_recv,
  Curl_cf_def_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_ngtcp2_cmn_query,
};

CURLcode Curl_cf_ngtcp2_proxy_create(ସଂରଚନା Curl_cfilter **pcf,
                                     ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା Curl_peer *origin,
                                     ସଂରଚନା Curl_peer *peer,
                                     uint8_t transport_peer,
                                     ସଂରଚନା connectdata *conn,
                                     ସଂରଚନା Curl_sockaddr_ex *addr,
                                     ସଂରଚନା Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  ସଂରଚନା cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  ଯଦି(!tunnel_peer)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଯଦି((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  result = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  ଯଦି(result)
    ଯାଅ out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  ଯଦି(result)
    ଯାଅ out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ଖାଲି, TRNSPRT_QUIC);
  ଯଦି(result)
    ଯାଅ out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : ଖାଲି;
  ଯଦି(result) {
    ଯଦି(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    ଅନ୍ୟଥା ଯଦି(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  ଅନ୍ୟଥା
    CURL_TRC_CF(data, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cf_ngtcp2_proxy_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                           ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା Curl_peer *origin,
                                           ସଂରଚନା Curl_peer *peer,
                                           ସଂରଚନା Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  ସଂରଚନା cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  (ଶୂନ୍ୟ)data;

  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx)
    ଯାଅ out;
  result = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  ଯଦି(result)
    ଯାଅ out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  ଯଦି(result)
    ଯାଅ out;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_ଭର୍ତ୍ତି_after(cf_at, cf);

out:
  ଯଦି(result) {
    ଯଦି(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    ଅନ୍ୟଥା ଯଦି(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  ଫେରନ୍ତୁ result;
}

#endif
