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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_NGHTTP2)

#شامل <nghttp2/nghttp2.h>

#شامل "urldata.h"
#شامل "url.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "curl_trc.h"
#شامل "bufq.h"
#شامل "curlx/dynbuf.h"
#شامل "dynhds.h"
#شامل "http2.h"
#شامل "http_proxy.h"
#شامل "multiif.h"
#شامل "ngh2_shim.h"
#شامل "sendf.h"
#شامل "select.h"
#شامل "cf-h2-proxy.h"

#تعریف_کرو PROXY_H2_CHUNK_سائز  (16 * 1024)

#تعریف_کرو PROXY_HTTP2_HUGE_کھڑکی_سائز (100 * 1024 * 1024)
#تعریف_کرو H2_TUNNEL_کھڑکی_سائز        (10 * 1024 * 1024)

#تعریف_کرو PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_کھڑکی_سائز / PROXY_H2_CHUNK_سائز)
#تعریف_کرو PROXY_H2_NW_SEND_CHUNKS   1

#تعریف_کرو H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_کھڑکی_سائز / PROXY_H2_CHUNK_سائز)
#تعریف_کرو H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_سائز)


قسمتعریف شمار {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

ساخت tunnel_دھارا {
  ساخت http_resp *resp;
  ساخت bufq recvbuf;
  ساخت bufq sendbuf;
  حرف *authority;
  int32_t دھارا_id;
  uint32_t error;
  h2_tunnel_state state;
  ثنائی_ہندسہ(has_final_response);
  ثنائی_ہندسہ(closed);
  ثنائی_ہندسہ(reset);
};

ساکن CURLcode tunnel_دھارا_init(ساخت tunnel_دھارا *ts,
                                   ساخت Curl_peer *dest)
{
  ts->state = H2_TUNNEL_INIT;
  ts->دھارا_id = -1;
  Curl_bufq_init2(&ts->recvbuf, PROXY_H2_CHUNK_سائز, H2_TUNNEL_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندی);
  Curl_bufq_init(&ts->sendbuf, PROXY_H2_CHUNK_سائز, H2_TUNNEL_SEND_CHUNKS);

  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->پورٹ);
  اگر(!ts->authority)
    واپس CURLE_برآمد_کا_حافظہ;

  واپس CURLE_OK;
}

ساکن خالی tunnel_دھارا_reset(ساخت tunnel_دھارا *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = تہی;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->دھارا_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

ساکن خالی tunnel_دھارا_clear(ساخت tunnel_دھارا *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, حجم(*ts));
  ts->state = H2_TUNNEL_INIT;
}

ساکن خالی h2_tunnel_go_state(ساخت Curl_cfilter *cf,
                               ساخت tunnel_دھارا *ts,
                               h2_tunnel_state new_state,
                               ساخت Curl_easy *ڈیٹا,
                               bool udp_tunnel)
{
  (خالی)cf;
  (خالی)udp_tunnel;

  اگر(ts->state == new_state)
    واپس;
  /* leaving this one */
  انتخاب(ts->state) {
  حالت H2_TUNNEL_CONNECT:
    ڈیٹا->req.ignorebody = FALSE;
    توڑو;
  طےشدہ:
    توڑو;
  }
  /* entering this one */
  انتخاب(new_state) {
  حالت H2_TUNNEL_INIT:
    CURL_TRC_CF(ڈیٹا, cf, "[%d] new tunnel state 'init'", ts->دھارا_id);
    tunnel_دھارا_reset(ts);
    توڑو;

  حالت H2_TUNNEL_CONNECT:
    CURL_TRC_CF(ڈیٹا, cf, "[%d] new tunnel state 'connect'", ts->دھارا_id);
    ts->state = H2_TUNNEL_CONNECT;
    توڑو;

  حالت H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(ڈیٹا, cf, "[%d] new tunnel state 'response'", ts->دھارا_id);
    ts->state = H2_TUNNEL_RESPONSE;
    توڑو;

  حالت H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(ڈیٹا, cf, "[%d] new tunnel state 'established'",
                ts->دھارا_id);
    infof(ڈیٹا, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    ڈیٹا->state.authproxy.مکمل = TRUE;
    ڈیٹا->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  حالت H2_TUNNEL_FAILED:
    اگر(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(ڈیٹا, cf, "[%d] new tunnel state 'failed'", ts->دھارا_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(ڈیٹا->req.hd_proxy_auth);
    توڑو;
  }
}

ساخت cf_h2_proxy_ctx {
  ساخت curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ساخت cf_call_ڈیٹا call_ڈیٹا;

  ساخت bufq inbufq;  /* network receive buffer */
  ساخت bufq outbufq; /* network send buffer */

  ساخت Curl_peer *peer; /* proxy we talk to */
  ساخت Curl_peer *dest; /* where to tunnel to */
  ساخت tunnel_دھارا tunnel; /* our tunnel CONNECT stream */
  int32_t goaway_error;
  int32_t remote_max_sid;
  ثنائی_ہندسہ(conn_closed);
  ثنائی_ہندسہ(rcvd_goaway);
  ثنائی_ہندسہ(بھیجا_goaway);
  ثنائی_ہندسہ(nw_برآمد_blocked);
  ثنائی_ہندسہ(udp_tunnel);
};

/* How to access `call_data` from a cf_h2 filter */
#تعریف_منسوخ CF_CTX_CALL_ڈیٹا
#تعریف_کرو CF_CTX_CALL_ڈیٹا(cf) ((ساخت cf_h2_proxy_ctx *)(cf)->ctx)->call_ڈیٹا

ساکن خالی cf_h2_proxy_ctx_clear(ساخت cf_h2_proxy_ctx *ctx)
{
  ساخت cf_call_ڈیٹا save = ctx->call_ڈیٹا;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_دھارا_clear(&ctx->tunnel);
  memset(ctx, 0, حجم(*ctx));
  ctx->call_ڈیٹا = save;
}

ساکن خالی cf_h2_proxy_ctx_free(ساخت cf_h2_proxy_ctx *ctx)
{
  اگر(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

ساکن خالی drain_tunnel(ساخت Curl_cfilter *cf,
                         ساخت Curl_easy *ڈیٹا,
                         ساخت tunnel_دھارا *tunnel)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  (خالی)cf;
  اگر(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_خالی_خانے(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_خالی_خانے(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(ڈیٹا);
}

ساکن CURLcode proxy_h2_nw_برآمد_writer(خالی *writer_ctx,
                                       ثابت uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  ساخت Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  اگر(cf) {
    ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
    CURLcode نتیجہ;
    نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(ڈیٹا, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (صحیح)نتیجہ, *pnwritten);
    واپس نتیجہ;
  }
  واپس CURLE_FAILED_INIT;
}

ساکن صحیح proxy_h2_client_new(ساخت Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { تہی, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  صحیح rc = nghttp2_option_new(&o);
  اگر(rc)
    واپس rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_سیٹ_no_auto_کھڑکی_update(o, 1);
#اگر NGHTTP2_نسخہ_عدد >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_سیٹ_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#اگر_ختم
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  واپس rc;
}

ساکن صحیح proxy_h2_should_close_session(ساخت cf_h2_proxy_ctx *ctx)
{
  واپس !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

ساکن CURLcode proxy_h2_nw_برآمد_flush(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode نتیجہ;

  اگر(Curl_bufq_is_خالی_خانے(&ctx->outbufq))
    واپس CURLE_OK;

  نتیجہ = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_برآمد_writer, cf,
                          &nwritten);
  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_AGAIN) {
      CURL_TRC_CF(ڈیٹا, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_برآمد_blocked = 1;
    }
    واپس نتیجہ;
  }
  CURL_TRC_CF(ڈیٹا, cf, "[0] nw send buffer flushed");
  واپس Curl_bufq_is_خالی_خانے(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ساکن CURLcode proxy_h2_process_pending_داخلہ(ساخت Curl_cfilter *cf,
                                               ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  ثابت غیرعلامتی حرف *buf;
  size_t blen, nread;
  ssize_t rv;

  جب(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ثابت uint8_t *)buf, blen);
    CURL_TRC_CF(ڈیٹا, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    اگر(!curlx_sztouz(rv, &nread)) {
      failf(ڈیٹا,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((صحیح)rv));
      واپس CURLE_RECV_ERROR;
    }
    ورنہ اگر(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      توڑو;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    اگر(Curl_bufq_is_خالی_خانے(&ctx->inbufq)) {
      CURL_TRC_CF(ڈیٹا, cf, "[0] all data in connection buffer processed");
      توڑو;
    }
    ورنہ {
      CURL_TRC_CF(ڈیٹا, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  واپس CURLE_OK;
}

ساکن CURLcode proxy_h2_پیش_رفت_ingress(ساخت Curl_cfilter *cf,
                                          ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  اگر(!Curl_bufq_is_خالی_خانے(&ctx->inbufq)) {
    CURL_TRC_CF(ڈیٹا, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    نتیجہ = proxy_h2_process_pending_داخلہ(cf, ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  جب(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_خالی_خانے(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_بھرا(&ctx->tunnel.recvbuf)) {

    نتیجہ = Curl_cf_recv_bufq(cf->اگلا, ڈیٹا, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(ڈیٹا, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (صحیح)نتیجہ, nread);
    اگر(نتیجہ) {
      اگر(نتیجہ != CURLE_AGAIN) {
        failf(ڈیٹا, "Failed receiving HTTP2 proxy data");
        واپس نتیجہ;
      }
      توڑو;
    }
    ورنہ اگر(nread == 0) {
      CURL_TRC_CF(ڈیٹا, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      توڑو;
    }

    نتیجہ = proxy_h2_process_pending_داخلہ(cf, ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  واپس CURLE_OK;
}

ساکن CURLcode proxy_h2_پیش_رفت_egress(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  صحیح rv = 0;

  ctx->nw_برآمد_blocked = 0;
  جب(!rv && !ctx->nw_برآمد_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  اگر(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(ڈیٹا, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    واپس CURLE_SEND_ERROR;
  }
  واپس proxy_h2_nw_برآمد_flush(cf, ڈیٹا);
}

ساکن ssize_t on_session_send(nghttp2_session *h2,
                               ثابت uint8_t *buf, size_t blen, صحیح flags,
                               خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  size_t nwritten;
  CURLcode نتیجہ = CURLE_OK;

  (خالی)h2;
  (خالی)flags;
  DEBUGASSERT(ڈیٹا);

  نتیجہ = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_برآمد_writer, cf, &nwritten);
  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_AGAIN) {
      ctx->nw_برآمد_blocked = 1;
      واپس NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(ڈیٹا, "Failed sending HTTP2 data");
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  اگر(!nwritten)
    واپس NGHTTP2_ERR_WOULDBLOCK;

  واپس (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#اگر_تعریف_شدہ CURLVERBOSE
ساکن صحیح proxy_h2_on_چوکھٹا_send(nghttp2_session *session,
                                  ثابت nghttp2_چوکھٹا *چوکھٹا,
                                  خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);

  (خالی)session;
  DEBUGASSERT(ڈیٹا);
  اگر(Curl_trc_cf_is_verbose(cf, ڈیٹا)) {
    حرف بفر[256];
    صحیح len;
    len = Curl_nghttp2_fr_print(چوکھٹا, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(ڈیٹا, cf, "[%d] -> %s", چوکھٹا->hd.دھارا_id, بفر);
  }
  واپس 0;
}
#اگر_ختم /* CURLVERBOSE */

ساکن صحیح proxy_h2_on_چوکھٹا_recv(nghttp2_session *session,
                                  ثابت nghttp2_چوکھٹا *چوکھٹا,
                                  خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  int32_t دھارا_id = چوکھٹا->hd.دھارا_id;

  (خالی)session;
  DEBUGASSERT(ڈیٹا);
#اگر_تعریف_شدہ CURLVERBOSE
  اگر(Curl_trc_cf_is_verbose(cf, ڈیٹا)) {
    حرف بفر[256];
    صحیح len;
    len = Curl_nghttp2_fr_print(چوکھٹا, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(ڈیٹا, cf, "[%d] <- %s", چوکھٹا->hd.دھارا_id, بفر);
  }
#اگر_ختم /* CURLVERBOSE */

  اگر(!دھارا_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(ڈیٹا);
    انتخاب(چوکھٹا->hd.type) {
    حالت NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      اگر(CURL_REQ_WANT_SEND(ڈیٹا)) {
        drain_tunnel(cf, ڈیٹا, &ctx->tunnel);
      }
      توڑو;
    حالت NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = چوکھٹا->goaway.آخری_دھارا_id;
      اگر(ڈیٹا) {
        infof(ڈیٹا, "received GOAWAY, error=%u, last_stream=%d",
              چوکھٹا->goaway.error_رمز, ctx->remote_max_sid);
      }
      توڑو;
    طےشدہ:
      توڑو;
    }
    واپس 0;
  }

  اگر(دھارا_id != ctx->tunnel.دھارا_id) {
    CURL_TRC_CF(ڈیٹا, cf, "[%d] rcvd FRAME not for tunnel", دھارا_id);
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  انتخاب(چوکھٹا->hd.type) {
  حالت NGHTTP2_سرخیاں:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    اگر(!ctx->tunnel.resp)
      واپس NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(ڈیٹا, cf, "[%d] got http status: %d",
                دھارا_id, ctx->tunnel.resp->حالت_2);
    اگر(!ctx->tunnel.has_final_response && ctx->tunnel.resp->حالت_2 / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    توڑو;
  حالت NGHTTP2_کھڑکی_UPDATE:
    اگر(CURL_REQ_WANT_SEND(ڈیٹا)) {
      drain_tunnel(cf, ڈیٹا, &ctx->tunnel);
    }
    توڑو;
  حالت NGHTTP2_RST_دھارا:
    اگر(چوکھٹا->rst_دھارا.error_رمز)
      ctx->tunnel.reset = TRUE;
    توڑو;
  طےشدہ:
    توڑو;
  }
  واپس 0;
}

ساکن صحیح proxy_h2_on_سرخی(nghttp2_session *session,
                              ثابت nghttp2_چوکھٹا *چوکھٹا,
                              ثابت uint8_t *name, size_t namelen,
                              ثابت uint8_t *قدر, size_t valuelen,
                              uint8_t flags,
                              خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  int32_t دھارا_id = چوکھٹا->hd.دھارا_id;
  CURLcode نتیجہ;

  (خالی)flags;
  (خالی)session;
  DEBUGASSERT(دھارا_id); /* should never be a zero stream ID here */
  اگر(دھارا_id != ctx->tunnel.دھارا_id) {
    CURL_TRC_CF(ڈیٹا, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", دھارا_id,
                (صحیح)namelen, name, (صحیح)valuelen, قدر);
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  اگر(چوکھٹا->hd.type == NGHTTP2_ڈالو_PROMISE)
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;

  اگر(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    واپس 0;
  }

  اگر(namelen == CURL_CSTRLEN(HTTP_PSEUDO_حالت) &&
     !memcmp(HTTP_PSEUDO_حالت, name, namelen)) {
    int32_t http_حالت;
    ساخت http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    نتیجہ = Curl_http_decode_حالت(&http_حالت,
                                    (ثابت حرف *)قدر, valuelen);
    اگر(نتیجہ)
      واپس NGHTTP2_ERR_CALLBACK_FAILURE;
    نتیجہ = Curl_http_resp_make(&resp, http_حالت, تہی);
    اگر(نتیجہ)
      واپس NGHTTP2_ERR_CALLBACK_FAILURE;
    اگر(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(ڈیٹا, cf, "[%d] status: HTTP/2 %03d",
                دھارا_id, ctx->tunnel.resp->حالت_2);
    واپس 0;
  }

  اگر(!ctx->tunnel.resp)
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;

  نتیجہ = Curl_dynhds_جوڑو(&ctx->tunnel.resp->سرخیاں,
                           (ثابت حرف *)name, namelen,
                           (ثابت حرف *)قدر, valuelen);
  اگر(نتیجہ)
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(ڈیٹا, cf, "[%d] header: %.*s: %.*s",
              دھارا_id, (صحیح)namelen, name, (صحیح)valuelen, قدر);

  واپس 0; /* 0 is successful */
}

ساکن ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t دھارا_id,
                                    uint8_t *buf, size_t لمبائی,
                                    uint32_t *ڈیٹا_flags,
                                    nghttp2_ڈیٹا_ماخذ *ماخذ,
                                    خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  ساخت tunnel_دھارا *ts;
  CURLcode نتیجہ;
  size_t nread;

  (خالی)session;
  (خالی)ماخذ;
  (خالی)ctx;

  اگر(!دھارا_id)
    واپس NGHTTP2_ERR_INVALID_دلیل;

  ts = Curl_ngh2_session_get_دھارا_user_ڈیٹا(&ctx->ngh2, دھارا_id);
  اگر(!ts)
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  نتیجہ = Curl_bufq_read(&ts->sendbuf, buf, لمبائی, &nread);
  اگر(نتیجہ) {
    اگر(نتیجہ != CURLE_AGAIN)
      واپس NGHTTP2_ERR_CALLBACK_FAILURE;
    واپس NGHTTP2_ERR_DEFERRED;
  }
  اگر(ts->closed && Curl_bufq_is_خالی_خانے(&ts->sendbuf))
    *ڈیٹا_flags = NGHTTP2_ڈیٹا_FLAG_EOF;

  CURL_TRC_CF(ڈیٹا, cf, "[%d] tunnel_send_callback -> %zu",
              ts->دھارا_id, nread);
  واپس (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

ساکن صحیح tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t دھارا_id,
                                ثابت uint8_t *mem, size_t len, خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode نتیجہ;

  (خالی)flags;
  (خالی)session;
  DEBUGASSERT(دھارا_id); /* should never be a zero stream ID here */

  اگر(دھارا_id != ctx->tunnel.دھارا_id)
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;

  نتیجہ = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  اگر(نتیجہ) {
    اگر(نتیجہ != CURLE_AGAIN)
      واپس NGHTTP2_ERR_CALLBACK_FAILURE;
#اگر_تعریف_شدہ DEBUGBUILD
    nwritten = 0;
#اگر_ختم
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  واپس 0;
}

ساکن صحیح proxy_h2_on_دھارا_close(nghttp2_session *session,
                                    int32_t دھارا_id,
                                    uint32_t error_رمز, خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);

  (خالی)session;

  اگر(دھارا_id != ctx->tunnel.دھارا_id)
    واپس 0;

  CURL_TRC_CF(ڈیٹا, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              دھارا_id, nghttp2_http2_strerror(error_رمز), error_رمز);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_رمز;
  اگر(error_رمز)
    ctx->tunnel.reset = TRUE;

  واپس 0;
}

ساکن CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  ساخت Curl_cfilter *cf,
  ساخت Curl_easy *ڈیٹا,
  ساخت curl_ngh2 *ngh2,
  ساخت httpreq *req,
  ثابت nghttp2_ترجیح_spec *pri_spec,
  خالی *دھارا_user_ڈیٹا,
  nghttp2_ڈیٹا_ماخذ_read_callback read_callback,
  خالی *read_ctx)
{
  ساخت dynhds h2_سرخیاں;
  nghttp2_nv *nva = تہی;
  int32_t دھارا_id = -1;
  size_t nheader;
  CURLcode نتیجہ;

  (خالی)cf;
  Curl_dynhds_init(&h2_سرخیاں, MAX_HTTP_RESP_سرخی_گنتی, DYN_HTTP_REQUEST);
  نتیجہ = Curl_http_req_مقصد_h2(&h2_سرخیاں, req, ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

  nva = Curl_dynhds_مقصد_nva(&h2_سرخیاں, &nheader);
  اگر(!nva) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  اگر(read_callback) {
    nghttp2_ڈیٹا_provider ڈیٹا_prd;

    ڈیٹا_prd.read_callback = read_callback;
    ڈیٹا_prd.ماخذ.ptr = read_ctx;
    دھارا_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &ڈیٹا_prd, دھارا_user_ڈیٹا);
  }
  ورنہ {
    دھارا_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, تہی, دھارا_user_ڈیٹا);
  }

  اگر(دھارا_id < 0) {
    failf(ڈیٹا, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(دھارا_id), دھارا_id);
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ برآمد;
  }
  نتیجہ = CURLE_OK;

برآمد:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرخیاں);
  *pstream_id = دھارا_id;
  واپس نتیجہ;
}

ساکن CURLcode submit_CONNECT(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت tunnel_دھارا *ts)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;
  ساخت httpreq *req = تہی;

  نتیجہ = Curl_http_proxy_create_tunnel_request(&req, cf, ڈیٹا,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  اگر(نتیجہ)
    جاؤ برآمد;
  نتیجہ = Curl_creader_سیٹ_null(ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = proxy_h2_submit(&ts->دھارا_id, cf, ڈیٹا, &ctx->ngh2, req,
                           تہی, ts, tunnel_send_callback, cf);
  اگر(نتیجہ) {
    CURL_TRC_CF(ڈیٹا, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->دھارا_id, nghttp2_strerror(ts->دھارا_id));
  }

برآمد:
  اگر(req)
    Curl_http_req_free(req);
  اگر(نتیجہ)
    failf(ڈیٹا, "Failed sending CONNECT to proxy");
  واپس نتیجہ;
}

ساکن CURLcode inspect_response(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت tunnel_دھارا *ts)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_نتیجہ res;
  CURLcode نتیجہ;

  نتیجہ = Curl_http_proxy_inspect_tunnel_response(
      cf, ڈیٹا, ts->resp, (bool)ctx->udp_tunnel, &res);
  اگر(نتیجہ)
    واپس نتیجہ;
  انتخاب(res) {
  حالت PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, ڈیٹا,
                       (bool)ctx->udp_tunnel);
    توڑو;
  حالت PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, ڈیٹا,
                       (bool)ctx->udp_tunnel);
    نتیجہ = CURLE_COULDNT_CONNECT;
    توڑو;
  حالت PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, ڈیٹا,
                       (bool)ctx->udp_tunnel);
    توڑو;
  }
  واپس نتیجہ;
}

ساکن CURLcode H2_CONNECT(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا,
                           ساخت tunnel_دھارا *ts)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  اگر(ctx->conn_closed) {
    failf(ڈیٹا, "proxy closed connection");
    واپس CURLE_COULDNT_CONNECT;
  }

  کرو {
    انتخاب(ts->state) {
    حالت H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(ڈیٹا, cf, "[0] CONNECT start for %s", ts->authority);
      نتیجہ = submit_CONNECT(cf, ڈیٹا, ts);
      اگر(نتیجہ)
        جاؤ برآمد;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, ڈیٹا,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    حالت H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      نتیجہ = proxy_h2_پیش_رفت_ingress(cf, ڈیٹا);
      اگر(!نتیجہ)
        نتیجہ = proxy_h2_پیش_رفت_egress(cf, ڈیٹا);
      اگر(نتیجہ && نتیجہ != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, ڈیٹا,
                           (bool)ctx->udp_tunnel);
        توڑو;
      }

      اگر(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, ڈیٹا,
                           (bool)ctx->udp_tunnel);
      }
      ورنہ {
        نتیجہ = CURLE_OK;
        جاؤ برآمد;
      }
      FALLTHROUGH();

    حالت H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      نتیجہ = inspect_response(cf, ڈیٹا, ts);
      اگر(نتیجہ)
        جاؤ برآمد;
      توڑو;

    حالت H2_TUNNEL_ESTABLISHED:
      واپس CURLE_OK;

    حالت H2_TUNNEL_FAILED:
      واپس CURLE_RECV_ERROR;

    طےشدہ:
      توڑو;
    }

  } جب(ts->state == H2_TUNNEL_INIT);

برآمد:
  اگر((نتیجہ && (نتیجہ != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, ڈیٹا,
                       (bool)ctx->udp_tunnel);
  واپس نتیجہ;
}

/*
 * Initialize the cfilter context
 */
ساکن CURLcode cf_h2_proxy_ctx_init(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  nghttp2_session_callbacks *cbs = تہی;
  صحیح rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, حجم(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_سائز, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_سائز, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  اگر(tunnel_دھارا_init(&ctx->tunnel, ctx->dest))
    جاؤ برآمد;

  rc = nghttp2_session_callbacks_new(&cbs);
  اگر(rc) {
    failf(ڈیٹا, "Could not initialize nghttp2 callbacks");
    جاؤ برآمد;
  }

  nghttp2_session_callbacks_سیٹ_send_callback(cbs, on_session_send);
  nghttp2_session_callbacks_سیٹ_on_چوکھٹا_recv_callback(
    cbs, proxy_h2_on_چوکھٹا_recv);
#اگر_تعریف_شدہ CURLVERBOSE
  nghttp2_session_callbacks_سیٹ_on_چوکھٹا_send_callback(cbs,
                                                       proxy_h2_on_چوکھٹا_send);
#اگر_ختم
  nghttp2_session_callbacks_سیٹ_on_ڈیٹا_chunk_recv_callback(
    cbs, tunnel_recv_callback);
  nghttp2_session_callbacks_سیٹ_on_دھارا_close_callback(
    cbs, proxy_h2_on_دھارا_close);
  nghttp2_session_callbacks_سیٹ_on_سرخی_callback(cbs, proxy_h2_on_سرخی);

  /* The nghttp2 session is not yet setup, do it */
  rc = proxy_h2_client_new(cf, cbs);
  اگر(rc) {
    failf(ڈیٹا, "Could not initialize nghttp2");
    جاؤ برآمد;
  }

  {
    nghttp2_settings_اندراج iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].قدر = Curl_multi_max_concurrent_streams(ڈیٹا->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_کھڑکی_سائز;
    iv[1].قدر = H2_TUNNEL_کھڑکی_سائز;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ڈالو;
    iv[2].قدر = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    اگر(rc) {
      failf(ڈیٹا, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      نتیجہ = CURLE_HTTP2;
      جاؤ برآمد;
    }
  }

  rc = Curl_ngh2_session_سیٹ_local_کھڑکی_سائز(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_کھڑکی_سائز);
  اگر(rc) {
    failf(ڈیٹا, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    نتیجہ = CURLE_HTTP2;
    جاؤ برآمد;
  }

  /* all set, traffic will be send on connect */
  نتیجہ = CURLE_OK;

برآمد:
  اگر(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(ڈیٹا, cf, "[0] init proxy ctx -> %d", (صحیح)نتیجہ);
  واپس نتیجہ;
}

ساکن CURLcode cf_h2_proxy_connect(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    bool *مکمل)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  ساخت cf_call_ڈیٹا save;
  ساخت tunnel_دھارا *ts = &ctx->tunnel;

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  /* Connect the lower filters first */
  اگر(!cf->اگلا->connected) {
    نتیجہ = Curl_conn_cf_connect(cf->اگلا, ڈیٹا, مکمل);
    اگر(نتیجہ || !*مکمل)
      واپس نتیجہ;
  }

  *مکمل = FALSE;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  اگر(!ctx->ngh2.s) {
    نتیجہ = cf_h2_proxy_ctx_init(cf, ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  DEBUGASSERT(ts->authority);

  اگر(Curl_timeleft_ms(ڈیٹا) < 0) {
    failf(ڈیٹا, "Proxy CONNECT aborted due to timeout");
    نتیجہ = CURLE_OPERATION_TIMEDOUT;
    جاؤ برآمد;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  نتیجہ = H2_CONNECT(cf, ڈیٹا, ts);

برآمد:
  *مکمل = (نتیجہ == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  اگر(*مکمل) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&ڈیٹا->req, ڈیٹا);
    Curl_client_reset(ڈیٹا);
  }
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن خالی cf_h2_proxy_destroy(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;

  (خالی)ڈیٹا;
  اگر(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = تہی;
  }
}

ساکن CURLcode cf_h2_proxy_shutdown(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  CURLcode نتیجہ;
  صحیح rv;

  اگر(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);

  اگر(!ctx->بھیجا_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ثابت uint8_t *)"shutdown", حجم("shutdown"));
    اگر(rv) {
      failf(ڈیٹا, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
    ctx->بھیجا_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  نتیجہ = CURLE_OK;
  اگر(Curl_ngh2_session_want_write(&ctx->ngh2))
    نتیجہ = proxy_h2_پیش_رفت_egress(cf, ڈیٹا);
  اگر(!نتیجہ && Curl_ngh2_session_want_read(&ctx->ngh2))
    نتیجہ = proxy_h2_پیش_رفت_ingress(cf, ڈیٹا);

  *مکمل = (ctx->conn_closed ||
           (!نتیجہ && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
برآمد:
  CF_ڈیٹا_RESTORE(cf, save);
  cf->shutdown = (نتیجہ || *مکمل);
  واپس نتیجہ;
}

ساکن bool cf_h2_proxy_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                                     ثابت ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  اگر((ctx && !Curl_bufq_is_خالی_خانے(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_خالی_خانے(&ctx->tunnel.recvbuf)))
    واپس TRUE;
  واپس cf->اگلا ? cf->اگلا->cft->has_ڈیٹا_pending(cf->اگلا, ڈیٹا) : FALSE;
}

ساکن CURLcode cf_h2_proxy_adjust_pollset(ساخت Curl_cfilter *cf,
                                           ساخت Curl_easy *ڈیٹا,
                                           ساخت easy_pollset *ps)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, ڈیٹا);
  bool want_recv, want_send;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_خالی_خانے(&ctx->outbufq) ||
                !Curl_bufq_is_خالی_خانے(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  ورنہ
    Curl_pollset_check(ڈیٹا, ps, sock, &want_recv, &want_send);

  اگر(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
    bool c_exhaust, s_exhaust;

    CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
    c_exhaust = !Curl_ngh2_session_get_remote_کھڑکی_سائز(&ctx->ngh2);
    s_exhaust = ctx->tunnel.دھارا_id >= 0 &&
                !Curl_ngh2_session_get_دھارا_remote_کھڑکی_سائز(
                  &ctx->ngh2, ctx->tunnel.دھارا_id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_خالی_خانے(&ctx->outbufq) ||
                !Curl_bufq_is_خالی_خانے(&ctx->tunnel.sendbuf);

    نتیجہ = Curl_pollset_سیٹ(ڈیٹا, ps, sock, want_recv, want_send);
    CURL_TRC_CF(ڈیٹا, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (صحیح)نتیجہ);
    CF_ڈیٹا_RESTORE(cf, save);
  }
  ورنہ اگر(ctx->بھیجا_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_خالی_خانے(&ctx->outbufq) ||
                !Curl_bufq_is_خالی_خانے(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    نتیجہ = Curl_pollset_سیٹ(ڈیٹا, ps, sock, want_recv, want_send);
    CURL_TRC_CF(ڈیٹا, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (صحیح)نتیجہ);
    CF_ڈیٹا_RESTORE(cf, save);
  }
  واپس نتیجہ;
}

ساکن CURLcode h2_handle_tunnel_close(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا,
                                       size_t *pnread)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  اگر(ctx->tunnel.error) {
    failf(ڈیٹا, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.دھارا_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    واپس CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(ڈیٹا, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.دھارا_id);
  واپس CURLE_OK;
}

ساکن CURLcode tunnel_recv(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                            حرف *buf, size_t len, size_t *pnread)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_AGAIN;

  *pnread = 0;
  اگر(!Curl_bufq_is_خالی_خانے(&ctx->tunnel.recvbuf))
    نتیجہ = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  ورنہ {
    اگر(ctx->tunnel.closed) {
      نتیجہ = h2_handle_tunnel_close(cf, ڈیٹا, pnread);
    }
    ورنہ اگر(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_خالی_خانے(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.دھارا_id)) {
      نتیجہ = CURLE_RECV_ERROR;
    }
    ورنہ
      نتیجہ = CURLE_AGAIN;
  }

  CURL_TRC_CF(ڈیٹا, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.دھارا_id, len, (صحیح)نتیجہ, *pnread);
  واپس نتیجہ;
}

ساکن CURLcode cf_h2_proxy_recv(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 حرف *buf, size_t len,
                                 size_t *pnread)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  CURLcode نتیجہ;

  *pnread = 0;
  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);

  اگر(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    نتیجہ = CURLE_RECV_ERROR;
    جاؤ برآمد;
  }

  اگر(Curl_bufq_is_خالی_خانے(&ctx->tunnel.recvbuf)) {
    نتیجہ = proxy_h2_پیش_رفت_ingress(cf, ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  نتیجہ = tunnel_recv(cf, ڈیٹا, buf, len, pnread);

  اگر(!نتیجہ) {
    CURL_TRC_CF(ڈیٹا, cf, "[%d] increase window by %zu",
                ctx->tunnel.دھارا_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.دھارا_id, *pnread);
  }

  نتیجہ = Curl_1st_fatal(نتیجہ, proxy_h2_پیش_رفت_egress(cf, ڈیٹا));

برآمد:
  اگر((!Curl_bufq_is_خالی_خانے(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_خالی_خانے(&ctx->tunnel.sendbuf)) &&
     (!نتیجہ || (نتیجہ == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, ڈیٹا, &ctx->tunnel);
  }
  CURL_TRC_CF(ڈیٹا, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.دھارا_id, len, (صحیح)نتیجہ, *pnread);
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن CURLcode cf_h2_proxy_send(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ثابت uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  CURLcode نتیجہ;

  (خالی)eos;
  *pnwritten = 0;
  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);

  اگر(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ برآمد;
  }

  اگر(ctx->tunnel.closed) {
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ برآمد;
  }

  نتیجہ = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(ڈیٹا, cf, "cf_send(), bufq_write %d, %zu", (صحیح)نتیجہ,
              *pnwritten);
  اگر(نتیجہ && (نتیجہ != CURLE_AGAIN))
    جاؤ برآمد;

  اگر(!Curl_bufq_is_خالی_خانے(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_ڈیٹا(&ctx->ngh2, ctx->tunnel.دھارا_id))) {
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
  }

  نتیجہ = Curl_1st_fatal(نتیجہ, proxy_h2_پیش_رفت_ingress(cf, ڈیٹا));
  نتیجہ = Curl_1st_fatal(نتیجہ, proxy_h2_پیش_رفت_egress(cf, ڈیٹا));

  اگر(!نتیجہ && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    اگر(ctx->tunnel.closed) {
      نتیجہ = CURLE_SEND_ERROR;
    }
    ورنہ {
      CURL_TRC_CF(ڈیٹا, cf, "[0] send: nothing to do in this session");
      نتیجہ = CURLE_HTTP2;
    }
  }

برآمد:
  اگر((!Curl_bufq_is_خالی_خانے(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_خالی_خانے(&ctx->tunnel.sendbuf)) &&
     (!نتیجہ || (نتیجہ == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, ڈیٹا, &ctx->tunnel);
  }
  CURL_TRC_CF(ڈیٹا, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.دھارا_id, len, (صحیح)نتیجہ, *pnwritten,
              Curl_ngh2_session_get_دھارا_remote_کھڑکی_سائز(
                &ctx->ngh2, ctx->tunnel.دھارا_id),
              Curl_ngh2_session_get_remote_کھڑکی_سائز(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن CURLcode cf_h2_proxy_flush(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  CURLcode نتیجہ = CURLE_OK;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  اگر(!Curl_bufq_is_خالی_خانے(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_ڈیٹا(&ctx->ngh2, ctx->tunnel.دھارا_id))) {
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
  }

  نتیجہ = proxy_h2_پیش_رفت_egress(cf, ڈیٹا);

برآمد:
  CURL_TRC_CF(ڈیٹا, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.دھارا_id, (صحیح)نتیجہ,
              Curl_ngh2_session_get_دھارا_remote_کھڑکی_سائز(
                &ctx->ngh2, ctx->tunnel.دھارا_id),
              Curl_ngh2_session_get_remote_کھڑکی_سائز(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن bool proxy_h2_connisalive(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 bool *داخلہ_pending)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *داخلہ_pending = FALSE;
  اگر(!cf->اگلا || !cf->اگلا->cft->is_alive(cf->اگلا, ڈیٹا, داخلہ_pending))
    واپس FALSE;

  اگر(*داخلہ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode نتیجہ;
    size_t nread;

    *داخلہ_pending = FALSE;
    نتیجہ = Curl_cf_recv_bufq(cf->اگلا, ڈیٹا, &ctx->inbufq, 0, &nread);
    اگر(!نتیجہ) {
      اگر(proxy_h2_process_pending_داخلہ(cf, ڈیٹا))
        /* immediate error, considered dead */
        alive = FALSE;
      ورنہ {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    ورنہ اگر(نتیجہ != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  واپس alive;
}

ساکن bool cf_h2_proxy_is_alive(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 bool *داخلہ_pending)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  ساخت cf_call_ڈیٹا save;

  *داخلہ_pending = FALSE;
  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, ڈیٹا, داخلہ_pending));
  CURL_TRC_CF(ڈیٹا, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *داخلہ_pending);
  CF_ڈیٹا_RESTORE(cf, save);
  واپس alive;
}

ساکن CURLcode cf_h2_proxy_query(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  صحیح query, صحیح *pres1, خالی *pres2)
{
  ساخت cf_h2_proxy_ctx *ctx = cf->ctx;

  انتخاب(query) {
  حالت CF_QUERY_HOST_پورٹ:
    *pres1 = (صحیح)ctx->dest->پورٹ;
    *((ثابت حرف **)pres2) = ctx->dest->hostname;
    واپس CURLE_OK;
  حالت CF_QUERY_NEED_FLUSH: {
    اگر(!Curl_bufq_is_خالی_خانے(&ctx->outbufq) ||
       !Curl_bufq_is_خالی_خانے(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(ڈیٹا, cf, "needs flush");
      *pres1 = TRUE;
      واپس CURLE_OK;
    }
    توڑو;
  }
  حالت CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = تہی;
    واپس CURLE_OK;
  }
  طےشدہ:
    توڑو;
  }
  واپس cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, ڈیٹا, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساکن CURLcode cf_h2_proxy_cntrl(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  صحیح event, صحیح arg1, خالی *arg2)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت cf_call_ڈیٹا save;

  (خالی)arg1;
  (خالی)arg2;

  انتخاب(event) {
  حالت CF_CTRL_FLUSH:
    CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
    نتیجہ = cf_h2_proxy_flush(cf, ڈیٹا);
    CF_ڈیٹا_RESTORE(cf, save);
    توڑو;
  طےشدہ:
    توڑو;
  }
  واپس نتیجہ;
}

ساخت Curl_cftype Curl_cft_h2_proxy = {
  "H2-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  cf_h2_proxy_destroy,
  cf_h2_proxy_connect,
  cf_h2_proxy_shutdown,
  cf_h2_proxy_adjust_pollset,
  cf_h2_proxy_ڈیٹا_pending,
  cf_h2_proxy_send,
  cf_h2_proxy_recv,
  cf_h2_proxy_cntrl,
  cf_h2_proxy_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h2_proxy_query,
};

CURLcode Curl_cf_h2_proxy_داخل_کرو_after(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا,
                                       ساخت Curl_peer *peer,
                                       ساخت Curl_peer *dest,
                                       bool udp_tunnel)
{
  ساخت Curl_cfilter *cf_h2_proxy = تہی;
  ساخت cf_h2_proxy_ctx *ctx;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  (خالی)ڈیٹا;
  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx)
    جاؤ برآمد;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  نتیجہ = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;
  ctx = تہی;
  Curl_conn_cf_داخل_کرو_after(cf, cf_h2_proxy);

برآمد:
  cf_h2_proxy_ctx_free(ctx);
  واپس نتیجہ;
}

#اگر_ختم /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
