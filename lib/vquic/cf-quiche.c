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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#ಅನ್ತರ್ಭಾವ <quiche.h>
#ಅನ್ತರ್ಭಾವ <openssl/err.h>
#ಅನ್ತರ್ಭಾವ <openssl/ssl.h>

#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "u32_ptrset.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-socket.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "http1.h"
#ಅನ್ತರ್ಭಾವ "sockaddr.h"
#ಅನ್ತರ್ಭಾವ "vdns/cf-dns.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic_int.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic-tls.h"
#ಅನ್ತರ್ಭಾವ "vquic/cf-quiche.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "vtls/openssl.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"

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
ಶೂನ್ಯ Curl_quiche_ver(ವರ್ಣ *p, size_t len)
{
  (ಶೂನ್ಯ)curl_msnprintf(p, len, "quiche/%s", quiche_version());
}

ಸಂರಚನಾ cf_quiche_ctx {
  ಸಂರಚನಾ cf_quic_ctx q;
  ಸಂರಚನಾ ssl_peer ssl_peer;
  ಸಂರಚನಾ curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  ಸಂರಚನಾ curltime started_at;        /* time the current attempt started */
  ಸಂರಚನಾ curltime handshake_at;      /* time connect handshake finished */
  ಸಂರಚನಾ u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  ಸಂರಚನಾ dynbuf h1hdr;               /* temp buffer for header construction */
  ಸಂರಚನಾ bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t data_recvd;
  BIT(initialized);
  BIT(goaway);                       /* got GOAWAY from server */
  BIT(x509_store_setup);             /* if x509 store has been set up */
  BIT(shutdown_started);             /* queued shutdown packets */
  BIT(stats_reported);               /* connect statistics reported */
};

#ifdef DEBUG_QUICHE
/* initialize debug log callback only once */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ debug_log_init = 0;
ಸ್ಥಿರ ಶೂನ್ಯ quiche_debug_log(ಧ್ರುವ ವರ್ಣ *ಸಾಲು, ಶೂನ್ಯ *argp)
{
  (ಶೂನ್ಯ)argp;
  curl_mfprintf(stderr, "%s\n", ಸಾಲು);
}
#endif

ಸ್ಥಿರ ಶೂನ್ಯ h3_stream_set_free(uint32_t id, ಶೂನ್ಯ *stream);

ಸ್ಥಿರ CURLcode cf_quiche_ctx_init(ಸಂರಚನಾ cf_quiche_ctx *ctx,
                                   ಸಂರಚನಾ Curl_peer *origin,
                                   ಸಂರಚನಾ Curl_peer *peer,
                                   ಸಂರಚನಾ ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#ifdef DEBUG_QUICHE
  ಯದಿ(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, ರಿಕ್ತ);
    debug_log_init = 1;
  }
