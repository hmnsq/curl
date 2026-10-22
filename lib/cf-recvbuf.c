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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_WEBSOCKETS
/* only used for this protocol, so far */

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "bufq.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cf-recvbuf.h"
#అన్తర్భావ "curl_trc.h"

#నిర్వచించు CURL_CF_RECVBUF_CHUNK     (16 * 1024)

నిర్మాణం cf_recvbuf_ctx {
  నిర్మాణం bufq recvbuf;
};

స్థిర శూన్య cf_recvbuf_destroy(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_recvbuf_ctx *ctx = cf->ctx;
  (శూన్య)దత్తాంశం;
  ఒకవేళ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
}

స్థిర CURLcode cf_recvbuf_recv(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                అక్షరం *buf, size_t len,
                                size_t *pnread)
{
  నిర్మాణం cf_recvbuf_ctx *ctx = cf->ctx;

  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->recvbuf)) {
    తిరిగి Curl_bufq_cread(&ctx->recvbuf, buf, len, pnread);
  }

  ఒకవేళ(cf->తదుపరి)
    తిరిగి cf->తదుపరి->cft->do_recv(cf->తదుపరి, దత్తాంశం, buf, len, pnread);
  *pnread = 0;
  తిరిగి CURLE_RECV_ERROR;
}

స్థిర bool cf_recvbuf_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                                    స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_recvbuf_ctx *ctx = cf->ctx;

  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->recvbuf))
    తిరిగి TRUE;

  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->has_దత్తాంశం_pending(cf->తదుపరి, దత్తాంశం) : FALSE;
}

నిర్మాణం Curl_cftype Curl_cft_recvbuf = {
  "RECVBUF",
  0,
  CURL_LOG_LVL_NONE,
  cf_recvbuf_destroy,
  Curl_cf_def_connect,
  Curl_cf_def_shutdown,
  Curl_cf_def_adjust_pollset,
  cf_recvbuf_దత్తాంశం_pending,
  Curl_cf_def_send,
  cf_recvbuf_recv,
  Curl_cf_def_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  Curl_cf_def_query,
};

స్థిర CURLcode cf_recvbuf_create(నిర్మాణం Curl_cfilter **pcf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి uint8_t *buf, size_t blen)
{
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  నిర్మాణం cf_recvbuf_ctx *ctx;
  CURLcode ఫలితం = CURLE_OK;
  size_t nwritten = 0;

  (శూన్య)దత్తాంశం;
  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  Curl_bufq_init2(&ctx->recvbuf, CURL_CF_RECVBUF_CHUNK,
                  (blen / CURL_CF_RECVBUF_CHUNK) + 1,
                  (BUFQ_OPT_SOFT_హద్దు | BUFQ_OPT_NO_SPARES));
  ఫలితం = Curl_bufq_write(&ctx->recvbuf, buf, blen, &nwritten);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ఒకవేళ(nwritten != blen) {
    ఫలితం = CURLE_FAILED_INIT;
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_recvbuf, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ctx = ఖాళీ;

బహిర్గతం:
  *pcf = ఫలితం ? ఖాళీ : cf;
  ఒకవేళ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    curlx_free(ctx);
  }
  తిరిగి ఫలితం;
}

CURLcode Curl_cf_recvbuf_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం connectdata *conn,
                             int8_t sockindex,
                             స్థిరరాశి uint8_t *buf, size_t blen)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(దత్తాంశం);
  ఫలితం = cf_recvbuf_create(&cf, దత్తాంశం, buf, blen);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  cf->connected = Curl_conn_is_connected(conn, sockindex);
  Curl_conn_cf_కూడు(దత్తాంశం, conn, sockindex, cf);
బహిర్గతం:
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !CURL_DISABLE_WEBSOCKETS */
