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
#অন্তর্ভাৱ "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "curlx/dynbuf.h"
#অন্তর্ভাৱ "http_proxy.h"
#অন্তর্ভাৱ "vquic/vquic.h"
#অন্তর্ভাৱ "vquic/cf-ngtcp2-cmn.h"
#অন্তর্ভাৱ "vquic/cf-ngtcp2-proxy.h"
#অন্তর্ভাৱ "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#define PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#define PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_STREAM_CHUNK_SIZE)

প্রকারনির্দেশ গণনা {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

সংরচনা h3_tunnel_stream {
  সংরচনা Curl_peer *dest;  /* where the tunnel goes to */
  সংরচনা http_resp *resp;
  সংরচনা bufq recvbuf;
  ৱর্ণ *authority;
  সংরচনা h3_stream_ctx *stream;
  h3_tunnel_state state;
  BIT(udp);
  BIT(has_final_response);
  BIT(closed);
};

স্থির CURLcode h3_tunnel_stream_init(সংরচনা h3_tunnel_stream *ts,
                                      সংরচনা Curl_peer *dest,
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
  যদি(!ts->authority)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  ঘূৰাই CURLE_OK;
}

স্থির শূন্য h3_tunnel_stream_reset(সংরচনা h3_tunnel_stream *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = রিক্ত;
  ts->stream = রিক্ত;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

স্থির শূন্য h3_tunnel_stream_cleanup(সংরচনা h3_tunnel_stream *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

স্থির শূন্য h3_tunnel_go_state(সংরচনা Curl_cfilter *cf,
                               সংরচনা h3_tunnel_stream *ts,
                               h3_tunnel_state new_state,
                               সংরচনা Curl_easy *data)
{
  VERBOSE(int64_t stream_id = ts->stream ? ts->stream->id : -1);
  (শূন্য)cf;

  যদি(ts->state == new_state)
    ঘূৰাই;

  /* leaving this one */
  ৱিকল্প(ts->state) {
  পক্ষ H3_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    ভঙ্গ;
  স্ৱভাৱ:
    ভঙ্গ;
  }

  /* entering this one */
  ৱিকল্প(new_state) {
  পক্ষ H3_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [init]", stream_id);
    h3_tunnel_stream_reset(ts);
    ভঙ্গ;
  পক্ষ H3_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [connect]", stream_id);
    ts->state = H3_TUNNEL_CONNECT;
    ভঙ্গ;
  পক্ষ H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [response]", stream_id);
    ts->state = H3_TUNNEL_RESPONSE;
    ভঙ্গ;
  পক্ষ H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [established]", stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(data->req.hd_proxy_auth);
    ভঙ্গ;
  পক্ষ H3_TUNNEL_FAILED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [failed]", stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    ভঙ্গ;
  }
}

সংরচনা cf_h3_proxy_ctx {
  সংরচনা cf_ngtcp2_ctx ngtcp2_ctx;
  সংরচনা h3_tunnel_stream tunnel; /* our tunnel CONNECT stream */
  BIT(connected);
};

স্থির CURLcode cf_ngtcp2_proxy_h3_init(সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data,
                                        সংরচনা cf_ngtcp2_ctx *ctx);

স্থির CURLcode cf_h3_proxy_ctx_init(সংরচনা cf_h3_proxy_ctx *ctx,
                                     সংরচনা Curl_peer *origin,
                                     সংরচনা Curl_peer *peer,
                                     সংরচনা ssl_filter_config *sslc,
                                     সংরচনা Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode result;
  result = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  যদি(!result)
    result = h3_tunnel_stream_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  ঘূৰাই result;
}

স্থির শূন্য cf_h3_proxy_ctx_free(সংরচনা cf_h3_proxy_ctx *ctx)
{
  যদি(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_stream_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

স্থির পূর্ণাঙ্ক cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                      uint64_t datalen, শূন্য *user_data,
                                      শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream;
  size_t skiplen;
  (শূন্য)stream_user_data;

  stream = pctx->tunnel.stream;
  যদি(!stream || (stream->id != stream_id))
    ঘূৰাই 0;

  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  যদি(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  অন্যথা
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  যদি(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    পূর্ণাঙ্ক rv = nghttp3_conn_resume_stream(conn, stream_id);
    যদি(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক cb_h3_proxy_stream_close(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, শূন্য *user_data,
                                    শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  সংরচনা h3_stream_ctx *stream;

  (শূন্য)conn;
  (শূন্য)stream_user_data;
  যদি(!data)
    ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  যদি(!stream || (stream->id != stream_id))
    ঘূৰাই 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  যদি(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  অন্যথা
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  pctx->tunnel.stream = রিক্ত;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(data);
  ঘূৰাই 0;
}

স্থির শূন্য cf_h3_proxy_upd_rx_win(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা h3_stream_ctx *stream)
{
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  যদি(!stream) /* already closed */
    ঘূৰাই;
  যদি(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    ঘূৰাই;
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
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  stream->id, cur_win, rx_buffered, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
      ngtcp2_conn_extend_max_offset(ctx->qconn, delta);
    }
  }
}

স্থির পূর্ণাঙ্ক cb_h3_proxy_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                                 ধ্রুৱ uint8_t *buf, size_t buflen,
                                 শূন্য *user_data, শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  সংরচনা h3_stream_ctx *stream;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  (শূন্য)conn;
  (শূন্য)stream3_id;
  (শূন্য)stream_user_data;

  stream = pctx->tunnel.stream;
  যদি(!data || !stream || (stream->id != stream3_id)) {
    ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  stream->rx_total += (curl_off_t)buflen;
  CURL_TRC_CF(data, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              stream->id, buflen, stream->rx_total);

  result = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  যদি(result || (nwritten < buflen)) {
    ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  stream->rx_offset += buflen;
  যদি(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_h3_proxy_upd_rx_win(cf, data, stream);

  Curl_multi_mark_dirty(data);
  ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t stream_id,
                                        size_t consumed, শূন্য *user_data,
                                        শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (শূন্য)conn;
  (শূন্য)stream_user_data;

  যদি(!ctx)
    ঘূৰাই 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক cb_h3_proxy_recv_header(nghttp3_conn *conn, int64_t stream_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *মান, uint8_t flags,
                                   শূন্য *user_data, শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(মান);
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  সংরচনা h3_stream_ctx *stream;
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক http_status;
  সংরচনা http_resp *resp;
  (শূন্য)conn;
  (শূন্য)stream_id;
  (শূন্য)token;
  (শূন্য)flags;
  (শূন্য)stream_user_data;

  /* stream_user_data might be NULL for control streams */
  যদি(!data)
    ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  যদি(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (শূন্য *)data, data ? data->mid : 0);
    ঘূৰাই 0;
  }

  যদি(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    ঘূৰাই 0;
  }

  যদি(token == NGHTTP3_QPACK_TOKEN__STATUS) {
    result = Curl_http_decode_status(&stream->status_code,
                                     (ধ্রুৱ ৱর্ণ *)h3val.base, h3val.len);
    যদি(result)
      ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;
    http_status = stream->status_code;
    result = Curl_http_resp_make(&resp, http_status, রিক্ত);
    যদি(result)
      ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;
    যদি(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  অন্যথা {
    যদি(!pctx->tunnel.resp) {
      ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s", stream_id,
                (পূর্ণাঙ্ক)h3name.len, h3name.base, (পূর্ণাঙ্ক)h3val.len, h3val.base);
    result = Curl_dynhds_add(&pctx->tunnel.resp->headers,
                             (ধ্রুৱ ৱর্ণ *)h3name.base, h3name.len,
                             (ধ্রুৱ ৱর্ণ *)h3val.base, h3val.len);
    যদি(result) {
      ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক cb_h3_proxy_end_headers(nghttp3_conn *conn, int64_t stream_id,
                                   পূর্ণাঙ্ক fin, শূন্য *user_data,
                                   শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  সংরচনা h3_stream_ctx *stream;
  (শূন্য)conn;
  (শূন্য)stream_id;
  (শূন্য)fin;
  (শূন্য)stream_user_data;

  যদি(!data)
    ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  যদি(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (শূন্য *)data, data ? data->mid : 0);
    ঘূৰাই 0;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d", stream_id,
              stream->status_code);

  যদি(!pctx->tunnel.has_final_response) {
    যদি(stream->status_code / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  যদি(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(data);
  ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, শূন্য *user_data,
                                    শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (শূন্য)conn;
  (শূন্য)stream_user_data;

  যদি(ctx) {
    পূর্ণাঙ্ক rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                              app_error_code);

    যদি(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
      ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক cb_h3_proxy_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, শূন্য *user_data,
                                    শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  পূর্ণাঙ্ক rv;

  (শূন্য)conn;
  (শূন্য)stream_user_data;
  যদি(!data)
    ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;

  যদি(!pctx->tunnel.stream ||
     (stream_id != pctx->tunnel.stream->id))
    ঘূৰাই 0;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  pctx->tunnel.stream = রিক্ত;
  pctx->tunnel.closed = TRUE;
  যদি(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  ঘূৰাই 0;
}

স্থির nghttp3_ssize cb_h3_tunnel_read_data(nghttp3_conn *conn,
                                            int64_t stream_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            শূন্য *user_data,
                                            শূন্য *stream_user_data)
{
  সংরচনা Curl_cfilter *cf = user_data;
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  সংরচনা h3_stream_ctx *stream;
  size_t nwritten = 0;
  size_t nvecs = 0;
  ধ্রুৱ unsigned ৱর্ণ *buf_base;

  (শূন্য)conn;
  (শূন্য)stream_id;
  (শূন্য)veccnt;
  (শূন্য)stream_user_data;
  (শূন্য)pflags;

  stream = pctx->tunnel.stream;
  যদি(!data || !stream || (stream->id != stream_id))
    ঘূৰাই NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  যদি(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    যেতিয়ালৈ(nvecs < veccnt) {
      যদি(!Curl_bufq_peek_at(&stream->sendbuf,
                           stream->tx_in_flight_size,
                           &buf_base,
                           &vec[nvecs].len))
        ভঙ্গ;
      vec[nvecs].base = (uint8_t *)(uintptr_t)buf_base;
      stream->tx_in_flight_size += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  যদি(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN",
                stream->id);
    ঘূৰাই NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf));
  ঘূৰাই (nghttp3_ssize)nvecs;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
স্থির nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_stream_close,
  cb_h3_proxy_recv_data,
  cb_h3_proxy_deferred_consume,
  রিক্ত, /* begin_headers */
  cb_h3_proxy_recv_header,
  cb_h3_proxy_end_headers,
  রিক্ত, /* begin_trailers */
  cb_h3_proxy_recv_header,
  রিক্ত, /* end_trailers */
  cb_h3_proxy_stop_sending,
  রিক্ত, /* end_stream */
  cb_h3_proxy_reset_stream,
  রিক্ত, /* shutdown */
  রিক্ত, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  রিক্ত, /* recv_origin */
  রিক্ত, /* end_origin */
  রিক্ত, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  রিক্ত, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  রিক্ত, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic বেৰ_কৰো
#endif

স্থির CURLcode cf_ngtcp2_proxy_h3_init(সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data,
                                        সংরচনা cf_ngtcp2_ctx *ctx)
{
  পূর্ণাঙ্ক rc;

  যদি(ngtcp2_conn_get_streams_uni_বাম(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    ঘূৰাই CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  যদি(rc) {
    failf(data, "error creating nghttp3 connection instance");
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  ঘূৰাই Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

স্থির ssize_t cf_h3_proxy_recv_closed_stream(সংরচনা Curl_cfilter *cf,
                                              সংরচনা Curl_easy *data,
                                              সংরচনা h3_stream_ctx *stream,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  যদি(stream->reset) {
    যদি(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn);
      data->state.refused_stream = TRUE;
      *err = CURLE_RECV_ERROR;
      গচ্ছ out;
    }
    অন্যথা যদি(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", stream->id, stream->error3,
                  Curl_vquic_h3_err_str(stream->error3));
      nread = 0;
      গচ্ছ out;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    *err = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    গচ্ছ out;
  }
  অন্যথা যদি(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    *err = CURLE_HTTP3;
    গচ্ছ out;
  }
  nread = 0;

out:
  ঘূৰাই nread;
}

স্থির CURLcode cf_h3_proxy_sendbuf_add(সংরচনা Curl_easy *data,
                                        সংরচনা h3_stream_ctx *stream,
                                        ধ্রুৱ uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode result;
  *pnwritten = 0;
  (শূন্য)data;

  result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
  ঘূৰাই result;
}

স্থির CURLcode cf_h3_proxy_send(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 ধ্রুৱ uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  সংরচনা h3_stream_ctx *stream = রিক্ত;
  সংরচনা cf_call_data save;
  সংরচনা cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  যদি(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    গচ্ছ denied;
  }

  (শূন্য)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  যদি(result)
    গচ্ছ out;

  যদি(pctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    গচ্ছ denied;
  }

  stream = pctx->tunnel.stream;
  যদি(!stream) {
    result = CURLE_SEND_ERROR;
    গচ্ছ denied;
  }

  যদি(stream->closed) {
    যদি(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      গচ্ছ out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    গচ্ছ out;
  }
  অন্যথা {
    result = cf_h3_proxy_sendbuf_add(data, stream, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (পূর্ণাঙ্ক)result, *pnwritten);
    যদি(result)
      গচ্ছ out;
    (শূন্য)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  যদি(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (পূর্ণাঙ্ক)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  ঘূৰাই result;
}

/* incoming data frames on the h3 stream */
স্থির CURLcode cf_h3_proxy_recv(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 ৱর্ণ *buf, size_t len, size_t *pnread)
{
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  সংরচনা h3_stream_ctx *stream = pctx->tunnel.stream;
  সংরচনা cf_call_data save;
  সংরচনা cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  যদি(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    গচ্ছ denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  যদি(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    গচ্ছ out;
  }

  যদি(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    যদি(result)
      গচ্ছ out;
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  যদি(result)
    গচ্ছ out;

  /* inbufq had nothing before, maybe after progressing ingress? */
  যদি(!*pnread && !Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    যদি(result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  stream->id, len, *pnread, (পূর্ণাঙ্ক)result);
      গচ্ছ out;
    }
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  যদি(*pnread) {
    Curl_multi_mark_dirty(data);
  }
  অন্যথা {
    যদি(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      গচ্ছ out;
    }
    অন্যথা যদি(stream->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_stream(cf, data, stream, &result);
      যদি(nread > 0)
        *pnread = (size_t)nread;
      গচ্ছ out;
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
              stream ? stream->id : -1, len, (পূর্ণাঙ্ক)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  ঘূৰাই result;
}

স্থির CURLcode cf_h3_proxy_submit(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা h3_tunnel_stream *ts,
                                   সংরচনা httpreq *req)
{
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  সংরচনা h3_stream_ctx *stream = রিক্ত;
  সংরচনা dynhds h2_headers;
  nghttp3_nv *nva = রিক্ত;
  size_t nheader;
  পূর্ণাঙ্ক rc = 0;
  unsigned পূর্ণাঙ্ক i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = রিক্ত;
  CURLcode result;

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  যদি(result)
    গচ্ছ out;

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  যদি(result)
    গচ্ছ out;

  nheader = Curl_dynhds_গুনতি(&h2_headers);
  nva = curlx_malloc(পরিমাণ(nghttp3_nv) * nheader);
  যদি(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  বাবে(i = 0; i < nheader; ++i) {
    সংরচনা dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned ৱর্ণ *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].মান = (unsigned ৱর্ণ *)e->মান;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  /* Open a bidirectional stream */
  {
    int64_t sid;
    পূর্ণাঙ্ক rv;

    DEBUGASSERT(stream->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, রিক্ত);
    যদি(rv) {
      failf(data, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
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

  যদি(rc) {
    ৱিকল্প(rc) {
    পক্ষ NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      ভঙ্গ;
    স্ৱভাৱ:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  stream->id, rc, nghttp3_strerror(rc));
      ভঙ্গ;
    }
    result = CURLE_SEND_ERROR;
    গচ্ছ out;
  }

  যদি(Curl_trc_is_verbose(data)) {
    CURL_TRC_CF(data, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", stream->id,
                Curl_bufref_ptr(&data->state.url));
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  ঘূৰাই result;
}

স্থির CURLcode cf_h3_proxy_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                           সংরচনা Curl_easy *data,
                                           সংরচনা easy_pollset *ps)
{
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  যদি(!ctx->qconn || !pctx->tunnel.stream || (sock == CURL_SOCKET_BAD))
    ঘূৰাই CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  যদি(want_recv || want_send || !Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    সংরচনা h3_stream_ctx *stream = pctx->tunnel.stream;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_বাম(ctx->qconn) ||
                 !ngtcp2_conn_get_max_data_বাম(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                stream->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
  }
  ঘূৰাই result;
}

স্থির bool cf_h3_proxy_data_pending(সংরচনা Curl_cfilter *cf,
                                     ধ্রুৱ সংরচনা Curl_easy *data)
{
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  যদি(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf))
    ঘূৰাই TRUE;
  ঘূৰাই cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

স্থির CURLcode cf_h3_proxy_submit_CONNECT(সংরচনা Curl_cfilter *cf,
                                           সংরচনা Curl_easy *data,
                                           সংরচনা h3_tunnel_stream *ts)
{
  সংরচনা cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  সংরচনা httpreq *req = রিক্ত;

  result = Curl_http_proxy_create_tunnel_request(
    &req, cf, data, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  যদি(!result)
    result = Curl_creader_set_null(data);
  যদি(!result)
    result = cf_h3_proxy_submit(cf, data, ts, req);

  যদি(req)
    Curl_http_req_free(req);
  যদি(result)
    failf(data, "Failed sending CONNECT to proxy");
  ঘূৰাই result;
}

স্থির CURLcode cf_h3_proxy_inspect_response(সংরচনা Curl_cfilter *cf,
                                             সংরচনা Curl_easy *data,
                                             সংরচনা h3_tunnel_stream *ts)
{
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)pctx->tunnel.udp, &res);
  যদি(result)
    ঘূৰাই result;
  ৱিকল্প(res) {
  পক্ষ PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, data);
    ভঙ্গ;
  পক্ষ PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
    result = CURLE_COULDNT_CONNECT;
    ভঙ্গ;
  পক্ষ PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, data);
    ভঙ্গ;
  }
  ঘূৰাই result;
}

স্থির CURLcode cf_h3_proxy_tunnel(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা h3_tunnel_stream *ts,
                                   bool *pdone)
{
  সংরচনা cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  কৰা {
    ৱিকল্প(ts->state) {
    পক্ষ H3_TUNNEL_INIT:
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = cf_h3_proxy_submit_CONNECT(cf, data, ts);
      যদি(result)
        গচ্ছ out;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, data);

      result = Curl_cf_ngtcp2_progress_egress(cf, data, রিক্ত);
      যদি(result)
        গচ্ছ out;
      FALLTHROUGH();

    পক্ষ H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, রিক্ত);
      যদি(result)
        গচ্ছ out;
      result = Curl_cf_ngtcp2_progress_egress(cf, data, রিক্ত);
      যদি(result && result != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
        গচ্ছ out;
      }

      যদি(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, data);
      }
      অন্যথা {
        /* Not done yet, return and let multi interface call us again */
        result = CURLE_OK;
        গচ্ছ out;
      }
      FALLTHROUGH();

    পক্ষ H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = cf_h3_proxy_inspect_response(cf, data, ts);
      যদি(result)
        গচ্ছ out;
      ctx->connected = TRUE;
      ভঙ্গ;

    পক্ষ H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      ঘূৰাই CURLE_OK;

    পক্ষ H3_TUNNEL_FAILED:
      ঘূৰাই CURLE_RECV_ERROR;

    স্ৱভাৱ:
      ভঙ্গ;
    }

  } যেতিয়ালৈ(ts->state == H3_TUNNEL_INIT);

out:
  যদি((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
  ঘূৰাই result;
}

স্থির CURLcode cf_h3_proxy_connect(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা cf_h3_proxy_ctx *pctx = cf->ctx;
  সংরচনা cf_call_data save = { 0 };
  CURLcode result = CURLE_OK;
  সংরচনা h3_tunnel_stream *ts = &pctx->tunnel;
  bool data_saved = FALSE;

  result = Curl_cf_ngtcp2_cmn_connect(cf, data, done);
  যদি(result || !*done)
    গচ্ছ out;

  CF_DATA_SAVE(save, cf, data);
  data_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  result = cf_h3_proxy_tunnel(cf, data, ts, done);

out:
  যদি(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }

  যদি(data_saved)
    CF_DATA_RESTORE(cf, save);
  ঘূৰাই result;
}

স্থির শূন্য cf_h3_proxy_destroy(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data)
{
  সংরচনা cf_h3_proxy_ctx *ctx = cf->ctx;

  (শূন্য)data;
  যদি(ctx) {
    CURL_TRC_CF(data, cf, "cf_h3_proxy_destroy()");
    যদি(ctx->ngtcp2_ctx.qconn) {
      সংরচনা cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = রিক্ত;
  }
}

স্থির CURLcode cf_h3_proxy_shutdown(সংরচনা Curl_cfilter *cf,
                                     সংরচনা Curl_easy *data, bool *done)
{
  ঘূৰাই Curl_cf_ngtcp2_cmn_shutdown(cf, data, done);
}

সংরচনা Curl_cftype Curl_cft_h3_proxy = {
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

CURLcode Curl_cf_ngtcp2_proxy_create(সংরচনা Curl_cfilter **pcf,
                                     সংরচনা Curl_easy *data,
                                     সংরচনা Curl_peer *origin,
                                     সংরচনা Curl_peer *peer,
                                     uint8_t transport_peer,
                                     সংরচনা connectdata *conn,
                                     সংরচনা Curl_sockaddr_ex *addr,
                                     সংরচনা Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  সংরচনা Curl_cfilter *cf = রিক্ত;
  সংরচনা cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  যদি(!tunnel_peer)
    ঘূৰাই CURLE_FAILED_INIT;
  যদি((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    ঘূৰাই CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  result = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  যদি(result)
    গচ্ছ out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  যদি(result)
    গচ্ছ out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, রিক্ত, TRNSPRT_QUIC);
  যদি(result)
    গচ্ছ out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : রিক্ত;
  যদি(result) {
    যদি(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    অন্যথা যদি(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  অন্যথা
    CURL_TRC_CF(data, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  ঘূৰাই result;
}

CURLcode Curl_cf_ngtcp2_proxy_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                           সংরচনা Curl_easy *data,
                                           সংরচনা Curl_peer *origin,
                                           সংরচনা Curl_peer *peer,
                                           সংরচনা Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  সংরচনা Curl_cfilter *cf = রিক্ত;
  সংরচনা cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  (শূন্য)data;

  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx)
    গচ্ছ out;
  result = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  যদি(result)
    গচ্ছ out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  যদি(result)
    গচ্ছ out;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);

out:
  যদি(result) {
    যদি(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    অন্যথা যদি(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  ঘূৰাই result;
}

#endif