#endif
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  Curl_bufq_init2(&ctx->writebuf, H3_STREAM_CHUNK_SIZE, H3_STREAM_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  ctx->data_recvd = 0;
  ctx->initialized = TRUE;
  ಪ್ರತ್ಯಾಗಮ Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_ctx_free(ಸಂರಚನಾ cf_quiche_ctx *ctx)
{
  ಯದಿ(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = ರಿಕ್ತ;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_ctx_close(ಸಂರಚನಾ cf_quiche_ctx *ctx)
{
  ಯದಿ(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = ರಿಕ್ತ;
  }
  ಯದಿ(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = ರಿಕ್ತ;
  }
  ಯದಿ(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = ರಿಕ್ತ;
  }
  ಯದಿ(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = ರಿಕ್ತ;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

ಸ್ಥಿರ CURLcode cf_flush_egress(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data);

/**
 * All about the H3 internals of a stream
 */
ಸಂರಚನಾ h3_stream_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  ಸಂರಚನಾ h1_req_parser h1; /* h1 request parsing */
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

ಸ್ಥಿರ ಶೂನ್ಯ h3_stream_ctx_free(ಸಂರಚನಾ h3_stream_ctx *stream)
{
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_stream_set_free(uint32_t id, ಶೂನ್ಯ *stream)
{
  (ಶೂನ್ಯ)id;
  DEBUGASSERT(stream);
  h3_stream_ctx_free((ಸಂರಚನಾ h3_stream_ctx *)stream);
}

ಪ್ರಕಾರನಿರ್ದೇಶ bool cf_quiche_svisit(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *sdata,
                              ಸಂರಚನಾ h3_stream_ctx *stream,
                              ಶೂನ್ಯ *user_data);

ಸಂರಚನಾ cf_quiche_visit_ctx {
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ Curl_multi *multi;
  cf_quiche_svisit *cb;
  ಶೂನ್ಯ *user_data;
};

ಸ್ಥಿರ bool cf_quiche_stream_do(uint32_t mid, ಶೂನ್ಯ *val, ಶೂನ್ಯ *user_data)
{
  ಸಂರಚನಾ cf_quiche_visit_ctx *vctx = user_data;
  ಸಂರಚನಾ h3_stream_ctx *stream = val;
  ಸಂರಚನಾ Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  ಯದಿ(sdata)
    ಪ್ರತ್ಯಾಗಮ vctx->cb(vctx->cf, sdata, stream, vctx->user_data);
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_for_all_streams(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      ಶೂನ್ಯ *user_data)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ cf_quiche_visit_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_data = user_data;
  Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_stream_do, &vctx);
}

ಸ್ಥಿರ bool cf_quiche_do_resume(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *sdata,
                                ಸಂರಚನಾ h3_stream_ctx *stream,
                                ಶೂನ್ಯ *user_data)
{
  (ಶೂನ್ಯ)user_data;
  ಯದಿ(stream->quic_flow_blocked) {
    stream->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", stream->id);
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ bool cf_quiche_do_expire(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *sdata,
                                ಸಂರಚನಾ h3_stream_ctx *stream,
                                ಶೂನ್ಯ *user_data)
{
  (ಶೂನ್ಯ)user_data;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  stream->xfer_result = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ CURLcode h3_data_setup(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  ಯದಿ(stream)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  stream = curlx_calloc(1, ಪರಿಮಾಣ(*stream));
  ಯದಿ(!stream)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_ಸಾಲು_LEN);

  ಯದಿ(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h3_stream_ctx_free(stream);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_stream_close(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ h3_stream_ctx *stream)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result;

  ಯದಿ(ctx->qconn && !stream->closed) {
    quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    ಯದಿ(!stream->send_closed) {
      quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      stream->send_closed = TRUE;
    }
    stream->closed = TRUE;
    result = cf_flush_egress(cf, data);
    ಯದಿ(result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  stream->id, (ಪೂರ್ಣಾಙ್ಕ)result);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_data_done(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  (ಶೂನ್ಯ)cf;
  ಯದಿ(stream) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] easy handle is done", stream->id);
    cf_quiche_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_expire_conn_closed(ಸಂರಚನಾ Curl_cfilter *cf,
                                         ಸಂರಚನಾ Curl_easy *data)
{
  DEBUGASSERT(data->multi);
  CURL_TRC_CF(data, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_expire, ರಿಕ್ತ);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_write_hd(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ h3_stream_ctx *stream,
                               ಧ್ರುವ ವರ್ಣ *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ಯದಿ(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    ಯದಿ(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (ಪೂರ್ಣಾಙ್ಕ)stream->xfer_result,
                  blen);
  }
}

ಸಂರಚನಾ cb_ctx {
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ Curl_easy *data;
  ಸಂರಚನಾ h3_stream_ctx *stream;
};

ಸ್ಥಿರ bool is_valid_h3_header(ಧ್ರುವ uint8_t *hdr, size_t hlen)
{
  ಯಾವತ್(hlen--) {
    ವಿಕಲ್ಪ(*hdr++) {
    ಪಕ್ಷ '\n':
    ಪಕ್ಷ '\r':
    ಪಕ್ಷ '\0':
      ಪ್ರತ್ಯಾಗಮ FALSE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cb_each_header(uint8_t *name, size_t name_len,
                          uint8_t *ಮೌಲ್ಯ, size_t ಮೌಲ್ಯ_len,
                          ಶೂನ್ಯ *argp)
{
  ಸಂರಚನಾ cb_ctx *x = argp;
  ಸಂರಚನಾ Curl_cfilter *cf = x->cf;
  ಸಂರಚನಾ Curl_easy *data = x->data;
  ಸಂರಚನಾ h3_stream_ctx *stream = x->stream;
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ಯದಿ(!stream || stream->xfer_result)
    ಪ್ರತ್ಯಾಗಮ 1; /* abort iteration */

  ಯದಿ((name_len == 7) && !strncmp(HTTP_PSEUDO_STATUS, (ವರ್ಣ *)name, 7) &&
     is_valid_h3_header(ಮೌಲ್ಯ, ಮೌಲ್ಯ_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    result = Curl_http_decode_status(&stream->status_code,
                                     (ಧ್ರುವ ವರ್ಣ *)ಮೌಲ್ಯ, ಮೌಲ್ಯ_len);
    ಯದಿ(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    ಯದಿ(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, (ಧ್ರುವ ವರ್ಣ *)ಮೌಲ್ಯ, ಮೌಲ್ಯ_len);
    ಯದಿ(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    ಯದಿ(!result)
      cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] status: %s",
                stream->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(is_valid_h3_header(ಮೌಲ್ಯ, ಮೌಲ್ಯ_len) &&
       is_valid_h3_header(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  stream->id, (ಪೂರ್ಣಾಙ್ಕ)name_len, name,
                  (ಪೂರ್ಣಾಙ್ಕ)ಮೌಲ್ಯ_len, ಮೌಲ್ಯ);
      curlx_dyn_reset(&ctx->h1hdr);
      result = curlx_dyn_addn(&ctx->h1hdr, (ಧ್ರುವ ವರ್ಣ *)name, name_len);
      ಯದಿ(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      ಯದಿ(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, (ಧ್ರುವ ವರ್ಣ *)ಮೌಲ್ಯ, ಮೌಲ್ಯ_len);
      ಯದಿ(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      ಯದಿ(!result)
        cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    ಅನ್ಯಥಾ
      CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  stream->id, ಮೌಲ್ಯ_len + name_len);
  }

  ಯದಿ(result) {
    CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] on header error %d",
                stream->id, (ಪೂರ್ಣಾಙ್ಕ)result);
    ಯದಿ(!stream->xfer_result)
      stream->xfer_result = result;
  }
  ಪ್ರತ್ಯಾಗಮ stream->xfer_result ? 1 : 0;
}

ಸ್ಥಿರ CURLcode stream_resp_read(ಶೂನ್ಯ *reader_ctx,
                                 unsigned ವರ್ಣ *buf, size_t len,
                                 size_t *pnread)
{
  ಸಂರಚನಾ cb_ctx *x = reader_ctx;
  ಸಂರಚನಾ cf_quiche_ctx *ctx = x->cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, x->data);
  ssize_t nread;

  *pnread = 0;
  ಯದಿ(!stream)
    ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, stream->id, buf, len);
  ಯದಿ(!curlx_sztouz(nread, pnread))
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_flush_body(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ h3_stream_ctx *stream)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ಧ್ರುವ uint8_t *buf;
  size_t blen;

  ಯಾವತ್(stream && !stream->xfer_result) {
    ಯದಿ(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      stream->xfer_result = Curl_xfer_write_resp(
        data, (ಧ್ರುವ ವರ್ಣ *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      ಯದಿ(stream->xfer_result) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", stream->id, (ಪೂರ್ಣಾಙ್ಕ)stream->xfer_result, blen);
      }
    }
    ಅನ್ಯಥಾ
      ಭಙ್ಗ;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_recv_body(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ h3_stream_ctx *stream)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  ಸಂರಚನಾ cb_ctx cb_ctx;
  CURLcode result = CURLE_OK;

  ಯದಿ(!stream)
    ಪ್ರತ್ಯಾಗಮ;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.data = data;
  cb_ctx.stream = stream;
  Curl_bufq_reset(&ctx->writebuf);
  ಯಾವತ್(!result) {
    result = Curl_bufq_slurp(&ctx->writebuf,
                             stream_resp_read, &cb_ctx, &nread);
    ಯದಿ(!result)
      cf_quiche_flush_body(cf, data, stream);
    ಅನ್ಯಥಾ ಯದಿ(result == CURLE_AGAIN)
      ಭಙ್ಗ;
    ಅನ್ಯಥಾ ಯದಿ(result) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv_body error %d",
                  stream->id, (ಪೂರ್ಣಾಙ್ಕ)result);
      failf(data, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            stream->id, (ಪೂರ್ಣಾಙ್ಕ)result);
      stream->closed = TRUE;
      stream->reset = TRUE;
      stream->send_closed = TRUE;
      ಯದಿ(!stream->xfer_result)
        stream->xfer_result = result;
    }
  }
  cf_quiche_flush_body(cf, data, stream);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_process_ev(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ h3_stream_ctx *stream,
                                 quiche_h3_event *ev)
{
  ಗಣನಾ quiche_h3_event_type type;

  ಯದಿ(!stream)
    ಪ್ರತ್ಯಾಗಮ;

  type = quiche_h3_event_type(ev);

  ವಿಕಲ್ಪ(type) {
  ಪಕ್ಷ QUICHE_H3_EVENT_HEADERS: {
    ಸಂರಚನಾ cb_ctx cb_ctx;
    stream->resp_got_header = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.data = data;
    cb_ctx.stream = stream;
    quiche_h3_event_for_each_header(ev, cb_each_header, &cb_ctx);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [HEADERS]", stream->id);
    Curl_multi_mark_dirty(data);
    ಭಙ್ಗ;
  }
  ಪಕ್ಷ QUICHE_H3_EVENT_DATA:
    ಯದಿ(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, data, stream);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [DATA]", stream->id);
    Curl_multi_mark_dirty(data);
    ಭಙ್ಗ;

  ಪಕ್ಷ QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] RESET", stream->id);
    stream->closed = TRUE;
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    Curl_multi_mark_dirty(data);
    ಭಙ್ಗ;

  ಪಕ್ಷ QUICHE_H3_EVENT_FINISHED:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] CLOSED", stream->id);
    ಯದಿ(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, TRUE);
    }
    stream->closed = TRUE;
    Curl_multi_mark_dirty(data);
    ಭಙ್ಗ;

  ಪಕ್ಷ QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [GOAWAY]", stream->id);
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv, unhandled event %d",
                stream->id, (ಪೂರ್ಣಾಙ್ಕ)type);
    ಭಙ್ಗ;
  }
}

ಸಂರಚನಾ cf_quiche_disp_ctx {
  uint64_t stream_id;
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ Curl_multi *multi;
  quiche_h3_event *ev;
};

ಸ್ಥಿರ bool cf_quiche_disp_event(uint32_t mid, ಶೂನ್ಯ *val, ಶೂನ್ಯ *user_data)
{
  ಸಂರಚನಾ cf_quiche_disp_ctx *dctx = user_data;
  ಸಂರಚನಾ h3_stream_ctx *stream = val;

  ಯದಿ(stream->id == dctx->stream_id) {
    ಸಂರಚನಾ Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    ಯದಿ(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, stream, dctx->ev);
    ಪ್ರತ್ಯಾಗಮ FALSE; /* stop iterating */
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ CURLcode cf_poll_events(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = ರಿಕ್ತ;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  ಯಾವತ್(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    ಯದಿ(rv == QUICHE_H3_ERR_DONE) {
      ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ ಯದಿ(rv < 0) {
      CURL_TRC_CF(data, cf, "error poll: %" PRId64, rv);
      ಪ್ರತ್ಯಾಗಮ CURLE_HTTP3;
    }
    ಅನ್ಯಥಾ {
      stream = H3_STREAM_CTX(ctx, data);
      ಯದಿ(stream && stream->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, data, stream, ev);
        quiche_h3_event_free(ev);
        ಯದಿ(stream->xfer_result)
          ಪ್ರತ್ಯಾಗಮ stream->xfer_result;
      }
      ಅನ್ಯಥಾ {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        ಸಂರಚನಾ cf_quiche_disp_ctx dctx;
        dctx.stream_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = data->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸಂರಚನಾ recv_ctx {
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ Curl_easy *data;
  ಪೂರ್ಣಾಙ್ಕ pkts;
};

ಸ್ಥಿರ CURLcode cf_quiche_recv_pkts(ಧ್ರುವ unsigned ವರ್ಣ *buf, size_t buflen,
                                    size_t gso_size,
                                    ಸಂರಚನಾ sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ recv_ctx *r = userp;
  ಸಂರಚನಾ cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, offset, nread;
  ssize_t rv;

  (ಶೂನ್ಯ)ecn;

  recv_info.to = (ಸಂರಚನಾ sockaddr *)&ctx->q.local_addr;
  recv_info.to_len = ctx->q.local_addrlen;
  recv_info.from = (ಸಂರಚನಾ sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  ಪ್ರತಿ(offset = 0; offset < buflen; offset += gso_size) {
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = quiche_conn_recv(ctx->qconn,
                          (unsigned ವರ್ಣ *)CURL_UNCONST(buf + offset),
                          pktlen, &recv_info);
    ಯದಿ(!curlx_sztouz(rv, &nread)) {
      ಯದಿ(QUICHE_ERR_DONE == rv) {
        ಯದಿ(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is draining");
          ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
        }
        ಯದಿ(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is closed");
          ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->data, r->cf, "ingress, quiche is DONE");
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      ಅನ್ಯಥಾ ಯದಿ(QUICHE_ERR_TLS_FAIL == rv) {
        long verify_ok = SSL_get_verify_result(ctx->tls.ossl.ssl);
        ಯದಿ(verify_ok != X509_V_OK) {
          failf(r->data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->data, "ingress, quiche reports TLS fail");
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }
      ಅನ್ಯಥಾ {
        failf(r->data, "quiche reports error %zd on receive", rv);
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(nread < pktlen) {
      CURL_TRC_CF(r->data, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_process_ingress(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ recv_ctx rctx;
  CURLcode result;

  DEBUGASSERT(ctx->qconn);
  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  rctx.cf = cf;
  rctx.data = data;
  rctx.pkts = 0;

  result = Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಯದಿ(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(data->multi);
    cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_resume, ರಿಕ್ತ);
  }
  ಪ್ರತ್ಯಾಗಮ cf_poll_events(cf, data);
}

ಸಂರಚನಾ read_ctx {
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ Curl_easy *data;
  quiche_send_info send_info;
};

ಸ್ಥಿರ CURLcode read_pkt_to_send(ಶೂನ್ಯ *userp,
                                 unsigned ವರ್ಣ *buf, size_t buflen,
                                 size_t *pnread)
{
  ಸಂರಚನಾ read_ctx *x = userp;
  ಸಂರಚನಾ cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  ಯದಿ(rv == QUICHE_ERR_DONE)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ಯದಿ(!curlx_sztouz(rv, pnread)) {
    failf(x->data, "quiche_conn_send returned %zd", rv);
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
ಸ್ಥಿರ CURLcode cf_flush_egress(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode result;
  int64_t expiry_ns;
  int64_t timeout_ns;
  ಸಂರಚನಾ read_ctx readx;
  size_t pkt_ಎಣಿಕೆ, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  ಯದಿ(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    ಯದಿ(quiche_conn_is_closed(ctx->qconn)) {
      ಯದಿ(quiche_conn_is_timed_out(ctx->qconn))
        failf(data, "connection closed by idle timeout");
      ಅನ್ಯಥಾ
        failf(data, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, data);
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
    }
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  ಯದಿ(result) {
    ಯದಿ(result == CURLE_AGAIN) {
      Curl_expire(data, 1, EXPIRE_QUIC);
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಪ್ರತ್ಯಾಗಮ result;
  }

  readx.cf = cf;
  readx.data = data;
  memset(&readx.send_info, 0, ಪರಿಮಾಣ(readx.send_info));
  pkt_ಎಣಿಕೆ = 0;
  gsolen = quiche_conn_max_send_udp_payload_size(ctx->qconn);
  ಪ್ರತಿ(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_to_send, &readx, &nread);
    ಯದಿ(result) {
      ಯದಿ(result != CURLE_AGAIN)
        ಪ್ರತ್ಯಾಗಮ result;
      /* Nothing more to add, flush and leave */
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      ಯದಿ(result) {
        ಯದಿ(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        }
        ಪ್ರತ್ಯಾಗಮ result;
      }
      ಗಚ್ಛ out;
    }

    ++pkt_ಎಣಿಕೆ;
    ಯದಿ(nread < gsolen || pkt_ಎಣಿಕೆ >= MAX_PKT_BURST) {
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      ಯದಿ(result) {
        ಯದಿ(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        }
        ಗಚ್ಛ out;
      }
      pkt_ಎಣಿಕೆ = 0;
    }
  }

out:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  ಯದಿ(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(data, (timeout_ns / 1000000), EXPIRE_QUIC);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode recv_closed_stream(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   size_t *pnread)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(stream);
  *pnread = 0;
  ಯದಿ(stream->reset) {
    ಯದಿ(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ಅನ್ಯಥಾ ಯದಿ(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                stream->id, (ಪೂರ್ಣಾಙ್ಕ)result);
  }
  ಅನ್ಯಥಾ ಯದಿ(!stream->resp_got_header) {
    failf(data, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    result = CURLE_HTTP3;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_quiche_recv(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                               ವರ್ಣ *buf, size_t blen, size_t *pnread)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  *pnread = 0;
  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)blen;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  ಯದಿ(!stream)
    ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;

  result = cf_process_ingress(cf, data);
  ಯದಿ(result) {
    CURL_TRC_CF(data, cf, "cf_recv, error on ingress");
    ಗಚ್ಛ out;
  }

  ಯದಿ(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ ಯದಿ(stream->closed)
    result = recv_closed_stream(cf, data, pnread);
  ಅನ್ಯಥಾ ಯದಿ(quiche_conn_is_draining(ctx->qconn)) {
    failf(data, "QUIC connection is draining");
    result = CURLE_HTTP3;
  }
  ಅನ್ಯಥಾ
    result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));
  ಯದಿ(*pnread > 0)
    ctx->data_recvd += *pnread;
  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, stream->id, blen, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread,
              ctx->data_recvd);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_quiche_send_body(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ h3_stream_ctx *stream,
                                    ಧ್ರುವ uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, stream->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  ಯದಿ(rv == QUICHE_H3_ERR_DONE || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    ಯದಿ(!quiche_conn_stream_writable(ctx->qconn, stream->id, len)) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", stream->id, len);
      stream->quic_flow_blocked = TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  }
  ಅನ್ಯಥಾ ಯದಿ(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_STREAM_STATE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", stream->id, len);
    ಪ್ರತ್ಯಾಗಮ CURLE_HTTP3;
  }
  ಅನ್ಯಥಾ ಯದಿ(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_SIZE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                stream->id, len);
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  }
  ಅನ್ಯಥಾ ಯದಿ(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                stream->id, len, rv);
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(eos && (len == *pnwritten))
      stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                stream->id, len, stream->send_closed, *pnwritten);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
}

ಸ್ಥಿರ CURLcode h3_open_stream(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಧ್ರುವ uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  size_t nheader, i;
  int64_t rv;
  ಸಂರಚನಾ dynhds h2_headers;
  quiche_h3_header *nva = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  ಯದಿ(!stream) {
    result = h3_data_setup(cf, data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    stream = H3_STREAM_CTX(ctx, data);
    DEBUGASSERT(stream);
  }

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(stream);

  result = Curl_h1_req_parse_read(&stream->h1, buf, blen, ರಿಕ್ತ,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : ರಿಕ್ತ,
    0, pnwritten);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ಯದಿ(!stream->h1.done) {
    /* need more data */
    ಗಚ್ಛ out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_ಎಣಿಕೆ(&h2_headers);
  nva = curlx_malloc(ಪರಿಮಾಣ(quiche_h3_header) * nheader);
  ಯದಿ(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  ಪ್ರತಿ(i = 0; i < nheader; ++i) {
    ಸಂರಚನಾ dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned ವರ್ಣ *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].ಮೌಲ್ಯ = (unsigned ವರ್ಣ *)e->ಮೌಲ್ಯ;
    nva[i].ಮೌಲ್ಯ_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  ಯದಿ(eos && !blen)
    stream->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              stream->send_closed);
  CURL_TRC_CF(data, cf, "quiche_send_request() -> %" PRId64, rv);
  ಯದಿ(rv < 0) {
    ಯದಿ(QUICHE_H3_ERR_STREAM_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] blocked", stream->id);
      stream->quic_flow_blocked = TRUE;
      result = CURLE_AGAIN;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ {
      CURL_TRC_CF(data, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&data->state.url), rv);
    }
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ out;
  }

  DEBUGASSERT(!stream->opened);
  stream->id = (uint64_t)rv;
  stream->opened = TRUE;
  stream->closed = FALSE;
  stream->reset = FALSE;

  ಯದಿ(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    ಪ್ರತಿ(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", stream->id,
            (ಪೂರ್ಣಾಙ್ಕ)nva[i].name_len, nva[i].name,
            (ಪೂರ್ಣಾಙ್ಕ)nva[i].ಮೌಲ್ಯ_len, nva[i].ಮೌಲ್ಯ);
    }
  }

  ಯದಿ(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, data, stream, buf, blen, eos, &nwritten);
    ಯದಿ(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      result = r2;
    }
    ಅನ್ಯಥಾ ಯದಿ(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_quiche_send(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                               ಧ್ರುವ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result;

  *pnwritten = 0;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  result = cf_process_ingress(cf, data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ಯದಿ(!stream || !stream->opened) {
    result = h3_open_stream(cf, data, buf, len, eos, pnwritten);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    stream = H3_STREAM_CTX(ctx, data);
  }
  ಅನ್ಯಥಾ ಯದಿ(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
  }
  ಅನ್ಯಥಾ ಯದಿ(stream->closed) {
    ಯದಿ(stream->resp_hds_complete) {
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
      ಗಚ್ಛ out;
    }
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ {
    result = cf_quiche_send_body(cf, data, stream, buf, len, eos, pnwritten);
  }

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));

  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : (uint64_t)~0, len,
              (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool stream_is_writable(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  ಪ್ರತ್ಯಾಗಮ stream && (quiche_conn_stream_writable(
    ctx->qconn, stream->id, 1) > 0);
}

ಸ್ಥಿರ CURLcode cf_quiche_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                         ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  ಯದಿ(!ctx->qconn)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  Curl_pollset_check(data, ps, ctx->q.sockfd, &want_recv, &want_send);
  ಯದಿ(want_recv || want_send) {
    ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
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
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode h3_data_pause(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (ಶೂನ್ಯ)cf;
  ಯದಿ(!pause) {
    Curl_multi_mark_dirty(data);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_quiche_cntrl(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  ವಿಕಲ್ಪ(event) {
  ಪಕ್ಷ CF_CTRL_DATA_SETUP:
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_DATA_DONE:
    h3_data_done(cf, data);
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_DATA_DONE_SEND: {
    ಸಂರಚನಾ h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    ಯದಿ(stream && !stream->send_closed) {
      unsigned ವರ್ಣ body[1];
      size_t sent;

      stream->send_closed = TRUE;
      body[0] = 'X';
      result = cf_quiche_send(cf, data, body, 0, TRUE, &sent);
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  stream->id, (ಪೂರ್ಣಾಙ್ಕ)result, sent);
    }
    ಭಙ್ಗ;
  }
  ಪಕ್ಷ CF_CTRL_CONN_INFO_UPDATE:
    ಯದಿ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_REPORT_STATS:
    ಯದಿ(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_quiche_ctx_open(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಙ್ಕ rv;
  CURLcode result;
  ಧ್ರುವ ಸಂರಚನಾ Curl_sockaddr_ex *sockaddr;
  ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_VERSION);
  ಯದಿ(!ctx->cfg) {
    failf(data, "cannot create quiche config");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
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
                               &ALPN_SPEC_H3, ರಿಕ್ತ, ರಿಕ್ತ, cf, ರಿಕ್ತ);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = Curl_rand(data, ctx->scid, ಪರಿಮಾಣ(ctx->scid));
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಯದಿ(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, ರಿಕ್ತ))
    ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = ಪರಿಮಾಣ(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (ಸಂರಚನಾ sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  ಯದಿ(rv == -1)
    ಪ್ರತ್ಯಾಗಮ CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((ಧ್ರುವ uint8_t *)ctx->scid,
                                        ಪರಿಮಾಣ(ctx->scid), ರಿಕ್ತ, 0,
                                        (ಸಂರಚನಾ sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  ಯದಿ(!ctx->qconn) {
    failf(data, "cannot create quiche connection");
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  /* Known to not work on Windows */
#ಯದಿ !defined(_WIN32) && defined(HAVE_QUICHE_CONN_SET_QLOG_FD)
  {
    ಪೂರ್ಣಾಙ್ಕ qfd;
    (ಶೂನ್ಯ)Curl_qlogdir(data, ctx->scid, ಪರಿಮಾಣ(ctx->scid), &qfd);
    ಯದಿ(qfd != -1)
      quiche_conn_set_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#endif

  result = cf_flush_egress(cf, data);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  {
    unsigned ವರ್ಣ alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    unsigned alpn_len, offset = 0;

    /* Replace each ALPN length prefix by a comma. */
    ಯಾವತ್(offset < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[offset];
      alpn_protocols[offset] = ',';
      offset += 1 + alpn_len;
    }

    CURL_TRC_CF(data, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_quiche_verify_peer(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  ಪ್ರತ್ಯಾಗಮ Curl_vquic_tls_verify_peer(&ctx->tls, cf, data, &ctx->ssl_peer);
}

ಸ್ಥಿರ CURLcode cf_quiche_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  bool *done)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  /* Connect the UDP filter first */
  ಯದಿ(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ಯದಿ(result || !*done)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  *done = FALSE;
  ಯದಿ(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  ಯದಿ(!ctx->qconn) {
    result = cf_quiche_ctx_open(cf, data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    ctx->started_at = ctx->q.last_op;
    result = cf_flush_egress(cf, data);
    /* we do not expect to be able to recv anything yet */
    ಗಚ್ಛ out;
  }

  result = cf_process_ingress(cf, data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = cf_flush_egress(cf, data);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ಯದಿ(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.last_op;
    CURL_TRC_CF(data, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    result = cf_quiche_verify_peer(cf, data);
    ಯದಿ(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      ಯದಿ(!ctx->h3config) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_set_max_field_section_size(ctx->h3config,
                                                  MAX_HTTP_RESP_HEADER_SIZE);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      ಯದಿ(!ctx->h3c) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
      cf->connected = TRUE;
      *done = TRUE;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    result = CURLE_WEIRD_SERVER_REPLY;
  }

out:
#ifdef CURLVERBOSE
  ಯದಿ(result && result != CURLE_AGAIN) {
    ಸಂರಚನಾ ip_quadruple ip;

    ಯದಿ(!Curl_cf_socket_peek(cf->next, data, ರಿಕ್ತ, ರಿಕ್ತ, &ip))
      infof(data, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    ಅನ್ಯಥಾ
      infof(data, "connect failed: %s", curl_easy_strerror(result));
  }
#endif
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_quiche_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  ಯದಿ(cf->shutdown || !ctx || !ctx->qconn) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  *done = FALSE;
  ಯದಿ(!ctx->shutdown_started) {
    ಪೂರ್ಣಾಙ್ಕ err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, ರಿಕ್ತ, 0);
    ಯದಿ(err) {
      CURL_TRC_CF(data, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
  }

  ಯದಿ(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
    result = cf_flush_egress(cf, data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ಯದಿ(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
  }
  ಅನ್ಯಥಾ {
    CURL_TRC_CF(data, cf, "shutdown sending blocked");
  }

out:
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_destroy(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  (ಶೂನ್ಯ)data;
  ಯದಿ(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = ರಿಕ್ತ;
  }
}

ಸ್ಥಿರ CURLcode cf_quiche_query(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;

  ವಿಕಲ್ಪ(query) {
  ಪಕ್ಷ CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    ಯದಿ(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_ಎಡ_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > INT_MAX) ? INT_MAX : (ಪೂರ್ಣಾಙ್ಕ)max_streams;
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಪಕ್ಷ CF_QUERY_CONNECT_REPLY_MS:
    ಯದಿ(ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (ಪೂರ್ಣಾಙ್ಕ)ms : INT_MAX;
    }
    ಅನ್ಯಥಾ
      *pres1 = -1;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CF_QUERY_SSL_INFO:
  ಪಕ್ಷ CF_QUERY_SSL_CTX_INFO: {
    ಸಂರಚನಾ curl_tlssessioninfo *info = pres2;
    ಯದಿ(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    ಭಙ್ಗ;
  }
  ಪಕ್ಷ CF_QUERY_ALPN_NEGOTIATED: {
    ಧ್ರುವ ವರ್ಣ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ bool cf_quiche_conn_is_alive(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    bool *ಒಳಹರಿವು_pending)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ಒಳಹರಿವು_pending = FALSE;
  ಯದಿ(!ctx->qconn)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಯದಿ(quiche_conn_is_closed(ctx->qconn)) {
    ಯದಿ(quiche_conn_is_timed_out(ctx->qconn))
      CURL_TRC_CF(data, cf, "connection was closed due to idle timeout");
    ಅನ್ಯಥಾ
      CURL_TRC_CF(data, cf, "connection is closed");
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }

  ಯದಿ(!cf->next || !cf->next->cft->is_alive(cf->next, data, ಒಳಹರಿವು_pending))
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಯದಿ(*ಒಳಹರಿವು_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *ಒಳಹರಿವು_pending = FALSE;
    ಯದಿ(cf_process_ingress(cf, data))
      alive = FALSE;
    ಅನ್ಯಥಾ {
      alive = TRUE;
    }
  }

  ಪ್ರತ್ಯಾಗಮ alive;
}

ಸಂರಚನಾ Curl_cftype Curl_cft_http3 = {
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

CURLcode Curl_cf_quiche_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_peer *origin,
                               ಸಂರಚನಾ Curl_peer *peer,
                               ಸಂರಚನಾ connectdata *conn,
                               ಸಂರಚನಾ Curl_sockaddr_ex *addr)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  CURLcode result;

  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  ಯದಿ(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ರಿಕ್ತ, TRNSPRT_QUIC);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : ರಿಕ್ತ;
  ಯದಿ(result) {
    ಯದಿ(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    ಅನ್ಯಥಾ ಯದಿ(ctx)
      cf_quiche_ctx_free(ctx);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_quiche_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                     ಸಂರಚನಾ Curl_peer *origin,
                                     ಸಂರಚನಾ Curl_peer *peer,
                                     ಸಂರಚನಾ Curl_cfilter **pcf)
{
  ಸಂರಚನಾ cf_quiche_ctx *ctx = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  CURLcode result;

  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  ಯದಿ(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);

out:
  ಯದಿ(result) {
    curlx_safefree(cf);
    ಯದಿ(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif
