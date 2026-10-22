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

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#अन्तर्भाव <quiche.h>
#अन्तर्भाव <openssl/err.h>
#अन्तर्भाव <openssl/ssl.h>

#अन्तर्भाव "bufq.h"
#अन्तर्भाव "u32_ptrset.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-socket.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "http1.h"
#अन्तर्भाव "sockaddr.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "vquic/vquic_int.h"
#अन्तर्भाव "vquic/vquic-tls.h"
#अन्तर्भाव "vquic/cf-quiche.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "vtls/openssl.h"
#अन्तर्भाव "vtls/vtls.h"

/* HTTP/3 error values defined in RFC 9114, ch. 8.1 */
#define CURL_H3_NO_ERROR  0x0100

#define MAX_PKT_BURST          10

#define QUIC_MAX_STREAMS       100

#define H3_STREAM_WINDOW_SIZE  (1024 * 128)
#define H3_STREAM_CHUNK_SIZE   (1024 * 16)
/* Receive and Send max number of chunks follows from the
 * chunk size and window size */
#define H3_STREAM_RECV_CHUNKS \
  (H3_STREAM_WINDOW_SIZE / H3_STREAM_CHUNK_SIZE)

/*
 * Store quiche version info in this buffer.
 */
शून्य Curl_quiche_ver(वर्ण *p, size_t len)
{
  (शून्य)curl_msnprintf(p, len, "quiche/%s", quiche_version());
}

संरचना cf_quiche_ctx {
  संरचना cf_quic_ctx q;
  संरचना ssl_peer ssl_peer;
  संरचना curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  संरचना curltime started_at;        /* time the current attempt started */
  संरचना curltime handshake_at;      /* time connect handshake finished */
  संरचना u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  संरचना dynbuf h1hdr;               /* temp buffer for header construction */
  संरचना bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t data_recvd;
  BIT(initialized);
  BIT(goaway);                       /* got GOAWAY from server */
  BIT(x509_store_setup);             /* if x509 store has been set up */
  BIT(shutdown_started);             /* queued shutdown packets */
  BIT(stats_reported);               /* connect statistics reported */
};

#ifdef DEBUG_QUICHE
/* initialize debug log callback only once */
स्थिर पूर्णाङ्क debug_log_init = 0;
स्थिर शून्य quiche_debug_log(ध्रुव वर्ण *पंक्ति, शून्य *argp)
{
  (शून्य)argp;
  curl_mfprintf(stderr, "%s\n", पंक्ति);
}
#endif

स्थिर शून्य h3_stream_set_free(uint32_t id, शून्य *stream);

स्थिर CURLcode cf_quiche_ctx_init(संरचना cf_quiche_ctx *ctx,
                                   संरचना Curl_peer *origin,
                                   संरचना Curl_peer *peer,
                                   संरचना ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#ifdef DEBUG_QUICHE
  यदि(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, रिक्त);
    debug_log_init = 1;
  }
#endif
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  Curl_bufq_init2(&ctx->writebuf, H3_STREAM_CHUNK_SIZE, H3_STREAM_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  ctx->data_recvd = 0;
  ctx->initialized = TRUE;
  प्रत्यागम Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

स्थिर शून्य cf_quiche_ctx_free(संरचना cf_quiche_ctx *ctx)
{
  यदि(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = रिक्त;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

स्थिर शून्य cf_quiche_ctx_close(संरचना cf_quiche_ctx *ctx)
{
  यदि(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = रिक्त;
  }
  यदि(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = रिक्त;
  }
  यदि(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = रिक्त;
  }
  यदि(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = रिक्त;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

स्थिर CURLcode cf_flush_egress(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data);

/**
 * All about the H3 internals of a stream
 */
संरचना h3_stream_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  संरचना h1_req_parser h1; /* h1 request parsing */
  uint64_t error3;         /* HTTP/3 stream error code */
  int32_t status_code;     /* HTTP status code */
  CURLcode xfer_result;    /* result from cf_quiche_write_(hd/body) */
  BIT(opened);             /* TRUE after stream has been opened */
  BIT(closed);             /* TRUE on stream close */
  BIT(reset);              /* TRUE on stream reset */
  BIT(send_closed);        /* stream is locally closed */
  BIT(resp_hds_complete);  /* final response has been received */
  BIT(resp_got_header);    /* TRUE when h3 stream has recvd some HEADER */
  BIT(quic_flow_blocked);  /* stream is blocked by QUIC flow control */
};

स्थिर शून्य h3_stream_ctx_free(संरचना h3_stream_ctx *stream)
{
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

स्थिर शून्य h3_stream_set_free(uint32_t id, शून्य *stream)
{
  (शून्य)id;
  DEBUGASSERT(stream);
  h3_stream_ctx_free((संरचना h3_stream_ctx *)stream);
}

प्रकारनिर्देश bool cf_quiche_svisit(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *sdata,
                              संरचना h3_stream_ctx *stream,
                              शून्य *user_data);

संरचना cf_quiche_visit_ctx {
  संरचना Curl_cfilter *cf;
  संरचना Curl_multi *multi;
  cf_quiche_svisit *cb;
  शून्य *user_data;
};

स्थिर bool cf_quiche_stream_do(uint32_t mid, शून्य *val, शून्य *user_data)
{
  संरचना cf_quiche_visit_ctx *vctx = user_data;
  संरचना h3_stream_ctx *stream = val;
  संरचना Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  यदि(sdata)
    प्रत्यागम vctx->cb(vctx->cf, sdata, stream, vctx->user_data);
  प्रत्यागम TRUE;
}

स्थिर शून्य cf_quiche_for_all_streams(संरचना Curl_cfilter *cf,
                                      संरचना Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      शून्य *user_data)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना cf_quiche_visit_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_data = user_data;
  Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_stream_do, &vctx);
}

स्थिर bool cf_quiche_do_resume(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *sdata,
                                संरचना h3_stream_ctx *stream,
                                शून्य *user_data)
{
  (शून्य)user_data;
  यदि(stream->quic_flow_blocked) {
    stream->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", stream->id);
  }
  प्रत्यागम TRUE;
}

स्थिर bool cf_quiche_do_expire(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *sdata,
                                संरचना h3_stream_ctx *stream,
                                शून्य *user_data)
{
  (शून्य)user_data;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  stream->xfer_result = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  प्रत्यागम TRUE;
}

स्थिर CURLcode h3_data_setup(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  यदि(stream)
    प्रत्यागम CURLE_OK;

  stream = curlx_calloc(1, परिमाण(*stream));
  यदि(!stream)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_पंक्ति_LEN);

  यदि(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h3_stream_ctx_free(stream);
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cf_quiche_stream_close(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   संरचना h3_stream_ctx *stream)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result;

  यदि(ctx->qconn && !stream->closed) {
    quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    यदि(!stream->send_closed) {
      quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      stream->send_closed = TRUE;
    }
    stream->closed = TRUE;
    result = cf_flush_egress(cf, data);
    यदि(result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  stream->id, (पूर्णाङ्क)result);
  }
}

स्थिर शून्य h3_data_done(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  (शून्य)cf;
  यदि(stream) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] easy handle is done", stream->id);
    cf_quiche_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
  }
}

स्थिर शून्य cf_quiche_expire_conn_closed(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *data)
{
  DEBUGASSERT(data->multi);
  CURL_TRC_CF(data, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_expire, रिक्त);
}

स्थिर शून्य cf_quiche_write_hd(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               संरचना h3_stream_ctx *stream,
                               ध्रुव वर्ण *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  यदि(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    यदि(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (पूर्णाङ्क)stream->xfer_result,
                  blen);
  }
}

संरचना cb_ctx {
  संरचना Curl_cfilter *cf;
  संरचना Curl_easy *data;
  संरचना h3_stream_ctx *stream;
};

स्थिर bool is_valid_h3_header(ध्रुव uint8_t *hdr, size_t hlen)
{
  यावत्(hlen--) {
    विकल्प(*hdr++) {
    पक्ष '\n':
    पक्ष '\r':
    पक्ष '\0':
      प्रत्यागम FALSE;
    }
  }
  प्रत्यागम TRUE;
}

स्थिर पूर्णाङ्क cb_each_header(uint8_t *name, size_t name_len,
                          uint8_t *मूल्य, size_t मूल्य_len,
                          शून्य *argp)
{
  संरचना cb_ctx *x = argp;
  संरचना Curl_cfilter *cf = x->cf;
  संरचना Curl_easy *data = x->data;
  संरचना h3_stream_ctx *stream = x->stream;
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  यदि(!stream || stream->xfer_result)
    प्रत्यागम 1; /* abort iteration */

  यदि((name_len == 7) && !strncmp(HTTP_PSEUDO_STATUS, (वर्ण *)name, 7) &&
     is_valid_h3_header(मूल्य, मूल्य_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    result = Curl_http_decode_status(&stream->status_code,
                                     (ध्रुव वर्ण *)मूल्य, मूल्य_len);
    यदि(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    यदि(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, (ध्रुव वर्ण *)मूल्य, मूल्य_len);
    यदि(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    यदि(!result)
      cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] status: %s",
                stream->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  अन्यथा {
    यदि(is_valid_h3_header(मूल्य, मूल्य_len) &&
       is_valid_h3_header(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  stream->id, (पूर्णाङ्क)name_len, name,
                  (पूर्णाङ्क)मूल्य_len, मूल्य);
      curlx_dyn_reset(&ctx->h1hdr);
      result = curlx_dyn_addn(&ctx->h1hdr, (ध्रुव वर्ण *)name, name_len);
      यदि(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      यदि(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, (ध्रुव वर्ण *)मूल्य, मूल्य_len);
      यदि(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      यदि(!result)
        cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    अन्यथा
      CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  stream->id, मूल्य_len + name_len);
  }

  यदि(result) {
    CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] on header error %d",
                stream->id, (पूर्णाङ्क)result);
    यदि(!stream->xfer_result)
      stream->xfer_result = result;
  }
  प्रत्यागम stream->xfer_result ? 1 : 0;
}

स्थिर CURLcode stream_resp_read(शून्य *reader_ctx,
                                 unsigned वर्ण *buf, size_t len,
                                 size_t *pnread)
{
  संरचना cb_ctx *x = reader_ctx;
  संरचना cf_quiche_ctx *ctx = x->cf->ctx;
  संरचना h3_stream_ctx *stream = H3_STREAM_CTX(ctx, x->data);
  ssize_t nread;

  *pnread = 0;
  यदि(!stream)
    प्रत्यागम CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, stream->id, buf, len);
  यदि(!curlx_sztouz(nread, pnread))
    प्रत्यागम CURLE_AGAIN;
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cf_quiche_flush_body(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना h3_stream_ctx *stream)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  ध्रुव uint8_t *buf;
  size_t blen;

  यावत्(stream && !stream->xfer_result) {
    यदि(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      stream->xfer_result = Curl_xfer_write_resp(
        data, (ध्रुव वर्ण *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      यदि(stream->xfer_result) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", stream->id, (पूर्णाङ्क)stream->xfer_result, blen);
      }
    }
    अन्यथा
      भङ्ग;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

स्थिर शून्य cf_quiche_recv_body(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                संरचना h3_stream_ctx *stream)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  संरचना cb_ctx cb_ctx;
  CURLcode result = CURLE_OK;

  यदि(!stream)
    प्रत्यागम;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.data = data;
  cb_ctx.stream = stream;
  Curl_bufq_reset(&ctx->writebuf);
  यावत्(!result) {
    result = Curl_bufq_slurp(&ctx->writebuf,
                             stream_resp_read, &cb_ctx, &nread);
    यदि(!result)
      cf_quiche_flush_body(cf, data, stream);
    अन्यथा यदि(result == CURLE_AGAIN)
      भङ्ग;
    अन्यथा यदि(result) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv_body error %d",
                  stream->id, (पूर्णाङ्क)result);
      failf(data, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            stream->id, (पूर्णाङ्क)result);
      stream->closed = TRUE;
      stream->reset = TRUE;
      stream->send_closed = TRUE;
      यदि(!stream->xfer_result)
        stream->xfer_result = result;
    }
  }
  cf_quiche_flush_body(cf, data, stream);
}

स्थिर शून्य cf_quiche_process_ev(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना h3_stream_ctx *stream,
                                 quiche_h3_event *ev)
{
  गणना quiche_h3_event_type type;

  यदि(!stream)
    प्रत्यागम;

  type = quiche_h3_event_type(ev);

  विकल्प(type) {
  पक्ष QUICHE_H3_EVENT_HEADERS: {
    संरचना cb_ctx cb_ctx;
    stream->resp_got_header = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.data = data;
    cb_ctx.stream = stream;
    quiche_h3_event_for_each_header(ev, cb_each_header, &cb_ctx);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [HEADERS]", stream->id);
    Curl_multi_mark_dirty(data);
    भङ्ग;
  }
  पक्ष QUICHE_H3_EVENT_DATA:
    यदि(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, data, stream);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [DATA]", stream->id);
    Curl_multi_mark_dirty(data);
    भङ्ग;

  पक्ष QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] RESET", stream->id);
    stream->closed = TRUE;
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    Curl_multi_mark_dirty(data);
    भङ्ग;

  पक्ष QUICHE_H3_EVENT_FINISHED:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] CLOSED", stream->id);
    यदि(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, TRUE);
    }
    stream->closed = TRUE;
    Curl_multi_mark_dirty(data);
    भङ्ग;

  पक्ष QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [GOAWAY]", stream->id);
    भङ्ग;

  स्वभाव:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv, unhandled event %d",
                stream->id, (पूर्णाङ्क)type);
    भङ्ग;
  }
}

संरचना cf_quiche_disp_ctx {
  uint64_t stream_id;
  संरचना Curl_cfilter *cf;
  संरचना Curl_multi *multi;
  quiche_h3_event *ev;
};

स्थिर bool cf_quiche_disp_event(uint32_t mid, शून्य *val, शून्य *user_data)
{
  संरचना cf_quiche_disp_ctx *dctx = user_data;
  संरचना h3_stream_ctx *stream = val;

  यदि(stream->id == dctx->stream_id) {
    संरचना Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    यदि(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, stream, dctx->ev);
    प्रत्यागम FALSE; /* stop iterating */
  }
  प्रत्यागम TRUE;
}

स्थिर CURLcode cf_poll_events(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_stream_ctx *stream = रिक्त;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  यावत्(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    यदि(rv == QUICHE_H3_ERR_DONE) {
      भङ्ग;
    }
    अन्यथा यदि(rv < 0) {
      CURL_TRC_CF(data, cf, "error poll: %" PRId64, rv);
      प्रत्यागम CURLE_HTTP3;
    }
    अन्यथा {
      stream = H3_STREAM_CTX(ctx, data);
      यदि(stream && stream->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, data, stream, ev);
        quiche_h3_event_free(ev);
        यदि(stream->xfer_result)
          प्रत्यागम stream->xfer_result;
      }
      अन्यथा {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        संरचना cf_quiche_disp_ctx dctx;
        dctx.stream_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = data->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  प्रत्यागम CURLE_OK;
}

संरचना recv_ctx {
  संरचना Curl_cfilter *cf;
  संरचना Curl_easy *data;
  पूर्णाङ्क pkts;
};

स्थिर CURLcode cf_quiche_recv_pkts(ध्रुव unsigned वर्ण *buf, size_t buflen,
                                    size_t gso_size,
                                    संरचना sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    शून्य *userp)
{
  संरचना recv_ctx *r = userp;
  संरचना cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, offset, nread;
  ssize_t rv;

  (शून्य)ecn;

  recv_info.to = (संरचना sockaddr *)&ctx->q.local_addr;
  recv_info.to_len = ctx->q.local_addrlen;
  recv_info.from = (संरचना sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  प्रति(offset = 0; offset < buflen; offset += gso_size) {
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = quiche_conn_recv(ctx->qconn,
                          (unsigned वर्ण *)CURL_UNCONST(buf + offset),
                          pktlen, &recv_info);
    यदि(!curlx_sztouz(rv, &nread)) {
      यदि(QUICHE_ERR_DONE == rv) {
        यदि(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is draining");
          प्रत्यागम CURLE_RECV_ERROR;
        }
        यदि(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is closed");
          प्रत्यागम CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->data, r->cf, "ingress, quiche is DONE");
        प्रत्यागम CURLE_OK;
      }
      अन्यथा यदि(QUICHE_ERR_TLS_FAIL == rv) {
        long verify_ok = SSL_get_verify_result(ctx->tls.ossl.ssl);
        यदि(verify_ok != X509_V_OK) {
          failf(r->data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->data, "ingress, quiche reports TLS fail");
        प्रत्यागम CURLE_RECV_ERROR;
      }
      अन्यथा {
        failf(r->data, "quiche reports error %zd on receive", rv);
        प्रत्यागम CURLE_RECV_ERROR;
      }
    }
    अन्यथा यदि(nread < pktlen) {
      CURL_TRC_CF(r->data, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_process_ingress(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना recv_ctx rctx;
  CURLcode result;

  DEBUGASSERT(ctx->qconn);
  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  यदि(result)
    प्रत्यागम result;

  rctx.cf = cf;
  rctx.data = data;
  rctx.pkts = 0;

  result = Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  यदि(result)
    प्रत्यागम result;

  यदि(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(data->multi);
    cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_resume, रिक्त);
  }
  प्रत्यागम cf_poll_events(cf, data);
}

संरचना read_ctx {
  संरचना Curl_cfilter *cf;
  संरचना Curl_easy *data;
  quiche_send_info send_info;
};

स्थिर CURLcode read_pkt_to_send(शून्य *userp,
                                 unsigned वर्ण *buf, size_t buflen,
                                 size_t *pnread)
{
  संरचना read_ctx *x = userp;
  संरचना cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  यदि(rv == QUICHE_ERR_DONE)
    प्रत्यागम CURLE_AGAIN;

  यदि(!curlx_sztouz(rv, pnread)) {
    failf(x->data, "quiche_conn_send returned %zd", rv);
    प्रत्यागम CURLE_SEND_ERROR;
  }
  प्रत्यागम CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
स्थिर CURLcode cf_flush_egress(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode result;
  int64_t expiry_ns;
  int64_t timeout_ns;
  संरचना read_ctx readx;
  size_t pkt_गिनती, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  यदि(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    यदि(quiche_conn_is_closed(ctx->qconn)) {
      यदि(quiche_conn_is_timed_out(ctx->qconn))
        failf(data, "connection closed by idle timeout");
      अन्यथा
        failf(data, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, data);
      प्रत्यागम CURLE_SEND_ERROR;
    }
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  यदि(result) {
    यदि(result == CURLE_AGAIN) {
      Curl_expire(data, 1, EXPIRE_QUIC);
      प्रत्यागम CURLE_OK;
    }
    प्रत्यागम result;
  }

  readx.cf = cf;
  readx.data = data;
  memset(&readx.send_info, 0, परिमाण(readx.send_info));
  pkt_गिनती = 0;
  gsolen = quiche_conn_max_send_udp_payload_size(ctx->qconn);
  प्रति(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_to_send, &readx, &nread);
    यदि(result) {
      यदि(result != CURLE_AGAIN)
        प्रत्यागम result;
      /* Nothing more to add, flush and leave */
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      यदि(result) {
        यदि(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          प्रत्यागम CURLE_OK;
        }
        प्रत्यागम result;
      }
      गच्छ out;
    }

    ++pkt_गिनती;
    यदि(nread < gsolen || pkt_गिनती >= MAX_PKT_BURST) {
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      यदि(result) {
        यदि(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          प्रत्यागम CURLE_OK;
        }
        गच्छ out;
      }
      pkt_गिनती = 0;
    }
  }

out:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  यदि(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(data, (timeout_ns / 1000000), EXPIRE_QUIC);
  प्रत्यागम result;
}

स्थिर CURLcode recv_closed_stream(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   size_t *pnread)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(stream);
  *pnread = 0;
  यदि(stream->reset) {
    यदि(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      प्रत्यागम CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    अन्यथा यदि(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        प्रत्यागम CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                stream->id, (पूर्णाङ्क)result);
  }
  अन्यथा यदि(!stream->resp_got_header) {
    failf(data, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    result = CURLE_HTTP3;
  }
  प्रत्यागम result;
}

स्थिर CURLcode cf_quiche_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                               वर्ण *buf, size_t blen, size_t *pnread)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  *pnread = 0;
  (शून्य)buf;
  (शून्य)blen;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  यदि(!stream)
    प्रत्यागम CURLE_RECV_ERROR;

  result = cf_process_ingress(cf, data);
  यदि(result) {
    CURL_TRC_CF(data, cf, "cf_recv, error on ingress");
    गच्छ out;
  }

  यदि(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
    गच्छ out;
  }
  अन्यथा यदि(stream->closed)
    result = recv_closed_stream(cf, data, pnread);
  अन्यथा यदि(quiche_conn_is_draining(ctx->qconn)) {
    failf(data, "QUIC connection is draining");
    result = CURLE_HTTP3;
  }
  अन्यथा
    result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));
  यदि(*pnread > 0)
    ctx->data_recvd += *pnread;
  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, stream->id, blen, (पूर्णाङ्क)result, *pnread,
              ctx->data_recvd);
  प्रत्यागम result;
}

स्थिर CURLcode cf_quiche_send_body(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    संरचना h3_stream_ctx *stream,
                                    ध्रुव uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, stream->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  यदि(rv == QUICHE_H3_ERR_DONE || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    यदि(!quiche_conn_stream_writable(ctx->qconn, stream->id, len)) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", stream->id, len);
      stream->quic_flow_blocked = TRUE;
    }
    प्रत्यागम CURLE_AGAIN;
  }
  अन्यथा यदि(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_STREAM_STATE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", stream->id, len);
    प्रत्यागम CURLE_HTTP3;
  }
  अन्यथा यदि(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_SIZE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                stream->id, len);
    प्रत्यागम CURLE_SEND_ERROR;
  }
  अन्यथा यदि(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                stream->id, len, rv);
    प्रत्यागम CURLE_SEND_ERROR;
  }
  अन्यथा {
    यदि(eos && (len == *pnwritten))
      stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                stream->id, len, stream->send_closed, *pnwritten);
    प्रत्यागम CURLE_OK;
  }
}

स्थिर CURLcode h3_open_stream(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               ध्रुव uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  size_t nheader, i;
  int64_t rv;
  संरचना dynhds h2_headers;
  quiche_h3_header *nva = रिक्त;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  यदि(!stream) {
    result = h3_data_setup(cf, data);
    यदि(result)
      प्रत्यागम result;
    stream = H3_STREAM_CTX(ctx, data);
    DEBUGASSERT(stream);
  }

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(stream);

  result = Curl_h1_req_parse_read(&stream->h1, buf, blen, रिक्त,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : रिक्त,
    0, pnwritten);
  यदि(result)
    गच्छ out;
  यदि(!stream->h1.done) {
    /* need more data */
    गच्छ out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  यदि(result)
    गच्छ out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_गिनती(&h2_headers);
  nva = curlx_malloc(परिमाण(quiche_h3_header) * nheader);
  यदि(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  प्रति(i = 0; i < nheader; ++i) {
    संरचना dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned वर्ण *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].मूल्य = (unsigned वर्ण *)e->मूल्य;
    nva[i].मूल्य_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  यदि(eos && !blen)
    stream->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              stream->send_closed);
  CURL_TRC_CF(data, cf, "quiche_send_request() -> %" PRId64, rv);
  यदि(rv < 0) {
    यदि(QUICHE_H3_ERR_STREAM_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] blocked", stream->id);
      stream->quic_flow_blocked = TRUE;
      result = CURLE_AGAIN;
      गच्छ out;
    }
    अन्यथा {
      CURL_TRC_CF(data, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&data->state.url), rv);
    }
    result = CURLE_SEND_ERROR;
    गच्छ out;
  }

  DEBUGASSERT(!stream->opened);
  stream->id = (uint64_t)rv;
  stream->opened = TRUE;
  stream->closed = FALSE;
  stream->reset = FALSE;

  यदि(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    प्रति(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", stream->id,
            (पूर्णाङ्क)nva[i].name_len, nva[i].name,
            (पूर्णाङ्क)nva[i].मूल्य_len, nva[i].मूल्य);
    }
  }

  यदि(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, data, stream, buf, blen, eos, &nwritten);
    यदि(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      result = r2;
    }
    अन्यथा यदि(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  प्रत्यागम result;
}

स्थिर CURLcode cf_quiche_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                               ध्रुव uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result;

  *pnwritten = 0;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  result = cf_process_ingress(cf, data);
  यदि(result)
    गच्छ out;

  यदि(!stream || !stream->opened) {
    result = h3_open_stream(cf, data, buf, len, eos, pnwritten);
    यदि(result)
      गच्छ out;
    stream = H3_STREAM_CTX(ctx, data);
  }
  अन्यथा यदि(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
  }
  अन्यथा यदि(stream->closed) {
    यदि(stream->resp_hds_complete) {
      /* sending request body on a stream that has been closed by the
       * server. If the server has send us a final response, we should
       * silently discard the send data.
       * This happens for example on redirects where the server, instead
       * of reading the full request body closed the stream after
       * sending the 30x response.
       * This is sort of a race: had the transfer loop called recv first,
       * it would see the response and stop/discard sending on its own- */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] discarding data "
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      गच्छ out;
    }
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    गच्छ out;
  }
  अन्यथा {
    result = cf_quiche_send_body(cf, data, stream, buf, len, eos, pnwritten);
  }

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));

  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : (uint64_t)~0, len,
              (पूर्णाङ्क)result, *pnwritten);
  प्रत्यागम result;
}

स्थिर bool stream_is_writable(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  प्रत्यागम stream && (quiche_conn_stream_writable(
    ctx->qconn, stream->id, 1) > 0);
}

स्थिर CURLcode cf_quiche_adjust_pollset(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *data,
                                         संरचना easy_pollset *ps)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  यदि(!ctx->qconn)
    प्रत्यागम CURLE_OK;

  Curl_pollset_check(data, ps, ctx->q.sockfd, &want_recv, &want_send);
  यदि(want_recv || want_send) {
    संरचना h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    bool c_exhaust, s_exhaust;

    c_exhaust = FALSE; /* Have not found any call in quiche that tells
                          us if the connection itself is blocked */
    s_exhaust = want_send && stream && stream->opened &&
                (stream->quic_flow_blocked || !stream_is_writable(cf, data));
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, ctx->q.sockfd, want_recv, want_send);
  }
  प्रत्यागम result;
}

स्थिर CURLcode h3_data_pause(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (शून्य)cf;
  यदि(!pause) {
    Curl_multi_mark_dirty(data);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_quiche_cntrl(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (शून्य)arg1;
  (शून्य)arg2;
  विकल्प(event) {
  पक्ष CF_CTRL_DATA_SETUP:
    भङ्ग;
  पक्ष CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    भङ्ग;
  पक्ष CF_CTRL_DATA_DONE:
    h3_data_done(cf, data);
    भङ्ग;
  पक्ष CF_CTRL_DATA_DONE_SEND: {
    संरचना h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    यदि(stream && !stream->send_closed) {
      unsigned वर्ण body[1];
      size_t sent;

      stream->send_closed = TRUE;
      body[0] = 'X';
      result = cf_quiche_send(cf, data, body, 0, TRUE, &sent);
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  stream->id, (पूर्णाङ्क)result, sent);
    }
    भङ्ग;
  }
  पक्ष CF_CTRL_CONN_INFO_UPDATE:
    यदि(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    भङ्ग;
  पक्ष CF_CTRL_REPORT_STATS:
    यदि(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम result;
}

स्थिर CURLcode cf_quiche_ctx_open(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  पूर्णाङ्क rv;
  CURLcode result;
  ध्रुव संरचना Curl_sockaddr_ex *sockaddr;
  स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  यदि(result)
    प्रत्यागम result;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_VERSION);
  यदि(!ctx->cfg) {
    failf(data, "cannot create quiche config");
    प्रत्यागम CURLE_FAILED_INIT;
  }
  quiche_config_enable_pacing(ctx->cfg, FALSE);
  quiche_config_set_initial_max_data(ctx->cfg, (1 * 1024 * 1024)
    /* (QUIC_MAX_STREAMS/2) * H3_STREAM_WINDOW_SIZE */);
  quiche_config_set_initial_max_streams_bidi(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_set_initial_max_streams_uni(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_set_initial_max_stream_data_bidi_local(ctx->cfg,
    H3_STREAM_WINDOW_SIZE);
  quiche_config_set_initial_max_stream_data_bidi_remote(ctx->cfg,
    H3_STREAM_WINDOW_SIZE);
  quiche_config_set_initial_max_stream_data_uni(ctx->cfg,
    H3_STREAM_WINDOW_SIZE);
  quiche_config_set_disable_active_migration(ctx->cfg, TRUE);

  quiche_config_set_max_connection_window(ctx->cfg,
    10 * QUIC_MAX_STREAMS * H3_STREAM_WINDOW_SIZE);
  quiche_config_set_max_stream_window(ctx->cfg, 10 * H3_STREAM_WINDOW_SIZE);
  quiche_config_set_application_protos(ctx->cfg,
                      (uint8_t *)CURL_UNCONST(QUICHE_H3_APPLICATION_PROTOCOL),
                                 CURL_CSTRLEN(QUICHE_H3_APPLICATION_PROTOCOL));

  result = Curl_vquic_tls_init(&ctx->tls, cf, data, &ctx->ssl_peer,
                               &ALPN_SPEC_H3, रिक्त, रिक्त, cf, रिक्त);
  यदि(result)
    प्रत्यागम result;

  result = Curl_rand(data, ctx->scid, परिमाण(ctx->scid));
  यदि(result)
    प्रत्यागम result;

  यदि(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, रिक्त))
    प्रत्यागम CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = परिमाण(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (संरचना sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  यदि(rv == -1)
    प्रत्यागम CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((ध्रुव uint8_t *)ctx->scid,
                                        परिमाण(ctx->scid), रिक्त, 0,
                                        (संरचना sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  यदि(!ctx->qconn) {
    failf(data, "cannot create quiche connection");
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  /* Known to not work on Windows */
#यदि !defined(_WIN32) && defined(HAVE_QUICHE_CONN_SET_QLOG_FD)
  {
    पूर्णाङ्क qfd;
    (शून्य)Curl_qlogdir(data, ctx->scid, परिमाण(ctx->scid), &qfd);
    यदि(qfd != -1)
      quiche_conn_set_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#endif

  result = cf_flush_egress(cf, data);
  यदि(result)
    प्रत्यागम result;

  {
    unsigned वर्ण alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    unsigned alpn_len, offset = 0;

    /* Replace each ALPN length prefix by a comma. */
    यावत्(offset < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[offset];
      alpn_protocols[offset] = ',';
      offset += 1 + alpn_len;
    }

    CURL_TRC_CF(data, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_quiche_verify_peer(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  प्रत्यागम Curl_vquic_tls_verify_peer(&ctx->tls, cf, data, &ctx->ssl_peer);
}

स्थिर CURLcode cf_quiche_connect(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  bool *done)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  यदि(cf->connected) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }

  /* Connect the UDP filter first */
  यदि(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    यदि(result || !*done)
      प्रत्यागम result;
  }

  *done = FALSE;
  यदि(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    प्रत्यागम CURLE_OK;
  }

  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  यदि(!ctx->qconn) {
    result = cf_quiche_ctx_open(cf, data);
    यदि(result)
      गच्छ out;
    ctx->started_at = ctx->q.last_op;
    result = cf_flush_egress(cf, data);
    /* we do not expect to be able to recv anything yet */
    गच्छ out;
  }

  result = cf_process_ingress(cf, data);
  यदि(result)
    गच्छ out;

  result = cf_flush_egress(cf, data);
  यदि(result)
    गच्छ out;

  यदि(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.last_op;
    CURL_TRC_CF(data, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    result = cf_quiche_verify_peer(cf, data);
    यदि(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      यदि(!ctx->h3config) {
        result = CURLE_OUT_OF_MEMORY;
        गच्छ out;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_set_max_field_section_size(ctx->h3config,
                                                  MAX_HTTP_RESP_HEADER_SIZE);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      यदि(!ctx->h3c) {
        result = CURLE_OUT_OF_MEMORY;
        गच्छ out;
      }
      cf->connected = TRUE;
      *done = TRUE;
    }
  }
  अन्यथा यदि(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    result = CURLE_WEIRD_SERVER_REPLY;
  }

out:
#ifdef CURLVERBOSE
  यदि(result && result != CURLE_AGAIN) {
    संरचना ip_quadruple ip;

    यदि(!Curl_cf_socket_peek(cf->next, data, रिक्त, रिक्त, &ip))
      infof(data, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    अन्यथा
      infof(data, "connect failed: %s", curl_easy_strerror(result));
  }
#endif
  प्रत्यागम result;
}

स्थिर CURLcode cf_quiche_shutdown(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data, bool *done)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  यदि(cf->shutdown || !ctx || !ctx->qconn) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *done = FALSE;
  यदि(!ctx->shutdown_started) {
    पूर्णाङ्क err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, रिक्त, 0);
    यदि(err) {
      CURL_TRC_CF(data, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      result = CURLE_SEND_ERROR;
      गच्छ out;
    }
  }

  यदि(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
    result = cf_flush_egress(cf, data);
    यदि(result)
      गच्छ out;
  }

  यदि(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
  }
  अन्यथा {
    CURL_TRC_CF(data, cf, "shutdown sending blocked");
  }

out:
  प्रत्यागम result;
}

स्थिर शून्य cf_quiche_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  (शून्य)data;
  यदि(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = रिक्त;
  }
}

स्थिर CURLcode cf_quiche_query(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;

  विकल्प(query) {
  पक्ष CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    यदि(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_बायाँ_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > INT_MAX) ? INT_MAX : (पूर्णाङ्क)max_streams;
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    प्रत्यागम CURLE_OK;
  }
  पक्ष CF_QUERY_CONNECT_REPLY_MS:
    यदि(ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (पूर्णाङ्क)ms : INT_MAX;
    }
    अन्यथा
      *pres1 = -1;
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_HTTP_VERSION:
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
  प्रत्यागम cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर bool cf_quiche_conn_is_alive(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    bool *निवेश_pending)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *निवेश_pending = FALSE;
  यदि(!ctx->qconn)
    प्रत्यागम FALSE;

  यदि(quiche_conn_is_closed(ctx->qconn)) {
    यदि(quiche_conn_is_timed_out(ctx->qconn))
      CURL_TRC_CF(data, cf, "connection was closed due to idle timeout");
    अन्यथा
      CURL_TRC_CF(data, cf, "connection is closed");
    प्रत्यागम FALSE;
  }

  यदि(!cf->next || !cf->next->cft->is_alive(cf->next, data, निवेश_pending))
    प्रत्यागम FALSE;

  यदि(*निवेश_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *निवेश_pending = FALSE;
    यदि(cf_process_ingress(cf, data))
      alive = FALSE;
    अन्यथा {
      alive = TRUE;
    }
  }

  प्रत्यागम alive;
}

संरचना Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_quiche_destroy,
  cf_quiche_connect,
  cf_quiche_shutdown,
  cf_quiche_adjust_pollset,
  Curl_cf_def_data_pending,
  cf_quiche_send,
  cf_quiche_recv,
  cf_quiche_cntrl,
  cf_quiche_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_quiche_query,
};

CURLcode Curl_cf_quiche_create(संरचना Curl_cfilter **pcf,
                               संरचना Curl_easy *data,
                               संरचना Curl_peer *origin,
                               संरचना Curl_peer *peer,
                               संरचना connectdata *conn,
                               संरचना Curl_sockaddr_ex *addr)
{
  संरचना cf_quiche_ctx *ctx = रिक्त;
  संरचना Curl_cfilter *cf = रिक्त;
  CURLcode result;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  यदि(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  यदि(result)
    गच्छ out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, रिक्त, TRNSPRT_QUIC);
  यदि(result)
    गच्छ out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : रिक्त;
  यदि(result) {
    यदि(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    अन्यथा यदि(ctx)
      cf_quiche_ctx_free(ctx);
  }

  प्रत्यागम result;
}

CURLcode Curl_cf_quiche_जोड़ो_after(संरचना Curl_cfilter *cf_at,
                                     संरचना Curl_peer *origin,
                                     संरचना Curl_peer *peer,
                                     संरचना Curl_cfilter **pcf)
{
  संरचना cf_quiche_ctx *ctx = रिक्त;
  संरचना Curl_cfilter *cf = रिक्त;
  CURLcode result;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  यदि(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  यदि(result)
    गच्छ out;
  Curl_conn_cf_जोड़ो_after(cf_at, cf);

out:
  यदि(result) {
    curlx_safefree(cf);
    यदि(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  प्रत्यागम result;
}

#endif
