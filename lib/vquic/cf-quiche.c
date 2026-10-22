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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#समावेश <quiche.h>
#समावेश <openssl/err.h>
#समावेश <openssl/ssl.h>

#समावेश "bufq.h"
#समावेश "u32_ptrset.h"
#समावेश "urldata.h"
#समावेश "cfilters.h"
#समावेश "cf-socket.h"
#समावेश "curl_trc.h"
#समावेश "rand.h"
#समावेश "multiif.h"
#समावेश "connect.h"
#समावेश "progress.h"
#समावेश "select.h"
#समावेश "http.h"
#समावेश "http1.h"
#समावेश "sockaddr.h"
#समावेश "vdns/cf-dns.h"
#समावेश "vquic/vquic.h"
#समावेश "vquic/vquic_int.h"
#समावेश "vquic/vquic-tls.h"
#समावेश "vquic/cf-quiche.h"
#समावेश "transfer.h"
#समावेश "url.h"
#समावेश "bufref.h"
#समावेश "vtls/openssl.h"
#समावेश "vtls/vtls.h"

/* HTTP/3 error values defined in RFC 9114, ch. 8.1 */
#घोषणा CURL_H3_NO_ERROR  0x0100

#घोषणा MAX_PKT_BURST          10

#घोषणा QUIC_MAX_STREAMS       100

#घोषणा H3_STREAM_WINDOW_आकार  (1024 * 128)
#घोषणा H3_STREAM_CHUNK_आकार   (1024 * 16)
/* Receive and Send max number of chunks follows from the
 * chunk size and window size */
#घोषणा H3_STREAM_RECV_CHUNKS \
  (H3_STREAM_WINDOW_आकार / H3_STREAM_CHUNK_आकार)

/*
 * Store quiche version info in this buffer.
 */
व्योम Curl_quiche_ver(अक्षर *p, माप_प्रकार len)
{
  (व्योम)curl_msnprintf(p, len, "quiche/%s", quiche_version());
}

काष्ठा cf_quiche_ctx {
  काष्ठा cf_quic_ctx q;
  काष्ठा ssl_peer ssl_peer;
  काष्ठा curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  काष्ठा curltime started_at;        /* time the current attempt started */
  काष्ठा curltime handshake_at;      /* time connect handshake finished */
  काष्ठा u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  काष्ठा dynbuf h1hdr;               /* temp buffer for header construction */
  काष्ठा bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t तथ्य_recvd;
  BIT(initialized);
  BIT(goaway);                       /* got GOAWAY from server */
  BIT(x509_store_setup);             /* if x509 store has been set up */
  BIT(shutdown_started);             /* queued shutdown packets */
  BIT(stats_reported);               /* connect statistics reported */
};

#अगर_घोषित DEBUG_QUICHE
/* initialize debug log callback only once */
अटल पूर्णांक debug_log_init = 0;
अटल व्योम quiche_debug_log(स्थिर अक्षर *पंक्ति, व्योम *argp)
{
  (व्योम)argp;
  curl_mfprintf(मानक_त्रुटि, "%s\n", पंक्ति);
}
#पूर्ण_अगर

अटल व्योम h3_stream_set_free(uint32_t id, व्योम *stream);

अटल CURLcode cf_quiche_ctx_init(काष्ठा cf_quiche_ctx *ctx,
                                   काष्ठा Curl_peer *origin,
                                   काष्ठा Curl_peer *peer,
                                   काष्ठा ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#अगर_घोषित DEBUG_QUICHE
  अगर(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, शून्य);
    debug_log_init = 1;
  }
#पूर्ण_अगर
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  Curl_bufq_init2(&ctx->writebuf, H3_STREAM_CHUNK_आकार, H3_STREAM_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  ctx->तथ्य_recvd = 0;
  ctx->initialized = TRUE;
  वापस Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

अटल व्योम cf_quiche_ctx_free(काष्ठा cf_quiche_ctx *ctx)
{
  अगर(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = शून्य;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

अटल व्योम cf_quiche_ctx_close(काष्ठा cf_quiche_ctx *ctx)
{
  अगर(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = शून्य;
  }
  अगर(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = शून्य;
  }
  अगर(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = शून्य;
  }
  अगर(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = शून्य;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

अटल CURLcode cf_flush_egress(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य);

/**
 * All about the H3 internals of a stream
 */
काष्ठा h3_stream_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  काष्ठा h1_req_parser h1; /* h1 request parsing */
  uint64_t error3;         /* HTTP/3 stream error code */
  int32_t status_code;     /* HTTP status code */
  CURLcode xfer_परिणाम;    /* result from cf_quiche_write_(hd/body) */
  BIT(opened);             /* TRUE after stream has been opened */
  BIT(closed);             /* TRUE on stream close */
  BIT(reset);              /* TRUE on stream reset */
  BIT(send_closed);        /* stream is locally closed */
  BIT(resp_hds_complete);  /* final response has been received */
  BIT(resp_got_header);    /* TRUE when h3 stream has recvd some HEADER */
  BIT(quic_flow_blocked);  /* stream is blocked by QUIC flow control */
};

अटल व्योम h3_stream_ctx_free(काष्ठा h3_stream_ctx *stream)
{
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

अटल व्योम h3_stream_set_free(uint32_t id, व्योम *stream)
{
  (व्योम)id;
  DEBUGASSERT(stream);
  h3_stream_ctx_free((काष्ठा h3_stream_ctx *)stream);
}

प्रकार bool cf_quiche_svisit(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *sdata,
                              काष्ठा h3_stream_ctx *stream,
                              व्योम *user_तथ्य);

काष्ठा cf_quiche_visit_ctx {
  काष्ठा Curl_cfilter *cf;
  काष्ठा Curl_multi *multi;
  cf_quiche_svisit *cb;
  व्योम *user_तथ्य;
};

अटल bool cf_quiche_stream_do(uint32_t mid, व्योम *val, व्योम *user_तथ्य)
{
  काष्ठा cf_quiche_visit_ctx *vctx = user_तथ्य;
  काष्ठा h3_stream_ctx *stream = val;
  काष्ठा Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  अगर(sdata)
    वापस vctx->cb(vctx->cf, sdata, stream, vctx->user_तथ्य);
  वापस TRUE;
}

अटल व्योम cf_quiche_for_all_streams(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      व्योम *user_तथ्य)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  काष्ठा cf_quiche_visit_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_तथ्य = user_तथ्य;
  Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_stream_do, &vctx);
}

अटल bool cf_quiche_do_resume(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *sdata,
                                काष्ठा h3_stream_ctx *stream,
                                व्योम *user_तथ्य)
{
  (व्योम)user_तथ्य;
  अगर(stream->quic_flow_blocked) {
    stream->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", stream->id);
  }
  वापस TRUE;
}

अटल bool cf_quiche_do_expire(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *sdata,
                                काष्ठा h3_stream_ctx *stream,
                                व्योम *user_तथ्य)
{
  (व्योम)user_तथ्य;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  stream->xfer_परिणाम = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  वापस TRUE;
}

अटल CURLcode h3_तथ्य_setup(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);

  अगर(stream)
    वापस CURLE_OK;

  stream = curlx_calloc(1, माप(*stream));
  अगर(!stream)
    वापस CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_पंक्ति_LEN);

  अगर(Curl_u32_ptrset_set(&ctx->streams, तथ्य->mid, stream)) {
    h3_stream_ctx_free(stream);
    वापस CURLE_OUT_OF_MEMORY;
  }

  वापस CURLE_OK;
}

अटल व्योम cf_quiche_stream_close(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा h3_stream_ctx *stream)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  CURLcode परिणाम;

  अगर(ctx->qconn && !stream->closed) {
    quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    अगर(!stream->send_closed) {
      quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      stream->send_closed = TRUE;
    }
    stream->closed = TRUE;
    परिणाम = cf_flush_egress(cf, तथ्य);
    अगर(परिणाम)
      CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  stream->id, (पूर्णांक)परिणाम);
  }
}

अटल व्योम h3_तथ्य_done(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);

  (व्योम)cf;
  अगर(stream) {
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] easy handle is done", stream->id);
    cf_quiche_stream_close(cf, तथ्य, stream);
    Curl_u32_ptrset_unset(&ctx->streams, तथ्य->mid);
  }
}

अटल व्योम cf_quiche_expire_conn_closed(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य)
{
  DEBUGASSERT(तथ्य->multi);
  CURL_TRC_CF(तथ्य, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, तथ्य->multi, cf_quiche_do_expire, शून्य);
}

अटल व्योम cf_quiche_write_hd(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा h3_stream_ctx *stream,
                               स्थिर अक्षर *buf, माप_प्रकार blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  अगर(!stream->xfer_परिणाम) {
    stream->xfer_परिणाम = Curl_xfer_write_resp_hd(तथ्य, buf, blen, eos);
    अगर(stream->xfer_परिणाम)
      CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (पूर्णांक)stream->xfer_परिणाम,
                  blen);
  }
}

काष्ठा cb_ctx {
  काष्ठा Curl_cfilter *cf;
  काष्ठा Curl_easy *तथ्य;
  काष्ठा h3_stream_ctx *stream;
};

अटल bool is_valid_h3_header(स्थिर uint8_t *hdr, माप_प्रकार hlen)
{
  जबतक(hlen--) {
    चयन(*hdr++) {
    हाल '\n':
    हाल '\r':
    हाल '\0':
      वापस FALSE;
    }
  }
  वापस TRUE;
}

अटल पूर्णांक cb_each_header(uint8_t *name, माप_प्रकार name_len,
                          uint8_t *मूल्य, माप_प्रकार मूल्य_len,
                          व्योम *argp)
{
  काष्ठा cb_ctx *x = argp;
  काष्ठा Curl_cfilter *cf = x->cf;
  काष्ठा Curl_easy *तथ्य = x->तथ्य;
  काष्ठा h3_stream_ctx *stream = x->stream;
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  अगर(!stream || stream->xfer_परिणाम)
    वापस 1; /* abort iteration */

  अगर((name_len == 7) && !म_भेदन(HTTP_PSEUDO_STATUS, (अक्षर *)name, 7) &&
     is_valid_h3_header(मूल्य, मूल्य_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    परिणाम = Curl_http_decode_status(&stream->status_code,
                                     (स्थिर अक्षर *)मूल्य, मूल्य_len);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->h1hdr, (स्थिर अक्षर *)मूल्य, मूल्य_len);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    अगर(!परिणाम)
      cf_quiche_write_hd(cf, तथ्य, stream, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] status: %s",
                stream->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  अन्यथा {
    अगर(is_valid_h3_header(मूल्य, मूल्य_len) &&
       is_valid_h3_header(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  stream->id, (पूर्णांक)name_len, name,
                  (पूर्णांक)मूल्य_len, मूल्य);
      curlx_dyn_reset(&ctx->h1hdr);
      परिणाम = curlx_dyn_addn(&ctx->h1hdr, (स्थिर अक्षर *)name, name_len);
      अगर(!परिणाम)
        परिणाम = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      अगर(!परिणाम)
        परिणाम = curlx_dyn_addn(&ctx->h1hdr, (स्थिर अक्षर *)मूल्य, मूल्य_len);
      अगर(!परिणाम)
        परिणाम = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      अगर(!परिणाम)
        cf_quiche_write_hd(cf, तथ्य, stream, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    अन्यथा
      CURL_TRC_CF(x->तथ्य, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  stream->id, मूल्य_len + name_len);
  }

  अगर(परिणाम) {
    CURL_TRC_CF(x->तथ्य, x->cf, "[%" PRIu64 "] on header error %d",
                stream->id, (पूर्णांक)परिणाम);
    अगर(!stream->xfer_परिणाम)
      stream->xfer_परिणाम = परिणाम;
  }
  वापस stream->xfer_परिणाम ? 1 : 0;
}

अटल CURLcode stream_resp_read(व्योम *reader_ctx,
                                 अचिन्हित अक्षर *buf, माप_प्रकार len,
                                 माप_प्रकार *pnread)
{
  काष्ठा cb_ctx *x = reader_ctx;
  काष्ठा cf_quiche_ctx *ctx = x->cf->ctx;
  काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, x->तथ्य);
  ssize_t nread;

  *pnread = 0;
  अगर(!stream)
    वापस CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, stream->id, buf, len);
  अगर(!curlx_sztouz(nread, pnread))
    वापस CURLE_AGAIN;
  वापस CURLE_OK;
}

अटल व्योम cf_quiche_flush_body(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा h3_stream_ctx *stream)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  स्थिर uint8_t *buf;
  माप_प्रकार blen;

  जबतक(stream && !stream->xfer_परिणाम) {
    अगर(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      stream->xfer_परिणाम = Curl_xfer_write_resp(
        तथ्य, (स्थिर अक्षर *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      अगर(stream->xfer_परिणाम) {
        CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", stream->id, (पूर्णांक)stream->xfer_परिणाम, blen);
      }
    }
    अन्यथा
      अवरोध;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

अटल व्योम cf_quiche_recv_body(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                काष्ठा h3_stream_ctx *stream)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  माप_प्रकार nread;
  काष्ठा cb_ctx cb_ctx;
  CURLcode परिणाम = CURLE_OK;

  अगर(!stream)
    वापस;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.तथ्य = तथ्य;
  cb_ctx.stream = stream;
  Curl_bufq_reset(&ctx->writebuf);
  जबतक(!परिणाम) {
    परिणाम = Curl_bufq_slurp(&ctx->writebuf,
                             stream_resp_read, &cb_ctx, &nread);
    अगर(!परिणाम)
      cf_quiche_flush_body(cf, तथ्य, stream);
    अन्यथा अगर(परिणाम == CURLE_AGAIN)
      अवरोध;
    अन्यथा अगर(परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] recv_body error %d",
                  stream->id, (पूर्णांक)परिणाम);
      failf(तथ्य, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            stream->id, (पूर्णांक)परिणाम);
      stream->closed = TRUE;
      stream->reset = TRUE;
      stream->send_closed = TRUE;
      अगर(!stream->xfer_परिणाम)
        stream->xfer_परिणाम = परिणाम;
    }
  }
  cf_quiche_flush_body(cf, तथ्य, stream);
}

अटल व्योम cf_quiche_process_ev(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा h3_stream_ctx *stream,
                                 quiche_h3_event *ev)
{
  क्रमागत quiche_h3_event_type type;

  अगर(!stream)
    वापस;

  type = quiche_h3_event_type(ev);

  चयन(type) {
  हाल QUICHE_H3_EVENT_HEADERS: {
    काष्ठा cb_ctx cb_ctx;
    stream->resp_got_header = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.तथ्य = तथ्य;
    cb_ctx.stream = stream;
    quiche_h3_event_for_each_header(ev, cb_each_header, &cb_ctx);
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] <- [HEADERS]", stream->id);
    Curl_multi_mark_dirty(तथ्य);
    अवरोध;
  }
  हाल QUICHE_H3_EVENT_तथ्य:
    अगर(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, तथ्य, stream, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, तथ्य, stream);
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] <- [DATA]", stream->id);
    Curl_multi_mark_dirty(तथ्य);
    अवरोध;

  हाल QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] RESET", stream->id);
    stream->closed = TRUE;
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    Curl_multi_mark_dirty(तथ्य);
    अवरोध;

  हाल QUICHE_H3_EVENT_FINISHED:
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] CLOSED", stream->id);
    अगर(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, तथ्य, stream, "\r\n", 2, TRUE);
    }
    stream->closed = TRUE;
    Curl_multi_mark_dirty(तथ्य);
    अवरोध;

  हाल QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] <- [GOAWAY]", stream->id);
    अवरोध;

  शेष:
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] recv, unhandled event %d",
                stream->id, (पूर्णांक)type);
    अवरोध;
  }
}

काष्ठा cf_quiche_disp_ctx {
  uint64_t stream_id;
  काष्ठा Curl_cfilter *cf;
  काष्ठा Curl_multi *multi;
  quiche_h3_event *ev;
};

अटल bool cf_quiche_disp_event(uint32_t mid, व्योम *val, व्योम *user_तथ्य)
{
  काष्ठा cf_quiche_disp_ctx *dctx = user_तथ्य;
  काष्ठा h3_stream_ctx *stream = val;

  अगर(stream->id == dctx->stream_id) {
    काष्ठा Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    अगर(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, stream, dctx->ev);
    वापस FALSE; /* stop iterating */
  }
  वापस TRUE;
}

अटल CURLcode cf_poll_events(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = शून्य;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  जबतक(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    अगर(rv == QUICHE_H3_ERR_DONE) {
      अवरोध;
    }
    अन्यथा अगर(rv < 0) {
      CURL_TRC_CF(तथ्य, cf, "error poll: %" PRId64, rv);
      वापस CURLE_HTTP3;
    }
    अन्यथा {
      stream = H3_STREAM_CTX(ctx, तथ्य);
      अगर(stream && stream->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, तथ्य, stream, ev);
        quiche_h3_event_free(ev);
        अगर(stream->xfer_परिणाम)
          वापस stream->xfer_परिणाम;
      }
      अन्यथा {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        काष्ठा cf_quiche_disp_ctx dctx;
        dctx.stream_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = तथ्य->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  वापस CURLE_OK;
}

काष्ठा recv_ctx {
  काष्ठा Curl_cfilter *cf;
  काष्ठा Curl_easy *तथ्य;
  पूर्णांक pkts;
};

अटल CURLcode cf_quiche_recv_pkts(स्थिर अचिन्हित अक्षर *buf, माप_प्रकार buflen,
                                    माप_प्रकार gso_आकार,
                                    काष्ठा sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    व्योम *userp)
{
  काष्ठा recv_ctx *r = userp;
  काष्ठा cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  माप_प्रकार pktlen, offset, nread;
  ssize_t rv;

  (व्योम)ecn;

  recv_info.to = (काष्ठा sockaddr *)&ctx->q.local_addr;
  recv_info.to_len = ctx->q.local_addrlen;
  recv_info.from = (काष्ठा sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  क्रम(offset = 0; offset < buflen; offset += gso_आकार) {
    pktlen = ((offset + gso_आकार) <= buflen) ? gso_आकार : (buflen - offset);
    rv = quiche_conn_recv(ctx->qconn,
                          (अचिन्हित अक्षर *)CURL_UNCONST(buf + offset),
                          pktlen, &recv_info);
    अगर(!curlx_sztouz(rv, &nread)) {
      अगर(QUICHE_ERR_DONE == rv) {
        अगर(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->तथ्य, r->cf, "ingress, connection is draining");
          वापस CURLE_RECV_ERROR;
        }
        अगर(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->तथ्य, r->cf, "ingress, connection is closed");
          वापस CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->तथ्य, r->cf, "ingress, quiche is DONE");
        वापस CURLE_OK;
      }
      अन्यथा अगर(QUICHE_ERR_TLS_FAIL == rv) {
        दीर्घ verify_ok = SSL_get_verify_परिणाम(ctx->tls.ossl.ssl);
        अगर(verify_ok != X509_V_OK) {
          failf(r->तथ्य, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          वापस CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->तथ्य, "ingress, quiche reports TLS fail");
        वापस CURLE_RECV_ERROR;
      }
      अन्यथा {
        failf(r->तथ्य, "quiche reports error %zd on receive", rv);
        वापस CURLE_RECV_ERROR;
      }
    }
    अन्यथा अगर(nread < pktlen) {
      CURL_TRC_CF(r->तथ्य, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  वापस CURLE_OK;
}

अटल CURLcode cf_process_ingress(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  काष्ठा recv_ctx rctx;
  CURLcode परिणाम;

  DEBUGASSERT(ctx->qconn);
  परिणाम = Curl_vquic_tls_before_recv(&ctx->tls, cf, तथ्य);
  अगर(परिणाम)
    वापस परिणाम;

  rctx.cf = cf;
  rctx.तथ्य = तथ्य;
  rctx.pkts = 0;

  परिणाम = Curl_vquic_recv_packets(cf, तथ्य, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  अगर(परिणाम)
    वापस परिणाम;

  अगर(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(तथ्य->multi);
    cf_quiche_for_all_streams(cf, तथ्य->multi, cf_quiche_do_resume, शून्य);
  }
  वापस cf_poll_events(cf, तथ्य);
}

काष्ठा read_ctx {
  काष्ठा Curl_cfilter *cf;
  काष्ठा Curl_easy *तथ्य;
  quiche_send_info send_info;
};

अटल CURLcode read_pkt_to_send(व्योम *userp,
                                 अचिन्हित अक्षर *buf, माप_प्रकार buflen,
                                 माप_प्रकार *pnread)
{
  काष्ठा read_ctx *x = userp;
  काष्ठा cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  अगर(rv == QUICHE_ERR_DONE)
    वापस CURLE_AGAIN;

  अगर(!curlx_sztouz(rv, pnread)) {
    failf(x->तथ्य, "quiche_conn_send returned %zd", rv);
    वापस CURLE_SEND_ERROR;
  }
  वापस CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
अटल CURLcode cf_flush_egress(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  माप_प्रकार nread;
  CURLcode परिणाम;
  int64_t expiry_ns;
  int64_t timeout_ns;
  काष्ठा read_ctx readx;
  माप_प्रकार pkt_गिनती, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  अगर(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    अगर(quiche_conn_is_closed(ctx->qconn)) {
      अगर(quiche_conn_is_timed_out(ctx->qconn))
        failf(तथ्य, "connection closed by idle timeout");
      अन्यथा
        failf(तथ्य, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, तथ्य);
      वापस CURLE_SEND_ERROR;
    }
  }

  परिणाम = Curl_vquic_flush(cf, तथ्य, &ctx->q);
  अगर(परिणाम) {
    अगर(परिणाम == CURLE_AGAIN) {
      Curl_expire(तथ्य, 1, EXPIRE_QUIC);
      वापस CURLE_OK;
    }
    वापस परिणाम;
  }

  readx.cf = cf;
  readx.तथ्य = तथ्य;
  स_रखो(&readx.send_info, 0, माप(readx.send_info));
  pkt_गिनती = 0;
  gsolen = quiche_conn_max_send_udp_payload_आकार(ctx->qconn);
  क्रम(;;) {
    /* add the next packet to send, if any, to our buffer */
    परिणाम = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_to_send, &readx, &nread);
    अगर(परिणाम) {
      अगर(परिणाम != CURLE_AGAIN)
        वापस परिणाम;
      /* Nothing more to add, flush and leave */
      परिणाम = Curl_vquic_send(cf, तथ्य, &ctx->q, gsolen);
      अगर(परिणाम) {
        अगर(परिणाम == CURLE_AGAIN) {
          Curl_expire(तथ्य, 1, EXPIRE_QUIC);
          वापस CURLE_OK;
        }
        वापस परिणाम;
      }
      जाओ out;
    }

    ++pkt_गिनती;
    अगर(nread < gsolen || pkt_गिनती >= MAX_PKT_BURST) {
      परिणाम = Curl_vquic_send(cf, तथ्य, &ctx->q, gsolen);
      अगर(परिणाम) {
        अगर(परिणाम == CURLE_AGAIN) {
          Curl_expire(तथ्य, 1, EXPIRE_QUIC);
          वापस CURLE_OK;
        }
        जाओ out;
      }
      pkt_गिनती = 0;
    }
  }

out:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  अगर(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(तथ्य, (timeout_ns / 1000000), EXPIRE_QUIC);
  वापस परिणाम;
}

अटल CURLcode recv_closed_stream(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   माप_प्रकार *pnread)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(stream);
  *pnread = 0;
  अगर(stream->reset) {
    अगर(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(तथ्य, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      तथ्य->state.refused_stream = TRUE;
      वापस CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    अन्यथा अगर(stream->resp_hds_complete && तथ्य->req.no_body) {
        CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        वापस CURLE_OK;
    }
    failf(तथ्य, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    परिणाम = तथ्य->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                stream->id, (पूर्णांक)परिणाम);
  }
  अन्यथा अगर(!stream->resp_got_header) {
    failf(तथ्य, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    परिणाम = CURLE_HTTP3;
  }
  वापस परिणाम;
}

अटल CURLcode cf_quiche_recv(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                               अक्षर *buf, माप_प्रकार blen, माप_प्रकार *pnread)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);
  CURLcode परिणाम = CURLE_OK;

  *pnread = 0;
  (व्योम)buf;
  (व्योम)blen;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(तथ्य));

  अगर(!stream)
    वापस CURLE_RECV_ERROR;

  परिणाम = cf_process_ingress(cf, तथ्य);
  अगर(परिणाम) {
    CURL_TRC_CF(तथ्य, cf, "cf_recv, error on ingress");
    जाओ out;
  }

  अगर(stream->xfer_परिणाम) {
    cf_quiche_stream_close(cf, तथ्य, stream);
    परिणाम = stream->xfer_परिणाम;
    जाओ out;
  }
  अन्यथा अगर(stream->closed)
    परिणाम = recv_closed_stream(cf, तथ्य, pnread);
  अन्यथा अगर(quiche_conn_is_draining(ctx->qconn)) {
    failf(तथ्य, "QUIC connection is draining");
    परिणाम = CURLE_HTTP3;
  }
  अन्यथा
    परिणाम = CURLE_AGAIN;

out:
  परिणाम = Curl_1st_fatal(परिणाम, cf_flush_egress(cf, तथ्य));
  अगर(*pnread > 0)
    ctx->तथ्य_recvd += *pnread;
  CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, stream->id, blen, (पूर्णांक)परिणाम, *pnread,
              ctx->तथ्य_recvd);
  वापस परिणाम;
}

अटल CURLcode cf_quiche_send_body(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा h3_stream_ctx *stream,
                                    स्थिर uint8_t *buf, माप_प्रकार len, bool eos,
                                    माप_प्रकार *pnwritten)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, stream->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  अगर(rv == QUICHE_H3_ERR_DONE || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    अगर(!quiche_conn_stream_writable(ctx->qconn, stream->id, len)) {
      CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", stream->id, len);
      stream->quic_flow_blocked = TRUE;
    }
    वापस CURLE_AGAIN;
  }
  अन्यथा अगर(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_STREAM_STATE) {
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", stream->id, len);
    वापस CURLE_HTTP3;
  }
  अन्यथा अगर(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_आकार) {
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                stream->id, len);
    वापस CURLE_SEND_ERROR;
  }
  अन्यथा अगर(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                stream->id, len, rv);
    वापस CURLE_SEND_ERROR;
  }
  अन्यथा {
    अगर(eos && (len == *pnwritten))
      stream->send_closed = TRUE;
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                stream->id, len, stream->send_closed, *pnwritten);
    वापस CURLE_OK;
  }
}

अटल CURLcode h3_open_stream(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               स्थिर uint8_t *buf, माप_प्रकार blen, bool eos,
                               माप_प्रकार *pnwritten)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);
  माप_प्रकार nheader, i;
  int64_t rv;
  काष्ठा dynhds h2_headers;
  quiche_h3_header *nva = शून्य;
  CURLcode परिणाम = CURLE_OK;

  *pnwritten = 0;
  अगर(!stream) {
    परिणाम = h3_तथ्य_setup(cf, तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
    stream = H3_STREAM_CTX(ctx, तथ्य);
    DEBUGASSERT(stream);
  }

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(stream);

  परिणाम = Curl_h1_req_parse_read(&stream->h1, buf, blen, शून्य,
    !तथ्य->state.http_ignorecustom ?
    CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST) : शून्य,
    0, pnwritten);
  अगर(परिणाम)
    जाओ out;
  अगर(!stream->h1.done) {
    /* need more data */
    जाओ out;
  }
  DEBUGASSERT(stream->h1.req);

  परिणाम = Curl_http_req_to_h2(&h2_headers, stream->h1.req, तथ्य);
  अगर(परिणाम)
    जाओ out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_गिनती(&h2_headers);
  nva = curlx_malloc(माप(quiche_h3_header) * nheader);
  अगर(!nva) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  क्रम(i = 0; i < nheader; ++i) {
    काष्ठा dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (अचिन्हित अक्षर *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].मूल्य = (अचिन्हित अक्षर *)e->मूल्य;
    nva[i].मूल्य_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  अगर(eos && !blen)
    stream->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              stream->send_closed);
  CURL_TRC_CF(तथ्य, cf, "quiche_send_request() -> %" PRId64, rv);
  अगर(rv < 0) {
    अगर(QUICHE_H3_ERR_STREAM_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] blocked", stream->id);
      stream->quic_flow_blocked = TRUE;
      परिणाम = CURLE_AGAIN;
      जाओ out;
    }
    अन्यथा {
      CURL_TRC_CF(तथ्य, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&तथ्य->state.url), rv);
    }
    परिणाम = CURLE_SEND_ERROR;
    जाओ out;
  }

  DEBUGASSERT(!stream->opened);
  stream->id = (uint64_t)rv;
  stream->opened = TRUE;
  stream->closed = FALSE;
  stream->reset = FALSE;

  अगर(Curl_trc_is_verbose(तथ्य)) {
    infof(तथ्य, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&तथ्य->state.url));
    क्रम(i = 0; i < nheader; ++i) {
      infof(तथ्य, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", stream->id,
            (पूर्णांक)nva[i].name_len, nva[i].name,
            (पूर्णांक)nva[i].मूल्य_len, nva[i].मूल्य);
    }
  }

  अगर(blen) {  /* after the headers, there was request BODY data */
    माप_प्रकार nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, तथ्य, stream, buf, blen, eos, &nwritten);
    अगर(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      परिणाम = r2;
    }
    अन्यथा अगर(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  वापस परिणाम;
}

अटल CURLcode cf_quiche_send(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                               स्थिर uint8_t *buf, माप_प्रकार len, bool eos,
                               माप_प्रकार *pnwritten)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);
  CURLcode परिणाम;

  *pnwritten = 0;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(तथ्य));

  परिणाम = cf_process_ingress(cf, तथ्य);
  अगर(परिणाम)
    जाओ out;

  अगर(!stream || !stream->opened) {
    परिणाम = h3_open_stream(cf, तथ्य, buf, len, eos, pnwritten);
    अगर(परिणाम)
      जाओ out;
    stream = H3_STREAM_CTX(ctx, तथ्य);
  }
  अन्यथा अगर(stream->xfer_परिणाम) {
    cf_quiche_stream_close(cf, तथ्य, stream);
    परिणाम = stream->xfer_परिणाम;
  }
  अन्यथा अगर(stream->closed) {
    अगर(stream->resp_hds_complete) {
      /* sending request body on a stream that has been closed by the
       * server. If the server has send us a final response, we should
       * silently discard the send data.
       * This happens for example on redirects where the server, instead
       * of reading the full request body closed the stream after
       * sending the 30x response.
       * This is sort of a race: had the transfer loop called recv first,
       * it would see the response and stop/discard sending on its own- */
      CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] discarding data "
                  "on closed stream with response", stream->id);
      परिणाम = CURLE_OK;
      *pnwritten = len;
      जाओ out;
    }
    CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    परिणाम = CURLE_HTTP3;
    जाओ out;
  }
  अन्यथा {
    परिणाम = cf_quiche_send_body(cf, तथ्य, stream, buf, len, eos, pnwritten);
  }

