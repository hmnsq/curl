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
  defined(USE_NGHTTP2)

#અન્તર્ભાવ <nghttp2/nghttp2.h>

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "bufq.h"
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "dynhds.h"
#અન્તર્ભાવ "http2.h"
#અન્તર્ભાવ "http_proxy.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "ngh2_shim.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "cf-h2-proxy.h"

#define PROXY_H2_CHUNK_SIZE  (16 * 1024)

#define PROXY_HTTP2_HUGE_WINDOW_SIZE (100 * 1024 * 1024)
#define H2_TUNNEL_WINDOW_SIZE        (10 * 1024 * 1024)

#define PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define PROXY_H2_NW_SEND_CHUNKS   1

#define H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_SIZE)


પ્રકારવ્યાખ્યા ગણના {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

સંરચના tunnel_stream {
  સંરચના http_resp *resp;
  સંરચના bufq recvbuf;
  સંરચના bufq sendbuf;
  અક્ષર *authority;
  int32_t stream_id;
  uint32_t error;
  h2_tunnel_state state;
  BIT(has_final_response);
  BIT(closed);
  BIT(reset);
};

સ્થિર CURLcode tunnel_stream_init(સંરચના tunnel_stream *ts,
                                   સંરચના Curl_peer *dest)
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
  જો(!ts->authority)
    પાછા CURLE_OUT_OF_MEMORY;

  પાછા CURLE_OK;
}

સ્થિર શૂન્ય tunnel_stream_reset(સંરચના tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = ખાલી;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->stream_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

સ્થિર શૂન્ય tunnel_stream_clear(સંરચના tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, કદ(*ts));
  ts->state = H2_TUNNEL_INIT;
}

