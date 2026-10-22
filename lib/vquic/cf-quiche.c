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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#అన్తర్భావ <quiche.h>
#అన్తర్భావ <openssl/err.h>
#అన్తర్భావ <openssl/ssl.h>

#అన్తర్భావ "bufq.h"
#అన్తర్భావ "u32_ptrset.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cf-socket.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "http.h"
#అన్తర్భావ "http1.h"
#అన్తర్భావ "sockaddr.h"
#అన్తర్భావ "vdns/cf-dns.h"
#అన్తర్భావ "vquic/vquic.h"
#అన్తర్భావ "vquic/vquic_int.h"
#అన్తర్భావ "vquic/vquic-tls.h"
#అన్తర్భావ "vquic/cf-quiche.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "vtls/openssl.h"
#అన్తర్భావ "vtls/vtls.h"

/* HTTP/3 error values defined in RFC 9114, ch. 8.1 */
#నిర్వచించు CURL_H3_NO_ERROR  0x0100

#నిర్వచించు MAX_PKT_వ్యవధి          10

#నిర్వచించు QUIC_MAX_STREAMS       100

#నిర్వచించు H3_ప్రవాహం_కిటికీ_కొలత  (1024 * 128)
#నిర్వచించు H3_ప్రవాహం_CHUNK_కొలత   (1024 * 16)
/* Receive and Send max number of chunks follows from the
 * chunk size and window size */
#నిర్వచించు H3_ప్రవాహం_RECV_CHUNKS \
  (H3_ప్రవాహం_కిటికీ_కొలత / H3_ప్రవాహం_CHUNK_కొలత)

/*
 * Store quiche version info in this buffer.
 */
శూన్య Curl_quiche_ver(అక్షరం *p, size_t len)
{
  (శూన్య)curl_msnprintf(p, len, "quiche/%s", quiche_సంస్కరణ());
}

నిర్మాణం cf_quiche_ctx {
  నిర్మాణం cf_quic_ctx q;
  నిర్మాణం ssl_peer ssl_peer;
  నిర్మాణం curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  నిర్మాణం curltime started_at;        /* time the current attempt started */
  నిర్మాణం curltime handshake_at;      /* time connect handshake finished */
  నిర్మాణం u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  నిర్మాణం dynbuf h1hdr;               /* temp buffer for header construction */
  నిర్మాణం bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t దత్తాంశం_recvd;
  బిట్టు(initialized);
  బిట్టు(goaway);                       /* got GOAWAY from server */
  బిట్టు(x509_store_అమరిక);             /* if x509 store has been set up */
  బిట్టు(shutdown_started);             /* queued shutdown packets */
  బిట్టు(stats_reported);               /* connect statistics reported */
};

#నిర్వచితమైతే DEBUG_QUICHE
/* initialize debug log callback only once */
స్థిర పూర్ణాంకం debug_log_init = 0;
స్థిర శూన్య quiche_debug_log(స్థిరరాశి అక్షరం *పంక్తి, శూన్య *argp)
{
  (శూన్య)argp;
  curl_mfprintf(stderr, "%s\n", పంక్తి);
}
#అయితే_ముగింపు

స్థిర శూన్య h3_ప్రవాహం_సమితి_free(uint32_t id, శూన్య *ప్రవాహం);

స్థిర CURLcode cf_quiche_ctx_init(నిర్మాణం cf_quiche_ctx *ctx,
                                   నిర్మాణం Curl_peer *origin,
                                   నిర్మాణం Curl_peer *peer,
                                   నిర్మాణం ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#నిర్వచితమైతే DEBUG_QUICHE
  ఒకవేళ(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, ఖాళీ);
    debug_log_init = 1;
  }
#అయితే_ముగింపు
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_శీర్షిక);
  Curl_u32_ptrset_init(&ctx->streams, h3_ప్రవాహం_సమితి_free);
  Curl_bufq_init2(&ctx->writebuf, H3_ప్రవాహం_CHUNK_కొలత, H3_ప్రవాహం_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_హద్దు);
  ctx->దత్తాంశం_recvd = 0;
  ctx->initialized = TRUE;
  తిరిగి Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

