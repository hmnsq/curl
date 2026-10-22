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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#ਅਨ੍ਤਰ੍ਭਾਵ <quiche.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/err.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/ssl.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "u32_ptrset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-socket.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http1.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sockaddr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/cf-dns.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic-tls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/cf-quiche.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/openssl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"

/* HTTP/3 error values defined in RFC 9114, ch. 8.1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_H3_NO_ERROR  0x0100

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_PKT_ਮਿਆਦ          10

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QUIC_MAX_STREAMS       100

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ  (1024 * 128)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H3_ਧਾਰਾ_CHUNK_ਮਾਪ   (1024 * 16)
/* Receive and Send max number of chunks follows from the
 * chunk size and window size */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H3_ਧਾਰਾ_RECV_CHUNKS \
  (H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ / H3_ਧਾਰਾ_CHUNK_ਮਾਪ)

/*
 * Store quiche version info in this buffer.
 */
ਖਾਲੀ Curl_quiche_ver(ਅੱਖਰ *p, size_t len)
{
  (ਖਾਲੀ)curl_msnprintf(p, len, "quiche/%s", quiche_ਸੰਸਕਰਣ());
}

ਬਣਤਰ cf_quiche_ctx {
  ਬਣਤਰ cf_quic_ctx q;
  ਬਣਤਰ ssl_peer ssl_peer;
  ਬਣਤਰ curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  ਬਣਤਰ curltime started_at;        /* time the current attempt started */
  ਬਣਤਰ curltime handshake_at;      /* time connect handshake finished */
  ਬਣਤਰ u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  ਬਣਤਰ dynbuf h1hdr;               /* temp buffer for header construction */
  ਬਣਤਰ bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t ਅੰਕੜੇ_recvd;
  ਦੋਆਧਾਰੀ_ਅੰਕ(initialized);
  ਦੋਆਧਾਰੀ_ਅੰਕ(goaway);                       /* got GOAWAY from server */
  ਦੋਆਧਾਰੀ_ਅੰਕ(x509_store_ਤਿਆਰੀ);             /* if x509 store has been set up */
  ਦੋਆਧਾਰੀ_ਅੰਕ(shutdown_started);             /* queued shutdown packets */
  ਦੋਆਧਾਰੀ_ਅੰਕ(stats_reported);               /* connect statistics reported */
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_QUICHE
/* initialize debug log callback only once */
ਸਥਿਰ ਪੂਰਨ debug_log_init = 0;
ਸਥਿਰ ਖਾਲੀ quiche_debug_log(ਅਚਲ ਅੱਖਰ *ਸਤਰ, ਖਾਲੀ *argp)
{
  (ਖਾਲੀ)argp;
  curl_mfprintf(stderr, "%s\n", ਸਤਰ);
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ h3_ਧਾਰਾ_ਸੈੱਟ_free(uint32_t id, ਖਾਲੀ *ਧਾਰਾ);

ਸਥਿਰ CURLcode cf_quiche_ctx_init(ਬਣਤਰ cf_quiche_ctx *ctx,
                                   ਬਣਤਰ Curl_peer *origin,
                                   ਬਣਤਰ Curl_peer *peer,
                                   ਬਣਤਰ ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_QUICHE
  ਜੇ(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, ਨਿਰਾ);
    debug_log_init = 1;
  }
#ਜੇ_ਅੰਤ
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_ਸਿਰਲੇਖ);
  Curl_u32_ptrset_init(&ctx->streams, h3_ਧਾਰਾ_ਸੈੱਟ_free);
  Curl_bufq_init2(&ctx->writebuf, H3_ਧਾਰਾ_CHUNK_ਮਾਪ, H3_ਧਾਰਾ_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ);
  ctx->ਅੰਕੜੇ_recvd = 0;
  ctx->initialized = TRUE;
  ਵਾਪਸ Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

ਸਥਿਰ ਖਾਲੀ cf_quiche_ctx_free(ਬਣਤਰ cf_quiche_ctx *ctx)
{
  ਜੇ(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = ਨਿਰਾ;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

ਸਥਿਰ ਖਾਲੀ cf_quiche_ctx_close(ਬਣਤਰ cf_quiche_ctx *ctx)
{
  ਜੇ(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = ਨਿਰਾ;
  }
  ਜੇ(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = ਨਿਰਾ;
  }
  ਜੇ(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = ਨਿਰਾ;
  }
  ਜੇ(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = ਨਿਰਾ;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

ਸਥਿਰ CURLcode cf_flush_egress(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/**
 * All about the H3 internals of a stream
 */
ਬਣਤਰ h3_ਧਾਰਾ_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  ਬਣਤਰ h1_req_parser h1; /* h1 request parsing */
  uint64_t error3;         /* HTTP/3 stream error code */
  int32_t ਸਥਿਤੀ_ਕੋਡ;     /* HTTP status code */
  CURLcode xfer_ਨਤੀਜਾ;    /* result from cf_quiche_write_(hd/body) */
  ਦੋਆਧਾਰੀ_ਅੰਕ(opened);             /* TRUE after stream has been opened */
  ਦੋਆਧਾਰੀ_ਅੰਕ(closed);             /* TRUE on stream close */
  ਦੋਆਧਾਰੀ_ਅੰਕ(reset);              /* TRUE on stream reset */
  ਦੋਆਧਾਰੀ_ਅੰਕ(send_closed);        /* stream is locally closed */
  ਦੋਆਧਾਰੀ_ਅੰਕ(resp_hds_complete);  /* final response has been received */
  ਦੋਆਧਾਰੀ_ਅੰਕ(resp_got_ਸਿਰਲੇਖ);    /* TRUE when h3 stream has recvd some HEADER */
  ਦੋਆਧਾਰੀ_ਅੰਕ(quic_flow_blocked);  /* stream is blocked by QUIC flow control */
};

ਸਥਿਰ ਖਾਲੀ h3_ਧਾਰਾ_ctx_free(ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ)
{
  Curl_h1_req_parse_free(&ਧਾਰਾ->h1);
  curlx_free(ਧਾਰਾ);
}

ਸਥਿਰ ਖਾਲੀ h3_ਧਾਰਾ_ਸੈੱਟ_free(uint32_t id, ਖਾਲੀ *ਧਾਰਾ)
{
  (ਖਾਲੀ)id;
  DEBUGASSERT(ਧਾਰਾ);
  h3_ਧਾਰਾ_ctx_free((ਬਣਤਰ h3_ਧਾਰਾ_ctx *)ਧਾਰਾ);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ bool cf_quiche_svisit(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *sdata,
                              ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ,
                              ਖਾਲੀ *user_ਅੰਕੜੇ);

ਬਣਤਰ cf_quiche_ਦੇਖੋ_ctx {
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ Curl_multi *multi;
  cf_quiche_svisit *cb;
  ਖਾਲੀ *user_ਅੰਕੜੇ;
};

ਸਥਿਰ bool cf_quiche_ਧਾਰਾ_do(uint32_t mid, ਖਾਲੀ *val, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_quiche_ਦੇਖੋ_ctx *vctx = user_ਅੰਕੜੇ;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = val;
  ਬਣਤਰ Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  ਜੇ(sdata)
    ਵਾਪਸ vctx->cb(vctx->cf, sdata, ਧਾਰਾ, vctx->user_ਅੰਕੜੇ);
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ ਖਾਲੀ cf_quiche_for_all_streams(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ਬਣਤਰ cf_quiche_ਦੇਖੋ_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_ਅੰਕੜੇ = user_ਅੰਕੜੇ;
  Curl_u32_ptrset_ਦੇਖੋ(&ctx->streams, cf_quiche_ਧਾਰਾ_do, &vctx);
}

ਸਥਿਰ bool cf_quiche_do_resume(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *sdata,
                                ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ,
                                ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  (ਖਾਲੀ)user_ਅੰਕੜੇ;
  ਜੇ(ਧਾਰਾ->quic_flow_blocked) {
    ਧਾਰਾ->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", ਧਾਰਾ->id);
  }
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ bool cf_quiche_do_expire(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *sdata,
                                ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ,
                                ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  (ਖਾਲੀ)user_ਅੰਕੜੇ;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  ਧਾਰਾ->xfer_ਨਤੀਜਾ = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ CURLcode h3_ਅੰਕੜੇ_ਤਿਆਰੀ(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);

  ਜੇ(ਧਾਰਾ)
    ਵਾਪਸ CURLE_OK;

  ਧਾਰਾ = curlx_calloc(1, ਆਕਾਰ(*ਧਾਰਾ));
  ਜੇ(!ਧਾਰਾ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਧਾਰਾ->id = -1;
  Curl_h1_req_parse_init(&ਧਾਰਾ->h1, H1_PARSE_DEFAULT_MAX_ਸਤਰ_LEN);

  ਜੇ(Curl_u32_ptrset_ਸੈੱਟ(&ctx->streams, ਅੰਕੜੇ->mid, ਧਾਰਾ)) {
    h3_ਧਾਰਾ_ctx_free(ਧਾਰਾ);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cf_quiche_ਧਾਰਾ_close(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(ctx->qconn && !ਧਾਰਾ->closed) {
    quiche_conn_ਧਾਰਾ_shutdown(ctx->qconn, ਧਾਰਾ->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    ਜੇ(!ਧਾਰਾ->send_closed) {
      quiche_conn_ਧਾਰਾ_shutdown(ctx->qconn, ਧਾਰਾ->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      ਧਾਰਾ->send_closed = TRUE;
    }
    ਧਾਰਾ->closed = TRUE;
    ਨਤੀਜਾ = cf_flush_egress(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  ਧਾਰਾ->id, (ਪੂਰਨ)ਨਤੀਜਾ);
  }
}

ਸਥਿਰ ਖਾਲੀ h3_ਅੰਕੜੇ_ਪੂਰਾ(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);

  (ਖਾਲੀ)cf;
  ਜੇ(ਧਾਰਾ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] easy handle is done", ਧਾਰਾ->id);
    cf_quiche_ਧਾਰਾ_close(cf, ਅੰਕੜੇ, ਧਾਰਾ);
    Curl_u32_ptrset_unset(&ctx->streams, ਅੰਕੜੇ->mid);
  }
}

ਸਥਿਰ ਖਾਲੀ cf_quiche_expire_conn_closed(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  DEBUGASSERT(ਅੰਕੜੇ->multi);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, ਅੰਕੜੇ->multi, cf_quiche_do_expire, ਨਿਰਾ);
}

ਸਥਿਰ ਖਾਲੀ cf_quiche_write_hd(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ,
                               ਅਚਲ ਅੱਖਰ *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ਜੇ(!ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
    ਧਾਰਾ->xfer_ਨਤੀਜਾ = Curl_xfer_write_resp_hd(ਅੰਕੜੇ, buf, blen, eos);
    ਜੇ(ਧਾਰਾ->xfer_ਨਤੀਜਾ)
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", ਧਾਰਾ->id, (ਪੂਰਨ)ਧਾਰਾ->xfer_ਨਤੀਜਾ,
                  blen);
  }
}

ਬਣਤਰ cb_ctx {
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ;
};

ਸਥਿਰ bool is_valid_h3_ਸਿਰਲੇਖ(ਅਚਲ uint8_t *hdr, size_t hlen)
{
  ਜਦੋਂ(hlen--) {
    ਚੋਣ(*hdr++) {
    ਹਾਲਤ '\n':
    ਹਾਲਤ '\r':
    ਹਾਲਤ '\0':
      ਵਾਪਸ FALSE;
    }
  }
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ ਪੂਰਨ cb_each_ਸਿਰਲੇਖ(uint8_t *name, size_t name_len,
                          uint8_t *ਮੁੱਲ, size_t ਮੁੱਲ_len,
                          ਖਾਲੀ *argp)
{
  ਬਣਤਰ cb_ctx *x = argp;
  ਬਣਤਰ Curl_cfilter *cf = x->cf;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = x->ਅੰਕੜੇ;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = x->ਧਾਰਾ;
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!ਧਾਰਾ || ਧਾਰਾ->xfer_ਨਤੀਜਾ)
    ਵਾਪਸ 1; /* abort iteration */

  ਜੇ((name_len == 7) && !ਲੜੀ_ਤੁਲਨਾ(HTTP_PSEUDO_ਸਥਿਤੀ, (ਅੱਖਰ *)name, 7) &&
     is_valid_h3_ਸਿਰਲੇਖ(ਮੁੱਲ, ਮੁੱਲ_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    ਨਤੀਜਾ = Curl_http_decode_ਸਥਿਤੀ(&ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ,
                                     (ਅਚਲ ਅੱਖਰ *)ਮੁੱਲ, ਮੁੱਲ_len);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(&ctx->h1hdr, (ਅਚਲ ਅੱਖਰ *)ਮੁੱਲ, ਮੁੱਲ_len);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    ਜੇ(!ਨਤੀਜਾ)
      cf_quiche_write_hd(cf, ਅੰਕੜੇ, ਧਾਰਾ, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] status: %s",
                ਧਾਰਾ->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(is_valid_h3_ਸਿਰਲੇਖ(ਮੁੱਲ, ਮੁੱਲ_len) &&
       is_valid_h3_ਸਿਰਲੇਖ(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  ਧਾਰਾ->id, (ਪੂਰਨ)name_len, name,
                  (ਪੂਰਨ)ਮੁੱਲ_len, ਮੁੱਲ);
      curlx_dyn_reset(&ctx->h1hdr);
      ਨਤੀਜਾ = curlx_dyn_addn(&ctx->h1hdr, (ਅਚਲ ਅੱਖਰ *)name, name_len);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = curlx_dyn_addn(&ctx->h1hdr, (ਅਚਲ ਅੱਖਰ *)ਮੁੱਲ, ਮੁੱਲ_len);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      ਜੇ(!ਨਤੀਜਾ)
        cf_quiche_write_hd(cf, ਅੰਕੜੇ, ਧਾਰਾ, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    ਨਹੀਂਤਾਂ
      CURL_TRC_CF(x->ਅੰਕੜੇ, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  ਧਾਰਾ->id, ਮੁੱਲ_len + name_len);
  }

  ਜੇ(ਨਤੀਜਾ) {
    CURL_TRC_CF(x->ਅੰਕੜੇ, x->cf, "[%" PRIu64 "] on header error %d",
                ਧਾਰਾ->id, (ਪੂਰਨ)ਨਤੀਜਾ);
    ਜੇ(!ਧਾਰਾ->xfer_ਨਤੀਜਾ)
      ਧਾਰਾ->xfer_ਨਤੀਜਾ = ਨਤੀਜਾ;
  }
  ਵਾਪਸ ਧਾਰਾ->xfer_ਨਤੀਜਾ ? 1 : 0;
}

ਸਥਿਰ CURLcode ਧਾਰਾ_resp_read(ਖਾਲੀ *reader_ctx,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t len,
                                 size_t *pnread)
{
  ਬਣਤਰ cb_ctx *x = reader_ctx;
  ਬਣਤਰ cf_quiche_ctx *ctx = x->cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, x->ਅੰਕੜੇ);
  ssize_t nread;

  *pnread = 0;
  ਜੇ(!ਧਾਰਾ)
    ਵਾਪਸ CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, ਧਾਰਾ->id, buf, len);
  ਜੇ(!curlx_sztouz(nread, pnread))
    ਵਾਪਸ CURLE_AGAIN;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cf_quiche_flush_body(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ਅਚਲ uint8_t *buf;
  size_t blen;

  ਜਦੋਂ(ਧਾਰਾ && !ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
    ਜੇ(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      ਧਾਰਾ->xfer_ਨਤੀਜਾ = Curl_xfer_write_resp(
        ਅੰਕੜੇ, (ਅਚਲ ਅੱਖਰ *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      ਜੇ(ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", ਧਾਰਾ->id, (ਪੂਰਨ)ਧਾਰਾ->xfer_ਨਤੀਜਾ, blen);
      }
    }
    ਨਹੀਂਤਾਂ
      ਰੋਕੋ;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

ਸਥਿਰ ਖਾਲੀ cf_quiche_recv_body(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  ਬਣਤਰ cb_ctx cb_ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!ਧਾਰਾ)
    ਵਾਪਸ;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.ਅੰਕੜੇ = ਅੰਕੜੇ;
  cb_ctx.ਧਾਰਾ = ਧਾਰਾ;
  Curl_bufq_reset(&ctx->writebuf);
  ਜਦੋਂ(!ਨਤੀਜਾ) {
    ਨਤੀਜਾ = Curl_bufq_slurp(&ctx->writebuf,
                             ਧਾਰਾ_resp_read, &cb_ctx, &nread);
    ਜੇ(!ਨਤੀਜਾ)
      cf_quiche_flush_body(cf, ਅੰਕੜੇ, ਧਾਰਾ);
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      ਰੋਕੋ;
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] recv_body error %d",
                  ਧਾਰਾ->id, (ਪੂਰਨ)ਨਤੀਜਾ);
      failf(ਅੰਕੜੇ, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            ਧਾਰਾ->id, (ਪੂਰਨ)ਨਤੀਜਾ);
      ਧਾਰਾ->closed = TRUE;
      ਧਾਰਾ->reset = TRUE;
      ਧਾਰਾ->send_closed = TRUE;
      ਜੇ(!ਧਾਰਾ->xfer_ਨਤੀਜਾ)
        ਧਾਰਾ->xfer_ਨਤੀਜਾ = ਨਤੀਜਾ;
    }
  }
  cf_quiche_flush_body(cf, ਅੰਕੜੇ, ਧਾਰਾ);
}

ਸਥਿਰ ਖਾਲੀ cf_quiche_process_ev(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ,
                                 quiche_h3_event *ev)
{
  ਗਿਣਤੀ quiche_h3_event_type type;

  ਜੇ(!ਧਾਰਾ)
    ਵਾਪਸ;

  type = quiche_h3_event_type(ev);

  ਚੋਣ(type) {
  ਹਾਲਤ QUICHE_H3_EVENT_ਸਿਰਲੇਖਾਂ: {
    ਬਣਤਰ cb_ctx cb_ctx;
    ਧਾਰਾ->resp_got_ਸਿਰਲੇਖ = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.ਅੰਕੜੇ = ਅੰਕੜੇ;
    cb_ctx.ਧਾਰਾ = ਧਾਰਾ;
    quiche_h3_event_for_each_ਸਿਰਲੇਖ(ev, cb_each_ਸਿਰਲੇਖ, &cb_ctx);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] <- [HEADERS]", ਧਾਰਾ->id);
    Curl_multi_mark_dirty(ਅੰਕੜੇ);
    ਰੋਕੋ;
  }
  ਹਾਲਤ QUICHE_H3_EVENT_ਅੰਕੜੇ:
    ਜੇ(!ਧਾਰਾ->resp_hds_complete) {
      ਧਾਰਾ->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, ਅੰਕੜੇ, ਧਾਰਾ, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, ਅੰਕੜੇ, ਧਾਰਾ);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] <- [DATA]", ਧਾਰਾ->id);
    Curl_multi_mark_dirty(ਅੰਕੜੇ);
    ਰੋਕੋ;

  ਹਾਲਤ QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] RESET", ਧਾਰਾ->id);
    ਧਾਰਾ->closed = TRUE;
    ਧਾਰਾ->reset = TRUE;
    ਧਾਰਾ->send_closed = TRUE;
    Curl_multi_mark_dirty(ਅੰਕੜੇ);
    ਰੋਕੋ;

  ਹਾਲਤ QUICHE_H3_EVENT_ਮੁਕੰਮਲ:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] CLOSED", ਧਾਰਾ->id);
    ਜੇ(!ਧਾਰਾ->resp_hds_complete) {
      ਧਾਰਾ->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, ਅੰਕੜੇ, ਧਾਰਾ, "\r\n", 2, TRUE);
    }
    ਧਾਰਾ->closed = TRUE;
    Curl_multi_mark_dirty(ਅੰਕੜੇ);
    ਰੋਕੋ;

  ਹਾਲਤ QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] <- [GOAWAY]", ਧਾਰਾ->id);
    ਰੋਕੋ;

  ਮੂਲ:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] recv, unhandled event %d",
                ਧਾਰਾ->id, (ਪੂਰਨ)type);
    ਰੋਕੋ;
  }
}

ਬਣਤਰ cf_quiche_disp_ctx {
  uint64_t ਧਾਰਾ_id;
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ Curl_multi *multi;
  quiche_h3_event *ev;
};

ਸਥਿਰ bool cf_quiche_disp_event(uint32_t mid, ਖਾਲੀ *val, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_quiche_disp_ctx *dctx = user_ਅੰਕੜੇ;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = val;

  ਜੇ(ਧਾਰਾ->id == dctx->ਧਾਰਾ_id) {
    ਬਣਤਰ Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    ਜੇ(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, ਧਾਰਾ, dctx->ev);
    ਵਾਪਸ FALSE; /* stop iterating */
  }
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ CURLcode cf_poll_events(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = ਨਿਰਾ;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  ਜਦੋਂ(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    ਜੇ(rv == QUICHE_H3_ERR_ਪੂਰਾ) {
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(rv < 0) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "error poll: %" PRId64, rv);
      ਵਾਪਸ CURLE_HTTP3;
    }
    ਨਹੀਂਤਾਂ {
      ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
      ਜੇ(ਧਾਰਾ && ਧਾਰਾ->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, ਅੰਕੜੇ, ਧਾਰਾ, ev);
        quiche_h3_event_free(ev);
        ਜੇ(ਧਾਰਾ->xfer_ਨਤੀਜਾ)
          ਵਾਪਸ ਧਾਰਾ->xfer_ਨਤੀਜਾ;
      }
      ਨਹੀਂਤਾਂ {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        ਬਣਤਰ cf_quiche_disp_ctx dctx;
        dctx.ਧਾਰਾ_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = ਅੰਕੜੇ->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_ਦੇਖੋ(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  ਵਾਪਸ CURLE_OK;
}

ਬਣਤਰ recv_ctx {
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  ਪੂਰਨ pkts;
};

ਸਥਿਰ CURLcode cf_quiche_recv_pkts(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t buflen,
                                    size_t gso_ਮਾਪ,
                                    ਬਣਤਰ sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    ਖਾਲੀ *userp)
{
  ਬਣਤਰ recv_ctx *r = userp;
  ਬਣਤਰ cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, ਵਿੱਥ, nread;
  ssize_t rv;

  (ਖਾਲੀ)ecn;

  recv_info.ਮੰਜ਼ਿਲ = (ਬਣਤਰ sockaddr *)&ctx->q.local_addr;
  recv_info.ਮੰਜ਼ਿਲ_len = ctx->q.local_addrlen;
  recv_info.from = (ਬਣਤਰ sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  ਲਈ(ਵਿੱਥ = 0; ਵਿੱਥ < buflen; ਵਿੱਥ += gso_ਮਾਪ) {
    pktlen = ((ਵਿੱਥ + gso_ਮਾਪ) <= buflen) ? gso_ਮਾਪ : (buflen - ਵਿੱਥ);
    rv = quiche_conn_recv(ctx->qconn,
                          (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)CURL_UNCONST(buf + ਵਿੱਥ),
                          pktlen, &recv_info);
    ਜੇ(!curlx_sztouz(rv, &nread)) {
      ਜੇ(QUICHE_ERR_ਪੂਰਾ == rv) {
        ਜੇ(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->ਅੰਕੜੇ, r->cf, "ingress, connection is draining");
          ਵਾਪਸ CURLE_RECV_ERROR;
        }
        ਜੇ(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->ਅੰਕੜੇ, r->cf, "ingress, connection is closed");
          ਵਾਪਸ CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->ਅੰਕੜੇ, r->cf, "ingress, quiche is DONE");
        ਵਾਪਸ CURLE_OK;
      }
      ਨਹੀਂਤਾਂ ਜੇ(QUICHE_ERR_TLS_FAIL == rv) {
        ਲੰਮਾ verify_ok = SSL_get_verify_ਨਤੀਜਾ(ctx->tls.ossl.ssl);
        ਜੇ(verify_ok != X509_V_OK) {
          failf(r->ਅੰਕੜੇ, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->ਅੰਕੜੇ, "ingress, quiche reports TLS fail");
        ਵਾਪਸ CURLE_RECV_ERROR;
      }
      ਨਹੀਂਤਾਂ {
        failf(r->ਅੰਕੜੇ, "quiche reports error %zd on receive", rv);
        ਵਾਪਸ CURLE_RECV_ERROR;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(nread < pktlen) {
      CURL_TRC_CF(r->ਅੰਕੜੇ, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_process_ingress(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ਬਣਤਰ recv_ctx rctx;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(ctx->qconn);
  ਨਤੀਜਾ = Curl_vquic_tls_before_recv(&ctx->tls, cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  rctx.cf = cf;
  rctx.ਅੰਕੜੇ = ਅੰਕੜੇ;
  rctx.pkts = 0;

  ਨਤੀਜਾ = Curl_vquic_recv_packets(cf, ਅੰਕੜੇ, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(ਅੰਕੜੇ->multi);
    cf_quiche_for_all_streams(cf, ਅੰਕੜੇ->multi, cf_quiche_do_resume, ਨਿਰਾ);
  }
  ਵਾਪਸ cf_poll_events(cf, ਅੰਕੜੇ);
}

ਬਣਤਰ read_ctx {
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  quiche_send_info send_info;
};

ਸਥਿਰ CURLcode read_pkt_ਮੰਜ਼ਿਲ_send(ਖਾਲੀ *userp,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t buflen,
                                 size_t *pnread)
{
  ਬਣਤਰ read_ctx *x = userp;
  ਬਣਤਰ cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  ਜੇ(rv == QUICHE_ERR_ਪੂਰਾ)
    ਵਾਪਸ CURLE_AGAIN;

  ਜੇ(!curlx_sztouz(rv, pnread)) {
    failf(x->ਅੰਕੜੇ, "quiche_conn_send returned %zd", rv);
    ਵਾਪਸ CURLE_SEND_ERROR;
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
ਸਥਿਰ CURLcode cf_flush_egress(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode ਨਤੀਜਾ;
  int64_t expiry_ns;
  int64_t timeout_ns;
  ਬਣਤਰ read_ctx readx;
  size_t pkt_ਗਿਣਤੀ, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  ਜੇ(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    ਜੇ(quiche_conn_is_closed(ctx->qconn)) {
      ਜੇ(quiche_conn_is_timed_ਬਾਹਰਲਾ(ctx->qconn))
        failf(ਅੰਕੜੇ, "connection closed by idle timeout");
      ਨਹੀਂਤਾਂ
        failf(ਅੰਕੜੇ, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, ਅੰਕੜੇ);
      ਵਾਪਸ CURLE_SEND_ERROR;
    }
  }

  ਨਤੀਜਾ = Curl_vquic_flush(cf, ਅੰਕੜੇ, &ctx->q);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      Curl_expire(ਅੰਕੜੇ, 1, EXPIRE_QUIC);
      ਵਾਪਸ CURLE_OK;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  readx.cf = cf;
  readx.ਅੰਕੜੇ = ਅੰਕੜੇ;
  memset(&readx.send_info, 0, ਆਕਾਰ(readx.send_info));
  pkt_ਗਿਣਤੀ = 0;
  gsolen = quiche_conn_max_send_udp_payload_ਮਾਪ(ctx->qconn);
  ਲਈ(;;) {
    /* add the next packet to send, if any, to our buffer */
    ਨਤੀਜਾ = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_ਮੰਜ਼ਿਲ_send, &readx, &nread);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN)
        ਵਾਪਸ ਨਤੀਜਾ;
      /* Nothing more to add, flush and leave */
      ਨਤੀਜਾ = Curl_vquic_send(cf, ਅੰਕੜੇ, &ctx->q, gsolen);
      ਜੇ(ਨਤੀਜਾ) {
        ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
          Curl_expire(ਅੰਕੜੇ, 1, EXPIRE_QUIC);
          ਵਾਪਸ CURLE_OK;
        }
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ++pkt_ਗਿਣਤੀ;
    ਜੇ(nread < gsolen || pkt_ਗਿਣਤੀ >= MAX_PKT_ਮਿਆਦ) {
      ਨਤੀਜਾ = Curl_vquic_send(cf, ਅੰਕੜੇ, &ctx->q, gsolen);
      ਜੇ(ਨਤੀਜਾ) {
        ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
          Curl_expire(ਅੰਕੜੇ, 1, EXPIRE_QUIC);
          ਵਾਪਸ CURLE_OK;
        }
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      pkt_ਗਿਣਤੀ = 0;
    }
  }

ਬਾਹਰਲਾ:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  ਜੇ(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(ਅੰਕੜੇ, (timeout_ns / 1000000), EXPIRE_QUIC);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode recv_closed_ਧਾਰਾ(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   size_t *pnread)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(ਧਾਰਾ);
  *pnread = 0;
  ਜੇ(ਧਾਰਾ->reset) {
    ਜੇ(ਧਾਰਾ->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(ਅੰਕੜੇ, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", ਧਾਰਾ->id);
      connclose(cf->conn); /* do not use this anymore */
      ਅੰਕੜੇ->state.refused_ਧਾਰਾ = TRUE;
      ਵਾਪਸ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->resp_hds_complete && ਅੰਕੜੇ->req.no_body) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", ਧਾਰਾ->id, ਧਾਰਾ->error3,
                    Curl_vquic_h3_err_str(ਧਾਰਾ->error3));
        ਵਾਪਸ CURLE_OK;
    }
    failf(ਅੰਕੜੇ, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", ਧਾਰਾ->id, ਧਾਰਾ->error3,
          Curl_vquic_h3_err_str(ਧਾਰਾ->error3));
    ਨਤੀਜਾ = ਅੰਕੜੇ->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                ਧਾਰਾ->id, (ਪੂਰਨ)ਨਤੀਜਾ);
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਧਾਰਾ->resp_got_ਸਿਰਲੇਖ) {
    failf(ਅੰਕੜੇ, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          ਧਾਰਾ->id);
    ਨਤੀਜਾ = CURLE_HTTP3;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_quiche_recv(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅੱਖਰ *buf, size_t blen, size_t *pnread)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *pnread = 0;
  (ਖਾਲੀ)buf;
  (ਖਾਲੀ)blen;
  Curl_vquic_ctx_update_ਸਮਾਂ(&ctx->q, Curl_pgrs_now(ਅੰਕੜੇ));

  ਜੇ(!ਧਾਰਾ)
    ਵਾਪਸ CURLE_RECV_ERROR;

  ਨਤੀਜਾ = cf_process_ingress(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_recv, error on ingress");
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
    cf_quiche_ਧਾਰਾ_close(cf, ਅੰਕੜੇ, ਧਾਰਾ);
    ਨਤੀਜਾ = ਧਾਰਾ->xfer_ਨਤੀਜਾ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->closed)
    ਨਤੀਜਾ = recv_closed_ਧਾਰਾ(cf, ਅੰਕੜੇ, pnread);
  ਨਹੀਂਤਾਂ ਜੇ(quiche_conn_is_draining(ctx->qconn)) {
    failf(ਅੰਕੜੇ, "QUIC connection is draining");
    ਨਤੀਜਾ = CURLE_HTTP3;
  }
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = CURLE_AGAIN;

ਬਾਹਰਲਾ:
  ਨਤੀਜਾ = Curl_1st_fatal(ਨਤੀਜਾ, cf_flush_egress(cf, ਅੰਕੜੇ));
  ਜੇ(*pnread > 0)
    ctx->ਅੰਕੜੇ_recvd += *pnread;
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, ਧਾਰਾ->id, blen, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread,
              ctx->ਅੰਕੜੇ_recvd);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_quiche_send_body(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ,
                                    ਅਚਲ uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, ਧਾਰਾ->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  ਜੇ(rv == QUICHE_H3_ERR_ਪੂਰਾ || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    ਜੇ(!quiche_conn_ਧਾਰਾ_writable(ctx->qconn, ਧਾਰਾ->id, len)) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", ਧਾਰਾ->id, len);
      ਧਾਰਾ->quic_flow_blocked = TRUE;
    }
    ਵਾਪਸ CURLE_AGAIN;
  }
  ਨਹੀਂਤਾਂ ਜੇ(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_ਧਾਰਾ_STATE) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", ਧਾਰਾ->id, len);
    ਵਾਪਸ CURLE_HTTP3;
  }
  ਨਹੀਂਤਾਂ ਜੇ(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_ਮਾਪ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                ਧਾਰਾ->id, len);
    ਵਾਪਸ CURLE_SEND_ERROR;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                ਧਾਰਾ->id, len, rv);
    ਵਾਪਸ CURLE_SEND_ERROR;
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(eos && (len == *pnwritten))
      ਧਾਰਾ->send_closed = TRUE;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                ਧਾਰਾ->id, len, ਧਾਰਾ->send_closed, *pnwritten);
    ਵਾਪਸ CURLE_OK;
  }
}

ਸਥਿਰ CURLcode h3_open_ਧਾਰਾ(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅਚਲ uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
  size_t nheader, i;
  int64_t rv;
  ਬਣਤਰ dynhds h2_ਸਿਰਲੇਖਾਂ;
  quiche_h3_ਸਿਰਲੇਖ *nva = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *pnwritten = 0;
  ਜੇ(!ਧਾਰਾ) {
    ਨਤੀਜਾ = h3_ਅੰਕੜੇ_ਤਿਆਰੀ(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
    DEBUGASSERT(ਧਾਰਾ);
  }

  Curl_dynhds_init(&h2_ਸਿਰਲੇਖਾਂ, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(ਧਾਰਾ);

  ਨਤੀਜਾ = Curl_h1_req_parse_read(&ਧਾਰਾ->h1, buf, blen, ਨਿਰਾ,
    !ਅੰਕੜੇ->state.http_ignorecustom ?
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST) : ਨਿਰਾ,
    0, pnwritten);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਜੇ(!ਧਾਰਾ->h1.ਪੂਰਾ) {
    /* need more data */
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  DEBUGASSERT(ਧਾਰਾ->h1.req);

  ਨਤੀਜਾ = Curl_http_req_ਮੰਜ਼ਿਲ_h2(&h2_ਸਿਰਲੇਖਾਂ, ਧਾਰਾ->h1.req, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* no longer needed */
  Curl_h1_req_parse_free(&ਧਾਰਾ->h1);

  nheader = Curl_dynhds_ਗਿਣਤੀ(&h2_ਸਿਰਲੇਖਾਂ);
  nva = curlx_malloc(ਆਕਾਰ(quiche_h3_ਸਿਰਲੇਖ) * nheader);
  ਜੇ(!nva) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਲਈ(i = 0; i < nheader; ++i) {
    ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *e = Curl_dynhds_getn(&h2_ਸਿਰਲੇਖਾਂ, i);
    nva[i].name = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].ਮੁੱਲ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)e->ਮੁੱਲ;
    nva[i].ਮੁੱਲ_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  ਜੇ(eos && !blen)
    ਧਾਰਾ->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              ਧਾਰਾ->send_closed);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "quiche_send_request() -> %" PRId64, rv);
  ਜੇ(rv < 0) {
    ਜੇ(QUICHE_H3_ERR_ਧਾਰਾ_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] blocked", ਧਾਰਾ->id);
      ਧਾਰਾ->quic_flow_blocked = TRUE;
      ਨਤੀਜਾ = CURLE_AGAIN;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&ਅੰਕੜੇ->state.url), rv);
    }
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  DEBUGASSERT(!ਧਾਰਾ->opened);
  ਧਾਰਾ->id = (uint64_t)rv;
  ਧਾਰਾ->opened = TRUE;
  ਧਾਰਾ->closed = FALSE;
  ਧਾਰਾ->reset = FALSE;

  ਜੇ(Curl_trc_is_verbose(ਅੰਕੜੇ)) {
    infof(ਅੰਕੜੇ, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          ਧਾਰਾ->id, Curl_bufref_ptr(&ਅੰਕੜੇ->state.url));
    ਲਈ(i = 0; i < nheader; ++i) {
      infof(ਅੰਕੜੇ, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", ਧਾਰਾ->id,
            (ਪੂਰਨ)nva[i].name_len, nva[i].name,
            (ਪੂਰਨ)nva[i].ਮੁੱਲ_len, nva[i].ਮੁੱਲ);
    }
  }

  ਜੇ(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, ਅੰਕੜੇ, ਧਾਰਾ, buf, blen, eos, &nwritten);
    ਜੇ(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      ਨਤੀਜਾ = r2;
    }
    ਨਹੀਂਤਾਂ ਜੇ(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

ਬਾਹਰਲਾ:
  curlx_free(nva);
  Curl_dynhds_free(&h2_ਸਿਰਲੇਖਾਂ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_quiche_send(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਅਚਲ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
  CURLcode ਨਤੀਜਾ;

  *pnwritten = 0;
  Curl_vquic_ctx_update_ਸਮਾਂ(&ctx->q, Curl_pgrs_now(ਅੰਕੜੇ));

  ਨਤੀਜਾ = cf_process_ingress(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(!ਧਾਰਾ || !ਧਾਰਾ->opened) {
    ਨਤੀਜਾ = h3_open_ਧਾਰਾ(cf, ਅੰਕੜੇ, buf, len, eos, pnwritten);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
    cf_quiche_ਧਾਰਾ_close(cf, ਅੰਕੜੇ, ਧਾਰਾ);
    ਨਤੀਜਾ = ਧਾਰਾ->xfer_ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->closed) {
    ਜੇ(ਧਾਰਾ->resp_hds_complete) {
      /* sending request body on a stream that has been closed by the
       * server. If the server has send us a final response, we should
       * silently discard the send data.
       * This happens for example on redirects where the server, instead
       * of reading the full request body closed the stream after
       * sending the 30x response.
       * This is sort of a race: had the transfer loop called recv first,
       * it would see the response and stop/discard sending on its own- */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] discarding data "
                  "on closed stream with response", ਧਾਰਾ->id);
      ਨਤੀਜਾ = CURLE_OK;
      *pnwritten = len;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", ਧਾਰਾ->id, len);
    ਨਤੀਜਾ = CURLE_HTTP3;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = cf_quiche_send_body(cf, ਅੰਕੜੇ, ਧਾਰਾ, buf, len, eos, pnwritten);
  }

ਬਾਹਰਲਾ:
  ਨਤੀਜਾ = Curl_1st_fatal(ਨਤੀਜਾ, cf_flush_egress(cf, ਅੰਕੜੇ));

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              ਧਾਰਾ ? ਧਾਰਾ->id : (uint64_t)~0, len,
              (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool ਧਾਰਾ_is_writable(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);

  ਵਾਪਸ ਧਾਰਾ && (quiche_conn_ਧਾਰਾ_writable(
    ctx->qconn, ਧਾਰਾ->id, 1) > 0);
}

ਸਥਿਰ CURLcode cf_quiche_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!ctx->qconn)
    ਵਾਪਸ CURLE_OK;

  Curl_pollset_check(ਅੰਕੜੇ, ps, ctx->q.sockfd, &want_recv, &want_send);
  ਜੇ(want_recv || want_send) {
    ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
    bool c_exhaust, s_exhaust;

    c_exhaust = FALSE; /* Have not found any call in quiche that tells
                          us if the connection itself is blocked */
    s_exhaust = want_send && ਧਾਰਾ && ਧਾਰਾ->opened &&
                (ਧਾਰਾ->quic_flow_blocked || !ਧਾਰਾ_is_writable(cf, ਅੰਕੜੇ));
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->q.sendbuf);

    ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ(ਅੰਕੜੇ, ps, ctx->q.sockfd, want_recv, want_send);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode h3_ਅੰਕੜੇ_pause(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (ਖਾਲੀ)cf;
  ਜੇ(!pause) {
    Curl_multi_mark_dirty(ਅੰਕੜੇ);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_quiche_cntrl(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)arg1;
  (ਖਾਲੀ)arg2;
  ਚੋਣ(event) {
  ਹਾਲਤ CF_CTRL_ਅੰਕੜੇ_ਤਿਆਰੀ:
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_ਅੰਕੜੇ_PAUSE:
    ਨਤੀਜਾ = h3_ਅੰਕੜੇ_pause(cf, ਅੰਕੜੇ, (arg1 != 0));
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_ਅੰਕੜੇ_ਪੂਰਾ:
    h3_ਅੰਕੜੇ_ਪੂਰਾ(cf, ਅੰਕੜੇ);
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_ਅੰਕੜੇ_ਪੂਰਾ_SEND: {
    ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ = H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
    ਜੇ(ਧਾਰਾ && !ਧਾਰਾ->send_closed) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ body[1];
      size_t ਭੇਜਿਆ;

      ਧਾਰਾ->send_closed = TRUE;
      body[0] = 'X';
      ਨਤੀਜਾ = cf_quiche_send(cf, ਅੰਕੜੇ, body, 0, TRUE, &ਭੇਜਿਆ);
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  ਧਾਰਾ->id, (ਪੂਰਨ)ਨਤੀਜਾ, ਭੇਜਿਆ);
    }
    ਰੋਕੋ;
  }
  ਹਾਲਤ CF_CTRL_CONN_INFO_UPDATE:
    ਜੇ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_ਵੇਖਿਆ = 30;
      Curl_conn_ਸੈੱਟ_multiplex(cf->conn);
    }
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_REPORT_STATS:
    ਜੇ(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrs_ਸਮਾਂ_Was(ਅੰਕੜੇ, TIMER_CONNECT, ctx->q.ਪਹਿਲਾ_byte_at);
      Curl_pgrs_ਸਮਾਂ_Was(ਅੰਕੜੇ, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_quiche_ctx_open(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ਪੂਰਨ rv;
  CURLcode ਨਤੀਜਾ;
  ਅਚਲ ਬਣਤਰ Curl_sockaddr_ex *sockaddr;
  ਸਥਿਰ ਅਚਲ ਬਣਤਰ alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  ਨਤੀਜਾ = Curl_vquic_ctx_init(ਅੰਕੜੇ, &ctx->q);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_ਸੰਸਕਰਣ);
  ਜੇ(!ctx->cfg) {
    failf(ਅੰਕੜੇ, "cannot create quiche config");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  quiche_config_enable_pacing(ctx->cfg, FALSE);
  quiche_config_ਸੈੱਟ_initial_max_ਅੰਕੜੇ(ctx->cfg, (1 * 1024 * 1024)
    /* (QUIC_MAX_STREAMS/2) * H3_STREAM_WINDOW_SIZE */);
  quiche_config_ਸੈੱਟ_initial_max_streams_bidi(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_ਸੈੱਟ_initial_max_streams_uni(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_ਸੈੱਟ_initial_max_ਧਾਰਾ_ਅੰਕੜੇ_bidi_local(ctx->cfg,
    H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ);
  quiche_config_ਸੈੱਟ_initial_max_ਧਾਰਾ_ਅੰਕੜੇ_bidi_remote(ctx->cfg,
    H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ);
  quiche_config_ਸੈੱਟ_initial_max_ਧਾਰਾ_ਅੰਕੜੇ_uni(ctx->cfg,
    H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ);
  quiche_config_ਸੈੱਟ_disable_ਸਰਗਰਮ_migration(ctx->cfg, TRUE);

  quiche_config_ਸੈੱਟ_max_connection_ਖਿੜਕੀ(ctx->cfg,
    10 * QUIC_MAX_STREAMS * H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ);
  quiche_config_ਸੈੱਟ_max_ਧਾਰਾ_ਖਿੜਕੀ(ctx->cfg, 10 * H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ);
  quiche_config_ਸੈੱਟ_application_protos(ctx->cfg,
                      (uint8_t *)CURL_UNCONST(QUICHE_H3_APPLICATION_PROTOCOL),
                                 CURL_CSTRLEN(QUICHE_H3_APPLICATION_PROTOCOL));

  ਨਤੀਜਾ = Curl_vquic_tls_init(&ctx->tls, cf, ਅੰਕੜੇ, &ctx->ssl_peer,
                               &ALPN_SPEC_H3, ਨਿਰਾ, ਨਿਰਾ, cf, ਨਿਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_rand(ਅੰਕੜੇ, ctx->scid, ਆਕਾਰ(ctx->scid));
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(Curl_cf_socket_peek(cf->ਅਗਲਾ, ਅੰਕੜੇ, &ctx->q.sockfd, &sockaddr, ਨਿਰਾ))
    ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = ਆਕਾਰ(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (ਬਣਤਰ sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  ਜੇ(rv == -1)
    ਵਾਪਸ CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((ਅਚਲ uint8_t *)ctx->scid,
                                        ਆਕਾਰ(ctx->scid), ਨਿਰਾ, 0,
                                        (ਬਣਤਰ sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  ਜੇ(!ctx->qconn) {
    failf(ਅੰਕੜੇ, "cannot create quiche connection");
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  /* Known to not work on Windows */
#ਜੇ !defined(_WIN32) && defined(HAVE_QUICHE_CONN_ਸੈੱਟ_QLOG_FD)
  {
    ਪੂਰਨ qfd;
    (ਖਾਲੀ)Curl_qlogdir(ਅੰਕੜੇ, ctx->scid, ਆਕਾਰ(ctx->scid), &qfd);
    ਜੇ(qfd != -1)
      quiche_conn_ਸੈੱਟ_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#ਜੇ_ਅੰਤ

  ਨਤੀਜਾ = cf_flush_egress(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    ਬਿਨਾਨਿਸ਼ਾਨੀ alpn_len, ਵਿੱਥ = 0;

    /* Replace each ALPN length prefix by a comma. */
    ਜਦੋਂ(ਵਿੱਥ < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[ਵਿੱਥ];
      alpn_protocols[ਵਿੱਥ] = ',';
      ਵਿੱਥ += 1 + alpn_len;
    }

    CURL_TRC_CF(ਅੰਕੜੇ, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_quiche_verify_peer(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  ਵਾਪਸ Curl_vquic_tls_verify_peer(&ctx->tls, cf, ਅੰਕੜੇ, &ctx->ssl_peer);
}

ਸਥਿਰ CURLcode cf_quiche_connect(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  /* Connect the UDP filter first */
  ਜੇ(!cf->ਅਗਲਾ->connected) {
    ਨਤੀਜਾ = Curl_conn_cf_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ || !*ਪੂਰਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  *ਪੂਰਾ = FALSE;
  ਜੇ(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(ਅੰਕੜੇ, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "need HTTPS-RR, delaying connect");
    ਵਾਪਸ CURLE_OK;
  }

  Curl_vquic_ctx_update_ਸਮਾਂ(&ctx->q, Curl_pgrs_now(ਅੰਕੜੇ));

  ਜੇ(!ctx->qconn) {
    ਨਤੀਜਾ = cf_quiche_ctx_open(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ctx->started_at = ctx->q.ਆਖਰੀ_op;
    ਨਤੀਜਾ = cf_flush_egress(cf, ਅੰਕੜੇ);
    /* we do not expect to be able to recv anything yet */
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = cf_process_ingress(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = cf_flush_egress(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.ਆਖਰੀ_op;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    ਨਤੀਜਾ = cf_quiche_verify_peer(cf, ਅੰਕੜੇ);
    ਜੇ(!ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      ਜੇ(!ctx->h3config) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_ਸੈੱਟ_max_ਖੇਤਰ_section_ਮਾਪ(ctx->h3config,
                                                  MAX_HTTP_RESP_ਸਿਰਲੇਖ_ਮਾਪ);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      ਜੇ(!ctx->h3c) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      cf->connected = TRUE;
      *ਪੂਰਾ = TRUE;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }

ਬਾਹਰਲਾ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਜੇ(ਨਤੀਜਾ && ਨਤੀਜਾ != CURLE_AGAIN) {
    ਬਣਤਰ ip_quadruple ip;

    ਜੇ(!Curl_cf_socket_peek(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਨਿਰਾ, ਨਿਰਾ, &ip))
      infof(ਅੰਕੜੇ, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_ਪੋਰਟ, curl_easy_strerror(ਨਤੀਜਾ));
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "connect failed: %s", curl_easy_strerror(ਨਤੀਜਾ));
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_quiche_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(cf->shutdown || !ctx || !ctx->qconn) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  *ਪੂਰਾ = FALSE;
  ਜੇ(!ctx->shutdown_started) {
    ਪੂਰਨ err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_ਸਮਾਂ(&ctx->q, Curl_pgrs_now(ਅੰਕੜੇ));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, ਨਿਰਾ, 0);
    ਜੇ(err) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->q.sendbuf)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown, flushing sendbuf");
    ਨਤੀਜਾ = cf_flush_egress(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown completely sent off, done");
    *ਪੂਰਾ = TRUE;
  }
  ਨਹੀਂਤਾਂ {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown sending blocked");
  }

ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_quiche_destroy(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = ਨਿਰਾ;
  }
}

ਸਥਿਰ CURLcode cf_quiche_query(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;

  ਚੋਣ(query) {
  ਹਾਲਤ CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    ਜੇ(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_ਖੱਬਾ_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ : (ਪੂਰਨ)max_streams;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    ਵਾਪਸ CURLE_OK;
  }
  ਹਾਲਤ CF_QUERY_CONNECT_ਜਵਾਬ_MS:
    ਜੇ(ctx->q.got_ਪਹਿਲਾ_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.ਪਹਿਲਾ_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? (ਪੂਰਨ)ms : ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
    }
    ਨਹੀਂਤਾਂ
      *pres1 = -1;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CF_QUERY_HTTP_ਸੰਸਕਰਣ:
    *pres1 = 30;
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CF_QUERY_SSL_INFO:
  ਹਾਲਤ CF_QUERY_SSL_CTX_INFO: {
    ਬਣਤਰ curl_tlssessioninfo *info = pres2;
    ਜੇ(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      ਵਾਪਸ CURLE_OK;
    ਰੋਕੋ;
  }
  ਹਾਲਤ CF_QUERY_ALPN_NEGOTIATED: {
    ਅਚਲ ਅੱਖਰ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ਨਿਰਾ;
    ਵਾਪਸ CURLE_OK;
  }
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ਸਥਿਰ bool cf_quiche_conn_is_alive(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    bool *ਨਿਵੇਸ਼_pending)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ਨਿਵੇਸ਼_pending = FALSE;
  ਜੇ(!ctx->qconn)
    ਵਾਪਸ FALSE;

  ਜੇ(quiche_conn_is_closed(ctx->qconn)) {
    ਜੇ(quiche_conn_is_timed_ਬਾਹਰਲਾ(ctx->qconn))
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "connection was closed due to idle timeout");
    ਨਹੀਂਤਾਂ
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "connection is closed");
    ਵਾਪਸ FALSE;
  }

  ਜੇ(!cf->ਅਗਲਾ || !cf->ਅਗਲਾ->cft->is_alive(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਨਿਵੇਸ਼_pending))
    ਵਾਪਸ FALSE;

  ਜੇ(*ਨਿਵੇਸ਼_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *ਨਿਵੇਸ਼_pending = FALSE;
    ਜੇ(cf_process_ingress(cf, ਅੰਕੜੇ))
      alive = FALSE;
    ਨਹੀਂਤਾਂ {
      alive = TRUE;
    }
  }

  ਵਾਪਸ alive;
}

ਬਣਤਰ Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_quiche_destroy,
  cf_quiche_connect,
  cf_quiche_shutdown,
  cf_quiche_adjust_pollset,
  Curl_cf_def_ਅੰਕੜੇ_pending,
  cf_quiche_send,
  cf_quiche_recv,
  cf_quiche_cntrl,
  cf_quiche_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_quiche_query,
};

CURLcode Curl_cf_quiche_create(ਬਣਤਰ Curl_cfilter **pcf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_peer *origin,
                               ਬਣਤਰ Curl_peer *peer,
                               ਬਣਤਰ connectdata *conn,
                               ਬਣਤਰ Curl_sockaddr_ex *addr)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = ਨਿਰਾ;
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਤੀਜਾ = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  cf->conn = conn;

  ਨਤੀਜਾ = Curl_cf_udp_create(&cf->ਅਗਲਾ, ਅੰਕੜੇ, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ਨਿਰਾ, TRNSPRT_QUIC);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  cf->ਅਗਲਾ->conn = cf->conn;
  cf->ਅਗਲਾ->sockindex = cf->sockindex;

ਬਾਹਰਲਾ:
  *pcf = (!ਨਤੀਜਾ) ? cf : ਨਿਰਾ;
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(cf)
      Curl_conn_cf_discard_chain(&cf, ਅੰਕੜੇ);
    ਨਹੀਂਤਾਂ ਜੇ(ctx)
      cf_quiche_ctx_free(ctx);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_quiche_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                     ਬਣਤਰ Curl_peer *origin,
                                     ਬਣਤਰ Curl_peer *peer,
                                     ਬਣਤਰ Curl_cfilter **pcf)
{
  ਬਣਤਰ cf_quiche_ctx *ctx = ਨਿਰਾ;
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਤੀਜਾ = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  Curl_conn_cf_ਪਾਓ_after(cf_at, cf);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ) {
    curlx_safefree(cf);
    ਜੇ(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ
