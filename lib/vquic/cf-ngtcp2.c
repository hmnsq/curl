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

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "u32_ptrset.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-socket.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "curlx/win32-fopen.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "http1.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "vquic/vquic_int.h"
#अन्तर्भाव "vquic/cf-ngtcp2-cmn.h"
#अन्तर्भाव "vquic/cf-ngtcp2.h"


स्थिर पूर्णाङ्क cb_h3_acked_req_body(nghttp3_conn *conn, int64_t प्रवाहः_id,
                                uint64_t datalen, शून्य *user_दत्त,
                                शून्य *प्रवाहः_user_दत्त);

स्थिर CURLcode cf_ngtcp2_adjust_pollset(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *दत्त,
                                         संरचना easy_pollset *ps)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode परिणाम = CURLE_OK;
  curl_socket_t sock = (ctx->q.sockfd != CURL_SOCKET_BAD) ?
   ctx->q.sockfd : Curl_conn_cf_get_socket(cf, दत्त);

  यदि(!ctx->qconn || (sock == CURL_SOCKET_BAD))
    प्रत्यागम CURLE_OK;

  Curl_pollset_check(दत्त, ps, sock, &want_recv, &want_send);
  यदि(!want_send && !Curl_bufq_is_रिक्तानि(&ctx->q.sendbuf))
    want_send = TRUE;

  यदि(want_recv || want_send) {
    संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
    संरचना cf_call_दत्त save;
    bool c_exhaust, s_exhaust;

    CF_दत्त_SAVE(save, cf, दत्त);
    c_exhaust = want_send && (!ngtcp2_conn_get_cwnd_वामः(ctx->qconn) ||
                              !ngtcp2_conn_get_max_दत्त_वामः(ctx->qconn));
    s_exhaust = want_send && प्रवाहः && प्रवाहः->id >= 0 &&
                (प्रवाहः->quic_flow_blocked ||
                 प्रवाहः->tx_अन्तर्_flight_आकार >= प्रवाहः->tx_अन्तर्_flight_ideal);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_रिक्तानि(&ctx->q.sendbuf);

    परिणाम = Curl_pollset_समुच्चयः(दत्त, ps, sock, want_recv, want_send);
    CF_दत्त_RESTORE(cf, save);
  }
  प्रत्यागम परिणाम;
}

स्थिर पूर्णाङ्क cb_h3_प्रवाहः_close(nghttp3_conn *conn, int64_t प्रवाहः_id,
                              uint64_t app_error_संहिता, शून्य *user_दत्त,
                              शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  संरचना Curl_easy *दत्त =
    Curl_cf_ngtcp2_get_xfer(cf, प्रवाहः_user_दत्त, &प्रवाहः);
  (शून्य)conn;
  (शून्य)प्रवाहः_id;

  /* we might be called by nghttp3 after we already cleaned up */
  यदि(!दत्त || !प्रवाहः)
    प्रत्यागम 0;

  प्रवाहः->closed = TRUE;
  प्रवाहः->error3 = app_error_संहिता;
  यदि(प्रवाहः->error3 != NGHTTP3_H3_NO_ERROR) {
    प्रवाहः->reset = TRUE;
    प्रवाहः->send_closed = TRUE;
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] RESET: error %" PRIu64,
                प्रवाहः->id, प्रवाहः->error3);
  }
  अन्यथा {
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] CLOSED", प्रवाहः->id);
  }
  Curl_multi_mark_dirty(दत्त);
  प्रत्यागम 0;
}

स्थिर शून्य h3_xfer_write_resp_hd(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  संरचना h3_प्रवाहः_ctx *प्रवाहः,
                                  ध्रुव वर्ण *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  यदि(!प्रवाहः->xfer_परिणाम) {
    प्रवाहः->xfer_परिणाम = Curl_xfer_write_resp_hd(दत्त, buf, buflen, eos);
    यदि(प्रवाहः->xfer_परिणाम)
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] error %d writing %zu "
                  "bytes of headers", प्रवाहः->id, (पूर्णाङ्क)प्रवाहः->xfer_परिणाम,
                  buflen);
  }
}

स्थिर शून्य h3_xfer_write_resp(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               संरचना h3_प्रवाहः_ctx *प्रवाहः,
                               ध्रुव वर्ण *buf, size_t buflen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  यदि(!प्रवाहः->xfer_परिणाम) {
    प्रवाहः->xfer_परिणाम = Curl_xfer_write_resp(दत्त, buf, buflen, eos);
    /* If the transfer write is errored, we do not want any more data */
    यदि(प्रवाहः->xfer_परिणाम) {
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] error %d writing %zu bytes of data",
                  प्रवाहः->id, (पूर्णाङ्क)प्रवाहः->xfer_परिणाम, buflen);
    }
  }
}

स्थिर शून्य cf_ngtcp2_upd_rx_win(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना h3_प्रवाहः_ctx *प्रवाहः)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  uint64_t cur_win, wanted_win = H3_प्रवाहः_गवाक्षः_आकार_MAX;

  /* how much does rate limiting allow us to acknowledge? */
  यदि(Curl_rlimit_सक्रिय(&दत्त->प्रगतिः.dl.rlimit)) {
    int64_t avail;

    /* start rate limit updates only after first bytes arrived */
    यदि(!प्रवाहः->rx_विस्थापनम्)
      प्रत्यागम;

    avail = Curl_rlimit_avail(&दत्त->प्रगतिः.dl.rlimit, रिक्त);
    यदि(avail <= 0) {
      /* nothing available, do not extend the rx offset */
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] dl rate limit exhausted (%" PRId64
                  " tokens)", प्रवाहः->id, avail);
      प्रत्यागम;
    }
    wanted_win = CURLMIN((uint64_t)avail, H3_प्रवाहः_गवाक्षः_आकार_MAX);
  }

  यदि(प्रवाहः->rx_विस्थापनम्_max < प्रवाहः->rx_विस्थापनम्) {
    DEBUGASSERT(0);
    प्रत्यागम;
  }
  cur_win = प्रवाहः->rx_विस्थापनम्_max - प्रवाहः->rx_विस्थापनम्;

  यदि(wanted_win > cur_win) {
    uint64_t delta = wanted_win - cur_win;

    यदि(UINT64_MAX - delta < प्रवाहः->rx_विस्थापनम्_max)
      delta = UINT64_MAX - प्रवाहः->rx_विस्थापनम्_max;
    यदि(delta) {
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] rx window, extend by %" PRIu64
                  " bytes", प्रवाहः->id, delta);
      प्रवाहः->rx_विस्थापनम्_max += delta;
      ngtcp2_conn_extend_max_प्रवाहः_विस्थापनम्(ctx->qconn, प्रवाहः->id, delta);
    }
  }
}

