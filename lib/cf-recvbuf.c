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

#ifndef CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#ضمن "urldata.h"
#ضمن "bufq.h"
#ضمن "cfilters.h"
#ضمن "cf-recvbuf.h"
#ضمن "curl_trc.h"

#define CURL_CF_RECVBUF_CHUNK     (16 * 1024)

بنية cf_recvbuf_ctx {
  بنية bufq recvbuf;
};

ساكن فراغ cf_recvbuf_destroy(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data)
{
  بنية cf_recvbuf_ctx *ctx = cf->ctx;
  (فراغ)data;
  إذا(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

ساكن CURLcode cf_recvbuf_recv(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                حرف *buf, size_t len,
                                size_t *pnread)
{
  بنية cf_recvbuf_ctx *ctx = cf->ctx;

  إذا(!Curl_bufq_is_empty(&ctx->recvbuf)) {
    أرجع Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  إذا(cf->next)
    أرجع cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  أرجع CURLE_RECV_ERROR;
}

ساكن bool cf_recvbuf_data_pending(بنية Curl_cfilter *cf,
                                    ثابت بنية Curl_easy *data)
{
  بنية cf_recvbuf_ctx *ctx = cf->ctx;

  إذا(!Curl_bufq_is_empty(&ctx->recvbuf))
    أرجع TRUE;

  أرجع cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

بنية Curl_cftype Curl_cft_recvbuf = {
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

ساكن CURLcode cf_recvbuf_create(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *data,
                                  ثابت uint8_t *buf, size_t blen)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_recvbuf_ctx *ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten = 0;

  (فراغ)data;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
  result = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  إذا(result)
    اذهب out;
  إذا(nwritten != blen) {
    result = CURLE_FAILED_INIT;
    اذهب out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  إذا(result)
    اذهب out;
  ctx = عدم;

out:
  *pcf = result ? عدم : cf;
  إذا(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  أرجع result;
}

CURLcode Curl_cf_recvbuf_add(بنية Curl_easy *data,
                             بنية connectdata *conn,
                             int8_t sockindex,
                             ثابت uint8_t *buf, size_t blen)
{
  بنية Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_recvbuf_create(&cf, data, buf, blen);
  إذا(result)
    اذهب out;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  أرجع result;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
