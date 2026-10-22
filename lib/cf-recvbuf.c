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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-recvbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_CF_RECVBUF_CHUNK     (16 * 1024)

ਬਣਤਰ cf_recvbuf_ctx {
  ਬਣਤਰ bufq recvbuf;
};

ਸਥਿਰ ਖਾਲੀ cf_recvbuf_destroy(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_recvbuf_ctx *ctx = cf->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

ਸਥਿਰ CURLcode cf_recvbuf_recv(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਅੱਖਰ *buf, size_t len,
                                size_t *pnread)
{
  ਬਣਤਰ cf_recvbuf_ctx *ctx = cf->ctx;

  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->recvbuf)) {
    ਵਾਪਸ Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  ਜੇ(cf->ਅਗਲਾ)
    ਵਾਪਸ cf->ਅਗਲਾ->cft->do_recv(cf->ਅਗਲਾ, ਅੰਕੜੇ, buf, len, pnread);
  *pnread = 0;
  ਵਾਪਸ CURLE_RECV_ERROR;
}

ਸਥਿਰ bool cf_recvbuf_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                                    ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_recvbuf_ctx *ctx = cf->ctx;

  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->recvbuf))
    ਵਾਪਸ TRUE;

  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->has_ਅੰਕੜੇ_pending(cf->ਅਗਲਾ, ਅੰਕੜੇ) : FALSE;
}

ਬਣਤਰ Curl_cftype Curl_cft_recvbuf = {
  "RECVBUF",
  0,
  CURL_LOG_LVL_NONE,
  cf_recvbuf_destroy,
  Curl_cf_def_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  cf_recvbuf_ਅੰਕੜੇ_pending,
  Curl_cf_def_send,
  cf_recvbuf_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ਸਥਿਰ CURLcode cf_recvbuf_create(ਬਣਤਰ Curl_cfilter **pcf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ uint8_t *buf, size_t blen)
{
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  ਬਣਤਰ cf_recvbuf_ctx *ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t nwritten = 0;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_ਹੱਦਬੰਦੀ | BUFQ_OPT_NO_SPARES));
  ਨਤੀਜਾ = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਜੇ(nwritten != blen) {
    ਨਤੀਜਾ = CURLE_FAILED_INIT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ctx = ਨਿਰਾ;

ਬਾਹਰਲਾ:
  *pcf = ਨਤੀਜਾ ? ਨਿਰਾ : cf;
  ਜੇ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_recvbuf_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ connectdata *conn,
                             int8_t sockindex,
                             ਅਚਲ uint8_t *buf, size_t blen)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(ਅੰਕੜੇ);
  ਨਤੀਜਾ = cf_recvbuf_create(&cf, ਅੰਕੜੇ, buf, blen);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_ਜੋੜੋ(ਅੰਕੜੇ, conn, sockindex, cf);
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_WEBSOCKETS */
