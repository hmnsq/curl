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
#تعریف_کرو CURL_H3_NO_ERROR  0x0100

#تعریف_کرو MAX_PKT_مدت          10

#تعریف_کرو QUIC_MAX_STREAMS       100

#تعریف_کرو H3_دھارا_کھڑکی_سائز  (1024 * 128)
#تعریف_کرو H3_دھارا_CHUNK_سائز   (1024 * 16)
/* Receive and Send max number of chunks follows from the
 * chunk size and window size */
#تعریف_کرو H3_دھارا_RECV_CHUNKS \
  (H3_دھارا_کھڑکی_سائز / H3_دھارا_CHUNK_سائز)

/*
 * Store quiche version info in this buffer.
 */
خالی Curl_quiche_ver(حرف *p, size_t len)
{
  (خالی)curl_msnprintf(p, len, "quiche/%s", quiche_نسخہ());
}

ساخت cf_quiche_ctx {
  ساخت cf_quic_ctx q;
  ساخت ssl_peer ssl_peer;
  ساخت curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  ساخت curltime started_at;        /* time the current attempt started */
  ساخت curltime handshake_at;      /* time connect handshake finished */
  ساخت u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  ساخت dynbuf h1hdr;               /* temp buffer for header construction */
  ساخت bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t ڈیٹا_recvd;
  ثنائی_ہندسہ(initialized);
  ثنائی_ہندسہ(goaway);                       /* got GOAWAY from server */
  ثنائی_ہندسہ(x509_store_تیاری);             /* if x509 store has been set up */
  ثنائی_ہندسہ(shutdown_started);             /* queued shutdown packets */
  ثنائی_ہندسہ(stats_reported);               /* connect statistics reported */
};

#اگر_تعریف_شدہ DEBUG_QUICHE
/* initialize debug log callback only once */
ساکن صحیح debug_log_init = 0;
ساکن خالی quiche_debug_log(ثابت حرف *سطر, خالی *argp)
{
  (خالی)argp;
  curl_mfprintf(stderr, "%s\n", سطر);
}
#اگر_ختم

ساکن خالی h3_دھارا_سیٹ_free(uint32_t id, خالی *دھارا);

ساکن CURLcode cf_quiche_ctx_init(ساخت cf_quiche_ctx *ctx,
                                   ساخت Curl_peer *origin,
                                   ساخت Curl_peer *peer,
                                   ساخت ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#اگر_تعریف_شدہ DEBUG_QUICHE
  اگر(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, تہی);
    debug_log_init = 1;
  }
#اگر_ختم
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_سرخی);
  Curl_u32_ptrset_init(&ctx->streams, h3_دھارا_سیٹ_free);
  Curl_bufq_init2(&ctx->writebuf, H3_دھارا_CHUNK_سائز, H3_دھارا_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندی);
  ctx->ڈیٹا_recvd = 0;
  ctx->initialized = TRUE;
  واپس Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

ساکن خالی cf_quiche_ctx_free(ساخت cf_quiche_ctx *ctx)
{
  اگر(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = تہی;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

ساکن خالی cf_quiche_ctx_close(ساخت cf_quiche_ctx *ctx)
{
  اگر(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = تہی;
  }
  اگر(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = تہی;
  }
  اگر(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = تہی;
  }
  اگر(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = تہی;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

ساکن CURLcode cf_flush_egress(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا);

/**
 * All about the H3 internals of a stream
 */
ساخت h3_دھارا_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  ساخت h1_req_parser h1; /* h1 request parsing */
  uint64_t error3;         /* HTTP/3 stream error code */
  int32_t حالت_رمز;     /* HTTP status code */
  CURLcode xfer_نتیجہ;    /* result from cf_quiche_write_(hd/body) */
  ثنائی_ہندسہ(opened);             /* TRUE after stream has been opened */
  ثنائی_ہندسہ(closed);             /* TRUE on stream close */
  ثنائی_ہندسہ(reset);              /* TRUE on stream reset */
  ثنائی_ہندسہ(send_closed);        /* stream is locally closed */
  ثنائی_ہندسہ(resp_hds_complete);  /* final response has been received */
  ثنائی_ہندسہ(resp_got_سرخی);    /* TRUE when h3 stream has recvd some HEADER */
  ثنائی_ہندسہ(quic_flow_blocked);  /* stream is blocked by QUIC flow control */
};

ساکن خالی h3_دھارا_ctx_free(ساخت h3_دھارا_ctx *دھارا)
{
  Curl_h1_req_parse_free(&دھارا->h1);
  curlx_free(دھارا);
}

ساکن خالی h3_دھارا_سیٹ_free(uint32_t id, خالی *دھارا)
{
  (خالی)id;
  DEBUGASSERT(دھارا);
  h3_دھارا_ctx_free((ساخت h3_دھارا_ctx *)دھارا);
}

قسمتعریف bool cf_quiche_svisit(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *sdata,
                              ساخت h3_دھارا_ctx *دھارا,
                              خالی *user_ڈیٹا);

ساخت cf_quiche_دورہ_ctx {
  ساخت Curl_cfilter *cf;
  ساخت Curl_multi *multi;
  cf_quiche_svisit *cb;
  خالی *user_ڈیٹا;
};

ساکن bool cf_quiche_دھارا_do(uint32_t mid, خالی *val, خالی *user_ڈیٹا)
{
  ساخت cf_quiche_دورہ_ctx *vctx = user_ڈیٹا;
  ساخت h3_دھارا_ctx *دھارا = val;
  ساخت Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  اگر(sdata)
    واپس vctx->cb(vctx->cf, sdata, دھارا, vctx->user_ڈیٹا);
  واپس TRUE;
}

ساکن خالی cf_quiche_for_all_streams(ساخت Curl_cfilter *cf,
                                      ساخت Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      خالی *user_ڈیٹا)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  ساخت cf_quiche_دورہ_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_ڈیٹا = user_ڈیٹا;
  Curl_u32_ptrset_دورہ(&ctx->streams, cf_quiche_دھارا_do, &vctx);
}

ساکن bool cf_quiche_do_resume(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *sdata,
                                ساخت h3_دھارا_ctx *دھارا,
                                خالی *user_ڈیٹا)
{
  (خالی)user_ڈیٹا;
  اگر(دھارا->quic_flow_blocked) {
    دھارا->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", دھارا->id);
  }
  واپس TRUE;
}

ساکن bool cf_quiche_do_expire(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *sdata,
                                ساخت h3_دھارا_ctx *دھارا,
                                خالی *user_ڈیٹا)
{
  (خالی)user_ڈیٹا;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  دھارا->xfer_نتیجہ = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  واپس TRUE;
}

ساکن CURLcode h3_ڈیٹا_تیاری(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);

  اگر(دھارا)
    واپس CURLE_OK;

  دھارا = curlx_calloc(1, حجم(*دھارا));
  اگر(!دھارا)
    واپس CURLE_برآمد_کا_حافظہ;

  دھارا->id = -1;
  Curl_h1_req_parse_init(&دھارا->h1, H1_PARSE_DEFAULT_MAX_سطر_LEN);

  اگر(Curl_u32_ptrset_سیٹ(&ctx->streams, ڈیٹا->mid, دھارا)) {
    h3_دھارا_ctx_free(دھارا);
    واپس CURLE_برآمد_کا_حافظہ;
  }

  واپس CURLE_OK;
}

ساکن خالی cf_quiche_دھارا_close(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت h3_دھارا_ctx *دھارا)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;

  اگر(ctx->qconn && !دھارا->closed) {
    quiche_conn_دھارا_shutdown(ctx->qconn, دھارا->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    اگر(!دھارا->send_closed) {
      quiche_conn_دھارا_shutdown(ctx->qconn, دھارا->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      دھارا->send_closed = TRUE;
    }
    دھارا->closed = TRUE;
    نتیجہ = cf_flush_egress(cf, ڈیٹا);
    اگر(نتیجہ)
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  دھارا->id, (صحیح)نتیجہ);
  }
}

ساکن خالی h3_ڈیٹا_مکمل(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);

  (خالی)cf;
  اگر(دھارا) {
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] easy handle is done", دھارا->id);
    cf_quiche_دھارا_close(cf, ڈیٹا, دھارا);
    Curl_u32_ptrset_unset(&ctx->streams, ڈیٹا->mid);
  }
}

