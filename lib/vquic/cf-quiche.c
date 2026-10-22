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

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
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
#تعریف_کرو CURL_H3_NO_ERROR  0x0100

#تعریف_کرو MAX_PKT_مدت          10

#تعریف_کرو QUIC_MAX_STREAMS       100

#تعریف_کرو H3_دھارا_کھڑکی_ناپ  (1024 * 128)
#تعریف_کرو H3_دھارا_CHUNK_ناپ   (1024 * 16)
/* Receive and Send max number of chunks follows from the
 * chunk size and window size */
#تعریف_کرو H3_دھارا_RECV_CHUNKS \
  (H3_دھارا_کھڑکی_ناپ / H3_دھارا_CHUNK_ناپ)

/*
 * Store quiche version info in this buffer.
 */
فراغ Curl_quiche_ver(حرف *p, size_t len)
{
  (فراغ)curl_msnprintf(p, len, "quiche/%s", quiche_نسخہ());
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
  curl_off_t انکڑے_recvd;
  ثنائی_ہندسہ(initialized);
  ثنائی_ہندسہ(goaway);                       /* got GOAWAY from server */
  ثنائی_ہندسہ(x509_store_تیاری);             /* if x509 store has been set up */
  ثنائی_ہندسہ(shutdown_started);             /* queued shutdown packets */
  ثنائی_ہندسہ(stats_reported);               /* connect statistics reported */
};

#جے_تعریف_شدہ DEBUG_QUICHE
/* initialize debug log callback only once */
ساكن صحيح debug_log_init = 0;
ساكن فراغ quiche_debug_log(ثابت حرف *سطر, فراغ *argp)
{
  (فراغ)argp;
  curl_mfprintf(stderr, "%s\n", سطر);
}
#جے_انت

ساكن فراغ h3_دھارا_سیٹ_free(uint32_t id, فراغ *دھارا);

ساكن CURLcode cf_quiche_ctx_init(بنية cf_quiche_ctx *ctx,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer,
                                   بنية ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#جے_تعریف_شدہ DEBUG_QUICHE
  إذا(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, عدم);
    debug_log_init = 1;
  }
#جے_انت
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_سرخی);
  Curl_u32_ptrset_init(&ctx->streams, h3_دھارا_سیٹ_free);
  Curl_bufq_init2(&ctx->writebuf, H3_دھارا_CHUNK_ناپ, H3_دھارا_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندی);
  ctx->انکڑے_recvd = 0;
  ctx->initialized = TRUE;
  أرجع Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

ساكن فراغ cf_quiche_ctx_free(بنية cf_quiche_ctx *ctx)
{
  إذا(ctx && ctx->initialized) {
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
  إذا(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = عدم;
  }
  إذا(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = عدم;
  }
  إذا(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = عدم;
  }
  إذا(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = عدم;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

ساكن CURLcode cf_flush_egress(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے);

/**
 * All about the H3 internals of a stream
 */
بنية h3_دھارا_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  بنية h1_req_parser h1; /* h1 request parsing */
  uint64_t error3;         /* HTTP/3 stream error code */
  int32_t حالت_کوڈ;     /* HTTP status code */
  CURLcode xfer_نتیجہ;    /* result from cf_quiche_write_(hd/body) */
  ثنائی_ہندسہ(opened);             /* TRUE after stream has been opened */
  ثنائی_ہندسہ(closed);             /* TRUE on stream close */
  ثنائی_ہندسہ(reset);              /* TRUE on stream reset */
  ثنائی_ہندسہ(send_closed);        /* stream is locally closed */
  ثنائی_ہندسہ(resp_hds_complete);  /* final response has been received */
  ثنائی_ہندسہ(resp_got_سرخی);    /* TRUE when h3 stream has recvd some HEADER */
  ثنائی_ہندسہ(quic_flow_blocked);  /* stream is blocked by QUIC flow control */
};

ساكن فراغ h3_دھارا_ctx_free(بنية h3_دھارا_ctx *دھارا)
{
  Curl_h1_req_parse_free(&دھارا->h1);
  curlx_free(دھارا);
}

ساكن فراغ h3_دھارا_سیٹ_free(uint32_t id, فراغ *دھارا)
{
  (فراغ)id;
  DEBUGASSERT(دھارا);
  h3_دھارا_ctx_free((بنية h3_دھارا_ctx *)دھارا);
}

تعريفنوع bool cf_quiche_svisit(بنية Curl_cfilter *cf,
                              بنية Curl_easy *sdata,
                              بنية h3_دھارا_ctx *دھارا,
                              فراغ *user_انکڑے);

بنية cf_quiche_ویکھو_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_multi *multi;
  cf_quiche_svisit *cb;
  فراغ *user_انکڑے;
};

ساكن bool cf_quiche_دھارا_do(uint32_t mid, فراغ *val, فراغ *user_انکڑے)
{
  بنية cf_quiche_ویکھو_ctx *vctx = user_انکڑے;
  بنية h3_دھارا_ctx *دھارا = val;
  بنية Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  إذا(sdata)
    أرجع vctx->cb(vctx->cf, sdata, دھارا, vctx->user_انکڑے);
  أرجع TRUE;
}

ساكن فراغ cf_quiche_for_all_streams(بنية Curl_cfilter *cf,
                                      بنية Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      فراغ *user_انکڑے)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية cf_quiche_ویکھو_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_انکڑے = user_انکڑے;
  Curl_u32_ptrset_ویکھو(&ctx->streams, cf_quiche_دھارا_do, &vctx);
}

ساكن bool cf_quiche_do_resume(بنية Curl_cfilter *cf,
                                بنية Curl_easy *sdata,
                                بنية h3_دھارا_ctx *دھارا,
                                فراغ *user_انکڑے)
{
  (فراغ)user_انکڑے;
  إذا(دھارا->quic_flow_blocked) {
    دھارا->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", دھارا->id);
  }
  أرجع TRUE;
}

