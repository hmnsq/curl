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
  defined(USE_NGHTTP2)

#அந்தர்பாவ <nghttp2/nghttp2.h>

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "bufq.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "dynhds.h"
#அந்தர்பாவ "http2.h"
#அந்தர்பாவ "http_proxy.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "ngh2_shim.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "cf-h2-proxy.h"

#define PROXY_H2_CHUNK_SIZE  (16 * 1024)

#define PROXY_HTTP2_HUGE_WINDOW_SIZE (100 * 1024 * 1024)
#define H2_TUNNEL_WINDOW_SIZE        (10 * 1024 * 1024)

#define PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define PROXY_H2_NW_SEND_CHUNKS   1

#define H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_SIZE)


வகைவரை எண்ணிக்கை {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

கட்டமைப்பு tunnel_stream {
  கட்டமைப்பு http_resp *resp;
  கட்டமைப்பு bufq recvbuf;
  கட்டமைப்பு bufq sendbuf;
  எழுத்து *authority;
  int32_t stream_id;
  uint32_t error;
  h2_tunnel_state state;
  BIT(has_final_response);
  BIT(closed);
  BIT(reset);
};

நிலையான CURLcode tunnel_stream_init(கட்டமைப்பு tunnel_stream *ts,
                                   கட்டமைப்பு Curl_peer *dest)
{
  ts->state = H2_TUNNEL_INIT;
  ts->stream_id = -1;
  Curl_bufq_init2(&ts->recvbuf, PROXY_H2_CHUNK_SIZE, H2_TUNNEL_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init(&ts->sendbuf, PROXY_H2_CHUNK_SIZE, H2_TUNNEL_SEND_CHUNKS);

  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->port);
  எனில்(!ts->authority)
    திரும்பு CURLE_OUT_OF_MEMORY;

  திரும்பு CURLE_OK;
}

நிலையான வெறுமை tunnel_stream_reset(கட்டமைப்பு tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = வெற்று;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->stream_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

நிலையான வெறுமை tunnel_stream_clear(கட்டமைப்பு tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, அளவு(*ts));
  ts->state = H2_TUNNEL_INIT;
}

நிலையான வெறுமை h2_tunnel_go_state(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு tunnel_stream *ts,
                               h2_tunnel_state new_state,
                               கட்டமைப்பு Curl_easy *data,
                               bool udp_tunnel)
{
  (வெறுமை)cf;
  (வெறுமை)udp_tunnel;

  எனில்(ts->state == new_state)
    திரும்பு;
  /* leaving this one */
  தேர்வு(ts->state) {
  நிலை H2_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  /* entering this one */
  தேர்வு(new_state) {
  நிலை H2_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'init'", ts->stream_id);
    tunnel_stream_reset(ts);
    நிறுத்து;

  நிலை H2_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'connect'", ts->stream_id);
    ts->state = H2_TUNNEL_CONNECT;
    நிறுத்து;

  நிலை H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'response'", ts->stream_id);
    ts->state = H2_TUNNEL_RESPONSE;
    நிறுத்து;

  நிலை H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'established'",
                ts->stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  நிலை H2_TUNNEL_FAILED:
    எனில்(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(data, cf, "[%d] new tunnel state 'failed'", ts->stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    நிறுத்து;
  }
}

கட்டமைப்பு cf_h2_proxy_ctx {
  கட்டமைப்பு curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  கட்டமைப்பு cf_call_data call_data;

  கட்டமைப்பு bufq inbufq;  /* network receive buffer */
  கட்டமைப்பு bufq outbufq; /* network send buffer */

  கட்டமைப்பு Curl_peer *peer; /* proxy we talk to */
  கட்டமைப்பு Curl_peer *dest; /* where to tunnel to */
  கட்டமைப்பு tunnel_stream tunnel; /* our tunnel CONNECT stream */
  int32_t goaway_error;
  int32_t remote_max_sid;
  BIT(conn_closed);
  BIT(rcvd_goaway);
  BIT(sent_goaway);
  BIT(nw_out_blocked);
  BIT(udp_tunnel);
};

/* How to access `call_data` from a cf_h2 filter */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((கட்டமைப்பு cf_h2_proxy_ctx *)(cf)->ctx)->call_data

நிலையான வெறுமை cf_h2_proxy_ctx_clear(கட்டமைப்பு cf_h2_proxy_ctx *ctx)
{
  கட்டமைப்பு cf_call_data save = ctx->call_data;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_stream_clear(&ctx->tunnel);
  memset(ctx, 0, அளவு(*ctx));
  ctx->call_data = save;
}

நிலையான வெறுமை cf_h2_proxy_ctx_free(கட்டமைப்பு cf_h2_proxy_ctx *ctx)
{
  எனில்(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

நிலையான வெறுமை drain_tunnel(கட்டமைப்பு Curl_cfilter *cf,
                         கட்டமைப்பு Curl_easy *data,
                         கட்டமைப்பு tunnel_stream *tunnel)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  (வெறுமை)cf;
  எனில்(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_empty(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(data);
}

நிலையான CURLcode proxy_h2_nw_out_writer(வெறுமை *writer_ctx,
                                       மாறிலி uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  கட்டமைப்பு Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  எனில்(cf) {
    கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
    CURLcode result;
    result = Curl_conn_cf_send(cf->next, data, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(data, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (முழு)result, *pnwritten);
    திரும்பு result;
  }
  திரும்பு CURLE_FAILED_INIT;
}

நிலையான முழு proxy_h2_client_new(கட்டமைப்பு Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { வெற்று, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  முழு rc = nghttp2_option_new(&o);
  எனில்(rc)
    திரும்பு rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#எனில் NGHTTP2_VERSION_இலக்கம் >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  திரும்பு rc;
}

நிலையான முழு proxy_h2_should_close_session(கட்டமைப்பு cf_h2_proxy_ctx *ctx)
{
  திரும்பு !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

நிலையான CURLcode proxy_h2_nw_out_flush(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  எனில்(Curl_bufq_is_empty(&ctx->outbufq))
    திரும்பு CURLE_OK;

  result = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_out_writer, cf,
                          &nwritten);
  எனில்(result) {
    எனில்(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    திரும்பு result;
  }
  CURL_TRC_CF(data, cf, "[0] nw send buffer flushed");
  திரும்பு Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
நிலையான CURLcode proxy_h2_process_pending_உள்ளீடு(கட்டமைப்பு Curl_cfilter *cf,
                                               கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  மாறிலி unsigned எழுத்து *buf;
  size_t blen, nread;
  ssize_t rv;

  போது(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (மாறிலி uint8_t *)buf, blen);
    CURL_TRC_CF(data, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    எனில்(!curlx_sztouz(rv, &nread)) {
      failf(data,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((முழு)rv));
      திரும்பு CURLE_RECV_ERROR;
    }
    இல்லையெனில் எனில்(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      நிறுத்து;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    எனில்(Curl_bufq_is_empty(&ctx->inbufq)) {
      CURL_TRC_CF(data, cf, "[0] all data in connection buffer processed");
      நிறுத்து;
    }
    இல்லையெனில் {
      CURL_TRC_CF(data, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode proxy_h2_progress_ingress(கட்டமைப்பு Curl_cfilter *cf,
                                          கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  எனில்(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = proxy_h2_process_pending_உள்ளீடு(cf, data);
    எனில்(result)
      திரும்பு result;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  போது(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_empty(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_full(&ctx->tunnel.recvbuf)) {

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (முழு)result, nread);
    எனில்(result) {
      எனில்(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 proxy data");
        திரும்பு result;
      }
      நிறுத்து;
    }
    இல்லையெனில் எனில்(nread == 0) {
      CURL_TRC_CF(data, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      நிறுத்து;
    }

    result = proxy_h2_process_pending_உள்ளீடு(cf, data);
    எனில்(result)
      திரும்பு result;
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode proxy_h2_progress_egress(கட்டமைப்பு Curl_cfilter *cf,
                                         கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  முழு rv = 0;

  ctx->nw_out_blocked = 0;
  போது(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  எனில்(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    திரும்பு CURLE_SEND_ERROR;
  }
  திரும்பு proxy_h2_nw_out_flush(cf, data);
}

நிலையான ssize_t on_session_send(nghttp2_session *h2,
                               மாறிலி uint8_t *buf, size_t blen, முழு flags,
                               வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (வெறுமை)h2;
  (வெறுமை)flags;
  DEBUGASSERT(data);

  result = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_out_writer, cf, &nwritten);
  எனில்(result) {
    எனில்(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      திரும்பு NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  எனில்(!nwritten)
    திரும்பு NGHTTP2_ERR_WOULDBLOCK;

  திரும்பு (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#ifdef CURLVERBOSE
நிலையான முழு proxy_h2_on_frame_send(nghttp2_session *session,
                                  மாறிலி nghttp2_frame *frame,
                                  வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);

  (வெறுமை)session;
  DEBUGASSERT(data);
  எனில்(Curl_trc_cf_is_verbose(cf, data)) {
    எழுத்து buffer[256];
    முழு len;
    len = Curl_nghttp2_fr_print(frame, buffer, அளவு(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  திரும்பு 0;
}
#endif /* CURLVERBOSE */

நிலையான முழு proxy_h2_on_frame_recv(nghttp2_session *session,
                                  மாறிலி nghttp2_frame *frame,
                                  வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;

  (வெறுமை)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  எனில்(Curl_trc_cf_is_verbose(cf, data)) {
    எழுத்து buffer[256];
    முழு len;
    len = Curl_nghttp2_fr_print(frame, buffer, அளவு(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  எனில்(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    தேர்வு(frame->hd.type) {
    நிலை NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      எனில்(CURL_REQ_WANT_SEND(data)) {
        drain_tunnel(cf, data, &ctx->tunnel);
      }
      நிறுத்து;
    நிலை NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      எனில்(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              frame->goaway.error_code, ctx->remote_max_sid);
      }
      நிறுத்து;
    இயல்பு:
      நிறுத்து;
    }
    திரும்பு 0;
  }

  எனில்(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] rcvd FRAME not for tunnel", stream_id);
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  தேர்வு(frame->hd.type) {
  நிலை NGHTTP2_HEADERS:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    எனில்(!ctx->tunnel.resp)
      திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(data, cf, "[%d] got http status: %d",
                stream_id, ctx->tunnel.resp->status);
    எனில்(!ctx->tunnel.has_final_response && ctx->tunnel.resp->status / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    நிறுத்து;
  நிலை NGHTTP2_WINDOW_UPDATE:
    எனில்(CURL_REQ_WANT_SEND(data)) {
      drain_tunnel(cf, data, &ctx->tunnel);
    }
    நிறுத்து;
  நிலை NGHTTP2_RST_STREAM:
    எனில்(frame->rst_stream.error_code)
      ctx->tunnel.reset = TRUE;
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு 0;
}

நிலையான முழு proxy_h2_on_header(nghttp2_session *session,
                              மாறிலி nghttp2_frame *frame,
                              மாறிலி uint8_t *name, size_t namelen,
                              மாறிலி uint8_t *மதிப்பு, size_t valuelen,
                              uint8_t flags,
                              வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (வெறுமை)flags;
  (வெறுமை)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  எனில்(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", stream_id,
                (முழு)namelen, name, (முழு)valuelen, மதிப்பு);
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  எனில்(frame->hd.type == NGHTTP2_தள்ளு_PROMISE)
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;

  எனில்(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    திரும்பு 0;
  }

  எனில்(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    int32_t http_status;
    கட்டமைப்பு http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    result = Curl_http_decode_status(&http_status,
                                    (மாறிலி எழுத்து *)மதிப்பு, valuelen);
    எனில்(result)
      திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
    result = Curl_http_resp_make(&resp, http_status, வெற்று);
    எனில்(result)
      திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
    எனில்(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream_id, ctx->tunnel.resp->status);
    திரும்பு 0;
  }

  எனில்(!ctx->tunnel.resp)
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_dynhds_add(&ctx->tunnel.resp->headers,
                           (மாறிலி எழுத்து *)name, namelen,
                           (மாறிலி எழுத்து *)மதிப்பு, valuelen);
  எனில்(result)
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream_id, (முழு)namelen, name, (முழு)valuelen, மதிப்பு);

  திரும்பு 0; /* 0 is successful */
}

நிலையான ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint8_t *buf, size_t length,
                                    uint32_t *data_flags,
                                    nghttp2_data_source *source,
                                    வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  கட்டமைப்பு tunnel_stream *ts;
  CURLcode result;
  size_t nread;

  (வெறுமை)session;
  (வெறுமை)source;
  (வெறுமை)ctx;

  எனில்(!stream_id)
    திரும்பு NGHTTP2_ERR_INVALID_ARGUMENT;

  ts = Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  எனில்(!ts)
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  result = Curl_bufq_read(&ts->sendbuf, buf, length, &nread);
  எனில்(result) {
    எனில்(result != CURLE_AGAIN)
      திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
    திரும்பு NGHTTP2_ERR_DEFERRED;
  }
  எனில்(ts->closed && Curl_bufq_is_empty(&ts->sendbuf))
    *data_flags = NGHTTP2_DATA_FLAG_EOF;

  CURL_TRC_CF(data, cf, "[%d] tunnel_send_callback -> %zu",
              ts->stream_id, nread);
  திரும்பு (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

நிலையான முழு tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t stream_id,
                                மாறிலி uint8_t *mem, size_t len, வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  (வெறுமை)flags;
  (வெறுமை)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  எனில்(stream_id != ctx->tunnel.stream_id)
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  எனில்(result) {
    எனில்(result != CURLE_AGAIN)
      திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
#ifdef DEBUGBUILD
    nwritten = 0;
#endif
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  திரும்பு 0;
}

நிலையான முழு proxy_h2_on_stream_close(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint32_t error_code, வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);

  (வெறுமை)session;

  எனில்(stream_id != ctx->tunnel.stream_id)
    திரும்பு 0;

  CURL_TRC_CF(data, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_code;
  எனில்(error_code)
    ctx->tunnel.reset = TRUE;

  திரும்பு 0;
}

நிலையான CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  கட்டமைப்பு Curl_cfilter *cf,
  கட்டமைப்பு Curl_easy *data,
  கட்டமைப்பு curl_ngh2 *ngh2,
  கட்டமைப்பு httpreq *req,
  மாறிலி nghttp2_priority_spec *pri_spec,
  வெறுமை *stream_user_data,
  nghttp2_data_source_read_callback read_callback,
  வெறுமை *read_ctx)
{
  கட்டமைப்பு dynhds h2_headers;
  nghttp2_nv *nva = வெற்று;
  int32_t stream_id = -1;
  size_t nheader;
  CURLcode result;

  (வெறுமை)cf;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_மொத்தம், DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  எனில்(result)
    செல் out;

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  எனில்(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  எனில்(read_callback) {
    nghttp2_data_provider data_prd;

    data_prd.read_callback = read_callback;
    data_prd.source.ptr = read_ctx;
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &data_prd, stream_user_data);
  }
  இல்லையெனில் {
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, வெற்று, stream_user_data);
  }

  எனில்(stream_id < 0) {
    failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    செல் out;
  }
  result = CURLE_OK;

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  *pstream_id = stream_id;
  திரும்பு result;
}

நிலையான CURLcode submit_CONNECT(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு tunnel_stream *ts)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  கட்டமைப்பு httpreq *req = வெற்று;

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  எனில்(result)
    செல் out;
  result = Curl_creader_set_null(data);
  எனில்(result)
    செல் out;

  result = proxy_h2_submit(&ts->stream_id, cf, data, &ctx->ngh2, req,
                           வெற்று, ts, tunnel_send_callback, cf);
  எனில்(result) {
    CURL_TRC_CF(data, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->stream_id, nghttp2_strerror(ts->stream_id));
  }

out:
  எனில்(req)
    Curl_http_req_free(req);
  எனில்(result)
    failf(data, "Failed sending CONNECT to proxy");
  திரும்பு result;
}

நிலையான CURLcode inspect_response(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு tunnel_stream *ts)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)ctx->udp_tunnel, &res);
  எனில்(result)
    திரும்பு result;
  தேர்வு(res) {
  நிலை PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, data,
                       (bool)ctx->udp_tunnel);
    நிறுத்து;
  நிலை PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
    result = CURLE_COULDNT_CONNECT;
    நிறுத்து;
  நிலை PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, data,
                       (bool)ctx->udp_tunnel);
    நிறுத்து;
  }
  திரும்பு result;
}

நிலையான CURLcode H2_CONNECT(கட்டமைப்பு Curl_cfilter *cf,
                           கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு tunnel_stream *ts)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  எனில்(ctx->conn_closed) {
    failf(data, "proxy closed connection");
    திரும்பு CURLE_COULDNT_CONNECT;
  }

  செய் {
    தேர்வு(ts->state) {
    நிலை H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = submit_CONNECT(cf, data, ts);
      எனில்(result)
        செல் out;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, data,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    நிலை H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      result = proxy_h2_progress_ingress(cf, data);
      எனில்(!result)
        result = proxy_h2_progress_egress(cf, data);
      எனில்(result && result != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                           (bool)ctx->udp_tunnel);
        நிறுத்து;
      }

      எனில்(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, data,
                           (bool)ctx->udp_tunnel);
      }
      இல்லையெனில் {
        result = CURLE_OK;
        செல் out;
      }
      FALLTHROUGH();

    நிலை H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = inspect_response(cf, data, ts);
      எனில்(result)
        செல் out;
      நிறுத்து;

    நிலை H2_TUNNEL_ESTABLISHED:
      திரும்பு CURLE_OK;

    நிலை H2_TUNNEL_FAILED:
      திரும்பு CURLE_RECV_ERROR;

    இயல்பு:
      நிறுத்து;
    }

  } போது(ts->state == H2_TUNNEL_INIT);

out:
  எனில்((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
  திரும்பு result;
}

/*
 * Initialize the cfilter context
 */
நிலையான CURLcode cf_h2_proxy_ctx_init(கட்டமைப்பு Curl_cfilter *cf,
                                     கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  nghttp2_session_callbacks *cbs = வெற்று;
  முழு rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, அளவு(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  எனில்(tunnel_stream_init(&ctx->tunnel, ctx->dest))
    செல் out;

  rc = nghttp2_session_callbacks_new(&cbs);
  எனில்(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    செல் out;
  }

  nghttp2_session_callbacks_set_send_callback(cbs, on_session_send);
  nghttp2_session_callbacks_set_on_frame_recv_callback(
    cbs, proxy_h2_on_frame_recv);
#ifdef CURLVERBOSE
  nghttp2_session_callbacks_set_on_frame_send_callback(cbs,
                                                       proxy_h2_on_frame_send);
#endif
  nghttp2_session_callbacks_set_on_data_chunk_recv_callback(
    cbs, tunnel_recv_callback);
  nghttp2_session_callbacks_set_on_stream_close_callback(
    cbs, proxy_h2_on_stream_close);
  nghttp2_session_callbacks_set_on_header_callback(cbs, proxy_h2_on_header);

  /* The nghttp2 session is not yet setup, do it */
  rc = proxy_h2_client_new(cf, cbs);
  எனில்(rc) {
    failf(data, "Could not initialize nghttp2");
    செல் out;
  }

  {
    nghttp2_settings_entry iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].மதிப்பு = Curl_multi_max_concurrent_streams(data->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
    iv[1].மதிப்பு = H2_TUNNEL_WINDOW_SIZE;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_தள்ளு;
    iv[2].மதிப்பு = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    எனில்(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      செல் out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_WINDOW_SIZE);
  எனில்(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    செல் out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;

out:
  எனில்(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(data, cf, "[0] init proxy ctx -> %d", (முழு)result);
  திரும்பு result;
}

நிலையான CURLcode cf_h2_proxy_connect(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    bool *done)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு cf_call_data save;
  கட்டமைப்பு tunnel_stream *ts = &ctx->tunnel;

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  /* Connect the lower filters first */
  எனில்(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    எனில்(result || !*done)
      திரும்பு result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  எனில்(!ctx->ngh2.s) {
    result = cf_h2_proxy_ctx_init(cf, data);
    எனில்(result)
      செல் out;
  }
  DEBUGASSERT(ts->authority);

  எனில்(Curl_timeleft_ms(data) < 0) {
    failf(data, "Proxy CONNECT aborted due to timeout");
    result = CURLE_OPERATION_TIMEDOUT;
    செல் out;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  result = H2_CONNECT(cf, data, ts);

out:
  *done = (result == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  எனில்(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான வெறுமை cf_h2_proxy_destroy(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;

  (வெறுமை)data;
  எனில்(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = வெற்று;
  }
}

நிலையான CURLcode cf_h2_proxy_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                                     கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  CURLcode result;
  முழு rv;

  எனில்(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  எனில்(!ctx->sent_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (மாறிலி uint8_t *)"shutdown", அளவு("shutdown"));
    எனில்(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      செல் out;
    }
    ctx->sent_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  எனில்(Curl_ngh2_session_want_write(&ctx->ngh2))
    result = proxy_h2_progress_egress(cf, data);
  எனில்(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = proxy_h2_progress_ingress(cf, data);

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  திரும்பு result;
}

நிலையான bool cf_h2_proxy_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                                     மாறிலி கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  எனில்((ctx && !Curl_bufq_is_empty(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    திரும்பு TRUE;
  திரும்பு cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

நிலையான CURLcode cf_h2_proxy_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                           கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  எனில்(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  இல்லையெனில்
    Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  எனில்(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
    bool c_exhaust, s_exhaust;

    CF_DATA_SAVE(save, cf, data);
    c_exhaust = !Curl_ngh2_session_get_remote_window_size(&ctx->ngh2);
    s_exhaust = ctx->tunnel.stream_id >= 0 &&
                !Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, ctx->tunnel.stream_id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CURL_TRC_CF(data, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (முழு)result);
    CF_DATA_RESTORE(cf, save);
  }
  இல்லையெனில் எனில்(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CURL_TRC_CF(data, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (முழு)result);
    CF_DATA_RESTORE(cf, save);
  }
  திரும்பு result;
}

நிலையான CURLcode h2_handle_tunnel_close(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data,
                                       size_t *pnread)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  எனில்(ctx->tunnel.error) {
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.stream_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    திரும்பு CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(data, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.stream_id);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode tunnel_recv(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                            எழுத்து *buf, size_t len, size_t *pnread)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  *pnread = 0;
  எனில்(!Curl_bufq_is_empty(&ctx->tunnel.recvbuf))
    result = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  இல்லையெனில் {
    எனில்(ctx->tunnel.closed) {
      result = h2_handle_tunnel_close(cf, data, pnread);
    }
    இல்லையெனில் எனில்(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.stream_id)) {
      result = CURLE_RECV_ERROR;
    }
    இல்லையெனில்
      result = CURLE_AGAIN;
  }

  CURL_TRC_CF(data, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (முழு)result, *pnread);
  திரும்பு result;
}

நிலையான CURLcode cf_h2_proxy_recv(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 எழுத்து *buf, size_t len,
                                 size_t *pnread)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  CURLcode result;

  *pnread = 0;
  CF_DATA_SAVE(save, cf, data);

  எனில்(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_RECV_ERROR;
    செல் out;
  }

  எனில்(Curl_bufq_is_empty(&ctx->tunnel.recvbuf)) {
    result = proxy_h2_progress_ingress(cf, data);
    எனில்(result)
      செல் out;
  }

  result = tunnel_recv(cf, data, buf, len, pnread);

  எனில்(!result) {
    CURL_TRC_CF(data, cf, "[%d] increase window by %zu",
                ctx->tunnel.stream_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.stream_id, *pnread);
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

out:
  எனில்((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (முழு)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான CURLcode cf_h2_proxy_send(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 மாறிலி uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  CURLcode result;

  (வெறுமை)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  எனில்(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_SEND_ERROR;
    செல் out;
  }

  எனில்(ctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    செல் out;
  }

  result = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(data, cf, "cf_send(), bufq_write %d, %zu", (முழு)result,
              *pnwritten);
  எனில்(result && (result != CURLE_AGAIN))
    செல் out;

  எனில்(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    எனில்(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      செல் out;
    }
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_ingress(cf, data));
  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

  எனில்(!result && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    எனில்(ctx->tunnel.closed) {
      result = CURLE_SEND_ERROR;
    }
    இல்லையெனில் {
      CURL_TRC_CF(data, cf, "[0] send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  எனில்((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, len, (முழு)result, *pnwritten,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான CURLcode cf_h2_proxy_flush(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  எனில்(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    எனில்(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      செல் out;
    }
  }

  result = proxy_h2_progress_egress(cf, data);

out:
  CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, (முழு)result,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான bool proxy_h2_connisalive(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 bool *உள்ளீடு_pending)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *உள்ளீடு_pending = FALSE;
  எனில்(!cf->next || !cf->next->cft->is_alive(cf->next, data, உள்ளீடு_pending))
    திரும்பு FALSE;

  எனில்(*உள்ளீடு_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *உள்ளீடு_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    எனில்(!result) {
      எனில்(proxy_h2_process_pending_உள்ளீடு(cf, data))
        /* immediate error, considered dead */
        alive = FALSE;
      இல்லையெனில் {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    இல்லையெனில் எனில்(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  திரும்பு alive;
}

நிலையான bool cf_h2_proxy_is_alive(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 bool *உள்ளீடு_pending)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  கட்டமைப்பு cf_call_data save;

  *உள்ளீடு_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, data, உள்ளீடு_pending));
  CURL_TRC_CF(data, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *உள்ளீடு_pending);
  CF_DATA_RESTORE(cf, save);
  திரும்பு alive;
}

நிலையான CURLcode cf_h2_proxy_query(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  முழு query, முழு *pres1, வெறுமை *pres2)
{
  கட்டமைப்பு cf_h2_proxy_ctx *ctx = cf->ctx;

  தேர்வு(query) {
  நிலை CF_QUERY_HOST_PORT:
    *pres1 = (முழு)ctx->dest->port;
    *((மாறிலி எழுத்து **)pres2) = ctx->dest->hostname;
    திரும்பு CURLE_OK;
  நிலை CF_QUERY_NEED_FLUSH: {
    எனில்(!Curl_bufq_is_empty(&ctx->outbufq) ||
       !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(data, cf, "needs flush");
      *pres1 = TRUE;
      திரும்பு CURLE_OK;
    }
    நிறுத்து;
  }
  நிலை CF_QUERY_ALPN_NEGOTIATED: {
    மாறிலி எழுத்து **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = வெற்று;
    திரும்பு CURLE_OK;
  }
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

நிலையான CURLcode cf_h2_proxy_cntrl(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  முழு event, முழு arg1, வெறுமை *arg2)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு cf_call_data save;

  (வெறுமை)arg1;
  (வெறுமை)arg2;

  தேர்வு(event) {
  நிலை CF_CTRL_FLUSH:
    CF_DATA_SAVE(save, cf, data);
    result = cf_h2_proxy_flush(cf, data);
    CF_DATA_RESTORE(cf, save);
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு result;
}

கட்டமைப்பு Curl_cftype Curl_cft_h2_proxy = {
  "H2-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  cf_h2_proxy_destroy,
  cf_h2_proxy_connect,
  cf_h2_proxy_shutdown,
  cf_h2_proxy_adjust_pollset,
  cf_h2_proxy_data_pending,
  cf_h2_proxy_send,
  cf_h2_proxy_recv,
  cf_h2_proxy_cntrl,
  cf_h2_proxy_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h2_proxy_query,
};

CURLcode Curl_cf_h2_proxy_செருகு_after(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு Curl_peer *peer,
                                       கட்டமைப்பு Curl_peer *dest,
                                       bool udp_tunnel)
{
  கட்டமைப்பு Curl_cfilter *cf_h2_proxy = வெற்று;
  கட்டமைப்பு cf_h2_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (வெறுமை)data;
  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx)
    செல் out;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  result = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  எனில்(result)
    செல் out;
  ctx = வெற்று;
  Curl_conn_cf_செருகு_after(cf, cf_h2_proxy);

out:
  cf_h2_proxy_ctx_free(ctx);
  திரும்பு result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
