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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_NGHTTP2)

#شامل <nghttp2/nghttp2.h>

#شامل "urldata.h"
#شامل "url.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "curl_trc.h"
#شامل "bufq.h"
#شامل "curlx/dynbuf.h"
#شامل "dynhds.h"
#شامل "http2.h"
#شامل "http_proxy.h"
#شامل "multiif.h"
#شامل "ngh2_shim.h"
#شامل "sendf.h"
#شامل "select.h"
#شامل "cf-h2-proxy.h"

#define PROXY_H2_CHUNK_SIZE  (16 * 1024)

#define PROXY_HTTP2_HUGE_WINDOW_SIZE (100 * 1024 * 1024)
#define H2_TUNNEL_WINDOW_SIZE        (10 * 1024 * 1024)

#define PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define PROXY_H2_NW_SEND_CHUNKS   1

#define H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_SIZE)


تعریفنوع شمارش {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

ساختار tunnel_stream {
  ساختار http_resp *resp;
  ساختار bufq recvbuf;
  ساختار bufq sendbuf;
  نویسه *authority;
  int32_t stream_id;
  uint32_t error;
  h2_tunnel_state state;
  BIT(has_final_response);
  BIT(closed);
  BIT(reset);
};

ایستا CURLcode tunnel_stream_init(ساختار tunnel_stream *ts,
                                   ساختار Curl_peer *dest)
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
  اگر(!ts->authority)
    بازگشت CURLE_OUT_OF_MEMORY;

  بازگشت CURLE_OK;
}

ایستا تهی tunnel_stream_reset(ساختار tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = پوچ;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->stream_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

ایستا تهی tunnel_stream_clear(ساختار tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, اندازه(*ts));
  ts->state = H2_TUNNEL_INIT;
}

