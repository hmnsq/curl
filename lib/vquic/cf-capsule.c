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

#ఒకవేళ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "bufq.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "vquic/capsule.h"
#అన్తర్భావ "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#నిర్వచించు CAPSULE_RECV_CHUNKS    4
#నిర్వచించు CAPSULE_SEND_CHUNKS    4
#నిర్వచించు CAPSULE_CHUNK_కొలత     (16 * 1024)

నిర్మాణం cf_capsule_ctx {
  నిర్మాణం bufq recvbuf;
  నిర్మాణం bufq sendbuf;
};

స్థిర శూన్య cf_capsule_destroy(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_capsule_ctx *ctx = cf->ctx;
  (శూన్య)దత్తాంశం;
  ఒకవేళ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

స్థిర CURLcode cf_capsule_connect(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   bool *పూర్తి)
{
  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }
  ఒకవేళ(cf->తదుపరి) {
    CURLcode ఫలితం = cf->తదుపరి->cft->do_connect(cf->తదుపరి, దత్తాంశం, పూర్తి);
    ఒకవేళ(!ఫలితం && *పూర్తి)
      cf->connected = TRUE;
    తిరిగి ఫలితం;
  }
  *పూర్తి = FALSE;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_capsule_flush(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_capsule_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;
  size_t nwritten;

  ఒకవేళ(Curl_bufq_is_ఖాళీలు(&ctx->sendbuf))
    తిరిగి CURLE_OK;

  ఫలితం = Curl_cf_send_bufq(cf->తదుపరి, దత్తాంశం, &ctx->sendbuf, ఖాళీ, 0,
                             &nwritten);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      CURL_TRC_CF(దత్తాంశం, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    తిరిగి ఫలితం;
  }
  తిరిగి Curl_bufq_is_ఖాళీలు(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

స్థిర CURLcode cf_capsule_send(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                స్థిరరాశి uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  నిర్మాణం cf_capsule_ctx *ctx = cf->ctx;
  CURLcode ఫలితం;

  (శూన్య)eos;
  *pnwritten = 0;

  ఒకవేళ(Curl_bufq_is_నిండిన(&ctx->sendbuf)) {
    ఫలితం = cf_capsule_flush(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  /* encapsulate new payload into a capsule */
  ఫలితం = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = cf_capsule_flush(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    ఫలితం = CURLE_OK;
  }

  ఒకవేళ(!ఫలితం)
    *pnwritten = len;
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_capsule_recv(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                అక్షరం *buf, size_t len,
                                size_t *pnread)
{
  నిర్మాణం cf_capsule_ctx *ctx = cf->ctx;
  CURLcode ఫలితం;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  అయితే(!Curl_bufq_is_నిండిన(&ctx->recvbuf)) {
    ఫలితం = Curl_cf_recv_bufq(cf->తదుపరి, దత్తాంశం, &ctx->recvbuf, 0, &nread);
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      ఆపు;
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఒకవేళ(!nread)
      ఆపు;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, దత్తాంశం, &ctx->recvbuf,
                                         (గుర్తులేని అక్షరం *)buf, len,
                                         &ఫలితం);
  తిరిగి ఫలితం;
}

స్థిర bool cf_capsule_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                                    స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_capsule_ctx *ctx = cf->ctx;

  ఒకవేళ(ctx && !Curl_bufq_is_ఖాళీలు(&ctx->recvbuf))
    తిరిగి TRUE;
  తిరిగి cf->తదుపరి ? cf->తదుపరి->cft->has_దత్తాంశం_pending(cf->తదుపరి, దత్తాంశం) : FALSE;
}

స్థిర CURLcode cf_capsule_cntrl(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)arg1;
  (శూన్య)arg2;
  ఎంపిక(event) {
  స్థితి CF_CTRL_FLUSH:
    ఫలితం = cf_capsule_flush(cf, దత్తాంశం);
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_capsule_query(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  నిర్మాణం cf_capsule_ctx *ctx = cf->ctx;

  (శూన్య)pres2;
  ఎంపిక(query) {
  స్థితి CF_QUERY_NEED_FLUSH: {
    ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->sendbuf)) {
      *pres1 = TRUE;
      తిరిగి CURLE_OK;
    }
    ఆపు;
  }
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->query(cf->తదుపరి, దత్తాంశం, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

స్థిర CURLcode cf_capsule_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                          నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం cf_capsule_ctx *ctx = cf->ctx;

  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, దత్తాంశం);
    ఒకవేళ(sock != CURL_SOCKET_BAD)
      తిరిగి Curl_pollset_కూడు_బహిర్గతం(దత్తాంశం, ps, sock);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_capsule_shutdown(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!cf->connected || cf->shutdown) {
    *పూర్తి = TRUE;
  }
  లేకపోతే {
    ఫలితం = cf_capsule_flush(cf, దత్తాంశం);
    *పూర్తి = !ఫలితం;
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      ఫలితం = CURLE_OK;
  }
  తిరిగి ఫలితం;
}

నిర్మాణం Curl_cftype Curl_cft_capsule = {
  "CAPSULE",
  0,
  0,
  cf_capsule_destroy,
  cf_capsule_connect,
  cf_capsule_shutdown,
  cf_capsule_adjust_pollset,
  cf_capsule_దత్తాంశం_pending,
  cf_capsule_send,
  cf_capsule_recv,
  cf_capsule_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_capsule_query,
};

స్థిర CURLcode cf_capsule_create(నిర్మాణం Curl_cfilter **pcf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn)
{
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  నిర్మాణం cf_capsule_ctx *ctx;
  CURLcode ఫలితం;

  (శూన్య)దత్తాంశం;
  (శూన్య)conn;
  *pcf = ఖాళీ;
  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_కొలత, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_హద్దు);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_కొలత, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_హద్దు);

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

బహిర్గతం:
  *pcf = (!ఫలితం) ? cf : ఖాళీ;
  ఒకవేళ(ఫలితం && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  తిరిగి ఫలితం;
}

CURLcode Curl_cf_capsule_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం;

  ఫలితం = cf_capsule_create(&cf, దత్తాంశం, cf_at->conn);
  ఒకవేళ(!ఫలితం)
    Curl_conn_cf_చొప్పించు_after(cf_at, cf);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
