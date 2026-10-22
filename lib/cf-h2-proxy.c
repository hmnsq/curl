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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_NGHTTP2)

#ଅନ୍ତର୍ଭାଵ <nghttp2/nghttp2.h>

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "bufq.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "dynhds.h"
#ଅନ୍ତର୍ଭାଵ "http2.h"
#ଅନ୍ତର୍ଭାଵ "http_proxy.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "ngh2_shim.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "cf-h2-proxy.h"

#define PROXY_H2_CHUNK_SIZE  (16 * 1024)

#define PROXY_HTTP2_HUGE_WINDOW_SIZE (100 * 1024 * 1024)
#define H2_TUNNEL_WINDOW_SIZE        (10 * 1024 * 1024)

#define PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define PROXY_H2_NW_SEND_CHUNKS   1

#define H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_SIZE)


ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

ସଂରଚନା tunnel_stream {
  ସଂରଚନା http_resp *resp;
  ସଂରଚନା bufq recvbuf;
  ସଂରଚନା bufq sendbuf;
  ଅକ୍ଷର *authority;
  int32_t stream_id;
  uint32_t error;
  h2_tunnel_state state;
  BIT(has_final_response);
  BIT(closed);
  BIT(reset);
};

ସ୍ଥିର CURLcode tunnel_stream_init(ସଂରଚନା tunnel_stream *ts,
                                   ସଂରଚନା Curl_peer *dest)
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
  ଯଦି(!ts->authority)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ tunnel_stream_reset(ସଂରଚନା tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = ଖାଲି;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->stream_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

ସ୍ଥିର ଶୂନ୍ୟ tunnel_stream_clear(ସଂରଚନା tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, ଆକାର(*ts));
  ts->state = H2_TUNNEL_INIT;
}

ସ୍ଥିର ଶୂନ୍ୟ h2_tunnel_go_state(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା tunnel_stream *ts,
                               h2_tunnel_state new_state,
                               ସଂରଚନା Curl_easy *data,
                               bool udp_tunnel)
{
  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)udp_tunnel;

  ଯଦି(ts->state == new_state)
    ଫେରନ୍ତୁ;
  /* leaving this one */
  ବାଛ(ts->state) {
  ସ୍ଥିତି H2_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  /* entering this one */
  ବାଛ(new_state) {
  ସ୍ଥିତି H2_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'init'", ts->stream_id);
    tunnel_stream_reset(ts);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H2_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'connect'", ts->stream_id);
    ts->state = H2_TUNNEL_CONNECT;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'response'", ts->stream_id);
    ts->state = H2_TUNNEL_RESPONSE;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'established'",
                ts->stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  ସ୍ଥିତି H2_TUNNEL_FAILED:
    ଯଦି(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(data, cf, "[%d] new tunnel state 'failed'", ts->stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    ଭାଙ୍ଗ;
  }
}

ସଂରଚନା cf_h2_proxy_ctx {
  ସଂରଚନା curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ସଂରଚନା cf_call_data call_data;

  ସଂରଚନା bufq inbufq;  /* network receive buffer */
  ସଂରଚନା bufq outbufq; /* network send buffer */

  ସଂରଚନା Curl_peer *peer; /* proxy we talk to */
  ସଂରଚନା Curl_peer *dest; /* where to tunnel to */
  ସଂରଚନା tunnel_stream tunnel; /* our tunnel CONNECT stream */
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
#define CF_CTX_CALL_DATA(cf) ((ସଂରଚନା cf_h2_proxy_ctx *)(cf)->ctx)->call_data

ସ୍ଥିର ଶୂନ୍ୟ cf_h2_proxy_ctx_clear(ସଂରଚନା cf_h2_proxy_ctx *ctx)
{
  ସଂରଚନା cf_call_data save = ctx->call_data;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_stream_clear(&ctx->tunnel);
  memset(ctx, 0, ଆକାର(*ctx));
  ctx->call_data = save;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_h2_proxy_ctx_free(ସଂରଚନା cf_h2_proxy_ctx *ctx)
{
  ଯଦି(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ drain_tunnel(ସଂରଚନା Curl_cfilter *cf,
                         ସଂରଚନା Curl_easy *data,
                         ସଂରଚନା tunnel_stream *tunnel)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  (ଶୂନ୍ୟ)cf;
  ଯଦି(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_empty(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(data);
}

ସ୍ଥିର CURLcode proxy_h2_nw_out_writer(ଶୂନ୍ୟ *writer_ctx,
                                       ଅଚଳ uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  ସଂରଚନା Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  ଯଦି(cf) {
    ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
    CURLcode result;
    result = Curl_conn_cf_send(cf->next, data, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(data, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten);
    ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ CURLE_FAILED_INIT;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା proxy_h2_client_new(ସଂରଚନା Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ଖାଲି, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = nghttp2_option_new(&o);
  ଯଦି(rc)
    ଫେରନ୍ତୁ rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#ଯଦି NGHTTP2_VERSION_ଅଙ୍କ >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା proxy_h2_should_close_session(ସଂରଚନା cf_h2_proxy_ctx *ctx)
{
  ଫେରନ୍ତୁ !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

ସ୍ଥିର CURLcode proxy_h2_nw_out_flush(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  ଯଦି(Curl_bufq_is_empty(&ctx->outbufq))
    ଫେରନ୍ତୁ CURLE_OK;

  result = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_out_writer, cf,
                          &nwritten);
  ଯଦି(result) {
    ଯଦି(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    ଫେରନ୍ତୁ result;
  }
  CURL_TRC_CF(data, cf, "[0] nw send buffer flushed");
  ଫେରନ୍ତୁ Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ସ୍ଥିର CURLcode proxy_h2_process_pending_ନିବେଶ(ସଂରଚନା Curl_cfilter *cf,
                                               ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  ଅଚଳ unsigned ଅକ୍ଷର *buf;
  size_t blen, nread;
  ssize_t rv;

  ଯେତେବେଳେ(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ଅଚଳ uint8_t *)buf, blen);
    CURL_TRC_CF(data, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    ଯଦି(!curlx_sztouz(rv, &nread)) {
      failf(data,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rv));
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;
    }
    ଅନ୍ୟଥା ଯଦି(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      ଭାଙ୍ଗ;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    ଯଦି(Curl_bufq_is_empty(&ctx->inbufq)) {
      CURL_TRC_CF(data, cf, "[0] all data in connection buffer processed");
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା {
      CURL_TRC_CF(data, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode proxy_h2_progress_ingress(ସଂରଚନା Curl_cfilter *cf,
                                          ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  ଯଦି(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = proxy_h2_process_pending_ନିବେଶ(cf, data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  ଯେତେବେଳେ(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_empty(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_full(&ctx->tunnel.recvbuf)) {

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, nread);
    ଯଦି(result) {
      ଯଦି(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 proxy data");
        ଫେରନ୍ତୁ result;
      }
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା ଯଦି(nread == 0) {
      CURL_TRC_CF(data, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      ଭାଙ୍ଗ;
    }

    result = proxy_h2_process_pending_ନିବେଶ(cf, data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode proxy_h2_progress_egress(ସଂରଚନା Curl_cfilter *cf,
                                         ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv = 0;

  ctx->nw_out_blocked = 0;
  ଯେତେବେଳେ(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  ଯଦି(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  }
  ଫେରନ୍ତୁ proxy_h2_nw_out_flush(cf, data);
}

ସ୍ଥିର ssize_t on_session_send(nghttp2_session *h2,
                               ଅଚଳ uint8_t *buf, size_t blen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                               ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)h2;
  (ଶୂନ୍ୟ)flags;
  DEBUGASSERT(data);

  result = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_out_writer, cf, &nwritten);
  ଯଦି(result) {
    ଯଦି(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      ଫେରନ୍ତୁ NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ଯଦି(!nwritten)
    ଫେରନ୍ତୁ NGHTTP2_ERR_WOULDBLOCK;

  ଫେରନ୍ତୁ (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#ifdef CURLVERBOSE
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା proxy_h2_on_frame_send(nghttp2_session *session,
                                  ଅଚଳ nghttp2_frame *frame,
                                  ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);

  (ଶୂନ୍ୟ)session;
  DEBUGASSERT(data);
  ଯଦି(Curl_trc_cf_is_verbose(cf, data)) {
    ଅକ୍ଷର buffer[256];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
    len = Curl_nghttp2_fr_print(frame, buffer, ଆକାର(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  ଫେରନ୍ତୁ 0;
}
#endif /* CURLVERBOSE */

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା proxy_h2_on_frame_recv(nghttp2_session *session,
                                  ଅଚଳ nghttp2_frame *frame,
                                  ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;

  (ଶୂନ୍ୟ)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  ଯଦି(Curl_trc_cf_is_verbose(cf, data)) {
    ଅକ୍ଷର buffer[256];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
    len = Curl_nghttp2_fr_print(frame, buffer, ଆକାର(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  ଯଦି(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    ବାଛ(frame->hd.type) {
    ସ୍ଥିତି NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      ଯଦି(CURL_REQ_WANT_SEND(data)) {
        drain_tunnel(cf, data, &ctx->tunnel);
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      ଯଦି(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              frame->goaway.error_code, ctx->remote_max_sid);
      }
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] rcvd FRAME not for tunnel", stream_id);
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ବାଛ(frame->hd.type) {
  ସ୍ଥିତି NGHTTP2_HEADERS:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    ଯଦି(!ctx->tunnel.resp)
      ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(data, cf, "[%d] got http status: %d",
                stream_id, ctx->tunnel.resp->status);
    ଯଦି(!ctx->tunnel.has_final_response && ctx->tunnel.resp->status / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି NGHTTP2_WINDOW_UPDATE:
    ଯଦି(CURL_REQ_WANT_SEND(data)) {
      drain_tunnel(cf, data, &ctx->tunnel);
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି NGHTTP2_RST_STREAM:
    ଯଦି(frame->rst_stream.error_code)
      ctx->tunnel.reset = TRUE;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା proxy_h2_on_header(nghttp2_session *session,
                              ଅଚଳ nghttp2_frame *frame,
                              ଅଚଳ uint8_t *name, size_t namelen,
                              ଅଚଳ uint8_t *ମୂଲ୍ୟ, size_t valuelen,
                              uint8_t flags,
                              ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (ଶୂନ୍ୟ)flags;
  (ଶୂନ୍ୟ)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  ଯଦି(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", stream_id,
                (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)namelen, name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)valuelen, ମୂଲ୍ୟ);
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ଯଦି(frame->hd.type == NGHTTP2_ଠେଲ_PROMISE)
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;

  ଯଦି(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    int32_t http_status;
    ସଂରଚନା http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    result = Curl_http_decode_status(&http_status,
                                    (ଅଚଳ ଅକ୍ଷର *)ମୂଲ୍ୟ, valuelen);
    ଯଦି(result)
      ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
    result = Curl_http_resp_make(&resp, http_status, ଖାଲି);
    ଯଦି(result)
      ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
    ଯଦି(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream_id, ctx->tunnel.resp->status);
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(!ctx->tunnel.resp)
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_dynhds_add(&ctx->tunnel.resp->headers,
                           (ଅଚଳ ଅକ୍ଷର *)name, namelen,
                           (ଅଚଳ ଅକ୍ଷର *)ମୂଲ୍ୟ, valuelen);
  ଯଦି(result)
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream_id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)namelen, name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)valuelen, ମୂଲ୍ୟ);

  ଫେରନ୍ତୁ 0; /* 0 is successful */
}

ସ୍ଥିର ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint8_t *buf, size_t length,
                                    uint32_t *data_flags,
                                    nghttp2_data_source *source,
                                    ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  ସଂରଚନା tunnel_stream *ts;
  CURLcode result;
  size_t nread;

  (ଶୂନ୍ୟ)session;
  (ଶୂନ୍ୟ)source;
  (ଶୂନ୍ୟ)ctx;

  ଯଦି(!stream_id)
    ଫେରନ୍ତୁ NGHTTP2_ERR_INVALID_ARGUMENT;

  ts = Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  ଯଦି(!ts)
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  result = Curl_bufq_read(&ts->sendbuf, buf, length, &nread);
  ଯଦି(result) {
    ଯଦି(result != CURLE_AGAIN)
      ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
    ଫେରନ୍ତୁ NGHTTP2_ERR_DEFERRED;
  }
  ଯଦି(ts->closed && Curl_bufq_is_empty(&ts->sendbuf))
    *data_flags = NGHTTP2_DATA_FLAG_EOF;

  CURL_TRC_CF(data, cf, "[%d] tunnel_send_callback -> %zu",
              ts->stream_id, nread);
  ଫେରନ୍ତୁ (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t stream_id,
                                ଅଚଳ uint8_t *mem, size_t len, ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  (ଶୂନ୍ୟ)flags;
  (ଶୂନ୍ୟ)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  ଯଦି(stream_id != ctx->tunnel.stream_id)
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  ଯଦି(result) {
    ଯଦି(result != CURLE_AGAIN)
      ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
#ifdef DEBUGBUILD
    nwritten = 0;
#endif
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା proxy_h2_on_stream_close(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint32_t error_code, ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);

  (ଶୂନ୍ୟ)session;

  ଯଦି(stream_id != ctx->tunnel.stream_id)
    ଫେରନ୍ତୁ 0;

  CURL_TRC_CF(data, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_code;
  ଯଦି(error_code)
    ctx->tunnel.reset = TRUE;

  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  ସଂରଚନା Curl_cfilter *cf,
  ସଂରଚନା Curl_easy *data,
  ସଂରଚନା curl_ngh2 *ngh2,
  ସଂରଚନା httpreq *req,
  ଅଚଳ nghttp2_priority_spec *pri_spec,
  ଶୂନ୍ୟ *stream_user_data,
  nghttp2_data_source_read_callback read_callback,
  ଶୂନ୍ୟ *read_ctx)
{
  ସଂରଚନା dynhds h2_headers;
  nghttp2_nv *nva = ଖାଲି;
  int32_t stream_id = -1;
  size_t nheader;
  CURLcode result;

  (ଶୂନ୍ୟ)cf;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_ଗଣନା, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  ଯଦି(result)
    ଯାଅ out;

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  ଯଦି(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  ଯଦି(read_callback) {
    nghttp2_data_provider data_prd;

    data_prd.read_callback = read_callback;
    data_prd.source.ptr = read_ctx;
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &data_prd, stream_user_data);
  }
  ଅନ୍ୟଥା {
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, ଖାଲି, stream_user_data);
  }

  ଯଦି(stream_id < 0) {
    failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    ଯାଅ out;
  }
  result = CURLE_OK;

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  *pstream_id = stream_id;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode submit_CONNECT(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା tunnel_stream *ts)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  ସଂରଚନା httpreq *req = ଖାଲି;

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  ଯଦି(result)
    ଯାଅ out;
  result = Curl_creader_set_null(data);
  ଯଦି(result)
    ଯାଅ out;

  result = proxy_h2_submit(&ts->stream_id, cf, data, &ctx->ngh2, req,
                           ଖାଲି, ts, tunnel_send_callback, cf);
  ଯଦି(result) {
    CURL_TRC_CF(data, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->stream_id, nghttp2_strerror(ts->stream_id));
  }

out:
  ଯଦି(req)
    Curl_http_req_free(req);
  ଯଦି(result)
    failf(data, "Failed sending CONNECT to proxy");
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode inspect_response(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା tunnel_stream *ts)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)ctx->udp_tunnel, &res);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  ବାଛ(res) {
  ସ୍ଥିତି PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, data,
                       (bool)ctx->udp_tunnel);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
    result = CURLE_COULDNT_CONNECT;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, data,
                       (bool)ctx->udp_tunnel);
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode H2_CONNECT(ସଂରଚନା Curl_cfilter *cf,
                           ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା tunnel_stream *ts)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  ଯଦି(ctx->conn_closed) {
    failf(data, "proxy closed connection");
    ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
  }

  କର {
    ବାଛ(ts->state) {
    ସ୍ଥିତି H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = submit_CONNECT(cf, data, ts);
      ଯଦି(result)
        ଯାଅ out;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, data,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    ସ୍ଥିତି H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      result = proxy_h2_progress_ingress(cf, data);
      ଯଦି(!result)
        result = proxy_h2_progress_egress(cf, data);
      ଯଦି(result && result != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                           (bool)ctx->udp_tunnel);
        ଭାଙ୍ଗ;
      }

      ଯଦି(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, data,
                           (bool)ctx->udp_tunnel);
      }
      ଅନ୍ୟଥା {
        result = CURLE_OK;
        ଯାଅ out;
      }
      FALLTHROUGH();

    ସ୍ଥିତି H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = inspect_response(cf, data, ts);
      ଯଦି(result)
        ଯାଅ out;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି H2_TUNNEL_ESTABLISHED:
      ଫେରନ୍ତୁ CURLE_OK;

    ସ୍ଥିତି H2_TUNNEL_FAILED:
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }

  } ଯେତେବେଳେ(ts->state == H2_TUNNEL_INIT);

out:
  ଯଦି((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
  ଫେରନ୍ତୁ result;
}

/*
 * Initialize the cfilter context
 */
ସ୍ଥିର CURLcode cf_h2_proxy_ctx_init(ସଂରଚନା Curl_cfilter *cf,
                                     ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  nghttp2_session_callbacks *cbs = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, ଆକାର(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  ଯଦି(tunnel_stream_init(&ctx->tunnel, ctx->dest))
    ଯାଅ out;

  rc = nghttp2_session_callbacks_new(&cbs);
  ଯଦି(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    ଯାଅ out;
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
  ଯଦି(rc) {
    failf(data, "Could not initialize nghttp2");
    ଯାଅ out;
  }

  {
    nghttp2_settings_entry iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].ମୂଲ୍ୟ = Curl_multi_max_concurrent_streams(data->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
    iv[1].ମୂଲ୍ୟ = H2_TUNNEL_WINDOW_SIZE;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ଠେଲ;
    iv[2].ମୂଲ୍ୟ = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    ଯଦି(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      ଯାଅ out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_WINDOW_SIZE);
  ଯଦି(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    ଯାଅ out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;

out:
  ଯଦି(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(data, cf, "[0] init proxy ctx -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h2_proxy_connect(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    bool *done)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ସଂରଚନା cf_call_data save;
  ସଂରଚନା tunnel_stream *ts = &ctx->tunnel;

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  /* Connect the lower filters first */
  ଯଦି(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ଯଦି(result || !*done)
      ଫେରନ୍ତୁ result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  ଯଦି(!ctx->ngh2.s) {
    result = cf_h2_proxy_ctx_init(cf, data);
    ଯଦି(result)
      ଯାଅ out;
  }
  DEBUGASSERT(ts->authority);

  ଯଦି(Curl_timeleft_ms(data) < 0) {
    failf(data, "Proxy CONNECT aborted due to timeout");
    result = CURLE_OPERATION_TIMEDOUT;
    ଯାଅ out;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  result = H2_CONNECT(cf, data, ts);

out:
  *done = (result == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  ଯଦି(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_h2_proxy_destroy(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;

  (ଶୂନ୍ୟ)data;
  ଯଦି(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = ଖାଲି;
  }
}

ସ୍ଥିର CURLcode cf_h2_proxy_shutdown(ସଂରଚନା Curl_cfilter *cf,
                                     ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  ସଂରଚନା cf_call_data save;
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;

  ଯଦି(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  ଯଦି(!ctx->sent_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ଅଚଳ uint8_t *)"shutdown", ଆକାର("shutdown"));
    ଯଦି(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
    ctx->sent_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  ଯଦି(Curl_ngh2_session_want_write(&ctx->ngh2))
    result = proxy_h2_progress_egress(cf, data);
  ଯଦି(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = proxy_h2_progress_ingress(cf, data);

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool cf_h2_proxy_data_pending(ସଂରଚନା Curl_cfilter *cf,
                                     ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  ଯଦି((ctx && !Curl_bufq_is_empty(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    ଫେରନ୍ତୁ TRUE;
  ଫେରନ୍ତୁ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ସ୍ଥିର CURLcode cf_h2_proxy_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                           ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  ସଂରଚନା cf_call_data save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  ଯଦି(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  ଅନ୍ୟଥା
    Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  ଯଦି(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
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
                want_recv, want_send, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    CF_DATA_RESTORE(cf, save);
  }
  ଅନ୍ୟଥା ଯଦି(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CURL_TRC_CF(data, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    CF_DATA_RESTORE(cf, save);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode h2_handle_tunnel_close(ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data,
                                       size_t *pnread)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  ଯଦି(ctx->tunnel.error) {
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.stream_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    ଫେରନ୍ତୁ CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(data, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.stream_id);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode tunnel_recv(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                            ଅକ୍ଷର *buf, size_t len, size_t *pnread)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  *pnread = 0;
  ଯଦି(!Curl_bufq_is_empty(&ctx->tunnel.recvbuf))
    result = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  ଅନ୍ୟଥା {
    ଯଦି(ctx->tunnel.closed) {
      result = h2_handle_tunnel_close(cf, data, pnread);
    }
    ଅନ୍ୟଥା ଯଦି(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.stream_id)) {
      result = CURLE_RECV_ERROR;
    }
    ଅନ୍ୟଥା
      result = CURLE_AGAIN;
  }

  CURL_TRC_CF(data, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnread);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h2_proxy_recv(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ଅକ୍ଷର *buf, size_t len,
                                 size_t *pnread)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  ସଂରଚନା cf_call_data save;
  CURLcode result;

  *pnread = 0;
  CF_DATA_SAVE(save, cf, data);

  ଯଦି(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_RECV_ERROR;
    ଯାଅ out;
  }

  ଯଦି(Curl_bufq_is_empty(&ctx->tunnel.recvbuf)) {
    result = proxy_h2_progress_ingress(cf, data);
    ଯଦି(result)
      ଯାଅ out;
  }

  result = tunnel_recv(cf, data, buf, len, pnread);

  ଯଦି(!result) {
    CURL_TRC_CF(data, cf, "[%d] increase window by %zu",
                ctx->tunnel.stream_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.stream_id, *pnread);
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

out:
  ଯଦି((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h2_proxy_send(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  ସଂରଚନା cf_call_data save;
  CURLcode result;

  (ଶୂନ୍ୟ)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  ଯଦି(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_SEND_ERROR;
    ଯାଅ out;
  }

  ଯଦି(ctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    ଯାଅ out;
  }

  result = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(data, cf, "cf_send(), bufq_write %d, %zu", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result,
              *pnwritten);
  ଯଦି(result && (result != CURLE_AGAIN))
    ଯାଅ out;

  ଯଦି(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    ଯଦି(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_ingress(cf, data));
  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

  ଯଦି(!result && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    ଯଦି(ctx->tunnel.closed) {
      result = CURLE_SEND_ERROR;
    }
    ଅନ୍ୟଥା {
      CURL_TRC_CF(data, cf, "[0] send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  ଯଦି((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h2_proxy_flush(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  ସଂରଚନା cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  ଯଦି(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    ଯଦି(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
  }

  result = proxy_h2_progress_egress(cf, data);

out:
  CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool proxy_h2_connisalive(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 bool *ନିବେଶ_pending)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ନିବେଶ_pending = FALSE;
  ଯଦି(!cf->next || !cf->next->cft->is_alive(cf->next, data, ନିବେଶ_pending))
    ଫେରନ୍ତୁ FALSE;

  ଯଦି(*ନିବେଶ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *ନିବେଶ_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    ଯଦି(!result) {
      ଯଦି(proxy_h2_process_pending_ନିବେଶ(cf, data))
        /* immediate error, considered dead */
        alive = FALSE;
      ଅନ୍ୟଥା {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    ଅନ୍ୟଥା ଯଦି(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ଫେରନ୍ତୁ alive;
}

ସ୍ଥିର bool cf_h2_proxy_is_alive(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 bool *ନିବେଶ_pending)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  ସଂରଚନା cf_call_data save;

  *ନିବେଶ_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, data, ନିବେଶ_pending));
  CURL_TRC_CF(data, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *ନିବେଶ_pending);
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ alive;
}

ସ୍ଥିର CURLcode cf_h2_proxy_query(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2)
{
  ସଂରଚନା cf_h2_proxy_ctx *ctx = cf->ctx;

  ବାଛ(query) {
  ସ୍ଥିତି CF_QUERY_HOST_PORT:
    *pres1 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ctx->dest->port;
    *((ଅଚଳ ଅକ୍ଷର **)pres2) = ctx->dest->hostname;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CF_QUERY_NEED_FLUSH: {
    ଯଦି(!Curl_bufq_is_empty(&ctx->outbufq) ||
       !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(data, cf, "needs flush");
      *pres1 = TRUE;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଭାଙ୍ଗ;
  }
  ସ୍ଥିତି CF_QUERY_ALPN_NEGOTIATED: {
    ଅଚଳ ଅକ୍ଷର **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ଖାଲି;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ସ୍ଥିର CURLcode cf_h2_proxy_cntrl(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା cf_call_data save;

  (ଶୂନ୍ୟ)arg1;
  (ଶୂନ୍ୟ)arg2;

  ବାଛ(event) {
  ସ୍ଥିତି CF_CTRL_FLUSH:
    CF_DATA_SAVE(save, cf, data);
    result = cf_h2_proxy_flush(cf, data);
    CF_DATA_RESTORE(cf, save);
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

ସଂରଚନା Curl_cftype Curl_cft_h2_proxy = {
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

CURLcode Curl_cf_h2_proxy_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା Curl_peer *peer,
                                       ସଂରଚନା Curl_peer *dest,
                                       bool udp_tunnel)
{
  ସଂରଚନା Curl_cfilter *cf_h2_proxy = ଖାଲି;
  ସଂରଚନା cf_h2_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (ଶୂନ୍ୟ)data;
  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx)
    ଯାଅ out;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  result = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  ଯଦି(result)
    ଯାଅ out;
  ctx = ଖାଲି;
  Curl_conn_cf_ଭର୍ତ୍ତି_after(cf, cf_h2_proxy);

out:
  cf_h2_proxy_ctx_free(ctx);
  ଫେରନ୍ତୁ result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
