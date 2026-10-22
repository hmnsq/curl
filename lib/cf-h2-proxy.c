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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
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

#ವ್ಯಾಖ್ಯಾನಿಸು PROXY_H2_CHUNK_ಗಾತ್ರ  (16 * 1024)

#ವ್ಯಾಖ್ಯಾನಿಸು PROXY_HTTP2_HUGE_ಕಿಟಕಿ_ಗಾತ್ರ (100 * 1024 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು H2_TUNNEL_ಕಿಟಕಿ_ಗಾತ್ರ        (10 * 1024 * 1024)

#ವ್ಯಾಖ್ಯಾನಿಸು PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_ಕಿಟಕಿ_ಗಾತ್ರ / PROXY_H2_CHUNK_ಗಾತ್ರ)
#ವ್ಯಾಖ್ಯಾನಿಸು PROXY_H2_NW_SEND_CHUNKS   1

#ವ್ಯಾಖ್ಯಾನಿಸು H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_ಕಿಟಕಿ_ಗಾತ್ರ / PROXY_H2_CHUNK_ಗಾತ್ರ)
#ವ್ಯಾಖ್ಯಾನಿಸು H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_ಗಾತ್ರ)


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

ರಚನೆ tunnel_ಹರಿವು {
  ರಚನೆ http_resp *resp;
  ರಚನೆ bufq recvbuf;
  ರಚನೆ bufq sendbuf;
  ಅಕ್ಷರ *authority;
  int32_t ಹರಿವು_id;
  uint32_t error;
  h2_tunnel_state state;
  ಬಿಟ್(has_final_response);
  ಬಿಟ್(closed);
  ಬಿಟ್(reset);
};

ಸ್ಥಿರ CURLcode tunnel_ಹರಿವು_init(ರಚನೆ tunnel_ಹರಿವು *ts,
                                   ರಚನೆ Curl_peer *dest)
{
  ts->state = H2_TUNNEL_INIT;
  ts->ಹರಿವು_id = -1;
  Curl_bufq_init2(&ts->recvbuf, PROXY_H2_CHUNK_ಗಾತ್ರ, H2_TUNNEL_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_ಎಲ್ಲೆ);
  Curl_bufq_init(&ts->sendbuf, PROXY_H2_CHUNK_ಗಾತ್ರ, H2_TUNNEL_SEND_CHUNKS);

  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->ಪೋರ್ಟ್);
  ಆದರೆ(!ts->authority)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ tunnel_ಹರಿವು_reset(ರಚನೆ tunnel_ಹರಿವು *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = ಖಾಲಿ;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->ಹರಿವು_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

ಸ್ಥಿರ ಶೂನ್ಯ tunnel_ಹರಿವು_clear(ರಚನೆ tunnel_ಹರಿವು *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, ಗಾತ್ರ(*ts));
  ts->state = H2_TUNNEL_INIT;
}

ಸ್ಥಿರ ಶೂನ್ಯ h2_tunnel_go_state(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ tunnel_ಹರಿವು *ts,
                               h2_tunnel_state new_state,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               bool udp_tunnel)
{
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)udp_tunnel;

  ಆದರೆ(ts->state == new_state)
    ಹಿಂತಿರುಗಿ;
  /* leaving this one */
  ಆಯ್ಕೆ(ts->state) {
  ಸ್ಥಿತಿ H2_TUNNEL_CONNECT:
    ದತ್ತಾಂಶ->req.ignorebody = FALSE;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  /* entering this one */
  ಆಯ್ಕೆ(new_state) {
  ಸ್ಥಿತಿ H2_TUNNEL_INIT:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] new tunnel state 'init'", ts->ಹರಿವು_id);
    tunnel_ಹರಿವು_reset(ts);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H2_TUNNEL_CONNECT:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] new tunnel state 'connect'", ts->ಹರಿವು_id);
    ts->state = H2_TUNNEL_CONNECT;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] new tunnel state 'response'", ts->ಹರಿವು_id);
    ts->state = H2_TUNNEL_RESPONSE;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] new tunnel state 'established'",
                ts->ಹರಿವು_id);
    infof(ದತ್ತಾಂಶ, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    ದತ್ತಾಂಶ->state.authproxy.ಮುಗಿದ = TRUE;
    ದತ್ತಾಂಶ->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  ಸ್ಥಿತಿ H2_TUNNEL_FAILED:
    ಆದರೆ(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] new tunnel state 'failed'", ts->ಹರಿವು_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(ದತ್ತಾಂಶ->req.hd_proxy_auth);
    ನಿಲ್ಲಿಸು;
  }
}

ರಚನೆ cf_h2_proxy_ctx {
  ರಚನೆ curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ರಚನೆ cf_call_ದತ್ತಾಂಶ call_ದತ್ತಾಂಶ;

  ರಚನೆ bufq inbufq;  /* network receive buffer */
  ರಚನೆ bufq outbufq; /* network send buffer */

  ರಚನೆ Curl_peer *peer; /* proxy we talk to */
  ರಚನೆ Curl_peer *dest; /* where to tunnel to */
  ರಚನೆ tunnel_ಹರಿವು tunnel; /* our tunnel CONNECT stream */
  int32_t goaway_error;
  int32_t remote_max_sid;
  ಬಿಟ್(conn_closed);
  ಬಿಟ್(rcvd_goaway);
  ಬಿಟ್(ಕಳುಹಿಸಿದ_goaway);
  ಬಿಟ್(nw_ಹೊರಹೋಗು_blocked);
  ಬಿಟ್(udp_tunnel);
};

/* How to access `call_data` from a cf_h2 filter */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು CF_CTX_CALL_ದತ್ತಾಂಶ
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CTX_CALL_ದತ್ತಾಂಶ(cf) ((ರಚನೆ cf_h2_proxy_ctx *)(cf)->ctx)->call_ದತ್ತಾಂಶ

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_proxy_ctx_clear(ರಚನೆ cf_h2_proxy_ctx *ctx)
{
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save = ctx->call_ದತ್ತಾಂಶ;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_ಹರಿವು_clear(&ctx->tunnel);
  memset(ctx, 0, ಗಾತ್ರ(*ctx));
  ctx->call_ದತ್ತಾಂಶ = save;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_proxy_ctx_free(ರಚನೆ cf_h2_proxy_ctx *ctx)
{
  ಆದರೆ(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ drain_tunnel(ರಚನೆ Curl_cfilter *cf,
                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ tunnel_ಹರಿವು *tunnel)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  (ಶೂನ್ಯ)cf;
  ಆದರೆ(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ CURLcode proxy_h2_nw_ಹೊರಹೋಗು_writer(ಶೂನ್ಯ *writer_ctx,
                                       ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  ರಚನೆ Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  ಆದರೆ(cf) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
    CURLcode ಫಲಿತಾಂಶ;
    ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ proxy_h2_client_new(ರಚನೆ Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ಖಾಲಿ, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  ಪೂರ್ಣಾಂಕ rc = nghttp2_option_new(&o);
  ಆದರೆ(rc)
    ಹಿಂತಿರುಗಿ rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_ಗಣ_no_auto_ಕಿಟಕಿ_update(o, 1);
#ಆದರೆ NGHTTP2_ಆವೃತ್ತಿ_ಅಂಕೆ >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_ಗಣ_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#ಆದರೆ_ಅಂತ್ಯ
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ proxy_h2_should_close_session(ರಚನೆ cf_h2_proxy_ctx *ctx)
{
  ಹಿಂತಿರುಗಿ !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

ಸ್ಥಿರ CURLcode proxy_h2_nw_ಹೊರಹೋಗು_flush(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->outbufq))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಫಲಿತಾಂಶ = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_ಹೊರಹೋಗು_writer, cf,
                          &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_ಹೊರಹೋಗು_blocked = 1;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] nw send buffer flushed");
  ಹಿಂತಿರುಗಿ Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ಸ್ಥಿರ CURLcode proxy_h2_process_pending_ಒಳಹರಿವು(ರಚನೆ Curl_cfilter *cf,
                                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf;
  size_t blen, nread;
  ssize_t rv;

  ಆಗ(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ಸ್ಥಿರಾಂಕ uint8_t *)buf, blen);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    ಆದರೆ(!curlx_sztouz(rv, &nread)) {
      failf(ದತ್ತಾಂಶ,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((ಪೂರ್ಣಾಂಕ)rv));
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      ನಿಲ್ಲಿಸು;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->inbufq)) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] all data in connection buffer processed");
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode proxy_h2_ಪ್ರಗತಿ_ingress(ರಚನೆ Curl_cfilter *cf,
                                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->inbufq)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    ಫಲಿತಾಂಶ = proxy_h2_process_pending_ಒಳಹರಿವು(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  ಆಗ(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_ತುಂಬಿದ(&ctx->tunnel.recvbuf)) {

    ಫಲಿತಾಂಶ = Curl_cf_recv_bufq(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, nread);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN) {
        failf(ದತ್ತಾಂಶ, "Failed receiving HTTP2 proxy data");
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread == 0) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      ನಿಲ್ಲಿಸು;
    }

    ಫಲಿತಾಂಶ = proxy_h2_process_pending_ಒಳಹರಿವು(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode proxy_h2_ಪ್ರಗತಿ_egress(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಂಕ rv = 0;

  ctx->nw_ಹೊರಹೋಗು_blocked = 0;
  ಆಗ(!rv && !ctx->nw_ಹೊರಹೋಗು_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  ಆದರೆ(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  }
  ಹಿಂತಿರುಗಿ proxy_h2_nw_ಹೊರಹೋಗು_flush(cf, ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ ssize_t on_session_send(nghttp2_session *h2,
                               ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t blen, ಪೂರ್ಣಾಂಕ flags,
                               ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)h2;
  (ಶೂನ್ಯ)flags;
  DEBUGASSERT(ದತ್ತಾಂಶ);

  ಫಲಿತಾಂಶ = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_ಹೊರಹೋಗು_writer, cf, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      ctx->nw_ಹೊರಹೋಗು_blocked = 1;
      ಹಿಂತಿರುಗಿ NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(ದತ್ತಾಂಶ, "Failed sending HTTP2 data");
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ಆದರೆ(!nwritten)
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_WOULDBLOCK;

  ಹಿಂತಿರುಗಿ (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ proxy_h2_on_ಚೌಕಟ್ಟು_send(nghttp2_session *session,
                                  ಸ್ಥಿರಾಂಕ nghttp2_ಚೌಕಟ್ಟು *ಚೌಕಟ್ಟು,
                                  ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);

  (ಶೂನ್ಯ)session;
  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಆದರೆ(Curl_trc_cf_is_verbose(cf, ದತ್ತಾಂಶ)) {
    ಅಕ್ಷರ ಬಫರ್[256];
    ಪೂರ್ಣಾಂಕ len;
    len = Curl_nghttp2_fr_print(ಚೌಕಟ್ಟು, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್) - 1);
    ಬಫರ್[len] = 0;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] -> %s", ಚೌಕಟ್ಟು->hd.ಹರಿವು_id, ಬಫರ್);
  }
  ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ proxy_h2_on_ಚೌಕಟ್ಟು_recv(nghttp2_session *session,
                                  ಸ್ಥಿರಾಂಕ nghttp2_ಚೌಕಟ್ಟು *ಚೌಕಟ್ಟು,
                                  ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  int32_t ಹರಿವು_id = ಚೌಕಟ್ಟು->hd.ಹರಿವು_id;

  (ಶೂನ್ಯ)session;
  DEBUGASSERT(ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆದರೆ(Curl_trc_cf_is_verbose(cf, ದತ್ತಾಂಶ)) {
    ಅಕ್ಷರ ಬಫರ್[256];
    ಪೂರ್ಣಾಂಕ len;
    len = Curl_nghttp2_fr_print(ಚೌಕಟ್ಟು, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್) - 1);
    ಬಫರ್[len] = 0;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] <- %s", ಚೌಕಟ್ಟು->hd.ಹರಿವು_id, ಬಫರ್);
  }
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

  ಆದರೆ(!ಹರಿವು_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(ದತ್ತಾಂಶ);
    ಆಯ್ಕೆ(ಚೌಕಟ್ಟು->hd.type) {
    ಸ್ಥಿತಿ NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      ಆದರೆ(CURL_REQ_WANT_SEND(ದತ್ತಾಂಶ)) {
        drain_tunnel(cf, ದತ್ತಾಂಶ, &ctx->tunnel);
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = ಚೌಕಟ್ಟು->goaway.ಕೊನೆಯ_ಹರಿವು_id;
      ಆದರೆ(ದತ್ತಾಂಶ) {
        infof(ದತ್ತಾಂಶ, "received GOAWAY, error=%u, last_stream=%d",
              ಚೌಕಟ್ಟು->goaway.error_ಸಂಕೇತ, ctx->remote_max_sid);
      }
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(ಹರಿವು_id != ctx->tunnel.ಹರಿವು_id) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] rcvd FRAME not for tunnel", ಹರಿವು_id);
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ಆಯ್ಕೆ(ಚೌಕಟ್ಟು->hd.type) {
  ಸ್ಥಿತಿ NGHTTP2_ಶೀರ್ಷಿಕೆಗಳು:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    ಆದರೆ(!ctx->tunnel.resp)
      ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] got http status: %d",
                ಹರಿವು_id, ctx->tunnel.resp->ಸ್ಥಿತಿ_2);
    ಆದರೆ(!ctx->tunnel.has_final_response && ctx->tunnel.resp->ಸ್ಥಿತಿ_2 / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ NGHTTP2_ಕಿಟಕಿ_UPDATE:
    ಆದರೆ(CURL_REQ_WANT_SEND(ದತ್ತಾಂಶ)) {
      drain_tunnel(cf, ದತ್ತಾಂಶ, &ctx->tunnel);
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ NGHTTP2_RST_ಹರಿವು:
    ಆದರೆ(ಚೌಕಟ್ಟು->rst_ಹರಿವು.error_ಸಂಕೇತ)
      ctx->tunnel.reset = TRUE;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ proxy_h2_on_ಶೀರ್ಷಿಕೆ(nghttp2_session *session,
                              ಸ್ಥಿರಾಂಕ nghttp2_ಚೌಕಟ್ಟು *ಚೌಕಟ್ಟು,
                              ಸ್ಥಿರಾಂಕ uint8_t *name, size_t namelen,
                              ಸ್ಥಿರಾಂಕ uint8_t *ಮೌಲ್ಯ, size_t valuelen,
                              uint8_t flags,
                              ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  int32_t ಹರಿವು_id = ಚೌಕಟ್ಟು->hd.ಹರಿವು_id;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)flags;
  (ಶೂನ್ಯ)session;
  DEBUGASSERT(ಹರಿವು_id); /* should never be a zero stream ID here */
  ಆದರೆ(ಹರಿವು_id != ctx->tunnel.ಹರಿವು_id) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", ಹರಿವು_id,
                (ಪೂರ್ಣಾಂಕ)namelen, name, (ಪೂರ್ಣಾಂಕ)valuelen, ಮೌಲ್ಯ);
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ಆದರೆ(ಚೌಕಟ್ಟು->hd.type == NGHTTP2_ತಳ್ಳು_PROMISE)
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;

  ಆದರೆ(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(namelen == CURL_CSTRLEN(HTTP_PSEUDO_ಸ್ಥಿತಿ) &&
     !memcmp(HTTP_PSEUDO_ಸ್ಥಿತಿ, name, namelen)) {
    int32_t http_ಸ್ಥಿತಿ;
    ರಚನೆ http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    ಫಲಿತಾಂಶ = Curl_http_decode_ಸ್ಥಿತಿ(&http_ಸ್ಥಿತಿ,
                                    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೌಲ್ಯ, valuelen);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
    ಫಲಿತಾಂಶ = Curl_http_resp_make(&resp, http_ಸ್ಥಿತಿ, ಖಾಲಿ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
    ಆದರೆ(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] status: HTTP/2 %03d",
                ಹರಿವು_id, ctx->tunnel.resp->ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(!ctx->tunnel.resp)
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;

  ಫಲಿತಾಂಶ = Curl_dynhds_ಕೂಡಿಸು(&ctx->tunnel.resp->ಶೀರ್ಷಿಕೆಗಳು,
                           (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)name, namelen,
                           (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೌಲ್ಯ, valuelen);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] header: %.*s: %.*s",
              ಹರಿವು_id, (ಪೂರ್ಣಾಂಕ)namelen, name, (ಪೂರ್ಣಾಂಕ)valuelen, ಮೌಲ್ಯ);

  ಹಿಂತಿರುಗಿ 0; /* 0 is successful */
}

ಸ್ಥಿರ ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t ಹರಿವು_id,
                                    uint8_t *buf, size_t ಉದ್ದಳತೆ,
                                    uint32_t *ದತ್ತಾಂಶ_flags,
                                    nghttp2_ದತ್ತಾಂಶ_ಮೂಲ *ಮೂಲ,
                                    ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  ರಚನೆ tunnel_ಹರಿವು *ts;
  CURLcode ಫಲಿತಾಂಶ;
  size_t nread;

  (ಶೂನ್ಯ)session;
  (ಶೂನ್ಯ)ಮೂಲ;
  (ಶೂನ್ಯ)ctx;

  ಆದರೆ(!ಹರಿವು_id)
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_INVALID_ವಾದ;

  ts = Curl_ngh2_session_get_ಹರಿವು_user_ದತ್ತಾಂಶ(&ctx->ngh2, ಹರಿವು_id);
  ಆದರೆ(!ts)
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  ಫಲಿತಾಂಶ = Curl_bufq_read(&ts->sendbuf, buf, ಉದ್ದಳತೆ, &nread);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN)
      ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_DEFERRED;
  }
  ಆದರೆ(ts->closed && Curl_bufq_is_ಖಾಲಿಗಳು(&ts->sendbuf))
    *ದತ್ತಾಂಶ_flags = NGHTTP2_ದತ್ತಾಂಶ_FLAG_EOF;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] tunnel_send_callback -> %zu",
              ts->ಹರಿವು_id, nread);
  ಹಿಂತಿರುಗಿ (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t ಹರಿವು_id,
                                ಸ್ಥಿರಾಂಕ uint8_t *mem, size_t len, ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)flags;
  (ಶೂನ್ಯ)session;
  DEBUGASSERT(ಹರಿವು_id); /* should never be a zero stream ID here */

  ಆದರೆ(ಹರಿವು_id != ctx->tunnel.ಹರಿವು_id)
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;

  ಫಲಿತಾಂಶ = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN)
      ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    nwritten = 0;
#ಆದರೆ_ಅಂತ್ಯ
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ proxy_h2_on_ಹರಿವು_close(nghttp2_session *session,
                                    int32_t ಹರಿವು_id,
                                    uint32_t error_ಸಂಕೇತ, ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);

  (ಶೂನ್ಯ)session;

  ಆದರೆ(ಹರಿವು_id != ctx->tunnel.ಹರಿವು_id)
    ಹಿಂತಿರುಗಿ 0;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              ಹರಿವು_id, nghttp2_http2_strerror(error_ಸಂಕೇತ), error_ಸಂಕೇತ);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_ಸಂಕೇತ;
  ಆದರೆ(error_ಸಂಕೇತ)
    ctx->tunnel.reset = TRUE;

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  ರಚನೆ Curl_cfilter *cf,
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ರಚನೆ curl_ngh2 *ngh2,
  ರಚನೆ httpreq *req,
  ಸ್ಥಿರಾಂಕ nghttp2_ಆದ್ಯತೆ_spec *pri_spec,
  ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ,
  nghttp2_ದತ್ತಾಂಶ_ಮೂಲ_read_callback read_callback,
  ಶೂನ್ಯ *read_ctx)
{
  ರಚನೆ dynhds h2_ಶೀರ್ಷಿಕೆಗಳು;
  nghttp2_nv *nva = ಖಾಲಿ;
  int32_t ಹರಿವು_id = -1;
  size_t nheader;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)cf;
  Curl_dynhds_init(&h2_ಶೀರ್ಷಿಕೆಗಳು, MAX_HTTP_RESP_ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ, DYN_HTTP_REQUEST);
  ಫಲಿತಾಂಶ = Curl_http_req_ಗಮ್ಯ_h2(&h2_ಶೀರ್ಷಿಕೆಗಳು, req, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  nva = Curl_dynhds_ಗಮ್ಯ_nva(&h2_ಶೀರ್ಷಿಕೆಗಳು, &nheader);
  ಆದರೆ(!nva) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(read_callback) {
    nghttp2_ದತ್ತಾಂಶ_provider ದತ್ತಾಂಶ_prd;

    ದತ್ತಾಂಶ_prd.read_callback = read_callback;
    ದತ್ತಾಂಶ_prd.ಮೂಲ.ptr = read_ctx;
    ಹರಿವು_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &ದತ್ತಾಂಶ_prd, ಹರಿವು_user_ದತ್ತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಹರಿವು_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, ಖಾಲಿ, ಹರಿವು_user_ದತ್ತಾಂಶ);
  }

  ಆದರೆ(ಹರಿವು_id < 0) {
    failf(ದತ್ತಾಂಶ, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(ಹರಿವು_id), ಹರಿವು_id);
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  curlx_free(nva);
  Curl_dynhds_free(&h2_ಶೀರ್ಷಿಕೆಗಳು);
  *pstream_id = ಹರಿವು_id;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode submit_CONNECT(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ tunnel_ಹರಿವು *ts)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ httpreq *req = ಖಾಲಿ;

  ಫಲಿತಾಂಶ = Curl_http_proxy_create_tunnel_request(&req, cf, ದತ್ತಾಂಶ,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಫಲಿತಾಂಶ = Curl_creader_ಗಣ_null(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = proxy_h2_submit(&ts->ಹರಿವು_id, cf, ದತ್ತಾಂಶ, &ctx->ngh2, req,
                           ಖಾಲಿ, ts, tunnel_send_callback, cf);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->ಹರಿವು_id, nghttp2_strerror(ts->ಹರಿವು_id));
  }

ಹೊರಹೋಗು:
  ಆದರೆ(req)
    Curl_http_req_free(req);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    failf(ದತ್ತಾಂಶ, "Failed sending CONNECT to proxy");
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode inspect_response(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ tunnel_ಹರಿವು *ts)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_ಫಲಿತಾಂಶ res;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_http_proxy_inspect_tunnel_response(
      cf, ದತ್ತಾಂಶ, ts->resp, (bool)ctx->udp_tunnel, &res);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  ಆಯ್ಕೆ(res) {
  ಸ್ಥಿತಿ PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, ದತ್ತಾಂಶ,
                       (bool)ctx->udp_tunnel);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, ದತ್ತಾಂಶ,
                       (bool)ctx->udp_tunnel);
    ಫಲಿತಾಂಶ = CURLE_COULDNT_CONNECT;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, ದತ್ತಾಂಶ,
                       (bool)ctx->udp_tunnel);
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode H2_CONNECT(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ tunnel_ಹರಿವು *ts)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  ಆದರೆ(ctx->conn_closed) {
    failf(ದತ್ತಾಂಶ, "proxy closed connection");
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }

  ಮಾಡು {
    ಆಯ್ಕೆ(ts->state) {
    ಸ್ಥಿತಿ H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] CONNECT start for %s", ts->authority);
      ಫಲಿತಾಂಶ = submit_CONNECT(cf, ದತ್ತಾಂಶ, ts);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, ದತ್ತಾಂಶ,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    ಸ್ಥಿತಿ H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      ಫಲಿತಾಂಶ = proxy_h2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = proxy_h2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, ದತ್ತಾಂಶ,
                           (bool)ctx->udp_tunnel);
        ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, ದತ್ತಾಂಶ,
                           (bool)ctx->udp_tunnel);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ = CURLE_OK;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      FALLTHROUGH();

    ಸ್ಥಿತಿ H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      ಫಲಿತಾಂಶ = inspect_response(cf, ದತ್ತಾಂಶ, ts);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ H2_TUNNEL_ESTABLISHED:
      ಹಿಂತಿರುಗಿ CURLE_OK;

    ಸ್ಥಿತಿ H2_TUNNEL_FAILED:
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;

    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }

  } ಆಗ(ts->state == H2_TUNNEL_INIT);

