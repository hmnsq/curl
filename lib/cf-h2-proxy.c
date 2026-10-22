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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_NGHTTP2)

#ਅਨ੍ਤਰ੍ਭਾਵ <nghttp2/nghttp2.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dynhds.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ngh2_shim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-h2-proxy.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_H2_CHUNK_ਮਾਪ  (16 * 1024)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_HTTP2_HUGE_ਖਿੜਕੀ_ਮਾਪ (100 * 1024 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_TUNNEL_ਖਿੜਕੀ_ਮਾਪ        (10 * 1024 * 1024)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_ਖਿੜਕੀ_ਮਾਪ / PROXY_H2_CHUNK_ਮਾਪ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROXY_H2_NW_SEND_CHUNKS   1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_ਖਿੜਕੀ_ਮਾਪ / PROXY_H2_CHUNK_ਮਾਪ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_ਮਾਪ)


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

ਬਣਤਰ tunnel_ਧਾਰਾ {
  ਬਣਤਰ http_resp *resp;
  ਬਣਤਰ bufq recvbuf;
  ਬਣਤਰ bufq sendbuf;
  ਅੱਖਰ *authority;
  int32_t ਧਾਰਾ_id;
  uint32_t error;
  h2_tunnel_state state;
  ਦੋਆਧਾਰੀ_ਅੰਕ(has_final_response);
  ਦੋਆਧਾਰੀ_ਅੰਕ(closed);
  ਦੋਆਧਾਰੀ_ਅੰਕ(reset);
};

ਸਥਿਰ CURLcode tunnel_ਧਾਰਾ_init(ਬਣਤਰ tunnel_ਧਾਰਾ *ts,
                                   ਬਣਤਰ Curl_peer *dest)
{
  ts->state = H2_TUNNEL_INIT;
  ts->ਧਾਰਾ_id = -1;
  Curl_bufq_init2(&ts->recvbuf, PROXY_H2_CHUNK_ਮਾਪ, H2_TUNNEL_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);
  Curl_bufq_init(&ts->sendbuf, PROXY_H2_CHUNK_ਮਾਪ, H2_TUNNEL_SEND_CHUNKS);

  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->ਪੋਰਟ);
  ਜੇ(!ts->authority)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ tunnel_ਧਾਰਾ_reset(ਬਣਤਰ tunnel_ਧਾਰਾ *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = ਨਿਰਾ;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->ਧਾਰਾ_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

ਸਥਿਰ ਖਾਲੀ tunnel_ਧਾਰਾ_clear(ਬਣਤਰ tunnel_ਧਾਰਾ *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, ਆਕਾਰ(*ts));
  ts->state = H2_TUNNEL_INIT;
}

ਸਥਿਰ ਖਾਲੀ h2_tunnel_go_state(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ tunnel_ਧਾਰਾ *ts,
                               h2_tunnel_state new_state,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               bool udp_tunnel)
{
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)udp_tunnel;

  ਜੇ(ts->state == new_state)
    ਵਾਪਸ;
  /* leaving this one */
  ਚੋਣ(ts->state) {
  ਹਾਲਤ H2_TUNNEL_CONNECT:
    ਅੰਕੜੇ->req.ignorebody = FALSE;
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  /* entering this one */
  ਚੋਣ(new_state) {
  ਹਾਲਤ H2_TUNNEL_INIT:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] new tunnel state 'init'", ts->ਧਾਰਾ_id);
    tunnel_ਧਾਰਾ_reset(ts);
    ਰੋਕੋ;

  ਹਾਲਤ H2_TUNNEL_CONNECT:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] new tunnel state 'connect'", ts->ਧਾਰਾ_id);
    ts->state = H2_TUNNEL_CONNECT;
    ਰੋਕੋ;

  ਹਾਲਤ H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] new tunnel state 'response'", ts->ਧਾਰਾ_id);
    ts->state = H2_TUNNEL_RESPONSE;
    ਰੋਕੋ;

  ਹਾਲਤ H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] new tunnel state 'established'",
                ts->ਧਾਰਾ_id);
    infof(ਅੰਕੜੇ, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    ਅੰਕੜੇ->state.authproxy.ਪੂਰਾ = TRUE;
    ਅੰਕੜੇ->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  ਹਾਲਤ H2_TUNNEL_FAILED:
    ਜੇ(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] new tunnel state 'failed'", ts->ਧਾਰਾ_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(ਅੰਕੜੇ->req.hd_proxy_auth);
    ਰੋਕੋ;
  }
}

ਬਣਤਰ cf_h2_proxy_ctx {
  ਬਣਤਰ curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ਬਣਤਰ cf_call_ਅੰਕੜੇ call_ਅੰਕੜੇ;

  ਬਣਤਰ bufq inbufq;  /* network receive buffer */
  ਬਣਤਰ bufq outbufq; /* network send buffer */

  ਬਣਤਰ Curl_peer *peer; /* proxy we talk to */
  ਬਣਤਰ Curl_peer *dest; /* where to tunnel to */
  ਬਣਤਰ tunnel_ਧਾਰਾ tunnel; /* our tunnel CONNECT stream */
  int32_t goaway_error;
  int32_t remote_max_sid;
  ਦੋਆਧਾਰੀ_ਅੰਕ(conn_closed);
  ਦੋਆਧਾਰੀ_ਅੰਕ(rcvd_goaway);
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਭੇਜਿਆ_goaway);
  ਦੋਆਧਾਰੀ_ਅੰਕ(nw_ਬਾਹਰਲਾ_blocked);
  ਦੋਆਧਾਰੀ_ਅੰਕ(udp_tunnel);
};

