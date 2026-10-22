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
  defined(USE_NGHTTP2)

#অন্তর্ভাৱ <nghttp2/nghttp2.h>

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "bufq.h"
#অন্তর্ভাৱ "curlx/dynbuf.h"
#অন্তর্ভাৱ "dynhds.h"
#অন্তর্ভাৱ "http2.h"
#অন্তর্ভাৱ "http_proxy.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "ngh2_shim.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "cf-h2-proxy.h"

#define PROXY_H2_CHUNK_SIZE  (16 * 1024)

#define PROXY_HTTP2_HUGE_WINDOW_SIZE (100 * 1024 * 1024)
#define H2_TUNNEL_WINDOW_SIZE        (10 * 1024 * 1024)

#define PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define PROXY_H2_NW_SEND_CHUNKS   1

#define H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_SIZE)


প্রকারনির্দেশ গণনা {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

সংরচনা tunnel_stream {
  সংরচনা http_resp *resp;
  সংরচনা bufq recvbuf;
  সংরচনা bufq sendbuf;
  ৱর্ণ *authority;
  int32_t stream_id;
  uint32_t error;
  h2_tunnel_state state;
  BIT(has_final_response);
  BIT(closed);
  BIT(reset);
};

স্থির CURLcode tunnel_stream_init(সংরচনা tunnel_stream *ts,
                                   সংরচনা Curl_peer *dest)
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
  যদি(!ts->authority)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  ঘূৰাই CURLE_OK;
}

স্থির শূন্য tunnel_stream_reset(সংরচনা tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = রিক্ত;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->stream_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

স্থির শূন্য tunnel_stream_clear(সংরচনা tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, পরিমাণ(*ts));
  ts->state = H2_TUNNEL_INIT;
}

স্থির শূন্য h2_tunnel_go_state(সংরচনা Curl_cfilter *cf,
                               সংরচনা tunnel_stream *ts,
                               h2_tunnel_state new_state,
                               সংরচনা Curl_easy *data,
                               bool udp_tunnel)
{
  (শূন্য)cf;
  (শূন্য)udp_tunnel;

  যদি(ts->state == new_state)
    ঘূৰাই;
  /* leaving this one */
  ৱিকল্প(ts->state) {
  পক্ষ H2_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    ভঙ্গ;
  স্ৱভাৱ:
    ভঙ্গ;
  }
  /* entering this one */
  ৱিকল্প(new_state) {
  পক্ষ H2_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'init'", ts->stream_id);
    tunnel_stream_reset(ts);
    ভঙ্গ;

  পক্ষ H2_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'connect'", ts->stream_id);
    ts->state = H2_TUNNEL_CONNECT;
    ভঙ্গ;

  পক্ষ H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'response'", ts->stream_id);
    ts->state = H2_TUNNEL_RESPONSE;
    ভঙ্গ;

  পক্ষ H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'established'",
                ts->stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  পক্ষ H2_TUNNEL_FAILED:
    যদি(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(data, cf, "[%d] new tunnel state 'failed'", ts->stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    ভঙ্গ;
  }
}

সংরচনা cf_h2_proxy_ctx {
  সংরচনা curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  সংরচনা cf_call_data call_data;

  সংরচনা bufq inbufq;  /* network receive buffer */
  সংরচনা bufq outbufq; /* network send buffer */

  সংরচনা Curl_peer *peer; /* proxy we talk to */
  সংরচনা Curl_peer *dest; /* where to tunnel to */
  সংরচনা tunnel_stream tunnel; /* our tunnel CONNECT stream */
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
#define CF_CTX_CALL_DATA(cf) ((সংরচনা cf_h2_proxy_ctx *)(cf)->ctx)->call_data

স্থির শূন্য cf_h2_proxy_ctx_clear(সংরচনা cf_h2_proxy_ctx *ctx)
{
  সংরচনা cf_call_data save = ctx->call_data;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_stream_clear(&ctx->tunnel);
  memset(ctx, 0, পরিমাণ(*ctx));
  ctx->call_data = save;
}

স্থির শূন্য cf_h2_proxy_ctx_free(সংরচনা cf_h2_proxy_ctx *ctx)
{
  যদি(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

স্থির শূন্য drain_tunnel(সংরচনা Curl_cfilter *cf,
                         সংরচনা Curl_easy *data,
                         সংরচনা tunnel_stream *tunnel)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  (শূন্য)cf;
  যদি(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_empty(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(data);
}

স্থির CURLcode proxy_h2_nw_out_writer(শূন্য *writer_ctx,
                                       ধ্রুৱ uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  সংরচনা Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  যদি(cf) {
    সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
    CURLcode result;
    result = Curl_conn_cf_send(cf->next, data, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(data, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (পূর্ণাঙ্ক)result, *pnwritten);
    ঘূৰাই result;
  }
  ঘূৰাই CURLE_FAILED_INIT;
}

স্থির পূর্ণাঙ্ক proxy_h2_client_new(সংরচনা Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { রিক্ত, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  পূর্ণাঙ্ক rc = nghttp2_option_new(&o);
  যদি(rc)
    ঘূৰাই rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#যদি NGHTTP2_VERSION_অঙ্ক >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ঘূৰাই rc;
}

স্থির পূর্ণাঙ্ক proxy_h2_should_close_session(সংরচনা cf_h2_proxy_ctx *ctx)
{
  ঘূৰাই !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

স্থির CURLcode proxy_h2_nw_out_flush(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  যদি(Curl_bufq_is_empty(&ctx->outbufq))
    ঘূৰাই CURLE_OK;

  result = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_out_writer, cf,
                          &nwritten);
  যদি(result) {
    যদি(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    ঘূৰাই result;
  }
  CURL_TRC_CF(data, cf, "[0] nw send buffer flushed");
  ঘূৰাই Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
স্থির CURLcode proxy_h2_process_pending_প্ৰবেশ(সংরচনা Curl_cfilter *cf,
                                               সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  ধ্রুৱ unsigned ৱর্ণ *buf;
  size_t blen, nread;
  ssize_t rv;

  যেতিয়ালৈ(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ধ্রুৱ uint8_t *)buf, blen);
    CURL_TRC_CF(data, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    যদি(!curlx_sztouz(rv, &nread)) {
      failf(data,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((পূর্ণাঙ্ক)rv));
      ঘূৰাই CURLE_RECV_ERROR;
    }
    অন্যথা যদি(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      ভঙ্গ;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    যদি(Curl_bufq_is_empty(&ctx->inbufq)) {
      CURL_TRC_CF(data, cf, "[0] all data in connection buffer processed");
      ভঙ্গ;
    }
    অন্যথা {
      CURL_TRC_CF(data, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode proxy_h2_progress_ingress(সংরচনা Curl_cfilter *cf,
                                          সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  যদি(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = proxy_h2_process_pending_প্ৰবেশ(cf, data);
    যদি(result)
      ঘূৰাই result;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  যেতিয়ালৈ(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_empty(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_full(&ctx->tunnel.recvbuf)) {

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (পূর্ণাঙ্ক)result, nread);
    যদি(result) {
      যদি(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 proxy data");
        ঘূৰাই result;
      }
      ভঙ্গ;
    }
    অন্যথা যদি(nread == 0) {
      CURL_TRC_CF(data, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      ভঙ্গ;
    }

    result = proxy_h2_process_pending_প্ৰবেশ(cf, data);
    যদি(result)
      ঘূৰাই result;
  }

  ঘূৰাই CURLE_OK;
}

স্থির CURLcode proxy_h2_progress_egress(সংরচনা Curl_cfilter *cf,
                                         সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  পূর্ণাঙ্ক rv = 0;

  ctx->nw_out_blocked = 0;
  যেতিয়ালৈ(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  যদি(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ঘূৰাই CURLE_SEND_ERROR;
  }
  ঘূৰাই proxy_h2_nw_out_flush(cf, data);
}

স্থির ssize_t on_session_send(nghttp2_session *h2,
                               ধ্রুৱ uint8_t *buf, size_t blen, পূর্ণাঙ্ক flags,
                               শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (শূন্য)h2;
  (শূন্য)flags;
  DEBUGASSERT(data);

  result = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_out_writer, cf, &nwritten);
  যদি(result) {
    যদি(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      ঘূৰাই NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    ঘূৰাই NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  যদি(!nwritten)
    ঘূৰাই NGHTTP2_ERR_WOULDBLOCK;

  ঘূৰাই (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#ifdef CURLVERBOSE
স্থির পূর্ণাঙ্ক proxy_h2_on_frame_send(nghttp2_session *session,
                                  ধ্রুৱ nghttp2_frame *frame,
                                  শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);

  (শূন্য)session;
  DEBUGASSERT(data);
  যদি(Curl_trc_cf_is_verbose(cf, data)) {
    ৱর্ণ buffer[256];
    পূর্ণাঙ্ক len;
    len = Curl_nghttp2_fr_print(frame, buffer, পরিমাণ(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  ঘূৰাই 0;
}
#endif /* CURLVERBOSE */

স্থির পূর্ণাঙ্ক proxy_h2_on_frame_recv(nghttp2_session *session,
                                  ধ্রুৱ nghttp2_frame *frame,
                                  শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;

  (শূন্য)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  যদি(Curl_trc_cf_is_verbose(cf, data)) {
    ৱর্ণ buffer[256];
    পূর্ণাঙ্ক len;
    len = Curl_nghttp2_fr_print(frame, buffer, পরিমাণ(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  যদি(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    ৱিকল্প(frame->hd.type) {
    পক্ষ NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      যদি(CURL_REQ_WANT_SEND(data)) {
        drain_tunnel(cf, data, &ctx->tunnel);
      }
      ভঙ্গ;
    পক্ষ NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      যদি(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              frame->goaway.error_code, ctx->remote_max_sid);
      }
      ভঙ্গ;
    স্ৱভাৱ:
      ভঙ্গ;
    }
    ঘূৰাই 0;
  }

  যদি(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] rcvd FRAME not for tunnel", stream_id);
    ঘূৰাই NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ৱিকল্প(frame->hd.type) {
  পক্ষ NGHTTP2_HEADERS:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    যদি(!ctx->tunnel.resp)
      ঘূৰাই NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(data, cf, "[%d] got http status: %d",
                stream_id, ctx->tunnel.resp->status);
    যদি(!ctx->tunnel.has_final_response && ctx->tunnel.resp->status / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    ভঙ্গ;
  পক্ষ NGHTTP2_WINDOW_UPDATE:
    যদি(CURL_REQ_WANT_SEND(data)) {
      drain_tunnel(cf, data, &ctx->tunnel);
    }
    ভঙ্গ;
  পক্ষ NGHTTP2_RST_STREAM:
    যদি(frame->rst_stream.error_code)
      ctx->tunnel.reset = TRUE;
    ভঙ্গ;
  স্ৱভাৱ:
    ভঙ্গ;
  }
  ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক proxy_h2_on_header(nghttp2_session *session,
                              ধ্রুৱ nghttp2_frame *frame,
                              ধ্রুৱ uint8_t *name, size_t namelen,
                              ধ্রুৱ uint8_t *মান, size_t valuelen,
                              uint8_t flags,
                              শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (শূন্য)flags;
  (শূন্য)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  যদি(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", stream_id,
                (পূর্ণাঙ্ক)namelen, name, (পূর্ণাঙ্ক)valuelen, মান);
    ঘূৰাই NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  যদি(frame->hd.type == NGHTTP2_ঠেলো_PROMISE)
    ঘূৰাই NGHTTP2_ERR_CALLBACK_FAILURE;

  যদি(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    ঘূৰাই 0;
  }

  যদি(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    int32_t http_status;
    সংরচনা http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    result = Curl_http_decode_status(&http_status,
                                    (ধ্রুৱ ৱর্ণ *)মান, valuelen);
    যদি(result)
      ঘূৰাই NGHTTP2_ERR_CALLBACK_FAILURE;
    result = Curl_http_resp_make(&resp, http_status, রিক্ত);
    যদি(result)
      ঘূৰাই NGHTTP2_ERR_CALLBACK_FAILURE;
    যদি(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream_id, ctx->tunnel.resp->status);
    ঘূৰাই 0;
  }

  যদি(!ctx->tunnel.resp)
    ঘূৰাই NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_dynhds_add(&ctx->tunnel.resp->headers,
                           (ধ্রুৱ ৱর্ণ *)name, namelen,
                           (ধ্রুৱ ৱর্ণ *)মান, valuelen);
  যদি(result)
    ঘূৰাই NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream_id, (পূর্ণাঙ্ক)namelen, name, (পূর্ণাঙ্ক)valuelen, মান);

  ঘূৰাই 0; /* 0 is successful */
}

স্থির ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint8_t *buf, size_t length,
                                    uint32_t *data_flags,
                                    nghttp2_data_source *source,
                                    শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  সংরচনা tunnel_stream *ts;
  CURLcode result;
  size_t nread;

  (শূন্য)session;
  (শূন্য)source;
  (শূন্য)ctx;

  যদি(!stream_id)
    ঘূৰাই NGHTTP2_ERR_INVALID_ARGUMENT;

  ts = Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  যদি(!ts)
    ঘূৰাই NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  result = Curl_bufq_read(&ts->sendbuf, buf, length, &nread);
  যদি(result) {
    যদি(result != CURLE_AGAIN)
      ঘূৰাই NGHTTP2_ERR_CALLBACK_FAILURE;
    ঘূৰাই NGHTTP2_ERR_DEFERRED;
  }
  যদি(ts->closed && Curl_bufq_is_empty(&ts->sendbuf))
    *data_flags = NGHTTP2_DATA_FLAG_EOF;

  CURL_TRC_CF(data, cf, "[%d] tunnel_send_callback -> %zu",
              ts->stream_id, nread);
  ঘূৰাই (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

স্থির পূর্ণাঙ্ক tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t stream_id,
                                ধ্রুৱ uint8_t *mem, size_t len, শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  (শূন্য)flags;
  (শূন্য)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  যদি(stream_id != ctx->tunnel.stream_id)
    ঘূৰাই NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  যদি(result) {
    যদি(result != CURLE_AGAIN)
      ঘূৰাই NGHTTP2_ERR_CALLBACK_FAILURE;
#ifdef DEBUGBUILD
    nwritten = 0;
#endif
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক proxy_h2_on_stream_close(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint32_t error_code, শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);

  (শূন্য)session;

  যদি(stream_id != ctx->tunnel.stream_id)
    ঘূৰাই 0;

  CURL_TRC_CF(data, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_code;
  যদি(error_code)
    ctx->tunnel.reset = TRUE;

  ঘূৰাই 0;
}

স্থির CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  সংরচনা Curl_cfilter *cf,
  সংরচনা Curl_easy *data,
  সংরচনা curl_ngh2 *ngh2,
  সংরচনা httpreq *req,
  ধ্রুৱ nghttp2_priority_spec *pri_spec,
  শূন্য *stream_user_data,
  nghttp2_data_source_read_callback read_callback,
  শূন্য *read_ctx)
{
  সংরচনা dynhds h2_headers;
  nghttp2_nv *nva = রিক্ত;
  int32_t stream_id = -1;
  size_t nheader;
  CURLcode result;

  (শূন্য)cf;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_গুনতি, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  যদি(result)
    গচ্ছ out;

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  যদি(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  যদি(read_callback) {
    nghttp2_data_provider data_prd;

    data_prd.read_callback = read_callback;
    data_prd.source.ptr = read_ctx;
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &data_prd, stream_user_data);
  }
  অন্যথা {
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, রিক্ত, stream_user_data);
  }

  যদি(stream_id < 0) {
    failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    গচ্ছ out;
  }
  result = CURLE_OK;

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  *pstream_id = stream_id;
  ঘূৰাই result;
}

স্থির CURLcode submit_CONNECT(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               সংরচনা tunnel_stream *ts)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  সংরচনা httpreq *req = রিক্ত;

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  যদি(result)
    গচ্ছ out;
  result = Curl_creader_set_null(data);
  যদি(result)
    গচ্ছ out;

  result = proxy_h2_submit(&ts->stream_id, cf, data, &ctx->ngh2, req,
                           রিক্ত, ts, tunnel_send_callback, cf);
  যদি(result) {
    CURL_TRC_CF(data, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->stream_id, nghttp2_strerror(ts->stream_id));
  }

out:
  যদি(req)
    Curl_http_req_free(req);
  যদি(result)
    failf(data, "Failed sending CONNECT to proxy");
  ঘূৰাই result;
}

স্থির CURLcode inspect_response(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা tunnel_stream *ts)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)ctx->udp_tunnel, &res);
  যদি(result)
    ঘূৰাই result;
  ৱিকল্প(res) {
  পক্ষ PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, data,
                       (bool)ctx->udp_tunnel);
    ভঙ্গ;
  পক্ষ PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
    result = CURLE_COULDNT_CONNECT;
    ভঙ্গ;
  পক্ষ PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, data,
                       (bool)ctx->udp_tunnel);
    ভঙ্গ;
  }
  ঘূৰাই result;
}

স্থির CURLcode H2_CONNECT(সংরচনা Curl_cfilter *cf,
                           সংরচনা Curl_easy *data,
                           সংরচনা tunnel_stream *ts)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  যদি(ctx->conn_closed) {
    failf(data, "proxy closed connection");
    ঘূৰাই CURLE_COULDNT_CONNECT;
  }

  কৰা {
    ৱিকল্প(ts->state) {
    পক্ষ H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = submit_CONNECT(cf, data, ts);
      যদি(result)
        গচ্ছ out;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, data,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    পক্ষ H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      result = proxy_h2_progress_ingress(cf, data);
      যদি(!result)
        result = proxy_h2_progress_egress(cf, data);
      যদি(result && result != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                           (bool)ctx->udp_tunnel);
        ভঙ্গ;
      }

      যদি(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, data,
                           (bool)ctx->udp_tunnel);
      }
      অন্যথা {
        result = CURLE_OK;
        গচ্ছ out;
      }
      FALLTHROUGH();

    পক্ষ H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = inspect_response(cf, data, ts);
      যদি(result)
        গচ্ছ out;
      ভঙ্গ;

    পক্ষ H2_TUNNEL_ESTABLISHED:
      ঘূৰাই CURLE_OK;

    পক্ষ H2_TUNNEL_FAILED:
      ঘূৰাই CURLE_RECV_ERROR;

    স্ৱভাৱ:
      ভঙ্গ;
    }

  } যেতিয়ালৈ(ts->state == H2_TUNNEL_INIT);

out:
  যদি((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
  ঘূৰাই result;
}

/*
 * Initialize the cfilter context
 */
স্থির CURLcode cf_h2_proxy_ctx_init(সংরচনা Curl_cfilter *cf,
                                     সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  nghttp2_session_callbacks *cbs = রিক্ত;
  পূর্ণাঙ্ক rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, পরিমাণ(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  যদি(tunnel_stream_init(&ctx->tunnel, ctx->dest))
    গচ্ছ out;

  rc = nghttp2_session_callbacks_new(&cbs);
  যদি(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    গচ্ছ out;
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
  যদি(rc) {
    failf(data, "Could not initialize nghttp2");
    গচ্ছ out;
  }

  {
    nghttp2_settings_entry iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].মান = Curl_multi_max_concurrent_streams(data->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
    iv[1].মান = H2_TUNNEL_WINDOW_SIZE;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ঠেলো;
    iv[2].মান = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    যদি(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      গচ্ছ out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_WINDOW_SIZE);
  যদি(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    গচ্ছ out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;

out:
  যদি(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(data, cf, "[0] init proxy ctx -> %d", (পূর্ণাঙ্ক)result);
  ঘূৰাই result;
}

স্থির CURLcode cf_h2_proxy_connect(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    bool *done)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  সংরচনা cf_call_data save;
  সংরচনা tunnel_stream *ts = &ctx->tunnel;

  যদি(cf->connected) {
    *done = TRUE;
    ঘূৰাই CURLE_OK;
  }

  /* Connect the lower filters first */
  যদি(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    যদি(result || !*done)
      ঘূৰাই result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  যদি(!ctx->ngh2.s) {
    result = cf_h2_proxy_ctx_init(cf, data);
    যদি(result)
      গচ্ছ out;
  }
  DEBUGASSERT(ts->authority);

  যদি(Curl_timeleft_ms(data) < 0) {
    failf(data, "Proxy CONNECT aborted due to timeout");
    result = CURLE_OPERATION_TIMEDOUT;
    গচ্ছ out;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  result = H2_CONNECT(cf, data, ts);

out:
  *done = (result == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  যদি(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }
  CF_DATA_RESTORE(cf, save);
  ঘূৰাই result;
}

স্থির শূন্য cf_h2_proxy_destroy(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;

  (শূন্য)data;
  যদি(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = রিক্ত;
  }
}

স্থির CURLcode cf_h2_proxy_shutdown(সংরচনা Curl_cfilter *cf,
                                     সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  সংরচনা cf_call_data save;
  CURLcode result;
  পূর্ণাঙ্ক rv;

  যদি(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    ঘূৰাই CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  যদি(!ctx->sent_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ধ্রুৱ uint8_t *)"shutdown", পরিমাণ("shutdown"));
    যদি(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
    ctx->sent_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  যদি(Curl_ngh2_session_want_write(&ctx->ngh2))
    result = proxy_h2_progress_egress(cf, data);
  যদি(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = proxy_h2_progress_ingress(cf, data);

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  ঘূৰাই result;
}

স্থির bool cf_h2_proxy_data_pending(সংরচনা Curl_cfilter *cf,
                                     ধ্রুৱ সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  যদি((ctx && !Curl_bufq_is_empty(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    ঘূৰাই TRUE;
  ঘূৰাই cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

স্থির CURLcode cf_h2_proxy_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                           সংরচনা Curl_easy *data,
                                           সংরচনা easy_pollset *ps)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  সংরচনা cf_call_data save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  যদি(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  অন্যথা
    Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  যদি(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
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
                want_recv, want_send, (পূর্ণাঙ্ক)result);
    CF_DATA_RESTORE(cf, save);
  }
  অন্যথা যদি(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CURL_TRC_CF(data, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (পূর্ণাঙ্ক)result);
    CF_DATA_RESTORE(cf, save);
  }
  ঘূৰাই result;
}

স্থির CURLcode h2_handle_tunnel_close(সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data,
                                       size_t *pnread)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  যদি(ctx->tunnel.error) {
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.stream_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    ঘূৰাই CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(data, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.stream_id);
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode tunnel_recv(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                            ৱর্ণ *buf, size_t len, size_t *pnread)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  *pnread = 0;
  যদি(!Curl_bufq_is_empty(&ctx->tunnel.recvbuf))
    result = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  অন্যথা {
    যদি(ctx->tunnel.closed) {
      result = h2_handle_tunnel_close(cf, data, pnread);
    }
    অন্যথা যদি(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.stream_id)) {
      result = CURLE_RECV_ERROR;
    }
    অন্যথা
      result = CURLE_AGAIN;
  }

  CURL_TRC_CF(data, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (পূর্ণাঙ্ক)result, *pnread);
  ঘূৰাই result;
}

স্থির CURLcode cf_h2_proxy_recv(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 ৱর্ণ *buf, size_t len,
                                 size_t *pnread)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  সংরচনা cf_call_data save;
  CURLcode result;

  *pnread = 0;
  CF_DATA_SAVE(save, cf, data);

  যদি(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_RECV_ERROR;
    গচ্ছ out;
  }

  যদি(Curl_bufq_is_empty(&ctx->tunnel.recvbuf)) {
    result = proxy_h2_progress_ingress(cf, data);
    যদি(result)
      গচ্ছ out;
  }

  result = tunnel_recv(cf, data, buf, len, pnread);

  যদি(!result) {
    CURL_TRC_CF(data, cf, "[%d] increase window by %zu",
                ctx->tunnel.stream_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.stream_id, *pnread);
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

out:
  যদি((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (পূর্ণাঙ্ক)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  ঘূৰাই result;
}

স্থির CURLcode cf_h2_proxy_send(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 ধ্রুৱ uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  সংরচনা cf_call_data save;
  CURLcode result;

  (শূন্য)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  যদি(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_SEND_ERROR;
    গচ্ছ out;
  }

  যদি(ctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    গচ্ছ out;
  }

  result = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(data, cf, "cf_send(), bufq_write %d, %zu", (পূর্ণাঙ্ক)result,
              *pnwritten);
  যদি(result && (result != CURLE_AGAIN))
    গচ্ছ out;

  যদি(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_ingress(cf, data));
  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

  যদি(!result && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    যদি(ctx->tunnel.closed) {
      result = CURLE_SEND_ERROR;
    }
    অন্যথা {
      CURL_TRC_CF(data, cf, "[0] send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  যদি((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, len, (পূর্ণাঙ্ক)result, *pnwritten,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  ঘূৰাই result;
}

স্থির CURLcode cf_h2_proxy_flush(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  সংরচনা cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  যদি(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
  }

  result = proxy_h2_progress_egress(cf, data);

out:
  CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, (পূর্ণাঙ্ক)result,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  ঘূৰাই result;
}

স্থির bool proxy_h2_connisalive(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 bool *প্ৰবেশ_pending)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *প্ৰবেশ_pending = FALSE;
  যদি(!cf->next || !cf->next->cft->is_alive(cf->next, data, প্ৰবেশ_pending))
    ঘূৰাই FALSE;

  যদি(*প্ৰবেশ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *প্ৰবেশ_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    যদি(!result) {
      যদি(proxy_h2_process_pending_প্ৰবেশ(cf, data))
        /* immediate error, considered dead */
        alive = FALSE;
      অন্যথা {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    অন্যথা যদি(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ঘূৰাই alive;
}

স্থির bool cf_h2_proxy_is_alive(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 bool *প্ৰবেশ_pending)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  সংরচনা cf_call_data save;

  *প্ৰবেশ_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, data, প্ৰবেশ_pending));
  CURL_TRC_CF(data, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *প্ৰবেশ_pending);
  CF_DATA_RESTORE(cf, save);
  ঘূৰাই alive;
}

স্থির CURLcode cf_h2_proxy_query(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  পূর্ণাঙ্ক query, পূর্ণাঙ্ক *pres1, শূন্য *pres2)
{
  সংরচনা cf_h2_proxy_ctx *ctx = cf->ctx;

  ৱিকল্প(query) {
  পক্ষ CF_QUERY_HOST_PORT:
    *pres1 = (পূর্ণাঙ্ক)ctx->dest->port;
    *((ধ্রুৱ ৱর্ণ **)pres2) = ctx->dest->hostname;
    ঘূৰাই CURLE_OK;
  পক্ষ CF_QUERY_NEED_FLUSH: {
    যদি(!Curl_bufq_is_empty(&ctx->outbufq) ||
       !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(data, cf, "needs flush");
      *pres1 = TRUE;
      ঘূৰাই CURLE_OK;
    }
    ভঙ্গ;
  }
  পক্ষ CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুৱ ৱর্ণ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = রিক্ত;
    ঘূৰাই CURLE_OK;
  }
  স্ৱভাৱ:
    ভঙ্গ;
  }
  ঘূৰাই cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির CURLcode cf_h2_proxy_cntrl(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  পূর্ণাঙ্ক event, পূর্ণাঙ্ক arg1, শূন্য *arg2)
{
  CURLcode result = CURLE_OK;
  সংরচনা cf_call_data save;

  (শূন্য)arg1;
  (শূন্য)arg2;

  ৱিকল্প(event) {
  পক্ষ CF_CTRL_FLUSH:
    CF_DATA_SAVE(save, cf, data);
    result = cf_h2_proxy_flush(cf, data);
    CF_DATA_RESTORE(cf, save);
    ভঙ্গ;
  স্ৱভাৱ:
    ভঙ্গ;
  }
  ঘূৰাই result;
}

সংরচনা Curl_cftype Curl_cft_h2_proxy = {
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

CURLcode Curl_cf_h2_proxy_ঢোকাও_after(সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data,
                                       সংরচনা Curl_peer *peer,
                                       সংরচনা Curl_peer *dest,
                                       bool udp_tunnel)
{
  সংরচনা Curl_cfilter *cf_h2_proxy = রিক্ত;
  সংরচনা cf_h2_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (শূন্য)data;
  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx)
    গচ্ছ out;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  result = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  যদি(result)
    গচ্ছ out;
  ctx = রিক্ত;
  Curl_conn_cf_ঢোকাও_after(cf, cf_h2_proxy);

out:
  cf_h2_proxy_ctx_free(ctx);
  ঘূৰাই result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
