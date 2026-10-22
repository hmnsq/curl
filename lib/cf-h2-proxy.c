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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_NGHTTP2)

#অন্তর্ভাব <nghttp2/nghttp2.h>

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "bufq.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "dynhds.h"
#অন্তর্ভাব "http2.h"
#অন্তর্ভাব "http_proxy.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "ngh2_shim.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "cf-h2-proxy.h"

#define PROXY_H2_CHUNK_আকার  (16 * 1024)

#define PROXY_HTTP2_HUGE_WINDOW_আকার (100 * 1024 * 1024)
#define H2_TUNNEL_WINDOW_আকার        (10 * 1024 * 1024)

#define PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_WINDOW_আকার / PROXY_H2_CHUNK_আকার)
#define PROXY_H2_NW_SEND_CHUNKS   1

#define H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_WINDOW_আকার / PROXY_H2_CHUNK_আকার)
#define H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_আকার)


প্রকারলেখ গণনা {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

কাঠামো tunnel_stream {
  কাঠামো http_resp *resp;
  কাঠামো bufq recvbuf;
  কাঠামো bufq sendbuf;
  অক্ষর *authority;
  int32_t stream_id;
  uint32_t error;
  h2_tunnel_state state;
  BIT(has_final_response);
  BIT(closed);
  BIT(reset);
};

স্থির CURLcode tunnel_stream_init(কাঠামো tunnel_stream *ts,
                                   কাঠামো Curl_peer *dest)
{
  ts->state = H2_TUNNEL_INIT;
  ts->stream_id = -1;
  Curl_bufq_init2(&ts->recvbuf, PROXY_H2_CHUNK_আকার, H2_TUNNEL_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init(&ts->sendbuf, PROXY_H2_CHUNK_আকার, H2_TUNNEL_SEND_CHUNKS);

  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->port);
  যদি(!ts->authority)
    ফেরত CURLE_OUT_OF_MEMORY;

  ফেরত CURLE_OK;
}

স্থির শূন্য tunnel_stream_reset(কাঠামো tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = ফাঁকা;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->stream_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

স্থির শূন্য tunnel_stream_clear(কাঠামো tunnel_stream *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, আকার(*ts));
  ts->state = H2_TUNNEL_INIT;
}

স্থির শূন্য h2_tunnel_go_state(কাঠামো Curl_cfilter *cf,
                               কাঠামো tunnel_stream *ts,
                               h2_tunnel_state new_state,
                               কাঠামো Curl_easy *তথ্য,
                               bool udp_tunnel)
{
  (শূন্য)cf;
  (শূন্য)udp_tunnel;

  যদি(ts->state == new_state)
    ফেরত;
  /* leaving this one */
  নির্বাচন(ts->state) {
  ক্ষেত্র H2_TUNNEL_CONNECT:
    তথ্য->req.ignorebody = FALSE;
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  /* entering this one */
  নির্বাচন(new_state) {
  ক্ষেত্র H2_TUNNEL_INIT:
    CURL_TRC_CF(তথ্য, cf, "[%d] new tunnel state 'init'", ts->stream_id);
    tunnel_stream_reset(ts);
    বিরতি;

  ক্ষেত্র H2_TUNNEL_CONNECT:
    CURL_TRC_CF(তথ্য, cf, "[%d] new tunnel state 'connect'", ts->stream_id);
    ts->state = H2_TUNNEL_CONNECT;
    বিরতি;

  ক্ষেত্র H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(তথ্য, cf, "[%d] new tunnel state 'response'", ts->stream_id);
    ts->state = H2_TUNNEL_RESPONSE;
    বিরতি;

  ক্ষেত্র H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(তথ্য, cf, "[%d] new tunnel state 'established'",
                ts->stream_id);
    infof(তথ্য, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    তথ্য->state.authproxy.done = TRUE;
    তথ্য->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  ক্ষেত্র H2_TUNNEL_FAILED:
    যদি(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(তথ্য, cf, "[%d] new tunnel state 'failed'", ts->stream_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(তথ্য->req.hd_proxy_auth);
    বিরতি;
  }
}

কাঠামো cf_h2_proxy_ctx {
  কাঠামো curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  কাঠামো cf_call_তথ্য call_তথ্য;

  কাঠামো bufq inbufq;  /* network receive buffer */
  কাঠামো bufq outbufq; /* network send buffer */

  কাঠামো Curl_peer *peer; /* proxy we talk to */
  কাঠামো Curl_peer *dest; /* where to tunnel to */
  কাঠামো tunnel_stream tunnel; /* our tunnel CONNECT stream */
  int32_t goaway_error;
  int32_t remote_max_sid;
  BIT(conn_closed);
  BIT(rcvd_goaway);
  BIT(sent_goaway);
  BIT(nw_out_blocked);
  BIT(udp_tunnel);
};

/* How to access `call_data` from a cf_h2 filter */
#undef CF_CTX_CALL_তথ্য
#define CF_CTX_CALL_তথ্য(cf) ((কাঠামো cf_h2_proxy_ctx *)(cf)->ctx)->call_তথ্য

স্থির শূন্য cf_h2_proxy_ctx_clear(কাঠামো cf_h2_proxy_ctx *ctx)
{
  কাঠামো cf_call_তথ্য save = ctx->call_তথ্য;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_stream_clear(&ctx->tunnel);
  memset(ctx, 0, আকার(*ctx));
  ctx->call_তথ্য = save;
}

স্থির শূন্য cf_h2_proxy_ctx_free(কাঠামো cf_h2_proxy_ctx *ctx)
{
  যদি(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

স্থির শূন্য drain_tunnel(কাঠামো Curl_cfilter *cf,
                         কাঠামো Curl_easy *তথ্য,
                         কাঠামো tunnel_stream *tunnel)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  (শূন্য)cf;
  যদি(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_empty(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(তথ্য);
}

স্থির CURLcode proxy_h2_nw_out_writer(শূন্য *writer_ctx,
                                       ধ্রুবক uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  কাঠামো Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  যদি(cf) {
    কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
    CURLcode ফলাফল;
    ফলাফল = Curl_conn_cf_send(cf->পরবর্তী, তথ্য, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(তথ্য, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (পূর্ণসংখ্যা)ফলাফল, *pnwritten);
    ফেরত ফলাফল;
  }
  ফেরত CURLE_FAILED_INIT;
}

স্থির পূর্ণসংখ্যা proxy_h2_client_new(কাঠামো Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ফাঁকা, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  পূর্ণসংখ্যা rc = nghttp2_option_new(&o);
  যদি(rc)
    ফেরত rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#যদি NGHTTP2_VERSION_অঙ্ক >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ফেরত rc;
}

স্থির পূর্ণসংখ্যা proxy_h2_should_close_session(কাঠামো cf_h2_proxy_ctx *ctx)
{
  ফেরত !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

স্থির CURLcode proxy_h2_nw_out_flush(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode ফলাফল;

  যদি(Curl_bufq_is_empty(&ctx->outbufq))
    ফেরত CURLE_OK;

  ফলাফল = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_out_writer, cf,
                          &nwritten);
  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_AGAIN) {
      CURL_TRC_CF(তথ্য, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    ফেরত ফলাফল;
  }
  CURL_TRC_CF(তথ্য, cf, "[0] nw send buffer flushed");
  ফেরত Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
স্থির CURLcode proxy_h2_process_pending_প্রবেশ(কাঠামো Curl_cfilter *cf,
                                               কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  ধ্রুবক unsigned অক্ষর *buf;
  size_t blen, nread;
  ssize_t rv;

  যখন(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ধ্রুবক uint8_t *)buf, blen);
    CURL_TRC_CF(তথ্য, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    যদি(!curlx_sztouz(rv, &nread)) {
      failf(তথ্য,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((পূর্ণসংখ্যা)rv));
      ফেরত CURLE_RECV_ERROR;
    }
    অন্যথা যদি(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      বিরতি;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    যদি(Curl_bufq_is_empty(&ctx->inbufq)) {
      CURL_TRC_CF(তথ্য, cf, "[0] all data in connection buffer processed");
      বিরতি;
    }
    অন্যথা {
      CURL_TRC_CF(তথ্য, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode proxy_h2_progress_ingress(কাঠামো Curl_cfilter *cf,
                                          কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  যদি(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(তথ্য, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    ফলাফল = proxy_h2_process_pending_প্রবেশ(cf, তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  যখন(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_empty(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_full(&ctx->tunnel.recvbuf)) {

    ফলাফল = Curl_cf_recv_bufq(cf->পরবর্তী, তথ্য, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(তথ্য, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (পূর্ণসংখ্যা)ফলাফল, nread);
    যদি(ফলাফল) {
      যদি(ফলাফল != CURLE_AGAIN) {
        failf(তথ্য, "Failed receiving HTTP2 proxy data");
        ফেরত ফলাফল;
      }
      বিরতি;
    }
    অন্যথা যদি(nread == 0) {
      CURL_TRC_CF(তথ্য, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      বিরতি;
    }

    ফলাফল = proxy_h2_process_pending_প্রবেশ(cf, তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode proxy_h2_progress_egress(কাঠামো Curl_cfilter *cf,
                                         কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  পূর্ণসংখ্যা rv = 0;

  ctx->nw_out_blocked = 0;
  যখন(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  যদি(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(তথ্য, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ফেরত CURLE_SEND_ERROR;
  }
  ফেরত proxy_h2_nw_out_flush(cf, তথ্য);
}

স্থির ssize_t on_session_send(nghttp2_session *h2,
                               ধ্রুবক uint8_t *buf, size_t blen, পূর্ণসংখ্যা flags,
                               শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  size_t nwritten;
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)h2;
  (শূন্য)flags;
  DEBUGASSERT(তথ্য);

  ফলাফল = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_out_writer, cf, &nwritten);
  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      ফেরত NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(তথ্য, "Failed sending HTTP2 data");
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  যদি(!nwritten)
    ফেরত NGHTTP2_ERR_WOULDBLOCK;

  ফেরত (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#ifdef CURLVERBOSE
স্থির পূর্ণসংখ্যা proxy_h2_on_frame_send(nghttp2_session *session,
                                  ধ্রুবক nghttp2_frame *frame,
                                  শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);

  (শূন্য)session;
  DEBUGASSERT(তথ্য);
  যদি(Curl_trc_cf_is_verbose(cf, তথ্য)) {
    অক্ষর বাফার[256];
    পূর্ণসংখ্যা len;
    len = Curl_nghttp2_fr_print(frame, বাফার, আকার(বাফার) - 1);
    বাফার[len] = 0;
    CURL_TRC_CF(তথ্য, cf, "[%d] -> %s", frame->hd.stream_id, বাফার);
  }
  ফেরত 0;
}
#endif /* CURLVERBOSE */

স্থির পূর্ণসংখ্যা proxy_h2_on_frame_recv(nghttp2_session *session,
                                  ধ্রুবক nghttp2_frame *frame,
                                  শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;

  (শূন্য)session;
  DEBUGASSERT(তথ্য);
#ifdef CURLVERBOSE
  যদি(Curl_trc_cf_is_verbose(cf, তথ্য)) {
    অক্ষর বাফার[256];
    পূর্ণসংখ্যা len;
    len = Curl_nghttp2_fr_print(frame, বাফার, আকার(বাফার) - 1);
    বাফার[len] = 0;
    CURL_TRC_CF(তথ্য, cf, "[%d] <- %s", frame->hd.stream_id, বাফার);
  }
#endif /* CURLVERBOSE */

  যদি(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(তথ্য);
    নির্বাচন(frame->hd.type) {
    ক্ষেত্র NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      যদি(CURL_REQ_WANT_SEND(তথ্য)) {
        drain_tunnel(cf, তথ্য, &ctx->tunnel);
      }
      বিরতি;
    ক্ষেত্র NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      যদি(তথ্য) {
        infof(তথ্য, "received GOAWAY, error=%u, last_stream=%d",
              frame->goaway.error_code, ctx->remote_max_sid);
      }
      বিরতি;
    পূর্বনির্ধারিত:
      বিরতি;
    }
    ফেরত 0;
  }

  যদি(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(তথ্য, cf, "[%d] rcvd FRAME not for tunnel", stream_id);
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  নির্বাচন(frame->hd.type) {
  ক্ষেত্র NGHTTP2_HEADERS:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    যদি(!ctx->tunnel.resp)
      ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(তথ্য, cf, "[%d] got http status: %d",
                stream_id, ctx->tunnel.resp->status);
    যদি(!ctx->tunnel.has_final_response && ctx->tunnel.resp->status / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    বিরতি;
  ক্ষেত্র NGHTTP2_WINDOW_UPDATE:
    যদি(CURL_REQ_WANT_SEND(তথ্য)) {
      drain_tunnel(cf, তথ্য, &ctx->tunnel);
    }
    বিরতি;
  ক্ষেত্র NGHTTP2_RST_STREAM:
    যদি(frame->rst_stream.error_code)
      ctx->tunnel.reset = TRUE;
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা proxy_h2_on_header(nghttp2_session *session,
                              ধ্রুবক nghttp2_frame *frame,
                              ধ্রুবক uint8_t *name, size_t namelen,
                              ধ্রুবক uint8_t *মান, size_t valuelen,
                              uint8_t flags,
                              শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  int32_t stream_id = frame->hd.stream_id;
  CURLcode ফলাফল;

  (শূন্য)flags;
  (শূন্য)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  যদি(stream_id != ctx->tunnel.stream_id) {
    CURL_TRC_CF(তথ্য, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", stream_id,
                (পূর্ণসংখ্যা)namelen, name, (পূর্ণসংখ্যা)valuelen, মান);
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  যদি(frame->hd.type == NGHTTP2_ঠেলো_PROMISE)
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;

  যদি(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    ফেরত 0;
  }

  যদি(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    int32_t http_status;
    কাঠামো http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    ফলাফল = Curl_http_decode_status(&http_status,
                                    (ধ্রুবক অক্ষর *)মান, valuelen);
    যদি(ফলাফল)
      ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
    ফলাফল = Curl_http_resp_make(&resp, http_status, ফাঁকা);
    যদি(ফলাফল)
      ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
    যদি(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(তথ্য, cf, "[%d] status: HTTP/2 %03d",
                stream_id, ctx->tunnel.resp->status);
    ফেরত 0;
  }

  যদি(!ctx->tunnel.resp)
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;

  ফলাফল = Curl_dynhds_add(&ctx->tunnel.resp->headers,
                           (ধ্রুবক অক্ষর *)name, namelen,
                           (ধ্রুবক অক্ষর *)মান, valuelen);
  যদি(ফলাফল)
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(তথ্য, cf, "[%d] header: %.*s: %.*s",
              stream_id, (পূর্ণসংখ্যা)namelen, name, (পূর্ণসংখ্যা)valuelen, মান);

  ফেরত 0; /* 0 is successful */
}

স্থির ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint8_t *buf, size_t দৈর্ঘ্য,
                                    uint32_t *তথ্য_flags,
                                    nghttp2_তথ্য_source *source,
                                    শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  কাঠামো tunnel_stream *ts;
  CURLcode ফলাফল;
  size_t nread;

  (শূন্য)session;
  (শূন্য)source;
  (শূন্য)ctx;

  যদি(!stream_id)
    ফেরত NGHTTP2_ERR_INVALID_ARGUMENT;

  ts = Curl_ngh2_session_get_stream_user_তথ্য(&ctx->ngh2, stream_id);
  যদি(!ts)
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  ফলাফল = Curl_bufq_read(&ts->sendbuf, buf, দৈর্ঘ্য, &nread);
  যদি(ফলাফল) {
    যদি(ফলাফল != CURLE_AGAIN)
      ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
    ফেরত NGHTTP2_ERR_DEFERRED;
  }
  যদি(ts->closed && Curl_bufq_is_empty(&ts->sendbuf))
    *তথ্য_flags = NGHTTP2_তথ্য_FLAG_EOF;

  CURL_TRC_CF(তথ্য, cf, "[%d] tunnel_send_callback -> %zu",
              ts->stream_id, nread);
  ফেরত (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

স্থির পূর্ণসংখ্যা tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t stream_id,
                                ধ্রুবক uint8_t *mem, size_t len, শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode ফলাফল;

  (শূন্য)flags;
  (শূন্য)session;
  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  যদি(stream_id != ctx->tunnel.stream_id)
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;

  ফলাফল = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  যদি(ফলাফল) {
    যদি(ফলাফল != CURLE_AGAIN)
      ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
#ifdef DEBUGBUILD
    nwritten = 0;
#endif
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা proxy_h2_on_stream_close(nghttp2_session *session,
                                    int32_t stream_id,
                                    uint32_t error_code, শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);

  (শূন্য)session;

  যদি(stream_id != ctx->tunnel.stream_id)
    ফেরত 0;

  CURL_TRC_CF(তথ্য, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_code;
  যদি(error_code)
    ctx->tunnel.reset = TRUE;

  ফেরত 0;
}

স্থির CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  কাঠামো Curl_cfilter *cf,
  কাঠামো Curl_easy *তথ্য,
  কাঠামো curl_ngh2 *ngh2,
  কাঠামো httpreq *req,
  ধ্রুবক nghttp2_priority_spec *pri_spec,
  শূন্য *stream_user_তথ্য,
  nghttp2_তথ্য_source_read_callback read_callback,
  শূন্য *read_ctx)
{
  কাঠামো dynhds h2_headers;
  nghttp2_nv *nva = ফাঁকা;
  int32_t stream_id = -1;
  size_t nheader;
  CURLcode ফলাফল;

  (শূন্য)cf;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_গুনতি, DYN_HTTP_REQUEST);
  ফলাফল = Curl_http_req_to_h2(&h2_headers, req, তথ্য);
  যদি(ফলাফল)
    যাও out;

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  যদি(!nva) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  যদি(read_callback) {
    nghttp2_তথ্য_provider তথ্য_prd;

    তথ্য_prd.read_callback = read_callback;
    তথ্য_prd.source.ptr = read_ctx;
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &তথ্য_prd, stream_user_তথ্য);
  }
  অন্যথা {
    stream_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, ফাঁকা, stream_user_তথ্য);
  }

  যদি(stream_id < 0) {
    failf(তথ্য, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(stream_id), stream_id);
    ফলাফল = CURLE_SEND_ERROR;
    যাও out;
  }
  ফলাফল = CURLE_OK;

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  *pstream_id = stream_id;
  ফেরত ফলাফল;
}

স্থির CURLcode submit_CONNECT(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো tunnel_stream *ts)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল;
  কাঠামো httpreq *req = ফাঁকা;

  ফলাফল = Curl_http_proxy_create_tunnel_request(&req, cf, তথ্য,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  যদি(ফলাফল)
    যাও out;
  ফলাফল = Curl_creader_set_null(তথ্য);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = proxy_h2_submit(&ts->stream_id, cf, তথ্য, &ctx->ngh2, req,
                           ফাঁকা, ts, tunnel_send_callback, cf);
  যদি(ফলাফল) {
    CURL_TRC_CF(তথ্য, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->stream_id, nghttp2_strerror(ts->stream_id));
  }

out:
  যদি(req)
    Curl_http_req_free(req);
  যদি(ফলাফল)
    failf(তথ্য, "Failed sending CONNECT to proxy");
  ফেরত ফলাফল;
}

স্থির CURLcode inspect_response(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো tunnel_stream *ts)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_ফলাফল res;
  CURLcode ফলাফল;

  ফলাফল = Curl_http_proxy_inspect_tunnel_response(
      cf, তথ্য, ts->resp, (bool)ctx->udp_tunnel, &res);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  নির্বাচন(res) {
  ক্ষেত্র PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, তথ্য,
                       (bool)ctx->udp_tunnel);
    বিরতি;
  ক্ষেত্র PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, তথ্য,
                       (bool)ctx->udp_tunnel);
    ফলাফল = CURLE_COULDNT_CONNECT;
    বিরতি;
  ক্ষেত্র PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, তথ্য,
                       (bool)ctx->udp_tunnel);
    বিরতি;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode H2_CONNECT(কাঠামো Curl_cfilter *cf,
                           কাঠামো Curl_easy *তথ্য,
                           কাঠামো tunnel_stream *ts)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  যদি(ctx->conn_closed) {
    failf(তথ্য, "proxy closed connection");
    ফেরত CURLE_COULDNT_CONNECT;
  }

  করো {
    নির্বাচন(ts->state) {
    ক্ষেত্র H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(তথ্য, cf, "[0] CONNECT start for %s", ts->authority);
      ফলাফল = submit_CONNECT(cf, তথ্য, ts);
      যদি(ফলাফল)
        যাও out;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, তথ্য,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    ক্ষেত্র H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      ফলাফল = proxy_h2_progress_ingress(cf, তথ্য);
      যদি(!ফলাফল)
        ফলাফল = proxy_h2_progress_egress(cf, তথ্য);
      যদি(ফলাফল && ফলাফল != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, তথ্য,
                           (bool)ctx->udp_tunnel);
        বিরতি;
      }

      যদি(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, তথ্য,
                           (bool)ctx->udp_tunnel);
      }
      অন্যথা {
        ফলাফল = CURLE_OK;
        যাও out;
      }
      FALLTHROUGH();

    ক্ষেত্র H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      ফলাফল = inspect_response(cf, তথ্য, ts);
      যদি(ফলাফল)
        যাও out;
      বিরতি;

    ক্ষেত্র H2_TUNNEL_ESTABLISHED:
      ফেরত CURLE_OK;

    ক্ষেত্র H2_TUNNEL_FAILED:
      ফেরত CURLE_RECV_ERROR;

    পূর্বনির্ধারিত:
      বিরতি;
    }

  } যখন(ts->state == H2_TUNNEL_INIT);

out:
  যদি((ফলাফল && (ফলাফল != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, তথ্য,
                       (bool)ctx->udp_tunnel);
  ফেরত ফলাফল;
}

/*
 * Initialize the cfilter context
 */
স্থির CURLcode cf_h2_proxy_ctx_init(কাঠামো Curl_cfilter *cf,
                                     কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  nghttp2_session_callbacks *cbs = ফাঁকা;
  পূর্ণসংখ্যা rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, আকার(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_আকার, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_আকার, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  যদি(tunnel_stream_init(&ctx->tunnel, ctx->dest))
    যাও out;

  rc = nghttp2_session_callbacks_new(&cbs);
  যদি(rc) {
    failf(তথ্য, "Could not initialize nghttp2 callbacks");
    যাও out;
  }

  nghttp2_session_callbacks_set_send_callback(cbs, on_session_send);
  nghttp2_session_callbacks_set_on_frame_recv_callback(
    cbs, proxy_h2_on_frame_recv);
#ifdef CURLVERBOSE
  nghttp2_session_callbacks_set_on_frame_send_callback(cbs,
                                                       proxy_h2_on_frame_send);
#endif
  nghttp2_session_callbacks_set_on_তথ্য_chunk_recv_callback(
    cbs, tunnel_recv_callback);
  nghttp2_session_callbacks_set_on_stream_close_callback(
    cbs, proxy_h2_on_stream_close);
  nghttp2_session_callbacks_set_on_header_callback(cbs, proxy_h2_on_header);

  /* The nghttp2 session is not yet setup, do it */
  rc = proxy_h2_client_new(cf, cbs);
  যদি(rc) {
    failf(তথ্য, "Could not initialize nghttp2");
    যাও out;
  }

  {
    nghttp2_settings_entry iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].মান = Curl_multi_max_concurrent_streams(তথ্য->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_আকার;
    iv[1].মান = H2_TUNNEL_WINDOW_আকার;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ঠেলো;
    iv[2].মান = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    যদি(rc) {
      failf(তথ্য, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      ফলাফল = CURLE_HTTP2;
      যাও out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_আকার(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_WINDOW_আকার);
  যদি(rc) {
    failf(তথ্য, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ফলাফল = CURLE_HTTP2;
    যাও out;
  }

  /* all set, traffic will be send on connect */
  ফলাফল = CURLE_OK;

out:
  যদি(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(তথ্য, cf, "[0] init proxy ctx -> %d", (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h2_proxy_connect(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    bool *done)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো cf_call_তথ্য save;
  কাঠামো tunnel_stream *ts = &ctx->tunnel;

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  /* Connect the lower filters first */
  যদি(!cf->পরবর্তী->connected) {
    ফলাফল = Curl_conn_cf_connect(cf->পরবর্তী, তথ্য, done);
    যদি(ফলাফল || !*done)
      ফেরত ফলাফল;
  }

  *done = FALSE;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  যদি(!ctx->ngh2.s) {
    ফলাফল = cf_h2_proxy_ctx_init(cf, তথ্য);
    যদি(ফলাফল)
      যাও out;
  }
  DEBUGASSERT(ts->authority);

  যদি(Curl_timeleft_ms(তথ্য) < 0) {
    failf(তথ্য, "Proxy CONNECT aborted due to timeout");
    ফলাফল = CURLE_OPERATION_TIMEDOUT;
    যাও out;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  ফলাফল = H2_CONNECT(cf, তথ্য, ts);

out:
  *done = (ফলাফল == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  যদি(*done) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&তথ্য->req, তথ্য);
    Curl_client_reset(তথ্য);
  }
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির শূন্য cf_h2_proxy_destroy(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;

  (শূন্য)তথ্য;
  যদি(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = ফাঁকা;
  }
}

স্থির CURLcode cf_h2_proxy_shutdown(কাঠামো Curl_cfilter *cf,
                                     কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা rv;

  যদি(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  CF_তথ্য_SAVE(save, cf, তথ্য);

  যদি(!ctx->sent_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ধ্রুবক uint8_t *)"shutdown", আকার("shutdown"));
    যদি(rv) {
      failf(তথ্য, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
    ctx->sent_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  ফলাফল = CURLE_OK;
  যদি(Curl_ngh2_session_want_write(&ctx->ngh2))
    ফলাফল = proxy_h2_progress_egress(cf, তথ্য);
  যদি(!ফলাফল && Curl_ngh2_session_want_read(&ctx->ngh2))
    ফলাফল = proxy_h2_progress_ingress(cf, তথ্য);

  *done = (ctx->conn_closed ||
           (!ফলাফল && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
out:
  CF_তথ্য_RESTORE(cf, save);
  cf->shutdown = (ফলাফল || *done);
  ফেরত ফলাফল;
}

স্থির bool cf_h2_proxy_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                                     ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  যদি((ctx && !Curl_bufq_is_empty(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_empty(&ctx->tunnel.recvbuf)))
    ফেরত TRUE;
  ফেরত cf->পরবর্তী ? cf->পরবর্তী->cft->has_তথ্য_pending(cf->পরবর্তী, তথ্য) : FALSE;
}

স্থির CURLcode cf_h2_proxy_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                           কাঠামো Curl_easy *তথ্য,
                                           কাঠামো easy_pollset *ps)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, তথ্য);
  bool want_recv, want_send;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  অন্যথা
    Curl_pollset_check(তথ্য, ps, sock, &want_recv, &want_send);

  যদি(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
    bool c_exhaust, s_exhaust;

    CF_তথ্য_SAVE(save, cf, তথ্য);
    c_exhaust = !Curl_ngh2_session_get_remote_window_আকার(&ctx->ngh2);
    s_exhaust = ctx->tunnel.stream_id >= 0 &&
                !Curl_ngh2_session_get_stream_remote_window_আকার(
                  &ctx->ngh2, ctx->tunnel.stream_id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);

    ফলাফল = Curl_pollset_set(তথ্য, ps, sock, want_recv, want_send);
    CURL_TRC_CF(তথ্য, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (পূর্ণসংখ্যা)ফলাফল);
    CF_তথ্য_RESTORE(cf, save);
  }
  অন্যথা যদি(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_তথ্য_SAVE(save, cf, তথ্য);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq) ||
                !Curl_bufq_is_empty(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    ফলাফল = Curl_pollset_set(তথ্য, ps, sock, want_recv, want_send);
    CURL_TRC_CF(তথ্য, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (পূর্ণসংখ্যা)ফলাফল);
    CF_তথ্য_RESTORE(cf, save);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode h2_handle_tunnel_close(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য,
                                       size_t *pnread)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  যদি(ctx->tunnel.error) {
    failf(তথ্য, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.stream_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    ফেরত CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(তথ্য, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.stream_id);
  ফেরত CURLE_OK;
}

স্থির CURLcode tunnel_recv(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                            অক্ষর *buf, size_t len, size_t *pnread)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_AGAIN;

  *pnread = 0;
  যদি(!Curl_bufq_is_empty(&ctx->tunnel.recvbuf))
    ফলাফল = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  অন্যথা {
    যদি(ctx->tunnel.closed) {
      ফলাফল = h2_handle_tunnel_close(cf, তথ্য, pnread);
    }
    অন্যথা যদি(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.stream_id)) {
      ফলাফল = CURLE_RECV_ERROR;
    }
    অন্যথা
      ফলাফল = CURLE_AGAIN;
  }

  CURL_TRC_CF(তথ্য, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (পূর্ণসংখ্যা)ফলাফল, *pnread);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h2_proxy_recv(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 অক্ষর *buf, size_t len,
                                 size_t *pnread)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  CURLcode ফলাফল;

  *pnread = 0;
  CF_তথ্য_SAVE(save, cf, তথ্য);

  যদি(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    ফলাফল = CURLE_RECV_ERROR;
    যাও out;
  }

  যদি(Curl_bufq_is_empty(&ctx->tunnel.recvbuf)) {
    ফলাফল = proxy_h2_progress_ingress(cf, তথ্য);
    যদি(ফলাফল)
      যাও out;
  }

  ফলাফল = tunnel_recv(cf, তথ্য, buf, len, pnread);

  যদি(!ফলাফল) {
    CURL_TRC_CF(তথ্য, cf, "[%d] increase window by %zu",
                ctx->tunnel.stream_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.stream_id, *pnread);
  }

  ফলাফল = Curl_1st_fatal(ফলাফল, proxy_h2_progress_egress(cf, তথ্য));

out:
  যদি((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!ফলাফল || (ফলাফল == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, তথ্য, &ctx->tunnel);
  }
  CURL_TRC_CF(তথ্য, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.stream_id, len, (পূর্ণসংখ্যা)ফলাফল, *pnread);
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h2_proxy_send(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 ধ্রুবক uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  CURLcode ফলাফল;

  (শূন্য)eos;
  *pnwritten = 0;
  CF_তথ্য_SAVE(save, cf, তথ্য);

  যদি(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    ফলাফল = CURLE_SEND_ERROR;
    যাও out;
  }

  যদি(ctx->tunnel.closed) {
    ফলাফল = CURLE_SEND_ERROR;
    যাও out;
  }

  ফলাফল = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(তথ্য, cf, "cf_send(), bufq_write %d, %zu", (পূর্ণসংখ্যা)ফলাফল,
              *pnwritten);
  যদি(ফলাফল && (ফলাফল != CURLE_AGAIN))
    যাও out;

  যদি(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_তথ্য(&ctx->ngh2, ctx->tunnel.stream_id))) {
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
  }

  ফলাফল = Curl_1st_fatal(ফলাফল, proxy_h2_progress_ingress(cf, তথ্য));
  ফলাফল = Curl_1st_fatal(ফলাফল, proxy_h2_progress_egress(cf, তথ্য));

  যদি(!ফলাফল && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    যদি(ctx->tunnel.closed) {
      ফলাফল = CURLE_SEND_ERROR;
    }
    অন্যথা {
      CURL_TRC_CF(তথ্য, cf, "[0] send: nothing to do in this session");
      ফলাফল = CURLE_HTTP2;
    }
  }

out:
  যদি((!Curl_bufq_is_empty(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) &&
     (!ফলাফল || (ফলাফল == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, তথ্য, &ctx->tunnel);
  }
  CURL_TRC_CF(তথ্য, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, len, (পূর্ণসংখ্যা)ফলাফল, *pnwritten,
              Curl_ngh2_session_get_stream_remote_window_আকার(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_আকার(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h2_proxy_flush(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  CURLcode ফলাফল = CURLE_OK;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  যদি(!Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_তথ্য(&ctx->ngh2, ctx->tunnel.stream_id))) {
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
  }

  ফলাফল = proxy_h2_progress_egress(cf, তথ্য);

out:
  CURL_TRC_CF(তথ্য, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.stream_id, (পূর্ণসংখ্যা)ফলাফল,
              Curl_ngh2_session_get_stream_remote_window_আকার(
                &ctx->ngh2, ctx->tunnel.stream_id),
              Curl_ngh2_session_get_remote_window_আকার(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির bool proxy_h2_connisalive(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 bool *প্রবেশ_pending)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *প্রবেশ_pending = FALSE;
  যদি(!cf->পরবর্তী || !cf->পরবর্তী->cft->is_alive(cf->পরবর্তী, তথ্য, প্রবেশ_pending))
    ফেরত FALSE;

  যদি(*প্রবেশ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode ফলাফল;
    size_t nread;

    *প্রবেশ_pending = FALSE;
    ফলাফল = Curl_cf_recv_bufq(cf->পরবর্তী, তথ্য, &ctx->inbufq, 0, &nread);
    যদি(!ফলাফল) {
      যদি(proxy_h2_process_pending_প্রবেশ(cf, তথ্য))
        /* immediate error, considered dead */
        alive = FALSE;
      অন্যথা {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    অন্যথা যদি(ফলাফল != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ফেরত alive;
}

স্থির bool cf_h2_proxy_is_alive(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 bool *প্রবেশ_pending)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  কাঠামো cf_call_তথ্য save;

  *প্রবেশ_pending = FALSE;
  CF_তথ্য_SAVE(save, cf, তথ্য);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, তথ্য, প্রবেশ_pending));
  CURL_TRC_CF(তথ্য, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *প্রবেশ_pending);
  CF_তথ্য_RESTORE(cf, save);
  ফেরত alive;
}

স্থির CURLcode cf_h2_proxy_query(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  কাঠামো cf_h2_proxy_ctx *ctx = cf->ctx;

  নির্বাচন(query) {
  ক্ষেত্র CF_QUERY_HOST_PORT:
    *pres1 = (পূর্ণসংখ্যা)ctx->dest->port;
    *((ধ্রুবক অক্ষর **)pres2) = ctx->dest->hostname;
    ফেরত CURLE_OK;
  ক্ষেত্র CF_QUERY_NEED_FLUSH: {
    যদি(!Curl_bufq_is_empty(&ctx->outbufq) ||
       !Curl_bufq_is_empty(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(তথ্য, cf, "needs flush");
      *pres1 = TRUE;
      ফেরত CURLE_OK;
    }
    বিরতি;
  }
  ক্ষেত্র CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুবক অক্ষর **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ফাঁকা;
    ফেরত CURLE_OK;
  }
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির CURLcode cf_h2_proxy_cntrl(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো cf_call_তথ্য save;

  (শূন্য)arg1;
  (শূন্য)arg2;

  নির্বাচন(event) {
  ক্ষেত্র CF_CTRL_FLUSH:
    CF_তথ্য_SAVE(save, cf, তথ্য);
    ফলাফল = cf_h2_proxy_flush(cf, তথ্য);
    CF_তথ্য_RESTORE(cf, save);
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত ফলাফল;
}

কাঠামো Curl_cftype Curl_cft_h2_proxy = {
  "H2-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  cf_h2_proxy_destroy,
  cf_h2_proxy_connect,
  cf_h2_proxy_shutdown,
  cf_h2_proxy_adjust_pollset,
  cf_h2_proxy_তথ্য_pending,
  cf_h2_proxy_send,
  cf_h2_proxy_recv,
  cf_h2_proxy_cntrl,
  cf_h2_proxy_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h2_proxy_query,
};

CURLcode Curl_cf_h2_proxy_ঢোকাও_after(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য,
                                       কাঠামো Curl_peer *peer,
                                       কাঠামো Curl_peer *dest,
                                       bool udp_tunnel)
{
  কাঠামো Curl_cfilter *cf_h2_proxy = ফাঁকা;
  কাঠামো cf_h2_proxy_ctx *ctx;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  (শূন্য)তথ্য;
  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx)
    যাও out;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  ফলাফল = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  যদি(ফলাফল)
    যাও out;
  ctx = ফাঁকা;
  Curl_conn_cf_ঢোকাও_after(cf, cf_h2_proxy);

out:
  cf_h2_proxy_ctx_free(ctx);
  ফেরত ফলাফল;
}

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
