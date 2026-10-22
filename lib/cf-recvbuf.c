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

#که_نه_تعریف_شوی CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#ضمن "urldata.h"
#ضمن "bufq.h"
#ضمن "cfilters.h"
#ضمن "cf-recvbuf.h"
#ضمن "curl_trc.h"

#تعریف_کړه CURL_CF_RECVBUF_CHUNK     (16 * 1024)

بنية cf_recvbuf_ctx {
  بنية bufq recvbuf;
};

ساكن فراغ cf_recvbuf_destroy(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات)
{
  بنية cf_recvbuf_ctx *ctx = cf->ctx;
  (فراغ)معلومات;
  إذا(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

ساكن CURLcode cf_recvbuf_recv(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                حرف *buf, size_t len,
                                size_t *pnread)
{
  بنية cf_recvbuf_ctx *ctx = cf->ctx;

  إذا(!Curl_bufq_is_تشه(&ctx->recvbuf)) {
    أرجع Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  إذا(cf->بل)
    أرجع cf->بل->cft->do_recv(cf->بل, معلومات, buf, len, pnread);
  *pnread = 0;
  أرجع CURLE_RECV_ERROR;
}

ساكن bool cf_recvbuf_معلومات_pending(بنية Curl_cfilter *cf,
                                    ثابت بنية Curl_easy *معلومات)
{
  بنية cf_recvbuf_ctx *ctx = cf->ctx;

  إذا(!Curl_bufq_is_تشه(&ctx->recvbuf))
    أرجع TRUE;

  أرجع cf->بل ?
    cf->بل->cft->has_معلومات_pending(cf->بل, معلومات) : FALSE;
}

بنية Curl_cftype Curl_cft_recvbuf = {
  "RECVBUF",
  0,
  CURL_LOG_LVL_NONE,
  cf_recvbuf_destroy,
  Curl_cf_def_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  cf_recvbuf_معلومات_pending,
  Curl_cf_def_send,
  cf_recvbuf_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

ساكن CURLcode cf_recvbuf_create(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *معلومات,
                                  ثابت uint8_t *buf, size_t blen)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_recvbuf_ctx *ctx;
  CURLcode پایله = CURLE_OK;
  size_t nwritten = 0;

  (فراغ)معلومات;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_حد | BUFQ_OPT_NO_SPARES));
  پایله = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  إذا(پایله)
    اذهب بهر;
  إذا(nwritten != blen) {
    پایله = CURLE_FAILED_INIT;
    اذهب بهر;
  }

  پایله = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  إذا(پایله)
    اذهب بهر;
  ctx = عدم;

بهر:
  *pcf = پایله ? عدم : cf;
  إذا(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  أرجع پایله;
}

CURLcode Curl_cf_recvbuf_جمع_کړه(بنية Curl_easy *معلومات,
                             بنية connectdata *conn,
                             int8_t sockindex,
                             ثابت uint8_t *buf, size_t blen)
{
  بنية Curl_cfilter *cf;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(معلومات);
  پایله = cf_recvbuf_create(&cf, معلومات, buf, blen);
  إذا(پایله)
    اذهب بهر;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_جمع_کړه(معلومات, conn, sockindex, cf);
بهر:
  أرجع پایله;
}

#د_که_پای /* !CURL_DISABLE_WEBSOCKETS */
