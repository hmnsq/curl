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
#تعریف_کن CURL_H3_NO_ERROR  0x0100

#تعریف_کن MAX_PKT_مدت          10

#تعریف_کن QUIC_MAX_STREAMS       100

#تعریف_کن H3_جریان_پنجره_اندازه  (1024 * 128)
#تعریف_کن H3_جریان_CHUNK_اندازه   (1024 * 16)
/* Receive and Send max number of chunks follows from the
 * chunk size and window size */
#تعریف_کن H3_جریان_RECV_CHUNKS \
  (H3_جریان_پنجره_اندازه / H3_جریان_CHUNK_اندازه)

/*
 * Store quiche version info in this buffer.
 */
فراغ Curl_quiche_ver(حرف *p, size_t len)
{
  (فراغ)curl_msnprintf(p, len, "quiche/%s", quiche_نسخه());
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
  curl_off_t داده_recvd;
  بیت(initialized);
  بیت(goaway);                       /* got GOAWAY from server */
  بیت(x509_store_راهاندازی);             /* if x509 store has been set up */
  بیت(shutdown_started);             /* queued shutdown packets */
  بیت(stats_reported);               /* connect statistics reported */
};

#اگر_تعریف_شده DEBUG_QUICHE
/* initialize debug log callback only once */
ساكن صحيح debug_log_init = 0;
ساكن فراغ quiche_debug_log(ثابت حرف *سطر, فراغ *argp)
{
  (فراغ)argp;
  curl_mfprintf(stderr, "%s\n", سطر);
}
#پایان_اگر

ساكن فراغ h3_جریان_مجموعه_free(uint32_t id, فراغ *جریان);

ساكن CURLcode cf_quiche_ctx_init(بنية cf_quiche_ctx *ctx,
                                   بنية Curl_peer *origin,
                                   بنية Curl_peer *peer,
                                   بنية ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#اگر_تعریف_شده DEBUG_QUICHE
  إذا(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, عدم);
    debug_log_init = 1;
  }
#پایان_اگر
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_سرآیند);
  Curl_u32_ptrset_init(&ctx->streams, h3_جریان_مجموعه_free);
  Curl_bufq_init2(&ctx->writebuf, H3_جریان_CHUNK_اندازه, H3_جریان_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_حد);
  ctx->داده_recvd = 0;
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
                                بنية Curl_easy *داده);

/**
 * All about the H3 internals of a stream
 */
بنية h3_جریان_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  بنية h1_req_parser h1; /* h1 request parsing */
  uint64_t error3;         /* HTTP/3 stream error code */
  int32_t وضعیت_کد;     /* HTTP status code */
  CURLcode xfer_نتیجه;    /* result from cf_quiche_write_(hd/body) */
  بیت(opened);             /* TRUE after stream has been opened */
  بیت(closed);             /* TRUE on stream close */
  بیت(reset);              /* TRUE on stream reset */
  بیت(send_closed);        /* stream is locally closed */
  بیت(resp_hds_complete);  /* final response has been received */
  بیت(resp_got_سرآیند);    /* TRUE when h3 stream has recvd some HEADER */
  بیت(quic_flow_blocked);  /* stream is blocked by QUIC flow control */
};

ساكن فراغ h3_جریان_ctx_free(بنية h3_جریان_ctx *جریان)
{
  Curl_h1_req_parse_free(&جریان->h1);
  curlx_free(جریان);
}

ساكن فراغ h3_جریان_مجموعه_free(uint32_t id, فراغ *جریان)
{
  (فراغ)id;
  DEBUGASSERT(جریان);
  h3_جریان_ctx_free((بنية h3_جریان_ctx *)جریان);
}

تعريفنوع bool cf_quiche_svisit(بنية Curl_cfilter *cf,
                              بنية Curl_easy *sdata,
                              بنية h3_جریان_ctx *جریان,
                              فراغ *user_داده);

بنية cf_quiche_بازدید_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_multi *multi;
  cf_quiche_svisit *cb;
  فراغ *user_داده;
};

ساكن bool cf_quiche_جریان_do(uint32_t mid, فراغ *val, فراغ *user_داده)
{
  بنية cf_quiche_بازدید_ctx *vctx = user_داده;
  بنية h3_جریان_ctx *جریان = val;
  بنية Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  إذا(sdata)
    أرجع vctx->cb(vctx->cf, sdata, جریان, vctx->user_داده);
  أرجع TRUE;
}

ساكن فراغ cf_quiche_for_all_streams(بنية Curl_cfilter *cf,
                                      بنية Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      فراغ *user_داده)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية cf_quiche_بازدید_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_داده = user_داده;
  Curl_u32_ptrset_بازدید(&ctx->streams, cf_quiche_جریان_do, &vctx);
}

ساكن bool cf_quiche_do_resume(بنية Curl_cfilter *cf,
                                بنية Curl_easy *sdata,
                                بنية h3_جریان_ctx *جریان,
                                فراغ *user_داده)
{
  (فراغ)user_داده;
  إذا(جریان->quic_flow_blocked) {
    جریان->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", جریان->id);
  }
  أرجع TRUE;
}

ساكن bool cf_quiche_do_expire(بنية Curl_cfilter *cf,
                                بنية Curl_easy *sdata,
                                بنية h3_جریان_ctx *جریان,
                                فراغ *user_داده)
{
  (فراغ)user_داده;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  جریان->xfer_نتیجه = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  أرجع TRUE;
}

ساكن CURLcode h3_داده_راهاندازی(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);

  إذا(جریان)
    أرجع CURLE_OK;

  جریان = curlx_calloc(1, حجم(*جریان));
  إذا(!جریان)
    أرجع CURLE_بیرون_از_حافظه;

  جریان->id = -1;
  Curl_h1_req_parse_init(&جریان->h1, H1_PARSE_DEFAULT_MAX_سطر_LEN);

  إذا(Curl_u32_ptrset_مجموعه(&ctx->streams, داده->mid, جریان)) {
    h3_جریان_ctx_free(جریان);
    أرجع CURLE_بیرون_از_حافظه;
  }

  أرجع CURLE_OK;
}

ساكن فراغ cf_quiche_جریان_close(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   بنية h3_جریان_ctx *جریان)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجه;

  إذا(ctx->qconn && !جریان->closed) {
    quiche_conn_جریان_shutdown(ctx->qconn, جریان->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    إذا(!جریان->send_closed) {
      quiche_conn_جریان_shutdown(ctx->qconn, جریان->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      جریان->send_closed = TRUE;
    }
    جریان->closed = TRUE;
    نتیجه = cf_flush_egress(cf, داده);
    إذا(نتیجه)
      CURL_TRC_CF(داده, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  جریان->id, (صحيح)نتیجه);
  }
}

ساكن فراغ h3_داده_انجامشده(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);

  (فراغ)cf;
  إذا(جریان) {
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] easy handle is done", جریان->id);
    cf_quiche_جریان_close(cf, داده, جریان);
    Curl_u32_ptrset_unset(&ctx->streams, داده->mid);
  }
}

ساكن فراغ cf_quiche_expire_conn_closed(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *داده)
{
  DEBUGASSERT(داده->multi);
  CURL_TRC_CF(داده, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, داده->multi, cf_quiche_do_expire, عدم);
}

ساكن فراغ cf_quiche_write_hd(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               بنية h3_جریان_ctx *جریان,
                               ثابت حرف *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  إذا(!جریان->xfer_نتیجه) {
    جریان->xfer_نتیجه = Curl_xfer_write_resp_hd(داده, buf, blen, eos);
    إذا(جریان->xfer_نتیجه)
      CURL_TRC_CF(داده, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", جریان->id, (صحيح)جریان->xfer_نتیجه,
                  blen);
  }
}

بنية cb_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *داده;
  بنية h3_جریان_ctx *جریان;
};

ساكن bool is_valid_h3_سرآیند(ثابت uint8_t *hdr, size_t hlen)
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

ساكن صحيح cb_each_سرآیند(uint8_t *name, size_t name_len,
                          uint8_t *مقدار, size_t مقدار_len,
                          فراغ *argp)
{
  بنية cb_ctx *x = argp;
  بنية Curl_cfilter *cf = x->cf;
  بنية Curl_easy *داده = x->داده;
  بنية h3_جریان_ctx *جریان = x->جریان;
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  إذا(!جریان || جریان->xfer_نتیجه)
    أرجع 1; /* abort iteration */

  إذا((name_len == 7) && !مقایسه_رشته(HTTP_PSEUDO_وضعیت, (حرف *)name, 7) &&
     is_valid_h3_سرآیند(مقدار, مقدار_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    نتیجه = Curl_http_decode_وضعیت(&جریان->وضعیت_کد,
                                     (ثابت حرف *)مقدار, مقدار_len);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)مقدار, مقدار_len);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    إذا(!نتیجه)
      cf_quiche_write_hd(cf, داده, جریان, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] status: %s",
                جریان->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  وإلا {
    إذا(is_valid_h3_سرآیند(مقدار, مقدار_len) &&
       is_valid_h3_سرآیند(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(داده, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  جریان->id, (صحيح)name_len, name,
                  (صحيح)مقدار_len, مقدار);
      curlx_dyn_reset(&ctx->h1hdr);
      نتیجه = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)name, name_len);
      إذا(!نتیجه)
        نتیجه = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      إذا(!نتیجه)
        نتیجه = curlx_dyn_addn(&ctx->h1hdr, (ثابت حرف *)مقدار, مقدار_len);
      إذا(!نتیجه)
        نتیجه = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      إذا(!نتیجه)
        cf_quiche_write_hd(cf, داده, جریان, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    وإلا
      CURL_TRC_CF(x->داده, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  جریان->id, مقدار_len + name_len);
  }

  إذا(نتیجه) {
    CURL_TRC_CF(x->داده, x->cf, "[%" PRIu64 "] on header error %d",
                جریان->id, (صحيح)نتیجه);
    إذا(!جریان->xfer_نتیجه)
      جریان->xfer_نتیجه = نتیجه;
  }
  أرجع جریان->xfer_نتیجه ? 1 : 0;
}

ساكن CURLcode جریان_resp_read(فراغ *reader_ctx,
                                 بیعلامت حرف *buf, size_t len,
                                 size_t *pnread)
{
  بنية cb_ctx *x = reader_ctx;
  بنية cf_quiche_ctx *ctx = x->cf->ctx;
  بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, x->داده);
  ssize_t nread;

  *pnread = 0;
  إذا(!جریان)
    أرجع CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, جریان->id, buf, len);
  إذا(!curlx_sztouz(nread, pnread))
    أرجع CURLE_AGAIN;
  أرجع CURLE_OK;
}

