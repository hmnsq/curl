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

#define H3_STREAM_WINDOW_আকার  (1024 * 128)
#define H3_STREAM_CHUNK_আকার   (1024 * 16)
/* Receive and Send max number of chunks follows from the
 * chunk size and window size */
#define H3_STREAM_RECV_CHUNKS \
  (H3_STREAM_WINDOW_আকার / H3_STREAM_CHUNK_আকার)

/*
 * Store quiche version info in this buffer.
 */
শূন্য Curl_quiche_ver(অক্ষর *p, size_t len)
{
  (শূন্য)curl_msnprintf(p, len, "quiche/%s", quiche_version());
}

কাঠামো cf_quiche_ctx {
  কাঠামো cf_quic_ctx q;
  কাঠামো ssl_peer ssl_peer;
  কাঠামো curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  কাঠামো curltime started_at;        /* time the current attempt started */
  কাঠামো curltime handshake_at;      /* time connect handshake finished */
  কাঠামো u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  কাঠামো dynbuf h1hdr;               /* temp buffer for header construction */
  কাঠামো bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t তথ্য_recvd;
  BIT(initialized);
  BIT(goaway);                       /* got GOAWAY from server */
  BIT(x509_store_setup);             /* if x509 store has been set up */
  BIT(shutdown_started);             /* queued shutdown packets */
  BIT(stats_reported);               /* connect statistics reported */
};

#ifdef DEBUG_QUICHE
/* initialize debug log callback only once */
স্থির পূর্ণসংখ্যা debug_log_init = 0;
স্থির শূন্য quiche_debug_log(ধ্রুবক অক্ষর *সারি, শূন্য *argp)
{
  (শূন্য)argp;
  curl_mfprintf(stderr, "%s\n", সারি);
}
#endif

স্থির শূন্য h3_stream_set_free(uint32_t id, শূন্য *stream);

স্থির CURLcode cf_quiche_ctx_init(কাঠামো cf_quiche_ctx *ctx,
                                   কাঠামো Curl_peer *origin,
                                   কাঠামো Curl_peer *peer,
                                   কাঠামো ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#ifdef DEBUG_QUICHE
  যদি(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, ফাঁকা);
    debug_log_init = 1;
  }
#endif
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  Curl_bufq_init2(&ctx->writebuf, H3_STREAM_CHUNK_আকার, H3_STREAM_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  ctx->তথ্য_recvd = 0;
  ctx->initialized = TRUE;
  ফেরত Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

স্থির শূন্য cf_quiche_ctx_free(কাঠামো cf_quiche_ctx *ctx)
{
  যদি(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = ফাঁকা;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

স্থির শূন্য cf_quiche_ctx_close(কাঠামো cf_quiche_ctx *ctx)
{
  যদি(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = ফাঁকা;
  }
  যদি(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = ফাঁকা;
  }
  যদি(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = ফাঁকা;
  }
  যদি(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = ফাঁকা;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

স্থির CURLcode cf_flush_egress(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য);

/**
 * All about the H3 internals of a stream
 */
কাঠামো h3_stream_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  কাঠামো h1_req_parser h1; /* h1 request parsing */
  uint64_t error3;         /* HTTP/3 stream error code */
  int32_t status_code;     /* HTTP status code */
  CURLcode xfer_ফলাফল;    /* result from cf_quiche_write_(hd/body) */
  BIT(opened);             /* TRUE after stream has been opened */
  BIT(closed);             /* TRUE on stream close */
  BIT(reset);              /* TRUE on stream reset */
  BIT(send_closed);        /* stream is locally closed */
  BIT(resp_hds_complete);  /* final response has been received */
  BIT(resp_got_header);    /* TRUE when h3 stream has recvd some HEADER */
  BIT(quic_flow_blocked);  /* stream is blocked by QUIC flow control */
};

স্থির শূন্য h3_stream_ctx_free(কাঠামো h3_stream_ctx *stream)
{
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

স্থির শূন্য h3_stream_set_free(uint32_t id, শূন্য *stream)
{
  (শূন্য)id;
  DEBUGASSERT(stream);
  h3_stream_ctx_free((কাঠামো h3_stream_ctx *)stream);
}

প্রকারলেখ bool cf_quiche_svisit(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *sdata,
                              কাঠামো h3_stream_ctx *stream,
                              শূন্য *user_তথ্য);

কাঠামো cf_quiche_visit_ctx {
  কাঠামো Curl_cfilter *cf;
  কাঠামো Curl_multi *multi;
  cf_quiche_svisit *cb;
  শূন্য *user_তথ্য;
};

স্থির bool cf_quiche_stream_do(uint32_t mid, শূন্য *val, শূন্য *user_তথ্য)
{
  কাঠামো cf_quiche_visit_ctx *vctx = user_তথ্য;
  কাঠামো h3_stream_ctx *stream = val;
  কাঠামো Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  যদি(sdata)
    ফেরত vctx->cb(vctx->cf, sdata, stream, vctx->user_তথ্য);
  ফেরত TRUE;
}

স্থির শূন্য cf_quiche_for_all_streams(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      শূন্য *user_তথ্য)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  কাঠামো cf_quiche_visit_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_তথ্য = user_তথ্য;
  Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_stream_do, &vctx);
}

স্থির bool cf_quiche_do_resume(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *sdata,
                                কাঠামো h3_stream_ctx *stream,
                                শূন্য *user_তথ্য)
{
  (শূন্য)user_তথ্য;
  যদি(stream->quic_flow_blocked) {
    stream->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", stream->id);
  }
  ফেরত TRUE;
}

স্থির bool cf_quiche_do_expire(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *sdata,
                                কাঠামো h3_stream_ctx *stream,
                                শূন্য *user_তথ্য)
{
  (শূন্য)user_তথ্য;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  stream->xfer_ফলাফল = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  ফেরত TRUE;
}

স্থির CURLcode h3_তথ্য_setup(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);

  যদি(stream)
    ফেরত CURLE_OK;

  stream = curlx_calloc(1, আকার(*stream));
  যদি(!stream)
    ফেরত CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_সারি_LEN);

  যদি(Curl_u32_ptrset_set(&ctx->streams, তথ্য->mid, stream)) {
    h3_stream_ctx_free(stream);
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  ফেরত CURLE_OK;
}

স্থির শূন্য cf_quiche_stream_close(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো h3_stream_ctx *stream)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ফলাফল;

  যদি(ctx->qconn && !stream->closed) {
    quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    যদি(!stream->send_closed) {
      quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      stream->send_closed = TRUE;
    }
    stream->closed = TRUE;
    ফলাফল = cf_flush_egress(cf, তথ্য);
    যদি(ফলাফল)
      CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  stream->id, (পূর্ণসংখ্যা)ফলাফল);
  }
}

স্থির শূন্য h3_তথ্য_done(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);

  (শূন্য)cf;
  যদি(stream) {
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] easy handle is done", stream->id);
    cf_quiche_stream_close(cf, তথ্য, stream);
    Curl_u32_ptrset_unset(&ctx->streams, তথ্য->mid);
  }
}

স্থির শূন্য cf_quiche_expire_conn_closed(কাঠামো Curl_cfilter *cf,
                                         কাঠামো Curl_easy *তথ্য)
{
  DEBUGASSERT(তথ্য->multi);
  CURL_TRC_CF(তথ্য, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, তথ্য->multi, cf_quiche_do_expire, ফাঁকা);
}

স্থির শূন্য cf_quiche_write_hd(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো h3_stream_ctx *stream,
                               ধ্রুবক অক্ষর *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  যদি(!stream->xfer_ফলাফল) {
    stream->xfer_ফলাফল = Curl_xfer_write_resp_hd(তথ্য, buf, blen, eos);
    যদি(stream->xfer_ফলাফল)
      CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (পূর্ণসংখ্যা)stream->xfer_ফলাফল,
                  blen);
  }
}

কাঠামো cb_ctx {
  কাঠামো Curl_cfilter *cf;
  কাঠামো Curl_easy *তথ্য;
  কাঠামো h3_stream_ctx *stream;
};

স্থির bool is_valid_h3_header(ধ্রুবক uint8_t *hdr, size_t hlen)
{
  যখন(hlen--) {
    নির্বাচন(*hdr++) {
    ক্ষেত্র '\n':
    ক্ষেত্র '\r':
    ক্ষেত্র '\0':
      ফেরত FALSE;
    }
  }
  ফেরত TRUE;
}

স্থির পূর্ণসংখ্যা cb_each_header(uint8_t *name, size_t name_len,
                          uint8_t *মান, size_t মান_len,
                          শূন্য *argp)
{
  কাঠামো cb_ctx *x = argp;
  কাঠামো Curl_cfilter *cf = x->cf;
  কাঠামো Curl_easy *তথ্য = x->তথ্য;
  কাঠামো h3_stream_ctx *stream = x->stream;
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!stream || stream->xfer_ফলাফল)
    ফেরত 1; /* abort iteration */

  যদি((name_len == 7) && !strncmp(HTTP_PSEUDO_STATUS, (অক্ষর *)name, 7) &&
     is_valid_h3_header(মান, মান_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    ফলাফল = Curl_http_decode_status(&stream->status_code,
                                     (ধ্রুবক অক্ষর *)মান, মান_len);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(&ctx->h1hdr, (ধ্রুবক অক্ষর *)মান, মান_len);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    যদি(!ফলাফল)
      cf_quiche_write_hd(cf, তথ্য, stream, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] status: %s",
                stream->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  অন্যথা {
    যদি(is_valid_h3_header(মান, মান_len) &&
       is_valid_h3_header(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  stream->id, (পূর্ণসংখ্যা)name_len, name,
                  (পূর্ণসংখ্যা)মান_len, মান);
      curlx_dyn_reset(&ctx->h1hdr);
      ফলাফল = curlx_dyn_addn(&ctx->h1hdr, (ধ্রুবক অক্ষর *)name, name_len);
      যদি(!ফলাফল)
        ফলাফল = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      যদি(!ফলাফল)
        ফলাফল = curlx_dyn_addn(&ctx->h1hdr, (ধ্রুবক অক্ষর *)মান, মান_len);
      যদি(!ফলাফল)
        ফলাফল = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      যদি(!ফলাফল)
        cf_quiche_write_hd(cf, তথ্য, stream, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    অন্যথা
      CURL_TRC_CF(x->তথ্য, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  stream->id, মান_len + name_len);
  }

  যদি(ফলাফল) {
    CURL_TRC_CF(x->তথ্য, x->cf, "[%" PRIu64 "] on header error %d",
                stream->id, (পূর্ণসংখ্যা)ফলাফল);
    যদি(!stream->xfer_ফলাফল)
      stream->xfer_ফলাফল = ফলাফল;
  }
  ফেরত stream->xfer_ফলাফল ? 1 : 0;
}

স্থির CURLcode stream_resp_read(শূন্য *reader_ctx,
                                 unsigned অক্ষর *buf, size_t len,
                                 size_t *pnread)
{
  কাঠামো cb_ctx *x = reader_ctx;
  কাঠামো cf_quiche_ctx *ctx = x->cf->ctx;
  কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, x->তথ্য);
  ssize_t nread;

  *pnread = 0;
  যদি(!stream)
    ফেরত CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, stream->id, buf, len);
  যদি(!curlx_sztouz(nread, pnread))
    ফেরত CURLE_AGAIN;
  ফেরত CURLE_OK;
}

স্থির শূন্য cf_quiche_flush_body(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো h3_stream_ctx *stream)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  ধ্রুবক uint8_t *buf;
  size_t blen;

  যখন(stream && !stream->xfer_ফলাফল) {
    যদি(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      stream->xfer_ফলাফল = Curl_xfer_write_resp(
        তথ্য, (ধ্রুবক অক্ষর *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      যদি(stream->xfer_ফলাফল) {
        CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", stream->id, (পূর্ণসংখ্যা)stream->xfer_ফলাফল, blen);
      }
    }
    অন্যথা
      বিরতি;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

স্থির শূন্য cf_quiche_recv_body(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                কাঠামো h3_stream_ctx *stream)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  কাঠামো cb_ctx cb_ctx;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!stream)
    ফেরত;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.তথ্য = তথ্য;
  cb_ctx.stream = stream;
  Curl_bufq_reset(&ctx->writebuf);
  যখন(!ফলাফল) {
    ফলাফল = Curl_bufq_slurp(&ctx->writebuf,
                             stream_resp_read, &cb_ctx, &nread);
    যদি(!ফলাফল)
      cf_quiche_flush_body(cf, তথ্য, stream);
    অন্যথা যদি(ফলাফল == CURLE_AGAIN)
      বিরতি;
    অন্যথা যদি(ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] recv_body error %d",
                  stream->id, (পূর্ণসংখ্যা)ফলাফল);
      failf(তথ্য, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            stream->id, (পূর্ণসংখ্যা)ফলাফল);
      stream->closed = TRUE;
      stream->reset = TRUE;
      stream->send_closed = TRUE;
      যদি(!stream->xfer_ফলাফল)
        stream->xfer_ফলাফল = ফলাফল;
    }
  }
  cf_quiche_flush_body(cf, তথ্য, stream);
}

স্থির শূন্য cf_quiche_process_ev(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো h3_stream_ctx *stream,
                                 quiche_h3_event *ev)
{
  গণনা quiche_h3_event_type type;

  যদি(!stream)
    ফেরত;

  type = quiche_h3_event_type(ev);

  নির্বাচন(type) {
  ক্ষেত্র QUICHE_H3_EVENT_HEADERS: {
    কাঠামো cb_ctx cb_ctx;
    stream->resp_got_header = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.তথ্য = তথ্য;
    cb_ctx.stream = stream;
    quiche_h3_event_for_each_header(ev, cb_each_header, &cb_ctx);
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] <- [HEADERS]", stream->id);
    Curl_multi_mark_dirty(তথ্য);
    বিরতি;
  }
  ক্ষেত্র QUICHE_H3_EVENT_তথ্য:
    যদি(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, তথ্য, stream, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, তথ্য, stream);
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] <- [DATA]", stream->id);
    Curl_multi_mark_dirty(তথ্য);
    বিরতি;

  ক্ষেত্র QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] RESET", stream->id);
    stream->closed = TRUE;
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    Curl_multi_mark_dirty(তথ্য);
    বিরতি;

  ক্ষেত্র QUICHE_H3_EVENT_FINISHED:
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] CLOSED", stream->id);
    যদি(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, তথ্য, stream, "\r\n", 2, TRUE);
    }
    stream->closed = TRUE;
    Curl_multi_mark_dirty(তথ্য);
    বিরতি;

  ক্ষেত্র QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] <- [GOAWAY]", stream->id);
    বিরতি;

  পূর্বনির্ধারিত:
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] recv, unhandled event %d",
                stream->id, (পূর্ণসংখ্যা)type);
    বিরতি;
  }
}

কাঠামো cf_quiche_disp_ctx {
  uint64_t stream_id;
  কাঠামো Curl_cfilter *cf;
  কাঠামো Curl_multi *multi;
  quiche_h3_event *ev;
};

স্থির bool cf_quiche_disp_event(uint32_t mid, শূন্য *val, শূন্য *user_তথ্য)
{
  কাঠামো cf_quiche_disp_ctx *dctx = user_তথ্য;
  কাঠামো h3_stream_ctx *stream = val;

  যদি(stream->id == dctx->stream_id) {
    কাঠামো Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    যদি(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, stream, dctx->ev);
    ফেরত FALSE; /* stop iterating */
  }
  ফেরত TRUE;
}

স্থির CURLcode cf_poll_events(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = ফাঁকা;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  যখন(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    যদি(rv == QUICHE_H3_ERR_DONE) {
      বিরতি;
    }
    অন্যথা যদি(rv < 0) {
      CURL_TRC_CF(তথ্য, cf, "error poll: %" PRId64, rv);
      ফেরত CURLE_HTTP3;
    }
    অন্যথা {
      stream = H3_STREAM_CTX(ctx, তথ্য);
      যদি(stream && stream->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, তথ্য, stream, ev);
        quiche_h3_event_free(ev);
        যদি(stream->xfer_ফলাফল)
          ফেরত stream->xfer_ফলাফল;
      }
      অন্যথা {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        কাঠামো cf_quiche_disp_ctx dctx;
        dctx.stream_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = তথ্য->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  ফেরত CURLE_OK;
}

কাঠামো recv_ctx {
  কাঠামো Curl_cfilter *cf;
  কাঠামো Curl_easy *তথ্য;
  পূর্ণসংখ্যা pkts;
};

স্থির CURLcode cf_quiche_recv_pkts(ধ্রুবক unsigned অক্ষর *buf, size_t buflen,
                                    size_t gso_আকার,
                                    কাঠামো sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    শূন্য *userp)
{
  কাঠামো recv_ctx *r = userp;
  কাঠামো cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, offset, nread;
  ssize_t rv;

  (শূন্য)ecn;

  recv_info.to = (কাঠামো sockaddr *)&ctx->q.local_addr;
  recv_info.to_len = ctx->q.local_addrlen;
  recv_info.from = (কাঠামো sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  জন্য(offset = 0; offset < buflen; offset += gso_আকার) {
    pktlen = ((offset + gso_আকার) <= buflen) ? gso_আকার : (buflen - offset);
    rv = quiche_conn_recv(ctx->qconn,
                          (unsigned অক্ষর *)CURL_UNCONST(buf + offset),
                          pktlen, &recv_info);
    যদি(!curlx_sztouz(rv, &nread)) {
      যদি(QUICHE_ERR_DONE == rv) {
        যদি(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->তথ্য, r->cf, "ingress, connection is draining");
          ফেরত CURLE_RECV_ERROR;
        }
        যদি(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->তথ্য, r->cf, "ingress, connection is closed");
          ফেরত CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->তথ্য, r->cf, "ingress, quiche is DONE");
        ফেরত CURLE_OK;
      }
      অন্যথা যদি(QUICHE_ERR_TLS_FAIL == rv) {
        long verify_ok = SSL_get_verify_ফলাফল(ctx->tls.ossl.ssl);
        যদি(verify_ok != X509_V_OK) {
          failf(r->তথ্য, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          ফেরত CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->তথ্য, "ingress, quiche reports TLS fail");
        ফেরত CURLE_RECV_ERROR;
      }
      অন্যথা {
        failf(r->তথ্য, "quiche reports error %zd on receive", rv);
        ফেরত CURLE_RECV_ERROR;
      }
    }
    অন্যথা যদি(nread < pktlen) {
      CURL_TRC_CF(r->তথ্য, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode cf_process_ingress(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  কাঠামো recv_ctx rctx;
  CURLcode ফলাফল;

  DEBUGASSERT(ctx->qconn);
  ফলাফল = Curl_vquic_tls_before_recv(&ctx->tls, cf, তথ্য);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  rctx.cf = cf;
  rctx.তথ্য = তথ্য;
  rctx.pkts = 0;

  ফলাফল = Curl_vquic_recv_packets(cf, তথ্য, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(তথ্য->multi);
    cf_quiche_for_all_streams(cf, তথ্য->multi, cf_quiche_do_resume, ফাঁকা);
  }
  ফেরত cf_poll_events(cf, তথ্য);
}

কাঠামো read_ctx {
  কাঠামো Curl_cfilter *cf;
  কাঠামো Curl_easy *তথ্য;
  quiche_send_info send_info;
};

স্থির CURLcode read_pkt_to_send(শূন্য *userp,
                                 unsigned অক্ষর *buf, size_t buflen,
                                 size_t *pnread)
{
  কাঠামো read_ctx *x = userp;
  কাঠামো cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  যদি(rv == QUICHE_ERR_DONE)
    ফেরত CURLE_AGAIN;

  যদি(!curlx_sztouz(rv, pnread)) {
    failf(x->তথ্য, "quiche_conn_send returned %zd", rv);
    ফেরত CURLE_SEND_ERROR;
  }
  ফেরত CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
স্থির CURLcode cf_flush_egress(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode ফলাফল;
  int64_t expiry_ns;
  int64_t timeout_ns;
  কাঠামো read_ctx readx;
  size_t pkt_গুনতি, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  যদি(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    যদি(quiche_conn_is_closed(ctx->qconn)) {
      যদি(quiche_conn_is_timed_out(ctx->qconn))
        failf(তথ্য, "connection closed by idle timeout");
      অন্যথা
        failf(তথ্য, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, তথ্য);
      ফেরত CURLE_SEND_ERROR;
    }
  }

  ফলাফল = Curl_vquic_flush(cf, তথ্য, &ctx->q);
  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_AGAIN) {
      Curl_expire(তথ্য, 1, EXPIRE_QUIC);
      ফেরত CURLE_OK;
    }
    ফেরত ফলাফল;
  }

  readx.cf = cf;
  readx.তথ্য = তথ্য;
  memset(&readx.send_info, 0, আকার(readx.send_info));
  pkt_গুনতি = 0;
  gsolen = quiche_conn_max_send_udp_payload_আকার(ctx->qconn);
  জন্য(;;) {
    /* add the next packet to send, if any, to our buffer */
    ফলাফল = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_to_send, &readx, &nread);
    যদি(ফলাফল) {
      যদি(ফলাফল != CURLE_AGAIN)
        ফেরত ফলাফল;
      /* Nothing more to add, flush and leave */
      ফলাফল = Curl_vquic_send(cf, তথ্য, &ctx->q, gsolen);
      যদি(ফলাফল) {
        যদি(ফলাফল == CURLE_AGAIN) {
          Curl_expire(তথ্য, 1, EXPIRE_QUIC);
          ফেরত CURLE_OK;
        }
        ফেরত ফলাফল;
      }
      যাও out;
    }

    ++pkt_গুনতি;
    যদি(nread < gsolen || pkt_গুনতি >= MAX_PKT_BURST) {
      ফলাফল = Curl_vquic_send(cf, তথ্য, &ctx->q, gsolen);
      যদি(ফলাফল) {
        যদি(ফলাফল == CURLE_AGAIN) {
          Curl_expire(তথ্য, 1, EXPIRE_QUIC);
          ফেরত CURLE_OK;
        }
        যাও out;
      }
      pkt_গুনতি = 0;
    }
  }

out:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  যদি(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(তথ্য, (timeout_ns / 1000000), EXPIRE_QUIC);
  ফেরত ফলাফল;
}

স্থির CURLcode recv_closed_stream(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   size_t *pnread)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(stream);
  *pnread = 0;
  যদি(stream->reset) {
    যদি(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(তথ্য, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      তথ্য->state.refused_stream = TRUE;
      ফেরত CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    অন্যথা যদি(stream->resp_hds_complete && তথ্য->req.no_body) {
        CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        ফেরত CURLE_OK;
    }
    failf(তথ্য, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    ফলাফল = তথ্য->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                stream->id, (পূর্ণসংখ্যা)ফলাফল);
  }
  অন্যথা যদি(!stream->resp_got_header) {
    failf(তথ্য, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    ফলাফল = CURLE_HTTP3;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_quiche_recv(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                               অক্ষর *buf, size_t blen, size_t *pnread)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);
  CURLcode ফলাফল = CURLE_OK;

  *pnread = 0;
  (শূন্য)buf;
  (শূন্য)blen;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(তথ্য));

  যদি(!stream)
    ফেরত CURLE_RECV_ERROR;

  ফলাফল = cf_process_ingress(cf, তথ্য);
  যদি(ফলাফল) {
    CURL_TRC_CF(তথ্য, cf, "cf_recv, error on ingress");
    যাও out;
  }

  যদি(stream->xfer_ফলাফল) {
    cf_quiche_stream_close(cf, তথ্য, stream);
    ফলাফল = stream->xfer_ফলাফল;
    যাও out;
  }
  অন্যথা যদি(stream->closed)
    ফলাফল = recv_closed_stream(cf, তথ্য, pnread);
  অন্যথা যদি(quiche_conn_is_draining(ctx->qconn)) {
    failf(তথ্য, "QUIC connection is draining");
    ফলাফল = CURLE_HTTP3;
  }
  অন্যথা
    ফলাফল = CURLE_AGAIN;

out:
  ফলাফল = Curl_1st_fatal(ফলাফল, cf_flush_egress(cf, তথ্য));
  যদি(*pnread > 0)
    ctx->তথ্য_recvd += *pnread;
  CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, stream->id, blen, (পূর্ণসংখ্যা)ফলাফল, *pnread,
              ctx->তথ্য_recvd);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_quiche_send_body(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    কাঠামো h3_stream_ctx *stream,
                                    ধ্রুবক uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, stream->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  যদি(rv == QUICHE_H3_ERR_DONE || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    যদি(!quiche_conn_stream_writable(ctx->qconn, stream->id, len)) {
      CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", stream->id, len);
      stream->quic_flow_blocked = TRUE;
    }
    ফেরত CURLE_AGAIN;
  }
  অন্যথা যদি(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_STREAM_STATE) {
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", stream->id, len);
    ফেরত CURLE_HTTP3;
  }
  অন্যথা যদি(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_আকার) {
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                stream->id, len);
    ফেরত CURLE_SEND_ERROR;
  }
  অন্যথা যদি(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                stream->id, len, rv);
    ফেরত CURLE_SEND_ERROR;
  }
  অন্যথা {
    যদি(eos && (len == *pnwritten))
      stream->send_closed = TRUE;
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                stream->id, len, stream->send_closed, *pnwritten);
    ফেরত CURLE_OK;
  }
}

স্থির CURLcode h3_open_stream(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               ধ্রুবক uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);
  size_t nheader, i;
  int64_t rv;
  কাঠামো dynhds h2_headers;
  quiche_h3_header *nva = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  *pnwritten = 0;
  যদি(!stream) {
    ফলাফল = h3_তথ্য_setup(cf, তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    stream = H3_STREAM_CTX(ctx, তথ্য);
    DEBUGASSERT(stream);
  }

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(stream);

  ফলাফল = Curl_h1_req_parse_read(&stream->h1, buf, blen, ফাঁকা,
    !তথ্য->state.http_ignorecustom ?
    CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST) : ফাঁকা,
    0, pnwritten);
  যদি(ফলাফল)
    যাও out;
  যদি(!stream->h1.done) {
    /* need more data */
    যাও out;
  }
  DEBUGASSERT(stream->h1.req);

  ফলাফল = Curl_http_req_to_h2(&h2_headers, stream->h1.req, তথ্য);
  যদি(ফলাফল)
    যাও out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_গুনতি(&h2_headers);
  nva = curlx_malloc(আকার(quiche_h3_header) * nheader);
  যদি(!nva) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  জন্য(i = 0; i < nheader; ++i) {
    কাঠামো dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned অক্ষর *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].মান = (unsigned অক্ষর *)e->মান;
    nva[i].মান_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  যদি(eos && !blen)
    stream->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              stream->send_closed);
  CURL_TRC_CF(তথ্য, cf, "quiche_send_request() -> %" PRId64, rv);
  যদি(rv < 0) {
    যদি(QUICHE_H3_ERR_STREAM_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] blocked", stream->id);
      stream->quic_flow_blocked = TRUE;
      ফলাফল = CURLE_AGAIN;
      যাও out;
    }
    অন্যথা {
      CURL_TRC_CF(তথ্য, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&তথ্য->state.url), rv);
    }
    ফলাফল = CURLE_SEND_ERROR;
    যাও out;
  }

  DEBUGASSERT(!stream->opened);
  stream->id = (uint64_t)rv;
  stream->opened = TRUE;
  stream->closed = FALSE;
  stream->reset = FALSE;

  যদি(Curl_trc_is_verbose(তথ্য)) {
    infof(তথ্য, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&তথ্য->state.url));
    জন্য(i = 0; i < nheader; ++i) {
      infof(তথ্য, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", stream->id,
            (পূর্ণসংখ্যা)nva[i].name_len, nva[i].name,
            (পূর্ণসংখ্যা)nva[i].মান_len, nva[i].মান);
    }
  }

  যদি(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, তথ্য, stream, buf, blen, eos, &nwritten);
    যদি(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      ফলাফল = r2;
    }
    অন্যথা যদি(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_quiche_send(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                               ধ্রুবক uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);
  CURLcode ফলাফল;

  *pnwritten = 0;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(তথ্য));

  ফলাফল = cf_process_ingress(cf, তথ্য);
  যদি(ফলাফল)
    যাও out;

  যদি(!stream || !stream->opened) {
    ফলাফল = h3_open_stream(cf, তথ্য, buf, len, eos, pnwritten);
    যদি(ফলাফল)
      যাও out;
    stream = H3_STREAM_CTX(ctx, তথ্য);
  }
  অন্যথা যদি(stream->xfer_ফলাফল) {
    cf_quiche_stream_close(cf, তথ্য, stream);
    ফলাফল = stream->xfer_ফলাফল;
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
      CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] discarding data "
                  "on closed stream with response", stream->id);
      ফলাফল = CURLE_OK;
      *pnwritten = len;
      যাও out;
    }
    CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    ফলাফল = CURLE_HTTP3;
    যাও out;
  }
  অন্যথা {
    ফলাফল = cf_quiche_send_body(cf, তথ্য, stream, buf, len, eos, pnwritten);
  }

out:
  ফলাফল = Curl_1st_fatal(ফলাফল, cf_flush_egress(cf, তথ্য));

  CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : (uint64_t)~0, len,
              (পূর্ণসংখ্যা)ফলাফল, *pnwritten);
  ফেরত ফলাফল;
}

স্থির bool stream_is_writable(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);

  ফেরত stream && (quiche_conn_stream_writable(
    ctx->qconn, stream->id, 1) > 0);
}

স্থির CURLcode cf_quiche_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                         কাঠামো Curl_easy *তথ্য,
                                         কাঠামো easy_pollset *ps)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!ctx->qconn)
    ফেরত CURLE_OK;

  Curl_pollset_check(তথ্য, ps, ctx->q.sockfd, &want_recv, &want_send);
  যদি(want_recv || want_send) {
    কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);
    bool c_exhaust, s_exhaust;

    c_exhaust = FALSE; /* Have not found any call in quiche that tells
                          us if the connection itself is blocked */
    s_exhaust = want_send && stream && stream->opened &&
                (stream->quic_flow_blocked || !stream_is_writable(cf, তথ্য));
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    ফলাফল = Curl_pollset_set(তথ্য, ps, ctx->q.sockfd, want_recv, want_send);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode h3_তথ্য_pause(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (শূন্য)cf;
  যদি(!pause) {
    Curl_multi_mark_dirty(তথ্য);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode cf_quiche_cntrl(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)arg1;
  (শূন্য)arg2;
  নির্বাচন(event) {
  ক্ষেত্র CF_CTRL_তথ্য_SETUP:
    বিরতি;
  ক্ষেত্র CF_CTRL_তথ্য_PAUSE:
    ফলাফল = h3_তথ্য_pause(cf, তথ্য, (arg1 != 0));
    বিরতি;
  ক্ষেত্র CF_CTRL_তথ্য_DONE:
    h3_তথ্য_done(cf, তথ্য);
    বিরতি;
  ক্ষেত্র CF_CTRL_তথ্য_DONE_SEND: {
    কাঠামো h3_stream_ctx *stream = H3_STREAM_CTX(ctx, তথ্য);
    যদি(stream && !stream->send_closed) {
      unsigned অক্ষর body[1];
      size_t sent;

      stream->send_closed = TRUE;
      body[0] = 'X';
      ফলাফল = cf_quiche_send(cf, তথ্য, body, 0, TRUE, &sent);
      CURL_TRC_CF(তথ্য, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  stream->id, (পূর্ণসংখ্যা)ফলাফল, sent);
    }
    বিরতি;
  }
  ক্ষেত্র CF_CTRL_CONN_INFO_UPDATE:
    যদি(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    বিরতি;
  ক্ষেত্র CF_CTRL_REPORT_STATS:
    যদি(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrsTimeWas(তথ্য, TIMER_CONNECT, ctx->q.first_byte_at);
      Curl_pgrsTimeWas(তথ্য, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_quiche_ctx_open(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  পূর্ণসংখ্যা rv;
  CURLcode ফলাফল;
  ধ্রুবক কাঠামো Curl_sockaddr_ex *sockaddr;
  স্থির ধ্রুবক কাঠামো alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  ফলাফল = Curl_vquic_ctx_init(তথ্য, &ctx->q);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_VERSION);
  যদি(!ctx->cfg) {
    failf(তথ্য, "cannot create quiche config");
    ফেরত CURLE_FAILED_INIT;
  }
  quiche_config_enable_pacing(ctx->cfg, FALSE);
  quiche_config_set_initial_max_তথ্য(ctx->cfg, (1 * 1024 * 1024)
    /* (QUIC_MAX_STREAMS/2) * H3_STREAM_WINDOW_SIZE */);
  quiche_config_set_initial_max_streams_bidi(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_set_initial_max_streams_uni(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_set_initial_max_stream_তথ্য_bidi_local(ctx->cfg,
    H3_STREAM_WINDOW_আকার);
  quiche_config_set_initial_max_stream_তথ্য_bidi_remote(ctx->cfg,
    H3_STREAM_WINDOW_আকার);
  quiche_config_set_initial_max_stream_তথ্য_uni(ctx->cfg,
    H3_STREAM_WINDOW_আকার);
  quiche_config_set_disable_active_migration(ctx->cfg, TRUE);

  quiche_config_set_max_connection_window(ctx->cfg,
    10 * QUIC_MAX_STREAMS * H3_STREAM_WINDOW_আকার);
  quiche_config_set_max_stream_window(ctx->cfg, 10 * H3_STREAM_WINDOW_আকার);
  quiche_config_set_application_protos(ctx->cfg,
                      (uint8_t *)CURL_UNCONST(QUICHE_H3_APPLICATION_PROTOCOL),
                                 CURL_CSTRLEN(QUICHE_H3_APPLICATION_PROTOCOL));

  ফলাফল = Curl_vquic_tls_init(&ctx->tls, cf, তথ্য, &ctx->ssl_peer,
                               &ALPN_SPEC_H3, ফাঁকা, ফাঁকা, cf, ফাঁকা);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = Curl_rand(তথ্য, ctx->scid, আকার(ctx->scid));
  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(Curl_cf_socket_peek(cf->পরবর্তী, তথ্য, &ctx->q.sockfd, &sockaddr, ফাঁকা))
    ফেরত CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = আকার(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (কাঠামো sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  যদি(rv == -1)
    ফেরত CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((ধ্রুবক uint8_t *)ctx->scid,
                                        আকার(ctx->scid), ফাঁকা, 0,
                                        (কাঠামো sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  যদি(!ctx->qconn) {
    failf(তথ্য, "cannot create quiche connection");
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  /* Known to not work on Windows */
#যদি !defined(_WIN32) && defined(HAVE_QUICHE_CONN_SET_QLOG_FD)
  {
    পূর্ণসংখ্যা qfd;
    (শূন্য)Curl_qlogdir(তথ্য, ctx->scid, আকার(ctx->scid), &qfd);
    যদি(qfd != -1)
      quiche_conn_set_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#endif

  ফলাফল = cf_flush_egress(cf, তথ্য);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  {
    unsigned অক্ষর alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    unsigned alpn_len, offset = 0;

    /* Replace each ALPN length prefix by a comma. */
    যখন(offset < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[offset];
      alpn_protocols[offset] = ',';
      offset += 1 + alpn_len;
    }

    CURL_TRC_CF(তথ্য, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode cf_quiche_verify_peer(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  ফেরত Curl_vquic_tls_verify_peer(&ctx->tls, cf, তথ্য, &ctx->ssl_peer);
}

স্থির CURLcode cf_quiche_connect(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  bool *done)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  /* Connect the UDP filter first */
  যদি(!cf->পরবর্তী->connected) {
    ফলাফল = Curl_conn_cf_connect(cf->পরবর্তী, তথ্য, done);
    যদি(ফলাফল || !*done)
      ফেরত ফলাফল;
  }

  *done = FALSE;
  যদি(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(তথ্য, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(তথ্য, cf, "need HTTPS-RR, delaying connect");
    ফেরত CURLE_OK;
  }

  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(তথ্য));

  যদি(!ctx->qconn) {
    ফলাফল = cf_quiche_ctx_open(cf, তথ্য);
    যদি(ফলাফল)
      যাও out;
    ctx->started_at = ctx->q.last_op;
    ফলাফল = cf_flush_egress(cf, তথ্য);
    /* we do not expect to be able to recv anything yet */
    যাও out;
  }

  ফলাফল = cf_process_ingress(cf, তথ্য);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = cf_flush_egress(cf, তথ্য);
  যদি(ফলাফল)
    যাও out;

  যদি(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.last_op;
    CURL_TRC_CF(তথ্য, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    ফলাফল = cf_quiche_verify_peer(cf, তথ্য);
    যদি(!ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      যদি(!ctx->h3config) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_set_max_field_section_আকার(ctx->h3config,
                                                  MAX_HTTP_RESP_HEADER_আকার);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      যদি(!ctx->h3c) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
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
    ফলাফল = CURLE_WEIRD_SERVER_REPLY;
  }

out:
#ifdef CURLVERBOSE
  যদি(ফলাফল && ফলাফল != CURLE_AGAIN) {
    কাঠামো ip_quadruple ip;

    যদি(!Curl_cf_socket_peek(cf->পরবর্তী, তথ্য, ফাঁকা, ফাঁকা, &ip))
      infof(তথ্য, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_port, curl_easy_strerror(ফলাফল));
    অন্যথা
      infof(তথ্য, "connect failed: %s", curl_easy_strerror(ফলাফল));
  }
#endif
  ফেরত ফলাফল;
}

স্থির CURLcode cf_quiche_shutdown(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  যদি(cf->shutdown || !ctx || !ctx->qconn) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  *done = FALSE;
  যদি(!ctx->shutdown_started) {
    পূর্ণসংখ্যা err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(তথ্য));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, ফাঁকা, 0);
    যদি(err) {
      CURL_TRC_CF(তথ্য, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
  }

  যদি(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(তথ্য, cf, "shutdown, flushing sendbuf");
    ফলাফল = cf_flush_egress(cf, তথ্য);
    যদি(ফলাফল)
      যাও out;
  }

  যদি(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(তথ্য, cf, "shutdown completely sent off, done");
    *done = TRUE;
  }
  অন্যথা {
    CURL_TRC_CF(তথ্য, cf, "shutdown sending blocked");
  }

out:
  ফেরত ফলাফল;
}

স্থির শূন্য cf_quiche_destroy(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  (শূন্য)তথ্য;
  যদি(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = ফাঁকা;
  }
}

স্থির CURLcode cf_quiche_query(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;

  নির্বাচন(query) {
  ক্ষেত্র CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    যদি(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_বাম_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > INT_MAX) ? INT_MAX : (পূর্ণসংখ্যা)max_streams;
    CURL_TRC_CF(তথ্য, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    ফেরত CURLE_OK;
  }
  ক্ষেত্র CF_QUERY_CONNECT_REPLY_MS:
    যদি(ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (পূর্ণসংখ্যা)ms : INT_MAX;
    }
    অন্যথা
      *pres1 = -1;
    ফেরত CURLE_OK;
  ক্ষেত্র CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    ফেরত CURLE_OK;
  ক্ষেত্র CF_QUERY_SSL_INFO:
  ক্ষেত্র CF_QUERY_SSL_CTX_INFO: {
    কাঠামো curl_tlssessioninfo *info = pres2;
    যদি(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      ফেরত CURLE_OK;
    বিরতি;
  }
  ক্ষেত্র CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুবক অক্ষর **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ফাঁকা;
    ফেরত CURLE_OK;
  }
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির bool cf_quiche_conn_is_alive(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    bool *প্রবেশ_pending)
{
  কাঠামো cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *প্রবেশ_pending = FALSE;
  যদি(!ctx->qconn)
    ফেরত FALSE;

  যদি(quiche_conn_is_closed(ctx->qconn)) {
    যদি(quiche_conn_is_timed_out(ctx->qconn))
      CURL_TRC_CF(তথ্য, cf, "connection was closed due to idle timeout");
    অন্যথা
      CURL_TRC_CF(তথ্য, cf, "connection is closed");
    ফেরত FALSE;
  }

  যদি(!cf->পরবর্তী || !cf->পরবর্তী->cft->is_alive(cf->পরবর্তী, তথ্য, প্রবেশ_pending))
    ফেরত FALSE;

  যদি(*প্রবেশ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *প্রবেশ_pending = FALSE;
    যদি(cf_process_ingress(cf, তথ্য))
      alive = FALSE;
    অন্যথা {
      alive = TRUE;
    }
  }

  ফেরত alive;
}

কাঠামো Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_quiche_destroy,
  cf_quiche_connect,
  cf_quiche_shutdown,
  cf_quiche_adjust_pollset,
  Curl_cf_def_তথ্য_pending,
  cf_quiche_send,
  cf_quiche_recv,
  cf_quiche_cntrl,
  cf_quiche_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_quiche_query,
};

CURLcode Curl_cf_quiche_create(কাঠামো Curl_cfilter **pcf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_peer *origin,
                               কাঠামো Curl_peer *peer,
                               কাঠামো connectdata *conn,
                               কাঠামো Curl_sockaddr_ex *addr)
{
  কাঠামো cf_quiche_ctx *ctx = ফাঁকা;
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  CURLcode ফলাফল;

  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  ফলাফল = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  যদি(!ফলাফল)
    ফলাফল = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  যদি(ফলাফল)
    যাও out;
  cf->conn = conn;

  ফলাফল = Curl_cf_udp_create(&cf->পরবর্তী, তথ্য, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ফাঁকা, TRNSPRT_QUIC);
  যদি(ফলাফল)
    যাও out;
  cf->পরবর্তী->conn = cf->conn;
  cf->পরবর্তী->sockindex = cf->sockindex;

out:
  *pcf = (!ফলাফল) ? cf : ফাঁকা;
  যদি(ফলাফল) {
    যদি(cf)
      Curl_conn_cf_discard_chain(&cf, তথ্য);
    অন্যথা যদি(ctx)
      cf_quiche_ctx_free(ctx);
  }

  ফেরত ফলাফল;
}

CURLcode Curl_cf_quiche_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                     কাঠামো Curl_peer *origin,
                                     কাঠামো Curl_peer *peer,
                                     কাঠামো Curl_cfilter **pcf)
{
  কাঠামো cf_quiche_ctx *ctx = ফাঁকা;
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  CURLcode ফলাফল;

  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  ফলাফল = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  যদি(!ফলাফল)
    ফলাফল = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  যদি(ফলাফল)
    যাও out;
  Curl_conn_cf_ঢোকাও_after(cf_at, cf);

out:
  যদি(ফলাফল) {
    curlx_safefree(cf);
    যদি(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  ফেরত ফলাফল;
}

#endif