స్థిర శూన్య cf_quiche_ctx_free(నిర్మాణం cf_quiche_ctx *ctx)
{
  ఒకవేళ(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = ఖాళీ;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

స్థిర శూన్య cf_quiche_ctx_close(నిర్మాణం cf_quiche_ctx *ctx)
{
  ఒకవేళ(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = ఖాళీ;
  }
  ఒకవేళ(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = ఖాళీ;
  }
  ఒకవేళ(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = ఖాళీ;
  }
  ఒకవేళ(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = ఖాళీ;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

స్థిర CURLcode cf_flush_egress(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం);

/**
 * All about the H3 internals of a stream
 */
నిర్మాణం h3_ప్రవాహం_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  నిర్మాణం h1_req_parser h1; /* h1 request parsing */
  uint64_t error3;         /* HTTP/3 stream error code */
  int32_t స్థితి_సంకేతం;     /* HTTP status code */
  CURLcode xfer_ఫలితం;    /* result from cf_quiche_write_(hd/body) */
  బిట్టు(opened);             /* TRUE after stream has been opened */
  బిట్టు(closed);             /* TRUE on stream close */
  బిట్టు(reset);              /* TRUE on stream reset */
  బిట్టు(send_closed);        /* stream is locally closed */
  బిట్టు(resp_hds_complete);  /* final response has been received */
  బిట్టు(resp_got_శీర్షిక);    /* TRUE when h3 stream has recvd some HEADER */
  బిట్టు(quic_flow_blocked);  /* stream is blocked by QUIC flow control */
};

స్థిర శూన్య h3_ప్రవాహం_ctx_free(నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం)
{
  Curl_h1_req_parse_free(&ప్రవాహం->h1);
  curlx_free(ప్రవాహం);
}

స్థిర శూన్య h3_ప్రవాహం_సమితి_free(uint32_t id, శూన్య *ప్రవాహం)
{
  (శూన్య)id;
  DEBUGASSERT(ప్రవాహం);
  h3_ప్రవాహం_ctx_free((నిర్మాణం h3_ప్రవాహం_ctx *)ప్రవాహం);
}

రకనిర్వచనం bool cf_quiche_svisit(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *sdata,
                              నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం,
                              శూన్య *user_దత్తాంశం);

నిర్మాణం cf_quiche_సందర్శించు_ctx {
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం Curl_multi *multi;
  cf_quiche_svisit *cb;
  శూన్య *user_దత్తాంశం;
};

స్థిర bool cf_quiche_ప్రవాహం_do(uint32_t mid, శూన్య *val, శూన్య *user_దత్తాంశం)
{
  నిర్మాణం cf_quiche_సందర్శించు_ctx *vctx = user_దత్తాంశం;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = val;
  నిర్మాణం Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  ఒకవేళ(sdata)
    తిరిగి vctx->cb(vctx->cf, sdata, ప్రవాహం, vctx->user_దత్తాంశం);
  తిరిగి TRUE;
}

స్థిర శూన్య cf_quiche_for_all_streams(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      శూన్య *user_దత్తాంశం)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  నిర్మాణం cf_quiche_సందర్శించు_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_దత్తాంశం = user_దత్తాంశం;
  Curl_u32_ptrset_సందర్శించు(&ctx->streams, cf_quiche_ప్రవాహం_do, &vctx);
}

స్థిర bool cf_quiche_do_resume(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *sdata,
                                నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం,
                                శూన్య *user_దత్తాంశం)
{
  (శూన్య)user_దత్తాంశం;
  ఒకవేళ(ప్రవాహం->quic_flow_blocked) {
    ప్రవాహం->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", ప్రవాహం->id);
  }
  తిరిగి TRUE;
}

స్థిర bool cf_quiche_do_expire(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *sdata,
                                నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం,
                                శూన్య *user_దత్తాంశం)
{
  (శూన్య)user_దత్తాంశం;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  ప్రవాహం->xfer_ఫలితం = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  తిరిగి TRUE;
}

స్థిర CURLcode h3_దత్తాంశం_అమరిక(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);

  ఒకవేళ(ప్రవాహం)
    తిరిగి CURLE_OK;

  ప్రవాహం = curlx_calloc(1, పరిమాణం(*ప్రవాహం));
  ఒకవేళ(!ప్రవాహం)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ప్రవాహం->id = -1;
  Curl_h1_req_parse_init(&ప్రవాహం->h1, H1_PARSE_DEFAULT_MAX_పంక్తి_LEN);

  ఒకవేళ(Curl_u32_ptrset_సమితి(&ctx->streams, దత్తాంశం->mid, ప్రవాహం)) {
    h3_ప్రవాహం_ctx_free(ప్రవాహం);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  తిరిగి CURLE_OK;
}

స్థిర శూన్య cf_quiche_ప్రవాహం_close(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ఫలితం;

  ఒకవేళ(ctx->qconn && !ప్రవాహం->closed) {
    quiche_conn_ప్రవాహం_shutdown(ctx->qconn, ప్రవాహం->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    ఒకవేళ(!ప్రవాహం->send_closed) {
      quiche_conn_ప్రవాహం_shutdown(ctx->qconn, ప్రవాహం->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      ప్రవాహం->send_closed = TRUE;
    }
    ప్రవాహం->closed = TRUE;
    ఫలితం = cf_flush_egress(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  ప్రవాహం->id, (పూర్ణాంకం)ఫలితం);
  }
}

స్థిర శూన్య h3_దత్తాంశం_పూర్తి(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);

  (శూన్య)cf;
  ఒకవేళ(ప్రవాహం) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] easy handle is done", ప్రవాహం->id);
    cf_quiche_ప్రవాహం_close(cf, దత్తాంశం, ప్రవాహం);
    Curl_u32_ptrset_unset(&ctx->streams, దత్తాంశం->mid);
  }
}

స్థిర శూన్య cf_quiche_expire_conn_closed(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం)
{
  DEBUGASSERT(దత్తాంశం->multi);
  CURL_TRC_CF(దత్తాంశం, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, దత్తాంశం->multi, cf_quiche_do_expire, ఖాళీ);
}

స్థిర శూన్య cf_quiche_write_hd(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం,
                               స్థిరరాశి అక్షరం *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ఒకవేళ(!ప్రవాహం->xfer_ఫలితం) {
    ప్రవాహం->xfer_ఫలితం = Curl_xfer_write_resp_hd(దత్తాంశం, buf, blen, eos);
    ఒకవేళ(ప్రవాహం->xfer_ఫలితం)
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", ప్రవాహం->id, (పూర్ణాంకం)ప్రవాహం->xfer_ఫలితం,
                  blen);
  }
}

నిర్మాణం cb_ctx {
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం Curl_easy *దత్తాంశం;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం;
};

స్థిర bool is_valid_h3_శీర్షిక(స్థిరరాశి uint8_t *hdr, size_t hlen)
{
  అయితే(hlen--) {
    ఎంపిక(*hdr++) {
    స్థితి '\n':
    స్థితి '\r':
    స్థితి '\0':
      తిరిగి FALSE;
    }
  }
  తిరిగి TRUE;
}

