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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "http_proxy.h"
#અન્તર્ભાવ "vquic/vquic.h"
#અન્તર્ભાવ "vquic/cf-ngtcp2-cmn.h"
#અન્તર્ભાવ "vquic/cf-ngtcp2-proxy.h"
#અન્તર્ભાવ "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#define PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#define PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_STREAM_CHUNK_SIZE)

પ્રકારવ્યાખ્યા ગણના {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

સંરચના h3_tunnel_stream {
  સંરચના Curl_peer *dest;  /* where the tunnel goes to */
  સંરચના http_resp *resp;
  સંરચના bufq recvbuf;
  અક્ષર *authority;
  સંરચના h3_stream_ctx *stream;
  h3_tunnel_state state;
  BIT(udp);
  BIT(has_final_response);
  BIT(closed);
};

સ્થિર CURLcode h3_tunnel_stream_init(સંરચના h3_tunnel_stream *ts,
                                      સંરચના Curl_peer *dest,
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
  જો(!ts->authority)
    પાછા CURLE_OUT_OF_MEMORY;

  પાછા CURLE_OK;
}

સ્થિર શૂન્ય h3_tunnel_stream_reset(સંરચના h3_tunnel_stream *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = ખાલી;
  ts->stream = ખાલી;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

સ્થિર શૂન્ય h3_tunnel_stream_cleanup(સંરચના h3_tunnel_stream *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

સ્થિર શૂન્ય h3_tunnel_go_state(સંરચના Curl_cfilter *cf,
                               સંરચના h3_tunnel_stream *ts,
                               h3_tunnel_state new_state,
                               સંરચના Curl_easy *data)
{
  VERBOSE(int64_t stream_id = ts->stream ? ts->stream->id : -1);
  (શૂન્ય)cf;

  જો(ts->state == new_state)
    પાછા;

  /* leaving this one */
  પસંદગી(ts->state) {
  સ્થિતિ H3_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    તોડો;
  મૂળભૂત:
    તોડો;
  }

  /* entering this one */
  પસંદગી(new_state) {
  સ્થિતિ H3_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [init]", stream_id);
    h3_tunnel_stream_reset(ts);
    તોડો;
  સ્થિતિ H3_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [connect]", stream_id);
    ts->state = H3_TUNNEL_CONNECT;
    તોડો;
  સ્થિતિ H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [response]", stream_id);
    ts->state = H3_TUNNEL_RESPONSE;
    તોડો;
  સ્થિતિ H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [established]", stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(data->req.hd_proxy_auth);
    તોડો;
  સ્થિતિ H3_TUNNEL_FAILED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [failed]", stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    તોડો;
  }
}

સંરચના cf_h3_proxy_ctx {
  સંરચના cf_ngtcp2_ctx ngtcp2_ctx;
  સંરચના h3_tunnel_stream tunnel; /* our tunnel CONNECT stream */
  BIT(connected);
};

સ્થિર CURLcode cf_ngtcp2_proxy_h3_init(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        સંરચના cf_ngtcp2_ctx *ctx);

સ્થિર CURLcode cf_h3_proxy_ctx_init(સંરચના cf_h3_proxy_ctx *ctx,
                                     સંરચના Curl_peer *origin,
                                     સંરચના Curl_peer *peer,
                                     સંરચના ssl_filter_config *sslc,
                                     સંરચના Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode result;
  result = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  જો(!result)
    result = h3_tunnel_stream_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  પાછા result;
}

સ્થિર શૂન્ય cf_h3_proxy_ctx_free(સંરચના cf_h3_proxy_ctx *ctx)
{
  જો(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_stream_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

સ્થિર પૂર્ણાંક cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                      uint64_t datalen, શૂન્ય *user_data,
                                      શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના h3_stream_ctx *stream;
  size_t skiplen;
  (શૂન્ય)stream_user_data;

  stream = pctx->tunnel.stream;
  જો(!stream || (stream->id != stream_id))
    પાછા 0;

  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  જો(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  નહીંતર
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  જો(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    પૂર્ણાંક rv = nghttp3_conn_resume_stream(conn, stream_id);
    જો(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_h3_proxy_stream_close(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, શૂન્ય *user_data,
                                    શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  સંરચના h3_stream_ctx *stream;

  (શૂન્ય)conn;
  (શૂન્ય)stream_user_data;
  જો(!data)
    પાછા NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  જો(!stream || (stream->id != stream_id))
    પાછા 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  જો(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  નહીંતર
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  pctx->tunnel.stream = ખાલી;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(data);
  પાછા 0;
}

સ્થિર શૂન્ય cf_h3_proxy_upd_rx_win(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   સંરચના h3_stream_ctx *stream)
{
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  જો(!stream) /* already closed */
    પાછા;
  જો(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    પાછા;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = stream->rx_offset_max - stream->rx_offset;
  જો(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    જો(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    જો(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  stream->id, cur_win, rx_buffered, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
      ngtcp2_conn_extend_max_offset(ctx->qconn, delta);
    }
  }
}

સ્થિર પૂર્ણાંક cb_h3_proxy_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                                 અચલ uint8_t *buf, size_t buflen,
                                 શૂન્ય *user_data, શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  સંરચના h3_stream_ctx *stream;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  (શૂન્ય)conn;
  (શૂન્ય)stream3_id;
  (શૂન્ય)stream_user_data;

  stream = pctx->tunnel.stream;
  જો(!data || !stream || (stream->id != stream3_id)) {
    પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  stream->rx_total += (curl_off_t)buflen;
  CURL_TRC_CF(data, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              stream->id, buflen, stream->rx_total);

  result = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  જો(result || (nwritten < buflen)) {
    પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  stream->rx_offset += buflen;
  જો(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_h3_proxy_upd_rx_win(cf, data, stream);

  Curl_multi_mark_dirty(data);
  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t stream_id,
                                        size_t consumed, શૂન્ય *user_data,
                                        શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (શૂન્ય)conn;
  (શૂન્ય)stream_user_data;

  જો(!ctx)
    પાછા 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_h3_proxy_recv_header(nghttp3_conn *conn, int64_t stream_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *કિંમત, uint8_t flags,
                                   શૂન્ય *user_data, શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(કિંમત);
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  સંરચના h3_stream_ctx *stream;
  CURLcode result = CURLE_OK;
  પૂર્ણાંક http_status;
  સંરચના http_resp *resp;
  (શૂન્ય)conn;
  (શૂન્ય)stream_id;
  (શૂન્ય)token;
  (શૂન્ય)flags;
  (શૂન્ય)stream_user_data;

  /* stream_user_data might be NULL for control streams */
  જો(!data)
    પાછા NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  જો(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (શૂન્ય *)data, data ? data->mid : 0);
    પાછા 0;
  }

  જો(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    પાછા 0;
  }

  જો(token == NGHTTP3_QPACK_TOKEN__STATUS) {
    result = Curl_http_decode_status(&stream->status_code,
                                     (અચલ અક્ષર *)h3val.base, h3val.len);
    જો(result)
      પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
    http_status = stream->status_code;
    result = Curl_http_resp_make(&resp, http_status, ખાલી);
    જો(result)
      પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
    જો(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  નહીંતર {
    જો(!pctx->tunnel.resp) {
      પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s", stream_id,
                (પૂર્ણાંક)h3name.len, h3name.base, (પૂર્ણાંક)h3val.len, h3val.base);
    result = Curl_dynhds_add(&pctx->tunnel.resp->headers,
                             (અચલ અક્ષર *)h3name.base, h3name.len,
                             (અચલ અક્ષર *)h3val.base, h3val.len);
    જો(result) {
      પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_h3_proxy_end_headers(nghttp3_conn *conn, int64_t stream_id,
                                   પૂર્ણાંક fin, શૂન્ય *user_data,
                                   શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  સંરચના h3_stream_ctx *stream;
  (શૂન્ય)conn;
  (શૂન્ય)stream_id;
  (શૂન્ય)fin;
  (શૂન્ય)stream_user_data;

  જો(!data)
    પાછા NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  જો(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (શૂન્ય *)data, data ? data->mid : 0);
    પાછા 0;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d", stream_id,
              stream->status_code);

  જો(!pctx->tunnel.has_final_response) {
    જો(stream->status_code / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  જો(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(data);
  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, શૂન્ય *user_data,
                                    શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (શૂન્ય)conn;
  (શૂન્ય)stream_user_data;

  જો(ctx) {
    પૂર્ણાંક rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                              app_error_code);

    જો(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
      પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  પાછા 0;
}

સ્થિર પૂર્ણાંક cb_h3_proxy_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, શૂન્ય *user_data,
                                    શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  પૂર્ણાંક rv;

  (શૂન્ય)conn;
  (શૂન્ય)stream_user_data;
  જો(!data)
    પાછા NGHTTP3_ERR_CALLBACK_FAILURE;

  જો(!pctx->tunnel.stream ||
     (stream_id != pctx->tunnel.stream->id))
    પાછા 0;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  pctx->tunnel.stream = ખાલી;
  pctx->tunnel.closed = TRUE;
  જો(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    પાછા NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  પાછા 0;
}

સ્થિર nghttp3_ssize cb_h3_tunnel_read_data(nghttp3_conn *conn,
                                            int64_t stream_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            શૂન્ય *user_data,
                                            શૂન્ય *stream_user_data)
{
  સંરચના Curl_cfilter *cf = user_data;
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  સંરચના h3_stream_ctx *stream;
  size_t nwritten = 0;
  size_t nvecs = 0;
  અચલ unsigned અક્ષર *buf_base;

  (શૂન્ય)conn;
  (શૂન્ય)stream_id;
  (શૂન્ય)veccnt;
  (શૂન્ય)stream_user_data;
  (શૂન્ય)pflags;

  stream = pctx->tunnel.stream;
  જો(!data || !stream || (stream->id != stream_id))
    પાછા NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  જો(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    જ્યારે(nvecs < veccnt) {
      જો(!Curl_bufq_peek_at(&stream->sendbuf,
                           stream->tx_in_flight_size,
                           &buf_base,
                           &vec[nvecs].len))
        તોડો;
      vec[nvecs].base = (uint8_t *)(uintptr_t)buf_base;
      stream->tx_in_flight_size += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  જો(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN",
                stream->id);
    પાછા NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf));
  પાછા (nghttp3_ssize)nvecs;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ધકેલો
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
સ્થિર nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_stream_close,
  cb_h3_proxy_recv_data,
  cb_h3_proxy_deferred_consume,
  ખાલી, /* begin_headers */
  cb_h3_proxy_recv_header,
  cb_h3_proxy_end_headers,
  ખાલી, /* begin_trailers */
  cb_h3_proxy_recv_header,
  ખાલી, /* end_trailers */
  cb_h3_proxy_stop_sending,
  ખાલી, /* end_stream */
  cb_h3_proxy_reset_stream,
  ખાલી, /* shutdown */
  ખાલી, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  ખાલી, /* recv_origin */
  ખાલી, /* end_origin */
  ખાલી, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  ખાલી, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  ખાલી, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic કાઢો
#endif

સ્થિર CURLcode cf_ngtcp2_proxy_h3_init(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        સંરચના cf_ngtcp2_ctx *ctx)
{
  પૂર્ણાંક rc;

  જો(ngtcp2_conn_get_streams_uni_ડાબું(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    પાછા CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  જો(rc) {
    failf(data, "error creating nghttp3 connection instance");
    પાછા CURLE_OUT_OF_MEMORY;
  }

  પાછા Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

સ્થિર ssize_t cf_h3_proxy_recv_closed_stream(સંરચના Curl_cfilter *cf,
                                              સંરચના Curl_easy *data,
                                              સંરચના h3_stream_ctx *stream,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  જો(stream->reset) {
    જો(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn);
      data->state.refused_stream = TRUE;
      *err = CURLE_RECV_ERROR;
      જાઓ out;
    }
    નહીંતર જો(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", stream->id, stream->error3,
                  Curl_vquic_h3_err_str(stream->error3));
      nread = 0;
      જાઓ out;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    *err = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    જાઓ out;
  }
  નહીંતર જો(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    *err = CURLE_HTTP3;
    જાઓ out;
  }
  nread = 0;

out:
  પાછા nread;
}

સ્થિર CURLcode cf_h3_proxy_sendbuf_add(સંરચના Curl_easy *data,
                                        સંરચના h3_stream_ctx *stream,
                                        અચલ uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode result;
  *pnwritten = 0;
  (શૂન્ય)data;

  result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
  પાછા result;
}

સ્થિર CURLcode cf_h3_proxy_send(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 અચલ uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  સંરચના h3_stream_ctx *stream = ખાલી;
  સંરચના cf_call_data save;
  સંરચના cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  જો(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    જાઓ denied;
  }

  (શૂન્ય)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  જો(result)
    જાઓ out;

  જો(pctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    જાઓ denied;
  }

  stream = pctx->tunnel.stream;
  જો(!stream) {
    result = CURLE_SEND_ERROR;
    જાઓ denied;
  }

  જો(stream->closed) {
    જો(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      જાઓ out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    જાઓ out;
  }
  નહીંતર {
    result = cf_h3_proxy_sendbuf_add(data, stream, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (પૂર્ણાંક)result, *pnwritten);
    જો(result)
      જાઓ out;
    (શૂન્ય)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  જો(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (પૂર્ણાંક)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

/* incoming data frames on the h3 stream */
સ્થિર CURLcode cf_h3_proxy_recv(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 અક્ષર *buf, size_t len, size_t *pnread)
{
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  સંરચના h3_stream_ctx *stream = pctx->tunnel.stream;
  સંરચના cf_call_data save;
  સંરચના cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  જો(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    જાઓ denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  જો(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    જાઓ out;
  }

  જો(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    જો(result)
      જાઓ out;
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  જો(result)
    જાઓ out;

  /* inbufq had nothing before, maybe after progressing ingress? */
  જો(!*pnread && !Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    જો(result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  stream->id, len, *pnread, (પૂર્ણાંક)result);
      જાઓ out;
    }
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  જો(*pnread) {
    Curl_multi_mark_dirty(data);
  }
  નહીંતર {
    જો(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      જાઓ out;
    }
    નહીંતર જો(stream->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_stream(cf, data, stream, &result);
      જો(nread > 0)
        *pnread = (size_t)nread;
      જાઓ out;
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
              stream ? stream->id : -1, len, (પૂર્ણાંક)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર CURLcode cf_h3_proxy_submit(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   સંરચના h3_tunnel_stream *ts,
                                   સંરચના httpreq *req)
{
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  સંરચના h3_stream_ctx *stream = ખાલી;
  સંરચના dynhds h2_headers;
  nghttp3_nv *nva = ખાલી;
  size_t nheader;
  પૂર્ણાંક rc = 0;
  unsigned પૂર્ણાંક i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = ખાલી;
  CURLcode result;

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  જો(result)
    જાઓ out;

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  જો(result)
    જાઓ out;

  nheader = Curl_dynhds_ગણતરી(&h2_headers);
  nva = curlx_malloc(કદ(nghttp3_nv) * nheader);
  જો(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  માટે(i = 0; i < nheader; ++i) {
    સંરચના dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned અક્ષર *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].કિંમત = (unsigned અક્ષર *)e->કિંમત;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  /* Open a bidirectional stream */
  {
    int64_t sid;
    પૂર્ણાંક rv;

    DEBUGASSERT(stream->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, ખાલી);
    જો(rv) {
      failf(data, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      result = CURLE_SEND_ERROR;
      જાઓ out;
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

  જો(rc) {
    પસંદગી(rc) {
    સ્થિતિ NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      તોડો;
    મૂળભૂત:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  stream->id, rc, nghttp3_strerror(rc));
      તોડો;
    }
    result = CURLE_SEND_ERROR;
    જાઓ out;
  }

  જો(Curl_trc_is_verbose(data)) {
    CURL_TRC_CF(data, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", stream->id,
                Curl_bufref_ptr(&data->state.url));
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  પાછા result;
}

સ્થિર CURLcode cf_h3_proxy_adjust_pollset(સંરચના Curl_cfilter *cf,
                                           સંરચના Curl_easy *data,
                                           સંરચના easy_pollset *ps)
{
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  જો(!ctx->qconn || !pctx->tunnel.stream || (sock == CURL_SOCKET_BAD))
    પાછા CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  જો(want_recv || want_send || !Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    સંરચના h3_stream_ctx *stream = pctx->tunnel.stream;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_ડાબું(ctx->qconn) ||
                 !ngtcp2_conn_get_max_data_ડાબું(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                stream->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
  }
  પાછા result;
}

સ્થિર bool cf_h3_proxy_data_pending(સંરચના Curl_cfilter *cf,
                                     અચલ સંરચના Curl_easy *data)
{
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  જો(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf))
    પાછા TRUE;
  પાછા cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

સ્થિર CURLcode cf_h3_proxy_submit_CONNECT(સંરચના Curl_cfilter *cf,
                                           સંરચના Curl_easy *data,
                                           સંરચના h3_tunnel_stream *ts)
{
  સંરચના cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  સંરચના httpreq *req = ખાલી;

  result = Curl_http_proxy_create_tunnel_request(
    &req, cf, data, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  જો(!result)
    result = Curl_creader_set_null(data);
  જો(!result)
    result = cf_h3_proxy_submit(cf, data, ts, req);

  જો(req)
    Curl_http_req_free(req);
  જો(result)
    failf(data, "Failed sending CONNECT to proxy");
  પાછા result;
}

સ્થિર CURLcode cf_h3_proxy_inspect_response(સંરચના Curl_cfilter *cf,
                                             સંરચના Curl_easy *data,
                                             સંરચના h3_tunnel_stream *ts)
{
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)pctx->tunnel.udp, &res);
  જો(result)
    પાછા result;
  પસંદગી(res) {
  સ્થિતિ PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, data);
    તોડો;
  સ્થિતિ PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
    result = CURLE_COULDNT_CONNECT;
    તોડો;
  સ્થિતિ PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, data);
    તોડો;
  }
  પાછા result;
}

સ્થિર CURLcode cf_h3_proxy_tunnel(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   સંરચના h3_tunnel_stream *ts,
                                   bool *pdone)
{
  સંરચના cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  કરો {
    પસંદગી(ts->state) {
    સ્થિતિ H3_TUNNEL_INIT:
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = cf_h3_proxy_submit_CONNECT(cf, data, ts);
      જો(result)
        જાઓ out;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, data);

      result = Curl_cf_ngtcp2_progress_egress(cf, data, ખાલી);
      જો(result)
        જાઓ out;
      FALLTHROUGH();

    સ્થિતિ H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, ખાલી);
      જો(result)
        જાઓ out;
      result = Curl_cf_ngtcp2_progress_egress(cf, data, ખાલી);
      જો(result && result != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
        જાઓ out;
      }

      જો(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, data);
      }
      નહીંતર {
        /* Not done yet, return and let multi interface call us again */
        result = CURLE_OK;
        જાઓ out;
      }
      FALLTHROUGH();

    સ્થિતિ H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = cf_h3_proxy_inspect_response(cf, data, ts);
      જો(result)
        જાઓ out;
      ctx->connected = TRUE;
      તોડો;

    સ્થિતિ H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      પાછા CURLE_OK;

    સ્થિતિ H3_TUNNEL_FAILED:
      પાછા CURLE_RECV_ERROR;

    મૂળભૂત:
      તોડો;
    }

  } જ્યારે(ts->state == H3_TUNNEL_INIT);

out:
  જો((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
  પાછા result;
}

સ્થિર CURLcode cf_h3_proxy_connect(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data, bool *done)
{
  સંરચના cf_h3_proxy_ctx *pctx = cf->ctx;
  સંરચના cf_call_data save = { 0 };
  CURLcode result = CURLE_OK;
  સંરચના h3_tunnel_stream *ts = &pctx->tunnel;
  bool data_saved = FALSE;

  result = Curl_cf_ngtcp2_cmn_connect(cf, data, done);
  જો(result || !*done)
    જાઓ out;

  CF_DATA_SAVE(save, cf, data);
  data_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  result = cf_h3_proxy_tunnel(cf, data, ts, done);

out:
  જો(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }

  જો(data_saved)
    CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર શૂન્ય cf_h3_proxy_destroy(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data)
{
  સંરચના cf_h3_proxy_ctx *ctx = cf->ctx;

  (શૂન્ય)data;
  જો(ctx) {
    CURL_TRC_CF(data, cf, "cf_h3_proxy_destroy()");
    જો(ctx->ngtcp2_ctx.qconn) {
      સંરચના cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = ખાલી;
  }
}

સ્થિર CURLcode cf_h3_proxy_shutdown(સંરચના Curl_cfilter *cf,
                                     સંરચના Curl_easy *data, bool *done)
{
  પાછા Curl_cf_ngtcp2_cmn_shutdown(cf, data, done);
}

સંરચના Curl_cftype Curl_cft_h3_proxy = {
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

CURLcode Curl_cf_ngtcp2_proxy_create(સંરચના Curl_cfilter **pcf,
                                     સંરચના Curl_easy *data,
                                     સંરચના Curl_peer *origin,
                                     સંરચના Curl_peer *peer,
                                     uint8_t transport_peer,
                                     સંરચના connectdata *conn,
                                     સંરચના Curl_sockaddr_ex *addr,
                                     સંરચના Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  સંરચના Curl_cfilter *cf = ખાલી;
  સંરચના cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  જો(!tunnel_peer)
    પાછા CURLE_FAILED_INIT;
  જો((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    પાછા CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  result = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  જો(result)
    જાઓ out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  જો(result)
    જાઓ out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ખાલી, TRNSPRT_QUIC);
  જો(result)
    જાઓ out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : ખાલી;
  જો(result) {
    જો(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    નહીંતર જો(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  નહીંતર
    CURL_TRC_CF(data, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  પાછા result;
}

CURLcode Curl_cf_ngtcp2_proxy_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                           સંરચના Curl_easy *data,
                                           સંરચના Curl_peer *origin,
                                           સંરચના Curl_peer *peer,
                                           સંરચના Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  સંરચના Curl_cfilter *cf = ખાલી;
  સંરચના cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  (શૂન્ય)data;

  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx)
    જાઓ out;
  result = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  જો(result)
    જાઓ out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  જો(result)
    જાઓ out;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_ઉમેરો_after(cf_at, cf);

out:
  જો(result) {
    જો(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    નહીંતર જો(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  પાછા result;
}

#endif
