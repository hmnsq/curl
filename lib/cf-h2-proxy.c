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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_NGHTTP2)

#ಅನ್ತರ್ಭಾವ <nghttp2/nghttp2.h>

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "dynhds.h"
#ಅನ್ತರ್ಭಾವ "http2.h"
#ಅನ್ತರ್ಭಾವ "http_proxy.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "ngh2_shim.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "cf-h2-proxy.h"

#define PROXY_H2_CHUNK_SIZE  (16 * 1024)

#define PROXY_HTTP2_HUGE_WINDOW_SIZE (100 * 1024 * 1024)
#define H2_TUNNEL_WINDOW_SIZE        (10 * 1024 * 1024)

#define PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define PROXY_H2_NW_SEND_CHUNKS   1

#define H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_SIZE)


ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

ಸಂರಚನಾ tunnel_stream {
  ಸಂರಚನಾ http_resp *resp;
  ಸಂರಚನಾ bufq recvbuf;
  ಸಂರಚನಾ bufq sendbuf;
  ವರ್ಣ *authority;
  int32_t stream_id;
  uint32_t error;
  h2_tunnel_state state;
  BIT(has_final_response);
  BIT(closed);
  BIT(reset);
};

ಸ್ಥಿರ CURLcode tunnel_stream_init(ಸಂರಚನಾ tunnel_stream *ts,
                                   ಸಂರಚನಾ Curl_peer *dest)
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
  ಯದಿ(!ts->authority)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ tunnel_stream_reset(ಸಂರಚನಾ tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = ರಿಕ್ತ;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->stream_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

ಸ್ಥಿರ ಶೂನ್ಯ tunnel_stream_clear(ಸಂರಚನಾ tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, ಪರಿಮಾಣ(*ts));
  ts->state = H2_TUNNEL_INIT;
}