ایستا تهی h2_tunnel_go_state(ساختار Curl_cfilter *cf,
                               ساختار tunnel_stream *ts,
                               h2_tunnel_state new_state,
                               ساختار Curl_easy *data,
                               bool udp_tunnel)
{
  (تهی)cf;
  (تهی)udp_tunnel;

  اگر(ts->state == new_state)
    بازگشت;
  /* leaving this one */
  انتخاب(ts->state) {
  حالت H2_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    بشکن;
  پیشفرض:
    بشکن;
  }
  /* entering this one */
  انتخاب(new_state) {
  حالت H2_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'init'", ts->stream_id);
    tunnel_stream_reset(ts);
    بشکن;

  حالت H2_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'connect'", ts->stream_id);
    ts->state = H2_TUNNEL_CONNECT;
    بشکن;

  حالت H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'response'", ts->stream_id);
    ts->state = H2_TUNNEL_RESPONSE;
    بشکن;

  حالت H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'established'",
                ts->stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  حالت H2_TUNNEL_FAILED:
    اگر(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(data, cf, "[%d] new tunnel state 'failed'", ts->stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    بشکن;
  }
}

ساختار cf_h2_proxy_ctx {
  ساختار curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ساختار cf_call_data call_data;

  ساختار bufq inbufq;  /* network receive buffer */
  ساختار bufq outbufq; /* network send buffer */

  ساختار Curl_peer *peer; /* proxy we talk to */
  ساختار Curl_peer *dest; /* where to tunnel to */
  ساختار tunnel_stream tunnel; /* our tunnel CONNECT stream */
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
#define CF_CTX_CALL_DATA(cf) ((ساختار cf_h2_proxy_ctx *)(cf)->ctx)->call_data

ایستا تهی cf_h2_proxy_ctx_clear(ساختار cf_h2_proxy_ctx *ctx)
{
  ساختار cf_call_data save = ctx->call_data;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_stream_clear(&ctx->tunnel);
  memset(ctx, 0, اندازه(*ctx));
  ctx->call_data = save;
}

ایستا تهی cf_h2_proxy_ctx_free(ساختار cf_h2_proxy_ctx *ctx)
{
  اگر(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

ایستا تهی drain_tunnel(ساختار Curl_cfilter *cf,
                         ساختار Curl_easy *data,
                         ساختار tunnel_stream *tunnel)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  (تهی)cf;
  اگر(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_empty(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(data);
}

ایستا CURLcode proxy_h2_nw_out_writer(تهی *writer_ctx,
                                       ثابت uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  ساختار Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  اگر(cf) {
    ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
    CURLcode result;
    result = Curl_conn_cf_send(cf->next, data, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(data, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (صحیح)result, *pnwritten);
    بازگشت result;
  }
  بازگشت CURLE_FAILED_INIT;
}

ایستا صحیح proxy_h2_client_new(ساختار Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { پوچ, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  صحیح rc = nghttp2_option_new(&o);
  اگر(rc)
    بازگشت rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#اگر NGHTTP2_VERSION_رقم >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  بازگشت rc;
}

ایستا صحیح proxy_h2_should_close_session(ساختار cf_h2_proxy_ctx *ctx)
{
  بازگشت !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

ایستا CURLcode proxy_h2_nw_out_flush(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  اگر(Curl_bufq_is_empty(&ctx->outbufq))
    بازگشت CURLE_OK;

  result = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_out_writer, cf,
                          &nwritten);
  اگر(result) {
    اگر(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    بازگشت result;
  }
  CURL_TRC_CF(data, cf, "[0] nw send buffer flushed");
  بازگشت Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ایستا CURLcode proxy_h2_process_pending_درون_داد(ساختار Curl_cfilter *cf,
                                               ساختار Curl_easy *data)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  ثابت unsigned نویسه *buf;
  size_t blen, nread;
  ssize_t rv;

  وقتی(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ثابت uint8_t *)buf, blen);
    CURL_TRC_CF(data, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    اگر(!curlx_sztouz(rv, &nread)) {
      failf(data,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((صحیح)rv));
      بازگشت CURLE_RECV_ERROR;
    }
    وگرنه اگر(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      بشکن;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    اگر(Curl_bufq_is_empty(&ctx->inbufq)) {
      CURL_TRC_CF(data, cf, "[0] all data in connection buffer processed");
      بشکن;
    }
    وگرنه {
      CURL_TRC_CF(data, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode proxy_h2_progress_ingress(ساختار Curl_cfilter *cf,
                                          ساختار Curl_easy *data)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  اگر(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = proxy_h2_process_pending_درون_داد(cf, data);
    اگر(result)
      بازگشت result;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  وقتی(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_empty(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_full(&ctx->tunnel.recvbuf)) {

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (صحیح)result, nread);
    اگر(result) {
      اگر(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 proxy data");
        بازگشت result;
      }
      بشکن;
    }
    وگرنه اگر(nread == 0) {
      CURL_TRC_CF(data, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      بشکن;
    }

    result = proxy_h2_process_pending_درون_داد(cf, data);
    اگر(result)
      بازگشت result;
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode proxy_h2_progress_egress(ساختار Curl_cfilter *cf,
                                         ساختار Curl_easy *data)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  صحیح rv = 0;

  ctx->nw_out_blocked = 0;
  وقتی(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  اگر(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    بازگشت CURLE_SEND_ERROR;
  }
  بازگشت proxy_h2_nw_out_flush(cf, data);
}

ایستا ssize_t on_session_send(nghttp2_session *h2,
                               ثابت uint8_t *buf, size_t blen, صحیح flags,
                               تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (تهی)h2;
  (تهی)flags;
  DEBUGASSERT(data);

  result = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_out_writer, cf, &nwritten);
  اگر(result) {
    اگر(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      بازگشت NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  اگر(!nwritten)
    بازگشت NGHTTP2_ERR_WOULDBLOCK;

  بازگشت (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#ifdef CURLVERBOSE
ایستا صحیح proxy_h2_on_frame_send(nghttp2_session *session,
                                  ثابت nghttp2_frame *frame,
                                  تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);

  (تهی)session;
  DEBUGASSERT(data);
  اگر(Curl_trc_cf_is_verbose(cf, data)) {
    نویسه buffer[256];
    صحیح len;
    len = Curl_nghttp2_fr_print(frame, buffer, اندازه(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  بازگشت 0;
}
#endif /* CURLVERBOSE */

ایستا صحیح proxy_h2_on_frame_recv(nghttp2_session *session,
                                  ثابت nghttp2_frame *frame,
                                  تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;

  (تهی)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  اگر(Curl_trc_cf_is_verbose(cf, data)) {
    نویسه buffer[256];
    صحیح len;
    len = Curl_nghttp2_fr_print(frame, buffer, اندازه(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  اگر(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    انتخاب(frame->hd.type) {
    حالت NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      اگر(CURL_REQ_WANT_SEND(data)) {
        drain_tunnel(cf, data, &ctx->tunnel);
      }
      بشکن;
    حالت NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      اگر(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              frame->goaway.error_code, ctx->remote_max_sid);
      }
      بشکن;
    پیشفرض:
      بشکن;
    }
    بازگشت 0;
  }

  اگر(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] rcvd FRAME not for tunnel", stream_id);
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  انتخاب(frame->hd.type) {
  حالت NGHTTP2_HEADERS:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    اگر(!ctx->tunnel.resp)
      بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(data, cf, "[%d] got http status: %d",
                stream_id, ctx->tunnel.resp->status);
    اگر(!ctx->tunnel.has_final_response && ctx->tunnel.resp->status / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    بشکن;
  حالت NGHTTP2_WINDOW_UPDATE:
    اگر(CURL_REQ_WANT_SEND(data)) {
      drain_tunnel(cf, data, &ctx->tunnel);
    }
    بشکن;
  حالت NGHTTP2_RST_STREAM:
    اگر(frame->rst_stream.error_code)
      ctx->tunnel.reset = TRUE;
    بشکن;
  پیشفرض:
    بشکن;
  }
  بازگشت 0;
}

ایستا صحیح proxy_h2_on_header(nghttp2_session *session,
                              ثابت nghttp2_frame *frame,
                              ثابت uint8_t *name, size_t namelen,
                              ثابت uint8_t *مقدار, size_t valuelen,
                              uint8_t flags,
                              تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (تهی)flags;
  (تهی)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  اگر(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", stream_id,
                (صحیح)namelen, name, (صحیح)valuelen, مقدار);
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  اگر(frame->hd.type == NGHTTP2_افزودن_PROMISE)
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;

  اگر(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    بازگشت 0;
  }

  اگر(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    int32_t http_status;
    ساختار http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    result = Curl_http_decode_status(&http_status,
                                    (ثابت نویسه *)مقدار, valuelen);
    اگر(result)
      بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
    result = Curl_http_resp_make(&resp, http_status, پوچ);
    اگر(result)
      بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
    اگر(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream_id, ctx->tunnel.resp->status);
    بازگشت 0;
  }

  اگر(!ctx->tunnel.resp)
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_dynhds_add(&ctx->tunnel.resp->headers,
                           (ثابت نویسه *)name, namelen,
                           (ثابت نویسه *)مقدار, valuelen);
  اگر(result)
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream_id, (صحیح)namelen, name, (صحیح)valuelen, مقدار);

  بازگشت 0; /* 0 is successful */
}

ایستا ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint8_t *buf, size_t length,
                                    uint32_t *data_flags,
                                    nghttp2_data_source *source,
                                    تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
  ساختار tunnel_stream *ts;
  CURLcode result;
  size_t nread;

  (تهی)session;
  (تهی)source;
  (تهی)ctx;

  اگر(!stream_id)
    بازگشت NGHTTP2_ERR_INVALID_ARGUMENT;

  ts = Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  اگر(!ts)
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  result = Curl_bufq_read(&ts->sendbuf, buf, length, &nread);
  اگر(result) {
    اگر(result != CURLE_AGAIN)
      بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
    بازگشت NGHTTP2_ERR_DEFERRED;
  }
  اگر(ts->closed && Curl_bufq_is_empty(&ts->sendbuf))
    *data_flags = NGHTTP2_DATA_FLAG_EOF;

  CURL_TRC_CF(data, cf, "[%d] tunnel_send_callback -> %zu",
              ts->stream_id, nread);
  بازگشت (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

ایستا صحیح tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t stream_id,
                                ثابت uint8_t *mem, size_t len, تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  (تهی)flags;
  (تهی)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  اگر(stream_id != ctx->tunnel.stream_id)
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  اگر(result) {
    اگر(result != CURLE_AGAIN)
      بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
#ifdef DEBUGBUILD
    nwritten = 0;
#endif
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  بازگشت 0;
}

ایستا صحیح proxy_h2_on_stream_close(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint32_t error_code, تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);

  (تهی)session;

  اگر(stream_id != ctx->tunnel.stream_id)
    بازگشت 0;

  CURL_TRC_CF(data, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_code;
  اگر(error_code)
    ctx->tunnel.reset = TRUE;

  بازگشت 0;
}

ایستا CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  ساختار Curl_cfilter *cf,
  ساختار Curl_easy *data,
  ساختار curl_ngh2 *ngh2,
  ساختار httpreq *req,
  ثابت nghttp2_priority_spec *pri_spec,
  تهی *stream_user_data,
  nghttp2_data_source_read_callback read_callback,
  تهی *read_ctx)
{
  ساختار dynhds h2_headers;
  nghttp2_nv *nva = پوچ;
  int32_t stream_id = -1;
  size_t nheader;
  CURLcode result;

  (تهی)cf;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_شمار, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  اگر(result)
    برو out;

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  اگر(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  اگر(read_callback) {
    nghttp2_data_provider data_prd;

    data_prd.read_callback = read_callback;
    data_prd.source.ptr = read_ctx;
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &data_prd, stream_user_data);
  }
  وگرنه {
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, پوچ, stream_user_data);
  }

  اگر(stream_id < 0) {
    failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    برو out;
  }
  result = CURLE_OK;

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  *pstream_id = stream_id;
  بازگشت result;
}

ایستا CURLcode submit_CONNECT(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ساختار tunnel_stream *ts)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  ساختار httpreq *req = پوچ;

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  اگر(result)
    برو out;
  result = Curl_creader_set_null(data);
  اگر(result)
    برو out;

  result = proxy_h2_submit(&ts->stream_id, cf, data, &ctx->ngh2, req,
                           پوچ, ts, tunnel_send_callback, cf);
  اگر(result) {
    CURL_TRC_CF(data, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->stream_id, nghttp2_strerror(ts->stream_id));
  }

out:
  اگر(req)
    Curl_http_req_free(req);
  اگر(result)
    failf(data, "Failed sending CONNECT to proxy");
  بازگشت result;
}

ایستا CURLcode inspect_response(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار tunnel_stream *ts)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)ctx->udp_tunnel, &res);
  اگر(result)
    بازگشت result;
  انتخاب(res) {
  حالت PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, data,
                       (bool)ctx->udp_tunnel);
    بشکن;
  حالت PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
    result = CURLE_COULDNT_CONNECT;
    بشکن;
  حالت PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, data,
                       (bool)ctx->udp_tunnel);
    بشکن;
  }
  بازگشت result;
}

ایستا CURLcode H2_CONNECT(ساختار Curl_cfilter *cf,
                           ساختار Curl_easy *data,
                           ساختار tunnel_stream *ts)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  اگر(ctx->conn_closed) {
    failf(data, "proxy closed connection");
    بازگشت CURLE_COULDNT_CONNECT;
  }

  انجامبده {
    انتخاب(ts->state) {
    حالت H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = submit_CONNECT(cf, data, ts);
      اگر(result)
        برو out;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, data,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    حالت H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      result = proxy_h2_progress_ingress(cf, data);
      اگر(!result)
        result = proxy_h2_progress_egress(cf, data);
      اگر(result && result != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                           (bool)ctx->udp_tunnel);
        بشکن;
      }

      اگر(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, data,
                           (bool)ctx->udp_tunnel);
      }
      وگرنه {
        result = CURLE_OK;
        برو out;
      }
      FALLTHROUGH();

    حالت H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = inspect_response(cf, data, ts);
      اگر(result)
        برو out;
      بشکن;

    حالت H2_TUNNEL_ESTABLISHED:
      بازگشت CURLE_OK;

    حالت H2_TUNNEL_FAILED:
      بازگشت CURLE_RECV_ERROR;

    پیشفرض:
      بشکن;
    }

  } وقتی(ts->state == H2_TUNNEL_INIT);

out:
  اگر((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
  بازگشت result;
}

/*
 * Initialize the cfilter context
 */
ایستا CURLcode cf_h2_proxy_ctx_init(ساختار Curl_cfilter *cf,
                                     ساختار Curl_easy *data)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  nghttp2_session_callbacks *cbs = پوچ;
  صحیح rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, اندازه(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  اگر(tunnel_stream_init(&ctx->tunnel, ctx->dest))
    برو out;

  rc = nghttp2_session_callbacks_new(&cbs);
  اگر(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    برو out;
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
  اگر(rc) {
    failf(data, "Could not initialize nghttp2");
    برو out;
  }

  {
    nghttp2_settings_entry iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].مقدار = Curl_multi_max_concurrent_streams(data->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
    iv[1].مقدار = H2_TUNNEL_WINDOW_SIZE;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_افزودن;
    iv[2].مقدار = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    اگر(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      برو out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_WINDOW_SIZE);
  اگر(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    برو out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;

out:
  اگر(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(data, cf, "[0] init proxy ctx -> %d", (صحیح)result);
  بازگشت result;
}

ایستا CURLcode cf_h2_proxy_connect(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    bool *done)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ساختار cf_call_data save;
  ساختار tunnel_stream *ts = &ctx->tunnel;

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  /* Connect the lower filters first */
  اگر(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    اگر(result || !*done)
      بازگشت result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  اگر(!ctx->ngh2.s) {
    result = cf_h2_proxy_ctx_init(cf, data);
    اگر(result)
      برو out;
  }
  DEBUGASSERT(ts->authority);

  اگر(Curl_timeleft_ms(data) < 0) {
    failf(data, "Proxy CONNECT aborted due to timeout");
    result = CURLE_OPERATION_TIMEDOUT;
    برو out;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  result = H2_CONNECT(cf, data, ts);

out:
  *done = (result == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  اگر(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا تهی cf_h2_proxy_destroy(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;

  (تهی)data;
  اگر(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = پوچ;
  }
}

ایستا CURLcode cf_h2_proxy_shutdown(ساختار Curl_cfilter *cf,
                                     ساختار Curl_easy *data, bool *done)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  ساختار cf_call_data save;
  CURLcode result;
  صحیح rv;

  اگر(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  اگر(!ctx->sent_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ثابت uint8_t *)"shutdown", اندازه("shutdown"));
    اگر(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      برو out;
    }
    ctx->sent_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  اگر(Curl_ngh2_session_want_write(&ctx->ngh2))
    result = proxy_h2_progress_egress(cf, data);
  اگر(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = proxy_h2_progress_ingress(cf, data);

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  بازگشت result;
}

ایستا bool cf_h2_proxy_data_pending(ساختار Curl_cfilter *cf,
                                     ثابت ساختار Curl_easy *data)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  اگر((ctx && !Curl_bufq_is_empty(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    بازگشت TRUE;
  بازگشت cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ایستا CURLcode cf_h2_proxy_adjust_pollset(ساختار Curl_cfilter *cf,
                                           ساختار Curl_easy *data,
                                           ساختار easy_pollset *ps)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  ساختار cf_call_data save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  اگر(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  وگرنه
    Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  اگر(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
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
                want_recv, want_send, (صحیح)result);
    CF_DATA_RESTORE(cf, save);
  }
  وگرنه اگر(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CURL_TRC_CF(data, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (صحیح)result);
    CF_DATA_RESTORE(cf, save);
  }
  بازگشت result;
}

ایستا CURLcode h2_handle_tunnel_close(ساختار Curl_cfilter *cf,
                                       ساختار Curl_easy *data,
                                       size_t *pnread)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  اگر(ctx->tunnel.error) {
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.stream_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    بازگشت CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(data, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.stream_id);
  بازگشت CURLE_OK;
}

ایستا CURLcode tunnel_recv(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                            نویسه *buf, size_t len, size_t *pnread)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  *pnread = 0;
  اگر(!Curl_bufq_is_empty(&ctx->tunnel.recvbuf))
    result = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  وگرنه {
    اگر(ctx->tunnel.closed) {
      result = h2_handle_tunnel_close(cf, data, pnread);
    }
    وگرنه اگر(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.stream_id)) {
      result = CURLE_RECV_ERROR;
    }
    وگرنه
      result = CURLE_AGAIN;
  }

  CURL_TRC_CF(data, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (صحیح)result, *pnread);
  بازگشت result;
}

ایستا CURLcode cf_h2_proxy_recv(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 نویسه *buf, size_t len,
                                 size_t *pnread)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  ساختار cf_call_data save;
  CURLcode result;

  *pnread = 0;
  CF_DATA_SAVE(save, cf, data);

  اگر(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_RECV_ERROR;
    برو out;
  }

  اگر(Curl_bufq_is_empty(&ctx->tunnel.recvbuf)) {
    result = proxy_h2_progress_ingress(cf, data);
    اگر(result)
      برو out;
  }

  result = tunnel_recv(cf, data, buf, len, pnread);

  اگر(!result) {
    CURL_TRC_CF(data, cf, "[%d] increase window by %zu",
                ctx->tunnel.stream_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.stream_id, *pnread);
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

out:
  اگر((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (صحیح)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا CURLcode cf_h2_proxy_send(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ثابت uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  ساختار cf_call_data save;
  CURLcode result;

  (تهی)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  اگر(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_SEND_ERROR;
    برو out;
  }

  اگر(ctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    برو out;
  }

  result = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(data, cf, "cf_send(), bufq_write %d, %zu", (صحیح)result,
              *pnwritten);
  اگر(result && (result != CURLE_AGAIN))
    برو out;

  اگر(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      برو out;
    }
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_ingress(cf, data));
  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

  اگر(!result && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    اگر(ctx->tunnel.closed) {
      result = CURLE_SEND_ERROR;
    }
    وگرنه {
      CURL_TRC_CF(data, cf, "[0] send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  اگر((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, len, (صحیح)result, *pnwritten,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا CURLcode cf_h2_proxy_flush(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  ساختار cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  اگر(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      برو out;
    }
  }

  result = proxy_h2_progress_egress(cf, data);

out:
  CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, (صحیح)result,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا bool proxy_h2_connisalive(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 bool *درون_داد_pending)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *درون_داد_pending = FALSE;
  اگر(!cf->next || !cf->next->cft->is_alive(cf->next, data, درون_داد_pending))
    بازگشت FALSE;

  اگر(*درون_داد_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *درون_داد_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    اگر(!result) {
      اگر(proxy_h2_process_pending_درون_داد(cf, data))
        /* immediate error, considered dead */
        alive = FALSE;
      وگرنه {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    وگرنه اگر(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  بازگشت alive;
}

ایستا bool cf_h2_proxy_is_alive(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 bool *درون_داد_pending)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  ساختار cf_call_data save;

  *درون_داد_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, data, درون_داد_pending));
  CURL_TRC_CF(data, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *درون_داد_pending);
  CF_DATA_RESTORE(cf, save);
  بازگشت alive;
}

ایستا CURLcode cf_h2_proxy_query(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  صحیح query, صحیح *pres1, تهی *pres2)
{
  ساختار cf_h2_proxy_ctx *ctx = cf->ctx;

  انتخاب(query) {
  حالت CF_QUERY_HOST_PORT:
    *pres1 = (صحیح)ctx->dest->port;
    *((ثابت نویسه **)pres2) = ctx->dest->hostname;
    بازگشت CURLE_OK;
  حالت CF_QUERY_NEED_FLUSH: {
    اگر(!Curl_bufq_is_empty(&ctx->outbufq) ||
       !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(data, cf, "needs flush");
      *pres1 = TRUE;
      بازگشت CURLE_OK;
    }
    بشکن;
  }
  حالت CF_QUERY_ALPN_NEGOTIATED: {
    ثابت نویسه **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = پوچ;
    بازگشت CURLE_OK;
  }
  پیشفرض:
    بشکن;
  }
  بازگشت cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ایستا CURLcode cf_h2_proxy_cntrl(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  صحیح event, صحیح arg1, تهی *arg2)
{
  CURLcode result = CURLE_OK;
  ساختار cf_call_data save;

  (تهی)arg1;
  (تهی)arg2;

  انتخاب(event) {
  حالت CF_CTRL_FLUSH:
    CF_DATA_SAVE(save, cf, data);
    result = cf_h2_proxy_flush(cf, data);
    CF_DATA_RESTORE(cf, save);
    بشکن;
  پیشفرض:
    بشکن;
  }
  بازگشت result;
}

ساختار Curl_cftype Curl_cft_h2_proxy = {
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

CURLcode Curl_cf_h2_proxy_درج_after(ساختار Curl_cfilter *cf,
                                       ساختار Curl_easy *data,
                                       ساختار Curl_peer *peer,
                                       ساختار Curl_peer *dest,
                                       bool udp_tunnel)
{
  ساختار Curl_cfilter *cf_h2_proxy = پوچ;
  ساختار cf_h2_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (تهی)data;
  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx)
    برو out;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  result = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  اگر(result)
    برو out;
  ctx = پوچ;
  Curl_conn_cf_درج_after(cf, cf_h2_proxy);

out:
  cf_h2_proxy_ctx_free(ctx);
  بازگشت result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