/* How to access `call_data` from a cf_h2 filter */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ CF_CTX_CALL_ਅੰਕੜੇ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CF_CTX_CALL_ਅੰਕੜੇ(cf) ((ਬਣਤਰ cf_h2_proxy_ctx *)(cf)->ctx)->call_ਅੰਕੜੇ

ਸਥਿਰ ਖਾਲੀ cf_h2_proxy_ctx_clear(ਬਣਤਰ cf_h2_proxy_ctx *ctx)
{
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save = ctx->call_ਅੰਕੜੇ;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_ਧਾਰਾ_clear(&ctx->tunnel);
  memset(ctx, 0, ਆਕਾਰ(*ctx));
  ctx->call_ਅੰਕੜੇ = save;
}

ਸਥਿਰ ਖਾਲੀ cf_h2_proxy_ctx_free(ਬਣਤਰ cf_h2_proxy_ctx *ctx)
{
  ਜੇ(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

ਸਥਿਰ ਖਾਲੀ drain_tunnel(ਬਣਤਰ Curl_cfilter *cf,
                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ tunnel_ਧਾਰਾ *tunnel)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  (ਖਾਲੀ)cf;
  ਜੇ(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(ਅੰਕੜੇ);
}

ਸਥਿਰ CURLcode proxy_h2_nw_ਬਾਹਰਲਾ_writer(ਖਾਲੀ *writer_ctx,
                                       ਅਚਲ uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  ਬਣਤਰ Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  ਜੇ(cf) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
    CURLcode ਨਤੀਜਾ;
    ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten);
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ CURLE_FAILED_INIT;
}

ਸਥਿਰ ਪੂਰਨ proxy_h2_client_new(ਬਣਤਰ Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ਨਿਰਾ, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  ਪੂਰਨ rc = nghttp2_option_new(&o);
  ਜੇ(rc)
    ਵਾਪਸ rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_ਸੈੱਟ_no_auto_ਖਿੜਕੀ_update(o, 1);
#ਜੇ NGHTTP2_ਸੰਸਕਰਣ_ਅੰਕ >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_ਸੈੱਟ_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#ਜੇ_ਅੰਤ
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ proxy_h2_should_close_session(ਬਣਤਰ cf_h2_proxy_ctx *ctx)
{
  ਵਾਪਸ !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

ਸਥਿਰ CURLcode proxy_h2_nw_ਬਾਹਰਲਾ_flush(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->outbufq))
    ਵਾਪਸ CURLE_OK;

  ਨਤੀਜਾ = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_ਬਾਹਰਲਾ_writer, cf,
                          &nwritten);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_ਬਾਹਰਲਾ_blocked = 1;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] nw send buffer flushed");
  ਵਾਪਸ Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ਸਥਿਰ CURLcode proxy_h2_process_pending_ਨਿਵੇਸ਼(ਬਣਤਰ Curl_cfilter *cf,
                                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf;
  size_t blen, nread;
  ssize_t rv;

  ਜਦੋਂ(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ਅਚਲ uint8_t *)buf, blen);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    ਜੇ(!curlx_sztouz(rv, &nread)) {
      failf(ਅੰਕੜੇ,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((ਪੂਰਨ)rv));
      ਵਾਪਸ CURLE_RECV_ERROR;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      ਰੋਕੋ;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->inbufq)) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] all data in connection buffer processed");
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode proxy_h2_ਤਰੱਕੀ_ingress(ਬਣਤਰ Curl_cfilter *cf,
                                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->inbufq)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    ਨਤੀਜਾ = proxy_h2_process_pending_ਨਿਵੇਸ਼(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  ਜਦੋਂ(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_ਭਰਿਆ(&ctx->tunnel.recvbuf)) {

    ਨਤੀਜਾ = Curl_cf_recv_bufq(cf->ਅਗਲਾ, ਅੰਕੜੇ, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (ਪੂਰਨ)ਨਤੀਜਾ, nread);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN) {
        failf(ਅੰਕੜੇ, "Failed receiving HTTP2 proxy data");
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(nread == 0) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      ਰੋਕੋ;
    }

    ਨਤੀਜਾ = proxy_h2_process_pending_ਨਿਵੇਸ਼(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode proxy_h2_ਤਰੱਕੀ_egress(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  ਪੂਰਨ rv = 0;

  ctx->nw_ਬਾਹਰਲਾ_blocked = 0;
  ਜਦੋਂ(!rv && !ctx->nw_ਬਾਹਰਲਾ_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  ਜੇ(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ਵਾਪਸ CURLE_SEND_ERROR;
  }
  ਵਾਪਸ proxy_h2_nw_ਬਾਹਰਲਾ_flush(cf, ਅੰਕੜੇ);
}

ਸਥਿਰ ssize_t on_session_send(nghttp2_session *h2,
                               ਅਚਲ uint8_t *buf, size_t blen, ਪੂਰਨ flags,
                               ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  size_t nwritten;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)h2;
  (ਖਾਲੀ)flags;
  DEBUGASSERT(ਅੰਕੜੇ);

  ਨਤੀਜਾ = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_ਬਾਹਰਲਾ_writer, cf, &nwritten);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      ctx->nw_ਬਾਹਰਲਾ_blocked = 1;
      ਵਾਪਸ NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(ਅੰਕੜੇ, "Failed sending HTTP2 data");
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ਜੇ(!nwritten)
    ਵਾਪਸ NGHTTP2_ERR_WOULDBLOCK;

  ਵਾਪਸ (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਸਥਿਰ ਪੂਰਨ proxy_h2_on_ਚੌਖਟਾ_send(nghttp2_session *session,
                                  ਅਚਲ nghttp2_ਚੌਖਟਾ *ਚੌਖਟਾ,
                                  ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);

  (ਖਾਲੀ)session;
  DEBUGASSERT(ਅੰਕੜੇ);
  ਜੇ(Curl_trc_cf_is_verbose(cf, ਅੰਕੜੇ)) {
    ਅੱਖਰ ਬਫਰ[256];
    ਪੂਰਨ len;
    len = Curl_nghttp2_fr_print(ਚੌਖਟਾ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ) - 1);
    ਬਫਰ[len] = 0;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] -> %s", ਚੌਖਟਾ->hd.ਧਾਰਾ_id, ਬਫਰ);
  }
  ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

ਸਥਿਰ ਪੂਰਨ proxy_h2_on_ਚੌਖਟਾ_recv(nghttp2_session *session,
                                  ਅਚਲ nghttp2_ਚੌਖਟਾ *ਚੌਖਟਾ,
                                  ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  int32_t ਧਾਰਾ_id = ਚੌਖਟਾ->hd.ਧਾਰਾ_id;

  (ਖਾਲੀ)session;
  DEBUGASSERT(ਅੰਕੜੇ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਜੇ(Curl_trc_cf_is_verbose(cf, ਅੰਕੜੇ)) {
    ਅੱਖਰ ਬਫਰ[256];
    ਪੂਰਨ len;
    len = Curl_nghttp2_fr_print(ਚੌਖਟਾ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ) - 1);
    ਬਫਰ[len] = 0;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] <- %s", ਚੌਖਟਾ->hd.ਧਾਰਾ_id, ਬਫਰ);
  }
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

  ਜੇ(!ਧਾਰਾ_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(ਅੰਕੜੇ);
    ਚੋਣ(ਚੌਖਟਾ->hd.type) {
    ਹਾਲਤ NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      ਜੇ(CURL_REQ_WANT_SEND(ਅੰਕੜੇ)) {
        drain_tunnel(cf, ਅੰਕੜੇ, &ctx->tunnel);
      }
      ਰੋਕੋ;
    ਹਾਲਤ NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = ਚੌਖਟਾ->goaway.ਆਖਰੀ_ਧਾਰਾ_id;
      ਜੇ(ਅੰਕੜੇ) {
        infof(ਅੰਕੜੇ, "received GOAWAY, error=%u, last_stream=%d",
              ਚੌਖਟਾ->goaway.error_ਕੋਡ, ctx->remote_max_sid);
      }
      ਰੋਕੋ;
    ਮੂਲ:
      ਰੋਕੋ;
    }
    ਵਾਪਸ 0;
  }

  ਜੇ(ਧਾਰਾ_id != ctx->tunnel.ਧਾਰਾ_id) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] rcvd FRAME not for tunnel", ਧਾਰਾ_id);
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ਚੋਣ(ਚੌਖਟਾ->hd.type) {
  ਹਾਲਤ NGHTTP2_ਸਿਰਲੇਖਾਂ:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    ਜੇ(!ctx->tunnel.resp)
      ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] got http status: %d",
                ਧਾਰਾ_id, ctx->tunnel.resp->ਸਥਿਤੀ);
    ਜੇ(!ctx->tunnel.has_final_response && ctx->tunnel.resp->ਸਥਿਤੀ / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    ਰੋਕੋ;
  ਹਾਲਤ NGHTTP2_ਖਿੜਕੀ_UPDATE:
    ਜੇ(CURL_REQ_WANT_SEND(ਅੰਕੜੇ)) {
      drain_tunnel(cf, ਅੰਕੜੇ, &ctx->tunnel);
    }
    ਰੋਕੋ;
  ਹਾਲਤ NGHTTP2_RST_ਧਾਰਾ:
    ਜੇ(ਚੌਖਟਾ->rst_ਧਾਰਾ.error_ਕੋਡ)
      ctx->tunnel.reset = TRUE;
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ proxy_h2_on_ਸਿਰਲੇਖ(nghttp2_session *session,
                              ਅਚਲ nghttp2_ਚੌਖਟਾ *ਚੌਖਟਾ,
                              ਅਚਲ uint8_t *name, size_t namelen,
                              ਅਚਲ uint8_t *ਮੁੱਲ, size_t valuelen,
                              uint8_t flags,
                              ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  int32_t ਧਾਰਾ_id = ਚੌਖਟਾ->hd.ਧਾਰਾ_id;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)flags;
  (ਖਾਲੀ)session;
  DEBUGASSERT(ਧਾਰਾ_id); /* should never be a zero stream ID here */
  ਜੇ(ਧਾਰਾ_id != ctx->tunnel.ਧਾਰਾ_id) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", ਧਾਰਾ_id,
                (ਪੂਰਨ)namelen, name, (ਪੂਰਨ)valuelen, ਮੁੱਲ);
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ਜੇ(ਚੌਖਟਾ->hd.type == NGHTTP2_ਧੱਕੋ_PROMISE)
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;

  ਜੇ(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    ਵਾਪਸ 0;
  }

  ਜੇ(namelen == CURL_CSTRLEN(HTTP_PSEUDO_ਸਥਿਤੀ) &&
     !memcmp(HTTP_PSEUDO_ਸਥਿਤੀ, name, namelen)) {
    int32_t http_ਸਥਿਤੀ;
    ਬਣਤਰ http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    ਨਤੀਜਾ = Curl_http_decode_ਸਥਿਤੀ(&http_ਸਥਿਤੀ,
                                    (ਅਚਲ ਅੱਖਰ *)ਮੁੱਲ, valuelen);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
    ਨਤੀਜਾ = Curl_http_resp_make(&resp, http_ਸਥਿਤੀ, ਨਿਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
    ਜੇ(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] status: HTTP/2 %03d",
                ਧਾਰਾ_id, ctx->tunnel.resp->ਸਥਿਤੀ);
    ਵਾਪਸ 0;
  }

  ਜੇ(!ctx->tunnel.resp)
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;

  ਨਤੀਜਾ = Curl_dynhds_ਜੋੜੋ(&ctx->tunnel.resp->ਸਿਰਲੇਖਾਂ,
                           (ਅਚਲ ਅੱਖਰ *)name, namelen,
                           (ਅਚਲ ਅੱਖਰ *)ਮੁੱਲ, valuelen);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] header: %.*s: %.*s",
              ਧਾਰਾ_id, (ਪੂਰਨ)namelen, name, (ਪੂਰਨ)valuelen, ਮੁੱਲ);

  ਵਾਪਸ 0; /* 0 is successful */
}

ਸਥਿਰ ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t ਧਾਰਾ_id,
                                    uint8_t *buf, size_t ਲੰਮਾਈ,
                                    uint32_t *ਅੰਕੜੇ_flags,
                                    nghttp2_ਅੰਕੜੇ_ਸਰੋਤ *ਸਰੋਤ,
                                    ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  ਬਣਤਰ tunnel_ਧਾਰਾ *ts;
  CURLcode ਨਤੀਜਾ;
  size_t nread;

  (ਖਾਲੀ)session;
  (ਖਾਲੀ)ਸਰੋਤ;
  (ਖਾਲੀ)ctx;

  ਜੇ(!ਧਾਰਾ_id)
    ਵਾਪਸ NGHTTP2_ERR_INVALID_ਦਲੀਲ;

  ts = Curl_ngh2_session_get_ਧਾਰਾ_user_ਅੰਕੜੇ(&ctx->ngh2, ਧਾਰਾ_id);
  ਜੇ(!ts)
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  ਨਤੀਜਾ = Curl_bufq_read(&ts->sendbuf, buf, ਲੰਮਾਈ, &nread);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN)
      ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
    ਵਾਪਸ NGHTTP2_ERR_DEFERRED;
  }
  ਜੇ(ts->closed && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ts->sendbuf))
    *ਅੰਕੜੇ_flags = NGHTTP2_ਅੰਕੜੇ_FLAG_EOF;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] tunnel_send_callback -> %zu",
              ts->ਧਾਰਾ_id, nread);
  ਵਾਪਸ (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

ਸਥਿਰ ਪੂਰਨ tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t ਧਾਰਾ_id,
                                ਅਚਲ uint8_t *mem, size_t len, ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)flags;
  (ਖਾਲੀ)session;
  DEBUGASSERT(ਧਾਰਾ_id); /* should never be a zero stream ID here */

  ਜੇ(ਧਾਰਾ_id != ctx->tunnel.ਧਾਰਾ_id)
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;

  ਨਤੀਜਾ = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN)
      ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    nwritten = 0;