ಸ್ಥಿರ ಶೂನ್ಯ h2_tunnel_go_state(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ tunnel_stream *ts,
                               h2_tunnel_state new_state,
                               ಸಂರಚನಾ Curl_easy *data,
                               bool udp_tunnel)
{
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)udp_tunnel;

  ಯದಿ(ts->state == new_state)
    ಪ್ರತ್ಯಾಗಮ;
  /* leaving this one */
  ವಿಕಲ್ಪ(ts->state) {
  ಪಕ್ಷ H2_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  /* entering this one */
  ವಿಕಲ್ಪ(new_state) {
  ಪಕ್ಷ H2_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'init'", ts->stream_id);
    tunnel_stream_reset(ts);
    ಭಙ್ಗ;

  ಪಕ್ಷ H2_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'connect'", ts->stream_id);
    ts->state = H2_TUNNEL_CONNECT;
    ಭಙ್ಗ;

  ಪಕ್ಷ H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'response'", ts->stream_id);
    ts->state = H2_TUNNEL_RESPONSE;
    ಭಙ್ಗ;

  ಪಕ್ಷ H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'established'",
                ts->stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  ಪಕ್ಷ H2_TUNNEL_FAILED:
    ಯದಿ(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(data, cf, "[%d] new tunnel state 'failed'", ts->stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    ಭಙ್ಗ;
  }
}

ಸಂರಚನಾ cf_h2_proxy_ctx {
  ಸಂರಚನಾ curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ಸಂರಚನಾ cf_call_data call_data;

  ಸಂರಚನಾ bufq inbufq;  /* network receive buffer */
  ಸಂರಚನಾ bufq outbufq; /* network send buffer */

  ಸಂರಚನಾ Curl_peer *peer; /* proxy we talk to */
  ಸಂರಚನಾ Curl_peer *dest; /* where to tunnel to */
  ಸಂರಚನಾ tunnel_stream tunnel; /* our tunnel CONNECT stream */
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
#define CF_CTX_CALL_DATA(cf) ((ಸಂರಚನಾ cf_h2_proxy_ctx *)(cf)->ctx)->call_data

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_proxy_ctx_clear(ಸಂರಚನಾ cf_h2_proxy_ctx *ctx)
{
  ಸಂರಚನಾ cf_call_data save = ctx->call_data;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_stream_clear(&ctx->tunnel);
  memset(ctx, 0, ಪರಿಮಾಣ(*ctx));
  ctx->call_data = save;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_proxy_ctx_free(ಸಂರಚನಾ cf_h2_proxy_ctx *ctx)
{
  ಯದಿ(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ drain_tunnel(ಸಂರಚನಾ Curl_cfilter *cf,
                         ಸಂರಚನಾ Curl_easy *data,
                         ಸಂರಚನಾ tunnel_stream *tunnel)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  (ಶೂನ್ಯ)cf;
  ಯದಿ(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_empty(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(data);
}

ಸ್ಥಿರ CURLcode proxy_h2_nw_out_writer(ಶೂನ್ಯ *writer_ctx,
                                       ಧ್ರುವ uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  ಸಂರಚನಾ Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  ಯದಿ(cf) {
    ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
    CURLcode result;
    result = Curl_conn_cf_send(cf->next, data, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(data, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten);
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ proxy_h2_client_new(ಸಂರಚನಾ Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ರಿಕ್ತ, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  ಪೂರ್ಣಾಙ್ಕ rc = nghttp2_option_new(&o);
  ಯದಿ(rc)
    ಪ್ರತ್ಯಾಗಮ rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#ಯದಿ NGHTTP2_VERSION_ಅಂಕೆ >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ proxy_h2_should_close_session(ಸಂರಚನಾ cf_h2_proxy_ctx *ctx)
{
  ಪ್ರತ್ಯಾಗಮ !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

ಸ್ಥಿರ CURLcode proxy_h2_nw_out_flush(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  ಯದಿ(Curl_bufq_is_empty(&ctx->outbufq))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  result = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_out_writer, cf,
                          &nwritten);
  ಯದಿ(result) {
    ಯದಿ(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    ಪ್ರತ್ಯಾಗಮ result;
  }
  CURL_TRC_CF(data, cf, "[0] nw send buffer flushed");
  ಪ್ರತ್ಯಾಗಮ Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ಸ್ಥಿರ CURLcode proxy_h2_process_pending_ಒಳಹರಿವು(ಸಂರಚನಾ Curl_cfilter *cf,
                                               ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಧ್ರುವ unsigned ವರ್ಣ *buf;
  size_t blen, nread;
  ssize_t rv;

  ಯಾವತ್(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ಧ್ರುವ uint8_t *)buf, blen);
    CURL_TRC_CF(data, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    ಯದಿ(!curlx_sztouz(rv, &nread)) {
      failf(data,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((ಪೂರ್ಣಾಙ್ಕ)rv));
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
    }
    ಅನ್ಯಥಾ ಯದಿ(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      ಭಙ್ಗ;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    ಯದಿ(Curl_bufq_is_empty(&ctx->inbufq)) {
      CURL_TRC_CF(data, cf, "[0] all data in connection buffer processed");
      ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ {
      CURL_TRC_CF(data, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode proxy_h2_progress_ingress(ಸಂರಚನಾ Curl_cfilter *cf,
                                          ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  ಯದಿ(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = proxy_h2_process_pending_ಒಳಹರಿವು(cf, data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  ಯಾವತ್(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_empty(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_full(&ctx->tunnel.recvbuf)) {

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (ಪೂರ್ಣಾಙ್ಕ)result, nread);
    ಯದಿ(result) {
      ಯದಿ(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 proxy data");
        ಪ್ರತ್ಯಾಗಮ result;
      }
      ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ ಯದಿ(nread == 0) {
      CURL_TRC_CF(data, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      ಭಙ್ಗ;
    }

    result = proxy_h2_process_pending_ಒಳಹರಿವು(cf, data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode proxy_h2_progress_egress(ಸಂರಚನಾ Curl_cfilter *cf,
                                         ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಙ್ಕ rv = 0;

  ctx->nw_out_blocked = 0;
  ಯಾವತ್(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  ಯದಿ(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  }
  ಪ್ರತ್ಯಾಗಮ proxy_h2_nw_out_flush(cf, data);
}

ಸ್ಥಿರ ssize_t on_session_send(nghttp2_session *h2,
                               ಧ್ರುವ uint8_t *buf, size_t blen, ಪೂರ್ಣಾಙ್ಕ flags,
                               ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)h2;
  (ಶೂನ್ಯ)flags;
  DEBUGASSERT(data);

  result = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_out_writer, cf, &nwritten);
  ಯದಿ(result) {
    ಯದಿ(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ಯದಿ(!nwritten)
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_WOULDBLOCK;

  ಪ್ರತ್ಯಾಗಮ (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#ifdef CURLVERBOSE
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ proxy_h2_on_frame_send(nghttp2_session *session,
                                  ಧ್ರುವ nghttp2_frame *frame,
                                  ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);

  (ಶೂನ್ಯ)session;
  DEBUGASSERT(data);
  ಯದಿ(Curl_trc_cf_is_verbose(cf, data)) {
    ವರ್ಣ buffer[256];
    ಪೂರ್ಣಾಙ್ಕ len;
    len = Curl_nghttp2_fr_print(frame, buffer, ಪರಿಮಾಣ(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}
#endif /* CURLVERBOSE */

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ proxy_h2_on_frame_recv(nghttp2_session *session,
                                  ಧ್ರುವ nghttp2_frame *frame,
                                  ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;

  (ಶೂನ್ಯ)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  ಯದಿ(Curl_trc_cf_is_verbose(cf, data)) {
    ವರ್ಣ buffer[256];
    ಪೂರ್ಣಾಙ್ಕ len;
    len = Curl_nghttp2_fr_print(frame, buffer, ಪರಿಮಾಣ(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  ಯದಿ(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    ವಿಕಲ್ಪ(frame->hd.type) {
    ಪಕ್ಷ NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      ಯದಿ(CURL_REQ_WANT_SEND(data)) {
        drain_tunnel(cf, data, &ctx->tunnel);
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      ಯದಿ(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              frame->goaway.error_code, ctx->remote_max_sid);
      }
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] rcvd FRAME not for tunnel", stream_id);
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ವಿಕಲ್ಪ(frame->hd.type) {
  ಪಕ್ಷ NGHTTP2_HEADERS:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    ಯದಿ(!ctx->tunnel.resp)
      ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(data, cf, "[%d] got http status: %d",
                stream_id, ctx->tunnel.resp->status);
    ಯದಿ(!ctx->tunnel.has_final_response && ctx->tunnel.resp->status / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    ಭಙ್ಗ;
  ಪಕ್ಷ NGHTTP2_WINDOW_UPDATE:
    ಯದಿ(CURL_REQ_WANT_SEND(data)) {
      drain_tunnel(cf, data, &ctx->tunnel);
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ NGHTTP2_RST_STREAM:
    ಯದಿ(frame->rst_stream.error_code)
      ctx->tunnel.reset = TRUE;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ proxy_h2_on_header(nghttp2_session *session,
                              ಧ್ರುವ nghttp2_frame *frame,
                              ಧ್ರುವ uint8_t *name, size_t namelen,
                              ಧ್ರುವ uint8_t *ಮೌಲ್ಯ, size_t valuelen,
                              uint8_t flags,
                              ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (ಶೂನ್ಯ)flags;
  (ಶೂನ್ಯ)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  ಯದಿ(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", stream_id,
                (ಪೂರ್ಣಾಙ್ಕ)namelen, name, (ಪೂರ್ಣಾಙ್ಕ)valuelen, ಮೌಲ್ಯ);
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ಯದಿ(frame->hd.type == NGHTTP2_ತಳ್ಳು_PROMISE)
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;

  ಯದಿ(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    int32_t http_status;
    ಸಂರಚನಾ http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    result = Curl_http_decode_status(&http_status,
                                    (ಧ್ರುವ ವರ್ಣ *)ಮೌಲ್ಯ, valuelen);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
    result = Curl_http_resp_make(&resp, http_status, ರಿಕ್ತ);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
    ಯದಿ(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream_id, ctx->tunnel.resp->status);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(!ctx->tunnel.resp)
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_dynhds_add(&ctx->tunnel.resp->headers,
                           (ಧ್ರುವ ವರ್ಣ *)name, namelen,
                           (ಧ್ರುವ ವರ್ಣ *)ಮೌಲ್ಯ, valuelen);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream_id, (ಪೂರ್ಣಾಙ್ಕ)namelen, name, (ಪೂರ್ಣಾಙ್ಕ)valuelen, ಮೌಲ್ಯ);

  ಪ್ರತ್ಯಾಗಮ 0; /* 0 is successful */
}

ಸ್ಥಿರ ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint8_t *buf, size_t length,
                                    uint32_t *data_flags,
                                    nghttp2_data_source *source,
                                    ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  ಸಂರಚನಾ tunnel_stream *ts;
  CURLcode result;
  size_t nread;

  (ಶೂನ್ಯ)session;
  (ಶೂನ್ಯ)source;
  (ಶೂನ್ಯ)ctx;

  ಯದಿ(!stream_id)
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_INVALID_ARGUMENT;

  ts = Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  ಯದಿ(!ts)
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  result = Curl_bufq_read(&ts->sendbuf, buf, length, &nread);
  ಯದಿ(result) {
    ಯದಿ(result != CURLE_AGAIN)
      ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_DEFERRED;
  }
  ಯದಿ(ts->closed && Curl_bufq_is_empty(&ts->sendbuf))
    *data_flags = NGHTTP2_DATA_FLAG_EOF;

  CURL_TRC_CF(data, cf, "[%d] tunnel_send_callback -> %zu",
              ts->stream_id, nread);
  ಪ್ರತ್ಯಾಗಮ (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t stream_id,
                                ಧ್ರುವ uint8_t *mem, size_t len, ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  (ಶೂನ್ಯ)flags;
  (ಶೂನ್ಯ)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  ಯದಿ(stream_id != ctx->tunnel.stream_id)
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  ಯದಿ(result) {
    ಯದಿ(result != CURLE_AGAIN)
      ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
#ifdef DEBUGBUILD
    nwritten = 0;
#endif
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ proxy_h2_on_stream_close(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint32_t error_code, ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);

  (ಶೂನ್ಯ)session;

  ಯದಿ(stream_id != ctx->tunnel.stream_id)
    ಪ್ರತ್ಯಾಗಮ 0;

  CURL_TRC_CF(data, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_code;
  ಯದಿ(error_code)
    ctx->tunnel.reset = TRUE;

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  ಸಂರಚನಾ Curl_cfilter *cf,
  ಸಂರಚನಾ Curl_easy *data,
  ಸಂರಚನಾ curl_ngh2 *ngh2,
  ಸಂರಚನಾ httpreq *req,
  ಧ್ರುವ nghttp2_priority_spec *pri_spec,
  ಶೂನ್ಯ *stream_user_data,
  nghttp2_data_source_read_callback read_callback,
  ಶೂನ್ಯ *read_ctx)
{
  ಸಂರಚನಾ dynhds h2_headers;
  nghttp2_nv *nva = ರಿಕ್ತ;
  int32_t stream_id = -1;
  size_t nheader;
  CURLcode result;

  (ಶೂನ್ಯ)cf;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_ಎಣಿಕೆ, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  ಯದಿ(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  ಯದಿ(read_callback) {
    nghttp2_data_provider data_prd;

    data_prd.read_callback = read_callback;
    data_prd.source.ptr = read_ctx;
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &data_prd, stream_user_data);
  }
  ಅನ್ಯಥಾ {
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, ರಿಕ್ತ, stream_user_data);
  }

  ಯದಿ(stream_id < 0) {
    failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ out;
  }
  result = CURLE_OK;

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  *pstream_id = stream_id;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode submit_CONNECT(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ tunnel_stream *ts)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  ಸಂರಚನಾ httpreq *req = ರಿಕ್ತ;

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  result = Curl_creader_set_null(data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = proxy_h2_submit(&ts->stream_id, cf, data, &ctx->ngh2, req,
                           ರಿಕ್ತ, ts, tunnel_send_callback, cf);
  ಯದಿ(result) {
    CURL_TRC_CF(data, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->stream_id, nghttp2_strerror(ts->stream_id));
  }

out:
  ಯದಿ(req)
    Curl_http_req_free(req);
  ಯದಿ(result)
    failf(data, "Failed sending CONNECT to proxy");
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode inspect_response(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ tunnel_stream *ts)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)ctx->udp_tunnel, &res);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ವಿಕಲ್ಪ(res) {
  ಪಕ್ಷ PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, data,
                       (bool)ctx->udp_tunnel);
    ಭಙ್ಗ;
  ಪಕ್ಷ PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
    result = CURLE_COULDNT_CONNECT;
    ಭಙ್ಗ;
  ಪಕ್ಷ PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, data,
                       (bool)ctx->udp_tunnel);
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode H2_CONNECT(ಸಂರಚನಾ Curl_cfilter *cf,
                           ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ tunnel_stream *ts)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  ಯದಿ(ctx->conn_closed) {
    failf(data, "proxy closed connection");
    ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
  }

  ಕುರು {
    ವಿಕಲ್ಪ(ts->state) {
    ಪಕ್ಷ H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = submit_CONNECT(cf, data, ts);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, data,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    ಪಕ್ಷ H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      result = proxy_h2_progress_ingress(cf, data);
      ಯದಿ(!result)
        result = proxy_h2_progress_egress(cf, data);
      ಯದಿ(result && result != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                           (bool)ctx->udp_tunnel);
        ಭಙ್ಗ;
      }

      ಯದಿ(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, data,
                           (bool)ctx->udp_tunnel);
      }
      ಅನ್ಯಥಾ {
        result = CURLE_OK;
        ಗಚ್ಛ out;
      }
      FALLTHROUGH();

    ಪಕ್ಷ H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = inspect_response(cf, data, ts);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      ಭಙ್ಗ;

    ಪಕ್ಷ H2_TUNNEL_ESTABLISHED:
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;

    ಪಕ್ಷ H2_TUNNEL_FAILED:
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;

    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }

  } ಯಾವತ್(ts->state == H2_TUNNEL_INIT);

out:
  ಯದಿ((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Initialize the cfilter context
 */
ಸ್ಥಿರ CURLcode cf_h2_proxy_ctx_init(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  nghttp2_session_callbacks *cbs = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, ಪರಿಮಾಣ(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  ಯದಿ(tunnel_stream_init(&ctx->tunnel, ctx->dest))
    ಗಚ್ಛ out;

  rc = nghttp2_session_callbacks_new(&cbs);
  ಯದಿ(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    ಗಚ್ಛ out;
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
  ಯದಿ(rc) {
    failf(data, "Could not initialize nghttp2");
    ಗಚ್ಛ out;
  }

  {
    nghttp2_settings_entry iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].ಮೌಲ್ಯ = Curl_multi_max_concurrent_streams(data->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
    iv[1].ಮೌಲ್ಯ = H2_TUNNEL_WINDOW_SIZE;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ತಳ್ಳು;
    iv[2].ಮೌಲ್ಯ = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    ಯದಿ(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      ಗಚ್ಛ out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_WINDOW_SIZE);
  ಯದಿ(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    ಗಚ್ಛ out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;

out:
  ಯದಿ(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(data, cf, "[0] init proxy ctx -> %d", (ಪೂರ್ಣಾಙ್ಕ)result);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    bool *done)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ cf_call_data save;
  ಸಂರಚನಾ tunnel_stream *ts = &ctx->tunnel;

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  /* Connect the lower filters first */
  ಯದಿ(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ಯದಿ(result || !*done)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  ಯದಿ(!ctx->ngh2.s) {
    result = cf_h2_proxy_ctx_init(cf, data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  DEBUGASSERT(ts->authority);

  ಯದಿ(Curl_timeleft_ms(data) < 0) {
    failf(data, "Proxy CONNECT aborted due to timeout");
    result = CURLE_OPERATION_TIMEDOUT;
    ಗಚ್ಛ out;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  result = H2_CONNECT(cf, data, ts);

out:
  *done = (result == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  ಯದಿ(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_proxy_destroy(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;

  (ಶೂನ್ಯ)data;
  ಯದಿ(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = ರಿಕ್ತ;
  }
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ rv;

  ಯದಿ(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  ಯದಿ(!ctx->sent_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ಧ್ರುವ uint8_t *)"shutdown", ಪರಿಮಾಣ("shutdown"));
    ಯದಿ(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
    ctx->sent_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  ಯದಿ(Curl_ngh2_session_want_write(&ctx->ngh2))
    result = proxy_h2_progress_egress(cf, data);
  ಯದಿ(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = proxy_h2_progress_ingress(cf, data);

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool cf_h2_proxy_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಯದಿ((ctx && !Curl_bufq_is_empty(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಪ್ರತ್ಯಾಗಮ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                           ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  ಯದಿ(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  ಅನ್ಯಥಾ
    Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  ಯದಿ(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
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
                want_recv, want_send, (ಪೂರ್ಣಾಙ್ಕ)result);
    CF_DATA_RESTORE(cf, save);
  }
  ಅನ್ಯಥಾ ಯದಿ(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CURL_TRC_CF(data, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (ಪೂರ್ಣಾಙ್ಕ)result);
    CF_DATA_RESTORE(cf, save);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode h2_handle_tunnel_close(ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       size_t *pnread)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  ಯದಿ(ctx->tunnel.error) {
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.stream_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(data, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.stream_id);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode tunnel_recv(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                            ವರ್ಣ *buf, size_t len, size_t *pnread)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  *pnread = 0;
  ಯದಿ(!Curl_bufq_is_empty(&ctx->tunnel.recvbuf))
    result = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  ಅನ್ಯಥಾ {
    ಯದಿ(ctx->tunnel.closed) {
      result = h2_handle_tunnel_close(cf, data, pnread);
    }
    ಅನ್ಯಥಾ ಯದಿ(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.stream_id)) {
      result = CURLE_RECV_ERROR;
    }
    ಅನ್ಯಥಾ
      result = CURLE_AGAIN;
  }

  CURL_TRC_CF(data, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_recv(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ವರ್ಣ *buf, size_t len,
                                 size_t *pnread)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  CURLcode result;

  *pnread = 0;
  CF_DATA_SAVE(save, cf, data);

  ಯದಿ(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_RECV_ERROR;
    ಗಚ್ಛ out;
  }

  ಯದಿ(Curl_bufq_is_empty(&ctx->tunnel.recvbuf)) {
    result = proxy_h2_progress_ingress(cf, data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  result = tunnel_recv(cf, data, buf, len, pnread);

  ಯದಿ(!result) {
    CURL_TRC_CF(data, cf, "[%d] increase window by %zu",
                ctx->tunnel.stream_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.stream_id, *pnread);
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

out:
  ಯದಿ((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_send(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಧ್ರುವ uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  CURLcode result;

  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  ಯದಿ(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ out;
  }

  ಯದಿ(ctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ out;
  }

  result = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(data, cf, "cf_send(), bufq_write %d, %zu", (ಪೂರ್ಣಾಙ್ಕ)result,
              *pnwritten);
  ಯದಿ(result && (result != CURLE_AGAIN))
    ಗಚ್ಛ out;

  ಯದಿ(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    ಯದಿ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_ingress(cf, data));
  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

  ಯದಿ(!result && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    ಯದಿ(ctx->tunnel.closed) {
      result = CURLE_SEND_ERROR;
    }
    ಅನ್ಯಥಾ {
      CURL_TRC_CF(data, cf, "[0] send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  ಯದಿ((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_flush(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  ಯದಿ(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    ಯದಿ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
  }

  result = proxy_h2_progress_egress(cf, data);

out:
  CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, (ಪೂರ್ಣಾಙ್ಕ)result,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool proxy_h2_connisalive(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 bool *ಒಳಹರಿವು_pending)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ಒಳಹರಿವು_pending = FALSE;
  ಯದಿ(!cf->next || !cf->next->cft->is_alive(cf->next, data, ಒಳಹರಿವು_pending))
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಯದಿ(*ಒಳಹರಿವು_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *ಒಳಹರಿವು_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    ಯದಿ(!result) {
      ಯದಿ(proxy_h2_process_pending_ಒಳಹರಿವು(cf, data))
        /* immediate error, considered dead */
        alive = FALSE;
      ಅನ್ಯಥಾ {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ಪ್ರತ್ಯಾಗಮ alive;
}

ಸ್ಥಿರ bool cf_h2_proxy_is_alive(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 bool *ಒಳಹರಿವು_pending)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  ಸಂರಚನಾ cf_call_data save;

  *ಒಳಹರಿವು_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, data, ಒಳಹರಿವು_pending));
  CURL_TRC_CF(data, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *ಒಳಹರಿವು_pending);
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ alive;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_query(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx = cf->ctx;

  ವಿಕಲ್ಪ(query) {
  ಪಕ್ಷ CF_QUERY_HOST_PORT:
    *pres1 = (ಪೂರ್ಣಾಙ್ಕ)ctx->dest->port;
    *((ಧ್ರುವ ವರ್ಣ **)pres2) = ctx->dest->hostname;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CF_QUERY_NEED_FLUSH: {
    ಯದಿ(!Curl_bufq_is_empty(&ctx->outbufq) ||
       !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(data, cf, "needs flush");
      *pres1 = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಭಙ್ಗ;
  }
  ಪಕ್ಷ CF_QUERY_ALPN_NEGOTIATED: {
    ಧ್ರುವ ವರ್ಣ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_cntrl(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ cf_call_data save;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;

  ವಿಕಲ್ಪ(event) {
  ಪಕ್ಷ CF_CTRL_FLUSH:
    CF_DATA_SAVE(save, cf, data);
    result = cf_h2_proxy_flush(cf, data);
    CF_DATA_RESTORE(cf, save);
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸಂರಚನಾ Curl_cftype Curl_cft_h2_proxy = {
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

CURLcode Curl_cf_h2_proxy_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ Curl_peer *peer,
                                       ಸಂರಚನಾ Curl_peer *dest,
                                       bool udp_tunnel)
{
  ಸಂರಚನಾ Curl_cfilter *cf_h2_proxy = ರಿಕ್ತ;
  ಸಂರಚನಾ cf_h2_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (ಶೂನ್ಯ)data;
  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx)
    ಗಚ್ಛ out;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  result = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ctx = ರಿಕ್ತ;
  Curl_conn_cf_ಸೇರಿಸು_after(cf, cf_h2_proxy);

out:
  cf_h2_proxy_ctx_free(ctx);
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
