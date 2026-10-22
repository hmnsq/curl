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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#شامل "urldata.h"
#شامل "bufq.h"
#شامل "cfilters.h"
#شامل "cf-recvbuf.h"
#شامل "curl_trc.h"

#تعریف_کرو CURL_CF_RECVBUF_CHUNK     (16 * 1024)

ساخت cf_recvbuf_ctx {
  ساخت bufq recvbuf;
};

ساکن خالی cf_recvbuf_destroy(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_recvbuf_ctx *ctx = cf->ctx;
  (خالی)ڈیٹا;
  اگر(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

ساکن CURLcode cf_recvbuf_recv(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                حرف *buf, size_t len,
                                size_t *pnread)
{
  ساخت cf_recvbuf_ctx *ctx = cf->ctx;

  اگر(!Curl_bufq_is_خالی_خانے(&ctx->recvbuf)) {
    واپس Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  اگر(cf->اگلا)
    واپس cf->اگلا->cft->do_recv(cf->اگلا, ڈیٹا, buf, len, pnread);
  *pnread = 0;
  واپس CURLE_RECV_ERROR;
}

ساکن bool cf_recvbuf_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                                    ثابت ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_recvbuf_ctx *ctx = cf->ctx;

  اگر(!Curl_bufq_is_خالی_خانے(&ctx->recvbuf))
    واپس TRUE;

  واپس cf->اگلا ?
    cf->اگلا->cft->has_ڈیٹا_pending(cf->اگلا, ڈیٹا) : FALSE;
}

ساخت Curl_cftype Curl_cft_recvbuf = {
  "RECVBUF",
  0,
  CURL_LOG_LVL_NONE,
  cf_recvbuf_destroy,
  Curl_cf_def_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  cf_recvbuf_ڈیٹا_pending,
  Curl_cf_def_send,
  cf_recvbuf_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساکن CURLcode cf_recvbuf_create(ساخت Curl_cfilter **pcf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ثابت uint8_t *buf, size_t blen)
{
  ساخت Curl_cfilter *cf = تہی;
  ساخت cf_recvbuf_ctx *ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nwritten = 0;

  (خالی)ڈیٹا;
  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_حد_بندی | BUFQ_OPT_NO_SPARES));
  نتیجہ = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  اگر(نتیجہ)
    جاؤ برآمد;
  اگر(nwritten != blen) {
    نتیجہ = CURLE_FAILED_INIT;
    جاؤ برآمد;
  }

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;
  ctx = تہی;

برآمد:
  *pcf = نتیجہ ? تہی : cf;
  اگر(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  واپس نتیجہ;
}

CURLcode Curl_cf_recvbuf_جوڑو(ساخت Curl_easy *ڈیٹا,
                             ساخت connectdata *conn,
                             int8_t sockindex,
                             ثابت uint8_t *buf, size_t blen)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(ڈیٹا);
  نتیجہ = cf_recvbuf_create(&cf, ڈیٹا, buf, blen);
  اگر(نتیجہ)
    جاؤ برآمد;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_جوڑو(ڈیٹا, conn, sockindex, cf);
برآمد:
  واپس نتیجہ;
}

#اگر_ختم /* !CURL_DISABLE_WEBSOCKETS */