out:
  परिणाम = Curl_1st_fatal(परिणाम, cf_flush_egress(cf, तथ्य));

  CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : (uint64_t)~0, len,
              (पूर्णांक)परिणाम, *pnwritten);
  वापस परिणाम;
}

अटल bool stream_is_writable(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);

  वापस stream && (quiche_conn_stream_writable(
    ctx->qconn, stream->id, 1) > 0);
}

अटल CURLcode cf_quiche_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य,
                                         काष्ठा easy_pollset *ps)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode परिणाम = CURLE_OK;

  अगर(!ctx->qconn)
    वापस CURLE_OK;

  Curl_pollset_check(तथ्य, ps, ctx->q.sockfd, &want_recv, &want_send);
  अगर(want_recv || want_send) {
    काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);
    bool c_exhaust, s_exhaust;

    c_exhaust = FALSE; /* Have not found any call in quiche that tells
                          us if the connection itself is blocked */
    s_exhaust = want_send && stream && stream->opened &&
                (stream->quic_flow_blocked || !stream_is_writable(cf, तथ्य));
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    परिणाम = Curl_pollset_set(तथ्य, ps, ctx->q.sockfd, want_recv, want_send);
  }
  वापस परिणाम;
}

अटल CURLcode h3_तथ्य_pause(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              bool छोड़ो)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (व्योम)cf;
  अगर(!छोड़ो) {
    Curl_multi_mark_dirty(तथ्य);
  }
  वापस CURLE_OK;
}