स्थिर पूर्णाङ्क cb_h3_recv_दत्त(nghttp3_conn *conn, int64_t stream3_id,
                           ध्रुव uint8_t *buf, size_t buflen,
                           शून्य *user_दत्त, शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  संरचना Curl_easy *दत्त =
    Curl_cf_ngtcp2_get_xfer(cf, प्रवाहः_user_दत्त, &प्रवाहः);

  (शून्य)conn;
  (शून्य)stream3_id;

  यदि(!प्रवाहः || !दत्त)
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;

  h3_xfer_write_resp(cf, दत्त, प्रवाहः, (ध्रुव वर्ण *)buf, buflen, FALSE);

  ngtcp2_conn_extend_max_विस्थापनम्(ctx->qconn, buflen);
  प्रवाहः->rx_विस्थापनम् += buflen;
  यदि(प्रवाहः->rx_विस्थापनम्_max < प्रवाहः->rx_विस्थापनम्)
    प्रवाहः->rx_विस्थापनम्_max = प्रवाहः->rx_विस्थापनम्;

  CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] DATA len=%zu, rx win=%" PRIu64,
              प्रवाहः->id, buflen, प्रवाहः->rx_विस्थापनम्_max - प्रवाहः->rx_विस्थापनम्);
  cf_ngtcp2_upd_rx_win(cf, दत्त, प्रवाहः);
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_deferred_consume(nghttp3_conn *conn, int64_t stream3_id,
                                  size_t consumed, शून्य *user_दत्त,
                                  शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = प्रवाहः_user_दत्त;
  (शून्य)conn;

  /* nghttp3 has consumed bytes on the QUIC stream and we need to
   * tell the QUIC connection to increase its flow control */
  ngtcp2_conn_extend_max_प्रवाहः_विस्थापनम्(ctx->qconn, stream3_id, consumed);
  ngtcp2_conn_extend_max_विस्थापनम्(ctx->qconn, consumed);

  यदि(प्रवाहः) {
    प्रवाहः->rx_विस्थापनम् += consumed;
    प्रवाहः->rx_विस्थापनम्_max += consumed;
  }
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_अन्तः_शीर्षकाणि(nghttp3_conn *conn, int64_t प्रवाहः_id,
                             पूर्णाङ्क fin, शून्य *user_दत्त, शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  संरचना Curl_easy *दत्त =
    Curl_cf_ngtcp2_get_xfer(cf, प्रवाहः_user_दत्त, &प्रवाहः);
  (शून्य)conn;
  (शून्य)प्रवाहः_id;
  (शून्य)fin;
  (शून्य)cf;

  यदि(!प्रवाहः || !दत्त)
    प्रत्यागम 0;
  /* add a CRLF only if we have received some headers */
  h3_xfer_write_resp_hd(cf, दत्त, प्रवाहः, STRCONST("\r\n"),
                        (bool)प्रवाहः->closed);

  CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] end_headers, status=%d",
              प्रवाहः_id, प्रवाहः->स्थितिः_संहिता);
  यदि(प्रवाहः->स्थितिः_संहिता / 100 != 1) {
    प्रवाहः->resp_hds_complete = TRUE;
  }
  Curl_multi_mark_dirty(दत्त);
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_recv_शीर्षकम्(nghttp3_conn *conn, int64_t प्रवाहः_id,
                             int32_t token, nghttp3_rcbuf *name,
                             nghttp3_rcbuf *मानम्, uint8_t flags,
                             शून्य *user_दत्त, शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  nghttp3_vec h3name = nghttp3_rcbuf_get_buf(name);
  nghttp3_vec h3val = nghttp3_rcbuf_get_buf(मानम्);
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  संरचना Curl_easy *दत्त =
    Curl_cf_ngtcp2_get_xfer(cf, प्रवाहः_user_दत्त, &प्रवाहः);
  CURLcode परिणाम = CURLE_OK;
  (शून्य)conn;
  (शून्य)प्रवाहः_id;
  (शून्य)token;
  (शून्य)flags;
  (शून्य)cf;

  /* we might have cleaned up this transfer already */
  यदि(!प्रवाहः || !दत्त)
    प्रत्यागम 0;

  यदि(token == NGHTTP3_QPACK_TOKEN_स्थितिः) {

    परिणाम = Curl_http_decode_स्थितिः(&प्रवाहः->स्थितिः_संहिता,
                                     (ध्रुव वर्ण *)h3val.आधारः, h3val.len);
    यदि(परिणाम)
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    curlx_dyn_reset(&ctx->scratch);
    परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/3 "));
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch,
                              (ध्रुव वर्ण *)h3val.आधारः, h3val.len);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    यदि(!परिणाम)
      h3_xfer_write_resp_hd(cf, दत्त, प्रवाहः, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] status: %s",
                प्रवाहः_id, curlx_dyn_ptr(&ctx->scratch));
    यदि(परिणाम) {
      प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
    }
  }
  अन्यथा {
    /* store as an HTTP1-style header */
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] header: %.*s: %.*s",
                प्रवाहः_id, (पूर्णाङ्क)h3name.len, h3name.आधारः,
                (पूर्णाङ्क)h3val.len, h3val.आधारः);
    curlx_dyn_reset(&ctx->scratch);
    परिणाम = curlx_dyn_addn(&ctx->scratch,
                            (ध्रुव वर्ण *)h3name.आधारः, h3name.len);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch,
                              (ध्रुव वर्ण *)h3val.आधारः, h3val.len);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
    यदि(!परिणाम)
      h3_xfer_write_resp_hd(cf, दत्त, प्रवाहः, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
  }
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_stop_sending(nghttp3_conn *conn, int64_t प्रवाहः_id,
                              uint64_t app_error_संहिता, शून्य *user_दत्त,
                              शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  पूर्णाङ्क rv;
  (शून्य)conn;
  (शून्य)प्रवाहः_user_दत्त;

  rv = ngtcp2_conn_shutdown_प्रवाहः_read(ctx->qconn, 0, प्रवाहः_id,
                                        app_error_संहिता);
  यदि(rv && rv != NGTCP2_ERR_प्रवाहः_NOT_प्राप्तम्) {
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क cb_h3_reset_प्रवाहः(nghttp3_conn *conn, int64_t प्रवाहः_id,
                              uint64_t app_error_संहिता, शून्य *user_दत्त,
                              शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त = Curl_cf_ngtcp2_get_xfer(cf, प्रवाहः_user_दत्त, रिक्त);
  पूर्णाङ्क rv;
  (शून्य)conn;

  rv = ngtcp2_conn_shutdown_प्रवाहः_write(ctx->qconn, 0, प्रवाहः_id,
                                         app_error_संहिता);
  यदि(दत्त)
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] reset -> %d", प्रवाहः_id, rv);
  यदि(rv && rv != NGTCP2_ERR_प्रवाहः_NOT_प्राप्तम्) {
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
  }

  प्रत्यागम 0;
}

#यदि_परिभाषितम् CURL_HAVE_DIAG
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wmissing-field-initializers"
#यदि_अन्तः
स्थिर nghttp3_callbacks ngh3_callbacks = {
  cb_h3_acked_req_body, /* acked_stream_data */
  cb_h3_प्रवाहः_close,
  cb_h3_recv_दत्त,
  cb_h3_deferred_consume,
  रिक्त, /* begin_headers */
  cb_h3_recv_शीर्षकम्,
  cb_h3_अन्तः_शीर्षकाणि,
  रिक्त, /* begin_trailers */
  cb_h3_recv_शीर्षकम्,
  रिक्त, /* end_trailers */
  cb_h3_stop_sending,
  रिक्त, /* end_stream */
  cb_h3_reset_प्रवाहः,
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

स्थिर CURLcode init_ngh3_conn(संरचना Curl_cfilter *cf,
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
                               &ngh3_callbacks,
                               &ctx->h3settings,
                               Curl_nghttp3_mem(),
                               cf);
  यदि(rc) {
    failf(दत्त, "error creating nghttp3 connection instance");
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  प्रत्यागम Curl_cf_ngtcp2_h3_init_ctrls(ctx, दत्त);
}

स्थिर CURLcode recv_closed_प्रवाहः(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   संरचना h3_प्रवाहः_ctx *प्रवाहः,
                                   size_t *pnread)
{
  (शून्य)cf;
  *pnread = 0;
  यदि(प्रवाहः->reset) {
    यदि(प्रवाहः->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(दत्त, "HTTP/3 stream %" PRId64 " refused by server, try again "
            "on a new connection", प्रवाहः->id);
      connclose(cf->conn); /* do not use this anymore */
      दत्त->state.refused_प्रवाहः = TRUE;
      प्रत्यागम CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    अन्यथा यदि(प्रवाहः->resp_hds_complete && दत्त->req.no_body) {
        CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", प्रवाहः->id, प्रवाहः->error3,
                    Curl_vquic_h3_err_str(प्रवाहः->error3));
        प्रत्यागम CURLE_OK;
    }
    failf(दत्त, "HTTP/3 stream %" PRId64 " reset by server (error 0x%" PRIx64
          " %s)", प्रवाहः->id, प्रवाहः->error3,
          Curl_vquic_h3_err_str(प्रवाहः->error3));
    प्रत्यागम दत्त->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
  }
  अन्यथा यदि(!प्रवाहः->resp_hds_complete) {
    failf(दत्त,
          "HTTP/3 stream %" PRId64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          प्रवाहः->id);
    प्रत्यागम CURLE_HTTP3;
  }
  प्रत्यागम CURLE_OK;
}

/* incoming data frames on the h3 stream */
स्थिर CURLcode cf_ngtcp2_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                               वर्ण *buf, size_t buflen, size_t *pnread)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
  संरचना cf_call_दत्त save;
  संरचना cf_ngtcp2_io_ctx pktx;
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क i;

  (शून्य)ctx;
  (शून्य)buf;
  NOVERBOSE((शून्य)buflen);

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

  cf_ngtcp2_upd_rx_win(cf, दत्त, प्रवाहः);

  /* first check for results/closed already known without touching
   * the connection. For an already failed/closed stream, errors on
   * the connection do not count.
   * Then handle incoming data and check for failed/closed again.
   */
  प्रति(i = 0; i < 2; ++i) {
    यदि(प्रवाहः->xfer_परिणाम) {
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] xfer write failed", प्रवाहः->id);
      Curl_cf_ngtcp2_h3_प्रवाहः_close(cf, दत्त, प्रवाहः);
      परिणाम = प्रवाहः->xfer_परिणाम;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(प्रवाहः->closed) {
      परिणाम = recv_closed_प्रवाहः(cf, दत्त, प्रवाहः, pnread);
      गच्छ बहिर्गमनम्;
    }

    यदि(!i && Curl_cf_ngtcp2_प्रगतिः_ingress(cf, दत्त, &pktx)) {
      परिणाम = CURLE_RECV_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }

  परिणाम = CURLE_AGAIN;

बहिर्गमनम्:
  परिणाम = Curl_1st_fatal(परिणाम,
                          Curl_cf_ngtcp2_प्रगतिः_egress(cf, दत्त, &pktx));
  परिणाम = Curl_1st_fatal(परिणाम,
                          Curl_cf_ngtcp2_cmn_समुच्चयः_expiry(cf, दत्त, &pktx));
  यदि(ctx->tls_vrfy_परिणाम)
    परिणाम = ctx->tls_vrfy_परिणाम;
denied:
  CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] cf_recv(buflen=%zu) -> %d, %zu",
              प्रवाहः ? प्रवाहः->id : -1, buflen, (पूर्णाङ्क)परिणाम, *pnread);
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर पूर्णाङ्क cb_h3_acked_req_body(nghttp3_conn *conn, int64_t प्रवाहः_id,
                                uint64_t datalen, शून्य *user_दत्त,
                                शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = प्रवाहः_user_दत्त;
  size_t skiplen;

  (शून्य)cf;
  यदि(!प्रवाहः)
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

स्थिर nghttp3_ssize cb_h3_read_req_body(nghttp3_conn *conn, int64_t प्रवाहः_id,
                                         nghttp3_vec *vec, size_t veccnt,
                                         uint32_t *pflags, शून्य *user_दत्त,
                                         शून्य *प्रवाहः_user_दत्त)
{
  संरचना Curl_cfilter *cf = user_दत्त;
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
  संरचना Curl_easy *दत्त =
    Curl_cf_ngtcp2_get_xfer(cf, प्रवाहः_user_दत्त, &प्रवाहः);
  size_t nwritten = 0;
  size_t nvecs = 0;
  (शून्य)cf;
  (शून्य)conn;
  (शून्य)प्रवाहः_id;
  (शून्य)user_दत्त;
  (शून्य)veccnt;

  यदि(!प्रवाहः || !दत्त)
    प्रत्यागम NGHTTP3_ERR_CALLBACK_FAILURE;
  /* nghttp3 keeps references to the sendbuf data until it is ACKed
   * by the server (see `cb_h3_acked_req_body()` for updates).
   * `tx_in_flight_size` is the amount of bytes in `sendbuf`
   * that we have already passed to nghttp3, but which have not been
   * ACKed yet.
   * Any amount beyond `tx_in_flight_size` we need still to pass
   * to nghttp3. Do that now, if we can. */
  यदि(प्रवाहः->tx_अन्तर्_flight_आकार < Curl_bufq_len(&प्रवाहः->sendbuf)) {
    nvecs = 0;
    यावत्(nvecs < veccnt &&
          Curl_bufq_peek_at(&प्रवाहः->sendbuf,
                            प्रवाहः->tx_अन्तर्_flight_आकार,
                            CURL_UNCONST(&vec[nvecs].आधारः),
                            &vec[nvecs].len)) {
      प्रवाहः->tx_अन्तर्_flight_आकार += vec[nvecs].len;
      nwritten += vec[nvecs].len;
      ++nvecs;
    }
    DEBUGASSERT(nvecs > 0); /* we SHOULD have been be able to peek */
  }

  यदि(nwritten > 0 && प्रवाहः->upload_वामः != -1)
    प्रवाहः->upload_वामः -= nwritten;

  /* When we stopped sending and everything in `sendbuf` is "in flight",
   * we are at the end of the request body. */
  यदि(प्रवाहः->upload_वामः == 0) {
    *pflags = NGHTTP3_दत्त_FLAG_EOF;
    प्रवाहः->send_closed = TRUE;
  }
  अन्यथा यदि(!nwritten) {
    /* Not EOF, and nothing to give, we signal WOULDBLOCK. */
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] read req body -> AGAIN", प्रवाहः->id);
    प्रत्यागम NGHTTP3_ERR_WOULDBLOCK;
  }

  CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] read req body -> "
              "%zu vecs%s with %zu (buffered=%zu, left=%" FMT_OFF_T ")",
              प्रवाहः->id, nvecs,
              *pflags == NGHTTP3_दत्त_FLAG_EOF ? " EOF" : "",
              nwritten, Curl_bufq_len(&प्रवाहः->sendbuf),
              प्रवाहः->upload_वामः);
  प्रत्यागम (nghttp3_ssize)nvecs;
}

