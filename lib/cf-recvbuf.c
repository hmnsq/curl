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

#ifndef CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#شامل "urldata.h"
#شامل "bufq.h"
#شامل "cfilters.h"
#شامل "cf-recvbuf.h"
#شامل "curl_trc.h"

#define CURL_CF_RECVBUF_CHUNK     (16 * 1024)

ساختار cf_recvbuf_ctx {
  ساختار bufq recvbuf;
};

ایستا تهی cf_recvbuf_destroy(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data)
{
  ساختار cf_recvbuf_ctx *ctx = cf->ctx;
  (تهی)data;
  اگر(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

ایستا CURLcode cf_recvbuf_recv(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                نویسه *buf, size_t len,
                                size_t *pnread)
{
  ساختار cf_recvbuf_ctx *ctx = cf->ctx;

  اگر(!Curl_bufq_is_empty(&ctx->recvbuf)) {
    بازگشت Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  اگر(cf->next)
    بازگشت cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  بازگشت CURLE_RECV_ERROR;
}

ایستا bool cf_recvbuf_data_pending(ساختار Curl_cfilter *cf,
                                    ثابت ساختار Curl_easy *data)
{
  ساختار cf_recvbuf_ctx *ctx = cf->ctx;

  اگر(!Curl_bufq_is_empty(&ctx->recvbuf))
    بازگشت TRUE;

  بازگشت cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ساختار Curl_cftype Curl_cft_recvbuf = {
  "RECVBUF",
  0,
  CURL_LOG_LVL_NONE,
  cf_recvbuf_destroy,
  Curl_cf_def_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  cf_recvbuf_data_pending,
  Curl_cf_def_send,
  cf_recvbuf_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ایستا CURLcode cf_recvbuf_create(ساختار Curl_cfilter **pcf,
                                  ساختار Curl_easy *data,
                                  ثابت uint8_t *buf, size_t blen)
{
  ساختار Curl_cfilter *cf = پوچ;
  ساختار cf_recvbuf_ctx *ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten = 0;

  (تهی)data;
  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
  result = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  اگر(result)
    برو out;
  اگر(nwritten != blen) {
    result = CURLE_FAILED_INIT;
    برو out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  اگر(result)
    برو out;
  ctx = پوچ;

out:
  *pcf = result ? پوچ : cf;
  اگر(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  بازگشت result;
}

CURLcode Curl_cf_recvbuf_add(ساختار Curl_easy *data,
                             ساختار connectdata *conn,
                             int8_t sockindex,
                             ثابت uint8_t *buf, size_t blen)
{
  ساختار Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_recvbuf_create(&cf, data, buf, blen);
  اگر(result)
    برو out;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  بازگشت result;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
