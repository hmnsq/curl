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
#include "curl_setup.h"

#གལ་སྲིད !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_NGHTTP2)

#include <nghttp2/nghttp2.h>

#include "urldata.h"
#include "url.h"
#include "cfilters.h"
#include "connect.h"
#include "curl_trc.h"
#include "bufq.h"
#include "curlx/dynbuf.h"
#include "dynhds.h"
#include "http2.h"
#include "http_proxy.h"
#include "multiif.h"
#include "ngh2_shim.h"
#include "sendf.h"
#include "select.h"
#include "cf-h2-proxy.h"

#define PROXY_H2_CHUNK_SIZE  (16 * 1024)

#define PROXY_HTTP2_HUGE_WINDOW_SIZE (100 * 1024 * 1024)
#define H2_TUNNEL_WINDOW_SIZE        (10 * 1024 * 1024)

#define PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define PROXY_H2_NW_SEND_CHUNKS   1

#define H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_WINDOW_SIZE / PROXY_H2_CHUNK_SIZE)
#define H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_SIZE)


typedef ga_nanaa {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

sa_nracanaa tunnel_stream {
  sa_nracanaa http_resp *resp;
  sa_nracanaa bufq recvbuf;
  sa_nracanaa bufq sendbuf;
  var_na *authority;
  int32_t stream_id;
  uint32_t error;
  h2_tunnel_state state;
  BIT(has_final_response);
  BIT(closed);
  BIT(reset);
};

static CURLcode tunnel_stream_init(sa_nracanaa tunnel_stream *ts,
                                   sa_nracanaa Curl_peer *dest)
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
  གལ་སྲིད(!ts->authority)
    ལོག CURLE_OUT_OF_MEMORY;

  ལོག CURLE_OK;
}

static void tunnel_stream_reset(sa_nracanaa tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = NULL;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->stream_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

static void tunnel_stream_clear(sa_nracanaa tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, parimaa_na(*ts));
  ts->state = H2_TUNNEL_INIT;
}

static void h2_tunnel_go_state(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa tunnel_stream *ts,
                               h2_tunnel_state new_state,
                               sa_nracanaa Curl_easy *data,
                               bool udp_tunnel)
{
  (void)cf;
  (void)udp_tunnel;

  གལ་སྲིད(ts->state == new_state)
    ལོག;
  /* leaving this one */
  switch(ts->state) {
  pak_sa H2_TUNNEL_CONNECT:
    data->req.ignorebody = FALSE;
    bha_nga;
  default:
    bha_nga;
  }
  /* entering this one */
  switch(new_state) {
  pak_sa H2_TUNNEL_INIT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'init'", ts->stream_id);
    tunnel_stream_reset(ts);
    bha_nga;

  pak_sa H2_TUNNEL_CONNECT:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'connect'", ts->stream_id);
    ts->state = H2_TUNNEL_CONNECT;
    bha_nga;

  pak_sa H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'response'", ts->stream_id);
    ts->state = H2_TUNNEL_RESPONSE;
    bha_nga;

  pak_sa H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(data, cf, "[%d] new tunnel state 'established'",
                ts->stream_id);
    infof(data, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    data->state.authproxy.done = TRUE;
    data->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  pak_sa H2_TUNNEL_FAILED:
    གལ་སྲིད(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(data, cf, "[%d] new tunnel state 'failed'", ts->stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(data->req.hd_proxy_auth);
    bha_nga;
  }
}

sa_nracanaa cf_h2_proxy_ctx {
  sa_nracanaa curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  sa_nracanaa cf_call_data call_data;

  sa_nracanaa bufq inbufq;  /* network receive buffer */
  sa_nracanaa bufq outbufq; /* network send buffer */

  sa_nracanaa Curl_peer *peer; /* proxy we talk to */
  sa_nracanaa Curl_peer *dest; /* where to tunnel to */
  sa_nracanaa tunnel_stream tunnel; /* our tunnel CONNECT stream */
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
#define CF_CTX_CALL_DATA(cf) ((sa_nracanaa cf_h2_proxy_ctx *)(cf)->ctx)->call_data

static void cf_h2_proxy_ctx_clear(sa_nracanaa cf_h2_proxy_ctx *ctx)
{
  sa_nracanaa cf_call_data save = ctx->call_data;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_stream_clear(&ctx->tunnel);
  memset(ctx, 0, parimaa_na(*ctx));
  ctx->call_data = save;
}

static void cf_h2_proxy_ctx_free(sa_nracanaa cf_h2_proxy_ctx *ctx)
{
  གལ་སྲིད(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

static void drain_tunnel(sa_nracanaa Curl_cfilter *cf,
                         sa_nracanaa Curl_easy *data,
                         sa_nracanaa tunnel_stream *tunnel)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  (void)cf;
  གལ་སྲིད(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_empty(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(data);
}

static CURLcode proxy_h2_nw_out_writer(void *writer_ctx,
                                       const uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  sa_nracanaa Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  གལ་སྲིད(cf) {
    sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
    CURLcode result;
    result = Curl_conn_cf_send(cf->next, data, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(data, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (ཧྲིལ)result, *pnwritten);
    ལོག result;
  }
  ལོག CURLE_FAILED_INIT;
}

static ཧྲིལ proxy_h2_client_new(sa_nracanaa Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { NULL, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  ཧྲིལ rc = nghttp2_option_new(&o);
  གལ་སྲིད(rc)
    ལོག rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#གལ་སྲིད NGHTTP2_VERSION_NUM >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ལོག rc;
}

static ཧྲིལ proxy_h2_should_close_session(sa_nracanaa cf_h2_proxy_ctx *ctx)
{
  ལོག !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

static CURLcode proxy_h2_nw_out_flush(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  གལ་སྲིད(Curl_bufq_is_empty(&ctx->outbufq))
    ལོག CURLE_OK;

  result = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_out_writer, cf,
                          &nwritten);
  གལ་སྲིད(result) {
    གལ་སྲིད(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    ལོག result;
  }
  CURL_TRC_CF(data, cf, "[0] nw send buffer flushed");
  ལོག Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
static CURLcode proxy_h2_process_pending_input(sa_nracanaa Curl_cfilter *cf,
                                               sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  const unsigned var_na *buf;
  size_t blen, nread;
  ssize_t rv;

  སྐབས(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (const uint8_t *)buf, blen);
    CURL_TRC_CF(data, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    གལ་སྲིད(!curlx_sztouz(rv, &nread)) {
      failf(data,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((ཧྲིལ)rv));
      ལོག CURLE_RECV_ERROR;
    }
    དེ་མིན གལ་སྲིད(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      bha_nga;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    གལ་སྲིད(Curl_bufq_is_empty(&ctx->inbufq)) {
      CURL_TRC_CF(data, cf, "[0] all data in connection buffer processed");
      bha_nga;
    }
    དེ་མིན {
      CURL_TRC_CF(data, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  ལོག CURLE_OK;
}

static CURLcode proxy_h2_progress_ingress(sa_nracanaa Curl_cfilter *cf,
                                          sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  གལ་སྲིད(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = proxy_h2_process_pending_input(cf, data);
    གལ་སྲིད(result)
      ལོག result;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  སྐབས(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_empty(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_full(&ctx->tunnel.recvbuf)) {

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (ཧྲིལ)result, nread);
    གལ་སྲིད(result) {
      གལ་སྲིད(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 proxy data");
        ལོག result;
      }
      bha_nga;
    }
    དེ་མིན གལ་སྲིད(nread == 0) {
      CURL_TRC_CF(data, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      bha_nga;
    }

    result = proxy_h2_process_pending_input(cf, data);
    གལ་སྲིད(result)
      ལོག result;
  }

  ལོག CURLE_OK;
}

static CURLcode proxy_h2_progress_egress(sa_nracanaa Curl_cfilter *cf,
                                         sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  ཧྲིལ rv = 0;

  ctx->nw_out_blocked = 0;
  སྐབས(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  གལ་སྲིད(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ལོག CURLE_SEND_ERROR;
  }
  ལོག proxy_h2_nw_out_flush(cf, data);
}

static ssize_t on_session_send(nghttp2_session *h2,
                               const uint8_t *buf, size_t blen, ཧྲིལ flags,
                               void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (void)h2;
  (void)flags;
  DEBUGASSERT(data);

  result = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_out_writer, cf, &nwritten);
  གལ་སྲིད(result) {
    གལ་སྲིད(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      ལོག NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    ལོག NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  གལ་སྲིད(!nwritten)
    ལོག NGHTTP2_ERR_WOULDBLOCK;

  ལོག (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#ifdef CURLVERBOSE
static ཧྲིལ proxy_h2_on_frame_send(nghttp2_session *session,
                                  const nghttp2_frame *frame,
                                  void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);

  (void)session;
  DEBUGASSERT(data);
  གལ་སྲིད(Curl_trc_cf_is_verbose(cf, data)) {
    var_na buffer[256];
    ཧྲིལ len;
    len = Curl_nghttp2_fr_print(frame, buffer, parimaa_na(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  ལོག 0;
}
#endif /* CURLVERBOSE */

static ཧྲིལ proxy_h2_on_frame_recv(nghttp2_session *session,
                                  const nghttp2_frame *frame,
                                  void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;

  (void)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  གལ་སྲིད(Curl_trc_cf_is_verbose(cf, data)) {
    var_na buffer[256];
    ཧྲིལ len;
    len = Curl_nghttp2_fr_print(frame, buffer, parimaa_na(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  གལ་སྲིད(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    switch(frame->hd.type) {
    pak_sa NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      གལ་སྲིད(CURL_REQ_WANT_SEND(data)) {
        drain_tunnel(cf, data, &ctx->tunnel);
      }
      bha_nga;
    pak_sa NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      གལ་སྲིད(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              frame->goaway.error_code, ctx->remote_max_sid);
      }
      bha_nga;
    default:
      bha_nga;
    }
    ལོག 0;
  }

  གལ་སྲིད(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] rcvd FRAME not for tunnel", stream_id);
    ལོག NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  switch(frame->hd.type) {
  pak_sa NGHTTP2_HEADERS:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    གལ་སྲིད(!ctx->tunnel.resp)
      ལོག NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(data, cf, "[%d] got http status: %d",
                stream_id, ctx->tunnel.resp->status);
    གལ་སྲིད(!ctx->tunnel.has_final_response && ctx->tunnel.resp->status / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    bha_nga;
  pak_sa NGHTTP2_WINDOW_UPDATE:
    གལ་སྲིད(CURL_REQ_WANT_SEND(data)) {
      drain_tunnel(cf, data, &ctx->tunnel);
    }
    bha_nga;
  pak_sa NGHTTP2_RST_STREAM:
    གལ་སྲིད(frame->rst_stream.error_code)
      ctx->tunnel.reset = TRUE;
    bha_nga;
  default:
    bha_nga;
  }
  ལོག 0;
}

static ཧྲིལ proxy_h2_on_header(nghttp2_session *session,
                              const nghttp2_frame *frame,
                              const uint8_t *name, size_t namelen,
                              const uint8_t *value, size_t valuelen,
                              uint8_t flags,
                              void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (void)flags;
  (void)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  གལ་སྲིད(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(data, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", stream_id,
                (ཧྲིལ)namelen, name, (ཧྲིལ)valuelen, value);
    ལོག NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  གལ་སྲིད(frame->hd.type == NGHTTP2_PUSH_PROMISE)
    ལོག NGHTTP2_ERR_CALLBACK_FAILURE;

  གལ་སྲིད(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    ལོག 0;
  }

  གལ་སྲིད(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    int32_t http_status;
    sa_nracanaa http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    result = Curl_http_decode_status(&http_status,
                                    (const var_na *)value, valuelen);
    གལ་སྲིད(result)
      ལོག NGHTTP2_ERR_CALLBACK_FAILURE;
    result = Curl_http_resp_make(&resp, http_status, NULL);
    གལ་སྲིད(result)
      ལོག NGHTTP2_ERR_CALLBACK_FAILURE;
    གལ་སྲིད(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream_id, ctx->tunnel.resp->status);
    ལོག 0;
  }

  གལ་སྲིད(!ctx->tunnel.resp)
    ལོག NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_dynhds_add(&ctx->tunnel.resp->headers,
                           (const var_na *)name, namelen,
                           (const var_na *)value, valuelen);
  གལ་སྲིད(result)
    ལོག NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream_id, (ཧྲིལ)namelen, name, (ཧྲིལ)valuelen, value);

  ལོག 0; /* 0 is successful */
}

static ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint8_t *buf, size_t length,
                                    uint32_t *data_flags,
                                    nghttp2_data_source *source,
                                    void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
  sa_nracanaa tunnel_stream *ts;
  CURLcode result;
  size_t nread;

  (void)session;
  (void)source;
  (void)ctx;

  གལ་སྲིད(!stream_id)
    ལོག NGHTTP2_ERR_INVALID_ARGUMENT;

  ts = Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  གལ་སྲིད(!ts)
    ལོག NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  result = Curl_bufq_read(&ts->sendbuf, buf, length, &nread);
  གལ་སྲིད(result) {
    གལ་སྲིད(result != CURLE_AGAIN)
      ལོག NGHTTP2_ERR_CALLBACK_FAILURE;
    ལོག NGHTTP2_ERR_DEFERRED;
  }
  གལ་སྲིད(ts->closed && Curl_bufq_is_empty(&ts->sendbuf))
    *data_flags = NGHTTP2_DATA_FLAG_EOF;

  CURL_TRC_CF(data, cf, "[%d] tunnel_send_callback -> %zu",
              ts->stream_id, nread);
  ལོག (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

static ཧྲིལ tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t stream_id,
                                const uint8_t *mem, size_t len, void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  (void)flags;
  (void)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  གལ་སྲིད(stream_id != ctx->tunnel.stream_id)
    ལོག NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  གལ་སྲིད(result) {
    གལ་སྲིད(result != CURLE_AGAIN)
      ལོག NGHTTP2_ERR_CALLBACK_FAILURE;
#ifdef DEBUGBUILD
    nwritten = 0;
#endif
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  ལོག 0;
}

static ཧྲིལ proxy_h2_on_stream_close(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint32_t error_code, void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);

  (void)session;

  གལ་སྲིད(stream_id != ctx->tunnel.stream_id)
    ལོག 0;

  CURL_TRC_CF(data, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_code;
  གལ་སྲིད(error_code)
    ctx->tunnel.reset = TRUE;

  ལོག 0;
}

static CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  sa_nracanaa Curl_cfilter *cf,
  sa_nracanaa Curl_easy *data,
  sa_nracanaa curl_ngh2 *ngh2,
  sa_nracanaa httpreq *req,
  const nghttp2_priority_spec *pri_spec,
  void *stream_user_data,
  nghttp2_data_source_read_callback read_callback,
  void *read_ctx)
{
  sa_nracanaa dynhds h2_headers;
  nghttp2_nv *nva = NULL;
  int32_t stream_id = -1;
  size_t nheader;
  CURLcode result;

  (void)cf;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_COUNT, DYN_HTTP_REQUEST);
  result = Curl_http_req_to_h2(&h2_headers, req, data);
  གལ་སྲིད(result)
    goto out;

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  གལ་སྲིད(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  གལ་སྲིད(read_callback) {
    nghttp2_data_provider data_prd;

    data_prd.read_callback = read_callback;
    data_prd.source.ptr = read_ctx;
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &data_prd, stream_user_data);
  }
  དེ་མིན {
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, NULL, stream_user_data);
  }

  གལ་སྲིད(stream_id < 0) {
    failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    goto out;
  }
  result = CURLE_OK;

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  *pstream_id = stream_id;
  ལོག result;
}

static CURLcode submit_CONNECT(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               sa_nracanaa tunnel_stream *ts)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result;
  sa_nracanaa httpreq *req = NULL;

  result = Curl_http_proxy_create_tunnel_request(&req, cf, data,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  གལ་སྲིད(result)
    goto out;
  result = Curl_creader_set_null(data);
  གལ་སྲིད(result)
    goto out;

  result = proxy_h2_submit(&ts->stream_id, cf, data, &ctx->ngh2, req,
                           NULL, ts, tunnel_send_callback, cf);
  གལ་སྲིད(result) {
    CURL_TRC_CF(data, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->stream_id, nghttp2_strerror(ts->stream_id));
  }

out:
  གལ་སྲིད(req)
    Curl_http_req_free(req);
  གལ་སྲིད(result)
    failf(data, "Failed sending CONNECT to proxy");
  ལོག result;
}

static CURLcode inspect_response(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa tunnel_stream *ts)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_result res;
  CURLcode result;

  result = Curl_http_proxy_inspect_tunnel_response(
      cf, data, ts->resp, (bool)ctx->udp_tunnel, &res);
  གལ་སྲིད(result)
    ལོག result;
  switch(res) {
  pak_sa PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, data,
                       (bool)ctx->udp_tunnel);
    bha_nga;
  pak_sa PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
    result = CURLE_COULDNT_CONNECT;
    bha_nga;
  pak_sa PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, data,
                       (bool)ctx->udp_tunnel);
    bha_nga;
  }
  ལོག result;
}

static CURLcode H2_CONNECT(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           sa_nracanaa tunnel_stream *ts)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  གལ་སྲིད(ctx->conn_closed) {
    failf(data, "proxy closed connection");
    ལོག CURLE_COULDNT_CONNECT;
  }

  do {
    switch(ts->state) {
    pak_sa H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(data, cf, "[0] CONNECT start for %s", ts->authority);
      result = submit_CONNECT(cf, data, ts);
      གལ་སྲིད(result)
        goto out;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, data,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    pak_sa H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      result = proxy_h2_progress_ingress(cf, data);
      གལ་སྲིད(!result)
        result = proxy_h2_progress_egress(cf, data);
      གལ་སྲིད(result && result != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                           (bool)ctx->udp_tunnel);
        bha_nga;
      }

      གལ་སྲིད(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, data,
                           (bool)ctx->udp_tunnel);
      }
      དེ་མིན {
        result = CURLE_OK;
        goto out;
      }
      FALLTHROUGH();

    pak_sa H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      result = inspect_response(cf, data, ts);
      གལ་སྲིད(result)
        goto out;
      bha_nga;

    pak_sa H2_TUNNEL_ESTABLISHED:
      ལོག CURLE_OK;

    pak_sa H2_TUNNEL_FAILED:
      ལོག CURLE_RECV_ERROR;

    default:
      bha_nga;
    }

  } སྐབས(ts->state == H2_TUNNEL_INIT);

out:
  གལ་སྲིད((result && (result != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, data,
                       (bool)ctx->udp_tunnel);
  ལོག result;
}

/*
 * Initialize the cfilter context
 */
static CURLcode cf_h2_proxy_ctx_init(sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  nghttp2_session_callbacks *cbs = NULL;
  ཧྲིལ rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, parimaa_na(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_SIZE, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  གལ་སྲིད(tunnel_stream_init(&ctx->tunnel, ctx->dest))
    goto out;

  rc = nghttp2_session_callbacks_new(&cbs);
  གལ་སྲིད(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    goto out;
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
  གལ་སྲིད(rc) {
    failf(data, "Could not initialize nghttp2");
    goto out;
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
    གལ་སྲིད(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      goto out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_WINDOW_SIZE);
  གལ་སྲིད(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    goto out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;

out:
  གལ་སྲིད(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(data, cf, "[0] init proxy ctx -> %d", (ཧྲིལ)result);
  ལོག result;
}

static CURLcode cf_h2_proxy_connect(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    bool *done)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  sa_nracanaa cf_call_data save;
  sa_nracanaa tunnel_stream *ts = &ctx->tunnel;

  གལ་སྲིད(cf->connected) {
    *done = TRUE;
    ལོག CURLE_OK;
  }

  /* Connect the lower filters first */
  གལ་སྲིད(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    གལ་སྲིད(result || !*done)
      ལོག result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  གལ་སྲིད(!ctx->ngh2.s) {
    result = cf_h2_proxy_ctx_init(cf, data);
    གལ་སྲིད(result)
      goto out;
  }
  DEBUGASSERT(ts->authority);

  གལ་སྲིད(Curl_timeleft_ms(data) < 0) {
    failf(data, "Proxy CONNECT aborted due to timeout");
    result = CURLE_OPERATION_TIMEDOUT;
    goto out;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  result = H2_CONNECT(cf, data, ts);

out:
  *done = (result == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  གལ་སྲིད(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&data->req, data);
    Curl_client_reset(data);
  }
  CF_DATA_RESTORE(cf, save);
  ལོག result;
}

static void cf_h2_proxy_destroy(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;

  (void)data;
  གལ་སྲིད(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = NULL;
  }
}

static CURLcode cf_h2_proxy_shutdown(sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  sa_nracanaa cf_call_data save;
  CURLcode result;
  ཧྲིལ rv;

  གལ་སྲིད(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    ལོག CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  གལ་སྲིད(!ctx->sent_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (const uint8_t *)"shutdown", parimaa_na("shutdown"));
    གལ་སྲིད(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      goto out;
    }
    ctx->sent_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  གལ་སྲིད(Curl_ngh2_session_want_write(&ctx->ngh2))
    result = proxy_h2_progress_egress(cf, data);
  གལ་སྲིད(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = proxy_h2_progress_ingress(cf, data);

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  ལོག result;
}

static bool cf_h2_proxy_data_pending(sa_nracanaa Curl_cfilter *cf,
                                     const sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  གལ་སྲིད((ctx && !Curl_bufq_is_empty(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    ལོག TRUE;
  ལོག cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

static CURLcode cf_h2_proxy_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                           sa_nracanaa Curl_easy *data,
                                           sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  sa_nracanaa cf_call_data save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  གལ་སྲིད(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  དེ་མིན
    Curl_pollset_check(data, ps, sock, &want_recv, &want_send);

  གལ་སྲིད(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
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
                want_recv, want_send, (ཧྲིལ)result);
    CF_DATA_RESTORE(cf, save);
  }
  དེ་མིན གལ་སྲིད(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CURL_TRC_CF(data, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (ཧྲིལ)result);
    CF_DATA_RESTORE(cf, save);
  }
  ལོག result;
}

static CURLcode h2_handle_tunnel_close(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data,
                                       size_t *pnread)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  གལ་སྲིད(ctx->tunnel.error) {
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.stream_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    ལོག CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(data, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.stream_id);
  ལོག CURLE_OK;
}

static CURLcode tunnel_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                            var_na *buf, size_t len, size_t *pnread)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  *pnread = 0;
  གལ་སྲིད(!Curl_bufq_is_empty(&ctx->tunnel.recvbuf))
    result = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  དེ་མིན {
    གལ་སྲིད(ctx->tunnel.closed) {
      result = h2_handle_tunnel_close(cf, data, pnread);
    }
    དེ་མིན གལ་སྲིད(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.stream_id)) {
      result = CURLE_RECV_ERROR;
    }
    དེ་མིན
      result = CURLE_AGAIN;
  }

  CURL_TRC_CF(data, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (ཧྲིལ)result, *pnread);
  ལོག result;
}

static CURLcode cf_h2_proxy_recv(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 var_na *buf, size_t len,
                                 size_t *pnread)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  sa_nracanaa cf_call_data save;
  CURLcode result;

  *pnread = 0;
  CF_DATA_SAVE(save, cf, data);

  གལ་སྲིད(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_RECV_ERROR;
    goto out;
  }

  གལ་སྲིད(Curl_bufq_is_empty(&ctx->tunnel.recvbuf)) {
    result = proxy_h2_progress_ingress(cf, data);
    གལ་སྲིད(result)
      goto out;
  }

  result = tunnel_recv(cf, data, buf, len, pnread);

  གལ་སྲིད(!result) {
    CURL_TRC_CF(data, cf, "[%d] increase window by %zu",
                ctx->tunnel.stream_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.stream_id, *pnread);
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

out:
  གལ་སྲིད((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (ཧྲིལ)result, *pnread);
  CF_DATA_RESTORE(cf, save);
  ལོག result;
}

static CURLcode cf_h2_proxy_send(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 const uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  sa_nracanaa cf_call_data save;
  CURLcode result;

  (void)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  གལ་སྲིད(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    result = CURLE_SEND_ERROR;
    goto out;
  }

  གལ་སྲིད(ctx->tunnel.closed) {
    result = CURLE_SEND_ERROR;
    goto out;
  }

  result = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(data, cf, "cf_send(), bufq_write %d, %zu", (ཧྲིལ)result,
              *pnwritten);
  གལ་སྲིད(result && (result != CURLE_AGAIN))
    goto out;

  གལ་སྲིད(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    གལ་སྲིད(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      goto out;
    }
  }

  result = Curl_1st_fatal(result, proxy_h2_progress_ingress(cf, data));
  result = Curl_1st_fatal(result, proxy_h2_progress_egress(cf, data));

  གལ་སྲིད(!result && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    གལ་སྲིད(ctx->tunnel.closed) {
      result = CURLE_SEND_ERROR;
    }
    དེ་མིན {
      CURL_TRC_CF(data, cf, "[0] send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  གལ་སྲིད((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!result || (result == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, data, &ctx->tunnel);
  }
  CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, len, (ཧྲིལ)result, *pnwritten,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  ལོག result;
}

static CURLcode cf_h2_proxy_flush(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  sa_nracanaa cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  གལ་སྲིད(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    གལ་སྲིད(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, ctx->tunnel.stream_id))) {
      result = CURLE_SEND_ERROR;
      goto out;
    }
  }

  result = proxy_h2_progress_egress(cf, data);

out:
  CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, (ཧྲིལ)result,
              Curl_ngh2_session_get_stream_remote_window_size(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_DATA_RESTORE(cf, save);
  ལོག result;
}

static bool proxy_h2_connisalive(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 bool *input_pending)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *input_pending = FALSE;
  གལ་སྲིད(!cf->next || !cf->next->cft->is_alive(cf->next, data, input_pending))
    ལོག FALSE;

  གལ་སྲིད(*input_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *input_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    གལ་སྲིད(!result) {
      གལ་སྲིད(proxy_h2_process_pending_input(cf, data))
        /* immediate error, considered dead */
        alive = FALSE;
      དེ་མིན {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    དེ་མིན གལ་སྲིད(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ལོག alive;
}

static bool cf_h2_proxy_is_alive(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 bool *input_pending)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  sa_nracanaa cf_call_data save;

  *input_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, data, input_pending));
  CURL_TRC_CF(data, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *input_pending);
  CF_DATA_RESTORE(cf, save);
  ལོག alive;
}

static CURLcode cf_h2_proxy_query(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  ཧྲིལ query, ཧྲིལ *pres1, void *pres2)
{
  sa_nracanaa cf_h2_proxy_ctx *ctx = cf->ctx;

  switch(query) {
  pak_sa CF_QUERY_HOST_PORT:
    *pres1 = (ཧྲིལ)ctx->dest->port;
    *((const var_na **)pres2) = ctx->dest->hostname;
    ལོག CURLE_OK;
  pak_sa CF_QUERY_NEED_FLUSH: {
    གལ་སྲིད(!Curl_bufq_is_empty(&ctx->outbufq) ||
       !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(data, cf, "needs flush");
      *pres1 = TRUE;
      ལོག CURLE_OK;
    }
    bha_nga;
  }
  pak_sa CF_QUERY_ALPN_NEGOTIATED: {
    const var_na **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = NULL;
    ལོག CURLE_OK;
  }
  default:
    bha_nga;
  }
  ལོག cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static CURLcode cf_h2_proxy_cntrl(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  ཧྲིལ event, ཧྲིལ arg1, void *arg2)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa cf_call_data save;

  (void)arg1;
  (void)arg2;

  switch(event) {
  pak_sa CF_CTRL_FLUSH:
    CF_DATA_SAVE(save, cf, data);
    result = cf_h2_proxy_flush(cf, data);
    CF_DATA_RESTORE(cf, save);
    bha_nga;
  default:
    bha_nga;
  }
  ལོག result;
}

sa_nracanaa Curl_cftype Curl_cft_h2_proxy = {
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

CURLcode Curl_cf_h2_proxy_insert_after(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data,
                                       sa_nracanaa Curl_peer *peer,
                                       sa_nracanaa Curl_peer *dest,
                                       bool udp_tunnel)
{
  sa_nracanaa Curl_cfilter *cf_h2_proxy = NULL;
  sa_nracanaa cf_h2_proxy_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (void)data;
  ctx = curlx_calloc(1, parimaa_na(*ctx));
  གལ་སྲིད(!ctx)
    goto out;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  result = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  གལ་སྲིད(result)
    goto out;
  ctx = NULL;
  Curl_conn_cf_insert_after(cf, cf_h2_proxy);

out:
  cf_h2_proxy_ctx_free(ctx);
  ལོག result;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
