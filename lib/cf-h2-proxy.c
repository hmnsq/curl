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

#وصف_ڪريو PROXY_H2_CHUNK_سائيز  (16 * 1024)

#وصف_ڪريو PROXY_HTTP2_HUGE_دري_سائيز (100 * 1024 * 1024)
#وصف_ڪريو H2_TUNNEL_دري_سائيز        (10 * 1024 * 1024)

#وصف_ڪريو PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_دري_سائيز / PROXY_H2_CHUNK_سائيز)
#وصف_ڪريو PROXY_H2_NW_SEND_CHUNKS   1

#وصف_ڪريو H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_دري_سائيز / PROXY_H2_CHUNK_سائيز)
#وصف_ڪريو H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_سائيز)


تعريفنوع تعداد {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

بنية tunnel_وهڪرو {
  بنية http_resp *resp;
  بنية bufq recvbuf;
  بنية bufq sendbuf;
  حرف *authority;
  int32_t وهڪرو_id;
  uint32_t error;
  h2_tunnel_state state;
  ٻٽو_انگ(has_final_response);
  ٻٽو_انگ(closed);
  ٻٽو_انگ(reset);
};

ساكن CURLcode tunnel_وهڪرو_init(بنية tunnel_وهڪرو *ts,
                                   بنية Curl_peer *dest)
{
  ts->state = H2_TUNNEL_INIT;
  ts->وهڪرو_id = -1;
  Curl_bufq_init2(&ts->recvbuf, PROXY_H2_CHUNK_سائيز, H2_TUNNEL_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندي);
  Curl_bufq_init(&ts->sendbuf, PROXY_H2_CHUNK_سائيز, H2_TUNNEL_SEND_CHUNKS);

  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->پورٽ);
  إذا(!ts->authority)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  أرجع CURLE_OK;
}

ساكن فراغ tunnel_وهڪرو_reset(بنية tunnel_وهڪرو *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = عدم;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->وهڪرو_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

ساكن فراغ tunnel_وهڪرو_clear(بنية tunnel_وهڪرو *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, حجم(*ts));
  ts->state = H2_TUNNEL_INIT;
}