#ਜੇ_ਅੰਤ
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ proxy_h2_on_ਧਾਰਾ_close(nghttp2_session *session,
                                    int32_t ਧਾਰਾ_id,
                                    uint32_t error_ਕੋਡ, ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);

  (ਖਾਲੀ)session;

  ਜੇ(ਧਾਰਾ_id != ctx->tunnel.ਧਾਰਾ_id)
    ਵਾਪਸ 0;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              ਧਾਰਾ_id, nghttp2_http2_strerror(error_ਕੋਡ), error_ਕੋਡ);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_ਕੋਡ;
  ਜੇ(error_ਕੋਡ)
    ctx->tunnel.reset = TRUE;

  ਵਾਪਸ 0;
}

ਸਥਿਰ CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  ਬਣਤਰ Curl_cfilter *cf,
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਬਣਤਰ curl_ngh2 *ngh2,
  ਬਣਤਰ httpreq *req,
  ਅਚਲ nghttp2_ਤਰਜੀਹ_spec *pri_spec,
  ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ,
  nghttp2_ਅੰਕੜੇ_ਸਰੋਤ_read_callback read_callback,
  ਖਾਲੀ *read_ctx)
{
  ਬਣਤਰ dynhds h2_ਸਿਰਲੇਖਾਂ;
  nghttp2_nv *nva = ਨਿਰਾ;
  int32_t ਧਾਰਾ_id = -1;
  size_t nheader;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)cf;
  Curl_dynhds_init(&h2_ਸਿਰਲੇਖਾਂ, MAX_HTTP_RESP_ਸਿਰਲੇਖ_ਗਿਣਤੀ, DYN_HTTP_REQUEST);
  ਨਤੀਜਾ = Curl_http_req_ਮੰਜ਼ਿਲ_h2(&h2_ਸਿਰਲੇਖਾਂ, req, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  nva = Curl_dynhds_ਮੰਜ਼ਿਲ_nva(&h2_ਸਿਰਲੇਖਾਂ, &nheader);
  ਜੇ(!nva) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(read_callback) {
    nghttp2_ਅੰਕੜੇ_provider ਅੰਕੜੇ_prd;

    ਅੰਕੜੇ_prd.read_callback = read_callback;
    ਅੰਕੜੇ_prd.ਸਰੋਤ.ptr = read_ctx;
    ਧਾਰਾ_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &ਅੰਕੜੇ_prd, ਧਾਰਾ_user_ਅੰਕੜੇ);
  }
  ਨਹੀਂਤਾਂ {
    ਧਾਰਾ_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, ਨਿਰਾ, ਧਾਰਾ_user_ਅੰਕੜੇ);
  }

  ਜੇ(ਧਾਰਾ_id < 0) {
    failf(ਅੰਕੜੇ, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(ਧਾਰਾ_id), ਧਾਰਾ_id);
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  curlx_free(nva);
  Curl_dynhds_free(&h2_ਸਿਰਲੇਖਾਂ);
  *pstream_id = ਧਾਰਾ_id;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode submit_CONNECT(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ tunnel_ਧਾਰਾ *ts)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ httpreq *req = ਨਿਰਾ;

  ਨਤੀਜਾ = Curl_http_proxy_create_tunnel_request(&req, cf, ਅੰਕੜੇ,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਨਤੀਜਾ = Curl_creader_ਸੈੱਟ_null(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = proxy_h2_submit(&ts->ਧਾਰਾ_id, cf, ਅੰਕੜੇ, &ctx->ngh2, req,
                           ਨਿਰਾ, ts, tunnel_send_callback, cf);
  ਜੇ(ਨਤੀਜਾ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->ਧਾਰਾ_id, nghttp2_strerror(ts->ਧਾਰਾ_id));
  }

ਬਾਹਰਲਾ:
  ਜੇ(req)
    Curl_http_req_free(req);
  ਜੇ(ਨਤੀਜਾ)
    failf(ਅੰਕੜੇ, "Failed sending CONNECT to proxy");
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode inspect_response(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ tunnel_ਧਾਰਾ *ts)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_ਨਤੀਜਾ res;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_http_proxy_inspect_tunnel_response(
      cf, ਅੰਕੜੇ, ts->resp, (bool)ctx->udp_tunnel, &res);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  ਚੋਣ(res) {
  ਹਾਲਤ PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, ਅੰਕੜੇ,
                       (bool)ctx->udp_tunnel);
    ਰੋਕੋ;
  ਹਾਲਤ PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, ਅੰਕੜੇ,
                       (bool)ctx->udp_tunnel);
    ਨਤੀਜਾ = CURLE_COULDNT_CONNECT;
    ਰੋਕੋ;
  ਹਾਲਤ PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, ਅੰਕੜੇ,
                       (bool)ctx->udp_tunnel);
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode H2_CONNECT(ਬਣਤਰ Curl_cfilter *cf,
                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ tunnel_ਧਾਰਾ *ts)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  ਜੇ(ctx->conn_closed) {
    failf(ਅੰਕੜੇ, "proxy closed connection");
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }

  ਕਰੋ {
    ਚੋਣ(ts->state) {
    ਹਾਲਤ H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] CONNECT start for %s", ts->authority);
      ਨਤੀਜਾ = submit_CONNECT(cf, ਅੰਕੜੇ, ts);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, ਅੰਕੜੇ,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    ਹਾਲਤ H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      ਨਤੀਜਾ = proxy_h2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = proxy_h2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ && ਨਤੀਜਾ != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, ਅੰਕੜੇ,
                           (bool)ctx->udp_tunnel);
        ਰੋਕੋ;
      }

      ਜੇ(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, ਅੰਕੜੇ,
                           (bool)ctx->udp_tunnel);
      }
      ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ = CURLE_OK;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      FALLTHROUGH();

    ਹਾਲਤ H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      ਨਤੀਜਾ = inspect_response(cf, ਅੰਕੜੇ, ts);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      ਰੋਕੋ;

    ਹਾਲਤ H2_TUNNEL_ESTABLISHED:
      ਵਾਪਸ CURLE_OK;

    ਹਾਲਤ H2_TUNNEL_FAILED:
      ਵਾਪਸ CURLE_RECV_ERROR;

    ਮੂਲ:
      ਰੋਕੋ;
    }

  } ਜਦੋਂ(ts->state == H2_TUNNEL_INIT);

ਬਾਹਰਲਾ:
  ਜੇ((ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, ਅੰਕੜੇ,
                       (bool)ctx->udp_tunnel);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Initialize the cfilter context
 */
ਸਥਿਰ CURLcode cf_h2_proxy_ctx_init(ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  nghttp2_session_callbacks *cbs = ਨਿਰਾ;
  ਪੂਰਨ rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, ਆਕਾਰ(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_ਮਾਪ, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_ਮਾਪ, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  ਜੇ(tunnel_ਧਾਰਾ_init(&ctx->tunnel, ctx->dest))
    ਜਾਓ ਬਾਹਰਲਾ;

  rc = nghttp2_session_callbacks_new(&cbs);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "Could not initialize nghttp2 callbacks");
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  nghttp2_session_callbacks_ਸੈੱਟ_send_callback(cbs, on_session_send);
  nghttp2_session_callbacks_ਸੈੱਟ_on_ਚੌਖਟਾ_recv_callback(
    cbs, proxy_h2_on_ਚੌਖਟਾ_recv);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  nghttp2_session_callbacks_ਸੈੱਟ_on_ਚੌਖਟਾ_send_callback(cbs,
                                                       proxy_h2_on_ਚੌਖਟਾ_send);
#ਜੇ_ਅੰਤ
  nghttp2_session_callbacks_ਸੈੱਟ_on_ਅੰਕੜੇ_chunk_recv_callback(
    cbs, tunnel_recv_callback);
  nghttp2_session_callbacks_ਸੈੱਟ_on_ਧਾਰਾ_close_callback(
    cbs, proxy_h2_on_ਧਾਰਾ_close);
  nghttp2_session_callbacks_ਸੈੱਟ_on_ਸਿਰਲੇਖ_callback(cbs, proxy_h2_on_ਸਿਰਲੇਖ);

  /* The nghttp2 session is not yet setup, do it */
  rc = proxy_h2_client_new(cf, cbs);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "Could not initialize nghttp2");
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  {
    nghttp2_settings_ਇੰਦਰਾਜ iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].ਮੁੱਲ = Curl_multi_max_concurrent_streams(ਅੰਕੜੇ->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_ਖਿੜਕੀ_ਮਾਪ;
    iv[1].ਮੁੱਲ = H2_TUNNEL_ਖਿੜਕੀ_ਮਾਪ;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ਧੱਕੋ;
    iv[2].ਮੁੱਲ = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    ਜੇ(rc) {
      failf(ਅੰਕੜੇ, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      ਨਤੀਜਾ = CURLE_HTTP2;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  rc = Curl_ngh2_session_ਸੈੱਟ_local_ਖਿੜਕੀ_ਮਾਪ(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_ਖਿੜਕੀ_ਮਾਪ);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ਨਤੀਜਾ = CURLE_HTTP2;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* all set, traffic will be send on connect */
  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  ਜੇ(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] init proxy ctx -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h2_proxy_connect(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  ਬਣਤਰ tunnel_ਧਾਰਾ *ts = &ctx->tunnel;

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  /* Connect the lower filters first */
  ਜੇ(!cf->ਅਗਲਾ->connected) {
    ਨਤੀਜਾ = Curl_conn_cf_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ || !*ਪੂਰਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  *ਪੂਰਾ = FALSE;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  ਜੇ(!ctx->ngh2.s) {
    ਨਤੀਜਾ = cf_h2_proxy_ctx_init(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  DEBUGASSERT(ts->authority);

  ਜੇ(Curl_timeleft_ms(ਅੰਕੜੇ) < 0) {
    failf(ਅੰਕੜੇ, "Proxy CONNECT aborted due to timeout");
    ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  ਨਤੀਜਾ = H2_CONNECT(cf, ਅੰਕੜੇ, ts);

ਬਾਹਰਲਾ:
  *ਪੂਰਾ = (ਨਤੀਜਾ == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  ਜੇ(*ਪੂਰਾ) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&ਅੰਕੜੇ->req, ਅੰਕੜੇ);
    Curl_client_reset(ਅੰਕੜੇ);
  }
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_h2_proxy_destroy(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = ਨਿਰਾ;
  }
}

ਸਥਿਰ CURLcode cf_h2_proxy_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ rv;

  ਜੇ(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);

  ਜੇ(!ctx->ਭੇਜਿਆ_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ਅਚਲ uint8_t *)"shutdown", ਆਕਾਰ("shutdown"));
    ਜੇ(rv) {
      failf(ਅੰਕੜੇ, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ctx->ਭੇਜਿਆ_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(Curl_ngh2_session_want_write(&ctx->ngh2))
    ਨਤੀਜਾ = proxy_h2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ);
  ਜੇ(!ਨਤੀਜਾ && Curl_ngh2_session_want_read(&ctx->ngh2))
    ਨਤੀਜਾ = proxy_h2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ);

  *ਪੂਰਾ = (ctx->conn_closed ||
           (!ਨਤੀਜਾ && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
ਬਾਹਰਲਾ:
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  cf->shutdown = (ਨਤੀਜਾ || *ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool cf_h2_proxy_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                                     ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  ਜੇ((ctx && !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.recvbuf)))
    ਵਾਪਸ TRUE;
  ਵਾਪਸ cf->ਅਗਲਾ ? cf->ਅਗਲਾ->cft->has_ਅੰਕੜੇ_pending(cf->ਅਗਲਾ, ਅੰਕੜੇ) : FALSE;
}

ਸਥਿਰ CURLcode cf_h2_proxy_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ);
  bool want_recv, want_send;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->outbufq) ||
                !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  ਨਹੀਂਤਾਂ
    Curl_pollset_check(ਅੰਕੜੇ, ps, sock, &want_recv, &want_send);

  ਜੇ(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
    bool c_exhaust, s_exhaust;

    CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
    c_exhaust = !Curl_ngh2_session_get_remote_ਖਿੜਕੀ_ਮਾਪ(&ctx->ngh2);
    s_exhaust = ctx->tunnel.ਧਾਰਾ_id >= 0 &&
                !Curl_ngh2_session_get_ਧਾਰਾ_remote_ਖਿੜਕੀ_ਮਾਪ(
                  &ctx->ngh2, ctx->tunnel.ਧਾਰਾ_id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->outbufq) ||
                !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.sendbuf);

    ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ(ਅੰਕੜੇ, ps, sock, want_recv, want_send);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (ਪੂਰਨ)ਨਤੀਜਾ);
    CF_ਅੰਕੜੇ_RESTORE(cf, save);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ctx->ਭੇਜਿਆ_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->outbufq) ||
                !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ(ਅੰਕੜੇ, ps, sock, want_recv, want_send);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (ਪੂਰਨ)ਨਤੀਜਾ);
    CF_ਅੰਕੜੇ_RESTORE(cf, save);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode h2_handle_tunnel_close(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       size_t *pnread)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  ਜੇ(ctx->tunnel.error) {
    failf(ਅੰਕੜੇ, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.ਧਾਰਾ_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    ਵਾਪਸ CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.ਧਾਰਾ_id);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode tunnel_recv(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅੱਖਰ *buf, size_t len, size_t *pnread)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_AGAIN;

  *pnread = 0;
  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.recvbuf))
    ਨਤੀਜਾ = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  ਨਹੀਂਤਾਂ {
    ਜੇ(ctx->tunnel.closed) {
      ਨਤੀਜਾ = h2_handle_tunnel_close(cf, ਅੰਕੜੇ, pnread);
    }
    ਨਹੀਂਤਾਂ ਜੇ(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.ਧਾਰਾ_id)) {
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = CURLE_AGAIN;
  }

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.ਧਾਰਾ_id, len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h2_proxy_recv(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅੱਖਰ *buf, size_t len,
                                 size_t *pnread)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  CURLcode ਨਤੀਜਾ;

  *pnread = 0;
  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);

  ਜੇ(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    ਨਤੀਜਾ = CURLE_RECV_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.recvbuf)) {
    ਨਤੀਜਾ = proxy_h2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = tunnel_recv(cf, ਅੰਕੜੇ, buf, len, pnread);

  ਜੇ(!ਨਤੀਜਾ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] increase window by %zu",
                ctx->tunnel.ਧਾਰਾ_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.ਧਾਰਾ_id, *pnread);
  }

  ਨਤੀਜਾ = Curl_1st_fatal(ਨਤੀਜਾ, proxy_h2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ));

