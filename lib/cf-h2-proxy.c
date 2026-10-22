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
#ضمن "curl_setup.h"

#אם !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_NGHTTP2)

#ضمن <nghttp2/nghttp2.h>

#ضمن "urldata.h"
#ضمن "url.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "curl_trc.h"
#ضمن "bufq.h"
#ضمن "curlx/dynbuf.h"
#ضمن "dynhds.h"
#ضمن "http2.h"
#ضمن "http_proxy.h"
#ضمن "multiif.h"
#ضمن "ngh2_shim.h"
#ضمن "sendf.h"
#ضمن "select.h"
#ضمن "cf-h2-proxy.h"

#define PROXY_H2_CHUNK_SIZE  (16 * 1024)

#define PROXY_HTTP2_HUGE_WINDOW_SIZE (100 * 1024 * 1024)
#define H2_TUNNEL_WINDOW_SIZE        (10 * 1024 * 1024)

#define PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define PROXY_H2_NW_SEND_CHUNKS   1

#define H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_SIZE)


تعريفنوع تعداد {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

بنية tunnel_stream {
  بنية http_resp *resp;
  بنية bufq recvbuf;
  بنية bufq sendbuf;
  حرف *authority;
  int32_t stream_id;
  uint32_t error;
  h2_tunnel_state state;
  BIT(has_final_response);
  BIT(closed);
  BIT(reset);
};

ساكن CURLcode tunnel_stream_init(بنية tunnel_stream *ts,
                                   بنية Curl_peer *dest)
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
  אם(!ts->authority)
    החזר CURLE_OUT_OF_MEMORY;

  החזר CURLE_OK;
}

ساكن فراغ tunnel_stream_reset(بنية tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = عدم;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->stream_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

ساكن فراغ tunnel_stream_clear(بنية tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, حجم(*ts));
  ts->state = H2_TUNNEL_INIT;
}

ساكن فراغ h2_tunnel_go_state(بنية Curl_cfilter *cf,
                               بنية tunnel_stream *ts,
                               h2_tunnel_state new_state,
                               بنية Curl_easy *data,
                               bool udp_tunnel)
{
  (فراغ)cf;
  (فراغ)udp_tunnel;

  אם(ts->state == new_state)
    החזר;
  /* leaving this one */
  بدل(ts->state) {
  حالة H2_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }
  /* entering this one */
  بدل(new_state) {
  حالة H2_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'init'", ts->stream_id);
    tunnel_stream_reset(ts);
    اكسر;

  حالة H2_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'connect'", ts->stream_id);
    ts->state = H2_TUNNEL_CONNECT;
    اكسر;

  حالة H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'response'", ts->stream_id);
    ts->state = H2_TUNNEL_RESPONSE;
    اكسر;

  حالة H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'established'",
                ts->stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  حالة H2_TUNNEL_FAILED:
    אם(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(data, cf, "[%d] new tunnel state 'failed'", ts->stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    اكسر;
  }
}

بنية cf_h2_proxy_ctx {
  بنية curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  بنية cf_call_data call_data;

  بنية bufq inbufq;  /* network receive buffer */
  بنية bufq outbufq; /* network send buffer */

  بنية Curl_peer *peer; /* proxy we talk to */
  بنية Curl_peer *dest; /* where to tunnel to */
  بنية tunnel_stream tunnel; /* our tunnel CONNECT stream */
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
#define CF_CTX_CALL_DATA(cf) ((بنية cf_h2_proxy_ctx *)(cf)->ctx)->call_data

ساكن فراغ cf_h2_proxy_ctx_clear(بنية cf_h2_proxy_ctx *ctx)
{
  بنية cf_call_data save = ctx->call_data;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_stream_clear(&ctx->tunnel);
  memset(ctx, 0, حجم(*ctx));
  ctx->call_data = save;
}

ساكن فراغ cf_h2_proxy_ctx_free(بنية cf_h2_proxy_ctx *ctx)
{
  אם(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

ساكن فراغ drain_tunnel(بنية Curl_cfilter *cf,
                         بنية Curl_easy *data,
                         بنية tunnel_stream *tunnel)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  (فراغ)cf;
  אם(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_empty(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(data);
}

ساكن CURLcode proxy_h2_nw_out_writer(فراغ *writer_ctx,
                                       ثابت uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  بنية Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  אם(cf) {
    بنية Curl_easy *data = CF_DATA_CURRENT(cf);
    CURLcode result;
    result = Curl_conn_cf_send(cf->next, data, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(data, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (صحيح)result, *pnwritten);
    החזר result;
  }
  החזר CURLE_FAILED_INIT;
}

ساكن صحيح proxy_h2_client_new(بنية Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { عدم, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  صحيح rc = nghttp2_option_new(&o);
  אם(rc)
    החזר rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#אם NGHTTP2_VERSION_NUM >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  החזר rc;
}

ساكن صحيح proxy_h2_should_close_session(بنية cf_h2_proxy_ctx *ctx)
{
  החזר !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

ساكن CURLcode proxy_h2_nw_out_flush(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  אם(Curl_bufq_is_empty(&ctx->outbufq))
    החזר CURLE_OK;

  result = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_out_writer, cf,
                          &nwritten);
  אם(result) {
    אם(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    החזר result;
  }
  CURL_TRC_CF(data, cf, "[0] nw send buffer flushed");
  החזר Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ساكن CURLcode proxy_h2_process_pending_input(بنية Curl_cfilter *cf,
                                               بنية Curl_easy *data)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  ثابت unsigned حرف *buf;
  size_t blen, nread;
  ssize_t rv;

  כלעוד(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ثابت uint8_t *)buf, blen);
    CURL_TRC_CF(data, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    אם(!curlx_sztouz(rv, &nread)) {
      failf(data,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((صحيح)rv));
      החזר CURLE_RECV_ERROR;
    }
    אחרת אם(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      اكسر;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    אם(Curl_bufq_is_empty(&ctx->inbufq)) {
      CURL_TRC_CF(data, cf, "[0] all data in connection buffer processed");
      اكسر;
    }
    אחרת {
      CURL_TRC_CF(data, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  החזר CURLE_OK;
}

ساكن CURLcode proxy_h2_progress_ingress(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *data)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  אם(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = proxy_h2_process_pending_input(cf, data);
    אם(result)
      החזר result;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  כלעוד(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_empty(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_full(&ctx->tunnel.recvbuf)) {

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (صحيح)result, nread);
    אם(result) {
      אם(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 proxy data");
        החזר result;
      }
      اكسر;
    }
    אחרת אם(nread == 0) {
      CURL_TRC_CF(data, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      اكسر;
    }

    result = proxy_h2_process_pending_input(cf, data);
    אם(result)
      החזר result;
  }

  החזר CURLE_OK;
}

ساكن CURLcode proxy_h2_progress_egress(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *data)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  صحيح rv = 0;

  ctx->nw_out_blocked = 0;
  כלעוד(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  אם(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    החזר CURLE_SEND_ERROR;
  }
  החזר proxy_h2_nw_out_flush(cf, data);
}

ساكن ssize_t on_session_send(nghttp2_session *h2,
                               ثابت uint8_t *buf, size_t blen, صحيح flags,
                               فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (فراغ)h2;
  (فراغ)flags;
  DEBUGASSERT(data);

  result = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_out_writer, cf, &nwritten);
  אם(result) {
    אם(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      החזר NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    החזר NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  אם(!nwritten)
    החזר NGHTTP2_ERR_WOULDBLOCK;

  החזר (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#ifdef CURLVERBOSE
ساكن صحيح proxy_h2_on_frame_send(nghttp2_session *session,
                                  ثابت nghttp2_frame *frame,
                                  فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);

  (فراغ)session;
  DEBUGASSERT(data);
  אם(Curl_trc_cf_is_verbose(cf, data)) {
    حرف buffer[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(frame, buffer, حجم(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  החזר 0;
}
#endif /* CURLVERBOSE */

ساكن صحيح proxy_h2_on_frame_recv(nghttp2_session *session,
                                  ثابت nghttp2_frame *frame,
                                  فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;

  (فراغ)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  אם(Curl_trc_cf_is_verbose(cf, data)) {
    حرف buffer[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(frame, buffer, حجم(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  אם(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    بدل(frame->hd.type) {
    حالة NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      אם(CURL_REQ_WANT_SEND(data)) {
        drain_tunnel(cf, data, &ctx->tunnel);
      }
      اكسر;
    حالة NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      אם(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              frame->goaway.error_code, ctx->remote_max_sid);
      }
      اكسر;
    افتراضي:
      اكسر;
    }
    החזר 0;
  }

  אם(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] rcvd FRAME not for tunnel", stream_id);
    החזר NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  بدل(frame->hd.type) {
  حالة NGHTTP2_HEADERS:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    אם(!ctx->tunnel.resp)
      החזר NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(data, cf, "[%d] got http status: %d",
                stream_id, ctx->tunnel.resp->status);
    אם(!ctx->tunnel.has_final_response && ctx->tunnel.resp->status / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    اكسر;
  حالة NGHTTP2_WINDOW_UPDATE:
    אם(CURL_REQ_WANT_SEND(data)) {
      drain_tunnel(cf, data, &ctx->tunnel);
    }
    اكسر;
  حالة NGHTTP2_RST_STREAM:
    אם(frame->rst_stream.error_code)
      ctx->tunnel.reset = TRUE;
    اكسر;
  افتراضي:
    اكسر;
  }
  החזר 0;
}

ساكن صحيح proxy_h2_on_header(nghttp2_session *session,
                              ثابت nghttp2_frame *frame,
                              ثابت uint8_t *name, size_t namelen,
                              ثابت uint8_t *value, size_t valuelen,
                              uint8_t flags,
                              فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (فراغ)flags;
  (فراغ)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  אם(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", stream_id,
                (صحيح)namelen, name, (صحيح)valuelen, value);
    החזר NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  אם(frame->hd.type == NGHTTP2_PUSH_PROMISE)
    החזר NGHTTP2_ERR_CALLBACK_FAILURE;

  אם(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    החזר 0;
  }

  אם(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    int32_t http_status;
    بنية http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    result = Curl_http_decode_status(&http_status,
                                    (ثابت حرف *)value, valuelen);
    אם(result)
      החזר NGHTTP2_ERR_CALLBACK_FAILURE;
    result = Curl_http_resp_make(&resp, http_status, عدم);
    אם(result)
      החזר NGHTTP2_ERR_CALLBACK_FAILURE;
    אם(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream_id, ctx->tunnel.resp->status);
    החזר 0;
  }

  אם(!ctx->tunnel.resp)
    החזר NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_dynhds_add(&ctx->tunnel.resp->headers,
                           (ثابت حرف *)name, namelen,
                           (ثابت حرف *)value, valuelen);
  אם(result)
    החזר NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream_id, (صحيح)namelen, name, (صحيح)valuelen, value);

  החזר 0; /* 0 is successful */
}

ساكن ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint8_t *buf, size_t length,
                                    uint32_t *data_flags,
                                    nghttp2_data_source *source,
                                    فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);
  بنية tunnel_stream *ts;
  CURLcode result;
  size_t nread;

  (فراغ)session;
  (فراغ)source;
  (فراغ)ctx;

  אם(!stream_id)
    החזר NGHTTP2_ERR_INVALID_ARGUMENT;

  ts = Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  אם(!ts)
    החזר NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  result = Curl_bufq_read(&ts->sendbuf, buf, length, &nread);
  אם(result) {
    אם(result != CURLE_AGAIN)
      החזר NGHTTP2_ERR_CALLBACK_FAILURE;
    החזר NGHTTP2_ERR_DEFERRED;
  }
  אם(ts->closed && Curl_bufq_is_empty(&ts->sendbuf))
    *data_flags = NGHTTP2_DATA_FLAG_EOF;

  CURL_TRC_CF(data, cf, "[%d] tunnel_send_callback -> %zu",
              ts->stream_id, nread);
  החזר (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

ساكن صحيح tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t stream_id,
                                ثابت uint8_t *mem, size_t len, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  (فراغ)flags;
  (فراغ)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  אם(stream_id != ctx->tunnel.stream_id)
    החזר NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  אם(result) {
    אם(result != CURLE_AGAIN)
      החזר NGHTTP2_ERR_CALLBACK_FAILURE;
#ifdef DEBUGBUILD
    nwritten = 0;
#endif
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  החזר 0;
}

ساكن صحيح proxy_h2_on_stream_close(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint32_t error_code, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);

  (فراغ)session;

  אם(stream_id != ctx->tunnel.stream_id)
    החזר 0;

  CURL_TRC_CF(data, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_code;
  אם(error_code)
    ctx->tunnel.reset = TRUE;

  החזר 0;
}

ساكن CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  بنية Curl_cfilter *cf,
  بنية Curl_easy *data,
  بنية curl_ngh2 *ngh2,
  بنية httpreq *req,
  ثابت nghttp2_priority_spec *pri_spec,
  فراغ *stream_user_data,
  nghttp2_data_source_read_callback read_callback,
  فراغ *read_ctx)
{
  بنية dynhds h2_headers;
  nghttp2_nv *nva = عدم;
  int32_t stream_id = -1;
  size_t nheader;
  CURLcode result;

  (فراغ)cf;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_COUNT, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  אם(result)
    اذهب out;

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  אם(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  אם(read_callback) {
    nghttp2_data_provider data_prd;

    data_prd.read_callback = read_callback;
    data_prd.source.ptr = read_ctx;
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &data_prd, stream_user_data);
  }
  אחרת {
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, عدم, stream_user_data);
  }

  אם(stream_id < 0) {
    failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    اذهب out;
  }
  result = CURLE_OK;

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  *pstream_id = stream_id;
  החזר result;
}

ساكن CURLcode submit_CONNECT(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               بنية tunnel_stream *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  بنية httpreq *req = عدم;

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  אם(result)
    اذهب out;
  result = Curl_creader_set_null(data);
  אם(result)
    اذهب out;

  result = proxy_h2_submit(&ts->stream_id, cf, data, &ctx->ngh2, req,
                           عدم, ts, tunnel_send_callback, cf);
  אם(result) {
    CURL_TRC_CF(data, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->stream_id, nghttp2_strerror(ts->stream_id));
  }

out:
  אם(req)
    Curl_http_req_free(req);
  אם(result)
    failf(data, "Failed sending CONNECT to proxy");
  החזר result;
}

ساكن CURLcode inspect_response(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 بنية tunnel_stream *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)ctx->udp_tunnel, &res);
  אם(result)
    החזר result;
  بدل(res) {
  حالة PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, data,
                       (bool)ctx->udp_tunnel);
    اكسر;
  حالة PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
    result = CURLE_COULDNT_CONNECT;
    اكسر;
  حالة PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, data,
                       (bool)ctx->udp_tunnel);
    اكسر;
  }
  החזר result;
}

ساكن CURLcode H2_CONNECT(بنية Curl_cfilter *cf,
                           بنية Curl_easy *data,
                           بنية tunnel_stream *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  אם(ctx->conn_closed) {
    failf(data, "proxy closed connection");
    החזר CURLE_COULDNT_CONNECT;
  }

  עשה {
    بدل(ts->state) {
    حالة H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = submit_CONNECT(cf, data, ts);
      אם(result)
        اذهب out;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, data,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    حالة H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      result = proxy_h2_progress_ingress(cf, data);
      אם(!result)
        result = proxy_h2_progress_egress(cf, data);
      אם(result && result != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                           (bool)ctx->udp_tunnel);
        اكسر;
      }

      אם(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, data,
                           (bool)ctx->udp_tunnel);
      }
      אחרת {
        result = CURLE_OK;
        اذهب out;
      }
      FALLTHROUGH();

    حالة H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = inspect_response(cf, data, ts);
      אם(result)
        اذهب out;
      اكسر;

    حالة H2_TUNNEL_ESTABLISHED:
      החזר CURLE_OK;

    حالة H2_TUNNEL_FAILED:
      החזר CURLE_RECV_ERROR;

    افتراضي:
      اكسر;
    }

  } כלעוד(ts->state == H2_TUNNEL_INIT);

out:
  אם((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
  החזר result;
}

/*
 * Initialize the cfilter context
 */
ساكن CURLcode cf_h2_proxy_ctx_init(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *data)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  nghttp2_session_callbacks *cbs = عدم;
  صحيح rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, حجم(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  אם(tunnel_stream_init(&ctx->tunnel, ctx->dest))
    اذهب out;

  rc = nghttp2_session_callbacks_new(&cbs);
  אם(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    اذهب out;
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
  אם(rc) {
    failf(data, "Could not initialize nghttp2");
    اذهب out;
  }

  {
    nghttp2_settings_entry iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].value = Curl_multi_max_concurrent_streams(data->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
    iv[1].value = H2_TUNNEL_WINDOW_SIZE;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_PUSH;
    iv[2].value = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    אם(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      اذهب out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_WINDOW_SIZE);
  אם(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    اذهب out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;

out:
  אם(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(data, cf, "[0] init proxy ctx -> %d", (صحيح)result);
  החזר result;
}

ساكن CURLcode cf_h2_proxy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    bool *done)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  بنية cf_call_data save;
  بنية tunnel_stream *ts = &ctx->tunnel;

  אם(cf->connected) {
    *done = TRUE;
    החזר CURLE_OK;
  }

  /* Connect the lower filters first */
  אם(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    אם(result || !*done)
      החזר result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  אם(!ctx->ngh2.s) {
    result = cf_h2_proxy_ctx_init(cf, data);
    אם(result)
      اذهب out;
  }
  DEBUGASSERT(ts->authority);

  אם(Curl_timeleft_ms(data) < 0) {
    failf(data, "Proxy CONNECT aborted due to timeout");
    result = CURLE_OPERATION_TIMEDOUT;
    اذهب out;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  result = H2_CONNECT(cf, data, ts);

out:
  *done = (result == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  אם(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }
  CF_DATA_RESTORE(cf, save);
  החזר result;
}

ساكن فراغ cf_h2_proxy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  (فراغ)data;
  אם(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_h2_proxy_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *data, bool *done)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_data save;
  CURLcode result;
  صحيح rv;

  אם(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    החזר CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  אם(!ctx->sent_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ثابت uint8_t *)"shutdown", حجم("shutdown"));
    אם(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      اذهب out;
    }
    ctx->sent_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  אם(Curl_ngh2_session_want_write(&ctx->ngh2))
    result = proxy_h2_progress_egress(cf, data);
  אם(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = proxy_h2_progress_ingress(cf, data);

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  החזר result;
}

ساكن bool cf_h2_proxy_data_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *data)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  אם((ctx && !Curl_bufq_is_empty(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    החזר TRUE;
  החזר cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ساكن CURLcode cf_h2_proxy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *data,
                                           بنية easy_pollset *ps)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_data save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  אם(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  אחרת
    Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  אם(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
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
                want_recv, want_send, (صحيح)result);
    CF_DATA_RESTORE(cf, save);
  }
  אחרת אם(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CURL_TRC_CF(data, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (صحيح)result);
    CF_DATA_RESTORE(cf, save);
  }
  החזר result;
}

ساكن CURLcode h2_handle_tunnel_close(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *data,
                                       size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  אם(ctx->tunnel.error) {
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.stream_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    החזר CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(data, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.stream_id);
  החזר CURLE_OK;
}

ساكن CURLcode tunnel_recv(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                            حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  *pnread = 0;
  אם(!Curl_bufq_is_empty(&ctx->tunnel.recvbuf))
    result = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  אחרת {
    אם(ctx->tunnel.closed) {
      result = h2_handle_tunnel_close(cf, data, pnread);
    }
    אחרת אם(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.stream_id)) {
      result = CURLE_RECV_ERROR;
    }
    אחרת
      result = CURLE_AGAIN;
  }

  CURL_TRC_CF(data, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (صحيح)result, *pnread);
  החזר result;
}

ساكن CURLcode cf_h2_proxy_recv(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 حرف *buf, size_t len,
                                 size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_data save;
  CURLcode result;

  *pnread = 0;
  CF_DATA_SAVE(save, cf, data);

  אם(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_RECV_ERROR;
    اذهب out;
  }

  אם(Curl_bufq_is_empty(&ctx->tunnel.recvbuf)) {
    result = proxy_h2_progress_ingress(cf, data);
    אם(result)
      اذهب out;
  }

  result = tunnel_recv(cf, data, buf, len, pnread);

  אם(!result) {
    CURL_TRC_CF(data, cf, "[%d] increase window by %zu",
                ctx->tunnel.stream_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.stream_id, *pnread);
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

out:
  אם((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (صحيح)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  החזר result;
}

ساكن CURLcode cf_h2_proxy_send(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 ثابت uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_data save;
  CURLcode result;

  (فراغ)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  אם(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_SEND_ERROR;
    اذهب out;
  }

  אם(ctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    اذهب out;
  }

  result = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(data, cf, "cf_send(), bufq_write %d, %zu", (صحيح)result,
              *pnwritten);
  אם(result && (result != CURLE_AGAIN))
    اذهب out;

  אם(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    אם(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      اذهب out;
    }
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_ingress(cf, data));
  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

  אם(!result && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    אם(ctx->tunnel.closed) {
      result = CURLE_SEND_ERROR;
    }
    אחרת {
      CURL_TRC_CF(data, cf, "[0] send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  אם((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, len, (صحيح)result, *pnwritten,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  החזר result;
}

ساكن CURLcode cf_h2_proxy_flush(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  אם(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    אם(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      اذهب out;
    }
  }

  result = proxy_h2_progress_egress(cf, data);

out:
  CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, (صحيح)result,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  החזר result;
}

ساكن bool proxy_h2_connisalive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 bool *input_pending)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *input_pending = FALSE;
  אם(!cf->next || !cf->next->cft->is_alive(cf->next, data, input_pending))
    החזר FALSE;

  אם(*input_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *input_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    אם(!result) {
      אם(proxy_h2_process_pending_input(cf, data))
        /* immediate error, considered dead */
        alive = FALSE;
      אחרת {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    אחרת אם(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  החזר alive;
}

ساكن bool cf_h2_proxy_is_alive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 bool *input_pending)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  بنية cf_call_data save;

  *input_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, data, input_pending));
  CURL_TRC_CF(data, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *input_pending);
  CF_DATA_RESTORE(cf, save);
  החזר alive;
}

ساكن CURLcode cf_h2_proxy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_HOST_PORT:
    *pres1 = (صحيح)ctx->dest->port;
    *((ثابت حرف **)pres2) = ctx->dest->hostname;
    החזר CURLE_OK;
  حالة CF_QUERY_NEED_FLUSH: {
    אם(!Curl_bufq_is_empty(&ctx->outbufq) ||
       !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(data, cf, "needs flush");
      *pres1 = TRUE;
      החזר CURLE_OK;
    }
    اكسر;
  }
  حالة CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = عدم;
    החזר CURLE_OK;
  }
  افتراضي:
    اكسر;
  }
  החזר cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_h2_proxy_cntrl(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode result = CURLE_OK;
  بنية cf_call_data save;

  (فراغ)arg1;
  (فراغ)arg2;

  بدل(event) {
  حالة CF_CTRL_FLUSH:
    CF_DATA_SAVE(save, cf, data);
    result = cf_h2_proxy_flush(cf, data);
    CF_DATA_RESTORE(cf, save);
    اكسر;
  افتراضي:
    اكسر;
  }
  החזר result;
}

بنية Curl_cftype Curl_cft_h2_proxy = {
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

CURLcode Curl_cf_h2_proxy_insert_after(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *data,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *dest,
                                       bool udp_tunnel)
{
  بنية Curl_cfilter *cf_h2_proxy = عدم;
  بنية cf_h2_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (فراغ)data;
  ctx = curlx_calloc(1, حجم(*ctx));
  אם(!ctx)
    اذهب out;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  result = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  אם(result)
    اذهب out;
  ctx = عدم;
  Curl_conn_cf_insert_after(cf, cf_h2_proxy);

out:
  cf_h2_proxy_ctx_free(ctx);
  החזר result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
