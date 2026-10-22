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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#અન્તર્ભાવ <quiche.h>
#અન્તર્ભાવ <openssl/err.h>
#અન્તર્ભાવ <openssl/ssl.h>

#અન્તર્ભાવ "bufq.h"
#અન્તર્ભાવ "u32_ptrset.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "cf-socket.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "http.h"
#અન્તર્ભાવ "http1.h"
#અન્તર્ભાવ "sockaddr.h"
#અન્તર્ભાવ "vdns/cf-dns.h"
#અન્તર્ભાવ "vquic/vquic.h"
#અન્તર્ભાવ "vquic/vquic_int.h"
#અન્તર્ભાવ "vquic/vquic-tls.h"
#અન્તર્ભાવ "vquic/cf-quiche.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "bufref.h"
#અન્તર્ભાવ "vtls/openssl.h"
#અન્તર્ભાવ "vtls/vtls.h"

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
શૂન્ય Curl_quiche_ver(અક્ષર *p, size_t len)
{
  (શૂન્ય)curl_msnprintf(p, len, "quiche/%s", quiche_version());
}

સંરચના cf_quiche_ctx {
  સંરચના cf_quic_ctx q;
  સંરચના ssl_peer ssl_peer;
  સંરચના curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  સંરચના curltime started_at;        /* time the current attempt started */
  સંરચના curltime handshake_at;      /* time connect handshake finished */
  સંરચના u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  સંરચના dynbuf h1hdr;               /* temp buffer for header construction */
  સંરચના bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t data_recvd;
  BIT(initialized);
  BIT(goaway);                       /* got GOAWAY from server */
  BIT(x509_store_setup);             /* if x509 store has been set up */
  BIT(shutdown_started);             /* queued shutdown packets */
  BIT(stats_reported);               /* connect statistics reported */
};

#ifdef DEBUG_QUICHE
/* initialize debug log callback only once */
સ્થિર પૂર્ણાંક debug_log_init = 0;
સ્થિર શૂન્ય quiche_debug_log(અચલ અક્ષર *લીટી, શૂન્ય *argp)
{
  (શૂન્ય)argp;
  curl_mfprintf(stderr, "%s\n", લીટી);
}
#endif

સ્થિર શૂન્ય h3_stream_set_free(uint32_t id, શૂન્ય *stream);

સ્થિર CURLcode cf_quiche_ctx_init(સંરચના cf_quiche_ctx *ctx,
                                   સંરચના Curl_peer *origin,
                                   સંરચના Curl_peer *peer,
                                   સંરચના ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#ifdef DEBUG_QUICHE
  જો(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, ખાલી);
    debug_log_init = 1;
  }
#endif
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  Curl_bufq_init2(&ctx->writebuf, H3_STREAM_CHUNK_SIZE, H3_STREAM_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  ctx->data_recvd = 0;
  ctx->initialized = TRUE;
  પાછા Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

સ્થિર શૂન્ય cf_quiche_ctx_free(સંરચના cf_quiche_ctx *ctx)
{
  જો(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = ખાલી;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

સ્થિર શૂન્ય cf_quiche_ctx_close(સંરચના cf_quiche_ctx *ctx)
{
  જો(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = ખાલી;
  }
  જો(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = ખાલી;
  }
  જો(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = ખાલી;
  }
  જો(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = ખાલી;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

સ્થિર CURLcode cf_flush_egress(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data);

/**
 * All about the H3 internals of a stream
 */
સંરચના h3_stream_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  સંરચના h1_req_parser h1; /* h1 request parsing */
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

સ્થિર શૂન્ય h3_stream_ctx_free(સંરચના h3_stream_ctx *stream)
{
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

સ્થિર શૂન્ય h3_stream_set_free(uint32_t id, શૂન્ય *stream)
{
  (શૂન્ય)id;
  DEBUGASSERT(stream);
  h3_stream_ctx_free((સંરચના h3_stream_ctx *)stream);
}

પ્રકારવ્યાખ્યા bool cf_quiche_svisit(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *sdata,
                              સંરચના h3_stream_ctx *stream,
                              શૂન્ય *user_data);

સંરચના cf_quiche_visit_ctx {
  સંરચના Curl_cfilter *cf;
  સંરચના Curl_multi *multi;
  cf_quiche_svisit *cb;
  શૂન્ય *user_data;
};

સ્થિર bool cf_quiche_stream_do(uint32_t mid, શૂન્ય *val, શૂન્ય *user_data)
{
  સંરચના cf_quiche_visit_ctx *vctx = user_data;
  સંરચના h3_stream_ctx *stream = val;
  સંરચના Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  જો(sdata)
    પાછા vctx->cb(vctx->cf, sdata, stream, vctx->user_data);
  પાછા TRUE;
}

સ્થિર શૂન્ય cf_quiche_for_all_streams(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      શૂન્ય *user_data)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  સંરચના cf_quiche_visit_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_data = user_data;
  Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_stream_do, &vctx);
}

સ્થિર bool cf_quiche_do_resume(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *sdata,
                                સંરચના h3_stream_ctx *stream,
                                શૂન્ય *user_data)
{
  (શૂન્ય)user_data;
  જો(stream->quic_flow_blocked) {
    stream->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", stream->id);
  }
  પાછા TRUE;
}

સ્થિર bool cf_quiche_do_expire(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *sdata,
                                સંરચના h3_stream_ctx *stream,
                                શૂન્ય *user_data)
{
  (શૂન્ય)user_data;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  stream->xfer_result = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  પાછા TRUE;
}

સ્થિર CURLcode h3_data_setup(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  જો(stream)
    પાછા CURLE_OK;

  stream = curlx_calloc(1, કદ(*stream));
  જો(!stream)
    પાછા CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_લીટી_LEN);

  જો(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h3_stream_ctx_free(stream);
    પાછા CURLE_OUT_OF_MEMORY;
  }

  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cf_quiche_stream_close(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   સંરચના h3_stream_ctx *stream)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result;

  જો(ctx->qconn && !stream->closed) {
    quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    જો(!stream->send_closed) {
      quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      stream->send_closed = TRUE;
    }
    stream->closed = TRUE;
    result = cf_flush_egress(cf, data);
    જો(result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  stream->id, (પૂર્ણાંક)result);
  }
}

સ્થિર શૂન્ય h3_data_done(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  (શૂન્ય)cf;
  જો(stream) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] easy handle is done", stream->id);
    cf_quiche_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
  }
}

સ્થિર શૂન્ય cf_quiche_expire_conn_closed(સંરચના Curl_cfilter *cf,
                                         સંરચના Curl_easy *data)
{
  DEBUGASSERT(data->multi);
  CURL_TRC_CF(data, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_expire, ખાલી);
}

સ્થિર શૂન્ય cf_quiche_write_hd(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               સંરચના h3_stream_ctx *stream,
                               અચલ અક્ષર *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  જો(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    જો(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (પૂર્ણાંક)stream->xfer_result,
                  blen);
  }
}

સંરચના cb_ctx {
  સંરચના Curl_cfilter *cf;
  સંરચના Curl_easy *data;
  સંરચના h3_stream_ctx *stream;
};

સ્થિર bool is_valid_h3_header(અચલ uint8_t *hdr, size_t hlen)
{
  જ્યારે(hlen--) {
    પસંદગી(*hdr++) {
    સ્થિતિ '\n':
    સ્થિતિ '\r':
    સ્થિતિ '\0':
      પાછા FALSE;
    }
  }
  પાછા TRUE;
}

સ્થિર પૂર્ણાંક cb_each_header(uint8_t *name, size_t name_len,
                          uint8_t *કિંમત, size_t કિંમત_len,
                          શૂન્ય *argp)
{
  સંરચના cb_ctx *x = argp;
  સંરચના Curl_cfilter *cf = x->cf;
  સંરચના Curl_easy *data = x->data;
  સંરચના h3_stream_ctx *stream = x->stream;
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  જો(!stream || stream->xfer_result)
    પાછા 1; /* abort iteration */

  જો((name_len == 7) && !strncmp(HTTP_PSEUDO_STATUS, (અક્ષર *)name, 7) &&
     is_valid_h3_header(કિંમત, કિંમત_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    result = Curl_http_decode_status(&stream->status_code,
                                     (અચલ અક્ષર *)કિંમત, કિંમત_len);
    જો(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    જો(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, (અચલ અક્ષર *)કિંમત, કિંમત_len);
    જો(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    જો(!result)
      cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] status: %s",
                stream->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  નહીંતર {
    જો(is_valid_h3_header(કિંમત, કિંમત_len) &&
       is_valid_h3_header(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  stream->id, (પૂર્ણાંક)name_len, name,
                  (પૂર્ણાંક)કિંમત_len, કિંમત);
      curlx_dyn_reset(&ctx->h1hdr);
      result = curlx_dyn_addn(&ctx->h1hdr, (અચલ અક્ષર *)name, name_len);
      જો(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      જો(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, (અચલ અક્ષર *)કિંમત, કિંમત_len);
      જો(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      જો(!result)
        cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    નહીંતર
      CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  stream->id, કિંમત_len + name_len);
  }

  જો(result) {
    CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] on header error %d",
                stream->id, (પૂર્ણાંક)result);
    જો(!stream->xfer_result)
      stream->xfer_result = result;
  }
  પાછા stream->xfer_result ? 1 : 0;
}

સ્થિર CURLcode stream_resp_read(શૂન્ય *reader_ctx,
                                 unsigned અક્ષર *buf, size_t len,
                                 size_t *pnread)
{
  સંરચના cb_ctx *x = reader_ctx;
  સંરચના cf_quiche_ctx *ctx = x->cf->ctx;
  સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, x->data);
  ssize_t nread;

  *pnread = 0;
  જો(!stream)
    પાછા CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, stream->id, buf, len);
  જો(!curlx_sztouz(nread, pnread))
    પાછા CURLE_AGAIN;
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cf_quiche_flush_body(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના h3_stream_ctx *stream)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  અચલ uint8_t *buf;
  size_t blen;

  જ્યારે(stream && !stream->xfer_result) {
    જો(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      stream->xfer_result = Curl_xfer_write_resp(
        data, (અચલ અક્ષર *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      જો(stream->xfer_result) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", stream->id, (પૂર્ણાંક)stream->xfer_result, blen);
      }
    }
    નહીંતર
      તોડો;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

સ્થિર શૂન્ય cf_quiche_recv_body(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                સંરચના h3_stream_ctx *stream)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  સંરચના cb_ctx cb_ctx;
  CURLcode result = CURLE_OK;

  જો(!stream)
    પાછા;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.data = data;
  cb_ctx.stream = stream;
  Curl_bufq_reset(&ctx->writebuf);
  જ્યારે(!result) {
    result = Curl_bufq_slurp(&ctx->writebuf,
                             stream_resp_read, &cb_ctx, &nread);
    જો(!result)
      cf_quiche_flush_body(cf, data, stream);
    નહીંતર જો(result == CURLE_AGAIN)
      તોડો;
    નહીંતર જો(result) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv_body error %d",
                  stream->id, (પૂર્ણાંક)result);
      failf(data, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            stream->id, (પૂર્ણાંક)result);
      stream->closed = TRUE;
      stream->reset = TRUE;
      stream->send_closed = TRUE;
      જો(!stream->xfer_result)
        stream->xfer_result = result;
    }
  }
  cf_quiche_flush_body(cf, data, stream);
}

સ્થિર શૂન્ય cf_quiche_process_ev(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના h3_stream_ctx *stream,
                                 quiche_h3_event *ev)
{
  ગણના quiche_h3_event_type type;

  જો(!stream)
    પાછા;

  type = quiche_h3_event_type(ev);

  પસંદગી(type) {
  સ્થિતિ QUICHE_H3_EVENT_HEADERS: {
    સંરચના cb_ctx cb_ctx;
    stream->resp_got_header = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.data = data;
    cb_ctx.stream = stream;
    quiche_h3_event_for_each_header(ev, cb_each_header, &cb_ctx);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [HEADERS]", stream->id);
    Curl_multi_mark_dirty(data);
    તોડો;
  }
  સ્થિતિ QUICHE_H3_EVENT_DATA:
    જો(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, data, stream);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [DATA]", stream->id);
    Curl_multi_mark_dirty(data);
    તોડો;

  સ્થિતિ QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] RESET", stream->id);
    stream->closed = TRUE;
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    Curl_multi_mark_dirty(data);
    તોડો;

  સ્થિતિ QUICHE_H3_EVENT_FINISHED:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] CLOSED", stream->id);
    જો(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, TRUE);
    }
    stream->closed = TRUE;
    Curl_multi_mark_dirty(data);
    તોડો;

  સ્થિતિ QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [GOAWAY]", stream->id);
    તોડો;

  મૂળભૂત:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv, unhandled event %d",
                stream->id, (પૂર્ણાંક)type);
    તોડો;
  }
}

