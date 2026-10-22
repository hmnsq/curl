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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#अन्तर्भाव <quiche.h>
#अन्तर्भाव <openssl/err.h>
#अन्तर्भाव <openssl/ssl.h>

#अन्तर्भाव "bufq.h"
#अन्तर्भाव "u32_ptrset.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-socket.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "http1.h"
#अन्तर्भाव "sockaddr.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "vquic/vquic_int.h"
#अन्तर्भाव "vquic/vquic-tls.h"
#अन्तर्भाव "vquic/cf-quiche.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "vtls/openssl.h"
#अन्तर्भाव "vtls/vtls.h"

/* HTTP/3 error values defined in RFC 9114, ch. 8.1 */
#परिभाषय CURL_H3_NO_ERROR  0x0100

#परिभाषय MAX_PKT_BURST          10

#परिभाषय QUIC_MAX_STREAMS       100

#परिभाषय H3_प्रवाहः_गवाक्षः_आकार  (1024 * 128)
#परिभाषय H3_प्रवाहः_CHUNK_आकार   (1024 * 16)
/* Receive and Send max number of chunks follows from the
 * chunk size and window size */
#परिभाषय H3_प्रवाहः_RECV_CHUNKS \
  (H3_प्रवाहः_गवाक्षः_आकार / H3_प्रवाहः_CHUNK_आकार)

/*
 * Store quiche version info in this buffer.
 */
शून्य Curl_quiche_ver(वर्ण *p, size_t len)
{
  (शून्य)curl_msnprintf(p, len, "quiche/%s", quiche_संस्करणम्());
}

संरचना cf_quiche_ctx {
  संरचना cf_quic_ctx q;
  संरचना ssl_peer ssl_peer;
  संरचना curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  संरचना curltime started_at;        /* time the current attempt started */
  संरचना curltime handshake_at;      /* time connect handshake finished */
  संरचना u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  संरचना dynbuf h1hdr;               /* temp buffer for header construction */
  संरचना bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t दत्त_recvd;
  BIT(initialized);
  BIT(goaway);                       /* got GOAWAY from server */
  BIT(x509_store_सज्जता);             /* if x509 store has been set up */
  BIT(shutdown_started);             /* queued shutdown packets */
  BIT(stats_reported);               /* connect statistics reported */
};

#यदि_परिभाषितम् DEBUG_QUICHE
/* initialize debug log callback only once */
स्थिर पूर्णाङ्क debug_log_init = 0;
स्थिर शून्य quiche_debug_log(ध्रुव वर्ण *पङ्क्तिः, शून्य *argp)
{
  (शून्य)argp;
  curl_mfprintf(stderr, "%s\n", पङ्क्तिः);
}
#यदि_अन्तः

स्थिर शून्य h3_प्रवाहः_समुच्चयः_free(uint32_t id, शून्य *प्रवाहः);

स्थिर CURLcode cf_quiche_ctx_init(संरचना cf_quiche_ctx *ctx,
                                   संरचना Curl_peer *origin,
                                   संरचना Curl_peer *peer,
                                   संरचना ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#यदि_परिभाषितम् DEBUG_QUICHE
  यदि(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, रिक्त);
    debug_log_init = 1;
  }
#यदि_अन्तः
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_शीर्षकम्);
  Curl_u32_ptrset_init(&ctx->streams, h3_प्रवाहः_समुच्चयः_free);
  Curl_bufq_init2(&ctx->writebuf, H3_प्रवाहः_CHUNK_आकार, H3_प्रवाहः_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_सीमा);
  ctx->दत्त_recvd = 0;
  ctx->initialized = TRUE;
  प्रत्यागम Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

स्थिर शून्य cf_quiche_ctx_free(संरचना cf_quiche_ctx *ctx)
{
  यदि(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = रिक्त;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

स्थिर शून्य cf_quiche_ctx_close(संरचना cf_quiche_ctx *ctx)
{
  यदि(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = रिक्त;
  }
  यदि(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = रिक्त;
  }
  यदि(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = रिक्त;
  }
  यदि(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = रिक्त;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

स्थिर CURLcode cf_flush_egress(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त);

/**
 * All about the H3 internals of a stream
 */
संरचना h3_प्रवाहः_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  संरचना h1_req_parser h1; /* h1 request parsing */
  uint64_t error3;         /* HTTP/3 stream error code */
  int32_t स्थितिः_संहिता;     /* HTTP status code */
  CURLcode xfer_परिणाम;    /* result from cf_quiche_write_(hd/body) */
  BIT(opened);             /* TRUE after stream has been opened */
  BIT(closed);             /* TRUE on stream close */
  BIT(reset);              /* TRUE on stream reset */
  BIT(send_closed);        /* stream is locally closed */
  BIT(resp_hds_complete);  /* final response has been received */
  BIT(resp_got_शीर्षकम्);    /* TRUE when h3 stream has recvd some HEADER */
  BIT(quic_flow_blocked);  /* stream is blocked by QUIC flow control */
};

स्थिर शून्य h3_प्रवाहः_ctx_free(संरचना h3_प्रवाहः_ctx *प्रवाहः)
{
  Curl_h1_req_parse_free(&प्रवाहः->h1);
  curlx_free(प्रवाहः);
}

स्थिर शून्य h3_प्रवाहः_समुच्चयः_free(uint32_t id, शून्य *प्रवाहः)
{
  (शून्य)id;
  DEBUGASSERT(प्रवाहः);
  h3_प्रवाहः_ctx_free((संरचना h3_प्रवाहः_ctx *)प्रवाहः);
}

प्रकारनिर्देश bool cf_quiche_svisit(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *sdata,
                              संरचना h3_प्रवाहः_ctx *प्रवाहः,
                              शून्य *user_दत्त);

संरचना cf_quiche_दर्शय_ctx {
  संरचना Curl_cfilter *cf;
  संरचना Curl_multi *multi;
  cf_quiche_svisit *cb;
  शून्य *user_दत्त;
};

स्थिर bool cf_quiche_प्रवाहः_do(uint32_t mid, शून्य *val, शून्य *user_दत्त)
{
  संरचना cf_quiche_दर्शय_ctx *vctx = user_दत्त;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = val;
  संरचना Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  यदि(sdata)
    प्रत्यागम vctx->cb(vctx->cf, sdata, प्रवाहः, vctx->user_दत्त);
  प्रत्यागम TRUE;
}

स्थिर शून्य cf_quiche_for_all_streams(संरचना Curl_cfilter *cf,
                                      संरचना Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      शून्य *user_दत्त)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना cf_quiche_दर्शय_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_दत्त = user_दत्त;
  Curl_u32_ptrset_दर्शय(&ctx->streams, cf_quiche_प्रवाहः_do, &vctx);
}

स्थिर bool cf_quiche_do_resume(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *sdata,
                                संरचना h3_प्रवाहः_ctx *प्रवाहः,
                                शून्य *user_दत्त)
{
  (शून्य)user_दत्त;
  यदि(प्रवाहः->quic_flow_blocked) {
    प्रवाहः->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", प्रवाहः->id);
  }
  प्रत्यागम TRUE;
}

स्थिर bool cf_quiche_do_expire(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *sdata,
                                संरचना h3_प्रवाहः_ctx *प्रवाहः,
                                शून्य *user_दत्त)
{
  (शून्य)user_दत्त;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  प्रवाहः->xfer_परिणाम = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  प्रत्यागम TRUE;
}

स्थिर CURLcode h3_दत्त_सज्जता(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);

  यदि(प्रवाहः)
    प्रत्यागम CURLE_OK;

  प्रवाहः = curlx_calloc(1, परिमाण(*प्रवाहः));
  यदि(!प्रवाहः)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रवाहः->id = -1;
  Curl_h1_req_parse_init(&प्रवाहः->h1, H1_PARSE_DEFAULT_MAX_पङ्क्तिः_LEN);

  यदि(Curl_u32_ptrset_समुच्चयः(&ctx->streams, दत्त->mid, प्रवाहः)) {
    h3_प्रवाहः_ctx_free(प्रवाहः);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cf_quiche_प्रवाहः_close(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   संरचना h3_प्रवाहः_ctx *प्रवाहः)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  CURLcode परिणाम;

  यदि(ctx->qconn && !प्रवाहः->closed) {
    quiche_conn_प्रवाहः_shutdown(ctx->qconn, प्रवाहः->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    यदि(!प्रवाहः->send_closed) {
      quiche_conn_प्रवाहः_shutdown(ctx->qconn, प्रवाहः->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      प्रवाहः->send_closed = TRUE;
    }
    प्रवाहः->closed = TRUE;
    परिणाम = cf_flush_egress(cf, दत्त);
    यदि(परिणाम)
      CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  प्रवाहः->id, (पूर्णाङ्क)परिणाम);
  }
}

स्थिर शून्य h3_दत्त_समाप्तम्(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);

  (शून्य)cf;
  यदि(प्रवाहः) {
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] easy handle is done", प्रवाहः->id);
    cf_quiche_प्रवाहः_close(cf, दत्त, प्रवाहः);
    Curl_u32_ptrset_unset(&ctx->streams, दत्त->mid);
  }
}

स्थिर शून्य cf_quiche_expire_conn_closed(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *दत्त)
{
  DEBUGASSERT(दत्त->multi);
  CURL_TRC_CF(दत्त, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, दत्त->multi, cf_quiche_do_expire, रिक्त);
}

स्थिर शून्य cf_quiche_write_hd(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               संरचना h3_प्रवाहः_ctx *प्रवाहः,
                               ध्रुव वर्ण *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  यदि(!प्रवाहः->xfer_परिणाम) {
    प्रवाहः->xfer_परिणाम = Curl_xfer_write_resp_hd(दत्त, buf, blen, eos);
    यदि(प्रवाहः->xfer_परिणाम)
      CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", प्रवाहः->id, (पूर्णाङ्क)प्रवाहः->xfer_परिणाम,
                  blen);
  }
}

संरचना cb_ctx {
  संरचना Curl_cfilter *cf;
  संरचना Curl_easy *दत्त;
  संरचना h3_प्रवाहः_ctx *प्रवाहः;
};

स्थिर bool is_valid_h3_शीर्षकम्(ध्रुव uint8_t *hdr, size_t hlen)
{
  यावत्(hlen--) {
    विकल्प(*hdr++) {
    पक्ष '\n':
    पक्ष '\r':
    पक्ष '\0':
      प्रत्यागम FALSE;
    }
  }
  प्रत्यागम TRUE;
}

स्थिर पूर्णाङ्क cb_each_शीर्षकम्(uint8_t *name, size_t name_len,
                          uint8_t *मानम्, size_t मानम्_len,
                          शून्य *argp)
{
  संरचना cb_ctx *x = argp;
  संरचना Curl_cfilter *cf = x->cf;
  संरचना Curl_easy *दत्त = x->दत्त;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = x->प्रवाहः;
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  यदि(!प्रवाहः || प्रवाहः->xfer_परिणाम)
    प्रत्यागम 1; /* abort iteration */

  यदि((name_len == 7) && !वाक्ये_तुलय(HTTP_PSEUDO_स्थितिः, (वर्ण *)name, 7) &&
     is_valid_h3_शीर्षकम्(मानम्, मानम्_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    परिणाम = Curl_http_decode_स्थितिः(&प्रवाहः->स्थितिः_संहिता,
                                     (ध्रुव वर्ण *)मानम्, मानम्_len);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->h1hdr, (ध्रुव वर्ण *)मानम्, मानम्_len);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    यदि(!परिणाम)
      cf_quiche_write_hd(cf, दत्त, प्रवाहः, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] status: %s",
                प्रवाहः->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  अन्यथा {
    यदि(is_valid_h3_शीर्षकम्(मानम्, मानम्_len) &&
       is_valid_h3_शीर्षकम्(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  प्रवाहः->id, (पूर्णाङ्क)name_len, name,
                  (पूर्णाङ्क)मानम्_len, मानम्);
      curlx_dyn_reset(&ctx->h1hdr);
      परिणाम = curlx_dyn_addn(&ctx->h1hdr, (ध्रुव वर्ण *)name, name_len);
      यदि(!परिणाम)
        परिणाम = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      यदि(!परिणाम)
        परिणाम = curlx_dyn_addn(&ctx->h1hdr, (ध्रुव वर्ण *)मानम्, मानम्_len);
      यदि(!परिणाम)
        परिणाम = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      यदि(!परिणाम)
        cf_quiche_write_hd(cf, दत्त, प्रवाहः, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    अन्यथा
      CURL_TRC_CF(x->दत्त, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  प्रवाहः->id, मानम्_len + name_len);
  }

  यदि(परिणाम) {
    CURL_TRC_CF(x->दत्त, x->cf, "[%" PRIu64 "] on header error %d",
                प्रवाहः->id, (पूर्णाङ्क)परिणाम);
    यदि(!प्रवाहः->xfer_परिणाम)
      प्रवाहः->xfer_परिणाम = परिणाम;
  }
  प्रत्यागम प्रवाहः->xfer_परिणाम ? 1 : 0;
}

स्थिर CURLcode प्रवाहः_resp_read(शून्य *reader_ctx,
                                 अचिह्नित वर्ण *buf, size_t len,
                                 size_t *pnread)
{
  संरचना cb_ctx *x = reader_ctx;
  संरचना cf_quiche_ctx *ctx = x->cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, x->दत्त);
  ssize_t nread;

  *pnread = 0;
  यदि(!प्रवाहः)
    प्रत्यागम CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, प्रवाहः->id, buf, len);
  यदि(!curlx_sztouz(nread, pnread))
    प्रत्यागम CURLE_AGAIN;
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cf_quiche_flush_body(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना h3_प्रवाहः_ctx *प्रवाहः)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  ध्रुव uint8_t *buf;
  size_t blen;

  यावत्(प्रवाहः && !प्रवाहः->xfer_परिणाम) {
    यदि(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      प्रवाहः->xfer_परिणाम = Curl_xfer_write_resp(
        दत्त, (ध्रुव वर्ण *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      यदि(प्रवाहः->xfer_परिणाम) {
        CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", प्रवाहः->id, (पूर्णाङ्क)प्रवाहः->xfer_परिणाम, blen);
      }
    }
    अन्यथा
      भङ्ग;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

स्थिर शून्य cf_quiche_recv_body(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                संरचना h3_प्रवाहः_ctx *प्रवाहः)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  संरचना cb_ctx cb_ctx;
  CURLcode परिणाम = CURLE_OK;

  यदि(!प्रवाहः)
    प्रत्यागम;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.दत्त = दत्त;
  cb_ctx.प्रवाहः = प्रवाहः;
  Curl_bufq_reset(&ctx->writebuf);
  यावत्(!परिणाम) {
    परिणाम = Curl_bufq_slurp(&ctx->writebuf,
                             प्रवाहः_resp_read, &cb_ctx, &nread);
    यदि(!परिणाम)
      cf_quiche_flush_body(cf, दत्त, प्रवाहः);
    अन्यथा यदि(परिणाम == CURLE_AGAIN)
      भङ्ग;
    अन्यथा यदि(परिणाम) {
      CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] recv_body error %d",
                  प्रवाहः->id, (पूर्णाङ्क)परिणाम);
      failf(दत्त, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            प्रवाहः->id, (पूर्णाङ्क)परिणाम);
      प्रवाहः->closed = TRUE;
      प्रवाहः->reset = TRUE;
      प्रवाहः->send_closed = TRUE;
      यदि(!प्रवाहः->xfer_परिणाम)
        प्रवाहः->xfer_परिणाम = परिणाम;
    }
  }
  cf_quiche_flush_body(cf, दत्त, प्रवाहः);
}

स्थिर शून्य cf_quiche_process_ev(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना h3_प्रवाहः_ctx *प्रवाहः,
                                 quiche_h3_event *ev)
{
  गणना quiche_h3_event_type type;

  यदि(!प्रवाहः)
    प्रत्यागम;

  type = quiche_h3_event_type(ev);

  विकल्प(type) {
  पक्ष QUICHE_H3_EVENT_शीर्षकाणि: {
    संरचना cb_ctx cb_ctx;
    प्रवाहः->resp_got_शीर्षकम् = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.दत्त = दत्त;
    cb_ctx.प्रवाहः = प्रवाहः;
    quiche_h3_event_for_each_शीर्षकम्(ev, cb_each_शीर्षकम्, &cb_ctx);
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] <- [HEADERS]", प्रवाहः->id);
    Curl_multi_mark_dirty(दत्त);
    भङ्ग;
  }
  पक्ष QUICHE_H3_EVENT_दत्त:
    यदि(!प्रवाहः->resp_hds_complete) {
      प्रवाहः->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, दत्त, प्रवाहः, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, दत्त, प्रवाहः);
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] <- [DATA]", प्रवाहः->id);
    Curl_multi_mark_dirty(दत्त);
    भङ्ग;

  पक्ष QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] RESET", प्रवाहः->id);
    प्रवाहः->closed = TRUE;
    प्रवाहः->reset = TRUE;
    प्रवाहः->send_closed = TRUE;
    Curl_multi_mark_dirty(दत्त);
    भङ्ग;

  पक्ष QUICHE_H3_EVENT_परिसमाप्तम्:
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] CLOSED", प्रवाहः->id);
    यदि(!प्रवाहः->resp_hds_complete) {
      प्रवाहः->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, दत्त, प्रवाहः, "\r\n", 2, TRUE);
    }
    प्रवाहः->closed = TRUE;
    Curl_multi_mark_dirty(दत्त);
    भङ्ग;

  पक्ष QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] <- [GOAWAY]", प्रवाहः->id);
    भङ्ग;

  स्वभाव:
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] recv, unhandled event %d",
                प्रवाहः->id, (पूर्णाङ्क)type);
    भङ्ग;
  }
}

संरचना cf_quiche_disp_ctx {
  uint64_t प्रवाहः_id;
  संरचना Curl_cfilter *cf;
  संरचना Curl_multi *multi;
  quiche_h3_event *ev;
};

स्थिर bool cf_quiche_disp_event(uint32_t mid, शून्य *val, शून्य *user_दत्त)
{
  संरचना cf_quiche_disp_ctx *dctx = user_दत्त;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = val;

  यदि(प्रवाहः->id == dctx->प्रवाहः_id) {
    संरचना Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    यदि(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, प्रवाहः, dctx->ev);
    प्रत्यागम FALSE; /* stop iterating */
  }
  प्रत्यागम TRUE;
}

स्थिर CURLcode cf_poll_events(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = रिक्त;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  यावत्(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    यदि(rv == QUICHE_H3_ERR_समाप्तम्) {
      भङ्ग;
    }
    अन्यथा यदि(rv < 0) {
      CURL_TRC_CF(दत्त, cf, "error poll: %" PRId64, rv);
      प्रत्यागम CURLE_HTTP3;
    }
    अन्यथा {
      प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
      यदि(प्रवाहः && प्रवाहः->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, दत्त, प्रवाहः, ev);
        quiche_h3_event_free(ev);
        यदि(प्रवाहः->xfer_परिणाम)
          प्रत्यागम प्रवाहः->xfer_परिणाम;
      }
      अन्यथा {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        संरचना cf_quiche_disp_ctx dctx;
        dctx.प्रवाहः_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = दत्त->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_दर्शय(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  प्रत्यागम CURLE_OK;
}

संरचना recv_ctx {
  संरचना Curl_cfilter *cf;
  संरचना Curl_easy *दत्त;
  पूर्णाङ्क pkts;
};

स्थिर CURLcode cf_quiche_recv_pkts(ध्रुव अचिह्नित वर्ण *buf, size_t buflen,
                                    size_t gso_आकार,
                                    संरचना sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    शून्य *userp)
{
  संरचना recv_ctx *r = userp;
  संरचना cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, विस्थापनम्, nread;
  ssize_t rv;

  (शून्य)ecn;

  recv_info.गन्तव्यम् = (संरचना sockaddr *)&ctx->q.local_addr;
  recv_info.गन्तव्यम्_len = ctx->q.local_addrlen;
  recv_info.from = (संरचना sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  प्रति(विस्थापनम् = 0; विस्थापनम् < buflen; विस्थापनम् += gso_आकार) {
    pktlen = ((विस्थापनम् + gso_आकार) <= buflen) ? gso_आकार : (buflen - विस्थापनम्);
    rv = quiche_conn_recv(ctx->qconn,
                          (अचिह्नित वर्ण *)CURL_UNCONST(buf + विस्थापनम्),
                          pktlen, &recv_info);
    यदि(!curlx_sztouz(rv, &nread)) {
      यदि(QUICHE_ERR_समाप्तम् == rv) {
        यदि(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->दत्त, r->cf, "ingress, connection is draining");
          प्रत्यागम CURLE_RECV_ERROR;
        }
        यदि(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->दत्त, r->cf, "ingress, connection is closed");
          प्रत्यागम CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->दत्त, r->cf, "ingress, quiche is DONE");
        प्रत्यागम CURLE_OK;
      }
      अन्यथा यदि(QUICHE_ERR_TLS_FAIL == rv) {
        दीर्घ verify_ok = SSL_get_verify_परिणाम(ctx->tls.ossl.ssl);
        यदि(verify_ok != X509_V_OK) {
          failf(r->दत्त, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->दत्त, "ingress, quiche reports TLS fail");
        प्रत्यागम CURLE_RECV_ERROR;
      }
      अन्यथा {
        failf(r->दत्त, "quiche reports error %zd on receive", rv);
        प्रत्यागम CURLE_RECV_ERROR;
      }
    }
    अन्यथा यदि(nread < pktlen) {
      CURL_TRC_CF(r->दत्त, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_process_ingress(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना recv_ctx rctx;
  CURLcode परिणाम;

  DEBUGASSERT(ctx->qconn);
  परिणाम = Curl_vquic_tls_before_recv(&ctx->tls, cf, दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  rctx.cf = cf;
  rctx.दत्त = दत्त;
  rctx.pkts = 0;

  परिणाम = Curl_vquic_recv_packets(cf, दत्त, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(दत्त->multi);
    cf_quiche_for_all_streams(cf, दत्त->multi, cf_quiche_do_resume, रिक्त);
  }
  प्रत्यागम cf_poll_events(cf, दत्त);
}

संरचना read_ctx {
  संरचना Curl_cfilter *cf;
  संरचना Curl_easy *दत्त;
  quiche_send_info send_info;
};

स्थिर CURLcode read_pkt_गन्तव्यम्_send(शून्य *userp,
                                 अचिह्नित वर्ण *buf, size_t buflen,
                                 size_t *pnread)
{
  संरचना read_ctx *x = userp;
  संरचना cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  यदि(rv == QUICHE_ERR_समाप्तम्)
    प्रत्यागम CURLE_AGAIN;

  यदि(!curlx_sztouz(rv, pnread)) {
    failf(x->दत्त, "quiche_conn_send returned %zd", rv);
    प्रत्यागम CURLE_SEND_ERROR;
  }
  प्रत्यागम CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
स्थिर CURLcode cf_flush_egress(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode परिणाम;
  int64_t expiry_ns;
  int64_t timeout_ns;
  संरचना read_ctx readx;
  size_t pkt_गणनम्, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  यदि(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    यदि(quiche_conn_is_closed(ctx->qconn)) {
      यदि(quiche_conn_is_timed_बहिर्गमनम्(ctx->qconn))
        failf(दत्त, "connection closed by idle timeout");
      अन्यथा
        failf(दत्त, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, दत्त);
      प्रत्यागम CURLE_SEND_ERROR;
    }
  }

  परिणाम = Curl_vquic_flush(cf, दत्त, &ctx->q);
  यदि(परिणाम) {
    यदि(परिणाम == CURLE_AGAIN) {
      Curl_expire(दत्त, 1, EXPIRE_QUIC);
      प्रत्यागम CURLE_OK;
    }
    प्रत्यागम परिणाम;
  }

  readx.cf = cf;
  readx.दत्त = दत्त;
  स्मृतिं_पूरय(&readx.send_info, 0, परिमाण(readx.send_info));
  pkt_गणनम् = 0;
  gsolen = quiche_conn_max_send_udp_payload_आकार(ctx->qconn);
  प्रति(;;) {
    /* add the next packet to send, if any, to our buffer */
    परिणाम = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_गन्तव्यम्_send, &readx, &nread);
    यदि(परिणाम) {
      यदि(परिणाम != CURLE_AGAIN)
        प्रत्यागम परिणाम;
      /* Nothing more to add, flush and leave */
      परिणाम = Curl_vquic_send(cf, दत्त, &ctx->q, gsolen);
      यदि(परिणाम) {
        यदि(परिणाम == CURLE_AGAIN) {
          Curl_expire(दत्त, 1, EXPIRE_QUIC);
          प्रत्यागम CURLE_OK;
        }
        प्रत्यागम परिणाम;
      }
      गच्छ बहिर्गमनम्;
    }

    ++pkt_गणनम्;
    यदि(nread < gsolen || pkt_गणनम् >= MAX_PKT_BURST) {
      परिणाम = Curl_vquic_send(cf, दत्त, &ctx->q, gsolen);
      यदि(परिणाम) {
        यदि(परिणाम == CURLE_AGAIN) {
          Curl_expire(दत्त, 1, EXPIRE_QUIC);
          प्रत्यागम CURLE_OK;
        }
        गच्छ बहिर्गमनम्;
      }
      pkt_गणनम् = 0;
    }
  }

बहिर्गमनम्:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  यदि(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(दत्त, (timeout_ns / 1000000), EXPIRE_QUIC);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode recv_closed_प्रवाहः(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   size_t *pnread)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(प्रवाहः);
  *pnread = 0;
  यदि(प्रवाहः->reset) {
    यदि(प्रवाहः->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(दत्त, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", प्रवाहः->id);
      connclose(cf->conn); /* do not use this anymore */
      दत्त->state.refused_प्रवाहः = TRUE;
      प्रत्यागम CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    अन्यथा यदि(प्रवाहः->resp_hds_complete && दत्त->req.no_body) {
        CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", प्रवाहः->id, प्रवाहः->error3,
                    Curl_vquic_h3_err_str(प्रवाहः->error3));
        प्रत्यागम CURLE_OK;
    }
    failf(दत्त, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", प्रवाहः->id, प्रवाहः->error3,
          Curl_vquic_h3_err_str(प्रवाहः->error3));
    परिणाम = दत्त->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                प्रवाहः->id, (पूर्णाङ्क)परिणाम);
  }
  अन्यथा यदि(!प्रवाहः->resp_got_शीर्षकम्) {
    failf(दत्त, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          प्रवाहः->id);
    परिणाम = CURLE_HTTP3;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_quiche_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                               वर्ण *buf, size_t blen, size_t *pnread)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
  CURLcode परिणाम = CURLE_OK;

  *pnread = 0;
  (शून्य)buf;
  (शून्य)blen;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(दत्त));

  यदि(!प्रवाहः)
    प्रत्यागम CURLE_RECV_ERROR;

  परिणाम = cf_process_ingress(cf, दत्त);
  यदि(परिणाम) {
    CURL_TRC_CF(दत्त, cf, "cf_recv, error on ingress");
    गच्छ बहिर्गमनम्;
  }

  यदि(प्रवाहः->xfer_परिणाम) {
    cf_quiche_प्रवाहः_close(cf, दत्त, प्रवाहः);
    परिणाम = प्रवाहः->xfer_परिणाम;
    गच्छ बहिर्गमनम्;
  }
  अन्यथा यदि(प्रवाहः->closed)
    परिणाम = recv_closed_प्रवाहः(cf, दत्त, pnread);
  अन्यथा यदि(quiche_conn_is_draining(ctx->qconn)) {
    failf(दत्त, "QUIC connection is draining");
    परिणाम = CURLE_HTTP3;
  }
  अन्यथा
    परिणाम = CURLE_AGAIN;

बहिर्गमनम्:
  परिणाम = Curl_1st_fatal(परिणाम, cf_flush_egress(cf, दत्त));
  यदि(*pnread > 0)
    ctx->दत्त_recvd += *pnread;
  CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, प्रवाहः->id, blen, (पूर्णाङ्क)परिणाम, *pnread,
              ctx->दत्त_recvd);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_quiche_send_body(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    संरचना h3_प्रवाहः_ctx *प्रवाहः,
                                    ध्रुव uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, प्रवाहः->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  यदि(rv == QUICHE_H3_ERR_समाप्तम् || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    यदि(!quiche_conn_प्रवाहः_writable(ctx->qconn, प्रवाहः->id, len)) {
      CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", प्रवाहः->id, len);
      प्रवाहः->quic_flow_blocked = TRUE;
    }
    प्रत्यागम CURLE_AGAIN;
  }
  अन्यथा यदि(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_प्रवाहः_STATE) {
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", प्रवाहः->id, len);
    प्रत्यागम CURLE_HTTP3;
  }
  अन्यथा यदि(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_आकार) {
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                प्रवाहः->id, len);
    प्रत्यागम CURLE_SEND_ERROR;
  }
  अन्यथा यदि(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                प्रवाहः->id, len, rv);
    प्रत्यागम CURLE_SEND_ERROR;
  }
  अन्यथा {
    यदि(eos && (len == *pnwritten))
      प्रवाहः->send_closed = TRUE;
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                प्रवाहः->id, len, प्रवाहः->send_closed, *pnwritten);
    प्रत्यागम CURLE_OK;
  }
}

स्थिर CURLcode h3_open_प्रवाहः(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               ध्रुव uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
  size_t nheader, i;
  int64_t rv;
  संरचना dynhds h2_शीर्षकाणि;
  quiche_h3_शीर्षकम् *nva = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  *pnwritten = 0;
  यदि(!प्रवाहः) {
    परिणाम = h3_दत्त_सज्जता(cf, दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
    DEBUGASSERT(प्रवाहः);
  }

  Curl_dynhds_init(&h2_शीर्षकाणि, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(प्रवाहः);

  परिणाम = Curl_h1_req_parse_read(&प्रवाहः->h1, buf, blen, रिक्त,
    !दत्त->state.http_ignorecustom ?
    CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST) : रिक्त,
    0, pnwritten);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  यदि(!प्रवाहः->h1.समाप्तम्) {
    /* need more data */
    गच्छ बहिर्गमनम्;
  }
  DEBUGASSERT(प्रवाहः->h1.req);

  परिणाम = Curl_http_req_गन्तव्यम्_h2(&h2_शीर्षकाणि, प्रवाहः->h1.req, दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* no longer needed */
  Curl_h1_req_parse_free(&प्रवाहः->h1);

  nheader = Curl_dynhds_गणनम्(&h2_शीर्षकाणि);
  nva = curlx_malloc(परिमाण(quiche_h3_शीर्षकम्) * nheader);
  यदि(!nva) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  प्रति(i = 0; i < nheader; ++i) {
    संरचना dynhds_प्रविष्टिः *e = Curl_dynhds_getn(&h2_शीर्षकाणि, i);
    nva[i].name = (अचिह्नित वर्ण *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].मानम् = (अचिह्नित वर्ण *)e->मानम्;
    nva[i].मानम्_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  यदि(eos && !blen)
    प्रवाहः->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              प्रवाहः->send_closed);
  CURL_TRC_CF(दत्त, cf, "quiche_send_request() -> %" PRId64, rv);
  यदि(rv < 0) {
    यदि(QUICHE_H3_ERR_प्रवाहः_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] blocked", प्रवाहः->id);
      प्रवाहः->quic_flow_blocked = TRUE;
      परिणाम = CURLE_AGAIN;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा {
      CURL_TRC_CF(दत्त, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&दत्त->state.url), rv);
    }
    परिणाम = CURLE_SEND_ERROR;
    गच्छ बहिर्गमनम्;
  }

  DEBUGASSERT(!प्रवाहः->opened);
  प्रवाहः->id = (uint64_t)rv;
  प्रवाहः->opened = TRUE;
  प्रवाहः->closed = FALSE;
  प्रवाहः->reset = FALSE;

  यदि(Curl_trc_is_verbose(दत्त)) {
    infof(दत्त, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          प्रवाहः->id, Curl_bufref_ptr(&दत्त->state.url));
    प्रति(i = 0; i < nheader; ++i) {
      infof(दत्त, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", प्रवाहः->id,
            (पूर्णाङ्क)nva[i].name_len, nva[i].name,
            (पूर्णाङ्क)nva[i].मानम्_len, nva[i].मानम्);
    }
  }

  यदि(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, दत्त, प्रवाहः, buf, blen, eos, &nwritten);
    यदि(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      परिणाम = r2;
    }
    अन्यथा यदि(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

बहिर्गमनम्:
  curlx_free(nva);
  Curl_dynhds_free(&h2_शीर्षकाणि);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_quiche_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                               ध्रुव uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
  CURLcode परिणाम;

  *pnwritten = 0;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(दत्त));

  परिणाम = cf_process_ingress(cf, दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यदि(!प्रवाहः || !प्रवाहः->opened) {
    परिणाम = h3_open_प्रवाहः(cf, दत्त, buf, len, eos, pnwritten);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
  }
  अन्यथा यदि(प्रवाहः->xfer_परिणाम) {
    cf_quiche_प्रवाहः_close(cf, दत्त, प्रवाहः);
    परिणाम = प्रवाहः->xfer_परिणाम;
  }
  अन्यथा यदि(प्रवाहः->closed) {
    यदि(प्रवाहः->resp_hds_complete) {
      /* sending request body on a stream that has been closed by the
       * server. If the server has send us a final response, we should
       * silently discard the send data.
       * This happens for example on redirects where the server, instead
       * of reading the full request body closed the stream after
       * sending the 30x response.
       * This is sort of a race: had the transfer loop called recv first,
       * it would see the response and stop/discard sending on its own- */
      CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] discarding data "
                  "on closed stream with response", प्रवाहः->id);
      परिणाम = CURLE_OK;
      *pnwritten = len;
      गच्छ बहिर्गमनम्;
    }
    CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", प्रवाहः->id, len);
    परिणाम = CURLE_HTTP3;
    गच्छ बहिर्गमनम्;
  }
  अन्यथा {
    परिणाम = cf_quiche_send_body(cf, दत्त, प्रवाहः, buf, len, eos, pnwritten);
  }

बहिर्गमनम्:
  परिणाम = Curl_1st_fatal(परिणाम, cf_flush_egress(cf, दत्त));

  CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              प्रवाहः ? प्रवाहः->id : (uint64_t)~0, len,
              (पूर्णाङ्क)परिणाम, *pnwritten);
  प्रत्यागम परिणाम;
}

स्थिर bool प्रवाहः_is_writable(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);

  प्रत्यागम प्रवाहः && (quiche_conn_प्रवाहः_writable(
    ctx->qconn, प्रवाहः->id, 1) > 0);
}

स्थिर CURLcode cf_quiche_adjust_pollset(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *दत्त,
                                         संरचना easy_pollset *ps)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode परिणाम = CURLE_OK;

  यदि(!ctx->qconn)
    प्रत्यागम CURLE_OK;

  Curl_pollset_check(दत्त, ps, ctx->q.sockfd, &want_recv, &want_send);
  यदि(want_recv || want_send) {
    संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
    bool c_exhaust, s_exhaust;

    c_exhaust = FALSE; /* Have not found any call in quiche that tells
                          us if the connection itself is blocked */
    s_exhaust = want_send && प्रवाहः && प्रवाहः->opened &&
                (प्रवाहः->quic_flow_blocked || !प्रवाहः_is_writable(cf, दत्त));
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_रिक्तानि(&ctx->q.sendbuf);

    परिणाम = Curl_pollset_समुच्चयः(दत्त, ps, ctx->q.sockfd, want_recv, want_send);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode h3_दत्त_pause(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (शून्य)cf;
  यदि(!pause) {
    Curl_multi_mark_dirty(दत्त);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_quiche_cntrl(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  (शून्य)arg1;
  (शून्य)arg2;
  विकल्प(event) {
  पक्ष CF_CTRL_दत्त_सज्जता:
    भङ्ग;
  पक्ष CF_CTRL_दत्त_PAUSE:
    परिणाम = h3_दत्त_pause(cf, दत्त, (arg1 != 0));
    भङ्ग;
  पक्ष CF_CTRL_दत्त_समाप्तम्:
    h3_दत्त_समाप्तम्(cf, दत्त);
    भङ्ग;
  पक्ष CF_CTRL_दत्त_समाप्तम्_SEND: {
    संरचना h3_प्रवाहः_ctx *प्रवाहः = H3_प्रवाहः_CTX(ctx, दत्त);
    यदि(प्रवाहः && !प्रवाहः->send_closed) {
      अचिह्नित वर्ण body[1];
      size_t प्रेषितम्;

      प्रवाहः->send_closed = TRUE;
      body[0] = 'X';
      परिणाम = cf_quiche_send(cf, दत्त, body, 0, TRUE, &प्रेषितम्);
      CURL_TRC_CF(दत्त, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  प्रवाहः->id, (पूर्णाङ्क)परिणाम, प्रेषितम्);
    }
    भङ्ग;
  }
  पक्ष CF_CTRL_CONN_INFO_UPDATE:
    यदि(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_आलोकितम् = 30;
      Curl_conn_समुच्चयः_multiplex(cf->conn);
    }
    भङ्ग;
  पक्ष CF_CTRL_REPORT_STATS:
    यदि(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrsTimeWas(दत्त, TIMER_CONNECT, ctx->q.first_byte_at);
      Curl_pgrsTimeWas(दत्त, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_quiche_ctx_open(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  पूर्णाङ्क rv;
  CURLcode परिणाम;
  ध्रुव संरचना Curl_sockaddr_ex *sockaddr;
  स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  परिणाम = Curl_vquic_ctx_init(दत्त, &ctx->q);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_संस्करणम्);
  यदि(!ctx->cfg) {
    failf(दत्त, "cannot create quiche config");
    प्रत्यागम CURLE_FAILED_INIT;
  }
  quiche_config_enable_pacing(ctx->cfg, FALSE);
  quiche_config_समुच्चयः_initial_max_दत्त(ctx->cfg, (1 * 1024 * 1024)
    /* (QUIC_MAX_STREAMS/2) * H3_STREAM_WINDOW_SIZE */);
  quiche_config_समुच्चयः_initial_max_streams_bidi(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_समुच्चयः_initial_max_streams_uni(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_समुच्चयः_initial_max_प्रवाहः_दत्त_bidi_local(ctx->cfg,
    H3_प्रवाहः_गवाक्षः_आकार);
  quiche_config_समुच्चयः_initial_max_प्रवाहः_दत्त_bidi_remote(ctx->cfg,
    H3_प्रवाहः_गवाक्षः_आकार);
  quiche_config_समुच्चयः_initial_max_प्रवाहः_दत्त_uni(ctx->cfg,
    H3_प्रवाहः_गवाक्षः_आकार);
  quiche_config_समुच्चयः_disable_सक्रिय_migration(ctx->cfg, TRUE);

  quiche_config_समुच्चयः_max_connection_गवाक्षः(ctx->cfg,
    10 * QUIC_MAX_STREAMS * H3_प्रवाहः_गवाक्षः_आकार);
  quiche_config_समुच्चयः_max_प्रवाहः_गवाक्षः(ctx->cfg, 10 * H3_प्रवाहः_गवाक्षः_आकार);
  quiche_config_समुच्चयः_application_protos(ctx->cfg,
                      (uint8_t *)CURL_UNCONST(QUICHE_H3_APPLICATION_PROTOCOL),
                                 CURL_CSTRLEN(QUICHE_H3_APPLICATION_PROTOCOL));

  परिणाम = Curl_vquic_tls_init(&ctx->tls, cf, दत्त, &ctx->ssl_peer,
                               &ALPN_SPEC_H3, रिक्त, रिक्त, cf, रिक्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = Curl_rand(दत्त, ctx->scid, परिमाण(ctx->scid));
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(Curl_cf_socket_peek(cf->अग्र, दत्त, &ctx->q.sockfd, &sockaddr, रिक्त))
    प्रत्यागम CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = परिमाण(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (संरचना sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  यदि(rv == -1)
    प्रत्यागम CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((ध्रुव uint8_t *)ctx->scid,
                                        परिमाण(ctx->scid), रिक्त, 0,
                                        (संरचना sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  यदि(!ctx->qconn) {
    failf(दत्त, "cannot create quiche connection");
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  /* Known to not work on Windows */
#यदि !defined(_WIN32) && defined(HAVE_QUICHE_CONN_समुच्चयः_QLOG_FD)
  {
    पूर्णाङ्क qfd;
    (शून्य)Curl_qlogdir(दत्त, ctx->scid, परिमाण(ctx->scid), &qfd);
    यदि(qfd != -1)
      quiche_conn_समुच्चयः_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#यदि_अन्तः

  परिणाम = cf_flush_egress(cf, दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  {
    अचिह्नित वर्ण alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    अचिह्नित alpn_len, विस्थापनम् = 0;

    /* Replace each ALPN length prefix by a comma. */
    यावत्(विस्थापनम् < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[विस्थापनम्];
      alpn_protocols[विस्थापनम्] = ',';
      विस्थापनम् += 1 + alpn_len;
    }

    CURL_TRC_CF(दत्त, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_quiche_verify_peer(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  प्रत्यागम Curl_vquic_tls_verify_peer(&ctx->tls, cf, दत्त, &ctx->ssl_peer);
}

स्थिर CURLcode cf_quiche_connect(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  bool *समाप्तम्)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  /* Connect the UDP filter first */
  यदि(!cf->अग्र->connected) {
    परिणाम = Curl_conn_cf_connect(cf->अग्र, दत्त, समाप्तम्);
    यदि(परिणाम || !*समाप्तम्)
      प्रत्यागम परिणाम;
  }

  *समाप्तम् = FALSE;
  यदि(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(दत्त, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(दत्त, cf, "need HTTPS-RR, delaying connect");
    प्रत्यागम CURLE_OK;
  }

  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(दत्त));

  यदि(!ctx->qconn) {
    परिणाम = cf_quiche_ctx_open(cf, दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    ctx->started_at = ctx->q.चरमः_op;
    परिणाम = cf_flush_egress(cf, दत्त);
    /* we do not expect to be able to recv anything yet */
    गच्छ बहिर्गमनम्;
  }

  परिणाम = cf_process_ingress(cf, दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = cf_flush_egress(cf, दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यदि(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.चरमः_op;
    CURL_TRC_CF(दत्त, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    परिणाम = cf_quiche_verify_peer(cf, दत्त);
    यदि(!परिणाम) {
      CURL_TRC_CF(दत्त, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      यदि(!ctx->h3config) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_समुच्चयः_max_क्षेत्रम्_section_आकार(ctx->h3config,
                                                  MAX_HTTP_RESP_शीर्षकम्_आकार);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      यदि(!ctx->h3c) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
      cf->connected = TRUE;
      *समाप्तम् = TRUE;
    }
  }
  अन्यथा यदि(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
  }

बहिर्गमनम्:
#यदि_परिभाषितम् CURLVERBOSE
  यदि(परिणाम && परिणाम != CURLE_AGAIN) {
    संरचना ip_quadruple ip;

    यदि(!Curl_cf_socket_peek(cf->अग्र, दत्त, रिक्त, रिक्त, &ip))
      infof(दत्त, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_पत्तनम्, curl_easy_strerror(परिणाम));
    अन्यथा
      infof(दत्त, "connect failed: %s", curl_easy_strerror(परिणाम));
  }
#यदि_अन्तः
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_quiche_shutdown(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  यदि(cf->shutdown || !ctx || !ctx->qconn) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *समाप्तम् = FALSE;
  यदि(!ctx->shutdown_started) {
    पूर्णाङ्क err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(दत्त));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, रिक्त, 0);
    यदि(err) {
      CURL_TRC_CF(दत्त, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }

  यदि(!Curl_bufq_is_रिक्तानि(&ctx->q.sendbuf)) {
    CURL_TRC_CF(दत्त, cf, "shutdown, flushing sendbuf");
    परिणाम = cf_flush_egress(cf, दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  यदि(Curl_bufq_is_रिक्तानि(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(दत्त, cf, "shutdown completely sent off, done");
    *समाप्तम् = TRUE;
  }
  अन्यथा {
    CURL_TRC_CF(दत्त, cf, "shutdown sending blocked");
  }

बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_quiche_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  (शून्य)दत्त;
  यदि(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = रिक्त;
  }
}

स्थिर CURLcode cf_quiche_query(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;

  विकल्प(query) {
  पक्ष CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    यदि(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_वामः_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > अधिकतम_पूर्णाङ्कः) ? अधिकतम_पूर्णाङ्कः : (पूर्णाङ्क)max_streams;
    CURL_TRC_CF(दत्त, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    प्रत्यागम CURLE_OK;
  }
  पक्ष CF_QUERY_CONNECT_प्रत्युत्तरम्_MS:
    यदि(ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < अधिकतम_पूर्णाङ्कः) ? (पूर्णाङ्क)ms : अधिकतम_पूर्णाङ्कः;
    }
    अन्यथा
      *pres1 = -1;
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_HTTP_संस्करणम्:
    *pres1 = 30;
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_SSL_INFO:
  पक्ष CF_QUERY_SSL_CTX_INFO: {
    संरचना curl_tlssessioninfo *info = pres2;
    यदि(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      प्रत्यागम CURLE_OK;
    भङ्ग;
  }
  पक्ष CF_QUERY_ALPN_NEGOTIATED: {
    ध्रुव वर्ण **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : रिक्त;
    प्रत्यागम CURLE_OK;
  }
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, दत्त, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर bool cf_quiche_conn_is_alive(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    bool *निवेशः_pending)
{
  संरचना cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *निवेशः_pending = FALSE;
  यदि(!ctx->qconn)
    प्रत्यागम FALSE;

  यदि(quiche_conn_is_closed(ctx->qconn)) {
    यदि(quiche_conn_is_timed_बहिर्गमनम्(ctx->qconn))
      CURL_TRC_CF(दत्त, cf, "connection was closed due to idle timeout");
    अन्यथा
      CURL_TRC_CF(दत्त, cf, "connection is closed");
    प्रत्यागम FALSE;
  }

  यदि(!cf->अग्र || !cf->अग्र->cft->is_alive(cf->अग्र, दत्त, निवेशः_pending))
    प्रत्यागम FALSE;

  यदि(*निवेशः_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *निवेशः_pending = FALSE;
    यदि(cf_process_ingress(cf, दत्त))
      alive = FALSE;
    अन्यथा {
      alive = TRUE;
    }
  }

  प्रत्यागम alive;
}

संरचना Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_quiche_destroy,
  cf_quiche_connect,
  cf_quiche_shutdown,
  cf_quiche_adjust_pollset,
  Curl_cf_def_दत्त_pending,
  cf_quiche_send,
  cf_quiche_recv,
  cf_quiche_cntrl,
  cf_quiche_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_quiche_query,
};

CURLcode Curl_cf_quiche_create(संरचना Curl_cfilter **pcf,
                               संरचना Curl_easy *दत्त,
                               संरचना Curl_peer *origin,
                               संरचना Curl_peer *peer,
                               संरचना connectdata *conn,
                               संरचना Curl_sockaddr_ex *addr)
{
  संरचना cf_quiche_ctx *ctx = रिक्त;
  संरचना Curl_cfilter *cf = रिक्त;
  CURLcode परिणाम;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  परिणाम = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  यदि(!परिणाम)
    परिणाम = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  cf->conn = conn;

  परिणाम = Curl_cf_udp_create(&cf->अग्र, दत्त, origin, peer, TRNSPRT_QUIC,
                              conn, addr, रिक्त, TRNSPRT_QUIC);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  cf->अग्र->conn = cf->conn;
  cf->अग्र->sockindex = cf->sockindex;

बहिर्गमनम्:
  *pcf = (!परिणाम) ? cf : रिक्त;
  यदि(परिणाम) {
    यदि(cf)
      Curl_conn_cf_discard_chain(&cf, दत्त);
    अन्यथा यदि(ctx)
      cf_quiche_ctx_free(ctx);
  }

  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_quiche_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                     संरचना Curl_peer *origin,
                                     संरचना Curl_peer *peer,
                                     संरचना Curl_cfilter **pcf)
{
  संरचना cf_quiche_ctx *ctx = रिक्त;
  संरचना Curl_cfilter *cf = रिक्त;
  CURLcode परिणाम;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  परिणाम = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  यदि(!परिणाम)
    परिणाम = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  Curl_conn_cf_निवेशय_after(cf_at, cf);

बहिर्गमनम्:
  यदि(परिणाम) {
    curlx_safefree(cf);
    यदि(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  प्रत्यागम परिणाम;
}

#यदि_अन्तः