अटल CURLcode cf_quiche_cntrl(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  (व्योम)arg1;
  (व्योम)arg2;
  चयन(event) {
  हाल CF_CTRL_तथ्य_SETUP:
    अवरोध;
  हाल CF_CTRL_तथ्य_PAUSE:
    परिणाम = h3_तथ्य_pause(cf, तथ्य, (arg1 != 0));
    अवरोध;
  हाल CF_CTRL_तथ्य_DONE:
    h3_तथ्य_done(cf, तथ्य);
    अवरोध;
  हाल CF_CTRL_तथ्य_DONE_SEND: {
    काष्ठा h3_stream_ctx *stream = H3_STREAM_CTX(ctx, तथ्य);
    अगर(stream && !stream->send_closed) {
      अचिन्हित अक्षर body[1];
      माप_प्रकार sent;

      stream->send_closed = TRUE;
      body[0] = 'X';
      परिणाम = cf_quiche_send(cf, तथ्य, body, 0, TRUE, &sent);
      CURL_TRC_CF(तथ्य, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  stream->id, (पूर्णांक)परिणाम, sent);
    }
    अवरोध;
  }
  हाल CF_CTRL_CONN_INFO_UPDATE:
    अगर(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    अवरोध;
  हाल CF_CTRL_REPORT_STATS:
    अगर(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrsTimeWas(तथ्य, TIMER_CONNECT, ctx->q.first_byte_at);
      Curl_pgrsTimeWas(तथ्य, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    अवरोध;
  शेष:
    अवरोध;
  }
  वापस परिणाम;
}

अटल CURLcode cf_quiche_ctx_open(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  पूर्णांक rv;
  CURLcode परिणाम;
  स्थिर काष्ठा Curl_sockaddr_ex *sockaddr;
  अटल स्थिर काष्ठा alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  परिणाम = Curl_vquic_ctx_init(तथ्य, &ctx->q);
  अगर(परिणाम)
    वापस परिणाम;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_VERSION);
  अगर(!ctx->cfg) {
    failf(तथ्य, "cannot create quiche config");
    वापस CURLE_FAILED_INIT;
  }
  quiche_config_enable_pacing(ctx->cfg, FALSE);
  quiche_config_set_initial_max_तथ्य(ctx->cfg, (1 * 1024 * 1024)
    /* (QUIC_MAX_STREAMS/2) * H3_STREAM_WINDOW_SIZE */);
  quiche_config_set_initial_max_streams_bidi(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_set_initial_max_streams_uni(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_set_initial_max_stream_तथ्य_bidi_local(ctx->cfg,
    H3_STREAM_WINDOW_आकार);
  quiche_config_set_initial_max_stream_तथ्य_bidi_remote(ctx->cfg,
    H3_STREAM_WINDOW_आकार);
  quiche_config_set_initial_max_stream_तथ्य_uni(ctx->cfg,
    H3_STREAM_WINDOW_आकार);
  quiche_config_set_disable_active_migration(ctx->cfg, TRUE);

  quiche_config_set_max_connection_window(ctx->cfg,
    10 * QUIC_MAX_STREAMS * H3_STREAM_WINDOW_आकार);
  quiche_config_set_max_stream_window(ctx->cfg, 10 * H3_STREAM_WINDOW_आकार);
  quiche_config_set_application_protos(ctx->cfg,
                      (uint8_t *)CURL_UNCONST(QUICHE_H3_APPLICATION_PROTOCOL),
                                 CURL_CSTRLEN(QUICHE_H3_APPLICATION_PROTOCOL));

  परिणाम = Curl_vquic_tls_init(&ctx->tls, cf, तथ्य, &ctx->ssl_peer,
                               &ALPN_SPEC_H3, शून्य, शून्य, cf, शून्य);
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = Curl_rand(तथ्य, ctx->scid, माप(ctx->scid));
  अगर(परिणाम)
    वापस परिणाम;

  अगर(Curl_cf_socket_peek(cf->अग्र, तथ्य, &ctx->q.sockfd, &sockaddr, शून्य))
    वापस CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = माप(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (काष्ठा sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  अगर(rv == -1)
    वापस CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((स्थिर uint8_t *)ctx->scid,
                                        माप(ctx->scid), शून्य, 0,
                                        (काष्ठा sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  अगर(!ctx->qconn) {
    failf(तथ्य, "cannot create quiche connection");
    वापस CURLE_OUT_OF_MEMORY;
  }

  /* Known to not work on Windows */
#अगर !defined(_WIN32) && defined(HAVE_QUICHE_CONN_SET_QLOG_FD)
  {
    पूर्णांक qfd;
    (व्योम)Curl_qlogdir(तथ्य, ctx->scid, माप(ctx->scid), &qfd);
    अगर(qfd != -1)
      quiche_conn_set_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#पूर्ण_अगर

  परिणाम = cf_flush_egress(cf, तथ्य);
  अगर(परिणाम)
    वापस परिणाम;

  {
    अचिन्हित अक्षर alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    अचिन्हित alpn_len, offset = 0;

    /* Replace each ALPN length prefix by a comma. */
    जबतक(offset < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[offset];
      alpn_protocols[offset] = ',';
      offset += 1 + alpn_len;
    }

    CURL_TRC_CF(तथ्य, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  वापस CURLE_OK;
}

अटल CURLcode cf_quiche_verify_peer(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  वापस Curl_vquic_tls_verify_peer(&ctx->tls, cf, तथ्य, &ctx->ssl_peer);
}

अटल CURLcode cf_quiche_connect(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  bool *done)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  /* Connect the UDP filter first */
  अगर(!cf->अग्र->connected) {
    परिणाम = Curl_conn_cf_connect(cf->अग्र, तथ्य, done);
    अगर(परिणाम || !*done)
      वापस परिणाम;
  }

  *done = FALSE;
  अगर(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(तथ्य, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(तथ्य, cf, "need HTTPS-RR, delaying connect");
    वापस CURLE_OK;
  }

  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(तथ्य));

  अगर(!ctx->qconn) {
    परिणाम = cf_quiche_ctx_open(cf, तथ्य);
    अगर(परिणाम)
      जाओ out;
    ctx->started_at = ctx->q.last_op;
    परिणाम = cf_flush_egress(cf, तथ्य);
    /* we do not expect to be able to recv anything yet */
    जाओ out;
  }

  परिणाम = cf_process_ingress(cf, तथ्य);
  अगर(परिणाम)
    जाओ out;

  परिणाम = cf_flush_egress(cf, तथ्य);
  अगर(परिणाम)
    जाओ out;

  अगर(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.last_op;
    CURL_TRC_CF(तथ्य, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    परिणाम = cf_quiche_verify_peer(cf, तथ्य);
    अगर(!परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      अगर(!ctx->h3config) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_set_max_field_section_आकार(ctx->h3config,
                                                  MAX_HTTP_RESP_HEADER_आकार);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      अगर(!ctx->h3c) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
      cf->connected = TRUE;
      *done = TRUE;
    }
  }
  अन्यथा अगर(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    परिणाम = CURLE_WEIRD_SERVER_REPLY;
  }

out:
#अगर_घोषित CURLVERBOSE
  अगर(परिणाम && परिणाम != CURLE_AGAIN) {
    काष्ठा ip_quadruple ip;

    अगर(!Curl_cf_socket_peek(cf->अग्र, तथ्य, शून्य, शून्य, &ip))
      infof(तथ्य, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_port, curl_easy_strerror(परिणाम));
    अन्यथा
      infof(तथ्य, "connect failed: %s", curl_easy_strerror(परिणाम));
  }
#पूर्ण_अगर
  वापस परिणाम;
}

अटल CURLcode cf_quiche_shutdown(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  अगर(cf->shutdown || !ctx || !ctx->qconn) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  *done = FALSE;
  अगर(!ctx->shutdown_started) {
    पूर्णांक err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(तथ्य));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, शून्य, 0);
    अगर(err) {
      CURL_TRC_CF(तथ्य, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
  }

  अगर(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(तथ्य, cf, "shutdown, flushing sendbuf");
    परिणाम = cf_flush_egress(cf, तथ्य);
    अगर(परिणाम)
      जाओ out;
  }

  अगर(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(तथ्य, cf, "shutdown completely sent off, done");
    *done = TRUE;
  }
  अन्यथा {
    CURL_TRC_CF(तथ्य, cf, "shutdown sending blocked");
  }

out:
  वापस परिणाम;
}

अटल व्योम cf_quiche_destroy(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  (व्योम)तथ्य;
  अगर(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = शून्य;
  }
}

अटल CURLcode cf_quiche_query(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;

  चयन(query) {
  हाल CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    अगर(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_बायाँ_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > पूर्णांक_उच्च) ? पूर्णांक_उच्च : (पूर्णांक)max_streams;
    CURL_TRC_CF(तथ्य, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    वापस CURLE_OK;
  }
  हाल CF_QUERY_CONNECT_REPLY_MS:
    अगर(ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < पूर्णांक_उच्च) ? (पूर्णांक)ms : पूर्णांक_उच्च;
    }
    अन्यथा
      *pres1 = -1;
    वापस CURLE_OK;
  हाल CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    वापस CURLE_OK;
  हाल CF_QUERY_SSL_INFO:
  हाल CF_QUERY_SSL_CTX_INFO: {
    काष्ठा curl_tlssessioninfo *info = pres2;
    अगर(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      वापस CURLE_OK;
    अवरोध;
  }
  हाल CF_QUERY_ALPN_NEGOTIATED: {
    स्थिर अक्षर **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : शून्य;
    वापस CURLE_OK;
  }
  शेष:
    अवरोध;
  }
  वापस cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, तथ्य, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

अटल bool cf_quiche_conn_is_alive(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    bool *निवेश_pending)
{
  काष्ठा cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *निवेश_pending = FALSE;
  अगर(!ctx->qconn)
    वापस FALSE;

  अगर(quiche_conn_is_closed(ctx->qconn)) {
    अगर(quiche_conn_is_timed_out(ctx->qconn))
      CURL_TRC_CF(तथ्य, cf, "connection was closed due to idle timeout");
    अन्यथा
      CURL_TRC_CF(तथ्य, cf, "connection is closed");
    वापस FALSE;
  }

  अगर(!cf->अग्र || !cf->अग्र->cft->is_alive(cf->अग्र, तथ्य, निवेश_pending))
    वापस FALSE;

  अगर(*निवेश_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *निवेश_pending = FALSE;
    अगर(cf_process_ingress(cf, तथ्य))
      alive = FALSE;
    अन्यथा {
      alive = TRUE;
    }
  }

  वापस alive;
}

काष्ठा Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_quiche_destroy,
  cf_quiche_connect,
  cf_quiche_shutdown,
  cf_quiche_adjust_pollset,
  Curl_cf_def_तथ्य_pending,
  cf_quiche_send,
  cf_quiche_recv,
  cf_quiche_cntrl,
  cf_quiche_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_quiche_query,
};

CURLcode Curl_cf_quiche_create(काष्ठा Curl_cfilter **pcf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_peer *origin,
                               काष्ठा Curl_peer *peer,
                               काष्ठा connectdata *conn,
                               काष्ठा Curl_sockaddr_ex *addr)
{
  काष्ठा cf_quiche_ctx *ctx = शून्य;
  काष्ठा Curl_cfilter *cf = शून्य;
  CURLcode परिणाम;

  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  परिणाम = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  अगर(!परिणाम)
    परिणाम = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  अगर(परिणाम)
    जाओ out;
  cf->conn = conn;

  परिणाम = Curl_cf_udp_create(&cf->अग्र, तथ्य, origin, peer, TRNSPRT_QUIC,
                              conn, addr, शून्य, TRNSPRT_QUIC);
  अगर(परिणाम)
    जाओ out;
  cf->अग्र->conn = cf->conn;
  cf->अग्र->sockindex = cf->sockindex;

out:
  *pcf = (!परिणाम) ? cf : शून्य;
  अगर(परिणाम) {
    अगर(cf)
      Curl_conn_cf_discard_chain(&cf, तथ्य);
    अन्यथा अगर(ctx)
      cf_quiche_ctx_free(ctx);
  }

  वापस परिणाम;
}

CURLcode Curl_cf_quiche_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                     काष्ठा Curl_peer *origin,
                                     काष्ठा Curl_peer *peer,
                                     काष्ठा Curl_cfilter **pcf)
{
  काष्ठा cf_quiche_ctx *ctx = शून्य;
  काष्ठा Curl_cfilter *cf = शून्य;
  CURLcode परिणाम;

  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  परिणाम = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  अगर(!परिणाम)
    परिणाम = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  अगर(परिणाम)
    जाओ out;
  Curl_conn_cf_जोड़ो_after(cf_at, cf);

out:
  अगर(परिणाम) {
    curlx_safefree(cf);
    अगर(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  वापस परिणाम;
}

#पूर्ण_अगर