ساكن فراغ h2_tunnel_go_state(بنية Curl_cfilter *cf,
                               بنية tunnel_وهڪرو *ts,
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
    CURL_TRC_CF(معلومات, cf, "[%d] new tunnel state 'init'", ts->وهڪرو_id);
    tunnel_وهڪرو_reset(ts);
    اكسر;

  حالة H2_TUNNEL_CONNECT:
    CURL_TRC_CF(معلومات, cf, "[%d] new tunnel state 'connect'", ts->وهڪرو_id);
    ts->state = H2_TUNNEL_CONNECT;
    اكسر;

  حالة H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(معلومات, cf, "[%d] new tunnel state 'response'", ts->وهڪرو_id);
    ts->state = H2_TUNNEL_RESPONSE;
    اكسر;

  حالة H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(معلومات, cf, "[%d] new tunnel state 'established'",
                ts->وهڪرو_id);
    infof(معلومات, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    معلومات->state.authproxy.پورو = TRUE;
    معلومات->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  حالة H2_TUNNEL_FAILED:
    إذا(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(معلومات, cf, "[%d] new tunnel state 'failed'", ts->وهڪرو_id);
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
  بنية tunnel_وهڪرو tunnel; /* our tunnel CONNECT stream */
  int32_t goaway_error;
  int32_t remote_max_sid;
  ٻٽو_انگ(conn_closed);
  ٻٽو_انگ(rcvd_goaway);
  ٻٽو_انگ(موڪليل_goaway);
  ٻٽو_انگ(nw_ٻاهريون_blocked);
  ٻٽو_انگ(udp_tunnel);
};

/* How to access `call_data` from a cf_h2 filter */
#وصف_رد CF_CTX_CALL_معلومات
#وصف_ڪريو CF_CTX_CALL_معلومات(cf) ((بنية cf_h2_proxy_ctx *)(cf)->ctx)->call_معلومات

ساكن فراغ cf_h2_proxy_ctx_clear(بنية cf_h2_proxy_ctx *ctx)
{
  بنية cf_call_معلومات save = ctx->call_معلومات;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_وهڪرو_clear(&ctx->tunnel);
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
                         بنية tunnel_وهڪرو *tunnel)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  (فراغ)cf;
  إذا(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_خالي_جايون(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_خالي_جايون(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(معلومات);
}

ساكن CURLcode proxy_h2_nw_ٻاهريون_writer(فراغ *writer_ctx,
                                       ثابت uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  بنية Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  إذا(cf) {
    بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
    CURLcode نتيجو;
    نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(معلومات, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (صحيح)نتيجو, *pnwritten);
    أرجع نتيجو;
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
  nghttp2_option_سيٽ_no_auto_دري_update(o, 1);
#إذا NGHTTP2_نسخو_انگ >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_سيٽ_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#جيڪڏهن_پورو
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

ساكن CURLcode proxy_h2_nw_ٻاهريون_flush(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode نتيجو;

  إذا(Curl_bufq_is_خالي_جايون(&ctx->outbufq))
    أرجع CURLE_OK;

  نتيجو = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_ٻاهريون_writer, cf,
                          &nwritten);
  إذا(نتيجو) {
    إذا(نتيجو == CURLE_AGAIN) {
      CURL_TRC_CF(معلومات, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_ٻاهريون_blocked = 1;
    }
    أرجع نتيجو;
  }
  CURL_TRC_CF(معلومات, cf, "[0] nw send buffer flushed");
  أرجع Curl_bufq_is_خالي_جايون(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ساكن CURLcode proxy_h2_process_pending_داخلا(بنية Curl_cfilter *cf,
                                               بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  ثابت بنانشان حرف *buf;
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
    إذا(Curl_bufq_is_خالي_جايون(&ctx->inbufq)) {
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

ساكن CURLcode proxy_h2_ترقي_ingress(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  إذا(!Curl_bufq_is_خالي_جايون(&ctx->inbufq)) {
    CURL_TRC_CF(معلومات, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    نتيجو = proxy_h2_process_pending_داخلا(cf, معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  طالما(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_خالي_جايون(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_ڀريل(&ctx->tunnel.recvbuf)) {

    نتيجو = Curl_cf_recv_bufq(cf->ايندڙ, معلومات, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(معلومات, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (صحيح)نتيجو, nread);
    إذا(نتيجو) {
      إذا(نتيجو != CURLE_AGAIN) {
        failf(معلومات, "Failed receiving HTTP2 proxy data");
        أرجع نتيجو;
      }
      اكسر;
    }
    وإلا إذا(nread == 0) {
      CURL_TRC_CF(معلومات, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      اكسر;
    }

    نتيجو = proxy_h2_process_pending_داخلا(cf, معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode proxy_h2_ترقي_egress(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  صحيح rv = 0;

  ctx->nw_ٻاهريون_blocked = 0;
  طالما(!rv && !ctx->nw_ٻاهريون_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  إذا(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(معلومات, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    أرجع CURLE_SEND_ERROR;
  }
  أرجع proxy_h2_nw_ٻاهريون_flush(cf, معلومات);
}

ساكن ssize_t on_session_send(nghttp2_session *h2,
                               ثابت uint8_t *buf, size_t blen, صحيح flags,
                               فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  size_t nwritten;
  CURLcode نتيجو = CURLE_OK;

  (فراغ)h2;
  (فراغ)flags;
  DEBUGASSERT(معلومات);

  نتيجو = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_ٻاهريون_writer, cf, &nwritten);
  إذا(نتيجو) {
    إذا(نتيجو == CURLE_AGAIN) {
      ctx->nw_ٻاهريون_blocked = 1;
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

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
ساكن صحيح proxy_h2_on_چوکٺو_send(nghttp2_session *session,
                                  ثابت nghttp2_چوکٺو *چوکٺو,
                                  فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);

  (فراغ)session;
  DEBUGASSERT(معلومات);
  إذا(Curl_trc_cf_is_verbose(cf, معلومات)) {
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکٺو, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(معلومات, cf, "[%d] -> %s", چوکٺو->hd.وهڪرو_id, بفر);
  }
  أرجع 0;
}
#جيڪڏهن_پورو /* CURLVERBOSE */

ساكن صحيح proxy_h2_on_چوکٺو_recv(nghttp2_session *session,
                                  ثابت nghttp2_چوکٺو *چوکٺو,
                                  فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  int32_t وهڪرو_id = چوکٺو->hd.وهڪرو_id;

  (فراغ)session;
  DEBUGASSERT(معلومات);
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  إذا(Curl_trc_cf_is_verbose(cf, معلومات)) {
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکٺو, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(معلومات, cf, "[%d] <- %s", چوکٺو->hd.وهڪرو_id, بفر);
  }
#جيڪڏهن_پورو /* CURLVERBOSE */

  إذا(!وهڪرو_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(معلومات);
    بدل(چوکٺو->hd.type) {
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
      ctx->remote_max_sid = چوکٺو->goaway.آخري_وهڪرو_id;
      إذا(معلومات) {
        infof(معلومات, "received GOAWAY, error=%u, last_stream=%d",
              چوکٺو->goaway.error_ڪوڊ, ctx->remote_max_sid);
      }
      اكسر;
    افتراضي:
      اكسر;
    }
    أرجع 0;
  }

  إذا(وهڪرو_id != ctx->tunnel.وهڪرو_id) {
    CURL_TRC_CF(معلومات, cf, "[%d] rcvd FRAME not for tunnel", وهڪرو_id);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  بدل(چوکٺو->hd.type) {
  حالة NGHTTP2_سرا:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    إذا(!ctx->tunnel.resp)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(معلومات, cf, "[%d] got http status: %d",
                وهڪرو_id, ctx->tunnel.resp->حالت);
    إذا(!ctx->tunnel.has_final_response && ctx->tunnel.resp->حالت / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    اكسر;
  حالة NGHTTP2_دري_UPDATE:
    إذا(CURL_REQ_WANT_SEND(معلومات)) {
      drain_tunnel(cf, معلومات, &ctx->tunnel);
    }
    اكسر;
  حالة NGHTTP2_RST_وهڪرو:
    إذا(چوکٺو->rst_وهڪرو.error_ڪوڊ)
      ctx->tunnel.reset = TRUE;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع 0;
}

ساكن صحيح proxy_h2_on_سرو(nghttp2_session *session,
                              ثابت nghttp2_چوکٺو *چوکٺو,
                              ثابت uint8_t *name, size_t namelen,
                              ثابت uint8_t *قدر, size_t valuelen,
                              uint8_t flags,
                              فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  int32_t وهڪرو_id = چوکٺو->hd.وهڪرو_id;
  CURLcode نتيجو;

  (فراغ)flags;
  (فراغ)session;
  DEBUGASSERT(وهڪرو_id); /* should never be a zero stream ID here */
  إذا(وهڪرو_id != ctx->tunnel.وهڪرو_id) {
    CURL_TRC_CF(معلومات, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", وهڪرو_id,
                (صحيح)namelen, name, (صحيح)valuelen, قدر);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  إذا(چوکٺو->hd.type == NGHTTP2_ڌڪو_PROMISE)
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
    نتيجو = Curl_http_decode_حالت(&http_حالت,
                                    (ثابت حرف *)قدر, valuelen);
    إذا(نتيجو)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    نتيجو = Curl_http_resp_make(&resp, http_حالت, عدم);
    إذا(نتيجو)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    إذا(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(معلومات, cf, "[%d] status: HTTP/2 %03d",
                وهڪرو_id, ctx->tunnel.resp->حالت);
    أرجع 0;
  }

  إذا(!ctx->tunnel.resp)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  نتيجو = Curl_dynhds_جوڙيو(&ctx->tunnel.resp->سرا,
                           (ثابت حرف *)name, namelen,
                           (ثابت حرف *)قدر, valuelen);
  إذا(نتيجو)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(معلومات, cf, "[%d] header: %.*s: %.*s",
              وهڪرو_id, (صحيح)namelen, name, (صحيح)valuelen, قدر);

  أرجع 0; /* 0 is successful */
}

ساكن ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t وهڪرو_id,
                                    uint8_t *buf, size_t ڊيگهائي,
                                    uint32_t *معلومات_flags,
                                    nghttp2_معلومات_ذريعو *ذريعو,
                                    فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  بنية tunnel_وهڪرو *ts;
  CURLcode نتيجو;
  size_t nread;

  (فراغ)session;
  (فراغ)ذريعو;
  (فراغ)ctx;

  إذا(!وهڪرو_id)
    أرجع NGHTTP2_ERR_INVALID_دليل;

  ts = Curl_ngh2_session_get_وهڪرو_user_معلومات(&ctx->ngh2, وهڪرو_id);
  إذا(!ts)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  نتيجو = Curl_bufq_read(&ts->sendbuf, buf, ڊيگهائي, &nread);
  إذا(نتيجو) {
    إذا(نتيجو != CURLE_AGAIN)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    أرجع NGHTTP2_ERR_DEFERRED;
  }
  إذا(ts->closed && Curl_bufq_is_خالي_جايون(&ts->sendbuf))
    *معلومات_flags = NGHTTP2_معلومات_FLAG_EOF;

  CURL_TRC_CF(معلومات, cf, "[%d] tunnel_send_callback -> %zu",
              ts->وهڪرو_id, nread);
  أرجع (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

ساكن صحيح tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t وهڪرو_id,
                                ثابت uint8_t *mem, size_t len, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode نتيجو;

  (فراغ)flags;
  (فراغ)session;
  DEBUGASSERT(وهڪرو_id); /* should never be a zero stream ID here */

  إذا(وهڪرو_id != ctx->tunnel.وهڪرو_id)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  نتيجو = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  إذا(نتيجو) {
    إذا(نتيجو != CURLE_AGAIN)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
    nwritten = 0;
#جيڪڏهن_پورو
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  أرجع 0;
}

ساكن صحيح proxy_h2_on_وهڪرو_close(nghttp2_session *session,
                                    int32_t وهڪرو_id,
                                    uint32_t error_ڪوڊ, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);

  (فراغ)session;

  إذا(وهڪرو_id != ctx->tunnel.وهڪرو_id)
    أرجع 0;

  CURL_TRC_CF(معلومات, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              وهڪرو_id, nghttp2_http2_strerror(error_ڪوڊ), error_ڪوڊ);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_ڪوڊ;
  إذا(error_ڪوڊ)
    ctx->tunnel.reset = TRUE;

  أرجع 0;
}

ساكن CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  بنية Curl_cfilter *cf,
  بنية Curl_easy *معلومات,
  بنية curl_ngh2 *ngh2,
  بنية httpreq *req,
  ثابت nghttp2_ترجيح_spec *pri_spec,
  فراغ *وهڪرو_user_معلومات,
  nghttp2_معلومات_ذريعو_read_callback read_callback,
  فراغ *read_ctx)
{
  بنية dynhds h2_سرا;
  nghttp2_nv *nva = عدم;
  int32_t وهڪرو_id = -1;
  size_t nheader;
  CURLcode نتيجو;

  (فراغ)cf;
  Curl_dynhds_init(&h2_سرا, MAX_HTTP_RESP_سرو_ڳڻپ, DYN_HTTP_REQUEST);
  نتيجو = Curl_http_req_منزل_h2(&h2_سرا, req, معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;

  nva = Curl_dynhds_منزل_nva(&h2_سرا, &nheader);
  إذا(!nva) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  إذا(read_callback) {
    nghttp2_معلومات_provider معلومات_prd;

    معلومات_prd.read_callback = read_callback;
    معلومات_prd.ذريعو.ptr = read_ctx;
    وهڪرو_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &معلومات_prd, وهڪرو_user_معلومات);
  }
  وإلا {
    وهڪرو_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, عدم, وهڪرو_user_معلومات);
  }

  إذا(وهڪرو_id < 0) {
    failf(معلومات, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(وهڪرو_id), وهڪرو_id);
    نتيجو = CURLE_SEND_ERROR;
    اذهب ٻاهريون;
  }
  نتيجو = CURLE_OK;

ٻاهريون:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرا);
  *pstream_id = وهڪرو_id;
  أرجع نتيجو;
}

ساكن CURLcode submit_CONNECT(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية tunnel_وهڪرو *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتيجو;
  بنية httpreq *req = عدم;

  نتيجو = Curl_http_proxy_create_tunnel_request(&req, cf, معلومات,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  إذا(نتيجو)
    اذهب ٻاهريون;
  نتيجو = Curl_creader_سيٽ_null(معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;

  نتيجو = proxy_h2_submit(&ts->وهڪرو_id, cf, معلومات, &ctx->ngh2, req,
                           عدم, ts, tunnel_send_callback, cf);
  إذا(نتيجو) {
    CURL_TRC_CF(معلومات, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->وهڪرو_id, nghttp2_strerror(ts->وهڪرو_id));
  }

ٻاهريون:
  إذا(req)
    Curl_http_req_free(req);
  إذا(نتيجو)
    failf(معلومات, "Failed sending CONNECT to proxy");
  أرجع نتيجو;
}

ساكن CURLcode inspect_response(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية tunnel_وهڪرو *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_نتيجو res;
  CURLcode نتيجو;

  نتيجو = Curl_http_proxy_inspect_tunnel_response(
      cf, معلومات, ts->resp, (bool)ctx->udp_tunnel, &res);
  إذا(نتيجو)
    أرجع نتيجو;
  بدل(res) {
  حالة PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, معلومات,
                       (bool)ctx->udp_tunnel);
    اكسر;
  حالة PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, معلومات,
                       (bool)ctx->udp_tunnel);
    نتيجو = CURLE_COULDNT_CONNECT;
    اكسر;
  حالة PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, معلومات,
                       (bool)ctx->udp_tunnel);
    اكسر;
  }
  أرجع نتيجو;
}

ساكن CURLcode H2_CONNECT(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات,
                           بنية tunnel_وهڪرو *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

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
      نتيجو = submit_CONNECT(cf, معلومات, ts);
      إذا(نتيجو)
        اذهب ٻاهريون;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, معلومات,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    حالة H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      نتيجو = proxy_h2_ترقي_ingress(cf, معلومات);
      إذا(!نتيجو)
        نتيجو = proxy_h2_ترقي_egress(cf, معلومات);
      إذا(نتيجو && نتيجو != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, معلومات,
                           (bool)ctx->udp_tunnel);
        اكسر;
      }

      إذا(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, معلومات,
                           (bool)ctx->udp_tunnel);
      }
      وإلا {
        نتيجو = CURLE_OK;
        اذهب ٻاهريون;
      }
      FALLTHROUGH();

    حالة H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      نتيجو = inspect_response(cf, معلومات, ts);
      إذا(نتيجو)
        اذهب ٻاهريون;
      اكسر;

    حالة H2_TUNNEL_ESTABLISHED:
      أرجع CURLE_OK;

    حالة H2_TUNNEL_FAILED:
      أرجع CURLE_RECV_ERROR;

    افتراضي:
      اكسر;
    }

  } طالما(ts->state == H2_TUNNEL_INIT);

ٻاهريون:
  إذا((نتيجو && (نتيجو != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, معلومات,
                       (bool)ctx->udp_tunnel);
  أرجع نتيجو;
}

/*
 * Initialize the cfilter context
 */
ساكن CURLcode cf_h2_proxy_ctx_init(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  nghttp2_session_callbacks *cbs = عدم;
  صحيح rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, حجم(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_سائيز, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_سائيز, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  إذا(tunnel_وهڪرو_init(&ctx->tunnel, ctx->dest))
    اذهب ٻاهريون;

  rc = nghttp2_session_callbacks_new(&cbs);
  إذا(rc) {
    failf(معلومات, "Could not initialize nghttp2 callbacks");
    اذهب ٻاهريون;
  }

  nghttp2_session_callbacks_سيٽ_send_callback(cbs, on_session_send);
  nghttp2_session_callbacks_سيٽ_on_چوکٺو_recv_callback(
    cbs, proxy_h2_on_چوکٺو_recv);
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  nghttp2_session_callbacks_سيٽ_on_چوکٺو_send_callback(cbs,
                                                       proxy_h2_on_چوکٺو_send);
#جيڪڏهن_پورو
  nghttp2_session_callbacks_سيٽ_on_معلومات_chunk_recv_callback(
    cbs, tunnel_recv_callback);
  nghttp2_session_callbacks_سيٽ_on_وهڪرو_close_callback(
    cbs, proxy_h2_on_وهڪرو_close);
  nghttp2_session_callbacks_سيٽ_on_سرو_callback(cbs, proxy_h2_on_سرو);

  /* The nghttp2 session is not yet setup, do it */
  rc = proxy_h2_client_new(cf, cbs);
  إذا(rc) {
    failf(معلومات, "Could not initialize nghttp2");
    اذهب ٻاهريون;
  }

  {
    nghttp2_settings_داخلا iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].قدر = Curl_multi_max_concurrent_streams(معلومات->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_دري_سائيز;
    iv[1].قدر = H2_TUNNEL_دري_سائيز;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ڌڪو;
    iv[2].قدر = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    إذا(rc) {
      failf(معلومات, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      نتيجو = CURLE_HTTP2;
      اذهب ٻاهريون;
    }
  }

  rc = Curl_ngh2_session_سيٽ_local_دري_سائيز(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_دري_سائيز);
  إذا(rc) {
    failf(معلومات, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    نتيجو = CURLE_HTTP2;
    اذهب ٻاهريون;
  }

  /* all set, traffic will be send on connect */
  نتيجو = CURLE_OK;

ٻاهريون:
  إذا(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(معلومات, cf, "[0] init proxy ctx -> %d", (صحيح)نتيجو);
  أرجع نتيجو;
}

ساكن CURLcode cf_h2_proxy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    bool *پورو)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;
  بنية cf_call_معلومات save;
  بنية tunnel_وهڪرو *ts = &ctx->tunnel;

  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the lower filters first */
  إذا(!cf->ايندڙ->connected) {
    نتيجو = Curl_conn_cf_connect(cf->ايندڙ, معلومات, پورو);
    إذا(نتيجو || !*پورو)
      أرجع نتيجو;
  }

  *پورو = FALSE;

  CF_معلومات_SAVE(save, cf, معلومات);
  إذا(!ctx->ngh2.s) {
    نتيجو = cf_h2_proxy_ctx_init(cf, معلومات);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  DEBUGASSERT(ts->authority);

  إذا(Curl_timeleft_ms(معلومات) < 0) {
    failf(معلومات, "Proxy CONNECT aborted due to timeout");
    نتيجو = CURLE_OPERATION_TIMEDOUT;
    اذهب ٻاهريون;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  نتيجو = H2_CONNECT(cf, معلومات, ts);

ٻاهريون:
  *پورو = (نتيجو == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  إذا(*پورو) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&معلومات->req, معلومات);
    Curl_client_reset(معلومات);
  }
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
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
                                     بنية Curl_easy *معلومات, bool *پورو)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode نتيجو;
  صحيح rv;

  إذا(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  CF_معلومات_SAVE(save, cf, معلومات);

  إذا(!ctx->موڪليل_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ثابت uint8_t *)"shutdown", حجم("shutdown"));
    إذا(rv) {
      failf(معلومات, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }
    ctx->موڪليل_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  نتيجو = CURLE_OK;
  إذا(Curl_ngh2_session_want_write(&ctx->ngh2))
    نتيجو = proxy_h2_ترقي_egress(cf, معلومات);
  إذا(!نتيجو && Curl_ngh2_session_want_read(&ctx->ngh2))
    نتيجو = proxy_h2_ترقي_ingress(cf, معلومات);

  *پورو = (ctx->conn_closed ||
           (!نتيجو && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
ٻاهريون:
  CF_معلومات_RESTORE(cf, save);
  cf->shutdown = (نتيجو || *پورو);
  أرجع نتيجو;
}

ساكن bool cf_h2_proxy_معلومات_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  إذا((ctx && !Curl_bufq_is_خالي_جايون(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_خالي_جايون(&ctx->tunnel.recvbuf)))
    أرجع TRUE;
  أرجع cf->ايندڙ ? cf->ايندڙ->cft->has_معلومات_pending(cf->ايندڙ, معلومات) : FALSE;
}

ساكن CURLcode cf_h2_proxy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *معلومات,
                                           بنية easy_pollset *ps)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, معلومات);
  bool want_recv, want_send;
  CURLcode نتيجو = CURLE_OK;

  إذا(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_خالي_جايون(&ctx->outbufq) ||
                !Curl_bufq_is_خالي_جايون(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  وإلا
    Curl_pollset_check(معلومات, ps, sock, &want_recv, &want_send);

  إذا(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
    bool c_exhaust, s_exhaust;

    CF_معلومات_SAVE(save, cf, معلومات);
    c_exhaust = !Curl_ngh2_session_get_remote_دري_سائيز(&ctx->ngh2);
    s_exhaust = ctx->tunnel.وهڪرو_id >= 0 &&
                !Curl_ngh2_session_get_وهڪرو_remote_دري_سائيز(
                  &ctx->ngh2, ctx->tunnel.وهڪرو_id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_خالي_جايون(&ctx->outbufq) ||
                !Curl_bufq_is_خالي_جايون(&ctx->tunnel.sendbuf);

    نتيجو = Curl_pollset_سيٽ(معلومات, ps, sock, want_recv, want_send);
    CURL_TRC_CF(معلومات, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (صحيح)نتيجو);
    CF_معلومات_RESTORE(cf, save);
  }
  وإلا إذا(ctx->موڪليل_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_معلومات_SAVE(save, cf, معلومات);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_خالي_جايون(&ctx->outbufq) ||
                !Curl_bufq_is_خالي_جايون(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    نتيجو = Curl_pollset_سيٽ(معلومات, ps, sock, want_recv, want_send);
    CURL_TRC_CF(معلومات, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (صحيح)نتيجو);
    CF_معلومات_RESTORE(cf, save);
  }
  أرجع نتيجو;
}

ساكن CURLcode h2_handle_tunnel_close(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  إذا(ctx->tunnel.error) {
    failf(معلومات, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.وهڪرو_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    أرجع CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(معلومات, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.وهڪرو_id);
  أرجع CURLE_OK;
}

ساكن CURLcode tunnel_recv(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                            حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_AGAIN;

  *pnread = 0;
  إذا(!Curl_bufq_is_خالي_جايون(&ctx->tunnel.recvbuf))
    نتيجو = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  وإلا {
    إذا(ctx->tunnel.closed) {
      نتيجو = h2_handle_tunnel_close(cf, معلومات, pnread);
    }
    وإلا إذا(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_خالي_جايون(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.وهڪرو_id)) {
      نتيجو = CURLE_RECV_ERROR;
    }
    وإلا
      نتيجو = CURLE_AGAIN;
  }

  CURL_TRC_CF(معلومات, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.وهڪرو_id, len, (صحيح)نتيجو, *pnread);
  أرجع نتيجو;
}

ساكن CURLcode cf_h2_proxy_recv(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 حرف *buf, size_t len,
                                 size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode نتيجو;

  *pnread = 0;
  CF_معلومات_SAVE(save, cf, معلومات);

  إذا(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    نتيجو = CURLE_RECV_ERROR;
    اذهب ٻاهريون;
  }

  إذا(Curl_bufq_is_خالي_جايون(&ctx->tunnel.recvbuf)) {
    نتيجو = proxy_h2_ترقي_ingress(cf, معلومات);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  نتيجو = tunnel_recv(cf, معلومات, buf, len, pnread);

  إذا(!نتيجو) {
    CURL_TRC_CF(معلومات, cf, "[%d] increase window by %zu",
                ctx->tunnel.وهڪرو_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.وهڪرو_id, *pnread);
  }

  نتيجو = Curl_1st_fatal(نتيجو, proxy_h2_ترقي_egress(cf, معلومات));

ٻاهريون:
  إذا((!Curl_bufq_is_خالي_جايون(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_خالي_جايون(&ctx->tunnel.sendbuf)) &&
     (!نتيجو || (نتيجو == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, معلومات, &ctx->tunnel);
  }
  CURL_TRC_CF(معلومات, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.وهڪرو_id, len, (صحيح)نتيجو, *pnread);
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

ساكن CURLcode cf_h2_proxy_send(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 ثابت uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode نتيجو;

  (فراغ)eos;
  *pnwritten = 0;
  CF_معلومات_SAVE(save, cf, معلومات);

  إذا(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    نتيجو = CURLE_SEND_ERROR;
    اذهب ٻاهريون;
  }

  إذا(ctx->tunnel.closed) {
    نتيجو = CURLE_SEND_ERROR;
    اذهب ٻاهريون;
  }

  نتيجو = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(معلومات, cf, "cf_send(), bufq_write %d, %zu", (صحيح)نتيجو,
              *pnwritten);
  إذا(نتيجو && (نتيجو != CURLE_AGAIN))
    اذهب ٻاهريون;

  إذا(!Curl_bufq_is_خالي_جايون(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_معلومات(&ctx->ngh2, ctx->tunnel.وهڪرو_id))) {
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }
  }

  نتيجو = Curl_1st_fatal(نتيجو, proxy_h2_ترقي_ingress(cf, معلومات));
  نتيجو = Curl_1st_fatal(نتيجو, proxy_h2_ترقي_egress(cf, معلومات));

  إذا(!نتيجو && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    إذا(ctx->tunnel.closed) {
      نتيجو = CURLE_SEND_ERROR;
    }
    وإلا {
      CURL_TRC_CF(معلومات, cf, "[0] send: nothing to do in this session");
      نتيجو = CURLE_HTTP2;
    }
  }

ٻاهريون:
  إذا((!Curl_bufq_is_خالي_جايون(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_خالي_جايون(&ctx->tunnel.sendbuf)) &&
     (!نتيجو || (نتيجو == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, معلومات, &ctx->tunnel);
  }
  CURL_TRC_CF(معلومات, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.وهڪرو_id, len, (صحيح)نتيجو, *pnwritten,
              Curl_ngh2_session_get_وهڪرو_remote_دري_سائيز(
                &ctx->ngh2, ctx->tunnel.وهڪرو_id),
              Curl_ngh2_session_get_remote_دري_سائيز(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

ساكن CURLcode cf_h2_proxy_flush(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode نتيجو = CURLE_OK;

  CF_معلومات_SAVE(save, cf, معلومات);
  إذا(!Curl_bufq_is_خالي_جايون(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_معلومات(&ctx->ngh2, ctx->tunnel.وهڪرو_id))) {
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }
  }

  نتيجو = proxy_h2_ترقي_egress(cf, معلومات);

ٻاهريون:
  CURL_TRC_CF(معلومات, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.وهڪرو_id, (صحيح)نتيجو,
              Curl_ngh2_session_get_وهڪرو_remote_دري_سائيز(
                &ctx->ngh2, ctx->tunnel.وهڪرو_id),
              Curl_ngh2_session_get_remote_دري_سائيز(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

ساكن bool proxy_h2_connisalive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 bool *داخلا_pending)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *داخلا_pending = FALSE;
  إذا(!cf->ايندڙ || !cf->ايندڙ->cft->is_alive(cf->ايندڙ, معلومات, داخلا_pending))
    أرجع FALSE;

  إذا(*داخلا_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode نتيجو;
    size_t nread;

    *داخلا_pending = FALSE;
    نتيجو = Curl_cf_recv_bufq(cf->ايندڙ, معلومات, &ctx->inbufq, 0, &nread);
    إذا(!نتيجو) {
      إذا(proxy_h2_process_pending_داخلا(cf, معلومات))
        /* immediate error, considered dead */
        alive = FALSE;
      وإلا {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    وإلا إذا(نتيجو != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  أرجع alive;
}

ساكن bool cf_h2_proxy_is_alive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 bool *داخلا_pending)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  بنية cf_call_معلومات save;

  *داخلا_pending = FALSE;
  CF_معلومات_SAVE(save, cf, معلومات);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, معلومات, داخلا_pending));
  CURL_TRC_CF(معلومات, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *داخلا_pending);
  CF_معلومات_RESTORE(cf, save);
  أرجع alive;
}

ساكن CURLcode cf_h2_proxy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_HOST_پورٽ:
    *pres1 = (صحيح)ctx->dest->پورٽ;
    *((ثابت حرف **)pres2) = ctx->dest->hostname;
    أرجع CURLE_OK;
  حالة CF_QUERY_NEED_FLUSH: {
    إذا(!Curl_bufq_is_خالي_جايون(&ctx->outbufq) ||
       !Curl_bufq_is_خالي_جايون(&ctx->tunnel.sendbuf)) {
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
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->query(cf->ايندڙ, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_h2_proxy_cntrl(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode نتيجو = CURLE_OK;
  بنية cf_call_معلومات save;

  (فراغ)arg1;
  (فراغ)arg2;

  بدل(event) {
  حالة CF_CTRL_FLUSH:
    CF_معلومات_SAVE(save, cf, معلومات);
    نتيجو = cf_h2_proxy_flush(cf, معلومات);
    CF_معلومات_RESTORE(cf, save);
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتيجو;
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

CURLcode Curl_cf_h2_proxy_وجهو_after(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *dest,
                                       bool udp_tunnel)
{
  بنية Curl_cfilter *cf_h2_proxy = عدم;
  بنية cf_h2_proxy_ctx *ctx;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;

  (فراغ)معلومات;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب ٻاهريون;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  نتيجو = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  إذا(نتيجو)
    اذهب ٻاهريون;
  ctx = عدم;
  Curl_conn_cf_وجهو_after(cf, cf_h2_proxy);

ٻاهريون:
  cf_h2_proxy_ctx_free(ctx);
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
