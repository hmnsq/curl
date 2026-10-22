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
  defined(USE_NGHTTP2)

#समावेश <nghttp2/nghttp2.h>

#समावेश "urldata.h"
#समावेश "url.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "curl_trc.h"
#समावेश "bufq.h"
#समावेश "curlx/dynbuf.h"
#समावेश "dynhds.h"
#समावेश "http2.h"
#समावेश "http_proxy.h"
#समावेश "multiif.h"
#समावेश "ngh2_shim.h"
#समावेश "sendf.h"
#समावेश "select.h"
#समावेश "cf-h2-proxy.h"

#घोषणा PROXY_H2_CHUNK_आकार  (16 * 1024)

#घोषणा PROXY_HTTP2_HUGE_WINDOW_आकार (100 * 1024 * 1024)
#घोषणा H2_TUNNEL_WINDOW_आकार        (10 * 1024 * 1024)

#घोषणा PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_WINDOW_आकार / PROXY_H2_CHUNK_आकार)
#घोषणा PROXY_H2_NW_SEND_CHUNKS   1

#घोषणा H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_WINDOW_आकार / PROXY_H2_CHUNK_आकार)
#घोषणा H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_आकार)


प्रकार क्रमागत {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

काष्ठा tunnel_stream {
  काष्ठा http_resp *resp;
  काष्ठा bufq recvbuf;
  काष्ठा bufq sendbuf;
  अक्षर *authority;
  int32_t stream_id;
  uint32_t error;
  h2_tunnel_state state;
  BIT(has_final_response);
  BIT(closed);
  BIT(reset);
};

अटल CURLcode tunnel_stream_init(काष्ठा tunnel_stream *ts,
                                   काष्ठा Curl_peer *dest)
{
  ts->state = H2_TUNNEL_INIT;
  ts->stream_id = -1;
  Curl_bufq_init2(&ts->recvbuf, PROXY_H2_CHUNK_आकार, H2_TUNNEL_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init(&ts->sendbuf, PROXY_H2_CHUNK_आकार, H2_TUNNEL_SEND_CHUNKS);

  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->port);
  अगर(!ts->authority)
    वापस CURLE_OUT_OF_MEMORY;

  वापस CURLE_OK;
}

अटल व्योम tunnel_stream_reset(काष्ठा tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = शून्य;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->stream_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

अटल व्योम tunnel_stream_clear(काष्ठा tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  स_रखो(ts, 0, माप(*ts));
  ts->state = H2_TUNNEL_INIT;
}

अटल व्योम h2_tunnel_go_state(काष्ठा Curl_cfilter *cf,
                               काष्ठा tunnel_stream *ts,
                               h2_tunnel_state new_state,
                               काष्ठा Curl_easy *तथ्य,
                               bool udp_tunnel)
{
  (व्योम)cf;
  (व्योम)udp_tunnel;

  अगर(ts->state == new_state)
    वापस;
  /* leaving this one */
  चयन(ts->state) {
  हाल H2_TUNNEL_CONNECT:
    तथ्य->req.ignorebody = FALSE;
    अवरोध;
  शेष:
    अवरोध;
  }
  /* entering this one */
  चयन(new_state) {
  हाल H2_TUNNEL_INIT:
    CURL_TRC_CF(तथ्य, cf, "[%d] new tunnel state 'init'", ts->stream_id);
    tunnel_stream_reset(ts);
    अवरोध;

  हाल H2_TUNNEL_CONNECT:
    CURL_TRC_CF(तथ्य, cf, "[%d] new tunnel state 'connect'", ts->stream_id);
    ts->state = H2_TUNNEL_CONNECT;
    अवरोध;

  हाल H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(तथ्य, cf, "[%d] new tunnel state 'response'", ts->stream_id);
    ts->state = H2_TUNNEL_RESPONSE;
    अवरोध;

  हाल H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(तथ्य, cf, "[%d] new tunnel state 'established'",
                ts->stream_id);
    infof(तथ्य, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    तथ्य->state.authproxy.done = TRUE;
    तथ्य->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  हाल H2_TUNNEL_FAILED:
    अगर(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(तथ्य, cf, "[%d] new tunnel state 'failed'", ts->stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(तथ्य->req.hd_proxy_auth);
    अवरोध;
  }
}

काष्ठा cf_h2_proxy_ctx {
  काष्ठा curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  काष्ठा cf_call_तथ्य call_तथ्य;

  काष्ठा bufq inbufq;  /* network receive buffer */
  काष्ठा bufq outbufq; /* network send buffer */

  काष्ठा Curl_peer *peer; /* proxy we talk to */
  काष्ठा Curl_peer *dest; /* where to tunnel to */
  काष्ठा tunnel_stream tunnel; /* our tunnel CONNECT stream */
  int32_t goaway_error;
  int32_t remote_max_sid;
  BIT(conn_closed);
  BIT(rcvd_goaway);
  BIT(sent_goaway);
  BIT(nw_out_blocked);
  BIT(udp_tunnel);
};

/* How to access `call_data` from a cf_h2 filter */
#अघोषित CF_CTX_CALL_तथ्य
#घोषणा CF_CTX_CALL_तथ्य(cf) ((काष्ठा cf_h2_proxy_ctx *)(cf)->ctx)->call_तथ्य

अटल व्योम cf_h2_proxy_ctx_clear(काष्ठा cf_h2_proxy_ctx *ctx)
{
  काष्ठा cf_call_तथ्य save = ctx->call_तथ्य;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_stream_clear(&ctx->tunnel);
  स_रखो(ctx, 0, माप(*ctx));
  ctx->call_तथ्य = save;
}

अटल व्योम cf_h2_proxy_ctx_free(काष्ठा cf_h2_proxy_ctx *ctx)
{
  अगर(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

अटल व्योम drain_tunnel(काष्ठा Curl_cfilter *cf,
                         काष्ठा Curl_easy *तथ्य,
                         काष्ठा tunnel_stream *tunnel)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  (व्योम)cf;
  अगर(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_empty(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(तथ्य);
}

अटल CURLcode proxy_h2_nw_out_writer(व्योम *writer_ctx,
                                       स्थिर uint8_t *buf, माप_प्रकार buflen,
                                       माप_प्रकार *pnwritten)
{
  काष्ठा Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  अगर(cf) {
    काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
    CURLcode परिणाम;
    परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(तथ्य, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (पूर्णांक)परिणाम, *pnwritten);
    वापस परिणाम;
  }
  वापस CURLE_FAILED_INIT;
}

अटल पूर्णांक proxy_h2_client_new(काष्ठा Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { शून्य, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  पूर्णांक rc = nghttp2_option_new(&o);
  अगर(rc)
    वापस rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#अगर NGHTTP2_VERSION_अंक >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#पूर्ण_अगर
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  वापस rc;
}

अटल पूर्णांक proxy_h2_should_close_session(काष्ठा cf_h2_proxy_ctx *ctx)
{
  वापस !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

अटल CURLcode proxy_h2_nw_out_flush(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  माप_प्रकार nwritten;
  CURLcode परिणाम;

  अगर(Curl_bufq_is_empty(&ctx->outbufq))
    वापस CURLE_OK;

  परिणाम = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_out_writer, cf,
                          &nwritten);
  अगर(परिणाम) {
    अगर(परिणाम == CURLE_AGAIN) {
      CURL_TRC_CF(तथ्य, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    वापस परिणाम;
  }
  CURL_TRC_CF(तथ्य, cf, "[0] nw send buffer flushed");
  वापस Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
अटल CURLcode proxy_h2_process_pending_निवेश(काष्ठा Curl_cfilter *cf,
                                               काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  स्थिर अचिन्हित अक्षर *buf;
  माप_प्रकार blen, nread;
  ssize_t rv;

  जबतक(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (स्थिर uint8_t *)buf, blen);
    CURL_TRC_CF(तथ्य, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    अगर(!curlx_sztouz(rv, &nread)) {
      failf(तथ्य,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((पूर्णांक)rv));
      वापस CURLE_RECV_ERROR;
    }
    अन्यथा अगर(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      अवरोध;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    अगर(Curl_bufq_is_empty(&ctx->inbufq)) {
      CURL_TRC_CF(तथ्य, cf, "[0] all data in connection buffer processed");
      अवरोध;
    }
    अन्यथा {
      CURL_TRC_CF(तथ्य, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  वापस CURLE_OK;
}

अटल CURLcode proxy_h2_progress_ingress(काष्ठा Curl_cfilter *cf,
                                          काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार nread;

  /* Process network input buffer first */
  अगर(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(तथ्य, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    परिणाम = proxy_h2_process_pending_निवेश(cf, तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  जबतक(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_empty(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_full(&ctx->tunnel.recvbuf)) {

    परिणाम = Curl_cf_recv_bufq(cf->अग्र, तथ्य, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(तथ्य, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (पूर्णांक)परिणाम, nread);
    अगर(परिणाम) {
      अगर(परिणाम != CURLE_AGAIN) {
        failf(तथ्य, "Failed receiving HTTP2 proxy data");
        वापस परिणाम;
      }
      अवरोध;
    }
    अन्यथा अगर(nread == 0) {
      CURL_TRC_CF(तथ्य, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      अवरोध;
    }

    परिणाम = proxy_h2_process_pending_निवेश(cf, तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  वापस CURLE_OK;
}

अटल CURLcode proxy_h2_progress_egress(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  पूर्णांक rv = 0;

  ctx->nw_out_blocked = 0;
  जबतक(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  अगर(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(तथ्य, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    वापस CURLE_SEND_ERROR;
  }
  वापस proxy_h2_nw_out_flush(cf, तथ्य);
}

अटल ssize_t on_session_send(nghttp2_session *h2,
                               स्थिर uint8_t *buf, माप_प्रकार blen, पूर्णांक flags,
                               व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  माप_प्रकार nwritten;
  CURLcode परिणाम = CURLE_OK;

  (व्योम)h2;
  (व्योम)flags;
  DEBUGASSERT(तथ्य);

  परिणाम = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_out_writer, cf, &nwritten);
  अगर(परिणाम) {
    अगर(परिणाम == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      वापस NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(तथ्य, "Failed sending HTTP2 data");
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  अगर(!nwritten)
    वापस NGHTTP2_ERR_WOULDBLOCK;

  वापस (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#अगर_घोषित CURLVERBOSE
अटल पूर्णांक proxy_h2_on_frame_send(nghttp2_session *session,
                                  स्थिर nghttp2_frame *frame,
                                  व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);

  (व्योम)session;
  DEBUGASSERT(तथ्य);
  अगर(Curl_trc_cf_is_verbose(cf, तथ्य)) {
    अक्षर बफर[256];
    पूर्णांक len;
    len = Curl_nghttp2_fr_print(frame, बफर, माप(बफर) - 1);
    बफर[len] = 0;
    CURL_TRC_CF(तथ्य, cf, "[%d] -> %s", frame->hd.stream_id, बफर);
  }
  वापस 0;
}
#पूर्ण_अगर /* CURLVERBOSE */

अटल पूर्णांक proxy_h2_on_frame_recv(nghttp2_session *session,
                                  स्थिर nghttp2_frame *frame,
                                  व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;

  (व्योम)session;
  DEBUGASSERT(तथ्य);
#अगर_घोषित CURLVERBOSE
  अगर(Curl_trc_cf_is_verbose(cf, तथ्य)) {
    अक्षर बफर[256];
    पूर्णांक len;
    len = Curl_nghttp2_fr_print(frame, बफर, माप(बफर) - 1);
    बफर[len] = 0;
    CURL_TRC_CF(तथ्य, cf, "[%d] <- %s", frame->hd.stream_id, बफर);
  }
#पूर्ण_अगर /* CURLVERBOSE */

  अगर(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(तथ्य);
    चयन(frame->hd.type) {
    हाल NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      अगर(CURL_REQ_WANT_SEND(तथ्य)) {
        drain_tunnel(cf, तथ्य, &ctx->tunnel);
      }
      अवरोध;
    हाल NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      अगर(तथ्य) {
        infof(तथ्य, "received GOAWAY, error=%u, last_stream=%d",
              frame->goaway.error_code, ctx->remote_max_sid);
      }
      अवरोध;
    शेष:
      अवरोध;
    }
    वापस 0;
  }

  अगर(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(तथ्य, cf, "[%d] rcvd FRAME not for tunnel", stream_id);
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  चयन(frame->hd.type) {
  हाल NGHTTP2_HEADERS:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    अगर(!ctx->tunnel.resp)
      वापस NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(तथ्य, cf, "[%d] got http status: %d",
                stream_id, ctx->tunnel.resp->status);
    अगर(!ctx->tunnel.has_final_response && ctx->tunnel.resp->status / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    अवरोध;
  हाल NGHTTP2_WINDOW_UPDATE:
    अगर(CURL_REQ_WANT_SEND(तथ्य)) {
      drain_tunnel(cf, तथ्य, &ctx->tunnel);
    }
    अवरोध;
  हाल NGHTTP2_RST_STREAM:
    अगर(frame->rst_stream.error_code)
      ctx->tunnel.reset = TRUE;
    अवरोध;
  शेष:
    अवरोध;
  }
  वापस 0;
}

अटल पूर्णांक proxy_h2_on_header(nghttp2_session *session,
                              स्थिर nghttp2_frame *frame,
                              स्थिर uint8_t *name, माप_प्रकार namelen,
                              स्थिर uint8_t *मूल्य, माप_प्रकार valuelen,
                              uint8_t flags,
                              व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;
  CURLcode परिणाम;

  (व्योम)flags;
  (व्योम)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  अगर(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(तथ्य, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", stream_id,
                (पूर्णांक)namelen, name, (पूर्णांक)valuelen, मूल्य);
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  अगर(frame->hd.type == NGHTTP2_धकेलो_PROMISE)
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;

  अगर(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    वापस 0;
  }

  अगर(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !स_भेद(HTTP_PSEUDO_STATUS, name, namelen)) {
    int32_t http_status;
    काष्ठा http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    परिणाम = Curl_http_decode_status(&http_status,
                                    (स्थिर अक्षर *)मूल्य, valuelen);
    अगर(परिणाम)
      वापस NGHTTP2_ERR_CALLBACK_FAILURE;
    परिणाम = Curl_http_resp_make(&resp, http_status, शून्य);
    अगर(परिणाम)
      वापस NGHTTP2_ERR_CALLBACK_FAILURE;
    अगर(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(तथ्य, cf, "[%d] status: HTTP/2 %03d",
                stream_id, ctx->tunnel.resp->status);
    वापस 0;
  }

  अगर(!ctx->tunnel.resp)
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;

  परिणाम = Curl_dynhds_add(&ctx->tunnel.resp->headers,
                           (स्थिर अक्षर *)name, namelen,
                           (स्थिर अक्षर *)मूल्य, valuelen);
  अगर(परिणाम)
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(तथ्य, cf, "[%d] header: %.*s: %.*s",
              stream_id, (पूर्णांक)namelen, name, (पूर्णांक)valuelen, मूल्य);

  वापस 0; /* 0 is successful */
}

अटल ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint8_t *buf, माप_प्रकार लंबाई,
                                    uint32_t *तथ्य_flags,
                                    nghttp2_तथ्य_source *source,
                                    व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  काष्ठा tunnel_stream *ts;
  CURLcode परिणाम;
  माप_प्रकार nread;

  (व्योम)session;
  (व्योम)source;
  (व्योम)ctx;

  अगर(!stream_id)
    वापस NGHTTP2_ERR_INVALID_ARGUMENT;

  ts = Curl_ngh2_session_get_stream_user_तथ्य(&ctx->ngh2, stream_id);
  अगर(!ts)
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  परिणाम = Curl_bufq_read(&ts->sendbuf, buf, लंबाई, &nread);
  अगर(परिणाम) {
    अगर(परिणाम != CURLE_AGAIN)
      वापस NGHTTP2_ERR_CALLBACK_FAILURE;
    वापस NGHTTP2_ERR_DEFERRED;
  }
  अगर(ts->closed && Curl_bufq_is_empty(&ts->sendbuf))
    *तथ्य_flags = NGHTTP2_तथ्य_FLAG_EOF;

  CURL_TRC_CF(तथ्य, cf, "[%d] tunnel_send_callback -> %zu",
              ts->stream_id, nread);
  वापस (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

अटल पूर्णांक tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t stream_id,
                                स्थिर uint8_t *mem, माप_प्रकार len, व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  माप_प्रकार nwritten;
  CURLcode परिणाम;

  (व्योम)flags;
  (व्योम)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  अगर(stream_id != ctx->tunnel.stream_id)
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;

  परिणाम = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  अगर(परिणाम) {
    अगर(परिणाम != CURLE_AGAIN)
      वापस NGHTTP2_ERR_CALLBACK_FAILURE;
#अगर_घोषित DEBUGBUILD
    nwritten = 0;
#पूर्ण_अगर
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  वापस 0;
}

अटल पूर्णांक proxy_h2_on_stream_close(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint32_t error_code, व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);

  (व्योम)session;

  अगर(stream_id != ctx->tunnel.stream_id)
    वापस 0;

  CURL_TRC_CF(तथ्य, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_code;
  अगर(error_code)
    ctx->tunnel.reset = TRUE;

  वापस 0;
}

अटल CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  काष्ठा Curl_cfilter *cf,
  काष्ठा Curl_easy *तथ्य,
  काष्ठा curl_ngh2 *ngh2,
  काष्ठा httpreq *req,
  स्थिर nghttp2_priority_spec *pri_spec,
  व्योम *stream_user_तथ्य,
  nghttp2_तथ्य_source_read_callback read_callback,
  व्योम *read_ctx)
{
  काष्ठा dynhds h2_headers;
  nghttp2_nv *nva = शून्य;
  int32_t stream_id = -1;
  माप_प्रकार nheader;
  CURLcode परिणाम;

  (व्योम)cf;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_गिनती, DYN_HTTP_REQUEST);
  परिणाम = Curl_http_req_to_h2(&h2_headers, req, तथ्य);
  अगर(परिणाम)
    जाओ out;

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  अगर(!nva) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  अगर(read_callback) {
    nghttp2_तथ्य_provider तथ्य_prd;

    तथ्य_prd.read_callback = read_callback;
    तथ्य_prd.source.ptr = read_ctx;
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &तथ्य_prd, stream_user_तथ्य);
  }
  अन्यथा {
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, शून्य, stream_user_तथ्य);
  }

  अगर(stream_id < 0) {
    failf(तथ्य, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(stream_id), stream_id);
    परिणाम = CURLE_SEND_ERROR;
    जाओ out;
  }
  परिणाम = CURLE_OK;

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  *pstream_id = stream_id;
  वापस परिणाम;
}

अटल CURLcode submit_CONNECT(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा tunnel_stream *ts)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम;
  काष्ठा httpreq *req = शून्य;

  परिणाम = Curl_http_proxy_create_tunnel_request(&req, cf, तथ्य,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  अगर(परिणाम)
    जाओ out;
  परिणाम = Curl_creader_set_null(तथ्य);
  अगर(परिणाम)
    जाओ out;

  परिणाम = proxy_h2_submit(&ts->stream_id, cf, तथ्य, &ctx->ngh2, req,
                           शून्य, ts, tunnel_send_callback, cf);
  अगर(परिणाम) {
    CURL_TRC_CF(तथ्य, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->stream_id, nghttp2_strerror(ts->stream_id));
  }

out:
  अगर(req)
    Curl_http_req_free(req);
  अगर(परिणाम)
    failf(तथ्य, "Failed sending CONNECT to proxy");
  वापस परिणाम;
}

अटल CURLcode inspect_response(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा tunnel_stream *ts)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_परिणाम res;
  CURLcode परिणाम;

  परिणाम = Curl_http_proxy_inspect_tunnel_response(
      cf, तथ्य, ts->resp, (bool)ctx->udp_tunnel, &res);
  अगर(परिणाम)
    वापस परिणाम;
  चयन(res) {
  हाल PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, तथ्य,
                       (bool)ctx->udp_tunnel);
    अवरोध;
  हाल PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, तथ्य,
                       (bool)ctx->udp_tunnel);
    परिणाम = CURLE_COULDNT_CONNECT;
    अवरोध;
  हाल PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, तथ्य,
                       (bool)ctx->udp_tunnel);
    अवरोध;
  }
  वापस परिणाम;
}

अटल CURLcode H2_CONNECT(काष्ठा Curl_cfilter *cf,
                           काष्ठा Curl_easy *तथ्य,
                           काष्ठा tunnel_stream *ts)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  अगर(ctx->conn_closed) {
    failf(तथ्य, "proxy closed connection");
    वापस CURLE_COULDNT_CONNECT;
  }

  करो {
    चयन(ts->state) {
    हाल H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(तथ्य, cf, "[0] CONNECT start for %s", ts->authority);
      परिणाम = submit_CONNECT(cf, तथ्य, ts);
      अगर(परिणाम)
        जाओ out;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, तथ्य,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    हाल H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      परिणाम = proxy_h2_progress_ingress(cf, तथ्य);
      अगर(!परिणाम)
        परिणाम = proxy_h2_progress_egress(cf, तथ्य);
      अगर(परिणाम && परिणाम != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, तथ्य,
                           (bool)ctx->udp_tunnel);
        अवरोध;
      }

      अगर(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, तथ्य,
                           (bool)ctx->udp_tunnel);
      }
      अन्यथा {
        परिणाम = CURLE_OK;
        जाओ out;
      }
      FALLTHROUGH();

    हाल H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      परिणाम = inspect_response(cf, तथ्य, ts);
      अगर(परिणाम)
        जाओ out;
      अवरोध;

    हाल H2_TUNNEL_ESTABLISHED:
      वापस CURLE_OK;

    हाल H2_TUNNEL_FAILED:
      वापस CURLE_RECV_ERROR;

    शेष:
      अवरोध;
    }

  } जबतक(ts->state == H2_TUNNEL_INIT);

out:
  अगर((परिणाम && (परिणाम != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, तथ्य,
                       (bool)ctx->udp_tunnel);
  वापस परिणाम;
}

/*
 * Initialize the cfilter context
 */
अटल CURLcode cf_h2_proxy_ctx_init(काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  nghttp2_session_callbacks *cbs = शून्य;
  पूर्णांक rc;

  DEBUGASSERT(!ctx->ngh2.s);
  स_रखो(&ctx->tunnel, 0, माप(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_आकार, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_आकार, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  अगर(tunnel_stream_init(&ctx->tunnel, ctx->dest))
    जाओ out;

  rc = nghttp2_session_callbacks_new(&cbs);
  अगर(rc) {
    failf(तथ्य, "Could not initialize nghttp2 callbacks");
    जाओ out;
  }

  nghttp2_session_callbacks_set_send_callback(cbs, on_session_send);
  nghttp2_session_callbacks_set_on_frame_recv_callback(
    cbs, proxy_h2_on_frame_recv);
#अगर_घोषित CURLVERBOSE
  nghttp2_session_callbacks_set_on_frame_send_callback(cbs,
                                                       proxy_h2_on_frame_send);
#पूर्ण_अगर
  nghttp2_session_callbacks_set_on_तथ्य_chunk_recv_callback(
    cbs, tunnel_recv_callback);
  nghttp2_session_callbacks_set_on_stream_close_callback(
    cbs, proxy_h2_on_stream_close);
  nghttp2_session_callbacks_set_on_header_callback(cbs, proxy_h2_on_header);

  /* The nghttp2 session is not yet setup, do it */
  rc = proxy_h2_client_new(cf, cbs);
  अगर(rc) {
    failf(तथ्य, "Could not initialize nghttp2");
    जाओ out;
  }

  {
    nghttp2_settings_entry iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].मूल्य = Curl_multi_max_concurrent_streams(तथ्य->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_आकार;
    iv[1].मूल्य = H2_TUNNEL_WINDOW_आकार;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_धकेलो;
    iv[2].मूल्य = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    अगर(rc) {
      failf(तथ्य, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      परिणाम = CURLE_HTTP2;
      जाओ out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_आकार(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_WINDOW_आकार);
  अगर(rc) {
    failf(तथ्य, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    परिणाम = CURLE_HTTP2;
    जाओ out;
  }

  /* all set, traffic will be send on connect */
  परिणाम = CURLE_OK;

out:
  अगर(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(तथ्य, cf, "[0] init proxy ctx -> %d", (पूर्णांक)परिणाम);
  वापस परिणाम;
}

अटल CURLcode cf_h2_proxy_connect(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    bool *done)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा cf_call_तथ्य save;
  काष्ठा tunnel_stream *ts = &ctx->tunnel;

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  /* Connect the lower filters first */
  अगर(!cf->अग्र->connected) {
    परिणाम = Curl_conn_cf_connect(cf->अग्र, तथ्य, done);
    अगर(परिणाम || !*done)
      वापस परिणाम;
  }

  *done = FALSE;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  अगर(!ctx->ngh2.s) {
    परिणाम = cf_h2_proxy_ctx_init(cf, तथ्य);
    अगर(परिणाम)
      जाओ out;
  }
  DEBUGASSERT(ts->authority);

  अगर(Curl_timeleft_ms(तथ्य) < 0) {
    failf(तथ्य, "Proxy CONNECT aborted due to timeout");
    परिणाम = CURLE_OPERATION_TIMEDOUT;
    जाओ out;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  परिणाम = H2_CONNECT(cf, तथ्य, ts);

out:
  *done = (परिणाम == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  अगर(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&तथ्य->req, तथ्य);
    Curl_client_reset(तथ्य);
  }
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल व्योम cf_h2_proxy_destroy(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;

  (व्योम)तथ्य;
  अगर(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = शून्य;
  }
}

अटल CURLcode cf_h2_proxy_shutdown(काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  CURLcode परिणाम;
  पूर्णांक rv;

  अगर(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  CF_तथ्य_SAVE(save, cf, तथ्य);

  अगर(!ctx->sent_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (स्थिर uint8_t *)"shutdown", माप("shutdown"));
    अगर(rv) {
      failf(तथ्य, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
    ctx->sent_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  परिणाम = CURLE_OK;
  अगर(Curl_ngh2_session_want_write(&ctx->ngh2))
    परिणाम = proxy_h2_progress_egress(cf, तथ्य);
  अगर(!परिणाम && Curl_ngh2_session_want_read(&ctx->ngh2))
    परिणाम = proxy_h2_progress_ingress(cf, तथ्य);

  *done = (ctx->conn_closed ||
           (!परिणाम && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
out:
  CF_तथ्य_RESTORE(cf, save);
  cf->shutdown = (परिणाम || *done);
  वापस परिणाम;
}

अटल bool cf_h2_proxy_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                                     स्थिर काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  अगर((ctx && !Curl_bufq_is_empty(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    वापस TRUE;
  वापस cf->अग्र ? cf->अग्र->cft->has_तथ्य_pending(cf->अग्र, तथ्य) : FALSE;
}

अटल CURLcode cf_h2_proxy_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                           काष्ठा Curl_easy *तथ्य,
                                           काष्ठा easy_pollset *ps)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, तथ्य);
  bool want_recv, want_send;
  CURLcode परिणाम = CURLE_OK;

  अगर(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  अन्यथा
    Curl_pollset_check(तथ्य, ps, sock, &want_recv, &want_send);

  अगर(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
    bool c_exhaust, s_exhaust;

    CF_तथ्य_SAVE(save, cf, तथ्य);
    c_exhaust = !Curl_ngh2_session_get_remote_window_आकार(&ctx->ngh2);
    s_exhaust = ctx->tunnel.stream_id >= 0 &&
                !Curl_ngh2_session_get_stream_remote_window_आकार(
                  &ctx->ngh2, ctx->tunnel.stream_id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);

    परिणाम = Curl_pollset_set(तथ्य, ps, sock, want_recv, want_send);
    CURL_TRC_CF(तथ्य, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (पूर्णांक)परिणाम);
    CF_तथ्य_RESTORE(cf, save);
  }
  अन्यथा अगर(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_तथ्य_SAVE(save, cf, तथ्य);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    परिणाम = Curl_pollset_set(तथ्य, ps, sock, want_recv, want_send);
    CURL_TRC_CF(तथ्य, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (पूर्णांक)परिणाम);
    CF_तथ्य_RESTORE(cf, save);
  }
  वापस परिणाम;
}

अटल CURLcode h2_handle_tunnel_close(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य,
                                       माप_प्रकार *pnread)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  अगर(ctx->tunnel.error) {
    failf(तथ्य, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.stream_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    वापस CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(तथ्य, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.stream_id);
  वापस CURLE_OK;
}

अटल CURLcode tunnel_recv(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                            अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_AGAIN;

  *pnread = 0;
  अगर(!Curl_bufq_is_empty(&ctx->tunnel.recvbuf))
    परिणाम = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  अन्यथा {
    अगर(ctx->tunnel.closed) {
      परिणाम = h2_handle_tunnel_close(cf, तथ्य, pnread);
    }
    अन्यथा अगर(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.stream_id)) {
      परिणाम = CURLE_RECV_ERROR;
    }
    अन्यथा
      परिणाम = CURLE_AGAIN;
  }

  CURL_TRC_CF(तथ्य, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (पूर्णांक)परिणाम, *pnread);
  वापस परिणाम;
}

अटल CURLcode cf_h2_proxy_recv(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 अक्षर *buf, माप_प्रकार len,
                                 माप_प्रकार *pnread)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  CURLcode परिणाम;

  *pnread = 0;
  CF_तथ्य_SAVE(save, cf, तथ्य);

  अगर(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    परिणाम = CURLE_RECV_ERROR;
    जाओ out;
  }

  अगर(Curl_bufq_is_empty(&ctx->tunnel.recvbuf)) {
    परिणाम = proxy_h2_progress_ingress(cf, तथ्य);
    अगर(परिणाम)
      जाओ out;
  }

  परिणाम = tunnel_recv(cf, तथ्य, buf, len, pnread);

  अगर(!परिणाम) {
    CURL_TRC_CF(तथ्य, cf, "[%d] increase window by %zu",
                ctx->tunnel.stream_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.stream_id, *pnread);
  }

  परिणाम = Curl_1st_fatal(परिणाम, proxy_h2_progress_egress(cf, तथ्य));

out:
  अगर((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!परिणाम || (परिणाम == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, तथ्य, &ctx->tunnel);
  }
  CURL_TRC_CF(तथ्य, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (पूर्णांक)परिणाम, *pnread);
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल CURLcode cf_h2_proxy_send(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 स्थिर uint8_t *buf, माप_प्रकार len, bool eos,
                                 माप_प्रकार *pnwritten)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  CURLcode परिणाम;

  (व्योम)eos;
  *pnwritten = 0;
  CF_तथ्य_SAVE(save, cf, तथ्य);

  अगर(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    परिणाम = CURLE_SEND_ERROR;
    जाओ out;
  }

  अगर(ctx->tunnel.closed) {
    परिणाम = CURLE_SEND_ERROR;
    जाओ out;
  }

  परिणाम = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(तथ्य, cf, "cf_send(), bufq_write %d, %zu", (पूर्णांक)परिणाम,
              *pnwritten);
  अगर(परिणाम && (परिणाम != CURLE_AGAIN))
    जाओ out;

  अगर(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    अगर(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_तथ्य(&ctx->ngh2, ctx->tunnel.stream_id))) {
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
  }

  परिणाम = Curl_1st_fatal(परिणाम, proxy_h2_progress_ingress(cf, तथ्य));
  परिणाम = Curl_1st_fatal(परिणाम, proxy_h2_progress_egress(cf, तथ्य));

  अगर(!परिणाम && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    अगर(ctx->tunnel.closed) {
      परिणाम = CURLE_SEND_ERROR;
    }
    अन्यथा {
      CURL_TRC_CF(तथ्य, cf, "[0] send: nothing to do in this session");
      परिणाम = CURLE_HTTP2;
    }
  }

out:
  अगर((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!परिणाम || (परिणाम == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, तथ्य, &ctx->tunnel);
  }
  CURL_TRC_CF(तथ्य, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, len, (पूर्णांक)परिणाम, *pnwritten,
              Curl_ngh2_session_get_stream_remote_window_आकार(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_आकार(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल CURLcode cf_h2_proxy_flush(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  CURLcode परिणाम = CURLE_OK;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  अगर(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    अगर(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_तथ्य(&ctx->ngh2, ctx->tunnel.stream_id))) {
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
  }

  परिणाम = proxy_h2_progress_egress(cf, तथ्य);

out:
  CURL_TRC_CF(तथ्य, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, (पूर्णांक)परिणाम,
              Curl_ngh2_session_get_stream_remote_window_आकार(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_आकार(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल bool proxy_h2_connisalive(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 bool *निवेश_pending)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *निवेश_pending = FALSE;
  अगर(!cf->अग्र || !cf->अग्र->cft->is_alive(cf->अग्र, तथ्य, निवेश_pending))
    वापस FALSE;

  अगर(*निवेश_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode परिणाम;
    माप_प्रकार nread;

    *निवेश_pending = FALSE;
    परिणाम = Curl_cf_recv_bufq(cf->अग्र, तथ्य, &ctx->inbufq, 0, &nread);
    अगर(!परिणाम) {
      अगर(proxy_h2_process_pending_निवेश(cf, तथ्य))
        /* immediate error, considered dead */
        alive = FALSE;
      अन्यथा {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    अन्यथा अगर(परिणाम != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  वापस alive;
}

अटल bool cf_h2_proxy_is_alive(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 bool *निवेश_pending)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  काष्ठा cf_call_तथ्य save;

  *निवेश_pending = FALSE;
  CF_तथ्य_SAVE(save, cf, तथ्य);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, तथ्य, निवेश_pending));
  CURL_TRC_CF(तथ्य, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *निवेश_pending);
  CF_तथ्य_RESTORE(cf, save);
  वापस alive;
}

अटल CURLcode cf_h2_proxy_query(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  काष्ठा cf_h2_proxy_ctx *ctx = cf->ctx;

  चयन(query) {
  हाल CF_QUERY_HOST_PORT:
    *pres1 = (पूर्णांक)ctx->dest->port;
    *((स्थिर अक्षर **)pres2) = ctx->dest->hostname;
    वापस CURLE_OK;
  हाल CF_QUERY_NEED_FLUSH: {
    अगर(!Curl_bufq_is_empty(&ctx->outbufq) ||
       !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(तथ्य, cf, "needs flush");
      *pres1 = TRUE;
      वापस CURLE_OK;
    }
    अवरोध;
  }
  हाल CF_QUERY_ALPN_NEGOTIATED: {
    स्थिर अक्षर **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = शून्य;
    वापस CURLE_OK;
  }
  शेष:
    अवरोध;
  }
  वापस cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, तथ्य, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

अटल CURLcode cf_h2_proxy_cntrl(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा cf_call_तथ्य save;

  (व्योम)arg1;
  (व्योम)arg2;

  चयन(event) {
  हाल CF_CTRL_FLUSH:
    CF_तथ्य_SAVE(save, cf, तथ्य);
    परिणाम = cf_h2_proxy_flush(cf, तथ्य);
    CF_तथ्य_RESTORE(cf, save);
    अवरोध;
  शेष:
    अवरोध;
  }
  वापस परिणाम;
}

काष्ठा Curl_cftype Curl_cft_h2_proxy = {
  "H2-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  cf_h2_proxy_destroy,
  cf_h2_proxy_connect,
  cf_h2_proxy_shutdown,
  cf_h2_proxy_adjust_pollset,
  cf_h2_proxy_तथ्य_pending,
  cf_h2_proxy_send,
  cf_h2_proxy_recv,
  cf_h2_proxy_cntrl,
  cf_h2_proxy_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h2_proxy_query,
};

CURLcode Curl_cf_h2_proxy_जोड़ो_after(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य,
                                       काष्ठा Curl_peer *peer,
                                       काष्ठा Curl_peer *dest,
                                       bool udp_tunnel)
{
  काष्ठा Curl_cfilter *cf_h2_proxy = शून्य;
  काष्ठा cf_h2_proxy_ctx *ctx;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  (व्योम)तथ्य;
  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx)
    जाओ out;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  परिणाम = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  अगर(परिणाम)
    जाओ out;
  ctx = शून्य;
  Curl_conn_cf_जोड़ो_after(cf, cf_h2_proxy);

out:
  cf_h2_proxy_ctx_free(ctx);
  वापस परिणाम;
}

#पूर्ण_अगर /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
