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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#شامل <quiche.h>
#شامل <openssl/err.h>
#شامل <openssl/ssl.h>

#شامل "bufq.h"
#شامل "u32_ptrset.h"
#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "cf-socket.h"
#شامل "curl_trc.h"
#شامل "rand.h"
#شامل "multiif.h"
#شامل "connect.h"
#شامل "progress.h"
#شامل "select.h"
#شامل "http.h"
#شامل "http1.h"
#شامل "sockaddr.h"
#شامل "vdns/cf-dns.h"
#شامل "vquic/vquic.h"
#شامل "vquic/vquic_int.h"
#شامل "vquic/vquic-tls.h"
#شامل "vquic/cf-quiche.h"
#شامل "transfer.h"
#شامل "url.h"
#شامل "bufref.h"
#شامل "vtls/openssl.h"
#شامل "vtls/vtls.h"

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
تهی Curl_quiche_ver(نویسه *p, size_t len)
{
  (تهی)curl_msnprintf(p, len, "quiche/%s", quiche_version());
}

ساختار cf_quiche_ctx {
  ساختار cf_quic_ctx q;
  ساختار ssl_peer ssl_peer;
  ساختار curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  ساختار curltime started_at;        /* time the current attempt started */
  ساختار curltime handshake_at;      /* time connect handshake finished */
  ساختار u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  ساختار dynbuf h1hdr;               /* temp buffer for header construction */
  ساختار bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t data_recvd;
  BIT(initialized);
  BIT(goaway);                       /* got GOAWAY from server */
  BIT(x509_store_setup);             /* if x509 store has been set up */
  BIT(shutdown_started);             /* queued shutdown packets */
  BIT(stats_reported);               /* connect statistics reported */
};

#ifdef DEBUG_QUICHE
/* initialize debug log callback only once */
ایستا صحیح debug_log_init = 0;
ایستا تهی quiche_debug_log(ثابت نویسه *سطر, تهی *argp)
{
  (تهی)argp;
  curl_mfprintf(stderr, "%s\n", سطر);
}
#endif

ایستا تهی h3_stream_set_free(uint32_t id, تهی *stream);

ایستا CURLcode cf_quiche_ctx_init(ساختار cf_quiche_ctx *ctx,
                                   ساختار Curl_peer *origin,
                                   ساختار Curl_peer *peer,
                                   ساختار ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#ifdef DEBUG_QUICHE
  اگر(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, پوچ);
    debug_log_init = 1;
  }
#endif
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  Curl_bufq_init2(&ctx->writebuf, H3_STREAM_CHUNK_SIZE, H3_STREAM_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  ctx->data_recvd = 0;
  ctx->initialized = TRUE;
  بازگشت Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

ایستا تهی cf_quiche_ctx_free(ساختار cf_quiche_ctx *ctx)
{
  اگر(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = پوچ;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

ایستا تهی cf_quiche_ctx_close(ساختار cf_quiche_ctx *ctx)
{
  اگر(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = پوچ;
  }
  اگر(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = پوچ;
  }
  اگر(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = پوچ;
  }
  اگر(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = پوچ;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

ایستا CURLcode cf_flush_egress(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data);

/**
 * All about the H3 internals of a stream
 */
ساختار h3_stream_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  ساختار h1_req_parser h1; /* h1 request parsing */
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

ایستا تهی h3_stream_ctx_free(ساختار h3_stream_ctx *stream)
{
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

ایستا تهی h3_stream_set_free(uint32_t id, تهی *stream)
{
  (تهی)id;
  DEBUGASSERT(stream);
  h3_stream_ctx_free((ساختار h3_stream_ctx *)stream);
}

تعریفنوع bool cf_quiche_svisit(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *sdata,
                              ساختار h3_stream_ctx *stream,
                              تهی *user_data);

ساختار cf_quiche_visit_ctx {
  ساختار Curl_cfilter *cf;
  ساختار Curl_multi *multi;
  cf_quiche_svisit *cb;
  تهی *user_data;
};

ایستا bool cf_quiche_stream_do(uint32_t mid, تهی *val, تهی *user_data)
{
  ساختار cf_quiche_visit_ctx *vctx = user_data;
  ساختار h3_stream_ctx *stream = val;
  ساختار Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  اگر(sdata)
    بازگشت vctx->cb(vctx->cf, sdata, stream, vctx->user_data);
  بازگشت TRUE;
}

ایستا تهی cf_quiche_for_all_streams(ساختار Curl_cfilter *cf,
                                      ساختار Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      تهی *user_data)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  ساختار cf_quiche_visit_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_data = user_data;
  Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_stream_do, &vctx);
}

ایستا bool cf_quiche_do_resume(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *sdata,
                                ساختار h3_stream_ctx *stream,
                                تهی *user_data)
{
  (تهی)user_data;
  اگر(stream->quic_flow_blocked) {
    stream->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", stream->id);
  }
  بازگشت TRUE;
}

ایستا bool cf_quiche_do_expire(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *sdata,
                                ساختار h3_stream_ctx *stream,
                                تهی *user_data)
{
  (تهی)user_data;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  stream->xfer_result = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  بازگشت TRUE;
}

ایستا CURLcode h3_data_setup(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  اگر(stream)
    بازگشت CURLE_OK;

  stream = curlx_calloc(1, اندازه(*stream));
  اگر(!stream)
    بازگشت CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_سطر_LEN);

  اگر(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h3_stream_ctx_free(stream);
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  بازگشت CURLE_OK;
}

ایستا تهی cf_quiche_stream_close(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   ساختار h3_stream_ctx *stream)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result;

  اگر(ctx->qconn && !stream->closed) {
    quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    اگر(!stream->send_closed) {
      quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      stream->send_closed = TRUE;
    }
    stream->closed = TRUE;
    result = cf_flush_egress(cf, data);
    اگر(result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  stream->id, (صحیح)result);
  }
}

ایستا تهی h3_data_done(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  (تهی)cf;
  اگر(stream) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] easy handle is done", stream->id);
    cf_quiche_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
  }
}

ایستا تهی cf_quiche_expire_conn_closed(ساختار Curl_cfilter *cf,
                                         ساختار Curl_easy *data)
{
  DEBUGASSERT(data->multi);
  CURL_TRC_CF(data, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_expire, پوچ);
}

ایستا تهی cf_quiche_write_hd(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ساختار h3_stream_ctx *stream,
                               ثابت نویسه *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  اگر(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    اگر(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (صحیح)stream->xfer_result,
                  blen);
  }
}

ساختار cb_ctx {
  ساختار Curl_cfilter *cf;
  ساختار Curl_easy *data;
  ساختار h3_stream_ctx *stream;
};

ایستا bool is_valid_h3_header(ثابت uint8_t *hdr, size_t hlen)
{
  وقتی(hlen--) {
    انتخاب(*hdr++) {
    حالت '\n':
    حالت '\r':
    حالت '\0':
      بازگشت FALSE;
    }
  }
  بازگشت TRUE;
}

ایستا صحیح cb_each_header(uint8_t *name, size_t name_len,
                          uint8_t *مقدار, size_t مقدار_len,
                          تهی *argp)
{
  ساختار cb_ctx *x = argp;
  ساختار Curl_cfilter *cf = x->cf;
  ساختار Curl_easy *data = x->data;
  ساختار h3_stream_ctx *stream = x->stream;
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  اگر(!stream || stream->xfer_result)
    بازگشت 1; /* abort iteration */

  اگر((name_len == 7) && !strncmp(HTTP_PSEUDO_STATUS, (نویسه *)name, 7) &&
     is_valid_h3_header(مقدار, مقدار_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    result = Curl_http_decode_status(&stream->status_code,
                                     (ثابت نویسه *)مقدار, مقدار_len);
    اگر(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    اگر(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, (ثابت نویسه *)مقدار, مقدار_len);
    اگر(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    اگر(!result)
      cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] status: %s",
                stream->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  وگرنه {
    اگر(is_valid_h3_header(مقدار, مقدار_len) &&
       is_valid_h3_header(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  stream->id, (صحیح)name_len, name,
                  (صحیح)مقدار_len, مقدار);
      curlx_dyn_reset(&ctx->h1hdr);
      result = curlx_dyn_addn(&ctx->h1hdr, (ثابت نویسه *)name, name_len);
      اگر(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      اگر(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, (ثابت نویسه *)مقدار, مقدار_len);
      اگر(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      اگر(!result)
        cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    وگرنه
      CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  stream->id, مقدار_len + name_len);
  }

  اگر(result) {
    CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] on header error %d",
                stream->id, (صحیح)result);
    اگر(!stream->xfer_result)
      stream->xfer_result = result;
  }
  بازگشت stream->xfer_result ? 1 : 0;
}

ایستا CURLcode stream_resp_read(تهی *reader_ctx,
                                 unsigned نویسه *buf, size_t len,
                                 size_t *pnread)
{
  ساختار cb_ctx *x = reader_ctx;
  ساختار cf_quiche_ctx *ctx = x->cf->ctx;
  ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, x->data);
  ssize_t nread;

  *pnread = 0;
  اگر(!stream)
    بازگشت CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, stream->id, buf, len);
  اگر(!curlx_sztouz(nread, pnread))
    بازگشت CURLE_AGAIN;
  بازگشت CURLE_OK;
}

ایستا تهی cf_quiche_flush_body(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار h3_stream_ctx *stream)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  ثابت uint8_t *buf;
  size_t blen;

  وقتی(stream && !stream->xfer_result) {
    اگر(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      stream->xfer_result = Curl_xfer_write_resp(
        data, (ثابت نویسه *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      اگر(stream->xfer_result) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", stream->id, (صحیح)stream->xfer_result, blen);
      }
    }
    وگرنه
      بشکن;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

ایستا تهی cf_quiche_recv_body(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                ساختار h3_stream_ctx *stream)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  ساختار cb_ctx cb_ctx;
  CURLcode result = CURLE_OK;

  اگر(!stream)
    بازگشت;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.data = data;
  cb_ctx.stream = stream;
  Curl_bufq_reset(&ctx->writebuf);
  وقتی(!result) {
    result = Curl_bufq_slurp(&ctx->writebuf,
                             stream_resp_read, &cb_ctx, &nread);
    اگر(!result)
      cf_quiche_flush_body(cf, data, stream);
    وگرنه اگر(result == CURLE_AGAIN)
      بشکن;
    وگرنه اگر(result) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv_body error %d",
                  stream->id, (صحیح)result);
      failf(data, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            stream->id, (صحیح)result);
      stream->closed = TRUE;
      stream->reset = TRUE;
      stream->send_closed = TRUE;
      اگر(!stream->xfer_result)
        stream->xfer_result = result;
    }
  }
  cf_quiche_flush_body(cf, data, stream);
}

ایستا تهی cf_quiche_process_ev(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار h3_stream_ctx *stream,
                                 quiche_h3_event *ev)
{
  شمارش quiche_h3_event_type type;

  اگر(!stream)
    بازگشت;

  type = quiche_h3_event_type(ev);

  انتخاب(type) {
  حالت QUICHE_H3_EVENT_HEADERS: {
    ساختار cb_ctx cb_ctx;
    stream->resp_got_header = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.data = data;
    cb_ctx.stream = stream;
    quiche_h3_event_for_each_header(ev, cb_each_header, &cb_ctx);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [HEADERS]", stream->id);
    Curl_multi_mark_dirty(data);
    بشکن;
  }
  حالت QUICHE_H3_EVENT_DATA:
    اگر(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, data, stream);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [DATA]", stream->id);
    Curl_multi_mark_dirty(data);
    بشکن;

  حالت QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] RESET", stream->id);
    stream->closed = TRUE;
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    Curl_multi_mark_dirty(data);
    بشکن;

  حالت QUICHE_H3_EVENT_FINISHED:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] CLOSED", stream->id);
    اگر(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, TRUE);
    }
    stream->closed = TRUE;
    Curl_multi_mark_dirty(data);
    بشکن;

  حالت QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [GOAWAY]", stream->id);
    بشکن;

  پیشفرض:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv, unhandled event %d",
                stream->id, (صحیح)type);
    بشکن;
  }
}

