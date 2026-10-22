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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_NGHTTP2)

#అన్తర్భావ <nghttp2/nghttp2.h>

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "bufq.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "dynhds.h"
#అన్తర్భావ "http2.h"
#అన్తర్భావ "http_proxy.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "ngh2_shim.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "cf-h2-proxy.h"

#నిర్వచించు PROXY_H2_CHUNK_కొలత  (16 * 1024)

#నిర్వచించు PROXY_HTTP2_HUGE_కిటికీ_కొలత (100 * 1024 * 1024)
#నిర్వచించు H2_TUNNEL_కిటికీ_కొలత        (10 * 1024 * 1024)

#నిర్వచించు PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_కిటికీ_కొలత / PROXY_H2_CHUNK_కొలత)
#నిర్వచించు PROXY_H2_NW_SEND_CHUNKS   1

#నిర్వచించు H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_కిటికీ_కొలత / PROXY_H2_CHUNK_కొలత)
#నిర్వచించు H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_కొలత)


రకనిర్వచనం గణన {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

నిర్మాణం tunnel_ప్రవాహం {
  నిర్మాణం http_resp *resp;
  నిర్మాణం bufq recvbuf;
  నిర్మాణం bufq sendbuf;
  అక్షరం *authority;
  int32_t ప్రవాహం_id;
  uint32_t error;
  h2_tunnel_state state;
  బిట్టు(has_final_response);
  బిట్టు(closed);
  బిట్టు(reset);
};

స్థిర CURLcode tunnel_ప్రవాహం_init(నిర్మాణం tunnel_ప్రవాహం *ts,
                                   నిర్మాణం Curl_peer *dest)
{
  ts->state = H2_TUNNEL_INIT;
  ts->ప్రవాహం_id = -1;
  Curl_bufq_init2(&ts->recvbuf, PROXY_H2_CHUNK_కొలత, H2_TUNNEL_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_హద్దు);
  Curl_bufq_init(&ts->sendbuf, PROXY_H2_CHUNK_కొలత, H2_TUNNEL_SEND_CHUNKS);

  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->పోర్టు);
  ఒకవేళ(!ts->authority)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  తిరిగి CURLE_OK;
}

స్థిర శూన్య tunnel_ప్రవాహం_reset(నిర్మాణం tunnel_ప్రవాహం *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = ఖాళీ;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->ప్రవాహం_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

స్థిర శూన్య tunnel_ప్రవాహం_clear(నిర్మాణం tunnel_ప్రవాహం *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  memset(ts, 0, పరిమాణం(*ts));
  ts->state = H2_TUNNEL_INIT;
}

స్థిర శూన్య h2_tunnel_go_state(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం tunnel_ప్రవాహం *ts,
                               h2_tunnel_state new_state,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               bool udp_tunnel)
{
  (శూన్య)cf;
  (శూన్య)udp_tunnel;

  ఒకవేళ(ts->state == new_state)
    తిరిగి;
  /* leaving this one */
  ఎంపిక(ts->state) {
  స్థితి H2_TUNNEL_CONNECT:
    దత్తాంశం->req.ignorebody = FALSE;
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  /* entering this one */
  ఎంపిక(new_state) {
  స్థితి H2_TUNNEL_INIT:
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] new tunnel state 'init'", ts->ప్రవాహం_id);
    tunnel_ప్రవాహం_reset(ts);
    ఆపు;

  స్థితి H2_TUNNEL_CONNECT:
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] new tunnel state 'connect'", ts->ప్రవాహం_id);
    ts->state = H2_TUNNEL_CONNECT;
    ఆపు;

  స్థితి H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] new tunnel state 'response'", ts->ప్రవాహం_id);
    ts->state = H2_TUNNEL_RESPONSE;
    ఆపు;

  స్థితి H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] new tunnel state 'established'",
                ts->ప్రవాహం_id);
    infof(దత్తాంశం, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    దత్తాంశం->state.authproxy.పూర్తి = TRUE;
    దత్తాంశం->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  స్థితి H2_TUNNEL_FAILED:
    ఒకవేళ(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(దత్తాంశం, cf, "[%d] new tunnel state 'failed'", ts->ప్రవాహం_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(దత్తాంశం->req.hd_proxy_auth);
    ఆపు;
  }
}

నిర్మాణం cf_h2_proxy_ctx {
  నిర్మాణం curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  నిర్మాణం cf_call_దత్తాంశం call_దత్తాంశం;

  నిర్మాణం bufq inbufq;  /* network receive buffer */
  నిర్మాణం bufq outbufq; /* network send buffer */

  నిర్మాణం Curl_peer *peer; /* proxy we talk to */
  నిర్మాణం Curl_peer *dest; /* where to tunnel to */
  నిర్మాణం tunnel_ప్రవాహం tunnel; /* our tunnel CONNECT stream */
  int32_t goaway_error;
  int32_t remote_max_sid;
  బిట్టు(conn_closed);
  బిట్టు(rcvd_goaway);
  బిట్టు(పంపిన_goaway);
  బిట్టు(nw_బహిర్గతం_blocked);
  బిట్టు(udp_tunnel);
};

/* How to access `call_data` from a cf_h2 filter */
#నిర్వచనం_రద్దు CF_CTX_CALL_దత్తాంశం
#నిర్వచించు CF_CTX_CALL_దత్తాంశం(cf) ((నిర్మాణం cf_h2_proxy_ctx *)(cf)->ctx)->call_దత్తాంశం

స్థిర శూన్య cf_h2_proxy_ctx_clear(నిర్మాణం cf_h2_proxy_ctx *ctx)
{
  నిర్మాణం cf_call_దత్తాంశం save = ctx->call_దత్తాంశం;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_ప్రవాహం_clear(&ctx->tunnel);
  memset(ctx, 0, పరిమాణం(*ctx));
  ctx->call_దత్తాంశం = save;
}

స్థిర శూన్య cf_h2_proxy_ctx_free(నిర్మాణం cf_h2_proxy_ctx *ctx)
{
  ఒకవేళ(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

స్థిర శూన్య drain_tunnel(నిర్మాణం Curl_cfilter *cf,
                         నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం tunnel_ప్రవాహం *tunnel)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  (శూన్య)cf;
  ఒకవేళ(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_ఖాళీలు(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_ఖాళీలు(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(దత్తాంశం);
}

స్థిర CURLcode proxy_h2_nw_బహిర్గతం_writer(శూన్య *writer_ctx,
                                       స్థిరరాశి uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  నిర్మాణం Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  ఒకవేళ(cf) {
    నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
    CURLcode ఫలితం;
    ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(దత్తాంశం, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (పూర్ణాంకం)ఫలితం, *pnwritten);
    తిరిగి ఫలితం;
  }
  తిరిగి CURLE_FAILED_INIT;
}

స్థిర పూర్ణాంకం proxy_h2_client_new(నిర్మాణం Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ఖాళీ, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  పూర్ణాంకం rc = nghttp2_option_new(&o);
  ఒకవేళ(rc)
    తిరిగి rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_సమితి_no_auto_కిటికీ_update(o, 1);
#ఒకవేళ NGHTTP2_సంస్కరణ_అంకె >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_సమితి_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#అయితే_ముగింపు
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  తిరిగి rc;
}

స్థిర పూర్ణాంకం proxy_h2_should_close_session(నిర్మాణం cf_h2_proxy_ctx *ctx)
{
  తిరిగి !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

స్థిర CURLcode proxy_h2_nw_బహిర్గతం_flush(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode ఫలితం;

  ఒకవేళ(Curl_bufq_is_ఖాళీలు(&ctx->outbufq))
    తిరిగి CURLE_OK;

  ఫలితం = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_బహిర్గతం_writer, cf,
                          &nwritten);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      CURL_TRC_CF(దత్తాంశం, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_బహిర్గతం_blocked = 1;
    }
    తిరిగి ఫలితం;
  }
  CURL_TRC_CF(దత్తాంశం, cf, "[0] nw send buffer flushed");
  తిరిగి Curl_bufq_is_ఖాళీలు(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
స్థిర CURLcode proxy_h2_process_pending_ఆదానం(నిర్మాణం Curl_cfilter *cf,
                                               నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  స్థిరరాశి గుర్తులేని అక్షరం *buf;
  size_t blen, nread;
  ssize_t rv;

  అయితే(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (స్థిరరాశి uint8_t *)buf, blen);
    CURL_TRC_CF(దత్తాంశం, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    ఒకవేళ(!curlx_sztouz(rv, &nread)) {
      failf(దత్తాంశం,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((పూర్ణాంకం)rv));
      తిరిగి CURLE_RECV_ERROR;
    }
    లేకపోతే ఒకవేళ(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      ఆపు;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    ఒకవేళ(Curl_bufq_is_ఖాళీలు(&ctx->inbufq)) {
      CURL_TRC_CF(దత్తాంశం, cf, "[0] all data in connection buffer processed");
      ఆపు;
    }
    లేకపోతే {
      CURL_TRC_CF(దత్తాంశం, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode proxy_h2_పురోగతి_ingress(నిర్మాణం Curl_cfilter *cf,
                                          నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->inbufq)) {
    CURL_TRC_CF(దత్తాంశం, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    ఫలితం = proxy_h2_process_pending_ఆదానం(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  అయితే(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_ఖాళీలు(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_నిండిన(&ctx->tunnel.recvbuf)) {

    ఫలితం = Curl_cf_recv_bufq(cf->తదుపరి, దత్తాంశం, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(దత్తాంశం, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (పూర్ణాంకం)ఫలితం, nread);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం != CURLE_AGAIN) {
        failf(దత్తాంశం, "Failed receiving HTTP2 proxy data");
        తిరిగి ఫలితం;
      }
      ఆపు;
    }
    లేకపోతే ఒకవేళ(nread == 0) {
      CURL_TRC_CF(దత్తాంశం, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      ఆపు;
    }

    ఫలితం = proxy_h2_process_pending_ఆదానం(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode proxy_h2_పురోగతి_egress(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  పూర్ణాంకం rv = 0;

  ctx->nw_బహిర్గతం_blocked = 0;
  అయితే(!rv && !ctx->nw_బహిర్గతం_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  ఒకవేళ(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(దత్తాంశం, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    తిరిగి CURLE_SEND_ERROR;
  }
  తిరిగి proxy_h2_nw_బహిర్గతం_flush(cf, దత్తాంశం);
}

స్థిర ssize_t on_session_send(nghttp2_session *h2,
                               స్థిరరాశి uint8_t *buf, size_t blen, పూర్ణాంకం flags,
                               శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  size_t nwritten;
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)h2;
  (శూన్య)flags;
  DEBUGASSERT(దత్తాంశం);

  ఫలితం = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_బహిర్గతం_writer, cf, &nwritten);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      ctx->nw_బహిర్గతం_blocked = 1;
      తిరిగి NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(దత్తాంశం, "Failed sending HTTP2 data");
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ఒకవేళ(!nwritten)
    తిరిగి NGHTTP2_ERR_WOULDBLOCK;

  తిరిగి (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#నిర్వచితమైతే CURLVERBOSE
స్థిర పూర్ణాంకం proxy_h2_on_చట్రం_send(nghttp2_session *session,
                                  స్థిరరాశి nghttp2_చట్రం *చట్రం,
                                  శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);

  (శూన్య)session;
  DEBUGASSERT(దత్తాంశం);
  ఒకవేళ(Curl_trc_cf_is_verbose(cf, దత్తాంశం)) {
    అక్షరం బఫరు[256];
    పూర్ణాంకం len;
    len = Curl_nghttp2_fr_print(చట్రం, బఫరు, పరిమాణం(బఫరు) - 1);
    బఫరు[len] = 0;
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] -> %s", చట్రం->hd.ప్రవాహం_id, బఫరు);
  }
  తిరిగి 0;
}
#అయితే_ముగింపు /* CURLVERBOSE */

స్థిర పూర్ణాంకం proxy_h2_on_చట్రం_recv(nghttp2_session *session,
                                  స్థిరరాశి nghttp2_చట్రం *చట్రం,
                                  శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  int32_t ప్రవాహం_id = చట్రం->hd.ప్రవాహం_id;

  (శూన్య)session;
  DEBUGASSERT(దత్తాంశం);
#నిర్వచితమైతే CURLVERBOSE
  ఒకవేళ(Curl_trc_cf_is_verbose(cf, దత్తాంశం)) {
    అక్షరం బఫరు[256];
    పూర్ణాంకం len;
    len = Curl_nghttp2_fr_print(చట్రం, బఫరు, పరిమాణం(బఫరు) - 1);
    బఫరు[len] = 0;
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] <- %s", చట్రం->hd.ప్రవాహం_id, బఫరు);
  }
#అయితే_ముగింపు /* CURLVERBOSE */

  ఒకవేళ(!ప్రవాహం_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(దత్తాంశం);
    ఎంపిక(చట్రం->hd.type) {
    స్థితి NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      ఒకవేళ(CURL_REQ_WANT_SEND(దత్తాంశం)) {
        drain_tunnel(cf, దత్తాంశం, &ctx->tunnel);
      }
      ఆపు;
    స్థితి NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = చట్రం->goaway.చివరి_ప్రవాహం_id;
      ఒకవేళ(దత్తాంశం) {
        infof(దత్తాంశం, "received GOAWAY, error=%u, last_stream=%d",
              చట్రం->goaway.error_సంకేతం, ctx->remote_max_sid);
      }
      ఆపు;
    అప్రమేయ:
      ఆపు;
    }
    తిరిగి 0;
  }

  ఒకవేళ(ప్రవాహం_id != ctx->tunnel.ప్రవాహం_id) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] rcvd FRAME not for tunnel", ప్రవాహం_id);
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ఎంపిక(చట్రం->hd.type) {
  స్థితి NGHTTP2_శీర్షికలు:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    ఒకవేళ(!ctx->tunnel.resp)
      తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] got http status: %d",
                ప్రవాహం_id, ctx->tunnel.resp->స్థితి_2);
    ఒకవేళ(!ctx->tunnel.has_final_response && ctx->tunnel.resp->స్థితి_2 / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    ఆపు;
  స్థితి NGHTTP2_కిటికీ_UPDATE:
    ఒకవేళ(CURL_REQ_WANT_SEND(దత్తాంశం)) {
      drain_tunnel(cf, దత్తాంశం, &ctx->tunnel);
    }
    ఆపు;
  స్థితి NGHTTP2_RST_ప్రవాహం:
    ఒకవేళ(చట్రం->rst_ప్రవాహం.error_సంకేతం)
      ctx->tunnel.reset = TRUE;
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి 0;
}

స్థిర పూర్ణాంకం proxy_h2_on_శీర్షిక(nghttp2_session *session,
                              స్థిరరాశి nghttp2_చట్రం *చట్రం,
                              స్థిరరాశి uint8_t *name, size_t namelen,
                              స్థిరరాశి uint8_t *విలువ, size_t valuelen,
                              uint8_t flags,
                              శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  int32_t ప్రవాహం_id = చట్రం->hd.ప్రవాహం_id;
  CURLcode ఫలితం;

  (శూన్య)flags;
  (శూన్య)session;
  DEBUGASSERT(ప్రవాహం_id); /* should never be a zero stream ID here */
  ఒకవేళ(ప్రవాహం_id != ctx->tunnel.ప్రవాహం_id) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", ప్రవాహం_id,
                (పూర్ణాంకం)namelen, name, (పూర్ణాంకం)valuelen, విలువ);
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ఒకవేళ(చట్రం->hd.type == NGHTTP2_నెట్టు_PROMISE)
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;

  ఒకవేళ(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    తిరిగి 0;
  }

  ఒకవేళ(namelen == CURL_CSTRLEN(HTTP_PSEUDO_స్థితి) &&
     !memcmp(HTTP_PSEUDO_స్థితి, name, namelen)) {
    int32_t http_స్థితి;
    నిర్మాణం http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    ఫలితం = Curl_http_decode_స్థితి(&http_స్థితి,
                                    (స్థిరరాశి అక్షరం *)విలువ, valuelen);
    ఒకవేళ(ఫలితం)
      తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
    ఫలితం = Curl_http_resp_make(&resp, http_స్థితి, ఖాళీ);
    ఒకవేళ(ఫలితం)
      తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
    ఒకవేళ(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] status: HTTP/2 %03d",
                ప్రవాహం_id, ctx->tunnel.resp->స్థితి_2);
    తిరిగి 0;
  }

  ఒకవేళ(!ctx->tunnel.resp)
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;

  ఫలితం = Curl_dynhds_కూడు(&ctx->tunnel.resp->శీర్షికలు,
                           (స్థిరరాశి అక్షరం *)name, namelen,
                           (స్థిరరాశి అక్షరం *)విలువ, valuelen);
  ఒకవేళ(ఫలితం)
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(దత్తాంశం, cf, "[%d] header: %.*s: %.*s",
              ప్రవాహం_id, (పూర్ణాంకం)namelen, name, (పూర్ణాంకం)valuelen, విలువ);

  తిరిగి 0; /* 0 is successful */
}

స్థిర ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t ప్రవాహం_id,
                                    uint8_t *buf, size_t నిడివి,
                                    uint32_t *దత్తాంశం_flags,
                                    nghttp2_దత్తాంశం_ఆధారం *ఆధారం,
                                    శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  నిర్మాణం tunnel_ప్రవాహం *ts;
  CURLcode ఫలితం;
  size_t nread;

  (శూన్య)session;
  (శూన్య)ఆధారం;
  (శూన్య)ctx;

  ఒకవేళ(!ప్రవాహం_id)
    తిరిగి NGHTTP2_ERR_INVALID_వాదం;

  ts = Curl_ngh2_session_get_ప్రవాహం_user_దత్తాంశం(&ctx->ngh2, ప్రవాహం_id);
  ఒకవేళ(!ts)
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  ఫలితం = Curl_bufq_read(&ts->sendbuf, buf, నిడివి, &nread);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం != CURLE_AGAIN)
      తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
    తిరిగి NGHTTP2_ERR_DEFERRED;
  }
  ఒకవేళ(ts->closed && Curl_bufq_is_ఖాళీలు(&ts->sendbuf))
    *దత్తాంశం_flags = NGHTTP2_దత్తాంశం_FLAG_EOF;

  CURL_TRC_CF(దత్తాంశం, cf, "[%d] tunnel_send_callback -> %zu",
              ts->ప్రవాహం_id, nread);
  తిరిగి (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

స్థిర పూర్ణాంకం tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t ప్రవాహం_id,
                                స్థిరరాశి uint8_t *mem, size_t len, శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode ఫలితం;

  (శూన్య)flags;
  (శూన్య)session;
  DEBUGASSERT(ప్రవాహం_id); /* should never be a zero stream ID here */

  ఒకవేళ(ప్రవాహం_id != ctx->tunnel.ప్రవాహం_id)
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;

  ఫలితం = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం != CURLE_AGAIN)
      తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
#నిర్వచితమైతే DEBUGBUILD
    nwritten = 0;
#అయితే_ముగింపు
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  తిరిగి 0;
}

స్థిర పూర్ణాంకం proxy_h2_on_ప్రవాహం_close(nghttp2_session *session,
                                    int32_t ప్రవాహం_id,
                                    uint32_t error_సంకేతం, శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);

  (శూన్య)session;

  ఒకవేళ(ప్రవాహం_id != ctx->tunnel.ప్రవాహం_id)
    తిరిగి 0;

  CURL_TRC_CF(దత్తాంశం, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              ప్రవాహం_id, nghttp2_http2_strerror(error_సంకేతం), error_సంకేతం);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_సంకేతం;
  ఒకవేళ(error_సంకేతం)
    ctx->tunnel.reset = TRUE;

  తిరిగి 0;
}

స్థిర CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  నిర్మాణం Curl_cfilter *cf,
  నిర్మాణం Curl_easy *దత్తాంశం,
  నిర్మాణం curl_ngh2 *ngh2,
  నిర్మాణం httpreq *req,
  స్థిరరాశి nghttp2_ప్రాధాన్యత_spec *pri_spec,
  శూన్య *ప్రవాహం_user_దత్తాంశం,
  nghttp2_దత్తాంశం_ఆధారం_read_callback read_callback,
  శూన్య *read_ctx)
{
  నిర్మాణం dynhds h2_శీర్షికలు;
  nghttp2_nv *nva = ఖాళీ;
  int32_t ప్రవాహం_id = -1;
  size_t nheader;
  CURLcode ఫలితం;

  (శూన్య)cf;
  Curl_dynhds_init(&h2_శీర్షికలు, MAX_HTTP_RESP_శీర్షిక_లెక్క, DYN_HTTP_REQUEST);
  ఫలితం = Curl_http_req_గమ్యం_h2(&h2_శీర్షికలు, req, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  nva = Curl_dynhds_గమ్యం_nva(&h2_శీర్షికలు, &nheader);
  ఒకవేళ(!nva) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(read_callback) {
    nghttp2_దత్తాంశం_provider దత్తాంశం_prd;

    దత్తాంశం_prd.read_callback = read_callback;
    దత్తాంశం_prd.ఆధారం.ptr = read_ctx;
    ప్రవాహం_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &దత్తాంశం_prd, ప్రవాహం_user_దత్తాంశం);
  }
  లేకపోతే {
    ప్రవాహం_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, ఖాళీ, ప్రవాహం_user_దత్తాంశం);
  }

  ఒకవేళ(ప్రవాహం_id < 0) {
    failf(దత్తాంశం, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(ప్రవాహం_id), ప్రవాహం_id);
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు బహిర్గతం;
  }
  ఫలితం = CURLE_OK;

బహిర్గతం:
  curlx_free(nva);
  Curl_dynhds_free(&h2_శీర్షికలు);
  *pstream_id = ప్రవాహం_id;
  తిరిగి ఫలితం;
}

స్థిర CURLcode submit_CONNECT(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం tunnel_ప్రవాహం *ts)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం;
  నిర్మాణం httpreq *req = ఖాళీ;

  ఫలితం = Curl_http_proxy_create_tunnel_request(&req, cf, దత్తాంశం,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ఫలితం = Curl_creader_సమితి_null(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = proxy_h2_submit(&ts->ప్రవాహం_id, cf, దత్తాంశం, &ctx->ngh2, req,
                           ఖాళీ, ts, tunnel_send_callback, cf);
  ఒకవేళ(ఫలితం) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->ప్రవాహం_id, nghttp2_strerror(ts->ప్రవాహం_id));
  }

బహిర్గతం:
  ఒకవేళ(req)
    Curl_http_req_free(req);
  ఒకవేళ(ఫలితం)
    failf(దత్తాంశం, "Failed sending CONNECT to proxy");
  తిరిగి ఫలితం;
}

స్థిర CURLcode inspect_response(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం tunnel_ప్రవాహం *ts)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_ఫలితం res;
  CURLcode ఫలితం;

  ఫలితం = Curl_http_proxy_inspect_tunnel_response(
      cf, దత్తాంశం, ts->resp, (bool)ctx->udp_tunnel, &res);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  ఎంపిక(res) {
  స్థితి PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, దత్తాంశం,
                       (bool)ctx->udp_tunnel);
    ఆపు;
  స్థితి PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, దత్తాంశం,
                       (bool)ctx->udp_tunnel);
    ఫలితం = CURLE_COULDNT_CONNECT;
    ఆపు;
  స్థితి PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, దత్తాంశం,
                       (bool)ctx->udp_tunnel);
    ఆపు;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode H2_CONNECT(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం tunnel_ప్రవాహం *ts)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  ఒకవేళ(ctx->conn_closed) {
    failf(దత్తాంశం, "proxy closed connection");
    తిరిగి CURLE_COULDNT_CONNECT;
  }

  చేయి {
    ఎంపిక(ts->state) {
    స్థితి H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(దత్తాంశం, cf, "[0] CONNECT start for %s", ts->authority);
      ఫలితం = submit_CONNECT(cf, దత్తాంశం, ts);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, దత్తాంశం,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    స్థితి H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      ఫలితం = proxy_h2_పురోగతి_ingress(cf, దత్తాంశం);
      ఒకవేళ(!ఫలితం)
        ఫలితం = proxy_h2_పురోగతి_egress(cf, దత్తాంశం);
      ఒకవేళ(ఫలితం && ఫలితం != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, దత్తాంశం,
                           (bool)ctx->udp_tunnel);
        ఆపు;
      }

      ఒకవేళ(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, దత్తాంశం,
                           (bool)ctx->udp_tunnel);
      }
      లేకపోతే {
        ఫలితం = CURLE_OK;
        వెళ్ళు బహిర్గతం;
      }
      FALLTHROUGH();

    స్థితి H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      ఫలితం = inspect_response(cf, దత్తాంశం, ts);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      ఆపు;

    స్థితి H2_TUNNEL_ESTABLISHED:
      తిరిగి CURLE_OK;

    స్థితి H2_TUNNEL_FAILED:
      తిరిగి CURLE_RECV_ERROR;

    అప్రమేయ:
      ఆపు;
    }

  } అయితే(ts->state == H2_TUNNEL_INIT);

బహిర్గతం:
  ఒకవేళ((ఫలితం && (ఫలితం != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, దత్తాంశం,
                       (bool)ctx->udp_tunnel);
  తిరిగి ఫలితం;
}

/*
 * Initialize the cfilter context
 */
స్థిర CURLcode cf_h2_proxy_ctx_init(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  nghttp2_session_callbacks *cbs = ఖాళీ;
  పూర్ణాంకం rc;

  DEBUGASSERT(!ctx->ngh2.s);
  memset(&ctx->tunnel, 0, పరిమాణం(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_కొలత, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_కొలత, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  ఒకవేళ(tunnel_ప్రవాహం_init(&ctx->tunnel, ctx->dest))
    వెళ్ళు బహిర్గతం;

  rc = nghttp2_session_callbacks_new(&cbs);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "Could not initialize nghttp2 callbacks");
    వెళ్ళు బహిర్గతం;
  }

  nghttp2_session_callbacks_సమితి_send_callback(cbs, on_session_send);
  nghttp2_session_callbacks_సమితి_on_చట్రం_recv_callback(
    cbs, proxy_h2_on_చట్రం_recv);
#నిర్వచితమైతే CURLVERBOSE
  nghttp2_session_callbacks_సమితి_on_చట్రం_send_callback(cbs,
                                                       proxy_h2_on_చట్రం_send);
#అయితే_ముగింపు
  nghttp2_session_callbacks_సమితి_on_దత్తాంశం_chunk_recv_callback(
    cbs, tunnel_recv_callback);
  nghttp2_session_callbacks_సమితి_on_ప్రవాహం_close_callback(
    cbs, proxy_h2_on_ప్రవాహం_close);
  nghttp2_session_callbacks_సమితి_on_శీర్షిక_callback(cbs, proxy_h2_on_శీర్షిక);

  /* The nghttp2 session is not yet setup, do it */
  rc = proxy_h2_client_new(cf, cbs);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "Could not initialize nghttp2");
    వెళ్ళు బహిర్గతం;
  }

  {
    nghttp2_settings_నమోదు iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].విలువ = Curl_multi_max_concurrent_streams(దత్తాంశం->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_కిటికీ_కొలత;
    iv[1].విలువ = H2_TUNNEL_కిటికీ_కొలత;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_నెట్టు;
    iv[2].విలువ = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    ఒకవేళ(rc) {
      failf(దత్తాంశం, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      ఫలితం = CURLE_HTTP2;
      వెళ్ళు బహిర్గతం;
    }
  }

  rc = Curl_ngh2_session_సమితి_local_కిటికీ_కొలత(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_కిటికీ_కొలత);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ఫలితం = CURLE_HTTP2;
    వెళ్ళు బహిర్గతం;
  }

  /* all set, traffic will be send on connect */
  ఫలితం = CURLE_OK;

బహిర్గతం:
  ఒకవేళ(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(దత్తాంశం, cf, "[0] init proxy ctx -> %d", (పూర్ణాంకం)ఫలితం);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h2_proxy_connect(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    bool *పూర్తి)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం cf_call_దత్తాంశం save;
  నిర్మాణం tunnel_ప్రవాహం *ts = &ctx->tunnel;

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  /* Connect the lower filters first */
  ఒకవేళ(!cf->తదుపరి->connected) {
    ఫలితం = Curl_conn_cf_connect(cf->తదుపరి, దత్తాంశం, పూర్తి);
    ఒకవేళ(ఫలితం || !*పూర్తి)
      తిరిగి ఫలితం;
  }

  *పూర్తి = FALSE;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  ఒకవేళ(!ctx->ngh2.s) {
    ఫలితం = cf_h2_proxy_ctx_init(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  DEBUGASSERT(ts->authority);

  ఒకవేళ(Curl_timeleft_ms(దత్తాంశం) < 0) {
    failf(దత్తాంశం, "Proxy CONNECT aborted due to timeout");
    ఫలితం = CURLE_OPERATION_TIMEDOUT;
    వెళ్ళు బహిర్గతం;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  ఫలితం = H2_CONNECT(cf, దత్తాంశం, ts);

బహిర్గతం:
  *పూర్తి = (ఫలితం == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  ఒకవేళ(*పూర్తి) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&దత్తాంశం->req, దత్తాంశం);
    Curl_client_reset(దత్తాంశం);
  }
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_h2_proxy_destroy(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;

  (శూన్య)దత్తాంశం;
  ఒకవేళ(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = ఖాళీ;
  }
}

స్థిర CURLcode cf_h2_proxy_shutdown(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  CURLcode ఫలితం;
  పూర్ణాంకం rv;

  ఒకవేళ(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);

  ఒకవేళ(!ctx->పంపిన_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (స్థిరరాశి uint8_t *)"shutdown", పరిమాణం("shutdown"));
    ఒకవేళ(rv) {
      failf(దత్తాంశం, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    ctx->పంపిన_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  ఫలితం = CURLE_OK;
  ఒకవేళ(Curl_ngh2_session_want_write(&ctx->ngh2))
    ఫలితం = proxy_h2_పురోగతి_egress(cf, దత్తాంశం);
  ఒకవేళ(!ఫలితం && Curl_ngh2_session_want_read(&ctx->ngh2))
    ఫలితం = proxy_h2_పురోగతి_ingress(cf, దత్తాంశం);

  *పూర్తి = (ctx->conn_closed ||
           (!ఫలితం && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
బహిర్గతం:
  CF_దత్తాంశం_RESTORE(cf, save);
  cf->shutdown = (ఫలితం || *పూర్తి);
  తిరిగి ఫలితం;
}

స్థిర bool cf_h2_proxy_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                                     స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  ఒకవేళ((ctx && !Curl_bufq_is_ఖాళీలు(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_ఖాళీలు(&ctx->tunnel.recvbuf)))
    తిరిగి TRUE;
  తిరిగి cf->తదుపరి ? cf->తదుపరి->cft->has_దత్తాంశం_pending(cf->తదుపరి, దత్తాంశం) : FALSE;
}

స్థిర CURLcode cf_h2_proxy_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                           నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, దత్తాంశం);
  bool want_recv, want_send;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_ఖాళీలు(&ctx->outbufq) ||
                !Curl_bufq_is_ఖాళీలు(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  లేకపోతే
    Curl_pollset_check(దత్తాంశం, ps, sock, &want_recv, &want_send);

  ఒకవేళ(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
    bool c_exhaust, s_exhaust;

    CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
    c_exhaust = !Curl_ngh2_session_get_remote_కిటికీ_కొలత(&ctx->ngh2);
    s_exhaust = ctx->tunnel.ప్రవాహం_id >= 0 &&
                !Curl_ngh2_session_get_ప్రవాహం_remote_కిటికీ_కొలత(
                  &ctx->ngh2, ctx->tunnel.ప్రవాహం_id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_ఖాళీలు(&ctx->outbufq) ||
                !Curl_bufq_is_ఖాళీలు(&ctx->tunnel.sendbuf);

    ఫలితం = Curl_pollset_సమితి(దత్తాంశం, ps, sock, want_recv, want_send);
    CURL_TRC_CF(దత్తాంశం, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (పూర్ణాంకం)ఫలితం);
    CF_దత్తాంశం_RESTORE(cf, save);
  }
  లేకపోతే ఒకవేళ(ctx->పంపిన_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_ఖాళీలు(&ctx->outbufq) ||
                !Curl_bufq_is_ఖాళీలు(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    ఫలితం = Curl_pollset_సమితి(దత్తాంశం, ps, sock, want_recv, want_send);
    CURL_TRC_CF(దత్తాంశం, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (పూర్ణాంకం)ఫలితం);
    CF_దత్తాంశం_RESTORE(cf, save);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode h2_handle_tunnel_close(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       size_t *pnread)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  ఒకవేళ(ctx->tunnel.error) {
    failf(దత్తాంశం, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.ప్రవాహం_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    తిరిగి CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(దత్తాంశం, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.ప్రవాహం_id);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode tunnel_recv(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                            అక్షరం *buf, size_t len, size_t *pnread)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_AGAIN;

  *pnread = 0;
  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->tunnel.recvbuf))
    ఫలితం = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  లేకపోతే {
    ఒకవేళ(ctx->tunnel.closed) {
      ఫలితం = h2_handle_tunnel_close(cf, దత్తాంశం, pnread);
    }
    లేకపోతే ఒకవేళ(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_ఖాళీలు(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.ప్రవాహం_id)) {
      ఫలితం = CURLE_RECV_ERROR;
    }
    లేకపోతే
      ఫలితం = CURLE_AGAIN;
  }

  CURL_TRC_CF(దత్తాంశం, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.ప్రవాహం_id, len, (పూర్ణాంకం)ఫలితం, *pnread);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h2_proxy_recv(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 అక్షరం *buf, size_t len,
                                 size_t *pnread)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  CURLcode ఫలితం;

  *pnread = 0;
  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);

  ఒకవేళ(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    ఫలితం = CURLE_RECV_ERROR;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(Curl_bufq_is_ఖాళీలు(&ctx->tunnel.recvbuf)) {
    ఫలితం = proxy_h2_పురోగతి_ingress(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఫలితం = tunnel_recv(cf, దత్తాంశం, buf, len, pnread);

  ఒకవేళ(!ఫలితం) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] increase window by %zu",
                ctx->tunnel.ప్రవాహం_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.ప్రవాహం_id, *pnread);
  }

  ఫలితం = Curl_1st_fatal(ఫలితం, proxy_h2_పురోగతి_egress(cf, దత్తాంశం));

బహిర్గతం:
  ఒకవేళ((!Curl_bufq_is_ఖాళీలు(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_ఖాళీలు(&ctx->tunnel.sendbuf)) &&
     (!ఫలితం || (ఫలితం == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, దత్తాంశం, &ctx->tunnel);
  }
  CURL_TRC_CF(దత్తాంశం, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.ప్రవాహం_id, len, (పూర్ణాంకం)ఫలితం, *pnread);
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h2_proxy_send(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  CURLcode ఫలితం;

  (శూన్య)eos;
  *pnwritten = 0;
  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);

  ఒకవేళ(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(ctx->tunnel.closed) {
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(దత్తాంశం, cf, "cf_send(), bufq_write %d, %zu", (పూర్ణాంకం)ఫలితం,
              *pnwritten);
  ఒకవేళ(ఫలితం && (ఫలితం != CURLE_AGAIN))
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    ఒకవేళ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_దత్తాంశం(&ctx->ngh2, ctx->tunnel.ప్రవాహం_id))) {
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఫలితం = Curl_1st_fatal(ఫలితం, proxy_h2_పురోగతి_ingress(cf, దత్తాంశం));
  ఫలితం = Curl_1st_fatal(ఫలితం, proxy_h2_పురోగతి_egress(cf, దత్తాంశం));

  ఒకవేళ(!ఫలితం && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    ఒకవేళ(ctx->tunnel.closed) {
      ఫలితం = CURLE_SEND_ERROR;
    }
    లేకపోతే {
      CURL_TRC_CF(దత్తాంశం, cf, "[0] send: nothing to do in this session");
      ఫలితం = CURLE_HTTP2;
    }
  }

బహిర్గతం:
  ఒకవేళ((!Curl_bufq_is_ఖాళీలు(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_ఖాళీలు(&ctx->tunnel.sendbuf)) &&
     (!ఫలితం || (ఫలితం == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, దత్తాంశం, &ctx->tunnel);
  }
  CURL_TRC_CF(దత్తాంశం, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.ప్రవాహం_id, len, (పూర్ణాంకం)ఫలితం, *pnwritten,
              Curl_ngh2_session_get_ప్రవాహం_remote_కిటికీ_కొలత(
                &ctx->ngh2, ctx->tunnel.ప్రవాహం_id),
              Curl_ngh2_session_get_remote_కిటికీ_కొలత(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h2_proxy_flush(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  CURLcode ఫలితం = CURLE_OK;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    ఒకవేళ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_దత్తాంశం(&ctx->ngh2, ctx->tunnel.ప్రవాహం_id))) {
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఫలితం = proxy_h2_పురోగతి_egress(cf, దత్తాంశం);

బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.ప్రవాహం_id, (పూర్ణాంకం)ఫలితం,
              Curl_ngh2_session_get_ప్రవాహం_remote_కిటికీ_కొలత(
                &ctx->ngh2, ctx->tunnel.ప్రవాహం_id),
              Curl_ngh2_session_get_remote_కిటికీ_కొలత(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర bool proxy_h2_connisalive(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 bool *ఆదానం_pending)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ఆదానం_pending = FALSE;
  ఒకవేళ(!cf->తదుపరి || !cf->తదుపరి->cft->is_alive(cf->తదుపరి, దత్తాంశం, ఆదానం_pending))
    తిరిగి FALSE;

  ఒకవేళ(*ఆదానం_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode ఫలితం;
    size_t nread;

    *ఆదానం_pending = FALSE;
    ఫలితం = Curl_cf_recv_bufq(cf->తదుపరి, దత్తాంశం, &ctx->inbufq, 0, &nread);
    ఒకవేళ(!ఫలితం) {
      ఒకవేళ(proxy_h2_process_pending_ఆదానం(cf, దత్తాంశం))
        /* immediate error, considered dead */
        alive = FALSE;
      లేకపోతే {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    లేకపోతే ఒకవేళ(ఫలితం != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  తిరిగి alive;
}

స్థిర bool cf_h2_proxy_is_alive(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 bool *ఆదానం_pending)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  నిర్మాణం cf_call_దత్తాంశం save;

  *ఆదానం_pending = FALSE;
  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, దత్తాంశం, ఆదానం_pending));
  CURL_TRC_CF(దత్తాంశం, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *ఆదానం_pending);
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి alive;
}

స్థిర CURLcode cf_h2_proxy_query(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  నిర్మాణం cf_h2_proxy_ctx *ctx = cf->ctx;

  ఎంపిక(query) {
  స్థితి CF_QUERY_HOST_పోర్టు:
    *pres1 = (పూర్ణాంకం)ctx->dest->పోర్టు;
    *((స్థిరరాశి అక్షరం **)pres2) = ctx->dest->hostname;
    తిరిగి CURLE_OK;
  స్థితి CF_QUERY_NEED_FLUSH: {
    ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->outbufq) ||
       !Curl_bufq_is_ఖాళీలు(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(దత్తాంశం, cf, "needs flush");
      *pres1 = TRUE;
      తిరిగి CURLE_OK;
    }
    ఆపు;
  }
  స్థితి CF_QUERY_ALPN_NEGOTIATED: {
    స్థిరరాశి అక్షరం **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = ఖాళీ;
    తిరిగి CURLE_OK;
  }
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->query(cf->తదుపరి, దత్తాంశం, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

స్థిర CURLcode cf_h2_proxy_cntrl(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం cf_call_దత్తాంశం save;

  (శూన్య)arg1;
  (శూన్య)arg2;

  ఎంపిక(event) {
  స్థితి CF_CTRL_FLUSH:
    CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
    ఫలితం = cf_h2_proxy_flush(cf, దత్తాంశం);
    CF_దత్తాంశం_RESTORE(cf, save);
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి ఫలితం;
}

నిర్మాణం Curl_cftype Curl_cft_h2_proxy = {
  "H2-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  cf_h2_proxy_destroy,
  cf_h2_proxy_connect,
  cf_h2_proxy_shutdown,
  cf_h2_proxy_adjust_pollset,
  cf_h2_proxy_దత్తాంశం_pending,
  cf_h2_proxy_send,
  cf_h2_proxy_recv,
  cf_h2_proxy_cntrl,
  cf_h2_proxy_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h2_proxy_query,
};

CURLcode Curl_cf_h2_proxy_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం Curl_peer *peer,
                                       నిర్మాణం Curl_peer *dest,
                                       bool udp_tunnel)
{
  నిర్మాణం Curl_cfilter *cf_h2_proxy = ఖాళీ;
  నిర్మాణం cf_h2_proxy_ctx *ctx;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  (శూన్య)దత్తాంశం;
  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx)
    వెళ్ళు బహిర్గతం;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  ఫలితం = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ctx = ఖాళీ;
  Curl_conn_cf_చొప్పించు_after(cf, cf_h2_proxy);

బహిర్గతం:
  cf_h2_proxy_ctx_free(ctx);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
