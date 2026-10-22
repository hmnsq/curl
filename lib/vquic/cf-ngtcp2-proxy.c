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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#समावेश "urldata.h"
#समावेश "url.h"
#समावेश "curl_trc.h"
#समावेश "sendf.h"
#समावेश "multiif.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "progress.h"
#समावेश "curlx/dynbuf.h"
#समावेश "http_proxy.h"
#समावेश "vquic/vquic.h"
#समावेश "vquic/cf-ngtcp2-cmn.h"
#समावेश "vquic/cf-ngtcp2-proxy.h"
#समावेश "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#घोषणा PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#घोषणा PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_STREAM_CHUNK_आकार)

प्रकार क्रमागत {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

काष्ठा h3_tunnel_stream {
  काष्ठा Curl_peer *dest;  /* where the tunnel goes to */
  काष्ठा http_resp *resp;
  काष्ठा bufq recvbuf;
  अक्षर *authority;
  काष्ठा h3_stream_ctx *stream;
  h3_tunnel_state state;
  BIT(udp);
  BIT(has_final_response);
  BIT(closed);
};

अटल CURLcode h3_tunnel_stream_init(काष्ठा h3_tunnel_stream *ts,
                                      काष्ठा Curl_peer *dest,
                                      bool udp)
{
  ts->state = H3_TUNNEL_INIT;
  Curl_peer_link(&ts->dest, dest);
  Curl_bufq_init2(&ts->recvbuf, H3_STREAM_CHUNK_आकार,
                  PROXY_H3_TUNNEL_RECV_CHUNKS, BUFQ_OPT_SOFT_LIMIT);
  ts->udp = udp;
  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->port);
  अगर(!ts->authority)
    वापस CURLE_OUT_OF_MEMORY;

  वापस CURLE_OK;
}

अटल व्योम h3_tunnel_stream_reset(काष्ठा h3_tunnel_stream *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = शून्य;
  ts->stream = शून्य;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

अटल व्योम h3_tunnel_stream_cleanup(काष्ठा h3_tunnel_stream *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

अटल व्योम h3_tunnel_go_state(काष्ठा Curl_cfilter *cf,
                               काष्ठा h3_tunnel_stream *ts,
                               h3_tunnel_state new_state,
                               काष्ठा Curl_easy *तथ्य)
{
  VERBOSE(int64_t stream_id = ts->stream ? ts->stream->id : -1);
  (व्योम)cf;

  अगर(ts->state == new_state)
    वापस;

  /* leaving this one */
  चयन(ts->state) {
  हाल H3_TUNNEL_CONNECT:
    तथ्य->req.ignorebody = FALSE;
    अवरोध;
  शेष:
    अवरोध;
  }

  /* entering this one */
  चयन(new_state) {
  हाल H3_TUNNEL_INIT:
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] -> [init]", stream_id);
    h3_tunnel_stream_reset(ts);
    अवरोध;
  हाल H3_TUNNEL_CONNECT:
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] -> [connect]", stream_id);
    ts->state = H3_TUNNEL_CONNECT;
    अवरोध;
  हाल H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] -> [response]", stream_id);
    ts->state = H3_TUNNEL_RESPONSE;
    अवरोध;
  हाल H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] -> [established]", stream_id);
    infof(तथ्य, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    तथ्य->state.authproxy.done = TRUE;
    तथ्य->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(तथ्य->req.hd_proxy_auth);
    अवरोध;
  हाल H3_TUNNEL_FAILED:
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] -> [failed]", stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(तथ्य->req.hd_proxy_auth);
    अवरोध;
  }
}

काष्ठा cf_h3_proxy_ctx {
  काष्ठा cf_ngtcp2_ctx ngtcp2_ctx;
  काष्ठा h3_tunnel_stream tunnel; /* our tunnel CONNECT stream */
  BIT(connected);
};

अटल CURLcode cf_ngtcp2_proxy_h3_init(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        काष्ठा cf_ngtcp2_ctx *ctx);

अटल CURLcode cf_h3_proxy_ctx_init(काष्ठा cf_h3_proxy_ctx *ctx,
                                     काष्ठा Curl_peer *origin,
                                     काष्ठा Curl_peer *peer,
                                     काष्ठा ssl_filter_config *sslc,
                                     काष्ठा Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode परिणाम;
  परिणाम = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  अगर(!परिणाम)
    परिणाम = h3_tunnel_stream_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  वापस परिणाम;
}

