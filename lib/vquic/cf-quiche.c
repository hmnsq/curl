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

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#অন্তর্ভাব <quiche.h>
#অন্তর্ভাব <openssl/err.h>
#অন্তর্ভাব <openssl/ssl.h>

#অন্তর্ভাব "bufq.h"
#অন্তর্ভাব "u32_ptrset.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-socket.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "http.h"
#অন্তর্ভাব "http1.h"
#অন্তর্ভাব "sockaddr.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "vquic/vquic.h"
#অন্তর্ভাব "vquic/vquic_int.h"
#অন্তর্ভাব "vquic/vquic-tls.h"
#অন্তর্ভাব "vquic/cf-quiche.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "vtls/openssl.h"
#অন্তর্ভাব "vtls/vtls.h"

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
শূন্য Curl_quiche_ver(বর্ণ *p, size_t len)
{
  (শূন্য)curl_msnprintf(p, len, "quiche/%s", quiche_version());
}

সংরচনা cf_quiche_ctx {
  সংরচনা cf_quic_ctx q;
  সংরচনা ssl_peer ssl_peer;
  সংরচনা curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  সংরচনা curltime started_at;        /* time the current attempt started */
  সংরচনা curltime handshake_at;      /* time connect handshake finished */
  সংরচনা u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  সংরচনা dynbuf h1hdr;               /* temp buffer for header construction */
  সংরচনা bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t data_recvd;
  BIT(initialized);
  BIT(goaway);                       /* got GOAWAY from server */
  BIT(x509_store_setup);             /* if x509 store has been set up */
  BIT(shutdown_started);             /* queued shutdown packets */
  BIT(stats_reported);               /* connect statistics reported */
};

#ifdef DEBUG_QUICHE
/* initialize debug log callback only once */
স্থির পূর্ণাঙ্ক debug_log_init = 0;
স্থির শূন্য quiche_debug_log(ধ্রুব বর্ণ *সারি, শূন্য *argp)
{
  (শূন্য)argp;
  curl_mfprintf(stderr, "%s\n", সারি);
}
#endif

স্থির শূন্য h3_stream_set_free(uint32_t id, শূন্য *stream);

স্থির CURLcode cf_quiche_ctx_init(সংরচনা cf_quiche_ctx *ctx,
                                   সংরচনা Curl_peer *origin,
                                   সংরচনা Curl_peer *peer,
                                   সংরচনা ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#ifdef DEBUG_QUICHE
  যদি(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, রিক্ত);
    debug_log_init = 1;
  }
#endif
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  Curl_bufq_init2(&ctx->writebuf, H3_STREAM_CHUNK_SIZE, H3_STREAM_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  ctx->data_recvd = 0;
  ctx->initialized = TRUE;
  প্রত্যাগম Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

স্থির শূন্য cf_quiche_ctx_free(সংরচনা cf_quiche_ctx *ctx)
{
  যদি(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = রিক্ত;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

স্থির শূন্য cf_quiche_ctx_close(সংরচনা cf_quiche_ctx *ctx)
{
  যদি(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = রিক্ত;
  }
  যদি(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = রিক্ত;
  }
  যদি(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = রিক্ত;
  }
  যদি(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = রিক্ত;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

স্থির CURLcode cf_flush_egress(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data);

/**
 * All about the H3 internals of a stream
 */
সংরচনা h3_stream_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  সংরচনা h1_req_parser h1; /* h1 request parsing */
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

স্থির শূন্য h3_stream_ctx_free(সংরচনা h3_stream_ctx *stream)
{
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

স্থির শূন্য h3_stream_set_free(uint32_t id, শূন্য *stream)
{
  (শূন্য)id;
  DEBUGASSERT(stream);
  h3_stream_ctx_free((সংরচনা h3_stream_ctx *)stream);
}

প্রকারনির্দেশ bool cf_quiche_svisit(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *sdata,
                              সংরচনা h3_stream_ctx *stream,
                              শূন্য *user_data);

সংরচনা cf_quiche_visit_ctx {
  সংরচনা Curl_cfilter *cf;
  সংরচনা Curl_multi *multi;
  cf_quiche_svisit *cb;
  শূন্য *user_data;
};

স্থির bool cf_quiche_stream_do(uint32_t mid, শূন্য *val, শূন্য *user_data)
{
  সংরচনা cf_quiche_visit_ctx *vctx = user_data;
  সংরচনা h3_stream_ctx *stream = val;
  সংরচনা Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  যদি(sdata)
    প্রত্যাগম vctx->cb(vctx->cf, sdata, stream, vctx->user_data);
  প্রত্যাগম TRUE;
}

স্থির শূন্য cf_quiche_for_all_streams(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      শূন্য *user_data)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  সংরচনা cf_quiche_visit_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_data = user_data;
  Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_stream_do, &vctx);
}

স্থির bool cf_quiche_do_resume(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *sdata,
                                সংরচনা h3_stream_ctx *stream,
                                শূন্য *user_data)
{
  (শূন্য)user_data;
  যদি(stream->quic_flow_blocked) {
    stream->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", stream->id);
  }
  প্রত্যাগম TRUE;
}

স্থির bool cf_quiche_do_expire(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *sdata,
                                সংরচনা h3_stream_ctx *stream,
                                শূন্য *user_data)
{
  (শূন্য)user_data;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  stream->xfer_result = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  প্রত্যাগম TRUE;
}

স্থির CURLcode h3_data_setup(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  যদি(stream)
    প্রত্যাগম CURLE_OK;

  stream = curlx_calloc(1, পরিমাণ(*stream));
  যদি(!stream)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_সারি_LEN);

  যদি(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h3_stream_ctx_free(stream);
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য cf_quiche_stream_close(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা h3_stream_ctx *stream)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result;

  যদি(ctx->qconn && !stream->closed) {
    quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    যদি(!stream->send_closed) {
      quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      stream->send_closed = TRUE;
    }
    stream->closed = TRUE;
    result = cf_flush_egress(cf, data);
    যদি(result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  stream->id, (পূর্ণাঙ্ক)result);
  }
}

স্থির শূন্য h3_data_done(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  (শূন্য)cf;
  যদি(stream) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] easy handle is done", stream->id);
    cf_quiche_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
  }
}

স্থির শূন্য cf_quiche_expire_conn_closed(সংরচনা Curl_cfilter *cf,
                                         সংরচনা Curl_easy *data)
{
  DEBUGASSERT(data->multi);
  CURL_TRC_CF(data, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_expire, রিক্ত);
}

স্থির শূন্য cf_quiche_write_hd(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               সংরচনা h3_stream_ctx *stream,
                               ধ্রুব বর্ণ *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  যদি(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    যদি(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (পূর্ণাঙ্ক)stream->xfer_result,
                  blen);
  }
}

সংরচনা cb_ctx {
  সংরচনা Curl_cfilter *cf;
  সংরচনা Curl_easy *data;
  সংরচনা h3_stream_ctx *stream;
};

স্থির bool is_valid_h3_header(ধ্রুব uint8_t *hdr, size_t hlen)
{
  যাবত্(hlen--) {
    বিকল্প(*hdr++) {
    পক্ষ '\n':
    পক্ষ '\r':
    পক্ষ '\0':
      প্রত্যাগম FALSE;
    }
  }
  প্রত্যাগম TRUE;
}

স্থির পূর্ণাঙ্ক cb_each_header(uint8_t *name, size_t name_len,
                          uint8_t *মান, size_t মান_len,
                          শূন্য *argp)
{
  সংরচনা cb_ctx *x = argp;
  সংরচনা Curl_cfilter *cf = x->cf;
  সংরচনা Curl_easy *data = x->data;
  সংরচনা h3_stream_ctx *stream = x->stream;
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  যদি(!stream || stream->xfer_result)
    প্রত্যাগম 1; /* abort iteration */

  যদি((name_len == 7) && !strncmp(HTTP_PSEUDO_STATUS, (বর্ণ *)name, 7) &&
     is_valid_h3_header(মান, মান_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    result = Curl_http_decode_status(&stream->status_code,
                                     (ধ্রুব বর্ণ *)মান, মান_len);
    যদি(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    যদি(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, (ধ্রুব বর্ণ *)মান, মান_len);
    যদি(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    যদি(!result)
      cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] status: %s",
                stream->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  অন্যথা {
    যদি(is_valid_h3_header(মান, মান_len) &&
       is_valid_h3_header(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  stream->id, (পূর্ণাঙ্ক)name_len, name,
                  (পূর্ণাঙ্ক)মান_len, মান);
      curlx_dyn_reset(&ctx->h1hdr);
      result = curlx_dyn_addn(&ctx->h1hdr, (ধ্রুব বর্ণ *)name, name_len);
      যদি(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      যদি(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, (ধ্রুব বর্ণ *)মান, মান_len);
      যদি(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      যদি(!result)
        cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    অন্যথা
      CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  stream->id, মান_len + name_len);
  }

  যদি(result) {
    CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] on header error %d",
                stream->id, (পূর্ণাঙ্ক)result);
    যদি(!stream->xfer_result)
      stream->xfer_result = result;
  }
  প্রত্যাগম stream->xfer_result ? 1 : 0;
}

স্থির CURLcode stream_resp_read(শূন্য *reader_ctx,
                                 unsigned বর্ণ *buf, size_t len,
                                 size_t *pnread)
{
  সংরচনা cb_ctx *x = reader_ctx;
  সংরচনা cf_quiche_ctx *ctx = x->cf->ctx;
  সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, x->data);
  ssize_t nread;

  *pnread = 0;
  যদি(!stream)
    প্রত্যাগম CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, stream->id, buf, len);
  যদি(!curlx_sztouz(nread, pnread))
    প্রত্যাগম CURLE_AGAIN;
  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য cf_quiche_flush_body(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা h3_stream_ctx *stream)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  ধ্রুব uint8_t *buf;
  size_t blen;

  যাবত্(stream && !stream->xfer_result) {
    যদি(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      stream->xfer_result = Curl_xfer_write_resp(
        data, (ধ্রুব বর্ণ *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      যদি(stream->xfer_result) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", stream->id, (পূর্ণাঙ্ক)stream->xfer_result, blen);
      }
    }
    অন্যথা
      ভঙ্গ;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

স্থির শূন্য cf_quiche_recv_body(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                সংরচনা h3_stream_ctx *stream)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  সংরচনা cb_ctx cb_ctx;
  CURLcode result = CURLE_OK;

  যদি(!stream)
    প্রত্যাগম;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.data = data;
  cb_ctx.stream = stream;
  Curl_bufq_reset(&ctx->writebuf);
  যাবত্(!result) {
    result = Curl_bufq_slurp(&ctx->writebuf,
                             stream_resp_read, &cb_ctx, &nread);
    যদি(!result)
      cf_quiche_flush_body(cf, data, stream);
    অন্যথা যদি(result == CURLE_AGAIN)
      ভঙ্গ;
    অন্যথা যদি(result) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv_body error %d",
                  stream->id, (পূর্ণাঙ্ক)result);
      failf(data, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            stream->id, (পূর্ণাঙ্ক)result);
      stream->closed = TRUE;
      stream->reset = TRUE;
      stream->send_closed = TRUE;
      যদি(!stream->xfer_result)
        stream->xfer_result = result;
    }
  }
  cf_quiche_flush_body(cf, data, stream);
}

স্থির শূন্য cf_quiche_process_ev(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা h3_stream_ctx *stream,
                                 quiche_h3_event *ev)
{
  গণনা quiche_h3_event_type type;

  যদি(!stream)
    প্রত্যাগম;

  type = quiche_h3_event_type(ev);

  বিকল্প(type) {
  পক্ষ QUICHE_H3_EVENT_HEADERS: {
    সংরচনা cb_ctx cb_ctx;
    stream->resp_got_header = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.data = data;
    cb_ctx.stream = stream;
    quiche_h3_event_for_each_header(ev, cb_each_header, &cb_ctx);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [HEADERS]", stream->id);
    Curl_multi_mark_dirty(data);
    ভঙ্গ;
  }
  পক্ষ QUICHE_H3_EVENT_DATA:
    যদি(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, data, stream);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [DATA]", stream->id);
    Curl_multi_mark_dirty(data);
    ভঙ্গ;

  পক্ষ QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] RESET", stream->id);
    stream->closed = TRUE;
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    Curl_multi_mark_dirty(data);
    ভঙ্গ;

  পক্ষ QUICHE_H3_EVENT_FINISHED:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] CLOSED", stream->id);
    যদি(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, TRUE);
    }
    stream->closed = TRUE;
    Curl_multi_mark_dirty(data);
    ভঙ্গ;

  পক্ষ QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [GOAWAY]", stream->id);
    ভঙ্গ;

  স্বভাব:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv, unhandled event %d",
                stream->id, (পূর্ণাঙ্ক)type);
    ভঙ্গ;
  }
}

সংরচনা cf_quiche_disp_ctx {
  uint64_t stream_id;
  সংরচনা Curl_cfilter *cf;
  সংরচনা Curl_multi *multi;
  quiche_h3_event *ev;
};

স্থির bool cf_quiche_disp_event(uint32_t mid, শূন্য *val, শূন্য *user_data)
{
  সংরচনা cf_quiche_disp_ctx *dctx = user_data;
  সংরচনা h3_stream_ctx *stream = val;

  যদি(stream->id == dctx->stream_id) {
    সংরচনা Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    যদি(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, stream, dctx->ev);
    প্রত্যাগম FALSE; /* stop iterating */
  }
  প্রত্যাগম TRUE;
}

স্থির CURLcode cf_poll_events(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream = রিক্ত;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  যাবত্(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    যদি(rv == QUICHE_H3_ERR_DONE) {
      ভঙ্গ;
    }
    অন্যথা যদি(rv < 0) {
      CURL_TRC_CF(data, cf, "error poll: %" PRId64, rv);
      প্রত্যাগম CURLE_HTTP3;
    }
    অন্যথা {
      stream = H3_STREAM_CTX(ctx, data);
      যদি(stream && stream->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, data, stream, ev);
        quiche_h3_event_free(ev);
        যদি(stream->xfer_result)
          প্রত্যাগম stream->xfer_result;
      }
      অন্যথা {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        সংরচনা cf_quiche_disp_ctx dctx;
        dctx.stream_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = data->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  প্রত্যাগম CURLE_OK;
}

সংরচনা recv_ctx {
  সংরচনা Curl_cfilter *cf;
  সংরচনা Curl_easy *data;
  পূর্ণাঙ্ক pkts;
};

স্থির CURLcode cf_quiche_recv_pkts(ধ্রুব unsigned বর্ণ *buf, size_t buflen,
                                    size_t gso_size,
                                    সংরচনা sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    শূন্য *userp)
{
  সংরচনা recv_ctx *r = userp;
  সংরচনা cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, offset, nread;
  ssize_t rv;

  (শূন্য)ecn;

  recv_info.to = (সংরচনা sockaddr *)&ctx->q.local_addr;
  recv_info.to_len = ctx->q.local_addrlen;
  recv_info.from = (সংরচনা sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  প্রতি(offset = 0; offset < buflen; offset += gso_size) {
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = quiche_conn_recv(ctx->qconn,
                          (unsigned বর্ণ *)CURL_UNCONST(buf + offset),
                          pktlen, &recv_info);
    যদি(!curlx_sztouz(rv, &nread)) {
      যদি(QUICHE_ERR_DONE == rv) {
        যদি(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is draining");
          প্রত্যাগম CURLE_RECV_ERROR;
        }
        যদি(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is closed");
          প্রত্যাগম CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->data, r->cf, "ingress, quiche is DONE");
        প্রত্যাগম CURLE_OK;
      }
      অন্যথা যদি(QUICHE_ERR_TLS_FAIL == rv) {
        long verify_ok = SSL_get_verify_result(ctx->tls.ossl.ssl);
        যদি(verify_ok != X509_V_OK) {
          failf(r->data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          প্রত্যাগম CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->data, "ingress, quiche reports TLS fail");
        প্রত্যাগম CURLE_RECV_ERROR;
      }
      অন্যথা {
        failf(r->data, "quiche reports error %zd on receive", rv);
        প্রত্যাগম CURLE_RECV_ERROR;
      }
    }
    অন্যথা যদি(nread < pktlen) {
      CURL_TRC_CF(r->data, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode cf_process_ingress(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  সংরচনা recv_ctx rctx;
  CURLcode result;

  DEBUGASSERT(ctx->qconn);
  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  যদি(result)
    প্রত্যাগম result;

  rctx.cf = cf;
  rctx.data = data;
  rctx.pkts = 0;

  result = Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  যদি(result)
    প্রত্যাগম result;

  যদি(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(data->multi);
    cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_resume, রিক্ত);
  }
  প্রত্যাগম cf_poll_events(cf, data);
}

সংরচনা read_ctx {
  সংরচনা Curl_cfilter *cf;
  সংরচনা Curl_easy *data;
  quiche_send_info send_info;
};

স্থির CURLcode read_pkt_to_send(শূন্য *userp,
                                 unsigned বর্ণ *buf, size_t buflen,
                                 size_t *pnread)
{
  সংরচনা read_ctx *x = userp;
  সংরচনা cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  যদি(rv == QUICHE_ERR_DONE)
    প্রত্যাগম CURLE_AGAIN;

  যদি(!curlx_sztouz(rv, pnread)) {
    failf(x->data, "quiche_conn_send returned %zd", rv);
    প্রত্যাগম CURLE_SEND_ERROR;
  }
  প্রত্যাগম CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
স্থির CURLcode cf_flush_egress(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode result;
  int64_t expiry_ns;
  int64_t timeout_ns;
  সংরচনা read_ctx readx;
  size_t pkt_গুনতি, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  যদি(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    যদি(quiche_conn_is_closed(ctx->qconn)) {
      যদি(quiche_conn_is_timed_out(ctx->qconn))
        failf(data, "connection closed by idle timeout");
      অন্যথা
        failf(data, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, data);
      প্রত্যাগম CURLE_SEND_ERROR;
    }
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  যদি(result) {
    যদি(result == CURLE_AGAIN) {
      Curl_expire(data, 1, EXPIRE_QUIC);
      প্রত্যাগম CURLE_OK;
    }
    প্রত্যাগম result;
  }

  readx.cf = cf;
  readx.data = data;
  memset(&readx.send_info, 0, পরিমাণ(readx.send_info));
  pkt_গুনতি = 0;
  gsolen = quiche_conn_max_send_udp_payload_size(ctx->qconn);
  প্রতি(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_to_send, &readx, &nread);
    যদি(result) {
      যদি(result != CURLE_AGAIN)
        প্রত্যাগম result;
      /* Nothing more to add, flush and leave */
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      যদি(result) {
        যদি(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          প্রত্যাগম CURLE_OK;
        }
        প্রত্যাগম result;
      }
      গচ্ছ out;
    }

    ++pkt_গুনতি;
    যদি(nread < gsolen || pkt_গুনতি >= MAX_PKT_BURST) {
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      যদি(result) {
        যদি(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          প্রত্যাগম CURLE_OK;
        }
        গচ্ছ out;
      }
      pkt_গুনতি = 0;
    }
  }

out:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  যদি(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(data, (timeout_ns / 1000000), EXPIRE_QUIC);
  প্রত্যাগম result;
}

স্থির CURLcode recv_closed_stream(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   size_t *pnread)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(stream);
  *pnread = 0;
  যদি(stream->reset) {
    যদি(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      প্রত্যাগম CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    অন্যথা যদি(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        প্রত্যাগম CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                stream->id, (পূর্ণাঙ্ক)result);
  }
  অন্যথা যদি(!stream->resp_got_header) {
    failf(data, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    result = CURLE_HTTP3;
  }
  প্রত্যাগম result;
}

স্থির CURLcode cf_quiche_recv(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                               বর্ণ *buf, size_t blen, size_t *pnread)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  *pnread = 0;
  (শূন্য)buf;
  (শূন্য)blen;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  যদি(!stream)
    প্রত্যাগম CURLE_RECV_ERROR;

  result = cf_process_ingress(cf, data);
  যদি(result) {
    CURL_TRC_CF(data, cf, "cf_recv, error on ingress");
    গচ্ছ out;
  }

  যদি(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
    গচ্ছ out;
  }
  অন্যথা যদি(stream->closed)
    result = recv_closed_stream(cf, data, pnread);
  অন্যথা যদি(quiche_conn_is_draining(ctx->qconn)) {
    failf(data, "QUIC connection is draining");
    result = CURLE_HTTP3;
  }
  অন্যথা
    result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));
  যদি(*pnread > 0)
    ctx->data_recvd += *pnread;
  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, stream->id, blen, (পূর্ণাঙ্ক)result, *pnread,
              ctx->data_recvd);
  প্রত্যাগম result;
}

স্থির CURLcode cf_quiche_send_body(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    সংরচনা h3_stream_ctx *stream,
                                    ধ্রুব uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, stream->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  যদি(rv == QUICHE_H3_ERR_DONE || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    যদি(!quiche_conn_stream_writable(ctx->qconn, stream->id, len)) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", stream->id, len);
      stream->quic_flow_blocked = TRUE;
    }
    প্রত্যাগম CURLE_AGAIN;
  }
  অন্যথা যদি(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_STREAM_STATE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", stream->id, len);
    প্রত্যাগম CURLE_HTTP3;
  }
  অন্যথা যদি(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_SIZE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                stream->id, len);
    প্রত্যাগম CURLE_SEND_ERROR;
  }
  অন্যথা যদি(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                stream->id, len, rv);
    প্রত্যাগম CURLE_SEND_ERROR;
  }
  অন্যথা {
    যদি(eos && (len == *pnwritten))
      stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                stream->id, len, stream->send_closed, *pnwritten);
    প্রত্যাগম CURLE_OK;
  }
}

স্থির CURLcode h3_open_stream(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               ধ্রুব uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  size_t nheader, i;
  int64_t rv;
  সংরচনা dynhds h2_headers;
  quiche_h3_header *nva = রিক্ত;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  যদি(!stream) {
    result = h3_data_setup(cf, data);
    যদি(result)
      প্রত্যাগম result;
    stream = H3_STREAM_CTX(ctx, data);
    DEBUGASSERT(stream);
  }

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(stream);

  result = Curl_h1_req_parse_read(&stream->h1, buf, blen, রিক্ত,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : রিক্ত,
    0, pnwritten);
  যদি(result)
    গচ্ছ out;
  যদি(!stream->h1.done) {
    /* need more data */
    গচ্ছ out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  যদি(result)
    গচ্ছ out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_গুনতি(&h2_headers);
  nva = curlx_malloc(পরিমাণ(quiche_h3_header) * nheader);
  যদি(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  প্রতি(i = 0; i < nheader; ++i) {
    সংরচনা dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned বর্ণ *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].মান = (unsigned বর্ণ *)e->মান;
    nva[i].মান_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  যদি(eos && !blen)
    stream->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              stream->send_closed);
  CURL_TRC_CF(data, cf, "quiche_send_request() -> %" PRId64, rv);
  যদি(rv < 0) {
    যদি(QUICHE_H3_ERR_STREAM_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] blocked", stream->id);
      stream->quic_flow_blocked = TRUE;
      result = CURLE_AGAIN;
      গচ্ছ out;
    }
    অন্যথা {
      CURL_TRC_CF(data, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&data->state.url), rv);
    }
    result = CURLE_SEND_ERROR;
    গচ্ছ out;
  }

  DEBUGASSERT(!stream->opened);
  stream->id = (uint64_t)rv;
  stream->opened = TRUE;
  stream->closed = FALSE;
  stream->reset = FALSE;

  যদি(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    প্রতি(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", stream->id,
            (পূর্ণাঙ্ক)nva[i].name_len, nva[i].name,
            (পূর্ণাঙ্ক)nva[i].মান_len, nva[i].মান);
    }
  }

  যদি(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, data, stream, buf, blen, eos, &nwritten);
    যদি(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      result = r2;
    }
    অন্যথা যদি(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  প্রত্যাগম result;
}

স্থির CURLcode cf_quiche_send(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                               ধ্রুব uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result;

  *pnwritten = 0;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  result = cf_process_ingress(cf, data);
  যদি(result)
    গচ্ছ out;

  যদি(!stream || !stream->opened) {
    result = h3_open_stream(cf, data, buf, len, eos, pnwritten);
    যদি(result)
      গচ্ছ out;
    stream = H3_STREAM_CTX(ctx, data);
  }
  অন্যথা যদি(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
  }
  অন্যথা যদি(stream->closed) {
    যদি(stream->resp_hds_complete) {
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
      গচ্ছ out;
    }
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    গচ্ছ out;
  }
  অন্যথা {
    result = cf_quiche_send_body(cf, data, stream, buf, len, eos, pnwritten);
  }

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));

  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : (uint64_t)~0, len,
              (পূর্ণাঙ্ক)result, *pnwritten);
  প্রত্যাগম result;
}

স্থির bool stream_is_writable(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  প্রত্যাগম stream && (quiche_conn_stream_writable(
    ctx->qconn, stream->id, 1) > 0);
}

স্থির CURLcode cf_quiche_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                         সংরচনা Curl_easy *data,
                                         সংরচনা easy_pollset *ps)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  যদি(!ctx->qconn)
    প্রত্যাগম CURLE_OK;

  Curl_pollset_check(data, ps, ctx->q.sockfd, &want_recv, &want_send);
  যদি(want_recv || want_send) {
    সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
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
  প্রত্যাগম result;
}

স্থির CURLcode h3_data_pause(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (শূন্য)cf;
  যদি(!pause) {
    Curl_multi_mark_dirty(data);
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode cf_quiche_cntrl(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                পূর্ণাঙ্ক event, পূর্ণাঙ্ক arg1, শূন্য *arg2)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (শূন্য)arg1;
  (শূন্য)arg2;
  বিকল্প(event) {
  পক্ষ CF_CTRL_DATA_SETUP:
    ভঙ্গ;
  পক্ষ CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    ভঙ্গ;
  পক্ষ CF_CTRL_DATA_DONE:
    h3_data_done(cf, data);
    ভঙ্গ;
  পক্ষ CF_CTRL_DATA_DONE_SEND: {
    সংরচনা h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    যদি(stream && !stream->send_closed) {
      unsigned বর্ণ body[1];
      size_t sent;

      stream->send_closed = TRUE;
      body[0] = 'X';
      result = cf_quiche_send(cf, data, body, 0, TRUE, &sent);
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  stream->id, (পূর্ণাঙ্ক)result, sent);
    }
    ভঙ্গ;
  }
  পক্ষ CF_CTRL_CONN_INFO_UPDATE:
    যদি(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    ভঙ্গ;
  পক্ষ CF_CTRL_REPORT_STATS:
    যদি(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    ভঙ্গ;
  স্বভাব:
    ভঙ্গ;
  }
  প্রত্যাগম result;
}

স্থির CURLcode cf_quiche_ctx_open(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  পূর্ণাঙ্ক rv;
  CURLcode result;
  ধ্রুব সংরচনা Curl_sockaddr_ex *sockaddr;
  স্থির ধ্রুব সংরচনা alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  যদি(result)
    প্রত্যাগম result;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_VERSION);
  যদি(!ctx->cfg) {
    failf(data, "cannot create quiche config");
    প্রত্যাগম CURLE_FAILED_INIT;
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
                               &ALPN_SPEC_H3, রিক্ত, রিক্ত, cf, রিক্ত);
  যদি(result)
    প্রত্যাগম result;

  result = Curl_rand(data, ctx->scid, পরিমাণ(ctx->scid));
  যদি(result)
    প্রত্যাগম result;

  যদি(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, রিক্ত))
    প্রত্যাগম CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = পরিমাণ(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (সংরচনা sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  যদি(rv == -1)
    প্রত্যাগম CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((ধ্রুব uint8_t *)ctx->scid,
                                        পরিমাণ(ctx->scid), রিক্ত, 0,
                                        (সংরচনা sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  যদি(!ctx->qconn) {
    failf(data, "cannot create quiche connection");
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  /* Known to not work on Windows */
#যদি !defined(_WIN32) && defined(HAVE_QUICHE_CONN_SET_QLOG_FD)
  {
    পূর্ণাঙ্ক qfd;
    (শূন্য)Curl_qlogdir(data, ctx->scid, পরিমাণ(ctx->scid), &qfd);
    যদি(qfd != -1)
      quiche_conn_set_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#endif

  result = cf_flush_egress(cf, data);
  যদি(result)
    প্রত্যাগম result;

  {
    unsigned বর্ণ alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    unsigned alpn_len, offset = 0;

    /* Replace each ALPN length prefix by a comma. */
    যাবত্(offset < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[offset];
      alpn_protocols[offset] = ',';
      offset += 1 + alpn_len;
    }

    CURL_TRC_CF(data, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode cf_quiche_verify_peer(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  প্রত্যাগম Curl_vquic_tls_verify_peer(&ctx->tls, cf, data, &ctx->ssl_peer);
}

স্থির CURLcode cf_quiche_connect(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  bool *done)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  যদি(cf->connected) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  /* Connect the UDP filter first */
  যদি(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    যদি(result || !*done)
      প্রত্যাগম result;
  }

  *done = FALSE;
  যদি(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    প্রত্যাগম CURLE_OK;
  }

  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  যদি(!ctx->qconn) {
    result = cf_quiche_ctx_open(cf, data);
    যদি(result)
      গচ্ছ out;
    ctx->started_at = ctx->q.last_op;
    result = cf_flush_egress(cf, data);
    /* we do not expect to be able to recv anything yet */
    গচ্ছ out;
  }

  result = cf_process_ingress(cf, data);
  যদি(result)
    গচ্ছ out;

  result = cf_flush_egress(cf, data);
  যদি(result)
    গচ্ছ out;

  যদি(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.last_op;
    CURL_TRC_CF(data, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    result = cf_quiche_verify_peer(cf, data);
    যদি(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      যদি(!ctx->h3config) {
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ out;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_set_max_field_section_size(ctx->h3config,
                                                  MAX_HTTP_RESP_HEADER_SIZE);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      যদি(!ctx->h3c) {
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ out;
      }
      cf->connected = TRUE;
      *done = TRUE;
    }
  }
  অন্যথা যদি(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    result = CURLE_WEIRD_SERVER_REPLY;
  }

out:
#ifdef CURLVERBOSE
  যদি(result && result != CURLE_AGAIN) {
    সংরচনা ip_quadruple ip;

    যদি(!Curl_cf_socket_peek(cf->next, data, রিক্ত, রিক্ত, &ip))
      infof(data, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    অন্যথা
      infof(data, "connect failed: %s", curl_easy_strerror(result));
  }
#endif
  প্রত্যাগম result;
}

স্থির CURLcode cf_quiche_shutdown(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  যদি(cf->shutdown || !ctx || !ctx->qconn) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  *done = FALSE;
  যদি(!ctx->shutdown_started) {
    পূর্ণাঙ্ক err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, রিক্ত, 0);
    যদি(err) {
      CURL_TRC_CF(data, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
  }

  যদি(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
    result = cf_flush_egress(cf, data);
    যদি(result)
      গচ্ছ out;
  }

  যদি(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
  }
  অন্যথা {
    CURL_TRC_CF(data, cf, "shutdown sending blocked");
  }

out:
  প্রত্যাগম result;
}

স্থির শূন্য cf_quiche_destroy(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  (শূন্য)data;
  যদি(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = রিক্ত;
  }
}

স্থির CURLcode cf_quiche_query(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                পূর্ণাঙ্ক query, পূর্ণাঙ্ক *pres1, শূন্য *pres2)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;

  বিকল্প(query) {
  পক্ষ CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    যদি(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_বাম_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > INT_MAX) ? INT_MAX : (পূর্ণাঙ্ক)max_streams;
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    প্রত্যাগম CURLE_OK;
  }
  পক্ষ CF_QUERY_CONNECT_REPLY_MS:
    যদি(ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (পূর্ণাঙ্ক)ms : INT_MAX;
    }
    অন্যথা
      *pres1 = -1;
    প্রত্যাগম CURLE_OK;
  পক্ষ CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    প্রত্যাগম CURLE_OK;
  পক্ষ CF_QUERY_SSL_INFO:
  পক্ষ CF_QUERY_SSL_CTX_INFO: {
    সংরচনা curl_tlssessioninfo *info = pres2;
    যদি(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      প্রত্যাগম CURLE_OK;
    ভঙ্গ;
  }
  পক্ষ CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুব বর্ণ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : রিক্ত;
    প্রত্যাগম CURLE_OK;
  }
  স্বভাব:
    ভঙ্গ;
  }
  প্রত্যাগম cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির bool cf_quiche_conn_is_alive(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    bool *প্রবেশ_pending)
{
  সংরচনা cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *প্রবেশ_pending = FALSE;
  যদি(!ctx->qconn)
    প্রত্যাগম FALSE;

  যদি(quiche_conn_is_closed(ctx->qconn)) {
    যদি(quiche_conn_is_timed_out(ctx->qconn))
      CURL_TRC_CF(data, cf, "connection was closed due to idle timeout");
    অন্যথা
      CURL_TRC_CF(data, cf, "connection is closed");
    প্রত্যাগম FALSE;
  }

  যদি(!cf->next || !cf->next->cft->is_alive(cf->next, data, প্রবেশ_pending))
    প্রত্যাগম FALSE;

  যদি(*প্রবেশ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *প্রবেশ_pending = FALSE;
    যদি(cf_process_ingress(cf, data))
      alive = FALSE;
    অন্যথা {
      alive = TRUE;
    }
  }

  প্রত্যাগম alive;
}

সংরচনা Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_quiche_create(সংরচনা Curl_cfilter **pcf,
                               সংরচনা Curl_easy *data,
                               সংরচনা Curl_peer *origin,
                               সংরচনা Curl_peer *peer,
                               সংরচনা connectdata *conn,
                               সংরচনা Curl_sockaddr_ex *addr)
{
  সংরচনা cf_quiche_ctx *ctx = রিক্ত;
  সংরচনা Curl_cfilter *cf = রিক্ত;
  CURLcode result;

  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  যদি(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  যদি(result)
    গচ্ছ out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, রিক্ত, TRNSPRT_QUIC);
  যদি(result)
    গচ্ছ out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : রিক্ত;
  যদি(result) {
    যদি(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    অন্যথা যদি(ctx)
      cf_quiche_ctx_free(ctx);
  }

  প্রত্যাগম result;
}

CURLcode Curl_cf_quiche_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                     সংরচনা Curl_peer *origin,
                                     সংরচনা Curl_peer *peer,
                                     সংরচনা Curl_cfilter **pcf)
{
  সংরচনা cf_quiche_ctx *ctx = রিক্ত;
  সংরচনা Curl_cfilter *cf = রিক্ত;
  CURLcode result;

  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  যদি(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  যদি(result)
    গচ্ছ out;
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);

out:
  যদি(result) {
    curlx_safefree(cf);
    যদি(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  প্রত্যাগম result;
}

#endif
