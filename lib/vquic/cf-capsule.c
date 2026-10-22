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

#གལ་ཏེ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

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

sa_nracanaa cf_capsule_ctx {
  sa_nracanaa bufq recvbuf;
  sa_nracanaa bufq sendbuf;
};

static void cf_capsule_destroy(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_capsule_ctx *ctx = cf->ctx;
  (void)data;
  གལ་ཏེ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

static CURLcode cf_capsule_connect(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   bool *done)
{
  གལ་ཏེ(cf->connected) {
    *done = TRUE;
    ཕྱིར CURLE_OK;
  }
  གལ་ཏེ(cf->next) {
    CURLcode result = cf->next->cft->do_connect(cf->next, data, done);
    གལ་ཏེ(!result && *done)
      cf->connected = TRUE;
    ཕྱིར result;
  }
  *done = FALSE;
  ཕྱིར CURLE_OK;
}

static CURLcode cf_capsule_flush(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  གལ་ཏེ(Curl_bufq_is_empty(&ctx->sendbuf))
    ཕྱིར CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->sendbuf, NULL, 0,
                             &nwritten);
  གལ་ཏེ(result) {
    གལ་ཏེ(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    ཕྱིར result;
  }
  ཕྱིར Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
}

static CURLcode cf_capsule_send(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                const uint8_t *buf, size_t len,
                                bool eos, size_t *pnwritten)
{
  sa_nracanaa cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;

  (void)eos;
  *pnwritten = 0;

  གལ་ཏེ(Curl_bufq_is_full(&ctx->sendbuf)) {
    result = cf_capsule_flush(cf, data);
    གལ་ཏེ(result)
      ཕྱིར result;
  }

  /* encapsulate new payload into a capsule */
  result = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  གལ་ཏེ(result)
    ཕྱིར result;

  result = cf_capsule_flush(cf, data);
  གལ་ཏེ(result == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    result = CURLE_OK;
  }

  གལ་ཏེ(!result)
    *pnwritten = len;
  ཕྱིར result;
}

static CURLcode cf_capsule_recv(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                var_na *buf, size_t len,
                                size_t *pnread)
{
  sa_nracanaa cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result;
  size_t nread;

  *pnread = 0;

  /* fill our receive buffer from the filter below */
  སྐབས(!Curl_bufq_is_full(&ctx->recvbuf)) {
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->recvbuf, 0, &nread);
    གལ་ཏེ(result == CURLE_AGAIN)
      bha_nga;
    གལ་ཏེ(result)
      ཕྱིར result;
    གལ་ཏེ(!nread)
      bha_nga;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, data, &ctx->recvbuf,
                                         (unsigned var_na *)buf, len,
                                         &result);
  ཕྱིར result;
}

static bool cf_capsule_data_pending(sa_nracanaa Curl_cfilter *cf,
                                    const sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_capsule_ctx *ctx = cf->ctx;

  གལ་ཏེ(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    ཕྱིར TRUE;
  ཕྱིར cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

static CURLcode cf_capsule_cntrl(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 ཧྲིལ event, ཧྲིལ arg1, void *arg2)
{
  CURLcode result = CURLE_OK;

  (void)arg1;
  (void)arg2;
  switch(event) {
  pak_sa CF_CTRL_FLUSH:
    result = cf_capsule_flush(cf, data);
    bha_nga;
  default:
    bha_nga;
  }
  ཕྱིར result;
}

static CURLcode cf_capsule_query(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 ཧྲིལ query, ཧྲིལ *pres1, void *pres2)
{
  sa_nracanaa cf_capsule_ctx *ctx = cf->ctx;

  (void)pres2;
  switch(query) {
  pak_sa CF_QUERY_NEED_FLUSH: {
    གལ་ཏེ(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      ཕྱིར CURLE_OK;
    }
    bha_nga;
  }
  default:
    bha_nga;
  }
  ཕྱིར cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static CURLcode cf_capsule_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                          sa_nracanaa Curl_easy *data,
                                          sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa cf_capsule_ctx *ctx = cf->ctx;

  གལ་ཏེ(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    གལ་ཏེ(sock != CURL_SOCKET_BAD)
      ཕྱིར Curl_pollset_add_out(data, ps, sock);
  }
  ཕྱིར CURLE_OK;
}

static CURLcode cf_capsule_shutdown(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;

  གལ་ཏེ(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  དེ་མིན {
    result = cf_capsule_flush(cf, data);
    *done = !result;
    གལ་ཏེ(result == CURLE_AGAIN)
      result = CURLE_OK;
  }
  ཕྱིར result;
}

sa_nracanaa Curl_cftype Curl_cft_capsule = {
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

static CURLcode cf_capsule_create(sa_nracanaa Curl_cfilter **pcf,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn)
{
  sa_nracanaa Curl_cfilter *cf = NULL;
  sa_nracanaa cf_capsule_ctx *ctx;
  CURLcode result;

  (void)data;
  (void)conn;
  *pcf = NULL;
  ctx = curlx_calloc(1, parimaa_na(*ctx));
  གལ་ཏེ(!ctx) {
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
  གལ་ཏེ(result && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  ཕྱིར result;
}

CURLcode Curl_cf_capsule_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                      sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;

  result = cf_capsule_create(&cf, data, cf_at->conn);
  གལ་ཏེ(!result)
    Curl_conn_cf_insert_after(cf_at, cf);
  ཕྱིར result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