स्थिर CURLcode h3_प्रवाहः_open(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               ध्रुव uint8_t *buf, size_t len,
                               size_t *pnwritten)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = रिक्त;
  int64_t sid;
  संरचना dynhds h2_शीर्षकाणि;
  size_t nheader;
  nghttp3_nv *nva = रिक्त;
  पूर्णाङ्क rc = 0;
  अचिह्नित पूर्णाङ्क i;
  nghttp3_दत्त_reader reader;
  nghttp3_दत्त_reader *preader = रिक्त;
  CURLcode परिणाम;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_शीर्षकाणि, 0, DYN_HTTP_REQUEST);

  परिणाम = Curl_cf_ngtcp2_h3_प्रवाहः_सज्जता(cf, दत्त, &प्रवाहः);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = Curl_h1_req_parse_read(&प्रवाहः->h1, buf, len, रिक्त,
    !दत्त->state.http_ignorecustom ?
    CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST) : रिक्त,
    0, pnwritten);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  यदि(!प्रवाहः->h1.समाप्तम्) {
    /* need more data */
    गच्छ बहिर्गमनम्;
  }
  DEBUGASSERT(प्रवाहः->h1.req);

  परिणाम = Curl_http_req_गन्तव्यम्_h2(&h2_शीर्षकाणि, प्रवाहः->h1.req, दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* no longer needed */
  Curl_h1_req_parse_free(&प्रवाहः->h1);

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

  rc = ngtcp2_conn_open_bidi_प्रवाहः(ctx->qconn, &sid, प्रवाहः);
  यदि(rc) {
    failf(दत्त, "cannot open bidi streams");
    परिणाम = CURLE_SEND_ERROR;
    गच्छ बहिर्गमनम्;
  }
  प्रवाहः->id = sid;
  ++ctx->प्रयुक्तम्_bidi_streams;

  विकल्प(दत्त->state.httpreq) {
  पक्ष HTTPREQ_POST:
  पक्ष HTTPREQ_POST_FORM:
  पक्ष HTTPREQ_POST_MIME:
  पक्ष HTTPREQ_PUT:
    /* known request body size or -1 */
    यदि(दत्त->state.infilesize != -1)
      प्रवाहः->upload_वामः = दत्त->state.infilesize;
    अन्यथा
      /* data sending without specifying the data amount up front */
      प्रवाहः->upload_वामः = -1; /* unknown */
    भङ्ग;
  स्वभाव:
    /* there is no request body */
    प्रवाहः->upload_वामः = 0; /* no request body */
    भङ्ग;
  }

  प्रवाहः->send_closed = (प्रवाहः->upload_वामः == 0);
  यदि(!प्रवाहः->send_closed) {
    reader.read_दत्त = cb_h3_read_req_body;
    preader = &reader;
  }

  rc = nghttp3_conn_submit_request(ctx->h3conn, प्रवाहः->id,
                                   nva, nheader, preader, प्रवाहः);
  यदि(rc) {
    विकल्प(rc) {
    पक्ष NGHTTP3_ERR_CONN_CLOSING:
      CURL_TRC_CF(दत्त, cf, "h3sid[%" PRId64 "] failed to send, "
                  "connection is closing", प्रवाहः->id);
      भङ्ग;
    स्वभाव:
      CURL_TRC_CF(दत्त, cf, "h3sid[%" PRId64 "] failed to send -> "
                  "%d (%s)", प्रवाहः->id, rc, nghttp3_strerror(rc));
      भङ्ग;
    }
    Curl_cf_ngtcp2_h3_प्रवाहः_close(cf, दत्त, प्रवाहः);
    परिणाम = CURLE_SEND_ERROR;
    गच्छ बहिर्गमनम्;
  }

  cf_ngtcp2_upd_rx_win(cf, दत्त, प्रवाहः);

  यदि(Curl_trc_is_verbose(दत्त)) {
    infof(दत्त, "[HTTP/3] [%" PRId64 "] OPENED stream for %s",
          प्रवाहः->id, Curl_bufref_ptr(&दत्त->state.url));
    प्रति(i = 0; i < nheader; ++i) {
      infof(दत्त, "[HTTP/3] [%" PRId64 "] [%.*s: %.*s]", प्रवाहः->id,
            (पूर्णाङ्क)nva[i].namelen, nva[i].name,
            (पूर्णाङ्क)nva[i].valuelen, nva[i].मानम्);
    }
  }

बहिर्गमनम्:
  curlx_free(nva);
  Curl_dynhds_free(&h2_शीर्षकाणि);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_ngtcp2_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                               ध्रुव uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
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

  यदि(!प्रवाहः || प्रवाहः->id < 0) {
    यदि(ctx->shutdown_started) {
      CURL_TRC_CF(दत्त, cf, "cannot open stream on closed connection");
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
    परिणाम = h3_प्रवाहः_open(cf, दत्त, buf, len, pnwritten);
    यदि(परिणाम) {
      CURL_TRC_CF(दत्त, cf, "failed to open stream -> %d", (पूर्णाङ्क)परिणाम);
      गच्छ बहिर्गमनम्;
    }
    प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
  }
  अन्यथा यदि(प्रवाहः->xfer_परिणाम) {
    CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] xfer write failed", प्रवाहः->id);
    Curl_cf_ngtcp2_h3_प्रवाहः_close(cf, दत्त, प्रवाहः);
    परिणाम = प्रवाहः->xfer_परिणाम;
    गच्छ बहिर्गमनम्;
  }
  अन्यथा यदि(प्रवाहः->closed) {
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
  अन्यथा यदि(ctx->shutdown_started) {
    CURL_TRC_CF(दत्त, cf, "cannot send on closed connection");
    परिणाम = CURLE_SEND_ERROR;
    गच्छ बहिर्गमनम्;
  }
  अन्यथा यदि(प्रवाहः->tx_अन्तर्_flight_आकार >= प्रवाहः->tx_अन्तर्_flight_ideal) {
    परिणाम = CURLE_AGAIN;
    गच्छ बहिर्गमनम्;
  }
  अन्यथा {
    परिणाम = Curl_bufq_write(&प्रवाहः->sendbuf, buf, len, pnwritten);
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

  यदि(*pnwritten > 0) {
    ध्रुव size_t delta = (2 * 1024);
    यदि(प्रवाहः->tx_अन्तर्_flight_आकार > (प्रवाहः->tx_अन्तर्_flight_ideal + delta)) {
      /* ngtcp2 keeps more in flight than we try to provide for,
       * increase our "ideal" tx buffer length */
      प्रवाहः->tx_अन्तर्_flight_ideal += delta;
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, increase tx_win=%zu",
                  प्रवाहः->id, len, *pnwritten,
                  प्रवाहः->tx_अन्तर्_flight_आकार, प्रवाहः->tx_अन्तर्_flight_ideal);
    }
    अन्यथा यदि((प्रवाहः->tx_अन्तर्_flight_आकार + delta) < प्रवाहः->tx_अन्तर्_flight_ideal) {
      /* ngtcp2 keeps less in flight than we try to provide for,
       * decrease our "ideal" tx buffer length */
      प्रवाहः->tx_अन्तर्_flight_ideal -= delta;
      CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] cf_send(%zu), added %zu, "
                  "inflight=%zu, reduce tx_win=%zu",
                  प्रवाहः->id, len, *pnwritten,
                  प्रवाहः->tx_अन्तर्_flight_आकार, प्रवाहः->tx_अन्तर्_flight_ideal);
    }
  }

