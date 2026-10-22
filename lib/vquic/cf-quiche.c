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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#അന്തര്ഭാവ <quiche.h>
#അന്തര്ഭാവ <openssl/err.h>
#അന്തര്ഭാവ <openssl/ssl.h>

#അന്തര്ഭാവ "bufq.h"
#അന്തര്ഭാവ "u32_ptrset.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "cf-socket.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "http.h"
#അന്തര്ഭാവ "http1.h"
#അന്തര്ഭാവ "sockaddr.h"
#അന്തര്ഭാവ "vdns/cf-dns.h"
#അന്തര്ഭാവ "vquic/vquic.h"
#അന്തര്ഭാവ "vquic/vquic_int.h"
#അന്തര്ഭാവ "vquic/vquic-tls.h"
#അന്തര്ഭാവ "vquic/cf-quiche.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "vtls/openssl.h"
#അന്തര്ഭാവ "vtls/vtls.h"

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
ശൂന്യം Curl_quiche_ver(അക്ഷരം *p, size_t len)
{
  (ശൂന്യം)curl_msnprintf(p, len, "quiche/%s", quiche_version());
}

ഘടന cf_quiche_ctx {
  ഘടന cf_quic_ctx q;
  ഘടന ssl_peer ssl_peer;
  ഘടന curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  ഘടന curltime started_at;        /* time the current attempt started */
  ഘടന curltime handshake_at;      /* time connect handshake finished */
  ഘടന u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  ഘടന dynbuf h1hdr;               /* temp buffer for header construction */
  ഘടന bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t data_recvd;
  BIT(initialized);
  BIT(goaway);                       /* got GOAWAY from server */
  BIT(x509_store_setup);             /* if x509 store has been set up */
  BIT(shutdown_started);             /* queued shutdown packets */
  BIT(stats_reported);               /* connect statistics reported */
};

#ifdef DEBUG_QUICHE
/* initialize debug log callback only once */
സ്ഥിതം പൂർണ്ണസംഖ്യ debug_log_init = 0;
സ്ഥിതം ശൂന്യം quiche_debug_log(സ്ഥിരം അക്ഷരം *വരി, ശൂന്യം *argp)
{
  (ശൂന്യം)argp;
  curl_mfprintf(stderr, "%s\n", വരി);
}
#endif

സ്ഥിതം ശൂന്യം h3_stream_set_free(uint32_t id, ശൂന്യം *stream);

സ്ഥിതം CURLcode cf_quiche_ctx_init(ഘടന cf_quiche_ctx *ctx,
                                   ഘടന Curl_peer *origin,
                                   ഘടന Curl_peer *peer,
                                   ഘടന ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#ifdef DEBUG_QUICHE
  എങ്കിൽ(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, ശൂന്യ);
    debug_log_init = 1;
  }
