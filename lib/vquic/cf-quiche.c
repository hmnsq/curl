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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#அந்தர்பாவ <quiche.h>
#அந்தர்பாவ <openssl/err.h>
#அந்தர்பாவ <openssl/ssl.h>

#அந்தர்பாவ "bufq.h"
#அந்தர்பாவ "u32_ptrset.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "cf-socket.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "http.h"
#அந்தர்பாவ "http1.h"
#அந்தர்பாவ "sockaddr.h"
#அந்தர்பாவ "vdns/cf-dns.h"
#அந்தர்பாவ "vquic/vquic.h"
#அந்தர்பாவ "vquic/vquic_int.h"
#அந்தர்பாவ "vquic/vquic-tls.h"
#அந்தர்பாவ "vquic/cf-quiche.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "vtls/openssl.h"
#அந்தர்பாவ "vtls/vtls.h"

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
வெறுமை Curl_quiche_ver(எழுத்து *p, size_t len)
{
  (வெறுமை)curl_msnprintf(p, len, "quiche/%s", quiche_version());
}

கட்டமைப்பு cf_quiche_ctx {
  கட்டமைப்பு cf_quic_ctx q;
  கட்டமைப்பு ssl_peer ssl_peer;
  கட்டமைப்பு curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  கட்டமைப்பு curltime started_at;        /* time the current attempt started */
  கட்டமைப்பு curltime handshake_at;      /* time connect handshake finished */
  கட்டமைப்பு u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  கட்டமைப்பு dynbuf h1hdr;               /* temp buffer for header construction */
  கட்டமைப்பு bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t data_recvd;
  BIT(initialized);
  BIT(goaway);                       /* got GOAWAY from server */
  BIT(x509_store_setup);             /* if x509 store has been set up */
  BIT(shutdown_started);             /* queued shutdown packets */
  BIT(stats_reported);               /* connect statistics reported */
};

#ifdef DEBUG_QUICHE
/* initialize debug log callback only once */
நிலையான முழு debug_log_init = 0;
நிலையான வெறுமை quiche_debug_log(மாறிலி எழுத்து *வரி, வெறுமை *argp)
{
  (வெறுமை)argp;
  curl_mfprintf(stderr, "%s\n", வரி);
}
#endif

நிலையான வெறுமை h3_stream_set_free(uint32_t id, வெறுமை *stream);

நிலையான CURLcode cf_quiche_ctx_init(கட்டமைப்பு cf_quiche_ctx *ctx,
                                   கட்டமைப்பு Curl_peer *origin,
                                   கட்டமைப்பு Curl_peer *peer,
                                   கட்டமைப்பு ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#ifdef DEBUG_QUICHE
  எனில்(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, வெற்று);
    debug_log_init = 1;
  }
