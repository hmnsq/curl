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

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
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
#define PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#define PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_STREAM_CHUNK_SIZE)

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

ಸಂರಚನಾ h3_tunnel_stream {
  ಸಂರಚನಾ Curl_peer *dest;  /* where the tunnel goes to */
  ಸಂರಚನಾ http_resp *resp;
  ಸಂರಚನಾ bufq recvbuf;
  ವರ್ಣ *authority;
  ಸಂರಚನಾ h3_stream_ctx *stream;
  h3_tunnel_state state;
  BIT(udp);
  BIT(has_final_response);
  BIT(closed);
};

ಸ್ಥಿರ CURLcode h3_tunnel_stream_init(ಸಂರಚನಾ h3_tunnel_stream *ts,
                                      ಸಂರಚನಾ Curl_peer *dest,
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
  ಯದಿ(!ts->authority)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_tunnel_stream_reset(ಸಂರಚನಾ h3_tunnel_stream *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = ರಿಕ್ತ;
  ts->stream = ರಿಕ್ತ;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_tunnel_stream_cleanup(ಸಂರಚನಾ h3_tunnel_stream *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_tunnel_go_state(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ h3_tunnel_stream *ts,
                               h3_tunnel_state new_state,
                               ಸಂರಚನಾ Curl_easy *data)
{
  VERBOSE(int64_t stream_id = ts->stream ? ts->stream->id : -1);
  (ಶೂನ್ಯ)cf;

  ಯದಿ(ts->state == new_state)
    ಪ್ರತ್ಯಾಗಮ;

  /* leaving this one */
  ವಿಕಲ್ಪ(ts->state) {
  ಪಕ್ಷ H3_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }

  /* entering this one */
  ವಿಕಲ್ಪ(new_state) {
  ಪಕ್ಷ H3_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [init]", stream_id);
    h3_tunnel_stream_reset(ts);
    ಭಙ್ಗ;
  ಪಕ್ಷ H3_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [connect]", stream_id);
    ts->state = H3_TUNNEL_CONNECT;
    ಭಙ್ಗ;
  ಪಕ್ಷ H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [response]", stream_id);
    ts->state = H3_TUNNEL_RESPONSE;
    ಭಙ್ಗ;
  ಪಕ್ಷ H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [established]", stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(data->req.hd_proxy_auth);
    ಭಙ್ಗ;
  ಪಕ್ಷ H3_TUNNEL_FAILED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [failed]", stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    ಭಙ್ಗ;
  }
}

ಸಂರಚನಾ cf_h3_proxy_ctx {
  ಸಂರಚನಾ cf_ngtcp2_ctx ngtcp2_ctx;
  ಸಂರಚನಾ h3_tunnel_stream tunnel; /* our tunnel CONNECT stream */
  BIT(connected);
};

ಸ್ಥಿರ CURLcode cf_ngtcp2_proxy_h3_init(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ cf_ngtcp2_ctx *ctx);

ಸ್ಥಿರ CURLcode cf_h3_proxy_ctx_init(ಸಂರಚನಾ cf_h3_proxy_ctx *ctx,
                                     ಸಂರಚನಾ Curl_peer *origin,
                                     ಸಂರಚನಾ Curl_peer *peer,
                                     ಸಂರಚನಾ ssl_filter_config *sslc,
                                     ಸಂರಚನಾ Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode result;
  result = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  ಯದಿ(!result)
    result = h3_tunnel_stream_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h3_proxy_ctx_free(ಸಂರಚನಾ cf_h3_proxy_ctx *ctx)
{
  ಯದಿ(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_stream_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                      uint64_t datalen, ಶೂನ್ಯ *user_data,
                                      ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream;
  size_t skiplen;
  (ಶೂನ್ಯ)stream_user_data;

  stream = pctx->tunnel.stream;
  ಯದಿ(!stream || (stream->id != stream_id))
    ಪ್ರತ್ಯಾಗಮ 0;

  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  ಯದಿ(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  ಅನ್ಯಥಾ
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  ಯದಿ(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    ಪೂರ್ಣಾಙ್ಕ rv = nghttp3_conn_resume_stream(conn, stream_id);
    ಯದಿ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_proxy_stream_close(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, ಶೂನ್ಯ *user_data,
                                    ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  ಸಂರಚನಾ h3_stream_ctx *stream;

  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream_user_data;
  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  ಯದಿ(!stream || (stream->id != stream_id))
    ಪ್ರತ್ಯಾಗಮ 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  ಯದಿ(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  ಅನ್ಯಥಾ
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  pctx->tunnel.stream = ರಿಕ್ತ;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(data);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h3_proxy_upd_rx_win(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ h3_stream_ctx *stream)
{
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  ಯದಿ(!stream) /* already closed */
    ಪ್ರತ್ಯಾಗಮ;
  ಯದಿ(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = stream->rx_offset_max - stream->rx_offset;
  ಯದಿ(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    ಯದಿ(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    ಯದಿ(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  stream->id, cur_win, rx_buffered, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
      ngtcp2_conn_extend_max_offset(ctx->qconn, delta);
    }
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_proxy_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                                 ಧ್ರುವ uint8_t *buf, size_t buflen,
                                 ಶೂನ್ಯ *user_data, ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  ಸಂರಚನಾ h3_stream_ctx *stream;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream3_id;
  (ಶೂನ್ಯ)stream_user_data;

  stream = pctx->tunnel.stream;
  ಯದಿ(!data || !stream || (stream->id != stream3_id)) {
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  stream->rx_total += (curl_off_t)buflen;
  CURL_TRC_CF(data, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              stream->id, buflen, stream->rx_total);

  result = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  ಯದಿ(result || (nwritten < buflen)) {
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  stream->rx_offset += buflen;
  ಯದಿ(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_h3_proxy_upd_rx_win(cf, data, stream);

  Curl_multi_mark_dirty(data);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t stream_id,
                                        size_t consumed, ಶೂನ್ಯ *user_data,
                                        ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream_user_data;

  ಯದಿ(!ctx)
    ಪ್ರತ್ಯಾಗಮ 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_proxy_recv_header(nghttp3_conn *conn, int64_t stream_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *ಮೌಲ್ಯ, uint8_t flags,
                                   ಶೂನ್ಯ *user_data, ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(ಮೌಲ್ಯ);
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  ಸಂರಚನಾ h3_stream_ctx *stream;
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ http_status;
  ಸಂರಚನಾ http_resp *resp;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream_id;
  (ಶೂನ್ಯ)token;
  (ಶೂನ್ಯ)flags;
  (ಶೂನ್ಯ)stream_user_data;

  /* stream_user_data might be NULL for control streams */
  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  ಯದಿ(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (ಶೂನ್ಯ *)data, data ? data->mid : 0);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(token == NGHTTP3_QPACK_TOKEN__STATUS) {
    result = Curl_http_decode_status(&stream->status_code,
                                     (ಧ್ರುವ ವರ್ಣ *)h3val.base, h3val.len);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
    http_status = stream->status_code;
    result = Curl_http_resp_make(&resp, http_status, ರಿಕ್ತ);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
    ಯದಿ(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(!pctx->tunnel.resp) {
      ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s", stream_id,
                (ಪೂರ್ಣಾಙ್ಕ)h3name.len, h3name.base, (ಪೂರ್ಣಾಙ್ಕ)h3val.len, h3val.base);
    result = Curl_dynhds_add(&pctx->tunnel.resp->headers,
                             (ಧ್ರುವ ವರ್ಣ *)h3name.base, h3name.len,
                             (ಧ್ರುವ ವರ್ಣ *)h3val.base, h3val.len);
    ಯದಿ(result) {
      ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_proxy_end_headers(nghttp3_conn *conn, int64_t stream_id,
                                   ಪೂರ್ಣಾಙ್ಕ fin, ಶೂನ್ಯ *user_data,
                                   ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  ಸಂರಚನಾ h3_stream_ctx *stream;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream_id;
  (ಶೂನ್ಯ)fin;
  (ಶೂನ್ಯ)stream_user_data;

  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  ಯದಿ(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (ಶೂನ್ಯ *)data, data ? data->mid : 0);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d", stream_id,
              stream->status_code);

  ಯದಿ(!pctx->tunnel.has_final_response) {
    ಯದಿ(stream->status_code / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  ಯದಿ(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(data);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, ಶೂನ್ಯ *user_data,
                                    ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream_user_data;

  ಯದಿ(ctx) {
    ಪೂರ್ಣಾಙ್ಕ rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                              app_error_code);

    ಯದಿ(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
      ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_h3_proxy_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, ಶೂನ್ಯ *user_data,
                                    ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  ಪೂರ್ಣಾಙ್ಕ rv;

  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream_user_data;
  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;

  ಯದಿ(!pctx->tunnel.stream ||
     (stream_id != pctx->tunnel.stream->id))
    ಪ್ರತ್ಯಾಗಮ 0;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  pctx->tunnel.stream = ರಿಕ್ತ;
  pctx->tunnel.closed = TRUE;
  ಯದಿ(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ nghttp3_ssize cb_h3_tunnel_read_data(nghttp3_conn *conn,
                                            int64_t stream_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            ಶೂನ್ಯ *user_data,
                                            ಶೂನ್ಯ *stream_user_data)
{
  ಸಂರಚನಾ Curl_cfilter *cf = user_data;
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  ಸಂರಚನಾ h3_stream_ctx *stream;
  size_t nwritten = 0;
  size_t nvecs = 0;
  ಧ್ರುವ unsigned ವರ್ಣ *buf_base;

  (ಶೂನ್ಯ)conn;
  (ಶೂನ್ಯ)stream_id;
  (ಶೂನ್ಯ)veccnt;
  (ಶೂನ್ಯ)stream_user_data;
  (ಶೂನ್ಯ)pflags;

  stream = pctx->tunnel.stream;
  ಯದಿ(!data || !stream || (stream->id != stream_id))
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  ಯದಿ(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    ಯಾವತ್(nvecs < veccnt) {
      ಯದಿ(!Curl_bufq_peek_at(&stream->sendbuf,
                           stream->tx_in_flight_size,
                           &buf_base,
                           &vec[nvecs].len))
        ಭಙ್ಗ;
      vec[nvecs].base = (uint8_t *)(uintptr_t)buf_base;
      stream->tx_in_flight_size += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  ಯದಿ(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN",
                stream->id);
    ಪ್ರತ್ಯಾಗಮ NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf));
  ಪ್ರತ್ಯಾಗಮ (nghttp3_ssize)nvecs;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
ಸ್ಥಿರ nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_stream_close,
  cb_h3_proxy_recv_data,
  cb_h3_proxy_deferred_consume,
  ರಿಕ್ತ, /* begin_headers */
  cb_h3_proxy_recv_header,
  cb_h3_proxy_end_headers,
  ರಿಕ್ತ, /* begin_trailers */
  cb_h3_proxy_recv_header,
  ರಿಕ್ತ, /* end_trailers */
  cb_h3_proxy_stop_sending,
  ರಿಕ್ತ, /* end_stream */
  cb_h3_proxy_reset_stream,
  ರಿಕ್ತ, /* shutdown */
  ರಿಕ್ತ, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  ರಿಕ್ತ, /* recv_origin */
  ರಿಕ್ತ, /* end_origin */
  ರಿಕ್ತ, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  ರಿಕ್ತ, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  ರಿಕ್ತ, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ತೆಗೆ
#endif

ಸ್ಥಿರ CURLcode cf_ngtcp2_proxy_h3_init(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ cf_ngtcp2_ctx *ctx)
{
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(ngtcp2_conn_get_streams_uni_ಎಡ(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  ಯದಿ(rc) {
    failf(data, "error creating nghttp3 connection instance");
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಪ್ರತ್ಯಾಗಮ Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

ಸ್ಥಿರ ssize_t cf_h3_proxy_recv_closed_stream(ಸಂರಚನಾ Curl_cfilter *cf,
                                              ಸಂರಚನಾ Curl_easy *data,
                                              ಸಂರಚನಾ h3_stream_ctx *stream,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  ಯದಿ(stream->reset) {
    ಯದಿ(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn);
      data->state.refused_stream = TRUE;
      *err = CURLE_RECV_ERROR;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", stream->id, stream->error3,
                  Curl_vquic_h3_err_str(stream->error3));
      nread = 0;
      ಗಚ್ಛ out;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    *err = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ ಯದಿ(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    *err = CURLE_HTTP3;
    ಗಚ್ಛ out;
  }
  nread = 0;

out:
  ಪ್ರತ್ಯಾಗಮ nread;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_sendbuf_add(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ h3_stream_ctx *stream,
                                        ಧ್ರುವ uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode result;
  *pnwritten = 0;
  (ಶೂನ್ಯ)data;

  result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_send(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಧ್ರುವ uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = ರಿಕ್ತ;
  ಸಂರಚನಾ cf_call_data save;
  ಸಂರಚನಾ cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  ಯದಿ(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    ಗಚ್ಛ denied;
  }

  (ಶೂನ್ಯ)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ಯದಿ(pctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ denied;
  }

  stream = pctx->tunnel.stream;
  ಯದಿ(!stream) {
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ denied;
  }

  ಯದಿ(stream->closed) {
    ಯದಿ(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      ಗಚ್ಛ out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ {
    result = cf_h3_proxy_sendbuf_add(data, stream, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    (ಶೂನ್ಯ)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  ಯದಿ(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* incoming data frames on the h3 stream */
ಸ್ಥಿರ CURLcode cf_h3_proxy_recv(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ವರ್ಣ *buf, size_t len, size_t *pnread)
{
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = pctx->tunnel.stream;
  ಸಂರಚನಾ cf_call_data save;
  ಸಂರಚನಾ cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  ಯದಿ(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    ಗಚ್ಛ denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  ಯದಿ(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    ಗಚ್ಛ out;
  }

  ಯದಿ(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* inbufq had nothing before, maybe after progressing ingress? */
  ಯದಿ(!*pnread && !Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    ಯದಿ(result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  stream->id, len, *pnread, (ಪೂರ್ಣಾಙ್ಕ)result);
      ಗಚ್ಛ out;
    }
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  ಯದಿ(*pnread) {
    Curl_multi_mark_dirty(data);
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(stream->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_stream(cf, data, stream, &result);
      ಯದಿ(nread > 0)
        *pnread = (size_t)nread;
      ಗಚ್ಛ out;
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
              stream ? stream->id : -1, len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_submit(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ h3_tunnel_stream *ts,
                                   ಸಂರಚನಾ httpreq *req)
{
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = ರಿಕ್ತ;
  ಸಂರಚನಾ dynhds h2_headers;
  nghttp3_nv *nva = ರಿಕ್ತ;
  size_t nheader;
  ಪೂರ್ಣಾಙ್ಕ rc = 0;
  unsigned ಪೂರ್ಣಾಙ್ಕ i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = ರಿಕ್ತ;
  CURLcode result;

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  nheader = Curl_dynhds_ಎಣಿಕೆ(&h2_headers);
  nva = curlx_malloc(ಪರಿಮಾಣ(nghttp3_nv) * nheader);
  ಯದಿ(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  ಪ್ರತಿ(i = 0; i < nheader; ++i) {
    ಸಂರಚನಾ dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned ವರ್ಣ *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].ಮೌಲ್ಯ = (unsigned ವರ್ಣ *)e->ಮೌಲ್ಯ;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  /* Open a bidirectional stream */
  {
    int64_t sid;
    ಪೂರ್ಣಾಙ್ಕ rv;

    DEBUGASSERT(stream->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, ರಿಕ್ತ);
    ಯದಿ(rv) {
      failf(data, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
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

  ಯದಿ(rc) {
    ವಿಕಲ್ಪ(rc) {
    ಪಕ್ಷ NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  stream->id, rc, nghttp3_strerror(rc));
      ಭಙ್ಗ;
    }
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ out;
  }

  ಯದಿ(Curl_trc_is_verbose(data)) {
    CURL_TRC_CF(data, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", stream->id,
                Curl_bufref_ptr(&data->state.url));
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                           ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  ಯದಿ(!ctx->qconn || !pctx->tunnel.stream || (sock == CURL_SOCKET_BAD))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  ಯದಿ(want_recv || want_send || !Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    ಸಂರಚನಾ h3_stream_ctx *stream = pctx->tunnel.stream;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_ಎಡ(ctx->qconn) ||
                 !ngtcp2_conn_get_max_data_ಎಡ(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                stream->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool cf_h3_proxy_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಯದಿ(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf))
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_submit_CONNECT(ಸಂರಚನಾ Curl_cfilter *cf,
                                           ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ h3_tunnel_stream *ts)
{
  ಸಂರಚನಾ cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  ಸಂರಚನಾ httpreq *req = ರಿಕ್ತ;

  result = Curl_http_proxy_create_tunnel_request(
    &req, cf, data, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  ಯದಿ(!result)
    result = Curl_creader_set_null(data);
  ಯದಿ(!result)
    result = cf_h3_proxy_submit(cf, data, ts, req);

  ಯದಿ(req)
    Curl_http_req_free(req);
  ಯದಿ(result)
    failf(data, "Failed sending CONNECT to proxy");
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_inspect_response(ಸಂರಚನಾ Curl_cfilter *cf,
                                             ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ h3_tunnel_stream *ts)
{
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)pctx->tunnel.udp, &res);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ವಿಕಲ್ಪ(res) {
  ಪಕ್ಷ PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, data);
    ಭಙ್ಗ;
  ಪಕ್ಷ PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
    result = CURLE_COULDNT_CONNECT;
    ಭಙ್ಗ;
  ಪಕ್ಷ PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, data);
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_tunnel(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ h3_tunnel_stream *ts,
                                   bool *pdone)
{
  ಸಂರಚನಾ cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  ಕುರು {
    ವಿಕಲ್ಪ(ts->state) {
    ಪಕ್ಷ H3_TUNNEL_INIT:
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = cf_h3_proxy_submit_CONNECT(cf, data, ts);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, data);

      result = Curl_cf_ngtcp2_progress_egress(cf, data, ರಿಕ್ತ);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      FALLTHROUGH();

    ಪಕ್ಷ H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, ರಿಕ್ತ);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      result = Curl_cf_ngtcp2_progress_egress(cf, data, ರಿಕ್ತ);
      ಯದಿ(result && result != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
        ಗಚ್ಛ out;
      }

      ಯದಿ(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, data);
      }
      ಅನ್ಯಥಾ {
        /* Not done yet, return and let multi interface call us again */
        result = CURLE_OK;
        ಗಚ್ಛ out;
      }
      FALLTHROUGH();

    ಪಕ್ಷ H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = cf_h3_proxy_inspect_response(cf, data, ts);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      ctx->connected = TRUE;
      ಭಙ್ಗ;

    ಪಕ್ಷ H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;

    ಪಕ್ಷ H3_TUNNEL_FAILED:
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;

    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }

  } ಯಾವತ್(ts->state == H3_TUNNEL_INIT);

out:
  ಯದಿ((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ cf_h3_proxy_ctx *pctx = cf->ctx;
  ಸಂರಚನಾ cf_call_data save = { 0 };
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ h3_tunnel_stream *ts = &pctx->tunnel;
  bool data_saved = FALSE;

  result = Curl_cf_ngtcp2_cmn_connect(cf, data, done);
  ಯದಿ(result || !*done)
    ಗಚ್ಛ out;

  CF_DATA_SAVE(save, cf, data);
  data_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  result = cf_h3_proxy_tunnel(cf, data, ts, done);

out:
  ಯದಿ(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }

  ಯದಿ(data_saved)
    CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h3_proxy_destroy(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h3_proxy_ctx *ctx = cf->ctx;

  (ಶೂನ್ಯ)data;
  ಯದಿ(ctx) {
    CURL_TRC_CF(data, cf, "cf_h3_proxy_destroy()");
    ಯದಿ(ctx->ngtcp2_ctx.qconn) {
      ಸಂರಚನಾ cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = ರಿಕ್ತ;
  }
}

ಸ್ಥಿರ CURLcode cf_h3_proxy_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಪ್ರತ್ಯಾಗಮ Curl_cf_ngtcp2_cmn_shutdown(cf, data, done);
}

ಸಂರಚನಾ Curl_cftype Curl_cft_h3_proxy = {
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

CURLcode Curl_cf_ngtcp2_proxy_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                                     ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ Curl_peer *origin,
                                     ಸಂರಚನಾ Curl_peer *peer,
                                     uint8_t transport_peer,
                                     ಸಂರಚನಾ connectdata *conn,
                                     ಸಂರಚನಾ Curl_sockaddr_ex *addr,
                                     ಸಂರಚನಾ Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  ಸಂರಚನಾ cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  ಯದಿ(!tunnel_peer)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಯದಿ((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  result = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ರಿಕ್ತ, TRNSPRT_QUIC);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : ರಿಕ್ತ;
  ಯದಿ(result) {
    ಯದಿ(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    ಅನ್ಯಥಾ ಯದಿ(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  ಅನ್ಯಥಾ
    CURL_TRC_CF(data, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_ngtcp2_proxy_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                           ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ Curl_peer *origin,
                                           ಸಂರಚನಾ Curl_peer *peer,
                                           ಸಂರಚನಾ Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  ಸಂರಚನಾ cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  (ಶೂನ್ಯ)data;

  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx)
    ಗಚ್ಛ out;
  result = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);

out:
  ಯದಿ(result) {
    ಯದಿ(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    ಅನ್ಯಥಾ ಯದಿ(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif
