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

#ܐܢ !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#ضمن <quiche.h>
#ضمن <openssl/err.h>
#ضمن <openssl/ssl.h>

#ضمن "bufq.h"
#ضمن "u32_ptrset.h"
#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "cf-socket.h"
#ضمن "curl_trc.h"
#ضمن "rand.h"
#ضمن "multiif.h"
#ضمن "connect.h"
#ضمن "progress.h"
#ضمن "select.h"
#ضمن "http.h"
#ضمن "http1.h"
#ضمن "sockaddr.h"
#ضمن "vdns/cf-dns.h"
#ضمن "vquic/vquic.h"
#ضمن "vquic/vquic_int.h"
#ضمن "vquic/vquic-tls.h"
#ضمن "vquic/cf-quiche.h"
#ضمن "transfer.h"
#ضمن "url.h"
#ضمن "bufref.h"
#ضمن "vtls/openssl.h"
#ضمن "vtls/vtls.h"

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
فراغ Curl_quiche_ver(حرف *p, size_t len)
{
  (فراغ)curl_msnprintf(p, len, "quiche/%s", quiche_version());
}

بنية cf_quiche_ctx {
  بنية cf_quic_ctx q;
  بنية ssl_peer ssl_peer;
  بنية curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  بنية curltime started_at;        /* time the current attempt started */
  بنية curltime handshake_at;      /* time connect handshake finished */
  بنية u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  بنية dynbuf h1hdr;               /* temp buffer for header construction */
  بنية bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t data_recvd;
  BIT(initialized);
  BIT(goaway);                       /* got GOAWAY from server */
  BIT(x509_store_setup);             /* if x509 store has been set up */
  BIT(shutdown_started);             /* queued shutdown packets */
  BIT(stats_reported);               /* connect statistics reported */
};

#ifdef DEBUG_QUICHE
/* initialize debug log callback only once */
ساكن صحيح debug_log_init = 0;
ساكن فراغ quiche_debug_log(ثابت حرف *line, فراغ *argp)
{
  (فراغ)argp;
  curl_mfprintf(stderr, "%s\n", line);
}
#endif

ساكن فراغ h3_stream_set_free(uint32_t id, فراغ *stream);

ساكن CURLcode cf_quiche_ctx_init(بنية cf_quiche_ctx *ctx,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer,
                                   بنية ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#ifdef DEBUG_QUICHE
  ܐܢ(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, عدم);
    debug_log_init = 1;
  }
#endif
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  Curl_bufq_init2(&ctx->writebuf, H3_STREAM_CHUNK_SIZE, H3_STREAM_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  ctx->data_recvd = 0;
  ctx->initialized = TRUE;
  ܦܢܐ Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

ساكن فراغ cf_quiche_ctx_free(بنية cf_quiche_ctx *ctx)
{
  ܐܢ(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = عدم;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

ساكن فراغ cf_quiche_ctx_close(بنية cf_quiche_ctx *ctx)
{
  ܐܢ(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = عدم;
  }
  ܐܢ(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = عدم;
  }
  ܐܢ(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = عدم;
  }
  ܐܢ(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = عدم;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

ساكن CURLcode cf_flush_egress(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data);

/**
 * All about the H3 internals of a stream
 */
بنية h3_stream_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  بنية h1_req_parser h1; /* h1 request parsing */
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

ساكن فراغ h3_stream_ctx_free(بنية h3_stream_ctx *stream)
{
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

ساكن فراغ h3_stream_set_free(uint32_t id, فراغ *stream)
{
  (فراغ)id;
  DEBUGASSERT(stream);
  h3_stream_ctx_free((بنية h3_stream_ctx *)stream);
}

تعريفنوع bool cf_quiche_svisit(بنية Curl_cfilter *cf,
                              بنية Curl_easy *sdata,
                              بنية h3_stream_ctx *stream,
                              فراغ *user_data);

بنية cf_quiche_visit_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_multi *multi;
  cf_quiche_svisit *cb;
  فراغ *user_data;
};

ساكن bool cf_quiche_stream_do(uint32_t mid, فراغ *val, فراغ *user_data)
{
  بنية cf_quiche_visit_ctx *vctx = user_data;
  بنية h3_stream_ctx *stream = val;
  بنية Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  ܐܢ(sdata)
    ܦܢܐ vctx->cb(vctx->cf, sdata, stream, vctx->user_data);
  ܦܢܐ TRUE;
}

ساكن فراغ cf_quiche_for_all_streams(بنية Curl_cfilter *cf,
                                      بنية Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      فراغ *user_data)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية cf_quiche_visit_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_data = user_data;
  Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_stream_do, &vctx);
}

ساكن bool cf_quiche_do_resume(بنية Curl_cfilter *cf,
                                بنية Curl_easy *sdata,
                                بنية h3_stream_ctx *stream,
                                فراغ *user_data)
{
  (فراغ)user_data;
  ܐܢ(stream->quic_flow_blocked) {
    stream->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", stream->id);
  }
  ܦܢܐ TRUE;
}

ساكن bool cf_quiche_do_expire(بنية Curl_cfilter *cf,
                                بنية Curl_easy *sdata,
                                بنية h3_stream_ctx *stream,
                                فراغ *user_data)
{
  (فراغ)user_data;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  stream->xfer_result = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  ܦܢܐ TRUE;
}

ساكن CURLcode h3_data_setup(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  ܐܢ(stream)
    ܦܢܐ CURLE_OK;

  stream = curlx_calloc(1, حجم(*stream));
  ܐܢ(!stream)
    ܦܢܐ CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_LINE_LEN);

  ܐܢ(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h3_stream_ctx_free(stream);
    ܦܢܐ CURLE_OUT_OF_MEMORY;
  }

  ܦܢܐ CURLE_OK;
}

ساكن فراغ cf_quiche_stream_close(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data,
                                   بنية h3_stream_ctx *stream)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result;

  ܐܢ(ctx->qconn && !stream->closed) {
    quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    ܐܢ(!stream->send_closed) {
      quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      stream->send_closed = TRUE;
    }
    stream->closed = TRUE;
    result = cf_flush_egress(cf, data);
    ܐܢ(result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  stream->id, (صحيح)result);
  }
}

ساكن فراغ h3_data_done(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  (فراغ)cf;
  ܐܢ(stream) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] easy handle is done", stream->id);
    cf_quiche_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
  }
}

ساكن فراغ cf_quiche_expire_conn_closed(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *data)
{
  DEBUGASSERT(data->multi);
  CURL_TRC_CF(data, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_expire, عدم);
}

ساكن فراغ cf_quiche_write_hd(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               بنية h3_stream_ctx *stream,
                               ثابت حرف *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ܐܢ(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    ܐܢ(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (صحيح)stream->xfer_result,
                  blen);
  }
}

بنية cb_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *data;
  بنية h3_stream_ctx *stream;
};

ساكن bool is_valid_h3_header(ثابت uint8_t *hdr, size_t hlen)
{
  ܟܡܐ(hlen--) {
    بدل(*hdr++) {
    حالة '\n':
    حالة '\r':
    حالة '\0':
      ܦܢܐ FALSE;
    }
  }
  ܦܢܐ TRUE;
}

ساكن صحيح cb_each_header(uint8_t *name, size_t name_len,
                          uint8_t *value, size_t value_len,
                          فراغ *argp)
{
  بنية cb_ctx *x = argp;
  بنية Curl_cfilter *cf = x->cf;
  بنية Curl_easy *data = x->data;
  بنية h3_stream_ctx *stream = x->stream;
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ܐܢ(!stream || stream->xfer_result)
    ܦܢܐ 1; /* abort iteration */

  ܐܢ((name_len == 7) && !strncmp(HTTP_PSEUDO_STATUS, (حرف *)name, 7) &&
     is_valid_h3_header(value, value_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    result = Curl_http_decode_status(&stream->status_code,
                                     (ثابت حرف *)value, value_len);
    ܐܢ(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    ܐܢ(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)value, value_len);
    ܐܢ(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    ܐܢ(!result)
      cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] status: %s",
                stream->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  ܐܚܪܢܐ {
    ܐܢ(is_valid_h3_header(value, value_len) &&
       is_valid_h3_header(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  stream->id, (صحيح)name_len, name,
                  (صحيح)value_len, value);
      curlx_dyn_reset(&ctx->h1hdr);
      result = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)name, name_len);
      ܐܢ(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      ܐܢ(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)value, value_len);
      ܐܢ(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      ܐܢ(!result)
        cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    ܐܚܪܢܐ
      CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  stream->id, value_len + name_len);
  }

  ܐܢ(result) {
    CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] on header error %d",
                stream->id, (صحيح)result);
    ܐܢ(!stream->xfer_result)
      stream->xfer_result = result;
  }
  ܦܢܐ stream->xfer_result ? 1 : 0;
}

ساكن CURLcode stream_resp_read(فراغ *reader_ctx,
                                 unsigned حرف *buf, size_t len,
                                 size_t *pnread)
{
  بنية cb_ctx *x = reader_ctx;
  بنية cf_quiche_ctx *ctx = x->cf->ctx;
  بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, x->data);
  ssize_t nread;

  *pnread = 0;
  ܐܢ(!stream)
    ܦܢܐ CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, stream->id, buf, len);
  ܐܢ(!curlx_sztouz(nread, pnread))
    ܦܢܐ CURLE_AGAIN;
  ܦܢܐ CURLE_OK;
}

ساكن فراغ cf_quiche_flush_body(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 بنية h3_stream_ctx *stream)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  ثابت uint8_t *buf;
  size_t blen;

  ܟܡܐ(stream && !stream->xfer_result) {
    ܐܢ(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      stream->xfer_result = Curl_xfer_write_resp(
        data, (ثابت حرف *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      ܐܢ(stream->xfer_result) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", stream->id, (صحيح)stream->xfer_result, blen);
      }
    }
    ܐܚܪܢܐ
      اكسر;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

ساكن فراغ cf_quiche_recv_body(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                بنية h3_stream_ctx *stream)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  بنية cb_ctx cb_ctx;
  CURLcode result = CURLE_OK;

  ܐܢ(!stream)
    ܦܢܐ;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.data = data;
  cb_ctx.stream = stream;
  Curl_bufq_reset(&ctx->writebuf);
  ܟܡܐ(!result) {
    result = Curl_bufq_slurp(&ctx->writebuf,
                             stream_resp_read, &cb_ctx, &nread);
    ܐܢ(!result)
      cf_quiche_flush_body(cf, data, stream);
    ܐܚܪܢܐ ܐܢ(result == CURLE_AGAIN)
      اكسر;
    ܐܚܪܢܐ ܐܢ(result) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv_body error %d",
                  stream->id, (صحيح)result);
      failf(data, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            stream->id, (صحيح)result);
      stream->closed = TRUE;
      stream->reset = TRUE;
      stream->send_closed = TRUE;
      ܐܢ(!stream->xfer_result)
        stream->xfer_result = result;
    }
  }
  cf_quiche_flush_body(cf, data, stream);
}

ساكن فراغ cf_quiche_process_ev(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 بنية h3_stream_ctx *stream,
                                 quiche_h3_event *ev)
{
  تعداد quiche_h3_event_type type;

  ܐܢ(!stream)
    ܦܢܐ;

  type = quiche_h3_event_type(ev);

  بدل(type) {
  حالة QUICHE_H3_EVENT_HEADERS: {
    بنية cb_ctx cb_ctx;
    stream->resp_got_header = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.data = data;
    cb_ctx.stream = stream;
    quiche_h3_event_for_each_header(ev, cb_each_header, &cb_ctx);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [HEADERS]", stream->id);
    Curl_multi_mark_dirty(data);
    اكسر;
  }
  حالة QUICHE_H3_EVENT_DATA:
    ܐܢ(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, data, stream);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [DATA]", stream->id);
    Curl_multi_mark_dirty(data);
    اكسر;

  حالة QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] RESET", stream->id);
    stream->closed = TRUE;
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    Curl_multi_mark_dirty(data);
    اكسر;

  حالة QUICHE_H3_EVENT_FINISHED:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] CLOSED", stream->id);
    ܐܢ(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, TRUE);
    }
    stream->closed = TRUE;
    Curl_multi_mark_dirty(data);
    اكسر;

  حالة QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [GOAWAY]", stream->id);
    اكسر;

  افتراضي:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv, unhandled event %d",
                stream->id, (صحيح)type);
    اكسر;
  }
}

بنية cf_quiche_disp_ctx {
  uint64_t stream_id;
  بنية Curl_cfilter *cf;
  بنية Curl_multi *multi;
  quiche_h3_event *ev;
};

ساكن bool cf_quiche_disp_event(uint32_t mid, فراغ *val, فراغ *user_data)
{
  بنية cf_quiche_disp_ctx *dctx = user_data;
  بنية h3_stream_ctx *stream = val;

  ܐܢ(stream->id == dctx->stream_id) {
    بنية Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    ܐܢ(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, stream, dctx->ev);
    ܦܢܐ FALSE; /* stop iterating */
  }
  ܦܢܐ TRUE;
}

ساكن CURLcode cf_poll_events(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = عدم;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  ܟܡܐ(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    ܐܢ(rv == QUICHE_H3_ERR_DONE) {
      اكسر;
    }
    ܐܚܪܢܐ ܐܢ(rv < 0) {
      CURL_TRC_CF(data, cf, "error poll: %" PRId64, rv);
      ܦܢܐ CURLE_HTTP3;
    }
    ܐܚܪܢܐ {
      stream = H3_STREAM_CTX(ctx, data);
      ܐܢ(stream && stream->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, data, stream, ev);
        quiche_h3_event_free(ev);
        ܐܢ(stream->xfer_result)
          ܦܢܐ stream->xfer_result;
      }
      ܐܚܪܢܐ {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        بنية cf_quiche_disp_ctx dctx;
        dctx.stream_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = data->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  ܦܢܐ CURLE_OK;
}

بنية recv_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *data;
  صحيح pkts;
};

ساكن CURLcode cf_quiche_recv_pkts(ثابت unsigned حرف *buf, size_t buflen,
                                    size_t gso_size,
                                    بنية sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    فراغ *userp)
{
  بنية recv_ctx *r = userp;
  بنية cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, offset, nread;
  ssize_t rv;

  (فراغ)ecn;

  recv_info.to = (بنية sockaddr *)&ctx->q.local_addr;
  recv_info.to_len = ctx->q.local_addrlen;
  recv_info.from = (بنية sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  لكل(offset = 0; offset < buflen; offset += gso_size) {
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = quiche_conn_recv(ctx->qconn,
                          (unsigned حرف *)CURL_UNCONST(buf + offset),
                          pktlen, &recv_info);
    ܐܢ(!curlx_sztouz(rv, &nread)) {
      ܐܢ(QUICHE_ERR_DONE == rv) {
        ܐܢ(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is draining");
          ܦܢܐ CURLE_RECV_ERROR;
        }
        ܐܢ(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is closed");
          ܦܢܐ CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->data, r->cf, "ingress, quiche is DONE");
        ܦܢܐ CURLE_OK;
      }
      ܐܚܪܢܐ ܐܢ(QUICHE_ERR_TLS_FAIL == rv) {
        long verify_ok = SSL_get_verify_result(ctx->tls.ossl.ssl);
        ܐܢ(verify_ok != X509_V_OK) {
          failf(r->data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          ܦܢܐ CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->data, "ingress, quiche reports TLS fail");
        ܦܢܐ CURLE_RECV_ERROR;
      }
      ܐܚܪܢܐ {
        failf(r->data, "quiche reports error %zd on receive", rv);
        ܦܢܐ CURLE_RECV_ERROR;
      }
    }
    ܐܚܪܢܐ ܐܢ(nread < pktlen) {
      CURL_TRC_CF(r->data, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  ܦܢܐ CURLE_OK;
}

ساكن CURLcode cf_process_ingress(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية recv_ctx rctx;
  CURLcode result;

  DEBUGASSERT(ctx->qconn);
  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  ܐܢ(result)
    ܦܢܐ result;

  rctx.cf = cf;
  rctx.data = data;
  rctx.pkts = 0;

  result = Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  ܐܢ(result)
    ܦܢܐ result;

  ܐܢ(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(data->multi);
    cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_resume, عدم);
  }
  ܦܢܐ cf_poll_events(cf, data);
}

بنية read_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *data;
  quiche_send_info send_info;
};

ساكن CURLcode read_pkt_to_send(فراغ *userp,
                                 unsigned حرف *buf, size_t buflen,
                                 size_t *pnread)
{
  بنية read_ctx *x = userp;
  بنية cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  ܐܢ(rv == QUICHE_ERR_DONE)
    ܦܢܐ CURLE_AGAIN;

  ܐܢ(!curlx_sztouz(rv, pnread)) {
    failf(x->data, "quiche_conn_send returned %zd", rv);
    ܦܢܐ CURLE_SEND_ERROR;
  }
  ܦܢܐ CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
ساكن CURLcode cf_flush_egress(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode result;
  int64_t expiry_ns;
  int64_t timeout_ns;
  بنية read_ctx readx;
  size_t pkt_count, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  ܐܢ(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    ܐܢ(quiche_conn_is_closed(ctx->qconn)) {
      ܐܢ(quiche_conn_is_timed_out(ctx->qconn))
        failf(data, "connection closed by idle timeout");
      ܐܚܪܢܐ
        failf(data, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, data);
      ܦܢܐ CURLE_SEND_ERROR;
    }
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  ܐܢ(result) {
    ܐܢ(result == CURLE_AGAIN) {
      Curl_expire(data, 1, EXPIRE_QUIC);
      ܦܢܐ CURLE_OK;
    }
    ܦܢܐ result;
  }

  readx.cf = cf;
  readx.data = data;
  memset(&readx.send_info, 0, حجم(readx.send_info));
  pkt_count = 0;
  gsolen = quiche_conn_max_send_udp_payload_size(ctx->qconn);
  لكل(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_to_send, &readx, &nread);
    ܐܢ(result) {
      ܐܢ(result != CURLE_AGAIN)
        ܦܢܐ result;
      /* Nothing more to add, flush and leave */
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      ܐܢ(result) {
        ܐܢ(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          ܦܢܐ CURLE_OK;
        }
        ܦܢܐ result;
      }
      اذهب out;
    }

    ++pkt_count;
    ܐܢ(nread < gsolen || pkt_count >= MAX_PKT_BURST) {
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      ܐܢ(result) {
        ܐܢ(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          ܦܢܐ CURLE_OK;
        }
        اذهب out;
      }
      pkt_count = 0;
    }
  }

out:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  ܐܢ(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(data, (timeout_ns / 1000000), EXPIRE_QUIC);
  ܦܢܐ result;
}

ساكن CURLcode recv_closed_stream(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data,
                                   size_t *pnread)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(stream);
  *pnread = 0;
  ܐܢ(stream->reset) {
    ܐܢ(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      ܦܢܐ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ܐܚܪܢܐ ܐܢ(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        ܦܢܐ CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                stream->id, (صحيح)result);
  }
  ܐܚܪܢܐ ܐܢ(!stream->resp_got_header) {
    failf(data, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    result = CURLE_HTTP3;
  }
  ܦܢܐ result;
}

ساكن CURLcode cf_quiche_recv(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                               حرف *buf, size_t blen, size_t *pnread)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  *pnread = 0;
  (فراغ)buf;
  (فراغ)blen;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  ܐܢ(!stream)
    ܦܢܐ CURLE_RECV_ERROR;

  result = cf_process_ingress(cf, data);
  ܐܢ(result) {
    CURL_TRC_CF(data, cf, "cf_recv, error on ingress");
    اذهب out;
  }

  ܐܢ(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
    اذهب out;
  }
  ܐܚܪܢܐ ܐܢ(stream->closed)
    result = recv_closed_stream(cf, data, pnread);
  ܐܚܪܢܐ ܐܢ(quiche_conn_is_draining(ctx->qconn)) {
    failf(data, "QUIC connection is draining");
    result = CURLE_HTTP3;
  }
  ܐܚܪܢܐ
    result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));
  ܐܢ(*pnread > 0)
    ctx->data_recvd += *pnread;
  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, stream->id, blen, (صحيح)result, *pnread,
              ctx->data_recvd);
  ܦܢܐ result;
}

ساكن CURLcode cf_quiche_send_body(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    بنية h3_stream_ctx *stream,
                                    ثابت uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, stream->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  ܐܢ(rv == QUICHE_H3_ERR_DONE || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    ܐܢ(!quiche_conn_stream_writable(ctx->qconn, stream->id, len)) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", stream->id, len);
      stream->quic_flow_blocked = TRUE;
    }
    ܦܢܐ CURLE_AGAIN;
  }
  ܐܚܪܢܐ ܐܢ(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_STREAM_STATE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", stream->id, len);
    ܦܢܐ CURLE_HTTP3;
  }
  ܐܚܪܢܐ ܐܢ(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_SIZE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                stream->id, len);
    ܦܢܐ CURLE_SEND_ERROR;
  }
  ܐܚܪܢܐ ܐܢ(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                stream->id, len, rv);
    ܦܢܐ CURLE_SEND_ERROR;
  }
  ܐܚܪܢܐ {
    ܐܢ(eos && (len == *pnwritten))
      stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                stream->id, len, stream->send_closed, *pnwritten);
    ܦܢܐ CURLE_OK;
  }
}

ساكن CURLcode h3_open_stream(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               ثابت uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  size_t nheader, i;
  int64_t rv;
  بنية dynhds h2_headers;
  quiche_h3_header *nva = عدم;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  ܐܢ(!stream) {
    result = h3_data_setup(cf, data);
    ܐܢ(result)
      ܦܢܐ result;
    stream = H3_STREAM_CTX(ctx, data);
    DEBUGASSERT(stream);
  }

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(stream);

  result = Curl_h1_req_parse_read(&stream->h1, buf, blen, عدم,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : عدم,
    0, pnwritten);
  ܐܢ(result)
    اذهب out;
  ܐܢ(!stream->h1.done) {
    /* need more data */
    اذهب out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  ܐܢ(result)
    اذهب out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_count(&h2_headers);
  nva = curlx_malloc(حجم(quiche_h3_header) * nheader);
  ܐܢ(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  لكل(i = 0; i < nheader; ++i) {
    بنية dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned حرف *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].value = (unsigned حرف *)e->value;
    nva[i].value_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  ܐܢ(eos && !blen)
    stream->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              stream->send_closed);
  CURL_TRC_CF(data, cf, "quiche_send_request() -> %" PRId64, rv);
  ܐܢ(rv < 0) {
    ܐܢ(QUICHE_H3_ERR_STREAM_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] blocked", stream->id);
      stream->quic_flow_blocked = TRUE;
      result = CURLE_AGAIN;
      اذهب out;
    }
    ܐܚܪܢܐ {
      CURL_TRC_CF(data, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&data->state.url), rv);
    }
    result = CURLE_SEND_ERROR;
    اذهب out;
  }

  DEBUGASSERT(!stream->opened);
  stream->id = (uint64_t)rv;
  stream->opened = TRUE;
  stream->closed = FALSE;
  stream->reset = FALSE;

  ܐܢ(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    لكل(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", stream->id,
            (صحيح)nva[i].name_len, nva[i].name,
            (صحيح)nva[i].value_len, nva[i].value);
    }
  }

  ܐܢ(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, data, stream, buf, blen, eos, &nwritten);
    ܐܢ(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      result = r2;
    }
    ܐܚܪܢܐ ܐܢ(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  ܦܢܐ result;
}

ساكن CURLcode cf_quiche_send(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result;

  *pnwritten = 0;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  result = cf_process_ingress(cf, data);
  ܐܢ(result)
    اذهب out;

  ܐܢ(!stream || !stream->opened) {
    result = h3_open_stream(cf, data, buf, len, eos, pnwritten);
    ܐܢ(result)
      اذهب out;
    stream = H3_STREAM_CTX(ctx, data);
  }
  ܐܚܪܢܐ ܐܢ(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
  }
  ܐܚܪܢܐ ܐܢ(stream->closed) {
    ܐܢ(stream->resp_hds_complete) {
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
      اذهب out;
    }
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    اذهب out;
  }
  ܐܚܪܢܐ {
    result = cf_quiche_send_body(cf, data, stream, buf, len, eos, pnwritten);
  }

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));

  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : (uint64_t)~0, len,
              (صحيح)result, *pnwritten);
  ܦܢܐ result;
}

ساكن bool stream_is_writable(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  ܦܢܐ stream && (quiche_conn_stream_writable(
    ctx->qconn, stream->id, 1) > 0);
}

ساكن CURLcode cf_quiche_adjust_pollset(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *data,
                                         بنية easy_pollset *ps)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  ܐܢ(!ctx->qconn)
    ܦܢܐ CURLE_OK;

  Curl_pollset_check(data, ps, ctx->q.sockfd, &want_recv, &want_send);
  ܐܢ(want_recv || want_send) {
    بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
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
  ܦܢܐ result;
}