ਬਾਹਰਲਾ:
  ਜੇ((!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.sendbuf)) &&
     (!ਨਤੀਜਾ || (ਨਤੀਜਾ == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, ਅੰਕੜੇ, &ctx->tunnel);
  }
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.ਧਾਰਾ_id, len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread);
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h2_proxy_send(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)eos;
  *pnwritten = 0;
  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);

  ਜੇ(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(ctx->tunnel.closed) {
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_send(), bufq_write %d, %zu", (ਪੂਰਨ)ਨਤੀਜਾ,
              *pnwritten);
  ਜੇ(ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_AGAIN))
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    ਜੇ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_ਅੰਕੜੇ(&ctx->ngh2, ctx->tunnel.ਧਾਰਾ_id))) {
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਨਤੀਜਾ = Curl_1st_fatal(ਨਤੀਜਾ, proxy_h2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ));
  ਨਤੀਜਾ = Curl_1st_fatal(ਨਤੀਜਾ, proxy_h2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ));

  ਜੇ(!ਨਤੀਜਾ && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    ਜੇ(ctx->tunnel.closed) {
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
    }
    ਨਹੀਂਤਾਂ {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] send: nothing to do in this session");
      ਨਤੀਜਾ = CURLE_HTTP2;
    }
  }

ਬਾਹਰਲਾ:
  ਜੇ((!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.sendbuf)) &&
     (!ਨਤੀਜਾ || (ਨਤੀਜਾ == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, ਅੰਕੜੇ, &ctx->tunnel);
  }
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.ਧਾਰਾ_id, len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten,
              Curl_ngh2_session_get_ਧਾਰਾ_remote_ਖਿੜਕੀ_ਮਾਪ(
                &ctx->ngh2, ctx->tunnel.ਧਾਰਾ_id),
              Curl_ngh2_session_get_remote_ਖਿੜਕੀ_ਮਾਪ(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h2_proxy_flush(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    ਜੇ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_ਅੰਕੜੇ(&ctx->ngh2, ctx->tunnel.ਧਾਰਾ_id))) {
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਨਤੀਜਾ = proxy_h2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ);

ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.ਧਾਰਾ_id, (ਪੂਰਨ)ਨਤੀਜਾ,
              Curl_ngh2_session_get_ਧਾਰਾ_remote_ਖਿੜਕੀ_ਮਾਪ(
                &ctx->ngh2, ctx->tunnel.ਧਾਰਾ_id),
              Curl_ngh2_session_get_remote_ਖਿੜਕੀ_ਮਾਪ(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool proxy_h2_connisalive(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 bool *ਨਿਵੇਸ਼_pending)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ਨਿਵੇਸ਼_pending = FALSE;
  ਜੇ(!cf->ਅਗਲਾ || !cf->ਅਗਲਾ->cft->is_alive(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਨਿਵੇਸ਼_pending))
    ਵਾਪਸ FALSE;

  ਜੇ(*ਨਿਵੇਸ਼_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode ਨਤੀਜਾ;
    size_t nread;

    *ਨਿਵੇਸ਼_pending = FALSE;
    ਨਤੀਜਾ = Curl_cf_recv_bufq(cf->ਅਗਲਾ, ਅੰਕੜੇ, &ctx->inbufq, 0, &nread);
    ਜੇ(!ਨਤੀਜਾ) {
      ਜੇ(proxy_h2_process_pending_ਨਿਵੇਸ਼(cf, ਅੰਕੜੇ))
        /* immediate error, considered dead */
        alive = FALSE;
      ਨਹੀਂਤਾਂ {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ਵਾਪਸ alive;
}

ਸਥਿਰ bool cf_h2_proxy_is_alive(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 bool *ਨਿਵੇਸ਼_pending)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;

  *ਨਿਵੇਸ਼_pending = FALSE;
  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, ਅੰਕੜੇ, ਨਿਵੇਸ਼_pending));
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *ਨਿਵੇਸ਼_pending);
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ alive;
}

