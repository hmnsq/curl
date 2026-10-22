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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "http_proxy.h"
#அந்தர்பாவ "vquic/vquic.h"
#அந்தர்பாவ "vquic/cf-ngtcp2-cmn.h"
#அந்தர்பாவ "vquic/cf-ngtcp2-proxy.h"
#அந்தர்பாவ "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#define PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#define PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_STREAM_CHUNK_SIZE)

வகைவரை எண்ணிக்கை {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

கட்டமைப்பு h3_tunnel_stream {
  கட்டமைப்பு Curl_peer *dest;  /* where the tunnel goes to */
  கட்டமைப்பு http_resp *resp;
  கட்டமைப்பு bufq recvbuf;
  எழுத்து *authority;
  கட்டமைப்பு h3_stream_ctx *stream;
  h3_tunnel_state state;
  BIT(udp);
  BIT(has_final_response);
  BIT(closed);
};

நிலையான CURLcode h3_tunnel_stream_init(கட்டமைப்பு h3_tunnel_stream *ts,
                                      கட்டமைப்பு Curl_peer *dest,
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
  எனில்(!ts->authority)
    திரும்பு CURLE_OUT_OF_MEMORY;

  திரும்பு CURLE_OK;
}

நிலையான வெறுமை h3_tunnel_stream_reset(கட்டமைப்பு h3_tunnel_stream *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = வெற்று;
  ts->stream = வெற்று;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

நிலையான வெறுமை h3_tunnel_stream_cleanup(கட்டமைப்பு h3_tunnel_stream *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

நிலையான வெறுமை h3_tunnel_go_state(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு h3_tunnel_stream *ts,
                               h3_tunnel_state new_state,
                               கட்டமைப்பு Curl_easy *data)
{
  VERBOSE(int64_t stream_id = ts->stream ? ts->stream->id : -1);
  (வெறுமை)cf;

  எனில்(ts->state == new_state)
    திரும்பு;

  /* leaving this one */
  தேர்வு(ts->state) {
  நிலை H3_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }

  /* entering this one */
  தேர்வு(new_state) {
  நிலை H3_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [init]", stream_id);
    h3_tunnel_stream_reset(ts);
    நிறுத்து;
  நிலை H3_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [connect]", stream_id);
    ts->state = H3_TUNNEL_CONNECT;
    நிறுத்து;
  நிலை H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [response]", stream_id);
    ts->state = H3_TUNNEL_RESPONSE;
    நிறுத்து;
  நிலை H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [established]", stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(data->req.hd_proxy_auth);
    நிறுத்து;
  நிலை H3_TUNNEL_FAILED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [failed]", stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    நிறுத்து;
  }
}

கட்டமைப்பு cf_h3_proxy_ctx {
  கட்டமைப்பு cf_ngtcp2_ctx ngtcp2_ctx;
  கட்டமைப்பு h3_tunnel_stream tunnel; /* our tunnel CONNECT stream */
  BIT(connected);
};

நிலையான CURLcode cf_ngtcp2_proxy_h3_init(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு cf_ngtcp2_ctx *ctx);

நிலையான CURLcode cf_h3_proxy_ctx_init(கட்டமைப்பு cf_h3_proxy_ctx *ctx,
                                     கட்டமைப்பு Curl_peer *origin,
                                     கட்டமைப்பு Curl_peer *peer,
                                     கட்டமைப்பு ssl_filter_config *sslc,
                                     கட்டமைப்பு Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode result;
  result = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  எனில்(!result)
    result = h3_tunnel_stream_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  திரும்பு result;
}

நிலையான வெறுமை cf_h3_proxy_ctx_free(கட்டமைப்பு cf_h3_proxy_ctx *ctx)
{
  எனில்(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_stream_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

நிலையான முழு cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                      uint64_t datalen, வெறுமை *user_data,
                                      வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream;
  size_t skiplen;
  (வெறுமை)stream_user_data;

  stream = pctx->tunnel.stream;
  எனில்(!stream || (stream->id != stream_id))
    திரும்பு 0;

  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  எனில்(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  இல்லையெனில்
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  எனில்(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    முழு rv = nghttp3_conn_resume_stream(conn, stream_id);
    எனில்(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  திரும்பு 0;
}

நிலையான முழு cb_h3_proxy_stream_close(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, வெறுமை *user_data,
                                    வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  கட்டமைப்பு h3_stream_ctx *stream;

  (வெறுமை)conn;
  (வெறுமை)stream_user_data;
  எனில்(!data)
    திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  எனில்(!stream || (stream->id != stream_id))
    திரும்பு 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  எனில்(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  இல்லையெனில்
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  pctx->tunnel.stream = வெற்று;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(data);
  திரும்பு 0;
}

நிலையான வெறுமை cf_h3_proxy_upd_rx_win(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு h3_stream_ctx *stream)
{
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  எனில்(!stream) /* already closed */
    திரும்பு;
  எனில்(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    திரும்பு;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = stream->rx_offset_max - stream->rx_offset;
  எனில்(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    எனில்(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    எனில்(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  stream->id, cur_win, rx_buffered, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
      ngtcp2_conn_extend_max_offset(ctx->qconn, delta);
    }
  }
}

நிலையான முழு cb_h3_proxy_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                                 மாறிலி uint8_t *buf, size_t buflen,
                                 வெறுமை *user_data, வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  கட்டமைப்பு h3_stream_ctx *stream;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  (வெறுமை)conn;
  (வெறுமை)stream3_id;
  (வெறுமை)stream_user_data;

  stream = pctx->tunnel.stream;
  எனில்(!data || !stream || (stream->id != stream3_id)) {
    திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  stream->rx_total += (curl_off_t)buflen;
  CURL_TRC_CF(data, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              stream->id, buflen, stream->rx_total);

  result = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  எனில்(result || (nwritten < buflen)) {
    திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  stream->rx_offset += buflen;
  எனில்(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_h3_proxy_upd_rx_win(cf, data, stream);

  Curl_multi_mark_dirty(data);
  திரும்பு 0;
}

நிலையான முழு cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t stream_id,
                                        size_t consumed, வெறுமை *user_data,
                                        வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (வெறுமை)conn;
  (வெறுமை)stream_user_data;

  எனில்(!ctx)
    திரும்பு 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  திரும்பு 0;
}

நிலையான முழு cb_h3_proxy_recv_header(nghttp3_conn *conn, int64_t stream_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *மதிப்பு, uint8_t flags,
                                   வெறுமை *user_data, வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(மதிப்பு);
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  கட்டமைப்பு h3_stream_ctx *stream;
  CURLcode result = CURLE_OK;
  முழு http_status;
  கட்டமைப்பு http_resp *resp;
  (வெறுமை)conn;
  (வெறுமை)stream_id;
  (வெறுமை)token;
  (வெறுமை)flags;
  (வெறுமை)stream_user_data;

  /* stream_user_data might be NULL for control streams */
  எனில்(!data)
    திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  எனில்(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (வெறுமை *)data, data ? data->mid : 0);
    திரும்பு 0;
  }

  எனில்(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    திரும்பு 0;
  }

  எனில்(token == NGHTTP3_QPACK_TOKEN__STATUS) {
    result = Curl_http_decode_status(&stream->status_code,
                                     (மாறிலி எழுத்து *)h3val.base, h3val.len);
    எனில்(result)
      திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
    http_status = stream->status_code;
    result = Curl_http_resp_make(&resp, http_status, வெற்று);
    எனில்(result)
      திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
    எனில்(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  இல்லையெனில் {
    எனில்(!pctx->tunnel.resp) {
      திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s", stream_id,
                (முழு)h3name.len, h3name.base, (முழு)h3val.len, h3val.base);
    result = Curl_dynhds_add(&pctx->tunnel.resp->headers,
                             (மாறிலி எழுத்து *)h3name.base, h3name.len,
                             (மாறிலி எழுத்து *)h3val.base, h3val.len);
    எனில்(result) {
      திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  திரும்பு 0;
}

நிலையான முழு cb_h3_proxy_end_headers(nghttp3_conn *conn, int64_t stream_id,
                                   முழு fin, வெறுமை *user_data,
                                   வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  கட்டமைப்பு h3_stream_ctx *stream;
  (வெறுமை)conn;
  (வெறுமை)stream_id;
  (வெறுமை)fin;
  (வெறுமை)stream_user_data;

  எனில்(!data)
    திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  எனில்(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (வெறுமை *)data, data ? data->mid : 0);
    திரும்பு 0;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d", stream_id,
              stream->status_code);

  எனில்(!pctx->tunnel.has_final_response) {
    எனில்(stream->status_code / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  எனில்(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(data);
  திரும்பு 0;
}

நிலையான முழு cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, வெறுமை *user_data,
                                    வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (வெறுமை)conn;
  (வெறுமை)stream_user_data;

  எனில்(ctx) {
    முழு rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                              app_error_code);

    எனில்(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
      திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  திரும்பு 0;
}

நிலையான முழு cb_h3_proxy_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, வெறுமை *user_data,
                                    வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  முழு rv;

  (வெறுமை)conn;
  (வெறுமை)stream_user_data;
  எனில்(!data)
    திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;

  எனில்(!pctx->tunnel.stream ||
     (stream_id != pctx->tunnel.stream->id))
    திரும்பு 0;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  pctx->tunnel.stream = வெற்று;
  pctx->tunnel.closed = TRUE;
  எனில்(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  திரும்பு 0;
}

நிலையான nghttp3_ssize cb_h3_tunnel_read_data(nghttp3_conn *conn,
                                            int64_t stream_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            வெறுமை *user_data,
                                            வெறுமை *stream_user_data)
{
  கட்டமைப்பு Curl_cfilter *cf = user_data;
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  கட்டமைப்பு h3_stream_ctx *stream;
  size_t nwritten = 0;
  size_t nvecs = 0;
  மாறிலி unsigned எழுத்து *buf_base;

  (வெறுமை)conn;
  (வெறுமை)stream_id;
  (வெறுமை)veccnt;
  (வெறுமை)stream_user_data;
  (வெறுமை)pflags;

  stream = pctx->tunnel.stream;
  எனில்(!data || !stream || (stream->id != stream_id))
    திரும்பு NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  எனில்(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    போது(nvecs < veccnt) {
      எனில்(!Curl_bufq_peek_at(&stream->sendbuf,
                           stream->tx_in_flight_size,
                           &buf_base,
                           &vec[nvecs].len))
        நிறுத்து;
      vec[nvecs].base = (uint8_t *)(uintptr_t)buf_base;
      stream->tx_in_flight_size += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  எனில்(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN",
                stream->id);
    திரும்பு NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf));
  திரும்பு (nghttp3_ssize)nvecs;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
நிலையான nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_stream_close,
  cb_h3_proxy_recv_data,
  cb_h3_proxy_deferred_consume,
  வெற்று, /* begin_headers */
  cb_h3_proxy_recv_header,
  cb_h3_proxy_end_headers,
  வெற்று, /* begin_trailers */
  cb_h3_proxy_recv_header,
  வெற்று, /* end_trailers */
  cb_h3_proxy_stop_sending,
  வெற்று, /* end_stream */
  cb_h3_proxy_reset_stream,
  வெற்று, /* shutdown */
  வெற்று, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  வெற்று, /* recv_origin */
  வெற்று, /* end_origin */
  வெற்று, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  வெற்று, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  வெற்று, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic எடு
#endif

நிலையான CURLcode cf_ngtcp2_proxy_h3_init(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு cf_ngtcp2_ctx *ctx)
{
  முழு rc;

  எனில்(ngtcp2_conn_get_streams_uni_இடது(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    திரும்பு CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  எனில்(rc) {
    failf(data, "error creating nghttp3 connection instance");
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  திரும்பு Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

நிலையான ssize_t cf_h3_proxy_recv_closed_stream(கட்டமைப்பு Curl_cfilter *cf,
                                              கட்டமைப்பு Curl_easy *data,
                                              கட்டமைப்பு h3_stream_ctx *stream,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  எனில்(stream->reset) {
    எனில்(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn);
      data->state.refused_stream = TRUE;
      *err = CURLE_RECV_ERROR;
      செல் out;
    }
    இல்லையெனில் எனில்(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", stream->id, stream->error3,
                  Curl_vquic_h3_err_str(stream->error3));
      nread = 0;
      செல் out;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    *err = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    செல் out;
  }
  இல்லையெனில் எனில்(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    *err = CURLE_HTTP3;
    செல் out;
  }
  nread = 0;

out:
  திரும்பு nread;
}

நிலையான CURLcode cf_h3_proxy_sendbuf_add(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு h3_stream_ctx *stream,
                                        மாறிலி uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode result;
  *pnwritten = 0;
  (வெறுமை)data;

  result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
  திரும்பு result;
}

நிலையான CURLcode cf_h3_proxy_send(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 மாறிலி uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  கட்டமைப்பு h3_stream_ctx *stream = வெற்று;
  கட்டமைப்பு cf_call_data save;
  கட்டமைப்பு cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  எனில்(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    செல் denied;
  }

  (வெறுமை)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  எனில்(result)
    செல் out;

  எனில்(pctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    செல் denied;
  }

  stream = pctx->tunnel.stream;
  எனில்(!stream) {
    result = CURLE_SEND_ERROR;
    செல் denied;
  }

  எனில்(stream->closed) {
    எனில்(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      செல் out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    செல் out;
  }
  இல்லையெனில் {
    result = cf_h3_proxy_sendbuf_add(data, stream, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (முழு)result, *pnwritten);
    எனில்(result)
      செல் out;
    (வெறுமை)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  எனில்(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (முழு)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

/* incoming data frames on the h3 stream */
நிலையான CURLcode cf_h3_proxy_recv(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 எழுத்து *buf, size_t len, size_t *pnread)
{
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  கட்டமைப்பு h3_stream_ctx *stream = pctx->tunnel.stream;
  கட்டமைப்பு cf_call_data save;
  கட்டமைப்பு cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  எனில்(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    செல் denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  எனில்(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    செல் out;
  }

  எனில்(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    எனில்(result)
      செல் out;
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  எனில்(result)
    செல் out;

  /* inbufq had nothing before, maybe after progressing ingress? */
  எனில்(!*pnread && !Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    எனில்(result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  stream->id, len, *pnread, (முழு)result);
      செல் out;
    }
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  எனில்(*pnread) {
    Curl_multi_mark_dirty(data);
  }
  இல்லையெனில் {
    எனில்(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      செல் out;
    }
    இல்லையெனில் எனில்(stream->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_stream(cf, data, stream, &result);
      எனில்(nread > 0)
        *pnread = (size_t)nread;
      செல் out;
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
              stream ? stream->id : -1, len, (முழு)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான CURLcode cf_h3_proxy_submit(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு h3_tunnel_stream *ts,
                                   கட்டமைப்பு httpreq *req)
{
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  கட்டமைப்பு h3_stream_ctx *stream = வெற்று;
  கட்டமைப்பு dynhds h2_headers;
  nghttp3_nv *nva = வெற்று;
  size_t nheader;
  முழு rc = 0;
  unsigned முழு i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = வெற்று;
  CURLcode result;

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  எனில்(result)
    செல் out;

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  எனில்(result)
    செல் out;

  nheader = Curl_dynhds_மொத்தம்(&h2_headers);
  nva = curlx_malloc(அளவு(nghttp3_nv) * nheader);
  எனில்(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  க்கு(i = 0; i < nheader; ++i) {
    கட்டமைப்பு dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned எழுத்து *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].மதிப்பு = (unsigned எழுத்து *)e->மதிப்பு;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  /* Open a bidirectional stream */
  {
    int64_t sid;
    முழு rv;

    DEBUGASSERT(stream->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, வெற்று);
    எனில்(rv) {
      failf(data, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      result = CURLE_SEND_ERROR;
      செல் out;
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

  எனில்(rc) {
    தேர்வு(rc) {
    நிலை NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      நிறுத்து;
    இயல்பு:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  stream->id, rc, nghttp3_strerror(rc));
      நிறுத்து;
    }
    result = CURLE_SEND_ERROR;
    செல் out;
  }

  எனில்(Curl_trc_is_verbose(data)) {
    CURL_TRC_CF(data, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", stream->id,
                Curl_bufref_ptr(&data->state.url));
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  திரும்பு result;
}

நிலையான CURLcode cf_h3_proxy_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                           கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  எனில்(!ctx->qconn || !pctx->tunnel.stream || (sock == CURL_SOCKET_BAD))
    திரும்பு CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  எனில்(want_recv || want_send || !Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    கட்டமைப்பு h3_stream_ctx *stream = pctx->tunnel.stream;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_இடது(ctx->qconn) ||
                 !ngtcp2_conn_get_max_data_இடது(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                stream->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
  }
  திரும்பு result;
}

நிலையான bool cf_h3_proxy_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                                     மாறிலி கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  எனில்(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf))
    திரும்பு TRUE;
  திரும்பு cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

நிலையான CURLcode cf_h3_proxy_submit_CONNECT(கட்டமைப்பு Curl_cfilter *cf,
                                           கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு h3_tunnel_stream *ts)
{
  கட்டமைப்பு cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  கட்டமைப்பு httpreq *req = வெற்று;

  result = Curl_http_proxy_create_tunnel_request(
    &req, cf, data, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  எனில்(!result)
    result = Curl_creader_set_null(data);
  எனில்(!result)
    result = cf_h3_proxy_submit(cf, data, ts, req);

  எனில்(req)
    Curl_http_req_free(req);
  எனில்(result)
    failf(data, "Failed sending CONNECT to proxy");
  திரும்பு result;
}

நிலையான CURLcode cf_h3_proxy_inspect_response(கட்டமைப்பு Curl_cfilter *cf,
                                             கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு h3_tunnel_stream *ts)
{
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)pctx->tunnel.udp, &res);
  எனில்(result)
    திரும்பு result;
  தேர்வு(res) {
  நிலை PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, data);
    நிறுத்து;
  நிலை PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
    result = CURLE_COULDNT_CONNECT;
    நிறுத்து;
  நிலை PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, data);
    நிறுத்து;
  }
  திரும்பு result;
}

நிலையான CURLcode cf_h3_proxy_tunnel(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு h3_tunnel_stream *ts,
                                   bool *pdone)
{
  கட்டமைப்பு cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  செய் {
    தேர்வு(ts->state) {
    நிலை H3_TUNNEL_INIT:
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = cf_h3_proxy_submit_CONNECT(cf, data, ts);
      எனில்(result)
        செல் out;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, data);

      result = Curl_cf_ngtcp2_progress_egress(cf, data, வெற்று);
      எனில்(result)
        செல் out;
      FALLTHROUGH();

    நிலை H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, வெற்று);
      எனில்(result)
        செல் out;
      result = Curl_cf_ngtcp2_progress_egress(cf, data, வெற்று);
      எனில்(result && result != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
        செல் out;
      }

      எனில்(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, data);
      }
      இல்லையெனில் {
        /* Not done yet, return and let multi interface call us again */
        result = CURLE_OK;
        செல் out;
      }
      FALLTHROUGH();

    நிலை H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = cf_h3_proxy_inspect_response(cf, data, ts);
      எனில்(result)
        செல் out;
      ctx->connected = TRUE;
      நிறுத்து;

    நிலை H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      திரும்பு CURLE_OK;

    நிலை H3_TUNNEL_FAILED:
      திரும்பு CURLE_RECV_ERROR;

    இயல்பு:
      நிறுத்து;
    }

  } போது(ts->state == H3_TUNNEL_INIT);

out:
  எனில்((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
  திரும்பு result;
}

நிலையான CURLcode cf_h3_proxy_connect(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு cf_h3_proxy_ctx *pctx = cf->ctx;
  கட்டமைப்பு cf_call_data save = { 0 };
  CURLcode result = CURLE_OK;
  கட்டமைப்பு h3_tunnel_stream *ts = &pctx->tunnel;
  bool data_saved = FALSE;

  result = Curl_cf_ngtcp2_cmn_connect(cf, data, done);
  எனில்(result || !*done)
    செல் out;

  CF_DATA_SAVE(save, cf, data);
  data_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  result = cf_h3_proxy_tunnel(cf, data, ts, done);

out:
  எனில்(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }

  எனில்(data_saved)
    CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான வெறுமை cf_h3_proxy_destroy(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h3_proxy_ctx *ctx = cf->ctx;

  (வெறுமை)data;
  எனில்(ctx) {
    CURL_TRC_CF(data, cf, "cf_h3_proxy_destroy()");
    எனில்(ctx->ngtcp2_ctx.qconn) {
      கட்டமைப்பு cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = வெற்று;
  }
}

நிலையான CURLcode cf_h3_proxy_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                                     கட்டமைப்பு Curl_easy *data, bool *done)
{
  திரும்பு Curl_cf_ngtcp2_cmn_shutdown(cf, data, done);
}

கட்டமைப்பு Curl_cftype Curl_cft_h3_proxy = {
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

CURLcode Curl_cf_ngtcp2_proxy_create(கட்டமைப்பு Curl_cfilter **pcf,
                                     கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு Curl_peer *origin,
                                     கட்டமைப்பு Curl_peer *peer,
                                     uint8_t transport_peer,
                                     கட்டமைப்பு connectdata *conn,
                                     கட்டமைப்பு Curl_sockaddr_ex *addr,
                                     கட்டமைப்பு Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  கட்டமைப்பு cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  எனில்(!tunnel_peer)
    திரும்பு CURLE_FAILED_INIT;
  எனில்((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    திரும்பு CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  result = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  எனில்(result)
    செல் out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  எனில்(result)
    செல் out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, வெற்று, TRNSPRT_QUIC);
  எனில்(result)
    செல் out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : வெற்று;
  எனில்(result) {
    எனில்(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    இல்லையெனில் எனில்(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  இல்லையெனில்
    CURL_TRC_CF(data, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  திரும்பு result;
}

CURLcode Curl_cf_ngtcp2_proxy_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                           கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு Curl_peer *origin,
                                           கட்டமைப்பு Curl_peer *peer,
                                           கட்டமைப்பு Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  கட்டமைப்பு cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  (வெறுமை)data;

  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx)
    செல் out;
  result = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  எனில்(result)
    செல் out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  எனில்(result)
    செல் out;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_செருகு_after(cf_at, cf);

out:
  எனில்(result) {
    எனில்(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    இல்லையெனில் எனில்(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  திரும்பு result;
}

#endif
