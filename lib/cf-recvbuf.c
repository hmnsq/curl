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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "bufq.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "cf-recvbuf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"

#define CURL_CF_RECVBUF_CHUNK     (16 * 1024)

ସଂରଚନା cf_recvbuf_ctx {
  ସଂରଚନା bufq recvbuf;
};

ସ୍ଥିର ଶୂନ୍ୟ cf_recvbuf_destroy(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_recvbuf_ctx *ctx = cf->ctx;
  (ଶୂନ୍ୟ)data;
  ଯଦି(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

ସ୍ଥିର CURLcode cf_recvbuf_recv(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ଅକ୍ଷର *buf, size_t len,
                                size_t *pnread)
{
  ସଂରଚନା cf_recvbuf_ctx *ctx = cf->ctx;

  ଯଦି(!Curl_bufq_is_empty(&ctx->recvbuf)) {
    ଫେରନ୍ତୁ Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  ଯଦି(cf->next)
    ଫେରନ୍ତୁ cf->next->cft->do_recv(cf->next, data, buf, len, pnread);
  *pnread = 0;
  ଫେରନ୍ତୁ CURLE_RECV_ERROR;
}

ସ୍ଥିର bool cf_recvbuf_data_pending(ସଂରଚନା Curl_cfilter *cf,
                                    ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_recvbuf_ctx *ctx = cf->ctx;

  ଯଦି(!Curl_bufq_is_empty(&ctx->recvbuf))
    ଫେରନ୍ତୁ TRUE;

  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ସଂରଚନା Curl_cftype Curl_cft_recvbuf = {
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

ସ୍ଥିର CURLcode cf_recvbuf_create(ସଂରଚନା Curl_cfilter **pcf,
                                  ସଂରଚନା Curl_easy *data,
                                  ଅଚଳ uint8_t *buf, size_t blen)
{
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  ସଂରଚନା cf_recvbuf_ctx *ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten = 0;

  (ଶୂନ୍ୟ)data;
  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
  result = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  ଯଦି(result)
    ଯାଅ out;
  ଯଦି(nwritten != blen) {
    result = CURLE_FAILED_INIT;
    ଯାଅ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  ଯଦି(result)
    ଯାଅ out;
  ctx = ଖାଲି;

out:
  *pcf = result ? ଖାଲି : cf;
  ଯଦି(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cf_recvbuf_add(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା connectdata *conn,
                             int8_t sockindex,
                             ଅଚଳ uint8_t *buf, size_t blen)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_recvbuf_create(&cf, data, buf, blen);
  ଯଦି(result)
    ଯାଅ out;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_add(data, conn, sockindex, cf);
out:
  ଫେରନ୍ତୁ result;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