ساختار cf_quiche_disp_ctx {
  uint64_t stream_id;
  ساختار Curl_cfilter *cf;
  ساختار Curl_multi *multi;
  quiche_h3_event *ev;
};

ایستا bool cf_quiche_disp_event(uint32_t mid, تهی *val, تهی *user_data)
{
  ساختار cf_quiche_disp_ctx *dctx = user_data;
  ساختار h3_stream_ctx *stream = val;

  اگر(stream->id == dctx->stream_id) {
    ساختار Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    اگر(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, stream, dctx->ev);
    بازگشت FALSE; /* stop iterating */
  }
  بازگشت TRUE;
}

ایستا CURLcode cf_poll_events(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = پوچ;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  وقتی(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    اگر(rv == QUICHE_H3_ERR_DONE) {
      بشکن;
    }
    وگرنه اگر(rv < 0) {
      CURL_TRC_CF(data, cf, "error poll: %" PRId64, rv);
      بازگشت CURLE_HTTP3;
    }
    وگرنه {
      stream = H3_STREAM_CTX(ctx, data);
      اگر(stream && stream->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, data, stream, ev);
        quiche_h3_event_free(ev);
        اگر(stream->xfer_result)
          بازگشت stream->xfer_result;
      }
      وگرنه {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        ساختار cf_quiche_disp_ctx dctx;
        dctx.stream_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = data->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  بازگشت CURLE_OK;
}

ساختار recv_ctx {
  ساختار Curl_cfilter *cf;
  ساختار Curl_easy *data;
  صحیح pkts;
};

ایستا CURLcode cf_quiche_recv_pkts(ثابت unsigned نویسه *buf, size_t buflen,
                                    size_t gso_size,
                                    ساختار sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    تهی *userp)
{
  ساختار recv_ctx *r = userp;
  ساختار cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, offset, nread;
  ssize_t rv;

  (تهی)ecn;

  recv_info.to = (ساختار sockaddr *)&ctx->q.local_addr;
  recv_info.to_len = ctx->q.local_addrlen;
  recv_info.from = (ساختار sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  برای(offset = 0; offset < buflen; offset += gso_size) {
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = quiche_conn_recv(ctx->qconn,
                          (unsigned نویسه *)CURL_UNCONST(buf + offset),
                          pktlen, &recv_info);
    اگر(!curlx_sztouz(rv, &nread)) {
      اگر(QUICHE_ERR_DONE == rv) {
        اگر(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is draining");
          بازگشت CURLE_RECV_ERROR;
        }
        اگر(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is closed");
          بازگشت CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->data, r->cf, "ingress, quiche is DONE");
        بازگشت CURLE_OK;
      }
      وگرنه اگر(QUICHE_ERR_TLS_FAIL == rv) {
        long verify_ok = SSL_get_verify_result(ctx->tls.ossl.ssl);
        اگر(verify_ok != X509_V_OK) {
          failf(r->data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          بازگشت CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->data, "ingress, quiche reports TLS fail");
        بازگشت CURLE_RECV_ERROR;
      }
      وگرنه {
        failf(r->data, "quiche reports error %zd on receive", rv);
        بازگشت CURLE_RECV_ERROR;
      }
    }
    وگرنه اگر(nread < pktlen) {
      CURL_TRC_CF(r->data, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode cf_process_ingress(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  ساختار recv_ctx rctx;
  CURLcode result;

  DEBUGASSERT(ctx->qconn);
  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  اگر(result)
    بازگشت result;

  rctx.cf = cf;
  rctx.data = data;
  rctx.pkts = 0;

  result = Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  اگر(result)
    بازگشت result;

  اگر(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(data->multi);
    cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_resume, پوچ);
  }
  بازگشت cf_poll_events(cf, data);
}

ساختار read_ctx {
  ساختار Curl_cfilter *cf;
  ساختار Curl_easy *data;
  quiche_send_info send_info;
};

ایستا CURLcode read_pkt_to_send(تهی *userp,
                                 unsigned نویسه *buf, size_t buflen,
                                 size_t *pnread)
{
  ساختار read_ctx *x = userp;
  ساختار cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  اگر(rv == QUICHE_ERR_DONE)
    بازگشت CURLE_AGAIN;

  اگر(!curlx_sztouz(rv, pnread)) {
    failf(x->data, "quiche_conn_send returned %zd", rv);
    بازگشت CURLE_SEND_ERROR;
  }
  بازگشت CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
ایستا CURLcode cf_flush_egress(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode result;
  int64_t expiry_ns;
  int64_t timeout_ns;
  ساختار read_ctx readx;
  size_t pkt_شمار, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  اگر(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    اگر(quiche_conn_is_closed(ctx->qconn)) {
      اگر(quiche_conn_is_timed_out(ctx->qconn))
        failf(data, "connection closed by idle timeout");
      وگرنه
        failf(data, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, data);
      بازگشت CURLE_SEND_ERROR;
    }
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  اگر(result) {
    اگر(result == CURLE_AGAIN) {
      Curl_expire(data, 1, EXPIRE_QUIC);
      بازگشت CURLE_OK;
    }
    بازگشت result;
  }

  readx.cf = cf;
  readx.data = data;
  memset(&readx.send_info, 0, اندازه(readx.send_info));
  pkt_شمار = 0;
  gsolen = quiche_conn_max_send_udp_payload_size(ctx->qconn);
  برای(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_to_send, &readx, &nread);
    اگر(result) {
      اگر(result != CURLE_AGAIN)
        بازگشت result;
      /* Nothing more to add, flush and leave */
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      اگر(result) {
        اگر(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          بازگشت CURLE_OK;
        }
        بازگشت result;
      }
      برو out;
    }

    ++pkt_شمار;
    اگر(nread < gsolen || pkt_شمار >= MAX_PKT_BURST) {
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      اگر(result) {
        اگر(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          بازگشت CURLE_OK;
        }
        برو out;
      }
      pkt_شمار = 0;
    }
  }

out:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  اگر(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(data, (timeout_ns / 1000000), EXPIRE_QUIC);
  بازگشت result;
}

ایستا CURLcode recv_closed_stream(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   size_t *pnread)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(stream);
  *pnread = 0;
  اگر(stream->reset) {
    اگر(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      بازگشت CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وگرنه اگر(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        بازگشت CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                stream->id, (صحیح)result);
  }
  وگرنه اگر(!stream->resp_got_header) {
    failf(data, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    result = CURLE_HTTP3;
  }
  بازگشت result;
}

ایستا CURLcode cf_quiche_recv(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                               نویسه *buf, size_t blen, size_t *pnread)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  *pnread = 0;
  (تهی)buf;
  (تهی)blen;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  اگر(!stream)
    بازگشت CURLE_RECV_ERROR;

  result = cf_process_ingress(cf, data);
  اگر(result) {
    CURL_TRC_CF(data, cf, "cf_recv, error on ingress");
    برو out;
  }

  اگر(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
    برو out;
  }
  وگرنه اگر(stream->closed)
    result = recv_closed_stream(cf, data, pnread);
  وگرنه اگر(quiche_conn_is_draining(ctx->qconn)) {
    failf(data, "QUIC connection is draining");
    result = CURLE_HTTP3;
  }
  وگرنه
    result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));
  اگر(*pnread > 0)
    ctx->data_recvd += *pnread;
  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, stream->id, blen, (صحیح)result, *pnread,
              ctx->data_recvd);
  بازگشت result;
}

ایستا CURLcode cf_quiche_send_body(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    ساختار h3_stream_ctx *stream,
                                    ثابت uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, stream->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  اگر(rv == QUICHE_H3_ERR_DONE || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    اگر(!quiche_conn_stream_writable(ctx->qconn, stream->id, len)) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", stream->id, len);
      stream->quic_flow_blocked = TRUE;
    }
    بازگشت CURLE_AGAIN;
  }
  وگرنه اگر(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_STREAM_STATE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", stream->id, len);
    بازگشت CURLE_HTTP3;
  }
  وگرنه اگر(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_SIZE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                stream->id, len);
    بازگشت CURLE_SEND_ERROR;
  }
  وگرنه اگر(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                stream->id, len, rv);
    بازگشت CURLE_SEND_ERROR;
  }
  وگرنه {
    اگر(eos && (len == *pnwritten))
      stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                stream->id, len, stream->send_closed, *pnwritten);
    بازگشت CURLE_OK;
  }
}

ایستا CURLcode h3_open_stream(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ثابت uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  size_t nheader, i;
  int64_t rv;
  ساختار dynhds h2_headers;
  quiche_h3_header *nva = پوچ;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  اگر(!stream) {
    result = h3_data_setup(cf, data);
    اگر(result)
      بازگشت result;
    stream = H3_STREAM_CTX(ctx, data);
    DEBUGASSERT(stream);
  }

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(stream);

  result = Curl_h1_req_parse_read(&stream->h1, buf, blen, پوچ,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : پوچ,
    0, pnwritten);
  اگر(result)
    برو out;
  اگر(!stream->h1.done) {
    /* need more data */
    برو out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  اگر(result)
    برو out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_شمار(&h2_headers);
  nva = curlx_malloc(اندازه(quiche_h3_header) * nheader);
  اگر(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  برای(i = 0; i < nheader; ++i) {
    ساختار dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned نویسه *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].مقدار = (unsigned نویسه *)e->مقدار;
    nva[i].مقدار_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  اگر(eos && !blen)
    stream->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              stream->send_closed);
  CURL_TRC_CF(data, cf, "quiche_send_request() -> %" PRId64, rv);
  اگر(rv < 0) {
    اگر(QUICHE_H3_ERR_STREAM_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] blocked", stream->id);
      stream->quic_flow_blocked = TRUE;
      result = CURLE_AGAIN;
      برو out;
    }
    وگرنه {
      CURL_TRC_CF(data, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&data->state.url), rv);
    }
    result = CURLE_SEND_ERROR;
    برو out;
  }

  DEBUGASSERT(!stream->opened);
  stream->id = (uint64_t)rv;
  stream->opened = TRUE;
  stream->closed = FALSE;
  stream->reset = FALSE;

  اگر(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    برای(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", stream->id,
            (صحیح)nva[i].name_len, nva[i].name,
            (صحیح)nva[i].مقدار_len, nva[i].مقدار);
    }
  }

  اگر(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, data, stream, buf, blen, eos, &nwritten);
    اگر(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      result = r2;
    }
    وگرنه اگر(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  بازگشت result;
}

ایستا CURLcode cf_quiche_send(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result;

  *pnwritten = 0;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  result = cf_process_ingress(cf, data);
  اگر(result)
    برو out;

  اگر(!stream || !stream->opened) {
    result = h3_open_stream(cf, data, buf, len, eos, pnwritten);
    اگر(result)
      برو out;
    stream = H3_STREAM_CTX(ctx, data);
  }
  وگرنه اگر(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
  }
  وگرنه اگر(stream->closed) {
    اگر(stream->resp_hds_complete) {
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
      برو out;
    }
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    برو out;
  }
  وگرنه {
    result = cf_quiche_send_body(cf, data, stream, buf, len, eos, pnwritten);
  }

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));

  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : (uint64_t)~0, len,
              (صحیح)result, *pnwritten);
  بازگشت result;
}

ایستا bool stream_is_writable(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  بازگشت stream && (quiche_conn_stream_writable(
    ctx->qconn, stream->id, 1) > 0);
}

ایستا CURLcode cf_quiche_adjust_pollset(ساختار Curl_cfilter *cf,
                                         ساختار Curl_easy *data,
                                         ساختار easy_pollset *ps)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  اگر(!ctx->qconn)
    بازگشت CURLE_OK;

  Curl_pollset_check(data, ps, ctx->q.sockfd, &want_recv, &want_send);
  اگر(want_recv || want_send) {
    ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
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
  بازگشت result;
}

ایستا CURLcode h3_data_pause(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (تهی)cf;
  اگر(!pause) {
    Curl_multi_mark_dirty(data);
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode cf_quiche_cntrl(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                صحیح event, صحیح arg1, تهی *arg2)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (تهی)arg1;
  (تهی)arg2;
  انتخاب(event) {
  حالت CF_CTRL_DATA_SETUP:
    بشکن;
  حالت CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    بشکن;
  حالت CF_CTRL_DATA_DONE:
    h3_data_done(cf, data);
    بشکن;
  حالت CF_CTRL_DATA_DONE_SEND: {
    ساختار h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    اگر(stream && !stream->send_closed) {
      unsigned نویسه body[1];
      size_t sent;

      stream->send_closed = TRUE;
      body[0] = 'X';
      result = cf_quiche_send(cf, data, body, 0, TRUE, &sent);
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  stream->id, (صحیح)result, sent);
    }
    بشکن;
  }
  حالت CF_CTRL_CONN_INFO_UPDATE:
    اگر(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    بشکن;
  حالت CF_CTRL_REPORT_STATS:
    اگر(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    بشکن;
  پیشفرض:
    بشکن;
  }
  بازگشت result;
}

ایستا CURLcode cf_quiche_ctx_open(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  صحیح rv;
  CURLcode result;
  ثابت ساختار Curl_sockaddr_ex *sockaddr;
  ایستا ثابت ساختار alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  اگر(result)
    بازگشت result;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_VERSION);
  اگر(!ctx->cfg) {
    failf(data, "cannot create quiche config");
    بازگشت CURLE_FAILED_INIT;
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
                               &ALPN_SPEC_H3, پوچ, پوچ, cf, پوچ);
  اگر(result)
    بازگشت result;

  result = Curl_rand(data, ctx->scid, اندازه(ctx->scid));
  اگر(result)
    بازگشت result;

  اگر(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, پوچ))
    بازگشت CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = اندازه(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (ساختار sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  اگر(rv == -1)
    بازگشت CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((ثابت uint8_t *)ctx->scid,
                                        اندازه(ctx->scid), پوچ, 0,
                                        (ساختار sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  اگر(!ctx->qconn) {
    failf(data, "cannot create quiche connection");
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  /* Known to not work on Windows */
#اگر !defined(_WIN32) && defined(HAVE_QUICHE_CONN_SET_QLOG_FD)
  {
    صحیح qfd;
    (تهی)Curl_qlogdir(data, ctx->scid, اندازه(ctx->scid), &qfd);
    اگر(qfd != -1)
      quiche_conn_set_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#endif

  result = cf_flush_egress(cf, data);
  اگر(result)
    بازگشت result;

  {
    unsigned نویسه alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    unsigned alpn_len, offset = 0;

    /* Replace each ALPN length prefix by a comma. */
    وقتی(offset < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[offset];
      alpn_protocols[offset] = ',';
      offset += 1 + alpn_len;
    }

    CURL_TRC_CF(data, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode cf_quiche_verify_peer(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  بازگشت Curl_vquic_tls_verify_peer(&ctx->tls, cf, data, &ctx->ssl_peer);
}

ایستا CURLcode cf_quiche_connect(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  bool *done)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  /* Connect the UDP filter first */
  اگر(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    اگر(result || !*done)
      بازگشت result;
  }

  *done = FALSE;
  اگر(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    بازگشت CURLE_OK;
  }

  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  اگر(!ctx->qconn) {
    result = cf_quiche_ctx_open(cf, data);
    اگر(result)
      برو out;
    ctx->started_at = ctx->q.last_op;
    result = cf_flush_egress(cf, data);
    /* we do not expect to be able to recv anything yet */
    برو out;
  }

  result = cf_process_ingress(cf, data);
  اگر(result)
    برو out;

  result = cf_flush_egress(cf, data);
  اگر(result)
    برو out;

  اگر(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.last_op;
    CURL_TRC_CF(data, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    result = cf_quiche_verify_peer(cf, data);
    اگر(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      اگر(!ctx->h3config) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_set_max_field_section_size(ctx->h3config,
                                                  MAX_HTTP_RESP_HEADER_SIZE);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      اگر(!ctx->h3c) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
      cf->connected = TRUE;
      *done = TRUE;
    }
  }
  وگرنه اگر(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    result = CURLE_WEIRD_SERVER_REPLY;
  }

out:
#ifdef CURLVERBOSE
  اگر(result && result != CURLE_AGAIN) {
    ساختار ip_quadruple ip;

    اگر(!Curl_cf_socket_peek(cf->next, data, پوچ, پوچ, &ip))
      infof(data, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    وگرنه
      infof(data, "connect failed: %s", curl_easy_strerror(result));
  }
#endif
  بازگشت result;
}

ایستا CURLcode cf_quiche_shutdown(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data, bool *done)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  اگر(cf->shutdown || !ctx || !ctx->qconn) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  *done = FALSE;
  اگر(!ctx->shutdown_started) {
    صحیح err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, پوچ, 0);
    اگر(err) {
      CURL_TRC_CF(data, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      result = CURLE_SEND_ERROR;
      برو out;
    }
  }

  اگر(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
    result = cf_flush_egress(cf, data);
    اگر(result)
      برو out;
  }

  اگر(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
  }
  وگرنه {
    CURL_TRC_CF(data, cf, "shutdown sending blocked");
  }

out:
  بازگشت result;
}

ایستا تهی cf_quiche_destroy(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  (تهی)data;
  اگر(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = پوچ;
  }
}

ایستا CURLcode cf_quiche_query(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                صحیح query, صحیح *pres1, تهی *pres2)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;

  انتخاب(query) {
  حالت CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    اگر(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_چپ_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > INT_MAX) ? INT_MAX : (صحیح)max_streams;
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    بازگشت CURLE_OK;
  }
  حالت CF_QUERY_CONNECT_REPLY_MS:
    اگر(ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (صحیح)ms : INT_MAX;
    }
    وگرنه
      *pres1 = -1;
    بازگشت CURLE_OK;
  حالت CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    بازگشت CURLE_OK;
  حالت CF_QUERY_SSL_INFO:
  حالت CF_QUERY_SSL_CTX_INFO: {
    ساختار curl_tlssessioninfo *info = pres2;
    اگر(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      بازگشت CURLE_OK;
    بشکن;
  }
  حالت CF_QUERY_ALPN_NEGOTIATED: {
    ثابت نویسه **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : پوچ;
    بازگشت CURLE_OK;
  }
  پیشفرض:
    بشکن;
  }
  بازگشت cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ایستا bool cf_quiche_conn_is_alive(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    bool *درون_داد_pending)
{
  ساختار cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *درون_داد_pending = FALSE;
  اگر(!ctx->qconn)
    بازگشت FALSE;

  اگر(quiche_conn_is_closed(ctx->qconn)) {
    اگر(quiche_conn_is_timed_out(ctx->qconn))
      CURL_TRC_CF(data, cf, "connection was closed due to idle timeout");
    وگرنه
      CURL_TRC_CF(data, cf, "connection is closed");
    بازگشت FALSE;
  }

  اگر(!cf->next || !cf->next->cft->is_alive(cf->next, data, درون_داد_pending))
    بازگشت FALSE;

  اگر(*درون_داد_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *درون_داد_pending = FALSE;
    اگر(cf_process_ingress(cf, data))
      alive = FALSE;
    وگرنه {
      alive = TRUE;
    }
  }

  بازگشت alive;
}

ساختار Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_quiche_create(ساختار Curl_cfilter **pcf,
                               ساختار Curl_easy *data,
                               ساختار Curl_peer *origin,
                               ساختار Curl_peer *peer,
                               ساختار connectdata *conn,
                               ساختار Curl_sockaddr_ex *addr)
{
  ساختار cf_quiche_ctx *ctx = پوچ;
  ساختار Curl_cfilter *cf = پوچ;
  CURLcode result;

  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  اگر(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  اگر(result)
    برو out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, پوچ, TRNSPRT_QUIC);
  اگر(result)
    برو out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : پوچ;
  اگر(result) {
    اگر(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    وگرنه اگر(ctx)
      cf_quiche_ctx_free(ctx);
  }

  بازگشت result;
}

CURLcode Curl_cf_quiche_درج_after(ساختار Curl_cfilter *cf_at,
                                     ساختار Curl_peer *origin,
                                     ساختار Curl_peer *peer,
                                     ساختار Curl_cfilter **pcf)
{
  ساختار cf_quiche_ctx *ctx = پوچ;
  ساختار Curl_cfilter *cf = پوچ;
  CURLcode result;

  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  اگر(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  اگر(result)
    برو out;
  Curl_conn_cf_درج_after(cf_at, cf);

out:
  اگر(result) {
    curlx_safefree(cf);
    اگر(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  بازگشت result;
}

#endif