ساكن فراغ cf_quiche_flush_body(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 بنية h3_جریان_ctx *جریان)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  ثابت uint8_t *buf;
  size_t blen;

  طالما(جریان && !جریان->xfer_نتیجه) {
    إذا(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      جریان->xfer_نتیجه = Curl_xfer_write_resp(
        داده, (ثابت حرف *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      إذا(جریان->xfer_نتیجه) {
        CURL_TRC_CF(داده, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", جریان->id, (صحيح)جریان->xfer_نتیجه, blen);
      }
    }
    وإلا
      اكسر;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

ساكن فراغ cf_quiche_recv_body(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                بنية h3_جریان_ctx *جریان)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  بنية cb_ctx cb_ctx;
  CURLcode نتیجه = CURLE_OK;

  إذا(!جریان)
    أرجع;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.داده = داده;
  cb_ctx.جریان = جریان;
  Curl_bufq_reset(&ctx->writebuf);
  طالما(!نتیجه) {
    نتیجه = Curl_bufq_slurp(&ctx->writebuf,
                             جریان_resp_read, &cb_ctx, &nread);
    إذا(!نتیجه)
      cf_quiche_flush_body(cf, داده, جریان);
    وإلا إذا(نتیجه == CURLE_AGAIN)
      اكسر;
    وإلا إذا(نتیجه) {
      CURL_TRC_CF(داده, cf, "[%" PRIu64 "] recv_body error %d",
                  جریان->id, (صحيح)نتیجه);
      failf(داده, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            جریان->id, (صحيح)نتیجه);
      جریان->closed = TRUE;
      جریان->reset = TRUE;
      جریان->send_closed = TRUE;
      إذا(!جریان->xfer_نتیجه)
        جریان->xfer_نتیجه = نتیجه;
    }
  }
  cf_quiche_flush_body(cf, داده, جریان);
}

ساكن فراغ cf_quiche_process_ev(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 بنية h3_جریان_ctx *جریان,
                                 quiche_h3_event *ev)
{
  تعداد quiche_h3_event_type type;

  إذا(!جریان)
    أرجع;

  type = quiche_h3_event_type(ev);

  بدل(type) {
  حالة QUICHE_H3_EVENT_سرآیندها: {
    بنية cb_ctx cb_ctx;
    جریان->resp_got_سرآیند = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.داده = داده;
    cb_ctx.جریان = جریان;
    quiche_h3_event_for_each_سرآیند(ev, cb_each_سرآیند, &cb_ctx);
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] <- [HEADERS]", جریان->id);
    Curl_multi_mark_dirty(داده);
    اكسر;
  }
  حالة QUICHE_H3_EVENT_داده:
    إذا(!جریان->resp_hds_complete) {
      جریان->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, داده, جریان, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, داده, جریان);
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] <- [DATA]", جریان->id);
    Curl_multi_mark_dirty(داده);
    اكسر;

  حالة QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] RESET", جریان->id);
    جریان->closed = TRUE;
    جریان->reset = TRUE;
    جریان->send_closed = TRUE;
    Curl_multi_mark_dirty(داده);
    اكسر;

  حالة QUICHE_H3_EVENT_پایانیافته:
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] CLOSED", جریان->id);
    إذا(!جریان->resp_hds_complete) {
      جریان->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, داده, جریان, "\r\n", 2, TRUE);
    }
    جریان->closed = TRUE;
    Curl_multi_mark_dirty(داده);
    اكسر;

  حالة QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] <- [GOAWAY]", جریان->id);
    اكسر;

  افتراضي:
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] recv, unhandled event %d",
                جریان->id, (صحيح)type);
    اكسر;
  }
}

بنية cf_quiche_disp_ctx {
  uint64_t جریان_id;
  بنية Curl_cfilter *cf;
  بنية Curl_multi *multi;
  quiche_h3_event *ev;
};

ساكن bool cf_quiche_disp_event(uint32_t mid, فراغ *val, فراغ *user_داده)
{
  بنية cf_quiche_disp_ctx *dctx = user_داده;
  بنية h3_جریان_ctx *جریان = val;

  إذا(جریان->id == dctx->جریان_id) {
    بنية Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    إذا(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, جریان, dctx->ev);
    أرجع FALSE; /* stop iterating */
  }
  أرجع TRUE;
}

ساكن CURLcode cf_poll_events(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = عدم;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  طالما(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    إذا(rv == QUICHE_H3_ERR_انجامشده) {
      اكسر;
    }
    وإلا إذا(rv < 0) {
      CURL_TRC_CF(داده, cf, "error poll: %" PRId64, rv);
      أرجع CURLE_HTTP3;
    }
    وإلا {
      جریان = H3_جریان_CTX(ctx, داده);
      إذا(جریان && جریان->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, داده, جریان, ev);
        quiche_h3_event_free(ev);
        إذا(جریان->xfer_نتیجه)
          أرجع جریان->xfer_نتیجه;
      }
      وإلا {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        بنية cf_quiche_disp_ctx dctx;
        dctx.جریان_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = داده->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_بازدید(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  أرجع CURLE_OK;
}

بنية recv_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *داده;
  صحيح pkts;
};

ساكن CURLcode cf_quiche_recv_pkts(ثابت بیعلامت حرف *buf, size_t buflen,
                                    size_t gso_اندازه,
                                    بنية sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    فراغ *userp)
{
  بنية recv_ctx *r = userp;
  بنية cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, جابه_جایی, nread;
  ssize_t rv;

  (فراغ)ecn;

  recv_info.مقصد = (بنية sockaddr *)&ctx->q.local_addr;
  recv_info.مقصد_len = ctx->q.local_addrlen;
  recv_info.from = (بنية sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  لكل(جابه_جایی = 0; جابه_جایی < buflen; جابه_جایی += gso_اندازه) {
    pktlen = ((جابه_جایی + gso_اندازه) <= buflen) ? gso_اندازه : (buflen - جابه_جایی);
    rv = quiche_conn_recv(ctx->qconn,
                          (بیعلامت حرف *)CURL_UNCONST(buf + جابه_جایی),
                          pktlen, &recv_info);
    إذا(!curlx_sztouz(rv, &nread)) {
      إذا(QUICHE_ERR_انجامشده == rv) {
        إذا(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->داده, r->cf, "ingress, connection is draining");
          أرجع CURLE_RECV_ERROR;
        }
        إذا(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->داده, r->cf, "ingress, connection is closed");
          أرجع CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->داده, r->cf, "ingress, quiche is DONE");
        أرجع CURLE_OK;
      }
      وإلا إذا(QUICHE_ERR_TLS_FAIL == rv) {
        دراز verify_ok = SSL_get_verify_نتیجه(ctx->tls.ossl.ssl);
        إذا(verify_ok != X509_V_OK) {
          failf(r->داده, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          أرجع CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->داده, "ingress, quiche reports TLS fail");
        أرجع CURLE_RECV_ERROR;
      }
      وإلا {
        failf(r->داده, "quiche reports error %zd on receive", rv);
        أرجع CURLE_RECV_ERROR;
      }
    }
    وإلا إذا(nread < pktlen) {
      CURL_TRC_CF(r->داده, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode cf_process_ingress(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية recv_ctx rctx;
  CURLcode نتیجه;

  DEBUGASSERT(ctx->qconn);
  نتیجه = Curl_vquic_tls_before_recv(&ctx->tls, cf, داده);
  إذا(نتیجه)
    أرجع نتیجه;

  rctx.cf = cf;
  rctx.داده = داده;
  rctx.pkts = 0;

  نتیجه = Curl_vquic_recv_packets(cf, داده, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(داده->multi);
    cf_quiche_for_all_streams(cf, داده->multi, cf_quiche_do_resume, عدم);
  }
  أرجع cf_poll_events(cf, داده);
}

بنية read_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *داده;
  quiche_send_info send_info;
};

ساكن CURLcode read_pkt_مقصد_send(فراغ *userp,
                                 بیعلامت حرف *buf, size_t buflen,
                                 size_t *pnread)
{
  بنية read_ctx *x = userp;
  بنية cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  إذا(rv == QUICHE_ERR_انجامشده)
    أرجع CURLE_AGAIN;

  إذا(!curlx_sztouz(rv, pnread)) {
    failf(x->داده, "quiche_conn_send returned %zd", rv);
    أرجع CURLE_SEND_ERROR;
  }
  أرجع CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
ساكن CURLcode cf_flush_egress(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode نتیجه;
  int64_t expiry_ns;
  int64_t timeout_ns;
  بنية read_ctx readx;
  size_t pkt_شمار, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  إذا(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    إذا(quiche_conn_is_closed(ctx->qconn)) {
      إذا(quiche_conn_is_timed_بیرون(ctx->qconn))
        failf(داده, "connection closed by idle timeout");
      وإلا
        failf(داده, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, داده);
      أرجع CURLE_SEND_ERROR;
    }
  }

  نتیجه = Curl_vquic_flush(cf, داده, &ctx->q);
  إذا(نتیجه) {
    إذا(نتیجه == CURLE_AGAIN) {
      Curl_expire(داده, 1, EXPIRE_QUIC);
      أرجع CURLE_OK;
    }
    أرجع نتیجه;
  }

  readx.cf = cf;
  readx.داده = داده;
  memset(&readx.send_info, 0, حجم(readx.send_info));
  pkt_شمار = 0;
  gsolen = quiche_conn_max_send_udp_payload_اندازه(ctx->qconn);
  لكل(;;) {
    /* add the next packet to send, if any, to our buffer */
    نتیجه = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_مقصد_send, &readx, &nread);
    إذا(نتیجه) {
      إذا(نتیجه != CURLE_AGAIN)
        أرجع نتیجه;
      /* Nothing more to add, flush and leave */
      نتیجه = Curl_vquic_send(cf, داده, &ctx->q, gsolen);
      إذا(نتیجه) {
        إذا(نتیجه == CURLE_AGAIN) {
          Curl_expire(داده, 1, EXPIRE_QUIC);
          أرجع CURLE_OK;
        }
        أرجع نتیجه;
      }
      اذهب بیرون;
    }

    ++pkt_شمار;
    إذا(nread < gsolen || pkt_شمار >= MAX_PKT_مدت) {
      نتیجه = Curl_vquic_send(cf, داده, &ctx->q, gsolen);
      إذا(نتیجه) {
        إذا(نتیجه == CURLE_AGAIN) {
          Curl_expire(داده, 1, EXPIRE_QUIC);
          أرجع CURLE_OK;
        }
        اذهب بیرون;
      }
      pkt_شمار = 0;
    }
  }

بیرون:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  إذا(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(داده, (timeout_ns / 1000000), EXPIRE_QUIC);
  أرجع نتیجه;
}

ساكن CURLcode recv_closed_جریان(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   size_t *pnread)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(جریان);
  *pnread = 0;
  إذا(جریان->reset) {
    إذا(جریان->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(داده, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", جریان->id);
      connclose(cf->conn); /* do not use this anymore */
      داده->state.refused_جریان = TRUE;
      أرجع CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وإلا إذا(جریان->resp_hds_complete && داده->req.no_body) {
        CURL_TRC_CF(داده, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", جریان->id, جریان->error3,
                    Curl_vquic_h3_err_str(جریان->error3));
        أرجع CURLE_OK;
    }
    failf(داده, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", جریان->id, جریان->error3,
          Curl_vquic_h3_err_str(جریان->error3));
    نتیجه = داده->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                جریان->id, (صحيح)نتیجه);
  }
  وإلا إذا(!جریان->resp_got_سرآیند) {
    failf(داده, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          جریان->id);
    نتیجه = CURLE_HTTP3;
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_quiche_recv(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                               حرف *buf, size_t blen, size_t *pnread)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);
  CURLcode نتیجه = CURLE_OK;

  *pnread = 0;
  (فراغ)buf;
  (فراغ)blen;
  Curl_vquic_ctx_update_زمان(&ctx->q, Curl_pgrs_now(داده));

  إذا(!جریان)
    أرجع CURLE_RECV_ERROR;

  نتیجه = cf_process_ingress(cf, داده);
  إذا(نتیجه) {
    CURL_TRC_CF(داده, cf, "cf_recv, error on ingress");
    اذهب بیرون;
  }

  إذا(جریان->xfer_نتیجه) {
    cf_quiche_جریان_close(cf, داده, جریان);
    نتیجه = جریان->xfer_نتیجه;
    اذهب بیرون;
  }
  وإلا إذا(جریان->closed)
    نتیجه = recv_closed_جریان(cf, داده, pnread);
  وإلا إذا(quiche_conn_is_draining(ctx->qconn)) {
    failf(داده, "QUIC connection is draining");
    نتیجه = CURLE_HTTP3;
  }
  وإلا
    نتیجه = CURLE_AGAIN;

بیرون:
  نتیجه = Curl_1st_fatal(نتیجه, cf_flush_egress(cf, داده));
  إذا(*pnread > 0)
    ctx->داده_recvd += *pnread;
  CURL_TRC_CF(داده, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, جریان->id, blen, (صحيح)نتیجه, *pnread,
              ctx->داده_recvd);
  أرجع نتیجه;
}

ساكن CURLcode cf_quiche_send_body(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    بنية h3_جریان_ctx *جریان,
                                    ثابت uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, جریان->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  إذا(rv == QUICHE_H3_ERR_انجامشده || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    إذا(!quiche_conn_جریان_writable(ctx->qconn, جریان->id, len)) {
      CURL_TRC_CF(داده, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", جریان->id, len);
      جریان->quic_flow_blocked = TRUE;
    }
    أرجع CURLE_AGAIN;
  }
  وإلا إذا(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_جریان_STATE) {
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", جریان->id, len);
    أرجع CURLE_HTTP3;
  }
  وإلا إذا(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_اندازه) {
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                جریان->id, len);
    أرجع CURLE_SEND_ERROR;
  }
  وإلا إذا(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                جریان->id, len, rv);
    أرجع CURLE_SEND_ERROR;
  }
  وإلا {
    إذا(eos && (len == *pnwritten))
      جریان->send_closed = TRUE;
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                جریان->id, len, جریان->send_closed, *pnwritten);
    أرجع CURLE_OK;
  }
}

ساكن CURLcode h3_open_جریان(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               ثابت uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);
  size_t nheader, i;
  int64_t rv;
  بنية dynhds h2_سرآیندها;
  quiche_h3_سرآیند *nva = عدم;
  CURLcode نتیجه = CURLE_OK;

  *pnwritten = 0;
  إذا(!جریان) {
    نتیجه = h3_داده_راهاندازی(cf, داده);
    إذا(نتیجه)
      أرجع نتیجه;
    جریان = H3_جریان_CTX(ctx, داده);
    DEBUGASSERT(جریان);
  }

  Curl_dynhds_init(&h2_سرآیندها, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(جریان);

  نتیجه = Curl_h1_req_parse_read(&جریان->h1, buf, blen, عدم,
    !داده->state.http_ignorecustom ?
    CURL_EASY_STR(داده, STRING_CUSTOMREQUEST) : عدم,
    0, pnwritten);
  إذا(نتیجه)
    اذهب بیرون;
  إذا(!جریان->h1.انجامشده) {
    /* need more data */
    اذهب بیرون;
  }
  DEBUGASSERT(جریان->h1.req);

  نتیجه = Curl_http_req_مقصد_h2(&h2_سرآیندها, جریان->h1.req, داده);
  إذا(نتیجه)
    اذهب بیرون;

  /* no longer needed */
  Curl_h1_req_parse_free(&جریان->h1);

  nheader = Curl_dynhds_شمار(&h2_سرآیندها);
  nva = curlx_malloc(حجم(quiche_h3_سرآیند) * nheader);
  إذا(!nva) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  لكل(i = 0; i < nheader; ++i) {
    بنية dynhds_مدخل *e = Curl_dynhds_getn(&h2_سرآیندها, i);
    nva[i].name = (بیعلامت حرف *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].مقدار = (بیعلامت حرف *)e->مقدار;
    nva[i].مقدار_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  إذا(eos && !blen)
    جریان->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              جریان->send_closed);
  CURL_TRC_CF(داده, cf, "quiche_send_request() -> %" PRId64, rv);
  إذا(rv < 0) {
    إذا(QUICHE_H3_ERR_جریان_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(داده, cf, "[%" PRIu64 "] blocked", جریان->id);
      جریان->quic_flow_blocked = TRUE;
      نتیجه = CURLE_AGAIN;
      اذهب بیرون;
    }
    وإلا {
      CURL_TRC_CF(داده, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&داده->state.url), rv);
    }
    نتیجه = CURLE_SEND_ERROR;
    اذهب بیرون;
  }

  DEBUGASSERT(!جریان->opened);
  جریان->id = (uint64_t)rv;
  جریان->opened = TRUE;
  جریان->closed = FALSE;
  جریان->reset = FALSE;

  إذا(Curl_trc_is_verbose(داده)) {
    infof(داده, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          جریان->id, Curl_bufref_ptr(&داده->state.url));
    لكل(i = 0; i < nheader; ++i) {
      infof(داده, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", جریان->id,
            (صحيح)nva[i].name_len, nva[i].name,
            (صحيح)nva[i].مقدار_len, nva[i].مقدار);
    }
  }

  إذا(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, داده, جریان, buf, blen, eos, &nwritten);
    إذا(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      نتیجه = r2;
    }
    وإلا إذا(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

بیرون:
  curlx_free(nva);
  Curl_dynhds_free(&h2_سرآیندها);
  أرجع نتیجه;
}

ساكن CURLcode cf_quiche_send(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                               ثابت uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);
  CURLcode نتیجه;

  *pnwritten = 0;
  Curl_vquic_ctx_update_زمان(&ctx->q, Curl_pgrs_now(داده));

  نتیجه = cf_process_ingress(cf, داده);
  إذا(نتیجه)
    اذهب بیرون;

  إذا(!جریان || !جریان->opened) {
    نتیجه = h3_open_جریان(cf, داده, buf, len, eos, pnwritten);
    إذا(نتیجه)
      اذهب بیرون;
    جریان = H3_جریان_CTX(ctx, داده);
  }
  وإلا إذا(جریان->xfer_نتیجه) {
    cf_quiche_جریان_close(cf, داده, جریان);
    نتیجه = جریان->xfer_نتیجه;
  }
  وإلا إذا(جریان->closed) {
    إذا(جریان->resp_hds_complete) {
      /* sending request body on a stream that has been closed by the
       * server. If the server has send us a final response, we should
       * silently discard the send data.
       * This happens for example on redirects where the server, instead
       * of reading the full request body closed the stream after
       * sending the 30x response.
       * This is sort of a race: had the transfer loop called recv first,
       * it would see the response and stop/discard sending on its own- */
      CURL_TRC_CF(داده, cf, "[%" PRIu64 "] discarding data "
                  "on closed stream with response", جریان->id);
      نتیجه = CURLE_OK;
      *pnwritten = len;
      اذهب بیرون;
    }
    CURL_TRC_CF(داده, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", جریان->id, len);
    نتیجه = CURLE_HTTP3;
    اذهب بیرون;
  }
  وإلا {
    نتیجه = cf_quiche_send_body(cf, داده, جریان, buf, len, eos, pnwritten);
  }

بیرون:
  نتیجه = Curl_1st_fatal(نتیجه, cf_flush_egress(cf, داده));

  CURL_TRC_CF(داده, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              جریان ? جریان->id : (uint64_t)~0, len,
              (صحيح)نتیجه, *pnwritten);
  أرجع نتیجه;
}

ساكن bool جریان_is_writable(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);

  أرجع جریان && (quiche_conn_جریان_writable(
    ctx->qconn, جریان->id, 1) > 0);
}

ساكن CURLcode cf_quiche_adjust_pollset(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *داده,
                                         بنية easy_pollset *ps)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode نتیجه = CURLE_OK;

  إذا(!ctx->qconn)
    أرجع CURLE_OK;

  Curl_pollset_check(داده, ps, ctx->q.sockfd, &want_recv, &want_send);
  إذا(want_recv || want_send) {
    بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);
    bool c_exhaust, s_exhaust;

    c_exhaust = FALSE; /* Have not found any call in quiche that tells
                          us if the connection itself is blocked */
    s_exhaust = want_send && جریان && جریان->opened &&
                (جریان->quic_flow_blocked || !جریان_is_writable(cf, داده));
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_خالی(&ctx->q.sendbuf);

    نتیجه = Curl_pollset_مجموعه(داده, ps, ctx->q.sockfd, want_recv, want_send);
  }
  أرجع نتیجه;
}

ساكن CURLcode h3_داده_pause(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (فراغ)cf;
  إذا(!pause) {
    Curl_multi_mark_dirty(داده);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_quiche_cntrl(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  (فراغ)arg1;
  (فراغ)arg2;
  بدل(event) {
  حالة CF_CTRL_داده_راهاندازی:
    اكسر;
  حالة CF_CTRL_داده_PAUSE:
    نتیجه = h3_داده_pause(cf, داده, (arg1 != 0));
    اكسر;
  حالة CF_CTRL_داده_انجامشده:
    h3_داده_انجامشده(cf, داده);
    اكسر;
  حالة CF_CTRL_داده_انجامشده_SEND: {
    بنية h3_جریان_ctx *جریان = H3_جریان_CTX(ctx, داده);
    إذا(جریان && !جریان->send_closed) {
      بیعلامت حرف body[1];
      size_t فرستاده;

      جریان->send_closed = TRUE;
      body[0] = 'X';
      نتیجه = cf_quiche_send(cf, داده, body, 0, TRUE, &فرستاده);
      CURL_TRC_CF(داده, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  جریان->id, (صحيح)نتیجه, فرستاده);
    }
    اكسر;
  }
  حالة CF_CTRL_CONN_INFO_UPDATE:
    إذا(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_دیده = 30;
      Curl_conn_مجموعه_multiplex(cf->conn);
    }
    اكسر;
  حالة CF_CTRL_REPORT_STATS:
    إذا(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrs_زمان_Was(داده, TIMER_CONNECT, ctx->q.اولی_byte_at);
      Curl_pgrs_زمان_Was(داده, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_quiche_ctx_open(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  صحيح rv;
  CURLcode نتیجه;
  ثابت بنية Curl_sockaddr_ex *sockaddr;
  ساكن ثابت بنية alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  نتیجه = Curl_vquic_ctx_init(داده, &ctx->q);
  إذا(نتیجه)
    أرجع نتیجه;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_نسخه);
  إذا(!ctx->cfg) {
    failf(داده, "cannot create quiche config");
    أرجع CURLE_FAILED_INIT;
  }
  quiche_config_enable_pacing(ctx->cfg, FALSE);
  quiche_config_مجموعه_initial_max_داده(ctx->cfg, (1 * 1024 * 1024)
    /* (QUIC_MAX_STREAMS/2) * H3_STREAM_WINDOW_SIZE */);
  quiche_config_مجموعه_initial_max_streams_bidi(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_مجموعه_initial_max_streams_uni(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_مجموعه_initial_max_جریان_داده_bidi_local(ctx->cfg,
    H3_جریان_پنجره_اندازه);
  quiche_config_مجموعه_initial_max_جریان_داده_bidi_remote(ctx->cfg,
    H3_جریان_پنجره_اندازه);
  quiche_config_مجموعه_initial_max_جریان_داده_uni(ctx->cfg,
    H3_جریان_پنجره_اندازه);
  quiche_config_مجموعه_disable_فعال_migration(ctx->cfg, TRUE);

  quiche_config_مجموعه_max_connection_پنجره(ctx->cfg,
    10 * QUIC_MAX_STREAMS * H3_جریان_پنجره_اندازه);
  quiche_config_مجموعه_max_جریان_پنجره(ctx->cfg, 10 * H3_جریان_پنجره_اندازه);
  quiche_config_مجموعه_application_protos(ctx->cfg,
                      (uint8_t *)CURL_UNCONST(QUICHE_H3_APPLICATION_PROTOCOL),
                                 CURL_CSTRLEN(QUICHE_H3_APPLICATION_PROTOCOL));

  نتیجه = Curl_vquic_tls_init(&ctx->tls, cf, داده, &ctx->ssl_peer,
                               &ALPN_SPEC_H3, عدم, عدم, cf, عدم);
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = Curl_rand(داده, ctx->scid, حجم(ctx->scid));
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(Curl_cf_socket_peek(cf->بعدی, داده, &ctx->q.sockfd, &sockaddr, عدم))
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
    failf(داده, "cannot create quiche connection");
    أرجع CURLE_بیرون_از_حافظه;
  }

  /* Known to not work on Windows */
#إذا !defined(_WIN32) && defined(HAVE_QUICHE_CONN_مجموعه_QLOG_FD)
  {
    صحيح qfd;
    (فراغ)Curl_qlogdir(داده, ctx->scid, حجم(ctx->scid), &qfd);
    إذا(qfd != -1)
      quiche_conn_مجموعه_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#پایان_اگر

  نتیجه = cf_flush_egress(cf, داده);
  إذا(نتیجه)
    أرجع نتیجه;

  {
    بیعلامت حرف alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    بیعلامت alpn_len, جابه_جایی = 0;

    /* Replace each ALPN length prefix by a comma. */
    طالما(جابه_جایی < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[جابه_جایی];
      alpn_protocols[جابه_جایی] = ',';
      جابه_جایی += 1 + alpn_len;
    }

    CURL_TRC_CF(داده, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  أرجع CURLE_OK;
}

ساكن CURLcode cf_quiche_verify_peer(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  أرجع Curl_vquic_tls_verify_peer(&ctx->tls, cf, داده, &ctx->ssl_peer);
}

ساكن CURLcode cf_quiche_connect(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  bool *انجامشده)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the UDP filter first */
  إذا(!cf->بعدی->connected) {
    نتیجه = Curl_conn_cf_connect(cf->بعدی, داده, انجامشده);
    إذا(نتیجه || !*انجامشده)
      أرجع نتیجه;
  }

  *انجامشده = FALSE;
  إذا(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(داده, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(داده, cf, "need HTTPS-RR, delaying connect");
    أرجع CURLE_OK;
  }

  Curl_vquic_ctx_update_زمان(&ctx->q, Curl_pgrs_now(داده));

  إذا(!ctx->qconn) {
    نتیجه = cf_quiche_ctx_open(cf, داده);
    إذا(نتیجه)
      اذهب بیرون;
    ctx->started_at = ctx->q.آخرین_op;
    نتیجه = cf_flush_egress(cf, داده);
    /* we do not expect to be able to recv anything yet */
    اذهب بیرون;
  }

  نتیجه = cf_process_ingress(cf, داده);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = cf_flush_egress(cf, داده);
  إذا(نتیجه)
    اذهب بیرون;

  إذا(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.آخرین_op;
    CURL_TRC_CF(داده, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    نتیجه = cf_quiche_verify_peer(cf, داده);
    إذا(!نتیجه) {
      CURL_TRC_CF(داده, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      إذا(!ctx->h3config) {
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب بیرون;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_مجموعه_max_فیلد_section_اندازه(ctx->h3config,
                                                  MAX_HTTP_RESP_سرآیند_اندازه);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      إذا(!ctx->h3c) {
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب بیرون;
      }
      cf->connected = TRUE;
      *انجامشده = TRUE;
    }
  }
  وإلا إذا(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    نتیجه = CURLE_WEIRD_SERVER_پاسخ;
  }

بیرون:
#اگر_تعریف_شده CURLVERBOSE
  إذا(نتیجه && نتیجه != CURLE_AGAIN) {
    بنية ip_quadruple ip;

    إذا(!Curl_cf_socket_peek(cf->بعدی, داده, عدم, عدم, &ip))
      infof(داده, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_پورت, curl_easy_strerror(نتیجه));
    وإلا
      infof(داده, "connect failed: %s", curl_easy_strerror(نتیجه));
  }
#پایان_اگر
  أرجع نتیجه;
}

ساكن CURLcode cf_quiche_shutdown(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده, bool *انجامشده)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;

  إذا(cf->shutdown || !ctx || !ctx->qconn) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  *انجامشده = FALSE;
  إذا(!ctx->shutdown_started) {
    صحيح err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_زمان(&ctx->q, Curl_pgrs_now(داده));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, عدم, 0);
    إذا(err) {
      CURL_TRC_CF(داده, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
  }

  إذا(!Curl_bufq_is_خالی(&ctx->q.sendbuf)) {
    CURL_TRC_CF(داده, cf, "shutdown, flushing sendbuf");
    نتیجه = cf_flush_egress(cf, داده);
    إذا(نتیجه)
      اذهب بیرون;
  }

  إذا(Curl_bufq_is_خالی(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(داده, cf, "shutdown completely sent off, done");
    *انجامشده = TRUE;
  }
  وإلا {
    CURL_TRC_CF(داده, cf, "shutdown sending blocked");
  }

بیرون:
  أرجع نتیجه;
}

ساكن فراغ cf_quiche_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  (فراغ)داده;
  إذا(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_quiche_query(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    إذا(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_چپ_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > بیشترین_صحیح) ? بیشترین_صحیح : (صحيح)max_streams;
    CURL_TRC_CF(داده, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    أرجع CURLE_OK;
  }
  حالة CF_QUERY_CONNECT_پاسخ_MS:
    إذا(ctx->q.got_اولی_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.اولی_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < بیشترین_صحیح) ? (صحيح)ms : بیشترین_صحیح;
    }
    وإلا
      *pres1 = -1;
    أرجع CURLE_OK;
  حالة CF_QUERY_HTTP_نسخه:
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
  أرجع cf->بعدی ?
    cf->بعدی->cft->query(cf->بعدی, داده, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن bool cf_quiche_conn_is_alive(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    bool *درون_داد_pending)
{
  بنية cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *درون_داد_pending = FALSE;
  إذا(!ctx->qconn)
    أرجع FALSE;

  إذا(quiche_conn_is_closed(ctx->qconn)) {
    إذا(quiche_conn_is_timed_بیرون(ctx->qconn))
      CURL_TRC_CF(داده, cf, "connection was closed due to idle timeout");
    وإلا
      CURL_TRC_CF(داده, cf, "connection is closed");
    أرجع FALSE;
  }

  إذا(!cf->بعدی || !cf->بعدی->cft->is_alive(cf->بعدی, داده, درون_داد_pending))
    أرجع FALSE;

  إذا(*درون_داد_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *درون_داد_pending = FALSE;
    إذا(cf_process_ingress(cf, داده))
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
  Curl_cf_def_داده_pending,
  cf_quiche_send,
  cf_quiche_recv,
  cf_quiche_cntrl,
  cf_quiche_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_quiche_query,
};

CURLcode Curl_cf_quiche_create(بنية Curl_cfilter **pcf,
                               بنية Curl_easy *داده,
                               بنية Curl_peer *origin,
                               بنية Curl_peer *peer,
                               بنية connectdata *conn,
                               بنية Curl_sockaddr_ex *addr)
{
  بنية cf_quiche_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode نتیجه;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  نتیجه = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  إذا(!نتیجه)
    نتیجه = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(نتیجه)
    اذهب بیرون;
  cf->conn = conn;

  نتیجه = Curl_cf_udp_create(&cf->بعدی, داده, origin, peer, TRNSPRT_QUIC,
                              conn, addr, عدم, TRNSPRT_QUIC);
  إذا(نتیجه)
    اذهب بیرون;
  cf->بعدی->conn = cf->conn;
  cf->بعدی->sockindex = cf->sockindex;

بیرون:
  *pcf = (!نتیجه) ? cf : عدم;
  إذا(نتیجه) {
    إذا(cf)
      Curl_conn_cf_discard_chain(&cf, داده);
    وإلا إذا(ctx)
      cf_quiche_ctx_free(ctx);
  }

  أرجع نتیجه;
}

CURLcode Curl_cf_quiche_درج_after(بنية Curl_cfilter *cf_at,
                                     بنية Curl_peer *origin,
                                     بنية Curl_peer *peer,
                                     بنية Curl_cfilter **pcf)
{
  بنية cf_quiche_ctx *ctx = عدم;
  بنية Curl_cfilter *cf = عدم;
  CURLcode نتیجه;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  نتیجه = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  إذا(!نتیجه)
    نتیجه = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  إذا(نتیجه)
    اذهب بیرون;
  Curl_conn_cf_درج_after(cf_at, cf);

بیرون:
  إذا(نتیجه) {
    curlx_safefree(cf);
    إذا(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  أرجع نتیجه;
}

#پایان_اگر
