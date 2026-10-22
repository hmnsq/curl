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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "http_proxy.h"
#അന്തര്ഭാവ "vquic/vquic.h"
#അന്തര്ഭാവ "vquic/cf-ngtcp2-cmn.h"
#അന്തര്ഭാവ "vquic/cf-ngtcp2-proxy.h"
#അന്തര്ഭാവ "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#define PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#define PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_STREAM_CHUNK_SIZE)

തരനിർവചനം എണ്ണൽ {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

ഘടന h3_tunnel_stream {
  ഘടന Curl_peer *dest;  /* where the tunnel goes to */
  ഘടന http_resp *resp;
  ഘടന bufq recvbuf;
  അക്ഷരം *authority;
  ഘടന h3_stream_ctx *stream;
  h3_tunnel_state state;
  BIT(udp);
  BIT(has_final_response);
  BIT(closed);
};

സ്ഥിതം CURLcode h3_tunnel_stream_init(ഘടന h3_tunnel_stream *ts,
                                      ഘടന Curl_peer *dest,
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
  എങ്കിൽ(!ts->authority)
    തിരികെ CURLE_OUT_OF_MEMORY;

  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം h3_tunnel_stream_reset(ഘടന h3_tunnel_stream *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = ശൂന്യ;
  ts->stream = ശൂന്യ;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

സ്ഥിതം ശൂന്യം h3_tunnel_stream_cleanup(ഘടന h3_tunnel_stream *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

സ്ഥിതം ശൂന്യം h3_tunnel_go_state(ഘടന Curl_cfilter *cf,
                               ഘടന h3_tunnel_stream *ts,
                               h3_tunnel_state new_state,
                               ഘടന Curl_easy *data)
{
  VERBOSE(int64_t stream_id = ts->stream ? ts->stream->id : -1);
  (ശൂന്യം)cf;

  എങ്കിൽ(ts->state == new_state)
    തിരികെ;

  /* leaving this one */
  തിരഞ്ഞെടുപ്പ്(ts->state) {
  അവസ്ഥ H3_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }

  /* entering this one */
  തിരഞ്ഞെടുപ്പ്(new_state) {
  അവസ്ഥ H3_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [init]", stream_id);
    h3_tunnel_stream_reset(ts);
    നിർത്തുക;
  അവസ്ഥ H3_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [connect]", stream_id);
    ts->state = H3_TUNNEL_CONNECT;
    നിർത്തുക;
  അവസ്ഥ H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [response]", stream_id);
    ts->state = H3_TUNNEL_RESPONSE;
    നിർത്തുക;
  അവസ്ഥ H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [established]", stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(data->req.hd_proxy_auth);
    നിർത്തുക;
  അവസ്ഥ H3_TUNNEL_FAILED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [failed]", stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    നിർത്തുക;
  }
}

ഘടന cf_h3_proxy_ctx {
  ഘടന cf_ngtcp2_ctx ngtcp2_ctx;
  ഘടന h3_tunnel_stream tunnel; /* our tunnel CONNECT stream */
  BIT(connected);
};

സ്ഥിതം CURLcode cf_ngtcp2_proxy_h3_init(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        ഘടന cf_ngtcp2_ctx *ctx);

സ്ഥിതം CURLcode cf_h3_proxy_ctx_init(ഘടന cf_h3_proxy_ctx *ctx,
                                     ഘടന Curl_peer *origin,
                                     ഘടന Curl_peer *peer,
                                     ഘടന ssl_filter_config *sslc,
                                     ഘടന Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode result;
  result = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  എങ്കിൽ(!result)
    result = h3_tunnel_stream_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_h3_proxy_ctx_free(ഘടന cf_h3_proxy_ctx *ctx)
{
  എങ്കിൽ(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_stream_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                      uint64_t datalen, ശൂന്യം *user_data,
                                      ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന h3_stream_ctx *stream;
  size_t skiplen;
  (ശൂന്യം)stream_user_data;

  stream = pctx->tunnel.stream;
  എങ്കിൽ(!stream || (stream->id != stream_id))
    തിരികെ 0;

  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  എങ്കിൽ(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  അല്ലെങ്കിൽ
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  എങ്കിൽ(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    പൂർണ്ണസംഖ്യ rv = nghttp3_conn_resume_stream(conn, stream_id);
    എങ്കിൽ(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_proxy_stream_close(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, ശൂന്യം *user_data,
                                    ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  ഘടന h3_stream_ctx *stream;

  (ശൂന്യം)conn;
  (ശൂന്യം)stream_user_data;
  എങ്കിൽ(!data)
    തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  എങ്കിൽ(!stream || (stream->id != stream_id))
    തിരികെ 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  എങ്കിൽ(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  അല്ലെങ്കിൽ
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  pctx->tunnel.stream = ശൂന്യ;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(data);
  തിരികെ 0;
}

സ്ഥിതം ശൂന്യം cf_h3_proxy_upd_rx_win(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ഘടന h3_stream_ctx *stream)
{
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  എങ്കിൽ(!stream) /* already closed */
    തിരികെ;
  എങ്കിൽ(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    തിരികെ;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = stream->rx_offset_max - stream->rx_offset;
  എങ്കിൽ(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    എങ്കിൽ(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    എങ്കിൽ(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  stream->id, cur_win, rx_buffered, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
      ngtcp2_conn_extend_max_offset(ctx->qconn, delta);
    }
  }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_proxy_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                                 സ്ഥിരം uint8_t *buf, size_t buflen,
                                 ശൂന്യം *user_data, ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  ഘടന h3_stream_ctx *stream;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  (ശൂന്യം)conn;
  (ശൂന്യം)stream3_id;
  (ശൂന്യം)stream_user_data;

  stream = pctx->tunnel.stream;
  എങ്കിൽ(!data || !stream || (stream->id != stream3_id)) {
    തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  stream->rx_total += (curl_off_t)buflen;
  CURL_TRC_CF(data, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              stream->id, buflen, stream->rx_total);

  result = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  എങ്കിൽ(result || (nwritten < buflen)) {
    തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  stream->rx_offset += buflen;
  എങ്കിൽ(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_h3_proxy_upd_rx_win(cf, data, stream);

  Curl_multi_mark_dirty(data);
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t stream_id,
                                        size_t consumed, ശൂന്യം *user_data,
                                        ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (ശൂന്യം)conn;
  (ശൂന്യം)stream_user_data;

  എങ്കിൽ(!ctx)
    തിരികെ 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_proxy_recv_header(nghttp3_conn *conn, int64_t stream_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *മൂല്യം, uint8_t flags,
                                   ശൂന്യം *user_data, ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(മൂല്യം);
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  ഘടന h3_stream_ctx *stream;
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ http_status;
  ഘടന http_resp *resp;
  (ശൂന്യം)conn;
  (ശൂന്യം)stream_id;
  (ശൂന്യം)token;
  (ശൂന്യം)flags;
  (ശൂന്യം)stream_user_data;

  /* stream_user_data might be NULL for control streams */
  എങ്കിൽ(!data)
    തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  എങ്കിൽ(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (ശൂന്യം *)data, data ? data->mid : 0);
    തിരികെ 0;
  }

  എങ്കിൽ(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    തിരികെ 0;
  }

  എങ്കിൽ(token == NGHTTP3_QPACK_TOKEN__STATUS) {
    result = Curl_http_decode_status(&stream->status_code,
                                     (സ്ഥിരം അക്ഷരം *)h3val.base, h3val.len);
    എങ്കിൽ(result)
      തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
    http_status = stream->status_code;
    result = Curl_http_resp_make(&resp, http_status, ശൂന്യ);
    എങ്കിൽ(result)
      തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
    എങ്കിൽ(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(!pctx->tunnel.resp) {
      തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s", stream_id,
                (പൂർണ്ണസംഖ്യ)h3name.len, h3name.base, (പൂർണ്ണസംഖ്യ)h3val.len, h3val.base);
    result = Curl_dynhds_add(&pctx->tunnel.resp->headers,
                             (സ്ഥിരം അക്ഷരം *)h3name.base, h3name.len,
                             (സ്ഥിരം അക്ഷരം *)h3val.base, h3val.len);
    എങ്കിൽ(result) {
      തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_proxy_end_headers(nghttp3_conn *conn, int64_t stream_id,
                                   പൂർണ്ണസംഖ്യ fin, ശൂന്യം *user_data,
                                   ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  ഘടന h3_stream_ctx *stream;
  (ശൂന്യം)conn;
  (ശൂന്യം)stream_id;
  (ശൂന്യം)fin;
  (ശൂന്യം)stream_user_data;

  എങ്കിൽ(!data)
    തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  എങ്കിൽ(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (ശൂന്യം *)data, data ? data->mid : 0);
    തിരികെ 0;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d", stream_id,
              stream->status_code);

  എങ്കിൽ(!pctx->tunnel.has_final_response) {
    എങ്കിൽ(stream->status_code / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  എങ്കിൽ(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(data);
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, ശൂന്യം *user_data,
                                    ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (ശൂന്യം)conn;
  (ശൂന്യം)stream_user_data;

  എങ്കിൽ(ctx) {
    പൂർണ്ണസംഖ്യ rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                              app_error_code);

    എങ്കിൽ(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
      തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_h3_proxy_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, ശൂന്യം *user_data,
                                    ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  പൂർണ്ണസംഖ്യ rv;

  (ശൂന്യം)conn;
  (ശൂന്യം)stream_user_data;
  എങ്കിൽ(!data)
    തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;

  എങ്കിൽ(!pctx->tunnel.stream ||
     (stream_id != pctx->tunnel.stream->id))
    തിരികെ 0;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  pctx->tunnel.stream = ശൂന്യ;
  pctx->tunnel.closed = TRUE;
  എങ്കിൽ(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  തിരികെ 0;
}

സ്ഥിതം nghttp3_ssize cb_h3_tunnel_read_data(nghttp3_conn *conn,
                                            int64_t stream_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            ശൂന്യം *user_data,
                                            ശൂന്യം *stream_user_data)
{
  ഘടന Curl_cfilter *cf = user_data;
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  ഘടന h3_stream_ctx *stream;
  size_t nwritten = 0;
  size_t nvecs = 0;
  സ്ഥിരം unsigned അക്ഷരം *buf_base;

  (ശൂന്യം)conn;
  (ശൂന്യം)stream_id;
  (ശൂന്യം)veccnt;
  (ശൂന്യം)stream_user_data;
  (ശൂന്യം)pflags;

  stream = pctx->tunnel.stream;
  എങ്കിൽ(!data || !stream || (stream->id != stream_id))
    തിരികെ NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  എങ്കിൽ(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    ആകുമ്പോൾ(nvecs < veccnt) {
      എങ്കിൽ(!Curl_bufq_peek_at(&stream->sendbuf,
                           stream->tx_in_flight_size,
                           &buf_base,
                           &vec[nvecs].len))
        നിർത്തുക;
      vec[nvecs].base = (uint8_t *)(uintptr_t)buf_base;
      stream->tx_in_flight_size += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  എങ്കിൽ(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN",
                stream->id);
    തിരികെ NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf));
  തിരികെ (nghttp3_ssize)nvecs;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
സ്ഥിതം nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_stream_close,
  cb_h3_proxy_recv_data,
  cb_h3_proxy_deferred_consume,
  ശൂന്യ, /* begin_headers */
  cb_h3_proxy_recv_header,
  cb_h3_proxy_end_headers,
  ശൂന്യ, /* begin_trailers */
  cb_h3_proxy_recv_header,
  ശൂന്യ, /* end_trailers */
  cb_h3_proxy_stop_sending,
  ശൂന്യ, /* end_stream */
  cb_h3_proxy_reset_stream,
  ശൂന്യ, /* shutdown */
  ശൂന്യ, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  ശൂന്യ, /* recv_origin */
  ശൂന്യ, /* end_origin */
  ശൂന്യ, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  ശൂന്യ, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  ശൂന്യ, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic എടുക്കുക
#endif

സ്ഥിതം CURLcode cf_ngtcp2_proxy_h3_init(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        ഘടന cf_ngtcp2_ctx *ctx)
{
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(ngtcp2_conn_get_streams_uni_ഇടത്(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    തിരികെ CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  എങ്കിൽ(rc) {
    failf(data, "error creating nghttp3 connection instance");
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  തിരികെ Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

സ്ഥിതം ssize_t cf_h3_proxy_recv_closed_stream(ഘടന Curl_cfilter *cf,
                                              ഘടന Curl_easy *data,
                                              ഘടന h3_stream_ctx *stream,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  എങ്കിൽ(stream->reset) {
    എങ്കിൽ(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn);
      data->state.refused_stream = TRUE;
      *err = CURLE_RECV_ERROR;
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", stream->id, stream->error3,
                  Curl_vquic_h3_err_str(stream->error3));
      nread = 0;
      പോകുക out;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    *err = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    പോകുക out;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    *err = CURLE_HTTP3;
    പോകുക out;
  }
  nread = 0;

out:
  തിരികെ nread;
}

സ്ഥിതം CURLcode cf_h3_proxy_sendbuf_add(ഘടന Curl_easy *data,
                                        ഘടന h3_stream_ctx *stream,
                                        സ്ഥിരം uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode result;
  *pnwritten = 0;
  (ശൂന്യം)data;

  result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h3_proxy_send(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 സ്ഥിരം uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ഘടന h3_stream_ctx *stream = ശൂന്യ;
  ഘടന cf_call_data save;
  ഘടന cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  എങ്കിൽ(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    പോകുക denied;
  }

  (ശൂന്യം)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  എങ്കിൽ(result)
    പോകുക out;

  എങ്കിൽ(pctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    പോകുക denied;
  }

  stream = pctx->tunnel.stream;
  എങ്കിൽ(!stream) {
    result = CURLE_SEND_ERROR;
    പോകുക denied;
  }

  എങ്കിൽ(stream->closed) {
    എങ്കിൽ(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      പോകുക out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    പോകുക out;
  }
  അല്ലെങ്കിൽ {
    result = cf_h3_proxy_sendbuf_add(data, stream, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (പൂർണ്ണസംഖ്യ)result, *pnwritten);
    എങ്കിൽ(result)
      പോകുക out;
    (ശൂന്യം)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  എങ്കിൽ(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (പൂർണ്ണസംഖ്യ)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

/* incoming data frames on the h3 stream */
സ്ഥിതം CURLcode cf_h3_proxy_recv(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 അക്ഷരം *buf, size_t len, size_t *pnread)
{
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ഘടന h3_stream_ctx *stream = pctx->tunnel.stream;
  ഘടന cf_call_data save;
  ഘടന cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  എങ്കിൽ(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    പോകുക denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  എങ്കിൽ(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    പോകുക out;
  }

  എങ്കിൽ(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    എങ്കിൽ(result)
      പോകുക out;
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  എങ്കിൽ(result)
    പോകുക out;

  /* inbufq had nothing before, maybe after progressing ingress? */
  എങ്കിൽ(!*pnread && !Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    എങ്കിൽ(result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  stream->id, len, *pnread, (പൂർണ്ണസംഖ്യ)result);
      പോകുക out;
    }
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  എങ്കിൽ(*pnread) {
    Curl_multi_mark_dirty(data);
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(stream->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_stream(cf, data, stream, &result);
      എങ്കിൽ(nread > 0)
        *pnread = (size_t)nread;
      പോകുക out;
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
              stream ? stream->id : -1, len, (പൂർണ്ണസംഖ്യ)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h3_proxy_submit(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ഘടന h3_tunnel_stream *ts,
                                   ഘടന httpreq *req)
{
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  ഘടന h3_stream_ctx *stream = ശൂന്യ;
  ഘടന dynhds h2_headers;
  nghttp3_nv *nva = ശൂന്യ;
  size_t nheader;
  പൂർണ്ണസംഖ്യ rc = 0;
  unsigned പൂർണ്ണസംഖ്യ i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = ശൂന്യ;
  CURLcode result;

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  എങ്കിൽ(result)
    പോകുക out;

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  എങ്കിൽ(result)
    പോകുക out;

  nheader = Curl_dynhds_എണ്ണം(&h2_headers);
  nva = curlx_malloc(വലുപ്പം(nghttp3_nv) * nheader);
  എങ്കിൽ(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  വേണ്ടി(i = 0; i < nheader; ++i) {
    ഘടന dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned അക്ഷരം *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].മൂല്യം = (unsigned അക്ഷരം *)e->മൂല്യം;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  /* Open a bidirectional stream */
  {
    int64_t sid;
    പൂർണ്ണസംഖ്യ rv;

    DEBUGASSERT(stream->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, ശൂന്യ);
    എങ്കിൽ(rv) {
      failf(data, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      result = CURLE_SEND_ERROR;
      പോകുക out;
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

  എങ്കിൽ(rc) {
    തിരഞ്ഞെടുപ്പ്(rc) {
    അവസ്ഥ NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  stream->id, rc, nghttp3_strerror(rc));
      നിർത്തുക;
    }
    result = CURLE_SEND_ERROR;
    പോകുക out;
  }

  എങ്കിൽ(Curl_trc_is_verbose(data)) {
    CURL_TRC_CF(data, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", stream->id,
                Curl_bufref_ptr(&data->state.url));
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h3_proxy_adjust_pollset(ഘടന Curl_cfilter *cf,
                                           ഘടന Curl_easy *data,
                                           ഘടന easy_pollset *ps)
{
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  എങ്കിൽ(!ctx->qconn || !pctx->tunnel.stream || (sock == CURL_SOCKET_BAD))
    തിരികെ CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  എങ്കിൽ(want_recv || want_send || !Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    ഘടന h3_stream_ctx *stream = pctx->tunnel.stream;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_ഇടത്(ctx->qconn) ||
                 !ngtcp2_conn_get_max_data_ഇടത്(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                stream->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
  }
  തിരികെ result;
}

സ്ഥിതം bool cf_h3_proxy_data_pending(ഘടന Curl_cfilter *cf,
                                     സ്ഥിരം ഘടന Curl_easy *data)
{
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  എങ്കിൽ(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf))
    തിരികെ TRUE;
  തിരികെ cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

സ്ഥിതം CURLcode cf_h3_proxy_submit_CONNECT(ഘടന Curl_cfilter *cf,
                                           ഘടന Curl_easy *data,
                                           ഘടന h3_tunnel_stream *ts)
{
  ഘടന cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  ഘടന httpreq *req = ശൂന്യ;

  result = Curl_http_proxy_create_tunnel_request(
    &req, cf, data, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  എങ്കിൽ(!result)
    result = Curl_creader_set_null(data);
  എങ്കിൽ(!result)
    result = cf_h3_proxy_submit(cf, data, ts, req);

  എങ്കിൽ(req)
    Curl_http_req_free(req);
  എങ്കിൽ(result)
    failf(data, "Failed sending CONNECT to proxy");
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h3_proxy_inspect_response(ഘടന Curl_cfilter *cf,
                                             ഘടന Curl_easy *data,
                                             ഘടന h3_tunnel_stream *ts)
{
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)pctx->tunnel.udp, &res);
  എങ്കിൽ(result)
    തിരികെ result;
  തിരഞ്ഞെടുപ്പ്(res) {
  അവസ്ഥ PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, data);
    നിർത്തുക;
  അവസ്ഥ PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
    result = CURLE_COULDNT_CONNECT;
    നിർത്തുക;
  അവസ്ഥ PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, data);
    നിർത്തുക;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h3_proxy_tunnel(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ഘടന h3_tunnel_stream *ts,
                                   bool *pdone)
{
  ഘടന cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  ചെയ്യുക {
    തിരഞ്ഞെടുപ്പ്(ts->state) {
    അവസ്ഥ H3_TUNNEL_INIT:
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = cf_h3_proxy_submit_CONNECT(cf, data, ts);
      എങ്കിൽ(result)
        പോകുക out;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, data);

      result = Curl_cf_ngtcp2_progress_egress(cf, data, ശൂന്യ);
      എങ്കിൽ(result)
        പോകുക out;
      FALLTHROUGH();

    അവസ്ഥ H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, ശൂന്യ);
      എങ്കിൽ(result)
        പോകുക out;
      result = Curl_cf_ngtcp2_progress_egress(cf, data, ശൂന്യ);
      എങ്കിൽ(result && result != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
        പോകുക out;
      }

      എങ്കിൽ(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, data);
      }
      അല്ലെങ്കിൽ {
        /* Not done yet, return and let multi interface call us again */
        result = CURLE_OK;
        പോകുക out;
      }
      FALLTHROUGH();

    അവസ്ഥ H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = cf_h3_proxy_inspect_response(cf, data, ts);
      എങ്കിൽ(result)
        പോകുക out;
      ctx->connected = TRUE;
      നിർത്തുക;

    അവസ്ഥ H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      തിരികെ CURLE_OK;

    അവസ്ഥ H3_TUNNEL_FAILED:
      തിരികെ CURLE_RECV_ERROR;

    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }

  } ആകുമ്പോൾ(ts->state == H3_TUNNEL_INIT);

out:
  എങ്കിൽ((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h3_proxy_connect(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data, bool *done)
{
  ഘടന cf_h3_proxy_ctx *pctx = cf->ctx;
  ഘടന cf_call_data save = { 0 };
  CURLcode result = CURLE_OK;
  ഘടന h3_tunnel_stream *ts = &pctx->tunnel;
  bool data_saved = FALSE;

  result = Curl_cf_ngtcp2_cmn_connect(cf, data, done);
  എങ്കിൽ(result || !*done)
    പോകുക out;

  CF_DATA_SAVE(save, cf, data);
  data_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  result = cf_h3_proxy_tunnel(cf, data, ts, done);

out:
  എങ്കിൽ(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }

  എങ്കിൽ(data_saved)
    CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_h3_proxy_destroy(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data)
{
  ഘടന cf_h3_proxy_ctx *ctx = cf->ctx;

  (ശൂന്യം)data;
  എങ്കിൽ(ctx) {
    CURL_TRC_CF(data, cf, "cf_h3_proxy_destroy()");
    എങ്കിൽ(ctx->ngtcp2_ctx.qconn) {
      ഘടന cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = ശൂന്യ;
  }
}

സ്ഥിതം CURLcode cf_h3_proxy_shutdown(ഘടന Curl_cfilter *cf,
                                     ഘടന Curl_easy *data, bool *done)
{
  തിരികെ Curl_cf_ngtcp2_cmn_shutdown(cf, data, done);
}

ഘടന Curl_cftype Curl_cft_h3_proxy = {
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

CURLcode Curl_cf_ngtcp2_proxy_create(ഘടന Curl_cfilter **pcf,
                                     ഘടന Curl_easy *data,
                                     ഘടന Curl_peer *origin,
                                     ഘടന Curl_peer *peer,
                                     uint8_t transport_peer,
                                     ഘടന connectdata *conn,
                                     ഘടന Curl_sockaddr_ex *addr,
                                     ഘടന Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  ഘടന cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  എങ്കിൽ(!tunnel_peer)
    തിരികെ CURLE_FAILED_INIT;
  എങ്കിൽ((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    തിരികെ CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  result = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  എങ്കിൽ(result)
    പോകുക out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  എങ്കിൽ(result)
    പോകുക out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ശൂന്യ, TRNSPRT_QUIC);
  എങ്കിൽ(result)
    പോകുക out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : ശൂന്യ;
  എങ്കിൽ(result) {
    എങ്കിൽ(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    അല്ലെങ്കിൽ എങ്കിൽ(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  അല്ലെങ്കിൽ
    CURL_TRC_CF(data, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  തിരികെ result;
}

CURLcode Curl_cf_ngtcp2_proxy_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                           ഘടന Curl_easy *data,
                                           ഘടന Curl_peer *origin,
                                           ഘടന Curl_peer *peer,
                                           ഘടന Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  ഘടന cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  (ശൂന്യം)data;

  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx)
    പോകുക out;
  result = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  എങ്കിൽ(result)
    പോകുക out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  എങ്കിൽ(result)
    പോകുക out;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_തിരുകുക_after(cf_at, cf);

out:
  എങ്കിൽ(result) {
    എങ്കിൽ(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    അല്ലെങ്കിൽ എങ്കിൽ(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  തിരികെ result;
}

#endif
