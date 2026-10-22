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
  defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3) &&              \
  defined(USE_NGTCP2) && defined(USE_OPENSSL)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "vquic/cf-ngtcp2-cmn.h"
#अन्तर्भाव "vquic/cf-ngtcp2-proxy.h"
#अन्तर्भाव "capsule.h"

/* A stream window is the maximum amount we need to buffer for
 * each active transfer. We use HTTP/3 flow control and only ACK
 * when we take things out of the buffer.
 * Chunk size is large enough to take a full DATA frame */
#परिभाषय PROXY_H3_TUNNEL_RX_WIN      (512 * 1024)
#परिभाषय PROXY_H3_TUNNEL_RECV_CHUNKS \
                               (PROXY_H3_TUNNEL_RX_WIN / H3_प्रवाहः_CHUNK_आकार)

प्रकारनिर्देश गणना {
  H3_TUNNEL_INIT,     /* init/default/no tunnel state */
  H3_TUNNEL_CONNECT,  /* CONNECT request is being sent */
  H3_TUNNEL_RESPONSE, /* CONNECT response received completely */
  H3_TUNNEL_ESTABLISHED,
  H3_TUNNEL_FAILED
} h3_tunnel_state;

संरचना h3_tunnel_प्रवाहः {
  संरचना Curl_peer *dest;  /* where the tunnel goes to */
  संरचना http_resp *resp;
  संरचना bufq recvbuf;
  वर्ण *authority;
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  h3_tunnel_state state;
  BIT(udp);
  BIT(has_final_response);
  BIT(closed);
};

स्थिर CURLcode h3_tunnel_प्रवाहः_init(संरचना h3_tunnel_प्रवाहः *ts,
                                      संरचना Curl_peer *dest,
                                      bool udp)
{
  ts->state = H3_TUNNEL_INIT;
  Curl_peer_link(&ts->dest, dest);
  Curl_bufq_init2(&ts->recvbuf, H3_प्रवाहः_CHUNK_आकार,
                  PROXY_H3_TUNNEL_RECV_CHUNKS, BUFQ_OPT_SOFT_सीमा);
  ts->udp = udp;
  /* host:port with IPv6 support */
  ts->authority = curl_maprintf("%s%s%s:%u", dest->ipv6 ? "[" : "",
                                dest->hostname,
                                dest->ipv6 ? "]" : "",
                                dest->पत्तनम्);
  यदि(!ts->authority)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रत्यागम CURLE_OK;
}

स्थिर शून्य h3_tunnel_प्रवाहः_reset(संरचना h3_tunnel_प्रवाहः *ts)
{
  Curl_bufq_reset(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  ts->resp = रिक्त;
  ts->प्रवाहः = रिक्त;
  ts->has_final_response = FALSE;
  ts->closed = FALSE;
  ts->state = H3_TUNNEL_INIT;
}

स्थिर शून्य h3_tunnel_प्रवाहः_cleanup(संरचना h3_tunnel_प्रवाहः *ts)
{
  Curl_peer_unlink(&ts->dest);
  Curl_bufq_free(&ts->recvbuf);
  Curl_http_resp_free(ts->resp);
  curlx_safefree(ts->authority);
  ts->state = H3_TUNNEL_INIT;
}

स्थिर शून्य h3_tunnel_go_state(संरचना Curl_cfilter *cf,
                               संरचना h3_tunnel_प्रवाहः *ts,
                               h3_tunnel_state new_state,
                               संरचना Curl_easy *दत्त)
{
  VERBOSE(int64_t प्रवाहः_id = ts->प्रवाहः ? ts->प्रवाहः->id : -1);
  (शून्य)cf;

  यदि(ts->state == new_state)
    प्रत्यागम;

  /* leaving this one */
  विकल्प(ts->state) {
  पक्ष H3_TUNNEL_CONNECT:
    दत्त->req.ignorebody = FALSE;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }

  /* entering this one */
  विकल्प(new_state) {
  पक्ष H3_TUNNEL_INIT:
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] -> [init]", प्रवाहः_id);
    h3_tunnel_प्रवाहः_reset(ts);
    भङ्ग;
  पक्ष H3_TUNNEL_CONNECT:
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] -> [connect]", प्रवाहः_id);
    ts->state = H3_TUNNEL_CONNECT;
    भङ्ग;
  पक्ष H3_TUNNEL_RESPONSE:
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] -> [response]", प्रवाहः_id);
    ts->state = H3_TUNNEL_RESPONSE;
    भङ्ग;
  पक्ष H3_TUNNEL_ESTABLISHED:
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] -> [established]", प्रवाहः_id);
    infof(दत्त, "CONNECT%s phase completed for HTTP/3 proxy",
          ts->udp ? "-UDP" : "");
    दत्त->state.authproxy.समाप्तम् = TRUE;
    दत्त->state.authproxy.multipass = FALSE;
    ts->state = new_state;
    curlx_safefree(दत्त->req.hd_proxy_auth);
    भङ्ग;
  पक्ष H3_TUNNEL_FAILED:
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] -> [failed]", प्रवाहः_id);
    ts->state = new_state;
    /* If a proxy-authorization header was used for the proxy, then we should
       make sure that it is not accidentally used for the document request
       after we have connected. So let's free and clear it here. */
    curlx_safefree(दत्त->req.hd_proxy_auth);
    भङ्ग;
  }
}

संरचना cf_h3_proxy_ctx {
  संरचना cf_ngtcp2_ctx ngtcp2_ctx;
  संरचना h3_tunnel_प्रवाहः tunnel; /* our tunnel CONNECT stream */
  BIT(connected);
};

स्थिर CURLcode cf_ngtcp2_proxy_h3_init(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त,
                                        संरचना cf_ngtcp2_ctx *ctx);