#endif
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  Curl_bufq_init2(&ctx->writebuf, H3_STREAM_CHUNK_SIZE, H3_STREAM_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  ctx->data_recvd = 0;
  ctx->initialized = TRUE;
  തിരികെ Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

സ്ഥിതം ശൂന്യം cf_quiche_ctx_free(ഘടന cf_quiche_ctx *ctx)
{
  എങ്കിൽ(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = ശൂന്യ;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

സ്ഥിതം ശൂന്യം cf_quiche_ctx_close(ഘടന cf_quiche_ctx *ctx)
{
  എങ്കിൽ(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = ശൂന്യ;
  }
  എങ്കിൽ(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = ശൂന്യ;
  }
  എങ്കിൽ(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = ശൂന്യ;
  }
  എങ്കിൽ(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = ശൂന്യ;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

സ്ഥിതം CURLcode cf_flush_egress(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data);

/**
 * All about the H3 internals of a stream
 */
ഘടന h3_stream_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  ഘടന h1_req_parser h1; /* h1 request parsing */
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

സ്ഥിതം ശൂന്യം h3_stream_ctx_free(ഘടന h3_stream_ctx *stream)
{
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

സ്ഥിതം ശൂന്യം h3_stream_set_free(uint32_t id, ശൂന്യം *stream)
{
  (ശൂന്യം)id;
  DEBUGASSERT(stream);
  h3_stream_ctx_free((ഘടന h3_stream_ctx *)stream);
}

തരനിർവചനം bool cf_quiche_svisit(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *sdata,
                              ഘടന h3_stream_ctx *stream,
                              ശൂന്യം *user_data);

ഘടന cf_quiche_visit_ctx {
  ഘടന Curl_cfilter *cf;
  ഘടന Curl_multi *multi;
  cf_quiche_svisit *cb;
  ശൂന്യം *user_data;
};

സ്ഥിതം bool cf_quiche_stream_do(uint32_t mid, ശൂന്യം *val, ശൂന്യം *user_data)
{
  ഘടന cf_quiche_visit_ctx *vctx = user_data;
  ഘടന h3_stream_ctx *stream = val;
  ഘടന Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  എങ്കിൽ(sdata)
    തിരികെ vctx->cb(vctx->cf, sdata, stream, vctx->user_data);
  തിരികെ TRUE;
}

സ്ഥിതം ശൂന്യം cf_quiche_for_all_streams(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      ശൂന്യം *user_data)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  ഘടന cf_quiche_visit_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_data = user_data;
  Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_stream_do, &vctx);
}

സ്ഥിതം bool cf_quiche_do_resume(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *sdata,
                                ഘടന h3_stream_ctx *stream,
                                ശൂന്യം *user_data)
{
  (ശൂന്യം)user_data;
  എങ്കിൽ(stream->quic_flow_blocked) {
    stream->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", stream->id);
  }
  തിരികെ TRUE;
}

സ്ഥിതം bool cf_quiche_do_expire(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *sdata,
                                ഘടന h3_stream_ctx *stream,
                                ശൂന്യം *user_data)
{
  (ശൂന്യം)user_data;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  stream->xfer_result = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  തിരികെ TRUE;
}

സ്ഥിതം CURLcode h3_data_setup(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  എങ്കിൽ(stream)
    തിരികെ CURLE_OK;

  stream = curlx_calloc(1, വലുപ്പം(*stream));
  എങ്കിൽ(!stream)
    തിരികെ CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_വരി_LEN);

  എങ്കിൽ(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h3_stream_ctx_free(stream);
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cf_quiche_stream_close(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ഘടന h3_stream_ctx *stream)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result;

  എങ്കിൽ(ctx->qconn && !stream->closed) {
    quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    എങ്കിൽ(!stream->send_closed) {
      quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      stream->send_closed = TRUE;
    }
    stream->closed = TRUE;
    result = cf_flush_egress(cf, data);
    എങ്കിൽ(result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  stream->id, (പൂർണ്ണസംഖ്യ)result);
  }
}

സ്ഥിതം ശൂന്യം h3_data_done(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  (ശൂന്യം)cf;
  എങ്കിൽ(stream) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] easy handle is done", stream->id);
    cf_quiche_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
  }
}

സ്ഥിതം ശൂന്യം cf_quiche_expire_conn_closed(ഘടന Curl_cfilter *cf,
                                         ഘടന Curl_easy *data)
{
  DEBUGASSERT(data->multi);
  CURL_TRC_CF(data, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_expire, ശൂന്യ);
}

സ്ഥിതം ശൂന്യം cf_quiche_write_hd(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               ഘടന h3_stream_ctx *stream,
                               സ്ഥിരം അക്ഷരം *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  എങ്കിൽ(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    എങ്കിൽ(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (പൂർണ്ണസംഖ്യ)stream->xfer_result,
                  blen);
  }
}

ഘടന cb_ctx {
  ഘടന Curl_cfilter *cf;
  ഘടന Curl_easy *data;
  ഘടന h3_stream_ctx *stream;
};

സ്ഥിതം bool is_valid_h3_header(സ്ഥിരം uint8_t *hdr, size_t hlen)
{
  ആകുമ്പോൾ(hlen--) {
    തിരഞ്ഞെടുപ്പ്(*hdr++) {
    അവസ്ഥ '\n':
    അവസ്ഥ '\r':
    അവസ്ഥ '\0':
      തിരികെ FALSE;
    }
  }
  തിരികെ TRUE;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cb_each_header(uint8_t *name, size_t name_len,
                          uint8_t *മൂല്യം, size_t മൂല്യം_len,
                          ശൂന്യം *argp)
{
  ഘടന cb_ctx *x = argp;
  ഘടന Curl_cfilter *cf = x->cf;
  ഘടന Curl_easy *data = x->data;
  ഘടന h3_stream_ctx *stream = x->stream;
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!stream || stream->xfer_result)
    തിരികെ 1; /* abort iteration */

  എങ്കിൽ((name_len == 7) && !strncmp(HTTP_PSEUDO_STATUS, (അക്ഷരം *)name, 7) &&
     is_valid_h3_header(മൂല്യം, മൂല്യം_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    result = Curl_http_decode_status(&stream->status_code,
                                     (സ്ഥിരം അക്ഷരം *)മൂല്യം, മൂല്യം_len);
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, (സ്ഥിരം അക്ഷരം *)മൂല്യം, മൂല്യം_len);
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    എങ്കിൽ(!result)
      cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] status: %s",
                stream->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(is_valid_h3_header(മൂല്യം, മൂല്യം_len) &&
       is_valid_h3_header(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  stream->id, (പൂർണ്ണസംഖ്യ)name_len, name,
                  (പൂർണ്ണസംഖ്യ)മൂല്യം_len, മൂല്യം);
      curlx_dyn_reset(&ctx->h1hdr);
      result = curlx_dyn_addn(&ctx->h1hdr, (സ്ഥിരം അക്ഷരം *)name, name_len);
      എങ്കിൽ(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      എങ്കിൽ(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, (സ്ഥിരം അക്ഷരം *)മൂല്യം, മൂല്യം_len);
      എങ്കിൽ(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      എങ്കിൽ(!result)
        cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    അല്ലെങ്കിൽ
      CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  stream->id, മൂല്യം_len + name_len);
  }

  എങ്കിൽ(result) {
    CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] on header error %d",
                stream->id, (പൂർണ്ണസംഖ്യ)result);
    എങ്കിൽ(!stream->xfer_result)
      stream->xfer_result = result;
  }
  തിരികെ stream->xfer_result ? 1 : 0;
}

സ്ഥിതം CURLcode stream_resp_read(ശൂന്യം *reader_ctx,
                                 unsigned അക്ഷരം *buf, size_t len,
                                 size_t *pnread)
{
  ഘടന cb_ctx *x = reader_ctx;
  ഘടന cf_quiche_ctx *ctx = x->cf->ctx;
  ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, x->data);
  ssize_t nread;

  *pnread = 0;
  എങ്കിൽ(!stream)
    തിരികെ CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, stream->id, buf, len);
  എങ്കിൽ(!curlx_sztouz(nread, pnread))
    തിരികെ CURLE_AGAIN;
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cf_quiche_flush_body(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന h3_stream_ctx *stream)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  സ്ഥിരം uint8_t *buf;
  size_t blen;

  ആകുമ്പോൾ(stream && !stream->xfer_result) {
    എങ്കിൽ(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      stream->xfer_result = Curl_xfer_write_resp(
        data, (സ്ഥിരം അക്ഷരം *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      എങ്കിൽ(stream->xfer_result) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", stream->id, (പൂർണ്ണസംഖ്യ)stream->xfer_result, blen);
      }
    }
    അല്ലെങ്കിൽ
      നിർത്തുക;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

സ്ഥിതം ശൂന്യം cf_quiche_recv_body(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                ഘടന h3_stream_ctx *stream)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  ഘടന cb_ctx cb_ctx;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!stream)
    തിരികെ;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.data = data;
  cb_ctx.stream = stream;
  Curl_bufq_reset(&ctx->writebuf);
  ആകുമ്പോൾ(!result) {
    result = Curl_bufq_slurp(&ctx->writebuf,
                             stream_resp_read, &cb_ctx, &nread);
    എങ്കിൽ(!result)
      cf_quiche_flush_body(cf, data, stream);
    അല്ലെങ്കിൽ എങ്കിൽ(result == CURLE_AGAIN)
      നിർത്തുക;
    അല്ലെങ്കിൽ എങ്കിൽ(result) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv_body error %d",
                  stream->id, (പൂർണ്ണസംഖ്യ)result);
      failf(data, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            stream->id, (പൂർണ്ണസംഖ്യ)result);
      stream->closed = TRUE;
      stream->reset = TRUE;
      stream->send_closed = TRUE;
      എങ്കിൽ(!stream->xfer_result)
        stream->xfer_result = result;
    }
  }
  cf_quiche_flush_body(cf, data, stream);
}

സ്ഥിതം ശൂന്യം cf_quiche_process_ev(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന h3_stream_ctx *stream,
                                 quiche_h3_event *ev)
{
  എണ്ണൽ quiche_h3_event_type type;

  എങ്കിൽ(!stream)
    തിരികെ;

  type = quiche_h3_event_type(ev);

  തിരഞ്ഞെടുപ്പ്(type) {
  അവസ്ഥ QUICHE_H3_EVENT_HEADERS: {
    ഘടന cb_ctx cb_ctx;
    stream->resp_got_header = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.data = data;
    cb_ctx.stream = stream;
    quiche_h3_event_for_each_header(ev, cb_each_header, &cb_ctx);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [HEADERS]", stream->id);
    Curl_multi_mark_dirty(data);
    നിർത്തുക;
  }
  അവസ്ഥ QUICHE_H3_EVENT_DATA:
    എങ്കിൽ(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, data, stream);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [DATA]", stream->id);
    Curl_multi_mark_dirty(data);
    നിർത്തുക;

  അവസ്ഥ QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] RESET", stream->id);
    stream->closed = TRUE;
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    Curl_multi_mark_dirty(data);
    നിർത്തുക;

  അവസ്ഥ QUICHE_H3_EVENT_FINISHED:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] CLOSED", stream->id);
    എങ്കിൽ(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, TRUE);
    }
    stream->closed = TRUE;
    Curl_multi_mark_dirty(data);
    നിർത്തുക;

  അവസ്ഥ QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [GOAWAY]", stream->id);
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv, unhandled event %d",
                stream->id, (പൂർണ്ണസംഖ്യ)type);
    നിർത്തുക;
  }
}

