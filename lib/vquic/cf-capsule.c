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
#include "curl_setup.h"

#nếu !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#include "urldata.h"
#include "cfilters.h"
#include "curl_trc.h"
#include "bufq.h"
#include "select.h"
#include "vquic/capsule.h"
#include "vquic/cf-capsule.h"

/* send/recv buffer: 4 chunks of 16KB = 64KB, enough for large datagrams */
#define CAPSULE_RECV_CHUNKS    4
#define CAPSULE_SEND_CHUNKS    4
#define CAPSULE_CHUNK_SIZE     (16 * 1024)

struct cf_capsule_ctx {
  struct bufq recvbuf;
  struct bufq sendbuf;
};

static void cf_capsule_destroy(struct Curl_cfilter *cf,
                               struct Curl_easy *data)
{
  struct cf_capsule_ctx *ctx = cf->ctx;
  (void)data;
  nếu(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

static CURLcode cf_capsule_connect(struct Curl_cfilter *cf,
                                   struct Curl_easy *data,
                                   bool *done)
{
  nếu(cf->connected) {
    *done = TRUE;
    trả_về CURLE_OK;
  }
  nếu(cf->next) {
    CURLcode result = cf->next->cft->do_connect(cf->next, data, done);
    nếu(!result && *done)
      cf->connected = TRUE;
    trả_về result;
  }
  *done = FALSE;
  trả_về CURLE_OK;
}

static CURLcode cf_capsule_flush(struct Curl_cfilter *cf,
                                 struct Curl_easy *data)
{
  struct cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  nếu(Curl_bufq_is_empty(&ctx->sendbuf))
    trả_về CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->sendbuf, NULL, 0,
                             &nwritten);
  nếu(result) {
    nếu(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    trả_về result;
  }
  trả_về Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

static CURLcode cf_capsule_send(struct Curl_cfilter *cf,
                                struct Curl_easy *data,
                                const uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  struct cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;

  (void)eos;
  *pnwritten = 0;

  nếu(Curl_bufq_is_full(&ctx->sendbuf)) {
    result = cf_capsule_flush(cf, data);
    nếu(result)
      trả_về result;
  }

  /* encapsulate new payload into a capsule */
  result = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  nếu(result)
    trả_về result;

  result = cf_capsule_flush(cf, data);
  nếu(result == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    result = CURLE_OK;
  }

  nếu(!result)
    *pnwritten = len;
  trả_về result;
}

static CURLcode cf_capsule_recv(struct Curl_cfilter *cf,
                                struct Curl_easy *data,
                                char *buf, size_t len,
                                size_t *pnread)
{
  struct cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  trong_khi(!Curl_bufq_is_full(&ctx->recvbuf)) {
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->recvbuf, 0, &nread);
    nếu(result == CURLE_AGAIN)
      break;
    nếu(result)
      trả_về result;
    nếu(!nread)
      break;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, data, &ctx->recvbuf,
                                         (unsigned char *)buf, len,
                                         &result);
  trả_về result;
}

static bool cf_capsule_data_pending(struct Curl_cfilter *cf,
                                    const struct Curl_easy *data)
{
  struct cf_capsule_ctx *ctx = cf->ctx;

  nếu(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    trả_về TRUE;
  trả_về cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

static CURLcode cf_capsule_cntrl(struct Curl_cfilter *cf,
                                 struct Curl_easy *data,
                                 số_nguyên event, số_nguyên arg1, void *arg2)
{
  CURLcode result = CURLE_OK;

  (void)arg1;
  (void)arg2;
  switch(event) {
  case CF_CTRL_FLUSH:
    result = cf_capsule_flush(cf, data);
    break;
  default:
    break;
  }
  trả_về result;
}

static CURLcode cf_capsule_query(struct Curl_cfilter *cf,
                                 struct Curl_easy *data,
                                 số_nguyên query, số_nguyên *pres1, void *pres2)
{
  struct cf_capsule_ctx *ctx = cf->ctx;

  (void)pres2;
  switch(query) {
  case CF_QUERY_NEED_FLUSH: {
    nếu(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      trả_về CURLE_OK;
    }
    break;
  }
  default:
    break;
  }
  trả_về cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static CURLcode cf_capsule_adjust_pollset(struct Curl_cfilter *cf,
                                          struct Curl_easy *data,
                                          struct easy_pollset *ps)
{
  struct cf_capsule_ctx *ctx = cf->ctx;

  nếu(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    nếu(sock != CURL_SOCKET_BAD)
      trả_về Curl_pollset_add_out(data, ps, sock);
  }
  trả_về CURLE_OK;
}

static CURLcode cf_capsule_shutdown(struct Curl_cfilter *cf,
                                    struct Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;

  nếu(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  không_thì {
    result = cf_capsule_flush(cf, data);
    *done = !result;
    nếu(result == CURLE_AGAIN)
      result = CURLE_OK;
  }
  trả_về result;
}

struct Curl_cftype Curl_cft_capsule = {
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

static CURLcode cf_capsule_create(struct Curl_cfilter **pcf,
                                  struct Curl_easy *data,
                                  struct connectdata *conn)
{
  struct Curl_cfilter *cf = NULL;
  struct cf_capsule_ctx *ctx;
  CURLcode result;

  (void)data;
  (void)conn;
  *pcf = NULL;
  ctx = curlx_calloc(1, sizeof(*ctx));
  nếu(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  Curl_bufq_init2(&ctx->recvbuf, CAPSULE_CHUNK_SIZE, CAPSULE_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  Curl_bufq_init2(&ctx->sendbuf, CAPSULE_CHUNK_SIZE, CAPSULE_SEND_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);

  result = Curl_cf_create(&cf, &Curl_cft_capsule, ctx);

out:
  *pcf = (!result) ? cf : NULL;
  nếu(result && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  trả_về result;
}

CURLcode Curl_cf_capsule_insert_after(struct Curl_cfilter *cf_at,
                                      struct Curl_easy *data)
{
  struct Curl_cfilter *cf;
  CURLcode result;

  result = cf_capsule_create(&cf, data, cf_at->conn);
  nếu(!result)
    Curl_conn_cf_insert_after(cf_at, cf);
  trả_về result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