స్థిర పూర్ణాంకం cb_each_శీర్షిక(uint8_t *name, size_t name_len,
                          uint8_t *విలువ, size_t విలువ_len,
                          శూన్య *argp)
{
  నిర్మాణం cb_ctx *x = argp;
  నిర్మాణం Curl_cfilter *cf = x->cf;
  నిర్మాణం Curl_easy *దత్తాంశం = x->దత్తాంశం;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = x->ప్రవాహం;
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!ప్రవాహం || ప్రవాహం->xfer_ఫలితం)
    తిరిగి 1; /* abort iteration */

  ఒకవేళ((name_len == 7) && !అక్షరశ్రేణి_పోలిక(HTTP_PSEUDO_స్థితి, (అక్షరం *)name, 7) &&
     is_valid_h3_శీర్షిక(విలువ, విలువ_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    ఫలితం = Curl_http_decode_స్థితి(&ప్రవాహం->స్థితి_సంకేతం,
                                     (స్థిరరాశి అక్షరం *)విలువ, విలువ_len);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(&ctx->h1hdr, (స్థిరరాశి అక్షరం *)విలువ, విలువ_len);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    ఒకవేళ(!ఫలితం)
      cf_quiche_write_hd(cf, దత్తాంశం, ప్రవాహం, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] status: %s",
                ప్రవాహం->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  లేకపోతే {
    ఒకవేళ(is_valid_h3_శీర్షిక(విలువ, విలువ_len) &&
       is_valid_h3_శీర్షిక(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  ప్రవాహం->id, (పూర్ణాంకం)name_len, name,
                  (పూర్ణాంకం)విలువ_len, విలువ);
      curlx_dyn_reset(&ctx->h1hdr);
      ఫలితం = curlx_dyn_addn(&ctx->h1hdr, (స్థిరరాశి అక్షరం *)name, name_len);
      ఒకవేళ(!ఫలితం)
        ఫలితం = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      ఒకవేళ(!ఫలితం)
        ఫలితం = curlx_dyn_addn(&ctx->h1hdr, (స్థిరరాశి అక్షరం *)విలువ, విలువ_len);
      ఒకవేళ(!ఫలితం)
        ఫలితం = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      ఒకవేళ(!ఫలితం)
        cf_quiche_write_hd(cf, దత్తాంశం, ప్రవాహం, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    లేకపోతే
      CURL_TRC_CF(x->దత్తాంశం, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  ప్రవాహం->id, విలువ_len + name_len);
  }

  ఒకవేళ(ఫలితం) {
    CURL_TRC_CF(x->దత్తాంశం, x->cf, "[%" PRIu64 "] on header error %d",
                ప్రవాహం->id, (పూర్ణాంకం)ఫలితం);
    ఒకవేళ(!ప్రవాహం->xfer_ఫలితం)
      ప్రవాహం->xfer_ఫలితం = ఫలితం;
  }
  తిరిగి ప్రవాహం->xfer_ఫలితం ? 1 : 0;
}

స్థిర CURLcode ప్రవాహం_resp_read(శూన్య *reader_ctx,
                                 గుర్తులేని అక్షరం *buf, size_t len,
                                 size_t *pnread)
{
  నిర్మాణం cb_ctx *x = reader_ctx;
  నిర్మాణం cf_quiche_ctx *ctx = x->cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, x->దత్తాంశం);
  ssize_t nread;

  *pnread = 0;
  ఒకవేళ(!ప్రవాహం)
    తిరిగి CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, ప్రవాహం->id, buf, len);
  ఒకవేళ(!curlx_sztouz(nread, pnread))
    తిరిగి CURLE_AGAIN;
  తిరిగి CURLE_OK;
}

స్థిర శూన్య cf_quiche_flush_body(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  స్థిరరాశి uint8_t *buf;
  size_t blen;

  అయితే(ప్రవాహం && !ప్రవాహం->xfer_ఫలితం) {
    ఒకవేళ(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      ప్రవాహం->xfer_ఫలితం = Curl_xfer_write_resp(
        దత్తాంశం, (స్థిరరాశి అక్షరం *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      ఒకవేళ(ప్రవాహం->xfer_ఫలితం) {
        CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", ప్రవాహం->id, (పూర్ణాంకం)ప్రవాహం->xfer_ఫలితం, blen);
      }
    }
    లేకపోతే
      ఆపు;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

స్థిర శూన్య cf_quiche_recv_body(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  నిర్మాణం cb_ctx cb_ctx;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!ప్రవాహం)
    తిరిగి;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.దత్తాంశం = దత్తాంశం;
  cb_ctx.ప్రవాహం = ప్రవాహం;
  Curl_bufq_reset(&ctx->writebuf);
  అయితే(!ఫలితం) {
    ఫలితం = Curl_bufq_slurp(&ctx->writebuf,
                             ప్రవాహం_resp_read, &cb_ctx, &nread);
    ఒకవేళ(!ఫలితం)
      cf_quiche_flush_body(cf, దత్తాంశం, ప్రవాహం);
    లేకపోతే ఒకవేళ(ఫలితం == CURLE_AGAIN)
      ఆపు;
    లేకపోతే ఒకవేళ(ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] recv_body error %d",
                  ప్రవాహం->id, (పూర్ణాంకం)ఫలితం);
      failf(దత్తాంశం, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            ప్రవాహం->id, (పూర్ణాంకం)ఫలితం);
      ప్రవాహం->closed = TRUE;
      ప్రవాహం->reset = TRUE;
      ప్రవాహం->send_closed = TRUE;
      ఒకవేళ(!ప్రవాహం->xfer_ఫలితం)
        ప్రవాహం->xfer_ఫలితం = ఫలితం;
    }
  }
  cf_quiche_flush_body(cf, దత్తాంశం, ప్రవాహం);
}

స్థిర శూన్య cf_quiche_process_ev(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం,
                                 quiche_h3_event *ev)
{
  గణన quiche_h3_event_type type;

  ఒకవేళ(!ప్రవాహం)
    తిరిగి;

  type = quiche_h3_event_type(ev);

  ఎంపిక(type) {
  స్థితి QUICHE_H3_EVENT_శీర్షికలు: {
    నిర్మాణం cb_ctx cb_ctx;
    ప్రవాహం->resp_got_శీర్షిక = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.దత్తాంశం = దత్తాంశం;
    cb_ctx.ప్రవాహం = ప్రవాహం;
    quiche_h3_event_for_each_శీర్షిక(ev, cb_each_శీర్షిక, &cb_ctx);
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] <- [HEADERS]", ప్రవాహం->id);
    Curl_multi_mark_dirty(దత్తాంశం);
    ఆపు;
  }
  స్థితి QUICHE_H3_EVENT_దత్తాంశం:
    ఒకవేళ(!ప్రవాహం->resp_hds_complete) {
      ప్రవాహం->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, దత్తాంశం, ప్రవాహం, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, దత్తాంశం, ప్రవాహం);
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] <- [DATA]", ప్రవాహం->id);
    Curl_multi_mark_dirty(దత్తాంశం);
    ఆపు;

  స్థితి QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] RESET", ప్రవాహం->id);
    ప్రవాహం->closed = TRUE;
    ప్రవాహం->reset = TRUE;
    ప్రవాహం->send_closed = TRUE;
    Curl_multi_mark_dirty(దత్తాంశం);
    ఆపు;

  స్థితి QUICHE_H3_EVENT_ముగిసిన:
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] CLOSED", ప్రవాహం->id);
    ఒకవేళ(!ప్రవాహం->resp_hds_complete) {
      ప్రవాహం->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, దత్తాంశం, ప్రవాహం, "\r\n", 2, TRUE);
    }
    ప్రవాహం->closed = TRUE;
    Curl_multi_mark_dirty(దత్తాంశం);
    ఆపు;

  స్థితి QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] <- [GOAWAY]", ప్రవాహం->id);
    ఆపు;

  అప్రమేయ:
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] recv, unhandled event %d",
                ప్రవాహం->id, (పూర్ణాంకం)type);
    ఆపు;
  }
}

నిర్మాణం cf_quiche_disp_ctx {
  uint64_t ప్రవాహం_id;
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం Curl_multi *multi;
  quiche_h3_event *ev;
};

స్థిర bool cf_quiche_disp_event(uint32_t mid, శూన్య *val, శూన్య *user_దత్తాంశం)
{
  నిర్మాణం cf_quiche_disp_ctx *dctx = user_దత్తాంశం;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = val;

  ఒకవేళ(ప్రవాహం->id == dctx->ప్రవాహం_id) {
    నిర్మాణం Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    ఒకవేళ(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, ప్రవాహం, dctx->ev);
    తిరిగి FALSE; /* stop iterating */
  }
  తిరిగి TRUE;
}