अटल व्योम cf_h3_proxy_ctx_free(काष्ठा cf_h3_proxy_ctx *ctx)
{
  अगर(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_stream_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

अटल पूर्णांक cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t stream_id,
                                      uint64_t datalen, व्योम *user_तथ्य,
                                      व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream;
  माप_प्रकार skiplen;
  (व्योम)stream_user_तथ्य;

  stream = pctx->tunnel.stream;
  अगर(!stream || (stream->id != stream_id))
    वापस 0;

  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  अगर(datalen >= (uint64_t)stream->tx_in_flight_आकार)
    skiplen = stream->tx_in_flight_आकार;
  अन्यथा
    skiplen = (माप_प्रकार)datalen;
  Curl_bufq_skip(&stream->sendbuf, skiplen);
  stream->tx_in_flight_आकार -= skiplen;

  /* Resume upload processing if we have more data to send */
  अगर(stream->tx_in_flight_आकार < Curl_bufq_len(&stream->sendbuf)) {
    पूर्णांक rv = nghttp3_conn_resume_stream(conn, stream_id);
    अगर(rv && rv != NGHTTP3_ERR_STREAM_NOT_FOUND) {
      वापस NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  वापस 0;
}

अटल पूर्णांक cb_h3_proxy_stream_close(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, व्योम *user_तथ्य,
                                    व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  काष्ठा h3_stream_ctx *stream;

  (व्योम)conn;
  (व्योम)stream_user_तथ्य;
  अगर(!तथ्य)
    वापस NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  अगर(!stream || (stream->id != stream_id))
    वापस 0;

  stream->closed = TRUE;
  stream->error3 = app_error_code;
  अगर(stream->error3 != NGHTTP3_H3_NO_ERROR) {
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                stream->id, stream->error3);
  }
  अन्यथा
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] CLOSED", stream->id);
  pctx->tunnel.stream = शून्य;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(तथ्य);
  वापस 0;
}

अटल व्योम cf_h3_proxy_upd_rx_win(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा h3_stream_ctx *stream)
{
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  माप_प्रकार rx_buffered;

  अगर(!stream) /* already closed */
    वापस;
  अगर(stream->rx_offset_max < stream->rx_offset) {
    DEBUGASSERT(0);
    वापस;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = stream->rx_offset_max - stream->rx_offset;
  अगर(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    अगर(UINT64_MAX - delta < stream->rx_offset_max)
      delta = UINT64_MAX - stream->rx_offset_max;
    अगर(delta) {
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  stream->id, cur_win, rx_buffered, delta);
      stream->rx_offset_max += delta;
      ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream->id, delta);
      ngtcp2_conn_extend_max_offset(ctx->qconn, delta);
    }
  }
}

अटल पूर्णांक cb_h3_proxy_recv_तथ्य(nghttp3_conn *conn, int64_t stream3_id,
                                 स्थिर uint8_t *buf, माप_प्रकार buflen,
                                 व्योम *user_तथ्य, व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  काष्ठा h3_stream_ctx *stream;
  माप_प्रकार nwritten;
  CURLcode परिणाम = CURLE_OK;
  (व्योम)conn;
  (व्योम)stream3_id;
  (व्योम)stream_user_तथ्य;

  stream = pctx->tunnel.stream;
  अगर(!तथ्य || !stream || (stream->id != stream3_id)) {
    वापस NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  stream->rx_योग += (curl_off_t)buflen;
  CURL_TRC_CF(तथ्य, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              stream->id, buflen, stream->rx_योग);

  परिणाम = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  अगर(परिणाम || (nwritten < buflen)) {
    वापस NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  stream->rx_offset += buflen;
  अगर(stream->rx_offset_max < stream->rx_offset)
    stream->rx_offset_max = stream->rx_offset;

  CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              stream->id, buflen, stream->rx_offset_max - stream->rx_offset);
  cf_h3_proxy_upd_rx_win(cf, तथ्य, stream);

  Curl_multi_mark_dirty(तथ्य);
  वापस 0;
}

अटल पूर्णांक cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t stream_id,
                                        माप_प्रकार consumed, व्योम *user_तथ्य,
                                        व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (व्योम)conn;
  (व्योम)stream_user_तथ्य;

  अगर(!ctx)
    वापस 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_stream_offset(ctx->qconn, stream_id, consumed);
  ngtcp2_conn_extend_max_offset(ctx->qconn, consumed);

  वापस 0;
}

अटल पूर्णांक cb_h3_proxy_recv_header(nghttp3_conn *conn, int64_t stream_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *मूल्य, uint8_t flags,
                                   व्योम *user_तथ्य, व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(मूल्य);
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  काष्ठा h3_stream_ctx *stream;
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक http_status;
  काष्ठा http_resp *resp;
  (व्योम)conn;
  (व्योम)stream_id;
  (व्योम)token;
  (व्योम)flags;
  (व्योम)stream_user_तथ्य;

  /* stream_user_data might be NULL for control streams */
  अगर(!तथ्य)
    वापस NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  अगर(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (व्योम *)तथ्य, तथ्य ? तथ्य->mid : 0);
    वापस 0;
  }

  अगर(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    वापस 0;
  }

  अगर(token == NGHTTP3_QPACK_TOKEN__STATUS) {
    परिणाम = Curl_http_decode_status(&stream->status_code,
                                     (स्थिर अक्षर *)h3val.base, h3val.len);
    अगर(परिणाम)
      वापस NGHTTP3_ERR_CALLBACK_FAILURE;
    http_status = stream->status_code;
    परिणाम = Curl_http_resp_make(&resp, http_status, शून्य);
    अगर(परिणाम)
      वापस NGHTTP3_ERR_CALLBACK_FAILURE;
    अगर(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  अन्यथा {
    अगर(!pctx->tunnel.resp) {
      वापस NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] header: %.*s: %.*s", stream_id,
                (पूर्णांक)h3name.len, h3name.base, (पूर्णांक)h3val.len, h3val.base);
    परिणाम = Curl_dynhds_add(&pctx->tunnel.resp->headers,
                             (स्थिर अक्षर *)h3name.base, h3name.len,
                             (स्थिर अक्षर *)h3val.base, h3val.len);
    अगर(परिणाम) {
      वापस NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  वापस 0;
}

अटल पूर्णांक cb_h3_proxy_end_headers(nghttp3_conn *conn, int64_t stream_id,
                                   पूर्णांक fin, व्योम *user_तथ्य,
                                   व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  काष्ठा h3_stream_ctx *stream;
  (व्योम)conn;
  (व्योम)stream_id;
  (व्योम)fin;
  (व्योम)stream_user_तथ्य;

  अगर(!तथ्य)
    वापस NGHTTP3_ERR_CALLBACK_FAILURE;

  stream = pctx->tunnel.stream;
  अगर(!stream || (stream->id != stream_id)) {
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                stream_id, (व्योम *)तथ्य, तथ्य ? तथ्य->mid : 0);
    वापस 0;
  }

  CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] end_headers, status=%d", stream_id,
              stream->status_code);

  अगर(!pctx->tunnel.has_final_response) {
    अगर(stream->status_code / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  अगर(stream->status_code / 100 != 1) {
    stream->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(तथ्य);
  वापस 0;
}

अटल पूर्णांक cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, व्योम *user_तथ्य,
                                    व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (व्योम)conn;
  (व्योम)stream_user_तथ्य;

  अगर(ctx) {
    पूर्णांक rv = ngtcp2_conn_shutdown_stream_read(ctx->qconn, 0, stream_id,
                                              app_error_code);

    अगर(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
      वापस NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  वापस 0;
}

अटल पूर्णांक cb_h3_proxy_reset_stream(nghttp3_conn *conn, int64_t stream_id,
                                    uint64_t app_error_code, व्योम *user_तथ्य,
                                    व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  पूर्णांक rv;

  (व्योम)conn;
  (व्योम)stream_user_तथ्य;
  अगर(!तथ्य)
    वापस NGHTTP3_ERR_CALLBACK_FAILURE;

  अगर(!pctx->tunnel.stream ||
     (stream_id != pctx->tunnel.stream->id))
    वापस 0;

  rv = ngtcp2_conn_shutdown_stream_write(ctx->qconn, 0, stream_id,
                                         app_error_code);
  CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] reset -> %d", stream_id, rv);
  pctx->tunnel.stream = शून्य;
  pctx->tunnel.closed = TRUE;
  अगर(rv && rv != NGTCP2_ERR_STREAM_NOT_FOUND) {
    वापस NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  वापस 0;
}

अटल nghttp3_ssize cb_h3_tunnel_read_तथ्य(nghttp3_conn *conn,
                                            int64_t stream_id,
                                            nghttp3_vec *vec,
                                            माप_प्रकार veccnt,
                                            uint32_t *pflags,
                                            व्योम *user_तथ्य,
                                            व्योम *stream_user_तथ्य)
{
  काष्ठा Curl_cfilter *cf = user_तथ्य;
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  काष्ठा h3_stream_ctx *stream;
  माप_प्रकार nwritten = 0;
  माप_प्रकार nvecs = 0;
  स्थिर अचिन्हित अक्षर *buf_base;

  (व्योम)conn;
  (व्योम)stream_id;
  (व्योम)veccnt;
  (व्योम)stream_user_तथ्य;
  (व्योम)pflags;

  stream = pctx->tunnel.stream;
  अगर(!तथ्य || !stream || (stream->id != stream_id))
    वापस NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  अगर(stream->tx_in_flight_आकार < Curl_bufq_len(&stream->sendbuf)) {
    nvecs = 0;
    जबतक(nvecs < veccnt) {
      अगर(!Curl_bufq_peek_at(&stream->sendbuf,
                           stream->tx_in_flight_आकार,
                           &buf_base,
                           &vec[nvecs].len))
        अवरोध;
      vec[nvecs].base = (uint8_t *)(uintptr_t)buf_base;
      stream->tx_in_flight_आकार += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  अगर(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] read req body -> AGAIN",
                stream->id);
    वापस NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              stream->id, nvecs,
              *pflags == NGHTTP3_तथ्य_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&stream->sendbuf));
  वापस (nghttp3_ssize)nvecs;
}

#अगर_घोषित CURL_HAVE_DIAG
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wmissing-field-initializers"
#पूर्ण_अगर
अटल nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_stream_close,
  cb_h3_proxy_recv_तथ्य,
  cb_h3_proxy_deferred_consume,
  शून्य, /* begin_headers */
  cb_h3_proxy_recv_header,
  cb_h3_proxy_end_headers,
  शून्य, /* begin_trailers */
  cb_h3_proxy_recv_header,
  शून्य, /* end_trailers */
  cb_h3_proxy_stop_sending,
  शून्य, /* end_stream */
  cb_h3_proxy_reset_stream,
  शून्य, /* shutdown */
  शून्य, /* recv_settings (deprecated) */
#अगर_घोषित NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  शून्य, /* recv_origin */
  शून्य, /* end_origin */
  शून्य, /* rand */
#पूर्ण_अगर
#अगर_घोषित NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  शून्य, /* recv_settings2 */
#पूर्ण_अगर
#अगर_घोषित NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  शून्य, /* stream_close2 */
#पूर्ण_अगर
};
#अगर_घोषित CURL_HAVE_DIAG
#आशय GCC diagnostic निकालो
#पूर्ण_अगर

अटल CURLcode cf_ngtcp2_proxy_h3_init(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        काष्ठा cf_ngtcp2_ctx *ctx)
{
  पूर्णांक rc;

  अगर(ngtcp2_conn_get_streams_uni_बायाँ(ctx->qconn) < 3) {
    failf(तथ्य, "QUIC connection lacks 3 uni streams to run HTTP/3");
    वापस CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  अगर(rc) {
    failf(तथ्य, "error creating nghttp3 connection instance");
    वापस CURLE_OUT_OF_MEMORY;
  }

  वापस Curl_cf_ngtcp2_h3_init_ctrls(ctx, तथ्य);
}

अटल ssize_t cf_h3_proxy_recv_closed_stream(काष्ठा Curl_cfilter *cf,
                                              काष्ठा Curl_easy *तथ्य,
                                              काष्ठा h3_stream_ctx *stream,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  अगर(stream->reset) {
    अगर(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(तथ्य, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn);
      तथ्य->state.refused_stream = TRUE;
      *err = CURLE_RECV_ERROR;
      जाओ out;
    }
    अन्यथा अगर(stream->resp_hds_complete && तथ्य->req.no_body) {
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", stream->id, stream->error3,
                  Curl_vquic_h3_err_str(stream->error3));
      nread = 0;
      जाओ out;
    }
    failf(तथ्य, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    *err = तथ्य->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    जाओ out;
  }
  अन्यथा अगर(!stream->resp_hds_complete) {
    failf(तथ्य,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    *err = CURLE_HTTP3;
    जाओ out;
  }
  nread = 0;

out:
  वापस nread;
}

अटल CURLcode cf_h3_proxy_sendbuf_add(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा h3_stream_ctx *stream,
                                        स्थिर uint8_t *buf, माप_प्रकार len,
                                        माप_प्रकार *pnwritten)
{
  CURLcode परिणाम;
  *pnwritten = 0;
  (व्योम)तथ्य;

  परिणाम = Curl_bufq_write(&stream->sendbuf, buf, len, pnwritten);
  वापस परिणाम;
}

अटल CURLcode cf_h3_proxy_send(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 स्थिर uint8_t *buf, माप_प्रकार len,
                                 bool eos, माप_प्रकार *pnwritten)
{
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  काष्ठा h3_stream_ctx *stream = शून्य;
  काष्ठा cf_call_तथ्य save;
  काष्ठा cf_ngtcp2_io_ctx pktx;
  CURLcode परिणाम = CURLE_OK;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, तथ्य);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  अगर(ctx->tls_vrfy_परिणाम) {
    परिणाम = ctx->tls_vrfy_परिणाम;
    जाओ denied;
  }

  (व्योम)eos; /* use for stream EOF and block handling */
  परिणाम = Curl_cf_ngtcp2_progress_ingress(cf, तथ्य, &pktx);
  अगर(परिणाम)
    जाओ out;

  अगर(pctx->tunnel.closed) {
    परिणाम = CURLE_SEND_ERROR;
    जाओ denied;
  }

  stream = pctx->tunnel.stream;
  अगर(!stream) {
    परिणाम = CURLE_SEND_ERROR;
    जाओ denied;
  }

  अगर(stream->closed) {
    अगर(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", stream->id);
      परिणाम = CURLE_OK;
      *pnwritten = len;
      जाओ out;
    }
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    परिणाम = CURLE_HTTP3;
    जाओ out;
  }
  अन्यथा {
    परिणाम = cf_h3_proxy_sendbuf_add(तथ्य, stream, buf, len, pnwritten);
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                stream->id, len, (पूर्णांक)परिणाम, *pnwritten);
    अगर(परिणाम)
      जाओ out;
    (व्योम)nghttp3_conn_resume_stream(ctx->h3conn, stream->id);
  }

  अगर(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!परिणाम);
  परिणाम = Curl_cf_ngtcp2_progress_egress(cf, तथ्य, &pktx);

out:
  परिणाम = Curl_1st_fatal(परिणाम,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, तथ्य, &pktx));
denied:
  CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (पूर्णांक)परिणाम, *pnwritten);
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

/* incoming data frames on the h3 stream */
अटल CURLcode cf_h3_proxy_recv(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  काष्ठा h3_stream_ctx *stream = pctx->tunnel.stream;
  काष्ठा cf_call_तथ्य save;
  काष्ठा cf_ngtcp2_io_ctx pktx;
  CURLcode परिणाम = CURLE_OK;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  अगर(ctx->tls_vrfy_परिणाम) {
    परिणाम = ctx->tls_vrfy_परिणाम;
    जाओ denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, तथ्य);

  अगर(!stream || ctx->shutdown_started) {
    परिणाम = CURLE_RECV_ERROR;
    जाओ out;
  }

  अगर(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    परिणाम = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    अगर(परिणाम)
      जाओ out;
    cf_h3_proxy_upd_rx_win(cf, तथ्य, pctx->tunnel.stream);
  }

  परिणाम = Curl_cf_ngtcp2_progress_ingress(cf, तथ्य, &pktx);
  अगर(परिणाम)
    जाओ out;

  /* inbufq had nothing before, maybe after progressing ingress? */
  अगर(!*pnread && !Curl_bufq_is_empty(&pctx->tunnel.recvbuf)) {
    परिणाम = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    अगर(परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  stream->id, len, *pnread, (पूर्णांक)परिणाम);
      जाओ out;
    }
    cf_h3_proxy_upd_rx_win(cf, तथ्य, pctx->tunnel.stream);
  }

  अगर(*pnread) {
    Curl_multi_mark_dirty(तथ्य);
  }
  अन्यथा {
    अगर(stream->xfer_परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] xfer write failed", stream->id);
      Curl_cf_ngtcp2_h3_stream_close(cf, तथ्य, stream);
      परिणाम = stream->xfer_परिणाम;
      जाओ out;
    }
    अन्यथा अगर(stream->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_stream(cf, तथ्य, stream, &परिणाम);
      अगर(nread > 0)
        *pnread = (माप_प्रकार)nread;
      जाओ out;
    }
    परिणाम = CURLE_AGAIN;
  }

out:
  परिणाम = Curl_1st_fatal(परिणाम,
                          Curl_cf_ngtcp2_progress_egress(cf, तथ्य, &pktx));
  परिणाम = Curl_1st_fatal(परिणाम,
                          Curl_cf_ngtcp2_cmn_set_expiry(cf, तथ्य, &pktx));
denied:
  CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] cf_recv(len=%zu) -> %d, %zu",
              stream ? stream->id : -1, len, (पूर्णांक)परिणाम, *pnread);
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल CURLcode cf_h3_proxy_submit(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा h3_tunnel_stream *ts,
                                   काष्ठा httpreq *req)
{
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  काष्ठा h3_stream_ctx *stream = शून्य;
  काष्ठा dynhds h2_headers;
  nghttp3_nv *nva = शून्य;
  माप_प्रकार nheader;
  पूर्णांक rc = 0;
  अचिन्हित पूर्णांक i;
  nghttp3_तथ्य_reader reader;
  nghttp3_तथ्य_reader *preader = शून्य;
  CURLcode परिणाम;

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);
  परिणाम = Curl_http_req_to_h2(&h2_headers, req, तथ्य);
  अगर(परिणाम)
    जाओ out;

  परिणाम = Curl_cf_ngtcp2_h3_stream_setup(cf, तथ्य, &stream);
  अगर(परिणाम)
    जाओ out;

  nheader = Curl_dynhds_गिनती(&h2_headers);
  nva = curlx_malloc(माप(nghttp3_nv) * nheader);
  अगर(!nva) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  क्रम(i = 0; i < nheader; ++i) {
    काष्ठा dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (अचिन्हित अक्षर *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].मूल्य = (अचिन्हित अक्षर *)e->मूल्य;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  /* Open a bidirectional stream */
  {
    int64_t sid;
    पूर्णांक rv;

    DEBUGASSERT(stream->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_stream(ctx->qconn, &sid, शून्य);
    अगर(rv) {
      failf(तथ्य, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
    stream->id = sid;
    ts->stream = stream;
    ++ctx->used_bidi_streams;
    CURL_TRC_CF(तथ्य, cf, "[%" PRId64 "] opened bidi stream", sid);
  }

  /* CONNECT-UDP request stream remains open for capsules, no fixed EOF. */
  stream->send_closed = 0;
  reader.read_तथ्य = cb_h3_tunnel_read_तथ्य;
  preader = &reader;

  rc = nghttp3_conn_submit_request(ctx->h3conn, stream->id,
                                   nva, nheader, preader, stream);

  अगर(rc) {
    चयन(rc) {
    हाल NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(तथ्य, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", stream->id);
      अवरोध;
    शेष:
      CURL_TRC_CF(तथ्य, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  stream->id, rc, nghttp3_strerror(rc));
      अवरोध;
    }
    परिणाम = CURLE_SEND_ERROR;
    जाओ out;
  }

  अगर(Curl_trc_is_verbose(तथ्य)) {
    CURL_TRC_CF(तथ्य, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", stream->id,
                Curl_bufref_ptr(&तथ्य->state.url));
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  वापस परिणाम;
}

अटल CURLcode cf_h3_proxy_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                           काष्ठा Curl_easy *तथ्य,
                                           काष्ठा easy_pollset *ps)
{
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode परिणाम = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, तथ्य);

  अगर(!ctx->qconn || !pctx->tunnel.stream || (sock == CURL_SOCKET_BAD))
    वापस CURLE_OK;

  Curl_pollset_check(तथ्य, ps, sock, &want_recv, &want_send);

  अगर(want_recv || want_send || !Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    काष्ठा h3_stream_ctx *stream = pctx->tunnel.stream;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_बायाँ(ctx->qconn) ||
                 !ngtcp2_conn_get_max_तथ्य_बायाँ(ctx->qconn));
    s_exhaust = want_send && stream && stream->id >= 0 &&
                stream->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    परिणाम = Curl_pollset_set(तथ्य, ps, sock, want_recv, want_send);
  }
  वापस परिणाम;
}

अटल bool cf_h3_proxy_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                                     स्थिर काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  अगर(!Curl_bufq_is_empty(&pctx->tunnel.recvbuf))
    वापस TRUE;
  वापस cf->अग्र ?
    cf->अग्र->cft->has_तथ्य_pending(cf->अग्र, तथ्य) : FALSE;
}

अटल CURLcode cf_h3_proxy_submit_CONNECT(काष्ठा Curl_cfilter *cf,
                                           काष्ठा Curl_easy *तथ्य,
                                           काष्ठा h3_tunnel_stream *ts)
{
  काष्ठा cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम;
  काष्ठा httpreq *req = शून्य;

  परिणाम = Curl_http_proxy_create_tunnel_request(
    &req, cf, तथ्य, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  अगर(!परिणाम)
    परिणाम = Curl_creader_set_null(तथ्य);
  अगर(!परिणाम)
    परिणाम = cf_h3_proxy_submit(cf, तथ्य, ts, req);

  अगर(req)
    Curl_http_req_free(req);
  अगर(परिणाम)
    failf(तथ्य, "Failed sending CONNECT to proxy");
  वापस परिणाम;
}

अटल CURLcode cf_h3_proxy_inspect_response(काष्ठा Curl_cfilter *cf,
                                             काष्ठा Curl_easy *तथ्य,
                                             काष्ठा h3_tunnel_stream *ts)
{
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_परिणाम res;
  CURLcode परिणाम;

  परिणाम = Curl_http_proxy_inspect_tunnel_response(
      cf, तथ्य, ts->resp, (bool)pctx->tunnel.udp, &res);
  अगर(परिणाम)
    वापस परिणाम;
  चयन(res) {
  हाल PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, तथ्य);
    अवरोध;
  हाल PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, तथ्य);
    परिणाम = CURLE_COULDNT_CONNECT;
    अवरोध;
  हाल PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, तथ्य);
    अवरोध;
  }
  वापस परिणाम;
}

अटल CURLcode cf_h3_proxy_tunnel(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा h3_tunnel_stream *ts,
                                   bool *pdone)
{
  काष्ठा cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  करो {
    चयन(ts->state) {
    हाल H3_TUNNEL_INIT:
      CURL_TRC_CF(तथ्य, cf, "[0] CONNECT start for %s", ts->authority);
      परिणाम = cf_h3_proxy_submit_CONNECT(cf, तथ्य, ts);
      अगर(परिणाम)
        जाओ out;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, तथ्य);

      परिणाम = Curl_cf_ngtcp2_progress_egress(cf, तथ्य, शून्य);
      अगर(परिणाम)
        जाओ out;
      FALLTHROUGH();

    हाल H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      परिणाम = Curl_cf_ngtcp2_progress_ingress(cf, तथ्य, शून्य);
      अगर(परिणाम)
        जाओ out;
      परिणाम = Curl_cf_ngtcp2_progress_egress(cf, तथ्य, शून्य);
      अगर(परिणाम && परिणाम != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, तथ्य);
        जाओ out;
      }

      अगर(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, तथ्य);
      }
      अन्यथा {
        /* Not done yet, return and let multi interface call us again */
        परिणाम = CURLE_OK;
        जाओ out;
      }
      FALLTHROUGH();

    हाल H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      परिणाम = cf_h3_proxy_inspect_response(cf, तथ्य, ts);
      अगर(परिणाम)
        जाओ out;
      ctx->connected = TRUE;
      अवरोध;

    हाल H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      वापस CURLE_OK;

    हाल H3_TUNNEL_FAILED:
      वापस CURLE_RECV_ERROR;

    शेष:
      अवरोध;
    }

  } जबतक(ts->state == H3_TUNNEL_INIT);

out:
  अगर((परिणाम && (परिणाम != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, तथ्य);
  वापस परिणाम;
}

अटल CURLcode cf_h3_proxy_connect(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा cf_h3_proxy_ctx *pctx = cf->ctx;
  काष्ठा cf_call_तथ्य save = { 0 };
  CURLcode परिणाम = CURLE_OK;
  काष्ठा h3_tunnel_stream *ts = &pctx->tunnel;
  bool तथ्य_saved = FALSE;

  परिणाम = Curl_cf_ngtcp2_cmn_connect(cf, तथ्य, done);
  अगर(परिणाम || !*done)
    जाओ out;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  तथ्य_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  परिणाम = cf_h3_proxy_tunnel(cf, तथ्य, ts, done);

out:
  अगर(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&तथ्य->req, तथ्य);
    Curl_client_reset(तथ्य);
  }

  अगर(तथ्य_saved)
    CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल व्योम cf_h3_proxy_destroy(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h3_proxy_ctx *ctx = cf->ctx;

  (व्योम)तथ्य;
  अगर(ctx) {
    CURL_TRC_CF(तथ्य, cf, "cf_h3_proxy_destroy()");
    अगर(ctx->ngtcp2_ctx.qconn) {
      काष्ठा cf_call_तथ्य save;
      CF_तथ्य_SAVE(save, cf, तथ्य);
      Curl_cf_ngtcp2_cmn_conn_close(cf, तथ्य);
      CF_तथ्य_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = शून्य;
  }
}

अटल CURLcode cf_h3_proxy_shutdown(काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य, bool *done)
{
  वापस Curl_cf_ngtcp2_cmn_shutdown(cf, तथ्य, done);
}

काष्ठा Curl_cftype Curl_cft_h3_proxy = {
  "H3-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  cf_h3_proxy_destroy,
  cf_h3_proxy_connect,
  cf_h3_proxy_shutdown,
  cf_h3_proxy_adjust_pollset,
  cf_h3_proxy_तथ्य_pending,
  cf_h3_proxy_send,
  cf_h3_proxy_recv,
  Curl_cf_def_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_ngtcp2_cmn_query,
};

CURLcode Curl_cf_ngtcp2_proxy_create(काष्ठा Curl_cfilter **pcf,
                                     काष्ठा Curl_easy *तथ्य,
                                     काष्ठा Curl_peer *origin,
                                     काष्ठा Curl_peer *peer,
                                     uint8_t transport_peer,
                                     काष्ठा connectdata *conn,
                                     काष्ठा Curl_sockaddr_ex *addr,
                                     काष्ठा Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  काष्ठा Curl_cfilter *cf = शून्य;
  काष्ठा cf_h3_proxy_ctx *ctx;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  अगर(!tunnel_peer)
    वापस CURLE_FAILED_INIT;
  अगर((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    वापस CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  परिणाम = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  अगर(परिणाम)
    जाओ out;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  अगर(परिणाम)
    जाओ out;
  cf->conn = conn;

  परिणाम = Curl_cf_udp_create(&cf->अग्र, तथ्य, origin, peer, TRNSPRT_QUIC,
                              conn, addr, शून्य, TRNSPRT_QUIC);
  अगर(परिणाम)
    जाओ out;
  cf->अग्र->conn = cf->conn;
  cf->अग्र->sockindex = cf->sockindex;

out:
  *pcf = (!परिणाम) ? cf : शून्य;
  अगर(परिणाम) {
    अगर(cf)
      Curl_conn_cf_discard_chain(&cf, तथ्य);
    अन्यथा अगर(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  अन्यथा
    CURL_TRC_CF(तथ्य, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  वापस परिणाम;
}

CURLcode Curl_cf_ngtcp2_proxy_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                           काष्ठा Curl_easy *तथ्य,
                                           काष्ठा Curl_peer *origin,
                                           काष्ठा Curl_peer *peer,
                                           काष्ठा Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  काष्ठा Curl_cfilter *cf = शून्य;
  काष्ठा cf_h3_proxy_ctx *ctx;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  (व्योम)तथ्य;

  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx)
    जाओ out;
  परिणाम = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  अगर(परिणाम)
    जाओ out;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  अगर(परिणाम)
    जाओ out;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_जोड़ो_after(cf_at, cf);

out:
  अगर(परिणाम) {
    अगर(cf)
      Curl_conn_cf_discard_chain(&cf, तथ्य);
    अन्यथा अगर(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  वापस परिणाम;
}

#पूर्ण_अगर
