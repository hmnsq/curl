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

#تعریف_کن PROXY_H2_CHUNK_اندازه  (16 * 1024)

#تعریف_کن PROXY_HTTP2_HUGE_پنجره_اندازه (100 * 1024 * 1024)
#تعریف_کن H2_TUNNEL_پنجره_اندازه        (10 * 1024 * 1024)

#تعریف_کن PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_پنجره_اندازه / PROXY_H2_CHUNK_اندازه)
#تعریف_کن PROXY_H2_NW_SEND_CHUNKS   1

#تعریف_کن H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_پنجره_اندازه / PROXY_H2_CHUNK_اندازه)
#تعریف_کن H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_اندازه)


تعريفنوع تعداد {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

بنية tunnel_جریان {
  بنية http_resp *resp;
  بنية bufq recvbuf;
  بنية bufq sendbuf;
  حرف *authority;
  int32_t جریان_id;
  uint32_t error;
  h2_tunnel_state state;
  بیت(has_final_response);
  بیت(closed);
  بیت(reset);
};

ساكن CURLcode tunnel_جریان_init(بنية tunnel_جریان *ts,
                                   بنية Curl_peer *dest)
{
  ts->state = H2_TUNNEL_INIT;
  ts->جریان_id = -1;
  Curl_bufq_init2(&ts->recvbuf, PROXY_H2_CHUNK_اندازه, H2_TUNNEL_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد);
  Curl_bufq_init(&ts->sendbuf, PROXY_H2_CHUNK_اندازه, H2_TUNNEL_SEND_CHUNKS);

  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->پورت);
  إذا(!ts->authority)
    أرجع CURLE_بیرون_از_حافظه;

  أرجع CURLE_OK;
}

ساكن فراغ tunnel_جریان_reset(بنية tunnel_جریان *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = عدم;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->جریان_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

ساكن فراغ tunnel_جریان_clear(بنية tunnel_جریان *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, حجم(*ts));
  ts->state = H2_TUNNEL_INIT;
}

