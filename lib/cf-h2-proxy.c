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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_PROXY) && \
  defined(USE_NGHTTP2)

#अन्तर्भाव <nghttp2/nghttp2.h>

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "bufq.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "dynhds.h"
#अन्तर्भाव "http2.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "ngh2_shim.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "cf-h2-proxy.h"

#परिभाषय PROXY_H2_CHUNK_आकार  (16 * 1024)

#परिभाषय PROXY_HTTP2_HUGE_गवाक्षः_आकार (100 * 1024 * 1024)
#परिभाषय H2_TUNNEL_गवाक्षः_आकार        (10 * 1024 * 1024)

#परिभाषय PROXY_H2_NW_RECV_CHUNKS   (H2_TUNNEL_गवाक्षः_आकार / PROXY_H2_CHUNK_आकार)
#परिभाषय PROXY_H2_NW_SEND_CHUNKS   1

#परिभाषय H2_TUNNEL_RECV_CHUNKS   (H2_TUNNEL_गवाक्षः_आकार / PROXY_H2_CHUNK_आकार)
#परिभाषय H2_TUNNEL_SEND_CHUNKS   ((128 * 1024) / PROXY_H2_CHUNK_आकार)


प्रकारनिर्देश गणना {
  H2_TUNNEL_INIT,     /* init/default/no tunnel state */
  H2_TUNNEL_CONNECT,  /* CONNECT request is being send */
  H2_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H2_TUNNEL_ESTABLISHED,
  H2_TUNNEL_FAILED
} h2_tunnel_state;

संरचना tunnel_प्रवाहः {
  संरचना http_resp *resp;
  संरचना bufq recvbuf;
  संरचना bufq sendbuf;
  वर्ण *authority;
  int32_t प्रवाहः_id;
  uint32_t error;
  h2_tunnel_state state;
  BIT(has_final_response);
  BIT(closed);
  BIT(reset);
};

स्थिर CURLcode tunnel_प्रवाहः_init(संरचना tunnel_प्रवाहः *ts,
                                   संरचना Curl_peer *dest)
{
  ts->state = H2_TUNNEL_INIT;
  ts->प्रवाहः_id = -1;
  Curl_bufq_init2(&ts->recvbuf, PROXY_H2_CHUNK_आकार, H2_TUNNEL_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_सीमा);
  Curl_bufq_init(&ts->sendbuf, PROXY_H2_CHUNK_आकार, H2_TUNNEL_SEND_CHUNKS);

  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->पत्तनम्);
  यदि(!ts->authority)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रत्यागम CURLE_OK;
}

स्थिर शून्य tunnel_प्रवाहः_reset(संरचना tunnel_प्रवाहः *ts)
{
  Curl_http_resp_free(ts->resp);
  ts->resp = रिक्त;
  Curl_bufq_reset(&ts->recvbuf);
  Curl_bufq_reset(&ts->sendbuf);
  ts->प्रवाहः_id = -1;
  ts->error = 0;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->reset = FALSE;
  ts->state = H2_TUNNEL_INIT;
}

स्थिर शून्य tunnel_प्रवाहः_clear(संरचना tunnel_प्रवाहः *ts)
{
  Curl_http_resp_free(ts->resp);
  Curl_bufq_free(&ts->recvbuf);
  Curl_bufq_free(&ts->sendbuf);
  curlx_safefree(ts->authority);
  स्मृतिं_पूरय(ts, 0, परिमाण(*ts));
  ts->state = H2_TUNNEL_INIT;
}

स्थिर शून्य h2_tunnel_go_state(संरचना Curl_cfilter *cf,
                               संरचना tunnel_प्रवाहः *ts,
                               h2_tunnel_state new_state,
                               संरचना Curl_easy *दत्त,
                               bool udp_tunnel)
{
  (शून्य)cf;
  (शून्य)udp_tunnel;

  यदि(ts->state == new_state)
    प्रत्यागम;
  /* leaving this one */
  विकल्प(ts->state) {
  पक्ष H2_TUNNEL_CONNECT:
    दत्त->req.ignorebody = FALSE;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  /* entering this one */
  विकल्प(new_state) {
  पक्ष H2_TUNNEL_INIT:
    CURL_TRC_CF(दत्त, cf, "[%d] new tunnel state 'init'", ts->प्रवाहः_id);
    tunnel_प्रवाहः_reset(ts);
    भङ्ग;

  पक्ष H2_TUNNEL_CONNECT:
    CURL_TRC_CF(दत्त, cf, "[%d] new tunnel state 'connect'", ts->प्रवाहः_id);
    ts->state = H2_TUNNEL_CONNECT;
    भङ्ग;

  पक्ष H2_TUNNEL_RESPONSE:
    CURL_TRC_CF(दत्त, cf, "[%d] new tunnel state 'response'", ts->प्रवाहः_id);
    ts->state = H2_TUNNEL_RESPONSE;
    भङ्ग;

  पक्ष H2_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(दत्त, cf, "[%d] new tunnel state 'established'",
                ts->प्रवाहः_id);
    infof(दत्त, "CONNECT%s phase completed for HTTP/2 proxy",
          udp_tunnel ? "-UDP" : "");
    दत्त->state.authproxy.समाप्तम् = TRUE;
    दत्त->state.authproxy.multipass = FALSE;
    FALLTHROUGH();
  पक्ष H2_TUNNEL_FAILED:
    यदि(new_state == H2_TUNNEL_FAILED)
      CURL_TRC_CF(दत्त, cf, "[%d] new tunnel state 'failed'", ts->प्रवाहः_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. Let's thus free and clear it here. */
    curlx_safefree(दत्त->req.hd_proxy_auth);
    भङ्ग;
  }
}

संरचना cf_h2_proxy_ctx {
  संरचना curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  संरचना cf_call_दत्त call_दत्त;

  संरचना bufq inbufq;  /* network receive buffer */
  संरचना bufq outbufq; /* network send buffer */

  संरचना Curl_peer *peer; /* proxy we talk to */
  संरचना Curl_peer *dest; /* where to tunnel to */
  संरचना tunnel_प्रवाहः tunnel; /* our tunnel CONNECT stream */
  int32_t goaway_error;
  int32_t remote_max_sid;
  BIT(conn_closed);
  BIT(rcvd_goaway);
  BIT(प्रेषितम्_goaway);
  BIT(nw_बहिर्गमनम्_blocked);
  BIT(udp_tunnel);
};

/* How to access `call_data` from a cf_h2 filter */
#अपरिभाषय CF_CTX_CALL_दत्त
#परिभाषय CF_CTX_CALL_दत्त(cf) ((संरचना cf_h2_proxy_ctx *)(cf)->ctx)->call_दत्त

स्थिर शून्य cf_h2_proxy_ctx_clear(संरचना cf_h2_proxy_ctx *ctx)
{
  संरचना cf_call_दत्त save = ctx->call_दत्त;

  Curl_ngh2_del(&ctx->ngh2);
  Curl_bufq_free(&ctx->inbufq);
  Curl_bufq_free(&ctx->outbufq);
  Curl_peer_unlink(&ctx->peer);
  Curl_peer_unlink(&ctx->dest);
  tunnel_प्रवाहः_clear(&ctx->tunnel);
  स्मृतिं_पूरय(ctx, 0, परिमाण(*ctx));
  ctx->call_दत्त = save;
}

स्थिर शून्य cf_h2_proxy_ctx_free(संरचना cf_h2_proxy_ctx *ctx)
{
  यदि(ctx) {
    cf_h2_proxy_ctx_clear(ctx);
    curlx_free(ctx);
  }
}

स्थिर शून्य drain_tunnel(संरचना Curl_cfilter *cf,
                         संरचना Curl_easy *दत्त,
                         संरचना tunnel_प्रवाहः *tunnel)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  (शून्य)cf;
  यदि(!tunnel->closed && !tunnel->reset &&
     (!Curl_bufq_is_रिक्तानि(&ctx->tunnel.sendbuf) ||
      !Curl_bufq_is_रिक्तानि(&ctx->tunnel.recvbuf)))
    Curl_multi_mark_dirty(दत्त);
}

स्थिर CURLcode proxy_h2_nw_बहिर्गमनम्_writer(शून्य *writer_ctx,
                                       ध्रुव uint8_t *buf, size_t buflen,
                                       size_t *pnwritten)
{
  संरचना Curl_cfilter *cf = writer_ctx;
  *pnwritten = 0;
  यदि(cf) {
    संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
    CURLcode परिणाम;
    परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त, buf, buflen, FALSE, pnwritten);
    CURL_TRC_CF(दत्त, cf, "[0] nw_out_writer(len=%zu) -> %d, %zu",
                buflen, (पूर्णाङ्क)परिणाम, *pnwritten);
    प्रत्यागम परिणाम;
  }
  प्रत्यागम CURLE_FAILED_INIT;
}

स्थिर पूर्णाङ्क proxy_h2_client_new(संरचना Curl_cfilter *cf,
                               nghttp2_session_callbacks *cbs)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { रिक्त, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  पूर्णाङ्क rc = nghttp2_option_new(&o);
  यदि(rc)
    प्रत्यागम rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_समुच्चयः_no_auto_गवाक्षः_update(o, 1);
#यदि NGHTTP2_संस्करणम्_अङ्कः >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_समुच्चयः_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#यदि_अन्तः
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  प्रत्यागम rc;
}

स्थिर पूर्णाङ्क proxy_h2_should_close_session(संरचना cf_h2_proxy_ctx *ctx)
{
  प्रत्यागम !Curl_ngh2_valid(&ctx->ngh2) ||
    (!Curl_ngh2_session_want_read(&ctx->ngh2) &&
     !Curl_ngh2_session_want_write(&ctx->ngh2));
}

स्थिर CURLcode proxy_h2_nw_बहिर्गमनम्_flush(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode परिणाम;

  यदि(Curl_bufq_is_रिक्तानि(&ctx->outbufq))
    प्रत्यागम CURLE_OK;

  परिणाम = Curl_bufq_pass(&ctx->outbufq, proxy_h2_nw_बहिर्गमनम्_writer, cf,
                          &nwritten);
  यदि(परिणाम) {
    यदि(परिणाम == CURLE_AGAIN) {
      CURL_TRC_CF(दत्त, cf, "[0] flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_बहिर्गमनम्_blocked = 1;
    }
    प्रत्यागम परिणाम;
  }
  CURL_TRC_CF(दत्त, cf, "[0] nw send buffer flushed");
  प्रत्यागम Curl_bufq_is_रिक्तानि(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
स्थिर CURLcode proxy_h2_process_pending_निवेशः(संरचना Curl_cfilter *cf,
                                               संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  ध्रुव अचिह्नित वर्ण *buf;
  size_t blen, nread;
  ssize_t rv;

  यावत्(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {

    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ध्रुव uint8_t *)buf, blen);
    CURL_TRC_CF(दत्त, cf, "[0] %zu bytes to nghttp2 -> %zd", blen, rv);
    यदि(!curlx_sztouz(rv, &nread)) {
      failf(दत्त,
            "process_pending_input: nghttp2_session_mem_recv() returned "
            "%zd:%s", rv, nghttp2_strerror((पूर्णाङ्क)rv));
      प्रत्यागम CURLE_RECV_ERROR;
    }
    अन्यथा यदि(!nread) {
      /* nghttp2 does not want to process more, but has no error. This
       * probably cannot happen, but be safe. */
      भङ्ग;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    यदि(Curl_bufq_is_रिक्तानि(&ctx->inbufq)) {
      CURL_TRC_CF(दत्त, cf, "[0] all data in connection buffer processed");
      भङ्ग;
    }
    अन्यथा {
      CURL_TRC_CF(दत्त, cf, "[0] process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode proxy_h2_प्रगतिः_ingress(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  size_t nread;

  /* Process network input buffer first */
  यदि(!Curl_bufq_is_रिक्तानि(&ctx->inbufq)) {
    CURL_TRC_CF(दत्त, cf, "[0] process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    परिणाम = proxy_h2_process_pending_निवेशः(cf, दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop or we have enough data for this stream */
  यावत्(!ctx->conn_closed &&                /* not closed the connection */
        !ctx->tunnel.closed &&              /* nor the tunnel */
        Curl_bufq_is_रिक्तानि(&ctx->inbufq) && /* and we consumed our input */
        !Curl_bufq_is_पूर्णम्(&ctx->tunnel.recvbuf)) {

    परिणाम = Curl_cf_recv_bufq(cf->अग्र, दत्त, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(दत्त, cf, "[0] read %zu bytes nw data -> %d, %zu",
                Curl_bufq_len(&ctx->inbufq), (पूर्णाङ्क)परिणाम, nread);
    यदि(परिणाम) {
      यदि(परिणाम != CURLE_AGAIN) {
        failf(दत्त, "Failed receiving HTTP2 proxy data");
        प्रत्यागम परिणाम;
      }
      भङ्ग;
    }
    अन्यथा यदि(nread == 0) {
      CURL_TRC_CF(दत्त, cf, "server closed connection");
      ctx->conn_closed = TRUE;
      भङ्ग;
    }

    परिणाम = proxy_h2_process_pending_निवेशः(cf, दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode proxy_h2_प्रगतिः_egress(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  पूर्णाङ्क rv = 0;

  ctx->nw_बहिर्गमनम्_blocked = 0;
  यावत्(!rv && !ctx->nw_बहिर्गमनम्_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

  यदि(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(दत्त, cf, "[0] nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    प्रत्यागम CURLE_SEND_ERROR;
  }
  प्रत्यागम proxy_h2_nw_बहिर्गमनम्_flush(cf, दत्त);
}

स्थिर ssize_t on_session_send(nghttp2_session *h2,
                               ध्रुव uint8_t *buf, size_t blen, पूर्णाङ्क flags,
                               शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  size_t nwritten;
  CURLcode परिणाम = CURLE_OK;

  (शून्य)h2;
  (शून्य)flags;
  DEBUGASSERT(दत्त);

  परिणाम = Curl_bufq_write_pass(&ctx->outbufq, buf, blen,
                                proxy_h2_nw_बहिर्गमनम्_writer, cf, &nwritten);
  यदि(परिणाम) {
    यदि(परिणाम == CURLE_AGAIN) {
      ctx->nw_बहिर्गमनम्_blocked = 1;
      प्रत्यागम NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(दत्त, "Failed sending HTTP2 data");
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  यदि(!nwritten)
    प्रत्यागम NGHTTP2_ERR_WOULDBLOCK;

  प्रत्यागम (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

#यदि_परिभाषितम् CURLVERBOSE
स्थिर पूर्णाङ्क proxy_h2_on_frame_send(nghttp2_session *session,
                                  ध्रुव nghttp2_frame *frame,
                                  शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);

  (शून्य)session;
  DEBUGASSERT(दत्त);
  यदि(Curl_trc_cf_is_verbose(cf, दत्त)) {
    वर्ण बफर[256];
    पूर्णाङ्क len;
    len = Curl_nghttp2_fr_print(frame, बफर, परिमाण(बफर) - 1);
    बफर[len] = 0;
    CURL_TRC_CF(दत्त, cf, "[%d] -> %s", frame->hd.प्रवाहः_id, बफर);
  }
  प्रत्यागम 0;
}
#यदि_अन्तः /* CURLVERBOSE */

स्थिर पूर्णाङ्क proxy_h2_on_frame_recv(nghttp2_session *session,
                                  ध्रुव nghttp2_frame *frame,
                                  शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  int32_t प्रवाहः_id = frame->hd.प्रवाहः_id;

  (शून्य)session;
  DEBUGASSERT(दत्त);
#यदि_परिभाषितम् CURLVERBOSE
  यदि(Curl_trc_cf_is_verbose(cf, दत्त)) {
    वर्ण बफर[256];
    पूर्णाङ्क len;
    len = Curl_nghttp2_fr_print(frame, बफर, परिमाण(बफर) - 1);
    बफर[len] = 0;
    CURL_TRC_CF(दत्त, cf, "[%d] <- %s", frame->hd.प्रवाहः_id, बफर);
  }
#यदि_अन्तः /* CURLVERBOSE */

  यदि(!प्रवाहः_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(दत्त);
    विकल्प(frame->hd.type) {
    पक्ष NGHTTP2_SETTINGS:
      /* Since the initial stream window is 64K, a request might be on HOLD,
       * due to exhaustion. The (initial) SETTINGS may announce a much larger
       * window and *assume* that we treat this like a WINDOW_UPDATE. Some
       * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
       * To be safe, we UNHOLD a stream in order not to stall. */
      यदि(CURL_REQ_WANT_SEND(दत्त)) {
        drain_tunnel(cf, दत्त, &ctx->tunnel);
      }
      भङ्ग;
    पक्ष NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->remote_max_sid = frame->goaway.चरमः_प्रवाहः_id;
      यदि(दत्त) {
        infof(दत्त, "received GOAWAY, error=%u, last_stream=%d",
              frame->goaway.error_संहिता, ctx->remote_max_sid);
      }
      भङ्ग;
    स्वभाव:
      भङ्ग;
    }
    प्रत्यागम 0;
  }

  यदि(प्रवाहः_id != ctx->tunnel.प्रवाहः_id) {
    CURL_TRC_CF(दत्त, cf, "[%d] rcvd FRAME not for tunnel", प्रवाहः_id);
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  विकल्प(frame->hd.type) {
  पक्ष NGHTTP2_शीर्षकाणि:
    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    यदि(!ctx->tunnel.resp)
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    /* Only final status code signals the end of header */
    CURL_TRC_CF(दत्त, cf, "[%d] got http status: %d",
                प्रवाहः_id, ctx->tunnel.resp->स्थितिः);
    यदि(!ctx->tunnel.has_final_response && ctx->tunnel.resp->स्थितिः / 100 != 1)
      ctx->tunnel.has_final_response = TRUE;
    भङ्ग;
  पक्ष NGHTTP2_गवाक्षः_UPDATE:
    यदि(CURL_REQ_WANT_SEND(दत्त)) {
      drain_tunnel(cf, दत्त, &ctx->tunnel);
    }
    भङ्ग;
  पक्ष NGHTTP2_RST_प्रवाहः:
    यदि(frame->rst_प्रवाहः.error_संहिता)
      ctx->tunnel.reset = TRUE;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क proxy_h2_on_शीर्षकम्(nghttp2_session *session,
                              ध्रुव nghttp2_frame *frame,
                              ध्रुव uint8_t *name, size_t namelen,
                              ध्रुव uint8_t *मानम्, size_t valuelen,
                              uint8_t flags,
                              शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  int32_t प्रवाहः_id = frame->hd.प्रवाहः_id;
  CURLcode परिणाम;

  (शून्य)flags;
  (शून्य)session;
  DEBUGASSERT(प्रवाहः_id); /* should never be a zero stream ID here */
  यदि(प्रवाहः_id != ctx->tunnel.प्रवाहः_id) {
    CURL_TRC_CF(दत्त, cf, "[%d] header for non-tunnel stream: "
                "%.*s: %.*s", प्रवाहः_id,
                (पूर्णाङ्क)namelen, name, (पूर्णाङ्क)valuelen, मानम्);
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  यदि(frame->hd.type == NGHTTP2_प्रक्षिप_PROMISE)
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  यदि(ctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    प्रत्यागम 0;
  }

  यदि(namelen == CURL_CSTRLEN(HTTP_PSEUDO_स्थितिः) &&
     !memcmp(HTTP_PSEUDO_स्थितिः, name, namelen)) {
    int32_t http_स्थितिः;
    संरचना http_resp *resp;

    /* status: always comes first, we might get more than one response,
     * discard previous, interim responses */
    परिणाम = Curl_http_decode_स्थितिः(&http_स्थितिः,
                                    (ध्रुव वर्ण *)मानम्, valuelen);
    यदि(परिणाम)
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    परिणाम = Curl_http_resp_make(&resp, http_स्थितिः, रिक्त);
    यदि(परिणाम)
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    यदि(ctx->tunnel.resp)
      Curl_http_resp_free(ctx->tunnel.resp);
    ctx->tunnel.resp = resp;
    CURL_TRC_CF(दत्त, cf, "[%d] status: HTTP/2 %03d",
                प्रवाहः_id, ctx->tunnel.resp->स्थितिः);
    प्रत्यागम 0;
  }

  यदि(!ctx->tunnel.resp)
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  परिणाम = Curl_dynhds_सङ्कलय(&ctx->tunnel.resp->शीर्षकाणि,
                           (ध्रुव वर्ण *)name, namelen,
                           (ध्रुव वर्ण *)मानम्, valuelen);
  यदि(परिणाम)
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  CURL_TRC_CF(दत्त, cf, "[%d] header: %.*s: %.*s",
              प्रवाहः_id, (पूर्णाङ्क)namelen, name, (पूर्णाङ्क)valuelen, मानम्);

  प्रत्यागम 0; /* 0 is successful */
}

स्थिर ssize_t tunnel_send_callback(nghttp2_session *session,
                                    int32_t प्रवाहः_id,
                                    uint8_t *buf, size_t दीर्घता,
                                    uint32_t *दत्त_flags,
                                    nghttp2_दत्त_source *source,
                                    शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  संरचना tunnel_प्रवाहः *ts;
  CURLcode परिणाम;
  size_t nread;

  (शून्य)session;
  (शून्य)source;
  (शून्य)ctx;

  यदि(!प्रवाहः_id)
    प्रत्यागम NGHTTP2_ERR_INVALID_तर्कः;

  ts = Curl_ngh2_session_get_प्रवाहः_user_दत्त(&ctx->ngh2, प्रवाहः_id);
  यदि(!ts)
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  DEBUGASSERT(ts == &ctx->tunnel);

  परिणाम = Curl_bufq_read(&ts->sendbuf, buf, दीर्घता, &nread);
  यदि(परिणाम) {
    यदि(परिणाम != CURLE_AGAIN)
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    प्रत्यागम NGHTTP2_ERR_DEFERRED;
  }
  यदि(ts->closed && Curl_bufq_is_रिक्तानि(&ts->sendbuf))
    *दत्त_flags = NGHTTP2_दत्त_FLAG_EOF;

  CURL_TRC_CF(दत्त, cf, "[%d] tunnel_send_callback -> %zu",
              ts->प्रवाहः_id, nread);
  प्रत्यागम (nread  > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nread;
}

स्थिर पूर्णाङ्क tunnel_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t प्रवाहः_id,
                                ध्रुव uint8_t *mem, size_t len, शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode परिणाम;

  (शून्य)flags;
  (शून्य)session;
  DEBUGASSERT(प्रवाहः_id); /* should never be a zero stream ID here */

  यदि(प्रवाहः_id != ctx->tunnel.प्रवाहः_id)
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  परिणाम = Curl_bufq_write(&ctx->tunnel.recvbuf, mem, len, &nwritten);
  यदि(परिणाम) {
    यदि(परिणाम != CURLE_AGAIN)
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
#यदि_परिभाषितम् DEBUGBUILD
    nwritten = 0;
#यदि_अन्तः
  }
  /* tunnel.recbuf has soft limit, any success MUST add all data */
  DEBUGASSERT(nwritten == len);
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क proxy_h2_on_प्रवाहः_close(nghttp2_session *session,
                                    int32_t प्रवाहः_id,
                                    uint32_t error_संहिता, शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);

  (शून्य)session;

  यदि(प्रवाहः_id != ctx->tunnel.प्रवाहः_id)
    प्रत्यागम 0;

  CURL_TRC_CF(दत्त, cf, "[%d] proxy_h2_on_stream_close, %s (err %u)",
              प्रवाहः_id, nghttp2_http2_strerror(error_संहिता), error_संहिता);
  ctx->tunnel.closed = TRUE;
  ctx->tunnel.error = error_संहिता;
  यदि(error_संहिता)
    ctx->tunnel.reset = TRUE;

  प्रत्यागम 0;
}

स्थिर CURLcode proxy_h2_submit(
  int32_t *pstream_id,
  संरचना Curl_cfilter *cf,
  संरचना Curl_easy *दत्त,
  संरचना curl_ngh2 *ngh2,
  संरचना httpreq *req,
  ध्रुव nghttp2_प्राथमिकता_spec *pri_spec,
  शून्य *प्रवाहः_user_दत्त,
  nghttp2_दत्त_source_read_callback read_callback,
  शून्य *read_ctx)
{
  संरचना dynhds h2_शीर्षकाणि;
  nghttp2_nv *nva = रिक्त;
  int32_t प्रवाहः_id = -1;
  size_t nheader;
  CURLcode परिणाम;

  (शून्य)cf;
  Curl_dynhds_init(&h2_शीर्षकाणि, MAX_HTTP_RESP_शीर्षकम्_गणनम्, DYN_HTTP_REQUEST);
  परिणाम = Curl_http_req_गन्तव्यम्_h2(&h2_शीर्षकाणि, req, दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  nva = Curl_dynhds_गन्तव्यम्_nva(&h2_शीर्षकाणि, &nheader);
  यदि(!nva) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  यदि(read_callback) {
    nghttp2_दत्त_provider दत्त_prd;

    दत्त_prd.read_callback = read_callback;
    दत्त_prd.source.ptr = read_ctx;
    प्रवाहः_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, &दत्त_prd, प्रवाहः_user_दत्त);
  }
  अन्यथा {
    प्रवाहः_id = Curl_ngh2_submit_request(
      ngh2, pri_spec, nva, nheader, रिक्त, प्रवाहः_user_दत्त);
  }

  यदि(प्रवाहः_id < 0) {
    failf(दत्त, "nghttp2_session_upgrade2() failed: %s(%d)",
          nghttp2_strerror(प्रवाहः_id), प्रवाहः_id);
    परिणाम = CURLE_SEND_ERROR;
    गच्छ बहिर्गमनम्;
  }
  परिणाम = CURLE_OK;

बहिर्गमनम्:
  curlx_free(nva);
  Curl_dynhds_free(&h2_शीर्षकाणि);
  *pstream_id = प्रवाहः_id;
  प्रत्यागम परिणाम;
}

स्थिर CURLcode submit_CONNECT(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               संरचना tunnel_प्रवाहः *ts)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम;
  संरचना httpreq *req = रिक्त;

  परिणाम = Curl_http_proxy_create_tunnel_request(&req, cf, दत्त,
                                                 ctx->peer, ctx->dest,
                                                  PROXY_HTTP_V2,
                                                  (bool)ctx->udp_tunnel);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  परिणाम = Curl_creader_समुच्चयः_null(दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = proxy_h2_submit(&ts->प्रवाहः_id, cf, दत्त, &ctx->ngh2, req,
                           रिक्त, ts, tunnel_send_callback, cf);
  यदि(परिणाम) {
    CURL_TRC_CF(दत्त, cf, "[%d] send, nghttp2_submit_request error: %s",
                ts->प्रवाहः_id, nghttp2_strerror(ts->प्रवाहः_id));
  }

बहिर्गमनम्:
  यदि(req)
    Curl_http_req_free(req);
  यदि(परिणाम)
    failf(दत्त, "Failed sending CONNECT to proxy");
  प्रत्यागम परिणाम;
}

स्थिर CURLcode inspect_response(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना tunnel_प्रवाहः *ts)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  proxy_inspect_परिणाम res;
  CURLcode परिणाम;

  परिणाम = Curl_http_proxy_inspect_tunnel_response(
      cf, दत्त, ts->resp, (bool)ctx->udp_tunnel, &res);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  विकल्प(res) {
  पक्ष PROXY_INSPECT_OK:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_ESTABLISHED, दत्त,
                       (bool)ctx->udp_tunnel);
    भङ्ग;
  पक्ष PROXY_INSPECT_FAILED:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, दत्त,
                       (bool)ctx->udp_tunnel);
    परिणाम = CURLE_COULDNT_CONNECT;
    भङ्ग;
  पक्ष PROXY_INSPECT_AUTH_RETRY:
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_INIT, दत्त,
                       (bool)ctx->udp_tunnel);
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode H2_CONNECT(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *दत्त,
                           संरचना tunnel_प्रवाहः *ts)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  यदि(ctx->conn_closed) {
    failf(दत्त, "proxy closed connection");
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }

  कुरु {
    विकल्प(ts->state) {
    पक्ष H2_TUNNEL_INIT:
      /* Prepare the CONNECT request and make a first attempt to send. */
      CURL_TRC_CF(दत्त, cf, "[0] CONNECT start for %s", ts->authority);
      परिणाम = submit_CONNECT(cf, दत्त, ts);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      h2_tunnel_go_state(cf, ts, H2_TUNNEL_CONNECT, दत्त,
                         (bool)ctx->udp_tunnel);
      FALLTHROUGH();

    पक्ष H2_TUNNEL_CONNECT:
      /* see that the request is completely sent */
      परिणाम = proxy_h2_प्रगतिः_ingress(cf, दत्त);
      यदि(!परिणाम)
        परिणाम = proxy_h2_प्रगतिः_egress(cf, दत्त);
      यदि(परिणाम && परिणाम != CURLE_AGAIN) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, दत्त,
                           (bool)ctx->udp_tunnel);
        भङ्ग;
      }

      यदि(ts->has_final_response) {
        h2_tunnel_go_state(cf, ts, H2_TUNNEL_RESPONSE, दत्त,
                           (bool)ctx->udp_tunnel);
      }
      अन्यथा {
        परिणाम = CURLE_OK;
        गच्छ बहिर्गमनम्;
      }
      FALLTHROUGH();

    पक्ष H2_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      परिणाम = inspect_response(cf, दत्त, ts);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      भङ्ग;

    पक्ष H2_TUNNEL_ESTABLISHED:
      प्रत्यागम CURLE_OK;

    पक्ष H2_TUNNEL_FAILED:
      प्रत्यागम CURLE_RECV_ERROR;

    स्वभाव:
      भङ्ग;
    }

  } यावत्(ts->state == H2_TUNNEL_INIT);

बहिर्गमनम्:
  यदि((परिणाम && (परिणाम != CURLE_AGAIN)) || ctx->tunnel.closed)
    h2_tunnel_go_state(cf, ts, H2_TUNNEL_FAILED, दत्त,
                       (bool)ctx->udp_tunnel);
  प्रत्यागम परिणाम;
}

/*
 * Initialize the cfilter context
 */
स्थिर CURLcode cf_h2_proxy_ctx_init(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  nghttp2_session_callbacks *cbs = रिक्त;
  पूर्णाङ्क rc;

  DEBUGASSERT(!ctx->ngh2.s);
  स्मृतिं_पूरय(&ctx->tunnel, 0, परिमाण(ctx->tunnel));

  Curl_bufq_init(&ctx->inbufq, PROXY_H2_CHUNK_आकार, PROXY_H2_NW_RECV_CHUNKS);
  Curl_bufq_init(&ctx->outbufq, PROXY_H2_CHUNK_आकार, PROXY_H2_NW_SEND_CHUNKS);
  ctx->remote_max_sid = INT32_MAX;

  यदि(tunnel_प्रवाहः_init(&ctx->tunnel, ctx->dest))
    गच्छ बहिर्गमनम्;

  rc = nghttp2_session_callbacks_new(&cbs);
  यदि(rc) {
    failf(दत्त, "Could not initialize nghttp2 callbacks");
    गच्छ बहिर्गमनम्;
  }

  nghttp2_session_callbacks_समुच्चयः_send_callback(cbs, on_session_send);
  nghttp2_session_callbacks_समुच्चयः_on_frame_recv_callback(
    cbs, proxy_h2_on_frame_recv);
#यदि_परिभाषितम् CURLVERBOSE
  nghttp2_session_callbacks_समुच्चयः_on_frame_send_callback(cbs,
                                                       proxy_h2_on_frame_send);
#यदि_अन्तः
  nghttp2_session_callbacks_समुच्चयः_on_दत्त_chunk_recv_callback(
    cbs, tunnel_recv_callback);
  nghttp2_session_callbacks_समुच्चयः_on_प्रवाहः_close_callback(
    cbs, proxy_h2_on_प्रवाहः_close);
  nghttp2_session_callbacks_समुच्चयः_on_शीर्षकम्_callback(cbs, proxy_h2_on_शीर्षकम्);

  /* The nghttp2 session is not yet setup, do it */
  rc = proxy_h2_client_new(cf, cbs);
  यदि(rc) {
    failf(दत्त, "Could not initialize nghttp2");
    गच्छ बहिर्गमनम्;
  }

  {
    nghttp2_settings_प्रविष्टिः iv[3];

    iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
    iv[0].मानम् = Curl_multi_max_concurrent_streams(दत्त->multi);
    iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_गवाक्षः_आकार;
    iv[1].मानम् = H2_TUNNEL_गवाक्षः_आकार;
    iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_प्रक्षिप;
    iv[2].मानम् = 0;
    rc = Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, iv, 3);
    यदि(rc) {
      failf(दत्त, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      परिणाम = CURLE_HTTP2;
      गच्छ बहिर्गमनम्;
    }
  }

  rc = Curl_ngh2_session_समुच्चयः_local_गवाक्षः_आकार(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, PROXY_HTTP2_HUGE_गवाक्षः_आकार);
  यदि(rc) {
    failf(दत्त, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    परिणाम = CURLE_HTTP2;
    गच्छ बहिर्गमनम्;
  }

  /* all set, traffic will be send on connect */
  परिणाम = CURLE_OK;

बहिर्गमनम्:
  यदि(cbs)
    nghttp2_session_callbacks_del(cbs);
  CURL_TRC_CF(दत्त, cf, "[0] init proxy ctx -> %d", (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h2_proxy_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    bool *समाप्तम्)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  संरचना cf_call_दत्त save;
  संरचना tunnel_प्रवाहः *ts = &ctx->tunnel;

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  /* Connect the lower filters first */
  यदि(!cf->अग्र->connected) {
    परिणाम = Curl_conn_cf_connect(cf->अग्र, दत्त, समाप्तम्);
    यदि(परिणाम || !*समाप्तम्)
      प्रत्यागम परिणाम;
  }

  *समाप्तम् = FALSE;

  CF_दत्त_SAVE(save, cf, दत्त);
  यदि(!ctx->ngh2.s) {
    परिणाम = cf_h2_proxy_ctx_init(cf, दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  DEBUGASSERT(ts->authority);

  यदि(Curl_timeleft_ms(दत्त) < 0) {
    failf(दत्त, "Proxy CONNECT aborted due to timeout");
    परिणाम = CURLE_OPERATION_TIMEDOUT;
    गच्छ बहिर्गमनम्;
  }

  /* for the secondary socket (FTP), use the "connect to host"
   * but ignore the "connect to port" (use the secondary port)
   */
  परिणाम = H2_CONNECT(cf, दत्त, ts);

बहिर्गमनम्:
  *समाप्तम् = (परिणाम == CURLE_OK) && (ts->state == H2_TUNNEL_ESTABLISHED);
  यदि(*समाप्तम्) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&दत्त->req, दत्त);
    Curl_client_reset(दत्त);
  }
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_h2_proxy_destroy(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;

  (शून्य)दत्त;
  यदि(ctx) {
    cf_h2_proxy_ctx_free(ctx);
    cf->ctx = रिक्त;
  }
}

स्थिर CURLcode cf_h2_proxy_shutdown(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना cf_call_दत्त save;
  CURLcode परिणाम;
  पूर्णाङ्क rv;

  यदि(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  CF_दत्त_SAVE(save, cf, दत्त);

  यदि(!ctx->प्रेषितम्_goaway) {
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, 0,
      (ध्रुव uint8_t *)"shutdown", परिमाण("shutdown"));
    यदि(rv) {
      failf(दत्त, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
    ctx->प्रेषितम्_goaway = TRUE;
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  परिणाम = CURLE_OK;
  यदि(Curl_ngh2_session_want_write(&ctx->ngh2))
    परिणाम = proxy_h2_प्रगतिः_egress(cf, दत्त);
  यदि(!परिणाम && Curl_ngh2_session_want_read(&ctx->ngh2))
    परिणाम = proxy_h2_प्रगतिः_ingress(cf, दत्त);

  *समाप्तम् = (ctx->conn_closed ||
           (!परिणाम && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2)));
बहिर्गमनम्:
  CF_दत्त_RESTORE(cf, save);
  cf->shutdown = (परिणाम || *समाप्तम्);
  प्रत्यागम परिणाम;
}

स्थिर bool cf_h2_proxy_दत्त_pending(संरचना Curl_cfilter *cf,
                                     ध्रुव संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  यदि((ctx && !Curl_bufq_is_रिक्तानि(&ctx->inbufq)) ||
     (ctx && ctx->tunnel.state == H2_TUNNEL_ESTABLISHED &&
      !Curl_bufq_is_रिक्तानि(&ctx->tunnel.recvbuf)))
    प्रत्यागम TRUE;
  प्रत्यागम cf->अग्र ? cf->अग्र->cft->has_दत्त_pending(cf->अग्र, दत्त) : FALSE;
}

स्थिर CURLcode cf_h2_proxy_adjust_pollset(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *दत्त,
                                           संरचना easy_pollset *ps)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना cf_call_दत्त save;
  curl_socket_t sock = Curl_conn_cf_get_socket(cf, दत्त);
  bool want_recv, want_send;
  CURLcode परिणाम = CURLE_OK;

  यदि(!cf->connected && Curl_ngh2_valid(&ctx->ngh2)) {
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_रिक्तानि(&ctx->outbufq) ||
                !Curl_bufq_is_रिक्तानि(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
  }
  अन्यथा
    Curl_pollset_check(दत्त, ps, sock, &want_recv, &want_send);

  यदि(Curl_ngh2_valid(&ctx->ngh2) && (want_recv || want_send)) {
    bool c_exhaust, s_exhaust;

    CF_दत्त_SAVE(save, cf, दत्त);
    c_exhaust = !Curl_ngh2_session_get_remote_गवाक्षः_आकार(&ctx->ngh2);
    s_exhaust = ctx->tunnel.प्रवाहः_id >= 0 &&
                !Curl_ngh2_session_get_प्रवाहः_remote_गवाक्षः_आकार(
                  &ctx->ngh2, ctx->tunnel.प्रवाहः_id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_रिक्तानि(&ctx->outbufq) ||
                !Curl_bufq_is_रिक्तानि(&ctx->tunnel.sendbuf);

    परिणाम = Curl_pollset_समुच्चयः(दत्त, ps, sock, want_recv, want_send);
    CURL_TRC_CF(दत्त, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (पूर्णाङ्क)परिणाम);
    CF_दत्त_RESTORE(cf, save);
  }
  अन्यथा यदि(ctx->प्रेषितम्_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_दत्त_SAVE(save, cf, दत्त);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_रिक्तानि(&ctx->outbufq) ||
                !Curl_bufq_is_रिक्तानि(&ctx->tunnel.sendbuf);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    परिणाम = Curl_pollset_समुच्चयः(दत्त, ps, sock, want_recv, want_send);
    CURL_TRC_CF(दत्त, cf, "adjust_pollset, want_recv=%d want_send=%d -> %d",
                want_recv, want_send, (पूर्णाङ्क)परिणाम);
    CF_दत्त_RESTORE(cf, save);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode h2_handle_tunnel_close(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त,
                                       size_t *pnread)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;

  *pnread = 0;
  यदि(ctx->tunnel.error) {
    failf(दत्त, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ctx->tunnel.प्रवाहः_id, ctx->tunnel.reset ? "server" : "curl",
          ctx->tunnel.error, nghttp2_http2_strerror(ctx->tunnel.error));
    प्रत्यागम CURLE_RECV_ERROR;
  }

  CURL_TRC_CF(दत्त, cf, "[%d] handle_tunnel_close -> 0",
              ctx->tunnel.प्रवाहः_id);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode tunnel_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                            वर्ण *buf, size_t len, size_t *pnread)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_AGAIN;

  *pnread = 0;
  यदि(!Curl_bufq_is_रिक्तानि(&ctx->tunnel.recvbuf))
    परिणाम = Curl_bufq_cread(&ctx->tunnel.recvbuf, buf, len, pnread);
  अन्यथा {
    यदि(ctx->tunnel.closed) {
      परिणाम = h2_handle_tunnel_close(cf, दत्त, pnread);
    }
    अन्यथा यदि(ctx->tunnel.reset ||
            (ctx->conn_closed && Curl_bufq_is_रिक्तानि(&ctx->inbufq)) ||
            (ctx->rcvd_goaway &&
             ctx->remote_max_sid < ctx->tunnel.प्रवाहः_id)) {
      परिणाम = CURLE_RECV_ERROR;
    }
    अन्यथा
      परिणाम = CURLE_AGAIN;
  }

  CURL_TRC_CF(दत्त, cf, "[%d] tunnel_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.प्रवाहः_id, len, (पूर्णाङ्क)परिणाम, *pnread);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h2_proxy_recv(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 वर्ण *buf, size_t len,
                                 size_t *pnread)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना cf_call_दत्त save;
  CURLcode परिणाम;

  *pnread = 0;
  CF_दत्त_SAVE(save, cf, दत्त);

  यदि(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    परिणाम = CURLE_RECV_ERROR;
    गच्छ बहिर्गमनम्;
  }

  यदि(Curl_bufq_is_रिक्तानि(&ctx->tunnel.recvbuf)) {
    परिणाम = proxy_h2_प्रगतिः_ingress(cf, दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  परिणाम = tunnel_recv(cf, दत्त, buf, len, pnread);

  यदि(!परिणाम) {
    CURL_TRC_CF(दत्त, cf, "[%d] increase window by %zu",
                ctx->tunnel.प्रवाहः_id, *pnread);
    Curl_ngh2_session_consume(&ctx->ngh2, ctx->tunnel.प्रवाहः_id, *pnread);
  }

  परिणाम = Curl_1st_fatal(परिणाम, proxy_h2_प्रगतिः_egress(cf, दत्त));

बहिर्गमनम्:
  यदि((!Curl_bufq_is_रिक्तानि(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_रिक्तानि(&ctx->tunnel.sendbuf)) &&
     (!परिणाम || (परिणाम == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, दत्त, &ctx->tunnel);
  }
  CURL_TRC_CF(दत्त, cf, "[%d] cf_recv(len=%zu) -> %d, %zu",
              ctx->tunnel.प्रवाहः_id, len, (पूर्णाङ्क)परिणाम, *pnread);
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h2_proxy_send(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 ध्रुव uint8_t *buf, size_t len, bool eos,
                                 size_t *pnwritten)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना cf_call_दत्त save;
  CURLcode परिणाम;

  (शून्य)eos;
  *pnwritten = 0;
  CF_दत्त_SAVE(save, cf, दत्त);

  यदि(ctx->tunnel.state != H2_TUNNEL_ESTABLISHED) {
    परिणाम = CURLE_SEND_ERROR;
    गच्छ बहिर्गमनम्;
  }

  यदि(ctx->tunnel.closed) {
    परिणाम = CURLE_SEND_ERROR;
    गच्छ बहिर्गमनम्;
  }

  परिणाम = Curl_bufq_write(&ctx->tunnel.sendbuf, buf, len, pnwritten);
  CURL_TRC_CF(दत्त, cf, "cf_send(), bufq_write %d, %zu", (पूर्णाङ्क)परिणाम,
              *pnwritten);
  यदि(परिणाम && (परिणाम != CURLE_AGAIN))
    गच्छ बहिर्गमनम्;

  यदि(!Curl_bufq_is_रिक्तानि(&ctx->tunnel.sendbuf)) {
    /* req body data is buffered, resume the potentially suspended stream */
    यदि(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_दत्त(&ctx->ngh2, ctx->tunnel.प्रवाहः_id))) {
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }

  परिणाम = Curl_1st_fatal(परिणाम, proxy_h2_प्रगतिः_ingress(cf, दत्त));
  परिणाम = Curl_1st_fatal(परिणाम, proxy_h2_प्रगतिः_egress(cf, दत्त));

  यदि(!परिणाम && proxy_h2_should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    यदि(ctx->tunnel.closed) {
      परिणाम = CURLE_SEND_ERROR;
    }
    अन्यथा {
      CURL_TRC_CF(दत्त, cf, "[0] send: nothing to do in this session");
      परिणाम = CURLE_HTTP2;
    }
  }

बहिर्गमनम्:
  यदि((!Curl_bufq_is_रिक्तानि(&ctx->tunnel.recvbuf) ||
      !Curl_bufq_is_रिक्तानि(&ctx->tunnel.sendbuf)) &&
     (!परिणाम || (परिणाम == CURLE_AGAIN))) {
    /* data pending and no fatal error to report. Need to trigger
     * draining to avoid stalling when no socket events happen. */
    drain_tunnel(cf, दत्त, &ctx->tunnel);
  }
  CURL_TRC_CF(दत्त, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.प्रवाहः_id, len, (पूर्णाङ्क)परिणाम, *pnwritten,
              Curl_ngh2_session_get_प्रवाहः_remote_गवाक्षः_आकार(
                &ctx->ngh2, ctx->tunnel.प्रवाहः_id),
              Curl_ngh2_session_get_remote_गवाक्षः_आकार(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h2_proxy_flush(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  संरचना cf_call_दत्त save;
  CURLcode परिणाम = CURLE_OK;

  CF_दत्त_SAVE(save, cf, दत्त);
  यदि(!Curl_bufq_is_रिक्तानि(&ctx->tunnel.sendbuf)) {
    /* resume the potentially suspended tunnel */
    यदि(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_दत्त(&ctx->ngh2, ctx->tunnel.प्रवाहः_id))) {
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }

  परिणाम = proxy_h2_प्रगतिः_egress(cf, दत्त);

बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf, "[%d] flush -> %d, "
              "h2 windows %d-%d (stream-conn), buffers %zu-%zu (stream-conn)",
              ctx->tunnel.प्रवाहः_id, (पूर्णाङ्क)परिणाम,
              Curl_ngh2_session_get_प्रवाहः_remote_गवाक्षः_आकार(
                &ctx->ngh2, ctx->tunnel.प्रवाहः_id),
              Curl_ngh2_session_get_remote_गवाक्षः_आकार(&ctx->ngh2),
              Curl_bufq_len(&ctx->tunnel.sendbuf),
              Curl_bufq_len(&ctx->outbufq));
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर bool proxy_h2_connisalive(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 bool *निवेशः_pending)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *निवेशः_pending = FALSE;
  यदि(!cf->अग्र || !cf->अग्र->cft->is_alive(cf->अग्र, दत्त, निवेशः_pending))
    प्रत्यागम FALSE;

  यदि(*निवेशः_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode परिणाम;
    size_t nread;

    *निवेशः_pending = FALSE;
    परिणाम = Curl_cf_recv_bufq(cf->अग्र, दत्त, &ctx->inbufq, 0, &nread);
    यदि(!परिणाम) {
      यदि(proxy_h2_process_pending_निवेशः(cf, दत्त))
        /* immediate error, considered dead */
        alive = FALSE;
      अन्यथा {
        alive = !proxy_h2_should_close_session(ctx);
      }
    }
    अन्यथा यदि(परिणाम != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  प्रत्यागम alive;
}

स्थिर bool cf_h2_proxy_is_alive(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 bool *निवेशः_pending)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;
  bool alive;
  संरचना cf_call_दत्त save;

  *निवेशः_pending = FALSE;
  CF_दत्त_SAVE(save, cf, दत्त);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
          proxy_h2_connisalive(cf, दत्त, निवेशः_pending));
  CURL_TRC_CF(दत्त, cf, "[0] conn alive -> %d, input_pending=%d",
              alive, *निवेशः_pending);
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम alive;
}

स्थिर CURLcode cf_h2_proxy_query(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_h2_proxy_ctx *ctx = cf->ctx;

  विकल्प(query) {
  पक्ष CF_QUERY_HOST_पत्तनम्:
    *pres1 = (पूर्णाङ्क)ctx->dest->पत्तनम्;
    *((ध्रुव वर्ण **)pres2) = ctx->dest->hostname;
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_NEED_FLUSH: {
    यदि(!Curl_bufq_is_रिक्तानि(&ctx->outbufq) ||
       !Curl_bufq_is_रिक्तानि(&ctx->tunnel.sendbuf)) {
      CURL_TRC_CF(दत्त, cf, "needs flush");
      *pres1 = TRUE;
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  }
  पक्ष CF_QUERY_ALPN_NEGOTIATED: {
    ध्रुव वर्ण **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = रिक्त;
    प्रत्यागम CURLE_OK;
  }
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, दत्त, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode cf_h2_proxy_cntrl(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना cf_call_दत्त save;

  (शून्य)arg1;
  (शून्य)arg2;

  विकल्प(event) {
  पक्ष CF_CTRL_FLUSH:
    CF_दत्त_SAVE(save, cf, दत्त);
    परिणाम = cf_h2_proxy_flush(cf, दत्त);
    CF_दत्त_RESTORE(cf, save);
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

संरचना Curl_cftype Curl_cft_h2_proxy = {
  "H2-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  cf_h2_proxy_destroy,
  cf_h2_proxy_connect,
  cf_h2_proxy_shutdown,
  cf_h2_proxy_adjust_pollset,
  cf_h2_proxy_दत्त_pending,
  cf_h2_proxy_send,
  cf_h2_proxy_recv,
  cf_h2_proxy_cntrl,
  cf_h2_proxy_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_h2_proxy_query,
};

CURLcode Curl_cf_h2_proxy_निवेशय_after(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त,
                                       संरचना Curl_peer *peer,
                                       संरचना Curl_peer *dest,
                                       bool udp_tunnel)
{
  संरचना Curl_cfilter *cf_h2_proxy = रिक्त;
  संरचना cf_h2_proxy_ctx *ctx;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  (शून्य)दत्त;
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx)
    गच्छ बहिर्गमनम्;
  Curl_peer_link(&ctx->peer, peer);
  Curl_peer_link(&ctx->dest, dest);
  ctx->udp_tunnel = udp_tunnel;

  परिणाम = Curl_cf_create(&cf_h2_proxy, &Curl_cft_h2_proxy, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  ctx = रिक्त;
  Curl_conn_cf_निवेशय_after(cf, cf_h2_proxy);

बहिर्गमनम्:
  cf_h2_proxy_ctx_free(ctx);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !CURL_DISABLE_HTTP && !CURL_DISABLE_PROXY && USE_NGHTTP2 */