ساکن خالی cf_quiche_expire_conn_closed(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا)
{
  DEBUGASSERT(ڈیٹا->multi);
  CURL_TRC_CF(ڈیٹا, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, ڈیٹا->multi, cf_quiche_do_expire, تہی);
}

ساکن خالی cf_quiche_write_hd(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت h3_دھارا_ctx *دھارا,
                               ثابت حرف *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  اگر(!دھارا->xfer_نتیجہ) {
    دھارا->xfer_نتیجہ = Curl_xfer_write_resp_hd(ڈیٹا, buf, blen, eos);
    اگر(دھارا->xfer_نتیجہ)
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", دھارا->id, (صحیح)دھارا->xfer_نتیجہ,
                  blen);
  }
}

ساخت cb_ctx {
  ساخت Curl_cfilter *cf;
  ساخت Curl_easy *ڈیٹا;
  ساخت h3_دھارا_ctx *دھارا;
};

ساکن bool is_valid_h3_سرخی(ثابت uint8_t *hdr, size_t hlen)
{
  جب(hlen--) {
    انتخاب(*hdr++) {
    حالت '\n':
    حالت '\r':
    حالت '\0':
      واپس FALSE;
    }
  }
  واپس TRUE;
}

ساکن صحیح cb_each_سرخی(uint8_t *name, size_t name_len,
                          uint8_t *قدر, size_t قدر_len,
                          خالی *argp)
{
  ساخت cb_ctx *x = argp;
  ساخت Curl_cfilter *cf = x->cf;
  ساخت Curl_easy *ڈیٹا = x->ڈیٹا;
  ساخت h3_دھارا_ctx *دھارا = x->دھارا;
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!دھارا || دھارا->xfer_نتیجہ)
    واپس 1; /* abort iteration */

  اگر((name_len == 7) && !لڑی_موازنہ(HTTP_PSEUDO_حالت, (حرف *)name, 7) &&
     is_valid_h3_سرخی(قدر, قدر_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    نتیجہ = Curl_http_decode_حالت(&دھارا->حالت_رمز,
                                     (ثابت حرف *)قدر, قدر_len);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)قدر, قدر_len);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    اگر(!نتیجہ)
      cf_quiche_write_hd(cf, ڈیٹا, دھارا, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] status: %s",
                دھارا->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  ورنہ {
    اگر(is_valid_h3_سرخی(قدر, قدر_len) &&
       is_valid_h3_سرخی(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  دھارا->id, (صحیح)name_len, name,
                  (صحیح)قدر_len, قدر);
      curlx_dyn_reset(&ctx->h1hdr);
      نتیجہ = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)name, name_len);
      اگر(!نتیجہ)
        نتیجہ = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      اگر(!نتیجہ)
        نتیجہ = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)قدر, قدر_len);
      اگر(!نتیجہ)
        نتیجہ = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      اگر(!نتیجہ)
        cf_quiche_write_hd(cf, ڈیٹا, دھارا, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    ورنہ
      CURL_TRC_CF(x->ڈیٹا, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  دھارا->id, قدر_len + name_len);
  }

  اگر(نتیجہ) {
    CURL_TRC_CF(x->ڈیٹا, x->cf, "[%" PRIu64 "] on header error %d",
                دھارا->id, (صحیح)نتیجہ);
    اگر(!دھارا->xfer_نتیجہ)
      دھارا->xfer_نتیجہ = نتیجہ;
  }
  واپس دھارا->xfer_نتیجہ ? 1 : 0;
}

ساکن CURLcode دھارا_resp_read(خالی *reader_ctx,
                                 غیرعلامتی حرف *buf, size_t len,
                                 size_t *pnread)
{
  ساخت cb_ctx *x = reader_ctx;
  ساخت cf_quiche_ctx *ctx = x->cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, x->ڈیٹا);
  ssize_t nread;

  *pnread = 0;
  اگر(!دھارا)
    واپس CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, دھارا->id, buf, len);
  اگر(!curlx_sztouz(nread, pnread))
    واپس CURLE_AGAIN;
  واپس CURLE_OK;
}

ساکن خالی cf_quiche_flush_body(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت h3_دھارا_ctx *دھارا)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  ثابت uint8_t *buf;
  size_t blen;

  جب(دھارا && !دھارا->xfer_نتیجہ) {
    اگر(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      دھارا->xfer_نتیجہ = Curl_xfer_write_resp(
        ڈیٹا, (ثابت حرف *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      اگر(دھارا->xfer_نتیجہ) {
        CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", دھارا->id, (صحیح)دھارا->xfer_نتیجہ, blen);
      }
    }
    ورنہ
      توڑو;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

ساکن خالی cf_quiche_recv_body(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                ساخت h3_دھارا_ctx *دھارا)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  ساخت cb_ctx cb_ctx;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!دھارا)
    واپس;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.ڈیٹا = ڈیٹا;
  cb_ctx.دھارا = دھارا;
  Curl_bufq_reset(&ctx->writebuf);
  جب(!نتیجہ) {
    نتیجہ = Curl_bufq_slurp(&ctx->writebuf,
                             دھارا_resp_read, &cb_ctx, &nread);
    اگر(!نتیجہ)
      cf_quiche_flush_body(cf, ڈیٹا, دھارا);
    ورنہ اگر(نتیجہ == CURLE_AGAIN)
      توڑو;
    ورنہ اگر(نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] recv_body error %d",
                  دھارا->id, (صحیح)نتیجہ);
      failf(ڈیٹا, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            دھارا->id, (صحیح)نتیجہ);
      دھارا->closed = TRUE;
      دھارا->reset = TRUE;
      دھارا->send_closed = TRUE;
      اگر(!دھارا->xfer_نتیجہ)
        دھارا->xfer_نتیجہ = نتیجہ;
    }
  }
  cf_quiche_flush_body(cf, ڈیٹا, دھارا);
}

ساکن خالی cf_quiche_process_ev(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت h3_دھارا_ctx *دھارا,
                                 quiche_h3_event *ev)
{
  شمار quiche_h3_event_type type;

  اگر(!دھارا)
    واپس;

  type = quiche_h3_event_type(ev);

  انتخاب(type) {
  حالت QUICHE_H3_EVENT_سرخیاں: {
    ساخت cb_ctx cb_ctx;
    دھارا->resp_got_سرخی = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.ڈیٹا = ڈیٹا;
    cb_ctx.دھارا = دھارا;
    quiche_h3_event_for_each_سرخی(ev, cb_each_سرخی, &cb_ctx);
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] <- [HEADERS]", دھارا->id);
    Curl_multi_mark_dirty(ڈیٹا);
    توڑو;
  }
  حالت QUICHE_H3_EVENT_ڈیٹا:
    اگر(!دھارا->resp_hds_complete) {
      دھارا->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, ڈیٹا, دھارا, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, ڈیٹا, دھارا);
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] <- [DATA]", دھارا->id);
    Curl_multi_mark_dirty(ڈیٹا);
    توڑو;

  حالت QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] RESET", دھارا->id);
    دھارا->closed = TRUE;
    دھارا->reset = TRUE;
    دھارا->send_closed = TRUE;
    Curl_multi_mark_dirty(ڈیٹا);
    توڑو;

  حالت QUICHE_H3_EVENT_ختم_شدہ:
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] CLOSED", دھارا->id);
    اگر(!دھارا->resp_hds_complete) {
      دھارا->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, ڈیٹا, دھارا, "\r\n", 2, TRUE);
    }
    دھارا->closed = TRUE;
    Curl_multi_mark_dirty(ڈیٹا);
    توڑو;

  حالت QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] <- [GOAWAY]", دھارا->id);
    توڑو;

  طےشدہ:
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] recv, unhandled event %d",
                دھارا->id, (صحیح)type);
    توڑو;
  }
}

ساخت cf_quiche_disp_ctx {
  uint64_t دھارا_id;
  ساخت Curl_cfilter *cf;
  ساخت Curl_multi *multi;
  quiche_h3_event *ev;
};

ساکن bool cf_quiche_disp_event(uint32_t mid, خالی *val, خالی *user_ڈیٹا)
{
  ساخت cf_quiche_disp_ctx *dctx = user_ڈیٹا;
  ساخت h3_دھارا_ctx *دھارا = val;

  اگر(دھارا->id == dctx->دھارا_id) {
    ساخت Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    اگر(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, دھارا, dctx->ev);
    واپس FALSE; /* stop iterating */
  }
  واپس TRUE;
}

ساکن CURLcode cf_poll_events(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = تہی;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  جب(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    اگر(rv == QUICHE_H3_ERR_مکمل) {
      توڑو;
    }
    ورنہ اگر(rv < 0) {
      CURL_TRC_CF(ڈیٹا, cf, "error poll: %" PRId64, rv);
      واپس CURLE_HTTP3;
    }
    ورنہ {
      دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
      اگر(دھارا && دھارا->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, ڈیٹا, دھارا, ev);
        quiche_h3_event_free(ev);
        اگر(دھارا->xfer_نتیجہ)
          واپس دھارا->xfer_نتیجہ;
      }
      ورنہ {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        ساخت cf_quiche_disp_ctx dctx;
        dctx.دھارا_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = ڈیٹا->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_دورہ(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  واپس CURLE_OK;
}

ساخت recv_ctx {
  ساخت Curl_cfilter *cf;
  ساخت Curl_easy *ڈیٹا;
  صحیح pkts;
};

ساکن CURLcode cf_quiche_recv_pkts(ثابت غیرعلامتی حرف *buf, size_t buflen,
                                    size_t gso_سائز,
                                    ساخت sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    خالی *userp)
{
  ساخت recv_ctx *r = userp;
  ساخت cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, انحراف, nread;
  ssize_t rv;

  (خالی)ecn;

  recv_info.مقصد = (ساخت sockaddr *)&ctx->q.local_addr;
  recv_info.مقصد_len = ctx->q.local_addrlen;
  recv_info.from = (ساخت sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  برائے(انحراف = 0; انحراف < buflen; انحراف += gso_سائز) {
    pktlen = ((انحراف + gso_سائز) <= buflen) ? gso_سائز : (buflen - انحراف);
    rv = quiche_conn_recv(ctx->qconn,
                          (غیرعلامتی حرف *)CURL_UNCONST(buf + انحراف),
                          pktlen, &recv_info);
    اگر(!curlx_sztouz(rv, &nread)) {
      اگر(QUICHE_ERR_مکمل == rv) {
        اگر(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->ڈیٹا, r->cf, "ingress, connection is draining");
          واپس CURLE_RECV_ERROR;
        }
        اگر(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->ڈیٹا, r->cf, "ingress, connection is closed");
          واپس CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->ڈیٹا, r->cf, "ingress, quiche is DONE");
        واپس CURLE_OK;
      }
      ورنہ اگر(QUICHE_ERR_TLS_FAIL == rv) {
        طویل verify_ok = SSL_get_verify_نتیجہ(ctx->tls.ossl.ssl);
        اگر(verify_ok != X509_V_OK) {
          failf(r->ڈیٹا, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          واپس CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->ڈیٹا, "ingress, quiche reports TLS fail");
        واپس CURLE_RECV_ERROR;
      }
      ورنہ {
        failf(r->ڈیٹا, "quiche reports error %zd on receive", rv);
        واپس CURLE_RECV_ERROR;
      }
    }
    ورنہ اگر(nread < pktlen) {
      CURL_TRC_CF(r->ڈیٹا, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  واپس CURLE_OK;
}

ساکن CURLcode cf_process_ingress(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  ساخت recv_ctx rctx;
  CURLcode نتیجہ;

  DEBUGASSERT(ctx->qconn);
  نتیجہ = Curl_vquic_tls_before_recv(&ctx->tls, cf, ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;

  rctx.cf = cf;
  rctx.ڈیٹا = ڈیٹا;
  rctx.pkts = 0;

  نتیجہ = Curl_vquic_recv_packets(cf, ڈیٹا, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(ڈیٹا->multi);
    cf_quiche_for_all_streams(cf, ڈیٹا->multi, cf_quiche_do_resume, تہی);
  }
  واپس cf_poll_events(cf, ڈیٹا);
}

ساخت read_ctx {
  ساخت Curl_cfilter *cf;
  ساخت Curl_easy *ڈیٹا;
  quiche_send_info send_info;
};

ساکن CURLcode read_pkt_مقصد_send(خالی *userp,
                                 غیرعلامتی حرف *buf, size_t buflen,
                                 size_t *pnread)
{
  ساخت read_ctx *x = userp;
  ساخت cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  اگر(rv == QUICHE_ERR_مکمل)
    واپس CURLE_AGAIN;

  اگر(!curlx_sztouz(rv, pnread)) {
    failf(x->ڈیٹا, "quiche_conn_send returned %zd", rv);
    واپس CURLE_SEND_ERROR;
  }
  واپس CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
ساکن CURLcode cf_flush_egress(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode نتیجہ;
  int64_t expiry_ns;
  int64_t timeout_ns;
  ساخت read_ctx readx;
  size_t pkt_گنتی, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  اگر(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    اگر(quiche_conn_is_closed(ctx->qconn)) {
      اگر(quiche_conn_is_timed_برآمد(ctx->qconn))
        failf(ڈیٹا, "connection closed by idle timeout");
      ورنہ
        failf(ڈیٹا, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, ڈیٹا);
      واپس CURLE_SEND_ERROR;
    }
  }

  نتیجہ = Curl_vquic_flush(cf, ڈیٹا, &ctx->q);
  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_AGAIN) {
      Curl_expire(ڈیٹا, 1, EXPIRE_QUIC);
      واپس CURLE_OK;
    }
    واپس نتیجہ;
  }

  readx.cf = cf;
  readx.ڈیٹا = ڈیٹا;
  memset(&readx.send_info, 0, حجم(readx.send_info));
  pkt_گنتی = 0;
  gsolen = quiche_conn_max_send_udp_payload_سائز(ctx->qconn);
  برائے(;;) {
    /* add the next packet to send, if any, to our buffer */
    نتیجہ = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_مقصد_send, &readx, &nread);
    اگر(نتیجہ) {
      اگر(نتیجہ != CURLE_AGAIN)
        واپس نتیجہ;
      /* Nothing more to add, flush and leave */
      نتیجہ = Curl_vquic_send(cf, ڈیٹا, &ctx->q, gsolen);
      اگر(نتیجہ) {
        اگر(نتیجہ == CURLE_AGAIN) {
          Curl_expire(ڈیٹا, 1, EXPIRE_QUIC);
          واپس CURLE_OK;
        }
        واپس نتیجہ;
      }
      جاؤ برآمد;
    }

    ++pkt_گنتی;
    اگر(nread < gsolen || pkt_گنتی >= MAX_PKT_مدت) {
      نتیجہ = Curl_vquic_send(cf, ڈیٹا, &ctx->q, gsolen);
      اگر(نتیجہ) {
        اگر(نتیجہ == CURLE_AGAIN) {
          Curl_expire(ڈیٹا, 1, EXPIRE_QUIC);
          واپس CURLE_OK;
        }
        جاؤ برآمد;
      }
      pkt_گنتی = 0;
    }
  }

برآمد:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  اگر(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(ڈیٹا, (timeout_ns / 1000000), EXPIRE_QUIC);
  واپس نتیجہ;
}

ساکن CURLcode recv_closed_دھارا(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   size_t *pnread)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(دھارا);
  *pnread = 0;
  اگر(دھارا->reset) {
    اگر(دھارا->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(ڈیٹا, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", دھارا->id);
      connclose(cf->conn); /* do not use this anymore */
      ڈیٹا->state.refused_دھارا = TRUE;
      واپس CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ورنہ اگر(دھارا->resp_hds_complete && ڈیٹا->req.no_body) {
        CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", دھارا->id, دھارا->error3,
                    Curl_vquic_h3_err_str(دھارا->error3));
        واپس CURLE_OK;
    }
    failf(ڈیٹا, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", دھارا->id, دھارا->error3,
          Curl_vquic_h3_err_str(دھارا->error3));
    نتیجہ = ڈیٹا->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                دھارا->id, (صحیح)نتیجہ);
  }
  ورنہ اگر(!دھارا->resp_got_سرخی) {
    failf(ڈیٹا, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          دھارا->id);
    نتیجہ = CURLE_HTTP3;
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_quiche_recv(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                               حرف *buf, size_t blen, size_t *pnread)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
  CURLcode نتیجہ = CURLE_OK;

  *pnread = 0;
  (خالی)buf;
  (خالی)blen;
  Curl_vquic_ctx_update_وقت(&ctx->q, Curl_pgrs_now(ڈیٹا));

  اگر(!دھارا)
    واپس CURLE_RECV_ERROR;

  نتیجہ = cf_process_ingress(cf, ڈیٹا);
  اگر(نتیجہ) {
    CURL_TRC_CF(ڈیٹا, cf, "cf_recv, error on ingress");
    جاؤ برآمد;
  }

  اگر(دھارا->xfer_نتیجہ) {
    cf_quiche_دھارا_close(cf, ڈیٹا, دھارا);
    نتیجہ = دھارا->xfer_نتیجہ;
    جاؤ برآمد;
  }
  ورنہ اگر(دھارا->closed)
    نتیجہ = recv_closed_دھارا(cf, ڈیٹا, pnread);
  ورنہ اگر(quiche_conn_is_draining(ctx->qconn)) {
    failf(ڈیٹا, "QUIC connection is draining");
    نتیجہ = CURLE_HTTP3;
  }
  ورنہ
    نتیجہ = CURLE_AGAIN;

برآمد:
  نتیجہ = Curl_1st_fatal(نتیجہ, cf_flush_egress(cf, ڈیٹا));
  اگر(*pnread > 0)
    ctx->ڈیٹا_recvd += *pnread;
  CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, دھارا->id, blen, (صحیح)نتیجہ, *pnread,
              ctx->ڈیٹا_recvd);
  واپس نتیجہ;
}

ساکن CURLcode cf_quiche_send_body(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت h3_دھارا_ctx *دھارا,
                                    ثابت uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, دھارا->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  اگر(rv == QUICHE_H3_ERR_مکمل || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    اگر(!quiche_conn_دھارا_writable(ctx->qconn, دھارا->id, len)) {
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", دھارا->id, len);
      دھارا->quic_flow_blocked = TRUE;
    }
    واپس CURLE_AGAIN;
  }
  ورنہ اگر(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_دھارا_STATE) {
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", دھارا->id, len);
    واپس CURLE_HTTP3;
  }
  ورنہ اگر(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_سائز) {
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                دھارا->id, len);
    واپس CURLE_SEND_ERROR;
  }
  ورنہ اگر(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                دھارا->id, len, rv);
    واپس CURLE_SEND_ERROR;
  }
  ورنہ {
    اگر(eos && (len == *pnwritten))
      دھارا->send_closed = TRUE;
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                دھارا->id, len, دھارا->send_closed, *pnwritten);
    واپس CURLE_OK;
  }
}

ساکن CURLcode h3_open_دھارا(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ثابت uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
  size_t nheader, i;
  int64_t rv;
  ساخت dynhds h2_سرخیاں;
  quiche_h3_سرخی *nva = تہی;
  CURLcode نتیجہ = CURLE_OK;

  *pnwritten = 0;
  اگر(!دھارا) {
    نتیجہ = h3_ڈیٹا_تیاری(cf, ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
    دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
    DEBUGASSERT(دھارا);
  }

  Curl_dynhds_init(&h2_سرخیاں, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(دھارا);

  نتیجہ = Curl_h1_req_parse_read(&دھارا->h1, buf, blen, تہی,
    !ڈیٹا->state.http_ignorecustom ?
    CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST) : تہی,
    0, pnwritten);
  اگر(نتیجہ)
    جاؤ برآمد;
  اگر(!دھارا->h1.مکمل) {
    /* need more data */
    جاؤ برآمد;
  }
  DEBUGASSERT(دھارا->h1.req);

  نتیجہ = Curl_http_req_مقصد_h2(&h2_سرخیاں, دھارا->h1.req, ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

  /* no longer needed */
  Curl_h1_req_parse_free(&دھارا->h1);

  nheader = Curl_dynhds_گنتی(&h2_سرخیاں);
  nva = curlx_malloc(حجم(quiche_h3_سرخی) * nheader);
  اگر(!nva) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  برائے(i = 0; i < nheader; ++i) {
    ساخت dynhds_اندراج *e = Curl_dynhds_getn(&h2_سرخیاں, i);
    nva[i].name = (غیرعلامتی حرف *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].قدر = (غیرعلامتی حرف *)e->قدر;
    nva[i].قدر_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  اگر(eos && !blen)
    دھارا->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              دھارا->send_closed);
  CURL_TRC_CF(ڈیٹا, cf, "quiche_send_request() -> %" PRId64, rv);
  اگر(rv < 0) {
    اگر(QUICHE_H3_ERR_دھارا_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] blocked", دھارا->id);
      دھارا->quic_flow_blocked = TRUE;
      نتیجہ = CURLE_AGAIN;
      جاؤ برآمد;
    }
    ورنہ {
      CURL_TRC_CF(ڈیٹا, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&ڈیٹا->state.url), rv);
    }
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ برآمد;
  }

  DEBUGASSERT(!دھارا->opened);
  دھارا->id = (uint64_t)rv;
  دھارا->opened = TRUE;
  دھارا->closed = FALSE;
  دھارا->reset = FALSE;

  اگر(Curl_trc_is_verbose(ڈیٹا)) {
    infof(ڈیٹا, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          دھارا->id, Curl_bufref_ptr(&ڈیٹا->state.url));
    برائے(i = 0; i < nheader; ++i) {
      infof(ڈیٹا, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", دھارا->id,
            (صحیح)nva[i].name_len, nva[i].name,
            (صحیح)nva[i].قدر_len, nva[i].قدر);
    }
  }

  اگر(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, ڈیٹا, دھارا, buf, blen, eos, &nwritten);
    اگر(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      نتیجہ = r2;
    }
    ورنہ اگر(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

برآمد:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرخیاں);
  واپس نتیجہ;
}

ساکن CURLcode cf_quiche_send(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
  CURLcode نتیجہ;

  *pnwritten = 0;
  Curl_vquic_ctx_update_وقت(&ctx->q, Curl_pgrs_now(ڈیٹا));

  نتیجہ = cf_process_ingress(cf, ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

  اگر(!دھارا || !دھارا->opened) {
    نتیجہ = h3_open_دھارا(cf, ڈیٹا, buf, len, eos, pnwritten);
    اگر(نتیجہ)
      جاؤ برآمد;
    دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
  }
  ورنہ اگر(دھارا->xfer_نتیجہ) {
    cf_quiche_دھارا_close(cf, ڈیٹا, دھارا);
    نتیجہ = دھارا->xfer_نتیجہ;
  }
  ورنہ اگر(دھارا->closed) {
    اگر(دھارا->resp_hds_complete) {
      /* sending request body on a stream that has been closed by the
       * server. If the server has send us a final response, we should
       * silently discard the send data.
       * This happens for example on redirects where the server, instead
       * of reading the full request body closed the stream after
       * sending the 30x response.
       * This is sort of a race: had the transfer loop called recv first,
       * it would see the response and stop/discard sending on its own- */
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] discarding data "
                  "on closed stream with response", دھارا->id);
      نتیجہ = CURLE_OK;
      *pnwritten = len;
      جاؤ برآمد;
    }
    CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", دھارا->id, len);
    نتیجہ = CURLE_HTTP3;
    جاؤ برآمد;
  }
  ورنہ {
    نتیجہ = cf_quiche_send_body(cf, ڈیٹا, دھارا, buf, len, eos, pnwritten);
  }

برآمد:
  نتیجہ = Curl_1st_fatal(نتیجہ, cf_flush_egress(cf, ڈیٹا));

  CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              دھارا ? دھارا->id : (uint64_t)~0, len,
              (صحیح)نتیجہ, *pnwritten);
  واپس نتیجہ;
}