ಹೊರಹೋಗು:
  ಆದರೆ((ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, ದತ್ತಾಂಶ,
                       (bool)ctx->udp_tunnel);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Initialize the cfilter context
 */
ಸ್ಥಿರ CURLcode cf_h2_proxy_ctx_init(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  nghttp2_session_callbacks *cbs = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, ಗಾತ್ರ(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_ಗಾತ್ರ, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_ಗಾತ್ರ, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  ಆದರೆ(tunnel_ಹರಿವು_init(&ctx->tunnel, ctx->dest))
    ಹೋಗು ಹೊರಹೋಗು;

  rc = nghttp2_session_callbacks_new(&cbs);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "Could not initialize nghttp2 callbacks");
    ಹೋಗು ಹೊರಹೋಗು;
  }

  nghttp2_session_callbacks_ಗಣ_send_callback(cbs, on_session_send);
  nghttp2_session_callbacks_ಗಣ_on_ಚೌಕಟ್ಟು_recv_callback(
    cbs, proxy_h2_on_ಚೌಕಟ್ಟು_recv);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  nghttp2_session_callbacks_ಗಣ_on_ಚೌಕಟ್ಟು_send_callback(cbs,
                                                       proxy_h2_on_ಚೌಕಟ್ಟು_send);
#ಆದರೆ_ಅಂತ್ಯ
  nghttp2_session_callbacks_ಗಣ_on_ದತ್ತಾಂಶ_chunk_recv_callback(
    cbs, tunnel_recv_callback);
  nghttp2_session_callbacks_ಗಣ_on_ಹರಿವು_close_callback(
    cbs, proxy_h2_on_ಹರಿವು_close);
  nghttp2_session_callbacks_ಗಣ_on_ಶೀರ್ಷಿಕೆ_callback(cbs, proxy_h2_on_ಶೀರ್ಷಿಕೆ);

  /* The nghttp2 session is not yet setup, do it */
  rc = proxy_h2_client_new(cf, cbs);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "Could not initialize nghttp2");
    ಹೋಗು ಹೊರಹೋಗು;
  }

  {
    nghttp2_settings_ನಮೂದು iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].ಮೌಲ್ಯ = Curl_multi_max_concurrent_streams(ದತ್ತಾಂಶ->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_ಕಿಟಕಿ_ಗಾತ್ರ;
    iv[1].ಮೌಲ್ಯ = H2_TUNNEL_ಕಿಟಕಿ_ಗಾತ್ರ;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ತಳ್ಳು;
    iv[2].ಮೌಲ್ಯ = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    ಆದರೆ(rc) {
      failf(ದತ್ತಾಂಶ, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      ಫಲಿತಾಂಶ = CURLE_HTTP2;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  rc = Curl_ngh2_session_ಗಣ_local_ಕಿಟಕಿ_ಗಾತ್ರ(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_ಕಿಟಕಿ_ಗಾತ್ರ);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ಫಲಿತಾಂಶ = CURLE_HTTP2;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* all set, traffic will be send on connect */
  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  ಆದರೆ(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] init proxy ctx -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_connect(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    bool *ಮುಗಿದ)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  ರಚನೆ tunnel_ಹರಿವು *ts = &ctx->tunnel;

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  /* Connect the lower filters first */
  ಆದರೆ(!cf->ಮುಂದಿನ->connected) {
    ಫಲಿತಾಂಶ = Curl_conn_cf_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ || !*ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  *ಮುಗಿದ = FALSE;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  ಆದರೆ(!ctx->ngh2.s) {
    ಫಲಿತಾಂಶ = cf_h2_proxy_ctx_init(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  DEBUGASSERT(ts->authority);

  ಆದರೆ(Curl_timeleft_ms(ದತ್ತಾಂಶ) < 0) {
    failf(ದತ್ತಾಂಶ, "Proxy CONNECT aborted due to timeout");
    ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  ಫಲಿತಾಂಶ = H2_CONNECT(cf, ದತ್ತಾಂಶ, ts);

ಹೊರಹೋಗು:
  *ಮುಗಿದ = (ಫಲಿತಾಂಶ == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  ಆದರೆ(*ಮುಗಿದ) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&ದತ್ತಾಂಶ->req, ದತ್ತಾಂಶ);
    Curl_client_reset(ದತ್ತಾಂಶ);
  }
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_proxy_destroy(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = ಖಾಲಿ;
  }
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_shutdown(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ rv;

  ಆದರೆ(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);

  ಆದರೆ(!ctx->ಕಳುಹಿಸಿದ_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ಸ್ಥಿರಾಂಕ uint8_t *)"shutdown", ಗಾತ್ರ("shutdown"));
    ಆದರೆ(rv) {
      failf(ದತ್ತಾಂಶ, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ctx->ಕಳುಹಿಸಿದ_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(Curl_ngh2_session_want_write(&ctx->ngh2))
    ಫಲಿತಾಂಶ = proxy_h2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(!ಫಲಿತಾಂಶ && Curl_ngh2_session_want_read(&ctx->ngh2))
    ಫಲಿತಾಂಶ = proxy_h2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ);

  *ಮುಗಿದ = (ctx->conn_closed ||
           (!ಫಲಿತಾಂಶ && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
ಹೊರಹೋಗು:
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  cf->shutdown = (ಫಲಿತಾಂಶ || *ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool cf_h2_proxy_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                                     ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  ಆದರೆ((ctx && !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.recvbuf)))
    ಹಿಂತಿರುಗಿ TRUE;
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ? cf->ಮುಂದಿನ->cft->has_ದತ್ತಾಂಶ_pending(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ) : FALSE;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ);
  bool want_recv, want_send;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->outbufq) ||
                !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    Curl_pollset_check(ದತ್ತಾಂಶ, ps, sock, &want_recv, &want_send);

  ಆದರೆ(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
    bool c_exhaust, s_exhaust;

    CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
    c_exhaust = !Curl_ngh2_session_get_remote_ಕಿಟಕಿ_ಗಾತ್ರ(&ctx->ngh2);
    s_exhaust = ctx->tunnel.ಹರಿವು_id >= 0 &&
                !Curl_ngh2_session_get_ಹರಿವು_remote_ಕಿಟಕಿ_ಗಾತ್ರ(
                  &ctx->ngh2, ctx->tunnel.ಹರಿವು_id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->outbufq) ||
                !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.sendbuf);

    ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ(ದತ್ತಾಂಶ, ps, sock, want_recv, want_send);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->ಕಳುಹಿಸಿದ_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->outbufq) ||
                !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ(ದತ್ತಾಂಶ, ps, sock, want_recv, want_send);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode h2_handle_tunnel_close(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       size_t *pnread)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  ಆದರೆ(ctx->tunnel.error) {
    failf(ದತ್ತಾಂಶ, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.ಹರಿವು_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.ಹರಿವು_id);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode tunnel_recv(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಅಕ್ಷರ *buf, size_t len, size_t *pnread)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_AGAIN;

  *pnread = 0;
  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.recvbuf))
    ಫಲಿತಾಂಶ = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(ctx->tunnel.closed) {
      ಫಲಿತಾಂಶ = h2_handle_tunnel_close(cf, ದತ್ತಾಂಶ, pnread);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.ಹರಿವು_id)) {
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
  }

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.ಹರಿವು_id, len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_recv(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಅಕ್ಷರ *buf, size_t len,
                                 size_t *pnread)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  CURLcode ಫಲಿತಾಂಶ;

  *pnread = 0;
  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);

  ಆದರೆ(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.recvbuf)) {
    ಫಲಿತಾಂಶ = proxy_h2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = tunnel_recv(cf, ದತ್ತಾಂಶ, buf, len, pnread);

  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] increase window by %zu",
                ctx->tunnel.ಹರಿವು_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.ಹರಿವು_id, *pnread);
  }

  ಫಲಿತಾಂಶ = Curl_1st_fatal(ಫಲಿತಾಂಶ, proxy_h2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ));

ಹೊರಹೋಗು:
  ಆದರೆ((!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.sendbuf)) &&
     (!ಫಲಿತಾಂಶ || (ಫಲಿತಾಂಶ == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, ದತ್ತಾಂಶ, &ctx->tunnel);
  }
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.ಹರಿವು_id, len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread);
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_send(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;
  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);

  ಆದರೆ(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(ctx->tunnel.closed) {
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_send(), bufq_write %d, %zu", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ,
              *pnwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_AGAIN))
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    ಆದರೆ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_ದತ್ತಾಂಶ(&ctx->ngh2, ctx->tunnel.ಹರಿವು_id))) {
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಫಲಿತಾಂಶ = Curl_1st_fatal(ಫಲಿತಾಂಶ, proxy_h2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ));
  ಫಲಿತಾಂಶ = Curl_1st_fatal(ಫಲಿತಾಂಶ, proxy_h2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ));

  ಆದರೆ(!ಫಲಿತಾಂಶ && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    ಆದರೆ(ctx->tunnel.closed) {
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] send: nothing to do in this session");
      ಫಲಿತಾಂಶ = CURLE_HTTP2;
    }
  }

ಹೊರಹೋಗು:
  ಆದರೆ((!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.sendbuf)) &&
     (!ಫಲಿತಾಂಶ || (ಫಲಿತಾಂಶ == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, ದತ್ತಾಂಶ, &ctx->tunnel);
  }
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.ಹರಿವು_id, len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten,
              Curl_ngh2_session_get_ಹರಿವು_remote_ಕಿಟಕಿ_ಗಾತ್ರ(
                &ctx->ngh2, ctx->tunnel.ಹರಿವು_id),
              Curl_ngh2_session_get_remote_ಕಿಟಕಿ_ಗಾತ್ರ(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_flush(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    ಆದರೆ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_ದತ್ತಾಂಶ(&ctx->ngh2, ctx->tunnel.ಹರಿವು_id))) {
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಫಲಿತಾಂಶ = proxy_h2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ);

ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.ಹರಿವು_id, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ,
              Curl_ngh2_session_get_ಹರಿವು_remote_ಕಿಟಕಿ_ಗಾತ್ರ(
                &ctx->ngh2, ctx->tunnel.ಹರಿವು_id),
              Curl_ngh2_session_get_remote_ಕಿಟಕಿ_ಗಾತ್ರ(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool proxy_h2_connisalive(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 bool *ಒಳಹರಿವು_pending)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ಒಳಹರಿವು_pending = FALSE;
  ಆದರೆ(!cf->ಮುಂದಿನ || !cf->ಮುಂದಿನ->cft->is_alive(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಒಳಹರಿವು_pending))
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ(*ಒಳಹರಿವು_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode ಫಲಿತಾಂಶ;
    size_t nread;

    *ಒಳಹರಿವು_pending = FALSE;
    ಫಲಿತಾಂಶ = Curl_cf_recv_bufq(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &ctx->inbufq, 0, &nread);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಆದರೆ(proxy_h2_process_pending_ಒಳಹರಿವು(cf, ದತ್ತಾಂಶ))
        /* immediate error, considered dead */
        alive = FALSE;
      ಇಲ್ಲದಿದ್ದರೆ {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ಹಿಂತಿರುಗಿ alive;
}

ಸ್ಥಿರ bool cf_h2_proxy_is_alive(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 bool *ಒಳಹರಿವು_pending)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;

  *ಒಳಹರಿವು_pending = FALSE;
  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, ದತ್ತಾಂಶ, ಒಳಹರಿವು_pending));
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *ಒಳಹರಿವು_pending);
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ alive;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_query(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ರಚನೆ cf_h2_proxy_ctx *ctx = cf->ctx;

  ಆಯ್ಕೆ(query) {
  ಸ್ಥಿತಿ CF_QUERY_HOST_ಪೋರ್ಟ್:
    *pres1 = (ಪೂರ್ಣಾಂಕ)ctx->dest->ಪೋರ್ಟ್;
    *((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **)pres2) = ctx->dest->hostname;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CF_QUERY_NEED_FLUSH: {
    ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->outbufq) ||
       !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "needs flush");
      *pres1 = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ನಿಲ್ಲಿಸು;
  }
  ಸ್ಥಿತಿ CF_QUERY_ALPN_NEGOTIATED: {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ CURLcode cf_h2_proxy_cntrl(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;

  ಆಯ್ಕೆ(event) {
  ಸ್ಥಿತಿ CF_CTRL_FLUSH:
    CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
    ಫಲಿತಾಂಶ = cf_h2_proxy_flush(cf, ದತ್ತಾಂಶ);
    CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ರಚನೆ Curl_cftype Curl_cft_h2_proxy = {
  "H2-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  cf_h2_proxy_destroy,
  cf_h2_proxy_connect,
  cf_h2_proxy_shutdown,
  cf_h2_proxy_adjust_pollset,
  cf_h2_proxy_ದತ್ತಾಂಶ_pending,
  cf_h2_proxy_send,
  cf_h2_proxy_recv,
  cf_h2_proxy_cntrl,
  cf_h2_proxy_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h2_proxy_query,
};

CURLcode Curl_cf_h2_proxy_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ Curl_peer *peer,
                                       ರಚನೆ Curl_peer *dest,
                                       bool udp_tunnel)
{
  ರಚನೆ Curl_cfilter *cf_h2_proxy = ಖಾಲಿ;
  ರಚನೆ cf_h2_proxy_ctx *ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx)
    ಹೋಗು ಹೊರಹೋಗು;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ctx = ಖಾಲಿ;
  Curl_conn_cf_ಸೇರಿಸು_after(cf, cf_h2_proxy);

ಹೊರಹೋಗು:
  cf_h2_proxy_ctx_free(ctx);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