#endif
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  Curl_bufq_init2(&ctx->writebuf, H3_STREAM_CHUNK_SIZE, H3_STREAM_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  ctx->data_recvd = 0;
  ctx->initialized = TRUE;
  திரும்பு Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

நிலையான வெறுமை cf_quiche_ctx_free(கட்டமைப்பு cf_quiche_ctx *ctx)
{
  எனில்(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = வெற்று;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

நிலையான வெறுமை cf_quiche_ctx_close(கட்டமைப்பு cf_quiche_ctx *ctx)
{
  எனில்(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = வெற்று;
  }
  எனில்(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = வெற்று;
  }
  எனில்(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = வெற்று;
  }
  எனில்(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = வெற்று;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

நிலையான CURLcode cf_flush_egress(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data);

/**
 * All about the H3 internals of a stream
 */
கட்டமைப்பு h3_stream_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  கட்டமைப்பு h1_req_parser h1; /* h1 request parsing */
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

நிலையான வெறுமை h3_stream_ctx_free(கட்டமைப்பு h3_stream_ctx *stream)
{
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

நிலையான வெறுமை h3_stream_set_free(uint32_t id, வெறுமை *stream)
{
  (வெறுமை)id;
  DEBUGASSERT(stream);
  h3_stream_ctx_free((கட்டமைப்பு h3_stream_ctx *)stream);
}

வகைவரை bool cf_quiche_svisit(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *sdata,
                              கட்டமைப்பு h3_stream_ctx *stream,
                              வெறுமை *user_data);

கட்டமைப்பு cf_quiche_visit_ctx {
  கட்டமைப்பு Curl_cfilter *cf;
  கட்டமைப்பு Curl_multi *multi;
  cf_quiche_svisit *cb;
  வெறுமை *user_data;
};

நிலையான bool cf_quiche_stream_do(uint32_t mid, வெறுமை *val, வெறுமை *user_data)
{
  கட்டமைப்பு cf_quiche_visit_ctx *vctx = user_data;
  கட்டமைப்பு h3_stream_ctx *stream = val;
  கட்டமைப்பு Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  எனில்(sdata)
    திரும்பு vctx->cb(vctx->cf, sdata, stream, vctx->user_data);
  திரும்பு TRUE;
}

நிலையான வெறுமை cf_quiche_for_all_streams(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      வெறுமை *user_data)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  கட்டமைப்பு cf_quiche_visit_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_data = user_data;
  Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_stream_do, &vctx);
}

நிலையான bool cf_quiche_do_resume(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *sdata,
                                கட்டமைப்பு h3_stream_ctx *stream,
                                வெறுமை *user_data)
{
  (வெறுமை)user_data;
  எனில்(stream->quic_flow_blocked) {
    stream->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", stream->id);
  }
  திரும்பு TRUE;
}

நிலையான bool cf_quiche_do_expire(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *sdata,
                                கட்டமைப்பு h3_stream_ctx *stream,
                                வெறுமை *user_data)
{
  (வெறுமை)user_data;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  stream->xfer_result = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  திரும்பு TRUE;
}

நிலையான CURLcode h3_data_setup(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  எனில்(stream)
    திரும்பு CURLE_OK;

  stream = curlx_calloc(1, அளவு(*stream));
  எனில்(!stream)
    திரும்பு CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_வரி_LEN);

  எனில்(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h3_stream_ctx_free(stream);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cf_quiche_stream_close(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு h3_stream_ctx *stream)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result;

  எனில்(ctx->qconn && !stream->closed) {
    quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    எனில்(!stream->send_closed) {
      quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      stream->send_closed = TRUE;
    }
    stream->closed = TRUE;
    result = cf_flush_egress(cf, data);
    எனில்(result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  stream->id, (முழு)result);
  }
}

நிலையான வெறுமை h3_data_done(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  (வெறுமை)cf;
  எனில்(stream) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] easy handle is done", stream->id);
    cf_quiche_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
  }
}

நிலையான வெறுமை cf_quiche_expire_conn_closed(கட்டமைப்பு Curl_cfilter *cf,
                                         கட்டமைப்பு Curl_easy *data)
{
  DEBUGASSERT(data->multi);
  CURL_TRC_CF(data, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_expire, வெற்று);
}

நிலையான வெறுமை cf_quiche_write_hd(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு h3_stream_ctx *stream,
                               மாறிலி எழுத்து *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  எனில்(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    எனில்(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (முழு)stream->xfer_result,
                  blen);
  }
}

கட்டமைப்பு cb_ctx {
  கட்டமைப்பு Curl_cfilter *cf;
  கட்டமைப்பு Curl_easy *data;
  கட்டமைப்பு h3_stream_ctx *stream;
};

நிலையான bool is_valid_h3_header(மாறிலி uint8_t *hdr, size_t hlen)
{
  போது(hlen--) {
    தேர்வு(*hdr++) {
    நிலை '\n':
    நிலை '\r':
    நிலை '\0':
      திரும்பு FALSE;
    }
  }
  திரும்பு TRUE;
}

நிலையான முழு cb_each_header(uint8_t *name, size_t name_len,
                          uint8_t *மதிப்பு, size_t மதிப்பு_len,
                          வெறுமை *argp)
{
  கட்டமைப்பு cb_ctx *x = argp;
  கட்டமைப்பு Curl_cfilter *cf = x->cf;
  கட்டமைப்பு Curl_easy *data = x->data;
  கட்டமைப்பு h3_stream_ctx *stream = x->stream;
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  எனில்(!stream || stream->xfer_result)
    திரும்பு 1; /* abort iteration */

  எனில்((name_len == 7) && !strncmp(HTTP_PSEUDO_STATUS, (எழுத்து *)name, 7) &&
     is_valid_h3_header(மதிப்பு, மதிப்பு_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    result = Curl_http_decode_status(&stream->status_code,
                                     (மாறிலி எழுத்து *)மதிப்பு, மதிப்பு_len);
    எனில்(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    எனில்(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, (மாறிலி எழுத்து *)மதிப்பு, மதிப்பு_len);
    எனில்(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    எனில்(!result)
      cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] status: %s",
                stream->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  இல்லையெனில் {
    எனில்(is_valid_h3_header(மதிப்பு, மதிப்பு_len) &&
       is_valid_h3_header(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  stream->id, (முழு)name_len, name,
                  (முழு)மதிப்பு_len, மதிப்பு);
      curlx_dyn_reset(&ctx->h1hdr);
      result = curlx_dyn_addn(&ctx->h1hdr, (மாறிலி எழுத்து *)name, name_len);
      எனில்(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      எனில்(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, (மாறிலி எழுத்து *)மதிப்பு, மதிப்பு_len);
      எனில்(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      எனில்(!result)
        cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    இல்லையெனில்
      CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  stream->id, மதிப்பு_len + name_len);
  }

  எனில்(result) {
    CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] on header error %d",
                stream->id, (முழு)result);
    எனில்(!stream->xfer_result)
      stream->xfer_result = result;
  }
  திரும்பு stream->xfer_result ? 1 : 0;
}

நிலையான CURLcode stream_resp_read(வெறுமை *reader_ctx,
                                 unsigned எழுத்து *buf, size_t len,
                                 size_t *pnread)
{
  கட்டமைப்பு cb_ctx *x = reader_ctx;
  கட்டமைப்பு cf_quiche_ctx *ctx = x->cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, x->data);
  ssize_t nread;

  *pnread = 0;
  எனில்(!stream)
    திரும்பு CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, stream->id, buf, len);
  எனில்(!curlx_sztouz(nread, pnread))
    திரும்பு CURLE_AGAIN;
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cf_quiche_flush_body(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு h3_stream_ctx *stream)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  மாறிலி uint8_t *buf;
  size_t blen;

  போது(stream && !stream->xfer_result) {
    எனில்(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      stream->xfer_result = Curl_xfer_write_resp(
        data, (மாறிலி எழுத்து *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      எனில்(stream->xfer_result) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", stream->id, (முழு)stream->xfer_result, blen);
      }
    }
    இல்லையெனில்
      நிறுத்து;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

நிலையான வெறுமை cf_quiche_recv_body(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு h3_stream_ctx *stream)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  கட்டமைப்பு cb_ctx cb_ctx;
  CURLcode result = CURLE_OK;

  எனில்(!stream)
    திரும்பு;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.data = data;
  cb_ctx.stream = stream;
  Curl_bufq_reset(&ctx->writebuf);
  போது(!result) {
    result = Curl_bufq_slurp(&ctx->writebuf,
                             stream_resp_read, &cb_ctx, &nread);
    எனில்(!result)
      cf_quiche_flush_body(cf, data, stream);
    இல்லையெனில் எனில்(result == CURLE_AGAIN)
      நிறுத்து;
    இல்லையெனில் எனில்(result) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv_body error %d",
                  stream->id, (முழு)result);
      failf(data, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            stream->id, (முழு)result);
      stream->closed = TRUE;
      stream->reset = TRUE;
      stream->send_closed = TRUE;
      எனில்(!stream->xfer_result)
        stream->xfer_result = result;
    }
  }
  cf_quiche_flush_body(cf, data, stream);
}

நிலையான வெறுமை cf_quiche_process_ev(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு h3_stream_ctx *stream,
                                 quiche_h3_event *ev)
{
  எண்ணிக்கை quiche_h3_event_type type;

  எனில்(!stream)
    திரும்பு;

  type = quiche_h3_event_type(ev);

  தேர்வு(type) {
  நிலை QUICHE_H3_EVENT_HEADERS: {
    கட்டமைப்பு cb_ctx cb_ctx;
    stream->resp_got_header = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.data = data;
    cb_ctx.stream = stream;
    quiche_h3_event_for_each_header(ev, cb_each_header, &cb_ctx);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [HEADERS]", stream->id);
    Curl_multi_mark_dirty(data);
    நிறுத்து;
  }
  நிலை QUICHE_H3_EVENT_DATA:
    எனில்(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, data, stream);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [DATA]", stream->id);
    Curl_multi_mark_dirty(data);
    நிறுத்து;

  நிலை QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] RESET", stream->id);
    stream->closed = TRUE;
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    Curl_multi_mark_dirty(data);
    நிறுத்து;

  நிலை QUICHE_H3_EVENT_FINISHED:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] CLOSED", stream->id);
    எனில்(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, TRUE);
    }
    stream->closed = TRUE;
    Curl_multi_mark_dirty(data);
    நிறுத்து;

  நிலை QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [GOAWAY]", stream->id);
    நிறுத்து;

  இயல்பு:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv, unhandled event %d",
                stream->id, (முழு)type);
    நிறுத்து;
  }
}

கட்டமைப்பு cf_quiche_disp_ctx {
  uint64_t stream_id;
  கட்டமைப்பு Curl_cfilter *cf;
  கட்டமைப்பு Curl_multi *multi;
  quiche_h3_event *ev;
};

நிலையான bool cf_quiche_disp_event(uint32_t mid, வெறுமை *val, வெறுமை *user_data)
{
  கட்டமைப்பு cf_quiche_disp_ctx *dctx = user_data;
  கட்டமைப்பு h3_stream_ctx *stream = val;

  எனில்(stream->id == dctx->stream_id) {
    கட்டமைப்பு Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    எனில்(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, stream, dctx->ev);
    திரும்பு FALSE; /* stop iterating */
  }
  திரும்பு TRUE;
}

நிலையான CURLcode cf_poll_events(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = வெற்று;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  போது(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    எனில்(rv == QUICHE_H3_ERR_DONE) {
      நிறுத்து;
    }
    இல்லையெனில் எனில்(rv < 0) {
      CURL_TRC_CF(data, cf, "error poll: %" PRId64, rv);
      திரும்பு CURLE_HTTP3;
    }
    இல்லையெனில் {
      stream = H3_STREAM_CTX(ctx, data);
      எனில்(stream && stream->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, data, stream, ev);
        quiche_h3_event_free(ev);
        எனில்(stream->xfer_result)
          திரும்பு stream->xfer_result;
      }
      இல்லையெனில் {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        கட்டமைப்பு cf_quiche_disp_ctx dctx;
        dctx.stream_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = data->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  திரும்பு CURLE_OK;
}

கட்டமைப்பு recv_ctx {
  கட்டமைப்பு Curl_cfilter *cf;
  கட்டமைப்பு Curl_easy *data;
  முழு pkts;
};

நிலையான CURLcode cf_quiche_recv_pkts(மாறிலி unsigned எழுத்து *buf, size_t buflen,
                                    size_t gso_size,
                                    கட்டமைப்பு sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    வெறுமை *userp)
{
  கட்டமைப்பு recv_ctx *r = userp;
  கட்டமைப்பு cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, offset, nread;
  ssize_t rv;

  (வெறுமை)ecn;

  recv_info.to = (கட்டமைப்பு sockaddr *)&ctx->q.local_addr;
  recv_info.to_len = ctx->q.local_addrlen;
  recv_info.from = (கட்டமைப்பு sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  க்கு(offset = 0; offset < buflen; offset += gso_size) {
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = quiche_conn_recv(ctx->qconn,
                          (unsigned எழுத்து *)CURL_UNCONST(buf + offset),
                          pktlen, &recv_info);
    எனில்(!curlx_sztouz(rv, &nread)) {
      எனில்(QUICHE_ERR_DONE == rv) {
        எனில்(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is draining");
          திரும்பு CURLE_RECV_ERROR;
        }
        எனில்(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is closed");
          திரும்பு CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->data, r->cf, "ingress, quiche is DONE");
        திரும்பு CURLE_OK;
      }
      இல்லையெனில் எனில்(QUICHE_ERR_TLS_FAIL == rv) {
        long verify_ok = SSL_get_verify_result(ctx->tls.ossl.ssl);
        எனில்(verify_ok != X509_V_OK) {
          failf(r->data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          திரும்பு CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->data, "ingress, quiche reports TLS fail");
        திரும்பு CURLE_RECV_ERROR;
      }
      இல்லையெனில் {
        failf(r->data, "quiche reports error %zd on receive", rv);
        திரும்பு CURLE_RECV_ERROR;
      }
    }
    இல்லையெனில் எனில்(nread < pktlen) {
      CURL_TRC_CF(r->data, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_process_ingress(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  கட்டமைப்பு recv_ctx rctx;
  CURLcode result;

  DEBUGASSERT(ctx->qconn);
  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  எனில்(result)
    திரும்பு result;

  rctx.cf = cf;
  rctx.data = data;
  rctx.pkts = 0;

  result = Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  எனில்(result)
    திரும்பு result;

  எனில்(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(data->multi);
    cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_resume, வெற்று);
  }
  திரும்பு cf_poll_events(cf, data);
}

கட்டமைப்பு read_ctx {
  கட்டமைப்பு Curl_cfilter *cf;
  கட்டமைப்பு Curl_easy *data;
  quiche_send_info send_info;
};

நிலையான CURLcode read_pkt_to_send(வெறுமை *userp,
                                 unsigned எழுத்து *buf, size_t buflen,
                                 size_t *pnread)
{
  கட்டமைப்பு read_ctx *x = userp;
  கட்டமைப்பு cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  எனில்(rv == QUICHE_ERR_DONE)
    திரும்பு CURLE_AGAIN;

  எனில்(!curlx_sztouz(rv, pnread)) {
    failf(x->data, "quiche_conn_send returned %zd", rv);
    திரும்பு CURLE_SEND_ERROR;
  }
  திரும்பு CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
நிலையான CURLcode cf_flush_egress(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode result;
  int64_t expiry_ns;
  int64_t timeout_ns;
  கட்டமைப்பு read_ctx readx;
  size_t pkt_மொத்தம், gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  எனில்(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    எனில்(quiche_conn_is_closed(ctx->qconn)) {
      எனில்(quiche_conn_is_timed_out(ctx->qconn))
        failf(data, "connection closed by idle timeout");
      இல்லையெனில்
        failf(data, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, data);
      திரும்பு CURLE_SEND_ERROR;
    }
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  எனில்(result) {
    எனில்(result == CURLE_AGAIN) {
      Curl_expire(data, 1, EXPIRE_QUIC);
      திரும்பு CURLE_OK;
    }
    திரும்பு result;
  }

  readx.cf = cf;
  readx.data = data;
  memset(&readx.send_info, 0, அளவு(readx.send_info));
  pkt_மொத்தம் = 0;
  gsolen = quiche_conn_max_send_udp_payload_size(ctx->qconn);
  க்கு(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_to_send, &readx, &nread);
    எனில்(result) {
      எனில்(result != CURLE_AGAIN)
        திரும்பு result;
      /* Nothing more to add, flush and leave */
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      எனில்(result) {
        எனில்(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          திரும்பு CURLE_OK;
        }
        திரும்பு result;
      }
      செல் out;
    }

    ++pkt_மொத்தம்;
    எனில்(nread < gsolen || pkt_மொத்தம் >= MAX_PKT_BURST) {
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      எனில்(result) {
        எனில்(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          திரும்பு CURLE_OK;
        }
        செல் out;
      }
      pkt_மொத்தம் = 0;
    }
  }

out:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  எனில்(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(data, (timeout_ns / 1000000), EXPIRE_QUIC);
  திரும்பு result;
}

நிலையான CURLcode recv_closed_stream(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   size_t *pnread)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(stream);
  *pnread = 0;
  எனில்(stream->reset) {
    எனில்(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      திரும்பு CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    இல்லையெனில் எனில்(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        திரும்பு CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                stream->id, (முழு)result);
  }
  இல்லையெனில் எனில்(!stream->resp_got_header) {
    failf(data, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    result = CURLE_HTTP3;
  }
  திரும்பு result;
}

நிலையான CURLcode cf_quiche_recv(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                               எழுத்து *buf, size_t blen, size_t *pnread)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  *pnread = 0;
  (வெறுமை)buf;
  (வெறுமை)blen;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  எனில்(!stream)
    திரும்பு CURLE_RECV_ERROR;

  result = cf_process_ingress(cf, data);
  எனில்(result) {
    CURL_TRC_CF(data, cf, "cf_recv, error on ingress");
    செல் out;
  }

  எனில்(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
    செல் out;
  }
  இல்லையெனில் எனில்(stream->closed)
    result = recv_closed_stream(cf, data, pnread);
  இல்லையெனில் எனில்(quiche_conn_is_draining(ctx->qconn)) {
    failf(data, "QUIC connection is draining");
    result = CURLE_HTTP3;
  }
  இல்லையெனில்
    result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));
  எனில்(*pnread > 0)
    ctx->data_recvd += *pnread;
  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, stream->id, blen, (முழு)result, *pnread,
              ctx->data_recvd);
  திரும்பு result;
}

நிலையான CURLcode cf_quiche_send_body(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு h3_stream_ctx *stream,
                                    மாறிலி uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, stream->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  எனில்(rv == QUICHE_H3_ERR_DONE || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    எனில்(!quiche_conn_stream_writable(ctx->qconn, stream->id, len)) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", stream->id, len);
      stream->quic_flow_blocked = TRUE;
    }
    திரும்பு CURLE_AGAIN;
  }
  இல்லையெனில் எனில்(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_STREAM_STATE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", stream->id, len);
    திரும்பு CURLE_HTTP3;
  }
  இல்லையெனில் எனில்(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_SIZE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                stream->id, len);
    திரும்பு CURLE_SEND_ERROR;
  }
  இல்லையெனில் எனில்(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                stream->id, len, rv);
    திரும்பு CURLE_SEND_ERROR;
  }
  இல்லையெனில் {
    எனில்(eos && (len == *pnwritten))
      stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                stream->id, len, stream->send_closed, *pnwritten);
    திரும்பு CURLE_OK;
  }
}

நிலையான CURLcode h3_open_stream(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               மாறிலி uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  size_t nheader, i;
  int64_t rv;
  கட்டமைப்பு dynhds h2_headers;
  quiche_h3_header *nva = வெற்று;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  எனில்(!stream) {
    result = h3_data_setup(cf, data);
    எனில்(result)
      திரும்பு result;
    stream = H3_STREAM_CTX(ctx, data);
    DEBUGASSERT(stream);
  }

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(stream);

  result = Curl_h1_req_parse_read(&stream->h1, buf, blen, வெற்று,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : வெற்று,
    0, pnwritten);
  எனில்(result)
    செல் out;
  எனில்(!stream->h1.done) {
    /* need more data */
    செல் out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  எனில்(result)
    செல் out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_மொத்தம்(&h2_headers);
  nva = curlx_malloc(அளவு(quiche_h3_header) * nheader);
  எனில்(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  க்கு(i = 0; i < nheader; ++i) {
    கட்டமைப்பு dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned எழுத்து *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].மதிப்பு = (unsigned எழுத்து *)e->மதிப்பு;
    nva[i].மதிப்பு_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  எனில்(eos && !blen)
    stream->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              stream->send_closed);
  CURL_TRC_CF(data, cf, "quiche_send_request() -> %" PRId64, rv);
  எனில்(rv < 0) {
    எனில்(QUICHE_H3_ERR_STREAM_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] blocked", stream->id);
      stream->quic_flow_blocked = TRUE;
      result = CURLE_AGAIN;
      செல் out;
    }
    இல்லையெனில் {
      CURL_TRC_CF(data, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&data->state.url), rv);
    }
    result = CURLE_SEND_ERROR;
    செல் out;
  }

  DEBUGASSERT(!stream->opened);
  stream->id = (uint64_t)rv;
  stream->opened = TRUE;
  stream->closed = FALSE;
  stream->reset = FALSE;

  எனில்(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    க்கு(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", stream->id,
            (முழு)nva[i].name_len, nva[i].name,
            (முழு)nva[i].மதிப்பு_len, nva[i].மதிப்பு);
    }
  }

  எனில்(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, data, stream, buf, blen, eos, &nwritten);
    எனில்(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      result = r2;
    }
    இல்லையெனில் எனில்(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  திரும்பு result;
}

நிலையான CURLcode cf_quiche_send(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                               மாறிலி uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result;

  *pnwritten = 0;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  result = cf_process_ingress(cf, data);
  எனில்(result)
    செல் out;

  எனில்(!stream || !stream->opened) {
    result = h3_open_stream(cf, data, buf, len, eos, pnwritten);
    எனில்(result)
      செல் out;
    stream = H3_STREAM_CTX(ctx, data);
  }
  இல்லையெனில் எனில்(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
  }
  இல்லையெனில் எனில்(stream->closed) {
    எனில்(stream->resp_hds_complete) {
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
      செல் out;
    }
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    செல் out;
  }
  இல்லையெனில் {
    result = cf_quiche_send_body(cf, data, stream, buf, len, eos, pnwritten);
  }

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));

  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : (uint64_t)~0, len,
              (முழு)result, *pnwritten);
  திரும்பு result;
}