ساکن bool دھارا_is_writable(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);

  واپس دھارا && (quiche_conn_دھارا_writable(
    ctx->qconn, دھارا->id, 1) > 0);
}

ساکن CURLcode cf_quiche_adjust_pollset(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا,
                                         ساخت easy_pollset *ps)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!ctx->qconn)
    واپس CURLE_OK;

  Curl_pollset_check(ڈیٹا, ps, ctx->q.sockfd, &want_recv, &want_send);
  اگر(want_recv || want_send) {
    ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
    bool c_exhaust, s_exhaust;

    c_exhaust = FALSE; /* Have not found any call in quiche that tells
                          us if the connection itself is blocked */
    s_exhaust = want_send && دھارا && دھارا->opened &&
                (دھارا->quic_flow_blocked || !دھارا_is_writable(cf, ڈیٹا));
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_خالی_خانے(&ctx->q.sendbuf);

    نتیجہ = Curl_pollset_سیٹ(ڈیٹا, ps, ctx->q.sockfd, want_recv, want_send);
  }
  واپس نتیجہ;
}

ساکن CURLcode h3_ڈیٹا_pause(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (خالی)cf;
  اگر(!pause) {
    Curl_multi_mark_dirty(ڈیٹا);
  }
  واپس CURLE_OK;
}