स्थिर CURLcode cf_h3_proxy_ctx_init(संरचना cf_h3_proxy_ctx *ctx,
                                     संरचना Curl_peer *origin,
                                     संरचना Curl_peer *peer,
                                     संरचना ssl_filter_config *sslc,
                                     संरचना Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  CURLcode परिणाम;
  परिणाम = Curl_cf_ngtcp2_ctx_init(&ctx->ngtcp2_ctx, origin, peer,
                                   sslc, cf_ngtcp2_proxy_h3_init);
  यदि(!परिणाम)
    परिणाम = h3_tunnel_प्रवाहः_init(&ctx->tunnel, tunnel_peer,
                                   TRNSPRT_IS_DGRAM(tunnel_transport));
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_h3_proxy_ctx_free(संरचना cf_h3_proxy_ctx *ctx)
{
  यदि(ctx) {
    Curl_cf_ngtcp2_ctx_cleanup(&ctx->ngtcp2_ctx);
    h3_tunnel_प्रवाहः_cleanup(&ctx->tunnel);
    curlx_free(ctx);
  }
}

स्थिर पूर्णाङ्क cb_h3_proxy_acked_req_body(nghttp3_conn *conn, int64_t प्रवाहः_id,
                                      uint64_t datalen, शून्य *user_दत्त,
                                      शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  size_t skiplen;
  (शून्य)प्रवाहः_user_दत्त;

  प्रवाहः = pctx->tunnel.प्रवाहः;
  यदि(!प्रवाहः || (प्रवाहः->id != प्रवाहः_id))
    प्रत्यागम 0;

  /* The server acknowledged `datalen` of bytes from our request body.
   * This is a delta. We have kept this data in `sendbuf` for
   * re-transmissions and can free it now. */
  यदि(datalen >= (uint64_t)प्रवाहः->tx_अन्तर्_flight_आकार)
    skiplen = प्रवाहः->tx_अन्तर्_flight_आकार;
  अन्यथा
    skiplen = (size_t)datalen;
  Curl_bufq_skip(&प्रवाहः->sendbuf, skiplen);
  प्रवाहः->tx_अन्तर्_flight_आकार -= skiplen;

  /* Resume upload processing if we have more data to send */
  यदि(प्रवाहः->tx_अन्तर्_flight_आकार < Curl_bufq_len(&प्रवाहः->sendbuf)) {
    पूर्णाङ्क rv = nghttp3_conn_resume_प्रवाहः(conn, प्रवाहः_id);
    यदि(rv && rv != NGHTTP3_ERR_प्रवाहः_NOT_प्राप्तम्) {
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_proxy_प्रवाहः_close(nghttp3_conn *conn, int64_t प्रवाहः_id,
                                    uint64_t app_error_संहिता, शून्य *user_दत्त,
                                    शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  संरचना h3_प्रवाहः_ctx *प्रवाहः;

  (शून्य)conn;
  (शून्य)प्रवाहः_user_दत्त;
  यदि(!दत्त)
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;

  प्रवाहः = pctx->tunnel.प्रवाहः;
  यदि(!प्रवाहः || (प्रवाहः->id != प्रवाहः_id))
    प्रत्यागम 0;

  प्रवाहः->closed = TRUE;
  प्रवाहः->error3 = app_error_संहिता;
  यदि(प्रवाहः->error3 != NGHTTP3_H3_NO_ERROR) {
    प्रवाहः->reset = TRUE;
    प्रवाहः->send_closed = TRUE;
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                प्रवाहः->id, प्रवाहः->error3);
  }
  अन्यथा
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] CLOSED", प्रवाहः->id);
  pctx->tunnel.प्रवाहः = रिक्त;
  pctx->tunnel.closed = TRUE;
  Curl_multi_mark_dirty(दत्त);
  प्रत्यागम 0;
}

स्थिर शून्य cf_h3_proxy_upd_rx_win(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   संरचना h3_प्रवाहः_ctx *प्रवाहः)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  uint64_t cur_win, wanted_win = 0;
  size_t rx_buffered;

  यदि(!प्रवाहः) /* already closed */
    प्रत्यागम;
  यदि(प्रवाहः->rx_विस्थापनम्_max < प्रवाहः->rx_विस्थापनम्) {
    DEBUGASSERT(0);
    प्रत्यागम;
  }

  rx_buffered = Curl_bufq_len(&pctx->tunnel.recvbuf);
  wanted_win = (rx_buffered < PROXY_H3_TUNNEL_RX_WIN) ?
               (PROXY_H3_TUNNEL_RX_WIN - rx_buffered) : 0;
  cur_win = प्रवाहः->rx_विस्थापनम्_max - प्रवाहः->rx_विस्थापनम्;
  यदि(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    यदि(UINT64_MAX - delta < प्रवाहः->rx_विस्थापनम्_max)
      delta = UINT64_MAX - प्रवाहः->rx_विस्थापनम्_max;
    यदि(delta) {
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] rx window, extend %" PRIu64
                  " (buffered %zu) by %" PRIu64 " bytes",
                  प्रवाहः->id, cur_win, rx_buffered, delta);
      प्रवाहः->rx_विस्थापनम्_max += delta;
      ngtcp2_conn_extend_max_प्रवाहः_विस्थापनम्(ctx->qconn, प्रवाहः->id, delta);
      ngtcp2_conn_extend_max_विस्थापनम्(ctx->qconn, delta);
    }
  }
}

