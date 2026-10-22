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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#ଅନ୍ତର୍ଭାଵ <quiche.h>
#ଅନ୍ତର୍ଭାଵ <openssl/err.h>
#ଅନ୍ତର୍ଭାଵ <openssl/ssl.h>

#ଅନ୍ତର୍ଭାଵ "bufq.h"
#ଅନ୍ତର୍ଭାଵ "u32_ptrset.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "cf-socket.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "http.h"
#ଅନ୍ତର୍ଭାଵ "http1.h"
#ଅନ୍ତର୍ଭାଵ "sockaddr.h"
#ଅନ୍ତର୍ଭାଵ "vdns/cf-dns.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic_int.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic-tls.h"
#ଅନ୍ତର୍ଭାଵ "vquic/cf-quiche.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "vtls/openssl.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"

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
ଶୂନ୍ୟ Curl_quiche_ver(ଅକ୍ଷର *p, size_t len)
{
  (ଶୂନ୍ୟ)curl_msnprintf(p, len, "quiche/%s", quiche_version());
}

ସଂରଚନା cf_quiche_ctx {
  ସଂରଚନା cf_quic_ctx q;
  ସଂରଚନା ssl_peer ssl_peer;
  ସଂରଚନା curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  ସଂରଚନା curltime started_at;        /* time the current attempt started */
  ସଂରଚନା curltime handshake_at;      /* time connect handshake finished */
  ସଂରଚନା u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  ସଂରଚନା dynbuf h1hdr;               /* temp buffer for header construction */
  ସଂରଚନା bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t data_recvd;
  BIT(initialized);
  BIT(goaway);                       /* got GOAWAY from server */
  BIT(x509_store_setup);             /* if x509 store has been set up */
  BIT(shutdown_started);             /* queued shutdown packets */
  BIT(stats_reported);               /* connect statistics reported */
};

#ifdef DEBUG_QUICHE
/* initialize debug log callback only once */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା debug_log_init = 0;
ସ୍ଥିର ଶୂନ୍ୟ quiche_debug_log(ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି, ଶୂନ୍ୟ *argp)
{
  (ଶୂନ୍ୟ)argp;
  curl_mfprintf(stderr, "%s\n", ଧାଡ଼ି);
}
#endif

ସ୍ଥିର ଶୂନ୍ୟ h3_stream_set_free(uint32_t id, ଶୂନ୍ୟ *stream);

ସ୍ଥିର CURLcode cf_quiche_ctx_init(ସଂରଚନା cf_quiche_ctx *ctx,
                                   ସଂରଚନା Curl_peer *origin,
                                   ସଂରଚନା Curl_peer *peer,
                                   ସଂରଚନା ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#ifdef DEBUG_QUICHE
  ଯଦି(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, ଖାଲି);
    debug_log_init = 1;
  }
#endif
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  Curl_bufq_init2(&ctx->writebuf, H3_STREAM_CHUNK_SIZE, H3_STREAM_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  ctx->data_recvd = 0;
  ctx->initialized = TRUE;
  ଫେରନ୍ତୁ Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

ସ୍ଥିର ଶୂନ୍ୟ cf_quiche_ctx_free(ସଂରଚନା cf_quiche_ctx *ctx)
{
  ଯଦି(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = ଖାଲି;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

ସ୍ଥିର ଶୂନ୍ୟ cf_quiche_ctx_close(ସଂରଚନା cf_quiche_ctx *ctx)
{
  ଯଦି(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = ଖାଲି;
  }
  ଯଦି(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = ଖାଲି;
  }
  ଯଦି(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = ଖାଲି;
  }
  ଯଦି(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = ଖାଲି;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

ସ୍ଥିର CURLcode cf_flush_egress(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data);

/**
 * All about the H3 internals of a stream
 */
ସଂରଚନା h3_stream_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  ସଂରଚନା h1_req_parser h1; /* h1 request parsing */
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

ସ୍ଥିର ଶୂନ୍ୟ h3_stream_ctx_free(ସଂରଚନା h3_stream_ctx *stream)
{
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

ସ୍ଥିର ଶୂନ୍ୟ h3_stream_set_free(uint32_t id, ଶୂନ୍ୟ *stream)
{
  (ଶୂନ୍ୟ)id;
  DEBUGASSERT(stream);
  h3_stream_ctx_free((ସଂରଚନା h3_stream_ctx *)stream);
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା bool cf_quiche_svisit(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *sdata,
                              ସଂରଚନା h3_stream_ctx *stream,
                              ଶୂନ୍ୟ *user_data);

ସଂରଚନା cf_quiche_visit_ctx {
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା Curl_multi *multi;
  cf_quiche_svisit *cb;
  ଶୂନ୍ୟ *user_data;
};

ସ୍ଥିର bool cf_quiche_stream_do(uint32_t mid, ଶୂନ୍ୟ *val, ଶୂନ୍ୟ *user_data)
{
  ସଂରଚନା cf_quiche_visit_ctx *vctx = user_data;
  ସଂରଚନା h3_stream_ctx *stream = val;
  ସଂରଚନା Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  ଯଦି(sdata)
    ଫେରନ୍ତୁ vctx->cb(vctx->cf, sdata, stream, vctx->user_data);
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_quiche_for_all_streams(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      ଶୂନ୍ୟ *user_data)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ସଂରଚନା cf_quiche_visit_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_data = user_data;
  Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_stream_do, &vctx);
}

ସ୍ଥିର bool cf_quiche_do_resume(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *sdata,
                                ସଂରଚନା h3_stream_ctx *stream,
                                ଶୂନ୍ୟ *user_data)
{
  (ଶୂନ୍ୟ)user_data;
  ଯଦି(stream->quic_flow_blocked) {
    stream->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", stream->id);
  }
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର bool cf_quiche_do_expire(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *sdata,
                                ସଂରଚନା h3_stream_ctx *stream,
                                ଶୂନ୍ୟ *user_data)
{
  (ଶୂନ୍ୟ)user_data;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  stream->xfer_result = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର CURLcode h3_data_setup(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ସଂରଚନା h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  ଯଦି(stream)
    ଫେରନ୍ତୁ CURLE_OK;

  stream = curlx_calloc(1, ଆକାର(*stream));
  ଯଦି(!stream)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_ଧାଡ଼ି_LEN);

  ଯଦି(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h3_stream_ctx_free(stream);
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_quiche_stream_close(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା h3_stream_ctx *stream)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result;

  ଯଦି(ctx->qconn && !stream->closed) {
    quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    ଯଦି(!stream->send_closed) {
      quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      stream->send_closed = TRUE;
    }
    stream->closed = TRUE;
    result = cf_flush_egress(cf, data);
    ଯଦି(result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ h3_data_done(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ସଂରଚନା h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  (ଶୂନ୍ୟ)cf;
  ଯଦି(stream) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] easy handle is done", stream->id);
    cf_quiche_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ cf_quiche_expire_conn_closed(ସଂରଚନା Curl_cfilter *cf,
                                         ସଂରଚନା Curl_easy *data)
{
  DEBUGASSERT(data->multi);
  CURL_TRC_CF(data, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_expire, ଖାଲି);
}

ସ୍ଥିର ଶୂନ୍ୟ cf_quiche_write_hd(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା h3_stream_ctx *stream,
                               ଅଚଳ ଅକ୍ଷର *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ଯଦି(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    ଯଦି(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)stream->xfer_result,
                  blen);
  }
}

ସଂରଚନା cb_ctx {
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା Curl_easy *data;
  ସଂରଚନା h3_stream_ctx *stream;
};

ସ୍ଥିର bool is_valid_h3_header(ଅଚଳ uint8_t *hdr, size_t hlen)
{
  ଯେତେବେଳେ(hlen--) {
    ବାଛ(*hdr++) {
    ସ୍ଥିତି '\n':
    ସ୍ଥିତି '\r':
    ସ୍ଥିତି '\0':
      ଫେରନ୍ତୁ FALSE;
    }
  }
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cb_each_header(uint8_t *name, size_t name_len,
                          uint8_t *ମୂଲ୍ୟ, size_t ମୂଲ୍ୟ_len,
                          ଶୂନ୍ୟ *argp)
{
  ସଂରଚନା cb_ctx *x = argp;
  ସଂରଚନା Curl_cfilter *cf = x->cf;
  ସଂରଚନା Curl_easy *data = x->data;
  ସଂରଚନା h3_stream_ctx *stream = x->stream;
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ଯଦି(!stream || stream->xfer_result)
    ଫେରନ୍ତୁ 1; /* abort iteration */

  ଯଦି((name_len == 7) && !strncmp(HTTP_PSEUDO_STATUS, (ଅକ୍ଷର *)name, 7) &&
     is_valid_h3_header(ମୂଲ୍ୟ, ମୂଲ୍ୟ_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    result = Curl_http_decode_status(&stream->status_code,
                                     (ଅଚଳ ଅକ୍ଷର *)ମୂଲ୍ୟ, ମୂଲ୍ୟ_len);
    ଯଦି(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    ଯଦି(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, (ଅଚଳ ଅକ୍ଷର *)ମୂଲ୍ୟ, ମୂଲ୍ୟ_len);
    ଯଦି(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    ଯଦି(!result)
      cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] status: %s",
                stream->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  ଅନ୍ୟଥା {
    ଯଦି(is_valid_h3_header(ମୂଲ୍ୟ, ମୂଲ୍ୟ_len) &&
       is_valid_h3_header(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)name_len, name,
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ମୂଲ୍ୟ_len, ମୂଲ୍ୟ);
      curlx_dyn_reset(&ctx->h1hdr);
      result = curlx_dyn_addn(&ctx->h1hdr, (ଅଚଳ ଅକ୍ଷର *)name, name_len);
      ଯଦି(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      ଯଦି(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, (ଅଚଳ ଅକ୍ଷର *)ମୂଲ୍ୟ, ମୂଲ୍ୟ_len);
      ଯଦି(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      ଯଦି(!result)
        cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    ଅନ୍ୟଥା
      CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  stream->id, ମୂଲ୍ୟ_len + name_len);
  }

  ଯଦି(result) {
    CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] on header error %d",
                stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଯଦି(!stream->xfer_result)
      stream->xfer_result = result;
  }
  ଫେରନ୍ତୁ stream->xfer_result ? 1 : 0;
}

ସ୍ଥିର CURLcode stream_resp_read(ଶୂନ୍ୟ *reader_ctx,
                                 unsigned ଅକ୍ଷର *buf, size_t len,
                                 size_t *pnread)
{
  ସଂରଚନା cb_ctx *x = reader_ctx;
  ସଂରଚନା cf_quiche_ctx *ctx = x->cf->ctx;
  ସଂରଚନା h3_stream_ctx *stream = H3_STREAM_CTX(ctx, x->data);
  ssize_t nread;

  *pnread = 0;
  ଯଦି(!stream)
    ଫେରନ୍ତୁ CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, stream->id, buf, len);
  ଯଦି(!curlx_sztouz(nread, pnread))
    ଫେରନ୍ତୁ CURLE_AGAIN;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_quiche_flush_body(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା h3_stream_ctx *stream)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ଅଚଳ uint8_t *buf;
  size_t blen;

  ଯେତେବେଳେ(stream && !stream->xfer_result) {
    ଯଦି(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      stream->xfer_result = Curl_xfer_write_resp(
        data, (ଅଚଳ ଅକ୍ଷର *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      ଯଦି(stream->xfer_result) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)stream->xfer_result, blen);
      }
    }
    ଅନ୍ୟଥା
      ଭାଙ୍ଗ;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

ସ୍ଥିର ଶୂନ୍ୟ cf_quiche_recv_body(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା h3_stream_ctx *stream)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  ସଂରଚନା cb_ctx cb_ctx;
  CURLcode result = CURLE_OK;

  ଯଦି(!stream)
    ଫେରନ୍ତୁ;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.data = data;
  cb_ctx.stream = stream;
  Curl_bufq_reset(&ctx->writebuf);
  ଯେତେବେଳେ(!result) {
    result = Curl_bufq_slurp(&ctx->writebuf,
                             stream_resp_read, &cb_ctx, &nread);
    ଯଦି(!result)
      cf_quiche_flush_body(cf, data, stream);
    ଅନ୍ୟଥା ଯଦି(result == CURLE_AGAIN)
      ଭାଙ୍ଗ;
    ଅନ୍ୟଥା ଯଦି(result) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv_body error %d",
                  stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      failf(data, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      stream->closed = TRUE;
      stream->reset = TRUE;
      stream->send_closed = TRUE;
      ଯଦି(!stream->xfer_result)
        stream->xfer_result = result;
    }
  }
  cf_quiche_flush_body(cf, data, stream);
}

ସ୍ଥିର ଶୂନ୍ୟ cf_quiche_process_ev(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା h3_stream_ctx *stream,
                                 quiche_h3_event *ev)
{
  ଗଣନା quiche_h3_event_type type;

  ଯଦି(!stream)
    ଫେରନ୍ତୁ;

  type = quiche_h3_event_type(ev);

  ବାଛ(type) {
  ସ୍ଥିତି QUICHE_H3_EVENT_HEADERS: {
    ସଂରଚନା cb_ctx cb_ctx;
    stream->resp_got_header = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.data = data;
    cb_ctx.stream = stream;
    quiche_h3_event_for_each_header(ev, cb_each_header, &cb_ctx);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [HEADERS]", stream->id);
    Curl_multi_mark_dirty(data);
    ଭାଙ୍ଗ;
  }
  ସ୍ଥିତି QUICHE_H3_EVENT_DATA:
    ଯଦି(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, data, stream);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [DATA]", stream->id);
    Curl_multi_mark_dirty(data);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] RESET", stream->id);
    stream->closed = TRUE;
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    Curl_multi_mark_dirty(data);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି QUICHE_H3_EVENT_FINISHED:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] CLOSED", stream->id);
    ଯଦି(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, TRUE);
    }
    stream->closed = TRUE;
    Curl_multi_mark_dirty(data);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [GOAWAY]", stream->id);
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv, unhandled event %d",
                stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)type);
    ଭାଙ୍ଗ;
  }
}

ସଂରଚନା cf_quiche_disp_ctx {
  uint64_t stream_id;
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା Curl_multi *multi;
  quiche_h3_event *ev;
};

ସ୍ଥିର bool cf_quiche_disp_event(uint32_t mid, ଶୂନ୍ୟ *val, ଶୂନ୍ୟ *user_data)
{
  ସଂରଚନା cf_quiche_disp_ctx *dctx = user_data;
  ସଂରଚନା h3_stream_ctx *stream = val;

  ଯଦି(stream->id == dctx->stream_id) {
    ସଂରଚନା Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    ଯଦି(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, stream, dctx->ev);
    ଫେରନ୍ତୁ FALSE; /* stop iterating */
  }
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର CURLcode cf_poll_events(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ସଂରଚନା h3_stream_ctx *stream = ଖାଲି;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  ଯେତେବେଳେ(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    ଯଦି(rv == QUICHE_H3_ERR_DONE) {
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା ଯଦି(rv < 0) {
      CURL_TRC_CF(data, cf, "error poll: %" PRId64, rv);
      ଫେରନ୍ତୁ CURLE_HTTP3;
    }
    ଅନ୍ୟଥା {
      stream = H3_STREAM_CTX(ctx, data);
      ଯଦି(stream && stream->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, data, stream, ev);
        quiche_h3_event_free(ev);
        ଯଦି(stream->xfer_result)
          ଫେରନ୍ତୁ stream->xfer_result;
      }
      ଅନ୍ୟଥା {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        ସଂରଚନା cf_quiche_disp_ctx dctx;
        dctx.stream_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = data->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସଂରଚନା recv_ctx {
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା Curl_easy *data;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pkts;
};

ସ୍ଥିର CURLcode cf_quiche_recv_pkts(ଅଚଳ unsigned ଅକ୍ଷର *buf, size_t buflen,
                                    size_t gso_size,
                                    ସଂରଚନା sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା recv_ctx *r = userp;
  ସଂରଚନା cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, offset, nread;
  ssize_t rv;

  (ଶୂନ୍ୟ)ecn;

  recv_info.to = (ସଂରଚନା sockaddr *)&ctx->q.local_addr;
  recv_info.to_len = ctx->q.local_addrlen;
  recv_info.from = (ସଂରଚନା sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  ପାଇଁ(offset = 0; offset < buflen; offset += gso_size) {
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = quiche_conn_recv(ctx->qconn,
                          (unsigned ଅକ୍ଷର *)CURL_UNCONST(buf + offset),
                          pktlen, &recv_info);
    ଯଦି(!curlx_sztouz(rv, &nread)) {
      ଯଦି(QUICHE_ERR_DONE == rv) {
        ଯଦି(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is draining");
          ଫେରନ୍ତୁ CURLE_RECV_ERROR;
        }
        ଯଦି(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is closed");
          ଫେରନ୍ତୁ CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->data, r->cf, "ingress, quiche is DONE");
        ଫେରନ୍ତୁ CURLE_OK;
      }
      ଅନ୍ୟଥା ଯଦି(QUICHE_ERR_TLS_FAIL == rv) {
        long verify_ok = SSL_get_verify_result(ctx->tls.ossl.ssl);
        ଯଦି(verify_ok != X509_V_OK) {
          failf(r->data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->data, "ingress, quiche reports TLS fail");
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }
      ଅନ୍ୟଥା {
        failf(r->data, "quiche reports error %zd on receive", rv);
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }
    }
    ଅନ୍ୟଥା ଯଦି(nread < pktlen) {
      CURL_TRC_CF(r->data, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_process_ingress(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ସଂରଚନା recv_ctx rctx;
  CURLcode result;

  DEBUGASSERT(ctx->qconn);
  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  rctx.cf = cf;
  rctx.data = data;
  rctx.pkts = 0;

  result = Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(data->multi);
    cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_resume, ଖାଲି);
  }
  ଫେରନ୍ତୁ cf_poll_events(cf, data);
}

ସଂରଚନା read_ctx {
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା Curl_easy *data;
  quiche_send_info send_info;
};

ସ୍ଥିର CURLcode read_pkt_to_send(ଶୂନ୍ୟ *userp,
                                 unsigned ଅକ୍ଷର *buf, size_t buflen,
                                 size_t *pnread)
{
  ସଂରଚନା read_ctx *x = userp;
  ସଂରଚନା cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  ଯଦି(rv == QUICHE_ERR_DONE)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ଯଦି(!curlx_sztouz(rv, pnread)) {
    failf(x->data, "quiche_conn_send returned %zd", rv);
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
ସ୍ଥିର CURLcode cf_flush_egress(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode result;
  int64_t expiry_ns;
  int64_t timeout_ns;
  ସଂରଚନା read_ctx readx;
  size_t pkt_ଗଣନା, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  ଯଦି(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    ଯଦି(quiche_conn_is_closed(ctx->qconn)) {
      ଯଦି(quiche_conn_is_timed_out(ctx->qconn))
        failf(data, "connection closed by idle timeout");
      ଅନ୍ୟଥା
        failf(data, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, data);
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;
    }
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  ଯଦି(result) {
    ଯଦି(result == CURLE_AGAIN) {
      Curl_expire(data, 1, EXPIRE_QUIC);
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଫେରନ୍ତୁ result;
  }

  readx.cf = cf;
  readx.data = data;
  memset(&readx.send_info, 0, ଆକାର(readx.send_info));
  pkt_ଗଣନା = 0;
  gsolen = quiche_conn_max_send_udp_payload_size(ctx->qconn);
  ପାଇଁ(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_to_send, &readx, &nread);
    ଯଦି(result) {
      ଯଦି(result != CURLE_AGAIN)
        ଫେରନ୍ତୁ result;
      /* Nothing more to add, flush and leave */
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      ଯଦି(result) {
        ଯଦି(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          ଫେରନ୍ତୁ CURLE_OK;
        }
        ଫେରନ୍ତୁ result;
      }
      ଯାଅ out;
    }

    ++pkt_ଗଣନା;
    ଯଦି(nread < gsolen || pkt_ଗଣନା >= MAX_PKT_BURST) {
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      ଯଦି(result) {
        ଯଦି(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          ଫେରନ୍ତୁ CURLE_OK;
        }
        ଯାଅ out;
      }
      pkt_ଗଣନା = 0;
    }
  }

out:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  ଯଦି(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(data, (timeout_ns / 1000000), EXPIRE_QUIC);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode recv_closed_stream(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   size_t *pnread)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ସଂରଚନା h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(stream);
  *pnread = 0;
  ଯଦି(stream->reset) {
    ଯଦି(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      ଫେରନ୍ତୁ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ଅନ୍ୟଥା ଯଦି(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        ଫେରନ୍ତୁ CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  }
  ଅନ୍ୟଥା ଯଦି(!stream->resp_got_header) {
    failf(data, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    result = CURLE_HTTP3;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_quiche_recv(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                               ଅକ୍ଷର *buf, size_t blen, size_t *pnread)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ସଂରଚନା h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  *pnread = 0;
  (ଶୂନ୍ୟ)buf;
  (ଶୂନ୍ୟ)blen;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  ଯଦି(!stream)
    ଫେରନ୍ତୁ CURLE_RECV_ERROR;

  result = cf_process_ingress(cf, data);
  ଯଦି(result) {
    CURL_TRC_CF(data, cf, "cf_recv, error on ingress");
    ଯାଅ out;
  }

  ଯଦି(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
    ଯାଅ out;
  }
  ଅନ୍ୟଥା ଯଦି(stream->closed)
    result = recv_closed_stream(cf, data, pnread);
  ଅନ୍ୟଥା ଯଦି(quiche_conn_is_draining(ctx->qconn)) {
    failf(data, "QUIC connection is draining");
    result = CURLE_HTTP3;
  }
  ଅନ୍ୟଥା
    result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));
  ଯଦି(*pnread > 0)
    ctx->data_recvd += *pnread;
  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, stream->id, blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnread,
              ctx->data_recvd);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_quiche_send_body(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା h3_stream_ctx *stream,
                                    ଅଚଳ uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, stream->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  ଯଦି(rv == QUICHE_H3_ERR_DONE || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    ଯଦି(!quiche_conn_stream_writable(ctx->qconn, stream->id, len)) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", stream->id, len);
      stream->quic_flow_blocked = TRUE;
    }
    ଫେରନ୍ତୁ CURLE_AGAIN;
  }
  ଅନ୍ୟଥା ଯଦି(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_STREAM_STATE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", stream->id, len);
    ଫେରନ୍ତୁ CURLE_HTTP3;
  }
  ଅନ୍ୟଥା ଯଦି(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_SIZE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                stream->id, len);
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  }
  ଅନ୍ୟଥା ଯଦି(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                stream->id, len, rv);
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  }
  ଅନ୍ୟଥା {
    ଯଦି(eos && (len == *pnwritten))
      stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                stream->id, len, stream->send_closed, *pnwritten);
    ଫେରନ୍ତୁ CURLE_OK;
  }
}

ସ୍ଥିର CURLcode h3_open_stream(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               ଅଚଳ uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ସଂରଚନା h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  size_t nheader, i;
  int64_t rv;
  ସଂରଚନା dynhds h2_headers;
  quiche_h3_header *nva = ଖାଲି;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  ଯଦି(!stream) {
    result = h3_data_setup(cf, data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    stream = H3_STREAM_CTX(ctx, data);
    DEBUGASSERT(stream);
  }

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(stream);

  result = Curl_h1_req_parse_read(&stream->h1, buf, blen, ଖାଲି,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : ଖାଲି,
    0, pnwritten);
  ଯଦି(result)
    ଯାଅ out;
  ଯଦି(!stream->h1.done) {
    /* need more data */
    ଯାଅ out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  ଯଦି(result)
    ଯାଅ out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_ଗଣନା(&h2_headers);
  nva = curlx_malloc(ଆକାର(quiche_h3_header) * nheader);
  ଯଦି(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  ପାଇଁ(i = 0; i < nheader; ++i) {
    ସଂରଚନା dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned ଅକ୍ଷର *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].ମୂଲ୍ୟ = (unsigned ଅକ୍ଷର *)e->ମୂଲ୍ୟ;
    nva[i].ମୂଲ୍ୟ_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  ଯଦି(eos && !blen)
    stream->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              stream->send_closed);
  CURL_TRC_CF(data, cf, "quiche_send_request() -> %" PRId64, rv);
  ଯଦି(rv < 0) {
    ଯଦି(QUICHE_H3_ERR_STREAM_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] blocked", stream->id);
      stream->quic_flow_blocked = TRUE;
      result = CURLE_AGAIN;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା {
      CURL_TRC_CF(data, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&data->state.url), rv);
    }
    result = CURLE_SEND_ERROR;
    ଯାଅ out;
  }

  DEBUGASSERT(!stream->opened);
  stream->id = (uint64_t)rv;
  stream->opened = TRUE;
  stream->closed = FALSE;
  stream->reset = FALSE;

  ଯଦି(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    ପାଇଁ(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", stream->id,
            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nva[i].name_len, nva[i].name,
            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nva[i].ମୂଲ୍ୟ_len, nva[i].ମୂଲ୍ୟ);
    }
  }

  ଯଦି(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, data, stream, buf, blen, eos, &nwritten);
    ଯଦି(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      result = r2;
    }
    ଅନ୍ୟଥା ଯଦି(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_quiche_send(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                               ଅଚଳ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ସଂରଚନା h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result;

  *pnwritten = 0;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  result = cf_process_ingress(cf, data);
  ଯଦି(result)
    ଯାଅ out;

  ଯଦି(!stream || !stream->opened) {
    result = h3_open_stream(cf, data, buf, len, eos, pnwritten);
    ଯଦି(result)
      ଯାଅ out;
    stream = H3_STREAM_CTX(ctx, data);
  }
  ଅନ୍ୟଥା ଯଦି(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
  }
  ଅନ୍ୟଥା ଯଦି(stream->closed) {
    ଯଦି(stream->resp_hds_complete) {
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
      ଯାଅ out;
    }
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    ଯାଅ out;
  }
  ଅନ୍ୟଥା {
    result = cf_quiche_send_body(cf, data, stream, buf, len, eos, pnwritten);
  }

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));

  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : (uint64_t)~0, len,
              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool stream_is_writable(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ସଂରଚନା h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  ଫେରନ୍ତୁ stream && (quiche_conn_stream_writable(
    ctx->qconn, stream->id, 1) > 0);
}

ସ୍ଥିର CURLcode cf_quiche_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                         ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  ଯଦି(!ctx->qconn)
    ଫେରନ୍ତୁ CURLE_OK;

  Curl_pollset_check(data, ps, ctx->q.sockfd, &want_recv, &want_send);
  ଯଦି(want_recv || want_send) {
    ସଂରଚନା h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
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
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode h3_data_pause(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (ଶୂନ୍ୟ)cf;
  ଯଦି(!pause) {
    Curl_multi_mark_dirty(data);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_quiche_cntrl(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)arg1;
  (ଶୂନ୍ୟ)arg2;
  ବାଛ(event) {
  ସ୍ଥିତି CF_CTRL_DATA_SETUP:
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_CTRL_DATA_DONE:
    h3_data_done(cf, data);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_CTRL_DATA_DONE_SEND: {
    ସଂରଚନା h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    ଯଦି(stream && !stream->send_closed) {
      unsigned ଅକ୍ଷର body[1];
      size_t sent;

      stream->send_closed = TRUE;
      body[0] = 'X';
      result = cf_quiche_send(cf, data, body, 0, TRUE, &sent);
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, sent);
    }
    ଭାଙ୍ଗ;
  }
  ସ୍ଥିତି CF_CTRL_CONN_INFO_UPDATE:
    ଯଦି(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_CTRL_REPORT_STATS:
    ଯଦି(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_quiche_ctx_open(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;
  CURLcode result;
  ଅଚଳ ସଂରଚନା Curl_sockaddr_ex *sockaddr;
  ସ୍ଥିର ଅଚଳ ସଂରଚନା alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_VERSION);
  ଯଦି(!ctx->cfg) {
    failf(data, "cannot create quiche config");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
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
                               &ALPN_SPEC_H3, ଖାଲି, ଖାଲି, cf, ଖାଲି);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  result = Curl_rand(data, ctx->scid, ଆକାର(ctx->scid));
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, ଖାଲି))
    ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = ଆକାର(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (ସଂରଚନା sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  ଯଦି(rv == -1)
    ଫେରନ୍ତୁ CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((ଅଚଳ uint8_t *)ctx->scid,
                                        ଆକାର(ctx->scid), ଖାଲି, 0,
                                        (ସଂରଚନା sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  ଯଦି(!ctx->qconn) {
    failf(data, "cannot create quiche connection");
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  /* Known to not work on Windows */
#ଯଦି !defined(_WIN32) && defined(HAVE_QUICHE_CONN_SET_QLOG_FD)
  {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା qfd;
    (ଶୂନ୍ୟ)Curl_qlogdir(data, ctx->scid, ଆକାର(ctx->scid), &qfd);
    ଯଦି(qfd != -1)
      quiche_conn_set_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#endif

  result = cf_flush_egress(cf, data);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  {
    unsigned ଅକ୍ଷର alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    unsigned alpn_len, offset = 0;

    /* Replace each ALPN length prefix by a comma. */
    ଯେତେବେଳେ(offset < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[offset];
      alpn_protocols[offset] = ',';
      offset += 1 + alpn_len;
    }

    CURL_TRC_CF(data, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_quiche_verify_peer(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  ଫେରନ୍ତୁ Curl_vquic_tls_verify_peer(&ctx->tls, cf, data, &ctx->ssl_peer);
}

ସ୍ଥିର CURLcode cf_quiche_connect(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  bool *done)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  /* Connect the UDP filter first */
  ଯଦି(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ଯଦି(result || !*done)
      ଫେରନ୍ତୁ result;
  }

  *done = FALSE;
  ଯଦି(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    ଫେରନ୍ତୁ CURLE_OK;
  }

  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  ଯଦି(!ctx->qconn) {
    result = cf_quiche_ctx_open(cf, data);
    ଯଦି(result)
      ଯାଅ out;
    ctx->started_at = ctx->q.last_op;
    result = cf_flush_egress(cf, data);
    /* we do not expect to be able to recv anything yet */
    ଯାଅ out;
  }

  result = cf_process_ingress(cf, data);
  ଯଦି(result)
    ଯାଅ out;

  result = cf_flush_egress(cf, data);
  ଯଦି(result)
    ଯାଅ out;

  ଯଦି(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.last_op;
    CURL_TRC_CF(data, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    result = cf_quiche_verify_peer(cf, data);
    ଯଦି(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      ଯଦି(!ctx->h3config) {
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_set_max_field_section_size(ctx->h3config,
                                                  MAX_HTTP_RESP_HEADER_SIZE);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      ଯଦି(!ctx->h3c) {
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
      cf->connected = TRUE;
      *done = TRUE;
    }
  }
  ଅନ୍ୟଥା ଯଦି(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    result = CURLE_WEIRD_SERVER_REPLY;
  }

out:
#ifdef CURLVERBOSE
  ଯଦି(result && result != CURLE_AGAIN) {
    ସଂରଚନା ip_quadruple ip;

    ଯଦି(!Curl_cf_socket_peek(cf->next, data, ଖାଲି, ଖାଲି, &ip))
      infof(data, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    ଅନ୍ୟଥା
      infof(data, "connect failed: %s", curl_easy_strerror(result));
  }
#endif
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_quiche_shutdown(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ଯଦି(cf->shutdown || !ctx || !ctx->qconn) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  *done = FALSE;
  ଯଦି(!ctx->shutdown_started) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, ଖାଲି, 0);
    ଯଦି(err) {
      CURL_TRC_CF(data, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
  }

  ଯଦି(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
    result = cf_flush_egress(cf, data);
    ଯଦି(result)
      ଯାଅ out;
  }

  ଯଦି(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
  }
  ଅନ୍ୟଥା {
    CURL_TRC_CF(data, cf, "shutdown sending blocked");
  }

out:
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_quiche_destroy(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  (ଶୂନ୍ୟ)data;
  ଯଦି(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = ଖାଲି;
  }
}

ସ୍ଥିର CURLcode cf_quiche_query(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;

  ବାଛ(query) {
  ସ୍ଥିତି CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    ଯଦି(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_ବାମ_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > INT_MAX) ? INT_MAX : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)max_streams;
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ସ୍ଥିତି CF_QUERY_CONNECT_REPLY_MS:
    ଯଦି(ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ms : INT_MAX;
    }
    ଅନ୍ୟଥା
      *pres1 = -1;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CF_QUERY_SSL_INFO:
  ସ୍ଥିତି CF_QUERY_SSL_CTX_INFO: {
    ସଂରଚନା curl_tlssessioninfo *info = pres2;
    ଯଦି(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      ଫେରନ୍ତୁ CURLE_OK;
    ଭାଙ୍ଗ;
  }
  ସ୍ଥିତି CF_QUERY_ALPN_NEGOTIATED: {
    ଅଚଳ ଅକ୍ଷର **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ଖାଲି;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ସ୍ଥିର bool cf_quiche_conn_is_alive(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    bool *ନିବେଶ_pending)
{
  ସଂରଚନା cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ନିବେଶ_pending = FALSE;
  ଯଦି(!ctx->qconn)
    ଫେରନ୍ତୁ FALSE;

  ଯଦି(quiche_conn_is_closed(ctx->qconn)) {
    ଯଦି(quiche_conn_is_timed_out(ctx->qconn))
      CURL_TRC_CF(data, cf, "connection was closed due to idle timeout");
    ଅନ୍ୟଥା
      CURL_TRC_CF(data, cf, "connection is closed");
    ଫେରନ୍ତୁ FALSE;
  }

  ଯଦି(!cf->next || !cf->next->cft->is_alive(cf->next, data, ନିବେଶ_pending))
    ଫେରନ୍ତୁ FALSE;

  ଯଦି(*ନିବେଶ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *ନିବେଶ_pending = FALSE;
    ଯଦି(cf_process_ingress(cf, data))
      alive = FALSE;
    ଅନ୍ୟଥା {
      alive = TRUE;
    }
  }

  ଫେରନ୍ତୁ alive;
}

ସଂରଚନା Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_quiche_create(ସଂରଚନା Curl_cfilter **pcf,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_peer *origin,
                               ସଂରଚନା Curl_peer *peer,
                               ସଂରଚନା connectdata *conn,
                               ସଂରଚନା Curl_sockaddr_ex *addr)
{
  ସଂରଚନା cf_quiche_ctx *ctx = ଖାଲି;
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  CURLcode result;

  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  ଯଦି(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ଯଦି(result)
    ଯାଅ out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ଖାଲି, TRNSPRT_QUIC);
  ଯଦି(result)
    ଯାଅ out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : ଖାଲି;
  ଯଦି(result) {
    ଯଦି(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    ଅନ୍ୟଥା ଯଦି(ctx)
      cf_quiche_ctx_free(ctx);
  }

  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cf_quiche_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                     ସଂରଚନା Curl_peer *origin,
                                     ସଂରଚନା Curl_peer *peer,
                                     ସଂରଚନା Curl_cfilter **pcf)
{
  ସଂରଚନା cf_quiche_ctx *ctx = ଖାଲି;
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  CURLcode result;

  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  ଯଦି(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ଯଦି(result)
    ଯାଅ out;
  Curl_conn_cf_ଭର୍ତ୍ତି_after(cf_at, cf);

out:
  ଯଦି(result) {
    curlx_safefree(cf);
    ଯଦି(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  ଫେରନ୍ତୁ result;
}

#endif
