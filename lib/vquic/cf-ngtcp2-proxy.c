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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "vquic/cf-ngtcp2-cmn.h"
#अन्तर्भाव "vquic/cf-ngtcp2-proxy.h"
#अन्तर्भाव "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#define PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#define PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_STREAM_CHUNK_SIZE)

प्रकारनिर्देश गणना {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

संरचना h3_tunnel_stream {
  संरचना Curl_peer *dest;  /* where the tunnel goes to */
  संरचना http_resp *resp;
  संरचना bufq recvbuf;
  वर्ण *authority;
  संरचना h3_stream_ctx *stream;
  h3_tunnel_state state;
  BIT(udp);
  BIT(has_final_response);
  BIT(closed);
};

स्थिर CURLcode h3_tunnel_stream_init(संरचना h3_tunnel_stream *ts,
                                      संरचना Curl_peer *dest,
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
  यदि(!ts->authority)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  प्रत्यागम CURLE_OK;
}

स्थिर शून्य h3_tunnel_stream_reset(संरचना h3_tunnel_stream *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = रिक्त;
  ts->stream = रिक्त;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

स्थिर शून्य h3_tunnel_stream_cleanup(संरचना h3_tunnel_stream *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

स्थिर शून्य h3_tunnel_go_state(संरचना Curl_cfilter *cf,
                               संरचना h3_tunnel_stream *ts,
                               h3_tunnel_state new_state,
                               संरचना Curl_easy *data)
{
  VERBOSE(int64_t stream_id = ts->stream ? ts->stream->id : -1);
  (शून्य)cf;

  यदि(ts->state == new_state)
    प्रत्यागम;

  /* leaving this one */
  विकल्प(ts->state) {
  पक्ष H3_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }

  /* entering this one */
  विकल्प(new_state) {
  पक्ष H3_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [init]", stream_id);
    h3_tunnel_stream_reset(ts);
    भङ्ग;
  पक्ष H3_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [connect]", stream_id);
    ts->state = H3_TUNNEL_CONNECT;
    भङ्ग;
  पक्ष H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [response]", stream_id);
    ts->state = H3_TUNNEL_RESPONSE;
    भङ्ग;
  पक्ष H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [established]", stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(data->req.hd_proxy_auth);
    भङ्ग;
  पक्ष H3_TUNNEL_FAILED:
    CURL_TRC_CF(data, cf, "[%" PRId64 "] -> [failed]", stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    भङ्ग;
  }
}

संरचना cf_h3_proxy_ctx {
  संरचना cf_ngtcp2_ctx ngtcp2_ctx;
  संरचना h3_tunnel_stream tunnel; /* our tunnel CONNECT stream */
  BIT(connected);
};

स्थिर CURLcode cf_ngtcp2_proxy_h3_init(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        संरचना cf_ngtcp2_ctx *ctx);

स्थिर CURLcode cf_h3_proxy_ctx_init(संरचना cf_h3_proxy_ctx *ctx,
                                     संरचना Curl_peer *origin,
                                     संरचना Curl_peer *peer,
                                     संरचना ssl_filter_config *sslc,
                                     संरचना Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode result;
  result = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  यदि(!result)
    result = h3_tunnel_stream_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  प्रत्यागम result;
}

स्थिर शून्य cf_h3_proxy_ctx_free(संरचना cf_h3_proxy_ctx *ctx)
{
  यदि(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_stream_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

स्थिर पूर्णाङ्क cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                      uint64_t datalen, शून्य *user_data,
                                      शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना h3_stream_ctx *stream;
  size_t skiplen;
  (शून्य)stream_user_data;

  stream = pctx->tunnel.stream;
  यदि(!stream || (stream->id != stream_id))
    प्रत्यागम 0;

  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  यदि(datalen >= (uint64_t)stream->tx_in_flight_size)
    skiplen = stream->tx_in_flight_size;
  अन्यथा
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_size -= skiplen;

  /* Resume upload processing if we have more data to send */
  यदि(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    पूर्णाङ्क rv = nghttp3_conn_resume_stream(conn, stream_id);
    यदि(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_proxy_stream_close(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, शून्य *user_data,
                                    शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  संरचना h3_stream_ctx *stream;

  (शून्य)conn;
  (शून्य)stream_user_data;
  यदि(!data)
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  यदि(!stream || (stream->id != stream_id))
    प्रत्यागम 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  यदि(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  अन्यथा
    CURL_TRC_CF(data, cf, "[%" PRId64 "] CLOSED", stream->id);
  pctx->tunnel.stream = रिक्त;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(data);
  प्रत्यागम 0;
}

स्थिर शून्य cf_h3_proxy_upd_rx_win(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   संरचना h3_stream_ctx *stream)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  यदि(!stream) /* already closed */
    प्रत्यागम;
  यदि(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    प्रत्यागम;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = stream->rx_offset_max - stream->rx_offset;
  यदि(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    यदि(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    यदि(delta) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  stream->id, cur_win, rx_buffered, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
      ngtcp2_conn_extend_max_offset(ctx->qconn, delta);
    }
  }
}

स्थिर पूर्णाङ्क cb_h3_proxy_recv_data(nghttp3_conn *conn, int64_t stream3_id,
                                 ध्रुव uint8_t *buf, size_t buflen,
                                 शून्य *user_data, शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  संरचना h3_stream_ctx *stream;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  (शून्य)conn;
  (शून्य)stream3_id;
  (शून्य)stream_user_data;

  stream = pctx->tunnel.stream;
  यदि(!data || !stream || (stream->id != stream3_id)) {
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  stream->rx_total += (curl_off_t)buflen;
  CURL_TRC_CF(data, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              stream->id, buflen, stream->rx_total);

  result = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  यदि(result || (nwritten < buflen)) {
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  stream->rx_offset += buflen;
  यदि(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(data, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_h3_proxy_upd_rx_win(cf, data, stream);

  Curl_multi_mark_dirty(data);
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t stream_id,
                                        size_t consumed, शून्य *user_data,
                                        शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (शून्य)conn;
  (शून्य)stream_user_data;

  यदि(!ctx)
    प्रत्यागम 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_proxy_recv_header(nghttp3_conn *conn, int64_t stream_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *मूल्य, uint8_t flags,
                                   शून्य *user_data, शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(मूल्य);
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  संरचना h3_stream_ctx *stream;
  CURLcode result = CURLE_OK;
  पूर्णाङ्क http_status;
  संरचना http_resp *resp;
  (शून्य)conn;
  (शून्य)stream_id;
  (शून्य)token;
  (शून्य)flags;
  (शून्य)stream_user_data;

  /* stream_user_data might be NULL for control streams */
  यदि(!data)
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  यदि(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (शून्य *)data, data ? data->mid : 0);
    प्रत्यागम 0;
  }

  यदि(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    प्रत्यागम 0;
  }

  यदि(token == NGHTTP3_QPACK_TOKEN__STATUS) {
    result = Curl_http_decode_status(&stream->status_code,
                                     (ध्रुव वर्ण *)h3val.base, h3val.len);
    यदि(result)
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    http_status = stream->status_code;
    result = Curl_http_resp_make(&resp, http_status, रिक्त);
    यदि(result)
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    यदि(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  अन्यथा {
    यदि(!pctx->tunnel.resp) {
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] header: %.*s: %.*s", stream_id,
                (पूर्णाङ्क)h3name.len, h3name.base, (पूर्णाङ्क)h3val.len, h3val.base);
    result = Curl_dynhds_add(&pctx->tunnel.resp->headers,
                             (ध्रुव वर्ण *)h3name.base, h3name.len,
                             (ध्रुव वर्ण *)h3val.base, h3val.len);
    यदि(result) {
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_proxy_end_headers(nghttp3_conn *conn, int64_t stream_id,
                                   पूर्णाङ्क fin, शून्य *user_data,
                                   शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  संरचना h3_stream_ctx *stream;
  (शून्य)conn;
  (शून्य)stream_id;
  (शून्य)fin;
  (शून्य)stream_user_data;

  यदि(!data)
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  यदि(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (शून्य *)data, data ? data->mid : 0);
    प्रत्यागम 0;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] end_headers, status=%d", stream_id,
              stream->status_code);

  यदि(!pctx->tunnel.has_final_response) {
    यदि(stream->status_code / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  यदि(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(data);
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, शून्य *user_data,
                                    शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (शून्य)conn;
  (शून्य)stream_user_data;

  यदि(ctx) {
    पूर्णाङ्क rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                              app_error_code);

    यदि(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_proxy_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, शून्य *user_data,
                                    शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  पूर्णाङ्क rv;

  (शून्य)conn;
  (शून्य)stream_user_data;
  यदि(!data)
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;

  यदि(!pctx->tunnel.stream ||
     (stream_id != pctx->tunnel.stream->id))
    प्रत्यागम 0;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  CURL_TRC_CF(data, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  pctx->tunnel.stream = रिक्त;
  pctx->tunnel.closed = TRUE;
  यदि(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  प्रत्यागम 0;
}

स्थिर nghttp3_ssize cb_h3_tunnel_read_data(nghttp3_conn *conn,
                                            int64_t stream_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            शून्य *user_data,
                                            शून्य *stream_user_data)
{
  संरचना Curl_cfilter *cf = user_data;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  संरचना h3_stream_ctx *stream;
  size_t nwritten = 0;
  size_t nvecs = 0;
  ध्रुव unsigned वर्ण *buf_base;

  (शून्य)conn;
  (शून्य)stream_id;
  (शून्य)veccnt;
  (शून्य)stream_user_data;
  (शून्य)pflags;

  stream = pctx->tunnel.stream;
  यदि(!data || !stream || (stream->id != stream_id))
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  यदि(stream->tx_in_flight_size < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    यावत्(nvecs < veccnt) {
      यदि(!Curl_bufq_peek_at(&stream->sendbuf,
                           stream->tx_in_flight_size,
                           &buf_base,
                           &vec[nvecs].len))
        भङ्ग;
      vec[nvecs].base = (uint8_t *)(uintptr_t)buf_base;
      stream->tx_in_flight_size += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  यदि(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> AGAIN",
                stream->id);
    प्रत्यागम NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(data, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              stream->id, nvecs,
              *pflags == NGHTTP3_DATA_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf));
  प्रत्यागम (nghttp3_ssize)nvecs;
}

#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ढकला
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
स्थिर nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_stream_close,
  cb_h3_proxy_recv_data,
  cb_h3_proxy_deferred_consume,
  रिक्त, /* begin_headers */
  cb_h3_proxy_recv_header,
  cb_h3_proxy_end_headers,
  रिक्त, /* begin_trailers */
  cb_h3_proxy_recv_header,
  रिक्त, /* end_trailers */
  cb_h3_proxy_stop_sending,
  रिक्त, /* end_stream */
  cb_h3_proxy_reset_stream,
  रिक्त, /* shutdown */
  रिक्त, /* recv_settings (deprecated) */
#ifdef NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  रिक्त, /* recv_origin */
  रिक्त, /* end_origin */
  रिक्त, /* rand */
#endif
#ifdef NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  रिक्त, /* recv_settings2 */
#endif
#ifdef NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  रिक्त, /* stream_close2 */
#endif
};
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic काढा
#endif

स्थिर CURLcode cf_ngtcp2_proxy_h3_init(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        संरचना cf_ngtcp2_ctx *ctx)
{
  पूर्णाङ्क rc;

  यदि(ngtcp2_conn_get_streams_uni_डावा(ctx->qconn) < 3) {
    failf(data, "QUIC connection lacks 3 uni streams to run HTTP/3");
    प्रत्यागम CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  यदि(rc) {
    failf(data, "error creating nghttp3 connection instance");
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  प्रत्यागम Curl_cf_ngtcp2_h3_init_ctrls(ctx, data);
}

स्थिर ssize_t cf_h3_proxy_recv_closed_stream(संरचना Curl_cfilter *cf,
                                              संरचना Curl_easy *data,
                                              संरचना h3_stream_ctx *stream,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  यदि(stream->reset) {
    यदि(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn);
      data->state.refused_stream = TRUE;
      *err = CURLE_RECV_ERROR;
      गच्छ out;
    }
    अन्यथा यदि(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", stream->id, stream->error3,
                  Curl_vquic_h3_err_str(stream->error3));
      nread = 0;
      गच्छ out;
    }
    failf(data, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    *err = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    गच्छ out;
  }
  अन्यथा यदि(!stream->resp_hds_complete) {
    failf(data,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    *err = CURLE_HTTP3;
    गच्छ out;
  }
  nread = 0;

out:
  प्रत्यागम nread;
}

स्थिर CURLcode cf_h3_proxy_sendbuf_add(संरचना Curl_easy *data,
                                        संरचना h3_stream_ctx *stream,
                                        ध्रुव uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode result;
  *pnwritten = 0;
  (शून्य)data;

  result = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h3_proxy_send(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 ध्रुव uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  संरचना h3_stream_ctx *stream = रिक्त;
  संरचना cf_call_data save;
  संरचना cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  यदि(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    गच्छ denied;
  }

  (शून्य)eos; /* use for stream EOF and block handling */
  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  यदि(result)
    गच्छ out;

  यदि(pctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    गच्छ denied;
  }

  stream = pctx->tunnel.stream;
  यदि(!stream) {
    result = CURLE_SEND_ERROR;
    गच्छ denied;
  }

  यदि(stream->closed) {
    यदि(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      गच्छ out;
    }
    CURL_TRC_CF(data, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    गच्छ out;
  }
  अन्यथा {
    result = cf_h3_proxy_sendbuf_add(data, stream, buf, len, pnwritten);
    CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (पूर्णाङ्क)result, *pnwritten);
    यदि(result)
      गच्छ out;
    (शून्य)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  यदि(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!result);
  result = Curl_cf_ngtcp2_progress_egress(cf, data, &pktx);

out:
  result = Curl_1st_fatal(result,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, data, &pktx));
denied:
  CURL_TRC_CF(data, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (पूर्णाङ्क)result, *pnwritten);
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

/* incoming data frames on the h3 stream */
स्थिर CURLcode cf_h3_proxy_recv(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 वर्ण *buf, size_t len, size_t *pnread)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  संरचना h3_stream_ctx *stream = pctx->tunnel.stream;
  संरचना cf_call_data save;
  संरचना cf_ngtcp2_io_ctx pktx;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  यदि(ctx->tls_vrfy_result) {
    result = ctx->tls_vrfy_result;
    गच्छ denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, data);

  यदि(!stream || ctx->shutdown_started) {
    result = CURLE_RECV_ERROR;
    गच्छ out;
  }

  यदि(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    यदि(result)
      गच्छ out;
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  result = Curl_cf_ngtcp2_progress_ingress(cf, data, &pktx);
  यदि(result)
    गच्छ out;

  /* inbufq had nothing before, maybe after progressing ingress? */
  यदि(!*pnread && !Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    result = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    यदि(result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  stream->id, len, *pnread, (पूर्णाङ्क)result);
      गच्छ out;
    }
    cf_h3_proxy_upd_rx_win(cf, data, pctx->tunnel.stream);
  }

  यदि(*pnread) {
    Curl_multi_mark_dirty(data);
  }
  अन्यथा {
    यदि(stream->xfer_result) {
      CURL_TRC_CF(data, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, data, stream);
      result = stream->xfer_result;
      गच्छ out;
    }
    अन्यथा यदि(stream->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_stream(cf, data, stream, &result);
      यदि(nread > 0)
        *pnread = (size_t)nread;
      गच्छ out;
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
              stream ? stream->id : -1, len, (पूर्णाङ्क)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h3_proxy_submit(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   संरचना h3_tunnel_stream *ts,
                                   संरचना httpreq *req)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  संरचना h3_stream_ctx *stream = रिक्त;
  संरचना dynhds h2_headers;
  nghttp3_nv *nva = रिक्त;
  size_t nheader;
  पूर्णाङ्क rc = 0;
  unsigned पूर्णाङ्क i;
  nghttp3_data_reader reader;
  nghttp3_data_reader *preader = रिक्त;
  CURLcode result;

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  यदि(result)
    गच्छ out;

  result = Curl_cf_ngtcp2_h3_stream_setup(cf, data, &stream);
  यदि(result)
    गच्छ out;

  nheader = Curl_dynhds_मोजणी(&h2_headers);
  nva = curlx_malloc(परिमाण(nghttp3_nv) * nheader);
  यदि(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  प्रति(i = 0; i < nheader; ++i) {
    संरचना dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned वर्ण *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].मूल्य = (unsigned वर्ण *)e->मूल्य;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  /* Open a bidirectional stream */
  {
    int64_t sid;
    पूर्णाङ्क rv;

    DEBUGASSERT(stream->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, रिक्त);
    यदि(rv) {
      failf(data, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      result = CURLE_SEND_ERROR;
      गच्छ out;
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

  यदि(rc) {
    विकल्प(rc) {
    पक्ष NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      भङ्ग;
    स्वभाव:
      CURL_TRC_CF(data, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  stream->id, rc, nghttp3_strerror(rc));
      भङ्ग;
    }
    result = CURLE_SEND_ERROR;
    गच्छ out;
  }

  यदि(Curl_trc_is_verbose(data)) {
    CURL_TRC_CF(data, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", stream->id,
                Curl_bufref_ptr(&data->state.url));
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h3_proxy_adjust_pollset(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *data,
                                           संरचना easy_pollset *ps)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, data);

  यदि(!ctx->qconn || !pctx->tunnel.stream || (sock == CURL_SOCKET_BAD))
    प्रत्यागम CURLE_OK;

  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  यदि(want_recv || want_send || !Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    संरचना h3_stream_ctx *stream = pctx->tunnel.stream;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_डावा(ctx->qconn) ||
                 !ngtcp2_conn_get_max_data_डावा(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                stream->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
  }
  प्रत्यागम result;
}

स्थिर bool cf_h3_proxy_data_pending(संरचना Curl_cfilter *cf,
                                     ध्रुव संरचना Curl_easy *data)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  यदि(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf))
    प्रत्यागम TRUE;
  प्रत्यागम cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

स्थिर CURLcode cf_h3_proxy_submit_CONNECT(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *data,
                                           संरचना h3_tunnel_stream *ts)
{
  संरचना cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  संरचना httpreq *req = रिक्त;

  result = Curl_http_proxy_create_tunnel_request(
    &req, cf, data, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  यदि(!result)
    result = Curl_creader_set_null(data);
  यदि(!result)
    result = cf_h3_proxy_submit(cf, data, ts, req);

  यदि(req)
    Curl_http_req_free(req);
  यदि(result)
    failf(data, "Failed sending CONNECT to proxy");
  प्रत्यागम result;
}

स्थिर CURLcode cf_h3_proxy_inspect_response(संरचना Curl_cfilter *cf,
                                             संरचना Curl_easy *data,
                                             संरचना h3_tunnel_stream *ts)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)pctx->tunnel.udp, &res);
  यदि(result)
    प्रत्यागम result;
  विकल्प(res) {
  पक्ष PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, data);
    भङ्ग;
  पक्ष PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
    result = CURLE_COULDNT_CONNECT;
    भङ्ग;
  पक्ष PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, data);
    भङ्ग;
  }
  प्रत्यागम result;
}

स्थिर CURLcode cf_h3_proxy_tunnel(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   संरचना h3_tunnel_stream *ts,
                                   bool *pdone)
{
  संरचना cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  कुरु {
    विकल्प(ts->state) {
    पक्ष H3_TUNNEL_INIT:
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = cf_h3_proxy_submit_CONNECT(cf, data, ts);
      यदि(result)
        गच्छ out;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, data);

      result = Curl_cf_ngtcp2_progress_egress(cf, data, रिक्त);
      यदि(result)
        गच्छ out;
      FALLTHROUGH();

    पक्ष H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      result = Curl_cf_ngtcp2_progress_ingress(cf, data, रिक्त);
      यदि(result)
        गच्छ out;
      result = Curl_cf_ngtcp2_progress_egress(cf, data, रिक्त);
      यदि(result && result != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
        गच्छ out;
      }

      यदि(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, data);
      }
      अन्यथा {
        /* Not done yet, return and let multi interface call us again */
        result = CURLE_OK;
        गच्छ out;
      }
      FALLTHROUGH();

    पक्ष H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = cf_h3_proxy_inspect_response(cf, data, ts);
      यदि(result)
        गच्छ out;
      ctx->connected = TRUE;
      भङ्ग;

    पक्ष H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      प्रत्यागम CURLE_OK;

    पक्ष H3_TUNNEL_FAILED:
      प्रत्यागम CURLE_RECV_ERROR;

    स्वभाव:
      भङ्ग;
    }

  } यावत्(ts->state == H3_TUNNEL_INIT);

out:
  यदि((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, data);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h3_proxy_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data, bool *done)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_call_data save = { 0 };
  CURLcode result = CURLE_OK;
  संरचना h3_tunnel_stream *ts = &pctx->tunnel;
  bool data_saved = FALSE;

  result = Curl_cf_ngtcp2_cmn_connect(cf, data, done);
  यदि(result || !*done)
    गच्छ out;

  CF_DATA_SAVE(save, cf, data);
  data_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  result = cf_h3_proxy_tunnel(cf, data, ts, done);

out:
  यदि(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }

  यदि(data_saved)
    CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर शून्य cf_h3_proxy_destroy(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data)
{
  संरचना cf_h3_proxy_ctx *ctx = cf->ctx;

  (शून्य)data;
  यदि(ctx) {
    CURL_TRC_CF(data, cf, "cf_h3_proxy_destroy()");
    यदि(ctx->ngtcp2_ctx.qconn) {
      संरचना cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      Curl_cf_ngtcp2_cmn_conn_close(cf, data);
      CF_DATA_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = रिक्त;
  }
}

स्थिर CURLcode cf_h3_proxy_shutdown(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data, bool *done)
{
  प्रत्यागम Curl_cf_ngtcp2_cmn_shutdown(cf, data, done);
}

संरचना Curl_cftype Curl_cft_h3_proxy = {
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

CURLcode Curl_cf_ngtcp2_proxy_create(संरचना Curl_cfilter **pcf,
                                     संरचना Curl_easy *data,
                                     संरचना Curl_peer *origin,
                                     संरचना Curl_peer *peer,
                                     uint8_t transport_peer,
                                     संरचना connectdata *conn,
                                     संरचना Curl_sockaddr_ex *addr,
                                     संरचना Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  यदि(!tunnel_peer)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    प्रत्यागम CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  result = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  यदि(result)
    गच्छ out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  यदि(result)
    गच्छ out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, रिक्त, TRNSPRT_QUIC);
  यदि(result)
    गच्छ out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : रिक्त;
  यदि(result) {
    यदि(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    अन्यथा यदि(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  अन्यथा
    CURL_TRC_CF(data, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  प्रत्यागम result;
}

CURLcode Curl_cf_ngtcp2_proxy_घाला_after(संरचना Curl_cfilter *cf_at,
                                           संरचना Curl_easy *data,
                                           संरचना Curl_peer *origin,
                                           संरचना Curl_peer *peer,
                                           संरचना Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_h3_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  (शून्य)data;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx)
    गच्छ out;
  result = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  यदि(result)
    गच्छ out;

  result = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  यदि(result)
    गच्छ out;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_घाला_after(cf_at, cf);

out:
  यदि(result) {
    यदि(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    अन्यथा यदि(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  प्रत्यागम result;
}

#endif