ਸਥਿਰ CURLcode cf_h2_proxy_query(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਬਣਤਰ cf_h2_proxy_ctx *ctx = cf->ctx;

  ਚੋਣ(query) {
  ਹਾਲਤ CF_QUERY_HOST_ਪੋਰਟ:
    *pres1 = (ਪੂਰਨ)ctx->dest->ਪੋਰਟ;
    *((ਅਚਲ ਅੱਖਰ **)pres2) = ctx->dest->hostname;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CF_QUERY_NEED_FLUSH: {
    ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->outbufq) ||
       !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "needs flush");
      *pres1 = TRUE;
      ਵਾਪਸ CURLE_OK;
    }
    ਰੋਕੋ;
  }
  ਹਾਲਤ CF_QUERY_ALPN_NEGOTIATED: {
    ਅਚਲ ਅੱਖਰ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ਨਿਰਾ;
    ਵਾਪਸ CURLE_OK;
  }
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ਸਥਿਰ CURLcode cf_h2_proxy_cntrl(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;

  (ਖਾਲੀ)arg1;
  (ਖਾਲੀ)arg2;

  ਚੋਣ(event) {
  ਹਾਲਤ CF_CTRL_FLUSH:
    CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
    ਨਤੀਜਾ = cf_h2_proxy_flush(cf, ਅੰਕੜੇ);
    CF_ਅੰਕੜੇ_RESTORE(cf, save);
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਬਣਤਰ Curl_cftype Curl_cft_h2_proxy = {
  "H2-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  cf_h2_proxy_destroy,
  cf_h2_proxy_connect,
  cf_h2_proxy_shutdown,
  cf_h2_proxy_adjust_pollset,
  cf_h2_proxy_ਅੰਕੜੇ_pending,
  cf_h2_proxy_send,
  cf_h2_proxy_recv,
  cf_h2_proxy_cntrl,
  cf_h2_proxy_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h2_proxy_query,
};

CURLcode Curl_cf_h2_proxy_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ Curl_peer *peer,
                                       ਬਣਤਰ Curl_peer *dest,
                                       bool udp_tunnel)
{
  ਬਣਤਰ Curl_cfilter *cf_h2_proxy = ਨਿਰਾ;
  ਬਣਤਰ cf_h2_proxy_ctx *ctx;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx)
    ਜਾਓ ਬਾਹਰਲਾ;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  ਨਤੀਜਾ = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ctx = ਨਿਰਾ;
  Curl_conn_cf_ਪਾਓ_after(cf, cf_h2_proxy);

ਬਾਹਰਲਾ:
  cf_h2_proxy_ctx_free(ctx);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