ساكن فراغ h2_tunnel_go_state(بنية Curl_cfilter *cf,
                               بنية tunnel_جریان *ts,
                               h2_tunnel_state new_state,
                               بنية Curl_easy *داده,
                               bool udp_tunnel)
{
  (فراغ)cf;
  (فراغ)udp_tunnel;

  إذا(ts->state == new_state)
    أرجع;
  /* leaving this one */
  بدل(ts->state) {
  حالة H2_TUNNEL_CONNECT:
    داده->req.ignorebody = FALSE;
    اكسر;
  افتراضي:
    اكسر;
  }
  /* entering this one */
  بدل(new_state) {
  حالة H2_TUNNEL_INIT:
    CURL_TRC_CF(داده, cf, "[%d] new tunnel state 'init'", ts->جریان_id);
    tunnel_جریان_reset(ts);
    اكسر;

  حالة H2_TUNNEL_CONNECT:
    CURL_TRC_CF(داده, cf, "[%d] new tunnel state 'connect'", ts->جریان_id);
    ts->state = H2_TUNNEL_CONNECT;
    اكسر;

  حالة H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(داده, cf, "[%d] new tunnel state 'response'", ts->جریان_id);
    ts->state = H2_TUNNEL_RESPONSE;
    اكسر;

  حالة H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(داده, cf, "[%d] new tunnel state 'established'",
                ts->جریان_id);
    infof(داده, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    داده->state.authproxy.انجامشده = TRUE;
    داده->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  حالة H2_TUNNEL_FAILED:
    إذا(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(داده, cf, "[%d] new tunnel state 'failed'", ts->جریان_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(داده->req.hd_proxy_auth);
    اكسر;
  }
}

بنية cf_h2_proxy_ctx {
  بنية curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  بنية cf_call_داده call_داده;

  بنية bufq inbufq;  /* network receive buffer */
  بنية bufq outbufq; /* network send buffer */

  بنية Curl_peer *peer; /* proxy we talk to */
  بنية Curl_peer *dest; /* where to tunnel to */
  بنية tunnel_جریان tunnel; /* our tunnel CONNECT stream */
  int32_t goaway_error;
  int32_t remote_max_sid;
  بیت(conn_closed);
  بیت(rcvd_goaway);
  بیت(فرستاده_goaway);
  بیت(nw_بیرون_blocked);
  بیت(udp_tunnel);
};

/* How to access `call_data` from a cf_h2 filter */
#لغو_تعریف CF_CTX_CALL_داده
#تعریف_کن CF_CTX_CALL_داده(cf) ((بنية cf_h2_proxy_ctx *)(cf)->ctx)->call_داده

ساكن فراغ cf_h2_proxy_ctx_clear(بنية cf_h2_proxy_ctx *ctx)
{
  بنية cf_call_داده save = ctx->call_داده;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_جریان_clear(&ctx->tunnel);
  memset(ctx, 0, حجم(*ctx));
  ctx->call_داده = save;
}

ساكن فراغ cf_h2_proxy_ctx_free(بنية cf_h2_proxy_ctx *ctx)
{
  إذا(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

ساكن فراغ drain_tunnel(بنية Curl_cfilter *cf,
                         بنية Curl_easy *داده,
                         بنية tunnel_جریان *tunnel)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  (فراغ)cf;
  إذا(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_خالی(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_خالی(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(داده);
}

ساكن CURLcode proxy_h2_nw_بیرون_writer(فراغ *writer_ctx,
                                       ثابت uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  بنية Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  إذا(cf) {
    بنية Curl_easy *داده = CF_داده_CURRENT(cf);
    CURLcode نتیجه;
    نتیجه = Curl_conn_cf_send(cf->بعدی, داده, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(داده, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (صحيح)نتیجه, *pnwritten);
    أرجع نتیجه;
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
  nghttp2_option_مجموعه_no_auto_پنجره_update(o, 1);
#إذا NGHTTP2_نسخه_رقم >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_مجموعه_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#پایان_اگر
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

ساكن CURLcode proxy_h2_nw_بیرون_flush(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode نتیجه;

  إذا(Curl_bufq_is_خالی(&ctx->outbufq))
    أرجع CURLE_OK;

  نتیجه = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_بیرون_writer, cf,
                          &nwritten);
  إذا(نتیجه) {
    إذا(نتیجه == CURLE_AGAIN) {
      CURL_TRC_CF(داده, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_بیرون_blocked = 1;
    }
    أرجع نتیجه;
  }
  CURL_TRC_CF(داده, cf, "[0] nw send buffer flushed");
  أرجع Curl_bufq_is_خالی(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ساكن CURLcode proxy_h2_process_pending_درون_داد(بنية Curl_cfilter *cf,
                                               بنية Curl_easy *داده)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  ثابت بیعلامت حرف *buf;
  size_t blen, nread;
  ssize_t rv;

  طالما(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ثابت uint8_t *)buf, blen);
    CURL_TRC_CF(داده, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    إذا(!curlx_sztouz(rv, &nread)) {
      failf(داده,
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
    إذا(Curl_bufq_is_خالی(&ctx->inbufq)) {
      CURL_TRC_CF(داده, cf, "[0] all data in connection buffer processed");
      اكسر;
    }
    وإلا {
      CURL_TRC_CF(داده, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode proxy_h2_پیشرفت_ingress(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *داده)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  إذا(!Curl_bufq_is_خالی(&ctx->inbufq)) {
    CURL_TRC_CF(داده, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    نتیجه = proxy_h2_process_pending_درون_داد(cf, داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  طالما(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_خالی(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_پر(&ctx->tunnel.recvbuf)) {

    نتیجه = Curl_cf_recv_bufq(cf->بعدی, داده, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(داده, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (صحيح)نتیجه, nread);
    إذا(نتیجه) {
      إذا(نتیجه != CURLE_AGAIN) {
        failf(داده, "Failed receiving HTTP2 proxy data");
        أرجع نتیجه;
      }
      اكسر;
    }
    وإلا إذا(nread == 0) {
      CURL_TRC_CF(داده, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      اكسر;
    }

    نتیجه = proxy_h2_process_pending_درون_داد(cf, داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode proxy_h2_پیشرفت_egress(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *داده)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  صحيح rv = 0;

  ctx->nw_بیرون_blocked = 0;
  طالما(!rv && !ctx->nw_بیرون_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  إذا(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(داده, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    أرجع CURLE_SEND_ERROR;
  }
  أرجع proxy_h2_nw_بیرون_flush(cf, داده);
}

ساكن ssize_t on_session_send(nghttp2_session *h2,
                               ثابت uint8_t *buf, size_t blen, صحيح flags,
                               فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  size_t nwritten;
  CURLcode نتیجه = CURLE_OK;

  (فراغ)h2;
  (فراغ)flags;
  DEBUGASSERT(داده);

  نتیجه = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_بیرون_writer, cf, &nwritten);
  إذا(نتیجه) {
    إذا(نتیجه == CURLE_AGAIN) {
      ctx->nw_بیرون_blocked = 1;
      أرجع NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(داده, "Failed sending HTTP2 data");
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  إذا(!nwritten)
    أرجع NGHTTP2_ERR_WOULDBLOCK;

  أرجع (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#اگر_تعریف_شده CURLVERBOSE
ساكن صحيح proxy_h2_on_قاب_send(nghttp2_session *session,
                                  ثابت nghttp2_قاب *قاب,
                                  فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);

  (فراغ)session;
  DEBUGASSERT(داده);
  إذا(Curl_trc_cf_is_verbose(cf, داده)) {
    حرف میانگیر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(قاب, میانگیر, حجم(میانگیر) - 1);
    میانگیر[len] = 0;
    CURL_TRC_CF(داده, cf, "[%d] -> %s", قاب->hd.جریان_id, میانگیر);
  }
  أرجع 0;
}
#پایان_اگر /* CURLVERBOSE */

ساكن صحيح proxy_h2_on_قاب_recv(nghttp2_session *session,
                                  ثابت nghttp2_قاب *قاب,
                                  فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  int32_t جریان_id = قاب->hd.جریان_id;

  (فراغ)session;
  DEBUGASSERT(داده);
#اگر_تعریف_شده CURLVERBOSE
  إذا(Curl_trc_cf_is_verbose(cf, داده)) {
    حرف میانگیر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(قاب, میانگیر, حجم(میانگیر) - 1);
    میانگیر[len] = 0;
    CURL_TRC_CF(داده, cf, "[%d] <- %s", قاب->hd.جریان_id, میانگیر);
  }
#پایان_اگر /* CURLVERBOSE */

  إذا(!جریان_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(داده);
    بدل(قاب->hd.type) {
    حالة NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      إذا(CURL_REQ_WANT_SEND(داده)) {
        drain_tunnel(cf, داده, &ctx->tunnel);
      }
      اكسر;
    حالة NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = قاب->goaway.آخرین_جریان_id;
      إذا(داده) {
        infof(داده, "received GOAWAY, error=%u, last_stream=%d",
              قاب->goaway.error_کد, ctx->remote_max_sid);
      }
      اكسر;
    افتراضي:
      اكسر;
    }
    أرجع 0;
  }

  إذا(جریان_id != ctx->tunnel.جریان_id) {
    CURL_TRC_CF(داده, cf, "[%d] rcvd FRAME not for tunnel", جریان_id);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  بدل(قاب->hd.type) {
  حالة NGHTTP2_سرآیندها:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    إذا(!ctx->tunnel.resp)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(داده, cf, "[%d] got http status: %d",
                جریان_id, ctx->tunnel.resp->وضعیت);
    إذا(!ctx->tunnel.has_final_response && ctx->tunnel.resp->وضعیت / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    اكسر;
  حالة NGHTTP2_پنجره_UPDATE:
    إذا(CURL_REQ_WANT_SEND(داده)) {
      drain_tunnel(cf, داده, &ctx->tunnel);
    }
    اكسر;
  حالة NGHTTP2_RST_جریان:
    إذا(قاب->rst_جریان.error_کد)
      ctx->tunnel.reset = TRUE;
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع 0;
}

ساكن صحيح proxy_h2_on_سرآیند(nghttp2_session *session,
                              ثابت nghttp2_قاب *قاب,
                              ثابت uint8_t *name, size_t namelen,
                              ثابت uint8_t *مقدار, size_t valuelen,
                              uint8_t flags,
                              فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  int32_t جریان_id = قاب->hd.جریان_id;
  CURLcode نتیجه;

  (فراغ)flags;
  (فراغ)session;
  DEBUGASSERT(جریان_id); /* should never be a zero stream ID here */
  إذا(جریان_id != ctx->tunnel.جریان_id) {
    CURL_TRC_CF(داده, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", جریان_id,
                (صحيح)namelen, name, (صحيح)valuelen, مقدار);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  إذا(قاب->hd.type == NGHTTP2_افزودن_PROMISE)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  إذا(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    أرجع 0;
  }

  إذا(namelen == CURL_CSTRLEN(HTTP_PSEUDO_وضعیت) &&
     !memcmp(HTTP_PSEUDO_وضعیت, name, namelen)) {
    int32_t http_وضعیت;
    بنية http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    نتیجه = Curl_http_decode_وضعیت(&http_وضعیت,
                                    (ثابت حرف *)مقدار, valuelen);
    إذا(نتیجه)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    نتیجه = Curl_http_resp_make(&resp, http_وضعیت, عدم);
    إذا(نتیجه)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    إذا(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(داده, cf, "[%d] status: HTTP/2 %03d",
                جریان_id, ctx->tunnel.resp->وضعیت);
    أرجع 0;
  }

  إذا(!ctx->tunnel.resp)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  نتیجه = Curl_dynhds_جمع_کن(&ctx->tunnel.resp->سرآیندها,
                           (ثابت حرف *)name, namelen,
                           (ثابت حرف *)مقدار, valuelen);
  إذا(نتیجه)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(داده, cf, "[%d] header: %.*s: %.*s",
              جریان_id, (صحيح)namelen, name, (صحيح)valuelen, مقدار);

  أرجع 0; /* 0 is successful */
}

ساكن ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t جریان_id,
                                    uint8_t *buf, size_t درازی,
                                    uint32_t *داده_flags,
                                    nghttp2_داده_منبع *منبع,
                                    فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  بنية tunnel_جریان *ts;
  CURLcode نتیجه;
  size_t nread;

  (فراغ)session;
  (فراغ)منبع;
  (فراغ)ctx;

  إذا(!جریان_id)
    أرجع NGHTTP2_ERR_INVALID_آرگومان;

  ts = Curl_ngh2_session_get_جریان_user_داده(&ctx->ngh2, جریان_id);
  إذا(!ts)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  نتیجه = Curl_bufq_read(&ts->sendbuf, buf, درازی, &nread);
  إذا(نتیجه) {
    إذا(نتیجه != CURLE_AGAIN)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    أرجع NGHTTP2_ERR_DEFERRED;
  }
  إذا(ts->closed && Curl_bufq_is_خالی(&ts->sendbuf))
    *داده_flags = NGHTTP2_داده_FLAG_EOF;

  CURL_TRC_CF(داده, cf, "[%d] tunnel_send_callback -> %zu",
              ts->جریان_id, nread);
  أرجع (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

ساكن صحيح tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t جریان_id,
                                ثابت uint8_t *mem, size_t len, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode نتیجه;

  (فراغ)flags;
  (فراغ)session;
  DEBUGASSERT(جریان_id); /* should never be a zero stream ID here */

  إذا(جریان_id != ctx->tunnel.جریان_id)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  نتیجه = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  إذا(نتیجه) {
    إذا(نتیجه != CURLE_AGAIN)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
#اگر_تعریف_شده DEBUGBUILD
    nwritten = 0;
#پایان_اگر
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  أرجع 0;
}

ساكن صحيح proxy_h2_on_جریان_close(nghttp2_session *session,
                                    int32_t جریان_id,
                                    uint32_t error_کد, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);

  (فراغ)session;

  إذا(جریان_id != ctx->tunnel.جریان_id)
    أرجع 0;

  CURL_TRC_CF(داده, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              جریان_id, nghttp2_http2_strerror(error_کد), error_کد);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_کد;
  إذا(error_کد)
    ctx->tunnel.reset = TRUE;

  أرجع 0;
}

ساكن CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  بنية Curl_cfilter *cf,
  بنية Curl_easy *داده,
  بنية curl_ngh2 *ngh2,
  بنية httpreq *req,
  ثابت nghttp2_اولویت_spec *pri_spec,
  فراغ *جریان_user_داده,
  nghttp2_داده_منبع_read_callback read_callback,
  فراغ *read_ctx)
{
  بنية dynhds h2_سرآیندها;
  nghttp2_nv *nva = عدم;
  int32_t جریان_id = -1;
  size_t nheader;
  CURLcode نتیجه;

  (فراغ)cf;
  Curl_dynhds_init(&h2_سرآیندها, MAX_HTTP_RESP_سرآیند_شمار, DYN_HTTP_REQUEST);
  نتیجه = Curl_http_req_مقصد_h2(&h2_سرآیندها, req, داده);
  إذا(نتیجه)
    اذهب بیرون;

  nva = Curl_dynhds_مقصد_nva(&h2_سرآیندها, &nheader);
  إذا(!nva) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  إذا(read_callback) {
    nghttp2_داده_provider داده_prd;

    داده_prd.read_callback = read_callback;
    داده_prd.منبع.ptr = read_ctx;
    جریان_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &داده_prd, جریان_user_داده);
  }
  وإلا {
    جریان_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, عدم, جریان_user_داده);
  }

  إذا(جریان_id < 0) {
    failf(داده, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(جریان_id), جریان_id);
    نتیجه = CURLE_SEND_ERROR;
    اذهب بیرون;
  }
  نتیجه = CURLE_OK;

بیرون:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرآیندها);
  *pstream_id = جریان_id;
  أرجع نتیجه;
}

ساكن CURLcode submit_CONNECT(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               بنية tunnel_جریان *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجه;
  بنية httpreq *req = عدم;

  نتیجه = Curl_http_proxy_create_tunnel_request(&req, cf, داده,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  إذا(نتیجه)
    اذهب بیرون;
  نتیجه = Curl_creader_مجموعه_null(داده);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = proxy_h2_submit(&ts->جریان_id, cf, داده, &ctx->ngh2, req,
                           عدم, ts, tunnel_send_callback, cf);
  إذا(نتیجه) {
    CURL_TRC_CF(داده, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->جریان_id, nghttp2_strerror(ts->جریان_id));
  }

بیرون:
  إذا(req)
    Curl_http_req_free(req);
  إذا(نتیجه)
    failf(داده, "Failed sending CONNECT to proxy");
  أرجع نتیجه;
}

ساكن CURLcode inspect_response(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 بنية tunnel_جریان *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_نتیجه res;
  CURLcode نتیجه;

  نتیجه = Curl_http_proxy_inspect_tunnel_response(
      cf, داده, ts->resp, (bool)ctx->udp_tunnel, &res);
  إذا(نتیجه)
    أرجع نتیجه;
  بدل(res) {
  حالة PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, داده,
                       (bool)ctx->udp_tunnel);
    اكسر;
  حالة PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, داده,
                       (bool)ctx->udp_tunnel);
    نتیجه = CURLE_COULDNT_CONNECT;
    اكسر;
  حالة PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, داده,
                       (bool)ctx->udp_tunnel);
    اكسر;
  }
  أرجع نتیجه;
}

ساكن CURLcode H2_CONNECT(بنية Curl_cfilter *cf,
                           بنية Curl_easy *داده,
                           بنية tunnel_جریان *ts)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  إذا(ctx->conn_closed) {
    failf(داده, "proxy closed connection");
    أرجع CURLE_COULDNT_CONNECT;
  }

  افعل {
    بدل(ts->state) {
    حالة H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(داده, cf, "[0] CONNECT start for %s", ts->authority);
      نتیجه = submit_CONNECT(cf, داده, ts);
      إذا(نتیجه)
        اذهب بیرون;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, داده,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    حالة H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      نتیجه = proxy_h2_پیشرفت_ingress(cf, داده);
      إذا(!نتیجه)
        نتیجه = proxy_h2_پیشرفت_egress(cf, داده);
      إذا(نتیجه && نتیجه != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, داده,
                           (bool)ctx->udp_tunnel);
        اكسر;
      }

      إذا(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, داده,
                           (bool)ctx->udp_tunnel);
      }
      وإلا {
        نتیجه = CURLE_OK;
        اذهب بیرون;
      }
      FALLTHROUGH();

    حالة H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      نتیجه = inspect_response(cf, داده, ts);
      إذا(نتیجه)
        اذهب بیرون;
      اكسر;

    حالة H2_TUNNEL_ESTABLISHED:
      أرجع CURLE_OK;

    حالة H2_TUNNEL_FAILED:
      أرجع CURLE_RECV_ERROR;

    افتراضي:
      اكسر;
    }

  } طالما(ts->state == H2_TUNNEL_INIT);

بیرون:
  إذا((نتیجه && (نتیجه != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, داده,
                       (bool)ctx->udp_tunnel);
  أرجع نتیجه;
}

/*
 * Initialize the cfilter context
 */
ساكن CURLcode cf_h2_proxy_ctx_init(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *داده)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  nghttp2_session_callbacks *cbs = عدم;
  صحيح rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, حجم(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_اندازه, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_اندازه, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  إذا(tunnel_جریان_init(&ctx->tunnel, ctx->dest))
    اذهب بیرون;

  rc = nghttp2_session_callbacks_new(&cbs);
  إذا(rc) {
    failf(داده, "Could not initialize nghttp2 callbacks");
    اذهب بیرون;
  }

  nghttp2_session_callbacks_مجموعه_send_callback(cbs, on_session_send);
  nghttp2_session_callbacks_مجموعه_on_قاب_recv_callback(
    cbs, proxy_h2_on_قاب_recv);
#اگر_تعریف_شده CURLVERBOSE
  nghttp2_session_callbacks_مجموعه_on_قاب_send_callback(cbs,
                                                       proxy_h2_on_قاب_send);
#پایان_اگر
  nghttp2_session_callbacks_مجموعه_on_داده_chunk_recv_callback(
    cbs, tunnel_recv_callback);
  nghttp2_session_callbacks_مجموعه_on_جریان_close_callback(
    cbs, proxy_h2_on_جریان_close);
  nghttp2_session_callbacks_مجموعه_on_سرآیند_callback(cbs, proxy_h2_on_سرآیند);

  /* The nghttp2 session is not yet setup, do it */
  rc = proxy_h2_client_new(cf, cbs);
  إذا(rc) {
    failf(داده, "Could not initialize nghttp2");
    اذهب بیرون;
  }

  {
    nghttp2_settings_مدخل iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].مقدار = Curl_multi_max_concurrent_streams(داده->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_پنجره_اندازه;
    iv[1].مقدار = H2_TUNNEL_پنجره_اندازه;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_افزودن;
    iv[2].مقدار = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    إذا(rc) {
      failf(داده, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      نتیجه = CURLE_HTTP2;
      اذهب بیرون;
    }
  }

  rc = Curl_ngh2_session_مجموعه_local_پنجره_اندازه(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_پنجره_اندازه);
  إذا(rc) {
    failf(داده, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    نتیجه = CURLE_HTTP2;
    اذهب بیرون;
  }

  /* all set, traffic will be send on connect */
  نتیجه = CURLE_OK;

بیرون:
  إذا(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(داده, cf, "[0] init proxy ctx -> %d", (صحيح)نتیجه);
  أرجع نتیجه;
}

ساكن CURLcode cf_h2_proxy_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    bool *انجامشده)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;
  بنية cf_call_داده save;
  بنية tunnel_جریان *ts = &ctx->tunnel;

  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the lower filters first */
  إذا(!cf->بعدی->connected) {
    نتیجه = Curl_conn_cf_connect(cf->بعدی, داده, انجامشده);
    إذا(نتیجه || !*انجامشده)
      أرجع نتیجه;
  }

  *انجامشده = FALSE;

  CF_داده_SAVE(save, cf, داده);
  إذا(!ctx->ngh2.s) {
    نتیجه = cf_h2_proxy_ctx_init(cf, داده);
    إذا(نتیجه)
      اذهب بیرون;
  }
  DEBUGASSERT(ts->authority);

  إذا(Curl_timeleft_ms(داده) < 0) {
    failf(داده, "Proxy CONNECT aborted due to timeout");
    نتیجه = CURLE_OPERATION_TIMEDOUT;
    اذهب بیرون;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  نتیجه = H2_CONNECT(cf, داده, ts);

بیرون:
  *انجامشده = (نتیجه == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  إذا(*انجامشده) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&داده->req, داده);
    Curl_client_reset(داده);
  }
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن فراغ cf_h2_proxy_destroy(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  (فراغ)داده;
  إذا(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_h2_proxy_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *داده, bool *انجامشده)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_داده save;
  CURLcode نتیجه;
  صحيح rv;

  إذا(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  CF_داده_SAVE(save, cf, داده);

  إذا(!ctx->فرستاده_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ثابت uint8_t *)"shutdown", حجم("shutdown"));
    إذا(rv) {
      failf(داده, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
    ctx->فرستاده_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  نتیجه = CURLE_OK;
  إذا(Curl_ngh2_session_want_write(&ctx->ngh2))
    نتیجه = proxy_h2_پیشرفت_egress(cf, داده);
  إذا(!نتیجه && Curl_ngh2_session_want_read(&ctx->ngh2))
    نتیجه = proxy_h2_پیشرفت_ingress(cf, داده);

  *انجامشده = (ctx->conn_closed ||
           (!نتیجه && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
بیرون:
  CF_داده_RESTORE(cf, save);
  cf->shutdown = (نتیجه || *انجامشده);
  أرجع نتیجه;
}

ساكن bool cf_h2_proxy_داده_pending(بنية Curl_cfilter *cf,
                                     ثابت بنية Curl_easy *داده)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  إذا((ctx && !Curl_bufq_is_خالی(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_خالی(&ctx->tunnel.recvbuf)))
    أرجع TRUE;
  أرجع cf->بعدی ? cf->بعدی->cft->has_داده_pending(cf->بعدی, داده) : FALSE;
}

ساكن CURLcode cf_h2_proxy_adjust_pollset(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *داده,
                                           بنية easy_pollset *ps)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_داده save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, داده);
  bool want_recv, want_send;
  CURLcode نتیجه = CURLE_OK;

  إذا(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_خالی(&ctx->outbufq) ||
                !Curl_bufq_is_خالی(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  وإلا
    Curl_pollset_check(داده, ps, sock, &want_recv, &want_send);

  إذا(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
    bool c_exhaust, s_exhaust;

    CF_داده_SAVE(save, cf, داده);
    c_exhaust = !Curl_ngh2_session_get_remote_پنجره_اندازه(&ctx->ngh2);
    s_exhaust = ctx->tunnel.جریان_id >= 0 &&
                !Curl_ngh2_session_get_جریان_remote_پنجره_اندازه(
                  &ctx->ngh2, ctx->tunnel.جریان_id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_خالی(&ctx->outbufq) ||
                !Curl_bufq_is_خالی(&ctx->tunnel.sendbuf);

    نتیجه = Curl_pollset_مجموعه(داده, ps, sock, want_recv, want_send);
    CURL_TRC_CF(داده, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (صحيح)نتیجه);
    CF_داده_RESTORE(cf, save);
  }
  وإلا إذا(ctx->فرستاده_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_داده_SAVE(save, cf, داده);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_خالی(&ctx->outbufq) ||
                !Curl_bufq_is_خالی(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    نتیجه = Curl_pollset_مجموعه(داده, ps, sock, want_recv, want_send);
    CURL_TRC_CF(داده, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (صحيح)نتیجه);
    CF_داده_RESTORE(cf, save);
  }
  أرجع نتیجه;
}

ساكن CURLcode h2_handle_tunnel_close(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *داده,
                                       size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  إذا(ctx->tunnel.error) {
    failf(داده, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.جریان_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    أرجع CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(داده, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.جریان_id);
  أرجع CURLE_OK;
}

ساكن CURLcode tunnel_recv(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                            حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_AGAIN;

  *pnread = 0;
  إذا(!Curl_bufq_is_خالی(&ctx->tunnel.recvbuf))
    نتیجه = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  وإلا {
    إذا(ctx->tunnel.closed) {
      نتیجه = h2_handle_tunnel_close(cf, داده, pnread);
    }
    وإلا إذا(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_خالی(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.جریان_id)) {
      نتیجه = CURLE_RECV_ERROR;
    }
    وإلا
      نتیجه = CURLE_AGAIN;
  }

  CURL_TRC_CF(داده, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.جریان_id, len, (صحيح)نتیجه, *pnread);
  أرجع نتیجه;
}

ساكن CURLcode cf_h2_proxy_recv(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 حرف *buf, size_t len,
                                 size_t *pnread)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_داده save;
  CURLcode نتیجه;

  *pnread = 0;
  CF_داده_SAVE(save, cf, داده);

  إذا(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    نتیجه = CURLE_RECV_ERROR;
    اذهب بیرون;
  }

  إذا(Curl_bufq_is_خالی(&ctx->tunnel.recvbuf)) {
    نتیجه = proxy_h2_پیشرفت_ingress(cf, داده);
    إذا(نتیجه)
      اذهب بیرون;
  }

  نتیجه = tunnel_recv(cf, داده, buf, len, pnread);

  إذا(!نتیجه) {
    CURL_TRC_CF(داده, cf, "[%d] increase window by %zu",
                ctx->tunnel.جریان_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.جریان_id, *pnread);
  }

  نتیجه = Curl_1st_fatal(نتیجه, proxy_h2_پیشرفت_egress(cf, داده));

بیرون:
  إذا((!Curl_bufq_is_خالی(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_خالی(&ctx->tunnel.sendbuf)) &&
     (!نتیجه || (نتیجه == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, داده, &ctx->tunnel);
  }
  CURL_TRC_CF(داده, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.جریان_id, len, (صحيح)نتیجه, *pnread);
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن CURLcode cf_h2_proxy_send(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 ثابت uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_داده save;
  CURLcode نتیجه;

  (فراغ)eos;
  *pnwritten = 0;
  CF_داده_SAVE(save, cf, داده);

  إذا(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    نتیجه = CURLE_SEND_ERROR;
    اذهب بیرون;
  }

  إذا(ctx->tunnel.closed) {
    نتیجه = CURLE_SEND_ERROR;
    اذهب بیرون;
  }

  نتیجه = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(داده, cf, "cf_send(), bufq_write %d, %zu", (صحيح)نتیجه,
              *pnwritten);
  إذا(نتیجه && (نتیجه != CURLE_AGAIN))
    اذهب بیرون;

  إذا(!Curl_bufq_is_خالی(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_داده(&ctx->ngh2, ctx->tunnel.جریان_id))) {
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
  }

  نتیجه = Curl_1st_fatal(نتیجه, proxy_h2_پیشرفت_ingress(cf, داده));
  نتیجه = Curl_1st_fatal(نتیجه, proxy_h2_پیشرفت_egress(cf, داده));

  إذا(!نتیجه && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    إذا(ctx->tunnel.closed) {
      نتیجه = CURLE_SEND_ERROR;
    }
    وإلا {
      CURL_TRC_CF(داده, cf, "[0] send: nothing to do in this session");
      نتیجه = CURLE_HTTP2;
    }
  }

بیرون:
  إذا((!Curl_bufq_is_خالی(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_خالی(&ctx->tunnel.sendbuf)) &&
     (!نتیجه || (نتیجه == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, داده, &ctx->tunnel);
  }
  CURL_TRC_CF(داده, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.جریان_id, len, (صحيح)نتیجه, *pnwritten,
              Curl_ngh2_session_get_جریان_remote_پنجره_اندازه(
                &ctx->ngh2, ctx->tunnel.جریان_id),
              Curl_ngh2_session_get_remote_پنجره_اندازه(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن CURLcode cf_h2_proxy_flush(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  بنية cf_call_داده save;
  CURLcode نتیجه = CURLE_OK;

  CF_داده_SAVE(save, cf, داده);
  إذا(!Curl_bufq_is_خالی(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_داده(&ctx->ngh2, ctx->tunnel.جریان_id))) {
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
  }

  نتیجه = proxy_h2_پیشرفت_egress(cf, داده);

بیرون:
  CURL_TRC_CF(داده, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.جریان_id, (صحيح)نتیجه,
              Curl_ngh2_session_get_جریان_remote_پنجره_اندازه(
                &ctx->ngh2, ctx->tunnel.جریان_id),
              Curl_ngh2_session_get_remote_پنجره_اندازه(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن bool proxy_h2_connisalive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 bool *درون_داد_pending)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *درون_داد_pending = FALSE;
  إذا(!cf->بعدی || !cf->بعدی->cft->is_alive(cf->بعدی, داده, درون_داد_pending))
    أرجع FALSE;

  إذا(*درون_داد_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode نتیجه;
    size_t nread;

    *درون_داد_pending = FALSE;
    نتیجه = Curl_cf_recv_bufq(cf->بعدی, داده, &ctx->inbufq, 0, &nread);
    إذا(!نتیجه) {
      إذا(proxy_h2_process_pending_درون_داد(cf, داده))
        /* immediate error, considered dead */
        alive = FALSE;
      وإلا {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    وإلا إذا(نتیجه != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  أرجع alive;
}

ساكن bool cf_h2_proxy_is_alive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 bool *درون_داد_pending)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  بنية cf_call_داده save;

  *درون_داد_pending = FALSE;
  CF_داده_SAVE(save, cf, داده);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, داده, درون_داد_pending));
  CURL_TRC_CF(داده, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *درون_داد_pending);
  CF_داده_RESTORE(cf, save);
  أرجع alive;
}

ساكن CURLcode cf_h2_proxy_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_h2_proxy_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_HOST_پورت:
    *pres1 = (صحيح)ctx->dest->پورت;
    *((ثابت حرف **)pres2) = ctx->dest->hostname;
    أرجع CURLE_OK;
  حالة CF_QUERY_NEED_FLUSH: {
    إذا(!Curl_bufq_is_خالی(&ctx->outbufq) ||
       !Curl_bufq_is_خالی(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(داده, cf, "needs flush");
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
  أرجع cf->بعدی ?
    cf->بعدی->cft->query(cf->بعدی, داده, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_h2_proxy_cntrl(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode نتیجه = CURLE_OK;
  بنية cf_call_داده save;

  (فراغ)arg1;
  (فراغ)arg2;

  بدل(event) {
  حالة CF_CTRL_FLUSH:
    CF_داده_SAVE(save, cf, داده);
    نتیجه = cf_h2_proxy_flush(cf, داده);
    CF_داده_RESTORE(cf, save);
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتیجه;
}

بنية Curl_cftype Curl_cft_h2_proxy = {
  "H2-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  cf_h2_proxy_destroy,
  cf_h2_proxy_connect,
  cf_h2_proxy_shutdown,
  cf_h2_proxy_adjust_pollset,
  cf_h2_proxy_داده_pending,
  cf_h2_proxy_send,
  cf_h2_proxy_recv,
  cf_h2_proxy_cntrl,
  cf_h2_proxy_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h2_proxy_query,
};

CURLcode Curl_cf_h2_proxy_درج_after(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *داده,
                                       بنية Curl_peer *peer,
                                       بنية Curl_peer *dest,
                                       bool udp_tunnel)
{
  بنية Curl_cfilter *cf_h2_proxy = عدم;
  بنية cf_h2_proxy_ctx *ctx;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;

  (فراغ)داده;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب بیرون;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  نتیجه = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  إذا(نتیجه)
    اذهب بیرون;
  ctx = عدم;
  Curl_conn_cf_درج_after(cf, cf_h2_proxy);

بیرون:
  cf_h2_proxy_ctx_free(ctx);
  أرجع نتیجه;
}

#پایان_اگر /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