स्थिर पूर्णाङ्क cb_h3_proxy_recv_दत्त(nghttp3_conn *conn, int64_t stream3_id,
                                 ध्रुव uint8_t *buf, size_t buflen,
                                 शून्य *user_दत्त, शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  size_t nwritten;
  CURLcode परिणाम = CURLE_OK;
  (शून्य)conn;
  (शून्य)stream3_id;
  (शून्य)प्रवाहः_user_दत्त;

  प्रवाहः = pctx->tunnel.प्रवाहः;
  यदि(!दत्त || !प्रवाहः || (प्रवाहः->id != stream3_id)) {
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  प्रवाहः->rx_योग += (curl_off_t)buflen;
  CURL_TRC_CF(दत्त, cf, "[cb_h3_proxy_recv_data] "
              "[%" PRId64 "] DATA len=%zu, total=%" FMT_OFF_T,
              प्रवाहः->id, buflen, प्रवाहः->rx_योग);

  परिणाम = Curl_bufq_write(&pctx->tunnel.recvbuf, buf, buflen, &nwritten);
  यदि(परिणाम || (nwritten < buflen)) {
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  /* DATA has been moved into our local recv buffer. Update stream offsets. */
  प्रवाहः->rx_विस्थापनम् += buflen;
  यदि(प्रवाहः->rx_विस्थापनम्_max < प्रवाहः->rx_विस्थापनम्)
    प्रवाहः->rx_विस्थापनम्_max = प्रवाहः->rx_विस्थापनम्;

  CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              प्रवाहः->id, buflen, प्रवाहः->rx_विस्थापनम्_max - प्रवाहः->rx_विस्थापनम्);
  cf_h3_proxy_upd_rx_win(cf, दत्त, प्रवाहः);

  Curl_multi_mark_dirty(दत्त);
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_proxy_deferred_consume(nghttp3_conn *conn, int64_t प्रवाहः_id,
                                        size_t consumed, शून्य *user_दत्त,
                                        शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (शून्य)conn;
  (शून्य)प्रवाहः_user_दत्त;

  यदि(!ctx)
    प्रत्यागम 0;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_प्रवाहः_विस्थापनम्(ctx->qconn, प्रवाहः_id, consumed);
  ngtcp2_conn_extend_max_विस्थापनम्(ctx->qconn, consumed);

  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_proxy_recv_शीर्षकम्(nghttp3_conn *conn, int64_t प्रवाहः_id,
                                   int32_t token, nghttp3_rcbuf *name,
                                   nghttp3_rcbuf *मानम्, uint8_t flags,
                                   शून्य *user_दत्त, शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(मानम्);
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क http_स्थितिः;
  संरचना http_resp *resp;
  (शून्य)conn;
  (शून्य)प्रवाहः_id;
  (शून्य)token;
  (शून्य)flags;
  (शून्य)प्रवाहः_user_दत्त;

  /* stream_user_data might be NULL for control streams */
  यदि(!दत्त)
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;

  प्रवाहः = pctx->tunnel.प्रवाहः;
  यदि(!प्रवाहः || (प्रवाहः->id != प्रवाहः_id)) {
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] recv_header: stream lookup "
                "failed for data=%p mid=%u",
                प्रवाहः_id, (शून्य *)दत्त, दत्त ? दत्त->mid : 0);
    प्रत्यागम 0;
  }

  यदि(pctx->tunnel.has_final_response) {
    /* we do not do anything with trailers for tunnel streams */
    प्रत्यागम 0;
  }

  यदि(token == NGHTTP3_QPACK_TOKEN_स्थितिः) {
    परिणाम = Curl_http_decode_स्थितिः(&प्रवाहः->स्थितिः_संहिता,
                                     (ध्रुव वर्ण *)h3val.आधारः, h3val.len);
    यदि(परिणाम)
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    http_स्थितिः = प्रवाहः->स्थितिः_संहिता;
    परिणाम = Curl_http_resp_make(&resp, http_स्थितिः, रिक्त);
    यदि(परिणाम)
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    यदि(pctx->tunnel.resp)
      Curl_http_resp_free(pctx->tunnel.resp);
    pctx->tunnel.resp = resp;
  }
  अन्यथा {
    यदि(!pctx->tunnel.resp) {
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    }
    /* store as an HTTP1-style header */
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] header: %.*s: %.*s", प्रवाहः_id,
                (पूर्णाङ्क)h3name.len, h3name.आधारः, (पूर्णाङ्क)h3val.len, h3val.आधारः);
    परिणाम = Curl_dynhds_सङ्कलय(&pctx->tunnel.resp->शीर्षकाणि,
                             (ध्रुव वर्ण *)h3name.आधारः, h3name.len,
                             (ध्रुव वर्ण *)h3val.आधारः, h3val.len);
    यदि(परिणाम) {
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_proxy_अन्तः_शीर्षकाणि(nghttp3_conn *conn, int64_t प्रवाहः_id,
                                   पूर्णाङ्क fin, शून्य *user_दत्त,
                                   शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  (शून्य)conn;
  (शून्य)प्रवाहः_id;
  (शून्य)fin;
  (शून्य)प्रवाहः_user_दत्त;

  यदि(!दत्त)
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;

  प्रवाहः = pctx->tunnel.प्रवाहः;
  यदि(!प्रवाहः || (प्रवाहः->id != प्रवाहः_id)) {
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] end_headers: stream lookup "
                "failed for data=%p mid=%u",
                प्रवाहः_id, (शून्य *)दत्त, दत्त ? दत्त->mid : 0);
    प्रत्यागम 0;
  }

  CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] end_headers, status=%d", प्रवाहः_id,
              प्रवाहः->स्थितिः_संहिता);

  यदि(!pctx->tunnel.has_final_response) {
    यदि(प्रवाहः->स्थितिः_संहिता / 100 != 1) {
      pctx->tunnel.has_final_response = TRUE;
    }
  }

  यदि(प्रवाहः->स्थितिः_संहिता / 100 != 1) {
    प्रवाहः->resp_hds_complete = TRUE;
  }

  Curl_multi_mark_dirty(दत्त);
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_proxy_stop_sending(nghttp3_conn *conn, int64_t प्रवाहः_id,
                                    uint64_t app_error_संहिता, शून्य *user_दत्त,
                                    शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  (शून्य)conn;
  (शून्य)प्रवाहः_user_दत्त;

  यदि(ctx) {
    पूर्णाङ्क rv = ngtcp2_conn_shutdown_प्रवाहः_read(ctx->qconn, 0, प्रवाहः_id,
                                              app_error_संहिता);

    यदि(rv && rv != NGTCP2_ERR_प्रवाहः_NOT_प्राप्तम्) {
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }

  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_proxy_reset_प्रवाहः(nghttp3_conn *conn, int64_t प्रवाहः_id,
                                    uint64_t app_error_संहिता, शून्य *user_दत्त,
                                    शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  पूर्णाङ्क rv;

  (शून्य)conn;
  (शून्य)प्रवाहः_user_दत्त;
  यदि(!दत्त)
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;

  यदि(!pctx->tunnel.प्रवाहः ||
     (प्रवाहः_id != pctx->tunnel.प्रवाहः->id))
    प्रत्यागम 0;

  rv = ngtcp2_conn_shutdown_प्रवाहः_write(ctx->qconn, 0, प्रवाहः_id,
                                         app_error_संहिता);
  CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] reset -> %d", प्रवाहः_id, rv);
  pctx->tunnel.प्रवाहः = रिक्त;
  pctx->tunnel.closed = TRUE;
  यदि(rv && rv != NGTCP2_ERR_प्रवाहः_NOT_प्राप्तम्) {
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
  }
  प्रत्यागम 0;
}

स्थिर nghttp3_ssize cb_h3_tunnel_read_दत्त(nghttp3_conn *conn,
                                            int64_t प्रवाहः_id,
                                            nghttp3_vec *vec,
                                            size_t veccnt,
                                            uint32_t *pflags,
                                            शून्य *user_दत्त,
                                            शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  size_t nwritten = 0;
  size_t nvecs = 0;
  ध्रुव अचिह्नित वर्ण *buf_आधारः;

  (शून्य)conn;
  (शून्य)प्रवाहः_id;
  (शून्य)veccnt;
  (शून्य)प्रवाहः_user_दत्त;
  (शून्य)pflags;

  प्रवाहः = pctx->tunnel.प्रवाहः;
  यदि(!दत्त || !प्रवाहः || (प्रवाहः->id != प्रवाहः_id))
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;

  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_proxy_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  यदि(प्रवाहः->tx_अन्तर्_flight_आकार < Curl_bufq_len(&प्रवाहः->sendbuf)) {
    nvecs = 0;
    यावत्(nvecs < veccnt) {
      यदि(!Curl_bufq_peek_at(&प्रवाहः->sendbuf,
                           प्रवाहः->tx_अन्तर्_flight_आकार,
                           &buf_आधारः,
                           &vec[nvecs].len))
        भङ्ग;
      vec[nvecs].आधारः = (uint8_t *)(uintptr_t)buf_आधारः;
      प्रवाहः->tx_अन्तर्_flight_आकार += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been able to peek */
  }

  यदि(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] read req body -> AGAIN",
                प्रवाहः->id);
    प्रत्यागम NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu)",
              प्रवाहः->id, nvecs,
              *pflags == NGHTTP3_दत्त_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&प्रवाहः->sendbuf));
  प्रत्यागम (nghttp3_ssize)nvecs;
}

#यदि_परिभाषितम् CURL_HAVE_DIAG
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wmissing-field-initializers"
#यदि_अन्तः
स्थिर nghttp3_callbacks ngh3_proxy_callbacks = {
  cb_h3_proxy_acked_req_body, /* acked_stream_data */
  cb_h3_proxy_प्रवाहः_close,
  cb_h3_proxy_recv_दत्त,
  cb_h3_proxy_deferred_consume,
  रिक्त, /* begin_headers */
  cb_h3_proxy_recv_शीर्षकम्,
  cb_h3_proxy_अन्तः_शीर्षकाणि,
  रिक्त, /* begin_trailers */
  cb_h3_proxy_recv_शीर्षकम्,
  रिक्त, /* end_trailers */
  cb_h3_proxy_stop_sending,
  रिक्त, /* end_stream */
  cb_h3_proxy_reset_प्रवाहः,
  रिक्त, /* shutdown */
  रिक्त, /* recv_settings (deprecated) */
#यदि_परिभाषितम् NGHTTP3_CALLBACKS_V2  /* nghttp3 v1.11.0+ */
  रिक्त, /* recv_origin */
  रिक्त, /* end_origin */
  रिक्त, /* rand */
#यदि_अन्तः
#यदि_परिभाषितम् NGHTTP3_CALLBACKS_V3  /* nghttp3 v1.14.0+ */
  रिक्त, /* recv_settings2 */
#यदि_अन्तः
#यदि_परिभाषितम् NGHTTP3_CALLBACKS_V4  /* nghttp3 v1.18.0+ */
  रिक्त, /* stream_close2 */
#यदि_अन्तः
};
#यदि_परिभाषितम् CURL_HAVE_DIAG
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः

स्थिर CURLcode cf_ngtcp2_proxy_h3_init(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त,
                                        संरचना cf_ngtcp2_ctx *ctx)
{
  पूर्णाङ्क rc;

  यदि(ngtcp2_conn_get_streams_uni_वामः(ctx->qconn) < 3) {
    failf(दत्त, "QUIC connection lacks 3 uni streams to run HTTP/3");
    प्रत्यागम CURLE_QUIC_CONNECT_ERROR;
  }

  nghttp3_settings_default(&ctx->h3settings);

  rc = nghttp3_conn_client_new(&ctx->h3conn,
                               &ngh3_proxy_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  यदि(rc) {
    failf(दत्त, "error creating nghttp3 connection instance");
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  प्रत्यागम Curl_cf_ngtcp2_h3_init_ctrls(ctx, दत्त);
}

स्थिर ssize_t cf_h3_proxy_recv_closed_प्रवाहः(संरचना Curl_cfilter *cf,
                                              संरचना Curl_easy *दत्त,
                                              संरचना h3_प्रवाहः_ctx *प्रवाहः,
                                              CURLcode *err)
{
  ssize_t nread = -1;
  *err = CURLE_OK;

  यदि(प्रवाहः->reset) {
    यदि(प्रवाहः->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(दत्त, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", प्रवाहः->id);
      connclose(cf->conn);
      दत्त->state.refused_प्रवाहः = TRUE;
      *err = CURLE_RECV_ERROR;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(प्रवाहः->resp_hds_complete && दत्त->req.no_body) {
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] error after response headers, "
                  "but we did not want a body anyway, ignore error 0x%"
                  PRIx64 " %s", प्रवाहः->id, प्रवाहः->error3,
                  Curl_vquic_h3_err_str(प्रवाहः->error3));
      nread = 0;
      गच्छ बहिर्गमनम्;
    }
    failf(दत्त, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", प्रवाहः->id, प्रवाहः->error3,
          Curl_vquic_h3_err_str(प्रवाहः->error3));
    *err = दत्त->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    गच्छ बहिर्गमनम्;
  }
  अन्यथा यदि(!प्रवाहः->resp_hds_complete) {
    failf(दत्त,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          प्रवाहः->id);
    *err = CURLE_HTTP3;
    गच्छ बहिर्गमनम्;
  }
  nread = 0;

बहिर्गमनम्:
  प्रत्यागम nread;
}

स्थिर CURLcode cf_h3_proxy_sendbuf_सङ्कलय(संरचना Curl_easy *दत्त,
                                        संरचना h3_प्रवाहः_ctx *प्रवाहः,
                                        ध्रुव uint8_t *buf, size_t len,
                                        size_t *pnwritten)
{
  CURLcode परिणाम;
  *pnwritten = 0;
  (शून्य)दत्त;

  परिणाम = Curl_bufq_write(&प्रवाहः->sendbuf, buf, len, pnwritten);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h3_proxy_send(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 ध्रुव uint8_t *buf, size_t len,
                                 bool eos, size_t *pnwritten)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = रिक्त;
  संरचना cf_call_दत्त save;
  संरचना cf_ngtcp2_io_ctx pktx;
  CURLcode परिणाम = CURLE_OK;

  CF_दत्त_SAVE(save, cf, दत्त);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, दत्त);
  *pnwritten = 0;

  /* handshake verification failed in callback, do not send anything */
  यदि(ctx->tls_vrfy_परिणाम) {
    परिणाम = ctx->tls_vrfy_परिणाम;
    गच्छ denied;
  }

  (शून्य)eos; /* use for stream EOF and block handling */
  परिणाम = Curl_cf_ngtcp2_प्रगतिः_ingress(cf, दत्त, &pktx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यदि(pctx->tunnel.closed) {
    परिणाम = CURLE_SEND_ERROR;
    गच्छ denied;
  }

  प्रवाहः = pctx->tunnel.प्रवाहः;
  यदि(!प्रवाहः) {
    परिणाम = CURLE_SEND_ERROR;
    गच्छ denied;
  }

  यदि(प्रवाहः->closed) {
    यदि(प्रवाहः->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] discarding data"
                  "on closed stream with response", प्रवाहः->id);
      परिणाम = CURLE_OK;
      *pnwritten = len;
      गच्छ बहिर्गमनम्;
    }
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] send_body(len=%zu) "
                "-> stream closed", प्रवाहः->id, len);
    परिणाम = CURLE_HTTP3;
    गच्छ बहिर्गमनम्;
  }
  अन्यथा {
    परिणाम = cf_h3_proxy_sendbuf_सङ्कलय(दत्त, प्रवाहः, buf, len, pnwritten);
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] cf_send, add to "
                "sendbuf(len=%zu) -> %d, %zu",
                प्रवाहः->id, len, (पूर्णाङ्क)परिणाम, *pnwritten);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    (शून्य)nghttp3_conn_resume_प्रवाहः(ctx->h3conn, प्रवाहः->id);
  }

  यदि(*pnwritten > 0 && !ctx->tls_handshake_complete && ctx->use_earlydata)
    ctx->earlydata_skip += *pnwritten;

  DEBUGASSERT(!परिणाम);
  परिणाम = Curl_cf_ngtcp2_प्रगतिः_egress(cf, दत्त, &pktx);

बहिर्गमनम्:
  परिणाम = Curl_1st_fatal(परिणाम,
                          Curl_cf_ngtcp2_cmn_समुच्चयः_expiry(cf, दत्त, &pktx));
denied:
  CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              प्रवाहः ? प्रवाहः->id : -1, len, (पूर्णाङ्क)परिणाम, *pnwritten);
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

/* incoming data frames on the h3 stream */
स्थिर CURLcode cf_h3_proxy_recv(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 वर्ण *buf, size_t len, size_t *pnread)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = pctx->tunnel.प्रवाहः;
  संरचना cf_call_दत्त save;
  संरचना cf_ngtcp2_io_ctx pktx;
  CURLcode परिणाम = CURLE_OK;

  CF_दत्त_SAVE(save, cf, दत्त);
  DEBUGASSERT(cf->connected);
  DEBUGASSERT(ctx);
  DEBUGASSERT(ctx->qconn);
  DEBUGASSERT(ctx->h3conn);
  *pnread = 0;

  /* handshake verification failed in callback, do not recv anything */
  यदि(ctx->tls_vrfy_परिणाम) {
    परिणाम = ctx->tls_vrfy_परिणाम;
    गच्छ denied;
  }

  Curl_cf_ngtcp2_io_ctx_init(&pktx, cf, दत्त);

  यदि(!प्रवाहः || ctx->shutdown_started) {
    परिणाम = CURLE_RECV_ERROR;
    गच्छ बहिर्गमनम्;
  }

  यदि(!Curl_bufq_is_रिक्तानि(&pctx->tunnel.recvbuf)) {
    परिणाम = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    cf_h3_proxy_upd_rx_win(cf, दत्त, pctx->tunnel.प्रवाहः);
  }

  परिणाम = Curl_cf_ngtcp2_प्रगतिः_ingress(cf, दत्त, &pktx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* inbufq had nothing before, maybe after progressing ingress? */
  यदि(!*pnread && !Curl_bufq_is_रिक्तानि(&pctx->tunnel.recvbuf)) {
    परिणाम = Curl_bufq_cread(&pctx->tunnel.recvbuf, buf, len, pnread);
    यदि(परिणाम) {
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] read inbufq(len=%zu) -> %zu, %d",
                  प्रवाहः->id, len, *pnread, (पूर्णाङ्क)परिणाम);
      गच्छ बहिर्गमनम्;
    }
    cf_h3_proxy_upd_rx_win(cf, दत्त, pctx->tunnel.प्रवाहः);
  }

  यदि(*pnread) {
    Curl_multi_mark_dirty(दत्त);
  }
  अन्यथा {
    यदि(प्रवाहः->xfer_परिणाम) {
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] xfer write failed", प्रवाहः->id);
      Curl_cf_ngtcp2_h3_प्रवाहः_close(cf, दत्त, प्रवाहः);
      परिणाम = प्रवाहः->xfer_परिणाम;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(प्रवाहः->closed) {
      ssize_t nread =
        cf_h3_proxy_recv_closed_प्रवाहः(cf, दत्त, प्रवाहः, &परिणाम);
      यदि(nread > 0)
        *pnread = (size_t)nread;
      गच्छ बहिर्गमनम्;
    }
    परिणाम = CURLE_AGAIN;
  }

बहिर्गमनम्:
  परिणाम = Curl_1st_fatal(परिणाम,
                          Curl_cf_ngtcp2_प्रगतिः_egress(cf, दत्त, &pktx));
  परिणाम = Curl_1st_fatal(परिणाम,
                          Curl_cf_ngtcp2_cmn_समुच्चयः_expiry(cf, दत्त, &pktx));
denied:
  CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] cf_recv(len=%zu) -> %d, %zu",
              प्रवाहः ? प्रवाहः->id : -1, len, (पूर्णाङ्क)परिणाम, *pnread);
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h3_proxy_submit(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   संरचना h3_tunnel_प्रवाहः *ts,
                                   संरचना httpreq *req)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = रिक्त;
  संरचना dynhds h2_शीर्षकाणि;
  nghttp3_nv *nva = रिक्त;
  size_t nheader;
  पूर्णाङ्क rc = 0;
  अचिह्नित पूर्णाङ्क i;
  nghttp3_दत्त_reader reader;
  nghttp3_दत्त_reader *preader = रिक्त;
  CURLcode परिणाम;

  Curl_dynhds_init(&h2_शीर्षकाणि, 0, DYN_HTTP_REQUEST);
  परिणाम = Curl_http_req_गन्तव्यम्_h2(&h2_शीर्षकाणि, req, दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = Curl_cf_ngtcp2_h3_प्रवाहः_सज्जता(cf, दत्त, &प्रवाहः);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  nheader = Curl_dynhds_गणनम्(&h2_शीर्षकाणि);
  nva = curlx_malloc(परिमाण(nghttp3_nv) * nheader);
  यदि(!nva) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  प्रति(i = 0; i < nheader; ++i) {
    संरचना dynhds_प्रविष्टिः *e = Curl_dynhds_getn(&h2_शीर्षकाणि, i);
    nva[i].name = (अचिह्नित वर्ण *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].मानम् = (अचिह्नित वर्ण *)e->मानम्;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP3_NV_FLAG_NONE;
  }

  /* Open a bidirectional stream */
  {
    int64_t sid;
    पूर्णाङ्क rv;

    DEBUGASSERT(प्रवाहः->id == -1);
    /* Do NOT set `data` as stream user data. The transfer `data` may
     * get cleaned up long before the tunnel goes down. */
    rv = ngtcp2_conn_open_bidi_प्रवाहः(ctx->qconn, &sid, रिक्त);
    यदि(rv) {
      failf(दत्त, "cannot get bidi streams: %s", ngtcp2_strerror(rv));
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
    प्रवाहः->id = sid;
    ts->प्रवाहः = प्रवाहः;
    ++ctx->प्रयुक्तम्_bidi_streams;
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] opened bidi stream", sid);
  }

  /* CONNECT-UDP request stream remains open for capsules, no fixed EOF. */
  प्रवाहः->send_closed = 0;
  reader.read_दत्त = cb_h3_tunnel_read_दत्त;
  preader = &reader;

  rc = nghttp3_conn_submit_request(ctx->h3conn, प्रवाहः->id,
                                   nva, nheader, preader, प्रवाहः);

  यदि(rc) {
    विकल्प(rc) {
    पक्ष NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(दत्त, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", प्रवाहः->id);
      भङ्ग;
    स्वभाव:
      CURL_TRC_CF(दत्त, cf, "h3sid[%" PRId64 "] failed to send -> %d (%s)",
                  प्रवाहः->id, rc, nghttp3_strerror(rc));
      भङ्ग;
    }
    परिणाम = CURLE_SEND_ERROR;
    गच्छ बहिर्गमनम्;
  }

  यदि(Curl_trc_is_verbose(दत्त)) {
    CURL_TRC_CF(दत्त, cf, "[H3-PROXY] [%" PRId64 "] OPENED stream "
                "for %s", प्रवाहः->id,
                Curl_bufref_ptr(&दत्त->state.url));
  }

बहिर्गमनम्:
  curlx_free(nva);
  Curl_dynhds_free(&h2_शीर्षकाणि);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h3_proxy_adjust_pollset(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *दत्त,
                                           संरचना easy_pollset *ps)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_ngtcp2_ctx *ctx = &pctx->ngtcp2_ctx;
  bool want_recv, want_send;
  CURLcode परिणाम = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, दत्त);

  यदि(!ctx->qconn || !pctx->tunnel.प्रवाहः || (sock == CURL_SOCKET_BAD))
    प्रत्यागम CURLE_OK;

  Curl_pollset_check(दत्त, ps, sock, &want_recv, &want_send);

  यदि(want_recv || want_send || !Curl_bufq_is_रिक्तानि(&ctx->q.sendbuf)) {
    संरचना h3_प्रवाहः_ctx *प्रवाहः = pctx->tunnel.प्रवाहः;
    bool c_exhaust, s_exhaust;

    c_exhaust = want_send &&
                (!ngtcp2_conn_get_cwnd_वामः(ctx->qconn) ||
                 !ngtcp2_conn_get_max_दत्त_वामः(ctx->qconn));
    s_exhaust = want_send && प्रवाहः && प्रवाहः->id >= 0 &&
                प्रवाहः->quic_flow_blocked;
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_रिक्तानि(&ctx->q.sendbuf);

    परिणाम = Curl_pollset_समुच्चयः(दत्त, ps, sock, want_recv, want_send);
  }
  प्रत्यागम परिणाम;
}