స్థిర CURLcode cf_poll_events(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = ఖాళీ;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  అయితే(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    ఒకవేళ(rv == QUICHE_H3_ERR_పూర్తి) {
      ఆపు;
    }
    లేకపోతే ఒకవేళ(rv < 0) {
      CURL_TRC_CF(దత్తాంశం, cf, "error poll: %" PRId64, rv);
      తిరిగి CURLE_HTTP3;
    }
    లేకపోతే {
      ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
      ఒకవేళ(ప్రవాహం && ప్రవాహం->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, దత్తాంశం, ప్రవాహం, ev);
        quiche_h3_event_free(ev);
        ఒకవేళ(ప్రవాహం->xfer_ఫలితం)
          తిరిగి ప్రవాహం->xfer_ఫలితం;
      }
      లేకపోతే {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        నిర్మాణం cf_quiche_disp_ctx dctx;
        dctx.ప్రవాహం_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = దత్తాంశం->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_సందర్శించు(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  తిరిగి CURLE_OK;
}

నిర్మాణం recv_ctx {
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం Curl_easy *దత్తాంశం;
  పూర్ణాంకం pkts;
};

స్థిర CURLcode cf_quiche_recv_pkts(స్థిరరాశి గుర్తులేని అక్షరం *buf, size_t buflen,
                                    size_t gso_కొలత,
                                    నిర్మాణం sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    శూన్య *userp)
{
  నిర్మాణం recv_ctx *r = userp;
  నిర్మాణం cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, స్థానభ్రంశం, nread;
  ssize_t rv;

  (శూన్య)ecn;

  recv_info.గమ్యం = (నిర్మాణం sockaddr *)&ctx->q.local_addr;
  recv_info.గమ్యం_len = ctx->q.local_addrlen;
  recv_info.from = (నిర్మాణం sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  కొరకు(స్థానభ్రంశం = 0; స్థానభ్రంశం < buflen; స్థానభ్రంశం += gso_కొలత) {
    pktlen = ((స్థానభ్రంశం + gso_కొలత) <= buflen) ? gso_కొలత : (buflen - స్థానభ్రంశం);
    rv = quiche_conn_recv(ctx->qconn,
                          (గుర్తులేని అక్షరం *)CURL_UNCONST(buf + స్థానభ్రంశం),
                          pktlen, &recv_info);
    ఒకవేళ(!curlx_sztouz(rv, &nread)) {
      ఒకవేళ(QUICHE_ERR_పూర్తి == rv) {
        ఒకవేళ(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->దత్తాంశం, r->cf, "ingress, connection is draining");
          తిరిగి CURLE_RECV_ERROR;
        }
        ఒకవేళ(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->దత్తాంశం, r->cf, "ingress, connection is closed");
          తిరిగి CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->దత్తాంశం, r->cf, "ingress, quiche is DONE");
        తిరిగి CURLE_OK;
      }
      లేకపోతే ఒకవేళ(QUICHE_ERR_TLS_FAIL == rv) {
        దీర్ఘ verify_ok = SSL_get_verify_ఫలితం(ctx->tls.ossl.ssl);
        ఒకవేళ(verify_ok != X509_V_OK) {
          failf(r->దత్తాంశం, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          తిరిగి CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->దత్తాంశం, "ingress, quiche reports TLS fail");
        తిరిగి CURLE_RECV_ERROR;
      }
      లేకపోతే {
        failf(r->దత్తాంశం, "quiche reports error %zd on receive", rv);
        తిరిగి CURLE_RECV_ERROR;
      }
    }
    లేకపోతే ఒకవేళ(nread < pktlen) {
      CURL_TRC_CF(r->దత్తాంశం, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_process_ingress(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  నిర్మాణం recv_ctx rctx;
  CURLcode ఫలితం;

  DEBUGASSERT(ctx->qconn);
  ఫలితం = Curl_vquic_tls_before_recv(&ctx->tls, cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  rctx.cf = cf;
  rctx.దత్తాంశం = దత్తాంశం;
  rctx.pkts = 0;

  ఫలితం = Curl_vquic_recv_packets(cf, దత్తాంశం, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(దత్తాంశం->multi);
    cf_quiche_for_all_streams(cf, దత్తాంశం->multi, cf_quiche_do_resume, ఖాళీ);
  }
  తిరిగి cf_poll_events(cf, దత్తాంశం);
}

నిర్మాణం read_ctx {
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం Curl_easy *దత్తాంశం;
  quiche_send_info send_info;
};

స్థిర CURLcode read_pkt_గమ్యం_send(శూన్య *userp,
                                 గుర్తులేని అక్షరం *buf, size_t buflen,
                                 size_t *pnread)
{
  నిర్మాణం read_ctx *x = userp;
  నిర్మాణం cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  ఒకవేళ(rv == QUICHE_ERR_పూర్తి)
    తిరిగి CURLE_AGAIN;

  ఒకవేళ(!curlx_sztouz(rv, pnread)) {
    failf(x->దత్తాంశం, "quiche_conn_send returned %zd", rv);
    తిరిగి CURLE_SEND_ERROR;
  }
  తిరిగి CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
స్థిర CURLcode cf_flush_egress(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode ఫలితం;
  int64_t expiry_ns;
  int64_t timeout_ns;
  నిర్మాణం read_ctx readx;
  size_t pkt_లెక్క, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  ఒకవేళ(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    ఒకవేళ(quiche_conn_is_closed(ctx->qconn)) {
      ఒకవేళ(quiche_conn_is_timed_బహిర్గతం(ctx->qconn))
        failf(దత్తాంశం, "connection closed by idle timeout");
      లేకపోతే
        failf(దత్తాంశం, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, దత్తాంశం);
      తిరిగి CURLE_SEND_ERROR;
    }
  }

  ఫలితం = Curl_vquic_flush(cf, దత్తాంశం, &ctx->q);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      Curl_expire(దత్తాంశం, 1, EXPIRE_QUIC);
      తిరిగి CURLE_OK;
    }
    తిరిగి ఫలితం;
  }

  readx.cf = cf;
  readx.దత్తాంశం = దత్తాంశం;
  memset(&readx.send_info, 0, పరిమాణం(readx.send_info));
  pkt_లెక్క = 0;
  gsolen = quiche_conn_max_send_udp_payload_కొలత(ctx->qconn);
  కొరకు(;;) {
    /* add the next packet to send, if any, to our buffer */
    ఫలితం = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_గమ్యం_send, &readx, &nread);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం != CURLE_AGAIN)
        తిరిగి ఫలితం;
      /* Nothing more to add, flush and leave */
      ఫలితం = Curl_vquic_send(cf, దత్తాంశం, &ctx->q, gsolen);
      ఒకవేళ(ఫలితం) {
        ఒకవేళ(ఫలితం == CURLE_AGAIN) {
          Curl_expire(దత్తాంశం, 1, EXPIRE_QUIC);
          తిరిగి CURLE_OK;
        }
        తిరిగి ఫలితం;
      }
      వెళ్ళు బహిర్గతం;
    }

    ++pkt_లెక్క;
    ఒకవేళ(nread < gsolen || pkt_లెక్క >= MAX_PKT_వ్యవధి) {
      ఫలితం = Curl_vquic_send(cf, దత్తాంశం, &ctx->q, gsolen);
      ఒకవేళ(ఫలితం) {
        ఒకవేళ(ఫలితం == CURLE_AGAIN) {
          Curl_expire(దత్తాంశం, 1, EXPIRE_QUIC);
          తిరిగి CURLE_OK;
        }
        వెళ్ళు బహిర్గతం;
      }
      pkt_లెక్క = 0;
    }
  }

బహిర్గతం:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  ఒకవేళ(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(దత్తాంశం, (timeout_ns / 1000000), EXPIRE_QUIC);
  తిరిగి ఫలితం;
}

స్థిర CURLcode recv_closed_ప్రవాహం(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   size_t *pnread)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(ప్రవాహం);
  *pnread = 0;
  ఒకవేళ(ప్రవాహం->reset) {
    ఒకవేళ(ప్రవాహం->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(దత్తాంశం, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", ప్రవాహం->id);
      connclose(cf->conn); /* do not use this anymore */
      దత్తాంశం->state.refused_ప్రవాహం = TRUE;
      తిరిగి CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    లేకపోతే ఒకవేళ(ప్రవాహం->resp_hds_complete && దత్తాంశం->req.no_body) {
        CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", ప్రవాహం->id, ప్రవాహం->error3,
                    Curl_vquic_h3_err_str(ప్రవాహం->error3));
        తిరిగి CURLE_OK;
    }
    failf(దత్తాంశం, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", ప్రవాహం->id, ప్రవాహం->error3,
          Curl_vquic_h3_err_str(ప్రవాహం->error3));
    ఫలితం = దత్తాంశం->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                ప్రవాహం->id, (పూర్ణాంకం)ఫలితం);
  }
  లేకపోతే ఒకవేళ(!ప్రవాహం->resp_got_శీర్షిక) {
    failf(దత్తాంశం, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          ప్రవాహం->id);
    ఫలితం = CURLE_HTTP3;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_quiche_recv(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                               అక్షరం *buf, size_t blen, size_t *pnread)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
  CURLcode ఫలితం = CURLE_OK;

  *pnread = 0;
  (శూన్య)buf;
  (శూన్య)blen;
  Curl_vquic_ctx_update_సమయం(&ctx->q, Curl_pgrs_now(దత్తాంశం));

  ఒకవేళ(!ప్రవాహం)
    తిరిగి CURLE_RECV_ERROR;

  ఫలితం = cf_process_ingress(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం) {
    CURL_TRC_CF(దత్తాంశం, cf, "cf_recv, error on ingress");
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(ప్రవాహం->xfer_ఫలితం) {
    cf_quiche_ప్రవాహం_close(cf, దత్తాంశం, ప్రవాహం);
    ఫలితం = ప్రవాహం->xfer_ఫలితం;
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే ఒకవేళ(ప్రవాహం->closed)
    ఫలితం = recv_closed_ప్రవాహం(cf, దత్తాంశం, pnread);
  లేకపోతే ఒకవేళ(quiche_conn_is_draining(ctx->qconn)) {
    failf(దత్తాంశం, "QUIC connection is draining");
    ఫలితం = CURLE_HTTP3;
  }
  లేకపోతే
    ఫలితం = CURLE_AGAIN;

బహిర్గతం:
  ఫలితం = Curl_1st_fatal(ఫలితం, cf_flush_egress(cf, దత్తాంశం));
  ఒకవేళ(*pnread > 0)
    ctx->దత్తాంశం_recvd += *pnread;
  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, ప్రవాహం->id, blen, (పూర్ణాంకం)ఫలితం, *pnread,
              ctx->దత్తాంశం_recvd);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_quiche_send_body(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం,
                                    స్థిరరాశి uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, ప్రవాహం->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  ఒకవేళ(rv == QUICHE_H3_ERR_పూర్తి || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    ఒకవేళ(!quiche_conn_ప్రవాహం_writable(ctx->qconn, ప్రవాహం->id, len)) {
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", ప్రవాహం->id, len);
      ప్రవాహం->quic_flow_blocked = TRUE;
    }
    తిరిగి CURLE_AGAIN;
  }
  లేకపోతే ఒకవేళ(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_ప్రవాహం_STATE) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", ప్రవాహం->id, len);
    తిరిగి CURLE_HTTP3;
  }
  లేకపోతే ఒకవేళ(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_కొలత) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                ప్రవాహం->id, len);
    తిరిగి CURLE_SEND_ERROR;
  }
  లేకపోతే ఒకవేళ(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                ప్రవాహం->id, len, rv);
    తిరిగి CURLE_SEND_ERROR;
  }
  లేకపోతే {
    ఒకవేళ(eos && (len == *pnwritten))
      ప్రవాహం->send_closed = TRUE;
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                ప్రవాహం->id, len, ప్రవాహం->send_closed, *pnwritten);
    తిరిగి CURLE_OK;
  }
}

స్థిర CURLcode h3_open_ప్రవాహం(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               స్థిరరాశి uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
  size_t nheader, i;
  int64_t rv;
  నిర్మాణం dynhds h2_శీర్షికలు;
  quiche_h3_శీర్షిక *nva = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  *pnwritten = 0;
  ఒకవేళ(!ప్రవాహం) {
    ఫలితం = h3_దత్తాంశం_అమరిక(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
    DEBUGASSERT(ప్రవాహం);
  }

  Curl_dynhds_init(&h2_శీర్షికలు, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(ప్రవాహం);

  ఫలితం = Curl_h1_req_parse_read(&ప్రవాహం->h1, buf, blen, ఖాళీ,
    !దత్తాంశం->state.http_ignorecustom ?
    CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST) : ఖాళీ,
    0, pnwritten);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ఒకవేళ(!ప్రవాహం->h1.పూర్తి) {
    /* need more data */
    వెళ్ళు బహిర్గతం;
  }
  DEBUGASSERT(ప్రవాహం->h1.req);

  ఫలితం = Curl_http_req_గమ్యం_h2(&h2_శీర్షికలు, ప్రవాహం->h1.req, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* no longer needed */
  Curl_h1_req_parse_free(&ప్రవాహం->h1);

  nheader = Curl_dynhds_లెక్క(&h2_శీర్షికలు);
  nva = curlx_malloc(పరిమాణం(quiche_h3_శీర్షిక) * nheader);
  ఒకవేళ(!nva) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  కొరకు(i = 0; i < nheader; ++i) {
    నిర్మాణం dynhds_నమోదు *e = Curl_dynhds_getn(&h2_శీర్షికలు, i);
    nva[i].name = (గుర్తులేని అక్షరం *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].విలువ = (గుర్తులేని అక్షరం *)e->విలువ;
    nva[i].విలువ_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  ఒకవేళ(eos && !blen)
    ప్రవాహం->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              ప్రవాహం->send_closed);
  CURL_TRC_CF(దత్తాంశం, cf, "quiche_send_request() -> %" PRId64, rv);
  ఒకవేళ(rv < 0) {
    ఒకవేళ(QUICHE_H3_ERR_ప్రవాహం_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] blocked", ప్రవాహం->id);
      ప్రవాహం->quic_flow_blocked = TRUE;
      ఫలితం = CURLE_AGAIN;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే {
      CURL_TRC_CF(దత్తాంశం, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&దత్తాంశం->state.url), rv);
    }
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు బహిర్గతం;
  }

  DEBUGASSERT(!ప్రవాహం->opened);
  ప్రవాహం->id = (uint64_t)rv;
  ప్రవాహం->opened = TRUE;
  ప్రవాహం->closed = FALSE;
  ప్రవాహం->reset = FALSE;

  ఒకవేళ(Curl_trc_is_verbose(దత్తాంశం)) {
    infof(దత్తాంశం, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          ప్రవాహం->id, Curl_bufref_ptr(&దత్తాంశం->state.url));
    కొరకు(i = 0; i < nheader; ++i) {
      infof(దత్తాంశం, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", ప్రవాహం->id,
            (పూర్ణాంకం)nva[i].name_len, nva[i].name,
            (పూర్ణాంకం)nva[i].విలువ_len, nva[i].విలువ);
    }
  }

  ఒకవేళ(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, దత్తాంశం, ప్రవాహం, buf, blen, eos, &nwritten);
    ఒకవేళ(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      ఫలితం = r2;
    }
    లేకపోతే ఒకవేళ(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

బహిర్గతం:
  curlx_free(nva);
  Curl_dynhds_free(&h2_శీర్షికలు);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_quiche_send(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                               స్థిరరాశి uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
  CURLcode ఫలితం;

  *pnwritten = 0;
  Curl_vquic_ctx_update_సమయం(&ctx->q, Curl_pgrs_now(దత్తాంశం));

  ఫలితం = cf_process_ingress(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(!ప్రవాహం || !ప్రవాహం->opened) {
    ఫలితం = h3_open_ప్రవాహం(cf, దత్తాంశం, buf, len, eos, pnwritten);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
  }
  లేకపోతే ఒకవేళ(ప్రవాహం->xfer_ఫలితం) {
    cf_quiche_ప్రవాహం_close(cf, దత్తాంశం, ప్రవాహం);
    ఫలితం = ప్రవాహం->xfer_ఫలితం;
  }
  లేకపోతే ఒకవేళ(ప్రవాహం->closed) {
    ఒకవేళ(ప్రవాహం->resp_hds_complete) {
      /* sending request body on a stream that has been closed by the
       * server. If the server has send us a final response, we should
       * silently discard the send data.
       * This happens for example on redirects where the server, instead
       * of reading the full request body closed the stream after
       * sending the 30x response.
       * This is sort of a race: had the transfer loop called recv first,
       * it would see the response and stop/discard sending on its own- */
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] discarding data "
                  "on closed stream with response", ప్రవాహం->id);
      ఫలితం = CURLE_OK;
      *pnwritten = len;
      వెళ్ళు బహిర్గతం;
    }
    CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", ప్రవాహం->id, len);
    ఫలితం = CURLE_HTTP3;
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే {
    ఫలితం = cf_quiche_send_body(cf, దత్తాంశం, ప్రవాహం, buf, len, eos, pnwritten);
  }

బహిర్గతం:
  ఫలితం = Curl_1st_fatal(ఫలితం, cf_flush_egress(cf, దత్తాంశం));

  CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              ప్రవాహం ? ప్రవాహం->id : (uint64_t)~0, len,
              (పూర్ణాంకం)ఫలితం, *pnwritten);
  తిరిగి ఫలితం;
}

స్థిర bool ప్రవాహం_is_writable(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);

  తిరిగి ప్రవాహం && (quiche_conn_ప్రవాహం_writable(
    ctx->qconn, ప్రవాహం->id, 1) > 0);
}

స్థిర CURLcode cf_quiche_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!ctx->qconn)
    తిరిగి CURLE_OK;

  Curl_pollset_check(దత్తాంశం, ps, ctx->q.sockfd, &want_recv, &want_send);
  ఒకవేళ(want_recv || want_send) {
    నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
    bool c_exhaust, s_exhaust;

    c_exhaust = FALSE; /* Have not found any call in quiche that tells
                          us if the connection itself is blocked */
    s_exhaust = want_send && ప్రవాహం && ప్రవాహం->opened &&
                (ప్రవాహం->quic_flow_blocked || !ప్రవాహం_is_writable(cf, దత్తాంశం));
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_ఖాళీలు(&ctx->q.sendbuf);

    ఫలితం = Curl_pollset_సమితి(దత్తాంశం, ps, ctx->q.sockfd, want_recv, want_send);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode h3_దత్తాంశం_pause(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (శూన్య)cf;
  ఒకవేళ(!pause) {
    Curl_multi_mark_dirty(దత్తాంశం);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_quiche_cntrl(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)arg1;
  (శూన్య)arg2;
  ఎంపిక(event) {
  స్థితి CF_CTRL_దత్తాంశం_అమరిక:
    ఆపు;
  స్థితి CF_CTRL_దత్తాంశం_PAUSE:
    ఫలితం = h3_దత్తాంశం_pause(cf, దత్తాంశం, (arg1 != 0));
    ఆపు;
  స్థితి CF_CTRL_దత్తాంశం_పూర్తి:
    h3_దత్తాంశం_పూర్తి(cf, దత్తాంశం);
    ఆపు;
  స్థితి CF_CTRL_దత్తాంశం_పూర్తి_SEND: {
    నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం = H3_ప్రవాహం_CTX(ctx, దత్తాంశం);
    ఒకవేళ(ప్రవాహం && !ప్రవాహం->send_closed) {
      గుర్తులేని అక్షరం body[1];
      size_t పంపిన;

      ప్రవాహం->send_closed = TRUE;
      body[0] = 'X';
      ఫలితం = cf_quiche_send(cf, దత్తాంశం, body, 0, TRUE, &పంపిన);
      CURL_TRC_CF(దత్తాంశం, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  ప్రవాహం->id, (పూర్ణాంకం)ఫలితం, పంపిన);
    }
    ఆపు;
  }
  స్థితి CF_CTRL_CONN_INFO_UPDATE:
    ఒకవేళ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_చూసిన = 30;
      Curl_conn_సమితి_multiplex(cf->conn);
    }
    ఆపు;
  స్థితి CF_CTRL_REPORT_STATS:
    ఒకవేళ(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrs_సమయం_Was(దత్తాంశం, TIMER_CONNECT, ctx->q.మొదటి_byte_at);
      Curl_pgrs_సమయం_Was(దత్తాంశం, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_quiche_ctx_open(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  పూర్ణాంకం rv;
  CURLcode ఫలితం;
  స్థిరరాశి నిర్మాణం Curl_sockaddr_ex *sockaddr;
  స్థిర స్థిరరాశి నిర్మాణం alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  ఫలితం = Curl_vquic_ctx_init(దత్తాంశం, &ctx->q);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_సంస్కరణ);
  ఒకవేళ(!ctx->cfg) {
    failf(దత్తాంశం, "cannot create quiche config");
    తిరిగి CURLE_FAILED_INIT;
  }
  quiche_config_enable_pacing(ctx->cfg, FALSE);
  quiche_config_సమితి_initial_max_దత్తాంశం(ctx->cfg, (1 * 1024 * 1024)
    /* (QUIC_MAX_STREAMS/2) * H3_STREAM_WINDOW_SIZE */);
  quiche_config_సమితి_initial_max_streams_bidi(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_సమితి_initial_max_streams_uni(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_సమితి_initial_max_ప్రవాహం_దత్తాంశం_bidi_local(ctx->cfg,
    H3_ప్రవాహం_కిటికీ_కొలత);
  quiche_config_సమితి_initial_max_ప్రవాహం_దత్తాంశం_bidi_remote(ctx->cfg,
    H3_ప్రవాహం_కిటికీ_కొలత);
  quiche_config_సమితి_initial_max_ప్రవాహం_దత్తాంశం_uni(ctx->cfg,
    H3_ప్రవాహం_కిటికీ_కొలత);
  quiche_config_సమితి_disable_చురుకైన_migration(ctx->cfg, TRUE);

  quiche_config_సమితి_max_connection_కిటికీ(ctx->cfg,
    10 * QUIC_MAX_STREAMS * H3_ప్రవాహం_కిటికీ_కొలత);
  quiche_config_సమితి_max_ప్రవాహం_కిటికీ(ctx->cfg, 10 * H3_ప్రవాహం_కిటికీ_కొలత);
  quiche_config_సమితి_application_protos(ctx->cfg,
                      (uint8_t *)CURL_UNCONST(QUICHE_H3_APPLICATION_PROTOCOL),
                                 CURL_CSTRLEN(QUICHE_H3_APPLICATION_PROTOCOL));

  ఫలితం = Curl_vquic_tls_init(&ctx->tls, cf, దత్తాంశం, &ctx->ssl_peer,
                               &ALPN_SPEC_H3, ఖాళీ, ఖాళీ, cf, ఖాళీ);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = Curl_rand(దత్తాంశం, ctx->scid, పరిమాణం(ctx->scid));
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(Curl_cf_socket_peek(cf->తదుపరి, దత్తాంశం, &ctx->q.sockfd, &sockaddr, ఖాళీ))
    తిరిగి CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = పరిమాణం(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (నిర్మాణం sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  ఒకవేళ(rv == -1)
    తిరిగి CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((స్థిరరాశి uint8_t *)ctx->scid,
                                        పరిమాణం(ctx->scid), ఖాళీ, 0,
                                        (నిర్మాణం sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  ఒకవేళ(!ctx->qconn) {
    failf(దత్తాంశం, "cannot create quiche connection");
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  /* Known to not work on Windows */
#ఒకవేళ !defined(_WIN32) && defined(HAVE_QUICHE_CONN_సమితి_QLOG_FD)
  {
    పూర్ణాంకం qfd;
    (శూన్య)Curl_qlogdir(దత్తాంశం, ctx->scid, పరిమాణం(ctx->scid), &qfd);
    ఒకవేళ(qfd != -1)
      quiche_conn_సమితి_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#అయితే_ముగింపు

  ఫలితం = cf_flush_egress(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  {
    గుర్తులేని అక్షరం alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    గుర్తులేని alpn_len, స్థానభ్రంశం = 0;

    /* Replace each ALPN length prefix by a comma. */
    అయితే(స్థానభ్రంశం < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[స్థానభ్రంశం];
      alpn_protocols[స్థానభ్రంశం] = ',';
      స్థానభ్రంశం += 1 + alpn_len;
    }

    CURL_TRC_CF(దత్తాంశం, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_quiche_verify_peer(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  తిరిగి Curl_vquic_tls_verify_peer(&ctx->tls, cf, దత్తాంశం, &ctx->ssl_peer);
}

స్థిర CURLcode cf_quiche_connect(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  bool *పూర్తి)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  /* Connect the UDP filter first */
  ఒకవేళ(!cf->తదుపరి->connected) {
    ఫలితం = Curl_conn_cf_connect(cf->తదుపరి, దత్తాంశం, పూర్తి);
    ఒకవేళ(ఫలితం || !*పూర్తి)
      తిరిగి ఫలితం;
  }

  *పూర్తి = FALSE;
  ఒకవేళ(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(దత్తాంశం, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(దత్తాంశం, cf, "need HTTPS-RR, delaying connect");
    తిరిగి CURLE_OK;
  }

  Curl_vquic_ctx_update_సమయం(&ctx->q, Curl_pgrs_now(దత్తాంశం));

  ఒకవేళ(!ctx->qconn) {
    ఫలితం = cf_quiche_ctx_open(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ctx->started_at = ctx->q.చివరి_op;
    ఫలితం = cf_flush_egress(cf, దత్తాంశం);
    /* we do not expect to be able to recv anything yet */
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = cf_process_ingress(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = cf_flush_egress(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.చివరి_op;
    CURL_TRC_CF(దత్తాంశం, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    ఫలితం = cf_quiche_verify_peer(cf, దత్తాంశం);
    ఒకవేళ(!ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      ఒకవేళ(!ctx->h3config) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_సమితి_max_క్షేత్రం_section_కొలత(ctx->h3config,
                                                  MAX_HTTP_RESP_శీర్షిక_కొలత);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      ఒకవేళ(!ctx->h3c) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
      cf->connected = TRUE;
      *పూర్తి = TRUE;
    }
  }
  లేకపోతే ఒకవేళ(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
  }

బహిర్గతం:
#నిర్వచితమైతే CURLVERBOSE
  ఒకవేళ(ఫలితం && ఫలితం != CURLE_AGAIN) {
    నిర్మాణం ip_quadruple ip;

    ఒకవేళ(!Curl_cf_socket_peek(cf->తదుపరి, దత్తాంశం, ఖాళీ, ఖాళీ, &ip))
      infof(దత్తాంశం, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_పోర్టు, curl_easy_strerror(ఫలితం));
    లేకపోతే
      infof(దత్తాంశం, "connect failed: %s", curl_easy_strerror(ఫలితం));
  }
#అయితే_ముగింపు
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_quiche_shutdown(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(cf->shutdown || !ctx || !ctx->qconn) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  *పూర్తి = FALSE;
  ఒకవేళ(!ctx->shutdown_started) {
    పూర్ణాంకం err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_సమయం(&ctx->q, Curl_pgrs_now(దత్తాంశం));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, ఖాళీ, 0);
    ఒకవేళ(err) {
      CURL_TRC_CF(దత్తాంశం, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->q.sendbuf)) {
    CURL_TRC_CF(దత్తాంశం, cf, "shutdown, flushing sendbuf");
    ఫలితం = cf_flush_egress(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(Curl_bufq_is_ఖాళీలు(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(దత్తాంశం, cf, "shutdown completely sent off, done");
    *పూర్తి = TRUE;
  }
  లేకపోతే {
    CURL_TRC_CF(దత్తాంశం, cf, "shutdown sending blocked");
  }

బహిర్గతం:
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_quiche_destroy(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  (శూన్య)దత్తాంశం;
  ఒకవేళ(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = ఖాళీ;
  }
}

స్థిర CURLcode cf_quiche_query(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;

  ఎంపిక(query) {
  స్థితి CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    ఒకవేళ(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_ఎడమ_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > గరిష్ఠ_పూర్ణాంకం) ? గరిష్ఠ_పూర్ణాంకం : (పూర్ణాంకం)max_streams;
    CURL_TRC_CF(దత్తాంశం, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    తిరిగి CURLE_OK;
  }
  స్థితి CF_QUERY_CONNECT_జవాబు_MS:
    ఒకవేళ(ctx->q.got_మొదటి_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.మొదటి_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < గరిష్ఠ_పూర్ణాంకం) ? (పూర్ణాంకం)ms : గరిష్ఠ_పూర్ణాంకం;
    }
    లేకపోతే
      *pres1 = -1;
    తిరిగి CURLE_OK;
  స్థితి CF_QUERY_HTTP_సంస్కరణ:
    *pres1 = 30;
    తిరిగి CURLE_OK;
  స్థితి CF_QUERY_SSL_INFO:
  స్థితి CF_QUERY_SSL_CTX_INFO: {
    నిర్మాణం curl_tlssessioninfo *info = pres2;
    ఒకవేళ(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      తిరిగి CURLE_OK;
    ఆపు;
  }
  స్థితి CF_QUERY_ALPN_NEGOTIATED: {
    స్థిరరాశి అక్షరం **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ఖాళీ;
    తిరిగి CURLE_OK;
  }
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->query(cf->తదుపరి, దత్తాంశం, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

స్థిర bool cf_quiche_conn_is_alive(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    bool *ఆదానం_pending)
{
  నిర్మాణం cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ఆదానం_pending = FALSE;
  ఒకవేళ(!ctx->qconn)
    తిరిగి FALSE;

  ఒకవేళ(quiche_conn_is_closed(ctx->qconn)) {
    ఒకవేళ(quiche_conn_is_timed_బహిర్గతం(ctx->qconn))
      CURL_TRC_CF(దత్తాంశం, cf, "connection was closed due to idle timeout");
    లేకపోతే
      CURL_TRC_CF(దత్తాంశం, cf, "connection is closed");
    తిరిగి FALSE;
  }

  ఒకవేళ(!cf->తదుపరి || !cf->తదుపరి->cft->is_alive(cf->తదుపరి, దత్తాంశం, ఆదానం_pending))
    తిరిగి FALSE;

  ఒకవేళ(*ఆదానం_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *ఆదానం_pending = FALSE;
    ఒకవేళ(cf_process_ingress(cf, దత్తాంశం))
      alive = FALSE;
    లేకపోతే {
      alive = TRUE;
    }
  }

  తిరిగి alive;
}

నిర్మాణం Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_quiche_destroy,
  cf_quiche_connect,
  cf_quiche_shutdown,
  cf_quiche_adjust_pollset,
  Curl_cf_def_దత్తాంశం_pending,
  cf_quiche_send,
  cf_quiche_recv,
  cf_quiche_cntrl,
  cf_quiche_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_quiche_query,
};

CURLcode Curl_cf_quiche_create(నిర్మాణం Curl_cfilter **pcf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_peer *origin,
                               నిర్మాణం Curl_peer *peer,
                               నిర్మాణం connectdata *conn,
                               నిర్మాణం Curl_sockaddr_ex *addr)
{
  నిర్మాణం cf_quiche_ctx *ctx = ఖాళీ;
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  CURLcode ఫలితం;

  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  ఫలితం = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  cf->conn = conn;

  ఫలితం = Curl_cf_udp_create(&cf->తదుపరి, దత్తాంశం, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ఖాళీ, TRNSPRT_QUIC);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  cf->తదుపరి->conn = cf->conn;
  cf->తదుపరి->sockindex = cf->sockindex;

బహిర్గతం:
  *pcf = (!ఫలితం) ? cf : ఖాళీ;
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(cf)
      Curl_conn_cf_discard_chain(&cf, దత్తాంశం);
    లేకపోతే ఒకవేళ(ctx)
      cf_quiche_ctx_free(ctx);
  }

  తిరిగి ఫలితం;
}

CURLcode Curl_cf_quiche_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                     నిర్మాణం Curl_peer *origin,
                                     నిర్మాణం Curl_peer *peer,
                                     నిర్మాణం Curl_cfilter **pcf)
{
  నిర్మాణం cf_quiche_ctx *ctx = ఖాళీ;
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  CURLcode ఫలితం;

  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  ఫలితం = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  Curl_conn_cf_చొప్పించు_after(cf_at, cf);

బహిర్గతం:
  ఒకవేళ(ఫలితం) {
    curlx_safefree(cf);
    ఒకవేళ(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు
