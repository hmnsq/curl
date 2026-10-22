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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "bufq.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "cf-recvbuf.h"
#അന്തര്ഭാവ "curl_trc.h"

#define CURL_CF_RECVBUF_CHUNK     (16 * 1024)

ഘടന cf_recvbuf_ctx {
  ഘടന bufq recvbuf;
};

സ്ഥിതം ശൂന്യം cf_recvbuf_destroy(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data)
{
  ഘടന cf_recvbuf_ctx *ctx = cf->ctx;
  (ശൂന്യം)data;
  എങ്കിൽ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

സ്ഥിതം CURLcode cf_recvbuf_recv(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                അക്ഷരം *buf, size_t len,
                                size_t *pnread)
{
  ഘടന cf_recvbuf_ctx *ctx = cf->ctx;

  എങ്കിൽ(!Curl_bufq_is_empty(&ctx->recvbuf)) {
    തിരികെ Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  എങ്കിൽ(cf->next)
    തിരികെ cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  തിരികെ CURLE_RECV_ERROR;
}

സ്ഥിതം bool cf_recvbuf_data_pending(ഘടന Curl_cfilter *cf,
                                    സ്ഥിരം ഘടന Curl_easy *data)
{
  ഘടന cf_recvbuf_ctx *ctx = cf->ctx;

  എങ്കിൽ(!Curl_bufq_is_empty(&ctx->recvbuf))
    തിരികെ TRUE;

  തിരികെ cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ഘടന Curl_cftype Curl_cft_recvbuf = {
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

സ്ഥിതം CURLcode cf_recvbuf_create(ഘടന Curl_cfilter **pcf,
                                  ഘടന Curl_easy *data,
                                  സ്ഥിരം uint8_t *buf, size_t blen)
{
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  ഘടന cf_recvbuf_ctx *ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten = 0;

  (ശൂന്യം)data;
  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
  result = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  എങ്കിൽ(result)
    പോകുക out;
  എങ്കിൽ(nwritten != blen) {
    result = CURLE_FAILED_INIT;
    പോകുക out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  എങ്കിൽ(result)
    പോകുക out;
  ctx = ശൂന്യ;

out:
  *pcf = result ? ശൂന്യ : cf;
  എങ്കിൽ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  തിരികെ result;
}

CURLcode Curl_cf_recvbuf_add(ഘടന Curl_easy *data,
                             ഘടന connectdata *conn,
                             int8_t sockindex,
                             സ്ഥിരം uint8_t *buf, size_t blen)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_recvbuf_create(&cf, data, buf, blen);
  എങ്കിൽ(result)
    പോകുക out;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  തിരികെ result;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
