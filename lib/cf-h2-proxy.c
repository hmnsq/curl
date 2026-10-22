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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_NGHTTP2)

#ضمن <nghttp2/nghttp2.h>

#ضمن "urldata.h"
#ضمن "url.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "curl_trc.h"
#ضمن "bufq.h"
#ضمن "curlx/dynbuf.h"
#ضمن "dynhds.h"
#ضمن "http2.h"
#ضمن "http_proxy.h"
#ضمن "multiif.h"
#ضمن "ngh2_shim.h"
#ضمن "sendf.h"
#ضمن "select.h"
#ضمن "cf-h2-proxy.h"

#تعریف_کړه PROXY_H2_CHUNK_اندازه  (16 * 1024)

#تعریف_کړه PROXY_HTTP2_HUGE_کړکۍ_اندازه (100 * 1024 * 1024)
#تعریف_کړه H2_TUNNEL_کړکۍ_اندازه        (10 * 1024 * 1024)

#تعریف_کړه PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_کړکۍ_اندازه / PROXY_H2_CHUNK_اندازه)
#تعریف_کړه PROXY_H2_NW_SEND_CHUNKS   1

#تعریف_کړه H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_کړکۍ_اندازه / PROXY_H2_CHUNK_اندازه)
#تعریف_کړه H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_اندازه)


تعريفنوع تعداد {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

بنية tunnel_بهیر {
  بنية http_resp *resp;
  بنية bufq recvbuf;
  بنية bufq sendbuf;
  حرف *authority;
  int32_t بهیر_id;
  uint32_t error;
  h2_tunnel_state state;
  بټ(has_final_response);
  بټ(closed);
  بټ(reset);
};

ساكن CURLcode tunnel_بهیر_init(بنية tunnel_بهیر *ts,
                                   بنية Curl_peer *dest)
{
  ts->state = H2_TUNNEL_INIT;
  ts->بهیر_id = -1;
  Curl_bufq_init2(&ts->recvbuf, PROXY_H2_CHUNK_اندازه, H2_TUNNEL_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد);
  Curl_bufq_init(&ts->sendbuf, PROXY_H2_CHUNK_اندازه, H2_TUNNEL_SEND_CHUNKS);

  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->پورټ);
  إذا(!ts->authority)
    أرجع CURLE_بهر_د_حافظه;

  أرجع CURLE_OK;
}

ساكن فراغ tunnel_بهیر_reset(بنية tunnel_بهیر *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = عدم;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->بهیر_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

ساكن فراغ tunnel_بهیر_clear(بنية tunnel_بهیر *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, حجم(*ts));
  ts->state = H2_TUNNEL_INIT;
}

ساكن فراغ h2_tunnel_go_state(بنية Curl_cfilter *cf,
                               بنية tunnel_بهیر *ts,
                               h2_tunnel_state new_state,
                               بنية Curl_easy *معلومات,
                               bool udp_tunnel)
{
  (فراغ)cf;
  (فراغ)udp_tunnel;

  إذا(ts->state == new_state)
    أرجع;
  /* leaving this one */
  بدل(ts->state) {
  حالة H2_TUNNEL_CONNECT:
    معلومات->req.ignorebody = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }
  /* entering this one */
  بدل(new_state) {
  حالة H2_TUNNEL_INIT:
    CURL_TRC_CF(معلومات, cf, "[%d] new tunnel state 'init'", ts->بهیر_id);
    tunnel_بهیر_reset(ts);
    اكسر;

  حالة H2_TUNNEL_CONNECT:
    CURL_TRC_CF(معلومات, cf, "[%d] new tunnel state 'connect'", ts->بهیر_id);
    ts->state = H2_TUNNEL_CONNECT;
    اكسر;

  حالة H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(معلومات, cf, "[%d] new tunnel state 'response'", ts->بهیر_id);
    ts->state = H2_TUNNEL_RESPONSE;
    اكسر;

  حالة H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(معلومات, cf, "[%d] new tunnel state 'established'",
                ts->بهیر_id);
    infof(معلومات, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    معلومات->state.authproxy.بشپړ = TRUE;
    معلومات->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  حالة H2_TUNNEL_FAILED:
    إذا(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(معلومات, cf, "[%d] new tunnel state 'failed'", ts->بهیر_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(معلومات->req.hd_proxy_auth);
    اكسر;
  }
}

بنية cf_h2_proxy_ctx {
  بنية curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  بنية cf_call_معلومات call_معلومات;

  بنية bufq inbufq;  /* network receive buffer */
  بنية bufq outbufq; /* network send buffer */

  بنية Curl_peer *peer; /* proxy we talk to */
  بنية Curl_peer *dest; /* where to tunnel to */
  بنية tunnel_بهیر tunnel; /* our tunnel CONNECT stream */
  int32_t goaway_error;
  int32_t remote_max_sid;
  بټ(conn_closed);
  بټ(rcvd_goaway);
  بټ(لېږل_شوی_goaway);
  بټ(nw_بهر_blocked);
  بټ(udp_tunnel);
};

/* How to access `call_data` from a cf_h2 filter */
#تعریف_لغوه CF_CTX_CALL_معلومات
#تعریف_کړه CF_CTX_CALL_معلومات(cf) ((بنية cf_h2_proxy_ctx *)(cf)->ctx)->call_معلومات

ساكن فراغ cf_h2_proxy_ctx_clear(بنية cf_h2_proxy_ctx *ctx)
{
  بنية cf_call_معلومات save = ctx->call_معلومات;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_بهیر_clear(&ctx->tunnel);
  memset(ctx, 0, حجم(*ctx));
  ctx->call_معلومات = save;
}

ساكن فراغ cf_h2_proxy_ctx_free(بنية cf_h2_proxy_ctx *ctx)
{
  إذا(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

ساكن فراغ drain_tunnel(بنية Curl_cfilter *cf,
                         بنية Curl_easy *معلومات,
                         بنية tunnel_بهیر *tunnel)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  (فراغ)cf;
  إذا(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_تشه(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_تشه(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(معلومات);
}

ساكن CURLcode proxy_h2_nw_بهر_writer(فراغ *writer_ctx,
                                       ثابت uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  بنية Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  إذا(cf) {
    بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
    CURLcode پایله;
    پایله = Curl_conn_cf_send(cf->بل, معلومات, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(معلومات, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (صحيح)پایله, *pnwritten);
    أرجع پایله;
  }
  أرجع CURLE_FAILED_INIT;
}

ساكن صحيح proxy_h2_client_new(بنية Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { عدم, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  صحيح rc = nghttp2_option_new(&o);
  إذا(rc)
    أرجع rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_سېټ_no_auto_کړکۍ_update(o, 1);
#إذا NGHTTP2_نسخه_عدد >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_سېټ_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#د_که_پای
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  أرجع rc;
}

ساكن صحيح proxy_h2_should_close_session(بنية cf_h2_proxy_ctx *ctx)
{
  أرجع !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

ساكن CURLcode proxy_h2_nw_بهر_flush(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode پایله;

  إذا(Curl_bufq_is_تشه(&ctx->outbufq))
    أرجع CURLE_OK;

  پایله = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_بهر_writer, cf,
                          &nwritten);
  إذا(پایله) {
    إذا(پایله == CURLE_AGAIN) {
      CURL_TRC_CF(معلومات, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_بهر_blocked = 1;
    }
    أرجع پایله;
  }
  CURL_TRC_CF(معلومات, cf, "[0] nw send buffer flushed");
  أرجع Curl_bufq_is_تشه(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ساكن CURLcode proxy_h2_process_pending_ننوت(بنية Curl_cfilter *cf,
                                               بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  ثابت بینښه حرف *buf;
  size_t blen, nread;
  ssize_t rv;

  طالما(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ثابت uint8_t *)buf, blen);
    CURL_TRC_CF(معلومات, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    إذا(!curlx_sztouz(rv, &nread)) {
      failf(معلومات,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((صحيح)rv));
      أرجع CURLE_RECV_ERROR;
    }
    وإلا إذا(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      اكسر;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    إذا(Curl_bufq_is_تشه(&ctx->inbufq)) {
      CURL_TRC_CF(معلومات, cf, "[0] all data in connection buffer processed");
      اكسر;
    }
    وإلا {
      CURL_TRC_CF(معلومات, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode proxy_h2_پرمختګ_ingress(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  إذا(!Curl_bufq_is_تشه(&ctx->inbufq)) {
    CURL_TRC_CF(معلومات, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    پایله = proxy_h2_process_pending_ننوت(cf, معلومات);
    إذا(پایله)
      أرجع پایله;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  طالما(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_تشه(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_ډک(&ctx->tunnel.recvbuf)) {

    پایله = Curl_cf_recv_bufq(cf->بل, معلومات, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(معلومات, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (صحيح)پایله, nread);
    إذا(پایله) {
      إذا(پایله != CURLE_AGAIN) {
        failf(معلومات, "Failed receiving HTTP2 proxy data");
        أرجع پایله;
      }
      اكسر;
    }
    وإلا إذا(nread == 0) {
      CURL_TRC_CF(معلومات, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      اكسر;
    }

    پایله = proxy_h2_process_pending_ننوت(cf, معلومات);
    إذا(پایله)
      أرجع پایله;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode proxy_h2_پرمختګ_egress(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  صحيح rv = 0;

  ctx->nw_بهر_blocked = 0;
  طالما(!rv && !ctx->nw_بهر_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  إذا(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(معلومات, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    أرجع CURLE_SEND_ERROR;
  }
  أرجع proxy_h2_nw_بهر_flush(cf, معلومات);
}

ساكن ssize_t on_session_send(nghttp2_session *h2,
                               ثابت uint8_t *buf, size_t blen, صحيح flags,
                               فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  size_t nwritten;
  CURLcode پایله = CURLE_OK;

  (فراغ)h2;
  (فراغ)flags;
  DEBUGASSERT(معلومات);

  پایله = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_بهر_writer, cf, &nwritten);
  إذا(پایله) {
    إذا(پایله == CURLE_AGAIN) {
      ctx->nw_بهر_blocked = 1;
      أرجع NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(معلومات, "Failed sending HTTP2 data");
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  إذا(!nwritten)
    أرجع NGHTTP2_ERR_WOULDBLOCK;

  أرجع (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#که_تعریف_شوی CURLVERBOSE
ساكن صحيح proxy_h2_on_چوکاټ_send(nghttp2_session *session,
                                  ثابت nghttp2_چوکاټ *چوکاټ,
                                  فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);

  (فراغ)session;
  DEBUGASSERT(معلومات);
  إذا(Curl_trc_cf_is_verbose(cf, معلومات)) {
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکاټ, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(معلومات, cf, "[%d] -> %s", چوکاټ->hd.بهیر_id, بفر);
  }
  أرجع 0;
}
#د_که_پای /* CURLVERBOSE */

ساكن صحيح proxy_h2_on_چوکاټ_recv(nghttp2_session *session,
                                  ثابت nghttp2_چوکاټ *چوکاټ,
                                  فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  int32_t بهیر_id = چوکاټ->hd.بهیر_id;

  (فراغ)session;
  DEBUGASSERT(معلومات);
#که_تعریف_شوی CURLVERBOSE
  إذا(Curl_trc_cf_is_verbose(cf, معلومات)) {
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکاټ, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(معلومات, cf, "[%d] <- %s", چوکاټ->hd.بهیر_id, بفر);
  }
#د_که_پای /* CURLVERBOSE */

  إذا(!بهیر_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(معلومات);
    بدل(چوکاټ->hd.type) {
    حالة NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      إذا(CURL_REQ_WANT_SEND(معلومات)) {
        drain_tunnel(cf, معلومات, &ctx->tunnel);
      }
      اكسر;
    حالة NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = چوکاټ->goaway.وروستی_بهیر_id;
      إذا(معلومات) {
        infof(معلومات, "received GOAWAY, error=%u, last_stream=%d",
              چوکاټ->goaway.error_کوډ, ctx->remote_max_sid);
      }
      اكسر;
    افتراضي:
      اكسر;
    }
    أرجع 0;
  }

  إذا(بهیر_id != ctx->tunnel.بهیر_id) {
    CURL_TRC_CF(معلومات, cf, "[%d] rcvd FRAME not for tunnel", بهیر_id);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  بدل(چوکاټ->hd.type) {
  حالة NGHTTP2_سرلیکونه:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    إذا(!ctx->tunnel.resp)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(معلومات, cf, "[%d] got http status: %d",
                بهیر_id, ctx->tunnel.resp->حالت);
    إذا(!ctx->tunnel.has_final_response && ctx->tunnel.resp->حالت / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    اكسر;
  حالة NGHTTP2_کړکۍ_UPDATE:
    إذا(CURL_REQ_WANT_SEND(معلومات)) {
      drain_tunnel(cf, معلومات, &ctx->tunnel);
    }
    اكسر;
  حالة NGHTTP2_RST_بهیر:
    إذا(چوکاټ->rst_بهیر.error_کوډ)
      ctx->tunnel.reset = TRUE;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع 0;
}

ساكن صحيح proxy_h2_on_سرلیک(nghttp2_session *session,
                              ثابت nghttp2_چوکاټ *چوکاټ,
                              ثابت uint8_t *name, size_t namelen,
                              ثابت uint8_t *ارزښت, size_t valuelen,
                              uint8_t flags,
                              فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  int32_t بهیر_id = چوکاټ->hd.بهیر_id;
  CURLcode پایله;

  (فراغ)flags;
  (فراغ)session;
  DEBUGASSERT(بهیر_id); /* should never be a zero stream ID here */
  إذا(بهیر_id != ctx->tunnel.بهیر_id) {
    CURL_TRC_CF(معلومات, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", بهیر_id,
                (صحيح)namelen, name, (صحيح)valuelen, ارزښت);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  إذا(چوکاټ->hd.type == NGHTTP2_ټېل_PROMISE)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  إذا(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    أرجع 0;
  }

  إذا(namelen == CURL_CSTRLEN(HTTP_PSEUDO_حالت) &&
     !memcmp(HTTP_PSEUDO_حالت, name, namelen)) {
    int32_t http_حالت;
    بنية http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    پایله = Curl_http_decode_حالت(&http_حالت,
                                    (ثابت حرف *)ارزښت, valuelen);
    إذا(پایله)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    پایله = Curl_http_resp_make(&resp, http_حالت, عدم);
    إذا(پایله)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    إذا(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(معلومات, cf, "[%d] status: HTTP/2 %03d",
                بهیر_id, ctx->tunnel.resp->حالت);
    أرجع 0;
  }

  إذا(!ctx->tunnel.resp)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  پایله = Curl_dynhds_جمع_کړه(&ctx->tunnel.resp->سرلیکونه,
                           (ثابت حرف *)name, namelen,
                           (ثابت حرف *)ارزښت, valuelen);
  إذا(پایله)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(معلومات, cf, "[%d] header: %.*s: %.*s",
              بهیر_id, (صحيح)namelen, name, (صحيح)valuelen, ارزښت);

  أرجع 0; /* 0 is successful */
}

ساكن ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t بهیر_id,
                                    uint8_t *buf, size_t اوږدتیا,
                                    uint32_t *معلومات_flags,
                                    nghttp2_معلومات_سرچینه *سرچینه,
                                    فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  بنية tunnel_بهیر *ts;
  CURLcode پایله;
  size_t nread;

  (فراغ)session;
  (فراغ)سرچینه;
  (فراغ)ctx;

  إذا(!بهیر_id)
    أرجع NGHTTP2_ERR_INVALID_دلیل;

  ts = Curl_ngh2_session_get_بهیر_user_معلومات(&ctx->ngh2, بهیر_id);
  إذا(!ts)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  پایله = Curl_bufq_read(&ts->sendbuf, buf, اوږدتیا, &nread);
  إذا(پایله) {
    إذا(پایله != CURLE_AGAIN)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    أرجع NGHTTP2_ERR_DEFERRED;
  }
  إذا(ts->closed && Curl_bufq_is_تشه(&ts->sendbuf))
    *معلومات_flags = NGHTTP2_معلومات_FLAG_EOF;

  CURL_TRC_CF(معلومات, cf, "[%d] tunnel_send_callback -> %zu",
              ts->بهیر_id, nread);
  أرجع (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

ساكن صحيح tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t بهیر_id,
                                ثابت uint8_t *mem, size_t len, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode پایله;

  (فراغ)flags;
  (فراغ)session;
  DEBUGASSERT(بهیر_id); /* should never be a zero stream ID here */

  إذا(بهیر_id != ctx->tunnel.بهیر_id)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  پایله = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  إذا(پایله) {
    إذا(پایله != CURLE_AGAIN)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
#که_تعریف_شوی DEBUGBUILD
    nwritten = 0;
#د_که_پای
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  أرجع 0;
}

ساكن صحيح proxy_h2_on_بهیر_close(nghttp2_session *session,
                                    int32_t بهیر_id,
                                    uint32_t error_کوډ, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);

  (فراغ)session;

  إذا(بهیر_id != ctx->tunnel.بهیر_id)
    أرجع 0;

  CURL_TRC_CF(معلومات, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              بهیر_id, nghttp2_http2_strerror(error_کوډ), error_کوډ);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_کوډ;
  إذا(error_کوډ)
    ctx->tunnel.reset = TRUE;

  أرجع 0;
}

ساكن CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  بنية Curl_cfilter *cf,
  بنية Curl_easy *معلومات,
  بنية curl_ngh2 *ngh2,
  بنية httpreq *req,
  ثابت nghttp2_لومړیتوب_spec *pri_spec,
  فراغ *بهیر_user_معلومات,
  nghttp2_معلومات_سرچینه_read_callback read_callback,
  فراغ *read_ctx)
{
  بنية dynhds h2_سرلیکونه;
  nghttp2_nv *nva = عدم;
  int32_t بهیر_id = -1;
  size_t nheader;
  CURLcode پایله;

  (فراغ)cf;
  Curl_dynhds_init(&h2_سرلیکونه, MAX_HTTP_RESP_سرلیک_شمېر, DYN_HTTP_REQUEST);
  پایله = Curl_http_req_منزل_h2(&h2_سرلیکونه, req, معلومات);
  إذا(پایله)
    اذهب بهر;

  nva = Curl_dynhds_منزل_nva(&h2_سرلیکونه, &nheader);
  إذا(!nva) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  إذا(read_callback) {
    nghttp2_معلومات_provider معلومات_prd;

    معلومات_prd.read_callback = read_callback;
    معلومات_prd.سرچینه.ptr = read_ctx;
    بهیر_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &معلومات_prd, بهیر_user_معلومات);
  }
  وإلا {
    بهیر_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, عدم, بهیر_user_معلومات);
  }

  إذا(بهیر_id < 0) {
    failf(معلومات, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(بهیر_id), بهیر_id);
    پایله = CURLE_SEND_ERROR;
    اذهب بهر;
  }
  پایله = CURLE_OK;

بهر:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرلیکونه);
  *pstream_id = بهیر_id;
  أرجع پایله;
}

ساكن CURLcode submit_CONNECT(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية tunnel_بهیر *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode پایله;
  بنية httpreq *req = عدم;

  پایله = Curl_http_proxy_create_tunnel_request(&req, cf, معلومات,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  إذا(پایله)
    اذهب بهر;
  پایله = Curl_creader_سېټ_null(معلومات);
  إذا(پایله)
    اذهب بهر;

  پایله = proxy_h2_submit(&ts->بهیر_id, cf, معلومات, &ctx->ngh2, req,
                           عدم, ts, tunnel_send_callback, cf);
  إذا(پایله) {
    CURL_TRC_CF(معلومات, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->بهیر_id, nghttp2_strerror(ts->بهیر_id));
  }

بهر:
  إذا(req)
    Curl_http_req_free(req);
  إذا(پایله)
    failf(معلومات, "Failed sending CONNECT to proxy");
  أرجع پایله;
}

ساكن CURLcode inspect_response(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية tunnel_بهیر *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_پایله res;
  CURLcode پایله;

  پایله = Curl_http_proxy_inspect_tunnel_response(
      cf, معلومات, ts->resp, (bool)ctx->udp_tunnel, &res);
  إذا(پایله)
    أرجع پایله;
  بدل(res) {
  حالة PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, معلومات,
                       (bool)ctx->udp_tunnel);
    اكسر;
  حالة PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, معلومات,
                       (bool)ctx->udp_tunnel);
    پایله = CURLE_COULDNT_CONNECT;
    اكسر;
  حالة PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, معلومات,
                       (bool)ctx->udp_tunnel);
    اكسر;
  }
  أرجع پایله;
}

ساكن CURLcode H2_CONNECT(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات,
                           بنية tunnel_بهیر *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  إذا(ctx->conn_closed) {
    failf(معلومات, "proxy closed connection");
    أرجع CURLE_COULDNT_CONNECT;
  }

  افعل {
    بدل(ts->state) {
    حالة H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(معلومات, cf, "[0] CONNECT start for %s", ts->authority);
      پایله = submit_CONNECT(cf, معلومات, ts);
      إذا(پایله)
        اذهب بهر;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, معلومات,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    حالة H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      پایله = proxy_h2_پرمختګ_ingress(cf, معلومات);
      إذا(!پایله)
        پایله = proxy_h2_پرمختګ_egress(cf, معلومات);
      إذا(پایله && پایله != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, معلومات,
                           (bool)ctx->udp_tunnel);
        اكسر;
      }

      إذا(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, معلومات,
                           (bool)ctx->udp_tunnel);
      }
      وإلا {
        پایله = CURLE_OK;
        اذهب بهر;
      }
      FALLTHROUGH();

    حالة H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      پایله = inspect_response(cf, معلومات, ts);
      إذا(پایله)
        اذهب بهر;
      اكسر;

    حالة H2_TUNNEL_ESTABLISHED:
      أرجع CURLE_OK;

    حالة H2_TUNNEL_FAILED:
      أرجع CURLE_RECV_ERROR;

    افتراضي:
      اكسر;
    }

  } طالما(ts->state == H2_TUNNEL_INIT);

بهر:
  إذا((پایله && (پایله != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, معلومات,
                       (bool)ctx->udp_tunnel);
  أرجع پایله;
}

/*
 * Initialize the cfilter context
 */
ساكن CURLcode cf_h2_proxy_ctx_init(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_بهر_د_حافظه;
  nghttp2_session_callbacks *cbs = عدم;
  صحيح rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, حجم(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_اندازه, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_اندازه, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  إذا(tunnel_بهیر_init(&ctx->tunnel, ctx->dest))
    اذهب بهر;

  rc = nghttp2_session_callbacks_new(&cbs);
  إذا(rc) {
    failf(معلومات, "Could not initialize nghttp2 callbacks");
    اذهب بهر;
  }

  nghttp2_session_callbacks_سېټ_send_callback(cbs, on_session_send);
  nghttp2_session_callbacks_سېټ_on_چوکاټ_recv_callback(
    cbs, proxy_h2_on_چوکاټ_recv);
#که_تعریف_شوی CURLVERBOSE
  nghttp2_session_callbacks_سېټ_on_چوکاټ_send_callback(cbs,
                                                       proxy_h2_on_چوکاټ_send);
#د_که_پای
  nghttp2_session_callbacks_سېټ_on_معلومات_chunk_recv_callback(
    cbs, tunnel_recv_callback);
  nghttp2_session_callbacks_سېټ_on_بهیر_close_callback(
    cbs, proxy_h2_on_بهیر_close);
  nghttp2_session_callbacks_سېټ_on_سرلیک_callback(cbs, proxy_h2_on_سرلیک);

  /* The nghttp2 session is not yet setup, do it */
  rc = proxy_h2_client_new(cf, cbs);
  إذا(rc) {
    failf(معلومات, "Could not initialize nghttp2");
    اذهب بهر;
  }

  {
    nghttp2_settings_ثبت iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].ارزښت = Curl_multi_max_concurrent_streams(معلومات->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_کړکۍ_اندازه;
    iv[1].ارزښت = H2_TUNNEL_کړکۍ_اندازه;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ټېل;
    iv[2].ارزښت = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    إذا(rc) {
      failf(معلومات, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      پایله = CURLE_HTTP2;
      اذهب بهر;
    }
  }

  rc = Curl_ngh2_session_سېټ_local_کړکۍ_اندازه(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_کړکۍ_اندازه);
  إذا(rc) {
    failf(معلومات, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    پایله = CURLE_HTTP2;
    اذهب بهر;
  }

  /* all set, traffic will be send on connect */
  پایله = CURLE_OK;

بهر:
  إذا(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(معلومات, cf, "[0] init proxy ctx -> %d", (صحيح)پایله);
  أرجع پایله;
}

ساكن CURLcode cf_h2_proxy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    bool *بشپړ)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;
  بنية cf_call_معلومات save;
  بنية tunnel_بهیر *ts = &ctx->tunnel;

  إذا(cf->connected) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the lower filters first */
  إذا(!cf->بل->connected) {
    پایله = Curl_conn_cf_connect(cf->بل, معلومات, بشپړ);
    إذا(پایله || !*بشپړ)
      أرجع پایله;
  }

  *بشپړ = FALSE;

  CF_معلومات_SAVE(save, cf, معلومات);
  إذا(!ctx->ngh2.s) {
    پایله = cf_h2_proxy_ctx_init(cf, معلومات);
    إذا(پایله)
      اذهب بهر;
  }
  DEBUGASSERT(ts->authority);

  إذا(Curl_timeleft_ms(معلومات) < 0) {
    failf(معلومات, "Proxy CONNECT aborted due to timeout");
    پایله = CURLE_OPERATION_TIMEDOUT;
    اذهب بهر;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  پایله = H2_CONNECT(cf, معلومات, ts);

بهر:
  *بشپړ = (پایله == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  إذا(*بشپړ) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&معلومات->req, معلومات);
    Curl_client_reset(معلومات);
  }
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

ساكن فراغ cf_h2_proxy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  (فراغ)معلومات;
  إذا(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_h2_proxy_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات, bool *بشپړ)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode پایله;
  صحيح rv;

  إذا(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  CF_معلومات_SAVE(save, cf, معلومات);

  إذا(!ctx->لېږل_شوی_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ثابت uint8_t *)"shutdown", حجم("shutdown"));
    إذا(rv) {
      failf(معلومات, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }
    ctx->لېږل_شوی_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  پایله = CURLE_OK;
  إذا(Curl_ngh2_session_want_write(&ctx->ngh2))
    پایله = proxy_h2_پرمختګ_egress(cf, معلومات);
  إذا(!پایله && Curl_ngh2_session_want_read(&ctx->ngh2))
    پایله = proxy_h2_پرمختګ_ingress(cf, معلومات);

  *بشپړ = (ctx->conn_closed ||
           (!پایله && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
بهر:
  CF_معلومات_RESTORE(cf, save);
  cf->shutdown = (پایله || *بشپړ);
  أرجع پایله;
}

ساكن bool cf_h2_proxy_معلومات_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  إذا((ctx && !Curl_bufq_is_تشه(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_تشه(&ctx->tunnel.recvbuf)))
    أرجع TRUE;
  أرجع cf->بل ? cf->بل->cft->has_معلومات_pending(cf->بل, معلومات) : FALSE;
}

ساكن CURLcode cf_h2_proxy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *معلومات,
                                           بنية easy_pollset *ps)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, معلومات);
  bool want_recv, want_send;
  CURLcode پایله = CURLE_OK;

  إذا(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_تشه(&ctx->outbufq) ||
                !Curl_bufq_is_تشه(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  وإلا
    Curl_pollset_check(معلومات, ps, sock, &want_recv, &want_send);

  إذا(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
    bool c_exhaust, s_exhaust;

    CF_معلومات_SAVE(save, cf, معلومات);
    c_exhaust = !Curl_ngh2_session_get_remote_کړکۍ_اندازه(&ctx->ngh2);
    s_exhaust = ctx->tunnel.بهیر_id >= 0 &&
                !Curl_ngh2_session_get_بهیر_remote_کړکۍ_اندازه(
                  &ctx->ngh2, ctx->tunnel.بهیر_id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_تشه(&ctx->outbufq) ||
                !Curl_bufq_is_تشه(&ctx->tunnel.sendbuf);

    پایله = Curl_pollset_سېټ(معلومات, ps, sock, want_recv, want_send);
    CURL_TRC_CF(معلومات, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (صحيح)پایله);
    CF_معلومات_RESTORE(cf, save);
  }
  وإلا إذا(ctx->لېږل_شوی_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_معلومات_SAVE(save, cf, معلومات);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_تشه(&ctx->outbufq) ||
                !Curl_bufq_is_تشه(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    پایله = Curl_pollset_سېټ(معلومات, ps, sock, want_recv, want_send);
    CURL_TRC_CF(معلومات, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (صحيح)پایله);
    CF_معلومات_RESTORE(cf, save);
  }
  أرجع پایله;
}

ساكن CURLcode h2_handle_tunnel_close(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  إذا(ctx->tunnel.error) {
    failf(معلومات, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.بهیر_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    أرجع CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(معلومات, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.بهیر_id);
  أرجع CURLE_OK;
}

ساكن CURLcode tunnel_recv(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                            حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_AGAIN;

  *pnread = 0;
  إذا(!Curl_bufq_is_تشه(&ctx->tunnel.recvbuf))
    پایله = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  وإلا {
    إذا(ctx->tunnel.closed) {
      پایله = h2_handle_tunnel_close(cf, معلومات, pnread);
    }
    وإلا إذا(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_تشه(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.بهیر_id)) {
      پایله = CURLE_RECV_ERROR;
    }
    وإلا
      پایله = CURLE_AGAIN;
  }

  CURL_TRC_CF(معلومات, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.بهیر_id, len, (صحيح)پایله, *pnread);
  أرجع پایله;
}

ساكن CURLcode cf_h2_proxy_recv(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 حرف *buf, size_t len,
                                 size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode پایله;

  *pnread = 0;
  CF_معلومات_SAVE(save, cf, معلومات);

  إذا(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    پایله = CURLE_RECV_ERROR;
    اذهب بهر;
  }

  إذا(Curl_bufq_is_تشه(&ctx->tunnel.recvbuf)) {
    پایله = proxy_h2_پرمختګ_ingress(cf, معلومات);
    إذا(پایله)
      اذهب بهر;
  }

  پایله = tunnel_recv(cf, معلومات, buf, len, pnread);

  إذا(!پایله) {
    CURL_TRC_CF(معلومات, cf, "[%d] increase window by %zu",
                ctx->tunnel.بهیر_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.بهیر_id, *pnread);
  }

  پایله = Curl_1st_fatal(پایله, proxy_h2_پرمختګ_egress(cf, معلومات));

بهر:
  إذا((!Curl_bufq_is_تشه(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_تشه(&ctx->tunnel.sendbuf)) &&
     (!پایله || (پایله == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, معلومات, &ctx->tunnel);
  }
  CURL_TRC_CF(معلومات, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.بهیر_id, len, (صحيح)پایله, *pnread);
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

ساكن CURLcode cf_h2_proxy_send(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 ثابت uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode پایله;

  (فراغ)eos;
  *pnwritten = 0;
  CF_معلومات_SAVE(save, cf, معلومات);

  إذا(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    پایله = CURLE_SEND_ERROR;
    اذهب بهر;
  }

  إذا(ctx->tunnel.closed) {
    پایله = CURLE_SEND_ERROR;
    اذهب بهر;
  }

  پایله = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(معلومات, cf, "cf_send(), bufq_write %d, %zu", (صحيح)پایله,
              *pnwritten);
  إذا(پایله && (پایله != CURLE_AGAIN))
    اذهب بهر;

  إذا(!Curl_bufq_is_تشه(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_معلومات(&ctx->ngh2, ctx->tunnel.بهیر_id))) {
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }
  }

  پایله = Curl_1st_fatal(پایله, proxy_h2_پرمختګ_ingress(cf, معلومات));
  پایله = Curl_1st_fatal(پایله, proxy_h2_پرمختګ_egress(cf, معلومات));

  إذا(!پایله && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    إذا(ctx->tunnel.closed) {
      پایله = CURLE_SEND_ERROR;
    }
    وإلا {
      CURL_TRC_CF(معلومات, cf, "[0] send: nothing to do in this session");
      پایله = CURLE_HTTP2;
    }
  }

بهر:
  إذا((!Curl_bufq_is_تشه(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_تشه(&ctx->tunnel.sendbuf)) &&
     (!پایله || (پایله == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, معلومات, &ctx->tunnel);
  }
  CURL_TRC_CF(معلومات, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.بهیر_id, len, (صحيح)پایله, *pnwritten,
              Curl_ngh2_session_get_بهیر_remote_کړکۍ_اندازه(
                &ctx->ngh2, ctx->tunnel.بهیر_id),
              Curl_ngh2_session_get_remote_کړکۍ_اندازه(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

ساكن CURLcode cf_h2_proxy_flush(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode پایله = CURLE_OK;

  CF_معلومات_SAVE(save, cf, معلومات);
  إذا(!Curl_bufq_is_تشه(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_معلومات(&ctx->ngh2, ctx->tunnel.بهیر_id))) {
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }
  }

  پایله = proxy_h2_پرمختګ_egress(cf, معلومات);

بهر:
  CURL_TRC_CF(معلومات, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.بهیر_id, (صحيح)پایله,
              Curl_ngh2_session_get_بهیر_remote_کړکۍ_اندازه(
                &ctx->ngh2, ctx->tunnel.بهیر_id),
              Curl_ngh2_session_get_remote_کړکۍ_اندازه(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

ساكن bool proxy_h2_connisalive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 bool *ننوت_pending)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ننوت_pending = FALSE;
  إذا(!cf->بل || !cf->بل->cft->is_alive(cf->بل, معلومات, ننوت_pending))
    أرجع FALSE;

  إذا(*ننوت_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode پایله;
    size_t nread;

    *ننوت_pending = FALSE;
    پایله = Curl_cf_recv_bufq(cf->بل, معلومات, &ctx->inbufq, 0, &nread);
    إذا(!پایله) {
      إذا(proxy_h2_process_pending_ننوت(cf, معلومات))
        /* immediate error, considered dead */
        alive = FALSE;
      وإلا {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    وإلا إذا(پایله != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  أرجع alive;
}

ساكن bool cf_h2_proxy_is_alive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 bool *ننوت_pending)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  بنية cf_call_معلومات save;

  *ننوت_pending = FALSE;
  CF_معلومات_SAVE(save, cf, معلومات);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, معلومات, ننوت_pending));
  CURL_TRC_CF(معلومات, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *ننوت_pending);
  CF_معلومات_RESTORE(cf, save);
  أرجع alive;
}

ساكن CURLcode cf_h2_proxy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_HOST_پورټ:
    *pres1 = (صحيح)ctx->dest->پورټ;
    *((ثابت حرف **)pres2) = ctx->dest->hostname;
    أرجع CURLE_OK;
  حالة CF_QUERY_NEED_FLUSH: {
    إذا(!Curl_bufq_is_تشه(&ctx->outbufq) ||
       !Curl_bufq_is_تشه(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(معلومات, cf, "needs flush");
      *pres1 = TRUE;
      أرجع CURLE_OK;
    }
    اكسر;
  }
  حالة CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = عدم;
    أرجع CURLE_OK;
  }
  افتراضي:
    اكسر;
  }
  أرجع cf->بل ?
    cf->بل->cft->query(cf->بل, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_h2_proxy_cntrl(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode پایله = CURLE_OK;
  بنية cf_call_معلومات save;

  (فراغ)arg1;
  (فراغ)arg2;

  بدل(event) {
  حالة CF_CTRL_FLUSH:
    CF_معلومات_SAVE(save, cf, معلومات);
    پایله = cf_h2_proxy_flush(cf, معلومات);
    CF_معلومات_RESTORE(cf, save);
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع پایله;
}

بنية Curl_cftype Curl_cft_h2_proxy = {
  "H2-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  cf_h2_proxy_destroy,
  cf_h2_proxy_connect,
  cf_h2_proxy_shutdown,
  cf_h2_proxy_adjust_pollset,
  cf_h2_proxy_معلومات_pending,
  cf_h2_proxy_send,
  cf_h2_proxy_recv,
  cf_h2_proxy_cntrl,
  cf_h2_proxy_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h2_proxy_query,
};

CURLcode Curl_cf_h2_proxy_ورننباسه_after(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *dest,
                                       bool udp_tunnel)
{
  بنية Curl_cfilter *cf_h2_proxy = عدم;
  بنية cf_h2_proxy_ctx *ctx;
  CURLcode پایله = CURLE_بهر_د_حافظه;

  (فراغ)معلومات;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب بهر;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  پایله = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  إذا(پایله)
    اذهب بهر;
  ctx = عدم;
  Curl_conn_cf_ورننباسه_after(cf, cf_h2_proxy);

بهر:
  cf_h2_proxy_ctx_free(ctx);
  أرجع پایله;
}

#د_که_پای /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