நிலையான bool stream_is_writable(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  திரும்பு stream && (quiche_conn_stream_writable(
    ctx->qconn, stream->id, 1) > 0);
}

நிலையான CURLcode cf_quiche_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                         கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  எனில்(!ctx->qconn)
    திரும்பு CURLE_OK;

  Curl_pollset_check(data, ps, ctx->q.sockfd, &want_recv, &want_send);
  எனில்(want_recv || want_send) {
    கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
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
  திரும்பு result;
}

நிலையான CURLcode h3_data_pause(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (வெறுமை)cf;
  எனில்(!pause) {
    Curl_multi_mark_dirty(data);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_quiche_cntrl(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                முழு event, முழு arg1, வெறுமை *arg2)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (வெறுமை)arg1;
  (வெறுமை)arg2;
  தேர்வு(event) {
  நிலை CF_CTRL_DATA_SETUP:
    நிறுத்து;
  நிலை CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    நிறுத்து;
  நிலை CF_CTRL_DATA_DONE:
    h3_data_done(cf, data);
    நிறுத்து;
  நிலை CF_CTRL_DATA_DONE_SEND: {
    கட்டமைப்பு h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    எனில்(stream && !stream->send_closed) {
      unsigned எழுத்து body[1];
      size_t sent;

      stream->send_closed = TRUE;
      body[0] = 'X';
      result = cf_quiche_send(cf, data, body, 0, TRUE, &sent);
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  stream->id, (முழு)result, sent);
    }
    நிறுத்து;
  }
  நிலை CF_CTRL_CONN_INFO_UPDATE:
    எனில்(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    நிறுத்து;
  நிலை CF_CTRL_REPORT_STATS:
    எனில்(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு result;
}

நிலையான CURLcode cf_quiche_ctx_open(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  முழு rv;
  CURLcode result;
  மாறிலி கட்டமைப்பு Curl_sockaddr_ex *sockaddr;
  நிலையான மாறிலி கட்டமைப்பு alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  எனில்(result)
    திரும்பு result;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_VERSION);
  எனில்(!ctx->cfg) {
    failf(data, "cannot create quiche config");
    திரும்பு CURLE_FAILED_INIT;
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
                               &ALPN_SPEC_H3, வெற்று, வெற்று, cf, வெற்று);
  எனில்(result)
    திரும்பு result;

  result = Curl_rand(data, ctx->scid, அளவு(ctx->scid));
  எனில்(result)
    திரும்பு result;

  எனில்(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, வெற்று))
    திரும்பு CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = அளவு(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (கட்டமைப்பு sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  எனில்(rv == -1)
    திரும்பு CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((மாறிலி uint8_t *)ctx->scid,
                                        அளவு(ctx->scid), வெற்று, 0,
                                        (கட்டமைப்பு sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  எனில்(!ctx->qconn) {
    failf(data, "cannot create quiche connection");
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  /* Known to not work on Windows */
#எனில் !defined(_WIN32) && defined(HAVE_QUICHE_CONN_SET_QLOG_FD)
  {
    முழு qfd;
    (வெறுமை)Curl_qlogdir(data, ctx->scid, அளவு(ctx->scid), &qfd);
    எனில்(qfd != -1)
      quiche_conn_set_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#endif

  result = cf_flush_egress(cf, data);
  எனில்(result)
    திரும்பு result;

  {
    unsigned எழுத்து alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    unsigned alpn_len, offset = 0;

    /* Replace each ALPN length prefix by a comma. */
    போது(offset < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[offset];
      alpn_protocols[offset] = ',';
      offset += 1 + alpn_len;
    }

    CURL_TRC_CF(data, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_quiche_verify_peer(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  திரும்பு Curl_vquic_tls_verify_peer(&ctx->tls, cf, data, &ctx->ssl_peer);
}

நிலையான CURLcode cf_quiche_connect(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  bool *done)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  /* Connect the UDP filter first */
  எனில்(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    எனில்(result || !*done)
      திரும்பு result;
  }

  *done = FALSE;
  எனில்(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    திரும்பு CURLE_OK;
  }

  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  எனில்(!ctx->qconn) {
    result = cf_quiche_ctx_open(cf, data);
    எனில்(result)
      செல் out;
    ctx->started_at = ctx->q.last_op;
    result = cf_flush_egress(cf, data);
    /* we do not expect to be able to recv anything yet */
    செல் out;
  }

  result = cf_process_ingress(cf, data);
  எனில்(result)
    செல் out;

  result = cf_flush_egress(cf, data);
  எனில்(result)
    செல் out;

  எனில்(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.last_op;
    CURL_TRC_CF(data, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    result = cf_quiche_verify_peer(cf, data);
    எனில்(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      எனில்(!ctx->h3config) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_set_max_field_section_size(ctx->h3config,
                                                  MAX_HTTP_RESP_HEADER_SIZE);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      எனில்(!ctx->h3c) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
      cf->connected = TRUE;
      *done = TRUE;
    }
  }
  இல்லையெனில் எனில்(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    result = CURLE_WEIRD_SERVER_REPLY;
  }

out:
#ifdef CURLVERBOSE
  எனில்(result && result != CURLE_AGAIN) {
    கட்டமைப்பு ip_quadruple ip;

    எனில்(!Curl_cf_socket_peek(cf->next, data, வெற்று, வெற்று, &ip))
      infof(data, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    இல்லையெனில்
      infof(data, "connect failed: %s", curl_easy_strerror(result));
  }
#endif
  திரும்பு result;
}

நிலையான CURLcode cf_quiche_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  எனில்(cf->shutdown || !ctx || !ctx->qconn) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  *done = FALSE;
  எனில்(!ctx->shutdown_started) {
    முழு err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, வெற்று, 0);
    எனில்(err) {
      CURL_TRC_CF(data, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      result = CURLE_SEND_ERROR;
      செல் out;
    }
  }

  எனில்(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
    result = cf_flush_egress(cf, data);
    எனில்(result)
      செல் out;
  }

  எனில்(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
  }
  இல்லையெனில் {
    CURL_TRC_CF(data, cf, "shutdown sending blocked");
  }

out:
  திரும்பு result;
}

நிலையான வெறுமை cf_quiche_destroy(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  (வெறுமை)data;
  எனில்(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = வெற்று;
  }
}

நிலையான CURLcode cf_quiche_query(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                முழு query, முழு *pres1, வெறுமை *pres2)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;

  தேர்வு(query) {
  நிலை CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    எனில்(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_இடது_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > INT_MAX) ? INT_MAX : (முழு)max_streams;
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    திரும்பு CURLE_OK;
  }
  நிலை CF_QUERY_CONNECT_REPLY_MS:
    எனில்(ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (முழு)ms : INT_MAX;
    }
    இல்லையெனில்
      *pres1 = -1;
    திரும்பு CURLE_OK;
  நிலை CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    திரும்பு CURLE_OK;
  நிலை CF_QUERY_SSL_INFO:
  நிலை CF_QUERY_SSL_CTX_INFO: {
    கட்டமைப்பு curl_tlssessioninfo *info = pres2;
    எனில்(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      திரும்பு CURLE_OK;
    நிறுத்து;
  }
  நிலை CF_QUERY_ALPN_NEGOTIATED: {
    மாறிலி எழுத்து **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : வெற்று;
    திரும்பு CURLE_OK;
  }
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

நிலையான bool cf_quiche_conn_is_alive(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    bool *உள்ளீடு_pending)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *உள்ளீடு_pending = FALSE;
  எனில்(!ctx->qconn)
    திரும்பு FALSE;

  எனில்(quiche_conn_is_closed(ctx->qconn)) {
    எனில்(quiche_conn_is_timed_out(ctx->qconn))
      CURL_TRC_CF(data, cf, "connection was closed due to idle timeout");
    இல்லையெனில்
      CURL_TRC_CF(data, cf, "connection is closed");
    திரும்பு FALSE;
  }

  எனில்(!cf->next || !cf->next->cft->is_alive(cf->next, data, உள்ளீடு_pending))
    திரும்பு FALSE;

  எனில்(*உள்ளீடு_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *உள்ளீடு_pending = FALSE;
    எனில்(cf_process_ingress(cf, data))
      alive = FALSE;
    இல்லையெனில் {
      alive = TRUE;
    }
  }

  திரும்பு alive;
}

கட்டமைப்பு Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_quiche_create(கட்டமைப்பு Curl_cfilter **pcf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_peer *origin,
                               கட்டமைப்பு Curl_peer *peer,
                               கட்டமைப்பு connectdata *conn,
                               கட்டமைப்பு Curl_sockaddr_ex *addr)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = வெற்று;
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  CURLcode result;

  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  எனில்(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  எனில்(result)
    செல் out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, வெற்று, TRNSPRT_QUIC);
  எனில்(result)
    செல் out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : வெற்று;
  எனில்(result) {
    எனில்(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    இல்லையெனில் எனில்(ctx)
      cf_quiche_ctx_free(ctx);
  }

  திரும்பு result;
}

CURLcode Curl_cf_quiche_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                     கட்டமைப்பு Curl_peer *origin,
                                     கட்டமைப்பு Curl_peer *peer,
                                     கட்டமைப்பு Curl_cfilter **pcf)
{
  கட்டமைப்பு cf_quiche_ctx *ctx = வெற்று;
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  CURLcode result;

  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  எனில்(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  எனில்(result)
    செல் out;
  Curl_conn_cf_செருகு_after(cf_at, cf);

out:
  எனில்(result) {
    curlx_safefree(cf);
    எனில்(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  திரும்பு result;
}

#endif