बहिर्गमनम्:
  परिणाम = Curl_1st_fatal(परिणाम,
                          Curl_cf_ngtcp2_cmn_समुच्चयः_expiry(cf, दत्त, &pktx));
  यदि(ctx->tls_vrfy_परिणाम)
    परिणाम = ctx->tls_vrfy_परिणाम;
denied:
  CURL_TRC_CF(दत्त, cf, "[%" PRId64 "] cf_send(len=%zu) -> %d, %zu",
              प्रवाहः ? प्रवाहः->id : -1, len, (पूर्णाङ्क)परिणाम, *pnwritten);
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode h3_दत्त_pause(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              bool pause)
{
  /* There seems to exist no API in ngtcp2 to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (शून्य)cf;
  यदि(!pause)
    Curl_multi_mark_dirty(दत्त);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_ngtcp2_cntrl(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  संरचना cf_call_दत्त save;

  CF_दत्त_SAVE(save, cf, दत्त);
  विकल्प(event) {
  पक्ष CF_CTRL_दत्त_सज्जता:
    भङ्ग;
  पक्ष CF_CTRL_दत्त_PAUSE:
    परिणाम = h3_दत्त_pause(cf, दत्त, (arg1 != 0));
    भङ्ग;
  पक्ष CF_CTRL_दत्त_समाप्तम्:
    Curl_cf_ngtcp2_h3_प्रवाहः_समाप्तम्(cf, दत्त);
    भङ्ग;
  पक्ष CF_CTRL_दत्त_समाप्तम्_SEND: {
    संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
    यदि(प्रवाहः && !प्रवाहः->send_closed) {
      प्रवाहः->send_closed = TRUE;
      प्रवाहः->upload_वामः = Curl_bufq_len(&प्रवाहः->sendbuf) -
        प्रवाहः->tx_अन्तर्_flight_आकार;
      (शून्य)nghttp3_conn_resume_प्रवाहः(ctx->h3conn, प्रवाहः->id);
    }
    भङ्ग;
  }
  पक्ष CF_CTRL_CONN_INFO_UPDATE:
    यदि(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_आलोकितम् = 30;
      Curl_conn_समुच्चयः_multiplex(cf->conn);
    }
    भङ्ग;
  स्वभाव:
    परिणाम = Curl_cf_ngtcp2_cmn_cntrl(cf, दत्त, event, arg1, arg2);
    भङ्ग;
  }
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_ngtcp2_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;

  CURL_TRC_CF(दत्त, cf, "destroy");
  यदि(ctx) {
    यदि(ctx->qconn) {
      संरचना cf_call_दत्त save;
      CF_दत्त_SAVE(save, cf, दत्त);
      Curl_cf_ngtcp2_cmn_conn_close(cf, दत्त);
      CF_दत्त_RESTORE(cf, save);
    }
    Curl_cf_ngtcp2_ctx_cleanup(ctx);
    curlx_free(ctx);
    cf->ctx = रिक्त;
  }
}

स्थिर CURLcode cf_ngtcp2_connect(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  bool *समाप्तम्)
{
  प्रत्यागम Curl_cf_ngtcp2_cmn_connect(cf, दत्त, समाप्तम्);
}

स्थिर CURLcode cf_ngtcp2_query(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_ngtcp2_ctx *ctx = cf->ctx;
  संरचना cf_call_दत्त save;

  विकल्प(query) {
  पक्ष CF_QUERY_MAX_CONCURRENT: {
    DEBUGASSERT(pres1);
    CF_दत्त_SAVE(save, cf, दत्त);
    /* Set after transport params arrived and continually updated
     * by callback. QUIC counts the number over the lifetime of the
     * connection, ever increasing.
     * We count the *open* transfers plus the budget for new ones. */
    यदि(!ctx->qconn || ctx->shutdown_started) {
      *pres1 = 0;
    }
    अन्यथा यदि(ctx->max_bidi_streams) {
      uint64_t avail_bidi_streams = 0;
      uint64_t max_streams = cf->conn->attached_xfers;
      यदि(ctx->max_bidi_streams > ctx->प्रयुक्तम्_bidi_streams)
        avail_bidi_streams = ctx->max_bidi_streams - ctx->प्रयुक्तम्_bidi_streams;
      max_streams += avail_bidi_streams;
      *pres1 = (max_streams > अधिकतम_पूर्णाङ्कः) ? अधिकतम_पूर्णाङ्कः : (पूर्णाङ्क)max_streams;
    }
    अन्यथा {  /* transport params not arrived yet? take our default. */
      uint32_t n = Curl_multi_max_concurrent_streams(दत्त->multi);
      *pres1 = (n < अधिकतम_पूर्णाङ्कः) ? (पूर्णाङ्क)n : अधिकतम_पूर्णाङ्कः;
    }
    CURL_TRC_CF(दत्त, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    CF_दत्त_RESTORE(cf, save);
    प्रत्यागम CURLE_OK;
  }
  पक्ष CF_QUERY_HTTP_संस्करणम्:
    *pres1 = 30;
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_SSL_INFO:
  पक्ष CF_QUERY_SSL_CTX_INFO: {
    संरचना curl_tlssessioninfo *info = pres2;
    यदि(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      प्रत्यागम CURLE_OK;
    भङ्ग;
  }
  पक्ष CF_QUERY_ALPN_NEGOTIATED: {
    ध्रुव वर्ण **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : रिक्त;
    प्रत्यागम CURLE_OK;
  }
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम Curl_cf_ngtcp2_cmn_query(cf, दत्त, query, pres1, pres2);
}

संरचना Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_ngtcp2_destroy,
  cf_ngtcp2_connect,
  Curl_cf_ngtcp2_cmn_shutdown,
  cf_ngtcp2_adjust_pollset,
  Curl_cf_def_दत्त_pending,
  cf_ngtcp2_send,
  cf_ngtcp2_recv,
  cf_ngtcp2_cntrl,
  Curl_cf_ngtcp2_cmn_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_ngtcp2_query,
};