ساکن CURLcode cf_quiche_cntrl(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                صحیح event, صحیح arg1, خالی *arg2)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  (خالی)arg1;
  (خالی)arg2;
  انتخاب(event) {
  حالت CF_CTRL_ڈیٹا_تیاری:
    توڑو;
  حالت CF_CTRL_ڈیٹا_PAUSE:
    نتیجہ = h3_ڈیٹا_pause(cf, ڈیٹا, (arg1 != 0));
    توڑو;
  حالت CF_CTRL_ڈیٹا_مکمل:
    h3_ڈیٹا_مکمل(cf, ڈیٹا);
    توڑو;
  حالت CF_CTRL_ڈیٹا_مکمل_SEND: {
    ساخت h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, ڈیٹا);
    اگر(دھارا && !دھارا->send_closed) {
      غیرعلامتی حرف body[1];
      size_t بھیجا;

      دھارا->send_closed = TRUE;
      body[0] = 'X';
      نتیجہ = cf_quiche_send(cf, ڈیٹا, body, 0, TRUE, &بھیجا);
      CURL_TRC_CF(ڈیٹا, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  دھارا->id, (صحیح)نتیجہ, بھیجا);
    }
    توڑو;
  }
  حالت CF_CTRL_CONN_INFO_UPDATE:
    اگر(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_نظر_آیا = 30;
      Curl_conn_سیٹ_multiplex(cf->conn);
    }
    توڑو;
  حالت CF_CTRL_REPORT_STATS:
    اگر(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrs_وقت_Was(ڈیٹا, TIMER_CONNECT, ctx->q.پہلا_byte_at);
      Curl_pgrs_وقت_Was(ڈیٹا, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    توڑو;
  طےشدہ:
    توڑو;
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_quiche_ctx_open(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  صحیح rv;
  CURLcode نتیجہ;
  ثابت ساخت Curl_sockaddr_ex *sockaddr;
  ساکن ثابت ساخت alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  نتیجہ = Curl_vquic_ctx_init(ڈیٹا, &ctx->q);
  اگر(نتیجہ)
    واپس نتیجہ;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_نسخہ);
  اگر(!ctx->cfg) {
    failf(ڈیٹا, "cannot create quiche config");
    واپس CURLE_FAILED_INIT;
  }
  quiche_config_enable_pacing(ctx->cfg, FALSE);
  quiche_config_سیٹ_initial_max_ڈیٹا(ctx->cfg, (1 * 1024 * 1024)
    /* (QUIC_MAX_STREAMS/2) * H3_STREAM_WINDOW_SIZE */);
  quiche_config_سیٹ_initial_max_streams_bidi(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_سیٹ_initial_max_streams_uni(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_سیٹ_initial_max_دھارا_ڈیٹا_bidi_local(ctx->cfg,
    H3_دھارا_کھڑکی_سائز);
  quiche_config_سیٹ_initial_max_دھارا_ڈیٹا_bidi_remote(ctx->cfg,
    H3_دھارا_کھڑکی_سائز);
  quiche_config_سیٹ_initial_max_دھارا_ڈیٹا_uni(ctx->cfg,
    H3_دھارا_کھڑکی_سائز);
  quiche_config_سیٹ_disable_فعال_migration(ctx->cfg, TRUE);

  quiche_config_سیٹ_max_connection_کھڑکی(ctx->cfg,
    10 * QUIC_MAX_STREAMS * H3_دھارا_کھڑکی_سائز);
  quiche_config_سیٹ_max_دھارا_کھڑکی(ctx->cfg, 10 * H3_دھارا_کھڑکی_سائز);
  quiche_config_سیٹ_application_protos(ctx->cfg,
                      (uint8_t *)CURL_UNCONST(QUICHE_H3_APPLICATION_PROTOCOL),
                                 CURL_CSTRLEN(QUICHE_H3_APPLICATION_PROTOCOL));

  نتیجہ = Curl_vquic_tls_init(&ctx->tls, cf, ڈیٹا, &ctx->ssl_peer,
                               &ALPN_SPEC_H3, تہی, تہی, cf, تہی);
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = Curl_rand(ڈیٹا, ctx->scid, حجم(ctx->scid));
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(Curl_cf_socket_peek(cf->اگلا, ڈیٹا, &ctx->q.sockfd, &sockaddr, تہی))
    واپس CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = حجم(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (ساخت sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  اگر(rv == -1)
    واپس CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((ثابت uint8_t *)ctx->scid,
                                        حجم(ctx->scid), تہی, 0,
                                        (ساخت sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  اگر(!ctx->qconn) {
    failf(ڈیٹا, "cannot create quiche connection");
    واپس CURLE_برآمد_کا_حافظہ;
  }

  /* Known to not work on Windows */
#اگر !defined(_WIN32) && defined(HAVE_QUICHE_CONN_سیٹ_QLOG_FD)
  {
    صحیح qfd;
    (خالی)Curl_qlogdir(ڈیٹا, ctx->scid, حجم(ctx->scid), &qfd);
    اگر(qfd != -1)
      quiche_conn_سیٹ_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#اگر_ختم

  نتیجہ = cf_flush_egress(cf, ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;

  {
    غیرعلامتی حرف alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    غیرعلامتی alpn_len, انحراف = 0;

    /* Replace each ALPN length prefix by a comma. */
    جب(انحراف < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[انحراف];
      alpn_protocols[انحراف] = ',';
      انحراف += 1 + alpn_len;
    }

    CURL_TRC_CF(ڈیٹا, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  واپس CURLE_OK;
}

ساکن CURLcode cf_quiche_verify_peer(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  واپس Curl_vquic_tls_verify_peer(&ctx->tls, cf, ڈیٹا, &ctx->ssl_peer);
}

ساکن CURLcode cf_quiche_connect(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  bool *مکمل)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  /* Connect the UDP filter first */
  اگر(!cf->اگلا->connected) {
    نتیجہ = Curl_conn_cf_connect(cf->اگلا, ڈیٹا, مکمل);
    اگر(نتیجہ || !*مکمل)
      واپس نتیجہ;
  }

  *مکمل = FALSE;
  اگر(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(ڈیٹا, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(ڈیٹا, cf, "need HTTPS-RR, delaying connect");
    واپس CURLE_OK;
  }

  Curl_vquic_ctx_update_وقت(&ctx->q, Curl_pgrs_now(ڈیٹا));

  اگر(!ctx->qconn) {
    نتیجہ = cf_quiche_ctx_open(cf, ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
    ctx->started_at = ctx->q.آخری_op;
    نتیجہ = cf_flush_egress(cf, ڈیٹا);
    /* we do not expect to be able to recv anything yet */
    جاؤ برآمد;
  }

  نتیجہ = cf_process_ingress(cf, ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = cf_flush_egress(cf, ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;

  اگر(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.آخری_op;
    CURL_TRC_CF(ڈیٹا, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    نتیجہ = cf_quiche_verify_peer(cf, ڈیٹا);
    اگر(!نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      اگر(!ctx->h3config) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_سیٹ_max_میدان_section_سائز(ctx->h3config,
                                                  MAX_HTTP_RESP_سرخی_سائز);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      اگر(!ctx->h3c) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
      cf->connected = TRUE;
      *مکمل = TRUE;
    }
  }
  ورنہ اگر(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
  }

برآمد:
#اگر_تعریف_شدہ CURLVERBOSE
  اگر(نتیجہ && نتیجہ != CURLE_AGAIN) {
    ساخت ip_quadruple ip;

    اگر(!Curl_cf_socket_peek(cf->اگلا, ڈیٹا, تہی, تہی, &ip))
      infof(ڈیٹا, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_پورٹ, curl_easy_strerror(نتیجہ));
    ورنہ
      infof(ڈیٹا, "connect failed: %s", curl_easy_strerror(نتیجہ));
  }
#اگر_ختم
  واپس نتیجہ;
}

ساکن CURLcode cf_quiche_shutdown(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  اگر(cf->shutdown || !ctx || !ctx->qconn) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  *مکمل = FALSE;
  اگر(!ctx->shutdown_started) {
    صحیح err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_وقت(&ctx->q, Curl_pgrs_now(ڈیٹا));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, تہی, 0);
    اگر(err) {
      CURL_TRC_CF(ڈیٹا, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
  }

  اگر(!Curl_bufq_is_خالی_خانے(&ctx->q.sendbuf)) {
    CURL_TRC_CF(ڈیٹا, cf, "shutdown, flushing sendbuf");
    نتیجہ = cf_flush_egress(cf, ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  اگر(Curl_bufq_is_خالی_خانے(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(ڈیٹا, cf, "shutdown completely sent off, done");
    *مکمل = TRUE;
  }
  ورنہ {
    CURL_TRC_CF(ڈیٹا, cf, "shutdown sending blocked");
  }

برآمد:
  واپس نتیجہ;
}

ساکن خالی cf_quiche_destroy(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  (خالی)ڈیٹا;
  اگر(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = تہی;
  }
}

ساکن CURLcode cf_quiche_query(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                صحیح query, صحیح *pres1, خالی *pres2)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;

  انتخاب(query) {
  حالت CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    اگر(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_بایاں_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > زیادہ_سے_زیادہ_عدد) ? زیادہ_سے_زیادہ_عدد : (صحیح)max_streams;
    CURL_TRC_CF(ڈیٹا, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    واپس CURLE_OK;
  }
  حالت CF_QUERY_CONNECT_جواب_MS:
    اگر(ctx->q.got_پہلا_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.پہلا_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < زیادہ_سے_زیادہ_عدد) ? (صحیح)ms : زیادہ_سے_زیادہ_عدد;
    }
    ورنہ
      *pres1 = -1;
    واپس CURLE_OK;
  حالت CF_QUERY_HTTP_نسخہ:
    *pres1 = 30;
    واپس CURLE_OK;
  حالت CF_QUERY_SSL_INFO:
  حالت CF_QUERY_SSL_CTX_INFO: {
    ساخت curl_tlssessioninfo *info = pres2;
    اگر(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      واپس CURLE_OK;
    توڑو;
  }
  حالت CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : تہی;
    واپس CURLE_OK;
  }
  طےشدہ:
    توڑو;
  }
  واپس cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, ڈیٹا, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساکن bool cf_quiche_conn_is_alive(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    bool *داخلہ_pending)
{
  ساخت cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *داخلہ_pending = FALSE;
  اگر(!ctx->qconn)
    واپس FALSE;

  اگر(quiche_conn_is_closed(ctx->qconn)) {
    اگر(quiche_conn_is_timed_برآمد(ctx->qconn))
      CURL_TRC_CF(ڈیٹا, cf, "connection was closed due to idle timeout");
    ورنہ
      CURL_TRC_CF(ڈیٹا, cf, "connection is closed");
    واپس FALSE;
  }

  اگر(!cf->اگلا || !cf->اگلا->cft->is_alive(cf->اگلا, ڈیٹا, داخلہ_pending))
    واپس FALSE;

  اگر(*داخلہ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *داخلہ_pending = FALSE;
    اگر(cf_process_ingress(cf, ڈیٹا))
      alive = FALSE;
    ورنہ {
      alive = TRUE;
    }
  }

  واپس alive;
}

ساخت Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_quiche_destroy,
  cf_quiche_connect,
  cf_quiche_shutdown,
  cf_quiche_adjust_pollset,
  Curl_cf_def_ڈیٹا_pending,
  cf_quiche_send,
  cf_quiche_recv,
  cf_quiche_cntrl,
  cf_quiche_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_quiche_query,
};

CURLcode Curl_cf_quiche_create(ساخت Curl_cfilter **pcf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_peer *origin,
                               ساخت Curl_peer *peer,
                               ساخت connectdata *conn,
                               ساخت Curl_sockaddr_ex *addr)
{
  ساخت cf_quiche_ctx *ctx = تہی;
  ساخت Curl_cfilter *cf = تہی;
  CURLcode نتیجہ;

  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  نتیجہ = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  اگر(!نتیجہ)
    نتیجہ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;
  cf->conn = conn;

  نتیجہ = Curl_cf_udp_create(&cf->اگلا, ڈیٹا, origin, peer, TRNSPRT_QUIC,
                              conn, addr, تہی, TRNSPRT_QUIC);
  اگر(نتیجہ)
    جاؤ برآمد;
  cf->اگلا->conn = cf->conn;
  cf->اگلا->sockindex = cf->sockindex;

برآمد:
  *pcf = (!نتیجہ) ? cf : تہی;
  اگر(نتیجہ) {
    اگر(cf)
      Curl_conn_cf_discard_chain(&cf, ڈیٹا);
    ورنہ اگر(ctx)
      cf_quiche_ctx_free(ctx);
  }

  واپس نتیجہ;
}

CURLcode Curl_cf_quiche_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                     ساخت Curl_peer *origin,
                                     ساخت Curl_peer *peer,
                                     ساخت Curl_cfilter **pcf)
{
  ساخت cf_quiche_ctx *ctx = تہی;
  ساخت Curl_cfilter *cf = تہی;
  CURLcode نتیجہ;

  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  نتیجہ = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  اگر(!نتیجہ)
    نتیجہ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);

برآمد:
  اگر(نتیجہ) {
    curlx_safefree(cf);
    اگر(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  واپس نتیجہ;
}

#اگر_ختم
