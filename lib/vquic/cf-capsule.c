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

#ᱡᱩᱫᱤ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

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
  ᱡᱩᱫᱤ(ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_safefree(ctx);
  }
}

static CURLcode cf_capsule_connect(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   bool *done)
{
  ᱡᱩᱫᱤ(cf->connected) {
    *done = TRUE;
    ᱨᱩᱣᱟᱹᱲ CURLE_OK;
  }
  ᱡᱩᱫᱤ(cf->next) {
    CURLcode result = cf->next->cft->do_connect(cf->next, data, done);
    ᱡᱩᱫᱤ(!result && *done)
      cf->connected = TRUE;
    ᱨᱩᱣᱟᱹᱲ result;
  }
  *done = FALSE;
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static CURLcode cf_capsule_flush(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_capsule_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nwritten;

  ᱡᱩᱫᱤ(Curl_bufq_is_empty(&ctx->sendbuf))
    ᱨᱩᱣᱟᱹᱲ CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->sendbuf, NULL, 0,
                             &nwritten);
  ᱡᱩᱫᱤ(result) {
    ᱡᱩᱫᱤ(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->sendbuf));
    }
    ᱨᱩᱣᱟᱹᱲ result;
  }
  ᱨᱩᱣᱟᱹᱲ Curl_bufq_is_empty(&ctx->sendbuf) ? CURLE_OK : CURLE_AGAIN;
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

  ᱡᱩᱫᱤ(Curl_bufq_is_full(&ctx->sendbuf)) {
    result = cf_capsule_flush(cf, data);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
  }

  /* encapsulate new payload into a capsule */
  result = Curl_capsule_encap_udp_datagram(&ctx->sendbuf, buf, len);
  ᱡᱩᱫᱤ(result)
    ᱨᱩᱣᱟᱹᱲ result;

  result = cf_capsule_flush(cf, data);
  ᱡᱩᱫᱤ(result == CURLE_AGAIN) {
    /* Could not send it (or all), report success nevertheless as we
     * have the payload buffered now and will flush it later. */
    result = CURLE_OK;
  }

  ᱡᱩᱫᱤ(!result)
    *pnwritten = len;
  ᱨᱩᱣᱟᱹᱲ result;
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
  ᱡᱚᱠᱷᱚᱱ(!Curl_bufq_is_full(&ctx->recvbuf)) {
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->recvbuf, 0, &nread);
    ᱡᱩᱫᱤ(result == CURLE_AGAIN)
      bha_nga;
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
    ᱡᱩᱫᱤ(!nread)
      bha_nga;
  }

  /* try to extract a complete capsule datagram */
  *pnread = Curl_capsule_process_udp_raw(cf, data, &ctx->recvbuf,
                                         (unsigned var_na *)buf, len,
                                         &result);
  ᱨᱩᱣᱟᱹᱲ result;
}

static bool cf_capsule_data_pending(sa_nracanaa Curl_cfilter *cf,
                                    const sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_capsule_ctx *ctx = cf->ctx;

  ᱡᱩᱫᱤ(ctx && !Curl_bufq_is_empty(&ctx->recvbuf))
    ᱨᱩᱣᱟᱹᱲ TRUE;
  ᱨᱩᱣᱟᱹᱲ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

static CURLcode cf_capsule_cntrl(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 puur_naa_nka event, puur_naa_nka arg1, void *arg2)
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
  ᱨᱩᱣᱟᱹᱲ result;
}

static CURLcode cf_capsule_query(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 puur_naa_nka query, puur_naa_nka *pres1, void *pres2)
{
  sa_nracanaa cf_capsule_ctx *ctx = cf->ctx;

  (void)pres2;
  switch(query) {
  pak_sa CF_QUERY_NEED_FLUSH: {
    ᱡᱩᱫᱤ(!Curl_bufq_is_empty(&ctx->sendbuf)) {
      *pres1 = TRUE;
      ᱨᱩᱣᱟᱹᱲ CURLE_OK;
    }
    bha_nga;
  }
  default:
    bha_nga;
  }
  ᱨᱩᱣᱟᱹᱲ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static CURLcode cf_capsule_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                          sa_nracanaa Curl_easy *data,
                                          sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa cf_capsule_ctx *ctx = cf->ctx;

  ᱡᱩᱫᱤ(!Curl_bufq_is_empty(&ctx->sendbuf)) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf, data);
    ᱡᱩᱫᱤ(sock != CURL_SOCKET_BAD)
      ᱨᱩᱣᱟᱹᱲ Curl_pollset_add_out(data, ps, sock);
  }
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static CURLcode cf_capsule_shutdown(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_OK;

  ᱡᱩᱫᱤ(!cf->connected || cf->shutdown) {
    *done = TRUE;
  }
  ᱵᱟᱝ {
    result = cf_capsule_flush(cf, data);
    *done = !result;
    ᱡᱩᱫᱤ(result == CURLE_AGAIN)
      result = CURLE_OK;
  }
  ᱨᱩᱣᱟᱹᱲ result;
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
  ᱡᱩᱫᱤ(!ctx) {
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
  ᱡᱩᱫᱤ(result && ctx) {
    Curl_bufq_free(&ctx->recvbuf);
    Curl_bufq_free(&ctx->sendbuf);
    curlx_free(ctx);
  }
  ᱨᱩᱣᱟᱹᱲ result;
}

CURLcode Curl_cf_capsule_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                      sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;

  result = cf_capsule_create(&cf, data, cf_at->conn);
  ᱡᱩᱫᱤ(!result)
    Curl_conn_cf_insert_after(cf_at, cf);
  ᱨᱩᱣᱟᱹᱲ result;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
