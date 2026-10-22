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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "bufq.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "cf-recvbuf.h"
#அந்தர்பாவ "curl_trc.h"

#define CURL_CF_RECVBUF_CHUNK     (16 * 1024)

கட்டமைப்பு cf_recvbuf_ctx {
  கட்டமைப்பு bufq recvbuf;
};

நிலையான வெறுமை cf_recvbuf_destroy(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_recvbuf_ctx *ctx = cf->ctx;
  (வெறுமை)data;
  எனில்(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

நிலையான CURLcode cf_recvbuf_recv(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                எழுத்து *buf, size_t len,
                                size_t *pnread)
{
  கட்டமைப்பு cf_recvbuf_ctx *ctx = cf->ctx;

  எனில்(!Curl_bufq_is_empty(&ctx->recvbuf)) {
    திரும்பு Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  எனில்(cf->next)
    திரும்பு cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  திரும்பு CURLE_RECV_ERROR;
}

நிலையான bool cf_recvbuf_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                                    மாறிலி கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_recvbuf_ctx *ctx = cf->ctx;

  எனில்(!Curl_bufq_is_empty(&ctx->recvbuf))
    திரும்பு TRUE;

  திரும்பு cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

கட்டமைப்பு Curl_cftype Curl_cft_recvbuf = {
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

நிலையான CURLcode cf_recvbuf_create(கட்டமைப்பு Curl_cfilter **pcf,
                                  கட்டமைப்பு Curl_easy *data,
                                  மாறிலி uint8_t *buf, size_t blen)
{
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  கட்டமைப்பு cf_recvbuf_ctx *ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten = 0;

  (வெறுமை)data;
  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
  result = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  எனில்(result)
    செல் out;
  எனில்(nwritten != blen) {
    result = CURLE_FAILED_INIT;
    செல் out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  எனில்(result)
    செல் out;
  ctx = வெற்று;

out:
  *pcf = result ? வெற்று : cf;
  எனில்(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  திரும்பு result;
}

CURLcode Curl_cf_recvbuf_add(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு connectdata *conn,
                             int8_t sockindex,
                             மாறிலி uint8_t *buf, size_t blen)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_recvbuf_create(&cf, data, buf, blen);
  எனில்(result)
    செல் out;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  திரும்பு result;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