स्थिर bool cf_h3_proxy_दत्त_pending(संरचना Curl_cfilter *cf,
                                     ध्रुव संरचना Curl_easy *दत्त)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  यदि(!Curl_bufq_is_रिक्तानि(&pctx->tunnel.recvbuf))
    प्रत्यागम TRUE;
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->has_दत्त_pending(cf->अग्र, दत्त) : FALSE;
}

स्थिर CURLcode cf_h3_proxy_submit_CONNECT(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *दत्त,
                                           संरचना h3_tunnel_प्रवाहः *ts)
{
  संरचना cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम;
  संरचना httpreq *req = रिक्त;

  परिणाम = Curl_http_proxy_create_tunnel_request(
    &req, cf, दत्त, ctx->ngtcp2_ctx.ssl_peer.peer,
    ts->dest, PROXY_HTTP_V3, (bool)ts->udp);
  यदि(!परिणाम)
    परिणाम = Curl_creader_समुच्चयः_null(दत्त);
  यदि(!परिणाम)
    परिणाम = cf_h3_proxy_submit(cf, दत्त, ts, req);

  यदि(req)
    Curl_http_req_free(req);
  यदि(परिणाम)
    failf(दत्त, "Failed sending CONNECT to proxy");
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h3_proxy_inspect_response(संरचना Curl_cfilter *cf,
                                             संरचना Curl_easy *दत्त,
                                             संरचना h3_tunnel_प्रवाहः *ts)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  proxy_inspect_परिणाम res;
  CURLcode परिणाम;

  परिणाम = Curl_http_proxy_inspect_tunnel_response(
      cf, दत्त, ts->resp, (bool)pctx->tunnel.udp, &res);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  विकल्प(res) {
  पक्ष PROXY_INSPECT_OK:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_ESTABLISHED, दत्त);
    भङ्ग;
  पक्ष PROXY_INSPECT_FAILED:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, दत्त);
    परिणाम = CURLE_COULDNT_CONNECT;
    भङ्ग;
  पक्ष PROXY_INSPECT_AUTH_RETRY:
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_INIT, दत्त);
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h3_proxy_tunnel(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   संरचना h3_tunnel_प्रवाहः *ts,
                                   bool *pdone)
{
  संरचना cf_h3_proxy_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(ts);
  DEBUGASSERT(ts->authority);
  *pdone = FALSE;
  कुरु {
    विकल्प(ts->state) {
    पक्ष H3_TUNNEL_INIT:
      CURL_TRC_CF(दत्त, cf, "[0] CONNECT start for %s", ts->authority);
      परिणाम = cf_h3_proxy_submit_CONNECT(cf, दत्त, ts);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      h3_tunnel_go_state(cf, ts, H3_TUNNEL_CONNECT, दत्त);

      परिणाम = Curl_cf_ngtcp2_प्रगतिः_egress(cf, दत्त, रिक्त);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      FALLTHROUGH();

    पक्ष H3_TUNNEL_CONNECT:
      /* Non-blocking: call ingress/egress once and return.
       * The multi interface will call us again when ready. */
      परिणाम = Curl_cf_ngtcp2_प्रगतिः_ingress(cf, दत्त, रिक्त);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      परिणाम = Curl_cf_ngtcp2_प्रगतिः_egress(cf, दत्त, रिक्त);
      यदि(परिणाम && परिणाम != CURLE_AGAIN) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, दत्त);
        गच्छ बहिर्गमनम्;
      }

      यदि(ts->has_final_response) {
        h3_tunnel_go_state(cf, ts, H3_TUNNEL_RESPONSE, दत्त);
      }
      अन्यथा {
        /* Not done yet, return and let multi interface call us again */
        परिणाम = CURLE_OK;
        गच्छ बहिर्गमनम्;
      }
      FALLTHROUGH();

    पक्ष H3_TUNNEL_RESPONSE:
      DEBUGASSERT(ts->has_final_response);
      परिणाम = cf_h3_proxy_inspect_response(cf, दत्त, ts);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      ctx->connected = TRUE;
      भङ्ग;

    पक्ष H3_TUNNEL_ESTABLISHED:
      *pdone = TRUE;
      प्रत्यागम CURLE_OK;

    पक्ष H3_TUNNEL_FAILED:
      प्रत्यागम CURLE_RECV_ERROR;

    स्वभाव:
      भङ्ग;
    }

  } यावत्(ts->state == H3_TUNNEL_INIT);

बहिर्गमनम्:
  यदि((परिणाम && (परिणाम != CURLE_AGAIN)) || ctx->tunnel.closed)
    h3_tunnel_go_state(cf, ts, H3_TUNNEL_FAILED, दत्त);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h3_proxy_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना cf_h3_proxy_ctx *pctx = cf->ctx;
  संरचना cf_call_दत्त save = { 0 };
  CURLcode परिणाम = CURLE_OK;
  संरचना h3_tunnel_प्रवाहः *ts = &pctx->tunnel;
  bool दत्त_saved = FALSE;

  परिणाम = Curl_cf_ngtcp2_cmn_connect(cf, दत्त, समाप्तम्);
  यदि(परिणाम || !*समाप्तम्)
    गच्छ बहिर्गमनम्;

  CF_दत्त_SAVE(save, cf, दत्त);
  दत्त_saved = TRUE;

  /* At this point the QUIC is connected, but the proxy is not connected */
  परिणाम = cf_h3_proxy_tunnel(cf, दत्त, ts, समाप्तम्);

बहिर्गमनम्:
  यदि(*समाप्तम्) {
    cf->connected = TRUE;
    /* The real request will follow the CONNECT, reset request partially */
    Curl_req_soft_reset(&दत्त->req, दत्त);
    Curl_client_reset(दत्त);
  }

  यदि(दत्त_saved)
    CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_h3_proxy_destroy(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त)
{
  संरचना cf_h3_proxy_ctx *ctx = cf->ctx;

  (शून्य)दत्त;
  यदि(ctx) {
    CURL_TRC_CF(दत्त, cf, "cf_h3_proxy_destroy()");
    यदि(ctx->ngtcp2_ctx.qconn) {
      संरचना cf_call_दत्त save;
      CF_दत्त_SAVE(save, cf, दत्त);
      Curl_cf_ngtcp2_cmn_conn_close(cf, दत्त);
      CF_दत्त_RESTORE(cf, save);
    }
    cf_h3_proxy_ctx_free(ctx);
    cf->ctx = रिक्त;
  }
}

स्थिर CURLcode cf_h3_proxy_shutdown(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  प्रत्यागम Curl_cf_ngtcp2_cmn_shutdown(cf, दत्त, समाप्तम्);
}

संरचना Curl_cftype Curl_cft_h3_proxy = {
  "H3-PROXY",
  CF_TYPE_IP_CONNECT | CF_TYPE_PROXY | CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  cf_h3_proxy_destroy,
  cf_h3_proxy_connect,
  cf_h3_proxy_shutdown,
  cf_h3_proxy_adjust_pollset,
  cf_h3_proxy_दत्त_pending,
  cf_h3_proxy_send,
  cf_h3_proxy_recv,
  Curl_cf_def_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_ngtcp2_cmn_query,
};

CURLcode Curl_cf_ngtcp2_proxy_create(संरचना Curl_cfilter **pcf,
                                     संरचना Curl_easy *दत्त,
                                     संरचना Curl_peer *origin,
                                     संरचना Curl_peer *peer,
                                     uint8_t transport_peer,
                                     संरचना connectdata *conn,
                                     संरचना Curl_sockaddr_ex *addr,
                                     संरचना Curl_peer *tunnel_peer,
                                     uint8_t tunnel_transport)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_h3_proxy_ctx *ctx;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  यदि(!tunnel_peer)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि((transport_peer != TRNSPRT_QUIC) || (!conn->http_proxy.peer))
    प्रत्यागम CURLE_FAILED_INIT;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  परिणाम = cf_h3_proxy_ctx_init(ctx, origin, peer, &conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  cf->conn = conn;

  परिणाम = Curl_cf_udp_create(&cf->अग्र, दत्त, origin, peer, TRNSPRT_QUIC,
                              conn, addr, रिक्त, TRNSPRT_QUIC);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  cf->अग्र->conn = cf->conn;
  cf->अग्र->sockindex = cf->sockindex;

बहिर्गमनम्:
  *pcf = (!परिणाम) ? cf : रिक्त;
  यदि(परिणाम) {
    यदि(cf)
      Curl_conn_cf_discard_chain(&cf, दत्त);
    अन्यथा यदि(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  अन्यथा
    CURL_TRC_CF(दत्त, cf, "created, udp_tunnel=%d", ctx->tunnel.udp);
  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_ngtcp2_proxy_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                           संरचना Curl_easy *दत्त,
                                           संरचना Curl_peer *origin,
                                           संरचना Curl_peer *peer,
                                           संरचना Curl_peer *tunnel_peer,
                                           uint8_t tunnel_transport)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_h3_proxy_ctx *ctx;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  (शून्य)दत्त;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx)
    गच्छ बहिर्गमनम्;
  परिणाम = cf_h3_proxy_ctx_init(ctx, origin, peer,
                                &cf_at->conn->proxy_ssl_config,
                                tunnel_peer, tunnel_transport);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_h3_proxy, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* H3-PROXY uses the UDP socket created by happy eyeballs below it.
     Curl_conn_cf_insert_after chains the existing sub-filters, i.e.
     "HAPPY-EYEBALLS -> UDP" as cf->next of H3-PROXY. */
  Curl_conn_cf_निवेशय_after(cf_at, cf);

बहिर्गमनम्:
  यदि(परिणाम) {
    यदि(cf)
      Curl_conn_cf_discard_chain(&cf, दत्त);
    अन्यथा यदि(ctx)
      cf_h3_proxy_ctx_free(ctx);
  }
  प्रत्यागम परिणाम;
}

#यदि_अन्तः