સ્થિર શૂન્ય h2_tunnel_go_state(સંરચના Curl_cfilter *cf,
                               સંરચના tunnel_stream *ts,
                               h2_tunnel_state new_state,
                               સંરચના Curl_easy *data,
                               bool udp_tunnel)
{
  (શૂન્ય)cf;
  (શૂન્ય)udp_tunnel;

  જો(ts->state == new_state)
    પાછા;
  /* leaving this one */
  પસંદગી(ts->state) {
  સ્થિતિ H2_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  /* entering this one */
  પસંદગી(new_state) {
  સ્થિતિ H2_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'init'", ts->stream_id);
    tunnel_stream_reset(ts);
    તોડો;

  સ્થિતિ H2_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'connect'", ts->stream_id);
    ts->state = H2_TUNNEL_CONNECT;
    તોડો;

  સ્થિતિ H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'response'", ts->stream_id);
    ts->state = H2_TUNNEL_RESPONSE;
    તોડો;

  સ્થિતિ H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'established'",
                ts->stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  સ્થિતિ H2_TUNNEL_FAILED:
    જો(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(data, cf, "[%d] new tunnel state 'failed'", ts->stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    તોડો;
  }
}

સંરચના cf_h2_proxy_ctx {
  સંરચના curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  સંરચના cf_call_data call_data;

  સંરચના bufq inbufq;  /* network receive buffer */
  સંરચના bufq outbufq; /* network send buffer */

  સંરચના Curl_peer *peer; /* proxy we talk to */
  સંરચના Curl_peer *dest; /* where to tunnel to */
  સંરચના tunnel_stream tunnel; /* our tunnel CONNECT stream */
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
#define CF_CTX_CALL_DATA(cf) ((સંરચના cf_h2_proxy_ctx *)(cf)->ctx)->call_data

સ્થિર શૂન્ય cf_h2_proxy_ctx_clear(સંરચના cf_h2_proxy_ctx *ctx)
{
  સંરચના cf_call_data save = ctx->call_data;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_stream_clear(&ctx->tunnel);
  memset(ctx, 0, કદ(*ctx));
  ctx->call_data = save;
}

સ્થિર શૂન્ય cf_h2_proxy_ctx_free(સંરચના cf_h2_proxy_ctx *ctx)
{
  જો(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

સ્થિર શૂન્ય drain_tunnel(સંરચના Curl_cfilter *cf,
                         સંરચના Curl_easy *data,
                         સંરચના tunnel_stream *tunnel)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  (શૂન્ય)cf;
  જો(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_empty(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(data);
}

સ્થિર CURLcode proxy_h2_nw_out_writer(શૂન્ય *writer_ctx,
                                       અચલ uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  સંરચના Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  જો(cf) {
    સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
    CURLcode result;
    result = Curl_conn_cf_send(cf->next, data, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(data, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (પૂર્ણાંક)result, *pnwritten);
    પાછા result;
  }
  પાછા CURLE_FAILED_INIT;
}

સ્થિર પૂર્ણાંક proxy_h2_client_new(સંરચના Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ખાલી, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  પૂર્ણાંક rc = nghttp2_option_new(&o);
  જો(rc)
    પાછા rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#જો NGHTTP2_VERSION_અંક >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  પાછા rc;
}

સ્થિર પૂર્ણાંક proxy_h2_should_close_session(સંરચના cf_h2_proxy_ctx *ctx)
{
  પાછા !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

સ્થિર CURLcode proxy_h2_nw_out_flush(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  જો(Curl_bufq_is_empty(&ctx->outbufq))
    પાછા CURLE_OK;

  result = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_out_writer, cf,
                          &nwritten);
  જો(result) {
    જો(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    પાછા result;
  }
  CURL_TRC_CF(data, cf, "[0] nw send buffer flushed");
  પાછા Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
સ્થિર CURLcode proxy_h2_process_pending_આગત(સંરચના Curl_cfilter *cf,
                                               સંરચના Curl_easy *data)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  અચલ unsigned અક્ષર *buf;
  size_t blen, nread;
  ssize_t rv;

  જ્યારે(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (અચલ uint8_t *)buf, blen);
    CURL_TRC_CF(data, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    જો(!curlx_sztouz(rv, &nread)) {
      failf(data,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((પૂર્ણાંક)rv));
      પાછા CURLE_RECV_ERROR;
    }
    નહીંતર જો(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      તોડો;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    જો(Curl_bufq_is_empty(&ctx->inbufq)) {
      CURL_TRC_CF(data, cf, "[0] all data in connection buffer processed");
      તોડો;
    }
    નહીંતર {
      CURL_TRC_CF(data, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode proxy_h2_progress_ingress(સંરચના Curl_cfilter *cf,
                                          સંરચના Curl_easy *data)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  જો(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = proxy_h2_process_pending_આગત(cf, data);
    જો(result)
      પાછા result;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  જ્યારે(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_empty(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_full(&ctx->tunnel.recvbuf)) {

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (પૂર્ણાંક)result, nread);
    જો(result) {
      જો(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 proxy data");
        પાછા result;
      }
      તોડો;
    }
    નહીંતર જો(nread == 0) {
      CURL_TRC_CF(data, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      તોડો;
    }

    result = proxy_h2_process_pending_આગત(cf, data);
    જો(result)
      પાછા result;
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode proxy_h2_progress_egress(સંરચના Curl_cfilter *cf,
                                         સંરચના Curl_easy *data)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  પૂર્ણાંક rv = 0;

  ctx->nw_out_blocked = 0;
  જ્યારે(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  જો(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    પાછા CURLE_SEND_ERROR;
  }
  પાછા proxy_h2_nw_out_flush(cf, data);
}

સ્થિર ssize_t on_session_send(nghttp2_session *h2,
                               અચલ uint8_t *buf, size_t blen, પૂર્ણાંક flags,
                               શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (શૂન્ય)h2;
  (શૂન્ય)flags;
  DEBUGASSERT(data);

  result = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_out_writer, cf, &nwritten);
  જો(result) {
    જો(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      પાછા NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  જો(!nwritten)
    પાછા NGHTTP2_ERR_WOULDBLOCK;

  પાછા (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#ifdef CURLVERBOSE
સ્થિર પૂર્ણાંક proxy_h2_on_frame_send(nghttp2_session *session,
                                  અચલ nghttp2_frame *frame,
                                  શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);

  (શૂન્ય)session;
  DEBUGASSERT(data);
  જો(Curl_trc_cf_is_verbose(cf, data)) {
    અક્ષર buffer[256];
    પૂર્ણાંક len;
    len = Curl_nghttp2_fr_print(frame, buffer, કદ(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  પાછા 0;
}
#endif /* CURLVERBOSE */

સ્થિર પૂર્ણાંક proxy_h2_on_frame_recv(nghttp2_session *session,
                                  અચલ nghttp2_frame *frame,
                                  શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;

  (શૂન્ય)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  જો(Curl_trc_cf_is_verbose(cf, data)) {
    અક્ષર buffer[256];
    પૂર્ણાંક len;
    len = Curl_nghttp2_fr_print(frame, buffer, કદ(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  જો(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    પસંદગી(frame->hd.type) {
    સ્થિતિ NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      જો(CURL_REQ_WANT_SEND(data)) {
        drain_tunnel(cf, data, &ctx->tunnel);
      }
      તોડો;
    સ્થિતિ NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      જો(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              frame->goaway.error_code, ctx->remote_max_sid);
      }
      તોડો;
    મૂળભૂત:
      તોડો;
    }
    પાછા 0;
  }

  જો(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] rcvd FRAME not for tunnel", stream_id);
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  પસંદગી(frame->hd.type) {
  સ્થિતિ NGHTTP2_HEADERS:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    જો(!ctx->tunnel.resp)
      પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(data, cf, "[%d] got http status: %d",
                stream_id, ctx->tunnel.resp->status);
    જો(!ctx->tunnel.has_final_response && ctx->tunnel.resp->status / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    તોડો;
  સ્થિતિ NGHTTP2_WINDOW_UPDATE:
    જો(CURL_REQ_WANT_SEND(data)) {
      drain_tunnel(cf, data, &ctx->tunnel);
    }
    તોડો;
  સ્થિતિ NGHTTP2_RST_STREAM:
    જો(frame->rst_stream.error_code)
      ctx->tunnel.reset = TRUE;
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  પાછા 0;
}

સ્થિર પૂર્ણાંક proxy_h2_on_header(nghttp2_session *session,
                              અચલ nghttp2_frame *frame,
                              અચલ uint8_t *name, size_t namelen,
                              અચલ uint8_t *કિંમત, size_t valuelen,
                              uint8_t flags,
                              શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (શૂન્ય)flags;
  (શૂન્ય)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  જો(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", stream_id,
                (પૂર્ણાંક)namelen, name, (પૂર્ણાંક)valuelen, કિંમત);
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  જો(frame->hd.type == NGHTTP2_ધકેલો_PROMISE)
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;

  જો(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    પાછા 0;
  }

  જો(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    int32_t http_status;
    સંરચના http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    result = Curl_http_decode_status(&http_status,
                                    (અચલ અક્ષર *)કિંમત, valuelen);
    જો(result)
      પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
    result = Curl_http_resp_make(&resp, http_status, ખાલી);
    જો(result)
      પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
    જો(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream_id, ctx->tunnel.resp->status);
    પાછા 0;
  }

  જો(!ctx->tunnel.resp)
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_dynhds_add(&ctx->tunnel.resp->headers,
                           (અચલ અક્ષર *)name, namelen,
                           (અચલ અક્ષર *)કિંમત, valuelen);
  જો(result)
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream_id, (પૂર્ણાંક)namelen, name, (પૂર્ણાંક)valuelen, કિંમત);

  પાછા 0; /* 0 is successful */
}

સ્થિર ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint8_t *buf, size_t length,
                                    uint32_t *data_flags,
                                    nghttp2_data_source *source,
                                    શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  સંરચના tunnel_stream *ts;
  CURLcode result;
  size_t nread;

  (શૂન્ય)session;
  (શૂન્ય)source;
  (શૂન્ય)ctx;

  જો(!stream_id)
    પાછા NGHTTP2_ERR_INVALID_ARGUMENT;

  ts = Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  જો(!ts)
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  result = Curl_bufq_read(&ts->sendbuf, buf, length, &nread);
  જો(result) {
    જો(result != CURLE_AGAIN)
      પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
    પાછા NGHTTP2_ERR_DEFERRED;
  }
  જો(ts->closed && Curl_bufq_is_empty(&ts->sendbuf))
    *data_flags = NGHTTP2_DATA_FLAG_EOF;

  CURL_TRC_CF(data, cf, "[%d] tunnel_send_callback -> %zu",
              ts->stream_id, nread);
  પાછા (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

સ્થિર પૂર્ણાંક tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t stream_id,
                                અચલ uint8_t *mem, size_t len, શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  (શૂન્ય)flags;
  (શૂન્ય)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  જો(stream_id != ctx->tunnel.stream_id)
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  જો(result) {
    જો(result != CURLE_AGAIN)
      પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
#ifdef DEBUGBUILD
    nwritten = 0;
#endif
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  પાછા 0;
}

સ્થિર પૂર્ણાંક proxy_h2_on_stream_close(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint32_t error_code, શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);

  (શૂન્ય)session;

  જો(stream_id != ctx->tunnel.stream_id)
    પાછા 0;

  CURL_TRC_CF(data, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_code;
  જો(error_code)
    ctx->tunnel.reset = TRUE;

  પાછા 0;
}

સ્થિર CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  સંરચના Curl_cfilter *cf,
  સંરચના Curl_easy *data,
  સંરચના curl_ngh2 *ngh2,
  સંરચના httpreq *req,
  અચલ nghttp2_priority_spec *pri_spec,
  શૂન્ય *stream_user_data,
  nghttp2_data_source_read_callback read_callback,
  શૂન્ય *read_ctx)
{
  સંરચના dynhds h2_headers;
  nghttp2_nv *nva = ખાલી;
  int32_t stream_id = -1;
  size_t nheader;
  CURLcode result;

  (શૂન્ય)cf;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_ગણતરી, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  જો(result)
    જાઓ out;

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  જો(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  જો(read_callback) {
    nghttp2_data_provider data_prd;

    data_prd.read_callback = read_callback;
    data_prd.source.ptr = read_ctx;
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &data_prd, stream_user_data);
  }
  નહીંતર {
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, ખાલી, stream_user_data);
  }

  જો(stream_id < 0) {
    failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    જાઓ out;
  }
  result = CURLE_OK;

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  *pstream_id = stream_id;
  પાછા result;
}

સ્થિર CURLcode submit_CONNECT(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               સંરચના tunnel_stream *ts)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  સંરચના httpreq *req = ખાલી;

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  જો(result)
    જાઓ out;
  result = Curl_creader_set_null(data);
  જો(result)
    જાઓ out;

  result = proxy_h2_submit(&ts->stream_id, cf, data, &ctx->ngh2, req,
                           ખાલી, ts, tunnel_send_callback, cf);
  જો(result) {
    CURL_TRC_CF(data, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->stream_id, nghttp2_strerror(ts->stream_id));
  }

out:
  જો(req)
    Curl_http_req_free(req);
  જો(result)
    failf(data, "Failed sending CONNECT to proxy");
  પાછા result;
}

સ્થિર CURLcode inspect_response(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના tunnel_stream *ts)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)ctx->udp_tunnel, &res);
  જો(result)
    પાછા result;
  પસંદગી(res) {
  સ્થિતિ PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, data,
                       (bool)ctx->udp_tunnel);
    તોડો;
  સ્થિતિ PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
    result = CURLE_COULDNT_CONNECT;
    તોડો;
  સ્થિતિ PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, data,
                       (bool)ctx->udp_tunnel);
    તોડો;
  }
  પાછા result;
}

સ્થિર CURLcode H2_CONNECT(સંરચના Curl_cfilter *cf,
                           સંરચના Curl_easy *data,
                           સંરચના tunnel_stream *ts)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  જો(ctx->conn_closed) {
    failf(data, "proxy closed connection");
    પાછા CURLE_COULDNT_CONNECT;
  }

  કરો {
    પસંદગી(ts->state) {
    સ્થિતિ H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = submit_CONNECT(cf, data, ts);
      જો(result)
        જાઓ out;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, data,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    સ્થિતિ H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      result = proxy_h2_progress_ingress(cf, data);
      જો(!result)
        result = proxy_h2_progress_egress(cf, data);
      જો(result && result != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                           (bool)ctx->udp_tunnel);
        તોડો;
      }

      જો(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, data,
                           (bool)ctx->udp_tunnel);
      }
      નહીંતર {
        result = CURLE_OK;
        જાઓ out;
      }
      FALLTHROUGH();

    સ્થિતિ H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = inspect_response(cf, data, ts);
      જો(result)
        જાઓ out;
      તોડો;

    સ્થિતિ H2_TUNNEL_ESTABLISHED:
      પાછા CURLE_OK;

    સ્થિતિ H2_TUNNEL_FAILED:
      પાછા CURLE_RECV_ERROR;

    મૂળભૂત:
      તોડો;
    }

  } જ્યારે(ts->state == H2_TUNNEL_INIT);

out:
  જો((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
  પાછા result;
}

/*
 * Initialize the cfilter context
 */
સ્થિર CURLcode cf_h2_proxy_ctx_init(સંરચના Curl_cfilter *cf,
                                     સંરચના Curl_easy *data)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  nghttp2_session_callbacks *cbs = ખાલી;
  પૂર્ણાંક rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, કદ(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  જો(tunnel_stream_init(&ctx->tunnel, ctx->dest))
    જાઓ out;

  rc = nghttp2_session_callbacks_new(&cbs);
  જો(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    જાઓ out;
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
  જો(rc) {
    failf(data, "Could not initialize nghttp2");
    જાઓ out;
  }

  {
    nghttp2_settings_entry iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].કિંમત = Curl_multi_max_concurrent_streams(data->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
    iv[1].કિંમત = H2_TUNNEL_WINDOW_SIZE;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ધકેલો;
    iv[2].કિંમત = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    જો(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      જાઓ out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_WINDOW_SIZE);
  જો(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    જાઓ out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;

out:
  જો(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(data, cf, "[0] init proxy ctx -> %d", (પૂર્ણાંક)result);
  પાછા result;
}

સ્થિર CURLcode cf_h2_proxy_connect(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    bool *done)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  સંરચના cf_call_data save;
  સંરચના tunnel_stream *ts = &ctx->tunnel;

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  /* Connect the lower filters first */
  જો(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    જો(result || !*done)
      પાછા result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  જો(!ctx->ngh2.s) {
    result = cf_h2_proxy_ctx_init(cf, data);
    જો(result)
      જાઓ out;
  }
  DEBUGASSERT(ts->authority);

  જો(Curl_timeleft_ms(data) < 0) {
    failf(data, "Proxy CONNECT aborted due to timeout");
    result = CURLE_OPERATION_TIMEDOUT;
    જાઓ out;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  result = H2_CONNECT(cf, data, ts);

out:
  *done = (result == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  જો(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર શૂન્ય cf_h2_proxy_destroy(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;

  (શૂન્ય)data;
  જો(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = ખાલી;
  }
}

સ્થિર CURLcode cf_h2_proxy_shutdown(સંરચના Curl_cfilter *cf,
                                     સંરચના Curl_easy *data, bool *done)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  સંરચના cf_call_data save;
  CURLcode result;
  પૂર્ણાંક rv;

  જો(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  જો(!ctx->sent_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (અચલ uint8_t *)"shutdown", કદ("shutdown"));
    જો(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
    ctx->sent_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  જો(Curl_ngh2_session_want_write(&ctx->ngh2))
    result = proxy_h2_progress_egress(cf, data);
  જો(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = proxy_h2_progress_ingress(cf, data);

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  પાછા result;
}

સ્થિર bool cf_h2_proxy_data_pending(સંરચના Curl_cfilter *cf,
                                     અચલ સંરચના Curl_easy *data)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  જો((ctx && !Curl_bufq_is_empty(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    પાછા TRUE;
  પાછા cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

સ્થિર CURLcode cf_h2_proxy_adjust_pollset(સંરચના Curl_cfilter *cf,
                                           સંરચના Curl_easy *data,
                                           સંરચના easy_pollset *ps)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  સંરચના cf_call_data save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  જો(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  નહીંતર
    Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  જો(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
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
                want_recv, want_send, (પૂર્ણાંક)result);
    CF_DATA_RESTORE(cf, save);
  }
  નહીંતર જો(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CURL_TRC_CF(data, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (પૂર્ણાંક)result);
    CF_DATA_RESTORE(cf, save);
  }
  પાછા result;
}

સ્થિર CURLcode h2_handle_tunnel_close(સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data,
                                       size_t *pnread)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  જો(ctx->tunnel.error) {
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.stream_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    પાછા CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(data, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.stream_id);
  પાછા CURLE_OK;
}

સ્થિર CURLcode tunnel_recv(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                            અક્ષર *buf, size_t len, size_t *pnread)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  *pnread = 0;
  જો(!Curl_bufq_is_empty(&ctx->tunnel.recvbuf))
    result = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  નહીંતર {
    જો(ctx->tunnel.closed) {
      result = h2_handle_tunnel_close(cf, data, pnread);
    }
    નહીંતર જો(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.stream_id)) {
      result = CURLE_RECV_ERROR;
    }
    નહીંતર
      result = CURLE_AGAIN;
  }

  CURL_TRC_CF(data, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (પૂર્ણાંક)result, *pnread);
  પાછા result;
}

સ્થિર CURLcode cf_h2_proxy_recv(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 અક્ષર *buf, size_t len,
                                 size_t *pnread)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  સંરચના cf_call_data save;
  CURLcode result;

  *pnread = 0;
  CF_DATA_SAVE(save, cf, data);

  જો(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_RECV_ERROR;
    જાઓ out;
  }

  જો(Curl_bufq_is_empty(&ctx->tunnel.recvbuf)) {
    result = proxy_h2_progress_ingress(cf, data);
    જો(result)
      જાઓ out;
  }

  result = tunnel_recv(cf, data, buf, len, pnread);

  જો(!result) {
    CURL_TRC_CF(data, cf, "[%d] increase window by %zu",
                ctx->tunnel.stream_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.stream_id, *pnread);
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

out:
  જો((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (પૂર્ણાંક)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર CURLcode cf_h2_proxy_send(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 અચલ uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  સંરચના cf_call_data save;
  CURLcode result;

  (શૂન્ય)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  જો(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_SEND_ERROR;
    જાઓ out;
  }

  જો(ctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    જાઓ out;
  }

  result = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(data, cf, "cf_send(), bufq_write %d, %zu", (પૂર્ણાંક)result,
              *pnwritten);
  જો(result && (result != CURLE_AGAIN))
    જાઓ out;

  જો(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    જો(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_ingress(cf, data));
  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

  જો(!result && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    જો(ctx->tunnel.closed) {
      result = CURLE_SEND_ERROR;
    }
    નહીંતર {
      CURL_TRC_CF(data, cf, "[0] send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  જો((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, len, (પૂર્ણાંક)result, *pnwritten,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર CURLcode cf_h2_proxy_flush(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  સંરચના cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  જો(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    જો(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
  }

  result = proxy_h2_progress_egress(cf, data);

out:
  CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, (પૂર્ણાંક)result,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર bool proxy_h2_connisalive(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 bool *આગત_pending)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *આગત_pending = FALSE;
  જો(!cf->next || !cf->next->cft->is_alive(cf->next, data, આગત_pending))
    પાછા FALSE;

  જો(*આગત_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *આગત_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    જો(!result) {
      જો(proxy_h2_process_pending_આગત(cf, data))
        /* immediate error, considered dead */
        alive = FALSE;
      નહીંતર {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    નહીંતર જો(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  પાછા alive;
}

સ્થિર bool cf_h2_proxy_is_alive(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 bool *આગત_pending)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  સંરચના cf_call_data save;

  *આગત_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, data, આગત_pending));
  CURL_TRC_CF(data, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *આગત_pending);
  CF_DATA_RESTORE(cf, save);
  પાછા alive;
}

સ્થિર CURLcode cf_h2_proxy_query(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  સંરચના cf_h2_proxy_ctx *ctx = cf->ctx;

  પસંદગી(query) {
  સ્થિતિ CF_QUERY_HOST_PORT:
    *pres1 = (પૂર્ણાંક)ctx->dest->port;
    *((અચલ અક્ષર **)pres2) = ctx->dest->hostname;
    પાછા CURLE_OK;
  સ્થિતિ CF_QUERY_NEED_FLUSH: {
    જો(!Curl_bufq_is_empty(&ctx->outbufq) ||
       !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(data, cf, "needs flush");
      *pres1 = TRUE;
      પાછા CURLE_OK;
    }
    તોડો;
  }
  સ્થિતિ CF_QUERY_ALPN_NEGOTIATED: {
    અચલ અક્ષર **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ખાલી;
    પાછા CURLE_OK;
  }
  મૂળભૂત:
    તોડો;
  }
  પાછા cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

સ્થિર CURLcode cf_h2_proxy_cntrl(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  CURLcode result = CURLE_OK;
  સંરચના cf_call_data save;

  (શૂન્ય)arg1;
  (શૂન્ય)arg2;

  પસંદગી(event) {
  સ્થિતિ CF_CTRL_FLUSH:
    CF_DATA_SAVE(save, cf, data);
    result = cf_h2_proxy_flush(cf, data);
    CF_DATA_RESTORE(cf, save);
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  પાછા result;
}

સંરચના Curl_cftype Curl_cft_h2_proxy = {
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

CURLcode Curl_cf_h2_proxy_ઉમેરો_after(સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data,
                                       સંરચના Curl_peer *peer,
                                       સંરચના Curl_peer *dest,
                                       bool udp_tunnel)
{
  સંરચના Curl_cfilter *cf_h2_proxy = ખાલી;
  સંરચના cf_h2_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (શૂન્ય)data;
  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx)
    જાઓ out;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  result = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  જો(result)
    જાઓ out;
  ctx = ખાલી;
  Curl_conn_cf_ઉમેરો_after(cf, cf_h2_proxy);

out:
  cf_h2_proxy_ctx_free(ctx);
  પાછા result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
