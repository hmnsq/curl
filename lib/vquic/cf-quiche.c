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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
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
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_H3_NO_ERROR  0x0100

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_PKT_ಅವಧಿ          10

#ವ್ಯಾಖ್ಯಾನಿಸು QUIC_MAX_STREAMS       100

#ವ್ಯಾಖ್ಯಾನಿಸು H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ  (1024 * 128)
#ವ್ಯಾಖ್ಯಾನಿಸು H3_ಹರಿವು_CHUNK_ಗಾತ್ರ   (1024 * 16)
/* Receive and Send max number of chunks follows from the
 * chunk size and window size */
#ವ್ಯಾಖ್ಯಾನಿಸು H3_ಹರಿವು_RECV_CHUNKS \
  (H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ / H3_ಹರಿವು_CHUNK_ಗಾತ್ರ)

/*
 * Store quiche version info in this buffer.
 */
ಶೂನ್ಯ Curl_quiche_ver(ಅಕ್ಷರ *p, size_t len)
{
  (ಶೂನ್ಯ)curl_msnprintf(p, len, "quiche/%s", quiche_ಆವೃತ್ತಿ());
}

ರಚನೆ cf_quiche_ctx {
  ರಚನೆ cf_quic_ctx q;
  ರಚನೆ ssl_peer ssl_peer;
  ರಚನೆ curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  ರಚನೆ curltime started_at;        /* time the current attempt started */
  ರಚನೆ curltime handshake_at;      /* time connect handshake finished */
  ರಚನೆ u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  ರಚನೆ dynbuf h1hdr;               /* temp buffer for header construction */
  ರಚನೆ bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t ದತ್ತಾಂಶ_recvd;
  ಬಿಟ್(initialized);
  ಬಿಟ್(goaway);                       /* got GOAWAY from server */
  ಬಿಟ್(x509_store_ಸಿದ್ಧತೆ);             /* if x509 store has been set up */
  ಬಿಟ್(shutdown_started);             /* queued shutdown packets */
  ಬಿಟ್(stats_reported);               /* connect statistics reported */
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_QUICHE
/* initialize debug log callback only once */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ debug_log_init = 0;
ಸ್ಥಿರ ಶೂನ್ಯ quiche_debug_log(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, ಶೂನ್ಯ *argp)
{
  (ಶೂನ್ಯ)argp;
  curl_mfprintf(stderr, "%s\n", ಸಾಲು);
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ h3_ಹರಿವು_ಗಣ_free(uint32_t id, ಶೂನ್ಯ *ಹರಿವು);

ಸ್ಥಿರ CURLcode cf_quiche_ctx_init(ರಚನೆ cf_quiche_ctx *ctx,
                                   ರಚನೆ Curl_peer *origin,
                                   ರಚನೆ Curl_peer *peer,
                                   ರಚನೆ ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_QUICHE
  ಆದರೆ(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, ಖಾಲಿ);
    debug_log_init = 1;
  }
#ಆದರೆ_ಅಂತ್ಯ
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);
  Curl_u32_ptrset_init(&ctx->streams, h3_ಹರಿವು_ಗಣ_free);
  Curl_bufq_init2(&ctx->writebuf, H3_ಹರಿವು_CHUNK_ಗಾತ್ರ, H3_ಹರಿವು_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_ಎಲ್ಲೆ);
  ctx->ದತ್ತಾಂಶ_recvd = 0;
  ctx->initialized = TRUE;
  ಹಿಂತಿರುಗಿ Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_ctx_free(ರಚನೆ cf_quiche_ctx *ctx)
{
  ಆದರೆ(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = ಖಾಲಿ;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_ctx_close(ರಚನೆ cf_quiche_ctx *ctx)
{
  ಆದರೆ(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = ಖಾಲಿ;
  }
  ಆದರೆ(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = ಖಾಲಿ;
  }
  ಆದರೆ(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = ಖಾಲಿ;
  }
  ಆದರೆ(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = ಖಾಲಿ;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

ಸ್ಥಿರ CURLcode cf_flush_egress(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/**
 * All about the H3 internals of a stream
 */
ರಚನೆ h3_ಹರಿವು_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  ರಚನೆ h1_req_parser h1; /* h1 request parsing */
  uint64_t error3;         /* HTTP/3 stream error code */
  int32_t ಸ್ಥಿತಿ_ಸಂಕೇತ;     /* HTTP status code */
  CURLcode xfer_ಫಲಿತಾಂಶ;    /* result from cf_quiche_write_(hd/body) */
  ಬಿಟ್(opened);             /* TRUE after stream has been opened */
  ಬಿಟ್(closed);             /* TRUE on stream close */
  ಬಿಟ್(reset);              /* TRUE on stream reset */
  ಬಿಟ್(send_closed);        /* stream is locally closed */
  ಬಿಟ್(resp_hds_complete);  /* final response has been received */
  ಬಿಟ್(resp_got_ಶೀರ್ಷಿಕೆ);    /* TRUE when h3 stream has recvd some HEADER */
  ಬಿಟ್(quic_flow_blocked);  /* stream is blocked by QUIC flow control */
};

ಸ್ಥಿರ ಶೂನ್ಯ h3_ಹರಿವು_ctx_free(ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು)
{
  Curl_h1_req_parse_free(&ಹರಿವು->h1);
  curlx_free(ಹರಿವು);
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_ಹರಿವು_ಗಣ_free(uint32_t id, ಶೂನ್ಯ *ಹರಿವು)
{
  (ಶೂನ್ಯ)id;
  DEBUGASSERT(ಹರಿವು);
  h3_ಹರಿವು_ctx_free((ರಚನೆ h3_ಹರಿವು_ctx *)ಹರಿವು);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ bool cf_quiche_svisit(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *sdata,
                              ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು,
                              ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);

ರಚನೆ cf_quiche_ಭೇಟಿ_ctx {
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ Curl_multi *multi;
  cf_quiche_svisit *cb;
  ಶೂನ್ಯ *user_ದತ್ತಾಂಶ;
};

ಸ್ಥಿರ bool cf_quiche_ಹರಿವು_do(uint32_t mid, ಶೂನ್ಯ *val, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_quiche_ಭೇಟಿ_ctx *vctx = user_ದತ್ತಾಂಶ;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = val;
  ರಚನೆ Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  ಆದರೆ(sdata)
    ಹಿಂತಿರುಗಿ vctx->cb(vctx->cf, sdata, ಹರಿವು, vctx->user_ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_for_all_streams(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ರಚನೆ cf_quiche_ಭೇಟಿ_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_ದತ್ತಾಂಶ = user_ದತ್ತಾಂಶ;
  Curl_u32_ptrset_ಭೇಟಿ(&ctx->streams, cf_quiche_ಹರಿವು_do, &vctx);
}

ಸ್ಥಿರ bool cf_quiche_do_resume(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *sdata,
                                ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು,
                                ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;
  ಆದರೆ(ಹರಿವು->quic_flow_blocked) {
    ಹರಿವು->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", ಹರಿವು->id);
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ bool cf_quiche_do_expire(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *sdata,
                                ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು,
                                ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  ಹರಿವು->xfer_ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ CURLcode h3_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);

  ಆದರೆ(ಹರಿವು)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಹರಿವು = curlx_calloc(1, ಗಾತ್ರ(*ಹರಿವು));
  ಆದರೆ(!ಹರಿವು)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಹರಿವು->id = -1;
  Curl_h1_req_parse_init(&ಹರಿವು->h1, H1_PARSE_DEFAULT_MAX_ಸಾಲು_LEN);

  ಆದರೆ(Curl_u32_ptrset_ಗಣ(&ctx->streams, ದತ್ತಾಂಶ->mid, ಹರಿವು)) {
    h3_ಹರಿವು_ctx_free(ಹರಿವು);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_ಹರಿವು_close(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(ctx->qconn && !ಹರಿವು->closed) {
    quiche_conn_ಹರಿವು_shutdown(ctx->qconn, ಹರಿವು->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    ಆದರೆ(!ಹರಿವು->send_closed) {
      quiche_conn_ಹರಿವು_shutdown(ctx->qconn, ಹರಿವು->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      ಹರಿವು->send_closed = TRUE;
    }
    ಹರಿವು->closed = TRUE;
    ಫಲಿತಾಂಶ = cf_flush_egress(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ h3_ದತ್ತಾಂಶ_ಮುಗಿದ(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);

  (ಶೂನ್ಯ)cf;
  ಆದರೆ(ಹರಿವು) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] easy handle is done", ಹರಿವು->id);
    cf_quiche_ಹರಿವು_close(cf, ದತ್ತಾಂಶ, ಹರಿವು);
    Curl_u32_ptrset_unset(&ctx->streams, ದತ್ತಾಂಶ->mid);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_expire_conn_closed(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  DEBUGASSERT(ದತ್ತಾಂಶ->multi);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, ದತ್ತಾಂಶ->multi, cf_quiche_do_expire, ಖಾಲಿ);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_write_hd(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  ಆದರೆ(!ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
    ಹರಿವು->xfer_ಫಲಿತಾಂಶ = Curl_xfer_write_resp_hd(ದತ್ತಾಂಶ, buf, blen, eos);
    ಆದರೆ(ಹರಿವು->xfer_ಫಲಿತಾಂಶ)
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಹರಿವು->xfer_ಫಲಿತಾಂಶ,
                  blen);
  }
}

ರಚನೆ cb_ctx {
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು;
};

ಸ್ಥಿರ bool is_valid_h3_ಶೀರ್ಷಿಕೆ(ಸ್ಥಿರಾಂಕ uint8_t *hdr, size_t hlen)
{
  ಆಗ(hlen--) {
    ಆಯ್ಕೆ(*hdr++) {
    ಸ್ಥಿತಿ '\n':
    ಸ್ಥಿತಿ '\r':
    ಸ್ಥಿತಿ '\0':
      ಹಿಂತಿರುಗಿ FALSE;
    }
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cb_each_ಶೀರ್ಷಿಕೆ(uint8_t *name, size_t name_len,
                          uint8_t *ಮೌಲ್ಯ, size_t ಮೌಲ್ಯ_len,
                          ಶೂನ್ಯ *argp)
{
  ರಚನೆ cb_ctx *x = argp;
  ರಚನೆ Curl_cfilter *cf = x->cf;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = x->ದತ್ತಾಂಶ;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = x->ಹರಿವು;
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!ಹರಿವು || ಹರಿವು->xfer_ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ 1; /* abort iteration */

  ಆದರೆ((name_len == 7) && !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(HTTP_PSEUDO_ಸ್ಥಿತಿ, (ಅಕ್ಷರ *)name, 7) &&
     is_valid_h3_ಶೀರ್ಷಿಕೆ(ಮೌಲ್ಯ, ಮೌಲ್ಯ_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    ಫಲಿತಾಂಶ = Curl_http_decode_ಸ್ಥಿತಿ(&ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ,
                                     (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೌಲ್ಯ, ಮೌಲ್ಯ_len);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->h1hdr, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೌಲ್ಯ, ಮೌಲ್ಯ_len);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      cf_quiche_write_hd(cf, ದತ್ತಾಂಶ, ಹರಿವು, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] status: %s",
                ಹರಿವು->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(is_valid_h3_ಶೀರ್ಷಿಕೆ(ಮೌಲ್ಯ, ಮೌಲ್ಯ_len) &&
       is_valid_h3_ಶೀರ್ಷಿಕೆ(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)name_len, name,
                  (ಪೂರ್ಣಾಂಕ)ಮೌಲ್ಯ_len, ಮೌಲ್ಯ);
      curlx_dyn_reset(&ctx->h1hdr);
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->h1hdr, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)name, name_len);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->h1hdr, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೌಲ್ಯ, ಮೌಲ್ಯ_len);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        cf_quiche_write_hd(cf, ದತ್ತಾಂಶ, ಹರಿವು, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      CURL_TRC_CF(x->ದತ್ತಾಂಶ, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  ಹರಿವು->id, ಮೌಲ್ಯ_len + name_len);
  }

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    CURL_TRC_CF(x->ದತ್ತಾಂಶ, x->cf, "[%" PRIu64 "] on header error %d",
                ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಆದರೆ(!ಹರಿವು->xfer_ಫಲಿತಾಂಶ)
      ಹರಿವು->xfer_ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ ಹರಿವು->xfer_ಫಲಿತಾಂಶ ? 1 : 0;
}

ಸ್ಥಿರ CURLcode ಹರಿವು_resp_read(ಶೂನ್ಯ *reader_ctx,
                                 ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t len,
                                 size_t *pnread)
{
  ರಚನೆ cb_ctx *x = reader_ctx;
  ರಚನೆ cf_quiche_ctx *ctx = x->cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, x->ದತ್ತಾಂಶ);
  ssize_t nread;

  *pnread = 0;
  ಆದರೆ(!ಹರಿವು)
    ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, ಹರಿವು->id, buf, len);
  ಆದರೆ(!curlx_sztouz(nread, pnread))
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_flush_body(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ಸ್ಥಿರಾಂಕ uint8_t *buf;
  size_t blen;

  ಆಗ(ಹರಿವು && !ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
    ಆದರೆ(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      ಹರಿವು->xfer_ಫಲಿತಾಂಶ = Curl_xfer_write_resp(
        ದತ್ತಾಂಶ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      ಆದರೆ(ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಹರಿವು->xfer_ಫಲಿತಾಂಶ, blen);
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ನಿಲ್ಲಿಸು;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_recv_body(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  ರಚನೆ cb_ctx cb_ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!ಹರಿವು)
    ಹಿಂತಿರುಗಿ;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
  cb_ctx.ಹರಿವು = ಹರಿವು;
  Curl_bufq_reset(&ctx->writebuf);
  ಆಗ(!ಫಲಿತಾಂಶ) {
    ಫಲಿತಾಂಶ = Curl_bufq_slurp(&ctx->writebuf,
                             ಹರಿವು_resp_read, &cb_ctx, &nread);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      cf_quiche_flush_body(cf, ದತ್ತಾಂಶ, ಹರಿವು);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      ನಿಲ್ಲಿಸು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] recv_body error %d",
                  ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      failf(ದತ್ತಾಂಶ, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಹರಿವು->closed = TRUE;
      ಹರಿವು->reset = TRUE;
      ಹರಿವು->send_closed = TRUE;
      ಆದರೆ(!ಹರಿವು->xfer_ಫಲಿತಾಂಶ)
        ಹರಿವು->xfer_ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
    }
  }
  cf_quiche_flush_body(cf, ದತ್ತಾಂಶ, ಹರಿವು);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_process_ev(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು,
                                 quiche_h3_event *ev)
{
  ಎಣಿಕೆ quiche_h3_event_type type;

  ಆದರೆ(!ಹರಿವು)
    ಹಿಂತಿರುಗಿ;

  type = quiche_h3_event_type(ev);

  ಆಯ್ಕೆ(type) {
  ಸ್ಥಿತಿ QUICHE_H3_EVENT_ಶೀರ್ಷಿಕೆಗಳು: {
    ರಚನೆ cb_ctx cb_ctx;
    ಹರಿವು->resp_got_ಶೀರ್ಷಿಕೆ = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
    cb_ctx.ಹರಿವು = ಹರಿವು;
    quiche_h3_event_for_each_ಶೀರ್ಷಿಕೆ(ev, cb_each_ಶೀರ್ಷಿಕೆ, &cb_ctx);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] <- [HEADERS]", ಹರಿವು->id);
    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  }
  ಸ್ಥಿತಿ QUICHE_H3_EVENT_ದತ್ತಾಂಶ:
    ಆದರೆ(!ಹರಿವು->resp_hds_complete) {
      ಹರಿವು->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, ದತ್ತಾಂಶ, ಹರಿವು, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, ದತ್ತಾಂಶ, ಹರಿವು);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] <- [DATA]", ಹರಿವು->id);
    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] RESET", ಹರಿವು->id);
    ಹರಿವು->closed = TRUE;
    ಹರಿವು->reset = TRUE;
    ಹರಿವು->send_closed = TRUE;
    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ QUICHE_H3_EVENT_ಪೂರ್ಣಗೊಂಡ:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] CLOSED", ಹರಿವು->id);
    ಆದರೆ(!ಹರಿವು->resp_hds_complete) {
      ಹರಿವು->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, ದತ್ತಾಂಶ, ಹರಿವು, "\r\n", 2, TRUE);
    }
    ಹರಿವು->closed = TRUE;
    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] <- [GOAWAY]", ಹರಿವು->id);
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] recv, unhandled event %d",
                ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)type);
    ನಿಲ್ಲಿಸು;
  }
}

ರಚನೆ cf_quiche_disp_ctx {
  uint64_t ಹರಿವು_id;
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ Curl_multi *multi;
  quiche_h3_event *ev;
};

ಸ್ಥಿರ bool cf_quiche_disp_event(uint32_t mid, ಶೂನ್ಯ *val, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_quiche_disp_ctx *dctx = user_ದತ್ತಾಂಶ;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = val;

  ಆದರೆ(ಹರಿವು->id == dctx->ಹರಿವು_id) {
    ರಚನೆ Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    ಆದರೆ(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, ಹರಿವು, dctx->ev);
    ಹಿಂತಿರುಗಿ FALSE; /* stop iterating */
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ CURLcode cf_poll_events(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = ಖಾಲಿ;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  ಆಗ(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    ಆದರೆ(rv == QUICHE_H3_ERR_ಮುಗಿದ) {
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rv < 0) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "error poll: %" PRId64, rv);
      ಹಿಂತಿರುಗಿ CURLE_HTTP3;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
      ಆದರೆ(ಹರಿವು && ಹರಿವು->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, ದತ್ತಾಂಶ, ಹರಿವು, ev);
        quiche_h3_event_free(ev);
        ಆದರೆ(ಹರಿವು->xfer_ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಹರಿವು->xfer_ಫಲಿತಾಂಶ;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        ರಚನೆ cf_quiche_disp_ctx dctx;
        dctx.ಹರಿವು_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = ದತ್ತಾಂಶ->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_ಭೇಟಿ(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ರಚನೆ recv_ctx {
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  ಪೂರ್ಣಾಂಕ pkts;
};

ಸ್ಥಿರ CURLcode cf_quiche_recv_pkts(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t buflen,
                                    size_t gso_ಗಾತ್ರ,
                                    ರಚನೆ sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    ಶೂನ್ಯ *userp)
{
  ರಚನೆ recv_ctx *r = userp;
  ರಚನೆ cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, ಸ್ಥಾನಾಂತರ, nread;
  ssize_t rv;

  (ಶೂನ್ಯ)ecn;

  recv_info.ಗಮ್ಯ = (ರಚನೆ sockaddr *)&ctx->q.local_addr;
  recv_info.ಗಮ್ಯ_len = ctx->q.local_addrlen;
  recv_info.from = (ರಚನೆ sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  ಗಾಗಿ(ಸ್ಥಾನಾಂತರ = 0; ಸ್ಥಾನಾಂತರ < buflen; ಸ್ಥಾನಾಂತರ += gso_ಗಾತ್ರ) {
    pktlen = ((ಸ್ಥಾನಾಂತರ + gso_ಗಾತ್ರ) <= buflen) ? gso_ಗಾತ್ರ : (buflen - ಸ್ಥಾನಾಂತರ);
    rv = quiche_conn_recv(ctx->qconn,
                          (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)CURL_UNCONST(buf + ಸ್ಥಾನಾಂತರ),
                          pktlen, &recv_info);
    ಆದರೆ(!curlx_sztouz(rv, &nread)) {
      ಆದರೆ(QUICHE_ERR_ಮುಗಿದ == rv) {
        ಆದರೆ(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->ದತ್ತಾಂಶ, r->cf, "ingress, connection is draining");
          ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
        }
        ಆದರೆ(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->ದತ್ತಾಂಶ, r->cf, "ingress, connection is closed");
          ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->ದತ್ತಾಂಶ, r->cf, "ingress, quiche is DONE");
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(QUICHE_ERR_TLS_FAIL == rv) {
        ದೀರ್ಘ verify_ok = SSL_get_verify_ಫಲಿತಾಂಶ(ctx->tls.ossl.ssl);
        ಆದರೆ(verify_ok != X509_V_OK) {
          failf(r->ದತ್ತಾಂಶ, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->ದತ್ತಾಂಶ, "ingress, quiche reports TLS fail");
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        failf(r->ದತ್ತಾಂಶ, "quiche reports error %zd on receive", rv);
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread < pktlen) {
      CURL_TRC_CF(r->ದತ್ತಾಂಶ, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_process_ingress(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ರಚನೆ recv_ctx rctx;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(ctx->qconn);
  ಫಲಿತಾಂಶ = Curl_vquic_tls_before_recv(&ctx->tls, cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  rctx.cf = cf;
  rctx.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
  rctx.pkts = 0;

  ಫಲಿತಾಂಶ = Curl_vquic_recv_packets(cf, ದತ್ತಾಂಶ, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(ದತ್ತಾಂಶ->multi);
    cf_quiche_for_all_streams(cf, ದತ್ತಾಂಶ->multi, cf_quiche_do_resume, ಖಾಲಿ);
  }
  ಹಿಂತಿರುಗಿ cf_poll_events(cf, ದತ್ತಾಂಶ);
}

ರಚನೆ read_ctx {
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  quiche_send_info send_info;
};

ಸ್ಥಿರ CURLcode read_pkt_ಗಮ್ಯ_send(ಶೂನ್ಯ *userp,
                                 ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t buflen,
                                 size_t *pnread)
{
  ರಚನೆ read_ctx *x = userp;
  ರಚನೆ cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  ಆದರೆ(rv == QUICHE_ERR_ಮುಗಿದ)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆದರೆ(!curlx_sztouz(rv, pnread)) {
    failf(x->ದತ್ತಾಂಶ, "quiche_conn_send returned %zd", rv);
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
ಸ್ಥಿರ CURLcode cf_flush_egress(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode ಫಲಿತಾಂಶ;
  int64_t expiry_ns;
  int64_t timeout_ns;
  ರಚನೆ read_ctx readx;
  size_t pkt_ಎಣಿಕೆ, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  ಆದರೆ(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    ಆದರೆ(quiche_conn_is_closed(ctx->qconn)) {
      ಆದರೆ(quiche_conn_is_timed_ಹೊರಹೋಗು(ctx->qconn))
        failf(ದತ್ತಾಂಶ, "connection closed by idle timeout");
      ಇಲ್ಲದಿದ್ದರೆ
        failf(ದತ್ತಾಂಶ, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, ದತ್ತಾಂಶ);
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
    }
  }

  ಫಲಿತಾಂಶ = Curl_vquic_flush(cf, ದತ್ತಾಂಶ, &ctx->q);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      Curl_expire(ದತ್ತಾಂಶ, 1, EXPIRE_QUIC);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  readx.cf = cf;
  readx.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
  memset(&readx.send_info, 0, ಗಾತ್ರ(readx.send_info));
  pkt_ಎಣಿಕೆ = 0;
  gsolen = quiche_conn_max_send_udp_payload_ಗಾತ್ರ(ctx->qconn);
  ಗಾಗಿ(;;) {
    /* add the next packet to send, if any, to our buffer */
    ಫಲಿತಾಂಶ = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_ಗಮ್ಯ_send, &readx, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      /* Nothing more to add, flush and leave */
      ಫಲಿತಾಂಶ = Curl_vquic_send(cf, ದತ್ತಾಂಶ, &ctx->q, gsolen);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
          Curl_expire(ದತ್ತಾಂಶ, 1, EXPIRE_QUIC);
          ಹಿಂತಿರುಗಿ CURLE_OK;
        }
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ++pkt_ಎಣಿಕೆ;
    ಆದರೆ(nread < gsolen || pkt_ಎಣಿಕೆ >= MAX_PKT_ಅವಧಿ) {
      ಫಲಿತಾಂಶ = Curl_vquic_send(cf, ದತ್ತಾಂಶ, &ctx->q, gsolen);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
          Curl_expire(ದತ್ತಾಂಶ, 1, EXPIRE_QUIC);
          ಹಿಂತಿರುಗಿ CURLE_OK;
        }
        ಹೋಗು ಹೊರಹೋಗು;
      }
      pkt_ಎಣಿಕೆ = 0;
    }
  }

ಹೊರಹೋಗು:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  ಆದರೆ(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(ದತ್ತಾಂಶ, (timeout_ns / 1000000), EXPIRE_QUIC);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode recv_closed_ಹರಿವು(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   size_t *pnread)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(ಹರಿವು);
  *pnread = 0;
  ಆದರೆ(ಹರಿವು->reset) {
    ಆದರೆ(ಹರಿವು->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(ದತ್ತಾಂಶ, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", ಹರಿವು->id);
      connclose(cf->conn); /* do not use this anymore */
      ದತ್ತಾಂಶ->state.refused_ಹರಿವು = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->resp_hds_complete && ದತ್ತಾಂಶ->req.no_body) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", ಹರಿವು->id, ಹರಿವು->error3,
                    Curl_vquic_h3_err_str(ಹರಿವು->error3));
        ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    failf(ದತ್ತಾಂಶ, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", ಹರಿವು->id, ಹರಿವು->error3,
          Curl_vquic_h3_err_str(ಹರಿವು->error3));
    ಫಲಿತಾಂಶ = ದತ್ತಾಂಶ->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಹರಿವು->resp_got_ಶೀರ್ಷಿಕೆ) {
    failf(ದತ್ತಾಂಶ, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          ಹರಿವು->id);
    ಫಲಿತಾಂಶ = CURLE_HTTP3;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_quiche_recv(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಅಕ್ಷರ *buf, size_t blen, size_t *pnread)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *pnread = 0;
  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)blen;
  Curl_vquic_ctx_update_ಸಮಯ(&ctx->q, Curl_pgrs_now(ದತ್ತಾಂಶ));

  ಆದರೆ(!ಹರಿವು)
    ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;

  ಫಲಿತಾಂಶ = cf_process_ingress(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_recv, error on ingress");
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
    cf_quiche_ಹರಿವು_close(cf, ದತ್ತಾಂಶ, ಹರಿವು);
    ಫಲಿತಾಂಶ = ಹರಿವು->xfer_ಫಲಿತಾಂಶ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->closed)
    ಫಲಿತಾಂಶ = recv_closed_ಹರಿವು(cf, ದತ್ತಾಂಶ, pnread);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(quiche_conn_is_draining(ctx->qconn)) {
    failf(ದತ್ತಾಂಶ, "QUIC connection is draining");
    ಫಲಿತಾಂಶ = CURLE_HTTP3;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = CURLE_AGAIN;

ಹೊರಹೋಗು:
  ಫಲಿತಾಂಶ = Curl_1st_fatal(ಫಲಿತಾಂಶ, cf_flush_egress(cf, ದತ್ತಾಂಶ));
  ಆದರೆ(*pnread > 0)
    ctx->ದತ್ತಾಂಶ_recvd += *pnread;
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, ಹರಿವು->id, blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread,
              ctx->ದತ್ತಾಂಶ_recvd);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_quiche_send_body(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು,
                                    ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, ಹರಿವು->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  ಆದರೆ(rv == QUICHE_H3_ERR_ಮುಗಿದ || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    ಆದರೆ(!quiche_conn_ಹರಿವು_writable(ctx->qconn, ಹರಿವು->id, len)) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", ಹರಿವು->id, len);
      ಹರಿವು->quic_flow_blocked = TRUE;
    }
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_ಹರಿವು_STATE) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", ಹರಿವು->id, len);
    ಹಿಂತಿರುಗಿ CURLE_HTTP3;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_ಗಾತ್ರ) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                ಹರಿವು->id, len);
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                ಹರಿವು->id, len, rv);
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(eos && (len == *pnwritten))
      ಹರಿವು->send_closed = TRUE;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                ಹರಿವು->id, len, ಹರಿವು->send_closed, *pnwritten);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
}

ಸ್ಥಿರ CURLcode h3_open_ಹರಿವು(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
  size_t nheader, i;
  int64_t rv;
  ರಚನೆ dynhds h2_ಶೀರ್ಷಿಕೆಗಳು;
  quiche_h3_ಶೀರ್ಷಿಕೆ *nva = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *pnwritten = 0;
  ಆದರೆ(!ಹರಿವು) {
    ಫಲಿತಾಂಶ = h3_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
    DEBUGASSERT(ಹರಿವು);
  }

  Curl_dynhds_init(&h2_ಶೀರ್ಷಿಕೆಗಳು, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(ಹರಿವು);

  ಫಲಿತಾಂಶ = Curl_h1_req_parse_read(&ಹರಿವು->h1, buf, blen, ಖಾಲಿ,
    !ದತ್ತಾಂಶ->state.http_ignorecustom ?
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST) : ಖಾಲಿ,
    0, pnwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಆದರೆ(!ಹರಿವು->h1.ಮುಗಿದ) {
    /* need more data */
    ಹೋಗು ಹೊರಹೋಗು;
  }
  DEBUGASSERT(ಹರಿವು->h1.req);

  ಫಲಿತಾಂಶ = Curl_http_req_ಗಮ್ಯ_h2(&h2_ಶೀರ್ಷಿಕೆಗಳು, ಹರಿವು->h1.req, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* no longer needed */
  Curl_h1_req_parse_free(&ಹರಿವು->h1);

  nheader = Curl_dynhds_ಎಣಿಕೆ(&h2_ಶೀರ್ಷಿಕೆಗಳು);
  nva = curlx_malloc(ಗಾತ್ರ(quiche_h3_ಶೀರ್ಷಿಕೆ) * nheader);
  ಆದರೆ(!nva) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಗಾಗಿ(i = 0; i < nheader; ++i) {
    ರಚನೆ dynhds_ನಮೂದು *e = Curl_dynhds_getn(&h2_ಶೀರ್ಷಿಕೆಗಳು, i);
    nva[i].name = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].ಮೌಲ್ಯ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)e->ಮೌಲ್ಯ;
    nva[i].ಮೌಲ್ಯ_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  ಆದರೆ(eos && !blen)
    ಹರಿವು->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              ಹರಿವು->send_closed);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "quiche_send_request() -> %" PRId64, rv);
  ಆದರೆ(rv < 0) {
    ಆದರೆ(QUICHE_H3_ERR_ಹರಿವು_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] blocked", ಹರಿವು->id);
      ಹರಿವು->quic_flow_blocked = TRUE;
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url), rv);
    }
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  DEBUGASSERT(!ಹರಿವು->opened);
  ಹರಿವು->id = (uint64_t)rv;
  ಹರಿವು->opened = TRUE;
  ಹರಿವು->closed = FALSE;
  ಹರಿವು->reset = FALSE;

  ಆದರೆ(Curl_trc_is_verbose(ದತ್ತಾಂಶ)) {
    infof(ದತ್ತಾಂಶ, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          ಹರಿವು->id, Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url));
    ಗಾಗಿ(i = 0; i < nheader; ++i) {
      infof(ದತ್ತಾಂಶ, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", ಹರಿವು->id,
            (ಪೂರ್ಣಾಂಕ)nva[i].name_len, nva[i].name,
            (ಪೂರ್ಣಾಂಕ)nva[i].ಮೌಲ್ಯ_len, nva[i].ಮೌಲ್ಯ);
    }
  }

  ಆದರೆ(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, ದತ್ತಾಂಶ, ಹರಿವು, buf, blen, eos, &nwritten);
    ಆದರೆ(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      ಫಲಿತಾಂಶ = r2;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

ಹೊರಹೋಗು:
  curlx_free(nva);
  Curl_dynhds_free(&h2_ಶೀರ್ಷಿಕೆಗಳು);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_quiche_send(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
  CURLcode ಫಲಿತಾಂಶ;

  *pnwritten = 0;
  Curl_vquic_ctx_update_ಸಮಯ(&ctx->q, Curl_pgrs_now(ದತ್ತಾಂಶ));

  ಫಲಿತಾಂಶ = cf_process_ingress(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(!ಹರಿವು || !ಹರಿವು->opened) {
    ಫಲಿತಾಂಶ = h3_open_ಹರಿವು(cf, ದತ್ತಾಂಶ, buf, len, eos, pnwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
    cf_quiche_ಹರಿವು_close(cf, ದತ್ತಾಂಶ, ಹರಿವು);
    ಫಲಿತಾಂಶ = ಹರಿವು->xfer_ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->closed) {
    ಆದರೆ(ಹರಿವು->resp_hds_complete) {
      /* sending request body on a stream that has been closed by the
       * server. If the server has send us a final response, we should
       * silently discard the send data.
       * This happens for example on redirects where the server, instead
       * of reading the full request body closed the stream after
       * sending the 30x response.
       * This is sort of a race: had the transfer loop called recv first,
       * it would see the response and stop/discard sending on its own- */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] discarding data "
                  "on closed stream with response", ಹರಿವು->id);
      ಫಲಿತಾಂಶ = CURLE_OK;
      *pnwritten = len;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", ಹರಿವು->id, len);
    ಫಲಿತಾಂಶ = CURLE_HTTP3;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = cf_quiche_send_body(cf, ದತ್ತಾಂಶ, ಹರಿವು, buf, len, eos, pnwritten);
  }

ಹೊರಹೋಗು:
  ಫಲಿತಾಂಶ = Curl_1st_fatal(ಫಲಿತಾಂಶ, cf_flush_egress(cf, ದತ್ತಾಂಶ));

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              ಹರಿವು ? ಹರಿವು->id : (uint64_t)~0, len,
              (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool ಹರಿವು_is_writable(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);

  ಹಿಂತಿರುಗಿ ಹರಿವು && (quiche_conn_ಹರಿವು_writable(
    ctx->qconn, ಹರಿವು->id, 1) > 0);
}

ಸ್ಥಿರ CURLcode cf_quiche_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!ctx->qconn)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  Curl_pollset_check(ದತ್ತಾಂಶ, ps, ctx->q.sockfd, &want_recv, &want_send);
  ಆದರೆ(want_recv || want_send) {
    ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
    bool c_exhaust, s_exhaust;

    c_exhaust = FALSE; /* Have not found any call in quiche that tells
                          us if the connection itself is blocked */
    s_exhaust = want_send && ಹರಿವು && ಹರಿವು->opened &&
                (ಹರಿವು->quic_flow_blocked || !ಹರಿವು_is_writable(cf, ದತ್ತಾಂಶ));
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->q.sendbuf);

    ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ(ದತ್ತಾಂಶ, ps, ctx->q.sockfd, want_recv, want_send);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode h3_ದತ್ತಾಂಶ_pause(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (ಶೂನ್ಯ)cf;
  ಆದರೆ(!pause) {
    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_quiche_cntrl(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  ಆಯ್ಕೆ(event) {
  ಸ್ಥಿತಿ CF_CTRL_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_ದತ್ತಾಂಶ_PAUSE:
    ಫಲಿತಾಂಶ = h3_ದತ್ತಾಂಶ_pause(cf, ದತ್ತಾಂಶ, (arg1 != 0));
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_ದತ್ತಾಂಶ_ಮುಗಿದ:
    h3_ದತ್ತಾಂಶ_ಮುಗಿದ(cf, ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_ದತ್ತಾಂಶ_ಮುಗಿದ_SEND: {
    ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು = H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
    ಆದರೆ(ಹರಿವು && !ಹರಿವು->send_closed) {
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ body[1];
      size_t ಕಳುಹಿಸಿದ;

      ಹರಿವು->send_closed = TRUE;
      body[0] = 'X';
      ಫಲಿತಾಂಶ = cf_quiche_send(cf, ದತ್ತಾಂಶ, body, 0, TRUE, &ಕಳುಹಿಸಿದ);
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, ಕಳುಹಿಸಿದ);
    }
    ನಿಲ್ಲಿಸು;
  }
  ಸ್ಥಿತಿ CF_CTRL_CONN_INFO_UPDATE:
    ಆದರೆ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_ನೋಡಿದ = 30;
      Curl_conn_ಗಣ_multiplex(cf->conn);
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_REPORT_STATS:
    ಆದರೆ(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrs_ಸಮಯ_Was(ದತ್ತಾಂಶ, TIMER_CONNECT, ctx->q.ಮೊದಲ_byte_at);
      Curl_pgrs_ಸಮಯ_Was(ದತ್ತಾಂಶ, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_quiche_ctx_open(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಂಕ rv;
  CURLcode ಫಲಿತಾಂಶ;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_sockaddr_ex *sockaddr;
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  ಫಲಿತಾಂಶ = Curl_vquic_ctx_init(ದತ್ತಾಂಶ, &ctx->q);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_ಆವೃತ್ತಿ);
  ಆದರೆ(!ctx->cfg) {
    failf(ದತ್ತಾಂಶ, "cannot create quiche config");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  quiche_config_enable_pacing(ctx->cfg, FALSE);
  quiche_config_ಗಣ_initial_max_ದತ್ತಾಂಶ(ctx->cfg, (1 * 1024 * 1024)
    /* (QUIC_MAX_STREAMS/2) * H3_STREAM_WINDOW_SIZE */);
  quiche_config_ಗಣ_initial_max_streams_bidi(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_ಗಣ_initial_max_streams_uni(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_ಗಣ_initial_max_ಹರಿವು_ದತ್ತಾಂಶ_bidi_local(ctx->cfg,
    H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ);
  quiche_config_ಗಣ_initial_max_ಹರಿವು_ದತ್ತಾಂಶ_bidi_remote(ctx->cfg,
    H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ);
  quiche_config_ಗಣ_initial_max_ಹರಿವು_ದತ್ತಾಂಶ_uni(ctx->cfg,
    H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ);
  quiche_config_ಗಣ_disable_ಸಕ್ರಿಯ_migration(ctx->cfg, TRUE);

  quiche_config_ಗಣ_max_connection_ಕಿಟಕಿ(ctx->cfg,
    10 * QUIC_MAX_STREAMS * H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ);
  quiche_config_ಗಣ_max_ಹರಿವು_ಕಿಟಕಿ(ctx->cfg, 10 * H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ);
  quiche_config_ಗಣ_application_protos(ctx->cfg,
                      (uint8_t *)CURL_UNCONST(QUICHE_H3_APPLICATION_PROTOCOL),
                                 CURL_CSTRLEN(QUICHE_H3_APPLICATION_PROTOCOL));

  ಫಲಿತಾಂಶ = Curl_vquic_tls_init(&ctx->tls, cf, ದತ್ತಾಂಶ, &ctx->ssl_peer,
                               &ALPN_SPEC_H3, ಖಾಲಿ, ಖಾಲಿ, cf, ಖಾಲಿ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_rand(ದತ್ತಾಂಶ, ctx->scid, ಗಾತ್ರ(ctx->scid));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(Curl_cf_socket_peek(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &ctx->q.sockfd, &sockaddr, ಖಾಲಿ))
    ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = ಗಾತ್ರ(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (ರಚನೆ sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  ಆದರೆ(rv == -1)
    ಹಿಂತಿರುಗಿ CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((ಸ್ಥಿರಾಂಕ uint8_t *)ctx->scid,
                                        ಗಾತ್ರ(ctx->scid), ಖಾಲಿ, 0,
                                        (ರಚನೆ sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  ಆದರೆ(!ctx->qconn) {
    failf(ದತ್ತಾಂಶ, "cannot create quiche connection");
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  /* Known to not work on Windows */
#ಆದರೆ !defined(_WIN32) && defined(HAVE_QUICHE_CONN_ಗಣ_QLOG_FD)
  {
    ಪೂರ್ಣಾಂಕ qfd;
    (ಶೂನ್ಯ)Curl_qlogdir(ದತ್ತಾಂಶ, ctx->scid, ಗಾತ್ರ(ctx->scid), &qfd);
    ಆದರೆ(qfd != -1)
      quiche_conn_ಗಣ_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಫಲಿತಾಂಶ = cf_flush_egress(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    ಚಿಹ್ನೆರಹಿತ alpn_len, ಸ್ಥಾನಾಂತರ = 0;

    /* Replace each ALPN length prefix by a comma. */
    ಆಗ(ಸ್ಥಾನಾಂತರ < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[ಸ್ಥಾನಾಂತರ];
      alpn_protocols[ಸ್ಥಾನಾಂತರ] = ',';
      ಸ್ಥಾನಾಂತರ += 1 + alpn_len;
    }

    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_quiche_verify_peer(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  ಹಿಂತಿರುಗಿ Curl_vquic_tls_verify_peer(&ctx->tls, cf, ದತ್ತಾಂಶ, &ctx->ssl_peer);
}

ಸ್ಥಿರ CURLcode cf_quiche_connect(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  bool *ಮುಗಿದ)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  /* Connect the UDP filter first */
  ಆದರೆ(!cf->ಮುಂದಿನ->connected) {
    ಫಲಿತಾಂಶ = Curl_conn_cf_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ || !*ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  *ಮುಗಿದ = FALSE;
  ಆದರೆ(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(ದತ್ತಾಂಶ, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "need HTTPS-RR, delaying connect");
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  Curl_vquic_ctx_update_ಸಮಯ(&ctx->q, Curl_pgrs_now(ದತ್ತಾಂಶ));

  ಆದರೆ(!ctx->qconn) {
    ಫಲಿತಾಂಶ = cf_quiche_ctx_open(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ctx->started_at = ctx->q.ಕೊನೆಯ_op;
    ಫಲಿತಾಂಶ = cf_flush_egress(cf, ದತ್ತಾಂಶ);
    /* we do not expect to be able to recv anything yet */
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = cf_process_ingress(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = cf_flush_egress(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.ಕೊನೆಯ_op;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    ಫಲಿತಾಂಶ = cf_quiche_verify_peer(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      ಆದರೆ(!ctx->h3config) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_ಗಣ_max_ಕ್ಷೇತ್ರ_section_ಗಾತ್ರ(ctx->h3config,
                                                  MAX_HTTP_RESP_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      ಆದರೆ(!ctx->h3c) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      cf->connected = TRUE;
      *ಮುಗಿದ = TRUE;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }

ಹೊರಹೋಗು:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆದರೆ(ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ != CURLE_AGAIN) {
    ರಚನೆ ip_quadruple ip;

    ಆದರೆ(!Curl_cf_socket_peek(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಖಾಲಿ, ಖಾಲಿ, &ip))
      infof(ದತ್ತಾಂಶ, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_ಪೋರ್ಟ್, curl_easy_strerror(ಫಲಿತಾಂಶ));
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "connect failed: %s", curl_easy_strerror(ಫಲಿತಾಂಶ));
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_quiche_shutdown(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(cf->shutdown || !ctx || !ctx->qconn) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  *ಮುಗಿದ = FALSE;
  ಆದರೆ(!ctx->shutdown_started) {
    ಪೂರ್ಣಾಂಕ err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_ಸಮಯ(&ctx->q, Curl_pgrs_now(ದತ್ತಾಂಶ));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, ಖಾಲಿ, 0);
    ಆದರೆ(err) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->q.sendbuf)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown, flushing sendbuf");
    ಫಲಿತಾಂಶ = cf_flush_egress(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown completely sent off, done");
    *ಮುಗಿದ = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown sending blocked");
  }

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_quiche_destroy(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = ಖಾಲಿ;
  }
}

ಸ್ಥಿರ CURLcode cf_quiche_query(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;

  ಆಯ್ಕೆ(query) {
  ಸ್ಥಿತಿ CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    ಆದರೆ(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_ಎಡ_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ : (ಪೂರ್ಣಾಂಕ)max_streams;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಸ್ಥಿತಿ CF_QUERY_CONNECT_ಉತ್ತರ_MS:
    ಆದರೆ(ctx->q.got_ಮೊದಲ_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.ಮೊದಲ_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? (ಪೂರ್ಣಾಂಕ)ms : ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      *pres1 = -1;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CF_QUERY_HTTP_ಆವೃತ್ತಿ:
    *pres1 = 30;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CF_QUERY_SSL_INFO:
  ಸ್ಥಿತಿ CF_QUERY_SSL_CTX_INFO: {
    ರಚನೆ curl_tlssessioninfo *info = pres2;
    ಆದರೆ(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      ಹಿಂತಿರುಗಿ CURLE_OK;
    ನಿಲ್ಲಿಸು;
  }
  ಸ್ಥಿತಿ CF_QUERY_ALPN_NEGOTIATED: {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ bool cf_quiche_conn_is_alive(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    bool *ಒಳಹರಿವು_pending)
{
  ರಚನೆ cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ಒಳಹರಿವು_pending = FALSE;
  ಆದರೆ(!ctx->qconn)
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ(quiche_conn_is_closed(ctx->qconn)) {
    ಆದರೆ(quiche_conn_is_timed_ಹೊರಹೋಗು(ctx->qconn))
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "connection was closed due to idle timeout");
    ಇಲ್ಲದಿದ್ದರೆ
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "connection is closed");
    ಹಿಂತಿರುಗಿ FALSE;
  }

  ಆದರೆ(!cf->ಮುಂದಿನ || !cf->ಮುಂದಿನ->cft->is_alive(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಒಳಹರಿವು_pending))
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ(*ಒಳಹರಿವು_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *ಒಳಹರಿವು_pending = FALSE;
    ಆದರೆ(cf_process_ingress(cf, ದತ್ತಾಂಶ))
      alive = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ {
      alive = TRUE;
    }
  }

  ಹಿಂತಿರುಗಿ alive;
}

ರಚನೆ Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_quiche_destroy,
  cf_quiche_connect,
  cf_quiche_shutdown,
  cf_quiche_adjust_pollset,
  Curl_cf_def_ದತ್ತಾಂಶ_pending,
  cf_quiche_send,
  cf_quiche_recv,
  cf_quiche_cntrl,
  cf_quiche_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_quiche_query,
};

CURLcode Curl_cf_quiche_create(ರಚನೆ Curl_cfilter **pcf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_peer *origin,
                               ರಚನೆ Curl_peer *peer,
                               ರಚನೆ connectdata *conn,
                               ರಚನೆ Curl_sockaddr_ex *addr)
{
  ರಚನೆ cf_quiche_ctx *ctx = ಖಾಲಿ;
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಫಲಿತಾಂಶ = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  cf->conn = conn;

  ಫಲಿತಾಂಶ = Curl_cf_udp_create(&cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, origin, peer, TRNSPRT_QUIC,
                              conn, addr, ಖಾಲಿ, TRNSPRT_QUIC);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  cf->ಮುಂದಿನ->conn = cf->conn;
  cf->ಮುಂದಿನ->sockindex = cf->sockindex;

ಹೊರಹೋಗು:
  *pcf = (!ಫಲಿತಾಂಶ) ? cf : ಖಾಲಿ;
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(cf)
      Curl_conn_cf_discard_chain(&cf, ದತ್ತಾಂಶ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx)
      cf_quiche_ctx_free(ctx);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_quiche_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                     ರಚನೆ Curl_peer *origin,
                                     ರಚನೆ Curl_peer *peer,
                                     ರಚನೆ Curl_cfilter **pcf)
{
  ರಚನೆ cf_quiche_ctx *ctx = ಖಾಲಿ;
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಫಲಿತಾಂಶ = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    curlx_safefree(cf);
    ಆದರೆ(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ
