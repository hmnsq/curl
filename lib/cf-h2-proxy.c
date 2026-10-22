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

#تعریف_کرو PROXY_H2_CHUNK_ناپ  (16 * 1024)

#تعریف_کرو PROXY_HTTP2_HUGE_کھڑکی_ناپ (100 * 1024 * 1024)
#تعریف_کرو H2_TUNNEL_کھڑکی_ناپ        (10 * 1024 * 1024)

#تعریف_کرو PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_کھڑکی_ناپ / PROXY_H2_CHUNK_ناپ)
#تعریف_کرو PROXY_H2_NW_SEND_CHUNKS   1

#تعریف_کرو H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_کھڑکی_ناپ / PROXY_H2_CHUNK_ناپ)
#تعریف_کرو H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_ناپ)


تعريفنوع تعداد {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

بنية tunnel_دھارا {
  بنية http_resp *resp;
  بنية bufq recvbuf;
  بنية bufq sendbuf;
  حرف *authority;
  int32_t دھارا_id;
  uint32_t error;
  h2_tunnel_state state;
  ثنائی_ہندسہ(has_final_response);
  ثنائی_ہندسہ(closed);
  ثنائی_ہندسہ(reset);
};

ساكن CURLcode tunnel_دھارا_init(بنية tunnel_دھارا *ts,
                                   بنية Curl_peer *dest)
{
  ts->state = H2_TUNNEL_INIT;
  ts->دھارا_id = -1;
  Curl_bufq_init2(&ts->recvbuf, PROXY_H2_CHUNK_ناپ, H2_TUNNEL_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندی);
  Curl_bufq_init(&ts->sendbuf, PROXY_H2_CHUNK_ناپ, H2_TUNNEL_SEND_CHUNKS);

  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->پورٹ);
  إذا(!ts->authority)
    أرجع CURLE_باہرلا_دا_حافظہ;

  أرجع CURLE_OK;
}

ساكن فراغ tunnel_دھارا_reset(بنية tunnel_دھارا *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = عدم;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->دھارا_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

ساكن فراغ tunnel_دھارا_clear(بنية tunnel_دھارا *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, حجم(*ts));
  ts->state = H2_TUNNEL_INIT;
}

ساكن فراغ h2_tunnel_go_state(بنية Curl_cfilter *cf,
                               بنية tunnel_دھارا *ts,
                               h2_tunnel_state new_state,
                               بنية Curl_easy *انکڑے,
                               bool udp_tunnel)
{
  (فراغ)cf;
  (فراغ)udp_tunnel;

  إذا(ts->state == new_state)
    أرجع;
  /* leaving this one */
  بدل(ts->state) {
  حالة H2_TUNNEL_CONNECT:
    انکڑے->req.ignorebody = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }
  /* entering this one */
  بدل(new_state) {
  حالة H2_TUNNEL_INIT:
    CURL_TRC_CF(انکڑے, cf, "[%d] new tunnel state 'init'", ts->دھارا_id);
    tunnel_دھارا_reset(ts);
    اكسر;

  حالة H2_TUNNEL_CONNECT:
    CURL_TRC_CF(انکڑے, cf, "[%d] new tunnel state 'connect'", ts->دھارا_id);
    ts->state = H2_TUNNEL_CONNECT;
    اكسر;

  حالة H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(انکڑے, cf, "[%d] new tunnel state 'response'", ts->دھارا_id);
    ts->state = H2_TUNNEL_RESPONSE;
    اكسر;

  حالة H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(انکڑے, cf, "[%d] new tunnel state 'established'",
                ts->دھارا_id);
    infof(انکڑے, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    انکڑے->state.authproxy.پورا = TRUE;
    انکڑے->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  حالة H2_TUNNEL_FAILED:
    إذا(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(انکڑے, cf, "[%d] new tunnel state 'failed'", ts->دھارا_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(انکڑے->req.hd_proxy_auth);
    اكسر;
  }
}

بنية cf_h2_proxy_ctx {
  بنية curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  بنية cf_call_انکڑے call_انکڑے;

  بنية bufq inbufq;  /* network receive buffer */
  بنية bufq outbufq; /* network send buffer */

  بنية Curl_peer *peer; /* proxy we talk to */
  بنية Curl_peer *dest; /* where to tunnel to */
  بنية tunnel_دھارا tunnel; /* our tunnel CONNECT stream */
  int32_t goaway_error;
  int32_t remote_max_sid;
  ثنائی_ہندسہ(conn_closed);
  ثنائی_ہندسہ(rcvd_goaway);
  ثنائی_ہندسہ(گھلیا_goaway);
  ثنائی_ہندسہ(nw_باہرلا_blocked);
  ثنائی_ہندسہ(udp_tunnel);
};

/* How to access `call_data` from a cf_h2 filter */
#تعریف_رد CF_CTX_CALL_انکڑے
#تعریف_کرو CF_CTX_CALL_انکڑے(cf) ((بنية cf_h2_proxy_ctx *)(cf)->ctx)->call_انکڑے

ساكن فراغ cf_h2_proxy_ctx_clear(بنية cf_h2_proxy_ctx *ctx)
{
  بنية cf_call_انکڑے save = ctx->call_انکڑے;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_دھارا_clear(&ctx->tunnel);
  memset(ctx, 0, حجم(*ctx));
  ctx->call_انکڑے = save;
}

ساكن فراغ cf_h2_proxy_ctx_free(بنية cf_h2_proxy_ctx *ctx)
{
  إذا(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

ساكن فراغ drain_tunnel(بنية Curl_cfilter *cf,
                         بنية Curl_easy *انکڑے,
                         بنية tunnel_دھارا *tunnel)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  (فراغ)cf;
  إذا(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(انکڑے);
}

ساكن CURLcode proxy_h2_nw_باہرلا_writer(فراغ *writer_ctx,
                                       ثابت uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  بنية Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  إذا(cf) {
    بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
    CURLcode نتیجہ;
    نتیجہ = Curl_conn_cf_send(cf->اگلا, انکڑے, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(انکڑے, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (صحيح)نتیجہ, *pnwritten);
    أرجع نتیجہ;
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
  nghttp2_option_سیٹ_no_auto_کھڑکی_update(o, 1);
#إذا NGHTTP2_نسخہ_عدد >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_سیٹ_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#جے_انت
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

ساكن CURLcode proxy_h2_nw_باہرلا_flush(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *انکڑے)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode نتیجہ;

  إذا(Curl_bufq_is_خالی_تھاواں(&ctx->outbufq))
    أرجع CURLE_OK;

  نتیجہ = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_باہرلا_writer, cf,
                          &nwritten);
  إذا(نتیجہ) {
    إذا(نتیجہ == CURLE_AGAIN) {
      CURL_TRC_CF(انکڑے, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_باہرلا_blocked = 1;
    }
    أرجع نتیجہ;
  }
  CURL_TRC_CF(انکڑے, cf, "[0] nw send buffer flushed");
  أرجع Curl_bufq_is_خالی_تھاواں(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ساكن CURLcode proxy_h2_process_pending_داخلہ(بنية Curl_cfilter *cf,
                                               بنية Curl_easy *انکڑے)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  ثابت بنانشانی حرف *buf;
  size_t blen, nread;
  ssize_t rv;

  طالما(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ثابت uint8_t *)buf, blen);
    CURL_TRC_CF(انکڑے, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    إذا(!curlx_sztouz(rv, &nread)) {
      failf(انکڑے,
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
    إذا(Curl_bufq_is_خالی_تھاواں(&ctx->inbufq)) {
      CURL_TRC_CF(انکڑے, cf, "[0] all data in connection buffer processed");
      اكسر;
    }
    وإلا {
      CURL_TRC_CF(انکڑے, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode proxy_h2_ترقی_ingress(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *انکڑے)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->inbufq)) {
    CURL_TRC_CF(انکڑے, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    نتیجہ = proxy_h2_process_pending_داخلہ(cf, انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  طالما(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_خالی_تھاواں(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_بھریا(&ctx->tunnel.recvbuf)) {

    نتیجہ = Curl_cf_recv_bufq(cf->اگلا, انکڑے, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(انکڑے, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (صحيح)نتیجہ, nread);
    إذا(نتیجہ) {
      إذا(نتیجہ != CURLE_AGAIN) {
        failf(انکڑے, "Failed receiving HTTP2 proxy data");
        أرجع نتیجہ;
      }
      اكسر;
    }
    وإلا إذا(nread == 0) {
      CURL_TRC_CF(انکڑے, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      اكسر;
    }

    نتیجہ = proxy_h2_process_pending_داخلہ(cf, انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode proxy_h2_ترقی_egress(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *انکڑے)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  صحيح rv = 0;

  ctx->nw_باہرلا_blocked = 0;
  طالما(!rv && !ctx->nw_باہرلا_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  إذا(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(انکڑے, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    أرجع CURLE_SEND_ERROR;
  }
  أرجع proxy_h2_nw_باہرلا_flush(cf, انکڑے);
}

ساكن ssize_t on_session_send(nghttp2_session *h2,
                               ثابت uint8_t *buf, size_t blen, صحيح flags,
                               فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  size_t nwritten;
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)h2;
  (فراغ)flags;
  DEBUGASSERT(انکڑے);

  نتیجہ = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_باہرلا_writer, cf, &nwritten);
  إذا(نتیجہ) {
    إذا(نتیجہ == CURLE_AGAIN) {
      ctx->nw_باہرلا_blocked = 1;
      أرجع NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(انکڑے, "Failed sending HTTP2 data");
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  إذا(!nwritten)
    أرجع NGHTTP2_ERR_WOULDBLOCK;

  أرجع (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#جے_تعریف_شدہ CURLVERBOSE
ساكن صحيح proxy_h2_on_چوکھٹا_send(nghttp2_session *session,
                                  ثابت nghttp2_چوکھٹا *چوکھٹا,
                                  فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);

  (فراغ)session;
  DEBUGASSERT(انکڑے);
  إذا(Curl_trc_cf_is_verbose(cf, انکڑے)) {
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکھٹا, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(انکڑے, cf, "[%d] -> %s", چوکھٹا->hd.دھارا_id, بفر);
  }
  أرجع 0;
}
#جے_انت /* CURLVERBOSE */

ساكن صحيح proxy_h2_on_چوکھٹا_recv(nghttp2_session *session,
                                  ثابت nghttp2_چوکھٹا *چوکھٹا,
                                  فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  int32_t دھارا_id = چوکھٹا->hd.دھارا_id;

  (فراغ)session;
  DEBUGASSERT(انکڑے);
#جے_تعریف_شدہ CURLVERBOSE
  إذا(Curl_trc_cf_is_verbose(cf, انکڑے)) {
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکھٹا, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(انکڑے, cf, "[%d] <- %s", چوکھٹا->hd.دھارا_id, بفر);
  }
#جے_انت /* CURLVERBOSE */

  إذا(!دھارا_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(انکڑے);
    بدل(چوکھٹا->hd.type) {
    حالة NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      إذا(CURL_REQ_WANT_SEND(انکڑے)) {
        drain_tunnel(cf, انکڑے, &ctx->tunnel);
      }
      اكسر;
    حالة NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = چوکھٹا->goaway.آخری_دھارا_id;
      إذا(انکڑے) {
        infof(انکڑے, "received GOAWAY, error=%u, last_stream=%d",
              چوکھٹا->goaway.error_کوڈ, ctx->remote_max_sid);
      }
      اكسر;
    افتراضي:
      اكسر;
    }
    أرجع 0;
  }

  إذا(دھارا_id != ctx->tunnel.دھارا_id) {
    CURL_TRC_CF(انکڑے, cf, "[%d] rcvd FRAME not for tunnel", دھارا_id);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  بدل(چوکھٹا->hd.type) {
  حالة NGHTTP2_سرخیاں:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    إذا(!ctx->tunnel.resp)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(انکڑے, cf, "[%d] got http status: %d",
                دھارا_id, ctx->tunnel.resp->حالت);
    إذا(!ctx->tunnel.has_final_response && ctx->tunnel.resp->حالت / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    اكسر;
  حالة NGHTTP2_کھڑکی_UPDATE:
    إذا(CURL_REQ_WANT_SEND(انکڑے)) {
      drain_tunnel(cf, انکڑے, &ctx->tunnel);
    }
    اكسر;
  حالة NGHTTP2_RST_دھارا:
    إذا(چوکھٹا->rst_دھارا.error_کوڈ)
      ctx->tunnel.reset = TRUE;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع 0;
}

ساكن صحيح proxy_h2_on_سرخی(nghttp2_session *session,
                              ثابت nghttp2_چوکھٹا *چوکھٹا,
                              ثابت uint8_t *name, size_t namelen,
                              ثابت uint8_t *قدر, size_t valuelen,
                              uint8_t flags,
                              فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  int32_t دھارا_id = چوکھٹا->hd.دھارا_id;
  CURLcode نتیجہ;

  (فراغ)flags;
  (فراغ)session;
  DEBUGASSERT(دھارا_id); /* should never be a zero stream ID here */
  إذا(دھارا_id != ctx->tunnel.دھارا_id) {
    CURL_TRC_CF(انکڑے, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", دھارا_id,
                (صحيح)namelen, name, (صحيح)valuelen, قدر);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  إذا(چوکھٹا->hd.type == NGHTTP2_ڈالو_PROMISE)
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
    نتیجہ = Curl_http_decode_حالت(&http_حالت,
                                    (ثابت حرف *)قدر, valuelen);
    إذا(نتیجہ)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    نتیجہ = Curl_http_resp_make(&resp, http_حالت, عدم);
    إذا(نتیجہ)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    إذا(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(انکڑے, cf, "[%d] status: HTTP/2 %03d",
                دھارا_id, ctx->tunnel.resp->حالت);
    أرجع 0;
  }

  إذا(!ctx->tunnel.resp)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  نتیجہ = Curl_dynhds_جوڑو(&ctx->tunnel.resp->سرخیاں,
                           (ثابت حرف *)name, namelen,
                           (ثابت حرف *)قدر, valuelen);
  إذا(نتیجہ)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(انکڑے, cf, "[%d] header: %.*s: %.*s",
              دھارا_id, (صحيح)namelen, name, (صحيح)valuelen, قدر);

  أرجع 0; /* 0 is successful */
}

ساكن ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t دھارا_id,
                                    uint8_t *buf, size_t لمائی,
                                    uint32_t *انکڑے_flags,
                                    nghttp2_انکڑے_سروت *سروت,
                                    فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  بنية tunnel_دھارا *ts;
  CURLcode نتیجہ;
  size_t nread;

  (فراغ)session;
  (فراغ)سروت;
  (فراغ)ctx;

  إذا(!دھارا_id)
    أرجع NGHTTP2_ERR_INVALID_دلیل;

  ts = Curl_ngh2_session_get_دھارا_user_انکڑے(&ctx->ngh2, دھارا_id);
  إذا(!ts)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  نتیجہ = Curl_bufq_read(&ts->sendbuf, buf, لمائی, &nread);
  إذا(نتیجہ) {
    إذا(نتیجہ != CURLE_AGAIN)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    أرجع NGHTTP2_ERR_DEFERRED;
  }
  إذا(ts->closed && Curl_bufq_is_خالی_تھاواں(&ts->sendbuf))
    *انکڑے_flags = NGHTTP2_انکڑے_FLAG_EOF;

  CURL_TRC_CF(انکڑے, cf, "[%d] tunnel_send_callback -> %zu",
              ts->دھارا_id, nread);
  أرجع (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

ساكن صحيح tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t دھارا_id,
                                ثابت uint8_t *mem, size_t len, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode نتیجہ;

  (فراغ)flags;
  (فراغ)session;
  DEBUGASSERT(دھارا_id); /* should never be a zero stream ID here */

  إذا(دھارا_id != ctx->tunnel.دھارا_id)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  نتیجہ = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  إذا(نتیجہ) {
    إذا(نتیجہ != CURLE_AGAIN)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
#جے_تعریف_شدہ DEBUGBUILD
    nwritten = 0;
#جے_انت
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  أرجع 0;
}

ساكن صحيح proxy_h2_on_دھارا_close(nghttp2_session *session,
                                    int32_t دھارا_id,
                                    uint32_t error_کوڈ, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);

  (فراغ)session;

  إذا(دھارا_id != ctx->tunnel.دھارا_id)
    أرجع 0;

  CURL_TRC_CF(انکڑے, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              دھارا_id, nghttp2_http2_strerror(error_کوڈ), error_کوڈ);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_کوڈ;
  إذا(error_کوڈ)
    ctx->tunnel.reset = TRUE;

  أرجع 0;
}

ساكن CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  بنية Curl_cfilter *cf,
  بنية Curl_easy *انکڑے,
  بنية curl_ngh2 *ngh2,
  بنية httpreq *req,
  ثابت nghttp2_ترجیح_spec *pri_spec,
  فراغ *دھارا_user_انکڑے,
  nghttp2_انکڑے_سروت_read_callback read_callback,
  فراغ *read_ctx)
{
  بنية dynhds h2_سرخیاں;
  nghttp2_nv *nva = عدم;
  int32_t دھارا_id = -1;
  size_t nheader;
  CURLcode نتیجہ;

  (فراغ)cf;
  Curl_dynhds_init(&h2_سرخیاں, MAX_HTTP_RESP_سرخی_گنتی, DYN_HTTP_REQUEST);
  نتیجہ = Curl_http_req_منزل_h2(&h2_سرخیاں, req, انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

  nva = Curl_dynhds_منزل_nva(&h2_سرخیاں, &nheader);
  إذا(!nva) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  إذا(read_callback) {
    nghttp2_انکڑے_provider انکڑے_prd;

    انکڑے_prd.read_callback = read_callback;
    انکڑے_prd.سروت.ptr = read_ctx;
    دھارا_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &انکڑے_prd, دھارا_user_انکڑے);
  }
  وإلا {
    دھارا_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, عدم, دھارا_user_انکڑے);
  }

  إذا(دھارا_id < 0) {
    failf(انکڑے, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(دھارا_id), دھارا_id);
    نتیجہ = CURLE_SEND_ERROR;
    اذهب باہرلا;
  }
  نتیجہ = CURLE_OK;

باہرلا:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرخیاں);
  *pstream_id = دھارا_id;
  أرجع نتیجہ;
}

ساكن CURLcode submit_CONNECT(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے,
                               بنية tunnel_دھارا *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;
  بنية httpreq *req = عدم;

  نتیجہ = Curl_http_proxy_create_tunnel_request(&req, cf, انکڑے,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  إذا(نتیجہ)
    اذهب باہرلا;
  نتیجہ = Curl_creader_سیٹ_null(انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = proxy_h2_submit(&ts->دھارا_id, cf, انکڑے, &ctx->ngh2, req,
                           عدم, ts, tunnel_send_callback, cf);
  إذا(نتیجہ) {
    CURL_TRC_CF(انکڑے, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->دھارا_id, nghttp2_strerror(ts->دھارا_id));
  }

باہرلا:
  إذا(req)
    Curl_http_req_free(req);
  إذا(نتیجہ)
    failf(انکڑے, "Failed sending CONNECT to proxy");
  أرجع نتیجہ;
}

ساكن CURLcode inspect_response(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 بنية tunnel_دھارا *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_نتیجہ res;
  CURLcode نتیجہ;

  نتیجہ = Curl_http_proxy_inspect_tunnel_response(
      cf, انکڑے, ts->resp, (bool)ctx->udp_tunnel, &res);
  إذا(نتیجہ)
    أرجع نتیجہ;
  بدل(res) {
  حالة PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, انکڑے,
                       (bool)ctx->udp_tunnel);
    اكسر;
  حالة PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, انکڑے,
                       (bool)ctx->udp_tunnel);
    نتیجہ = CURLE_COULDNT_CONNECT;
    اكسر;
  حالة PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, انکڑے,
                       (bool)ctx->udp_tunnel);
    اكسر;
  }
  أرجع نتیجہ;
}

ساكن CURLcode H2_CONNECT(بنية Curl_cfilter *cf,
                           بنية Curl_easy *انکڑے,
                           بنية tunnel_دھارا *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  إذا(ctx->conn_closed) {
    failf(انکڑے, "proxy closed connection");
    أرجع CURLE_COULDNT_CONNECT;
  }

  افعل {
    بدل(ts->state) {
    حالة H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(انکڑے, cf, "[0] CONNECT start for %s", ts->authority);
      نتیجہ = submit_CONNECT(cf, انکڑے, ts);
      إذا(نتیجہ)
        اذهب باہرلا;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, انکڑے,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    حالة H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      نتیجہ = proxy_h2_ترقی_ingress(cf, انکڑے);
      إذا(!نتیجہ)
        نتیجہ = proxy_h2_ترقی_egress(cf, انکڑے);
      إذا(نتیجہ && نتیجہ != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, انکڑے,
                           (bool)ctx->udp_tunnel);
        اكسر;
      }

      إذا(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, انکڑے,
                           (bool)ctx->udp_tunnel);
      }
      وإلا {
        نتیجہ = CURLE_OK;
        اذهب باہرلا;
      }
      FALLTHROUGH();

    حالة H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      نتیجہ = inspect_response(cf, انکڑے, ts);
      إذا(نتیجہ)
        اذهب باہرلا;
      اكسر;

    حالة H2_TUNNEL_ESTABLISHED:
      أرجع CURLE_OK;

    حالة H2_TUNNEL_FAILED:
      أرجع CURLE_RECV_ERROR;

    افتراضي:
      اكسر;
    }

  } طالما(ts->state == H2_TUNNEL_INIT);

باہرلا:
  إذا((نتیجہ && (نتیجہ != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, انکڑے,
                       (bool)ctx->udp_tunnel);
  أرجع نتیجہ;
}

/*
 * Initialize the cfilter context
 */
ساكن CURLcode cf_h2_proxy_ctx_init(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *انکڑے)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  nghttp2_session_callbacks *cbs = عدم;
  صحيح rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, حجم(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_ناپ, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_ناپ, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  إذا(tunnel_دھارا_init(&ctx->tunnel, ctx->dest))
    اذهب باہرلا;

  rc = nghttp2_session_callbacks_new(&cbs);
  إذا(rc) {
    failf(انکڑے, "Could not initialize nghttp2 callbacks");
    اذهب باہرلا;
  }

  nghttp2_session_callbacks_سیٹ_send_callback(cbs, on_session_send);
  nghttp2_session_callbacks_سیٹ_on_چوکھٹا_recv_callback(
    cbs, proxy_h2_on_چوکھٹا_recv);
#جے_تعریف_شدہ CURLVERBOSE
  nghttp2_session_callbacks_سیٹ_on_چوکھٹا_send_callback(cbs,
                                                       proxy_h2_on_چوکھٹا_send);
#جے_انت
  nghttp2_session_callbacks_سیٹ_on_انکڑے_chunk_recv_callback(
    cbs, tunnel_recv_callback);
  nghttp2_session_callbacks_سیٹ_on_دھارا_close_callback(
    cbs, proxy_h2_on_دھارا_close);
  nghttp2_session_callbacks_سیٹ_on_سرخی_callback(cbs, proxy_h2_on_سرخی);

  /* The nghttp2 session is not yet setup, do it */
  rc = proxy_h2_client_new(cf, cbs);
  إذا(rc) {
    failf(انکڑے, "Could not initialize nghttp2");
    اذهب باہرلا;
  }

  {
    nghttp2_settings_اندراج iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].قدر = Curl_multi_max_concurrent_streams(انکڑے->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_کھڑکی_ناپ;
    iv[1].قدر = H2_TUNNEL_کھڑکی_ناپ;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ڈالو;
    iv[2].قدر = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    إذا(rc) {
      failf(انکڑے, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      نتیجہ = CURLE_HTTP2;
      اذهب باہرلا;
    }
  }

  rc = Curl_ngh2_session_سیٹ_local_کھڑکی_ناپ(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_کھڑکی_ناپ);
  إذا(rc) {
    failf(انکڑے, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    نتیجہ = CURLE_HTTP2;
    اذهب باہرلا;
  }

  /* all set, traffic will be send on connect */
  نتیجہ = CURLE_OK;

باہرلا:
  إذا(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(انکڑے, cf, "[0] init proxy ctx -> %d", (صحيح)نتیجہ);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h2_proxy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے,
                                    bool *پورا)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  بنية cf_call_انکڑے save;
  بنية tunnel_دھارا *ts = &ctx->tunnel;

  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the lower filters first */
  إذا(!cf->اگلا->connected) {
    نتیجہ = Curl_conn_cf_connect(cf->اگلا, انکڑے, پورا);
    إذا(نتیجہ || !*پورا)
      أرجع نتیجہ;
  }

  *پورا = FALSE;

  CF_انکڑے_SAVE(save, cf, انکڑے);
  إذا(!ctx->ngh2.s) {
    نتیجہ = cf_h2_proxy_ctx_init(cf, انکڑے);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  DEBUGASSERT(ts->authority);

  إذا(Curl_timeleft_ms(انکڑے) < 0) {
    failf(انکڑے, "Proxy CONNECT aborted due to timeout");
    نتیجہ = CURLE_OPERATION_TIMEDOUT;
    اذهب باہرلا;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  نتیجہ = H2_CONNECT(cf, انکڑے, ts);

باہرلا:
  *پورا = (نتیجہ == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  إذا(*پورا) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&انکڑے->req, انکڑے);
    Curl_client_reset(انکڑے);
  }
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن فراغ cf_h2_proxy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  (فراغ)انکڑے;
  إذا(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_h2_proxy_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_انکڑے save;
  CURLcode نتیجہ;
  صحيح rv;

  إذا(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  CF_انکڑے_SAVE(save, cf, انکڑے);

  إذا(!ctx->گھلیا_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ثابت uint8_t *)"shutdown", حجم("shutdown"));
    إذا(rv) {
      failf(انکڑے, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      نتیجہ = CURLE_SEND_ERROR;
      اذهب باہرلا;
    }
    ctx->گھلیا_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  نتیجہ = CURLE_OK;
  إذا(Curl_ngh2_session_want_write(&ctx->ngh2))
    نتیجہ = proxy_h2_ترقی_egress(cf, انکڑے);
  إذا(!نتیجہ && Curl_ngh2_session_want_read(&ctx->ngh2))
    نتیجہ = proxy_h2_ترقی_ingress(cf, انکڑے);

  *پورا = (ctx->conn_closed ||
           (!نتیجہ && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
باہرلا:
  CF_انکڑے_RESTORE(cf, save);
  cf->shutdown = (نتیجہ || *پورا);
  أرجع نتیجہ;
}

ساكن bool cf_h2_proxy_انکڑے_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *انکڑے)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  إذا((ctx && !Curl_bufq_is_خالی_تھاواں(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.recvbuf)))
    أرجع TRUE;
  أرجع cf->اگلا ? cf->اگلا->cft->has_انکڑے_pending(cf->اگلا, انکڑے) : FALSE;
}

ساكن CURLcode cf_h2_proxy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *انکڑے,
                                           بنية easy_pollset *ps)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_انکڑے save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, انکڑے);
  bool want_recv, want_send;
  CURLcode نتیجہ = CURLE_OK;

  إذا(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_خالی_تھاواں(&ctx->outbufq) ||
                !Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  وإلا
    Curl_pollset_check(انکڑے, ps, sock, &want_recv, &want_send);

  إذا(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
    bool c_exhaust, s_exhaust;

    CF_انکڑے_SAVE(save, cf, انکڑے);
    c_exhaust = !Curl_ngh2_session_get_remote_کھڑکی_ناپ(&ctx->ngh2);
    s_exhaust = ctx->tunnel.دھارا_id >= 0 &&
                !Curl_ngh2_session_get_دھارا_remote_کھڑکی_ناپ(
                  &ctx->ngh2, ctx->tunnel.دھارا_id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_خالی_تھاواں(&ctx->outbufq) ||
                !Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.sendbuf);

    نتیجہ = Curl_pollset_سیٹ(انکڑے, ps, sock, want_recv, want_send);
    CURL_TRC_CF(انکڑے, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (صحيح)نتیجہ);
    CF_انکڑے_RESTORE(cf, save);
  }
  وإلا إذا(ctx->گھلیا_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_انکڑے_SAVE(save, cf, انکڑے);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_خالی_تھاواں(&ctx->outbufq) ||
                !Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    نتیجہ = Curl_pollset_سیٹ(انکڑے, ps, sock, want_recv, want_send);
    CURL_TRC_CF(انکڑے, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (صحيح)نتیجہ);
    CF_انکڑے_RESTORE(cf, save);
  }
  أرجع نتیجہ;
}

ساكن CURLcode h2_handle_tunnel_close(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *انکڑے,
                                       size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  إذا(ctx->tunnel.error) {
    failf(انکڑے, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.دھارا_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    أرجع CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(انکڑے, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.دھارا_id);
  أرجع CURLE_OK;
}

ساكن CURLcode tunnel_recv(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                            حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_AGAIN;

  *pnread = 0;
  إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.recvbuf))
    نتیجہ = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  وإلا {
    إذا(ctx->tunnel.closed) {
      نتیجہ = h2_handle_tunnel_close(cf, انکڑے, pnread);
    }
    وإلا إذا(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_خالی_تھاواں(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.دھارا_id)) {
      نتیجہ = CURLE_RECV_ERROR;
    }
    وإلا
      نتیجہ = CURLE_AGAIN;
  }

  CURL_TRC_CF(انکڑے, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.دھارا_id, len, (صحيح)نتیجہ, *pnread);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h2_proxy_recv(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 حرف *buf, size_t len,
                                 size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_انکڑے save;
  CURLcode نتیجہ;

  *pnread = 0;
  CF_انکڑے_SAVE(save, cf, انکڑے);

  إذا(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    نتیجہ = CURLE_RECV_ERROR;
    اذهب باہرلا;
  }

  إذا(Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.recvbuf)) {
    نتیجہ = proxy_h2_ترقی_ingress(cf, انکڑے);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  نتیجہ = tunnel_recv(cf, انکڑے, buf, len, pnread);

  إذا(!نتیجہ) {
    CURL_TRC_CF(انکڑے, cf, "[%d] increase window by %zu",
                ctx->tunnel.دھارا_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.دھارا_id, *pnread);
  }

  نتیجہ = Curl_1st_fatal(نتیجہ, proxy_h2_ترقی_egress(cf, انکڑے));

باہرلا:
  إذا((!Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.sendbuf)) &&
     (!نتیجہ || (نتیجہ == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, انکڑے, &ctx->tunnel);
  }
  CURL_TRC_CF(انکڑے, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.دھارا_id, len, (صحيح)نتیجہ, *pnread);
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h2_proxy_send(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 ثابت uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_انکڑے save;
  CURLcode نتیجہ;

  (فراغ)eos;
  *pnwritten = 0;
  CF_انکڑے_SAVE(save, cf, انکڑے);

  إذا(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    نتیجہ = CURLE_SEND_ERROR;
    اذهب باہرلا;
  }

  إذا(ctx->tunnel.closed) {
    نتیجہ = CURLE_SEND_ERROR;
    اذهب باہرلا;
  }

  نتیجہ = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(انکڑے, cf, "cf_send(), bufq_write %d, %zu", (صحيح)نتیجہ,
              *pnwritten);
  إذا(نتیجہ && (نتیجہ != CURLE_AGAIN))
    اذهب باہرلا;

  إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_انکڑے(&ctx->ngh2, ctx->tunnel.دھارا_id))) {
      نتیجہ = CURLE_SEND_ERROR;
      اذهب باہرلا;
    }
  }

  نتیجہ = Curl_1st_fatal(نتیجہ, proxy_h2_ترقی_ingress(cf, انکڑے));
  نتیجہ = Curl_1st_fatal(نتیجہ, proxy_h2_ترقی_egress(cf, انکڑے));

  إذا(!نتیجہ && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    إذا(ctx->tunnel.closed) {
      نتیجہ = CURLE_SEND_ERROR;
    }
    وإلا {
      CURL_TRC_CF(انکڑے, cf, "[0] send: nothing to do in this session");
      نتیجہ = CURLE_HTTP2;
    }
  }

باہرلا:
  إذا((!Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.sendbuf)) &&
     (!نتیجہ || (نتیجہ == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, انکڑے, &ctx->tunnel);
  }
  CURL_TRC_CF(انکڑے, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.دھارا_id, len, (صحيح)نتیجہ, *pnwritten,
              Curl_ngh2_session_get_دھارا_remote_کھڑکی_ناپ(
                &ctx->ngh2, ctx->tunnel.دھارا_id),
              Curl_ngh2_session_get_remote_کھڑکی_ناپ(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h2_proxy_flush(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_انکڑے save;
  CURLcode نتیجہ = CURLE_OK;

  CF_انکڑے_SAVE(save, cf, انکڑے);
  إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_انکڑے(&ctx->ngh2, ctx->tunnel.دھارا_id))) {
      نتیجہ = CURLE_SEND_ERROR;
      اذهب باہرلا;
    }
  }

  نتیجہ = proxy_h2_ترقی_egress(cf, انکڑے);

باہرلا:
  CURL_TRC_CF(انکڑے, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.دھارا_id, (صحيح)نتیجہ,
              Curl_ngh2_session_get_دھارا_remote_کھڑکی_ناپ(
                &ctx->ngh2, ctx->tunnel.دھارا_id),
              Curl_ngh2_session_get_remote_کھڑکی_ناپ(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن bool proxy_h2_connisalive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 bool *داخلہ_pending)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *داخلہ_pending = FALSE;
  إذا(!cf->اگلا || !cf->اگلا->cft->is_alive(cf->اگلا, انکڑے, داخلہ_pending))
    أرجع FALSE;

  إذا(*داخلہ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode نتیجہ;
    size_t nread;

    *داخلہ_pending = FALSE;
    نتیجہ = Curl_cf_recv_bufq(cf->اگلا, انکڑے, &ctx->inbufq, 0, &nread);
    إذا(!نتیجہ) {
      إذا(proxy_h2_process_pending_داخلہ(cf, انکڑے))
        /* immediate error, considered dead */
        alive = FALSE;
      وإلا {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    وإلا إذا(نتیجہ != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  أرجع alive;
}

ساكن bool cf_h2_proxy_is_alive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 bool *داخلہ_pending)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  بنية cf_call_انکڑے save;

  *داخلہ_pending = FALSE;
  CF_انکڑے_SAVE(save, cf, انکڑے);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, انکڑے, داخلہ_pending));
  CURL_TRC_CF(انکڑے, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *داخلہ_pending);
  CF_انکڑے_RESTORE(cf, save);
  أرجع alive;
}

ساكن CURLcode cf_h2_proxy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_HOST_پورٹ:
    *pres1 = (صحيح)ctx->dest->پورٹ;
    *((ثابت حرف **)pres2) = ctx->dest->hostname;
    أرجع CURLE_OK;
  حالة CF_QUERY_NEED_FLUSH: {
    إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->outbufq) ||
       !Curl_bufq_is_خالی_تھاواں(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(انکڑے, cf, "needs flush");
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
  أرجع cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, انکڑے, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_h2_proxy_cntrl(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية cf_call_انکڑے save;

  (فراغ)arg1;
  (فراغ)arg2;

  بدل(event) {
  حالة CF_CTRL_FLUSH:
    CF_انکڑے_SAVE(save, cf, انکڑے);
    نتیجہ = cf_h2_proxy_flush(cf, انکڑے);
    CF_انکڑے_RESTORE(cf, save);
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتیجہ;
}

بنية Curl_cftype Curl_cft_h2_proxy = {
  "H2-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  cf_h2_proxy_destroy,
  cf_h2_proxy_connect,
  cf_h2_proxy_shutdown,
  cf_h2_proxy_adjust_pollset,
  cf_h2_proxy_انکڑے_pending,
  cf_h2_proxy_send,
  cf_h2_proxy_recv,
  cf_h2_proxy_cntrl,
  cf_h2_proxy_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h2_proxy_query,
};

CURLcode Curl_cf_h2_proxy_داخل_کرو_after(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *انکڑے,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *dest,
                                       bool udp_tunnel)
{
  بنية Curl_cfilter *cf_h2_proxy = عدم;
  بنية cf_h2_proxy_ctx *ctx;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;

  (فراغ)انکڑے;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب باہرلا;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  نتیجہ = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  إذا(نتیجہ)
    اذهب باہرلا;
  ctx = عدم;
  Curl_conn_cf_داخل_کرو_after(cf, cf_h2_proxy);

باہرلا:
  cf_h2_proxy_ctx_free(ctx);
  أرجع نتیجہ;
}

#جے_انت /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
