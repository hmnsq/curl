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
  defined(USE_NGHTTP2)

#അന്തര്ഭാവ <nghttp2/nghttp2.h>

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "bufq.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "dynhds.h"
#അന്തര്ഭാവ "http2.h"
#അന്തര്ഭാവ "http_proxy.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "ngh2_shim.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "cf-h2-proxy.h"

#define PROXY_H2_CHUNK_SIZE  (16 * 1024)

#define PROXY_HTTP2_HUGE_WINDOW_SIZE (100 * 1024 * 1024)
#define H2_TUNNEL_WINDOW_SIZE        (10 * 1024 * 1024)

#define PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define PROXY_H2_NW_SEND_CHUNKS   1

#define H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_SIZE)


തരനിർവചനം എണ്ണൽ {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

ഘടന tunnel_stream {
  ഘടന http_resp *resp;
  ഘടന bufq recvbuf;
  ഘടന bufq sendbuf;
  അക്ഷരം *authority;
  int32_t stream_id;
  uint32_t error;
  h2_tunnel_state state;
  BIT(has_final_response);
  BIT(closed);
  BIT(reset);
};

സ്ഥിതം CURLcode tunnel_stream_init(ഘടന tunnel_stream *ts,
                                   ഘടന Curl_peer *dest)
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
  എങ്കിൽ(!ts->authority)
    തിരികെ CURLE_OUT_OF_MEMORY;

  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം tunnel_stream_reset(ഘടന tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = ശൂന്യ;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->stream_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

സ്ഥിതം ശൂന്യം tunnel_stream_clear(ഘടന tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, വലുപ്പം(*ts));
  ts->state = H2_TUNNEL_INIT;
}

സ്ഥിതം ശൂന്യം h2_tunnel_go_state(ഘടന Curl_cfilter *cf,
                               ഘടന tunnel_stream *ts,
                               h2_tunnel_state new_state,
                               ഘടന Curl_easy *data,
                               bool udp_tunnel)
{
  (ശൂന്യം)cf;
  (ശൂന്യം)udp_tunnel;

  എങ്കിൽ(ts->state == new_state)
    തിരികെ;
  /* leaving this one */
  തിരഞ്ഞെടുപ്പ്(ts->state) {
  അവസ്ഥ H2_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  /* entering this one */
  തിരഞ്ഞെടുപ്പ്(new_state) {
  അവസ്ഥ H2_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'init'", ts->stream_id);
    tunnel_stream_reset(ts);
    നിർത്തുക;

  അവസ്ഥ H2_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'connect'", ts->stream_id);
    ts->state = H2_TUNNEL_CONNECT;
    നിർത്തുക;

  അവസ്ഥ H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'response'", ts->stream_id);
    ts->state = H2_TUNNEL_RESPONSE;
    നിർത്തുക;

  അവസ്ഥ H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'established'",
                ts->stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  അവസ്ഥ H2_TUNNEL_FAILED:
    എങ്കിൽ(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(data, cf, "[%d] new tunnel state 'failed'", ts->stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    നിർത്തുക;
  }
}

ഘടന cf_h2_proxy_ctx {
  ഘടന curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ഘടന cf_call_data call_data;

  ഘടന bufq inbufq;  /* network receive buffer */
  ഘടന bufq outbufq; /* network send buffer */

  ഘടന Curl_peer *peer; /* proxy we talk to */
  ഘടന Curl_peer *dest; /* where to tunnel to */
  ഘടന tunnel_stream tunnel; /* our tunnel CONNECT stream */
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
#define CF_CTX_CALL_DATA(cf) ((ഘടന cf_h2_proxy_ctx *)(cf)->ctx)->call_data

സ്ഥിതം ശൂന്യം cf_h2_proxy_ctx_clear(ഘടന cf_h2_proxy_ctx *ctx)
{
  ഘടന cf_call_data save = ctx->call_data;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_stream_clear(&ctx->tunnel);
  memset(ctx, 0, വലുപ്പം(*ctx));
  ctx->call_data = save;
}

സ്ഥിതം ശൂന്യം cf_h2_proxy_ctx_free(ഘടന cf_h2_proxy_ctx *ctx)
{
  എങ്കിൽ(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

സ്ഥിതം ശൂന്യം drain_tunnel(ഘടന Curl_cfilter *cf,
                         ഘടന Curl_easy *data,
                         ഘടന tunnel_stream *tunnel)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  (ശൂന്യം)cf;
  എങ്കിൽ(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_empty(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(data);
}

സ്ഥിതം CURLcode proxy_h2_nw_out_writer(ശൂന്യം *writer_ctx,
                                       സ്ഥിരം uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  ഘടന Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  എങ്കിൽ(cf) {
    ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
    CURLcode result;
    result = Curl_conn_cf_send(cf->next, data, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(data, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (പൂർണ്ണസംഖ്യ)result, *pnwritten);
    തിരികെ result;
  }
  തിരികെ CURLE_FAILED_INIT;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ proxy_h2_client_new(ഘടന Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ശൂന്യ, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  പൂർണ്ണസംഖ്യ rc = nghttp2_option_new(&o);
  എങ്കിൽ(rc)
    തിരികെ rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#എങ്കിൽ NGHTTP2_VERSION_അക്കം >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  തിരികെ rc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ proxy_h2_should_close_session(ഘടന cf_h2_proxy_ctx *ctx)
{
  തിരികെ !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

സ്ഥിതം CURLcode proxy_h2_nw_out_flush(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  എങ്കിൽ(Curl_bufq_is_empty(&ctx->outbufq))
    തിരികെ CURLE_OK;

  result = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_out_writer, cf,
                          &nwritten);
  എങ്കിൽ(result) {
    എങ്കിൽ(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    തിരികെ result;
  }
  CURL_TRC_CF(data, cf, "[0] nw send buffer flushed");
  തിരികെ Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
സ്ഥിതം CURLcode proxy_h2_process_pending_നിവേശം(ഘടന Curl_cfilter *cf,
                                               ഘടന Curl_easy *data)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  സ്ഥിരം unsigned അക്ഷരം *buf;
  size_t blen, nread;
  ssize_t rv;

  ആകുമ്പോൾ(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (സ്ഥിരം uint8_t *)buf, blen);
    CURL_TRC_CF(data, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    എങ്കിൽ(!curlx_sztouz(rv, &nread)) {
      failf(data,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((പൂർണ്ണസംഖ്യ)rv));
      തിരികെ CURLE_RECV_ERROR;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      നിർത്തുക;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    എങ്കിൽ(Curl_bufq_is_empty(&ctx->inbufq)) {
      CURL_TRC_CF(data, cf, "[0] all data in connection buffer processed");
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ {
      CURL_TRC_CF(data, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode proxy_h2_progress_ingress(ഘടന Curl_cfilter *cf,
                                          ഘടന Curl_easy *data)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  എങ്കിൽ(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = proxy_h2_process_pending_നിവേശം(cf, data);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  ആകുമ്പോൾ(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_empty(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_full(&ctx->tunnel.recvbuf)) {

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (പൂർണ്ണസംഖ്യ)result, nread);
    എങ്കിൽ(result) {
      എങ്കിൽ(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 proxy data");
        തിരികെ result;
      }
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(nread == 0) {
      CURL_TRC_CF(data, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      നിർത്തുക;
    }

    result = proxy_h2_process_pending_നിവേശം(cf, data);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode proxy_h2_progress_egress(ഘടന Curl_cfilter *cf,
                                         ഘടന Curl_easy *data)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  പൂർണ്ണസംഖ്യ rv = 0;

  ctx->nw_out_blocked = 0;
  ആകുമ്പോൾ(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  എങ്കിൽ(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    തിരികെ CURLE_SEND_ERROR;
  }
  തിരികെ proxy_h2_nw_out_flush(cf, data);
}

സ്ഥിതം ssize_t on_session_send(nghttp2_session *h2,
                               സ്ഥിരം uint8_t *buf, size_t blen, പൂർണ്ണസംഖ്യ flags,
                               ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (ശൂന്യം)h2;
  (ശൂന്യം)flags;
  DEBUGASSERT(data);

  result = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_out_writer, cf, &nwritten);
  എങ്കിൽ(result) {
    എങ്കിൽ(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      തിരികെ NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  എങ്കിൽ(!nwritten)
    തിരികെ NGHTTP2_ERR_WOULDBLOCK;

  തിരികെ (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#ifdef CURLVERBOSE
സ്ഥിതം പൂർണ്ണസംഖ്യ proxy_h2_on_frame_send(nghttp2_session *session,
                                  സ്ഥിരം nghttp2_frame *frame,
                                  ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);

  (ശൂന്യം)session;
  DEBUGASSERT(data);
  എങ്കിൽ(Curl_trc_cf_is_verbose(cf, data)) {
    അക്ഷരം buffer[256];
    പൂർണ്ണസംഖ്യ len;
    len = Curl_nghttp2_fr_print(frame, buffer, വലുപ്പം(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  തിരികെ 0;
}
#endif /* CURLVERBOSE */

സ്ഥിതം പൂർണ്ണസംഖ്യ proxy_h2_on_frame_recv(nghttp2_session *session,
                                  സ്ഥിരം nghttp2_frame *frame,
                                  ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;

  (ശൂന്യം)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  എങ്കിൽ(Curl_trc_cf_is_verbose(cf, data)) {
    അക്ഷരം buffer[256];
    പൂർണ്ണസംഖ്യ len;
    len = Curl_nghttp2_fr_print(frame, buffer, വലുപ്പം(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  എങ്കിൽ(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    തിരഞ്ഞെടുപ്പ്(frame->hd.type) {
    അവസ്ഥ NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      എങ്കിൽ(CURL_REQ_WANT_SEND(data)) {
        drain_tunnel(cf, data, &ctx->tunnel);
      }
      നിർത്തുക;
    അവസ്ഥ NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      എങ്കിൽ(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              frame->goaway.error_code, ctx->remote_max_sid);
      }
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }
    തിരികെ 0;
  }

  എങ്കിൽ(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] rcvd FRAME not for tunnel", stream_id);
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  തിരഞ്ഞെടുപ്പ്(frame->hd.type) {
  അവസ്ഥ NGHTTP2_HEADERS:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    എങ്കിൽ(!ctx->tunnel.resp)
      തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(data, cf, "[%d] got http status: %d",
                stream_id, ctx->tunnel.resp->status);
    എങ്കിൽ(!ctx->tunnel.has_final_response && ctx->tunnel.resp->status / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    നിർത്തുക;
  അവസ്ഥ NGHTTP2_WINDOW_UPDATE:
    എങ്കിൽ(CURL_REQ_WANT_SEND(data)) {
      drain_tunnel(cf, data, &ctx->tunnel);
    }
    നിർത്തുക;
  അവസ്ഥ NGHTTP2_RST_STREAM:
    എങ്കിൽ(frame->rst_stream.error_code)
      ctx->tunnel.reset = TRUE;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ proxy_h2_on_header(nghttp2_session *session,
                              സ്ഥിരം nghttp2_frame *frame,
                              സ്ഥിരം uint8_t *name, size_t namelen,
                              സ്ഥിരം uint8_t *മൂല്യം, size_t valuelen,
                              uint8_t flags,
                              ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (ശൂന്യം)flags;
  (ശൂന്യം)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  എങ്കിൽ(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", stream_id,
                (പൂർണ്ണസംഖ്യ)namelen, name, (പൂർണ്ണസംഖ്യ)valuelen, മൂല്യം);
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  എങ്കിൽ(frame->hd.type == NGHTTP2_തള്ളുക_PROMISE)
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;

  എങ്കിൽ(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    തിരികെ 0;
  }

  എങ്കിൽ(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    int32_t http_status;
    ഘടന http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    result = Curl_http_decode_status(&http_status,
                                    (സ്ഥിരം അക്ഷരം *)മൂല്യം, valuelen);
    എങ്കിൽ(result)
      തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
    result = Curl_http_resp_make(&resp, http_status, ശൂന്യ);
    എങ്കിൽ(result)
      തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
    എങ്കിൽ(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream_id, ctx->tunnel.resp->status);
    തിരികെ 0;
  }

  എങ്കിൽ(!ctx->tunnel.resp)
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_dynhds_add(&ctx->tunnel.resp->headers,
                           (സ്ഥിരം അക്ഷരം *)name, namelen,
                           (സ്ഥിരം അക്ഷരം *)മൂല്യം, valuelen);
  എങ്കിൽ(result)
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream_id, (പൂർണ്ണസംഖ്യ)namelen, name, (പൂർണ്ണസംഖ്യ)valuelen, മൂല്യം);

  തിരികെ 0; /* 0 is successful */
}

സ്ഥിതം ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint8_t *buf, size_t length,
                                    uint32_t *data_flags,
                                    nghttp2_data_source *source,
                                    ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  ഘടന tunnel_stream *ts;
  CURLcode result;
  size_t nread;

  (ശൂന്യം)session;
  (ശൂന്യം)source;
  (ശൂന്യം)ctx;

  എങ്കിൽ(!stream_id)
    തിരികെ NGHTTP2_ERR_INVALID_ARGUMENT;

  ts = Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  എങ്കിൽ(!ts)
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  result = Curl_bufq_read(&ts->sendbuf, buf, length, &nread);
  എങ്കിൽ(result) {
    എങ്കിൽ(result != CURLE_AGAIN)
      തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
    തിരികെ NGHTTP2_ERR_DEFERRED;
  }
  എങ്കിൽ(ts->closed && Curl_bufq_is_empty(&ts->sendbuf))
    *data_flags = NGHTTP2_DATA_FLAG_EOF;

  CURL_TRC_CF(data, cf, "[%d] tunnel_send_callback -> %zu",
              ts->stream_id, nread);
  തിരികെ (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t stream_id,
                                സ്ഥിരം uint8_t *mem, size_t len, ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  (ശൂന്യം)flags;
  (ശൂന്യം)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  എങ്കിൽ(stream_id != ctx->tunnel.stream_id)
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  എങ്കിൽ(result) {
    എങ്കിൽ(result != CURLE_AGAIN)
      തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
#ifdef DEBUGBUILD
    nwritten = 0;
#endif
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ proxy_h2_on_stream_close(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint32_t error_code, ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);

  (ശൂന്യം)session;

  എങ്കിൽ(stream_id != ctx->tunnel.stream_id)
    തിരികെ 0;

  CURL_TRC_CF(data, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_code;
  എങ്കിൽ(error_code)
    ctx->tunnel.reset = TRUE;

  തിരികെ 0;
}

സ്ഥിതം CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  ഘടന Curl_cfilter *cf,
  ഘടന Curl_easy *data,
  ഘടന curl_ngh2 *ngh2,
  ഘടന httpreq *req,
  സ്ഥിരം nghttp2_priority_spec *pri_spec,
  ശൂന്യം *stream_user_data,
  nghttp2_data_source_read_callback read_callback,
  ശൂന്യം *read_ctx)
{
  ഘടന dynhds h2_headers;
  nghttp2_nv *nva = ശൂന്യ;
  int32_t stream_id = -1;
  size_t nheader;
  CURLcode result;

  (ശൂന്യം)cf;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_എണ്ണം, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  എങ്കിൽ(result)
    പോകുക out;

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  എങ്കിൽ(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  എങ്കിൽ(read_callback) {
    nghttp2_data_provider data_prd;

    data_prd.read_callback = read_callback;
    data_prd.source.ptr = read_ctx;
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &data_prd, stream_user_data);
  }
  അല്ലെങ്കിൽ {
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, ശൂന്യ, stream_user_data);
  }

  എങ്കിൽ(stream_id < 0) {
    failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    പോകുക out;
  }
  result = CURLE_OK;

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  *pstream_id = stream_id;
  തിരികെ result;
}

സ്ഥിതം CURLcode submit_CONNECT(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               ഘടന tunnel_stream *ts)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  ഘടന httpreq *req = ശൂന്യ;

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  എങ്കിൽ(result)
    പോകുക out;
  result = Curl_creader_set_null(data);
  എങ്കിൽ(result)
    പോകുക out;

  result = proxy_h2_submit(&ts->stream_id, cf, data, &ctx->ngh2, req,
                           ശൂന്യ, ts, tunnel_send_callback, cf);
  എങ്കിൽ(result) {
    CURL_TRC_CF(data, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->stream_id, nghttp2_strerror(ts->stream_id));
  }

out:
  എങ്കിൽ(req)
    Curl_http_req_free(req);
  എങ്കിൽ(result)
    failf(data, "Failed sending CONNECT to proxy");
  തിരികെ result;
}

സ്ഥിതം CURLcode inspect_response(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന tunnel_stream *ts)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)ctx->udp_tunnel, &res);
  എങ്കിൽ(result)
    തിരികെ result;
  തിരഞ്ഞെടുപ്പ്(res) {
  അവസ്ഥ PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, data,
                       (bool)ctx->udp_tunnel);
    നിർത്തുക;
  അവസ്ഥ PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
    result = CURLE_COULDNT_CONNECT;
    നിർത്തുക;
  അവസ്ഥ PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, data,
                       (bool)ctx->udp_tunnel);
    നിർത്തുക;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode H2_CONNECT(ഘടന Curl_cfilter *cf,
                           ഘടന Curl_easy *data,
                           ഘടന tunnel_stream *ts)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  എങ്കിൽ(ctx->conn_closed) {
    failf(data, "proxy closed connection");
    തിരികെ CURLE_COULDNT_CONNECT;
  }

  ചെയ്യുക {
    തിരഞ്ഞെടുപ്പ്(ts->state) {
    അവസ്ഥ H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = submit_CONNECT(cf, data, ts);
      എങ്കിൽ(result)
        പോകുക out;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, data,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    അവസ്ഥ H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      result = proxy_h2_progress_ingress(cf, data);
      എങ്കിൽ(!result)
        result = proxy_h2_progress_egress(cf, data);
      എങ്കിൽ(result && result != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                           (bool)ctx->udp_tunnel);
        നിർത്തുക;
      }

      എങ്കിൽ(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, data,
                           (bool)ctx->udp_tunnel);
      }
      അല്ലെങ്കിൽ {
        result = CURLE_OK;
        പോകുക out;
      }
      FALLTHROUGH();

    അവസ്ഥ H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = inspect_response(cf, data, ts);
      എങ്കിൽ(result)
        പോകുക out;
      നിർത്തുക;

    അവസ്ഥ H2_TUNNEL_ESTABLISHED:
      തിരികെ CURLE_OK;

    അവസ്ഥ H2_TUNNEL_FAILED:
      തിരികെ CURLE_RECV_ERROR;

    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }

  } ആകുമ്പോൾ(ts->state == H2_TUNNEL_INIT);

out:
  എങ്കിൽ((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
  തിരികെ result;
}

/*
 * Initialize the cfilter context
 */
സ്ഥിതം CURLcode cf_h2_proxy_ctx_init(ഘടന Curl_cfilter *cf,
                                     ഘടന Curl_easy *data)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  nghttp2_session_callbacks *cbs = ശൂന്യ;
  പൂർണ്ണസംഖ്യ rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, വലുപ്പം(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  എങ്കിൽ(tunnel_stream_init(&ctx->tunnel, ctx->dest))
    പോകുക out;

  rc = nghttp2_session_callbacks_new(&cbs);
  എങ്കിൽ(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    പോകുക out;
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
  എങ്കിൽ(rc) {
    failf(data, "Could not initialize nghttp2");
    പോകുക out;
  }

  {
    nghttp2_settings_entry iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].മൂല്യം = Curl_multi_max_concurrent_streams(data->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
    iv[1].മൂല്യം = H2_TUNNEL_WINDOW_SIZE;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_തള്ളുക;
    iv[2].മൂല്യം = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    എങ്കിൽ(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      പോകുക out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_WINDOW_SIZE);
  എങ്കിൽ(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    പോകുക out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;

out:
  എങ്കിൽ(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(data, cf, "[0] init proxy ctx -> %d", (പൂർണ്ണസംഖ്യ)result);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h2_proxy_connect(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    bool *done)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ഘടന cf_call_data save;
  ഘടന tunnel_stream *ts = &ctx->tunnel;

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  /* Connect the lower filters first */
  എങ്കിൽ(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    എങ്കിൽ(result || !*done)
      തിരികെ result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  എങ്കിൽ(!ctx->ngh2.s) {
    result = cf_h2_proxy_ctx_init(cf, data);
    എങ്കിൽ(result)
      പോകുക out;
  }
  DEBUGASSERT(ts->authority);

  എങ്കിൽ(Curl_timeleft_ms(data) < 0) {
    failf(data, "Proxy CONNECT aborted due to timeout");
    result = CURLE_OPERATION_TIMEDOUT;
    പോകുക out;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  result = H2_CONNECT(cf, data, ts);

out:
  *done = (result == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  എങ്കിൽ(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_h2_proxy_destroy(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;

  (ശൂന്യം)data;
  എങ്കിൽ(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = ശൂന്യ;
  }
}

സ്ഥിതം CURLcode cf_h2_proxy_shutdown(ഘടന Curl_cfilter *cf,
                                     ഘടന Curl_easy *data, bool *done)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  ഘടന cf_call_data save;
  CURLcode result;
  പൂർണ്ണസംഖ്യ rv;

  എങ്കിൽ(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  എങ്കിൽ(!ctx->sent_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (സ്ഥിരം uint8_t *)"shutdown", വലുപ്പം("shutdown"));
    എങ്കിൽ(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      പോകുക out;
    }
    ctx->sent_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  എങ്കിൽ(Curl_ngh2_session_want_write(&ctx->ngh2))
    result = proxy_h2_progress_egress(cf, data);
  എങ്കിൽ(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = proxy_h2_progress_ingress(cf, data);

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  തിരികെ result;
}

സ്ഥിതം bool cf_h2_proxy_data_pending(ഘടന Curl_cfilter *cf,
                                     സ്ഥിരം ഘടന Curl_easy *data)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  എങ്കിൽ((ctx && !Curl_bufq_is_empty(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    തിരികെ TRUE;
  തിരികെ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

സ്ഥിതം CURLcode cf_h2_proxy_adjust_pollset(ഘടന Curl_cfilter *cf,
                                           ഘടന Curl_easy *data,
                                           ഘടന easy_pollset *ps)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  ഘടന cf_call_data save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  അല്ലെങ്കിൽ
    Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  എങ്കിൽ(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
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
                want_recv, want_send, (പൂർണ്ണസംഖ്യ)result);
    CF_DATA_RESTORE(cf, save);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CURL_TRC_CF(data, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (പൂർണ്ണസംഖ്യ)result);
    CF_DATA_RESTORE(cf, save);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode h2_handle_tunnel_close(ഘടന Curl_cfilter *cf,
                                       ഘടന Curl_easy *data,
                                       size_t *pnread)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  എങ്കിൽ(ctx->tunnel.error) {
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.stream_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    തിരികെ CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(data, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.stream_id);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode tunnel_recv(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                            അക്ഷരം *buf, size_t len, size_t *pnread)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  *pnread = 0;
  എങ്കിൽ(!Curl_bufq_is_empty(&ctx->tunnel.recvbuf))
    result = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  അല്ലെങ്കിൽ {
    എങ്കിൽ(ctx->tunnel.closed) {
      result = h2_handle_tunnel_close(cf, data, pnread);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.stream_id)) {
      result = CURLE_RECV_ERROR;
    }
    അല്ലെങ്കിൽ
      result = CURLE_AGAIN;
  }

  CURL_TRC_CF(data, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (പൂർണ്ണസംഖ്യ)result, *pnread);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h2_proxy_recv(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 അക്ഷരം *buf, size_t len,
                                 size_t *pnread)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  ഘടന cf_call_data save;
  CURLcode result;

  *pnread = 0;
  CF_DATA_SAVE(save, cf, data);

  എങ്കിൽ(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_RECV_ERROR;
    പോകുക out;
  }

  എങ്കിൽ(Curl_bufq_is_empty(&ctx->tunnel.recvbuf)) {
    result = proxy_h2_progress_ingress(cf, data);
    എങ്കിൽ(result)
      പോകുക out;
  }

  result = tunnel_recv(cf, data, buf, len, pnread);

  എങ്കിൽ(!result) {
    CURL_TRC_CF(data, cf, "[%d] increase window by %zu",
                ctx->tunnel.stream_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.stream_id, *pnread);
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

out:
  എങ്കിൽ((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (പൂർണ്ണസംഖ്യ)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h2_proxy_send(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 സ്ഥിരം uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  ഘടന cf_call_data save;
  CURLcode result;

  (ശൂന്യം)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  എങ്കിൽ(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_SEND_ERROR;
    പോകുക out;
  }

  എങ്കിൽ(ctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    പോകുക out;
  }

  result = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(data, cf, "cf_send(), bufq_write %d, %zu", (പൂർണ്ണസംഖ്യ)result,
              *pnwritten);
  എങ്കിൽ(result && (result != CURLE_AGAIN))
    പോകുക out;

  എങ്കിൽ(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    എങ്കിൽ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      പോകുക out;
    }
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_ingress(cf, data));
  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

  എങ്കിൽ(!result && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    എങ്കിൽ(ctx->tunnel.closed) {
      result = CURLE_SEND_ERROR;
    }
    അല്ലെങ്കിൽ {
      CURL_TRC_CF(data, cf, "[0] send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  എങ്കിൽ((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, len, (പൂർണ്ണസംഖ്യ)result, *pnwritten,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h2_proxy_flush(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  ഘടന cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  എങ്കിൽ(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    എങ്കിൽ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      പോകുക out;
    }
  }

  result = proxy_h2_progress_egress(cf, data);

out:
  CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, (പൂർണ്ണസംഖ്യ)result,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം bool proxy_h2_connisalive(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 bool *നിവേശം_pending)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *നിവേശം_pending = FALSE;
  എങ്കിൽ(!cf->next || !cf->next->cft->is_alive(cf->next, data, നിവേശം_pending))
    തിരികെ FALSE;

  എങ്കിൽ(*നിവേശം_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *നിവേശം_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    എങ്കിൽ(!result) {
      എങ്കിൽ(proxy_h2_process_pending_നിവേശം(cf, data))
        /* immediate error, considered dead */
        alive = FALSE;
      അല്ലെങ്കിൽ {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  തിരികെ alive;
}

സ്ഥിതം bool cf_h2_proxy_is_alive(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 bool *നിവേശം_pending)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  ഘടന cf_call_data save;

  *നിവേശം_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, data, നിവേശം_pending));
  CURL_TRC_CF(data, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *നിവേശം_pending);
  CF_DATA_RESTORE(cf, save);
  തിരികെ alive;
}

സ്ഥിതം CURLcode cf_h2_proxy_query(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  ഘടന cf_h2_proxy_ctx *ctx = cf->ctx;

  തിരഞ്ഞെടുപ്പ്(query) {
  അവസ്ഥ CF_QUERY_HOST_PORT:
    *pres1 = (പൂർണ്ണസംഖ്യ)ctx->dest->port;
    *((സ്ഥിരം അക്ഷരം **)pres2) = ctx->dest->hostname;
    തിരികെ CURLE_OK;
  അവസ്ഥ CF_QUERY_NEED_FLUSH: {
    എങ്കിൽ(!Curl_bufq_is_empty(&ctx->outbufq) ||
       !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(data, cf, "needs flush");
      *pres1 = TRUE;
      തിരികെ CURLE_OK;
    }
    നിർത്തുക;
  }
  അവസ്ഥ CF_QUERY_ALPN_NEGOTIATED: {
    സ്ഥിരം അക്ഷരം **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ശൂന്യ;
    തിരികെ CURLE_OK;
  }
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

സ്ഥിതം CURLcode cf_h2_proxy_cntrl(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  CURLcode result = CURLE_OK;
  ഘടന cf_call_data save;

  (ശൂന്യം)arg1;
  (ശൂന്യം)arg2;

  തിരഞ്ഞെടുപ്പ്(event) {
  അവസ്ഥ CF_CTRL_FLUSH:
    CF_DATA_SAVE(save, cf, data);
    result = cf_h2_proxy_flush(cf, data);
    CF_DATA_RESTORE(cf, save);
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ result;
}

ഘടന Curl_cftype Curl_cft_h2_proxy = {
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

CURLcode Curl_cf_h2_proxy_തിരുകുക_after(ഘടന Curl_cfilter *cf,
                                       ഘടന Curl_easy *data,
                                       ഘടന Curl_peer *peer,
                                       ഘടന Curl_peer *dest,
                                       bool udp_tunnel)
{
  ഘടന Curl_cfilter *cf_h2_proxy = ശൂന്യ;
  ഘടന cf_h2_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (ശൂന്യം)data;
  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx)
    പോകുക out;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  result = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  എങ്കിൽ(result)
    പോകുക out;
  ctx = ശൂന്യ;
  Curl_conn_cf_തിരുകുക_after(cf, cf_h2_proxy);

out:
  cf_h2_proxy_ctx_free(ctx);
  തിരികെ result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