સંરચના cf_quiche_disp_ctx {
  uint64_t stream_id;
  સંરચના Curl_cfilter *cf;
  સંરચના Curl_multi *multi;
  quiche_h3_event *ev;
};

સ્થિર bool cf_quiche_disp_event(uint32_t mid, શૂન્ય *val, શૂન્ય *user_data)
{
  સંરચના cf_quiche_disp_ctx *dctx = user_data;
  સંરચના h3_stream_ctx *stream = val;

  જો(stream->id == dctx->stream_id) {
    સંરચના Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    જો(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, stream, dctx->ev);
    પાછા FALSE; /* stop iterating */
  }
  પાછા TRUE;
}

સ્થિર CURLcode cf_poll_events(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = ખાલી;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  જ્યારે(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    જો(rv == QUICHE_H3_ERR_DONE) {
      તોડો;
    }
    નહીંતર જો(rv < 0) {
      CURL_TRC_CF(data, cf, "error poll: %" PRId64, rv);
      પાછા CURLE_HTTP3;
    }
    નહીંતર {
      stream = H3_STREAM_CTX(ctx, data);
      જો(stream && stream->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, data, stream, ev);
        quiche_h3_event_free(ev);
        જો(stream->xfer_result)
          પાછા stream->xfer_result;
      }
      નહીંતર {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        સંરચના cf_quiche_disp_ctx dctx;
        dctx.stream_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = data->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  પાછા CURLE_OK;
}

સંરચના recv_ctx {
  સંરચના Curl_cfilter *cf;
  સંરચના Curl_easy *data;
  પૂર્ણાંક pkts;
};

સ્થિર CURLcode cf_quiche_recv_pkts(અચલ unsigned અક્ષર *buf, size_t buflen,
                                    size_t gso_size,
                                    સંરચના sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    શૂન્ય *userp)
{
  સંરચના recv_ctx *r = userp;
  સંરચના cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, offset, nread;
  ssize_t rv;

  (શૂન્ય)ecn;

  recv_info.to = (સંરચના sockaddr *)&ctx->q.local_addr;
  recv_info.to_len = ctx->q.local_addrlen;
  recv_info.from = (સંરચના sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  માટે(offset = 0; offset < buflen; offset += gso_size) {
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = quiche_conn_recv(ctx->qconn,
                          (unsigned અક્ષર *)CURL_UNCONST(buf + offset),
                          pktlen, &recv_info);
    જો(!curlx_sztouz(rv, &nread)) {
      જો(QUICHE_ERR_DONE == rv) {
        જો(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is draining");
          પાછા CURLE_RECV_ERROR;
        }
        જો(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is closed");
          પાછા CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->data, r->cf, "ingress, quiche is DONE");
        પાછા CURLE_OK;
      }
      નહીંતર જો(QUICHE_ERR_TLS_FAIL == rv) {
        long verify_ok = SSL_get_verify_result(ctx->tls.ossl.ssl);
        જો(verify_ok != X509_V_OK) {
          failf(r->data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          પાછા CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->data, "ingress, quiche reports TLS fail");
        પાછા CURLE_RECV_ERROR;
      }
      નહીંતર {
        failf(r->data, "quiche reports error %zd on receive", rv);
        પાછા CURLE_RECV_ERROR;
      }
    }
    નહીંતર જો(nread < pktlen) {
      CURL_TRC_CF(r->data, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_process_ingress(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  સંરચના recv_ctx rctx;
  CURLcode result;

  DEBUGASSERT(ctx->qconn);
  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  જો(result)
    પાછા result;

  rctx.cf = cf;
  rctx.data = data;
  rctx.pkts = 0;

  result = Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  જો(result)
    પાછા result;

  જો(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(data->multi);
    cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_resume, ખાલી);
  }
  પાછા cf_poll_events(cf, data);
}

સંરચના read_ctx {
  સંરચના Curl_cfilter *cf;
  સંરચના Curl_easy *data;
  quiche_send_info send_info;
};

સ્થિર CURLcode read_pkt_to_send(શૂન્ય *userp,
                                 unsigned અક્ષર *buf, size_t buflen,
                                 size_t *pnread)
{
  સંરચના read_ctx *x = userp;
  સંરચના cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  જો(rv == QUICHE_ERR_DONE)
    પાછા CURLE_AGAIN;

  જો(!curlx_sztouz(rv, pnread)) {
    failf(x->data, "quiche_conn_send returned %zd", rv);
    પાછા CURLE_SEND_ERROR;
  }
  પાછા CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
સ્થિર CURLcode cf_flush_egress(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode result;
  int64_t expiry_ns;
  int64_t timeout_ns;
  સંરચના read_ctx readx;
  size_t pkt_ગણતરી, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  જો(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    જો(quiche_conn_is_closed(ctx->qconn)) {
      જો(quiche_conn_is_timed_out(ctx->qconn))
        failf(data, "connection closed by idle timeout");
      નહીંતર
        failf(data, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, data);
      પાછા CURLE_SEND_ERROR;
    }
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  જો(result) {
    જો(result == CURLE_AGAIN) {
      Curl_expire(data, 1, EXPIRE_QUIC);
      પાછા CURLE_OK;
    }
    પાછા result;
  }

  readx.cf = cf;
  readx.data = data;
  memset(&readx.send_info, 0, કદ(readx.send_info));
  pkt_ગણતરી = 0;
  gsolen = quiche_conn_max_send_udp_payload_size(ctx->qconn);
  માટે(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_to_send, &readx, &nread);
    જો(result) {
      જો(result != CURLE_AGAIN)
        પાછા result;
      /* Nothing more to add, flush and leave */
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      જો(result) {
        જો(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          પાછા CURLE_OK;
        }
        પાછા result;
      }
      જાઓ out;
    }

    ++pkt_ગણતરી;
    જો(nread < gsolen || pkt_ગણતરી >= MAX_PKT_BURST) {
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      જો(result) {
        જો(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          પાછા CURLE_OK;
        }
        જાઓ out;
      }
      pkt_ગણતરી = 0;
    }
  }

out:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  જો(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(data, (timeout_ns / 1000000), EXPIRE_QUIC);
  પાછા result;
}

સ્થિર CURLcode recv_closed_stream(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   size_t *pnread)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(stream);
  *pnread = 0;
  જો(stream->reset) {
    જો(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      પાછા CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    નહીંતર જો(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        પાછા CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                stream->id, (પૂર્ણાંક)result);
  }
  નહીંતર જો(!stream->resp_got_header) {
    failf(data, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    result = CURLE_HTTP3;
  }
  પાછા result;
}

સ્થિર CURLcode cf_quiche_recv(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                               અક્ષર *buf, size_t blen, size_t *pnread)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  *pnread = 0;
  (શૂન્ય)buf;
  (શૂન્ય)blen;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  જો(!stream)
    પાછા CURLE_RECV_ERROR;

  result = cf_process_ingress(cf, data);
  જો(result) {
    CURL_TRC_CF(data, cf, "cf_recv, error on ingress");
    જાઓ out;
  }

  જો(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
    જાઓ out;
  }
  નહીંતર જો(stream->closed)
    result = recv_closed_stream(cf, data, pnread);
  નહીંતર જો(quiche_conn_is_draining(ctx->qconn)) {
    failf(data, "QUIC connection is draining");
    result = CURLE_HTTP3;
  }
  નહીંતર
    result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));
  જો(*pnread > 0)
    ctx->data_recvd += *pnread;
  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, stream->id, blen, (પૂર્ણાંક)result, *pnread,
              ctx->data_recvd);
  પાછા result;
}

સ્થિર CURLcode cf_quiche_send_body(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    સંરચના h3_stream_ctx *stream,
                                    અચલ uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, stream->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  જો(rv == QUICHE_H3_ERR_DONE || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    જો(!quiche_conn_stream_writable(ctx->qconn, stream->id, len)) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", stream->id, len);
      stream->quic_flow_blocked = TRUE;
    }
    પાછા CURLE_AGAIN;
  }
  નહીંતર જો(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_STREAM_STATE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", stream->id, len);
    પાછા CURLE_HTTP3;
  }
  નહીંતર જો(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_SIZE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                stream->id, len);
    પાછા CURLE_SEND_ERROR;
  }
  નહીંતર જો(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                stream->id, len, rv);
    પાછા CURLE_SEND_ERROR;
  }
  નહીંતર {
    જો(eos && (len == *pnwritten))
      stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                stream->id, len, stream->send_closed, *pnwritten);
    પાછા CURLE_OK;
  }
}

સ્થિર CURLcode h3_open_stream(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               અચલ uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  size_t nheader, i;
  int64_t rv;
  સંરચના dynhds h2_headers;
  quiche_h3_header *nva = ખાલી;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  જો(!stream) {
    result = h3_data_setup(cf, data);
    જો(result)
      પાછા result;
    stream = H3_STREAM_CTX(ctx, data);
    DEBUGASSERT(stream);
  }

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(stream);

  result = Curl_h1_req_parse_read(&stream->h1, buf, blen, ખાલી,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : ખાલી,
    0, pnwritten);
  જો(result)
    જાઓ out;
  જો(!stream->h1.done) {
    /* need more data */
    જાઓ out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  જો(result)
    જાઓ out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_ગણતરી(&h2_headers);
  nva = curlx_malloc(કદ(quiche_h3_header) * nheader);
  જો(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  માટે(i = 0; i < nheader; ++i) {
    સંરચના dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned અક્ષર *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].કિંમત = (unsigned અક્ષર *)e->કિંમત;
    nva[i].કિંમત_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  જો(eos && !blen)
    stream->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              stream->send_closed);
  CURL_TRC_CF(data, cf, "quiche_send_request() -> %" PRId64, rv);
  જો(rv < 0) {
    જો(QUICHE_H3_ERR_STREAM_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] blocked", stream->id);
      stream->quic_flow_blocked = TRUE;
      result = CURLE_AGAIN;
      જાઓ out;
    }
    નહીંતર {
      CURL_TRC_CF(data, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&data->state.url), rv);
    }
    result = CURLE_SEND_ERROR;
    જાઓ out;
  }

  DEBUGASSERT(!stream->opened);
  stream->id = (uint64_t)rv;
  stream->opened = TRUE;
  stream->closed = FALSE;
  stream->reset = FALSE;

  જો(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    માટે(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", stream->id,
            (પૂર્ણાંક)nva[i].name_len, nva[i].name,
            (પૂર્ણાંક)nva[i].કિંમત_len, nva[i].કિંમત);
    }
  }

  જો(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, data, stream, buf, blen, eos, &nwritten);
    જો(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      result = r2;
    }
    નહીંતર જો(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  પાછા result;
}

સ્થિર CURLcode cf_quiche_send(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                               અચલ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result;

  *pnwritten = 0;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  result = cf_process_ingress(cf, data);
  જો(result)
    જાઓ out;

  જો(!stream || !stream->opened) {
    result = h3_open_stream(cf, data, buf, len, eos, pnwritten);
    જો(result)
      જાઓ out;
    stream = H3_STREAM_CTX(ctx, data);
  }
  નહીંતર જો(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
  }
  નહીંતર જો(stream->closed) {
    જો(stream->resp_hds_complete) {
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
      જાઓ out;
    }
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    જાઓ out;
  }
  નહીંતર {
    result = cf_quiche_send_body(cf, data, stream, buf, len, eos, pnwritten);
  }

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));

  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : (uint64_t)~0, len,
              (પૂર્ણાંક)result, *pnwritten);
  પાછા result;
}

સ્થિર bool stream_is_writable(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  પાછા stream && (quiche_conn_stream_writable(
    ctx->qconn, stream->id, 1) > 0);
}

સ્થિર CURLcode cf_quiche_adjust_pollset(સંરચના Curl_cfilter *cf,
                                         સંરચના Curl_easy *data,
                                         સંરચના easy_pollset *ps)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  જો(!ctx->qconn)
    પાછા CURLE_OK;

  Curl_pollset_check(data, ps, ctx->q.sockfd, &want_recv, &want_send);
  જો(want_recv || want_send) {
    સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
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
  પાછા result;
}

સ્થિર CURLcode h3_data_pause(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (શૂન્ય)cf;
  જો(!pause) {
    Curl_multi_mark_dirty(data);
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_quiche_cntrl(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (શૂન્ય)arg1;
  (શૂન્ય)arg2;
  પસંદગી(event) {
  સ્થિતિ CF_CTRL_DATA_SETUP:
    તોડો;
  સ્થિતિ CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    તોડો;
  સ્થિતિ CF_CTRL_DATA_DONE:
    h3_data_done(cf, data);
    તોડો;
  સ્થિતિ CF_CTRL_DATA_DONE_SEND: {
    સંરચના h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    જો(stream && !stream->send_closed) {
      unsigned અક્ષર body[1];
      size_t sent;

      stream->send_closed = TRUE;
      body[0] = 'X';
      result = cf_quiche_send(cf, data, body, 0, TRUE, &sent);
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  stream->id, (પૂર્ણાંક)result, sent);
    }
    તોડો;
  }
  સ્થિતિ CF_CTRL_CONN_INFO_UPDATE:
    જો(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    તોડો;
  સ્થિતિ CF_CTRL_REPORT_STATS:
    જો(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  પાછા result;
}

સ્થિર CURLcode cf_quiche_ctx_open(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  પૂર્ણાંક rv;
  CURLcode result;
  અચલ સંરચના Curl_sockaddr_ex *sockaddr;
  સ્થિર અચલ સંરચના alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  જો(result)
    પાછા result;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_VERSION);
  જો(!ctx->cfg) {
    failf(data, "cannot create quiche config");
    પાછા CURLE_FAILED_INIT;
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
                               &ALPN_SPEC_H3, ખાલી, ખાલી, cf, ખાલી);
  જો(result)
    પાછા result;

  result = Curl_rand(data, ctx->scid, કદ(ctx->scid));
  જો(result)
    પાછા result;

  જો(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, ખાલી))
    પાછા CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = કદ(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (સંરચના sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  જો(rv == -1)
    પાછા CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((અચલ uint8_t *)ctx->scid,
                                        કદ(ctx->scid), ખાલી, 0,
                                        (સંરચના sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  જો(!ctx->qconn) {
    failf(data, "cannot create quiche connection");
    પાછા CURLE_OUT_OF_MEMORY;
  }

  /* Known to not work on Windows */
#જો !defined(_WIN32) && defined(HAVE_QUICHE_CONN_SET_QLOG_FD)
  {
    પૂર્ણાંક qfd;
    (શૂન્ય)Curl_qlogdir(data, ctx->scid, કદ(ctx->scid), &qfd);
    જો(qfd != -1)
      quiche_conn_set_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#endif

  result = cf_flush_egress(cf, data);
  જો(result)
    પાછા result;

  {
    unsigned અક્ષર alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    unsigned alpn_len, offset = 0;

    /* Replace each ALPN length prefix by a comma. */
    જ્યારે(offset < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[offset];
      alpn_protocols[offset] = ',';
      offset += 1 + alpn_len;
    }

    CURL_TRC_CF(data, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_quiche_verify_peer(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  પાછા Curl_vquic_tls_verify_peer(&ctx->tls, cf, data, &ctx->ssl_peer);
}

સ્થિર CURLcode cf_quiche_connect(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  bool *done)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  /* Connect the UDP filter first */
  જો(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    જો(result || !*done)
      પાછા result;
  }

  *done = FALSE;
  જો(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    પાછા CURLE_OK;
  }

  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  જો(!ctx->qconn) {
    result = cf_quiche_ctx_open(cf, data);
    જો(result)
      જાઓ out;
    ctx->started_at = ctx->q.last_op;
    result = cf_flush_egress(cf, data);
    /* we do not expect to be able to recv anything yet */
    જાઓ out;
  }

  result = cf_process_ingress(cf, data);
  જો(result)
    જાઓ out;

  result = cf_flush_egress(cf, data);
  જો(result)
    જાઓ out;

  જો(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.last_op;
    CURL_TRC_CF(data, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    result = cf_quiche_verify_peer(cf, data);
    જો(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      જો(!ctx->h3config) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_set_max_field_section_size(ctx->h3config,
                                                  MAX_HTTP_RESP_HEADER_SIZE);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      જો(!ctx->h3c) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
      cf->connected = TRUE;
      *done = TRUE;
    }
  }
  નહીંતર જો(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    result = CURLE_WEIRD_SERVER_REPLY;
  }

out:
#ifdef CURLVERBOSE
  જો(result && result != CURLE_AGAIN) {
    સંરચના ip_quadruple ip;

    જો(!Curl_cf_socket_peek(cf->next, data, ખાલી, ખાલી, &ip))
      infof(data, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    નહીંતર
      infof(data, "connect failed: %s", curl_easy_strerror(result));
  }
#endif
  પાછા result;
}

સ્થિર CURLcode cf_quiche_shutdown(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data, bool *done)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  જો(cf->shutdown || !ctx || !ctx->qconn) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  *done = FALSE;
  જો(!ctx->shutdown_started) {
    પૂર્ણાંક err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, ખાલી, 0);
    જો(err) {
      CURL_TRC_CF(data, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
  }

  જો(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
    result = cf_flush_egress(cf, data);
    જો(result)
      જાઓ out;
  }

  જો(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
  }
  નહીંતર {
    CURL_TRC_CF(data, cf, "shutdown sending blocked");
  }

out:
  પાછા result;
}

સ્થિર શૂન્ય cf_quiche_destroy(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  (શૂન્ય)data;
  જો(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = ખાલી;
  }
}

સ્થિર CURLcode cf_quiche_query(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;

  પસંદગી(query) {
  સ્થિતિ CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    જો(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_ડાબું_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > INT_MAX) ? INT_MAX : (પૂર્ણાંક)max_streams;
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    પાછા CURLE_OK;
  }
  સ્થિતિ CF_QUERY_CONNECT_REPLY_MS:
    જો(ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (પૂર્ણાંક)ms : INT_MAX;
    }
    નહીંતર
      *pres1 = -1;
    પાછા CURLE_OK;
  સ્થિતિ CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    પાછા CURLE_OK;
  સ્થિતિ CF_QUERY_SSL_INFO:
  સ્થિતિ CF_QUERY_SSL_CTX_INFO: {
    સંરચના curl_tlssessioninfo *info = pres2;
    જો(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      પાછા CURLE_OK;
    તોડો;
  }
  સ્થિતિ CF_QUERY_ALPN_NEGOTIATED: {
    અચલ અક્ષર **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ખાલી;
    પાછા CURLE_OK;
  }
  મૂળભૂત:
    તોડો;
  }
  પાછા cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

સ્થિર bool cf_quiche_conn_is_alive(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    bool *આગત_pending)
{
  સંરચના cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *આગત_pending = FALSE;
  જો(!ctx->qconn)
    પાછા FALSE;

  જો(quiche_conn_is_closed(ctx->qconn)) {
    જો(quiche_conn_is_timed_out(ctx->qconn))
      CURL_TRC_CF(data, cf, "connection was closed due to idle timeout");
    નહીંતર
      CURL_TRC_CF(data, cf, "connection is closed");
    પાછા FALSE;
  }

  જો(!cf->next || !cf->next->cft->is_alive(cf->next, data, આગત_pending))
    પાછા FALSE;

  જો(*આગત_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *આગત_pending = FALSE;
    જો(cf_process_ingress(cf, data))
      alive = FALSE;
    નહીંતર {
      alive = TRUE;
    }
  }

  પાછા alive;
}

સંરચના Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_quiche_create(સંરચના Curl_cfilter **pcf,
                               સંરચના Curl_easy *data,
                               સંરચના Curl_peer *origin,
                               સંરચના Curl_peer *peer,
                               સંરચના connectdata *conn,
                               સંરચના Curl_sockaddr_ex *addr)
{
  સંરચના cf_quiche_ctx *ctx = ખાલી;
  સંરચના Curl_cfilter *cf = ખાલી;
  CURLcode result;

  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  જો(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  જો(result)
    જાઓ out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ખાલી, TRNSPRT_QUIC);
  જો(result)
    જાઓ out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : ખાલી;
  જો(result) {
    જો(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    નહીંતર જો(ctx)
      cf_quiche_ctx_free(ctx);
  }

  પાછા result;
}

CURLcode Curl_cf_quiche_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                     સંરચના Curl_peer *origin,
                                     સંરચના Curl_peer *peer,
                                     સંરચના Curl_cfilter **pcf)
{
  સંરચના cf_quiche_ctx *ctx = ખાલી;
  સંરચના Curl_cfilter *cf = ખાલી;
  CURLcode result;

  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  જો(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  જો(result)
    જાઓ out;
  Curl_conn_cf_ઉમેરો_after(cf_at, cf);

out:
  જો(result) {
    curlx_safefree(cf);
    જો(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  પાછા result;
}

#endif
