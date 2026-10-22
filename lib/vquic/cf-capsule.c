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

#ଯଦି !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "bufq.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "vquic/capsule.h"
#ଅନ୍ତର୍ଭାଵ "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#define CAPSULE_RECV_CHUNKS    4
#define CAPSULE_SEND_CHUNKS    4
#define CAPSULE_CHUNK_SIZE     (16 * 1024)

ସଂରଚନା cf_capsule_ctx {
  ସଂରଚନା bufq recvbuf;
  ସଂରଚନା bufq sendbuf;
};

ସ୍ଥିର ଶୂନ୍ୟ cf_capsule_destroy(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_capsule_ctx *ctx = cf->ctx;
  (ଶୂନ୍ୟ)data;
  ଯଦି(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

ସ୍ଥିର CURLcode cf_capsule_connect(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   bool *done)
{
  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଯଦି(cf->next) {
    CURLcode result = cf->next->cft->do_connect(cf->next, data, done);
    ଯଦି(!result && *done)
      cf->connected = TRUE;
    ଫେରନ୍ତୁ result;
  }
  *done = FALSE;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_capsule_flush(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  ଯଦି(Curl_bufq_is_empty(&ctx->sendbuf))
    ଫେରନ୍ତୁ CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->sendbuf, ଖାଲି, 0,
                             &nwritten);
  ଯଦି(result) {
    ଯଦି(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

ସ୍ଥିର CURLcode cf_capsule_send(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ଅଚଳ uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  ସଂରଚନା cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;

  (ଶୂନ୍ୟ)eos;
  *pnwritten = 0;

  ଯଦି(Curl_bufq_is_full(&ctx->sendbuf)) {
    result = cf_capsule_flush(cf, data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  /* encapsulate new payload into a capsule */
  result = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  result = cf_capsule_flush(cf, data);
  ଯଦି(result == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    result = CURLE_OK;
  }

  ଯଦି(!result)
    *pnwritten = len;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_capsule_recv(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ଅକ୍ଷର *buf, size_t len,
                                size_t *pnread)
{
  ସଂରଚନା cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  ଯେତେବେଳେ(!Curl_bufq_is_full(&ctx->recvbuf)) {
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->recvbuf, 0, &nread);
    ଯଦି(result == CURLE_AGAIN)
      ଭାଙ୍ଗ;
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ଯଦି(!nread)
      ଭାଙ୍ଗ;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, data, &ctx->recvbuf,
                                         (unsigned ଅକ୍ଷର *)buf, len,
                                         &result);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool cf_capsule_data_pending(ସଂରଚନା Curl_cfilter *cf,
                                    ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_capsule_ctx *ctx = cf->ctx;

  ଯଦି(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    ଫେରନ୍ତୁ TRUE;
  ଫେରନ୍ତୁ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ସ୍ଥିର CURLcode cf_capsule_cntrl(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2)
{
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)arg1;
  (ଶୂନ୍ୟ)arg2;
  ବାଛ(event) {
  ସ୍ଥିତି CF_CTRL_FLUSH:
    result = cf_capsule_flush(cf, data);
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_capsule_query(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2)
{
  ସଂରଚନା cf_capsule_ctx *ctx = cf->ctx;

  (ଶୂନ୍ୟ)pres2;
  ବାଛ(query) {
  ସ୍ଥିତି CF_QUERY_NEED_FLUSH: {
    ଯଦି(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଭାଙ୍ଗ;
  }
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ସ୍ଥିର CURLcode cf_capsule_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                          ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା cf_capsule_ctx *ctx = cf->ctx;

  ଯଦି(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    ଯଦି(sock != CURL_SOCKET_BAD)
      ଫେରନ୍ତୁ Curl_pollset_add_out(data, ps, sock);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_capsule_shutdown(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;

  ଯଦି(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  ଅନ୍ୟଥା {
    result = cf_capsule_flush(cf, data);
    *done = !result;
    ଯଦି(result == CURLE_AGAIN)
      result = CURLE_OK;
  }
  ଫେରନ୍ତୁ result;
}

ସଂରଚନା Curl_cftype Curl_cft_capsule = {
  "CAPSULE",
  0,
  0,
  cf_capsule_destroy,
  cf_capsule_connect,
  cf_capsule_shutdown,
  cf_capsule_adjust_pollset,
  cf_capsule_data_pending,
  cf_capsule_send,
  cf_capsule_recv,
  cf_capsule_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_capsule_query,
};

ସ୍ଥିର CURLcode cf_capsule_create(ସଂରଚନା Curl_cfilter **pcf,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  ସଂରଚନା cf_capsule_ctx *ctx;
  CURLcode result;

  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)conn;
  *pcf = ଖାଲି;
  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_SIZE, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_SIZE, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

out:
  *pcf = (!result) ? cf : ଖାଲି;
  ଯଦି(result && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cf_capsule_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                      ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result;

  result = cf_capsule_create(&cf, data, cf_at->conn);
  ଯଦି(!result)
    Curl_conn_cf_ଭର୍ତ୍ତି_after(cf_at, cf);
  ଫେରନ୍ତୁ result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
