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
  defined(USE_NGHTTP2)

#अन्तर्भाव <nghttp2/nghttp2.h>

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "bufq.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "dynhds.h"
#अन्तर्भाव "http2.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "ngh2_shim.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "cf-h2-proxy.h"

#define PROXY_H2_CHUNK_SIZE  (16 * 1024)

#define PROXY_HTTP2_HUGE_WINDOW_SIZE (100 * 1024 * 1024)
#define H2_TUNNEL_WINDOW_SIZE        (10 * 1024 * 1024)

#define PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define PROXY_H2_NW_SEND_CHUNKS   1

#define H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_SIZE)


प्रकारनिर्देश गणना {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

संरचना tunnel_stream {
  संरचना http_resp *resp;
  संरचना bufq recvbuf;
  संरचना bufq sendbuf;
  वर्ण *authority;
  int32_t stream_id;
  uint32_t error;
  h2_tunnel_state state;
  BIT(has_final_response);
  BIT(closed);
  BIT(reset);
};

स्थिर CURLcode tunnel_stream_init(संरचना tunnel_stream *ts,
                                   संरचना Curl_peer *dest)
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
  यदि(!ts->authority)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  प्रत्यागम CURLE_OK;
}

स्थिर शून्य tunnel_stream_reset(संरचना tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = रिक्त;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->stream_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

स्थिर शून्य tunnel_stream_clear(संरचना tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  स_रखो(ts, 0, परिमाण(*ts));
  ts->state = H2_TUNNEL_INIT;
}

स्थिर शून्य h2_tunnel_go_state(संरचना Curl_cfilter *cf,
                               संरचना tunnel_stream *ts,
                               h2_tunnel_state new_state,
                               संरचना Curl_easy *data,
                               bool udp_tunnel)
{
  (शून्य)cf;
  (शून्य)udp_tunnel;

  यदि(ts->state == new_state)
    प्रत्यागम;
  /* leaving this one */
  विकल्प(ts->state) {
  पक्ष H2_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  /* entering this one */
  विकल्प(new_state) {
  पक्ष H2_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'init'", ts->stream_id);
    tunnel_stream_reset(ts);
    भङ्ग;

  पक्ष H2_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'connect'", ts->stream_id);
    ts->state = H2_TUNNEL_CONNECT;
    भङ्ग;

  पक्ष H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'response'", ts->stream_id);
    ts->state = H2_TUNNEL_RESPONSE;
    भङ्ग;

  पक्ष H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'established'",
                ts->stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  पक्ष H2_TUNNEL_FAILED:
    यदि(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(data, cf, "[%d] new tunnel state 'failed'", ts->stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    भङ्ग;
  }
}

संरचना cf_h2_proxy_ctx {
  संरचना curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  संरचना cf_call_data call_data;

  संरचना bufq inbufq;  /* network receive buffer */
  संरचना bufq outbufq; /* network send buffer */

  संरचना Curl_peer *peer; /* proxy we talk to */
  संरचना Curl_peer *dest; /* where to tunnel to */
  संरचना tunnel_stream tunnel; /* our tunnel CONNECT stream */
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
#define CF_CTX_CALL_DATA(cf) ((संरचना cf_h2_proxy_ctx *)(cf)->ctx)->call_data

स्थिर शून्य cf_h2_proxy_ctx_clear(संरचना cf_h2_proxy_ctx *ctx)
{
  संरचना cf_call_data save = ctx->call_data;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_stream_clear(&ctx->tunnel);
  स_रखो(ctx, 0, परिमाण(*ctx));
  ctx->call_data = save;
}

स्थिर शून्य cf_h2_proxy_ctx_free(संरचना cf_h2_proxy_ctx *ctx)
{
  यदि(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

स्थिर शून्य drain_tunnel(संरचना Curl_cfilter *cf,
                         संरचना Curl_easy *data,
                         संरचना tunnel_stream *tunnel)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  (शून्य)cf;
  यदि(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_empty(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(data);
}

स्थिर CURLcode proxy_h2_nw_out_writer(शून्य *writer_ctx,
                                       ध्रुव uint8_t *buf, माप_प्रकार buflen,
                                       माप_प्रकार *pnwritten)
{
  संरचना Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  यदि(cf) {
    संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
    CURLcode result;
    result = Curl_conn_cf_send(cf->next, data, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(data, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (पूर्णाङ्क)result, *pnwritten);
    प्रत्यागम result;
  }
  प्रत्यागम CURLE_FAILED_INIT;
}

स्थिर पूर्णाङ्क proxy_h2_client_new(संरचना Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { रिक्त, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  पूर्णाङ्क rc = nghttp2_option_new(&o);
  यदि(rc)
    प्रत्यागम rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#यदि NGHTTP2_VERSION_अंक >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  प्रत्यागम rc;
}

स्थिर पूर्णाङ्क proxy_h2_should_close_session(संरचना cf_h2_proxy_ctx *ctx)
{
  प्रत्यागम !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

स्थिर CURLcode proxy_h2_nw_out_flush(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  माप_प्रकार nwritten;
  CURLcode result;

  यदि(Curl_bufq_is_empty(&ctx->outbufq))
    प्रत्यागम CURLE_OK;

  result = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_out_writer, cf,
                          &nwritten);
  यदि(result) {
    यदि(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    प्रत्यागम result;
  }
  CURL_TRC_CF(data, cf, "[0] nw send buffer flushed");
  प्रत्यागम Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
स्थिर CURLcode proxy_h2_process_pending_निवेश(संरचना Curl_cfilter *cf,
                                               संरचना Curl_easy *data)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  ध्रुव अचिन्हित वर्ण *buf;
  माप_प्रकार blen, nread;
  ssize_t rv;

  यावत्(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ध्रुव uint8_t *)buf, blen);
    CURL_TRC_CF(data, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    यदि(!curlx_sztouz(rv, &nread)) {
      failf(data,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((पूर्णाङ्क)rv));
      प्रत्यागम CURLE_RECV_ERROR;
    }
    अन्यथा यदि(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      भङ्ग;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    यदि(Curl_bufq_is_empty(&ctx->inbufq)) {
      CURL_TRC_CF(data, cf, "[0] all data in connection buffer processed");
      भङ्ग;
    }
    अन्यथा {
      CURL_TRC_CF(data, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode proxy_h2_progress_ingress(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *data)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  माप_प्रकार nread;

  /* Process network input buffer first */
  यदि(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = proxy_h2_process_pending_निवेश(cf, data);
    यदि(result)
      प्रत्यागम result;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  यावत्(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_empty(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_full(&ctx->tunnel.recvbuf)) {

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (पूर्णाङ्क)result, nread);
    यदि(result) {
      यदि(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 proxy data");
        प्रत्यागम result;
      }
      भङ्ग;
    }
    अन्यथा यदि(nread == 0) {
      CURL_TRC_CF(data, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      भङ्ग;
    }

    result = proxy_h2_process_pending_निवेश(cf, data);
    यदि(result)
      प्रत्यागम result;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode proxy_h2_progress_egress(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *data)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  पूर्णाङ्क rv = 0;

  ctx->nw_out_blocked = 0;
  यावत्(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  यदि(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    प्रत्यागम CURLE_SEND_ERROR;
  }
  प्रत्यागम proxy_h2_nw_out_flush(cf, data);
}

स्थिर ssize_t on_session_send(nghttp2_session *h2,
                               ध्रुव uint8_t *buf, माप_प्रकार blen, पूर्णाङ्क flags,
                               शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  माप_प्रकार nwritten;
  CURLcode result = CURLE_OK;

  (शून्य)h2;
  (शून्य)flags;
  DEBUGASSERT(data);

  result = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_out_writer, cf, &nwritten);
  यदि(result) {
    यदि(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      प्रत्यागम NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  यदि(!nwritten)
    प्रत्यागम NGHTTP2_ERR_WOULDBLOCK;

  प्रत्यागम (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#ifdef CURLVERBOSE
स्थिर पूर्णाङ्क proxy_h2_on_frame_send(nghttp2_session *session,
                                  ध्रुव nghttp2_frame *frame,
                                  शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);

  (शून्य)session;
  DEBUGASSERT(data);
  यदि(Curl_trc_cf_is_verbose(cf, data)) {
    वर्ण buffer[256];
    पूर्णाङ्क len;
    len = Curl_nghttp2_fr_print(frame, buffer, परिमाण(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  प्रत्यागम 0;
}
#endif /* CURLVERBOSE */

स्थिर पूर्णाङ्क proxy_h2_on_frame_recv(nghttp2_session *session,
                                  ध्रुव nghttp2_frame *frame,
                                  शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;

  (शून्य)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  यदि(Curl_trc_cf_is_verbose(cf, data)) {
    वर्ण buffer[256];
    पूर्णाङ्क len;
    len = Curl_nghttp2_fr_print(frame, buffer, परिमाण(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  यदि(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    विकल्प(frame->hd.type) {
    पक्ष NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      यदि(CURL_REQ_WANT_SEND(data)) {
        drain_tunnel(cf, data, &ctx->tunnel);
      }
      भङ्ग;
    पक्ष NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      यदि(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              frame->goaway.error_code, ctx->remote_max_sid);
      }
      भङ्ग;
    स्वभाव:
      भङ्ग;
    }
    प्रत्यागम 0;
  }

  यदि(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] rcvd FRAME not for tunnel", stream_id);
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  विकल्प(frame->hd.type) {
  पक्ष NGHTTP2_HEADERS:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    यदि(!ctx->tunnel.resp)
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(data, cf, "[%d] got http status: %d",
                stream_id, ctx->tunnel.resp->status);
    यदि(!ctx->tunnel.has_final_response && ctx->tunnel.resp->status / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    भङ्ग;
  पक्ष NGHTTP2_WINDOW_UPDATE:
    यदि(CURL_REQ_WANT_SEND(data)) {
      drain_tunnel(cf, data, &ctx->tunnel);
    }
    भङ्ग;
  पक्ष NGHTTP2_RST_STREAM:
    यदि(frame->rst_stream.error_code)
      ctx->tunnel.reset = TRUE;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क proxy_h2_on_header(nghttp2_session *session,
                              ध्रुव nghttp2_frame *frame,
                              ध्रुव uint8_t *name, माप_प्रकार namelen,
                              ध्रुव uint8_t *मूल्य, माप_प्रकार valuelen,
                              uint8_t flags,
                              शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (शून्य)flags;
  (शून्य)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  यदि(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", stream_id,
                (पूर्णाङ्क)namelen, name, (पूर्णाङ्क)valuelen, मूल्य);
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  यदि(frame->hd.type == NGHTTP2_धकेलो_PROMISE)
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  यदि(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    प्रत्यागम 0;
  }

  यदि(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !स_भेद(HTTP_PSEUDO_STATUS, name, namelen)) {
    int32_t http_status;
    संरचना http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    result = Curl_http_decode_status(&http_status,
                                    (ध्रुव वर्ण *)मूल्य, valuelen);
    यदि(result)
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    result = Curl_http_resp_make(&resp, http_status, रिक्त);
    यदि(result)
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    यदि(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream_id, ctx->tunnel.resp->status);
    प्रत्यागम 0;
  }

  यदि(!ctx->tunnel.resp)
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_dynhds_add(&ctx->tunnel.resp->headers,
                           (ध्रुव वर्ण *)name, namelen,
                           (ध्रुव वर्ण *)मूल्य, valuelen);
  यदि(result)
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream_id, (पूर्णाङ्क)namelen, name, (पूर्णाङ्क)valuelen, मूल्य);

  प्रत्यागम 0; /* 0 is successful */
}

स्थिर ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint8_t *buf, माप_प्रकार length,
                                    uint32_t *data_flags,
                                    nghttp2_data_source *source,
                                    शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  संरचना tunnel_stream *ts;
  CURLcode result;
  माप_प्रकार nread;

  (शून्य)session;
  (शून्य)source;
  (शून्य)ctx;

  यदि(!stream_id)
    प्रत्यागम NGHTTP2_ERR_INVALID_ARGUMENT;

  ts = Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  यदि(!ts)
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  result = Curl_bufq_read(&ts->sendbuf, buf, length, &nread);
  यदि(result) {
    यदि(result != CURLE_AGAIN)
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    प्रत्यागम NGHTTP2_ERR_DEFERRED;
  }
  यदि(ts->closed && Curl_bufq_is_empty(&ts->sendbuf))
    *data_flags = NGHTTP2_DATA_FLAG_EOF;

  CURL_TRC_CF(data, cf, "[%d] tunnel_send_callback -> %zu",
              ts->stream_id, nread);
  प्रत्यागम (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

स्थिर पूर्णाङ्क tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t stream_id,
                                ध्रुव uint8_t *mem, माप_प्रकार len, शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  माप_प्रकार nwritten;
  CURLcode result;

  (शून्य)flags;
  (शून्य)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  यदि(stream_id != ctx->tunnel.stream_id)
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  यदि(result) {
    यदि(result != CURLE_AGAIN)
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
#ifdef DEBUGBUILD
    nwritten = 0;
#endif
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क proxy_h2_on_stream_close(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint32_t error_code, शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);

  (शून्य)session;

  यदि(stream_id != ctx->tunnel.stream_id)
    प्रत्यागम 0;

  CURL_TRC_CF(data, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_code;
  यदि(error_code)
    ctx->tunnel.reset = TRUE;

  प्रत्यागम 0;
}

स्थिर CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  संरचना Curl_cfilter *cf,
  संरचना Curl_easy *data,
  संरचना curl_ngh2 *ngh2,
  संरचना httpreq *req,
  ध्रुव nghttp2_priority_spec *pri_spec,
  शून्य *stream_user_data,
  nghttp2_data_source_read_callback read_callback,
  शून्य *read_ctx)
{
  संरचना dynhds h2_headers;
  nghttp2_nv *nva = रिक्त;
  int32_t stream_id = -1;
  माप_प्रकार nheader;
  CURLcode result;

  (शून्य)cf;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_गिनती, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  यदि(result)
    गच्छ out;

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  यदि(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  यदि(read_callback) {
    nghttp2_data_provider data_prd;

    data_prd.read_callback = read_callback;
    data_prd.source.ptr = read_ctx;
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &data_prd, stream_user_data);
  }
  अन्यथा {
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, रिक्त, stream_user_data);
  }

  यदि(stream_id < 0) {
    failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    गच्छ out;
  }
  result = CURLE_OK;

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  *pstream_id = stream_id;
  प्रत्यागम result;
}

स्थिर CURLcode submit_CONNECT(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               संरचना tunnel_stream *ts)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  संरचना httpreq *req = रिक्त;

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  यदि(result)
    गच्छ out;
  result = Curl_creader_set_null(data);
  यदि(result)
    गच्छ out;

  result = proxy_h2_submit(&ts->stream_id, cf, data, &ctx->ngh2, req,
                           रिक्त, ts, tunnel_send_callback, cf);
  यदि(result) {
    CURL_TRC_CF(data, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->stream_id, nghttp2_strerror(ts->stream_id));
  }

out:
  यदि(req)
    Curl_http_req_free(req);
  यदि(result)
    failf(data, "Failed sending CONNECT to proxy");
  प्रत्यागम result;
}

स्थिर CURLcode inspect_response(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना tunnel_stream *ts)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)ctx->udp_tunnel, &res);
  यदि(result)
    प्रत्यागम result;
  विकल्प(res) {
  पक्ष PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, data,
                       (bool)ctx->udp_tunnel);
    भङ्ग;
  पक्ष PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
    result = CURLE_COULDNT_CONNECT;
    भङ्ग;
  पक्ष PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, data,
                       (bool)ctx->udp_tunnel);
    भङ्ग;
  }
  प्रत्यागम result;
}

स्थिर CURLcode H2_CONNECT(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           संरचना tunnel_stream *ts)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  यदि(ctx->conn_closed) {
    failf(data, "proxy closed connection");
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  कुरु {
    विकल्प(ts->state) {
    पक्ष H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = submit_CONNECT(cf, data, ts);
      यदि(result)
        गच्छ out;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, data,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    पक्ष H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      result = proxy_h2_progress_ingress(cf, data);
      यदि(!result)
        result = proxy_h2_progress_egress(cf, data);
      यदि(result && result != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                           (bool)ctx->udp_tunnel);
        भङ्ग;
      }

      यदि(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, data,
                           (bool)ctx->udp_tunnel);
      }
      अन्यथा {
        result = CURLE_OK;
        गच्छ out;
      }
      FALLTHROUGH();

    पक्ष H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = inspect_response(cf, data, ts);
      यदि(result)
        गच्छ out;
      भङ्ग;

    पक्ष H2_TUNNEL_ESTABLISHED:
      प्रत्यागम CURLE_OK;

    पक्ष H2_TUNNEL_FAILED:
      प्रत्यागम CURLE_RECV_ERROR;

    स्वभाव:
      भङ्ग;
    }

  } यावत्(ts->state == H2_TUNNEL_INIT);

out:
  यदि((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
  प्रत्यागम result;
}

/*
 * Initialize the cfilter context
 */
स्थिर CURLcode cf_h2_proxy_ctx_init(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  nghttp2_session_callbacks *cbs = रिक्त;
  पूर्णाङ्क rc;

  DEBUGASSERT(!ctx->ngh2.s);
  स_रखो(&ctx->tunnel, 0, परिमाण(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  यदि(tunnel_stream_init(&ctx->tunnel, ctx->dest))
    गच्छ out;

  rc = nghttp2_session_callbacks_new(&cbs);
  यदि(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    गच्छ out;
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
  यदि(rc) {
    failf(data, "Could not initialize nghttp2");
    गच्छ out;
  }

  {
    nghttp2_settings_entry iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].मूल्य = Curl_multi_max_concurrent_streams(data->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
    iv[1].मूल्य = H2_TUNNEL_WINDOW_SIZE;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_धकेलो;
    iv[2].मूल्य = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    यदि(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      गच्छ out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_WINDOW_SIZE);
  यदि(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    गच्छ out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;

out:
  यदि(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(data, cf, "[0] init proxy ctx -> %d", (पूर्णाङ्क)result);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h2_proxy_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    bool *done)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  संरचना cf_call_data save;
  संरचना tunnel_stream *ts = &ctx->tunnel;

  यदि(cf->connected) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }

  /* Connect the lower filters first */
  यदि(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    यदि(result || !*done)
      प्रत्यागम result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  यदि(!ctx->ngh2.s) {
    result = cf_h2_proxy_ctx_init(cf, data);
    यदि(result)
      गच्छ out;
  }
  DEBUGASSERT(ts->authority);

  यदि(Curl_timeleft_ms(data) < 0) {
    failf(data, "Proxy CONNECT aborted due to timeout");
    result = CURLE_OPERATION_TIMEDOUT;
    गच्छ out;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  result = H2_CONNECT(cf, data, ts);

out:
  *done = (result == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  यदि(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर शून्य cf_h2_proxy_destroy(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;

  (शून्य)data;
  यदि(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = रिक्त;
  }
}

स्थिर CURLcode cf_h2_proxy_shutdown(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data, bool *done)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना cf_call_data save;
  CURLcode result;
  पूर्णाङ्क rv;

  यदि(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  यदि(!ctx->sent_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ध्रुव uint8_t *)"shutdown", परिमाण("shutdown"));
    यदि(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      गच्छ out;
    }
    ctx->sent_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  यदि(Curl_ngh2_session_want_write(&ctx->ngh2))
    result = proxy_h2_progress_egress(cf, data);
  यदि(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = proxy_h2_progress_ingress(cf, data);

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  प्रत्यागम result;
}

स्थिर bool cf_h2_proxy_data_pending(संरचना Curl_cfilter *cf,
                                     ध्रुव संरचना Curl_easy *data)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  यदि((ctx && !Curl_bufq_is_empty(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    प्रत्यागम TRUE;
  प्रत्यागम cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

स्थिर CURLcode cf_h2_proxy_adjust_pollset(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *data,
                                           संरचना easy_pollset *ps)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना cf_call_data save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  यदि(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  अन्यथा
    Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  यदि(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
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
                want_recv, want_send, (पूर्णाङ्क)result);
    CF_DATA_RESTORE(cf, save);
  }
  अन्यथा यदि(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CURL_TRC_CF(data, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (पूर्णाङ्क)result);
    CF_DATA_RESTORE(cf, save);
  }
  प्रत्यागम result;
}

स्थिर CURLcode h2_handle_tunnel_close(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data,
                                       माप_प्रकार *pnread)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  यदि(ctx->tunnel.error) {
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.stream_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    प्रत्यागम CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(data, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.stream_id);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode tunnel_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                            वर्ण *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  *pnread = 0;
  यदि(!Curl_bufq_is_empty(&ctx->tunnel.recvbuf))
    result = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  अन्यथा {
    यदि(ctx->tunnel.closed) {
      result = h2_handle_tunnel_close(cf, data, pnread);
    }
    अन्यथा यदि(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.stream_id)) {
      result = CURLE_RECV_ERROR;
    }
    अन्यथा
      result = CURLE_AGAIN;
  }

  CURL_TRC_CF(data, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (पूर्णाङ्क)result, *pnread);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h2_proxy_recv(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 वर्ण *buf, माप_प्रकार len,
                                 माप_प्रकार *pnread)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना cf_call_data save;
  CURLcode result;

  *pnread = 0;
  CF_DATA_SAVE(save, cf, data);

  यदि(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_RECV_ERROR;
    गच्छ out;
  }

  यदि(Curl_bufq_is_empty(&ctx->tunnel.recvbuf)) {
    result = proxy_h2_progress_ingress(cf, data);
    यदि(result)
      गच्छ out;
  }

  result = tunnel_recv(cf, data, buf, len, pnread);

  यदि(!result) {
    CURL_TRC_CF(data, cf, "[%d] increase window by %zu",
                ctx->tunnel.stream_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.stream_id, *pnread);
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

out:
  यदि((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (पूर्णाङ्क)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h2_proxy_send(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 ध्रुव uint8_t *buf, माप_प्रकार len, bool eos,
                                 माप_प्रकार *pnwritten)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना cf_call_data save;
  CURLcode result;

  (शून्य)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  यदि(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_SEND_ERROR;
    गच्छ out;
  }

  यदि(ctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    गच्छ out;
  }

  result = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(data, cf, "cf_send(), bufq_write %d, %zu", (पूर्णाङ्क)result,
              *pnwritten);
  यदि(result && (result != CURLE_AGAIN))
    गच्छ out;

  यदि(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    यदि(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      गच्छ out;
    }
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_ingress(cf, data));
  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

  यदि(!result && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    यदि(ctx->tunnel.closed) {
      result = CURLE_SEND_ERROR;
    }
    अन्यथा {
      CURL_TRC_CF(data, cf, "[0] send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  यदि((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, len, (पूर्णाङ्क)result, *pnwritten,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h2_proxy_flush(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  यदि(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    यदि(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      गच्छ out;
    }
  }

  result = proxy_h2_progress_egress(cf, data);

out:
  CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, (पूर्णाङ्क)result,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर bool proxy_h2_connisalive(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 bool *निवेश_pending)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *निवेश_pending = FALSE;
  यदि(!cf->next || !cf->next->cft->is_alive(cf->next, data, निवेश_pending))
    प्रत्यागम FALSE;

  यदि(*निवेश_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    माप_प्रकार nread;

    *निवेश_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    यदि(!result) {
      यदि(proxy_h2_process_pending_निवेश(cf, data))
        /* immediate error, considered dead */
        alive = FALSE;
      अन्यथा {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    अन्यथा यदि(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  प्रत्यागम alive;
}

स्थिर bool cf_h2_proxy_is_alive(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 bool *निवेश_pending)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  संरचना cf_call_data save;

  *निवेश_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, data, निवेश_pending));
  CURL_TRC_CF(data, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *निवेश_pending);
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम alive;
}

स्थिर CURLcode cf_h2_proxy_query(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;

  विकल्प(query) {
  पक्ष CF_QUERY_HOST_PORT:
    *pres1 = (पूर्णाङ्क)ctx->dest->port;
    *((ध्रुव वर्ण **)pres2) = ctx->dest->hostname;
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_NEED_FLUSH: {
    यदि(!Curl_bufq_is_empty(&ctx->outbufq) ||
       !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(data, cf, "needs flush");
      *pres1 = TRUE;
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  }
  पक्ष CF_QUERY_ALPN_NEGOTIATED: {
    ध्रुव वर्ण **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = रिक्त;
    प्रत्यागम CURLE_OK;
  }
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode cf_h2_proxy_cntrl(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  CURLcode result = CURLE_OK;
  संरचना cf_call_data save;

  (शून्य)arg1;
  (शून्य)arg2;

  विकल्प(event) {
  पक्ष CF_CTRL_FLUSH:
    CF_DATA_SAVE(save, cf, data);
    result = cf_h2_proxy_flush(cf, data);
    CF_DATA_RESTORE(cf, save);
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम result;
}

संरचना Curl_cftype Curl_cft_h2_proxy = {
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

CURLcode Curl_cf_h2_proxy_जोड़ो_after(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data,
                                       संरचना Curl_peer *peer,
                                       संरचना Curl_peer *dest,
                                       bool udp_tunnel)
{
  संरचना Curl_cfilter *cf_h2_proxy = रिक्त;
  संरचना cf_h2_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (शून्य)data;
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx)
    गच्छ out;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  result = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  यदि(result)
    गच्छ out;
  ctx = रिक्त;
  Curl_conn_cf_जोड़ो_after(cf, cf_h2_proxy);

out:
  cf_h2_proxy_ctx_free(ctx);
  प्रत्यागम result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