ساكن bool cf_quiche_do_expire(بنية Curl_cfilter *cf,
                                بنية Curl_easy *sdata,
                                بنية h3_دھارا_ctx *دھارا,
                                فراغ *user_انکڑے)
{
  (فراغ)user_انکڑے;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  دھارا->xfer_نتیجہ = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  أرجع TRUE;
}

ساكن CURLcode h3_انکڑے_تیاری(بنية Curl_cfilter *cf,
                              بنية Curl_easy *انکڑے)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);

  إذا(دھارا)
    أرجع CURLE_OK;

  دھارا = curlx_calloc(1, حجم(*دھارا));
  إذا(!دھارا)
    أرجع CURLE_باہرلا_دا_حافظہ;

  دھارا->id = -1;
  Curl_h1_req_parse_init(&دھارا->h1, H1_PARSE_DEFAULT_MAX_سطر_LEN);

  إذا(Curl_u32_ptrset_سیٹ(&ctx->streams, انکڑے->mid, دھارا)) {
    h3_دھارا_ctx_free(دھارا);
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  أرجع CURLE_OK;
}

ساكن فراغ cf_quiche_دھارا_close(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے,
                                   بنية h3_دھارا_ctx *دھارا)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;

  إذا(ctx->qconn && !دھارا->closed) {
    quiche_conn_دھارا_shutdown(ctx->qconn, دھارا->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    إذا(!دھارا->send_closed) {
      quiche_conn_دھارا_shutdown(ctx->qconn, دھارا->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      دھارا->send_closed = TRUE;
    }
    دھارا->closed = TRUE;
    نتیجہ = cf_flush_egress(cf, انکڑے);
    إذا(نتیجہ)
      CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  دھارا->id, (صحيح)نتیجہ);
  }
}

ساكن فراغ h3_انکڑے_پورا(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);

  (فراغ)cf;
  إذا(دھارا) {
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] easy handle is done", دھارا->id);
    cf_quiche_دھارا_close(cf, انکڑے, دھارا);
    Curl_u32_ptrset_unset(&ctx->streams, انکڑے->mid);
  }
}

ساكن فراغ cf_quiche_expire_conn_closed(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *انکڑے)
{
  DEBUGASSERT(انکڑے->multi);
  CURL_TRC_CF(انکڑے, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, انکڑے->multi, cf_quiche_do_expire, عدم);
}

ساكن فراغ cf_quiche_write_hd(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے,
                               بنية h3_دھارا_ctx *دھارا,
                               ثابت حرف *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  إذا(!دھارا->xfer_نتیجہ) {
    دھارا->xfer_نتیجہ = Curl_xfer_write_resp_hd(انکڑے, buf, blen, eos);
    إذا(دھارا->xfer_نتیجہ)
      CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", دھارا->id, (صحيح)دھارا->xfer_نتیجہ,
                  blen);
  }
}

بنية cb_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *انکڑے;
  بنية h3_دھارا_ctx *دھارا;
};

ساكن bool is_valid_h3_سرخی(ثابت uint8_t *hdr, size_t hlen)
{
  طالما(hlen--) {
    بدل(*hdr++) {
    حالة '\n':
    حالة '\r':
    حالة '\0':
      أرجع FALSE;
    }
  }
  أرجع TRUE;
}

ساكن صحيح cb_each_سرخی(uint8_t *name, size_t name_len,
                          uint8_t *قدر, size_t قدر_len,
                          فراغ *argp)
{
  بنية cb_ctx *x = argp;
  بنية Curl_cfilter *cf = x->cf;
  بنية Curl_easy *انکڑے = x->انکڑے;
  بنية h3_دھارا_ctx *دھارا = x->دھارا;
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  إذا(!دھارا || دھارا->xfer_نتیجہ)
    أرجع 1; /* abort iteration */

  إذا((name_len == 7) && !لڑی_مقابلہ(HTTP_PSEUDO_حالت, (حرف *)name, 7) &&
     is_valid_h3_سرخی(قدر, قدر_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    نتیجہ = Curl_http_decode_حالت(&دھارا->حالت_کوڈ,
                                     (ثابت حرف *)قدر, قدر_len);
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)قدر, قدر_len);
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    إذا(!نتیجہ)
      cf_quiche_write_hd(cf, انکڑے, دھارا, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] status: %s",
                دھارا->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  وإلا {
    إذا(is_valid_h3_سرخی(قدر, قدر_len) &&
       is_valid_h3_سرخی(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  دھارا->id, (صحيح)name_len, name,
                  (صحيح)قدر_len, قدر);
      curlx_dyn_reset(&ctx->h1hdr);
      نتیجہ = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)name, name_len);
      إذا(!نتیجہ)
        نتیجہ = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      إذا(!نتیجہ)
        نتیجہ = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)قدر, قدر_len);
      إذا(!نتیجہ)
        نتیجہ = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      إذا(!نتیجہ)
        cf_quiche_write_hd(cf, انکڑے, دھارا, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    وإلا
      CURL_TRC_CF(x->انکڑے, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  دھارا->id, قدر_len + name_len);
  }

  إذا(نتیجہ) {
    CURL_TRC_CF(x->انکڑے, x->cf, "[%" PRIu64 "] on header error %d",
                دھارا->id, (صحيح)نتیجہ);
    إذا(!دھارا->xfer_نتیجہ)
      دھارا->xfer_نتیجہ = نتیجہ;
  }
  أرجع دھارا->xfer_نتیجہ ? 1 : 0;
}

ساكن CURLcode دھارا_resp_read(فراغ *reader_ctx,
                                 بنانشانی حرف *buf, size_t len,
                                 size_t *pnread)
{
  بنية cb_ctx *x = reader_ctx;
  بنية cf_quiche_ctx *ctx = x->cf->ctx;
  بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, x->انکڑے);
  ssize_t nread;

  *pnread = 0;
  إذا(!دھارا)
    أرجع CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, دھارا->id, buf, len);
  إذا(!curlx_sztouz(nread, pnread))
    أرجع CURLE_AGAIN;
  أرجع CURLE_OK;
}

ساكن فراغ cf_quiche_flush_body(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 بنية h3_دھارا_ctx *دھارا)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  ثابت uint8_t *buf;
  size_t blen;

  طالما(دھارا && !دھارا->xfer_نتیجہ) {
    إذا(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      دھارا->xfer_نتیجہ = Curl_xfer_write_resp(
        انکڑے, (ثابت حرف *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      إذا(دھارا->xfer_نتیجہ) {
        CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", دھارا->id, (صحيح)دھارا->xfer_نتیجہ, blen);
      }
    }
    وإلا
      اكسر;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

ساكن فراغ cf_quiche_recv_body(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                بنية h3_دھارا_ctx *دھارا)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  بنية cb_ctx cb_ctx;
  CURLcode نتیجہ = CURLE_OK;

  إذا(!دھارا)
    أرجع;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.انکڑے = انکڑے;
  cb_ctx.دھارا = دھارا;
  Curl_bufq_reset(&ctx->writebuf);
  طالما(!نتیجہ) {
    نتیجہ = Curl_bufq_slurp(&ctx->writebuf,
                             دھارا_resp_read, &cb_ctx, &nread);
    إذا(!نتیجہ)
      cf_quiche_flush_body(cf, انکڑے, دھارا);
    وإلا إذا(نتیجہ == CURLE_AGAIN)
      اكسر;
    وإلا إذا(نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] recv_body error %d",
                  دھارا->id, (صحيح)نتیجہ);
      failf(انکڑے, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            دھارا->id, (صحيح)نتیجہ);
      دھارا->closed = TRUE;
      دھارا->reset = TRUE;
      دھارا->send_closed = TRUE;
      إذا(!دھارا->xfer_نتیجہ)
        دھارا->xfer_نتیجہ = نتیجہ;
    }
  }
  cf_quiche_flush_body(cf, انکڑے, دھارا);
}

ساكن فراغ cf_quiche_process_ev(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 بنية h3_دھارا_ctx *دھارا,
                                 quiche_h3_event *ev)
{
  تعداد quiche_h3_event_type type;

  إذا(!دھارا)
    أرجع;

  type = quiche_h3_event_type(ev);

  بدل(type) {
  حالة QUICHE_H3_EVENT_سرخیاں: {
    بنية cb_ctx cb_ctx;
    دھارا->resp_got_سرخی = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.انکڑے = انکڑے;
    cb_ctx.دھارا = دھارا;
    quiche_h3_event_for_each_سرخی(ev, cb_each_سرخی, &cb_ctx);
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] <- [HEADERS]", دھارا->id);
    Curl_multi_mark_dirty(انکڑے);
    اكسر;
  }
  حالة QUICHE_H3_EVENT_انکڑے:
    إذا(!دھارا->resp_hds_complete) {
      دھارا->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, انکڑے, دھارا, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, انکڑے, دھارا);
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] <- [DATA]", دھارا->id);
    Curl_multi_mark_dirty(انکڑے);
    اكسر;

  حالة QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] RESET", دھارا->id);
    دھارا->closed = TRUE;
    دھارا->reset = TRUE;
    دھارا->send_closed = TRUE;
    Curl_multi_mark_dirty(انکڑے);
    اكسر;

  حالة QUICHE_H3_EVENT_مکمل:
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] CLOSED", دھارا->id);
    إذا(!دھارا->resp_hds_complete) {
      دھارا->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, انکڑے, دھارا, "\r\n", 2, TRUE);
    }
    دھارا->closed = TRUE;
    Curl_multi_mark_dirty(انکڑے);
    اكسر;

  حالة QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] <- [GOAWAY]", دھارا->id);
    اكسر;

  افتراضي:
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] recv, unhandled event %d",
                دھارا->id, (صحيح)type);
    اكسر;
  }
}

بنية cf_quiche_disp_ctx {
  uint64_t دھارا_id;
  بنية Curl_cfilter *cf;
  بنية Curl_multi *multi;
  quiche_h3_event *ev;
};

ساكن bool cf_quiche_disp_event(uint32_t mid, فراغ *val, فراغ *user_انکڑے)
{
  بنية cf_quiche_disp_ctx *dctx = user_انکڑے;
  بنية h3_دھارا_ctx *دھارا = val;

  إذا(دھارا->id == dctx->دھارا_id) {
    بنية Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    إذا(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, دھارا, dctx->ev);
    أرجع FALSE; /* stop iterating */
  }
  أرجع TRUE;
}

ساكن CURLcode cf_poll_events(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = عدم;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  طالما(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    إذا(rv == QUICHE_H3_ERR_پورا) {
      اكسر;
    }
    وإلا إذا(rv < 0) {
      CURL_TRC_CF(انکڑے, cf, "error poll: %" PRId64, rv);
      أرجع CURLE_HTTP3;
    }
    وإلا {
      دھارا = H3_دھارا_CTX(ctx, انکڑے);
      إذا(دھارا && دھارا->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, انکڑے, دھارا, ev);
        quiche_h3_event_free(ev);
        إذا(دھارا->xfer_نتیجہ)
          أرجع دھارا->xfer_نتیجہ;
      }
      وإلا {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        بنية cf_quiche_disp_ctx dctx;
        dctx.دھارا_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = انکڑے->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_ویکھو(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  أرجع CURLE_OK;
}

بنية recv_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *انکڑے;
  صحيح pkts;
};

ساكن CURLcode cf_quiche_recv_pkts(ثابت بنانشانی حرف *buf, size_t buflen,
                                    size_t gso_ناپ,
                                    بنية sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    فراغ *userp)
{
  بنية recv_ctx *r = userp;
  بنية cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, وتھ, nread;
  ssize_t rv;

  (فراغ)ecn;

  recv_info.منزل = (بنية sockaddr *)&ctx->q.local_addr;
  recv_info.منزل_len = ctx->q.local_addrlen;
  recv_info.from = (بنية sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  لكل(وتھ = 0; وتھ < buflen; وتھ += gso_ناپ) {
    pktlen = ((وتھ + gso_ناپ) <= buflen) ? gso_ناپ : (buflen - وتھ);
    rv = quiche_conn_recv(ctx->qconn,
                          (بنانشانی حرف *)CURL_UNCONST(buf + وتھ),
                          pktlen, &recv_info);
    إذا(!curlx_sztouz(rv, &nread)) {
      إذا(QUICHE_ERR_پورا == rv) {
        إذا(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->انکڑے, r->cf, "ingress, connection is draining");
          أرجع CURLE_RECV_ERROR;
        }
        إذا(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->انکڑے, r->cf, "ingress, connection is closed");
          أرجع CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->انکڑے, r->cf, "ingress, quiche is DONE");
        أرجع CURLE_OK;
      }
      وإلا إذا(QUICHE_ERR_TLS_FAIL == rv) {
        لما verify_ok = SSL_get_verify_نتیجہ(ctx->tls.ossl.ssl);
        إذا(verify_ok != X509_V_OK) {
          failf(r->انکڑے, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          أرجع CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->انکڑے, "ingress, quiche reports TLS fail");
        أرجع CURLE_RECV_ERROR;
      }
      وإلا {
        failf(r->انکڑے, "quiche reports error %zd on receive", rv);
        أرجع CURLE_RECV_ERROR;
      }
    }
    وإلا إذا(nread < pktlen) {
      CURL_TRC_CF(r->انکڑے, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode cf_process_ingress(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية recv_ctx rctx;
  CURLcode نتیجہ;

  DEBUGASSERT(ctx->qconn);
  نتیجہ = Curl_vquic_tls_before_recv(&ctx->tls, cf, انکڑے);
  إذا(نتیجہ)
    أرجع نتیجہ;

  rctx.cf = cf;
  rctx.انکڑے = انکڑے;
  rctx.pkts = 0;

  نتیجہ = Curl_vquic_recv_packets(cf, انکڑے, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  إذا(نتیجہ)
    أرجع نتیجہ;

  إذا(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(انکڑے->multi);
    cf_quiche_for_all_streams(cf, انکڑے->multi, cf_quiche_do_resume, عدم);
  }
  أرجع cf_poll_events(cf, انکڑے);
}

بنية read_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *انکڑے;
  quiche_send_info send_info;
};

ساكن CURLcode read_pkt_منزل_send(فراغ *userp,
                                 بنانشانی حرف *buf, size_t buflen,
                                 size_t *pnread)
{
  بنية read_ctx *x = userp;
  بنية cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  إذا(rv == QUICHE_ERR_پورا)
    أرجع CURLE_AGAIN;

  إذا(!curlx_sztouz(rv, pnread)) {
    failf(x->انکڑے, "quiche_conn_send returned %zd", rv);
    أرجع CURLE_SEND_ERROR;
  }
  أرجع CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
ساكن CURLcode cf_flush_egress(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode نتیجہ;
  int64_t expiry_ns;
  int64_t timeout_ns;
  بنية read_ctx readx;
  size_t pkt_گنتی, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  إذا(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    إذا(quiche_conn_is_closed(ctx->qconn)) {
      إذا(quiche_conn_is_timed_باہرلا(ctx->qconn))
        failf(انکڑے, "connection closed by idle timeout");
      وإلا
        failf(انکڑے, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, انکڑے);
      أرجع CURLE_SEND_ERROR;
    }
  }

  نتیجہ = Curl_vquic_flush(cf, انکڑے, &ctx->q);
  إذا(نتیجہ) {
    إذا(نتیجہ == CURLE_AGAIN) {
      Curl_expire(انکڑے, 1, EXPIRE_QUIC);
      أرجع CURLE_OK;
    }
    أرجع نتیجہ;
  }

  readx.cf = cf;
  readx.انکڑے = انکڑے;
  memset(&readx.send_info, 0, حجم(readx.send_info));
  pkt_گنتی = 0;
  gsolen = quiche_conn_max_send_udp_payload_ناپ(ctx->qconn);
  لكل(;;) {
    /* add the next packet to send, if any, to our buffer */
    نتیجہ = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_منزل_send, &readx, &nread);
    إذا(نتیجہ) {
      إذا(نتیجہ != CURLE_AGAIN)
        أرجع نتیجہ;
      /* Nothing more to add, flush and leave */
      نتیجہ = Curl_vquic_send(cf, انکڑے, &ctx->q, gsolen);
      إذا(نتیجہ) {
        إذا(نتیجہ == CURLE_AGAIN) {
          Curl_expire(انکڑے, 1, EXPIRE_QUIC);
          أرجع CURLE_OK;
        }
        أرجع نتیجہ;
      }
      اذهب باہرلا;
    }

    ++pkt_گنتی;
    إذا(nread < gsolen || pkt_گنتی >= MAX_PKT_مدت) {
      نتیجہ = Curl_vquic_send(cf, انکڑے, &ctx->q, gsolen);
      إذا(نتیجہ) {
        إذا(نتیجہ == CURLE_AGAIN) {
          Curl_expire(انکڑے, 1, EXPIRE_QUIC);
          أرجع CURLE_OK;
        }
        اذهب باہرلا;
      }
      pkt_گنتی = 0;
    }
  }

باہرلا:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  إذا(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(انکڑے, (timeout_ns / 1000000), EXPIRE_QUIC);
  أرجع نتیجہ;
}

ساكن CURLcode recv_closed_دھارا(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے,
                                   size_t *pnread)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(دھارا);
  *pnread = 0;
  إذا(دھارا->reset) {
    إذا(دھارا->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(انکڑے, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", دھارا->id);
      connclose(cf->conn); /* do not use this anymore */
      انکڑے->state.refused_دھارا = TRUE;
      أرجع CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وإلا إذا(دھارا->resp_hds_complete && انکڑے->req.no_body) {
        CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", دھارا->id, دھارا->error3,
                    Curl_vquic_h3_err_str(دھارا->error3));
        أرجع CURLE_OK;
    }
    failf(انکڑے, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", دھارا->id, دھارا->error3,
          Curl_vquic_h3_err_str(دھارا->error3));
    نتیجہ = انکڑے->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                دھارا->id, (صحيح)نتیجہ);
  }
  وإلا إذا(!دھارا->resp_got_سرخی) {
    failf(انکڑے, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          دھارا->id);
    نتیجہ = CURLE_HTTP3;
  }
  أرجع نتیجہ;
}

ساكن CURLcode cf_quiche_recv(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                               حرف *buf, size_t blen, size_t *pnread)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);
  CURLcode نتیجہ = CURLE_OK;

  *pnread = 0;
  (فراغ)buf;
  (فراغ)blen;
  Curl_vquic_ctx_update_ویلا(&ctx->q, Curl_pgrs_now(انکڑے));

  إذا(!دھارا)
    أرجع CURLE_RECV_ERROR;

  نتیجہ = cf_process_ingress(cf, انکڑے);
  إذا(نتیجہ) {
    CURL_TRC_CF(انکڑے, cf, "cf_recv, error on ingress");
    اذهب باہرلا;
  }

  إذا(دھارا->xfer_نتیجہ) {
    cf_quiche_دھارا_close(cf, انکڑے, دھارا);
    نتیجہ = دھارا->xfer_نتیجہ;
    اذهب باہرلا;
  }
  وإلا إذا(دھارا->closed)
    نتیجہ = recv_closed_دھارا(cf, انکڑے, pnread);
  وإلا إذا(quiche_conn_is_draining(ctx->qconn)) {
    failf(انکڑے, "QUIC connection is draining");
    نتیجہ = CURLE_HTTP3;
  }
  وإلا
    نتیجہ = CURLE_AGAIN;

باہرلا:
  نتیجہ = Curl_1st_fatal(نتیجہ, cf_flush_egress(cf, انکڑے));
  إذا(*pnread > 0)
    ctx->انکڑے_recvd += *pnread;
  CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, دھارا->id, blen, (صحيح)نتیجہ, *pnread,
              ctx->انکڑے_recvd);
  أرجع نتیجہ;
}

ساكن CURLcode cf_quiche_send_body(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے,
                                    بنية h3_دھارا_ctx *دھارا,
                                    ثابت uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, دھارا->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  إذا(rv == QUICHE_H3_ERR_پورا || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    إذا(!quiche_conn_دھارا_writable(ctx->qconn, دھارا->id, len)) {
      CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", دھارا->id, len);
      دھارا->quic_flow_blocked = TRUE;
    }
    أرجع CURLE_AGAIN;
  }
  وإلا إذا(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_دھارا_STATE) {
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", دھارا->id, len);
    أرجع CURLE_HTTP3;
  }
  وإلا إذا(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_ناپ) {
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                دھارا->id, len);
    أرجع CURLE_SEND_ERROR;
  }
  وإلا إذا(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                دھارا->id, len, rv);
    أرجع CURLE_SEND_ERROR;
  }
  وإلا {
    إذا(eos && (len == *pnwritten))
      دھارا->send_closed = TRUE;
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                دھارا->id, len, دھارا->send_closed, *pnwritten);
    أرجع CURLE_OK;
  }
}

ساكن CURLcode h3_open_دھارا(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے,
                               ثابت uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);
  size_t nheader, i;
  int64_t rv;
  بنية dynhds h2_سرخیاں;
  quiche_h3_سرخی *nva = عدم;
  CURLcode نتیجہ = CURLE_OK;

  *pnwritten = 0;
  إذا(!دھارا) {
    نتیجہ = h3_انکڑے_تیاری(cf, انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
    دھارا = H3_دھارا_CTX(ctx, انکڑے);
    DEBUGASSERT(دھارا);
  }

  Curl_dynhds_init(&h2_سرخیاں, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(دھارا);

  نتیجہ = Curl_h1_req_parse_read(&دھارا->h1, buf, blen, عدم,
    !انکڑے->state.http_ignorecustom ?
    CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST) : عدم,
    0, pnwritten);
  إذا(نتیجہ)
    اذهب باہرلا;
  إذا(!دھارا->h1.پورا) {
    /* need more data */
    اذهب باہرلا;
  }
  DEBUGASSERT(دھارا->h1.req);

  نتیجہ = Curl_http_req_منزل_h2(&h2_سرخیاں, دھارا->h1.req, انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

  /* no longer needed */
  Curl_h1_req_parse_free(&دھارا->h1);

  nheader = Curl_dynhds_گنتی(&h2_سرخیاں);
  nva = curlx_malloc(حجم(quiche_h3_سرخی) * nheader);
  إذا(!nva) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  لكل(i = 0; i < nheader; ++i) {
    بنية dynhds_اندراج *e = Curl_dynhds_getn(&h2_سرخیاں, i);
    nva[i].name = (بنانشانی حرف *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].قدر = (بنانشانی حرف *)e->قدر;
    nva[i].قدر_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  إذا(eos && !blen)
    دھارا->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              دھارا->send_closed);
  CURL_TRC_CF(انکڑے, cf, "quiche_send_request() -> %" PRId64, rv);
  إذا(rv < 0) {
    إذا(QUICHE_H3_ERR_دھارا_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] blocked", دھارا->id);
      دھارا->quic_flow_blocked = TRUE;
      نتیجہ = CURLE_AGAIN;
      اذهب باہرلا;
    }
    وإلا {
      CURL_TRC_CF(انکڑے, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&انکڑے->state.url), rv);
    }
    نتیجہ = CURLE_SEND_ERROR;
    اذهب باہرلا;
  }

  DEBUGASSERT(!دھارا->opened);
  دھارا->id = (uint64_t)rv;
  دھارا->opened = TRUE;
  دھارا->closed = FALSE;
  دھارا->reset = FALSE;

  إذا(Curl_trc_is_verbose(انکڑے)) {
    infof(انکڑے, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          دھارا->id, Curl_bufref_ptr(&انکڑے->state.url));
    لكل(i = 0; i < nheader; ++i) {
      infof(انکڑے, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", دھارا->id,
            (صحيح)nva[i].name_len, nva[i].name,
            (صحيح)nva[i].قدر_len, nva[i].قدر);
    }
  }

  إذا(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, انکڑے, دھارا, buf, blen, eos, &nwritten);
    إذا(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      نتیجہ = r2;
    }
    وإلا إذا(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

باہرلا:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرخیاں);
  أرجع نتیجہ;
}

ساكن CURLcode cf_quiche_send(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);
  CURLcode نتیجہ;

  *pnwritten = 0;
  Curl_vquic_ctx_update_ویلا(&ctx->q, Curl_pgrs_now(انکڑے));

  نتیجہ = cf_process_ingress(cf, انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

  إذا(!دھارا || !دھارا->opened) {
    نتیجہ = h3_open_دھارا(cf, انکڑے, buf, len, eos, pnwritten);
    إذا(نتیجہ)
      اذهب باہرلا;
    دھارا = H3_دھارا_CTX(ctx, انکڑے);
  }
  وإلا إذا(دھارا->xfer_نتیجہ) {
    cf_quiche_دھارا_close(cf, انکڑے, دھارا);
    نتیجہ = دھارا->xfer_نتیجہ;
  }
  وإلا إذا(دھارا->closed) {
    إذا(دھارا->resp_hds_complete) {
      /* sending request body on a stream that has been closed by the
       * server. If the server has send us a final response, we should
       * silently discard the send data.
       * This happens for example on redirects where the server, instead
       * of reading the full request body closed the stream after
       * sending the 30x response.
       * This is sort of a race: had the transfer loop called recv first,
       * it would see the response and stop/discard sending on its own- */
      CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] discarding data "
                  "on closed stream with response", دھارا->id);
      نتیجہ = CURLE_OK;
      *pnwritten = len;
      اذهب باہرلا;
    }
    CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", دھارا->id, len);
    نتیجہ = CURLE_HTTP3;
    اذهب باہرلا;
  }
  وإلا {
    نتیجہ = cf_quiche_send_body(cf, انکڑے, دھارا, buf, len, eos, pnwritten);
  }

باہرلا:
  نتیجہ = Curl_1st_fatal(نتیجہ, cf_flush_egress(cf, انکڑے));

  CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              دھارا ? دھارا->id : (uint64_t)~0, len,
              (صحيح)نتیجہ, *pnwritten);
  أرجع نتیجہ;
}

ساكن bool دھارا_is_writable(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);

  أرجع دھارا && (quiche_conn_دھارا_writable(
    ctx->qconn, دھارا->id, 1) > 0);
}

ساكن CURLcode cf_quiche_adjust_pollset(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *انکڑے,
                                         بنية easy_pollset *ps)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode نتیجہ = CURLE_OK;

  إذا(!ctx->qconn)
    أرجع CURLE_OK;

  Curl_pollset_check(انکڑے, ps, ctx->q.sockfd, &want_recv, &want_send);
  إذا(want_recv || want_send) {
    بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);
    bool c_exhaust, s_exhaust;

    c_exhaust = FALSE; /* Have not found any call in quiche that tells
                          us if the connection itself is blocked */
    s_exhaust = want_send && دھارا && دھارا->opened &&
                (دھارا->quic_flow_blocked || !دھارا_is_writable(cf, انکڑے));
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_خالی_تھاواں(&ctx->q.sendbuf);

    نتیجہ = Curl_pollset_سیٹ(انکڑے, ps, ctx->q.sockfd, want_recv, want_send);
  }
  أرجع نتیجہ;
}

ساكن CURLcode h3_انکڑے_pause(بنية Curl_cfilter *cf,
                              بنية Curl_easy *انکڑے,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (فراغ)cf;
  إذا(!pause) {
    Curl_multi_mark_dirty(انکڑے);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_quiche_cntrl(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_انکڑے_تیاری:
    اكسر;
  حالة CF_CTRL_انکڑے_PAUSE:
    نتیجہ = h3_انکڑے_pause(cf, انکڑے, (arg1 != 0));
    اكسر;
  حالة CF_CTRL_انکڑے_پورا:
    h3_انکڑے_پورا(cf, انکڑے);
    اكسر;
  حالة CF_CTRL_انکڑے_پورا_SEND: {
    بنية h3_دھارا_ctx *دھارا = H3_دھارا_CTX(ctx, انکڑے);
    إذا(دھارا && !دھارا->send_closed) {
      بنانشانی حرف body[1];
      size_t گھلیا;

      دھارا->send_closed = TRUE;
      body[0] = 'X';
      نتیجہ = cf_quiche_send(cf, انکڑے, body, 0, TRUE, &گھلیا);
      CURL_TRC_CF(انکڑے, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  دھارا->id, (صحيح)نتیجہ, گھلیا);
    }
    اكسر;
  }
  حالة CF_CTRL_CONN_INFO_UPDATE:
    إذا(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_تکیا = 30;
      Curl_conn_سیٹ_multiplex(cf->conn);
    }
    اكسر;
  حالة CF_CTRL_REPORT_STATS:
    إذا(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrs_ویلا_Was(انکڑے, TIMER_CONNECT, ctx->q.پہلا_byte_at);
      Curl_pgrs_ویلا_Was(انکڑے, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتیجہ;
}

ساكن CURLcode cf_quiche_ctx_open(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  صحيح rv;
  CURLcode نتیجہ;
  ثابت بنية Curl_sockaddr_ex *sockaddr;
  ساكن ثابت بنية alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  نتیجہ = Curl_vquic_ctx_init(انکڑے, &ctx->q);
  إذا(نتیجہ)
    أرجع نتیجہ;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_نسخہ);
  إذا(!ctx->cfg) {
    failf(انکڑے, "cannot create quiche config");
    أرجع CURLE_FAILED_INIT;
  }
  quiche_config_enable_pacing(ctx->cfg, FALSE);
  quiche_config_سیٹ_initial_max_انکڑے(ctx->cfg, (1 * 1024 * 1024)
    /* (QUIC_MAX_STREAMS/2) * H3_STREAM_WINDOW_SIZE */);
  quiche_config_سیٹ_initial_max_streams_bidi(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_سیٹ_initial_max_streams_uni(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_سیٹ_initial_max_دھارا_انکڑے_bidi_local(ctx->cfg,
    H3_دھارا_کھڑکی_ناپ);
  quiche_config_سیٹ_initial_max_دھارا_انکڑے_bidi_remote(ctx->cfg,
    H3_دھارا_کھڑکی_ناپ);
  quiche_config_سیٹ_initial_max_دھارا_انکڑے_uni(ctx->cfg,
    H3_دھارا_کھڑکی_ناپ);
  quiche_config_سیٹ_disable_سرگرم_migration(ctx->cfg, TRUE);

  quiche_config_سیٹ_max_connection_کھڑکی(ctx->cfg,
    10 * QUIC_MAX_STREAMS * H3_دھارا_کھڑکی_ناپ);
  quiche_config_سیٹ_max_دھارا_کھڑکی(ctx->cfg, 10 * H3_دھارا_کھڑکی_ناپ);
  quiche_config_سیٹ_application_protos(ctx->cfg,
                      (uint8_t *)CURL_UNCONST(QUICHE_H3_APPLICATION_PROTOCOL),
                                 CURL_CSTRLEN(QUICHE_H3_APPLICATION_PROTOCOL));

  نتیجہ = Curl_vquic_tls_init(&ctx->tls, cf, انکڑے, &ctx->ssl_peer,
                               &ALPN_SPEC_H3, عدم, عدم, cf, عدم);
  إذا(نتیجہ)
    أرجع نتیجہ;

  نتیجہ = Curl_rand(انکڑے, ctx->scid, حجم(ctx->scid));
  إذا(نتیجہ)
    أرجع نتیجہ;

  إذا(Curl_cf_socket_peek(cf->اگلا, انکڑے, &ctx->q.sockfd, &sockaddr, عدم))
    أرجع CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = حجم(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (بنية sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  إذا(rv == -1)
    أرجع CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((ثابت uint8_t *)ctx->scid,
                                        حجم(ctx->scid), عدم, 0,
                                        (بنية sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  إذا(!ctx->qconn) {
    failf(انکڑے, "cannot create quiche connection");
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  /* Known to not work on Windows */
#إذا !defined(_WIN32) && defined(HAVE_QUICHE_CONN_سیٹ_QLOG_FD)
  {
    صحيح qfd;
    (فراغ)Curl_qlogdir(انکڑے, ctx->scid, حجم(ctx->scid), &qfd);
    إذا(qfd != -1)
      quiche_conn_سیٹ_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#جے_انت

  نتیجہ = cf_flush_egress(cf, انکڑے);
  إذا(نتیجہ)
    أرجع نتیجہ;

  {
    بنانشانی حرف alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    بنانشانی alpn_len, وتھ = 0;

    /* Replace each ALPN length prefix by a comma. */
    طالما(وتھ < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[وتھ];
      alpn_protocols[وتھ] = ',';
      وتھ += 1 + alpn_len;
    }

    CURL_TRC_CF(انکڑے, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  أرجع CURLE_OK;
}

ساكن CURLcode cf_quiche_verify_peer(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *انکڑے)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  أرجع Curl_vquic_tls_verify_peer(&ctx->tls, cf, انکڑے, &ctx->ssl_peer);
}

ساكن CURLcode cf_quiche_connect(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  bool *پورا)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the UDP filter first */
  إذا(!cf->اگلا->connected) {
    نتیجہ = Curl_conn_cf_connect(cf->اگلا, انکڑے, پورا);
    إذا(نتیجہ || !*پورا)
      أرجع نتیجہ;
  }

  *پورا = FALSE;
  إذا(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(انکڑے, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(انکڑے, cf, "need HTTPS-RR, delaying connect");
    أرجع CURLE_OK;
  }

  Curl_vquic_ctx_update_ویلا(&ctx->q, Curl_pgrs_now(انکڑے));

  إذا(!ctx->qconn) {
    نتیجہ = cf_quiche_ctx_open(cf, انکڑے);
    إذا(نتیجہ)
      اذهب باہرلا;
    ctx->started_at = ctx->q.آخری_op;
    نتیجہ = cf_flush_egress(cf, انکڑے);
    /* we do not expect to be able to recv anything yet */
    اذهب باہرلا;
  }

  نتیجہ = cf_process_ingress(cf, انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = cf_flush_egress(cf, انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;

  إذا(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.آخری_op;
    CURL_TRC_CF(انکڑے, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    نتیجہ = cf_quiche_verify_peer(cf, انکڑے);
    إذا(!نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      إذا(!ctx->h3config) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_سیٹ_max_میدان_section_ناپ(ctx->h3config,
                                                  MAX_HTTP_RESP_سرخی_ناپ);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      إذا(!ctx->h3c) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
      cf->connected = TRUE;
      *پورا = TRUE;
    }
  }
  وإلا إذا(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    نتیجہ = CURLE_WEIRD_SERVER_جواب;
  }

باہرلا:
#جے_تعریف_شدہ CURLVERBOSE
  إذا(نتیجہ && نتیجہ != CURLE_AGAIN) {
    بنية ip_quadruple ip;

    إذا(!Curl_cf_socket_peek(cf->اگلا, انکڑے, عدم, عدم, &ip))
      infof(انکڑے, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_پورٹ, curl_easy_strerror(نتیجہ));
    وإلا
      infof(انکڑے, "connect failed: %s", curl_easy_strerror(نتیجہ));
  }
#جے_انت
  أرجع نتیجہ;
}

ساكن CURLcode cf_quiche_shutdown(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  إذا(cf->shutdown || !ctx || !ctx->qconn) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  *پورا = FALSE;
  إذا(!ctx->shutdown_started) {
    صحيح err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_ویلا(&ctx->q, Curl_pgrs_now(انکڑے));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, عدم, 0);
    إذا(err) {
      CURL_TRC_CF(انکڑے, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      نتیجہ = CURLE_SEND_ERROR;
      اذهب باہرلا;
    }
  }

  إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->q.sendbuf)) {
    CURL_TRC_CF(انکڑے, cf, "shutdown, flushing sendbuf");
    نتیجہ = cf_flush_egress(cf, انکڑے);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  إذا(Curl_bufq_is_خالی_تھاواں(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(انکڑے, cf, "shutdown completely sent off, done");
    *پورا = TRUE;
  }
  وإلا {
    CURL_TRC_CF(انکڑے, cf, "shutdown sending blocked");
  }

باہرلا:
  أرجع نتیجہ;
}

ساكن فراغ cf_quiche_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے)
{
  (فراغ)انکڑے;
  إذا(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_quiche_query(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    إذا(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_بایاں_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > ودھ_توں_ودھ_عدد) ? ودھ_توں_ودھ_عدد : (صحيح)max_streams;
    CURL_TRC_CF(انکڑے, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    أرجع CURLE_OK;
  }
  حالة CF_QUERY_CONNECT_جواب_MS:
    إذا(ctx->q.got_پہلا_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.پہلا_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < ودھ_توں_ودھ_عدد) ? (صحيح)ms : ودھ_توں_ودھ_عدد;
    }
    وإلا
      *pres1 = -1;
    أرجع CURLE_OK;
  حالة CF_QUERY_HTTP_نسخہ:
    *pres1 = 30;
    أرجع CURLE_OK;
  حالة CF_QUERY_SSL_INFO:
  حالة CF_QUERY_SSL_CTX_INFO: {
    بنية curl_tlssessioninfo *info = pres2;
    إذا(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      أرجع CURLE_OK;
    اكسر;
  }
  حالة CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : عدم;
    أرجع CURLE_OK;
  }
  افتراضي:
    اكسر;
  }
  أرجع cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, انکڑے, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن bool cf_quiche_conn_is_alive(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے,
                                    bool *داخلہ_pending)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *داخلہ_pending = FALSE;
  إذا(!ctx->qconn)
    أرجع FALSE;

  إذا(quiche_conn_is_closed(ctx->qconn)) {
    إذا(quiche_conn_is_timed_باہرلا(ctx->qconn))
      CURL_TRC_CF(انکڑے, cf, "connection was closed due to idle timeout");
    وإلا
      CURL_TRC_CF(انکڑے, cf, "connection is closed");
    أرجع FALSE;
  }

  إذا(!cf->اگلا || !cf->اگلا->cft->is_alive(cf->اگلا, انکڑے, داخلہ_pending))
    أرجع FALSE;

  إذا(*داخلہ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *داخلہ_pending = FALSE;
    إذا(cf_process_ingress(cf, انکڑے))
      alive = FALSE;
    وإلا {
      alive = TRUE;
    }
  }

  أرجع alive;
}

بنية Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_quiche_destroy,
  cf_quiche_connect,
  cf_quiche_shutdown,
  cf_quiche_adjust_pollset,
  Curl_cf_def_انکڑے_pending,
  cf_quiche_send,
  cf_quiche_recv,
  cf_quiche_cntrl,
  cf_quiche_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_quiche_query,
};

CURLcode Curl_cf_quiche_create(بنية Curl_cfilter **pcf,
                               بنية Curl_easy *انکڑے,
                               بنية Curl_peer *origin,
                               بنية Curl_peer *peer,
                               بنية connectdata *conn,
                               بنية Curl_sockaddr_ex *addr)
{
  بنية cf_quiche_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode نتیجہ;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  نتیجہ = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  إذا(!نتیجہ)
    نتیجہ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(نتیجہ)
    اذهب باہرلا;
  cf->conn = conn;

  نتیجہ = Curl_cf_udp_create(&cf->اگلا, انکڑے, origin, peer, TRNSPRT_QUIC,
                              conn, addr, عدم, TRNSPRT_QUIC);
  إذا(نتیجہ)
    اذهب باہرلا;
  cf->اگلا->conn = cf->conn;
  cf->اگلا->sockindex = cf->sockindex;

باہرلا:
  *pcf = (!نتیجہ) ? cf : عدم;
  إذا(نتیجہ) {
    إذا(cf)
      Curl_conn_cf_discard_chain(&cf, انکڑے);
    وإلا إذا(ctx)
      cf_quiche_ctx_free(ctx);
  }

  أرجع نتیجہ;
}

CURLcode Curl_cf_quiche_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     بنية Curl_cfilter **pcf)
{
  بنية cf_quiche_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode نتیجہ;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  نتیجہ = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  إذا(!نتیجہ)
    نتیجہ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(نتیجہ)
    اذهب باہرلا;
  Curl_conn_cf_داخل_کرو_after(cf_at, cf);

باہرلا:
  إذا(نتیجہ) {
    curlx_safefree(cf);
    إذا(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  أرجع نتیجہ;
}

#جے_انت
