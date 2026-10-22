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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "bufq.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "cf-recvbuf.h"
#અન્તર્ભાવ "curl_trc.h"

#define CURL_CF_RECVBUF_CHUNK     (16 * 1024)

સંરચના cf_recvbuf_ctx {
  સંરચના bufq recvbuf;
};

સ્થિર શૂન્ય cf_recvbuf_destroy(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data)
{
  સંરચના cf_recvbuf_ctx *ctx = cf->ctx;
  (શૂન્ય)data;
  જો(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

સ્થિર CURLcode cf_recvbuf_recv(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                અક્ષર *buf, size_t len,
                                size_t *pnread)
{
  સંરચના cf_recvbuf_ctx *ctx = cf->ctx;

  જો(!Curl_bufq_is_empty(&ctx->recvbuf)) {
    પાછા Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  જો(cf->next)
    પાછા cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  પાછા CURLE_RECV_ERROR;
}

સ્થિર bool cf_recvbuf_data_pending(સંરચના Curl_cfilter *cf,
                                    અચલ સંરચના Curl_easy *data)
{
  સંરચના cf_recvbuf_ctx *ctx = cf->ctx;

  જો(!Curl_bufq_is_empty(&ctx->recvbuf))
    પાછા TRUE;

  પાછા cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

સંરચના Curl_cftype Curl_cft_recvbuf = {
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

સ્થિર CURLcode cf_recvbuf_create(સંરચના Curl_cfilter **pcf,
                                  સંરચના Curl_easy *data,
                                  અચલ uint8_t *buf, size_t blen)
{
  સંરચના Curl_cfilter *cf = ખાલી;
  સંરચના cf_recvbuf_ctx *ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten = 0;

  (શૂન્ય)data;
  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
  result = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  જો(result)
    જાઓ out;
  જો(nwritten != blen) {
    result = CURLE_FAILED_INIT;
    જાઓ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  જો(result)
    જાઓ out;
  ctx = ખાલી;

out:
  *pcf = result ? ખાલી : cf;
  જો(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  પાછા result;
}

CURLcode Curl_cf_recvbuf_add(સંરચના Curl_easy *data,
                             સંરચના connectdata *conn,
                             int8_t sockindex,
                             અચલ uint8_t *buf, size_t blen)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_recvbuf_create(&cf, data, buf, blen);
  જો(result)
    જાઓ out;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  પાછા result;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
