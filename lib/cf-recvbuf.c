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

#اگر_تعریف_نشده CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#ضمن "urldata.h"
#ضمن "bufq.h"
#ضمن "cfilters.h"
#ضمن "cf-recvbuf.h"
#ضمن "curl_trc.h"

#تعریف_کن CURL_CF_RECVBUF_CHUNK     (16 * 1024)

بنية cf_recvbuf_ctx {
  بنية bufq recvbuf;
};

ساكن فراغ cf_recvbuf_destroy(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده)
{
  بنية cf_recvbuf_ctx *ctx = cf->ctx;
  (فراغ)داده;
  إذا(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

ساكن CURLcode cf_recvbuf_recv(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                حرف *buf, size_t len,
                                size_t *pnread)
{
  بنية cf_recvbuf_ctx *ctx = cf->ctx;

  إذا(!Curl_bufq_is_خالی(&ctx->recvbuf)) {
    أرجع Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  إذا(cf->بعدی)
    أرجع cf->بعدی->cft->do_recv(cf->بعدی, داده, buf, len, pnread);
  *pnread = 0;
  أرجع CURLE_RECV_ERROR;
}

ساكن bool cf_recvbuf_داده_pending(بنية Curl_cfilter *cf,
                                    ثابت بنية Curl_easy *داده)
{
  بنية cf_recvbuf_ctx *ctx = cf->ctx;

  إذا(!Curl_bufq_is_خالی(&ctx->recvbuf))
    أرجع TRUE;

  أرجع cf->بعدی ?
    cf->بعدی->cft->has_داده_pending(cf->بعدی, داده) : FALSE;
}

بنية Curl_cftype Curl_cft_recvbuf = {
  "RECVBUF",
  0,
  CURL_LOG_LVL_NONE,
  cf_recvbuf_destroy,
  Curl_cf_def_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  cf_recvbuf_داده_pending,
  Curl_cf_def_send,
  cf_recvbuf_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساكن CURLcode cf_recvbuf_create(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *داده,
                                  ثابت uint8_t *buf, size_t blen)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_recvbuf_ctx *ctx;
  CURLcode نتیجه = CURLE_OK;
  size_t nwritten = 0;

  (فراغ)داده;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_حد | BUFQ_OPT_NO_SPARES));
  نتیجه = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  إذا(نتیجه)
    اذهب بیرون;
  إذا(nwritten != blen) {
    نتیجه = CURLE_FAILED_INIT;
    اذهب بیرون;
  }

  نتیجه = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  إذا(نتیجه)
    اذهب بیرون;
  ctx = عدم;

بیرون:
  *pcf = نتیجه ? عدم : cf;
  إذا(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  أرجع نتیجه;
}

CURLcode Curl_cf_recvbuf_جمع_کن(بنية Curl_easy *داده,
                             بنية connectdata *conn,
                             int8_t sockindex,
                             ثابت uint8_t *buf, size_t blen)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(داده);
  نتیجه = cf_recvbuf_create(&cf, داده, buf, blen);
  إذا(نتیجه)
    اذهب بیرون;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_جمع_کن(داده, conn, sockindex, cf);
بیرون:
  أرجع نتیجه;
}

#پایان_اگر /* !CURL_DISABLE_WEBSOCKETS */