CURLcode Curl_cf_ngtcp2_create(संरचना Curl_cfilter **pcf,
                               संरचना Curl_easy *दत्त,
                               संरचना Curl_peer *origin,
                               संरचना Curl_peer *peer,
                               संरचना connectdata *conn,
                               संरचना Curl_sockaddr_ex *addr)
{
  संरचना cf_ngtcp2_ctx *ctx = रिक्त;
  संरचना Curl_cfilter *cf = रिक्त;
  CURLcode परिणाम;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  परिणाम = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &conn->ssl_config, init_ngh3_conn);
  यदि(!परिणाम)
    परिणाम = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
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
    अन्यथा यदि(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_ngtcp2_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                     संरचना Curl_peer *origin,
                                     संरचना Curl_peer *peer,
                                     संरचना Curl_cfilter **pcf)
{
  संरचना cf_ngtcp2_ctx *ctx = रिक्त;
  संरचना Curl_cfilter *cf = रिक्त;
  CURLcode परिणाम;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  परिणाम = Curl_cf_ngtcp2_ctx_init(ctx, origin, peer,
                                   &cf_at->conn->ssl_config, init_ngh3_conn);
  यदि(!परिणाम)
    परिणाम = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  Curl_conn_cf_निवेशय_after(cf_at, cf);
बहिर्गमनम्:
  यदि(परिणाम) {
    curlx_safefree(cf);
    यदि(ctx) {
      Curl_cf_ngtcp2_ctx_cleanup(ctx);
      curlx_free(ctx);
    }
  }
  *pcf = cf;
  प्रत्यागम परिणाम;
}

#यदि_अन्तः
