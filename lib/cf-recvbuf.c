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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "bufq.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-recvbuf.h"
#অন্তর্ভাব "curl_trc.h"

#define CURL_CF_RECVBUF_CHUNK     (16 * 1024)

কাঠামো cf_recvbuf_ctx {
  কাঠামো bufq recvbuf;
};

স্থির শূন্য cf_recvbuf_destroy(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_recvbuf_ctx *ctx = cf->ctx;
  (শূন্য)তথ্য;
  যদি(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

স্থির CURLcode cf_recvbuf_recv(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                অক্ষর *buf, size_t len,
                                size_t *pnread)
{
  কাঠামো cf_recvbuf_ctx *ctx = cf->ctx;

  যদি(!Curl_bufq_is_empty(&ctx->recvbuf)) {
    ফেরত Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  যদি(cf->পরবর্তী)
    ফেরত cf->পরবর্তী->cft->do_recv(cf->পরবর্তী, তথ্য, buf, len, pnread);
  *pnread = 0;
  ফেরত CURLE_RECV_ERROR;
}

স্থির bool cf_recvbuf_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                                    ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_recvbuf_ctx *ctx = cf->ctx;

  যদি(!Curl_bufq_is_empty(&ctx->recvbuf))
    ফেরত TRUE;

  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->has_তথ্য_pending(cf->পরবর্তী, তথ্য) : FALSE;
}

কাঠামো Curl_cftype Curl_cft_recvbuf = {
  "RECVBUF",
  0,
  CURL_LOG_LVL_NONE,
  cf_recvbuf_destroy,
  Curl_cf_def_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  cf_recvbuf_তথ্য_pending,
  Curl_cf_def_send,
  cf_recvbuf_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

স্থির CURLcode cf_recvbuf_create(কাঠামো Curl_cfilter **pcf,
                                  কাঠামো Curl_easy *তথ্য,
                                  ধ্রুবক uint8_t *buf, size_t blen)
{
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  কাঠামো cf_recvbuf_ctx *ctx;
  CURLcode ফলাফল = CURLE_OK;
  size_t nwritten = 0;

  (শূন্য)তথ্য;
  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_LIMIT | BUFQ_OPT_NO_SPARES));
  ফলাফল = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  যদি(ফলাফল)
    যাও out;
  যদি(nwritten != blen) {
    ফলাফল = CURLE_FAILED_INIT;
    যাও out;
  }

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  যদি(ফলাফল)
    যাও out;
  ctx = ফাঁকা;

out:
  *pcf = ফলাফল ? ফাঁকা : cf;
  যদি(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  ফেরত ফলাফল;
}

CURLcode Curl_cf_recvbuf_add(কাঠামো Curl_easy *তথ্য,
                             কাঠামো connectdata *conn,
                             int8_t sockindex,
                             ধ্রুবক uint8_t *buf, size_t blen)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(তথ্য);
  ফলাফল = cf_recvbuf_create(&cf, তথ্য, buf, blen);
  যদি(ফলাফল)
    যাও out;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_add(তথ্য, conn, sockindex, cf);
out:
  ফেরত ফলাফল;
}

#endif /* !CURL_DISABLE_WEBSOCKETS */