ഘടന cf_quiche_disp_ctx {
  uint64_t stream_id;
  ഘടന Curl_cfilter *cf;
  ഘടന Curl_multi *multi;
  quiche_h3_event *ev;
};

സ്ഥിതം bool cf_quiche_disp_event(uint32_t mid, ശൂന്യം *val, ശൂന്യം *user_data)
{
  ഘടന cf_quiche_disp_ctx *dctx = user_data;
  ഘടന h3_stream_ctx *stream = val;

  എങ്കിൽ(stream->id == dctx->stream_id) {
    ഘടന Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    എങ്കിൽ(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, stream, dctx->ev);
    തിരികെ FALSE; /* stop iterating */
  }
  തിരികെ TRUE;
}

സ്ഥിതം CURLcode cf_poll_events(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = ശൂന്യ;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  ആകുമ്പോൾ(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    എങ്കിൽ(rv == QUICHE_H3_ERR_DONE) {
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(rv < 0) {
      CURL_TRC_CF(data, cf, "error poll: %" PRId64, rv);
      തിരികെ CURLE_HTTP3;
    }
    അല്ലെങ്കിൽ {
      stream = H3_STREAM_CTX(ctx, data);
      എങ്കിൽ(stream && stream->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, data, stream, ev);
        quiche_h3_event_free(ev);
        എങ്കിൽ(stream->xfer_result)
          തിരികെ stream->xfer_result;
      }
      അല്ലെങ്കിൽ {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        ഘടന cf_quiche_disp_ctx dctx;
        dctx.stream_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = data->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  തിരികെ CURLE_OK;
}

ഘടന recv_ctx {
  ഘടന Curl_cfilter *cf;
  ഘടന Curl_easy *data;
  പൂർണ്ണസംഖ്യ pkts;
};

സ്ഥിതം CURLcode cf_quiche_recv_pkts(സ്ഥിരം unsigned അക്ഷരം *buf, size_t buflen,
                                    size_t gso_size,
                                    ഘടന sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    ശൂന്യം *userp)
{
  ഘടന recv_ctx *r = userp;
  ഘടന cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, offset, nread;
  ssize_t rv;

  (ശൂന്യം)ecn;

  recv_info.to = (ഘടന sockaddr *)&ctx->q.local_addr;
  recv_info.to_len = ctx->q.local_addrlen;
  recv_info.from = (ഘടന sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  വേണ്ടി(offset = 0; offset < buflen; offset += gso_size) {
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = quiche_conn_recv(ctx->qconn,
                          (unsigned അക്ഷരം *)CURL_UNCONST(buf + offset),
                          pktlen, &recv_info);
    എങ്കിൽ(!curlx_sztouz(rv, &nread)) {
      എങ്കിൽ(QUICHE_ERR_DONE == rv) {
        എങ്കിൽ(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is draining");
          തിരികെ CURLE_RECV_ERROR;
        }
        എങ്കിൽ(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is closed");
          തിരികെ CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->data, r->cf, "ingress, quiche is DONE");
        തിരികെ CURLE_OK;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(QUICHE_ERR_TLS_FAIL == rv) {
        long verify_ok = SSL_get_verify_result(ctx->tls.ossl.ssl);
        എങ്കിൽ(verify_ok != X509_V_OK) {
          failf(r->data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          തിരികെ CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->data, "ingress, quiche reports TLS fail");
        തിരികെ CURLE_RECV_ERROR;
      }
      അല്ലെങ്കിൽ {
        failf(r->data, "quiche reports error %zd on receive", rv);
        തിരികെ CURLE_RECV_ERROR;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(nread < pktlen) {
      CURL_TRC_CF(r->data, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_process_ingress(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  ഘടന recv_ctx rctx;
  CURLcode result;

  DEBUGASSERT(ctx->qconn);
  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  എങ്കിൽ(result)
    തിരികെ result;

  rctx.cf = cf;
  rctx.data = data;
  rctx.pkts = 0;

  result = Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(data->multi);
    cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_resume, ശൂന്യ);
  }
  തിരികെ cf_poll_events(cf, data);
}

ഘടന read_ctx {
  ഘടന Curl_cfilter *cf;
  ഘടന Curl_easy *data;
  quiche_send_info send_info;
};

സ്ഥിതം CURLcode read_pkt_to_send(ശൂന്യം *userp,
                                 unsigned അക്ഷരം *buf, size_t buflen,
                                 size_t *pnread)
{
  ഘടന read_ctx *x = userp;
  ഘടന cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  എങ്കിൽ(rv == QUICHE_ERR_DONE)
    തിരികെ CURLE_AGAIN;

  എങ്കിൽ(!curlx_sztouz(rv, pnread)) {
    failf(x->data, "quiche_conn_send returned %zd", rv);
    തിരികെ CURLE_SEND_ERROR;
  }
  തിരികെ CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
സ്ഥിതം CURLcode cf_flush_egress(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode result;
  int64_t expiry_ns;
  int64_t timeout_ns;
  ഘടന read_ctx readx;
  size_t pkt_എണ്ണം, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  എങ്കിൽ(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    എങ്കിൽ(quiche_conn_is_closed(ctx->qconn)) {
      എങ്കിൽ(quiche_conn_is_timed_out(ctx->qconn))
        failf(data, "connection closed by idle timeout");
      അല്ലെങ്കിൽ
        failf(data, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, data);
      തിരികെ CURLE_SEND_ERROR;
    }
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  എങ്കിൽ(result) {
    എങ്കിൽ(result == CURLE_AGAIN) {
      Curl_expire(data, 1, EXPIRE_QUIC);
      തിരികെ CURLE_OK;
    }
    തിരികെ result;
  }

  readx.cf = cf;
  readx.data = data;
  memset(&readx.send_info, 0, വലുപ്പം(readx.send_info));
  pkt_എണ്ണം = 0;
  gsolen = quiche_conn_max_send_udp_payload_size(ctx->qconn);
  വേണ്ടി(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_to_send, &readx, &nread);
    എങ്കിൽ(result) {
      എങ്കിൽ(result != CURLE_AGAIN)
        തിരികെ result;
      /* Nothing more to add, flush and leave */
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      എങ്കിൽ(result) {
        എങ്കിൽ(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          തിരികെ CURLE_OK;
        }
        തിരികെ result;
      }
      പോകുക out;
    }

    ++pkt_എണ്ണം;
    എങ്കിൽ(nread < gsolen || pkt_എണ്ണം >= MAX_PKT_BURST) {
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      എങ്കിൽ(result) {
        എങ്കിൽ(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          തിരികെ CURLE_OK;
        }
        പോകുക out;
      }
      pkt_എണ്ണം = 0;
    }
  }

out:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  എങ്കിൽ(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(data, (timeout_ns / 1000000), EXPIRE_QUIC);
  തിരികെ result;
}

സ്ഥിതം CURLcode recv_closed_stream(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   size_t *pnread)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(stream);
  *pnread = 0;
  എങ്കിൽ(stream->reset) {
    എങ്കിൽ(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      തിരികെ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    അല്ലെങ്കിൽ എങ്കിൽ(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        തിരികെ CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                stream->id, (പൂർണ്ണസംഖ്യ)result);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!stream->resp_got_header) {
    failf(data, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    result = CURLE_HTTP3;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_quiche_recv(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                               അക്ഷരം *buf, size_t blen, size_t *pnread)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  *pnread = 0;
  (ശൂന്യം)buf;
  (ശൂന്യം)blen;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  എങ്കിൽ(!stream)
    തിരികെ CURLE_RECV_ERROR;

  result = cf_process_ingress(cf, data);
  എങ്കിൽ(result) {
    CURL_TRC_CF(data, cf, "cf_recv, error on ingress");
    പോകുക out;
  }

  എങ്കിൽ(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
    പോകുക out;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(stream->closed)
    result = recv_closed_stream(cf, data, pnread);
  അല്ലെങ്കിൽ എങ്കിൽ(quiche_conn_is_draining(ctx->qconn)) {
    failf(data, "QUIC connection is draining");
    result = CURLE_HTTP3;
  }
  അല്ലെങ്കിൽ
    result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));
  എങ്കിൽ(*pnread > 0)
    ctx->data_recvd += *pnread;
  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, stream->id, blen, (പൂർണ്ണസംഖ്യ)result, *pnread,
              ctx->data_recvd);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_quiche_send_body(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    ഘടന h3_stream_ctx *stream,
                                    സ്ഥിരം uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, stream->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  എങ്കിൽ(rv == QUICHE_H3_ERR_DONE || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    എങ്കിൽ(!quiche_conn_stream_writable(ctx->qconn, stream->id, len)) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", stream->id, len);
      stream->quic_flow_blocked = TRUE;
    }
    തിരികെ CURLE_AGAIN;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_STREAM_STATE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", stream->id, len);
    തിരികെ CURLE_HTTP3;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_SIZE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                stream->id, len);
    തിരികെ CURLE_SEND_ERROR;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                stream->id, len, rv);
    തിരികെ CURLE_SEND_ERROR;
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(eos && (len == *pnwritten))
      stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                stream->id, len, stream->send_closed, *pnwritten);
    തിരികെ CURLE_OK;
  }
}

സ്ഥിതം CURLcode h3_open_stream(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               സ്ഥിരം uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  size_t nheader, i;
  int64_t rv;
  ഘടന dynhds h2_headers;
  quiche_h3_header *nva = ശൂന്യ;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  എങ്കിൽ(!stream) {
    result = h3_data_setup(cf, data);
    എങ്കിൽ(result)
      തിരികെ result;
    stream = H3_STREAM_CTX(ctx, data);
    DEBUGASSERT(stream);
  }

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(stream);

  result = Curl_h1_req_parse_read(&stream->h1, buf, blen, ശൂന്യ,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : ശൂന്യ,
    0, pnwritten);
  എങ്കിൽ(result)
    പോകുക out;
  എങ്കിൽ(!stream->h1.done) {
    /* need more data */
    പോകുക out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  എങ്കിൽ(result)
    പോകുക out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_എണ്ണം(&h2_headers);
  nva = curlx_malloc(വലുപ്പം(quiche_h3_header) * nheader);
  എങ്കിൽ(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  വേണ്ടി(i = 0; i < nheader; ++i) {
    ഘടന dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned അക്ഷരം *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].മൂല്യം = (unsigned അക്ഷരം *)e->മൂല്യം;
    nva[i].മൂല്യം_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  എങ്കിൽ(eos && !blen)
    stream->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              stream->send_closed);
  CURL_TRC_CF(data, cf, "quiche_send_request() -> %" PRId64, rv);
  എങ്കിൽ(rv < 0) {
    എങ്കിൽ(QUICHE_H3_ERR_STREAM_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] blocked", stream->id);
      stream->quic_flow_blocked = TRUE;
      result = CURLE_AGAIN;
      പോകുക out;
    }
    അല്ലെങ്കിൽ {
      CURL_TRC_CF(data, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&data->state.url), rv);
    }
    result = CURLE_SEND_ERROR;
    പോകുക out;
  }

  DEBUGASSERT(!stream->opened);
  stream->id = (uint64_t)rv;
  stream->opened = TRUE;
  stream->closed = FALSE;
  stream->reset = FALSE;

  എങ്കിൽ(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    വേണ്ടി(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", stream->id,
            (പൂർണ്ണസംഖ്യ)nva[i].name_len, nva[i].name,
            (പൂർണ്ണസംഖ്യ)nva[i].മൂല്യം_len, nva[i].മൂല്യം);
    }
  }

  എങ്കിൽ(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, data, stream, buf, blen, eos, &nwritten);
    എങ്കിൽ(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      result = r2;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_quiche_send(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                               സ്ഥിരം uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result;

  *pnwritten = 0;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  result = cf_process_ingress(cf, data);
  എങ്കിൽ(result)
    പോകുക out;

  എങ്കിൽ(!stream || !stream->opened) {
    result = h3_open_stream(cf, data, buf, len, eos, pnwritten);
    എങ്കിൽ(result)
      പോകുക out;
    stream = H3_STREAM_CTX(ctx, data);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(stream->closed) {
    എങ്കിൽ(stream->resp_hds_complete) {
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
      പോകുക out;
    }
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    പോകുക out;
  }
  അല്ലെങ്കിൽ {
    result = cf_quiche_send_body(cf, data, stream, buf, len, eos, pnwritten);
  }

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));

  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : (uint64_t)~0, len,
              (പൂർണ്ണസംഖ്യ)result, *pnwritten);
  തിരികെ result;
}

സ്ഥിതം bool stream_is_writable(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  തിരികെ stream && (quiche_conn_stream_writable(
    ctx->qconn, stream->id, 1) > 0);
}

സ്ഥിതം CURLcode cf_quiche_adjust_pollset(ഘടന Curl_cfilter *cf,
                                         ഘടന Curl_easy *data,
                                         ഘടന easy_pollset *ps)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!ctx->qconn)
    തിരികെ CURLE_OK;

  Curl_pollset_check(data, ps, ctx->q.sockfd, &want_recv, &want_send);
  എങ്കിൽ(want_recv || want_send) {
    ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
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
  തിരികെ result;
}

സ്ഥിതം CURLcode h3_data_pause(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (ശൂന്യം)cf;
  എങ്കിൽ(!pause) {
    Curl_multi_mark_dirty(data);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_quiche_cntrl(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (ശൂന്യം)arg1;
  (ശൂന്യം)arg2;
  തിരഞ്ഞെടുപ്പ്(event) {
  അവസ്ഥ CF_CTRL_DATA_SETUP:
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_DATA_DONE:
    h3_data_done(cf, data);
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_DATA_DONE_SEND: {
    ഘടന h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    എങ്കിൽ(stream && !stream->send_closed) {
      unsigned അക്ഷരം body[1];
      size_t sent;

      stream->send_closed = TRUE;
      body[0] = 'X';
      result = cf_quiche_send(cf, data, body, 0, TRUE, &sent);
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  stream->id, (പൂർണ്ണസംഖ്യ)result, sent);
    }
    നിർത്തുക;
  }
  അവസ്ഥ CF_CTRL_CONN_INFO_UPDATE:
    എങ്കിൽ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_REPORT_STATS:
    എങ്കിൽ(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_quiche_ctx_open(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  പൂർണ്ണസംഖ്യ rv;
  CURLcode result;
  സ്ഥിരം ഘടന Curl_sockaddr_ex *sockaddr;
  സ്ഥിതം സ്ഥിരം ഘടന alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  എങ്കിൽ(result)
    തിരികെ result;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_VERSION);
  എങ്കിൽ(!ctx->cfg) {
    failf(data, "cannot create quiche config");
    തിരികെ CURLE_FAILED_INIT;
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
                               &ALPN_SPEC_H3, ശൂന്യ, ശൂന്യ, cf, ശൂന്യ);
  എങ്കിൽ(result)
    തിരികെ result;

  result = Curl_rand(data, ctx->scid, വലുപ്പം(ctx->scid));
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, ശൂന്യ))
    തിരികെ CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = വലുപ്പം(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (ഘടന sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  എങ്കിൽ(rv == -1)
    തിരികെ CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((സ്ഥിരം uint8_t *)ctx->scid,
                                        വലുപ്പം(ctx->scid), ശൂന്യ, 0,
                                        (ഘടന sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  എങ്കിൽ(!ctx->qconn) {
    failf(data, "cannot create quiche connection");
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  /* Known to not work on Windows */
#എങ്കിൽ !defined(_WIN32) && defined(HAVE_QUICHE_CONN_SET_QLOG_FD)
  {
    പൂർണ്ണസംഖ്യ qfd;
    (ശൂന്യം)Curl_qlogdir(data, ctx->scid, വലുപ്പം(ctx->scid), &qfd);
    എങ്കിൽ(qfd != -1)
      quiche_conn_set_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#endif

  result = cf_flush_egress(cf, data);
  എങ്കിൽ(result)
    തിരികെ result;

  {
    unsigned അക്ഷരം alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    unsigned alpn_len, offset = 0;

    /* Replace each ALPN length prefix by a comma. */
    ആകുമ്പോൾ(offset < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[offset];
      alpn_protocols[offset] = ',';
      offset += 1 + alpn_len;
    }

    CURL_TRC_CF(data, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_quiche_verify_peer(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  തിരികെ Curl_vquic_tls_verify_peer(&ctx->tls, cf, data, &ctx->ssl_peer);
}

സ്ഥിതം CURLcode cf_quiche_connect(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  bool *done)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  /* Connect the UDP filter first */
  എങ്കിൽ(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    എങ്കിൽ(result || !*done)
      തിരികെ result;
  }

  *done = FALSE;
  എങ്കിൽ(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    തിരികെ CURLE_OK;
  }

  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  എങ്കിൽ(!ctx->qconn) {
    result = cf_quiche_ctx_open(cf, data);
    എങ്കിൽ(result)
      പോകുക out;
    ctx->started_at = ctx->q.last_op;
    result = cf_flush_egress(cf, data);
    /* we do not expect to be able to recv anything yet */
    പോകുക out;
  }

  result = cf_process_ingress(cf, data);
  എങ്കിൽ(result)
    പോകുക out;

  result = cf_flush_egress(cf, data);
  എങ്കിൽ(result)
    പോകുക out;

  എങ്കിൽ(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.last_op;
    CURL_TRC_CF(data, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    result = cf_quiche_verify_peer(cf, data);
    എങ്കിൽ(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      എങ്കിൽ(!ctx->h3config) {
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_set_max_field_section_size(ctx->h3config,
                                                  MAX_HTTP_RESP_HEADER_SIZE);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      എങ്കിൽ(!ctx->h3c) {
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
      cf->connected = TRUE;
      *done = TRUE;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    result = CURLE_WEIRD_SERVER_REPLY;
  }

out:
#ifdef CURLVERBOSE
  എങ്കിൽ(result && result != CURLE_AGAIN) {
    ഘടന ip_quadruple ip;

    എങ്കിൽ(!Curl_cf_socket_peek(cf->next, data, ശൂന്യ, ശൂന്യ, &ip))
      infof(data, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    അല്ലെങ്കിൽ
      infof(data, "connect failed: %s", curl_easy_strerror(result));
  }
#endif
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_quiche_shutdown(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data, bool *done)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(cf->shutdown || !ctx || !ctx->qconn) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  *done = FALSE;
  എങ്കിൽ(!ctx->shutdown_started) {
    പൂർണ്ണസംഖ്യ err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, ശൂന്യ, 0);
    എങ്കിൽ(err) {
      CURL_TRC_CF(data, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      result = CURLE_SEND_ERROR;
      പോകുക out;
    }
  }

  എങ്കിൽ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
    result = cf_flush_egress(cf, data);
    എങ്കിൽ(result)
      പോകുക out;
  }

  എങ്കിൽ(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
  }
  അല്ലെങ്കിൽ {
    CURL_TRC_CF(data, cf, "shutdown sending blocked");
  }

out:
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_quiche_destroy(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  (ശൂന്യം)data;
  എങ്കിൽ(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = ശൂന്യ;
  }
}

സ്ഥിതം CURLcode cf_quiche_query(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;

  തിരഞ്ഞെടുപ്പ്(query) {
  അവസ്ഥ CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    എങ്കിൽ(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_ഇടത്_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > INT_MAX) ? INT_MAX : (പൂർണ്ണസംഖ്യ)max_streams;
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    തിരികെ CURLE_OK;
  }
  അവസ്ഥ CF_QUERY_CONNECT_REPLY_MS:
    എങ്കിൽ(ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (പൂർണ്ണസംഖ്യ)ms : INT_MAX;
    }
    അല്ലെങ്കിൽ
      *pres1 = -1;
    തിരികെ CURLE_OK;
  അവസ്ഥ CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    തിരികെ CURLE_OK;
  അവസ്ഥ CF_QUERY_SSL_INFO:
  അവസ്ഥ CF_QUERY_SSL_CTX_INFO: {
    ഘടന curl_tlssessioninfo *info = pres2;
    എങ്കിൽ(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      തിരികെ CURLE_OK;
    നിർത്തുക;
  }
  അവസ്ഥ CF_QUERY_ALPN_NEGOTIATED: {
    സ്ഥിരം അക്ഷരം **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ശൂന്യ;
    തിരികെ CURLE_OK;
  }
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

സ്ഥിതം bool cf_quiche_conn_is_alive(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    bool *നിവേശം_pending)
{
  ഘടന cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *നിവേശം_pending = FALSE;
  എങ്കിൽ(!ctx->qconn)
    തിരികെ FALSE;

  എങ്കിൽ(quiche_conn_is_closed(ctx->qconn)) {
    എങ്കിൽ(quiche_conn_is_timed_out(ctx->qconn))
      CURL_TRC_CF(data, cf, "connection was closed due to idle timeout");
    അല്ലെങ്കിൽ
      CURL_TRC_CF(data, cf, "connection is closed");
    തിരികെ FALSE;
  }

  എങ്കിൽ(!cf->next || !cf->next->cft->is_alive(cf->next, data, നിവേശം_pending))
    തിരികെ FALSE;

  എങ്കിൽ(*നിവേശം_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *നിവേശം_pending = FALSE;
    എങ്കിൽ(cf_process_ingress(cf, data))
      alive = FALSE;
    അല്ലെങ്കിൽ {
      alive = TRUE;
    }
  }

  തിരികെ alive;
}

ഘടന Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_quiche_create(ഘടന Curl_cfilter **pcf,
                               ഘടന Curl_easy *data,
                               ഘടന Curl_peer *origin,
                               ഘടന Curl_peer *peer,
                               ഘടന connectdata *conn,
                               ഘടന Curl_sockaddr_ex *addr)
{
  ഘടന cf_quiche_ctx *ctx = ശൂന്യ;
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  CURLcode result;

  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  എങ്കിൽ(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  എങ്കിൽ(result)
    പോകുക out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ശൂന്യ, TRNSPRT_QUIC);
  എങ്കിൽ(result)
    പോകുക out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : ശൂന്യ;
  എങ്കിൽ(result) {
    എങ്കിൽ(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    അല്ലെങ്കിൽ എങ്കിൽ(ctx)
      cf_quiche_ctx_free(ctx);
  }

  തിരികെ result;
}

CURLcode Curl_cf_quiche_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                     ഘടന Curl_peer *origin,
                                     ഘടന Curl_peer *peer,
                                     ഘടന Curl_cfilter **pcf)
{
  ഘടന cf_quiche_ctx *ctx = ശൂന്യ;
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  CURLcode result;

  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  എങ്കിൽ(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  എങ്കിൽ(result)
    പോകുക out;
  Curl_conn_cf_തിരുകുക_after(cf_at, cf);

out:
  എങ്കിൽ(result) {
    curlx_safefree(cf);
    എങ്കിൽ(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  തിരികെ result;
}

#endif
