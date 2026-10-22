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
#وصف_ڪريو CURL_H3_NO_ERROR  0x0100

#وصف_ڪريو MAX_PKT_مدو          10

#وصف_ڪريو QUIC_MAX_STREAMS       100

#وصف_ڪريو H3_وهڪرو_دري_سائيز  (1024 * 128)
#وصف_ڪريو H3_وهڪرو_CHUNK_سائيز   (1024 * 16)
/* Receive and Send max number of chunks follows from the
 * chunk size and window size */
#وصف_ڪريو H3_وهڪرو_RECV_CHUNKS \
  (H3_وهڪرو_دري_سائيز / H3_وهڪرو_CHUNK_سائيز)

/*
 * Store quiche version info in this buffer.
 */
فراغ Curl_quiche_ver(حرف *p, size_t len)
{
  (فراغ)curl_msnprintf(p, len, "quiche/%s", quiche_نسخو());
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
  curl_off_t معلومات_recvd;
  ٻٽو_انگ(initialized);
  ٻٽو_انگ(goaway);                       /* got GOAWAY from server */
  ٻٽو_انگ(x509_store_تياري);             /* if x509 store has been set up */
  ٻٽو_انگ(shutdown_started);             /* queued shutdown packets */
  ٻٽو_انگ(stats_reported);               /* connect statistics reported */
};

#جيڪڏهن_وصف_ٿيل DEBUG_QUICHE
/* initialize debug log callback only once */
ساكن صحيح debug_log_init = 0;
ساكن فراغ quiche_debug_log(ثابت حرف *سٽ, فراغ *argp)
{
  (فراغ)argp;
  curl_mfprintf(stderr, "%s\n", سٽ);
}
#جيڪڏهن_پورو

ساكن فراغ h3_وهڪرو_سيٽ_free(uint32_t id, فراغ *وهڪرو);

ساكن CURLcode cf_quiche_ctx_init(بنية cf_quiche_ctx *ctx,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer,
                                   بنية ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#جيڪڏهن_وصف_ٿيل DEBUG_QUICHE
  إذا(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, عدم);
    debug_log_init = 1;
  }
#جيڪڏهن_پورو
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_سرو);
  Curl_u32_ptrset_init(&ctx->streams, h3_وهڪرو_سيٽ_free);
  Curl_bufq_init2(&ctx->writebuf, H3_وهڪرو_CHUNK_سائيز, H3_وهڪرو_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد_بندي);
  ctx->معلومات_recvd = 0;
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
                                بنية Curl_easy *معلومات);

/**
 * All about the H3 internals of a stream
 */
بنية h3_وهڪرو_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  بنية h1_req_parser h1; /* h1 request parsing */
  uint64_t error3;         /* HTTP/3 stream error code */
  int32_t حالت_ڪوڊ;     /* HTTP status code */
  CURLcode xfer_نتيجو;    /* result from cf_quiche_write_(hd/body) */
  ٻٽو_انگ(opened);             /* TRUE after stream has been opened */
  ٻٽو_انگ(closed);             /* TRUE on stream close */
  ٻٽو_انگ(reset);              /* TRUE on stream reset */
  ٻٽو_انگ(send_closed);        /* stream is locally closed */
  ٻٽو_انگ(resp_hds_complete);  /* final response has been received */
  ٻٽو_انگ(resp_got_سرو);    /* TRUE when h3 stream has recvd some HEADER */
  ٻٽو_انگ(quic_flow_blocked);  /* stream is blocked by QUIC flow control */
};

ساكن فراغ h3_وهڪرو_ctx_free(بنية h3_وهڪرو_ctx *وهڪرو)
{
  Curl_h1_req_parse_free(&وهڪرو->h1);
  curlx_free(وهڪرو);
}

ساكن فراغ h3_وهڪرو_سيٽ_free(uint32_t id, فراغ *وهڪرو)
{
  (فراغ)id;
  DEBUGASSERT(وهڪرو);
  h3_وهڪرو_ctx_free((بنية h3_وهڪرو_ctx *)وهڪرو);
}

تعريفنوع bool cf_quiche_svisit(بنية Curl_cfilter *cf,
                              بنية Curl_easy *sdata,
                              بنية h3_وهڪرو_ctx *وهڪرو,
                              فراغ *user_معلومات);

بنية cf_quiche_گهمو_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_multi *multi;
  cf_quiche_svisit *cb;
  فراغ *user_معلومات;
};

ساكن bool cf_quiche_وهڪرو_do(uint32_t mid, فراغ *val, فراغ *user_معلومات)
{
  بنية cf_quiche_گهمو_ctx *vctx = user_معلومات;
  بنية h3_وهڪرو_ctx *وهڪرو = val;
  بنية Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  إذا(sdata)
    أرجع vctx->cb(vctx->cf, sdata, وهڪرو, vctx->user_معلومات);
  أرجع TRUE;
}

ساكن فراغ cf_quiche_for_all_streams(بنية Curl_cfilter *cf,
                                      بنية Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      فراغ *user_معلومات)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية cf_quiche_گهمو_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_معلومات = user_معلومات;
  Curl_u32_ptrset_گهمو(&ctx->streams, cf_quiche_وهڪرو_do, &vctx);
}

ساكن bool cf_quiche_do_resume(بنية Curl_cfilter *cf,
                                بنية Curl_easy *sdata,
                                بنية h3_وهڪرو_ctx *وهڪرو,
                                فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  إذا(وهڪرو->quic_flow_blocked) {
    وهڪرو->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", وهڪرو->id);
  }
  أرجع TRUE;
}

ساكن bool cf_quiche_do_expire(بنية Curl_cfilter *cf,
                                بنية Curl_easy *sdata,
                                بنية h3_وهڪرو_ctx *وهڪرو,
                                فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  وهڪرو->xfer_نتيجو = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  أرجع TRUE;
}

ساكن CURLcode h3_معلومات_تياري(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_وهڪرو_ctx *وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);

  إذا(وهڪرو)
    أرجع CURLE_OK;

  وهڪرو = curlx_calloc(1, حجم(*وهڪرو));
  إذا(!وهڪرو)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  وهڪرو->id = -1;
  Curl_h1_req_parse_init(&وهڪرو->h1, H1_PARSE_DEFAULT_MAX_سٽ_LEN);

  إذا(Curl_u32_ptrset_سيٽ(&ctx->streams, معلومات->mid, وهڪرو)) {
    h3_وهڪرو_ctx_free(وهڪرو);
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  أرجع CURLE_OK;
}

ساكن فراغ cf_quiche_وهڪرو_close(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   بنية h3_وهڪرو_ctx *وهڪرو)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتيجو;

  إذا(ctx->qconn && !وهڪرو->closed) {
    quiche_conn_وهڪرو_shutdown(ctx->qconn, وهڪرو->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    إذا(!وهڪرو->send_closed) {
      quiche_conn_وهڪرو_shutdown(ctx->qconn, وهڪرو->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      وهڪرو->send_closed = TRUE;
    }
    وهڪرو->closed = TRUE;
    نتيجو = cf_flush_egress(cf, معلومات);
    إذا(نتيجو)
      CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  وهڪرو->id, (صحيح)نتيجو);
  }
}

ساكن فراغ h3_معلومات_پورو(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_وهڪرو_ctx *وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);

  (فراغ)cf;
  إذا(وهڪرو) {
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] easy handle is done", وهڪرو->id);
    cf_quiche_وهڪرو_close(cf, معلومات, وهڪرو);
    Curl_u32_ptrset_unset(&ctx->streams, معلومات->mid);
  }
}

ساكن فراغ cf_quiche_expire_conn_closed(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات)
{
  DEBUGASSERT(معلومات->multi);
  CURL_TRC_CF(معلومات, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, معلومات->multi, cf_quiche_do_expire, عدم);
}

ساكن فراغ cf_quiche_write_hd(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية h3_وهڪرو_ctx *وهڪرو,
                               ثابت حرف *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  إذا(!وهڪرو->xfer_نتيجو) {
    وهڪرو->xfer_نتيجو = Curl_xfer_write_resp_hd(معلومات, buf, blen, eos);
    إذا(وهڪرو->xfer_نتيجو)
      CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", وهڪرو->id, (صحيح)وهڪرو->xfer_نتيجو,
                  blen);
  }
}

بنية cb_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *معلومات;
  بنية h3_وهڪرو_ctx *وهڪرو;
};

ساكن bool is_valid_h3_سرو(ثابت uint8_t *hdr, size_t hlen)
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

ساكن صحيح cb_each_سرو(uint8_t *name, size_t name_len,
                          uint8_t *قدر, size_t قدر_len,
                          فراغ *argp)
{
  بنية cb_ctx *x = argp;
  بنية Curl_cfilter *cf = x->cf;
  بنية Curl_easy *معلومات = x->معلومات;
  بنية h3_وهڪرو_ctx *وهڪرو = x->وهڪرو;
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  إذا(!وهڪرو || وهڪرو->xfer_نتيجو)
    أرجع 1; /* abort iteration */

  إذا((name_len == 7) && !لڙي_ڀيٽ(HTTP_PSEUDO_حالت, (حرف *)name, 7) &&
     is_valid_h3_سرو(قدر, قدر_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    نتيجو = Curl_http_decode_حالت(&وهڪرو->حالت_ڪوڊ,
                                     (ثابت حرف *)قدر, قدر_len);
    إذا(!نتيجو)
      نتيجو = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    إذا(!نتيجو)
      نتيجو = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)قدر, قدر_len);
    إذا(!نتيجو)
      نتيجو = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    إذا(!نتيجو)
      cf_quiche_write_hd(cf, معلومات, وهڪرو, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] status: %s",
                وهڪرو->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  وإلا {
    إذا(is_valid_h3_سرو(قدر, قدر_len) &&
       is_valid_h3_سرو(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  وهڪرو->id, (صحيح)name_len, name,
                  (صحيح)قدر_len, قدر);
      curlx_dyn_reset(&ctx->h1hdr);
      نتيجو = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)name, name_len);
      إذا(!نتيجو)
        نتيجو = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      إذا(!نتيجو)
        نتيجو = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)قدر, قدر_len);
      إذا(!نتيجو)
        نتيجو = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      إذا(!نتيجو)
        cf_quiche_write_hd(cf, معلومات, وهڪرو, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    وإلا
      CURL_TRC_CF(x->معلومات, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  وهڪرو->id, قدر_len + name_len);
  }

  إذا(نتيجو) {
    CURL_TRC_CF(x->معلومات, x->cf, "[%" PRIu64 "] on header error %d",
                وهڪرو->id, (صحيح)نتيجو);
    إذا(!وهڪرو->xfer_نتيجو)
      وهڪرو->xfer_نتيجو = نتيجو;
  }
  أرجع وهڪرو->xfer_نتيجو ? 1 : 0;
}

ساكن CURLcode وهڪرو_resp_read(فراغ *reader_ctx,
                                 بنانشان حرف *buf, size_t len,
                                 size_t *pnread)
{
  بنية cb_ctx *x = reader_ctx;
  بنية cf_quiche_ctx *ctx = x->cf->ctx;
  بنية h3_وهڪرو_ctx *وهڪرو = H3_وهڪرو_CTX(ctx, x->معلومات);
  ssize_t nread;

  *pnread = 0;
  إذا(!وهڪرو)
    أرجع CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, وهڪرو->id, buf, len);
  إذا(!curlx_sztouz(nread, pnread))
    أرجع CURLE_AGAIN;
  أرجع CURLE_OK;
}

ساكن فراغ cf_quiche_flush_body(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية h3_وهڪرو_ctx *وهڪرو)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  ثابت uint8_t *buf;
  size_t blen;

  طالما(وهڪرو && !وهڪرو->xfer_نتيجو) {
    إذا(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      وهڪرو->xfer_نتيجو = Curl_xfer_write_resp(
        معلومات, (ثابت حرف *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      إذا(وهڪرو->xfer_نتيجو) {
        CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", وهڪرو->id, (صحيح)وهڪرو->xfer_نتيجو, blen);
      }
    }
    وإلا
      اكسر;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

ساكن فراغ cf_quiche_recv_body(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                بنية h3_وهڪرو_ctx *وهڪرو)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  بنية cb_ctx cb_ctx;
  CURLcode نتيجو = CURLE_OK;

  إذا(!وهڪرو)
    أرجع;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.معلومات = معلومات;
  cb_ctx.وهڪرو = وهڪرو;
  Curl_bufq_reset(&ctx->writebuf);
  طالما(!نتيجو) {
    نتيجو = Curl_bufq_slurp(&ctx->writebuf,
                             وهڪرو_resp_read, &cb_ctx, &nread);
    إذا(!نتيجو)
      cf_quiche_flush_body(cf, معلومات, وهڪرو);
    وإلا إذا(نتيجو == CURLE_AGAIN)
      اكسر;
    وإلا إذا(نتيجو) {
      CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] recv_body error %d",
                  وهڪرو->id, (صحيح)نتيجو);
      failf(معلومات, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            وهڪرو->id, (صحيح)نتيجو);
      وهڪرو->closed = TRUE;
      وهڪرو->reset = TRUE;
      وهڪرو->send_closed = TRUE;
      إذا(!وهڪرو->xfer_نتيجو)
        وهڪرو->xfer_نتيجو = نتيجو;
    }
  }
  cf_quiche_flush_body(cf, معلومات, وهڪرو);
}

ساكن فراغ cf_quiche_process_ev(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية h3_وهڪرو_ctx *وهڪرو,
                                 quiche_h3_event *ev)
{
  تعداد quiche_h3_event_type type;

  إذا(!وهڪرو)
    أرجع;

  type = quiche_h3_event_type(ev);

  بدل(type) {
  حالة QUICHE_H3_EVENT_سرا: {
    بنية cb_ctx cb_ctx;
    وهڪرو->resp_got_سرو = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.معلومات = معلومات;
    cb_ctx.وهڪرو = وهڪرو;
    quiche_h3_event_for_each_سرو(ev, cb_each_سرو, &cb_ctx);
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] <- [HEADERS]", وهڪرو->id);
    Curl_multi_mark_dirty(معلومات);
    اكسر;
  }
  حالة QUICHE_H3_EVENT_معلومات:
    إذا(!وهڪرو->resp_hds_complete) {
      وهڪرو->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, معلومات, وهڪرو, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, معلومات, وهڪرو);
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] <- [DATA]", وهڪرو->id);
    Curl_multi_mark_dirty(معلومات);
    اكسر;

  حالة QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] RESET", وهڪرو->id);
    وهڪرو->closed = TRUE;
    وهڪرو->reset = TRUE;
    وهڪرو->send_closed = TRUE;
    Curl_multi_mark_dirty(معلومات);
    اكسر;

  حالة QUICHE_H3_EVENT_ختم:
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] CLOSED", وهڪرو->id);
    إذا(!وهڪرو->resp_hds_complete) {
      وهڪرو->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, معلومات, وهڪرو, "\r\n", 2, TRUE);
    }
    وهڪرو->closed = TRUE;
    Curl_multi_mark_dirty(معلومات);
    اكسر;

  حالة QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] <- [GOAWAY]", وهڪرو->id);
    اكسر;

  افتراضي:
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] recv, unhandled event %d",
                وهڪرو->id, (صحيح)type);
    اكسر;
  }
}

بنية cf_quiche_disp_ctx {
  uint64_t وهڪرو_id;
  بنية Curl_cfilter *cf;
  بنية Curl_multi *multi;
  quiche_h3_event *ev;
};

ساكن bool cf_quiche_disp_event(uint32_t mid, فراغ *val, فراغ *user_معلومات)
{
  بنية cf_quiche_disp_ctx *dctx = user_معلومات;
  بنية h3_وهڪرو_ctx *وهڪرو = val;

  إذا(وهڪرو->id == dctx->وهڪرو_id) {
    بنية Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    إذا(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, وهڪرو, dctx->ev);
    أرجع FALSE; /* stop iterating */
  }
  أرجع TRUE;
}

ساكن CURLcode cf_poll_events(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_وهڪرو_ctx *وهڪرو = عدم;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  طالما(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    إذا(rv == QUICHE_H3_ERR_پورو) {
      اكسر;
    }
    وإلا إذا(rv < 0) {
      CURL_TRC_CF(معلومات, cf, "error poll: %" PRId64, rv);
      أرجع CURLE_HTTP3;
    }
    وإلا {
      وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);
      إذا(وهڪرو && وهڪرو->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, معلومات, وهڪرو, ev);
        quiche_h3_event_free(ev);
        إذا(وهڪرو->xfer_نتيجو)
          أرجع وهڪرو->xfer_نتيجو;
      }
      وإلا {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        بنية cf_quiche_disp_ctx dctx;
        dctx.وهڪرو_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = معلومات->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_گهمو(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  أرجع CURLE_OK;
}

بنية recv_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *معلومات;
  صحيح pkts;
};

ساكن CURLcode cf_quiche_recv_pkts(ثابت بنانشان حرف *buf, size_t buflen,
                                    size_t gso_سائيز,
                                    بنية sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    فراغ *userp)
{
  بنية recv_ctx *r = userp;
  بنية cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, وٿي, nread;
  ssize_t rv;

  (فراغ)ecn;

  recv_info.منزل = (بنية sockaddr *)&ctx->q.local_addr;
  recv_info.منزل_len = ctx->q.local_addrlen;
  recv_info.from = (بنية sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  لكل(وٿي = 0; وٿي < buflen; وٿي += gso_سائيز) {
    pktlen = ((وٿي + gso_سائيز) <= buflen) ? gso_سائيز : (buflen - وٿي);
    rv = quiche_conn_recv(ctx->qconn,
                          (بنانشان حرف *)CURL_UNCONST(buf + وٿي),
                          pktlen, &recv_info);
    إذا(!curlx_sztouz(rv, &nread)) {
      إذا(QUICHE_ERR_پورو == rv) {
        إذا(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->معلومات, r->cf, "ingress, connection is draining");
          أرجع CURLE_RECV_ERROR;
        }
        إذا(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->معلومات, r->cf, "ingress, connection is closed");
          أرجع CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->معلومات, r->cf, "ingress, quiche is DONE");
        أرجع CURLE_OK;
      }
      وإلا إذا(QUICHE_ERR_TLS_FAIL == rv) {
        ڊگهو verify_ok = SSL_get_verify_نتيجو(ctx->tls.ossl.ssl);
        إذا(verify_ok != X509_V_OK) {
          failf(r->معلومات, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          أرجع CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->معلومات, "ingress, quiche reports TLS fail");
        أرجع CURLE_RECV_ERROR;
      }
      وإلا {
        failf(r->معلومات, "quiche reports error %zd on receive", rv);
        أرجع CURLE_RECV_ERROR;
      }
    }
    وإلا إذا(nread < pktlen) {
      CURL_TRC_CF(r->معلومات, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode cf_process_ingress(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية recv_ctx rctx;
  CURLcode نتيجو;

  DEBUGASSERT(ctx->qconn);
  نتيجو = Curl_vquic_tls_before_recv(&ctx->tls, cf, معلومات);
  إذا(نتيجو)
    أرجع نتيجو;

  rctx.cf = cf;
  rctx.معلومات = معلومات;
  rctx.pkts = 0;

  نتيجو = Curl_vquic_recv_packets(cf, معلومات, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(معلومات->multi);
    cf_quiche_for_all_streams(cf, معلومات->multi, cf_quiche_do_resume, عدم);
  }
  أرجع cf_poll_events(cf, معلومات);
}

بنية read_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *معلومات;
  quiche_send_info send_info;
};

ساكن CURLcode read_pkt_منزل_send(فراغ *userp,
                                 بنانشان حرف *buf, size_t buflen,
                                 size_t *pnread)
{
  بنية read_ctx *x = userp;
  بنية cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  إذا(rv == QUICHE_ERR_پورو)
    أرجع CURLE_AGAIN;

  إذا(!curlx_sztouz(rv, pnread)) {
    failf(x->معلومات, "quiche_conn_send returned %zd", rv);
    أرجع CURLE_SEND_ERROR;
  }
  أرجع CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
ساكن CURLcode cf_flush_egress(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode نتيجو;
  int64_t expiry_ns;
  int64_t timeout_ns;
  بنية read_ctx readx;
  size_t pkt_ڳڻپ, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  إذا(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    إذا(quiche_conn_is_closed(ctx->qconn)) {
      إذا(quiche_conn_is_timed_ٻاهريون(ctx->qconn))
        failf(معلومات, "connection closed by idle timeout");
      وإلا
        failf(معلومات, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, معلومات);
      أرجع CURLE_SEND_ERROR;
    }
  }

  نتيجو = Curl_vquic_flush(cf, معلومات, &ctx->q);
  إذا(نتيجو) {
    إذا(نتيجو == CURLE_AGAIN) {
      Curl_expire(معلومات, 1, EXPIRE_QUIC);
      أرجع CURLE_OK;
    }
    أرجع نتيجو;
  }

  readx.cf = cf;
  readx.معلومات = معلومات;
  memset(&readx.send_info, 0, حجم(readx.send_info));
  pkt_ڳڻپ = 0;
  gsolen = quiche_conn_max_send_udp_payload_سائيز(ctx->qconn);
  لكل(;;) {
    /* add the next packet to send, if any, to our buffer */
    نتيجو = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_منزل_send, &readx, &nread);
    إذا(نتيجو) {
      إذا(نتيجو != CURLE_AGAIN)
        أرجع نتيجو;
      /* Nothing more to add, flush and leave */
      نتيجو = Curl_vquic_send(cf, معلومات, &ctx->q, gsolen);
      إذا(نتيجو) {
        إذا(نتيجو == CURLE_AGAIN) {
          Curl_expire(معلومات, 1, EXPIRE_QUIC);
          أرجع CURLE_OK;
        }
        أرجع نتيجو;
      }
      اذهب ٻاهريون;
    }

    ++pkt_ڳڻپ;
    إذا(nread < gsolen || pkt_ڳڻپ >= MAX_PKT_مدو) {
      نتيجو = Curl_vquic_send(cf, معلومات, &ctx->q, gsolen);
      إذا(نتيجو) {
        إذا(نتيجو == CURLE_AGAIN) {
          Curl_expire(معلومات, 1, EXPIRE_QUIC);
          أرجع CURLE_OK;
        }
        اذهب ٻاهريون;
      }
      pkt_ڳڻپ = 0;
    }
  }

ٻاهريون:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  إذا(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(معلومات, (timeout_ns / 1000000), EXPIRE_QUIC);
  أرجع نتيجو;
}

ساكن CURLcode recv_closed_وهڪرو(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   size_t *pnread)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_وهڪرو_ctx *وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(وهڪرو);
  *pnread = 0;
  إذا(وهڪرو->reset) {
    إذا(وهڪرو->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(معلومات, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", وهڪرو->id);
      connclose(cf->conn); /* do not use this anymore */
      معلومات->state.refused_وهڪرو = TRUE;
      أرجع CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وإلا إذا(وهڪرو->resp_hds_complete && معلومات->req.no_body) {
        CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", وهڪرو->id, وهڪرو->error3,
                    Curl_vquic_h3_err_str(وهڪرو->error3));
        أرجع CURLE_OK;
    }
    failf(معلومات, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", وهڪرو->id, وهڪرو->error3,
          Curl_vquic_h3_err_str(وهڪرو->error3));
    نتيجو = معلومات->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                وهڪرو->id, (صحيح)نتيجو);
  }
  وإلا إذا(!وهڪرو->resp_got_سرو) {
    failf(معلومات, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          وهڪرو->id);
    نتيجو = CURLE_HTTP3;
  }
  أرجع نتيجو;
}

ساكن CURLcode cf_quiche_recv(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                               حرف *buf, size_t blen, size_t *pnread)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_وهڪرو_ctx *وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);
  CURLcode نتيجو = CURLE_OK;

  *pnread = 0;
  (فراغ)buf;
  (فراغ)blen;
  Curl_vquic_ctx_update_وقت(&ctx->q, Curl_pgrs_now(معلومات));

  إذا(!وهڪرو)
    أرجع CURLE_RECV_ERROR;

  نتيجو = cf_process_ingress(cf, معلومات);
  إذا(نتيجو) {
    CURL_TRC_CF(معلومات, cf, "cf_recv, error on ingress");
    اذهب ٻاهريون;
  }

  إذا(وهڪرو->xfer_نتيجو) {
    cf_quiche_وهڪرو_close(cf, معلومات, وهڪرو);
    نتيجو = وهڪرو->xfer_نتيجو;
    اذهب ٻاهريون;
  }
  وإلا إذا(وهڪرو->closed)
    نتيجو = recv_closed_وهڪرو(cf, معلومات, pnread);
  وإلا إذا(quiche_conn_is_draining(ctx->qconn)) {
    failf(معلومات, "QUIC connection is draining");
    نتيجو = CURLE_HTTP3;
  }
  وإلا
    نتيجو = CURLE_AGAIN;

ٻاهريون:
  نتيجو = Curl_1st_fatal(نتيجو, cf_flush_egress(cf, معلومات));
  إذا(*pnread > 0)
    ctx->معلومات_recvd += *pnread;
  CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, وهڪرو->id, blen, (صحيح)نتيجو, *pnread,
              ctx->معلومات_recvd);
  أرجع نتيجو;
}

ساكن CURLcode cf_quiche_send_body(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية h3_وهڪرو_ctx *وهڪرو,
                                    ثابت uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, وهڪرو->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  إذا(rv == QUICHE_H3_ERR_پورو || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    إذا(!quiche_conn_وهڪرو_writable(ctx->qconn, وهڪرو->id, len)) {
      CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", وهڪرو->id, len);
      وهڪرو->quic_flow_blocked = TRUE;
    }
    أرجع CURLE_AGAIN;
  }
  وإلا إذا(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_وهڪرو_STATE) {
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", وهڪرو->id, len);
    أرجع CURLE_HTTP3;
  }
  وإلا إذا(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_سائيز) {
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                وهڪرو->id, len);
    أرجع CURLE_SEND_ERROR;
  }
  وإلا إذا(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                وهڪرو->id, len, rv);
    أرجع CURLE_SEND_ERROR;
  }
  وإلا {
    إذا(eos && (len == *pnwritten))
      وهڪرو->send_closed = TRUE;
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                وهڪرو->id, len, وهڪرو->send_closed, *pnwritten);
    أرجع CURLE_OK;
  }
}

ساكن CURLcode h3_open_وهڪرو(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               ثابت uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_وهڪرو_ctx *وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);
  size_t nheader, i;
  int64_t rv;
  بنية dynhds h2_سرا;
  quiche_h3_سرو *nva = عدم;
  CURLcode نتيجو = CURLE_OK;

  *pnwritten = 0;
  إذا(!وهڪرو) {
    نتيجو = h3_معلومات_تياري(cf, معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
    وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);
    DEBUGASSERT(وهڪرو);
  }

  Curl_dynhds_init(&h2_سرا, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(وهڪرو);

  نتيجو = Curl_h1_req_parse_read(&وهڪرو->h1, buf, blen, عدم,
    !معلومات->state.http_ignorecustom ?
    CURL_EASY_STR(معلومات, STRING_CUSTOMREQUEST) : عدم,
    0, pnwritten);
  إذا(نتيجو)
    اذهب ٻاهريون;
  إذا(!وهڪرو->h1.پورو) {
    /* need more data */
    اذهب ٻاهريون;
  }
  DEBUGASSERT(وهڪرو->h1.req);

  نتيجو = Curl_http_req_منزل_h2(&h2_سرا, وهڪرو->h1.req, معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;

  /* no longer needed */
  Curl_h1_req_parse_free(&وهڪرو->h1);

  nheader = Curl_dynhds_ڳڻپ(&h2_سرا);
  nva = curlx_malloc(حجم(quiche_h3_سرو) * nheader);
  إذا(!nva) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  لكل(i = 0; i < nheader; ++i) {
    بنية dynhds_داخلا *e = Curl_dynhds_getn(&h2_سرا, i);
    nva[i].name = (بنانشان حرف *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].قدر = (بنانشان حرف *)e->قدر;
    nva[i].قدر_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  إذا(eos && !blen)
    وهڪرو->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              وهڪرو->send_closed);
  CURL_TRC_CF(معلومات, cf, "quiche_send_request() -> %" PRId64, rv);
  إذا(rv < 0) {
    إذا(QUICHE_H3_ERR_وهڪرو_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] blocked", وهڪرو->id);
      وهڪرو->quic_flow_blocked = TRUE;
      نتيجو = CURLE_AGAIN;
      اذهب ٻاهريون;
    }
    وإلا {
      CURL_TRC_CF(معلومات, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&معلومات->state.url), rv);
    }
    نتيجو = CURLE_SEND_ERROR;
    اذهب ٻاهريون;
  }

  DEBUGASSERT(!وهڪرو->opened);
  وهڪرو->id = (uint64_t)rv;
  وهڪرو->opened = TRUE;
  وهڪرو->closed = FALSE;
  وهڪرو->reset = FALSE;

  إذا(Curl_trc_is_verbose(معلومات)) {
    infof(معلومات, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          وهڪرو->id, Curl_bufref_ptr(&معلومات->state.url));
    لكل(i = 0; i < nheader; ++i) {
      infof(معلومات, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", وهڪرو->id,
            (صحيح)nva[i].name_len, nva[i].name,
            (صحيح)nva[i].قدر_len, nva[i].قدر);
    }
  }

  إذا(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, معلومات, وهڪرو, buf, blen, eos, &nwritten);
    إذا(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      نتيجو = r2;
    }
    وإلا إذا(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

ٻاهريون:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرا);
  أرجع نتيجو;
}

ساكن CURLcode cf_quiche_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_وهڪرو_ctx *وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);
  CURLcode نتيجو;

  *pnwritten = 0;
  Curl_vquic_ctx_update_وقت(&ctx->q, Curl_pgrs_now(معلومات));

  نتيجو = cf_process_ingress(cf, معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;

  إذا(!وهڪرو || !وهڪرو->opened) {
    نتيجو = h3_open_وهڪرو(cf, معلومات, buf, len, eos, pnwritten);
    إذا(نتيجو)
      اذهب ٻاهريون;
    وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);
  }
  وإلا إذا(وهڪرو->xfer_نتيجو) {
    cf_quiche_وهڪرو_close(cf, معلومات, وهڪرو);
    نتيجو = وهڪرو->xfer_نتيجو;
  }
  وإلا إذا(وهڪرو->closed) {
    إذا(وهڪرو->resp_hds_complete) {
      /* sending request body on a stream that has been closed by the
       * server. If the server has send us a final response, we should
       * silently discard the send data.
       * This happens for example on redirects where the server, instead
       * of reading the full request body closed the stream after
       * sending the 30x response.
       * This is sort of a race: had the transfer loop called recv first,
       * it would see the response and stop/discard sending on its own- */
      CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] discarding data "
                  "on closed stream with response", وهڪرو->id);
      نتيجو = CURLE_OK;
      *pnwritten = len;
      اذهب ٻاهريون;
    }
    CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", وهڪرو->id, len);
    نتيجو = CURLE_HTTP3;
    اذهب ٻاهريون;
  }
  وإلا {
    نتيجو = cf_quiche_send_body(cf, معلومات, وهڪرو, buf, len, eos, pnwritten);
  }

ٻاهريون:
  نتيجو = Curl_1st_fatal(نتيجو, cf_flush_egress(cf, معلومات));

  CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              وهڪرو ? وهڪرو->id : (uint64_t)~0, len,
              (صحيح)نتيجو, *pnwritten);
  أرجع نتيجو;
}

ساكن bool وهڪرو_is_writable(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_وهڪرو_ctx *وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);

  أرجع وهڪرو && (quiche_conn_وهڪرو_writable(
    ctx->qconn, وهڪرو->id, 1) > 0);
}

ساكن CURLcode cf_quiche_adjust_pollset(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات,
                                         بنية easy_pollset *ps)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode نتيجو = CURLE_OK;

  إذا(!ctx->qconn)
    أرجع CURLE_OK;

  Curl_pollset_check(معلومات, ps, ctx->q.sockfd, &want_recv, &want_send);
  إذا(want_recv || want_send) {
    بنية h3_وهڪرو_ctx *وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);
    bool c_exhaust, s_exhaust;

    c_exhaust = FALSE; /* Have not found any call in quiche that tells
                          us if the connection itself is blocked */
    s_exhaust = want_send && وهڪرو && وهڪرو->opened &&
                (وهڪرو->quic_flow_blocked || !وهڪرو_is_writable(cf, معلومات));
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_خالي_جايون(&ctx->q.sendbuf);

    نتيجو = Curl_pollset_سيٽ(معلومات, ps, ctx->q.sockfd, want_recv, want_send);
  }
  أرجع نتيجو;
}

ساكن CURLcode h3_معلومات_pause(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (فراغ)cf;
  إذا(!pause) {
    Curl_multi_mark_dirty(معلومات);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_quiche_cntrl(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_معلومات_تياري:
    اكسر;
  حالة CF_CTRL_معلومات_PAUSE:
    نتيجو = h3_معلومات_pause(cf, معلومات, (arg1 != 0));
    اكسر;
  حالة CF_CTRL_معلومات_پورو:
    h3_معلومات_پورو(cf, معلومات);
    اكسر;
  حالة CF_CTRL_معلومات_پورو_SEND: {
    بنية h3_وهڪرو_ctx *وهڪرو = H3_وهڪرو_CTX(ctx, معلومات);
    إذا(وهڪرو && !وهڪرو->send_closed) {
      بنانشان حرف body[1];
      size_t موڪليل;

      وهڪرو->send_closed = TRUE;
      body[0] = 'X';
      نتيجو = cf_quiche_send(cf, معلومات, body, 0, TRUE, &موڪليل);
      CURL_TRC_CF(معلومات, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  وهڪرو->id, (صحيح)نتيجو, موڪليل);
    }
    اكسر;
  }
  حالة CF_CTRL_CONN_INFO_UPDATE:
    إذا(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_ڏٺل = 30;
      Curl_conn_سيٽ_multiplex(cf->conn);
    }
    اكسر;
  حالة CF_CTRL_REPORT_STATS:
    إذا(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrs_وقت_Was(معلومات, TIMER_CONNECT, ctx->q.پهريون_byte_at);
      Curl_pgrs_وقت_Was(معلومات, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتيجو;
}

ساكن CURLcode cf_quiche_ctx_open(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  صحيح rv;
  CURLcode نتيجو;
  ثابت بنية Curl_sockaddr_ex *sockaddr;
  ساكن ثابت بنية alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  نتيجو = Curl_vquic_ctx_init(معلومات, &ctx->q);
  إذا(نتيجو)
    أرجع نتيجو;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_نسخو);
  إذا(!ctx->cfg) {
    failf(معلومات, "cannot create quiche config");
    أرجع CURLE_FAILED_INIT;
  }
  quiche_config_enable_pacing(ctx->cfg, FALSE);
  quiche_config_سيٽ_initial_max_معلومات(ctx->cfg, (1 * 1024 * 1024)
    /* (QUIC_MAX_STREAMS/2) * H3_STREAM_WINDOW_SIZE */);
  quiche_config_سيٽ_initial_max_streams_bidi(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_سيٽ_initial_max_streams_uni(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_سيٽ_initial_max_وهڪرو_معلومات_bidi_local(ctx->cfg,
    H3_وهڪرو_دري_سائيز);
  quiche_config_سيٽ_initial_max_وهڪرو_معلومات_bidi_remote(ctx->cfg,
    H3_وهڪرو_دري_سائيز);
  quiche_config_سيٽ_initial_max_وهڪرو_معلومات_uni(ctx->cfg,
    H3_وهڪرو_دري_سائيز);
  quiche_config_سيٽ_disable_سرگرم_migration(ctx->cfg, TRUE);

  quiche_config_سيٽ_max_connection_دري(ctx->cfg,
    10 * QUIC_MAX_STREAMS * H3_وهڪرو_دري_سائيز);
  quiche_config_سيٽ_max_وهڪرو_دري(ctx->cfg, 10 * H3_وهڪرو_دري_سائيز);
  quiche_config_سيٽ_application_protos(ctx->cfg,
                      (uint8_t *)CURL_UNCONST(QUICHE_H3_APPLICATION_PROTOCOL),
                                 CURL_CSTRLEN(QUICHE_H3_APPLICATION_PROTOCOL));

  نتيجو = Curl_vquic_tls_init(&ctx->tls, cf, معلومات, &ctx->ssl_peer,
                               &ALPN_SPEC_H3, عدم, عدم, cf, عدم);
  إذا(نتيجو)
    أرجع نتيجو;

  نتيجو = Curl_rand(معلومات, ctx->scid, حجم(ctx->scid));
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(Curl_cf_socket_peek(cf->ايندڙ, معلومات, &ctx->q.sockfd, &sockaddr, عدم))
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
    failf(معلومات, "cannot create quiche connection");
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  /* Known to not work on Windows */
#إذا !defined(_WIN32) && defined(HAVE_QUICHE_CONN_سيٽ_QLOG_FD)
  {
    صحيح qfd;
    (فراغ)Curl_qlogdir(معلومات, ctx->scid, حجم(ctx->scid), &qfd);
    إذا(qfd != -1)
      quiche_conn_سيٽ_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#جيڪڏهن_پورو

  نتيجو = cf_flush_egress(cf, معلومات);
  إذا(نتيجو)
    أرجع نتيجو;

  {
    بنانشان حرف alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    بنانشان alpn_len, وٿي = 0;

    /* Replace each ALPN length prefix by a comma. */
    طالما(وٿي < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[وٿي];
      alpn_protocols[وٿي] = ',';
      وٿي += 1 + alpn_len;
    }

    CURL_TRC_CF(معلومات, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  أرجع CURLE_OK;
}

ساكن CURLcode cf_quiche_verify_peer(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  أرجع Curl_vquic_tls_verify_peer(&ctx->tls, cf, معلومات, &ctx->ssl_peer);
}

ساكن CURLcode cf_quiche_connect(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  bool *پورو)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the UDP filter first */
  إذا(!cf->ايندڙ->connected) {
    نتيجو = Curl_conn_cf_connect(cf->ايندڙ, معلومات, پورو);
    إذا(نتيجو || !*پورو)
      أرجع نتيجو;
  }

  *پورو = FALSE;
  إذا(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(معلومات, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(معلومات, cf, "need HTTPS-RR, delaying connect");
    أرجع CURLE_OK;
  }

  Curl_vquic_ctx_update_وقت(&ctx->q, Curl_pgrs_now(معلومات));

  إذا(!ctx->qconn) {
    نتيجو = cf_quiche_ctx_open(cf, معلومات);
    إذا(نتيجو)
      اذهب ٻاهريون;
    ctx->started_at = ctx->q.آخري_op;
    نتيجو = cf_flush_egress(cf, معلومات);
    /* we do not expect to be able to recv anything yet */
    اذهب ٻاهريون;
  }

  نتيجو = cf_process_ingress(cf, معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;

  نتيجو = cf_flush_egress(cf, معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;

  إذا(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.آخري_op;
    CURL_TRC_CF(معلومات, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    نتيجو = cf_quiche_verify_peer(cf, معلومات);
    إذا(!نتيجو) {
      CURL_TRC_CF(معلومات, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      إذا(!ctx->h3config) {
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
        اذهب ٻاهريون;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_سيٽ_max_ميدان_section_سائيز(ctx->h3config,
                                                  MAX_HTTP_RESP_سرو_سائيز);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      إذا(!ctx->h3c) {
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
        اذهب ٻاهريون;
      }
      cf->connected = TRUE;
      *پورو = TRUE;
    }
  }
  وإلا إذا(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    نتيجو = CURLE_WEIRD_SERVER_جواب;
  }

ٻاهريون:
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  إذا(نتيجو && نتيجو != CURLE_AGAIN) {
    بنية ip_quadruple ip;

    إذا(!Curl_cf_socket_peek(cf->ايندڙ, معلومات, عدم, عدم, &ip))
      infof(معلومات, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_پورٽ, curl_easy_strerror(نتيجو));
    وإلا
      infof(معلومات, "connect failed: %s", curl_easy_strerror(نتيجو));
  }
#جيڪڏهن_پورو
  أرجع نتيجو;
}

ساكن CURLcode cf_quiche_shutdown(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات, bool *پورو)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  إذا(cf->shutdown || !ctx || !ctx->qconn) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  *پورو = FALSE;
  إذا(!ctx->shutdown_started) {
    صحيح err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_وقت(&ctx->q, Curl_pgrs_now(معلومات));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, عدم, 0);
    إذا(err) {
      CURL_TRC_CF(معلومات, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }
  }

  إذا(!Curl_bufq_is_خالي_جايون(&ctx->q.sendbuf)) {
    CURL_TRC_CF(معلومات, cf, "shutdown, flushing sendbuf");
    نتيجو = cf_flush_egress(cf, معلومات);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  إذا(Curl_bufq_is_خالي_جايون(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(معلومات, cf, "shutdown completely sent off, done");
    *پورو = TRUE;
  }
  وإلا {
    CURL_TRC_CF(معلومات, cf, "shutdown sending blocked");
  }

ٻاهريون:
  أرجع نتيجو;
}

ساكن فراغ cf_quiche_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  (فراغ)معلومات;
  إذا(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_quiche_query(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    إذا(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_کاٻو_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > وڌ_کان_وڌ_عدد) ? وڌ_کان_وڌ_عدد : (صحيح)max_streams;
    CURL_TRC_CF(معلومات, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    أرجع CURLE_OK;
  }
  حالة CF_QUERY_CONNECT_جواب_MS:
    إذا(ctx->q.got_پهريون_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.پهريون_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < وڌ_کان_وڌ_عدد) ? (صحيح)ms : وڌ_کان_وڌ_عدد;
    }
    وإلا
      *pres1 = -1;
    أرجع CURLE_OK;
  حالة CF_QUERY_HTTP_نسخو:
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
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->query(cf->ايندڙ, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن bool cf_quiche_conn_is_alive(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    bool *داخلا_pending)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *داخلا_pending = FALSE;
  إذا(!ctx->qconn)
    أرجع FALSE;

  إذا(quiche_conn_is_closed(ctx->qconn)) {
    إذا(quiche_conn_is_timed_ٻاهريون(ctx->qconn))
      CURL_TRC_CF(معلومات, cf, "connection was closed due to idle timeout");
    وإلا
      CURL_TRC_CF(معلومات, cf, "connection is closed");
    أرجع FALSE;
  }

  إذا(!cf->ايندڙ || !cf->ايندڙ->cft->is_alive(cf->ايندڙ, معلومات, داخلا_pending))
    أرجع FALSE;

  إذا(*داخلا_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *داخلا_pending = FALSE;
    إذا(cf_process_ingress(cf, معلومات))
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
  Curl_cf_def_معلومات_pending,
  cf_quiche_send,
  cf_quiche_recv,
  cf_quiche_cntrl,
  cf_quiche_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_quiche_query,
};

CURLcode Curl_cf_quiche_create(بنية Curl_cfilter **pcf,
                               بنية Curl_easy *معلومات,
                               بنية Curl_peer *origin,
                               بنية Curl_peer *peer,
                               بنية connectdata *conn,
                               بنية Curl_sockaddr_ex *addr)
{
  بنية cf_quiche_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode نتيجو;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }
  نتيجو = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  إذا(!نتيجو)
    نتيجو = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(نتيجو)
    اذهب ٻاهريون;
  cf->conn = conn;

  نتيجو = Curl_cf_udp_create(&cf->ايندڙ, معلومات, origin, peer, TRNSPRT_QUIC,
                              conn, addr, عدم, TRNSPRT_QUIC);
  إذا(نتيجو)
    اذهب ٻاهريون;
  cf->ايندڙ->conn = cf->conn;
  cf->ايندڙ->sockindex = cf->sockindex;

ٻاهريون:
  *pcf = (!نتيجو) ? cf : عدم;
  إذا(نتيجو) {
    إذا(cf)
      Curl_conn_cf_discard_chain(&cf, معلومات);
    وإلا إذا(ctx)
      cf_quiche_ctx_free(ctx);
  }

  أرجع نتيجو;
}

CURLcode Curl_cf_quiche_وجهو_after(بنية Curl_cfilter *cf_at,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     بنية Curl_cfilter **pcf)
{
  بنية cf_quiche_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode نتيجو;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }
  نتيجو = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  إذا(!نتيجو)
    نتيجو = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(نتيجو)
    اذهب ٻاهريون;
  Curl_conn_cf_وجهو_after(cf_at, cf);

ٻاهريون:
  إذا(نتيجو) {
    curlx_safefree(cf);
    إذا(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  أرجع نتيجو;
}

#جيڪڏهن_پورو