ساكن CURLcode h3_data_pause(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (فراغ)cf;
  ܐܢ(!pause) {
    Curl_multi_mark_dirty(data);
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode cf_quiche_cntrl(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_DATA_SETUP:
    اكسر;
  حالة CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    اكسر;
  حالة CF_CTRL_DATA_DONE:
    h3_data_done(cf, data);
    اكسر;
  حالة CF_CTRL_DATA_DONE_SEND: {
    بنية h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    ܐܢ(stream && !stream->send_closed) {
      unsigned حرف body[1];
      size_t sent;

      stream->send_closed = TRUE;
      body[0] = 'X';
      result = cf_quiche_send(cf, data, body, 0, TRUE, &sent);
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  stream->id, (صحيح)result, sent);
    }
    اكسر;
  }
  حالة CF_CTRL_CONN_INFO_UPDATE:
    ܐܢ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    اكسر;
  حالة CF_CTRL_REPORT_STATS:
    ܐܢ(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  ܦܢܐ result;
}

ساكن CURLcode cf_quiche_ctx_open(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  صحيح rv;
  CURLcode result;
  ثابت بنية Curl_sockaddr_ex *sockaddr;
  ساكن ثابت بنية alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  ܐܢ(result)
    ܦܢܐ result;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_VERSION);
  ܐܢ(!ctx->cfg) {
    failf(data, "cannot create quiche config");
    ܦܢܐ CURLE_FAILED_INIT;
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
                               &ALPN_SPEC_H3, عدم, عدم, cf, عدم);
  ܐܢ(result)
    ܦܢܐ result;

  result = Curl_rand(data, ctx->scid, حجم(ctx->scid));
  ܐܢ(result)
    ܦܢܐ result;

  ܐܢ(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, عدم))
    ܦܢܐ CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = حجم(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (بنية sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  ܐܢ(rv == -1)
    ܦܢܐ CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((ثابت uint8_t *)ctx->scid,
                                        حجم(ctx->scid), عدم, 0,
                                        (بنية sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  ܐܢ(!ctx->qconn) {
    failf(data, "cannot create quiche connection");
    ܦܢܐ CURLE_OUT_OF_MEMORY;
  }

  /* Known to not work on Windows */
#ܐܢ !defined(_WIN32) && defined(HAVE_QUICHE_CONN_SET_QLOG_FD)
  {
    صحيح qfd;
    (فراغ)Curl_qlogdir(data, ctx->scid, حجم(ctx->scid), &qfd);
    ܐܢ(qfd != -1)
      quiche_conn_set_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#endif

  result = cf_flush_egress(cf, data);
  ܐܢ(result)
    ܦܢܐ result;

  {
    unsigned حرف alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    unsigned alpn_len, offset = 0;

    /* Replace each ALPN length prefix by a comma. */
    ܟܡܐ(offset < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[offset];
      alpn_protocols[offset] = ',';
      offset += 1 + alpn_len;
    }

    CURL_TRC_CF(data, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  ܦܢܐ CURLE_OK;
}

ساكن CURLcode cf_quiche_verify_peer(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  ܦܢܐ Curl_vquic_tls_verify_peer(&ctx->tls, cf, data, &ctx->ssl_peer);
}

ساكن CURLcode cf_quiche_connect(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  bool *done)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ܐܢ(cf->connected) {
    *done = TRUE;
    ܦܢܐ CURLE_OK;
  }

  /* Connect the UDP filter first */
  ܐܢ(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ܐܢ(result || !*done)
      ܦܢܐ result;
  }

  *done = FALSE;
  ܐܢ(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    ܦܢܐ CURLE_OK;
  }

  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  ܐܢ(!ctx->qconn) {
    result = cf_quiche_ctx_open(cf, data);
    ܐܢ(result)
      اذهب out;
    ctx->started_at = ctx->q.last_op;
    result = cf_flush_egress(cf, data);
    /* we do not expect to be able to recv anything yet */
    اذهب out;
  }

  result = cf_process_ingress(cf, data);
  ܐܢ(result)
    اذهب out;

  result = cf_flush_egress(cf, data);
  ܐܢ(result)
    اذهب out;

  ܐܢ(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.last_op;
    CURL_TRC_CF(data, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    result = cf_quiche_verify_peer(cf, data);
    ܐܢ(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      ܐܢ(!ctx->h3config) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_set_max_field_section_size(ctx->h3config,
                                                  MAX_HTTP_RESP_HEADER_SIZE);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      ܐܢ(!ctx->h3c) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
      cf->connected = TRUE;
      *done = TRUE;
    }
  }
  ܐܚܪܢܐ ܐܢ(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    result = CURLE_WEIRD_SERVER_REPLY;
  }

out:
#ifdef CURLVERBOSE
  ܐܢ(result && result != CURLE_AGAIN) {
    بنية ip_quadruple ip;

    ܐܢ(!Curl_cf_socket_peek(cf->next, data, عدم, عدم, &ip))
      infof(data, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    ܐܚܪܢܐ
      infof(data, "connect failed: %s", curl_easy_strerror(result));
  }
#endif
  ܦܢܐ result;
}

ساكن CURLcode cf_quiche_shutdown(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data, bool *done)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ܐܢ(cf->shutdown || !ctx || !ctx->qconn) {
    *done = TRUE;
    ܦܢܐ CURLE_OK;
  }

  *done = FALSE;
  ܐܢ(!ctx->shutdown_started) {
    صحيح err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, عدم, 0);
    ܐܢ(err) {
      CURL_TRC_CF(data, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      result = CURLE_SEND_ERROR;
      اذهب out;
    }
  }

  ܐܢ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
    result = cf_flush_egress(cf, data);
    ܐܢ(result)
      اذهب out;
  }

  ܐܢ(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
  }
  ܐܚܪܢܐ {
    CURL_TRC_CF(data, cf, "shutdown sending blocked");
  }

out:
  ܦܢܐ result;
}

ساكن فراغ cf_quiche_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  (فراغ)data;
  ܐܢ(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_quiche_query(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    ܐܢ(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_left_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > INT_MAX) ? INT_MAX : (صحيح)max_streams;
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    ܦܢܐ CURLE_OK;
  }
  حالة CF_QUERY_CONNECT_REPLY_MS:
    ܐܢ(ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (صحيح)ms : INT_MAX;
    }
    ܐܚܪܢܐ
      *pres1 = -1;
    ܦܢܐ CURLE_OK;
  حالة CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    ܦܢܐ CURLE_OK;
  حالة CF_QUERY_SSL_INFO:
  حالة CF_QUERY_SSL_CTX_INFO: {
    بنية curl_tlssessioninfo *info = pres2;
    ܐܢ(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      ܦܢܐ CURLE_OK;
    اكسر;
  }
  حالة CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : عدم;
    ܦܢܐ CURLE_OK;
  }
  افتراضي:
    اكسر;
  }
  ܦܢܐ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن bool cf_quiche_conn_is_alive(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    bool *input_pending)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *input_pending = FALSE;
  ܐܢ(!ctx->qconn)
    ܦܢܐ FALSE;

  ܐܢ(quiche_conn_is_closed(ctx->qconn)) {
    ܐܢ(quiche_conn_is_timed_out(ctx->qconn))
      CURL_TRC_CF(data, cf, "connection was closed due to idle timeout");
    ܐܚܪܢܐ
      CURL_TRC_CF(data, cf, "connection is closed");
    ܦܢܐ FALSE;
  }

  ܐܢ(!cf->next || !cf->next->cft->is_alive(cf->next, data, input_pending))
    ܦܢܐ FALSE;

  ܐܢ(*input_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *input_pending = FALSE;
    ܐܢ(cf_process_ingress(cf, data))
      alive = FALSE;
    ܐܚܪܢܐ {
      alive = TRUE;
    }
  }

  ܦܢܐ alive;
}

بنية Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_quiche_create(بنية Curl_cfilter **pcf,
                               بنية Curl_easy *data,
                               بنية Curl_peer *origin,
                               بنية Curl_peer *peer,
                               بنية connectdata *conn,
                               بنية Curl_sockaddr_ex *addr)
{
  بنية cf_quiche_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode result;

  ctx = curlx_calloc(1, حجم(*ctx));
  ܐܢ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  ܐܢ(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ܐܢ(result)
    اذهب out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, عدم, TRNSPRT_QUIC);
  ܐܢ(result)
    اذهب out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : عدم;
  ܐܢ(result) {
    ܐܢ(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    ܐܚܪܢܐ ܐܢ(ctx)
      cf_quiche_ctx_free(ctx);
  }

  ܦܢܐ result;
}

CURLcode Curl_cf_quiche_insert_after(بنية Curl_cfilter *cf_at,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     بنية Curl_cfilter **pcf)
{
  بنية cf_quiche_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode result;

  ctx = curlx_calloc(1, حجم(*ctx));
  ܐܢ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  ܐܢ(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ܐܢ(result)
    اذهب out;
  Curl_conn_cf_insert_after(cf_at, cf);

out:
  ܐܢ(result) {
    curlx_safefree(cf);
    ܐܢ(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  ܦܢܐ result;
}

#endif
